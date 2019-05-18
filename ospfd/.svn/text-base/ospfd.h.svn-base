/*
 * OSPFd main header.
 * Copyright (C) 1998, 99, 2000 Kunihiro Ishiguro, Toshiaki Takada
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef _ZEBRA_OSPFD_H
#define _ZEBRA_OSPFD_H

#include "lib/zebra.h"
#include "lib/libospf.h"
#include "lib/prefix.h"


#include "lib/filter.h"
#include "lib/log.h"
/* For vrf_bitmap_t. */
#include "lib/vrf.h"
#include "lib/hptimer.h"

#include "ospfd/ospf_msg.h"
#include "lib/route_com.h"

extern struct ospf_statics ospf_sta;

#define OSPF_VERSION            2

/* VTY port number. */
#define OSPF_VTY_PORT          2604

/* IP TTL for OSPF protocol. */
#define OSPF_IP_TTL             1
#define OSPF_VL_IP_TTL          100
#define OSPF_P2MP_IP_TTL        255


/* Default configuration file name for ospfd. */
#define OSPF_DEFAULT_CONFIG   "ospfd.conf"

#define OSPF_NSSA_TRANS_STABLE_DEFAULT      40

#define OSPF_ALLSPFROUTERS              0xe0000005      /* 224.0.0.5 */
#define OSPF_ALLDROUTERS                0xe0000006      /* 224.0.0.6 */

#define OSPF_HEADER_SIZE         24U
#define OSPF_AUTH_SIMPLE_SIZE     8U
#define OSPF_AUTH_MD5_SIZE       256U
#define OSPF_MD5_SIZE           16U


/* OSPF Authentication Type. */
#define OSPF_AUTH_NULL                      0
#define OSPF_AUTH_SIMPLE                    1
#define OSPF_AUTH_CRYPTOGRAPHIC             2
/* For Interface authentication setting default */
#define OSPF_AUTH_NOTSET                   -1
/* For the consumption and sanity of the command handler */
/* DO NIOT REMOVE!!! Need to detect whether a value has
   been given or not in VLink command handlers */
#define OSPF_AUTH_CMD_NOTSEEN              -2

#define OSPF_IF_LDP_SYNC_DISABLE            0
#define OSPF_IF_LDP_SYNC_ENABLE             1


/* OSPF options. */
#define OSPF_OPTION_MT                   0x01  /* M/T */
#define OSPF_OPTION_E                    0x02
#define OSPF_OPTION_MC                   0x04
#define OSPF_OPTION_NP                   0x08
#define OSPF_OPTION_EA                   0x10
#define OSPF_OPTION_DC                   0x20
#define OSPF_OPTION_O                    0x40
#define OSPF_OPTION_DN                   0x80


/* OSPF Database Description flags. */
#define OSPF_DD_FLAG_MS                  0x01
#define OSPF_DD_FLAG_M                   0x02
#define OSPF_DD_FLAG_I                   0x04
#define OSPF_DD_FLAG_ALL                 0x07

#define OSPF_LS_REFRESH_SHIFT           (60 * 15)
#define OSPF_LS_REFRESH_JITTER              60

#define OSPF_DEFAULT_PROCESS_ID				1
#define OSPF_MAX_PROCESS_ID					255
#define OSPF_MAX_PROCESS_ID_H3C				65535

#define OSPF_MAX_VRF_ID					128
#define OSPF_DCN_VRF_ID					127
#define OSPF_DCN_NE_IP_FREFIX			136		/* IP segment assigned by the company*/

#define U0_DEVICE_NUM					32		/* By default, only one U0 can be managed under one U3.*/
#define U0_LOOPBACK_INDEX_BASE			90		/* 预留loopback 90 ~ loopback 122 用来落地U0设备的虚拟ip*/

#define DCN_VLAN_ID						(4094)

/* OSPF master for system wide configuration and variables. */
struct ospf_master
{
    /* OSPF instance list*/
    struct list *ospf;

    /* OSPF thread master. */
    struct thread_master *master;

    /* OSPF interface list. */
    struct list *iflist;

    /* Redistributed external information. */
    struct route_table *external_info[OSPF_MAX_VRF_ID][ROUTE_PROTO_MAX + 1];
#define EXTERNAL_INFO(V,T)      om->external_info[V][T]

    /* OSPF start time. */
    time_t start_time;

