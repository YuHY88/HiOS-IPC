/*
*       define qos common function and structure
*
*/


#ifndef HIOS_QOS_COM_H
#define HIOS_QOS_COM_H


#include <lib/types.h>
#include <lib/inet_ip.h>


#if 0
#define CPCAR_OUTBAND_ARP_PPS 	300
#define CPCAR_OUTBAND_IPV6_PPS 	300
#define CPCAR_OUTBAND_IP_PPS 	3000
#define CPCAR_OUTBAND_DHCP_PPS 	200
#define CPCAR_OUTBAND_ICMP_PPS 	200
#define CPCAR_OUTBAND_SNMP_PPS 	500
#define CPCAR_OUTBAND_SSH_PPS 	500
#define CPCAR_OUTBAND_TFTP_PPS 	500
#define CPCAR_OUTBAND_TELNET_PPS 500
#else

#define CPCAR_OUTBAND_ARP_PPS 	300
#define CPCAR_OUTBAND_IPV6_PPS 	1000
#define CPCAR_OUTBAND_IP_PPS 	9000
#define CPCAR_OUTBAND_DHCP_PPS 	200
#define CPCAR_OUTBAND_ICMP_PPS 	400
#define CPCAR_OUTBAND_SNMP_PPS 	1000
#define CPCAR_OUTBAND_SSH_PPS 	1000
#define CPCAR_OUTBAND_TFTP_PPS 	1500
#define CPCAR_OUTBAND_TELNET_PPS 1500

#endif


#define QOS_CAR_TABLE_SIZE 1024 * 6

#define QOS_CAR_TAB_SIZE 1024
#define QOS_CAR_INDEX_OFFSET 1000
#define QOS_CAR_CIR_MIN 0
#define QOS_CAR_CIR_MAX 10000000
#define QOS_CAR_CBS_MIN 0
#define QOS_CAR_CBS_MAX 10000000
#define QOS_CAR_PIR_MIN 0
#define QOS_CAR_PIR_MAX 10000000
#define QOS_CAR_PBS_MIN 0
#define QOS_CAR_PBS_MAX 10000000
#define QOS_CAR_PHYSICAL_BURST_MAX 16000
#define QOS_CAR_PACKETS_DROP (1 << 0)
#define QOS_CAR_PACKETS_PASS (1 << 1)
#define QOS_CAR_COLOR_BLIND 1
#define QOS_CAR_COLOR_AWARE 2
#define QOS_CAR_PROFILE_SHARED 1
#define QOS_CAR_PROFILE_MONOPOLIZE 2
#define QOS_CAR_FLAG_LR  1
#define QOS_CAR_FLAG_CAR 2



/* QOS 映射的类型 */
enum QOS_TYPE
{
	QOS_TYPE_INVALID = 0,
	QOS_TYPE_COS,
	QOS_TYPE_TOS,
	QOS_TYPE_EXP,
	QOS_TYPE_DSCP,
	QOS_TYPE_MAX = 8
};


enum QOS_DIR
{
	QOS_DIR_INVALID = 0,
	QOS_DIR_INGRESS,
	QOS_DIR_EGRESS,
	QOS_DIR_BIDIR
};


/* QOS 消息的类型 */
enum QOS_INFO
{
	QOS_INFO_INVALID = 0,
	QOS_INFO_DOMAIN,
	QOS_INFO_PHB,
	QOS_INFO_UTAG_PRI,    /* 接口上配置的 priority of untag packet */
	QOS_INFO_FILTER,
	QOS_INFO_CAR,
	QOS_INFO_HQOS,
	QOS_INFO_POLICY,
	QOS_INFO_ACL_GROUP,
	QOS_INFO_ACL_RULE,
	QOS_INFO_MIRROR_GROUP,
	QOS_INFO_MIRROR_GROUP_PORT,
	QOS_INFO_MIRROR_TO_GROUP,
	QOS_INFO_MIRROR_TO_PORT,
	QOS_INFO_CPCAR,
	QOS_INFO_PACKET_IFG,
	QOS_INFO_SUBIF_MODE,
	QOS_INFO_MAX = 17
};


