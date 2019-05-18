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

#include <zebra.h>

#include "lib/thread.h"
#include "lib/memory.h"
#include "lib/linklist.h"
#include "lib/prefix.h"
#include "lib/if.h"
#include "lib/table.h"
#include "lib/filter.h"
#include "lib/log.h"
#include "lib/route_com.h"
#include "lib/memtypes.h"

#include "ospfd/ospfd.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_spf.h"
#include "ospfd/ospf_flood.h"
#include "ospfd/ospf_route.h"
#include "ospfd/ospf_zebra.h"
#include "ospfd/ospf_dump.h"


static struct ospf_route_redist*
ospf_route_policy_new(uint32_t ifindex, char *if_name)
{
    struct ospf_route_redist *ospf_route_redist = NULL;
	
	do
  	{		
		ospf_route_redist = XCALLOC (MTYPE_OSPF_ROUTE_POLICY, sizeof (struct ospf_route_redist));	
			  		
  		if (ospf_route_redist == NULL)
  		{
    		zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
  		}
  	}while(ospf_route_redist == NULL);
	
	if (ospf_route_redist == NULL)
    {
        zlog_err("Can't creat ospf_route_redist : malloc failed");
        return (struct ospf_route_redist *)NULL;
    }
	
	memset(ospf_route_redist, 0, sizeof(struct ospf_route_redist));
    ospf_route_redist->ifindex = ifindex;
    memcpy(ospf_route_redist->if_name, if_name, IFNET_NAMESIZE);
    return ospf_route_redist;
}


struct ospf_route_redist*
ospf_route_policy_look(struct ospf* ospf, uint32_t ifindex)
{
    struct listnode *node = NULL;
    struct ospf_route_redist *ospf_route_redist = NULL;

	if(ospf == NULL || ospf->redist_name == NULL)
	{
		zlog_warn("%s[%d]  ospf or ospf->redist_name is NULL",__func__,__LINE__);
		return NULL;		
	}
	
    assert (ospf);
    assert (ospf->redist_name);
	
    if(list_isempty(ospf->redist_name))
    {
        return NULL;
    }
	
    for (ALL_LIST_ELEMENTS_RO (ospf->redist_name, node, ospf_route_redist))
    {
        if (ospf_route_redist->ifindex == ifindex)
        {
            return ospf_route_redist;
        }
    }
	
    return NULL;

}
void
ospf_route_policy_free (struct ospf_route_redist *ospf_route_redist)
{
    XFREE (MTYPE_OSPF_ROUTE_POLICY, ospf_route_redist);
}

void
ospf_route_policy_list_free(struct ospf* ospf)
{
    struct listnode *node = NULL;
	struct listnode *nnode = NULL;
    struct ospf_route_redist *ospf_route_redist = NULL;

	if(ospf == NULL || ospf->redist_name == NULL)
	{
		zlog_warn("%s[%d]  ospf or ospf->redist_name is NULL",__func__,__LINE__);
		return ;		
	}
    assert (ospf);
    assert (ospf->redist_name);
	
    if(list_isempty(ospf->redist_name))
    {
        return ;
    }
	
    for (ALL_LIST_ELEMENTS (ospf->redist_name, node, nnode, ospf_route_redist))
    {
        listnode_delete (ospf->redist_name, ospf_route_redist);
        ospf_route_policy_free(ospf_route_redist);
        ospf_route_redist = NULL;
    }
}

struct ospf_route_redist*
ospf_route_policy_get(struct ospf *ospf, uint32_t ifindex, char *if_name)
{
    struct ospf_route_redist *ospf_route_redist = NULL;

	if(ospf == NULL || ospf->redist_name == NULL)
	{
		zlog_warn("%s[%d]  ospf or ospf->redist_name is NULL",__func__,__LINE__);
		return NULL;		
	}
	
    assert (ospf);
    assert (ospf->redist_name);

    if((ospf_route_redist = ospf_route_policy_look(ospf, ifindex)) != NULL)
    {
        return ospf_route_redist;
    }
	
    ospf_route_redist = ospf_route_policy_new(ifindex, if_name);
    listnode_add(ospf->redist_name, ospf_route_redist);

    return ospf_route_redist;
}

#if 0
/* Remove external route. */
static void
ospf_external_route_remove (struct ospf *ospf, struct prefix_ipv4 *p)
{
    struct route_node *rn;
    struct ospf_route *or;
    rn = route_node_lookup (ospf->old_external_route, (struct prefix *) p);
    if (rn)
    {
        if ((or = rn->info))
        {
            zlog_debug ("Route[%s/%d]: external path deleted",
                       inet_ntoa (p->prefix), p->prefixlen);
            /* Remove route from zebra. */
            if (or->type == OSPF_DESTINATION_NETWORK)
            {
                ospf_zebra_delete ((struct prefix_ipv4 *) &rn->p, or,ospf);
            }
            ospf_route_free (or);
            rn->info = NULL;
            route_unlock_node (rn);
            route_unlock_node (rn);
            return;
        }
    }
    zlog_debug ("Route[%s/%d]: no such external path",
               inet_ntoa (p->prefix), p->prefixlen);
}

/* Lookup external route. */
static struct ospf_route *
ospf_external_route_lookup (struct ospf *ospf,
                            struct prefix_ipv4 *p)
{
    struct route_node *rn;

    rn = route_node_lookup (ospf->old_external_route, (struct prefix *) p);
    if (rn)
    {
        route_unlock_node (rn);
        if (rn->info)
        {
            return rn->info;
        }
    }

    zlog_warn ("Route[%s/%d]: lookup, no such prefix",
               inet_ntoa (p->prefix), p->prefixlen);

    return NULL;
}
#endif


/* Add an External info for AS-external-LSA. */
static struct external_info *
ospf_external_info_new (u_char type, u_int8_t instance)
{
    struct external_info *new_ei = NULL;

	do
  	{		
		new_ei = (struct external_info *)
			  XCALLOC (MTYPE_OSPF_EXTERNAL_INFO, sizeof (struct external_info));

  		if (new_ei == NULL)
  		{
    		zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
  		}
  	}while(new_ei == NULL);

	if (new_ei == NULL)
    {
        zlog_err("Can't creat ospf_external_info : malloc failed");
        return (struct external_info *)NULL;
    }
	
	memset(new_ei, 0, sizeof(struct external_info));
    new_ei->type = type;
    new_ei->instance = instance;

    ospf_reset_route_map_set_values (&new_ei->route_map_set);
    return new_ei;
}

