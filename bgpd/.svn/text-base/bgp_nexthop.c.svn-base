/* BGP nexthop scan
   Copyright (C) 2000 Kunihiro Ishiguro

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

#include "command.h"
#include "thread.h"
#include "prefix.h"
#include "stream.h"
#include "network.h"
#include "log.h"
#include "memory.h"
#include "hash.h"
#include "jhash.h"
#include "filter.h"
#include <lib/msg_ipc.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_tcp.h>
#include <lib/hptimer.h>

#include "bgpd/bgpd.h"
#include "bgpd/bgp_table.h"
#include "bgpd/bgp_route.h"
#include "bgpd/bgp_attr.h"
#include "bgpd/bgp_nexthop.h"
#include "bgpd/bgp_debug.h"
#include "bgpd/bgp_damp.h"
#include "bgpd/bgp_ipc.h"
#include "bgpd/bgp_zebra.h"
#include "bgpd/bgp_vty.h"

/* Only one BGP scan thread are activated at the same time. */
static struct thread *bgp_scan_thread = NULL;

static TIMERID  bgp_scan_timeid;

/* BGP scan interval. */
static TIMERID bgp_scan_interval;

/* BGP import interval. */
static int bgp_import_interval;

/* Route table for connected route. */
static struct bgp_table *bgp_connected_table[AFI_MAX][BGP_VRF_MAX + 1];

int
bgp_nexthop_connect_check (uint16_t vrf_id, union sockunion *su)

{
	struct bgp_node *rn = NULL;

	if (AF_INET == su->sa.sa_family)
	{
		rn = bgp_node_match_ipv4 (bgp_connected_table[AFI_IP][vrf_id], &su->sin.sin_addr);
		if (rn)
		{
		  bgp_unlock_node (rn);
		  return 1;
		}
	}
	else if (AF_INET6 == su->sa.sa_family)
	{
		rn = bgp_node_match_ipv6 (bgp_connected_table[AFI_IP6][vrf_id],&su->sin6.sin6_addr);
		if (rn)
		{
			bgp_unlock_node (rn);
			return 1;
		}
	}
	return 0;
}

/* If nexthop exists on connected network return 1. */
int
bgp_nexthop_onlink (afi_t afi, struct attr *attr, uint16_t vrf_id)
{
    struct bgp_node *rn;
	
    /* Lookup the address is onlink or not. */
    if (afi == AFI_IP)
    {
        rn = bgp_node_match_ipv4 (bgp_connected_table[AFI_IP][vrf_id], &attr->nexthop);
        if (rn)
        {
            bgp_unlock_node (rn);
            return 1;
        }
    }
    else if (afi == AFI_IP6)
    {
        if (attr->extra->mp_nexthop_len == 32)
        {
            return 1;
        }
        else if (attr->extra->mp_nexthop_len == 16)
        {
            if (IN6_IS_ADDR_LINKLOCAL (&attr->extra->mp_nexthop_global))
            {
                return 1;
            }
            rn = bgp_node_match_ipv6 (bgp_connected_table[AFI_IP6][vrf_id], &attr->extra->mp_nexthop_global);
            if (rn)
            {
                bgp_unlock_node (rn);
                return 1;
            }
        }
    }
    return 0;
}

