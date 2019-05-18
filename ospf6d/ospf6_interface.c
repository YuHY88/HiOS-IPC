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

#include <zebra.h>

#include "memory.h"
#include "if.h"
#include "log.h"
#include "command.h"
#include "thread.h"
#include "prefix.h"
#include "plist.h"
#include "timer.h"
#include "vrf.h"
#include "ifm_common.h"

#include "ospf6_lsa.h"
#include "ospf6_lsdb.h"
#include "ospf6_message.h"
#include "ospf6_route.h"
#include "ospf6_top.h"
#include "ospf6_area.h"
#include "ospf6_interface.h"
#include "ospf6_neighbor.h"
#include "ospf6_intra.h"
#include "ospf6_spf.h"
#include "ospf6_snmp.h"
#include "ospf6_zebra.h"
#include "ospf6d.h"
#include "ospf6_abr.h"

unsigned char conf_debug_ospf6_interface = 0;

const char *ospf6_interface_state_str[] =
{
    "None",
    "Down",
    "Loopback",
    "Waiting",
    "P2P",
    "DROther",
    "BDR",
    "DR",
    NULL
};



struct ospf6_interface *
ospf6_interface_lookup_by_ifindex(ifindex_t ifindex)
{
    struct ospf6_interface *oi;
    struct interface *ifp;

    ifp = if_lookup_by_index(ifindex);

    if (ifp == NULL)
    {
        return (struct ospf6_interface *) NULL;
    }

    oi = (struct ospf6_interface *) ifp->info;
    return oi;
}

/* schedule routing table recalculation */
static void
ospf6_interface_lsdb_hook(struct ospf6_lsa *lsa, unsigned int reason)
{
    struct ospf6_interface *oi = NULL;

    if (lsa == NULL)
    {
        return;
    }

    oi = lsa->lsdb->data;

    switch (ntohs(lsa->header->type))
    {
        case OSPF6_LSTYPE_LINK:

            if (oi->state == OSPF6_INTERFACE_DR)
            {
                OSPF6_INTRA_PREFIX_LSA_SCHEDULE_TRANSIT(oi);
            }

            ospf6_spf_schedule(oi->ospf6, reason);
            break;
        default:
            break;
    }
}

static void
ospf6_interface_lsdb_hook_add(struct ospf6_lsa *lsa)
{
    ospf6_interface_lsdb_hook(lsa, ospf6_lsadd_to_spf_reason(lsa));
}

static void
ospf6_interface_lsdb_hook_remove(struct ospf6_lsa *lsa)
{
    ospf6_interface_lsdb_hook(lsa, ospf6_lsremove_to_spf_reason(lsa));
}

u_char
ospf6_default_iftype(struct interface *ifp)
{
    if (if_is_pointopoint(ifp))
    {
        return OSPF_IFTYPE_POINTOPOINT;
    }
    else if (if_is_loopback(ifp))
    {
        return OSPF_IFTYPE_LOOPBACK;
    }
    else
    {
        return OSPF_IFTYPE_BROADCAST;
    }
}

static u_int32_t
ospf6_interface_get_cost(struct ospf6_interface *oi)
{
    /* If all else fails, use default OSPF cost */
    u_int32_t cost;
    u_int32_t bw, refbw;
    struct ospf6 *ospf6 = oi->ospf6;

	if(if_is_loopback(oi->interface))
	{
		return 0;
	}
		
	
    bw = oi->interface->bandwidth ? oi->interface->bandwidth : OSPF6_INTERFACE_BANDWIDTH;
    refbw = ospf6 ? ospf6->ref_bandwidth : OSPF6_REFERENCE_BANDWIDTH;

    /* A specifed ip ospf cost overrides a calculated one. */
    if (CHECK_FLAG(oi->flag, OSPF6_INTERFACE_NOAUTOCOST))
    {
        cost = oi->cost;
    }
    else
    {
        cost = (u_int32_t)((double)refbw / (double)bw + (double)0.5);

        if (cost < 1)
        {
            cost = 1;
        }
        else if (cost > UINT32_MAX)
        {
            cost = UINT32_MAX;
        }
    }

    return cost;
}

static void
ospf6_interface_recalculate_cost(struct ospf6_interface *oi)
{
    u_int32_t newcost;
    newcost = ospf6_interface_get_cost(oi);

    if (newcost == oi->cost)
    {
        return;
    }

    oi->cost = newcost;
    /* update cost held in route_connected list in ospf6_interface */
    ospf6_interface_connected_route_update(oi->interface);

    /* execute LSA hooks */
    if (oi->area)
    {
        OSPF6_LINK_LSA_SCHEDULE(oi);
        OSPF6_ROUTER_LSA_SCHEDULE(oi->area);
        OSPF6_NETWORK_LSA_SCHEDULE(oi);
        OSPF6_INTRA_PREFIX_LSA_SCHEDULE_TRANSIT(oi);
        OSPF6_INTRA_PREFIX_LSA_SCHEDULE_STUB(oi->area);
    }
}

static void
ospf6_interface_set_cost(struct ospf6_interface *oi)
{
    u_int32_t newcost;
    newcost = ospf6_interface_get_cost(oi);

    oi->cost = newcost;
	
    /* update cost held in route_connected list in ospf6_interface */
    ospf6_interface_connected_route_update(oi->interface);

    /* execute LSA hooks */
    if (oi->area)
    {
        OSPF6_LINK_LSA_SCHEDULE(oi);
        OSPF6_ROUTER_LSA_SCHEDULE(oi->area);
        OSPF6_NETWORK_LSA_SCHEDULE(oi);
        OSPF6_INTRA_PREFIX_LSA_SCHEDULE_TRANSIT(oi);
        OSPF6_INTRA_PREFIX_LSA_SCHEDULE_STUB(oi->area);
    }
}


/* get the interface mode */
int ospf6_get_intf_mode(ifindex_t ifindex)
{
    uint8_t pmode = 0;
    int     ret = 0;
    zlog_debug(OSPF6_DBG_OTHER, "%s[%d]: In function '%s'.\n", __FILE__, __LINE__, __func__);
    
    ret = ifm_get_mode(ifindex, MODULE_ID_OSPF6, &pmode);

    if (ret != 0)
    {
        zlog_err("ifm_get_mode error %s[%d]: In function '%s'.\n", __FILE__, __LINE__, __func__);
        return -1;
    }
    else if (pmode == IFNET_MODE_L3)
    {
        return IFNET_MODE_L3;
    }
    else if (pmode == IFNET_MODE_L2)
    {
        return IFNET_MODE_L2;
    }
    else if (pmode == IFNET_MODE_SWITCH)
    {
        return IFNET_MODE_SWITCH;
    }
    else if (pmode == IFNET_MODE_PHYSICAL)
    {
        return IFNET_MODE_PHYSICAL;
    }
    else
    {
        return IFNET_MODE_MAX;
    }

    return IFNET_MODE_INVALID;
}


/* Create new ospf6 interface structure */
struct ospf6_interface *
ospf6_interface_create(struct ospf6 *o, struct interface *ifp)
{
    struct ospf6_interface *oi = NULL;
    unsigned int iobuflen;

    oi = (struct ospf6_interface *)
         XCALLOC(MTYPE_OSPF6_IF, sizeof(struct ospf6_interface));

    if (!oi)
    {
        zlog_err("Can't malloc ospf6_interface for ifindex %0x", ifp->ifindex);
        return (struct ospf6_interface *) NULL;
    }
	memset(oi, 0, sizeof(struct ospf6_interface));

    oi->area = (struct ospf6_area *) NULL;
    oi->neighbor_list = list_new();
    oi->neighbor_list->cmp = ospf6_neighbor_cmp;
    oi->linklocal_addr = (struct in6_addr *) NULL;
    oi->instance_id = OSPF6_INTERFACE_INSTANCE_ID;
    oi->transdelay = OSPF6_INTERFACE_TRANSDELAY;
    oi->priority = OSPF6_INTERFACE_PRIORITY;

    oi->hello_interval = OSPF_HELLO_INTERVAL_DEFAULT;
    oi->dead_interval = OSPF_ROUTER_DEAD_INTERVAL_DEFAULT;
    oi->rxmt_interval = OSPF_RETRANSMIT_INTERVAL_DEFAULT;
    oi->type = ospf6_default_iftype(ifp);
    oi->state = OSPF6_INTERFACE_DOWN;
    oi->flag = 0;
    oi->mtu_ignore = 0;
	
    oi->ospf6 = o;

	

    /* Try to adjust I/O buffer size with IfMtu */
    oi->ifmtu = ifp->mtu6;

    iobuflen = ospf6_iobuf_size(ifp->mtu6);

    if (oi->ifmtu > iobuflen)
    {
        if (IS_OSPF6_DEBUG_INTERFACE)
            zlog_debug(OSPF6_DBG_INTERFACE, "Interface %s: IfMtu is adjusted to I/O buffer size: %d.",
                       ifp->name, iobuflen);

        oi->ifmtu = iobuflen;
    }

    oi->lsupdate_list = ospf6_lsdb_create(oi);
    oi->lsack_list = ospf6_lsdb_create(oi);
    oi->lsdb = ospf6_lsdb_create(oi);
    oi->lsdb->hook_add = ospf6_interface_lsdb_hook_add;
    oi->lsdb->hook_remove = ospf6_interface_lsdb_hook_remove;
    oi->lsdb_self = ospf6_lsdb_create(oi);

    oi->route_connected = OSPF6_ROUTE_TABLE_CREATE(INTERFACE, CONNECTED_ROUTES);
    oi->route_connected->scope = oi;

    /* link both */
    oi->interface = ifp;
    ifp->info = oi;

    /* Compute cost. */
    oi->cost = ospf6_interface_get_cost(oi);

    return oi;
}

void
ospf6_interface_delete(struct ospf6_interface *oi)
{
    struct listnode *node, *nnode;
    struct ospf6_neighbor *on;

    for (ALL_LIST_ELEMENTS(oi->neighbor_list, node, nnode, on))
    {
        ospf6_neighbor_delete(on);
		on = NULL;
    }

    list_delete(oi->neighbor_list);
	oi->neighbor_list = NULL;
    OSPF6_TIMER_OFF(oi->thread_send_hello);
	OSPF6_TIMER_OFF(oi->t_wait);
    THREAD_OFF(oi->thread_send_lsupdate);
    OSPF6_TIMER_OFF(oi->thread_send_lsack);
    ospf6_lsdb_remove_all(oi->lsdb);
    ospf6_lsdb_remove_all(oi->lsupdate_list);
    ospf6_lsdb_remove_all(oi->lsack_list);
    ospf6_lsdb_delete(oi->lsdb);
    ospf6_lsdb_delete(oi->lsdb_self);
    ospf6_lsdb_delete(oi->lsupdate_list);
    ospf6_lsdb_delete(oi->lsack_list);
	oi->lsdb = NULL;
	oi->lsdb_self = NULL;
	oi->lsupdate_list = NULL;
	oi->lsack_list = NULL;
    ospf6_route_table_delete(oi->route_connected, oi->ospf6);
	oi->route_connected = NULL;
    /* cut link */
	if(oi->interface)
	{
    	oi->interface->info = NULL;
	}
	
	oi->area = (struct ospf6_area *) NULL;
	oi->ospf6 = (struct ospf6 *)NULL;
    /* cut link */
    oi->interface = NULL;

    /* plist_name */
    if (oi->plist_name)
    {
        XFREE(MTYPE_PREFIX_LIST_STR, oi->plist_name);
    }
	oi->plist_name = NULL;

    XFREE(MTYPE_OSPF6_IF, oi);
}

void
ospf6_interface_enable(struct ospf6_interface *oi)
{
    UNSET_FLAG(oi->flag, OSPF6_INTERFACE_DISABLE);
    ospf6_interface_state_update(oi->interface);
}

void
ospf6_interface_disable(struct ospf6_interface *oi)
{
    SET_FLAG(oi->flag, OSPF6_INTERFACE_DISABLE);
    thread_execute(master_ospf6, interface_down, oi, 0);
    ospf6_lsdb_remove_all(oi->lsdb);
    ospf6_lsdb_remove_all(oi->lsdb_self);
    ospf6_lsdb_remove_all(oi->lsupdate_list);
    ospf6_lsdb_remove_all(oi->lsack_list);
    OSPF6_TIMER_OFF(oi->thread_send_hello);
	OSPF6_TIMER_OFF(oi->t_wait);
    THREAD_OFF(oi->thread_send_lsupdate);
    OSPF6_TIMER_OFF(oi->thread_send_lsack);
    THREAD_OFF(oi->thread_network_lsa);
    THREAD_OFF(oi->thread_link_lsa);
    THREAD_OFF(oi->thread_intra_prefix_lsa);
}

