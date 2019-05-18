/******************************************************************************
 * Filename: synce.h
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: functions for syncE protocal and clock source header file.
 *
 * History:
 * 2016.10.13  cuiyudong created
 *
******************************************************************************/


#ifndef HIOS_SYNCE_H
#define HIOS_SYNCE_H

#include "lib/msg_ipc.h"

#define SYNCE_CLK_NUM   8   /* synce 时钟源的数量 */
#define SYNCE_ENABLE    1
#define SYNCE_DISABLE   0
#define SYNCE_DPLL_IC_NUM               8
#define SYNCE_SEND_MSG_INTERVAL         4
#define SYNCE_SELECT_SOURCE_INTERVAL    7
#define SYNCE_TIMER_RUN_INTERVAL        1

#define SYNCE_TIMER_FORCEFREE_DELAY     6
#define SYNCE_TIMER_EVENT_CHAGE_DELAY   5
#define SYNCE_TIMER_STATUS_SCAN         3

#define SYNCE_CONFIG_HOLDTIME_DEFAULT       259200
#define SYNCE_CONFIG_MODE_DEFAULT           0
#define SYNCE_CONFIG_SSM_MODE_DEFAULT       1
#define SYNCE_CONFIG_WTR_DEFAULT            5
#define SYNCE_CONFIG_PREEMPT_DEFAULT        1
#define SYNCE_CONFIG_SSM_INTERVAL_DEFAULT   1
#define SYNCE_CONFIG_SSM_TIMEOUT_DEFAULT    5
#define SYNCE_CONFIG_SSM_ENALBE_DEFAULT     0

/********syncE alarm define ************************************************/
#define SYNCE_ALARM_EVENT_REPORT        1

#define SYNCE_ALARM_EVENT_RECOVER       2

#define SYNCE_ALARM_FLAG_CLK_INVALID    0X01  /*输入信号失效 */

#define SYNCE_ALARM_FLAG_FREQ_OVERFLOW  0X02  /*输入偏移告警 */

#define SYNCE_ALARM_FLAG_LOSELOCK       0X04  /*同步失效*/

#define SYNCE_ALARM_FLAG_QLLOW          0X08  /*质量等级低告警*/

#define SYNCE_ALARM_FLAG_SYNC_CHANAGE   0X10  /*倒换事件*/

#define SYNCE_ALARM_FLAG_QL_SWITCH      0X20  /*质量等级改变告警*/

#define SYNCE_ALARM_FLAG_MASK           0X3F  /*告警掩码*/

/* synce 的配置信息类型 */
enum SYNCE_INFO
{
    SYNCE_INFO_INVALID = 0,
    SYNCE_INFO_MODE,
    SYNCE_INFO_SSM_ENABLE,
    SYNCE_INFO_SELECT,
    SYNCE_INFO_PREEMPT,
    SYNCE_INFO_ADD,
    SYNCE_INFO_DELETE,
    SYNCE_INFO_PRIORITY,
    SYNCE_INFO_FORCE_STATE,
    SYNCE_INFO_GET_GLOBAL,
    SYNCE_INFO_GET_CLOCK_STATE,
    SYNCE_INFO_SET_FREQ_OFFSET,
    SYNCE_INFO_MAX = 16
};


/* 时钟源的选择模式 */
enum SYNCE_MODE
{
    SYNCE_MODE_AUTO = 0,  /* 自动选择模式 */
    SYNCE_MODE_FORCE,     /* 强制模式 */
    SYNCE_MODE_MANUAL,    /* 手动模式 */
    SYNCE_MODE_UNKNOWN,
};


/* 时钟源的 SSM 模式 */
enum SSM_MODE
{
    SSM_MODE_ENABLE = 0,   /* ssm enable */
    SSM_MODE_DISABLE,      /* ssm disable */
    SSM_MODE_STANDARD,     /* ssm standard mode */
    SSM_MODE_EXTEND,       /* ssm extend mode */
};