    /* Various OSPF global configuration. */
    u_char options;
#define OSPF_MASTER_SHUTDOWN (1 << 0) /* deferred-shutdown */

    /* redistribute type flag  */
    u_char redis_count [ROUTE_PROTO_MAX + 1];

    /* packet recive thread */
    struct thread *t_read;

    //struct thread *t_thread_routefifo;    /*异步发送route thread*/
	TIMERID			t_thread_routefifo;    /*异步发送route thread*/
    struct fifo routefifo;    /*异步发送route fifo*/
    //struct fifo ifmfifo;    /*异步发送route fifo*/
	//struct fifo routefifo_rcv; /*异步接收route fifo*/
	
	/* ospf-dcn config parameter */
	unsigned char mac[6];
	char company_name[32];
	char device_name[32];
		
	struct in_addr ne_id_static;
  	struct in_addr ne_id_dynamic;
	
  	struct in_addr ne_ip_static;
	u_char ne_ip_static_prefixlen;
	
  	struct in_addr ne_ip_dynamic;
	u_char ne_ip_dynamic_prefixlen;

	/* U0 device manger*/
	u_char u0_flag;
#define U0_DEVICE_EXIST 		(1)
	struct list *u0_list;
	char loopback_index_pool[U0_DEVICE_NUM];	/* Used to mark whether the loopback is used: 0 is no use, 1 is uesed*/
#define LOOPBACK_HAS_USED 		(1)
#define LOOPBACK_NO_USED		(0)

};

struct ospf_err
{
    u_int32_t receive_own_count;            /* IP: received my own packet  */
    u_int32_t receive_bad_pkt_count;        /* Bad packet  */
    u_int32_t receive_bad_version_count;    /* Bad version  */
    u_int32_t receive_bad_check_count;      /* Bad checksum */
    u_int32_t receive_bad_area_count;       /* Bad area id  */   
    u_int32_t receive_unnumberd_count;      /* Drop on unnumbered interface */ 
    u_int32_t receive_bad_vertual_count;    /* Bad virtual link */
    u_int32_t receive_auth_type_count;      /* Bad authentication type */
    u_int32_t receive_auth_key_count;       /* Bad authentication key */
    u_int32_t receive_pkt_small_count;      /* Packet too small*/
    u_int32_t receive_pkt_large_count;      /* Packet size > ip length */
    u_int32_t receive_transmit_count;       /* Transmit error */
    u_int32_t receive_down_count;           /* Interface down */
    u_int32_t receive_neighbor_count;       /* Unknown neighbor */

    /* hello */
    u_int32_t receive_mask_mismatch_count;    /* Netmask mismatch */
    u_int32_t receive_hello_mismatch_count;   /* Hello timer mismatch */
    u_int32_t receive_dead_mismatch_count;    /* Dead timer mismatch */
    u_int32_t receive_option_mismatch_count;  /* Extern option mismatch */  
    u_int32_t receive_virnei_unknown_count;   /* Virtual neighbor unknown */
    u_int32_t receive_hello_routerid_mismatch_count;/* Router ID confusion */
    u_int32_t receive_nbmanei_unknown_count;  /* NBMA neighbor unknown */

    /* dd */
    u_int32_t receive_dd_neighbor_low_count; /* Neighbor state low */
    u_int32_t receive_dd_routerid_mismatch_count;/* Router ID confusion */
    u_int32_t receive_extern_option_count;  /* Extern option mismatch*/ 
    u_int32_t receive_dd_unlsa_count; /* Unknown LSA type */ 
    u_int32_t receive_mtu_mismatch_count; /* MTU option mismatch*/  

    /* lsack */
    u_int32_t receive_lsack_neighbor_low_count; /* Neighbor state low */
    u_int32_t receive_lsack_unlsa_count; /* Unknown LSA Type */ 

    /* lsr */
    u_int32_t receive_lsr_neighbor_low_count; /* Neighbor state low */
    u_int32_t receive_bad_req_count;          /* Bad request */

    /* lsu */ 
    u_int32_t receive_lsu_neighbor_low_count;/* Neighbor state low */
    u_int32_t receive_lsu_lsa_check_count;  /* LSA checksum bad  */
    u_int32_t receive_lsu_unlsa_count;      /* Unknown LSA Type */  
    u_int32_t receive_less_recetlsa_count;  /* Received less recent LSA */
                      
};