static struct in6_addr *
ospf6_interface_get_linklocal_address(struct interface *ifp)
{
    struct listnode *n = NULL;
    struct connected *c = NULL;
    struct in6_addr *l = (struct in6_addr *) NULL;

    /* for each connected address */
    for (ALL_LIST_ELEMENTS_RO(ifp->connected, n, c))
    {
        /* if family not AF_INET6, ignore */
        if (c->address->family != AF_INET6)
        {
            continue;
        }

        /* linklocal scope check */
        if (IN6_IS_ADDR_LINKLOCAL(&c->address->u.prefix6))
        {
            l = &c->address->u.prefix6;
        }
    }

    return l;
}
int 
ospf6_interface_get_address_num(struct interface *ifp)
{
    struct listnode *n = NULL;
    struct connected *c = NULL;
	int num = 0;
    //struct in6_addr *l = (struct in6_addr *) NULL;

    /* for each connected address */
    for (ALL_LIST_ELEMENTS_RO(ifp->connected, n, c))
    {
        /* if family not AF_INET6, ignore */
        if (c->address->family != AF_INET6)
        {
            continue;
        }

        /* linklocal scope check */
        if (!IN6_IS_ADDR_LINKLOCAL(&c->address->u.prefix6))
        {
            //l = &c->address->u.prefix6;
            num ++;
        }
    }

    return num;
}


void
ospf6_interface_if_add(struct interface *ifp)
{
    struct ospf6_interface *oi;
    unsigned int iobuflen;
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        return;
    }

    /* Try to adjust I/O buffer size with IfMtu */
    if (oi->ifmtu == 0)
    {
        oi->ifmtu = ifp->mtu6;
    }

    iobuflen = ospf6_iobuf_size(ifp->mtu);

    if (oi->ifmtu > iobuflen)
    {
        if (IS_OSPF6_DEBUG_INTERFACE)
            zlog_debug(OSPF6_DBG_INTERFACE, "Interface %s: IfMtu is adjusted to I/O buffer size: %d.",
                       ifp->name, iobuflen);

        oi->ifmtu = iobuflen;
    }

    /* interface start */
    ospf6_interface_state_update(oi->interface);
}

void
ospf6_interface_if_del(struct interface *ifp)
{
    struct ospf6_interface *oi;
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        return;
    }

    /* interface stop */
    if (oi->area)
    {
        thread_execute(master_ospf6, interface_down, oi, 0);
		listnode_delete(oi->area->if_list, oi);
    }

    //listnode_delete(oi->area->if_list, oi);
	#if 0
    oi->area = (struct ospf6_area *) NULL;
	oi->ospf6 = (struct ospf6 *)NULL;
    /* cut link */
    oi->interface = NULL;
    ifp->info = NULL;
	#endif
    ospf6_interface_delete(oi);
	oi = NULL;
}

void
ospf6_interface_state_update(struct interface *ifp)
{
    struct ospf6_interface *oi;
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        return;
    }

    if (oi->area == NULL)
    {
        return;
    }

    if (CHECK_FLAG(oi->flag, OSPF6_INTERFACE_DISABLE))
    {
        return;
    }

    if (if_is_operative(ifp)
            && ((ospf6_interface_get_linklocal_address(oi->interface)&&(ospf6_interface_get_address_num(ifp)))
                || if_is_loopback(oi->interface)))
    {
        //thread_add_event(master_ospf6, interface_up, oi, 0);
        thread_execute (master_ospf6, interface_up, oi, 0);
    }
    else
    {
        //thread_add_event(master_ospf6, interface_down, oi, 0);
        thread_execute (master_ospf6, interface_down, oi, 0);
    }

    return;
}

void
ospf6_interface_connected_route_update(struct interface *ifp)
{
    struct ospf6_interface *oi = NULL;
    struct ospf6_route *route = NULL;
    struct connected *c = NULL;
    struct listnode *node = NULL, *nnode = NULL;
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {    
        return;
    }		

    /* reset linklocal pointer */
    oi->linklocal_addr = ospf6_interface_get_linklocal_address(ifp);

    /* if area is null, do not make connected-route list */
    if (oi->area == NULL)
    {
        return;
    }

    if (CHECK_FLAG(oi->flag, OSPF6_INTERFACE_DISABLE))
    {
        return;
    }

    /* update "route to advertise" interface route table */
    ospf6_route_remove_all(oi->route_connected, oi->ospf6);

    for (ALL_LIST_ELEMENTS(oi->interface->connected, node, nnode, c))
    {
        if (c->address->family != AF_INET6)
        {
			continue;
        }

		if (! if_is_operative(oi->interface))
		{
			continue;
		}

        CONTINUE_IF_ADDRESS_LINKLOCAL(IS_OSPF6_DEBUG_INTERFACE, c->address);
        CONTINUE_IF_ADDRESS_UNSPECIFIED(IS_OSPF6_DEBUG_INTERFACE, c->address);
        CONTINUE_IF_ADDRESS_LOOPBACK(IS_OSPF6_DEBUG_INTERFACE, c->address);
        CONTINUE_IF_ADDRESS_V4COMPAT(IS_OSPF6_DEBUG_INTERFACE, c->address);
        CONTINUE_IF_ADDRESS_V4MAPPED(IS_OSPF6_DEBUG_INTERFACE, c->address);

        /* apply filter */
        if (oi->plist_name)
        {
            struct prefix_list *plist;
            enum prefix_list_type ret;
            char buf[128];
            prefix2str(c->address, buf, sizeof(buf));
            plist = prefix_list_lookup(AFI_IP6, oi->plist_name);
            ret = prefix_list_apply(plist, (void *) c->address);

            if (ret == PREFIX_DENY)
            {
                if (IS_OSPF6_DEBUG_INTERFACE)
                    zlog_debug(OSPF6_DBG_INTERFACE, "%s on %s filtered by prefix-list %s ",
                               buf, oi->interface->name, oi->plist_name);

                continue;
            }
        }

        route = ospf6_route_create();
        memcpy(&route->prefix, c->address, sizeof(struct prefix));
        apply_mask(&route->prefix);
        route->type = OSPF6_DEST_TYPE_NETWORK;
        route->path.area_id = oi->area->area_id;
        route->path.type = OSPF6_PATH_TYPE_INTRA;
        route->path.cost = oi->cost;
        route->nexthop[0].ifindex = oi->interface->ifindex;
        inet_pton(AF_INET6, "::1", &route->nexthop[0].address);
        ospf6_route_add(route, oi->route_connected , oi->ospf6);
    }

	if (oi->area)
	{
	    /* create new Link-LSA */
	    OSPF6_LINK_LSA_SCHEDULE(oi);
	    OSPF6_INTRA_PREFIX_LSA_SCHEDULE_TRANSIT(oi);
	    OSPF6_INTRA_PREFIX_LSA_SCHEDULE_STUB(oi->area);
	}
}

static void
ospf6_interface_state_change(u_char next_state, struct ospf6_interface *oi)
{
    u_char prev_state;
    prev_state = oi->state;
    oi->state = next_state;

    if (prev_state == next_state)
    {
        return;
    }

    /* log */
    if (IS_OSPF6_DEBUG_INTERFACE)
    {
        zlog_debug(OSPF6_DBG_INTERFACE, "Interface state change %s: %s -> %s", oi->interface->name,
                   ospf6_interface_state_str[prev_state],
                   ospf6_interface_state_str[next_state]);
    }

    oi->state_change++;
#if 0

    if ((prev_state == OSPF6_INTERFACE_DR ||
            prev_state == OSPF6_INTERFACE_BDR) &&
            (next_state != OSPF6_INTERFACE_DR &&
             next_state != OSPF6_INTERFACE_BDR))
    {
        ospf6_sso(oi->interface->ifindex, &alldrouters6, IPV6_LEAVE_GROUP);
    }

    if ((prev_state != OSPF6_INTERFACE_DR &&
            prev_state != OSPF6_INTERFACE_BDR) &&
            (next_state == OSPF6_INTERFACE_DR ||
             next_state == OSPF6_INTERFACE_BDR))
    {
        ospf6_sso(oi->interface->ifindex, &alldrouters6, IPV6_JOIN_GROUP);
    }

#endif
    OSPF6_ROUTER_LSA_SCHEDULE(oi->area);

    if (next_state == OSPF6_INTERFACE_DOWN)
    {
        OSPF6_NETWORK_LSA_EXECUTE(oi);
        OSPF6_INTRA_PREFIX_LSA_EXECUTE_TRANSIT(oi);
        OSPF6_INTRA_PREFIX_LSA_SCHEDULE_STUB(oi->area);
    }
    else if (prev_state == OSPF6_INTERFACE_DR ||
             next_state == OSPF6_INTERFACE_DR)
    {
        OSPF6_NETWORK_LSA_SCHEDULE(oi);
        OSPF6_INTRA_PREFIX_LSA_SCHEDULE_TRANSIT(oi);
        OSPF6_INTRA_PREFIX_LSA_SCHEDULE_STUB(oi->area);
    }

#ifdef HAVE_SNMP

    /* Terminal state or regression */
    if ((next_state == OSPF6_INTERFACE_POINTTOPOINT) ||
            (next_state == OSPF6_INTERFACE_DROTHER) ||
            (next_state == OSPF6_INTERFACE_BDR) ||
            (next_state == OSPF6_INTERFACE_DR) ||
            (next_state < prev_state))
    {
        ospf6TrapIfStateChange(oi);
    }

#endif
}


/* DR Election, RFC2328 section 9.4 */

#define IS_ELIGIBLE(n) \
  ((n)->state >= OSPF6_NEIGHBOR_TWOWAY && (n)->priority != 0)

static struct ospf6_neighbor *
better_bdrouter(struct ospf6_neighbor *a, struct ospf6_neighbor *b)
{
    if ((a == NULL || ! IS_ELIGIBLE(a) || a->drouter == a->router_id) &&
            (b == NULL || ! IS_ELIGIBLE(b) || b->drouter == b->router_id))
    {
        return NULL;
    }
    else if (a == NULL || ! IS_ELIGIBLE(a) || a->drouter == a->router_id)
    {
        return b;
    }
    else if (b == NULL || ! IS_ELIGIBLE(b) || b->drouter == b->router_id)
    {
        return a;
    }

    if (a->bdrouter == a->router_id && b->bdrouter != b->router_id)
    {
        return a;
    }

    if (a->bdrouter != a->router_id && b->bdrouter == b->router_id)
    {
        return b;
    }

    if (a->priority > b->priority)
    {
        return a;
    }

    if (a->priority < b->priority)
    {
        return b;
    }

    if (ntohl(a->router_id) > ntohl(b->router_id))
    {
        return a;
    }

    if (ntohl(a->router_id) < ntohl(b->router_id))
    {
        return b;
    }

    zlog_warn("Router-ID duplicate ?");
    return a;
}

static struct ospf6_neighbor *
better_drouter(struct ospf6_neighbor *a, struct ospf6_neighbor *b)
{
    if ((a == NULL || ! IS_ELIGIBLE(a) || a->drouter != a->router_id) &&
            (b == NULL || ! IS_ELIGIBLE(b) || b->drouter != b->router_id))
    {
        return NULL;
    }
    else if (a == NULL || ! IS_ELIGIBLE(a) || a->drouter != a->router_id)
    {
        return b;
    }
    else if (b == NULL || ! IS_ELIGIBLE(b) || b->drouter != b->router_id)
    {
        return a;
    }

    if (a->drouter == a->router_id && b->drouter != b->router_id)
    {
        return a;
    }

    if (a->drouter != a->router_id && b->drouter == b->router_id)
    {
        return b;
    }

    if (a->priority > b->priority)
    {
        return a;
    }

    if (a->priority < b->priority)
    {
        return b;
    }

    if (ntohl(a->router_id) > ntohl(b->router_id))
    {
        return a;
    }

    if (ntohl(a->router_id) < ntohl(b->router_id))
    {
        return b;
    }

    zlog_warn("Router-ID duplicate ?");
    return a;
}