/* Check specified next-hop is reachable or not. */
int
bgp_nexthop_lookup (afi_t afi, struct peer *peer, struct bgp_info *ri,struct prefix *q, uint32_t *ifindex)
{
	u_int16_t vrf_id = 0;
    struct bgp_node *rn;
    struct prefix p, d;
    struct in_addr addr;
    struct bgp *bgp;
    struct bgp_static *bi;
	struct attr *attr;
	
    if (afi == AFI_IP6)
    { 
    	attr = ri->attr;
		if(attr == NULL || attr->extra == NULL)
        	return 0;
		
		memset (&p, 0, sizeof (struct prefix));
		p.family = AF_INET6;
		p.prefixlen = IPV6_MAX_BITLEN;
		memcpy(&p.u.prefix6, &attr->extra->mp_nexthop_global, IPV6_MAX_BYTELEN);
		
	    memset (&d, 0, sizeof (struct prefix));
		d.family = AF_INET6;
		d.prefixlen = peer->mask_len;
		memcpy(&d.u.prefix6, &peer->su_local.sin6.sin6_addr, IPV6_MAX_BYTELEN);
		apply_mask(&d);
    }

	if(AFI_IP == afi)
	{
	    addr = ri->attr->nexthop;
	    memset (&p, 0, sizeof (struct prefix));
	    p.family = AF_INET;
	    p.prefixlen = IPV4_MAX_BITLEN;
	    p.u.prefix4 = addr;

	    memset (&d, 0, sizeof (struct prefix));
		d.family = AF_INET;
		d.prefixlen = peer->mask_len;
		d.u.prefix4 = peer->su_local.sin.sin_addr;
		apply_mask(&d);
	}
	
    bgp = peer->bgp;
    if(!bgp)
    {
        bgp = bgp_get_default();   
		if(!bgp)
	    {
	        return 0;
	    }
    }
		
    if(AFI_IP == afi && q->u.prefix4.s_addr && prefix_match(q,&p))
    {    
		if(prefix_match(&d,q) || (CHECK_FLAG(peer->flags, PEER_FLAG_LOOPBACK) && q->prefixlen == IPV4_MAX_PREFIXLEN))
        	return 0;
    }
	
    if(AFI_IP6 == afi && ipv6_is_zero((struct ipv6_addr *)&q->u.prefix6) == 0 && prefix_match(q,&p))
    {
		if(prefix_match(&d,q) || (CHECK_FLAG(peer->flags, PEER_FLAG_LOOPBACK) && q->prefixlen == IPV6_MAX_PREFIXLEN))
        	return 0;
    }

	if(ri->vrf_id)
	{
		vrf_id = ri->vrf_id;
	}
	else if(ri->vrf_id_to)
	{
		vrf_id = ri->vrf_id_to;
	}
	
    rn = bgp_node_match (bgp->route[afi][SAFI_UNICAST], &p);
    if(rn)
    {
        bgp_unlock_node (rn);
        for(bi = rn->info ; bi; bi = bi->next)
        {        	
        	if(vrf_id != bi->vrf_id)
    		{
				continue;
    		}
			
            if(ROUTE_PROTO_CONNECT == bi->type 
				&& ((rn->p.family == AF_INET && rn->p.prefixlen == IPV4_MAX_BITLEN)
				    || (rn->p.family == AF_INET6 && rn->p.prefixlen == IPV6_MAX_BITLEN)))
            {
                return 0;
            }
            else
            {
                (bgp_info_extra_get (ri))->igpmetric = bi->igpmetric;
				
				if(ifindex)
				{
					*ifindex = bi->ifindex;
				}
				else
				{
					ri->ifindex = bi->ifindex;
				}
				
                return 1;
            }
        }
    }
    return 0;
}