/* 时钟源类型 */
typedef enum SYNCE_CLK_TYPE
{
    SYNCE_CLK_TYPE_INVALID = 0,
    SYNCE_CLK_TYPE_INTERNAL,       /* 系统内部时钟 */
    SYNCE_CLK_TYPE_CLOCKIF,        /* 外时钟接口输入的时钟 */
    SYNCE_CLK_TYPE_SYNCE,           /* synce 传递的时钟 */
    SYNCE_CLK_TYPE_E1,
    SYNCE_CLK_TYPE_STM,
} E_SYNCE_CLK_TYPE;
typedef enum SYNCE_STAT
{
    SYNCE_RUN_STATE_AUTO = 0,
    SYNCE_RUN_STATE_FREERUN,
    SYNCE_RUN_STATE_HOLDOVER,
    SYNCE_RUN_STATE_LOCKED,
    SYNCE_RUN_STATE_LOSELOCK,            /*未使用*/
} E_SYNCE_STAT;
typedef enum SYNCE_SEL_CLK_EVENT
{
    SYNCE_EVENT_SEL_SSM_PKT_EVENT,      //收到SSM EVENT报文
    SYNCE_EVENT_SEL_IF_DOWN,            //接口DOWN事件，包括以太端口的down，和外时钟口的invalid
    SYNCE_EVENT_SEL_CMD_MANUAL_SELECT,  //收到人工选择模式的时钟源选择
    SYNCE_EVENT_SEL_CMD_FORCE_SELECT,   //收到强制模式下的时钟源选择
    SYNCE_EVENT_SEL_CMD_AUTO_SELECT,    //自动模式下，DPLL选择时钟源会触发该事件
    SYNCE_EVENT_SEL_PORT_ADD,           //新的时钟源加入
    SYNCE_EVENT_SEL_PORT_DELETE,         //时钟源删除操作
} e_synce_sel_clk_event;
/* synce 时钟源的数据结构 */
struct synce_clk_t
{
    E_SYNCE_CLK_TYPE    type;    /* 时钟源的类型 */
    int                 status;  /* 时钟源锁定状态 */
    E_VALID_STAT        valid_stat; /*时钟源有效状态*/
    uint32_t            ifindex;      /* 接口的 ifindex */
    unsigned char       synce_alarm_flag; /* 接口的相应告警位 */
};
struct synce_snmp_clock_src_t
{
    int         status;  /* 时钟源锁定状态 */
    int         valid_stat; /*时钟源有效状态*/
    uint32_t    ifindex;      /* 接口的 ifindex */
    uint8_t     priority;   /*优先级*/
    uint8_t     quality_level; /*质量等级*/
};

enum SYNCE_SSM_MODE
{
    SYNCE_SSM_DISABLE = 0,
    SYNCE_SSM_ENABLE
};

typedef struct
{
    /*app层数据下发更新*/
    unsigned char       port_num;  // 物理接口 端口号
    E_SYNCE_CLK_TYPE    if_type;   // 接口类型 分为 时钟口 和 以太口
    unsigned int        if_index;  // 接口索引号 或者时钟接口号
    unsigned char       ic_clk_enable;       /* 该 IC 是否使能 1 enalbe 0 disable*/
    unsigned char       ic_priority;
    /***************************************************/
    /*hal层逻辑判断得出或者从DPLL芯片读取得到*/
    unsigned char       ic_num;  // IC 号
    unsigned char       ic_clk_en;  //芯片读出的IC是否使能标志
    E_VALID_STAT        ic_clkstat;       /*使能后的IC 状态 */
    unsigned char       ic_softalm;       /*使能后的IC 告警 1 soft alarm , 0 no alarm*/
} SynceClkICData;
typedef struct
{
    struct synce_clk_t  cur_clk_source;
    unsigned char       cur_clk_ic_num;
    unsigned char       cur_force_clk_ic_num;
    unsigned char       cur_extern_clk_recv_ql;
    unsigned char       cur_clock_state;
    SynceClkICData      cur_ic_info[SYNCE_DPLL_IC_NUM + 1]; /*将使能的IC信息添加到该数组中*/
} SynceHalGlobalData;

