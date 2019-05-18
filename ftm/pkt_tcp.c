/*
*   tcp packet forward
*
*/
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <lib/pkt_buffer.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/zassert.h>
#include <lib/hash1.h>
#include <lib/log.h>
#include <lib/index.h>
#include <lib/errcode.h>
#include <time.h>
#include "ftm_pkt.h"
#include "proto_reg.h"
#include "pkt_ip.h"
#include "pkt_tcp.h"
#include "ftm.h"
#include <pthread.h> 
#include "ftm_bgp.h"
#include "command.h"

  
extern int bgp_port_swith;
int        pkt_with_md5 = 0;
uint16_t tcp_keepalive_switch = 0;
uint16_t tcp_keepalive_time = 0;

struct hash_table tcp_session_table;   /* tcp session hash table */


/* local function */
static void tcp_session_send_list_clear(struct tcp_session *psess);
static void tcp_session_rcv_list_clear(struct tcp_session *psess);
static int tcp_session_fsm(enum TCP_EVENT event, struct tcp_session *psess);
static int tcp_encap(struct pkt_buffer *pkt, struct tcp_session *psess);
static int tcp_send_control(int type, struct tcp_session *psess);
static int tcp_pkt_send(void *data, int data_len, struct ip_control *ipcb, struct tcp_session *psess);
int tcp_session_send_keepalive(struct tcp_session *psess);
int tcp_decap_option(struct pkt_buffer *pkt, struct tcp_session *psess, int opt_len);
int tcp_encap_option(struct pkt_buffer *pkt, struct tcp_session *psess, enum TCP_OPT opt);
static void tcp_session_send_list_ack(struct tcp_session *psess,struct pktInfo *Node);
int tcp_pkt_retransmit (struct pktInfo *pPkt,struct tcp_session *psess);

static unsigned int tcp_compute(void *hash_key)
{
	struct tcp_session *psess = (struct tcp_session *)hash_key;

	return psess->sip + psess->dport + psess->sport;
}

static int tcp_compare(void *item, void *hash_key)
{
	if(item == NULL || hash_key == NULL)
		return 1;
	
	struct hash_bucket *tmp = (struct hash_bucket *)item;
	struct tcp_session *psess1 = (struct tcp_session *)(tmp->hash_key);
	struct tcp_session *psess2 = (struct tcp_session *)hash_key;
	
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


/* return the module_id */
static struct tcp_session *tcp_session_lookup(struct tcp_session *psess)
{
   struct hash_bucket *bucket = hios_hash_find(&tcp_session_table, psess);

   if(bucket) 
   {
      return bucket->data;
   }

   return NULL;
}


/* init tcp proto hash */
static void tcp_session_hash_init(unsigned int size)
{
    hios_hash_init(&tcp_session_table, size, tcp_compute, tcp_compare);	
}

/* 定时检查 tcp重传报文 */
static int tcp_pkt_retransmit_check (struct tcp_session *psess)
{
	struct pktInfo *curNode = NULL;
	struct pktInfo *nextNode = NULL;
	
	curNode = psess->send_list;
	nextNode = NULL;
	
	if(curNode == NULL)
	{
		zlog_debug (FTM_DBG_TCP,"%s, %d\n",__FUNCTION__, __LINE__);
		goto out;
	}
	
	while(curNode != NULL)
	{
		nextNode = curNode->next;

		zlog_debug(FTM_DBG_TCP,"%s curNode->waitSeq:%X, psess->ack_seq:%X,curNode->ack Time:%d,curNode->unackcount:%d,curNode->restransmit:%d\n",
			__FUNCTION__,curNode->waitSeq,psess->ack_seq,curNode->ackTime,curNode->unackcount,curNode->restransmit);
		if(curNode->waitSeq == 0 || psess->ack_seq >= curNode->waitSeq)
		{
			goto _next;
		}

		if(curNode->ackTime == 0)
		{
			if(curNode->unackcount > 0)
			{
				if(curNode->unackcount < curNode->restransmit)
				{
					curNode->unackcount++;
				}
				else
				{
					tcp_pkt_retransmit( curNode, psess );
					curNode->restransmit += curNode->unackcount ;
					if(curNode->unackcount > TCP_PKT_RETRANMSMIT_MAX)
					{
						zlog_debug(FTM_DBG_TCP,"tcp reset session for retransmit packet five times failed\n");
						tcp_session_fsm(TCP_EVENT_RCV_RESET, psess);
						return 0;
					}
				}
			}
			else	
			{
				curNode->unackcount++;
			}
		}
_next:	
		curNode = nextNode;
	}
out:
	return 0;
}

/* TCP 模块基准定时器，每3秒超时一次，检查会话超时、报文重传超时，保活报文超时 */
static int tcp_status_check_timer (void *para)
{
	struct hash_bucket *node = NULL;
	struct hash_bucket *cursor = NULL;
	struct tcp_session *psess = NULL;

	if(tcp_session_table.num_entries == 0)
	{
		goto out;
	}
	
	for((node) = hios_hash_start(&tcp_session_table, (void **)&cursor); node; (node) = hios_hash_next(&tcp_session_table, (void **)&cursor))		
	{
		psess = (struct tcp_session *)(node->data);
		if(psess == NULL)
		{
			zlog_debug(FTM_DBG_TCP,"%s, %d tcp hash error! \n",__FUNCTION__, __LINE__);
		}
		
		if(psess->status == TCP_STATUS_ESTABLISHED)
		{
			if(tcp_keepalive_switch == 1)
			{
				if(psess->alive_time_count >= tcp_keepalive_time)
				{
					psess->timeout++;
					tcp_session_send_keepalive(psess);/* Every tcp_keepalive_time * 3s, a keepalive packet will be sent */
				}
				else
				{
					psess->alive_time_count++;
				}
			}
            /* Retransmit the packet that sent but no ack received */
			tcp_pkt_retransmit_check(psess);
		}
		else
		{
			psess->timeout++;
		}
        /* 3s * 20 ,one minute expire, a tcp session will delete */
		if(psess->timeout > 20)
		{
		    tcp_session_change_notify_app(psess,IPC_OPCODE_DISCONNECT);
			tcp_session_send_list_clear(psess);		
			tcp_session_rcv_list_clear(psess);
			tcp_session_delete(psess);
		}
	}	
	
out:
	return 0;
}


void tcp_init(void) 
{
	tcp_keepalive_switch = 0;
	tcp_keepalive_time = 40;
	tcp_session_hash_init(HASHTAB_SIZE);
	index_register(INDEX_TYPE_TCP_PORT, TCP_PORT_MAX - TCP_PORT_RESERVE);	
	high_pre_timer_add("TcpStatusCheckTimer",LIB_TIMER_TYPE_LOOP, tcp_status_check_timer, NULL, 3*1000);

}


/* add session to hash */
int tcp_session_add(struct tcp_session *psess)
{
	struct hash_bucket *bucket = NULL;
	int ret = 0;

	zlog_debug (FTM_DBG_TCP, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__ );
	
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
	
    ret = hios_hash_add(&tcp_session_table, bucket);	
	if(ret)
	{
		XFREE(MTYPE_HASH_BACKET, bucket);
		return -1;
	}

	return 0;
}


/* 创建一条 session */
struct tcp_session *tcp_session_create(struct tcp_session *psess)
{
	struct tcp_session *psess_new = XCALLOC(MTYPE_TCP, sizeof(struct tcp_session));
	int ret = 0;
	
	zlog_debug ( FTM_DBG_TCP,"%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__ );
	
	if(psess_new == NULL)
	{
		zlog_err ("%s, %d tcp_session_create fail!\n",__FUNCTION__, __LINE__);
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
	psess_new->v6session = psess->v6session;
	
	/* 初始化本端参数 */
	psess_new->status = TCP_STATUS_LISTEN;	
	psess_new->mss = TCP_MSS_DEFAULT;
	psess_new->recv_wnd = 9600;
	psess_new->send_wnd = 0x2000;
	psess_new->win_scale = 1;
	psess_new->seq_next = 1;
	
	/*对一个给定的连接，初始化cwnd为1个报文段，ssthresh为65535个字节*/
	psess_new->cwnd = 1;
	psess_new->ssthresh = 65535;
	
	psess_new->send_list = NULL;
	psess_new->recv_list = NULL;			
	psess_new->ttl = 0;

    /* 设置老化定时器 */

	/* 添加到 hash 表 */
	ret = tcp_session_add(psess_new);
	if(ret)
	{
		XFREE(MTYPE_TCP, psess_new);
		return NULL;
	}
	
	return psess_new;
}


/* delete tcp session */
int tcp_session_delete(struct tcp_session *psess)
{
	struct hash_bucket *pbucket = NULL;

	if(psess == NULL)
		return -1;
	pbucket = hios_hash_find(&tcp_session_table, psess);
	if(NULL == pbucket)
	{
		return 0;
	}

	hios_hash_delete(&tcp_session_table, pbucket);
	if(pbucket->hash_key)
		XFREE(MTYPE_TCP, pbucket->hash_key);
	
	XFREE(MTYPE_HASH_BACKET, pbucket);
	return 0;
}


/* 断开会话 */
int tcp_session_close(struct ip_proto *proto) 
{
	struct tcp_session tcp;	
	struct tcp_session *psess = NULL;

	if(proto == NULL)
	{
		zlog_debug (FTM_DBG_TCP,"%s, %d\n",__FUNCTION__, __LINE__);
		return -1;
	}
	memset(&tcp, 0, sizeof(struct tcp_session));
	tcp.sport = proto->dport;
	tcp.dport = proto->sport;

	if(proto->type == PROTO_TYPE_IPV6)
	{
		IPV6_ADDR_COPY(&tcp.sipv6, &proto->dipv6);	
		IPV6_ADDR_COPY(&tcp.dipv6, &proto->sipv6);
		if(ipv6_is_zeronet(&tcp.dipv6) || tcp.sport == 0 || tcp.dport == 0)
		{
			zlog_err("%s[%d] sip:%#x, sport:%d, dport:%d", __FUNCTION__, __LINE__, tcp.sip,tcp.sport,tcp.dport);
			return -1;
		}	
		tcp.v6session = TRUE;
	}
	else
	{
		tcp.sip = proto->dip;
		tcp.dip = proto->sip;
		if(tcp.sip == 0 || tcp.sport == 0 || tcp.dport == 0)
		{
			zlog_err("%s[%d] sip:%#x, sport:%d, dport:%d", __FUNCTION__, __LINE__, tcp.sip,tcp.sport,tcp.dport);
			return -1;
		}	
		tcp.v6session = FALSE;
	}
	
	zlog_debug(FTM_DBG_TCP,"%s, %d ip:0x%x, sport:0x%x, dport:0x%x\n",__FUNCTION__, __LINE__,
				tcp.sip,tcp.sport,tcp.dport);
	psess = tcp_session_lookup(&tcp);
	if(psess == NULL)
	{
		zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
		return 0;
	}
	zlog_notice("%s from %d to (%#x:%d) \n", __FUNCTION__, tcp.dport, tcp.sip, tcp.sport);
	
	tcp_session_send_list_clear(psess);	
	tcp_session_rcv_list_clear(psess);	
	tcp_session_fsm(TCP_EVENT_SND_FIN, psess);

	return 0;
}


/* 发送报文存储到链表,包括已发送未ACK和未发送报文，其中已发送未ACK报文添加发送超时定时器 */
static void tcp_session_send_list_add(struct pkt_buffer *pkt, struct tcp_session *psess)
{
	struct pktInfo *curNode = NULL;
	struct pktInfo *prevNode = NULL;
	struct pktInfo *curnext = NULL;
	struct pkt_buffer *send = NULL; 
	struct pkt_buffer *tail = NULL;
	struct pkt_buffer *tmp = NULL;
	int size = 0;
	if(psess == NULL || pkt == NULL)
	{
		return ;
	}

	size = sizeof(struct pkt_buffer);
	tmp = XMALLOC(MTYPE_TCP, size + pkt->data_len);
	if(tmp == NULL)
	{
		zlog_err ("%s, %d XMALLOC fail!\n",__FUNCTION__, __LINE__);
		return ;
	}
	memcpy(tmp, pkt, size);
	tmp->data = (char *)tmp + size;
	memcpy(tmp->data, pkt->data, pkt->data_len);

	if(psess->send_len > 0)
	{
		send = tmp;
	}
	else
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
		send = NULL;
	}
	
	zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
	if(psess->send_len == 0)
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
		tail = tmp;
	}
	else
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
		tail = NULL;
	}

	if(send == NULL && tail == NULL)
	{
		zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
		XFREE(MTYPE_TCP, tmp);
		return ;
	}

	/* 查找最后的挂接点 */
	curNode = psess->send_list;
	prevNode = NULL;		
	while(curNode != NULL)
	{
		prevNode = curNode;
		curNode = curNode->next;
	}
	zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
	if(send != NULL)
	{
		curNode = XCALLOC (MTYPE_TCP, sizeof (struct pktInfo));
		if(curNode == NULL)
		{
			XFREE(MTYPE_TCP, tmp);
			zlog_err ("%s, %d XCALLOC fail!\n",__FUNCTION__, __LINE__);
			return ;
		}

		curNode->data 	 = (uint8_t *)send;
		curNode->dataLen = send->data_len;
		curNode->waitSeq = psess->seq_next + send->data_len;
		curNode->ackTime = 0;
		curNode->prev = prevNode;
		curNode->next = NULL;
		if(prevNode == NULL)
		{
			psess->send_list = curNode;
		}
		else
		{
			prevNode->next = curNode;
		}
	}
	
	if(tail != NULL)
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
		curnext = XCALLOC (MTYPE_TCP, sizeof (struct pktInfo));
		if(curnext == NULL)
		{
			XFREE(MTYPE_TCP, tmp);
			zlog_err ("%s, %d XCALLOC fail!\n",__FUNCTION__, __LINE__);
			return ;
		}
		curnext->data = (uint8_t *)tail;
		curnext->dataLen = tail->data_len;
		curnext->waitSeq = 0;
		curnext->ackTime = 0;
		if(send == NULL)
		{
			curnext->prev = prevNode;
			curnext->next = NULL;
			if(prevNode == NULL)
			{
				psess->send_list = curnext;
			}
			else
			{
				prevNode->next = curnext;
			}
		}
		else
		{
			curnext->prev = curNode;
			curnext->next = NULL;
			curNode->next = curnext;
		}
	}	

	return;
}


