/* vrrpd and zebra interface.
 * Copyright (C) 2003 harbournetworks.com
 */
#include <zebra.h>
#include "command.h"
#include "zclient.h"
#include <net/ethernet.h>
  
#include "prefix.h"
#include "if.h"
#include "stream.h"
#include "memory.h"
#include "command.h"
#include "sockunion.h"
#include "sockopt.h"
#include "judge.h"
#include "thread.h"
  
#include "libnetlink.h"
#include "vrrpd.h"

#include "log.h"
#include "zlog.h"


/* All information about zebra. */
struct zclient *zclient = NULL;

/* Callback prototypes for zebra client service. */

/* Inteface up message processing */
static int
vrrp_interface_up (int command, struct zclient *zclient, zebra_size_t length)
{
    struct interface *ifp;
    vrrp_rt *vrt;
    vrrp_if *vif;
    struct listnode *vrt_node;

    ifp = zebra_interface_state_read (zclient->ibuf);
    if (!ifp)
    	return -1;

    DBG(zlog_info("Interface up"));

    vif = ifp->info;
    vif->up = 1;
    vif->ifindex = ifp->ifindex;
    strncpy(vif->ifname, ifp->name, INTERFACE_NAMSIZ);
    vif->ifname[INTERFACE_NAMSIZ] = 0;
    memcpy(vif->hwaddr, ifp->hw_addr, ETH_ALEN);
    DBG(zlog_info("Interface %s mac is: %02x:%02x:%02x:%02x:%02x:%02x", vif->ifname,
               vif->hwaddr[0], vif->hwaddr[1], vif->hwaddr[2],
               vif->hwaddr[3], vif->hwaddr[4], vif->hwaddr[5]));

    if(vif->ipaddr)
    {
    	LIST_LOOP(vif->vrt_list, vrt, vrt_node)
    	{
	    vrrp_router_enable(vrt, vif);
	}
    }
    
    return 0;
}

/* Inteface down message processing. */
static int
vrrp_interface_down (int command, struct zclient *zclient, zebra_size_t length)
{
    struct interface *ifp;
    struct stream *s;
    vrrp_rt *vrt;
    vrrp_if *vif;
    struct listnode *vrt_node;

    s = zclient->ibuf;  
    ifp = zebra_interface_state_read(s);
    if (!ifp)
    	return -1;

    DBG(zlog_info("Interface down"));

    vif = ifp->info;
    LIST_LOOP(vif->vrt_list, vrt, vrt_node)
    {
	vrrp_router_disable(vrt);
    }

    vif->up = 0;
    close_vrrp_sock(vif);
    
    return 0;
}


/* Inteface link up message processing */
static int
vrrp_interface_linkup (int command, struct zclient *zclient, zebra_size_t length)
{
    struct interface *ifp;
    vrrp_rt *vrt;
    vrrp_if *vif;
    struct listnode *vrt_node;

    ifp = zebra_interface_state_read (zclient->ibuf);
    if (!ifp)
    	return -1;

    DBG(zlog_info("Interface up"));

    vif = ifp->info;
    vif->up = 1;
    vif->ifindex = ifp->ifindex;
    strncpy(vif->ifname, ifp->name, INTERFACE_NAMSIZ);
    vif->ifname[INTERFACE_NAMSIZ] = 0;
    memcpy(vif->hwaddr, ifp->hw_addr, ETH_ALEN);
    DBG(zlog_info("Interface %s mac is: %02x:%02x:%02x:%02x:%02x:%02x", vif->ifname,
               vif->hwaddr[0], vif->hwaddr[1], vif->hwaddr[2],
               vif->hwaddr[3], vif->hwaddr[4], vif->hwaddr[5]));

    if(vif->ipaddr)
    {
    	LIST_LOOP(vif->vrt_list, vrt, vrt_node)
    	{
	    vrrp_router_enable(vrt, vif);
	}
    }
    
    return 0;
}

/* Inteface link down message processing. */
static int
vrrp_interface_linkdown (int command, struct zclient *zclient, zebra_size_t length)
{
    struct interface *ifp;
    struct stream *s;
    vrrp_rt *vrt;
    vrrp_if *vif;
    struct listnode *vrt_node;

    s = zclient->ibuf;  
    ifp = zebra_interface_state_read(s);
    if (!ifp)
    	return -1;

    DBG(zlog_info("Interface down"));

    vif = ifp->info;
    LIST_LOOP(vif->vrt_list, vrt, vrt_node)
    {
	vrrp_router_disable(vrt);
    }

    vif->up = 0;
    close_vrrp_sock(vif);
    
    return 0;
}