static u_char
dr_election(struct ospf6_interface *oi)
{
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf6_neighbor *on = NULL, *drouter = NULL, *bdrouter = NULL, myself;
    struct ospf6_neighbor *best_drouter = NULL, *best_bdrouter = NULL;
    u_char next_state = 0;
    drouter = bdrouter = NULL;
    best_drouter = best_bdrouter = NULL;

    /* pseudo neighbor myself, including noting current DR/BDR (1) */
    memset(&myself, 0, sizeof(myself));
    inet_ntop(AF_INET, &oi->area->ospf6->router_id, myself.name,
              sizeof(myself.name));
    myself.state = OSPF6_NEIGHBOR_TWOWAY;
    myself.drouter = oi->drouter;
    myself.bdrouter = oi->bdrouter;
    myself.priority = oi->priority;
    myself.router_id = oi->area->ospf6->router_id;

    /* Electing BDR (2) */
    for (ALL_LIST_ELEMENTS(oi->neighbor_list, node, nnode, on))
    {
        bdrouter = better_bdrouter(bdrouter, on);
    }

    best_bdrouter = bdrouter;
    bdrouter = better_bdrouter(best_bdrouter, &myself);

    /* Electing DR (3) */
    for (ALL_LIST_ELEMENTS(oi->neighbor_list, node, nnode, on))
    {
        drouter = better_drouter(drouter, on);
    }

    best_drouter = drouter;
    drouter = better_drouter(best_drouter, &myself);

    if (drouter == NULL)
    {
        drouter = bdrouter;
    }

    /* the router itself is newly/no longer DR/BDR (4) */
    if ((drouter == &myself && myself.drouter != myself.router_id) ||
            (drouter != &myself && myself.drouter == myself.router_id) ||
            (bdrouter == &myself && myself.bdrouter != myself.router_id) ||
            (bdrouter != &myself && myself.bdrouter == myself.router_id))
    {
        myself.drouter = (drouter ? drouter->router_id : htonl(0));
        myself.bdrouter = (bdrouter ? bdrouter->router_id : htonl(0));
        /* compatible to Electing BDR (2) */
        bdrouter = better_bdrouter(best_bdrouter, &myself);
        /* compatible to Electing DR (3) */
        drouter = better_drouter(best_drouter, &myself);

        if (drouter == NULL)
        {
            drouter = bdrouter;
        }
    }

    /* Set interface state accordingly (5) */
    if (drouter && drouter == &myself)
    {
        next_state = OSPF6_INTERFACE_DR;
    }
    else if (bdrouter && bdrouter == &myself)
    {
        next_state = OSPF6_INTERFACE_BDR;
    }
    else
    {
        next_state = OSPF6_INTERFACE_DROTHER;
    }

    /* If NBMA, schedule Start for each neighbor having priority of 0 (6) */

    /* XXX */

    /* If DR or BDR change, invoke AdjOK? for each neighbor (7) */

    /* RFC 2328 section 12.4. Originating LSAs (3) will be handled
       accordingly after AdjOK */
    if (oi->drouter != (drouter ? drouter->router_id : htonl(0)) ||
            oi->bdrouter != (bdrouter ? bdrouter->router_id : htonl(0)))
    {
        if (IS_OSPF6_DEBUG_INTERFACE)
            zlog_debug(OSPF6_DBG_INTERFACE, "DR Election on %s: DR: %s BDR: %s", oi->interface->name,
                       (drouter ? drouter->name : "0.0.0.0"),
                       (bdrouter ? bdrouter->name : "0.0.0.0"));

        for (ALL_LIST_ELEMENTS_RO(oi->neighbor_list, node, on))
        {
            if (on->state < OSPF6_NEIGHBOR_TWOWAY)
            {
                continue;
            }

            /* Schedule AdjOK. */
            thread_add_event(master_ospf6, adj_ok, on, 0);
        }
    }

    oi->drouter = (drouter ? drouter->router_id : htonl(0));
    oi->bdrouter = (bdrouter ? bdrouter->router_id : htonl(0));
    return next_state;
}


/* Interface State Machine */
int
interface_up(struct thread *thread)
{
    struct ospf6_interface *oi = NULL;
    oi = (struct ospf6_interface *) THREAD_ARG(thread);

    assert(oi && oi->interface);

    if (IS_OSPF6_DEBUG_INTERFACE)
    {
        zlog_debug(OSPF6_DBG_INTERFACE, "Interface Event %s: [InterfaceUp]",
                   oi->interface->name);
    }

    /* check physical interface is up */
    if (! if_is_operative(oi->interface))
    {
        if (IS_OSPF6_DEBUG_INTERFACE)
            zlog_debug(OSPF6_DBG_INTERFACE, "Interface %s is down, can't execute [InterfaceUp]",
                       oi->interface->name);

        return 0;
    }

    /* check interface has a link-local address */
    if (!(ospf6_interface_get_linklocal_address(oi->interface)
            || if_is_loopback(oi->interface)))
    {
        if (IS_OSPF6_DEBUG_INTERFACE)
            zlog_debug(OSPF6_DBG_INTERFACE, "Interface %s has no link local address, can't execute [InterfaceUp]",
                       oi->interface->name);

        return 0;
    }

    /* Recompute cost */
    ospf6_interface_recalculate_cost(oi);

    /* if already enabled, do nothing */
    if (oi->state > OSPF6_INTERFACE_DOWN)
    {
        if (IS_OSPF6_DEBUG_INTERFACE)
            zlog_debug(OSPF6_DBG_INTERFACE, "Interface %s already up",
                       oi->interface->name);

        return 0;
    }

    /* If no area assigned, return */
    if (oi->area == NULL)
    {
        zlog_debug(OSPF6_DBG_INTERFACE, "%s: Not scheduleing Hello for %s as there is no area assigned yet", __func__,
                   oi->interface->name);

        return 0;
    }
	oi->area->act_ints++;

    /* Join AllSPFRouters */
    //ospf6_sso (oi->interface->ifindex, &allspfrouters6, IPV6_JOIN_GROUP);
    /* Update interface route */
    ospf6_interface_connected_route_update(oi->interface);

    /* Schedule Hello */
    if (! CHECK_FLAG(oi->flag, OSPF6_INTERFACE_PASSIVE))
    {
        //oi->thread_send_hello = thread_add_event(master_ospf6, ospf6_hello_send, oi, 0);
		oi->thread_send_hello = high_pre_timer_add ((char *)"ospf6_hello_timer", 
					LIB_TIMER_TYPE_NOLOOP, ospf6_hello_send, (void *)oi, 10);
    }

    /* decide next interface state */
    if ((if_is_pointopoint(oi->interface)) ||
            (oi->type == OSPF_IFTYPE_POINTOPOINT))
    {
        ospf6_interface_state_change(OSPF6_INTERFACE_POINTTOPOINT, oi);
    }
	else if (oi->type == OSPF_IFTYPE_LOOPBACK)
	{
    
        ospf6_interface_state_change(OSPF6_INTERFACE_LOOPBACK, oi);
		//oi->t_wait = thread_add_timer(master_ospf6, wait_timer, oi, oi->dead_interval);
		oi->t_wait = high_pre_timer_add ((char *)"ospf6_wait_timer", 
					LIB_TIMER_TYPE_NOLOOP, wait_timer, (void *)oi, oi->dead_interval*1000);
		
	}
    else if (oi->priority == 0)
    {
        ospf6_interface_state_change(OSPF6_INTERFACE_DROTHER, oi);
    }
    else
    {
        ospf6_interface_state_change(OSPF6_INTERFACE_WAITING, oi);
        //oi->t_wait = thread_add_timer(master_ospf6, wait_timer, oi, oi->dead_interval);
		oi->t_wait = high_pre_timer_add ((char *)"ospf6_wait_timer", 
					LIB_TIMER_TYPE_NOLOOP, wait_timer, (void *)oi, oi->dead_interval*1000);
    }

    return 0;
}

int
wait_timer(void *thread)
{
    struct ospf6_interface *oi = NULL;
    oi = (struct ospf6_interface *)(thread);
    //assert(oi && oi->interface);
	if(!oi || !oi->interface)
	{
		zlog_err("%s[%d]: invalid argv!\n", __FUNCTION__, __LINE__);
		return 0;
	}
    oi->t_wait = 0;

    if (IS_OSPF6_DEBUG_INTERFACE)
        zlog_debug(OSPF6_DBG_INTERFACE, "%s[%d]:Interface Event %s: [WaitTimer]", __FUNCTION__, __LINE__,
                   oi->interface->name);

    if (oi->state == OSPF6_INTERFACE_WAITING)
    {
        ospf6_interface_state_change(dr_election(oi), oi);
    }

    return 0;
}

int
backup_seen(struct thread *thread)
{
    struct ospf6_interface *oi;
    oi = (struct ospf6_interface *) THREAD_ARG(thread);
    assert(oi && oi->interface);

    if (IS_OSPF6_DEBUG_INTERFACE)
        zlog_debug(OSPF6_DBG_INTERFACE, "Interface Event %s: [BackupSeen]",
                   oi->interface->name);

    if (oi->state == OSPF6_INTERFACE_WAITING)
    {
        ospf6_interface_state_change(dr_election(oi), oi);
    }

    return 0;
}

int
neighbor_change(struct thread *thread)
{
    struct ospf6_interface *oi = NULL;
    oi = (struct ospf6_interface *) THREAD_ARG(thread);
    assert(oi && oi->interface);

    if (IS_OSPF6_DEBUG_INTERFACE)
        zlog_debug(OSPF6_DBG_INTERFACE, "Interface Event %s: [NeighborChange]",
                   oi->interface->name);

    if (oi->state == OSPF6_INTERFACE_DROTHER ||
            oi->state == OSPF6_INTERFACE_BDR ||
            oi->state == OSPF6_INTERFACE_DR)
    {
        ospf6_interface_state_change(dr_election(oi), oi);
    }

    return 0;
}

int
interface_down(struct thread *thread)
{
    struct ospf6_interface *oi = NULL;
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf6_neighbor *on = NULL;
	
    oi = (struct ospf6_interface *) THREAD_ARG(thread);
    assert(oi && oi->interface);

    if (IS_OSPF6_DEBUG_INTERFACE)
        zlog_debug(OSPF6_DBG_INTERFACE, "Interface Event %s: [InterfaceDown]",
                   oi->interface->name);

	/* if already down, do nothing */
    if (oi->state == OSPF6_INTERFACE_DOWN)
    {
        if (IS_OSPF6_DEBUG_INTERFACE)
            zlog_debug(OSPF6_DBG_INTERFACE, "Interface %s already down",
                       oi->interface->name);

        return 0;
    }

    /* Stop Hellos */
    OSPF6_TIMER_OFF(oi->thread_send_hello);
    OSPF6_TIMER_OFF(oi->t_wait);
    /* Leave AllSPFRouters */
    //if (oi->state > OSPF6_INTERFACE_DOWN)
    //{
    //ospf6_sso (oi->interface->ifindex, &allspfrouters6, IPV6_LEAVE_GROUP);
    //}
    ospf6_interface_state_change(OSPF6_INTERFACE_DOWN, oi);
	oi->area->act_ints--;

    for (ALL_LIST_ELEMENTS(oi->neighbor_list, node, nnode, on))
    {
        ospf6_neighbor_delete(on);
		on = NULL;
    }

    list_delete_all_node(oi->neighbor_list);
    /* When interface state is reset, also reset information about
     * DR election, as it is no longer valid. */
    oi->drouter = oi->prev_drouter = htonl(0);
    oi->bdrouter = oi->prev_bdrouter = htonl(0);
    return 0;
}


