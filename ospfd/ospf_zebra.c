/*
 * Zebra connect library for OSPFd
 * Copyright (C) 1997, 98, 99, 2000 Kunihiro Ishiguro, Toshiaki Takada
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


#include "lib/command.h"
#include "lib/prefix.h"
#include "lib/routemap.h"
#include "lib/memory.h"
#include "lib/plist.h"
#include "lib/module_id.h"
#include "lib/msg_ipc.h"
//#include "lib/errcode.h"
#include "lib/thread.h"
#include "lib/ifm_common.h"
#include "lib/str.h"


//#include "inet_ip.h"

#include "ospfd/ospfd.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_abr.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_dump.h"
#include "ospfd/ospf_zebra.h"
#include "ospfd/ospf_ipc.h"
#ifdef HAVE_SNMP
//#include "ospfd/ospf_snmp.h"
#endif /* HAVE_SNMP */

//#include "route/route.h"


/* Zebra structure to hold current status. */

/* For registering threads. */
//extern struct thread_master *master_ospf;
struct in_addr router_id_zebra;
extern size_t strnlen(const char *s, size_t maxlen);

static int
memconstant(const void *s, int c, size_t n)
{
    const u_char *p = s;
    while (n-- > 0)
        if (*p++ != c)
        {
            return 0;
        }
    return 1;
}


/* Get prefix in ZServ format; family should be filled in on prefix */
static void
ospf_get_prefix (struct ifm_event pevent, struct prefix *p)
{
    size_t plen = prefix_blen (p);
    u_char c;
    p->prefixlen = 0;
    if (plen == 0)
    {
        return;
    }
    memcpy (&p->u.prefix, &(pevent.ipaddr.addr), plen);
    p->u.prefix4.s_addr = ntohl(p->u.prefix4.s_addr);
    c = pevent.ipaddr.prefixlen;
    p->prefixlen = MIN(plen * 8, c);
}

/* get the interface mode */
static int ospf_get_intf_mode(ifindex_t ifindex)
{
    uint8_t pmode = 0;
    int     ret = 0;
    zlog_debug(OSPF_DBG_OTHER, "%s[%d]: In function '%s'.\n", __FILE__, __LINE__, __func__);
    
    ret = ifm_get_mode(ifindex, MODULE_ID_OSPF, &pmode);
    
    if (0 != ret)
    {
    	zlog_warn("%s %d get mode error!!", __func__,__LINE__);
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
	
	zlog_warn("%s %d get mode invalid!!", __func__,__LINE__);
    return IFNET_MODE_INVALID;
}


static void
ospf_interface_if_set_value (struct ifm_event pevent,struct interface *ifp)
{
    uint16_t mtu = 0;
    uchar  pmac[6];
	//int num = 5;
	
    /* Read interface's index. */
    ifp->ifindex = pevent.ifindex;
    
    int type = IFM_TYPE_ID_GET (pevent.ifindex);
    /*set link type */
    ifp->ll_type = type;
    /* set ifp vpn*/
    ifp->vpn = pevent.vpn;
	ifp->flags = 0;

    if (type == IFNET_TYPE_LOOPBACK)
    {
        ifp->metric = 0;
        SET_FLAG(ifp->flags, IFF_LOOPBACK);
        SET_FLAG(ifp->flags, IFF_NOXMIT);
        SET_FLAG(ifp->flags, IFF_VIRTUAL);
        ifp->bandwidth = 0;
    }
    else
    {
        ifp->metric = 1;
        ifp->bandwidth = 100000;//Why is 100000 instead of 1000000?
        SET_FLAG(ifp->flags, IFF_BROADCAST);
    }
    /* set ifp link*/
    if(pevent.up_flag == IFNET_LINKUP)
    {
        SET_FLAG(ifp->status, ZEBRA_INTERFACE_ACTIVE);
        SET_FLAG(ifp->flags, IFF_UP);
        SET_FLAG(ifp->flags, IFF_RUNNING);
    }
    else
    {
        SET_FLAG(ifp->status , ZEBRA_INTERFACE_SUB);
        SET_FLAG(ifp->flags, 0);
    }
	
    /* get the mac */
	//while(num > 0)
	/*
	{
    	pmac = ifm_get_mac(pevent.ifindex, MODULE_ID_OSPF, pmac);
		if(pmac != NULL)
			break;
		usleep(5000);
		num --;
	}
	*/
	
    if(ifm_get_mac(pevent.ifindex, MODULE_ID_OSPF, pmac) == 0)
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
        zlog_warn("%-15s[%d]: OSPF get ifindex %0x mac error",__func__,__LINE__,pevent.ifindex);
    }
	
    /* get mtu */
    /*
	num = 5;
	while(num > 0)
	{
    	mtu = ifm_get_mtu(pevent.ifindex, MODULE_ID_OSPF, &mtu);
		if(mtu != NULL)
			break;
		usleep(5000);
		num --;
	}
	*/
    if(ifm_get_mtu(pevent.ifindex, MODULE_ID_OSPF, &mtu) == 0)
    {
        ifp->mtu = mtu;
    }
    else
    {
        zlog_warn("%-15s[%d]: OSPF get ifindex %0x mtu error",__func__,__LINE__,pevent.ifindex);
        ifp->mtu = 1500;
    }
}

struct interface *
ospf_interface_add_read(uint32_t ifindex, vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;
    char ifname_tmp[INTERFACE_NAMSIZ];

    /* Read interface name. */
    ifm_get_name_by_ifindex(ifindex, ifname_tmp);

    /* Lookup/create interface by name. */
    ifp = if_get_by_name_len_vrf (ifname_tmp,
                                  strnlen (ifname_tmp, INTERFACE_NAMSIZ),
                                  vrf_id);
	ifp->ifindex = ifindex;

    return ifp;
}


/*
 * Read interface up/down msg (ZEBRA_INTERFACE_UP/ZEBRA_INTERFACE_DOWN)
 * from zebra server.  The format of this message is the same as
 * that sent for ZEBRA_INTERFACE_ADD/ZEBRA_INTERFACE_DELETE (see
 * comments for zebra_interface_add_read), except that no sockaddr_dl
 * is sent at the tail of the message.
 */
static struct interface *
ospf_interface_state_read (uint32_t ifindex, vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;
    char ifname_tmp[INTERFACE_NAMSIZ];

    /* Get interface name. */
    ifm_get_name_by_ifindex(ifindex, ifname_tmp);

    /* Lookup this by interface index. */
   	ifp = if_lookup_by_name_len(ifname_tmp,
                                 strnlen(ifname_tmp, INTERFACE_NAMSIZ));

    /* If such interface does not exist, indicate an error */
    if (! ifp)
    {
        return NULL;
    }

    return ifp;
}


