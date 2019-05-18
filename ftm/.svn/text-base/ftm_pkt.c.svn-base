/*
*    packet receive and send queue 管理模块
*
*/
#include <string.h>
#include <unistd.h>
#include <lib/pkt_buffer.h>
#include <lib/module_id.h>
#include <lib/zassert.h>
#include <lib/log.h>
#include <lib/hash1.h>
#include <lib/thread.h>
#include <lib/msg_ipc_n.h>

#include "proto_reg.h"
#include "ftm_pkt.h"
#include "pkt_eth.h"
#include "pkt_ip.h"
#include "pkt_udp.h"
#include "pkt_tcp.h"
#include "pkt_mpls.h"
#include "pkt_netif.h"
#include "ftm.h"
#include "ftm_pkt.h"
#include "ftm_arp.h"

struct pkt_count ftm_pkt_cnt;   /* FTM 的收发包统计 */
struct pkt_debug ftm_pkt_dbg;   /* FTM 的报文调试开关 */
int ftm_pkt_rcv_queue[8];       /*packet IPC queue receive from hsl */
int ftm_pkt_send_queue;         /*packet IPC queue send to hal */
int ftm_pkt_rcv_queue_weight[8]={8,7,6,5,4,3,2,1};/*weight of each recv packet queue*/
int rcv_queue_total_weight;/*total weight of recv packet queue*/

/* 初始化报文统计 */
static void ftm_pkt_count_init(void) 
{
	memset(&ftm_pkt_cnt, 0, sizeof(struct pkt_count));
}


/* 初始化报文调试开关 */
static void ftm_pkt_debug_init(void) 
{
	memset(&ftm_pkt_dbg, 0, sizeof(struct pkt_debug));
}


/* init the packet ipc queue */
static void ftm_pkt_ipc_init(void)
{
	#if 0
	int id = 0;
	int key = IPC_PACKET_HSL0;

    /* ipc ftm_pkt_rcv_queue for receive packet from hsl */
	for(int i= 0; i< 8; i++) 
	{
        id = ipc_connect(key);
		if(id < 0)
		{
            FTM_PKT_ZLOG_ERR("Ipc connect failed!\n");
			return;
		}
		
        ftm_pkt_rcv_queue[i] = id;
		key++;
            rcv_queue_total_weight += ftm_pkt_rcv_queue_weight[i];
	}

    /* ipc for send packet to hal */
	ftm_pkt_send_queue = ipc_connect(IPC_PACKET_HAL);
    if (ftm_pkt_send_queue < 0)
    {
        FTM_PKT_ZLOG_ERR("Ipc connect failed!\n");
        return;
    }
	#endif
}


/* 初始化报文相关的数据 */
void ftm_pkt_init(void)
{
	ftm_pkt_count_init();
	ftm_pkt_debug_init();
	ftm_pkt_ipc_init();	
	proto_hash_init();
	pkt_list_init(); /* init packet buffer list */
	tcp_init();
	udp_init();
	ip_init();
}


int ftm_pkt_rcv_n(struct ipc_mesg_n *pmesg)
{
	//int i = 0;
	//struct ipc_msghdr *phdr = NULL;
	struct ipc_msghdr_n *phdr = NULL;
	struct pkt_buffer *pkt = NULL;	
	struct pkt_buffer *pkt_old = NULL;
	int bufszie = 0;

	phdr = &(pmesg->msghdr);

	pkt_old = (struct pkt_buffer *)pmesg->msg_data;
	if(pkt_old->buf_size != phdr->data_len)
	{
        FTM_PKT_ZLOG_ERR("Pkt error!\n");
		return -1;
	}
	
	pkt = pkt_alloc(pkt_old->buf_size - PKT_HDR_SIZE);
	if(!pkt)
	{
		FTM_PKT_ZLOG_ERR("pkt_alloc failed!\n");
		return -1;
	}

	bufszie = pkt->buf_size;/*var to avoid old buffsize override new pkt buffersize*/
	memcpy((void *)pkt, (void *)pkt_old, pkt_old->buf_size);
    pkt->next = NULL;
    pkt->prev = NULL;
    pkt->transport_header = NULL;
	pkt->network_header = NULL;
	pkt->link_header = NULL;
	pkt->mpls_header = NULL;
	pkt->buf_size = bufszie;
	pkt->data = (void *)((uint32_t)pkt + pkt->data_offset);

	if((pkt->cb_type == PKT_TYPE_INVALID) || (pkt->cb_type == PKT_TYPE_ETH))/* 报文有 eth 头 */
	{
		eth_rcv(pkt);
	}
	else if(pkt->cb_type == PKT_TYPE_IP)   /* 报文从 ip 头开始，没有 eth 头 */
	{
		ip_rcv(pkt);
	}
	else if(pkt->cb_type == PKT_TYPE_MPLS) /* mpls 报文，从标签开始, 没有 eth 头 */
	{
		mpls_rcv(pkt);
	}
    else
    {
        FTM_PKT_ZLOG_ERR("Pkt type error!\n");
        pkt_free(pkt);
    }
	return 0;
}