static int
ospf_external_info_free (struct external_info *ei, u_int8_t instance)
{
    if(instance == ei->instance)
    {
        XFREE (MTYPE_OSPF_EXTERNAL_INFO, ei);
        return 0;
    }
    return -1;
}

void
ospf_reset_route_map_set_values (struct route_map_set_values *values)
{
    values->metric = -1;
    values->metric_type = -1;
}

int
ospf_route_map_set_compare (struct route_map_set_values *values1,
                            struct route_map_set_values *values2)
{
    return values1->metric == values2->metric &&
           values1->metric_type == values2->metric_type;
}

static void
ospf_info_add (struct route_node *rn, struct external_info *ei)
{
    struct external_info *top = NULL;
	
    top = rn->info;
    ei->next = rn->info;
    ei->prev = NULL;
    if (top)
    {
        top->prev = ei;
    }
    rn->info = ei;
}

static struct external_info *
ospf_info_lookup (struct route_node *rn, struct external_info *ei1)
{
    struct external_info *ei = NULL;
	
    if(rn != NULL)
    {
        for (ei = rn->info; ei; ei = ei->next)
        {
            if(ei1 == ei)
            {
                return ei;
            }
        }
    }
    return NULL;
}


static struct external_info *
ospf_info_delete (struct route_node *rn, struct external_info *ei)
{
    struct external_info *ei1 = NULL;
	
    ei1 = ospf_info_lookup (rn, ei);
    if(ei1 == NULL)
    {
        return NULL;
    }

    if(ei1->next)
    {
        ei1->next->prev = ei1->prev;
    }

    if(ei1->prev)
    {
        ei1->prev->next = ei1->next;
    }
    else
    {
        rn->info = ei1->next;
    }

    return rn->info;
}


/* Add an External info for AS-external-LSA. */
struct external_info *
ospf_external_info_add (uint16_t vpn, u_char type, u_int8_t instance, struct prefix_ipv4 p,
                        ifindex_t ifindex, struct in_addr nexthop,
                       route_tag_t tag)
{
    struct route_node *rn = NULL;
    struct external_info *ei = NULL;
    struct external_info *new_ei = NULL;

    /* Initialize route table. */
    if (EXTERNAL_INFO (vpn, type) == NULL)
    {
        EXTERNAL_INFO (vpn, type) = route_table_init ();
    }

    /* first look up if the ei exist*/
    ei = ospf_external_info_lookup (vpn, type, instance, &p, nexthop);
    if(ei != NULL)
    {
        return ei;
    }

    rn = route_node_get (EXTERNAL_INFO (vpn, type), (struct prefix *) &p);
    /* If old info exists, -- discard new one or overwrite with new one? */

    /* If old info exists, --  add the new one */
    if(rn)
    {
        if (rn->info)
        {
            route_unlock_node (rn);
        }
    }

    /* Create new External info instance. */
    new_ei = ospf_external_info_new (type, instance);
    new_ei->p = p;
    new_ei->ifindex = ifindex;
    new_ei->nexthop = nexthop;
    new_ei->tag = 0;
    new_ei->flags = 0;
    new_ei->tag = tag;

    if (rn)
    {
        ospf_info_add(rn, new_ei);
    }

    if (IS_DEBUG_OSPF (lsa, LSA_GENERATE))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "Redistribute[%s]: %s/%d external info created.",ospf_redist_string(type),
                    inet_ntoa (p.prefix), p.prefixlen);
    }
    return new_ei;
}

void
ospf_external_info_delete (uint16_t vpn, u_char type, u_int8_t instance,
                           struct prefix_ipv4 p, struct in_addr nexthop)
{
    struct route_node *rn = NULL;
    struct external_info *ei = NULL;

	if(EXTERNAL_INFO (vpn, type) == NULL)
	{
		zlog_warn("%s[%d]  EXTERNAL_INFO (vpn, type) is NULL",__func__,__LINE__);
		return ;		
	}
	
    assert(EXTERNAL_INFO (vpn, type));
    rn = route_node_lookup (EXTERNAL_INFO (vpn, type), (struct prefix *) &p);
    if (rn)
    {
        route_unlock_node (rn);
        ei = ospf_external_info_lookup(vpn, type, instance, &p, nexthop);
        if(ei != NULL)
        {
            ospf_info_delete (rn, ei);
            ospf_external_info_free (ei, instance);
            if(rn->info == NULL)
            {
                route_unlock_node (rn);
            }
        }
    }
}

struct external_info *
ospf_external_info_lookup (uint16_t vpn, u_char type, u_int8_t instance,
                           struct prefix_ipv4 *p, struct in_addr nexthop)
{
    struct route_node *rn = NULL;
    struct external_info *ei = NULL;
	
    if(EXTERNAL_INFO (vpn, type) == NULL)
    {
        return NULL;
    }
	
    rn = route_node_lookup (EXTERNAL_INFO (vpn, type), (struct prefix *) p);
    if (rn)
    {
        route_unlock_node (rn);
        for(ei = rn->info; ei != NULL; ei = ei->next)
        {
            if(ei->type == ROUTE_PROTO_OSPF)
            {
                if(ei->flags == ei->instance && ei->nexthop.s_addr == nexthop.s_addr)
                {
                    return ei;
                }
            }
			
            if(ei->instance == instance && ei->nexthop.s_addr == nexthop.s_addr)
            {
                return ei;
            }
        }
    }

    return NULL;
}

struct external_info *
ospf_external_info_lookup_by_prefix (struct ospf *ospf, struct prefix_ipv4 *p,
                                     struct in_addr nexthop)
{
    int type,instance;
    struct external_info *ei = NULL;
	
