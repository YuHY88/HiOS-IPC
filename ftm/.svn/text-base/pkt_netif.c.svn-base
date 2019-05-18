/*
*          network interface receive and send the packet
*/
#include <string.h>
#include <lib/ether.h>
#include <lib/pkt_buffer.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/log.h>
#include "ifm/ifm.h"
#include "ftm_ifm.h"
#include "ftm_pkt.h"
#include "ftm_debug.h"
#include "pkt_eth.h"
#include "pkt_ip.h"
#include "pkt_mpls.h"
#include "proto_reg.h"
#include "ftm_arp.h"
#include "ftm.h"
#include "pkt_netif.h"


/*
指定出接口发送二层报文，支持以太口和 tunnel 口
*/
int netif_xmit(struct pkt_buffer *pkt)
{
	struct ftm_ifm *pifm = NULL;
	uint32_t ifindex;

	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_SEND, PKT_TYPE_ETH);
	ftm_pkt_cnt_add(PKT_ACTION_SEND, PKT_TYPE_ETH);

	ifindex = pkt->out_ifindex;

	pifm = ftm_ifm_lookup(ifindex);
	if(pifm == NULL)
	{
		zlog_err("ftm_ifm_lookup failed!\n");
		goto drop;
	}

    /* 根据接口类型走不同的转发流程 */
	switch (pifm->ifm.type)
	{
		case IFNET_TYPE_TUNNEL:      /* tunnel 接口 */
		{
			return tunnel_xmit(pkt);
		}
		case IFNET_TYPE_LOOPBACK:    /* loopback 接口 */
		{
			return ip_rcv_local(pkt);
		}
		case IFNET_TYPE_ETHERNET:	 /* 以太物理接口和子接口 */
		case IFNET_TYPE_GIGABIT_ETHERNET:
		case IFNET_TYPE_XGIGABIT_ETHERNET:
		case IFNET_TYPE_TRUNK:	     /* Trunk 接口 */		
		case IFNET_TYPE_VLANIF:      /* vlanif 接口 */
		{
			//ftm_pkt_send(pkt);
			ftm_pkt_send_n(pkt);
			pkt_free(pkt);
			return NET_SUCCESS;
		}
		default:
			break;
	}

drop:
	zlog_err("netif_xmit failed, drop the packet!\n");
	pkt_free(pkt);
	return NET_FAILED;
}


/* 指定入接口发送报文, 只支持 pw */
int netif_rcv(struct pkt_buffer *pkt)
{
	struct ftm_ifm *pifm = NULL;
	struct ftm_ifm_l2 *pl2if = NULL;
	struct mpls_control *pmpls_cb = NULL;
	union pkt_control cb;
	uint32_t index = 0;

	pifm = ftm_ifm_lookup(pkt->in_ifindex);
	if(pifm == NULL)
	{
		goto drop;
	}

	/* 将 cb 拷贝出来 */
	memset(&cb, 0, sizeof(union pkt_control));
	memcpy(&cb, &pkt->cb, PKT_CB_SIZE);

	if(pifm->pl2if) /* l2 接口走 pw 转发 */
	{
		pl2if = pifm->pl2if;
		if((pl2if->pw_backup == BACKUP_STATUS_MASTER) && (pl2if->master_index)) /* 主 pw 工作 */
		{
			index =  pl2if->master_index;
		}
		else if((pl2if->pw_backup == BACKUP_STATUS_SLAVE) && (pl2if->backup_index)) /* 备 pw 工作 */
		{
			index =  pl2if->backup_index;
		}

		if(index == 0)
			goto drop;

		pmpls_cb = (struct mpls_control *)(&(pkt->cb));
		memset(pmpls_cb, 0, PKT_CB_SIZE);
		pmpls_cb->ifindex = index;
		pmpls_cb->if_type = PKT_INIF_TYPE_PW;
		if(pkt->cb_type == PKT_TYPE_RAW)
		{
			pmpls_cb->exp = cb.rawcb.priority;
			pmpls_cb->ttl = cb.rawcb.ttl;
		}
		else
			goto drop;

		pmpls_cb->is_changed = 1;
		pkt->cb_type = PKT_TYPE_MPLS;
		return mpls_forward(pkt);
	}
	else
		goto drop;

	return 0;

drop:
	zlog_err("tunnel_xmit failed, drop it!\n");
	pkt_free(pkt);
	return NET_FAILED;
}