/* OSPF instance structure. */
struct ospf
{
    /* OSPF Router ID. */
    struct in_addr router_id;         /* Configured automatically. */
    struct in_addr router_id_static;  /* Configured manually. */

    /* OSPF instance ID. */
    u_int16_t ospf_id;

    /* ABR/ASBR internal flags. */
    u_char flags;
#define OSPF_FLAG_ABR           0x0001
#define OSPF_FLAG_ASBR          0x0002

    /* ABR type. */
    u_char abr_type;
#define OSPF_ABR_UNKNOWN        0
#define OSPF_ABR_STAND          1
#define OSPF_ABR_IBM            2
#define OSPF_ABR_CISCO          3
#define OSPF_ABR_SHORTCUT       4
#define OSPF_ABR_DEFAULT    OSPF_ABR_CISCO

    /* NSSA ABR */
    u_char anyNSSA;       /* Bump for every NSSA attached. */
    u_char anySTUB;       /* Bump for every STUB attached. */

    /* Configured variables. */
    u_char config;
#define OSPF_RFC1583_COMPATIBLE         (1 << 0)
#define OSPF_OPAQUE_CAPABLE             (1 << 2)
#define OSPF_LOG_ADJACENCY_CHANGES      (1 << 3)
#define OSPF_LOG_ADJACENCY_DETAIL       (1 << 4)

    u_char restarting;    /* OSPF RESET FLAG when reset the value is 1 */

    /* Opaque-LSA administrative flags. */
    u_char opaque;
#define OPAQUE_OPERATION_READY_BIT      (1 << 0)

    /* RFC3137 stub router. Configured time to stay stub / max-metric */
    unsigned int stub_router_startup_time;    /* seconds */
    unsigned int stub_router_shutdown_time;   /* seconds */
#define OSPF_STUB_ROUTER_UNCONFIGURED           0
    u_char       stub_router_admin_set;
#define OSPF_STUB_ROUTER_ADMINISTRATIVE_SET     1
#define OSPF_STUB_ROUTER_ADMINISTRATIVE_UNSET   0

#define OSPF_STUB_MAX_METRIC_SUMMARY_COST   0x00ff0000

    /* LSA timers */
    unsigned int min_ls_interval;     /* minimum delay between LSAs (in msec) */
    unsigned int min_ls_arrival;      /* minimum interarrival time between LSAs (in msec) */

    /* SPF parameters */
    unsigned int spf_delay;           /* SPF delay time. */
    unsigned int spf_holdtime;        /* SPF hold time. */
    unsigned int spf_max_holdtime;    /* SPF maximum-holdtime */
    unsigned int spf_hold_multiplier; /* Adaptive multiplier for hold time */

    int default_originate;            /* Default information originate. */
#define DEFAULT_ORIGINATE_NONE      0
#define DEFAULT_ORIGINATE_ZEBRA     1
#define DEFAULT_ORIGINATE_ALWAYS    2
    u_int32_t ref_bandwidth;          /* Reference Bandwidth (Kbps). */
    struct route_table *networks;     /* OSPF config networks. */
    struct list *vlinks;              /* Configured Virtual-Links. */
    struct list *areas;               /* OSPF areas belong to this OSPF instance. */
    struct route_table *nbr_nbma;
    struct ospf_area *backbone;       /* Pointer to the Backbone Area. */

    struct list *oiflist;             /* ospf interfaces belong to this OSPF instance*/
    u_char passive_interface_default; /* passive-interface default */

    /* LSDB of AS-external-LSAs. */
    struct ospf_lsdb *lsdb;

    /* Flags. */
    int external_origin;              /* AS-external-LSA origin flag. */
    int ase_calc;                     /* ASE calculation flag. */

    struct list *opaque_lsa_self;     /* Type-11 Opaque-LSAs */

    /* Routing tables. */
    struct route_table *old_table;        /* Old routing table. */
    struct route_table *new_table;        /* Current routing table. */

    struct route_table *old_rtrs;         /* Old ABR/ASBR RT. */
    struct route_table *new_rtrs;         /* New ABR/ASBR RT. */

    struct route_table *new_external_route;   /* New External Route. */
    struct route_table *old_external_route;   /* Old External Route. */

    struct route_table *external_lsas;        /* Database of external LSAs,
                                                prefix is LSA's adv. network*/

