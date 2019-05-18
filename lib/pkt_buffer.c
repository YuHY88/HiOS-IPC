/*
*    packet buffer 管理模块
*
*/


#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/linklist.h>
#include <lib/zassert.h>
#include <lib/msg_ipc.h>
#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/log.h>
#include <string.h>
#include <unistd.h>

#define PKT_BUFFER_NUM1 1000    /* buffer number of size1 */
#define PKT_BUFFER_NUM2 100     /* buffer number of size2 */


struct pkt_list pkt_free_list1; /*packet buffer free list of size1 */
struct pkt_list pkt_free_list2; /*packet buffer free list of size2 */

/* 输出报文到字符串 */ 
char* pkt_dump(void *data, unsigned int len)
{
    unsigned int i, taillen;
	u_char* pktdata;
	static char pktinfo[512];
	char tmp[100];
	
	memset(pktinfo, 0, 512);
    if ( data == NULL || len == 0)
    {
        return NULL;
    }

	if(len > 128)
		len = 128;
	
    for ( i = 0; i < len/16; i++ )
    {
		memset(tmp, 0, 100);
		pktdata = (unsigned char *)data + i*16;
		sprintf(tmp, "%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x\r\n", 
					pktdata[0], pktdata[1],pktdata[2],pktdata[3],pktdata[4],pktdata[5],pktdata[6],pktdata[7],
					pktdata[8], pktdata[9],pktdata[10],pktdata[11],pktdata[12],pktdata[13],pktdata[14],
					pktdata[15]);
		strcat(pktinfo, tmp);
    }
	
	taillen = len%16;
	
	if(taillen == 0)
	{
		return pktinfo;
	}
	
	/* 打印剩余字节 */
	pktdata = (unsigned char *)data + i*16;
	for(i=0; i < taillen; i++)
	{
		memset(tmp, 0, 100);
		if(i == 8)
			sprintf(tmp, " %02x ", pktdata[i]);
		else if(i == taillen -1)
			sprintf(tmp, "%02x", pktdata[i]);
		else
			sprintf(tmp, "%02x ", pktdata[i]);
		strcat(pktinfo, tmp);
	}
	strcat(pktinfo, "\r\n");
    return pktinfo;
}


/* data move len to head */
int pkt_push(struct pkt_buffer *pkt, unsigned int len)
{
     if((pkt->data_len + len + PKT_HDR_SIZE) > pkt->buf_size)
     	return -1;
	 
     pkt->data = (uchar *)pkt->data - len;
     pkt->data_len += len;
     return 0;
}


/* data move len to tail */
int pkt_pull(struct pkt_buffer *pkt, unsigned int len)
{
	if(len > pkt->data_len)
		return -1;

	pkt->data = (uchar *)pkt->data + len;
	pkt->data_len -= len;
	return     0;
}



/* set data to the packet */
int pkt_set(struct pkt_buffer *pkt, void *data, unsigned int len)
{
    if(len > pkt->data_len)
		return -1;

	memcpy(pkt->data, data, len);
	pkt->data_len = len;
	return 	0;
}


/* get a packet buffer from free list */
struct pkt_buffer *pkt_alloc(unsigned int size)
{
	struct pkt_buffer *pkt = NULL;
	int size1 = size + PKT_HDR_SIZE;

	if(size1 > PKT_BUFFER_SIZE2)
		return NULL;
	
    if((size1 > PKT_BUFFER_SIZE1) && (pkt_free_list2.num))
    {
       pkt = pkt_free_list2.head;
	   pkt_free_list2.head = pkt->next;
	   if(pkt == pkt_free_list2.tail)
	   	  pkt_free_list2.tail = NULL;
	   pkt_free_list2.num--;
    }
	else if((size1 <= PKT_BUFFER_SIZE1) && (pkt_free_list1.num))
	{
	   pkt = pkt_free_list1.head;
	   pkt_free_list1.head = pkt->next;
	   if(pkt == pkt_free_list1.tail)
		  pkt_free_list1.tail = NULL;
   	   pkt_free_list1.num--;
	}
	else
	{
		return NULL;
	}

	return pkt;
}


