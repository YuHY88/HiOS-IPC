/*
*    packet buffer 管理模块
*
*/

#ifndef HIOS_PKT_BUFFER_H
#define HIOS_PKT_BUFFER_H

#include <lib/types.h>
#include <lib/inet_ip.h>

#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>

#define NET_SUCCESS 0
#define NET_FAILED  1

#define PKT_HDR_SIZE     168   /* sizeof(struct pkt_buffer) */

#define PKT_BUFFER_SIZE1 1920  /* buffer size = 1920 bytes, for normal packet */
#define PKT_BUFFER_SIZE2 9900  /* buffer size = 9900 bytes, for 9600 bytes jumbo frame */

#define PKT_CB_SIZE      84    /* control buffer size of packet */
#define MIN_PKT_SIZE      64    /* min size of packet */

#if 0
/* packet ipc subtype */
enum PKT_SUBTYPE
{
	PKT_SUBTYPE_INVALID = 0,
	PKT_SUBTYPE_TCP_PORT,
	PKT_SUBTYPE_UDP_PORT,
	PKT_SUBTYPE_TCP_CONNECT
};
#endif


/* 报文类型 */
enum PKT_TYPE
{
	PKT_TYPE_INVALID = 0,
	PKT_TYPE_ETH,      /* ethernet packet */
	PKT_TYPE_IP,	   /* ipv4 unicast packet */	
	PKT_TYPE_IPBC,	   /* ipv4 broadcast packet */
	PKT_TYPE_IPMC,	   /* ipv4 muticast packet */	
	PKT_TYPE_IPV6,	   /* ipv6 unicast packet */
	PKT_TYPE_IPV6MC,   /* ipv6 muticast packet */
	PKT_TYPE_UDP,	   /* udp packet */
	PKT_TYPE_TCP,      /* tcp packet */
	PKT_TYPE_MPLS,	   /* mpls packet */
	PKT_TYPE_RAW,      /* raw packet */


	PKT_SUBTYPE_TCP_PORT,
	PKT_SUBTYPE_UDP_PORT,
	PKT_SUBTYPE_TCP_CONNECT,
	
	PKT_TYPE_MAX = 255
};


/* 报文入或出接口类型 */
enum PKT_INIF_TYPE
{
   PKT_INIF_TYPE_IF = 0, /* real interface */
   PKT_INIF_TYPE_PW,     /* pw */
   PKT_INIF_TYPE_LSP,    /* lsp */   
   PKT_INIF_TYPE_TUNNEL  /* tunnel */
};


/* 报文链表结构 */
struct pkt_list
{
	struct pkt_buffer	*head;	/* head of buffer list */
	struct pkt_buffer	*tail;	/* tail of buffer list */
    uint16_t num;               /* buffer number in list */
	uint8_t  lock;              /* lock of the list */	
	uint8_t  pad;               /* pad for 4 bytes align */
};


/* cb of ip layer: 36 + 48 bytes */
struct ip_control
{
	uint32_t sip;                /* 报文的源 IP，如果为 0 则使用接口 IP */
	uint32_t dip;                /* 报文的目的 IP，如果是组播 IP 必须指定出接口 */
	struct inet_addr nexthop;    /* 单播查 fib 的下一跳, 单跳可以用 dip */	
	struct ipv6_addr sipv6;	
	struct ipv6_addr dipv6;	
	uint32_t ifindex;            /* 发送报文的出接口索引 */	
	enum PKT_INIF_TYPE  if_type; /* 出接口类型，支持: pw、lsp 为出接口 */
	uint16_t sport;              /* source port of udp or tcp */
	uint16_t dport;              /* destination port of udp or tcp */
	uint16_t vpn;                /* l3vpn id */
	uint16_t frag_off;           /* 报文分片标志 */
	uint8_t  protocol;           /* ip level protocol */
	uint8_t  tos;                /* ip 头的优先级*/
	uint8_t  ttl;                /* 报文的 ttl */
	uint8_t  chsum_enable;       /* 0: udp 不计算 checksum，1: 计算 checksum */
	uint8_t  pkt_type;           /* 参见 enum PKT_TYPE */
	uint8_t  is_changed;         /* 如果置为 0，软转发不会重新封装 ip 头，为 1 才会封装新的 ip 头*/	
	uint8_t  option_len;    
	uint8_t  pad[1];
};


