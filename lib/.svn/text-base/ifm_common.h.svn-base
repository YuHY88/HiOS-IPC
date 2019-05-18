/*
*     common define and function of interface
*/


#ifndef HIOS_IFM_COM_H
#define HIOS_IFM_COM_H

#include <lib/types.h>
#include <lib/inet_ip.h>
#include <lib/qos_common.h>
#include <lib/vty.h>
#include "msg_ipc_n.h"

#define IFM_NUM_MAX        6*1024  /*整机接口数量*/


#define IFM_DIR_INGRESS      0
#define IFM_DIR_EGRESS       1


#define IFNET_IS_PORT        1     /* 判断物理接口 */
#define IFNET_IS_SUBPORT     2     /* 判断子接口 */

#define IFM_UNIT_MIN                          0
#define IFM_UNIT_MAX                          7
#define IFM_SLOT_MIN                          0
#define IFM_SLOT_MAX                          31
#define IFM_PORT_MIN                          1
#define IFM_PORT_MAX                          255
#define IFM_SUBPORT_MIN                       1
#define IFM_SUBPORT_MAX                       4095
#define IFM_SUBTDM_MIN                        1
#define IFM_SUBTDM_MAX                        31
#define IFM_SUBSTM_MIN                        1
#define IFM_SUBSTM_MAX                        4095
#define IFM_VLAN_MIN                          1
#define IFM_VLAN_MAX                          4095
#define IFM_LOOPBACK_MIN                      0
#define IFM_LOOPBACK_MAX                      128
#define IFM_TRUNK_MIN                         1
#define IFM_TRUNK_MAX                         128
#define IFM_SUBTRUNK_MIN                      1
#define IFM_SUBTRUNK_MAX                      4095
#define IFM_CLOCK_MIN                         1
#define IFM_CLOCK_MAX                         2
#define IFM_TUNNL_MIN                         1
#define IFM_TUNNL_MAX                         2000
#define IFM_MTU_MIN                           64
#define IFM_MTU_MAX                           9550
#define IFM_JUMBO_MIN                         1518
#define IFM_JUMBO_MAX                         12288
#define IFM_FLAP_PERIOD_MIN                   0
#define IFM_FLAP_PERIOD_MAX                   255
#define IFNET_NAMESIZE                        NAME_STRING_LEN
#define IFM_USP_STRTOK_LEN                    4
/*sfp 存储字段长度定义*/
#define SFP_BASE_NAME_SIZE                               16
#define SFP_BASE_PN_SIZE                                 16
#define SFP_BASE_SN_SIZE                                 16
#define SFP_BASE_VENDOR_REV_SIZE                         4
#define SFP_BASE_VENDOR_OUI_SIZE                         3
#define SFP_BASE_CONNECTOR_SIZE                          45
#define SFP_BASE_NOMINAL_BR_SIZE                         16
#define SFP_BASE_VENDOR_SPECIFIC_SIZE                    32
#define SFP_BASE_IDENTIFIER_SIZE                         50
#define SFP_BASE_TRANSMISSION_MEDIA_SIZE                 20
#define SFP_BASE_FC_TRANSMISSION_MEDIA_SIZE              30

#define IFM_VLAN_COS_INVALID                  8
/*环回标志*/
#define IFM_LOOPBACK_EXTERNEL                 (1 << 0)
#define IFM_LOOPBACK_INTERNAL                 (1 << 1)
/* 设置环回*/
#define IFM_LB_SVLAN_SET                   ( 1 <<0 )
#define IFM_LB_CVLAN_SET                   ( 1 <<1 )
#define IFM_LB_SMAC_SET                     ( 1 <<2 )
#define IFM_LB_DMAC_SET                    ( 1 <<3 )
#define IFM_LB_SIP_SET                          ( 1 <<4 )
#define IFM_LB_DIP_SET                         ( 1 <<5 )
/*sfp_flags*/
#define IFM_SFP_ALS_ENABLE                    1
#define IFM_SFP_ALS_DISABLE                   2
#define IFM_SFP_TX_ENABLE                     1
#define IFM_SFP_TX_DISABLE                    2

/*流控配置*/
#define IFM_FLOW_CONTROL_ENABLE             1
#define IFM_FLOW_CONTROL_DISABLE            0
#define IFM_FLOW_CONTROL_RECEIVE            (1 << 0)
#define IFM_FLOW_CONTROL_SEND               (1 << 1)
#define IFM_FLOW_CONTROL_RECEIVE_AND_SEND   (IFM_FLOW_CONTROL_RECEIVE | IFM_FLOW_CONTROL_SEND)


//接口模式
enum IFNET_MODE
{
    IFNET_MODE_INVALID = 0,
    IFNET_MODE_L3,        //L3 模式，允许配 IP 地址，使能 ip 和 mpls 转发
    IFNET_MODE_L2,         //L2 模式，使能 L2VPN 接入
    IFNET_MODE_SWITCH,     //switch 模式，使能 vlan 转发和 MAC 学习
    IFNET_MODE_PHYSICAL,//physical interface mode, only allow set shutdown/speed etc.
    IFNET_MODE_MAX = 5
};


//接口事件类型
enum IFNET_EVENT
{
    IFNET_EVENT_DOWN = 0,   //接口 linkdown 事件
    IFNET_EVENT_UP,         //接口 linkup 事件
    IFNET_EVENT_IF_DELETE,  //接口删除事件
    IFNET_EVENT_MODE_CHANGE,//接口 l2/l3/portswith 模式切换事件
    IFNET_EVENT_IP_ADD,     //接口 IP 地址添加事件
    IFNET_EVENT_IP_DELETE,  //接口 IP 地址删除事件
    IFNET_EVENT_MTU_CHANGE,
    IFNET_EVENT_IF_ADD ,    //接口 add事件
    IFNET_EVENT_IF_HIDE_RECOVER ,    //接口  hide 恢复事件
    IFNET_EVENT_L3VPN,      //接口 l3vpn 变更事件
    IFNET_EVENT_IF_ADD_FINISH,//设备上电板卡加载完成
    IFNET_EVENT_SPEED_CHANGE,
    IFNET_EVENT_IP_PD_ADD,     //接口 IP PD 地址添加事件
    IFNET_EVENT_IP_PD_DELETE,  //接口 IP PD 地址删除事件
    IFNET_EVENT_MAX = 16
};


//接口 link 状态

enum IFNET_LINKSTAT
{
    IFNET_LINKUP = 0,
    IFNET_LINKDOWN
};

//接口管理状态

enum IFNET_ADMINSTAT
{
    IFNET_NO_SHUTDOWN = 0,
    IFNET_SHUTDOWN
};

