/* RIP related values and structures.
 * Copyright (C) 1997, 1998, 1999 Kunihiro Ishiguro <kunihiro@zebra.org>
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
#include <lib/distribute.h>
#include <lib/command.h>
#include <lib/table.h>
#include <lib/vrf.h>
#include <lib/types.h>
#include <lib/stream.h>
#include "lib/route_com.h"
#include "lib/pkt_buffer.h"
#include "lib/hptimer.h"
#include "thread.h"


#ifndef _ZEBRA_RIP_H
#define _ZEBRA_RIP_H

/* RIP version number. */
#define RIPv1                            1
#define RIPv2                            2
/* N.B. stuff will break if
    (RIPv1 != RI_RIP_VERSION_1) || (RIPv2 != RI_RIP_VERSION_2) */


/* RIP command list. */
#define RIP_REQUEST                      1
#define RIP_RESPONSE                     2
#define RIP_TRACEON                      3  /* Obsolete */
#define RIP_TRACEOFF                     4  /* Obsolete */
#define RIP_POLL                         5
#define RIP_POLL_ENTRY                   6
#define RIP_COMMAND_MAX                  7

/* RIP metric infinity value.*/
#define RIP_METRIC_INFINITY             16

/* Normal RIP packet min and max size. */
#define RIP_PACKET_MINSIZ                4
#define RIP_PACKET_MAXSIZ              512

#define RIP_HEADER_SIZE                  4
#define RIP_RTE_SIZE                    20

/* Max count of routing table entry in one rip packet. */
#define RIP_MAX_RTE   ((RIP_PACKET_MAXSIZ - RIP_HEADER_SIZE) / RIP_RTE_SIZE)

/* RIP version 2 multicast address. */
//#ifndef INADDR_RIP_GROUP
//#define INADDR_RIP_GROUP        0xe0000009    /* 224.0.0.9 */
//#define INADDR_BROAD_GROUP        0xffffffff    /* 255.255.255.255 */
//#endif

#define IPC_RIP_MSG_LEN				  8192

/* RIP timers */
#define RIP_UPDATE_TIMER_DEFAULT        30
#define RIP_TIMEOUT_TIMER_DEFAULT      180
#define RIP_GARBAGE_TIMER_DEFAULT      120

/* RIP peer timeout value. */
#define RIP_PEER_TIMER_DEFAULT         180

/* RIP port number. */
#define RIP_PORT_DEFAULT               520
#define RIP_VTY_PORT                  2602

/* Default configuration file name. */
#define RIPD_DEFAULT_CONFIG    "ripd.conf"

/* RIP route types. */
//#define RIP_ROUTE_DEFAULT                0x02
#define RIP_ROUTE_REDISTRIBUTE           0x01
#define RIP_ROUTE_RTE                    0x02
#define RIP_ROUTE_INTERFACE              0x04
#define RIP_ROUTE_SUMMARY                0x08

/* RIP summary types. */
#define RIP_SUMMARY_SUB_AUTO               0x01
#define RIP_SUMMARY_SUB_ADDR               0x02
#define RIP_SUMMARY_AUTO                   0x04
#define RIP_SUMMARY_ADRESS                 0x08
#define RIP_SUMMARY_SUPER                  0x10


/* RIPv2 special RTE family types */
#define RIP_FAMILY_AUTH                  0xffff

/* RIPv2 authentication types, for RIP_FAMILY_AUTH RTE's */
#define RIP_NO_AUTH                0
#define RIP_AUTH_DATA              1
#define RIP_AUTH_SIMPLE_PASSWORD   2
#define RIP_AUTH_MD5               3

/* RIPv2 Simple authentication */
#define RIP_AUTH_SIMPLE_SIZE        16

/* RIPv2 MD5 authentication. */
#define RIP_AUTH_MD5_SIZE               16
#define RIP_AUTH_MD5_COMPAT_SIZE        RIP_RTE_SIZE

