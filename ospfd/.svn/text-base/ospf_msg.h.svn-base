#ifndef _ZEBRA_OSPF_MSG_H
#define _ZEBRA_OSPF_MSG_H
#include <lib/prefix.h>
#include <lib/ospf_common.h>
#include <lib/msg_ipc_n.h>


/*snmp get information*/
#define OSPF_SNMP_INSTANCE_GET  		0
#define OSPF_SNMP_AREA_GET      		1
#define OSPF_SNMP_NETWORK_GET   		2
#define OSPF_SNMP_INTERFACE_GET 		3
#define OSPF_SNMP_DCN_CONF_GET			4
#define OSPF_SNMP_DCN_NE_INFO_GET		5
#define OSPF_SNMP_AREA_GET_H3C      	6
#define OSPF_SNMP_NETWORK_GET_H3C   	7
#define OSPF_SNMP_INTERFACE_GET_H3C 	8
#define OSPF_SNMP_NEIGHBOR_GET_H3C  	9
#define OSPF_SNMP_ABR_SUMMARY_GET  		10
#define OSPF_SNMP_ASBR_SUMMARY_GET  	11
#define OSPF_SNMP_REDIS_ROUTE_GET  		12
#define OSPF_SNMP_NBMA_NEIGHBOR_GET  	13

enum OSPF_DCN_TRAP_TYPE
{
	OSPF_DCN_NE_OFFLINE = 0,
	OSPF_DCN_NE_ONLINE
};

enum OSPF_SNMP_RULE
{
	OSPF_START0  = 0 ,
	OSPF_START = 1,
};


/* ospf_key index */
struct ospf_key
{
    u_int32_t ospf_id;          /* ospf instance id <1-255> */
    u_int32_t area_id;          /* area id <0-255> */
};

/* ospf_key index */
struct ospf_key_network
{
    u_int32_t ospf_id;          /* ospf instance id <1-255> */
    u_int32_t area_id;          /* area id <0-255> */
    struct prefix p;            /* p.family:AF_INET, p.prefixlen: mask.p.u.prefix4:ip*/
};

/* ospf_key index */
struct ospf_key_network_h3c
{
    u_int32_t ospf_id;          /* ospf instance id <1-255> */
    u_int32_t area_id;          /* area id <0-255> */
    u_int32_t prefix4;		/* prefix4:ip */
};

struct ospf_key_prefix_h3c
{
	u_int32_t ifindex;
    u_int32_t prefix4;		/* prefix4:ip */
};



/* ospf table */
struct statics_ospf
{
    u_int32_t ospf_id;          /* ospf instance id <1-255> */
    struct in_addr router_id;   /* ospf router id*/
    u_int32_t vpn;
	
	u_int32_t ref_bandwidth;
	u_char opaque_lsa_conf;
	u_char rfc1583_conf;
	u_int32_t manger_distance;
	u_int32_t ase_distance;
	u_int32_t lsdb_refresh_interval;
	
};
/* area table */
struct statics_ospf_area
{
    uchar name[255];
    u_int32_t ospf_id;          /* ospf instance id <1-255> */
    u_int32_t area_id;          /* area id <0-255> */
    u_int32_t area_type;        /* area type 0:default, 1:stub ,2:nssa*/
    u_int32_t summary;          /* area type 0:summary, 1:no summary*/
    u_int32_t cost;             /* cost :only type is nssa or stub is valid*/
};

struct statics_ospf_area_h3c
{
    u_int32_t area_id;          /* area id <0-255> */
	int auth_type;				/* 0：none 1：simplePassword 2：md5 其它：为IANA保留 缺省为0。 */				
	int external_routing;		/* DEFAULT 0,STUB 1,NSSA:2 */
	u_int32_t spf_calculation;	/* SPF Calculation Count. */
    u_int32_t area_type;        /* area type 0:default, 1:stub ,2:nssa*/
	u_int32_t abr_count;      /* ABR router in this area. */
    u_int32_t asbr_count;     /* ASBR router in this area. */
	unsigned long lsa_count;
	//int AreaStatus;				/* no used */
    u_int32_t summary;          /* area type 0:summary, 1:no summary*/
	u_char NSSATranslatorRole;
	u_char NSSATranslatorState;
	int NSSATranslatorStabilityInterval;
    int is_first ;
	//int NssaTranslatorEvents;	/* no used */
};