/* car 参数类型 */
enum CAR_INFO
{
    CAR_INFO_INVALID = 0,
    CAR_INFO_CARID,
    CAR_INFO_CIR,
    CAR_INFO_CBS,
    CAR_INFO_PIR,
    CAR_INFO_PBS,
    CAR_INFO_RED_ACT,
    CAR_INFO_GREEN_ACT,
    CAR_INFO_YELLOW_ACT,
    CAR_INFO_COLOR_BLIND
};


/* hqos 的消息类型 */
enum HQOS_SUBTYPE
{
	HQOS_SUBTYPE_INVALID = 0,
	HQOS_SUBTYPE_WRED_RULE,
	HQOS_SUBTYPE_WRED_PROFILE,
	HQOS_SUBTYPE_QUEUE,
	HQOS_SUBTYPE_QUEUE_SCHEDULER,
	HQOS_SUBTYPE_QUEUE_PROFILE,
	HQOS_SUBTYPE_ID
};


/* QOS 映射的颜色 */
enum QOS_COLOR
{
    QOS_COLOR_GREEN = 0,
    QOS_COLOR_YELLOW,
    QOS_COLOR_RED,
    QOS_COLOR_MAX = 8
};

enum QOS_PROFILE_REF_OPER
{
	REF_OPER_INVALID = 0,
	REF_OPER_DECREASE,
	REF_OPER_INCREASE,
	REF_OPER_MAX
};

/* ACL rule 类型 */
enum ACL_TYPE
{
	ACL_TYPE_NONE = 0,
	ACL_TYPE_MAC,
	ACL_TYPE_IP,
	ACL_TYPE_IP_EXTEND,
	ACL_TYPE_MAC_IP_MIX,
	ACL_TYPE_MAX = 8
};


/* qos 策略类型 */
enum POLICY_TYPE
{
	POLICY_TYPE_INVALID = 0,
	POLICY_TYPE_FILTER,
	POLICY_TYPE_MIRROR,	
	POLICY_TYPE_REDIRECT,	
	POLICY_TYPE_QOSMAPPING,	
	POLICY_TYPE_CAR,
	POLICY_TYPE_REMARK,
	POLICY_TYPE_HQOS,
	POLICY_TYPE_VLANMAPPING,
	POLICY_TYPE_NAT,
	POLICY_TYPE_COPYTOCPU,
	POLICY_TYPE_MAX = 16
};



/* 接口配置的 qos mapping */
struct qos_entry
{
	enum QOS_TYPE domain_type;/* l2 接口默认是 cos，l3 接口默认是 dscp */
	enum QOS_TYPE phb_type;   /* 没有默认值 */
	uint8_t  domain_id; 	  /* QOS domain，默认是 domain 0 */
	uint8_t  phb_id;		  /* QOS phb */
	uint8_t  phb_enable;	  /* 1: phb 使能，0: phb 关闭，默认不使能 */
	uint8_t  untag_pri; 	  /* priority of untag packet, 默认是 5 */
};

/* qos 优先级映射规则 */
struct qos_map
{
    uint8_t priority;  /* cos/tos/exp/dscp */
    uint8_t queue;     /* 内部优先级队列 */
    uint8_t action;
    uint8_t pad;
    enum QOS_COLOR color;
};


/* cos/tos/exp/dscp 到内部优先级队列的映射 */
struct qos_domain
{

    uint8_t id;             /* domain ID */
    uint8_t pad[3];         /* for 4 byte align */
    uint32_t ref_cnt;		/* 引用计数 */
    enum QOS_TYPE type;     /* domain type */
    struct qos_map map[64]; /* 映射规则 */
};


/* 内部优先级队列到 cos/tos/exp/dscp 的映射 */
struct qos_phb
{
    uint8_t id;             /* phb ID */
    uint8_t pad[3];         /* for 4 byte align */
    uint32_t ref_cnt;		/* 引用计数 */
    enum QOS_TYPE type;     /* phb type */    
    struct qos_map map[24]; /* 映射规则 */
};

