/* zebra client
   Copyright (C) 1997, 98, 99 Kunihiro Ishiguro

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
along with GNU Zebra; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include <zebra.h>

#include "command.h"
#include "stream.h"
#include "prefix.h"
#include "log.h"
#include "sockunion.h"
#include "routemap.h"
#include "thread.h"
#include "filter.h"
#include "lib/msg_ipc_n.h"


#include "bgpd/bgpd.h"
#include "bgpd/bgp_route.h"
#include "bgpd/bgp_attr.h"
#include "bgpd/bgp_nexthop.h"
#include "bgpd/bgp_zebra.h"
#include "bgpd/bgp_debug.h"
#include "bgpd/bgp_ipc.h"
#include "bgpd/bgp_mplsvpn.h"

/* Inteface addition message from zebra. */
struct interface * bgp_interface_add (int type, struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;

    if(NULL == pevent)
    {
        return NULL;
    }

    ifp = bgp_interface_add_read (type, pevent, vrf_id);
    if(NULL == ifp)
    {
        return NULL;
    }

    //if(BGP_DEBUG(events, EVENTS))
    //{
		char buf[IPV6_ADDR_STRLEN] = "";
		
		if(pevent->ipaddr.type == INET_FAMILY_IPV6)
		{
			inet_ipv6tostr((struct ipv6_addr *)&pevent->ipaddr.addr, buf, IPV6_ADDR_STRLEN);
		}
		else
		{
			inet_ipv4tostr(pevent->ipaddr.addr.ipv4, buf);
		}
		
        zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d],type %d %s address %s/%d ", __FUNCTION__, __LINE__,type,ifp->name,buf,pevent->ipaddr.prefixlen);
    //}

    return ifp;
}

int bgp_interface_delete (struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
	
    if(NULL == pevent)
    {
        return 0;
    }
	
    ifp = bgp_interface_state_read (pevent, vrf_id);
    if(NULL == ifp)
    {
        return 0;
    }
	
    //if(BGP_DEBUG (events, EVENTS))
    //{
        zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d] %s ", __FUNCTION__, __LINE__, ifp->name);
    //}
	
    return 0;
}

int bgp_interface_up (struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
	
    if(NULL == pevent)
    {
        return 0;
    }
	
    ifp = bgp_interface_state_read (pevent, vrf_id);
    if (NULL == ifp)
    {
        return 0;
    }
	
    //if(BGP_DEBUG(events, EVENTS))
    //{
        zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d] %s ", __FUNCTION__, __LINE__, ifp->name);
    //}
	
    return 0;
}

int bgp_interface_down (struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
	
    if(NULL == pevent)
    {
        return 0;
    }
	
    ifp = bgp_interface_state_read (pevent, vrf_id);
    if (NULL == ifp)
    {
        return 0;
    }
	
    //if(BGP_DEBUG(events, EVENTS))
    //{
        zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d] %s ",  __FUNCTION__, __LINE__,ifp->name);
    //}
    
    return 0;
}

int bgp_interface_address_add (struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct connected *ifc;
	
    if(!pevent)
    {
        return 0;
    }
	
    ifc = bgp_interface_address_read (ZEBRA_INTERFACE_ADDRESS_ADD, pevent, vrf_id);
    if(!ifc || !ifc->ifp)
    {
        return 0;
    }

	if(pevent->vpn > 128)
	{
        return 0;
	}
	
	ifc->ifp->vpn = pevent->vpn;
    if(BGP_DEBUG(events, EVENTS))
    {
		char buf[2][IPV6_ADDR_STRLEN];
		
		memset(&buf[0],0,IPV6_ADDR_STRLEN);
		memset(&buf[1],0,IPV6_ADDR_STRLEN);
		
        prefix2str(ifc->address, buf[0], sizeof(buf[0]));
		
		if(pevent->ipaddr.type == INET_FAMILY_IPV6)
		{
			inet_ipv6tostr((struct ipv6_addr *)&pevent->ipaddr.addr, buf[1], IPV6_ADDR_STRLEN);
		}
		else
		{
			inet_ipv4tostr(pevent->ipaddr.addr.ipv4, buf[1]);
		}
		
        zlog_debug(BGP_DEBUG_TYPE_OTHER,"%s[%d] %s vpn %d ifc->address %s prevent->address %s/%d ", __FUNCTION__, __LINE__,ifc->ifp->name, ifc->ifp->vpn, buf[0],buf[1],pevent->ipaddr.prefixlen);
    }
	
    bgp_connected_add (ifc);
	
    return 0;
}