/* tcp会话挂接报文删除 */
static void tcp_session_send_list_clear(struct tcp_session *psess)
{
	struct pktInfo *curNode = NULL;
	struct pktInfo *nextNode = NULL;
	if(psess == NULL)
		return;
	
	curNode = psess->send_list;
	while(curNode != NULL)
	{
		nextNode = curNode->next;
		if(curNode->data != NULL)
		{
			//zlog_debug("%s, %d curNode!!!cnt:0x%x\n",__FUNCTION__, __LINE__,cnt);
			XFREE(MTYPE_TCP, curNode->data);
		}
		XFREE(MTYPE_TCP, curNode);	
		curNode = nextNode;
	}	
	psess->send_list = NULL;
	
	return;
}


/* 删除已ACK节点Node前的所有节点,最后的ack节点不删除，保留用来重传 */
static void tcp_session_send_list_ack(struct tcp_session *psess,struct pktInfo *Node)
{
	struct pktInfo *curNode = NULL;
	struct pktInfo *nextNode = NULL;
	if(psess == NULL || Node == NULL)
		return ;
	zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
	curNode = psess->send_list;
	while(curNode != Node)
	{
		nextNode = curNode->next;
		if(curNode->data != NULL)
		{		
			XFREE(MTYPE_TCP, curNode->data);
		}
		if(curNode != NULL)
		{
			XFREE(MTYPE_TCP, curNode);				
		}
		curNode = nextNode;
		if(curNode == NULL)
			break;
	}

	Node->prev = NULL;
	psess->send_list = Node;

	return;
}


/* tcp会话挂接报文发送 */
static int tcp_session_send_list_process(struct tcp_session *psess)
{
	struct pktInfo *curNode = NULL;
	struct pktInfo *prevNode = NULL;
	struct pktInfo *nextNode = NULL;
	struct pkt_buffer *pkt = NULL;
	struct ip_control *ipcb = NULL; 
	int ret;
	int cnt = 0;
	//struct pkt_buffer *pktNext = NULL;
	if(psess == NULL)
		return -1;
	
	zlog_debug(FTM_DBG_TCP,"%s, %d\n",__FUNCTION__, __LINE__);
	curNode = psess->send_list;
	prevNode = NULL;
	nextNode = NULL;
	if(curNode == NULL)
	{
		zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
		return -1;
	}

	prevNode = curNode->prev;
	nextNode = curNode->next;
	while(curNode != NULL)
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d psess->ack_seq:0x%x, curNode->waitSeq:0x%x\n",
			__FUNCTION__, __LINE__,psess->seq_next, curNode->waitSeq);
		
		cnt++;
		prevNode = curNode;
		zlog_debug(FTM_DBG_TCP,"%s, %d cnt:0x%d\n",__FUNCTION__, __LINE__,cnt);
		nextNode = curNode->next;
		if(curNode->data == NULL)
		{
			zlog_err("%s, %d \n",__FUNCTION__, __LINE__);
			break;
		}
		pkt = (struct pkt_buffer *)curNode->data;
		ipcb = (struct ip_control *)(&(pkt->cb));
		
		if(psess->send_wnd < pkt->data_len)
		{
			zlog_debug(FTM_DBG_TCP,"%s, %d send_wnd:0x%x, data_len:0x%x\n",__FUNCTION__, __LINE__,psess->send_wnd,pkt->data_len);
			break;
		}

		/* 未发送报文 */
		if(curNode->waitSeq == 0)
		{
			psess->headlen = 0x5;
			psess->flag = TCP_FLAG_PSH | TCP_FLAG_ACK ;
			zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
			ret = tcp_pkt_send(pkt->data, pkt->data_len, ipcb, psess);
			if(ret != 0)
			{
				zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
				break;
			}
			curNode->waitSeq = psess->seq_next;
		}
		else if(psess->ack_seq >= curNode->waitSeq)
		{
			/* 收到重复ACK报文，已ACK报文删除 */ 
			curNode->ackTime++;
			zlog_debug(FTM_DBG_TCP,"%s, %d curNode->ack Time:0x%x\n",__FUNCTION__, __LINE__,curNode->ackTime);
			if(prevNode != NULL)
			{
				zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
				tcp_session_send_list_ack(psess, curNode);
			}
		}

		curNode = nextNode;
	}
	
	zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
	return 0;
}


/* 接收未处理报文链表 */
static void tcp_session_rcv_list_add(struct pkt_buffer *pkt, struct tcp_session *psess)
{
	struct pkt_buffer *cur = NULL;
	struct pkt_buffer *next = NULL;
	struct pkt_buffer *tmp = NULL;
	int size = 0;
	struct tcphdr *tcph;

	if(psess == NULL || pkt == NULL)
		return ;
	size = sizeof(struct pkt_buffer);
	
	tmp = XMALLOC(MTYPE_TCP, size + pkt->data_len);
	if(tmp == NULL)
	{
		zlog_err ("%s, %d XMALLOC fail!\n",__FUNCTION__, __LINE__);
		return;
	}

	tcph = XMALLOC(MTYPE_TCP, sizeof(struct tcphdr));
	if(tcph == NULL)
	{
		XFREE(MTYPE_TCP, tmp);
		zlog_err ("%s, %d XMALLOC fail!\n",__FUNCTION__, __LINE__);
		return;
	}

	memcpy(tcph, (struct tcphdr *)(pkt->transport_header), sizeof(struct tcphdr));
	memcpy(tmp, pkt, size);
	tmp->data = (char *)tmp + size;
	tmp->transport_header = tcph;
	
	if(pkt->data_len != 0)
		memcpy(tmp->data, pkt->data, pkt->data_len);
	
	//tcph = (struct tcphdr *)(tmp->transport_header);
	zlog_debug(FTM_DBG_TCP,"%s, %d tmp->seq=0x%X,datalen=%d\n",__FUNCTION__, __LINE__,tcph->seq,tmp->data_len);
	/* 遍历查找接收链表最后节点 */
	cur = psess->recv_list;
	if(cur == NULL)
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
		psess->recv_list = tmp;
		tmp->next = NULL;
		tmp->prev = NULL;
		//return;
	}
	else	
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
		next = cur->next;
		while(next != NULL)
		{
			cur = next;
			next = cur->next;
		}

		cur->next = tmp;
		tmp->prev = cur;
		tmp->next = NULL;
	
		//return;	
	}
	
	return ;
}


/* 处理已收到未处理的乱序报文 */ 
static void tcp_session_rcv_list_proc_pkt_out_of_order(struct tcp_session *psess, int module_id)
{
	struct pkt_buffer *cur = NULL;
	struct pkt_buffer *next = NULL;	
	struct pkt_buffer *prev = NULL;
	struct pkt_buffer *pkt = NULL;
	uint32_t pktdata = 0;
	struct tcphdr *tcph;
	int ret = 0;
	
	if(psess == NULL)
		return ;
	
	if(psess->recv_list == NULL)
	{
		return ;
	}
	cur = psess->recv_list;

	while(cur != NULL)
	{
		prev = cur->prev;
		next = cur->next;
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
	    tcph = (struct tcphdr *)(cur->transport_header);
		if(tcph == NULL)
		{
			zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
			continue;
		}	
		zlog_debug(FTM_DBG_TCP,"%s, %d tcph->seq:0x%x, psess->seq_next_recv:0x%x\n",__FUNCTION__, __LINE__,tcph->seq,psess->seq_next_recv);

		if(ntohl(tcph->seq) == psess->seq_next_recv)
		{
			/* 取出查找到的pkt_buff */
			if(prev == NULL)
			{
				psess->recv_list = next;
			}
			else
			{
				prev->next = next;
			}
			if(next != NULL)
			{
				next->prev = prev;
			}
			zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
			
			pkt = pkt_alloc(cur->data_len + 100);
			if(!pkt)
			{
				zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
				return ;
			}

			pktdata = (uint32_t)pkt->data;
			memcpy(pkt, cur, sizeof(struct pkt_buffer));
			pkt->data = (void *)pktdata;
			pkt->transport_header = NULL;
			pkt->network_header = NULL;
			pkt->link_header = NULL;
			pkt->mpls_header = NULL;
			pkt->data_len = 0;

			if(cur->data_len != 0)
			{
				if(pkt_push(pkt, cur->data_len))
				{
					pkt_free(pkt);
					zlog_err ("%s, %d,pkt->data_len=%d,cur->data_len=%d\n",__FUNCTION__, __LINE__,pkt->data_len,cur->data_len);
					return ;
				}
				pkt_set(pkt, cur->data, cur->data_len);

				ret = ftm_pkt_send_to_n(pkt, module_id);
				if(ret)
				{
					pkt_free(pkt);
					zlog_err ("%s, %d,ftm_pkt_send_to failed,module_id =%d\n",__FUNCTION__, __LINE__,module_id);
					return ;
				}

				psess->seq = ntohl(tcph->seq);
				psess->ack_seq = ntohl(tcph->ack_seq);
				psess->seq_next = ntohl(tcph->ack_seq);
				psess->seq_next_recv = ntohl(tcph->seq) + cur->data_len;
				psess->flag = tcph->flag;
				psess->headlen = tcph->hlen;

				XFREE(MTYPE_TCP, cur->transport_header);
				XFREE(MTYPE_TCP, cur);
				pkt_free(pkt);
			}
			else
			{
				XFREE(MTYPE_TCP, cur->transport_header);
				XFREE(MTYPE_TCP, cur);
				pkt_free(pkt);
			}
		}
		cur = next;
	}

	return;	
}
static void tcp_session_rcv_list_clear(struct tcp_session *psess)
{
	struct pkt_buffer *cur = NULL;
	struct pkt_buffer *next = NULL;
	if(psess == NULL)
		return ;
	
	if(psess->recv_list == NULL)
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d psess->recv_list == NULL\n",__FUNCTION__, __LINE__);
		return ;
	}
	cur = psess->recv_list;

	while(cur != NULL)
	{
		next = cur->next;
		XFREE(MTYPE_TCP, cur->transport_header);
		XFREE(MTYPE_TCP, cur);
		cur = next;
	}
	zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
	psess->recv_list = NULL;
	return ;
}


