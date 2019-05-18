/**
 * @file      : route_com.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月27日 17:13:59
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_ROUTE_COM_H
#define HIOS_ROUTE_COM_H

#include <lib/inet_ip.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/errcode.h>

#include "fifo.h"
/* add for  route debug level */
#define ROUTE_DEBUG_RIB_EVENT                   0
#define ROUTE_DEBUG_ROUTE_UPDATE                1
#define ROUTE_DEBUG_NOTIFY_MSG_SEND             2
#define ROUTE_DEBUG_RUNNING_ERROR               3

/* added for ipc share memory */
#define ROUTE_TIMER_DEL(timerid)         if(timerid)\
                                        {\
                                            high_pre_timer_delete(timerid);\
                                            timerid = 0;\
                                        }

#define ROUTE_TIMER_ADD(pFunc, pArg, Sec)      high_pre_timer_add("RrouteTimer", LIB_TIMER_TYPE_NOLOOP, pFunc, pArg, (Sec * 1000))

extern int route_ipc_msg_send(void *pdata, int data_len, int data_num, int module_dst, int module_src, int ipc_type, uint16_t sub_opcode, uint8_t opcode, int msg_index);


#define ROUTE_IPC_SENDTO_FTM(pdata, data_len, data_num, module_dst, module_src, ipc_type, sub_opcode, opcode, msg_index)   route_ipc_msg_send(pdata, data_len, data_num, module_dst, module_src, ipc_type, sub_opcode, opcode, msg_index)

#define ROUTE_IPC_SENDTO_HAL(pdata, data_len, data_num, module_dst, module_src, ipc_type, sub_opcode, opcode, msg_index)   route_ipc_msg_send(pdata, data_len, data_num, module_dst, module_src, ipc_type, sub_opcode, opcode, msg_index)

#define L3VPN_SIZE         1024      // l3vpn 最大数量
#define NHP_NUM_MAX        (4096*4)  // nhp 最大数量
#define ECMP_NUM_MAX       1000      // ecmp 最大数量
#define NHP_ECMP_NUM       2         // 负载分担最大数量
#define ROUTE_PROTO_NUM    17        // 路由协议最大数量
#define ROUTE_COST         1         // 路由默认开销值

#define ROUTE_NUM_MAX      1024*20              // IPV4 active 路由最大数量
#define ROUTEV6_NUM_MAX    1024*20              // IPV6 active 路由最大数量
#define RIB_NUM_MAX        ROUTE_NUM_MAX*3      // IPV4 rib 最大数量, 是 active route 的 3 倍
#define RIBV6_NUM_MAX      ROUTEV6_NUM_MAX*3    // IPV6 rib 最大数量, 是 active routev6 的 3 倍

#define ARP_TOTAL_NUM      4096      // ARP 总数，包括动态和静态 ARP
#define ARP_STATIC_NUM     1024      // 静态 ARP 数量
#define ND_TOTAL_NUM       1024      // ipv6 nd 总数，包括动态和静态 ND
#define ND_STATIC_NUM      256       // 静态 NP 数量
#define SLOT_MAX           8        // max slot number

/* 各种路由的默认 metric 值 */
#define ROUTE_METRIC_CONNECT   0     // 直连路由
#define ROUTE_METRIC_STATIC    10    // 静态路由
#define ROUTE_METRIC_SARP      1     // 静态 ARP
#define ROUTE_METRIC_ARP       2     // 动态 ARP
#define ROUTE_METRIC_MPLSTP    3     // mpls-tp
#define ROUTE_METRIC_RSVPTE    4     // rsvp-te
#define ROUTE_METRIC_LDP       5     // ldp
#define ROUTE_METRIC_SEIGRP    15    // EIGRP summery
#define ROUTE_METRIC_EBGP      20
#define ROUTE_METRIC_IEIGRP    90    // internal EIGRP
#define ROUTE_METRIC_IGRP      100
#define ROUTE_METRIC_OSPF      110
#define ROUTE_METRIC_ISIS      115
#define ROUTE_METRIC_RIP       120
#define ROUTE_METRIC_EGP       140
#define ROUTE_METRIC_ODR       160
#define ROUTE_METRIC_EEIGRP    170    // external EIGRP
#define ROUTE_METRIC_IBGP      200
#define ROUTE_METRIC_UNKNOW    255

/* 通过 nhp 判断路由是否为黑洞路由 */
#define ROUTE_IS_BLACKHOLE(nhp) \
    (((nhp)->nexthop.addr.ipv4 == 0) && ((nhp)->action == NHP_ACTION_DROP))