static void
bgp_scan (afi_t afi, safi_t safi)
{
    struct prefix p, d;
    struct bgp_node *rn;
    struct bgp *bgp;
    struct bgp_info *bi;
    struct bgp_info *next;
    struct peer *peer;
    struct listnode *node, *nnode;
    int valid;
    int current;
	uint32_t ifindex = 0;
	
    /* Get default bgp. */
    bgp = bgp_get_default ();
    if (bgp == NULL)
    {
        return;
    }
	
    /* Maximum prefix check */
    for (ALL_LIST_ELEMENTS (bgp->peer, node, nnode, peer))
    {
        if (peer->status != Established)
        {
            continue;
        }
		
        if (peer->afc[afi][SAFI_UNICAST])
        {
            bgp_maximum_prefix_overflow (peer, afi, SAFI_UNICAST, 1);
        }
		
        if (peer->afc[afi][SAFI_MULTICAST])
        {
            bgp_maximum_prefix_overflow (peer, afi, SAFI_MULTICAST, 1);
        }
		
        if (peer->afc[afi][SAFI_MPLS_VPN])
        {
            bgp_maximum_prefix_overflow (peer, afi, SAFI_MPLS_VPN, 1);
        }
    }
	
    for (rn = bgp_table_top (bgp->rib[afi][SAFI_UNICAST]); rn; rn = bgp_route_next (rn))
    {
        for (bi = rn->info; bi; bi = next)
        {
            next = bi->next;
            if (bi->type == ROUTE_PROTO_EBGP && bi->sub_type == BGP_ROUTE_NORMAL)
            {
                memset (&p, 0, sizeof (struct prefix));
				memset (&d, 0, sizeof (struct prefix));
				
				if (afi == AFI_IP6)
				{
					if(bi->attr && bi->attr->extra)
					{
						p.family = AF_INET6;
						p.prefixlen = IPV6_MAX_BITLEN;
						p.u.prefix6 = bi->attr->extra->mp_nexthop_global;
					}
					
					if(bi->peer &&  ipv6_is_zero((struct ipv6_addr *)&bi->peer->su_local.sin6.sin6_addr) == 0)
					{
					    d.family = AF_INET6;
					    d.prefixlen = bi->peer->mask_len;
						memcpy(&d.u.prefix6, &bi->peer->su_local.sin6.sin6_addr, IPV6_MAX_BYTELEN);
						apply_mask(&d);
					}
				}
				else
				{
	                p.family = AF_INET;
	                p.prefixlen = IPV4_MAX_BITLEN;
	                p.u.prefix4 = bi->attr->nexthop;
					
					if(bi->peer && bi->peer->su_local.sin.sin_addr.s_addr)
					{
					    d.family = AF_INET;
					    d.prefixlen = bi->peer->mask_len;
					    d.u.prefix4 = bi->peer->su_local.sin.sin_addr;
						apply_mask(&d);
					}
				}
				
                if (bi->peer->sort == BGP_PEER_EBGP && bi->peer->ttl == 1
	                && !CHECK_FLAG(bi->peer->flags, PEER_FLAG_DISABLE_CONNECTED_CHECK)
	                && !CHECK_FLAG(bi->peer->flags, PEER_FLAG_LOOPBACK))
                {
                    valid = bgp_nexthop_onlink (afi, bi->attr, 0);
					ifindex = bi->ifindex;
                }
                else
                {
                    valid = bgp_nexthop_lookup (afi, bi->peer, bi, &rn->p, &ifindex);
                }
				
                if(bgp_nexthop_self (bi->attr))
                {
                    valid = 0;
                }
				
                if(afi == AFI_IP && rn->p.u.prefix4.s_addr && prefix_match(&rn->p,&p))
                {
					if(prefix_match(&d,&rn->p) || (CHECK_FLAG(bi->peer->flags, PEER_FLAG_LOOPBACK) && rn->p.prefixlen == IPV4_MAX_PREFIXLEN))
					{
						valid = 0;
					}
                }
				
	            if(afi == AFI_IP6 && ipv6_is_zero((struct ipv6_addr *)&rn->p.u.prefix6) == 0 && prefix_match(&rn->p,&p))
                {
					if(prefix_match(&d,&rn->p) || (CHECK_FLAG(bi->peer->flags, PEER_FLAG_LOOPBACK) && rn->p.prefixlen == IPV6_MAX_PREFIXLEN))
					{
						valid = 0;
					}
                }
								
                current = CHECK_FLAG (bi->flags, BGP_INFO_VALID) ? 1 : 0;
				
                if (valid != current)
                {
                    SET_FLAG (bi->flags, BGP_INFO_ATTR_CHANGED);
                    if (CHECK_FLAG (bi->flags, BGP_INFO_VALID))
                    {
                        bgp_aggregate_decrement (bgp, &rn->p, bi,
                                                 afi, SAFI_UNICAST);
                        bgp_info_unset_flag (rn, bi, BGP_INFO_VALID);
												
						if (!CHECK_FLAG (bi->flags, BGP_INFO_COUNTED))
					    {
					        struct bgp_table *table;
					        if(rn && bgp_node_table (rn))
					        {
					            table = bgp_node_table (rn);
					            SET_FLAG (bi->flags, BGP_INFO_COUNTED);
					            bi->peer->pcount[table->afi][table->safi]++;
					        }
					    }
                    }
                    else
                    {
                        bgp_info_set_flag (rn, bi, BGP_INFO_VALID);
                        bgp_aggregate_increment (bgp, &rn->p, bi,
                                                 afi, SAFI_UNICAST);
                    }
                }
				else if(! CHECK_FLAG(bi->flags, BGP_INFO_HISTORY) 
						&& CHECK_FLAG(bi->flags, BGP_INFO_VALID) 
						&& CHECK_FLAG(bi->flags, BGP_INFO_SELECTED)
						&& ifindex 
						&& ifindex != bi->ifindex)
				{
					bi->ifindex = ifindex;
					bgp_zebra_announce (&rn->p, bi, bgp, safi,IPC_OPCODE_UPDATE, 0);
				}
				
                if (CHECK_FLAG (bgp->af_flags[afi][SAFI_UNICAST], BGP_CONFIG_DAMPENING) &&  bi->extra && bi->extra->damp_info )
            	{
					if (bgp_damp_scan (bi, afi, SAFI_UNICAST))
                	{
                        bgp_aggregate_increment (bgp, &rn->p, bi, afi, SAFI_UNICAST);
                	}
            	}
            }
        }
        if (rn->info)
        {
            bgp_process (bgp, rn, afi, SAFI_UNICAST);
        }
    }
	
    //if (BGP_DEBUG (events, EVENTS))
    //{
        if (afi == AFI_IP)
        {
            zlog_debug (BGP_DEBUG_TYPE_EVENTS,"scanning IPv4 Unicast routing tables");
        }
        else if (afi == AFI_IP6)
        {
            zlog_debug (BGP_DEBUG_TYPE_EVENTS,"scanning IPv6 Unicast routing tables");
        }
   // }
	
    /* Reevaluate default-originate route-maps and announce/withdraw
     * default route if neccesary. */
    for (ALL_LIST_ELEMENTS (bgp->peer, node, nnode, peer))
    {
        if (peer->status == Established
            && CHECK_FLAG(peer->af_flags[afi][safi], PEER_FLAG_DEFAULT_ORIGINATE)
            && peer->default_rmap[afi][safi].name)
        {
            bgp_default_originate (peer, afi, safi, 0, 0);
        }
    }
}

