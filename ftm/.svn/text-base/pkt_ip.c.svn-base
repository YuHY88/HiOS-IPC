/*
*           process  ip packet forward
*/

#include <lib/checksum.h>
#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/ether.h>
#include <lib/inet_ip.h>
#include <lib/linklist.h>
#include <lib/zassert.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <string.h>
#include <mpls/mpls.h>
#include "memtypes.h"
#include "memory.h"
#include "ftm_ifm.h"
#include "ftm_nhp.h"
#include "ftm_fib.h"
#include "ftm_debug.h"
#include "pkt_ip.h"
#include "ftm_pkt.h"
#include "pkt_eth.h"
#include "pkt_icmp.h"
#include "pkt_tcp.h"
#include "pkt_udp.h"
#include "pkt_mpls.h"
#include "proto_reg.h"
#include "ftm_arp.h"
#include "ftm.h"
#include "ftm_lsp.h"
#include "ftm_tunnel.h"
#include "pkt_ipv6.h"
#include "pkt_netif.h"

/* 分片报文链表 */
struct ip_frag_list_s
{
	uint32_t    timeout;
	struct list fraglist;
	struct ip_frag_list_s *prev;
	struct ip_frag_list_s *next;
};


struct list ip_retry_list;     /* 重传报文链表 */
TIMERID ip_retry_timer = 0; /* 重传定时器 */

static int ip_frag_list_expire (struct thread *arg);
static struct ip_frag_list_s *pipfraglist; /* 分片报文链表，用于报文重组， 该指针始终指向头节点*/
static int g_ipfraglistnum;


/* local function */
int  ip_xmit_unicast(struct pkt_buffer *pkt); /* 查 arp 发送*/
int ip_xmit_broadcast(struct pkt_buffer *pkt);/*广播发包，暂时未使用*/
int ip_xmit_multicast(struct pkt_buffer *pkt);/*组播发包，暂时未使用*/
int  ip_encap(struct pkt_buffer *pkt); /* 封装 ip 头 */
int  ip_decap(struct pkt_buffer *pkt); /* 解封装 ip 头 */
int  gre_decap(struct pkt_buffer *pkt);
int  gre_encap(struct pkt_buffer *pkt);

/* 对分片报文进行重组，将一个报文的所有分片加入 fraglist，收集齐之后重组，返回重组后的报文 */
struct pkt_buffer *ip_defrag(struct pkt_buffer *pkt);
int ip_frag(struct ifm_info *p_ifinfo, struct pkt_buffer *pkt);/* 对超过 mtu 的报文进行分片 */


/*
接收 IP 报文，提取 IP 层信息生成 IP 控制头
*/
int ip_rcv(struct pkt_buffer *pkt)
{
	struct iphdr *iph = NULL;
	struct ip_control *ipcb = NULL;

	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_RECV, PKT_TYPE_IP);
	ftm_pkt_cnt_add(PKT_ACTION_RECV, PKT_TYPE_IP);

	/*analyze the ip header to get ipcb and iph */
	if (ip_decap(pkt))
	{
		zlog_debug(FTM_DBG_IP,"%s[%d] -> %s: ip_decap error, drop it!!\n", __FILE__, __LINE__, __func__);
		goto drop;
	}

	iph = (struct iphdr *)pkt->network_header;
	ipcb = (struct ip_control *)(&(pkt->cb));
	ipcb->pkt_type = PKT_TYPE_IP;

    /* 获取 vpn */
	ipcb->vpn = pkt->vpn;

	/* 检查源 IP */
	if((ipv4_is_broadcast(ipcb->sip))          /* SIP 是广播 */
		||(ipv4_is_multicast(ipcb->sip))       /* SIP 是组播 */
		||(ipv4_is_local_multicast(ipcb->sip)) /* SIP 是保留组播 */
		||(ipv4_is_loopback(ipcb->sip))        /* SIP 是 127 网段 */
		|| (ipcb->dip == 0)                    /* sip 是 0，dhcp client 报文的 sip = 0 */
	  )
	{
		zlog_debug(FTM_DBG_IP,"%s[%d] -> %s: sip format error, drop it!!\n", __FILE__, __LINE__, __func__);
		goto drop;
	}

	/* 检查目的 IP */
	if(!ipv4_is_valid(ipcb->dip))         /* 非法 IP 地址 */
	{
		zlog_debug(FTM_DBG_IP,"%s[%d] -> %s: dip format error, drop it!!\n", __FILE__, __LINE__, __func__);
		goto drop;
	}
    else if(ipv4_is_loopback(ipcb->dip) || 
            ipv4_is_broadcast(ipcb->dip)||
            ipv4_is_multicast(ipcb->dip)||
            ipv4_is_local_multicast(ipcb->dip))
    {
        return ip_rcv_local(pkt);
    }
	else                                  
	{
		iph->ttl--;        
		ipcb->ttl--;
		ipcb->is_changed = 1;
		return ip_forward(pkt);
	}

drop:
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
	pkt_free(pkt);
	return NET_FAILED;
}