    for (type = 0; type <= ROUTE_PROTO_MAX; type++)
    {
        int redist_type = is_prefix_default (p) ? DEFAULT_ROUTE : type;
        for(instance = 0; instance <= OSPF_MAX_PROCESS_ID; instance++)
        {
            if (ospf_is_type_redistributed (ospf, redist_type, instance)||
                    (type == ROUTE_PROTO_OSPF && instance == ospf->ospf_id))
            {
                if (EXTERNAL_INFO (ospf->vpn, type))
                {
                    ei = ospf_external_info_lookup (ospf->vpn, redist_type, instance, p, nexthop);
                    if(ei != NULL)
                    {
                        if (ospf_is_type_redistributed (ospf,ei->type,ei->instance))
                        {
                            if(ei->p.prefix.s_addr == p->prefix.s_addr)
                            {
                                return ei;
                            }
                        }
                        else if(ei->type == ROUTE_PROTO_OSPF)
                        {
                            if(ei->p.prefix.s_addr == p->prefix.s_addr)
                            {
                                if(ei->flags == ei->instance)
                                {
                                    return ei;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return NULL;
}

struct ospf_lsa *
ospf_external_info_find_lsa_specific (struct ospf *ospf,
                             struct prefix_ipv4 *p)
{
    struct in_addr mask, id;
    struct ospf_lsa *lsa = NULL;
    struct as_external_lsa *al = NULL;

    lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, OSPF_AS_EXTERNAL_LSA,
                                  p->prefix, ospf->router_id);

    if (!lsa)
    {
        masklen2ip (p->prefixlen, &mask);
        id.s_addr = p->prefix.s_addr | (~mask.s_addr);
        lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, OSPF_AS_EXTERNAL_LSA,
                                      id, ospf->router_id);
        if (!lsa)
        {
            return NULL;
        }
		
        al = (struct as_external_lsa *) lsa->data;
        masklen2ip (p->prefixlen, &mask);
        if (mask.s_addr != al->mask.s_addr)
        {
            return NULL;
        }
    }
    else
    {
        al = (struct as_external_lsa *) lsa->data;
		
        masklen2ip (p->prefixlen, &mask);
        if (mask.s_addr != al->mask.s_addr)
        {
            id.s_addr = p->prefix.s_addr | (~mask.s_addr);

            lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, OSPF_AS_EXTERNAL_LSA,
                                          id, ospf->router_id);
            if (!lsa)
            {
                return NULL;
            }
        }
    }

    return lsa;
}

struct ospf_lsa *
ospf_external_info_find_lsa_summary (struct ospf *ospf,
                             struct prefix_ipv4 *p)
{
    struct in_addr mask, id;
    struct ospf_lsa *lsa = NULL;
    struct as_external_lsa *al = NULL;

    lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, OSPF_AS_EXTERNAL_LSA,
                                  p->prefix, ospf->router_id);

    if (!lsa)
    {
        masklen2ip (p->prefixlen, &mask);
        id.s_addr = p->prefix.s_addr | (~mask.s_addr);
        lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, OSPF_AS_EXTERNAL_LSA,
                                      id, ospf->router_id);
        if (!lsa)
        {
            return NULL;
        }
        al = (struct as_external_lsa *) lsa->data;
        masklen2ip (p->prefixlen, &mask);
        if (mask.s_addr != al->mask.s_addr)
        {
            return NULL;
        }
    }
    else
    {
        al = (struct as_external_lsa *) lsa->data;
        if(IS_LSA_MAXAGE(lsa))
        {
            return lsa;
        }
		
		if (!CHECK_FLAG (lsa->flags, OSPF_LSA_APPROVED))
		{
			return lsa;
		}
		
        masklen2ip (p->prefixlen, &mask);
        if (mask.s_addr != al->mask.s_addr)
        {
            id.s_addr = p->prefix.s_addr | (~mask.s_addr);
            /*
            附录E的第二个假设条件
            */
            if (id.s_addr == p->prefix.s_addr)
            {
                return NULL;
            }
            lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, OSPF_AS_EXTERNAL_LSA,
                                          id, ospf->router_id);
            if (!lsa)
            {
                return NULL;
            }
        }
    }

    return lsa;
}

struct ospf_lsa *
ospf_external_info_find_lsa (struct ospf *ospf,
                             struct prefix_ipv4 *p)
{
    struct in_addr mask, id;
    struct ospf_lsa *lsa = NULL;
    struct as_external_lsa *al = NULL;

    lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, OSPF_AS_EXTERNAL_LSA,
                                  p->prefix, ospf->router_id);

    if (!lsa)
    {
        masklen2ip (p->prefixlen, &mask);
        id.s_addr = p->prefix.s_addr | (~mask.s_addr);
        lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, OSPF_AS_EXTERNAL_LSA,
                                      id, ospf->router_id);
        if (!lsa)
        {
            return NULL;
        }
		
        al = (struct as_external_lsa *) lsa->data;
        masklen2ip (p->prefixlen, &mask);
        if (mask.s_addr != al->mask.s_addr)
        {
            return NULL;
        }
    }
    else
    {
        al = (struct as_external_lsa *) lsa->data;
        if(IS_LSA_MAXAGE(lsa))
        {
            return lsa;
        }
		
        masklen2ip (p->prefixlen, &mask);
        if (mask.s_addr != al->mask.s_addr)
        {
            id.s_addr = p->prefix.s_addr | (~mask.s_addr);
            /*
            附录E的第二个假设条件
            */
            if (id.s_addr == p->prefix.s_addr)
            {
                return NULL;
            }
			
            lsa = ospf_lsdb_lookup_by_id (ospf->lsdb, OSPF_AS_EXTERNAL_LSA,
                                          id, ospf->router_id);
            if (!lsa)
            {
                return NULL;
            }
        }
    }

    return lsa;
}

struct ospf_lsa *
ospf_as_nssa_info_find_lsa (struct ospf *ospf,
                            struct prefix_ipv4 *p)
{
    struct ospf_lsa *lsa = NULL;
    struct listnode *node = NULL;
    struct ospf_area *area = NULL;

    for (ALL_LIST_ELEMENTS_RO (ospf->areas, node, area))
    {
        if ( (area->external_routing != OSPF_AREA_NSSA))
        {
            continue;
        }
        lsa = ospf_lsa_lookup_by_prefix (area->lsdb, OSPF_AS_NSSA_LSA,
                                         p, area->ospf->router_id);
        if (lsa)
        {
            break;
        }
    }

    return lsa;
}


/* Update ASBR status. */
void
ospf_asbr_status_update (struct ospf *ospf, u_char status)
{
    zlog_debug (OSPF_DBG_EVENT, "ASBR[Status:%d]: Update", status);
	
    /* ASBR on. */
    if (status)
    {
        /* Already ASBR. */
        if (IS_OSPF_ASBR (ospf))
        {
            zlog_debug (OSPF_DBG_EVENT, "ASBR[Status:%d]: Already ASBR", status);
            return;
        }
        SET_FLAG (ospf->flags, OSPF_FLAG_ASBR);
    }
    else
    {
        /* Already non ASBR. */
        if (! IS_OSPF_ASBR (ospf))
        {
            zlog_debug (OSPF_DBG_EVENT, "ASBR[Status:%d]: Already non ASBR", status);
            return;
        }
        UNSET_FLAG (ospf->flags, OSPF_FLAG_ASBR);
    }
	
    /* Transition from/to status ASBR, schedule timer. */
    ospf_spf_calculate_schedule (ospf, SPF_FLAG_ASBR_STATUS_CHANGE);
    ospf_router_lsa_update (ospf);
}