/*
从芯片接收报文，走软转发流程
*/
#if 0
int ftm_pkt_rcv(struct ipc_pkt *pmesg)
{
	struct ipc_msghdr *phdr = NULL;
	struct pkt_buffer *pkt = NULL;	
	struct pkt_buffer *pkt_old = NULL;
    //int total_token = rcv_queue_total_weight * 10;
    int queue_token = 0;
	int bufszie = 0;
    int nullcou = 0;

	phdr = &(pmesg->msghdr);

	/* 先接收优先级高的报文 ，按照从 7 到 0 的顺序 */
	for(int i=7; i>=0; i--) 
	{
        queue_token = 10 * ftm_pkt_rcv_queue_weight[i];
		while(queue_token--)
		{
			if(-1 == ipc_recv_pkt(ftm_pkt_rcv_queue[i], pmesg, MODULE_ID_FTM))
			{
			    nullcou++;
                
				break;  /* 退出 while 循环 */
			}

			pkt_old = (struct pkt_buffer *)pmesg->msg_data;
			if(pkt_old->buf_size != phdr->data_len)
			{
                FTM_PKT_ZLOG_ERR("Pkt error!\n");
				continue;
			}
			
			pkt = pkt_alloc(pkt_old->buf_size - PKT_HDR_SIZE);
			if(!pkt)
			{
				FTM_PKT_ZLOG_ERR("pkt_alloc failed!\n");
				continue;
			}

			bufszie = pkt->buf_size;/*var to avoid old buffsize override new pkt buffersize*/
			memcpy((void *)pkt, (void *)pkt_old, pkt_old->buf_size);
            pkt->next = NULL;
            pkt->prev = NULL;
            pkt->transport_header = NULL;
			pkt->network_header = NULL;
			pkt->link_header = NULL;
			pkt->mpls_header = NULL;
			pkt->buf_size = bufszie;
			pkt->data = (void *)((uint32_t)pkt + pkt->data_offset);

			if((pkt->cb_type == PKT_TYPE_INVALID) || (pkt->cb_type == PKT_TYPE_ETH))/* 报文有 eth 头 */
			{
				eth_rcv(pkt);
			}
			else if(pkt->cb_type == PKT_TYPE_IP)   /* 报文从 ip 头开始，没有 eth 头 */
			{
				ip_rcv(pkt);
			}
			else if(pkt->cb_type == PKT_TYPE_MPLS) /* mpls 报文，从标签开始, 没有 eth 头 */
			{
				mpls_rcv(pkt);
			}
            else
            {
                FTM_PKT_ZLOG_ERR("Pkt type error!\n");
                pkt_free(pkt);
            }
		}
	}

    return((nullcou == 8) ? -1 : 0);
	//return 0;
}
#endif

