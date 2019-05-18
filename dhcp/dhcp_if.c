/* file : dhcp_if.c
 *
 * brief: dhcp function on interface
*/


#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/command.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <lib/prefix.h>

#include "pool.h"
#include "pool_cli.h"
#include "dhcp_if.h"
#include "dhcp_relay.h"
#include "dhcp_client.h"
#include "dhcp_msg.h"
#include "clientpacket.h"
#include "pool_address.h"
#include "pool_static_address.h"

struct list dhcp_if_list = {0, 0, 0, 0, 0};

struct list dhcp_if_save_list = {0, 0, 0, 0, 0};
extern pthread_mutex_t dhcp_lock;
extern struct cmd_node physical_if_node;


/* write the configuration of each interface */ 
int dhcp_if_config_write (struct vty *vty)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Entering the function of '%s'--the line of %d", __func__,__LINE__);
	struct if_dhcp *pif = NULL;
	char ifname[IFNET_NAMESIZE];
	uint32_t ifindex;
	struct listnode *node;
	int ret = 0;
	char tmp[50];
	struct in_addr addr;
	char *str;
	
    for(node = dhcp_if_list.head ; node; node = node->next)
    {
		pif=listgetdata(node);
		if(pif && pif->dhcp_role != E_DHCP_NOTHING)
		{
		
			ifindex = pif->ifindex;
			ret = ifm_get_name_by_ifindex(ifindex, ifname);
			if(ret < 0)
			{
			    assert(0);
			}
			
			if(pif->dhcp_role == E_DHCP_CLIENT)
			{
			    continue;
			}
			vty_out(vty,"!%s",VTY_NEWLINE);
			vty_out (vty, "interface %s%s", ifname, VTY_NEWLINE );
			
			if(pif->dhcp_role == E_DHCP_SERVER)
			{
			    sprintf(tmp," ip dhcp server");
			}
			else if(pif->dhcp_role == E_DHCP_RELAY)
			{
				if(pif->r_ctrl && pif->r_ctrl->nexthop)
				{
					addr.s_addr = pif->r_ctrl->nexthop;
					sprintf(tmp," ip dhcp relay nexthop %s",inet_ntoa(addr));
				}
			}
			else if(pif->dhcp_role == E_DHCP_ZERO)
			{
				sprintf(tmp, " ip address dhcp zero");
			}
			vty_out(vty, "%s%s", tmp, VTY_NEWLINE);
			vty_out(vty, "!%s", VTY_NEWLINE);
		}
	}
	vty_out(vty,"!%s",VTY_NEWLINE);
	
	str = show_dhcp_option60();
	if(str != NULL)
	{
		vty_out(vty, "dhcp option60 %s%s", str, VTY_NEWLINE);
	}
	vty_out(vty,"!%s",VTY_NEWLINE);
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "leave the function of '%s',--the line of %d", __FUNCTION__, __LINE__);
	return 0;
}

/* write the configuration of each interface */ 
int dhcp_if_factory_write (struct vty *vty)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	struct if_dhcp *pif = NULL;
	struct if_dhcp *pif_save = NULL;
	char ifname[IFNET_NAMESIZE] = "";
	uint32_t ifindex;
	struct listnode *node;
	int ret = 0;
	char *str = NULL;

    for(node = dhcp_if_list.head ; node; node = node->next)
    {
		pif=listgetdata(node);
		if(pif && pif->dhcp_role != E_DHCP_NOTHING)
		{
			ifindex = pif->ifindex;
			ret = ifm_get_name_by_ifindex(ifindex, ifname);
			if(ret < 0)
			{
			    assert(0);
			}
			
			if(pif->dhcp_role == E_DHCP_ZERO)
			{
				vty_out(vty, "!%s",VTY_NEWLINE);
				vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE );
				vty_out(vty, " ip address dhcp zero%s", VTY_NEWLINE);
				pif_save = dhcp_if_save_lookup(ifindex);
				if(pif_save != NULL)
				{
					vty_out(vty, " ip address dhcp save%s", VTY_NEWLINE);
				}
				vty_out(vty, "!%s", VTY_NEWLINE);
			}
		}
	}
	str = show_dhcp_option60();
	if(str != NULL)
	{
		vty_out(vty, "dhcp option60 %s%s", str, VTY_NEWLINE);
	}
	vty_out(vty,"!%s",VTY_NEWLINE);
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "%s:leave the function of '%s',--the line of %d",__FILE__,__func__,__LINE__);
	return 0;
}

int show_dhcp_if_relay(struct vty *vty, struct if_dhcp *pif)
{
	struct xid_item *cur=NULL ;
	uint32_t xid;
	uint32_t ip;
	uint16_t port; 
	struct in_addr addr;

	if((!pif) || !(pif->r_ctrl))
	{
		return 1;
	}
	
	addr.s_addr = pif->r_ctrl->nexthop;
	vty_out (vty, "%-10s: %s%s","Nexthop",inet_ntoa(addr), VTY_NEWLINE );

	LOOP_XID_ITEM(pif->r_ctrl->xid_list,cur)
	{			
		xid = cur->xid ;	
		ip = cur->ip.sin_addr.s_addr ;	
		port = cur->ip.sin_port ;	
		addr.s_addr = ip;	
		
		vty_out(vty,"%-10s: xid 0x%08x, ip %-15s, port %d %s","Xid-item",xid,inet_ntoa(addr),port ,VTY_NEWLINE);
	}	
	return 0;
}