/* 处理 GRE 报文 */
int gre_rcv(struct pkt_buffer *pkt)
{
	struct ip_control *ipcb = NULL;
	struct tunnel_t *ptunnel = NULL;
	struct inet_addr sip, dip;
    uint16_t proto_type = 0;

	ipcb = (struct ip_control *)(&(pkt->cb));

	/* 先查 tunnel 表 */
	memset(&sip, 0,sizeof(struct inet_addr));
	memset(&dip, 0,sizeof(struct inet_addr));
	sip.type = INET_FAMILY_IPV4;
	sip.addr.ipv4 = ipcb->sip;
	dip.type = INET_FAMILY_IPV4;
	dip.addr.ipv4 = ipcb->dip;
	ptunnel = ftm_gre_tunnel_lookup(&dip, &sip);
	if(ptunnel == NULL)
		goto drop;

	pkt->inif_type = PKT_INIF_TYPE_TUNNEL;
	pkt->in_ifindex = ptunnel->ifindex;
	pkt->priority = ipcb->tos;
	//pkt->vpn = ptunnel->vpn;

	/* 剥掉 gre 头 */
    proto_type = gre_decap(pkt);
	if(proto_type == 0)
		goto drop;
	else if(proto_type == ETH_P_IP)      /* 乘客协议是 IPV4 */
		return ip_rcv(pkt);
	else if(proto_type == ETH_P_MPLS_UC) /* 乘客协议是 mpls */
		return mpls_rcv(pkt);

drop:
	zlog_debug(FTM_DBG_IP,"gre_rcv get a error packet,drop it!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
	pkt_free(pkt);
	return NET_FAILED;
}


/*
 *	本机接收的报文，封装新的 IP 头，调用 APP 的回调函数送给 APP
 */
int ip_rcv_local(struct pkt_buffer *pkt)
{
	struct ip_control *ipcb = NULL;
	struct ip_proto proto;
	int module_id;

	ipcb = (struct ip_control *)(&(pkt->cb));

	/* 分片报文需要重组 */
	if((ipcb->frag_off & IP_FLAG_OFFSET) || (ipcb->frag_off & IP_FLAG_MF))
	{
		pkt = ip_defrag(pkt);
		if(pkt == NULL)
			return NET_SUCCESS;

		ipcb = (struct ip_control *)(&(pkt->cb));
		ipcb->is_changed = 1;
	}

    if(ipcb->protocol == IP_P_UDP)
    {
        return udp_rcv(pkt);
    }
    else if(ipcb->protocol == IP_P_TCP)
	{
		return tcp_rcv(pkt);
	}
    else if(ipcb->protocol == IP_P_ICMP)
	{
		ipcb->pkt_type = PKT_TYPE_IP;
		return icmp_rcv(pkt);
	}
    else if(ipcb->protocol == IP_P_GRE)
	{
		return gre_rcv(pkt);
	}
    else if(ipcb->protocol == IP_P_IPV6_ICMP)
	{
		ipcb->pkt_type = PKT_TYPE_IPV6;
		return icmpv6_rcv(pkt);
	}

	if(ipcb->option_len != 0)
	{
		if(pkt_push(pkt, ipcb->option_len)) /* push  the ip option */
		{
			return -1;
		}
	}
	/*find the registered module_id */
	memset(&proto,0 ,sizeof(struct ip_proto));
	proto.dip = ipcb->dip;	
	proto.sip = ipcb->sip;
	proto.protocol = ipcb->protocol;	
	IPV6_ADDR_COPY(&proto.dipv6, &ipcb->dipv6);	
	IPV6_ADDR_COPY(&proto.sipv6, &ipcb->sipv6);
	if(IP_PROTO_IS_IPV6(ipcb))
	{
		proto.type = PROTO_TYPE_IPV6;
	}
		
	
	module_id = ip_proto_lookup(&proto);
	if(!module_id)
	{
		zlog_debug(FTM_DBG_IP,"%s[%d] -> %s: module_id not exist, drop it!!\n", __FILE__, __LINE__, __func__);
		goto drop;
	}

	//ftm_pkt_send_to(pkt, module_id);/* send to app */
	ftm_pkt_send_to_n(pkt, module_id);/* send to app */
	pkt_free(pkt);
	return NET_SUCCESS;

drop:
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
	pkt_free(pkt);
	return NET_FAILED;
}


/*
根据 IP 控制头查 FIB 表，获取下一跳和出接口
*/
int ip_forward(struct pkt_buffer *pkt)
{
	struct ecmp_group *ecmp = NULL;
	struct nhp_info   *pnhp_info = NULL;
	struct nhp_entry  *pnhp = NULL;
	struct ip_control *ipcb = NULL;
	int i;

	ipcb = (struct ip_control *)(&(pkt->cb));

	/* 如果指定了出接口索引，则不查路由，直接从该接口出报文 */
	if(ipcb->ifindex)
	{
		return ip_output(pkt);
	}

	/* 单播报文查路由 */
	if(ipcb->pkt_type == PKT_TYPE_IP)    /* IPV4 报文 */
	{
		struct fib_entry  *pfib = ftm_fib_match(ipcb->dip, IP_MASK_LEN, ipcb->vpn);
		if(!pfib)
		{
			zlog_debug(FTM_DBG_IP,"%s[%d] -> %s: ftm_fib_match error, drop it!!\n", __FILE__, __LINE__, __func__);
			goto drop;
		}
		
		pnhp_info = &pfib->nhp;

	   	/* 判断 dip 是否为网段广播 */
	    if((pfib->masklen == 31) && ipv4_is_broadcast1(ipcb->dip, pfib->masklen))
	    {
			ipcb->pkt_type = PKT_TYPE_IP;
	    }

		if((pfib->masklen != IP_MASK_LEN)&&(pfib->masklen != 31) && ipv4_is_broadcast1(ipcb->dip, pfib->masklen))
	    {
	    	if(pkt->in_port == 0)
	    	{
				ipcb->pkt_type = PKT_TYPE_IPBC;
			}
			else
			{
				zlog_debug(FTM_DBG_IP,"%s[%d] -> %s: network broadcast does not forward, drop it!!\n", __FILE__, __LINE__, __func__);
				goto drop;
			}	
	    }
	}
	else if(ipcb->pkt_type == PKT_TYPE_IPV6)      /* IPV6 报文 */
	{
	    struct fibv6_entry  *pfib = ftm_fibv6_match(&ipcb->dipv6, IPV6_MASK_LEN, ipcb->vpn);
		if(!pfib)
		{
			zlog_debug(FTM_DBG_IP,"%s[%d] -> %s: ftm_fib_match error, drop it!!\n", __FILE__, __LINE__, __func__);
			goto drop;
		}
		
		pnhp_info = &pfib->nhp;
	}

	if(pnhp_info == NULL) 
		goto drop;
	
	/* 处理多下一跳的情况 */
    switch (pnhp_info->nhp_type)
	{
		case NHP_TYPE_FRR:
		{
			ecmp = ftm_ecmp_lookup(pnhp_info->nhp_index);
		    if(ecmp == NULL)
			{
				goto drop;
			}

	        i = ecmp->frr_flag;
			pnhp_info = &(ecmp->nhp[i]);
			break;
		}
		case NHP_TYPE_ECMP:
		{
			ecmp = ftm_ecmp_lookup(pnhp_info->nhp_index);
		    if(!ecmp)
			{
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
				goto drop;
			}

			pnhp_info = &(ecmp->nhp[i]);
			break;
		}
		default: /* 单下一跳 */
		{
			break;
		}
    }

	/* 处理单个下一跳的情况 */
	pnhp =	ftm_nhp_lookup(pnhp_info->nhp_index);/* 获取 nhp */
	if(!pnhp)
	{
		goto drop;
	}
	if(pnhp->action == NHP_ACTION_DROP)
	{
		goto drop;
	}
	else if(pnhp->action == NHP_ACTION_TOCPU)
	{
		return ip_rcv_local(pkt);  /* ipv4 与 ipv6 共用 */
	}
	else if(pnhp->action == NHP_ACTION_FORWARD)
	{
		if((ipcb->ttl < 1)&&(pkt->in_ifindex != 0))
		{
			icmp_send(pkt, ICMP_TYPE_TIMEOUT, 0);
			goto drop;
		}
	
		if(pnhp->nhp_type == NHP_TYPE_LSP) /* 转发到 LSP */
		{
		    ipcb->if_type = PKT_INIF_TYPE_LSP;	/* 接口类型是 lsp */
		}
		else
		{
			ipcb->if_type = PKT_INIF_TYPE_IF;
			
			/* 设置下一跳 */
			memcpy(&ipcb->nexthop, &pnhp->nexthop_connect, sizeof(struct inet_addr));			
			if(ipcb->nexthop.type == INET_FAMILY_INVALID)
			{
				goto drop;
			}
			else if(ipcb->nexthop.type == INET_FAMILY_IPV4)
			{
				if(ipcb->nexthop.addr.ipv4 == 0)     /* 网段路由的下一跳为 0 */
					ipcb->nexthop.addr.ipv4 = ipcb->dip;
			}
			else if(ipcb->nexthop.type == INET_FAMILY_IPV6)				
			{
				if(ipv6_is_zero((struct ipv6_addr *)(ipcb->nexthop.addr.ipv6)))
					IPV6_ADDR_COPY(&ipcb->nexthop.addr, &ipcb->dipv6);
			}
		}

		ipcb->ifindex = pnhp->ifindex; /* 设置出接口 */
		return ip_output(pkt);
	}

drop:

	zlog_debug(FTM_DBG_IP,"ip_forward failed, drop the packet!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
	pkt_free(pkt);
	return NET_FAILED;
}


/*
获取出接口信息，处理 IP 分片，封装新的 IP 头
*/
int ip_output(struct pkt_buffer *pkt)
{
	struct ip_control   *ipcb = NULL;
	struct mpls_control  mpls_cb;
	struct ftm_ifm *pifm = NULL;
	struct ifm_info *pif_info = NULL;
    struct ipv6_addr *if_linklocal = NULL;
    struct ipv6_addr *paddr = NULL;
	int ret = -1;

	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_SEND, PKT_TYPE_IP);
	ftm_pkt_cnt_add(PKT_ACTION_SEND, PKT_TYPE_IP);

	ipcb = (struct ip_control *)(&(pkt->cb));
    if(ipcb->ifindex == 0)
		goto drop;

	/* 获取出接口 */
	if(ipcb->if_type == PKT_INIF_TYPE_IF)
	{
	    pifm = ftm_ifm_lookup(ipcb->ifindex);
	    if (!pifm)
		{
			zlog_debug(FTM_DBG_IP,"%s, %d ftm_ifm_lookup error ,drop it!\n",__FUNCTION__, __LINE__);
			goto drop;
		}

        if ((pifm->pl3if) && (ipcb->vpn != pifm->pl3if->vpn))
        {
            zlog_err("%s,%d ipcb->vpn is not equal to intf!\n",__func__,__LINE__);
            goto drop;
        }

		pif_info = &(pifm->ifm);
	    
		if(ipcb->pkt_type == PKT_TYPE_IP)
		{
			if((ipcb->sip == 0) && (ipcb->is_changed == 1))
			{
                /*intf is not l3intf and sip is 0,drop packet.maybe drop igmp pkt*/
                if(!(pifm->pl3if))
                {
                    FTM_PKT_ZLOG_DBG("l3if is NULL,sip set err! drop it!\n");
                    goto drop;
                }

				ipcb->sip = ftm_ifm_get_l3if_ipv4(pifm->pl3if, ipcb->nexthop.addr.ipv4);
			}
		}
		else if(ipcb->pkt_type == PKT_TYPE_IPV6)
		{
			if((ipv6_is_zero(&ipcb->sipv6)) && (ipcb->is_changed == 1))
            {
                /*intf is not l3intf and sip is 0,drop packet*/
                if(!(pifm->pl3if))
                {
                    FTM_PKT_ZLOG_DBG("l3if is NULL,sip set err! drop it!\n");
                    goto drop;
                }

                /*if dip is linklocal,set if_linklocal as sip.if if_linklocal is NULL,drop the pkt;i*/
        	    if(ipv6_is_linklocal(&(ipcb->dipv6)))
                {
                    if (!ipv6_is_zero((struct ipv6_addr *)(pifm->pl3if->ipv6_link_local.addr)))
					    IPV6_ADDR_COPY(&ipcb->sipv6, pifm->pl3if->ipv6_link_local.addr);
                    else
                        goto drop;
                }
                else
                {
                    /*if dip is not linklocal but nexthop is linklocal,first find dip's samesubnet ip in 
                    *this intf,if not exist ,set master ip as sip,if master ip not exist yet,set routerv6_id as sip;
                    *if dip is not linklocal and nexthop is not linklocal,first find nexthop's samesubnet ip in this 
                    *intf,if not exist(perhaps not happend),then set master ip as sip,if not exist,set routeid as sip*/
                    if (ipv6_is_linklocal((struct ipv6_addr *)(ipcb->nexthop.addr.ipv6)))
    				{
                        paddr = ftm_ifm_get_l3if_ipv6(pifm->pl3if, (struct ipv6_addr *)&(ipcb->dipv6));
                    }
                    
                    if (!paddr)
                    {
                        paddr = ftm_ifm_get_l3if_ipv6(pifm->pl3if, (struct ipv6_addr *)(ipcb->nexthop.addr.ipv6));
                    }
                    
                    if (!paddr) goto drop;

					IPV6_ADDR_COPY(&ipcb->sipv6, paddr);
                }
            }
		}
	}
	else if(ipcb->if_type == PKT_INIF_TYPE_LSP)
	{
	    if(ipcb->pkt_type == PKT_TYPE_IP)       /* ipv4 报文*/
	    {
	    	if(ipcb->sip == 0)
		    	ipcb->sip = g_mpls.lsr_id;
	    }
		else if(ipcb->pkt_type == PKT_TYPE_IPV6)   /* ipv6 报文*/
		{
	    	if(ipv6_is_zero(&ipcb->sipv6))
		    	IPV6_ADDR_COPY(&ipcb->sipv6, &g_mpls.lsrv6_id);
		}
	}
	else
		goto drop;

    /*if dip6 is linklocal and the same to outif linklocal,then ip_rcv_local*/
    if (IP_PROTO_IS_IPV6(ipcb) && ipv6_is_linklocal(&ipcb->dipv6))
    {
        //packet can not be send if the output ifindex is linkdown
        if (!pifm || pifm->ifm.status != IFNET_LINKUP)
            goto drop;

        IPV6_GET_IFLINKLOCAL(pifm, if_linklocal);
        if ((if_linklocal) && IPV6_ADDR_SAME(&ipcb->dipv6, if_linklocal))
        {
            pkt->out_ifindex = ipcb->ifindex;
            pkt->in_ifindex = pkt->out_ifindex; 
            return ip_rcv_local(pkt);
        }
    }

	/* encap new ip header */
	if(IP_PROTO_IS_IPV4(ipcb))
	{
        ret = ip_encap(pkt);
	}
	else if(IP_PROTO_IS_IPV6(ipcb))
	{
        ret = ipv6_encap(pkt);
	}
	if(ret)
	{
		zlog_debug(FTM_DBG_IP,"%s, %d ip_encap fail,drop it!\n",__FUNCTION__, __LINE__);
		goto drop;
	}
	
    /* 设置报文的出接口和优先级 */
	pkt->out_ifindex = ipcb->ifindex;
	pkt->priority = ipcb->tos;

    /* 出接口是 lsp, 走 mpls 转发 */
    if(ipcb->if_type == PKT_INIF_TYPE_LSP)
    {
		memset(&mpls_cb, 0 , sizeof(struct mpls_control));
		pkt->cb_type = PKT_TYPE_MPLS;
		mpls_cb.if_type = PKT_INIF_TYPE_LSP;   /* 接口类型是 lsp */
		mpls_cb.ifindex = ipcb->ifindex;
		mpls_cb.exp = ipcb->tos;
		mpls_cb.ttl = ipcb->ttl;
		mpls_cb.is_changed = 1;
		memcpy(&(pkt->cb), &mpls_cb, sizeof(struct mpls_control));
		return mpls_forward(pkt);
    }

    /* 出接口是实际接口 */
    if(pif_info == NULL)
		goto drop;

	/* 判断 ipv4 是否要分片 */
	if((pif_info->mtu < pkt->data_len) && (IP_PROTO_IS_IPV4(ipcb)))
	{
		if((ipcb->frag_off & IP_FLAG_DF) == 0)
			return ip_frag(pif_info, pkt);
		else
			goto drop;
	}

	/* 判断出接口类型，走不同的发送流程 */
	if((pif_info->type == IFNET_TYPE_TUNNEL) &&(ipcb->protocol != IP_P_GRE))
    {
    	return tunnel_xmit(pkt);
    }	
	else if(pif_info->type == IFNET_TYPE_LOOPBACK)  /* loopback 接口 */
	{
		return ip_rcv_local(pkt);
	}
	else if((pif_info->type == IFNET_TYPE_ETHERNET)  /* 以太物理接口或子接口 */
			||(pif_info->type == IFNET_TYPE_GIGABIT_ETHERNET)
			||(pif_info->type == IFNET_TYPE_XGIGABIT_ETHERNET)
		    ||(pif_info->type == IFNET_TYPE_TRUNK)   /* trunk 接口或子接口 */
		    ||(pif_info->type == IFNET_TYPE_VLANIF)) /* vlanif 接口 */
	{
		if(IP_PROTO_IS_IPV6(ipcb))     /* ipv6 报文*/
			return ipv6_xmit(pkt);
		else if(IP_PROTO_IS_IPV4(ipcb))
			return ip_xmit(pkt);   /* ipv4 报文*/
	}
	else
	{
		zlog_debug(FTM_DBG_IP,"%s, %d ip_output fail,drop it !pif_info->type %d ipcb->protocol %d\n",
			__FUNCTION__, __LINE__, pif_info->type ,ipcb->protocol);
    }

drop:
	zlog_debug(FTM_DBG_IP,"ip_output failed, drop the packet!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
	pkt_free(pkt);
	return NET_FAILED;
}


/* 封装二层头, 发送 IPv4 报文 */
int ip_xmit(struct pkt_buffer *pkt)
{
	struct iphdr *iph = NULL;
	struct eth_control *ethcb = NULL;
	struct ip_control *ipcb = NULL;
	int pkt_type = 0;

	iph = (struct iphdr *)pkt->network_header;
	ipcb = (struct ip_control *)(&(pkt->cb));
	pkt_type = ipcb->pkt_type;
	
	/* 检查目的 IP */
	if(!ipv4_is_valid(ipcb->dip)        /* 非法 IP 地址 */
	  ||(ipv4_is_loopback(ipcb->dip)))   /* DIP 是 127 网段 */   
	{
		zlog_debug(FTM_DBG_IP,"%s, %d dip invalid ,drop it!\n",__FUNCTION__, __LINE__);
		goto drop;
	}
	else if((ipv4_is_multicast(ipcb->dip))      /* DIP 是组播 */
		||(ipv4_is_local_multicast(ipcb->dip))) /* DIP 是保留组播 */
	{
        pkt_type = PKT_TYPE_IPMC;
	}
	else if(ipv4_is_broadcast(ipcb->dip))  /* DIP 是广播 */
	{
        pkt_type = PKT_TYPE_IPBC;
	}
	
	if(pkt_type == PKT_TYPE_IP)      /* 单播报文 */
	{
		 return ip_xmit_unicast(pkt);
	}

    /* 组播或广播报文直接发送 */
	if(ipcb->ifindex == 0)
		goto drop;
    pkt->priority = ipcb->tos;
	pkt->out_ifindex = ipcb->ifindex;

	/* 设置链路层封装信息 */
	ethcb = (struct eth_control *)(&(pkt->cb));
	memset(ethcb, 0, PKT_CB_SIZE);
	pkt->cb_type = PKT_TYPE_ETH;
	ethcb->ethtype = ETH_P_IP;
	ethcb->cos = pkt->priority;
	ethcb->ifindex = pkt->out_ifindex;
	if(pkt_type == PKT_TYPE_IPMC)
		ether_get_muticast_mac(htonl(iph->daddr), ethcb->dmac);
	else if(pkt_type == PKT_TYPE_IPBC)
		memcpy(ethcb->dmac, ether_get_broadcast_mac(), 6);/* 封装广播 MAC */
	ethcb->is_changed = 1;
	eth_forward(pkt);
	return ERRNO_SUCCESS;

drop:
	zlog_debug(FTM_DBG_IP,"ip_xmit failed, drop the packet!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
	pkt_free(pkt);
	return NET_FAILED;
}


/* 因 arp/nd 查找失败缓存的单播报文重新发送 */
int ip_xmit_retry(struct thread *thread)
{
	struct pkt_buffer *pkt = NULL;
	struct listnode  *pnode  = NULL;
	struct listnode  *pnextnode = NULL;
	int ret = ERRNO_FAIL;
	int num = ip_retry_list.count;

	for (ALL_LIST_ELEMENTS(&ip_retry_list, pnode, pnextnode, pkt))
	{
		list_detach_node(&ip_retry_list, pnode);
		if(pkt &&(pkt->data_len == 0))
		{
			zlog_debug(FTM_DBG_IP,"%s, %d ip_xmit_retry ,pkt->data_len == 0, drop the packet!\n",__FUNCTION__, __LINE__);
		}
		if(pkt && (pkt->data_len != 0))
		{
			pkt->retry_cnt++;
			
			if(pkt->protocol == ETH_P_IP)
				ret = ip_xmit_unicast(pkt);
			else if(pkt->protocol == ETH_P_IPV6)
			    ret = ipv6_xmit_unicast(pkt);
			
			if(ret == ERRNO_SUCCESS)
			{
				listnode_free (pnode);
			}
			else if(pkt->retry_cnt > PKT_RETRY_NUM)
			{
				zlog_debug(FTM_DBG_IP,"ip_xmit_retry timeout, drop the packet!\n");
				ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
				listnode_free (pnode);
				pkt_free(pkt);
			}
			else
			{
				list_add_node(&ip_retry_list, pnode);
			}
		}

		/* 遍历完链表退出 */
		num --;
		if(num == 0)
			break;
	}

	if(ip_retry_list.count)
	{
		ip_retry_timer = high_pre_timer_add("FtmPktXmitRetry",LIB_TIMER_TYPE_NOLOOP, ip_xmit_retry, NULL, 500);
		//ip_retry_timer = thread_add_timer_msec(ftm_master, ip_xmit_retry, 0, 500); /* 500ms 重传*/
	}
	else
	{
		ip_retry_timer = 0;
	}
	return ret;
}


/* 单播报文查 ARP/nd 获取以太封装信息 */
int ip_xmit_unicast(struct pkt_buffer *pkt)
{
	struct ip_control   *ipcb = NULL;
	struct eth_control  *ethcb = NULL;
	struct arp_entry    *parp = NULL;

	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_SEND, PKT_TYPE_IP);
	ftm_pkt_cnt_add(PKT_ACTION_SEND, PKT_TYPE_IP);

	ipcb = (struct ip_control *)(&(pkt->cb));
	if(ipcb->nexthop.addr.ipv4 == 0)
		ipcb->nexthop.addr.ipv4 = ipcb->dip;

	/* 单播报文查 arp */
	parp = arp_lookup(ipcb->nexthop.addr.ipv4, ipcb->vpn);
	if((NULL == parp)||(parp && (parp->status == ARP_STATUS_INCOMPLETE)))
	{
		if(!parp) /* arp 不存在时，触发 ARP 学习 */
		{
		    arp_miss_anti_cnt(pkt);
			arp_miss(ipcb->nexthop.addr.ipv4, ipcb->vpn);
		}

		if((pkt->retry_cnt == 0) && (ip_retry_list.count < PKT_RETRY_LIST_LEN)) /* 限制重传链表的长度为 100 */
		{
			listnode_add(&ip_retry_list, pkt);	/* 报文加入重传链表 */
			if(ip_retry_timer == 0)
				ip_retry_timer = high_pre_timer_add("FtmPktXmitRetry",LIB_TIMER_TYPE_NOLOOP, ip_xmit_retry, NULL, 500);
				//ip_retry_timer = thread_add_timer_msec(ftm_master, ip_xmit_retry, 0, 500); /* 500ms 重传*/
            return ERRNO_SUCCESS;
        }
		else if(ip_retry_list.count >= PKT_RETRY_LIST_LEN)
		{
			zlog_debug(FTM_DBG_IP,"%s, %d ip_xmit_retry , drop the packet!\n",__FUNCTION__, __LINE__);
			goto drop;
		}

		return ERRNO_FAIL;
	}

	/* 设置链路层封装信息 */
	ethcb = (struct eth_control *)(&(pkt->cb));
	memset(ethcb, 0, PKT_CB_SIZE);
	pkt->cb_type = PKT_TYPE_ETH;
	ethcb->ethtype = ETH_P_IP;
	ethcb->cos = pkt->priority;
	ethcb->ifindex = pkt->out_ifindex;

	if(parp->port)
	{
		ethcb->ifindex = parp->port;		
	    ethcb->svlan = IFM_VLANIF_ID_GET(parp->ifindex);
	}
	else
	{
		if(ethcb->ifindex != parp->ifindex)
		{
			zlog_err("%s, %d ethcb->ifindex(%x) != parp->ifindex(%x) , drop the packet!\n",__FUNCTION__, __LINE__,ethcb->ifindex, parp->ifindex);
			goto drop;
		}
		//ethcb->ifindex = parp->ifindex;
	}
	memcpy((char *)ethcb->dmac, (char *)parp->mac, 6);
	ethcb->is_changed = 1;
	eth_forward(pkt);
	return ERRNO_SUCCESS;

drop:
	zlog_debug(FTM_DBG_IP,"ip_xmit_unicast failed, drop the packet!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
	pkt_free(pkt);
	return NET_FAILED;
}


/* 封装二层头, 发送 IPv4 广播报文 */
int ip_xmit_broadcast(struct pkt_buffer *pkt)
{
	struct eth_control *ethcb = NULL;
	struct ip_control *ipcb = NULL;

	ipcb = (struct ip_control *)(&(pkt->cb));

    /* 组播或广播报文直接发送 */
	if(ipcb->ifindex == 0)
		goto drop;
	
    pkt->priority = ipcb->tos;
	pkt->out_ifindex = ipcb->ifindex;

	/* 设置链路层封装信息 */
	ethcb = (struct eth_control *)(&(pkt->cb));
	memset(ethcb, 0, PKT_CB_SIZE);
	pkt->cb_type = PKT_TYPE_ETH;
	ethcb->ethtype = ETH_P_IP;
	ethcb->cos = pkt->priority;
	ethcb->ifindex = pkt->out_ifindex;
	memcpy(ethcb->dmac, ether_get_broadcast_mac(), 6);/* 封装广播 MAC */
	ethcb->is_changed = 1;
	eth_forward(pkt);
	return ERRNO_SUCCESS;

drop:
	zlog_debug(FTM_DBG_IP,"ip_xmit failed, drop the packet!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
	pkt_free(pkt);
	return NET_FAILED;
}


/* 组播报文转发 */
int ip_xmit_multicast(struct pkt_buffer *pkt)
{
	struct iphdr *iph = NULL;
	struct eth_control *ethcb = NULL;
	struct ip_control *ipcb = NULL;

	iph = (struct iphdr *)pkt->network_header;
	ipcb = (struct ip_control *)(&(pkt->cb));

	/* 组播或广播报文直接发送 */
	if(ipcb->ifindex == 0)
		goto drop;
	
	pkt->priority = ipcb->tos;
	pkt->out_ifindex = ipcb->ifindex;

	/* 设置链路层封装信息 */
	ethcb = (struct eth_control *)(&(pkt->cb));
	memset(ethcb, 0, PKT_CB_SIZE);
	pkt->cb_type = PKT_TYPE_ETH;
	ethcb->ethtype = ETH_P_IP;
	ethcb->cos = pkt->priority;
	ethcb->ifindex = pkt->out_ifindex;
	ether_get_muticast_mac(iph->daddr, ethcb->dmac);  /* 封装组播 MAC */
	ethcb->is_changed = 1;
	eth_forward(pkt);
	return ERRNO_SUCCESS;

drop:
	zlog_debug(FTM_DBG_IP,"ip_xmit failed, drop the packet!\n");
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
	pkt_free(pkt);
	return NET_FAILED;
}


/*add ip header to transport data*/
int ip_encap(struct pkt_buffer *pkt)
{
	struct iphdr *iph = NULL;
	struct ip_control *ipcb = NULL;
	struct udphdr *udph = NULL;
	struct tcphdr *tcph = NULL;
	struct pseudo_hdr *ppseudo = NULL;
	static uint16_t id = 0;
	uint16_t size = 0;
	uint8_t ip_len = 0;
	int id_t = 0;

	ipcb = (struct ip_control *)(&(pkt->cb));
 	if(ipcb->is_changed == 0)
 	{
		return pkt_push(pkt, ((uint32_t)pkt->data - (uint32_t)pkt->network_header)); /*recover the ip header */
 	}
	else
	 	ipcb->is_changed = 0;

	size = pkt->data_len;  /* 先记录数据长度 */

	ip_len  = ipcb->option_len + IP_HEADER_SIZE; 
	/* set ip header */
	if(pkt_push(pkt, sizeof(struct iphdr)))
		return -1;
	pkt->network_header = pkt->data;

	if(ipcb->chsum_enable)
	{
		if(ipcb->protocol == IP_P_UDP) /* 计算 UDP 的 checksum */
		{
			/* 偏移出伪首部, 计算 checksum */
	        ppseudo = (struct pseudo_hdr *)((uchar *)pkt->data + (IP_HEADER_SIZE - IP_PSEUDO_HDR_SIZE));
			ppseudo->saddr = htonl(ipcb->sip);
			ppseudo->daddr = htonl(ipcb->dip);
			ppseudo->placeholder = 0;
			ppseudo->protocol = IP_P_UDP;
			ppseudo->length = htons(size);			
			udph = (struct udphdr *)((uint8_t *)(pkt->data) + ip_len);
			udph->checksum = 0;
			udph->checksum = in_checksum((uint16_t *)ppseudo, IP_PSEUDO_HDR_SIZE + size);
			zlog_debug(FTM_DBG_IP,"%s, %d udph->checksum:0x%x,pkt->data_len:0x%x\n",__FUNCTION__, __LINE__, udph->checksum, size);
		}
		else if(ipcb->protocol == IP_P_TCP) /* 计算 TCP 的 checksum */
		{
			/* 偏移出伪首部, 计算 checksum */
	        ppseudo = (struct pseudo_hdr *)((uchar *)pkt->data + (IP_HEADER_SIZE - IP_PSEUDO_HDR_SIZE));
			ppseudo->saddr = htonl(ipcb->sip);
			ppseudo->daddr = htonl(ipcb->dip);
			ppseudo->placeholder = 0;
			ppseudo->protocol = IP_P_TCP;
			ppseudo->length = htons(size);
			tcph = (struct tcphdr *)((uint8_t *)(pkt->data) + ip_len);
			tcph->checksum = 0;
			tcph->checksum = in_checksum((uint16_t *)ppseudo, IP_PSEUDO_HDR_SIZE + size);
			zlog_debug(FTM_DBG_IP,"%s, %d tcph->checksum:0x%x,pkt->data_len:0x%x\n",__FUNCTION__, __LINE__, tcph->checksum, size);
		}
	}

	/* 封装新的ip头*/
	iph = (struct iphdr *)pkt->network_header;
	if (iph->id) /*软转发不应该修改id,这里需要先将报文id保存*/
    {
    	id_t = iph->id;
    }
	memset(iph, 0, sizeof(struct iphdr));
	iph->protocol =	ipcb->protocol;
	iph->daddr = htonl(ipcb->dip);
	iph->saddr = htonl(ipcb->sip);
	iph->tot_len  =	htons(pkt->data_len);

	iph->ihl  =	5 + ipcb->option_len/4;     /* 如果有 ip option，就不是 5 */
	iph->version  =	4;
	iph->tos	  =	ipcb->tos<<5;
	iph->ttl	  =	ipcb->ttl;

	if(id_t)/*软转发不应该修改id,这里将保存的报文id重新赋值给报文*/
    	iph->id       = id_t;
    else
    	iph->id       = htons(id++);
	iph->frag_off =	htons(ipcb->frag_off);
	iph->check = 0;
	iph->check = in_checksum((u_int16_t *)iph, IP_HEADER_SIZE + ipcb->option_len);

	pkt->protocol = ETH_P_IP;
	return 0;
}


/*offset ip header to get ip_cb */
int ip_decap(struct pkt_buffer *pkt)
{
	struct iphdr *iph = NULL;
	struct ip_control *ipcb  = NULL;
	int len, hlen;

	pkt->network_header = pkt->data;  /* set ip header pointer */
	iph = (struct iphdr *)(pkt->network_header); /* get ip header */

	/*check the version and header len */
	hlen = iph->ihl*4;
	if (hlen < IP_HEADER_SIZE || iph->version != 4)
	{
		zlog_debug (FTM_DBG_IP,"%s, %d hlen < IP_HEADER_SIZE || iph->version != 4\n",__FUNCTION__, __LINE__);
		return -1;
	}

	/*check the length of packet */
	len = ntohs(iph->tot_len);
	if (len < IP_HEADER_SIZE)
	{
		return -1;
	}

    if(pkt_pull(pkt, hlen)) /* offset the ip header */
	{
		return -1;
	}

	/* ip payload length */
	pkt->data_len = len - hlen;

    /* set ip cb */
    ipcb = (struct ip_control *)(&(pkt->cb));
    memset(ipcb, 0 , PKT_CB_SIZE);
    pkt->cb_type = PKT_TYPE_IP;
	ipcb->sip = ntohl(iph->saddr);
	ipcb->dip = ntohl(iph->daddr);
	ipcb->protocol = iph->protocol;
	ipcb->ttl = iph->ttl;
	ipcb->tos = iph->tos;
	ipcb->frag_off = ntohs(iph->frag_off);
	if(hlen > IP_HEADER_SIZE)
	{
		ipcb->option_len = hlen - IP_HEADER_SIZE;
	}
	
    return 0;
}


/*add gre header to payload */
int gre_encap(struct pkt_buffer *pkt)
{
	struct gre_hdr *greh = NULL;

	/* 封装 GRE 头 */
	if(pkt_push(pkt, 4)) /* 不要 checksum 字段就只有 4 字节 */
		return -1;

	greh = (struct gre_hdr *)pkt->data;
	greh->cbit = 0;
	greh->reserv0 = 0;
	greh->version = 0;
	greh->proto_type = htons(pkt->protocol);

	return 0;
}


/*offset GRE header to get proto_type */
int gre_decap(struct pkt_buffer *pkt)
{
	int hlen;
	struct gre_hdr *greh = NULL;
	uint16_t proto_type = 0;

	/*check the version and header len */
	greh = (struct gre_hdr *)pkt->data;
	if((greh->reserv0) || (greh->version))  /* 报文错误 */
		return 0;

	hlen = 4;       /* GRE 头 4 字节 */
	if(greh->cbit)
		hlen =+ 4;  /* checksum 字段有效 */

	if(pkt_pull(pkt, hlen)) /* offset the gre header */
	{
		return 0;
	}

	proto_type = ntohs(greh->proto_type);
    return proto_type;
}


static struct ip_frag_list_s * ip_frag_list_alloc(void)
{
	struct ip_frag_list_s *ipfrag = NULL;

	ipfrag = XCALLOC (MTYPE_PREFIX_IPV4, sizeof (struct ip_frag_list_s));
	if(ipfrag == NULL)
	{
		zlog_debug (FTM_DBG_IP,"%s, %d XCALLOC fail!\n",__FUNCTION__, __LINE__);
        return NULL;
	}
	if(g_ipfraglistnum == 0)
	{
		zlog_debug(FTM_DBG_IP,"%s, %d \n",__FUNCTION__, __LINE__);
		pipfraglist = ipfrag;
		ipfrag->prev = NULL;
		ipfrag->next = NULL;
		ipfrag->fraglist.del = (void *)pkt_free;
	}
	else
	{
		zlog_debug(FTM_DBG_IP,"%s, %d \n",__FUNCTION__, __LINE__);
		ipfrag->prev = NULL;
		ipfrag->next = pipfraglist;
		ipfrag->fraglist.del = (void *)pkt_free;
		pipfraglist->prev = ipfrag;
		pipfraglist = ipfrag;
	}

	g_ipfraglistnum++;
	zlog_debug(FTM_DBG_IP,"%s, %d g_ipfraglistnum:0x%x\n",__FUNCTION__, __LINE__,g_ipfraglistnum);
	return ipfrag;
}

static void ip_frag_list_free(struct ip_frag_list_s *Node)
{
	struct ip_frag_list_s *prev = NULL;
	struct ip_frag_list_s *next = NULL;
	struct list *plist = NULL;

	prev = Node->prev;
	next = Node->next;
	plist = &(Node->fraglist);
	list_delete_all_node(plist);
	XFREE(MTYPE_PREFIX_IPV4, Node);

	/* 首节点信息 */
	if(prev == NULL)
	{
		pipfraglist = next;
		if(next != NULL)
		{
			next->prev = NULL;
		}
	}
	else
	{
		prev->next = next;
		if(next != NULL)
		{
			next->prev = prev;
		}
	}

	g_ipfraglistnum --;

	return;
}

/* 在系统收到一个IP分片包后的30秒再没有收到任何和它属于同一IP包的分片数据时，
系统会丢弃那些属于同一个IP包但未完成的IP分片重组的数据.*/
static int ip_frag_list_expire (struct thread *arg)
{
	struct ip_frag_list_s *pfragList = NULL;
	struct ip_frag_list_s *curfragList = NULL;

	curfragList = pipfraglist;

	/* Remove this entry from the "incomplete datagrams" queue. */
	while(curfragList != NULL)
	{
		pfragList = curfragList;
		curfragList = curfragList->next;
		if(pfragList->timeout < 3)
		{
			ip_frag_list_free(pfragList);
		}
		else
		{
			pfragList->timeout -= 3;
		}
	}

	//thread_add_timer(ftm_master, ip_frag_list_expire, NULL, 3);
	return 0;
}


/* 初始化分片报文链表 */
static void ip_frag_list_init ( void )
{
	pipfraglist = NULL;
	g_ipfraglistnum = 0;

	//thread_add_timer(ftm_master, ip_frag_list_expire, NULL, 3);
	high_pre_timer_add("FtmIPFragList", LIB_TIMER_TYPE_LOOP, ip_frag_list_expire, NULL, 3 * 1000);
	return;
}


/* 查找已经存在的分片链表 */
static struct ip_frag_list_s * ip_frag_list_lookup(struct pkt_buffer *pkt)
{
	struct ip_frag_list_s *pfragList = NULL;
	struct listnode *tmpNode = NULL;
	struct list *plist = NULL;
	struct pkt_buffer *pktBuf = NULL;
	struct iphdr *iphCmp = NULL;
	struct iphdr *iph = NULL;
	int i;

	iphCmp = (struct iphdr *)pkt->network_header;
	pfragList = pipfraglist;

	/* 遍历所有的链表，查找是否已经有分片在链表内 */
	for(i = 0; i < g_ipfraglistnum; i++)
	{
		if(pfragList == NULL)
		{
			zlog_debug (FTM_DBG_IP,"%s, %d pfragList == NULL\n",__FUNCTION__, __LINE__);
			break;
		}
		plist = &(pfragList->fraglist);
		tmpNode = plist->head;
		if(tmpNode != NULL)
		{
			pktBuf = (struct pkt_buffer *)tmpNode->data;
			if(pktBuf != NULL)
			{
				iph = (struct iphdr *)pktBuf->network_header;
				if(iph->daddr == iphCmp->daddr && iph->saddr == iphCmp->saddr
				&& iph->protocol == iphCmp->protocol && iph->id == iphCmp->id)
				{
					return pfragList;
				}
				else
				{
					pfragList = pfragList->next;
					continue;
				}
			}
			else
			{
				ip_frag_list_free(pfragList);
				break;
			}
		}
		else
			break;
	}

	return NULL;
}


/* 将分片报文添加到链表 */
static int ip_frag_list_add(struct ip_frag_list_s *Node, struct pkt_buffer *pkt)
{
	struct list *pfraglist = NULL;
	struct listnode *prev, *next = NULL;
	struct pkt_buffer *pkttmp = NULL;
	struct iphdr *iph = NULL;
	int offset,PktOffset;
	int ihl, end, prevEnd;

	pfraglist = &(Node->fraglist);

	iph = (struct iphdr *)pkt->network_header;
	if(iph == NULL)
	{
		zlog_debug (FTM_DBG_IP,"%s, %d iph == NULL\n",__FUNCTION__, __LINE__);
		return -1;
	}

	offset = ntohs(iph->frag_off);
	offset &= IP_FLAG_OFFSET;
	offset <<= 3;                   /* offset is in 8-byte chunks */
	ihl = iph->ihl * 4;

	/* Attempt to construct an oversize packet. PKT_BUFFER_SIZE2 */
	if (ntohs(iph->tot_len) + (int) offset > PKT_BUFFER_SIZE2)
	{
		zlog_debug (FTM_DBG_IP,"%s, %d tot_len:0x%x, + offset:0x%x > 9900\n",__FUNCTION__, __LINE__, ntohs(iph->tot_len), offset);
		return -1;
	}
	/* Determine the position of this fragment. */
	end = offset + ntohs(iph->tot_len) - ihl;

	/* 根据offset在链表中找要插入的位置 */
	prev = NULL;
	for (next = pfraglist->head; next != NULL; next = next->next)
	{
		pkttmp = (struct pkt_buffer *)(next->data);
		if(pkttmp == NULL)
		{
			zlog_debug (FTM_DBG_IP,"%s, %d pkttmp == NULL\n",__FUNCTION__, __LINE__);
			return -1;
		}
		iph = (struct iphdr *)pkttmp->network_header;
		PktOffset = ntohs(iph->frag_off);
		PktOffset &= IP_FLAG_OFFSET;
		PktOffset <<= 3;                   /* offset is in 8-byte chunks */

		/* 分片偏移重叠或者和next所指向的分片数据有重叠时，删除分片 */
		if((PktOffset == offset) || ((PktOffset > offset) && (PktOffset < end)))
		{
			zlog_debug (FTM_DBG_IP,"%s, %d (PktOffset == offset) || ((PktOffset > offset) && (PktOffset < end)\n",__FUNCTION__, __LINE__);
			return -1;
		}
		else if(PktOffset >=  end)
		{
			break;
		}
		else
		{
			prev = next;
			continue;
		}
	}

	/*	检查当前分片数据和prev所指向的分片数据是否有重叠，重叠时删除分片	*/
	if(prev != NULL)
	{
		pkttmp = (struct pkt_buffer *)(prev->data);
		if(pkttmp == NULL)
		{
			zlog_debug (FTM_DBG_IP,"%s, %d pkttmp == NULL\n",__FUNCTION__, __LINE__);
			return -1;
		}
		iph = (struct iphdr *)pkttmp->network_header;
		PktOffset = ntohs(iph->frag_off);
		PktOffset &= IP_FLAG_OFFSET;
		PktOffset <<= 3;                   /* offset is in 8-byte chunks */
		ihl = iph->ihl * 4;

		/* Determine the position of this fragment. */
		prevEnd = PktOffset + ntohs(iph->tot_len) - ihl;

		if(prevEnd > offset)
		{
			zlog_debug (FTM_DBG_IP,"%s, %d prevEnd:0x%x, offset:0x%x\n",__FUNCTION__, __LINE__,prevEnd, offset);
			return -1;
		}
	}

	listnode_add_next(pfraglist, prev, pkt);
	zlog_debug(FTM_DBG_IP,"%s, %d \n",__FUNCTION__, __LINE__);
	/* 重新设置该分片包的失效期限 */
	Node->timeout = 30;

	return 0;
}


/* See if a fragment queue is complete. */
static int ip_frag_list_complete(struct ip_frag_list_s *Node)
{
	struct list *pfraglist = NULL;
	struct listnode *tmp = NULL;
	struct pkt_buffer *pkttmp = NULL;
	struct iphdr *iph = NULL;
	int offset;
	int ihl, end;

	pfraglist = &(Node->fraglist);
	tmp = pfraglist->tail;
	pkttmp = (struct pkt_buffer *)(tmp->data);
	if(pkttmp == NULL)
	{
		zlog_debug (FTM_DBG_IP,"%s, %d pkttmp == NULL\n",__FUNCTION__, __LINE__);
		return -1;
	}

	iph = (struct iphdr *)pkttmp->network_header;
	offset = ntohs(iph->frag_off);

	/* 检查最后一个分片包MF 是否为 0 */
	if(offset & IP_FLAG_MF)
	{
		zlog_debug(FTM_DBG_IP,"%s, %d IP_MF, offset:0x%x fail!\n",__FUNCTION__, __LINE__, offset);
		return -1;
	}

	/* Check all fragment offsets to see if they connect. */
	pkttmp = (struct pkt_buffer *)listnode_head(pfraglist);
	if(pkttmp == NULL)
	{
		zlog_debug (FTM_DBG_IP,"%s, %d pkttmp == NULL\n",__FUNCTION__, __LINE__);
		return -1;
	}
	iph = (struct iphdr *)pkttmp->network_header;
	offset = ntohs(iph->frag_off);
	offset &= IP_FLAG_OFFSET;
	offset <<= 3;                   /* offset is in 8-byte chunks */
	ihl = iph->ihl * 4;
	end = offset + ntohs(iph->tot_len)-ihl;

	/* 链表内的首分片偏移必须是0 */
	if(offset != 0)
	{
		zlog_debug(FTM_DBG_IP,"%s, %d offset != 0\n",__FUNCTION__, __LINE__);
		return -1;
	}

	/* 遍历除头分片外的所有的分片，保证分片是连续的 */
	tmp = pfraglist->head;
	for (tmp = tmp->next; tmp != NULL; tmp = tmp->next)
	{
		pkttmp = (struct pkt_buffer *)(tmp->data);
		if(pkttmp == NULL)
		{
			zlog_debug (FTM_DBG_IP,"%s, %d pkttmp == NULL\n",__FUNCTION__, __LINE__);
			return -1;
		}
		iph = (struct iphdr *)pkttmp->network_header;
		offset = ntohs(iph->frag_off);
		offset &= IP_FLAG_OFFSET;
		offset <<= 3;                   /* offset is in 8-byte chunks */
		if(offset != end)
		{
			zlog_debug(FTM_DBG_IP,"%s, %d offset:0x%x\n",__FUNCTION__, __LINE__, offset);
			return -1;
		}
		end = offset + ntohs(iph->tot_len) - iph->ihl * 4;
	}

	/* 返回最后一个分片的大小，也就是整个报文的长度 */
	return end;
}


/* 重组所有的分片 */
static struct pkt_buffer *ip_frag_list_glue(struct ip_frag_list_s *Node, int totalLen)
{
	struct list *pfraglist = NULL;
	struct listnode *tmp = NULL;
	struct pkt_buffer *pkttmp = NULL;
	struct pkt_buffer *pkt = NULL;
	struct iphdr *iph = NULL;
	int ihl, dataLen;

	pfraglist = &(Node->fraglist);

	/* 避免分配的空间不满足报文填充 */
	dataLen = totalLen + IP_HEADER_SIZE;
	pkt = pkt_alloc(dataLen);
	if(!pkt)
	{
		zlog_debug (FTM_DBG_IP,"%s, %d pkt_alloc fail\n",__FUNCTION__, __LINE__);
		return NULL;
	}
	if(pkt->buf_size < pkt->data_len)
	{
		zlog_debug(FTM_DBG_IP,"%s, %d dataLen:0x%x, bufsize:0x%x pkt->buf_size < pkt->data_len \n",__FUNCTION__, __LINE__,dataLen, pkt->buf_size);
		goto drop;
	}

	/* 重组所有的分片包 */
	for (tmp = pfraglist->tail; tmp != NULL; tmp = tmp->prev)
	{
		pkttmp = (struct pkt_buffer *)(tmp->data);
		if(pkttmp == NULL)
		{
			zlog_debug (FTM_DBG_IP,"%s, %d pkttmp == NULL\n",__FUNCTION__, __LINE__);
			goto drop;
		}
		iph = (struct iphdr *)pkttmp->network_header;
		ihl = iph->ihl * 4;

		if(tmp->next == NULL)
		{
			/* tail分片处理，为了保持与普通报文一致，添加尾部的4自己校验 */
			dataLen = ntohs(iph->tot_len) - ihl;
			if(pkt_push(pkt, dataLen))
			{
				zlog_debug(FTM_DBG_IP,"%s, %d pkt_push(pkt, dataLen) \n",__FUNCTION__, __LINE__);
				goto drop;
			}

			memcpy((char *)pkt->data, (char *)pkttmp->network_header + ihl, dataLen);
		}
		else if(tmp->prev == NULL)
		{
			/* head片处理 */
			dataLen = ntohs(iph->tot_len);
			if(pkt_push(pkt, dataLen))
			{
				zlog_debug(FTM_DBG_IP,"%s, %d \n",__FUNCTION__, __LINE__);
				goto drop;
			}

			memcpy((char *)pkt->data, (char *)pkttmp->network_header, dataLen);
			iph = (struct iphdr *)pkt->data;
			iph->tot_len = htons(totalLen + ihl);
			iph->frag_off = 0;
			iph->check = 0x0;
			iph->check = in_checksum((u_int16_t *)iph, ihl);
			ip_decap(pkt);
			
			/*拷贝原有控制块*/
			memcpy(&(pkt->time), &(pkttmp->time), sizeof(struct time_stamp));
			pkt->inif_type = pkttmp->inif_type;
            pkt->in_ifindex = pkttmp->in_ifindex;
			pkt->out_ifindex = pkttmp->out_ifindex;
			pkt->in_port = pkttmp->in_port;
			pkt->vpn = pkttmp->vpn;
			pkt->protocol = pkttmp->protocol;
			pkt->svlan = pkttmp->svlan;
			pkt->cvlan = pkttmp->cvlan;
			memcpy(pkt->smac, pkttmp->smac, 6);
			memcpy(pkt->dmac, pkttmp->dmac, 6);

			//拷贝报文首次解析的控制内容，新解析的控制块内容不完整
			pkt->cb_type = pkttmp->cb_type;
			memcpy(&(pkt->cb), &(pkttmp->cb), PKT_CB_SIZE);

		}
		else
		{
			/* 其他片处理 */
			dataLen = ntohs(iph->tot_len) - ihl;
			if(pkt_push(pkt, dataLen))
			{
				zlog_debug(FTM_DBG_IP,"%s, %d tot_len:0x%x, dataLen:0x%x, ihl:0x%x,data_len:0x%x, bufsize:0x%x\n",
							__FUNCTION__, __LINE__,ntohs(iph->tot_len), dataLen, ihl,pkt->data_len, pkt->buf_size);
				goto drop;
			}

			memcpy((char *)pkt->data, (char *)pkttmp->network_header + ihl, dataLen);
		}
	}

	/* 删除所有的分片资源和节点 */
	ip_frag_list_free(Node);
	return pkt;
drop:
	zlog_debug(FTM_DBG_IP,"ip_frag_list_glue fail!\n");
	pkt_free(pkt);
	ip_frag_list_free(Node);
	return NULL;
}


/* 对超过 mtu 的报文进行分片 */
int ip_frag(struct ifm_info *p_ifinfo, struct pkt_buffer *pkt)
{
	struct iphdr *iph = NULL;
	struct pkt_buffer *frag_pkt  = NULL;
	int offset = 0;
	int i, realMtu, fragcnt, taillen;
	int mtu = p_ifinfo->mtu;

	/* frag_off必须是8Byte的整数倍 */
	realMtu = mtu - IP_HEADER_SIZE;
	realMtu = realMtu - realMtu % 8;
	fragcnt = (pkt->data_len-IP_HEADER_SIZE) / realMtu;
	taillen = (pkt->data_len-IP_HEADER_SIZE) % realMtu;
	if(taillen != 0)
	{
		fragcnt++;
	}
	else
		taillen = realMtu;
	zlog_debug(FTM_DBG_IP,"%s, %d  mtu:%d realMtu:%d, taillen:%d, fragcnt:%d\n",
		   			__FUNCTION__, __LINE__, mtu, realMtu, taillen, fragcnt);

	for(i = 0; i < fragcnt; i++)
	{
		frag_pkt = pkt_alloc(realMtu + IP_HEADER_SIZE);
		if(!frag_pkt)
		{
			zlog_debug(FTM_DBG_IP,"%s, %d ip_frag_list_lookup fail!\n",__FUNCTION__, __LINE__);
			return NET_FAILED;
		}
		offset = i * realMtu;

		/* 复制数据 */
		if(i == fragcnt - 1)
		{
			/* last fragment */
			if(pkt_push(frag_pkt, taillen))
			{
				pkt_free(pkt);
				zlog_debug (FTM_DBG_IP,"%s, %d pkt_push(frag_pkt, taillen)\n",__FUNCTION__, __LINE__);
				return NET_FAILED;
			}
			memcpy((char *)frag_pkt->data, (char *)pkt->data + IP_HEADER_SIZE + offset, taillen);
		}
		else
		{
			if(pkt_push(frag_pkt, realMtu))
			{
				pkt_free(pkt);
				zlog_debug (FTM_DBG_IP,"%s, %d pkt_push(frag_pkt, realMtu)\n",__FUNCTION__, __LINE__);
				return NET_FAILED;
			}
			memcpy((char *)frag_pkt->data, (char *)pkt->data + IP_HEADER_SIZE + offset, realMtu);
		}

		if(pkt_push(frag_pkt, IP_HEADER_SIZE))
		{
			pkt_free(pkt);
			zlog_debug (FTM_DBG_IP,"%s, %d pkt_push(frag_pkt, IP_HEADER_SIZE) \n",__FUNCTION__, __LINE__);
			return NET_FAILED;
		}
		memcpy((char *)frag_pkt->data, (char *)pkt->data, IP_HEADER_SIZE);

		memcpy((char *)&(frag_pkt->cb), (char *)&(pkt->cb), sizeof(union pkt_control));

		iph = (struct iphdr *)frag_pkt->data;

		if(i == fragcnt - 1)   /* 最后一片 */
		{
			iph->frag_off = 0;
			iph->tot_len = htons(taillen + IP_HEADER_SIZE);
		}
		else
		{
			iph->frag_off = IP_FLAG_MF;
			iph->tot_len = htons(realMtu + IP_HEADER_SIZE);
		}

		iph->frag_off += offset / 8;
		iph->frag_off = htons(iph->frag_off);
		iph->check = 0x0;
		iph->check = in_checksum((u_int16_t *)iph, IP_HEADER_SIZE);

		frag_pkt->out_ifindex = pkt->out_ifindex;
		frag_pkt->network_header = frag_pkt->data;
        frag_pkt->priority = pkt->priority;
		zlog_debug(FTM_DBG_IP,"%s, %d  frag_off:0x%x frag_pkt->data_len:0x%x\n",
			   			__FUNCTION__, __LINE__, iph->frag_off, frag_pkt->data_len);

		if(p_ifinfo->type == IFNET_TYPE_TUNNEL)
		{
			tunnel_xmit(frag_pkt);
		}
		else
			ip_xmit(frag_pkt);
	}

	pkt_free(pkt);
	return NET_SUCCESS;
}


/* 对分片报文进行重组，将一个报文的所有分片加入 fraglist，收集齐之后重组，返回重组后的报文 */
struct pkt_buffer *ip_defrag(struct pkt_buffer *pkt)
{
	int ret, totallen;
	struct ip_frag_list_s *Node = NULL;
	struct pkt_buffer *pktdefrag = NULL;

	Node = ip_frag_list_lookup(pkt);
	if(Node == NULL)
	{
		if(g_ipfraglistnum+1 > IP_FRAG_LIST_MAX)
		{
			zlog_debug(FTM_DBG_IP,"%s, %d ip_frag_list exceed limit! g_ipfraglistnum+1 > IP_FRAG_LIST_MAX \n",__FUNCTION__, __LINE__);
			goto drop;
		}
		else
		{
			Node = ip_frag_list_alloc();
            if(Node == NULL)
            {
                FTM_PKT_ZLOG_ERR();
                goto drop;
            }
		}
	}

	ret = ip_frag_list_add(Node, pkt);
	if(ret != 0)
	{
		zlog_debug(FTM_DBG_IP,"%s, %d ip_frag_list_add fail!\n",__FUNCTION__, __LINE__);
		goto drop;
	}

	/* 判断分片收发全都收到 */
	totallen = ip_frag_list_complete(Node);
	if(totallen > 0)
	{
		/* 重组报文 */
		pktdefrag = ip_frag_list_glue(Node, totallen);
		if(pktdefrag != NULL)
		{
			zlog_debug(FTM_DBG_IP,"%s, %d ip frag list glue complete!\n",__FUNCTION__, __LINE__);
			return pktdefrag;
		}
	}
  	return NULL;

drop:
	zlog_debug(FTM_DBG_IP,"ip_defrag get a error packet,drop it!\n");
	pkt_free(pkt);
	return NULL;
}


void ip_init(void)
{
	ip_frag_list_init();

	memset(&ip_retry_list, 0, sizeof(struct list));
}


