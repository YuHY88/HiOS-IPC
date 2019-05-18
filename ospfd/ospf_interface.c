/*
 * OSPF Interface functions.
 * Copyright (C) 1999, 2000 Toshiaki Takada
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
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

#include "lib/prefix.h"
#include "lib/memory.h"
#include "lib/thread.h"


#include "ospfd/ospfd.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_ism.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_nsm.h"
#include "ospfd/ospf_dump.h"
#include "ospfd/ospf_ipc.h"
#include "ospfd/ospf_abr.h"



#ifdef HAVE_SNMP
//#include "ospfd/ospf_snmp.h"
#endif /* HAVE_SNMP */


int
ospf_if_get_output_cost (struct ospf_interface *oi)
{
    /* If all else fails, use default OSPF cost */
    u_int32_t cost;
    u_int32_t bw, refbw;
    bw = oi->ifp->bandwidth ? oi->ifp->bandwidth : OSPF_DEFAULT_BANDWIDTH;
    refbw = oi->ospf->ref_bandwidth;
    /* A specifed ip ospf cost overrides a calculated one. */
    if (OSPF_IF_PARAM_CONFIGURED (IF_DEF_PARAMS (oi->ifp), output_cost_cmd) ||
            OSPF_IF_PARAM_CONFIGURED (oi->params, output_cost_cmd))
    {
        cost = OSPF_IF_PARAM (oi, output_cost_cmd);
    }
    /* See if a cost can be calculated from the zebra processes
     interface bandwidth field. */
    else
    {
        cost = (u_int32_t) ((double)refbw / (double)bw + (double)0.5);
        if (cost < 1)
        {
            cost = 1;
        }
        else if (cost > 65535)
        {
            cost = 65535;
        }
    }
    return cost;
}

void
ospf_if_recalculate_output_cost (struct interface *ifp)
{
    u_int32_t newcost;
    struct route_node *rn = NULL;
    if(ifp == NULL)
    {
        return ;
    }
    for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
    {
        struct ospf_interface *oi = NULL;
        if ( (oi = rn->info) == NULL)
        {
            continue;
        }
        newcost = ospf_if_get_output_cost (oi);
        /* Is actual output cost changed? */
        if (oi->output_cost != newcost)
        {
            oi->output_cost = newcost;
            ospf_router_lsa_update_area (oi->area);
        }
    }
}

/* Simulate down/up on the interface.  This is needed, for example, when
   the MTU changes. */
void
ospf_if_reset(struct interface *ifp)
{
    struct route_node *rn = NULL;
    for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
    {
        struct ospf_interface *oi = NULL;
        if ( (oi = rn->info) == NULL)
        {
            continue;
        }
        ospf_if_down(oi);
        ospf_if_up(oi);
    }
}

void
ospf_if_reset_variables (struct ospf_interface *oi)
{
    /* Set default values. */
    /* don't clear this flag.  oi->flag = OSPF_IF_DISABLE; */
    if (oi->vl_data)
    {
        oi->type = OSPF_IFTYPE_VIRTUALLINK;
    }
    else
        /* preserve network-type */
        if (oi->type != OSPF_IFTYPE_NBMA)
        {
            oi->type = OSPF_IFTYPE_BROADCAST;
        }
    oi->state = ISM_Down;
    oi->crypt_seqnum = 0;
    /* This must be short, (less than RxmtInterval)
     - RFC 2328 Section 13.5 para 3.  Set to 1 second to avoid Acks being
       held back for too long - MAG */
    oi->v_ls_ack = 1;
}

/* lookup oi for specified prefix/ifp */
struct ospf_interface *
ospf_if_table_lookup (struct interface *ifp, struct prefix *prefix)
{
    struct prefix p;
    struct route_node *rn = NULL;
    struct ospf_interface *rninfo = NULL;

    p = *prefix;
    p.prefixlen = IPV4_MAX_PREFIXLEN;

    /* route_node_get implicitely locks */
    if ((rn = route_node_lookup (IF_OIFS (ifp), &p)))
    {
        rninfo = (struct ospf_interface *) rn->info;
        route_unlock_node (rn);
    }

    return rninfo;
}

void ospf_add_to_if (struct interface *ifp, struct ospf_interface *oi)
{
    struct route_node *rn = NULL;
    struct prefix p;
    p = *oi->address;
    p.prefixlen = IPV4_MAX_PREFIXLEN;
    rn = route_node_get (IF_OIFS (ifp), &p);
    /* rn->info should either be NULL or equal to this oi
    * as route_node_get may return an existing node
    */
	if(!(!rn->info || rn->info == oi))
	{
		zlog_warn("%s[%d] rn->info == NULL || rn->info != oi",__func__,__LINE__);
		return ;
	}
    assert (!rn->info || rn->info == oi);
    rn->info = oi;
}

void ospf_delete_from_if (struct interface *ifp, struct ospf_interface *oi)
{
    struct route_node *rn = NULL;
    struct prefix p;
    p = *oi->address;
    p.prefixlen = IPV4_MAX_PREFIXLEN;
    rn = route_node_lookup (IF_OIFS (oi->ifp), &p);
	if(rn == NULL || rn->info == NULL)
	{
		zlog_warn("%s[%d] rn == NULL || rn->info == NULL",__func__,__LINE__);
		return ;
	}
    assert (rn);
    assert (rn->info);
    rn->info = NULL;
    route_unlock_node (rn);
    route_unlock_node (rn);
}

struct ospf_interface *
ospf_if_new (struct ospf *ospf, struct interface *ifp, struct prefix *p)
{
    struct ospf_interface *oi = NULL;

    if ((oi = ospf_if_table_lookup (ifp, p)) == NULL)
    {
		do
		{		
			oi = XCALLOC (MTYPE_OSPF_IF, sizeof (struct ospf_interface));
		
			if (oi == NULL)
			{
				zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
				sleep(1);
			}
		}while(oi == NULL);
		
		if(oi == NULL)
	    {
	        zlog_err("Can't creat ospf_if : malloc failed");
	        return (struct ospf_interface *)NULL;
	    }
        memset (oi, 0, sizeof (struct ospf_interface));
    }
    else
    {
        return oi;
    }

    /* Set zebra interface pointer. */
    oi->ifp = ifp;
    oi->address = p;

    ospf_add_to_if (ifp, oi);
    listnode_add (ospf->oiflist, oi);

    /* Initialize neighbor list. */
    oi->nbrs = route_table_init ();

    /* Initialize static neighbor list. */
    oi->nbr_nbma = list_new ();

    /* Initialize Link State Acknowledgment list. */
    oi->ls_ack = list_new ();
    oi->ls_ack_direct.ls_ack = list_new ();

    /* Set default values. */
    ospf_if_reset_variables (oi);

    /* Add pseudo neighbor. */
    //oi->nbr_self = ospf_nbr_new (oi);


    /* Set pseudo neighbor to Null */
    oi->nbr_self = NULL;

    oi->ls_upd_queue = route_table_init ();
    oi->t_ls_upd_event = NULL;
    oi->t_ls_ack_direct = NULL;

    oi->crypt_seqnum = time (NULL);

    ospf_opaque_type9_lsa_init (oi);

    oi->ospf = ospf;

    return oi;
}