#if 0
static struct interface *
ospf_interface_if_lookup (struct ifm_event pevent, vrf_id_t vrf_id)
{
    char ifname_tmp[INTERFACE_NAMSIZ];

    /* Read interface name. */
    ifm_get_name_by_ifindex(pevent.ifindex, ifname_tmp);

    /* And look it up. */
    return if_lookup_by_name_len(ifname_tmp,
                                 strnlen(ifname_tmp, INTERFACE_NAMSIZ));
}
#endif
/* connect address add/delete from ifp */
static struct connected *
ospf_interface_address_read (int type, struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;
    struct connected *ifc = NULL;
    struct prefix p, d, *dp = NULL;
    int plen;

    memset (&p, 0, sizeof(p));
    memset (&d, 0, sizeof(d));

	
    if (type == ZEBRA_INTERFACE_ADDRESS_ADD)
    {
		ifp = ospf_interface_add (type,pevent,vrf_id);
	}
	else
	{
		ifp = ospf_interface_state_read(pevent.ifindex, vrf_id);
	}

    //ifp = ospf_interface_add (type,pevent,vrf_id);
    if (ifp == NULL)
    {
        if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
    	{
        	zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "ospf_interface_address_read(%s): "
                   "Can't find interface by ifindex: %0x ",
                   (type == ZEBRA_INTERFACE_ADDRESS_ADD? "ADD" : "DELETE"),
                   pevent.ifindex);
        }
        return NULL;
    }


    /* Fetch interface address. */
    if (pevent.ipaddr.type == INET_FAMILY_IPV4)
    {
        d.family = p.family = AF_INET;
    }
    plen = prefix_blen (&d);

    ospf_get_prefix (pevent, &p);

    /* N.B. NULL destination pointers are encoded as all zeroes */
    dp = memconstant(&d.u.prefix,0,plen) ? NULL : &d;

    if (type == ZEBRA_INTERFACE_ADDRESS_ADD)
    {
    	if((ifc = connected_lookup_address(ifp,p.u.prefix4)) != NULL)
    	{
    		if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
    		{
    			zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "The ip address is already exist %s %d",__func__,__LINE__);	
    		}
			return ifc;
    	}
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
                if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
    			{
                	zlog_debug(OSPF_DBG_ZEBRA_INTERFACE, "warnig: ip flag error");
                }
            }

            if (ifc->destination)
            {
                ifc->destination->prefixlen = ifc->address->prefixlen;
            }
            else if (CHECK_FLAG(ifc->flags, ZEBRA_IFA_PEER))
            {
                /* carp interfaces on OpenBSD with 0.0.0.0/0 as "peer" */
                char buf[PREFIX_STRLEN];
				if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
    			{
                	zlog_debug(OSPF_DBG_ZEBRA_INTERFACE, "warning: interface %s address %s "
                          "with peer flag set, but no peer address!",
                          ifp->name,prefix2str (ifc->address, buf, sizeof buf));
				}
                UNSET_FLAG(ifc->flags, ZEBRA_IFA_PEER);
            }
        }
    }
    else
    {
        assert (type == ZEBRA_INTERFACE_ADDRESS_DELETE);
        ifc = connected_delete_by_prefix(ifp, &p);
    }

    return ifc;
}



/* Inteface addition ADD . */
struct interface *
ospf_interface_add (int type, struct ifm_event pevent,
                    vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;
    int mode = 0;

    ifp = ospf_interface_add_read(pevent.ifindex, vrf_id);

	if (ifp->info == NULL)
	{
		zlog_err("%s():%d ifp->info is NULL!", __FUNCTION__, __LINE__);
		return NULL;
	}

    if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
    {
    	zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "%s[%d] index:%0x", __FUNCTION__, __LINE__, pevent.ifindex);
    }

    /*in order to first set the interface value*/
    if(type == ZEBRA_INTERFACE_ADDRESS_ADD)
    {
        ospf_interface_if_set_value (pevent, ifp);
        if (!OSPF_IF_PARAM_CONFIGURED (IF_DEF_PARAMS (ifp), type))
        {
            SET_IF_PARAM (IF_DEF_PARAMS (ifp), type);
            IF_DEF_PARAMS (ifp)->type = ospf_default_iftype(ifp);
        }

        if (ifp->ll_type == IFNET_TYPE_LOOPBACK)
        {
            SET_IF_PARAM (IF_DEF_PARAMS (ifp), passive_interface);
            IF_DEF_PARAMS (ifp)->passive_interface = OSPF_IF_ACTIVE;
        }
        /* get the mode of the interface*/
        mode = ospf_get_intf_mode(pevent.ifindex);
        if( mode >= 0)
        {
            SET_IF_PARAM (IF_DEF_PARAMS (ifp), mode);
            IF_DEF_PARAMS (ifp)->mode = mode;
        }
        if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
        {
            zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "%s %d %s index %0x flags %llx metric %d mtu %d",__func__, __LINE__,
                        ifp->name, ifp->ifindex, (unsigned long long)ifp->flags,
                        ifp->metric, ifp->mtu);	
        }
        ospf_if_update (NULL, ifp);
#ifdef HAVE_SNMP
        ospf_snmp_if_update (ifp);
#endif /* HAVE_SNMP */
    }


    return ifp;
}

int
ospf_interface_delete (struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;
    struct route_node *rn = NULL;
	
    /* ospf_interface_state_read() updates interface structure in iflist */
    ifp = ospf_interface_state_read (pevent.ifindex, vrf_id);
    if (ifp == NULL)
    {
        return 0;
    }
    if (if_is_up (ifp))
    {
        if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
    	{
        	zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "got delete of %s, but interface is still up",
                   ifp->name);
        }
    }
    if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
    {
        zlog_debug
        (OSPF_DBG_ZEBRA_INTERFACE, "interface delete %s index %0x flags %llx metric %d mtu %d",
         ifp->name, ifp->ifindex, (unsigned long long)ifp->flags, ifp->metric, ifp->mtu);
    }
#ifdef HAVE_SNMP
    ospf_snmp_if_delete (ifp);
#endif /* HAVE_SNMP */
    ospf_param_if_default (ifp);
    for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
    {
        if (rn->info)
        {
            ospf_if_free ((struct ospf_interface *) rn->info);
			rn->info = NULL;
        }
    }
    ifp->ifindex = IFINDEX_INTERNAL;
    return 0;
}

int
ospf_interface_mode_change (struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;
    ifp = ospf_interface_state_read (pevent.ifindex, vrf_id);
    if(ifp != NULL)
    {
        if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
    	{
        	zlog_debug(OSPF_DBG_ZEBRA_INTERFACE, "%s[%d] MODE change from %d to %d\n",__func__,__LINE__,
				IF_DEF_PARAMS (ifp)->mode, pevent.mode);
        }
        IF_DEF_PARAMS (ifp)->mode = pevent.mode;
        if(IF_DEF_PARAMS (ifp)->mode != IFNET_MODE_L3)
        {
            ospf_interface_delete (pevent,vrf_id);
        }
    }
    return 0;
}

int
ospf_interface_mtu_change (struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;
    ifp = ospf_interface_state_read (pevent.ifindex, vrf_id);
    if(ifp != NULL)
    {	
    	if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
        {
        	zlog_debug(OSPF_DBG_ZEBRA_INTERFACE, "%s[%d] interface %0x mtu change from %d to %d\n"
				,__func__,__LINE__,pevent.ifindex, ifp->mtu ,pevent.mtu);
		}
        ifp->mtu = pevent.mtu;
        if(if_is_operative(ifp))
        {
            ospf_if_reset(ifp);
        }
    }
    return 0;
}


int
ospf_interface_state_up (struct ifm_event pevent, vrf_id_t vrf_id)
{
    uint16_t mtu = 0;
    int      ret = 0;
    struct interface *ifp = NULL;
    struct route_node *rn = NULL;
    struct ospf_interface *oi = NULL;
	
    ifp = ospf_interface_state_read (pevent.ifindex, vrf_id);
    if (ifp == NULL)
    {
        int type = IFM_TYPE_ID_GET (pevent.ifindex);
        /*set link type and metric*/
        if(type == IFNET_TYPE_ETHERNET)
        {
            return 0;
        }
        else if(type == IFNET_TYPE_LOOPBACK)
        {
            ifp = ospf_interface_add_read (pevent.ifindex, vrf_id);
        }
    }
    if (ifp == NULL)
    {
        return 0;
    }
    
    ret = ifm_get_mtu(pevent.ifindex, MODULE_ID_OSPF, &mtu);
    
    /* Interface is already up. */
    if (if_is_operative (ifp))
    {
        /* Temporarily keep ifp values. */
        struct interface if_tmp;
        memcpy (&if_tmp, ifp, sizeof (struct interface));

        if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
        {
            zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "Interface[%s] state update.", ifp->name);
        }
        if ((ret == 0)&&(mtu != ifp->mtu) && !mtu )
        {
            ifp->mtu = mtu;
            if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
            {
                zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "Interface[%s] MTU change %u -> %u.",
                            ifp->name, if_tmp.mtu, ifp->mtu);
            }
            /* Must reset the interface (simulate down/up) when MTU changes. */
            ospf_if_reset(ifp);
			return 0;
        }
    }
    if(ret == 0)
    {
        ifp->mtu = mtu;
    }
    else
    {
        ifp->mtu = 1500;
    }
    SET_FLAG(ifp->flags, IFF_UP);
    SET_FLAG(ifp->flags, IFF_RUNNING);
    if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
    {
        zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "Interface[%s] state change to up.", ifp->name);
    }
    for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
    {
        if ((oi = rn->info) == NULL)
        {
            continue;
        }
        ospf_if_up (oi);
    }
    return 0;
}

