/*
*          tcp packet define and forward function
*/

#ifndef HIOS_PKT_TCP_H
#define HIOS_PKT_TCP_H

#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/msg_ipc.h>

#define TCP_HEADER_SIZE         20  /* tcp 头长度 */
#define TCP_OPTION_SIZE         40  /* tcp 选项长度，最大 40 字节 */

#define TCP_PKT_RETRANMSMIT_MAX     5

/* tcp 端口定义 */
#define TCP_PORT_ECHO           7
#define TCP_PORT_DISCARD        9
#define TCP_PORT_NETSTAT        15
#define TCP_PORT_FTP_DATA       20
#define TCP_PORT_FTP_CTRL       21
#define TCP_PORT_SSH            22     /* ssh */
#define TCP_PORT_TELNET         23     /* telnet */
#define TCP_PORT_SMTP           25
#define TCP_PORT_RLP            39
#define TCP_PORT_WINS           42
#define TCP_PORT_WHOIS          43
#define TCP_PORT_TACAS          49     /* tacas */
#define TCP_PORT_DNS            53     /* dns */
#define TCP_PORT_MTP            57
#define TCP_PORT_FINGER         79
#define TCP_PORT_HTTP           80     /* http */
#define TCP_PORT_SNMP           161    /* snmp */
#define TCP_PORT_SNMP_TRAP      162    /* snmp trap */
#define TCP_PORT_BGP            179    /* bgp */
#define TCP_PORT_HTTPS          443    /* https */
#define TCP_PORT_DHCPV6_CLIENT  546    /* dhcp client */
#define TCP_PORT_DHCPV6_SERVER  547    /* dhcp server */
#define TCP_PORT_RTSP           554
//#define TCP_PORT_SMTP           587
#define TCP_PORT_LDP            646    /* ldp */
#define TCP_PORT_PPTP           1723
#define TCP_PORT_RADIUS_AUTH    1812   /* radius authenticate */
#define TCP_PORT_RADIUS_ACCT    1813   /* radius accounting */
#define TCP_PORT_HSRP           1985
#define TCP_PORT_MGCP           2427
#define TCP_PORT_RTP            5004
#define TCP_PORT_OPENFLOW       6633   /* openflow */

#define TCP_PORT_RESERVE        49152  /* 保留 TCP 端口的起始值 */
#define TCP_PORT_MAX            65535  /* TCP 端口的最大值 */


/* tcp flag bit define */
#define TCP_FLAG_FIN		    0x01   /* tcp fin */
#define TCP_FLAG_SYN		    0x02   /* tcp syn */
#define TCP_FLAG_RST		    0x04   /* tcp reset */
#define TCP_FLAG_PSH		    0x08   /* tcp push */
#define TCP_FLAG_ACK		    0x10   /* tcp ack */
#define TCP_FLAG_URG		    0x20   /* tcp urgent, 标识 urg_ptr 有效 */
#define TCP_FLAG_ECE		    0x40   /* tcp ecn echo */
#define TCP_FLAG_CWR		    0x80   /* tcp cwr */

/* tcp urg_ptr value */
#define TCP_URG_VALID	        0x0100
#define TCP_URG_NOTYET	        0x0200
#define TCP_URG_READ	        0x0400


#define TCP_WINDOWN_BIT         16     /* TCP 滑动窗口的最大位数 */
#define TCP_WINDOWN_BIT_SHIFT   14     /* TCP 窗口扩大的最大位数 */

#define TCP_WINDOWN_MAX         65535  /* TCP 窗口的最大值，以字节为单位 */
#define TCP_WINDOWN_DEFAULT     9600   /* TCP 窗口的默认值 */
#define TCP_MSS_MIN             536    /* TCP payload 的默认最小值 */
#define TCP_MSS_DEFAULT         1460   /* TCP payload 的默认值 */


/* TCP option length */
#define TCP_OPTLEN_MSS          4  /* max payload length */
#define TCP_OPTLEN_WIN_SCALE    3  /* window scale len */
#define TCP_OPTLEN_SACK         2  /* SACK Permitted/block len */
#define TCP_OPTLEN_TIMESTAMP    10 /* timestamp option length */


/* TCP option type */
enum TCP_OPT
{
	TCP_OPT_EOL = 0,           /* 选项结束 */
	TCP_OPT_NOP = 1,	       /* Padding, 用于 4 字节对齐 */
	TCP_OPT_MSS = 2,           /* mss */
	TCP_OPT_WIN_SCALE = 3,     /* win scale */
	TCP_OPT_SACK_PERMIT = 4,   /* SACK Permitted */
	TCP_OPT_SACK_BLOCK = 5,    /* SACK Block */
	TCP_OPT_TIMESTAMP = 8,     /* 时间戳 */
	TCP_OPT_MD5 = 19,
	TCP_OPT_MAX = 20,	
};

/* tcp payload 最大长度选项, 4 字节 */
struct tcp_opt_mss
{
	uint8_t  kind;          /* 选项类型, 参见 enum TCP_OPT 定义 */
	uint8_t  length;        /* 选项长度 = 4, TCP_OPTLEN_MSS */
	uint16_t mss;	        /* 报文 payload 最大长度 */
};
#define TCP_MD5_OPTION_SET 1
struct tcp_opt_md5
{
	uint8_t  kind;         
	uint8_t  length;       
	char digest[16];	      
};