int bgp_interface_address_delete (struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct connected *ifc;
	
    if(! pevent)
    {
        return 0;
    }
	
    if(BGP_DEBUG(events, EVENTS))
    {
		char buf[IPV6_ADDR_STRLEN] = "";
		
		if(pevent->ipaddr.type == INET_FAMILY_IPV6)
		{
			inet_ipv6tostr((struct ipv6_addr *)&pevent->ipaddr.addr, buf, IPV6_ADDR_STRLEN);
		}
		else
		{
			inet_ipv4tostr(pevent->ipaddr.addr.ipv4, buf);
		}
		
        zlog_debug(BGP_DEBUG_TYPE_OTHER,"%s[%d] address %s/%d ", __FUNCTION__, __LINE__, buf, pevent->ipaddr.prefixlen);
    }
	
    ifc = bgp_interface_address_read (ZEBRA_INTERFACE_ADDRESS_DELETE, pevent, vrf_id);
    if (!ifc || !ifc->ifp)
    {
        return 0;
    }
	
    //if(BGP_DEBUG(events, EVENTS))
    //{
		char buf[2][IPV6_ADDR_STRLEN];
		
        prefix2str(ifc->address, buf[0], sizeof(buf[0]));
		
		if(pevent->ipaddr.type == INET_FAMILY_IPV6)
		{
			inet_ipv6tostr((struct ipv6_addr *)&pevent->ipaddr.addr, buf[1], IPV6_ADDR_STRLEN);
		}
		else
		{
			inet_ipv4tostr(pevent->ipaddr.addr.ipv4, buf[1]);
		}
        zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d] %s ifc->address %s pevent->address %s/%d ", __FUNCTION__, __LINE__, ifc->ifp->name, buf[0],buf[1],pevent->ipaddr.prefixlen);
    //}
	
    bgp_connected_delete (ifc);
	
    connected_free (ifc);
	
    return 0;
}

int
bgp_route_read_ipv4 (struct route_entry *p_route, struct ipc_mesg_n *mesg)
{
    uchar plength = 0;
    struct zapi_route api;
    struct prefix p;
	
    memset(&api, 0x0, sizeof(struct zapi_route));
    memset(&p, 0x0, sizeof(struct prefix_ipv4));

    if(!p_route)
	{
		return 0;
	}
		
    if(p_route->prefix.type == INET_FAMILY_IPV6)
	{
	    p.family = AF_INET6;
	    plength = p_route->prefix.prefixlen;
	    p.prefixlen = MIN(IPV6_MAX_PREFIXLEN, plength);
		IPV6_ADDR_COPY(&p.u.prefix6,p_route->prefix.addr.ipv6);		
	}
	else
	{
	    p.family = AF_INET;
	    plength = p_route->prefix.prefixlen;
	    p.prefixlen = MIN(IPV4_MAX_PREFIXLEN, plength);
	    p.u.prefix4.s_addr = htonl(p_route->prefix.addr.ipv4);
	}
	
    api.type = p_route->nhp[0].protocol;
    api.metric = p_route->nhp[0].cost;
    api.ifindex = p_route->nhp[0].ifindex;
    api.distance = p_route->nhp[0].distance;
    api.nhp_type = p_route->nhp[0].nhp_type;
	api.vrf_id = p_route->nhp[0].vpn;
	
    if(ROUTE_PROTO_ISIS == api.type || ROUTE_PROTO_RIP == api.type || ROUTE_PROTO_OSPF == api.type
		||ROUTE_PROTO_ISIS6 == api.type || ROUTE_PROTO_RIPNG == api.type || ROUTE_PROTO_OSPF6 == api.type)
    {
        api.instance = p_route->nhp[0].instance;
    }
    else
    {
        api.instance = 0;
    }
	
    if(p_route->prefix.type == INET_FAMILY_IPV6)
	{
		IPV6_ADDR_COPY(&api.nexthop_v6, p_route->nhp[0].nexthop.addr.ipv6);		
	}
	else
	{
    	api.nexthop.s_addr = htonl(p_route->nhp[0].nexthop.addr.ipv4);
	}
	
    //if(BGP_DEBUG (events, EVENTS))
    //{
        char buf[2][IPV6_ADDR_STRLEN];
		
        prefix2str(&p, buf[0], IPV6_ADDR_STRLEN);
        zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d] : %s route %s %s vpn %d  %s nexthop %s ifindex 0x%x metric %u instance %d ",
                   __FUNCTION__,__LINE__,
                   (p.family == AF_INET) ? "IPv4" : "IPv6",
                   (mesg->msghdr.opcode  == IPC_OPCODE_ADD )? "add" : "delete",
                   zebra_route_string(api.type), api.vrf_id, buf[0],
                   (p.family == AF_INET6) ? inet_ntop(AF_INET6, &api.nexthop_v6, buf[1], sizeof(buf[1])):inet_ntop(AF_INET, &api.nexthop, buf[1], sizeof(buf[1])),
                   api.ifindex ,api.metric,api.instance);
    //}
        	
    if (mesg->msghdr.opcode == IPC_OPCODE_ADD)
    {
		bgp_route_add(&p, &api);
    }
    else
    {
        bgp_route_delete(&p, &api);	  
    }
		
    return 0;
}

