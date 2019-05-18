/* BGP-4 Finite State Machine
   From RFC1771 [A Border Gateway Protocol 4 (BGP-4)]
   Copyright (C) 1996, 97, 98 Kunihiro Ishiguro

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

#include <zebra.h>

#include "linklist.h"
#include "prefix.h"
#include "vty.h"
#include "sockunion.h"
#include "thread.h"
#include "log.h"
#include "stream.h"
#include "memory.h"
#include "plist.h"
#include "workqueue.h"
#include "filter.h"
#include <lib/ifm_common.h>
#include "lib/memtypes.h"
#include "bgpd/bgpd.h"
#include "bgpd/bgp_attr.h"
#include "bgpd/bgp_debug.h"
#include "bgpd/bgp_fsm.h"
#include "bgpd/bgp_packet.h"
#include "bgpd/bgp_route.h"
#include "bgpd/bgp_dump.h"
#include "bgpd/bgp_open.h"
#include "bgpd/bgp_nexthop.h"
#ifdef HAVE_SNMP
#include "bgpd/bgp_snmp.h"
#endif /* HAVE_SNMP */

/* BGP FSM (finite state machine) has three types of functions.  Type
   one is thread functions.  Type two is event functions.  Type three
   is FSM functions.  Timer functions are set by bgp_timer_set
   function. */

/* BGP event function. */
int bgp_event (void *para);

/* BGP thread functions. */
static int bgp_start_timer (void * para);
static int bgp_connect_timer (void *para);
static int bgp_holdtime_timer (void *para);
static int bgp_keepalive_timer (void *para);

/* BGP FSM functions. */
static int bgp_start (struct peer *);

/* BGP start timer jitter. */
static int
bgp_start_jitter (int t)
{
    return ((random () % (t + 1)) - (t / 2));
}

/* 获取接口的 IP */
struct ipc_mesg_n *ifm_get_l3if_n ( uint32_t ifindex, int module_id )
{
    return ipc_sync_send_n2 ( NULL, 0, 1 , MODULE_ID_ROUTE, module_id, IPC_TYPE_L3IF,
                               0, IPC_OPCODE_GET, ifindex, 2000);
}

