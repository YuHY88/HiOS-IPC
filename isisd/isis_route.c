/*
 * IS-IS Rout(e)ing protocol               - isis_route.c
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

#include <zebra.h>

#include "thread.h"
#include "linklist.h"
#include "vty.h"
#include "log.h"
#include "memory.h"
#include "prefix.h"
#include "hash.h"
#include "if.h"
#include "table.h"

#include "isis_constants.h"
#include "isis_common.h"
#include "isis_flags.h"
#include "dict.h"
#include "isisd.h"
#include "isis_misc.h"
#include "isis_adjacency.h"
#include "isis_circuit.h"
#include "isis_tlv.h"
#include "isis_pdu.h"
#include "isis_lsp.h"
#include "isis_spf.h"
#include "isis_route.h"
#include "isis_zebra.h"
#include "isis_csm.h"

extern char *inet_ntoa(struct in_addr in);
extern int
ip_same_subnet(struct prefix_ipv4 *ip1, struct in_addr *ip2);

static struct isis_nexthop *
isis_nexthop_create(struct in_addr *ip, ifindex_t ifindex)
{
	struct listnode *node;
	struct isis_nexthop *nexthop;

	for (ALL_LIST_ELEMENTS_RO(isis->nexthops, node, nexthop))
	{
		if (nexthop->ifindex != ifindex)
			continue;
		if (ip && memcmp(&nexthop->ip, ip, sizeof(struct in_addr)) != 0)
			continue;

		nexthop->lock++;
		return nexthop;
	}

	nexthop = XCALLOC(MTYPE_ISIS_NEXTHOP, sizeof(struct isis_nexthop));

	memset(nexthop, 0, sizeof(struct isis_nexthop));

	nexthop->ifindex = ifindex;
	memcpy(&nexthop->ip, ip, sizeof(struct in_addr));
	listnode_add(isis->nexthops, nexthop);
	nexthop->lock++;

	return nexthop;
}

static struct isis_nexthop *
isis_nexthop_create_old(struct in_addr *ip, ifindex_t ifindex)
{
	struct isis_nexthop *nexthop;

	nexthop = XCALLOC(MTYPE_ISIS_NEXTHOP, sizeof(struct isis_nexthop));
	memset(nexthop, 0, sizeof(struct isis_nexthop));

	nexthop->ifindex = ifindex;
	memcpy(&nexthop->ip, ip, sizeof(struct in_addr));
	listnode_add(isis->nexthops, nexthop);
	nexthop->lock++;

	return nexthop;
}

static struct merge_isis_nexthop *
isis_nexthop_create_merge_old(struct in_addr *ip, ifindex_t ifindex)
{
	struct merge_isis_nexthop *nexthop;

	nexthop = XCALLOC(MTYPE_ISIS_NEXTHOP, sizeof(struct isis_nexthop));
	memset(nexthop, 0, sizeof(struct isis_nexthop));

	nexthop->ifindex = ifindex;
	memcpy(&nexthop->ip, ip, sizeof(struct in_addr));
	listnode_add(isis->nexthops, nexthop);
	nexthop->lock++;

	return nexthop;
}

void isis_nexthop_delete(struct isis_nexthop *nexthop)
{
	nexthop->lock--;
	if (nexthop->lock == 0)
	{
		listnode_delete(isis->nexthops, nexthop);
		XFREE(MTYPE_ISIS_NEXTHOP, nexthop);
	}

	return;
}

static void
isis_nexthop_merge_delete(struct merge_isis_nexthop *nexthop)
{
	nexthop->lock--;
	if (nexthop->lock == 0)
	{
		listnode_delete(isis->nexthops, nexthop);
		XFREE(MTYPE_ISIS_NEXTHOP, nexthop);
	}

	return;
}

static int
nexthoplookup(struct list *nexthops, struct in_addr *ip,
							ifindex_t ifindex)
{
	struct listnode *node;
	struct isis_nexthop *nh;

	for (ALL_LIST_ELEMENTS_RO(nexthops, node, nh))
	{
		if (!(memcmp(ip, &nh->ip, sizeof(struct in_addr))) && ifindex == nh->ifindex)
		{
			return 1;
		}
	}

	return 0;
}

static int
mergenexthoplookup(struct list *nexthops, struct in_addr *ip,
									 ifindex_t ifindex)
{
	struct listnode *node;
	struct merge_isis_nexthop *nh;

	for (ALL_LIST_ELEMENTS_RO(nexthops, node, nh))
	{
		if (!(memcmp(ip, &nh->ip, sizeof(struct in_addr))) &&
				ifindex == nh->ifindex)
			return 1;
	}

	return 0;
}

#ifdef EXTREME_DEBUG
static void
nexthop_print(struct isis_nexthop *nh)
{
	u_char buf[BUFSIZ];

	inet_ntop(AF_INET, &nh->ip, (char *)buf, BUFSIZ);

	zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "      %s %u", buf, nh->ifindex);
}

static void
nexthops_print(struct list *nhs)
{
	struct listnode *node;
	struct isis_nexthop *nh;

	for (ALL_LIST_ELEMENTS_RO(nhs, node, nh))
		nexthop_print(nh);
}
#endif /* EXTREME_DEBUG */

static struct merge_isis_nexthop6 *
isis_nexthop6_create_merge_old(struct in6_addr *ip6, ifindex_t ifindex)
{
	struct merge_isis_nexthop6 *nexthop6;

	nexthop6 = XCALLOC(MTYPE_ISIS_NEXTHOP6, sizeof(struct isis_nexthop6));
	memset(nexthop6, 0, sizeof(struct isis_nexthop6));

	nexthop6->ifindex = ifindex;
	memcpy(&nexthop6->ip, ip6, sizeof(struct in6_addr));
	listnode_add(isis->nexthops6, nexthop6);
	nexthop6->lock++;

	return nexthop6;
}

static struct isis_nexthop6 *
isis_nexthop6_create(struct in6_addr *ip6, ifindex_t ifindex)
{
	struct listnode *node;
	struct isis_nexthop6 *nexthop6;

	for (ALL_LIST_ELEMENTS_RO(isis->nexthops6, node, nexthop6))
	{
		if (nexthop6->ifindex != ifindex)
			continue;
		if (ip6 && memcmp(&nexthop6->ip6, ip6, sizeof(struct in6_addr)) != 0)
			continue;

		nexthop6->lock++;
		return nexthop6;
	}

	nexthop6 = XCALLOC(MTYPE_ISIS_NEXTHOP6, sizeof(struct isis_nexthop6));

	memset(nexthop6, 0, sizeof(struct isis_nexthop6));

	nexthop6->ifindex = ifindex;
	memcpy(&nexthop6->ip6, ip6, sizeof(struct in6_addr));
	listnode_add(isis->nexthops6, nexthop6);
	nexthop6->lock++;

	return nexthop6;
}

static struct isis_nexthop6 *
isis_nexthop6_create_old(struct in6_addr *ip6, ifindex_t ifindex)
{
	struct isis_nexthop6 *nexthop6;

	nexthop6 = XCALLOC(MTYPE_ISIS_NEXTHOP6, sizeof(struct isis_nexthop6));
	memset(nexthop6, 0, sizeof(struct isis_nexthop6));

	nexthop6->ifindex = ifindex;
	memcpy(&nexthop6->ip6, ip6, sizeof(struct in6_addr));
	listnode_add(isis->nexthops6, nexthop6);
	nexthop6->lock++;

	return nexthop6;
}

void isis_nexthop6_delete(struct isis_nexthop6 *nexthop6)
{

	nexthop6->lock--;
	if (nexthop6->lock == 0)
	{
		listnode_delete(isis->nexthops6, nexthop6);
		XFREE(MTYPE_ISIS_NEXTHOP6, nexthop6);
	}

	return;
}

static void
isis_nexthop6_merge_delete(struct merge_isis_nexthop6 *nexthop6)
{
	nexthop6->lock--;
	if (nexthop6->lock == 0)
	{
		listnode_delete(isis->nexthops6, nexthop6);
		XFREE(MTYPE_ISIS_NEXTHOP6, nexthop6);
	}

	return;
}

int nexthop6lookup(struct list *nexthops6, struct in6_addr *ip6,
									 ifindex_t ifindex)
{
	struct listnode *node;
	struct isis_nexthop6 *nh6;

	for (ALL_LIST_ELEMENTS_RO(nexthops6, node, nh6))
	{
		if (!(memcmp(ip6, &nh6->ip6, sizeof(struct in6_addr))) &&
				ifindex == nh6->ifindex)
			return 1;
	}

	return 0;
}

int mergenexthop6lookup(struct list *nexthops6, struct in6_addr *ip6,
												ifindex_t ifindex)
{
	struct listnode *node;
	struct merge_isis_nexthop6 *nh6;

	for (ALL_LIST_ELEMENTS_RO(nexthops6, node, nh6))
	{
		if (!(memcmp(ip6, &nh6->ip, sizeof(struct in6_addr))) &&
				ifindex == nh6->ifindex)
			return 1;
	}

	return 0;
}

#ifdef EXTREME_DEBUG
static void
nexthop6_print(struct isis_nexthop6 *nh6)
{
	u_char buf[BUFSIZ];

	inet_ntop(AF_INET6, &nh6->ip6, (char *)buf, BUFSIZ);

	zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "      %s %u", buf, nh6->ifindex);
}

static void
nexthops6_print(struct list *nhs6)
{
	struct listnode *node;
	struct isis_nexthop6 *nh6;

	for (ALL_LIST_ELEMENTS_RO(nhs6, node, nh6))
		nexthop6_print(nh6);
}
#endif /* EXTREME_DEBUG */

int isis_adj_nh_match(struct in_addr *ipv4_addr, struct isis_circuit *circuit)
{
	struct listnode *node;
	struct prefix_ipv4 *c_ip;
	struct prefix_ipv4 ip1_tmp;

	/*get the master ip */
	c_ip = (struct prefix_ipv4 *)isis_ip_slave_judge(circuit);
	/*if the master ip is existed*/
	if (c_ip != NULL)
	{
		ip1_tmp = *c_ip;
		ip1_tmp.prefix.s_addr = htonl(c_ip->prefix.s_addr);
		if (ip_same_subnet(&ip1_tmp, ipv4_addr))
			return 1;
		else
			return 0;
	}
	/*there is no master ip ,only slave ip*/
	else
	{
		for (ALL_LIST_ELEMENTS_RO(circuit->ip_addrs, node, c_ip))
		{
			ip1_tmp = *c_ip;
			ip1_tmp.prefix.s_addr = htonl(c_ip->prefix.s_addr);
			if (ip_same_subnet(&ip1_tmp, ipv4_addr))
				return 1;
		}
		return 0;
	}
}

