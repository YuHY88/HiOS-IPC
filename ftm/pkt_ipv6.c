/*
*           process ipv6 packet forward
*/

#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/route_com.h>
#include <lib/ether.h>
#include <lib/inet_ip.h>
#include <lib/zassert.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/prefix.h>
#include <lib/checksum.h>
#include <string.h>
#include "ftm_pkt.h"
#include "ftm_arp.h"
#include "ftm_ndp.h"
#include "pkt_ip.h"
#include "pkt_ipv6.h"
#include "pkt_eth.h"
#include "pkt_icmp.h"
#include "pkt_tcp.h"
#include "pkt_udp.h"
#include "pkt_ndp.h"
#include "pkt_mpls.h"
#include "proto_reg.h"
#include "ftm.h"
#include "ftm_ifm.h"
#include "ftm_debug.h"


extern struct list ip_retry_list;     /* �ش��������� */
extern struct thread *ip_retry_timer; /* �ش���ʱ�� */


/*
���� IPv6 ���ģ���ȡ IP ����Ϣ���� IP ����ͷ
*/
int ipv6_rcv(struct pkt_buffer *pkt)
{
	struct ipv6_hdr *iph = NULL;
	struct ip_control *ipcb = NULL;

	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_RECV, PKT_TYPE_IP);
	ftm_pkt_cnt_add(PKT_ACTION_RECV, PKT_TYPE_IP);

	/*analyze the ip header to get ipcb and iph */
	if (ipv6_decap(pkt))
	{
        FTM_PKT_ZLOG_ERR("ip_decap error, drop it!!\n");
		goto drop;
	}

	iph = (struct ipv6_hdr *)pkt->network_header;
	ipcb = (struct ip_control *)(&(pkt->cb));
	ipcb->pkt_type = PKT_TYPE_IPV6;

    /* ��ȡ vpn */
	ipcb->vpn = pkt->vpn;

	/* ���Դ IP */
	if((ipv6_is_multicast(&ipcb->sipv6))       /* SIP ���鲥 */
		|| (ipv6_is_loopback(&ipcb->sipv6))    /* SIP �� loopback ��ַ */
		|| (ipv6_is_zero(&ipcb->sipv6))        /* sip �� 0 */
	  )
	{
        FTM_PKT_ZLOG_ERR("Pkt sip6 invalid,drop it!\n");
		goto drop;
	}

	/* ���Ŀ�� IP */
	if(ipv6_is_zero(&ipcb->dipv6))             /* dip �� 0  */
	{
        FTM_PKT_ZLOG_ERR("Pkt dip6 invalid,drop it!\n");
		goto drop;
	}
	else if( ipv6_is_loopback(&ipcb->dipv6)     /* DIP �� loopback ��ַ */
			|| ipv6_is_multicast(&ipcb->dipv6)  /* DIP ���鲥 */
			|| ipv6_is_linklocal(&ipcb->dipv6)  /* DIP ��·���ص�ַ */			
			|| ipv6_is_sitelocal(&ipcb->dipv6) )/* DIP վ�㱾�ص�ַ */
	{
		return ip_rcv_local(pkt);
	}
	else      /* IPV6 �������� */
	{
		iph->ttl--; 
		ipcb->ttl = iph->ttl;
		ipcb->is_changed = 1;
		return ip_forward(pkt);
	}

drop:	
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
	pkt_free(pkt);
	return NET_FAILED;
}


