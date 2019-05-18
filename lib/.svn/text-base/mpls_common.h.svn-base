/**
 * @file      : mpls_common.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 15:55:55
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_MPLS_COM_H
#define HIOS_MPLS_COM_H

#include <lib/types.h>
#include <lib/inet_ip.h>
#include <lib/ptree.h>
#include <lib/linklist.h>
#include <lib/route_com.h>
#include <lib/qos_common.h>
#include <lib/hptimer.h>
#include <lib/msg_ipc_n.h>

/* add for mpls debug level */
#define MPLS_DEBUG_LSP                      0
#define MPLS_DEBUG_PW                       1
#define MPLS_DEBUG_TUNNEL                   2
#define MPLS_DEBUG_VPLS                     3
#define MPLS_DEBUG_L3VPN                    4
#define MPLS_IPC_MSG_TIMEOUT_INTERNAL           5000
/* added for ipc share memory */
#define MPLS_TIMER_DEL(timerid)         if(timerid)\
                                        {\
                                            high_pre_timer_delete(timerid);\
                                            timerid = 0;\
                                        }

#define MPLS_TIMER_ADD(pFunc, pArg, Sec)      high_pre_timer_add("MplsTimer", LIB_TIMER_TYPE_NOLOOP, pFunc, pArg, ((Sec) * 1000))

extern int mpls_ipc_msg_send(uint8_t *pdata, int data_len, int data_num, int module_src, int module_dst, int ipc_type, uint8_t opcode, uint16_t sub_opcode, int msg_index);


#define MPLS_IPC_SENDTO_FTM(pdata, data_len, data_num, module_src, ipc_type, opcode, sub_opcode, msg_index)   mpls_ipc_msg_send((uint8_t *)pdata, data_len, data_num, module_src, MODULE_ID_FTM, ipc_type, opcode, sub_opcode, msg_index)
#define MPLS_IPC_SENDTO_HAL(pdata, data_len, data_num, module_src, ipc_type, opcode, sub_opcode, msg_index)   mpls_ipc_msg_send((uint8_t *)pdata, data_len, data_num, module_src, MODULE_ID_HAL, ipc_type, opcode, sub_opcode, msg_index)

extern int mpls_ipc_send_hal_wait_ack(void *pdata, int data_len, int data_num, int sender_id,
                     int msg_type, uint8_t opcode, uint8_t subtype,  int msg_index);

/******************************/

/*mpls debug type*/
#define MPLS_DBG_BFD	(0x1U<<0)
#define MPLS_DBG_OAM	(0x1U<<1)
#define MPLS_DBG_LSP	(0x1U<<2)
#define MPLS_DBG_L2VC	(0x1U<<3)
#define MPLS_DBG_VPLS	(0x1U<<4)
#define MPLS_DBG_TUNNEL	(0x1U<<5)
#define MPLS_DBG_L3VPN	(0x1U<<6)
#define MPLS_DBG_LDP	(0x1U<<7)
#define MPLS_DBG_PING	(0x1U<<8)


#define MPLS_DEBUG_COMMON(format, ...)   do{\
                                                            FILE *debug_fp = fopen("/data/dat/mpls_debug.txt", "a");\
                                                            fprintf(debug_fp, ">%d %s %s "format"\n", __LINE__, __FILE__, __func__, ##__VA_ARGS__);\
                                                            fclose(debug_fp);\
                                                         }while(0);


#define PW_NUM_MAX          1024        // pw 的最大数量
#define VSI_NUM_MAX         1024        // vsi 的最大数量
#define LSP_NUM_MAX         (4096*4)    // lsp 总数，包括静态和动态 LSP
#define LSP_STATIC_NUM_MAX  1024        // 静态 LSP 数量
#define L3VPN_NUM_MAX       1024        // l3vpn 最大数量
#define VSI_MAC_LIST_MAX    128         // vsi mac 黑/白名单最大数量

#define CAR_VALUE_L1_FLAG   (0x80000000U)   // include interframe

/* PW 封装方式 */
#define PW_ENCAP_ETHERNET_RAW   0
#define PW_ENCAP_ETHERNET_TAG   1
#define PW_ENCAP_VLAN_RAW       2
#define PW_ENCAP_VLAN_TAG       3

/* VPLS 封装方式 */
#define VSI_ENCAP_ETHERNET_RAW  0
#define VSI_ENCAP_ETHERNET_TAG  1
#define VSI_ENCAP_VLAN_RAW      2
#define VSI_ENCAP_VLAN_TAG      3