int show_dhcp_if_client(struct vty *vty, struct if_dhcp *pif)
{
	char str[20];
	//unsigned char *mac=NULL; 
	struct in_addr addr;
	
	if(!pif)
	{
	    return 1;
	}
	
	if(!pif->status)
	{		
	    vty_out(vty, "%-30s: %s%s", "Current machine state", "HALT", VTY_NEWLINE);	
	    return 1;
	}

	if(!(pif->c_ctrl))
	{
	    return 1;
	}
	
	dhcp_state_to_str(pif->c_ctrl->state, str);	
	vty_out(vty, "%-30s: %s%s", "Current machine state", str , VTY_NEWLINE);	
	vty_out(vty, "%-30s: %s%s", "Internet address assigned via", "DHCP" , VTY_NEWLINE);

	//mac = pif->mac;
	//vty_out (vty, "%-30s: %02X-%02X-%02X-%02X-%02X-%02X%s","Physical address", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], VTY_NEWLINE );

	if(pif->c_ctrl->ip)
	{
		addr.s_addr = pif->c_ctrl->ip;	
		vty_out(vty, "%-30s: %s%s", "IP address", inet_ntoa(addr), VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%-30s: %s%s", "IP address", "--", VTY_NEWLINE);
	}
	
	if(pif->c_ctrl->mask)	
	{
		addr.s_addr = pif->c_ctrl->mask;	
		vty_out(vty, "%-30s: %s%s", "Subnet mask", inet_ntoa(addr), VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%-30s: %s%s", "Subnet mask", "--", VTY_NEWLINE);
	}
	
	if(pif->c_ctrl->gate)
	{
		addr.s_addr = pif->c_ctrl->gate;	
		vty_out(vty, "%-30s: %s%s", "Gateway", inet_ntoa(addr), VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%-30s: %s%s", "Gateway", "--", VTY_NEWLINE);
	}
	
	if(pif->c_ctrl->dns)
	{
		addr.s_addr = pif->c_ctrl->dns;	
		vty_out(vty, "%-30s: %s%s", "DNS server", inet_ntoa(addr), VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%-30s: %s%s", "DNS server", "--", VTY_NEWLINE);
	}
	
	if(pif->c_ctrl->server_addr)
	{	
		addr.s_addr = pif->c_ctrl->server_addr; 
		vty_out(vty, "%-30s: %s%s", "DHCP server", inet_ntoa(addr), VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "%-30s: %s%s", "DHCP server", "--", VTY_NEWLINE);
	}	
	return 0;
}


int show_dhcp_if_server(struct vty *vty, struct if_dhcp *pif)
{
	struct in_addr addr;

	if(!pif)
	{
	    return 0;
	}

	if(pif->ip_addr)
	{	
	    addr.s_addr = pif->ip_addr;
	    vty_out(vty, "%-15s: %s/", "IP address", inet_ntoa(addr));		
	    addr.s_addr = pif->mask;
	    vty_out(vty, "%d %s", ip_masklen(addr), VTY_NEWLINE );
	}
	else
	{
	    vty_out(vty, "%-15s: %s%s", "IP address", "--", VTY_NEWLINE );
	}
	
	return 0;	
}

int show_dhcp_if (struct vty *vty, enum DHCP_ROLE role)
{
	struct if_dhcp *pif = NULL;
	char ifname[IFNET_NAMESIZE];
	uint32_t ifindex;
	struct listnode *node;
	int ret = 0;

	if(role != E_DHCP_CLIENT && role != E_DHCP_SERVER && role != E_DHCP_RELAY)
	{
		vty_out (vty, "Error ,dhcp  info: %s",  VTY_NEWLINE );
		return 0;
	}
	
    for(node = dhcp_if_list.head ; node; node = node->next)
    {
		pif = listgetdata(node);
		if(pif && pif->dhcp_role == role)
		{
			
			ifindex = pif->ifindex;
			ret = ifm_get_name_by_ifindex(ifindex, ifname);
			if(ret < 0)
			{
			    assert(0);
			}
			if(pif->mode != IFNET_MODE_L3)
			{
				continue;
			}
			
			if(role == E_DHCP_CLIENT)
			{
				vty_out(vty, "DHCP client lease information on interface  %s%s", ifname, VTY_NEWLINE );
				show_dhcp_if_client(vty , pif);
			}
			else if(role == E_DHCP_SERVER)
			{
				vty_out(vty, "DHCP server information on interface  %s%s", ifname, VTY_NEWLINE );
				show_dhcp_if_server(vty , pif);
			}
			else if(role == E_DHCP_RELAY)
			{
				vty_out(vty, "DHCP relay information on interface  %s%s", ifname, VTY_NEWLINE );
				show_dhcp_if_relay(vty , pif);				
			}
			
			//vty_out (vty, "%s",  VTY_NEWLINE );
		}
	}
	return 0;
}

DEFUN (ip_dhcp_server,
       ip_dhcp_server_cmd,
       "ip dhcp server",
	   "Specify IP configurations for interfaces\n"       
       "Dynamic host configure protocol\n"
	   "Interface as a  dhcp server\n"
		)
{
	uint32_t ifindex = 0;
	struct if_dhcp *pif=NULL;
	enum DHCP_ROLE role;
	struct ifm_l3  l3 = {0};
	uint32_t ipaddr;
	uint8_t  mask_len;
	uchar    flag = DISABLE;/* DHCP接口链表中是否存在此ifindex相关结构体 */
	uint8_t  status = 0;
	uint8_t  p_mode = 0;	
	
	ifindex = (uint32_t)vty->index;
	role = E_DHCP_SERVER;		

	if(ifm_get_mode(ifindex,MODULE_ID_DHCP, &p_mode) != 0)
	{
		vty_error_out(vty,"get the interface mode timeout!%s", VTY_NEWLINE);			
		return CMD_WARNING; 			
		
	}
	else if(p_mode != IFNET_MODE_L3)
	{
		vty_error_out(vty,"please config the interface mode and set 'mode l3' at first!%s", VTY_NEWLINE);			
		return CMD_WARNING; 			
	}

	pif = dhcp_if_lookup(ifindex);

	if(NULL != pif)
	{
		if(pif->dhcp_role != E_DHCP_NOTHING)
		{
			if(pif->dhcp_role == role)
			{
			  vty_error_out(vty, "please check your dhcp configurations first , repeated configrations%s", VTY_NEWLINE); 		
			}
			else
			{
			  vty_error_out(vty, "please check your dhcp configurations first , conflict configrations%s", VTY_NEWLINE); 		
			}

			return CMD_WARNING;			  
		}
	}
	else
	{
		flag = ENABLE;
		pif = (struct if_dhcp*)XCALLOC(MTYPE_IF, sizeof(struct if_dhcp));
		if(NULL == pif) 
		{
			vty_out(vty, "Error: There is no ram space%s", VTY_NEWLINE);	  		

			return CMD_WARNING;
		}
	}

	if(ifm_get_link(ifindex, MODULE_ID_DHCP, &status) == 0)
	{
		if(status == IFNET_LINKUP)
		{
			pif->status = ENABLE;	  
		}
		else
		{
			pif->status = DISABLE;	  
		}
	}
	else
	{
		vty_error_out(vty, "get interface status timeout%s", VTY_NEWLINE);			  
	}
	
	pif->ip_addr = 0;
	pif->mask = 0;

	if((ifm_get_l3if(ifindex, MODULE_ID_DHCP, &l3) == 0) && (l3.ipv4_flag == IP_TYPE_STATIC))
	{
		ipaddr = htonl(l3.ipv4[0].addr);
		mask_len = l3.ipv4[0].prefixlen;
		pif->ip_addr = ipaddr;
		masklen2ip(mask_len, (struct in_addr *)&pif->mask);			
	}
	else if(!vty->config_read_flag)
	{
		vty_error_out(vty, "Failed ,because no IP address has been configured on the interface.	%s", VTY_NEWLINE);
		if(flag == ENABLE)
		{
			XFREE(MTYPE_IF, pif);		
			pif = NULL;
		}
		
		return CMD_WARNING;
	}

	pif->ifindex = ifindex;
	pif->dhcp_role = role;
	pif->mode = IFNET_MODE_L3;
	
    if(flag == ENABLE)
    {
		pthread_mutex_lock(&dhcp_lock);
		dhcp_if_add(pif);
		pthread_mutex_unlock(&dhcp_lock);
    }

	vty_info_out(vty, "ifindex %08X, set dhcp server ok %s", ifindex, VTY_NEWLINE);
  
	return CMD_SUCCESS;
}

DEFUN (no_ip_dhcp_server,
       no_ip_dhcp_server_cmd,
       "no ip dhcp (server|relay)",
		NO_STR       
	   "Specify IP configurations for interfaces\n"       
       "Dynamic host configure protocol\n"
	   "Interface as a  dhcp server\n"
	   "Interface as a  dhcp relay\n"	   
		)

{
	uint32_t ifindex = 0;
	struct if_dhcp *pif;
	enum DHCP_ROLE role =E_DHCP_SERVER;
		
	ifindex = (uint32_t)vty->index;

	if(!strncmp(argv[0], "r", 1))
	{
		role = E_DHCP_RELAY;		
	}

	vty_info_out(vty, "ifindex %08X%s", ifindex, VTY_NEWLINE); 	

	pif = dhcp_if_lookup(ifindex);

	if(NULL != pif && pif->dhcp_role == role)	
	{
		pif->dhcp_role = E_DHCP_NOTHING;

		if(role == E_DHCP_RELAY)
		{
			dhcp_relay_fsm(pif, NULL);		
		}
		else
		{
			dhcp_addr_del_by_ifindex(pif->ifindex);
			dhcp_static_addr_del_by_ifindex(pif->ifindex);			
		}
		
		vty_info_out(vty, "interface dhcp %s closed%s", role==E_DHCP_SERVER?"server":"relay",VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	vty_error_out(vty, "please check your dhcp configurations first , no such interface%s", VTY_NEWLINE);	
	return CMD_WARNING; 	
}


DEFUN (ip_dhcp_relay,
       ip_dhcp_relay_cmd,
       "ip dhcp relay nexthop A.B.C.D",
	   "Specify IP configurations for interfaces\n"       
       "Dynamic host configure protocol\n"
	   "Interface as a  dhcp relay\n"
	   "Next relay address or server address\n"	   
	   "IP address\n"
		)

{
	uint32_t ifindex = 0;
	struct if_dhcp *pif=NULL;
	enum DHCP_ROLE role ;
	struct ifm_l3  l3 = {0};
	uint32_t ipaddr;
	uint8_t  mask_len;
	uchar    flag = DISABLE;/* DHCP接口链表中是否存在此ifindex相关结构体 */
	uint8_t  status = 0;
	uint8_t  p_mode = 0;	
	uint32_t nexthop,nexthop_host;
	char *str;

	ifindex = (uint32_t)vty->index;
	role = E_DHCP_RELAY;	

	if (argv[0] == NULL) 
	{
		vty_error_out(vty, "Input value is NULL%s", VTY_NEWLINE);
		return CMD_WARNING; 		
	}
	
	str = (char*)XCALLOC(MTYPE_IF, 17);
	if(NULL == str) 
	{
		DHCP_LOG_ERROR("Error: There is no ram space\n");		  
		return CMD_WARNING;
	}
	memcpy(str, argv[0], 17);

	//check nexthop
	nexthop_host = inet_strtoipv4(str);
	nexthop	 = htonl(nexthop_host);
	if(!inet_valid_host(nexthop_host))
	{
		vty_error_out(vty,"Invalid nexthop value%s",VTY_NEWLINE);
		XFREE(MTYPE_IF, str);
		str = NULL;
		return CMD_WARNING;			
	}
	
	if(!inet_valid_ipv4(nexthop_host))
	{
		vty_error_out(vty,"Invalid nexthop value%s",VTY_NEWLINE);
		XFREE(MTYPE_IF, str);
		str = NULL;
		return CMD_WARNING;			
	}
	
	//check interface mode
	if(ifm_get_mode(ifindex,MODULE_ID_DHCP, &p_mode) != 0)
	{
		vty_error_out(vty,"get the interface mode timeout!%s", VTY_NEWLINE);			
		XFREE(MTYPE_IF, str);
		str = NULL;
		return CMD_WARNING; 			
		
	}
	else if(p_mode != IFNET_MODE_L3)
	{
		vty_error_out(vty,"please config the interface mode and set 'mode l3' at first!%s", VTY_NEWLINE);			
		XFREE(MTYPE_IF, str);
		str = NULL;
		return CMD_WARNING; 			
	}

  	pif = dhcp_if_lookup(ifindex);
  
	if(NULL != pif)
	{
		if(pif->dhcp_role != E_DHCP_NOTHING)
		{
			if(pif->dhcp_role == role)
			{
				vty_error_out (vty, "please check your dhcp configurations first , repeated configrations%s", VTY_NEWLINE);	  
			}
			else
			{
				vty_error_out (vty, "please check your dhcp configurations first , conflict configrations%s", VTY_NEWLINE);	  
			}
			XFREE(MTYPE_IF, str);
			str = NULL;
			return CMD_WARNING; 			
		}
	}
	else
	{
		flag = ENABLE;
		pif = (struct if_dhcp*)XCALLOC(MTYPE_IF, sizeof(struct if_dhcp));
		if(NULL == pif) 
		{
			vty_error_out (vty, "There is no ram space%s", VTY_NEWLINE);		  
			XFREE(MTYPE_IF, str);
			str = NULL;
			return CMD_WARNING;
		}
	}
	
	//get interface status
	if(ifm_get_link(ifindex ,MODULE_ID_DHCP, &status) == 0)
	{
		if(status == IFNET_LINKUP)
		{
			pif->status = ENABLE;	  
		}
		else
		{	  
			pif->status = DISABLE;		
		}
	}
	else
	{
		vty_error_out (vty, "get interface status timeout%s", VTY_NEWLINE);			  
	}

	//get interface ip/mask
	pif->ip_addr = 0;
	pif->mask = 0;	

	if((ifm_get_l3if(ifindex, MODULE_ID_DHCP, &l3) == 0) && (l3.ipv4_flag == IP_TYPE_STATIC))
	{
		ipaddr = htonl(l3.ipv4[0].addr);
		mask_len = l3.ipv4[0].prefixlen;
		pif->ip_addr = ipaddr;
		masklen2ip(mask_len, (struct in_addr *)&pif->mask);		
	}
	else if(!vty->config_read_flag)
	{
		vty_error_out (vty, "Failed ,because no IP address has been configured on the interface.	%s", VTY_NEWLINE);
		
		if(flag == ENABLE)
		{
			XFREE(MTYPE_IF,pif);		
		}	
		XFREE(MTYPE_IF, str);
		str = NULL;
		return CMD_WARNING;		
	}

	if(!pif->r_ctrl)
	{
		pif->r_ctrl= (struct relay_ctrl*)XCALLOC(MTYPE_IF, sizeof(struct relay_ctrl));
		if(!pif->r_ctrl) 
		{
			if(flag == ENABLE)
			{
				XFREE(MTYPE_IF, pif);
				pif = NULL;
			}
			vty_error_out (vty, "There is no ram space%s", VTY_NEWLINE);						
			XFREE(MTYPE_IF, str);
			str = NULL;
			return CMD_WARNING;
		}
	}

	pif->ifindex = ifindex;
	pif->dhcp_role = role;
	pif->r_ctrl->nexthop = nexthop;
	pif->mode = IFNET_MODE_L3;

    if(flag == ENABLE)
    {
		pthread_mutex_lock(&dhcp_lock);
		dhcp_if_add(pif);
		pthread_mutex_unlock(&dhcp_lock);
    }
	vty_info_out (vty, "ifindex %08X, set dhcp relay ok %s", ifindex, VTY_NEWLINE);
		
	XFREE(MTYPE_IF, str);
	str = NULL;
	return CMD_SUCCESS;
}


DEFUN (show_dhcp_interface,                                                         
        show_dhcp_interface_cmd,                                                     
        "show ip dhcp (client|relay|server)",                                                       
        SHOW_STR     
        "Specify IP configurations for interfaces\n"
        "Dynamic host configure protocol\n"
        "Dhcp interface client information\n" 
		"Dhcp interface relay information\n" 
		"Dhcp interface server information\n" 
        
		)
{
	enum DHCP_ROLE role;

	if(!strncmp(argv[0],"client",1))
		role = E_DHCP_CLIENT;
	else if(!strncmp(argv[0],"server",1))
		role = E_DHCP_SERVER;
	else
		role = E_DHCP_RELAY;
		
    show_dhcp_if(vty,role);
    return CMD_SUCCESS;
}
DEFUN (ip_dhcp_zero,
	   ip_dhcp_zero_cmd,
	   "ip address dhcp zero",
	   "Specify IP configurations for interfaces\n" 
	   "Address\n"
	   "Dynamic host configure protocol\n"
	   "This interface is null\n"
		)
{
	struct if_dhcp *pif=NULL;
	enum DHCP_ROLE role;
	uchar	  pmac[6];
	uint8_t  status = 0;
	uint32_t ifindex = 0;
	uint8_t flag = DISABLE;
	#if 0
	if (vty->config_read_flag != 1) {
		vty_error_out (vty, "This cmd is not from config%s", VTY_NEWLINE);						
		return CMD_WARNING;
	}
	#endif
	role = E_DHCP_ZERO;

	ifindex = (uint32_t)vty->index;
	if (ifindex == 0) {
		vty_error_out (vty, "This interface is null%s", VTY_NEWLINE);						
		return CMD_WARNING;
	}
	pif = dhcp_if_lookup(ifindex);
	if (pif) {
		if(pif->dhcp_role != E_DHCP_NOTHING) {
			vty_error_out (vty, "This interface 0x%x is alive%s", ifindex, VTY_NEWLINE);						
			return CMD_WARNING;
		}
	} else {
		flag = ENABLE;
		pif = (struct if_dhcp*)XCALLOC(MTYPE_IF, sizeof(struct if_dhcp));
		if(NULL == pif) 
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Error: There is no ram space\n");		  
		
			return CMD_WARNING;
		}
	}

	if(ifm_get_mac(ifindex, MODULE_ID_DHCP,pmac) == 0)
	{
		memcpy(pif->mac,pmac,6);
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Info: get interface mac :[%02X:%02X:%02X:%02X:%02X:%02X] \n",pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);
	}
	pif->ifindex = ifindex;
	pif->dhcp_role = role;	
	pif->mode = IFNET_MODE_L3;
	pif->zero = ENABLE;

	if(ifm_get_link(ifindex ,MODULE_ID_DHCP, &status) == 0)//in ifm , 0  means link up
	{
		if(status == IFNET_LINKUP)
			pif->status = ENABLE;	
		else
			pif->status = DISABLE;	
	}
	else
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Error: Failed ,get interface status\n");   
		
	}

	if(pif->status == DISABLE)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Info: interface status down\n");	  
	  //pif->status = 1;	//aaaaaaaaaaaaaaaaafix	 
	}
	if(flag == ENABLE)
	{
		pthread_mutex_lock(&dhcp_lock);
		dhcp_if_add(pif);
		pthread_mutex_unlock(&dhcp_lock);
	}
	dhcp_client_fsm(pif , NULL , DHCP_EVENT_FROM_CMD );

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Info: interface dhcp client zero enabled\n");
	return CMD_SUCCESS;
}

DEFUN (no_ip_dhcp_zero,
	   no_ip_dhcp_zero_cmd,
	   "no ip address dhcp zero",
	   "disable dhcp zero\n"
	   "Specify IP configurations for interfaces\n" 
	   "Address\n"
	   "Dynamic host configure protocol\n"
	   "This interface is null\n"
		)

{
	uint32_t ifindex = 0;
	uint32_t ret = 0;
	
	ifindex = (uint32_t)vty->index;
	if (ifindex == 0) {
		vty_error_out (vty, "This interface is null%s", VTY_NEWLINE);						
		return CMD_WARNING;
	}
	//pif = dhcp_if_lookup(ifindex);
	
	ret = dhcp_client_zero_disable(ifindex);
	if(ret != ERRNO_SUCCESS)
	{
		if(ret == ERRNO_EXISTED)
		{
			vty_error_out(vty, "DHCP address zero already save, Please no save !%s", VTY_NEWLINE);
		}
		else if(ret == ERRNO_NOT_FOUND)
		{
			vty_error_out(vty, "This interface dhcp zero is not enable!%s", VTY_NEWLINE);
		}
		else
		{
			vty_error_out(vty, "This interface delete dhcp zero failed!%s", VTY_NEWLINE);						
		}
	}
	return CMD_SUCCESS;
}
DEFUN (dhcp_option60,
	   dhcp_option60_cmd,
	   "dhcp option60 STRING",
	   "Dynamic host configure protocol\n"
	   "dhcp option60\n"
	   "vendor identification\n")

{
	int ret = -1;
	int size = 0;
	char *str;
	if (argv[0] == NULL)
	{
		vty_error_out(vty,"Input value is NULL%s",VTY_NEWLINE);
		return CMD_WARNING;			
	}
	size = strlen(argv[0]);
	str = (char*)XCALLOC(MTYPE_IF, size + 1);
	if(NULL == str) 
	{
		DHCP_LOG_ERROR("Error: There is no ram space\n");		  
	
		return CMD_WARNING;
	}
	memcpy(str, argv[0], size);
	
	ret = dhcp_option60_set(str);
	if(!ret) 
	{
		vty_info_out (vty, "Info :set dhcp option60 is success!%s", VTY_NEWLINE); 					
		XFREE(MTYPE_IF, str);
		str = NULL;
		return CMD_SUCCESS; 	
	}
	
	vty_error_out (vty, "set dhcp option60 is failed!%s", VTY_NEWLINE); 					
	XFREE(MTYPE_IF, str);
	str = NULL;
	return CMD_WARNING; 	
  
}

DEFUN (dhcp_option60_show,
	  dhcp_option60_show_cmd,
	  "show dhcp option60",
	  SHOW_STR
	  "Dynamic host configure protocol\n"
	  "dhcp option60vendor identification\n")

{
	char *str;
	
	str = show_dhcp_option60();
	if(str == NULL)
	{
		vty_error_out (vty, "dhcp option60 is NULL!%s", VTY_NEWLINE);				   
		return CMD_WARNING;	 
	}
	else
	{
		vty_out(vty, "dhcp option60 %s%s", str, VTY_NEWLINE);
		return CMD_SUCCESS;
	}
}
DEFUN(no_ip_addr_dhcp,
	no_ip_addr_dhcp_cmd,
	"no ip address dhcp",
	"Delete command\n"
	"IP command\n"
	"Address\n"
	"IP address allocated by DHCP\n")
{
	uint32_t ifindex = 0;
	ifindex = (uint32_t)vty->index;
	uint32_t ret;
	if(ifindex == 0)
	{
		vty_error_out(vty, "interface index is error!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	ret = dhcp_client_disable(ifindex);
	if(ret != ERRNO_SUCCESS && ret != ERRNO_EXISTED)
	{
		vty_error_out(vty, "disable dhcp is failed!!%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}
	return CMD_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------*/
DEFUN (dhcp_conf_debug_fun,
	   dhcp_conf_debug_cmd,
	   "debug dhcp (enable|disable) (all|client|relay|server)",
	   "Debug config\n"
	   "dhcp config\n"
	   "dhcp debug enable\n"
	   "dhcp debug disable\n"
	   "dhcp debug type all\n"
	   "dhcp debug type client\n"
	   "dhcp debug type relay\n"
	   "dhcp debug type server\n")
{
	int enable = 0;
	unsigned int type = 0;

	if(argv[0][0] == 'e') enable = 1;

	if(strcmp(argv[1],"client") == 0)
		type = DHCP_DEBUG_TYPE_CLIENT;
	else if(strcmp(argv[1],"relay") == 0)
		type = DHCP_DEBUG_TYPE_RELAY;
	else if(strcmp(argv[1],"server") == 0)
		type = DHCP_DEBUG_TYPE_SERVER;
	else type = DHCP_DEBUG_TYPE_ALL;

	zlog_debug_set(vty, type,  enable);

	return CMD_SUCCESS;
}
	   
void dhcp_if_cmd_init(void)
{     

	 ifm_l3if_init(dhcp_if_config_write);    
	 install_node_factory ( &physical_if_node, dhcp_if_factory_write );
     install_element(CONFIG_NODE, &show_dhcp_interface_cmd, CMD_LOCAL);
     install_element(CONFIG_NODE, &dhcp_option60_cmd, CMD_SYNC);
     install_element(CONFIG_NODE, &dhcp_option60_show_cmd, CMD_LOCAL);
     install_element(CONFIG_NODE, &dhcp_conf_debug_cmd, CMD_LOCAL);
	 
	 install_element(PHYSICAL_IF_NODE,&ip_dhcp_server_cmd, CMD_SYNC);
	 install_element(PHYSICAL_IF_NODE,&ip_dhcp_relay_cmd, CMD_SYNC);	 
	 install_element(PHYSICAL_IF_NODE,&no_ip_dhcp_server_cmd, CMD_SYNC);	
	 install_element(PHYSICAL_IF_NODE,&no_ip_addr_dhcp_cmd, CMD_SYNC);
	 install_element(PHYSICAL_IF_NODE,&ip_dhcp_zero_cmd, CMD_SYNC);
	 install_element(PHYSICAL_IF_NODE,&no_ip_dhcp_zero_cmd, CMD_SYNC);
	 
	 install_element(PHYSICAL_SUBIF_NODE,&ip_dhcp_server_cmd, CMD_SYNC);
	 install_element(PHYSICAL_SUBIF_NODE,&ip_dhcp_relay_cmd, CMD_SYNC);	
	 install_element(PHYSICAL_SUBIF_NODE,&no_ip_dhcp_server_cmd, CMD_SYNC);	
	 install_element(PHYSICAL_SUBIF_NODE,&no_ip_addr_dhcp_cmd, CMD_SYNC);
	 install_element(PHYSICAL_SUBIF_NODE,&ip_dhcp_zero_cmd, CMD_SYNC);
	 install_element(PHYSICAL_SUBIF_NODE,&no_ip_dhcp_zero_cmd, CMD_SYNC);
	 
	 install_element(TRUNK_IF_NODE,&ip_dhcp_server_cmd, CMD_SYNC);
	 install_element(TRUNK_IF_NODE,&ip_dhcp_relay_cmd, CMD_SYNC);   
	 install_element(TRUNK_IF_NODE,&no_ip_dhcp_server_cmd, CMD_SYNC);   
	 install_element(TRUNK_IF_NODE,&no_ip_addr_dhcp_cmd, CMD_SYNC);
	 install_element(TRUNK_IF_NODE,&ip_dhcp_zero_cmd, CMD_SYNC);
	 install_element(TRUNK_IF_NODE,&no_ip_dhcp_zero_cmd, CMD_SYNC);
	 
	 install_element(TRUNK_SUBIF_NODE,&ip_dhcp_server_cmd, CMD_SYNC);
	 install_element(TRUNK_SUBIF_NODE,&ip_dhcp_relay_cmd, CMD_SYNC);   
	 install_element(TRUNK_SUBIF_NODE,&no_ip_dhcp_server_cmd, CMD_SYNC);   
	 install_element(TRUNK_SUBIF_NODE,&no_ip_addr_dhcp_cmd, CMD_SYNC);
	 install_element(TRUNK_SUBIF_NODE,&ip_dhcp_zero_cmd, CMD_SYNC);
	 install_element(TRUNK_SUBIF_NODE,&no_ip_dhcp_zero_cmd, CMD_SYNC);
	 
	 install_element(VLANIF_NODE,&ip_dhcp_server_cmd, CMD_SYNC);
	 install_element(VLANIF_NODE,&ip_dhcp_relay_cmd, CMD_SYNC);   
	 install_element(VLANIF_NODE,&no_ip_dhcp_server_cmd, CMD_SYNC);   
	 install_element(VLANIF_NODE,&no_ip_addr_dhcp_cmd, CMD_SYNC);
	 install_element(VLANIF_NODE,&ip_dhcp_zero_cmd, CMD_SYNC);
	 install_element(VLANIF_NODE,&no_ip_dhcp_zero_cmd, CMD_SYNC);
}


int dhcp_if_add(struct if_dhcp *pif)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Entering the function of '%s'--the line of %d", __func__,__LINE__);

	if (NULL == pif)					//预防操作空指针
	{
		return 1;
	}

	listnode_add(&dhcp_if_list,  pif);
	
	return 0;
}


int dhcp_if_delete(uint32_t ifindex)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Entering the function of '%s'--the line of %d", __func__,__LINE__);
	struct if_dhcp *pif = NULL;
	struct listnode *node;

	
	if(0 == ifindex)					
	{
		return 1;
	}
	
    for(node = dhcp_if_list.head ; node; node = node->next ,pif=NULL)
    {
		pif = listgetdata(node);

		if(pif && pif->ifindex == ifindex)
		{
			pif->status = DISABLE;
			usleep(50);
			listnode_delete(&dhcp_if_list,pif);
			XFREE(MTYPE_IF,pif);	
			pif = NULL;
			break;
		}
	}
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "leave the function of '%s',--the line of %d", __func__,__LINE__);

	return 0;
}


struct if_dhcp *dhcp_if_lookup(uint32_t ifindex)
{
	struct if_dhcp *pif = NULL;
	struct listnode *node;
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Entering the function of '%s',ifindex  %08X", __func__,ifindex);

    for(node = dhcp_if_list.head ; node; node = node->next,pif=NULL)
    {
		pif=listgetdata(node);
		if(pif && pif->ifindex == ifindex)
		{
			break;
		}
	}

	if(NULL == pif)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "fail to find dhcp_if entry according to ifindex of %08X", ifindex);
		return NULL;
	}
	else
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "ok to find dhcp_if entry according to ifindex of %08X", ifindex);
		return pif;
	}
}

struct if_dhcp *dhcp_if_lookup_by_ip(uint32_t ip, uint8_t relay_flag)
{
	struct if_dhcp *pif = NULL;
	struct listnode *node;
	struct in_addr addr;

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Entering the function of '%s',relay_flag %d", __func__,relay_flag);

	if(ip == 0 || ip == 0xffffffff)
	{
		return pif;
	}
	
    for(node = dhcp_if_list.head ; node; node = node->next, pif=NULL)
    {
		pif = listgetdata(node);
		if(pif && pif->ip_addr == ip)
		{
			if(!relay_flag || pif->dhcp_role == E_DHCP_RELAY)
			{
				break;
			}
		}
	}

	addr.s_addr 	= ip;

	if(NULL == pif)
	{
		zlog_err("%s:%s:fail to find dhcp_if entry according to ip  %s",__FILE__,__func__,inet_ntoa(addr));
		return NULL;
	}
	else
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "ok to find dhcp_if entry according to ip  %s", inet_ntoa(addr));
		return pif;
	}
}