/* show specified interface structure */
static int
ospf6_interface_show(struct vty *vty, struct interface *ifp)
{
    struct ospf6_interface *oi = NULL;
    struct connected *c = NULL;
    struct prefix *p = NULL;
    struct listnode *i = NULL;
    char strbuf[64], drouter[32], bdrouter[32];
    const char *updown[3] = {"down", "up", NULL};
    const char *type;
    struct timeval res, now;
	int remain_time = 0;
    char duration[32];
    struct ospf6_lsa *lsa = NULL;

    if (ifp->ifindex == 0)
        return 0;
#if 0
    /* check physical interface type */
    if (if_is_loopback(ifp))
    {
        type = "LOOPBACK";
    }
    else if (if_is_broadcast(ifp))
    {
        type = "BROADCAST";
    }
    else if (if_is_pointopoint(ifp))
    {
        type = "POINTOPOINT";
    }
    else
    {
        type = "UNKNOWN";
    }
#endif
    vty_out(vty, "%s is %s %s",
            ifp->name, updown[if_is_operative(ifp)],
            VNL);
    vty_out(vty, "  Interface ID: 0x%0x%s", ifp->ifindex, VNL);

    if (ifp->info == NULL)
    {
        vty_out(vty, "   OSPF6 not enabled on this interface%s", VNL);
        return 0;
    }
    else
    {
        oi = (struct ospf6_interface *) ifp->info;
    }

    vty_out(vty, "  Internet Address:%s", VNL);

    for (ALL_LIST_ELEMENTS_RO(ifp->connected, i, c))
    {
        p = c->address;
        prefix2str(p, strbuf, sizeof(strbuf));

        switch (p->family)
        {
            case AF_INET:
                vty_out(vty, "    inet : %s%s", strbuf,
                        VNL);
                break;
            case AF_INET6:
                vty_out(vty, "    inet6: %s%s", strbuf,
                        VNL);
                break;
            default:
                vty_out(vty, "    ???  : %s%s", strbuf,
                        VNL);
                break;
        }
    }

    if (oi->area)
    {
        inet_ntop(AF_INET, &oi->area->area_id,
          strbuf, sizeof(strbuf));
        vty_out(vty," OSPF6 Instance %d, Area %s, Instance ID %d %s",
            oi->ospf6->ospf_id, strbuf, oi->instance_id, VNL);
        
        vty_out(vty, "  Interface MTU %d (autodetect: %d)%s",
                oi->ifmtu, ifp->mtu6, VNL);
        vty_out(vty, "  MTU mismatch detection: %s, cost %u%s", oi->mtu_ignore ?
                "disabled" : "enabled", oi->cost, VNL);
    }
    else
    {
        vty_out(vty, "  Not Attached to Area%s", VNL);
    }

	if (oi->type == OSPF_IFTYPE_BROADCAST)
	{
		type = "BROADCAST";
	}
	else if(oi->type == OSPF_IFTYPE_POINTOPOINT)
	{
        type = "POINTOPOINT";
	}
	else if(oi->type == OSPF_IFTYPE_NBMA)
	{ 
		type = "NBMA";
	}
	else if(oi->type == OSPF_IFTYPE_POINTOMULTIPOINT)
	{
		type = "POINTOMULTIPOINT";
	}
	else if(oi->type == OSPF_IFTYPE_LOOPBACK)
	{
		type = "LOOPBACK";
	}
	else
	{
		type = "UNKNOWN";
	}
	
	vty_out(vty, "  Type %s", type);

    vty_out(vty, "  State %s, Transmit Delay %d sec, Priority %d%s",
            ospf6_interface_state_str[oi->state],
            oi->transdelay, oi->priority,
            VNL);
    vty_out(vty, "  Timer intervals configured:%s", VNL);
    vty_out(vty, "   Hello %d, Dead %d, Retransmit %d%s",
            oi->hello_interval, oi->dead_interval, oi->rxmt_interval,
            VNL);
	if (oi->type == OSPF_IFTYPE_BROADCAST)
	{
	    inet_ntop(AF_INET, &oi->drouter, drouter, sizeof(drouter));
	    inet_ntop(AF_INET, &oi->bdrouter, bdrouter, sizeof(bdrouter));
	    vty_out(vty, "  DR: %s BDR: %s%s", drouter, bdrouter, VNL);
	}
    vty_out(vty, "  Number of I/F scoped LSAs is %u%s",
            oi->lsdb->count, VNL);
    time_get_time(TIME_CLK_MONOTONIC, &now);
    timerclear(&res);

    if (oi->thread_send_lsupdate)
    {
        timersub(&oi->thread_send_lsupdate->u.sands, &now, &res);
    }

    timerstring(&res, duration, sizeof(duration));
    vty_out(vty, "    %d Pending LSAs for LSUpdate in Time %s [thread %s]%s",
            oi->lsupdate_list->count, duration,
            (oi->thread_send_lsupdate ? "on" : "off"),
            VNL);

    for (lsa = ospf6_lsdb_head(oi->lsupdate_list); lsa;
            lsa = ospf6_lsdb_next(lsa))
    {
        vty_out(vty, "      %s%s", lsa->name, VNL);
    }

    timerclear(&res);

    if (oi->thread_send_lsack)
    {
        //timersub(&oi->thread_send_lsack->u.sands, &now, &res);
        /* get the timer remain time*/
		high_pre_timer_remain(oi->thread_send_lsack, &remain_time);
		res.tv_sec = remain_time;
		res.tv_usec = 0;
    }
    
    timerstring(&res, duration, sizeof(duration));
    vty_out(vty, "    %d Pending LSAs for LSAck in Time %s [thread %s]%s",
            oi->lsack_list->count, duration,
            (oi->thread_send_lsack ? "on" : "off"),
            VNL);

    for (lsa = ospf6_lsdb_head(oi->lsack_list); lsa;
            lsa = ospf6_lsdb_next(lsa))
    {
        vty_out(vty, "      %s%s", lsa->name, VNL);
    }

    return 0;
}

static struct interface *
ospf6_find_interface(uint32_t ifindex)
{
    //struct interface *ifp = NULL;
    //vrf_id_t vrf_id = VRF_DEFAULT;
    struct ifm_event pevent;
	char ifname_tmp[INTERFACE_NAMSIZ];
    memset(&pevent, 0, sizeof(struct ifm_event));
    pevent.ifindex = ifindex;


    /* Read interface name. */
    ifm_get_name_by_ifindex(ifindex, ifname_tmp);

    /* And look it up. */
    return if_lookup_by_name_len(ifname_tmp,
                                 strnlen(ifname_tmp, INTERFACE_NAMSIZ));


}
static void
ospf6_interface_get(uint32_t ifindex, vrf_id_t vrf_id)
{
    struct ifm_l3  l3 = {0};
    struct ifm_l3  l3_unnum = {0};
    struct ifm_event pevent;
    memset(&pevent, 0, sizeof(struct ifm_event));
	uint8_t pflag = 1;
    uint8_t pdown_flag = 0;
    char ip_str[IPV6_ADDR_STRLEN];
	int flag = 0;

    /* get link */

    if(ifm_get_link(ifindex, MODULE_ID_OSPF6, &pdown_flag) != 0)
    {
        zlog_err("%-15s[%d]: OSPF6 get ifindex %0x link status time out", __FUNCTION__, __LINE__, ifindex);
        pflag = IFNET_LINKDOWN;
    }
    else
    {
        pflag = pdown_flag;
        zlog_debug(OSPF6_DBG_INTERFACE, "%-15s[%d]: OSPF6 get link ifindex %0x %d", __FUNCTION__, __LINE__, ifindex, pevent.up_flag);
    }

    if(ifm_get_l3if(ifindex, MODULE_ID_OSPF6, &l3) == 0)
    {   
    	/*if exit link local address*/
		if(IFP_IF_IPV6_EXIST(l3.ipv6_link_local.addr))
		{
			memset(&pevent, 0, sizeof(struct ifm_event));
			pevent.ifindex = ifindex;
			pevent.mode = IFNET_MODE_L3;
			pevent.ipaddr.type = INET_FAMILY_IPV6;
			pevent.ipflag =  IP_TYPE_LINK_LOCAL;
			pevent.up_flag = pflag;
			pevent.vpn= l3.vpn;
			flag = 1;

			IPV6_ADDR_COPY(pevent.ipaddr.addr.ipv6, l3.ipv6_link_local.addr);
			pevent.ipaddr.prefixlen = l3.ipv6_link_local.prefixlen;
			zlog_debug(OSPF6_DBG_INTERFACE, "%-15s[%d]: OSPF6 get ipv6  ifindex %0x:[%s],mask :%d", __func__, __LINE__, ifindex,
				   inet_ipv6tostr((struct ipv6_addr *)pevent.ipaddr.addr.ipv6, ip_str, IPV6_ADDR_STRLEN) ,
				   pevent.ipaddr.prefixlen);

			ospf6_zebra_if_address_update_add(pevent, vrf_id);
		}
		
        if (l3.ipv6_flag == IP_TYPE_UNNUMBERED)
        {
            if(ifm_get_l3if(l3.unnumbered_if, MODULE_ID_OSPF6, &l3_unnum) == 0)
            {

                if (IFP_IF_IPV6_EXIST(l3_unnum.ipv6[0].addr))
                {
                    pevent.ifindex = ifindex;
                    pevent.mode = IFNET_MODE_L3;
					pevent.ipaddr.type = INET_FAMILY_IPV6;                    
					IPV6_ADDR_COPY(pevent.ipaddr.addr.ipv6, l3_unnum.ipv6[0].addr);
                    pevent.ipaddr.prefixlen = l3_unnum.ipv6[0].prefixlen;

                    pevent.ipflag = l3.ipv6_flag;
                    pevent.up_flag = pflag;
					pevent.vpn= l3.vpn;
                    zlog_debug(OSPF6_DBG_INTERFACE, "%-15s[%d]: OSPF6 get ipv6  ifindex %0x:[%s],mask :%d", __func__, __LINE__, ifindex,
                               inet_ipv6tostr((struct ipv6_addr *)pevent.ipaddr.addr.ipv6, ip_str, IPV6_ADDR_STRLEN) ,
                               pevent.ipaddr.prefixlen);
                    ospf6_zebra_if_address_update_add(pevent, vrf_id);

					
					

                    if (IFP_IF_IPV6_EXIST(l3_unnum.ipv6[1].addr))
                    {
                        memset(&pevent, 0, sizeof(struct ifm_event));
                        pevent.ifindex = ifindex;
                        IPV6_ADDR_COPY(pevent.ipaddr.addr.ipv6, l3_unnum.ipv6[1].addr);
                        pevent.ipaddr.prefixlen = l3_unnum.ipv6[1].prefixlen;
                        pevent.ipflag = IP_TYPE_SLAVE;
                        pevent.up_flag = pflag;
						pevent.vpn= l3.vpn;
                        ospf6_zebra_if_address_update_add(pevent, vrf_id);
                    }
					return ;
                }
				
            }
        }
        else
        {			
            if (IFP_IF_IPV6_EXIST(l3.ipv6[0].addr))
            {            	
                pevent.ifindex = ifindex;
                pevent.mode = IFNET_MODE_L3;
                pevent.ipaddr.type = INET_FAMILY_IPV6;

                IPV6_ADDR_COPY(pevent.ipaddr.addr.ipv6, l3.ipv6[0].addr);
                pevent.ipaddr.prefixlen = l3.ipv6[0].prefixlen;
                pevent.up_flag = pflag;
                pevent.ipflag = l3.ipv6_flag;
				pevent.vpn= l3.vpn;
				
                zlog_debug(OSPF6_DBG_INTERFACE, "%-15s[%d]: OSPF6 get ipv6  ifindex %0x:[%s],mask :%d", __func__, __LINE__, ifindex,
                           inet_ipv6tostr((struct ipv6_addr *)pevent.ipaddr.addr.ipv6, ip_str, IPV6_ADDR_STRLEN) ,
                           pevent.ipaddr.prefixlen);

                ospf6_zebra_if_address_update_add(pevent, vrf_id);


                if (IFP_IF_IPV6_EXIST(l3.ipv6[1].addr))
                {
                    memset(&pevent, 0, sizeof(struct ifm_event));
                    pevent.ifindex = ifindex;
                    pevent.ipaddr.type = INET_FAMILY_IPV6;
                    IPV6_ADDR_COPY(pevent.ipaddr.addr.ipv6, l3.ipv6[1].addr);
                    pevent.ipaddr.prefixlen = l3.ipv6[1].prefixlen;

                    pevent.ipflag = IP_TYPE_SLAVE;
                    pevent.up_flag = pflag;
					pevent.vpn= l3.vpn;
                    ospf6_zebra_if_address_update_add(pevent, vrf_id);
                }
				return;
            }
			

			
        }
		if(flag == 1)
			return;

    }
	
    memset(&pevent, 0, sizeof(struct ifm_event));
	pevent.ifindex = ifindex;
	pevent.mode = IFNET_MODE_L3;
	pevent.ipaddr.type = INET_FAMILY_IPV6;
	pevent.up_flag = pflag;
	ospf6_if_add(pevent, vrf_id);
    ospf6_zebra_if_state_update(pevent, vrf_id);

        
}


static struct interface *
ospf6_enable_interface_init(uint32_t ifindex)
{
    struct interface *ifp = NULL;
    vrf_id_t vrf_id = VRF_DEFAULT;

    /*according to the interface name look up the struct pointer ifp */
    ifp = if_lookup_by_index_vrf(ifindex, vrf_id);

    if (ifp)/*if the if exit,first must delete the address*/
    {
        struct connected *ifc = NULL;
        struct listnode *node = NULL;
        struct listnode *next = NULL;

        for (node = listhead(ifp->connected); node; node = next)
        {
            ifc = listgetdata(node);
            next = node->next;

            if (ifc != NULL)
            {
                connected_delete_by_prefix(ifp, ifc->address);
            }
        }
    }

    /*if the interface exist,reget the ifp*/
    ospf6_interface_get(ifindex, vrf_id);

    if (ifp == NULL)
    {
    	ifp = ospf6_find_interface(ifindex);
    }

    return ifp;
}

static int
ospf6_str2area_id (const char *str, u_int32_t *area_id)
{
	char *endptr = NULL;
	unsigned long ret;

	/* match "A.B.C.D". */
	if (strchr (str, '.') != NULL)
	{
		if(inet_pton(AF_INET, str, area_id) != 1)
		{
			return -1;
		}
		
		//*area_id = ntohl(*area_id);
		/*ret = inet_aton (str, area_id);
		if (!ret)
		return -1;
		*/
	}
	/* match "<0-4294967295>". */
	else
	{	
		if (*str == '-')
			return -1;
		
		errno = 0;
		ret = strtoul (str, &endptr, 10);
		if (*endptr != '\0' || errno || ret > UINT32_MAX)
			return -1;

		*area_id = htonl(ret);
	}

  return 0;
}