int
ospf_interface_state_down (struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct interface *ifp = NULL;
    struct ospf_interface *oi = NULL;
    struct route_node *node = NULL;
	
    ifp = ospf_interface_state_read (pevent.ifindex, vrf_id);
    if (ifp == NULL)
    {
        return 0;
    }
	
    ifp->flags = 0;
    if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
    {
        zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "Interface[%s] state change to down.", ifp->name);
    }
    for (node = route_top (IF_OIFS (ifp)); node; node = route_next (node))
    {
        if ((oi = node->info) == NULL)
        {
            continue;
        }
        ospf_if_down (oi);
    }
    return 0;
}

int
ospf_interface_address_add (struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct connected *c = NULL;
    
    c = ospf_interface_address_read (ZEBRA_INTERFACE_ADDRESS_ADD, pevent, vrf_id);
    if (c == NULL)
    {
        return 0;
    }
    
    if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
    {
        char buf[128];
        prefix2str(c->address, buf, sizeof(buf));
        zlog_debug(OSPF_DBG_ZEBRA_INTERFACE, "interface %s address add %s", c->ifp->name, buf);
    }
    ospf_if_update (NULL, c->ifp);
#ifdef HAVE_SNMP
    ospf_snmp_if_update (c->ifp);
#endif /* HAVE_SNMP */
    return 0;
}

int
ospf_interface_address_delete (struct ifm_event pevent, vrf_id_t vrf_id)
{
    struct connected *c = NULL;
    struct interface *ifp = NULL;
    struct ospf_interface *oi = NULL;
    struct route_node *rn = NULL;
    struct prefix p;
    /* onnect address delete from ifp */
    c = ospf_interface_address_read (ZEBRA_INTERFACE_ADDRESS_DELETE, pevent, vrf_id);
    if (c == NULL)
    {
        return 0;
    }
    if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
    {
        char buf[128];
        prefix2str(c->address, buf, sizeof(buf));
        zlog_debug(OSPF_DBG_ZEBRA_INTERFACE, "interface %s address delete %s", c->ifp->name, buf);
    }
    ifp = c->ifp;
    p = *c->address;
    p.prefixlen = IPV4_MAX_PREFIXLEN;
    /* check the prefix in oifs ,if NULL, Free the connect */
    rn = route_node_lookup (IF_OIFS (ifp), &p);
    if (!rn)
    {
        connected_free (c);
        return 0;
    }
    assert (rn->info);
    oi = rn->info;
    route_unlock_node (rn);
    /* Call interface hook functions to clean up */
    ospf_if_free (oi);
	rn->info = NULL;
#ifdef HAVE_SNMP
    ospf_snmp_if_update (c->ifp);
#endif /* HAVE_SNMP */
    connected_free (c);
    return 0;
}