struct interface *
if_lookup_by_ipv4 (struct in_addr *addr)
{
    struct listnode *ifnode;
    struct listnode *cnode;
    struct interface *ifp;
    struct connected *connected;
    struct prefix_ipv4 p;
    struct prefix *cp;

    p.family = AF_INET;
    p.prefix = *addr;
    p.prefixlen = IPV4_MAX_BITLEN;

    for (ALL_LIST_ELEMENTS_RO (iflist, ifnode, ifp))
    {
        for (ALL_LIST_ELEMENTS_RO (ifp->connected, cnode, connected))
        {
            cp = connected->address;
            if (cp->family == AF_INET)
        	{
                if (prefix_match (cp, (struct prefix *)&p))
                {
                    return ifp;
                }
        	}
        }
    }
    return NULL;
}

struct interface *
if_lookup_by_ipv4_exact (struct in_addr *addr)
{
    struct listnode *ifnode;
    struct listnode *cnode;
    struct interface *ifp;
    struct connected *connected;
    struct prefix *cp;

    for (ALL_LIST_ELEMENTS_RO (iflist, ifnode, ifp))
    {
        for (ALL_LIST_ELEMENTS_RO (ifp->connected, cnode, connected))
        {
            cp = connected->address;
            if (cp->family == AF_INET)
        	{
                if (IPV4_ADDR_SAME (&cp->u.prefix4, addr))
                {
                    return ifp;
                }
        	}
        }
    }
    return NULL;
}

struct interface *
if_lookup_by_ipv6 (struct in6_addr *addr)
{
    struct listnode *ifnode;
    struct listnode *cnode;
    struct interface *ifp;
    struct connected *connected;
    struct prefix_ipv6 p;
    struct prefix *cp;

    p.family = AF_INET6;
    p.prefix = *addr;
    p.prefixlen = IPV6_MAX_BITLEN;

    for (ALL_LIST_ELEMENTS_RO (iflist, ifnode, ifp))
    {
        for (ALL_LIST_ELEMENTS_RO (ifp->connected, cnode, connected))
        {
            cp = connected->address;
            if (cp->family == AF_INET6)
        	{
                if (prefix_match (cp, (struct prefix *)&p))
                {
                    return ifp;
                }
        	}
        }
    }
    return NULL;
}

struct interface *
if_lookup_by_ipv6_exact (struct in6_addr *addr)
{
    struct listnode *ifnode;
    struct listnode *cnode;
    struct interface *ifp;
    struct connected *connected;
    struct prefix *cp;

    for (ALL_LIST_ELEMENTS_RO (iflist, ifnode, ifp))
    {
        for (ALL_LIST_ELEMENTS_RO (ifp->connected, cnode, connected))
        {
            cp = connected->address;
            if (cp->family == AF_INET6)
        	{
                if (IPV6_ADDR_SAME (&cp->u.prefix6, addr))
                {
                    return ifp;
                }
        	}
        }
    }
    return NULL;
}

static int
if_get_ipv6_global (struct interface *ifp, struct in6_addr *addr)
{
    struct listnode *cnode;
    struct connected *connected;
    struct prefix *cp;
	
    for (ALL_LIST_ELEMENTS_RO (ifp->connected, cnode, connected))
    {
        cp = connected->address;
        if (cp->family == AF_INET6)
    	{
            if (! IN6_IS_ADDR_LINKLOCAL (&cp->u.prefix6))
            {
                memcpy (addr, &cp->u.prefix6, IPV6_MAX_BYTELEN);
                return 1;
            }
    	}
    }
    return 0;
}

