#ifndef HIOS_ERPS_H
#define HIOS_ERPS_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/hptimer.h>

#include "l2/l2_if.h"
#include "l2/cfm/cfm_session.h"


#define ERPS_NUM  255
#define VLAN_MEAP_MAX 512
#define ERPS_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_ERPS, fmt, ##__VA_ARGS__ ); \
    } while(0)

enum SESSION_STATUS
{
    SESSION_STATUS_DISABLE = 0,
    SESSION_STATUS_ENABLE
};


enum ERPS_ROLE
{
    ERPS_ROLE_INVALID = 0,
    ERPS_ROLE_NONOWNER,
    ERPS_ROLE_OWNER,
    ERPS_ROLE_NEIGHBOUR
};

enum ERPS_STATUS
{
    ERPS_STATE_INIT,
    ERPS_STATE_IDLE,
    ERPS_STATE_PROTECTION,
    ERPS_STATE_MS,
    ERPS_STATE_FS,
    ERPS_STATE_PENDING,
    ERPS_STATE_PROTECTION_REMOTE,
    ERPS_INVALID_STATE = 10,
};
enum ERPS_RAPS_STATUS
{
    ERPS_NO_REQUEST = 0,
    ERPS_MANUAL_SWITCH = 7,
    ERPS_SIGNAL_FAIL = 11,
    ERPS_FORCED_SWITCH = 13,
    ERPS_EVENT = 14,
};

enum ERPS_EVENT_C
{
    ERPS_INVALID_EVENT = 0,
    ERPS_EVENT_ADMIN_CLEAR,
    ERPS_EVENT_ADMIN_FS,
    ERPS_EVENT_RAPS_FS,
    ERPS_EVENT_LOCAL_SF,
    ERPS_EVENT_LOCAL_CLEAR_SF,
    ERPS_EVENT_RAPS_SF,
    ERPS_EVENT_RAPS_MS,
    ERPS_EVENT_ADMIN_MS,
    ERPS_EVENT_WTR_RUN,
    ERPS_EVENT_WTR_EXP,
    ERPS_EVENT_WTB_RUN,
    ERPS_EVENT_WTB_EXP,
    ERPS_EVENT_RAPS_NR_RB,
    ERPS_EVENT_RAPS_NR,
    ERPS_MAX_EVENTS
};

enum ERPS_PORT_EVENT
{
    ERPS_PORT_DOWN,
    ERPS_PORT_UP,
    ERPS_CC_DOWN,
    ERPS_CC_UP,
    ERPS_CC_DELETE,
    ERPS_PORT_EVENT_MAX
};

/* erps 全局数据结构 */
struct erps_global
{
    uint32_t        pkt_recv;           /* 接收报文统计 */
    uint32_t        pkt_send;        /* 发送报文统计 */
    uint32_t        pkt_err;            /* 错误报文统计 */
    uint16_t        sess_total;         /* session 总数*/
    uint16_t        sess_enable;        /* enable 的 session 数 */
    uint8_t     debug_packet;   /*debug 调试开关*/
} __attribute__((packed));


/* erps 会话的基本数据结构 */
struct erps_info
{
    uint16_t        sess_id;
    uint16_t        pvlan;       /* primariy vlan */
    /*one bit which is vlan id map*/
    uint8_t         data_vlan_map[VLAN_MEAP_MAX];    /*value is 4096=512*8*/
	uint8_t         dataVlanMapIsSet;
	uint8_t         sendMsgRapsNRRB;
    uint32_t        east_interface; /* 东向端口 ifindex */
    uint8_t         east_interface_flag;/*east interface status flag 0 is up 1 is down*/
	uint8_t         eastInterfaceMac[6];
    uint32_t        west_interface; /* 西向端口 ifindex */
    uint8_t         west_interface_flag;/*west interface status flag 0 is up 1 is down*/
	uint8_t         westInterfaceMac[6];
    uint32_t        rpl_interface;     /*rpl正常情况下block 端口 ifindex */
    uint32_t        block_interface;   /*block 端口 ifindex */
    uint32_t        event_block;   /*admin/falied  block 端口 ifindex */
    uint16_t        guardtimer;   /* guard-timer 时间 */
    uint16_t        holdoff;     /* holdoff 时间 */
    uint16_t        wtr;         /* wtr 时间 */
    uint16_t        wtb;         /* wtb 时间 */
    uint16_t        ring_id;/*环ID*/
    uint16_t        sub_ring;    /* 0:master ring 1:subring */
    uint16_t        attach_sess_id;
    uint16_t        east_cfm_session_id;/*east 端口上绑定的cfm session */
    uint16_t        west_cfm_session_id;/*west 端口上绑定的cfm session*/
    uint8_t         level;/*保护组level*/
    uint8_t         priority;    /* 报文的优先级，默认是 6 */
    uint16_t        keepalive;
    enum ERPS_ROLE role; /* session在erps保护组中的角色*/
    enum ERPS_STATUS current_status; /* 保护组当前本地状态 */
    enum ERPS_EVENT_C current_event;  /* 保护组当前事件(本地触发或者APS发送) */
    enum SESSION_STATUS status;      /* ERPS SESSION 状态*/
    enum FAILBACK_E failback;    /* 保护是否回切 */
    uint8_t          node_id[MAC_LEN];
} __attribute__((packed));