/* put a packet buffer into free list */
void pkt_free(struct pkt_buffer *pkt)     
{
    struct pkt_buffer *pkt_old = NULL;

    if(pkt == NULL)
	{
		zlog_err ("%s, %d pkt is NULL!\n",__FUNCTION__, __LINE__);
		assert(0);
		return;
	}

    if(pkt->buf_size == PKT_BUFFER_SIZE1)
    {
        for (pkt_old = pkt_free_list1.head; pkt_old; pkt_old = pkt_old->next)
        {
            if (pkt == pkt_old)
            {
                return;
            }
        }
    }
    else if (pkt->buf_size == PKT_BUFFER_SIZE2)
    {
        for (pkt_old = pkt_free_list2.head; pkt_old; pkt_old = pkt_old->next)
        {
            if (pkt == pkt_old)
            {
                return;
            }
        }
    }

    pkt_buffer_init(pkt);

    if(pkt->buf_size == PKT_BUFFER_SIZE1)
    {
		pkt->next = pkt_free_list1.head;
		pkt_free_list1.head = pkt;
		if(pkt_free_list1.tail == NULL)
		   pkt_free_list1.tail = pkt;
		pkt_free_list1.num++;
    }
	else if(pkt->buf_size == PKT_BUFFER_SIZE2)
	{
		pkt->next = pkt_free_list2.head;
		pkt_free_list2.head = pkt;
		if(pkt_free_list2.tail == NULL)
		   pkt_free_list2.tail = pkt;
		pkt_free_list2.num++;
	}
    else		
	{
		zlog_err ("%s, %d pkt->buf_size:0x%x!\n",__FUNCTION__, __LINE__,pkt->buf_size);		
		assert(0);
		return;
	}
}
	

/* init the packet buffer */
void pkt_buffer_init(struct pkt_buffer *pkt)
{
    int buf_size = pkt->buf_size;

    if (buf_size > PKT_BUFFER_SIZE1)
    {
        buf_size = PKT_BUFFER_SIZE2;
    }
    else
    {
        buf_size = PKT_BUFFER_SIZE1;
    }

    memset(pkt, 0, buf_size);
	pkt->next = NULL;
	pkt->prev = NULL;
	pkt->data = (char *)pkt + buf_size; /* set the data pointer to the end of buffer */
	pkt->buf_size = buf_size;
}


/* alloc a packet buffer of size */
static struct pkt_buffer *pkt_new(unsigned int size)
{
	struct pkt_buffer *pkt = NULL;
	unsigned int buff_size;

    if(size > PKT_BUFFER_SIZE1)
       buff_size = PKT_BUFFER_SIZE2;
	else
	   buff_size = PKT_BUFFER_SIZE1;
	
	pkt = XMALLOC (MTYPE_PKT_BUFFER, buff_size);
	if(pkt)
	{		
		pkt->buf_size = buff_size;
		pkt_buffer_init(pkt);
	}
	
	return pkt;
}


/* init the free list of packet buffer */
void pkt_list_init()
{
	struct pkt_buffer *pkt;
	int i;

    /* init free list2 */
	pkt_free_list2.head = NULL;	
	pkt_free_list2.tail = NULL;	
	pkt_free_list2.num = 0;	
	pkt_free_list2.lock = 0;	
	pkt_free_list2.pad = 0;	
	for(i=0; i<PKT_BUFFER_NUM2; i++) 
	{
	 	pkt = pkt_new(PKT_BUFFER_SIZE2);
		if(pkt == NULL) 
		{
			assert(0);
			return;
		}
		
		pkt->next = pkt_free_list2.head;
		pkt_free_list2.head = pkt;
		if(pkt_free_list2.tail == NULL)
		   pkt_free_list2.tail = pkt;
		pkt_free_list2.num++;
	}
	
    /* init free list1 */
	pkt_free_list1.head = NULL;	
	pkt_free_list1.tail = NULL;	
	pkt_free_list1.num = 0;	
	pkt_free_list1.lock = 0;	
	pkt_free_list1.pad = 0;	
	for(i=0; i<PKT_BUFFER_NUM1; i++) 
	{
	 	pkt = pkt_new(PKT_BUFFER_SIZE1);
		if(pkt == NULL) 
		{
			assert(0);
			return;
		}
		
		pkt->next = pkt_free_list1.head;
		pkt_free_list1.head = pkt;
		if(pkt_free_list1.tail == NULL)
		   pkt_free_list1.tail = pkt;
		pkt_free_list1.num++;
	}
}


