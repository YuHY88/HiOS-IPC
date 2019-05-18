/*
 * OSPF AS Boundary Router functions.
 * Copyright (C) 1999, 2000 Kunihiro Ishiguro, Toshiaki Takada
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

#ifndef _ZEBRA_OSPF_ASBR_H
#define _ZEBRA_OSPF_ASBR_H

#define OSPF_SUMMARY_TIMER_DELAY        1
#define OSPF_SUMMARY_ADVERTISE          (1 << 0)
#define OSPF_SUMMARY_SUBSTITUTE         (1 << 1)
#include "lib/ifm_common.h"
#include "lib/if.h"


struct route_map_set_values
{
    int32_t metric;
    int32_t metric_type;
};

struct ospf_route_redist
{
    uint32_t ifindex;
    char if_name[IFNET_NAMESIZE];
    
};



/* Redistributed external information. */
struct external_info
{
    /* For linked list. */
    struct external_info *next;
    struct external_info *prev;

    /* Type of source protocol. */
    int type;

    /* Prefix. */
    struct prefix_ipv4 p;

    /* Interface index. */
    ifindex_t ifindex;

    /* Nexthop address. */
    struct in_addr nexthop;

    /* Additional Route tag: this is the wire type */
    u_int32_t tag;

    struct route_map_set_values route_map_set;
#define ROUTEMAP_METRIC(E)      (E)->route_map_set.metric
#define ROUTEMAP_METRIC_TYPE(E) (E)->route_map_set.metric_type

    u_int8_t instance;

    u_int16_t flags;

};

/* Area summary. */
struct ospf_summary
{
    /* Area range address. */
    struct in_addr addr;

    /* Area range masklen. */
    u_char masklen;

    /* Flags. */
    u_char flags;
#define OSPF_SUMMARY_ADDRESS_ADVERTISE          (1 << 0)

    /* Number of more specific prefixes. */
    int specifics;

    /* Addr and masklen to substitute. */
    struct in_addr subst_addr;
    u_char subst_masklen;

    /* summary cost. */
    int cost;

    /* Configured range cost. */
    int cost_config;

    int type;
#define OSPF_SUMMARY_COST_UNSPEC    (-1)

};


#define OSPF_ASBR_CHECK_DELAY       30

extern struct ospf_route_redist*ospf_route_policy_look(struct ospf* , uint32_t );
extern struct ospf_route_redist *ospf_route_policy_get(struct ospf *, uint32_t , char *);
extern struct ospf_summary *ospf_summary_lookup(struct ospf* , struct prefix_ipv4 *);
extern void ospf_route_policy_free (struct ospf_route_redist *);
extern void ospf_route_policy_list_free(struct ospf* );

extern void ospf_reset_route_map_set_values (struct route_map_set_values *);
extern int ospf_route_map_set_compare (struct route_map_set_values *,
                                       struct route_map_set_values *);
extern struct external_info *ospf_external_info_add (uint16_t, u_char, u_int8_t,
        struct prefix_ipv4, ifindex_t, struct in_addr, route_tag_t);
extern void ospf_external_info_delete (uint16_t, u_char, u_int8_t, struct prefix_ipv4, struct in_addr);
extern struct external_info *ospf_external_info_lookup (uint16_t, u_char,u_int8_t,
        struct prefix_ipv4 *, struct in_addr nexthop);
extern struct external_info *ospf_external_info_lookup_by_prefix (struct ospf *,
        struct prefix_ipv4 *, struct in_addr nexthop);
extern void ospf_asbr_status_update (struct ospf *, u_char);

extern void ospf_redistribute_withdraw (struct ospf *, u_char, u_int8_t);
extern void ospf_schedule_asbr_task (struct ospf *);
extern struct ospf_lsa *ospf_external_info_find_lsa (struct ospf *,
        struct prefix_ipv4 *);
extern struct ospf_lsa *ospf_external_info_find_lsa_specific (struct ospf *,
        struct prefix_ipv4 *);
extern struct ospf_lsa *ospf_external_info_find_lsa_summary (struct ospf *,
        struct prefix_ipv4 *);
extern struct ospf_lsa *ospf_as_nssa_info_find_lsa (struct ospf *,
        struct prefix_ipv4 *);
extern struct ospf_summary *ospf_summary_match (struct ospf *, struct prefix_ipv4 *);
extern int ospf_summary_set (struct ospf *,struct prefix_ipv4 *, int );
extern int ospf_summary_unset (struct ospf *, struct prefix_ipv4 *);
extern int ospf_summary_cost_set (struct ospf *, struct prefix_ipv4 *, u_int32_t);

#endif /* _ZEBRA_OSPF_ASBR_H */


