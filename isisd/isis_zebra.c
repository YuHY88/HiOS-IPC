/*
 * IS-IS Rout(e)ing protocol - isis_zebra.c
 *
 * Copyright (C) 2001,2002   Sampo Saaristo
 *                           Tampere University of Technology
 *                           Institute of Communications Engineering
 * Copyright (C) 2013-2015   Christian Franke <chris@opensourcerouting.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public Licenseas published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <zebra.h>

#include "thread.h"
#include "command.h"
#include "memory.h"
#include "log.h"
#include "if.h"
#include "network.h"
#include "prefix.h"
#include "stream.h"
#include "linklist.h"
#include "vrf.h"

#include "isisd/dict.h"
#include "isisd/isis_constants.h"
#include "isisd/isis_common.h"
#include "isisd/isis_flags.h"
#include "isisd/isis_misc.h"
#include "isisd/isis_circuit.h"
#include "isisd/isis_tlv.h"
#include "isisd/isisd.h"
#include "isisd/isis_circuit.h"
#include "isisd/isis_csm.h"
#include "isisd/isis_lsp.h"
#include "isisd/isis_route.h"
#include "isisd/isis_zebra.h"
#include "ifm_common.h"
#include "route/route.h"
#include "isisd/isis_ipc.h"
#include "isisd/isis_adjacency.h"
#include "isisd/isis_pdu.h"

extern struct isis_statistics isis_sta;
extern char * inet_ntoa(struct in_addr in);

void
isis_interface_if_set_value (struct ifm_event *pevent, struct interface *ifp, char updflag)
{
	uint16_t  mtu ;
   	uchar pmac[6] ;

	/* Read interface's index. */
	ifp->ifindex = pevent->ifindex;
	/* Read interface's value. */

	/* set ifp link*/
	if(updflag)
	{
		ifp->status = 0;
		SET_FLAG(ifp->flags, IFF_UP);
		SET_FLAG(ifp->flags, IFF_RUNNING);
		SET_FLAG(ifp->flags, IFF_BROADCAST);
	}
	else
	{
		ifp->status = 1;
		ifp->flags = 0;
	}

	/* get the mac */
	if(0 != ifm_get_mac(pevent->ifindex, MODULE_ID_ISIS,pmac))
	{
		zlog_err("%-15s[%d]: ISIS get ifindex %0x mac time out", __FUNCTION__, __LINE__, pevent->ifindex);
	}
	
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


	/* get mtu */
	if(0 != ifm_get_mtu(pevent->ifindex, MODULE_ID_ISIS,&mtu))
	{
		zlog_err("%-15s[%d]: ISIS get ifindex %0x mtu time out", __FUNCTION__, __LINE__, pevent->ifindex);
	}
	
	if(mtu != 0)
	{
		ifp->mtu = mtu;
	}
	else
	{
		ifp->mtu = 1500;
	}

	ifp->ll_type = ZEBRA_LLT_ETHER;
}

static int
isis_memconstant(const void *s, int c, size_t n)
{
  const u_char *p = s;

  while (n-- > 0)
    if (*p++ != c)
      return 0;
  return 1;
}

static void
isis_stream_get_prefix (struct ifm_event *pevent, struct prefix *p)
{
  size_t plen = prefix_blen (p);
  u_char c;
  p->prefixlen = 0;

  if (plen == 0)
    return;
    
  memcpy (&p->u.prefix, &(pevent->ipaddr.addr), plen);
  c = pevent->ipaddr.prefixlen;
  p->prefixlen = MIN(plen * 8, c);
}

struct interface *
ifm_interface_state_read (struct ifm_event *pevent, vrf_id_t vrf_id,u_int8_t stat_flag)
{
  struct interface *ifp;
  char ifname_tmp[INTERFACE_NAMSIZ];

  /* Get interface name. */
  ifm_get_name_by_ifindex(pevent->ifindex, ifname_tmp);

  /* Lookup this by interface index. */
  ifp = if_lookup_by_name_len_vrf (ifname_tmp,
                                   strnlen (ifname_tmp, INTERFACE_NAMSIZ),
                                   vrf_id);
  
  if (ifp == NULL)
     return NULL;

  isis_interface_if_set_value (pevent, ifp,stat_flag);

  return ifp;
}

void
isis_route_info_delete_old (struct isis_route_info *route_info)
{  
    if (route_info->old_nexthops)
    {
        route_info->old_nexthops->del = (void (*)(void *)) isis_nexthop_delete;

        if (route_info->old_nexthops->count > 0)
        {
             list_delete_all_node (route_info->old_nexthops);
        }
    }
}

void
isis_route_info_delete_ipv6_old (struct isis_route_info *route_info)
{  
    if (route_info->old_nexthops6)
    {
        route_info->old_nexthops6->del = (void (*)(void *)) isis_nexthop6_delete;

        if (route_info->old_nexthops6->count > 0)
        {
             list_delete_all_node (route_info->old_nexthops6);
        }
    }
}