/*只有 subtype = GE 的接口可以配置速率*/
enum IFNET_SPEED
{
    IFNET_SPEED_INVALID = 0,
    IFNET_SPEED_GE,
    IFNET_SPEED_FE,
    IFNET_SPEED_10GE,
    IFNET_SPEED_10M
};

/* 速率模式 */
enum IFNET_SPEED_MODE
{
    IFNET_SPEED_MODE_INVALID = 0,
    IFNET_SPEED_MODE_AUTO,
    IFNET_SPEED_MODE_FORCE
};


/* 双工模式 */
enum IFNET_DUPLEX
{
    IFNET_DUPLEX_INVALID = 0,
    IFNET_DUPLEX_FULL,
    IFNET_DUPLEX_HALF
};


/* 统计的动作 */

enum IFNET_STAT_ACTION
{
    IFNET_STAT_DISABLE = 0, /*统计关闭*/
    IFNET_STAT_ENABLE,      /*统计开启*/
    IFNET_STAT_CLEAR        /*统计清除*/
};

/* 接口的 STP 状态 */
enum IFNET_STP_STATUS
{
    IFNET_STP_DISABLE = 0,
    IFNET_STP_BLOCK,
    IFNET_STP_LISTEN,
    IFNET_STP_LEARN,
    IFNET_STP_FORWARD
};


//接口类型
enum IFNET_TYPE
{
    IFNET_TYPE_INVALID = 0,
    IFNET_TYPE_ETHERNET,   //以太网接口
    IFNET_TYPE_GIGABIT_ETHERNET,//千兆以太网接口
    IFNET_TYPE_XGIGABIT_ETHERNET,//万兆以太网接口
    IFNET_TYPE_ETH_SUBPORT,//以太网子接口，包括 trunk 子接口
    IFNET_TYPE_VLANIF,     //vlanif 接口,是逻辑接口
    IFNET_TYPE_LOOPBACK,   //loopback 接口，是逻辑接口
    IFNET_TYPE_TUNNEL,     //tunnel接口，是逻辑接口
    IFNET_TYPE_TRUNK,      //trunk 接口，是逻辑接口
    IFNET_TYPE_TDM,        //TDM 接口
    IFNET_TYPE_STM,        //STM 接口
    IFNET_TYPE_CLOCK,      //外时钟接口
    IFNET_TYPE_VCG,        //VCG 接口
    IFNET_TYPE_E1,         //Controller E1 接口
    IFNET_TYPE_MAX = 16
};


//接口子类型，接口创建时确定，不能更改
enum IFNET_SUBTYPE
{
    IFNET_SUBTYPE_INVALID = 0,
    IFNET_SUBTYPE_SUBPORT,      /*子接口*/
    IFNET_SUBTYPE_VPORT,        /*逻辑接口*/
    IFNET_SUBTYPE_FE,           /*以太 FE 接口*/
    IFNET_SUBTYPE_GE,           /*以太 GE 接口*/
    IFNET_SUBTYPE_10GE,         /*以太 10GE 接口*/
    IFNET_SUBTYPE_40GE,         /*以太 40GE 接口*/
    IFNET_SUBTYPE_100GE,        /*以太 100GE 接口*/
    IFNET_SUBTYPE_COMBO,        /*以太 COMBO 接口*/
    IFNET_SUBTYPE_E1,           /*TDM E1 接口*/
    IFNET_SUBTYPE_T1,           /*TDM T1 接口*/
    IFNET_SUBTYPE_STM1,         /*STM-1 接口*/
    IFNET_SUBTYPE_STM4,         /*STM-4 接口*/
    IFNET_SUBTYPE_STM16,        /*STM-16 接口*/
    IFNET_SUBTYPE_STM64,        /*STM-64 接口*/
    IFNET_SUBTYPE_CLOCK2M,      /*2m clock 接口*/
    IFNET_SUBTYPE_ODU0,         /*OTN ODU0 接口, 带宽 1G */
    IFNET_SUBTYPE_ODU1,         /*OTN ODU1 接口, 带宽 2.5G */
    IFNET_SUBTYPE_ODU2,         /*OTN ODU2 接口, 带宽 10G */
    IFNET_SUBTYPE_ODU3,         /*OTN ODU3 接口, 带宽 40G */
    IFNET_SUBTYPE_ODU4,         /*OTN ODU4 接口, 带宽 100G */
    IFNET_SUBTYPE_PHYSICAL,
    IFNET_SUBTYPE_CE1,          /*TDM E1 for VX*/
    IFNET_SUBTYEP_MAX = 255
};


/* 接口光电属性 */

enum IFNET_FIBERTYPE
{
    IFNET_FIBERTYPE_FIBER = 0,  /*fibre port*/
    IFNET_FIBERTYPE_COPPER,     /*copper port*/
    IFNET_FIBERTYPE_COMBO,      /*combo port*/
    IFNET_FIBERTYPE_OTHER,      /*other port*/
};


/* 接口事件注册用的接口类型 */
enum IFNET_EVENT_IFTYPE
{
    IFNET_IFTYPE_INVALID = 0,
    IFNET_IFTYPE_L2IF,
    IFNET_IFTYPE_L3IF,
    IFNET_IFTYPE_CES,
    IFNET_IFTYPE_PORT,
    IFNET_IFTYPE_E1,
    IFNET_IFTYPE_VCG,
    IFNET_IFTYPE_ALL = 7
};


/* 子接口封装类型 */

enum IFNET_ENCAP
{
    IFNET_ENCAP_INVALID = 0,
    IFNET_ENCAP_UNTAG,
    IFNET_ENCAP_DOT1Q,
    IFNET_ENCAP_QINQ,
    IFNET_ENCAP_VLANIF,   /* vlanif 接口的封装 */
    IFNET_ENCAP_MAX = 8
};


