/*
 * RIPng related value and structure.
 * Copyright (C) 1998 Kunihiro Ishiguro
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

#ifndef _ZEBRA_RIPNG_RIPNGD_H
#define _ZEBRA_RIPNG_RIPNGD_H

//#include <zclient.h>
//#include "lib/vty.h"
//#include "lib/inet_ip.h"
//#include "lib/route_com.h"
#include "lib/hptimer.h"
//#include "if.h"
#include "ripd/ripd.h"

/* RIPng version and port number. */
#define RIPNG_V1                         1
#define RIPNG_PORT_DEFAULT             521
#define RIPNG_VTY_PORT                2603
#define RIPNG_MAX_PACKET_SIZE         1500
#define RIPNG_PRIORITY_DEFAULT           0

/* RIPng commands. */
#define RIPNG_REQUEST                    1
#define RIPNG_RESPONSE                   2

/* RIPng metric and multicast group address. */
#define RIPNG_METRIC_INFINITY           16
#define RIPNG_METRIC_NEXTHOP          0xff
#define RIPNG_GROUP              "ff02::9"

/* RIPng timers. */
#define RIPNG_UPDATE_TIMER_DEFAULT      30
#define RIPNG_TIMEOUT_TIMER_DEFAULT    180
#define RIPNG_GARBAGE_TIMER_DEFAULT    120

/* RIPng peer timeout value. */
#define RIPNG_PEER_TIMER_DEFAULT       180

/* Default config file name. */
#define RIPNG_DEFAULT_CONFIG "ripngd.conf"

/* RIPng route types. */
#define RIPNG_ROUTE_REDISTRIBUTE         1
#define RIPNG_ROUTE_RTE                  2
#define RIPNG_ROUTE_INTERFACE            4
#define RIPNG_ROUTE_AGGREGATE            8

/* Interface send/receive configuration. */
#define RIPNG_SEND_UNSPEC                0
#define RIPNG_SEND_OFF                   1
#define RIPNG_RECEIVE_UNSPEC             0
#define RIPNG_RECEIVE_OFF                1

/* RIP default route's accept/announce methods. */
#define RIPNG_DEFAULT_ADVERTISE_UNSPEC   0
#define RIPNG_DEFAULT_ADVERTISE_NONE     1
#define RIPNG_DEFAULT_ADVERTISE          2

#define RIPNG_DEFAULT_ACCEPT_UNSPEC      0
#define RIPNG_DEFAULT_ACCEPT_NONE        1
#define RIPNG_DEFAULT_ACCEPT             2

/* Default value for "default-metric" command. */
#define RIPNG_DEFAULT_METRIC_DEFAULT     0

/* For max RTE calculation. */
#ifndef IPV6_HDRLEN
#define IPV6_HDRLEN 40
#endif /* IPV6_HDRLEN */

#ifndef IFMINMTU
#define IFMINMTU    576
#endif /* IFMINMTU */

enum
{
    ripng_all_route,
    ripng_changed_route
};


extern struct list *ripng_list;

/* Routing table entry. */
struct rte_ipv6
{
    struct in6_addr addr; /* RIPng destination prefix */
    u_short tag;      /* RIPng tag */
    u_char prefixlen; /* Length of the RIPng prefix */
    u_char metric;    /* Metric of the RIPng route */
    /* The nexthop is stored by the structure
     * ripng_nexthop within ripngd.c */
};

/* RIPNG send packet. */
struct ripng_packet
{
    u_char command;
    u_char version;
    u_int16_t zero;
    struct rte_ipv6 rte[1];
};

/* Each route's information. */
struct ripng_info
{
    /* This route's type.  Static, ripng or aggregate. */
    u_char type;

    /* Sub type for static route. */
    u_char sub_type;

    u_char id;
    uint8_t external_id;

    /* RIPng specific information */
    struct in6_addr nexthop;
    struct in6_addr from;

    /* Which interface does this route come from. */
    ifindex_t ifindex;


    /* Metric of this route.  */
    u_char metric;

    /* External metric of this route.
       if learnt from an externalm proto */
    u_int32_t external_metric;

    u_char distance;

    /* Tag field of RIPng packet.*/
    u_int16_t tag;

    /* For aggregation. */
    unsigned int suppress;

    /* Flags of RIPng route. */
#define RIPNG_RTF_FIB      1
#define RIPNG_RTF_CHANGED  2
    u_char flags;

    /* Garbage collect timer. */
    //struct thread *t_timeout;
    //struct thread *t_garbage_collect;
	TIMERID			t_timeout;
    TIMERID			t_garbage_collect;

	/* point to route_node this ripng_info belongs to */
    struct route_node *rp;
	
    /* Route-map features - this variables can be changed. */
    struct in6_addr nexthop_out;
    u_char metric_set;
    u_char metric_out;
    u_short tag_out;

};

/* RIPng peer information. */
struct ripng_peer
{
    /* Peer address. */
    struct in6_addr addr;

    /* connected interface ifindex */
    ifindex_t ifindex;

    u_char id;

    /* Peer RIPng tag value. */
    int domain;

    /* Last update time. */
    time_t uptime;

    /* Statistics. */
    int recv_badpackets;
    int recv_badroutes;

    /* Timeout thread. */
    //struct thread *t_timeout;
	TIMERID			t_timeout;
};