struct connected *
ifm_interface_address_read (int type, struct ifm_event *pevent, vrf_id_t vrf_id)
{
  ifindex_t ifindex;
  struct interface *ifp;
  struct connected *ifc;
  struct prefix p, d, *dp;
  int plen;
  u_char ifc_flags;
  char ifname_tmp[INTERFACE_NAMSIZ];
  struct isis_circuit *circuit;
  struct listnode *node;
  struct prefix_ipv4 *ipv4;
  struct prefix_ipv6 *ipv6;

  memset (&p, 0, sizeof(p));
  memset (&d, 0, sizeof(d));

  /* Get interface index. */
  ifindex = pevent->ifindex;

  /* Get interface name. */
  ifm_get_name_by_ifindex(pevent->ifindex, ifname_tmp);

  /* Lookup this by interface index. */
  ifp = if_lookup_by_name_len_vrf (ifname_tmp,
                                   strnlen (ifname_tmp, INTERFACE_NAMSIZ),
                                   vrf_id);

  if (ifp == NULL)
  {
      zlog_debug (ISIS_DEBUG_TYPE_MSG,"zebra_interface_address_read(%s): "
                 "Can't find interface by ifindex: %d ",
                 (type == ZEBRA_INTERFACE_ADDRESS_ADD? "ADD" : "DELETE"),
                 ifindex);
      return NULL;
  }

  /* Fetch flag. */
  ifc_flags = 0x00;
  
  /* Fetch interface address. */
  if (pevent->ipaddr.type == INET_FAMILY_IPV4)
  {
  	d.family = p.family = AF_INET;
  }
  else if(pevent->ipaddr.type == INET_FAMILY_IPV6)
  {
  	d.family = p.family = AF_INET6;
  }
  plen = prefix_blen (&d);

  isis_stream_get_prefix (pevent, &p);

  /* Fetch destination address. */
  //stream_get (&d.u.prefix, s, plen);

  /* N.B. NULL destination pointers are encoded as all zeroes */
  dp = isis_memconstant(&d.u.prefix,0,plen) ? NULL : &d;

  if (type == ZEBRA_INTERFACE_ADDRESS_ADD)
    {
       /* N.B. NULL destination pointers are encoded as all zeroes */
	   circuit = circuit_scan_by_ifp (ifp);
	   if(circuit != NULL)
	   {
	     if(p.family == AF_INET)
	     {
	       for (ALL_LIST_ELEMENTS_RO (circuit->ip_addrs, node, ipv4))
           {
      	     if (prefix_same ((struct prefix *) ipv4, &p))
               return NULL;
           }
	     }

		 if(p.family == AF_INET6)
		 {
		   for (ALL_LIST_ELEMENTS_RO (circuit->ipv6_link, node, ipv6))
		   {
		     if (prefix_same ((struct prefix *) ipv6, &p))
				 return NULL;
		   }
			 
		   for (ALL_LIST_ELEMENTS_RO (circuit->ipv6_non_link, node, ipv6))
		   {
			 if (prefix_same ((struct prefix *) ipv6, &p))
				 return NULL;
		   }

		 }
	   }

	   
       ifc = connected_add_by_prefix(ifp, &p, dp);
       if (ifc != NULL)
	   {
		   /* slave ip address could not config isis */
		   /* Fetch flag. */
		   if(pevent->ipflag == IP_TYPE_SLAVE)
		   {
			   SET_FLAG(ifc->flags, ZEBRA_IFA_SLAVE);
		   }
		   else if (pevent->ipflag == IP_TYPE_STATIC)
		   {
			   SET_FLAG(ifc->flags, ZEBRA_IFA_SECONDARY);
		   }
		   else if (pevent->ipflag == IP_TYPE_DHCP)
		   {
			   SET_FLAG(ifc->flags, ZEBRA_IFA_DHCP);
		   }
		   else if (pevent->ipflag == IP_TYPE_UNNUMBERED)
		   {
			   SET_FLAG(ifc->flags, ZEBRA_IFA_UNNUMBERED);
		   }
		   else
		   {
			   ifc->flags = ifc_flags;
			   zlog_debug(ISIS_DEBUG_TYPE_MSG,"warnig: ip flag error");
		   }
	   
	     if (ifc->destination)
	       ifc->destination->prefixlen = ifc->address->prefixlen;
		 
	     else if (CHECK_FLAG(ifc->flags, ZEBRA_IFA_PEER))
	     {
	       /* carp interfaces on OpenBSD with 0.0.0.0/0 as "peer" */
	       char buf[PREFIX_STRLEN];
	       zlog_debug(ISIS_DEBUG_TYPE_MSG,"warning: interface %s address %s "
		    "with peer flag set, but no peer address!",
		    ifp->name,
		    prefix2str (ifc->address, buf, sizeof buf));
	       UNSET_FLAG(ifc->flags, ZEBRA_IFA_PEER);
	     }
	   }
    }
  else
    {
      //assert (type == ZEBRA_INTERFACE_ADDRESS_DELETE);
	  if(type != ZEBRA_INTERFACE_ADDRESS_DELETE)
	  {
		  zlog_err("%-20s[%d] error: ",__FUNCTION__ ,  __LINE__);
		  return NULL;
	  }

      ifc = connected_delete_by_prefix(ifp, &p);
    }

  return ifc;
}