#define IFF_UP      0x0001      /* interface is up */
#define IFF_RUNNING 0x0040      /* resources allocated */
#if 0
#define IFF_BROADCAST   0x0002      /* broadcast address valid */
#define IFF_DEBUG   0x0004      /* turn on debugging */
#define IFF_LOOPBACK    0x0008      /* is a loopback net */
#define IFF_POINTOPOINT 0x0010      /* interface is point-to-point link */
#define IFF_NOTRAILERS  0x0020      /* avoid use of trailers */
#define IFF_NOARP   0x0080      /* no address resolution protocol */
#define IFF_PROMISC 0x0100      /* receive all packets */
#define IFF_ALLMULTI    0x0200      /* receive all multicast packets */
#define IFF_OACTIVE 0x0400      /* transmission in progress */
#define IFF_SIMPLEX 0x0800      /* can't hear own transmissions */
#define IFF_LINK0   0x1000      /* per link layer defined bit */
#define IFF_LINK1   0x2000      /* per link layer defined bit */
#define IFF_LINK2   0x4000      /* per link layer defined bit */
#define IFF_MULTICAST   0x8000      /* supports multicast */


/* Zebra API message flag. */
#define ZAPI_MESSAGE_NEXTHOP  0x01
#define ZAPI_MESSAGE_IFINDEX  0x02
#define ZAPI_MESSAGE_DISTANCE 0x04
#define ZAPI_MESSAGE_METRIC   0x08
#define ZAPI_MESSAGE_MTU      0x10
#endif

/* RIP output routes type. */
enum
{
    rip_all_route,
    rip_changed_route
};

//extern pthread_mutex_t pkt_lock;

struct stats
{
    unsigned long rx_packets;   /* total packets received       */
    unsigned long tx_packets;   /* total packets transmitted    */
    unsigned long rx_errors;    /* bad packets received         */
    unsigned long tx_failed;
    unsigned long rx_routes;
    unsigned long rx_bad_routes;       /* receiver ring buff overflow  */
    unsigned long rx_auth_failed;        /* recved pkt with crc error    */
    unsigned long tx_routes;
};


/* Redistribute information. */
struct redist_msg
{
    /* redistribute type */
    enum ROUTE_PROTO type;

    /* redistribute instance number */
    u_char id;

    /* redistribute instance metric */
    u_int32_t metric;
};


/* RIP routing table entry which belong to rip_packet. */
struct ipc_dec_msg
{
    u_char type;
    u_char flag;
    u_char msg;
    safi_t safi;
    u_char nexthop_num;
    struct in_addr **nexthop;
    u_char ifindex_num;
    ifindex_t *ifindex;
    u_char distance;
    u_char metrics;
    u_int32_t mtu;
    vrf_id_t vrf_id;
};
extern struct rip_fifo_master rip_m;

/* pkt cache fifo */
struct pkteventfifo
{
    struct fifo fifo;
    struct pkt_buffer pkt;	
	uint8_t pkt_data[IPC_MSG_LEN1];
};

struct rip_fifo_master
{
    //struct thread *t_thread_routefifo;    /*route thread*/
	TIMERID			t_thread_routefifo;    /*route thread*/
    struct fifo routefifo;    /*route fifo*/
    struct fifo ifmfifo;  /*ifm fifo*/
    struct fifo pktfifo;  /*pkt fifo*/
    struct stream *pktsend_buf;

    //struct thread *pkt_recv;  /* rip read pkt thread */
    //struct thread *ifm_recv;  /* rip read pkt thread */
    //struct thread *route_recv;  /* rip read pkt thread */

};

/* RIP structure. */
struct rip_instance
{
    /* RIP id. */
    u_char id;

    /* rip triggered update flag */
    u_char triggered_update_flag;
    /* RIP enable interface count. */
    u_char inter_count;

    /* rip summary all subnet routes flag */
    u_char summaryflag;
    u_int32 inter_sum_count;
    //struct thread *t_summary_set;
    //struct thread *t_summary_unset;
	TIMERID			t_summary_set;
    TIMERID			t_summary_unset;

    /* Default version of rip instance. */
    int version_send; /* version 1 or 2 (but not both) */
    int version_recv; /* version 1 or 2 or both */
    //struct thread *t_version2;
	TIMERID			t_version2;

    /* RIP routing information base. */
    struct route_table *table;

    /* Linked list of RIP peer. */
    struct list *peer_list;

    /* RIP threads. */
//  struct thread *t_read;

    /* Update and garbage timer. */
    //struct thread *t_update;
	TIMERID			t_update;