int dhcp_if_down(uint32_t ifindex)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Entering the function of '%s'--the line of %d", __func__,__LINE__);
	struct if_dhcp *pif;
	
	pif = dhcp_if_lookup(ifindex);
	
	if(pif)
	{
		pif->status = DISABLE;
		
		if(pif->dhcp_role == E_DHCP_CLIENT || pif->dhcp_role == E_DHCP_ZERO)
		{
			if(pif->dhcp_role == E_DHCP_ZERO)
			{	
				dhcp_zero_cancel(ifindex);
			}
			dhcp_client_fsm(pif , NULL , DHCP_EVENT_FROM_IPC);		
		}
		else if(pif->dhcp_role == E_DHCP_RELAY)
		{	
			dhcp_relay_fsm(pif , NULL);
		}
	}
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "leave the function of '%s',--the line of %d", __func__, __LINE__);
	return 0;
}

int dhcp_if_up(uint32_t ifindex)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Entering the function of '%s'--the line of %d", __func__, __LINE__);
	struct if_dhcp *pif;
	
	pif = dhcp_if_lookup(ifindex);
	
	if(pif)
	{
		pif->status = ENABLE;
		
		if(pif->dhcp_role == E_DHCP_CLIENT || pif->dhcp_role == E_DHCP_ZERO)
		{	
			dhcp_client_fsm(pif , NULL , DHCP_EVENT_FROM_IPC );		
		}
	}
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "leave the function of '%s',--the line of %d", __func__, __LINE__);

	return 0;
}