/* 通过协议类型判断路由是否为 lsp 路由 */
#define ROUTE_PROTO_IS_LSP(nhp) \
    ((ROUTE_PROTO_MPLSTP == (nhp)->protocol) || \
    (ROUTE_PROTO_LDP == (nhp)->protocol) ||     \
    (ROUTE_PROTO_RSVPTE == (nhp)->protocol))

/* 检测 ip 地址是否存在 */
#define ROUTE_IPV4_ADDR_EXIST(ip)   ((ip)!=0)
#define ROUTE_IPV6_ADDR_EXIST(ip)   (((ip[0])!=0) || ((ip[1])!=0) || ((ip[2])!=0) || ((ip[3])!=0) \
                                    || ((ip[4])!=0) || ((ip[5])!=0) || ((ip[6])!=0) || ((ip[7])!=0) \
                                    || ((ip[8])!=0) || ((ip[9])!=0) || ((ip[10])!=0) || ((ip[11])!=0) \
                                    || ((ip[12])!=0) || ((ip[13])!=0) || ((ip[14])!=0) || ((ip[15])!=0))


#define zebra_route_string  route_com_type_to_string    /* 待删除 */
#define proto_redistnum  route_com_string_to_type       /* 待删除 */


/* 获取路由信息子类型 */
enum ROUTE_GET_SUBTYPE
{
    ROUTE_GET_INVALID = 0,
    ROUTE_GET_ROUTER_ID,
    ROUTE_GET_ROUTERV6_ID,
    ROUTE_GET_RIB_ROUTE,
    ROUTE_GET_ACTIVE_ROUTE,
    ROUTE_GET_ACTIVE_ROUTE_BRIEF,
    ROUTE_GET_STATIC_ROUTE_BULK,
    ROUTE_GET_RIB_COUNT,
    ROUTE_GET_ROUTE_GCONF,
    ROUTE_GET_MAX = 9,
};


/* 路由类型 */
enum ROUTE_PROTO
{
	ROUTE_PROTO_INVALID = 0,
    ROUTE_PROTO_STATIC,     // 静态路由
    ROUTE_PROTO_CONNECT,    // 接口路由
    ROUTE_PROTO_ARP,        // ARP 产生的路由
    ROUTE_PROTO_OSPF,       // OSPF 协议路由
    ROUTE_PROTO_OSPF6,      // OSPF 协议路由
    ROUTE_PROTO_ISIS,       // ISIS 协议路由
    ROUTE_PROTO_ISIS6,      // ISIS 协议路由
    ROUTE_PROTO_RIP,        // RIP 协议路由
    ROUTE_PROTO_RIPNG,      // RIPng 协议路由
    ROUTE_PROTO_IBGP,       // BGP 协议路由
    ROUTE_PROTO_EBGP,       // BGP 协议路由
	ROUTE_PROTO_IBGP6,		// BGP4+ 协议路由
	ROUTE_PROTO_EBGP6,		// BGP4+ 协议路由
    ROUTE_PROTO_MPLSTP,     // mpls tp
    ROUTE_PROTO_RSVPTE,     // rsvp te
    ROUTE_PROTO_LDP,        // ldp
    ROUTE_PROTO_MAX = ROUTE_PROTO_NUM
};


/* NHP 类型*/
enum NHP_TYPE
{
    NHP_TYPE_INVALID = 0,
    NHP_TYPE_HOST,   // 主机路由，用于 ARP 生成的主机路由
    NHP_TYPE_IP,     // 非直连路由，包括网段路由和主机路由
    NHP_TYPE_CONNECT,// 接口直连的路由，包括直连网段路由和直连主机路由
    NHP_TYPE_LSP,    // to lsp
    NHP_TYPE_FRR,    // to frr group
    NHP_TYPE_ECMP,   // to ecmp group
    NHP_TYPE_TUNNEL, // to tunnel
    NHP_TYPE_MAX = 100
};


/* ECMP 类型 */
enum ECMP_TYPE
{
    ECMP_TYPE_ECMP = 0,
    ECMP_TYPE_FRR
};


/* nhp 动作类型 */
enum NHP_ACTION
{
    NHP_ACTION_FORWARD = 0,  // 转发
    NHP_ACTION_DROP,         // 丢弃
    NHP_ACTION_TOCPU,        // 上送 CPU
    NHP_ACTION_MAX = 8
};


