/*
*   udp packet forward
*
*/


#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/zassert.h>
#include <lib/index.h>
#include <lib/msg_ipc.h>
#include <lib/ifm_common.h>
#include <lib/memory.h>

#include "ftm_pkt.h"
#include "ftm_ifm.h"
#include "pkt_udp.h"
#include "pkt_ip.h"
#include "proto_reg.h"
#include "pkt_icmp.h"
#include "ftm.h"


struct hash_table udp_session_table;   /* udp session hash table */

/* local function */
int udp_encap(struct pkt_buffer *pkt, struct udp_session *psess);
int udp_decap(struct pkt_buffer *pkt);
static int udp_session_hold_timer_expire (void *para);
static void udp_session_hash_init(unsigned int size);
int udp_session_add(struct udp_session *psess);
struct udp_session *udp_session_create(struct udp_session *psess);
static struct udp_session *udp_session_lookup(struct udp_session *psess);
int udp_session_delete(struct udp_session *psess);




/* init udp proto hash */
void udp_init(void)
{
	index_register(INDEX_TYPE_UDP_PORT, UDP_PORT_MAX - UDP_PORT_RESERVE);
	udp_session_hash_init(HASHTAB_SIZE);

	high_pre_timer_add("FtmUdpIdleCheck", LIB_TIMER_TYPE_LOOP, udp_session_hold_timer_expire, NULL, 5 * 1000);
	return;
}

static unsigned int udp_compute(void *hash_key)
{
	struct udp_session *psess = (struct udp_session *)hash_key;

	return psess->sip + psess->dport + psess->sport;
}

static int udp_compare(void *item, void *hash_key)
{
	if(item == NULL || hash_key == NULL)
		return 1;
	
	struct hash_bucket *tmp = (struct hash_bucket *)item;
	struct udp_session *psess1 = (struct udp_session *)(tmp->hash_key);
	struct udp_session *psess2 = (struct udp_session *)hash_key;
	
	if(psess1 == NULL || psess2 == NULL)
		return 1;
		
	if(((psess1->sip == psess2->sip)
		||(ipv6_is_same_subnet(&psess1->sipv6, &psess2->sipv6,128)))
		 && (psess1->dport == psess2->dport)
		 && (psess1->sport == psess2->sport))
			return 0;
		else
			return 1;
}

/* init udp proto hash */
static void udp_session_hash_init(unsigned int size)
{
    hios_hash_init(&udp_session_table, size, udp_compute, udp_compare);	
}


/* return the module_id */
static struct udp_session *udp_session_lookup(struct udp_session *psess)
{
   struct hash_bucket *bucket = hios_hash_find(&udp_session_table, psess);

   if(bucket) 
   {
      return bucket->data;
   }

   return NULL;
}