    /* Time stamps */
    struct timeval ts_spf;                /* SPF calculation time stamp. */
    struct timeval ts_spf_duration;       /* Execution time of last SPF */

    struct route_table *maxage_lsa;       /* List of MaxAge LSA for deletion. */
    int redistribute;                     /* Num of redistributed protocols. */

#if 0
    /* Threads. */
    struct thread *t_abr_task;            /* ABR task timer. */
    struct thread *t_asbr_check;          /* ASBR check timer. */
    struct thread *t_distribute_update;   /* Distirbute list update timer. */
    struct thread *t_spf_calc;            /* SPF calculation timer. */
    struct thread *t_ase_calc;            /* ASE calculation timer. */
    struct thread *t_external_lsa;        /* AS-external-LSA origin timer. */
    struct thread *t_opaque_lsa_self;     /* Type-11 Opaque-LSAs origin event. */
	struct thread *t_external_fw;		  /* external lsa fw update */
	struct thread *t_nssa_fw;		  	  /* nssa lsa fw update */
#endif

	 /* Threads. */
    TIMERID			t_abr_task;            	/* ABR task timer. */
    TIMERID			t_asbr_check;          	/* ASBR check timer. */
    TIMERID			t_distribute_update;   	/* Distirbute list update timer. */
    TIMERID			t_spf_calc;            	/* SPF calculation timer. */
    TIMERID			t_ase_calc;            	/* ASE calculation timer. */
    TIMERID			t_external_lsa;        	/* AS-external-LSA origin timer. */
    TIMERID			t_opaque_lsa_self;     	/* Type-11 Opaque-LSAs origin event. */
	TIMERID			t_external_fw;		  	/* external lsa fw update */
	TIMERID			t_nssa_fw;		  	  	/* nssa lsa fw update */

    unsigned int maxage_delay;            	/* Delay on Maxage remover timer, sec */
#if 0
    struct thread *t_maxage;              /* MaxAge LSA remover timer. */
    struct thread *t_maxage_walker;       /* MaxAge LSA checking timer. */
    struct thread *t_reset;               /* MaxAge LSA checking timer. */
#endif
    TIMERID			t_maxage;              	/* MaxAge LSA remover timer. */
    TIMERID			t_maxage_walker;       	/* MaxAge LSA checking timer. */
    TIMERID			t_reset;               	/* MaxAge LSA checking timer. */

    //struct thread *t_deferred_shutdown;   /* deferred/stub-router shutdown timer*/
    TIMERID			t_deferred_shutdown;

    struct thread *t_write;
    struct thread *t_read;
    unsigned int maxsndbuflen;
    struct list *oi_write_q;

    /* Distribute lists out of other route sources. */
    struct
    {
        char *name;
        struct access_list *list;
    } dlist[ROUTE_PROTO_MAX];
#define DISTRIBUTE_NAME(O,T)    (O)->dlist[T].name
#define DISTRIBUTE_LIST(O,T)    (O)->dlist[T].list

    /* Redistribute metric info. */
    struct
    {
        int type;                  /* External metric type (E1 or E2).  */
        int value;                      /* Value for static metric (24-bit).
                                        -1 means metric value is not set. */
        vrf_bitmap_t redist;
    } dmetric [ROUTE_PROTO_MAX + 1][OSPF_MAX_PROCESS_ID + 1];

    /* For redistribute route map. */
    struct
    {
        char *name;
        struct route_map *map;
    } route_map [ROUTE_PROTO_MAX + 1];    /* +1 is for default-information */
#define ROUTEMAP_NAME(O,T)   (O)->route_map[T].name
#define ROUTEMAP(O,T)        (O)->route_map[T].map

    int default_metric;                   /* Default metric for redistribute. */

#define OSPF_LSA_REFRESHER_GRANULARITY      10
#define OSPF_LSA_REFRESHER_SLOTS ((OSPF_LS_REFRESH_TIME + \
                                  OSPF_LS_REFRESH_SHIFT)/10 + 1)
    struct
    {
        u_int16_t index;
        struct list *qs[OSPF_LSA_REFRESHER_SLOTS];
    } lsa_refresh_queue;

    //struct thread *t_lsa_refresher;
	TIMERID			t_lsa_refresher;
    time_t lsa_refresher_started;