/* Router-id update message from zebra. */
static int
isis_router_id_update_zebra ()
{
#if 1
  struct isis_area *area;
  struct listnode *node;

  isis->router_id = 0x01020304;
  for (ALL_LIST_ELEMENTS_RO (isis->area_list, node, area))
    if (listcount (area->area_addrs) > 0)
      lsp_regenerate_schedule (area, area->is_type, 0);
#endif
  return 0;
}
int isis_interface_mtu_change (struct ifm_event *pevent, vrf_id_t vrf_id)
{
	struct interface *ifp = NULL;
	char ifname_tmp[INTERFACE_NAMSIZ];
	struct isis_circuit *circuit;
	
    ifm_get_name_by_ifindex(pevent->ifindex, ifname_tmp);

    /* Lookup/create interface by name. */
    ifp = if_get_by_name_len_vrf (ifname_tmp,
                                  strnlen (ifname_tmp, INTERFACE_NAMSIZ),
                                  vrf_id);

    
    if(ifp != NULL)
    {   
        ifp->mtu = pevent->mtu;
    }
	circuit = circuit_scan_by_ifp (ifp);
	if(circuit != NULL )
	{
	    if(if_is_operative(ifp))
	    {
	      if(circuit->state == C_STATE_UP)
	      {
	        isis_csm_state_change (IF_DOWN_FROM_Z, circuit, ifp);
	        isis_csm_state_change (IF_UP_FROM_Z, circuit, ifp);
	      }
		  else
		  {
		    isis_csm_state_change (IF_UP_FROM_Z, circuit, ifp);
		  }
	    }
	}
    return 0;
}

int
isis_zebra_if_state_up (struct ifm_event *pevent, vrf_id_t vrf_id)
{
  struct interface *ifp;
  u_int8_t stat_flag = 1;

  ifp = ifm_interface_state_read (pevent, vrf_id,stat_flag);

  if (ifp == NULL)
    return 0;

  isis_csm_state_change (IF_UP_FROM_Z, circuit_scan_by_ifp (ifp), ifp);
  return 0;
}

int
isis_zebra_if_del (struct ifm_event *pevent, vrf_id_t vrf_id)
{
  struct interface *ifp;
  struct isis_circuit *circuit;
  char ifname_tmp[INTERFACE_NAMSIZ];

  /* Get interface name. */
  ifm_get_name_by_ifindex(pevent->ifindex, ifname_tmp);

  /* Lookup this by interface index. */
  ifp = if_lookup_by_name_len_vrf (ifname_tmp,
                                   strnlen (ifname_tmp, INTERFACE_NAMSIZ),
                                   vrf_id);

  /* If such interface does not exist, indicate an error */

  if (ifp == NULL)
     return 0;

  circuit = circuit_scan_by_ifp(ifp);

  if(circuit == NULL)
  {
  	if_delete(ifp);
  	return 0;
  }

  if(circuit->state != C_STATE_NA)
  {
  	isis_csm_state_change (ISIS_DISABLE, circuit, circuit->area);
  	isis_csm_state_change (IF_DOWN_FROM_Z, circuit_scan_by_ifp (ifp),
                                   ifp);
  }

  if_delete(ifp);

  return 0;
}

int
isis_zebra_if_state_down (struct ifm_event *pevent, vrf_id_t vrf_id)
{
  struct interface *ifp;
  struct isis_circuit *circuit;
  u_int8_t stat_flag = 0;

  ifp = ifm_interface_state_read (pevent, vrf_id,stat_flag);

  if (ifp == NULL)
    return 0;

  circuit = circuit_scan_by_ifp (ifp);

  if(circuit == NULL)
  	return 0;

  circuit = isis_csm_state_change (IF_DOWN_FROM_Z, circuit, ifp);

  if (circuit)
    SET_FLAG(circuit->flags, ISIS_CIRCUIT_FLAPPED_AFTER_SPF);

  return 0;
}

int
isis_zebra_if_address_add (struct ifm_event *pevent, vrf_id_t vrf_id)
{
  struct connected *c;
  struct prefix *p;
  char buf[BUFSIZ];

  c = ifm_interface_address_read (ZEBRA_INTERFACE_ADDRESS_ADD,pevent, vrf_id);

  if (c == NULL)
    return 0;
  
  p = c->address;
  prefix2str (p, buf, BUFSIZ);
  
  if (p->family == AF_INET)
    zlog_debug (ISIS_DEBUG_TYPE_ROUTE,"connected IP address %s", buf);
  if (p->family == AF_INET6)
    zlog_debug (ISIS_DEBUG_TYPE_ROUTE,"connected IPv6 address %s", buf);

  if (if_is_operative (c->ifp))
  {
    isis_circuit_add_addr (circuit_scan_by_ifp (c->ifp), c);
  }

  return 0;
}

int
isis_zebra_if_address_del (struct ifm_event *pevent, vrf_id_t vrf_id)
{
 #if 1
  struct connected *c;
  struct interface *ifp;
#ifdef EXTREME_DEBUG
  struct prefix *p;
  u_char buf[BUFSIZ];
#endif /* EXTREME_DEBUG */
  struct isis_circuit *circuit;	

  c = ifm_interface_address_read (ZEBRA_INTERFACE_ADDRESS_DELETE, pevent, vrf_id);

  if (c == NULL)
    return 0;

  ifp = c->ifp;

#ifdef EXTREME_DEBUG
  p = c->address;
  prefix2str (p, buf, BUFSIZ);

  if (p->family == AF_INET)
    zlog_debug (ISIS_DEBUG_TYPE_ROUTE,"disconnected IP address %s", buf);
  if (p->family == AF_INET6)
    zlog_debug (ISIS_DEBUG_TYPE_ROUTE,"disconnected IPv6 address %s", buf);
#endif /* EXTREME_DEBUG */

  circuit = circuit_scan_by_ifp(ifp);

  if(circuit == NULL)
  	return 0;
  
  if (if_is_operative (ifp))
  { 
    isis_circuit_del_addr (circuit, c);
  }
  
  connected_free (c);
#endif
  return 0;
}