    /* Triggered update hack. */
    int trigger;
    //struct thread *t_triggered_update;
    //struct thread *t_triggered_interval;
	TIMERID			t_triggered_update;
    TIMERID			t_triggered_interval;

    /* RIP timer values. */
    unsigned long update_time;
    unsigned long timeout_time;
    unsigned long garbage_time;

    /* RIP default metric. */
    int default_metric;

    /* RIP default-information originate. */
    char *default_information_route_map;

    /* Redistribute information. */
    struct list *redist_infos;

    /* RIP default distance. */
    u_char distance;
    struct route_table *distance_table;

    /* For redistribute route map. */
    struct
    {
        char *name;
        struct route_map *map;
        int metric_config;
        u_int32_t metric;
    } route_map[ROUTE_PROTO_NUM];
};

/* RIPng structure. */
struct ripng_instance
{
    /* RIPng instance id. */
    u_char id;

	/* enabled interface */
	uint32_t ifcount;
	
    /* RIPng Parameters.*/
    u_char command;
    u_char version;
    unsigned long update_time;
    unsigned long timeout_time;
    unsigned long garbage_time;
    int max_mtu;
    int default_metric;

    u_char distance;

    /* RIPng routing information base. */
    struct route_table *table;

    /* RIPng aggregate route information. */
    struct route_table *aggregate;

    struct list *peer_list;

    /* RIPng threads. */
    //struct thread *t_update;
    //struct thread *t_garbage;
	TIMERID			t_update;
    TIMERID			t_garbage;

    /* Triggered update hack. */
    int trigger;
    //struct thread *t_triggered_update;
    //struct thread *t_triggered_interval;
	TIMERID			t_triggered_update;
    TIMERID			t_triggered_interval;
	u_char triggered_flag;

    /* RIPng ECMP flag */
    unsigned int ecmp;

    /* Redistribute information. */
    struct list *redist_infos;

    /* For redistribute route map. */
    struct
    {
        char *name;
        struct route_map *map;
        int metric_config;
        u_int32_t metric;
    } route_map[ROUTE_PROTO_NUM];
};

/* RIP routing table entry which belong to rip_packet. */
struct rte
{
    u_int16_t family;     /* Address family of this route. */
    u_int16_t tag;        /* Route Tag which included in RIP2 packet. */
    struct in_addr prefix;    /* Prefix of rip route. */
    struct in_addr mask;      /* Netmask of rip route. */
    struct in_addr nexthop;   /* Next hop of rip route. */
    u_int32_t metric;     /* Metric value of rip route. */
};

/* RIP packet structure. */
struct rip_packet
{
    unsigned char command;    /* Command type of RIP packet. */
    unsigned char version;    /* RIP version which coming from peer. */
    unsigned char pad1;       /* Padding of RIP packet header. */
    unsigned char pad2;       /* Same as above. */
    struct rte rte[1];        /* Address structure. */
};

/* Buffer to read RIP packet. */
union rip_buf
{
    struct rip_packet rip_packet;
    char buf[RIP_PACKET_MAXSIZ];
};

/* RIP route information. */
struct rip_info
{
    /* This route's type. */
    enum ROUTE_PROTO type;

    /*rip instance number */
    u_char id;

    /*external instance number */
    u_char external_id;

    /* Sub type. */
    int sub_type;

    /* summary flag. */
    u_char summary_flag;
    /* summary count configured under interface */
    u_char summary_count;
    /* summay routes count */

    /* RIP nexthop. */
    struct in_addr nexthop;
    struct in_addr from;

    /* Which interface does this route come from. */
    ifindex_t ifindex;

    /* add or update by which packet */
    unsigned long rx_packets;

    /* Metric of this route. */
    u_int32_t metric;

    /* External metric of this route.
       if learnt from an externalm proto */
    u_int32_t external_metric;

    /* Tag information of this route. */
    u_int16_t tag;

    /* Flags of RIP route. */
#define RIP_RTF_FIB      1
#define RIP_RTF_CHANGED  2
    u_char flags;

    /* Garbage collect timer. */
    //struct thread *t_timeout;
    //struct thread *t_garbage_collect;
	TIMERID			t_timeout;
    TIMERID			t_garbage_collect;