#define BFD_TYPE_MASTER      1
#define BFD_TYPE_BACKUP      2
#define BFD_FLAG_ENABLE      1
#define BFD_FLAG_DISABLE     0
#define BFD_WTR_DEFAULT      3


/* MPLS 获取消息子类型 */
enum MPLS_GET_SUBTYPE
{
    MPLS_GET_INVALID          = 0,
    MPLS_GET_LSR_ID           = 1,
    MPLS_GET_STATIC_LSP_BULK  = 2,
    MPLS_GET_L2VC_BULK        = 3,
    MPLS_GET_L2VC_COUNTER     = 4,
    MPLS_GET_TUNNEL           = 5,
    MPLS_GET_TUNNEL_BULK      = 6,
    MPLS_GET_TUNNEL_COUNTER   = 7,
    MPLS_GET_VSI_BULK         = 8,
    MPLS_GET_VSIIF_BULK       = 9,
    MPLS_GET_PW_HQOS_BULK     = 10,
    MPLS_GET_TUNNEL_HQOS_BULK = 11,
    MPLS_GET_LDP_PREFIX_BULK  = 12,
    MPLS_GET_LDP_LOCALIF_BULK = 13,
    MPLS_GET_LDP_REMOT_BULK   = 14,
    MPLS_GET_LDP_CONF_GLOBAL  = 15,
    MPLS_GET_LDP_SESS_BULK    = 16,
    MPLS_GET_LDP_LSP_BULK     = 17,
    MPLS_GET_PW_CLASS_BULK    = 18,
    MPLS_GET_SERV_INS_BULK    = 19,
    MPLS_GET_XC_GROUP_BULK    = 20,
    MPLS_GET_MPLS_SERV_BULK   = 21,
    MPLS_GET_VSI_WHITEMAC_BULK= 22,
    MPLS_GET_VSI_BLACKMAC_BULK= 23,
    MPLS_GET_VSI_SLAVE_PW_BULK= 24,
    MPLS_GET_LABEL_POOL_BULK  = 25,
    MPLS_GET_VSI_STATIC_MAC_BULK= 26,
};


/* PW 类型 */
enum PW_TYPE_E
{
    PW_TYPE_MASTER = 0,
    PW_TYPE_BACKUP,
    PW_TYPE_MAX = 8
};


/* AC 侧的类型 */
enum VC_TYPE_E
{
    AC_TYPE_INVALID = 0,
    AC_TYPE_ETH,
    AC_TYPE_TDM,
    AC_TYPE_STM,
    AC_TYPE_MAX = 8
};


/* PW 的协议类型 */
enum PW_PROTO_E
{
    PW_PROTO_INVALID = 0,
    PW_PROTO_SVC,
    PW_PROTO_MARTINI,
    PW_PROTO_KOMPELLA,
    PW_PROTO_MAX = 8
};


/* vpls pw 的类型 */
enum VPLS_TYPE
{
    VPLS_TYPE_SPE = 0,
    VPLS_TYPE_UPE,
};


/* 标签转发动作 */
enum LABEL_ACTION
{
    LABEL_ACTION_NOACTION = 0,
    LABEL_ACTION_POP,
    LABEL_ACTION_PUSH,
    LABEL_ACTION_SWAP,
    LABEL_ACTION_GO,
    LABEL_ACTION_POPGO,
    LABEL_ACTION_MAX = 8
};


/* lsp 方向 */
enum LSP_DIRECTION
{
    LSP_DIRECTION_INGRESS = 0,
    LSP_DIRECTION_EGRESS,
    LSP_DIRECTION_TRANSIT,
    LSP_DIRECTION_MAX = 8
};


/* lsp 类型 */
enum LSP_TYPE
{
    LSP_TYPE_INVALID = 0,
    LSP_TYPE_L2VC,          // pw
    LSP_TYPE_L3VPN,
    LSP_TYPE_STATIC,        // static lsp
    LSP_TYPE_LDP,           // ldp lsp
    LSP_TYPE_RSVPTE,
    LSP_TYPE_RESERVED,      // 保留标签
    LSP_TYPE_MAX = 10
};

struct mpls_smac
{
    uint32_t vplsid;
    uint32_t mac[MAC_LEN];
    uint32_t type;
    uchar    ifname[NAME_STRING_LEN];
    uint16_t pwid;
    uint8_t  pad[2];
};