#define OSPF_LSA_REFRESH_INTERVAL_DEFAULT   10
    u_int16_t lsa_refresh_interval;

    u_int16_t refresh_interval;

    /* Distance parameter. */
    u_char distance_all;
    u_char distance_intra;
    u_char distance_inter;
    u_char distance_external;

    /* Statistics for LSA origination. */
    u_int32_t lsa_originate_count;

    /* Statistics for LSA used for new instantiation. */
    u_int32_t rx_lsa_count;

    struct route_table *distance_table;
    struct route_table *summary;      /* Configured ASBR Summary*/

    /* Redistribute information. */
    u_char redist_default;
    /* Redistribute defauilt. */
    vrf_bitmap_t default_information;

    uint16_t vpn;                  /* Configured Ospf vpnid */
    struct ospf_err o_err;
    u_char dcn_enable;              /* DCN ENABLE*/
/*#define OSPF_DCN_DISABLE        0
#define OSPF_DCN_ENABLE         1*/

    u_char dcn_report;              /* Report  ENABLE*/
#define OSPF_DCN_REPORT_DISABLE	0
#define OSPF_DCN_REPORT_ENABLE	1

	/* ospf bfd cfg for ospf instance*/
	u_char all_interfaces_bfd_enable;
#define OSPF_ALL_IF_BFD_DISABLE        0
#define OSPF_ALL_IF_BFD_ENABLE         1
	uint32_t bfd_recv_interval;	  	 	/* 实例下全局报文接收间隔 */
	uint32_t bfd_send_interval;	   		/* 实例下全局报文发送间隔 */
	uint16_t bfd_detect_multiplier;  	/* 实例下全局检测次数*/


    int route_policy_flag;
    struct list *redist_name;

};

/* OSPF area structure. */
struct ospf_area
{
    /* OSPF instance. */
    struct ospf *ospf;

    /* area name */
    char *area_name;

    /* OSPF interface list belonging to the area. */
    struct list *oiflist;

    /* Area ID. */
    struct in_addr area_id;

    /* Area ID format. */
    char format;
#define OSPF_AREA_ID_FORMAT_ADDRESS         1
#define OSPF_AREA_ID_FORMAT_DECIMAL         2

    /* Address range. */
    struct list *address_range;

    /* Configured variables. */
    int external_routing;                 /* ExternalRoutingCapability. */
#define OSPF_AREA_DEFAULT                   0
#define OSPF_AREA_STUB                      1
#define OSPF_AREA_NSSA                      2
#define OSPF_AREA_TYPE_MAX                  3
    int external_routing_config;
    int no_summary;                       /* Don't inject summaries into stub.*/
    int shortcut_configured;              /* Area configured as shortcut. */
#define OSPF_SHORTCUT_DEFAULT               0
#define OSPF_SHORTCUT_ENABLE                1
#define OSPF_SHORTCUT_DISABLE               2
    int shortcut_capability;              /* Other ABRs agree on S-bit */
    u_int32_t default_cost;               /* StubDefaultCost. */

	/* Authentication data. */
    int auth_type;                        /* Authentication type. */
	/******************start:add by zzl 20180604*****************/
	u_char auth_type__config:1;
#define AREA_AUTH_PARAM_CLEAN 				0
#define AREA_AUTH_PARAM_CONFIG 				1
	 
    u_char auth_simple[OSPF_AUTH_SIMPLE_SIZE + 1];       /* Simple password. */
    u_char auth_simple__config:1;

	struct list *auth_crypt;		/* List of Auth cryptographic data. */
	u_char auth_crypt__config:1;
	/******************end:add by zzl 20180604*******************/

    u_char NSSATranslatorRole;            /* NSSA configured role */
#define OSPF_NSSA_ROLE_NEVER                0
#define OSPF_NSSA_ROLE_CANDIDATE            1
#define OSPF_NSSA_ROLE_ALWAYS               2
    u_char NSSATranslatorState;           /* NSSA operational role */
#define OSPF_NSSA_TRANSLATE_DISABLED        0
#define OSPF_NSSA_TRANSLATE_ENABLED         1
    int NSSATranslatorStabilityInterval;

    u_char transit;                       /* TransitCapability. */
#define OSPF_TRANSIT_FALSE                  0
#define OSPF_TRANSIT_TRUE                   1
    struct route_table *ranges;           /* Configured Area Ranges. */