/* Restore an interface to its pre UP state
   Used from ism_interface_down only */
void
ospf_if_cleanup (struct ospf_interface *oi)
{
    struct route_node *rn = NULL,
                      *rn_next = NULL;
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf_neighbor *nbr = NULL;
    struct ospf_nbr_nbma *nbr_nbma = NULL;
    struct ospf_lsa *lsa = NULL;
    
    /* oi->nbrs and oi->nbr_nbma should be deleted on InterfaceDown event */
    /* delete all static neighbors attached to this interface */
    for (ALL_LIST_ELEMENTS (oi->nbr_nbma, node, nnode, nbr_nbma))
    {
        OSPF_POLL_TIMER_OFF (nbr_nbma->t_poll);
        if (nbr_nbma->nbr)
        {
            nbr_nbma->nbr->nbr_nbma = NULL;
            nbr_nbma->nbr = NULL;
        }
        nbr_nbma->oi = NULL;
        listnode_delete (oi->nbr_nbma, nbr_nbma);
    }
    /* send Neighbor event KillNbr to all associated neighbors. */
    for (rn = route_top (oi->nbrs); rn; rn = rn_next)
    {
        rn_next = route_next (rn);
        if ((nbr = rn->info) != NULL)
        {
            if (nbr != oi->nbr_self)
            {
                OSPF_NSM_EVENT_EXECUTE (nbr, NSM_KillNbr);
            }
        }
    }
    /* Cleanup Link State Acknowlegdment list. */
    for (ALL_LIST_ELEMENTS (oi->ls_ack, node, nnode, lsa))
    {
        ospf_lsa_unlock (&lsa);    /* oi->ls_ack */
    }
    list_delete_all_node (oi->ls_ack);
    oi->crypt_seqnum = 0;
    /* Empty link state update queue */
    ospf_ls_upd_queue_empty (oi);
    /* Reset pseudo neighbor. */
    ospf_nbr_self_reset (oi);
}

void
ospf_sub_interface_free(struct interface *ifp)
{
    struct ospf_interface *oi = NULL;
    struct route_node *rn = NULL;
    for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
    {
        oi = rn->info;
        if (!oi) /* oi can be NULL for PtP aliases */
        {
            continue;
        }
        if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
        {
            continue;
        }
        if (if_is_loopback (oi->ifp))
        {
            continue;
        }
        if (CHECK_FLAG(oi->connected->flags, ZEBRA_IFA_SLAVE))
        {
            ospf_if_free(oi);
			rn->info = NULL;
        }
    }
    return ;
}

int
ospf_sub_interface_look_master(struct interface *ifp)
{
    struct ospf_interface * oi = NULL;
    struct route_node *rn = NULL;
    for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
    {
        oi = rn->info;
        if (!oi) /* oi can be NULL for PtP aliases */
        {
            continue;
        }
        if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
        {
            continue;
        }
        if (if_is_loopback (oi->ifp))
        {
            continue;
        }
        if (!CHECK_FLAG(oi->connected->flags, ZEBRA_IFA_SLAVE))
        {
            return 1;
        }
    }
    return 0;
}



void
ospf_if_free (struct ospf_interface *oi)
{
	if(oi == NULL)
	{
		zlog_warn("%s[%d] oi is NULL",__func__,__LINE__);
		return ;
	}
	struct ospf_area *area = oi->area;
    ospf_if_down (oi);
	if(oi->state != ISM_Down)
	{
		zlog_warn("%s[%d] oi->state != ISM_Down",__func__,__LINE__);
		return ;
	}
    assert (oi->state == ISM_Down);
    ospf_opaque_type9_lsa_term (oi);
    /* Free Pseudo Neighbour */
    ospf_nbr_delete (oi->nbr_self);
    oi->nbr_self = NULL;
    route_table_finish (oi->nbrs);
    oi->nbrs = NULL;
    route_table_finish (oi->ls_upd_queue);
    oi->ls_upd_queue = NULL;
    /* Free any lists that should be freed */
    list_free (oi->nbr_nbma);
    list_free (oi->ls_ack);
    list_free (oi->ls_ack_direct.ls_ack);
    ospf_delete_from_if (oi->ifp, oi);
    listnode_delete (oi->ospf->oiflist, oi);
    listnode_delete (oi->area->oiflist, oi);
    thread_cancel_event (master_ospf, oi);
	thread_cancel_event_normal (master_ospf, oi);
    memset (oi, 0, sizeof (*oi));
    XFREE (MTYPE_OSPF_IF, oi);
	ospf_nssa_lsa_fw_update(area);
}


/*
*  check if interface with given address is configured and
*  return it if yes.  special treatment for PtP networks.
*/
struct ospf_interface *
ospf_if_is_configured (struct ospf *ospf, struct in_addr *address)
{
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf_interface *oi = NULL;
    struct prefix_ipv4 addr;

    addr.family = AF_INET;
    addr.prefix = *address;
    addr.prefixlen = IPV4_MAX_PREFIXLEN;

    for (ALL_LIST_ELEMENTS (ospf->oiflist, node, nnode, oi))
    {
        if (oi->type != OSPF_IFTYPE_VIRTUALLINK)
        {
            if (oi->type == OSPF_IFTYPE_POINTOPOINT)
            {
                /* special leniency: match if addr is anywhere on peer subnet */
                if (prefix_match(CONNECTED_PREFIX(oi->connected),
                                 (struct prefix *)&addr))
                {
                    return oi;
                }
            }
            else
            {
                if (IPV4_ADDR_SAME (address, &oi->address->u.prefix4))
                {
                    return oi;
                }
            }
        }
    }
    return NULL;
}

int
ospf_if_is_up (struct ospf_interface *oi)
{
    return if_is_up (oi->ifp);
}

struct ospf_interface *
ospf_if_exists (struct ospf_interface *oic)
{
    struct listnode *node = NULL;
    struct ospf *ospf = oic->ospf;
    struct ospf_interface *oi = NULL;

    for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, node, oi))
    {
        if (oi == oic)
        {
            return oi;
        }
    }

    return NULL;
}

/* Lookup OSPF interface by router LSA posistion */
struct ospf_interface *
ospf_if_lookup_by_lsa_pos (struct ospf_area *area, int lsa_pos)
{
    struct listnode *node = NULL;
    struct ospf_interface *oi = NULL;

    for (ALL_LIST_ELEMENTS_RO (area->oiflist, node, oi))
    {
        if (lsa_pos >= oi->lsa_pos_beg && lsa_pos < oi->lsa_pos_end)
        {
            return oi;
        }
    }
    return NULL;
}

struct ospf_interface *
ospf_if_lookup_by_local_addr (struct ospf *ospf,
                              struct interface *ifp, struct in_addr address)
{
    struct listnode *node = NULL;
    struct ospf_interface *oi = NULL;