void
ospf_zebra_distance_update (struct prefix_ipv4 *p, struct ospf_route *or,struct ospf *ospf)
{
    struct ospf_path *path = NULL;
    struct listnode *node = NULL;
    int i = 0;
    struct route_entry route;
    struct routefifo *newroutefifoentry;
    memset(&route, 0 , sizeof(struct route_entry));
    /*add by tai*/
    route.prefix.type = INET_FAMILY_IPV4;
    route.prefix.addr.ipv4 = ntohl(p->prefix.s_addr);
    route.prefix.prefixlen = p->prefixlen;
    route.vpn = ospf->vpn;
    if(or->paths->count < NHP_ECMP_NUM)
    {
        route.nhp_num = or->paths->count;
    }
    else
    {
        route.nhp_num = NHP_ECMP_NUM;
    }
	
	if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
	{
    	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "%-15s[%d]:%s ",__FILE__, __LINE__,inet_ntoa(p->prefix));
	}
    for (ALL_LIST_ELEMENTS_RO (or->paths, node, path))
    {  
        route.nhp[i].nexthop.type = INET_FAMILY_IPV4;
        route.nhp[i].nexthop.addr.ipv4 = ntohl(path->nexthop.s_addr);
        route.nhp[i].ifindex = path->ifindex;
        route.nhp[i].distance = ospf_distance_apply (p, or,ospf);
        route.nhp[i].vpn = ospf->vpn;
        route.nhp[i].active = 0;
        route.nhp[i].nhp_type = NHP_TYPE_IP;
        route.nhp[i].action = NHP_ACTION_FORWARD;
        route.nhp[i].protocol = ROUTE_PROTO_OSPF;
        route.nhp[i].down_flag = 1;
        route.nhp[i].instance = ospf->ospf_id;
        if (or->path_type == OSPF_PATH_TYPE1_EXTERNAL)
        {
            route.nhp[i].cost =  (or->cost + or->u.ext.type2_cost);
        }
        else if (or->path_type == OSPF_PATH_TYPE2_EXTERNAL)
        {
            route.nhp[i].cost = (or->u.ext.type2_cost);
        }
        else
        {
            route.nhp[i].cost = (or->cost);
        }
		
		if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
		{
        	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "%-15s[%d]:nexthop:%s cost:%d:distance:%d",__FUNCTION__, __LINE__,
                   inet_ntoa(path->nexthop),route.nhp[i].cost,route.nhp[i].distance);
		}
        i++;
        if (i > NHP_ECMP_NUM -1)
        {
            break;
        }
    }
    
	do
	{		
		newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));	
		
		if (newroutefifoentry == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(newroutefifoentry == NULL);
    if (newroutefifoentry == NULL)
    {
        zlog_err("%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
        return;
    }
    memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
    newroutefifoentry->opcode = IPC_OPCODE_UPDATE;
    FIFO_ADD(&om->routefifo, &newroutefifoentry->fifo);

}


void
ospf_zebra_add (struct prefix_ipv4 *p, struct ospf_route *or,struct ospf *ospf)
{
    int i = 0;
    struct route_entry route;
    struct listnode *node = NULL;
    struct ospf_path *path = NULL;
    struct routefifo *newroutefifoentry;
	
    memset(&route, 0 , sizeof(struct route_entry));
    /*add by tai*/
    route.prefix.type = INET_FAMILY_IPV4;
    route.prefix.addr.ipv4 = ntohl(p->prefix.s_addr);
    route.prefix.prefixlen = p->prefixlen;
    route.vpn = ospf->vpn;
	
    if(or->paths->count < NHP_ECMP_NUM)
    {
        route.nhp_num = or->paths->count;
    }
    else
    {
        route.nhp_num = NHP_ECMP_NUM;
    }
	
	if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
	{
    	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "%-15s[%d]:%s ",__FILE__, __LINE__,inet_ntoa(p->prefix));
	}
    for (ALL_LIST_ELEMENTS_RO (or->paths, node, path))
    {
        route.nhp[i].nexthop.type = INET_FAMILY_IPV4;
        route.nhp[i].nexthop.addr.ipv4 = ntohl(path->nexthop.s_addr);
        route.nhp[i].ifindex = path->ifindex;
        route.nhp[i].distance = ospf_distance_apply (p, or,ospf);
        route.nhp[i].vpn = ospf->vpn;
        route.nhp[i].active = 0;
        route.nhp[i].nhp_type = NHP_TYPE_IP;
        route.nhp[i].action = NHP_ACTION_FORWARD;
        route.nhp[i].protocol = ROUTE_PROTO_OSPF;
        route.nhp[i].instance = ospf->ospf_id;
        route.nhp[i].down_flag = 1;
        if (or->path_type == OSPF_PATH_TYPE1_EXTERNAL)
        {
            route.nhp[i].cost =  (or->cost + or->u.ext.type2_cost);
        }
        else if (or->path_type == OSPF_PATH_TYPE2_EXTERNAL)
        {
            route.nhp[i].cost = (or->u.ext.type2_cost);
        }
        else
        {
            route.nhp[i].cost = (or->cost);
        }
		
		if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
		{
        	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "%-15s[%d]:nexthop:%s cost:%d,distance:%d",__FUNCTION__, __LINE__,
                   inet_ntoa(path->nexthop),route.nhp[i].cost,route.nhp[i].distance);
		}
        i++;
        if (i > NHP_ECMP_NUM -1)
        {
            break;
        }
    }
	
	do
	{		
		newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));	
		
		if (newroutefifoentry == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(newroutefifoentry == NULL);
    if (newroutefifoentry == NULL)
    {
        zlog_err("%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
        return;
    }
    memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
    newroutefifoentry->opcode = IPC_OPCODE_ADD;
    FIFO_ADD(&om->routefifo, &newroutefifoentry->fifo);
	
}

void
ospf_zebra_delete (struct prefix_ipv4 *p, struct ospf_route *or,struct ospf *ospf)
{
    struct ospf_path *path = NULL;
    struct listnode *node = NULL;
    int i = 0;
    struct route_entry route;
    struct routefifo *newroutefifoentry;
    memset(&route, 0 , sizeof(struct route_entry));
    /*add by tai*/
    route.prefix.type = INET_FAMILY_IPV4;
    route.prefix.addr.ipv4 = ntohl(p->prefix.s_addr);
    route.prefix.prefixlen = p->prefixlen;
    route.vpn = ospf->vpn;
    if(or->paths->count < NHP_ECMP_NUM)
    {
        route.nhp_num = or->paths->count;
    }
    else
    {
        route.nhp_num = NHP_ECMP_NUM;
    }
	
	if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
	{
    	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "%-15s[%d]:%s ",__FILE__, __LINE__,inet_ntoa(p->prefix));
	}
    for (ALL_LIST_ELEMENTS_RO (or->paths, node, path))
    { 
        route.nhp[i].nexthop.type = INET_FAMILY_IPV4;
        route.nhp[i].nexthop.addr.ipv4 = ntohl(path->nexthop.s_addr);
        route.nhp[i].ifindex = path->ifindex;
        route.nhp[i].distance = ospf_distance_apply (p, or,ospf);
        route.nhp[i].vpn = ospf->vpn;
        route.nhp[i].active = 0;
        route.nhp[i].nhp_type = NHP_TYPE_IP;
        route.nhp[i].action = NHP_ACTION_FORWARD;
        route.nhp[i].protocol = ROUTE_PROTO_OSPF;
        route.nhp[i].instance = ospf->ospf_id;
        route.nhp[i].down_flag = 1;
        if (or->path_type == OSPF_PATH_TYPE1_EXTERNAL)
        {
            route.nhp[i].cost = or->cost + or->u.ext.type2_cost;
        }
        else if (or->path_type == OSPF_PATH_TYPE2_EXTERNAL)
        {
            route.nhp[i].cost = or->u.ext.type2_cost;
        }
        else
        {
            route.nhp[i].cost = or->cost;
        }
		
		if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
		{
        	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "%-15s[%d]:nexthop:%s cost:%d,distance:%d",__FUNCTION__, __LINE__,
                   inet_ntoa(path->nexthop),route.nhp[i].cost,route.nhp[i].distance);
		}
		
        i++;
        if (i > NHP_ECMP_NUM -1)
        {
            break;
        }
    }
    
	do
	{		
		newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));	
		
		if (newroutefifoentry == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(newroutefifoentry == NULL);
    if (newroutefifoentry == NULL)
    {
        zlog_err("%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
        return;
    }
    memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
    newroutefifoentry->opcode = IPC_OPCODE_DELETE;
    FIFO_ADD(&om->routefifo, &newroutefifoentry->fifo);
	
}

int
ospf_is_type_redistributed (struct ospf *ospf, int type ,int instance)
{
    return (DEFAULT_ROUTE_TYPE (type)) ?
           vrf_bitmap_check (ospf->default_information, VRF_DEFAULT) : \
           vrf_bitmap_check (ospf->dmetric[type][instance].redist, VRF_DEFAULT);
}

int
ospf_redistribute_set (struct ospf *ospf, int source, int mtype, int mvalue, int instance)
{
    int force = 0;
    if(ROUTE_PROTO_OSPF == source  && instance == (int)ospf->ospf_id)
    {
        return -1;
    }

    
    if (ospf_is_type_redistributed (ospf, source, instance))
    {
        if (mtype != ospf->dmetric[source][instance].type)
        {
            ospf->dmetric[source][instance].type = mtype;
            force = LSA_REFRESH_FORCE;
        }
        if (mvalue != ospf->dmetric[source][instance].value)
        {
            ospf->dmetric[source][instance].value = mvalue;
            force = LSA_REFRESH_FORCE;
        }
        ospf_external_lsa_refresh_type (ospf, source, force, instance);
        ospf_schedule_asbr_task (ospf);
        if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
        {
            zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[%s]: Refresh  Type[%d:%d], Metric[%d]",
                        ospf_redist_string(source),instance,
                        metric_type (ospf, source,instance), metric_value (ospf, source, instance));
        }
        return CMD_SUCCESS;
    }
    ospf->dmetric[source][instance].type = mtype;
    ospf->dmetric[source][instance].value = mvalue;
    ospf_redistribute (ZEBRA_REDISTRIBUTE_ADD, ospf, source,instance, VRF_DEFAULT);
    if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
	{
        zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[%s]: Start  Type[%d:%d], Metric[%d]",
                    ospf_redist_string(source),instance,
                    metric_type (ospf, source,instance), metric_value (ospf, source,instance));
	}
    ospf_asbr_status_update (ospf, ++ospf->redistribute);
	
    return CMD_SUCCESS;
}

int
ospf_redistribute_unset (struct ospf *ospf, int source ,int instance)
{
    /* Don't redistibute ospf self instance route */
    if(ROUTE_PROTO_OSPF == source  && instance == (int)ospf->ospf_id)
    {
        return -1;
    }
    
    /* Check if the type already configured no redistribute */
    if (!ospf_is_type_redistributed (ospf, source, instance))
    {
        return CMD_SUCCESS;
    }
    if(((ROUTE_PROTO_CONNECT == source) && list_isempty(ospf->redist_name))
        ||(ROUTE_PROTO_CONNECT != source))
    {
        
        ospf_redistribute (ZEBRA_REDISTRIBUTE_DELETE, ospf, source, instance, VRF_DEFAULT);
        
        if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
        {
            zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[%s]: Stop",
                        ospf_redist_string(source));
        }
        
        ospf->dmetric[source][instance].type = -1;
        ospf->dmetric[source][instance].value = -1;
        
        /* Remove the routes from OSPF table. */
        ospf_redistribute_withdraw (ospf, source, instance);
        ospf_asbr_status_update (ospf, --ospf->redistribute);
    }
    ospf_schedule_asbr_task (ospf);
    return CMD_SUCCESS;
}

int
ospf_redistribute_connect_set (struct ospf *ospf, int source, int mtype, int mvalue, int instance)
{    
    if (ospf_is_type_redistributed (ospf, source, instance))
    {
        if (mtype != ospf->dmetric[source][instance].type)
        {
            ospf->dmetric[source][instance].type = mtype;
        }
		
        if (mvalue != ospf->dmetric[source][instance].value)
        {
            ospf->dmetric[source][instance].value = mvalue;
        }
        
        ospf_schedule_asbr_task (ospf);
        if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
        {
            zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[%s]: Refresh  Type[%d:%d], Metric[%d]",
                        ospf_redist_string(source),instance,
                        metric_type (ospf, source,instance), metric_value (ospf, source, instance));
        }
		
        return CMD_SUCCESS;
    }
	
    ospf->dmetric[source][instance].type = mtype;
    ospf->dmetric[source][instance].value = mvalue;
    ospf_redistribute (ZEBRA_REDISTRIBUTE_ADD, ospf, source,instance, VRF_DEFAULT);
	
    if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
    {
        zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[%s]: Start  Type[%d:%d], Metric[%d]",
                    ospf_redist_string(source),instance,
                    metric_type (ospf, source,instance), metric_value (ospf, source,instance));
    }
    ospf_asbr_status_update (ospf, ++ospf->redistribute);
    return CMD_SUCCESS;
}


