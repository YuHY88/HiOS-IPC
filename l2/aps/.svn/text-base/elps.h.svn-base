/*
*   管理 ELPS 会话
*/
#ifndef HIOS_ELPS_H
#define HIOS_ELPS_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/oam_common.h>
#include <lib/msg_ipc.h>
#include <lib/hptimer.h>
#include "../l2_if.h"

#define ELPS_NUM  128
#define ELPS_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_ELPS, fmt, ##__VA_ARGS__ ); \
    } while(0)

enum ELPS_STATUS
{
	ELPS_NO_REQUEST  = 0,	 /* NR */
	ELPS_DO_NOT_REVERT,		 /* DNR */
	ELPS_WAIT_TO_RESTORE  =5,		 /* WTR */
	ELPS_MANUAL_SWITCH =8,	/* MS */
	ELPS_SIGNAL_FAIL_FOR_WORKING =11,	/* SF-W */
	ELPS_FORCED_SWITCH =13,		/* FS */
	ELPS_SIGNAL_FAIL_FOR_PROTECTION,	 /* SF-P */
     	ELPS_LOCKOUT_OF_PROTECTION		/* LO */

};

enum ELPS_ACTION
{
	ELPS_ACTION_INVILD = 0,
	ELPS_FORWRAD_MASTER,
	ELPS_FORWRAD_BACKUP,
};

enum ELPS_PORT_EVENT
{
	PORT_STATE_DOWN = 0,
	PORT_STATE_UP,
	CC_STATE_DOWN,
	CC_STATE_UP
};

enum ELPS_EVENT
{
	APS_ELPS_NR  = 0,	 /* NR */
	APS_ELPS_DNR,		 /* DNR */
	APS_ELPS_WTR  =5,		 /* WTR */
	APS_ELPS_MS =8,	/* MS */
	APS_ELPS_SF_W=11,	/* SF-W */
	APS_ELPS_FS=13,		/* FS */
	APS_ELPS_SF_P,	 /* SF-P */
    APS_ELPS_LO,		/* LO */
	LOCAL_ELPS_LOP,
   	LOCAL_ELPS_FS,
   	LOCAL_ELPS_SF_W,
   	LOCAL_ELPS_W_RECOVERS_FROM_SF,
   	LOCAL_ELPS_SF_P,
   	LOCAL_ELPS_P_RECOVERS_FROM_SF,
   	LOCAL_ELPS_MS,
   	LOCAL_ELPS_CLEAR,
   	LOCAL_ELPS_WTR_EXPIRES
};

/* elps 全局数据结构 */
struct elps_global
{
	uint32_t        pkt_recv;	    	/* 接收报文统计 */
	uint32_t        pkt_send;	   	 /* 发送报文统计 */
	uint32_t        pkt_err;	    	/* 错误报文统计 */	
	uint16_t        sess_total;     	/* session 总数*/
	uint16_t        sess_enable;		/* enable 的 session 数 */
	uint8_t	    debug_packet;	/*debug 调试开关*/
}__attribute__((packed));


/* aps 协议状态 */
enum APS_STATUS
{
	APS_STATUS_DISABLE = 0,
	APS_STATUS_ENABLE
};

/* aps 报文格式 */
struct aps_pkt
{
	
#if BIG_ENDIAN_ON 
	uint8_t level:3,
  		    	version:5;
	uint8_t opcode;
	uint8_t flag;
	uint8_t tlv_offset;
	uint8_t request_state:4,
			port_type_A:1,
			port_type_B:1,
			port_type_D:1,
			port_type_R:1;
	uint8_t request_signal;
	uint8_t bridge_signal;
	uint8_t bridge_type:1,
  		    	reserved:7;//session_id
#else
	uint8_t version:5,
			level:3;
	uint8_t opcode;
	uint8_t flag;
	uint8_t tlv_offset;
	uint8_t port_type_R:1,
		    	port_type_D:1,
		    	port_type_B:1,
		    	port_type_A:1,
			request_state:4;	
	uint8_t request_signal;
	uint8_t bridge_signal;
	uint8_t reserved:7,//session_id
  			bridge_type:1;
#endif
	uint8_t end_tlv;
}__attribute__((packed));