    for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, node, oi))
    {
        if (oi->type != OSPF_IFTYPE_VIRTUALLINK)
        {
            if (ifp && oi->ifp != ifp)
            {
                continue;
            }
            if (IPV4_ADDR_SAME (&address, &oi->address->u.prefix4))
            {
                return oi;
            }
        }
    }

    return NULL;
}

struct ospf_interface *
ospf_if_lookup_by_prefix (struct ospf *ospf, struct prefix_ipv4 *p)
{
    struct listnode *node = NULL;
    struct ospf_interface *oi = NULL;

    /* Check each Interface. */
    for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, node, oi))
    {
        if (oi->type != OSPF_IFTYPE_VIRTUALLINK)
        {
            struct prefix ptmp;
            prefix_copy (&ptmp, CONNECTED_PREFIX(oi->connected));
            apply_mask (&ptmp);
            if (prefix_same (&ptmp, (struct prefix *) p))
            {
                return oi;
            }
        }
    }
    return NULL;
}

/* determine receiving interface by ifp and source address */
struct ospf_interface *
ospf_if_lookup_recv_if (struct ospf *ospf, struct in_addr src,
                        struct interface *ifp)
{
    struct route_node *rn = NULL;
    struct prefix_ipv4 addr;
    struct ospf_interface *oi = NULL, *match = NULL;

    addr.family = AF_INET;
    addr.prefix = src;
    addr.prefixlen = IPV4_MAX_BITLEN;

    match = NULL;

    for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
    {
        oi = rn->info;
        if (!oi) /* oi can be NULL for PtP aliases */
        {
            continue;
        }
		
		/* slave ip do not recive packet */
        if (CHECK_FLAG(oi->connected->flags, ZEBRA_IFA_SLAVE))
        {
            continue;
        }
		
        if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
        {
            continue;
        }
        if (if_is_loopback (oi->ifp))
        {
            continue;
        }
        
        if (oi->type == OSPF_IFTYPE_POINTOPOINT)
        {
            match = oi;
        }
        
        if (prefix_match (CONNECTED_PREFIX(oi->connected),
                          (struct prefix *) &addr))
        {
            if ( (match == NULL) ||
                    (match->address->prefixlen < oi->address->prefixlen)
               )
            {
                match = oi;
            }
        }
    }

    return match;
}

void
ospf_if_stream_set (struct ospf_interface *oi)
{
	if(oi == NULL)
	{
		zlog_err("%s %d oi is null",__func__, __LINE__);
		return ;
	}
    /* set output fifo queue. */
    if (oi->obuf == NULL)
    {
        oi->obuf = ospf_fifo_new ();
    }

	if (oi->obuf_highest_priority == NULL)
    {
        oi->obuf_highest_priority = ospf_fifo_new ();
    }
}

void
ospf_if_stream_unset (struct ospf_interface *oi)
{
    struct ospf *ospf = NULL;
	if(oi == NULL)
	{
		zlog_err("%s %d oi is null",__func__, __LINE__);
		return ;
	}
	
	ospf = oi->ospf;
    if (oi->obuf)
    {
        zlog_warn("%s %d oi->obuf free",__func__, __LINE__);
        ospf_fifo_free (oi->obuf);
        oi->obuf = NULL;
        
    }

	if (oi->obuf_highest_priority)
    {
        zlog_warn("%s %d oi->obuf_highest_priority free",__func__, __LINE__);
        ospf_fifo_free (oi->obuf_highest_priority);
        oi->obuf_highest_priority = NULL;
    }
	
	if (oi->on_write_q)
    {
        listnode_delete (ospf->oi_write_q, oi);
        if (list_isempty(ospf->oi_write_q))
        {
            OSPF_TIMER_OFF_OLD (ospf->t_write);
        }
        oi->on_write_q = 0;
    }
}


struct ospf_if_params *ospf_new_if_params (void)
{
    struct ospf_if_params *oip = NULL;