//接口信息类型
enum IFNET_INFO
{
    IFNET_INFO_PARENT,  //父接口
    IFNET_INFO_TYPE,    //接口类型
    IFNET_INFO_SUBTYPE, //子接口类型
    IFNET_INFO_MODE,    //接口模式，默认 L3 模式
    IFNET_INFO_ENCAP,   //接口封装格式
    IFNET_INFO_SHUTDOWN,
    IFNET_INFO_BLOCK,   //接口 block
    IFNET_INFO_SPEED,   //接口速率和协商方式
    IFNET_INFO_DUPLEX,  //接口双工模式
    IFNET_INFO_USP,
    IFNET_INFO_STATUS,  //接口 link 状态，从芯片获取
    IFNET_INFO_STATISTICS,//接口统计状态
    IFNET_INFO_TPID,    //接口 TPID，默认 0X8100
    IFNET_INFO_MTU,     //最大传输单元，默认 9600
    IFNET_INFO_JUMBO,   //接口jumbo 帧长，默认 10000
    IFNET_INFO_FLAP,    //震荡抑制周期
    IFNET_INFO_ALIAS,   //接口别名
    IFNET_INFO_MAC,
    IFNET_INFO_IPV4,    //接口 IPV4 地址，只能配置在 L3 模式接口下
    IFNET_INFO_IPV6,
    IFNET_INFO_VPN,     //接口与 VPN 绑定
    IFNET_INFO_URPF,    //接口 urpf 属性
    IFNET_INFO_COUNTER,//接口统计计数
    IFNET_INFO_HW,
    IFNET_INFO_SON,
    IFNET_INFO_NEXT_IFINDEX,
    IFNET_INFO_PORT_INFO,
    IFNET_INFO_LOOPBACK,
    IFNET_INFO_SUBIF,
    IFNET_INFO_SFP_ALS,
    IFNET_INFO_SFP_TX,
    IFNET_INFO_SFP_ON,
    IFNET_INFO_SFP_OFF,
    IFNET_INFO_SFP,
    IFNET_INFO_HA_PORT,
    IFNET_INFO_HA_ALL,
    IFNET_INFO_HA_REQUEST,
    IFNET_INFO_SFP_ALL,
    IFNET_INFO_DOWN_FLAP,    //震荡抑制周期
    IFNET_INFO_PORT_COUNT,//接口数量
    IFNET_INFO_LOOPBACK_FUNC,
    IFNET_INFO_COMBO,
    IFNET_INFO_FLOW_CONTROL,    //接口流控功能
	IFNET_INFO_SUBIF2,//创建l2子接口
	IFNET_INFO_INTERVAL, //采样间隔
	IFNET_INFO_PHYMASTER,
	IFNET_INFO_PHYMASTER_BULK,
	IFNET_INFO_SPEED_BULK,
    IFNET_INFO_MAX = 49 //添加枚举值，请将最大值加1
};


/* 接口 IP flag */
enum IP_TYPE
{
    IP_TYPE_INVALID = 0,
    IP_TYPE_STATIC,     /* 接口静态 IP */
    IP_TYPE_DHCP,       /* 接口 dhcp 分配 IP */
    IP_TYPE_UNNUMBERED, /* 接口借用 IP */
    IP_TYPE_SLAVE,      /* 接口从 IP */
    IP_TYPE_LINK_LOCAL, /* IPv6 链路本地地址 */
    IP_TYPE_DHCP_ZERO,
    IP_TYPE_DHCP_PD,    /* 接口 dhcp 分配 IP PD*/
    IP_TYPE_MAX = 8
};

/* Qinq 转换的动作 */

enum VLAN_ACTION
{
    VLAN_ACTION_NO = 0,
    VLAN_ACTION_ADD,
    VLAN_ACTION_DELETE,
    VLAN_ACTION_TRANSLATE,
    VLAN_ACTION_MAX = 8
};

/* ospf-dcn IPC 消息子类型 */
enum OSPF_DCN_STATE_TYPE
{
	OSPF_DCN_DISABLE = 0,
	OSPF_DCN_ENABLE
};

struct vlan_range
{
    uint16_t vlan_start;
    uint16_t vlan_end;
};

struct ifm_reflector
{
	uint8_t reflector_flag;
	uint8_t exchange_mac;
	uint8_t exchange_ip;
	uint8_t set_flag;
};

/*环回信息*/
struct  ifm_loopback
{
	uint8_t set_flag;
	uchar smac[6];
	uchar dmac[6];
	uint16_t svlan;
	uint16_t cvlan;
	struct ifm_reflector reflector;
	struct inet_prefixv4 sip;
	struct inet_prefixv4 dip;
};

/* 通知接口事件的同时，通知接口的 up/down 状态 */

struct ifm_event
{
    uint32_t ifindex;
    enum IFNET_EVENT event;   /* 接口事件类型 */
    enum IFNET_MODE  mode;    /* 接口模式，用于接口模式改变事件 */
    enum IP_TYPE     ipflag;  /* 接口 IP 类型，用于 IP 改变事件 */
    uint16_t         vpn;     /* 接口绑定的 VPN */
    uint8_t          up_flag; /* 接口 up/down 状态, 1: up, 0: down */
    struct inet_prefix ipaddr;/* 接口 IP 地址 */
    uint16_t         mtu;
} __attribute__ ( ( packed ) );


/* 子接口封装参数 */
struct ifm_encap
{
    enum IFNET_ENCAP  type;      /* 封装类型 */
    struct vlan_range cvlan;     /* 转换前的 cvlan */
    struct vlan_range svlan;     /* 转换前的 svlan */
    struct vlan_range cvlan_new; /* 转换后的 cvlan */
    struct vlan_range svlan_new; /* 转换后的 svlan */
    uchar cvlan_cos;             /* 转换后的 cvlan cos : 0 -7 有效， 8 表示无效 */
    uchar svlan_cos;             /* 转换后的 svlan cos : 0 -7 有效， 8 表示无效 */
    enum VLAN_ACTION cvlan_act;  /* cvlan 的转换动作*/
    enum VLAN_ACTION svlan_act;  /* svlan 的转换动作*/
};


/* 接口 usp 信息，在接口创建时确定，不能更改 */
struct ifm_usp
{
    enum IFNET_TYPE     type;    /* 接口类型 */
    enum IFNET_SUBTYPE  subtype; /* 接口子类型 */
    uint8_t  unit;
    uint8_t  slot;
    uint8_t  port;               /* 子接口和逻辑接口的 port 固定为 0 */
    uint16_t sub_port;           /* 物理接口的 sub_port 固定为 0 */
} __attribute__ ( ( packed ) );


/* 物理端口的信息 */
struct ifm_port
{
    uint8_t  speed;       /*enum IFNET_SPEED*/
    uint8_t  autoneg;     /*enum IFNET_SPEED_MODE */
    uint8_t  duplex;      /*enum IFNET_DUPLEX*/
    uint8_t  chipid;      /* 交换芯片 ID */
    uint8_t  gport;       /* 交换芯片的 port 号*/
    uint8_t  fiber;       /* 端口的光电类型   enum IFNET_FIBERTYPE*/
	uint8_t  fiber_sub;    /* 端口的光电类型   enum IFNET_FIBERTYPE*/
    uint8_t  flap_period; /* up port flapping-control period */
    uint8_t  time;        /* up震荡抑制剩余时间 */
    uint16_t trunkid;     /* 所属的 trunk 接口的 id */
    uint8_t  hide;        /* 1: 表示为隐藏接口，不对用户呈现*/
    uint8_t  down_flap_period; /* down port flapping-control period */
	uint8_t  down_time;        /* down震荡抑制剩余时间 */
	uint64_t rx_bytes;
	uint64_t tx_bytes;
};