static void tcp_session_rcv_list_sendto_app(struct tcp_session *psess)
{
	struct pkt_buffer *cur = NULL;
	struct pkt_buffer *next = NULL;
	if(psess == NULL)
		return ;

	if(psess->module_id == 0)
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d psess->module_id == 0\n",__FUNCTION__, __LINE__);
		return ;
	}
	
	if(psess->recv_list == NULL)
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d psess->recv_list == NULL\n",__FUNCTION__, __LINE__);
		return ;
	}
	cur = psess->recv_list;

	while(cur != NULL)
	{
		//ftm_pkt_send_to(cur, psess->module_id);
		ftm_pkt_send_to_n(cur, psess->module_id);
		next = cur->next;
		XFREE(MTYPE_TCP, cur->transport_header);
		XFREE(MTYPE_TCP, cur);
		cur = next;
	}
	zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
	psess->recv_list = NULL;
	return ;
}


/* tcp packet process : tcp sesion control packet and the user data */
int tcp_rcv_process(struct pkt_buffer *pkt, struct tcp_session *psess, int module_id)
{
	struct tcphdr *tcph = NULL;
	uint8_t flags = 0;
	int len;
	
    tcph = (struct tcphdr *)(pkt->transport_header);
	if(tcph == NULL)
	{
		zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
		goto drop;
	}
	
	flags = tcph->flag;
	len = tcph->hlen * 4; 
	zlog_debug(FTM_DBG_TCP,"%s, %d datalen:%d status:0x%x flag:%d\n",__FUNCTION__, __LINE__,len,psess->status,flags);
	if(len > TCP_HEADER_SIZE)
	{
		if(tcp_decap_option(pkt,psess, len- TCP_HEADER_SIZE) == -1)
		{
			zlog_err ("%s, %d tcp_decap_option  error\n",__FUNCTION__, __LINE__);
			goto drop;
		}
		/*pkt->data_len -= len-TCP_HEADER_SIZE;*/
		if(pkt_pull(pkt,len-TCP_HEADER_SIZE))
	    {
			zlog_err("%s, %d pkt_pull fail!\n",__FUNCTION__, __LINE__);
			goto drop;
	    }
		zlog_debug(FTM_DBG_TCP,"%s, %d psess->win_scale:0x%x\n",__FUNCTION__, __LINE__,psess->win_scale);
	}
    
#if TCP_MD5_OPTION_SET
	if(((psess->dport == TCP_PORT_BGP)||(psess->sport == TCP_PORT_BGP))
		&&(pkt_with_md5 == 0) && 
		(bgp_peer_entry_lookup(psess->dip,psess->sip) != -1))
	{
		zlog_err ("%s, %d tcp_decap_option  error \n",__FUNCTION__, __LINE__);
		goto drop;
	}
#endif	
	/* 处理控制报文 */
	if(flags == TCP_FLAG_SYN)  /* 收到 syn 报文*/
	{
		/* 一次握手 */
		psess->seq_next_recv = ntohl(tcph->seq);
		psess->send_wnd = ntohs(tcph->window) * psess->win_scale;
		tcp_session_fsm(TCP_EVENT_RCV_SYN, psess);	

	}
	else if(flags == (TCP_FLAG_ACK + TCP_FLAG_SYN))  /* 收到 ack 报文*/
	{
		/* 二次握手 */
		psess->seq_next_recv = ntohl(tcph->seq);
		psess->send_wnd = ntohs(tcph->window) * psess->win_scale;
		tcp_session_fsm(TCP_EVENT_RCV_SYN_ACK, psess);

		/* 三次握手成功后，发送链表内的报文 */
		if(psess->status == TCP_STATUS_ESTABLISHED)
		{
			zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
			tcp_session_send_list_process(psess);
		}		
	}
	else if((flags == TCP_FLAG_ACK) || (flags == (TCP_FLAG_ACK+TCP_FLAG_PSH)))
	{
		if(psess->status == TCP_STATUS_ESTABLISHED)
		{
			/*when reveived open-pkt but BGP neighbor is down && del all BGP config,send FIN*/
			if((flags == (TCP_FLAG_ACK+TCP_FLAG_PSH)) && ((psess->dport == TCP_PORT_BGP)
				||(psess->sport == TCP_PORT_BGP))&&(bgp_port_swith == 0))
			{
				tcp_session_send_list_clear(psess);	
				tcp_session_rcv_list_clear(psess);	
				tcp_session_fsm(TCP_EVENT_SND_FIN, psess);
				goto drop;
			}/* 应答报文的ack中tcp segment长度为0 */
			else if((flags == (TCP_FLAG_ACK+TCP_FLAG_PSH) && pkt->data_len > 0) || (flags == TCP_FLAG_ACK && pkt->data_len > 0))
			{
				tcp_pkt_recv_usr_data_process(psess, pkt, module_id);
				return NET_SUCCESS;	
			}
            else if((flags == TCP_FLAG_ACK) &&(0 == pkt->data_len) && (psess->seq_next_recv - ntohl(tcph->seq)) == 1)
			{
                zlog_debug("%s, %d recevied tcp keepalvie packet, send the keepalvie_ack\n",__FUNCTION__, __LINE__);
                /* recevied tcp keepalvie packet, send the keepalvie_ack */
                /* because no data in the packet, the sequuence num  + 0 */
                //psess->seq_next_recv += 1;
                tcp_send_control(TCP_TYPE_ACK, psess);
            }
			else
			{
				/* 触发一次存储链表发送操作 */
				tcp_session_send_list_process(psess);					

				psess->send_wnd += ntohl(tcph->ack_seq) - psess->ack_last;
				zlog_debug(FTM_DBG_TCP,"%s, %d psess->send_wnd  --->%d\n",__FUNCTION__, __LINE__,psess->send_wnd);
				/*tcp dup ack*/
				if(psess->ack_last != ntohl(tcph->ack_seq))
				{
					psess->reackcnt = 0;
					psess->ack_last = ntohl(tcph->ack_seq);
				}
				else 
				{
					psess->reackcnt++;
				}
				
				if(psess->reackcnt >= 3)
				{
					psess->seq_next = ntohl(tcph->ack_seq);					
				}
			}	
		}
		else if(psess->status == TCP_STATUS_LAST_ACK)
		{
			zlog_debug(FTM_DBG_TCP,"%s, %d TCP_STATUS_LAST_ACK\n",__FUNCTION__, __LINE__);
			tcp_session_fsm(TCP_EVENT_RCV_ACK, psess);
		}
		else if(psess->status == TCP_STATUS_SYN_SENT)
		{
			/* 三次握手或其他 */		
			tcp_session_fsm(TCP_EVENT_RCV_ACK, psess);
			
			/* 三次握手成功后，发送链表内的报文 */
			if(psess->status == TCP_STATUS_ESTABLISHED)
			{
				zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
				tcp_session_send_list_process(psess);
			}
			
			zlog_debug(FTM_DBG_TCP,"%s, %d\n",__FUNCTION__, __LINE__);
			psess->seq_next_recv  = ntohl(tcph->seq);
			psess->seq_next = ntohl(tcph->ack_seq);
		}
		else if(psess->status == TCP_STATUS_SYN_RECEIVED)
		{
			zlog_debug(FTM_DBG_TCP,"%s, %d TCP_STATUS_LAST_ACK\n",__FUNCTION__, __LINE__);
			tcp_session_fsm(TCP_EVENT_RCV_ACK, psess);
			if(flags == (TCP_FLAG_ACK+TCP_FLAG_PSH))
			{
				tcp_pkt_recv_usr_data_process(psess, pkt, module_id);
				return NET_SUCCESS;	
			}
		}
		else
		{
			if((psess->status == TCP_STATUS_SYN_RECEIVED)
				&& ((flags == (TCP_FLAG_ACK+TCP_FLAG_PSH)) 
				|| (flags == TCP_FLAG_ACK && pkt->data_len > 0)))
		   	{
					zlog_debug(FTM_DBG_TCP,"%s, %d tcp rcvlist_add \n",__FUNCTION__, __LINE__);
					tcp_session_rcv_list_add(pkt, psess);
					pkt_free(pkt);
					return 0;
		   	}
			
			/* 三次握手或其他 */		
			tcp_session_fsm(TCP_EVENT_RCV_ACK, psess);	
			psess->seq_next_recv  = ntohl(tcph->seq);
			psess->seq_next = ntohl(tcph->ack_seq);
			zlog_debug(FTM_DBG_TCP,"%s, %d psess->seq_next_recv:0x%x\n",__FUNCTION__, __LINE__,psess->seq_next_recv);
			if(psess->status == TCP_STATUS_ESTABLISHED)
			{
				tcp_session_rcv_list_proc_pkt_out_of_order(psess,module_id);
			}
		}
	}
	else if(flags == TCP_FLAG_FIN + TCP_FLAG_PSH + TCP_FLAG_ACK)  /* 收到 fin+psh+ack 报文*/
	{
		if(psess->seq_next_recv == psess->seq && psess->status == TCP_STATUS_ESTABLISHED)
		{
			ftm_pkt_send_to_n(pkt, module_id);/* send to app */
			psess->seq_next_recv = ntohl(tcph->seq) + pkt->data_len - 1;
			psess->seq_next = ntohl(tcph->ack_seq);						
			tcp_session_fsm(TCP_EVENT_RCV_FIN, psess);
		}
	}
	else if(flags == TCP_FLAG_FIN)  /* 收到 fin 报文*/
	{
		if(psess->seq_next_recv == psess->seq && (psess->status == TCP_STATUS_ESTABLISHED
			|| psess->status == TCP_STATUS_FIN_WAIT_2))
		{
			tcp_session_fsm(TCP_EVENT_RCV_FIN, psess);
		}		
	}
	else if(flags == (TCP_FLAG_FIN + TCP_FLAG_ACK))  /* 收到 fin+ack 报文*/
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d psess->seq_next_recv:0x%x,tcph->seq:0x%x\n",__FUNCTION__, __LINE__,psess->seq_next_recv,tcph->seq);
		if(psess->seq_next_recv == psess->seq && (psess->status == TCP_STATUS_ESTABLISHED 
				|| psess->status == TCP_STATUS_FIN_WAIT_1 || psess->status == TCP_STATUS_FIN_WAIT_2))
		{
			tcp_session_fsm(TCP_EVENT_RCV_FIN_ACK, psess);
		}
		else if(psess->seq_next_recv < psess->seq && psess->status == TCP_STATUS_ESTABLISHED)
		{
			/* 提前收到FIN报文，不处理 */
			zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
			tcp_session_rcv_list_add(pkt, psess);
			pkt_free(pkt);
			return 0;
		}
	}	
	else if((flags == TCP_FLAG_RST) || (flags == (TCP_FLAG_RST+TCP_FLAG_ACK)))  /* 收到 rst 报文*/
	{
		tcp_session_fsm(TCP_EVENT_RCV_RESET, psess);
	}	
	else
	{
		zlog_err ("%s, %d psess->status:0x%x,flags:0x%x\n",__FUNCTION__, __LINE__,
					psess->status, flags);
		goto drop;
	}
	
	pkt_free(pkt);
    return NET_SUCCESS;