	do
	{		
		oip = XCALLOC (MTYPE_OSPF_IF_PARAMS, sizeof (struct ospf_if_params));
	
		if (oip == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(oip == NULL);

	memset(oip, 0, sizeof(struct ospf_if_params));

    UNSET_IF_PARAM (oip, output_cost_cmd);
    UNSET_IF_PARAM (oip, transmit_delay);
    UNSET_IF_PARAM (oip, retransmit_interval);
    UNSET_IF_PARAM (oip, passive_interface);
    UNSET_IF_PARAM (oip, v_hello);
    UNSET_IF_PARAM (oip, fast_hello);
    UNSET_IF_PARAM (oip, v_wait);
    UNSET_IF_PARAM (oip, priority);
    UNSET_IF_PARAM (oip, type);
    UNSET_IF_PARAM (oip, auth_simple);
    UNSET_IF_PARAM (oip, auth_crypt);
    UNSET_IF_PARAM (oip, auth_type);
	UNSET_IF_PARAM (oip, bfd_flag);
	UNSET_IF_PARAM (oip, if_bfd_recv_interval);
	UNSET_IF_PARAM (oip, if_bfd_send_interval);
	UNSET_IF_PARAM (oip, if_bfd_detect_multiplier);

    oip->auth_crypt = list_new ();

    oip->network_lsa_seqnum = htonl(OSPF_INITIAL_SEQUENCE_NUMBER);

    return oip;
}

void
ospf_del_if_params (struct ospf_if_params *oip)
{
	if(oip == NULL)
	{
		zlog_err("%s %d oip is null",__func__, __LINE__);
		return ;
	}

    list_delete (oip->auth_crypt);
	oip->auth_crypt = NULL;
    XFREE (MTYPE_OSPF_IF_PARAMS, oip);
}

void
ospf_free_if_params (struct interface *ifp, struct in_addr addr)
{
    struct ospf_if_params *oip = NULL;
    struct prefix_ipv4 p;
    struct route_node *rn = NULL;
    p.family = AF_INET;
    p.prefixlen = IPV4_MAX_PREFIXLEN;
    p.prefix = addr;
    rn = route_node_lookup (IF_OIFS_PARAMS (ifp), (struct prefix*)&p);
    if (!rn || !rn->info)
    {
        return;
    }
    oip = rn->info;
    route_unlock_node (rn);
    if (!OSPF_IF_PARAM_CONFIGURED (oip, output_cost_cmd) &&
            !OSPF_IF_PARAM_CONFIGURED (oip, transmit_delay) &&
            !OSPF_IF_PARAM_CONFIGURED (oip, retransmit_interval) &&
            OSPF_IF_PARAM_CONFIGURED (oip, passive_interface) &&
            !OSPF_IF_PARAM_CONFIGURED (oip, v_hello) &&
            !OSPF_IF_PARAM_CONFIGURED (oip, fast_hello) &&
            !OSPF_IF_PARAM_CONFIGURED (oip, v_wait) &&
            !OSPF_IF_PARAM_CONFIGURED (oip, priority) &&
            !OSPF_IF_PARAM_CONFIGURED (oip, type) &&
            !OSPF_IF_PARAM_CONFIGURED (oip, auth_simple) &&
            !OSPF_IF_PARAM_CONFIGURED (oip, auth_type) &&
            listcount (oip->auth_crypt) == 0 &&
            ntohl (oip->network_lsa_seqnum) != OSPF_INITIAL_SEQUENCE_NUMBER)
    {
        ospf_del_if_params (oip);
        rn->info = NULL;
        route_unlock_node (rn);
    }
}

struct ospf_if_params *
ospf_lookup_if_params (struct interface *ifp, struct in_addr addr)
{
    struct prefix_ipv4 p;
    struct route_node *rn = NULL;

    p.family = AF_INET;
    p.prefixlen = IPV4_MAX_PREFIXLEN;
    p.prefix = addr;

    rn = route_node_lookup (IF_OIFS_PARAMS (ifp), (struct prefix*)&p);

    if (rn)
    {
        route_unlock_node (rn);
        return rn->info;
    }

    return NULL;
}

struct ospf_if_params *
ospf_get_if_params (struct interface *ifp, struct in_addr addr)
{
    struct prefix_ipv4 p;
    struct route_node *rn = NULL;

    p.family = AF_INET;
    p.prefixlen = IPV4_MAX_PREFIXLEN;
    p.prefix = addr;

    rn = route_node_get (IF_OIFS_PARAMS (ifp), (struct prefix*)&p);

    if (rn->info == NULL)
    {
        rn->info = ospf_new_if_params ();
    }
    else
    {
        route_unlock_node (rn);
    }

    return rn->info;
}

void
ospf_if_update_params (struct interface *ifp, struct in_addr addr)
{
    struct route_node *rn = NULL;
    struct ospf_interface *oi = NULL;
    for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
    {
        if ((oi = rn->info) == NULL)
        {
            continue;
        }
        if (IPV4_ADDR_SAME (&oi->address->u.prefix4, &addr))
        {
            oi->params = ospf_lookup_if_params (ifp, oi->address->u.prefix4);
        }
    }
}

int
ospf_if_new_hook (struct interface *ifp)
{
    int rc = 0;
	do
	{		
		ifp->info = XCALLOC (MTYPE_OSPF_IF_INFO, sizeof (struct ospf_if_info));
	
		if (ifp->info == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(ifp->info == NULL);
	
	if(ifp->info == NULL)
    {
        zlog_err("Can't creat ospf_if_info : malloc failed");
        return -1;
    }
	
	memset(ifp->info, 0, sizeof(struct ospf_if_info));
    IF_OIFS (ifp) = route_table_init ();
    IF_OIFS_PARAMS (ifp) = route_table_init ();
    IF_DEF_PARAMS (ifp) = ospf_new_if_params ();
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), transmit_delay);
    IF_DEF_PARAMS (ifp)->transmit_delay = OSPF_TRANSMIT_DELAY_DEFAULT;
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), retransmit_interval);
    IF_DEF_PARAMS (ifp)->retransmit_interval = OSPF_RETRANSMIT_INTERVAL_DEFAULT;
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), priority);
    IF_DEF_PARAMS (ifp)->priority = OSPF_ROUTER_PRIORITY_DEFAULT;
    IF_DEF_PARAMS (ifp)->mtu_ignore = OSPF_MTU_IGNORE_DEFAULT;
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), v_hello);
    IF_DEF_PARAMS (ifp)->v_hello = OSPF_HELLO_INTERVAL_DEFAULT;
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), fast_hello);
    IF_DEF_PARAMS (ifp)->fast_hello = OSPF_FAST_HELLO_DEFAULT;
    //SET_IF_PARAM (IF_DEF_PARAMS (ifp), v_wait);
    IF_DEF_PARAMS (ifp)->v_wait = OSPF_ROUTER_DEAD_INTERVAL_DEFAULT;
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), auth_simple);
    memset (IF_DEF_PARAMS (ifp)->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE);
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), auth_type);
    IF_DEF_PARAMS (ifp)->auth_type = OSPF_AUTH_NOTSET;
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), mode);
    IF_DEF_PARAMS (ifp)->mode = IFNET_MODE_L3;

    SET_IF_PARAM (IF_DEF_PARAMS (ifp), v_holddown);
    IF_DEF_PARAMS (ifp)->v_holddown = OSPF_SYNC_LDP_HOLD_DOWN_INTERVAL;

    SET_IF_PARAM (IF_DEF_PARAMS (ifp), v_maxcost);
    IF_DEF_PARAMS (ifp)->v_maxcost = OSPF_SYNC_LDP_MAX_COST_INTERVAL;
    
    IF_DEF_PARAMS (ifp)->ldp_enable = OSPF_IF_LDP_SYNC_DISABLE;

	
	UNSET_IF_PARAM (IF_DEF_PARAMS (ifp), bfd_flag);
    IF_DEF_PARAMS (ifp)->bfd_flag = OSPF_IF_BFD_DISABLE;
	UNSET_IF_PARAM (IF_DEF_PARAMS (ifp), if_bfd_recv_interval);
    IF_DEF_PARAMS (ifp)->if_bfd_recv_interval = OSPF_IF_BFD_RECV_INTERVAL_DEFAULT;
	UNSET_IF_PARAM (IF_DEF_PARAMS (ifp), if_bfd_send_interval);
    IF_DEF_PARAMS (ifp)->if_bfd_send_interval = OSPF_IF_BFD_SEND_INTERVAL_DEFAULT;
	UNSET_IF_PARAM (IF_DEF_PARAMS (ifp), if_bfd_detect_multiplier);
    IF_DEF_PARAMS (ifp)->if_bfd_detect_multiplier = OSPF_IF_BFD_DETECT_MULT_DEFAULT;
	
    rc = ospf_opaque_new_if (ifp);
    return rc;
}

static int
ospf_if_delete_hook (struct interface *ifp)
{
    int rc = 0;
    struct route_node *rn;
    rc = ospf_opaque_del_if (ifp);
    route_table_finish (IF_OIFS (ifp));
    IF_OIFS (ifp) = NULL;
    for (rn = route_top (IF_OIFS_PARAMS (ifp)); rn; rn = route_next (rn))
    {
        if (rn->info)
        {
            ospf_del_if_params (rn->info);
			rn->info = NULL;
        }
    }
    route_table_finish (IF_OIFS_PARAMS (ifp));
    IF_OIFS_PARAMS (ifp) = NULL;
    ospf_del_if_params ((struct ospf_if_params *) IF_DEF_PARAMS (ifp));
	IF_DEF_PARAMS (ifp) = NULL;
    XFREE (MTYPE_OSPF_IF_INFO, ifp->info);
    ifp->info = NULL;
    return rc;
}

int
ospf_if_is_enable (struct ospf_interface *oi)
{
    if (!if_is_loopback (oi->ifp))
        if (if_is_up (oi->ifp))
        {
            return 1;
        }
    return 0;
}

