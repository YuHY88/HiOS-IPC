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
#include "vty.h"
#include "command.h"
#include "prefix.h"
#include "stream.h"
#include "memory.h"
#include "vrf.h"
#include "ospf6_ipc.h"

#include "ospf6_proto.h"
#include "ospf6_top.h"
#include "ospf6_interface.h"
#include "ospf6_route.h"
#include "ospf6_lsa.h"
#include "ospf6_lsdb.h"
#include "ospf6_asbr.h"
#include "ospf6_zebra.h"
#include "ospf6d.h"
#include "ospf6_ipc.h"
#include "ospf6_area.h"
#include "ospf6_intra.h"

unsigned char conf_debug_ospf6_zebra = 0;

struct in_addr router_id_zebra;

#if 0
/* Router-id update message from zebra. */
static int
ospf6_router_id_update_zebra(int command, struct zclient *zclient,
                             zebra_size_t length, vrf_id_t vrf_id)
{
    struct prefix router_id;
    struct ospf6 *o = ospf6;
    zebra_router_id_update_read(zclient->ibuf, &router_id);
    router_id_zebra = router_id.u.prefix4;

    if (o == NULL)
    {
        return 0;
    }

    if (o->router_id  == 0)
    {
        o->router_id = (u_int32_t) router_id_zebra.s_addr;
    }

    return 0;
}
#endif


static int
memconstant(const void *s, int c, size_t n)
{
    const u_char *p = s;

    while (n-- > 0)
        if (*p++ != c)
            return 0;

    return 1;
}


/* Get prefix in ZServ format; family should be filled in on prefix */
static void
ospf6_get_prefix(struct ifm_event pevent, struct prefix *p)
{
    size_t plen = prefix_blen(p);
    u_char c;
    p->prefixlen = 0;

    if (plen == 0)
    {
        return;
    }

    IPV6_ADDR_COPY(&p->u.prefix6, pevent.ipaddr.addr.ipv6);
    c = pevent.ipaddr.prefixlen;
    p->prefixlen = MIN(plen * 8, c);
}

struct interface *
ospf6_interface_add_read(uint32_t ifindex, vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;
    char ifname_tmp[INTERFACE_NAMSIZ];

    /* Read interface name. */
    ifm_get_name_by_ifindex(ifindex, ifname_tmp);

    /* Lookup/create interface by name. */
    ifp = if_get_by_name_len_vrf(ifname_tmp,
                                 strnlen(ifname_tmp, INTERFACE_NAMSIZ),
                                 vrf_id);

    return ifp;
}

static void
ospf6_interface_if_set_value(struct ifm_event pevent, struct interface *ifp)
{
    uint16_t mtu = 0;
    uchar  pmac[6];
    /* Read interface's index. */
    ifp->ifindex = pevent.ifindex;
    int type = IFM_TYPE_ID_GET(pevent.ifindex);
    /*set link type */
    ifp->ll_type = type;

    /*set link type and metric*/
    if (type == IFNET_TYPE_LOOPBACK)
    {
        ifp->metric = 0;
        SET_FLAG(ifp->flags, IFF_LOOPBACK);
        ifp->bandwidth = 0;
    }
    else
    {
        ifp->metric = 1;
        ifp->bandwidth = 100000;
        SET_FLAG(ifp->flags, IFF_BROADCAST);
    }
    
    /* set ifp link*/
    if (pevent.up_flag == IFNET_LINKUP)
    {
        ifp->status = ZEBRA_INTERFACE_ACTIVE;
        SET_FLAG(ifp->flags, IFF_UP);
        SET_FLAG(ifp->flags, IFF_RUNNING);

    }
    else
    {
        ifp->status = ZEBRA_INTERFACE_SUB;
        UNSET_FLAG(ifp->flags, IFF_UP);
        UNSET_FLAG(ifp->flags, IFF_RUNNING);
    }

    /* get the mac */
	if(ifm_get_mac(pevent.ifindex, MODULE_ID_OSPF6, pmac) == 0)
    {
        ifp->hw_addr_len = 6;
        if (ifp->hw_addr_len)
        {
            ifp->hw_addr[0] = pmac[0];
            ifp->hw_addr[1] = pmac[1];
            ifp->hw_addr[2] = pmac[2];
            ifp->hw_addr[3] = pmac[3];
            ifp->hw_addr[4] = pmac[4];
            ifp->hw_addr[5] = pmac[5];
        }
    }
    else
    {
        zlog_warn("%-15s[%d]: OSPF get ifindex %0x mac error", __func__, __LINE__, pevent.ifindex);
    }

    /* get mtu */
    if (ifm_get_mtu(pevent.ifindex, MODULE_ID_OSPF6, &mtu) == 0)
    {
        ifp->mtu = mtu;
        ifp->mtu6 = mtu;
    }
    else
    {
        zlog_warn("%-15s[%d]: OSPF get ifindex %0x mtu error", __func__, __LINE__, pevent.ifindex);
        ifp->mtu = 1500;
        ifp->mtu6 = 1500;
    }
}

static struct interface *
ospf6_interface_if_lookup(struct ifm_event pevent, vrf_id_t vrf_id)
{
    char ifname_tmp[INTERFACE_NAMSIZ];

    /* Read interface name. */
    ifm_get_name_by_ifindex(pevent.ifindex, ifname_tmp);

    /* And look it up. */
    return if_lookup_by_name_len(ifname_tmp,
                                 strnlen(ifname_tmp, INTERFACE_NAMSIZ));
}


