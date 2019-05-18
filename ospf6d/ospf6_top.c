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

#include "log.h"
#include "memory.h"
#include "vty.h"
#include "linklist.h"
#include "prefix.h"
#include "table.h"
#include "thread.h"
#include "command.h"
#include "timer.h"
#include "ifm_common.h"
#include "vrf.h"
#include "mpls_common.h"


#include "ospf6_proto.h"
#include "ospf6_message.h"
#include "ospf6_lsa.h"
#include "ospf6_lsdb.h"
#include "ospf6_route.h"
#include "ospf6_zebra.h"

#include "ospf6_top.h"
#include "ospf6_area.h"
#include "ospf6_interface.h"
#include "ospf6_neighbor.h"

#include "ospf6_flood.h"
#include "ospf6_asbr.h"
#include "ospf6_abr.h"
#include "ospf6_intra.h"
#include "ospf6_spf.h"
#include "ospf6d.h"


static void ospf6_disable(struct ospf6 *o);

static void
ospf6_top_lsdb_hook_add(struct ospf6_lsa *lsa)
{
    struct ospf6 *ospf6 = ospf6_get_instance_self(lsa);

    switch (ntohs(lsa->header->type))
    {
        case OSPF6_LSTYPE_AS_EXTERNAL:
            ospf6_asbr_lsa_add(ospf6, lsa);
            break;
        default:
            break;
    }
}

static void
ospf6_top_lsdb_hook_remove(struct ospf6_lsa *lsa)
{
    struct ospf6 *ospf6 = ospf6_get_instance_self(lsa);

    switch (ntohs(lsa->header->type))
    {
        case OSPF6_LSTYPE_AS_EXTERNAL:
            ospf6_asbr_lsa_remove(ospf6, lsa);
            break;
        default:
            break;
    }
}

static void
ospf6_top_route_hook_add(struct ospf6_route *route, struct ospf6 *ospf6)
{
    ospf6_abr_originate_summary(route, ospf6);
    ospf6_zebra_route_update_add(route, ospf6);
}

static void
ospf6_top_route_hook_remove(struct ospf6_route *route, struct ospf6 *ospf6)
{
    ospf6_abr_originate_summary(route, ospf6);
    ospf6_zebra_route_update_remove(route, ospf6);
}

static void
ospf6_top_brouter_hook_add(struct ospf6_route *route, struct ospf6 *ospf6)
{
    ospf6_abr_examin_brouter(ospf6, ADV_ROUTER_IN_PREFIX(&route->prefix));
    ospf6_asbr_lsentry_add(route, ospf6);
    ospf6_abr_originate_summary(route , ospf6);
}

static void
ospf6_top_brouter_hook_remove(struct ospf6_route *route, struct ospf6 *ospf6)
{
    ospf6_abr_examin_brouter(ospf6, ADV_ROUTER_IN_PREFIX(&route->prefix));
    ospf6_asbr_lsentry_remove(route, ospf6);
    ospf6_abr_originate_summary(route, ospf6);
}

static struct ospf6 *
ospf6_create(u_int16_t ospf_id)
{
    struct ospf6 *o;

    o = XCALLOC(MTYPE_OSPF6_TOP, sizeof(struct ospf6));

	if (o == NULL)
    {
        zlog_err("%-15s[%d] Can't malloc ospf6", __FUNCTION__, __LINE__);
        return (struct ospf6 *)NULL;
    }
	memset(o, 0, sizeof(struct ospf6));

    /* initialize */
    time_get_time(TIME_CLK_MONOTONIC, &o->starttime);
    o->area_list = list_new();
    o->area_list->cmp = ospf6_area_cmp;
    o->lsdb = ospf6_lsdb_create(o);
    o->lsdb_self = ospf6_lsdb_create(o);
    o->lsdb->hook_add = ospf6_top_lsdb_hook_add;
    o->lsdb->hook_remove = ospf6_top_lsdb_hook_remove;

    o->spf_delay = OSPF_SPF_DELAY_DEFAULT;
    o->spf_holdtime = OSPF_SPF_HOLDTIME_DEFAULT;
    o->spf_max_holdtime = OSPF_SPF_MAX_HOLDTIME_DEFAULT;
    o->spf_hold_multiplier = 1;

    o->route_table = OSPF6_ROUTE_TABLE_CREATE(GLOBAL, ROUTES);
    o->route_table->scope = o;
    o->route_table->hook_add = ospf6_top_route_hook_add;
    o->route_table->hook_remove = ospf6_top_route_hook_remove;

    o->brouter_table = OSPF6_ROUTE_TABLE_CREATE(GLOBAL, BORDER_ROUTERS);
    o->brouter_table->scope = o;
    o->brouter_table->hook_add = ospf6_top_brouter_hook_add;
    o->brouter_table->hook_remove = ospf6_top_brouter_hook_remove;

    o->external_table = OSPF6_ROUTE_TABLE_CREATE(GLOBAL, EXTERNAL_ROUTES);
    o->external_table->scope = o;
    o->ospf_id = ospf_id;

    o->external_id_table = route_table_init();

    o->redist = list_new();
    o->redist->cmp = ospf6_redist_cmp;

    o->ref_bandwidth = OSPF6_REFERENCE_BANDWIDTH;
	o->distance_all = ROUTE_METRIC_OSPF;

    return o;
}


/* lookup ospf6 instance by ospf_id */
struct ospf6 *
ospf6_lookup_id(u_int16_t ospf_id)
{
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf6* o = NULL;

    for (ALL_LIST_ELEMENTS(om6->ospf6, node, nnode, o))
    {
        if (o->ospf_id == ospf_id)
        {
            return o;
        }
    }

    return NULL;

}

/* add ospf6 instance to om6 */
static void
ospf6_add(struct ospf6 *o)
{
    listnode_add_sort(om6->ospf6, o);
}

/* delete ospf6 instance from om6*/
static void
ospf6_del(struct ospf6 *o)
{
    listnode_delete(om6->ospf6, o);
}


/* if ospf6 instance exist return it,
   else create ospf6 instance and return it 
*/
static struct ospf6 *
ospf6_get(u_int16_t ospf_id)
{
    struct ospf6 *o = NULL;

    o = ospf6_lookup_id(ospf_id);

    if (o == NULL)
    {
        o = ospf6_create(ospf_id);
        ospf6_add(o);
    }

    return o;
}

void
ospf6_delete(struct ospf6 *o)
{
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf6_area *oa = NULL;
	struct interface *ifp = NULL;
    struct listnode *i = NULL;
    struct ospf6_interface *oi = NULL;

    ospf6_disable(o);
	
	/* First delete all interface belong to this ospf6 */
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
		ospf6_interface_if_del(ifp);

	}
	
	/* Second delete all area belong to this ospf */
    for (ALL_LIST_ELEMENTS(o->area_list, node, nnode, oa))
    {
        ospf6_area_delete(oa);
		oa = NULL;
    }

    list_delete(o->area_list);
	o->area_list = NULL;
    ospf6_lsdb_delete(o->lsdb);
    ospf6_lsdb_delete(o->lsdb_self);
	o->lsdb = NULL;
	o->lsdb_self = NULL;
    ospf6_route_table_delete(o->route_table, o);
    ospf6_route_table_delete(o->brouter_table, o);
    ospf6_route_table_delete(o->external_table, o);
	o->route_table = NULL;
	o->brouter_table = NULL;
	o->external_table = NULL;
    route_table_finish(o->external_id_table);
	o->external_id_table = NULL;
    ospf6_del(o);
    XFREE(MTYPE_OSPF6_TOP, o);
}