int dhcp_if_ip_add(uint32_t ifindex, struct inet_prefix ipaddr, uint8_t slave)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Entering the function of '%s',slave %d", __func__, slave);
	struct if_dhcp *pif;
	
	pif = dhcp_if_lookup(ifindex);
	
	if(pif && (pif->dhcp_role == E_DHCP_SERVER ||pif->dhcp_role == E_DHCP_RELAY))
	{
		if(!slave)
		{
			pif->ip_addr = ipaddr.addr.ipv4;	
		}
		else
		{	
			pif->ip_addr_slave = ipaddr.addr.ipv4;	
		}
		masklen2ip(ipaddr.prefixlen,(struct in_addr *)&pif->mask);
		
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "dhcp_if_ip_add , ip:%08X ,mask:%08X",ntohl(ipaddr.addr.ipv4),pif->mask);
	}

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "leave the function of '%s',--the line of %d", __func__, __LINE__);

	return 0;
}

int dhcp_if_ip_del(uint32_t ifindex, uint8_t slave)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Entering the function of '%s',slave %d", __func__, slave);
	struct if_dhcp *pif;
	enum DHCP_ROLE role;
	
	pif = dhcp_if_lookup(ifindex);

	if(!pif)
	{
		return 0;
	}
	role = pif->dhcp_role;
	
	if(role != E_DHCP_CLIENT)
	{		
		if(!slave)
		{
			pif->ip_addr = 0;	
		}
		else
		{	
			pif->ip_addr_slave = 0;	
		}
		pif->mask = 0;	
		pif->dhcp_role = E_DHCP_NOTHING;		

		if(role == E_DHCP_SERVER)
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Info: interface dhcp server closed\n");	
			
			dhcp_addr_del_by_ifindex(pif->ifindex);
			dhcp_static_addr_del_by_ifindex(pif->ifindex);	
		}
		else if(role == E_DHCP_RELAY)
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_RELAY, "Info: interface dhcp relay closed\n");
		
			dhcp_relay_fsm(pif,NULL);				
		}
		
	}

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "leave the function of '%s',--the line of %d", __func__,__LINE__);

	return 0;
}