static int peer_check_address(struct peer *peer)
{
	afi_t afi = AFI_IP;
    int local_flag = 0;
	int remote_flag = 0;
	int source_flag = 1;
    struct prefix p;
    struct bgp *bgp;
    struct bgp_static *bgp_static;
    struct bgp_node *rn;
    struct ifm_l3 *l3 = NULL;
    struct ipc_mesg_n *pMsgRcv = NULL;
	
    if(!peer)
    {
        return 0;
    }

	if(peer->su_local.sin6.sin6_family == AF_INET6)
	{
		afi = AFI_IP6;
	}
		
    if((afi == AFI_IP && (!peer->su_local.sin.sin_addr.s_addr || !peer->su.sin.sin_addr.s_addr) )
		|| (afi == AFI_IP6 &&((ipv6_is_zero((struct ipv6_addr *)&peer->su_local.sin6.sin6_addr))|| (ipv6_is_zero((struct ipv6_addr *)&peer->su.sin6.sin6_addr)))))
    {
        return 0;
    }
	
    bgp = bgp_get_default();
    if(!bgp)
    {
        return 0;
    }

    if(!bgp->route[afi][SAFI_UNICAST])
    {
        return 0;
    }
	
    //if(BGP_DEBUG (fsm, FSM))
    //{
		char buf[2][SU_ADDRSTRLEN];
        zlog_debug(BGP_DEBUG_TYPE_FSM,"%s[%d] vrf %d su_local %s su %s", __FUNCTION__, __LINE__, peer->vrf_id, sockunion2str (&peer->su_local, buf[0], SU_ADDRSTRLEN),sockunion2str (&peer->su, buf[1], SU_ADDRSTRLEN));
	//}

	if(peer->vrf_id)
	{
		if(bgp_vrf_redist_info[peer->vrf_id] == NULL || bgp_vrf_redist_info[peer->vrf_id]->prd.prefixlen != 64)
        	return 0;
	}
	
    memset (&p, 0, sizeof (struct prefix));
	if(afi == AFI_IP6)
	{
		p.family = AF_INET6;
	    p.prefixlen = IPV6_MAX_BITLEN;
		IPV6_ADDR_COPY(&p.u.prefix6, &peer->su_local.sin6.sin6_addr);
	}
	else
	{
	    p.family = AF_INET;
	    p.prefixlen = IPV4_MAX_BITLEN;
	    p.u.prefix4 = peer->su_local.sin.sin_addr;
	}
	
    for(rn = bgp_table_top (bgp->route[afi][SAFI_UNICAST]); rn; rn = bgp_route_next (rn))
    {
    	for(bgp_static = rn->info; bgp_static ; bgp_static = bgp_static->next)
        {
            /*源IP须是直连接口的IP地址*/
            if((ROUTE_PROTO_CONNECT == bgp_static->type) && prefix_same(&rn->p, &p) && bgp_static->vrf_id == peer->vrf_id)
            {
                local_flag = 1;
                if(IFM_TYPE_IS_LOOPBCK(bgp_static->ifindex))
                {
                    SET_FLAG(peer->flags,PEER_FLAG_LOOPBACK);
                }
                else
                {
                    UNSET_FLAG(peer->flags,PEER_FLAG_LOOPBACK);
                }
				
				pMsgRcv = ifm_get_l3if_n(bgp_static->ifindex, MODULE_ID_BGP);
				
				l3 = (struct ifm_l3 *)pMsgRcv->msg_data;
				if(l3)
				{
					if(afi == AFI_IP6)
					{					
						peer->mask_len = l3->ipv6[0].prefixlen;
					}
					else
					{
						peer->mask_len = l3->ipv4[0].prefixlen;
					}
				}
            	mem_share_free(pMsgRcv, MODULE_ID_BGP);
				
                break;
            }
        }
    }
	
    memset (&p, 0, sizeof (struct prefix));
	if(afi == AFI_IP6)
	{
		p.family = AF_INET6;
	    p.prefixlen = IPV6_MAX_BITLEN;
		IPV6_ADDR_COPY(&p.u.prefix6, &peer->su.sin6.sin6_addr);
	}
	else
	{
	    p.family = AF_INET;
	    p.prefixlen = IPV4_MAX_BITLEN;
	    p.u.prefix4 = peer->su.sin.sin_addr;
	}
	
    for(rn = bgp_table_top (bgp->route[afi][SAFI_UNICAST]); rn; rn = bgp_route_next (rn))
    {
    	for(bgp_static = rn->info; bgp_static ; bgp_static = bgp_static->next)
        {
           	/*目的IP需要有路由*/
            if((ROUTE_PROTO_CONNECT == bgp_static->type) && prefix_same(&rn->p, &p) && bgp_static->vrf_id == peer->vrf_id)
            {
                source_flag = 0;
                break;
            }
        }
    }
	
    for(rn = bgp_table_top (bgp->route[afi][SAFI_UNICAST]); rn; rn = bgp_route_next (rn))
    {
    	for(bgp_static = rn->info; bgp_static ; bgp_static = bgp_static->next)
        {
            memset (&p, 0, sizeof (struct prefix));
			if(afi == AFI_IP6)
			{
				p.family = AF_INET6;
			    p.prefixlen = rn->p.prefixlen;;
				IPV6_ADDR_COPY(&p.u.prefix6, &peer->su.sin6.sin6_addr);
			}
			else
			{
	            p.family = AF_INET;
	            p.u.prefix4 = peer->su.sin.sin_addr;
	            p.prefixlen = rn->p.prefixlen;
			}
			
            apply_mask(&p);
			
			/*目的IP不能是直连的接口的IP地址*/
            if(prefix_same(&rn->p, &p) && peer->vrf_id == bgp_static->vrf_id)
            {
                remote_flag = 1;
                break;
            }
        }
    }
	
    //if(BGP_DEBUG (fsm, FSM))
    //{
        zlog_debug(BGP_DEBUG_TYPE_FSM,"%s[%d] local_addr %d remote_addr %d source_addr %d ",__FUNCTION__,__LINE__,local_flag,remote_flag,source_flag);
    //}
	
    if(local_flag && remote_flag && source_flag)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* Check if suppress start/restart of sessions to peer. */
#define BGP_PEER_START_SUPPRESSED(P) \
  (CHECK_FLAG ((P)->flags, PEER_FLAG_SHUTDOWN) \
   || CHECK_FLAG ((P)->sflags, PEER_STATUS_PREFIX_OVERFLOW))

/* Hook function called after bgp event is occered.  And vty's
   neighbor command invoke this function after making neighbor
   structure. */
void
bgp_timer_set (struct peer *peer)
{
	int ret = 0;
    int jitter = 0;
	
    switch (peer->status)
    {
	    case Idle:
	        /* First entry point of peer's finite state machine.  In Idle
	        status start timer is on unless peer is shutdown or peer is
	         inactive.  All other timer must be turned off */
	        ret = peer_check_address(peer);
	        if (!ret || BGP_PEER_START_SUPPRESSED (peer) || ! peer_active (peer)|| ! CHECK_FLAG(peer->flags,PEER_FLAG_ENABLE))
	        {
	            BGP_TIMER_OFF (peer->t_start);
	        }
	        else
	        {
	            jitter = bgp_start_jitter (peer->v_start);
			   // peer->t_start = high_pre_timer_add("bgp_start_timer", LIB_TIMER_TYPE_NOLOOP, bgp_start_timer, (void *)peer, (peer->v_start + jitter) * 1000);
	            BGP_TIMER_ON (peer->t_start, bgp_start_timer,
	                           peer->v_start + jitter);
	        }
	        BGP_TIMER_OFF (peer->t_connect);
	        BGP_TIMER_OFF (peer->t_holdtime);
	        BGP_TIMER_OFF (peer->t_keepalive);
	        BGP_TIMER_OFF (peer->t_routeadv);
	        break;
	    case Connect:
	        /* After start timer is expired, the peer moves to Connnect
	           status.  Make sure start timer is off and connect timer is
	           on. */
	        BGP_TIMER_OFF (peer->t_start);
			//peer->t_connect = high_pre_timer_add("bgp_connect_timer", LIB_TIMER_TYPE_NOLOOP, bgp_connect_timer, (void *)peer, (peer->v_connect) * 1000 );
	        BGP_TIMER_ON (peer->t_connect, bgp_connect_timer, peer->v_connect);
	        BGP_TIMER_OFF (peer->t_holdtime);
	        BGP_TIMER_OFF (peer->t_keepalive);
	        BGP_TIMER_OFF (peer->t_routeadv);
	        break;
	    case Active:
	        /* Active is waiting connection from remote peer.  And if
	           connect timer is expired, change status to Connect. */
	        BGP_TIMER_OFF (peer->t_start);
			
	        /* If peer is passive mode, do not set connect timer. */
	        if (CHECK_FLAG (peer->flags, PEER_FLAG_PASSIVE) || CHECK_FLAG (peer->sflags, PEER_STATUS_NSF_WAIT))
	        {
	            BGP_TIMER_OFF (peer->t_connect);
	        }
	        else
	        {    
	            //peer->t_connect = high_pre_timer_add("bgp_connect_timer", LIB_TIMER_TYPE_NOLOOP, bgp_connect_timer, (void *)peer, (peer->v_connect) * 1000 ) ;
	            BGP_TIMER_ON (peer->t_connect, bgp_connect_timer, peer->v_connect);
	        }
	        BGP_TIMER_OFF (peer->t_holdtime);
	        BGP_TIMER_OFF (peer->t_keepalive);
	        BGP_TIMER_OFF (peer->t_routeadv);
	        break;
	    case OpenSent:
	        /* OpenSent status. */
	        BGP_TIMER_OFF (peer->t_start);
			//peer->t_connect = high_pre_timer_add("bgp_connect_timer", LIB_TIMER_TYPE_NOLOOP, bgp_connect_timer, (void *)peer, (peer->v_connect) * 1000);
	        BGP_TIMER_ON (peer->t_connect, bgp_connect_timer, peer->v_connect);
	        if (peer->v_holdtime != 0)
	        {  
	            //peer->t_holdtime = high_pre_timer_add("bgp_holdtime_timer", LIB_TIMER_TYPE_NOLOOP, bgp_holdtime_timer, (void *)peer, (peer->v_holdtime) * 1000);
	            BGP_TIMER_ON (peer->t_holdtime, bgp_holdtime_timer,
	                         peer->v_holdtime);
	        }
	        else
	        {
	            BGP_TIMER_OFF (peer->t_holdtime);
	        }
	        BGP_TIMER_OFF (peer->t_keepalive);
	        BGP_TIMER_OFF (peer->t_routeadv);
	        break;
	    case OpenConfirm:
	        /* OpenConfirm status. */
	        BGP_TIMER_OFF (peer->t_start);
	        BGP_TIMER_OFF (peer->t_connect);
		
	        /* If the negotiated Hold Time value is zero, then the Hold Time
	           timer and KeepAlive timers are not started. */
	        if (peer->v_holdtime == 0)
	        {
	            BGP_TIMER_OFF (peer->t_holdtime);
	            BGP_TIMER_OFF (peer->t_keepalive);
	        }
	        else
	        {
	            //peer->t_holdtime = high_pre_timer_add("bgp_holdtime_timer", LIB_TIMER_TYPE_NOLOOP, bgp_holdtime_timer, (void *)peer, (peer->v_holdtime) * 1000 );
			    //peer->t_keepalive = high_pre_timer_add("bgp_holdtime_timer", LIB_TIMER_TYPE_NOLOOP, bgp_keepalive_timer, (void *)peer, (peer->v_keepalive) * 1000 );
	 #if 1      
	            BGP_TIMER_ON (peer->t_holdtime, bgp_holdtime_timer,
	                          peer->v_holdtime);
	            BGP_TIMER_ON (peer->t_keepalive, bgp_keepalive_timer,
	                          peer->v_keepalive);
	#endif
	        }
	        BGP_TIMER_OFF (peer->t_routeadv);
	        break;
	    case Established:
	        /* In Established status start and connect timer is turned
	           off. */
	        BGP_TIMER_OFF (peer->t_start);
	        BGP_TIMER_OFF (peer->t_connect);
			
	        /* Same as OpenConfirm, if holdtime is zero then both holdtime
	           and keepalive must be turned off. */
	        if (peer->v_holdtime == 0)
	        {
	            BGP_TIMER_OFF (peer->t_holdtime);
	            BGP_TIMER_OFF (peer->t_keepalive);
	        }
	        else
	        {
	            //peer->t_holdtime = high_pre_timer_add("bgp_holdtime_timer", LIB_TIMER_TYPE_NOLOOP, bgp_holdtime_timer, (void *)peer, (peer->v_holdtime) * 1000 );
				// peer->t_keepalive = high_pre_timer_add("bgp_holdtime_timer", LIB_TIMER_TYPE_NOLOOP, bgp_keepalive_timer, (void *)peer, (peer->v_keepalive) * 1000);
#if 1
	            BGP_TIMER_ON (peer->t_holdtime, bgp_holdtime_timer,
	                          peer->v_holdtime);
	            BGP_TIMER_ON (peer->t_keepalive, bgp_keepalive_timer,
	                          peer->v_keepalive);
#endif
	        }
	        break;
	    case Deleted:
	        BGP_TIMER_OFF (peer->t_gr_restart);
	        BGP_TIMER_OFF (peer->t_gr_stale);
	        BGP_TIMER_OFF (peer->t_pmax_restart);
	    case Clearing:
	        BGP_TIMER_OFF (peer->t_start);
	        BGP_TIMER_OFF (peer->t_connect);
	        BGP_TIMER_OFF (peer->t_holdtime);
	        BGP_TIMER_OFF (peer->t_keepalive);
	        BGP_TIMER_OFF (peer->t_routeadv);
			break;
		default:
			zlog_err("%s[%d] status %d",__func__,__LINE__, peer->status);
			break;
    }
}

/* BGP start timer.  This function set BGP_Start event to thread value
   and process event. */
static int
bgp_start_timer (void *para)
{
    struct peer *peer;
	
    peer = (struct peer *)para;
    peer->t_start = 0;
	
    if (BGP_DEBUG (fsm, FSM))
        zlog (peer->log, LOG_DEBUG,
              "%s [FSM] Timer (start timer expire).", peer->host);
	
    peer->event = BGP_Start;
    bgp_event ((void *)peer);  /* bgp_event unlocks peer */
	
    return 0;
}

/* BGP connect retry timer. */
static int
bgp_connect_timer (void *para)
{
    struct peer *peer;
	
    peer = (struct peer *)para;;
    peer->t_connect = 0;
	
    if (BGP_DEBUG (fsm, FSM))
        zlog (peer->log, LOG_DEBUG, "%s [FSM] Timer (connect timer expire)",
              peer->host);
	
    peer->event = ConnectRetry_timer_expired;
    bgp_event ((void *)peer); /* bgp_event unlocks peer */
	
    return 0;
}

/* BGP holdtime timer. */
static int
bgp_holdtime_timer (void *para)
{
    struct peer *peer;
	
    peer = (struct peer *) para;
    peer->t_holdtime = 0;
	
    if (BGP_DEBUG (fsm, FSM))
        zlog (peer->log, LOG_DEBUG,
              "%s [FSM] Timer (holdtime timer expire)",
              peer->host);
	
    peer->event = Hold_Timer_expired;
    bgp_event ((void *)peer); /* bgp_event unlocks peer */
	
    return 0;
}

/* BGP keepalive fire ! */
static int
bgp_keepalive_timer (void *para)
{
    struct peer *peer;
	
    peer = (struct peer *)para;;
    peer->t_keepalive = 0;
	
    if (BGP_DEBUG (fsm, FSM))
        zlog (peer->log, LOG_DEBUG,
              "%s [FSM] Timer (keepalive timer expire)",
              peer->host);
	
    peer->event = KeepAlive_timer_expired;
    bgp_event ((void *)peer); /* bgp_event unlocks peer */
	
    return 0;
}

static int
bgp_routeadv_timer (void *para)
{
    struct peer *peer;
	
    peer = (struct peer *)para;;
    peer->t_routeadv = 0;
	
    if (BGP_DEBUG (fsm, FSM))
        zlog (peer->log, LOG_DEBUG,
              "%s [FSM] Timer (routeadv timer expire)",
              peer->host);
	
    peer->synctime = bgp_clock ();
	
    BGP_WRITE_ON (peer->t_write, bgp_write, peer->fd);
    //if (!(peer->t_write) && (peer->status != Deleted))
    //{
    //   bgp_write((void *)peer);
	//}
    //peer->t_routeadv = high_pre_timer_add("bgp_routeadv_timer", LIB_TIMER_TYPE_NOLOOP, bgp_routeadv_timer, (void *)peer, (peer->v_routeadv) * 1000);
    BGP_TIMER_ON (peer->t_routeadv, bgp_routeadv_timer, peer->v_routeadv);
	
    return 0;
}

/* BGP Peer Down Cause */
const char *peer_down_str[] =
{
    "",
    "Router ID changed",
    "Remote AS changed",
    "Local AS change",
    "Cluster ID changed",
    "Confederation identifier changed",
    "Confederation peer changed",
    "RR client config change",
    "RS client config change",
    "Update source change",
    "Address family activated",
    "Admin. shutdown",
    "User reset",
    "BGP Notification received",
    "BGP Notification send",
    "Peer closed the session",
    "Neighbor Idle",
    "Peer-group add member",
    "Peer-group delete member",
    "Capability changed",
    "Passive config change",
    "Multihop config change",
    "NSF peer closed the session"
};

static int
bgp_graceful_restart_timer_expire (void *arg)
{
    afi_t afi;
    safi_t safi;
    struct peer *peer;
	
    peer =  (struct peer *)arg;
    peer->t_gr_restart = 0;
	
    /* NSF delete stale route */
    for (afi = AFI_IP ; afi < AFI_MAX ; afi++)
        for (safi = SAFI_UNICAST ; safi < SAFI_RESERVED_3 ; safi++)
            if (peer->nsf[afi][safi])
            {
                bgp_clear_stale_route (peer, afi, safi);
            }
			
    UNSET_FLAG (peer->sflags, PEER_STATUS_NSF_WAIT);
    BGP_TIMER_OFF (peer->t_gr_stale);
	
    //if (BGP_DEBUG (events, EVENTS))
    //{
        zlog_debug (BGP_DEBUG_TYPE_EVENTS,"%s graceful restart timer expired", peer->host);
        zlog_debug (BGP_DEBUG_TYPE_EVENTS,"%s graceful restart stalepath timer stopped", peer->host);
    //}
	
    bgp_timer_set (peer);
	
    return 0;
}

static int
bgp_graceful_stale_timer_expire (void *arg)
{
    afi_t afi;
    safi_t safi;
    struct peer *peer;
	
    peer = (struct peer * )arg;
    peer->t_gr_stale = 0;
	
    //if (BGP_DEBUG (events, EVENTS))
    //{
        zlog_debug (BGP_DEBUG_TYPE_EVENTS,"%s graceful restart stalepath timer expired", peer->host);
    //}
	
    /* NSF delete stale route */
    for (afi = AFI_IP ; afi < AFI_MAX ; afi++)
        for (safi = SAFI_UNICAST ; safi < SAFI_RESERVED_3 ; safi++)
            if (peer->nsf[afi][safi])
                bgp_clear_stale_route (peer, afi, safi);
			
    return 0;
}

/* Called after event occured, this function change status and reset
   read/write and timer thread. */
void
bgp_fsm_change_status (struct peer *peer, int status)
{
    bgp_dump_state (peer, peer->status, status);
	
    /* Transition into Clearing or Deleted must /always/ clear all routes..
     * (and must do so before actually changing into Deleted..
     */
    if (status >= Clearing)
    {
        bgp_clear_route_all (peer);
        /* If no route was queued for the clear-node processing, generate the
         * completion event here. This is needed because if there are no routes
         * to trigger the background clear-node thread, the event won't get
         * generated and the peer would be stuck in Clearing. Note that this
         * event is for the peer and helps the peer transition out of Clearing
         * state; it should not be generated per (AFI,SAFI). The event is
         * directly posted here without calling clear_node_complete() as we
         * shouldn't do an extra unlock. This event will get processed after
         * the state change that happens below, so peer will be in Clearing
         * (or Deleted).
         */
        if (!work_queue_is_scheduled (peer->clear_node_queue))
        {   
            peer->event = Clearing_Completed;
            BGP_EVENT_ADD (peer);
        }
    }
	
    /* Preserve old status and change into new status. */
    peer->ostatus = peer->status;
    peer->status = status;
	
    //if (BGP_DEBUG (normal, NORMAL) && peer->ostatus <= Established && peer->ostatus != Idle && peer->status != Idle)
      if ( peer->ostatus <= Established && peer->ostatus != Idle && peer->status != Idle)
        zlog_debug (BGP_DEBUG_TYPE_NORMAL,"%s went from %s to %s",
                    peer->host,
                    LOOKUP (bgp_status_msg, peer->ostatus),
                    LOOKUP (bgp_status_msg, peer->status));
}

/* Flush the event queue and ensure the peer is shut down */
static int
bgp_clearing_completed (struct peer *peer)
{
	if(!peer)
	{
		return -1;
	}
	int rc = bgp_stop(peer);
	
    BGP_EVENT_FLUSH (peer);
	
    return rc;
}

/* Administrative BGP peer stop event. */
/* May be called multiple times for the same peer */
int
bgp_stop (struct peer *peer)
{
    afi_t afi;
    safi_t safi;
    char orf_name[BUFSIZ];
	
    /* Can't do this in Clearing; events are used for state transitions */
    if (peer->status != Clearing)
    {
        /* Delete all existing events of the peer */
        BGP_EVENT_FLUSH (peer);
    }
	
    /* Increment Dropped count. */
    if (peer->status == Established)
    {
        peer->dropped++;
		
        /*reset packet statstics*/
        peer->open_in = 0;
        peer->open_out = 0;
        peer->update_in = 0;
        peer->update_out = 0;
        peer->keepalive_in = 0;
        peer->keepalive_out = 0;
        peer->notify_in = 0;
        peer->notify_out = 0;
		
        /* bgp log-neighbor-changes of neighbor Down */
        if (bgp_flag_check (peer->bgp, BGP_FLAG_LOG_NEIGHBOR_CHANGES))
            zlog_debug (BGP_DEBUG_TYPE_OTHER,"%%ADJCHANGE: neighbor %s Down %s", peer->host,
                       peer_down_str [(int) peer->last_reset]);
		
        /* graceful restart */
        if (peer->t_gr_stale)
        {
            BGP_TIMER_OFF (peer->t_gr_stale);
            //if (BGP_DEBUG (events, EVENTS))
            //{
                zlog_debug (BGP_DEBUG_TYPE_EVENTS,"%s graceful restart stalepath timer stopped", peer->host);
            //}
        }
		
        if (CHECK_FLAG (peer->sflags, PEER_STATUS_NSF_WAIT))
        {
            //f (BGP_DEBUG (events, EVENTS))
            //{
                zlog_debug (BGP_DEBUG_TYPE_EVENTS,"%s graceful restart timer started for %d sec",
                            peer->host, peer->v_gr_restart);
                zlog_debug (BGP_DEBUG_TYPE_EVENTS,"%s graceful restart stalepath timer started for %d sec",
                            peer->host, peer->bgp->stalepath_time);
            //}

           //peer->t_gr_restart = high_pre_timer_add("bgp_graceful_restart", LIB_TIMER_TYPE_NOLOOP, bgp_graceful_restart_timer_expire, (void *)peer, (peer->v_routeadv) * 1000);
		   //peer->t_gr_stale = high_pre_timer_add("gp_graceful_stale", LIB_TIMER_TYPE_NOLOOP, bgp_graceful_stale_timer_expire, (void *)peer, (peer->v_routeadv) * 1000);
#if 1
            BGP_TIMER_ON (peer->t_gr_restart, bgp_graceful_restart_timer_expire,
                          peer->v_gr_restart);
            BGP_TIMER_ON (peer->t_gr_stale, bgp_graceful_stale_timer_expire,
                          peer->bgp->stalepath_time);
#endif
        }
        else
        {
            UNSET_FLAG (peer->sflags, PEER_STATUS_NSF_MODE);
            for (afi = AFI_IP ; afi < AFI_MAX ; afi++)
                for (safi = SAFI_UNICAST ; safi < SAFI_RESERVED_3 ; safi++)
                {
                    peer->nsf[afi][safi] = 0;
                }
        }
#ifdef HAVE_SNMP
        bgpTrapBackwardTransition (peer);
#endif /* HAVE_SNMP */
        /* Reset peer synctime */
        peer->synctime = 0;
    }
	
    /* Stop read and write threads when exists. */
    BGP_READ_OFF (peer->t_read);
    BGP_WRITE_OFF (peer->t_write);
    //peer->t_read = 0;
	//peer->t_write = 0;
	
    /* Stop all timers. */
    BGP_TIMER_OFF (peer->t_start);
    BGP_TIMER_OFF (peer->t_connect);
    BGP_TIMER_OFF (peer->t_holdtime);
    BGP_TIMER_OFF (peer->t_keepalive);
    BGP_TIMER_OFF (peer->t_routeadv);
	
    /* Stream reset. */
    peer->packet_size = 0;
	
	peer->mask_len = 0;
	
    /* Clear input and output buffer.  */
    if (peer->ibuf)
    {
        stream_reset (peer->ibuf);
    }
	
    if (peer->ibuf_all)
    {
        stream_reset (peer->ibuf_all);
    }
	
    if (peer->work)
    {
        stream_reset (peer->work);
    }
	
    if (peer->obuf)
    {
        stream_fifo_clean (peer->obuf);
    }

	if (peer->hold_buf)
    {
        stream_fifo_clean (peer->hold_buf);
    }
	
    zlog_debug (BGP_DEBUG_TYPE_EVENTS,"%s peer close tcp send by bgp stop", peer->host);
	
	/*close tcp session*/
    peer_close_tcp_send(peer);
    peer_close_tcp_local(peer);
	
    for (afi = AFI_IP ; afi < AFI_MAX ; afi++)
    {
        for (safi = SAFI_UNICAST ; safi < SAFI_MAX ; safi++)
        {
            /* Reset all negotiated variables */
            peer->afc_nego[afi][safi] = 0;
            peer->afc_adv[afi][safi] = 0;
            peer->afc_recv[afi][safi] = 0;
			
            /* peer address family capability flags*/
            peer->af_cap[afi][safi] = 0;
			
            /* peer address family status flags*/
            peer->af_sflags[afi][safi] = 0;
			
            /* Received ORF prefix-filter */
            peer->orf_plist[afi][safi] = NULL;
			
            /* ORF received prefix-filter pnt */
            sprintf (orf_name, "%s.%d.%d", peer->host, afi, safi);
            prefix_bgp_orf_remove_all (afi, orf_name);
        }
     }
	
    /* Reset keepalive and holdtime */
    if (CHECK_FLAG (peer->config, PEER_CONFIG_TIMER))
    {
        peer->v_keepalive = peer->keepalive;
        peer->v_holdtime = peer->holdtime;
    }
    else
    {
        peer->v_keepalive = peer->bgp->default_keepalive;
        peer->v_holdtime = peer->bgp->default_holdtime;
    }
	
    peer->update_time = 0;
	
    /* set last reset time */
    peer->resettime = peer->uptime = bgp_clock ();
	
    return 0;
}

/* BGP peer is stoped by the error. */
static int
bgp_stop_with_error (struct peer *peer)
{
    /* Double start timer. */
    peer->v_start *= 2;
	
    /* Overflow check. */
    if (peer->v_start >= (60 * 2))
    {
        peer->v_start = (60 * 2);
    }
	
    bgp_stop (peer);
	
    return 0;
}


/* something went wrong, send notify and tear down */
static int
bgp_stop_with_notify (struct peer *peer, u_char code, u_char sub_code)
{
    /* Send notify to remote peer */
    bgp_notify_send (peer, code, sub_code);
	
    /* Clear start timer value to default. */
    peer->v_start = BGP_INIT_START_TIMER;
	
    /* bgp_stop needs to be invoked while in Established state */
    bgp_stop(peer);

    return 0;
}


/* TCP connection open.  Next we send open message to remote peer. And
   add read thread for reading open message. */
static int
bgp_connect_success (struct peer *peer)
{	
    bgp_open_send (peer);
	
    return 0;
}

/* TCP connect fail */
static int
bgp_connect_fail (struct peer *peer)
{
    bgp_stop (peer);
	
    return 0;
}

/* This function is the first starting point of all BGP connection. It
   try to connect to remote peer with non-blocking IO. */
int
bgp_start (struct peer *peer)
{
    if (BGP_PEER_START_SUPPRESSED (peer))
    {
        if (BGP_DEBUG (fsm, FSM))
            plog_err (peer->log, "%s [FSM] Trying to start suppressed peer"
                      " - this is never supposed to happen!", peer->host);
        return -1;
    }

	if(peer_check_address(peer) == 0)
	{   
	    peer->event = TCP_connection_open_failed;
        BGP_EVENT_ADD (peer);
		return 0;
	}
	
	if(bgp_nexthop_connect_check(peer->vrf_id, &peer->su_local) == 0)
	{   
	    peer->event = TCP_connection_open_failed;
        BGP_EVENT_ADD (peer);
		return 0;
	}
    /* Scrub some information that might be left over from a previous,
     * session
     */
    /* Connection information. */
    /* Clear peer capability flag. */
    peer->cap = 0;
	
    /* If the peer is passive mode, force to move to Active mode. */
    if (CHECK_FLAG (peer->flags, PEER_FLAG_PASSIVE))
    {  
        peer->event = TCP_connection_open_failed;
        BGP_EVENT_ADD (peer);
        return 0;
    }
	
    if (!CHECK_FLAG (peer->flags, PEER_FLAG_ENABLE))
    {  
        peer->event = TCP_connection_open_failed;
        BGP_EVENT_ADD (peer);
        return 0;
    }
	
    if (BGP_DEBUG (fsm, FSM))
        plog_debug (peer->log, "%s [FSM] Connect immediately success",
                    peer->host);

    peer->event = TCP_connection_open;
    BGP_EVENT_ADD (peer);
	
    return 0;
}

/* Connect retry timer is expired when the peer status is Connect. */
static int
bgp_reconnect (struct peer *peer)
{
  bgp_stop (peer);
  bgp_start (peer);
  return 0;
}

static int
bgp_fsm_open (struct peer *peer)
{
    /* Send keepalive and make keepalive timer */
    bgp_keepalive_send (peer);
	
    /* Reset holdtimer value. */
    BGP_TIMER_OFF (peer->t_holdtime);
	
    return 0;
}

/* Keepalive send to peer. */
static int
bgp_fsm_keepalive_expire (struct peer *peer)
{
    bgp_keepalive_send (peer);
	
    return 0;
}

/* FSM error, unexpected event.  This is error of BGP connection. So cut the
   peer and change to Idle status. */
static int
bgp_fsm_event_error (struct peer *peer)
{
    plog_err (peer->log, "%s [FSM] unexpected packet received in state %s oldstatus %s",
              peer->host, LOOKUP (bgp_status_msg, peer->status), LOOKUP (bgp_status_msg, peer->ostatus));
	
    return bgp_stop_with_notify (peer, BGP_NOTIFY_FSM_ERR, 0);
}

/* Hold timer expire.  This is error of BGP connection. So cut the
   peer and change to Idle status. */
static int
bgp_fsm_holdtime_expire (struct peer *peer)
{
    if (BGP_DEBUG (fsm, FSM))
        plog_debug (peer->log, "%s status(%s),oldstatus(%s) [FSM] Hold timer expire", peer->host,LOOKUP (bgp_status_msg, peer->ostatus),
                    LOOKUP (bgp_status_msg, peer->status));
	
    return bgp_stop_with_notify (peer, BGP_NOTIFY_HOLD_ERR, 0);
}

/* Status goes to Established.  Send keepalive packet then make first
   update information. */
static int
bgp_establish (struct peer *peer)
{
    afi_t afi;
    safi_t safi;
    int nsf_af_count = 0;
    struct bgp_notify *notify;
	
    /* Reset capability open status flag. */
    if (! CHECK_FLAG (peer->sflags, PEER_STATUS_CAPABILITY_OPEN))
    {
        SET_FLAG (peer->sflags, PEER_STATUS_CAPABILITY_OPEN);
    }
	
    /* Clear last notification data. */
    notify = &peer->notify;
    if (notify->data)
    {
        XFREE (MTYPE_TMP, notify->data);
    }
	
    memset (notify, 0, sizeof (struct bgp_notify));
	
    /* Clear start timer value to default. */
    peer->v_start = BGP_INIT_START_TIMER;
	
    /* Increment established count. */
    peer->established++;
    bgp_fsm_change_status (peer, Established);
	
    /* bgp log-neighbor-changes of neighbor Up */
    if (bgp_flag_check (peer->bgp, BGP_FLAG_LOG_NEIGHBOR_CHANGES))
    {
        zlog_debug (BGP_DEBUG_TYPE_OTHER,"%%ADJCHANGE: neighbor %s Up", peer->host);
    }
	
    /* graceful restart */
    UNSET_FLAG (peer->sflags, PEER_STATUS_NSF_WAIT);
    for (afi = AFI_IP ; afi < AFI_MAX ; afi++)
	{
        for (safi = SAFI_UNICAST ; safi < SAFI_RESERVED_3 ; safi++)
        {
            if (peer->afc_nego[afi][safi]
                    && CHECK_FLAG (peer->cap, PEER_CAP_RESTART_ADV)
                    && CHECK_FLAG (peer->af_cap[afi][safi], PEER_CAP_RESTART_AF_RCV))
            {
                if (peer->nsf[afi][safi]
                        && ! CHECK_FLAG (peer->af_cap[afi][safi], PEER_CAP_RESTART_AF_PRESERVE_RCV))
                {
                    bgp_clear_stale_route (peer, afi, safi);
                }
                peer->nsf[afi][safi] = 1;
                nsf_af_count++;
            }
            else
            {
                if (peer->nsf[afi][safi])
                {
                    bgp_clear_stale_route (peer, afi, safi);
                }
                peer->nsf[afi][safi] = 0;
            }
        }
	}
	
    if (nsf_af_count)
    {
        SET_FLAG (peer->sflags, PEER_STATUS_NSF_MODE);
    }
    else
    {
        UNSET_FLAG (peer->sflags, PEER_STATUS_NSF_MODE);
        if (peer->t_gr_stale)
        {
            BGP_TIMER_OFF (peer->t_gr_stale);
            //if (BGP_DEBUG (events, EVENTS))
            //{
                zlog_debug (BGP_DEBUG_TYPE_EVENTS,"%s graceful restart stalepath timer stopped", peer->host);
            //}
        }
    }
	
    if (peer->t_gr_restart)
    {
        BGP_TIMER_OFF (peer->t_gr_restart);
        //if (BGP_DEBUG (events, EVENTS))
        //{
            zlog_debug (BGP_DEBUG_TYPE_EVENTS,"%s graceful restart timer stopped", peer->host);
        //}
    }
	
#ifdef HAVE_SNMP
    bgpTrapEstablished (peer);
#endif /* HAVE_SNMP */

    /* Reset uptime, send keepalive, send current table. */
    peer->uptime = bgp_clock ();

    /* Send route-refresh when ORF is enabled */
    for (afi = AFI_IP ; afi < AFI_MAX ; afi++)
    {
        for (safi = SAFI_UNICAST ; safi < SAFI_MAX ; safi++)
            if (CHECK_FLAG (peer->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_SM_ADV))
            {
                if (CHECK_FLAG (peer->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_RM_RCV))
                    bgp_route_refresh_send (peer, afi, safi, ORF_TYPE_PREFIX,
                                            REFRESH_IMMEDIATE, 0);
                else if (CHECK_FLAG (peer->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_RM_OLD_RCV))
                    bgp_route_refresh_send (peer, afi, safi, ORF_TYPE_PREFIX_OLD,
                                            REFRESH_IMMEDIATE, 0);
            }
    }
	
    if (peer->v_keepalive)
    {
        bgp_keepalive_send (peer);
    }
	
    /* First update is deferred until ORF or ROUTE-REFRESH is received */
    for (afi = AFI_IP ; afi < AFI_MAX ; afi++)
        for (safi = SAFI_UNICAST ; safi < SAFI_MAX ; safi++)
            if (CHECK_FLAG (peer->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_RM_ADV))
                if (CHECK_FLAG (peer->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_SM_RCV)
                    || CHECK_FLAG (peer->af_cap[afi][safi], PEER_CAP_ORF_PREFIX_SM_OLD_RCV))
                    	SET_FLAG (peer->af_sflags[afi][safi], PEER_STATUS_ORF_WAIT_REFRESH);
				
    bgp_announce_route_all (peer);


	 //peer->t_routeadv = high_pre_timer_add("bgp_routeadv", LIB_TIMER_TYPE_NOLOOP, bgp_routeadv_timer, (void*)peer,  1000);
    BGP_TIMER_ON (peer->t_routeadv, bgp_routeadv_timer, 1);
	
    return 0;
}

/* Keepalive packet is received. */
static int
bgp_fsm_keepalive (struct peer *peer)
{
    /* peer count update */
    peer->keepalive_in++;
	
    BGP_TIMER_OFF (peer->t_holdtime);
	
    return 0;
}

/* Update packet is received. */
static int
bgp_fsm_update (struct peer *peer)
{
    BGP_TIMER_OFF (peer->t_holdtime);
	
    return 0;
}

/* This is empty event. */
static int
bgp_ignore (struct peer *peer)
{
    if (BGP_DEBUG (fsm, FSM) && peer->ostatus <= Established)
        zlog (peer->log, LOG_DEBUG, "%s,status(%s),oldstatus(%s) [FSM] bgp_ignore called", peer->host,LOOKUP (bgp_status_msg, peer->ostatus),
              LOOKUP (bgp_status_msg, peer->status));
	
    return 0;
}

/* Finite State Machine structure */
static const struct
{
    int (*func) (struct peer *);
    int next_state;
} FSM [BGP_STATUS_MAX - 1][BGP_EVENTS_MAX - 1] =
{
    {
        /* Idle state: In Idle state, all events other than BGP_Start is
           ignored.  With BGP_Start event, finite state machine calls
           bgp_start(). */
        {bgp_start,  Connect},  /* BGP_Start                    */
        {bgp_stop,   Idle},     /* BGP_Stop                     */
        {bgp_stop,   Idle},     /* TCP_connection_open          */
        {bgp_stop,   Idle},     /* TCP_connection_closed        */
        {bgp_ignore, Idle},     /* TCP_connection_open_failed   */
        {bgp_stop,   Idle},     /* TCP_fatal_error              */
        {bgp_ignore, Idle},     /* ConnectRetry_timer_expired   */
        {bgp_ignore, Idle},     /* Hold_Timer_expired           */
        {bgp_ignore, Idle},     /* KeepAlive_timer_expired      */
        {bgp_ignore, Idle},     /* Receive_OPEN_message         */
        {bgp_ignore, Idle},     /* Receive_KEEPALIVE_message    */
        {bgp_ignore, Idle},     /* Receive_UPDATE_message       */
        {bgp_ignore, Idle},     /* Receive_NOTIFICATION_message */
        {bgp_ignore, Idle},         /* Clearing_Completed           */
    },
    {
        /* Connect */
        {bgp_ignore,  Connect}, /* BGP_Start                    */
        {bgp_stop,    Idle},    /* BGP_Stop                     */
        {bgp_connect_success, OpenSent}, /* TCP_connection_open          */
        {bgp_stop, Idle},       /* TCP_connection_closed        */
        {bgp_stop, Idle}, /* TCP_connection_open_failed   */
        {bgp_connect_fail, Idle},   /* TCP_fatal_error              */
        {bgp_reconnect, Connect},    /* ConnectRetry_timer_expired   */
        {bgp_ignore,  Idle},    /* Hold_Timer_expired           */
        {bgp_ignore,  Idle},    /* KeepAlive_timer_expired      */
        {bgp_ignore,  Idle},    /* Receive_OPEN_message         */
        {bgp_ignore,  Idle},    /* Receive_KEEPALIVE_message    */
        {bgp_ignore,  Idle},    /* Receive_UPDATE_message       */
        {bgp_stop,    Idle},    /* Receive_NOTIFICATION_message */
        {bgp_ignore,  Idle},         /* Clearing_Completed           */
    },
    {
        /* Active, */
        {bgp_ignore,  Active},  /* BGP_Start                    */
        {bgp_stop,    Idle},    /* BGP_Stop                     */
        {bgp_connect_success, OpenSent}, /* TCP_connection_open          */
        {bgp_stop,    Idle},    /* TCP_connection_closed        */
        {bgp_ignore,  Active},  /* TCP_connection_open_failed   */
        {bgp_ignore,  Idle},    /* TCP_fatal_error              */
        {bgp_start,   Connect}, /* ConnectRetry_timer_expired   */
        {bgp_ignore,  Idle},    /* Hold_Timer_expired           */
        {bgp_ignore,  Idle},    /* KeepAlive_timer_expired      */
        {bgp_ignore,  Idle},    /* Receive_OPEN_message         */
        {bgp_ignore,  Idle},    /* Receive_KEEPALIVE_message    */
        {bgp_ignore,  Idle},    /* Receive_UPDATE_message       */
        {bgp_stop_with_error, Idle}, /* Receive_NOTIFICATION_message */
        {bgp_ignore, Idle},         /* Clearing_Completed           */
    },
    {
        /* OpenSent, */
        {bgp_ignore,  OpenSent},    /* BGP_Start                    */
        {bgp_stop,    Idle},    /* BGP_Stop                     */
        {bgp_stop,    Connect}, /* TCP_connection_open          */
        {bgp_stop,    Connect}, /* TCP_connection_closed        */
        {bgp_stop,    Connect}, /* TCP_connection_open_failed   */
        {bgp_stop,    Connect}, /* TCP_fatal_error              */
        {bgp_ignore,  Idle},    /* ConnectRetry_timer_expired   */
        {bgp_fsm_holdtime_expire, Idle},    /* Hold_Timer_expired           */
        {bgp_ignore,  Idle},    /* KeepAlive_timer_expired      */
        {bgp_fsm_open,    OpenConfirm}, /* Receive_OPEN_message         */
        {bgp_ignore, OpenSent}, /* Receive_KEEPALIVE_message    */
        {bgp_fsm_event_error, Idle}, /* Receive_UPDATE_message       */
        {bgp_stop_with_error, Idle}, /* Receive_NOTIFICATION_message */
        {bgp_ignore, Idle},         /* Clearing_Completed           */
    },
    {
        /* OpenConfirm, */
        {bgp_ignore,  OpenConfirm}, /* BGP_Start                    */
        {bgp_stop,    Idle},    /* BGP_Stop                     */
        {bgp_stop,    Idle},    /* TCP_connection_open          */
        {bgp_stop,    Idle},    /* TCP_connection_closed        */
        {bgp_stop,    Idle},    /* TCP_connection_open_failed   */
        {bgp_stop,    Idle},    /* TCP_fatal_error              */
        {bgp_ignore,  Idle},    /* ConnectRetry_timer_expired   */
        {bgp_fsm_holdtime_expire, Idle},    /* Hold_Timer_expired           */
        {bgp_ignore,  OpenConfirm}, /* KeepAlive_timer_expired      */
        {bgp_ignore,  OpenConfirm}, /* Receive_OPEN_message         */
        {bgp_establish, Established}, /* Receive_KEEPALIVE_message    */
        {bgp_ignore,  Idle},    /* Receive_UPDATE_message       */
        {bgp_stop_with_error, Idle}, /* Receive_NOTIFICATION_message */
        {bgp_ignore, Idle},         /* Clearing_Completed           */
    },
    {
        /* Established, */
        {bgp_ignore,               Established}, /* BGP_Start                    */
        {bgp_stop,                    Clearing}, /* BGP_Stop                     */
        {bgp_stop,                    Clearing}, /* TCP_connection_open          */
        {bgp_stop,                    Clearing}, /* TCP_connection_closed        */
        {bgp_stop,                 Clearing},   /* TCP_connection_open_failed   */
        {bgp_stop,                    Clearing}, /* TCP_fatal_error              */
        {bgp_stop,                 Clearing},   /* ConnectRetry_timer_expired   */
        {bgp_fsm_holdtime_expire,     Clearing}, /* Hold_Timer_expired           */
        {bgp_fsm_keepalive_expire, Established}, /* KeepAlive_timer_expired      */
		{bgp_stop,					  Clearing}, /* Receive_OPEN_message		 */
        {bgp_fsm_keepalive,        Established}, /* Receive_KEEPALIVE_message    */
        {bgp_fsm_update,           Established}, /* Receive_UPDATE_message       */
        {bgp_stop_with_error,         Clearing}, /* Receive_NOTIFICATION_message */
        {bgp_ignore,                      Idle}, /* Clearing_Completed           */
    },
    {
        /* Clearing, */
        {bgp_ignore,  Clearing},    /* BGP_Start                    */
        {bgp_stop,          Clearing},  /* BGP_Stop                     */
        {bgp_stop,          Clearing},  /* TCP_connection_open          */
        {bgp_stop,          Clearing},  /* TCP_connection_closed        */
        {bgp_stop,          Clearing},  /* TCP_connection_open_failed   */
        {bgp_stop,          Clearing},  /* TCP_fatal_error              */
        {bgp_stop,          Clearing},  /* ConnectRetry_timer_expired   */
        {bgp_stop,          Clearing},  /* Hold_Timer_expired           */
        {bgp_stop,          Clearing},  /* KeepAlive_timer_expired      */
        {bgp_stop,          Clearing},  /* Receive_OPEN_message         */
        {bgp_stop,          Clearing},  /* Receive_KEEPALIVE_message    */
        {bgp_stop,          Clearing},  /* Receive_UPDATE_message       */
        {bgp_stop,          Clearing},  /* Receive_NOTIFICATION_message */
        {bgp_clearing_completed,    Idle},      /* Clearing_Completed           */
    },
    {
        /* Deleted, */
        {bgp_ignore,  Deleted}, /* BGP_Start                    */
        {bgp_ignore,  Deleted}, /* BGP_Stop                     */
        {bgp_ignore,  Deleted}, /* TCP_connection_open          */
        {bgp_ignore,  Deleted}, /* TCP_connection_closed        */
        {bgp_ignore,  Deleted}, /* TCP_connection_open_failed   */
        {bgp_ignore,  Deleted}, /* TCP_fatal_error              */
        {bgp_ignore,  Deleted}, /* ConnectRetry_timer_expired   */
        {bgp_ignore,  Deleted}, /* Hold_Timer_expired           */
        {bgp_ignore,  Deleted}, /* KeepAlive_timer_expired      */
        {bgp_ignore,  Deleted}, /* Receive_OPEN_message         */
        {bgp_ignore,  Deleted}, /* Receive_KEEPALIVE_message    */
        {bgp_ignore,  Deleted}, /* Receive_UPDATE_message       */
        {bgp_ignore,  Deleted}, /* Receive_NOTIFICATION_message */
        {bgp_ignore,  Deleted}, /* Clearing_Completed           */
    },
};

static const char *bgp_event_str[] =
{
    NULL,
    "BGP_Start",
    "BGP_Stop",
    "TCP_connection_open",
    "TCP_connection_closed",
    "TCP_connection_open_failed",
    "TCP_fatal_error",
    "ConnectRetry_timer_expired",
    "Hold_Timer_expired",
    "KeepAlive_timer_expired",
    "Receive_OPEN_message",
    "Receive_KEEPALIVE_message",
    "Receive_UPDATE_message",
    "Receive_NOTIFICATION_message",
    "Completed",
};

/* Execute event process. */
int
bgp_event (void *para)
{
    int ret = 0;
    int event;
    int next;
    struct peer *peer;
	
    peer = (struct peer *)para;
    event = peer->event;
	
    /* Logging this event. */
    next = FSM [peer->status -1][event - 1].next_state;
	
    if (BGP_DEBUG (fsm, FSM) && peer->status != next && peer->status != Clearing)
        plog_debug (peer->log, "%s [FSM] %s (%s->%s)", peer->host,
                    bgp_event_str[event],
                    LOOKUP (bgp_status_msg, peer->status),
                    LOOKUP (bgp_status_msg, next));
	
    /* Call function. */
    if (FSM [peer->status -1][event - 1].func)
    {
        ret = (*(FSM [peer->status - 1][event - 1].func))(peer);
    }
	
    /* When function do not want proceed next job return -1. */
    if (ret >= 0)
    {
        /* If status is changed. */
        if (next != peer->status)
        {
            bgp_fsm_change_status (peer, next);
        }

		if(peer->status == Clearing || peer->status == Deleted)
        {
           peer->status = Idle;
		}
		
        /* Make sure timer is set. */
        bgp_timer_set (peer);
    }
	
    return ret;
}