static void
adjinfo2nexthop(struct list *nexthops, struct isis_adjacency *adj,
								struct isis_route_info *rinfo, int level, uint32_t cost)
{
	struct isis_nexthop *nh;
	struct listnode *node;
	struct in_addr *ipv4_addr;

	if (adj->ipv4_addrs == NULL)
		return;
	rinfo->cost = cost;

	for (ALL_LIST_ELEMENTS_RO(adj->ipv4_addrs, node, ipv4_addr))
	{
		if (!isis_adj_nh_match(ipv4_addr, adj->circuit))
			continue;

		if (!nexthoplookup(nexthops, ipv4_addr,
											 adj->circuit->interface->ifindex))
		{
			nh = isis_nexthop_create(ipv4_addr,
															 adj->circuit->interface->ifindex);
			nh->router_address = adj->router_address;
			listnode_add(nexthops, nh);
		}
	}
}

#if 1
static void
adjinfo2nexthop6(struct list *nexthops6, struct isis_adjacency *adj)
{
	struct listnode *node;
	struct in6_addr *ipv6_addr;
	struct isis_nexthop6 *nh6;

	if (!adj->ipv6_addrs)
		return;

	for (ALL_LIST_ELEMENTS_RO(adj->ipv6_addrs, node, ipv6_addr))
	{
		if (!nexthop6lookup(nexthops6, ipv6_addr,
												adj->circuit->interface->ifindex))
		{
			nh6 = isis_nexthop6_create(ipv6_addr,
																 adj->circuit->interface->ifindex);
			nh6->router_address6 = adj->router_address6;
			listnode_add(nexthops6, nh6);
		}
	}
}
#endif /* 0 */

struct isis_route_info *
isis_route_info_new(struct prefix *prefix, uint32_t cost, uint32_t depth,
										struct list *adjacencies, int level)
{
	struct isis_route_info *rinfo;
	struct isis_adjacency *adj;
	struct listnode *node;

	rinfo = XCALLOC(MTYPE_ISIS_ROUTE_INFO, sizeof(struct isis_route_info));

	if (!rinfo)
	{
		zlog_err("%-20s[%d] malloc error: ", __FUNCTION__, __LINE__);
		return NULL;
	}

	memset(rinfo, 0, sizeof(struct isis_route_info));

	if (prefix->family == AF_INET)
	{
		rinfo->old_nexthops = list_new();
		rinfo->nexthops = list_new();
		for (ALL_LIST_ELEMENTS_RO(adjacencies, node, adj))
		{
			/* check for force resync this route */
			if (CHECK_FLAG(adj->circuit->flags, ISIS_CIRCUIT_FLAPPED_AFTER_SPF))
				SET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_RESYNC);

			/* update neighbor router address */
			if (depth == 2 && prefix->prefixlen == 32)
				adj->router_address = prefix->u.prefix4;
			adjinfo2nexthop(rinfo->nexthops, adj, rinfo, level, cost);
		}
	}
#if 1
	if (prefix->family == AF_INET6)
	{
		rinfo->nexthops6 = list_new();
		rinfo->old_nexthops6 = list_new();
		for (ALL_LIST_ELEMENTS_RO(adjacencies, node, adj))
		{
			/* check for force resync this route */
			if (CHECK_FLAG(adj->circuit->flags, ISIS_CIRCUIT_FLAPPED_AFTER_SPF))
				SET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_RESYNC);
			/* update neighbor router address */
			if (depth == 2 && prefix->prefixlen == 128)
				adj->router_address6 = prefix->u.prefix6;
			adjinfo2nexthop6(rinfo->nexthops6, adj);
		}
	}
#endif /* 0 */
	rinfo->depth = depth;
	rinfo->cost = cost;

	return rinfo;
}

void isis_route_info_delete(struct isis_route_info *route_info)
{
	if (route_info->nexthops)
	{
		if (!CHECK_FLAG(route_info->flag, ISIS_ROUTE_FLAG_MERGE))
		{
			route_info->nexthops->del = (void (*)(void *))isis_nexthop_delete;
		}
		else
		{
			route_info->nexthops->del = (void (*)(void *))isis_nexthop_merge_delete;
		}

		list_delete(route_info->nexthops);
		route_info->nexthops = NULL;
	}

	if (route_info->old_nexthops)
	{
		if (!CHECK_FLAG(route_info->flag, ISIS_ROUTE_FLAG_MERGE))
		{
			route_info->old_nexthops->del = (void (*)(void *))isis_nexthop_delete;
		}
		else
		{
			route_info->old_nexthops->del = (void (*)(void *))isis_nexthop_merge_delete;
		}
		list_delete(route_info->old_nexthops);
		route_info->old_nexthops = NULL;
	}

#if 1
	if (route_info->nexthops6)
	{
		if (!CHECK_FLAG(route_info->flag, ISIS_ROUTE_FLAG_MERGE))
		{
			route_info->nexthops6->del = (void (*)(void *))isis_nexthop6_delete;
		}
		else
		{
			route_info->nexthops6->del = (void (*)(void *))isis_nexthop6_merge_delete;
		}

		list_delete(route_info->nexthops6);
		route_info->nexthops6 = NULL;
	}

	if (route_info->old_nexthops6)
	{
		if (!CHECK_FLAG(route_info->flag, ISIS_ROUTE_FLAG_MERGE))
		{
			route_info->old_nexthops6->del = (void (*)(void *))isis_nexthop6_delete;
		}
		else
		{
			route_info->old_nexthops6->del = (void (*)(void *))isis_nexthop6_merge_delete;
		}
		list_delete(route_info->old_nexthops6);
		route_info->old_nexthops6 = NULL;
	}
#endif /* 0 */

	XFREE(MTYPE_ISIS_ROUTE_INFO, route_info);
}

int isis_route_info_same_attrib(struct isis_route_info *info_new,
																struct isis_route_info *info_old, struct isis_area *area)
{
	if (info_new->cost != info_old->cost && info_new->depth != 1)
	{
		return 0;
	}

	/* internal route replace the external one */
	if (CHECK_FLAG(info_new->flag, CTRL_INFO_DISTRIBUTION) == 0 &&
			CHECK_FLAG(info_old->flag, CTRL_INFO_DISTRIBUTION) == 1)
	{
		return 0;
	}

	/* just like , It's from L2 at first, but it's from L1 now, update the route info */
	if (CHECK_FLAG(info_new->flag, CTRL_INFO_DIRECTION) != CHECK_FLAG(info_old->flag, CTRL_INFO_DIRECTION))
	{
		return 0;
	}

	return 1;
}

int isis_route_info_same(struct isis_route_info *info_new,
												 struct isis_route_info *info_old, u_char family, struct isis_area *area)
{
	int rcflag = 0;
	struct listnode *node;
	struct isis_nexthop *nexthop;
	struct isis_nexthop6 *nexthop6;

	if (CHECK_FLAG(info_new->flag, ISIS_ROUTE_FLAG_ZEBRA_RESYNC))
	{
		SET_FLAG(rcflag, ISIS_ROUTE_FLAG_NEED_ALL_CHANGE);
		return rcflag;
	}

	if (!isis_route_info_same_attrib(info_new, info_old, area))
	{
		SET_FLAG(rcflag, ISIS_ROUTE_FLAG_NEED_ALL_CHANGE);
		return rcflag;
	}

	if (family == AF_INET)
	{
		for (ALL_LIST_ELEMENTS_RO(info_new->nexthops, node, nexthop))
		{
			if (nexthoplookup(info_old->nexthops, &nexthop->ip, nexthop->ifindex) == 0)
			{
				SET_FLAG(rcflag, ISIS_ROUTE_FLAG_NEED_ADD_NEW);
			}
		}

		for (ALL_LIST_ELEMENTS_RO(info_old->nexthops, node, nexthop))
		{
			if (nexthoplookup(info_new->nexthops, &nexthop->ip, nexthop->ifindex) == 0)
			{
				SET_FLAG(rcflag, ISIS_ROUTE_FLAG_NEED_DELETE_OLD);
			}
		}
	}

	if (family == AF_INET6)
	{
		for (ALL_LIST_ELEMENTS_RO(info_new->nexthops6, node, nexthop6))
		{
			if (nexthop6lookup(info_old->nexthops6, &nexthop6->ip6, nexthop6->ifindex) == 0)
			{
				SET_FLAG(rcflag, ISIS_ROUTE_FLAG_NEED_ADD_NEW);
			}
		}

		for (ALL_LIST_ELEMENTS_RO(info_old->nexthops6, node, nexthop6))
		{
			if (nexthop6lookup(info_new->nexthops6, &nexthop6->ip6, nexthop6->ifindex) == 0)
			{
				SET_FLAG(rcflag, ISIS_ROUTE_FLAG_NEED_DELETE_OLD);
			}
		}
	}

	return rcflag;
}

#if 0
static int
isis_route_info_ecmp_check (struct isis_route_info *info_new, struct isis_route_info *info_old,int family)
{
  struct listnode *node;
  struct isis_nexthop *nexthop;
#if 1
  struct isis_nexthop6 *nexthop6;
#endif /* 0 */

  if(!CHECK_FLAG(info_old->flag, ISIS_ROUTE_FLAG_ACTIVE))  
  	return 0;

  if(info_old->cost != info_new->cost)
  	return 0;

  if(family == AF_INET)
  {
    for (ALL_LIST_ELEMENTS_RO (info_new->nexthops, node, nexthop))
    {
      if (nexthoplookup (info_old->nexthops, &nexthop->ip, nexthop->ifindex)
              == 0)
      {
       return 1;
      }
    }
  }

#if 1
  if (family == AF_INET6)
  {
	for (ALL_LIST_ELEMENTS_RO (info_new->nexthops6, node, nexthop6))
	{
	  if (nexthop6lookup (info_old->nexthops6, &nexthop6->ip6,
	      nexthop6->ifindex) == 0)
		return 0;
	}

  }
#endif /* 0 */

  return 0;
  
}
#endif