void
ospf_redistribute_withdraw (struct ospf *ospf, u_char type ,u_int8_t instance)
{
    struct route_node *rn = NULL;
	struct route_node *rn1 = NULL;
    struct external_info *ei = NULL;
	struct external_info *ei1 = NULL;
    
    /* Delete external info for specified type. */
    if (EXTERNAL_INFO (ospf->vpn, type))
    {
        for (rn = route_top (EXTERNAL_INFO (ospf->vpn, type)); rn; rn = rn1)
        {
            rn1 = route_next (rn);
            for(ei = rn->info ; ei != NULL ; ei = ei1)
            {
                ei1 = ei->next;
                if(ei->instance == instance)
                {
                    if (ospf_external_info_find_lsa (ospf, &ei->p))
                    {
                        if (is_prefix_default (&ei->p) &&
                                ospf->default_originate != DEFAULT_ORIGINATE_NONE)
                        {
                            continue;
                        }
                        if(ei->prev == NULL && ei->next == NULL)
                        {
                            ospf_external_lsa_flush (ospf, type, &ei->p,
                                                     ei->ifindex /*, ei->nexthop */);
                        }
                    }
                    if(om->redis_count[type] == 0)
                    {
                        ospf_external_info_delete (ospf->vpn, ei->type, ei->instance, ei->p, ei->nexthop);
                    }
                }
            }
        }
    }
}

static void
ospf_asbr_unapprove_external (struct ospf *ospf)
{
    struct ospf_lsa *lsa = NULL;
    struct route_node *rn = NULL;
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_unapprove_external(): Start");
    }
	
    if (ospf_lsdb_count (ospf->lsdb, OSPF_AS_EXTERNAL_LSA) == 0)
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_unapprove_external(): External lsdb are not yet ready");
        }
        return ;
    }
	
    LSDB_LOOP (EXTERNAL_LSDB (ospf), rn, lsa)
    {
	    if (ospf_lsa_is_self_originated (ospf, lsa)
	            && !CHECK_FLAG (lsa->flags, OSPF_LSA_LOCAL_XLT)
	            &&!CHECK_FLAG (lsa->flags, OSPF_LSA_DISCARD)
	       )
	    {
	        if (IS_DEBUG_OSPF_EVENT)
	        {
	            zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_unapprove_external(): "
	                        "approved unset on external link id %s",
	                        inet_ntoa (lsa->data->id));
	        }
			
	        UNSET_FLAG (lsa->flags, OSPF_LSA_APPROVED);
	    }
    }
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_unapprove_external(): Stop");
    }
}

static void
ospf_asbr_prepare_aggregates (struct ospf *ospf)
{
    struct route_node *rn = NULL;
    struct ospf_summary *summary = NULL;
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_prepare_aggregates(): Start");
    }
	
    for (rn = route_top (ospf->summary); rn; rn = route_next (rn))
    {
        if ((summary = rn->info) != NULL)
        {
            summary->cost = -1;
            summary->specifics = 0;
            summary->type = -1;
        }
    }
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_prepare_aggregates(): Stop");
    }
}


static void
ospf_asbr_announce_external_lsa (struct prefix_ipv4 *p,
                                 struct ospf *ospf, struct external_info *ei)
{
    struct ospf_lsa *lsa = NULL;
    struct ospf_lsa *old = NULL;
    struct ospf_lsa *type7 = NULL;
	struct ospf_lsa *lsa_tmp = NULL;
    struct as_external_lsa *al = NULL;
    struct external_info *ei_temp = NULL;
	
    if (IS_DEBUG_OSPF_EVENT)
	{
        zlog_debug (OSPF_DBG_EVENT, "%s(%d): Start",__FUNCTION__,__LINE__);
    }
	
    old = ospf_external_info_find_lsa (ospf,p);
	