static void
isis_zebra_route_old_del_ipv4 (struct prefix *prefix,struct isis_area *area,
			   struct isis_route_info *route_info)
{
	struct isis_nexthop *nexthop;
	struct listnode *node;
    struct route_entry route;
	struct routefifo *newroutefifoentry;

    if(route_info->old_nexthops == NULL)
		return;
	
	if(listcount(route_info->old_nexthops) == 0)
		return;

	memset(&route, 0 , sizeof(struct route_entry));
	for (ALL_LIST_ELEMENTS_RO (route_info->old_nexthops, node, nexthop))
    {
      if (nexthop->ip.s_addr == INADDR_ANY)
	  	continue;
      route.prefix.type = INET_FAMILY_IPV4;
      route.prefix.addr.ipv4 = ntohl(prefix->u.prefix4.s_addr);
      route.prefix.prefixlen = prefix->prefixlen;
      route.vpn = area->vpn;
      route.nhp_num = 1;
	  
	  route.nhp[0].nexthop.type = INET_FAMILY_IPV4;
	  route.nhp[0].nexthop.addr.ipv4 = ntohl(nexthop->ip.s_addr);
	  route.nhp[0].ifindex = nexthop->ifindex;
      route.nhp[0].action = NHP_ACTION_FORWARD;	  
      route.nhp[0].distance = area->distance4;
      route.nhp[0].cost = route_info->cost;
      route.nhp[0].vpn = area->vpn ;
      route.nhp[0].down_flag = 1;
      route.nhp[0].instance = atoi(area->area_tag);
      route.nhp[0].nhp_type = NHP_TYPE_IP;
      route.nhp[0].protocol = ROUTE_PROTO_ISIS;
      route.nhp[0].active = 0;
       
	  newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));
	  if (newroutefifoentry == NULL)
	  {
		zlog_debug(ISIS_DEBUG_TYPE_MSG,"%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
		return;
	  }
	  memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
	  newroutefifoentry->opcode = IPC_OPCODE_DELETE;
	  isis_sta.isis_route_del_merge++;
	  FIFO_ADD(&isis_m.routefifo, &newroutefifoentry->fifo);
    }

	isis_route_info_delete_old(route_info);
	UNSET_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);

	return;
}

static int
isis_zebra_route_add_ipv4 (struct prefix *prefix,struct isis_area *area,
			   struct isis_route_info *route_info)
{
	struct isis_nexthop *nexthop;
	struct listnode *node;
    struct route_entry route;
	struct routefifo *newroutefifoentry;

    if (CHECK_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED))
    	return 0;

	if (CHECK_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE))
		isis_zebra_route_old_del_ipv4(prefix, area, route_info);

    if(route_info->nexthops == NULL)
		return 0;
	
    memset(&route, 0 , sizeof(struct route_entry));
	
	for (ALL_LIST_ELEMENTS_RO (route_info->nexthops, node, nexthop))
    {
      if (nexthop->ip.s_addr == INADDR_ANY)
	  	continue;
      route.prefix.type = INET_FAMILY_IPV4;
      route.prefix.addr.ipv4 = ntohl(prefix->u.prefix4.s_addr);
      route.prefix.prefixlen = prefix->prefixlen;
      route.vpn = area->vpn;
      route.nhp_num = 1;
	  
	  route.nhp[0].nexthop.type = INET_FAMILY_IPV4;
	  route.nhp[0].nexthop.addr.ipv4 = ntohl(nexthop->ip.s_addr);
	  route.nhp[0].ifindex = nexthop->ifindex;
	  route.nhp[0].action = NHP_ACTION_FORWARD;
	  route.nhp[0].cost = route_info->cost; 
      route.nhp[0].distance = area->distance4;
      route.nhp[0].vpn = area->vpn ;
      route.nhp[0].down_flag = 1;
      route.nhp[0].instance = atoi(area->area_tag);
      route.nhp[0].active = 0;
      route.nhp[0].nhp_type = NHP_TYPE_IP;
      route.nhp[0].protocol = ROUTE_PROTO_ISIS;
	 
	  newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));
	  if (newroutefifoentry == NULL)
	  {
		 zlog_debug(ISIS_DEBUG_TYPE_MSG,"%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
		 return 0;
	  }
	  memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
	  newroutefifoentry->opcode = IPC_OPCODE_ADD;
	  isis_sta.isis_route_add_merge++;
	  FIFO_ADD(&isis_m.routefifo, &newroutefifoentry->fifo);
  }

  SET_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
  UNSET_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_RESYNC);
  UNSET_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);	
  return 1;
}

static int
isis_zebra_route_add_ipv4_merge (struct prefix *prefix,struct isis_area *area,
			   struct isis_route_info *route_info)
{
	struct merge_isis_nexthop *nexthop;
	struct listnode *node;
    struct route_entry route;
	struct routefifo *newroutefifoentry = NULL;
	int flag = 0;
    u_char buff[BUFSIZ];
    
    memset(&route, 0 , sizeof(struct route_entry));

	if(route_info->nexthops == NULL)
		return 0;
	
	if(listcount(route_info->nexthops) == 0)
		return 0;

