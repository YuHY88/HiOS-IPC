/* OSPF version 2 daemon program.
   Copyright (C) 1999, 2000 Toshiaki Takada

This file is part of GNU Zebra.

GNU Zebra is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Zebra is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Zebra; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */
#include "lib/fifo.h"
#include "lib/memory.h"
//#include "sockunion.h"          /* for inet_aton () */
#include "lib/errcode.h"
#include "lib/thread.h"
#include "lib/stream.h"
#include "lib/devm_com.h"
#include "lib/table.h"
#include "lib/memtypes.h"
#include "lib/timer.h"
#include "lib/ospf_common.h"


#include "ospfd/ospfd.h"
#include "ospfd/ospf_spf.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_nsm.h"
#include "ospfd/ospf_ism.h"
#include "ospfd/ospf_dump.h"
#include "ospfd/ospf_ipc.h"
#include "ospfd/ospf_zebra.h"
#include "ospfd/ospf_route.h"
#include "ospfd/ospf_abr.h"
#include "ospfd/ospf_ase.h"



/* OSPF instance wide configuration. */
static struct ospf_master ospf_master;
//extern struct ospf_statics ospf_sta;

/* OSPF instance wide configuration pointer to export. */
struct ospf_master *om;

//extern struct in_addr router_id_zebra;


void ospf_remove_vls_through_area (struct ospf *, struct ospf_area *);
static void ospf_network_free (struct ospf *, struct ospf_network *);
static void ospf_network_run (struct prefix *, struct ospf_area *);
static void ospf_network_run_interface (struct ospf *, struct interface *,
                                        struct prefix *, struct ospf_area *);
static void ospf_network_run_subnet (struct ospf *, struct connected *,
                                     struct prefix *, struct ospf_area *);
static int ospf_network_match_iface (const struct connected *,
                                     const struct prefix *);
static void ospf_finish_final (struct ospf *);

//#define OSPF_EXTERNAL_LSA_ORIGINATE_DELAY 1

void
ospf_router_id_update (struct ospf *ospf)
{
    struct in_addr router_id;
	struct in_addr router_id_old;
    struct listnode *node = NULL;
    struct ospf_interface *oi = NULL;
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "Router-ID[OLD:%s]: Update", inet_ntoa (ospf->router_id));
    }
	
    router_id_old = ospf->router_id;
    /* Select the router ID based on these priorities:
       1. Statically assigned router ID is always the first choice.
       2. If there is no statically assigned router ID, then try to stick
          with the most recent value, since changing router ID's is very
      disruptive.
       3. Last choice: just go with whatever the zebra daemon recommends.
    */
    if (ospf->router_id_static.s_addr != 0)
    {
        router_id = ospf->router_id_static;
    }
    else if (ospf->router_id.s_addr != 0)
    {
        router_id = ospf->router_id;
    }
    else
    {
        router_id = router_id_zebra;
    }
	
    ospf->router_id = router_id;
	
    if (IS_DEBUG_OSPF_EVENT)
    {
        zlog_debug (OSPF_DBG_EVENT, "Router-ID[NEW:%s]: Update", inet_ntoa (ospf->router_id));
    }
	
    if (!IPV4_ADDR_SAME (&router_id_old, &router_id))
    {
        for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, node, oi))
        {
            /* Some nbrs are identified by router_id, these needs
                  * to be rebuilt. Possible optimization would be to do
                  * oi->nbr_self->router_id = router_id for
                  * !(virtual | ptop) links
                  */
            ospf_nbr_self_reset (oi);
        }
        /* If AS-external-LSA is queued, then flush those LSAs. */
        if (router_id_old.s_addr == 0 && ospf->external_origin)
        {
            ospf_schedule_asbr_task(ospf);
            //thread_add_event (master_ospf, ospf_external_lsa_originate_timer,
            //ospf, 0);
        }
        /* update router-lsa's for each area */
        ospf_router_lsa_update (ospf);
        /* update ospf_interface's */
        for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, node, oi))
        {
            ospf_if_update (ospf, oi->ifp);
        }
    }
}

/* For OSPF area sort by area id. */
static int
ospf_area_id_cmp (struct ospf_area *a1, struct ospf_area *a2)
{
    if (ntohl (a1->area_id.s_addr) > ntohl (a2->area_id.s_addr))
    {
        return 1;
    }
    if (ntohl (a1->area_id.s_addr) < ntohl (a2->area_id.s_addr))
    {
        return -1;
    }
    return 0;
}

/*******************************************************************************************

*Function: 	Allocate new ospf structure.
*Parameter:	u_int16_t ospf_id //0-255 instance id
*return: 	return ospf instance

********************************************************************************************/

static struct ospf *
ospf_new (u_int16_t ospf_id)
{
    int i, j;
	struct ospf *new_ospf = NULL;
	
	do
  	{
  		new_ospf = XCALLOC (MTYPE_OSPF_TOP, sizeof (struct ospf));
  		if (new_ospf == NULL)
  		{
    		zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
  		}
  	}while(new_ospf == NULL);
	
	if (new_ospf == NULL)
    {
        zlog_err("Can't creat ospf : malloc failed");
        return (struct ospf *)NULL;
    }
	memset(new_ospf, 0, sizeof(struct ospf));

    new_ospf->router_id.s_addr = htonl (0);
    new_ospf->router_id_static.s_addr = htonl (0);
    new_ospf->ospf_id = ospf_id;

    /* Set to "enabled OSPF_RFC1583_COMPATIBLE" by default */
    SET_FLAG (new_ospf->config, OSPF_RFC1583_COMPATIBLE);

    new_ospf->abr_type = OSPF_ABR_DEFAULT;
    new_ospf->oiflist = list_new ();
    new_ospf->vlinks = list_new ();
    new_ospf->areas = list_new ();
    new_ospf->areas->cmp = (int (*)(void *, void *)) ospf_area_id_cmp;
    new_ospf->networks = route_table_init ();
    new_ospf->nbr_nbma = route_table_init ();

    new_ospf->lsdb = ospf_lsdb_new ();
    new_ospf->summary = route_table_init ();

    new_ospf->default_originate = DEFAULT_ORIGINATE_NONE;

    new_ospf->passive_interface_default = OSPF_IF_PASSIVE;

    new_ospf->new_external_route = route_table_init ();
    new_ospf->old_external_route = route_table_init ();
    new_ospf->external_lsas = route_table_init ();

    new_ospf->stub_router_startup_time = OSPF_STUB_ROUTER_UNCONFIGURED;
    new_ospf->stub_router_shutdown_time = OSPF_STUB_ROUTER_UNCONFIGURED;
    new_ospf->stub_router_admin_set     = OSPF_STUB_ROUTER_ADMINISTRATIVE_UNSET;

    /* Clear redistribution flags. */
    for (i = 0; i <= ROUTE_PROTO_MAX; i++)
    {
        for (j = 0; j <= OSPF_MAX_PROCESS_ID; j++)
        {
            new_ospf->dmetric[i][j].type = -1;
            new_ospf->dmetric[i][j].value = -1;
            new_ospf->dmetric[i][j].redist = vrf_bitmap_init ();
        }
    }
    new_ospf->default_metric = -1;
    new_ospf->ref_bandwidth = OSPF_DEFAULT_REF_BANDWIDTH;

    /* LSA timers */
    new_ospf->min_ls_interval = OSPF_MIN_LS_INTERVAL;
    new_ospf->min_ls_arrival = OSPF_MIN_LS_ARRIVAL;

    /* SPF timer value init. */
    new_ospf->spf_delay = OSPF_SPF_DELAY_DEFAULT;
    new_ospf->spf_holdtime = OSPF_SPF_HOLDTIME_DEFAULT;
    new_ospf->spf_max_holdtime = OSPF_SPF_MAX_HOLDTIME_DEFAULT;
    new_ospf->spf_hold_multiplier = 1;

    /* MaxAge init. */
    new_ospf->maxage_delay = OSPF_LSA_MAXAGE_REMOVE_DELAY_DEFAULT;
    new_ospf->maxage_lsa = route_table_init();
    /*new_ospf->t_maxage_walker =
        thread_add_timer (master_ospf, ospf_lsa_maxage_walker,
                          new_ospf, OSPF_LSA_MAXAGE_CHECK_INTERVAL);*/
	
	new_ospf->t_maxage_walker =
        high_pre_timer_add ((char *)"ospf_timer", LIB_TIMER_TYPE_NOLOOP,\
        			ospf_lsa_maxage_walker, new_ospf, (OSPF_LSA_MAXAGE_CHECK_INTERVAL)*1000);

    /* Distance table init. */
    new_ospf->distance_table = route_table_init ();

    /*distance default*/
    new_ospf->distance_all = ROUTE_METRIC_OSPF;
    new_ospf->refresh_interval = OSPF_LS_REFRESH_TIME;
    new_ospf->lsa_refresh_queue.index = 0;
    new_ospf->lsa_refresh_interval = OSPF_LSA_REFRESH_INTERVAL_DEFAULT;
    /*new_ospf->t_lsa_refresher = thread_add_timer (master_ospf, ospf_lsa_refresh_walker,
                           new_ospf, new_ospf->lsa_refresh_interval);*/
	new_ospf->t_lsa_refresher = high_pre_timer_add ((char *)"ospf_lsa_fresh_timer", LIB_TIMER_TYPE_NOLOOP,\
								ospf_lsa_refresh_walker, new_ospf, (new_ospf->lsa_refresh_interval)*1000);
    new_ospf->lsa_refresher_started = time_get_real_time (NULL);

    new_ospf->oi_write_q = list_new ();
    new_ospf->vpn = 0; 
    new_ospf->dcn_enable = OSPF_DCN_DISABLE;
    new_ospf->dcn_report = OSPF_DCN_REPORT_DISABLE;


    int redist_default = ROUTE_PROTO_OSPF;

    /* Set unwanted redistribute route.  bgpd does not need BGP route
     redistribution. */
    new_ospf->redist_default = redist_default;
    new_ospf->redist_name = list_new ();

    /* Set default-information redistribute to zero. */
    //new_ospf->default_information = vrf_bitmap_init ();

    //vrf_bitmap_set (new->dmetric[ROUTE_PROTO_OSPF][ospf_id].redist, vrf_id);

	new_ospf->all_interfaces_bfd_enable = OSPF_ALL_IF_BFD_DISABLE;
	new_ospf->bfd_recv_interval = 10;		/*default min recv_interval 10ms*/
	new_ospf->bfd_send_interval = 10;		/*default min send_interval 10ms*/
	new_ospf->bfd_detect_multiplier = 3;	/*default detect multiplier 3 times*/
	
    return new_ospf;
}

/*******************************************************************************************

*Function: 	According to id find ospf instance
*Parameter:	u_int16_t ospf_id //0-255 instance id
*return: 	If you find a return ospf instance，else return NULL

********************************************************************************************/
struct ospf *
ospf_lookup_id (u_int16_t ospf_id)
{
    struct ospf *ospf = NULL;
    struct listnode *node = NULL;
	struct listnode *nnode = NULL;
	
    for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
    {
        if ((ospf != NULL) && (ospf->ospf_id == ospf_id))
        {
            return ospf;
        }
    }

    return NULL;
}


struct ospf *
ospf_lookup ()
{	
	struct listnode *ospf_node = NULL;
	
    if (listcount (om->ospf) == 0)
    {
        return NULL;
    }
	
	ospf_node = listhead(om->ospf);
	if(ospf_node == NULL)
	{
		return NULL;
	}
	
	return listgetdata(ospf_node);
    //return listgetdata((struct listnode *)listhead(om->ospf));
}

static int
ospf_is_ready (struct ospf *ospf)
{
    /* OSPF must be on and Router-ID must be configured. */
    if (!ospf || ospf->router_id.s_addr == 0)
    {
        return 0;
    }
    return 1;
}

static void
ospf_add (struct ospf *ospf)
{
    listnode_add_sort (om->ospf, ospf);
}

static void
ospf_delete (struct ospf *ospf)
{
    listnode_delete (om->ospf, ospf);
}

/*******************************************************************************************

*Function: 	According to id find ospf instance or create a ospf instance
*Parameter:	u_int16_t ospf_id //0-255 instance id
*return: 	return ospf instance

********************************************************************************************/
struct ospf *
ospf_get (u_int16_t ospf_id)
{
    struct ospf *ospf = NULL;

    ospf = ospf_lookup_id(ospf_id);
    if (ospf == NULL)
    {
        ospf = ospf_new (ospf_id);
        ospf_add (ospf);
        if (ospf->router_id_static.s_addr == 0)
        {
            ospf_router_id_update (ospf);
        }
        ospf_opaque_type11_lsa_init (ospf);
    }

    return ospf;
}

