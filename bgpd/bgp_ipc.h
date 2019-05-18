/* Zebra's client header.
 * Copyright (C) 1999 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _BGP_IPC_H
#define _BGP_IPC_H

#include <lib/types.h>

/* For struct zapi_ipv{4,6}. */
#include <lib/prefix.h>

/* For struct interface and struct connected. */
#include <lib/if.h>

/* For vrf_bitmap_t. */
#include <lib/vrf.h>

#include <lib/table.h>
#include <lib/ifm_common.h>
#include <lib/msg_ipc_n.h>

/*interface state add by tai*/
#define IFF_UP      0x0001      /* interface is up */
#define IFF_BROADCAST   0x0002      /* broadcast address valid */
#define IFF_DEBUG   0x0004      /* turn on debugging */
#define IFF_LOOPBACK    0x0008      /* is a loopback net */
#define IFF_POINTOPOINT 0x0010      /* interface is point-to-point link */
//#define   IFF_NOTRAILERS  0x0020      /* avoid use of trailers */
#define IFF_RUNNING 0x0040      /* resources allocated */
#define IFF_NOARP   0x0080      /* no address resolution protocol */
#define IFF_PROMISC 0x0100      /* receive all packets */
#define IFF_ALLMULTI    0x0200      /* receive all multicast packets */
//#define   IFF_OACTIVE 0x0400      /* transmission in progress */
//#define   IFF_SIMPLEX 0x0800      /* can't hear own transmissions */
//#define   IFF_LINK0   0x1000      /* per link layer defined bit */
//#define   IFF_LINK1   0x2000      /* per link layer defined bit */
//#define   IFF_LINK2   0x4000      /* per link layer defined bit */
#define IFF_MULTICAST   0x8000      /* supports multicast */

extern struct bgp_damp_config *damp;


/* For input/output buffer to zebra. */
#define ZEBRA_MAX_PACKET_SIZ          4096

enum nexthop_types_t
{
    NEXTHOP_TYPE_IFINDEX = 1,      /* Directly connected.  */
    NEXTHOP_TYPE_IFNAME,           /* Interface route.  */
    NEXTHOP_TYPE_IPV4,             /* IPv4 nexthop.  */
    NEXTHOP_TYPE_IPV4_IFINDEX,     /* IPv4 nexthop with ifindex.  */
    NEXTHOP_TYPE_IPV4_IFNAME,      /* IPv4 nexthop with ifname.  */
    NEXTHOP_TYPE_IPV6,             /* IPv6 nexthop.  */
    NEXTHOP_TYPE_IPV6_IFINDEX,     /* IPv6 nexthop with ifindex.  */
    NEXTHOP_TYPE_IPV6_IFNAME,      /* IPv6 nexthop with ifname.  */
    NEXTHOP_TYPE_BLACKHOLE,        /* Null0 nexthop.  */
};

/* Routing information base. */

union g_addr
{
    struct in_addr ipv4;
#ifdef HAVE_IPV6
    struct in6_addr ipv6;
#endif /* HAVE_IPV6 */
};

/* Zebra API message flag. */
#define ZAPI_MESSAGE_NEXTHOP  0x01
#define ZAPI_MESSAGE_IFINDEX  0x02
#define ZAPI_MESSAGE_DISTANCE 0x04
#define ZAPI_MESSAGE_METRIC   0x08
#define ZAPI_MESSAGE_MTU      0x10


/* Zebra IPv4 route message API. */

#ifdef HAVE_IPV6
/* IPv6 prefix add and delete function prototype. */

struct zapi_ipv6
{
    u_char type;

    u_char flags;

    u_char message;

    safi_t safi;

    u_char nexthop_num;
    struct in6_addr **nexthop;

    u_char ifindex_num;
    ifindex_t *ifindex;

    u_char distance;

    u_int32_t metric;

    u_int32_t mtu;

    vrf_id_t vrf_id;
};

/* Structure for the zebra client. */
struct zclient
{
    /* The thread master we schedule ourselves on */
    struct thread_master *master;

    /* Socket to zebra daemon. */
    int sock;

    /* Flag of communication to zebra is enabled or not.  Default is on.
       This flag is disabled by `no router zebra' statement. */
    int enable;

    /* Connection failure count. */
    int fail;

    /* Input buffer for zebra message. */
    struct stream *ibuf;

    /* Output buffer for zebra message. */
    struct stream *obuf;

    /* Buffer of data waiting to be written to zebra. */
    struct buffer *wb;

    /* Read and connect thread. */
    struct thread *t_read;
    struct thread *t_connect;

