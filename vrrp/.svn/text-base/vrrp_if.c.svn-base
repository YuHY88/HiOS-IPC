/* file : vrrp_if.c
 *
 * brief: vrrp function on interface
*/

#include <zebra.h>
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


struct vrrp_if_req{
	struct nlmsghdr 	n;
	struct vrrpmsg 	        vrrp;
	char   			buf[256];
};


struct cmd_node interface_node = {
  INTERFACE_NODE,
  "%s(config-if)# ",
  1,
};

struct cmd_node tunnel_gre_node = {
	TUNNEL_GRE_NODE,
	"%s(config-if)# ",
	1
};

struct cmd_node tunnel_ipsec_node = {
	TUNNEL_IPSEC_NODE,
	"%s(config-if)# ",
	1
};

#ifdef CONFIG_BRIDGE_MODULE
struct cmd_node bridge_node = {
	BRIDGE_NODE,
	"%s(config-if)# ",
	1
};
#endif

struct cmd_node vlan_node = {
	VLAN_NODE,
	"%s(config-if)# ",
	1
};

extern int check_vrrp_temp(vrrp_rt *vsrv, struct vty *vty);

/* write the configuration of each interface */ 
int vrrp_if_config_write (struct vty *vty)
{
  struct interface *ifp;
  struct listnode *if_node;
  struct list *vrt_list; 
  struct listnode *vrt_node;
  vrrp_rt *vrt;
  vrrp_if *vif;
       
  LIST_LOOP (iflist, ifp, if_node)
  {    
      if (strncmp (ifp->name, "ipsec", 5))
      {
     vif = (vrrp_if *)ifp->info;
     vrt_list = vif->vrt_list;

     if(vif->auth_type || vrt_list->count)
     {
     	vty_out (vty, "interface %s%s", ifp->name, VTY_NEWLINE);
 
     	//authentication
     	if(vif->auth_type == VRRP_AUTH_PASS)
     	{
           vty_out(vty, " ip vrrp authentication pap");
           if(strncmp(vif->auth_data, "\0", VRRP_AUTH_LEN) != 0)
           	vty_out(vty, " password %s%s", vif->auth_data, VTY_NEWLINE);
     	}
     	else if(vif->auth_type == VRRP_AUTH_AH)
     	{
           vty_out(vty, " ip vrrp authentication ah");
           if(strncmp(vif->auth_data, "\0", VRRP_AUTH_LEN) != 0)
           	vty_out(vty, " password %s%s", vif->auth_data, VTY_NEWLINE);
     	}

        LIST_LOOP(vrt_list, vrt, vrt_node)
	{
            vty_out (vty, " ip vrrp router %d%s", vrt->vrid, VTY_NEWLINE);
    	}
     	
     	vty_out(vty,"!%s",VTY_NEWLINE);
     }
      }
   }

   return 0;
}