/* 发送报文给 hal */
int ftm_pkt_send_n(struct pkt_buffer *pkt)
{
	int ret = 0;
	struct ipc_mesg_n *pSndMsg = NULL;

	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + pkt->buf_size, MODULE_ID_FTM);
	if(pSndMsg != NULL)
	{
		pkt->data_offset = (char *)pkt->data - (char *)pkt;
		
		pSndMsg->msghdr.data_len	= pkt->buf_size;
		pSndMsg->msghdr.module_id	= MODULE_ID_HAL;
		pSndMsg->msghdr.sender_id	= MODULE_ID_FTM;
		pSndMsg->msghdr.msg_type	= IPC_TYPE_PACKET;
		pSndMsg->msghdr.msg_subtype = PKT_TYPE_RAW;
		pSndMsg->msghdr.data_num	= 1;
		pSndMsg->msghdr.opcode		= 0;
			
		if(pkt)
		{
			memcpy(pSndMsg->msg_data, pkt, pkt->buf_size);
		}
		
		/*send info*/
		ret = ipc_send_msg_n1(pSndMsg, sizeof(struct ipc_mesg_n) + pkt->buf_size);
		if(ret)
		{
			zlog_err("%s[%d],ipc_send_msg_n1 failed\n",__FUNCTION__,__LINE__);
			ipc_msg_free(pSndMsg,MODULE_ID_FTM);
			FTM_PKT_ZLOG_ERR("Ipc send error!\n");
			goto drop;
		}
	}
	
	return NET_SUCCESS;
	
drop:
	return NET_FAILED;
}


/* 发送报文给 hal */
#if 0
int ftm_pkt_send(struct pkt_buffer *pkt)
{
	static struct ipc_msghdr msghdr;
	int ret = -1;

	if(ftm_pkt_send_queue <= 0)
	{
		ftm_pkt_send_queue = ipc_connect(IPC_PACKET_HAL);
        if (ftm_pkt_send_queue <= 0)
        {
            FTM_PKT_ZLOG_ERR("Ipc connect failed!\n");
            return NET_FAILED;
        }
	}
	
	pkt->data_offset = (char *)pkt->data - (char *)pkt; /* 计算 data 的 offset，用于接收时恢复 data */
	
	memset ( &msghdr, 0, sizeof ( struct ipc_msghdr ) );
    msghdr.data_len = pkt->buf_size;	
	msghdr.msg_type = IPC_TYPE_PACKET;
	msghdr.msg_subtype = PKT_TYPE_RAW;
	msghdr.module_id = MODULE_ID_HAL;	
	msghdr.sender_id = MODULE_ID_FTM;
	msghdr.data_num = 1;
	msghdr.opcode = 0;

    ret = ipc_send(ftm_pkt_send_queue, &msghdr, pkt);	
	if(ret < 0)
	{
		usleep(10000);//发送失败让出 CPU 10ms
        FTM_PKT_ZLOG_ERR("Ipc send error!\n");
		goto drop;
	}
	
	return NET_SUCCESS;
	
drop:
	return NET_FAILED;
}
#endif