    for (ALL_LIST_ELEMENTS_RO (route_info->nexthops, node, nexthop))
    {
       if (nexthop->ip.s_addr == INADDR_ANY)
	   	  continue;
	   
	   prefix2str (prefix, (char *) buff, BUFSIZ);
       route.prefix.type = INET_FAMILY_IPV4;
       route.prefix.addr.ipv4 = ntohl(prefix->u.prefix4.s_addr);
       route.prefix.prefixlen = prefix->prefixlen;
       route.vpn = area->vpn;
       route.nhp_num = 1;
	   
	   route.nhp[0].nexthop.type = INET_FAMILY_IPV4;
	   route.nhp[0].nexthop.addr.ipv4 = ntohl(nexthop->ip.s_addr);
	   route.nhp[0].ifindex = nexthop->ifindex;
	   route.nhp[0].action = NHP_ACTION_FORWARD;
	   route.nhp[0].cost = nexthop->cost;
       route.nhp[0].distance = area->distance4;
       route.nhp[0].vpn = area->vpn ;
       route.nhp[0].down_flag = 1;
       route.nhp[0].instance = atoi(area->area_tag);
       route.nhp[0].active = 0;
       route.nhp[0].nhp_type = NHP_TYPE_IP;
       route.nhp[0].protocol = ROUTE_PROTO_ISIS;
       flag++;
	   
	   newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));
	   if (newroutefifoentry == NULL)
	   {
		 zlog_debug(ISIS_DEBUG_TYPE_MSG,"%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
		 return 0;
	   }
	   memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
	   newroutefifoentry->opcode = IPC_OPCODE_ADD;
	   isis_sta.isis_route_add_merge++;
	   FIFO_ADD(&isis_m.routefifo, &newroutefifoentry->fifo);
    }

    return flag;
}

int
isis_zebra_route_del_ipv4_merge (struct prefix *prefix,struct isis_area *area,
			   struct isis_route_info *route_info)
{
	struct merge_isis_nexthop *nexthop;
	struct listnode *node;
    struct route_entry route;
	struct routefifo *newroutefifoentry;
	int flag = 0;
	u_char buff[BUFSIZ];

    if(route_info->old_nexthops == NULL)
		return 0;

	if(listcount(route_info->old_nexthops) == 0)
		return 0;

	memset(&route, 0 , sizeof(struct route_entry));
    for (ALL_LIST_ELEMENTS_RO (route_info->old_nexthops, node, nexthop))
    {
      if (nexthop->ip.s_addr == INADDR_ANY)
	  	continue;
	  prefix2str (prefix, (char *) buff, BUFSIZ);

	  route.prefix.type = INET_FAMILY_IPV4;
      route.prefix.addr.ipv4 = ntohl(prefix->u.prefix4.s_addr);
      route.prefix.prefixlen = prefix->prefixlen;
      route.vpn = area->vpn;
      route.nhp_num = 1;
	  
	  route.nhp[0].nexthop.type = INET_FAMILY_IPV4;
	  route.nhp[0].nexthop.addr.ipv4 = ntohl(nexthop->ip.s_addr);
	  route.nhp[0].ifindex = nexthop->ifindex;
      route.nhp[0].action = NHP_ACTION_FORWARD;	  
      route.nhp[0].distance = area->distance4;
	  route.nhp[0].cost = nexthop->cost;
      route.nhp[0].vpn = area->vpn ;
      route.nhp[0].down_flag = 1;
      route.nhp[0].instance = atoi(area->area_tag);
      route.nhp[0].nhp_type = NHP_TYPE_IP;
      route.nhp[0].protocol = ROUTE_PROTO_ISIS;
      route.nhp[0].active = 0;
      flag++;
      	  
	  newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));
	  if (newroutefifoentry == NULL)
	  {
		zlog_debug(ISIS_DEBUG_TYPE_MSG,"%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
		return 1;
	  }
	  memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
	  newroutefifoentry->opcode = IPC_OPCODE_DELETE;
	  isis_sta.isis_route_del_merge++;
	  FIFO_ADD(&isis_m.routefifo, &newroutefifoentry->fifo);
    }
    return flag;
}

void
isis_zebra_route_del_ipv4 (struct prefix *prefix, struct isis_area *area,
			   struct isis_route_info *route_info)
{
	struct isis_nexthop *nexthop;
	struct listnode *node;
    struct route_entry route;
	struct routefifo *newroutefifoentry;
	
    memset(&route, 0 , sizeof(struct route_entry));
	if(route_info->nexthops == NULL)
		return;
	
	for (ALL_LIST_ELEMENTS_RO (route_info->nexthops, node, nexthop))
	{	
	   if (nexthop->ip.s_addr == INADDR_ANY)
	   	continue;

	   route.prefix.type = INET_FAMILY_IPV4;
	   route.prefix.addr.ipv4 = ntohl(prefix->u.prefix4.s_addr);
	   route.prefix.prefixlen = prefix->prefixlen;
	   route.vpn = area->vpn;
	   route.nhp_num = 1;
	   
	   route.nhp[0].nexthop.type = INET_FAMILY_IPV4;
	   route.nhp[0].nexthop.addr.ipv4 = ntohl(nexthop->ip.s_addr);
	   route.nhp[0].ifindex = nexthop->ifindex;
	   route.nhp[0].action = NHP_ACTION_FORWARD;   
	   route.nhp[0].distance = area->distance4;
	   route.nhp[0].cost = route_info->cost;
	   route.nhp[0].vpn = area->vpn ;
	   route.nhp[0].down_flag = 1;
	   route.nhp[0].instance = atoi(area->area_tag);
	   route.nhp[0].nhp_type = NHP_TYPE_IP;
	   route.nhp[0].protocol = ROUTE_PROTO_ISIS;
	   route.nhp[0].active = 0;
	   
	   newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));
	   if (newroutefifoentry == NULL)
	   {
		 zlog_debug(ISIS_DEBUG_TYPE_MSG,"%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
		 return;
	   }
	   memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
	   newroutefifoentry->opcode = IPC_OPCODE_DELETE;
	   isis_sta.isis_route_del_merge++;
	   FIFO_ADD(&isis_m.routefifo, &newroutefifoentry->fifo);
	}

	UNSET_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
	
}