int
ospf_if_up (struct ospf_interface *oi)
{
    if (oi == NULL)
    {
        return 0;
    }
    /* update abr status */
    ospf_schedule_abr_task (oi->ospf);
    if (oi->type == OSPF_IFTYPE_LOOPBACK)
    {
        //OSPF_ISM_EVENT_SCHEDULE (oi, ISM_LoopInd);
		OSPF_ISM_EVENT_EXECUTE (oi, ISM_LoopInd);
    }
    else
    {
        //if (oi->ospf != NULL)
        //ospf_adjust_sndbuflen (oi->ospf, oi->ifp->mtu);
        ospf_if_stream_set (oi);
        //OSPF_ISM_EVENT_SCHEDULE (oi, ISM_InterfaceUp);
		OSPF_ISM_EVENT_EXECUTE (oi, ISM_InterfaceUp);
    }
    
    ospf_external_lsa_fw_update(oi->ospf);
    return 1;
}

int
ospf_if_down (struct ospf_interface *oi)
{
    if (oi == NULL)
    {
        return 0;
    }
    /* update abr status */
    ospf_schedule_abr_task (oi->ospf);
    ospf_external_lsa_fw_update(oi->ospf);
    OSPF_ISM_EVENT_EXECUTE (oi, ISM_InterfaceDown);
    /* delete position in router LSA */
    oi->lsa_pos_beg = 0;
    oi->lsa_pos_end = 0;
    /* Shutdown packet reception and sending */
    ospf_if_stream_unset (oi);
    return 1;
}


/* Virtual Link related functions. */

struct ospf_vl_data *
ospf_vl_data_new (struct ospf_area *area, struct in_addr vl_peer)
{
    struct ospf_vl_data *vl_data = NULL;

	do
	{		
		vl_data = XCALLOC (MTYPE_OSPF_VL_DATA, sizeof (struct ospf_vl_data));
	
		if (vl_data == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(vl_data == NULL);
	
	if(vl_data== NULL)
    {
        zlog_err("Can't creat ospf_vl_data : malloc failed");
        return (struct ospf_vl_data *)NULL;
    }
	memset(vl_data, 0, sizeof(struct ospf_vl_data));

    vl_data->vl_peer.s_addr = vl_peer.s_addr;
    vl_data->vl_area_id = area->area_id;
    vl_data->format = area->format;

    return vl_data;
}

void
ospf_vl_data_free (struct ospf_vl_data *vl_data)
{
    XFREE (MTYPE_OSPF_VL_DATA, vl_data);
}

u_int vlink_count = 0;

struct ospf_interface *
ospf_vl_new (struct ospf *ospf, struct ospf_vl_data *vl_data)
{
	int flag = 0;		//==1标志接口名称有冲突。这里作为循环因子
	int temp_vlink_count = 0;
    struct ospf_interface * voi = NULL;
    struct interface * vi = NULL;
    char   ifname[INTERFACE_NAMSIZ + 1];
    struct ospf_area *area = NULL;
    struct in_addr area_id;
    struct connected *co = NULL;
    struct prefix_ipv4 *p = NULL;

    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_vl_new(): Start");
    }
    if (vlink_count == OSPF_VL_MAX_COUNT)
    {
        if (IS_DEBUG_OSPF_EVENT)
            zlog_debug (OSPF_DBG_EVENT, "ospf_vl_new(): Alarm: "
                        "cannot create more than OSPF_MAX_VL_COUNT virtual links");
        return NULL;
    }

    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_vl_new(): creating pseudo zebra interface");
    }

	/*这里防止有多个vlink接口时，先删后加会出现vlink接口名重复的问题*/
	temp_vlink_count = vlink_count;
	do{
		flag = 0;
		memset(ifname, 0, sizeof(ifname));
		snprintf (ifname, sizeof(ifname), "VLINK%d", temp_vlink_count);
		if(if_lookup_by_name_vrf (ifname, VRF_DEFAULT) != NULL)
		{
			temp_vlink_count++;
			flag++;
		}
	}while(flag);
	/************************************************************/
    //snprintf (ifname, sizeof(ifname), "VLINK%d", vlink_count);
    vi = if_create (ifname, strnlen(ifname, sizeof(ifname)));
    /* Ensure that linkdetection is not enabled on the stub interfaces
     * created for OSPF virtual links. */
    UNSET_FLAG(vi->status, ZEBRA_INTERFACE_LINKDETECTION);
    co = connected_new ();
    co->ifp = vi;
    listnode_add (vi->connected, co);

    p = prefix_ipv4_new ();
    p->family = AF_INET;
    p->prefix.s_addr = 0;
    p->prefixlen = 0;

    co->address = (struct prefix *)p;

    voi = ospf_if_new (ospf, vi, co->address);
    if (voi == NULL)
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "ospf_vl_new(): Alarm: OSPF int structure is not created");
        }
        return NULL;
    }
    voi->connected = co;
    voi->vl_data = vl_data;
    voi->ifp->mtu = OSPF_VL_MTU;
    voi->type = OSPF_IFTYPE_VIRTUALLINK;

	//add by zzl
	IF_DEF_PARAMS(vi)->passive_interface = OSPF_IF_ACTIVE;
	SET_IF_PARAM(IF_DEF_PARAMS(vi), passive_interface);

    vlink_count++;
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_vl_new(): Created name: %s", ifname);
    }
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_vl_new(): set if->name to %s", vi->name);
    }

    area_id.s_addr = 0;
    area = ospf_area_get (ospf, area_id, OSPF_AREA_ID_FORMAT_ADDRESS);
    voi->area = area;

    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_vl_new(): set associated area to the backbone");
    }

    /* Add pseudo neighbor. */
    ospf_nbr_self_reset(voi);
    ospf_area_add_if (voi->area, voi);

    ospf_if_stream_set (voi);

    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_vl_new(): Stop");
    }
    return voi;
}

static void
ospf_vl_if_delete (struct ospf_vl_data *vl_data)
{
    struct interface *ifp = vl_data->vl_oi->ifp;
    vl_data->vl_oi->address->u.prefix4.s_addr = 0;
    vl_data->vl_oi->address->prefixlen = 0;
    ospf_if_free (vl_data->vl_oi);
	vl_data->vl_oi = NULL;
    if_delete (ifp);
	ifp = NULL;
    vlink_count--;
}

/* Look up vl_data for given peer, optionally qualified to be in the
 * specified area. NULL area returns first found..
 */
struct ospf_vl_data *
ospf_vl_lookup (struct ospf *ospf, struct ospf_area *area,
                struct in_addr vl_peer)
{
    struct ospf_vl_data *vl_data = NULL;
    struct listnode *node = NULL;

    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "%s: Looking for %s", __func__, inet_ntoa (vl_peer));
        if (area)
        {
            zlog_debug (OSPF_DBG_EVENT, "%s: in area %s", __func__, inet_ntoa (area->area_id));
        }
    }

    for (ALL_LIST_ELEMENTS_RO (ospf->vlinks, node, vl_data))
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
            zlog_debug (OSPF_DBG_EVENT, "%s: VL %s, peer %s", __func__,
                        vl_data->vl_oi->ifp->name,
                        inet_ntoa (vl_data->vl_peer));
			zlog_debug (OSPF_DBG_EVENT, "%s: VL %s, vlink's area = %s", __func__,
                        vl_data->vl_oi->ifp->name,
                        inet_ntoa (vl_data->vl_area_id));
        }
		
        if (area && !IPV4_ADDR_SAME (&vl_data->vl_area_id, &area->area_id))
        {
            continue;
        }
        if (IPV4_ADDR_SAME (&vl_data->vl_peer, &vl_peer))
        {
            return vl_data;
        }
    }

    return NULL;
}


