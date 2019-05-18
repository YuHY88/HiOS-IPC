/*
*   define of bfd 全局数据
*/

#ifndef HIOS_BFD_H
#define HIOS_BFD_H


#include <lib/types.h>
#include <lib/command.h>
#include <lib/msg_ipc_n.h>
#include <lib/hptimer.h>
#include <lib/oam_common.h>

#define BFD_NUM_MAX   1024		/* bfd 的规格 */
#define BFD_SESS_DISC_MAX 65535	/*会话标识<1-65535>*/
#define BFD_SESS_LOCAL_ID_MAX 1025 /*bfd本端id号*/
#define BFD_CC_INTERVAL 1000000	/*默认发送间隔3.3ms,单位为us*/
#define BFD_DETECT_MULTIPLIER 5	/*默认检测次数*/
#define BFD_WTR_INTERVAL 30		/*默认延迟up的时间间隔*/

#define BFD_PRIORITY	7
#define BFD_TTL			255

#define BFD_PASSIVE		0		//会话工作模式（主动/被动）
#define BFD_ACTIVE 		1

#define BFD_SNMP_DISABLE 0	   //bfd 告警
#define BFD_SNMP_ENABLE  1

#define BFD_MPLS_DISABLE 0		//mpls bfd 使能开关
#define BFD_MPLS_ENABLE  1

#define BFD_ECHO_DISABLE 0		//bfd echo 功能开关
#define BFD_ECHO_ENABLE  1

#define BFD_DEMAND_DISABLE 0	//bfd 查询模式使能开关
#define BFD_DEMAND_ENABLE  1

#define BFD_UP_TIME_DISABLE 0	//bfd 检测会话异常监测开关
#define BFD_UP_TIME_ENABLE  1
#define BFD_UP_WAIT_COUNTER  30

#define BFD_VERSION 1
#define BFD_LENGTH 24
#define BFD_SRCPORT 49152
#define BFD_DSTPORT 3784
#define BFD_MULTIHOP_DSTPORT 4784
#define BFD_NEGO_INTERVAL 1000000	/*默认协商间隔1s,单位为us*/
#define BFD_DFT_MULTI_INADDR 	0xe00000B8U	 /* 默认组播地址224.0.0.184   */
#define INTERFACE_NAMSIZE   32
#define BFD_SIZE			20
#define BFD_SEND 0
#define BFD_RECV 1
#define BFD_DISABLE  0
#define BFD_ENABLE   1

enum BFD_SUBTYPE
{
	BFD_SUBTYPE_SESSION = 0,
	BFD_SUBTYPE_GLOBAL,
	BFD_SUBTYPE_IFNAME,
	BFD_SUBTYPE_BIND_OSPF,
	BFD_SUBTYPE_UNBIND_OSPF,
	BFD_SUBTYPE_BIND_ISIS,
	BFD_SUBTYPE_UNBIND_ISIS,
	BFD_SUBTYPE_BIND_RIP,
	BFD_SUBTYPE_UNBIND_RIP,
	BFD_SUBTYPE_BIND_BGP,
	BFD_SUBTYPE_UNBIND_BGP,
	BFD_SUBTYPE_CFG,
	BFD_SUBTYPE_REPLY,
	BFD_SUBTYPE_BIND_LDP,
	BFD_SUBTYPE_UNBIND_LDP,
	BFD_SUBTYPE_TEMPLATE,
	BFD_SUBTYPE_DETAIL,
};

/* bfd 类型 */
enum BFD_TYPE
{
	BFD_TYPE_INVALID = 0,
	BFD_TYPE_INTERFACE,  /* bfd for 接口 */
	BFD_TYPE_IP,         /* bfd for ip */
	BFD_TYPE_PW,	     /* bfd for pw */
	BFD_TYPE_LSP,	     /* bfd for lsp */
	BFD_TYPE_TUNNEL,	 /* bfd for lsp */
	BFD_TYPE_MAX = 8
};

/* bfd 会话状态 */
enum BFD_STATUS
{
	BFD_STATUS_ADMINDOWN = 0, /* 管理 down 状态 */
	BFD_STATUS_DOWN,          /* down 状态 */
	BFD_STATUS_INIT,          /* INIT 状态 */
	BFD_STATUS_UP 	          /* UP 状态 */
};