/* Handle the second half of deferred shutdown. This is called either
 * from the deferred-shutdown timer thread, or directly through
 * ospf_deferred_shutdown_check.
 *
 * Function is to cleanup G-R state, if required then call ospf_finish_final
 * to complete shutdown of this ospf instance. Possibly exit if the
 * whole instance is being shutdown and this was the last OSPF instance.
 */
static void
ospf_deferred_shutdown_finish (struct ospf *ospf)
{
    ospf->stub_router_shutdown_time = OSPF_STUB_ROUTER_UNCONFIGURED;
    OSPF_TIMER_OFF (ospf->t_deferred_shutdown);
    ospf_finish_final (ospf);
    /* *ospf is now invalid */
    /* ospfd being shut-down? If so, was this the last ospf instance? */
    if (CHECK_FLAG (om->options, OSPF_MASTER_SHUTDOWN)
            && (listcount (om->ospf) == 0))
    {
        exit (0);
    }
    return;
}

/* Timer thread for G-R */
static int
ospf_deferred_shutdown_timer (void *t)
{
    struct ospf *ospf = (struct ospf *)(t);
	
    if(ospf == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
    assert(ospf);
    //ospf->t_deferred_shutdown = NULL;
	ospf->t_deferred_shutdown = 0;
    ospf_deferred_shutdown_finish (ospf);
    return 0;
}

/* Check whether deferred-shutdown must be scheduled, otherwise call
 * down directly into second-half of instance shutdown.
 */
static void
ospf_deferred_shutdown_check (struct ospf *ospf)
{
    unsigned long timeout;
    struct listnode *ln = NULL;
    struct ospf_area *area = NULL;
	
    /* deferred shutdown already running? */
    if (ospf->t_deferred_shutdown)
    {
        return;
    }
    /* Should we try push out max-metric LSAs? */
    if (ospf->stub_router_shutdown_time != OSPF_STUB_ROUTER_UNCONFIGURED)
    {
        for (ALL_LIST_ELEMENTS_RO (ospf->areas, ln, area))
        {
            SET_FLAG (area->stub_router_state, OSPF_AREA_ADMIN_STUB_ROUTED);
            if (!CHECK_FLAG (area->stub_router_state, OSPF_AREA_IS_STUB_ROUTED))
            {
                ospf_router_lsa_update_area (area);
            }
        }
        timeout = ospf->stub_router_shutdown_time;
    }
    else
    {
        /* No timer needed */
        ospf_deferred_shutdown_finish (ospf);
        return;
    }
    OSPF_TIMER_ON (ospf->t_deferred_shutdown, ospf_deferred_shutdown_timer,
                   timeout);
    return;
}

/* Shut down the entire instance */
void
ospf_terminate (void)
{
    struct ospf *ospf = NULL;
    struct listnode *node = NULL;
	struct listnode	*nnode = NULL;
    struct routefifo *routefifo_entry = NULL;
	
    /* shutdown already in progress */
    if (CHECK_FLAG (om->options, OSPF_MASTER_SHUTDOWN))
    {
        return;
    }
    SET_FLAG (om->options, OSPF_MASTER_SHUTDOWN);
    /* exit immediately if OSPF not actually running */
    if (listcount(om->ospf) == 0)
    {
        ospf_pkt_unregister();
        exit(0);
    }
    ospf_pkt_unregister();
    for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
    {
        ospf_finish (ospf);
    }
    /* Deliberately go back up, hopefully to thread scheduler, as
     * One or more ospf_finish()'s may have deferred shutdown to a timer
     * thread
     */
    /*释放异步发送路由FIFO*/
    while (!FIFO_EMPTY(&om->routefifo))
    {
        routefifo_entry = (struct routefifo *)FIFO_TOP(&om->routefifo);
        if (routefifo_entry)
        {
            FIFO_DEL(routefifo_entry);
            XFREE (MTYPE_ROUTE_FIFO, routefifo_entry);
        }
    }
}

void
ospf_finish (struct ospf *ospf)
{
	OSPF_LOG_DEBUG();
    /* let deferred shutdown decide */
    ospf_deferred_shutdown_check (ospf);
    /* if ospf_deferred_shutdown returns, then ospf_finish_final is
     * deferred to expiry of G-S timer thread. Return back up, hopefully
     * to thread scheduler.
     */
    return;
}
#if 0
static void
ospf_router_id_update_as_external_lsa (struct ospf *ospf)
{
    //struct interface *ifp;
    struct listnode *node;
    struct ospf_interface *oi;
    thread_add_event (master_ospf, ospf_external_lsa_originate_timer,
                      ospf, 0);
    /* update router-lsa's for each area */
    ospf_router_lsa_update (ospf);
    /* update ospf_interface's */
    for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, node, oi))
    {
        ospf_if_update (ospf, oi->ifp);
    }
}
#endif
static void
ospf_area_reset (struct ospf_area *area)
{
    struct ospf_lsa *lsa = NULL;
    struct route_node *rn = NULL;
	
	if(NULL == area)
	{
		zlog_err("%s %d area is NULL",__func__,__LINE__);
		return ;
	}
    /* Free LSDBs. */
    if(listcount (area->oiflist) > 0 )
    {
        LSDB_LOOP (ROUTER_LSDB (area), rn, lsa)
        {
            ospf_discard_from_db (area->ospf, area->lsdb, lsa);
        }
        LSDB_LOOP (NETWORK_LSDB (area), rn, lsa)
        {
            ospf_discard_from_db (area->ospf, area->lsdb, lsa);
        }
        LSDB_LOOP (SUMMARY_LSDB (area), rn, lsa)
        {
            ospf_discard_from_db (area->ospf, area->lsdb, lsa);
        }
        LSDB_LOOP (ASBR_SUMMARY_LSDB (area), rn, lsa)
        {
            ospf_discard_from_db (area->ospf, area->lsdb, lsa);
        }
        LSDB_LOOP (NSSA_LSDB (area), rn, lsa)
        {
            ospf_discard_from_db (area->ospf, area->lsdb, lsa);
        }
        LSDB_LOOP (OPAQUE_AREA_LSDB (area), rn, lsa)
        {
            ospf_discard_from_db (area->ospf, area->lsdb, lsa);
        }
        LSDB_LOOP (OPAQUE_LINK_LSDB (area), rn, lsa)
        {
            ospf_discard_from_db (area->ospf, area->lsdb, lsa);
        }
        ospf_lsdb_delete_all (area->lsdb);
        /* Cancel timer. */
        //OSPF_TIMER_OFF (area->t_stub_router);
        //OSPF_TIMER_OFF (area->t_opaque_lsa_self);
    }
}

static void
ospf_router_id_set (struct ospf *ospf)
{
    struct in_addr router_id;
	
    /* Select the router ID based on these priorities:
       1. Statically assigned router ID is always the first choice.
       2. If there is no statically assigned router ID, then try to stick
          with the most recent value, since changing router ID's is very
      disruptive.
       3. Last choice: just go with whatever the zebra daemon recommends.
    */
    if (ospf->router_id_static.s_addr != 0)
    {
        router_id = ospf->router_id_static;
    }
    else if (ospf->router_id.s_addr != 0)
    {
        router_id = ospf->router_id;
    }
    else
    {
        router_id = router_id_zebra;
    }
    ospf->router_id = router_id;
}


/* Final cleanup of ospf instance */
static void
ospf_reset_stage1(struct ospf *ospf)
{
    ospf->restarting = 1;
    ospf_opaque_type11_lsa_term (ospf);
    ospf_opaque_type11_lsa_init(ospf);
    ospf_flush_self_originated_lsas_now (ospf);
    ospf_external_self_originated_lsas_now(ospf);
}


/* Final cleanup of ospf instance */
static int
ospf_reset_stage2(void *thread)
{
    struct ospf_lsa *lsa = NULL;
    struct route_node *rn = NULL;    
	struct listnode *node = NULL;
    struct listnode *nnode = NULL;
    struct ospf_area *area = NULL;
    struct ospf_interface *oi = NULL;
    struct ospf *ospf = (struct ospf *)(thread);
    //struct ospf_vl_data *vl_data;
	
    if(ospf == NULL)
    {
        zlog_warn("%s[%d] get thread arg is NULL",__func__,__LINE__);
        return 0;
    }
	
    //ospf->t_reset = NULL;
	ospf->t_reset = 0;
	
    //if(ospf->router_id_static.s_addr != ospf->router_id.s_addr)
    ospf_router_id_set(ospf);
	
    /* Reset interface. */
    for (ALL_LIST_ELEMENTS (ospf->oiflist, node, nnode, oi))
    {
        if(oi->state > ISM_Down)
        {
            ospf_if_down (oi);
            ospf_opaque_type9_lsa_term (oi);
        }
    }
	
    /* Clear networks and Areas. */
    for (ALL_LIST_ELEMENTS (ospf->areas, node, nnode, area))
    {
        ospf_area_reset (area);
    }
	
    /* Cancel all timers. */
    OSPF_TIMER_OFF (ospf->t_external_lsa);
    OSPF_TIMER_OFF (ospf->t_ase_calc);
    OSPF_TIMER_OFF (ospf->t_spf_calc);
    OSPF_TIMER_OFF (ospf->t_abr_task);
    OSPF_TIMER_OFF (ospf->t_maxage_walker);
    //OSPF_TIMER_OFF (ospf->t_maxage);
    OSPF_TIMER_OFF (ospf->t_asbr_check);
    OSPF_TIMER_OFF (ospf->t_distribute_update);
    //OSPF_TIMER_OFF (ospf->t_lsa_refresher);
    OSPF_TIMER_OFF (ospf->t_opaque_lsa_self);
    //OSPF_TIMER_OFF (om->t_read);
    
    OSPF_TIMER_OFF (ospf->t_lsa_refresher);
	OSPF_TIMER_OFF (ospf->t_external_fw);
	OSPF_TIMER_OFF (ospf->t_nssa_fw);
	
    //om->t_read = thread_add_event_normal (master_ospf, ospf_pkt_rcv, NULL, 0);
	
    LSDB_LOOP (OPAQUE_AS_LSDB (ospf), rn, lsa)
    {
        ospf_discard_from_db (ospf, ospf->lsdb, lsa);
    }
	
    LSDB_LOOP (EXTERNAL_LSDB (ospf), rn, lsa)
    {
        ospf_discard_from_db (ospf, ospf->lsdb, lsa);
    }
	
    ospf_lsdb_delete_all (ospf->lsdb);
    for (rn = route_top (ospf->maxage_lsa); rn; rn = route_next (rn))
    {
        if ((lsa = rn->info) != NULL)
        {
            ospf_lsa_unlock (&lsa);
            rn->info = NULL;
        }
        //route_unlock_node (rn);
    }
	
    if (ospf->maxage_lsa)
    {
        //ospf_ase_external_lsas_finish (ospf->maxage_lsa);
        route_table_finish (ospf->maxage_lsa);
        ospf->maxage_lsa = route_table_init ();
	}
    if (ospf->old_table)
    {
        ospf_route_table_free (ospf->old_table);
        ospf->old_table = route_table_init ();
    }
    if (ospf->new_table)
    {
        ospf_route_delete (ospf->new_table,ospf);
        ospf_route_table_free (ospf->new_table);
        ospf->new_table = route_table_init ();
    }
    if (ospf->old_rtrs)
    {
        ospf_rtrs_free (ospf->old_rtrs);
        ospf->old_rtrs = route_table_init ();
    }
    if (ospf->new_rtrs)
    {
        ospf_rtrs_free (ospf->new_rtrs);
        ospf->new_rtrs = route_table_init ();
    }
    if (ospf->new_external_route)
    {
        ospf_route_delete (ospf->new_external_route,ospf);
        ospf_route_table_free (ospf->new_external_route);
        ospf->new_external_route = route_table_init ();
    }
    if (ospf->old_external_route)
    {
        ospf_route_delete (ospf->old_external_route,ospf);
        ospf_route_table_free (ospf->old_external_route);
        ospf->old_external_route = route_table_init ();
    }
    if (ospf->external_lsas)
    {
        ospf_ase_external_lsas_finish (ospf->external_lsas);
        ospf->external_lsas = route_table_init ();
    }
    /*ospf->t_maxage_walker =
        thread_add_timer (master_ospf, ospf_lsa_maxage_walker,
                          ospf, OSPF_LSA_MAXAGE_CHECK_INTERVAL);*/
	OSPF_TIMER_ON (ospf->t_maxage_walker, ospf_lsa_maxage_walker,
                          OSPF_LSA_MAXAGE_CHECK_INTERVAL);
	
    ospf->lsa_refresh_queue.index = 0;
    ospf->lsa_refresher_started = time_get_real_time (NULL);
    /*ospf->t_lsa_refresher = thread_add_timer (master_ospf, ospf_lsa_refresh_walker,
                           ospf, ospf->lsa_refresh_interval);*/
	ospf->t_lsa_refresher = high_pre_timer_add ((char *)"ospf_lsa_fresh_timer", LIB_TIMER_TYPE_NOLOOP,\
							ospf_lsa_refresh_walker, ospf, (ospf->lsa_refresh_interval)*1000);
    if(ospf->oi_write_q)
    {
        /* If packets still remain in queue, call write thread. */
        if (!list_isempty (ospf->oi_write_q))
        {
            zlog_err ("%s():%d: ospfd terniated!", __FUNCTION__, __LINE__);
        }
    }
    memset(&ospf_sta,0,sizeof(struct ospf_statics));
    ospf->restarting = 0;
    for (ALL_LIST_ELEMENTS (ospf->oiflist, node, nnode, oi))
    {
        if(ospf_if_is_up(oi))
        {
            ospf_if_up (oi);
        }
    }
    ospf_schedule_abr_task (ospf);
    ospf_schedule_asbr_task(ospf);
    return 0;
}