/* 接收 app 发送的报文 */
int ftm_pkt_rcv_from_app_n(struct ipc_mesg_n *pmesg)
{
	//struct ipc_msghdr *phdr = NULL;
	struct ipc_msghdr_n *phdr = NULL;
	struct pkt_buffer *pkt = NULL;	
	struct ip_control *ipcb = NULL;	
	struct mpls_control *mplscb = NULL;	
	struct eth_control  *ethcb = NULL;	
	struct raw_control  *rawcb = NULL;
    int data_len = 0;


	phdr = &(pmesg->msghdr);
	if(phdr->msg_type != IPC_TYPE_PACKET)
	{
        FTM_PKT_ZLOG_ERR("Pkt error!\n");
		return -1;
	}

	/* 收到的数据，第一部分是 control，第二部分是 data */
	data_len = phdr->data_len - PKT_CB_SIZE;
	pkt = pkt_alloc(data_len + 100);/*100bytes used to fill layer2--layer4 header*/
	if(!pkt)
	{
        FTM_PKT_ZLOG_ERR("Pkt_alloc failed!\n");
		return -1;
	}

	pkt_push(pkt, data_len);		
	memcpy((void *)&(pkt->cb), (void *)(pmesg->msg_data), PKT_CB_SIZE);	       /* get control block */
	memcpy((void *)(pkt->data), (void *)(pmesg->msg_data + PKT_CB_SIZE), data_len);/* get data */
	pkt->cb_type = phdr->msg_subtype;

	/* 根据控制信息类型，走不同的转发流程 */
    switch (pkt->cb_type)
    {
		case PKT_TYPE_TCP:
		{
			ipcb = (struct ip_control *)&(pkt->cb);
			ipcb->is_changed = 1;
			if(ipcb->dip)
			{
				ipcb->pkt_type = PKT_TYPE_IP;
				tcp_pkt_from_app_process(pkt);  /* 发送 tcp 报文*/
			}
			else if(ipv6_is_valid(&ipcb->dipv6))
			{
				ipcb->pkt_type = PKT_TYPE_IPV6;
				ipcb->frag_off = IP_FLAG_DF; /* 暂不支持分片 */
				tcp_pkt_from_app_process(pkt);  /* 发送 tcp 报文*/
			}
			else
			{
				pkt_free(pkt);
			}
			break;
		}
		case PKT_TYPE_UDP:	   /* 发送 udp 报文 */
		{
			ipcb = (struct ip_control *)&(pkt->cb);
			ipcb->is_changed = 1;				
			if(ipcb->dip)
			{
				ipcb->pkt_type = PKT_TYPE_IP;
				udp_forward(pkt);  /* 发送 udp 报文*/
			}
			else if(ipv6_is_valid(&ipcb->dipv6))
			{
				ipcb->pkt_type = PKT_TYPE_IPV6;					
				ipcb->frag_off = IP_FLAG_DF; /* 暂不支持分片 */
				udp_forward(pkt);  /* 发送 udp 报文*/
			}				
			else
			{
				pkt_free(pkt);
			}
			break;
		}
    	case PKT_TYPE_IP:      /* 指定目的 IP 发送 IPv4 单播报文*/
		{ 
			ipcb = (struct ip_control *)&(pkt->cb);				
			ipcb->is_changed = 1;
			if(ipcb->dip)
			{
				ipcb->pkt_type = PKT_TYPE_IP;
				ip_forward(pkt);
			}
			else
			{
				pkt_free(pkt);
			}
			break;
    	}			
    	case PKT_TYPE_IPV6:    /* 指定目的 IP 发送 IPv6 单播报文*/
		{ 
			ipcb = (struct ip_control *)&(pkt->cb);
			ipcb->is_changed = 1;				
			if(ipv6_is_valid(&ipcb->dipv6))
			{
				ipcb->pkt_type = PKT_TYPE_IPV6;					
				ipcb->frag_off = IP_FLAG_DF; /* 暂不支持分片 */
				ip_forward(pkt);
			}				
			else
			{
				pkt_free(pkt);
			}
			break;
    	}			
    	case PKT_TYPE_IPBC:    /* 指定出接口发送 IPv4 广播报文*/
		{
			ipcb = (struct ip_control *)&(pkt->cb);
            if(ipcb->ifindex)
            {
				ipcb->is_changed = 1;					
				ipcb->pkt_type = PKT_TYPE_IPBC;
				ip_output(pkt);
            }
            else
            {
                FTM_PKT_ZLOG_ERR("IPBC pkt send error!\n");                  
                pkt_free(pkt);
            }
			break;
    	}
    	case PKT_TYPE_IPMC:     /* 指定目的 IP 或出接口发送 IPv4 组播报文*/
		{
			ipcb = (struct ip_control *)&(pkt->cb);
			ipcb->is_changed = 1;				
			if(ipcb->ifindex)   /* 指定出接口发送组播报文*/
			{
				ipcb->pkt_type = PKT_TYPE_IPMC;
				ip_output(pkt);
			}
			else
			{
                FTM_PKT_ZLOG_ERR("IPMC pkt send error!\n");                  
				pkt_free(pkt);
			}
			break;
    	}
    	case PKT_TYPE_MPLS:
		{
			mplscb = (struct mpls_control *)&(pkt->cb);
			mplscb->is_changed = 1;
			mpls_forward(pkt);  /* 发送 mpls 报文 */
			break;
    	}	
		case PKT_TYPE_ETH:
		{
			ethcb = (struct eth_control *)&(pkt->cb);
			ethcb->is_changed = 1;
			eth_forward(pkt);   /* 指定出接口和 mac，发送 eth 报文 */
			break;
		}
		case PKT_TYPE_RAW:      /* 指定出接口或入接口发送完整报文 */
		{
		   rawcb = (struct raw_control *)&(pkt->cb);			   
		   pkt->priority = rawcb->priority;
		   if(rawcb->out_ifindex)      /* 指定出接口发送 */
		   {
           	  pkt->out_ifindex = rawcb->out_ifindex;
			  pkt->priority = rawcb->priority;
			  pkt->protocol = rawcb->ethtype;
			  pkt->cb_type = PKT_TYPE_RAW;
		   	  netif_xmit(pkt);
		   }
		   else if(rawcb->in_ifindex)  /* 指定入接口发送 */
		   {
			  pkt->inif_type = PKT_INIF_TYPE_IF;
			  pkt->in_ifindex = rawcb->in_ifindex;
			  pkt->priority = rawcb->priority;
			  pkt->protocol = rawcb->ethtype;
			  netif_rcv(pkt);
		   }
           else
           {
                FTM_PKT_ZLOG_ERR("Raw pkt send error!\n");
                pkt_free(pkt);
           }
		   break;
		}
		default:
            FTM_PKT_ZLOG_ERR("Pkt send error!\n");
			pkt_free(pkt);
			break;
	}
	return 0;
}