/* 报文转发到 tunnel */
int tunnel_xmit(struct pkt_buffer *pkt)
{
	struct ftm_ifm *pifm = NULL;
	struct tunnel_t *ptunnel = NULL;
	struct raw_control *raw_cb = NULL;
	struct mpls_control *mpls_cb = NULL;
	struct ip_control *ipcb = NULL;
	uint32_t index;
	uint8_t  ttl = 255;
	int ret;

	pifm = ftm_ifm_lookup(pkt->out_ifindex);
	if((pifm == NULL) || (pifm->ptunnel == NULL))
	{
		goto drop;
	}

	ptunnel = pifm->ptunnel;
	switch(ptunnel->protocol)
	{
		case TUNNEL_PRO_MPLSTP:
		case TUNNEL_PRO_MPLSTE:
		{
			if(ptunnel->backup_status == TUNNEL_STATUS_MASTER)
				index = ptunnel->master_index;
			else if(ptunnel->backup_status == TUNNEL_STATUS_BACKUP)
				index = ptunnel->backup_index;
			else
				index = ptunnel->frr_index;

			if(index == 0)
			{
				goto drop;
			}

			mpls_cb = (struct mpls_control *)(&(pkt->cb));
			if(pkt->cb_type != PKT_TYPE_MPLS) /* 内层不是 mpls 报文 */
			{
				if(pkt->cb_type == PKT_TYPE_RAW)/* ping tunnel */
				{
					raw_cb = (struct raw_control *)(&(pkt->cb));
					ttl = raw_cb->ttl;
				}
				
				memset(mpls_cb, 0, PKT_CB_SIZE);
				mpls_cb->ttl = ttl;
			}

			if(mpls_cb->chtype == MPLS_CTLWORD_CHTYPE_8902  || mpls_cb->chtype == MPLS_CTLWORD_CHTYPE_7FFA)  /* mpls-tp oam 报文 */
			{
				mpls_cb->ttl = ttl;
			}
			
			mpls_cb->exp = pkt->priority;
			mpls_cb->ifindex = index;
			mpls_cb->if_type = PKT_INIF_TYPE_LSP;
			return mpls_forward(pkt);
		}
		case TUNNEL_PRO_GRE:
		{
			ret = gre_encap(pkt);  /* 封装 GRE 头 */
			if(ret)
				goto drop;

            /* 获取 raw 发包的 ttl */
			if(pkt->cb_type == PKT_TYPE_RAW)/* ping tunnel */
			{
				raw_cb = (struct raw_control *)(&(pkt->cb));
				ttl = raw_cb->ttl;
			}

			/* 设置 IP 控制信息，走 IP 转发流程 */			
			ipcb = (struct ip_control *)(&(pkt->cb));
            memset(ipcb, 0, sizeof(struct ip_control));
			ipcb->sip = ptunnel->sip.addr.ipv4;
			ipcb->dip = ptunnel->dip.addr.ipv4;
			ipcb->pkt_type = PKT_TYPE_IP;
			ipcb->protocol = IP_P_GRE;
			ipcb->tos = pkt->priority;
			ipcb->ttl = ttl;
            ipcb->is_changed = 1;
			return ip_forward(pkt);
		}
		default:
			goto drop;
	}

drop:
	zlog_err("tunnel_xmit failed, drop it!\n");
	pkt_free(pkt);
	return NET_FAILED;
}