/* Final cleanup of ospf instance */
void
ospf_reset(struct ospf *ospf)
{
    if(ospf == NULL)
    {
        zlog_warn("%s[%d] OSPF is NULL",__func__,__LINE__);
        return ;
    }
	
    if(ospf->t_reset)
    {
        OSPF_TIMER_OFF (ospf->t_reset);
    }
	
    ospf_reset_stage1(ospf);

    //ospf->t_reset = thread_add_timer (master_ospf, ospf_reset_stage2, ospf, 5);
	OSPF_TIMER_ON(ospf->t_reset, ospf_reset_stage2, 5);
}

int send_ne_ip_singal_to_snmpd(uint32_t ne_ip, enum IPC_OPCODE opcode)
{
	int ret = 0;
	uint32_t temp_ne_ip = 0;
	
	temp_ne_ip = ne_ip;
	ret = ipc_send_msg_n2(&(temp_ne_ip), sizeof(uint32_t), 1,
											   MODULE_ID_SNMPD, MODULE_ID_OSPF,
											   IPC_TYPE_SYNCE, IPC_SUB_TYPE_NE_IP, opcode, 0);
	return ret;
}


int send_dcn_singal_to_target_module(u_char dcn_enable, int module_id,
							enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode)
{
	int ret = 0;
	u_char dcn_flag = 0;
	
	dcn_flag = dcn_enable;
	ret = ipc_send_msg_n2(&(dcn_flag), sizeof(u_char), 1,
											   module_id, MODULE_ID_OSPF,
											   msg_type, subtype, opcode, 0);
	return ret;
}

