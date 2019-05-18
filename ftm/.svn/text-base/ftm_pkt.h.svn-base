/* receive and send packet function */


#ifndef FTM_PKT_H
#define FTM_PKT_H

#include <lib/msg_ipc_n.h>


#define FTM_PKT_LEN	   128  /* debug packet len */

#define FTM_PKT_ZLOG_DBG(format,...)\
    zlog_debug(FTM_DBG_IP,"%s[%d]:In function '%s',"format,__FILE__,__LINE__,__func__,##__VA_ARGS__);

#define FTM_PKT_ZLOG_ERR(format,...)\
    zlog_err("%s[%d]:In function '%s',"format,__FILE__,__LINE__,__func__,##__VA_ARGS__);

enum PKT_ACTION
{
	PKT_ACTION_RECV = 0,
	PKT_ACTION_SEND,
	PKT_ACTION_DROP
};


/* packet 调试开关 */
struct pkt_debug
{
	uint8_t eth_recv_dbg;
	uint8_t eth_send_dbg;
	uint8_t ip_recv_dbg;
	uint8_t ip_send_dbg;	
	uint8_t ipv6_recv_dbg;
	uint8_t ipv6_send_dbg;
	uint8_t udp_recv_dbg;
	uint8_t udp_send_dbg;
	uint8_t tcp_recv_dbg;
	uint8_t tcp_send_dbg;
	uint8_t mpls_recv_dbg;
	uint8_t mpls_send_dbg;
	uint8_t arp_recv_dbg;
	uint8_t arp_send_dbg;
};


/* 报文统计数据结构*/
struct pkt_count
{
	uint64_t eth_recv_pkts;
	uint64_t eth_send_pkts;	
	uint64_t eth_drop_pkts;
	uint64_t ip_recv_pkts;
	uint64_t ip_send_pkts;
	uint64_t ip_drop_pkts;
	uint64_t ipv6_recv_pkts;
	uint64_t ipv6_send_pkts;
	uint64_t ipv6_drop_pkts;
	uint64_t mpls_recv_pkts;
	uint64_t mpls_send_pkts;
	uint64_t mpls_drop_pkts;
	uint64_t udp_recv_pkts;
	uint64_t udp_send_pkts;
	uint64_t udp_drop_pkts;
	uint64_t tcp_recv_pkts;
	uint64_t tcp_send_pkts;	
	uint64_t tcp_drop_pkts;
};


extern struct pkt_count ftm_pkt_cnt;   /* FTM 的收发包统计 */
extern struct pkt_debug ftm_pkt_dbg;   /* FTM 的报文调试开关 */
extern int ftm_pkt_rcv_queue[8];       /*packet IPC queue receive from hsl */


void ftm_pkt_init(void); /* 初始化报文相关的数据 */
int  ftm_pkt_rcv(struct ipc_pkt *pmesg);  /* 从 hal 接收报文 */
int  ftm_pkt_rcv_from_app(struct ipc_pkt *pmesg);        /* 从 app 接收报文 */
int  ftm_pkt_send_n(struct pkt_buffer *pkt); /* 报文发送给 hal */
int  ftm_pkt_send(struct pkt_buffer *pkt); /* 报文发送给 hal */
int  ftm_pkt_send_to_n(struct pkt_buffer *pkt, int module_id); /* 报文发送给 app */
int  ftm_pkt_send_to(struct pkt_buffer *pkt, int module_id); /* 报文发送给 app */
void ftm_pkt_dump (void *data, int len, int opcode, int optype);
void ftm_pkt_cnt_add(int opcode, int optype);
int ftm_pkt_rcv_n(struct ipc_mesg_n *pmesg);
int ftm_pkt_rcv_from_app_n(struct ipc_mesg_n *pmesg);
int ftm_pkt_msg_n(struct ipc_msghdr_n *phdr,void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, int sender_id);

//int ftm_pkt_rcv_msg(struct ipc_mesg_n *pmesg); /* receive control message from common ipc */ 

/* 处理控制消息 */
int ftm_pkt_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, int sender_id);


#endif