/*******************************************************************************************
*Function:	根据router-lsa里的link内容中的link_id和link_data来确认目标vl_data。目的是为了解决
			两台路由器同时接入多个区域且在这些区域中都配置了vlink时，ospf->vlinks会有多个vl_peer
			相同的数据结构，通过link_id和link_data即可准确的找到对应的vl_data。这样避免了ospf_vl_lookup()
			只找到ospf->vlinks链表中第一个匹配的vl_peer时就返回导致vl_data不是期望的vl_data数据
			结构的问题。
*Parameter: struct ospf *ospf 			//ospf实例数据结构
			struct in_addr vl_peer 		//Router-ID of the peer for VLs.
			struct in_addr link_data 	//router-lsa中link的数据域，该值实际上为vlink接口的ip
*return:	目标vl_data数据结构
********************************************************************************************/
struct ospf_vl_data *
ospf_vl_lookup_for_culc_nexthop (struct ospf *ospf, struct in_addr vl_peer, 
						struct in_addr link_data)
{
	struct ospf_vl_data *vl_data = NULL;
	struct listnode *node = NULL;

	if (IS_DEBUG_OSPF_EVENT)
	{
		zlog_debug (OSPF_DBG_EVENT, "%s: Looking for %s", __func__, inet_ntoa (vl_peer));
		zlog_debug (OSPF_DBG_EVENT, "%s: The link_data: %s", __func__, inet_ntoa (link_data));
	}

	for (ALL_LIST_ELEMENTS_RO (ospf->vlinks, node, vl_data))
	{
		if (IS_DEBUG_OSPF_EVENT)
		{
			zlog_debug (OSPF_DBG_EVENT, "%s: VL %s, peer %s", __func__,
						vl_data->vl_oi->ifp->name,
						inet_ntoa (vl_data->vl_peer));
			zlog_debug (OSPF_DBG_EVENT, "%s: VL %s, vlink's area = %s", __func__,
						vl_data->vl_oi->ifp->name,
						inet_ntoa (vl_data->vl_area_id));
		}
		if (IPV4_ADDR_SAME (&vl_data->vl_peer, &vl_peer) 
			&& IPV4_ADDR_SAME(&(vl_data->vl_oi->address->u.prefix4), &link_data))
		{
			if (IS_DEBUG_OSPF_EVENT)
			{
				zlog_debug (OSPF_DBG_EVENT, "%s: target VL %s, peer %s", __func__,
							vl_data->vl_oi->ifp->name,
							inet_ntoa (vl_data->vl_peer));
				zlog_debug (OSPF_DBG_EVENT, "%s: target VL %s, vlink's area = %s", __func__,
							vl_data->vl_oi->ifp->name,
							inet_ntoa (vl_data->vl_area_id));
			}
			return vl_data;
		}
	}

	return NULL;
}

static void
ospf_vl_shutdown (struct ospf_vl_data *vl_data)
{
    struct ospf_interface *oi = NULL;
    if ((oi = vl_data->vl_oi) == NULL)
    {
        return;
    }
    oi->address->u.prefix4.s_addr = 0;
    oi->address->prefixlen = 0;
    UNSET_FLAG (oi->ifp->flags, IFF_UP);
    /* OSPF_ISM_EVENT_SCHEDULE (oi, ISM_InterfaceDown); */
    OSPF_ISM_EVENT_EXECUTE (oi, ISM_InterfaceDown);
}

void
ospf_vl_add (struct ospf *ospf, struct ospf_vl_data *vl_data)
{
    listnode_add (ospf->vlinks, vl_data);
#ifdef HAVE_SNMP
    ospf_snmp_vl_add (vl_data);
#endif /* HAVE_SNMP */
}

void
ospf_vl_delete (struct ospf *ospf, struct ospf_vl_data *vl_data)
{
	if(ospf == NULL||vl_data == NULL)
	{
		zlog_err("%s %d ospf or vl_data is null",__func__, __LINE__);
		return ;
	}

    ospf_vl_shutdown (vl_data);
    ospf_vl_if_delete (vl_data);
#ifdef HAVE_SNMP
    ospf_snmp_vl_delete (vl_data);
#endif /* HAVE_SNMP */
    listnode_delete (ospf->vlinks, vl_data);
    ospf_vl_data_free (vl_data);
}

static int
ospf_vl_set_params (struct ospf_vl_data *vl_data, struct vertex *v)
{
    int changed = 0;
    struct ospf_interface *voi = NULL;
    struct listnode *node = NULL;
    struct vertex_parent *vp = NULL;
    unsigned int i;
    struct router_lsa *rl = NULL;
    voi = vl_data->vl_oi;
    if (voi->output_cost != v->distance)
    {
        voi->output_cost = v->distance;
        changed = 1;
    }
    for (ALL_LIST_ELEMENTS_RO (v->parents, node, vp))
    {
        vl_data->nexthop.oi = vp->nexthop->oi;
        vl_data->nexthop.router = vp->nexthop->router;
        if (!IPV4_ADDR_SAME(&voi->address->u.prefix4,
                            &vl_data->nexthop.oi->address->u.prefix4))
        {
            changed = 1;
        }
        voi->address->u.prefix4 = vl_data->nexthop.oi->address->u.prefix4;
        voi->address->prefixlen = vl_data->nexthop.oi->address->prefixlen;
        break; /* We take the first interface. */
    }
    rl = (struct router_lsa *)v->lsa;
    /* use SPF determined backlink index in struct vertex
     * for virtual link destination address
     */
    if (vp && vp->backlink >= 0)
    {
        if (!IPV4_ADDR_SAME (&vl_data->peer_addr,
                             &rl->link[vp->backlink].link_data))
        {
            changed = 1;
        }
        vl_data->peer_addr = rl->link[vp->backlink].link_data;
    }
    else
    {
        /* This is highly odd, there is no backlink index
         * there should be due to the ospf_spf_has_link() check
         * in SPF. Lets warn and try pick a link anyway.
         */
        zlog_warn ("ospf_vl_set_params: No backlink for %s!",
                   vl_data->vl_oi->ifp->name);
        for (i = 0; i < ntohs (rl->links); i++)
        {
            switch (rl->link[i].type)
            {
            case LSA_LINK_TYPE_VIRTUALLINK:
                if (IS_DEBUG_OSPF_EVENT)
                {
                    zlog_debug (OSPF_DBG_EVENT, "found back link through VL");
                }
            case LSA_LINK_TYPE_TRANSIT:
            case LSA_LINK_TYPE_POINTOPOINT:
                if (!IPV4_ADDR_SAME (&vl_data->peer_addr,
                                     &rl->link[i].link_data))
                {
                    changed = 1;
                }
                vl_data->peer_addr = rl->link[i].link_data;
				break;
			default:
				break;
            }
        }
    }
    if (IS_DEBUG_OSPF_EVENT)
        zlog_debug (OSPF_DBG_EVENT, "%s: %s peer address: %s, cost: %d,%schanged", __func__,
                    vl_data->vl_oi->ifp->name,
                    inet_ntoa(vl_data->peer_addr),
                    voi->output_cost,
                    (changed ? " " : " un"));
    return changed;
}