void isis_route_info_nexthops_add(struct isis_route_info *info_new, struct isis_route_info *info_old, int family)
{
	struct listnode *node;
	struct isis_nexthop *nexthop, *nexthop_new;
	struct isis_nexthop6 *nexthop6, *nexthop6_new;

	if (family == AF_INET)
	{
		for (ALL_LIST_ELEMENTS_RO(info_new->nexthops, node, nexthop))
		{
			if (nexthoplookup(info_old->nexthops, &nexthop->ip, nexthop->ifindex) == 0)
			{
				nexthop_new = XCALLOC(MTYPE_ISIS_NEXTHOP, sizeof(struct isis_nexthop));
				memset(nexthop_new, 0, sizeof(struct isis_nexthop));

				nexthop_new->ifindex = nexthop->ifindex;
				memcpy(&nexthop_new->ip, &nexthop->ip, sizeof(struct in_addr));
				listnode_add(isis->nexthops, nexthop_new);
				listnode_add(info_old->nexthops, nexthop_new);
				nexthop_new->lock++;
			}
		}
	}

	if (family == AF_INET6)
	{
		for (ALL_LIST_ELEMENTS_RO(info_new->nexthops6, node, nexthop6))
		{
			if (nexthop6lookup(info_old->nexthops6, &nexthop6->ip6, nexthop6->ifindex) == 0)
			{
				nexthop6_new = XCALLOC(MTYPE_ISIS_NEXTHOP6, sizeof(struct isis_nexthop6));

				nexthop6_new->ifindex = nexthop6->ifindex;
				memcpy(&nexthop6_new->ip6, &nexthop6->ip6, sizeof(struct in6_addr));
				listnode_add(isis->nexthops6, nexthop6_new);
				listnode_add(info_old->nexthops6, nexthop6_new);
				nexthop6_new->lock++;
			}
		}
	}
}

void isis_route_info_nexthops_del(struct isis_route_info *info_new, struct isis_route_info *info_old, int family)
{
	struct listnode *node, *node1;
	struct isis_nexthop *nexthop, *nexthop_new;
	struct isis_nexthop6 *nexthop6, *nexthop6_new;

	if (family == AF_INET)
	{
		for (ALL_LIST_ELEMENTS(info_old->nexthops, node, node1, nexthop))
		{
			if (nexthoplookup(info_new->nexthops, &nexthop->ip, nexthop->ifindex) == 0)
			{
				if (nexthoplookup(info_old->old_nexthops, &nexthop->ip, nexthop->ifindex))
					continue;
				nexthop_new = XCALLOC(MTYPE_ISIS_NEXTHOP, sizeof(struct isis_nexthop));
				memset(nexthop_new, 0, sizeof(struct isis_nexthop));

				nexthop_new->ifindex = nexthop->ifindex;
				memcpy(&nexthop_new->ip, &nexthop->ip, sizeof(struct in_addr));
				nexthop_new->lock++;
				listnode_add(info_old->old_nexthops, nexthop_new);

				//memory leak add by zy
				listnode_add(info_new->old_nexthops, nexthop);

				list_delete_node(info_old->nexthops, node);
				//listnode_delete(info_old->nexthops, nexthop);
				//XFREE(MTYPE_ISIS_NEXTHOP,nexthop);
			}
		}
	}

	if (family == AF_INET6)
	{
		for (ALL_LIST_ELEMENTS(info_old->nexthops6, node, node1, nexthop6))
		{
			if (nexthop6lookup(info_new->nexthops6, &nexthop6->ip6, nexthop6->ifindex) == 0)
			{
				if (nexthop6lookup(info_old->old_nexthops6, &nexthop6->ip6, nexthop6->ifindex))
					continue;

				nexthop6_new = XCALLOC(MTYPE_ISIS_NEXTHOP6, sizeof(struct isis_nexthop6));
				memset(nexthop6_new, 0, sizeof(struct isis_nexthop6));

				nexthop6_new->ifindex = nexthop6->ifindex;
				memcpy(&nexthop6_new->ip6, &nexthop6->ip6, sizeof(struct in6_addr));
				nexthop6_new->lock++;
				listnode_add(info_old->old_nexthops6, nexthop6_new);
				listnode_add(info_new->old_nexthops6, nexthop6);
				list_delete_node(info_old->nexthops6, node);
			}
		}
	}
}
void isis_route_info_nexthops_all_change(struct isis_route_info *info_new, struct isis_route_info *info_old, int family)
{
	struct listnode *node, *node1;
	struct isis_nexthop *nexthop, *nh;
	struct isis_nexthop6 *nexthop6, *nh6;

	if (family == AF_INET)
	{
		for (ALL_LIST_ELEMENTS(info_old->nexthops, node, node1, nexthop))
		{
			//if(nexthoplookup(info_new->nexthops, &nexthop->ip, nexthop->ifindex))
			//continue;
			nh = XCALLOC(MTYPE_ISIS_NEXTHOP, sizeof(struct isis_nexthop));
			memset(nh, 0, sizeof(struct isis_nexthop));

			nh->ifindex = nexthop->ifindex;
			memcpy(&nh->ip, &nexthop->ip, sizeof(struct in_addr));
			nh->lock++;

			listnode_add(info_new->old_nexthops, nh);
		}
		isis_route_info_delete(info_old);
	}

	if (family == AF_INET6)
	{
		for (ALL_LIST_ELEMENTS(info_old->nexthops6, node, node1, nexthop6))
		{
			//if(nexthoplookup(info_new->nexthops6, &nexthop6->ip6, nexthop6->ifindex))
			//continue;

			nh6 = XCALLOC(MTYPE_ISIS_NEXTHOP6, sizeof(struct isis_nexthop6));
			memset(nh6, 0, sizeof(struct isis_nexthop6));
			nh6->ifindex = nexthop6->ifindex;
			memcpy(&nh6->ip6, &nexthop6->ip6, sizeof(struct in6_addr));
			nh6->lock++;
			listnode_add(info_new->old_nexthops6, nh6);
		}
		isis_route_info_delete(info_old);
	}
}

static int
isis_default_route_info_same(struct isis_route_info *info_new,
														 struct isis_route_info *info_old, u_char family)
{
	struct listnode *node;
	struct isis_nexthop *nexthop;
	struct isis_nexthop6 *nexthop6;

	if (info_new->cost != info_old->cost)
		return 0;

	if (family == AF_INET)
	{
		if (listcount(info_old->nexthops) == 0 || listcount(info_new->nexthops) == 0)
			return 0;
	}

	if (family == AF_INET6)
	{
		if (listcount(info_old->nexthops6) == 0 || listcount(info_new->nexthops6) == 0)
			return 0;
	}

	if (family == AF_INET)
	{
		for (ALL_LIST_ELEMENTS_RO(info_new->nexthops, node, nexthop))
		{
			if (nexthoplookup(info_old->nexthops, &nexthop->ip, nexthop->ifindex) == 0)
				return 0;
		}

		for (ALL_LIST_ELEMENTS_RO(info_old->nexthops, node, nexthop))
		{

			if (nexthoplookup(info_new->nexthops, &nexthop->ip, nexthop->ifindex) == 0)
				return 0;
		}
	}

	if (family == AF_INET6)
	{
		for (ALL_LIST_ELEMENTS_RO(info_new->nexthops6, node, nexthop6))
		{
			if (nexthop6lookup(info_old->nexthops6, &nexthop6->ip6, nexthop6->ifindex) == 0)
				return 0;
		}

		for (ALL_LIST_ELEMENTS_RO(info_old->nexthops6, node, nexthop6))
		{

			if (nexthop6lookup(info_new->nexthops6, &nexthop6->ip6, nexthop6->ifindex) == 0)
				return 0;
		}
	}
	return 1;
}

