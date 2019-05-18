/*
*          ether interface receive and send the packet
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
#include "pkt_ipv6.h"
#include "pkt_mpls.h"
#include "pkt_netif.h"
#include "proto_reg.h"
#include "pkt_arp.h"
#include "ftm.h"
#include "ftm_vlan.h"

/* �������� CPU ��Э�鱨�� */
int eth_rcv_local(struct pkt_buffer *pkt)
{
	struct eth_proto proto;
	struct eth_control *eth_cb = NULL;
	int module_id;

	eth_cb = (struct eth_control *)(&(pkt->cb));
	memset(&proto, 0, sizeof(struct eth_proto));
	memcpy(proto.dmac, eth_cb->dmac, 6);
	memcpy(proto.smac, eth_cb->smac, 6);
	proto.vlan = eth_cb->svlan;
	proto.ethtype = eth_cb->ethtype;
	proto.sub_ethtype = eth_cb->sub_ethtype;

	/*find the registered proto node*/
	module_id = eth_proto_lookup(&proto);
	if(module_id)
	{
		//ftm_pkt_send_to(pkt, module_id);/* send to app */
		ftm_pkt_send_to_n(pkt, module_id);/* send to app */
		pkt_free(pkt);
	    return 0;
	}

	FTM_PKT_ZLOG_DBG("eth_rcv_local failed, drop it!\n");
	pkt_free(pkt);
	return NET_FAILED;
}