/* connect address add/delete from ifp */
static struct connected *
ospf6_interface_address_read(int type, struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;
    struct connected *ifc = NULL;
    struct prefix p, d, *dp = NULL;
    //struct ospf6_interface *oi = NULL;
    int plen;
    //int mode = 0;

    memset(&p, 0, sizeof(p));
    memset(&d, 0, sizeof(d));

    if (type == ZEBRA_INTERFACE_ADDRESS_ADD)
    {
        ifp = ospf6_if_add(pevent, vrf_id);
        ospf6_interface_if_set_value(pevent, ifp);
#if 0
        oi = (struct ospf6_interface *) ifp->info;

        if (oi != NULL)
        {
            oi->type = ospf6_default_iftype(ifp);
            /* get the mode of the interface*/
            mode = ospf6_get_intf_mode(pevent.ifindex);
            if( mode >= 0)
            {
                oi->mode = mode;
            }
			oi->ifmtu = ifp->mtu6;
        }
#endif
    }
    else
    {
        ifp = ospf6_interface_if_lookup(pevent, vrf_id);
    }

    if (ifp == NULL)
    {
        zlog_warn("%s %d (%s): "
                  "Can't find interface by ifindex: %0x ",__func__,__LINE__,
                  (type == ZEBRA_INTERFACE_ADDRESS_ADD ? "ADD" : "DELETE"),
                  pevent.ifindex);
        return NULL;
    }


    /* Fetch interface address. */
    if (pevent.ipaddr.type == INET_FAMILY_IPV6)
    {
        d.family = p.family = AF_INET6;
    }

    plen = prefix_blen(&d);

    ospf6_get_prefix(pevent, &p);

    /* N.B. NULL destination pointers are encoded as all zeroes */
    dp = memconstant(&d.u.prefix, 0, plen) ? NULL : &d;

    if (type == ZEBRA_INTERFACE_ADDRESS_ADD)
    {
        /* N.B. NULL destination pointers are encoded as all zeroes */
        ifc = connected_add_by_prefix(ifp, &p, dp);

        if (ifc != NULL)
        {
            /* slave ip address could not config ospf */

            /* Fetch flag. */
            if(pevent.ipflag == IP_TYPE_SLAVE)
            {
                SET_FLAG(ifc->flags, ZEBRA_IFA_SLAVE);
            }
            else if (pevent.ipflag == IP_TYPE_STATIC)
            {
                SET_FLAG(ifc->flags, ZEBRA_IFA_SECONDARY);
            }
            else if (pevent.ipflag == IP_TYPE_DHCP)
            {
                SET_FLAG(ifc->flags, ZEBRA_IFA_DHCP);
            }
            else if (pevent.ipflag == IP_TYPE_UNNUMBERED)
            {
                SET_FLAG(ifc->flags, ZEBRA_IFA_UNNUMBERED);
            }
			else if(pevent.ipflag == IP_TYPE_LINK_LOCAL)
			{
				SET_FLAG(ifc->flags, ZEBRA_IFA_LINK);
			}
            else
            {
                zlog_warn("warnig: ip flag error");
            }

            if (ifc->destination)
            {
                ifc->destination->prefixlen = ifc->address->prefixlen;
            }
            else if (CHECK_FLAG(ifc->flags, ZEBRA_IFA_PEER))
            {
                /* carp interfaces on OpenBSD with 0.0.0.0/0 as "peer" */
                char buf[PREFIX_STRLEN];
                zlog_warn("warning: interface %s address %s "
                          "with peer flag set, but no peer address!",
                          ifp->name, prefix2str(ifc->address, buf, sizeof buf));
                UNSET_FLAG(ifc->flags, ZEBRA_IFA_PEER);
            }
        }
    }
    else
    {
        assert(type == ZEBRA_INTERFACE_ADDRESS_DELETE);
        ifc = connected_delete_by_prefix(ifp, &p);
    }

    return ifc;
}

void
ospf6_add_ipv6_locallink_addr(struct ifm_event pevent)
{
    u_char  pmac[6] = {0,0,0,0,0,0};

    ifm_get_mac(pevent.ifindex, MODULE_ID_OSPF6, pmac);

    pevent.ipaddr.type = INET_FAMILY_IPV6;
    pevent.ipaddr.addr.ipv6[0] = 0xfe;
    pevent.ipaddr.addr.ipv6[1] = 0x80;
    pevent.ipaddr.addr.ipv6[2] = 0x00;
    pevent.ipaddr.addr.ipv6[3] = 0x00;
    pevent.ipaddr.addr.ipv6[4] = 0x00;
    pevent.ipaddr.addr.ipv6[5] = 0x00;
    pevent.ipaddr.addr.ipv6[6] = 0x00;
    pevent.ipaddr.addr.ipv6[7] = 0x00;
    pevent.ipaddr.addr.ipv6[8] = (*pmac ^ 0x2);
    pevent.ipaddr.addr.ipv6[9] = *(pmac + 1);
    pevent.ipaddr.addr.ipv6[10] = *(pmac + 2);
    pevent.ipaddr.addr.ipv6[11] = 0xff;
    pevent.ipaddr.addr.ipv6[12] = 0xfe;
    pevent.ipaddr.addr.ipv6[13] = *(pmac + 3);
    pevent.ipaddr.addr.ipv6[14] = *(pmac + 4);
    pevent.ipaddr.addr.ipv6[15] = *(pmac + 5);
    pevent.ipaddr.prefixlen = 64;

    if (pevent.up_flag == IFNET_LINKUP)
    {
        ospf6_zebra_if_address_update_add(pevent, VRF_DEFAULT);
    }
    else
    {
        ospf6_zebra_if_address_update_delete(pevent, VRF_DEFAULT);
    }
}



/* redistribute function */
void
ospf6_zebra_redistribute(struct ospf6 *ospf6, int source, int instance, int type ,int metric)
{
	struct listnode *lnode = NULL, *lnnode = NULL;
    struct ospf6_area *oa = NULL;
    struct ospf6_route *route = NULL;
	struct ospf6_external_info *info = NULL;
	struct ospf6_redist *redist = NULL;
	zlog_warn("%s %d \n",__func__,__LINE__);

    if ((redist = ospf6_redist_lookup(ospf6, source, instance)) != NULL)
    {
    	if((redist->type == type) && (redist->metric == metric))
    	{
			zlog_warn("%s %d \n",__func__,__LINE__);
        	return;
    	}
		else
		{
		
			redist->type = type;
			redist->metric = metric;
			zlog_warn("%s %d metric:%d\n",__func__,__LINE__,redist->metric);
			/* refresh the lsa */
			for (route = ospf6_route_head(om6->external6_table); route;
			route = ospf6_route_next(route))
			{			
			    info = route->route_option;
				if((info->type == source) && (info->instance == instance))
				{	
					#if 0
					printf("%s %d %d\n",__func__,__LINE__,route->path.origin.id);
					route->path.metric_type = redist->type;
					route->path.cost = redist->metric;
					struct ospf6_lsa *old = NULL;
					old = ospf6_lsdb_lookup(htons(OSPF6_LSTYPE_AS_EXTERNAL), route->path.origin.id,
					     ospf6->router_id, ospf6->lsdb_self);
					if(old != NULL)
					{
						//assert(old->ospf6);
						
						printf("%s %d ---------%d\n",__func__,__LINE__,old->header->id);
						old->refresh = thread_add_event(master_ospf6, ospf6_lsa_refresh, old, 0);
					}
					else
					{
					
						printf("%s %d --------\n",__func__,__LINE__);
						ospf6_as_external_lsa_originate(ospf6, route);
					}
					#endif
						
					route->path.metric_type = redist->type;
					route->path.cost = redist->metric;
					ospf6_as_external_lsa_originate(ospf6, route);
					
				}
			}
		}
		return;
    }
	zlog_warn("%s %d \n",__func__,__LINE__);

    ospf6_redist_create(ospf6, source, instance, type, metric);
    om6->redis_count[source] ++;
	ospf6->redistribute ++;

	/* Router-Bit (ASBR Flag) may have to be updated */
	for (ALL_LIST_ELEMENTS(ospf6->area_list, lnode, lnnode, oa))
	{
		OSPF6_ROUTER_LSA_SCHEDULE(oa);
	}
	
	if (om6->redis_count[source] == 1)
	{
    	ospf6_redistribute_send(ZEBRA_REDISTRIBUTE_ADD, ospf6, source);
	}
	else
	{
		for (route = ospf6_route_head(om6->external6_table); route;
	    route = ospf6_route_next(route))
	    {
	    	
	        info = route->route_option;
			if((info->type == source) && (info->instance == instance))
			{
				info->id[ospf6->ospf_id -1] = ospf6->external_id++;
				route->path.origin.id = htonl (info->id[ospf6->ospf_id -1]);		
				route->path.metric_type = redist->type;
				route->path.cost = redist->metric;
				ospf6_as_external_lsa_originate(ospf6, route);
			}
		}
	}


}