    if (old)
    {
        al = (struct as_external_lsa *) old->data;
		
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "%s(%d) old external found: old metric: %d, new metric: %d, adver: %s",__FUNCTION__,__LINE__,
                        GET_METRIC (al->e[0].metric), metric_value (ospf, ei->type,ei->instance),\
                        inet_ntoa(al->header.adv_router));
			zlog_debug (OSPF_DBG_EVENT, "%s(%d) old external found: fwd_addr: %s",__FUNCTION__,__LINE__,
                        		inet_ntoa(al->e[0].fwd_addr));
        }
		
        /* check if the old lsa'external info exist , */
        ei_temp = ospf_external_info_check (ospf,old);
        if(((old->flags & OSPF_LSA_IN_MAXAGE) == 0)
                && ((int)(GET_METRIC (al->e[0].metric)) ==  (metric_value (ospf, ei->type,ei->instance)))
                && ((IS_EXTERNAL_METRIC (al->e[0].tos) ? EXTERNAL_METRIC_TYPE_2 : EXTERNAL_METRIC_TYPE_1)
                    == (metric_type(ospf, ei->type,ei->instance)))
                &&(ei_temp != NULL) && (ip_masklen(al->mask) == p->prefixlen))
        {
            SET_FLAG (old->flags, OSPF_LSA_APPROVED);
            /* unchanged. simply reapprove it */
            if (IS_DEBUG_OSPF_EVENT)
            {
                zlog_debug (OSPF_DBG_EVENT, "%s(%d):old external approved",__FUNCTION__,__LINE__);
            }
        }
        else
        {
            /* LSA is changed, refresh it */
            if (IS_DEBUG_OSPF_EVENT)
            {
                zlog_debug (OSPF_DBG_EVENT, "%s(%d): "
                            "refreshing external",__FUNCTION__,__LINE__);
            }
			
            if( (int) GET_METRIC (al->e[0].metric) !=  metric_value (ospf, ei->type,ei->instance))
            {
                if (IS_DEBUG_OSPF_EVENT)
                {
                    zlog_debug (OSPF_DBG_EVENT, "%s(%d):refreshing external",__FUNCTION__,__LINE__);    /*change the lsa metric*/
                }
            }
			
			/* ABR router if any nssa area attach ,should consider the type-7 lsa*/
            if(ospf->anyNSSA > 0 && IS_OSPF_ABR(ospf))
            {
                /* Originate or refresh Type 7 LSA */
                type7 = ospf_as_nssa_info_find_lsa(ospf, &ei->p);
                if(type7)
                {
                    ospf_nssa_lsa_refresh (ospf, type7, ei, LSA_REFRESH_FORCE);
                }
                else
                {
                    ospf_nssa_lsa_orignate_only(ospf, ei);
                }
				
				/* find the highest priority type-7 lsa  to refresh */
                lsa_tmp = ospf_find_nssa_lsa_highestprio(ospf, &ei->p.prefix);
                if (lsa_tmp && IS_LSA_SELF(lsa_tmp))
                {
                    /* highest priority is self,then refresh Type 5 ONLY*/
                    lsa = ospf_external_lsa_refresh_only(ospf, old, ei, LSA_REFRESH_FORCE);
                    if(lsa)
                    {
                        SET_FLAG (lsa->flags, OSPF_LSA_APPROVED);
                    }
                }
                else if (lsa_tmp && !IS_LSA_SELF(lsa_tmp))
                {
                    /* highest priority is NOT self,then refresh Type 7 LSA ONLY*/
                    SET_FLAG (old->flags, OSPF_LSA_APPROVED);
                }
            }
            else
            {
                lsa = ospf_external_lsa_refresh (ospf, old, ei, LSA_REFRESH_FORCE);
            }
            if (!lsa)
            {
                char buf[INET_ADDRSTRLEN + 3]; /* ipv4 and /XX */
                prefix2str ((struct prefix *) p, buf, sizeof(buf));
                zlog_warn ("%s %d: Could not refresh %s",__func__,__LINE__,buf);
                return;
            }
            SET_FLAG (lsa->flags, OSPF_LSA_APPROVED);
            /* This will flood through area. */
        }
    }
    else
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "%s(%d): creating new external",__FUNCTION__,__LINE__);
        }
		
        if (ei)
        {
            lsa = ospf_external_lsa_originate (ospf,ei);
        }
		
        /* This will flood through area. */
        if (!lsa)
        {
            char buf[INET_ADDRSTRLEN + 3]; /* ipv4 and /XX */
            prefix2str ((struct prefix *)p, buf, sizeof(buf));
            zlog_warn ("%s[%d]: Could not originate %s",__func__,__LINE__,buf);
            return;
        }
		
        SET_FLAG (lsa->flags, OSPF_LSA_APPROVED);
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_announce_external_to_area(): "
                        "flooding new version of external");
        }
    }
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_announce_external_to_area(): Stop");
    }
}


/* 聚合LSA生成、刷新代码 */
static void
ospf_asbr_announce_summary_external (struct prefix_ipv4 *p, struct ospf_summary *summary,
                                     struct ospf *ospf, struct external_info *ei)
{
    struct ospf_lsa *lsa = NULL;
    struct ospf_lsa *old = NULL;
	struct as_external_lsa *al = NULL;
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "%s(%d): Start",__FUNCTION__,__LINE__);
    }
	
    old = ospf_external_info_find_lsa_summary (ospf,p);
    if (old)
    {
        al = (struct as_external_lsa *)old->data;
		
        if(CHECK_FLAG(old->flags, OSPF_LSA_IN_MAXAGE)
			    || ((int)(GET_METRIC (al->e[0].metric)) !=  summary->cost)
                || ((IS_EXTERNAL_METRIC (al->e[0].tos) ? EXTERNAL_METRIC_TYPE_2 : EXTERNAL_METRIC_TYPE_1)
                    != summary->type)
                ||(ip_masklen(al->mask) != p->prefixlen))
        {
            /* unchanged. simply reapprove it */
            if (IS_DEBUG_OSPF_EVENT)
            {
                zlog_debug (OSPF_DBG_EVENT, "%s(%d):old external approved",__FUNCTION__,__LINE__);
            }
			
            //ospf->dmetric[ROUTE_PROTO_OSPF][ospf->ospf_id].value = summary->cost;
            //ospf->dmetric[ROUTE_PROTO_OSPF][ospf->ospf_id].type = summary->type;
            lsa = ospf_external_lsa_refresh (ospf, old, ei, LSA_REFRESH_FORCE);
			
            /* This will flood through area. */
            if (!lsa)
            {
                char buf[INET_ADDRSTRLEN + 3]; /* ipv4 and /XX */
                prefix2str ((struct prefix *) p, buf, sizeof(buf));
                zlog_warn ("%s %d: Could not refresh %s",__func__,__LINE__,buf);
                return;
            }
            SET_FLAG (lsa->flags, OSPF_LSA_APPROVED);
        }
        else
        {
            SET_FLAG (old->flags, OSPF_LSA_APPROVED);
        }
    }
    else
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "%s(%d): creating new external",__FUNCTION__,__LINE__);
        }
		
        //ospf->dmetric[ROUTE_PROTO_OSPF][ospf->ospf_id].value = summary->cost;
        //ospf->dmetric[ROUTE_PROTO_OSPF][ospf->ospf_id].type = summary->type;
		
        lsa = ospf_external_lsa_originate (ospf,ei);
        /* This will flood through area. */
        if (!lsa)
        {
            char buf[INET_ADDRSTRLEN + 3]; /* ipv4 and /XX */
            prefix2str ((struct prefix *)p, buf, sizeof(buf));
            zlog_warn ("%s[%d]: Could not originate %s",__func__,__LINE__,buf);
            return;
        }
		
        SET_FLAG (lsa->flags, OSPF_LSA_APPROVED);
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "%s(): flooding new version of external",__func__);
        }
		
    }
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "%s %d: Stop",__func__,__LINE__);
    }
}


struct ospf_summary *
ospf_summary_match (struct ospf *ospf, struct prefix_ipv4 *p)
{
    struct route_node *node = NULL;

    node = route_node_match (ospf->summary, (struct prefix *) p);
    if (node)
    {
        route_unlock_node (node);
        return node->info;
    }
    return NULL;
}


