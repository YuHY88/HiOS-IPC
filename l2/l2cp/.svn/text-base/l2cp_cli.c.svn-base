
#include "l2cp.h"
#include <lib/log.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lib/memory.h>
#include <lib/oam_common.h>
#include <lib/command.h>
#include <lib/ether.h>
#include <memory.h>
#include <lib/pkt_type.h>


extern void install_element (enum node_type ntype, struct cmd_element *cmd, enum sync_type sync_flag);
extern struct l2if * l2if_get(uint32_t ifindex);
extern uchar *ether_string_to_mac (const char *str, uchar *mac);

struct l2cp_dmac group_mac[3] = 
{
	{{0x01, 0x00, 0x0c, 0xcd, 0xcd, 0xd0}},
	{{0x01, 0x00, 0x0c, 0xcd, 0xcd, 0xd1}},
	{{0x01, 0x00, 0x0c, 0xcd, 0xcd, 0xd2}},
};

struct l2cp_dmac stp_mac = {{0x1, 0x80, 0xc2, 0x00, 0x00, 0x00}};
struct l2cp_dmac lacp_mac = {{0x1, 0x80, 0xc2, 0x00, 0x00, 0x02}};
struct l2cp_dmac lldp_mac = {{0x1, 0x80, 0xc2, 0x00, 0x00, 0x0e}};
struct l2cp_dmac eoam_mac = {{0x1, 0x80, 0xc2, 0x00, 0x00, 0x02}}; 
struct l2cp_dmac dot1x_mac = {{0x1, 0x80, 0xc2, 0x00, 0x00, 0x03}};//EAPOL