/* Inteface addition message from zebra. */
static int
vrrp_interface_add (int command, struct zclient *zclient, zebra_size_t length)
{
  struct interface *ifp;
  vrrp_if *vif;
  
  ifp = zebra_interface_add_read (zclient->ibuf);
  if (!ifp)
	return -1;
	
  DBG(zlog_info("Interface add"));

  vif = ifp->info;
  if (vif)
  {
    	vif->ifindex = ifp->ifindex;
    	strncpy(vif->ifname, ifp->name, INTERFACE_NAMSIZ);
    	vif->ifname[INTERFACE_NAMSIZ] = 0;
    	memcpy(vif->hwaddr, ifp->hw_addr, ETH_ALEN);
    	DBG(zlog_info("Interface %s mac is: %02x:%02x:%02x:%02x:%02x:%02x", vif->ifname,
               vif->hwaddr[0], vif->hwaddr[1], vif->hwaddr[2],
               vif->hwaddr[3], vif->hwaddr[4], vif->hwaddr[5]));

        return 0;
  }
  
  return 0;
}

static int
vrrp_interface_delete (int command, struct zclient *zclient, zebra_size_t length)
{
  struct interface *ifp;
  struct stream *s;
  vrrp_if *vif;
  
  s = zclient->ibuf;  
  ifp = zebra_interface_state_read(s);
  if (!ifp)
	return -1;
	  
  vif = ifp->info;  
  if(vif->vrt_list)
  {
        list_free(vif->vrt_list);
        vif->vrt_list = NULL;
  }

  close_vrrp_sock(vif);
  XFREE(MTYPE_VRRP_VIF, vif);
  ifp->info = NULL;

  if_delete(ifp);

  DBG(zlog_info("Interface delete"));

  return 0;
}


static int
vrrp_interface_address_add (int command, struct zclient *zclient, zebra_size_t length)
{
  struct connected *ifc;
  struct interface *ifp;  
  struct prefix_ipv4 *addr;
  uint32_t ifip;
  vrrp_if *vif;
  vrrp_rt *vrt;
  struct listnode *vrt_node;
  int ret;

  ifc = zebra_interface_address_add_read (zclient->ibuf);
  if (ifc)                                                                      
  {     
      addr = (struct prefix_ipv4 *)ifc->address;
      ifip = addr->prefix.s_addr;
 
      DBG(zlog_info("Interface address add: %s", ip_ntoa(ifip)));

      ifp = ifc->ifp;                                                            
      if (ifp && ifp->info)
      {
	  vif = ifp->info;

	  /* only handle interface primary address */
	  if (!(ifc->flags & ZEBRA_IFA_SECONDARY))
	  {
	      vif->ipaddr = addr->prefix.s_addr;
	      vif->masklen = addr->prefixlen;

	      LIST_LOOP(vif->vrt_list, vrt, vrt_node)
    	      {
	    	  vrrp_router_enable(vrt, vif);
 	      }
	  }
      }
  }

  return 0;
}


static int
vrrp_interface_address_delete (int command, struct zclient *zclient, zebra_size_t length)
{
  struct connected *ifc;
  struct interface *ifp;  
  struct prefix_ipv4 *addr;
  uint32_t ifip;
  vrrp_if *vif;
  vrrp_rt *vrt;
  struct listnode *vrt_node;
  int ret;

  ifc = zebra_interface_address_delete_read (zclient->ibuf);
  if (ifc)                                                                      
  {     
      addr = (struct prefix_ipv4 *)ifc->address;
      ifip = addr->prefix.s_addr;
                                                                   
      DBG(zlog_info("Interface address delete: %s", ip_ntoa(ifip)));
      ifp = ifc->ifp;                                                            
      if (ifp && ifp->info)
      {
	  vif = ifp->info;

	  /* only handle interface primary address */
	  if (!(ifc->flags & ZEBRA_IFA_SECONDARY))
	  {
	      LIST_LOOP(vif->vrt_list, vrt, vrt_node)
    	      {
		 vrrp_router_disable(vrt);
              }
	      
	      vif->ipaddr = 0;
	      vif->masklen = 0;
	      close_vrrp_sock(vif);
	  }
      }
		  
      connected_free (ifc);                                                     
  }
                    
  return 0;
}

void
vrrp_zclient_init ()
{
  DBG(zlog_info("vrrp_zclient_init"));

  /* Set default value to the zebra client structure. */
  zclient = zclient_new ();
  zclient_init (zclient, ZEBRA_ROUTE_SYSTEM);
 
  zclient->interface_up = vrrp_interface_up;
  zclient->interface_down = vrrp_interface_down;
  zclient->interface_add = vrrp_interface_add;
  zclient->interface_delete = vrrp_interface_delete;
  zclient->interface_address_add = vrrp_interface_address_add;
  zclient->interface_address_delete = vrrp_interface_address_delete;

  zclient->interface_linkon = vrrp_interface_linkup;
  zclient->interface_linkdown = vrrp_interface_linkdown;
}