drop:	
	zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
	pkt_free(pkt);
	return NET_FAILED;
	
}



/* 接收 tcp 报文，处理数据报文的组包 */
int tcp_pkt_recv_usr_data_process(struct tcp_session *psess, struct pkt_buffer *pkt, int module_id)
{
	struct tcphdr *tcph = NULL;
	/*struct tcphdr *tcphtmp = NULL;
	struct pkt_buffer *rcvPkt = NULL;
	struct pkt_buffer *prevPkt = NULL;*/
	uint32_t	seqnexttmp; 
	if(psess == NULL || pkt == NULL)
		return -1;
	
	tcph = (struct tcphdr *)(pkt->transport_header);
	if(ntohl(tcph->seq) == psess->seq_next_recv)
	{
		/* 收到正常序列报文 */	
		
		//ftm_pkt_send_to(pkt, module_id);/* send to app */
		int ret = ftm_pkt_send_to_n(pkt, module_id);/* send to app */
		if(ret)
		{
			psess->recv_wnd /= 2;
			if(psess->recv_wnd < 2048)
			{
				psess->recv_wnd = 2048;
			}
			pkt_free(pkt);
			return -1;
		}

		if(psess->recv_wnd < 65336){
			psess->recv_wnd *= 2;
			if(psess->recv_wnd > 65336)
			{
				psess->recv_wnd = 65336;
			}
		}
		
		psess->seq_next_recv = ntohl(tcph->seq) + pkt->data_len;
		seqnexttmp = psess->seq_next;
		psess->seq_next = ntohl(tcph->ack_seq);

		/* 发送应答 */
		pkt_free(pkt);
		
		zlog_debug(FTM_DBG_TCP,"%s, %d psess->send_wnd %d,win_scale:0x%d  psess->seq 0x%x psess->ack_seq 0x%x psess->seq_next 0x%x psess->seq_next_recv 0x%x psess->flag 0x%x\n",
			__FUNCTION__, __LINE__,psess->send_wnd,psess->win_scale, psess->seq,psess->ack_seq,psess->seq_next,psess->seq_next_recv,psess->flag );
		
		tcp_session_rcv_list_proc_pkt_out_of_order(psess, module_id);
		tcp_session_fsm(TCP_EVENT_RCV_ACK, psess);
		if(psess->reackcnt < 3)
		{
			psess->seq_next = seqnexttmp;
		}

		return 0;			
	}
	else if(ntohl(tcph->seq) > psess->seq_next_recv)
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d tcph->seq > psess->seq_next_recv,tcph->seq=0x%X\n",__FUNCTION__, __LINE__,ntohl(tcph->seq));
		/* 收到乱序报文 */
		tcp_session_rcv_list_add(pkt,psess);
		pkt_free(pkt);
	}
	else
	{
		pkt_free(pkt);
		/* 收到重复报文，不处理 */
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}

/* Analysis the tcp packet header and find which module the packet will be sent to*/
int tcp_rcv(struct pkt_buffer *pkt)
{
	struct tcphdr *tcph = NULL;
    struct ip_control *ip_cb = NULL;
	struct tcp_session *psess = NULL;
	struct ip_proto proto;
	struct tcp_session tcp;	
	int module_id = 0;
	uint8_t flags = 0;
	int ret; 
	
	if(pkt == NULL)
	{
        return -1;
	}

	zlog_debug (FTM_DBG_TCP, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__ );
	
	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_RECV, PKT_TYPE_TCP);
	ftm_pkt_cnt_add(PKT_ACTION_RECV, PKT_TYPE_TCP);

	ret = tcp_decap(pkt);
    if (ret == -1)
    {
        goto drop;
    }
	
	/*find the registered module_id */	
    ip_cb = (struct ip_control *)(&(pkt->cb));
	memset(&proto, 0, sizeof(struct ip_proto));
	proto.dip = ip_cb->dip;
	proto.protocol = ip_cb->protocol;
	proto.sport = ip_cb->sport;	
	proto.dport = ip_cb->dport;	
	IPV6_ADDR_COPY(&proto.sipv6, &ip_cb->sipv6);	
	IPV6_ADDR_COPY(&proto.dipv6, &ip_cb->dipv6);
	if(IP_PROTO_IS_IPV6(ip_cb))
	{
		proto.type = PROTO_TYPE_IPV6;
	}
	module_id = ip_proto_lookup(&proto);
	if(!module_id)
	{
		/* TCP处理程序会在自己认为的异常时刻发送RST包。例如，A向B发起连接，
		但B之上并未监听相应的端口，这时B操作系统上的TCP处理程序会发RST包 */
		zlog_debug (FTM_DBG_TCP,"%s, %d dip:0x%x, sport:0x%x,dport:0x%x,moudle_id = %d\n",
					__FUNCTION__, __LINE__,proto.dip,proto.sport,proto.dport,module_id);		
		goto drop;
	}
	
	/* get tcp session */	
    tcph = (struct tcphdr *)(pkt->transport_header);
	if(tcph == NULL)
	{
		zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
		goto drop;
	}
	flags = tcph->flag;
	memset(&tcp, 0, sizeof(struct tcp_session));
	tcp.sip = ip_cb->sip;
	tcp.dip = ip_cb->dip;
	IPV6_ADDR_COPY(&tcp.sipv6, &ip_cb->sipv6);	
	IPV6_ADDR_COPY(&tcp.dipv6, &ip_cb->dipv6);
	tcp.sport = ip_cb->sport;
	tcp.dport = ip_cb->dport;
	tcp.vpn = ip_cb->vpn;
	tcp.module_id = module_id;

	if(proto.type == PROTO_TYPE_IPV6)
	{
		tcp.v6session = TRUE;
		if(ipv6_is_zeronet(&tcp.dipv6) || tcp.sport == 0 || tcp.dport == 0)
		{
			zlog_err ("%s, tcp sipv6 0 or sport 0 or dport 0,drop it!%d\n",__FUNCTION__, __LINE__);
			goto drop;
		}
	}
	else	
	{
		tcp.v6session = FALSE;
		if(tcp.sip == 0 || tcp.sport == 0 || tcp.dport == 0)
		{
			zlog_err ("%s, tcp sip 0 or sport 0 or dport 0,drop it!%d\n",__FUNCTION__, __LINE__);
			goto drop;
		}
	}

	psess = tcp_session_lookup(&tcp);
	if(NULL == psess)
	{
		if (flags == TCP_FLAG_SYN) /* 收到第一个报文，触发建立 session */
		{
			psess = tcp_session_create(&tcp);
			if(NULL == psess)
			{
				zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
				goto drop;
			}
		}
		else 
		{
			zlog_err ("%s,tcp packet flag is not syn, do not create tcp session,drop it%d\n",__FUNCTION__, __LINE__);
			goto drop;
		}
	}

	psess->flag = tcph->flag;
	psess->headlen = tcph->hlen;
	psess->seq = ntohl(tcph->seq);
	psess->ack_seq = ntohl(tcph->ack_seq);
	psess->alive_time_count = 0;/* 保活报文定时计数清0 */
	psess->timeout = 0;/* 会话超时定时计数清0 */

	if(psess->module_id == 0)
	{
		psess->module_id = module_id;
	}

	if(psess->dip == 0)
	{
		psess->dip = ip_cb->dip;
	}
	
	if(psess->seq_next < ntohl(tcph->ack_seq))
	{
		/* 两端同时发送报文避免seq被修改 */
		psess->seq_next = ntohl(tcph->ack_seq);
	}
	else
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d seq_next:0x%x,ack_seq:0x%x\n",__FUNCTION__, __LINE__,psess->seq_next,tcph->ack_seq);
	}	

	zlog_debug(FTM_DBG_TCP,"%s, %d psess->send_wnd %d,win_scale:0x%d  psess->seq 0x%x psess->ack_seq 0x%x psess->seq_next 0x%x psess->seq_next_recv 0x%x psess->flag 0x%x\n",
			__FUNCTION__, __LINE__,psess->send_wnd,psess->win_scale, psess->seq,psess->ack_seq,psess->seq_next,psess->seq_next_recv,psess->flag );

	tcp_rcv_process(pkt, psess, module_id);

	return NET_SUCCESS;
	
drop:	

	ftm_pkt_cnt_add(PKT_ACTION_DROP, PKT_TYPE_TCP);	
	pkt_free(pkt);
	return NET_FAILED;
}


/* 发送 tcp 数据报文，输入是 payload */
int tcp_pkt_send(void *data, int data_len, struct ip_control *ipcb, struct tcp_session *psess)
{
	struct pkt_buffer *pkt = NULL;
	int ret;

	if(psess == NULL)
	{
		return -1;
	}

	pkt = pkt_alloc(data_len + sizeof(struct pkt_buffer));
	if(!pkt)
	{
		zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
		return -1;
	}
	
	if(pkt_push(pkt, data_len))
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
		goto drop;
	}
    if(NULL != data)
    {
    	if(pkt_set(pkt, data, data_len))
    	{
    		zlog_debug("%s, %d \n",__FUNCTION__, __LINE__);
    		goto drop;
    	}
    }

	memcpy((char *)(&(pkt->cb)), (char *)ipcb, sizeof(struct ip_control));
	
    ret = tcp_encap(pkt,psess);
	if(ret)
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
		goto drop;
	}
	
	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_SEND, PKT_TYPE_TCP);
	ftm_pkt_cnt_add(PKT_ACTION_SEND, PKT_TYPE_TCP);

	/* 设置下一个发包的seq值 */
	psess->send_len = data_len;
	psess->seq_next += psess->send_len;
	/* ip_forward内部会丢弃报文 */
    return ip_forward(pkt);

drop:
	pkt_free(pkt);
	return NET_FAILED;		
}