/* ��װ����ͷ, ���� IPv6 ���� */
int ipv6_xmit(struct pkt_buffer *pkt)
{
	struct ipv6_hdr *iph = NULL;
	struct eth_control *ethcb = NULL;
	struct ip_control *ipcb = NULL;

	iph = (struct ipv6_hdr *)pkt->network_header;
	ipcb = (struct ip_control *)(&(pkt->cb));

	/* ���Ŀ�� IP */
	if( ipv6_is_zero(&ipcb->dipv6)           /* �Ƿ� IP ��ַ */
	    ||(ipv6_is_loopback(&ipcb->dipv6))   /* DIP �� loopback ��ַ */
      )
	{
	    FTM_PKT_ZLOG_ERR("Pkt dip6 invalid,drop it!\n");
        goto drop;
	}
	else if(ipv6_is_multicast(&ipcb->dipv6))
	{
		ipcb->pkt_type = PKT_TYPE_IPV6MC;
	}
	
	if(ipcb->pkt_type == PKT_TYPE_IPV6)   /* IPV6 ���� */
	{
		 return ipv6_xmit_unicast(pkt);
	}

    /* �鲥����ֱ�ӷ��� */
	if(ipcb->ifindex == 0)
		goto drop;
    pkt->priority = ipcb->tos;
	pkt->out_ifindex = ipcb->ifindex;

	/* ������·���װ��Ϣ */
	ethcb = (struct eth_control *)(&(pkt->cb));
	memset(ethcb, 0, PKT_CB_SIZE);
	pkt->cb_type = PKT_TYPE_ETH;
	ethcb->ethtype = ETH_P_IPV6;
	ethcb->cos = pkt->priority;
	ethcb->ifindex = pkt->out_ifindex;
	ether_get_ipv6_muticast_mac(&iph->daddr, ethcb->dmac);  /* ��װ�ಥipv6 MAC ӳ���ַ*/
	ethcb->is_changed = 1;
	eth_forward(pkt);
	return ERRNO_SUCCESS;

drop:
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
	pkt_free(pkt);
	return NET_FAILED;
}


/* �������Ĳ� ND ��ȡ��̫��װ��Ϣ */
int ipv6_xmit_unicast(struct pkt_buffer *pkt)
{
	struct ip_control   *ipcb = NULL;
	struct eth_control  *ethcb = NULL;
	struct ndp_neighbor *pnd = NULL;
    struct ipv6_addr     dip6_addr;
    struct icmpv6_hdr   *icmpv6hdr = NULL;

	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_SEND, PKT_TYPE_IP);
	ftm_pkt_cnt_add(PKT_ACTION_SEND, PKT_TYPE_IP);

	ipcb = (struct ip_control *)(&(pkt->cb));
	if(ipv6_is_zero((struct ipv6_addr *)(ipcb->nexthop.addr.ipv6)))
		IPV6_ADDR_COPY(ipcb->nexthop.addr.ipv6, &ipcb->dipv6);
    
    /*save dip6_addr for packet which dip6 is  linklocal*/
    memset (&dip6_addr, 0, sizeof(struct ipv6_addr));
    IPV6_ADDR_COPY(&dip6_addr, &ipcb->dipv6);

	pnd = ndp_lookup_active((struct ipv6_addr *)(ipcb->nexthop.addr.ipv6), ipcb->vpn, ipcb->ifindex);

    /*only used by ndp unicast probe;ndp probe pkt not add into ip_retry_list*/
    if ((NULL == pnd) && (ipcb->protocol == IP_P_IPV6_ICMP) )
    {
        icmpv6hdr = (void *)((uint32_t)(pkt->network_header) + IPV6_HEADER_SIZE);
        if ((icmpv6hdr) && (icmpv6hdr->type == NDP_NEIGHBOR_SOLICIT))
        {
	        pnd = ndp_lookup(ipcb->nexthop.addr.ipv6, ipcb->vpn, ipcb->ifindex);
            if (!pnd || pnd->status != NDP_STATUS_PROBE)
            {
                return ERRNO_FAIL;
            }
        }
    }
	
    if((NULL == pnd) )
	{
		if((pkt->retry_cnt == 0) && (ip_retry_list.count < PKT_RETRY_LIST_LEN))
		{
			listnode_add(&ip_retry_list, pkt);	/* ���ļ����ش����� */
			if(ip_retry_timer == NULL)
				ip_retry_timer = high_pre_timer_add("FtmPktXmitRetry", LIB_TIMER_TYPE_NOLOOP, ip_xmit_retry, NULL, 500);
				//ip_retry_timer = thread_add_timer_msec(ftm_master, ip_xmit_retry, 0, 500); /* 500ms �ش�*/
			return ERRNO_SUCCESS;//�˷���ֵӰ���ش�
		}
		else if(ip_retry_list.count >= PKT_RETRY_LIST_LEN)
		{
            FTM_PKT_ZLOG_DBG("Ip_xmit_retry , drop the packet!\n");
			goto drop;
		}
        else
		{
            FTM_PKT_ZLOG_DBG("Ip_xmit_retry , drop the packet!\n");
		}
		return ERRNO_FAIL;//�˷���ֵӰ���ش���tcp 
	}

	/* ������·���װ��Ϣ */
	ethcb = (struct eth_control *)(&(pkt->cb));
	memset(ethcb, 0, PKT_CB_SIZE);
	pkt->cb_type = PKT_TYPE_ETH;
	ethcb->ethtype = ETH_P_IPV6;
	ethcb->cos = pkt->priority;
	ethcb->ifindex = pkt->out_ifindex;

	/* vlanif �ӿ� */
	if(pnd->port)
	{
		ethcb->ifindex = pnd->port;		
	    ethcb->svlan = IFM_VLANIF_ID_GET(pnd->key.ifindex);
	}
	else if(!ipv6_is_linklocal(&dip6_addr))//if dip6 is linklocal,packet will be send use specified outif
	{
		ethcb->ifindex = pnd->key.ifindex;
	}
    
	
	memcpy((char *)ethcb->dmac, (char *)pnd->mac, 6);
	ethcb->is_changed = 1;
	eth_forward(pkt);
	return ERRNO_SUCCESS;