/* add session to hash */
int udp_session_add(struct udp_session *psess)
{
	struct hash_bucket *bucket = NULL;
	int ret = 0;

	zlog_debug ( FTM_DBG_UDP,"%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__ );
	
	bucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(bucket == NULL)
	{
		zlog_err ("%s, %d XMALLOC fail!\n",__FUNCTION__, __LINE__);
		return -1;
	}
	
	bucket->hash_key = psess;
	bucket->data = psess;
	bucket->hashval = 0;
	bucket->next = NULL;
	bucket->prev = NULL;
	
    ret = hios_hash_add(&udp_session_table, bucket);	
	if(ret)
	{
		XFREE(MTYPE_HASH_BACKET, bucket);
		return -1;
	}

	return 0;
}


/* 创建一条 session */
struct udp_session *udp_session_create(struct udp_session *psess)
{
	struct udp_session *psess_new = XCALLOC(MTYPE_UDP, sizeof(struct udp_session));
	int ret = 0;
	
	zlog_debug(FTM_DBG_UDP,"%s, %d psess.dip:0x%x psess.sip:0x%x,psess.sport:0x%x, psess.dport:0x%x, psess.vpn:0x%x\n",
						__FUNCTION__, __LINE__,psess->dip,psess->sip,psess->sport,psess->dport, psess->vpn);

	if(psess_new == NULL)
	{
		zlog_err ("%s, %d udp_session_create fail!\n",__FUNCTION__, __LINE__);
		return NULL;
	}

	IPV6_ADDR_COPY(&psess_new->sipv6,&psess->sipv6);	
	IPV6_ADDR_COPY(&psess_new->dipv6,&psess->dipv6);
	psess_new->sip = psess->sip;	
	psess_new->dip = psess->dip;
	psess_new->sport = psess->sport;	
	psess_new->dport = psess->dport;
	psess_new->vpn = psess->vpn;
	psess_new->module_id = psess->module_id;
	
    /* 设置老化定时器 */

	/* 添加到 hash 表 */
	ret = udp_session_add(psess_new);
	if(ret)
	{
		XFREE(MTYPE_UDP, psess_new);
		return NULL;
	}
	
	return psess_new;
}


/* delete udp session */
int udp_session_delete(struct udp_session *psess)
{
	struct hash_bucket *pbucket = NULL;

	if(psess == NULL)
		return -1;
	pbucket = hios_hash_find(&udp_session_table, psess);
	if(NULL == pbucket)
	{
		return 0;
	}

	hios_hash_delete(&udp_session_table, pbucket);
	if(pbucket->hash_key)
		XFREE(MTYPE_UDP, pbucket->hash_key);
	
	XFREE(MTYPE_HASH_BACKET, pbucket);
	return 0;
}

/* 定时检查 udp 状态 */
static int udp_session_hold_timer_expire (void *para)
{
	struct hash_bucket *node = NULL;
	struct hash_bucket *cursor = NULL;
	struct udp_session *psess = NULL;
	
	if(udp_session_table.num_entries == 0)
	{
		goto drop;
	}
	
	for((node) = hios_hash_start(&udp_session_table, (void **)&cursor); node; (node) = hios_hash_next(&udp_session_table, (void **)&cursor))		
	{
		psess = (struct udp_session *)(node->data);
		if(psess == NULL)
		{
			zlog_debug(FTM_DBG_UDP,"%s, %d UDP hash error! \n",__FUNCTION__, __LINE__);
		}
		
		psess->timeout++;

		if(psess->timeout > 10)
		{
			zlog_debug(FTM_DBG_UDP,"%s, %d UDP session idle timeout.sip = %d,sport=%d,dport=%d\n",__FUNCTION__, __LINE__,psess->sip,psess->sport,psess->dport);
			udp_session_delete(psess);
		}
	}	
	
drop:

	return 0;
}



/* 接收 udp 报文，识别报文类型 */
int udp_rcv(struct pkt_buffer *pkt)
{
	struct ip_control *ip_cb = NULL;
	struct ip_proto proto; 	
	struct udp_session udp; 
	struct udphdr *udph = NULL;
	struct udp_session *psess = NULL;
	int module_id = 0;

	if(pkt == NULL)
		return -1;
	
	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_RECV, PKT_TYPE_UDP);
	ftm_pkt_cnt_add(PKT_ACTION_RECV, PKT_TYPE_UDP);
	
    memset(&proto, 0, sizeof(struct ip_proto));
	ip_cb = (struct ip_control *)(&(pkt->cb));

    if (udp_decap(pkt))
    {
        FTM_PKT_ZLOG_ERR("udp_decap fail!\n");
        goto drop;
    }
		

	/*find the registered module_id */
	memset(&proto, 0 , sizeof(struct ip_proto));
	proto.dip = ip_cb->dip;
	proto.protocol = ip_cb->protocol;
	proto.sport = ip_cb->sport;	
	proto.dport = ip_cb->dport;
	IPV6_ADDR_COPY(&proto.sipv6, &ip_cb->sipv6);	
	IPV6_ADDR_COPY(&proto.dipv6, &ip_cb->dipv6);
	if(IP_PROTO_IS_IPV6(ip_cb))
		proto.type = PROTO_TYPE_IPV6;
	module_id = ip_proto_lookup(&proto);
	if(!module_id) 
	{
		/*端口不可达错误应答*/
		FTM_PKT_ZLOG_DBG("ip_proto_lookup fail! sport:0x%x,dport:0x%x,dip:0x%x\n",proto.sport,proto.dport,proto.dip);
		icmp_send(pkt, ICMP_TYPE_UNREACH, ICMP_CODE_PORT);
		goto drop;
	}


	/* Add for VPN*/
	/* get udp session */	
	udph = (struct udphdr *)(pkt->transport_header);
	if(udph == NULL)
	{
		zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
		goto drop;
	}
	memset(&udp, 0, sizeof(struct udp_session));
	udp.sip = ip_cb->sip;
	udp.dip = ip_cb->dip;
	IPV6_ADDR_COPY(&udp.sipv6, &ip_cb->sipv6);	
	IPV6_ADDR_COPY(&udp.dipv6, &ip_cb->dipv6);
	udp.sport = ip_cb->sport;
	udp.dport = ip_cb->dport;
	udp.vpn = ip_cb->vpn;
	udp.module_id = module_id;

	if(proto.type == PROTO_TYPE_IPV6)
	{
		udp.v6session = TRUE;
		if(ipv6_is_zeronet(&udp.dipv6) || udp.sport == 0 || udp.dport == 0)
		{
			zlog_err ("%s, udp sipv6 0 or sport 0 or dport 0,drop it!%d\n",__FUNCTION__, __LINE__);
			goto drop;
		}
	}
	else	
	{
		udp.v6session = FALSE;
		if( MODULE_ID_DHCP == module_id )
		{
			if( udp.sport == 0 || udp.dport == 0 )
			{
				zlog_err ("%s, sport 0 or dport 0,drop it!%d\n",__FUNCTION__, __LINE__);
				goto drop;
			}
		}
		else if(udp.sport == 0 || udp.dport == 0)
		{
			zlog_err ("%s, udp sip 0 or sport 0 or dport 0,drop it!%d\n",__FUNCTION__, __LINE__);
			goto drop;
		}
	}
	
	psess = udp_session_lookup(&udp);
	if(NULL == psess)
	{
		psess = udp_session_create(&udp);
		if(NULL == psess)
		{
			zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
			goto drop;
		}
	}
	else
	{
		psess->timeout = 0;
	}
	/**/
	ftm_pkt_send_to_n(pkt, module_id);/* send to app */
	pkt_free(pkt);
    return NET_SUCCESS;

drop:
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_UDP);
	pkt_free(pkt);
	return NET_FAILED;
}