    /* Route-map futures - this variables can be changed. */
    struct in_addr nexthop_out;
    u_char metric_set;
    u_int32_t metric_out;
    u_int32_t mmt_count;
    u_short tag_out;
    ifindex_t ifindex_out;

    struct route_node *rp;

    u_char distance;


#ifdef NEW_RIP_TABLE
    struct rip_info *next;
    struct rip_info *prev;
#endif /* NEW_RIP_TABLE */
};

typedef enum
{
    RIP_NO_SPLIT_HORIZON = 0,
    RIP_SPLIT_HORIZON,
    RIP_SPLIT_HORIZON_POISONED_REVERSE,
    RIP_NO_POISON_REVERSE
} split_horizon_policy_t;

struct rip_summary
{
    struct prefix p;
    char dflag;
    char uflag;
    struct rip_interface *ri;
    struct route_node *tablenode;

    //struct thread *t_sup;
    //struct thread *t_sdown;
	TIMERID			t_sup;
    TIMERID			t_sdown;
};

/* RIP specific interface configuration. */
struct rip_interface
{
    /* RIP is enabled on this interface. */

    /* enable this interface under its node. */
    unsigned char enable_inter_node;

    /* enable this interface under its node. */
    unsigned char ripng_enable;

//    struct prefix lladdr;
    /* interface mode. */
    int mode;

    /* RIP is running on this interface. */
    int running;

    /* RIP version control. */
    int ri_send;
    int ri_receive;

    /* RIPv2 authentication type. */
    int auth_type;

    /* RIPv2 authentication string. */
    char auth_str[RIP_AUTH_MD5_COMPAT_SIZE+1];

    /* RIPv2 authentication key chain. */
    char *key_chain;

    /* MD5 key id. */
    unsigned char md5_auth_id;
    /* value to use for md5->auth_len */
    u_int8_t md5_auth_len;

    /* Split horizon flag. */
    split_horizon_policy_t split_horizon;
    split_horizon_policy_t poison_reverse;

    /* Split horizon flag for ipv6. */
    split_horizon_policy_t ipv6_split_horizon;
    split_horizon_policy_t ipv6_poison_reverse;
    /* Passive interface for ipv6. */
    int ipv6_passive;

	/* ipv6 Interface metric */
	int ipv6_metric;

    /* For filter type slot. */
#define RIP_FILTER_IN  0
#define RIP_FILTER_OUT 1
#define RIP_FILTER_MAX 2

    /* Access-list. */
    struct access_list *list[RIP_FILTER_MAX];

    /* Prefix-list. */
    struct prefix_list *prefix[RIP_FILTER_MAX];

    /* Route-map. */
    struct route_map *routemap[RIP_FILTER_MAX];

    /* Wake up thread. */
//    struct thread *t_wakeup;

    /* Interface statistics. */
    int recv_badpackets;
    int recv_badroutes;
    int sent_updates;
    int receive_packet;

    /* Passive interface. */
    int passive;

    /*interface summary prefix list */
    struct list *summary;

    /*interface neighbor prefix list */
    struct list *neighbor;
};

/* RIP peer information. */
struct rip_peer
{
    /* Peer address. */
    struct in_addr addr;

    /* connected interface ifindex */
    ifindex_t ifindex;

    /* rip instance id */
    unsigned char id;

    /* Peer RIP tag value. */
    int domain;

    /* Last update time. */
    time_t uptime;

    /* Peer RIP version. */
    u_char version;

    /* Statistics. */
    int recv_badpackets;
    int recv_badroutes;

    /* Timeout thread. */
    //struct thread *t_timeout;
	TIMERID			t_timeout;
};

struct rip_md5_info
{
    u_int16_t family;
    u_int16_t type;
    u_int16_t packet_len;
    u_char keyid;
    u_char auth_len;
    u_int32_t sequence;
    u_int32_t reserv1;
    u_int32_t reserv2;
};

struct rip_md5_data
{
    u_int16_t family;
    u_int16_t type;
    u_char digest[16];
};