static int
if_get_ipv6_local (struct interface *ifp, struct in6_addr *addr)
{
    struct listnode *cnode;
    struct connected *connected;
    struct prefix *cp;
	
    for (ALL_LIST_ELEMENTS_RO (ifp->connected, cnode, connected))
    {
        cp = connected->address;
        if (cp->family == AF_INET6) 
    	{
            if (IN6_IS_ADDR_LINKLOCAL (&cp->u.prefix6))
            {
                memcpy (addr, &cp->u.prefix6, IPV6_MAX_BYTELEN);
                return 1;
            }
    	}
    }
    return 0;
}

static int
if_get_ipv4_address (struct interface *ifp, struct in_addr *addr)
{
    struct listnode *cnode;
    struct connected *connected;
    struct prefix *cp;
	
    for (ALL_LIST_ELEMENTS_RO (ifp->connected, cnode, connected))
    {
        cp = connected->address;
        if ((cp->family == AF_INET) && !ipv4_martian(&(cp->u.prefix4)))
        {
            *addr = cp->u.prefix4;
            return 1;
        }
    }
    return 0;
}

int
bgp_nexthop_set (union sockunion *local, union sockunion *remote,
                 struct bgp_nexthop *nexthop, struct peer *peer)
{
    int ret = 0;
    struct interface *ifp = NULL;
	
    memset (nexthop, 0, sizeof (struct bgp_nexthop));
	
    if (!local)
    {
        return -1;
    }
	
    if (!remote)
    {
        return -1;
    }
	
    if (local->sa.sa_family == AF_INET)
    {
        nexthop->v4 = local->sin.sin_addr;
        ifp = if_lookup_by_ipv4 (&local->sin.sin_addr);
    }
	
    if (local->sa.sa_family == AF_INET6)
    {
      	memcpy (&nexthop->v6_global, &local->sin6.sin6_addr, IPV6_MAX_BYTELEN);
		memcpy (&nexthop->v6_local, &local->sin6.sin6_addr, IPV6_MAX_BYTELEN);
        if (IN6_IS_ADDR_LINKLOCAL (&local->sin6.sin6_addr))
        {
            if (peer->ifname)
            {
                ifp = if_lookup_by_name (peer->ifname);
            }
        }
        else
        {
            ifp = if_lookup_by_ipv6 (&local->sin6.sin6_addr);
        }
    }
	
    if (!ifp)
    {
        return -1;
    }
	
    nexthop->ifp = ifp;
	
    /* IPv4 connection. */
    if (local->sa.sa_family == AF_INET)
    {
        /* IPv6 nexthop*/
        ret = if_get_ipv6_global (ifp, &nexthop->v6_global);
		
        /* There is no global nexthop. */
        if (!ret)
        {
            if_get_ipv6_local (ifp, &nexthop->v6_global);
        }
        else
        {
            if_get_ipv6_local (ifp, &nexthop->v6_local);
        }
    }
	
    /* IPv6 connection. */
    if (local->sa.sa_family == AF_INET6)
    {
        struct interface *direct = NULL;
		
        /* IPv4 nexthop. */
        ret = if_get_ipv4_address(ifp, &nexthop->v4);
        if (!ret && peer->local_id.s_addr)
        {
            nexthop->v4 = peer->local_id;
        }
		
        /* Global address*/
        if (! IN6_IS_ADDR_LINKLOCAL (&local->sin6.sin6_addr))
        {
            memcpy (&nexthop->v6_global, &local->sin6.sin6_addr,IPV6_MAX_BYTELEN);
			
            /* If directory connected set link-local address. */
            direct = if_lookup_by_ipv6 (&remote->sin6.sin6_addr);
            if (direct)
            {
                if_get_ipv6_local (ifp, &nexthop->v6_local);
            }
        }
        else /* Link-local address. */
        {
            ret = if_get_ipv6_global (ifp, &nexthop->v6_global);
			
            /* If there is no global address.  Set link-local address as
                   global.  I know this break RFC specification... */
            if (!ret)
                memcpy (&nexthop->v6_global, &local->sin6.sin6_addr,
                        IPV6_MAX_BYTELEN);
            else
                memcpy (&nexthop->v6_local, &local->sin6.sin6_addr,
                        IPV6_MAX_BYTELEN);
        }
    }
	