/* 发送 tcp 控制报文，输入是报文类型 */
int tcp_send_control(int type, struct tcp_session *psess)
{
	struct pkt_buffer *pkt = NULL;	
	int ret;
	if(psess == NULL)
		return -1;

	psess->flag = 0x00;/* 首部长度为20 */
	switch(type)
	{
		case TCP_TYPE_FIN:
			psess->flag |= TCP_FLAG_FIN;
			break;	
		case TCP_TYPE_FIN_ACK:
			psess->flag |= TCP_FLAG_FIN | TCP_FLAG_ACK;
			break;				
		case TCP_TYPE_SYN:
			psess->flag |= TCP_FLAG_SYN;
			break;
		case TCP_TYPE_RESET:
			psess->flag |= TCP_FLAG_RST;
			break;		
		case TCP_TYPE_PUSH:
			psess->flag |= TCP_FLAG_PSH;
			break;			
		case TCP_TYPE_ACK:
			psess->flag |= TCP_FLAG_ACK;
			break;
		case TCP_TYPE_SYN_ACK:
			psess->flag |= TCP_FLAG_ACK | TCP_FLAG_SYN;
		default:
			break;			
	}	
	
	pkt = pkt_alloc(100);
	if(!pkt)
	{
		zlog_err("%s, %d pkt_alloc fail! \n",__FUNCTION__, __LINE__);
		return NET_FAILED;
	}
    ret = tcp_encap(pkt,psess);	
	if(ret != 0)
	{
		zlog_err("%s, %d tcp_encap fail! \n",__FUNCTION__, __LINE__);
		goto drop;
	}

	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_SEND, PKT_TYPE_TCP);
	ftm_pkt_cnt_add(PKT_ACTION_SEND, PKT_TYPE_TCP);
	
    return ip_forward(pkt);
	
drop:
	pkt_free(pkt);
	return NET_FAILED;		
}


/*
发送 tcp 数据报文，输入是 payload,TCP会话的每一端都包含一个32位（bit）的序列号，
该序列号被用来跟踪该端发送的数据量,当某个主机开启一个TCP会话时，他的初始序列号
是随机的，可能是0和4,294,967,295之间的任意值序列号为当前端成功发送的数据位数，
确认号为当前端成功接收的数据位数，SYN标志位和FIN标志位也要占1位
MSS: Maxitum Segment Size 最大分段大小
*/
int tcp_pkt_from_app_process(struct pkt_buffer *pkt)
{	
	struct tcp_session *psess = NULL;
	struct ip_control *ipcb = NULL; 
	struct tcp_session tcp;
	//int dataLen = 0;
	int ret;
	if(pkt == NULL)
		return -1;
	
	ipcb = (struct ip_control *)(&(pkt->cb));
	zlog_debug(FTM_DBG_TCP,"%s, %d ipcb->sip:0x%x\n",__FUNCTION__, __LINE__,ipcb->sip);

	memset(&tcp, 0 ,sizeof(struct tcp_session));
	tcp.sport = ipcb->dport;
	tcp.dport = ipcb->sport;
	tcp.vpn = ipcb->vpn;
	if(ipcb->pkt_type == PKT_TYPE_IP)
	{
		tcp.sip = ipcb->dip;
		tcp.dip = ipcb->sip;
		if(tcp.sip == 0 || tcp.sport == 0 || tcp.dport == 0)
		{
			zlog_err ("%s, tcp sip 0 or sport 0 or dport 0,drop it!%d\n",__FUNCTION__, __LINE__);
			goto drop;
		}
		tcp.v6session = FALSE;
	}
	else if(ipcb->pkt_type == PKT_TYPE_IPV6)
	{
		IPV6_ADDR_COPY(&tcp.sipv6, &ipcb->dipv6);	
		IPV6_ADDR_COPY(&tcp.dipv6, &ipcb->sipv6);
		if(ipv6_is_zeronet(&tcp.dipv6) || tcp.sport == 0 || tcp.dport == 0)
		{
			zlog_err ("%s, tcp sipv6 0 or sport 0 or dport 0,drop it!%d\n",__FUNCTION__, __LINE__);
			goto drop;
		}
		tcp.v6session = TRUE;
	}
	else
	{
		zlog_err ("%s, %d ipcb->pkt_type is unvalid\n",__FUNCTION__, __LINE__);
		goto drop;
	}
	zlog_debug(FTM_DBG_TCP,"%s, %d tcp.dip:0x%x，tcp.sip:0x%x,tcp.sport:0x%x, tcp.dport:0x%x, tcp.vpn:0x%x\n",
				__FUNCTION__, __LINE__,tcp.dip,tcp.sip,tcp.sport,tcp.dport, tcp.vpn);
	
	psess = tcp_session_lookup(&tcp);
	if(psess == NULL)
	{
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
		psess = XMALLOC(MTYPE_TCP, sizeof(struct tcp_session));
		if(psess == NULL)
		{
			zlog_err ("%s, %d XMALLOC fail!\n",__FUNCTION__, __LINE__);
			goto drop;
		}
		memset(psess, 0, sizeof(struct tcp_session));
		memcpy(psess, &tcp, sizeof(struct tcp_session));
		
		/*对一个给定的连接，初始化cwnd为1个报文段，ssthresh为65535个字节*/
		psess->headlen = 0x5;
		psess->cwnd = 1;
		psess->win_scale = 1;
		psess->ssthresh = 65535;
		psess->send_wnd = 0x2000;
		psess->recv_wnd = 9600;
		psess->send_len = 0;
		psess->status = TCP_STATUS_CLOSED;
		psess->send_list = NULL;
		psess->recv_list = NULL;
		psess->mss = TCP_MSS_DEFAULT;
		psess->ttl = ipcb->ttl;
		if(tcp_session_add(psess))
		{
			XFREE(MTYPE_TCP, psess);
			zlog_err ("tcp_session_add failed! %s, %d\n",__FUNCTION__, __LINE__);
			goto drop;
		}
		/* 一次握手 */
		ret = tcp_session_fsm(TCP_EVENT_SND_DATA, psess);	
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
		tcp_session_send_list_add(pkt, psess);
		goto drop;
	}

	if(psess->status != TCP_STATUS_ESTABLISHED)
	{
		zlog_debug(FTM_DBG_TCP,"psess->status = %d  %s, %d \n",psess->status,__FUNCTION__, __LINE__);
		psess->send_len = 0;
		tcp_session_send_list_add(pkt, psess);
		goto drop;
	}
	
	if(psess->send_wnd < 1400)
	{
		zlog_err("psess->send_wnd=%d %s, %d \n",psess->send_wnd,__FUNCTION__, __LINE__);
		psess->send_wnd = 1400;
	}


	
	psess->send_len = pkt->data_len;
	
	/* 添加发送报文到链表 */
	tcp_session_send_list_add(pkt, psess);	
	
	/* 没有发送的窗口或发送窗口不满足 */

	/* 报文发送出去 */
	psess->flag = TCP_FLAG_ACK + TCP_FLAG_PSH;
	psess->headlen = 0x5;

    ret = tcp_encap(pkt,psess);
	if(ret != 0)
	{
		zlog_err("tcp_encap failed %s, %d \n",__FUNCTION__, __LINE__);
		goto drop;
	}

	if(psess->send_wnd <= 0)
	{
		zlog_err("psess->send_wnd=%d %s, %d \n",psess->send_wnd,__FUNCTION__, __LINE__);
		goto drop;
	}
	
	ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_SEND, PKT_TYPE_TCP);
	ftm_pkt_cnt_add(PKT_ACTION_SEND, PKT_TYPE_TCP);

	/* 设置下一个发包的seq值 */
	psess->seq_next += psess->send_len;

	/* ip_forward内部会丢弃报文 */
    return ip_forward(pkt);
	
drop:	

	pkt_free(pkt);
	return 0;		
}


/* 解析 tcp 报文中的选项内容，解析结果保存到 session 中 */
extern struct bgp_tcp_connect_with_md5 bgp_peer_entry[PEER_MAX_NUM];
int tcp_decap_option(struct pkt_buffer *pkt, struct tcp_session *psess, int opt_len)
{
	uchar *opt = NULL;
	int len = 0;

	if((pkt == NULL) || (psess == NULL) || (opt_len == 0))
		return -1;
	
	opt = pkt->data;
	
	while(len < opt_len)
	{
		switch (*opt)
		{
			case TCP_OPT_EOL:  /* 选项结束 */
			{
				opt++;
				len++;
				break;
				/*goto out;*/
			}
			case TCP_OPT_NOP:  /* 1 字节 padding */
			{
				opt++;
				len++;
				break;
			}
			case TCP_OPT_MSS:  /* 最大 payload 长度 */
			{
				struct tcp_opt_mss *pmss = (struct tcp_opt_mss *)opt;
				if(psess->mss > ntohs(pmss->mss))
				{
					psess->mss = ntohs(pmss->mss);
				}
				opt = opt + sizeof(struct tcp_opt_mss);
				len = len + sizeof(struct tcp_opt_mss);
				break;
			}
			case TCP_OPT_WIN_SCALE:
			{
				struct tcp_opt_winscale  *pwinscale = (struct tcp_opt_winscale  *)opt;
				
				psess->win_scale = pwinscale->winscale;
				opt = opt + sizeof(struct tcp_opt_winscale);
				len = len + sizeof(struct tcp_opt_winscale);
				break;
			}
			case TCP_OPT_TIMESTAMP:
			{
				struct tcp_opt_timestamp *ptimestamp = (struct tcp_opt_timestamp *)opt;
				
				psess->timestampreply = ntohl(ptimestamp->timestamp);
				opt = opt + sizeof(struct tcp_opt_timestamp);
				len = len + sizeof(struct tcp_opt_timestamp);
				break;
			}
			case TCP_OPT_SACK_PERMIT:
			{
				/*struct tcp_sack  *psack = (struct tcp_sack  *)opt;*/
				
				opt = opt + sizeof(struct tcp_sack);
				len = len + sizeof(struct tcp_sack);
				break;
			}
#if TCP_MD5_OPTION_SET
			case TCP_OPT_MD5:
			{
				struct tcp_opt_md5 *popt = (struct tcp_opt_md5 *)opt;	
				struct tcphdr *tcph = NULL;
				char temp_buf[16]= {0};
				int flag = 0;
				char *pdata = (char *)pkt->data;
				
				if((psess->sport == TCP_PORT_BGP) || (psess->dport == TCP_PORT_BGP))
				{
					flag = bgp_peer_entry_lookup(psess->dip,psess->sip);
					if(flag == -1)
					{
						zlog_err("%s, %d err, can not find this peer \n",__FUNCTION__, __LINE__);
						return -1;
					}
					pkt_with_md5 = 1;
					tcph = (struct tcphdr *)(pkt->transport_header);
					if(0 != bgp_md5_digest_make(flag,tcph,(char *)(&pdata[opt_len]),pkt->data_len-opt_len,(char *)(&temp_buf),1))
					{
						zlog_err("%s, %d err, make digest err \n",__FUNCTION__, __LINE__);
						return -1;
					}
					
					if( 0 != memcmp(&popt->digest,&temp_buf,16) )
					{
						zlog_err("%s, %d err,Md5 confirm err %x %x %x %x %x %x %x %x \n",__FUNCTION__, __LINE__,
							temp_buf[0],temp_buf[1],temp_buf[2],temp_buf[3],temp_buf[4],temp_buf[5],temp_buf[6],temp_buf[7]);
					
						return -1;
					}
				}
				opt = opt + sizeof(struct tcp_opt_md5);
				len = len + sizeof(struct tcp_opt_md5);
				break;
			}
#endif
			default:
			{				
				zlog_debug(FTM_DBG_TCP,"%s, %d unknown tcp opt: %d \n",__FUNCTION__, __LINE__, *opt);
				return -1;
			}
		}
	}
	return 0;
	/*

out:
	 选项长度错误 
	if(len != opt_len)
		return -1;*/
	return 0;
}