/* show interface */
DEFUN(show_ipv6_ospf6_interface,
      show_ipv6_ospf6_interface_cmd,
      "show ipv6 ospf6 [<1-255>] interface",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      OSPF6_INSTANCE
      INTERFACE_STR
      "Ethernet interface type\n")
{
    struct interface *ifp = NULL;
    struct listnode *i = NULL;
    struct ospf6 *o = NULL;
    struct ospf6_interface *oi = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;
    uint32_t ifindex = 0;
    int ret = -1;
    char if_name[IFNET_NAMESIZE];

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf6 instance", ospf_id, argv[0],
                              OSPF6_DEFAULT_PROCESS_ID, OSPF6_MAX_PROCESS_ID);
    }

    o = ospf6_lookup_id(ospf_id);

    if (o == NULL)
    {
        VTY_OSPF6_INSTANCE_ERR;
    }

    if (argc == 3)
    {
       
        ifindex = ifm_get_ifindex_by_name((char *)argv[1], (char *) argv[2]);
       

        if (ifindex == 0)
        {
            vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        /*get the name of the interface*/
        ret = ifm_get_name_by_ifindex(ifindex , if_name);

        if (ret < 0)
        {
            vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        ifp = if_lookup_by_name(if_name);

        if (ifp == NULL)
        {
            vty_error_out(vty, "No such Interface: %s",
                    VNL);
            return CMD_WARNING;
        }

        if (ifp->info == NULL)
        {
            vty_error_out(vty, "OSPF6 not enabled on this interface%s", VNL);
            return CMD_WARNING;
        }
        else
        {
            oi = (struct ospf6_interface *) ifp->info;

            if (oi->ospf6 != o)
            {
                vty_error_out(vty, "OSPF6 not enabled on this interface%s", VNL);
				return CMD_WARNING;
            }
        }

        ospf6_interface_show(vty, ifp);
    }
    else
    {
        for (ALL_LIST_ELEMENTS_RO(iflist, i, ifp))
        {
            if (ifp->info == NULL)
            {
                continue;
            }
            else
            {
                oi = (struct ospf6_interface *) ifp->info;

                if (oi->ospf6 != o)
                    continue;
            }

            ospf6_interface_show(vty, ifp);
        }
    }

    return CMD_SUCCESS;
}

ALIAS (show_ipv6_ospf6_interface,
       show_ipv6_ospf6_interface_ifname_cmd,
       "show ipv6 ospf6 [<1-255>] interface (ethernet |gigabitethernet |xgigabitethernet ) USP",
       SHOW_STR
       IP6_STR
       OSPF6_STR
       OSPF6_INSTANCE
       INTERFACE_STR
       "Ethernet interface type\n"
       "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
       CLI_INTERFACE_GIGABIT_ETHERNET_STR
       CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
       CLI_INTERFACE_XGIGABIT_ETHERNET_STR
       CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)

DEFUN(show_ipv6_ospf6_interface_trunk,
	  show_ipv6_ospf6_interface_trunk_cmd, 
	  "show ipv6 ospf6 [<1-255>] interface trunk TRUNK",
	  SHOW_STR
	  IP6_STR
	  OSPF6_STR
	  OSPF6_INSTANCE
	  INTERFACE_STR
	  CLI_INTERFACE_TRUNK_STR
      CLI_INTERFACE_TRUNK_VHELP_STR)
{
    struct interface *ifp = NULL;
    struct ospf6 *o = NULL;
    struct ospf6_interface *oi = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;
    uint32_t ifindex = 0;
    int ret = -1;
    char if_name[IFNET_NAMESIZE];

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf6 instance", ospf_id, argv[0],
                              OSPF6_DEFAULT_PROCESS_ID, OSPF6_MAX_PROCESS_ID);
    }

    o = ospf6_lookup_id(ospf_id);

    if (o == NULL)
    {
        VTY_OSPF6_INSTANCE_ERR;
    }

       
    ifindex = ifm_get_ifindex_by_name((char *)"trunk", (char *) argv[1]);
   

    if (ifindex == 0)
    {
        vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get the name of the interface*/
    ret = ifm_get_name_by_ifindex(ifindex , if_name);

    if (ret < 0)
    {
        vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ifp = if_lookup_by_name(if_name);

    if (ifp == NULL)
    {
        vty_error_out(vty, "No such Interface: %s",
                VNL);
        return CMD_WARNING;
    }

    if (ifp->info == NULL)
    {
        vty_error_out(vty, "OSPF6 not enabled on this interface%s", VNL);
        return CMD_WARNING;
    }
    else
    {
        oi = (struct ospf6_interface *) ifp->info;

        if (oi->ospf6 != o)
        {
            vty_error_out(vty, "OSPF6 not enabled on this interface%s", VNL);
			return CMD_WARNING;
        }
    }

    ospf6_interface_show(vty, ifp);
    

    return CMD_SUCCESS;
}

DEFUN(show_ipv6_ospf6_interface_vlanif,
	  show_ipv6_ospf6_interface_vlanif_cmd, 
	  "show ipv6 ospf6 [<1-255>] interface vlanif <1-4094>",
	  SHOW_STR
	  IP6_STR
	  OSPF6_STR
	  OSPF6_INSTANCE
	  INTERFACE_STR       
	  CLI_INTERFACE_VLANIF_STR
      CLI_INTERFACE_VLANIF_VHELP_STR)
{
    struct interface *ifp = NULL;
    struct ospf6 *o = NULL;
    struct ospf6_interface *oi = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;
    uint32_t ifindex = 0;
    int ret = -1;
    char if_name[IFNET_NAMESIZE];

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf6 instance", ospf_id, argv[0],
                              OSPF6_DEFAULT_PROCESS_ID, OSPF6_MAX_PROCESS_ID);
    }

    o = ospf6_lookup_id(ospf_id);

    if (o == NULL)
    {
        VTY_OSPF6_INSTANCE_ERR;
    }

       
    ifindex = ifm_get_ifindex_by_name((char *)"vlanif", (char *) argv[1]);
   

    if (ifindex == 0)
    {
        vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get the name of the interface*/
    ret = ifm_get_name_by_ifindex(ifindex , if_name);

    if (ret < 0)
    {
        vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ifp = if_lookup_by_name(if_name);

    if (ifp == NULL)
    {
        vty_error_out(vty, "No such Interface: %s",
                VNL);
        return CMD_WARNING;
    }

    if (ifp->info == NULL)
    {
        vty_error_out(vty, "OSPF6 not enabled on this interface%s", VNL);
        return CMD_WARNING;
    }
    else
    {
        oi = (struct ospf6_interface *) ifp->info;

        if (oi->ospf6 != o)
        {
            vty_error_out(vty, "OSPF6 not enabled on this interface%s", VNL);
			return CMD_WARNING;
        }
    }

    ospf6_interface_show(vty, ifp);

	return CMD_SUCCESS;
}

DEFUN(show_ipv6_ospf6_interface_loopback,
	  show_ipv6_ospf6_interface_loopback_cmd, 
	  "show ipv6 ospf6 [<1-255>] interface loopback <0-128>",
	  SHOW_STR
	  IP6_STR
	  OSPF6_STR
	  OSPF6_INSTANCE
	  INTERFACE_STR
	  CLI_INTERFACE_LOOPBACK_STR
      CLI_INTERFACE_LOOPBACK_VHELP_STR)
{
    struct interface *ifp = NULL;
    struct ospf6 *o = NULL;
    struct ospf6_interface *oi = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;
    uint32_t ifindex = 0;
    int ret = -1;
    char if_name[IFNET_NAMESIZE];

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf6 instance", ospf_id, argv[0],
                              OSPF6_DEFAULT_PROCESS_ID, OSPF6_MAX_PROCESS_ID);
    }

    o = ospf6_lookup_id(ospf_id);

    if (o == NULL)
    {
        VTY_OSPF6_INSTANCE_ERR;
    }

       
    ifindex = ifm_get_ifindex_by_name((char *)"loopback", (char *) argv[1]);
   

    if (ifindex == 0)
    {
        vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get the name of the interface*/
    ret = ifm_get_name_by_ifindex(ifindex , if_name);

    if (ret < 0)
    {
        vty_error_out(vty, "Failed to get interface information.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ifp = if_lookup_by_name(if_name);

    if (ifp == NULL)
    {
        vty_error_out(vty, "No such Interface: %s",
                VNL);
        return CMD_WARNING;
    }

    if (ifp->info == NULL)
    {
        vty_error_out(vty, "OSPF6 not enabled on this interface%s", VNL);
        return CMD_WARNING;
    }
    else
    {
        oi = (struct ospf6_interface *) ifp->info;

        if (oi->ospf6 != o)
        {
            vty_error_out(vty, "OSPF6 not enabled on this interface%s", VNL);
			return CMD_WARNING;
        }
    }

    ospf6_interface_show(vty, ifp);

	return CMD_SUCCESS;
}




DEFUN(show_ipv6_ospf6_interface_ifname_prefix,
      show_ipv6_ospf6_interface_ifname_prefix_cmd,
      "show ipv6 ospf6 interface IFNAME prefix",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      INTERFACE_STR
      IFNAME_STR
      "Display connected prefixes to advertise\n"
     )
{
    struct interface *ifp;
    struct ospf6_interface *oi;
    ifp = if_lookup_by_name(argv[0]);

    if (ifp == NULL)
    {
        vty_out(vty, "No such Interface: %s%s", argv[0], VNL);
        return CMD_WARNING;
    }

    oi = ifp->info;

    if (oi == NULL)
    {
        vty_error_out(vty, "OSPF6 is not enabled on %s%s", argv[0], VNL);
        return CMD_WARNING;
    }

    argc--;
    argv++;
    ospf6_route_table_show(vty, argc, argv, oi->route_connected);
    return CMD_SUCCESS;
}

ALIAS(show_ipv6_ospf6_interface_ifname_prefix,
      show_ipv6_ospf6_interface_ifname_prefix_detail_cmd,
      "show ipv6 ospf6 interface IFNAME prefix (X:X::X:X|X:X::X:X/M|detail)",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      INTERFACE_STR
      IFNAME_STR
      "Display connected prefixes to advertise\n"
      OSPF6_ROUTE_ADDRESS_STR
      OSPF6_ROUTE_PREFIX_STR
      "Display details of the prefixes\n"
     )

ALIAS(show_ipv6_ospf6_interface_ifname_prefix,
      show_ipv6_ospf6_interface_ifname_prefix_match_cmd,
      "show ipv6 ospf6 interface IFNAME prefix X:X::X:X/M (match|detail)",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      INTERFACE_STR
      IFNAME_STR
      "Display connected prefixes to advertise\n"
      OSPF6_ROUTE_PREFIX_STR
      OSPF6_ROUTE_MATCH_STR
      "Display details of the prefixes\n"
     )

DEFUN(show_ipv6_ospf6_interface_prefix,
      show_ipv6_ospf6_interface_prefix_cmd,
      "show ipv6 ospf6 interface prefix",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      INTERFACE_STR
      "Display connected prefixes to advertise\n"
     )
{
    struct listnode *i;
    struct ospf6_interface *oi;
    struct interface *ifp;

    for (ALL_LIST_ELEMENTS_RO(iflist, i, ifp))
    {
        oi = (struct ospf6_interface *) ifp->info;

        if (oi == NULL)
        {
            continue;
        }

        ospf6_route_table_show(vty, argc, argv, oi->route_connected);
    }

    return CMD_SUCCESS;
}

ALIAS(show_ipv6_ospf6_interface_prefix,
      show_ipv6_ospf6_interface_prefix_detail_cmd,
      "show ipv6 ospf6 interface prefix (X:X::X:X|X:X::X:X/M|detail)",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      INTERFACE_STR
      "Display connected prefixes to advertise\n"
      OSPF6_ROUTE_ADDRESS_STR
      OSPF6_ROUTE_PREFIX_STR
      "Display details of the prefixes\n"
     )

ALIAS(show_ipv6_ospf6_interface_prefix,
      show_ipv6_ospf6_interface_prefix_match_cmd,
      "show ipv6 ospf6 interface prefix X:X::X:X/M (match|detail)",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      INTERFACE_STR
      "Display connected prefixes to advertise\n"
      OSPF6_ROUTE_PREFIX_STR
      OSPF6_ROUTE_MATCH_STR
      "Display details of the prefixes\n"
     )

/* interface variable set command */
DEFUN(ipv6_ospf6_ifmtu,
      ipv6_ospf6_ifmtu_cmd,
      "ipv6 ospf6 ifmtu <1-65535>",
      IP6_STR
      OSPF6_STR
      "Interface MTU\n"
      "OSPF6 Interface MTU\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    unsigned int ifmtu, iobuflen;
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf6_neighbor *on = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    ifmtu = strtol(argv[0], NULL, 10);

    if (oi->ifmtu == ifmtu)
    {
        return CMD_SUCCESS;
    }

    if (ifp->mtu6 != 0 && ifp->mtu6 < ifmtu)
    {
        vty_error_out(vty, "%s's ospf6 ifmtu cannot go beyond physical mtu (%d)%s",
                ifp->name, ifp->mtu6, VNL);
        return CMD_WARNING;
    }

    if (oi->ifmtu < ifmtu)
    {
        iobuflen = ospf6_iobuf_size(ifmtu);

        if (iobuflen < ifmtu)
        {
            vty_error_out(vty, "%s's ifmtu is adjusted to I/O buffer size (%d).%s",
                    ifp->name, iobuflen, VNL);
            oi->ifmtu = iobuflen;
        }
        else
        {
            oi->ifmtu = ifmtu;
        }
    }
    else
    {
        oi->ifmtu = ifmtu;
    }

    /* re-establish adjacencies */
    for (ALL_LIST_ELEMENTS(oi->neighbor_list, node, nnode, on))
    {
        OSPF6_TIMER_OFF(on->inactivity_timer);
        /*thread_execute(master_ospf6, inactivity_timer, on, 0);*/
		high_pre_timer_add ((char *)"ospf6_inact_timer", LIB_TIMER_TYPE_NOLOOP,\
						inactivity_timer, (void *)on, 10);
    }

    return CMD_SUCCESS;
}

DEFUN(no_ipv6_ospf6_ifmtu,
      no_ipv6_ospf6_ifmtu_cmd,
      "no ipv6 ospf6 ifmtu",
      NO_STR
      IP6_STR
      OSPF6_STR
      "Interface MTU\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    unsigned int iobuflen;
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf6_neighbor *on = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    if (oi->ifmtu < ifp->mtu)
    {
        iobuflen = ospf6_iobuf_size(ifp->mtu);

        if (iobuflen < ifp->mtu)
        {
            vty_error_out(vty, "%s's ifmtu is adjusted to I/O buffer size (%d).%s",
                    ifp->name, iobuflen, VNL);
            oi->ifmtu = iobuflen;
        }
        else
        {
            oi->ifmtu = ifp->mtu;
        }
    }
    else
    {
        oi->ifmtu = ifp->mtu;
    }

    /* re-establish adjacencies */
    for (ALL_LIST_ELEMENTS(oi->neighbor_list, node, nnode, on))
    {
        OSPF6_TIMER_OFF(on->inactivity_timer);
        /*thread_execute(master_ospf6, inactivity_timer, on, 0);*/
		high_pre_timer_add ((char *)"ospf6_inact_timer", LIB_TIMER_TYPE_NOLOOP,\
						inactivity_timer, (void *)on, 10);
    }

    return CMD_SUCCESS;
}

DEFUN(ipv6_ospf6_cost,
      ipv6_ospf6_cost_cmd,
      "ipv6 ospf6 cost <1-65535>",
      IP6_STR
      OSPF6_STR
      "Interface cost\n"
      "Outgoing metric of this interface\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    unsigned long int lcost;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);

    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    lcost = strtol(argv[0], NULL, 10);

    if (lcost > UINT32_MAX)
    {
        vty_warning_out(vty, "Cost %ld is out of range%s", lcost, VNL);
        return CMD_WARNING;
    }

    if (oi->cost == lcost)
    {
        return CMD_SUCCESS;
    }

    oi->cost = lcost;
    SET_FLAG(oi->flag, OSPF6_INTERFACE_NOAUTOCOST);
    //ospf6_interface_recalculate_cost(oi);
    ospf6_interface_set_cost(oi);
    return CMD_SUCCESS;
}

DEFUN(no_ipv6_ospf6_cost,
      no_ipv6_ospf6_cost_cmd,
      "no ipv6 ospf6 cost",
      NO_STR
      IP6_STR
      OSPF6_STR
      "Calculate interface cost from bandwidth\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    UNSET_FLAG(oi->flag, OSPF6_INTERFACE_NOAUTOCOST);
    ospf6_interface_recalculate_cost(oi);
    return CMD_SUCCESS;
}

DEFUN(auto_cost_reference_bandwidth,
      auto_cost_reference_bandwidth_cmd,
      "auto-cost reference-bandwidth <1-4294967>",
      "Calculate OSPF interface cost according to bandwidth\n"
      "Use reference bandwidth method to assign OSPF cost\n"
      "The reference bandwidth in terms of Mbits per second\n")
{
    struct ospf6 *o = vty->index;
    struct ospf6_area *oa = NULL;
    struct ospf6_interface *oi = NULL;
    struct listnode *i = NULL, *j = NULL;
    u_int32_t refbw;

    refbw = strtol(argv[0], NULL, 10);

    if (refbw < 1 || refbw > 4294967)
    {
        vty_error_out(vty, "reference-bandwidth value is invalid%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* If reference bandwidth is changed. */
    if ((refbw * 1000) == o->ref_bandwidth)
    {
        return CMD_SUCCESS;
    }

    o->ref_bandwidth = refbw * 1000;

    for (ALL_LIST_ELEMENTS_RO(o->area_list, i, oa))
    {
        for (ALL_LIST_ELEMENTS_RO(oa->if_list, j, oi))
        {
            ospf6_interface_recalculate_cost(oi);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(no_auto_cost_reference_bandwidth,
      no_auto_cost_reference_bandwidth_cmd,
      "no auto-cost reference-bandwidth",
      NO_STR
      "Calculate OSPF interface cost according to bandwidth\n"
      "Use reference bandwidth method to assign OSPF cost\n")
{
    struct ospf6 *o = vty->index;
    struct ospf6_area *oa;
    struct ospf6_interface *oi;
    struct listnode *i, *j;

    if (o->ref_bandwidth == OSPF6_REFERENCE_BANDWIDTH)
    {
        return CMD_SUCCESS;
    }

    o->ref_bandwidth = OSPF6_REFERENCE_BANDWIDTH;

    for (ALL_LIST_ELEMENTS_RO(o->area_list, i, oa))
    {
        for (ALL_LIST_ELEMENTS_RO(oa->if_list, j, oi))
        {
            ospf6_interface_recalculate_cost(oi);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(ipv6_ospf6_hellointerval,
      ipv6_ospf6_hellointerval_cmd,
      "ipv6 ospf6 hello-interval <1-65535>",
      IP6_STR
      OSPF6_STR
      "Interval time of Hello packets\n"
      SECONDS_STR
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    u_int16_t hello_interval = 0;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    hello_interval = strtol(argv[0], NULL, 10);

    if (oi->hello_interval == hello_interval)
    {
        return CMD_SUCCESS;
    }

    if (oi->hello_interval*4 == oi->dead_interval)
    {
        oi->dead_interval = 4 * hello_interval;
		OSPF6_TIMER_OFF(oi->t_wait);
        //oi->t_wait = thread_add_timer(master_ospf6, wait_timer, oi, oi->dead_interval);
		oi->t_wait = high_pre_timer_add ((char *)"ospf6_wait_timer", 
					LIB_TIMER_TYPE_NOLOOP, wait_timer, (void *)oi, oi->dead_interval*1000);
        zlog_debug(OSPF6_DBG_OTHER, "%s %d dead interval is not configed: dead interval change to 4 times of hello \n",
            __func__, __LINE__);
    }
    else if (hello_interval > oi->dead_interval)
    {
        oi->dead_interval = 4 * hello_interval;
		OSPF6_TIMER_OFF(oi->t_wait);
        //oi->t_wait = thread_add_timer(master_ospf6, wait_timer, oi, oi->dead_interval);
		oi->t_wait = high_pre_timer_add ((char *)"ospf6_wait_timer", 
					LIB_TIMER_TYPE_NOLOOP, wait_timer, (void *)oi, oi->dead_interval*1000);
        vty_info_out(vty, "Hello interval is greater than Dead interval,"
                "now Dead interval is forced to adjust to 4 times of new Hello interval. %s", VTY_NEWLINE);
    }

    oi->hello_interval = hello_interval;
	
	OSPF6_TIMER_OFF(oi->thread_send_hello);
	//oi->thread_send_hello = thread_add_event(master_ospf6, ospf6_hello_send, oi, 0);
	oi->thread_send_hello = high_pre_timer_add ((char *)"ospf6_hello_timer", 
					LIB_TIMER_TYPE_NOLOOP, ospf6_hello_send, (void *)oi, 10);
    return CMD_SUCCESS;
}

DEFUN(no_ipv6_ospf6_hellointerval,
      no_ipv6_ospf6_hellointerval_cmd,
      "no ipv6 ospf6 hello-interval",
      NO_STR
      IP6_STR
      OSPF6_STR
      "Interval time of Hello packets\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    if (oi->hello_interval == OSPF6_INTERFACE_HELLO_INTERVAL)
    {
        return CMD_SUCCESS;
    }

    if (oi->hello_interval*4 == oi->dead_interval)
    {
        oi->dead_interval = OSPF6_INTERFACE_DEAD_INTERVAL;
        zlog_debug(OSPF6_DBG_OTHER, "%s %d dead interval is not configed: dead interval change to 4 times of hello \n", __func__, __LINE__);
    }

    oi->hello_interval = OSPF6_INTERFACE_HELLO_INTERVAL;


    return CMD_SUCCESS;

}


/* interface variable set command */
DEFUN(ipv6_ospf6_deadinterval,
      ipv6_ospf6_deadinterval_cmd,
      "ipv6 ospf6 dead-interval <1-65535>",
      IP6_STR
      OSPF6_STR
      "Interval time after which a neighbor is declared down\n"
      SECONDS_STR
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    u_int16_t dead_interval = 0;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    dead_interval = strtol(argv[0], NULL, 10);

    if (oi->dead_interval == dead_interval)
    {
        return CMD_SUCCESS;
    }

    if (dead_interval < oi->hello_interval)
    {
        vty_error_out(vty, "The dead interval must greater than hello interval%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }

    oi->dead_interval = dead_interval;
    return CMD_SUCCESS;
}

DEFUN(no_ipv6_ospf6_deadinterval,
      no_ipv6_ospf6_deadinterval_cmd,
      "no ipv6 ospf6 dead-interval",
      NO_STR
      IP6_STR
      OSPF6_STR
      "Interval time after which a neighbor is declared down\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    if (oi->dead_interval == OSPF6_INTERFACE_DEAD_INTERVAL)
    {
        return CMD_SUCCESS;
    }

    oi->dead_interval = OSPF6_INTERFACE_DEAD_INTERVAL;
    return CMD_SUCCESS;

}


/* interface variable set command */
DEFUN(ipv6_ospf6_transmitdelay,
      ipv6_ospf6_transmitdelay_cmd,
      "ipv6 ospf6 transmit-delay <1-3600>",
      IP6_STR
      OSPF6_STR
      "Transmit delay of this interface\n"
      SECONDS_STR
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    oi->transdelay = strtol(argv[0], NULL, 10);
    return CMD_SUCCESS;
}

/* interface variable set command */
DEFUN(ipv6_ospf6_retransmitinterval,
      ipv6_ospf6_retransmitinterval_cmd,
      "ipv6 ospf6 retransmit-interval <1-65535>",
      IP6_STR
      OSPF6_STR
      "Time between retransmitting lost link state advertisements\n"
      SECONDS_STR
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    oi->rxmt_interval = strtol(argv[0], NULL, 10);
    return CMD_SUCCESS;
}

/* interface variable set command */
DEFUN(ipv6_ospf6_priority,
      ipv6_ospf6_priority_cmd,
      "ipv6 ospf6 priority <0-255>",
      IP6_STR
      OSPF6_STR
      "Router priority\n"
      "Priority value\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    u_char priority = 0;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    priority = strtol(argv[0], NULL, 10);

    if (priority == oi->priority)
    {
        return CMD_SUCCESS;
    }

    oi->priority = priority;

    if (oi->area &&
            (oi->state == OSPF6_INTERFACE_DROTHER ||
             oi->state == OSPF6_INTERFACE_BDR ||
             oi->state == OSPF6_INTERFACE_DR))
    {
        ospf6_interface_state_change(dr_election(oi), oi);
    }

    return CMD_SUCCESS;
}

DEFUN(no_ipv6_ospf6_priority,
      no_ipv6_ospf6_priority_cmd,
      "no ipv6 ospf6 priority",
      NO_STR
      IP6_STR
      OSPF6_STR
      "Router priority\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    if (oi->priority == OSPF6_INTERFACE_PRIORITY)
    {
        return CMD_SUCCESS;
    }

    oi->priority = OSPF6_INTERFACE_PRIORITY;

    if (oi->area &&
            (oi->state == OSPF6_INTERFACE_DROTHER ||
             oi->state == OSPF6_INTERFACE_BDR ||
             oi->state == OSPF6_INTERFACE_DR))
    {
        ospf6_interface_state_change(dr_election(oi), oi);
    }

    return CMD_SUCCESS;
}


DEFUN(ipv6_ospf6_instance,
      ipv6_ospf6_instance_cmd,
      "ipv6 ospf6 instance-id <0-255>",
      IP6_STR
      OSPF6_STR
      "Instance ID for this interface\n"
      "Instance ID value\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *)ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    oi->instance_id = strtol(argv[0], NULL, 10);
    return CMD_SUCCESS;
}

DEFUN(no_ipv6_ospf6_instance,
      no_ipv6_ospf6_instance_cmd,
      "no ipv6 ospf6 instance-id",
      NO_STR
      IP6_STR
      OSPF6_STR
      "Instance ID for this interface\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *)ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    oi->instance_id = OSPF6_INTERFACE_INSTANCE_ID;
    return CMD_SUCCESS;
}


DEFUN(ipv6_ospf6_passive,
      ipv6_ospf6_passive_cmd,
      "ipv6 ospf6 passive",
      IP6_STR
      OSPF6_STR
      "passive interface, No adjacency will be formed on this interface\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf6_neighbor *on = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    SET_FLAG(oi->flag, OSPF6_INTERFACE_PASSIVE);
    OSPF6_TIMER_OFF(oi->thread_send_hello);
    OSPF6_TIMER_OFF(oi->t_wait);

    for (ALL_LIST_ELEMENTS(oi->neighbor_list, node, nnode, on))
    {
        OSPF6_TIMER_OFF(on->inactivity_timer);
        /*thread_execute(master_ospf6, inactivity_timer, on, 0);*/
		high_pre_timer_add ((char *)"ospf6_inact_timer", LIB_TIMER_TYPE_NOLOOP,\
						inactivity_timer, (void *)on, 10);
    }

    return CMD_SUCCESS;
}

DEFUN(no_ipv6_ospf6_passive,
      no_ipv6_ospf6_passive_cmd,
      "no ipv6 ospf6 passive",
      NO_STR
      IP6_STR
      OSPF6_STR
      "passive interface: No Adjacency will be formed on this I/F\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);

    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }


    UNSET_FLAG(oi->flag, OSPF6_INTERFACE_PASSIVE);
    OSPF6_TIMER_OFF(oi->thread_send_hello);
    OSPF6_TIMER_OFF(oi->t_wait);
    /*oi->thread_send_hello =
        thread_add_event(master_ospf6, ospf6_hello_send, oi, 0);*/
	oi->thread_send_hello = high_pre_timer_add ((char *)"ospf6_hello_timer", 
					LIB_TIMER_TYPE_NOLOOP, ospf6_hello_send, (void *)oi, 10);
	
	//oi->t_wait = thread_add_timer(master_ospf6, wait_timer, oi, oi->dead_interval);
	oi->t_wait = high_pre_timer_add ((char *)"ospf6_wait_timer", 
					LIB_TIMER_TYPE_NOLOOP, wait_timer, (void *)oi, oi->dead_interval*1000);
    return CMD_SUCCESS;
}

DEFUN(ipv6_ospf6_mtu_ignore,
      ipv6_ospf6_mtu_ignore_cmd,
      "ipv6 ospf6 mtu-ignore",
      IP6_STR
      OSPF6_STR
      "Ignore MTU mismatch on this interface\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }


    oi->mtu_ignore = 1;
    return CMD_SUCCESS;
}

DEFUN(no_ipv6_ospf6_mtu_ignore,
      no_ipv6_ospf6_mtu_ignore_cmd,
      "no ipv6 ospf6 mtu-ignore",
      NO_STR
      IP6_STR
      OSPF6_STR
      "Ignore MTU mismatch on this interface\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    oi->mtu_ignore = 0;
    return CMD_SUCCESS;
}

DEFUN(ipv6_ospf6_advertise_prefix_list,
      ipv6_ospf6_advertise_prefix_list_cmd,
      "ipv6 ospf6 advertise prefix-list WORD",
      IP6_STR
      OSPF6_STR
      "Advertising options\n"
      "Filter prefix using prefix-list\n"
      "Prefix list name\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->plist_name)
    {
        XFREE(MTYPE_PREFIX_LIST_STR, oi->plist_name);
    }

    oi->plist_name = XSTRDUP(MTYPE_PREFIX_LIST_STR, argv[0]);
    ospf6_interface_connected_route_update(oi->interface);

    if (oi->area)
    {
        OSPF6_LINK_LSA_SCHEDULE(oi);

        if (oi->state == OSPF6_INTERFACE_DR)
        {
            OSPF6_NETWORK_LSA_SCHEDULE(oi);
            OSPF6_INTRA_PREFIX_LSA_SCHEDULE_TRANSIT(oi);
        }

        OSPF6_INTRA_PREFIX_LSA_SCHEDULE_STUB(oi->area);
    }

    return CMD_SUCCESS;
}

DEFUN(no_ipv6_ospf6_advertise_prefix_list,
      no_ipv6_ospf6_advertise_prefix_list_cmd,
      "no ipv6 ospf6 advertise prefix-list",
      NO_STR
      IP6_STR
      OSPF6_STR
      "Advertising options\n"
      "Filter prefix using prefix-list\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);
    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->plist_name)
    {
        XFREE(MTYPE_PREFIX_LIST_STR, oi->plist_name);
        oi->plist_name = NULL;
    }

    ospf6_interface_connected_route_update(oi->interface);

    if (oi->area)
    {
        OSPF6_LINK_LSA_SCHEDULE(oi);

        if (oi->state == OSPF6_INTERFACE_DR)
        {
            OSPF6_NETWORK_LSA_SCHEDULE(oi);
            OSPF6_INTRA_PREFIX_LSA_SCHEDULE_TRANSIT(oi);
        }

        OSPF6_INTRA_PREFIX_LSA_SCHEDULE_STUB(oi->area);
    }

    return CMD_SUCCESS;
}

DEFUN(ipv6_ospf6_network,
      ipv6_ospf6_network_cmd,
      "ipv6 ospf6 network (broadcast|p2p)",
      IP6_STR
      OSPF6_STR
      "Network Type\n"
      "Specify OSPFv6 broadcast network\n"
      "Specify OSPF6 point-to-point network\n"
     )
{
	u_char state;
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);

    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    if (strncmp(argv[0], "b", 1) == 0)
    {
        if (oi->type == OSPF_IFTYPE_BROADCAST)
        {
            return CMD_SUCCESS;
        }

        oi->type = OSPF_IFTYPE_BROADCAST;
    }
    else if (strncmp(argv[0], "p", 1) == 0)
    {
        if (oi->type == OSPF_IFTYPE_POINTOPOINT)
        {
            return CMD_SUCCESS;
        }

        oi->type = OSPF_IFTYPE_POINTOPOINT;
    }

    /* Reset the interface */
    //thread_add_event(master_ospf6, interface_down, oi, 0);
    //thread_add_event(master_ospf6, interface_up, oi, 0);
    state = oi->state;
    thread_execute(master_ospf6, interface_down, oi, 0);
	
	if(state > OSPF6_INTERFACE_DOWN)
	{
    	thread_execute(master_ospf6, interface_up, oi, 0);
	}
    return CMD_SUCCESS;
}

DEFUN(no_ipv6_ospf6_network,
      no_ipv6_ospf6_network_cmd,
      "no ipv6 ospf6 network",
      NO_STR
      IP6_STR
      OSPF6_STR
      "Network Type\n"
      "Default to whatever interface type system specifies"
     )
{
	u_char state;
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    int type;

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

    assert(ifp);

    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        return CMD_SUCCESS;
    }

    assert(oi);

    if (oi->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    type = ospf6_default_iftype(ifp);

    if (oi->type == type)
    {
        return CMD_SUCCESS;
    }

    oi->type = type;
    /* Reset the interface */
    //thread_add_event(master_ospf6, interface_down, oi, 0);
    //thread_add_event(master_ospf6, interface_up, oi, 0);
    
    state = oi->state;
    thread_execute(master_ospf6, interface_down, oi, 0);
	
	if(state > OSPF6_INTERFACE_DOWN)
	{
    	thread_execute(master_ospf6, interface_up, oi, 0);
	}
    return CMD_SUCCESS;
}

DEFUN(ipv6_ospf6_interface_area,
      ipv6_ospf6_interface_area_cmd,
      "ipv6 ospf6 instance <1-255> area (A.B.C.D |<0-4294967295>)",
      IP6_STR
      OSPF6_STR
      "Start OSPF6 configuration\n"
      "instance value\n"
      "Specify the OSPF6 area ID\n"
      "OSPF6 area ID in IPv4 address notation <0-255>\n"       
      "OSPF6 area ID as a decimal value <0-255>\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    //int type;
    struct ospf6_area *oa = NULL;
    u_int32_t area_id;
    struct ospf6 *o = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;
	int mode = 1;

    VTY_GET_INTEGER_RANGE("ospf6 instance", ospf_id, argv[0],
                          OSPF6_DEFAULT_PROCESS_ID, OSPF6_MAX_PROCESS_ID);

    o = ospf6_lookup_id(ospf_id);

    if (o == NULL)
    {
        VTY_OSPF6_INSTANCE_ERR;
    }

	/* parse Area-ID */
	ospf6_str2area_id(argv[1], &area_id);
	if(ntohl(area_id) > 255)
	{
		vty_error_out(vty, "Invalid Area-ID: %s, Please input area <0-255>%s", argv[1], VNL);		
        return CMD_WARNING;
	}

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);;

    if (ifp == NULL)
    {
        ifp = ospf6_enable_interface_init(ifindex);
    }
	
	if(ifp == NULL)											
	{														
		vty_error_out(vty, "GET ospf6 ifp is NULL%s", VNL);	
		return CMD_WARNING; 								
	}

	/* get the mode of the interface*/
    mode = ospf6_get_intf_mode(ifindex);

	if (mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR6;
    }

    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        oi = ospf6_interface_create(o, ifp);
		
	    oi->mode = mode;
	    
    }
	
	if(oi == NULL)											
	{														
		vty_error_out(vty, "GET ospf6_interface is NULL%s", VNL);	
		return CMD_WARNING; 								
	}
	
	if((oi->ospf6 != NULL) && (oi->ospf6->ospf_id != ospf_id))
	{
		vty_error_out(vty,"Interface is already enabled by other OSPF6 Instance.%s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (o->router_id == 0)
    {
        vty_warning_out(vty, "Please first config router ID.%s", VTY_NEWLINE);
        return CMD_WARNING;

    }
	
    if (oi->area)
    {
    	if (oi->area->area_id == area_id)
	    {
	        return CMD_SUCCESS;
	    }
        vty_error_out(vty, "%s already attached to Area %s%s",
                oi->interface->name, oi->area->name, VNL);
        return CMD_WARNING;
    }
	
    /* find/create ospf6 area */
    oa = ospf6_area_lookup(area_id, o);

    if (oa == NULL)
    {
        oa = ospf6_area_create(area_id, o);
		if(oa == NULL)											
	  	{														
			vty_error_out(vty, "GET ospf6_area is NULL%s", VNL);	
			return CMD_WARNING; 								
		}
    }

    /* attach interface to area */
    listnode_add(oa->if_list, oi);  /* sort ?? */
    oi->area = oa;
    oi->ospf6 = o;
    SET_FLAG(oa->flag, OSPF6_AREA_ENABLE);

    /* ospf6 process is currently disabled, not much more to do */
    if (CHECK_FLAG(o->flag, OSPF6_DISABLED))
    {
        return CMD_SUCCESS;
    }

    /* start up */
    ospf6_interface_enable(oi);

    /* If the router is ABR, originate summary routes */
    if (ospf6_is_router_abr(o))
    {
        ospf6_abr_enable_area(oa);
    }

    return CMD_SUCCESS;
}

DEFUN(no_ipv6_ospf6_interface_area,
      no_ipv6_ospf6_interface_area_cmd,
      "no ipv6 ospf6 instance <1-255> area (A.B.C.D |<0-4294967295>)",
      NO_STR
      IP6_STR
      "Start OSPF6 configuration\n"
      "instance value\n"
      "Specify the OSPF6 area ID\n"
      "OSPF6 area ID in IPv4 address notation <0-255>\n"      
      "OSPF6 area ID as a decimal value <0-255>\n"
     )
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    //int type;
    struct ospf6_area *oa = NULL;
    u_int32_t area_id = 0;
    struct ospf6 *o = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;

    VTY_GET_INTEGER_RANGE("ospf6 instance", ospf_id, argv[0],
                          OSPF6_DEFAULT_PROCESS_ID, OSPF6_MAX_PROCESS_ID);

    VTY_CHECK_NM_INTERFACE6(ifindex);
    ifp = ospf6_find_interface(ifindex);

    if (ifp == NULL)
    {
        VTY_OSPF_INTERFACE_ERR;
    }

	if(ifp == NULL)											
	{														
		vty_error_out(vty, "GET ospf6 ifp is NULL.%s", VNL);	
		return CMD_WARNING; 								
	}

    o = ospf6_lookup_id(ospf_id);

    if (o == NULL)
    {
        vty_error_out(vty, "The instance is not exist. %s", VNL);
        return CMD_WARNING;
    }

	/* parse Area-ID */
	ospf6_str2area_id(argv[1], &area_id);
	if(ntohl(area_id) > 255)
	{
		vty_error_out(vty, "Invalid Area-ID: %s, Please input area <0-255>.%s", argv[1], VNL);
        return CMD_WARNING;
	}

    if (o->router_id == 0)
    {
        vty_warning_out(vty, "Please first config router ID.%s", VTY_NEWLINE);
        return CMD_WARNING;

    }

    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        vty_error_out(vty, "Interface %s not enabled%s", ifp->name, VNL);
        return CMD_WARNING;
    }
	
	if(oi->ospf6 == NULL)
	{
		vty_error_out(vty, "No such instance is enabled in this interface%s", VNL);
        return CMD_WARNING;
	}

    /* Verify Area */
    if (oi->area == NULL)
    {
		vty_error_out(vty, "No such instance is enabled in this interface%s", VNL);
        return CMD_WARNING;
    }

    if (oi->area->area_id != area_id)
    {
        vty_error_out(vty, "Wrong Area-ID: %s is attached to area %s%s",
                oi->interface->name, oi->area->name, VNL);
        return CMD_WARNING;
    }

    thread_execute(master_ospf6, interface_down, oi, 0);
    oa = oi->area;

	ospf6_interface_if_del(ifp);
	oi = NULL;

    /* Withdraw inter-area routes from this area, if necessary */
    if (oa->if_list->count == 0)
    {
        UNSET_FLAG(oa->flag, OSPF6_AREA_ENABLE);
        ospf6_abr_disable_area(oa);
    }

    return CMD_SUCCESS;


}



static int
config_write_ospf6_interface(struct vty *vty)
{
    struct listnode *i = NULL;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;

    for (ALL_LIST_ELEMENTS_RO(iflist, i, ifp))
    {
        oi = (struct ospf6_interface *) ifp->info;

        if (oi == NULL)
        {
            continue;
        }

        vty_out(vty, "interface %s%s",
                oi->interface->name, VNL);

        if (ifp->desc)
        {
            vty_out(vty, " description %s%s", ifp->desc, VNL);
        }

        if (oi->area)
        {
            vty_out(vty, " ipv6 ospf6 instance %d area %s%s",
                    oi->ospf6->ospf_id, oi->area->name, VNL);
        }

        //if (ifp->mtu6 != oi->ifmtu)
        //{
        //vty_out (vty, " ipv6 ospf6 ifmtu %d%s", oi->ifmtu, VNL);
        //}
        if (CHECK_FLAG(oi->flag, OSPF6_INTERFACE_NOAUTOCOST))
            vty_out(vty, " ipv6 ospf6 cost %d%s",
                    oi->cost, VNL);

        if (oi->hello_interval != OSPF6_INTERFACE_HELLO_INTERVAL)
            vty_out(vty, " ipv6 ospf6 hello-interval %d%s",
                    oi->hello_interval, VNL);

        //if (oi->dead_interval != OSPF6_INTERFACE_DEAD_INTERVAL)
        if (oi->dead_interval != oi->hello_interval*4)
            vty_out(vty, " ipv6 ospf6 dead-interval %d%s",
                    oi->dead_interval, VNL);

        if (oi->rxmt_interval != OSPF6_INTERFACE_RXMT_INTERVAL)
            vty_out(vty, " ipv6 ospf6 retransmit-interval %d%s",
                    oi->rxmt_interval, VNL);

        if (oi->priority != OSPF6_INTERFACE_PRIORITY)
            vty_out(vty, " ipv6 ospf6 priority %d%s",
                    oi->priority, VNL);

        if (oi->transdelay != OSPF6_INTERFACE_TRANSDELAY)
            vty_out(vty, " ipv6 ospf6 transmit-delay %d%s",
                    oi->transdelay, VNL);

        if (oi->instance_id != OSPF6_INTERFACE_INSTANCE_ID)
            vty_out(vty, " ipv6 ospf6 instance-id %d%s",
                    oi->instance_id, VNL);

        if (oi->plist_name)
            vty_out(vty, " ipv6 ospf6 advertise prefix-list %s%s",
                    oi->plist_name, VNL);

        if (CHECK_FLAG(oi->flag, OSPF6_INTERFACE_PASSIVE))
        {
            vty_out(vty, " ipv6 ospf6 passive%s", VNL);
        }

        if (oi->mtu_ignore)
        {
            vty_out(vty, " ipv6 ospf6 mtu-ignore%s", VNL);
        }

        if (oi->type == OSPF_IFTYPE_POINTOPOINT)
        {
            vty_out(vty, " ipv6 ospf6 network p2p%s", VNL);
        }

#if 0
        else if (oi->type == OSPF_IFTYPE_BROADCAST)
        {
            vty_out(vty, " ipv6 ospf6 network broadcast%s", VNL);
        }

#endif
        vty_out(vty, "!%s", VNL);
    }

    return 0;
}

static void
ospf6_trunkif_init(void)
{
    /* ipv6 ospf6 cost */
    install_element(TRUNK_IF_NODE, &ipv6_ospf6_cost_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ospf6_cost_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ipv6_ospf6_cost_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ospf6_cost_cmd, CMD_SYNC);
    /* ipv6 ospf6 ifmtu */
    install_element(TRUNK_IF_NODE, &ipv6_ospf6_ifmtu_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ospf6_ifmtu_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ipv6_ospf6_ifmtu_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ospf6_ifmtu_cmd, CMD_SYNC);
    /* ipv6 ospf6 deadinterval */
    install_element(TRUNK_IF_NODE, &ipv6_ospf6_deadinterval_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ospf6_deadinterval_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ipv6_ospf6_deadinterval_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ospf6_deadinterval_cmd, CMD_SYNC);
    /* ipv6 ospf6 hellointerval */
    install_element(TRUNK_IF_NODE, &ipv6_ospf6_hellointerval_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ospf6_hellointerval_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ipv6_ospf6_hellointerval_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ospf6_hellointerval_cmd, CMD_SYNC);
    /* ipv6 ospf6 priority */
    install_element(TRUNK_IF_NODE, &ipv6_ospf6_priority_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ospf6_priority_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ipv6_ospf6_priority_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ospf6_priority_cmd, CMD_SYNC);
    /* ipv6 ospf6 instace */
    install_element(TRUNK_IF_NODE, &ipv6_ospf6_instance_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ospf6_instance_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ipv6_ospf6_instance_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ospf6_instance_cmd, CMD_SYNC);
    /* ipv6 ospf6 passive */
    install_element(TRUNK_IF_NODE, &ipv6_ospf6_passive_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ospf6_passive_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ipv6_ospf6_passive_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ospf6_passive_cmd, CMD_SYNC);
    /* ipv6 ospf6 mtu_ignore */
    install_element(TRUNK_IF_NODE, &ipv6_ospf6_mtu_ignore_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ospf6_mtu_ignore_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ipv6_ospf6_mtu_ignore_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ospf6_mtu_ignore_cmd, CMD_SYNC);
    /* ipv6 ospf6 network */
    install_element(TRUNK_IF_NODE, &ipv6_ospf6_network_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ospf6_network_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ipv6_ospf6_network_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ospf6_network_cmd, CMD_SYNC);
    /* ipv6 ospf6 instance area */
    install_element(TRUNK_IF_NODE, &ipv6_ospf6_interface_area_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ospf6_interface_area_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ipv6_ospf6_interface_area_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ospf6_interface_area_cmd, CMD_SYNC);
}

static void
ospf6_vlanif_init(void)
{
    /* ipv6 ospf6 cost */
    install_element(VLANIF_NODE, &ipv6_ospf6_cost_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ipv6_ospf6_cost_cmd, CMD_SYNC);

    /* ipv6 ospf6 ifmtu */
    install_element(VLANIF_NODE, &ipv6_ospf6_ifmtu_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ipv6_ospf6_ifmtu_cmd, CMD_SYNC);

    /* ipv6 ospf6 deadinterval */
    install_element(VLANIF_NODE, &ipv6_ospf6_deadinterval_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ipv6_ospf6_deadinterval_cmd, CMD_SYNC);

    /* ipv6 ospf6 hellointerval */
    install_element(VLANIF_NODE, &ipv6_ospf6_hellointerval_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ipv6_ospf6_hellointerval_cmd, CMD_SYNC);

    /* ipv6 ospf6 priority */
    install_element(VLANIF_NODE, &ipv6_ospf6_priority_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ipv6_ospf6_priority_cmd, CMD_SYNC);

    /* ipv6 ospf6 instace */
    install_element(VLANIF_NODE, &ipv6_ospf6_instance_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ipv6_ospf6_instance_cmd, CMD_SYNC);

    /* ipv6 ospf6 passive */
    install_element(VLANIF_NODE, &ipv6_ospf6_passive_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ipv6_ospf6_passive_cmd, CMD_SYNC);

    /* ipv6 ospf6 mtu_ignore */
    install_element(VLANIF_NODE, &ipv6_ospf6_mtu_ignore_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ipv6_ospf6_mtu_ignore_cmd, CMD_SYNC);
	
    /* ipv6 ospf6 network */
    install_element(VLANIF_NODE, &ipv6_ospf6_network_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ipv6_ospf6_network_cmd, CMD_SYNC);

    /* ipv6 ospf6 instance area */
    install_element(VLANIF_NODE, &ipv6_ospf6_interface_area_cmd, CMD_SYNC);
    install_element(VLANIF_NODE, &no_ipv6_ospf6_interface_area_cmd, CMD_SYNC);

}


void
ospf6_interface_init(void)
{
    /* Install interface node. */
    ifm_l3if_init(config_write_ospf6_interface);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_interface_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_interface_prefix_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_interface_prefix_detail_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_interface_prefix_match_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_interface_ifname_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_interface_ifname_prefix_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_interface_ifname_prefix_detail_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_interface_ifname_prefix_match_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_interface_trunk_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_interface_vlanif_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_interface_loopback_cmd, CMD_LOCAL);
	install_element(OSPF6_NODE, &show_ipv6_ospf6_interface_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_interface_prefix_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_interface_prefix_detail_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_interface_prefix_match_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_interface_ifname_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_interface_ifname_prefix_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_interface_ifname_prefix_detail_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_interface_ifname_prefix_match_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_interface_trunk_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_interface_vlanif_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_interface_loopback_cmd, CMD_LOCAL);	

    /* ipv6 ospf6 cost */
    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ospf6_cost_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_cost_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ipv6_ospf6_cost_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ospf6_cost_cmd, CMD_SYNC);
    /* ipv6 ospf6 ifmtu */
    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ospf6_ifmtu_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_ifmtu_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ipv6_ospf6_ifmtu_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ospf6_ifmtu_cmd, CMD_SYNC);
    /* ipv6 ospf6 deadinterval */
    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ospf6_deadinterval_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_deadinterval_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ipv6_ospf6_deadinterval_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ospf6_deadinterval_cmd, CMD_SYNC);
    /* ipv6 ospf6 hellointerval */
    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ospf6_hellointerval_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_hellointerval_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ipv6_ospf6_hellointerval_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ospf6_hellointerval_cmd, CMD_SYNC);
    /* ipv6 ospf6 priority */
    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ospf6_priority_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_priority_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ipv6_ospf6_priority_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ospf6_priority_cmd, CMD_SYNC);
    /* ipv6 ospf6 instace */
    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ospf6_instance_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_instance_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ipv6_ospf6_instance_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ospf6_instance_cmd, CMD_SYNC);
    /* ipv6 ospf6 passive */
    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ospf6_passive_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_passive_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ipv6_ospf6_passive_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ospf6_passive_cmd, CMD_SYNC);
    /* ipv6 ospf6 mtu_ignore */
    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ospf6_mtu_ignore_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_mtu_ignore_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ipv6_ospf6_mtu_ignore_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ospf6_mtu_ignore_cmd, CMD_SYNC);
    /* ipv6 ospf6 network */
    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ospf6_network_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_network_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ipv6_ospf6_network_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ospf6_network_cmd, CMD_SYNC);
    /* ipv6 ospf6 instance area */
    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ospf6_interface_area_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_interface_area_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ipv6_ospf6_interface_area_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ospf6_interface_area_cmd, CMD_SYNC);
    install_element(LOOPBACK_IF_NODE, &ipv6_ospf6_interface_area_cmd, CMD_SYNC);
    install_element(LOOPBACK_IF_NODE, &no_ipv6_ospf6_interface_area_cmd, CMD_SYNC);

	ospf6_trunkif_init();
	ospf6_vlanif_init();

    /* reference bandwidth commands */
    install_element(OSPF6_NODE, &auto_cost_reference_bandwidth_cmd, CMD_SYNC);
    install_element(OSPF6_NODE, &no_auto_cost_reference_bandwidth_cmd, CMD_SYNC);
}

#if 0
DEFUN(debug_ospf6_interface,
      debug_ospf6_interface_cmd,
      "debug ospf6 interface",
      DEBUG_STR
      OSPF6_STR
      "Debug OSPF6 Interface\n"
     )
{
    OSPF6_DEBUG_INTERFACE_ON();
    return CMD_SUCCESS;
}

DEFUN(no_debug_ospf6_interface,
      no_debug_ospf6_interface_cmd,
      "no debug ospf6 interface",
      NO_STR
      DEBUG_STR
      OSPF6_STR
      "Debug OSPF6 Interface\n"
     )
{
    OSPF6_DEBUG_INTERFACE_OFF();
    return CMD_SUCCESS;
}
#endif
int
config_write_ospf6_debug_interface(struct vty *vty)
{
    if (IS_OSPF6_DEBUG_INTERFACE)
    {
        vty_out(vty, "debug ospf6 interface%s", VNL);
    }

    return 0;
}

#if 0
void
install_element_ospf6_debug_interface(void)
{
    install_element(CONFIG_NODE, &debug_ospf6_interface_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_debug_ospf6_interface_cmd, CMD_SYNC);
}
#endif



