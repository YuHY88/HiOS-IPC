/*
*          tcp packet define and forward function
*/

#ifndef HIOS_PKT_TCP_H
#define HIOS_PKT_TCP_H

#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/msg_ipc.h>

#define TCP_HEADER_SIZE         20  /* tcp ͷ���� */
#define TCP_OPTION_SIZE         40  /* tcp ѡ��ȣ���� 40 �ֽ� */

#define TCP_PKT_RETRANMSMIT_MAX     5

/* tcp �˿ڶ��� */
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

#define TCP_PORT_RESERVE        49152  /* ���� TCP �˿ڵ���ʼֵ */
#define TCP_PORT_MAX            65535  /* TCP �˿ڵ����ֵ */


/* tcp flag bit define */
#define TCP_FLAG_FIN		    0x01   /* tcp fin */
#define TCP_FLAG_SYN		    0x02   /* tcp syn */
#define TCP_FLAG_RST		    0x04   /* tcp reset */
#define TCP_FLAG_PSH		    0x08   /* tcp push */
#define TCP_FLAG_ACK		    0x10   /* tcp ack */
#define TCP_FLAG_URG		    0x20   /* tcp urgent, ��ʶ urg_ptr ��Ч */
#define TCP_FLAG_ECE		    0x40   /* tcp ecn echo */
#define TCP_FLAG_CWR		    0x80   /* tcp cwr */

/* tcp urg_ptr value */
#define TCP_URG_VALID	        0x0100
#define TCP_URG_NOTYET	        0x0200
#define TCP_URG_READ	        0x0400


#define TCP_WINDOWN_BIT         16     /* TCP �������ڵ����λ�� */
#define TCP_WINDOWN_BIT_SHIFT   14     /* TCP ������������λ�� */

#define TCP_WINDOWN_MAX         65535  /* TCP ���ڵ����ֵ�����ֽ�Ϊ��λ */
#define TCP_WINDOWN_DEFAULT     9600   /* TCP ���ڵ�Ĭ��ֵ */
#define TCP_MSS_MIN             536    /* TCP payload ��Ĭ����Сֵ */
#define TCP_MSS_DEFAULT         1460   /* TCP payload ��Ĭ��ֵ */


/* TCP option length */
#define TCP_OPTLEN_MSS          4  /* max payload length */
#define TCP_OPTLEN_WIN_SCALE    3  /* window scale len */
#define TCP_OPTLEN_SACK         2  /* SACK Permitted/block len */
#define TCP_OPTLEN_TIMESTAMP    10 /* timestamp option length */


/* TCP option type */
enum TCP_OPT
{
	TCP_OPT_EOL = 0,           /* ѡ����� */
	TCP_OPT_NOP = 1,	       /* Padding, ���� 4 �ֽڶ��� */
	TCP_OPT_MSS = 2,           /* mss */
	TCP_OPT_WIN_SCALE = 3,     /* win scale */
	TCP_OPT_SACK_PERMIT = 4,   /* SACK Permitted */
	TCP_OPT_SACK_BLOCK = 5,    /* SACK Block */
	TCP_OPT_TIMESTAMP = 8,     /* ʱ��� */
	TCP_OPT_MD5 = 19,
	TCP_OPT_MAX = 20,	
};

/* tcp payload ��󳤶�ѡ��, 4 �ֽ� */
struct tcp_opt_mss
{
	uint8_t  kind;          /* ѡ������, �μ� enum TCP_OPT ���� */
	uint8_t  length;        /* ѡ��� = 4, TCP_OPTLEN_MSS */
	uint16_t mss;	        /* ���� payload ��󳤶� */
};
#define TCP_MD5_OPTION_SET 1
struct tcp_opt_md5
{
	uint8_t  kind;         
	uint8_t  length;       
	char digest[16];	      
};