/* RIP accepet/announce methods. */
#define RI_RIP_UNSPEC                      0
#define RI_RIP_VERSION_1                   1
#define RI_RIP_VERSION_2                   2
#define RI_RIP_VERSION_1_AND_2             3
#define RI_RIP_VERSION_2_MULTI             6
#define RI_RIP_VERSION_2_BROAD             7
/* N.B. stuff will break if
    (RIPv1 != RI_RIP_VERSION_1) || (RIPv2 != RI_RIP_VERSION_2) */

/* Default value for "default-metric" command. */
#define RIP_DEFAULT_METRIC_DEFAULT         0

/* RIP event. */
enum rip_event
{
    RIP_READ,
    RIP_UPDATE_EVENT,
    RIP_TRIGGERED_UPDATE,
};

#if 0
/* Macro for timer turn on. */
#define RIP_TIMER_ON(T,F,V) \
  do { \
    if (!(T)) \
      (T) = thread_add_timer (master_rip, (F), rinfo, (V)); \
  } while (0)

/* Macro for timer turn off. */
#define RIP_TIMER_OFF(X) \
  do { \
    if (X) \
      { \
        thread_cancel (X); \
        (X) = NULL; \
      } \
  } while (0)
#endif

/* Macro for timer turn on. */
#define RIP_TIMER_ON(T,F,V) \
	  do { \
		if (!(T)) \
		  (T) = high_pre_timer_add ((char *)"rip_timer", LIB_TIMER_TYPE_NOLOOP, (F), rinfo, (V)*1000);\
	  } while (0)
	
/* Macro for timer turn off. */
#define RIP_TIMER_OFF(X) \
	  do { \
		if (X) \
		  { \
			high_pre_timer_delete (X);\
			(X) = 0; \
		  } \
	  } while (0)

#if 0
/* Macro for rip_summary under interface timer turn on. */
#define RS_TIMER_ON(T,F,V) \
      do { \
        if (!(T)) \
          (T) = thread_add_timer (master_rip, (F), rs, (V)); \
      } while (0)

/* Macro for timer turn off. */
#define RS_TIMER_OFF(X) \
      do { \
        if (X) \
          { \
            thread_cancel (X); \
            (X) = NULL; \
          } \
      } while (0)

#endif
/* Macro for rip_summary under interface timer turn on. */
#define RS_TIMER_ON(T,F,V) \
	  do { \
		if (!(T)) \
		  (T) = high_pre_timer_add ((char *)"rip_sum_timer", LIB_TIMER_TYPE_NOLOOP, (F), rs, (V)*1000);\
	  } while (0)
		
/* Macro for timer turn off. */
#define RS_TIMER_OFF(X) \
	  do { \
		if (X) \
		  { \
			high_pre_timer_delete (X);\
			(X) = 0; \
		  } \
	  } while (0)

/* Prototypes. */
extern char *inet_ntoa(struct in_addr);
extern int inet_aton(const char *, struct in_addr *);
extern unsigned long stream_get_endp(struct stream *);
//extern void sockopt_iphdrincl_swab_htosys (struct ip *);
//extern int check_plens_equal(struct prefix *p);
extern struct rip_info * rip_info_new(void);
extern void rip_info_free(struct rip_info *);
//extern int rip_garbage_collect(void *);
extern void rip_init(void);
extern void rip_reset(void);
extern void global_rip_m_free(void);
extern struct rip_instance *rip_instance_get(struct vty *, const u_char);
extern int rip_instance_delete(struct vty *, const u_char);
extern void rip_clean_all(void);
extern void rip_clean(struct rip_instance *);
extern void rip_interface_clean(const char);
extern void rip_interface_reset(void);
extern void rip_if_init(void);
extern void rip_if_down_all(void);
extern void rip_route_map_init(void);
extern void rip_route_map_reset(void);
extern void rip_snmp_init(void);
extern void rip_zebra_init(void);
extern void ripng_zebra_init(void);
extern void rip_offset_init(void);
extern int if_check_address(struct in_addr addr);
extern int rip_peer_list_cmp(struct rip_peer *, struct rip_peer *);

extern int rip_request_send(struct sockaddr_in *, struct interface *, u_char,
                            struct connected *);