    /* RFC3137 stub router state flags for area */
    u_char stub_router_state;
#define OSPF_AREA_ADMIN_STUB_ROUTED         (1 << 0) /* admin stub-router set */
#define OSPF_AREA_IS_STUB_ROUTED            (1 << 1) /* stub-router active */
#define OSPF_AREA_WAS_START_STUB_ROUTED     (1 << 2) /* startup SR was done */

    /* Area related LSDBs[Type1-4]. */
    struct ospf_lsdb *lsdb;

    /* Self-originated LSAs. */
    struct ospf_lsa *router_lsa_self;
    struct list *opaque_lsa_self;     /* Type-10 Opaque-LSAs */

    /* Area announce list. */
    struct
    {
        char *name;
        struct access_list *list;
    } export;
#define EXPORT_NAME(A)  (A)->export.name
#define EXPORT_LIST(A)  (A)->export.list

    /* Area acceptance list. */
    struct
    {
        char *name;
        struct access_list *list;
    } import;
#define IMPORT_NAME(A)  (A)->import.name
#define IMPORT_LIST(A)  (A)->import.list

    /* Type 3 LSA Area prefix-list. */
    struct
    {
        char *name;
        struct prefix_list *list;
    } plist_in;
#define PREFIX_LIST_IN(A)   (A)->plist_in.list
#define PREFIX_NAME_IN(A)   (A)->plist_in.name

    struct
    {
        char *name;
        struct prefix_list *list;
    } plist_out;
#define PREFIX_LIST_OUT(A)  (A)->plist_out.list
#define PREFIX_NAME_OUT(A)  (A)->plist_out.name

    /* Shortest Path Tree. */
    struct vertex *spf;

    /* Threads. */
#if 0
    struct thread *t_stub_router;     /* Stub-router timer */
    struct thread *t_opaque_lsa_self; /* Type-10 Opaque-LSAs origin. */
#endif
	TIMERID			t_stub_router;     /* Stub-router timer */
    TIMERID			t_opaque_lsa_self; /* Type-10 Opaque-LSAs origin. */

    /* Statistics field. */
    u_int32_t spf_calculation;        /* SPF Calculation Count. */

    /* Time stamps. */
    struct timeval ts_spf;            /* SPF calculation time stamp. */

    /* Router count. */
    u_int32_t abr_count;      /* ABR router in this area. */
    u_int32_t asbr_count;     /* ASBR router in this area. */

    /* Counters. */
    u_int32_t act_ints;       /* Active interfaces. */
    u_int32_t full_nbrs;      /* Fully adjacent neighbors. */
    u_int32_t full_vls;       /* Fully adjacent virtual neighbors. */
};

/* OSPF config network structure. */
struct ospf_network
{
    /* Area ID. */
    struct in_addr area_id;
    int format;
};

/* OSPF NBMA neighbor structure. */
struct ospf_nbr_nbma
{
    /* Neighbor IP address. */
    struct in_addr addr;

    /* OSPF interface. */
    struct ospf_interface *oi;

    /* OSPF neighbor structure. */
    struct ospf_neighbor *nbr;

    /* Neighbor priority. */
    u_char priority;

    /* Poll timer value. */
    u_int32_t v_poll;

    /* Poll timer thread. */
    //struct thread *t_poll;
	TIMERID			t_poll;

    /* State change. */
    u_int32_t state_change;
};

/* Macro. */
#define OSPF_AREA_SAME(X,Y) \
        (memcmp ((X->area_id), (Y->area_id), IPV4_MAX_BYTELEN) == 0)

#define IS_OSPF_ABR(O)      ((O)->flags & OSPF_FLAG_ABR)
#define IS_OSPF_ASBR(O)     ((O)->flags & OSPF_FLAG_ASBR)

#define OSPF_IS_AREA_ID_BACKBONE(I) ((I).s_addr == OSPF_AREA_BACKBONE)
#define OSPF_IS_AREA_BACKBONE(A)                                                                                            \
        OSPF_IS_AREA_ID_BACKBONE ((A)->area_id)

#define IS_AREA_DEFAULT(A)                                                    \
    ((A)->external_routing == OSPF_AREA_DEFAULT)
#define IS_AREA_STUB(A)                                                       \
    ((A)->external_routing == OSPF_AREA_STUB)