int dhcp_if_mode_change(uint32_t ifindex,enum IFNET_MODE  mode)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Entering the function of '%s'--the line of %d", __func__, __LINE__);
	struct if_dhcp *pif;
	
	pif = dhcp_if_lookup(ifindex);
	
	if(pif)
	{		
		pif->mode= mode; 
		
		if(mode != IFNET_MODE_L3 &&pif->dhcp_role == E_DHCP_CLIENT)
		{
			dhcp_client_save_disable(ifindex);
			pif->dhcp_role = E_DHCP_NOTHING;
			
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Info: interface dhcp client closed\n");
			
			dhcp_client_fsm(pif , NULL , DHCP_EVENT_FROM_CMD );
		}
		else if(mode != IFNET_MODE_L3 &&pif->dhcp_role == E_DHCP_ZERO)
		{
			dhcp_client_save_disable(ifindex);
			dhcp_client_zero_disable(ifindex);
		}
	}
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "leave the function of '%s',--the line of %d", __func__, __LINE__);
	return 0;
}


int dhcp_client_enable(uint32_t ifindex)
{
	struct if_dhcp *pif=NULL;
	enum DHCP_ROLE role;
	uchar    flag = DISABLE;/* DHCP接口链表中是否存在此ifindex相关结构体 */
	uchar     pmac[6];
	uint8_t   status = 0;

	role = E_DHCP_CLIENT ;

	pif = dhcp_if_lookup(ifindex);
	
	if(NULL != pif)
	{
		if(pif->dhcp_role != E_DHCP_NOTHING)
		{
			if(pif->dhcp_role == role)
			{
				DHCP_LOG_ERROR("Error: please check your dhcp configurations first , repeated configrations\n");		  
			}
			else
			{
				DHCP_LOG_ERROR("Error: please check your dhcp configurations first , conflict configrations\n");		  
			}
			
			return CMD_WARNING; 			
		}
	}
	else
	{
		flag = ENABLE;
		pif = (struct if_dhcp*)XCALLOC(MTYPE_IF, sizeof(struct if_dhcp));
		if(NULL == pif) 
		{
			DHCP_LOG_ERROR("Error: There is no ram space\n");		  
			return CMD_WARNING;
		}
	}


	//get if info
	//ifm_get_name_by_ifindex(ifindex ,pif->ifname);

	if(ifm_get_mac(ifindex, MODULE_ID_DHCP, pmac) == 0)
	{
		memcpy(pif->mac,pmac,6);
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Info: get interface mac :[%02X:%02X:%02X:%02X:%02X:%02X] \n",pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);
	}
	/*else
	{
		LOG (LOG_ERR, "Error: Failed ,get mac address timeout\n");
		pif->mac[0] = 0x00;
		pif->mac[1] = 0x1d;
		pif->mac[2] = 0x80;	
		pif->mac[3] = 0x00;
		pif->mac[4] = 0x00;	
		pif->mac[5] = 0x05;
		
		if(flag)
		{
			XFREE(MTYPE_IF ,pif);
		}
		
		return CMD_WARNING; 

	}  */

	pif->ifindex = ifindex;
	pif->dhcp_role = role;	
	pif->mode = IFNET_MODE_L3;

	if(ifm_get_link(ifindex ,MODULE_ID_DHCP, &status) == 0)//in ifm , 0  means link up
	{
		if(status == IFNET_LINKUP)
		{
			pif->status = ENABLE;	
		}
		else
		{
			pif->status = DISABLE;	
		}
	}
	else
	{
		DHCP_LOG_ERROR("Error: Failed ,get interface status\n");   		
	}

	if(pif->status == DISABLE)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Info: interface status down\n");	  
	  //pif->status = ENABLE;	//aaaaaaaaaaaaaaaaafix	 
	}

	if(flag == ENABLE)
	{
		pthread_mutex_lock(&dhcp_lock);
		dhcp_if_add(pif);
		pthread_mutex_unlock(&dhcp_lock);
	}
	dhcp_client_fsm(pif, NULL, DHCP_EVENT_FROM_CMD );

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Info: interface dhcp client enabled\n");
	
	return CMD_SUCCESS; 
}