void isis_route_create_default(struct isis_area *area, struct list *att, int family)
{
	struct route_node *route_node, *dnode;
	struct listnode *anode, *node;
	struct prefix *p;
	struct prefix pdefault;
	struct isis_route_info *rinfo_new, *rinfo_old, *route_info = NULL;
	struct isis_nexthop *nexthop, *nh_default;
	struct isis_nexthop6 *nexthop6, *nh_default6;
	u_int32_t cost = 0xffffffff;

	if (listcount(att) <= 0 || area->is_type == IS_LEVEL_1_AND_2 || area->is_type == IS_LEVEL_2)
	{
		list_free(att);
		att = NULL;
		return;
	}

	if (family == AF_INET)
	{
		pdefault.family = AF_INET;
		pdefault.prefixlen = 0;
		pdefault.u.prefix4.s_addr = INADDR_ANY;

		dnode = route_node_get(area->route_table[ISIS_LEVEL1 - 1], &pdefault);
		rinfo_old = dnode->info;

		route_info = XCALLOC(MTYPE_ISIS_ROUTE_INFO, sizeof(struct isis_route_info));
		route_info->old_nexthops = list_new();
		route_info->nexthops = list_new();

		for (ALL_LIST_ELEMENTS_RO(att, anode, p))
		{
			route_node = route_node_lookup(area->route_table[ISIS_LEVEL1 - 1], p);

			if (route_node == NULL)
				continue;

			rinfo_new = route_node->info;

			if (rinfo_new->nexthops == NULL && !CHECK_FLAG(rinfo_new->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT))
				continue;

			if (rinfo_new->cost < cost)
				cost = rinfo_new->cost;
		}

		for (ALL_LIST_ELEMENTS_RO(att, anode, p))
		{
			route_node = route_node_lookup(area->route_table[ISIS_LEVEL1 - 1], p);

			if (route_node == NULL)
				continue;

			rinfo_new = route_node->info;

			if (rinfo_new->nexthops == NULL && !CHECK_FLAG(rinfo_new->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT))
				continue;

			if (rinfo_new->cost != cost)
				continue;

			for (ALL_LIST_ELEMENTS_RO(rinfo_new->nexthops, node, nexthop))
			{
				if (nexthoplookup(route_info->nexthops, &nexthop->ip, nexthop->ifindex))
					continue;
				nh_default = isis_nexthop_create_old(&nexthop->ip, nexthop->ifindex);
				listnode_add(route_info->nexthops, nh_default);
			}

			route_info->cost = cost;
			route_info->depth = rinfo_new->depth;
		}

		if (!rinfo_old)
		{
			UNSET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
		}
		else
		{
			if (!isis_default_route_info_same(route_info, rinfo_old, AF_INET))
			{
				for (ALL_LIST_ELEMENTS_RO(rinfo_old->nexthops, node, nexthop))
				{
					if (nexthoplookup(route_info->old_nexthops, &nexthop->ip, nexthop->ifindex))
						continue;
					nh_default = isis_nexthop_create_old(&nexthop->ip, nexthop->ifindex);
					listnode_add(route_info->old_nexthops, nh_default);
				}
				isis_route_info_delete(rinfo_old);
				//list_add_list (route_info->old_nexthops, rinfo_old->nexthops);
				UNSET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
			}
		}

		SET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ACTIVE);
		SET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);
		dnode->info = route_info;
	}

	if (family == AF_INET6)
	{
		pdefault.family = AF_INET6;
		pdefault.prefixlen = 0;
		memset(pdefault.u.prefix6.s6_addr, 0, 16);
		dnode = route_node_get(area->route_table6[ISIS_LEVEL1 - 1], &pdefault);
		rinfo_old = dnode->info;

		route_info = XCALLOC(MTYPE_ISIS_ROUTE_INFO, sizeof(struct isis_route_info));
		route_info->old_nexthops6 = list_new();
		route_info->nexthops6 = list_new();

		for (ALL_LIST_ELEMENTS_RO(att, anode, p))
		{
			route_node = route_node_lookup(area->route_table6[ISIS_LEVEL1 - 1], p);

			if (route_node == NULL)
				continue;

			rinfo_new = route_node->info;

			if (rinfo_new->nexthops6 == NULL && !CHECK_FLAG(rinfo_new->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT))
				continue;

			if (rinfo_new->cost < cost)
				cost = rinfo_new->cost;
		}

		for (ALL_LIST_ELEMENTS_RO(att, anode, p))
		{
			route_node = route_node_lookup(area->route_table6[ISIS_LEVEL1 - 1], p);

			if (route_node == NULL)
				continue;

			rinfo_new = route_node->info;

			if (rinfo_new->nexthops6 == NULL && !CHECK_FLAG(rinfo_new->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT))
				continue;

			if (rinfo_new->cost != cost)
				continue;

			for (ALL_LIST_ELEMENTS_RO(rinfo_new->nexthops6, node, nexthop6))
			{
				if (nexthop6lookup(route_info->nexthops6, &nexthop6->ip6, nexthop6->ifindex))
					continue;
				nh_default6 = isis_nexthop6_create_old(&nexthop6->ip6, nexthop6->ifindex);
				listnode_add(route_info->nexthops6, nh_default6);
			}

			route_info->cost = cost;
			route_info->depth = rinfo_new->depth;
		}

		if (!rinfo_old)
		{
			UNSET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
		}
		else
		{
			if (!isis_default_route_info_same(route_info, rinfo_old, AF_INET6))
			{
				for (ALL_LIST_ELEMENTS_RO(rinfo_old->nexthops6, node, nexthop6))
				{
					if (nexthop6lookup(route_info->old_nexthops6, &nexthop6->ip6, nexthop6->ifindex))
						continue;
					nh_default6 = isis_nexthop6_create_old(&nexthop6->ip6, nexthop6->ifindex);
					listnode_add(route_info->old_nexthops6, nh_default6);
				}
				isis_route_info_delete(rinfo_old);
				//list_add_list (route_info->old_nexthops, rinfo_old->nexthops);
				UNSET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
			}
		}

		SET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ACTIVE);
		SET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);
		dnode->info = route_info;
	}

	list_delete_all_node(att);
	list_free(att);
}

struct isis_route_info *
isis_ipv6_route_create(struct prefix *prefix, u_int32_t cost_1, u_int32_t depth,
											 struct list *adjacencies, struct isis_area *area, int level, int distribute, uint8_t up_down_bit)
{
	u_char family;
	u_int32_t cost;
	u_char buff[BUFSIZ];
	struct route_node *route_node = NULL;
	struct isis_route_info *rinfo_new = NULL;
	struct isis_route_info *rinfo_old = NULL;
	struct isis_route_info *route_info = NULL;

	cost = cost_1;
	family = prefix->family;

	/* for debugs */
	prefix2str(prefix, (char *)buff, BUFSIZ);

	route_node = route_node_get(area->route_table6[level - 1], prefix);
	if (route_node == NULL)
	{
		return NULL;
	}

	rinfo_old = route_node->info;

	if (rinfo_old &&
			CHECK_FLAG(rinfo_old->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT) &&
			CHECK_FLAG(rinfo_old->flag, ISIS_ROUTE_FLAG_ACTIVE))
	{
		return NULL;
	}

	if (rinfo_old && rinfo_old->cost < cost && CHECK_FLAG(rinfo_old->flag, ISIS_ROUTE_FLAG_ACTIVE))
		return NULL;

	rinfo_new = isis_route_info_new(prefix, cost, depth, adjacencies, level);

	if (up_down_bit == 128)
	{
		SET_FLAG(rinfo_new->flag, ISIS_ROUTE_FLAG_DOWN);
	}

	if (distribute == DISTRIBUTION_EXTERNAL)
	{
		SET_FLAG(rinfo_new->flag, DISTRIBUTION_EXTERNAL);
	}

	if (!rinfo_old)
	{
		if (level == 1)
		{
			area->force_l2_lsp_refresh = 1;
		}
		zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "ISIS-Rte (%s) route created: %s", area->area_tag, buff);
		route_info = rinfo_new;
		UNSET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
	}
	else
	{
		zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "ISIS-Rte (%s) route already exists: %s", area->area_tag, buff);
		int rcflag = 0;
		rcflag = isis_route_info_same(rinfo_new, rinfo_old, family, area);

		if (rcflag == 0)
		{
			zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "ISIS-Rte (%s) route unchanged: %s", area->area_tag, buff);
			isis_route_info_delete(rinfo_new);
			route_info = rinfo_old;
		}

		else if (CHECK_FLAG(rcflag, ISIS_ROUTE_FLAG_NEED_ALL_CHANGE))
		{
			if (level == 1)
				area->force_l2_lsp_refresh = 1;

			zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "ISIS-Rte (%s) route changed: %s", area->area_tag, buff);
			/*
		when a route's cost decrease then before,the route before will be cover,
		i must send to route to delete the route first,other route wll refuse the new one;
		*/
			isis_zebra_route_del_ipv6(&route_node->p, area, rinfo_old);

			isis_route_info_nexthops_all_change(rinfo_new, rinfo_old, family);

			route_info = rinfo_new;
			UNSET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
		}

		else
		{
			if (level == 1)
				area->force_l2_lsp_refresh = 1;
			if (CHECK_FLAG(rcflag, ISIS_ROUTE_FLAG_NEED_DELETE_OLD))
			{
				if (!CHECK_FLAG(rinfo_old->flag, ISIS_ROUTE_FLAG_ACTIVE))
				{
					isis_route_info_nexthops_del(rinfo_new, rinfo_old, family);
				}
			}

			if (CHECK_FLAG(rcflag, ISIS_ROUTE_FLAG_NEED_ADD_NEW))
			{
				isis_route_info_nexthops_add(rinfo_new, rinfo_old, family);
			}
			isis_route_info_delete(rinfo_new);
			route_info = rinfo_old;
			UNSET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
		}
	}

	SET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);
	SET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ACTIVE);
	route_node->info = route_info;

	return route_info;
}

struct isis_route_info *
isis_route_create(struct prefix *prefix, u_int32_t cost_1, u_int32_t depth,
									struct list *adjacencies, struct isis_area *area,
									int level, int distribute, uint8_t up_down_bit)
{
	u_char buff[BUFSIZ];
	u_char family;
	u_int32_t cost;
	struct route_node *route_node = NULL;
	struct isis_route_info *rinfo_new = NULL;
	struct isis_route_info *rinfo_old = NULL;
	struct isis_route_info *route_info = NULL;

	cost = cost_1;
	family = prefix->family;

	/* for debugs */
	prefix2str(prefix, (char *)buff, BUFSIZ);

	route_node = route_node_get(area->route_table[level - 1], prefix);

	if (route_node == NULL)
	{
		return NULL;
	}

	rinfo_old = route_node->info;

	if (rinfo_old &&
			CHECK_FLAG(rinfo_old->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT) &&
			CHECK_FLAG(rinfo_old->flag, ISIS_ROUTE_FLAG_ACTIVE))
	{
		return NULL;
	}

	if (rinfo_old && rinfo_old->cost < cost && CHECK_FLAG(rinfo_old->flag, ISIS_ROUTE_FLAG_ACTIVE))
	{
		return NULL;
	}

	rinfo_new = isis_route_info_new(prefix, cost, depth, adjacencies, level);

	/* the prefix is distributed*/
	if (distribute == DISTRIBUTION_EXTERNAL)
	{
		SET_FLAG(rinfo_new->flag, DISTRIBUTION_EXTERNAL);
	}

	/* the prefix is from L2*/
	if (up_down_bit == DIRECTION_DOWN)
	{
		SET_FLAG(rinfo_new->flag, DIRECTION_DOWN);
	}

	if (!rinfo_old)
	{
		if (level == 1)
		{
			area->force_l2_lsp_refresh = 1;
		}
		zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "ISIS-Rte (%s) route created: %s", area->area_tag, buff);
		route_info = rinfo_new;
		UNSET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
	}
	else
	{
		zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "ISIS-Rte (%s) route already exists: %s", area->area_tag, buff);
		int rcflag = 0;
		rcflag = isis_route_info_same(rinfo_new, rinfo_old, family, area);

		if (rcflag == 0)
		{
			zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "ISIS-Rte (%s) route unchanged: %s", area->area_tag, buff);
			isis_route_info_delete(rinfo_new);
			route_info = rinfo_old;
		}

		else if (CHECK_FLAG(rcflag, ISIS_ROUTE_FLAG_NEED_ALL_CHANGE))
		{
			if (level == 1)
				area->force_l2_lsp_refresh = 1;

			zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "ISIS-Rte (%s) route changed: %s", area->area_tag, buff);
			/*
		when a route's cost decrease then before,the route before will be cover,
		i must send to route to delete the route first,other route wll refuse the new one;
		*/
			isis_zebra_route_del_ipv4(&route_node->p, area, rinfo_old);

			isis_route_info_nexthops_all_change(rinfo_new, rinfo_old, family);

			route_info = rinfo_new;
			UNSET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
		}

		else
		{
			if (level == 1)
			{
				area->force_l2_lsp_refresh = 1;
			}
			if (CHECK_FLAG(rcflag, ISIS_ROUTE_FLAG_NEED_DELETE_OLD))
			{
				if (!CHECK_FLAG(rinfo_old->flag, ISIS_ROUTE_FLAG_ACTIVE))
				{
					isis_route_info_nexthops_del(rinfo_new, rinfo_old, family);
				}
			}

			if (CHECK_FLAG(rcflag, ISIS_ROUTE_FLAG_NEED_ADD_NEW))
			{
				isis_route_info_nexthops_add(rinfo_new, rinfo_old, family);
			}
			isis_route_info_delete(rinfo_new);
			route_info = rinfo_old;
			UNSET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
		}
	}

	SET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);
	SET_FLAG(route_info->flag, ISIS_ROUTE_FLAG_ACTIVE);
	route_node->info = route_info;

	return route_info;
}

