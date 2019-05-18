#ifndef HIOS_MPLS_PING_H
#define HIOS_MPLS_PING_H

#include <ftm/pkt_ip.h>
#include <ftm/pkt_udp.h>

#define JAN_1970            0x83aa7e80      /* 2208988800 1970 - 1900 in seconds */
#define PING_PW_REPLY_BY_UDP 2
#define PING_PW_REPLY_BY_PW  4

/* ping 类型 */
enum PING_TYPE_E
{
	PING_TYPE_INVALID = 0,
	PING_TYPE_IP,     /* ip ping */
	PING_TYPE_PW,     /* pw ping */
	PING_TYPE_LSP,    /* lsp ping */
	PING_TYPE_TUNNEL,  /* tunnel ping */
	TRACERT_TYPE_PW,	  /* pw tracert */
	TRACERT_TYPE_LSP,	  /* lsp tracert */
	TRACERT_TYPE_TUNNEL  /* tunnel tracert */
};

enum MPLS_PING_TLV_TYPE
{
	MPLS_PING_TLV_PAD = 0,
	MPLS_PING_TLV_DS_SINGLE,
	MPLS_PING_TLV_DS_DOUBLE,
	MPLS_PING_TLV_NO_PAD
};

/* ping 报文类型 */
enum PING_ECHO_E
{
	PING_ECHO_REQUEST = 1,
	PING_ECHO_REPLY
};

/* ping tlv类型 */
enum PING_TLV_E
{
	PING_TVL_TRAGET = 1,
	PING_TVL_DOWNSTREAM,
	PING_TVL_PAD
};


/* pad action类型 */
enum PAD_ACT_E
{
	PAD_ACT_DORP = 1,
	PAD_ACT_COPY
};


/* ping 的配置信息 */
struct ping_config 
{
	enum PING_TYPE_E type;     /* ping 报文类型 */
	enum MPLS_PING_TLV_TYPE tlv_type;
	uint8_t reply_mode;	                  /*reply mode*/
	void  *pvty;                      /* 发起命令的vty 终端*/
	uint16_t timeout;                 /* 等待超时时间*/
	uint16_t size;                    /* 报文大小*/
	uint8_t count;                    /* 连续发送多个报文*/
	uint16_t ttl_curr;                /*当前ttl*/
	uint8_t ttl;	                  /*配置的ttl*/
	uint8_t mode;
	uint8_t interval;                 /* 连续发送的间隔*/
	uint32_t sip;                     /* ip ping 的sip */
	uint32_t dip;                     /* ip ping 的dip */
	uint32_t lsp_index;               /* lsp ping 的lsp */
	uint32_t lsp_label;
	uint8_t  gre_pw;                  /* 0:tp pw,1:gre pw */
	uint32_t pw_index;	              /* pw ping 的pw */
	uint32_t pw_label;
	uint32_t ifindex;                 /* tunnel ping 的ifindex */
	uint32_t ipv4_prefix;             /* tlv */
	uint32_t nexthop;                 /* tlv */
	uint32_t pw_id;                   /*vcid*/
	uint16_t pw_type;        
	uint32_t data_len;       
	
};

/*28字节*/
struct tunnel_target_tlv
{
	uint16_t  type;
	uint16_t  len;    
	uint16_t  sub_type;
	uint16_t  sub_len;
	uint32_t  end_addr;   /* tlv */
	uint16_t  mbz;         /* tlv */
	uint16_t  tunnel_id;
	uint32_t  extend_tid;
	uint32_t  send_addr;   /* tlv */
	uint16_t  mbz1;         /* tlv */
	uint16_t  lsp_id;
};

/*40字节*/
struct tunnel_pad_tlv
{
	uint16_t  type;
	uint16_t  len;    
	uint8_t   pad_action;
	uchar     pading[35];
};

/*24字节*/
struct downstream_tlv
{
	uint16_t  type;
	uint16_t  len;   
	uint16_t  mtu;
	uint8_t   addr_type;
	uint8_t   resvd;
	uint32_t  ds_ip;
	uint32_t  ds_inter_addr;
	uint8_t   muti_path_type;
	uint8_t   depth_limit;
	uint16_t  mutipath_len;
	
	uint16_t label1;	 /* 标签的高 16bit */
	
	#if BIG_ENDIAN_ON        /* CPU 大端序 */
	uint8_t  label0:4,	 /* 标签的低 4bit */
			 exp:3,
			 bos:1;  	 /* 栈底标志 */
	#else                    /* CPU 小端序 */
	uint8_t  bos:1, 	 /* 栈底标志 */
			 exp:3,
			 label0:4;	 /* 标签的低 4bit */
	#endif
	
	uint8_t protocal;
};

/*16zijie*/
struct ping_target_tlv
{
	uint16_t  type;
	uint16_t  len;    
	uint16_t  sub_type;
	uint16_t  sub_len;
	uint32_t  ipv4_prefix;   /* tlv */
	uint8_t   prefix_len;   /* tlv */
	uchar     resvd[3];
};

