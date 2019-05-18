/*
*   define of trunk
*/

#ifndef HIOS_TRUNK_H
#define HIOS_TRUNK_H


#include <lib/types.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/hptimer.h>
#include "l2_if.h"

#define TRUNK_DISABLE               0               /*不使能*/
#define TRUNK_ENABLE                1               /*使能*/

#define TRUNK_WTR_DEFAULT           30              /*回切时间默认值*/
#define TRUNK_PRIORITY_DEFAULT      32768           /*优先级默认值*/
#define TRUNK_INTERVAL_DEFAULT    1             /*报文发送间隔默认值*/
#define TRUNK_MEMBER_MAX        8               /*负载分担最大成员数*/
#define TRUNK_BACK_MEMBER_MAX   2               /*主备最大成员数*/

/* trunk 信息类型 */
enum TRUNK_INFO
{
    TRUNK_INFO_INVALID = 0,
    TRUNK_INFO_TRUNK,                               /*增删聚合与批量获取trunk信息*/
    TRUNK_INFO_PORT,                                /*增删聚合成员与批量获取trunk成员口信息*/
    TRUNK_INFO_GLOBAL_LOADBALANCE,
    TRUNK_INFO_WORK_MODE,                           /*工作模式*/
    TRUNK_INFO_ECMP_MODE,                           /*负载分担方式*/
    TRUNK_INFO_BACKUP_PORT_SELECT,              /*主备方式端口选择处理*/
    TRUNK_INFO_PORT_SELECT_SET,                 /*成员端口选中状态设置*/
    TRUNK_INFO_MASTER_PORT_SET,                 /*主端口选择设置*/
    TRUNK_INFO_LACP_ENABLE,                      /*lacp协议使能*/
    TRUNK_INFO_GLOABLE_ECMP_MODE
};


/* trunk 工作模式 */
enum TRUNK_MODE
{
    TRUNK_MODE_ECMP = 0,                            /* 负载分担方式*/
    TRUNK_MODE_BACKUP                               /* 主备方式*/
};


/*trunk 回切模式 */
enum TRUNK_SWITCH
{
    TRUNK_SWITCH_NOFAILBACK = 0,                /*不回切*/
    TRUNK_SWITCH_FAILBACK                       /*回切*/
};


/* trunk 负载分担的方式 */
enum TRUNK_ECMP
{
    TRUNK_BALANCE_LABEL = 0,                        /*依据label进行负载分担，默认值*/
    TRUNK_BALANCE_DIP,                              /*依据目的ip进行负载分担*/
    TRUNK_BALANCE_SIP,                              /*依据源ip进行负载分担*/
    TRUNK_BALANCE_SIP_DIP,                          /*依据源ip和目的ip进行负载分担*/
    TRUNK_BALANCE_SMAC,                         /*依据源mac进行负载分担*/
    TRUNK_BALANCE_DMAC,                         /*依据目的mac进行负载分担*/
    TRUNK_BALANCE_SMAC_DMAC,                        /*依据源mac和目的mac进行负载分担*/
    TRUNK_BALANCE_SPORT_DMAC,                       /*依据源port和目的mac进行负载分担*/
    TRUNK_BALANCE_SPORT_DPORT,
    TRUNK_BALANCE_MAX = 16
};

/*LACP事件*/
enum LACP_EVENT
{
    LACP_EVENT_PORT_LINKUP = 0,                         /*接口linkup事件*/
    LACP_EVENT_PORT_LINKDOWN,                           /*接口linkdown事件*/
    LACP_EVENT_RX_TIMEOUT,                                  /*接收超时事件*/
    LACP_EVENT_RX_PKT,                                      /*接收报文事件*/
    LACP_EVENT_ADD_PORT,                                    /*添加端口事件*/
    LACP_EVENT_ENABLE_LACP,                             /*使能lacp协议事件*/
    LACP_EVENT_WORK_MODE                                    /*工作模式改变事件*/
};


/*端口选中状态*/
enum LACP_PORT_SELECTED
{
    LACP_UNSELECTED = 0,                                    /*未选中*/
    LACP_SELECTED                                           /*选中*/
};