struct mpls_mac
{
    uint32_t vplsid;
    uint32_t type;
    uchar    mac[MAC_LEN];
    uint8_t  pad[2];
};

/* ces pw 的参数 */
struct ces_pw
{
    uint8_t  rtp;           // 1: rtp 使能，0 : rtp 禁止
    uint8_t  frame_num;     // pw 中封装的 ces 帧数
    uint16_t jitbuffer;     // jitter buffer 值
};


/* VPLS 的参数 */
struct vpls_info
{
    uint16_t vsi_id;
    uint8_t  mac_learn;     // 0: disable, 1: enable
    uint8_t  upe;           // 0: spe, 1: upe
	int      learning_limit;// set mac-learning limit value
    uint16_t tpid;          // tag 模式添加的 vlan tpid
    uint16_t vlan;          // tag 模式添加的 vlan
    uint8_t  tag_flag;      // 0: ethernet RAW, 1: ethernet TAG, 2: vlan RAW, 3: vlan TAG
    uint8_t  pad[3];
    uint32_t car_cir;       // QOS car CIR
    uint32_t car_pir;       // QOS car PIR
	uint32_t uc_cir;        // 单播承诺信息速率
    uint32_t bc_cir;	    // 广播
    uint32_t mc_cir;	    // 多播
    uint32_t uc_cbs;	    // 单播承诺突发速率
    uint32_t mc_cbs;	    // 多播
    uint32_t bc_cbs;	    // 广播
};


/* VSI 数据结构 */
struct vsi_entry
{
    uchar             name[NAME_STRING_LEN];
    struct vpls_info  vpls;
    enum FAILBACK_E   failback;     // pw 保护是否回切
    uint16_t          wtr;          // pw 保护回切时间，默认 30s
    uint8_t           pad[2];
    struct list      *pwlist;       // listnode 存储 struct l2vc_entry 结构指针
    struct list      *aclist;       // listnode 存储 ac 侧的 ifindex
    struct list      *blacklist;    // listnode 存储 MAC 学习黑名单
    struct list      *whitelist;    // listnode 存储 MAC 学习白名单
};


/* pw 的公共数据结构 */
struct pw_info
{
    uchar             name[NAME_STRING_LEN];
    uint32_t          pwindex;
    uint32_t          ifindex;      // ac 侧接口
    uint32_t          vcid;
    uint32_t          ingress_lsp;  // 私网 lsp
    uint32_t          egress_lsp;   // 私网 lsp
    uint32_t          nhp_index;    // 公网 nhp
    uint32_t          mspw_index;   // 交换 pw 的索引
    uint32_t          backup_index; // backup pw 的索引
    uint16_t          mtu;
    uint16_t          tpid;         // tag 模式添加的 vlan tpid
    uint16_t          vlan;         // tag 模式添加的 vlan
    uint8_t           tag_flag;     // 0: ethernet RAW, 1: ethernet TAG, 2: vlan RAW, 3: vlan TAG
    uint8_t           ctrlword_flag;// 0: no control word, 1: control word
    enum PW_PROTO_E   protocol;
    enum PW_TYPE_E    pw_type;
    enum VC_TYPE_E    ac_type;
    enum NHP_TYPE     nhp_type;
    uint8_t           up_flag;      // pw link status, 1: down, 0:up
    uint8_t           admin_up;     // pw admin status, 1: down, 0:up
    uint8_t            status;      // 1: enable, 0: disable
	uint8_t			  sample_interval;	//pw带宽采样间隔
    uint8_t           pad;
    struct ces_pw     ces;          // ces pw 参数
    struct vpls_info  vpls;         // vpls 参数
    struct counter_t *pcouter;      // pw 统计
};