void
ospf6_zebra_no_redistribute(struct ospf6* ospf6, int source, int instance)
{
    struct ospf6_redist *redist = NULL;

    if ((redist = ospf6_redist_lookup(ospf6, source, instance)) == NULL)
    {
        return;
    }

    ospf6_redist_delete(ospf6, redist);
	redist = NULL;
    om6->redis_count[source] --;
	ospf6->redistribute --;
    ospf6_redistribute_send(ZEBRA_REDISTRIBUTE_DELETE, ospf6, source);
}

int
ospf6_interface_mtu_change (struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;
    struct ospf6_interface *oi = NULL;
	
    ifp = ospf6_interface_if_lookup (pevent, vrf_id);
    if(ifp != NULL)
    {	

        ifp->mtu = pevent.mtu;
        ifp->mtu6 = pevent.mtu;
        oi = (struct ospf6_interface *) ifp->info;

        if (oi == NULL)
        {
            return 0;
        }
		oi->ifmtu = ifp->mtu6;

    }
    return 0;
}


/* Inteface addition message from zebra. */
struct interface *
ospf6_if_add(struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
    ifp = ospf6_interface_add_read(pevent.ifindex, vrf_id);  /////

    if (IS_OSPF6_DEBUG_ZEBRA(RECV))
        zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%s %d: %s index %0x mtu %d",__func__,__LINE__,
                   ifp->name, ifp->ifindex, ifp->mtu6);
	ifp->ifindex = pevent.ifindex;

    ospf6_interface_if_add(ifp);
    return ifp;
}

int
ospf6_zebra_if_del(struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;

    if (!(ifp = ospf6_interface_if_lookup(pevent, vrf_id)))
    {
        return 0;
    }

    if (if_is_up(ifp))
    {
        zlog_warn("OSPF6: got delete of %s, but interface is still up", ifp->name);
    }

    if (IS_OSPF6_DEBUG_ZEBRA(RECV))
        zlog_debug(OSPF6_DBG_ZEBRA_MSG, "OSPF6 Interface delete: %s index %0x mtu %d",
                   ifp->name, ifp->ifindex, ifp->mtu6);

#if 1
    /* XXX: ospf6_interface_if_del is not the right way to handle this,
     * because among other thinkable issues, it will also clear all
     * settings as they are contained in the struct ospf6_interface. */
    ospf6_interface_if_del(ifp);
#endif /*0*/
	
	if_delete(ifp);

    //ifp->ifindex = IFINDEX_INTERNAL;

    return 0;
}

int
ospf6_zebra_if_state_update(struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
    uint16_t mtu = 0;
    ifp = ospf6_interface_if_lookup(pevent, vrf_id);

    if (ifp == NULL)
    {
        return 0;
    }

    /* set ifp link*/
    if (pevent.up_flag == IFNET_LINKUP)
    {
        ifp->status = ZEBRA_INTERFACE_ACTIVE;
        SET_FLAG(ifp->flags, IFF_UP);
        SET_FLAG(ifp->flags, IFF_RUNNING);
    }
    else
    {
        ifp->status = ZEBRA_INTERFACE_SUB;
        UNSET_FLAG(ifp->flags, IFF_UP);
        UNSET_FLAG(ifp->flags, IFF_RUNNING);
    }

    if (ifp->mtu == 0)
    {

        if (ifm_get_mtu(pevent.ifindex, MODULE_ID_OSPF6, &mtu) == 0)
        {
            ifp->mtu = mtu;
        }
    }

    if (IS_OSPF6_DEBUG_ZEBRA(RECV))
	{
        zlog_debug(OSPF6_DBG_ZEBRA_MSG, "OSPF6 Interface state change: "
                   "%s index %0x flags %llx metric %d mtu %d bandwidth %d",
                   ifp->name, ifp->ifindex, (unsigned long long)ifp->flags,
                   ifp->metric, ifp->mtu6, ifp->bandwidth);
	}

    ospf6_interface_state_update(ifp);
    return 0;
}

int
ospf6_zebra_if_address_update_add(struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct connected *c;
    char buf[128];

    c = ospf6_interface_address_read(ZEBRA_INTERFACE_ADDRESS_ADD, pevent, vrf_id);

    if (c == NULL)
    {
        return 0;
    }

    if (IS_OSPF6_DEBUG_ZEBRA(RECV))
        zlog_debug(OSPF6_DBG_ZEBRA_MSG, "OSPF6 Interface address add: %s %5s %s/%d",
                   c->ifp->name, prefix_family_str(c->address),
                   inet_ntop(c->address->family, &c->address->u.prefix,
                             buf, sizeof(buf)), c->address->prefixlen);

    if (c->address->family == AF_INET6)
    {
        ospf6_interface_state_update(c->ifp);
        ospf6_interface_connected_route_update(c->ifp);
    }

    return 0;
}