/*52zijie*/
struct ping_pad_tlv
{
	uint16_t  type;
	uint16_t  len;    
	uint8_t   pad_action;
	uchar     pading[47];
};

/*24zijie*/
struct pw_target_tlv
{
	uint16_t  type;
	uint16_t  len;    
	uint16_t  sub_type;
	uint16_t  sub_len;
	uint32_t  send_addr;   /* tlv */
	uint32_t  remote_addr;   /* tlv */
	uint32_t  pw_id;
	uint16_t  pw_type;
	uint16_t  pad;
};

/*44zijie*/
struct pw_pad_tlv
{
	uint16_t  type;
	uint16_t  len;    
	uint8_t   pad_action;
	uchar     pading[39];
};

/*28*/
struct pw_pad_tlv_zx
{
	uint16_t  type;
	uint16_t  len;    
	uint8_t   pad_action;
	uchar     pading[23];
};

/*28zijie*/
struct pw_downstream_tlv
{
	uint16_t  type;
	uint16_t  len;   
	uint16_t  mtu;
	uint8_t   addr_type;
	uint8_t   resvd;
	uint32_t  ds_ip;
	uint32_t  ds_inter_addr;
	uint8_t   muti_path_type;
	uint8_t   depth_limit;
	uint16_t  mutipath_len;
	uint16_t label1;	 /* 标签的高 16bit */
	
	#if BIG_ENDIAN_ON        /* CPU 大端序 */
	uint8_t  label11:4,	 /* 标签的低 4bit */
			 exp1:3,
			 bos1:1;  	 /* 栈底标志 */
	#else                    /* CPU 小端序 */
	uint8_t  bos1:1, 	 /* 栈底标志 */
			 exp1:3,
			 label11:4;	 /* 标签的低 4bit */
	#endif
	uint8_t  protocal1;

	uint16_t label2;	 /* 标签的高 16bit */
		
	#if BIG_ENDIAN_ON        /* CPU 大端序 */
		uint8_t  label22:4,	 /* 标签的低 4bit */
				 exp2:3,
				 bos2:1; 	 /* 栈底标志 */
	#else                    /* CPU 小端序 */
		uint8_t  bos2:1, 	 /* 栈底标志 */
				 exp2:3,
				 label22:4;	 /* 标签的低 4bit */
	#endif
		uint8_t  protocal2;
};


/* mpls echo 报文头格式 32字节*/
struct mpls_echohdr
{
	uint16_t ver;			/* MPLS Echo Packet的版本号，当前版本为1 */
	uint16_t flag;			/* global flag，目前未使用 */
	uint8_t  msgType;		/* 消息类型 Echo Request(1)或EchoReply(2) */
	uint8_t	 replyMode;		/* 回应模式，1:不回，2:回udp，3:回 udp route alert, 4:回 control channel */
	uint8_t  retCode;		/* 返回的错误码，3:正常，4/5:网络故障 */
	uint8_t  retSubcode;	/* 返回的错误码 */	
	uint32_t sendHandle;	/* 发送者句柄 */
	uint32_t seqNum;		/* 序列号 */
	struct timeval sendSec;		/* 发送时间戳 */
	struct timeval rcvSec;        /* 接收时间戳 */
}__attribute__((packed));	

/* mpls lsp ping echo 报文格式32+16+52=100字节 */
struct mpls_lsp_ping_echo
{
	struct mpls_echohdr    echohdr;
	struct ping_target_tlv target_tlv;
	struct ping_pad_tlv    pad_tlv;
}__attribute__((packed));	

/* mpls lsp tracert echo 报文格式 32+16+24=72字节*/
struct mpls_lsp_tracert_echo
{
	struct mpls_echohdr    echohdr;
	struct ping_target_tlv target_tlv;
	struct downstream_tlv  downstearm_tlv;
}__attribute__((packed));	

/* mpls lsp tracert echo 报文格式 32+16=48字节*/
struct mpls_lsp_tracert_reply_echo
{
	struct mpls_echohdr    echohdr;     
	struct ping_target_tlv target_tlv;  
}__attribute__((packed));

struct mpls_lsp_tunnel_ping_echo
{
	struct mpls_echohdr echohdr;
	struct tunnel_target_tlv tunnel_tlv;
	struct tunnel_pad_tlv    pad_tlv;
}__attribute__((packed));

struct mpls_lsp_tunnel_tracert_echo
{
	struct mpls_echohdr echohdr;
	struct tunnel_target_tlv tunnel_tlv;
	struct downstream_tlv  downstream_tlv;
}__attribute__((packed));

/* mpls pw ping echo 报文格式 32+24+44=100*/
struct mpls_pw_ping_echo
{
	struct mpls_echohdr   echohdr;
	struct pw_target_tlv  target_tlv;
	struct pw_pad_tlv     pad_tlv;
}__attribute__((packed));	

/* mpls pw ping echo 报文格式 32+24=56字节*/
struct mpls_pw_ping_echo_zx
{
	struct mpls_echohdr   echohdr;
	struct pw_target_tlv  target_tlv;
}__attribute__((packed));	