void interface_cmd_config_for_dcn(struct ospf *ospf, struct prefix_ipv4 *p)
{	
	int i = 0;
	int ret = 0;
	int if_num = 0;
	int if_type = 0;
	//uint16_t temp_vpn = 0;
	uint32_t ifindex = 0;
	char if_name[100];
	char cmd_string[1024] = {0};
	const char *type_name = NULL;
	struct ifm_info *p_ifinfo = NULL;
	struct ifm_info *tp_ifinfo = NULL;
	
    if(IS_DEBUG_OSPF_DCN)
	{
		zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	
	if(NULL == ospf)
	{
		zlog_err("%s[%d]: In function '%s' The para p_ospf == NULL\n", __FILE__, __LINE__, __func__);
		return;
	}

	/*
	Note here: Send dcn status signal to ifm is separate. dcn disable must be on the front,
	dcn enable must be on the back. Otherwise you will not be able to access the 4094 subinterface.
	*/
	if(ospf->dcn_enable == OSPF_DCN_DISABLE)
	{
		send_dcn_singal_to_target_module(ospf->dcn_enable, MODULE_ID_IFM, IPC_TYPE_IFM, OSPF_DCN_ENABLE, 0);
		send_dcn_singal_to_target_module(ospf->dcn_enable, MODULE_ID_L2, \
												IPC_TYPE_OSPF_DCN, IPC_SUB_TYPE_DCN_STATUS, IPC_OPCODE_UPDATE);
		/*ret = ipc_send_msg_n2(&(temp_vpn), sizeof(uint16_t), 1,
											   MODULE_ID_SNMPD, MODULE_ID_OSPF,
											   IPC_TYPE_SYNCE, IPC_SUB_TYPE_VPN, IPC_OPCODE_DELETE, 0);
		send_ne_ip_singal_to_snmpd(0, IPC_OPCODE_DELETE);
		*/
		if(IS_DEBUG_OSPF_DCN)
		{
			zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s': send dcn disable signal to ifm [ret = %d]!\n\n",\
								__FILE__, __LINE__, __func__, ret);
		}
	}

	
	do{
		p_ifinfo = ifm_get_bulk(ifindex, MODULE_ID_OSPF, &if_num );
		tp_ifinfo = p_ifinfo;
		if(NULL != p_ifinfo)
		{
			if(IS_DEBUG_OSPF_DCN)
			{
				zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s':  get if from ifm_get_bulk()-->ifindex = %d if_num : %d\n",\
									__FILE__, __LINE__, __func__, ifindex, if_num);
			}
		
			for(i = 0 ; i < if_num ; i++)
			{
				ifindex    = p_ifinfo[i].ifindex;
				if_type = IFM_TYPE_ID_GET (p_ifinfo[i].ifindex);
				memset(if_name, 0, sizeof(if_name));
				ifm_get_name_by_ifindex(ifindex, if_name);

				if(IS_DEBUG_OSPF_DCN)
				{
					zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s':   the if name get from ifm_get_bulk()-->if_name:%s\n", \
										__FILE__, __LINE__, __func__, if_name);
				}
				/* Add the interface type from here */
				if(((if_type == IFNET_TYPE_ETHERNET) || (if_type == IFNET_TYPE_GIGABIT_ETHERNET) || (if_type == IFNET_TYPE_XGIGABIT_ETHERNET))
					&& (!IFM_IS_SUBPORT ( ifindex ))
					&& (IFM_SLOT_ID_GET ( ifindex ) > 0))
				{
			    	type_name = ifm_get_typestr ( if_type );
					/*
					Use ipc_send_common () here instead of ipc_send_common_wait_ack (),
					because ospf is a single thread, which will lead to IPC timeout.
					*/
					if(ospf->dcn_enable == OSPF_DCN_ENABLE)
					{
						/*
						Enter the physical interface and configure LLDP.
						*/
#if 0
						memset(cmd_string, 0, sizeof(cmd_string));
						sprintf(cmd_string, "interface %s %d/%d/%d",
											type_name,
											IFM_UNIT_ID_GET ( ifindex ),
											IFM_SLOT_ID_GET ( ifindex ),
											IFM_PORT_ID_GET ( ifindex ));
						
						ret = ipc_send_msg_n2(cmd_string, strlen(cmd_string), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0);
						
						if(IS_DEBUG_OSPF_DCN)
						{
	        				zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n", cmd_string, ret);
						}

						/*
						lldp enable
						*/
						memset(cmd_string, 0, sizeof(cmd_string));
						sprintf(cmd_string, "lldp enable");
						ret = ipc_send_msg_n2(cmd_string, strlen(cmd_string), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0);

						if(IS_DEBUG_OSPF_DCN)
						{
							zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n", cmd_string, ret);
						}

						/*
						lldp management-address arp-learning vlan 4094
						*/
						memset(cmd_string, 0, sizeof(cmd_string));
						sprintf(cmd_string, "lldp management-address arp-learning vlan 4094");
						ret = ipc_send_msg_n2(cmd_string, strlen(cmd_string), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0);

						if(IS_DEBUG_OSPF_DCN)
						{
							zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n", cmd_string, ret);
						}
						/*
						end
						*/
						ret = ipc_send_msg_n2((void *)"end", strlen("end"), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0);
						if(IS_DEBUG_OSPF_DCN)
						{
							zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n\n", "end", ret);
						}
#endif
						/*
						Enter the 4094 sub-interface and configure ospf related functions.
						*/
						memset(cmd_string, 0, sizeof(cmd_string));
						sprintf(cmd_string, "interface %s %d/%d/%d.4094",
											type_name,
											IFM_UNIT_ID_GET ( ifindex ),
											IFM_SLOT_ID_GET ( ifindex ),
											IFM_PORT_ID_GET ( ifindex ));
						
						ret = ipc_send_msg_n2(cmd_string, strlen(cmd_string), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0);
						
						if(IS_DEBUG_OSPF_DCN)
						{
	        				zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n", cmd_string, ret);
						}

						/*
						add 4094 sub-interface to DCN_VPN.
						*/
						memset(cmd_string, 0, sizeof(cmd_string));
						sprintf(cmd_string, "ip l3vpn 127");
						
						ret = ipc_send_msg_n2(cmd_string, strlen(cmd_string), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0);
						
						if(IS_DEBUG_OSPF_DCN)
						{
	        				zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n", cmd_string, ret);
						}
						
						/*
						encapsulate dot1q 4094
						*/
						memset(cmd_string, 0, sizeof(cmd_string));
						sprintf(cmd_string, "encapsulate dot1q 4094");
						ret = ipc_send_msg_n2(cmd_string, strlen(cmd_string), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0);

						if(IS_DEBUG_OSPF_DCN)
						{
							zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n", cmd_string, ret);
						}

						/*
						ip address unnumbered interface loopback 0
						*/
						memset(cmd_string, 0, sizeof(cmd_string));
						sprintf(cmd_string, "ip address unnumbered interface loopback 128");
	                    ret = ipc_send_msg_n2(cmd_string, strlen(cmd_string), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0);

						if(IS_DEBUG_OSPF_DCN)
						{
							zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n", cmd_string, ret);
						}
						
						//usleep(1000*10);
						/*
						ip ospf network p2p
						*/
						memset(cmd_string, 0, sizeof(cmd_string));
						sprintf(cmd_string, "ip ospf network p2p");
	                    ret = ipc_send_msg_n2(cmd_string, strlen(cmd_string), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0);
						if(IS_DEBUG_OSPF_DCN)
						{
							zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n", cmd_string, ret);
						}
						/*
						memset(cmd_string, 0, sizeof(cmd_string));
						sprintf(cmd_string, "arp proxy");
	                    ret = ipc_send_common(cmd_string, strlen(cmd_string), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0);
						if(IS_DEBUG_OSPF_DCN)
						{
							zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n", cmd_string, ret);
						}
						*/
	                    ret = ipc_send_msg_n2((void *)"end", strlen("end"), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0);
						if(IS_DEBUG_OSPF_DCN)
						{
							zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n\n", "end", ret);
						}
					}
					else
					{
						/*memset(cmd_string, 0, sizeof(cmd_string));
						sprintf(cmd_string, "no interface %s %d/%d/%d.4094 \r\n",
											type_name,
											IFM_UNIT_ID_GET ( ifindex ),
											IFM_SLOT_ID_GET ( ifindex ),
											IFM_PORT_ID_GET ( ifindex ));

						ret = ipc_send_msg_n2(cmd_string, strlen(cmd_string), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0);
						if(IS_DEBUG_OSPF_DCN)
						{
							zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n", cmd_string, ret);
						}
						//usleep(1000*100);
						
						ret = ipc_send_msg_n2((void *)"end", strlen("end"), 1,
	                                       MODULE_ID_VTY, MODULE_ID_OSPF,
	                                       IPC_TYPE_VTY, VTY_MSG_CMD, 0, 0);
						if(IS_DEBUG_OSPF_DCN)
						{
							zlog_debug(OSPF_DBG_DCN, "send cmd [%s] ret [%d]!\n\n", "end", ret);
						}
						*/
					}
				}
			}

            
            mem_share_free_bydata(tp_ifinfo, MODULE_ID_OSPF);
		}
		else
		{
			if(0 != if_num)
			{
				OSPF_LOG_ERROR(" get if from ifm_get_bulk() p_ifinfo == NULL and if_num : %d\n", if_num);
			}
	
		}
	}while(if_num > 0);

	/*
	Note here: Send dcn status signal to ifm is separate. dcn disable must be on the front,
	dcn enable must be on the back. Otherwise you will not be able to access the 4094 subinterface.
	*/
	if(ospf->dcn_enable == OSPF_DCN_ENABLE)
	{
		send_dcn_singal_to_target_module(ospf->dcn_enable, MODULE_ID_IFM, IPC_TYPE_IFM, OSPF_DCN_ENABLE, 0);
		send_dcn_singal_to_target_module(ospf->dcn_enable, MODULE_ID_L2, \
												IPC_TYPE_OSPF_DCN, IPC_SUB_TYPE_DCN_STATUS, IPC_OPCODE_UPDATE);
		/* Used to notify EFM to trigger the U0 device add operation.*/
		send_dcn_singal_to_target_module(ospf->dcn_enable, MODULE_ID_L2, \
												IPC_TYPE_EFM, IPC_SUB_TYPE_DCN_STATUS, IPC_OPCODE_ENABLE);
		/*ret = ipc_send_msg_n2(&(ospf->vpn), sizeof(uint16_t), 1,
											   MODULE_ID_SNMPD, MODULE_ID_OSPF,
											   IPC_TYPE_SYNCE, IPC_SUB_TYPE_VPN, IPC_OPCODE_ADD, 0);
		
		send_ne_ip_singal_to_snmpd(ntohl(p->prefix.s_addr), IPC_OPCODE_ADD);
		*/
		if(IS_DEBUG_OSPF_DCN)
		{
			zlog_debug(OSPF_DBG_DCN, "send dcn enable signal to ifm [ret = %d]!\n\n", ret);
		}
	}
	return;
}

/* Final cleanup of ospf instance */
static void
ospf_finish_final (struct ospf *ospf)
{
    int i,j;
    struct ospf_lsa *lsa = NULL;
    struct route_node *rn = NULL;	
	struct listnode *node = NULL;
    struct listnode *nnode = NULL;
    struct ospf_area *area = NULL;    
    struct ospf_interface *oi = NULL;
    struct ospf_vl_data *vl_data = NULL;
    struct ospf_nbr_nbma *nbr_nbma = NULL;
	struct u0_device_info_local *u0_device = NULL;
    
    ospf_opaque_type11_lsa_term (ospf);
    /* be nice if this worked, but it doesn't */
    /*ospf_flush_self_originated_lsas_now (ospf);*/
    /* Unregister redistribution */
    
    for (i = 0; i < ROUTE_PROTO_MAX; i++)
    {
        for(j = 0; j < OSPF_MAX_PROCESS_ID; j++)
        {
            ospf_redistribute_unset (ospf, i, j);
            vrf_bitmap_free(ospf->dmetric[i][j].redist);
            ospf->dmetric[i][j].redist = VRF_BITMAP_NULL;
        }
    }
    
    //ospf_redistribute_default_unset (ospf);
    //vrf_bitmap_free(ospf->default_information);
    ospf->default_information = VRF_BITMAP_NULL;
    
    for (ALL_LIST_ELEMENTS (ospf->areas, node, nnode, area))
    {
        ospf_remove_vls_through_area (ospf, area);
    }
    
    for (ALL_LIST_ELEMENTS (ospf->vlinks, node, nnode, vl_data))
    {
        ospf_vl_delete (ospf, vl_data);
		vl_data = NULL;
    }
    list_delete (ospf->vlinks);
	ospf->vlinks = NULL;

    ospf_route_policy_list_free(ospf);
    list_delete (ospf->redist_name);
	ospf->redist_name = NULL;
    
    /* Reset interface. */
    for (ALL_LIST_ELEMENTS (ospf->oiflist, node, nnode, oi))
    {
        ospf_if_free (oi);
    }
	oi = NULL;
    /* Clear static neighbors */
    for (rn = route_top (ospf->nbr_nbma); rn; rn = route_next (rn))
    {
        if ((nbr_nbma = rn->info))
        {
            OSPF_POLL_TIMER_OFF (nbr_nbma->t_poll);
            if (nbr_nbma->nbr)
            {
                nbr_nbma->nbr->nbr_nbma = NULL;
                nbr_nbma->nbr = NULL;
            }
            if (nbr_nbma->oi)
            {
                listnode_delete (nbr_nbma->oi->nbr_nbma, nbr_nbma);
                nbr_nbma->oi = NULL;
            }
            XFREE (MTYPE_OSPF_NEIGHBOR_STATIC, nbr_nbma);
			nbr_nbma = NULL;
        }
    }
    route_table_finish (ospf->nbr_nbma);
    ospf->nbr_nbma = NULL;
    /* Clear networks and Areas. */
    for (rn = route_top (ospf->networks); rn; rn = route_next (rn))
    {
        struct ospf_network *network;
        if ((network = rn->info) != NULL)
        {
            ospf_network_free (ospf, network);
            rn->info = NULL;
            route_unlock_node (rn);
        }
    }
    
    route_table_finish (ospf->networks);
    ospf->networks = NULL;
    
    for (ALL_LIST_ELEMENTS (ospf->areas, node, nnode, area))
    {
        listnode_delete (ospf->areas, area);
        ospf_area_free (area);
		area = NULL;
    }
    /* Cancel all timers. */
    OSPF_TIMER_OFF (ospf->t_external_lsa);
    OSPF_TIMER_OFF (ospf->t_spf_calc);
    OSPF_TIMER_OFF (ospf->t_ase_calc);
    OSPF_TIMER_OFF (ospf->t_maxage);
    OSPF_TIMER_OFF (ospf->t_maxage_walker);
    OSPF_TIMER_OFF (ospf->t_abr_task);
    OSPF_TIMER_OFF (ospf->t_asbr_check);
    OSPF_TIMER_OFF (ospf->t_distribute_update);
    OSPF_TIMER_OFF (ospf->t_lsa_refresher);
    //OSPF_TIMER_OFF (ospf->t_read);
    OSPF_TIMER_OFF_OLD (ospf->t_write);
    OSPF_TIMER_OFF (ospf->t_opaque_lsa_self);
    OSPF_TIMER_OFF (ospf->t_reset);
	OSPF_TIMER_OFF (ospf->t_external_fw);
	OSPF_TIMER_OFF (ospf->t_nssa_fw);
    LSDB_LOOP (OPAQUE_AS_LSDB (ospf), rn, lsa)
    {
        ospf_discard_from_db (ospf, ospf->lsdb, lsa);
    }
    LSDB_LOOP (EXTERNAL_LSDB (ospf), rn, lsa)
    {
        ospf_discard_from_db (ospf, ospf->lsdb, lsa);
    }
    ospf_lsdb_delete_all (ospf->lsdb);
    ospf_lsdb_free (ospf->lsdb);
    ospf->lsdb = NULL; 
    
    for (rn = route_top (ospf->maxage_lsa); rn; rn = route_next (rn))
    {
        if ((lsa = rn->info) != NULL)
        {
            ospf_lsa_unlock (&lsa);
            rn->info = NULL;
        }
    }
    route_table_finish (ospf->maxage_lsa);
    ospf->maxage_lsa = NULL;
    
    if (ospf->old_table)
    {
        ospf_route_table_free (ospf->old_table);
        ospf->old_table = NULL;
    }
    
    if (ospf->new_table)
    {
        ospf_route_delete (ospf->new_table,ospf);
        ospf_route_table_free (ospf->new_table);
        
        ospf->new_table = NULL;
    }
    
    if (ospf->old_rtrs)
    {
        ospf_rtrs_free (ospf->old_rtrs);
        ospf->old_rtrs = NULL;
    }
    
    if (ospf->new_rtrs)
    {
        ospf_rtrs_free (ospf->new_rtrs);
        ospf->new_rtrs = NULL;
    }
    
    if (ospf->new_external_route)
    {
        ospf_route_delete (ospf->new_external_route,ospf);
        ospf_route_table_free (ospf->new_external_route);
        ospf->new_external_route = NULL;
    }
    
    if (ospf->old_external_route)
    {
        ospf_route_delete (ospf->old_external_route,ospf);
        ospf_route_table_free (ospf->old_external_route);
        ospf->old_external_route = NULL;
    }
    if (ospf->external_lsas)
    {
        ospf_ase_external_lsas_finish (ospf->external_lsas);
        ospf->external_lsas = NULL;
    }
    
    ospf_area_range_table_free(ospf->summary);
    ospf->summary = NULL;
    
    list_delete (ospf->areas);
	ospf->areas = NULL;
    
    ospf_distance_reset (ospf);
    
    route_table_finish (ospf->distance_table);
    ospf->distance_table = NULL;
    
    ospf_delete (ospf);
    XFREE (MTYPE_OSPF_TOP, ospf);
    ospf = NULL;
	
    if (listcount (om->ospf) == 0)
    {
        for (i = ROUTE_PROTO_STATIC; i <= ROUTE_PROTO_MAX; i++)
        {
			for (j = 0; j < 128; j++)
			{
	            if (EXTERNAL_INFO (j, i) != NULL)
	            {
	                for (rn = route_top (EXTERNAL_INFO (j, i)); rn; rn = route_next (rn))
	                {
	                    if (rn->info == NULL)
	                    {
	                        continue;
	                    }
	                    XFREE (MTYPE_OSPF_EXTERNAL_INFO, rn->info);
	                    rn->info = NULL;
	                    route_unlock_node (rn);
	                }
	            }
			}
        }

		if(listcount (om->u0_list) > 0)
		{
			for (ALL_LIST_ELEMENTS(om->u0_list, node, nnode, u0_device))
			{
				listnode_delete (om->u0_list, u0_device);
		        XFREE (MTYPE_OSPF_TMP, u0_device);
				u0_device = NULL;
			}
		}
		
		if(listcount (om->u0_list) == 0)
		{
			om->u0_flag = !U0_DEVICE_EXIST;
			memset(om->loopback_index_pool, 0, sizeof(om->loopback_index_pool));
		}
    }
}


/* allocate new OSPF Area object */
static struct ospf_area *
ospf_area_new (struct ospf *ospf, struct in_addr area_id)
{
    struct ospf_area *new_area = NULL;

    /* Allocate new config_network. */
    do
  	{
  		new_area = XCALLOC (MTYPE_OSPF_AREA, sizeof (struct ospf_area));
  		if (new_area == NULL)
  		{
    		zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
  		}
  	}while(new_area == NULL);

	if (new_area == NULL)
    {
        zlog_err("Can't creat ospf_area : malloc failed");
        return (struct ospf_area *)NULL;
    }
	memset(new_area, 0, sizeof(struct ospf_area));

    new_area->ospf = ospf;

    new_area->area_id = area_id;

    new_area->external_routing = OSPF_AREA_DEFAULT;
    new_area->external_routing_config = OSPF_AREA_DEFAULT;
    new_area->default_cost = 1;
    new_area->auth_type = OSPF_AUTH_NULL;
	new_area->auth_type__config = AREA_AUTH_PARAM_CLEAN;
	new_area->auth_simple__config = AREA_AUTH_PARAM_CLEAN;
	new_area->auth_crypt = list_new ();
	new_area->auth_crypt__config = AREA_AUTH_PARAM_CLEAN;

    /* New LSDB init. */
    new_area->lsdb = ospf_lsdb_new ();

    /* Self-originated LSAs initialize. */
    new_area->router_lsa_self = NULL;

    ospf_opaque_type10_lsa_init (new_area);

    new_area->oiflist = list_new ();
    new_area->ranges = route_table_init ();

    if (area_id.s_addr == OSPF_AREA_BACKBONE)
    {
        ospf->backbone = new_area;
    }

    return new_area;
}

void
ospf_area_free (struct ospf_area *area)
{
    struct route_node *rn;
    struct ospf_lsa *lsa;
    /* Free LSDBs. */
    LSDB_LOOP (ROUTER_LSDB (area), rn, lsa)
    {
        ospf_discard_from_db (area->ospf, area->lsdb, lsa);
    }
    LSDB_LOOP (NETWORK_LSDB (area), rn, lsa)
    {
        ospf_discard_from_db (area->ospf, area->lsdb, lsa);
    }
    LSDB_LOOP (SUMMARY_LSDB (area), rn, lsa)
    {
        ospf_discard_from_db (area->ospf, area->lsdb, lsa);
    }
    LSDB_LOOP (ASBR_SUMMARY_LSDB (area), rn, lsa)
    {
        ospf_discard_from_db (area->ospf, area->lsdb, lsa);
    }
    LSDB_LOOP (NSSA_LSDB (area), rn, lsa)
    {
        ospf_discard_from_db (area->ospf, area->lsdb, lsa);
    }
    LSDB_LOOP (OPAQUE_AREA_LSDB (area), rn, lsa)
    {
        ospf_discard_from_db (area->ospf, area->lsdb, lsa);
    }
    LSDB_LOOP (OPAQUE_LINK_LSDB (area), rn, lsa)
    {
        ospf_discard_from_db (area->ospf, area->lsdb, lsa);
    }
    
    ospf_lsdb_delete_all (area->lsdb);
    ospf_lsdb_free (area->lsdb);
    area->lsdb = NULL;
    ospf_lsa_unlock (&area->router_lsa_self);
    
    ospf_area_range_table_free(area->ranges);
    area->ranges = NULL;
    
    list_delete (area->oiflist);
	area->oiflist = NULL;

	list_delete (area->auth_crypt);
	area->auth_crypt = NULL;
	
    if (EXPORT_NAME (area))
    {
        free (EXPORT_NAME (area));
    }
    if (IMPORT_NAME (area))
    {
        free (IMPORT_NAME (area));
    }
    /* Cancel timer. */
    OSPF_TIMER_OFF (area->t_stub_router);
    OSPF_TIMER_OFF (area->t_opaque_lsa_self);
    if (OSPF_IS_AREA_BACKBONE (area))
    {
        area->ospf->backbone = NULL;
    }
    
    ospf_name_unset(area);
    XFREE (MTYPE_OSPF_AREA, area);
}

void
ospf_area_check_free (struct ospf *ospf, struct in_addr area_id)
{
    struct ospf_area *area = NULL;
	
    area = ospf_area_lookup_by_area_id (ospf, area_id);
    if (area &&
            listcount (area->oiflist) == 0 &&
            area->ranges->top == NULL &&
            area->shortcut_configured == OSPF_SHORTCUT_DEFAULT &&
            area->external_routing == OSPF_AREA_DEFAULT &&
            area->no_summary == 0 &&
            area->default_cost == 1 &&
            EXPORT_NAME (area) == NULL &&
            IMPORT_NAME (area) == NULL &&
            area->auth_type == OSPF_AUTH_NULL)
    {
        listnode_delete (ospf->areas, area);
        ospf_area_free (area);
		area = NULL;
    }
}

struct ospf_area *
ospf_area_get (struct ospf *ospf, struct in_addr area_id, int format)
{
    struct ospf_area *area = NULL;

    area = ospf_area_lookup_by_area_id (ospf, area_id);
    if (!area)
    {
        area = ospf_area_new (ospf, area_id);
        area->format = format;
        listnode_add_sort (ospf->areas, area);
        ospf_check_abr_status (ospf);
        if (ospf->stub_router_admin_set == OSPF_STUB_ROUTER_ADMINISTRATIVE_SET)
        {
            SET_FLAG (area->stub_router_state, OSPF_AREA_ADMIN_STUB_ROUTED);
        }
    }

    return area;
}

struct ospf_area *
ospf_area_lookup_by_area_id (struct ospf *ospf, struct in_addr area_id)
{
    struct listnode *node = NULL;
    struct ospf_area *area = NULL;

    for (ALL_LIST_ELEMENTS_RO (ospf->areas, node, area))
    {
        if (IPV4_ADDR_SAME (&area->area_id, &area_id))
        {
            return area;
        }
    }

    return NULL;
}

void
ospf_area_add_if (struct ospf_area *area, struct ospf_interface *oi)
{
    listnode_add (area->oiflist, oi);
}

void
ospf_area_del_if (struct ospf_area *area, struct ospf_interface *oi)
{
    listnode_delete (area->oiflist, oi);
}


/* Config network statement related functions. */
static struct ospf_network *
ospf_network_new (struct in_addr area_id, int format)
{
    struct ospf_network *new_network = NULL;
	do
  	{
  		new_network = XCALLOC (MTYPE_OSPF_NETWORK, sizeof (struct ospf_network));
  		if (new_network == NULL)
  		{
    		zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
  		}
  	}while(new_network == NULL);
	if (new_network == NULL)
    {
        zlog_err("Can't creat ospf_network : malloc failed");
        return (struct ospf_network *)NULL;
    }
	memset(new_network, 0, sizeof(struct ospf_network));

    new_network->area_id = area_id;
    new_network->format = format;

    return new_network;
}

static void
add_ospf_interface (struct connected *co, struct ospf_area *area)
{
    struct ospf_interface *oi = NULL;
	
    oi = ospf_if_new (area->ospf, co->ifp, co->address);
    oi->connected = co;
    oi->area = area;
    oi->params = ospf_lookup_if_params (co->ifp, oi->address->u.prefix4);
    if(oi->ifp->ll_type == IFNET_TYPE_LOOPBACK)
    {
        oi->output_cost = 0;
    }
    else
    {
        oi->output_cost = ospf_if_get_output_cost (oi);
    }

    ///* Add pseudo neighbor. */
    //ospf_nbr_add_self (oi);
    /* Relate ospf interface to ospf instance. */
    oi->ospf = area->ospf;
    /* update network type as interface flag */
    /* If network type is specified previously,
       skip network type setting. */
    oi->type = IF_DEF_PARAMS(co->ifp)->type;

    /* Add pseudo neighbor. */
    ospf_nbr_self_reset(oi);

    ospf_area_add_if(oi->area, oi);

    /* if router_id is not configured, dont bring up
     * interfaces.
     * ospf_router_id_update() will call ospf_if_update
     * whenever r-id is configured instead.
     */
    /*if ((area->ospf->router_id.s_addr != 0)
            && if_is_operative (co->ifp))*/
	/* 解决菲律宾VX设备的XG盘出现掉电重启时由于事件发送顺序不正常导致4094子接口状态
	机不翻转的问题，暂时去掉4094子接口的状态检查。但是这样会引入一个问题：所有添加进区域的4094子接口
	状态机状态在ip添加事件发生时不会是down状态了 */	
    if ((area->ospf->router_id.s_addr != 0)
		&& (if_is_operative (co->ifp) || (DCN_VLAN_ID == IFM_SUBPORT_ID_GET(co->ifp->ifindex))))
    {
        ospf_if_up (oi);
    }
}

static void
update_redistributed (struct ospf *ospf, int add_to_ospf)
{
    struct route_node *rn = NULL;
    struct external_info *ei = NULL;
	
    if (ospf_is_type_redistributed (ospf,ROUTE_METRIC_CONNECT,0))
    {
        if (EXTERNAL_INFO (ospf->vpn, ROUTE_METRIC_CONNECT))
        {
            for (rn = route_top (EXTERNAL_INFO (ospf->vpn, ROUTE_METRIC_CONNECT));
                    rn; rn = route_next (rn))
            {
                if ((ei = rn->info) != NULL)
                {
                    if (add_to_ospf)
                    {
                        if (ospf_external_info_find_lsa (ospf, &ei->p))
                        {
                            if (!ospf_distribute_check_connected (ospf, ei))
                            {
                                ospf_external_lsa_flush (ospf, ei->type, &ei->p,
                                                         ei->ifindex /*, ei->nexthop */);
                            }
                        }
                    }
                    else
                    {
                        if (!ospf_external_info_find_lsa (ospf, &ei->p))
                        {
                            if (ospf_distribute_check_connected (ospf, ei))
                            {
                                ospf_external_lsa_originate (ospf, ei);
                            }
                        }
                    }
                }
            }
        }
    }
}

static void
ospf_network_free (struct ospf *ospf, struct ospf_network *network)
{
    //ospf_area_check_free (ospf, network->area_id);
    ospf_schedule_abr_task (ospf);
    XFREE (MTYPE_OSPF_NETWORK, network);
}

int
ospf_network_set (struct ospf *ospf, struct prefix_ipv4 *p,
                  struct in_addr area_id)
{
    //struct ospf_network *network = NULL;
    int ret = OSPF_AREA_ID_FORMAT_ADDRESS;
    struct ospf *top = NULL;
    struct route_node *rn = NULL;
    struct listnode *node = NULL;
    struct ospf_area *area = NULL;
    
    for (ALL_LIST_ELEMENTS_RO (om->ospf, node, top))
    {
        if(top == ospf)
        {
            continue;
        }
        rn = route_node_get (top->networks, (struct prefix *)p);
        if (rn->info)
        {
            /* There is already same network statement. */
            route_unlock_node (rn);
            return ERRNO_FAIL;
        }
    }
    
    rn = route_node_get (ospf->networks, (struct prefix *)p);
    if (rn->info)
    {
        /* There is already same network statement. */
        route_unlock_node (rn);
        return ERRNO_EXISTED;
    }
    
    rn->info = ospf_network_new (area_id, ret);
    area = ospf_area_get (ospf, area_id, ret);
    
    /* Run network config now. */
    ospf_network_run ((struct prefix *)p, area);
    
    /* Update connected redistribute. */
    update_redistributed(ospf, 1);
    ospf_external_lsa_fw_update(ospf);
    return ERRNO_SUCCESS;
}

int
ospf_network_unset (struct ospf *ospf, struct prefix_ipv4 *p,
                    struct in_addr area_id)
{
    int num = 0;
    struct ospf_area *area = NULL;
    struct route_node *rn = NULL;
    struct route_node *rn_temp = NULL;
    struct listnode *node = NULL;
    struct listnode *nnode = NULL;
    struct ospf_interface *oi = NULL;
	struct ospf_network *network = NULL;
    
    rn = route_node_lookup (ospf->networks, (struct prefix *)p);
    if (rn == NULL)
    {
        return 0;
    }
    
    network = rn->info;
    route_unlock_node (rn);
    if (!IPV4_ADDR_SAME (&area_id, &network->area_id))
    {
        return 0;
    }
    ospf_network_free (ospf, rn->info);
    rn->info = NULL;
    route_unlock_node (rn);   /* initial reference */
    area = ospf_area_lookup_by_area_id(ospf, area_id);
    if(area == NULL)
    {
        return 0;
    }
    
    /* Find interfaces that not configured already.  */
    for (ALL_LIST_ELEMENTS (area->oiflist, node, nnode, oi))
    {
        if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
        {
            continue;
        }
        ospf_network_run_subnet (ospf, oi->connected, NULL, NULL);
    }
    
    /* in order to delete slave ip  */
    for (ALL_LIST_ELEMENTS (area->oiflist, node, nnode, oi))
    {
        if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
        {
            continue;
        }

        if (CHECK_FLAG(oi->connected->flags, ZEBRA_IFA_SLAVE))
        {
            num = 0;
            for (rn_temp = route_top (IF_OIFS (oi->ifp)); rn_temp; rn_temp = route_next (rn))
            {
                struct ospf_interface *oi_temp = NULL;
                if ( (oi_temp = rn_temp->info) == NULL)
                {
                    continue;
                }
                if(oi_temp != oi)
                {
                    num ++;
                }
            }
            if(num)
            {
                ospf_sub_interface_free(oi->ifp);
                break;
            }
        }
        else
        {
            continue;
        }
    }
    /* Update connected redistribute. */
    update_redistributed(ospf, 0);
    ospf_external_lsa_fw_update(ospf);
    return 1;
}

/* Ensure there's an OSPF instance, as "ip ospf area" enabled OSPF means
 * there might not be any 'router ospf' config.
 *
 * Otherwise, doesn't do anything different to ospf_if_update for now
 */
void
ospf_interface_area_set (struct interface *ifp)
{
    //struct ospf *ospf = ospf_get();/*ospf_get*/
    //ospf_if_update (ospf, ifp);
    /* if_update does a update_redistributed */
    return;
}

void
ospf_interface_area_unset (struct interface *ifp)
{
    return;
}


/* Check whether interface matches given network
 * returns: 1, true. 0, false
 */
static int
ospf_network_match_iface(const struct connected *co, const struct prefix *net)
{
    /* new approach: more elegant and conceptually clean */
    return prefix_match(net, CONNECTED_PREFIX(co));
}

static void
ospf_update_interface_area (struct connected *co, struct ospf_area *area)
{
    struct ospf_interface *oi = ospf_if_table_lookup (co->ifp, co->address);
	zlog_debug(OSPF_DBG_OTHER, "%s[%d]:oi = %s\n", __FUNCTION__, __LINE__, oi == NULL ? "NULL" : IF_NAME(oi));
    /* nothing to be done case */
    if (oi && oi->area == area)
    {
        return;
    }
    if (oi)
    {
        ospf_if_free (oi);
    }
	oi = NULL;
    add_ospf_interface (co, area);
}

/* Run OSPF for the given subnet, taking into account the following
 * possible sources of area configuration, in the given order of preference:
 *
 * - Whether there is interface+address specific area configuration
 * - Whether there is a default area for the interface
 * - Whether there is an area given as a parameter.
 * - If no specific network prefix/area is supplied, whether there's
 *   a matching network configured.
 */
static void
ospf_network_run_subnet (struct ospf *ospf, struct connected *co,
                         struct prefix *p, struct ospf_area *given_area)
{
	zlog_debug(OSPF_DBG_OTHER, "%s[%d]:start!!! co->address = %s\n", __FUNCTION__, __LINE__, inet_ntoa(co->address->u.prefix4));
	int configed = 0;
    struct route_node *rn = NULL;
    struct ospf_area *area = NULL;
    struct ospf_interface *oi = NULL;
    struct ospf_if_params *params = NULL;
	
    if (co->address->family != AF_INET)
    {
        return;
    }
    /* Try determine the appropriate area for this interface + address
     * Start by checking interface config
     */
    if (!(params = ospf_lookup_if_params (co->ifp, co->address->u.prefix4)))
    {
        params = IF_DEF_PARAMS (co->ifp);
    }
    if (OSPF_IF_PARAM_CONFIGURED(params, if_area))
    {
        area = (ospf_area_get (ospf, params->if_area,
                               OSPF_AREA_ID_FORMAT_ADDRESS));
    }
    /* If we've found an interface and/or addr specific area, then we're
     * done
     */
    if (area)
    {
        ospf_update_interface_area (co, area);
        return;
    }
    /* Otherwise, only remaining possibility is a matching network statement */
    if (!CHECK_FLAG(co->flags, ZEBRA_IFA_SLAVE))
    {
		zlog_debug(OSPF_DBG_OTHER, "%s[%d]: master ip = %s\n", __FUNCTION__, __LINE__, inet_ntoa(co->address->u.prefix4));
		if (p)
        {
            if(given_area == NULL)
		    {
		        zlog_warn("%s[%d]  given_area is NULL",__func__,__LINE__);
		        return ;
		    }
            assert (given_area != NULL);
            /* Which either was supplied as a parameter.. (e.g. cause a new
             * network/area was just added)..
             */
            if (p->family == co->address->family
                    && ospf_network_match_iface (co, p))
            {
                ospf_update_interface_area (co, given_area);
            }
            return;
        }
    }
    else
    {
        if(ospf_sub_interface_look_master(co->ifp) == 0)
        {
        	zlog_debug(OSPF_DBG_OTHER, "%s[%d]: return!! SLAVE ip = %s\n", __FUNCTION__, __LINE__, inet_ntoa(co->address->u.prefix4));
            return;
        }
    }
    /* Else we have to search the existing network/area config to see
     * if any match..
     */
    for (rn = route_top (ospf->networks); rn; rn = route_next (rn))
    {
        if (rn->info != NULL
                && ospf_network_match_iface (co, &rn->p))
        {
            struct ospf_network *network = (struct ospf_network *) rn->info;
            area = ospf_area_get (ospf, network->area_id, network->format);
            ospf_update_interface_area (co, area);
            configed = 1;
        }
    }
    /* If the subnet isn't in any area, deconfigure */
    if (!configed && (oi = ospf_if_table_lookup (co->ifp, co->address)))
    {
        ospf_if_free (oi);
		oi = NULL;
    }
}

static void
ospf_network_run_interface (struct ospf *ospf, struct interface *ifp,
                            struct prefix *p,
                            struct ospf_area *given_area)
{
    struct connected *co = NULL;
    struct listnode *cnode = NULL;
    
    if (memcmp (ifp->name, "VLINK", 5) == 0)
    {
        return;
    }
    
    /* Network prefix without area is nonsensical */
    if (p)
    {
        if(given_area == NULL)
	    {
	        zlog_warn("%s[%d]  given_area is NULL",__func__,__LINE__);
	        return ;
	    }
        assert (given_area != NULL);
    }
    /* vpn instance must be same */
    if(ospf->vpn != ifp->vpn)
    {
        return;
    }
    /* if interface prefix is match specified prefix,
       then create socket and join multicast group. */
    for (ALL_LIST_ELEMENTS_RO (ifp->connected, cnode, co))
    {
        ospf_network_run_subnet (ospf, co, p, given_area);
    }
}

static void
ospf_network_run (struct prefix *p, struct ospf_area *area)
{
    struct interface *ifp = NULL;
    struct listnode *node = NULL;
	
    /* Schedule Router ID Update. */
    if (area->ospf->router_id.s_addr == 0)
    {
        ospf_router_id_update (area->ospf);
    }
    /* Get target interface. */
    for (ALL_LIST_ELEMENTS_RO (om->iflist, node, ifp))
    {
        ospf_network_run_interface (area->ospf, ifp, p, area);
    }
}

void
ospf_ls_upd_queue_empty (struct ospf_interface *oi)
{
    struct list *lst = NULL;
    struct ospf_lsa *lsa = NULL;
    struct route_node *rn = NULL;
    struct listnode *node = NULL;
	struct listnode *nnode = NULL;
	
    /* empty ls update queue */
    for (rn = route_top (oi->ls_upd_queue); rn;
            rn = route_next (rn))
    {
        if ((lst = (struct list *) rn->info))
        {
            for (ALL_LIST_ELEMENTS (lst, node, nnode, lsa))
            {
                ospf_lsa_unlock (&lsa);    /* oi->ls_upd_queue */
            }
			
			/*SHUT DOWN LIST BIG*/
			list_delete(lst);
			lst = NULL;
            //list_free (lst);
            rn->info = NULL;
        }
    }
    /* remove update event */
    if (oi->t_ls_upd_event)
    {
        thread_cancel (oi->t_ls_upd_event);
        oi->t_ls_upd_event = NULL;
    }
}


static struct ospf *
ospf_ifp_connected (struct ospf *ospf, struct interface *ifp)
{
    struct connected *co = NULL;
    struct route_node *rn = NULL;
    struct listnode *cnode = NULL;

    for (ALL_LIST_ELEMENTS_RO (ifp->connected, cnode, co))
    {
        if (CHECK_FLAG(co->flags, ZEBRA_IFA_SLAVE))
        {
        	if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
        	{
        		zlog_debug(OSPF_DBG_ZEBRA_INTERFACE, "%s[%d]:if is slave: ip = %s ifindex = 0x%x name = %s\n", \
					__FUNCTION__, __LINE__, inet_ntoa(co->address->u.prefix4), ifp->ifindex, ifp->name);
        	}
            continue;//bug#58382
        }
        for (rn = route_top (ospf->networks); rn; rn = route_next (rn))
        {
            if (rn->info != NULL && ospf_network_match_iface (co, &rn->p))
            {
                return ospf;
            }
        }
    }

    return NULL;
}

void
ospf_if_update (struct ospf *ospf, struct interface *ifp)
{
    struct ospf *top = NULL;
    struct listnode *node = NULL;

    if (listcount (om->ospf) == 0)
    {
        return ;
    }
    if (!ospf)
    {
        for (ALL_LIST_ELEMENTS_RO (om->ospf, node, top))
        {
            if ((ospf = ospf_ifp_connected(top, ifp)) != NULL)
            {
                break;
            }
        }
    }
    if (ospf == NULL)
    {
		if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
		{
			zlog_debug(OSPF_DBG_ZEBRA_INTERFACE, "%s[%d]:ospf == NULL, return!\n", __FUNCTION__, __LINE__);
		}
        //ospf = ospf_get(ospf_id);
        return;
    }
    /* OSPF must be ready. */
    if (!ospf_is_ready (ospf))
    {
        return;
    }

    ospf_network_run_interface (ospf, ifp, NULL, NULL);
    /* Update connected redistribute. */
    update_redistributed(ospf, 1);
    /* add by tai , change fw */
    ospf_external_lsa_fw_update(ospf);
}


void
ospf_remove_vls_through_area (struct ospf *ospf, struct ospf_area *area)
{
    struct listnode *node = NULL;
	struct listnode *nnode = NULL;
    struct ospf_vl_data *vl_data = NULL;
	
    for (ALL_LIST_ELEMENTS (ospf->vlinks, node, nnode, vl_data))
    {
        if (IPV4_ADDR_SAME (&vl_data->vl_area_id, &area->area_id))
        {
            ospf_vl_delete (ospf, vl_data);
			vl_data = NULL;
        }
    }
}


static const struct message ospf_area_type_msg[] =
{
    { OSPF_AREA_DEFAULT,  "Default" },
    { OSPF_AREA_STUB,     "Stub" },
    { OSPF_AREA_NSSA,     "NSSA" },
};
static const int ospf_area_type_msg_max = OSPF_AREA_TYPE_MAX;

void
ospf_area_type_set (struct ospf_area *area, int type)
{
    struct listnode *node = NULL;
	struct listnode *nnode = NULL;
    struct ospf_interface *oi = NULL;
	
    if(listcount (area->oiflist) > 0)
    {
        for (ALL_LIST_ELEMENTS (area->ospf->oiflist, node, nnode, oi))
        {
            if (oi->area->area_id.s_addr == area->area_id.s_addr)
            {
                if (oi->state > ISM_Down)
                {
                    ospf_if_down(oi);
                }
            }
        }
    }
    area->external_routing = type;
    if(listcount (area->oiflist) > 0)
    {
        for (ALL_LIST_ELEMENTS (area->ospf->oiflist, node, nnode, oi))
        {
            if (oi->area->area_id.s_addr == area->area_id.s_addr)
            {
                if (ospf_if_is_up(oi))
                {
                    ospf_if_up(oi);
                }
            }
        }
    }
    if (IS_DEBUG_OSPF_EVENT)
        zlog_debug (OSPF_DBG_EVENT, "Area[%s]: Configured as %s", inet_ntoa (area->area_id),
                    LOOKUP (ospf_area_type_msg, type));
    switch (area->external_routing)
    {
    case OSPF_AREA_DEFAULT:
        for (ALL_LIST_ELEMENTS_RO (area->oiflist, node, oi))
        {
            if (oi->nbr_self != NULL)
            {
                UNSET_FLAG (oi->nbr_self->options, OSPF_OPTION_NP);
                SET_FLAG (oi->nbr_self->options, OSPF_OPTION_E);
            }
        }
        break;
    case OSPF_AREA_STUB:
        for (ALL_LIST_ELEMENTS_RO (area->oiflist, node, oi))
        {
            if (oi->nbr_self != NULL)
            {
                if (IS_DEBUG_OSPF_EVENT)
                {
                    zlog_debug (OSPF_DBG_EVENT, "setting options on %s accordingly", IF_NAME (oi));
                }
                UNSET_FLAG (oi->nbr_self->options, OSPF_OPTION_NP);
                UNSET_FLAG (oi->nbr_self->options, OSPF_OPTION_E);
                if (IS_DEBUG_OSPF_EVENT)
                    zlog_debug (OSPF_DBG_EVENT, "options set on %s: %x",
                                IF_NAME (oi), OPTIONS (oi));
            }
        }
        break;
    case OSPF_AREA_NSSA:
        for (ALL_LIST_ELEMENTS_RO (area->oiflist, node, oi))
        {
            if (oi->nbr_self != NULL)
            {
                zlog_debug (OSPF_DBG_EVENT, "setting nssa options on %s accordingly", IF_NAME (oi));
                UNSET_FLAG (oi->nbr_self->options, OSPF_OPTION_E);
                SET_FLAG (oi->nbr_self->options, OSPF_OPTION_NP);
                zlog_debug (OSPF_DBG_EVENT, "options set on %s: %x", IF_NAME (oi), OPTIONS (oi));
            }
        }
        break;
    default:
        break;
    }
    ospf_schedule_abr_task (area->ospf);
    ospf_schedule_asbr_task (area->ospf);
}

int
ospf_area_shortcut_set (struct ospf *ospf, struct ospf_area *area, int mode)
{
    if (area->shortcut_configured == mode)
    {
        return 0;
    }
    area->shortcut_configured = mode;
    ospf_router_lsa_update_area (area);
    ospf_schedule_abr_task (ospf);
    ospf_area_check_free (ospf, area->area_id);
    return 1;
}

int
ospf_area_shortcut_unset (struct ospf *ospf, struct ospf_area *area)
{
    area->shortcut_configured = OSPF_SHORTCUT_DEFAULT;
    ospf_router_lsa_update_area (area);
    ospf_area_check_free (ospf, area->area_id);
    ospf_schedule_abr_task (ospf);
    return 1;
}

static int
ospf_area_vlink_count (struct ospf *ospf, struct ospf_area *area)
{
    int count = 0;
    struct ospf_vl_data *vl = NULL;
    struct listnode *node = NULL;
	
    for (ALL_LIST_ELEMENTS_RO (ospf->vlinks, node, vl))
    {
        if (IPV4_ADDR_SAME (&vl->vl_area_id, &area->area_id))
        {
            count++;
        }
    }
    return count;
}

int
ospf_area_stub_set (struct ospf *ospf, struct in_addr area_id, int area_summary)
{
    int format = OSPF_AREA_ID_FORMAT_ADDRESS;
    struct ospf_area *area = NULL;
	
    area = ospf_area_get (ospf, area_id, format);
    if (OSPF_IS_AREA_ID_BACKBONE (area_id))
    {
        return 4;
    }
    if (ospf_area_vlink_count (ospf, area))
    {
        return 0;
    }
    if (IS_AREA_NSSA (area))
    {
        return 2;
    }
    if(IS_AREA_STUB (area) && (area->no_summary != area_summary))
    {
        area->external_routing_config = OSPF_AREA_STUB;
        ospf_area_type_set (area, area->external_routing_config);
    }
    else if(IS_AREA_STUB (area))
    {
        if (IS_DEBUG_OSPF_EVENT)
            zlog_debug (OSPF_DBG_EVENT, "Area[%s]: Types are the same, ignored.",
                        inet_ntoa (area->area_id));
        return 3;
    }
    else if (IS_AREA_DEFAULT (area))
    {
        ospf->anySTUB++;
        area->external_routing_config = OSPF_AREA_STUB;
        ospf_asbr_status_update (ospf, ospf->redistribute);
        if(listcount (area->oiflist) > 0)
        {
            ospf_area_self_lsdb_discard(area);
        }
        ospf_area_type_set (area, area->external_routing_config);
    }
    return 1;
}

int
ospf_area_stub_unset (struct ospf *ospf, struct in_addr area_id)
{
    struct ospf_area *area = NULL;
	
    area = ospf_area_lookup_by_area_id (ospf, area_id);
    if (area == NULL)
    {
        return 1;
    }
    if (IS_AREA_STUB(area))
    {
        ospf->anySTUB--;
        area->external_routing_config = OSPF_AREA_DEFAULT;
        ospf_asbr_status_update (ospf, ospf->redistribute);
        ospf_area_type_set (area, area->external_routing_config);
    }
    //ospf_area_check_free (ospf, area_id);
    return 1;
}

int
ospf_area_no_summary_set (struct ospf *ospf, struct in_addr area_id)
{
    int format = OSPF_AREA_ID_FORMAT_ADDRESS;
    struct ospf_area *area = NULL;
	
    area = ospf_area_get (ospf, area_id, format);
    area->no_summary = 1;
    return 1;
}

int
ospf_area_no_summary_unset (struct ospf *ospf, struct in_addr area_id)
{
    struct ospf_area *area = NULL;
	
    area = ospf_area_lookup_by_area_id (ospf, area_id);
    if (area == NULL)
    {
        return 0;
    }
    area->no_summary = 0;
    //ospf_area_check_free (ospf, area_id);
    return 1;
}

int
ospf_area_nssa_set (struct ospf *ospf, struct in_addr area_id,int area_summary)
{
    int format = OSPF_AREA_ID_FORMAT_ADDRESS;
    struct ospf_area *area = NULL;
	
    area = ospf_area_get (ospf, area_id, format);
    if (ospf_area_vlink_count (ospf, area))
    {
        return 0;
    }
    if (OSPF_IS_AREA_ID_BACKBONE (area_id))
    {
        return 4;
    }
    if (IS_AREA_STUB (area))
    {
        return 3;
    }
    if(IS_AREA_NSSA (area) && (area->no_summary != area_summary))
    {
        area->external_routing_config = OSPF_AREA_NSSA;
        ospf_area_type_set (area, area->external_routing_config);
    }
    else if(IS_AREA_NSSA (area))
    {
        if (IS_DEBUG_OSPF_EVENT)
            zlog_debug (OSPF_DBG_EVENT, "Area[%s]: Types are the same, ignored.",
                        inet_ntoa (area->area_id));
        return 2;
    }
    else if (IS_AREA_DEFAULT (area))
    {
        ospf->anyNSSA++;
        area->external_routing_config = OSPF_AREA_NSSA;
        /* RFC3101, 3.1:
          * All NSSA border routers must set the E-bit in the Type-1 router-LSAs
          * of their directly attached non-stub areas, even when they are not
          * translating.
          */
        if(IS_OSPF_ABR (ospf))
        {
            ospf_asbr_status_update (ospf, ++ospf->redistribute);
        }
        zlog_debug(OSPF_DBG_EVENT, "%s [%d] ospf->redis %d",__func__,__LINE__,ospf->redistribute);
        ospf_reset_area(area);
    }
    /* set NSSA area defaults */
    area->no_summary = area_summary;
    area->NSSATranslatorRole = OSPF_NSSA_ROLE_CANDIDATE;
    area->NSSATranslatorState = OSPF_NSSA_TRANSLATE_DISABLED;
    area->NSSATranslatorStabilityInterval = OSPF_NSSA_TRANS_STABLE_DEFAULT;
    return 1;
}

int
ospf_area_nssa_unset (struct ospf *ospf, struct in_addr area_id)
{
    struct ospf_area *area = NULL;
	
    area = ospf_area_lookup_by_area_id (ospf, area_id);
    if (area == NULL)
    {
        return 0;
    }
    if (IS_AREA_NSSA(area))
    {
        ospf->anyNSSA--;
        area->external_routing_config = OSPF_AREA_DEFAULT;
        ospf_area_as_nssa_lsa_flush(area);
        /* RFC3101, 3.1:
            * All NSSA border routers must set the E-bit in the Type-1 router-LSAs
            * of their directly attached non-stub areas, even when they are not
            * translating.
            */
        if(IS_OSPF_ABR (area->ospf))
        {
            ospf_asbr_status_update (ospf, --ospf->redistribute);
        }
        zlog_debug(OSPF_DBG_NSSA, "%s [%d] ospf->redis %d",__func__,__LINE__,ospf->redistribute);
        ospf_reset_area(area);
    }
    return 1;
}

int
ospf_area_nssa_translator_role_set (struct ospf *ospf, struct in_addr area_id,
                                    int role)
{
    struct ospf_area *area = NULL;
	
    area = ospf_area_lookup_by_area_id (ospf, area_id);
    if (area == NULL)
    {
        return 0;
    }
    area->NSSATranslatorRole = role;
    return 1;
}


int
ospf_area_export_list_set (struct ospf *ospf,
                           struct ospf_area *area, const char *list_name)
{
    struct access_list *list = NULL;
	
    list = access_list_lookup (AFI_IP, list_name);
    EXPORT_LIST (area) = list;
    if (EXPORT_NAME (area))
    {
        free (EXPORT_NAME (area));
    }
    EXPORT_NAME (area) = strdup (list_name);
    ospf_schedule_abr_task (ospf);
    return 1;
}

int
ospf_area_export_list_unset (struct ospf *ospf, struct ospf_area * area)
{
    EXPORT_LIST (area) = 0;
    if (EXPORT_NAME (area))
    {
        free (EXPORT_NAME (area));
    }
    EXPORT_NAME (area) = NULL;
    ospf_area_check_free (ospf, area->area_id);
    ospf_schedule_abr_task (ospf);
    return 1;
}

int
ospf_area_import_list_set (struct ospf *ospf, struct ospf_area *area,
                           const char *name)
{
    struct access_list *list = NULL;
	
    list = access_list_lookup (AFI_IP, name);
    IMPORT_LIST (area) = list;
    if (IMPORT_NAME (area))
    {
        free (IMPORT_NAME (area));
    }
    IMPORT_NAME (area) = strdup (name);
    ospf_schedule_abr_task (ospf);
    return 1;
}

int
ospf_area_import_list_unset (struct ospf *ospf, struct ospf_area * area)
{
    IMPORT_LIST (area) = 0;
    if (IMPORT_NAME (area))
    {
        free (IMPORT_NAME (area));
    }
    IMPORT_NAME (area) = NULL;
    ospf_area_check_free (ospf, area->area_id);
    ospf_schedule_abr_task (ospf);
    return 1;
}


int
ospf_refresh_interval_set (struct ospf *ospf, int interval)
{
    int refresh_index;
    struct ospf_lsa *lsa = NULL;    
	struct listnode *node = NULL;
	struct listnode *nnode = NULL;
    struct list *refresh_list = NULL;
    struct list *lsa_to_refresh = list_new ();    
    struct list *lsa_to_refresh2 = list_new ();
    
    if (ospf->refresh_interval == interval)
    {
        return 1;
    }
    ospf->refresh_interval = interval;
    
    OSPF_TIMER_OFF (ospf->t_lsa_refresher);
    ospf->lsa_refresh_queue.index = 0;
    ospf->lsa_refresher_started = time_get_real_time (NULL);
    /*ospf->t_lsa_refresher = thread_add_timer (master_ospf, ospf_lsa_refresh_walker,
                           ospf, 0);*/
	/*ospf->t_lsa_refresher = high_pre_timer_add ((char *)"ospf_lsa_fresh_timer", LIB_TIMER_TYPE_NOLOOP,\
							ospf_lsa_refresh_walker, ospf, 10);*/

	ospf_lsa_refresh_walker(ospf);
    
    for(refresh_index = 0; refresh_index < OSPF_LSA_REFRESHER_SLOTS; refresh_index ++)
    {
        refresh_list = ospf->lsa_refresh_queue.qs [refresh_index];
        if (refresh_list)
        {
            for (ALL_LIST_ELEMENTS (refresh_list, node, nnode, lsa))
            {
                if(LS_AGE(lsa) < (int)ospf->refresh_interval)
                {
                    assert (lsa->lock > 0);
                    
                    lsa->refresh_list = -1;
                    listnode_add (lsa_to_refresh, lsa);
                    
                    ospf_refresher_unregister_lsa(ospf,lsa);
                    
                    list_delete_node (refresh_list, node);
                }
                else
                {
                    list_delete_node (refresh_list, node);
                    lsa->refresh_list = -1;
                    listnode_add (lsa_to_refresh2, lsa);
                }

            }
        }
        
    }

    for (ALL_LIST_ELEMENTS (lsa_to_refresh, node, nnode, lsa))
    {
        ospf_refresher_register_lsa(ospf,lsa);
    }
    list_delete (lsa_to_refresh);
	lsa_to_refresh = NULL;

    for (ALL_LIST_ELEMENTS (lsa_to_refresh2, node, nnode, lsa))
    {
        ospf_lsa_refresh(ospf,lsa);

    }
    list_delete (lsa_to_refresh2);
	lsa_to_refresh2 = NULL;
    
    return 1;
}


int
ospf_timers_refresh_set (struct ospf *ospf, int interval)
{
    int time_left;
	
    if (ospf->lsa_refresh_interval == interval)
    {
        return 1;
    }
    time_left = ospf->lsa_refresh_interval -
                (time_get_real_time (NULL) - ospf->lsa_refresher_started);
    if (time_left > interval)
    {
        OSPF_TIMER_OFF (ospf->t_lsa_refresher);
        /*ospf->t_lsa_refresher =
            thread_add_timer (master_ospf, ospf_lsa_refresh_walker, ospf, interval);*/
		ospf->t_lsa_refresher =
            high_pre_timer_add ((char *)"ospf_lsa_fresh_timer", LIB_TIMER_TYPE_NOLOOP,\
            				ospf_lsa_refresh_walker, ospf, (interval)*1000);
    }
    ospf->lsa_refresh_interval = interval;
    return 1;
}

int
ospf_timers_refresh_unset (struct ospf *ospf)
{
    int time_left;
	
    time_left = ospf->lsa_refresh_interval -
                (time_get_real_time (NULL) - ospf->lsa_refresher_started);
    if (time_left > OSPF_LSA_REFRESH_INTERVAL_DEFAULT)
    {
        OSPF_TIMER_OFF (ospf->t_lsa_refresher);
        /*ospf->t_lsa_refresher =
            thread_add_timer (master_ospf, ospf_lsa_refresh_walker, ospf,
                              OSPF_LSA_REFRESH_INTERVAL_DEFAULT);*/
		ospf->t_lsa_refresher =
            high_pre_timer_add ((char *)"ospf_lsa_fresh_timer", LIB_TIMER_TYPE_NOLOOP,\
            					ospf_lsa_refresh_walker, ospf, (OSPF_LSA_REFRESH_INTERVAL_DEFAULT)*1000);
    }
    ospf->lsa_refresh_interval = OSPF_LSA_REFRESH_INTERVAL_DEFAULT;
    return 1;
}


static struct ospf_nbr_nbma *
ospf_nbr_nbma_new (void)
{
    struct ospf_nbr_nbma *nbr_nbma = NULL;

	do
  	{
  		nbr_nbma = XCALLOC (MTYPE_OSPF_NEIGHBOR_STATIC,
                        sizeof (struct ospf_nbr_nbma));
  		if (nbr_nbma == NULL)
  		{
    		zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
  		}
  	}while(nbr_nbma == NULL);
	
	if (nbr_nbma == NULL)
    {
        zlog_err("Can't creat nbr_nbma : malloc failed");
        return (struct ospf_nbr_nbma *)NULL;
    }
	memset(nbr_nbma, 0, sizeof(struct ospf_nbr_nbma));

    nbr_nbma->priority = OSPF_NEIGHBOR_PRIORITY_DEFAULT;
    nbr_nbma->v_poll = OSPF_POLL_INTERVAL_DEFAULT;

    return nbr_nbma;
}

static void
ospf_nbr_nbma_free (struct ospf_nbr_nbma *nbr_nbma)
{
    XFREE (MTYPE_OSPF_NEIGHBOR_STATIC, nbr_nbma);
}

static void
ospf_nbr_nbma_delete (struct ospf *ospf, struct ospf_nbr_nbma *nbr_nbma)
{
    struct prefix_ipv4 p;
    struct route_node *rn = NULL;
	
    p.family = AF_INET;
    p.prefix = nbr_nbma->addr;
    p.prefixlen = IPV4_MAX_BITLEN;
    rn = route_node_lookup (ospf->nbr_nbma, (struct prefix *)&p);
    if (rn)
    {
        ospf_nbr_nbma_free (rn->info);
        rn->info = NULL;
        route_unlock_node (rn);
        route_unlock_node (rn);
    }
}

static void
ospf_nbr_nbma_down (struct ospf_nbr_nbma *nbr_nbma)
{
    OSPF_TIMER_OFF (nbr_nbma->t_poll);
    if (nbr_nbma->nbr)
    {
        nbr_nbma->nbr->nbr_nbma = NULL;
        OSPF_NSM_EVENT_EXECUTE (nbr_nbma->nbr, NSM_KillNbr);
    }
    if (nbr_nbma->oi)
    {
        listnode_delete (nbr_nbma->oi->nbr_nbma, nbr_nbma);
    }
}

static void
ospf_nbr_nbma_add (struct ospf_nbr_nbma *nbr_nbma,
                   struct ospf_interface *oi)
{
    struct prefix p;
    struct route_node *rn = NULL;
    struct ospf_neighbor *nbr = NULL;
	
    if (oi->type != OSPF_IFTYPE_NBMA)
    {
        return;
    }
    if (nbr_nbma->nbr != NULL)
    {
        return;
    }
    if (IPV4_ADDR_SAME (&oi->nbr_self->address.u.prefix4, &nbr_nbma->addr))
    {
        return;
    }
    nbr_nbma->oi = oi;
    listnode_add (oi->nbr_nbma, nbr_nbma);
    /* Get neighbor information from table. */
    p.family = AF_INET;
    p.prefixlen = IPV4_MAX_BITLEN;
    p.u.prefix4 = nbr_nbma->addr;
    rn = route_node_get (oi->nbrs, (struct prefix *)&p);
    if (rn->info)
    {
        nbr = rn->info;
        nbr->nbr_nbma = nbr_nbma;
        nbr_nbma->nbr = nbr;
        route_unlock_node (rn);
    }
    else
    {
        nbr = rn->info = ospf_nbr_new (oi);
        nbr->state = NSM_Down;
        nbr->src = nbr_nbma->addr;
        nbr->nbr_nbma = nbr_nbma;
        nbr->priority = nbr_nbma->priority;
        nbr->address = p;
        nbr_nbma->nbr = nbr;
        OSPF_NSM_EVENT_EXECUTE (nbr, NSM_Start);
    }
}

void
ospf_nbr_nbma_if_update (struct ospf *ospf, struct ospf_interface *oi)
{
    struct prefix_ipv4 p;
	struct route_node *rn = NULL;
    struct ospf_nbr_nbma *nbr_nbma = NULL;
	
    if (oi->type != OSPF_IFTYPE_NBMA)
    {
        return;
    }
	
    for (rn = route_top (ospf->nbr_nbma); rn; rn = route_next (rn))
    {
        if ((nbr_nbma = rn->info))
        {
            if (nbr_nbma->oi == NULL && nbr_nbma->nbr == NULL)
            {
                p.family = AF_INET;
                p.prefix = nbr_nbma->addr;
                p.prefixlen = IPV4_MAX_BITLEN;
                if (prefix_match (oi->address, (struct prefix *)&p))
                {
                    ospf_nbr_nbma_add (nbr_nbma, oi);
                }
            }
        }
    }
}

struct ospf_nbr_nbma *
ospf_nbr_nbma_lookup (struct ospf *ospf, struct in_addr nbr_addr)
{
    struct prefix_ipv4 p;
    struct route_node *rn = NULL;

    p.family = AF_INET;
    p.prefix = nbr_addr;
    p.prefixlen = IPV4_MAX_BITLEN;

    rn = route_node_lookup (ospf->nbr_nbma, (struct prefix *)&p);
    if (rn)
    {
        route_unlock_node (rn);
        return rn->info;
    }
    return NULL;
}

struct ospf_nbr_nbma *
ospf_nbr_nbma_lookup_next (struct ospf *ospf, struct in_addr *addr, int first)
{
    if (ospf == NULL)
    {
        return NULL;
    }

    return NULL;
}

int
ospf_nbr_nbma_set (struct ospf *ospf, struct in_addr nbr_addr)
{
    struct prefix_ipv4 p;
    struct route_node *rn = NULL;
    struct listnode *node = NULL;
    struct ospf_interface *oi = NULL;
    struct ospf_nbr_nbma *nbr_nbma = NULL;
	
    nbr_nbma = ospf_nbr_nbma_lookup (ospf, nbr_addr);
    if (nbr_nbma)
    {
        return 0;
    }
	
    nbr_nbma = ospf_nbr_nbma_new ();
    nbr_nbma->addr = nbr_addr;
    p.family = AF_INET;
    p.prefix = nbr_addr;
    p.prefixlen = IPV4_MAX_BITLEN;
    rn = route_node_get (ospf->nbr_nbma, (struct prefix *)&p);
    if (rn->info)
    {
        route_unlock_node (rn);
    }
    rn->info = nbr_nbma;
    for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, node, oi))
    {
        if (oi->type == OSPF_IFTYPE_NBMA)
        {
            if (prefix_match (oi->address, (struct prefix *)&p))
            {
                ospf_nbr_nbma_add (nbr_nbma, oi);
                break;
            }
        }
    }
    return 1;
}