/* 封装 tcp 选项，返回选项长度, -1 表示失败 */
int tcp_encap_option(struct pkt_buffer *pkt, struct tcp_session *psess, enum TCP_OPT opt)
{
	int len = 0;

	if((pkt == NULL) || (psess == NULL))
		return -1;
	
	switch (opt)
	{
		case TCP_OPT_EOL:  /* 选项结束 */
		{
			len = 1;			
			if (pkt_push(pkt, len))
			{
				return -1;
			}
			* (uchar *)pkt->data = TCP_OPT_EOL;
			break;
		}
		case TCP_OPT_NOP:  /* 1 字节 padding */
		{
			len = 1;			
			if (pkt_push(pkt, len))
			{
				return -1;
			}
			* (uchar *)pkt->data = TCP_OPT_NOP;
			break;
		}
		case TCP_OPT_MSS:  /* 最大 payload 长度 */
		{
			struct tcp_opt_mss *popt = NULL;
			
			len = sizeof(struct tcp_opt_mss);
			if (pkt_push(pkt, len))
			{
				return -1;
			}
			popt =  (struct tcp_opt_mss *)pkt->data;
			popt->kind = TCP_OPT_MSS;
			popt->length = len;
			popt->mss = htons(psess->mss);
			break;
		}
		case TCP_OPT_WIN_SCALE:
		{
			struct tcp_opt_winscale  *popt = NULL;			
			
			len = sizeof(struct tcp_opt_winscale);
			if (pkt_push(pkt, len))
			{
				return -1;
			}
			popt =  (struct tcp_opt_winscale *)pkt->data;
			popt->kind = TCP_OPT_WIN_SCALE;
			popt->length = len;
			popt->winscale = psess->win_scale;
			break;
		}
		case TCP_OPT_TIMESTAMP:
		{
			struct tcp_opt_timestamp *popt = NULL;			
			struct timeval time;
			
			len = sizeof(struct tcp_opt_timestamp);
			if (pkt_push(pkt, len))
			{
				return -1;
			}
			popt =  (struct tcp_opt_timestamp *)pkt->data;
			popt->kind = TCP_OPT_TIMESTAMP;
			popt->length = len;
			popt->timestampreply = htonl(psess->timestampreply);
				
			/* 设置报文的发送时间戳 */
			time_get_time (TIME_CLK_REALTIME, &time);
			popt->timestamp = time.tv_usec;
			break;
		}
#if TCP_MD5_OPTION_SET
		case TCP_OPT_MD5:
		{
			int flag = 0;
			struct tcp_opt_md5 *popt = NULL;
			struct tcphdr *tcph = (struct tcphdr *)pkt->data;
			char *pdata = (char *)pkt->data;
			len = sizeof(struct tcp_opt_md5);
			
			popt = (struct tcp_opt_md5 *)(&pdata[TCP_HEADER_SIZE]);
			popt->kind = TCP_OPT_MD5;
			popt->length = len;
			if((psess->sport == TCP_PORT_BGP) || (psess->dport == TCP_PORT_BGP))
			{
				flag = bgp_peer_entry_lookup(psess->dip,psess->sip);
				if(flag == -1)
				{
					return -1;
				}
				if(0 != bgp_md5_digest_make(flag,tcph,(char *)(&pdata[tcph->hlen*4]),pkt->data_len-tcph->hlen*4,(char *)(&popt->digest),0))
					return -1;
				/*
				zlog(NULL,NULL,"tcp_encap_option ......... 2 popt->digest \n \
					 %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n",
					 popt->digest[0],popt->digest[1],popt->digest[2],popt->digest[3],
					 popt->digest[4],popt->digest[5],popt->digest[6],popt->digest[7],
					 popt->digest[8],popt->digest[9],popt->digest[10],popt->digest[11],
					 popt->digest[12],popt->digest[13],popt->digest[14],popt->digest[15]);
			*/}
			break;
		}
#endif
		default:
		{				
			zlog_debug(FTM_DBG_TCP,"%s, %d unknown tcp opt: %d \n",__FUNCTION__, __LINE__, opt);
			return -1;
		}			
	}

	return len;
}

/* 封装 tcp 头 */
int tcp_encap(struct pkt_buffer *pkt, struct tcp_session *psess)
{
    struct ip_control *ipcb = NULL;	
	struct tcphdr *tcph = NULL;
	
	uint8_t flag = 0;
	uint8_t headLen = 0;
	uint8_t opt_datalen = 0;

	int ret = -1;
	if(psess == NULL)
		return -1;

	psess->send_wnd -= pkt->data_len;
	headLen = 5/*psess->headlen*/;
	flag = psess->flag;
	zlog_debug(FTM_DBG_TCP,"%s, %d flag:0x%x\n",__FUNCTION__, __LINE__,flag);
    
	if((psess->sport == TCP_PORT_LDP || psess->dport == TCP_PORT_LDP 
		|| psess->sport == TCP_PORT_BGP || psess->dport == TCP_PORT_BGP || psess->module_id == MODULE_ID_FILE) 
		&&(flag == TCP_FLAG_SYN || flag == (TCP_FLAG_ACK | TCP_FLAG_SYN)))
	{
		/* LDP的握手报文需要添加MSS */
		opt_datalen += tcp_encap_option(pkt,psess,TCP_OPT_MSS);
	}
	else if(psess->sport == TCP_PORT_OPENFLOW || psess->dport == TCP_PORT_OPENFLOW) 
	{
		if(flag == TCP_FLAG_SYN || flag == (TCP_FLAG_SYN | TCP_FLAG_ACK ))
		{
			opt_datalen += tcp_encap_option(pkt,psess,TCP_OPT_MSS);
		}
		else
		{
			opt_datalen += tcp_encap_option(pkt,psess,TCP_OPT_TIMESTAMP);
		}
		psess->flag += flag;
	}
#if TCP_MD5_OPTION_SET
	if((psess->sport == TCP_PORT_BGP) || (psess->dport == TCP_PORT_BGP))
	{
		ret = bgp_peer_entry_lookup(psess->dip,psess->sip);
		if(ret != -1)
		{
			headLen += 5;
		}
	}
#endif
	if(headLen*4 < TCP_HEADER_SIZE || headLen*4 > (TCP_HEADER_SIZE+TCP_OPTION_SIZE))
	{
		zlog_err ("%s, %d headLen %d \n",__FUNCTION__, __LINE__,headLen);
		return -1;
	}

    /* set tcp header */
	if (pkt_push(pkt, headLen*4))
	{
		zlog_err ("%s, %d headLen %d\n",__FUNCTION__, __LINE__,headLen);
		return -1;
	}

	headLen += opt_datalen/4;
	if(opt_datalen%4)
		headLen += 1;
	
    pkt->transport_header = pkt->data;
    tcph = (struct tcphdr *)(pkt->transport_header);
	tcph->sport   = htons(psess->dport);
	tcph->dport   = htons(psess->sport); 
	tcph->seq     = htonl(psess->seq_next);	
	tcph->ack_seq = htonl(psess->seq_next_recv);
	tcph->hlen    = headLen;                             
	tcph->reserv  = 0;
	tcph->flag    = psess->flag;
	tcph->window  = htons(psess->recv_wnd);	
	tcph->urg_ptr = htons(psess->urg_ptr);
	
#if TCP_MD5_OPTION_SET
	if(ret != -1)
	{
		tcp_encap_option(pkt,psess,TCP_OPT_MD5);
	}
#endif
	/*if(optdata != NULL)
		memcpy((char *)tcph+TCP_HEADER_SIZE, optdata, headLen*4-TCP_HEADER_SIZE);
	*/
	//psess->ack_last = psess->seq_next_recv;
	psess->seq_last = psess->seq_next;
	
	zlog_debug(FTM_DBG_TCP,"%s, %d tcph->flag:0x%x tcph->seq:0x%x tcph->ack_seq:0x%x headLen:0x%x\n",__FUNCTION__, __LINE__,
		tcph->flag,tcph->seq,tcph->ack_seq,headLen);
	
	ipcb = (struct ip_control *)(&(pkt->cb));

	if( 0 == ipcb->vpn )
	{
		ipcb->vpn = psess->vpn;
	}
	
	if(ipcb->ttl == 0)
	{
		if(psess->ttl != 0)
		{
			ipcb->ttl = psess->ttl;
		}
		else
			ipcb->ttl = IP_TTL_DEFAULT;
	}
	
	ipcb->protocol = IP_P_TCP;
	ipcb->is_changed = 1;	
	ipcb->chsum_enable = 1;	

	if(psess->v6session == TRUE)
	{
		IPV6_ADDR_COPY(&ipcb->sipv6, &psess->dipv6);	
		IPV6_ADDR_COPY(&ipcb->dipv6, &psess->sipv6);
		ipcb->frag_off = IP_FLAG_DF;
		ipcb->pkt_type = PKT_TYPE_IPV6;
	}
	else
	{
		ipcb->sip = psess->dip;
		ipcb->dip = psess->sip;
		ipcb->pkt_type = PKT_TYPE_IP;
	}
	tcph->checksum = 0;

	return 0;
}


/* 解析 tcp 头获取 sport 和 dport，返回 tcp 头长度 */
int tcp_decap(struct pkt_buffer *pkt)
{
	struct tcphdr     *tcph = NULL;
    struct ip_control *ipcb = NULL;
    int hlen = 0;

	zlog_debug (FTM_DBG_TCP, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__ );
	
    if (NULL == pkt)
    {
        zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
        return -1;
    }
	
    pkt->transport_header = pkt->data;	
    tcph = (struct tcphdr *)(pkt->transport_header);
	
    /* TCP 头长度范围 20 - 60，大于 20 表明有 option */
    hlen = tcph->hlen * 4;
	if((hlen < TCP_HEADER_SIZE) || (hlen > (TCP_HEADER_SIZE + TCP_OPTION_SIZE)))
	{
		return -1;
	}

	/* 检查端口合法性 */
	if((tcph->sport == 0) || (tcph->dport == 0))
		return -1;
	
    ipcb = (struct ip_control *)(&(pkt->cb));
	pkt->cb_type = PKT_TYPE_TCP;
	ipcb->sport = ntohs(tcph->sport);
	ipcb->dport = ntohs(tcph->dport);

    if(pkt_pull(pkt, TCP_HEADER_SIZE))
    {
		zlog_err("%s, %d pkt_pull fail!\n",__FUNCTION__, __LINE__);
		return -1;
    }
	
    return hlen;
}


