/**
 * \page cmds_ref_dhcpv6 DHCPv6
 * - \subpage modify_log_dhcpv6
 * 
 */
 
/**
 * \page modify_log_dhcpv6 Modify Log
 * \section dhcpv6-v007r004 HiOS-V007R004
 *  -# 
 * \section dhcpv6-v007r003 HiOS-V007R003
 *  -# 
 */
 
#include <zebra.h>
#include "command.h"
#include "vtysh.h"
extern struct cmd_node physical_if_node;

static struct cmd_node dhcpv6_node =
{
  DHCPV6_NODE,
  "%s(config-dhcpv6)# ",
  1,
};


DEFUNSH(VTYSH_DHCPV6,
	    dhcpv6_name,
		dhcpv6_name_cmd,
		"dhcpv6",
		"Specify IP configurations for the system\n")
{
	vty->node = DHCPV6_NODE;	
	return CMD_SUCCESS;
}

DEFUNSH (VTYSH_DHCPV6,
	vtysh_exit_dhcpv6,
	vtysh_exit_dhcpv6_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}

DEFSH (VTYSH_DHCPV6,
		dhcpv6_duid_cmd_vtysh,
		"dhcpv6 duid STRING<1-64>",
		"Dynamic Host Configuration Protocol for IPv6\n"
		"DHCPv6 unique identifier\n"
		"DUID hex string only with digits and letters 'A' to 'F' or 'a'to 'f'.\n")

DEFSH (VTYSH_DHCPV6,
		dhcpv6_keyinfo_cmd_vtysh,
		"dhcpv6 key STRING<1-64>",
		"Dynamic Host Configuration Protocol for IPv6\n"
		"DHCPv6  key configure command\n"
		"key hex string only with digits and letters 'A' to 'F' or 'a'to 'f'.\n")
#if 0		
DEFSH (VTYSH_DHCPV6,
		ip_address_dhcpv6_alloc_vtysh,
		"ethernet USP ip address dhcpv6",
		"Ethernet interface\n"
		"The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
	    "Specify IP configurations for interfaces\n"
	    "Address\n"
	    "IP address allocated by DHCPv6\n")
#endif
DEFSH (VTYSH_DHCPV6,
		no_ip_address_dhcpv6_alloc_vtysh,
		"no ethernet USP ip address dhcpv6",
		"disable interface\n"
		"Ethernet interface\n"
		"The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
	    "Specify IP configurations for interfaces\n"
	    "Address\n"
	    "IP address allocated by DHCPv6\n")

DEFSH (VTYSH_DHCPV6,
		show_dhcpv6_duid_string_vtysh,
		"show dhcpv6 duid",
		"show duid string\n"
		"Dynamic Host Configuration Protocol for IPv6\n"
		"DUID hex string only with digits and letters 'A' to 'F' or 'a'to 'f'.\n")

DEFSH (VTYSH_DHCPV6,
		interface_iana_set_vtysh,
		"ethernet USP iana STRING",
		"Ethernet interface\n"
		"The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
	    "iana\n"
	    "iana string\n")
DEFSH (VTYSH_DHCPV6,
		show_dhcpv6_client_vtysh,
		"show dhcpv6 client",
		"show dhcpv6 clinet status infomation\n"
		"Dynamic Host Configuration Protocol for IPv6\n"
	    "client\n")
DEFSH (VTYSH_DHCPV6,
		ip_address_pd_dhcpv6_alloc_vtysh,
		"ethernet USP ip address dhcpv6 pd",
		"Ethernet interface\n"
		"The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
		"Specify IP configurations for interfaces\n"
		"Address\n"
		"IP address allocated by DHCPv6\n"
		"IP address allocated by DHCPv6\n")
		
DEFSH (VTYSH_DHCPV6,
		no_ip_address_pd_dhcpv6_alloc_vtysh,
		"no ethernet USP ip address dhcpv6 pd",
		"disable interface\n"
		"Ethernet interface\n"
		"The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
		"Specify IP configurations for interfaces\n"
		"Address\n"
		"IP address allocated by DHCPv6\n"
		"IP address allocated by DHCPv6\n")
/* debug 日志发送到 syslog 使能状态设置 */
DEFSH (VTYSH_DHCPV6,dhcpv6_log_level_ctl_cmd_vtysh,	"debug dhcpv6(enable | disable)",		
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n")	
 
/* debug 日志发送到 syslog 使能状态显示 */
DEFSH (VTYSH_DHCPV6,dhcpv6_show_log_level_ctl_cmd_vtysh,"show debug dhcpv6",		
	SHOW_STR"Output log of debug level\n""Program name\n")	


void vtysh_init_dhcpv6_cmd ()
{
  	install_node (&dhcpv6_node, NULL);
	vtysh_install_default (DHCPV6_NODE);
	install_element_level (DHCPV6_NODE, &vtysh_exit_dhcpv6_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &dhcpv6_name_cmd, VISIT_LEVE, CMD_SYNC); 

	install_element_level (DHCPV6_NODE, &dhcpv6_duid_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (DHCPV6_NODE, &dhcpv6_keyinfo_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &dhcpv6_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &dhcpv6_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (DHCPV6_NODE, &show_dhcpv6_duid_string_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	//install_element_level (DHCPV6_NODE, &interface_iana_set_vtysh, CONFIG_LEVE_5);
	install_element_level (CONFIG_NODE, &show_dhcpv6_client_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (DHCPV6_NODE, &ip_address_pd_dhcpv6_alloc_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(DHCPV6_NODE, &no_ip_address_pd_dhcpv6_alloc_vtysh, CONFIG_LEVE_5, CMD_SYNC);

}