int
ospf_nbr_nbma_unset (struct ospf *ospf, struct in_addr nbr_addr)
{
    struct ospf_nbr_nbma *nbr_nbma = NULL;
	
    nbr_nbma = ospf_nbr_nbma_lookup (ospf, nbr_addr);
    if (nbr_nbma == NULL)
    {
        return ERRNO_FAIL;
    }
	
    ospf_nbr_nbma_down (nbr_nbma);
    ospf_nbr_nbma_delete (ospf, nbr_nbma);
    return ERRNO_SUCCESS;
}

int
ospf_nbr_nbma_priority_set (struct ospf *ospf, struct in_addr nbr_addr,
                            u_char priority)
{
    struct ospf_nbr_nbma *nbr_nbma = NULL;
	
    nbr_nbma = ospf_nbr_nbma_lookup (ospf, nbr_addr);
    if (nbr_nbma == NULL)
    {
        return 0;
    }
	
    if (nbr_nbma->priority != priority)
    {
        nbr_nbma->priority = priority;
    }
    return 1;
}

int
ospf_nbr_nbma_priority_unset (struct ospf *ospf, struct in_addr nbr_addr)
{
    struct ospf_nbr_nbma *nbr_nbma = NULL;
	
    nbr_nbma = ospf_nbr_nbma_lookup (ospf, nbr_addr);
    if (nbr_nbma == NULL)
    {	
        return 0;
    }
	
    if (nbr_nbma->priority != OSPF_NEIGHBOR_PRIORITY_DEFAULT)
    {
        nbr_nbma->priority = OSPF_NEIGHBOR_PRIORITY_DEFAULT;
    }
    return 1;
}