int
ospf_redistribute_connect_unset (struct ospf *ospf, int source ,int instance)
{
    /* Don't redistibute ospf self instance route */
    
    /* Check if the type already configured no redistribute */
    if (!ospf_is_type_redistributed (ospf, source, instance))
    {
        return CMD_SUCCESS;
    }
    
    if(list_isempty(ospf->redist_name))
    {
        ospf_redistribute (ZEBRA_REDISTRIBUTE_DELETE, ospf, source, instance, VRF_DEFAULT);
        
        if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
        {
            zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[%s]: Stop",
                        ospf_redist_string(source));
        }
        
        ospf->dmetric[source][instance].type = -1;
        ospf->dmetric[source][instance].value = -1;
        
        /* Remove the routes from OSPF table. */
        //ospf_redistribute_withdraw (ospf, source, instance);
        ospf_asbr_status_update (ospf, --ospf->redistribute);
    }
    ospf_schedule_asbr_task (ospf);
    return CMD_SUCCESS;
}


int
ospf_redistribute_default_set (struct ospf *ospf, int originate,
                               int mtype, int mvalue)
{
    ospf->default_originate = originate;
    ospf->dmetric[DEFAULT_ROUTE][0].type = mtype;
    ospf->dmetric[DEFAULT_ROUTE][0].value = mvalue;
    if (ospf_is_type_redistributed (ospf,DEFAULT_ROUTE,ospf->ospf_id))
    {
        /* if ospf->default_originate changes value, is calling
        ospf_external_lsa_refresh_default sufficient to implement
         the change? */
        ospf_external_lsa_refresh_default (ospf);
        if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
            zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[%s]: Refresh  Type[%d], Metric[%d]",
                        ospf_redist_string(DEFAULT_ROUTE),
                        metric_type (ospf, DEFAULT_ROUTE,0),
                        metric_value (ospf, DEFAULT_ROUTE,0));
        return CMD_SUCCESS;
    }
    ospf_redistribute_default (ZEBRA_REDISTRIBUTE_DEFAULT_ADD, ospf,
                               VRF_DEFAULT);
    if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
        zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[DEFAULT]: Start  Type[%d], Metric[%d]",
                    metric_type (ospf, DEFAULT_ROUTE,0),
                    metric_value (ospf, DEFAULT_ROUTE,0));
    if (ospf->router_id.s_addr == 0)
    {
        ospf->external_origin |= (1 << DEFAULT_ROUTE);
    }
    else
    {
        //thread_add_timer (master_ospf, ospf_default_originate_timer, ospf, 1);
		high_pre_timer_add ((char *)"ospf_timer", LIB_TIMER_TYPE_NOLOOP, ospf_default_originate_timer, ospf, 1000);
    }
    ospf_asbr_status_update (ospf, ++ospf->redistribute);
    return CMD_SUCCESS;
}

int
ospf_redistribute_default_unset (struct ospf *ospf)
{
    if (!ospf_is_type_redistributed (ospf,DEFAULT_ROUTE,ospf->ospf_id))
    {
        return CMD_SUCCESS;
    }
    ospf->default_originate = DEFAULT_ORIGINATE_NONE;
    ospf->dmetric[DEFAULT_ROUTE][0].type = -1;
    ospf->dmetric[DEFAULT_ROUTE][0].value = -1;
    ospf_redistribute_default (ZEBRA_REDISTRIBUTE_DEFAULT_DELETE, ospf,
                               VRF_DEFAULT);
    if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
    {
        zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[DEFAULT]: Stop");
    }
    ospf_asbr_status_update (ospf, --ospf->redistribute);
    return CMD_SUCCESS;
}

static int
ospf_external_lsa_originate_check (struct ospf *ospf,
                                   struct external_info *ei)
{
    /* If prefix is multicast, then do not originate LSA. */
    if (IN_MULTICAST (htonl (ei->p.prefix.s_addr)))
    {
        zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type5:%s]: Not originate AS-external-LSA, "
                   "Prefix belongs multicast", inet_ntoa (ei->p.prefix));
        return 0;
    }
    /* Take care of default-originate. */
    if (is_prefix_default (&ei->p))
        if (ospf->default_originate == DEFAULT_ORIGINATE_NONE)
        {
            zlog_debug (OSPF_DBG_LSA_GENERATE, "LSA[Type5:0.0.0.0]: Not originate AS-external-LSA "
                       "for default");
            return 0;
        }
    return 1;
}

/* If connected prefix is OSPF enable interface, then do not announce. */
int
ospf_distribute_check_connected (struct ospf *ospf, struct external_info *ei)
{
    struct listnode *node = NULL;
    struct ospf_interface *oi = NULL;
    for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, node, oi))
    {
        if (prefix_match (oi->address, (struct prefix *) &ei->p))
        {
            return 0;
        }
    }
    return 1;
}

/* return 1 if external LSA must be originated, 0 otherwise */
int
ospf_redistribute_check (struct ospf *ospf,
                         struct external_info *ei, int *changed)
{
    struct route_map_set_values save_values;
    struct prefix_ipv4 *p = &ei->p;
    u_char type = is_prefix_default (&ei->p) ? DEFAULT_ROUTE : ei->type;
    if (changed)
    {
        *changed = 0;
    }
    if (!ospf_external_lsa_originate_check (ospf, ei))
    {
        return 0;
    }
    /* Take care connected route. */
    if (type == ROUTE_METRIC_CONNECT &&
            !ospf_distribute_check_connected (ospf, ei))
    {
        return 0;
    }
    if (!DEFAULT_ROUTE_TYPE (type) && DISTRIBUTE_NAME (ospf, type))
        /* distirbute-list exists, but access-list may not? */
        if (DISTRIBUTE_LIST (ospf, type))
            if (access_list_apply (DISTRIBUTE_LIST (ospf, type), p) == FILTER_DENY)
            {
                if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
                    zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[%s]: %s/%d filtered by ditribute-list.",
                                ospf_redist_string(type),
                                inet_ntoa (p->prefix), p->prefixlen);
                return 0;
            }
    save_values = ei->route_map_set;
    ospf_reset_route_map_set_values (&ei->route_map_set);
    /* apply route-map if needed */
    if (ROUTEMAP_NAME (ospf, type))
    {
        int ret;
        ret = route_map_apply (ROUTEMAP (ospf, type), (struct prefix *) p,
                               RMAP_OSPF, ei);
        if (ret == RMAP_DENYMATCH)
        {
            ei->route_map_set = save_values;
            if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
                zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[%s]: %s/%d filtered by route-map.",
                            ospf_redist_string(type),
                            inet_ntoa (p->prefix), p->prefixlen);
            return 0;
        }
        /* check if 'route-map set' changed something */
        if (changed)
            *changed = !ospf_route_map_set_compare (&ei->route_map_set,
                                                    &save_values);
    }
    return 1;
}

/* OSPF route-map set for redistribution */
void
ospf_routemap_set (struct ospf *ospf, int type, const char *name)
{
    if (ROUTEMAP_NAME (ospf, type))
    {
        free (ROUTEMAP_NAME (ospf, type));
    }
    ROUTEMAP_NAME (ospf, type) = strdup (name);
    ROUTEMAP (ospf, type) = route_map_lookup_by_name (name);
}

void
ospf_routemap_unset (struct ospf *ospf, int type)
{
    if (ROUTEMAP_NAME (ospf, type))
    {
        free (ROUTEMAP_NAME (ospf, type));
    }
    ROUTEMAP_NAME (ospf, type) = NULL;
    ROUTEMAP (ospf, type) = NULL;
}