/* L2VPN 数据结构 */
struct l2vc_entry
{
    uchar    name[NAME_STRING_LEN];
    uchar    lsp_name[NAME_STRING_LEN];
    uint8_t  ldp_tunnel;            // 1: 选择 ldp tunnel
    uint8_t  mplste_tunnel;         // 1: 选择 mpls-te tunnel
    uint8_t  mplstp_tunnel;         // 1: 选择 mpls-tp tunnel
    uint8_t  gre_tunnel;            // 1: 选择 gre tunnel
    uint8_t  lsp_tunnel;            // 1: 选择 静态 lsp tunnel
    uint8_t  ac_flag;               // 1: ac down，0: ac up
    uint8_t  domain_id;             // QOS domain，默认是 domain 0
    uint8_t  phb_id;                // QOS phb，默认是 phb 0
    uint8_t  phb_enable;            // 0 表示 exp copy，1 表示 phb 有效
    uint8_t  statis_enable;         // 1: 表示统计使能
    uint32_t tunl_index;            // 静态绑定的 tunnel ifindex
    uint32_t inlabel;               // 静态 PW 的入标签
    uint32_t outlabel;              // 静态 PW 的出标签
    uint16_t mplsoam_id;            // mplstp oam session id
    uint16_t bfd_id;                // bfd session id
    uint16_t aps_id;                // aps session id
    uint16_t wtr;                   // pw 保护回切时间，默认 30s
    uint16_t hqos_id;               // HQOS queue id
    uint32_t car_cir[2];            // QOS car CIR: 0:ingress, 1:egress
    uint32_t car_pir[2];            // QOS car PIR: 0:ingress, 1:egress
    enum BACKUP_E      pw_backup;   // 1: 主 pw 工作, 2: 备 pw 工作
    enum FAILBACK_E    failback;    // pw 保护是否回切
    struct inet_addr   peerip;      // peer ip
    struct l2vc_entry *pswitch_pw;  // 交换 pw 的后段
    struct l2vc_entry *ppw_backup;  // 备份 pw
    TIMERID            pwtr_timer;  // wtr 定时器
    struct list       *mac_list;    // vpls pw 静态 MAC
    struct pw_info     pwinfo;      // 必须放在最后
};


/* 静态 lsp 数据结构 */
struct static_lsp
{
    uchar              name[NAME_STRING_LEN];
    uint32_t           lsp_index;
    struct inet_prefix destip;
    struct inet_addr   nexthop;
    uint32_t           inlabel;
    uint32_t           outlabel;
    uint32_t           nhp_index;   // 出接口索引
    uint32_t           group_index; // tunnel ifindex
    enum LSP_DIRECTION direction;
    enum LSP_TYPE      lsp_type;    // lsp 类型: static 类型
    enum NHP_TYPE      nhp_type;    // 出接口类型，支持 connect、tunnel、lsp、frr、ecmp
    uint32_t		   oam_nhp_index;//出接口索引用于支持oam告警正常发送
    uint16_t           mplsoam_id;  // mplstp oam session id
    uint16_t           bfd_id;      // bfd session id
    uint16_t           aps_id;      // aps session id
    uint8_t            pad;
    uint8_t            status;      // 1: enable, 0: disable
    uint8_t            down_flag;   // lsp link status, 1: down, 0 : up
    uint8_t            admin_down;  // lsp admin status, 1: down, 0 : up
    uchar              dmac[MAC_LEN];
};


/* LSP 数据结构 */
struct lsp_entry
{
    uchar              name[NAME_STRING_LEN];
    uint32_t           lsp_index;
    struct inet_prefix destip;
    struct inet_addr   nexthop;
    uint32_t           inlabel;
    uint32_t           outlabel;
    uint32_t           ac_index;        // ac ifindex for pw
    uint32_t           pw_index;
    uint32_t           group_index;     // tunnel ifindex
    uint32_t           nhp_index;       // 出接口
    enum NHP_TYPE      nhp_type;
    enum LSP_TYPE      lsp_type;
    enum LSP_DIRECTION direction;
    uchar              dmac[6];
	uint8_t            ttl;             // 默认值是 255
	uint8_t            exp;             // 默认值是 1
    uint16_t           vpnid;           // for l3vpn and vpls
    uint8_t            domain_id;       // QOS domain，默认是 domain 0
    uint8_t            phb_id;          // QOS phb，默认是 phb 0
    uint8_t            phb_enable;      // 0 表示 exp copy，1 表示 phb 有效
    uint8_t            statis_enable;   // 1: 表示统计使能
    uint8_t            down_flag;       // lsp link status, 1: down, 0 : up
    uint8_t            ttl_copy;        // 1: copy 上一级的 ttl，默认是 0
    uint16_t           mplsoam_id;      // mplstp oam session id
    uint16_t           bfd_id;          // bfd session id
    uint32_t           car_cir[2];      // QOS car CIR: 0:ingress, 1:egress
    uint32_t           car_pir[2];      // QOS car PIR: 0:ingress, 1:egress
    uint16_t           hqos_id;         // HQOS queue id
    uint8_t            pad[2];
    struct counter_t  *pcouter;         // lsp 统计
    struct list       *pwlist;          // lsp 上承载的 pw 链表, listnode 存放 pw 的指针
};


