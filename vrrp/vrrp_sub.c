/*  file : harbour_ac/src/zebra/vrrpd/vrrp_sub.c
 *  brief: 定义或删除一个模板及其相关参数
 *        
*/ 

#include "../config.h"
#include <zebra.h>

#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>

#include "command.h"
#include "if.h"
#include "prefix.h"
#include "memory.h"
#include "network.h"
#include "table.h"
#include "stream.h"
#include "thread.h"
#include "zclient.h"
#include "filter.h"
#include "sockopt.h"
#include "zebra/connected.h"

#include "log.h"
#include "zlog.h"

#include "vrrpd.h"



struct cmd_node vrrp_node = {
 	VRRP_NODE,
 	"%s(config-vrrp)# ",
 	1
}; 


vrrp_rt *vrrp_router[255];

void vrrp_temp_init();
void vrrp_temp_clean();

int vrrp_temp_config_write(struct vty *vty);
int vrrp_router_show(struct vty *vty, int vrid);


static void vrrp_router_init();
static vrrp_rt * vrrp_router_add(int vrid);
static int vrrp_router_delete(int vrid);


static void vrrp_router_init()
{
     int i;

     for (i=0; i<255; i++)
  	vrrp_router[i] = NULL;
}


static vrrp_rt * vrrp_router_add(int vrid)
{
    vrrp_rt *vsrv;

    if(vrid < 1 || vrid > 255)
	return NULL;	

    vsrv = XMALLOC(MTYPE_VRRP_VRT, sizeof(vrrp_rt));
    if(!vsrv)    
	return NULL;

    vrrp_router[vrid - 1] = vsrv;

    init_virtual_srv(vsrv);
    vsrv->vrid = vrid;

    vsrv->vmac[0] = 0x00;
    vsrv->vmac[1] = 0x00;
    vsrv->vmac[2] = 0x5E;
    vsrv->vmac[3] = 0x00;
    vsrv->vmac[4] = 0x01;
    vsrv->vmac[5] = vrid;
    DBG(zlog_info("Vrrp router %d vmac is: %02x:%02x:%02x:%02x:%02x:%02x", vsrv->vrid,
	vsrv->vmac[0], vsrv->vmac[1], vsrv->vmac[2], vsrv->vmac[3], vsrv->vmac[4], vsrv->vmac[5]));
    
    return vsrv;
}


static int vrrp_router_delete(int vrid)
{
    vrrp_rt *vsrv;
    struct listnode *node; 
    struct listnode *node1; 

    if(vrid < 1 || vrid > 255)
	return -1;	

    vsrv = vrrp_router[vrid - 1];
    if(vsrv)
    {
	if(vsrv->vaddr)
    	{
          node = vsrv->vaddr->head;
          while(node)
          {
               node1 = node;
               node = node->next;
	       XFREE(MTYPE_VRRP_VADDR, node1->data);
               list_delete_node (vsrv->vaddr, node1);
          }
	  XFREE(MTYPE_LINK_LIST, vsrv->vaddr);
          vsrv->vaddr = NULL;
        }

	XFREE(MTYPE_VRRP_VRT, vsrv);
	vrrp_router[vrid - 1] = NULL;
    }
 
    return 0;
}


/**
* @brief 清除所有模板，当收到SIGINIT信号时被调用
*/
void vrrp_temp_clean()
{
     int i;

     for (i=1; i<=255; i++)
  	vrrp_router_delete(i);
}