int
ospf6_zebra_if_address_update_delete(struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct connected *c;
    char buf[128];
    char ip_str[IPV6_ADDR_STRLEN];
	
	if (IS_OSPF6_DEBUG_ZEBRA(RECV))
		zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%-15s[%d]: OSPF6 get ipv6 [%s],mask :%d", __func__, __LINE__,
       		inet_ipv6tostr((struct ipv6_addr *)pevent.ipaddr.addr.ipv6, ip_str, IPV6_ADDR_STRLEN) ,
       		pevent.ipaddr.prefixlen);
	
    c = ospf6_interface_address_read(ZEBRA_INTERFACE_ADDRESS_DELETE, pevent, vrf_id);

    if (c == NULL)
    {
        return 0;
    }

    if (IS_OSPF6_DEBUG_ZEBRA(RECV))
        zlog_debug(OSPF6_DBG_ZEBRA_MSG, "OSPF6 Interface address delete: %s %5s %s/%d",
                   c->ifp->name, prefix_family_str(c->address),
                   inet_ntop(c->address->family, &c->address->u.prefix,
                             buf, sizeof(buf)), c->address->prefixlen);

    if (c->address->family == AF_INET6)
    {
        ospf6_interface_connected_route_update(c->ifp);
        ospf6_interface_state_update(c->ifp);
    }

    return 0;
}

int
ospf6_interface_mode_change(struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;
    struct ospf6_interface *oi = NULL;
    ifp = ospf6_interface_add_read(pevent.ifindex, vrf_id);

    if (ifp != NULL)
    {
    
		if (IS_OSPF6_DEBUG_ZEBRA (RECV))
		{
        	zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%s[%d] %d\n", __func__, __LINE__, pevent.mode);
		}
		
		if (pevent.mode != IFNET_MODE_L3)
        {     	
			if (IS_OSPF6_DEBUG_ZEBRA (RECV))
			{
        		zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%s[%d] mode %d del interface %s\n", __func__, __LINE__, pevent.mode, ifp->name);
			}
            ospf6_zebra_if_del(pevent, vrf_id);
        }
	
		
        oi = (struct ospf6_interface *) ifp->info;

        if (oi == NULL)
        {
            return 0;
        }

        oi->mode = pevent.mode;

    }

    return 0;
}

static int
ospf6_asbr_external_route_manage(int command, struct route_entry *p_route, struct prefix *prefix,
                                 struct ase_ipv6 api, struct in6_addr nexthop)
{
    struct ospf6_route *route = NULL, *match = NULL;
    struct ospf6_external_info *info = NULL;
    //char pbuf[64];

    if (om6->redis_count[api.type] > 0)
    {

        match = ospf6_route_lookup(prefix, om6->external6_table);

        if (command == IPC_OPCODE_ADD)
        {
            /* active route */
            if (p_route->nhp[0].active == 0)
            {
                zlog_warn("type:%d, the route is inactive", api.type);
                return -1;
            }

            if (match)
            {
                info = match->route_option;
                info->type = api.type;
                info->instance = api.instance;
                match->nexthop[0].ifindex = api.index;

                //if (api.nexthop_num && nexthop.s6_addr != 0)
				if((api.nexthop_num) && (ipv6_is_zero((struct ipv6_addr *)(&nexthop.s6_addr))))
                {
                    memcpy(&match->nexthop[0].address, &nexthop, sizeof(struct in6_addr));
                }

                return 0;
            }

            /* create new entry */
            route = ospf6_route_create();
            route->type = OSPF6_DEST_TYPE_NETWORK;
            memcpy(&route->prefix, prefix, sizeof(struct prefix));
			
            info = (struct ospf6_external_info *)
                   XCALLOC(MTYPE_OSPF6_EXTERNAL_INFO, sizeof(struct ospf6_external_info));
			if (info == NULL)
			{
				zlog_err("%-15s[%d] Can't malloc external_info", __FUNCTION__, __LINE__);
				return -1;
			}
			memset(info, 0, sizeof(struct ospf6_external_info));
            route->route_option = info;
			info->type = api.type;
            info->instance = api.instance;
            route->nexthop[0].ifindex = api.index;
						

            //if (api.nexthop_num && nexthop.s6_addr != 0)
			if((api.nexthop_num) && (ipv6_is_zero((struct ipv6_addr *)(&nexthop.s6_addr))))
            {
                memcpy(&route->nexthop[0].address, &nexthop, sizeof(struct in6_addr));
            }			
			route = ospf6_route_add (route, om6->external6_table, NULL);
			
        }
        else if (command == IPC_OPCODE_DELETE)  /* if (command == ZEBRA_IPV4_ROUTE_DELETE) */
        {
        #if 0
            if (match == NULL)
            {
                if (IS_OSPF6_DEBUG_ASBR)
                {
                    prefix2str(prefix, pbuf, sizeof(pbuf));
                    zlog_debug(OSPF6_DBG_ASBR, "No such route %s to withdraw", pbuf);
                }

                return -1;
            }

            info = match->route_option;
            assert(info);

            if (info->type != api.type && info->instance != api.instance)
            {
                if (IS_OSPF6_DEBUG_ASBR)
                {
                    prefix2str(prefix, pbuf, sizeof(pbuf));
                    zlog_debug(OSPF6_DBG_ASBR, "Original protocol mismatch: %s", pbuf);
                }

                return -1;
            }

            ospf6_route_remove(match, om6->external6_table, NULL);
            XFREE(MTYPE_OSPF6_EXTERNAL_INFO, info);
            info = NULL;
		#endif

        }
    }

    return 0;
}