/* cb of mpls layer: 32 bytes */
struct mpls_control
{
	uint8_t  exp;
	uint8_t  ttl;
	uint8_t  label_num; 
	uint8_t  is_changed;          /* 如果为 0，软转发不会重新封装 mpls 头，为 1 才会封装新的 mpls 头*/	
	enum PKT_INIF_TYPE  if_type;  /* 出接口类型，支持: pw、lsp 为出接口 */
	uint32_t ifindex;             /* 发送报文的出接口索引 */
	uint32_t nexthop;             /* 公网 lsp 的下一跳 */	
	uint32_t label;               /* 内层标签*/
	uint16_t chtype;              /* 控制字中的 chtype 字段 */
	uint8_t  oam_opcode;          /* 控制字后的第二个字节, 用于 cfm opcode */
	uint8_t  protocol;            /* IP payload 的协议号 */
	uint16_t dport;               /* udp 或 tcp 的目的端口 */
	uint16_t sport;               /* udp 或 tcp 的源端口 */
	uint32_t dip;	              /* IP payload 的目的 dip */	
	uint32_t sip;	              /* IP payload 的目的 sip */
};


/* cb of ethernet layer: 36 bytes */
struct eth_control
{
	uint8_t  smac[6];    /* 报文的源 mac */
	uint8_t  dmac[6];    /* 报文的目的 mac */
	uint16_t cvlan;      /*cvlan+cos+cfi, only for qinq */
	uint16_t svlan;      /*svlan+cos+cfi for dot1q */
	uint16_t cvlan_tpid;
	uint16_t svlan_tpid;
	uint16_t ethtype;	
	uint16_t llc;        /* ethtype 后的二个字节, 用于 802.3 协议的 llc 字段 */
	uint8_t  sub_ethtype;/* ethtype 后的第一个字节, 也用于 cfm level */
	uint8_t  oam_opcode; /* ethtype 后的第二个字节, 用于 cfm opcode */
	uint8_t  cos;        /* vlan 的优先级*/
	uint8_t  smac_valid; /* 如果置为 0，则软转发会获取接口的 mac，为 1 则使用本结构里的 mac */	
	uint32_t ifindex;    /* 发送报文的出接口索引 */	
	uint8_t  untag;		 /*untag  标记收到原始报文tag信息*/
	uint8_t  is_changed; /* 如果为 0，软转发不会重新封装 eth 头，为 1 才会封装新的 eth 头*/
	uint8_t  pad[2];
};


/* 指定入接口/出接口发送报文 */
struct raw_control
{
	uint32_t in_ifindex;  /* 发送报文的入接口索引，走业务提取流程 */	
	uint32_t out_ifindex; /* 发送报文的出接口索引，支持任意接口 */
	uint16_t ethtype;     /* 报文的 ethtype */
	uint8_t  priority;    /* 报文的优先级 */	
	uint8_t  ttl;         /* 报文的 ttl */
};


/* 4 bytes */
struct bc_control
{
	uint16_t vsi;
	uint8_t  vsi_type; /*0: vsi, 1: vlan*/
	uint8_t  reserv; 
};


/* 报文在每一层的控制信息 */
union pkt_control
{
	//struct ipv6_control ipv6cb;  /* ipv6 报文的控制信息 */
	struct ip_control   ipcb;    /* ip 报文的控制信息 */
	struct eth_control  ethcb;  /* eth 报文的控制信息 */
	struct mpls_control mplscb;/* mpls 报文的控制信息 */
	struct raw_control  rawcb;  /* 原始报文的控制信息 */
	uchar origin_cb[PKT_CB_SIZE];
};