/* tunnel 封装协议 */
enum TUNNEL_PROTO
{
    TUNNEL_PRO_INVALID = 0,
    TUNNEL_PRO_MPLSTP,
    TUNNEL_PRO_MPLSTE,
    TUNNEL_PRO_GRE,
    TUNNEL_PRO_MAX = 8
};


/* tunnel 保护状态 */
enum TUNNEL_STATUS
{
    TUNNEL_STATUS_INVALID = 0,  // invalid
    TUNNEL_STATUS_MASTER,       // 主 lsp
    TUNNEL_STATUS_BACKUP,       // backup lsp
    TUNNEL_STATUS_FRR,          // frr lsp
};


/* tunnel 的公共数据结构 */
struct tunnel_t
{
    uint32_t           ifindex;
    enum TUNNEL_PROTO  protocol;            // tunnel 封装协议
    enum TUNNEL_STATUS backup_status;       // 0: 主工作, 1: 备工作, 2: frr 工作
	enum FAILBACK_E    failback;            // lsp 保护是否回切
    struct inet_addr   sip;
    struct inet_addr   dip;
    uint32_t           master_index;        // master tunnel
    uint32_t           backup_index;        // backup tunnel
    uint32_t           master_egress_index; // egress lsp only for mpls-tp tunnel
    uint32_t           backup_egress_index; // egress lsp only for mpls-tp tunnel
    uint32_t           frr_index;           // frr tunnel
    uint32_t           car_cir[2];          // QOS car CIR: 0:ingress, 1:egress
    uint32_t           car_pir[2];          // QOS car PIR: 0:ingress, 1:egress
    uint32_t           car_cir_sum[2];      // tunnel QOS car 0:ingress cir limit，2:egress cir limit
    uint16_t           hqos_id;             // HQOS queue id
	uint16_t           wtr;                 // lsp 保护回切时间，默认 30s
    uint16_t           mplsoam_id;          // mplstp oam session id
    uint16_t           bfd_id;              // bfd session id
    uint16_t           aps_id; 		        // aps session id
    uint8_t            domain_id;           // QOS domain，默认是 domain 0
    uint8_t            phb_id;              // QOS phb，默认是 phb 0
    uint8_t            phb_enable;          // 0 表示 exp copy，1 表示 phb 有效
    uint8_t            down_flag;           // 0: linkup, 1: linkdown
    uint8_t            statis_enable;       // 1: 表示统计使能
    uint8_t            pad[3];
	uint8_t            dmac[6];             // gre tunnel 的 dmac
    uint32_t           outif;               // gre tunnel 的出接口 ifindex
    uint32_t           nexthop;             // gre tunnel 的下一跳
};


/* mplstp tunnel 数据结构 */
struct tunnel_mplstp
{
    struct static_lsp *ingress_lsp;         // ingress lsp
    struct static_lsp *egress_lsp;          // egress lsp
    struct static_lsp *backup_ingress_lsp;  // backup ingress lsp
    struct static_lsp *backup_egress_lsp;   // backup egress lsp
};


/* tunnel 接口 */
struct tunnel_if
{
    struct tunnel_t       tunnel;
	uchar inlsp_name[NAME_STRING_LEN];      	// ingress lsp name
	uchar elsp_name[NAME_STRING_LEN];           // egress lsp name
	uchar backup_inlsp_name[NAME_STRING_LEN];   // backup ingress lsp name
	uchar backup_elsp_name[NAME_STRING_LEN];    // backup egress lsp name
    uint32_t              set_flag;             // outif 下发标志
    struct tunnel_mplstp *p_mplstp;             // mplstp tunnel 数据结构
    void                 *p_mplste;             // mpls-te tunnel
    struct list          *pwlist;               // tunnel 接口绑定的所有 pw，保存 struct pw_info 指针
    struct counter_t     *pcouter;              // tunnel 统计
	TIMERID               pwtr_timer;	        // wtr 定时器
};


#define LABEL_APPLY_PER_VPN     0
#define LABEL_APPLY_PER_ROUTE   1
#define L3VPN_TARGET_IMPORT_MAX 4096
#define L3VPN_TARGET_EXPORT_MAX 500
#define L3VPN_NHP_IS_NOT_IMPORT 0
#define L3VPN_NHP_IS_IMPORT     1