/* raps 报文格式 */
struct raps_pkt
{

#if BIG_ENDIAN_ON
    uint8_t level: 3,
            version: 5;
    uint8_t opcode;
    uint8_t flag;
    uint8_t tlv_offset;
    uint8_t request_state: 4,
            sub_code: 4;
    uint8_t rb: 1,
            dnf: 1,
            bpr: 1,
            status_reserved: 5;
#else
    uint8_t version: 5,
            level: 3;
    uint8_t opcode;
    uint8_t flag;
    uint8_t tlv_offset;
    uint8_t sub_code: 4,
            request_state: 4;
    uint8_t status_reserved: 5,
            bpr: 1,
            dnf: 1,
            rb: 1;
#endif
    uint8_t node_id[MAC_LEN];
    uint8_t  reserved[24];//session_id
    uint8_t end_tlv;
} __attribute__((packed));

/* erps session 数据结构 */
struct erps_sess
{
    struct erps_info   info;                    /* 会话的基本信息 */
    struct raps_pkt    r_aps;
    struct erps_sess   *attach_to_sess;/*绑定的主环信息*/
    struct cfm_sess    *east_cfm_session;/*east 端口上绑定的cfm session */
    struct cfm_sess     *west_cfm_session;/*west 端口上绑定的cfm session*/
    TIMERID     holdoff_timer;   /* holdoff 定时器 */
    TIMERID     wtr_timer;       /* wtr 定时器 */
    TIMERID     wtb_timer;       /* wtr 定时器 */
    TIMERID     guard_timer;     /* guard 定时器 */
    TIMERID     keepalive_timer;/* keepalive 定时器 */
};

extern struct hash_table  erps_session_table; /* erps 会话 hash 表，用 sess_id 作为 key */
extern struct erps_global gerps;              /* erps 全局数据结构 */




void erps_init(void);
void erps_vlan_map_set(unsigned char *bitmap, uint16_t vlan_start, uint16_t vlan_end);
void erps_vlan_map_unset(unsigned char *bitmap, uint16_t vlan_start, uint16_t vlan_end);
int erps_vlan_map_isset(unsigned char *bitmap, uint16_t vlan_start, uint16_t vlan_end);
void erps_sess_table_init(unsigned int size);
struct erps_sess *erps_sess_create(uint16_t sess_id);/* 创建一个 erps session */
int erps_sess_add(struct erps_sess *psess); /* session 添加到 hash 表 */
int erps_sess_delete(uint16_t sess_id) ;/* 删除一个 erps session */
struct erps_sess *erps_sess_lookup(uint16_t sess_id) ;/* 查找一个erps session */
int erps_sess_enable(struct erps_sess *psess);
int erps_sess_disable(struct erps_sess *psess);
int erps_sess_update(struct erps_sess *psess);
int erps_get_bulk(struct erps_info *elps_buf, uint32_t session_id, uint32_t elps_max);
int erps_msg_rcv_get_global(struct ipc_msghdr_n  *pmsghdr, void *pdata);
int erps_msg_rcv_get_bulk(struct ipc_msghdr_n  *pmsghdr, void *pdata);

#endif