/* tcp 窗口扩大因子,3 字节 */
struct tcp_opt_winscale
{
	uint8_t kind;           /* 选项类型, 参见 enum TCP_OPT 定义 */
	uint8_t length;         /* 选项长度 = 3,TCP_OPTLEN_WIN_SCALE  */
	uint8_t winscale;	    /* 窗口扩大因子, 最大 14, 16 + winscale 是扩大后的值 */
}/*__attribute__ ((aligned (4)))*/;


/* tcp 时间戳选项 ,10 字节 */
struct tcp_opt_timestamp
{
	uint8_t  kind;          /* 选项类型, 参见 enum TCP_OPT 定义 */
	uint8_t  length;        /* 选项长度 = 10, TCP_OPTLEN_TIMESTAMP */
	uint32_t timestamp;     /* 发送报文的时间戳 */
	uint32_t timestampreply;/* 时间戳 echo reply, 复制接收报文的时间戳 */
}/*__attribute__ ((aligned (8)))*/;


/* tcp seletcive ack 选项,2 字节 */
struct tcp_sack
{
	uint8_t kind;           /* 选项类型, 参见 enum TCP_OPT 定义 */
	uint8_t length;         /* 选项长度 = 2, TCP_OPTLEN_SACK_PERM */
}/*__attribute__ ((aligned (8)))*/;


/* tcp 报文头: 20 字节 */
struct tcphdr 
{
	uint16_t	sport;      /* TCP 源端口 */
	uint16_t	dport;      /* TCP 目的端口 */
	uint32_t	seq;        /* 发送报文的序列号 */
	uint32_t	ack_seq;    /* 接收报文的确认序列号, flag bit5 置位才有效 */
	
#if BIG_ENDIAN_ON          
	uint8_t     hlen:4,     /* tcp 头长度, 以 4 字节为单位 */
			    reserv:4;
#else                      
	uint8_t     reserv:4,    
			    hlen:4;   	 /* tcp 头长度, 以 4 字节为单位 */
#endif

	uint8_t	    flag;       /* bit1: fin, bit2: syn, bit3: reset, bit4: push, bit5: ack, bit6: urgent, bit7: ece, bit8: cwr */
	uint16_t	window;     /* 接收窗口 */
	uint16_t	checksum; 
	uint16_t	urg_ptr;    /* 紧急指针,flag bit6 置位才有效 */
};


/* TCP 报文类型 */
enum TCP_TYPE
{
	TCP_TYPE_DATA = 0,     /* tcp data */
	TCP_TYPE_SYN,	       /* syn */
	TCP_TYPE_ACK,          /* ack  */
	TCP_TYPE_SYN_ACK,      /* syn ack ?? */
	TCP_TYPE_FIN,          /* fin  */
	TCP_TYPE_FIN_ACK,      /* fin ack ?? */
	TCP_TYPE_RESET,        /* reset  */
	TCP_TYPE_PUSH,         /* push  */
	TCP_TYPE_MAX = 16,	
};


/*
TCP 状态机
*/
enum TCP_STATUS
{	
	TCP_STATUS_LISTEN = 0,    /* 开始监听 */
	TCP_STATUS_SYN_SENT,	  /* 侦听来自远方的TCP端口的连接请求 */
	TCP_STATUS_SYN_RECEIVED,  /* SYN-RECEIVED：再收到和发送一个连接请求后等待对方对连接请求的确认 */
	TCP_STATUS_ESTABLISHED,   /* 三次握手结束，连接建立 */
	TCP_STATUS_FIN_WAIT_1,    /* FIN-WAIT-1：等待远程TCP连接中断请求，或先前的连接中断请求的确认 */
	TCP_STATUS_FIN_WAIT_2,    /* FIN-WAIT-2：从远程TCP等待连接中断请求 */
	TCP_STATUS_CLOSE_WAIT,    /* CLOSE-WAIT：等待从本地用户发来的连接中断请求 */
	TCP_STATUS_CLOSING,       /* CLOSING：等待远程TCP对连接中断的确认 */
	TCP_STATUS_LAST_ACK,      /* LAST-ACK：等待原来的发向远程TCP的连接中断请求的确认 */
	TCP_STATUS_TIME_WAIT,     /* TIME_WAIT: 等待足够的时间以确保远程TCP接收到连接中断请求的确认 */
	TCP_STATUS_CLOSED,        /* CLOSED：没有任何连接状态 */
	TCP_STATUS_MAX = 12
};


/* TCP 事件 */
enum TCP_EVENT
{	
	TCP_EVENT_INVALID = 0,
	TCP_EVENT_SND_DATA,	
	TCP_EVENT_SND_FIN,
	TCP_EVENT_RCV_SYN,
	TCP_EVENT_RCV_SYN_ACK,
	TCP_EVENT_RCV_ACK,
	TCP_EVENT_RCV_FIN,
	TCP_EVENT_RCV_FIN_ACK,
	TCP_EVENT_RCV_RESET,
	TCP_EVENT_RCV_PUSH,	
	TCP_EVENT_WAIT_TIMEOUT,	/* 连接超时 */
	TCP_EVENT_MAX = 16
};