int
ospf6_zebra_read_ipv6(int command, struct route_entry *p_route,
                      zebra_size_t length, vrf_id_t vrf_id)
{
    struct ase_ipv6 api;
    unsigned long ifindex;
    struct prefix_ipv6 p;
    struct in6_addr nexthop;
    unsigned char plength = 0;
    struct listnode *node = NULL;
    struct ospf6 *ospf6 = NULL;
    ifindex = 0;


    if (!p_route || p_route->prefix.type != INET_FAMILY_IPV6)
    {    
        return 0;
    }

    memset(&api, 0, sizeof(struct ase_ipv6));
    /* Type, flags, message. */
    api.type = p_route->nhp[0].protocol;
    api.flags = p_route->nhp[0].action;
    api.message = ZAPI_MESSAGE_NEXTHOP;/*??????*/



    /* IPv6 prefix. */
    memset(&p, 0, sizeof(struct prefix_ipv6));
    p.family = AF_INET6;
    plength = p_route->prefix.prefixlen;
    p.prefixlen = MIN(IPV6_MAX_PREFIXLEN, plength);
    IPV6_ADDR_COPY(&p.prefix, p_route->prefix.addr.ipv6);


    /* Nexthop, ifindex, distance, metric. */
    if (p_route->nhp[0].action != NHP_ACTION_DROP)
    {

        api.nexthop_num = p_route->nhp_num;
        IPV6_ADDR_COPY(&nexthop, p_route->nhp[0].nexthop.addr.ipv6);
        SET_FLAG(api.message, ZAPI_MESSAGE_NEXTHOP);

        api.ifindex_num = p_route->nhp_num;
        api.index = p_route->nhp[0].ifindex;
        ifindex = p_route->nhp[0].ifindex;
    }

    api.distance = p_route->nhp[0].distance;
    api.metric = (int)p_route->nhp[0].cost;
    api.instance = p_route->nhp[0].instance;
    api.nhp_type = p_route->nhp[0].nhp_type;

    if (IS_OSPF6_DEBUG_ZEBRA(RECV))
    {
        char prefixstr[128], nexthopstr[128];
        prefix2str((struct prefix *)&p, prefixstr, sizeof(prefixstr));

        //if (nexthop.s6_addr == 0)
        
		if(ipv6_is_zero((struct ipv6_addr *)(&nexthop.s6_addr)))
        {
            inet_ntop(AF_INET6, &nexthop, nexthopstr, sizeof(nexthopstr));
        }
        else
        {
            snprintf(nexthopstr, sizeof(nexthopstr), "::");
        }

        zlog_debug(OSPF6_DBG_ZEBRA_MSG, "Receive route %s: %s(%d) %s nexthop %s ifindex %ld",
                   (command == IPC_OPCODE_ADD ? "add" : "delete"),
                   zebra_route_string(api.type), api.type, prefixstr, nexthopstr, ifindex);
    }

    ospf6_asbr_external_route_manage(command, p_route, (struct prefix *) &p, api, nexthop);

    for (ALL_LIST_ELEMENTS_RO(om6->ospf6, node, ospf6))
    {    
        if (ospf6_redist_lookup(ospf6, api.type, api.instance))
        {        
            if (command == IPC_OPCODE_ADD)
			{
                ospf6_asbr_redistribute_add(ospf6, api, ifindex, (struct prefix *) &p,
                                            api.nexthop_num, &nexthop);
			}
            else
            {
                ospf6_asbr_redistribute_remove(ospf6, api.type, api.instance, ifindex, (struct prefix *) &p);
            }
        }
    }

    return 0;
}


#define ADD    0
#define REM    1
#define UPDATE 2

static int
zapi_ipv6_route(u_char cmd, struct prefix_ipv6 *p,
                struct ase_ipv6 *api)
{
    int i;
    struct route_entry route;
    struct routefifo *newroutefifoentry = NULL;
    memset(&route, 0 , sizeof(struct route_entry));

    route.prefix.type = INET_FAMILY_IPV6;
    IPV6_ADDR_COPY(route.prefix.addr.ipv6, &p->prefix);
    route.prefix.prefixlen = p->prefixlen;

    route.vpn = 0;

    if (api->nexthop_num < NHP_ECMP_NUM)
        route.nhp_num = api->nexthop_num;
    else
        route.nhp_num = NHP_ECMP_NUM;

    for (i = 0; i < api->nexthop_num; i++)
    {
        route.nhp[i].nexthop.type = INET_FAMILY_IPV6;
		if(api->nexthop[i] != NULL)
        	IPV6_ADDR_COPY(route.nhp[i].nexthop.addr.ipv6, api->nexthop[i]);
        route.nhp[i].ifindex = api->ifindex[i];
        route.nhp[i].distance = api->distance;
        //route.nhp[i].vpn = ospf->vpn;
        route.nhp[i].active = 0;
        route.nhp[i].nhp_type = NHP_TYPE_IP;
        route.nhp[i].action = NHP_ACTION_FORWARD;
        route.nhp[i].protocol = ROUTE_PROTO_OSPF6;
        route.nhp[i].down_flag = 1;
        route.nhp[i].instance = 1;
        route.nhp[i].cost = api->metric;
		route.nhp[i].instance = api->instance;
		route.nhp[i].distance = api->distance;


        if (i > NHP_ECMP_NUM - 1)
        {
            break;
        }
    }

    newroutefifoentry = XCALLOC(MTYPE_ROUTE_FIFO, sizeof(struct routefifo));

    if (newroutefifoentry == NULL)
    {
        zlog_err("%-15s[%d]: memory xcalloc error routefifo !", __FUNCTION__, __LINE__);
        return -1;
    }

    memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
    newroutefifoentry->opcode = cmd;
    FIFO_ADD(&om6->routefifo, &newroutefifoentry->fifo);
    return 0;

}

static int
zapi_ipv6_route_direct(u_char cmd, struct prefix_ipv6 *p,
                struct ase_ipv6 *api)
{
    int i;
    struct route_entry route;
    memset(&route, 0 , sizeof(struct route_entry));

    route.prefix.type = INET_FAMILY_IPV6;
    IPV6_ADDR_COPY(route.prefix.addr.ipv6, &p->prefix);
    route.prefix.prefixlen = p->prefixlen;

    route.vpn = 0;

    if (api->nexthop_num < NHP_ECMP_NUM)
        route.nhp_num = api->nexthop_num;
    else
        route.nhp_num = NHP_ECMP_NUM;

    for (i = 0; i < api->nexthop_num; i++)
    {
        route.nhp[i].nexthop.type = INET_FAMILY_IPV6;
		if(api->nexthop[i] != NULL)
        	IPV6_ADDR_COPY(route.nhp[i].nexthop.addr.ipv6, api->nexthop[i]);
        route.nhp[i].ifindex = api->ifindex[i];
        route.nhp[i].distance = api->distance;
        //route.nhp[i].vpn = ospf->vpn;
        route.nhp[i].active = 0;
        route.nhp[i].nhp_type = NHP_TYPE_IP;
        route.nhp[i].action = NHP_ACTION_FORWARD;
        route.nhp[i].protocol = ROUTE_PROTO_OSPF6;
        route.nhp[i].down_flag = 1;
        route.nhp[i].instance = 1;
        route.nhp[i].cost = api->metric;
		route.nhp[i].instance = api->instance;
		route.nhp[i].distance = api->distance;


        if (i > NHP_ECMP_NUM - 1)
        {
            break;
        }
    }
    int ret = ipc_send_msg_n2(&route, sizeof(struct route_entry) , 1, MODULE_ID_ROUTE, MODULE_ID_OSPF6,
                             IPC_TYPE_ROUTE, 0, cmd, 0);

    if (ret < 0)
    {
        zlog_err("%-15s[%d]:ERROR:route send ", __FUNCTION__, __LINE__);
        return -1;
    }
	
    return 0;

}