static void
isis_route_delete(struct prefix *prefix, struct route_table *table, struct isis_area *area)
{
	struct route_node *rode;
	struct isis_route_info *rinfo;
	char buff[BUFSIZ];

	/* for log */
	prefix2str(prefix, buff, BUFSIZ);

	rode = route_node_lookup(table, prefix);
	if (rode == NULL)
		return;
	rinfo = rode->info;

	if (rinfo == NULL)
	{
		zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "ISIS-Rte: tried to delete non-existant route %s", buff);
		return;
	}

	if (CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED))
	{
		UNSET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ACTIVE);
		zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "ISIS-Rte: route delete  %s", buff);
		isis_zebra_route_update(prefix, area, rinfo);
	}
	isis_route_info_delete(rinfo);
	rode->info = NULL;

	return;
}

/* Validating routes in particular table. */
void isis_route_validate_table(struct isis_area *area, struct route_table *table)
{
	struct route_node *rnode, *drnode;
	struct isis_route_info *rinfo, *dinfo;
	u_char buff[BUFSIZ];

	if (table == NULL)
		return;

	for (rnode = route_top(table); rnode; rnode = route_next(rnode))
	{
		if (rnode->info == NULL)
			continue;
		rinfo = rnode->info;

		if (!CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_MERGE))
		{
			if (rnode->p.family == AF_INET)
			{
				if ((rinfo->nexthops == NULL))
					continue;

				if (listcount(rinfo->nexthops) == 0)
				{
					continue;
				}
			}

			if (rnode->p.family == AF_INET6)
			{
				if ((rinfo->nexthops6 == NULL))
					continue;

				if (listcount(rinfo->nexthops6) == 0)
				{
					continue;
				}
			}

			// if (isis->debugs & DEBUG_RTE_EVENTS)

			prefix2str(&rnode->p, (char *)buff, BUFSIZ);
			zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "ISIS-Rte (%s): route validate: %s %s %s %s",
								 area->area_tag,
								 (CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED) ? "synced" : "not-synced"),
								 (CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_RESYNC) ? "resync" : "not-resync"),
								 (CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ACTIVE) ? "active" : "inactive"), buff);

			isis_zebra_route_update(&rnode->p, area, rinfo);

			if (!CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
			{
				/* Area is either L1 or L2 => we use level route tables directly for
	        * validating => no problems with deleting routes. */
				if (area->is_type != IS_LEVEL_1_AND_2)
				{
					isis_route_delete(&rnode->p, table, area);
					continue;
				}

				if (rnode->p.family == AF_INET)
				{
					drnode = route_node_lookup(area->route_table[0], &rnode->p);
					if (drnode != NULL)
					{
						if (drnode->info == rnode->info)
							drnode->info = NULL;
						isis_route_delete(&rnode->p, area->route_table[0], area);
					}
					drnode = route_node_lookup(area->route_table[1], &rnode->p);
					if (drnode != NULL)
					{
						if (drnode->info == rnode->info)
							drnode->info = NULL;
						isis_route_delete(&rnode->p, area->route_table[1], area);
					}
				}
#if 1
				if (rnode->p.family == AF_INET6)
				{
					drnode = route_node_lookup(area->route_table6[0], &rnode->p);
					if (drnode != NULL)
					{
						if (drnode->info == rnode->info)
							drnode->info = NULL;
						isis_route_delete(&rnode->p, area->route_table6[0], area);
					}
					drnode = route_node_lookup(area->route_table6[1], &rnode->p);
					if (drnode != NULL)
					{
						if (drnode->info == rnode->info)
							drnode->info = NULL;
						isis_route_delete(&rnode->p, area->route_table6[1], area);
					}
				}
#endif
				isis_route_delete(&rnode->p, table, area);
			}
		}
		else
		{
			isis_zebra_route_update(&rnode->p, area, rinfo);
			if (rnode->p.family == AF_INET)
			{
				drnode = route_node_lookup(area->route_table[0], &rnode->p);
				if (drnode != NULL && drnode->info != NULL)
				{
					dinfo = drnode->info;
					if (!CHECK_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
					{
						isis_route_info_delete(dinfo);
						drnode->info = NULL;
					}
					else
					{
						isis_route_info_delete_old(dinfo);
						//list_delete_all_node(dinfo->old_nexthops);
						SET_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
						UNSET_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_RESYNC);
						UNSET_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);
					}
				}

				drnode = route_node_lookup(area->route_table[1], &rnode->p);
				if (drnode != NULL && drnode->info != NULL)
				{
					dinfo = drnode->info;
					if (!CHECK_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
					{
						isis_route_info_delete(dinfo);
						drnode->info = NULL;
					}
					else
					{
						//list_delete_all_node(dinfo->old_nexthops);
						isis_route_info_delete_old(dinfo);
						SET_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
						UNSET_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_RESYNC);
						UNSET_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);
					}
				}
			}

			if (rnode->p.family == AF_INET6)
			{
				drnode = route_node_lookup(area->route_table6[0], &rnode->p);
				if (drnode != NULL && drnode->info != NULL)
				{
					dinfo = drnode->info;
					if (!CHECK_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
					{
						isis_route_info_delete(dinfo);
						drnode->info = NULL;
					}
					else
					{
						isis_route_info_delete_ipv6_old(dinfo);
						SET_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
						UNSET_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_RESYNC);
						UNSET_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);
					}
				}

				drnode = route_node_lookup(area->route_table6[1], &rnode->p);
				if (drnode != NULL && drnode->info != NULL)
				{
					dinfo = drnode->info;
					if (!CHECK_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
					{
						isis_route_info_delete(dinfo);
						drnode->info = NULL;
					}
					else
					{
						isis_route_info_delete_ipv6_old(dinfo);
						SET_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
						UNSET_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_RESYNC);
						UNSET_FLAG(dinfo->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);
					}
				}
			}

			isis_route_info_delete(rinfo);
			rnode->info = NULL;
		}
	}
}

/* Function to validate route tables for L1L2 areas. In this case we can't use
 * level route tables directly, we have to merge them at first. L1 routes are
 * preferred over the L2 ones.
 *
 * Merge algorithm is trivial (at least for now). All L1 paths are copied into
 * merge table at first, then L2 paths are added if L1 path for same prefix
 * doesn't already exists there.
 *
 * FIXME: Is it right place to do it at all? Maybe we should push both levels
 * to the RIB with different zebra route types and let RIB handle this? */
#if 1
void isis_route_info_same_merge(struct route_node *mrnode, struct isis_route_info *info,
																struct isis_route_info *ainfo)
{
	struct isis_route_info *minfo;
	struct listnode *node;
	struct merge_isis_nexthop *nexthop, *nh;

	minfo = XCALLOC(MTYPE_ISIS_ROUTE_INFO, sizeof(struct isis_route_info));
	minfo->old_nexthops = list_new();
	minfo->nexthops = list_new();