/* l3vpn 消息子类型 */
enum L3VPN_SUBTYPE
{
    L3VPN_SUBTYPE_LABEL = 0,
    L3VPN_SUBTYPE_INSTANCE,
    L3VPN_SUBTYPE_RD,
    L3VPN_SUBTYPE_IMPORT,
    L3VPN_SUBTYPE_EXPORT,
    L3VPN_SUBTYPE_TARGET,
    L3VPN_SUBTYPE_TUNNEL,
    L3VPN_SUBTYPE_APPLY,
    L3VPN_SUBTYPE_MAX = 10,
};


/* 私网路由下一跳信息 */
struct l3vpn_nhp
{
    uint32_t            inlabel;        // 私网路由入标签
    uint32_t            outlabel;       // 私网路由出标签
    uint32_t            lsp_index;      // 标签生成的 lsp 索引
    struct inet_addr    nexthop;        // 私网路由下一跳
    enum ROUTE_PROTO    protocol;       // 私网路由协议类型
    uint32_t            tunnel_if;      // 私网路由指向的隧道索引
    uint32_t            cost;           // 私网路由 cost
    uint8_t             distance;       // 私网路由优先级
    uint8_t             import_flag;    // 私网下一跳引入标志，0: 非引入，1: 引入路由
    uint8_t             down_flag;      // 私网下一跳状态
    uint8_t             pad;
};


/* 私网路由信息 */
struct l3vpn_route
{
    struct inet_prefix  prefix;         // 私网路由前缀
    uint16_t            vrf_id;         // 私网路由vpn id
    uint8_t             pad[2];
    enum LSP_TYPE       type;           // 生成私网 lsp 的类型
    struct l3vpn_nhp    nhp;            // 私网路由下一跳信息
    struct list         nhplist;        // 私网路由下一跳链表，存储 struct route_nhp
};


/* l3vpn 需要下发的信息 */
struct l3vpn_info
{
    uint16_t            vrf_id;         // vrf 标识
    uint8_t             tunnel_type;    // 隧道类型
    uint8_t             pad;
};


/* l3vpn 数据结构 */
struct l3vpn_entry
{
    struct l3vpn_info   l3vpn;
    uchar               name[NAME_STRING_LEN];  // vpn 实例名
    uchar               rd[NAME_STRING_LEN];    // RD
    uint32_t            inlabel;                // label_apply 为 vpn 时有效，vpn 标签值
    uint32_t            lsp_index;              // label_apply 为 vpn 时有效，vpn 私网 lsp 索引
    uint8_t             label_apply;            // 标签应用方式，默认为每条路由一个标签
    uint8_t             pad[3];
    struct ptree        route_tree;             // 路由信息树，存储 struct l3vpn_route
    struct list         import_list;            // import 属性链表，存储 struct prefix_rd
    struct list         export_list;            // export 属性链表，存储 struct prefix_rd
};


/* snmp 查询 l3vpn target 结构 */
struct l3vpn_target
{
    uint32_t           vrf_id;                  // vrf 标识
    uchar              target[NAME_STRING_LEN]; // target 内容
    enum L3VPN_SUBTYPE target_type;             // target 类型
};


struct hqos_pw
{
	uchar name[NAME_STRING_LEN];
	struct hqos_t hqos;
};


struct hqos_tunnel
{
	uint32_t ifindex;
	struct hqos_t hqos;
};


extern uint32_t *mpls_com_get_label(uint16_t vrf_id, int module_id);
extern int mpls_com_label_free(uint16_t vrf_id, uint32_t label, int module_id);
extern struct l3vpn_entry *mpls_com_get_l3vpn_instance(uint16_t vrf_id, int module_id);
extern uint16_t mpls_com_get_l3vpn_instance2(uint16_t vrf_id, int module_id);

extern int mpls_com_l3vpn_event_register(int module_id);
extern struct l3vpn_entry *mpls_com_get_l3vpn_bulk(int vrf_id, int module_id, int *pdata_num);
extern struct l3vpn_target *mpls_com_get_l3vpn_target_bulk(struct l3vpn_target *pl3target,
                                                                    int module_id, int *pdata_num);