static void
isis_zebra_route_old_del_ipv6 (struct prefix *prefix,struct isis_area *area,
			   struct isis_route_info *route_info)
{
	struct isis_nexthop6 *nexthop6;
	struct listnode *node;
    struct route_entry route;
	struct routefifo *newroutefifoentry;

    if(route_info->old_nexthops6 == NULL)
		return;
	
	if(listcount(route_info->old_nexthops6) == 0)
		return;

	memset(&route, 0 , sizeof(struct route_entry));
	
	for (ALL_LIST_ELEMENTS_RO (route_info->old_nexthops, node, nexthop6))
    {
		if (!IN6_IS_ADDR_LINKLOCAL (&nexthop6->ip6) && !IN6_IS_ADDR_UNSPECIFIED (&nexthop6->ip6))
			continue;
        route.prefix.type = INET_FAMILY_IPV6;
        memcpy(&route.prefix.addr.ipv6, &prefix->u.prefix6, 128);
        route.prefix.prefixlen = prefix->prefixlen;
        route.vpn = area->vpn;
		
	    route.nhp[0].nexthop.type = INET_FAMILY_IPV6;
	    IPV6_ADDR_COPY(&route.nhp[0].nexthop.addr.ipv4, &nexthop6->ip6);
	    route.nhp[0].ifindex = nexthop6->ifindex;
        route.nhp[0].action = NHP_ACTION_FORWARD;	     
        route.nhp[0].distance = area->distance6;
        route.nhp[0].cost = route_info->cost;
        route.nhp[0].vpn = area->vpn;
        route.nhp[0].down_flag = 1;
        route.nhp[0].instance = atoi(area->area_tag);
        route.nhp[0].active = 0;
        route.nhp[0].nhp_type = NHP_TYPE_IP;
        route.nhp[0].protocol = ROUTE_PROTO_ISIS6;             
	    route.nhp_num = 1;
		
	    newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));
	    if (newroutefifoentry == NULL)
	    {
		  zlog_debug(ISIS_DEBUG_TYPE_MSG,"%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
		  return;
	    }
	    memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
	    newroutefifoentry->opcode = IPC_OPCODE_DELETE;
	    FIFO_ADD(&isis_m.routefifo, &newroutefifoentry->fifo);

        isis_sta.isis_route_del_merge++;
	}	
	list_delete_all_node(route_info->old_nexthops6);
	UNSET_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);
    return;
}

static int
isis_zebra_route_add_ipv6 (struct prefix *prefix,struct isis_area *area,
			   struct isis_route_info *route_info)
{
	struct isis_nexthop6 *nexthop6;
	struct listnode *node;
    struct route_entry route;
	struct routefifo *newroutefifoentry;

    if (CHECK_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED))
    	return 0;

	if (CHECK_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE))
		isis_zebra_route_old_del_ipv6(prefix, area, route_info);

    memset(&route, 0 , sizeof(struct route_entry));

    if(route_info->nexthops6 == NULL)
		return 0;
	
    /* Nexthop, ifindex, distance and metric information */    
    for (ALL_LIST_ELEMENTS_RO (route_info->nexthops6, node, nexthop6))
    {
	   if (!IN6_IS_ADDR_LINKLOCAL (&nexthop6->ip6) && !IN6_IS_ADDR_UNSPECIFIED (&nexthop6->ip6))
	     continue;
	   
	   route.prefix.type = INET_FAMILY_IPV6;
	   IPV6_ADDR_COPY(&route.prefix.addr.ipv6, &prefix->u.prefix6);
	   route.prefix.prefixlen = prefix->prefixlen;
	   route.vpn = area->vpn;
	    
	   route.nhp[0].nexthop.type = INET_FAMILY_IPV6;
	   IPV6_ADDR_COPY(&route.nhp[0].nexthop.addr.ipv6, &nexthop6->ip6);
	   route.nhp[0].ifindex = nexthop6->ifindex;
       route.nhp[0].action = NHP_ACTION_FORWARD;	     
       route.nhp[0].distance = area->distance6;
       route.nhp[0].cost = route_info->cost;
       route.nhp[0].vpn = area->vpn;
       route.nhp[0].down_flag = 1;
       route.nhp[0].instance = atoi(area->area_tag);
       route.nhp[0].active = 0;
       route.nhp[0].nhp_type = NHP_TYPE_IP;
       route.nhp[0].protocol = ROUTE_PROTO_ISIS6;
       route.nhp_num = 1;
	
	   newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));
	   if (newroutefifoentry == NULL)
	   {
		 zlog_debug(ISIS_DEBUG_TYPE_MSG,"%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
		 return 0;
	   }
	   memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
	   newroutefifoentry->opcode = IPC_OPCODE_ADD;
	   FIFO_ADD(&isis_m.routefifo, &newroutefifoentry->fifo);
	   isis_sta.isis_route_add_merge++;
    }
	SET_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
	UNSET_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_RESYNC);
	UNSET_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ROUTE_CHANGE);	
	return 1;
}