void
ospf_vl_up_check (struct ospf_area *area, struct in_addr rid,
                  struct vertex *v)
{
    struct ospf *ospf = area->ospf;
    struct listnode *node = NULL;
    struct ospf_vl_data *vl_data = NULL;
    struct ospf_interface *oi = NULL;
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "ospf_vl_up_check(): Start");
        zlog_debug (OSPF_DBG_EVENT, "ospf_vl_up_check(): Router ID is %s", inet_ntoa (rid));
        zlog_debug (OSPF_DBG_EVENT, "ospf_vl_up_check(): Area is %s", inet_ntoa (area->area_id));
    }
    for (ALL_LIST_ELEMENTS_RO (ospf->vlinks, node, vl_data))
    {
        if (IS_DEBUG_OSPF_EVENT)
        {
        	zlog_debug (OSPF_DBG_EVENT, "ospf_vl_up_check(): in for() vlinks, list->count = %d\n",listcount(ospf->vlinks));
            zlog_debug (OSPF_DBG_EVENT, "%s: considering VL, %s in area %s", __func__,
                        vl_data->vl_oi->ifp->name,
                        inet_ntoa (vl_data->vl_area_id));
            zlog_debug (OSPF_DBG_EVENT, "%s: peer ID: %s", __func__,
                        inet_ntoa (vl_data->vl_peer));
        }
        if (IPV4_ADDR_SAME (&vl_data->vl_peer, &rid) &&
                IPV4_ADDR_SAME (&vl_data->vl_area_id, &area->area_id))
        {
            oi = vl_data->vl_oi;
            SET_FLAG (vl_data->flags, OSPF_VL_FLAG_APPROVED);
            if (IS_DEBUG_OSPF_EVENT)
            {
                zlog_debug (OSPF_DBG_EVENT, "ospf_vl_up_check(): this VL matched");
            }
            if (oi->state == ISM_Down)
            {
                if (IS_DEBUG_OSPF_EVENT)
                {
                    zlog_debug (OSPF_DBG_EVENT, "ospf_vl_up_check(): VL is down, waking it up.  if_name = %s oi->type = %d\n",oi->ifp->name, oi->type);
                }
                SET_FLAG (oi->ifp->flags, IFF_UP);
                OSPF_ISM_EVENT_EXECUTE(oi,ISM_InterfaceUp);
            }
            if (ospf_vl_set_params (vl_data, v))
            {
                if (IS_DEBUG_OSPF (ism, ISM_EVENTS))
                    zlog_debug (OSPF_DBG_ISM_EVENTS, "ospf_vl_up_check: VL cost change,"
                                " scheduling router lsa refresh");
                if (ospf->backbone)
                {
                    ospf_router_lsa_update_area (ospf->backbone);
                }
                else if (IS_DEBUG_OSPF (ism, ISM_EVENTS))
                {
                    zlog_debug (OSPF_DBG_ISM_EVENTS, "ospf_vl_up_check: VL cost change, no backbone!");
                }
            }
        }
    }
}

void
ospf_vl_unapprove (struct ospf *ospf)
{
    struct listnode *node = NULL;
    struct ospf_vl_data *vl_data = NULL;
    for (ALL_LIST_ELEMENTS_RO (ospf->vlinks, node, vl_data))
    {
        UNSET_FLAG (vl_data->flags, OSPF_VL_FLAG_APPROVED);
    }
}

void
ospf_vl_shut_unapproved (struct ospf *ospf)
{
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf_vl_data *vl_data = NULL;
    for (ALL_LIST_ELEMENTS (ospf->vlinks, node, nnode, vl_data))
    {
        if (!CHECK_FLAG (vl_data->flags, OSPF_VL_FLAG_APPROVED))
        {
            ospf_vl_shutdown (vl_data);
        }
    }
}

int
ospf_full_virtual_nbrs (struct ospf_area *area)
{
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "counting fully adjacent virtual neighbors in area %s",
                    inet_ntoa (area->area_id));
        zlog_debug (OSPF_DBG_EVENT, "there are %d of them", area->full_vls);
    }
    return area->full_vls;
}

int
ospf_vls_in_area (struct ospf_area *area)
{
    struct listnode *node = NULL;
    struct ospf_vl_data *vl_data = NULL;
    int c = 0;
    for (ALL_LIST_ELEMENTS_RO (area->ospf->vlinks, node, vl_data))
    {
        if (IPV4_ADDR_SAME (&vl_data->vl_area_id, &area->area_id))
        {
            c++;
        }
    }
    return c;
}