/* 发送报文给 app，只发送 payload 和 cb  */
int ftm_pkt_send_to_n(struct pkt_buffer *ppkt, int module_id)
{
	int ret = 0;
	struct ipc_mesg_n *pSndMsg = NULL;
	
	FTM_PKT_ZLOG_DBG("Module_id:0x%x\n", module_id);
	ppkt->data_offset = (char *)ppkt->data - (char *)ppkt; /* 计算 data 的 offset，用于接收时恢复 data */
	
	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + ppkt->buf_size, MODULE_ID_FTM);
	if(pSndMsg != NULL)
	{
		pSndMsg->msghdr.data_len    = ppkt->buf_size;
		pSndMsg->msghdr.module_id   = module_id;
		pSndMsg->msghdr.sender_id   = MODULE_ID_FTM;
		pSndMsg->msghdr.msg_type    = IPC_TYPE_PACKET;
		pSndMsg->msghdr.msg_subtype = 0;
		pSndMsg->msghdr.data_num    = 1;
		pSndMsg->msghdr.opcode      = 0;
			
		if(ppkt)
		{
			memcpy(pSndMsg->msg_data, ppkt, ppkt->buf_size);
		}
		
		/*send info*/
		ret = ipc_send_msg_n1(pSndMsg, sizeof(struct ipc_mesg_n) + ppkt->buf_size);
		if(ret)
		{
			zlog_err("%s[%d],ipc_send_msg_n1 failed\n",__FUNCTION__,__LINE__);
			ipc_msg_free(pSndMsg,MODULE_ID_FTM);
			return ret;
		}
	}
	else
	{
		zlog_err("%s[%d],mem_share_malloc failed\n",__FUNCTION__,__LINE__);
		return -1;
	}

	return NET_SUCCESS;
}

#if 0
/* 发送报文给 app，只发送 payload 和 cb  */
int ftm_pkt_send_to(struct pkt_buffer *ppkt, int module_id)
{
	static struct ipc_msghdr msghdr;
	int id = 0;
	int ret = 0;
	
	FTM_PKT_ZLOG_DBG("Module_id:0x%x\n", module_id);

	if((ppkt->cb_type == PKT_TYPE_TCP)&&((ppkt->cb.ipcb.sport == 23)||(ppkt->cb.ipcb.dport == 23)||(ppkt->cb.ipcb.dport == 22)||(ppkt->cb.ipcb.dport == 830)))
	{
		if(ipc_pktrx_vty_id < 0)
		{
			ipc_pktrx_vty_id = ipc_connect(IPC_PACKET_FTM_VTY);

	        if(ipc_pktrx_vty_id < 0)
	        {
	            FTM_PKT_ZLOG_ERR("Ipc connect failed!\n");
	            return -1;
	        }
		}
		id = ipc_pktrx_vty_id;
	}
	else
	{
		if(ipc_pktrx_id < 0)
		{
			ipc_pktrx_id = ipc_connect(IPC_PACKET_FTM);

	        if(ipc_pktrx_id < 0)
	        {
	            FTM_PKT_ZLOG_ERR("Ipc connect failed!\n");
	            return -1;
	        }
		}
		id = ipc_pktrx_id;

		{
			#if 0
			struct msqid_ds tmpbuf;
			/* Retrieve a current copy of the internal data structure */
			if( msgctl( id, IPC_STAT, &tmpbuf) == 0)
			{
				//printf("%s..................%d,queuenum = %d\n",__FUNCTION__,__LINE__,tmpbuf.msg_qnum);
				if( tmpbuf.msg_qnum > 1000)
				{
					FTM_PKT_ZLOG_ERR("msg queue IPC_PACKET_FTM FULL!\n");
					return -1;
				}
			}	
			else
			{
				FTM_PKT_ZLOG_ERR("Get msg queuenum failed!\n");
				return -1;
			}
			#endif
		}
	}

	ppkt->data_offset = (char *)ppkt->data - (char *)ppkt; /* 计算 data 的 offset，用于接收时恢复 data */
	memset ( &msghdr, 0, sizeof ( struct ipc_msghdr ) );
	msghdr.data_len = ppkt->buf_size;
	msghdr.module_id = module_id;
	msghdr.msg_type = IPC_TYPE_PACKET;
	msghdr.msg_subtype = 0;
	msghdr.data_num = 1;
	msghdr.opcode = 0;
	msghdr.sender_id = MODULE_ID_FTM;

    ret = ipc_send(id, &msghdr, ppkt);
	if(ret < 0)
	{
		usleep(10000);//发送失败让出 CPU 10ms
	    FTM_PKT_ZLOG_ERR("Ipc send failed!\n");
	    return NET_FAILED;
	}
	
	return NET_SUCCESS;
}
#endif 