static u_char
ospf6_distance_apply (struct ospf6_route *or, struct ospf6 *ospf6)
{
    if (ospf6 == NULL)
    {
        return 0;
    }
    if (ospf6->distance_external)
    {
        /* AS External routes are never considered */
    	if (or->path.type == OSPF6_PATH_TYPE_EXTERNAL1 ||
            or->path.type == OSPF6_PATH_TYPE_EXTERNAL2)
        {
            return ospf6->distance_external;
        }
    }
	
    if (ospf6->distance_all)
    {
        return ospf6->distance_all;
    }
    return 0;
}

static void
ospf6_zebra_route_update_direct(struct ospf6 *ospf6, int type, struct ospf6_route *request)
{
    struct ase_ipv6 api;
    char buf[64];
    int nhcount;
    struct in6_addr **nexthops;
    ifindex_t *ifindexes;
    int i, ret = 0;
    struct prefix_ipv6 *dest;

    if (IS_OSPF6_DEBUG_ZEBRA(SEND))
    {
        prefix2str(&request->prefix, buf, sizeof(buf));
		if(type == UPDATE)
		{
        	zlog_debug(OSPF6_DBG_ZEBRA_MSG, "Send %s route: %s",
                   "update", buf);

		}
		else
		{
        	zlog_debug(OSPF6_DBG_ZEBRA_MSG, "Send %s route: %s",
                   (type == REM ? "remove" : "add"), buf);
		}
    }


    if (request->path.origin.adv_router == ospf6->router_id &&
            (request->path.type == OSPF6_PATH_TYPE_EXTERNAL1 ||
             request->path.type == OSPF6_PATH_TYPE_EXTERNAL2))
    {
        if (IS_OSPF6_DEBUG_ZEBRA(SEND))
        {
            zlog_debug(OSPF6_DBG_ZEBRA_MSG, "  Ignore self-originated external route");
        }

        return;
    }

    /* If removing is the best path and if there's another path,
       treat this request as add the secondary path */
    if (type == REM && ospf6_route_is_best(request) &&
            request->next && ospf6_route_is_same(request, request->next))
    {
        if (IS_OSPF6_DEBUG_ZEBRA(SEND))
        {
            zlog_debug(OSPF6_DBG_ZEBRA_MSG, "  Best-path removal resulted Sencondary addition");
        }

        //type = ADD;
        //request = request->next;
		ospf6_zebra_route_update_direct(ospf6, ADD, request->next);
    }
	
    /* Only the best path will be sent to zebra. */
    if (! ospf6_route_is_best(request))
    {
        /* this is not preferred best route, ignore */
        if (IS_OSPF6_DEBUG_ZEBRA(SEND))
        {
            zlog_debug(OSPF6_DBG_ZEBRA_MSG, "  Ignore non-best route");
        }

        return;
    }

    nhcount = 0;

    for (i = 0; i < OSPF6_MULTI_PATH_LIMIT; i++)
    {
        if (ospf6_nexthop_is_set(&request->nexthop[i]))
        {
            nhcount++;
        }
    }

    if (nhcount == 0)
    {
        if (IS_OSPF6_DEBUG_ZEBRA(SEND))
        {
            zlog_debug(OSPF6_DBG_ZEBRA_MSG, "  No nexthop, ignore");
        }

        return;
    }

    /* allocate memory for nexthop_list */
    nexthops = XCALLOC(MTYPE_OSPF6_OTHER,
                       nhcount * sizeof(struct in6_addr *));

    if (nexthops == NULL)
    {
        zlog_err("Can't send route : malloc failed");
        return;
    }
	memset(nexthops, 0, nhcount * sizeof(struct in6_addr *));

    /* allocate memory for ifindex_list */
    ifindexes = XCALLOC(MTYPE_OSPF6_OTHER,
                        nhcount * sizeof(unsigned int));

    if (ifindexes == NULL)
    {
        zlog_err("Can't send route : malloc failed");
        XFREE(MTYPE_OSPF6_OTHER, nexthops);
		nexthops = NULL;
        return;
    }
	memset(ifindexes, 0, nhcount * sizeof(unsigned int));

    for (i = 0; i < nhcount; i++)
    {
        if (IS_OSPF6_DEBUG_ZEBRA(SEND))
        {
            const char *ifname;
            inet_ntop(AF_INET6, &request->nexthop[i].address,
                      buf, sizeof(buf));
            ifname = ifindex2ifname(request->nexthop[i].ifindex);
            zlog_debug(OSPF6_DBG_ZEBRA_MSG, "  nexthop: %s%%%.*s(%0x)", buf, NAME_STRING_LEN, ifname,
                       request->nexthop[i].ifindex);
        }
				
		if(ipv6_is_loopback((struct ipv6_addr *)(&request->nexthop[i].address.s6_addr)))
		{
        	nexthops[i] = &request->nexthop[i].address;
		}
        ifindexes[i] = request->nexthop[i].ifindex;
    }

    api.vrf_id = VRF_DEFAULT;
    api.type = ROUTE_PROTO_OSPF6;
    api.flags = 0;
    api.message = 0;
    api.safi = SAFI_UNICAST;
    SET_FLAG(api.message, ZAPI_MESSAGE_NEXTHOP);
    api.nexthop_num = nhcount;
    api.nexthop = nexthops;
    SET_FLAG(api.message, ZAPI_MESSAGE_IFINDEX);
    api.ifindex_num = nhcount;
    api.ifindex = ifindexes;
    SET_FLAG(api.message, ZAPI_MESSAGE_METRIC);
    api.metric = (request->path.metric_type == 2 ?
                  request->path.cost_e2 : request->path.cost);
    dest = (struct prefix_ipv6 *) & request->prefix;
	api.instance = ospf6->ospf_id;
	api.distance = ospf6_distance_apply(request, ospf6);

    if (type == REM)
    {
        ret = zapi_ipv6_route_direct(IPC_OPCODE_DELETE, dest, &api);
    }
    else if(type == ADD)
    {
        ret = zapi_ipv6_route_direct(IPC_OPCODE_ADD, dest, &api);
    }
	else 
	{
		ret = zapi_ipv6_route_direct(IPC_OPCODE_UPDATE, dest, &api);
	}

    if (ret < 0)
    {
    	if(type == UPDATE)
		{
			zlog_err("%s %d %s failed: %s",__func__,__LINE__,
                 "update", safe_strerror(errno));

		}
		else
		{
        	zlog_err("%s %d %s failed: %s",__func__,__LINE__,
                 (type == REM ? "delete" : "add"), safe_strerror(errno));
		}
    }

    XFREE(MTYPE_OSPF6_OTHER, nexthops);
	nexthops = NULL;
    XFREE(MTYPE_OSPF6_OTHER, ifindexes);
	ifindexes = NULL;
    return;
}