/*lacp状态机状态*/
enum LACP_STATE
{
    LACP_INVALID = 0,
    LACP_PORT_DISABLED,
    LACP_RX_EXPIRED,
    LACP_DEFAULT,
    LACP_CURRENT
};

struct lacp_link
{
    enum LACP_STATE  state;                                 /*状态机状态*/
    enum IFNET_STP_STATUS  port_state;                  /*端口状态:0:IFNET_STP_BLOCK 1:IFNET_STP_FORWARD*/
    enum LACP_PORT_SELECTED  selected;                      /*聚合端口是否选中*/
    uint32_t  sync_state;                                   /*同步状态*/
    uint32_t  rx_timer;                                     /*lacp报文接收超时计数*/
    uint32_t  tx_timer;                                     /*lacp报文发送计数*/
    uint32_t  rx_flag;                                      /*lacp接收标志*/
    struct lacp_pkt    *pdu;                                /*接收的lacp协议报文*/
    uint16_t  actor_port;                                   /*Actor的端口号*/
    uint16_t  actor_key;                                    /*Actor的运行key*/
    uint8_t   actor_state;                                  /*Actor的端口状态*/
    uint8_t   partner_admin_state;                          /*Actor认为的默认Partner状态*/
    uint8_t   partner_state;                                /*Actor认为的Partner状态*/
    uint8_t   pad;
    uint8_t   partner_admin_sys[MAC_LEN];                   /*Partner的默认MAC地址*/
    uint8_t   partner_sys[MAC_LEN];                         /*Partner的系统MAC地址*/
    uint16_t  partner_admin_spri;                           /*Partner的默认系统优先级*/
    uint16_t  partner_spri;                             /*Partner管理员配置的系统优化级*/
    uint16_t  partner_admin_key;                            /*Partner默认key*/
    uint16_t  partner_key;                                  /*Partner的运行key*/
    uint16_t  partner_admin_port;                           /*Partner的默认端口号*/
    uint16_t  partner_port;                             /*Partner的端口号*/
    uint16_t  partner_admin_ppri;                           /*Partner的默认端口优先级*/
    uint16_t  partner_ppri;                             /*Partner的端口优先级*/

    uint32_t pkt_tx;                                        /*发送的lacp报文数*/
    uint32_t pkt_rx;                                        /*接收的lacp报文数*/
};


/* trunk 成员端口数据结构 */
struct trunk_port
{
    uint32_t ifindex;                               /*成员端口索引*/
    uint16_t priority;                              /* 端口优先级，默认 32768 */
    uint8_t  passive;                               /* 1: lacp 被动模式开启 */
    uint8_t  linkstatus;                            /*成员口的link状态*/
    uint32_t failback;                              /*回切标志，回切定时期间，不发报文*/
    struct lacp_link link;                          /*lacp协议相关结构体*/
};


/* trunk 数据结构 */
struct trunk
{
    uint32_t           ifindex;                    /*trunk接口索引*/
    uint16_t           trunkid;                     /* trunk 接口的 id */
    uint16_t           pad;
    uint32_t           speed;                       /*trunk速率*/
    uint8_t            down_flag;                   /* 0: linkup, 1: linkdown，ecmp up 成员数为 0 时,或者 backup 成员数小于 2 时，trunk linkdown */
    uint8_t            lacp_enable;                 /* 1: lacp 协议使能， 0: 不使能 */
    uint8_t            lacp_interval;               /* lacp 报文发送间隔，默认 1s */
    uint8_t            upport_num;                  /* up 端口数 */
    enum TRUNK_MODE    work_mode;                   /* 1: 主备方式, 0: 负载分担方式 */
    enum TRUNK_SWITCH  failback;                    /* 0: 不回切, 1: 回切 */
    enum TRUNK_ECMP    ecmp_mode;                   /* 负载分担方式 */
    enum BACKUP_E      status;                      /* 主/备工作状态 */
    struct list        portlist;                    /* trunk 成员链表，存放 struct trunk_port 数据结构 */
    uint16_t           wtr;                         /* 回切时间，默认 30s*/
    uint16_t           priority;                    /* lacp sys 优先级，默认 32768 */
    uint32_t           master_if;                   /* 主端口索引 */
    uint32_t           member[TRUNK_MEMBER_MAX];    /*保存工作端口成员列表*/
    /*struct thread      *fb_timer;*/                   /*回切定时器*/
   TIMERID fb_timer;
   uint8_t  trunk_under_interface_flag;
};

