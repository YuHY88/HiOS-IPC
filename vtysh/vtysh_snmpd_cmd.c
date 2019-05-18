/**
 * \page cmds_ref_snmp SNMP
 * - \subpage modify_log_snmp
 * 
 */
 
/**
 * \page modify_log_snmp Modify Log
 * \section snmp-v007r004 HiOS-V007R004
 *  -# 
 * \section snmp-v007r003 HiOS-V007R003
 *  -# 
 */

#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "lib/ifm_common.h"

static struct cmd_node snmp_node =
{
	SNMPD_NODE,
	"%s(config-snmp)#",
};

DEFUNSH(VTYSH_SNMPD,
		snmp_mode_enable,
		snmp_mode_enable_cmd,
		"snmp",
		"snmp command node\n")
{
	vty->node = SNMPD_NODE;

	return CMD_SUCCESS;
}

DEFUNSH(VTYSH_SNMPD,
	vtysh_exit_snmp,
	vtysh_exit_snmp_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}


ALIAS (vtysh_exit_snmp,
	   vtysh_quit_snmp_cmd,
	   "quit",
	   "Exit current mode and down to previous mode\n")

DEFSH (VTYSH_SNMPD,
       snmp_conf_debug_cmd,
       "debug snmp (enable|disable) (all|receive_send|mib_modules|trap|cache)",
       "Debub config\n"
       "snmp config\n"
       "snmp debug enable\n"
       "snmp debug disable\n"
       "snmp debug type all\n"
       "snmp debug type receive /send to ftm \n"     
       "snmp debug type mib_modules\n"
       "snmp trap\n"
       "snmp cache\n");



DEFSH(VTYSH_SNMPD,
	snmp_version_config_cmd,
	"snmp version (v1|v2c|v3|all)",
	"snmp\n"
	"version of snmp\n"
	"version v1\n"
	"version v2c\n"
	"version v3\n"
	"version v1/v2c/v3\n");

DEFSH(VTYSH_SNMPD ,
	no_snmp_version_config_cmd ,
	"no snmp version",
	"delete\n"
	"snmp\n"
	"version of snmp\n") ;


DEFSH(VTYSH_SNMPD ,
	snmp_v1v2c_server_config_cmd ,
	"snmp server A.B.C.D {udp-port <1-65535> | vpn <1-128>} version (v1|v2c) community WORD",
	"snmp\n"
	"snmp server\n"
	"snmp server IP\n"
	"server port\n"
	"port number\n"
	"server vpn\n"
	"vpn number\n"
	"version of snmp\n"
	"version v1\n"
	"version v2c\n"
	"community item\n"
	"community name <1-31>\n");


DEFSH(VTYSH_SNMPD ,
	no_snmp_v1v2c_server_config_cmd ,
	"no snmp server A.B.C.D {vpn <1-128>} version (v1|v2c) community WORD",
	"delete\n"
	"snmp\n"
	"snmp server\n"
	"snmp server IP\n"
	"server vpn\n"
	"vpn number 1-1024\n"
	"version of snmp\n"
	"version v1\n"
	"version v2c\n"
	"community item\n"
	"community name <1-31>\n");

DEFSH(VTYSH_SNMPD ,
	snmp_v3_server_config_cmd ,
	"snmp server A.B.C.D {udp-port <1-65535> | vpn <1-128>} version v3 user WORD",
	"snmp\n"
	"snmp server\n"
	"snmp server IP\n"
	"server port\n"
	"port number\n"
	"server vpn\n"
	"vpn number\n"
	"version of snmp\n"
	"version v3\n"
	"user item\n"
	"user name <1-31>\n");


DEFSH(VTYSH_SNMPD ,
	no_snmp_v3_server_config_cmd ,
	"no snmp server A.B.C.D {vpn <1-128>} version v3 user WORD",
	"delete\n"
	"snmp\n"
	"snmp server\n"
	"snmp server IP\n"
	"server vpn\n"
	"vpn number\n"
	"version of snmp\n"
	"version v3\n"
	"user item\n"
	"user name <1-31>\n");