struct car_action
{
	enum QOS_COLOR color;
	uint8_t new_cos;
	uint8_t new_dscp;
	uint8_t new_queue;
	uint8_t drop;
};

/* car 数据结构 */
struct car_t
{
    uint16_t car_id;            /* car id */
    uint8_t  shared;            /* 1: 共享， 2: 独占 */
	uint8_t  color_blind;       /* 1: 色盲模式， 2: 色敏感模式 */
    uint32_t cir;
    uint32_t cbs;
    uint32_t pir;
    uint32_t pbs;
    uint32_t ref_cnt;
    struct car_action red_act;
    struct car_action green_act;
    struct car_action yellow_act;
};


/* pw/tunnel/接口下的 hqos 配置 */
struct hqos_t
{
    uint16_t id;             /*allocate hqos_id*/
	uint8_t hqos_id; 		 /* hqos queue profile ID */
	uint8_t pad;
    uint32_t cir;
    uint32_t pir;
};



/* mac 以及混合类型 acl 的结构 */
struct mac_rule
{
	uint16_t ruleid;        
	uint16_t ethtype;
    uchar    smac[6];
    uchar    dmac[6];     
	uint32_t offset_info; /* 以太头后自定义匹配报文的内容, 是 32byte */
    uint16_t vlan;        /* 4096 表示无效 */
    uint16_t cvlan;       /* 4096 表示无效 */       
	uint8_t  cos;         /* 8 表示无效 */
	uint8_t  cvlan_cos;   /* 8 表示无效 */
	uint8_t  offset_len;  /* 偏移的字节数 */
	uint8_t  offset_mask; /* 1: offset is valid */ 
	uint8_t  smac_mask;   /* 1: smac is valid */      
	uint8_t  dmac_mask;   /* 1: dmac is valid */
	uint8_t  ethtype_mask;/* 1: ethtype is valid */
	uint8_t  untag_mask;  /* 1: untag is valid */      

	uint32_t sip; 
    uint32_t dip; 
    uint16_t vpnid;      
	uint8_t  sip_masklen;
	uint8_t  dip_masklen;
    uint8_t  protocol;  /*<0-255> 1:icmp;2:igmp;4:ip;6:tcp;17:udp*/
    uint8_t  dscp;              /* 64 表示无效 */
    uint8_t  tos;       /* 8 表示无效 */
    uint8_t  ttl;      
    uint8_t  vpn_mask;   /* 1: vpn is valid */
    uint8_t  sip_mask;   /* 1: sip is valid */
    uint8_t  dip_mask;   /* 1: dip is valid */     
    uint8_t  proto_mask; /* 0: any 1:  protocol is valid */         
    uint8_t  ttl_mask;   /* 1: ttl is valid */

	uint8_t  fwd_type;	/* forwarding type */
	uint8_t	 fwd_mask;	
	uint8_t  pad;
    uint32_t vsi_id;
    uint32_t vsi_mask;  /* 1: vsi_id is valid */
	uint32_t prio;      /* ACL 优先级 */
}; 


/*ip 类型 acl 的结构 */
struct ip_rule
{
	uint16_t ruleid;
	uint16_t vpnid;
	uint32_t ip;
	uint8_t  ip_masklen;
	uint8_t  ip_flag;     /* 0: sip, 1: dip */
	uint8_t  vpn_mask;    /* 1: vpn is valid */
    uint8_t  ip_mask;     /* 1: ip is valid */
};