struct trunk_reserve_vlan_t{
	uint16_t trunkid;
	uint16_t reserve_vlan;
	uint16_t reserve_vlan_bak;
	uint8_t  flag;
};

struct trunk_reserve_vlan_t trunk_reserve_vlan_table[3];

struct trunk *trunk_table[IFM_TRUNK_MAX];

struct trunk_config
{
	uint8_t gloable_balance_mode;
};
extern struct trunk_config trunk_gloable_config;
enum gloable_balance_config
{
    GLOABLE_BALANCE_DEFAULT = 0,
    GLOABLE_BALANCE_CONFIG
};
#define  TRUNK_UNDER_INTERFACE_ENABLE  0x01

/*trunkl debug define */
	
#define TRUNK_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_TRUNK, fmt, ##__VA_ARGS__ ); \
    } while(0)


/* 对 trunk 的操作函数 */
void trunk_init(void);                                                              /*trunk初始化*/
void trunk_table_init(void);                                                     /*trunk表初始化*/
struct trunk *trunk_add(uint16_t trunkid);                                       /*trunk创建*/
int trunk_delete(uint16_t trunkid);                                                  /*trunk删除*/
struct trunk *trunk_lookup(uint16_t trunkid);                                    /*trunk查找*/

/* 对主备 trunk 的操作函数 */
int trunk_backup_switch(struct trunk *ptrunk, enum BACKUP_E status);         /* trunk成员主备切换*/
int trunk_linkstatus_set(struct trunk *trunk_entry);                            /*trunk link状态设置*/
int trunk_backup_port_select(struct trunk *trunk_entry);                    /*trunk主备模式主端口设置*/
int trunk_ecmp_port_select(struct trunk *trunk_entry);                      /*trunk负载分担模式工作端口设置*/


/* 对 trunk 成员端口的操作函数 */
int trunk_port_add(uint16_t trunkid, struct trunk_port *pport);              /*trunk成员端口添加*/
int trunk_port_delete(uint16_t trunkid, uint32_t ifindex);                   /*trunk成员端口删除*/
int trunk_port_up(uint16_t trunkid, uint32_t ifindex);                           /* trunk 成员端口 up */
int trunk_port_down(uint16_t trunkid, uint32_t ifindex);                     /* trunk 成员端口 down */
struct trunk_port *trunk_port_lookup(uint16_t trunkid, uint32_t ifindex);   /*trunk成员端口查找*/
struct trunk_port *trunk_port_init(uint16_t trunkid, uint32_t ifindex);             /*trunk成员端口信息初始化*/

/*trunk_cmd.c下相关函数*/
void trunk_cmd_init(void);                                                      /*trunk命令行初始化*/
int trunk_config_write(struct vty *vty);                                        /*trunk配置保存*/
void trunk_config_show(struct vty *vty, struct trunk *trunk_entry);         /*trunk配置信息显示*/
void trunk_info_show(struct vty *vty, struct trunk *trunk_entry);               /*trunk信息显示*/
void trunk_member_alarm_report(uint16_t trunkid, uint32_t ifindex);     /*上报/清除告警*/
int trunk_info_bulk_get(uint16_t trunkid, struct trunk pif[]);              /*查找传入 trunkid 之后对应的 n 个数据 */
int trunk_port_info_bulk_get(uint16_t trunkid, struct trunk_port tport[]);   /*查找传入 trunk中所有成员端口 */

int trunk_speed_set(uint16_t trunkid);    /*trunk接口速率设置*/
int trunk_config_write_all(struct vty *vty);

int trunk_comm_send_msg_wait_ack(uint16_t msg_subtype, uint8_t opcode,uint32_t length, void *pdata);

#endif