struct ifm_phy_mode
{
	uint32_t  ifindex;
	uint32_t  phy_mode;
};
struct ifm_speed
{
	uint32_t  ifindex;
	uint8_t   speed;       /*enum IFNET_SPEED*/
    uint8_t   autoneg;     /*enum IFNET_SPEED_MODE */
    uint8_t   duplex;      /*enum IFNET_DUPLEX*/
	
};

/* 接口创建时确定的基本信息 */
struct ifm_common
{
    struct ifm_usp    usp;
    struct ifm_port   port_info;/* 物理端口的信息 */
    uchar             mac[6];
} __attribute__ ( ( packed ) );


#define IFM_IP_NUM     2    /* 接口 IP 地址数量 */


/* l3 mode 的接口信息 */

struct ifm_l3
{
    uint32_t      ifindex;
    uint16_t      vpn;        /* l3vpn id */
    uint8_t       conflict_flag[IFM_IP_NUM];/* ip冲突标志位 */
    enum IP_TYPE  ipv4_flag; /* 主 IPV4 地址的协议类型, 0: 未配置, 1:静态 IP，2: dhcp 分配 IP ，3: 借用地址*/
    enum IP_TYPE  ipv6_flag;  /* 主 IPV6 地址的协议类型, 0: 未配置, 1:静态 IP，2: dhcp 分配 IP ，3: 借用地址*/
    uint32_t unnumbered_if;       /* ifindex of unnumbered interface */
    struct inet_prefixv4 ipv4[IFM_IP_NUM]; /* 接口 IPV4 地址, 第一个是主地址，第 2 个是从地址 */
    struct inet_prefixv6 ipv6[IFM_IP_NUM]; /* 接口 IPV6 地址, 第一个是主地址，第 2 个是从地址 */
    struct inet_prefixv6 ipv6_link_local;  /* IPv6 链路本地地址 */
};

//接口流控信息        added by liub 2018-4-23 for flow control
struct ifm_flow_control
{
    uint8_t status;  //流控状态，使能/非使能
    uint8_t direction;  //方向，发送/接收/发送和接收
    uint64_t rx_pause;
    uint64_t tx_pause;
};

//接口mac学习状态和限制          added by liub 2018-5-8 for h3c cli
struct ifm_mac_config
{
	uint8_t          mac_learn;  /* 端口的 MAC 学习*/
	uint8_t          limit_action;/* 超出mac学习限制值时报文处理 ,0: disable--discard, 1: enable--forward */
	uint32_t         limit_num;   /* mac学习限制数，1-16384,默认值为0 */ 
};


#define  PORT_MS_SLAVE      2
#define  PORT_MS_MASTER     1
#define  PORT_MS_AUTO       3

/* 接口公共数据结构 */
struct ifm_info
{
    uint32_t ifindex;           /* type:4bit, unit:3bit, slot:5bit, port:8bit, sub_port:12bit*/
    uint32_t parent;            /* 子接口的父接口是物理口，vlan 成员的父接口是 vlanif接口, trunk 成员的父接口是 trunk 接口 */
    enum IFNET_TYPE    type;    /* 接口类型 */
    enum IFNET_SUBTYPE sub_type;/* 接口子类型 */
    uint8_t  unit;
    uint8_t  slot;
    uint8_t  port;
    uint16_t  trunkid;           /* 加入的 trunkid */
    uint16_t sub_port;          /* 子接口编号，范围 1-4096 */
    uint8_t  hide;               /* 1: 接口隐藏， 0: 不隐藏 */
    uint8_t  mode;              /*enum IFNET_MODE */
    uint8_t  shutdown;          /*enum IFNET_SHUTDOWN*/
    uint8_t  status;            /*enum IFNET_LINKSTATUS */
    uint8_t  statistics;        /* enum IFNET_STAT_ACTION*/
    uint8_t  pad;
    uchar    mac[6];
    uint16_t tpid;              /*in packet will parse 0x8100 and the config tpid , egress packet use  the config tpid, if no config tpid use 0x8100*/
    uint16_t mtu;               /* mtu size */
    uint16_t jumbo;             /* jumbo frame size */
	int  rx_width;  
	int  tx_width;             
    struct ifm_encap encap;      /* 接口的封装 vlan */
    struct qos_entry qos;       /* 接口配置的 qos mapping 和优先级 */
    struct ifm_loopback loop_info;
    uint8_t  lb_flags;          /* loopback flags IFM_LOOPBACK_XXX. for stm1 0: noloopback, 1: remote, 2: local*/
	struct ifm_reflector reflector;
	uint8_t  sla_flag;          /* sla enable flag */
    uint32_t sfp_off;           /* sfp_off size*/
    uint32_t sfp_on;			/* sfp_on size*/
    uint8_t  sfp_als;         /* sfp auto flags */
    uint8_t  sfp_tx;            /* sfp manual */
	uint8_t  interval;          /* sample interval*/
	uint8_t  interval_time;         
    time_t   last_change;       /*接口link变化时间*/
    struct ifm_flow_control flow_ctrl;  /*接口流控配置*/
    struct ifm_mac_config mac_config;   /*mac 学习状态、限制*/
    uint32_t phymaster; 
    uint32_t phymaster_status;
};