extern uint32_t *mpls_com_get_lsr_id(int module_id);
extern struct static_lsp *mpls_com_get_slsp_bulk(uchar *plspname, int module_id, int *pdata_num);
extern struct l2vc_entry *mpls_com_get_l2vc_bulk(uchar *ppwname, int module_id, int *pdata_num);
extern struct vsi_entry *mpls_com_get_vsi_bulk(uint32_t vsi_id, int module_id, int *pdata_num);
extern uint32_t *mpls_com_get_vsiif_bulk(uint32_t vsi_id, uint32_t ifindex, int module_id,
                                                int *pdata_num);
extern struct tunnel_if *mpls_com_get_tunnel_bulk(uint32_t ifindex, int module_id, int *pdata_num);
extern struct tunnel_if *mpls_com_get_tunnel(uint32_t ifindex, int module_id);
extern struct counter_t *mpls_com_get_l2vc_counter(uchar *ppwname, int module_id);
extern struct counter_t *mpls_com_get_tunnel_counter(uint32_t ifindex, int module_id);
extern struct hqos_pw *mpls_com_get_pw_hqos_bulk(uchar *ppwname, int module_id, int *pdata_num);
struct hqos_tunnel *mpls_com_get_tunnel_hqos_bulk(uint32_t ifindex, int module_id, int *pdata_num);


/* LDP struct defie for mib */
struct ldp_ipprefix_mib
{
    uint32_t            key;
    struct inet_prefix  prefix;
};


struct ldp_config_global_mib
{
    uint32_t gldp_enable;
	uint32_t policy;
	uint32_t advertise;
	uint32_t control;
	uint32_t hold;
	uint16_t keepalive_internal;
	uint16_t hello_internal;
};


struct ldp_lsp_mib
{
	uint32_t lspindex;
    struct inet_prefix destip;
    uint32_t direction;
    struct inet_addr nexthop;
    uint32_t inlabel;
    uint32_t outlabel;
};


struct ldp_session_mib
{
	uint32_t peer_lsrid;
	uint32_t local_lsrid;
    uint32_t role;
    uint32_t type;
    uint32_t status;
    uint32_t sess_hold;
    long   creat_time;
};


struct ldp_localif_mib
{
    uint32_t ifindex;
    uint32_t ldp_enable;
};
struct ldp_remoteconf_mib
{
    uint32_t remote_peer;
};

struct ldp_ipprefix_mib *mpls_com_get_ldp_prefix_bulk(uint32_t prefix, uint32_t prelen, int module_id, int *pdata_num);
struct ldp_config_global_mib *mpls_com_get_ldp_global_conf( uint32_t local_lsrid, int module_id, int *pdata_num);
struct ldp_remoteconf_mib *mpls_com_get_ldp_remotconf_bulk(uint32_t peer_lsrid, int module_id, int *pdata_num);
struct ldp_localif_mib *mpls_com_get_ldp_localif_bulk(uint32_t ifindex, int module_id, int *pdata_num);
struct ldp_session_mib *mpls_com_get_ldp_sess_bulk(uint32_t peer_lsrid, int module_id, int *pdata_num);
struct ldp_lsp_mib *mpls_com_get_ldp_lsp_bulk(uint32_t lsp_index, int module_id, int *pdata_num);
struct mpls_smac *mpls_com_get_vpls_static_mac_bulk(struct mpls_smac *pindex, uint32_t data_len,
                                                int module_id, int *pdata_num);

struct ldp_frr_t
{
    uint32_t group_index;
    uint32_t master_lsp;
    uint32_t backup_lsp;
};




/* added for h3c cmdline */
#define CONNECTION_MAX_NUM          256
#define XC_GROUP_MAX_NUM            4096
#define PW_CLASS_MAX_NUM            128
#define SERVICE_INSTANCE_MAX_NUM    4096

#define UNI_FLAG            0x01
#define NNI_MASTER_FLAG     0x02
#define NNI_BACKUP_FLAG     0x04
#define NNI_SWITCH_FLAG     0x08



enum pw_type
{
    PW_TYPE_ETH,
    PW_TYPE_VLAN,
};

enum encap_type
{
    ENCAP_TYPE_INVALID,
    ENCAP_TYPE_VID,
    ENCAP_TYPE_DEFAULT,
    ENCAP_TYPE_TAGGED,
    ENCAP_TYPE_UNTAGGED,
};


enum access_mode
{
    ACCESS_MODE_VLAN,
    ACCESS_MODE_ETH,
};

struct pw_class
{
    char                name[NAME_STRING_LEN];
	char				bfd_temp[NAME_STRING_LEN];
    int                 pw_type;
    uint8_t             ctrlword;
	uint8_t				bfd_flag;
	uint8_t				bfd_chtype; /*default 0:with ip/udp header, 1:without ip/udp header*/
};