void
ospf6_finish_route_table(struct ospf6 *ospf6)
{
	struct ospf6_route *route = NULL;
	route = ospf6_route_head(ospf6->route_table); 
	while (route)
    {
    	ospf6_zebra_route_update_direct(ospf6, REM, route);
        route = ospf6_route_next(route);
    }
}



static void
ospf6_zebra_route_update(struct ospf6 *ospf6, int type, struct ospf6_route *request)
{
    struct ase_ipv6 api;
    char buf[64];
    int nhcount;
    struct in6_addr **nexthops;
    ifindex_t *ifindexes;
    int i, ret = 0;
    struct prefix_ipv6 *dest;

    if (IS_OSPF6_DEBUG_ZEBRA(SEND))
    {
        prefix2str(&request->prefix, buf, sizeof(buf));
		if(type == UPDATE)
		{
        	zlog_debug(OSPF6_DBG_ZEBRA_MSG, "Send %s route: %s",
                   "update", buf);

		}
		else
		{
        	zlog_debug(OSPF6_DBG_ZEBRA_MSG, "Send %s route: %s",
                   (type == REM ? "remove" : "add"), buf);
		}
    }


    if (request->path.origin.adv_router == ospf6->router_id &&
            (request->path.type == OSPF6_PATH_TYPE_EXTERNAL1 ||
             request->path.type == OSPF6_PATH_TYPE_EXTERNAL2))
    {
        if (IS_OSPF6_DEBUG_ZEBRA(SEND))
        {
            zlog_debug(OSPF6_DBG_ZEBRA_MSG, "  Ignore self-originated external route");
        }

        return;
    }

    /* If removing is the best path and if there's another path,
       treat this request as add the secondary path */
    if (type == REM && ospf6_route_is_best(request) &&
            request->next && ospf6_route_is_same(request, request->next))
    {
        if (IS_OSPF6_DEBUG_ZEBRA(SEND))
        {
            zlog_debug(OSPF6_DBG_ZEBRA_MSG, "  Best-path removal resulted Sencondary addition");
        }

        //type = ADD;
        //request = request->next;
        
		ospf6_zebra_route_update(ospf6, ADD, request->next);
    }

    /* Only the best path will be sent to zebra. */
    if (! ospf6_route_is_best(request))
    {
        /* this is not preferred best route, ignore */
        if (IS_OSPF6_DEBUG_ZEBRA(SEND))
        {
            zlog_debug(OSPF6_DBG_ZEBRA_MSG, "  Ignore non-best route");
        }

        return;
    }

    nhcount = 0;

    for (i = 0; i < OSPF6_MULTI_PATH_LIMIT; i++)
    {
        if (ospf6_nexthop_is_set(&request->nexthop[i]))
        {
            nhcount++;
        }
    }

    if (nhcount == 0)
    {
        if (IS_OSPF6_DEBUG_ZEBRA(SEND))
        {
            zlog_debug(OSPF6_DBG_ZEBRA_MSG, "  No nexthop, ignore");
        }

        return;
    }

    /* allocate memory for nexthop_list */
    nexthops = XCALLOC(MTYPE_OSPF6_OTHER,
                       nhcount * sizeof(struct in6_addr *));

    if (nexthops == NULL)
    {
        zlog_warn("Can't send route to zebra: malloc failed");
        return;
    }
	
	memset(nexthops, 0, nhcount * sizeof(struct in6_addr *));
	

    /* allocate memory for ifindex_list */
    ifindexes = XCALLOC(MTYPE_OSPF6_OTHER,
                        nhcount * sizeof(unsigned int));

    if (ifindexes == NULL)
    {
        zlog_warn("Can't send route to zebra: malloc failed");
        XFREE(MTYPE_OSPF6_OTHER, nexthops);
		nexthops = NULL;
        return;
    }
	memset(ifindexes, 0, nhcount * sizeof(unsigned int));

    for (i = 0; i < nhcount; i++)
    {
        if (IS_OSPF6_DEBUG_ZEBRA(SEND))
        {
            const char *ifname;
            inet_ntop(AF_INET6, &request->nexthop[i].address,
                      buf, sizeof(buf));
            ifname = ifindex2ifname(request->nexthop[i].ifindex);
            zlog_debug(OSPF6_DBG_ZEBRA_MSG, "  nexthop: %s%%%.*s(%0x)", buf, NAME_STRING_LEN, ifname,
                       request->nexthop[i].ifindex);            
        }
				
		if(!ipv6_is_loopback((struct ipv6_addr *)(&request->nexthop[i].address.s6_addr)))
		{
        	nexthops[i] = &request->nexthop[i].address;
		}
        ifindexes[i] = request->nexthop[i].ifindex;
    }

    api.vrf_id = VRF_DEFAULT;
    api.type = ROUTE_PROTO_OSPF6;
    api.flags = 0;
    api.message = 0;
    api.safi = SAFI_UNICAST;
    SET_FLAG(api.message, ZAPI_MESSAGE_NEXTHOP);
    api.nexthop_num = nhcount;
    api.nexthop = nexthops;
    SET_FLAG(api.message, ZAPI_MESSAGE_IFINDEX);
    api.ifindex_num = nhcount;
    api.ifindex = ifindexes;
    SET_FLAG(api.message, ZAPI_MESSAGE_METRIC);
    api.metric = (request->path.metric_type == 2 ?
                  request->path.cost_e2 : request->path.cost);
    dest = (struct prefix_ipv6 *) & request->prefix;
	api.instance = ospf6->ospf_id;
	api.distance = ospf6_distance_apply (request, ospf6);

    if (type == REM)
    {
        ret = zapi_ipv6_route(IPC_OPCODE_DELETE, dest, &api);
    }
    else if(type == ADD)
    {
        ret = zapi_ipv6_route(IPC_OPCODE_ADD, dest, &api);
    }
	else 
	{
		ret = zapi_ipv6_route(IPC_OPCODE_UPDATE, dest, &api);
	}

    if (ret < 0)
    {
    	if(type == UPDATE)
		{
			zlog_err("%s %d %s failed: %s",__func__,__LINE__,
                 "update", safe_strerror(errno));

		}
		else
		{
        	zlog_err("%s %d %s failed: %s",__func__,__LINE__,
                 (type == REM ? "delete" : "add"), safe_strerror(errno));
		}
    }

    XFREE(MTYPE_OSPF6_OTHER, nexthops);
	nexthops = NULL;
    XFREE(MTYPE_OSPF6_OTHER, ifindexes);
	ifindexes = NULL;
    return;
}