/* app call this function to send packet to ftm */
int pkt_send(enum PKT_TYPE type, union pkt_control *pcb, void *payload, int data_len)
{
	int ret = 0;
	struct ipc_mesg_n *pSndMsg = NULL;

	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + sizeof(union pkt_control) + data_len, MODULE_ID_FTM);
	if(pSndMsg != NULL)
	{
		pSndMsg->msghdr.data_len	= data_len + sizeof(union pkt_control);
		pSndMsg->msghdr.module_id	= MODULE_ID_FTM;
		pSndMsg->msghdr.sender_id	= 0;
		pSndMsg->msghdr.msg_type	= IPC_TYPE_PACKET;
		pSndMsg->msghdr.msg_subtype = type;
		pSndMsg->msghdr.data_num	= 1;
		pSndMsg->msghdr.opcode		= 0;
			
		memcpy(pSndMsg->msg_data, pcb, sizeof(union pkt_control));
		memcpy(pSndMsg->msg_data+sizeof(union pkt_control), payload, data_len);
		
		/*send info*/
		ret = ipc_send_msg_n1(pSndMsg, sizeof(struct ipc_mesg_n) + data_len + sizeof(union pkt_control));
		if(ret)
		{
			zlog_err("%s[%d],ipc_send_msg_n1 failed\n",__FUNCTION__,__LINE__);
			ipc_msg_free(pSndMsg,MODULE_ID_FTM);
			goto drop;
		}
	}

	return NET_SUCCESS;
	
drop:
	zlog_err("%s[%d], drop!\n",__FUNCTION__,__LINE__);
	return NET_FAILED;
}

#if 0
/* app call this function to send packet to ftm */
int pkt_send(enum PKT_TYPE type, union pkt_control *pcb, void *payload, int data_len)
{
	struct ipc_msghdr msghdr;
	static uchar buf[PKT_BUFFER_SIZE2];
	int id = 0;
	int ret = -1;

	if((type == PKT_TYPE_TCP)&&((pcb->ipcb.dport == 23)||(pcb->ipcb.dport == 22)||(pcb->ipcb.dport == 830)))
	{
		if(ipc_pkttx_vty_id < 0)
			ipc_pkttx_vty_id = ipc_connect(IPC_PACKET_SEND_VTY);

	    if(ipc_pkttx_vty_id < 0)
	    {
	        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
	        return NET_FAILED;
	    }

		id = ipc_pkttx_vty_id;
	}
	else
	{
		if(ipc_pkttx_id < 0)
			ipc_pkttx_id = ipc_connect(IPC_PACKET_SEND);

	    if(ipc_pkttx_id < 0)
	    {
	        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
	        return NET_FAILED;
	    }
		
		id = ipc_pkttx_id;
	}

    
	msghdr.module_id = MODULE_ID_FTM;	
	msghdr.sender_id = 0;
	msghdr.msg_type = IPC_TYPE_PACKET;
	msghdr.msg_subtype = type;
    msghdr.data_len = data_len + sizeof(union pkt_control);

    memcpy(buf, pcb, sizeof(union pkt_control));	
    memcpy(buf+sizeof(union pkt_control), payload, data_len);
    ret = ipc_send(id, &msghdr, buf);
	if(ret < 0)
	{
		usleep(10000);//发送失败让出 CPU 10ms
		goto drop;
	}
	
	return NET_SUCCESS;
	
drop:
	zlog_debug(ZLOG_LIB_DBG_PKG, "pkt_send fail.\n");
	return NET_FAILED;
}
#endif


/* app call this function to receive packet from ftm, 
*  报文的控制信息在 pkt->cb 中, 
*  报文的 payload 从 pkt->data 地址开始, payload 的长度是 pkt->datalen */
#if 1
struct pkt_buffer *pkt_rcv_n(struct ipc_mesg_n *pmesg)
{
	struct ipc_msghdr_n *phdr = NULL;
	struct pkt_buffer *pkt = NULL;

	phdr = &(pmesg->msghdr);
	if(phdr->msg_type != IPC_TYPE_PACKET)
		return NULL;
	