static void
bgp_scan_vrf (afi_t afi, safi_t safi)
{
	uint16_t vrf_id = 0;
    struct prefix p, d;
    struct bgp_node *rn;
    struct bgp_node *bn;
    struct bgp *bgp;
    struct bgp_info *bi;
    struct bgp_info *next;
	struct bgp_table *table = NULL;
	int update = 0, valid = 0, current = 0;
	
    bgp = bgp_get_default ();
    if (bgp == NULL)
    {
        return;
    }
		
    for (rn = bgp_table_top (bgp->rib[afi][SAFI_MPLS_VPN]); rn; rn = bgp_route_next (rn))
    {
    	if((table = rn->info))
		{
			for (bn = bgp_table_top (table); bn; bn = bgp_route_next (bn))
			{
				update = 0;
		        for (bi = bn->info; bi; bi = next)
		        {
		            next = bi->next;

					if(bi->peer == bgp->peer_self)
						continue;

					if( bi->extra && (!BGP_TAG_IS_EMPTY(bi->extra->tag) || !BGP_TAG_IS_EMPTY(bi->extra->in_tag) ))
						continue;
										
		            if (bi->type == ROUTE_PROTO_EBGP && bi->sub_type == BGP_ROUTE_NORMAL)
		            {
						update = 1;
		                memset (&p, 0, sizeof (struct prefix));
		                p.family = AF_INET;
		                p.prefixlen = IPV4_MAX_BITLEN;

						if(!(bi->vrf_id || bi->vrf_id_to))
							continue;
						
						if(bi->vrf_id)
						{						
		                	p.u.prefix4 = bi->attr->nexthop;
							vrf_id = bi->vrf_id;
						}
						else if(bi->vrf_id_to)
						{
							if(bi->attr && bi->attr->extra && bi->attr->extra->mp_nexthop_global_in.s_addr)
							{							
		                		p.u.prefix4 = bi->attr->extra->mp_nexthop_global_in;
							}
							
							vrf_id = bi->vrf_id_to;
						}

						if(! p.u.prefix4.s_addr )
							continue;
						
						if(bi->peer && bi->peer->su_local.sin.sin_addr.s_addr)
						{
						    memset (&d, 0, sizeof (struct prefix));
						    d.family = AF_INET;
						    d.prefixlen = bi->peer->mask_len;
						    d.u.prefix4 = bi->peer->su_local.sin.sin_addr;
							apply_mask(&d);
						}
						
		                if (bi->peer->sort == BGP_PEER_EBGP && bi->peer->ttl == 1
			                && !CHECK_FLAG(bi->peer->flags, PEER_FLAG_DISABLE_CONNECTED_CHECK)
			                && !CHECK_FLAG(bi->peer->flags, PEER_FLAG_LOOPBACK))
		                {
		                    valid = bgp_nexthop_onlink (afi, bi->attr, vrf_id);
		                }
		                else
		                {
		                    valid = bgp_nexthop_lookup (afi, bi->peer, bi, &bn->p, NULL);
		                }

		                if(bn->p.u.prefix4.s_addr && prefix_match(&bn->p,&p) && d.u.prefix4.s_addr)
		                {    
							if(prefix_match(&d,&bn->p) || (CHECK_FLAG(bi->peer->flags, PEER_FLAG_LOOPBACK) && bn->p.prefixlen == IPV4_MAX_PREFIXLEN))
							{
								valid = 0;
							}
		                }
										
		                current = CHECK_FLAG (bi->flags, BGP_INFO_VALID) ? 1 : 0;
										
		                if (valid != current)
		                {
		                    SET_FLAG (bi->flags, BGP_INFO_ATTR_CHANGED);
		                    if (CHECK_FLAG (bi->flags, BGP_INFO_VALID))
		                    {
		                        bgp_info_unset_flag (bn, bi, BGP_INFO_VALID);
								
							    if (!CHECK_FLAG (bi->flags, BGP_INFO_COUNTED))
							    {
							        struct bgp_table *table_new;
							        if(bn && bgp_node_table (bn))
							        {
							            table_new = bgp_node_table (bn);
							            SET_FLAG (bi->flags, BGP_INFO_COUNTED);
							            bi->peer->pcount[table_new->afi][table_new->safi]++;
							        }
							    }
		                    }
		                    else
		                    {
		                        bgp_info_set_flag (bn, bi, BGP_INFO_VALID);
		                    }
		                }

		            }
	        	}
				
		        if (update)
		        {
		            bgp_process (bgp, bn, afi, SAFI_MPLS_VPN);
		        }
			}
        }

    }
	
    //if (BGP_DEBUG (events, EVENTS))
    //{
        zlog_debug (BGP_DEBUG_TYPE_EVENTS,"scanning %s routing tables", afi_safi_print(afi, safi));
    //}
}