DEFSH(VTYSH_SNMPD ,
	snmp_community_config_cmd ,
	"snmp community WORD level (ro|rw)",
	"snmp\n"
	"snmp community\n"
	"community name <1-31>\n"
	"level\n"
	"read only\n"
	"read and write\n");


DEFSH(VTYSH_SNMPD ,
	no_snmp_community_config_cmd ,
	"no snmp community WORD",
	"delete\n"
	"snmp\n"
	"snmp community\n"
	"community name <1-31>\n");

DEFSH(VTYSH_SNMPD ,
	snmp_user_config_cmd ,
	"snmp user WORD",
	"snmp\n"
	"snmp user\n"
	"user name <1-31>\n");

DEFSH(VTYSH_SNMPD ,
	snmp_user_config2_cmd ,
	"snmp user WORD auth (md5|sha) auth-password WORD {priv des priv-password WORD}",
	"snmp\n"
	"snmp user\n"
	"user name<1-31>\n"
	"auth protocol\n"
	"md5\n"
	"sha\n"
	"auth-password<8-31>\n"
	"password\n"
	"priv protocol\n"
	"des\n"
	"priv-password<8-31>\n"
	"password\n");


DEFSH(VTYSH_SNMPD ,
	no_snmp_user_config_cmd ,
	"no snmp user WORD",
	"delete\n"
	"snmp\n"
	"snmp user\n"
	"user name <1-31>\n");

DEFSH(VTYSH_SNMPD ,
	snmp_user_privilege_config_cmd,
	"snmp user WORD privilege (guest|operator|admin)",
	"snmp\n"
	"snmp user\n"
	"user name <1-31>\n"
	"user privilege\n"
	"guest privilege\n"
	"operator privilege\n"
	"admin privilege\n");



DEFSH(VTYSH_SNMPD,
	no_snmp_user_privilege_config_cmd,
	"no snmp user WORD privilege",
	"delete\n"
	"snmp\n"
	"snmp user\n"
	"user name <1-31>\n"
	"user privilege\n");


DEFSH(VTYSH_SNMPD,
	show_snmp_config_cmd,
	"show snmp config",
	SHOW_STR
	"snmp\n"
	"snmp config information\n");

DEFSH(VTYSH_SNMPD,
	show_snmp_user_cmd,
	"show snmp user",
	SHOW_STR
	"snmp\n"
	"snmp user information\n");

DEFSH(VTYSH_SNMPD,
	show_snmp_statistics_cmd,
	"show snmp statistics",
	SHOW_STR
	"snmp\n"
	"snmp packet statistics information\n");