/*ip 扩展类型 acl 的结构 */
struct ip_extend_rule
{
	uint16_t ruleid;
	uint16_t vpnid ;
	uint32_t sip; 
    uint32_t dip; 
	uint16_t sport;	     /*<1-65535> */
	uint16_t dport;		 /*<1-65535> */
	uint8_t  sip_masklen;
	uint8_t  dip_masklen;
	uint8_t  protocol;	 /*<0-255> 1:icmp;2:igmp;4:ip;6:tcp;17:udp*/
	uint8_t  dscp;	     /* 64 表示无效 */
	uint8_t  tos;	     /* 8 表示无效 */
	uint8_t  ttl;      
	uint8_t  tcp_type;	 /*only when protocal is tcp ,flags*/
	uint8_t  vpn_mask;   /* 1: vpn is valid */
	uint8_t  sip_mask;   /* 1: sip is valid */
	uint8_t  dip_mask;   /* 1: dip is valid */	
	uint8_t  sport_mask; /* 1: sport is valid */
	uint8_t  dport_mask; /* 1: dport is valid */
	uint8_t  proto_mask; /* 0: any 1:  protocol is valid */	
	uint8_t  ttl_mask;   /* 1: ttl is valid */
	uint8_t  fragment_mask;  /* 1: untag is valid */   
	uint8_t  pad[1];
};


/*ipv6 类型 acl 的结构 */
struct ipv6_rule
{
	uint16_t ruleid;
	uint16_t vpnid;
	struct ipv6_addr ip;
	uint8_t  ip_masklen;
	uint8_t  ip_flag;     /* 0: sip, 1: dip */
	uint8_t  vpn_mask;    /* 1: vpn is valid */
    uint8_t  ip_mask;     /* 1: ip is valid */
};


/*ipv6 扩展类型 acl 的结构 */
struct ipv6_extend_rule
{
	uint16_t ruleid;
	uint16_t vpnid ;
	struct ipv6_addr sip; 
    struct ipv6_addr dip;
	uint16_t sport;	     /*<1-65535> */
	uint16_t dport;		 /*<1-65535> */
	uint8_t  sip_masklen;
	uint8_t  dip_masklen;
	uint8_t  protocol;	 /*<0-255> 1:icmp;2:igmp;4:ip;6:tcp;17:udp*/
	uint8_t  tos;	     /* 64 表示无效 */
	uint8_t  ttl;      
	uint8_t  tcp_type;	 /*only when protocal is tcp ,flags*/
	uint8_t  vpn_mask;   /* 1: vpn is valid */
	uint8_t  sip_mask;   /* 1: sip is valid */
	uint8_t  dip_mask;   /* 1: dip is valid */	
	uint8_t  sport_mask; /* 1: sport is valid */
	uint8_t  dport_mask; /* 1: dport is valid */
	uint8_t  proto_mask; /* 0: any 1:  protocol is valid */	
	uint8_t  ttl_mask;   /* 1: ttl is valid */
	uint8_t  pad[3];
};


/* rule 数据结构 */
struct rule_node
{
	enum ACL_TYPE type;
	uint32_t      acl_num;   /* 所属的 acl group number */
	union
	{
		struct mac_rule	mac_acl;
		struct ip_rule	ip_acl;
		struct ip_extend_rule ipex_acl;
	}rule;
};


/* acl 动作数据结构 */
struct acl_action
{   
	uint32_t ifindex;     /* policy 应用的接口 */
	uint32_t acl_num;     /* policy 应用的acl */
    uint8_t  act_num;	  /* action 计数 */
	uint8_t  filter; 	  /* 1: filter 使能 */
	uint8_t  copytocpu;   /* 1: copytocpu 使能 */
	uint8_t  remark;	  /* 1: remark 使能 */
	uint8_t  queue;       /* 1: 映射内部优先级*/
	uint8_t  cosq;        /* 1: 映射到 CPU 队列 */
	uint8_t  out_queue;  /* 1: 映射到 接口队列 */
	uint8_t  mirror;	  /* 1: mirror 使能 */ 
	uint8_t  redirect;	  /* 1: 重定向使能 */ 
	uint8_t  car;         /* 1: car 使能 */ 
	uint8_t  hqos;	      /* 1: hqos 使能 */
	uint8_t  class_source_value; /* 动作ClassSourceSet  set value */  
	uint8_t  class_source_set;   /* 1: enable ,动作仅vfp 支持,参数可用于ifp SrcClassField 匹配 */
	uint8_t  filter_act;  /* 0:permit  1: deny  */ 
	uint8_t  queue_id;	  /* qos 内部优先级 */	
	uint8_t  cosq_id;	  /* qos cpu 队列 id */
	uint8_t  mirror_id;   /* 镜像的 id */	
	uint8_t  remark_type; /* 1: cos, 2: tos, 3: exp, 4:dscp */
	uint8_t  remark_value;/* remark 的值 */
	uint8_t  pad;
	uint16_t car_id;      /* car 模板 id */
	uint32_t out_queue_id;/*  接口队列 id*/
	uint32_t mirror_if;   /* mirror 的目的接口 */
	uint32_t redirect_if; /* 重定向的目的接口 */
	uint8_t  dstport;     /* 1:使能dstport */
	void*    dstport_hw;  /* dstport 接口 */
};

