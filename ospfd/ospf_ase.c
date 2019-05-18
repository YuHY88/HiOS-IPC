/*
 * OSPF AS external route calculation.
 * Copyright (C) 1999, 2000 Alex Zinin, Toshiaki Takada
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

#include <zebra.h>

#include "lib/thread.h"
#include "lib/memory.h"
#include "lib/linklist.h"
#include "lib/prefix.h"
#include "lib/if.h"
#include "lib/table.h"
#include "lib/vty.h"
#include "lib/log.h"
#include "lib/timer.h"

#include "ospfd/ospfd.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_spf.h"
#include "ospfd/ospf_route.h"
#include "ospfd/ospf_ase.h"
#include "ospfd/ospf_zebra.h"
#include "ospfd/ospf_dump.h"

#if 0
static struct ospf_route *
ospf_find_asbr_route_exclude_selfarea (struct ospf *ospf,
                                       struct route_table *rtrs, struct prefix_ipv4 *asbr, struct ospf_area *area)
{
    struct route_node *rn;
    struct ospf_route *or, *best = NULL;
    struct listnode *node;
    struct list *chosen;
    struct ospf_area *area_temp;

    /* Sanity check. */
    if (rtrs == NULL)
    {
        return NULL;
    }

    rn = route_node_lookup (rtrs, (struct prefix *) asbr);
    if (! rn)
    {
        return NULL;
    }

    route_unlock_node (rn);

    chosen = list_new ();

    /* First try to find intra-area non-bb paths. */
    if (!CHECK_FLAG (ospf->config, OSPF_RFC1583_COMPATIBLE))
    {
        for (ALL_LIST_ELEMENTS_RO ((struct list *) rn->info, node, or))
        {
            if (or->cost < OSPF_LS_INFINITY)
            {
                if (!OSPF_IS_AREA_ID_BACKBONE (or->u.std.area_id) &&
                        or->path_type == OSPF_PATH_INTRA_AREA)
                {
                    listnode_add (chosen, or);
                }
            }
        }
    }

    /* If none is found -- look through all. */
    if (listcount (chosen) == 0)
    {
        list_free (chosen);
        chosen = rn->info;
    }

    /* Now find the route with least cost. */
    for (ALL_LIST_ELEMENTS_RO (chosen, node, or))
    {
        if (or->cost < OSPF_LS_INFINITY)
        {
            if((area_temp = ospf_area_lookup_by_area_id(ospf,or->u.std.area_id)))
            {
                if (area_temp->external_routing != OSPF_AREA_DEFAULT)
                {
                    continue;
                }
                if (IPV4_ADDR_SAME (&or->u.std.area_id, &area->area_id))
                {
                    continue;
                }
                if (best == NULL)
                {
                    best = or;
                }
                else if (best->cost > or->cost)
                {
                    best = or;
                }
                else if (best->cost == or->cost &&
                         IPV4_ADDR_CMP (&best->u.std.area_id,
                                        &or->u.std.area_id) < 0)
                {
                    best = or;
                }
            }
        }
    }

    if (chosen != rn->info)
    {
        list_delete (chosen);
		chosen = NULL;
    }

    return best;
}
#endif

struct ospf_route *
ospf_find_asbr_route (struct ospf *ospf,
                      struct route_table *rtrs, struct prefix_ipv4 *asbr)
{
    struct list *chosen = NULL;
    struct route_node *rn = NULL;
    struct ospf_route *or = NULL;
    struct listnode *node = NULL;
    struct ospf_route *best = NULL;
    struct ospf_area *area_or = NULL;

    /* Sanity check. */
    if (rtrs == NULL)
	{
        return NULL;
    }

    rn = route_node_lookup (rtrs, (struct prefix *) asbr);
    if (! rn)
    {
        return NULL;
    }

    route_unlock_node (rn);

    chosen = list_new ();

    /* First try to find intra-area non-bb paths. */
    if (!CHECK_FLAG (ospf->config, OSPF_RFC1583_COMPATIBLE))
    {
        for (ALL_LIST_ELEMENTS_RO ((struct list *) rn->info, node, or))
        {
            if (or && or->cost < OSPF_LS_INFINITY)
            {
                if (!OSPF_IS_AREA_ID_BACKBONE (or->u.std.area_id) &&
                        or->path_type == OSPF_PATH_INTRA_AREA)
                {
                    listnode_add (chosen, or);
                }
            }
        }
    }

    /* If none is found -- look through all. */
    if (listcount (chosen) == 0)
    {
        list_free (chosen);
        chosen = rn->info;
    }