static void
ospf_asbr_update_aggregate (struct ospf_summary *summary,
                            struct external_info *ei, struct ospf *ospf)
{
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "%s(%d): Start",__FUNCTION__,__LINE__);
    }
	
    /* if the cost is configured ,used the config cost */
    if (summary->cost_config != OSPF_SUMMARY_COST_UNSPEC)
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "%s(%d): use configured cost %d",__FUNCTION__,__LINE__,summary->cost_config);
        }
		
        summary->cost = summary->cost_config;
    }
    else        /* type2:MAX cost+1 > type1:cost */
    {
        if (summary->specifics == 0)/* 1st time get 1st cost */
        {
            if (IS_DEBUG_OSPF_EVENT)
            {
                zlog_debug (OSPF_DBG_EVENT, "%s(%d): use ospf->dmetric[%d][%d].value %d,type:%d",__FUNCTION__,__LINE__,
					ei->type,ei->instance, metric_value (ospf, ei->type,ei->instance)
					,metric_type (ospf, ei->type,ei->instance)+1);
            }
			
            if(metric_type (ospf, ei->type,ei->instance) == EXTERNAL_METRIC_TYPE_2)
            {
                summary->cost = metric_value (ospf, ei->type,ei->instance) + 1;
            }
            else if(metric_type (ospf, ei->type,ei->instance) == EXTERNAL_METRIC_TYPE_1)
            {
                summary->cost = metric_value (ospf, ei->type,ei->instance);
            }
			
        }
        else if(metric_type (ospf, ei->type,ei->instance) == EXTERNAL_METRIC_TYPE_2)
        {
            /* When find the route is type 2 */
            if (IS_DEBUG_OSPF_EVENT)
            {
                zlog_debug (OSPF_DBG_EVENT, "%s(%d): use ospf->dmetric[%d][%d].value %d,type:%d",__FUNCTION__,__LINE__,
					ei->type,ei->instance, metric_value (ospf, ei->type,ei->instance)
					,metric_type (ospf, ei->type,ei->instance)+1);
            }
			
            /*if the last time is type 1,the route is type 2,select the type2 cost,cost is cost+1*/
            if(summary->type == EXTERNAL_METRIC_TYPE_1)
            {
                summary->cost = metric_value (ospf, ei->type,ei->instance) + 1;
            }
            else if(summary->type == EXTERNAL_METRIC_TYPE_2)
            {
                /*if the last time is type 2,the route is type 2,select the bigger cost of type 2,cost is cost + 1*/
                if (metric_value (ospf, ei->type,ei->instance) >= summary->cost)
                {
                    summary->cost = metric_value (ospf, ei->type,ei->instance) + 1;
                }
            }
        }
        else if(metric_type (ospf, ei->type,ei->instance) == EXTERNAL_METRIC_TYPE_1)
        {
            /* When find the route is type 1 */
            if (IS_DEBUG_OSPF_EVENT)
            {
                 zlog_debug (OSPF_DBG_EVENT, "%s(%d): use ospf->dmetric[%d][%d].value %d,type:type:%d",__FUNCTION__,__LINE__,
					ei->type,ei->instance, metric_value (ospf, ei->type,ei->instance)
					,metric_type (ospf, ei->type,ei->instance)+1);
            }
			
            if(summary->type == EXTERNAL_METRIC_TYPE_1)
            {
                /*if the last time is type 1,select the bigger  cost of type 1 cost*/
                if (metric_value (ospf, ei->type,ei->instance) > summary->cost)
                {
                    summary->cost = metric_value (ospf, ei->type,ei->instance);
                }
            }
        }
        else
        {
            zlog_warn("%s[%d] Get cost error!!!\n",__func__,__LINE__);
        }
    }
	
    /*  type 2 > type 1 > -1 */
    if(metric_type (ospf, ei->type,ei->instance) > summary->type)
    {
        summary->type = metric_type (ospf, ei->type,ei->instance);
    }
	
	if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "%s(%d): use summary->type:%d summary->cost %d",
                    __FUNCTION__,__LINE__,summary->type, summary->cost);
    }
    summary->specifics++;
}


static void
ospf_asbr_announce_external (struct ospf *ospf,
                             struct prefix_ipv4 *p, struct external_info *ei)
{
    struct ospf_summary *summary = NULL;
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_announce_external(): Start");
    }
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_announce_external(): "
                    "this is external route to %s/%d",
                    inet_ntoa (p->prefix), p->prefixlen);
    }
	
    if ((summary = ospf_summary_match (ospf, p)) )
    {
        /* except self summary lsa */
        ospf_asbr_update_aggregate (summary, ei, ospf);
    }
    else
    {
        ospf_asbr_announce_external_lsa (p, ospf, ei);
    }
}


static void
ospf_asbr_process_external_rt (struct ospf *ospf)
{
    int type,instance;
    struct prefix_ipv4 p;
    struct route_node *rn = NULL;
    struct external_info *ei = NULL;
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_process_external_rt(): Start");
    }
	
    for (type = 0; type <= ROUTE_PROTO_MAX; type++)
    {
        int redist_type = is_prefix_default (&p) ? DEFAULT_ROUTE : type;
        for(instance = 0; instance <= OSPF_MAX_PROCESS_ID; instance++)
        {
            if (ospf_is_type_redistributed (ospf, redist_type,instance))
            {
                if (EXTERNAL_INFO (ospf->vpn, type))
                {
                    for (rn = route_top (EXTERNAL_INFO (ospf->vpn, type)); rn; rn = route_next (rn))
                    {
                        for(ei = rn->info ; ei != NULL ; ei = ei->next)
                        {
                            if (ospf_is_type_redistributed (ospf,ei->type,ei->instance))
                            {
                                
                                if((ei->type == ROUTE_PROTO_CONNECT && ospf->route_policy_flag == 1)
                                    || ei->type != ROUTE_PROTO_CONNECT)
                                {
                                    if (IS_DEBUG_OSPF_EVENT)
                                    {
                                        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_process_external_rt(): announcing");
                                    }
                                    ospf_asbr_announce_external (ospf, (struct prefix_ipv4 *)&ei->p, ei);
                                }
                                else
                                {
                                    if((ospf->route_policy_flag == 0) && (ospf_route_policy_look(ospf,ei->ifindex) != NULL))
                                    {
                                        ospf_asbr_announce_external (ospf, (struct prefix_ipv4 *)&ei->p, ei);
                                    }
                                }
                                    
                            }
                        }
                    }
                }
            }
        }
    }
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_process_external_rt(): Stop");
    }
}