int dhcp_client_disable(uint32_t ifindex)
{
	struct if_dhcp *pif = NULL;
	struct if_dhcp *pif_save = NULL;

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Info: ifindex %08X\n",ifindex); 
	pif_save = dhcp_if_save_lookup(ifindex);
	if(pif_save){
		DHCP_LOG_ERROR("Info: interface address already save\n");
		return ERRNO_EXISTED;
	}
	
	pif = dhcp_if_lookup(ifindex);
	
	if(NULL != pif && pif->dhcp_role == E_DHCP_CLIENT)	
	{
		//dhcp_if_delete(ifindex);
		
		pif->dhcp_role = E_DHCP_NOTHING;

		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Info: interface dhcp client closed\n");
		
		dhcp_client_fsm(pif , NULL , DHCP_EVENT_FROM_CMD );
		
		return ERRNO_SUCCESS;
	}
	
	DHCP_LOG_ERROR("Error: please check your dhcp configurations first");	
	return ERRNO_FAIL;		
}
int debug_dhcp_ifconfig_get_bulk(struct if_dhcp *pif)
{
	struct in_addr addr;
	if(!pif)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "This pool is not exist!");
		return 1;
	}
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "ifindex is %08X", pif->ifindex);
	if (pif->dhcp_role == E_DHCP_SERVER)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "This interface is dhcp server!");
	} 
	else if (pif->dhcp_role == E_DHCP_CLIENT)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "This interface is dhcp client!");
	} 
	else if (pif->dhcp_role == E_DHCP_RELAY)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "This interface is dhcp relay!");
		if (pif->r_ctrl->nexthop)
		{
			addr.s_addr = pif->r_ctrl->nexthop;
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "The dhcp relay nexthop is %s !", inet_ntoa(addr));
		}
	} 
	else if (pif->dhcp_role == E_DHCP_ZERO)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "This interface is dhcp client zero!");
	} 
	else 
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Don't known the interface is what role!");
	}
	if (pif->save == 1)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "This interface is dhcp client is save!");
	} 
	return 0;
}