void
isis_zebra_route_del_ipv6 (struct prefix *prefix,struct isis_area *area,
			   struct isis_route_info *route_info)
{
	struct isis_nexthop6 *nexthop6;
	struct listnode *node;
    struct route_entry route;
	struct routefifo *newroutefifoentry;	
    memset(&route, 0 , sizeof(struct route_entry));

    if(route_info->nexthops6 == NULL)
		return;
	
    /* Nexthop, ifindex, distance and metric information */
	for (ALL_LIST_ELEMENTS_RO (route_info->nexthops6, node, nexthop6))
    {
	  if (!IN6_IS_ADDR_LINKLOCAL (&nexthop6->ip6) && !IN6_IS_ADDR_UNSPECIFIED (&nexthop6->ip6))
		continue;

	  route.prefix.type = INET_FAMILY_IPV6;
      IPV6_ADDR_COPY(&route.prefix.addr.ipv6, &prefix->u.prefix6);
      route.prefix.prefixlen = prefix->prefixlen;
      route.vpn = area->vpn;
	  
	  route.nhp[0].nexthop.type = INET_FAMILY_IPV6;
	  IPV6_ADDR_COPY(&route.nhp[0].nexthop.addr.ipv6, &nexthop6->ip6);
	  route.nhp[0].ifindex = nexthop6->ifindex;
	  route.nhp[0].action = NHP_ACTION_FORWARD;	  
	  route.nhp[0].cost = route_info->cost;
      route.nhp[0].distance = area->distance6; 
      route.nhp[0].vpn = area->vpn ;
      route.nhp[0].instance = atoi(area->area_tag);
      route.nhp[0].down_flag = 1;
      route.nhp[0].active = 0;
      route.nhp[0].nhp_type = NHP_TYPE_IP;
      route.nhp[0].protocol = ROUTE_PROTO_ISIS6;      
	  route.nhp_num = 1;

	  newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));
   	  if (newroutefifoentry == NULL)
   	  {
   		zlog_debug(ISIS_DEBUG_TYPE_MSG,"%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
   		return;
   	  }
   	  memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
   	  newroutefifoentry->opcode = IPC_OPCODE_DELETE;
   	  FIFO_ADD(&isis_m.routefifo, &newroutefifoentry->fifo);
   	  isis_sta.isis_route_del_merge++;
	}
	
	UNSET_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ZEBRA_SYNCED);
}

static int
isis_zebra_route_add_ipv6_merge (struct prefix *prefix,struct isis_area *area,
			   struct isis_route_info *route_info)
{
	struct merge_isis_nexthop6 *nexthop6;
	struct listnode *node;
    struct route_entry route;
	struct routefifo *newroutefifoentry;
	int flag = 0;

    memset(&route, 0 , sizeof(struct route_entry));
	if(route_info->nexthops6 == NULL)
		return 0;
	
	if(listcount(route_info->nexthops6) == 0)
		return 0;

    for (ALL_LIST_ELEMENTS_RO (route_info->nexthops6, node, nexthop6))
    {
      if (!IN6_IS_ADDR_LINKLOCAL (&nexthop6->ip) && !IN6_IS_ADDR_UNSPECIFIED (&nexthop6->ip))
		continue;
	  route.prefix.type = INET_FAMILY_IPV6;
      IPV6_ADDR_COPY(&route.prefix.addr.ipv6, &prefix->u.prefix6);
      route.prefix.prefixlen = prefix->prefixlen;
      route.vpn = 0;
      route.nhp_num = 1;
	   
	  route.nhp[0].nexthop.type = INET_FAMILY_IPV6;
	  IPV6_ADDR_COPY(&route.nhp[0].nexthop.addr.ipv6, &nexthop6->ip);
	  route.nhp[0].ifindex = nexthop6->ifindex;
	  route.nhp[0].action = NHP_ACTION_FORWARD;
      route.nhp[0].cost = route_info->cost; 
      route.nhp[0].distance = area->distance6;
      route.nhp[0].vpn = area->vpn ;
      route.nhp[0].down_flag = 1;
      route.nhp[0].instance = atoi(area->area_tag);
      route.nhp[0].active = 0;
      route.nhp[0].nhp_type = NHP_TYPE_IP;
      route.nhp[0].protocol = ROUTE_PROTO_ISIS6;

	  newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));
	  if (newroutefifoentry == NULL)
	  {
		 zlog_debug(ISIS_DEBUG_TYPE_MSG,"%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
		 return 0;
	  }
	  memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
	  newroutefifoentry->opcode = IPC_OPCODE_ADD;
	  isis_sta.isis_route_add_merge++;
	  FIFO_ADD(&isis_m.routefifo, &newroutefifoentry->fifo);
  }

  return flag;
}

int
isis_zebra_route_del_ipv6_merge (struct prefix *prefix,struct isis_area *area,
			   struct isis_route_info *route_info)
{
	struct merge_isis_nexthop6 *nexthop6;
	struct listnode *node;
    struct route_entry route;
	struct routefifo *newroutefifoentry;
	int flag = 0;