static int
ospf_redistibute_external_info_process(int command,struct route_entry *p_route,struct prefix_ipv4 p,
                                       struct ase_ipv4 api, struct in_addr nexthop)
{
    if(om->redis_count[api.type] > 0)
    {
        if (command == IPC_OPCODE_ADD)
        {
            /* active route */
            if(p_route->nhp[0].active == 0)
            {
				if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
				{
                	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "type:%d,%s the route is inactive",api.type, inet_ntoa(p.prefix));
				}
                return -1;
            }
            /* do not redistribute connet route prefix lenth 32*/
            if(api.type == ROUTE_PROTO_CONNECT && p_route->prefix.prefixlen == IPV4_MAX_BITLEN)
            {
                if(!IFM_TYPE_IS_LOOPBCK(p_route->nhp[0].ifindex))
                {
					if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
					{
                    	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "Host route doesn't redistribute to ospf %s",inet_ntoa(p.prefix));
					}
                    return -1;
                }
            }
            ospf_external_info_add (api.vpn, api.type, api.instance, p, api.index, nexthop, api.tag);
        }
        else if(command == IPC_OPCODE_DELETE)   /* if (command == ZEBRA_IPV4_ROUTE_DELETE) */
        {
            if(EXTERNAL_INFO (api.vpn, api.type) == NULL)
            {
                return -1;
            }
            ospf_external_info_delete (api.vpn, api.type, api.instance, p, nexthop);
        }
    }
    return 0;
}




