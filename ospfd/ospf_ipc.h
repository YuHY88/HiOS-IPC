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

#ifndef _OSPF_IPC_H
#define _OSPF_IPC_H

/* For struct zapi_ipv{4,6}. */
#include "lib/prefix.h"

/* For struct interface and struct connected. */
#include "lib/if.h"

/* For vrf_bitmap_t. */
#include "lib/vrf.h"

#include "lib/table.h"
#include "lib/ifm_common.h"
#include "lib/msg_ipc_n.h"
#include "route/route.h"
#include "ospfd/ospfd.h"



#define IPC_OSPF_LEN     8192
/*interface state add by tai*/
#define IFF_UP      0x0001      /* interface is up */
#define   IFF_BROADCAST   0x0002      /* broadcast address valid */
//#define   IFF_DEBUG   0x0004      /* turn on debugging */
#define IFF_LOOPBACK    0x0008      /* is a loopback net */
//#define   IFF_POINTOPOINT 0x0010      /* interface is point-to-point link */
//#define   IFF_NOTRAILERS  0x0020      /* avoid use of trailers */
#define IFF_RUNNING 0x0040      /* resources allocated */
//#define   IFF_NOARP   0x0080      /* no address resolution protocol */
//#define   IFF_PROMISC 0x0100      /* receive all packets */
//#define   IFF_ALLMULTI    0x0200      /* receive all multicast packets */
//#define   IFF_OACTIVE 0x0400      /* transmission in progress */
//#define   IFF_SIMPLEX 0x0800      /* can't hear own transmissions */
//#define   IFF_LINK0   0x1000      /* per link layer defined bit */
//#define   IFF_LINK1   0x2000      /* per link layer defined bit */
//#define   IFF_LINK2   0x4000      /* per link layer defined bit */
//#define   IFF_MULTICAST   0x8000      /* supports multicast */



/* For input/output buffer to zebra. */
#define ZEBRA_MAX_PACKET_SIZ          4096

/* Zebra header size. */
//#define ZEBRA_HEADER_SIZE             8


/* Zebra API message flag. */
#define ZAPI_MESSAGE_NEXTHOP  0x01
#define ZAPI_MESSAGE_IFINDEX  0x02
#define ZAPI_MESSAGE_DISTANCE 0x04
#define ZAPI_MESSAGE_METRIC   0x08
#define ZAPI_MESSAGE_MTU      0x10


/* Zebra IPv4 route message API. */
struct ase_ipv4
{
    u_char type;

    u_char flags;

    u_char message;

    safi_t safi;

    u_char nexthop_num;
    struct in_addr **nexthop;

    u_char ifindex_num;
    ifindex_t *ifindex;

    u_char distance;

    route_tag_t tag;

    u_int8_t instance;

    u_int32_t index;

    u_int32_t metric;

    u_int32_t mtu;

    u_int32_t nhp_type;

    vrf_id_t vrf_id;
	uint16_t vpn;
};


/* Prototypes of zebra client service functions. */
extern void ospf_init (void);

extern int ospf_redistribute_send (int command, struct ospf *, int type,
                                   vrf_id_t vrf_id);

/* If state has changed, update state and call ospf_redistribute_send. */
extern void ospf_redistribute (int command, struct ospf *, int, int,
                               vrf_id_t);

/* If state has changed, update state and send the command to zebra. */
extern void ospf_redistribute_default (int command, struct ospf *,
                                       vrf_id_t vrf_id);

extern void ospf_router_id_update_read (struct stream *s, struct prefix *rid);
extern int zapi_ipv4_route (u_char, struct prefix_ipv4 *,
                            struct ase_ipv4 *);

//extern int ospf_msg_rcv(struct thread *);

extern int ospf_asyc_send_route_cmd (void *);

extern int get_dcn_ne_info_from_tlv(struct ospf_lsa *, struct ospf *, struct ospf_dcn_ne_info *);

extern void ospf_dcn_ne_info_trap(struct ospf_dcn_ne_info *ne_info, int noline_flag);

//int ospf_ifm_msg_rcv(struct ipc_mesg mesg);
//int ospf_route_msg_rcv(struct ipc_mesg mesg);
//int ospf_add_fifo_ifm(struct ipc_mesg *mesg);
//void ospf_read_mesg_from_ifmfifo(void);
//void ospf_common_manage(struct ipc_mesg mesg);
void ospf_common_manage_new(struct ipc_mesg_n *mesg);
void ospf_ifm_manage(struct ifm_event pevent, int event);
//int ospf_add_fifo_route(struct ipc_mesg *mesg);
//void ospf_read_mesg_from_routefifo(void);
//void ospf_route_manage(struct ipc_mesg mesg);
void ospf_route_manage_new(struct ipc_mesg_n *mesg);



#endif /* _PROTOCOL_IPC_H */