/* 处理控制 IPC */
int ftm_pkt_msg_n(struct ipc_msghdr_n *phdr,void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, int sender_id)
{
	if(subtype == PKT_SUBTYPE_TCP_PORT)
    {
        tcp_port_alloc_for_app(phdr,pdata, data_len, data_num, sender_id, opcode);
    }
	else if(subtype == PKT_SUBTYPE_UDP_PORT)
    {
        udp_port_alloc_for_app(phdr,pdata, data_len, data_num, sender_id, opcode);
    }
	else if(subtype == PKT_SUBTYPE_TCP_CONNECT)
    {
        tcp_session_open_for_app(phdr,pdata, data_len, data_num, sender_id, opcode);
    }
    else
    {
        FTM_PKT_ZLOG_DBG("Subtye error!\n");
		return -1;
    }

	return 0;
}


/* 报文分类型统计 */
void ftm_pkt_cnt_add(int opcode, int optype)
{
	if(opcode == PKT_ACTION_RECV)
	{
		if(optype == PKT_TYPE_ETH)
		{
			ftm_pkt_cnt.eth_recv_pkts++;
		}
		else if(optype == PKT_TYPE_IP)
		{
			ftm_pkt_cnt.ip_recv_pkts++;
		}		
		else if(optype == PKT_TYPE_IPV6)
		{
			ftm_pkt_cnt.ipv6_recv_pkts++;
		}			
		else if(optype == PKT_TYPE_TCP)
		{
			ftm_pkt_cnt.tcp_recv_pkts++;
		}	
		else if(optype == PKT_TYPE_UDP)
		{
			ftm_pkt_cnt.udp_recv_pkts++;
		}
		else if(optype == PKT_TYPE_MPLS)
		{
			ftm_pkt_cnt.mpls_recv_pkts++;
		}	
		else
		{
			FTM_PKT_ZLOG_ERR("Optype:0x%x\n",optype);
			return;
		}
	}
	else if(opcode == PKT_ACTION_SEND)
	{
		if(optype == PKT_TYPE_ETH)
		{
			ftm_pkt_cnt.eth_send_pkts++;
		}
		else if(optype == PKT_TYPE_IP)
		{
			ftm_pkt_cnt.ip_send_pkts++;
		}		
		else if(optype == PKT_TYPE_IPV6)
		{
			ftm_pkt_cnt.ipv6_send_pkts++;
		}			
		else if(optype == PKT_TYPE_TCP)
		{
			ftm_pkt_cnt.tcp_send_pkts++;
		}	
		else if(optype == PKT_TYPE_UDP)
		{
			ftm_pkt_cnt.udp_send_pkts++;
		}	
		else if(optype == PKT_TYPE_MPLS)
		{
			ftm_pkt_cnt.mpls_send_pkts++;
		}
		else
		{
			FTM_PKT_ZLOG_ERR("Optype:0x%x\n",optype);
			return;
		}
	}
	else if(opcode == PKT_ACTION_DROP)
	{
		if(optype == PKT_TYPE_ETH)
		{
			ftm_pkt_cnt.eth_drop_pkts++;
		}
		else if(optype == PKT_TYPE_IP)
		{
			ftm_pkt_cnt.ip_drop_pkts++;
		}		
		else if(optype == PKT_TYPE_IPV6)
		{
			ftm_pkt_cnt.ipv6_drop_pkts++;
		}			
		else if(optype == PKT_TYPE_TCP)
		{
			ftm_pkt_cnt.udp_drop_pkts++;
		}	
		else if(optype == PKT_TYPE_UDP)
		{
			ftm_pkt_cnt.tcp_drop_pkts++;
		}
		else if(optype == PKT_TYPE_MPLS)
		{
			ftm_pkt_cnt.mpls_drop_pkts++;
		}	
		else
		{
			FTM_PKT_ZLOG_ERR("Optype:0x%x\n",optype);
			return;
		}
	}	
	else
	{
		FTM_PKT_ZLOG_ERR("Opcode:0x%x\n",opcode);
		return;		
	}
}