/* 发送和接收报文链表 */
/* 发送重传报文的数据结构 */
struct pktInfo
{
	uint8_t *data;			/* 已发送但尚未确认的报文 */
	uint32_t dataLen;		/* 报文长度 */
	uint32_t waitSeq;		/* 未发送的报文长度 */
	uint32_t ackTime;		/* 接收应答的次数 */
	uint8_t  unackcount; 
	uint8_t  restransmit;
	struct pktInfo *next;	/* 指向下一个要发送的报文指针 */
	struct pktInfo *prev;	/* 指向前一个要发送的报文指针 */
};


/* tcp session 的 hash key */
struct tcp_key
{
	uint32_t    sip;        /* 对端设备的 ip */
	uint32_t    dip;	    /* 本机的接口 ip */
	uint16_t    sport;      /* 对端的 tcp 发送端口 */
	uint16_t    dport;      /* 本机的 tcp 接收端口 */
	uint32_t    vpn;        /* ip 所属的 l3vpn */	
};


/* tcp 会话数据结构 */
struct tcp_session
{
	enum TCP_STATUS status; /* 协议状态 */	
	int         module_id;  /* 该会话所属的 module id, 用于 TCP 连接断开时通知 */
	uint32_t    vpn;        /* ip 所属的 l3vpn */	
	uint32_t    sip;        /* 对端设备的 ip */
	uint32_t    dip;	    /* 本机的接口 ip */
	struct ipv6_addr dipv6;
	struct ipv6_addr sipv6;
	uint16_t    sport;      /* 对端的 tcp 发送端口 */
	uint16_t    dport;      /* 本机的 tcp 接收端口 */
	uint32_t	seq;     	/* 序列号 */
	uint32_t	ack_seq; 	/* 确认序列号 */	
	uint8_t	    headlen; /*  headlen * 4为tcp 头长度 */
	uint8_t	    flag;       /* bit1: fin, bit2: syn, bit3: reset, bit4: push, bit5: ack, bit6: urgent, bit7: ece, bit8: cwr */
	uint8_t 	win_scale;	/* 窗口扩大因子*/
	uint16_t	urg_ptr; 	/* 紧急指针 */
	uint32_t 	seq_unack; 	/* 最早的未确认过的序号*/
	uint32_t 	seq_next;	/* 下次的发送序号*/
	uint32_t 	seq_last;	/* 上次接收报文的序号，用于更新发送窗口*/
	uint32_t 	ack_last;	/* 上次接收报文的确认序号，用于更新发送窗口*/
	uint32_t 	seq_next_recv;		/* 下一个接收序号*/	
	//uint32_t 	seq_iss;	        /* 发送方的初始序号 */	
	uint16_t 	send_len;			/* 已发送报文长度 */	
	uint32_t 	send_wnd;	        /* 接收方提供的窗口*/
	uint32_t 	recv_wnd;	        /* 接收窗口，向发送方通告*/	
	uint16_t 	cwnd;				/* 拥塞窗口 */
	uint16_t 	ssthresh;			/* 慢启动门限 */	
	uint16_t    mss;
	uint8_t 	timeout; 			/* syn_ack 等待应答超时标志 */
	uint8_t 	reackcnt; 			/* syn_ack 等待应答超时标志 */
	uint32_t    timestampreply;		/* 应答报文的时间戳 */
	struct pktInfo    *send_list;	/* 发送报文缓存队列 */
	struct pkt_buffer *recv_list;	/* 接收报文缓存队列 */	
	uint16_t    alive_time_count;   /* 距离上次发报文的时间计数器 */
	u_char        v6session;
	uint8_t     ttl;
};


extern struct hash_table tcp_session_table;   /* tcp session hash table */

void tcp_init(void);
int tcp_rcv(struct pkt_buffer *pkt);
int tcp_pkt_from_app_process(struct pkt_buffer *pkt);
int tcp_output(struct pkt_buffer *pkt);
int tcp_port_alloc_for_app(struct ipc_msghdr_n *phdr,void *pdata, int data_len, int data_num, uint16_t sender_id, enum IPC_OPCODE opcode);
int tcp_session_open_for_app(struct ipc_msghdr_n *phdr,void *pdata, int data_len, int data_num, uint16_t sender_id, enum IPC_OPCODE opcode);
int tcp_decap(struct pkt_buffer *pkt);
int tcp_session_close(struct ip_proto *proto);
int tcp_session_delete(struct tcp_session *psess);
int tcp_session_change_notify_app(struct tcp_session *psession,enum IPC_OPCODE opcode);
struct tcp_session *tcp_session_create(struct tcp_session *psess);
int tcp_session_add(struct tcp_session *psess);
int tcp_rcv_process(struct pkt_buffer *pkt, struct tcp_session *psess, int module_id);
int tcp_pkt_recv_usr_data_process(struct tcp_session *psess, struct pkt_buffer *pkt, int module_id);

#endif