/* 接口统计 */
struct ifm_counter
{  
    uint32_t ifindex;
    uint64_t rx_packets;   /* total packets received       */
    uint64_t tx_packets;   /* total packets transmitted    */
    uint64_t rx_bytes;     /* total bytes received         */
    uint64_t tx_bytes;     /* total bytes transmitted      */
    uint64_t rx_errors;    /* error packets received       */
    uint64_t tx_errors;    /* error packet transmited      */
    uint64_t rx_dropped;   /* drop packets received        */
    uint64_t tx_dropped;   /* drop packets transmitted     */
    uint64_t rx_multicast; /* multicast packets received   */
    uint64_t tx_multicast; /* multicast packets transmitted*/
    uint64_t rx_broadcast; /* broadcast packets received   */
    uint64_t tx_broadcast; /* broadcast packets transmitted*/
    uint64_t rx_ucast; /* ucast packets received   */
    uint64_t tx_ucast; /* ucast packets transmitted*/
	uint64_t rx_nucast;/*nucast packets received */
	uint64_t tx_nucast;/*nucast packets transmitted*/
	uint64_t rx_uknownprotos;/*unknown protos received*/
	uint64_t rx_filtered;
    uint64_t undersize_pkts;
    uint64_t oversize_pkts;
    uint64_t bad_crc;
    uint64_t good_octets_rcv;     /*total good octets received*/
    uint64_t mac_transmit_err;    /*mac transmit error*/
    uint64_t good_pkts_rcv;       /*good packets received*/
	uint64_t mc_pkts_rcv;         /*mc packets received*/
	uint64_t pkts_64_octets;      /*packets 64 octets*/
	uint64_t pkts_65_127_octets;  /*packets 65 to 127 octets*/
	uint64_t pkts_128_255_octets; /*packets 128 to 255 octets*/
	uint64_t pkts_256_511_octets; /*packets 256 to 511 octets*/
	uint64_t pkts_512_1023_octets;/*packets 512 to 1023 octets*/
	uint64_t pkts_1024_max_octets;
	uint64_t good_octets_sent;    /*good octets sent*/
	uint64_t good_pkts_sent;      /*good packets sent*/
    uint64_t excessive_collisions;/*collisions*/
	uint64_t mc_pkts_sent;        /*mc packets sent*/
	uint64_t drop_events;         /*drop events*/
	uint64_t late_collisions;     /*late collisions*/
	uint64_t in_discards;         /*discard received*/
	uint64_t out_discards;        /*discard transmitted*/
	uint64_t brdc_pkts_rcv;
	uint64_t bad_octets_rcv;      /*bad octets received*/
	uint64_t brdc_pkts_sent;
    uint64_t fragments_pkts;
    uint64_t jabber_pkts;
    uint64_t collisions;
	uint64_t unic_pkts_rcv;
	uint64_t unic_pkts_sent;
    uint64_t mac_rcv_error;
	uint64_t bad_pkts_sent;
    uint64_t stats_pkts;
    uint64_t stats_octets;
    uint64_t stats_multicast;
	uint64_t stats_broadcast;
    uint64_t pkts_1024_1518_octets;
    uint64_t pkts_1519_1522_octets;
    uint64_t pkts_1523_2047_octets;
    uint64_t pkts_2048_4095_octets;
	uint64_t pkts_4096_9216_octets;
	uint64_t pkts_9217_16383_octets;
    uint64_t oversize_pkts_rcv;
	uint64_t pkts_1519_max_octets;
    uint64_t out_qlen;
	
    uint64_t unrecog_mac_cntr_rcv;
    uint64_t bad_pkts_rcv;
    uint64_t fc_sent;
    uint64_t good_fc_rcv;
    uint64_t bad_fc_rcv;

	int      rx_width;  
	int      tx_width;

};
struct ifm_counter_bulk
{
   uint32_t ifindex;
   struct ifm_counter counter_data;
};
/*ifm sfp 数据结构*/
struct ifm_sfp_thershold
{
         int32_t current;
         int32_t high_alarm;
         int32_t low_alarm;
         int32_t high_warn;
         int32_t low_warn;
};

/*sfp 阈值配置存储结构*/
struct hal_sfp_alarm_warn
{
    int high_alarm;
    int low_alarm;
	int high_warn;
    int low_warn;
};

/*sfp eeprom 信息结构体*/

struct ifm_sfp
{
         /*base eeprom:A0h*/
         int8_t vendor_pn[SFP_BASE_PN_SIZE + 1];
		 int8_t vendor_sn[SFP_BASE_SN_SIZE + 1];
         int8_t connector[SFP_BASE_CONNECTOR_SIZE];
         int8_t nominal_br[SFP_BASE_NOMINAL_BR_SIZE];
         int8_t transmission_media[SFP_BASE_TRANSMISSION_MEDIA_SIZE];
         int8_t vendor_name[SFP_BASE_NAME_SIZE + 1];
         int8_t vendor_specific[SFP_BASE_VENDOR_SPECIFIC_SIZE + 1];
         int8_t vendor_oui[SFP_BASE_VENDOR_OUI_SIZE + 1];
         int8_t vendor_rev[SFP_BASE_VENDOR_REV_SIZE + 1];
         int8_t identifier[SFP_BASE_IDENTIFIER_SIZE];
         int8_t fc_transmission_media[SFP_BASE_FC_TRANSMISSION_MEDIA_SIZE];/*光纤通道传输媒介*/
         uint32_t wave_length;
         uint32_t transmission_distance;
         /*diag eeprom:A2h*/
         uint32_t tx_disable;
         uint32_t tx_fault;
         uint32_t rx_loss;
		 uint32_t temp_halarm;
		 uint32_t temp_lalarm;
         struct ifm_sfp_thershold temperature;
         struct ifm_sfp_thershold voltage;
         struct ifm_sfp_thershold bias;
         struct ifm_sfp_thershold tx_power;
         struct ifm_sfp_thershold rx_power;

		 struct hal_sfp_alarm_warn tx_set;
		 struct hal_sfp_alarm_warn rx_set;
};

struct  ifm_sfp_info
{
    uint32_t ifindex;
    struct ifm_sfp info;
};


struct qos_entry_snmp
{
	uint32_t ifindex;
	struct qos_entry qos;
};

struct  mib_l2if_index
{
	uint32_t  ifindex;
};

struct  mib_l2if_storm_control_data
{
	uint8_t   storm_unicast; 	 /* 未知单播风暴抑制 ,0: disable, 1: enable*/
	uint8_t   storm_broadcast;	 /* 广播风暴抑制 ,0: disable, 1: enable*/
	uint8_t   storm_multicast;    /* 组播风暴抑制 ,0: disable, 1: enable*/
	uint8_t   pad;
	uint32_t  unicast_rate; 	 /* 未知单播风暴抑制速率*/
	uint32_t  broadcast_rate;	 /* 广播风暴抑制速率*/
	uint32_t  multicast_rate;     /* 组播风暴抑制速率*/
};

struct  mib_l2if_dot1q_tunnel_data
{
	uint8_t          dot1q_cos;  /* dot1q tunnel 的 cos */
	uint16_t         dot1q_tunnel;/* 端口的 dot1q-tunnel vlan */
};

struct  mib_l2if_storm_control_info
{
	struct mib_l2if_index ifindex;
	struct  mib_l2if_storm_control_data data;
};

struct  mib_l2if_dot1q_tunnel_info
{
	struct mib_l2if_index ifindex;
	struct  mib_l2if_dot1q_tunnel_data data;
};




#define _SHR_SUBPORT_SHIFT                             0
#define _SHR_SUBPORT_MASK                              0xfff
#define _SHR_PORT_SHIFT                                12
#define _SHR_PORT_MASK                                 0xff
#define _SHR_SLOT_SHIFT                                20
#define _SHR_SLOT_MASK                                 0x1f
#define _SHR_UNIT_SHIFT                                25
#define _SHR_UNIT_MASK                                 0x7
#define _SHR_PORT_TYPE_SHIFT                           28
#define _SHR_PORT_TYPE_MASK                            0xf