/* BGP scan thread.  This thread check nexthop reachability. */
static int
bgp_scan_timer (void * para)
{
#if 0
    bgp_scan_thread =
        thread_add_timer (bm->master, bgp_scan_timer, NULL, bgp_scan_interval);
#endif
	//high_pre_timer_add("bgp_scan_timer", LIB_TIMER_TYPE_LOOP, bgp_scan_timer, NULL, bgp_scan_interval * 1000);

    //if (BGP_DEBUG (events, EVENTS))
    //{
        zlog_debug (BGP_DEBUG_TYPE_EVENTS,"Performing BGP general scanning");
    //}
	
    bgp_scan (AFI_IP, SAFI_UNICAST);
	
    bgp_scan (AFI_IP6, SAFI_UNICAST);
	
    bgp_scan_vrf (AFI_IP, SAFI_MPLS_VPN);
	
    return 0;
}

/* BGP own address structure */
struct bgp_addr
{
    struct in_addr addr;
    int refcnt;
};

static struct hash *bgp_address_hash;

static void *
bgp_address_hash_alloc (void *p)
{
    struct in_addr *val = p;
    struct bgp_addr *addr;
	
    addr = XMALLOC (MTYPE_BGP_ADDR, sizeof (struct bgp_addr));
    addr->refcnt = 0;
    addr->addr.s_addr = val->s_addr;
	
    return addr;
}

static unsigned int
bgp_address_hash_key_make (void *p)
{
    const struct bgp_addr *addr = p;
	
    return jhash_1word(addr->addr.s_addr, 0);
}

static int
bgp_address_hash_cmp (const void *p1, const void *p2)
{
    const struct bgp_addr *addr1 = p1;
    const struct bgp_addr *addr2 = p2;
	
    return addr1->addr.s_addr == addr2->addr.s_addr;
}

void
bgp_address_init (void)
{
    bgp_address_hash = hash_create (bgp_address_hash_key_make,
                                    bgp_address_hash_cmp);
}

void
bgp_address_destroy (void)
{
    if (bgp_address_hash == NULL)
    {
        return;
    }
	
    hash_clean(bgp_address_hash, NULL);
    hash_free(bgp_address_hash);
    bgp_address_hash = NULL;
}

static void
bgp_address_add (struct prefix *p)
{
    struct bgp_addr tmp;
    struct bgp_addr *addr;
	
    tmp.addr = p->u.prefix4;
    addr = hash_get (bgp_address_hash, &tmp, bgp_address_hash_alloc);
    if (!addr)
    {
        return;
    }
	
    addr->refcnt++;
}