static int
ospf_redistibute_add_ipv4(struct prefix_ipv4 p, struct ase_ipv4 api, struct ospf *ospf, struct in_addr nexthop)
{
    struct ospf_lsa *current = NULL;
    struct external_info *ei = NULL;
	
    ei = ospf_external_info_lookup (api.vpn, api.type, api.instance, &p, nexthop);
    if(ei == NULL)
    {
        return -1;
    }
	
    if(ei->type == ROUTE_PROTO_OSPF && ei->instance == ospf->ospf_id)
    {
        return -1;
    }
	
    /*if do not register,do not manage it*/
    if(!ospf_is_type_redistributed (ospf, ei->type, ei->instance))
    {
		if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
		{
        	zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[%s(%d)]: %s/%d redistibute into ospf instance %d.",
                   ospf_redist_string(api.type),api.instance,
                   inet_ntoa (p.prefix), p.prefixlen,ospf->ospf_id);
		}
        return -1;
    }
	
    if (ospf->router_id.s_addr == 0)
    {
        /* Set flags to generate AS-external-LSA originate event
            for each redistributed protocols later. */
        ospf->external_origin |= (1 << api.type);
    }
    else
    {
        if (is_prefix_default (&p))
        {
            ospf_external_lsa_refresh_default (ospf);
        }
        else
        {                
            
            if((ei->type == ROUTE_PROTO_CONNECT && ospf->route_policy_flag == 1)
                    || ei->type != ROUTE_PROTO_CONNECT)
            {
                struct ospf_lsa *old;
                struct ospf_lsa *lsa_tmp;
				
                current = ospf_external_info_find_lsa (ospf, &ei->p);
                if (!current)
                {
                    ospf_external_lsa_originate (ospf, ei);
                }
                else if (IS_LSA_MAXAGE (current))
                {
                    ospf_external_lsa_refresh (ospf, current,
                                               ei, LSA_REFRESH_FORCE);
                }
                else
                {
                    struct as_external_lsa *al = (struct as_external_lsa *)current->data;
                    if (p.prefixlen < ip_masklen (al->mask))
                    {
                        ospf_external_lsa_refresh (ospf, current, ei, LSA_REFRESH_FORCE);
                    }
                    if(ospf->anyNSSA  && IS_OSPF_ABR(ospf))
                    {
                        /*  check the type 7 LSA self originate exist or not */
                        old = ospf_as_nssa_info_find_lsa(ospf, &ei->p);
                        if (old == NULL)
                        {
                            /* orignate type 7 LSA ,delete the new */
                            ospf_nssa_lsa_orignate_only(ospf, ei);
                            lsa_tmp = ospf_find_nssa_lsa_highestprio(ospf, &current->data->id);
                            if (lsa_tmp)
                            {
                                if (IS_LSA_SELF(lsa_tmp))
                                {
                                    /*refresh Type-7 LSA only*/
                                    ospf_external_lsa_refresh_only(ospf, current, ei, LSA_REFRESH_FORCE);
                                }
                                /*  Choose the 7->5 LSA ,no need to refresh*/
                                else
                                {
									if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
                                	{
                                    	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "%s[%d] Choose the 7->5 LSA ,no need to refresh",__func__,__LINE__);
                                	}
                                    if(!CHECK_FLAG (current->flags, OSPF_LSA_LOCAL_XLT))
                                    {
                                        ospf_abr_nssa_translate_type7totype5(lsa_tmp->area, lsa_tmp);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                    	if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
                    	{
                        	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "%s() : %s already exists",__func__,
                                   inet_ntoa (p.prefix));
                    	}
                    }
                    ospf_external_lsa_fw_update(ospf);
                }
            }
            else
            {
                if(ospf_route_policy_look(ospf,ei->ifindex) != NULL)
                {
                	if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
					{
						zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "OSPF configure route-policy::");
					}
                    struct ospf_lsa *old;
                    struct ospf_lsa *lsa_tmp;
                    current = ospf_external_info_find_lsa (ospf, &ei->p);
                    if (!current)
                    {
                        ospf_external_lsa_originate (ospf, ei);
                    }
                    else if (IS_LSA_MAXAGE (current))
                    {
                        ospf_external_lsa_refresh (ospf, current,
                                                   ei, LSA_REFRESH_FORCE);
                    }
                    else
                    {
                        struct as_external_lsa *al = (struct as_external_lsa *)current->data;
                        if (p.prefixlen < ip_masklen (al->mask))
                        {
                            ospf_external_lsa_refresh (ospf, current, ei, LSA_REFRESH_FORCE);
                        }
                        if(ospf->anyNSSA  && IS_OSPF_ABR(ospf))
                        {
                            /*  check the type 7 LSA self originate exist or not */
                            old = ospf_as_nssa_info_find_lsa(ospf, &ei->p);
                            if (old == NULL)
                            {
                                /* orignate type 7 LSA ,delete the new */
                                ospf_nssa_lsa_orignate_only(ospf, ei);
                                lsa_tmp = ospf_find_nssa_lsa_highestprio(ospf, &current->data->id);
                                if (lsa_tmp)
                                {
                                    if (IS_LSA_SELF(lsa_tmp))
                                    {
                                        /*refresh Type-7 LSA only*/
                                        ospf_external_lsa_refresh_only(ospf, current, ei, LSA_REFRESH_FORCE);
                                    }
                                    /*  Choose the 7->5 LSA ,no need to refresh*/
                                    else
                                    {
										if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
										{
                                        	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "%s[%d] Choose the 7->5 LSA ,no need to refresh",__func__,__LINE__);
										}
                                        if(!CHECK_FLAG (current->flags, OSPF_LSA_LOCAL_XLT))
                                        {
                                            ospf_abr_nssa_translate_type7totype5(lsa_tmp->area, lsa_tmp);
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
							if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
							{
                            	zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "%s() : %s already exists",__func__,
                                       inet_ntoa (p.prefix));
							}
                        }
                        ospf_external_lsa_fw_update(ospf);
                    }
            }
            }
        }
        
    }
    return 0;
}
static int
ospf_redistibute_del_ipv4(struct prefix_ipv4 p, struct ase_ipv4 api, struct ospf *ospf, struct in_addr nexthop)
{
    struct external_info *ei = NULL;
    ei = ospf_external_info_lookup_by_prefix (ospf, &p, nexthop);
    if(ei == NULL)
    {
        if (is_prefix_default (&p))
        {
            ospf_external_lsa_refresh_default (ospf);
        }
        else
        {
            ospf_external_lsa_flush (ospf, api.type, &p, 0 /*, nexthop */);
        }
    }
    else
    {
        ospf_external_lsa_fw_update(ospf);
    }
    return 0;
}





/* Zebra route add and delete treatment. */
int
ospf_route_read_ipv4 (int command, struct route_entry *p_route,
                      int length, vrf_id_t vrf_id)
{
    struct ase_ipv4 api;
    struct in_addr nexthop;
    struct prefix_ipv4 p;
    struct ospf *top = NULL;
    struct listnode *node = NULL;
    unsigned char plength = 0;
    int ret = -1;

    if(!p_route || p_route->prefix.type != INET_FAMILY_IPV4 || p_route->nhp[0].nexthop.type != INET_FAMILY_IPV4)
	{
		return 0;
	}
	
	if (p_route->vpn > OSPF_MAX_VRF_ID)
	{
		return 0;
	}
    
    if (listcount (om->ospf) == 0)
    {
        return 0;
    }
    
    nexthop.s_addr = 0;
    /* Type, flags, message. */
    memset (&api, 0, sizeof (struct ase_ipv4));
    api.type = p_route->nhp[0].protocol;
    api.flags = p_route->nhp[0].action;
	api.vpn = p_route->vpn;
    api.message = ZAPI_MESSAGE_NEXTHOP;/*??????*/
    
    /* IPv4 prefix. */
    memset (&p, 0, sizeof (struct prefix_ipv4));
    p.family = AF_INET;
    plength = p_route->prefix.prefixlen;
    p.prefixlen = MIN(IPV4_MAX_PREFIXLEN, plength);
    p.prefix.s_addr = htonl(p_route->prefix.addr.ipv4);
    if (IPV4_NET127(p_route->prefix.addr.ipv4))
    {
        return 0;
    }
    
    /* Nexthop, ifindex, distance, metric. */
    if(p_route->nhp[0].action != NHP_ACTION_DROP)
    {
        if(p_route->nhp[0].nexthop.addr.ipv4 != 0)
        {
            api.nexthop_num = p_route->nhp_num;
            nexthop.s_addr = htonl(p_route->nhp[0].nexthop.addr.ipv4);
            if(p_route->nhp[0].nexthop.type != INET_FAMILY_IPV4)   
                return 0;
            SET_FLAG(api.message, ZAPI_MESSAGE_NEXTHOP);
        }
        api.ifindex_num = p_route->nhp_num;
        api.index = p_route->nhp[0].ifindex;
    }
    api.distance = p_route->nhp[0].distance;
    api.metric = (int)p_route->nhp[0].cost;
    api.instance = p_route->nhp[0].instance;
    api.nhp_type = p_route->nhp[0].nhp_type;
    
    /* external info check add or delete*/
    ret = ospf_redistibute_external_info_process(command, p_route, p, api, nexthop);
    if(ret == -1)
    {
        return 0;
    }
    for (ALL_LIST_ELEMENTS_RO (om->ospf, node, top))
    {
    	if (top->vpn != api.vpn)
		{
			continue;
		}
		
        if (vrf_bitmap_check (top->dmetric[api.type][api.instance].redist, VRF_DEFAULT))
        {
            if (command == IPC_OPCODE_ADD)
            {
                if(api.type == ROUTE_PROTO_OSPF && api.instance == top->ospf_id)
                {
                    continue;
                }
				
				if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
				{
                	zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[%s(%d)] add %s/%d\n", ospf_redist_string(api.type),
                             api.instance,inet_ntoa(p.prefix),p.prefixlen);
				}
                ret = ospf_redistibute_add_ipv4(p,api,top, nexthop);
                if (ret == -1)
                {
                    continue;
                }
            }
            else if(command == IPC_OPCODE_DELETE)   /* if (command == ZEBRA_IPV4_ROUTE_DELETE) */
            {    
				if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
				{
                	zlog_debug (OSPF_DBG_ZEBRA_REDISTRIBUTE, "Redistribute[%s(%d)] delete %s/%d\n", ospf_redist_string(api.type),
                             api.instance,inet_ntoa(p.prefix),p.prefixlen);
				}
                ret = ospf_redistibute_del_ipv4(p,api,top, nexthop);
                if (ret == -1)
                {
                    continue;
                }
            }
            ospf_schedule_asbr_task (top);
        }
    }
    return 0;
}


int
ospf_distribute_list_out_set (struct ospf *ospf, int type, const char *name)
{
    /* Lookup access-list for distribute-list. */
    DISTRIBUTE_LIST (ospf, type) = access_list_lookup (AFI_IP, name);
    /* Clear previous distribute-name. */
    if (DISTRIBUTE_NAME (ospf, type))
    {
        free (DISTRIBUTE_NAME (ospf, type));
    }
    /* Set distribute-name. */
    DISTRIBUTE_NAME (ospf, type) = strdup (name);
    /* If access-list have been set, schedule update timer. */
    if (DISTRIBUTE_LIST (ospf, type))
    {
        ospf_distribute_list_update (ospf, type);
    }
    return CMD_SUCCESS;
}

int
ospf_distribute_list_out_unset (struct ospf *ospf, int type, const char *name)
{
    /* Schedule update timer. */
    if (DISTRIBUTE_LIST (ospf, type))
    {
        ospf_distribute_list_update (ospf, type);
    }
    /* Unset distribute-list. */
    DISTRIBUTE_LIST (ospf, type) = NULL;
    /* Clear distribute-name. */
    if (DISTRIBUTE_NAME (ospf, type))
    {
        free (DISTRIBUTE_NAME (ospf, type));
    }
    DISTRIBUTE_NAME (ospf, type) = NULL;
    return CMD_SUCCESS;
}

/* distribute-list update timer. */
static int
ospf_distribute_list_update_timer (void *thread)
{
    struct route_node *rn = NULL;
    struct external_info *ei = NULL;
    struct route_table *rt = NULL;
    struct ospf_lsa *lsa = NULL;
    int type, default_refresh = 0;
    struct ospf *ospf = NULL;
    u_int16_t ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;
    ospf = ospf_lookup_id(ospf_id);
    if (ospf == NULL)
    {
        return 0;
    }
    ospf->t_distribute_update = 0;
    zlog_debug (OSPF_DBG_OTHER, "[Redistribute]: distribute-list update timer fired!");
    /* foreach all external info. */
    for (type = 0; type <= ROUTE_PROTO_MAX; type++)
    {
        rt = EXTERNAL_INFO (ospf->vpn, type);
        if (!rt)
        {
            continue;
        }
        for (rn = route_top (rt); rn; rn = route_next (rn))
        {
            if ((ei = rn->info) != NULL)
            {
                if (is_prefix_default (&ei->p))
                {
                    default_refresh = 1;
                }
                else if ((lsa = ospf_external_info_find_lsa (ospf, &ei->p)))
                {
                    ospf_external_lsa_refresh (ospf, lsa, ei, LSA_REFRESH_IF_CHANGED);
                }
                else
                {
                    ospf_external_lsa_originate (ospf, ei);
                }
            }
        }
    }
    if (default_refresh)
    {
        ospf_external_lsa_refresh_default (ospf);
    }
    return 0;
}

/* Update distribute-list and set timer to apply access-list. */
void
ospf_distribute_list_update (struct ospf *ospf, uintptr_t type)
{
    //struct route_table *rt = NULL;
    /* External info does not exist. */
    //if (!(rt = EXTERNAL_INFO (ospf->vpn, type)))
    if (!(EXTERNAL_INFO (ospf->vpn, type)))
    {
        return;
    }
    /* If exists previously invoked thread, then let it continue. */
    if (ospf->t_distribute_update)
    {
        return;
    }
    /* Set timer. */
    /*ospf->t_distribute_update =
        thread_add_timer_msec (master_ospf, ospf_distribute_list_update_timer,
                               (void *) type, ospf->min_ls_interval);*/

	ospf->t_distribute_update = high_pre_timer_add ((char *)"ospf_timer", LIB_TIMER_TYPE_NOLOOP,\
							ospf_distribute_list_update_timer, (void *) type, ospf->min_ls_interval);
}

/* If access-list is updated, apply some check. */
static void
ospf_filter_update (struct access_list *access_t)
{    
	int type;
    int abr_inv = 0;
    u_int16_t ospf_id;
    struct ospf *ospf = NULL;
    struct ospf_area *area = NULL;
    struct listnode *node = NULL;
	
    ospf_id = OSPF_DEFAULT_PROCESS_ID;
    /* If OSPF instatnce does not exist, return right now. */
    ospf = ospf_lookup_id(ospf_id);
    if (ospf == NULL)
    {
        return;
    }
    /* Update distribute-list, and apply filter. */
    for (type = 0; type <= ROUTE_PROTO_MAX; type++)
    {
        if (ROUTEMAP (ospf, type) != NULL)
        {
            /* if route-map is not NULL it may be using this access list */
            ospf_distribute_list_update (ospf, type);
            continue;
        }
        /* There is place for route-map for default-information (ROUTE_PROTO_MAX),
            * but no distribute list. */
        if (type == ROUTE_PROTO_MAX)
        {
            break;
        }
        if (DISTRIBUTE_NAME (ospf, type))
        {
            /* Keep old access-list for distribute-list. */
            struct access_list *old = DISTRIBUTE_LIST (ospf, type);
            /* Update access-list for distribute-list. */
            DISTRIBUTE_LIST (ospf, type) =
                access_list_lookup (AFI_IP, DISTRIBUTE_NAME (ospf, type));
            /* No update for this distribute type. */
            if (old == NULL && DISTRIBUTE_LIST (ospf, type) == NULL)
            {
                continue;
            }
            /* Schedule distribute-list update timer. */
            if (DISTRIBUTE_LIST (ospf, type) == NULL ||
                    strcmp (DISTRIBUTE_NAME (ospf, type), access_t->name) == 0)
            {
                ospf_distribute_list_update (ospf, type);
            }
        }
    }
    /* Update Area access-list. */
    for (ALL_LIST_ELEMENTS_RO (ospf->areas, node, area))
    {
        if (EXPORT_NAME (area))
        {
            EXPORT_LIST (area) = NULL;
            abr_inv++;
        }
        if (IMPORT_NAME (area))
        {
            IMPORT_LIST (area) = NULL;
            abr_inv++;
        }
    }
    /* Schedule ABR tasks -- this will be changed -- takada. */
    if (IS_OSPF_ABR (ospf) && abr_inv)
    {
        ospf_schedule_abr_task (ospf);
    }
}

/* If prefix-list is updated, do some updates. */
void
ospf_prefix_list_update (struct prefix_list *plist)
{
    struct ospf *ospf = NULL;
    int type;
    int abr_inv = 0;
    struct ospf_area *area = NULL;
    struct listnode *node = NULL;
    u_int16_t ospf_id;
    ospf_id = OSPF_DEFAULT_PROCESS_ID;
    /* If OSPF instatnce does not exist, return right now. */
    ospf = ospf_lookup_id(ospf_id);
    if (ospf == NULL)
    {
        return;
    }
    /* Update all route-maps which are used as redistribution filters.
    * They might use prefix-list.
    */
    for (type = 0; type <= ROUTE_PROTO_MAX; type++)
    {
        if (ROUTEMAP (ospf, type) != NULL)
        {
            /* If route-map is not NULL it may be using this prefix list */
            ospf_distribute_list_update (ospf, type);
            continue;
        }
    }
    /* Update area filter-lists. */
    for (ALL_LIST_ELEMENTS_RO (ospf->areas, node, area))
    {
        /* Update filter-list in. */
        if (PREFIX_NAME_IN (area))
            if (strcmp (PREFIX_NAME_IN (area), prefix_list_name (plist)) == 0)
            {
                PREFIX_LIST_IN (area) =
                    prefix_list_lookup (AFI_IP, PREFIX_NAME_IN (area));
                abr_inv++;
            }
        /* Update filter-list out. */
        if (PREFIX_NAME_OUT (area))
            if (strcmp (PREFIX_NAME_OUT (area), prefix_list_name (plist)) == 0)
            {
                PREFIX_LIST_IN (area) =
                    prefix_list_lookup (AFI_IP, PREFIX_NAME_OUT (area));
                abr_inv++;
            }
    }
    /* Schedule ABR task. */
    if (IS_OSPF_ABR (ospf) && abr_inv)
    {
        ospf_schedule_abr_task (ospf);
    }
}

static struct ospf_distance *
ospf_distance_new (void)
{	
	struct ospf_distance *ospf_distance = NULL;

	do
	{		
		ospf_distance = XCALLOC (MTYPE_OSPF_DISTANCE, sizeof (struct ospf_distance));	
		
		if (ospf_distance == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(ospf_distance == NULL);
    return ospf_distance;
}

static void
ospf_distance_free (struct ospf_distance *odistance)
{
    XFREE (MTYPE_OSPF_DISTANCE, odistance);
}

int
ospf_distance_set (struct vty *vty, struct ospf *ospf,
                   const char *distance_str,
                   const char *ip_str,
                   const char *access_list_str)
{
    int ret;
    struct prefix_ipv4 p;
    u_char distance;
    struct route_node *rn = NULL;
    struct ospf_distance *odistance = NULL;
    ret = str2prefix_ipv4 (ip_str, &p);
    if (ret == 0)
    {
        vty_error_out (vty, "Malformed prefix%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    distance = atoi (distance_str);
    /* Get OSPF distance node. */
    rn = route_node_get (ospf->distance_table, (struct prefix *) &p);
    if (rn->info)
    {
        odistance = rn->info;
        route_unlock_node (rn);
    }
    else
    {
        odistance = ospf_distance_new ();
		if(odistance == NULL)
		{
			zlog_err("%s %d Can't creat ospf_distance_new : malloc failed",__func__,__LINE__);
			return 0;
		}
		memset(odistance, 0, sizeof(struct ospf_distance));
			
        rn->info = odistance;
    }
    /* Set distance value. */
    odistance->distance = distance;
    /* Reset access-list configuration. */
    if (odistance->access_list)
    {
        free (odistance->access_list);
        odistance->access_list = NULL;
    }
    if (access_list_str)
    {
        odistance->access_list = strdup (access_list_str);
    }
    return CMD_SUCCESS;
}

int
ospf_distance_unset (struct vty *vty, struct ospf *ospf,
                     const char *distance_str,
                     const char *ip_str, char
                     const *access_list_str)
{
    int ret;
    struct prefix_ipv4 p;
    struct route_node *rn = NULL;
    struct ospf_distance *odistance = NULL;
    ret = str2prefix_ipv4 (ip_str, &p);
    if (ret == 0)
    {
        vty_error_out (vty, "Malformed prefix%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    rn = route_node_lookup (ospf->distance_table, (struct prefix *) &p);
    if (!rn)
    {
        vty_error_out (vty, "Can't find specified prefix%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    odistance = rn->info;
    if (odistance->access_list)
    {
        free (odistance->access_list);
    }
    ospf_distance_free (odistance);
    rn->info = NULL;
    route_unlock_node (rn);
    route_unlock_node (rn);
    return CMD_SUCCESS;
}

void
ospf_distance_reset (struct ospf *ospf)
{
    struct route_node *rn = NULL;
    struct ospf_distance *odistance = NULL;
    for (rn = route_top (ospf->distance_table); rn; rn = route_next (rn))
    {
        if ((odistance = rn->info) != NULL)
        {
            if (odistance->access_list)
            {
                free (odistance->access_list);
            }
            ospf_distance_free (odistance);
            rn->info = NULL;
            route_unlock_node (rn);
        }
    }
}

u_char
ospf_distance_apply (struct prefix_ipv4 *p, struct ospf_route *or,struct ospf *ospf)
{
    if (ospf == NULL)
    {
        return 0;
    }
    if (ospf->distance_intra)
        if (or->path_type == OSPF_PATH_INTRA_AREA)
        {
            return ospf->distance_intra;
        }
    if (ospf->distance_inter)
        if (or->path_type == OSPF_PATH_INTER_AREA)
        {
            return ospf->distance_inter;
        }
    if (ospf->distance_external)
        if (or->path_type == OSPF_PATH_TYPE1_EXTERNAL
                || or->path_type == OSPF_PATH_TYPE2_EXTERNAL)
        {
            return ospf->distance_external;
        }
    if (ospf->distance_all)
    {
        return ospf->distance_all;
    }
    return 0;
}


void
ospf_buf_init (void)
{
    /* Allocate zebra structure. */
    access_list_add_hook (ospf_filter_update);
    access_list_delete_hook (ospf_filter_update);
    prefix_list_add_hook (ospf_prefix_list_update);
    prefix_list_delete_hook (ospf_prefix_list_update);
}