/* tcp ������������,3 �ֽ� */
struct tcp_opt_winscale
{
	uint8_t kind;           /* ѡ������, �μ� enum TCP_OPT ���� */
	uint8_t length;         /* ѡ��� = 3,TCP_OPTLEN_WIN_SCALE  */
	uint8_t winscale;	    /* ������������, ��� 14, 16 + winscale ��������ֵ */
}/*__attribute__ ((aligned (4)))*/;


/* tcp ʱ���ѡ�� ,10 �ֽ� */
struct tcp_opt_timestamp
{
	uint8_t  kind;          /* ѡ������, �μ� enum TCP_OPT ���� */
	uint8_t  length;        /* ѡ��� = 10, TCP_OPTLEN_TIMESTAMP */
	uint32_t timestamp;     /* ���ͱ��ĵ�ʱ��� */
	uint32_t timestampreply;/* ʱ��� echo reply, ���ƽ��ձ��ĵ�ʱ��� */
}/*__attribute__ ((aligned (8)))*/;


/* tcp seletcive ack ѡ��,2 �ֽ� */
struct tcp_sack
{
	uint8_t kind;           /* ѡ������, �μ� enum TCP_OPT ���� */
	uint8_t length;         /* ѡ��� = 2, TCP_OPTLEN_SACK_PERM */
}/*__attribute__ ((aligned (8)))*/;


/* tcp ����ͷ: 20 �ֽ� */
struct tcphdr 
{
	uint16_t	sport;      /* TCP Դ�˿� */
	uint16_t	dport;      /* TCP Ŀ�Ķ˿� */
	uint32_t	seq;        /* ���ͱ��ĵ����к� */
	uint32_t	ack_seq;    /* ���ձ��ĵ�ȷ�����к�, flag bit5 ��λ����Ч */
	
#if BIG_ENDIAN_ON          
	uint8_t     hlen:4,     /* tcp ͷ����, �� 4 �ֽ�Ϊ��λ */
			    reserv:4;
#else                      
	uint8_t     reserv:4,    
			    hlen:4;   	 /* tcp ͷ����, �� 4 �ֽ�Ϊ��λ */
#endif

	uint8_t	    flag;       /* bit1: fin, bit2: syn, bit3: reset, bit4: push, bit5: ack, bit6: urgent, bit7: ece, bit8: cwr */
	uint16_t	window;     /* ���մ��� */
	uint16_t	checksum; 
	uint16_t	urg_ptr;    /* ����ָ��,flag bit6 ��λ����Ч */
};


/* TCP �������� */
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
TCP ״̬��
*/
enum TCP_STATUS
{	
	TCP_STATUS_LISTEN = 0,    /* ��ʼ���� */
	TCP_STATUS_SYN_SENT,	  /* ��������Զ����TCP�˿ڵ��������� */
	TCP_STATUS_SYN_RECEIVED,  /* SYN-RECEIVED�����յ��ͷ���һ�����������ȴ��Է������������ȷ�� */
	TCP_STATUS_ESTABLISHED,   /* �������ֽ��������ӽ��� */
	TCP_STATUS_FIN_WAIT_1,    /* FIN-WAIT-1���ȴ�Զ��TCP�����ж����󣬻���ǰ�������ж������ȷ�� */
	TCP_STATUS_FIN_WAIT_2,    /* FIN-WAIT-2����Զ��TCP�ȴ������ж����� */
	TCP_STATUS_CLOSE_WAIT,    /* CLOSE-WAIT���ȴ��ӱ����û������������ж����� */
	TCP_STATUS_CLOSING,       /* CLOSING���ȴ�Զ��TCP�������жϵ�ȷ�� */
	TCP_STATUS_LAST_ACK,      /* LAST-ACK���ȴ�ԭ���ķ���Զ��TCP�������ж������ȷ�� */
	TCP_STATUS_TIME_WAIT,     /* TIME_WAIT: �ȴ��㹻��ʱ����ȷ��Զ��TCP���յ������ж������ȷ�� */
	TCP_STATUS_CLOSED,        /* CLOSED��û���κ�����״̬ */
	TCP_STATUS_MAX = 12
};


/* TCP �¼� */
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
	TCP_EVENT_WAIT_TIMEOUT,	/* ���ӳ�ʱ */
	TCP_EVENT_MAX = 16
};