    if (IN6_IS_ADDR_LINKLOCAL (&local->sin6.sin6_addr) || if_lookup_by_ipv6 (&remote->sin6.sin6_addr))
    {
        peer->shared_network = 1;
    }
    else
    {
        peer->shared_network = 0;
    }
	
    /* KAME stack specific treatment.  */
#ifdef KAME
    if (IN6_IS_ADDR_LINKLOCAL (&nexthop->v6_global)
            && IN6_LINKLOCAL_IFINDEX (nexthop->v6_global))
    {
        SET_IN6_LINKLOCAL_IFINDEX (nexthop->v6_global, 0);
    }
    if (IN6_IS_ADDR_LINKLOCAL (&nexthop->v6_local)
            && IN6_LINKLOCAL_IFINDEX (nexthop->v6_local))
    {
        SET_IN6_LINKLOCAL_IFINDEX (nexthop->v6_local, 0);
    }
#endif /* KAME */
    return ret;
}

void
bgp_zebra_announce (struct prefix *p, struct bgp_info *info, struct bgp *bgp, safi_t safi,enum IPC_OPCODE opcode, int mpls_flag)
{
	int flag = 0;
    struct peer *peer;
	struct zapi_route api;

    peer = info->peer;

	if(p->family == AF_INET6 && !(info->attr && info->attr->extra))
	{
		return;
	}
	
	memset(&api,0x0,sizeof(struct zapi_route));

    if (peer->sort == BGP_PEER_IBGP || peer->sort == BGP_PEER_CONFED)
    {
        SET_FLAG (api.flags, ZEBRA_FLAG_IBGP);
    }
	
    if(BGP_ROUTE_AGGREGATE == info->sub_type)
    {
        SET_FLAG (api.flags, ZEBRA_FLAG_IBGP);
        SET_FLAG (api.flags, ZEBRA_FLAG_BLACKHOLE);
    }
	
    if(BGP_ROUTE_REDISTRIBUTE == info->sub_type || BGP_ROUTE_STATIC == info->sub_type )
    {
        flag = 1;
    }
		
    api.safi = safi;
    api.metric = info->attr->med;
    api.distance = bgp_distance_apply (p, info, bgp);

	if(p->family == AF_INET)
	{
		api.nexthop = info->attr->nexthop;
		
		if(api.nexthop.s_addr == 0)
		{
			if(info->attr && info->attr->extra)
			{
				api.nexthop = info->attr->extra->mp_nexthop_global_in;
			}
		}
	}
	else if(p->family == AF_INET6)
	{
		api.nexthop_v6 = info->attr->extra->mp_nexthop_global;
	}
	
	if(BGP_ROUTE_AGGREGATE != info->sub_type)
	{
		api.ifindex =  info->ifindex;
	}

	if(info->extra)
	{
		api.inlabel = decode_label(info->extra->in_tag);
		api.outlabel = decode_label(info->extra->tag);
	}

	if(api.outlabel)
	{
		api.vrf_id = info->vrf_id_to;
	}
	else
	{
		api.vrf_id = info->vrf_id;
	}

	if(peer->ifindex_tnl)
	{
		api.ifindex_tnl = peer->ifindex_tnl;
	}

    //if(BGP_DEBUG (events, EVENTS))
    //{
		char buf[2][INET_ADDRSTRLEN];
		
		if(p->family == AF_INET)
		{
	        zlog_debug(BGP_DEBUG_TYPE_EVENTS, "%s[%d] mpls_flag %d flag %d inlabel %d outlabel %d  tnl_ifindex 0x%x vrf_id %d vrf_id_to %d ",
	                   __func__,__LINE__,mpls_flag,flag ,api.inlabel,api.outlabel,api.ifindex_tnl,api.vrf_id,info->vrf_id_to);
		}
		
        zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d] %s route %s %s/%d  nexthop %s metric %u distance %d ",
           __func__,__LINE__,(p->family == AF_INET)?"IPv4":"IPv6",(IPC_OPCODE_ADD == opcode)?"add":((IPC_OPCODE_UPDATE == opcode)?"update":"delete"),				   
			inet_ntop(p->family, &p->u.prefix, buf[0], SU_ADDRSTRLEN),
			p->prefixlen,
		   (p->family == AF_INET)?(inet_ntop(AF_INET, &api.nexthop, buf[1], sizeof(buf[1]))):(inet_ntop(AF_INET6, &api.nexthop_v6, buf[1], sizeof(buf[1]))),
		   api.metric,api.distance);
    //}

	if(mpls_flag)
	{
		bgp_zapi_ipv4_route_vrf (opcode, (struct prefix_ipv4 *) p, &api,flag);
	}
	else
	{
    	bgp_zapi_route (opcode, p, &api);
	}
}

