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

#ifndef OSPF6_INTERFACE_H
#define OSPF6_INTERFACE_H

#include "if.h"

/* Debug option */
extern unsigned char conf_debug_ospf6_interface;
#define OSPF6_DEBUG_INTERFACE_ON() \
  (conf_debug_ospf6_interface = 1)
#define OSPF6_DEBUG_INTERFACE_OFF() \
  (conf_debug_ospf6_interface = 0)
#define IS_OSPF6_DEBUG_INTERFACE \
  (conf_debug_ospf6_interface)

/* Interface structure */
struct ospf6_interface
{
    /* IF info from zebra */
    struct interface *interface;

    struct ospf6 *ospf6;

    /* back pointer */
    struct ospf6_area *area;

    /* list of ospf6 neighbor */
    struct list *neighbor_list;

    /* linklocal address of this I/F */
    struct in6_addr *linklocal_addr;

    /* Interface ID; use interface->ifindex */

    /* ospf6 instance id */
    u_char instance_id;

    /* I/F transmission delay */
    u_int32_t transdelay;

    /* Network Type */
    u_char type;

    /* Router Priority */
    u_char priority;

    /* Time Interval */
    u_int16_t hello_interval;
    u_int16_t dead_interval;
    u_int32_t rxmt_interval;

    u_int32_t state_change;

    /* Cost */
    u_int32_t cost;

    /* I/F MTU */
    u_int32_t ifmtu;

    /* Interface State */
    u_char state;

    /* OSPF6 Interface flag */
    char flag;

    /* MTU mismatch check */
    u_char mtu_ignore;

    /* interface mode */
    u_char mode;

    /* Decision of DR Election */
    u_int32_t drouter;
    u_int32_t bdrouter;
    u_int32_t prev_drouter;
    u_int32_t prev_bdrouter;

    /* Linklocal LSA Database: includes Link-LSA */
    struct ospf6_lsdb *lsdb;
    struct ospf6_lsdb *lsdb_self;

    struct ospf6_lsdb *lsupdate_list;
    struct ospf6_lsdb *lsack_list;

    /* Ongoing Tasks */
    //struct thread *thread_send_hello;
	TIMERID thread_send_hello;
	//struct thread *t_wait;
	TIMERID t_wait;
    struct thread *thread_send_lsupdate;	//not timer
    //struct thread *thread_send_lsack;
	TIMERID thread_send_lsack;

    struct thread *thread_network_lsa;		//not timer
    struct thread *thread_link_lsa;			//not timer
    struct thread *thread_intra_prefix_lsa;	//not timer

    struct ospf6_route_table *route_connected;

    /* prefix-list name to filter connected prefix */
    char *plist_name;
};

/* interface state */
#define OSPF6_INTERFACE_NONE             0
#define OSPF6_INTERFACE_DOWN             1
#define OSPF6_INTERFACE_LOOPBACK         2
#define OSPF6_INTERFACE_WAITING          3
#define OSPF6_INTERFACE_POINTTOPOINT     4
#define OSPF6_INTERFACE_DROTHER          5
#define OSPF6_INTERFACE_BDR              6
#define OSPF6_INTERFACE_DR               7
#define OSPF6_INTERFACE_MAX              8

extern const char *ospf6_interface_state_str[];

/* flags */
#define OSPF6_INTERFACE_DISABLE      0x01
#define OSPF6_INTERFACE_PASSIVE      0x02
#define OSPF6_INTERFACE_NOAUTOCOST   0x04

/* default values */
#define OSPF6_INTERFACE_HELLO_INTERVAL 10
#define OSPF6_INTERFACE_DEAD_INTERVAL  40
#define OSPF6_INTERFACE_RXMT_INTERVAL  5
#define OSPF6_INTERFACE_COST           1
#define OSPF6_INTERFACE_PRIORITY       1
#define OSPF6_INTERFACE_TRANSDELAY     1
#define OSPF6_INTERFACE_INSTANCE_ID    0
#define OSPF6_INTERFACE_BANDWIDTH      10000   /* Kbps */
#define OSPF6_REFERENCE_BANDWIDTH      100000  /* Kbps */

#define IFP_IF_IPV6_EXIST(ip)    (((ip[0])!=0) || ((ip[1])!=0) || ((ip[2])!=0) || ((ip[3])!=0) \
                                    || ((ip[4])!=0) || ((ip[5])!=0) || ((ip[6])!=0) || ((ip[7])!=0) \
                                    || ((ip[8])!=0) || ((ip[9])!=0) || ((ip[10])!=0) || ((ip[11])!=0) \
                                    || ((ip[12])!=0) || ((ip[13])!=0) || ((ip[14])!=0) || ((ip[15])!=0))


/* Function Prototypes */

extern struct ospf6_interface *ospf6_interface_lookup_by_ifindex(ifindex_t ifindex);
extern struct ospf6_interface *ospf6_interface_create(struct ospf6 *, struct interface *);
extern void ospf6_interface_delete(struct ospf6_interface *);

extern void ospf6_interface_enable(struct ospf6_interface *);
extern void ospf6_interface_disable(struct ospf6_interface *);

extern void ospf6_interface_if_add(struct interface *);
extern void ospf6_interface_if_del(struct interface *);
extern void ospf6_interface_state_update(struct interface *);
extern void ospf6_interface_connected_route_update(struct interface *);
extern int ospf6_interface_get_address_num(struct interface *);

/* interface event */
extern int interface_up(struct thread *);
extern int interface_down(struct thread *);
extern int wait_timer(void *);
extern int backup_seen(struct thread *);
extern int neighbor_change(struct thread *);
extern u_char ospf6_default_iftype(struct interface *);
extern int ospf6_get_intf_mode(ifindex_t );

extern void ospf6_interface_init(void);

extern int config_write_ospf6_debug_interface(struct vty *vty);
extern void install_element_ospf6_debug_interface(void);

#endif /* OSPF6_INTERFACE_H */