struct mpls_pw_tracert_echo_gre
{
	struct mpls_echohdr      echohdr;
	struct pw_target_tlv     target_tlv;
	struct downstream_tlv    gre_downstream_tlv;
}__attribute__((packed));

/* mpls pw tracert echo 报文格式 32+24+28=84*/
struct mpls_pw_tracert_echo
{
	struct mpls_echohdr      echohdr;
	struct pw_target_tlv     target_tlv;
	struct pw_downstream_tlv downstearm_tlv;
}__attribute__((packed));	

/* mpls pw tracert echo 报文格式 32+24=56*/
struct mpls_pw_tracert_reply_echo
{
	struct mpls_echohdr    echohdr;
	struct pw_target_tlv   target_tlv;
}__attribute__((packed));	

struct ping_ip_h  
{
	struct iphdr ip_h;
	uint32_t ip_option;
};

/* lsp ping  报文格式 */
struct lsp_ping_pkt  
{
	struct ping_ip_h iph;        
	struct udphdr    udph;
	union 
	{
		struct mpls_lsp_ping_echo         lsp_ping_echo;
		struct mpls_lsp_tracert_echo      lsp_tracert_echo;
		struct mpls_pw_ping_echo          pw_ping_echo;
		struct mpls_pw_ping_echo_zx       pw_ping_echo_zx;
		struct mpls_pw_tracert_echo       pw_tracert_echo;
		struct mpls_pw_tracert_echo_gre   pw_tracert_gre_echo;
		struct mpls_pw_tracert_reply_echo pw_tracert_reply_echo; 
		//struct mpls_tunnel_ping_echo	lsp_ping_echo;
		//struct mpls_tunnel_tracert_echo lsp_tracert_echo;
	}echo;
}__attribute__((packed));


/* lsp ping  报文格式 */
struct lsp_ping_pkt_bak  
{
	struct iphdr ip_h;
	struct udphdr    udph;
	union 
	{
		struct mpls_lsp_ping_echo         lsp_ping_echo;
		struct mpls_lsp_tracert_echo      lsp_tracert_echo;
		struct mpls_pw_ping_echo          pw_ping_echo;
		struct mpls_pw_ping_echo_zx       pw_ping_echo_zx;
		struct mpls_pw_tracert_echo       pw_tracert_echo;
		struct mpls_pw_tracert_echo_gre   pw_tracert_gre_echo;
		struct mpls_pw_tracert_reply_echo pw_tracert_reply_echo; 
		//struct mpls_tunnel_ping_echo	lsp_ping_echo;
		//struct mpls_tunnel_tracert_echo lsp_tracert_echo;
	}echo;
}__attribute__((packed));


/* ping_list_node  报文格式 */
struct ping_mpls_node  
{
	struct ping_config pping;
	struct lsp_ping_pkt preq; 
};


/* mpls ping 的全局数据结构 */
struct mpls_ping_global
{
	struct list *preq_list;  /* mpls request 报文链表*/
	TIMERID     ptimer;  /* 等待reply 的定时器*/
	uint32_t pkt_recv;     /* 接收报文数*/
	uint32_t pkt_send;     /* 发送报文数*/
	uint32_t pkt_error;    /* 收到的错误报文数*/
};


void mpls_ping_set_lsp_echo(struct mpls_lsp_ping_echo *pecho, enum PING_ECHO_E type);
void mpls_tracert_set_lsp_echo(struct mpls_lsp_tracert_echo *pecho, enum PING_ECHO_E type);
void mpls_ping_set_pw_echo(struct mpls_pw_ping_echo *pecho, enum PING_ECHO_E type);
void mpls_tracert_set_pw_echo(struct mpls_pw_tracert_echo *pecho, enum PING_ECHO_E type);
extern void vtysh_return (struct vty *vty, int cmd_flag);


void mpls_ping_set_echohdr(struct mpls_echohdr *pecho, enum PING_ECHO_E type);
void mpls_tracert_set_pw_echo_gre(struct mpls_pw_tracert_echo_gre *pecho, enum PING_ECHO_E type);

void mpls_ping_set_iphdr(struct ping_ip_h *piph, int data_len);
void mpls_ping_pkt_register(void);
int mpls_ping_timer_thread(void *para);
int mpls_lsp_ping_recv_request(void *pkt_cb, enum PKT_TYPE cb_type, char *data, uint32_t lsp_index);
int mpls_pw_ping_recv_request(struct ip_control *ipcb, char *data, uint32_t pwindex);
int mpls_ping_send(struct ping_config *pping, void *preq, int len);
int mpls_ping_send_request(struct ping_config *pping);
int mpls_ping_recv_reply(struct ip_control *ipcb_rcv, struct mpls_echohdr *echo, void *data);

int mpls_lsp_ping_recv_reply(struct ip_control *ipcb_rcv, struct mpls_echohdr  *echo, void *data);
int mpls_pw_ping_recv_reply(struct ip_control *ipcb_rcv, struct mpls_echohdr  *echo, void *data);

int mpls_ping_recv(struct pkt_buffer *pkt);
void mpls_ping_init(void);

#endif
