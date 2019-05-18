/*
*   管理MPLSAPS会话
*/
#ifndef HIOS_MPLS_APS_H
#define HIOS_MPLS_APS_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/oam_common.h>
#include "mpls/mpls_if.h"

#define MPLSAPS_NUM  255

/* APS  States  define */ 
#define APS_STATE_LO           15 //Lockout of Protection
#define APS_STATE_SF_P         14 //Signal Fail for Protection
#define APS_STATE_FS           13 //Forced Switch
#define APS_STATE_SF_W         11 //Signal Fail for Working
#define APS_STATE_SD           9  //Signal Degrade
#define APS_STATE_MS           7  //Manual Switch
#define APS_STATE_WTR          5  //Wait to Restore
#define APS_STATE_EXER         4  //Exercise
#define APS_STATE_RR           2  //Reverse Request
#define APS_STATE_DNR          1  //DO Not Revert
#define APS_STATE_NR           0  //No Request

/* APS 状态机输入事件类型定义 */
enum APS_EVENT_E
{
	APS_EVENT_REMOTE_NR = 0,
	APS_EVENT_REMOTE_DNR = 1,
	APS_EVENT_REMOTE_RR = 2,
	APS_EVENT_REMOTE_EXER = 4,
	APS_EVENT_REMOTE_WTR = 5,
	APS_EVENT_REMOTE_MS = 7,
	APS_EVENT_REMOTE_SD = 9,
	APS_EVENT_REMOTE_SF_W = 11,
	APS_EVENT_REMOTE_FS = 13,
	APS_EVENT_REMOTE_SF_P = 14,
	APS_EVENT_REMOTE_LO = 15,
	APS_EVENT_LOCAL_LO,                 
	APS_EVENT_LOCAL_FS,                  
	APS_EVENT_LOCAL_SF_W,            
	APS_EVENT_LOCAL_W_RECOVER_FROM_SF,  
	APS_EVENT_LOCAL_SF_P,
	APS_EVENT_LOCAL_P_RECOVER_FROM_SF,
	APS_EVENT_LOCAL_SD_W,            
	APS_EVENT_LOCAL_W_RECOVER_FROM_SD,  
	APS_EVENT_LOCAL_SD_P,
	APS_EVENT_LOCAL_P_RECOVER_FROM_SD,
	APS_EVENT_LOCAL_MS,
	APS_EVENT_LOCAL_CLEAR,
	APS_EVENT_LOCAL_EXER,
	APS_EVENT_LOCAL_WTR,	
    APS_EVENT_MAX = 30
};

/* aps 协议状态 */
enum MPLS_APS_STATUS
{
	MPLS_APS_STATUS_DISABLE = 0,
	MPLS_APS_STATUS_ENABLE
};


/* aps W/P oam states define */
enum APS_OAM_STATE_E
{
	APS_OAM_STATE_INVALID = 0,  //
	APS_OAM_STATE_SF_DOWN_W ,
	APS_OAM_STATE_SF_DOWN_P ,
	APS_OAM_STATE_SF_UP_W ,
	APS_OAM_STATE_SF_UP_P ,
	APS_OAM_STATE_SD_DOWN_W,
	APS_OAM_STATE_SD_DOWN_P ,
	APS_OAM_STATE_SD_U_W, 
	APS_OAM_STATE_SD_U_P ,
	APS_OAM_STATE_MAX = 10
};

/* APS service type define */
enum APS_TYPE_E
{
	APS_TYPE_LSP = 0,
	APS_TYPE_PW = 1,
	APS_TYPE_MAX = 2
};

/* elps 全局数据结构 */
struct mplsaps_global
{
	uint32_t        pkt_recv;	    	/* 接收报文统计 */
	uint32_t        pkt_send;	   	 /* 发送报文统计 */
	uint32_t        pkt_err;	    	/* 错误报文统计 */	
	uint16_t        sess_total;     	/* session 总数*/
	uint16_t        sess_enable;		/* enable 的 session 数 */
	uint8_t	        debug_packet;	/*debug 调试开关*/
}__attribute__((packed));


/* mplsaps 会话的基本数据结构 */
struct mplsaps_info
{
	uint16_t		  sess_id;	        /* aps_id 等于 session id */
	uint32_t		  master_index;		 /* 检测对象的索引，for 接口是 ifindex，for pw 是 pwindex，for lsp 是 lspindex */
	uint32_t		  backup_index;		 /* 保护路径index，同上 */
	uint32_t		  bind_index;		 /* 命令行输入的pw index*/
	uint16_t          keepalive;   /* keepalive 间隔 */	 
	uint16_t          holdoff;     /* holdoff 时间 */
	uint16_t          wtr;         /* wtr 时间 */
	uint8_t           current_status; /* 保护组当前本地状态 */
	uint8_t           current_event;  /* 保护组当前事件(本地触发或者APS发送) */
	enum BACKUP_E     backup;    /* 1: 主 工作, 2: 备 工作 */
	enum FAILBACK_E   failback;	  /* 保护是否回切 */
	enum APS_TYPE_E	            type;	        /* APS  检测对象类型 */
	enum MPLS_APS_STATUS        status;	        /* aps 状态1: enable, 0:disable */
	uint8_t          fs_enable;	  /* 1: force switch 使能 */  
	uint8_t          ms_enable;	  /* 1: manual switch 使能 */ 
	uint8_t          lck_enable;  /* 1: lockout 使能 */
	uint8_t          priority;    /* 报文的优先级，默认是 6 */
}__attribute__((packed));

struct snmp_aps_sess{
	struct mplsaps_info info;
	uchar  master_name[NAME_STRING_LEN];
	uchar  backup_name[NAME_STRING_LEN];
};

/* aps 会话数据结构 */
struct aps_sess
{
	struct mplsaps_info         info;           /* aps配置信息 */
	struct mpls_if              *pif;
	struct tunnel_if            *tunnel_pif;
    TIMERID          	pkeepalive_timer;/* keepalive 定时器 */
	TIMERID              pholdoff_timer;	/* holdoff 定时器 */
	TIMERID              pwtr_timer;		/* wtr 定时器 */
	TIMERID              dfop_timer;		/* dfop 定时器 */
};


extern struct mplsaps_global gmplsaps;              /* mplsaps 全局数据结构 */
extern struct hash_table  mplsaps_session_table; /* mplsaps会话 hash 表，用 sess_id 作为 key */


void mplsaps_init(void);
/* 对 mplsaps 会话的操作 */
void mplsaps_sess_table_init ( unsigned int size );
struct aps_sess *mplsaps_sess_create(uint16_t sess_id);/* 创建一个会话 */
int mplsaps_sess_add(struct aps_sess *psess);   /* 添加到 hash 表 */
int mplsaps_sess_delete(uint16_t sess_id);      /* 删除一个会话 */
struct aps_sess *mplsaps_sess_lookup(uint16_t sess_id); /* 查找一个会话 */

int mplsaps_sess_enable(struct aps_sess *psess);  /* 会话 enable */
int mplsaps_sess_disable(struct aps_sess *psess); /* 会话 disable */

int mplsaps_sess_forceswitch(struct aps_sess *psess, int is_creat); /* 强制倒换, is_creat 表示倒换或清除*/
int mplsaps_sess_manualswitch(struct aps_sess *psess, int is_creat);/* 手动倒换, is_creat 表示倒换或清除*/
int mplsaps_sess_lock(struct aps_sess *psess, int is_creat);/* lockout */
void mplsaps_unbind_lsp_pw(uint16_t sess_id);



#endif