/*show vrrp templete in vty shell*/
int vrrp_router_show(struct vty *vty, int vrid)
{
    vrrp_rt *vsrv;
    struct listnode *addr_nd = NULL;
    vip_addr *vaddr = NULL;

    if(vrid < 1 || vrid > 255)
	return -1;	

    vsrv = vrrp_router[vrid - 1];
    if(!vsrv)
	return -1;
 
    vty_out(vty,"vrrp-router %d%s", vrid ,VTY_NEWLINE);
	
    if (vsrv->cfg_priority)
       	vty_out(vty, " priority %d%s", vsrv->cfg_priority, VTY_NEWLINE);

    if (vsrv->adver_int)
       	vty_out(vty, " advertise-time %d%s", vsrv->adver_int, VTY_NEWLINE);

    if (vsrv->preempt)
       	vty_out(vty, " preempt-mode%s", VTY_NEWLINE);
#if 0
    if (vsrv->no_vmac)
       	vty_out(vty, " real-mac%s", VTY_NEWLINE);
#endif

    //address
    if(vsrv->vaddr)
    {
    	LIST_LOOP(vsrv->vaddr, vaddr, addr_nd)
    	{ 
            if(vaddr->inaddr.s_addr)
	    	vty_out(vty, " ip address %s%s", inet_ntoa(vaddr->inaddr), VTY_NEWLINE); 
    	}
    }
    
    vty_out(vty,"!%s",VTY_NEWLINE);

    return 0; 
}


/*show vrrp templete in vty shell*/
int vrrp_temp_config_write(struct vty *vty)
{
    int vrid;

    for(vrid=1; vrid<=255; vrid++)
    {	
	if(vrrp_router[vrid - 1])
	    vrrp_router_show(vty, vrid);
    }

    return 0; 
}


DEFUN (temp_vrrp_router,
         temp_vrrp_router_cmd,
         "vrrp-router <1-255>",
         "Configure vrrp router\n"
	 "vrrp router identifier from 1 to 255\n")
{
	vrrp_rt *vrt;
	int vrid;
	
	vrid = atoi(argv[0]);
  	vrt = vrrp_router[vrid - 1];
	if (!vrt)
	{ 
	    vrt = vrrp_router_add(vrid);	
	    if(!vrt)
	    {
                vty_out (vty, "Add vrrp router failed!%s",VTY_NEWLINE);
                return CMD_WARNING;
	    }
 	}

	vty->index = vrt;
	vty->node = VRRP_NODE;	
 	return CMD_SUCCESS;
}


DEFUN (no_temp_vrrp_router,
	no_temp_vrrp_router_cmd,
	"no vrrp-router <1-255>",
	NO_STR
	"vrrp router\n"
	"vrrp router identifier from 1 to 255\n")
{
	int vrid;
	int ret;
	vrrp_rt *vsrv;
	
	vrid = atoi(argv[0]);
	vsrv = vrrp_router[vrid-1];
	if(!vsrv)
	{
            vty_out (vty, "This vrrp router not defined!%s",VTY_NEWLINE);
	    return CMD_WARNING;
	}
	else if(vsrv->vif)
	{
            vty_out (vty, "This vrrp router is used!%s",VTY_NEWLINE);
	    return CMD_WARNING;
	}

	ret = vrrp_router_delete(vrid);	
	if(ret < 0)
	{
            vty_out (vty, "Delete vrrp router failed!%s",VTY_NEWLINE);
            return CMD_WARNING;
	}

        return CMD_SUCCESS;
}


DEFUN(vrrp_preempt,
	vrrp_preempt_cmd,
	"preempt-mode",
	"Permit backup vrrp router preempt\n")
{
	vrrp_rt *vrt = (vrrp_rt *)vty->index;

	vrt->preempt = 1;
	
	return CMD_SUCCESS;
}


DEFUN(no_vrrp_preempt,
	no_vrrp_preempt_cmd,
	"no preempt-mode",
	NO_STR
	"permit backup vrrp router preempt\n")
{
	vrrp_rt *vrt = (vrrp_rt *)vty->index;

 	vrt->preempt = 0;

	return CMD_SUCCESS;
}


DEFUN(vrrp_novmac,
	vrrp_novmac_cmd,
	"real-mac",
	"Configure vrrp router use real mac\n")
{
	vrrp_rt *vrt = (vrrp_rt *)vty->index;

	vrt->no_vmac = 1;
	
	return CMD_SUCCESS;
}


