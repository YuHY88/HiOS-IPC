#ifndef HIOS_OAM_COMMON_H
#define HIOS_OAM_COMMON_H

/*
*   common define of oam
*/

#include <lib/types.h>
#include <lib/hptimer.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>


#define OAM_MEP_MAX  8191   /* MEP 的最大值 8191 */


/* cfm opcode value */
#define CFM_OPCODE_CCM      1
#define CFM_OPCODE_LBR      2
#define CFM_OPCODE_LBM      3
#define CFM_OPCODE_LTR      4
#define CFM_OPCODE_LTM      5
#define CFM_OPCODE_AIS      33
#define CFM_OPCODE_LCK      35
#define CFM_OPCODE_TST      37
#define CFM_OPCODE_APS      39
#define CFM_OPCODE_RAPS     40
#define CFM_OPCODE_LMR      42
#define CFM_OPCODE_LMM      43
#define CFM_OPCODE_LDM      45
#define CFM_OPCODE_DMR      46
#define CFM_OPCODE_DMM      47

#define OAM_DEBUG(format, ...)\
		zlog_debug(MPLS_DBG_OAM, "%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define OAM_ERROR(format, ...)\
		zlog_err("%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define COM_THREAD_TIMER_OFF(x)	\
	do{if(x){\
			high_pre_timer_delete(x); \
			x = 0;\
		}\
	}while(0)
	
#define COM_THREAD_TIMER_ADD(f, h, w, x, y) high_pre_timer_add(f, h, w, x, y)

/* IPC 的 subtype */
enum OAM_SUBTYPE_E
{
	OAM_SUBTYPE_INVALID = 0,
	OAM_SUBTYPE_SESSION,
	OAM_SUBTYPE_RMEP,
	OAM_SUBTYPE_DIRECTION,
	OAM_SUBTYPE_PRIORITY,
	OAM_SUBTYPE_CC_INTERVAL,
	OAM_SUBTYPE_CC,
	OAM_SUBTYPE_AIS,
	OAM_SUBTYPE_LOCK,
	OAM_SUBTYPE_LM,
	OAM_SUBTYPE_DM,
	OAM_SUBTYPE_ALARM,
	OAM_SUBTYPE_MD,
	OAM_SUBTYPE_MA,	
	OAM_SUBTYPE_ELPS_SESSION,
	OAM_SUBTYPE_ELPS_STG,
	OAM_SUBTYPE_ERPS_BLOCK_EAST,
	OAM_SUBTYPE_ERPS_BLOCK_WEST,
	OAM_SUBTYPE_ERPS_FOWARD_EAST,
	OAM_SUBTYPE_ERPS_FOWARD_WEST,
	OAM_SUBTYPE_IFNAME,
	OAM_SUBTYPE_MEG,
	OAM_SUBTYPE_MAX = 25
};


/* oam 协议类型 */
enum OAM_PROTO
{
	OAM_PROTO_INVALID = 0,
	OAM_PROTO_BFD,         /* BFD */	
	OAM_PROTO_TPOAM,       /* mpls-tp oam */
	OAM_PROTO_G8131,       /* mpls-tp aps */	
	OAM_PROTO_G8031,       /* eth line aps */
	OAM_PROTO_G8032,       /* eth ring aps */
	OAM_PROTO_CFM,	       /* 802.1ag */
	OAM_PROTO_Y1731,       /* y.1731 */	
	OAM_PROTO_EFM,         /* 802.3ah */
	OAM_PROTO_MAX = 8
};


/* OAM 类型 */
enum OAM_TYPE
{
	OAM_TYPE_INVALID = 0,	
	OAM_TYPE_INTERFACE,	 /* oam for 接口 */	
	OAM_TYPE_IP,	     /* oam for ip */
	OAM_TYPE_PW,	     /* oam for pw */
	OAM_TYPE_LSP,	     /* oam for lsp */	
	OAM_TYPE_TUNNEL,	 /* oam for tunnel */
	OAM_TYPE_MAX = 8
};


/* oam 会话状态 */
enum OAM_STATUS
{
	OAM_STATUS_DISABLE = 0,   /* 未使能状态 */	
	OAM_STATUS_ENABLE,        /* 使能状态 */	
	OAM_STATUS_DOWN,          /* down 状态 */
	OAM_STATUS_UP, 	          /* UP 状态 */
	OAM_STATUS_STOP,		  /* 终止 状态 */
	OAM_STATUS_FINISH 	      /* 完成 状态 */
};


/* oam 事件 */
enum OAM_EVENT
{
	OAM_EVENT_INVALID = 0,         
	OAM_EVENT_RECV_DOWN,           /* 收到 down 事件 */
	OAM_EVENT_RECV_UP,	           /* 收到 UP 事件 */
	OAM_EVENT_TIMEOUT,			   /* 等待超时 */
	OAM_EVENT_MAX = 8	
};


/* oam 报文 cc 间隔 */
enum OAM_INTERVAL
{
	OAM_INTERVAL_0 = 0,
	OAM_INTERVAL_3,               /* 3.3 ms */
	OAM_INTERVAL_10,              /* 10 ms */
	OAM_INTERVAL_100,             /* 100 ms */
	OAM_INTERVAL_1000,            /* 1 s */
	OAM_INTERVAL_10000,           /* 10 s */
	OAM_INTERVAL_30,              /* 30 ms */
	OAM_INTERVAL_300,             /* 300 ms */
};


/* mep 方向 */
enum MEP_DIRECT
{
	MEP_DIRECT_UP = 0, /* up mep ,inward*/
	MEP_DIRECT_DOWN    /* down mep ,outward*/
};

/* MEP or MIP */
enum MP_TYPE
{
	MP_TYPE_INVALID = 0,
	MP_TYPE_MEP,
	MP_TYPE_MIP
};



/* OAM 告警类型 */
enum OAM_ALARM
{
	OAM_ALARM_INVALID = 0,
	OAM_ALARM_MEG,              /* meg 错误 */
	OAM_ALARM_MEP,              /* mep 错误 */
	OAM_ALARM_LEVEL,            /* level 错误 */
	OAM_ALARM_INTERVAL,         /* 发送间隔错误 */
	OAM_ALARM_RDI,              /* RDI */
	OAM_ALARM_LOC,              /* LOC */
	OAM_ALARM_AIS,              /* AIS*/
	OAM_ALARM_LCK,              /* LCK*/
	OAM_ALARM_MA,				/* MA */
	OAM_ALARM_ERROR,					
	OAM_ALARM_PORT_DOWN,	    /* DOWN */
	OAM_ALARM_PORT_BLOCK,		/* BLOCK */	
	OAM_ALARM_CSF,
	OAM_ALARM_ALL,		        /* used to delete all alarms by hal */ 
};

#if 0

/*802.1 alarm type*/
enum CFM_DEFECT_TYPE
{
  CFM_ALARM_INVALID = 0,
  CFM_ALARM_RDI,
  CFM_ALARM_PORT_STATUS,
  CFM_ALARM_INTERFACE_STATUS,
  CFM_ALARM_REMOTE_CCM,
  CFM_ALARM_INVALID_CCM,
  CFM_ALARM_XCON_CCM   
};

/* cfm 告警数据结构 */
struct cfm_alarm_t
{
	uint8_t alarm_rdi;    /*rdi bit is set in flag*/
	uint8_t alarm_port;    /* port tlv ,not psUp*/
	uint8_t alarm_interface;  /*interface tlv,not isUp*/
	uint8_t alarm_ncc;     /*not receive any ccm*/
	uint8_t alarm_icc;    /*mep not map*/
	uint8_t alarm_xcon;    /*maid not map*/
};

#endif

enum CFM_OPCODE
{
	CFM_CONTINUITY_CHECK_OPCODE = 1,
	CFM_LOOPBACK_REPLY_OPCODE,
	CFM_LOOPBACK_MESSAGE_OPCODE,
	CFM_LINKTRACE_REPLY_OPCODE,
	CFM_LINKTRACE_MESSAGE_OPCODE,
	CFM_AIS_MESSAGE_OPCODE = 33,
	CFM_LCK_MESSAGE_OPCODE = 35,	
	CFM_TEST_MESSAGE_OPCODE = 37,	
	CFM_APS_OPCODE = 39,
	CFM_RAPS_OPCODE = 40,
	CFM_OPCODE_CSF,
};

/* oam 告警数据结构 */
struct oam_alarm_t
{
	uint8_t alarm_meg;    /* meg 错误 */
	uint8_t alarm_mep;    /* mep 错误 ;in cfm:mepid or period error,means invalid ccm*/
	uint8_t alarm_level;  /* level 错误 */
	uint8_t alarm_cc;     /* cc 发送间隔错误 */
	uint8_t alarm_rdi;    /* RDI 告警 */
	uint8_t alarm_loc;    /* LOC 告警 */
	uint8_t alarm_lck;    /* lock 告警 */	
	uint8_t alarm_ais;    /* ais 告警 */
	uint8_t alarm_csf;
	uint8_t alarm_ma;    /* in cfm:maid or level error,means xcon*/
	uint8_t alarm_error;    
	uint8_t alarm_port_down;    /* in cfm:up mep,when uni port is down */	
	uint8_t alarm_port_block;    /* in cfm:down mep,when  port is block */	
	
};

struct oam_alarm_count
{
	uint32_t unl_pkt_count;
	uint32_t mmg_pkt_count;
	uint32_t unm_pkt_count;
	uint32_t unp_pkt_count;
	uint32_t loc_pkt_count;
	uint32_t rdi_pkt_count;
	uint32_t unl_period;
	uint32_t mmg_period;
	uint32_t unm_period;
	uint32_t unp_period;
	uint32_t loc_period;
	uint32_t rdi_period;
	uint32_t unl_period_count;
	uint32_t mmg_period_count;
	uint32_t unm_period_count;
	uint32_t unp_period_count;
	uint32_t loc_period_count;
	uint32_t rdi_period_count;
};



/* sla 测量的结果 */
struct sla_measure
{
	uint16_t        sess_id;   /* session 索引: 1-1024 */
	uint16_t        ma_index;    /* 所属的 ma */
	uint16_t        local_mep;   /* local mep 索引: 1-65535 */
	uint32_t        count;	        /* 测试次数 */
    enum OAM_STATUS state;          /* sla 测量的状态, 0为传送最后一组结果,1为running*/
	uint32_t        lm_max;	        /* 本端最大丢包率 */
	uint32_t        lm_mean;        /* 本端平均丢包率 */
	uint32_t        lm_min;	        /* 本端最小丢包率 */	
	uint32_t        lm_max_peer;	/* 对端最大丢包率 */
	uint32_t        lm_mean_peer;   /* 对端平均丢包率 */
	uint32_t        lm_min_peer;	/* 对端最小丢包率 */
	uint32_t        dm_max;	        /* 最大延时 */
	uint32_t        dm_mean;        /* 平均延时 */
	uint32_t        dm_min;	        /* 最小延时 */
	uint32_t        jitter_max;	    /* 最大抖动 */
	uint32_t        jitter_mean;    /* 平均抖动 */
	uint32_t        jitter_min;	    /* 最小抖动 */	
	uint32_t        throughput;	    /* 吞吐量 */
};


/* cfm base pdu */
struct cfm_pdu_header
{
#if BIG_ENDIAN_ON      /* CPU 大端序 */
	uint8_t  level:3,			/* md level 0-7 */
			 version:5; 		/* value = 0 */
#else    /* CPU 小端序 */
	uint8_t  version:5, 		/* value = 0 */
			 level:3;			/* md level 0-7 */
#endif
	uint8_t  opcode;
	uint8_t  flag;              /* bit0-3: cc interval, bit7: rdi */
	uint8_t  first_tlv_offset;  /* first tlv offset */	
};

struct cfm_lm_pdu
{
	struct cfm_pdu_header cfm_header;
	uint32_t txfcf;
	uint32_t rxfcf;
	uint32_t txfcb;
	uint8_t  end_tlv;
}__attribute__((packed));


/* cfm ccm packet */
struct cfm_ccm_pkt
{
	struct cfm_pdu_header  pdu;	
	uint32_t seq;               /* sequence number */
	uint16_t rmep;              /* remote mep id 1 - 8191 */
	uint8_t  ma_format;         /* ma format: 0 - 255, we use 32 */
	uint8_t  ma_len;            /* ma name len: 1 - 43, we use 4 */
	uint32_t ma_name;           /* ma name */
};


int msg_sender_to_other_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
		enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

#endif