#define IS_AREA_NSSA(A)                                                       \
    ((A)->external_routing == OSPF_AREA_NSSA)

#define OSPF_AREA_EXTERNAL_ROUTING(A)           (A)->external_routing



#ifdef roundup
#  define ROUNDUP(val, gran)    roundup(val, gran)
#else /* roundup */
#  define ROUNDUP(val, gran)    (((val) - 1 | (gran) - 1) + 1)
#endif /* roundup */

#define LSA_OPTIONS_GET(area) \
        (((area)->external_routing == OSPF_AREA_DEFAULT) ? OSPF_OPTION_E : 0)
/* nssa router lsa N/P can not set*/
#define LSA_OPTIONS_NSSA_GET(area) \
        (((area)->external_routing == OSPF_AREA_NSSA)  ? OSPF_OPTION_NP : 0)

#if 0
#define OSPF_TIMER_ON(T,F,V)                                                  \
    do {                                                                      \
      if (!(T))                                                               \
    (T) = thread_add_timer (master_ospf, (F), ospf, (V));                      \
    } while (0)

#define OSPF_AREA_TIMER_ON(T,F,V)                                             \
    do {                                                                      \
      if (!(T))                                                               \
        (T) = thread_add_timer (master_ospf, (F), area, (V));                      \
    } while (0)

#define OSPF_POLL_TIMER_ON(T,F,V)                                             \
    do {                                                                      \
      if (!(T))                                                               \
        (T) = thread_add_timer (master_ospf, (F), nbr_nbma, (V));                  \
    } while (0)

#define OSPF_POLL_TIMER_OFF(X)      OSPF_TIMER_OFF((X))

#define OSPF_TIMER_OFF(X)                                                     \
    do {                                                                      \
      if (X)                                                                  \
        {                                                                     \
          thread_cancel (X);                                                  \
          (X) = NULL;                                                         \
        }                                                                     \
    } while (0)
#endif

/*Change by zzl 20180911 for IPC optimization*/
#define OSPF_TIMER_ON(T,F,V)                                                  \
	do {                                                                      \
		if (!(T))                                                               \
			(T) = high_pre_timer_add ((char *)"ospf_timer", LIB_TIMER_TYPE_NOLOOP, (F), ospf, (V)*1000);\
	} while (0)

#define OSPF_AREA_TIMER_ON(T,F,V)                                             \
	do {                                                                      \
		if (!(T))                                                               \
			(T) = high_pre_timer_add ((char *)"ospf_area_timer", LIB_TIMER_TYPE_NOLOOP, (F), area, (V)*1000);\
	} while (0)

#define OSPF_POLL_TIMER_ON(T,F,V)                                             \
	do {                                                                      \
		if (!(T))                                                               \
			(T) = high_pre_timer_add ((char *)"ospf_nbma_timer", LIB_TIMER_TYPE_NOLOOP, (F), nbr_nbma, (V)*1000);\
	} while (0)

#define OSPF_POLL_TIMER_OFF(X)      OSPF_TIMER_OFF((X))

#define OSPF_TIMER_OFF(X)                                                     	\
    do {                                                                      	\
      if (X)                                                                  	\
        {                                                                     	\
          high_pre_timer_delete (X);        									\
          (X) = 0;                                                        		\
        }                                                                     	\
    } while (0)

#define OSPF_TIMER_OFF_OLD(X)                                                   \
	do {																		\
		if (X)																	\
		{ 																		\
			thread_cancel (X);													\
				(X) = NULL; 													\
		} 																		\
	} while (0)
		  

/* Extern variables. */
extern struct ospf_master *om;
extern const struct message ospf_ism_state_msg[];
extern const struct message ospf_nsm_state_msg[];
extern const struct message ospf_lsa_type_msg[];
extern const struct message ospf_link_state_id_type_msg[];
extern const struct message ospf_network_type_msg[];
extern const int ospf_ism_state_msg_max;
extern const int ospf_nsm_state_msg_max;
extern const int ospf_lsa_type_msg_max;
extern const int ospf_link_state_id_type_msg_max;
extern const int ospf_redistributed_proto_max;
extern const int ospf_network_type_msg_max;
extern struct thread_master *master_ospf;
extern int ospf_zlog;