static void
ospf_asbr_remove_unapproved_external (struct ospf *ospf)
{    
	struct prefix_ipv4 p;
    unsigned char plength = 0;
    struct ospf_lsa *lsa = NULL;
    struct route_node *rn = NULL;

    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_remove_unapproved_external(): Start");
    }
	
    if (ospf_lsdb_count (ospf->lsdb, OSPF_AS_EXTERNAL_LSA) == 0)
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_task(): External lsdb are not yet ready");
        }
        return ;
    }
	
    LSDB_LOOP (EXTERNAL_LSDB (ospf), rn, lsa)
    {
        if (ospf_lsa_is_self_originated (ospf, lsa))
        {
            if (!CHECK_FLAG (lsa->flags, OSPF_LSA_APPROVED)
                    &&!CHECK_FLAG (lsa->flags, OSPF_LSA_LOCAL_XLT))
            {
                if(!IS_LSA_MAXAGE(lsa))
                {
                    struct as_external_lsa *al = (struct as_external_lsa *) lsa->data;
                    memset (&p, 0, sizeof (struct prefix_ipv4));
                    p.family = AF_INET;
                    plength = ip_masklen (al->mask);
                    p.prefixlen = MIN(IPV4_MAX_PREFIXLEN, plength);
                    p.prefix.s_addr = lsa->data->id.s_addr;
                    ospf_external_summary_lsa_flush (ospf, 0, &p, 0 /*, nexthop */);
                }
            }
        }
    }
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_remove_unapproved_external(): Stop");
    }
}



static struct ospf_summary *
ospf_summary_new (struct prefix_ipv4 *p)
{
    struct ospf_summary *summary = NULL;

	do
  	{		
		summary = XCALLOC (MTYPE_OSPF_AREA_RANGE, sizeof (struct ospf_summary));
		
  		if (summary == NULL)
  		{
    		zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
  		}
  	}while(summary == NULL);
	if(summary== NULL)
    {
        zlog_err("Can't creat ospf_summary : malloc failed");
        return (struct ospf_summary *)NULL;
    }
	
	memset(summary, 0, sizeof(struct ospf_summary));
    summary->addr = p->prefix;
    summary->masklen = p->prefixlen;
    summary->cost_config = OSPF_SUMMARY_COST_UNSPEC;
    summary->type = OSPF_SUMMARY_COST_UNSPEC;
    return summary;
}

static void
ospf_summary_free (struct ospf_summary *summary)
{
    XFREE (MTYPE_OSPF_AREA_RANGE, summary);
}


static void
ospf_summary_add (struct ospf* ospf, struct ospf_summary *summary)
{
    struct prefix_ipv4 p;
    struct route_node *rn = NULL;
	
    p.family = AF_INET;
    p.prefixlen = summary->masklen;
    p.prefix = summary->addr;
    rn = route_node_get (ospf->summary, (struct prefix *)&p);
    if (rn->info)
    {
        route_unlock_node (rn);
    }
    else
    {
        rn->info = summary;
    }
}

static void
ospf_summary_delete (struct ospf *ospf, struct route_node *rn)
{
    struct prefix_ipv4 p;
    struct in_addr nexthop;
    struct ospf_summary *summary = rn->info;
	int num = summary->specifics;
	
	zlog_debug(OSPF_DBG_OTHER, "%s %d \n",__func__,__LINE__);
    p.family = AF_INET;
    p.prefix.s_addr = summary->addr.s_addr;
    p.prefixlen = summary->masklen;
    nexthop.s_addr = 0;
    if (summary->specifics != 0)
    {
        ospf_external_info_delete (ospf->vpn, ROUTE_PROTO_OSPF,ospf->ospf_id, p, nexthop);
        //ospf_external_lsa_flush (ospf, ROUTE_PROTO_OSPF, &p, 0 /*, nexthop */);
    }
    ospf_summary_free (summary);
    rn->info = NULL;
    route_unlock_node (rn);
    route_unlock_node (rn);

	if(num != 0)
	{
		zlog_debug(OSPF_DBG_OTHER, "%s %d \n",__func__,__LINE__);
        ospf_external_lsa_flush (ospf, ROUTE_PROTO_OSPF, &p, 0 /*, nexthop */);
	}
}


struct ospf_summary *
ospf_summary_lookup (struct ospf* ospf, struct prefix_ipv4 *p)
{
    struct route_node *rn = NULL;

    rn = route_node_lookup (ospf->summary, (struct prefix *)p);
    if (rn)
    {
        route_unlock_node (rn);
        return rn->info;
    }
    return NULL;
}
#if 0
static struct ospf_summary *
ospf_summary_lookup_next (struct ospf* ospf,
                          struct in_addr *range_net,
                          int first)
{
    struct route_node *rn;
    struct prefix_ipv4 p;
    struct ospf_summary *find;

    p.family = AF_INET;
    p.prefixlen = IPV4_MAX_BITLEN;
    p.prefix = *range_net;

    if (first)
    {
        rn = route_top (ospf->summary);
    }
    else
    {
        rn = route_node_get (ospf->summary, (struct prefix *) &p);
        rn = route_next (rn);
    }

    for (; rn; rn = route_next (rn))
    {
        if (rn->info)
        {
            break;
        }
    }

    if (rn && rn->info)
    {
        find = rn->info;
        *range_net = rn->p.u.prefix4;
        route_unlock_node (rn);
        return find;
    }
    return NULL;
}



static struct ospf_summary *
ospf_summary_match_any (struct ospf *ospf, struct prefix_ipv4 *p)
{
    struct ospf_summary *summary = NULL;
    struct ospf_area *area = NULL;
    struct listnode *node = NULL;

    for (ALL_LIST_ELEMENTS_RO (ospf->areas, node, area))
    {
        if ((summary = ospf_summary_match (area->ospf, p)))
        {
            return summary;
        }
    }

    return NULL;
}
#endif
static int
ospf_summary_active (struct ospf_summary *summary)
{
    return summary->specifics;
}


int
ospf_summary_set (struct ospf *ospf,
                  struct prefix_ipv4 *p, int advertise)
{
    struct ospf_summary *summary = NULL;
	