	if(route_info->old_nexthops6 == NULL)
		return 0;

	if(listcount(route_info->old_nexthops6) == 0)
		return 0;

	memset(&route, 0 , sizeof(struct route_entry));
    for (ALL_LIST_ELEMENTS_RO (route_info->old_nexthops6, node, nexthop6))
    {
      if (!IN6_IS_ADDR_LINKLOCAL (&nexthop6->ip) && !IN6_IS_ADDR_UNSPECIFIED (&nexthop6->ip))
		continue;
      route.prefix.type = INET_FAMILY_IPV6;
      IPV6_ADDR_COPY(&route.prefix.addr.ipv6, &prefix->u.prefix6);
      route.prefix.prefixlen = prefix->prefixlen;
      route.vpn = area->vpn;
	  
      route.nhp_num = 1;
      route.nhp[0].nexthop.type = INET_FAMILY_IPV6;
	  IPV6_ADDR_COPY(&route.nhp[0].nexthop.addr.ipv6, &nexthop6->ip);
	  route.nhp[0].ifindex = nexthop6->ifindex;
      route.nhp[0].action = NHP_ACTION_FORWARD;	  
      route.nhp[0].distance = area->distance6;
      route.nhp[0].cost = route_info->cost;
      route.nhp[0].vpn = area->vpn ;
      route.nhp[0].down_flag = 1;
      route.nhp[0].instance = atoi(area->area_tag);
      route.nhp[0].nhp_type = NHP_TYPE_IP;
      route.nhp[0].protocol = ROUTE_PROTO_ISIS6;
      route.nhp[0].active = 0;
      flag++;
       
	  newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));
	  if (newroutefifoentry == NULL)
	  {
		zlog_debug(ISIS_DEBUG_TYPE_MSG,"%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
		return 1;
	  }
	  memcpy(&newroutefifoentry->route, &route, sizeof(struct route_entry));
	  newroutefifoentry->opcode = IPC_OPCODE_DELETE;
	  isis_sta.isis_route_del_merge++;
	  FIFO_ADD(&isis_m.routefifo, &newroutefifoentry->fifo);
    }

	return flag;
}

int
isis_interface_change_mode (struct ifm_event *pevent, vrf_id_t vrf_id)
{	
	struct interface *ifp;
	char ifname_tmp[INTERFACE_NAMSIZ];
	struct isis_circuit *circuit;

	/* Get interface name. */
	ifm_get_name_by_ifindex(pevent->ifindex, ifname_tmp);
	
	/* Lookup this by interface index. */
	ifp = if_get_by_name_len_vrf(ifname_tmp,strnlen (ifname_tmp, INTERFACE_NAMSIZ),
								vrf_id);

    if (ifp == NULL)
       return CMD_WARNING;

    circuit = circuit_scan_by_ifp(ifp);

    if(circuit == NULL)
    {
  	  return CMD_WARNING;
    }

    if(pevent->mode != IFNET_MODE_L3)
    {
      /*the vpn value should be changed when the mode change to l2 or l3*/
      ifp->vpn = 0;
  	  isis_csm_state_change (ISIS_DISABLE, circuit, circuit->area);
  	  //isis_csm_state_change (IF_DOWN_FROM_Z, circuit_scan_by_ifp (ifp),
                                   //ifp);
      isis_circuit_conf_default (circuit);
    }

	if(pevent->mode == IFNET_MODE_L3)
	{
		ifp->vpn = 0;
		isis_csm_state_change (ISIS_DISABLE, circuit, circuit->area);
	}
	return 0;
}

void
isis_zebra_route_update (struct prefix *prefix,struct isis_area *area,
			 struct isis_route_info *route_info)
{
	if(CHECK_FLAG (route_info->flag, ISIS_ROUTE_FLAG_MERGE))
	{
	    if (prefix->family == AF_INET)
	    {
	      isis_zebra_route_del_ipv4_merge(prefix, area,route_info);
	      isis_zebra_route_add_ipv4_merge(prefix, area,route_info);
	      return;
	    }

	    if (prefix->family == AF_INET6)
	    {
	      isis_zebra_route_del_ipv6_merge(prefix, area,route_info);
	      isis_zebra_route_add_ipv6_merge(prefix, area,route_info);
	      return;
	    }
	}
    else
    {
	    if (CHECK_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ACTIVE))
        {
          if (prefix->family == AF_INET)
          { 
	  	     isis_zebra_route_add_ipv4 (prefix, area,route_info);
	  	     return;
          }

          if(prefix->family == AF_INET6)
          {
             isis_zebra_route_add_ipv6 (prefix, area,route_info);
             return;
          }
        }
        else if(!CHECK_FLAG (route_info->flag, ISIS_ROUTE_FLAG_ACTIVE))
        {
          if (prefix->family == AF_INET)
          {
		    isis_zebra_route_del_ipv4 (prefix,area,route_info);
		    return;
	      }

	      if(prefix->family == AF_INET6)
	      {
	         isis_zebra_route_del_ipv6 (prefix,area,route_info);
	         return;
	      }
        }
    }
    return;
}

int
isis_distribute_list_update (int routetype)
{
  return 0;
}

void isis_zebra_init (struct thread_master *master)
{
	isis_router_id_update_zebra();
	return;
}