DEFSH(VTYSH_SNMPD, snmpd_log_level_ctl_cmd_vtysh, "debug snmpd(enable | disable)",
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n");

DEFSH(VTYSH_SNMPD, snmpd_show_log_level_ctl_cmd_vtysh, "show debug snmpd",
	SHOW_STR"Output log of debug level\n""Program name\n");

/*-----------------------------------added by lidingcheng for rfc1213 system group 2018-04-13----------------------------*/
DEFSH(VTYSH_SNMPD,
    snmp_sysinfo_contact_cmd,
    "snmp-agent sys-info contact WORD ",    
    "snmp-agent\n"
    "sys-info\n"
    "contact\n"
    "contact <1-255>\n");


DEFSH(VTYSH_SNMPD,
    snmp_sysinfo_name_cmd ,
    "snmp-agent sys-info sysname WORD",    
    "snmp-agent\n"
    "sys-info\n"
    "sysname\n"
    "sysname <1-255>\n");

DEFSH(VTYSH_SNMPD,
    snmp_sysinfo_location_cmd,
    "snmp-agent sys-info location WORD",    
    "snmp\n"
    "sys-info\n"
    "location\n"
    "location <1-255>\n");


DEFSH(VTYSH_SNMPD,
	snmp_sysinfo_show_cmd,
	"display snmp-agent sys-info",
	"display "
	"snmp-agent \n"
	"system group\n");


DEFSH(VTYSH_SNMPD,
    undo_snmp_sysinfo_contact_cmd,
    "undo snmp-agent sys-info contact",
    "undo"
    "snmp-agent\n"
    "sys-info\n"
    "contact\n") ;

DEFSH(VTYSH_SNMPD,
    undo_snmp_sysinfo_location_cmd,
    "undo snmp-agent sys-info location",
    "undo"
    "snmp-agent\n"
    "sys-info\n"
    "location\n");

DEFSH(VTYSH_SNMPD,
    undo_snmp_sysinfo_sysname_cmd,
    "undo snmp-agent sys-info sysname",
    "undo"
    "snmp-agent\n"
    "sys-info\n"
    "contact\n");

DEFSH(VTYSH_SNMPD,
    snmp_trap_enable_cmd,
    "snmp-agent trap enable",    
    "snmp-agent\n"
    "trap\n"
    "enable\n") ;

DEFSH(VTYSH_SNMPD,
	undo_snmp_trap_enable_cmd,
	"undo snmp-agent trap enable",	 
	"undo\n"
	"snmp-agent\n"
	"trap\n"
	"enable\n") ;


DEFSH(VTYSH_SNMPD,
    snmp_linkup_down_enable_cmd,
    "enable snmp trap updown",
    "enable\n"
    "snmp\n"      
    "trap\n"
    "updown\n");

DEFSH(VTYSH_SNMPD,
      undo_snmp_linkup_down_enable_cmd,
      "undo enable snmp trap updown",
      "undo\n"
      "enable\n"
      "snmp\n"      
      "trap\n"
      "updown\n");

DEFSH(VTYSH_SNMPD,
	description_snmp_config_cmd,
	"description SNMP",
	"description\n"
	"snmp\n");

DEFSH(VTYSH_SNMPD ,
    h3c_community_config_cmd ,
    "snmp-agent community (read|write) WORD acl <2000-2999>",
    "snmp-agent\n"
    "snmp-agent community\n"
    "read only\n"
    "read and write\n"
    "community name <1-31>\n"
    "acl\n"
    "acl number\n");

DEFSH(VTYSH_SNMPD ,
	undo_h3c_community_config_cmd ,
	"undo snmp-agent community WORD",
	"delete\n"
	"snmp-agent\n"
	"snmp-agent community\n"
	"community name <1-31>\n");

DEFSH(VTYSH_SNMPD,
	h3c_version_config_cmd,
	"snmp-agent sys-info version (v1|v2c|v3|all)",
	"snmp\n"
	"sys-info\n"
	"version of snmp\n"
	"version v1\n"
	"version v2c\n"
	"version v3\n"
	"version v1/v2c/v3\n");

DEFSH(VTYSH_SNMPD,
	h3c_dying_gasp_cmd,
	"dying-gasp host  A.B.C.D {vpn-instance <1-128>} snmp-trap version (v1|v2c) securityname WORD",
	"dying-gasp \n"
	"host \n"
	"target address \n"
	"vpn-instance \n"
	"vpn value \n"
	"snmp-trap \n"
	"trap version \n"
	"v1\n"
	"v2c\n"
	"securityname \n"
	"securityname \n");

DEFSH(VTYSH_SNMPD,
    h3c_h3c_mib_view_config_cmd,
    "snmp-agent mib-view include iso-view iso",
    "snmp-agent\n"
    "mib-view \n"
    "include\n"
    "iso-view\n"
    "iso \n");


/*-----------------------------------lidingcheng added end  2018-04-13----------------------------*/

DEFSH(VTYSH_SNMPD,
      snmp_vpn_config_cmd ,
      "snmp vpn-instance <1-128> ",
      "snmp\n"
      "snmp vpn-instance\n"
      "vpn value\n") ;


DEFSH(VTYSH_SNMPD,
       no_snmp_vpn_config_cmd ,
      "no snmp vpn-instance",
      "no\n"
      "snmp\n"
      "snmp vpn-instance\n") ;

DEFSH(VTYSH_SNMPD,
	snmp_trap_source_cfg_cmd ,
	"snmp trap source interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP | loopback <0-128>}",
	"Snmp cmd\n"
	"Snmp trap\n"
	"Source interface\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	CLI_INTERFACE_LOOPBACK_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR);

DEFSH(VTYSH_SNMPD,
	snmp_trap_source_ipv4_cfg_cmd ,
	"snmp trap source ip A.B.C.D",
	"Snmp cmd\n"
	"Snmp trap\n"
	"Source\n"
	"Source ip address\n"
	"Source ip address A.B.C.D\n");


DEFSH(VTYSH_SNMPD,
	no_snmp_trap_source_cfg_cmd ,
	"no snmp trap source",
	"Delete config\n"
	"Snmp cmd\n"
	"Snmp trap\n"
	"Source port\n");


DEFSH(VTYSH_SNMPD,
	snmp_debug_monitor_cmd,
	"debug snmp (enable|disable) (get|set|trap|packet|cache|u0|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Type name of get messege\n"
	"Type name of set messege\n"
	"Type name of trap messege\n"
	"Type name of packet log queue \n"
	"Type name of cache log queue \n"
	"Type name of u0 log queue \n"
	"Type name of all debug\n");

DEFSH(VTYSH_SNMPD,
	show_snmp_debug_monitor_cmd,
	"show snmp debug",
	SHOW_STR
	"Syslog"
	"Debug status\n");




void vtysh_init_snmpd_cmd(void)
{
	install_node(&snmp_node, NULL);
	vtysh_install_default(SNMPD_NODE);

	install_element_level (CONFIG_NODE, &snmp_mode_enable_cmd, VISIT_LEVE, CMD_LOCAL);
	install_element_level (SNMPD_NODE, &vtysh_exit_snmp_cmd, VISIT_LEVE, CMD_LOCAL);
	install_element_level (SNMPD_NODE, &vtysh_quit_snmp_cmd, VISIT_LEVE, CMD_LOCAL);

   	//install_element_level (CONFIG_NODE, &snmp_conf_debug_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	
	install_element_level (SNMPD_NODE, &snmp_version_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &no_snmp_version_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &snmp_v1v2c_server_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &no_snmp_v1v2c_server_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &snmp_v3_server_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &no_snmp_v3_server_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &snmp_community_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &no_snmp_community_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &snmp_user_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &snmp_user_config2_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &no_snmp_user_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &snmp_user_privilege_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &no_snmp_user_privilege_config_cmd, CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level (SNMPD_NODE, &show_snmp_config_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (SNMPD_NODE, &show_snmp_user_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (SNMPD_NODE, &show_snmp_statistics_cmd, MONITOR_LEVE_2, CMD_LOCAL);

    /*------------------added by lidingcheng for rfc1213 system group 2018-04-13--------------------------*/     
    install_element_level (CONFIG_NODE, &snmp_sysinfo_contact_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &snmp_sysinfo_name_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &snmp_sysinfo_location_cmd, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level (CONFIG_NODE, &undo_snmp_sysinfo_contact_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &undo_snmp_sysinfo_sysname_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &undo_snmp_sysinfo_location_cmd, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level (CONFIG_NODE, &snmp_trap_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &undo_snmp_trap_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &snmp_linkup_down_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &undo_snmp_linkup_down_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level (CONFIG_NODE, &snmp_sysinfo_show_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &description_snmp_config_cmd, MONITOR_LEVE_2, CMD_LOCAL);

    install_element_level (CONFIG_NODE, &h3c_community_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &undo_h3c_community_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &h3c_version_config_cmd, CONFIG_LEVE_5, CMD_SYNC);  
    install_element_level (CONFIG_NODE, &h3c_dying_gasp_cmd, CONFIG_LEVE_5, CMD_SYNC);  
    install_element_level (CONFIG_NODE, &h3c_h3c_mib_view_config_cmd, CONFIG_LEVE_5, CMD_SYNC);  
    /*-----------------------------------lidingcheng added end  2018-04-13----------------------------*/     
    //install_element_level (CONFIG_NODE, &snmpd_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_SYNC); 
    //install_element_level (CONFIG_NODE, &snmpd_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

    install_element_level (SNMPD_NODE, &snmp_vpn_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SNMPD_NODE, &no_snmp_vpn_config_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &snmp_trap_source_cfg_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &snmp_trap_source_ipv4_cfg_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SNMPD_NODE, &no_snmp_trap_source_cfg_cmd, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (CONFIG_NODE, &snmp_debug_monitor_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &show_snmp_debug_monitor_cmd, MANAGE_LEVE, CMD_SYNC);
}