/* elps 会话的基本数据结构 */
struct elps_info
{
	 uint16_t        sess_id;
	 uint16_t	     pvlan; 	  /* primariy vlan */
	 /*one bit which is vlan id map*/
	 uint8_t 	    data_vlan_map[512];	/*value is 4096=512*8*/
	 uint8_t        data_vlan_set_flag; /* 0 no set, 1 set*/
	 uint8_t		master_port_mac[6];
	 uint8_t		backup_port_mac[6];
          uint32_t        master_port; /* 主端口 ifindex */
     	 uint32_t        backup_port; /* 备份端口 ifindex */
	 uint32_t	    active_port; 	/*forward 端口 ifindex */
     	 uint16_t        keepalive;   /* keepalive 间隔 */	 
     	 uint16_t        holdoff;     /* holdoff 时间 */
     	 uint16_t        wtr;         /* wtr 时间 */
	 uint8_t          dfop_alarm;         /* alarm for dfop */
	 enum ELPS_STATUS current_status; /* 保护组当前本地状态 */
	 enum ELPS_EVENT current_event;  /* 保护组当前事件(本地触发或者APS发送) */
	 enum APS_STATUS status;	  /* aps 状态*/
	 enum FAILBACK_E failback;	  /* 保护是否回切 */
	 uint8_t         fs_enable;	  /* 1: force switch 使能 */  
	 uint8_t         ms_enable;	  /* 1: manual switch 使能 */ 
	 uint8_t         lck_enable;  /* 1: lockout 使能 */
	 uint8_t         priority;    /* 报文的优先级，默认是 6 */
	 uint16_t 	    master_cfm_session;/*主端口上绑定的cfm session ID*/
	 uint16_t 	    backup_cfm_session;/*备端口上绑定的cfm session ID*/  
	 enum ELPS_ACTION switch_action;
}__attribute__((packed));

struct hhrelps_snmp{
	struct elps_info info;
	uchar master_name[NAME_STRING_LEN];
	uchar backup_name[NAME_STRING_LEN];
};


/* elps session 数据结构 */
struct elps_sess
{
	struct elps_info   info;            		/* 会话的基本信息 */
	TIMERID     pkeepalive_timer;/* keepalive 定时器 */
	TIMERID     pholdoff_timer;	/* holdoff 定时器 */
	TIMERID     pwtr_timer;		/* wtr 定时器 */
	TIMERID     dfop_timer;		/* dfop 定时器 */
};


extern struct elps_global gelps;              /* elps 全局数据结构 */
extern struct hash_table  elps_session_table; /* elps 会话 hash 表，用 sess_id 作为 key */


void elps_init(void);


/* 对 elps 会话的操作 */
void elps_sess_table_init ( unsigned int size );
struct elps_sess *elps_sess_create(uint16_t sess_id);/* 创建一个会话 */
int elps_sess_add(struct elps_sess *psess);   /* 添加到 hash 表 */
int elps_sess_delete(uint16_t sess_id);      /* 删除一个会话 */
struct elps_sess *elps_sess_lookup(uint16_t sess_id); /* 查找一个会话 */

int elps_sess_enable(struct elps_sess *psess);  /* 会话 enable */
int elps_sess_disable(struct elps_sess *psess); /* 会话 disable */

int elps_sess_forceswitch(struct elps_sess *psess, int is_creat); /* 强制倒换, is_creat 表示倒换或清除*/
int elps_sess_manualswitch(struct elps_sess *psess, int is_creat);/* 手动倒换, is_creat 表示倒换或清除*/
int elps_sess_lock(struct elps_sess *psess, int is_creat);/* lockout */

int elps_msg_rcv_get_bulk(struct ipc_msghdr_n *pmsghdr, void *pdata);
void elps_vlan_map_set(unsigned char *bitmap, uint16_t vlan_start,uint16_t vlan_end);
void elps_vlan_map_unset(unsigned char *bitmap, uint16_t vlan_start,uint16_t vlan_end);
int elps_vlan_map_isset(unsigned char *bitmap, uint16_t vlan_start,uint16_t vlan_end);
int elps_get_bulk(struct hhrelps_snmp *elps_buf, uint32_t session_id, uint32_t elps_max);


#endif