struct statics_ospf_neighbor_h3c
{
    //struct prefix address;        /* Neighbor Interface Address. */
    u_int32_t prefix4;
	u_int32_t ifindex;
    u_int32_t router_id;     		/* Router ID. */
	u_char options;           	  	/* Options. */
    int priority;             	  	/* Router Priority. */
    u_char state;             	  	/* NSM status. */
	u_int32_t state_change;       	/* NSM state change counter       */
	//unsigned long NbrLsRetransQLen;	/* no used */
	//u_int32_t NbmaNbrStatus;	  	/* no used	*/
	//u_int32_t ospfNbmaNbrPermanence;/* no used	*/
	//int priority_nbma;             	/* NBMA Router Priority. */
	//u_int32_t NbrHelloSuppressed;	/* no used	*/
	//u_int32_t NbrRestartHelperAge;	/* no used	*/
	//u_int32_t NbrRestartHelperExitReason;/* no used	*/
	
};

struct statics_ospf_if_h3c
{
	//struct prefix *address;               /* Interface prefix */
    u_int32_t prefix4;						/* OSPF接口的IP地址 */
	u_int32_t ifindex;						/* 接口索引*/
	u_int32_t area_id;						/* 区域id */
	u_char type;							/* OSPF接口的类型 1：Broadcast 2：nbma 3：Point to Point 4：Point to Multipoint*/
	u_int32_t IfAdminStat;					/* 1：enable，表明接口使能了OSPF。2：disable，表明接口没有使能OSPF。 */
	u_int32_t ospfIfRtrPriority;			/* 接口的优先级。 */
	u_int32_t ospfIfTransitDelay;			/* 接口传播一个LSU报文所发费的大概时间 */
	u_int32_t ospfIfRetransInterval; 		/* 邻接接口重传LSA的时间间隔 */
	u_int32_t ospfIfHelloInterval;	 		/* 设备发送Hello报文的时间间隔 */
	u_int32_t ospfIfRtrDeadInterval; 		/* 宣告邻居Down掉的时间间隔。 */
	//u_int32_t ospfIfPollInterval;	 		/* 在NBMA网络中，向非活动的接口发送Hello报文的时间间隔 */
	u_int32_t ospfIfState;			 		/*  OSPF接口的状态*/
	u_int32_t ospfIfDesignatedRouter;		/* DR的IP地址 */
	u_int32_t ospfIfBackupDesignatedRouter; /* BDR的IP地址*/
	u_int32_t ospfIfEvents;					/* OSPF接口状态变化或发生错误的次数 */
	//u_int32_t ospfIfAuthKey;				/* 认证值 */
	
    u_char ospfIfAuthKey[257];
	//u_int32_t ospfIfStatus;				/* NO used */
	//u_int32_t ospfIfMulticastForwarding;  /* 接口上转发组播的方式*/
	//u_int32_t ospfIfDemand;				/* 表明是否在这个接口上应用OSPF按需路由机制。*/
	u_int32_t ospfIfAuthType;				/* 接口上的认证类型 */
	//u_int32_t ospfIfLsaCount;				/* 本地链路LSDB的link-local LSA的数量 */
	//u_int32_t ospfIfLsaCksumSum;			/* 本地链路的LSDB中，32bit的LSA的LS无符号校验和。  */
	u_int32_t IfDesignatedRouterid;			/* DR的Router ID */
	u_int32_t IfBackupDesignatedRouterid;	/*BDR的Router ID*/
};


/* network table */
struct statics_ospf_network
{
    u_int32_t ospf_id;          /* ospf instance id <1-255> */
    u_int32_t area_id;          /* area id <0-255> */
    struct prefix p;            /* p.u.prefix4 is NetWorkIP, p.prefixlen is NetWorkMask*/
};
struct statics_ospf_if
{
    u_int32_t ifindex;              /*interface index*/
    uchar if_name[NAME_STRING_LEN];
    u_int32_t if_type;              /* 1:pointToPoint ,2:broadcast ,3:NBMA,4:POINTOMULTIPOINT*/
	
	u_char auth_type;
	u_int32_t md5_id;
	char auth_password[256];
	u_int32_t cost_value;
	u_int32_t dead_interval;
	u_int32_t hello_interval;
	u_char mtu_check;
	u_int32_t dr_priority;
	u_int32_t retransmit_interval;
};