	pkt = (struct pkt_buffer *)(pmesg->msg_data);
	pkt->next = NULL;
	pkt->prev = NULL;
	pkt->transport_header = NULL;
	pkt->network_header = NULL;
	pkt->link_header = NULL;
	pkt->data = (char *)pkt + pkt->data_offset;
	
	return pkt;
}
#endif
/*
struct pkt_buffer *pkt_rcv(int module_id)
{
	static struct ipc_pkt mesg;	
	struct ipc_msghdr *phdr = NULL;
	struct pkt_buffer *pkt = NULL;

	if(ipc_pktrx_id < 0)
		ipc_pktrx_id = ipc_connect(IPC_PACKET_FTM);

    if(ipc_pktrx_id < 0)
    {
        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
        return NULL;
    }
    
	if(-1 == ipc_recv_pkt(ipc_pktrx_id, &mesg, module_id))
	{
		return NULL;
	}

	phdr = &(mesg.msghdr);
	if(phdr->msg_type != IPC_TYPE_PACKET)
		return NULL;
	
	pkt = (struct pkt_buffer *)(mesg.msg_data);
	pkt->next = NULL;
	pkt->prev = NULL;
	pkt->transport_header = NULL;
	pkt->network_header = NULL;
	pkt->link_header = NULL;
	pkt->data = (char *)pkt + pkt->data_offset;
	
	return pkt;
}*/

#if 0
struct ipc_pkt *pkt_rcv_vty(int module_id)
{
	static struct ipc_pkt mesg;	
	//struct ipc_msghdr *phdr = NULL;
	//struct pkt_buffer *pkt = NULL;

	if(ipc_pktrx_vty_id < 0)
		ipc_pktrx_vty_id = ipc_connect(IPC_PACKET_FTM_VTY);

    if(ipc_pktrx_vty_id < 0)
    {
        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
        return NULL;
    }
    
	if(-1 == ipc_recv_pkt(ipc_pktrx_vty_id, &mesg, module_id))
	{
		return NULL;
	}

	return &mesg;
}

struct ipc_pkt *pkt_rcv_vty(int module_id)
{
	static struct ipc_pkt mesg;	
	struct ipc_msghdr *phdr = NULL;
	struct pkt_buffer *pkt = NULL;

	if(ipc_pktrx_vty_id < 0)
		ipc_pktrx_vty_id = ipc_connect(IPC_PACKET_FTM_VTY);

    if(ipc_pktrx_vty_id < 0)
    {
        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
        return NULL;
    }
    
	if(-1 == ipc_recv_pkt(ipc_pktrx_vty_id, &mesg, module_id))
	{
		return NULL;
	}

	phdr = &(mesg.msghdr);
	if(phdr->msg_type != IPC_TYPE_PACKET)
		return NULL;
	
	pkt = (struct pkt_buffer *)(mesg.msg_data);
	pkt->next = NULL;
	pkt->prev = NULL;
	pkt->transport_header = NULL;
	pkt->network_header = NULL;
	pkt->link_header = NULL;
	pkt->data = (char *)pkt + pkt->data_offset;
	
	return pkt;
}
#endif

/* app call this function to register packet type that will be received */
int pkt_register(int module_id, enum PROTO_TYPE type, union proto_reg *proto)
{
	int ret = -1;
    
	ret = ipc_send_msg_n2(proto, sizeof(union proto_reg),  1, MODULE_ID_FTM, module_id, IPC_TYPE_PROTO, type, IPC_OPCODE_REGISTER, 0);
	if(ret == -1)
	{
        zlog_err("%s[%d]: ipc send failed!\n", __FILE__, __LINE__);	
		return ret;
	}

	return ret;	
}


/* app call this function to cancel registerd packet type */
void pkt_unregister(int module_id, enum PROTO_TYPE type, union proto_reg *proto)
{
	int ret = -1;
    
	ret = ipc_send_msg_n2(proto, sizeof(union proto_reg),  1, MODULE_ID_FTM, module_id, IPC_TYPE_PROTO, type, IPC_OPCODE_UNREGISTER, 0);
	
	if(ret == -1)
	{
        zlog_err("%s[%d]: ipc send failed!\n", __FILE__, __LINE__);	
		return;
	}

	return;
}