struct l2vpn_global
{
    uint8_t l2vpn_flag;
};


extern struct l2vpn_global g_l2vpn;

struct serv_ins
{
    uint32_t            ifindex;
    uint16_t            instance;
    uint16_t            vid;
    enum encap_type     en_type;
    void                *pcont;
};


struct uni_info
{
    uint32_t            ifindex;
    struct serv_ins     instance;
    enum access_mode    mode;
};


struct nni_info
{
    uint32_t            peer;
    uint32_t            pwid;
    uint32_t            inlabel;
    uint32_t            outlabel;
    uint16_t            bfd_id;
    struct pw_class     pw_class;
};

struct cont_entry
{
    char                name[NAME_STRING_LEN];
    char                master_name[NAME_STRING_LEN];
    char                backup_name[NAME_STRING_LEN];
    struct uni_info     uni;
    struct nni_info     nni_master;
    struct nni_info     nni_backup;
    struct nni_info     nni_switch;
    uint16_t            wtr;
    uint8_t             failback;
    uint8_t             flag;
};



struct xc_group
{
    char name[NAME_STRING_LEN];
    struct list cont_list;
};


/* add for h3c mpls mib */

struct serv_ins_mib
{
    uint32_t            ifindex;
    uint16_t            instance;
    uint16_t            vid;
    enum encap_type     en_type;
    struct cont_entry   connection;
};

struct xc_group_mib
{
    char name[NAME_STRING_LEN];
    uint32_t connection_num;
    struct cont_entry connection[];
};

#define VPLS_DYNAMIC_MAC_TYPE_AC        1
#define VPLS_DYNAMIC_MAC_TYPE_PW        2


struct vpls_dynamic_mac_mib_index
{
    uint32_t vsi_id;
    char mac[MAC_LEN];
};

struct vpls_dynamic_mac_mib_data
{
    uint32_t type;
    uint32_t addr_index;
    char pw_name[NAME_STRING_LEN];
    char ac_interface[NAME_STRING_LEN];
};

struct vpls_dynamic_mac_mib
{
    struct vpls_dynamic_mac_mib_index index;
    struct vpls_dynamic_mac_mib_data data;
};



/* add for mpls service */
/* add for mpls-service cmd */
extern struct hash_table mpls_service_table;

#define MPLS_SERVICE_TABLE_MAX      1024

typedef enum
{
  UNKNOW_SERVICE_TYPE = 0,
  E_LINE = 1,
  E_LAN =  2,
  E_TREE = 3
}mpls_service_type;

typedef enum
{
  MPLS_VPN_TYPE_VC = 0,
  MPLS_VPN_TYPE_VPLS = 1
}mpls_service_vpn_t;

struct mpls_service
{
    uint32_t service_id;
    char     name[NAME_STRING_LEN];
    uint32_t service_type;
    uint32_t vpn_type;
    uint32_t vpn_id;
    char root_port[NAME_STRING_LEN];
};

/* add for label pool mib */
struct label_info
{
    uint32_t label;
    uint32_t module_id;
    uint32_t state;
};

struct pw_class *mpls_com_get_pw_class_bulk(struct pw_class *pindex, uint32_t index_flag, int module_id, int *pdata_num);
struct serv_ins_mib *mpls_com_get_service_instance_bulk(struct serv_ins_mib *pindex, uint32_t index_flag,
                                                int module_id, int *pdata_num);
struct xc_group_mib *mpls_com_get_xc_group_bulk(struct xc_group_mib *pindex, uint32_t index_flag,
                                                int module_id, int *pdata_num);
struct mpls_service *mpls_com_get_mpls_serivce_bulk(struct mpls_service *pindex, uint32_t index_flag,
                                                int module_id, int *pdata_num);
struct mpls_mac *mpls_com_get_vpls_black_white_mac_bulk(struct mpls_mac *pindex, uint32_t data_len,
                                                int module_id, int *pdata_num);
struct l2vc_entry *mpls_com_get_vpls_pw_bulk(int vsi_id, struct l2vc_entry *pindex, uint32_t data_len,
                        int module_id, int *pdata_num);
struct label_info *mpls_com_get_mpls_label_pool_bulk(struct label_info *pindex, uint32_t index_flag,
                                                int module_id, int *pdata_num);

#endif