void
ospf6_zebra_route_update_add(struct ospf6_route *request, struct ospf6 *ospf6)
{
#if 0

    if (! vrf_bitmap_check(zclient->redist[ZEBRA_ROUTE_OSPF6], VRF_DEFAULT))
    {
        ospf6->route_table->hook_add = NULL;
        ospf6->route_table->hook_remove = NULL;
        return;
    }

#endif
    ospf6_zebra_route_update(ospf6, ADD, request);
}

void
ospf6_zebra_route_update_remove(struct ospf6_route *request, struct ospf6 *ospf6)
{

#if 0

    if (! vrf_bitmap_check(zclient->redist[ZEBRA_ROUTE_OSPF6], VRF_DEFAULT))
    {
        ospf6->route_table->hook_add = NULL;
        ospf6->route_table->hook_remove = NULL;
        return;
    }

#endif

    ospf6_zebra_route_update(ospf6, REM, request);
}

void
ospf6_zebra_route_update_update(struct ospf6_route *request, struct ospf6 *ospf6)
{
	ospf6_zebra_route_update(ospf6, UPDATE, request);
}


DEFUN(redistribute_ospf6,
      redistribute_ospf6_cmd,
      "redistribute ospf6",
      "Redistribute control\n"
      "OSPF6 route\n")
{
    struct ospf6 *ospf6 = (struct ospf6 *)vty->index;
    struct ospf6_route *route;

    //if (vrf_bitmap_check (zclient->redist[ZEBRA_ROUTE_OSPF6], VRF_DEFAULT))
    //{
    //  return CMD_SUCCESS;
    //}
    //vrf_bitmap_set (zclient->redist[ZEBRA_ROUTE_OSPF6], VRF_DEFAULT);
    if (ospf6 == NULL)
    {
        return CMD_SUCCESS;
    }

    /* send ospf6 route to zebra route table */
    for (route = ospf6_route_head(ospf6->route_table); route;
            route = ospf6_route_next(route))
    {
        ospf6_zebra_route_update_add(route, ospf6);
    }

    ospf6->route_table->hook_add = ospf6_zebra_route_update_add;
    ospf6->route_table->hook_remove = ospf6_zebra_route_update_remove;
    return CMD_SUCCESS;
}

DEFUN(no_redistribute_ospf6,
      no_redistribute_ospf6_cmd,
      "no redistribute ospf6",
      NO_STR
      "Redistribute control\n"
      "OSPF6 route\n")
{
    struct ospf6_route *route = NULL;
    struct ospf6 *ospf6 = (struct ospf6 *)vty->index;

    //if (! vrf_bitmap_check (zclient->redist[ZEBRA_ROUTE_OSPF6], VRF_DEFAULT))
    //{
    //  return CMD_SUCCESS;
    //}
    //vrf_bitmap_unset (zclient->redist[ZEBRA_ROUTE_OSPF6], VRF_DEFAULT);
    if (ospf6 == NULL)
    {
        return CMD_SUCCESS;
    }

    ospf6->route_table->hook_add = NULL;
    ospf6->route_table->hook_remove = NULL;

    /* withdraw ospf6 route from zebra route table */
    for (route = ospf6_route_head(ospf6->route_table); route;
            route = ospf6_route_next(route))
    {
        ospf6_zebra_route_update_remove(route, ospf6);
    }

    return CMD_SUCCESS;
}

#if 0
/* Debug */

DEFUN(debug_ospf6_route_sendrecv,
      debug_ospf6_route_sendrecv_cmd,
      "debug ospf6 msg route (send|recv|all)",
      DEBUG_STR
      OSPF6_STR
      "message between route and ospf6"
      "Debug connection between route\n"
      "Debug Sending route\n"
      "Debug Receiving route\n"
      "Debug connection between route send and recv\n"
     )
{
    unsigned char level = 0;


    if (! strncmp(argv[0], "s", 1))
    {
        level = OSPF6_DEBUG_ZEBRA_SEND;
    }
    else if (! strncmp(argv[0], "r", 1))
    {
        level = OSPF6_DEBUG_ZEBRA_RECV;
    }
    else if (! strncmp(argv[0], "a", 1))
    {

        level = OSPF6_DEBUG_ZEBRA_SEND | OSPF6_DEBUG_ZEBRA_RECV;
    }
    else
    {
        vty_error_out(vty,"command error,%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    OSPF6_DEBUG_ZEBRA_ON(level);
    return CMD_SUCCESS;
}




DEFUN(no_debug_ospf6_route_sendrecv,
      no_debug_ospf6_route_sendrecv_cmd,
      "no debug ospf6 msg route (send|recv|all)",
      NO_STR
      DEBUG_STR
      OSPF6_STR
      "message between route and ospf6"
      "Debug connection between route\n"
      "Debug Sending route\n"
      "Debug Receiving route\n"
      "Debug connection between route send and recv\n"
     )
{
    unsigned char level = 0;

    if (! strncmp(argv[0], "s", 1))
    {
        level = OSPF6_DEBUG_ZEBRA_SEND;
    }
    else if (! strncmp(argv[0], "r", 1))
    {
        level = OSPF6_DEBUG_ZEBRA_RECV;
    }
    else
    {
        level = OSPF6_DEBUG_ZEBRA_SEND | OSPF6_DEBUG_ZEBRA_RECV;
    }

    OSPF6_DEBUG_ZEBRA_OFF(level);
    return CMD_SUCCESS;
}
#endif


int
config_write_ospf6_debug_zebra(struct vty *vty)
{
    if (IS_OSPF6_DEBUG_ZEBRA(SEND) && IS_OSPF6_DEBUG_ZEBRA(RECV))
    {
        vty_out(vty, "debug ospf6 zebra%s", VNL);
    }
    else
    {
        if (IS_OSPF6_DEBUG_ZEBRA(SEND))
        {
            vty_out(vty, "debug ospf6 zebra send%s", VNL);
        }

        if (IS_OSPF6_DEBUG_ZEBRA(RECV))
        {
            vty_out(vty, "debug ospf6 zebra recv%s", VNL);
        }
    }

    return 0;
}
#if 0
void
install_element_ospf6_debug_zebra(void)
{
    install_element(CONFIG_NODE, &debug_ospf6_route_sendrecv_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_debug_ospf6_route_sendrecv_cmd, CMD_SYNC);
}

#endif