    /* Now find the route with least cost. */
    for (ALL_LIST_ELEMENTS_RO (chosen, node, or))
    {
        if (or && or->cost < OSPF_LS_INFINITY)
        {
            area_or = ospf_area_lookup_by_area_id(ospf,or->u.std.area_id);
			if(area_or == NULL)
			{
				continue;
			}
            if(!IS_AREA_DEFAULT(area_or))
            {
                continue;
            }
            if (best == NULL)
            {
                best = or;
            }
            else if (best->cost > or->cost)
            {
                best = or;
            }
            else if (best->cost == or->cost &&
                     IPV4_ADDR_CMP (&best->u.std.area_id,
                                    &or->u.std.area_id) < 0)
            {
                best = or;
            }
        }
    }

    if (chosen != rn->info)
    {
        list_delete (chosen);
		chosen = NULL;
    }

    return best;
}

/* acording to lsa find the route to asbr*/
static struct ospf_route *
ospf_find_asbr_route_by_lsa (struct ospf *ospf,
                             struct route_table *rtrs, struct prefix_ipv4 *asbr, struct ospf_lsa *lsa)
{
    struct list *chosen = NULL;
    struct listnode *node = NULL;
    struct route_node *rn = NULL;
    struct ospf_route *or = NULL;
    struct ospf_area *area = NULL;
    struct ospf_route *best = NULL;

    /* Sanity check. */
    if (rtrs == NULL)
	{
        return NULL;
    }

    rn = route_node_lookup (rtrs, (struct prefix *) asbr);
    if (! rn)
    {
        return NULL;
    }

    route_unlock_node (rn);

    chosen = list_new ();

    /* First try to find intra-area non-bb paths. */
    if (!CHECK_FLAG (ospf->config, OSPF_RFC1583_COMPATIBLE))
    {
        for (ALL_LIST_ELEMENTS_RO ((struct list *) rn->info, node, or))
        {
            if (or && or->cost < OSPF_LS_INFINITY)
            {
                if (!OSPF_IS_AREA_ID_BACKBONE (or->u.std.area_id) &&
                        or->path_type == OSPF_PATH_INTRA_AREA)
                {
                    listnode_add (chosen, or);
                }
            }
        }
    }

    /* If none is found -- look through all. */
    if (listcount (chosen) == 0)
    {
        list_free (chosen);
        chosen = rn->info;
    }

    /* Now find the route with least cost. */
    /*
      If the lsa is type-5 lsa ,the route must be in default area
      If the lsa is type-7 lsa ,the route must be in nssa area
    */
    for (ALL_LIST_ELEMENTS_RO (chosen, node, or))
    {
        if (or && or->cost < OSPF_LS_INFINITY)
        {
            if (!(or->u.std.flags & ROUTER_LSA_EXTERNAL))
            {
                continue;
            }
            area = ospf_area_lookup_by_area_id(ospf,or->u.std.area_id);
            if(area == NULL)
            {
                continue;
            }
            if(lsa->data->type == OSPF_AS_EXTERNAL_LSA)
            {
                if(!IS_AREA_DEFAULT(area))
                {
                    continue;
                }
                if (best == NULL)
                {
                    best = or;
                }
                else if (best->cost > or->cost)
                {
                    best = or;
                }
                else if (best->cost == or->cost &&
                         IPV4_ADDR_CMP (&best->u.std.area_id,
                                        &or->u.std.area_id) < 0)
                {
                    best = or;
                }
            }
            else if(lsa->data->type == OSPF_AS_NSSA_LSA)
            {
                if(!IS_AREA_NSSA(area))
                {
                    continue;
                }
                if (best == NULL)
                {
                    best = or;
                }
                else if (best->cost > or->cost)
                {
                    best = or;
                }
                else if (best->cost == or->cost &&
                         IPV4_ADDR_CMP (&best->u.std.area_id,
                                        &or->u.std.area_id) < 0)
                {
                    best = or;
                }
            }
        }
    }

    if (chosen != rn->info)
    {
        list_delete (chosen);
		chosen = NULL;
    }



    return best;
}


struct ospf_route *
ospf_find_asbr_route_through_area (struct route_table *rtrs,
                                   struct prefix_ipv4 *asbr,
                                   struct ospf_area *area)
{
    struct route_node *rn = NULL;

    /* Sanity check. */
    if (rtrs == NULL)
    {
        return NULL;
    }

    rn = route_node_lookup (rtrs, (struct prefix *) asbr);