struct listnode *dhcp_ifconfig_get_list_nextnode(uint32_t ifindex)
{
	struct if_dhcp *pif = NULL;
	struct listnode *node;
	struct listnode *nextnode;
	int count = 0;

	for(node = dhcp_if_list.head ; node; node = node->next,pif=NULL)
    {
    	if(count != 0)
		{
			nextnode = node;
			return nextnode;
		}
		pif = listgetdata(node);

		if(pif && pif->ifindex == ifindex)
		{
			count++;
		}
	}
	return NULL;
}
int dhcp_ifconfig_get_bulk(void *pdata, int data_len, struct if_dhcp pif[])
{
	int data_num = 0;
	struct if_dhcp *pif_node = NULL;
	struct if_dhcp *pif_save_node = NULL;
	struct listnode *node;
	struct listnode *nextnode;
	int msg_num  = IPC_MSG_LEN/sizeof(struct if_dhcp);

	uint32_t ifindex = *((uint32_t *)pdata);	
	if(ifindex == 0)
	{
		for(node = dhcp_if_list.head; node; node = node->next, pif_node=NULL)
	    {
			pif_node=listgetdata(node);
			if(pif_node)
			{		
				memcpy(&pif[data_num], pif_node, sizeof(struct if_dhcp));
				if(pif_node->dhcp_role != E_DHCP_RELAY && pif_node->dhcp_role != E_DHCP_CLIENT && pif_node->dhcp_role != E_DHCP_SERVER && pif_node->dhcp_role != E_DHCP_ZERO)
				{
					continue;
				}
				pif_save_node = NULL;
				pif_save_node = dhcp_if_save_lookup(pif_node->ifindex);
				if(pif_save_node)
				{
					pif[data_num].save = 1;
					pif_node->save = 1;
				}
				if(pif_node->dhcp_role == E_DHCP_RELAY)
				{
					pif[data_num].nexthop = pif_node->r_ctrl->nexthop;
				}
				debug_dhcp_ifconfig_get_bulk(pif_node);
				data_num++;
				if(data_num == msg_num)
				{
					return data_num;
				}
			}
		}/* end for node = dhcp_if_list.head; node; node = node->next, pif_node=NULL*/
		
	} /* end if ifindex == 0*/
	else
	{
		nextnode = dhcp_ifconfig_get_list_nextnode(ifindex);
		if(nextnode == NULL)
		{
			return data_num;
		}
		for(node = nextnode ; node; node = node->next,pif_node=NULL)
	    {
			pif_node=listgetdata(node);
			if(pif_node)
			{
				memcpy(&pif[data_num], pif_node, sizeof(struct if_dhcp));
				if(pif_node->dhcp_role != E_DHCP_RELAY && pif_node->dhcp_role != E_DHCP_CLIENT && pif_node->dhcp_role != E_DHCP_SERVER && pif_node->dhcp_role != E_DHCP_ZERO)
				{
					continue;
				}
				pif_save_node = NULL;
				pif_save_node = dhcp_if_save_lookup(pif_node->ifindex);
				if(pif_save_node)
				{
					pif[data_num].save = 1;
					pif_node->save = 1;
				}
				if(pif_node->dhcp_role == E_DHCP_RELAY)
				{
					pif[data_num].nexthop = pif_node->r_ctrl->nexthop;
				}
				debug_dhcp_ifconfig_get_bulk(pif_node);

				data_num++;
				if(data_num == msg_num)
				{
					return data_num;
				}
			}
		}/* end for node = nextnode ; node; node = node->next,pif_node=NULL */
	}/* end else*/
	
	return data_num;
}

int dhcp_com_ifconfig_get_bulk(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	int ret = 0;
	int msg_len = 0;

	msg_len = IPC_MSG_LEN/sizeof(struct if_dhcp);
	struct if_dhcp pif[msg_len];
	memset(pif, 0, msg_len*sizeof(struct if_dhcp));
	ret = dhcp_ifconfig_get_bulk(pdata, pmsghdr->data_len, pif);
	
    if(ret > 0)
    {
        ret = ipc_send_reply_n2(pif, ret*sizeof(struct if_dhcp), ret, MODULE_ID_SNMPD, 
                                    MODULE_ID_DHCP,IPC_TYPE_L3IF, pmsghdr->msg_subtype, 0, pmsghdr->msg_index, IPC_OPCODE_REPLY);
    }
    else
    {
        ret = ipc_send_reply_n2(NULL, 0, 0, MODULE_ID_SNMPD, MODULE_ID_DHCP, IPC_TYPE_L3IF,
                                pmsghdr->msg_subtype, 0, 0, IPC_OPCODE_NACK);
    }

	return ret;  
}

int dhcp_if_save_add(struct if_dhcp *pif)
{
	zlog_err("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	if (NULL == pif)					//预防操作空指针
	{
		return 1;
	}

	listnode_add(&dhcp_if_save_list,  pif);
	
	return 0;
}

int dhcp_if_save_delete(uint32_t ifindex)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Entering the function of '%s'--the line of %d", __func__, __LINE__);
	struct if_dhcp *pif = NULL;
	struct listnode *node;

	
	if(0 == ifindex)					
	{
		return 1;
	}
	
    for(node = dhcp_if_save_list.head ;node;node = node->next, pif=NULL)
    {
		pif=listgetdata(node);

		if(pif && pif->ifindex == ifindex)
		{
			pif->status = DISABLE;
			usleep(50);
			listnode_delete(&dhcp_if_save_list,pif);
			XFREE(MTYPE_IF,pif);	
			pif = NULL;
			
			break;
		}
	}
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "leave the function of '%s',--the line of %d", __func__, __LINE__);

	return 0;
}

struct if_dhcp *dhcp_if_save_lookup(uint32_t ifindex)
{
	struct if_dhcp *pif = NULL;
	struct listnode *node;
	
	zlog_err("%s:Entering the function of '%s',ifindex  %08X",__FILE__,__func__,ifindex);

    for(node = dhcp_if_save_list.head; node;node = node->next, pif=NULL)
    {
		pif=listgetdata(node);
		if(pif && pif->ifindex == ifindex)
		{
			break;
		}
	}

	if (NULL == pif)
	{
		zlog_err("%s:%s:fail to find dhcp_if entry according to ifindex of %08X",__FILE__,__func__,ifindex);
		return NULL;
	}
	else
	{
		zlog_err("%s:%s:ok to find dhcp_if entry according to ifindex of %08X",__FILE__,__func__,ifindex);
		return pif;
	}
}