#if 0
int pkt_register(int module_id, enum PROTO_TYPE type, union proto_reg *proto)
{
	struct ipc_msghdr msghdr;
	int ret = -1;
    int id = 0;
	
    /* ipc for receive packet from ftm */
	id = ipc_connect(IPC_MSG_FTM);
	if(id < 0)
	{
        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);	
		return 0;
	}

    memset(&msghdr, 0, sizeof(struct ipc_msghdr));
	msghdr.module_id = MODULE_ID_FTM;
	msghdr.sender_id = module_id;
	msghdr.msg_type = IPC_TYPE_PROTO;
	msghdr.msg_subtype = type;
	msghdr.opcode = IPC_OPCODE_REGISTER;
	msghdr.data_len = sizeof(union proto_reg);

	ret = ipc_send(id, &msghdr, proto);
	if(ret == -1)
	{
        zlog_err("%s[%d]: ipc send failed!\n", __FILE__, __LINE__);	
		return -1;
	}

	return id;	
}


/* app call this function to cancel registerd packet type */
void pkt_unregister(int module_id, enum PROTO_TYPE type, union proto_reg *proto)
{
	struct ipc_msghdr_n msghdr;
	int ret = -1;
    int id;
	
    /* ipc for receive packet from ftm */
	id = ipc_connect(IPC_MSG_FTM);
	if(id < 0)
	{
        zlog_err("%s[%d]: packet unregister failed!\n", __FILE__, __LINE__);	
		return;
	}
	
    memset(&msghdr, 0, sizeof(struct ipc_msghdr_n));
	msghdr.module_id = MODULE_ID_FTM;
	msghdr.sender_id = module_id;
	msghdr.msg_type = IPC_TYPE_PROTO;
	msghdr.msg_subtype = type;
	msghdr.opcode = IPC_OPCODE_UNREGISTER;
	msghdr.data_len = sizeof(union proto_reg);

	ret = ipc_send(id, &msghdr, proto);
	if(ret == -1)
	{
        zlog_err("%s[%d]: ipc send failed!\n", __FILE__, __LINE__);	
		return;
	}

	return;
}
#endif


int ipc_send_common_wait_ack_n(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
		enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	int ret = 0;
	int rcvlen = 0;
	struct ipc_mesg_n *pSndMsg = NULL;
	struct ipc_mesg_n * pRcvMsg = NULL;
	
	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, sender_id);
	if(pSndMsg != NULL)
	{
		pSndMsg->msghdr.data_len    = data_len;
		pSndMsg->msghdr.module_id   = module_id;
		pSndMsg->msghdr.sender_id   = sender_id;
		pSndMsg->msghdr.msg_type    = msg_type;
		pSndMsg->msghdr.msg_subtype = msg_subtype;
		pSndMsg->msghdr.msg_index   = msg_index;
		pSndMsg->msghdr.data_num    = data_num;
		pSndMsg->msghdr.opcode      = opcode;
			
		if(pdata)
		{
			memcpy(pSndMsg->msg_data, pdata, data_len);
		}
		
		/*send info*/
		ret = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n) + data_len, &pRcvMsg, &rcvlen, 5000);
			
		switch(ret)
		{
			/*send fail*/
			case -1:
				{
					ipc_msg_free(pSndMsg,sender_id);
					return -1;
				}
			case -2:
				{
					/*recv fail*/
					return -1;
								
				}
			case 0:
				{	/*recv success*/
					if(pRcvMsg != NULL)
					{	
						/*if noack return errcode*/
						if(IPC_OPCODE_NACK == pRcvMsg->msghdr.opcode || IPC_OPCODE_REPLY == pRcvMsg->msghdr.opcode)
						{
							memcpy(&ret,pRcvMsg->msg_data,pRcvMsg->msghdr.data_len);
						}
						/*if ack return 0*/
						else if(IPC_OPCODE_ACK == pRcvMsg->msghdr.opcode)
						{
							ret = 0;
						}
						else
						{
							ret = -1;
						}
						//ret = (int)pRcvMsg->msg_data;
						ipc_msg_free(pRcvMsg,sender_id);
					}
					else
					{
						return -1;
					}
				}
				break;
			default:
				return -1;
						
		}
		
	}
	else
	{
		return -1;
	}
	return(ret);
}      