static void
__attribute__((unused))
ospf6_enable(struct ospf6 *o)
{
    struct listnode *node, *nnode;
    struct ospf6_area *oa;

    if (CHECK_FLAG(o->flag, OSPF6_DISABLED))
    {
        UNSET_FLAG(o->flag, OSPF6_DISABLED);

        for (ALL_LIST_ELEMENTS(o->area_list, node, nnode, oa))
        {
            ospf6_area_enable(oa);
        }
    }
}

static void
ospf6_disable(struct ospf6 *o)
{
    struct listnode *node, *nnode;
    struct ospf6_area *oa;

    if (! CHECK_FLAG(o->flag, OSPF6_DISABLED))
    {
        SET_FLAG(o->flag, OSPF6_DISABLED);

        for (ALL_LIST_ELEMENTS(o->area_list, node, nnode, oa))
        {
            ospf6_area_disable(oa);
        }

        /* XXX: This also changes persistent settings */
        ospf6_asbr_redistribute_reset(o);
        ospf6_lsdb_remove_all(o->lsdb);
        ospf6_route_remove_all(o->route_table, o);
        ospf6_route_remove_all(o->brouter_table, o);
        OSPF6_TIMER_OFF(o->maxage_remover);
        OSPF6_TIMER_OFF(o->t_spf_calc);
        OSPF6_TIMER_OFF(o->t_ase_calc);
    }
}

static int
ospf6_maxage_remover(void *thread)
{
    struct ospf6 *o = (struct ospf6 *)(thread);
    struct ospf6_area *oa = NULL;
    struct ospf6_interface *oi = NULL;
    struct ospf6_neighbor *on = NULL;
    struct listnode *i = NULL, *j = NULL, *k = NULL;
    assert(o);

    int reschedule = 0;
    o->maxage_remover = 0;

    for (ALL_LIST_ELEMENTS_RO(o->area_list, i, oa))
    {
        for (ALL_LIST_ELEMENTS_RO(oa->if_list, j, oi))
        {
            for (ALL_LIST_ELEMENTS_RO(oi->neighbor_list, k, on))
            {
                if (on->state != OSPF6_NEIGHBOR_EXCHANGE &&
                        on->state != OSPF6_NEIGHBOR_LOADING)
                {
                    continue;
                }

                ospf6_maxage_remove(o);
                return 0;
            }
        }
    }

    for (ALL_LIST_ELEMENTS_RO(o->area_list, i, oa))
    {
        for (ALL_LIST_ELEMENTS_RO(oa->if_list, j, oi))
        {
            if (ospf6_lsdb_maxage_remover(oi->lsdb))
            {
                reschedule = 1;
            }
        }

        if (ospf6_lsdb_maxage_remover(oa->lsdb))
        {
            reschedule = 1;
        }
    }

    if (ospf6_lsdb_maxage_remover(o->lsdb))
    {
        reschedule = 1;
    }

    if (reschedule)
    {
        ospf6_maxage_remove(o);
    }

    return 0;
}

void
ospf6_maxage_remove(struct ospf6 *o)
{
    if (o && ! o->maxage_remover)
    {
        /*o->maxage_remover = thread_add_timer(master_ospf6, ospf6_maxage_remover, o,
                                             OSPF_LSA_MAXAGE_REMOVE_DELAY_DEFAULT);*/
		o->maxage_remover = high_pre_timer_add ((char *)"ospf6_max_rm_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_maxage_remover, (void *)o, (OSPF_LSA_MAXAGE_REMOVE_DELAY_DEFAULT)*1000);
    }
}

static void
ospf6_interface_get1(uint32_t ifindex, vrf_id_t vrf_id)
{
    struct ifm_l3  l3 = {0};
    struct ifm_l3  l3_unnum = {0};
    struct ifm_event pevent;
    memset(&pevent, 0, sizeof(struct ifm_event));
    uint8_t pdown_flag = 0;
    char ip[INET_ADDRSTRLEN] = "";

    /* get link */

    if (ifm_get_link(ifindex, MODULE_ID_OSPF6, &pdown_flag) != 0)
    {
        zlog_err("%-15s[%d]: OSPF get ifindex %0x link status time out", __FUNCTION__, __LINE__, ifindex);
        pevent.up_flag = IFNET_LINKDOWN;
    }
    else
    {

        pevent.up_flag = pdown_flag;
        zlog_debug(OSPF6_DBG_INTERFACE, "%-15s[%d]: OSPF get link ifindex %0x %d", __FUNCTION__, __LINE__, ifindex, pevent.up_flag);
    }


    if (ifm_get_l3if(ifindex, MODULE_ID_OSPF6, &l3) == 0)
    {
        if (l3.ipv4_flag == IP_TYPE_UNNUMBERED)
        {
            if (ifm_get_l3if(l3.unnumbered_if, MODULE_ID_OSPF6, &l3_unnum) == 0)
            {
                pevent.ifindex = ifindex;
                pevent.mode = IFNET_MODE_L3;
                pevent.ipaddr.type = INET_FAMILY_IPV4;
                pevent.ipaddr.addr.ipv4 = l3_unnum.ipv4[0].addr;
                pevent.ipaddr.prefixlen = l3_unnum.ipv4[0].prefixlen;
                pevent.ipflag = l3.ipv4_flag;
                pevent.up_flag = pdown_flag;
                zlog_debug(OSPF6_DBG_INTERFACE, "%-15s[%d]: OSPF get ip  ifindex %0x:[%s],mask :%d", __func__, __LINE__, ifindex, inet_ipv4tostr(l3_unnum.ipv4[0].addr, ip) , \
                           l3_unnum.ipv4[0].prefixlen);
                ospf6_zebra_if_address_update_add(pevent, vrf_id);

                if (l3.ipv4[1].addr != 0)
                {
                    memset(&pevent, 0, sizeof(struct ifm_event));
                    pevent.ifindex = ifindex;
                    pevent.ipaddr.type = INET_FAMILY_IPV4;
                    pevent.ipaddr.addr.ipv4 = l3_unnum.ipv4[1].addr;
                    pevent.ipaddr.prefixlen = l3_unnum.ipv4[1].prefixlen;
                    pevent.ipflag = IP_TYPE_SLAVE;
                    pevent.up_flag = pdown_flag;
                    ospf6_zebra_if_address_update_add(pevent, vrf_id);
                }
            }
        }
        else
        {
            pevent.ifindex = ifindex;
            pevent.mode = IFNET_MODE_L3;
            pevent.ipaddr.type = INET_FAMILY_IPV4;
            pevent.ipaddr.addr.ipv4 = l3.ipv4[0].addr;
            pevent.ipaddr.prefixlen = l3.ipv4[0].prefixlen;
            pevent.ipflag = l3.ipv4_flag;
            zlog_debug(OSPF6_DBG_INTERFACE, "%-15s[%d]: OSPF get ip  ifindex %0x:[%s],mask :%d", __func__, __LINE__, ifindex, inet_ipv4tostr(l3.ipv4[0].addr, ip) , \
                       l3.ipv4[0].prefixlen);
            ospf6_zebra_if_address_update_add(pevent, vrf_id);

            if (l3.ipv4[1].addr != 0)
            {
                memset(&pevent, 0, sizeof(struct ifm_event));
                pevent.ifindex = ifindex;
                pevent.ipaddr.type = INET_FAMILY_IPV4;
                pevent.ipaddr.addr.ipv4 = l3.ipv4[1].addr;
                pevent.ipaddr.prefixlen = l3.ipv4[1].prefixlen;
                pevent.ipflag = IP_TYPE_SLAVE;
                pevent.up_flag = pdown_flag;
                ospf6_zebra_if_address_update_add(pevent, vrf_id);
            }
        }

    }
    else
    {
        ospf6_zebra_if_state_update(pevent, vrf_id);

        zlog_warn("%-15s[%d]: OSPF get ifindex %0x ip error", __func__, __LINE__, ifindex);
    }
}


static struct interface *
ospf6_enable_interface_init1(uint32_t ifindex)
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
    ospf6_interface_get1(ifindex, vrf_id);

    if (ifp == NULL)
    {
        ifp = if_lookup_by_index_vrf(ifindex, vrf_id);
    }

    return ifp;
}