/* 发送 udp 报文，输入是 payload */
int udp_forward(struct pkt_buffer *pkt)
{
	int ret;	
	struct udp_session *psess = NULL;
	struct ip_control *ipcb = NULL; 
	struct udp_session udp;
	
	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_SEND, PKT_TYPE_UDP);
	ftm_pkt_cnt_add(PKT_ACTION_SEND, PKT_TYPE_UDP);


	/*Add for VPN*/
	if(pkt == NULL)
		return -1;
	
	ipcb = (struct ip_control *)(&(pkt->cb));
	zlog_debug(FTM_DBG_UDP,"%s, %d ipcb->sip:0x%x\n",__FUNCTION__, __LINE__,ipcb->sip);

	memset(&udp, 0 ,sizeof(struct udp_session));
	udp.sport = ipcb->dport;
	udp.dport = ipcb->sport;
	udp.vpn = ipcb->vpn;

	if(udp.dport == 0)
	{
        udp.dport = udp.sport;
    }
	if(ipcb->pkt_type == PKT_TYPE_IP)
	{
		udp.sip = ipcb->dip;
		udp.dip = ipcb->sip;
		zlog_debug(FTM_DBG_UDP,"%s, %d udp.dip:0x%x，udp.sip:0x%x,udp.sport:0x%x, udp.dport:0x%x, udp.vpn:0x%x\n",
					__FUNCTION__, __LINE__,udp.dip,udp.sip,udp.sport,udp.dport, udp.vpn);
		if(udp.sport == 0 || udp.dport == 0)
		{
			zlog_err ("%s, udp sip 0 or sport 0 or dport 0,drop it!%d\n",__FUNCTION__, __LINE__);
			goto drop;
		}
		udp.v6session = FALSE;
	}
	else if(ipcb->pkt_type == PKT_TYPE_IPV6)
	{
		IPV6_ADDR_COPY(&udp.sipv6, &ipcb->dipv6);	
		IPV6_ADDR_COPY(&udp.dipv6, &ipcb->sipv6);
		
		if(ipv6_is_zeronet(&udp.dipv6) || udp.sport == 0 || udp.dport == 0)
		{
			zlog_err ("%s, udp sipv6 0 or sport 0 or dport 0,drop it!%d\n",__FUNCTION__, __LINE__);
			goto drop;
		}
		udp.v6session = TRUE;
	}
	else
	{
		zlog_err ("%s, %d ipcb->pkt_type is unvalid\n",__FUNCTION__, __LINE__);
		goto drop;
	}
	
	zlog_debug(FTM_DBG_UDP,"%s, %d udp.dip:0x%x，udp.sip:0x%x,udp.sport:0x%x, udp.dport:0x%x, udp.vpn:0x%x\n",
					__FUNCTION__, __LINE__,udp.dip,udp.sip,udp.sport,udp.dport, udp.vpn);
	psess = udp_session_lookup(&udp);
	if(NULL == psess)
	{
		psess = udp_session_create(&udp);
		if(NULL == psess)
		{
			zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
			goto drop;
		}
	}
	else
	{
		psess->timeout = 0;
	}
	/**/
	
    ret = udp_encap(pkt,psess);
	if(ret != 0)
	{
        FTM_PKT_ZLOG_ERR("udp_encap fail!\n");
		goto drop;
	}
	
    return ip_forward(pkt);