    if (rn)
    {
        struct listnode *node;
        struct ospf_route *or;
        route_unlock_node (rn);
        for (ALL_LIST_ELEMENTS_RO ((struct list *) rn->info, node, or))
        {
            if (IPV4_ADDR_SAME (&or->u.std.area_id, &area->area_id))
            {
                return or;
            }
        }
    }

    return NULL;
}

static int
ospf_ase_forward_address_check (struct ospf *ospf, struct in_addr fwd_addr)
{
    struct listnode *ifn = NULL;
    struct ospf_interface *oi = NULL;
    for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, ifn, oi))
    {
        if (if_is_operative (oi->ifp))
        {
            if (oi->type != OSPF_IFTYPE_VIRTUALLINK)
            {
                if (IPV4_ADDR_SAME (&oi->address->u.prefix4, &fwd_addr))
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}


static void
ospf_ase_complete_direct_routes (struct ospf *ospf, struct ospf_route *ro, struct in_addr nexthop)
{
    struct listnode *node = NULL;
    struct ospf_path *op = NULL;
	
    for (ALL_LIST_ELEMENTS_RO (ro->paths, node, op))
    {
        //if (op->nexthop.s_addr == 0)
        if (! ospf_ase_forward_address_check (ospf, op->nexthop))
        {
            op->nexthop.s_addr = nexthop.s_addr;
        }
    }
}




static struct ospf_route *
ospf_ase_calculate_new_route (struct ospf_lsa *lsa,
                              struct ospf_route *asbr_route, u_int32_t metric)
{
    struct as_external_lsa *al = NULL;
    struct ospf_route *new_route = NULL;

    al = (struct as_external_lsa *) lsa->data;

    new_route = ospf_route_new ();

    /* Set redistributed type -- does make sense? */
    /* new_ei->type = type; */
    //new->id = al->header.id;
    new_route->id.s_addr = (al->header.id.s_addr)&(al->mask.s_addr);
    new_route->mask = al->mask;

    if (!IS_EXTERNAL_METRIC (al->e[0].tos))
    {
        if (IS_DEBUG_OSPF (lsa, LSA))
        {
            zlog_debug (OSPF_DBG_LSA, "Route[External]: type-1 created.");
        }
        new_route->path_type = OSPF_PATH_TYPE1_EXTERNAL;
        new_route->cost = asbr_route->cost + metric;      /* X + Y */
    }
    else
    {
        if (IS_DEBUG_OSPF (lsa, LSA))
        {
            zlog_debug (OSPF_DBG_LSA, "Route[External]: type-2 created.");
        }
        new_route->path_type = OSPF_PATH_TYPE2_EXTERNAL;
        new_route->cost = asbr_route->cost;           /* X */
        new_route->u.ext.type2_cost = metric;         /* Y */
    }

    new_route->type = OSPF_DESTINATION_NETWORK;
    new_route->u.ext.origin = lsa;
    new_route->u.ext.tag = ntohl (al->e[0].route_tag);
    new_route->u.ext.asbr = asbr_route;

	if(new_route == asbr_route)
    {
        zlog_warn("%s[%d] new_route == asbr_route",__func__,__LINE__);
        return NULL;
    }

    assert (new_route != asbr_route);

    return new_route;
}

#define OSPF_ASE_CALC_INTERVAL 1

/* rfc2328 16.4 calculate AS external route */
int
ospf_ase_calculate_route (struct ospf *ospf, struct ospf_lsa * lsa)
{
    int ret;
    u_int32_t metric;
    struct prefix_ipv4 asbr, p;
    struct route_node *rn = NULL;   
	struct ospf_route *or  = NULL;
    struct ospf_route *new_or = NULL;
    struct as_external_lsa *al = NULL;
    struct ospf_route *asbr_route = NULL;

	if(lsa == NULL)
    {
        zlog_warn("%s[%d] lsa == NULL",__func__,__LINE__);
        return 0;
    }
    assert (lsa);
	
    al = (struct as_external_lsa *) lsa->data;
    if (lsa->data->type == OSPF_AS_NSSA_LSA)
    {
        if (IS_DEBUG_OSPF_NSSA)
        {
            zlog_debug (OSPF_DBG_NSSA, "ospf_ase_calc(): Processing Type-7");
        }
    }
    /* Stay away from any Local Translated Type-7 LSAs */
    if (CHECK_FLAG (lsa->flags, OSPF_LSA_LOCAL_XLT))
    {
        if (IS_DEBUG_OSPF_NSSA)
        {
            zlog_debug (OSPF_DBG_NSSA, "ospf_ase_calc(): Rejecting Local Xlt'd");
        }
        return 0;
    }
    if (IS_DEBUG_OSPF (lsa, LSA))
        zlog_debug (OSPF_DBG_LSA, "Route[External]: Calculate AS-external-LSA to %s/%d",
                    inet_ntoa (al->header.id), ip_masklen (al->mask));
    /* (1) If the cost specified by the LSA is LSInfinity, or if the
     LSA's LS age is equal to MaxAge, then examine the next LSA. */
    if ((metric = GET_METRIC (al->e[0].metric)) >= OSPF_LS_INFINITY)
    {
        if (IS_DEBUG_OSPF (lsa, LSA))
        {
            zlog_debug (OSPF_DBG_LSA, "Route[External]: Metric is OSPF_LS_INFINITY");
        }
        return 0;
    }
    if (IS_LSA_MAXAGE (lsa))
    {
        if (IS_DEBUG_OSPF (lsa, LSA))
        {
            zlog_debug (OSPF_DBG_LSA, "Route[External]: AS-external-LSA is MAXAGE");
        }
        return 0;
    }
    /* (2) If the LSA was originated by the calculating router itself,
     examine the next LSA. */
    if (IS_LSA_SELF (lsa))
    {
        if (IS_DEBUG_OSPF (lsa, LSA))
        {
            zlog_debug (OSPF_DBG_LSA, "Route[External]: AS-external-LSA is self originated");
        }
        return 0;
    }
    /*
    rfc 3101 section 2.5-(3) paragraph 2.
    Else if the destination is a Type-7 default route (destination
    ID = DefaultDestination) and one of the following is true,
    then do nothing with this LSA and consider the next in the
    list

    Type-7 default lsa can not calculating router when the router is ABR --- add by tai
    */
    if (lsa->data->type == OSPF_AS_NSSA_LSA && IS_OSPF_ABR(ospf))
    {
        if((al->mask.s_addr == 0) && ((lsa->data->id.s_addr & (al->mask.s_addr)) == 0))
        {
            return 0;
        }
    }
    /* (3) Call the destination described by the LSA N.  N's address is
         obtained by masking the LSA's Link State ID with the
         network/subnet mask contained in the body of the LSA.  Look
         up the routing table entries (potentially one per attached
         area) for the AS boundary router (ASBR) that originated the
         LSA. If no entries exist for router ASBR (i.e., ASBR is
         unreachable), do nothing with this LSA and consider the next
         in the list. */
    asbr.family = AF_INET;
    asbr.prefix = al->header.adv_router;
    asbr.prefixlen = IPV4_MAX_BITLEN;
    apply_mask_ipv4 (&asbr);
    //asbr_route = ospf_find_asbr_route (ospf, ospf->new_rtrs, &asbr);
    asbr_route = ospf_find_asbr_route_by_lsa(ospf, ospf->new_rtrs, &asbr, lsa);
    if (asbr_route == NULL)
    {
        if (IS_DEBUG_OSPF (lsa, LSA))
        {
            zlog_debug (OSPF_DBG_LSA, "Route[External]: Can't find originating ASBR route");
        }
        return 0;
    }
    if (!(asbr_route->u.std.flags & ROUTER_LSA_EXTERNAL))
    {
        if (IS_DEBUG_OSPF (lsa, LSA))
        {
            zlog_debug (OSPF_DBG_LSA, "Route[External]: Originating router is not an ASBR");
        }
        return 0;
    }
    /*  Else, this LSA describes an AS external path to destination
        N.  Examine the forwarding address specified in the AS-
        external-LSA.  This indicates the IP address to which
        packets for the destination should be forwarded. */
    if (al->e[0].fwd_addr.s_addr == 0)
    {
        /* If the forwarding address is set to 0.0.0.0, packets should
        be sent to the ASBR itself. Among the multiple routing table
         entries for the ASBR, select the preferred entry as follows.
         If RFC1583Compatibility is set to "disabled", prune the set
         of routing table entries for the ASBR as described in
         Section 16.4.1. In any case, among the remaining routing
         table entries, select the routing table entry with the least
         cost; when there are multiple least cost routing table
         entries the entry whose associated area has the largest OSPF
         Area ID (when considered as an unsigned 32-bit integer) is
         chosen. */
        /* asbr_route already contains the requested route */
    }
    else
    {
        /* If the forwarding address is non-zero, look up the
            forwarding address in the routing table.[24] The matching
            routing table entry must specify an intra-area or inter-area
            path; if no such path exists, do nothing with the LSA and
            consider the next in the list. */
        if (! ospf_ase_forward_address_check (ospf, al->e[0].fwd_addr))
        {
            if (IS_DEBUG_OSPF (lsa, LSA))
                zlog_debug (OSPF_DBG_LSA, "Route[External]: Forwarding address is our router "
                            "address");
            return 0;
        }
        asbr.family = AF_INET;
        asbr.prefix = al->e[0].fwd_addr;
        asbr.prefixlen = IPV4_MAX_BITLEN;
        rn = route_node_match (ospf->new_table, (struct prefix *) &asbr);
        if (rn == NULL || (asbr_route = rn->info) == NULL)
        {
            if (IS_DEBUG_OSPF (lsa, LSA))
                zlog_debug (OSPF_DBG_LSA, "Route[External]: Can't find route to forwarding "
                            "address");
            if (rn)
            {
                route_unlock_node (rn);
            }
            return 0;
        }
        route_unlock_node (rn);
    }
    /* (4) Let X be the cost specified by the preferred routing table
           entry for the ASBR/forwarding address, and Y the cost
     specified in the LSA.  X is in terms of the link state
     metric, and Y is a type 1 or 2 external metric. */
    /* (5) Look up the routing table entry for the destination N.  If
           no entry exists for N, install the AS external path to N,
     with next hop equal to the list of next hops to the
     forwarding address, and advertising router equal to ASBR.
     If the external metric type is 1, then the path-type is set
     to type 1 external and the cost is equal to X+Y.  If the
     external metric type is 2, the path-type is set to type 2
     external, the link state component of the route's cost is X,
     and the type 2 cost is Y. */
    new_or = ospf_ase_calculate_new_route (lsa, asbr_route, metric);
    /* (6) Compare the AS external path described by the LSA with the
         existing paths in N's routing table entry, as follows. If
     the new path is preferred, it replaces the present paths in
     N's routing table entry.  If the new path is of equal
     preference, it is added to N's routing table entry's list of
     paths. */
    /* Set prefix. */
    p.family = AF_INET;
    /* in order to surpport appendix E */
    p.prefix.s_addr = (al->header.id.s_addr) & (al->mask.s_addr);
    p.prefixlen = ip_masklen (al->mask);
    /* if there is a Intra/Inter area route to the N
     do not install external route */
    if ((rn = route_node_lookup (ospf->new_table,
                                 (struct prefix *) &p)))
    {
        route_unlock_node(rn);
        if (rn->info == NULL)
        {
            zlog_debug (OSPF_DBG_OTHER, "Route[External]: rn->info NULL");
        }
        if (new_or)
        {
            ospf_route_free (new_or);
        }
		new_or = NULL;
        return 0;
    }
    /* Find a route to the same dest */
    /* If there is no route, create new one. */
    if ((rn = route_node_lookup (ospf->new_external_route,(struct prefix *) &p)))
    {
        route_unlock_node(rn);
    }
    if (!rn || (or = rn->info) == NULL)
    {
        if (IS_DEBUG_OSPF (lsa, LSA))
            zlog_debug (OSPF_DBG_LSA, "Route[External]: Adding a new route %s/%d",
                        inet_ntoa (p.prefix), p.prefixlen);
        ospf_route_add (ospf->new_external_route, &p, new_or, asbr_route);
        if (al->e[0].fwd_addr.s_addr)
        {
            ospf_ase_complete_direct_routes (ospf, new_or, al->e[0].fwd_addr);
        }
        return 0;
    }
    else
    {
        /* (a) Intra-area and inter-area paths are always preferred
               over AS external paths.

           (b) Type 1 external paths are always preferred over type 2
               external paths. When all paths are type 2 external
           paths, the paths with the smallest advertised type 2
           metric are always preferred. */
        ret = ospf_route_cmp (ospf, new_or, or);
        /*     (c) If the new AS external path is still indistinguishable
                   from the current paths in the N's routing table entry,
             and RFC1583Compatibility is set to "disabled", select
             the preferred paths based on the intra-AS paths to the
             ASBR/forwarding addresses, as specified in Section
             16.4.1.

               (d) If the new AS external path is still indistinguishable
                   from the current paths in the N's routing table entry,
             select the preferred path based on a least cost
             comparison.  Type 1 external paths are compared by
             looking at the sum of the distance to the forwarding
             address and the advertised type 1 metric (X+Y).  Type 2
             external paths advertising equal type 2 metrics are
             compared by looking at the distance to the forwarding
             addresses.
        */
        /* New route is better */
        if (ret < 0)
        {
            if (IS_DEBUG_OSPF (lsa, LSA))
            {
                zlog_debug (OSPF_DBG_LSA, "Route[External]: New route is better");
            }
            ospf_route_subst (rn, new_or, asbr_route);
            if (al->e[0].fwd_addr.s_addr)
            {
                ospf_ase_complete_direct_routes (ospf, new_or, al->e[0].fwd_addr);
            }
            or = new_or;
            new_or = NULL;
        }
        /* Old route is better */
        else if (ret > 0)
        {
            if (IS_DEBUG_OSPF (lsa, LSA))
            {
                zlog_debug (OSPF_DBG_LSA, "Route[External]: Old route is better");
            }
            /* do nothing */
        }
        /* Routes are equal */
        else
        {
            if (IS_DEBUG_OSPF (lsa, LSA))
            {
                zlog_debug (OSPF_DBG_LSA, "Route[External]: Routes are equal");
            }
            ospf_route_copy_nexthops (or, asbr_route->paths);
            if (al->e[0].fwd_addr.s_addr)
            {
                ospf_ase_complete_direct_routes (ospf, or, al->e[0].fwd_addr);
            }
        }
    }
    /* Make sure setting newly calculated ASBR route.*/
    or->u.ext.asbr = asbr_route;
    if (new_or)
    {
        ospf_route_free (new_or);
    }
	new_or = NULL;
    lsa->route = or;
    return 0;
}

static int
ospf_ase_route_match_same (struct route_table *rt, struct prefix *prefix,
                           struct ospf_route *newor, struct ospf *ospf)
{    
	struct listnode *n1 = NULL;
    struct listnode *n2 = NULL;
    struct ospf_path *op = NULL;
    struct route_node *rn = NULL;
    struct ospf_route *or = NULL;
    struct ospf_path *newop = NULL;

    if (! rt || ! prefix)
    {
        return 0;
    }
    rn = route_node_lookup (rt, prefix);
    if (! rn)
    {
        return 0;
    }
    route_unlock_node (rn);
    or = rn->info;
    if (or->path_type != newor->path_type)
    {
        ospf_zebra_delete((struct prefix_ipv4 *) prefix, or, ospf);
        return 0;
    }
    switch (or->path_type)
    {
    case OSPF_PATH_TYPE1_EXTERNAL:
        if (or->cost != newor->cost)
        {
            ospf_zebra_delete((struct prefix_ipv4 *)prefix, or, ospf);
            return 0;
        }
        break;
    case OSPF_PATH_TYPE2_EXTERNAL:
        if ((or->cost != newor->cost) ||
                (or->u.ext.type2_cost != newor->u.ext.type2_cost))
        {
            ospf_zebra_delete((struct prefix_ipv4 *)prefix, or, ospf);
            return 0;
        }
        break;
    default:
        //assert (0);
        return 0;
    }
    if (or->paths->count != newor->paths->count)
    {
        ospf_zebra_delete((struct prefix_ipv4 *)prefix, or, ospf);
        return 0;
    }
    /* Check each path. */
    for (n1 = listhead (or->paths), n2 = listhead (newor->paths);
            n1 && n2; n1 = listnextnode (n1), n2 = listnextnode (n2))
    {
        op = listgetdata (n1);
        newop = listgetdata (n2);
        if (! IPV4_ADDR_SAME (&op->nexthop, &newop->nexthop))
        {
            ospf_zebra_delete((struct prefix_ipv4 *)prefix, or, ospf);
            return 0;
        }
        if (op->ifindex != newop->ifindex)
        {
            ospf_zebra_delete((struct prefix_ipv4 *)prefix, or, ospf);
            return 0;
        }
    }

    if (or->u.ext.tag != newor->u.ext.tag)
    {
        return 0;
    }

    return 1;
}

static int
ospf_ase_compare_tables (struct route_table *new_external_route,
                         struct route_table *old_external_route,struct ospf *ospf)
{
    struct route_node *rn  = NULL;
    struct ospf_route *or = NULL;
	struct route_node *new_rn = NULL;
	
    /* Remove deleted routes */
    for (rn = route_top (old_external_route); rn; rn = route_next (rn))
    {
        if ((or = rn->info))
        {
            if (! (new_rn = route_node_lookup (new_external_route, &rn->p)))
            {
                ospf_zebra_delete ((struct prefix_ipv4 *) &rn->p, or,ospf);
            }
            else
            {
                route_unlock_node (new_rn);
            }
        }
    }
    /* Install new routes */
    for (rn = route_top (new_external_route); rn; rn = route_next (rn))
    {
        if ((or = rn->info) != NULL)
        {
            if (! ospf_ase_route_match_same (old_external_route, &rn->p, or, ospf))
            {
                ospf_zebra_add ((struct prefix_ipv4 *) &rn->p, or,ospf);
            }
        }
    }
    return 0;
}

static int
ospf_ase_calculate_timer (void *t)
{
    struct timeval start_time, stop_time;
    struct ospf *ospf = NULL;
    struct ospf_lsa *lsa = NULL;
    struct route_node *rn = NULL;
    struct listnode *node = NULL;
    struct ospf_area *area = NULL;
	
    ospf = (struct ospf *)(t);
    if(ospf == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
    assert(ospf);
	
    ospf->t_ase_calc = 0;
    if (ospf->ase_calc)
    {
        ospf->ase_calc = 0;
        time_get_time(TIME_CLK_MONOTONIC, &start_time);
        /* Calculate external route for each AS-external-LSA */
        LSDB_LOOP (EXTERNAL_LSDB (ospf), rn, lsa)
        {
            ospf_ase_calculate_route (ospf, lsa);
        }
        /*  This version simple adds to the table all NSSA areas  */
        if (ospf->anyNSSA)
        {
            for (ALL_LIST_ELEMENTS_RO (ospf->areas, node, area))
            {
                if (IS_DEBUG_OSPF_NSSA)
                    zlog_debug (OSPF_DBG_NSSA, "ospf_ase_calculate_timer(): looking at area %s",
                                inet_ntoa (area->area_id));
                if (area->external_routing == OSPF_AREA_NSSA)
                    LSDB_LOOP (NSSA_LSDB (area), rn, lsa)
                    ospf_ase_calculate_route (ospf, lsa);
            }
        }
        /* kevinm: And add the NSSA routes in ospf_top */
        LSDB_LOOP (NSSA_LSDB (ospf),rn,lsa)
            ospf_ase_calculate_route(ospf,lsa);
        /* Compare old and new external routing table and install the
            difference info zebra/kernel */
        ospf_ase_compare_tables (ospf->new_external_route,
                                 ospf->old_external_route,ospf);
        /* Delete old external routing table */
        ospf_route_table_free (ospf->old_external_route);
        ospf->old_external_route = ospf->new_external_route;
        ospf->new_external_route = route_table_init ();
        
        time_get_time(TIME_CLK_MONOTONIC, &stop_time);
        zlog_debug (OSPF_DBG_NSSA, "SPF Processing Time(usecs): External Routes: %lld\n",
                   (stop_time.tv_sec - start_time.tv_sec)*1000000LL+
                   (stop_time.tv_usec - start_time.tv_usec));
    }
    return 0;
}

void
ospf_ase_calculate_schedule (struct ospf *ospf)
{
    if (ospf == NULL)
    {
        return;
    }
    ospf->ase_calc = 1;
}

void
ospf_ase_calculate_timer_add (struct ospf *ospf)
{
    if (ospf == NULL)
    {
        return;
    }
    if (! ospf->t_ase_calc)
    {
        /*ospf->t_ase_calc = thread_add_timer (master_ospf, ospf_ase_calculate_timer,
                                             ospf, OSPF_ASE_CALC_INTERVAL);*/
		ospf->t_ase_calc = high_pre_timer_add ((char *)"ospf_ase_calc_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf_ase_calculate_timer, ospf, (OSPF_ASE_CALC_INTERVAL)*1000);
    }
}

void
ospf_ase_register_external_lsa (struct ospf_lsa *lsa, struct ospf *top)
{
    struct prefix_ipv4 p;
    struct list *lst = NULL;
    struct route_node *rn = NULL;
    struct as_external_lsa *al = NULL;
	
    al = (struct as_external_lsa *) lsa->data;
    p.family = AF_INET;
    p.prefix = lsa->data->id;
    p.prefixlen = ip_masklen (al->mask);
    apply_mask_ipv4 (&p);
    rn = route_node_get (top->external_lsas, (struct prefix *) &p);
    if ((lst = rn->info) == NULL)
    {
        rn->info = lst = list_new();
    }
    else
    {
        route_unlock_node (rn);
    }
    /* We assume that if LSA is deleted from DB
       is is also deleted from this RT */
    listnode_add (lst, ospf_lsa_lock (lsa)); /* external_lsas lst */
}

void
ospf_ase_unregister_external_lsa (struct ospf_lsa *lsa, struct ospf *top)
{
    struct prefix_ipv4 p;
    struct list *lst = NULL;
    struct route_node *rn = NULL;
    struct as_external_lsa *al = NULL;
	
    al = (struct as_external_lsa *) lsa->data;
    p.family = AF_INET;
    p.prefix = lsa->data->id;
    p.prefixlen = ip_masklen (al->mask);
    apply_mask_ipv4 (&p);
    rn = route_node_lookup (top->external_lsas, (struct prefix *) &p);
    if (rn)
    {
        lst = rn->info;
        listnode_delete (lst, lsa);
        ospf_lsa_unlock (&lsa); /* external_lsas list */

		/*
		若属于rn节点的lsa全部删除，则释放lsa链表，
		并将rn->lock = 1用于删除该节点rn
		*/
		if (lst->count == 0)
		{
			list_free(lst);
			rn->info = NULL;
			rn->lock = 1;
		}
        route_unlock_node (rn);
    }
}

void
ospf_ase_external_lsas_finish (struct route_table *rt)
{
    struct list *lst = NULL;
    struct ospf_lsa *lsa = NULL;
    struct route_node *rn = NULL;
    struct listnode *node = NULL;
	struct listnode *nnode = NULL;
	
    for (rn = route_top (rt); rn; rn = route_next (rn))
    {
        if ((lst = rn->info) != NULL)
        {
            for (ALL_LIST_ELEMENTS (lst, node, nnode, lsa))
            {
                ospf_lsa_unlock (&lsa);    /* external_lsas lst */
            }
            list_delete (lst);
			lst = NULL;
        }
    }
    route_table_finish (rt);
}

void
ospf_ase_incremental_update (struct ospf *ospf, struct ospf_lsa *lsa)
{
    struct prefix_ipv4 p;
    struct list *lsas = NULL;
    struct listnode *node = NULL;
    struct route_node *rn = NULL;
	struct route_node *rn2 = NULL;
    struct route_table *tmp_old = NULL;
    struct as_external_lsa *al = NULL;
    
    al = (struct as_external_lsa *) lsa->data;
    p.family = AF_INET;
    p.prefix.s_addr = lsa->data->id.s_addr & al->mask.s_addr;
    p.prefixlen = ip_masklen (al->mask);
    apply_mask_ipv4 (&p);
    
    /* if new_table is NULL, there was no spf calculation, thus
    incremental update is unneeded */
    if (!ospf->new_table)
    {
        return;
    }
    
    /* If there is already an intra-area or inter-area route
    to the destination, no recalculation is necessary
    (internal routes take precedence). */
    rn = route_node_lookup (ospf->new_table, (struct prefix *) &p);
    if (rn)
    {
        route_unlock_node (rn);
        if (rn->info)
        {
            return;
        }
    }
    rn = route_node_lookup (ospf->external_lsas, (struct prefix *) &p);
	
	if(rn == NULL || rn->info == NULL)
    {
        zlog_warn("%s[%d] rn or rn->info is NULL",__func__,__LINE__);
        return ;
    }
    assert (rn);
    assert (rn->info);
	
    lsas = rn->info;
    route_unlock_node (rn);
    for (ALL_LIST_ELEMENTS_RO (lsas, node, lsa))
    {
        ospf_ase_calculate_route (ospf, lsa);
    }
    /* prepare temporary old routing table for compare */
    tmp_old = route_table_init ();
    rn = route_node_lookup (ospf->old_external_route, (struct prefix *) &p);
    if (rn && rn->info)
    {
        rn2 = route_node_get (tmp_old, (struct prefix *) &p);
        rn2->info = rn->info;
        route_unlock_node (rn);
    }
    /* install changes to zebra */
    ospf_ase_compare_tables (ospf->new_external_route, tmp_old,ospf);
    /* update ospf->old_external_route table */
    if (rn && rn->info)
    {
        ospf_route_free ((struct ospf_route *) rn->info);
		rn->info = NULL;
    }
    rn2 = route_node_lookup (ospf->new_external_route, (struct prefix *) &p);
    /* if new route exists, install it to ospf->old_external_route */
    if (rn2 && rn2->info)
    {
        if (!rn)
        {
            rn = route_node_get (ospf->old_external_route, (struct prefix *) &p);
        }
        rn->info = rn2->info;
    }
    else
    {
        /* remove route node from ospf->old_external_route */
        if (rn)
        {
            rn->info = NULL;
            route_unlock_node (rn);
        }
    }
    if (rn2)
    {
        /* rn2->info is stored in route node of ospf->old_external_route */
        rn2->info = NULL;
        route_unlock_node (rn2);
        route_unlock_node (rn2);
    }
    route_table_finish (tmp_old);
    tmp_old = NULL;
}