int
ospf_nbr_nbma_poll_interval_set (struct ospf *ospf, struct in_addr nbr_addr,
                                 unsigned int interval)
{
    struct ospf_nbr_nbma *nbr_nbma = NULL;
	
    nbr_nbma = ospf_nbr_nbma_lookup (ospf, nbr_addr);
    if (nbr_nbma == NULL)
    {
        return 0;
    }
	
    if (nbr_nbma->v_poll != interval)
    {
        nbr_nbma->v_poll = interval;
        if (nbr_nbma->oi && ospf_if_is_up (nbr_nbma->oi))
        {
            OSPF_TIMER_OFF (nbr_nbma->t_poll);
            OSPF_POLL_TIMER_ON (nbr_nbma->t_poll, ospf_poll_timer,
                                nbr_nbma->v_poll);
        }
    }
    return 1;
}

int
ospf_nbr_nbma_poll_interval_unset (struct ospf *ospf, struct in_addr addr)
{
    struct ospf_nbr_nbma *nbr_nbma = NULL;
	
    nbr_nbma = ospf_nbr_nbma_lookup (ospf, addr);
    if (nbr_nbma == NULL)
    {
        return 0;
    }
    if (nbr_nbma->v_poll != OSPF_POLL_INTERVAL_DEFAULT)
    {
        nbr_nbma->v_poll = OSPF_POLL_INTERVAL_DEFAULT;
    }
    return 1;
}

