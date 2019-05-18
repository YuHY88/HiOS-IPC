/*
*          receive and send mpls packet
*/

#include <lib/ether.h>
#include <lib/pkt_buffer.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/log.h>
#include <lib/memory.h>
#include "ifm/ifm.h"
#include "ftm_ifm.h"
#include "ftm_ilm.h"
#include "ftm_nhlfe.h"
#include "ftm_pkt.h"
#include "ftm_debug.h"
#include "pkt_eth.h"
#include "pkt_ip.h"
#include "pkt_mpls.h"
#include "proto_reg.h"
#include "ftm_arp.h"
#include "ftm.h"
#include "ftm_nhp.h"
#include "pkt_netif.h"


static struct mpls_label * mpls_decap_label(struct pkt_buffer *pkt);
static int mpls_encap_label(struct pkt_buffer *pkt, uint32_t label);
static struct mpls_arch * mpls_decap_arch(struct pkt_buffer *pkt);
static int mpls_encap_arch(struct pkt_buffer *pkt, struct mpls_arch *parch);


/* 处理带控制字的协议报文，报文已经剥掉标签和控制字，只剩 payload */
int mpls_rcv_local(struct pkt_buffer *pkt)
{
	struct mpls_proto    proto; 	
	struct mpls_control *pmpls_cb = NULL;	
	char buf[4] = {0,0,0,0};
	uint8_t oam_pkt_type = 0;
	int module_id;

	pmpls_cb = (struct mpls_control *)(&(pkt->cb));	
	/* 根据 chtype 确定报文类型 */
	if(pmpls_cb->chtype == MPLS_CTLWORD_CHTYPE_IPV4) /* payload 是 ipv4 报文 */
	{
		return ip_rcv(pkt);
	}	
	else if(pmpls_cb->chtype == MPLS_CTLWORD_CHTYPE_IPV6)/* ipv6 报文 */
	{
		goto drop;
	}
	else if(pmpls_cb->chtype == MPLS_CTLWORD_CHTYPE_8902 || pmpls_cb->chtype == MPLS_CTLWORD_CHTYPE_7FFA)  /* mpls-tp oam 报文 */
	{
		memcpy(buf, pkt->data ,4);
		oam_pkt_type = (uint8_t)buf[1];
		pmpls_cb->ttl = 1;
		goto out;
	}
	else if(pmpls_cb->chtype == MPLS_CTLWORD_CHTYPE_APS) /* mpls-tp aps 报文 */
	{
		goto out;
	}
	else if(MPLS_CTLWORD_WITHOUT_IP == pmpls_cb->chtype)
	{
		goto out;
	}
	else
	{
		goto drop;
	}

out:	
	/*find the registered proto node*/	
	pkt->cb_type = PKT_TYPE_MPLS;
	
	memset(&proto, 0, sizeof(struct mpls_proto));
    proto.chtype = pmpls_cb->chtype; /* 取 chtype 字段 */
	proto.oam_opcode = oam_pkt_type;
	proto.ttl = pmpls_cb->ttl;
	module_id = mpls_proto_lookup(&proto);
	if(module_id)
	{
		//ftm_pkt_send_to(pkt, module_id);/* send to app */
		ftm_pkt_send_to_n(pkt, module_id);/* send to app */
		pkt_free(pkt);
	    return 0;
	}
	
drop:	
	zlog_err("mpls_rcv_local failed, drop it!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_MPLS);
	pkt_free(pkt);
	return NET_FAILED;
}


/* pw 终结的报文 */
int mpls_rcv_pw(struct pkt_buffer *pkt)
{
	struct mpls_arch  *parch = NULL;
	struct mpls_control *mpls_cb = NULL;
	struct pw_info *ppw = NULL;
	struct iphdr *iph = NULL;

	ppw = ftm_pw_lookup(pkt->in_ifindex);
	if(ppw == NULL)
	{
		goto drop;
	}

	mpls_cb = (struct mpls_control *)(&(pkt->cb));

	/* 带控制字的报文走本地接收 */
	parch = mpls_decap_arch(pkt);
	if(NULL != parch)
	{	
		mpls_cb->chtype = ntohs(parch->chtype); 				
		return mpls_rcv_local(pkt);
	}
	else
	{
		
		pkt->network_header = pkt->data;  /* set ip header pointer */
		iph = (struct iphdr *)(pkt->network_header); /* get ip header */
		if(ntohl(iph->daddr) == inet_strtoipv4((char *)"127.0.0.1"))
		{
			/*mpls ping  pw  不带控制字*/
			return ip_rcv(pkt);
		}

		pkt->inif_type = PKT_INIF_TYPE_IF;
		pkt->in_ifindex = ppw->ifindex;    //change to ac port
		//pkt->in_port = IFM_PARENT_IFINDEX_GET(pkt->in_ifindex);
		return eth_rcv(pkt); /* 不带控制字的报文走以太转发*/
	}

    return NET_SUCCESS;

drop:
	FTM_LOG_DEBUG("mpls_rcv failed, drop it!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_MPLS);
	pkt_free(pkt);
	return NET_FAILED;
}


/* lsp 终结的报文 */
int mpls_rcv_lsp(struct pkt_buffer *pkt)
{
	struct mpls_arch  *parch = NULL;
	struct mpls_control *mpls_cb = NULL;
	mpls_cb = (struct mpls_control *)(&(pkt->cb));

	/* 带控制字的报文走本地接收 */
	parch = mpls_decap_arch(pkt);
	if(NULL != parch)
	{	
		mpls_cb->chtype = ntohs(parch->chtype);
		return mpls_rcv_local(pkt);
	}
	else
	{
		return ip_rcv(pkt); /* 不带控制字的报文走 ip 转发*/
	}

	return 0;
}


/* 接收 mpls 报文，剥掉标签，生成 mpls 层控制头 */
int mpls_rcv(struct pkt_buffer *pkt)
{
	struct mpls_label *plabel = NULL;
	struct mpls_arch  *parch = NULL;
	struct ilm_entry  *pilm = NULL;
	struct mpls_control *mpls_cb = NULL;
	struct pw_info *ppw = NULL;
	uint32_t label = 0;
	int bos = 0;

	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_RECV, PKT_TYPE_MPLS);
	ftm_pkt_cnt_add(PKT_ACTION_RECV, PKT_TYPE_MPLS);
	
	mpls_cb = (struct mpls_control *)(&(pkt->cb));
	memset(mpls_cb, 0, PKT_CB_SIZE);
	pkt->cb_type = PKT_TYPE_MPLS;

	/* 逐层解析标签，直到栈底 */
	while(bos == 0)   /* 非栈底 */
	{
		plabel = mpls_decap_label(pkt);
		label = ntohs ( plabel->label1 );
		label = (label << 4) + plabel->label0;
		
		mpls_cb->label_num ++;
		mpls_cb->label = label;
		
		pilm = ftm_ilm_lookup(label);
		if (pilm == NULL)
		{
			goto drop;
		}
		if(pilm->lsp_type == LSP_TYPE_L3VPN)
		{
			pkt->inif_type = PKT_INIF_TYPE_LSP;
			pkt->in_ifindex = pilm->lsp_index;
			pkt->vpn = pilm->vpnid;
			pkt->priority = plabel->exp;
		}
		/* 报文的入接口改成 lsp 或 PW，不再是物理接口 */
		if((pilm->lsp_type == LSP_TYPE_LDP) ||
			(pilm->lsp_type == LSP_TYPE_STATIC) || (pilm->lsp_type == LSP_TYPE_RSVPTE)) 
		{
			pkt->inif_type = PKT_INIF_TYPE_LSP;
			pkt->in_ifindex = pilm->lsp_index;
			pkt->vpn = 0;
			pkt->priority = plabel->exp;
		}
		else if(pilm->lsp_type == LSP_TYPE_L2VC)
		{
			pkt->inif_type = PKT_INIF_TYPE_PW;
			pkt->in_ifindex = pilm->pw_index;
			pkt->vpn = 0;			
			pkt->priority = plabel->exp;
		}
		else if(pilm->lsp_type == LSP_TYPE_RESERVED) /* 保留标签 1-15 */
		{
			if((pkt->inif_type != PKT_INIF_TYPE_PW) && (pkt->inif_type != PKT_INIF_TYPE_LSP)) /* lsp 倒数第二跳弹出 */
			{
				pkt->inif_type = PKT_INIF_TYPE_LSP;
				pkt->in_ifindex = pilm->lsp_index;				
				pkt->vpn = 0;
				pkt->priority = plabel->exp;
			}
		}
		else
		{
			goto drop;
		}

		/* 生成 mpls_cb 信息 */
		bos = plabel->bos;
		mpls_cb->exp = plabel->exp;
		mpls_cb->ttl = plabel->ttl;
		switch (pilm->action)
		{
 			case LABEL_ACTION_SWAP:  /* transit 节点 */
			{
				if(plabel->ttl <= 1) /* 本地接收 */
				{
					if( bos == 1)
					{
						parch = mpls_decap_arch(pkt);/* 解析报文中的控制字 */
						if(NULL != parch)
						{	
							mpls_cb->chtype = ntohs(parch->chtype); 				
							return mpls_rcv_local(pkt);
						}
						else
							return ip_rcv(pkt);  /* 不带控制字的报文为lsp报文 */
					}
					else
					{
						break;
					}
				
				}
				else  /* 正常转发 */
				{
					if((pilm->nhp_type == NHP_TYPE_ECMP) || (pilm->nhp_type == NHP_TYPE_FRR))
						return mpls_forward_ecmp(pkt, pilm->nhp_index, pilm->nhp_type);
					else if(pilm->nhp_type == NHP_TYPE_LSP)
					{
						mpls_cb->ifindex = pilm->lsp_index; /* 可能是个 ecmp 或 frr */
						mpls_cb->if_type = PKT_INIF_TYPE_LSP;

						return mpls_forward(pkt);
					}
				}
				
				break;
			}
			case LABEL_ACTION_POP:		/* egress 节点 */		
			case LABEL_ACTION_POPGO:
			{
				if(pkt->inif_type == PKT_INIF_TYPE_PW) /* PW 终结报文 */
				{
					parch = (struct mpls_arch *)(pkt->data);
					ppw = ftm_pw_lookup(pkt->in_ifindex);
					if((NULL != ppw)&&(ppw->ctrlword_flag)&&(ppw->mspw_index == 0))
					{
						if(parch->flag != MPLS_CTLWORD_FLAG)
						{
							pkt_pull(pkt, MPLS_ARCH_LEN);
						}
					}
					return mpls_rcv_pw(pkt);
				}
				else if(pkt->inif_type == PKT_INIF_TYPE_LSP) /* lsp 终结报文 */
				{
				    if(bos == 1)
						return mpls_rcv_lsp(pkt);
				}
				else
				{
					goto drop;
				}
				break;   /* 继续 pop 标签 */
			}
			default: 
				goto drop;
		}		
	}

drop:
	FTM_LOG_DEBUG("mpls_rcv failed, drop it!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_MPLS);
	pkt_free(pkt);
	return NET_FAILED;
}


/* 多下一跳转发 */
int mpls_forward_ecmp(struct pkt_buffer *pkt, uint32_t nhp_index, enum NHP_TYPE nhp_type)
{
	struct nhp_info *pinfo = NULL;
	struct mpls_control *mpls_cb = NULL;
	int i;

	mpls_cb = (struct mpls_control *)(&(pkt->cb));/* 一定是 mpls 报文 */

	/* 处理多下一跳的情况 */
    switch (nhp_type)
	{
		case NHP_TYPE_FRR:
		{
			struct ecmp_group *frr = ftm_ecmp_lookup(nhp_index);
		    if(frr == NULL)
			{
				goto drop;
			}				

	        i = frr->frr_flag;
			pinfo = &(frr->nhp[i]);
			break;
		}
		case NHP_TYPE_ECMP:
		{
			struct ecmp_group *ecmp = ftm_ecmp_lookup(pinfo->nhp_index);
		    if(!ecmp)
			{
				zlog_err("%s, %d ftm_ecmp_lookup fail! pinfo->nhp_index:0x%x\n",__FUNCTION__, __LINE__,pinfo->nhp_index);
				goto drop;
			}
			
	        for (i=0; i<NHP_ECMP_NUM; i++)
	        {
	           if(ecmp->nhp[i].nhp_type)
	           {
	              break;
	           }
	        }
			if(i == NHP_ECMP_NUM)
			{
				zlog_err("%s, %d nhp lookup fail!\n",__FUNCTION__, __LINE__);
				goto drop;
			}
			
			pinfo = &(ecmp->nhp[i]);
			break;
		}
		default: /* 单下一跳 */
		{
			goto drop;
		}
    }

	if(nhp_type == NHP_TYPE_LSP) /* 转发到 LSP */
	{
		mpls_cb->ifindex = pinfo->nhp_index; /* 出接口是 lsp */
		mpls_cb->if_type = NHP_TYPE_LSP;  /* 接口类型是 lsp */
		mpls_cb->is_changed = 1;
		return mpls_forward(pkt);
	}
	else if(nhp_type == NHP_TYPE_TUNNEL)
	{
		pkt->out_ifindex = pinfo->nhp_index;
		pkt->priority = mpls_cb->exp;		
		pkt->protocol = ETH_P_MPLS_UC;
		return tunnel_xmit(pkt);
	}

drop:
	FTM_LOG_DEBUG("mpls_forward_ecmp failed, drop it!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_MPLS);
	pkt_free(pkt);
	return NET_FAILED;
}


/* 根据 mpls_cb，报文转发到 PW 或 LSP */
int mpls_forward(struct pkt_buffer *pkt)
{
	struct mpls_control *mpls_cb = NULL;	
	struct eth_control  *ethcb = NULL;
	struct mpls_label mplslabel;	
	struct mpls_arch mplsparch;	
	struct pw_info *ppw = NULL;
	struct nhlfe_entry *pnhlfe = NULL;

	memset(&mplslabel, 0, sizeof(struct mpls_label));
	mpls_cb = (struct mpls_control *)(&(pkt->cb));
	
	if(mpls_cb->if_type == PKT_INIF_TYPE_PW) 	 /* 发送到 pw */
	{
		ppw = ftm_pw_lookup(mpls_cb->ifindex);
		if(ppw == NULL)
		{
			FTM_LOG_DEBUG("PKT_INIF_TYPE_PW1 ftm_pw_lookup fail,drop it!\n");
			goto drop;
		}
		
		if (ppw->mspw_index != 0)
		{
			ppw = ftm_pw_lookup(ppw->mspw_index);
			if(ppw == NULL)
			{
			 	FTM_LOG_DEBUG("PKT_INIF_TYPE_PW2 ftm_pw_lookup fail,drop it!\n");
				goto drop;
			}
		}

		/* 封装控制字*/	
		if((mpls_cb->chtype == 0)&&(ppw->ctrlword_flag) && (ppw->mspw_index == 0))
		{
			 memset(&mplsparch, 0, MPLS_ARCH_LEN);
			 if(mpls_encap_arch(pkt, &mplsparch))
				 goto drop;
		}
		else if(mpls_cb->chtype)
		{
			 memset(&mplsparch, 0, MPLS_ARCH_LEN);
			 mplsparch.chtype = htons(mpls_cb->chtype);
			 mplsparch.flag = 1;
			 if(mpls_encap_arch(pkt, &mplsparch))
				 goto drop;
		 }	
		
		mpls_cb->ifindex = ppw->ingress_lsp; /* 走私网lsp 转发*/
		mpls_cb->if_type = PKT_INIF_TYPE_LSP;
		return mpls_forward(pkt);
	}
	else if(mpls_cb->if_type == PKT_INIF_TYPE_LSP) /* 发送到 lsp */
	{
		pnhlfe = ftm_nhlfe_lookup(mpls_cb->ifindex);
		if(pnhlfe == NULL)
		{
			FTM_LOG_DEBUG("PKT_INIF_TYPE_LSP ftm_nhlfe_lookup fail,drop it!\n");
			goto drop;
		}

		/* 封装标签*/
		if(pnhlfe->outlabel)
		{
			if(mpls_encap_label(pkt, pnhlfe->outlabel))
			{
				goto drop;
			}
		}

		/* 看下一级转发信息 */
		mpls_cb->ifindex = pnhlfe->nhp_index;
		mpls_cb->nexthop = pnhlfe->nexthop;
		if(pnhlfe->nhp_type == NHP_TYPE_TUNNEL)
		{
			mpls_cb->if_type = PKT_INIF_TYPE_TUNNEL;			
			pkt->out_ifindex = mpls_cb->ifindex;
			pkt->priority = mpls_cb->exp;
			pkt->protocol = ETH_P_MPLS_UC;
			return tunnel_xmit(pkt);
		}
		else if(pnhlfe->nhp_type == NHP_TYPE_LSP)
		{
			mpls_cb->if_type = PKT_INIF_TYPE_LSP;
			return mpls_forward(pkt);
		}
		else if((pnhlfe->nhp_type == NHP_TYPE_ECMP) || (pnhlfe->nhp_type == NHP_TYPE_FRR))
		{
			return mpls_forward_ecmp(pkt, pnhlfe->nhp_index, pnhlfe->nhp_type);
		}
		else if(pnhlfe->nhp_type == NHP_TYPE_CONNECT)
		{
			if(pnhlfe->nexthop)
				return mpls_output(pkt);
			else          /* mac 方式 */
			{
				/* 设置链路层封装信息 */
				ethcb = (struct eth_control *)(&(pkt->cb));
				memset(ethcb, 0, PKT_CB_SIZE);
				pkt->cb_type = PKT_TYPE_ETH;
				ethcb->ethtype = ETH_P_MPLS_UC;
				ethcb->cos = pkt->priority;
				ethcb->ifindex = pnhlfe->nhp_index;
				memcpy((uint8_t *)ethcb->dmac, (uint8_t *)pnhlfe->dmac, 6);	
				ethcb->is_changed = 1;
				pkt->out_ifindex = ethcb->ifindex;
				return eth_forward(pkt);
		    }
		}
    }
	
drop:
	FTM_LOG_DEBUG("mpls_forward get a error packet,drop it!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_MPLS);
	pkt_free(pkt);
	return NET_FAILED;
}


/*
查 ARP 发送 mpls 报文
*/
int mpls_output(struct pkt_buffer *pkt)
{
	struct mpls_control *mpls_cb = NULL;	
	struct eth_control *ethcb = NULL;
	struct arp_entry *parp = NULL;

	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_SEND, PKT_TYPE_MPLS);
	ftm_pkt_cnt_add(PKT_ACTION_SEND, PKT_TYPE_MPLS);
	
	mpls_cb = (struct mpls_control *)(&(pkt->cb));	
	
	if(mpls_cb->nexthop == 0)
		goto drop;
	
	parp = arp_lookup_active(mpls_cb->nexthop, pkt->vpn);
	if(!parp) 
	{
		FTM_LOG_DEBUG("mpls_output arp_lookup_active failed, drop the packet!\n");
		goto drop;
	}			
		
	/* 设置链路层封装信息 */
	ethcb = (struct eth_control *)(&(pkt->cb));
	memset(ethcb, 0, PKT_CB_SIZE);
	pkt->cb_type = PKT_TYPE_ETH;
	ethcb->ethtype = ETH_P_MPLS_UC;
	ethcb->cos = pkt->priority;
	ethcb->ifindex = parp->ifindex;
	memcpy((char *)ethcb->dmac, (char *)parp->mac, 6);	
	ethcb->is_changed = 1;
	return eth_forward(pkt);

drop:	
	FTM_LOG_DEBUG("mpls_output failed, drop the packet!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_MPLS);
	pkt_free(pkt);
	return NET_FAILED;
}


/* add label to payload */
static int mpls_encap_label(struct pkt_buffer *pkt, uint32_t label)
{
	struct mpls_label   *plabel = NULL;
	struct mpls_control *mpls_cb = NULL;
	
	mpls_cb = (struct mpls_control *)(&(pkt->cb));
	if(mpls_cb->label_num >= MPLS_LABEL_NUM) /* 超过最大标签数*/
		return -1;

    pkt_push(pkt, MPLS_LABEL_LEN);  /* get buffer for the label */
	plabel = (struct mpls_label *)(pkt->data);
	plabel->label1 = htons(label >> 4);  /* get high 16bit */
	plabel->label0 = label & 0xf;        /* get low 4bit */
	plabel->ttl = mpls_cb->ttl;
	plabel->exp = mpls_cb->exp;
	if(mpls_cb->label_num == 0)
		plabel->bos = 1;
	else
		plabel->bos = 0;
	mpls_cb->label_num ++;

    pkt->protocol = ETH_P_MPLS_UC;
	pkt->priority = plabel->exp;

	return 0;
}


/* 从报文中获取一层标签 */
static struct mpls_label * mpls_decap_label(struct pkt_buffer *pkt)
{
	struct mpls_label *plabel = NULL;

    plabel = (struct mpls_label *)(pkt->data);
    pkt_pull(pkt, MPLS_LABEL_LEN);  /* offset the label */
	
	return plabel;
}


/* add arch to payload */
static int mpls_encap_arch(struct pkt_buffer *pkt, struct mpls_arch *parch)
{
	if(pkt_push(pkt, MPLS_ARCH_LEN))	/* get buffer for the label */
		return -1;
	
	memcpy(pkt->data, parch, MPLS_ARCH_LEN);
	
	return 0;
}


/* 从报文中获取 arch 字段 */
static struct mpls_arch * mpls_decap_arch(struct pkt_buffer *pkt)
{
	struct mpls_arch *parch = NULL;

    parch = (struct mpls_arch *)(pkt->data);

	if(parch->flag == MPLS_CTLWORD_FLAG)  /* 识别 arch 头以 0001 开头*/
	{
	    if(pkt_pull(pkt, MPLS_ARCH_LEN))  /* offset the control word */
			return NULL;
		else
			return parch;
	}
	else
		return NULL;
}