/* ���� eth ���ģ����� eth ͷ������ eth �����ͷ */
int eth_rcv(struct pkt_buffer *pkt)
{
	struct ftm_ifm *pifm = NULL;
	uint16_t tpid = ETH_P_8021Q;
	struct eth_control *pethcb = NULL;
	uint32_t trunk_ifindex = 0;
	uint32_t vlanif_ifindex = 0;
	uint32_t ifindex = 0;

	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_RECV, PKT_TYPE_ETH);
	ftm_pkt_cnt_add(PKT_ACTION_RECV, PKT_TYPE_ETH);

	/* packet rcv from a real interface, ����ӿڻ����ӽӿ� */
    if(pkt->inif_type == PKT_INIF_TYPE_IF)
    {
        /* ��ȡ����ڵ� ifindex */
		if(pkt->in_ifindex)
		{
			if(IFM_IS_SUBPORT(pkt->in_ifindex))
			{
				ifindex = IFM_PARENT_IFINDEX_GET(pkt->in_ifindex);
			}
			else
			{
				ifindex = pkt->in_ifindex;
			}
		}
        else if(pkt->in_port)
        {
			ifindex = pkt->in_port;
        }

	    if(ifindex == 0)
		{
            FTM_PKT_ZLOG_DBG("ifindex error!\n");
            goto drop;
        }

		/* ��ȡ����ӿڵ� tpid */
		pifm = ftm_ifm_lookup(ifindex);
		if ((pifm == NULL) || (pifm->ifm.shutdown == IFNET_SHUTDOWN))
		{
            FTM_PKT_ZLOG_DBG("intf shutdown or pifm is NULL!\n");
			goto drop;
		}

		if((pifm->pparent)&&(pifm->pparent->ifm.type == IFNET_TYPE_TRUNK))
		{
			/* trunk ��Ա�ڻ�ȡ trunk �� tpid �� ifindex */
		    trunk_ifindex = pifm->pparent->ifm.ifindex;
			tpid = pifm->pparent->ifm.tpid;
			pifm = pifm->pparent;
		}
		else
		{
			tpid = pifm->ifm.tpid;
		}
    }

	if((pkt->inif_type == PKT_INIF_TYPE_PW)       /* receive from pw */
      ||(pkt->inif_type == PKT_INIF_TYPE_TUNNEL)  /* receive from tunnel */
      ||(pkt->inif_type == PKT_INIF_TYPE_LSP)  /* receive from lsp */
	  ||(pkt->inif_type == PKT_INIF_TYPE_IF))     /* receive from real interface */
	{
		/* decapsulate the eth header */
		if(pkt->cb_type != PKT_TYPE_ETH)
		{
			if(eth_decap(pkt, tpid))
			{
				goto drop;
			}
		}

		pethcb = (struct eth_control *)(&(pkt->cb));

        /* ���Դ mac �Ϸ��� */
        if((ether_is_broadcast_mac(pethcb->smac) == 0)
			||(ether_is_muticast_mac(pethcb->smac) == 0)
			||(ether_is_zero_mac(pethcb->smac) == 0))
        {
            FTM_PKT_ZLOG_DBG("smac format error, drop it!!\n");
			goto drop;
        }

		if(pifm)
		{
		    /* ��ȡ vlanif �ӿڵ� ifindex */
		    if((pethcb->svlan) && (pethcb->cvlan == 0)
				&& (pifm->ifm.mode == IFNET_MODE_SWITCH))
		    {
				vlanif_ifindex = ftm_vlan_get_vlanif(pethcb->svlan);
		    }

		    if((vlanif_ifindex) && (trunk_ifindex)) /* trunk ���� vlanif ����� */
		    {
				pkt->in_ifindex = vlanif_ifindex;
				pkt->in_port = trunk_ifindex;
		    }
		    else if(trunk_ifindex)
		    {
				pkt->in_ifindex = trunk_ifindex;
		    }
			else if(vlanif_ifindex)
			{
				pkt->in_ifindex = vlanif_ifindex;
			}

			FTM_PKT_ZLOG_DBG( "ifindex:0x%x,pkt->in_port:0x%x,pkt->in_ifindex:0x%x,trunk_ifindex:0x%x, vlanif_ifindex:0x%x!\n",
                                                        ifindex, pkt->in_port, pkt->in_ifindex, trunk_ifindex, vlanif_ifindex );

            /* ��ȡ�߼��ӿ� */
			if(pkt->in_ifindex != ifindex)
			{
				pifm = ftm_ifm_lookup(pkt->in_ifindex);
				if(pifm == NULL)
				{
                    FTM_PKT_ZLOG_DBG("Pifm is NULL!\n");
					goto drop;
				}

				if((IFM_IS_SUBPORT(pkt->in_ifindex))&&(pifm->ifm.status == IFNET_LINKDOWN))
				{
					FTM_PKT_ZLOG_DBG("Error:interface not exist or interface shutdown,drop it!\n");
					goto drop;
				}
			}

			/* ��ȡ vpn */
			if(pifm->pl3if)
			{
				pkt->vpn = pifm->pl3if->vpn;
			}
		}

		if(pethcb->untag == 1)
		{
			pethcb->svlan = 0;
		}
		
		if(pkt->protocol == ETH_P_MPLS_UC)
		{
			return mpls_rcv(pkt); /* mpls ���� */
		}
	    else if(pkt->protocol == ETH_P_IP)
	    {
			return ip_rcv(pkt);  /* ipv4 ���� */
	    }
	    else if(pkt->protocol == ETH_P_IPV6)
	    {
			return ipv6_rcv(pkt);  /* ipv6 ���� */
	    }
		else if(pkt->protocol == ETH_P_ARP)
		{
			return arp_rcv(pkt, pifm); /* arp ���� */
		}
		else
		{
			return eth_rcv_local(pkt);/* ��̫���Ļ� ISIS Э�鱨�� */
		}
	}

drop:
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_ETH);
	pkt_free(pkt);
	return NET_FAILED;
}