/* 建立一条 tcp 连接 */
int pkt_open_tcp(int module_id, struct ip_proto *ptcp)
{
	return ipc_send_common_wait_ack_n(ptcp, sizeof(struct ip_proto), 1 , MODULE_ID_FTM, module_id,
                                        IPC_TYPE_PACKET, PKT_SUBTYPE_TCP_CONNECT, IPC_OPCODE_ADD, 0);
}

/* 关闭 tcp 连接 */
int pkt_close_tcp(int module_id, struct ip_proto *ptcp)
{
    return ipc_send_common_wait_ack_n(ptcp, sizeof(struct ip_proto), 1 , MODULE_ID_FTM, module_id,
                                        IPC_TYPE_PACKET, PKT_SUBTYPE_TCP_CONNECT, IPC_OPCODE_DELETE, 0);
}

/* 获取TCP或UDP的随机PORT */
int pkt_port_alloc(int module_id, enum PKT_TYPE type)
{
	uint16_t pport;
	int subtype = 0;

	if(type == PKT_TYPE_TCP)
		subtype = PKT_SUBTYPE_TCP_PORT;
	else if(type == PKT_TYPE_UDP)
		subtype = PKT_SUBTYPE_UDP_PORT;
	
	pport = ipc_send_common_wait_ack_n(NULL, 0, 1 , MODULE_ID_FTM, module_id,
										IPC_TYPE_PACKET, subtype, IPC_OPCODE_GET, 0);
	
	if(pport == 0 || pport == -1)
	{
		return 0;
	}
	else
	{
		return pport;		
	}
}


/* 释放 UDP/TCP 的随机PORT */
int pkt_port_relese(int module_id, enum PKT_TYPE type, uint16_t port)
{
	int subtype = 0;

	if(type == PKT_TYPE_TCP)
		subtype = PKT_SUBTYPE_TCP_PORT;
	else if(type == PKT_TYPE_UDP)
		subtype = PKT_SUBTYPE_UDP_PORT;

	return ipc_send_msg_n2(&port, 2, 1 , MODULE_ID_FTM, module_id, IPC_TYPE_PACKET, subtype, IPC_OPCODE_DELETE,0);
}


#if 0
/* 建立一条 tcp 连接 */
int pkt_open_tcp(int module_id, struct ip_proto *ptcp)
{
    return ipc_send_common_wait_ack(ptcp, sizeof(struct ip_proto), 1 , MODULE_ID_FTM, module_id,
                                        IPC_TYPE_PACKET, PKT_SUBTYPE_TCP_CONNECT, IPC_OPCODE_ADD, 0);
}


/* 关闭 tcp 连接 */
int pkt_close_tcp(int module_id, struct ip_proto *ptcp)
{
    return ipc_send_common_wait_ack(ptcp, sizeof(struct ip_proto), 1 , MODULE_ID_FTM, module_id,
                                        IPC_TYPE_PACKET, PKT_SUBTYPE_TCP_CONNECT, IPC_OPCODE_DELETE, 0);
}


/* 获取TCP或UDP的随机PORT */
int pkt_port_alloc(int module_id, enum PKT_TYPE type)
{
	uint16_t *pport;
	int subtype = 0;

	if(type == PKT_TYPE_TCP)
		subtype = PKT_SUBTYPE_TCP_PORT;
	else if(type == PKT_TYPE_UDP)
		subtype = PKT_SUBTYPE_UDP_PORT;
	
	pport = ipc_send_common_wait_reply(NULL, 0, 1 , MODULE_ID_FTM, module_id,
										IPC_TYPE_PACKET, subtype, IPC_OPCODE_GET, 0);
	if(pport == NULL)
	{
		return 0;
	}
	else
	{
		return *pport;		
	}
}


/* 释放 UDP/TCP 的随机PORT */
int pkt_port_relese(int module_id, enum PKT_TYPE type, uint16_t port)
{
	int subtype = 0;

	if(type == PKT_TYPE_TCP)
		subtype = PKT_SUBTYPE_TCP_PORT;
	else if(type == PKT_TYPE_UDP)
		subtype = PKT_SUBTYPE_UDP_PORT;

	return ipc_send_common(&port, 2, 1 , MODULE_ID_FTM, module_id, IPC_TYPE_PACKET, subtype, IPC_OPCODE_DELETE);
}
#endif