    summary = ospf_summary_lookup (ospf, p);
    if (summary != NULL)
    {
        if ((CHECK_FLAG (summary->flags, OSPF_SUMMARY_ADVERTISE)
                && !CHECK_FLAG (advertise, OSPF_SUMMARY_ADVERTISE))
                || (!CHECK_FLAG (summary->flags, OSPF_SUMMARY_ADVERTISE)
                    && CHECK_FLAG (advertise, OSPF_SUMMARY_ADVERTISE)))
        {
            ospf_schedule_asbr_task (ospf);
        }
        if (CHECK_FLAG (advertise, OSPF_SUMMARY_ADVERTISE))
        {
            SET_FLAG (summary->flags, OSPF_SUMMARY_ADVERTISE);
        }
        else
        {
            UNSET_FLAG (summary->flags, OSPF_SUMMARY_ADVERTISE);
        }
    }
    else
    {
        summary = ospf_summary_new (p);
        ospf_summary_add (ospf, summary);
        ospf_schedule_asbr_task (ospf);
        if (CHECK_FLAG (advertise, OSPF_SUMMARY_ADVERTISE))
        {
            SET_FLAG (summary->flags, OSPF_SUMMARY_ADVERTISE);
        }
        else
        {
            UNSET_FLAG (summary->flags, OSPF_SUMMARY_ADVERTISE);
        }
    }
    return 1;
}

int
ospf_summary_unset (struct ospf *ospf, struct prefix_ipv4 *p)
{
    struct route_node *rn = NULL;
	
    rn = route_node_lookup (ospf->summary, (struct prefix*)p);
    if (rn == NULL)
    {
        return 0;
    }
    if (ospf_summary_active (rn->info))
    {
        ospf_schedule_asbr_task (ospf);
    }
    ospf_summary_delete (ospf, rn);
    return 1;
}


int
ospf_summary_cost_set (struct ospf *ospf,
                       struct prefix_ipv4 *p, u_int32_t cost)
{
    struct ospf_summary *summary = NULL;
	
    summary = ospf_summary_lookup (ospf, p);
    if (summary == NULL)
    {
        return 0;
    }
    if (summary->cost_config != (int)cost)
    {
        summary->cost_config = cost;
        if (ospf_summary_active (summary))
        {
            ospf_schedule_asbr_task (ospf);
        }
    }
    return 1;
}

/*abr-summary function*/
static void
ospf_asbr_announce_aggregates (struct ospf *ospf)
{
	struct prefix p;
    struct in_addr nexthop;
    struct route_node *rn = NULL;
    struct external_info *ei = NULL;
    struct ospf_summary *summary = NULL;
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_announce_aggregates(): Start");
    }
	
    for (rn = route_top (ospf->summary); rn; rn = route_next (rn))
    {
        if ((summary = rn->info))
        {
            if (!CHECK_FLAG (summary->flags, OSPF_SUMMARY_ADVERTISE))
            {
                if (IS_DEBUG_OSPF_EVENT)
                {
                    zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_announce_aggregates():discarding suppress-summary");
                }
                continue;
            }
			
            p.family = AF_INET;
            p.u.prefix4 = summary->addr;
            p.prefixlen = summary->masklen;
            if (IS_DEBUG_OSPF_EVENT)
            {
                zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_announce_aggregates():this is external: %s/%d",
                            inet_ntoa (p.u.prefix4), p.prefixlen);
            }
			
            if (CHECK_FLAG (summary->flags, OSPF_SUMMARY_SUBSTITUTE))
            {
                p.family = AF_INET;
                p.u.prefix4 = summary->subst_addr;
                p.prefixlen = summary->subst_masklen;
            }
			
            if (summary->specifics)
            {
                if (IS_DEBUG_OSPF_EVENT)
                {
                    zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_announce_aggregates(): active external");
                }
				
                nexthop.s_addr = 0;
                ei = ospf_external_info_add (ospf->vpn, ROUTE_PROTO_OSPF, ospf->ospf_id, *(struct prefix_ipv4 *)&p, 0, nexthop, 0);
                ei->flags = ospf->ospf_id;
                ospf_asbr_announce_summary_external ((struct prefix_ipv4 *)&p,
                                                     summary, ospf, ei);
            }
        }
    }
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_announce_aggregates(): Stop");
    }
}

/* This is the function taking care about ASBR stuff, i.e.
   summary-LSA origination and flooding. */
static void
ospf_asbr_task (struct ospf *ospf)
{
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_task(): Start");
    }
    
    if(ospf->router_id.s_addr == 0)
    {
        return;
    }
    
    if((listcount(ospf->areas) >= 1) && (listcount(ospf->areas) == ospf->anySTUB))
    {
        return;
    }
    
    /* set all self originate external lsa discard*/
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_task(): unapprove external");
    }
    
    ospf_asbr_unapprove_external (ospf);
    /* reset cost and specifics*/
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_task(): prepare aggregates");
    }
	
    ospf_asbr_prepare_aggregates (ospf);
	
    if (IS_OSPF_ASBR (ospf))
    {
        /* reset lsa refresh*/
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_task(): process External RT");
        }
        ospf_asbr_process_external_rt (ospf);
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_task(): announce asbr summary");
        }
        ospf_asbr_announce_aggregates (ospf);
    }
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_task(): remove unapproved external");
    }
    ospf_asbr_remove_unapproved_external (ospf);
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_asbr_task(): Stop");
    }
}



static int
ospf_asbr_task_timer (void *thread)
{
    struct ospf *ospf = (struct ospf *)(thread);
    
    if(ospf == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
    assert(ospf);
    
    ospf->t_asbr_check = 0;
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "Running ABR task on timer");
    }
    
    ospf_asbr_task (ospf);
    return 0;
}

void
ospf_schedule_asbr_task (struct ospf *ospf)
{
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "Scheduling ASBR task");
    }
	
    if (ospf->t_asbr_check == 0)
    {
        /*ospf->t_asbr_check = thread_add_timer (master_ospf, ospf_asbr_task_timer,
                                               ospf, OSPF_SUMMARY_TIMER_DELAY);*/
        OSPF_TIMER_ON(ospf->t_asbr_check, ospf_asbr_task_timer, OSPF_SUMMARY_TIMER_DELAY);
    }
    else
    {
        OSPF_TIMER_OFF(ospf->t_asbr_check);
        /*ospf->t_asbr_check = thread_add_timer (master_ospf, ospf_asbr_task_timer,
                                               ospf, OSPF_SUMMARY_TIMER_DELAY);*/
		OSPF_TIMER_ON(ospf->t_asbr_check, ospf_asbr_task_timer, OSPF_SUMMARY_TIMER_DELAY);
    }
}