#define DHCP_IP_CONF_FILE "/data/cfg/dhcp_ip.conf"
int dhcp_client_save_enable(uint32_t ifindex)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Entering the function of '%s'", __FUNCTION__);
	struct if_dhcp *pif = NULL;
	struct if_dhcp *pif_save = NULL;
	FILE *dhcp_file = NULL;
	char if_save[64] = {0};
	struct listnode *node;
	if(ifindex == 0) 
	{
		return -1;
	}
	pif = dhcp_if_lookup(ifindex);

	if(pif == NULL)
	{
		DHCP_LOG_ERROR("ERROR:%s lookup ip by ifindex is failed!", __FUNCTION__);
		return -1;
	}
	if(pif->c_ctrl)
	{
		if (pif->c_ctrl->ip == 0 || pif->c_ctrl->mask == 0 || pif->c_ctrl->server_addr == 0)
		{
			DHCP_LOG_ERROR("ERROR:%s pif->c_ctrl is not null dhcp can not get ip!", __FUNCTION__);
			return -1;
		}
	}
	else
	{
		DHCP_LOG_ERROR("ERROR:%s  pif->c_ctrl is null dhcp can not get ip!", __FUNCTION__);
		return -1;
	}
	pif_save = dhcp_if_save_lookup(ifindex);

	if(pif_save != NULL)
	{
		DHCP_LOG_ERROR("ERROR:%s ifindex is added!", __FUNCTION__);
		return -1;
	}
	pif_save =(struct if_dhcp*)XCALLOC(MTYPE_IF, sizeof(struct if_dhcp));
	if(NULL == pif_save) 
	{
		DHCP_LOG_ERROR("Error: There is no ram space\n");		  

		return -1;
	}
	memset(pif_save, 0, sizeof(struct if_dhcp));
	pif_save->ifindex = ifindex;
	pif_save->c_ctrl = (struct client_ctrl*)XCALLOC(MTYPE_IF, sizeof(struct client_ctrl));
	if(pif_save->c_ctrl == NULL)
	{
		DHCP_LOG_ERROR("Error: There is no ram space\n");		  

		return -1;
	}
	memcpy(pif_save, pif, sizeof(struct if_dhcp));
	memcpy(pif_save->c_ctrl, pif->c_ctrl, sizeof(struct client_ctrl));
	dhcp_if_save_add(pif_save);

	dhcp_file = fopen(DHCP_IP_CONF_FILE, "w+");

	if(dhcp_file == NULL)
	{
		DHCP_LOG_ERROR("%s: fopen %s is failed!", __FUNCTION__, DHCP_IP_CONF_FILE);
		return -1;
	}
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, " 0x%x:%08X %08X %08X %08X %08X\n!", ifindex, ntohl(pif->c_ctrl->ip), ntohl(pif->c_ctrl->mask), ntohl(pif->c_ctrl->server_addr),
		ntohl(pif->c_ctrl->gate), ntohl(pif->c_ctrl->dns));
	for(node = dhcp_if_save_list.head ; node; node = node->next,pif=NULL)
    {
		pif = listgetdata(node);
		if(pif)
		{
			memset(if_save, 0, sizeof(if_save));
			sprintf(if_save, "0x%x:%08X %08X %08X %08X %08X %d\n", pif->ifindex, ntohl(pif->c_ctrl->ip), ntohl(pif->c_ctrl->mask), ntohl(pif->c_ctrl->server_addr), 
				ntohl(pif->c_ctrl->gate), ntohl(pif->c_ctrl->dns), pif->c_ctrl->vlanid);
			fprintf(dhcp_file, if_save);
			system("sync");
			sleep(1);
		}
	}

	fclose(dhcp_file);
	return 0;
}

int dhcp_client_save_disable(uint32_t ifindex)
{
	struct if_dhcp *pif = NULL;
	struct if_dhcp *pif_save = NULL;
	FILE *dhcp_file = NULL;
	char if_save[64] = {0};
	struct listnode *node;
	pif_save = dhcp_if_save_lookup(ifindex);

	if(pif_save == NULL)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "Donot find ifindex 0x%x in dhcp_if_save_list, the ifindex is always add!", ifindex);
		return -1;
	}

	if(pif_save->c_ctrl == NULL)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_CLIENT, "pif_save->c_ctrl is NULL!");
		return -1;
	}
	dhcp_if_save_delete(ifindex);
	dhcp_file = fopen(DHCP_IP_CONF_FILE, "w+");

	if(dhcp_file == NULL)
	{
		DHCP_LOG_ERROR("%s: fopen %s is failed!", __FUNCTION__, DHCP_IP_CONF_FILE);
		return -1;
	}

    for(node = dhcp_if_save_list.head; node; node = node->next, pif=NULL)
    {
		pif = listgetdata(node);
		if(pif)
		{
			memset(if_save, 0, sizeof(if_save));
			sprintf(if_save, "0x%x:%08X %08X %08X %08X %08X %d\n", pif->ifindex, ntohl(pif->c_ctrl->ip), ntohl(pif->c_ctrl->mask), ntohl(pif->c_ctrl->server_addr),
				ntohl(pif->c_ctrl->gate), ntohl(pif->c_ctrl->dns), pif->c_ctrl->vlanid);
			fprintf(dhcp_file, if_save);
			system("sync");
			sleep(1);
		}
	}
	fclose(dhcp_file);
	//send_release(pif_save, pif_save->c_ctrl->server_addr, pif_save->c_ctrl->ip);
	//dhcp_ip_address_set(ifindex , 0,0,1); 
	return 0;
}

void dhcp_ip_address_save_init(void)
{
	FILE *dhcp_file = NULL;
	char if_save[64] = {0};
	uint32_t ifindex;
	uint32_t ip_addr;
	uint32_t mask;
	uint32_t server_addr;
	uint32_t gate;
	uint32_t dns;
	uint32_t vlanid = 0;
	
	struct if_dhcp *pif_save = NULL;
	
	dhcp_file = fopen(DHCP_IP_CONF_FILE, "r+");
	if(dhcp_file == NULL)
	{
		DHCP_LOG_ERROR("ERROR:%s fopen %s is failed!", __FUNCTION__, DHCP_IP_CONF_FILE);
		return ;
	}
	while(fgets(if_save, 64, dhcp_file))
	{
		sscanf(if_save, "0x%x:%08X %08X %08X %08X %08X %d", &ifindex, &ip_addr, &mask, &server_addr, &gate, &dns, &vlanid);
		pif_save = dhcp_if_save_lookup(ifindex);
		if(pif_save == NULL)
		{
			pif_save = (struct if_dhcp*)XCALLOC(MTYPE_IF,sizeof(struct if_dhcp));
			memset(pif_save, 0, sizeof(struct if_dhcp));
			pif_save->ifindex = ifindex;
			pif_save->c_ctrl = (struct client_ctrl*)XCALLOC(MTYPE_IF,sizeof(struct client_ctrl));
			pif_save->c_ctrl->ip = ip_addr;
			pif_save->c_ctrl->mask = mask;
			pif_save->c_ctrl->server_addr = server_addr;
			pif_save->c_ctrl->gate = gate;
			pif_save->c_ctrl->dns = dns;
			pif_save->c_ctrl->vlanid = vlanid;
			if(vlanid)
			{
				pif_save->c_ctrl->ifvalid = 1;
			}
			dhcp_if_save_add(pif_save);
			DHCP_LOG_ERROR("%s:%d ifindex 0x%x, ip %08X, server_addr %08X, gate %08X, dns %08X\n", __FUNCTION__, __LINE__, ifindex, ip_addr, server_addr, gate, dns);
			//free(pif_save->c_ctrl);
			//free(pif_save);
			//pif = dhcp_if_save_lookup(ifindex);
		}
	}	
		
	fclose(dhcp_file);
	return ;
}

/*检查MAC地址是否合法*/
int dhcp_mac_is_empty(uchar *mac)
{
    if ( mac == NULL )
    {
        return 1;
    }

  	if ( mac[0] == 0 && mac[1] == 0
              && mac[2] == 0 && mac[3] == 0
              && mac[4] == 0 && mac[5] == 0 )
    {
        zlog_err ( "%s[%d] can't set mac all zero for port\n", __FUNCTION__, __LINE__ );
        return 1;
    }
    return 0;
}