static
void ospf6_distance_external_update(struct ospf6 *ospf6)
{
    struct ospf6_route *route = NULL;
	struct ospf6_route_table *table = ospf6->route_table;
	
    route = ospf6_route_head(table);  

    while (route)
    {
		if (route->path.type == OSPF6_PATH_TYPE_EXTERNAL1 ||
			route->path.type == OSPF6_PATH_TYPE_EXTERNAL2)
		{
        	ospf6_zebra_route_update_update(route, ospf6);
		}

        route = ospf6_route_next(route);
    }
}

static
void ospf6_distance_all_update(struct ospf6 *ospf6)
{
    struct ospf6_route *route = NULL;
	struct ospf6_route_table *table = ospf6->route_table;
	
    route = ospf6_route_head(table);  

    while (route)
    {
        ospf6_zebra_route_update_update(route, ospf6);

        route = ospf6_route_next(route);
    }
}


/* start ospf6 */
DEFUN(router_ospf6,
      router_ospf6_cmd,
      "ospf6 instance <1-255>",
      OSPF6_STR
      "Start OSPF6 configuration\n"
      OSPF6_INSTANCE)
{
    struct ospf6* o = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;
    //uint16_t vpnid;
    //struct l3vpn_entry *l3vpn_info = NULL;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf6 instance", ospf_id, argv[0],
                              OSPF6_DEFAULT_PROCESS_ID, OSPF6_MAX_PROCESS_ID);
    }

    if (ospf_id < OSPF6_DEFAULT_PROCESS_ID || ospf_id > OSPF6_MAX_PROCESS_ID)
    {
        vty_error_out(vty, "Cannot creat this ospf6 instance %d%s", ospf_id, VTY_NEWLINE);
        return CMD_WARNING;
    }
#if 0
    if(argv[1] != NULL)
    { 
        VTY_GET_INTEGER_RANGE ("ospf6 vpn", vpnid, argv[1], 1, 127);
        if((o = ospf6_lookup_id(ospf_id)) == NULL)
        {
            if(vty->config_read_flag != 1)
            { 
                l3vpn_info = mpls_com_get_l3vpn_instance(vpnid, MODULE_ID_OSPF6);
                if (NULL == l3vpn_info)
                {
                    vty_error_out(vty, "The VPN-Instance %d does not exist.%s", vpnid, VTY_NEWLINE);
                    return CMD_WARNING;
                }
            }
            
            o = ospf6_get(ospf_id);
            o->vpn = vpnid;
            
        }
        else
        {

            if(o->vpn != vpnid)
            {
                vty_error_out(vty,"The specified Instance ID has been used another vpn instance.%s", VTY_NEWLINE);
                return CMD_WARNING;
            }
        }
    }
    else
    {
        o = ospf6_get(ospf_id);
    }
#endif
    o = ospf6_get(ospf_id);

    /* set current ospf point. */
    vty->node = OSPF6_NODE;
    vty->index = o;
    return CMD_SUCCESS;
}