/* Prototypes. */
extern const char *ospf_redist_string(u_int );
extern struct ospf * ospf_lookup_id (u_int16_t );
extern struct ospf *ospf_lookup (void);
extern struct ospf *ospf_get (u_int16_t);
extern void ospf_finish (struct ospf *);
extern void ospf_reset(struct ospf *);

extern void ospf_router_id_update (struct ospf *ospf);
extern int ospf_network_set (struct ospf *, struct prefix_ipv4 *,
                             struct in_addr);
extern int ospf_network_unset (struct ospf *, struct prefix_ipv4 *,
                               struct in_addr);
extern int ospf_area_stub_set (struct ospf *, struct in_addr, int);
extern int ospf_area_stub_unset (struct ospf *, struct in_addr);
extern int ospf_area_no_summary_set (struct ospf *, struct in_addr);
extern int ospf_area_no_summary_unset (struct ospf *, struct in_addr);
extern int ospf_area_nssa_set (struct ospf *, struct in_addr,int);
extern int ospf_area_nssa_unset (struct ospf *, struct in_addr);
extern int ospf_area_nssa_translator_role_set (struct ospf *, struct in_addr,
        int);
extern int ospf_area_export_list_set (struct ospf *, struct ospf_area *,
                                      const char *);
extern int ospf_area_export_list_unset (struct ospf *, struct ospf_area *);
extern int ospf_area_import_list_set (struct ospf *, struct ospf_area *,
                                      const char *);
extern int ospf_area_import_list_unset (struct ospf *, struct ospf_area *);
extern int ospf_area_shortcut_set (struct ospf *, struct ospf_area *, int);
extern int ospf_area_shortcut_unset (struct ospf *, struct ospf_area *);
extern int ospf_refresh_interval_set (struct ospf *, int );
extern int ospf_timers_refresh_set (struct ospf *, int);
extern int ospf_timers_refresh_unset (struct ospf *);
extern int ospf_nbr_nbma_set (struct ospf *, struct in_addr);
extern int ospf_nbr_nbma_unset (struct ospf *, struct in_addr);
extern int ospf_nbr_nbma_priority_set (struct ospf *, struct in_addr, u_char);
extern int ospf_nbr_nbma_priority_unset (struct ospf *, struct in_addr);
extern int ospf_nbr_nbma_poll_interval_set (struct ospf *, struct in_addr,
        unsigned int);
extern int ospf_nbr_nbma_poll_interval_unset (struct ospf *, struct in_addr);
extern void ospf_prefix_list_update (struct prefix_list *);

extern void ospf_if_update (struct ospf *, struct interface *);
extern void ospf_ls_upd_queue_empty (struct ospf_interface *);
extern void ospf_terminate (void);
extern void ospf_nbr_nbma_if_update (struct ospf *, struct ospf_interface *);
extern struct ospf_nbr_nbma *ospf_nbr_nbma_lookup (struct ospf *,
        struct in_addr);
extern struct ospf_nbr_nbma *ospf_nbr_nbma_lookup_next (struct ospf *,
        struct in_addr *,
        int);
extern int ospf_oi_count (struct interface *);

extern void ospf_area_free (struct ospf_area *);
extern void ospf_area_type_set (struct ospf_area *, int);

extern struct ospf_area *ospf_area_get (struct ospf *, struct in_addr, int);
extern void ospf_area_check_free (struct ospf *, struct in_addr);
extern struct ospf_area *ospf_area_lookup_by_area_id (struct ospf *,
        struct in_addr);
extern void ospf_area_add_if (struct ospf_area *, struct ospf_interface *);
extern void ospf_area_del_if (struct ospf_area *, struct ospf_interface *);

extern void ospf_interface_area_set (struct interface *);
extern void ospf_interface_area_unset (struct interface *);

extern void ospf_route_map_init (void);
extern void ospf_snmp_init (void);

extern void ospf_master_init (void);
extern char *inet_ntoa(struct in_addr);

extern int ospf_name_set (struct ospf_area *area, const char *name);

extern int ospf_name_unset (struct ospf_area *area);

extern int send_ne_ip_singal_to_snmpd(uint32_t ne_ip, enum IPC_OPCODE opcode);

extern int send_dcn_singal_to_target_module(u_char dcn_enable, int module_id,
							enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode);

extern void interface_cmd_config_for_dcn(struct ospf *, struct prefix_ipv4 *);


#endif /* _ZEBRA_OSPFD_H */