/* bfd 事件 */
enum BFD_EVENT
{
	BFD_EVENT_RECV_ADMINDOWN = 0,  /* 收到管理 down 报文 */
	BFD_EVENT_RECV_DOWN,           /* 收到 down 报文 */
	BFD_EVENT_RECV_INIT,           /* 收到 INIT 报文 */
	BFD_EVENT_RECV_UP,	           /* 收到 UP 报文 */
	BFD_EVENT_RECV_DOWN_POLL,		/* 收到 down 报文 +POLL*/
	BFD_EVENT_RECV_INIT_POLL,		/* 收到 INIT 报文 +POLL*/
	BFD_EVENT_RECV_FIRST_UP,	   /* 收到 第一个UP 报文 */
	BFD_EVENT_ENABLE,	   			/* 使能会话*/
	BFD_EVENT_DISABLE,				/* 使能会话*/
	BFD_EVENT_SESSION_DELETE,	   /* 会话被删除*/
	BFD_EVENT_TIMEOUT,			   /* 等待超时 */
	BFD_EVENT_INIT_TIMEOUT,			/* init超时 */
	BFD_EVENT_INTERVAL_INC,		/* 间隔变大*/
	BFD_EVENT_INTERVAL_DEC,		/* 间隔变小*/
	BFD_EVENT_CHANGE_PARA, 		/* 参数改变*/
	BFD_EVENT_FIRST_UP,				/* 会话首次up，发送poll,发送检测阶段的间隔*/
	BFD_EVENT_BIND_IF,			/*与IF绑定*/
	BFD_EVENT_BIND_PW,			/*与pw绑定*/
	BFD_EVENT_BIND_LSP,			/*与LSP绑定*/
	BFD_EVENT_BIND_TUNNEL, 		/*与TUNNEL绑定*/
	BFD_EVENT_BIND_IF_IP,
	BFD_EVENT_MAX = 30	
};


/* bfd 会话从up变成其他状态的原因*/
enum BFD_DIAG
{
	BFD_DIAG_NO_DIAG = 0, 	  /* 无诊断*/
	BFD_DIAG_DETECT_EXPIRE,   /* 检测定时器超时*/
	BFD_DIAG_EHCO_FAIL,          /* 回声功能失败*/
	BFD_DIAG_NEIGHBOR_DOWN,	     /* 会话邻居down*/
	BFD_DIAG_FORWARD_RESET,		/*转发平面重新设置*/
	BFD_DIAG_PATH_DOWN,			/*路径down*/
	BFD_DIAG_CON_PATH_DOWN,		/*链接路径down*/
	BFD_DIAG_ADMIN_DOWN 		/*管理down*/
};

struct list_bfd_sess
{
	struct list_bfd_sess *prev;
	struct list_bfd_sess *next;
	struct bfd_sess *psess;
};

/* bfd 全局配置数据结构 */
struct bfd_global
{
	uint8_t  enable;			/* 全局使能 */	
	uint8_t  priority;          /* 报文的 tos */
	uint8_t  multiplier;        /* 检测次数 */	
	uint8_t  debug_packet;      /* packet 调试开关 */
	uint8_t  debug_fsm;         /* fsm 调试开关 */
	uint16_t wtr;               /* wtr 时间 */
	uint8_t  pad;
	uint16_t up_count;
	uint16_t down_count;
	uint32_t cc_interval;		/* 发送间隔，默认 10ms */
	uint16_t ttl;               /*报文ttl*/
	uint8_t  bfd_h3c_enable;
	uint8_t  mpls_bfd_enable;   /*华三全局使能mpls bfd*/
	uint8_t  bfd_init_mode;   	//会话工作模式（主动/被动）；
	uint8_t  ip_router_alert;	//Set the Router Alert option in BFD control packets for MPLS
	uint8_t  snmp_trap;			//bfd 告警使能；
	uint8_t  mult_auth;			//	全局多跳bfd配置，默认参数存在。mpls bfd 使能后，若未配置模板 则使用此模板；
	uint16_t mult_detect;
	uint16_t mult_hop_port;
	uint32_t mult_hop_cc_tx_interval;
	uint32_t mult_hop_cc_rx_interval;
	uint16_t total_bfd;       //bfd的会话总个数；
	uint16_t total_up;
	uint16_t total_init;
	uint16_t total_down;
	uint16_t bfd_for_pw;  	//bfd for pw 的会话个数；
	uint16_t bfd_for_pw_up;  
	uint16_t bfd_for_pw_init;
	uint16_t bfd_for_pw_down;
	uint16_t bfd_for_lsp;	 //bfd for lsp 的会话个数；
	uint16_t bfd_for_lsp_up;
	uint16_t bfd_for_lsp_init;
	uint16_t bfd_for_lsp_down;
	uint16_t bfd_for_onehop;	//bfd for 单跳/多跳的会话个数；
	uint16_t bfd_for_multhop;
	uint16_t hop_bfd_up;
	uint16_t hop_bfd_init;
	uint16_t hop_bfd_down;
	
};
struct bfd_ip_map
{
	uint16_t sess_id;
	uint32_t dest_ip;
};

struct list *bfd_sess_list;
struct bfd_global gbfd; /* bfd 全局参数 */
struct cmd_node bfd_session_node;
struct cmd_node bfd_template_node;

uint8_t bfd_up_timeout[BFD_SESS_DISC_MAX];
uint8_t bfd_up_wait_counter[BFD_SESS_DISC_MAX];

#define BFD_DEBUG(format, ...)\
		zlog_debug(MPLS_DBG_BFD, "%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define BFD_THREAD_TIMER_OFF(x)	\
	do{if(x){\
			high_pre_timer_delete(x); \
			x = 0;\
		}\
	}while(0)
	
#define BFD_THREAD_TIMER_ADD(f, h, w, x, y) high_pre_timer_add(f, h, w, x, y)

void bfd_cmd_init(void);
int bfd_config_write (struct vty *vty);
int bfd_config_write_h3c(struct vty *vty);
#endif