/* 下一跳结构 */
struct nhp_entry
{
    uint32_t         nhp_index;
    struct inet_addr nexthop;           // 原始下一跳
    struct inet_addr nexthop_connect;   // 迭代出来的直连下一跳
    uint32_t         ifindex;           // 迭代出来的直连出接口
    enum NHP_TYPE    nhp_type;          // 下一跳的类型，mpls 迭代之后跟 nhp_info 的类型可能不同
    enum NHP_ACTION  action;            // 丢弃或上送cpu, 用于黑洞路由和直连网段路由
    enum ROUTE_PROTO protocol;          // 生成路由的协议类型
    uint16_t         vpn;               // 下一跳所属的 vpn
    uint8_t          instance;          // 路由协议的实例号
    uint8_t          distance;          // 路由的 metic 值，0 最高
    uint32_t         cost;              // 路由的 cost 值，默认是 1
    uint8_t          active;            // 1: 路由是 active 的，优选之后的路由
    uint8_t          down_flag;         // 1: nhp is down when outif down
    uint8_t          arp_flag;          // 1: arp 有效
    uint8_t          pad[3];
    uint8_t          mac[6];
    uint32_t         port;              // 物理出接口, 用于 vlanif 成员口
};


/* 下一跳分离后的 nhp 数据结构 */
struct nhp_info
{
    uint32_t         nhp_index;
    enum NHP_TYPE    nhp_type;
    uint32_t         refcount;          // 引用计数
    void            *pnhp;              // 指向 nhp_entry 或 ecmp_group 数据结构
};


/* 下一跳分离后的 ECMP 数据结构 */
struct ecmp_group
{
    uint32_t        group_index;
    enum ECMP_TYPE  group_type;         // 0: ecmp, 1: frr
    struct nhp_info nhp[NHP_ECMP_NUM];  // nhp 组
    uint8_t         nhp_num;            // nhp 个数
    uint8_t         frr_flag;           // 0: master work, 1: backup work
    uint8_t         pad[2];
};


/* 路由表结构 */
struct route_entry
{
    struct inet_prefix prefix;
    uint16_t           vpn;
    uint8_t            nhp_num;
	uint8_t            pad;
    struct nhp_info    nhpinfo;          // 下一跳分离之后的 nhp
    struct nhp_entry   nhp[NHP_ECMP_NUM];// 原始 nhp 组
};


/* route cache fifo */
struct routefifo
{
    struct fifo        fifo;
    enum IPC_OPCODE    opcode;
    struct route_entry route;
};


/* route cache fifo */
struct ifmeventfifo
{
    struct fifo     fifo;
    struct ipc_mesg_n mesg;
};


/* 静态路由数据结构 */
struct route_static
{
    struct inet_prefix prefix;
    enum NHP_TYPE      nhp_type;
    enum NHP_ACTION    action;    // 丢弃或上送cpu, 用于黑洞路由和直连网段路由
    uint16_t           vpn;
    uint8_t            distance;
    uint8_t            down_flag; // 1: route is down when outif down
    struct inet_addr   nexthop;
    uint32_t           ifindex;
};

/* mib get rib route tree node */
struct route_rib_mib
{
    struct inet_prefix prefix;
    uint16_t           vpn;
    struct nhp_entry   nhp;
};

struct route_count_mib
{
    uint32_t vpn_instance;
    uint32_t total_num;
    uint32_t active_num;
    /* ---总数统计--- */
    uint32_t num_max;           // 路由总数
    uint32_t num_entries;       // 实际存储路由总数

    /* ---协议统计--- */
    uint32_t num_static;        // 静态路由总数
    uint32_t num_static_active; // 静态路由激活的总数
    uint32_t num_direct;        // 直连路由总数
    uint32_t num_direct_active; // 直连路由激活的总数
    uint32_t num_ospf;          // ospf 协议路由总数
    uint32_t num_ospf_active;   // ospf 协议路由激活的总数
    uint32_t num_isis;          // isis 协议路由总数
    uint32_t num_isis_active;   // isis 协议路由激活的总数
    uint32_t num_rip;           // rip 协议路由总数
    uint32_t num_rip_active;    // rip 协议路由激活的总数
    uint32_t num_ibgp;          // ibgp 协议路由总数
    uint32_t num_ibgp_active;   // ibgp 协议路由激活的总数
    uint32_t num_ebgp;          // ebgp 协议路由总数
    uint32_t num_ebgp_active;   // ebgp 协议路由激活的总数
    uint32_t num_ldp;           // ldp 协议路由总数
    uint32_t num_ldp_active;    // ldp 协议路由激活的总数
    uint32_t num_mplstp;        // mplstp 协议路由总数
    uint32_t num_mplstp_active; // mplstp 协议路由激活的总数
    uint32_t num_rsvpte;        // rsvpte 协议路由总数
    uint32_t num_rsvpte_active; // rsvpte 协议路由激活的总数
};


/* mib 获取 arp 操作子类型(msg_subtype) */
enum ARP_MIB_OPECODE
{
    ARP_MIB_GLOBAL_CONF = 0,
    ARP_MIB_STATIC_ARP,
    ARP_MIB_INTF_ARP_CONF,
    ARP_MIB_MAX = 5
};