/*
tcp 协议状态机:
客户端的状态可以用如下的流程来表示：
CLOSED->SYN_SENT->ESTABLISHED->FIN_WAIT_1->FIN_WAIT_2->TIME_WAIT->CLOSED
服务器的状态可以用如下的流程来表示：
CLOSED->LISTEN->SYN收到->ESTABLISHED->CLOSE_WAIT->LAST_ACK->CLOSED
*/
int tcp_session_fsm(enum TCP_EVENT event, struct tcp_session *psess)
{
	int rand = 0;
	int ret = 0;
	
	if(psess == NULL)
		return -1;

	if(event >= TCP_EVENT_MAX || psess->status >= TCP_STATUS_MAX)
	{
		zlog_notice("%s, %d \n",__FUNCTION__, __LINE__);
		return -1;
	}
	/*when received the SYN,but there is no BGP config, send rst!*/
	if(event == TCP_EVENT_RCV_SYN)
	{	
		if(((psess->dport == TCP_PORT_BGP) || (psess->sport == TCP_PORT_BGP)) && (bgp_port_swith == 0))
		{
			psess->seq_next_recv += 1;
			psess->seq_next = random ();
			ret = tcp_send_control(TCP_TYPE_RESET, psess);
			if(ret == 0)
			{				
				zlog_debug(FTM_DBG_TCP,"%s, %d no bgp config,drop it\n",__FUNCTION__, __LINE__);
			}
			goto drop;
		}
	}
	zlog_debug(FTM_DBG_TCP,"%s, %d event:0x%d, status:0x%x\n",__FUNCTION__, __LINE__,event, psess->status);
	switch(psess->status)
	{
		case TCP_STATUS_CLOSED:
		{
			zlog_debug(FTM_DBG_TCP,"%s, %d event:0x%x\n",__FUNCTION__, __LINE__,event);
			if(event == TCP_EVENT_SND_DATA)
			{
				/* 主动发送报文，发送SYN,进入SYN-SENT状态 */				
				srandom (time (NULL));
				psess->seq_next_recv = 0;
				psess->seq_next = random ();
				zlog_debug(FTM_DBG_TCP,"%s, %d seq_next:0x%x,seq_next_recv:0x%x\n",__FUNCTION__, __LINE__,psess->seq_next, psess->seq_next_recv);
				ret = tcp_send_control(TCP_TYPE_SYN, psess);
				if(ret == 0)
				{
					psess->status = TCP_STATUS_SYN_SENT;	
					//thread_add_timer(ftm_master, tcp_syn_timeout, psess, 60);	
				}
				else
				{
					return -1;
				}
				zlog_debug(FTM_DBG_TCP,"%s, %d psess->status:0x%x\n",__FUNCTION__, __LINE__,psess->status);
			}			
			break;
		}
		case TCP_STATUS_LISTEN:
		{
			if(event == TCP_EVENT_RCV_SYN)
			{	
				srandom (time (NULL));
				/* 接收SYN，发送SYN+ACK */
				psess->seq_next_recv += 1;
				psess->seq_next = random ();
				zlog_debug(FTM_DBG_TCP,"%s, %d seq:0x%x,ack:0x%x\n",__FUNCTION__, __LINE__,psess->seq_next, psess->seq_next_recv);
				ret = tcp_send_control(TCP_TYPE_SYN_ACK, psess);
				if(ret == 0)
				{
					psess->status = TCP_STATUS_SYN_RECEIVED;	
				}
				/* 建连时SYN超时问题 */
			}
			else if(event == TCP_EVENT_SND_DATA)
			{
				/* 主动发送报文，发送SYN */				
				srandom (time (NULL));
				psess->seq_next_recv = 0;
				psess->seq_next = random ();
				zlog_debug(FTM_DBG_TCP,"%s, %d seq:0x%x,ack:0x%x\n",__FUNCTION__, __LINE__,psess->seq_next, psess->seq_next_recv);
				ret = tcp_send_control(TCP_TYPE_SYN, psess);
				if(ret == 0)
				{
					psess->status = TCP_STATUS_SYN_SENT;	
					/* 应用进程关闭或超时 */
				}
				else
				{
					return -1;
				}				
				zlog_debug(FTM_DBG_TCP,"%s, %d event=%d\n",__FUNCTION__, __LINE__,event);
			}
			else
			{
				zlog_err("%s, %d event=%d\n",__FUNCTION__, __LINE__,event);
				goto drop;
			}
			break;
		}
		case TCP_STATUS_SYN_SENT:
		{
			if(event == TCP_EVENT_RCV_SYN)
			{
				srandom (time (NULL));
				rand = random ();
				/* 接收SYN，发送SYN+ACK */
				psess->seq_next_recv += 1;
				psess->seq_next = rand;
				ret = tcp_send_control(TCP_TYPE_ACK, psess);
				if(ret == 0)
				{
					psess->status = TCP_STATUS_SYN_RECEIVED;	
				}
			}
			else if(event == TCP_EVENT_RCV_SYN_ACK)
			{
				/* 接收SYN+ACK，发送ACK */
				psess->seq_next_recv += 1;
				zlog_debug(FTM_DBG_TCP,"%s, %d psess->seq_next_recv:0x%x\n",__FUNCTION__, __LINE__,psess->seq_next_recv);				
				ret = tcp_send_control(TCP_TYPE_ACK, psess);
				if(ret == 0)
				{
					psess->status = TCP_STATUS_ESTABLISHED;	
					psess->timeout = 0;
				}		
                tcp_session_change_notify_app(psess,IPC_OPCODE_CONNECT);
			}
			else if(event == TCP_EVENT_WAIT_TIMEOUT)
			{
				/* 在发送超时的情况下，会返回到CLOSED状态 */
				psess->status = TCP_STATUS_CLOSED;	
			}
			else if(event == TCP_EVENT_SND_FIN)
			{
				/* 在syn_sent状态下关闭tcp的情况下，会返回到CLOSED状态 */
				psess->status = TCP_STATUS_CLOSED;	
			}
			else
				goto drop;
			break;
		}
		case TCP_STATUS_SYN_RECEIVED:
		{
			if(event == TCP_EVENT_RCV_ACK)
			{
				psess->status = TCP_STATUS_ESTABLISHED;	
				psess->timeout = 0;
				tcp_session_change_notify_app(psess,IPC_OPCODE_CONNECT);
				tcp_session_rcv_list_sendto_app(psess);
					
			}
			else if(event == TCP_EVENT_RCV_RESET)
			{
				/* 如果收到RST包，会返回到LISTEN状态 */
				psess->status = TCP_STATUS_LISTEN;
			}
			else if(event == TCP_EVENT_SND_FIN)
			{
				/* 应用进程关闭发FIN，SYN_收到->FIN_WAIT_1 */
				psess->status = TCP_STATUS_FIN_WAIT_1;
			}			
			else
				goto drop;
			break;
		}
		case TCP_STATUS_ESTABLISHED:
		{
			if(event == TCP_EVENT_RCV_FIN)
			{
				/* 接收FIN M，发送ACK M+1 */
				zlog_debug(FTM_DBG_TCP,"%s, %d event=%d\n",__FUNCTION__, __LINE__,event);
				psess->seq_next_recv += 1;
				ret = tcp_send_control(TCP_TYPE_ACK, psess);
				if(ret == 0)
				{
					psess->status = TCP_STATUS_CLOSE_WAIT;	
				}	
				
				/* 等待应用进程关闭 */
				tcp_session_send_list_clear(psess);
				tcp_session_rcv_list_clear(psess);
				ret = tcp_send_control(TCP_TYPE_FIN, psess);
				if(ret == 0)
				{
					psess->status = TCP_STATUS_LAST_ACK;	
				}							
			}	
			else if(event == TCP_EVENT_RCV_FIN_ACK)
			{
				/* 接收FIN M，发送ACK M+1 */
				psess->seq_next_recv += 1;
				
				zlog_debug(FTM_DBG_TCP,"%s, %d event=%d\n",__FUNCTION__, __LINE__,event);
				//ret = tcp_send_control(TCP_TYPE_FIN_ACK, psess);
				ret = tcp_send_control(TCP_TYPE_ACK, psess);
				if(ret == 0)
				{
					psess->status = TCP_STATUS_CLOSE_WAIT;	
				}	
				
				/* 等待应用进程关闭 */
				tcp_session_send_list_clear(psess);
				tcp_session_rcv_list_clear(psess);
				ret = tcp_send_control(TCP_TYPE_FIN_ACK, psess);
				if(ret == 0)
				{
					psess->status = TCP_STATUS_LAST_ACK;	
				}							
				zlog_debug(FTM_DBG_TCP,"%s, %d TCP_TYPE_FIN!!!\n",__FUNCTION__, __LINE__);
			}	
			else if(event == TCP_EVENT_RCV_ACK)
			{
				/* 接收报文，发送应答 */
				ret = tcp_send_control(TCP_TYPE_ACK, psess);
			}
			else if(event == TCP_EVENT_SND_FIN)
			{
				/* 发送FIN+ACK */				
				zlog_debug(FTM_DBG_TCP,"%s, %d event=%d\n",__FUNCTION__, __LINE__,event);
				ret = tcp_send_control(TCP_TYPE_FIN_ACK, psess);
				psess->status = TCP_STATUS_FIN_WAIT_1;	
			}
			else if(event == TCP_EVENT_RCV_RESET)
			{
				/*如果接收方处于SYN-RECEIVED状态，而且以前处于LISTEN状态，接收方返回LISTEN状态，
				否则接收方关闭连接进入CLOSED状态。当接收方处于其它状态时，直接关闭连接回到CLOSED状态。*/
				tcp_session_change_notify_app(psess,IPC_OPCODE_DISCONNECT);
				tcp_session_send_list_clear(psess);
				tcp_session_rcv_list_clear(psess);
				tcp_session_delete(psess);
			}
			else
			{
				goto drop;
			}
			break;
		}
		case TCP_STATUS_FIN_WAIT_1:
		{
			if(event == TCP_EVENT_RCV_ACK)
			{
				/* 接收ACK，发送无 */
				psess->status = TCP_STATUS_FIN_WAIT_2;				
			}	
			else if(event == TCP_EVENT_RCV_FIN) 
			{
				/* 接收FIN M，发送ACK M+1 */
				psess->seq_next_recv += 1;
				ret = tcp_send_control(TCP_TYPE_ACK, psess);
				if(ret == 0)
				{
					psess->status = TCP_STATUS_CLOSING;	
				}
			}
			else if(event == TCP_EVENT_RCV_FIN_ACK) 
			{
				/* 接收FIN+ACK，发送ACK M+1 */
				/* 接收FIN M，发送ACK M+1 */
				psess->seq_next_recv += 1;
				ret = tcp_send_control(TCP_TYPE_ACK, psess);
				if(ret == 0)
				{
					psess->status = TCP_STATUS_TIME_WAIT;	
				}
			}	
			else
				goto drop;
			break;
		}
		case TCP_STATUS_FIN_WAIT_2:
		{
			if(event == TCP_EVENT_RCV_FIN_ACK || event == TCP_EVENT_RCV_FIN)
			{
				/* 接收FIN M，发送ACK M+1 */
				psess->seq_next_recv += 1;
				ret = tcp_send_control(TCP_TYPE_ACK, psess);
				if(ret == 0)
				{
					psess->status = TCP_STATUS_TIME_WAIT;	
				}
			}
			else
				goto drop;			
			break;
		}	
		case TCP_STATUS_CLOSING:
		{
			if(event == TCP_EVENT_RCV_ACK)
			{
				/* 接收ACK，发送无 */
				psess->status = TCP_STATUS_TIME_WAIT;	
			}
			else
				goto drop;			
			break;
		}	
		case TCP_STATUS_TIME_WAIT:
		{
			if(event == TCP_EVENT_WAIT_TIMEOUT)
			{
				psess->status = TCP_STATUS_CLOSED;	
			}
			else
				goto drop;
			break;
		}
		case TCP_STATUS_LAST_ACK:
		{
			/* 接收ACK，发送无 */
			if(event == TCP_EVENT_RCV_ACK || event == TCP_EVENT_RCV_RESET)
			{
				psess->status = TCP_STATUS_CLOSED;	
				tcp_session_change_notify_app(psess,IPC_OPCODE_DISCONNECT);
				tcp_session_delete(psess);
			}
			else
				goto drop;
			break;
		}		
		default:
			break;	
	}

	return 0;
drop:
	return -1;
}