extern int rip_neighbor_lookup(struct sockaddr_in *, struct rip_instance *);
extern int rip_ecmp_route_cmp(struct rip_info *, struct rip_info *);
extern int rip_redistribute_check(struct rip_instance *, int);
extern void rip_redistribute_add(struct rip_instance *, struct rip_info *, struct prefix_ipv4 *);
extern void rip_redistribute_delete(struct rip_instance *, struct rip_info *, struct prefix_ipv4 *);
extern void rip_redistribute_withdraw(enum ROUTE_PROTO , int , struct rip_instance *);
extern void rip_zebra_ipv4_add(struct route_node *);
extern void rip_zebra_ipv4_update(struct route_node *);
extern void rip_zebra_ipv4_delete(struct route_node *);
extern void rip_distribute_update_interface(struct interface *);
extern void rip_if_rmap_update_interface(struct interface *);
extern int rip_route_rte(struct rip_info *);
extern int config_write_rip(struct vty *);
extern int config_write_rip_offset_list(struct vty *);
extern int config_write_rip_redistribute(struct vty *, struct rip_instance *);
extern int config_write_ripng_redistribute(struct vty *, struct ripng_instance *);
extern void rip_event(enum rip_event, struct rip_instance *);
extern void rip_peer_init(void);
extern void rip_peer_update(struct rip_instance *, struct sockaddr_in *, u_char, ifindex_t ifindex);
extern void rip_peer_display(struct vty *);
extern struct rip_peer *rip_peer_lookup(struct rip_instance *, struct in_addr *);
extern struct rip_peer *rip_peer_lookup_next(struct rip_instance *, struct in_addr *);
extern void rip_peer_free(struct rip_peer *);
extern int rip_offset_list_apply_in(struct prefix_ipv4 *, struct interface *, u_int32_t *);
extern int rip_offset_list_apply_out(struct prefix_ipv4 *, struct interface *, u_int32_t *);
extern void rip_offset_clean(void);
extern void rip_redistribute_clean(struct list *);
extern void rip_ifaddr_add(struct interface *, struct connected *);
extern void rip_ifaddr_delete(struct interface *, struct connected *);
extern struct rip_info *rip_ecmp_add(struct rip_info *, struct rip_instance *);
extern struct rip_info *rip_ecmp_replace(struct rip_info *, struct rip_instance *);
extern struct rip_info *rip_ecmp_delete(struct rip_info *, struct rip_instance *);
extern void ripng_zebra_ipv6_add(struct route_node *);
extern void ripng_zebra_ipv6_delete(struct route_node *);
extern void ripng_zebra_ipv6_update(struct route_node *);
extern int ripng_if_down(struct interface *, int);
extern void ripng_connect_set(struct interface *, int, struct ripng_instance *);
extern void ripng_enable_apply(struct interface *, struct ripng_instance *);
extern const char *rip_route_string(unsigned int);
extern void rip_update_process(int, struct rip_instance *, u_int32_t);
extern void rip_update_neighbor_spec(struct interface *, int , struct rip_instance *, u_int32_t);
extern struct rip_instance *rip_instance_lookup(const u_char);
extern struct route_node *rip_prefix_match(struct route_table *, struct prefix *);
extern int rip_summary_on(void *);
extern int rip_summary_off(void *);
extern void rip_auto_summary_set_interface(struct rip_instance *);
extern struct listnode *ri_summary_prefix_match_lookup(struct list *, struct prefix *);
extern void rip_apply_classful_mask_ipv4(struct prefix_ipv4 *);
extern struct route_node * rip_inter_summary_prefix_match(struct prefix *);
extern int rip_update_interface_spec(int, struct interface *, struct rip_instance *, u_int32_t);
extern int rip_route_read(struct ipc_msghdr_n *, void *);
extern int rip_send_route_fifo(void *);
extern char rip_zebra_route_char(u_int);
extern int rip_ip_address_self_check(struct in_addr *);
//extern void rip_wrong_size_pkt_dump (void *, int );

/* Master thread strucutre. */
extern struct thread_master *master_rip;

/* RIP statistics for SNMP. */
extern long rip_global_route_add;
extern long rip_global_route_del;
extern long rip_global_queries;

int rip_garbage_collect(void *);
void rip_rte_process(struct rte *, struct sockaddr_in *,
                struct interface *, unsigned char , struct rip_instance *);