/* ARP 状态 */
enum ARP_STATUS
{
    ARP_STATUS_INCOMPLETE = 0,	// 动态 arp 半连接状态
    ARP_STATUS_COMPLETE,        // 动态 ARP
    ARP_STATUS_STATIC,          // 静态 ARP
    ARP_STATUS_AGED,            // 老化状态
    ARP_STATUS_LLDP,            //lldp学习到的arp
    ARP_STATUS_MAX = 8
};

/* arp 的 subtype */
enum ARP_INFO
{
	ARP_INFO_STATIC_ARP = 0,  /* 静态 ARP */
    ARP_INFO_LEARN_LIMIT,    /*ARP 学习限制*/
	ARP_INFO_NUM_LIMIT,       /* 学习数量限制 */
	ARP_INFO_AGE_TIME,	      /* 老化时间 */
	ARP_INFO_AGENT,           /* ARP 代理 */
	ARP_INFO_HAL_ENTRY,      /*添加hal arp表项*/
	ARP_INFO_HAL_ANTI,       /*添加hal arp 防御信息*/
	ARP_INFO_FAKE_TIME,      /*临时arp 表项生存时间*/
	ARP_INFO_FIXED,     /*动态arp表项固化方式*/
    ARP_INFO_LLDP,      /*lldp arp info*/
    
    /*for h3c cmd*/
    ARP_INFO_SLOT_NUM_LIMIT,/*for h3c slot num_limit*/
    ARP_INFO_RESET_SLOT,/*for h3c reset slot*/
    ARP_INFO_CONF_STATIS,/*ARP statistics*/
	ARP_INFO_MAX = 16
};

/* ARP 表的 key */
struct arp_key
{
	uint32_t ipaddr;
	uint32_t vpnid;
};


/* ARP 表结构 */
struct arp_entry
{
    struct arp_key   key;
	enum ARP_STATUS  status;   // arp 状态
	enum ARP_STATUS  status_old;   // arp 状态
	uint32_t         ifindex;  // output interface
	uint32_t         port;     // 用于 vlanif 接口的物理成员口
	uint32_t         arpindex; // arp 索引
	uint8_t          mac[6];
	uint16_t         time;     // 剩余老化时间
    uint8_t          count;    // 发送 arp 请求的次数
	uint8_t          pad[3];
	uint8_t          gratuious; //标记此entry是否是冲突ip生成的arp
	uint32_t         gratuious_count; //记录冲突次数，根据次数进行发送速率限制
};


/* arp 的全局参数 */
struct arp_global
{
    uint32_t num_limit;         // 动态 arp 学习限制
    uint32_t slot_num_limit[SLOT_MAX][2];//for h3c arp_slot_num_limit
    uint16_t age_time;          // 动态 arp 老化时间
	uint16_t timer;             // arp timer 的周期
	uint32_t count;             // 动态 arp 数量
	uint32_t num_static;        // 静态 arp 数量
	uint32_t num_complete;      // 全连接的 arp 数量
	uint32_t num_incomplete;    // 半连接的 arp 数量
    uint8_t  fake_expire_time;  // 半连接表项老化时间
    uint8_t  fixed_flag;        // arp表项固化方式
};

/* arp to static list */
struct arp_to_static
{
    uint8_t status;
	struct list *arp_to_static_list;
};



extern int route_event_register(enum ROUTE_PROTO type, int module_id);
extern int route_event_unregister(enum ROUTE_PROTO type, int module_id);
extern int routev6_event_register(enum ROUTE_PROTO type, int module_id);
extern int routev6_event_unregister(enum ROUTE_PROTO type, int module_id);
extern int route_com_update(struct route_entry *proute, enum IPC_OPCODE opcode, int module_id);
extern uint32_t *route_com_get_router_id(int module_id);
extern struct ipv6_addr *route_com_get_routerv6_id(int module_id);
extern struct route_entry *route_com_get_route(struct inet_prefix *pprefix, uint16_t vpn, int module_id);
extern struct route_static *route_com_get_sroute_bulk(struct route_static *proute, uint32_t index_flag,
                                                        int module_id, int *pdata_num);
extern struct arp_global *route_com_get_garp_conf(int module_id);
extern struct arp_entry *route_com_get_sarp_bulk(uint32_t ipaddr, uint16_t vpn, int module_id, int *pdata_num);
extern int route_com_string_to_type(int afi, const char *s);
extern const char *route_com_type_to_string(unsigned int route_type);
extern struct route_global *route_com_get_route_gconf(int module_id);


#endif