drop:
	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_UDP);	
	pkt_free(pkt);
	return NET_FAILED;
}


/* 封装 UDP 头 */
int udp_encap(struct pkt_buffer *pkt, struct udp_session *psess)
{
	struct udphdr *udph = NULL;
    struct ip_control *ipcb = NULL;

    ipcb = (struct ip_control *)(&(pkt->cb));

    /* set udp header */
	if (pkt_push(pkt, sizeof(struct udphdr)))
	{
        FTM_PKT_ZLOG_ERR();
		return -1;
	}

    pkt->transport_header = pkt->data;
    udph = (struct udphdr *)(pkt->transport_header);
    ipcb = (struct ip_control *)(&(pkt->cb));	
	udph->dport = htons(ipcb->dport);
	if(ipcb->sport)
	{
        udph->sport = htons(ipcb->sport);
    }
	else
    {
	    udph->sport = udph->dport;
    }
    udph->len = htons(pkt->data_len);
	udph->checksum = 0;
	
	ipcb->is_changed = 1;
	ipcb->protocol = IP_P_UDP;

	if( 0 == ipcb->vpn )
	{
		ipcb->vpn = psess->vpn;
	}
	
    return 0;
}


/* 解析 udp 头 */
int udp_decap(struct pkt_buffer *pkt)
{
	struct udphdr *udph = NULL;
    struct ip_control *ipcb = NULL;

    if (NULL == pkt)
    {
        FTM_PKT_ZLOG_ERR();
        return -1;
    }

    pkt->transport_header = pkt->data;
    udph = (struct udphdr *)(pkt->transport_header);
    ipcb = (struct ip_control *)(&(pkt->cb));
	ipcb->sport = ntohs(udph->sport);
	ipcb->dport = ntohs(udph->dport);
    pkt->cb_type = PKT_TYPE_UDP;

	if(pkt_pull(pkt, sizeof(struct udphdr)))
    {
        FTM_PKT_ZLOG_ERR();
        return -1;
    }
	
    return 0;
}


/* 处理 获取端口 消息 */
int udp_port_alloc_for_app(struct ipc_msghdr_n *phdr,void *pdata, int data_len, int data_num, uint16_t sender_id, uint8_t opcode)
{
	uint16_t port;
	int ret;

	if(opcode == IPC_OPCODE_GET)
	{
		port = index_alloc(INDEX_TYPE_UDP_PORT) + UDP_PORT_RESERVE - 1;
		ret =  ftm_msg_send_reply( &port, 2, phdr, 0);
		if ( ret < 0 )
		{
			FTM_PKT_ZLOG_ERR("ipc_send_reply fail!\n");
		}	
	}
	else if(opcode == IPC_OPCODE_DELETE)
	{
		port = * (uint16_t *)pdata;
		index_free(INDEX_TYPE_UDP_PORT, port);
	}
	
	return 0;
}