/* qos 策略 */
struct qos_policy
{
	enum POLICY_TYPE   type;
	enum QOS_DIR direct;  /* policy 的方向 */
	uint32_t           ifindex; /* 应用的接口，全局为 0 */
	uint32_t           acl_num;
	struct acl_action  action;	
};


struct qos_if_snmp
{
	uint32_t      ifindex;
	uint32_t      car_id[2];
    uint16_t      pad;
    uint8_t       car_flag[2];
	struct car_t  car[2];
	struct hqos_t hqos;
	/* add mirror/qos_policy here */
};
	
struct qos_mirror_snmp
{
		 uint8_t  id;				/* ?????? id */
		 uint32_t ifindex;			 /* ?????????*/
};



int *qos_domain_exist_get(uint8_t index, enum QOS_TYPE type, int module_id);
int qos_domain_ref_operate(uint8_t index, enum QOS_TYPE type, int module_id, uint8_t operate);
int *qos_phb_exist_get(uint8_t index, enum QOS_TYPE type, int module_id);
int qos_phb_ref_operate(uint8_t index, enum QOS_TYPE type, int module_id, uint8_t operate);
int *qos_queue_profile_exist_get(uint32_t index, int module_id);
uint16_t *qos_hqosid_get(struct hqos_t *phqos, int module_id);
struct hqos_t *qos_hqos_get(uint16_t hqos_id, int module_id);
void qos_hqos_delete(uint16_t hqos_id, int module_id);

struct ipc_mesg_n *qos_com_get_car_profile_bulk ( uint16_t car_id, int module_id, int *pdata_num );
struct ipc_mesg_n *qos_com_get_wred_profile_bulk ( uint8_t wred_id, int module_id, int *pdata_num );
struct ipc_mesg_n *qos_com_get_queue_profile_bulk ( uint8_t queue_id, int module_id, int *pdata_num );
struct ipc_mesg_n *qos_com_get_domain_profile_bulk ( uint8_t domain_id, enum QOS_TYPE type, int module_id, int *pdata_num );
struct ipc_mesg_n *qos_com_get_phb_profile_bulk ( uint8_t phb_id, enum QOS_TYPE type, int module_id, int *pdata_num );
struct ipc_mesg_n *qos_com_get_car_apply_bulk ( uint32_t ifindex, enum QOS_DIR dir, int module_id, int *pdata_num );
struct ipc_mesg_n *qos_com_get_mirror_group_bulk ( uint32_t ifindex, int group_id, int module_id, int *pdata_num );
struct ipc_mesg_n *qos_com_get_mirror_port_bulk ( uint32_t ifindex, int module_id, int *pdata_num );
struct ipc_mesg_n *qos_com_get_car_bulk ( uint32_t ifindex, enum QOS_DIR dir, int module_id, int *pdata_num );
struct ipc_mesg_n *qos_com_get_qosif_bulk(uint32_t ifindex, int module_id, int*pdata_num);
struct ipc_mesg_n *qos_com_get_acl_rule_bulk ( uint32_t acl_num,uint16_t ruleid,int module_id, int *pdata_num );
struct ipc_mesg_n *qos_com_get_policy_bulk (struct qos_policy *policy,int module_id, int *pdata_num );



#endif