DEFUN (l2cp_mpls_func,
     l2cp_mpls_func_cmd,
     "l2cp (eoam|lacp|lldp|stp|dot1x) mpls (vsi|vc-id) <1-4294967295>",
     "Configure Layer2 Protocol Tunnelling \n"
     "Ethernet Operation, Administration and Maintenance \n"
     "Link Aggregation (LACP) \n"
	 "Link Layer Discover Protocol \n"
	 "Spanning Tree Protocols \n"
     "Port Authentication (802.1 X) \n"
   	 "L2cp tunnel over MPLS \n"
   	 "L2cp tunnel over VPLS \n"
   	 "L2cp tunnel over VPWS \n"
     "vc-id: 1-4294967295 vsi:1-1024"
     )
{

	enum hal_l2_proto 	protocol;
    uint32_t ifindex = 0;
	uint32_t vc_id = 0;
    u_int8_t 			dmac[6];
	u_int16_t 			ethType	= ETHTYPE_INVALID;
	int 				ret;
	enum  l2cp_mode     mode;
    struct l2if * p_l2if = NULL;
	 
	ifindex = (uint32_t)vty->index;	

    p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		vty_error_out(vty,"%s[%d]:leave %s:error:fail to malloc if l2 table bucket \r\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}
	
	protocol = l2cp_str_to_protocol ((u_int8_t *)argv[0]);
    
    if(protocol == HAL_PROTO_MAX)
    {
		vty_error_out (vty, " The protocol type entered is not l2cp protocol \r\n");
		return CMD_WARNING;
    }

    memset(dmac, 0, 6);
	switch	(protocol)     //process dmac
	{
		case HAL_PROTO_STP:
		case HAL_PROTO_RSTP:
		case HAL_PROTO_MSTP:
			memcpy(dmac, stp_mac.mac, 6);
			break;
		case HAL_PROTO_LACP:
			ethType = 0x8809;
			memcpy(dmac, lacp_mac.mac, 6);
			break;
		case HAL_PROTO_LLDP:
			ethType = 0x88CC;
			memcpy(dmac, lldp_mac.mac, 6);
			break;
		case HAL_PROTO_EOAM:
			ethType = 0x8809;
			memcpy(dmac, eoam_mac.mac, 6);
			break;
		case HAL_PROTO_DOT1X:
			ethType = 0x888E;
			memcpy(dmac, dot1x_mac.mac, 6);
			break;
		default:
			vty_error_out (vty, " Not support this protocol! \r\n");
			return CMD_WARNING;
	}

    if (0 == strncmp (argv[1], "vc-id", 2))
	{
		mode = L2CP_BASE_VPWS;
	}
	else if(0 == strncmp (argv[1], "vsi", 2))
    {
    	mode = L2CP_BASE_VPLS;
    }
	else
	{
		vty_error_out (vty, " Not support this action! \r\n");
		return CMD_WARNING; 
	}
	
    vc_id = strtoul(argv[2], NULL, 10);
	if((vc_id > 1024) && (L2CP_BASE_VPLS == mode))
	{
		vty_error_out (vty, " The range of vsi must be 1 to 1024 \r\n");
		return CMD_WARNING; 
	}
	
	ret = l2cp_mpls_process(p_l2if, protocol, dmac, ethType, mode, vc_id);
    if (ret != L2CP_SUCCESS)
	{
		if(ret == ERROR_LINK_DOWN)
		{
			vty_error_out (vty, "Configuration failed:port link status is down. \r\n");
		}
		else if(ret == ERROR_PORT_MESS)
		{
			vty_error_out (vty, "Configuration failed:vsi or vc_id is not bound to this port or subinterface. \r\n");
		}
		else if(ret == ERROR_EGRESS_DELETE_VLAN)
		{
			vty_error_out (vty, "Configuration failed:Failed to delete egress access vlan. \r\n");
		}
		else 
		{
			vty_error_out (vty, "Configuration l2cp over mpls failed.\r\n");
		}
	    return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN (l2cp_discard_func,
     l2cp_discard_func_cmd,
     "l2cp (eoam|lacp|lldp|stp|dot1x) discard",
     "Configure Layer2 Protocol Tunnelling \n"
     "Ethernet Operation, Administration and Maintenance \n"
   /* "GMRP Multicast Registration Protocol \n"
     "GVRP VLAN Registration Protocol \n"*/
     "Link Aggregation (LACP) \n"
	 "Link Layer Discover Protocol \n"
	 "Spanning Tree Protocols \n"
     "Port Authentication (802.1 X) \n"
     "Discard the protocol data unit"
     )
{

	enum hal_l2_proto 	protocol;
    uint32_t ifindex = 0;
    u_int8_t 			dmac[6];

	enum l2cp_action 	action;
	u_int16_t 			ethType	= ETHTYPE_INVALID;
	int 				ret;
    struct l2if * p_l2if = NULL;
	 
	ifindex = (uint32_t)vty->index;

    p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		vty_error_out(vty,"%s[%d]:leave %s:error:fail to malloc if l2 table bucket \r\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}
	protocol = l2cp_str_to_protocol ((u_int8_t *)argv[0]);
    
    if(protocol == HAL_PROTO_MAX)
    {
		vty_error_out (vty, " The protocol type entered is not l2cp protocol \r\n");
		return CMD_WARNING;
    }

	if(p_l2if->mode != IFNET_MODE_SWITCH)
	{
		vty_error_out (vty, " The port must be in switch mode to configure l2cp \r\n");
		return CMD_WARNING;	
	}

    memset(dmac, 0, 6);
	switch	(protocol)     //process dmac
	{
		case HAL_PROTO_STP:
		case HAL_PROTO_RSTP:
		case HAL_PROTO_MSTP:
			memcpy(dmac, stp_mac.mac, 6);
			break;
		/*case HAL_PROTO_GMRP:
			memcpy(dmac, gmrp_mac.mac, 6);
			break;
		case HAL_PROTO_GVRP:
			memcpy(dmac, gvrp_mac.mac, 6);
			break;*/
		case HAL_PROTO_LACP:
			ethType = 0x8809;
			memcpy(dmac, lacp_mac.mac, 6);
			break;
		case HAL_PROTO_LLDP:
			ethType = 0x88CC;
			memcpy(dmac, lldp_mac.mac, 6);
			break;
		case HAL_PROTO_EOAM:
			ethType = 0x8809;
			memcpy(dmac, eoam_mac.mac, 6);
			break;
		case HAL_PROTO_DOT1X:
			ethType = 0x888E;
			memcpy(dmac, dot1x_mac.mac, 6);
			break;
		default:
			vty_error_out (vty, " Not support this protocol! \r\n");
			return CMD_WARNING;
	}

	action = L2CP_DISCARD;

	ret = l2cp_discard_process(p_l2if, protocol, dmac, ethType, action);
	if (ret != L2CP_SUCCESS)
	{
		vty_error_out (vty, " Config l2cp discard failed \r\n");
	    return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN (l2cp_tunnel_func,
     l2cp_tunnel_func_cmd,
     "l2cp (eoam|lacp|lldp|stp|dot1x) tunnel {dmac XX:XX:XX:XX:XX:XX} {vlan <1-4094> cos <0-7>}",
     "Configure Layer2 Protocol Tunnelling \n"
     "Ethernet Operation, Administration and Maintenance \n"
   /* "GMRP Multicast Registration Protocol \n"
     "GVRP VLAN Registration Protocol \n"*/
     "Link Aggregation (LACP) \n"
	 "Link Layer Discover Protocol \n"
	 "Spanning Tree Protocols \n"
     "Port Authentication (802.1 X) \n"
     "Tunnel the protocol data unit \n"
     "Change l2cp Dest MAC to Group Dest Mac Address \n"
     "Specify Group Dest Mac Address:01:00:0c:cd:cd:d0,01:00:0c:cd:cd:d1,01:00:0c:cd:cd:d2 \n"
     "Vlan \n"
     "Specify Vlan id, 1-4094 \n"
     "Cos \n"
     "Specify Cos id, 0-7"
     )
{

	enum hal_l2_proto 	protocol;
    uint32_t ifindex = 0;
    u_int8_t 			dmac[6];
    u_int8_t 			gmac[6];
    u_int16_t 			vlan_id = VLAN_INVALID;
    u_int8_t 			cos_id 	= COS_DEFAULT;
	enum l2cp_action 	action;
	enum  l2cp_mode     mode;
	u_int16_t 			ethType	= ETHTYPE_INVALID;
	int 				ret;
    struct l2if * p_l2if = NULL;
	 
	ifindex = (uint32_t)vty->index;

    p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		vty_error_out(vty,"%s[%d]:leave %s:error:fail to malloc if l2 table bucket \r\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}
	protocol = l2cp_str_to_protocol ((u_int8_t *)argv[0]);
    
    if(protocol == HAL_PROTO_MAX)
    {
		vty_error_out (vty, " The protocol type entered is not l2cp protocol \r\n");
		return CMD_WARNING;
    }

	if(p_l2if->mode != IFNET_MODE_SWITCH)
	{
		vty_error_out (vty, " The port must be in switch mode to configure l2cp \r\n");
		return CMD_WARNING;	
	}

    memset(dmac, 0, 6);
	switch	(protocol)     //process dmac
	{
		case HAL_PROTO_STP:
		case HAL_PROTO_RSTP:
		case HAL_PROTO_MSTP:
			memcpy(dmac, stp_mac.mac, 6);
			break;
		/*case HAL_PROTO_GMRP:
			memcpy(dmac, gmrp_mac.mac, 6);
			break;
		case HAL_PROTO_GVRP:
			memcpy(dmac, gvrp_mac.mac, 6);
			break;*/
		case HAL_PROTO_LACP:
			ethType = 0x8809;
			memcpy(dmac, lacp_mac.mac, 6);
			break;
		case HAL_PROTO_LLDP:
			ethType = 0x88CC;
			memcpy(dmac, lldp_mac.mac, 6);
			break;
		case HAL_PROTO_EOAM:
			ethType = 0x8809;
			memcpy(dmac, eoam_mac.mac, 6);
			break;
		case HAL_PROTO_DOT1X:
			ethType = 0x888E;
			memcpy(dmac, dot1x_mac.mac, 6);
			break;
		default:
			vty_error_out (vty, " Not support this protocol!\r\n");
			return CMD_WARNING;
	}

	action = L2CP_TUNNEL;
    memset(gmac, 0, 6);
    memcpy(gmac,group_mac[0].mac,6);
    
    if(NULL != argv[1])
    {
    	ether_string_to_mac(argv[1],gmac);
		if (memcmp(gmac, group_mac[0].mac, 6) != 0
			&& memcmp(gmac, group_mac[1].mac, 6) != 0
			&& memcmp(gmac, group_mac[2].mac, 6) != 0)
		{
			vty_error_out (vty, " this protocol dest mac should be 01:00:0c:cd:cd:d0,01:00:0c:cd:cd:d1 or 01:00:0c:cd:cd:d2 \r\n");
			return CMD_WARNING;
		} 
    }
    if(NULL != argv[2])
    {
       vlan_id =  atoi(argv[2]); 
       if(vlan_id == 0 || vlan_id >= VLAN_INVALID)
       {
          vty_error_out (vty, " The input vlan id is not valid.this value should be 1~4094 \r\n");
		  return CMD_WARNING;
       }
    }
    if(NULL != argv[3])
    {
        cos_id =  atoi(argv[3]);
        if(cos_id > VLAN_MAX_PRIORITY)
        {
            vty_error_out (vty, " The input cos id is not valid.this value should be 0~7\r\n");
		    return CMD_WARNING;
        }
    }
    		
	if (action == L2CP_TUNNEL)
	{
		if(vlan_id == VLAN_INVALID)
		{
			mode = L2CP_BASE_MAC;
		}
		else
		{
			mode = L2CP_BASE_VLAN;
		}
		
		ret = l2cp_tunnel_process(p_l2if, protocol, mode, dmac, ethType, gmac, vlan_id, cos_id);
        if (ret != L2CP_SUCCESS)
		{
			vty_error_out (vty, " Config l2cp tunnel failed \r\n");
		    return CMD_WARNING;
		}
	}
	else
	{
        vty_error_out (vty, " unkonw action \r\n");
		return CMD_WARNING;
    }
	
	return CMD_SUCCESS;
}


DEFUN(l2cp_config_show,
	l2cp_config_show_cmd,
	"show l2cp",
	"Show running system information \n"
	"show l2cp config")
{
    
    uint32_t ifindex = 0;
    struct l2if * p_l2if = NULL;
	 
	ifindex = (uint32_t)vty->index;

    p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		vty_error_out(vty,"%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}

	{
		/* All interface print. */
		vty_out (vty, "\r%22s%s \n", " ", "L2CP Tunnel over mac");
		vty_out (vty, "\r%s \n", "===========================================================================");
		vty_out (vty, "\r%-10s%-14s%-19s%-19s%-6s%-5s \n", 
					 "Protocol", "EthernetType", "Protocol-Mac", "D-MAC", "VLAN", "Pri");
		vty_out (vty, "\r%s \n", "---------------------------------------------------------------------------");

		l2cp_show_config(vty, p_l2if, L2CP_TUNNEL, L2CP_BASE_VLAN);

		vty_out (vty, "\r \n");
		vty_out (vty, "\r%22s%s \n", " ", "L2CP Tunnel over mpls");
		vty_out (vty, "\r%s \n", "===========================================================================");
		vty_out (vty, "\r%-10s%-14s%-20s%-14s%-6s \n", 
					 "Protocol", "EthernetType", "Protocol-Mac", "VC-id", "VSI");
		vty_out (vty, "\r%s \n", "---------------------------------------------------------------------------");

		l2cp_show_config(vty, p_l2if, L2CP_TUNNEL, L2CP_BASE_VPWS);
		
		vty_out (vty, "\r \n");
		vty_out (vty, "\r%26s%s \n", " ", "L2CP Discard");
		vty_out (vty, "\r%s \n", "===========================================================================");
		vty_out (vty, "\r%-10s%-14s%-16s \n", 
						 "Protocol", "EthernetType", "Protocol-Mac");
		vty_out (vty, "\r%s \n", "---------------------------------------------------------------------------");
		
		l2cp_show_config(vty, p_l2if, L2CP_DISCARD, 0);
        vty_out (vty, " \r\n");	
	}

	return CMD_SUCCESS;
}

DEFUN (no_l2cp_tunnel,
     no_l2cp_tunnel_cmd,
     "no l2cp (eoam|lacp|lldp|stp|dot1x|all)", 
	 "Disable Layer2 Protocol Function \n"
	 "Configure Layer2 Protocol Tunnelling \n"
     "Ethernet Operation, Administration and Maintenance \n"
    /* "GMRP Multicast Registration Protocol \n"
     "GVRP VLAN Registration Protocol \n"*/
     "Link Aggregation (LACP) \n"
	 "Link Layer Discover Protocol \n"
	 "Spanning Tree Protocols \n"
     "Port Authentication (802.1 X) \n"
     "delete all l2cp protocol \n"
     "Discard the protocol data unit \n"
     "Tunnel the protocol data unit ")
{
	enum hal_l2_proto 	protocol;
    uint32_t ifindex = 0;
    u_int8_t 			dmac[6];
	u_int16_t 			ethType	= ETHTYPE_INVALID;
	int 				ret;
    struct l2if * p_l2if = NULL;
	 
	ifindex = (uint32_t)vty->index;

    p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		vty_error_out(vty,"%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket \r\n",__FILE__,__LINE__,__FUNCTION__);
		return CMD_WARNING;
	}
	protocol = l2cp_str_to_protocol ((u_int8_t *)argv[0]);
    
    if(protocol == HAL_PROTO_MAX)
    {
		vty_error_out (vty, " The protocol type entered is not l2cp protocol \r\n");
		return CMD_WARNING;
    }
	
	memset(dmac, 0, 6);	
	{
		switch	(protocol)    
		{
			case HAL_PROTO_STP:
			case HAL_PROTO_RSTP:
			case HAL_PROTO_MSTP:
				memcpy(dmac, stp_mac.mac, 6);
				break;
			/*case HAL_PROTO_GMRP:
				memcpy(dmac, gmrp_mac.mac, 6);
				break;
    		case HAL_PROTO_GVRP:
				memcpy(dmac, gvrp_mac.mac, 6);
				break;*/
			case HAL_PROTO_LACP:
				ethType = 0x8809;
				memcpy(dmac, lacp_mac.mac, 6);
				break;
			case HAL_PROTO_LLDP:
				ethType = 0x88CC;
				memcpy(dmac, lldp_mac.mac, 6);
				break;
			case HAL_PROTO_EOAM:
				ethType = 0x8809;
				memcpy(dmac, eoam_mac.mac, 6);
				break;
			case HAL_PROTO_DOT1X:
				ethType = 0x888E;
				memcpy(dmac, dot1x_mac.mac, 6);
				break;
            case HAL_PROTO_ALL:
				break;
			default:
				vty_error_out (vty, " Not support this protocol! \r\n");
				return CMD_WARNING;
		}
	}

	ret = l2cp_no_action(p_l2if, protocol, dmac, ethType);
	if (ret != L2CP_SUCCESS)
	{
		vty_error_out (vty, " Disable l2cp failed \r\n");
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


void
l2cp_cli_init(void)
{
    install_element (PHYSICAL_IF_NODE, &l2cp_tunnel_func_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &l2cp_discard_func_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &l2cp_config_show_cmd, CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &no_l2cp_tunnel_cmd, CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &l2cp_mpls_func_cmd, CMD_SYNC);
}