/* 处理 获取端口 消息 */
int tcp_port_alloc_for_app(struct ipc_msghdr_n *phdr,void *pdata, int data_len, int data_num, uint16_t sender_id, enum IPC_OPCODE opcode)
{
	uint32_t port;
	int ret;
	if(pdata == NULL)
		return -1;

	if(IPC_OPCODE_GET == opcode)
	{
		port = index_alloc(INDEX_TYPE_TCP_PORT) + TCP_PORT_RESERVE - 1;

	    /*ret = ipc_send_reply ( &port, 2, sender_id, MODULE_ID_FTM, IPC_TYPE_PACKET, PKT_SUBTYPE_TCP_PORT, 0);*/
		ret =  ftm_msg_send_reply( &port, sizeof(uint32_t), phdr, 0);
		
		if ( ret < 0 )
	    {
	        zlog_err ( "[%s %d] ipc_send_reply fail!", __FUNCTION__, __LINE__);
	    }	
	}
	else if(opcode == IPC_OPCODE_DELETE)
	{
		port = * (uint32_t *)pdata;
		index_free(INDEX_TYPE_TCP_PORT, port - TCP_PORT_RESERVE + 1);
	}
	
	return 0;
}


/* 处理 TCP 连接和关闭的消息 */
int tcp_session_open_for_app(struct ipc_msghdr_n *phdr,void *pdata, int data_len, int data_num, uint16_t sender_id, enum IPC_OPCODE opcode)
{
	struct tcp_session *psess = NULL;
	struct ip_proto *ptcp = NULL;
	struct tcp_session tcp;
	int ret;

	if(pdata == NULL)
	{
		goto drop;
	}
	
	ptcp = (struct ip_proto *)pdata;
	if(IPC_OPCODE_ADD == opcode)
	{   
		memset(&tcp, 0 ,sizeof(struct tcp_session));
	    tcp.module_id = sender_id;
		tcp.sport = ptcp->dport;
		tcp.dport = ptcp->sport;

		if(ptcp->type == PROTO_TYPE_IPV6)
		{
			
			IPV6_ADDR_COPY(&tcp.sipv6, &ptcp->dipv6);	
			IPV6_ADDR_COPY(&tcp.dipv6, &ptcp->sipv6);
			if(ipv6_is_zeronet(&tcp.dipv6) || tcp.sport == 0 || tcp.dport == 0)
			{
				zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
				goto drop;
			}	
			tcp.v6session = TRUE;
		}
		else 
		{
			tcp.sip = ptcp->dip;
			tcp.dip = ptcp->sip;
			if(tcp.sip == 0 || tcp.sport == 0 || tcp.dport == 0)
			{
				zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
				goto drop;
			}	
			tcp.v6session = FALSE;
		}
		
		zlog_debug(FTM_DBG_TCP,"%s, %d tcp.sip:0x%x tcp.dip:0x%x  tcp.sport:0x%x tcp.dport:0x%x ",
			__FUNCTION__, __LINE__,tcp.dip,tcp.sip,tcp.sport,tcp.dport);
		
		psess = tcp_session_lookup(&tcp);
		if(psess != NULL)
		{
			zlog_err ("%s, %d psess != NULL\n",__FUNCTION__, __LINE__);
			goto drop;
		}

		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
		psess = XMALLOC(MTYPE_TCP, sizeof(struct tcp_session));
		if(psess == NULL)
		{
			zlog_err ("%s, %d XMALLOC fail!\n",__FUNCTION__, __LINE__);
			goto drop;
		}
		memset(psess, 0, sizeof(struct tcp_session));
		memcpy(psess, &tcp, sizeof(struct tcp_session));
		
		/*对一个给定的连接，初始化cwnd为1个报文段，ssthresh为65535个字节*/
		psess->cwnd = 1;
		psess->win_scale = 1;
		psess->recv_wnd = 8192;
		psess->ssthresh = 65535;
		psess->send_wnd = 0x2000;
		psess->status = TCP_STATUS_CLOSED;
		psess->send_list = NULL;
		psess->recv_list = NULL;
		psess->mss = TCP_MSS_DEFAULT;

		if(tcp_session_add(psess))
		{
			zlog_err ("%s, %d tcp_session_add failed\n",__FUNCTION__, __LINE__);
			goto drop;
		}	
		
		/* 一次握手 */
		ret = tcp_session_fsm(TCP_EVENT_SND_DATA, psess);
		if(ret != 0)
		{
			zlog_err ("%s, %d tcp fsm fail!\n",__FUNCTION__, __LINE__);
			tcp_session_delete(psess);
			goto drop;
		}
	    /*ret = ipc_send_ack ( sender_id, MODULE_ID_FTM, IPC_TYPE_PACKET, PKT_SUBTYPE_TCP_CONNECT, 0);*/
		ret = ftm_msg_send_ack(phdr,0);
		if ( ret < 0 )
	    {
	        zlog_err ( "[%s %d] ipc_send_ack fail!", __FUNCTION__, __LINE__);
	    }	
		return 0;
drop:		
		zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
	    /*ret = ipc_send_noack(1, sender_id, MODULE_ID_FTM, IPC_TYPE_PACKET, PKT_SUBTYPE_TCP_CONNECT, 0);*/
		ret = ftm_msg_send_noack(1,phdr, 0);
	    if ( ret < 0 )
	    {
	        zlog_err ( "[%s %d] ipc_send_noack fail!", __FUNCTION__, __LINE__);
	    }	
		return -1;
	}
	else if(IPC_OPCODE_DELETE == opcode)
	{
		ret = tcp_session_close(ptcp);
		if(ret < 0)
		{
		    /*ret = ipc_send_noack(1, sender_id, MODULE_ID_FTM, IPC_TYPE_PACKET, PKT_SUBTYPE_TCP_CONNECT, 0);*/
			ret = ftm_msg_send_noack(1,phdr, 0);
			if ( ret < 0 )
		    {
		        zlog_err ( "[%s %d] ipc_send_noack fail!", __FUNCTION__, __LINE__);
		    }
		}
		else
		{
		    /*ret = ipc_send_ack ( sender_id, MODULE_ID_FTM, IPC_TYPE_PACKET, PKT_SUBTYPE_TCP_CONNECT, 0);*/
			ret = ftm_msg_send_ack(phdr,0);
			if ( ret < 0 )
		    {
		        zlog_err ( "[%s %d] ipc_send_ack fail!", __FUNCTION__, __LINE__);
		    }	
		}
	}
	
	return 0;
}


/* 新建或关闭 tcp 连接 通知给app*/
int tcp_session_change_notify_app(struct tcp_session *psession,enum IPC_OPCODE opcode)
{
	struct ipc_mesg_n *pSndMsg = NULL;
	
	int ret = 0;
	if( psession->module_id != MODULE_ID_MPLS && 
		psession->module_id != MODULE_ID_FILE && 
		psession->module_id != MODULE_ID_VTY)
		{
			zlog_debug (FTM_DBG_TCP,"%s, %d psession->module_id:%d !\n",__FUNCTION__, __LINE__, psession->module_id );
			return -1;

		}
		
	zlog_debug (FTM_DBG_TCP,"%s, %d psession->module_id:%d psession->dport:%d psession->sport:%d psession->sip:0x%x  psession->dip:0x%x!\n",
		__FUNCTION__, __LINE__, psession->module_id ,psession->dport ,psession->sport ,psession->sip ,psession->dip);

	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + sizeof(struct tcp_session), MODULE_ID_FTM);
	if(pSndMsg != NULL)
	{
		pSndMsg->msghdr.data_len	= sizeof(struct tcp_session);
		pSndMsg->msghdr.module_id	= psession->module_id;
		pSndMsg->msghdr.sender_id	= MODULE_ID_FTM;
		pSndMsg->msghdr.msg_type	= IPC_TYPE_FTM;
		pSndMsg->msghdr.msg_subtype = PKT_SUBTYPE_TCP_CONNECT;
		pSndMsg->msghdr.data_num	= 1;
		pSndMsg->msghdr.opcode		= opcode;
			
		if(psession)
		{
			memcpy(pSndMsg->msg_data, psession, sizeof(struct tcp_session));
		}
		
		/*send info*/
		ret = ipc_send_msg_n1(pSndMsg, sizeof(struct ipc_mesg_n) + sizeof(struct tcp_session));
		if(ret)
		{
			zlog_err("%s[%d],ipc_send_msg_n1 failed\n",__FUNCTION__,__LINE__);
			ipc_msg_free(pSndMsg,MODULE_ID_FTM);
			FTM_PKT_ZLOG_ERR("Ipc send error!\n");
			return ret;
		}
	}
	
	return NET_SUCCESS;
}

int tcp_session_send_keepalive(struct tcp_session *psess)
{
	struct ip_control ipcb = {0}; 
	//char data[] = {0x00};
	zlog_debug(FTM_DBG_TCP,"%s, %d \n",__FUNCTION__, __LINE__);
	
	memset(&ipcb,0,sizeof(struct ip_control));
	psess->seq_next = psess->seq_next - 1;
	if(tcp_pkt_send(NULL, 0,&ipcb,psess) == NET_FAILED)
	{
		zlog_err("%s, %d tcp pkt_send failed!\n",__FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}


int tcp_pkt_retransmit (struct pktInfo *pPkt, struct tcp_session *psess)
{
	struct ip_control *ipcb = NULL; 
	struct pkt_buffer *pkt = NULL;
	int ret = 0;
	
	psess->headlen = 0x5;
	psess->flag = TCP_FLAG_ACK | TCP_FLAG_PSH;

	if(pPkt->data == NULL)
	{
		zlog_err("%s, %d \n",__FUNCTION__, __LINE__);
		return -1;
	}
	pkt = (struct pkt_buffer *)pPkt->data;
	ipcb = (struct ip_control *)(&(pkt->cb));

	psess->seq_next = pPkt->waitSeq - pPkt->dataLen;

	zlog_debug(FTM_DBG_TCP,"%s, %d\n",__FUNCTION__, __LINE__);
	ret = tcp_pkt_send(pkt->data, pkt->data_len, ipcb, psess);
	if(ret != 0)
	{
		zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
		return -1;
	}
	
	return 0;
}

int tcp_config_write ( struct vty *vty)
{
	vty_out ( vty, "tcp %s", VTY_NEWLINE ); 
	if(tcp_keepalive_switch != 0)
	{
		vty_out(vty, "    tcp keepalive enable%s", VTY_NEWLINE);
	}
	if(tcp_keepalive_time != 1200)
	{
		vty_out(vty, "    tcp keepalive-time %d %s",tcp_keepalive_time*3 , VTY_NEWLINE);
	}
	vty_out(vty, "!%s", VTY_NEWLINE);

    return ERRNO_SUCCESS; 
}