    /* Thread to write buffered data to zebra. */
    struct thread *t_write;

    /* Redistribute information. */
    u_char redist_default;
    vrf_bitmap_t redist[ROUTE_PROTO_MAX];

    /* Redistribute defauilt. */
    vrf_bitmap_t default_information;

    /* Pointer to the callback functions. */
    void (*zebra_connected) (struct zclient *);
    int (*router_id_update) (int, struct zclient *, uint16_t, vrf_id_t);
    int (*interface_add) (int, struct zclient *, uint16_t, vrf_id_t);
    int (*interface_delete) (int, struct zclient *, uint16_t, vrf_id_t);
    int (*interface_up) (int, struct zclient *, uint16_t, vrf_id_t);
    int (*interface_down) (int, struct zclient *, uint16_t, vrf_id_t);
    int (*interface_address_add) (int, struct zclient *, uint16_t, vrf_id_t);
    int (*interface_address_delete) (int, struct zclient *, uint16_t, vrf_id_t);
    int (*ipv4_route_add) (int, struct zclient *, uint16_t, vrf_id_t);
    int (*ipv4_route_delete) (int, struct zclient *, uint16_t, vrf_id_t);
    int (*ipv6_route_add) (int, struct zclient *, uint16_t, vrf_id_t);
    int (*ipv6_route_delete) (int, struct zclient *, uint16_t, vrf_id_t);
};

#endif /* HAVE_IPV6 */


/* Nexthop structure. */
struct nexthop
{
    struct nexthop *next;
    struct nexthop *prev;

    /* Interface index. */
    char *ifname;
    ifindex_t ifindex;

    enum nexthop_types_t type;

    u_char flags;
#define NEXTHOP_FLAG_ACTIVE     (1 << 0) /* This nexthop is alive. */
#define NEXTHOP_FLAG_FIB        (1 << 1) /* FIB nexthop. */
#define NEXTHOP_FLAG_RECURSIVE  (1 << 2) /* Recursive nexthop. */
#define NEXTHOP_FLAG_ONLINK     (1 << 3) /* Nexthop should be installed onlink. */

    /* Nexthop address */
    union g_addr gate;
    union g_addr src;

    /* Nexthops obtained by recursive resolution.
     *
     * If the nexthop struct needs to be resolved recursively,
     * NEXTHOP_FLAG_RECURSIVE will be set in flags and the nexthops
     * obtained by recursive resolution will be added to `resolved'.
     * Only one level of recursive resolution is currently supported. */
    struct nexthop *resolved;
};

/* route vrf cache fifo */
struct routefifo_vrf
{
    struct fifo fifo;
    enum IPC_OPCODE opcode;
    struct l3vpn_route l3vpn_route;
    
};

extern void bgp_redistribute_route_register(void);
extern void bgp_redistribute_route_unregister(void);
extern void bgp_redistribute_routev6_register(void);
extern void bgp_redistribute_routev6_unregister(void);

extern int bgp_send_route_timer(void *para);
extern int bgp_send_route_timer_vrf(void *para);
extern struct interface *bgp_interface_add_read (int , struct ifm_event *,vrf_id_t);
extern struct interface *bgp_interface_state_read (struct ifm_event *, vrf_id_t);
extern struct connected *bgp_interface_address_read (int type, struct ifm_event *, vrf_id_t);
extern void bgp_interface_if_set_value (struct ifm_event * ,struct interface *);
extern void bgp_zapi_route (enum IPC_OPCODE , struct prefix *, struct zapi_route *);
extern void bgp_zapi_ipv4_route_vrf (enum IPC_OPCODE , struct prefix_ipv4 *, struct zapi_route *, int);

extern int bgp_l3vpn_event_register(void);
extern int bgp_com_label_new(uint16_t );
extern int bgp_com_label_free(uint16_t , uint32_t );

extern int bgp_sendmsg_to_tcp(bgp_msg_type_t flag,struct peer *);
extern void bgp_route_msg_rcv_delete(void);
extern void bgp_common_msg_rcv_delete(void);
extern void bgp_ifm_register(void);
extern void bgp_route_msg_rcv(void *arg);
extern void bgp_common_msg_rcv(void *arg);
extern int bgp_msg_rcv(struct ipc_mesg_n *pmsg, int imlen);
extern void bgp_route_msg_rcv_init(void);
extern void bgp_common_msg_rcv_init(void);

void bgp_msg_send_noack(struct ipc_msghdr_n *pReqhdr, uint32_t errcode, unsigned int msg_index);



#endif /* _BGP_IPC_H */