/* All RIPng events. */
enum ripng_event
{
    RIPNG_READ,
    RIPNG_ZEBRA,
    RIPNG_REQUEST_EVENT,
    RIPNG_UPDATE_EVENT,
    RIPNG_TRIGGERED_UPDATE
};

#if 0
/* RIPng timer on/off macro. */
#define RIPNG_TIMER_ON(T,F,V) \
do { \
   if (!(T)) \
      (T) = thread_add_timer (master_rip, (F), rinfo, (V)); \
} while (0)

#define RIPNG_TIMER_OFF(T) \
do { \
   if (T) \
     { \
       thread_cancel(T); \
       (T) = NULL; \
     } \
} while (0)
#endif

/* RIPng timer on/off macro. */
#define RIPNG_TIMER_ON(T,F,V) \
   do { \
	  if (!(T)) \
		 (T) = high_pre_timer_add ((char *)"ripng_timer", LIB_TIMER_TYPE_NOLOOP, (F), rinfo, (V)*1000);\
   } while (0)
   
#define RIPNG_TIMER_OFF(T) \
   do { \
	  if (T) \
		{ \
		  high_pre_timer_delete (T); \
		  (T) = 0; \
		} \
   } while (0)


/* Count prefix size from mask length */
//#define PSIZE(a) (((a) + 7) / (8))

/* Extern variables. */
//extern struct ripng *ripng;

//extern struct thread_master *master;

/* Prototypes. */
extern void ripng_init(void);
extern void ripng_clean(struct ripng_instance *);
extern void ripng_clean_all(void);
extern void ripng_route_map_init(void);
extern int config_write_ripng_offset_list(struct vty *);
extern void ripng_peer_update(struct sockaddr_in6 *, struct ripng_instance *, ifindex_t);
extern void ripng_peer_bad_route(struct sockaddr_in6 *, struct list *);
extern void ripng_peer_bad_packet(struct sockaddr_in6 *, struct list *);
extern void ripng_peer_display(struct vty *, struct list *);
extern struct ripng_peer *ripng_peer_lookup(struct in6_addr *, struct list *);
extern struct ripng_peer *ripng_peer_lookup_next(struct in6_addr *, struct list *);

extern int ripng_offset_list_apply_in(struct prefix_ipv6 *,
                                      struct interface *, u_char *);
extern int ripng_offset_list_apply_out(struct prefix_ipv6 *,
                                       struct interface *, u_char *);
extern int ripng_request(struct interface *ifp);
extern int ripng_write_rte(int num, struct stream *s, struct prefix_ipv6 *p, struct in6_addr *nexthop,
                           u_int16_t tag, u_char metric);
extern void ripng_offset_clean(void);
extern void ripng_redistribute_add(struct ripng_info *, struct prefix_ipv6 *, struct ripng_instance *);
extern void ripng_redistribute_delete(struct ripng_info *, struct prefix_ipv6 *, struct ripng_instance *);
extern void ripng_redistribute_withdraw(enum ROUTE_PROTO type, int id, struct ripng_instance *ripng);
extern int ripng_send_packet(u_char *, int, struct sockaddr_in6 *, struct connected*);
extern void ripng_packet_dump(struct ripng_packet *, int , const char *);
extern struct ripng_info * ripng_ecmp_delete(struct ripng_info *, struct ripng_instance *);

extern struct ripng_info * ripng_info_new(void);
extern void ripng_info_free(struct ripng_info *rinfo);
extern void ripng_event(enum ripng_event, struct ripng_instance *);
extern int ripng_read(struct sockaddr_in6, uint32_t, void *, uint16_t);
extern int ripng_route_read_ipv6(uint8_t, struct route_entry *);
extern void ripng_if_init();
extern struct ripng_instance *ripng_instance_lookup(const u_char);
extern struct ripng_instance * ripng_instance_get(struct vty *, const u_char);
extern int ripng_instance_delete(struct vty *, const u_char);
extern void get_interface_for_short(ifindex_t, char *);
extern struct ripng_instance *ripng_create(const u_char);
extern int ripng_peer_list_cmp(struct ripng_peer *, struct ripng_peer *);
extern void ripng_peer_init(void);


void ripng_output_process(struct connected *, struct sockaddr_in6 *, int,
                     struct ripng_instance *, u_int32_t);
struct connected * connected_lookup_localaddress (struct interface *ifp);
int ripng_lladdr_check(struct interface *ifp, struct in6_addr *addr);
int ripng_garbage_collect(void *t);
struct ripng_info * ripng_ecmp_add(struct ripng_info *rinfo_new, struct ripng_instance *ripng);
struct ripng_info * ripng_ecmp_replace(struct ripng_info *rinfo_new, struct ripng_instance *ripng);
int ripng_timeout(void *t);
void ripng_timeout_update(struct ripng_info *rinfo, struct ripng_instance *ripng);
void ripng_request_process(struct ripng_packet *packet, int size,
                      struct sockaddr_in6 *from, struct connected *ifc, struct ripng_instance *ripng);
void ripng_clear_changed_flag(struct ripng_instance *ripng);
int ripng_update(void *t);
int ripng_triggered_interval(void *t);
int ripng_update_process(int route_type, struct ripng_instance *ripng, u_int32_t metric);
void ripng_table_distance_change(struct ripng_instance *ripng);
int ripng_config_write(struct vty *vty);
void ripng_interface_reset(u_char id);



#endif /* _ZEBRA_RIPNG_RIPNGD_H */