DEFUN(no_vrrp_novmac,
	no_vrrp_novmac_cmd,
	"no real-mac",
	NO_STR
	"vrrp router use real mac\n")
{
	vrrp_rt *vrt = (vrrp_rt *)vty->index;

 	vrt->no_vmac = 1;

	return CMD_SUCCESS;
}


DEFUN(vrrp_priority,
	vrrp_priority_cmd,
	"priority <1-254>",
	"Set vrrp router priority\n")
{
	vrrp_rt *vrt = (vrrp_rt *)vty->index;

	vrt->cfg_priority = atoi(argv[0]);

	if(vrt->priority != VRRP_PRIO_OWNER)
		vrt->priority = vrt->cfg_priority;
	
	return CMD_SUCCESS;
}


DEFUN(no_vrrp_priority,
	no_vrrp_priority_cmd,
	"no priority",
	NO_STR
	"vrrp router priority\n")
{
	vrrp_rt *vrt = (vrrp_rt *)vty->index;

 	vrt->cfg_priority = 100;

	if(vrt->priority != VRRP_PRIO_OWNER)
		vrt->priority = vrt->cfg_priority;

	return CMD_SUCCESS;
}


DEFUN(vrrp_advertise_time,
	vrrp_advertise_time_cmd,
	"advertise-time <1-10>",
	"Set vrrp router advertisement time, from 1 to 10 s\n")
{
	vrrp_rt *vrt = (vrrp_rt *)vty->index;

	vrt->adver_int = atoi(argv[0]);
	
	return CMD_SUCCESS;
}


DEFUN(no_vrrp_advertise_time,
	no_vrrp_advertise_time_cmd,
	"no advertise-time",
	NO_STR
	"vrrp router advertisement time\n")
{
	vrrp_rt *vrt = (vrrp_rt *)vty->index;

 	vrt->adver_int = VRRP_ADVER_DFL;

	return CMD_SUCCESS;
}