static void
bgp_address_del (struct prefix *p)
{
    struct bgp_addr tmp;
    struct bgp_addr *addr;
	
    tmp.addr = p->u.prefix4;
    addr = hash_lookup (bgp_address_hash, &tmp);
	
    /* may have been deleted earlier by bgp_interface_down() */
    if (addr == NULL)
    {
        return;
    }
	
    addr->refcnt--;
    if (addr->refcnt == 0)
    {
        hash_release (bgp_address_hash, addr);
        XFREE (MTYPE_BGP_ADDR, addr);
    }
}


struct bgp_connected_ref
{
    unsigned int refcnt;
	struct in_addr connect_addr;
};

void
bgp_connected_add (struct connected *ifc)
{
    struct prefix p;
    struct prefix *addr;
    struct interface *ifp;
    struct bgp_node *rn;
    struct bgp_connected_ref *bc;
	
    ifp = ifc->ifp;
    if (! ifp)
    {
        return;
    }
/*	
    if (if_is_loopback (ifp))
    {
        return;
    }
*/	
    addr = ifc->address;
    p = *(CONNECTED_PREFIX(ifc));
	
    if(BGP_DEBUG (events, EVENTS))
    {
		char buf[2][IPV6_ADDR_STRLEN];
				
		memset(&buf[0], 0x0, IPV6_ADDR_STRLEN);
		memset(&buf[1], 0x0, IPV6_ADDR_STRLEN);
		
        prefix2str(addr, buf[0], sizeof(buf[0]));
        prefix2str(&p, buf[1], sizeof(buf[1]));
		
        zlog_debug(BGP_DEBUG_TYPE_OTHER,"%s[%d] vpn %d family %d: addr %s p %s", __FUNCTION__, __LINE__, ifp->vpn, addr->family, buf[0], buf[1]);
    }
	
    if (addr->family == AF_INET)
    {
        apply_mask_ipv4 ((struct prefix_ipv4 *) &p);
		
        if (prefix_ipv4_any ((struct prefix_ipv4 *) &p))
        {
            return;
        }
		
        bgp_address_add (addr);
        rn = bgp_node_get (bgp_connected_table[AFI_IP][ifp->vpn], (struct prefix *) &p);
		if(rn != NULL)
		{
            if (rn->info)
            {
                bc = rn->info;
                bc->refcnt++;
            }
            else
            {
                bc = XCALLOC (MTYPE_BGP_CONN, sizeof (struct bgp_connected_ref));
                bc->refcnt = 1;
                rn->info = bc;
            }
		}	
    }
    else if (addr->family == AF_INET6)
    {
        apply_mask_ipv6 ((struct prefix_ipv6 *) &p);
		
        if (IN6_IS_ADDR_UNSPECIFIED (&p.u.prefix6))
        {
            return;
        }
		
        if (IN6_IS_ADDR_LINKLOCAL (&p.u.prefix6))
        {
            return;
        }
		
        rn = bgp_node_get (bgp_connected_table[AFI_IP6][ifp->vpn], (struct prefix *) &p);
        if (rn->info)
        {
            bc = rn->info;
            bc->refcnt++;
        }
        else
        {
            bc = XCALLOC (MTYPE_BGP_CONN, sizeof (struct bgp_connected_ref));
            bc->refcnt = 1;
            rn->info = bc;
        }
    }
}

