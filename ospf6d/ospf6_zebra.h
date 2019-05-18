/*
 * Copyright (C) 2003 Yasuhiro Ohara
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
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef OSPF6_ZEBRA_H
#define OSPF6_ZEBRA_H
#include "if.h"
#include "ifm_common.h"
#include "route_com.h"
#include "ospf6_route.h"



#define IFF_UP      0x0001      /* interface is up */
#define   IFF_BROADCAST   0x0002      /* broadcast address valid */
//#define   IFF_DEBUG   0x0004      /* turn on debugging */
#define IFF_LOOPBACK    0x0008      /* is a loopback net */

/* Debug option */
extern unsigned char conf_debug_ospf6_zebra;
#define OSPF6_DEBUG_ZEBRA_SEND 0x01
#define OSPF6_DEBUG_ZEBRA_RECV 0x02
#define OSPF6_DEBUG_ZEBRA_ON(level) \
  (conf_debug_ospf6_zebra |= level)
#define OSPF6_DEBUG_ZEBRA_OFF(level) \
  (conf_debug_ospf6_zebra &= ~(level))
#define IS_OSPF6_DEBUG_ZEBRA(e) \
  (conf_debug_ospf6_zebra & OSPF6_DEBUG_ZEBRA_ ## e)

#ifdef HAVE_IPV6
/* IPv6 prefix add and delete function prototype. */

struct ase_ipv6
{
    u_char type;

    u_char flags;

    u_char message;

    safi_t safi;

    u_char nexthop_num;
    struct in6_addr **nexthop;

    u_char ifindex_num;

    u_int32_t index;
    ifindex_t *ifindex;

    u_char distance;

    u_int8_t instance;

    u_int32_t metric;

    u_int32_t nhp_type;

    u_int32_t mtu;

    vrf_id_t vrf_id;
};
#endif

extern void ospf6_zebra_route_update_add(struct ospf6_route *, struct ospf6 *);
extern void ospf6_zebra_route_update_remove(struct ospf6_route *, struct ospf6 *);
extern void ospf6_zebra_route_update_update(struct ospf6_route *, struct ospf6 *);
extern struct interface *ospf6_interface_add_read(uint32_t , vrf_id_t);

extern void ospf6_zebra_redistribute(struct ospf6 *, int, int, int, int);
extern void ospf6_zebra_no_redistribute(struct ospf6 *, int, int);

extern int config_write_ospf6_debug_zebra(struct vty *vty);
//extern void install_element_ospf6_debug_zebra(void);

extern struct interface *ospf6_if_add(struct ifm_event , vrf_id_t);
extern int ospf6_zebra_if_del(struct ifm_event, vrf_id_t);
extern int ospf6_zebra_if_state_update(struct ifm_event, vrf_id_t);
extern int ospf6_zebra_if_address_update_add(struct ifm_event, vrf_id_t);
extern int ospf6_zebra_if_address_update_delete(struct ifm_event, vrf_id_t);
extern int ospf6_interface_mode_change(struct ifm_event , vrf_id_t);
extern int ospf6_zebra_read_ipv6(int , struct route_entry *, zebra_size_t , vrf_id_t);
extern void ospf6_add_ipv6_locallink_addr(struct ifm_event);
extern int ospf6_interface_mtu_change (struct ifm_event , vrf_id_t);
extern void ospf6_finish_route_table(struct ospf6 *);


#endif /*OSPF6_ZEBRA_H*/