struct ospf_dcn_ne_info
{
	u_int32_t dcn_ne_id;
	u_int32_t dcn_ne_ip;
	u_int32_t dcn_ne_metric;
	uchar dcn_ne_device_type[64];
	uchar dcn_ne_vendor[64];
	uint8_t  mac[6];
};

struct ospf_dcn_config
{
	u_int32_t ospf_id;
	uchar ospf_dcn_enable;
	uchar ospf_dcn_trap_enable;
	u_int32_t ospf_dcn_ne_id;//value range:0x10001-0xFEFFFE
	u_int32_t ospf_dcn_ne_ip;
	u_char ospf_dcn_ne_ip_prefixlen;
	uint8_t  mac[6];
	uchar dcn_ne_vendor[64];
};

struct ospf_abr_sum_info_key
{
	u_int32_t ospf_id_key;          		/* ospf instance id <1-255> */
    u_int32_t area_id_key;          		/* area id <0-255> */
    //struct prefix p_key;            		/* p.u.prefix4 is IP, p.prefixlen is Mask*/
	struct in_addr abr_sum_ip_addr;
	struct in_addr abr_sum_ip_mask;
};

struct statistics_ospf_abr_summary_info
{
	u_int32_t ospf_id;          		/* ospf instance id <1-255> */
    u_int32_t area_id;          		/* area id <0-255> */
    //struct prefix p;            		/* p.u.prefix4 is IP, p.prefixlen is Mask*/
	struct in_addr abr_sum_ip_addr;
	struct in_addr abr_sum_ip_mask;

	u_char abr_summary_notAdvertise;	/*设置不发布聚合路由。*/
	u_int32_t abr_summary_cost;			/*聚合路由的开销值*/
};

struct ospf_asbr_sum_info_key
{
	u_int32_t ospf_id_key;          		/* ospf instance id <1-255> */
    //struct prefix p_key;            		/* p.u.prefix4 is IP, p.prefixlen is Mask*/
	struct in_addr asbr_sum_ip_addr;
	struct in_addr asbr_sum_ip_mask;
};

struct statistics_ospf_asbr_summary_info
{
	u_int32_t ospf_id;					/* ospf instance id <1-255> */
	//struct prefix p;            		/* p.u.prefix4 is IP, p.prefixlen is Mask*/
	struct in_addr asbr_sum_ip_addr;
	struct in_addr asbr_sum_ip_mask;
	
	u_char asbr_summary_notAdvertise;	/*设置不发布聚合路由。*/
	u_int32_t asbr_summary_cost;		/*聚合路由的开销值*/
};

struct ospf_redistribute_route_info_key
{
	u_int32_t ospf_id_key;					/* ospf instance id <1-255> */
	u_char redis_route_type_key;
	u_int32_t redis_target_instance_id_key;
};

struct statistics_ospf_redistribute_route_info
{
	u_int32_t ospf_id;					/* ospf instance id <1-255> */
	u_char redis_route_type;
	u_int32_t redis_target_instance_id;

	u_int32_t redis_route_metric;
	u_char route_turn2_ospf_tpye;		/*ospfType1 ( 1 ) , ospfType2 ( 2 )*/
};

struct ospf_nbma_nbr_info_key
{
	u_int32_t ospf_id_key;					/* ospf instance id <1-255> */
	struct in_addr nbr_addr_key;
};

struct statistics_ospf_nbma_neighbor_info
{
	u_int32_t ospf_id;					/* ospf instance id <1-255> */
	struct in_addr nbr_addr;
	
	u_int32_t nbr_poll_interval;	
};


extern void ospf_pkt_register(void);
extern void ospf_pkt_unregister(void);
extern int u0_device_add_handle(struct u0_device_info *new_u0);
extern int u0_device_del_handle(struct u0_device_info *new_u0);
extern int u0_device_update_handle(struct u0_device_info *new_u0);
extern int interface_set_ip(uint32_t ifindex, struct in_addr ip_addr);
extern int interface_update_ip(uint32_t ifindex, struct in_addr ip_addr, struct in_addr new_ip_addr);
extern int u0_map_info_send_to_target_process(struct u0_device_info *new_u0, int target_module_id, enum IPC_OPCODE opcode);
extern struct u0_device_info_local *u0_repeat_detect(struct u0_device_info *new_u0);
extern struct u0_device_info_local *u0_repeat_detect_by_ne_ip(uint32_t ne_ip_decimal);


#endif