/* 报文的接收或发送时间戳 */
struct time_stamp
{
	unsigned int sec;	/* 秒 */
	unsigned int usec;  /* 微秒 */
};


/* ftm 软转发报文格式 */
struct pkt_buffer
{
	struct pkt_buffer	*next;  	/* Next buffer in list		*/
	struct pkt_buffer	*prev;	    /* Previous buffer in list	*/
	struct time_stamp	 time; 		/* 报文的接收或发送时间戳 */

	enum PKT_INIF_TYPE  inif_type;  /* type of input interface */
	uint32_t	        in_ifindex; /* interface that packet entered */
	uint32_t	        out_ifindex;/* interface that packet will go */	
	uint32_t	        in_port;    /* physical port that packet entered */
	uint16_t            vpn;        /* vpn id of input interface */
	uint16_t	        buf_size;	/* packet buffer size	    */
	uint16_t	        data_len;	/* length of actual data	*/	
	uint16_t	        data_offset;/* data 在 pkt_buffer 中的偏移位置，用于恢复 data 部分*/
	uint16_t	        protocol;	/* eth type */
	uint16_t 			svlan;      /*cvlan+cos+cfi, only for qinq */
	uint16_t 			cvlan;      /*svlan+cos+cfi for dot1q */
	uint8_t  			smac[6];    /* 报文的源 mac */
	uint8_t  			dmac[6];    /* 报文的目的 mac */
	uint8_t	            priority;	/* Packet queueing priority 	*/
	uint8_t             retry_cnt;  /* 报文重传次数 */
	enum PKT_TYPE       cb_type;    /* type of packet control buffer */
	union pkt_control   cb;         /* control buffer that is free to use for every layer. */	
	void      *transport_header;    /* udp/tcp layer header */
	void      *network_header;      /* ip layer header */
    void      *mpls_header;         /* mpls layer header */
	void	  *link_header;         /* Link layer header */
    void      *data;                /* pointer to the packet data */
};

extern char* pkt_dump(void *data, unsigned int len);

extern void pkt_buffer_init(struct pkt_buffer *pkt); /* init the packet buffer */
extern struct pkt_buffer *pkt_alloc(unsigned int size);/* get a packet buffer from free list */
extern void pkt_free(struct pkt_buffer *pkt);           /* put a packet buffer into free list */

extern int pkt_push(struct pkt_buffer *pkt, unsigned int len);/* 向报文头偏移 len 字节*/
extern int pkt_pull(struct pkt_buffer *pkt, unsigned int len);/* 向报文尾偏移 len 字节 */
extern int pkt_set(struct pkt_buffer *pkt, void *data, unsigned int len);/* 在 data 位置填充 len 字节数据 */


/* app call this function to send packet to ftm */
extern int pkt_send(enum PKT_TYPE type, union pkt_control *pcb, void *payload, int data_len);

/* app call this function to receive packet from ftm */
extern struct pkt_buffer *pkt_rcv(int module_id);
extern struct pkt_buffer *pkt_rcv_n(struct ipc_mesg_n *pmesg);

extern struct ipc_pkt *pkt_rcv_vty(int module_id);


extern int pkt_port_alloc(int module_id, enum PKT_TYPE type);
extern int pkt_port_relese(int module_id, enum PKT_TYPE type, uint16_t port);
void pkt_list_init( void );

extern int ipc_send_common_wait_ack_n(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
		enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);


#define APP_RECV_PKT_FROM_FTM_DATA_SET(pkt) pkt->next = NULL;\
	pkt->prev = NULL;\
	pkt->transport_header = NULL;\
	pkt->network_header = NULL;\
	pkt->link_header = NULL;\
	pkt->data = (char *)pkt + pkt->data_offset;

#endif