void rip_response_process(struct rip_packet *packet, int size,
                     struct sockaddr_in *from, struct connected *ifc, struct rip_instance *rip, struct rip_interface *ri);
int rip_read_pkt(struct sockaddr_in from, uint32_t in_ifindex, void *ibuf, uint16_t len);
int rip_ipv4_or6_seperate(struct pkt_buffer *pkt);
//int add_rip_fifo_pkt(struct pkt_buffer *recvpkt);
//int rip_pkt_rcv(struct thread *thread);
int rip_pkt_rcv_new(struct ipc_mesg_n *pmsg);
//void read_pkt_form_pktfifo(void);
void rip_output_process(struct connected *, struct sockaddr_in *, int, u_char, u_int32_t);
int rip_triggered_update(void *);
int rip_update_jitter(unsigned long);
int rip_timeout(void *t);
void rip_timeout_update(struct rip_info *rinfo, struct rip_instance *rip);
void rip_timeout_update(struct rip_info *rinfo, struct rip_instance *rip);
int rip_destination_check(struct in_addr addr);
int check_plens_equal(struct prefix *p);
int rip_auto_summary_check_sub_routes(struct route_node *rp, struct prefix *sump, ifindex_t ifindex);
int rip_summary_check_sub_routes(struct route_node *rp);
struct listnode * interface_summary_prefix_same_lookup(struct list *list, struct prefix *p);
void rip_summary_all_check(struct rip_info *rinfo_new, struct route_node *rp, u_int32_t opflag, u_int32_t metric, struct rip_instance *rip);
void rip_summary_count_check(struct rip_info *rinfo_new, struct route_node *rp, u_int32_t opflag, u_int32_t metric, struct rip_instance *rip);
void rip_packet_dump(struct rip_packet *packet, int size, const char *sndrcv);
void rip_wrong_size_pkt_dump(void *data, int len);
int rip_send_packet(u_char * buf, int size, struct sockaddr_in *to,
                struct connected *ifc, u_char version);
void rip_request_process(struct rip_packet *packet, int size,
                    struct sockaddr_in *from, struct connected *ifc);
int rip_write_rte(int num, struct stream *s, struct prefix_ipv4 *p,
              u_char version, struct rip_info *rinfo, u_int32_t metric, struct connected *ifc);
int rip_version2_ruoute_send_check(struct rip_instance *rip, struct route_node *rp,
                               struct connected *ifc, u_int32_t *metcount, struct rip_info *rinfo);
int rip_version1_send_route_check(struct route_node *rp, struct connected *ifc);
struct rip_info * rip_version1_if_class_route_check(struct route_node *rp, struct connected *ifc);
int rip_update(void *t);
void rip_clear_changed_flag(struct rip_instance *rip);
int rip_triggered_interval(void *t);
struct rip_instance * rip_create(const u_char id);
int rip_update_jitter(unsigned long time);
void rip_table_distance_change(struct rip_instance *rip);
int rip_version2_on(void *t);
const struct rip_desc_table * rip_zroute_lookup(u_int zroute);
//void rip_vty_out_uptime(struct vty *vty, struct rip_info *rinfo);
void rip_vty_out_uptime_new(struct vty *vty, struct rip_info *rinfo, struct rip_instance *rip);
void rip_distribute_update(struct distribute *dist);
void rip_distribute_update_all(struct prefix_list *notused);
void rip_distribute_update_all_wrapper(struct access_list *notused);


#define VTY_INTERFACE_MODE_ERR                                      \
{                                                                       \
    vty_error_out(vty,"Please select mode l3.%s",VTY_NEWLINE);        \
    return CMD_WARNING;                                                 \
}

#define VTY_RIP_INSTANCE_ERR                                      \
{                                                                       \
    vty_error_out(vty,"The specified instance does not exist.%s",VTY_NEWLINE);        \
    return CMD_WARNING;                                                 \
}

#define VTY_CHECK_NM_INTERFACE(V)                                     \
{                                                                       \
    if(IFM_TYPE_IS_OUTBAND (V))               \
    {                                               \
        vty_error_out(vty,"Outband is not support.%s",VTY_NEWLINE);        \
        return CMD_WARNING;                                                 \
    }                                                                       \
}                                                                           \
 
#endif /* _ZEBRA_RIP_H */