/* Other routes redistribution into BGP. */
int
bgp_redistribute_set (struct bgp *bgp, afi_t afi, int type, uint8_t instance , u_int32_t metric)
{
    int redis_type = 0;
	
    /* Set flag to BGP instance. */
    if(0 == instance)
    {
        bgp->redist[afi][type] = 1;
    }
    else
    {
        redis_type = type - ROUTE_PROTO_OSPF;
        bgp->redist_instance[redis_type][instance].redist_instance_flag = 1;
        if(metric)
        {
            bgp->redist_instance[redis_type][instance].redist_instance_metric_flag = 1;
            bgp->redist_instance[redis_type][instance].redist_instance_metric = metric;
        }
    }
	
    bgp_redistribute_route_add_type(bgp, afi, type, instance, 0);
	
    return CMD_SUCCESS;
}

/* Redistribute with route-map specification.  */
int
bgp_redistribute_rmap_set (struct bgp *bgp, afi_t afi, int type,
                           const char *name)
{
    if (bgp->rmap[afi][type].name && (strcmp (bgp->rmap[afi][type].name, name) == 0))
    {
        return 0;
    }
	
    if (bgp->rmap[afi][type].name)
    {
        free (bgp->rmap[afi][type].name);
    }
	
    bgp->rmap[afi][type].name = strdup (name);
    bgp->rmap[afi][type].map = route_map_lookup_by_name (name);
	
    return 1;
}

/* Redistribute with metric specification.  */
int
bgp_redistribute_metric_set (struct bgp *bgp, afi_t afi, int type,
                             u_int32_t metric)
{
    if (bgp->redist_metric_flag[afi][type] && bgp->redist_metric[afi][type] == metric)
    {
        return 0;
    }
	
    bgp->redist_metric_flag[afi][type] = 1;
    bgp->redist_metric[afi][type] = metric;
	
    return 1;
}

/* Unset redistribution.  */
int
bgp_redistribute_unset (struct bgp *bgp, afi_t afi, int type,uint8_t instance)
{
    int type_redis = 0;
	
    if(instance == 0)
    {
        /* Unset flag from BGP instance. */
        bgp->redist[afi][type] = 0;
		
        /* Unset route-map. */
        if (bgp->rmap[afi][type].name)
        {
            free (bgp->rmap[afi][type].name);
        }
		
        bgp->rmap[afi][type].name = NULL;
        bgp->rmap[afi][type].map = NULL;
		
        /* Unset metric. */
        bgp->redist_metric_flag[afi][type] = 0;
        bgp->redist_metric[afi][type] = 0;
    }
    else
    {
        type_redis = type - ROUTE_PROTO_OSPF;
        bgp->redist_instance[type_redis][instance].redist_instance_flag = 0;
        bgp->redist_instance[type_redis][instance].redist_instance_metric = 0;
        bgp->redist_instance[type_redis][instance].redist_instance_metric_flag = 0;
    }
	
    /* Withdraw redistributed routes from current BGP's routing table. */
    bgp_redistribute_route_delete_type (bgp, afi, type, instance, 0);
    return CMD_SUCCESS;
}

/* Unset redistribution route-map configuration.  */
int
bgp_redistribute_routemap_unset (struct bgp *bgp, afi_t afi, int type)
{
    if (! bgp->rmap[afi][type].name)
    {
        return 0;
    }
	
    /* Unset route-map. */
    free (bgp->rmap[afi][type].name);
    bgp->rmap[afi][type].name = NULL;
    bgp->rmap[afi][type].map = NULL;
	
    return 1;
}

/* Unset redistribution metric configuration.  */
int
bgp_redistribute_metric_unset (struct bgp *bgp, afi_t afi, int type)
{
    if (! bgp->redist_metric_flag[afi][type])
    {
        return 0;
    }
	
    /* Unset metric. */
    bgp->redist_metric_flag[afi][type] = 0;
    bgp->redist_metric[afi][type] = 0;
	
    return 1;
}