static int
ospf_id_cmp (struct ospf* ospf1, struct ospf *ospf2)
{
    if (ospf1->ospf_id > ospf2->ospf_id)
    {
        return 1;
    }
    if (ospf1->ospf_id < ospf2->ospf_id)
    {
        return -1;
    }
    return 0;
}


void
ospf_master_init (void)
{
    int i;
	int ret = 0;
    char ip_str[255];  
    struct prefix_ipv4 p;
	
    memset (&ospf_master, 0, sizeof (struct ospf_master));
    om = &ospf_master;
    om->ospf = list_new ();
    om->ospf->cmp = (int (*)(void *, void *)) ospf_id_cmp;
    om->master = master_ospf;
    om->start_time = time_get_real_time (NULL);
    
    /* init the redis count num */
    for (i = 0; i <= ROUTE_PROTO_MAX; i++)
    {
        om->redis_count[i] = 0;
    }
    om->t_read = NULL;
    om->t_thread_routefifo = 0;
    FIFO_INIT (&om->routefifo);
    //FIFO_INIT (&om->ifmfifo);
	//FIFO_INIT (&om->routefifo_rcv);

	//get device name
	memset(om->device_name, 0, sizeof(om->device_name));
	ret = devm_comm_get_dev_name(1, 0, MODULE_ID_OSPF, (uchar *)om->device_name);
	if(ret)
	{
		zlog_err("%s(): get dev name error!\n", __func__);
		sprintf(om->device_name, "%s", "DEV NAME UNKNOWN");
	}

	//get company name
	memset(om->company_name, 0, sizeof(om->company_name));
	ret = devm_comm_get_manufacture(1, MODULE_ID_OSPF, (uchar *)om->company_name);
	if(ret)
	{
		zlog_err("%s(): get Company name error!\n", __func__);
		sprintf(om->company_name, "%s", "COMPANY NAME UNKNOWN");
	}

	//get device mac
	memset(om->mac, 0, sizeof(om->mac));
    ret = devm_comm_get_mac ( 1, 0, MODULE_ID_OSPF, om->mac);
    if ( ret )
    {
        zlog_err ( "%s[%d] fail to get mac", __FUNCTION__, __LINE__ );
    }
	
    memset (ip_str, 0, sizeof (ip_str));
    sprintf(ip_str, "%d.%d.%d.%d/32", OSPF_DCN_NE_IP_FREFIX, om->mac[3], om->mac[4],om->mac[5]);

	memset (&p, 0, sizeof (struct prefix_ipv4));
    ret = str2prefix_ipv4 (ip_str, &p);
    om->ne_id_dynamic.s_addr = (p.prefix.s_addr & htonl(0xffffff));
    om->ne_id_static.s_addr = 0;
	om->ne_ip_dynamic.s_addr = p.prefix.s_addr;
	om->ne_ip_dynamic_prefixlen = p.prefixlen;
    om->ne_ip_static.s_addr = 0;
	om->ne_ip_static_prefixlen = 0;

	om->u0_flag = 0;
	om->u0_list = list_new ();
}

/* area name set */
int
ospf_name_set (struct ospf_area *area, const char *name)
{
    if (area->area_name)
    {
        XFREE (MTYPE_OSPF_AREA_NAME, area->area_name);
    }
    area->area_name = XSTRDUP (MTYPE_OSPF_AREA_NAME, name);
    return 0;
}
/* area name unset */
int
ospf_name_unset (struct ospf_area *area)
{
    if (area->area_name)
    {
        XFREE (MTYPE_OSPF_AREA_NAME, area->area_name);
    }
    area->area_name = NULL;
    return 0;
}