#define _SHR_PORT_TYPE_IS_ETHERNET(ifindex)\
        (((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK) == IFNET_TYPE_ETHERNET)
#define _SHR_PORT_TYPE_IS_GIGABIT_ETHERNET(ifindex)\
		(((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK) == IFNET_TYPE_GIGABIT_ETHERNET)
#define _SHR_PORT_TYPE_IS_XGIGABIT_ETHERNET(ifindex)\
		(((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK) == IFNET_TYPE_XGIGABIT_ETHERNET)
#define _SHR_PORT_TYPE_IS_VLANIF(ifindex)\
        (((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK) == IFNET_TYPE_VLANIF)
#define _SHR_PORT_TYPE_IS_LOOPBCK(ifindex)\
        (((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK) == IFNET_TYPE_LOOPBACK)
#define _SHR_PORT_TYPE_IS_TUNNEL(ifindex)\
        (((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK) == IFNET_TYPE_TUNNEL)
#define _SHR_PORT_TYPE_IS_TRUNK(ifindex)\
        (((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK) == IFNET_TYPE_TRUNK)
#define _SHR_PORT_TYPE_IS_TDM(ifindex)\
        (((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK) == IFNET_TYPE_TDM)
#define _SHR_PORT_TYPE_IS_STM(ifindex)\
        (((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK) == IFNET_TYPE_STM)
#define _SHR_PORT_TYPE_IS_CLOCK(ifindex)\
        (((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK) == IFNET_TYPE_CLOCK)
#define _SHR_PORT_TYPE_IS_VCG(ifindex)\
        (((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK) == IFNET_TYPE_VCG)   
#define _SHR_PORT_TYPE_IS_E1(ifindex)\
		(((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK) == IFNET_TYPE_E1)		
#define _SHR_PORT_IS_SUBPORT(ifindex)\
        (((ifindex >>_SHR_SUBPORT_SHIFT) & _SHR_SUBPORT_MASK) != 0)
#define _SHR_PORT_SUBPORT_ID_GET(ifindex)\
        ((ifindex >>_SHR_SUBPORT_SHIFT) & _SHR_SUBPORT_MASK)
#define _SHR_PORT_ID_GET(ifindex)\
        ((ifindex >>_SHR_PORT_SHIFT) & _SHR_PORT_MASK)
#define _SHR_PORT_SLOT_ID_GET(ifindex)\
        ((ifindex >>_SHR_SLOT_SHIFT) & _SHR_SLOT_MASK)
#define _SHR_PORT_UNIT_ID_GET(ifindex)\
        ((ifindex >>_SHR_UNIT_SHIFT) & _SHR_UNIT_MASK)
#define _SHR_PORT_TYPE_ID_GET(ifindex)\
        ((ifindex >>_SHR_PORT_TYPE_SHIFT) & _SHR_PORT_TYPE_MASK)
#define _SHR_PORT_PARENT_IFINDEX_GET(ifindex)\
        (ifindex & (~_SHR_SUBPORT_MASK))
#define _SHR_PORT_TRUNK_IFINDEX_GET(trunkid)\
          (( ( IFNET_TYPE_TRUNK & _SHR_PORT_TYPE_MASK ) << _SHR_PORT_TYPE_SHIFT )\
          | ( ( 0 & _SHR_UNIT_MASK ) << _SHR_UNIT_SHIFT )\
          | ( ( 0 & _SHR_SLOT_MASK ) << _SHR_SLOT_SHIFT )\
          | ( ( trunkid & _SHR_PORT_MASK ) << _SHR_PORT_SHIFT )\
          | ( ( 0 & _SHR_SUBPORT_MASK ) << _SHR_SUBPORT_SHIFT ))

#define _SHR_PORT_VLANIF_IFINDEX_GET(vlanid)\
          (( ( IFNET_TYPE_VLANIF & _SHR_PORT_TYPE_MASK ) << _SHR_PORT_TYPE_SHIFT )\
          | ( ( 0 & _SHR_UNIT_MASK ) << _SHR_UNIT_SHIFT )\
          | ( ( 0 & _SHR_SLOT_MASK ) << _SHR_SLOT_SHIFT )\
          | ( ( 0 & _SHR_PORT_MASK ) << _SHR_PORT_SHIFT )\
          | ( ( vlanid & _SHR_SUBPORT_MASK ) << _SHR_SUBPORT_SHIFT ))
#define _IFM_SUBIFINDEX_GET(ifindex, vlanid)\
		 ( ifindex | vlanid )


/* 根据 ifindex 判断接口类型 */
#define IFM_TYPE_IS_METHERNET(ifindex)\
		(IFM_TYPE_IS_ETHERNET(ifindex)||IFM_TYPE_IS_GIGABIT_ETHERNET(ifindex)||IFM_TYPE_IS_XGIGABIT_ETHERNET(ifindex))
#define IFM_TYPE_IS_ETHERNET(ifindex)\
       _SHR_PORT_TYPE_IS_ETHERNET(ifindex)
#define IFM_TYPE_IS_GIGABIT_ETHERNET(ifindex)\
       _SHR_PORT_TYPE_IS_GIGABIT_ETHERNET(ifindex)
#define IFM_TYPE_IS_XGIGABIT_ETHERNET(ifindex)\
       _SHR_PORT_TYPE_IS_XGIGABIT_ETHERNET(ifindex)
#define IFM_TYPE_IS_OUTBAND(ifindex)\
       (IFM_TYPE_IS_METHERNET(ifindex) && (_SHR_PORT_SLOT_ID_GET(ifindex) == 0))
#define IFM_TYPE_IS_VLANIF(ifindex)\
        _SHR_PORT_TYPE_IS_VLANIF(ifindex)
#define IFM_TYPE_IS_LOOPBCK(ifindex)\
        _SHR_PORT_TYPE_IS_LOOPBCK(ifindex)
#define IFM_TYPE_IS_TUNNEL(ifindex)\
        _SHR_PORT_TYPE_IS_TUNNEL(ifindex)
#define IFM_TYPE_IS_TRUNK(ifindex)\
        _SHR_PORT_TYPE_IS_TRUNK(ifindex)
#define IFM_TYPE_IS_TDM(ifindex)\
        _SHR_PORT_TYPE_IS_TDM(ifindex)
#define IFM_TYPE_IS_STM(ifindex)\
        _SHR_PORT_TYPE_IS_STM(ifindex)
#define IFM_TYPE_IS_CLOCK(ifindex)\
        _SHR_PORT_TYPE_IS_CLOCK(ifindex)
#define IFM_TYPE_IS_VCG(ifindex)\
		_SHR_PORT_TYPE_IS_VCG(ifindex)
#define IFM_TYPE_IS_E1(ifindex)\
		_SHR_PORT_TYPE_IS_E1(ifindex)		
#define IFM_IS_SUBPORT(ifindex)\
            _SHR_PORT_IS_SUBPORT(ifindex)

#define IFM_TYPE_IS_ETHERNET_PHYSICAL(_ifindex)\
		(IFM_TYPE_IS_METHERNET(_ifindex) && !IFM_IS_SUBPORT(_ifindex))
#define IFM_TYPE_IS_ETHERNET_SUBPORT(_ifindex)\
		(IFM_TYPE_IS_METHERNET(_ifindex) && IFM_IS_SUBPORT(_ifindex))
#define IFM_TYPE_IS_TRUNK_PHYSICAL(_ifindex)\
		(IFM_TYPE_IS_TRUNK(_ifindex) && !IFM_IS_SUBPORT(_ifindex))
#define IFM_TYPE_IS_TRUNK_SUBPORT(_ifindex)\
		(IFM_TYPE_IS_TRUNK(_ifindex) && IFM_IS_SUBPORT(_ifindex))
#define IFM_TYPE_IS_PHYSICAL(_ifindex)\
		((IFM_TYPE_IS_ETHERNET_PHYSICAL(_ifindex)) || (IFM_TYPE_IS_TRUNK_PHYSICAL(_ifindex)))
#define IFM_TYPE_IS_SUBPORT(_ifindex)\
		((IFM_TYPE_IS_ETHERNET_SUBPORT(_ifindex)) || (IFM_TYPE_IS_TRUNK_SUBPORT(_ifindex)))


/* 根据 ifindex 获取接口 ID */
#define IFM_UNIT_ID_GET(ifindex)\
        _SHR_PORT_UNIT_ID_GET(ifindex)
#define IFM_SLOT_ID_GET(ifindex)\
            _SHR_PORT_SLOT_ID_GET(ifindex)
#define IFM_PORT_ID_GET(ifindex)\
            _SHR_PORT_ID_GET(ifindex)
#define IFM_SUBPORT_ID_GET(ifindex)\
            _SHR_PORT_SUBPORT_ID_GET(ifindex)
#define IFM_TYPE_ID_GET(ifindex)\
        _SHR_PORT_TYPE_ID_GET(ifindex)
#define IFM_PARENT_IFINDEX_GET(ifindex)\
        _SHR_PORT_PARENT_IFINDEX_GET(ifindex)
#define IFM_TRUNK_ID_GET(ifindex)\
        _SHR_PORT_ID_GET(ifindex)
#define IFM_TRUNK_IFINDEX_GET(trunkid)\
        _SHR_PORT_TRUNK_IFINDEX_GET(trunkid)
#define IFM_VLANIF_IFINDEX_GET(vlanid)\
        _SHR_PORT_VLANIF_IFINDEX_GET(vlanid)
#define IFM_VLANIF_ID_GET(ifindex)\
        _SHR_PORT_SUBPORT_ID_GET(ifindex)
#define IFM_SUBIFINDEX_GET(ifindex, vlanid)\
		_IFM_SUBIFINDEX_GET(ifindex, vlanid)


/* IFM Common descriptions. */
#define CLI_INTERFACE_STR                           "Specify the interface configuration view\n"
#define CLI_INTERFACE_ETHERNET_STR                  "Ethernet interface\n"
#define CLI_INTERFACE_ETHERNET_VHELP_STR            "The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
#define CLI_INTERFACE_TDM_STR                       "TDM interface\n"
#define CLI_INTERFACE_TDM_VHELP_STR                 "The port/subport of tdm, format: <0-7>/<0-31>/<1-255>[.<1-31>]\n"
#define CLI_INTERFACE_STM_STR                       "STM interface\n"
#define CLI_INTERFACE_STM_VHELP_STR                 "The port/subport of stm, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
#define CLI_INTERFACE_TUNNEL_STR                    "Tunnel interface\n"
#define CLI_INTERFACE_TUNNEL_VHELP_STR              "The port of tunnel, HT201 format: <0-7>/<0-31>/<1-128> HT2200 format: <0-7>/<0-31>/<1-2000>\n"
#define CLI_INTERFACE_LOOPBACK_STR                  "LoopBack interface\n"
#define CLI_INTERFACE_LOOPBACK_VHELP_STR            "LoopBack interface number\n"
#define CLI_INTERFACE_TRUNK_STR                     "Trunk interface\n"
#define CLI_INTERFACE_TRUNK_VHELP_STR               "The port/subport of trunk, format: <1-128>[.<1-4095>]\n"
#define CLI_INTERFACE_VLANIF_STR                    "Vlan interface\n"
#define CLI_INTERFACE_VLANIF_VHELP_STR              "VLAN interface number\n"
#define CLI_INTERFACE_CLOCK_STR                     "Clock interface\n"
#define CLI_INTERFACE_CLOCK_VHELP_STR               "Clock interface number\n"
#define CLI_INTERFACE_SFP_STR                       "SFP interface\n"
#define CLI_INTERFACE_SFP_VHELP_STR 			    "Small form factor pluggable transceiver\n"
#define CLI_INTERFACE_GIGABIT_ETHERNET_STR          "Gigabit Ethernet interface\n"
#define CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR    "The port/subport of gigabit ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
#define CLI_INTERFACE_XGIGABIT_ETHERNET_STR          "10Gigabit Ethernet interface\n"
#define CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR    "The port/subport of 10gigabit ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
#define CLI_INTERFACE_VCG_STR                        "VCG interface\n"
#define CLI_INTERFACE_VCG_VHELP_STR                  "The port of VCG, format: <0-7>/<0-31>/<1-255>\n"
#define CLI_INTERFACE_E1_STR                         "Controller E1\n"
#define CLI_INTERFACE_E1_VHELP_STR                   "The port of Controller E1, format: <0-7>/<0-31>/<1-255>\n"

#define IS_DIGIT(ch)  (((ch) >= '0') && ((ch) <= '9'))


#define GET_DIGIT_RANGE(V,STR,MIN,MAX, ERRO) \
do { \
   char *endptr = NULL; \
   unsigned long tmpl; \
   ERRO = 0;\
  (tmpl) = strtoul ((STR), &endptr, 10); \
     if (*(STR) == '-' || *endptr != '\0' || ERRO) \
    { \
      ERRO = -1; \
    } \
    else if ( ((tmpl) <= (MIN) && (tmpl) != (MIN)) || (tmpl) > (MAX) ) \
    { \
    ERRO = -1;\
    }   \
    else\
    {\
    (V) = tmpl;\
     ERRO = 0;\
    }\
} while (0)


extern struct cmd_node physical_if_node;
extern struct cmd_node physical_subif_node;
extern struct cmd_node tdm_if_node;
extern struct cmd_node tdm_subif_node;
extern struct cmd_node stm_if_node;
extern struct cmd_node stm_subif_node;
extern struct cmd_node loopback_if_node;
extern struct cmd_node tunnel_if_node;
extern struct cmd_node trunk_if_node;
extern struct cmd_node trunk_subif_node;
extern struct cmd_node vlanif_node;
extern struct cmd_node clockif_node;
extern struct cmd_node vcgif_node;
extern struct cmd_node e1if_node;

extern struct cmd_element physical_ethernet_if_common_cmd;
extern struct cmd_element physical_gigabit_ethernet_if_common_cmd;
extern struct cmd_element physical_xgigabit_ethernet_if_common_cmd;
extern struct cmd_element physical_tdm_if_common_cmd;
extern struct cmd_element physical_stm_if_common_cmd;
extern struct cmd_element tunnel_if_common_cmd;
extern struct cmd_element loopback_if_common_cmd;
extern struct cmd_element trunk_if_common_cmd;
extern struct cmd_element vlanif_if_common_cmd;
extern struct cmd_element clock_if_common_cmd;
extern struct cmd_element vcg_if_common_cmd;
extern struct cmd_element e1_if_common_cmd;

/* 由接口类型枚举值获取接口类型字符串*/

const char *ifm_get_typestr ( enum IFNET_TYPE type );


/* 由接口类型字符串获取接口类型枚举值*/

int ifm_get_typenum ( char *type_str );

/* 返回 0 为无效索引 */

uint32_t ifm_get_ifindex_by_usp ( struct ifm_usp *usp );

/*通过 ifindex 获取 usp */
int ifm_get_usp_by_ifindex ( uint32_t ifindex, struct ifm_usp *pusp );

/* 通过名字获取索引返回 0 为无效索引 */

uint32_t ifm_get_ifindex_by_name ( const char *type, const char *ifname );

uint32_t ifm_get_ifindex_by_name2 ( char *type_ifname );


/*通过 ifindex 获取 接口字符串，字符串形式如vlanif 1*/
int ifm_get_name_by_ifindex ( uint32_t ifindex, char *name );

/* 获取网管端口 ifindex */
uint32_t ifm_get_nm_port ( void );



/* 注册所有的接口*/

void ifm_common_init ( int ( *config_write ) ( struct vty *vty ) );

/* 注册支持 L2 模式的接口*/

void ifm_l2if_init ( int ( *config_write ) ( struct vty *vty ) );

/* 注册支持 L3 模式的接口*/

void ifm_l3if_init ( int ( *config_write ) ( struct vty *vty ) );

/* 注册以太物理接口*/
void ifm_port_init ( int ( *config_write ) ( struct vty *vty ) );

/* 注册 CES 接口*/
void ifm_ces_init ( int ( *config_write ) ( struct vty *vty ) );

/* 注册 STM 接口*/
void ifm_stm_init( int ( *config_write) ( struct vty *vty));

/* 注册 controller E1 接口*/
void ifm_cotroller_e1_init ( int ( *config_write ) ( struct vty *vty ) );

/* 注册接口事件 */
int ifm_event_register ( enum IFNET_EVENT type, int module_id, enum IFNET_EVENT_IFTYPE iftype );
int no_ifm_event_register ( enum IFNET_EVENT type, int module_id, enum IFNET_EVENT_IFTYPE iftype );

int ifm_unset_subif_l2 ( uint32_t ifindex , int module_id );


/*注册指定接口事件*/
int ifm_event_register_with_ifindex ( enum IFNET_EVENT type, int module_id, u_int32 ifindex );

/* 获取接口的 IP 和 VPN */

int ifm_get_l3if ( uint32_t ifindex, int module_id, struct ifm_l3 *l3);

/* 设置接口 IP 地址 */
int ifm_set_l3if ( uint32_t ifindex, struct ifm_l3 *pl3if, enum IFNET_EVENT subtype, int module_id );

/* 获取接口的 MAC */

int ifm_get_mac ( uint32_t ifindex, int module_id, uchar *pmac );


/* 获取接口的 link 状态 */

int ifm_get_link ( uint32_t ifindex, int module_id,  uint8_t *pdata);


/* 获取接口的 mode  */

int ifm_get_mode ( uint32_t ifindex, int module_id, uint8_t *pdata);

/*获取接口全部信息 struct ifm_info*/
int ifm_get_all_info ( uint32_t ifindex, int module_id, struct ifm_info *pdata);

/*获取接口全部信息 struct ifm_port*/
int ifm_get_port_info ( uint32_t ifindex, int module_id, struct ifm_port *pdata);

/* 批量返回 ifindex 后面的 *pdata_num 个接口信息，返回值是数组的指针，*pdata_num 返回实际的接口数量 */

void *ifm_get_bulk ( uint32_t ifindex, int module_id, int *pdata_num );

/* 批量返回 ifindex 后面的 *pdata_num 个接口信息，返回值是数组的指针，*pdata_num 返回实际的接口数量*/

struct ifm_counter_bulk *ifm_get_count_bulk(uint32_t ifindex, int module_id, int *pdata_num);

/* 批量返回 ifindex 后面的 100 个接口信息，返回值是数组的指针，*pdata_num 返回实际的接口数量 */

struct ifm_sfp_info *ifm_get_sfp_bulk ( uint32_t ifindex, int module_id, int *pdata_num );

/* 批量返回 ifindex 后面的 100 个 l3 接口信息，返回值是数组的指针，*pdata_num 返回实际的接口数量 */

struct ifm_l3 *ifm_get_l3if_bulk ( uint32_t ifindex, int module_id, int *pdata_num );


/* 获取接口的 mtu  */

int ifm_get_mtu ( uint32_t ifindex, int module_id, uint16_t *pmtu);

/*设置接口shutdown*/
int ifm_set_enable ( uint32_t ifindex, int module_id, uint8_t enable );

/*设置接口block*/
int ifm_set_block ( uint32_t ifindex, int module_id, uint8_t status );

/* 传入物理接口的 ifindex 和 vlan，创建 l3 子接口 */

int ifm_set_subif ( uint32_t ifindex, int svlan, int cvlan, int module_id );

/* 传入物理接口的 ifindex 和 vlan，创建 l2 子接口 */
int ifm_set_subif_l2 ( uint32_t ifindex, int svlan, int cvlan, int module_id );

/* snmp查询qos mapping应用接口 */
struct qos_entry_snmp *ifm_get_qos_bulk(uint32_t ifindex, int module_id, int*pdata_num);

/*snmp查询用接口数量*/

int  ifm_get_port_count (int module_id, uint32_t *pdata);

void* ifm_get_sfp_if_bulk( uint32_t ifindex, int module_id, int * pdata_num );

void * ifm_get_subencap_bulk( uint32_t ifindex, int module_id, int * pdata_num );

void * l2if_get_storm_suppress_bulk( uint32_t ifindex, int module_id, int *pdata_num );

void * l2if_get_dot1q_tunnel_bulk( uint32_t ifindex, int module_id, int *pdata_num );

void * ifm_get_statistics_bulk(uint32_t ifindex,int module_id,int* pdata_num);

void * ifm_get_speed_bulk(uint32_t * ifindex,int module_id, int *pdata_num);


#endif