/* ���ͺͽ��ձ������� */
/* �����ش����ĵ����ݽṹ */
struct pktInfo
{
	uint8_t *data;			/* �ѷ��͵���δȷ�ϵı��� */
	uint32_t dataLen;		/* ���ĳ��� */
	uint32_t waitSeq;		/* δ���͵ı��ĳ��� */
	uint32_t ackTime;		/* ����Ӧ��Ĵ��� */
	uint8_t  unackcount; 
	uint8_t  restransmit;
	struct pktInfo *next;	/* ָ����һ��Ҫ���͵ı���ָ�� */
	struct pktInfo *prev;	/* ָ��ǰһ��Ҫ���͵ı���ָ�� */
};


/* tcp session �� hash key */
struct tcp_key
{
	uint32_t    sip;        /* �Զ��豸�� ip */
	uint32_t    dip;	    /* �����Ľӿ� ip */
	uint16_t    sport;      /* �Զ˵� tcp ���Ͷ˿� */
	uint16_t    dport;      /* ������ tcp ���ն˿� */
	uint32_t    vpn;        /* ip ������ l3vpn */	
};


/* tcp �Ự���ݽṹ */
struct tcp_session
{
	enum TCP_STATUS status; /* Э��״̬ */	
	int         module_id;  /* �ûỰ������ module id, ���� TCP ���ӶϿ�ʱ֪ͨ */
	uint32_t    vpn;        /* ip ������ l3vpn */	
	uint32_t    sip;        /* �Զ��豸�� ip */
	uint32_t    dip;	    /* �����Ľӿ� ip */
	struct ipv6_addr dipv6;
	struct ipv6_addr sipv6;
	uint16_t    sport;      /* �Զ˵� tcp ���Ͷ˿� */
	uint16_t    dport;      /* ������ tcp ���ն˿� */
	uint32_t	seq;     	/* ���к� */
	uint32_t	ack_seq; 	/* ȷ�����к� */	
	uint8_t	    headlen; /*  headlen * 4Ϊtcp ͷ���� */
	uint8_t	    flag;       /* bit1: fin, bit2: syn, bit3: reset, bit4: push, bit5: ack, bit6: urgent, bit7: ece, bit8: cwr */
	uint8_t 	win_scale;	/* ������������*/
	uint16_t	urg_ptr; 	/* ����ָ�� */
	uint32_t 	seq_unack; 	/* �����δȷ�Ϲ������*/
	uint32_t 	seq_next;	/* �´εķ������*/
	uint32_t 	seq_last;	/* �ϴν��ձ��ĵ���ţ����ڸ��·��ʹ���*/
	uint32_t 	ack_last;	/* �ϴν��ձ��ĵ�ȷ����ţ����ڸ��·��ʹ���*/
	uint32_t 	seq_next_recv;		/* ��һ���������*/	
	//uint32_t 	seq_iss;	        /* ���ͷ��ĳ�ʼ��� */	
	uint16_t 	send_len;			/* �ѷ��ͱ��ĳ��� */	
	uint32_t 	send_wnd;	        /* ���շ��ṩ�Ĵ���*/
	uint32_t 	recv_wnd;	        /* ���մ��ڣ����ͷ�ͨ��*/	
	uint16_t 	cwnd;				/* ӵ������ */
	uint16_t 	ssthresh;			/* ���������� */	
	uint16_t    mss;
	uint8_t 	timeout; 			/* syn_ack �ȴ�Ӧ��ʱ��־ */
	uint8_t 	reackcnt; 			/* syn_ack �ȴ�Ӧ��ʱ��־ */
	uint32_t    timestampreply;		/* Ӧ���ĵ�ʱ��� */
	struct pktInfo    *send_list;	/* ���ͱ��Ļ������ */
	struct pkt_buffer *recv_list;	/* ���ձ��Ļ������ */	
	uint16_t    alive_time_count;   /* �����ϴη����ĵ�ʱ������� */
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

