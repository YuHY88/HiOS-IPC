/*
 * IS-IS Rout(e)ing protocol               - isis_route.h
 *
 * Copyright (C) 2001,2002   Sampo Saaristo
 *                           Tampere University of Technology      
 *                           Institute of Communications Engineering
 *
 *                                         based on ../ospf6d/ospf6_route.[ch]
 *                                         by Yasuhiro Ohara
 *
 * This program is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU General Public Licenseas published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
 * more details.

 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef _ZEBRA_ISIS_ROUTE_H
#define _ZEBRA_ISIS_ROUTE_H

#include "table.h"
#include "isisd.h"
#include "isis_circuit.h"

#define ISIS_ROUTE_FLAG_NEED_DELETE_OLD       0x02
#define ISIS_ROUTE_FLAG_NEED_ADD_NEW          0x01
#define ISIS_ROUTE_FLAG_NEED_ALL_CHANGE       0x04

#if 1
struct isis_nexthop6
{
  ifindex_t ifindex;
  struct in6_addr ip6;
  struct in6_addr router_address6;
  unsigned int lock;
};
#endif /* 0 */

struct isis_nexthop
{
  ifindex_t ifindex;
  struct in_addr ip;
  struct in_addr router_address;
  unsigned int lock;
};

struct merge_isis_nexthop
{
  ifindex_t ifindex;
  struct in_addr ip;
  struct in_addr router_address;
  u_int32_t cost;
  unsigned int lock;
};

struct merge_isis_nexthop6
{
  ifindex_t ifindex;
  struct in6_addr ip;
  struct in6_addr router_address;
  u_int32_t cost;
  unsigned int lock;
};


#define ISIS_ROUTE_FLAG_ACTIVE       	0x01  /* active route for the prefix */
#define ISIS_ROUTE_FLAG_ZEBRA_SYNCED 	0x02  /* set when route synced to zebra */
#define ISIS_ROUTE_FLAG_ZEBRA_RESYNC 	0x04  /* set when route needs to sync */
#define ISIS_ROUTE_FLAG_ROUTE_CHANGE 	0x08
#define ISIS_ROUTE_FLAG_DIRECT_CONNECT  0x10
#define ISIS_ROUTE_FLAG_MERGE 			0x20
#define ISIS_ROUTE_FLAG_DISTRIBUTE 		0x40  /* use for internal and external */
#define ISIS_ROUTE_FLAG_DOWN 			0x80  /* use for L1 import to L2 or L2 import to L1 in ipv6 tlv*/

struct isis_route_info
{
  u_char flag;
  u_int32_t cost;
  u_int32_t depth;
  struct list *nexthops;
  struct list *old_nexthops;
#if 1
  struct list *nexthops6;
  struct list *old_nexthops6;
#endif				/* 0 */
};

void isis_route_info_ipv6_same_merge(struct route_node *mrnode, struct isis_route_info *info , struct isis_route_info *ainfo);
void isis_route_info_same_merge(struct route_node *mrnode, struct isis_route_info *info , struct isis_route_info *ainfo);
void isis_route_info_nexthops_all_change(struct isis_route_info *info_new, struct isis_route_info *info_old, int family);
void isis_route_info_nexthops_del(struct isis_route_info *info_new, struct isis_route_info *info_old, int family);
void isis_route_info_nexthops_add(struct isis_route_info *info_new, struct isis_route_info *info_old, int family);
int isis_route_info_same_attrib (struct isis_route_info *info_new, struct isis_route_info *info_old,struct isis_area* area);

struct isis_route_info *
isis_route_create (struct prefix *prefix, u_int32_t cost, u_int32_t depth,
		struct list *adjacencies, struct isis_area *area, int level, int distribute ,uint8_t up_down_bit);

struct isis_route_info *
isis_ipv6_route_create (struct prefix *prefix, u_int32_t cost_1, u_int32_t depth,
		   struct list *adjacencies, struct isis_area *area,
		   int level, int distribute ,uint8_t up_down_bit);

struct isis_route_info *
isis_route_info_new (struct prefix *prefix, uint32_t cost, uint32_t depth,
					struct list *adjacencies,int level);

int isis_route_info_same (struct isis_route_info *info_new,struct isis_route_info *info_old, u_char family,struct isis_area * area);
void isis_route_validate (struct isis_area *area);
void isis_route_invalidate_table (struct isis_area *area,struct route_table *table);
void isis_route_validate_table (struct isis_area *area,struct route_table *table);

void isis_route_invalidate (struct isis_area *area);

void isis_route_invalidate_level (struct isis_area *area, int level);
void isis_route_validate_level(struct isis_area *area, int level);
void isis_route_validate_after_spf (struct isis_area *area, int family);
void isis_route_info_delete (struct isis_route_info *route_info);
void isis_route_create_default (struct isis_area *area,struct list *att, int family);
void isis_direct_route_add(struct isis_area *area, struct route_table *table, int level, int family);
int isis_adj_nh_match(struct in_addr *ipv4_addr, struct isis_circuit * circuit);
void isis_nexthop_delete (struct isis_nexthop *nexthop);
int nexthop6lookup (struct list *nexthops6, struct in6_addr *ip6,ifindex_t ifindex);
int mergenexthop6lookup (struct list *nexthops6, struct in6_addr *ip6,ifindex_t ifindex);
void isis_nexthop6_delete (struct isis_nexthop6 *nexthop6);

#endif /* _ZEBRA_ISIS_ROUTE_H */