	if (CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ACTIVE) && CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
	{
		if (info->cost == ainfo->cost)
		{
			for (ALL_LIST_ELEMENTS_RO(info->nexthops, node, nexthop))
			{
				nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
				nh->cost = info->cost;
				listnode_add(minfo->nexthops, nh);
			}

			for (ALL_LIST_ELEMENTS_RO(ainfo->nexthops, node, nexthop))
			{
				if (mergenexthoplookup(minfo->nexthops, &nexthop->ip, nexthop->ifindex))
				{
					/*
			  	     here set the l2 route_info's flag , 
			  	     otherwise it will always be 0 
					 it will make the flag2 always be 1.
			  	  */
					SET_FLAG(info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
					continue;
				}

				nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
				nh->cost = info->cost;
				listnode_add(minfo->nexthops, nh);
			}
			//minfo->cost = info->cost;
			minfo->depth = info->depth;
		}
		else if (info->cost > ainfo->cost)
		{
			for (ALL_LIST_ELEMENTS_RO(ainfo->nexthops, node, nexthop))
			{
				nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
				nh->cost = ainfo->cost;

				listnode_add(minfo->nexthops, nh);
			}

			for (ALL_LIST_ELEMENTS_RO(info->old_nexthops, node, nexthop))
			{
				nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
				nh->cost = info->cost;

				listnode_add(minfo->old_nexthops, nh);
			}

			//minfo->cost = ainfo->cost;
			minfo->depth = ainfo->depth;
		}
		else
		{
			for (ALL_LIST_ELEMENTS_RO(info->nexthops, node, nexthop))
			{
				nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
				nh->cost = info->cost;

				listnode_add(minfo->nexthops, nh);
			}
			//minfo->cost = info->cost;

			for (ALL_LIST_ELEMENTS_RO(ainfo->old_nexthops, node, nexthop))
			{
				if (mergenexthoplookup(minfo->old_nexthops, &nexthop->ip, nexthop->ifindex))
					continue;

				nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
				nh->cost = ainfo->cost;

				listnode_add(minfo->old_nexthops, nh);
			}

			minfo->depth = info->depth;
		}

		if (CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE))
		{
			for (ALL_LIST_ELEMENTS_RO(info->old_nexthops, node, nexthop))
			{
				nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
				nh->cost = info->cost;

				listnode_add(minfo->old_nexthops, nh);
			}
		}

		if (CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE))
		{
			for (ALL_LIST_ELEMENTS_RO(ainfo->old_nexthops, node, nexthop))
			{
				if (mergenexthoplookup(minfo->old_nexthops, &nexthop->ip, nexthop->ifindex))
					continue;

				nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
				nh->cost = ainfo->cost;

				listnode_add(minfo->old_nexthops, nh);
			}
		}
	}
	else if ((CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ACTIVE) && !CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ACTIVE)) || (!CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ACTIVE) && CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ACTIVE)))
	{
		if (CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ACTIVE))
		{
			for (ALL_LIST_ELEMENTS_RO(info->nexthops, node, nexthop))
			{
				if (mergenexthoplookup(minfo->nexthops, &nexthop->ip, nexthop->ifindex))
					continue;
				nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
				nh->cost = info->cost;
				listnode_add(minfo->nexthops, nh);
			}

			if (CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE))
			{
				for (ALL_LIST_ELEMENTS_RO(info->old_nexthops, node, nexthop))
				{
					if (mergenexthoplookup(minfo->old_nexthops, &nexthop->ip, nexthop->ifindex))
						continue;
					nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
					nh->cost = info->cost;
					listnode_add(minfo->old_nexthops, nh);
				}
			}
			//minfo->cost = info->cost;
			minfo->depth = info->depth;
		}

		if (CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
		{
			for (ALL_LIST_ELEMENTS_RO(ainfo->nexthops, node, nexthop))
			{
				if (mergenexthoplookup(minfo->nexthops, &nexthop->ip, nexthop->ifindex))
					continue;
				nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
				nh->cost = ainfo->cost;
				listnode_add(minfo->nexthops, nh);
			}

			if (CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE))
			{
				for (ALL_LIST_ELEMENTS_RO(ainfo->old_nexthops, node, nexthop))
				{
					if (mergenexthoplookup(minfo->old_nexthops, &nexthop->ip, nexthop->ifindex))
						continue;
					nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
					nh->cost = ainfo->cost;
					listnode_add(minfo->old_nexthops, nh);
				}
			}
			//minfo->cost = info->cost;
			minfo->depth = info->depth;
		}

		if (!CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
		{
			for (ALL_LIST_ELEMENTS_RO(ainfo->nexthops, node, nexthop))
			{
				if (mergenexthoplookup(minfo->old_nexthops, &nexthop->ip, nexthop->ifindex))
					continue;

				//if(nexthoplookup (minfo->nexthops, &nexthop->ip, nexthop->ifindex)
				//&& minfo->cost == ainfo->cost)
				//continue;
				nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
				nh->cost = ainfo->cost;
				listnode_add(minfo->old_nexthops, nh);
			}
		}

		if (!CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ACTIVE))
		{
			for (ALL_LIST_ELEMENTS_RO(info->nexthops, node, nexthop))
			{
				if (mergenexthoplookup(minfo->old_nexthops, &nexthop->ip, nexthop->ifindex))
					continue;

				//if(nexthoplookup (minfo->nexthops, &nexthop->ip, nexthop->ifindex)
				//&& minfo->cost == ainfo->cost)
				//continue;

				nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
				nh->cost = info->cost;
				listnode_add(minfo->old_nexthops, nh);
			}
		}
	}

	if (!CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ACTIVE) && !CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
	{
		for (ALL_LIST_ELEMENTS_RO(info->nexthops, node, nexthop))
		{
			if (mergenexthoplookup(minfo->old_nexthops, &nexthop->ip, nexthop->ifindex))
				continue;

			nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
			nh->cost = info->cost;
			listnode_add(minfo->old_nexthops, nh);
		}

		for (ALL_LIST_ELEMENTS_RO(ainfo->nexthops, node, nexthop))
		{
			if (mergenexthoplookup(minfo->old_nexthops, &nexthop->ip, nexthop->ifindex))
				continue;

			nh = isis_nexthop_create_merge_old(&nexthop->ip, nexthop->ifindex);
			nh->cost = ainfo->cost;
			listnode_add(minfo->old_nexthops, nh);
		}

		//minfo->cost = info->cost;
		minfo->depth = info->depth;
	}
	minfo->flag = 0;

	SET_FLAG(minfo->flag, ISIS_ROUTE_FLAG_MERGE);
	mrnode->info = minfo;
}
#endif

void isis_route_info_ipv6_same_merge(struct route_node *mrnode, struct isis_route_info *info,
																		 struct isis_route_info *ainfo)
{
	struct isis_route_info *minfo;
	struct listnode *node;
	struct merge_isis_nexthop6 *nexthop6, *nh6;

	minfo = XCALLOC(MTYPE_ISIS_ROUTE_INFO, sizeof(struct isis_route_info));
	minfo->old_nexthops6 = list_new();
	minfo->nexthops6 = list_new();

	if (CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ACTIVE) && CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
	{
		if (info->cost == ainfo->cost)
		{
			for (ALL_LIST_ELEMENTS_RO(info->nexthops6, node, nexthop6))
			{
				nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
				nh6->cost = info->cost;
				listnode_add(minfo->nexthops6, nh6);
			}

			for (ALL_LIST_ELEMENTS_RO(ainfo->nexthops6, node, nexthop6))
			{
				if (mergenexthop6lookup(minfo->nexthops6, &nexthop6->ip, nexthop6->ifindex))
				{
					/*
						 here set the l2 route_info's flag , 
						 otherwise it will always be 0 
						 it will make the flag2 always be 1.
					  */
					SET_FLAG(info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
					continue;
				}

				nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
				nh6->cost = info->cost;
				listnode_add(minfo->nexthops6, nh6);
			}
			//minfo->cost = info->cost;
			minfo->depth = info->depth;
		}
		else if (info->cost > ainfo->cost)
		{
			for (ALL_LIST_ELEMENTS_RO(ainfo->nexthops6, node, nexthop6))
			{
				nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
				nh6->cost = ainfo->cost;

				listnode_add(minfo->nexthops6, nh6);
			}

			for (ALL_LIST_ELEMENTS_RO(info->old_nexthops6, node, nexthop6))
			{
				nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
				nh6->cost = info->cost;

				listnode_add(minfo->old_nexthops6, nh6);
			}

			//minfo->cost = ainfo->cost;
			minfo->depth = ainfo->depth;
		}
		else
		{
			for (ALL_LIST_ELEMENTS_RO(info->nexthops6, node, nexthop6))
			{
				nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
				nh6->cost = info->cost;

				listnode_add(minfo->nexthops6, nh6);
			}
			//minfo->cost = info->cost;

			for (ALL_LIST_ELEMENTS_RO(ainfo->old_nexthops6, node, nexthop6))
			{
				if (mergenexthop6lookup(minfo->old_nexthops6, &nexthop6->ip, nexthop6->ifindex))
					continue;

				nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
				nh6->cost = ainfo->cost;

				listnode_add(minfo->old_nexthops6, nh6);
			}

			minfo->depth = info->depth;
		}

		if (CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE))
		{
			for (ALL_LIST_ELEMENTS_RO(info->old_nexthops6, node, nexthop6))
			{
				nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
				nh6->cost = info->cost;

				listnode_add(minfo->old_nexthops6, nh6);
			}
		}

		if (CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE))
		{
			for (ALL_LIST_ELEMENTS_RO(ainfo->old_nexthops6, node, nexthop6))
			{
				if (mergenexthop6lookup(minfo->old_nexthops6, &nexthop6->ip, nexthop6->ifindex))
					continue;

				nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
				nh6->cost = ainfo->cost;

				listnode_add(minfo->old_nexthops6, nh6);
			}
		}
	}
	else if ((CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ACTIVE) && !CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ACTIVE)) || (!CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ACTIVE) && CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ACTIVE)))
	{
		if (CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ACTIVE))
		{
			for (ALL_LIST_ELEMENTS_RO(info->nexthops6, node, nexthop6))
			{
				if (mergenexthop6lookup(minfo->nexthops6, &nexthop6->ip, nexthop6->ifindex))
					continue;
				nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
				nh6->cost = info->cost;
				listnode_add(minfo->nexthops6, nh6);
			}

			if (CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE))
			{
				for (ALL_LIST_ELEMENTS_RO(info->old_nexthops6, node, nexthop6))
				{
					if (mergenexthop6lookup(minfo->old_nexthops6, &nexthop6->ip, nexthop6->ifindex))
						continue;
					nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
					nh6->cost = info->cost;
					listnode_add(minfo->old_nexthops6, nh6);
				}
			}
			//minfo->cost = info->cost;
			minfo->depth = info->depth;
		}

		if (CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
		{
			for (ALL_LIST_ELEMENTS_RO(ainfo->nexthops6, node, nexthop6))
			{
				if (mergenexthop6lookup(minfo->nexthops6, &nexthop6->ip, nexthop6->ifindex))
					continue;
				nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
				nh6->cost = ainfo->cost;
				listnode_add(minfo->nexthops6, nh6);
			}

			if (CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE))
			{
				for (ALL_LIST_ELEMENTS_RO(ainfo->old_nexthops6, node, nexthop6))
				{
					if (mergenexthop6lookup(minfo->old_nexthops6, &nexthop6->ip, nexthop6->ifindex))
						continue;
					nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
					nh6->cost = ainfo->cost;
					listnode_add(minfo->old_nexthops6, nh6);
				}
			}
			//minfo->cost = info->cost;
			minfo->depth = info->depth;
		}

		if (!CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
		{
			for (ALL_LIST_ELEMENTS_RO(ainfo->nexthops6, node, nexthop6))
			{
				if (mergenexthop6lookup(minfo->old_nexthops6, &nexthop6->ip, nexthop6->ifindex))
					continue;

				//if(nexthoplookup (minfo->nexthops, &nexthop->ip, nexthop->ifindex)
				//&& minfo->cost == ainfo->cost)
				//continue;
				nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
				nh6->cost = ainfo->cost;
				listnode_add(minfo->old_nexthops6, nh6);
			}
		}

		if (!CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ACTIVE))
		{
			for (ALL_LIST_ELEMENTS_RO(info->nexthops6, node, nexthop6))
			{
				if (mergenexthop6lookup(minfo->old_nexthops6, &nexthop6->ip, nexthop6->ifindex))
					continue;

				//if(nexthoplookup (minfo->nexthops, &nexthop->ip, nexthop->ifindex)
				//&& minfo->cost == ainfo->cost)
				//continue;

				nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
				nh6->cost = info->cost;
				listnode_add(minfo->old_nexthops6, nh6);
			}
		}
	}

	if (!CHECK_FLAG(info->flag, ISIS_ROUTE_FLAG_ACTIVE) && !CHECK_FLAG(ainfo->flag, ISIS_ROUTE_FLAG_ACTIVE))
	{
		for (ALL_LIST_ELEMENTS_RO(info->nexthops6, node, nexthop6))
		{
			if (mergenexthop6lookup(minfo->old_nexthops6, &nexthop6->ip, nexthop6->ifindex))
				continue;

			nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
			nh6->cost = info->cost;
			listnode_add(minfo->old_nexthops6, nh6);
		}

		for (ALL_LIST_ELEMENTS_RO(ainfo->nexthops6, node, nexthop6))
		{
			if (mergenexthop6lookup(minfo->old_nexthops6, &nexthop6->ip, nexthop6->ifindex))
				continue;

			nh6 = isis_nexthop6_create_merge_old(&nexthop6->ip, nexthop6->ifindex);
			nh6->cost = ainfo->cost;
			listnode_add(minfo->old_nexthops6, nh6);
		}

		//minfo->cost = info->cost;
		minfo->depth = info->depth;
	}
	minfo->flag = 0;

	SET_FLAG(minfo->flag, ISIS_ROUTE_FLAG_MERGE);
	mrnode->info = minfo;
}