/* synce 的全局数据结构 */
struct synce_t
{
    enum SYNCE_MODE     mode;
    enum SSM_MODE       ssm_mode;     /* ssm 模式 */
    enum SYNCE_STAT     cur_clk_stat; /*当前时钟状态*/
    uint32_t            clk_select;   /* 当前选中的时钟源  */
    uint32_t            clk_manul;    /* manual 指定的时钟源  */
    uint32_t            clk_force;    /* 强制指定的时钟源  */
    uint32_t            hold_time;    /* 时钟保持时间 */
    uint16_t            wtr;          /* 时钟回切的等待时间, 默认5S回切 */
    uint16_t            ssm_interval; /* ssm 报文发送间隔，默认 1s */
    uint16_t            ssm_timeout;  /* ssm 报文超时时间, 默认 3 个报文间隔 */
    uint8_t             cur_clk_ssm_ql; /*当前选中的时钟源的QL值*/
    uint8_t             uc_send_ql;     /*当前要发送的QL值*/
    uint8_t             preempt;       /* 1: 抢占使能，0: 不抢占 */
    uint8_t             ssm_enable;   /* 1: 使能，0: 不使能 */
    int                 iSendFlag;      /* 发送事件报文标志  1 : 发送事件报文  0 : 发送消息报文*/
    int                 iStatScanTime; /*扫描周期*/
    int                 iFlgClrTimer;            /*发送报文的FLAG值*/
    int                 iHoldOverTimer;          /*保持超时定时器*/
    int                 iWaitTimer;             /*恢复等待定时器*/
    int                 iFreqOffset;            /* DPLL frequence offset set */
    int                 iFreeClrTimer;           /*强制FREE状态清除定时器*/
    struct list         clk_list;     /* 时钟源链表，保存所有的时钟源 struct synce_clk_t */
};


struct synce_t g_synce;     /* synce 全局数据结构 */
SynceHalGlobalData g_synce_hal;

void synce_init(void);

int synce_set_info(void *value, enum SYNCE_INFO type); /* 配置 synce_t 的内容 */
void *synce_get_info(enum SYNCE_INFO type);  /* 获取底层硬件信息 */

int synce_select_clock(enum SYNCE_SEL_CLK_EVENT entype);  /* 选择时钟 */
int synce_add_clock(struct synce_clk_t *pclk);      /* 添加一个时钟源 */
int synce_delete_clock(struct synce_clk_t *pclk);  /* 删除一个时钟源 */
struct synce_clk_t *synce_lookup_clk(uint32_t ifindex); /*通过ifindex 查找链表，如果有，则返回该节点指针*/
enum CLOCK_STAT synce_get_lock_status(uint32_t ifindex);
enum VALID_STAT synce_get_valid_status(uint32_t ifindex);
enum SSM_STATUS synce_get_ssm_status(uint32_t ifindex);
uint8_t synce_get_priority(uint32_t ifindex);
uint8_t synce_get_quality_level(uint32_t ifindex);
unsigned char synce_get_soft_alarm_stat(uint32_t ifindex);
E_SYNCE_CLK_TYPE synce_get_clk_type(uint32_t ifindex);

enum SSM_STATUS synce_get_ssm_status(uint32_t ifindex);
enum VALID_STAT synce_get_clk_stat(uint32_t ifindex, enum SYNCE_CLK_TYPE clk_type);
int synce_set_clock_run_state(enum SYNCE_STAT state);
int synce_clock_update_if_status(void);
unsigned int synce_snmp_global_cfg_get(struct ipc_msghdr_n *phdr);
unsigned int synce_snmp_clk_src_next_ifindex_get(struct ipc_msghdr_n *phdr);

unsigned int synce_snmp_clk_src_info_get(struct ipc_msghdr_n *phdr);
int synce_timer_start(void *thread);
int synce_ipc_send_and_wait_ack(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);


#endif