/* �ȷ�װ eth ͷ��Ȼ��ָ�����ӿڷ��ͱ��� */
int eth_forward(struct pkt_buffer *pkt)
{
	struct eth_control *eth_cb = NULL;
	struct ftm_ifm *pifm = NULL;

	eth_cb = (struct eth_control *)(&(pkt->cb));

	pifm = ftm_ifm_lookup(eth_cb->ifindex);
	if(pifm == NULL)
	{
		FTM_PKT_ZLOG_DBG("ftm_ifm_lookup failed! ifindex:0x%x\n",eth_cb->ifindex);
		goto drop;
	}

    /* ��ȡ smac �� vlan ��װ��Ϣ*/
	if((eth_cb->smac_valid == 0) && (eth_cb->is_changed == 1))
	{
		/* MAC �� tpid ��Ҫ������ӿڻ�ȡ, vlan ���߼��ӿڻ�ȡ */
		if(pifm->ifm.type == IFNET_TYPE_VLANIF)
		{
		    if(eth_cb->svlan == 0)
				eth_cb->svlan = IFM_VLANIF_ID_GET(pifm->ifm.ifindex);
			eth_cb->svlan_tpid = ETH_P_8021Q;
			memcpy(eth_cb->smac, pifm->ifm.mac, MAC_LEN);
		}
		else if((pifm->pparent) && (IFM_IS_SUBPORT(pifm->ifm.ifindex)))/* �ӽӿ� */
		{
			/*�ӽӿ�δ��װʱ��״̬ΪIFNET_LINKDOWN����Ӧ����*/
			if(pifm->ifm.status == IFNET_LINKDOWN)
			{
				FTM_PKT_ZLOG_DBG("Error:sub_interface not encap ,IFNET_LINKDOWN!\n");
				goto drop;
			}
			eth_cb->svlan = pifm->ifm.encap.svlan.vlan_start;
			eth_cb->cvlan = pifm->ifm.encap.cvlan.vlan_start;
			eth_cb->cvlan_tpid = ETH_P_8021Q;
			eth_cb->svlan_tpid = pifm->pparent->ifm.tpid;
			memcpy(eth_cb->smac, pifm->pparent->ifm.mac, MAC_LEN);
		}
		else         /* ����ӿں� trunk �ӿ� */
		{
			eth_cb->svlan_tpid = pifm->ifm.tpid;
			memcpy(eth_cb->smac, pifm->ifm.mac, MAC_LEN);
		}

		eth_cb->smac_valid = 1;
	}

	/* ��װ��̫ͷ */
	if(eth_encap(pkt))
	{
		FTM_PKT_ZLOG_DBG("Eth_encap fail!\n");
		goto drop;
	}

	if( pkt->data_len < MIN_PKT_SIZE )
	{
		pkt->data_len += (MIN_PKT_SIZE - pkt->data_len);
	}

	pkt->out_ifindex = eth_cb->ifindex;
	pkt->priority = eth_cb->cos;

	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_SEND, PKT_TYPE_ETH);
	ftm_pkt_cnt_add(PKT_ACTION_SEND, PKT_TYPE_ETH);

	/* send to xmit queue */
	//ftm_pkt_send(pkt);
	ftm_pkt_send_n(pkt);
	pkt_free(pkt);
	return 0;

drop:
	pkt_free(pkt);
	return NET_FAILED;
}


/* add eth header to payload */
int eth_encap(struct pkt_buffer *pkt)
{
	struct ethhdr *eth_head = NULL;
	struct eth_control *eth_cb = NULL;
	int hlen = ETH_HLEN;

    /* get buffer for the eth header */
	eth_cb = (struct eth_control *)(&(pkt->cb));
	if(eth_cb->is_changed == 0)
	{
		if(pkt_push(pkt, ((uint32_t)pkt->data - (uint32_t)pkt->link_header))) /*recover the eth header */
		{
			FTM_PKT_ZLOG_DBG();
            return -1;
		}
		return 0;
	}
	
    eth_cb->is_changed = 0;

	if(eth_cb->svlan)
		hlen += VLAN_HLEN;
	if(eth_cb->cvlan)
		hlen += VLAN_HLEN;
	if(pkt_push(pkt, hlen))
	{
		FTM_PKT_ZLOG_DBG();
        return -1;
	}
	pkt->link_header = pkt->data;

	/* set the eth header */
	eth_head = (struct ethhdr *)(pkt->link_header);
	memcpy(eth_head->h_source, eth_cb->smac,6);
	memcpy(eth_head->h_dest, eth_cb->dmac,6);
	eth_head->h_ethtype = htons(eth_cb->ethtype);

	if(eth_cb->svlan) /* dot1q frame */
	{
		struct vlan_ethhdr *vethdr = (struct vlan_ethhdr *)(pkt->link_header);
		if(eth_cb->svlan_tpid)
			vethdr->h_tpid = htons(eth_cb->svlan_tpid);
		else
			vethdr->h_tpid = htons(ETH_P_8021Q_STAG);
		vethdr->h_vlan_tci = htons(eth_cb->svlan + (eth_cb->cos << 13));
		vethdr->h_ethtype = htons(eth_cb->ethtype);
	}

	if(eth_cb->cvlan) /* qinq frame */
	{
	    struct qinq_ethhdr *qethdr = (struct qinq_ethhdr *)(pkt->link_header);
		if(eth_cb->cvlan_tpid)
			qethdr->h_cvlan_tpid = htons(eth_cb->cvlan_tpid);
		else
			qethdr->h_cvlan_tpid = htons(ETH_P_8021Q);
		qethdr->h_cvlan_tci = htons(eth_cb->cvlan + (eth_cb->cos << 13));
		qethdr->h_ethtype = htons(eth_cb->ethtype);
	}

    pkt->protocol = eth_cb->ethtype;
	pkt->priority = eth_cb->cos;

	return 0;
}