/* stop ospf6 */
DEFUN(no_router_ospf6,
      no_router_ospf6_cmd,
      "no ospf6 instance <1-255>",
      NO_STR
      OSPF6_ROUTER_STR
      "Start OSPF6 configuration\n"
      OSPF6_INSTANCE)
{
    struct ospf6* o = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf6 instance", ospf_id, argv[0],
                              OSPF6_DEFAULT_PROCESS_ID, OSPF6_MAX_PROCESS_ID);
    }

    if (ospf_id < OSPF6_DEFAULT_PROCESS_ID || ospf_id > OSPF6_MAX_PROCESS_ID)
    {
        vty_error_out(vty, "Cannot creat this ospf instance %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    o = ospf6_lookup_id(ospf_id);

    if (o == NULL)
    {
        VTY_OSPF6_INSTANCE_ERR;
    }

    else
    {
        ospf6_delete(o);
        o = NULL;
    }

    /* return to config node . */
    vty->node = CONFIG_NODE;
    vty->index = NULL;
    return CMD_SUCCESS;
}

DEFUN(reset_ospf6,
      reset_ospf6_cmd,
      "reset ospf6 instance <1-255>",
      "Reset operation\n"
      OSPF6_STR
      OSPF6_INSTANCE
      "OSPF6 instance number\n")
{
    //struct ospf6* o = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;

    if (argv[0] != NULL)
    {
        VTY_GET_INTEGER_RANGE("ospf6 instance", ospf_id, argv[0],
                              OSPF6_DEFAULT_PROCESS_ID, OSPF6_MAX_PROCESS_ID);
    }

    if (ospf_id < OSPF6_DEFAULT_PROCESS_ID || ospf_id > OSPF6_MAX_PROCESS_ID)
    {
        vty_error_out(vty, "Cannot creat this ospf instance %d%s", ospf_id, VTY_NEWLINE);
        return CMD_WARNING;
    }


    return CMD_SUCCESS;
}


/* change Router_ID commands. */
DEFUN(ospf6_router_id,
      ospf6_router_id_cmd,
      "router-id A.B.C.D",
      "Configure OSPF Router-ID\n"
      V4NOTATION_STR)
{
    int ret;
    u_int32_t router_id;
    struct ospf6 *o = NULL;
    o = (struct ospf6 *) vty->index;

    ret = inet_pton(AF_INET, argv[0], &router_id);

    if (ret == 0)
    {
        vty_error_out(vty, "malformed OSPF Router-ID: %s%s", argv[0], VNL);
        return CMD_WARNING;
    }

    if (router_id == 0)
    {
        vty_error_out(vty, "Cannot set router id 0.0.0.0%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    o->router_id_static = router_id;

    if (o->router_id  == 0)
    {
        o->router_id  = router_id;
    }
    else
    {
        vty_out(vty, "Set router id succeeded. You need to restart the OSPF instance to validate the new router ID.%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}
DEFUN(distance_ospf6,
      distance_ospf6_cmd,
      "distance {external} <1-255>",
      "Define an administrative distance\n"
      "Distance for external routes\n"
      "Distance value\n")
{    
	struct ospf6 *o = NULL;
    o = (struct ospf6 *) vty->index;
	
	if (argv[0] != NULL)
    {
        o->distance_external = atoi(argv[1]);

        ospf6_distance_external_update(o);
        
    }
    else
    {
        o->distance_all = atoi(argv[1]);
        ospf6_distance_all_update(o);

    }
    return CMD_SUCCESS;
}

DEFUN(no_distance_ospf6,
      no_distance_ospf6_cmd,
      "no distance [external]",
      NO_STR
      "Define an administrative distance\n"
      "External routes\n")
{
	struct ospf6 *o = NULL;
	o = (struct ospf6 *) vty->index;
	
	if (argv[0] != NULL)
	{
		o->distance_external = 0;

		ospf6_distance_external_update(o);
	}
	else
	{
	   o->distance_all = ROUTE_METRIC_OSPF;
	   ospf6_distance_all_update(o);

	}

    return CMD_SUCCESS;
}



DEFUN(ospf6_log_adjacency_changes,
      ospf6_log_adjacency_changes_cmd,
      "log-adjacency-changes",
      "Log changes in adjacency state\n")
{
    struct ospf6 *ospf6 = vty->index;
    SET_FLAG(ospf6->config_flags, OSPF6_LOG_ADJACENCY_CHANGES);
    return CMD_SUCCESS;
}

DEFUN(ospf6_log_adjacency_changes_detail,
      ospf6_log_adjacency_changes_detail_cmd,
      "log-adjacency-changes detail",
      "Log changes in adjacency state\n"
      "Log all state changes\n")
{
    struct ospf6 *ospf6 = vty->index;
    SET_FLAG(ospf6->config_flags, OSPF6_LOG_ADJACENCY_CHANGES);
    SET_FLAG(ospf6->config_flags, OSPF6_LOG_ADJACENCY_DETAIL);
    return CMD_SUCCESS;
}

DEFUN(no_ospf6_log_adjacency_changes,
      no_ospf6_log_adjacency_changes_cmd,
      "no log-adjacency-changes",
      NO_STR
      "Log changes in adjacency state\n")
{
    struct ospf6 *ospf6 = vty->index;
    UNSET_FLAG(ospf6->config_flags, OSPF6_LOG_ADJACENCY_DETAIL);
    UNSET_FLAG(ospf6->config_flags, OSPF6_LOG_ADJACENCY_CHANGES);
    return CMD_SUCCESS;
}

DEFUN(no_ospf6_log_adjacency_changes_detail,
      no_ospf6_log_adjacency_changes_detail_cmd,
      "no log-adjacency-changes detail",
      NO_STR
      "Log changes in adjacency state\n"
      "Log all state changes\n")
{
    struct ospf6 *ospf6 = vty->index;
    UNSET_FLAG(ospf6->config_flags, OSPF6_LOG_ADJACENCY_DETAIL);
    return CMD_SUCCESS;
}

DEFUN(ospf6_interface_ethernet_area,
      ospf6_interface_ethernet_area_cmd,
      "interface (ethernet USP|gigabitethernet USP|xgigabitethernet USP) area A.B.C.D",
      "Enable routing on an IPv6 interface\n"
      "Ethernet interface type\n"
      "Tdm interface type\n"
      "Stm interface type\n"
      "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
      "Specify the OSPF6 area ID\n"
      "OSPF6 area ID in IPv4 address notation\n")
{
    struct ospf6 *o = NULL;
    struct ospf6_area *oa = NULL;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    u_int32_t area_id;
    o = (struct ospf6 *) vty->index;
    uint32_t ifindex = 0;
    int ret = -1;
    char if_name[IFNET_NAMESIZE];

    if (argv[0] != NULL && argv[1] == NULL && argv[2] == NULL)
    {
        ifindex = ifm_get_ifindex_by_name((char *)"ethernet", (char *) argv[0]);
    }
    else if (argv[0] == NULL && argv[1] != NULL && argv[2] == NULL)
    {
        ifindex = ifm_get_ifindex_by_name((char *)"gigabitethernet", (char *) argv[1]);
    }
    else if (argv[0] == NULL && argv[1] == NULL && argv[2] != NULL)
    {
        ifindex = ifm_get_ifindex_by_name((char *)"xgigabitethernet", (char *) argv[2]);
    }

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


    /* find/create ospf6 interface */
    ifp = if_get_by_name(if_name);

    if (ifp == NULL)
    {
        vty_error_out(vty, "This interface not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (ifp->ifindex == 0)
    {
        ifp = ospf6_enable_interface_init1(ifindex);
#if 0
        uint8_t pdown_flag = 0;

        /* get link */

        if (ifm_get_link(ifindex, MODULE_ID_OSPF6,&pdown_flag) != 0)
        {
            vty_out(vty, "Error:This interface not exist%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

#endif
        ifp->ifindex = ifindex;
    }

    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        oi = ospf6_interface_create(o, ifp);
    }
	
	if(oi == NULL)											
	{														
		vty_error_out(vty, "GET ospf6_interface is NULL%s", VNL);	
		return CMD_WARNING; 								
	}

    if (oi->area)
    {
        vty_error_out(vty, "%s already attached to Area %s%s",
                oi->interface->name, oi->area->name, VNL);
        return CMD_WARNING;
    }

    /* parse Area-ID */
    if (inet_pton(AF_INET, argv[3], &area_id) != 1)
    {
        vty_error_out(vty, "Invalid Area-ID: %s%s", argv[1], VNL);
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


DEFUN(ospf6_interface_loopback_area,
      ospf6_interface_loopback_area_cmd,
      "interface loopback <0-128> area A.B.C.D",
      "Enable routing on an IPv6 interface\n"
      "Ethernet interface type\n"
      "Tdm interface type\n"
      "Stm interface type\n"
      "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
      "Specify the OSPF6 area ID\n"
      "OSPF6 area ID in IPv4 address notation\n"
     )
{
    struct ospf6 *o = NULL;
    struct ospf6_area *oa = NULL;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    u_int32_t area_id;
    o = (struct ospf6 *) vty->index;
    uint32_t ifindex = 0;
    int ret = -1;
    char if_name[IFNET_NAMESIZE];

    ifindex = ifm_get_ifindex_by_name((char *)"loopback", (char *)argv[0]);


    if (ifindex == 0)
    {
        vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get the name of the interface*/
    ret  = ifm_get_name_by_ifindex(ifindex , if_name);

    if (ret < 0)
    {
        vty_error_out(vty, "DFailed to get interface information.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }


    /* find/create ospf6 interface */
    ifp = if_get_by_name(if_name);

    if (ifp == NULL)
    {
        vty_error_out(vty, "This interface not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (ifp->ifindex == 0)
    {
        ifp = ospf6_enable_interface_init1(ifindex);

        if (ifp == NULL)
        {
            vty_error_out(vty, "This interface not exist%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        ifp->ifindex = ifindex;
    }

    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        oi = ospf6_interface_create(o, ifp);
    }
	
	if(oi == NULL)											
	{														
		vty_error_out(vty, "GET ospf6_interface is NULL%s", VNL);	
		return CMD_WARNING; 								
	}

    if (oi->area)
    {
        vty_error_out(vty, "%s already attached to Area %s%s",
                oi->interface->name, oi->area->name, VNL);
        return CMD_SUCCESS;
    }

    /* parse Area-ID */
    if (inet_pton(AF_INET, argv[1], &area_id) != 1)
    {
        vty_error_out(vty, "Invalid Area-ID: %s%s", argv[1], VNL);
        return CMD_SUCCESS;
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


DEFUN(ospf6_interface_area,
      ospf6_interface_area_cmd,
      "interface (ethernet|gigabitethernet|xgigabitethernet|loopback) IFNAME area A.B.C.D",
      "Enable routing on an IPv6 interface\n"
      "Ethernet interface type\n"
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      "LoopBack interface\n"
      "Ethernet format: <0-7>/<0-31>/<1-255>[.<1-4095>],loopback <0-128>\n"
      "Specify the OSPF6 area ID\n"
      "OSPF6 area ID in IPv4 address notation\n"
     )
{
    struct ospf6 *o = NULL;
    struct ospf6_area *oa = NULL;
    struct ospf6_interface *oi = NULL;
    struct interface *ifp = NULL;
    u_int32_t area_id;
    o = (struct ospf6 *) vty->index;
    uint32_t ifindex = 0;
    int ret = -1;
    char if_name[IFNET_NAMESIZE];


    if (! strcmp(argv[0], "ethernet"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"ethernet", (char *)argv[1]);
    }
    else if (! strcmp(argv[0], "gigabitethernet"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"gigabitethernet", (char *)argv[1]);
    }
    else if (! strcmp(argv[0], "xgigabitethernet"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"xgigabitethernet", (char *)argv[1]);
    }
    else if (! strcmp(argv[0], "loopback"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"loopback", (char *)argv[1]);
    }
    else
    {

        return CMD_WARNING;
    }


    //ifindex = ifm_get_ifindex_by_name ((char *)"ethernet", (char *)argv[0] );
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


    /* find/create ospf6 interface */
    ifp = if_get_by_name(if_name);

    if (ifp == NULL)
    {
        vty_error_out(vty, "This interface not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (ifp->ifindex == 0)
    {
        ifp = ospf6_enable_interface_init1(ifindex);

        if (ifp == NULL)
        {
            vty_error_out(vty, "This interface not exist%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        ifp->ifindex = ifindex;
    }

    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        oi = ospf6_interface_create(o, ifp);
    }
	
	if(oi == NULL)											
	{														
		vty_error_out(vty, "GET ospf6_interface is NULL%s", VNL);	
		return CMD_WARNING; 								
	}

    if (oi->area)
    {
        vty_error_out(vty, "%s already attached to Area %s%s",
                oi->interface->name, oi->area->name, VNL);
        return CMD_SUCCESS;
    }

    /* parse Area-ID */
    if (inet_pton(AF_INET, argv[2], &area_id) != 1)
    {
        vty_error_out(vty, "Invalid Area-ID: %s%s", argv[2], VNL);
        return CMD_SUCCESS;
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

DEFUN(no_ospf6_interface_area,
      no_ospf6_interface_area_cmd,
      "no interface (ethernet|gigabitethernet|xgigabitethernet) IFNAME area A.B.C.D",
      NO_STR
      "Disable routing on an IPv6 interface\n"
      "Ethernet interface type\n"
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      IFNAME_STR
      "Specify the OSPF6 area ID\n"
      "OSPF6 area ID in IPv4 address notation\n"
     )
{
    struct ospf6_interface *oi;
    struct ospf6_area *oa;
    struct interface *ifp;
    u_int32_t area_id;
    int ret = -1;

    uint32_t ifindex = 0;
    char if_name[IFNET_NAMESIZE];

    if (! strcmp(argv[0], "ethernet"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"ethernet", (char *)argv[1]);
    }
    else if (! strcmp(argv[0], "gigabitethernet"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"gigabitethernet", (char *)argv[1]);
    }
    else if (! strcmp(argv[0], "xgigabitethernet"))
    {
        ifindex = ifm_get_ifindex_by_name((char *)"xgigabitethernet", (char *)argv[1]);
    }
    else
    {
        return CMD_WARNING;
    }

    if (ifindex == 0)
    {
        vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /*get the name of the interface*/
    ret = ifm_get_name_by_ifindex(ifindex , if_name);

    if (ret < 0)
    {
        vty_error_out(vty, "This interface not exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ifp = if_lookup_by_name(if_name);

    if (ifp == NULL)
    {
        vty_error_out(vty, "No such interface %s%s", argv[1], VNL);
        return CMD_WARNING;
    }

    oi = (struct ospf6_interface *) ifp->info;

    if (oi == NULL)
    {
        vty_error_out(vty, "Interface %s not enabled%s", ifp->name, VNL);
        return CMD_WARNING;
    }

    /* parse Area-ID */
    if (inet_pton(AF_INET, argv[1], &area_id) != 1)
    {
        vty_error_out(vty, "Invalid Area-ID: %s%s", argv[2], VNL);
        return CMD_WARNING;
    }

    /* Verify Area */
    if (oi->area == NULL)
    {
        vty_error_out(vty, "No such Area-ID: %s%s", argv[2], VNL);
        return CMD_SUCCESS;
    }

    if (oi->area->area_id != area_id)
    {
        vty_error_out(vty, "Wrong Area-ID: %s is attached to area %s%s",
                oi->interface->name, oi->area->name, VNL);
        return CMD_SUCCESS;
    }

    thread_execute(master_ospf6, interface_down, oi, 0);
    oa = oi->area;
    listnode_delete(oi->area->if_list, oi);
    oi->area = (struct ospf6_area *) NULL;

    /* Withdraw inter-area routes from this area, if necessary */
    if (oa->if_list->count == 0)
    {
        UNSET_FLAG(oa->flag, OSPF6_AREA_ENABLE);
        ospf6_abr_disable_area(oa);
    }

    return CMD_SUCCESS;
}

DEFUN(ospf6_stub_router_admin,
      ospf6_stub_router_admin_cmd,
      "stub-router administrative",
      "Make router a stub router\n"
      "Advertise inability to be a transit router\n"
      "Administratively applied, for an indefinite period\n")
{
    struct ospf6 *o = (struct ospf6 *) vty->index;
    struct listnode *node;
    struct ospf6_area *oa;

    if (!CHECK_FLAG(o->flag, OSPF6_STUB_ROUTER))
    {
        for (ALL_LIST_ELEMENTS_RO(o->area_list, node, oa))
        {
            OSPF6_OPT_CLEAR(oa->options, OSPF6_OPT_V6);
            OSPF6_OPT_CLEAR(oa->options, OSPF6_OPT_R);
            OSPF6_ROUTER_LSA_SCHEDULE(oa);
        }

        SET_FLAG(o->flag, OSPF6_STUB_ROUTER);
    }

    return CMD_SUCCESS;
}

DEFUN(no_ospf6_stub_router_admin,
      no_ospf6_stub_router_admin_cmd,
      "no stub-router administrative",
      NO_STR
      "Make router a stub router\n"
      "Advertise ability to be a transit router\n"
      "Administratively applied, for an indefinite period\n")
{
    struct listnode *node;
    struct ospf6_area *oa;
    struct ospf6 *o = (struct ospf6 *) vty->index;

    if (CHECK_FLAG(o->flag, OSPF6_STUB_ROUTER))
    {
        for (ALL_LIST_ELEMENTS_RO(o->area_list, node, oa))
        {
            OSPF6_OPT_SET(oa->options, OSPF6_OPT_V6);
            OSPF6_OPT_SET(oa->options, OSPF6_OPT_R);
            OSPF6_ROUTER_LSA_SCHEDULE(oa);
        }

        UNSET_FLAG(o->flag, OSPF6_STUB_ROUTER);
    }

    return CMD_SUCCESS;
}

DEFUN(ospf6_stub_router_startup,
      ospf6_stub_router_startup_cmd,
      "stub-router on-startup <5-86400>",
      "Make router a stub router\n"
      "Advertise inability to be a transit router\n"
      "Automatically advertise as stub-router on startup of OSPF6\n"
      "Time (seconds) to advertise self as stub-router\n")
{
    return CMD_SUCCESS;
}

DEFUN(no_ospf6_stub_router_startup,
      no_ospf6_stub_router_startup_cmd,
      "no stub-router on-startup",
      NO_STR
      "Make router a stub router\n"
      "Advertise inability to be a transit router\n"
      "Automatically advertise as stub-router on startup of OSPF6\n"
      "Time (seconds) to advertise self as stub-router\n")
{
    return CMD_SUCCESS;
}

DEFUN(ospf6_stub_router_shutdown,
      ospf6_stub_router_shutdown_cmd,
      "stub-router on-shutdown <5-86400>",
      "Make router a stub router\n"
      "Advertise inability to be a transit router\n"
      "Automatically advertise as stub-router before shutdown\n"
      "Time (seconds) to advertise self as stub-router\n")
{
    return CMD_SUCCESS;
}

DEFUN(no_ospf6_stub_router_shutdown,
      no_ospf6_stub_router_shutdown_cmd,
      "no stub-router on-shutdown",
      NO_STR
      "Make router a stub router\n"
      "Advertise inability to be a transit router\n"
      "Automatically advertise as stub-router before shutdown\n"
      "Time (seconds) to advertise self as stub-router\n")
{
    return CMD_SUCCESS;
}

static void
ospf6_show(struct vty *vty, struct ospf6 *o)
{
    struct listnode *n;
    struct ospf6_area *oa;
    char router_id[16], duration[32];
    struct timeval now, running, result;
    char buf[32], rbuf[32];
    /* process id, router id */
    inet_ntop(AF_INET, &o->router_id, router_id, sizeof(router_id));
    vty_out(vty, " OSPF6 Routing Instance (%d) with Router-ID %s%s",
            o->ospf_id, router_id, VNL);
    /* running time */
    time_get_time(TIME_CLK_MONOTONIC, &now);
    timersub(&now, &o->starttime, &running);
    timerstring(&running, duration, sizeof(duration));
    vty_out(vty, " Running %s%s", duration, VNL);
    /* Redistribute configuration */
    /* XXX */
    /* Show SPF parameters */
    vty_out(vty, " Initial SPF scheduling delay %d millisec(s)%s"
            " Minimum hold time between consecutive SPFs %d millsecond(s)%s"
            " Maximum hold time between consecutive SPFs %d millsecond(s)%s"
            " Hold time multiplier is currently %d%s",
            o->spf_delay, VNL,
            o->spf_holdtime, VNL,
            o->spf_max_holdtime, VNL,
            o->spf_hold_multiplier, VNL);
    vty_out(vty, " SPF algorithm ");

    if (o->ts_spf.tv_sec || o->ts_spf.tv_usec)
    {
        timersub(&now, &o->ts_spf, &result);
        timerstring(&result, buf, sizeof(buf));
        ospf6_spf_reason_string(o->last_spf_reason, rbuf, sizeof(rbuf));
        vty_out(vty, "last executed %s ago, reason %s%s", buf, rbuf, VNL);
        vty_out(vty, " Last SPF duration %lld sec %lld usec%s",
                (long long)o->ts_spf_duration.tv_sec,
                (long long)o->ts_spf_duration.tv_usec, VNL);
    }
    else
    {
        vty_out(vty, "has not been run$%s", VNL);
    }

    //threadtimer_string(now, o->t_spf_calc, buf, sizeof(buf));
    threadtimer_string_new(now, o->t_spf_calc, buf, sizeof(buf));
    vty_out(vty, " SPF timer %s%s%s",
            (o->t_spf_calc ? "due in " : "is "), buf, VNL);

    if (CHECK_FLAG(o->flag, OSPF6_STUB_ROUTER))
    {
        vty_out(vty, " Router Is Stub Router%s", VNL);
    }

    /* LSAs */
    vty_out(vty, " Number of AS scoped LSAs is %u%s",
            o->lsdb->count, VNL);
    /* Areas */
    vty_out(vty, " Number of areas in this router is %u%s",
            listcount(o->area_list), VNL);

    if (CHECK_FLAG(o->config_flags, OSPF6_LOG_ADJACENCY_CHANGES))
    {
        if (CHECK_FLAG(o->config_flags, OSPF6_LOG_ADJACENCY_DETAIL))
        {
            vty_out(vty, " All adjacency changes are logged%s", VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, " Adjacency changes are logged%s", VTY_NEWLINE);
        }
    }

    vty_out(vty, "%s", VTY_NEWLINE);

    for (ALL_LIST_ELEMENTS_RO(o->area_list, n, oa))
    {
        ospf6_area_show(vty, oa);
    }
}

/* show top level structures */
DEFUN(show_ipv6_ospf6,
      show_ipv6_ospf6_cmd,
      "show ipv6 ospf6 [<1-255>] instance",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      OSPF6_INSTANCE
      "OSPF6 Instance\n")
{
    struct ospf6 *o = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;

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

    ospf6_show(vty, o);
    return CMD_SUCCESS;
}

DEFUN(show_ipv6_ospf6_route,
      show_ipv6_ospf6_route_cmd,
      "show ipv6 ospf6 [<1-255>] route",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      OSPF6_INSTANCE
      ROUTE_STR
     )
{
    struct ospf6 *o = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;

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


    ospf6_route_table_show(vty, argc, argv, o->route_table);
    return CMD_SUCCESS;
}

ALIAS(show_ipv6_ospf6_route,
      show_ipv6_ospf6_route_detail_cmd,
      "show ipv6 ospf6 [<1-255>] route (X:X::X:X|X:X::X:X/M|detail|summary)",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      OSPF6_INSTANCE
      ROUTE_STR
      "Specify IPv6 address\n"
      "Specify IPv6 prefix\n"
      "Detailed information\n"
      "Summary of route table\n"
     )

DEFUN(show_ipv6_ospf6_route_match,
      show_ipv6_ospf6_route_match_cmd,
      "show ipv6 ospf6 [<1-255>]route X:X::X:X/M match",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      OSPF6_INSTANCE
      ROUTE_STR
      "Specify IPv6 prefix\n"
      "Display routes which match the specified route\n"
     )
{
    const char *sargv[CMD_ARGC_MAX];
    int i, sargc;
    struct ospf6 *o = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;

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

    /* copy argv to sargv and then append "match" */
    for (i = 0; i < argc; i++)
    {
        sargv[i] = argv[i];
    }

    sargc = argc;
    sargv[sargc++] = "match";
    sargv[sargc] = NULL;
    ospf6_route_table_show(vty, sargc, sargv, o->route_table);
    return CMD_SUCCESS;
}

DEFUN(show_ipv6_ospf6_route_match_detail,
      show_ipv6_ospf6_route_match_detail_cmd,
      "show ipv6 ospf6 [<1-255>] route X:X::X:X/M match detail",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      OSPF6_INSTANCE
      ROUTE_STR
      "Specify IPv6 prefix\n"
      "Display routes which match the specified route\n"
      "Detailed information\n"
     )
{
    const char *sargv[CMD_ARGC_MAX];
    int i, sargc;
    struct ospf6 *o = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;

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

    /* copy argv to sargv and then append "match" and "detail" */
    for (i = 0; i < argc; i++)
    {
        sargv[i] = argv[i];
    }

    sargc = argc;
    sargv[sargc++] = "match";
    sargv[sargc++] = "detail";
    sargv[sargc] = NULL;
    ospf6_route_table_show(vty, sargc, sargv, o->route_table);
    return CMD_SUCCESS;
}

ALIAS(show_ipv6_ospf6_route_match,
      show_ipv6_ospf6_route_longer_cmd,
      "show ipv6 ospf6 [<1-255>] route X:X::X:X/M longer",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      OSPF6_INSTANCE
      ROUTE_STR
      "Specify IPv6 prefix\n"
      "Display routes longer than the specified route\n"
     )

DEFUN(show_ipv6_ospf6_route_match_detail,
      show_ipv6_ospf6_route_longer_detail_cmd,
      "show ipv6 ospf6 [<1-255>] route X:X::X:X/M longer detail",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      OSPF6_INSTANCE
      ROUTE_STR
      "Specify IPv6 prefix\n"
      "Display routes longer than the specified route\n"
      "Detailed information\n"
     );

ALIAS(show_ipv6_ospf6_route,
      show_ipv6_ospf6_route_type_cmd,
      "show ipv6 ospf6 [<1-255>]route (intra-area|inter-area|external-1|external-2)",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      OSPF6_INSTANCE
      ROUTE_STR
      "Display Intra-Area routes\n"
      "Display Inter-Area routes\n"
      "Display Type-1 External routes\n"
      "Display Type-2 External routes\n"
     )

DEFUN(show_ipv6_ospf6_route_type_detail,
      show_ipv6_ospf6_route_type_detail_cmd,
      "show ipv6 ospf6 [<1-255>] route (intra-area|inter-area|external-1|external-2) detail",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      OSPF6_INSTANCE
      ROUTE_STR
      "Display Intra-Area routes\n"
      "Display Inter-Area routes\n"
      "Display Type-1 External routes\n"
      "Display Type-2 External routes\n"
      "Detailed information\n"
     )
{
    struct ospf6 *o = NULL;
    u_int16_t ospf_id = OSPF6_DEFAULT_PROCESS_ID;

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

    const char *sargv[CMD_ARGC_MAX];

    int i, sargc;

    /* copy argv to sargv and then append "detail" */
    for (i = 0; i < argc; i++)
    {
        sargv[i] = argv[i];
    }

    sargc = argc;
    sargv[sargc++] = "detail";
    sargv[sargc] = NULL;
    ospf6_route_table_show(vty, sargc, sargv, o->route_table);
    return CMD_SUCCESS;
}

static int
ospf6_get_lsa_num(struct ospf6* ospf6)
{
    struct ospf6_area *oa = NULL;
    struct ospf6_interface *oi = NULL;
    struct listnode *n = NULL, *i = NULL;
	int count = 0; 
	
    for (ALL_LIST_ELEMENTS_RO(ospf6->area_list, n, oa))
    {
    	count += oa->lsdb->count;
		for (ALL_LIST_ELEMENTS_RO(oa->if_list, i, oi))
		{
			count += oi->lsdb->count;
		}
    }
	count += ospf6->lsdb->count;
	return count;
}


DEFUN(show_ipv6_ospf6_statics,
      show_ipv6_ospf6_statics_cmd,
      "show ipv6 ospf6 statistics ",
      SHOW_STR
      IP6_STR
      OSPF6_STR
      "ospf packet,lsa,route statics\n")
{    
	struct ospf6 *ospf6 = NULL;
    struct listnode *node = NULL;
	if (listcount(om6->ospf6) == 0)
    {
        vty_error_out(vty, "There is no active ospf6 instance %s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

	vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "--------------------ospf6 packet---------------------%s", VTY_NEWLINE);

	vty_out(vty, "     send success:%-12d  send error:%-d%s", om6->ospf6_statics.send_all,
            om6->ospf6_statics.send_error, VTY_NEWLINE);
    vty_out(vty, "     recv success:%-12d  recv error:%-d%s", om6->ospf6_statics.recv_all,
        om6->ospf6_statics.recv_error, VTY_NEWLINE);
	vty_out(vty, "     hello,send  :%-12d  recv:%-d%s", om6->ospf6_statics.s_hel, om6->ospf6_statics.r_hel, VTY_NEWLINE);
    vty_out(vty, "     DD,   send  :%-12d  recv:%-d%s", om6->ospf6_statics.s_dd, om6->ospf6_statics.r_dd, VTY_NEWLINE);
    vty_out(vty, "     LSR,  send  :%-12d  recv:%-d%s", om6->ospf6_statics.s_lsr, om6->ospf6_statics.r_lsr, VTY_NEWLINE);
    vty_out(vty, "     LSU,  send  :%-12d  recv:%-d%s", om6->ospf6_statics.s_lsu, om6->ospf6_statics.r_lsu, VTY_NEWLINE);
    vty_out(vty, "     LSA,  send  :%-12d  recv:%-d%s", om6->ospf6_statics.s_lsa, om6->ospf6_statics.r_lsa, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);

    for (ALL_LIST_ELEMENTS_RO(om6->ospf6, node, ospf6))
    {
        int lsa_num = ospf6_get_lsa_num(ospf6);
        //int route_num = ospf_get_route_num(ospf6);
        vty_out(vty, "------------instance %d total lsa and route-----------%s%s", ospf6->ospf_id, VTY_NEWLINE, VTY_NEWLINE);
        vty_out(vty, "     lsa total   :%-12d", lsa_num);
        vty_out(vty, "  route total:%-d   %s%s", ospf6->route_table->count, VTY_NEWLINE, VTY_NEWLINE);
        vty_out(vty, "%s", VTY_NEWLINE);
   
    }

	
    return CMD_SUCCESS;
}


static void
ospf6_stub_router_config_write(struct ospf6 *ospf6, struct vty *vty)
{
    if (CHECK_FLAG(ospf6->flag, OSPF6_STUB_ROUTER))
    {
        vty_out(vty, " stub-router administrative%s", VNL);
    }

    return;
}

/* OSPF configuration write function. */
static int
config_write_ospf6(struct vty *vty)
{
    char router_id[16];
    //struct listnode *j = NULL, *k = NULL;
    //struct ospf6_area *oa = NULL;
    //struct ospf6_interface *oi = NULL;
    struct ospf6 *o = NULL;
    struct listnode *node = NULL, *nnode = NULL;
    /* OSPFv6 configuration. */
    //if (ospf6 == NULL)
    //{
    //  return CMD_SUCCESS;
    //}


    for (ALL_LIST_ELEMENTS(om6->ospf6, node, nnode, o))
    {
        inet_ntop(AF_INET, &o->router_id_static, router_id, sizeof(router_id));
        vty_out(vty, "ospf6 instance %d%s", o->ospf_id, VNL);

        if (o->router_id_static != 0)
        {
            vty_out(vty, " router-id %s%s", router_id, VNL);
        }
		
		if(o->distance_external)
		{
			vty_out(vty, " distance external %d %s", o->distance_external, VNL);
		}
		
		if(o->distance_all != ROUTE_METRIC_OSPF)
		{
			vty_out(vty, " distance %d %s", o->distance_all, VNL);
		}
        /* log-adjacency-changes flag print. */
        if (CHECK_FLAG(o->config_flags, OSPF6_LOG_ADJACENCY_CHANGES))
        {
            vty_out(vty, " log-adjacency-changes");

            if (CHECK_FLAG(o->config_flags, OSPF6_LOG_ADJACENCY_DETAIL))
            {
                vty_out(vty, " detail");
            }

            vty_out(vty, "%s", VTY_NEWLINE);
        }

        if (o->ref_bandwidth != OSPF6_REFERENCE_BANDWIDTH)
            vty_out(vty, " auto-cost reference-bandwidth %d%s", o->ref_bandwidth / 1000,
                    VNL);

        ospf6_stub_router_config_write(o, vty);
        ospf6_redistribute_config_write(o, vty);
        ospf6_area_config_write(vty, o);
        ospf6_spf_config_write(vty, o);
#if 0

        for (ALL_LIST_ELEMENTS_RO(ospf6->area_list, j, oa))
        {
            for (ALL_LIST_ELEMENTS_RO(oa->if_list, k, oi))
                vty_out(vty, " interface %s area %s%s",
                        oi->interface->name, oa->name, VNL);
        }

#endif
    }

    vty_out(vty, "!%s", VNL);
    return 0;
}

/* OSPF6 node structure. */
static struct cmd_node ospf6_node =
{
    OSPF6_NODE,
    "%s(config-ospf6)# ",
    1 /* VTYSH */
};

/* Install ospf related commands. */
void
ospf6_top_init(void)
{
    /* Install ospf6 top node. */
    install_node(&ospf6_node, config_write_ospf6);
    install_default(OSPF6_NODE);
    install_element(CONFIG_NODE, &router_ospf6_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_router_ospf6_cmd, CMD_SYNC);
	
	install_element(CONFIG_NODE, &show_ipv6_ospf6_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_route_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_route_detail_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_route_match_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_route_match_detail_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_route_longer_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_route_longer_detail_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_route_type_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ospf6_route_type_detail_cmd, CMD_LOCAL);	
	install_element(CONFIG_NODE, &show_ipv6_ospf6_statics_cmd, CMD_LOCAL);
	install_element(OSPF6_NODE, &show_ipv6_ospf6_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_route_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_route_detail_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_route_match_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_route_match_detail_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_route_longer_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_route_longer_detail_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_route_type_cmd, CMD_LOCAL);
    install_element(OSPF6_NODE, &show_ipv6_ospf6_route_type_detail_cmd, CMD_LOCAL);
	install_element(OSPF6_NODE, &show_ipv6_ospf6_cmd, CMD_LOCAL);
	
    install_element(OSPF6_NODE, &ospf6_router_id_cmd, CMD_SYNC);
	install_element(OSPF6_NODE, &distance_ospf6_cmd, CMD_SYNC);
	install_element(OSPF6_NODE, &no_distance_ospf6_cmd, CMD_SYNC);
    install_element(OSPF6_NODE, &ospf6_log_adjacency_changes_cmd, CMD_SYNC);
    install_element(OSPF6_NODE, &ospf6_log_adjacency_changes_detail_cmd, CMD_SYNC);
    install_element(OSPF6_NODE, &no_ospf6_log_adjacency_changes_cmd, CMD_SYNC);
    install_element(OSPF6_NODE, &no_ospf6_log_adjacency_changes_detail_cmd, CMD_SYNC);
    install_element(OSPF6_NODE, &ospf6_interface_area_cmd, CMD_SYNC);
    install_element(OSPF6_NODE, &ospf6_interface_ethernet_area_cmd, CMD_SYNC);
    install_element(OSPF6_NODE, &ospf6_interface_loopback_area_cmd, CMD_SYNC);
    install_element(OSPF6_NODE, &no_ospf6_interface_area_cmd, CMD_SYNC);
    install_element(OSPF6_NODE, &ospf6_stub_router_admin_cmd, CMD_SYNC);
    install_element(OSPF6_NODE, &no_ospf6_stub_router_admin_cmd, CMD_SYNC);
    /* For a later time
    install_element (OSPF6_NODE, &ospf6_stub_router_startup_cmd, CMD_SYNC);
    install_element (OSPF6_NODE, &no_ospf6_stub_router_startup_cmd, CMD_SYNC);
    install_element (OSPF6_NODE, &ospf6_stub_router_shutdown_cmd, CMD_SYNC);
    install_element (OSPF6_NODE, &no_ospf6_stub_router_shutdown_cmd, CMD_SYNC);
    */
}