struct crypt_key *
ospf_crypt_key_new ()
{
    //return XCALLOC (MTYPE_OSPF_CRYPT_KEY, sizeof (struct crypt_key));
    struct crypt_key *key = NULL;
    do
	{		
		key = XCALLOC (MTYPE_OSPF_CRYPT_KEY, sizeof (struct crypt_key));
	
		if (key == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(key == NULL);
	return key;
}

void
ospf_crypt_key_add (struct list *crypt, struct crypt_key *ck)
{
    listnode_add (crypt, ck);
}

struct crypt_key *
ospf_crypt_key_lookup (struct list *auth_crypt, u_char key_id)
{
    struct listnode *node = NULL;
    struct crypt_key *ck = NULL;

    for (ALL_LIST_ELEMENTS_RO (auth_crypt, node, ck))
    {
        if (ck->key_id == key_id)
        {
            return ck;
        }
    }

    return NULL;
}

int
ospf_crypt_key_delete (struct list *auth_crypt, u_char key_id)
{
    struct listnode *node = NULL, *nnode = NULL;
    struct crypt_key *ck = NULL;
    for (ALL_LIST_ELEMENTS (auth_crypt, node, nnode, ck))
    {
        if (ck->key_id == key_id)
        {
            listnode_delete (auth_crypt, ck);
            XFREE (MTYPE_OSPF_CRYPT_KEY, ck);
			ck = NULL;
            return 1;
        }
    }
    return 0;
}

void ospf_auth_md5_delete_delete_all(struct ospf_if_params *params)
{
    int key_id = 0;
    struct crypt_key *ck = NULL;
    params->auth_type = OSPF_AUTH_CRYPTOGRAPHIC;
    for(key_id = 1; key_id < 256; key_id++)
    {
        ck = NULL;
        if((ck = ospf_crypt_key_lookup(params->auth_crypt,key_id)) != NULL)
        {
            ospf_crypt_key_delete (params->auth_crypt, key_id);
        }
    }
    params->auth_type = OSPF_AUTH_NOTSET;
}

void ospf_area_auth_md5_delete_all(struct ospf_area *area)
{
    int key_id = 0;
    struct crypt_key *ck = NULL;
    area->auth_type = OSPF_AUTH_CRYPTOGRAPHIC;
    for(key_id = 1; key_id < 256; key_id++)
    {
        if((ck = ospf_crypt_key_lookup(area->auth_crypt, key_id)) != NULL)
        {
            ospf_crypt_key_delete (area->auth_crypt, key_id);
        }
    }
    area->auth_type = OSPF_AUTH_NULL;
}


u_char
ospf_default_iftype(struct interface *ifp)
{
	if(ifp == NULL)
	{
		zlog_err("%s %d ifp is null",__func__, __LINE__);
		return OSPF_IFTYPE_NONE;
	}

    if (if_is_pointopoint (ifp))
    {
        return OSPF_IFTYPE_POINTOPOINT;
    }
    else if (if_is_loopback (ifp))
    {
        return OSPF_IFTYPE_LOOPBACK;
    }
    else
    {
        return OSPF_IFTYPE_BROADCAST;
    }
}
#if 0
struct connected *get_ospf_out_if_address(uint32_t ifindex)
{
    struct interface *ifp = NULL;
    vrf_id_t vrf_id = VRF_DEFAULT;
    
    /*according to the interface name look up the struct pointer ifp */
    ifp = if_lookup_by_index_vrf (ifindex, vrf_id);


    if(ifp)/*if the if exit,first must delete the address*/
    {
        //struct ifm_event pevent;
        //pevent.ifindex = ifindex;
        struct connected *ifc = NULL;
        struct listnode *node = NULL;
        struct listnode *next = NULL;
        for (node = listhead (ifp->connected); node; node = next)
        {
            ifc = listgetdata (node);
            next = node->next;
            if (ifc != NULL)
            {
                break;
            }
        }
        return ifc;
    }
    return NULL;
}
#endif
void ospf_param_if_default(struct interface *ifp)
{
    struct ospf_if_params *params = NULL;
    if(ifp == NULL)
    {	
		zlog_err("%s %d ifp is null",__func__, __LINE__);
        return ;
    }
    params = IF_DEF_PARAMS (ifp);
    UNSET_IF_PARAM (params, output_cost_cmd);
    UNSET_IF_PARAM (params, transmit_delay);
    UNSET_IF_PARAM (params, retransmit_interval);
    UNSET_IF_PARAM (params, passive_interface);
    UNSET_IF_PARAM (params, v_hello);
    UNSET_IF_PARAM (params, fast_hello);
    UNSET_IF_PARAM (params, v_wait);
    UNSET_IF_PARAM (params, priority);
    UNSET_IF_PARAM (params, type);
    UNSET_IF_PARAM (params, auth_simple);
    UNSET_IF_PARAM (params, auth_crypt);
    UNSET_IF_PARAM (params, auth_type);
	UNSET_IF_PARAM (params, bfd_flag); 
	UNSET_IF_PARAM (params, if_bfd_recv_interval);
	UNSET_IF_PARAM (params, if_bfd_send_interval);
	UNSET_IF_PARAM (params, if_bfd_detect_multiplier);
    params->passive_interface = OSPF_IF_PASSIVE;
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), type);
    IF_DEF_PARAMS (ifp)->type = OSPF_IFTYPE_BROADCAST;
    UNSET_IF_PARAM (params, type);
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), transmit_delay);
    IF_DEF_PARAMS (ifp)->transmit_delay = OSPF_TRANSMIT_DELAY_DEFAULT;
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), retransmit_interval);
    IF_DEF_PARAMS (ifp)->retransmit_interval = OSPF_RETRANSMIT_INTERVAL_DEFAULT;
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), priority);
    IF_DEF_PARAMS (ifp)->priority = OSPF_ROUTER_PRIORITY_DEFAULT;
    IF_DEF_PARAMS (ifp)->mtu_ignore = OSPF_MTU_IGNORE_DEFAULT;
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), v_hello);
    IF_DEF_PARAMS (ifp)->v_hello = OSPF_HELLO_INTERVAL_DEFAULT;
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), fast_hello);
    IF_DEF_PARAMS (ifp)->fast_hello = OSPF_FAST_HELLO_DEFAULT;
    //SET_IF_PARAM (IF_DEF_PARAMS (ifp), v_wait);
    IF_DEF_PARAMS (ifp)->v_wait = OSPF_ROUTER_DEAD_INTERVAL_DEFAULT;
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), auth_simple);
    memset (IF_DEF_PARAMS (ifp)->auth_simple, 0, OSPF_AUTH_SIMPLE_SIZE);
    SET_IF_PARAM (IF_DEF_PARAMS (ifp), auth_type);
    IF_DEF_PARAMS (ifp)->auth_type = OSPF_AUTH_NOTSET;

    SET_IF_PARAM (IF_DEF_PARAMS (ifp), v_holddown);
    IF_DEF_PARAMS (ifp)->v_holddown = OSPF_SYNC_LDP_HOLD_DOWN_INTERVAL;

    SET_IF_PARAM (IF_DEF_PARAMS (ifp), v_maxcost);
    IF_DEF_PARAMS (ifp)->v_maxcost = OSPF_SYNC_LDP_MAX_COST_INTERVAL;
    
    IF_DEF_PARAMS (ifp)->ldp_enable = OSPF_IF_LDP_SYNC_DISABLE;

	
	UNSET_IF_PARAM (IF_DEF_PARAMS (ifp), bfd_flag);
    IF_DEF_PARAMS (ifp)->bfd_flag = OSPF_IF_BFD_DISABLE;
	UNSET_IF_PARAM (IF_DEF_PARAMS (ifp), if_bfd_recv_interval);
    IF_DEF_PARAMS (ifp)->if_bfd_recv_interval = OSPF_IF_BFD_RECV_INTERVAL_DEFAULT;
	UNSET_IF_PARAM (IF_DEF_PARAMS (ifp), if_bfd_send_interval);
    IF_DEF_PARAMS (ifp)->if_bfd_send_interval = OSPF_IF_BFD_SEND_INTERVAL_DEFAULT;
	UNSET_IF_PARAM (IF_DEF_PARAMS (ifp), if_bfd_detect_multiplier);
    IF_DEF_PARAMS (ifp)->if_bfd_detect_multiplier = OSPF_IF_BFD_DETECT_MULT_DEFAULT;
}

void
ospf_if_init (void)
{
    /* Initialize Zebra interface data structure. */
    om->iflist = iflist;
    if_add_hook (IF_NEW_HOOK, ospf_if_new_hook);
    if_add_hook (IF_DELETE_HOOK, ospf_if_delete_hook);
}