/*offset eth header to get eth_cb */
int eth_decap(struct pkt_buffer *pkt, uint16_t tpid)
{
	struct ethhdr *eth_head = NULL;
	struct eth_control *eth_cb = NULL;
	uint16_t proto;
	uint16_t vlan_tci;
	int hlen;

    /* get mac header and offset the header */
    pkt->link_header = pkt->data;
    eth_head = (struct ethhdr *)(pkt->link_header);
	hlen = ETH_HLEN;

    /* set  the packet control block */
    memset(&(pkt->cb), 0, PKT_CB_SIZE);
	pkt->cb_type = PKT_TYPE_ETH;
    eth_cb = (struct eth_control *)(&(pkt->cb));
    memcpy(eth_cb->smac, eth_head->h_source, 6);
    memcpy(eth_cb->dmac, eth_head->h_dest, 6);

    proto = ntohs(eth_head->h_ethtype);
	eth_cb->ethtype = proto;

    if((proto == tpid) || (proto == ETH_P_8021Q) || (proto == ETH_P_8021Q_STAG)) /*svlan exist */
    {
	  struct vlan_ethhdr *vethdr = (struct vlan_ethhdr *)(pkt->link_header);
	  vlan_tci = ntohs(vethdr->h_vlan_tci);
	  eth_cb->svlan = vlan_tci & VLAN_VID_MASK;  /* get vlan */
	  eth_cb->cos = (vlan_tci & VLAN_PRIO_MASK) >> VLAN_PRIO_SHIFT;
	  eth_cb->svlan_tpid = ntohs(vethdr->h_tpid); /* get tpid */
	  eth_cb->ethtype = ntohs(vethdr->h_ethtype);

	  hlen += VLAN_HLEN;

	  proto = ntohs(vethdr->h_ethtype);
	  if(proto == ETH_P_8021Q) /*cvlan exist */
	  {
		  struct qinq_ethhdr *qethdr = (struct qinq_ethhdr *)(pkt->link_header);
		  vlan_tci = ntohs(qethdr->h_cvlan_tci);
		  eth_cb->cvlan = vlan_tci & VLAN_VID_MASK;         /* get cvlan */
		  eth_cb->cvlan_tpid = ntohs(qethdr->h_cvlan_tpid); /* get cvlan tpid */
		  eth_cb->ethtype = ntohs(qethdr->h_ethtype);
		  hlen += VLAN_HLEN;
      }
    }

	pkt->priority = eth_cb->cos;
	pkt->protocol = eth_cb->ethtype;
    if(pkt_pull(pkt, hlen))  /* offset the vlan and eth header */
    {
		zlog_debug(FTM_DBG_IP,"%s, %d pkt_pull fail!\n",__FUNCTION__, __LINE__);
        return -1;
    }

	/* С�� 1536 ��ʾ���ȣ�llc ����ƫ�� 2 �ֽ� */
    if ( eth_cb->ethtype < ETH_P_LEN )
    {
        /* ��Ҫƫ�� 2 �ֽڻ�ȡ ethtype��subtype */
        eth_cb->ethtype = ntohs(* ( uint16_t * ) ( pkt->data ));
        eth_cb->sub_ethtype = * ( ( char * ) ( pkt->data ) + 2 );
        pkt->protocol = eth_cb->ethtype;
    }
    else
    {
        /* ��Э����Ҫ��ȡ subtype */
		eth_cb->sub_ethtype = *(char *)(pkt->data);
    }

    return 0;
}