void
bgp_connected_delete (struct connected *ifc)
{
    struct prefix p;
    struct prefix *addr;
    struct interface *ifp;
    struct bgp_node *rn;
    struct bgp_connected_ref *bc;
	
    ifp = ifc->ifp;
    if (if_is_loopback (ifp))
    {
        return;
    }
	
    addr = ifc->address;
    p = *(CONNECTED_PREFIX(ifc));
	
    //if(BGP_DEBUG (events, EVENTS))
    //{		
    	char buf[2][PREFIX_STRLEN];
        prefix2str(addr, buf[0], sizeof(buf[0]));
        prefix2str(&p, buf[1], sizeof(buf[1]));
        zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d] vpn %d family %d: p %s addr %s", __FUNCTION__, __LINE__, ifp->vpn, addr->family, buf[0],buf[1]);	
    //}
	
    if (addr->family == AF_INET)
    {
        apply_mask_ipv4 ((struct prefix_ipv4 *) &p);
		
        if (prefix_ipv4_any ((struct prefix_ipv4 *) &p))
        {
            return;
        }
		
        bgp_address_del (addr);
		
        rn = bgp_node_lookup (bgp_connected_table[AFI_IP][ifp->vpn], &p);
        if (! rn)
        {
            return;
        }
		
        bc = rn->info;
        bc->refcnt--;
        if (bc->refcnt == 0)
        {
            XFREE (MTYPE_BGP_CONN, bc);
            rn->info = NULL;
        }
		
        bgp_unlock_node (rn);
        bgp_unlock_node (rn);
    }
    else if (addr->family == AF_INET6)
    {
        apply_mask_ipv6 ((struct prefix_ipv6 *) &p);
		
        if (IN6_IS_ADDR_UNSPECIFIED (&p.u.prefix6))
        {
            return;
        }
		
        if (IN6_IS_ADDR_LINKLOCAL (&p.u.prefix6))
        {
            return;
        }
		
        rn = bgp_node_lookup (bgp_connected_table[AFI_IP6][ifp->vpn], (struct prefix *) &p);
        if (! rn)
        {
            return;
        }
		
        bc = rn->info;
        bc->refcnt--;
        if (bc->refcnt == 0)
        {
            XFREE (MTYPE_BGP_CONN, bc);
            rn->info = NULL;
        }
		
        bgp_unlock_node (rn);
        bgp_unlock_node (rn);
    }
}

int
bgp_nexthop_self (struct attr *attr)
{
    struct bgp_addr tmp, *addr;
	
    tmp.addr = attr->nexthop;
	
    addr = hash_lookup (bgp_address_hash, &tmp);
    if (addr)
    {
        return 1;
    }
	
    return 0;
}

/* Check specified multiaccess next-hop. */
int
bgp_multiaccess_check_v4 (struct in_addr nexthop, char *peer)
{
    struct bgp_node *rn1;
    struct bgp_node *rn2;
    struct prefix p1;
    struct prefix p2;
    struct in_addr addr;
    int ret;
	
    //if(BGP_DEBUG (events, EVENTS))
    //{
        zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d] nexthop.s_addr(0x%x),peer %s",__FUNCTION__,__LINE__,ntohl(nexthop.s_addr),peer);
    //}
	
    ret = inet_aton (peer, &addr);
    if (! ret)
    {
        return 0;
    }
	
    memset (&p1, 0, sizeof (struct prefix));
    p1.family = AF_INET;
    p1.prefixlen = IPV4_MAX_BITLEN;
    p1.u.prefix4 = nexthop;
	
    memset (&p2, 0, sizeof (struct prefix));
    p2.family = AF_INET;
    p2.prefixlen = IPV4_MAX_BITLEN;
    p2.u.prefix4 = addr;
	
    rn1 = bgp_node_match (bgp_connected_table[AFI_IP][0], &p1);
    if (! rn1)
    {
        return 0;
    }
    bgp_unlock_node (rn1);
	
    rn2 = bgp_node_match (bgp_connected_table[AFI_IP][0], &p2);
    if (! rn2)
    {
        return 0;
    }
    bgp_unlock_node (rn2);
	
    /* This is safe, even with above unlocks, since we are just
       comparing pointers to the objects, not the objects themselves. */
    if (rn1 == rn2)
    {
        return 1;
    }
	
    return 0;
}

DEFUN (bgp_scan_time,
       bgp_scan_time_cmd,
       "bgp scan-time <5-60>",
       "BGP specific commands\n"
       "Configure background scanner interval\n"
       "Scanner interval (seconds)\n")
{
    bgp_scan_interval = atoi (argv[0]);
#if 0	
    if (bgp_scan_thread)
    {
        thread_cancel (bgp_scan_thread);
        bgp_scan_thread =
            thread_add_timer (bm->master, bgp_scan_timer, NULL, bgp_scan_interval);
    }
#endif
    if(bgp_scan_timeid)
    { 
        high_pre_timer_delete(bgp_scan_timeid);
		bgp_scan_timeid = 0;
        bgp_scan_timeid = high_pre_timer_add("bgp_scan_timer", LIB_TIMER_TYPE_LOOP, bgp_scan_timer, NULL, bgp_scan_interval * 1000 );    
    }

    return CMD_SUCCESS;
}