DEFUN(vrrp_ip_address,
	vrrp_ip_address_cmd,
	"ip address A.B.C.D",
	"Configure vrrp router ip adddress\n"
	"ip address such as 10.1.1.1\n")
{
        struct listnode *node;
        vrrp_rt * vrt = (vrrp_rt *) vty->index;
        vip_addr *vaddr;
        uint32_t ipaddr;
	//char *p;

	if(vrt->vif)
	{
           vty_out (vty, "Can't change IP address while it's in use%s",VTY_NEWLINE);
           return CMD_WARNING;
        }
 
        ipaddr = inet_addr(argv[0]);

        /*find the existing ip address*/
        LIST_LOOP(vrt->vaddr, vaddr, node)
        {
            if (vaddr->inaddr.s_addr == ipaddr )
            {
                return CMD_SUCCESS;
            }
        }

        vaddr = XMALLOC(MTYPE_VRRP_VADDR, sizeof(vip_addr));
        if(!vaddr)
        {
            vty_out (vty, "Vrrp ip address malloc error!%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
        vaddr->inaddr.s_addr = ipaddr;
	vaddr->deletable = 1;
	
        listnode_add(vrt->vaddr, vaddr);

 	return CMD_SUCCESS;
}


DEFUN(no_vrrp_ip_address,
	no_vrrp_ip_address_cmd,
	"no ip address A.B.C.D",
	NO_STR
	"vrrp router ip\n"
	"vrrp router ip address\n"
	"ip address and mask such as 10.1.1.1\n")
{
        struct listnode *node;
        vrrp_rt * vrt = (vrrp_rt *) vty->index;
        vip_addr *vaddr;
        uint32_t ipaddr;
	//char *p;

	if(vrt->vif)
	{
           vty_out (vty, "Can't delete IP address while it's in use%s", VTY_NEWLINE);
           return CMD_WARNING;
        }

        ipaddr = inet_addr(argv[0]);

        /*find the existing address*/
        LIST_LOOP(vrt->vaddr, vaddr, node)
        {
            if (vaddr->inaddr.s_addr == ipaddr)
            {
                XFREE(MTYPE_VRRP_VADDR, vaddr);
                list_delete_node (vrt->vaddr, node);
                return CMD_SUCCESS;
            }
        }

     	return CMD_SUCCESS;
}


DEFUN (show_vrrp_router,
       show_vrrp_router_cmd,
       "show vrrp router [<1-255>]",
        SHOW_STR
       "vrrp information\n"
       "vrrp router information\n"
       "vrrp router identifier from 1 to 255\n")
{
    //vrrp_rt *vrt;
    //struct listnode * vrt_nd = NULL;
    int vrid;

    if (argc == 1)
    {
        vrid = atoi(argv[0]);
	vrrp_router_show(vty, vrid);
	return CMD_SUCCESS;
    }
    else
    {
	vrrp_temp_config_write(vty);
	return CMD_SUCCESS;
    }

    vty_out (vty, "Can't find this vrrp router%s", VTY_NEWLINE);
    return CMD_WARNING;
}         


DEFUN (show_vrrp_router_state,
       show_vrrp_router_state_cmd,
       "show vrrp state",
        SHOW_STR
       "vrrp information\n"
       "vrrp router state information\n")
{
    int i;
    vrrp_rt *vsrv;

    vty_out (vty, "    VRID      state      priority     interface%s", VTY_NEWLINE);
    vty_out (vty, "================================================= %s", VTY_NEWLINE);
    for (i=0; i<255; i++)
    {
        vsrv = vrrp_router[i];
        if(vsrv && vsrv->vif)
	{
    	    vty_out (vty, "      %4d", vsrv->vrid);
 	    if(vsrv->state == VRRP_STATE_BACK)
    		vty_out (vty, "      %s", "BACKUP");
 	    else if(vsrv->state == VRRP_STATE_MAST)
    		vty_out (vty, "      %s", "MASTER");
 	    else if(vsrv->state == VRRP_STATE_INIT)
    		vty_out (vty, "      %s", "INIT");
	
	    if(vsrv->ip_owner)
    		vty_out (vty, "      255");
 	    else
    		vty_out (vty, "      %d", vsrv->cfg_priority);

    	    vty_out (vty, "        %s%s", vsrv->vif->ifname, VTY_NEWLINE);
	}
    }

    return CMD_WARNING;
}         



/*initialize vrrp templete, called in main function*/
void vrrp_temp_init()
{
  vrrp_router_init();
  access_list_init();

  install_node (&vrrp_node, vrrp_temp_config_write);
  install_default (VRRP_NODE);
        
  install_element (CONFIG_NODE,&temp_vrrp_router_cmd);
  install_element (CONFIG_NODE,&no_temp_vrrp_router_cmd);

  //install_element(VRRP_NODE, &vrrp_novmac_cmd);
  //install_element(VRRP_NODE, &no_vrrp_novmac_cmd);

  install_element(VRRP_NODE, &vrrp_preempt_cmd);
  install_element(VRRP_NODE, &no_vrrp_preempt_cmd);

  install_element(VRRP_NODE, &no_vrrp_priority_cmd);
  install_element(VRRP_NODE, &vrrp_priority_cmd);

  install_element(VRRP_NODE, &no_vrrp_advertise_time_cmd);
  install_element(VRRP_NODE, &vrrp_advertise_time_cmd);

  //install_element(VRRP_NODE, &no_vrrp_authentication_cmd);
  //install_element(VRRP_NODE, &vrrp_authentication_cmd);

  install_element(VRRP_NODE, &no_vrrp_ip_address_cmd);
  install_element(VRRP_NODE, &vrrp_ip_address_cmd);

  install_element (VIEW_NODE, &show_vrrp_router_cmd);
  install_element (ENABLE_NODE, &show_vrrp_router_cmd);

  install_element (VIEW_NODE, &show_vrrp_router_state_cmd);
  install_element (ENABLE_NODE, &show_vrrp_router_state_cmd);

  return;
}