DEFUN (if_vrrp_router,
       if_vrrp_router_cmd,
       "ip vrrp router <1-255>",
       "Define ip parameter\n"
       "vrrp router parameter\n"
       "vrrp router identifier from 1 to 255\n")
{
    struct interface *ifp;
    int vrid;
    vrrp_rt *vsrv = NULL;
    vrrp_if *vif = NULL;
    //vrrp_rt *vrt;
    //struct listnode *vrt_node;
    //vip_addr *vaddr, *vaddr1;
    //struct listnode  *vaddr_node, *vaddr_node1;
	
    ifp = (struct interface *)vty->index;
    vif = ifp->info;
    if(!vif->ipaddr)   
    {
	vty_out(vty, "This interface has no primary ip address!%s", VTY_NEWLINE);
	return CMD_WARNING;
    }
    
    vrid = atoi(argv[0]);
    vsrv = vrrp_router[vrid - 1];
    if(!vsrv)
    {
	vty_out(vty, "This vrrp-router not defined!%s", VTY_NEWLINE);
	return CMD_WARNING;
    }
    else if(vsrv->vif)
    {
	vty_out(vty, "This vrrp-router already enabled!%s", VTY_NEWLINE);
	return CMD_WARNING;
    }
   
    if(!vif->ipaddr)   
    {
	vty_out(vty, "This interface has no primary ip address!%s", VTY_NEWLINE);
	return CMD_WARNING;
    }
 
#if 0    
    LIST_LOOP(vif->vrt_list, vrt, vrt_node)
    {
	LIST_LOOP(vrt->vaddr, vaddr, vaddr_node)
	{
	    LIST_LOOP(vsrv->vaddr, vaddr1, vaddr_node1)
	    {
	    	if (vaddr->inaddr.s_addr == vaddr1->inaddr.s_addr)
	    	{
		    vty_out(vty, "The virtual ip address has been used!%s", VTY_NEWLINE);
		    return CMD_WARNING;
	    	}
	    }
	}
    }
#endif    
    
    if(vrrp_router_enable(vsrv, vif) < 0)
    {
    	vty_out(vty," Vrrp-router %d enabled failed!%s", vsrv->vrid, VTY_NEWLINE);
    	return CMD_WARNING;
    }

    vty_out(vty," Vrrp-router %d enabled succeed!%s", vsrv->vrid, VTY_NEWLINE);
    listnode_add(vif->vrt_list, vsrv);
    return CMD_SUCCESS;
}


DEFUN (no_if_vrrp_router,
       no_if_vrrp_router_cmd,
       "no ip vrrp router <1-255>",
       NO_STR
       "ip parameter\n"
       "vrrp router parameter\n"
       "vrrp router identifier from 1 to 255\n")
{
    struct interface *ifp;
    vrrp_rt *vrt;
    vrrp_if *vif;
    struct listnode *vrt_node;
    int vrid;
    
    ifp = (struct interface *)vty->index;
    vrid = atoi(argv[0]);

    vif = ifp->info;
    LIST_LOOP(vif->vrt_list, vrt, vrt_node)
    {
	if(vrt->vrid == vrid)
	{
	   vrrp_router_disable(vrt);
    	   list_delete_node (vif->vrt_list, vrt_node);
    	   vrt->vif = NULL;
       
	   if(!vif->vrt_list->count)
		close_vrrp_sock(vif);

	   vty_out(vty, "Vrrp-router %d disabled!%s", vrt->vrid, VTY_NEWLINE);
    	   return CMD_WARNING; 
	}
    }

    vty_out(vty, "This vrrp-router not defined!%s", VTY_NEWLINE);
    return CMD_WARNING;
}


DEFUN(if_vrrp_authentication,
        if_vrrp_authentication_cmd,
        "ip vrrp authentication (pap|ah) password WORD",
        "Define ip parameter\n"
        "vrrp parameter\n"
        "authenticate type for vrrp router\n"
        "use pap authentication\n"
        "use ah authentication\n"
        "authentication password\n"
        "password, max length 8\n")
{
    	struct interface *ifp;
    	vrrp_if *vif;
    
    	ifp = (struct interface *)vty->index;
    	vif = ifp->info;

	if(vif->vrt_list->count)
	{
	    vty_out(vty, "Please disble all the vrrp router on this interface at first!%s", VTY_NEWLINE);
	    return CMD_WARNING;
	}
	
        if (strncmp(argv[0], "pap", 3) == 0)
           vif->auth_type = VRRP_AUTH_PASS;
        else if (strncmp(argv[0], "ah", 2) == 0)
           vif->auth_type = VRRP_AUTH_AH;
                
        strncpy(vif->auth_data, argv[1], VRRP_AUTH_LEN);
        vif->auth_data[VRRP_AUTH_LEN] = 0;

        return CMD_SUCCESS;
}