#if 1
static void
isis_route_validate_merge(struct isis_area *area, int family)
{
	struct route_table *table = NULL;
	struct route_table *merge = NULL;
	struct route_node *rnode, *mrnode;
	struct isis_route_info *rinfo, *minfo;
	int flag1 = 0;
	int flag2 = 0;

	merge = route_table_init();

	if (family == AF_INET)
		table = area->route_table[0];
#if 1
	else if (family == AF_INET6)
		table = area->route_table6[0];
#endif
	if (table != NULL)
	{
		for (rnode = route_top(table); rnode; rnode = route_next(rnode))
		{
			if (rnode->info == NULL)
				continue;

			rinfo = rnode->info;

			if (flag1 == 0 && !CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT))
			{
				if (!CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ACTIVE) || (CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ACTIVE) && !CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED)))
				{
					flag1 = 1;
				}
			}

			mrnode = route_node_get(merge, &rnode->p);
			mrnode->info = rnode->info;
		}
	}

	if (family == AF_INET)
		table = area->route_table[1];
#if 1
	else if (family == AF_INET6)
		table = area->route_table6[1];
#endif
	if (table != NULL)
	{
		for (rnode = route_top(table); rnode; rnode = route_next(rnode))
		{
			if (rnode->info == NULL)
				continue;
			/*l2*/
			rinfo = rnode->info;

			/*l1*/
			//mrnode = route_node_get (merge, &rnode->p);

			mrnode = route_node_lookup(merge, &rnode->p);
			if (mrnode == NULL)
			{
				mrnode = route_node_get(merge, &rnode->p);
			}

			if (mrnode->info != NULL)
			{
				minfo = mrnode->info;
				if (family == AF_INET)
				{
					isis_route_info_same_merge(mrnode, rinfo, minfo);
				}

				if (family == AF_INET6)
				{
					isis_route_info_ipv6_same_merge(mrnode, rinfo, minfo);
				}

				/* the ISIS_ROUTE_FLAG_ZEBRA_SYNCED falg will be set when merge */
				if (flag2 == 0 && !CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT))
				{
					if (!CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ACTIVE) || (CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ACTIVE) && !CHECK_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED)))
					{
						flag2 = 1;
					}
				}

				/*select route to upadte*/
				continue;
			}
			mrnode->info = rinfo;
		}
	}

	isis_route_validate_table(area, merge);
	route_table_finish(merge);
	merge = NULL;

	if (flag1 > 0 && area->is_type == IS_LEVEL_1_AND_2 && (area->area_import_to_l1 || area->area_import_to_l1_v6))
		lsp_regenerate_schedule(area, IS_LEVEL_1, 0);

	if (flag2 > 0 && area->is_type == IS_LEVEL_1_AND_2 && (area->area_import_to_l2 || area->area_import_to_l2_v6))
		lsp_regenerate_schedule(area, IS_LEVEL_2, 0);
}
#endif
/* Walk through route tables and propagate necessary changes into RIB. In case
 * of L1L2 area, level tables have to be merged at first. */
void isis_route_validate(struct isis_area *area)
{
	struct listnode *node;
	struct isis_circuit *circuit;

	if (area->is_type == IS_LEVEL_1)
		isis_route_validate_table(area, area->route_table[0]);
	else if (area->is_type == IS_LEVEL_2)
		isis_route_validate_table(area, area->route_table[1]);
	else
		isis_route_validate_merge(area, AF_INET);

#if 1
	if (area->is_type == IS_LEVEL_1)
		isis_route_validate_table(area, area->route_table6[0]);
	else if (area->is_type == IS_LEVEL_2)
		isis_route_validate_table(area, area->route_table6[1]);
	else
		isis_route_validate_merge(area, AF_INET6);
#endif

	/* walk all circuits and reset any spf specific flags */
	for (ALL_LIST_ELEMENTS_RO(area->circuit_list, node, circuit))
		UNSET_FLAG(circuit->flags, ISIS_CIRCUIT_FLAPPED_AFTER_SPF);

	return;
}

void isis_route_validate_after_spf(struct isis_area *area, int family)
{
	struct listnode *node;
	struct isis_circuit *circuit;

	if (family == AF_INET)
	{
		if (area->is_type == IS_LEVEL_1)
			isis_route_validate_table(area, area->route_table[0]);
		else if (area->is_type == IS_LEVEL_2)
			isis_route_validate_table(area, area->route_table[1]);
		else
			isis_route_validate_merge(area, AF_INET);
	}

	if (family == AF_INET6)
	{
		if (area->is_type == IS_LEVEL_1)
			isis_route_validate_table(area, area->route_table6[0]);
		else if (area->is_type == IS_LEVEL_2)
			isis_route_validate_table(area, area->route_table6[1]);
		else
			isis_route_validate_merge(area, AF_INET6);
	}

	/* walk all circuits and reset any spf specific flags */
	for (ALL_LIST_ELEMENTS_RO(area->circuit_list, node, circuit))
		UNSET_FLAG(circuit->flags, ISIS_CIRCUIT_FLAPPED_AFTER_SPF);

	return;
}

void isis_route_validate_level(struct isis_area *area, int level)
{
	if (level == IS_LEVEL_1)
		isis_route_validate_table(area, area->route_table[0]);
	else if (level == IS_LEVEL_2)
		isis_route_validate_table(area, area->route_table[1]);

	return;
}

void isis_route_invalidate_table(struct isis_area *area, struct route_table *table)
{
	struct route_node *rode;
	struct isis_route_info *rinfo;

	if (table == NULL)
		return;

	for (rode = route_top(table); rode; rode = route_next(rode))
	{
		if (rode->info == NULL)
			continue;
		rinfo = rode->info;

		UNSET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ACTIVE);
	}
}

void isis_route_invalidate(struct isis_area *area)
{
	if (area->is_type & IS_LEVEL_1)
	{
		isis_route_invalidate_table(area, area->route_table[0]);
		isis_route_invalidate_table(area, area->route_table6[0]);
	}
	if (area->is_type & IS_LEVEL_2)
	{
		isis_route_invalidate_table(area, area->route_table[1]);
		isis_route_invalidate_table(area, area->route_table6[1]);
	}
}

void isis_route_invalidate_level(struct isis_area *area, int level)
{
	if (level == IS_LEVEL_1)
		isis_route_invalidate_table(area, area->route_table[0]);
	if (level == IS_LEVEL_2)
		isis_route_invalidate_table(area, area->route_table[1]);
}

static int
isis_direct_route_info_same(struct isis_route_info *info_new,
														struct isis_route_info *info_old, u_char family)
{
	struct listnode *node;
	struct isis_nexthop *nexthop;
	struct isis_nexthop6 *nexthop6;

	if (CHECK_FLAG(info_new->flag, ISIS_ROUTE_FLAG_ZEBRA_RESYNC))
	{
		return 0;
	}

	if (info_new->cost != info_old->cost)
		return 0;

	if (family == AF_INET)
	{
		for (ALL_LIST_ELEMENTS_RO(info_new->nexthops, node, nexthop))
			if (nexthoplookup(info_old->nexthops, &nexthop->ip, nexthop->ifindex) == 0)
			{
				return 0;
			}

		for (ALL_LIST_ELEMENTS_RO(info_old->nexthops, node, nexthop))
			if (nexthoplookup(info_new->nexthops, &nexthop->ip, nexthop->ifindex) == 0)
			{
				return 0;
			}
	}

	if (family == AF_INET6)
	{
		for (ALL_LIST_ELEMENTS_RO(info_new->nexthops6, node, nexthop6))
			if (nexthop6lookup(info_old->nexthops6, &nexthop6->ip6, nexthop6->ifindex) == 0)
			{
				return 0;
			}

		for (ALL_LIST_ELEMENTS_RO(info_old->nexthops6, node, nexthop6))
			if (nexthop6lookup(info_new->nexthops6, &nexthop6->ip6, nexthop6->ifindex) == 0)
			{
				return 0;
			}
	}

	return 1;
}