drop:
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_IP);
	pkt_free(pkt);
	return NET_FAILED;
}


/* add ipv6 header to transport data */
int ipv6_encap(struct pkt_buffer *pkt)
{
	struct ipv6_hdr *iph = NULL;
	struct ip_control *ipcb = NULL;
	struct udphdr *udph = NULL;
	struct tcphdr *tcph = NULL;
	struct icmpv6_hdr *icmpv6 = NULL;
	struct pseudo_ipv6hdr *ppseudo = NULL;
	static uint16_t id = 0;
	uint16_t size = 0;

	ipcb = (struct ip_control *)(&(pkt->cb));
 	if(ipcb->is_changed == 0)
 	{
		return pkt_push(pkt, ((uint32_t)pkt->data - (uint32_t)pkt->network_header)); /*recover the ip header */
 	}
	else
	 	ipcb->is_changed = 0;

	size = pkt->data_len;  /* �ȼ�¼���ݳ��� */

    /* set ip header */
    pkt->transport_header = pkt->data;

	/* ��װ�µ�ipͷ*/
	if(pkt_push(pkt, sizeof(struct ipv6_hdr)))
    {
		return -1;
    }
	pkt->network_header = pkt->data;


	if(ipcb->chsum_enable)
	{
		if(ipcb->protocol == IP_P_IPV6_ICMP) /* ���� icmpv6 �� checksum */
		{
			/* ƫ�Ƴ�α�ײ�, ���� checksum */
	        ppseudo = (struct pseudo_ipv6hdr *)((uchar *)pkt->data + (IPV6_HEADER_SIZE - IPV6_PSEUDO_HDR_SIZE));
			IPV6_ADDR_COPY(&ppseudo->saddr, &ipcb->dipv6);
			IPV6_ADDR_COPY(&ppseudo->daddr, &ipcb->sipv6);
			ppseudo->placeholder = 0;
			ppseudo->protocol = IP_P_IPV6_ICMP;
			ppseudo->length = htons(size);			
			icmpv6 = (struct icmpv6_hdr *)(pkt->transport_header);
			icmpv6->checksum = 0;
			icmpv6->checksum = in_checksum((uint16_t *)ppseudo, IPV6_PSEUDO_HDR_SIZE + size);
            FTM_PKT_ZLOG_DBG("icmpv6->checksum:0x%x,pkt->data_len:0x%x\n",icmpv6->checksum, size);
		}
		else if(ipcb->protocol == IP_P_UDP) /* ���� UDP �� checksum */
		{
			/* ƫ�Ƴ�α�ײ�, ���� checksum */
	        ppseudo = (struct pseudo_ipv6hdr *)((uchar *)pkt->data + (IPV6_HEADER_SIZE - IPV6_PSEUDO_HDR_SIZE));
			IPV6_ADDR_COPY(&ppseudo->saddr, &ipcb->dipv6);
			IPV6_ADDR_COPY(&ppseudo->daddr, &ipcb->sipv6);
			ppseudo->placeholder = 0;
			ppseudo->protocol = IP_P_UDP;
			ppseudo->length = htons(size);			
			udph = (struct udphdr *)(pkt->transport_header);
			udph->checksum = 0;
			udph->checksum = in_checksum((uint16_t *)ppseudo, IPV6_PSEUDO_HDR_SIZE + size);
			FTM_PKT_ZLOG_DBG("udph->checksum:0x%x,pkt->data_len:0x%x\n",udph->checksum, size);
		}
		else if(ipcb->protocol == IP_P_TCP) /* ���� TCP �� checksum */
		{
			/* ƫ�Ƴ�α�ײ�, ���� checksum */
	        ppseudo = (struct pseudo_ipv6hdr *)((uchar *)pkt->data + (IPV6_HEADER_SIZE - IPV6_PSEUDO_HDR_SIZE));
			IPV6_ADDR_COPY(&ppseudo->saddr, &ipcb->dipv6);
			IPV6_ADDR_COPY(&ppseudo->daddr, &ipcb->sipv6);
			ppseudo->placeholder = 0;
			ppseudo->protocol = IP_P_TCP;
			ppseudo->length = htons(size);
			tcph = (struct tcphdr *)(pkt->transport_header);
			tcph->checksum = 0;
			tcph->checksum = in_checksum((uint16_t *)ppseudo, IPV6_PSEUDO_HDR_SIZE + size);
			FTM_PKT_ZLOG_DBG("tcph->checksum:0x%x,pkt->data_len:0x%x\n",tcph->checksum, size);
		}
	}
	
	iph = (struct ipv6_hdr *)pkt->network_header;
	memset(iph, 0, IPV6_HEADER_SIZE);
	iph->protocol =	ipcb->protocol;
	IPV6_ADDR_COPY(&iph->daddr, &ipcb->dipv6);
	IPV6_ADDR_COPY(&iph->saddr, &ipcb->sipv6);

	iph->data_len  =	htons(pkt->data_len - IPV6_HEADER_SIZE);
	iph->version  =	6;
	iph->tos1	  =	ipcb->tos;
	iph->ttl	  =	ipcb->ttl;
    iph->flabel2  = htons(id++);

    pkt->cb_type = PKT_TYPE_IPV6;
    pkt->protocol = ETH_P_IPV6;
	return 0;
}


/*offset ipv6 header to get ip_cb */
int ipv6_decap(struct pkt_buffer *pkt)
{
	struct ipv6_hdr *iph = NULL;
	struct ip_control *ipcb = NULL;

	pkt->network_header = pkt->data;  /* set ip header pointer */
	iph = (struct ipv6_hdr *)(pkt->network_header); /* get ip header */

	/*check the version */
	if (iph->version != 6)
	{
		FTM_PKT_ZLOG_ERR();
        return -1;
	}

    if(pkt_pull(pkt, IPV6_HEADER_SIZE)) /* offset the ip header */
	{
		return -1;
	}

	/* ip payload length */
	pkt->data_len = ntohs(iph->data_len);

    /* set ip cb */
    ipcb = (struct ip_control *)(&(pkt->cb));
    memset(ipcb, 0 , PKT_CB_SIZE);
    pkt->cb_type = PKT_TYPE_IPV6;
	IPV6_ADDR_COPY(&ipcb->sipv6, &iph->saddr);
	IPV6_ADDR_COPY(&ipcb->dipv6, &iph->daddr);
	ipcb->protocol = iph->protocol;
	ipcb->ttl = iph->ttl;
	ipcb->tos = iph->tos1;

    return 0;
}