DEFUN(no_if_vrrp_authentication,
        no_if_vrrp_authentication_cmd,
        "no ip vrrp authentication",
        NO_STR
        "ip parameter\n"
        "vrrp parameter\n"
        "vrrp authentication type\n")
{
     	struct interface *ifp;
    	vrrp_if *vif;
    
    	ifp = (struct interface *)vty->index;
    	vif = ifp->info;
	
	if(vif->vrt_list->count)
	{
	    vty_out(vty, "Please disble all the vrrp-router on this interface at first!%s", VTY_NEWLINE);
	    return CMD_WARNING;
	}

	vif->auth_type = VRRP_AUTH_NONE;

        return CMD_SUCCESS;
}


DEFUN (show_vrrp_interface,                                                         
        show_vrrp_interface_cmd,                                                     
        "show vrrp interface",                                                       
        SHOW_STR                                                                
        "vrrp information\n"
        "vrrp interface information\n" )                                        
{                                                                               
    vrrp_if_config_write(vty);
    return CMD_SUCCESS;
}                   


/* Called when interface structure allocated. */
static int
vrrp_interface_new_hook (struct interface *ifp)
{
     vrrp_if *vif;

     DBG(zlog_info("Interface new hook"));
 
     vif = XMALLOC (MTYPE_VRRP_VIF, sizeof (vrrp_if));
     if (vif)
     {
        memset (vif, '\0', sizeof (vrrp_if));
        ifp->info = vif;

        vif->up = 1;
        vif->sockfd = -1;
        vif->read_thread = NULL;
        vif->vrt_list = list_new();

        return 0;
     }
   
     return -1;
}


/* Called when interface structure deleted. */
static int
vrrp_interface_delete_hook (struct interface *ifp)
{
    vrrp_if *vif;
    
    vif = ifp->info;
    if(!vif)
	return -1;

    DBG(zlog_info("Interface delete hook"));

    return 0;
}


void vrrp_if_init()
{
  /* Default initial size of interface vector. */
  DBG(zlog_info("vrrp_if_init"));
	
  if_init();
  if_add_hook (IF_NEW_HOOK, vrrp_interface_new_hook);
  if_add_hook (IF_DELETE_HOOK, vrrp_interface_delete_hook);

  /* Install interface node. */
  install_node (&interface_node, vrrp_if_config_write);
  install_default (INTERFACE_NODE);

  //add by codica
  install_node(&tunnel_gre_node,NULL);
  install_default(TUNNEL_GRE_NODE);

  install_node(&tunnel_ipsec_node,NULL);
  install_default(TUNNEL_IPSEC_NODE);

#ifdef CONFIG_BRIDGE_MODULE
  install_node(&bridge_node,NULL);
  install_default(BRIDGE_NODE);
#endif  

  install_node(&vlan_node,NULL);
  install_default(VLAN_NODE);

  /* Install commands. */
  install_element (CONFIG_NODE, &interface_cmd);
  install_element(VIEW_NODE, &show_vrrp_interface_cmd);     
  install_element(ENABLE_NODE, &show_vrrp_interface_cmd);                   

  install_element (INTERFACE_NODE, &interface_desc_cmd);
  install_element (INTERFACE_NODE, &no_interface_desc_cmd);
  install_element (INTERFACE_NODE, &if_vrrp_router_cmd);
  install_element (INTERFACE_NODE, &no_if_vrrp_router_cmd);
  install_element (INTERFACE_NODE, &if_vrrp_authentication_cmd);
  install_element (INTERFACE_NODE, &no_if_vrrp_authentication_cmd);

  install_element (VLAN_NODE, &interface_desc_cmd);
  install_element (VLAN_NODE, &no_interface_desc_cmd);
  install_element (VLAN_NODE, &if_vrrp_router_cmd);
  install_element (VLAN_NODE, &no_if_vrrp_router_cmd);
  install_element (VLAN_NODE, &if_vrrp_authentication_cmd);
  install_element (VLAN_NODE, &no_if_vrrp_authentication_cmd);
}