void isis_direct_route_add(struct isis_area *area, struct route_table *table, int level, int family)
{
	struct isis_circuit *circuit;
	struct listnode *cnode, *ipnode, *cnnode;
	struct prefix_ipv4 *ipv4;
	struct prefix *p6;
	struct prefix_ipv6 *ipv6;
	struct in_addr *aip;
	struct in6_addr *ip6;
	struct prefix prefix, prefix_tmp;
	struct route_node *route_node;
	struct isis_route_info *rinfo, *rinfo_old, *rinfo_new = NULL;
	struct isis_nexthop *nexthop, *nh = NULL;
	struct isis_nexthop6 *nexthop6, *nh6 = NULL;

	struct listnode *node, *nnode;
	struct isis_adjacency *adj;
	struct list *adjdb;

	u_char lsp_id[ISIS_SYS_ID_LEN + 2];
	struct isis_lsp *lsp;

	int flag = 0;

	for (ALL_LIST_ELEMENTS_RO(area->circuit_list, cnode, circuit))
	{
		if (!(circuit->is_type & level))
			continue;

		if (circuit->area != area)
			continue;

		if (family == AF_INET && circuit->ip_router)
		{
			prefix.family = AF_INET;
			prefix_tmp.family = AF_INET;
			for (ALL_LIST_ELEMENTS_RO(circuit->ip_addrs, ipnode, ipv4))
			{
				prefix_tmp.u.prefix4.s_addr = htonl(ipv4->prefix.s_addr);
				prefix_tmp.prefixlen = ipv4->prefixlen;
				prefix.u.prefix4.s_addr = htonl(ipv4->prefix.s_addr);
				prefix.prefixlen = ipv4->prefixlen;
				apply_mask(&prefix_tmp);

				route_node = route_node_get(table, &prefix_tmp);

				rinfo_old = route_node->info;
				if (circuit->state != C_STATE_UP || circuit->area != area)
				{
					if (rinfo_old)
					{
						SET_FLAG(rinfo_old->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT);
					}
				}
				else
				{
					rinfo_new = XCALLOC(MTYPE_ISIS_ROUTE_INFO, sizeof(struct isis_route_info));
					rinfo_new->old_nexthops = list_new();
					rinfo_new->nexthops = list_new();

					rinfo_new->depth = 1;

					if (area->oldmetric)
					{
						rinfo_new->cost = circuit->metrics[level - 1].metric_default;
					}
					else
					{
						rinfo_new->cost = circuit->te_metric[level - 1];
					}

					if (circuit->circ_type == CIRCUIT_T_BROADCAST)
					{
						adjdb = circuit->u.bc.adjdb[level - 1];
						if (adjdb && adjdb->count)
						{
							for (ALL_LIST_ELEMENTS_RO(adjdb, node, adj))
							{
								if (adj)
								{

									if (adj->cost_out_of_range == 1)
										continue;

									memcpy(lsp_id, adj->sysid, ISIS_SYS_ID_LEN);
									LSP_FRAGMENT(lsp_id) = 0;
									LSP_PSEUDO_ID(lsp_id) = 0;
									lsp = lsp_search(lsp_id, area->lspdb[level - 1]);

									if (lsp == NULL)
									{
										zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "%20s[%d] no lsp found", __FUNCTION__, __LINE__);
										continue;
									}

									if (circuit->area->spftree[level - 1] == NULL)
									{
										zlog_debug(ISIS_DEBUG_TYPE_ROUTE, "%20s[%d] no spftree found", __FUNCTION__, __LINE__);
										continue;
									}

									if (!lsp_type_check(circuit->area->spftree[level - 1], lsp, AF_INET))
									{
										continue;
									}
									else
									{
										for (ALL_LIST_ELEMENTS_RO(adj->ipv4_addrs, nnode, aip))
										{
											if (ip_same_subnet((struct prefix_ipv4 *)&prefix_tmp, aip))
											{
												flag = 1;
												nexthop = XCALLOC(MTYPE_ISIS_NEXTHOP, sizeof(struct isis_nexthop));
												memset(nexthop, 0, sizeof(struct isis_nexthop));
												nexthop->ifindex = circuit->interface->ifindex;
												memcpy(&nexthop->ip, aip, sizeof(struct in_addr));
												nexthop->lock++;
												listnode_add(rinfo_new->nexthops, nexthop);
											}
										}
									}
								}
							}
						}
					}
					else if (circuit->circ_type == CIRCUIT_T_P2P && circuit->u.p2p.neighbor)
					{
						adj = circuit->u.p2p.neighbor;
						if (adj)
						{
							if (adj->cost_out_of_range == 1)
								continue;

							for (ALL_LIST_ELEMENTS_RO(adj->ipv4_addrs, nnode, aip))
							{
								if (ip_same_subnet((struct prefix_ipv4 *)&prefix_tmp, aip))
								{
									flag = 1;
									nexthop = XCALLOC(MTYPE_ISIS_NEXTHOP, sizeof(struct isis_nexthop));
									memset(nexthop, 0, sizeof(struct isis_nexthop));
									nexthop->ifindex = circuit->interface->ifindex;
									memcpy(&nexthop->ip, aip, sizeof(struct in_addr));
									nexthop->lock++;
									listnode_add(rinfo_new->nexthops, nexthop);
								}
							}
						}
					}

					if (flag == 0)
					{

						nexthop = XCALLOC(MTYPE_ISIS_NEXTHOP, sizeof(struct isis_nexthop));
						memset(nexthop, 0, sizeof(struct isis_nexthop));
						nexthop->ifindex = circuit->interface->ifindex;
						memcpy(&nexthop->ip, &prefix.u.prefix4, sizeof(struct in_addr));
						nexthop->lock++;
						listnode_add(isis->nexthops, nexthop);
						listnode_add(rinfo_new->nexthops, nexthop);
					}

					flag = 0;

					if (!rinfo_old)
					{
						rinfo = rinfo_new;
						UNSET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
					}
					else
					{
						if (!isis_direct_route_info_same(rinfo_new, rinfo_old, AF_INET))
						{
							for (ALL_LIST_ELEMENTS_RO(rinfo_old->nexthops, node, nexthop))
							{
								nh = isis_nexthop_create_old(&nexthop->ip, nexthop->ifindex);
								listnode_add(rinfo_new->old_nexthops, nh);
							}

							isis_route_info_delete(rinfo_old);
							rinfo = rinfo_new;
							UNSET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
						}
						else
						{
							isis_route_info_delete(rinfo_new);
							rinfo = rinfo_old;
						}
					}

					SET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT);
					SET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);
					SET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ACTIVE);
					route_node->info = rinfo;
				}
			}
		}

		if (family == AF_INET6 && circuit->ipv6_router)
		{
			prefix.family = AF_INET6;
			for (ALL_LIST_ELEMENTS_RO(circuit->ipv6_non_link, ipnode, p6))
			{
				/*ipv6*/
				route_node = route_node_get(table, p6);
				rinfo_old = route_node->info;
				if (circuit->state != C_STATE_UP || circuit->area != area)
				{
					if (rinfo_old)
					{
						SET_FLAG(rinfo_old->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT);
					}
				}
				else
				{
					rinfo_new = XCALLOC(MTYPE_ISIS_ROUTE_INFO, sizeof(struct isis_route_info));
					rinfo_new->old_nexthops6 = list_new();
					rinfo_new->nexthops6 = list_new();

					rinfo_new->depth = 1;
					if (area->oldmetric)
					{
						rinfo_new->cost = circuit->metrics_v6[level - 1].metric_default;
					}
					else
					{
						rinfo_new->cost = circuit->te_metric_v6[level - 1];
					}
					if (circuit->circ_type == CIRCUIT_T_BROADCAST)
					{
						adjdb = circuit->u.bc.adjdb[level - 1];
						if (adjdb && adjdb->count)
						{
							for (ALL_LIST_ELEMENTS_RO(adjdb, node, adj))
							{
								if (adj)
								{
									for (ALL_LIST_ELEMENTS_RO(adj->ipv6_addrs, nnode, ip6))
									{
										flag = 1;
										nexthop6 = XCALLOC(MTYPE_ISIS_NEXTHOP6, sizeof(struct isis_nexthop6));
										memset(nexthop6, 0, sizeof(struct isis_nexthop6));
										nexthop6->ifindex = circuit->interface->ifindex;
										memcpy(&nexthop6->ip6, ip6, sizeof(struct in6_addr));
										nexthop6->lock++;
										listnode_add(rinfo_new->nexthops6, nexthop6);
									}
								}
							}
						}
					}
					else if (circuit->circ_type == CIRCUIT_T_P2P && circuit->u.p2p.neighbor)
					{
						adj = circuit->u.p2p.neighbor;
						if (adj)
						{
							for (ALL_LIST_ELEMENTS_RO(adj->ipv6_addrs, nnode, ip6))
							{
								flag = 1;
								nexthop6 = XCALLOC(MTYPE_ISIS_NEXTHOP6, sizeof(struct isis_nexthop6));
								memset(nexthop6, 0, sizeof(struct isis_nexthop6));
								nexthop6->ifindex = circuit->interface->ifindex;
								memcpy(&nexthop6->ip6, ip6, sizeof(struct in6_addr));
								nexthop6->lock++;
								listnode_add(rinfo_new->nexthops6, nexthop6);
							}
						}
					}

					if (flag == 0)
					{
						for (ALL_LIST_ELEMENTS_RO(circuit->ipv6_link, cnnode, ipv6))
						{
							nexthop6 = isis_nexthop6_create(&ipv6->prefix, circuit->interface->ifindex);
							listnode_add(rinfo_new->nexthops6, nexthop6);
						}
					}

					flag = 0;

					if (!rinfo_old)
					{
						rinfo = rinfo_new;
						UNSET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
					}
					else
					{
						if (!isis_direct_route_info_same(rinfo_new, rinfo_old, AF_INET6))
						{
							for (ALL_LIST_ELEMENTS_RO(rinfo_old->nexthops6, node, nexthop6))
							{
								nh6 = isis_nexthop6_create_old(&nexthop6->ip6, nexthop6->ifindex);
								listnode_add(rinfo_new->old_nexthops6, nh6);
							}

							isis_route_info_delete(rinfo_old);
							rinfo = rinfo_new;
							UNSET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
						}
						else
						{
							isis_route_info_delete(rinfo_new);
							rinfo = rinfo_old;
						}
					}
					SET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_DIRECT_CONNECT);
					SET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);
					SET_FLAG(rinfo->flag, ISIS_ROUTE_FLAG_ACTIVE);
					route_node->info = rinfo;
				}
			}
		}
	}
}