/* 打印报文内容 */
void ftm_pkt_dump (void *data, int len, int opcode, int optype)
{
    int i, taillen;
	u_char flag = FALSE;
	u_char* pktdata;
	char string[200];
	char tmp[6];
	
    if(data == NULL)
    {
        return ;
    }

	if(len > FTM_PKT_LEN)
	{
		len = FTM_PKT_LEN;
	}

	if(opcode == PKT_ACTION_RECV)
	{
		if((optype == PKT_TYPE_ETH && ftm_pkt_dbg.eth_recv_dbg == 1)
			|| (optype == PKT_TYPE_IP && ftm_pkt_dbg.ip_recv_dbg == 1)
			|| (optype == PKT_TYPE_TCP && ftm_pkt_dbg.tcp_recv_dbg == 1)
			|| (optype == PKT_TYPE_UDP && ftm_pkt_dbg.udp_recv_dbg == 1)
			|| (optype == PKT_TYPE_MPLS && ftm_pkt_dbg.mpls_recv_dbg == 1)
			|| (optype == PKT_TYPE_IPV6 && ftm_pkt_dbg.ipv6_recv_dbg == 1))
		{
			flag = TRUE;
		}
	}
	else if(opcode == PKT_ACTION_SEND)
	{
		if((optype == PKT_TYPE_ETH && ftm_pkt_dbg.eth_send_dbg == 1)
			|| (optype == PKT_TYPE_IP && ftm_pkt_dbg.ip_send_dbg == 1)
			|| (optype == PKT_TYPE_TCP && ftm_pkt_dbg.tcp_send_dbg == 1)
			|| (optype == PKT_TYPE_UDP && ftm_pkt_dbg.udp_send_dbg == 1)
			|| (optype == PKT_TYPE_MPLS && ftm_pkt_dbg.mpls_send_dbg == 1)
			|| (optype == PKT_TYPE_IPV6 && ftm_pkt_dbg.ipv6_send_dbg == 1))
		{
			flag = TRUE;
		}
	}
	else
	{
		FTM_PKT_ZLOG_ERR("opcode:0x%x, optype:0x%x\n",opcode, optype);
		return;		
	}
	
	if(flag == FALSE)
	{
		return;
	}
	if(len > 128)
		len = 128;

    for ( i = 0; i < len/16; i++ )
    {
		pktdata = (unsigned char *)data + i*16;
        zlog_debug(FTM_DBG_PKT,"%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
					pktdata[0], pktdata[1],pktdata[2],pktdata[3],pktdata[4],pktdata[5],pktdata[6],pktdata[7],
					pktdata[8], pktdata[9],pktdata[10],pktdata[11],pktdata[12],pktdata[13],pktdata[14],pktdata[15]);
    }
	
	taillen = len%16;
	if(taillen != 0)
	{
		/* 打印剩余字节 */
		pktdata = (unsigned char *)data + i*16;
		memset(string, 0, sizeof(string));
		memset(tmp, 0, sizeof(tmp));
		for(i=0; i < taillen; i++)
		{
			if(i == 8)
				sprintf(tmp, " %02x ", pktdata[i]);
			else if(i == taillen -1)
				sprintf(tmp, "%02x", pktdata[i]);
			else
				sprintf(tmp, "%02x ", pktdata[i]);
			strcat(string, tmp);
		}
		zlog_debug(FTM_DBG_PKT,"%s", string);
	}
	
    return ;	
}