DEFUN (no_bgp_scan_time,
       no_bgp_scan_time_cmd,
       "no bgp scan-time",
       NO_STR
       "BGP specific commands\n"
       "Configure background scanner interval\n")
{
    bgp_scan_interval = BGP_SCAN_INTERVAL_DEFAULT;

#if 0
    if (bgp_scan_thread)
    {
        thread_cancel (bgp_scan_thread);
        bgp_scan_thread =
            thread_add_timer (bm->master, bgp_scan_timer, NULL, bgp_scan_interval);
    }
#endif
    if(bgp_scan_timeid)
    {  
        high_pre_timer_delete(bgp_scan_timeid);
		bgp_scan_timeid = 0;
        bgp_scan_timeid = high_pre_timer_add("bgp_scan_timer", LIB_TIMER_TYPE_LOOP, bgp_scan_timer, NULL, bgp_scan_interval* 1000 );    
    }

    return CMD_SUCCESS;
}

void
bgp_scan_init (void)
{		
	u_int16_t vrf_id = 0;
	
    bgp_scan_interval = BGP_SCAN_INTERVAL_DEFAULT;
    bgp_import_interval = BGP_IMPORT_INTERVAL_DEFAULT;
	
	for(vrf_id = 0 ; vrf_id < BGP_VRF_MAX + 1 ; vrf_id ++)
	{
    	bgp_connected_table[AFI_IP][vrf_id] = bgp_table_init (AFI_IP, SAFI_UNICAST);
	}

	for(vrf_id = 0 ; vrf_id < BGP_VRF_MAX + 1 ; vrf_id ++)
	{
    	bgp_connected_table[AFI_IP6][vrf_id] = bgp_table_init (AFI_IP6, SAFI_UNICAST);
	}
	
    /* Make BGP scan thread. */
#if 0	
    bgp_scan_thread = thread_add_timer (bm->master, bgp_scan_timer,
                                        NULL, bgp_scan_interval);
#endif
    bgp_scan_timeid =  high_pre_timer_add("bgp_scan_timer", LIB_TIMER_TYPE_LOOP, bgp_scan_timer, NULL, bgp_scan_interval * 1000);

    /* Make BGP import there. */
    install_element (BGP_NODE, &bgp_scan_time_cmd, CMD_SYNC);
    install_element (BGP_NODE, &no_bgp_scan_time_cmd, CMD_SYNC);
}

void
bgp_scan_finish (void)
{	
	u_int16_t vrf_id = 0;

    THREAD_OFF(bgp_scan_thread);
	
	for(vrf_id = 0 ; vrf_id < BGP_VRF_MAX + 1  ; vrf_id ++)
	{
	    if (bgp_connected_table[AFI_IP][vrf_id])
	    {
	        bgp_table_unlock (bgp_connected_table[AFI_IP][vrf_id]);
	    }
		
	    bgp_connected_table[AFI_IP][vrf_id] = NULL;
	}
	
	for(vrf_id = 0 ; vrf_id < BGP_VRF_MAX + 1  ; vrf_id ++)
	{
	    if (bgp_connected_table[AFI_IP6][vrf_id])
	    {
	        bgp_table_unlock (bgp_connected_table[AFI_IP6][vrf_id]);
	    }
		
	    bgp_connected_table[AFI_IP6][vrf_id] = NULL;
	}
}


int bgp_add_routerId_from_connect_table(struct bgp *bgp)
{
	int vrf_id = 0;
	struct route_table *table = NULL;
	struct route_node * rn = NULL;
	struct bgp_node *bn;
	struct bgp_connected_ref *bc;
	unsigned int buf = 0;
	
	for(vrf_id = 0; vrf_id < BGP_VRF_MAX + 1; vrf_id ++)
	{
		if (bgp_connected_table[AFI_IP][vrf_id])
		{
			table = bgp_connected_table[AFI_IP][vrf_id]->route_table;
			
			for (rn = route_top (table); rn; rn = route_next (rn))
			{
			  if (rn->info == NULL)
				continue;

			  
              bn = bgp_node_from_rnode(rn);
			  bc = bn->info;
			  
			  if(bc->connect_addr.s_addr > buf)
			  	  buf = bc->connect_addr.s_addr;
			}
		}
	}
	
	if(buf == 0)
		return -1;
	else
	{
		bgp->router_id_static.s_addr = buf;
		bgp_router_id_set (bgp, &(bgp->router_id_static));
		return 0;
	}
}

