/**
 * \page cmds_ref_isis ISIS
 * - \subpage modify_log_isis
 * 
 */
 
/**
 * \page modify_log_isis Modify Log
 * \section isis-v007r004 HiOS-V007R004
 *  -# 
 * \section isis-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"

#define ISIS_REDIST_STR_ISISD \
  "(static|connect|ibgp|ebgp)"

#define ISISV6_REDIST_STR_ISISD \
  "(static|connect|ibgp6|ebgp6)"

#define ISIS_REDIST_HELP_STR_ISISD \
  "static routes \n" \
  "connected routes (directly attached subnet or host)\n" \
  "ibgp route\n" \
  "ebgp route\n" 

#define ISIS_REDIST_STR_ISISD_WITH_INSTANCE \
  "(ospf|isis|rip)"
  
#define ISISV6_REDIST_STR_ISISD_WITH_INSTANCE \
	  "(ospfv3|isisv6|ripng)"

#define ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE \
  "ospf route\n" \
  "isis route\n" \
  "rip route\n" \


static struct cmd_node isis_node =
{
  ISIS_NODE,
  "%s(config-isis)# ",
};

static struct cmd_node isis_family_v4_node =
{
  ISIS_FAMILY_V4_NODE,
  "%s(config-isis-ipv4)# ",
};

static struct cmd_node isis_family_v6_node =
{
  ISIS_FAMILY_V6_NODE,
  "%s(config-isis-ipv6)# ",
};


DEFUNSH (VTYSH_ISISD,
	   isis_instance,
       isis_instance_cmd,
       "isis instance <1-255> {vpn-instance <1-127>}",
       ROUTER_STR
       "ISO IS-IS\n"
       "ISO Routing area tag\n"
       "Virtual private network\n"
       "Vpn number\n")
{
  vty->node = ISIS_NODE;
  return CMD_SUCCESS;
}

DEFUNSH (VTYSH_ISISD,
	   isis_instance_h3c,
       isis_instance_h3c_cmd,
       "isis <1-255> {vpn-instance <1-127>}",
	   "Intermediate System to Intermediate System (ISIS)\n"
	   "Process ID\n"
	   "VPN Routing/Forwarding instance\n"
	   "VPN instance name\n")

{
  vty->node = ISIS_NODE;
  return CMD_SUCCESS;
}


DEFUNSH (VTYSH_ISISD,
		isis_address_family_ipv4,
        isis_address_family_ipv4_cmd,
        "address-family ipv4",
        "Specify an address family\n"
        "Specify the IPv4 address family\n")
{
  	vty->node = ISIS_FAMILY_V4_NODE;
	return CMD_SUCCESS;
}

DEFUNSH(VTYSH_ISISD,
	   isis_address_family_ipv6,
	   isis_address_family_ipv6_cmd,
	   "address-family ipv6",
	   "Specify an address family\n"
	   "Specify the IPv6 address family\n")
{
	vty->node = ISIS_FAMILY_V6_NODE;
	return CMD_SUCCESS;
}
	
DEFUNSH (VTYSH_ISISD,
	 vtysh_exit_isis,
	 vtysh_exit_isis_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

ALIAS (vtysh_exit_isis,
       vtysh_quit_isis_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")
       
DEFSH (VTYSH_ISISD, no_isis_instance_cmd_vtysh,
       "no isis instance <1-255>",
       "no\n" ROUTER_STR "ISO IS-IS\n" "ISO Routing area tag")

DEFSH (VTYSH_ISISD,level_cmd_vtysh,
       "level (level-1|level-1-2|level-2)",
       "IS Level for this routing process (OSI only)\n"
       "Act as a station router only\n"
       "Act as both a station router and an area router\n"
       "Act as an area router only\n")

DEFSH(VTYSH_ISISD,no_level_cmd_vtysh,
       "no level",
       NO_STR
       "IS Level for this routing process (OSI only)\n")

DEFSH (VTYSH_ISISD,isis_enable_instance_cmd_vtysh,
	   "isis enable instance <1-255>",
	   "Interface Internet Protocol config commands\n"
	   "IP router interface commands\n"
	   "IS-IS Routing for IP\n"
	   "Routing process tag\n")

DEFSH (VTYSH_ISISD,
       no_isis_enable_instance_cmd_vtysh,
	   "no isis enable instance <1-255>",
	   NO_STR
	   "Interface Internet Protocol config commands\n"
	   "IP router interface commands\n"
	   "IS-IS Routing for IP\n"
	   "Routing process tag\n")

DEFSH (VTYSH_ISISD,net_entity_cmd_vtysh,
       "net entity WORD",
       "ISIS Network Entity Title\n"
       "A Network Entity Title for this process (OSI only)\n"
       "XX.XXXX. ... .XXX.XX  Network entity title (NET)\n")

DEFSH (VTYSH_ISISD,no_net_entity_cmd_vtysh,
       "no net entity WORD",
       NO_STR
       "ISIS Network Entity Title\n"
       "A Network Entity Title for this process (OSI only)\n"
       "XX.XXXX. ... .XXX.XX  Network entity title (NET)\n")

DEFSH (VTYSH_ISISD,metric_style_cmd_vtysh,
       "metric-style (narrow|transition|wide)",
       "Use old-style (ISO 10589) or new-style packet formats\n"
       "Use old style of TLVs with narrow metric\n"
       "Send and accept both styles of TLVs during transition\n"
       "Use new style of TLVs to carry wider metric\n")

DEFSH (VTYSH_ISISD,no_metric_style_cmd_vtysh,
	   "no metric-style",
	   NO_STR
	   "Use old-style (ISO 10589) or new-style packet formats\n")

DEFSH (VTYSH_ISISD,lsp_gen_interval_cmd_vtysh,
       "lsp gen-interval <1-3600>",
       "IS-IS link state database\n"
       "Minimum interval between regenerating same LSP\n"
       "Minimum interval in seconds\n")

DEFSH (VTYSH_ISISD,no_lsp_gen_interval_cmd_vtysh,
	   "no lsp gen-interval",
	   NO_STR
	   "IS-IS link state database\n"
	   "Minimum interval between regenerating same LSP\n")

DEFSH (VTYSH_ISISD,lsp_refresh_interval_cmd_vtysh,
       "lsp refresh-interval <1-3600>",
       "IS-IS link state database\n"
       "LSP refresh interval\n"
       "LSP refresh interval in seconds\n")

DEFSH (VTYSH_ISISD,no_lsp_refresh_interval_cmd_vtysh,
	   "no lsp refresh-interval",
	   NO_STR
	   "IS-IS link state database\n"
	   "LSP refresh interval in seconds\n")

DEFSH (VTYSH_ISISD,lsp_lifetime_cmd_vtysh,
       "lsp lifetime <360-3600>",
       "IS-IS link state database\n"
       "Maximum LSP lifetime\n"
       "LSP lifetime in seconds\n")

DEFSH (VTYSH_ISISD,no_lsp_lifetime_cmd_vtysh,
	   "no lsp lifetime",
	   NO_STR
	   "IS-IS link state database\n"
	   "LSP lifetime in seconds\n")

DEFSH (VTYSH_ISISD,lsp_length_cmd_vtysh,
       "lsp length <128-1497>",
       "IS-IS link state database\n"
       "Configure the maximum size of generated LSPs\n"
       "Maximum size of generated LSPs\n")

DEFSH(VTYSH_ISISD,no_lsp_length_cmd_vtysh,
      "no lsp length",
      NO_STR
      "IS-IS link state database\n"
      "Configure the maximum size of generated LSPs\n")

DEFSH(VTYSH_ISISD,spf_interval_cmd_vtysh,
       "spf interval <1-3600>",
       "Minimum interval between SPF calculations\n"
       "Minimum interval between consecutive SPFs in seconds\n")

DEFSH (VTYSH_ISISD,no_spf_interval_cmd_vtysh,
       "no spf interval",
       NO_STR
       "Minimum interval between SPF calculations\n")

DEFSH(VTYSH_ISISD,overload_bit_enable_cmd_vtysh,
	  "overload bit enable",
	  "Set overload bit to avoid any transit traffic\n"
	  "Set overload bit\n")
		   
DEFSH (VTYSH_ISISD,no_overload_bit_enable_cmd_vtysh,
       "no overload bit enable",
       "Reset overload bit to accept transit traffic\n"
       "Reset overload bit\n")

DEFSH(VTYSH_ISISD,attached_bit_enable_cmd_vtysh,
       "attached bit enable",
       "Set attached bit to identify as L1/L2 router for inter-area traffic\n"
       "Set attached bit\n")

DEFSH (VTYSH_ISISD,no_attached_bit_enable_cmd_vtysh,
       "no attached bit enable",
       "Reset attached bit\n")

DEFSH (VTYSH_ISISD,isis_passive_cmd_vtysh,
	   "isis passive",
	   "IS-IS commands\n"
	   "Configure the passive mode for interface\n")

DEFSH (VTYSH_ISISD,no_isis_passive_cmd_vtysh,
       "no isis passive",
       NO_STR
       "IS-IS commands\n"
       "Configure the passive mode for interface\n")

DEFSH (VTYSH_ISISD,isis_circuit_type_cmd_vtysh,
       "isis circuit-type (level-1|level-1-2|level-2)",
       "IS-IS commands\n"
       "Configure circuit type for interface\n"
       "Level-1 only adjacencies are formed\n"
       "Level-1-2 adjacencies are formed\n"
       "Level-2 only adjacencies are formed\n")

DEFSH (VTYSH_ISISD,no_isis_circuit_type_cmd_vtysh,
       "no isis circuit-type",
       NO_STR
       "IS-IS commands\n"
       "Configure circuit type for interface\n")

DEFSH (VTYSH_ISISD,isis_priority_cmd_vtysh,
	   "isis priority <0-127>",
	   "IS-IS commands\n"
	   "Set priority for Designated Router election\n"
	   "Priority value\n")

DEFSH (VTYSH_ISISD,no_isis_priority_cmd_vtysh,
       "no isis priority",
       NO_STR
       "IS-IS commands\n"
       "Set priority for Designated Router election\n")

DEFSH(VTYSH_ISISD,no_isis_priority_arg_cmd_vtysh,
       "no isis priority <0-127>",
       NO_STR
       "IS-IS commands\n"
       "Set priority for Designated Router election\n"
       "Priority value\n")

DEFSH (VTYSH_ISISD,isis_priority_l1_cmd_vtysh,
	   "isis priority <0-127> level-1",
	   "IS-IS commands\n"
	   "Set priority for Designated Router election\n"
	   "Priority value\n"
	   "Specify priority for level-1 routing\n")

DEFSH (VTYSH_ISISD, no_isis_priority_l1_cmd_vtysh,
	   "no isis priority level-1",
	   NO_STR
	   "IS-IS commands\n"
	   "Set priority for Designated Router election\n"
	   "Specify priority for level-1 routing\n")

DEFSH (VTYSH_ISISD, isis_priority_l2_cmd_vtysh,
       "isis priority <0-127> level-2",
       "IS-IS commands\n"
       "Set priority for Designated Router election\n"
       "Priority value\n"
       "Specify priority for level-2 routing\n")

DEFSH (VTYSH_ISISD,no_isis_priority_l2_cmd_vtysh,
       "no isis priority level-2",
       NO_STR
       "IS-IS commands\n"
       "Set priority for Designated Router election\n"
       "Specify priority for level-2 routing\n")

DEFSH (VTYSH_ISISD,isis_metric_cmd_vtysh,
	   "isis metric <1-16777215>",
	   "IS-IS commands\n"
	   "Set default metric for circuit\n"
	   "Default metric value ,narrow <1-63>\n")

DEFSH (VTYSH_ISISD,no_isis_metric_cmd_vtysh,
	   "no isis metric",
	   NO_STR
	   "IS-IS commands\n"
	   "Set default metric for circuit\n")

DEFSH (VTYSH_ISISD,isis_hello_interval_cmd_vtysh,
       "isis hello-interval <1-3600>",
       "IS-IS commands\n"
       "Set Hello interval\n"
       "Hello interval value\n"
       "Holdtime 1 seconds, interval depends on multiplier\n")

DEFSH (VTYSH_ISISD,no_isis_hello_interval_cmd_vtysh,
       "no isis hello-interval",
       NO_STR
       "IS-IS commands\n"
       "Set Hello interval\n")

DEFSH (VTYSH_ISISD,isis_hello_multiplier_cmd_vtysh,
       "isis hello-multiplier <3-100>",
       "IS-IS commands\n"
       "Set multiplier for Hello holding time\n"
       "Hello multiplier value\n")

DEFSH (VTYSH_ISISD,no_isis_hello_multiplier_cmd_vtysh,
	   "no isis hello-multiplier",
	   NO_STR
	   "IS-IS commands\n"
	   "Set multiplier for Hello holding time\n")
	   
#if 0
DEFSH(VTYSH_ISISD,no_isis_hello_multiplier_arg_cmd_vtysh,
       "no isis hello-multiplier <3-100>",
       NO_STR
       "IS-IS commands\n"
       "Set multiplier for Hello holding time\n"
       "Hello multiplier value\n")
#endif

DEFSH (VTYSH_ISISD,csnp_interval_cmd_vtysh,
       "isis csnp-interval <1-3600>",
       "IS-IS commands\n"
       "Set CSNP interval in seconds\n"
       "CSNP interval value\n")

DEFSH (VTYSH_ISISD,no_csnp_interval_cmd_vtysh,
       "no isis csnp-interval",
       NO_STR
       "IS-IS commands\n"
       "Set CSNP interval in seconds\n")

DEFSH (VTYSH_ISISD,psnp_interval_cmd_vtysh,
       "isis psnp-interval <1-3600>",
       "IS-IS commands\n"
       "Set PSNP interval in seconds\n"
       "PSNP interval value\n")

DEFSH (VTYSH_ISISD,no_psnp_interval_cmd_vtysh,
       "no isis psnp-interval",
       NO_STR
       "IS-IS commands\n"
       "Set PSNP interval in seconds\n")

DEFSH (VTYSH_ISISD,isis_network_cmd_vtysh,
	   "isis network p2p",
	   "IS-IS commands\n"
	   "Set network type\n"
	   "point-to-point network type\n")

DEFSH (VTYSH_ISISD,no_isis_network_cmd_vtysh,
       "no isis network p2p",
       NO_STR
       "IS-IS commands\n"
       "Set network type for circuit\n"
       "point-to-point network type\n")

DEFSH (VTYSH_ISISD,show_isis_neighbor_cmd_vtysh,
	   "show isis neighbor",
	   SHOW_STR
	   "ISIS network information\n"
	   "ISIS neighbor adjacencies\n")
	
DEFSH (VTYSH_ISISD,show_isis_neighbor_detail_cmd_vtysh,
	   "show isis neighbor detail",
	   SHOW_STR
	   "ISIS network information\n"
	   "ISIS neighbor adjacencies\n"
	   "show detailed information\n")

DEFSH (VTYSH_ISISD,clear_isis_neighbor_cmd_vtysh,
	   "clear isis neighbor",
	   CLEAR_STR
	   "Reset ISIS network information\n"
	   "Reset ISIS neighbor adjacencies\n")

DEFSH (VTYSH_ISISD,clear_isis_neighbor_arg_cmd_vtysh,
	   "clear isis neighbor WORD",
	   CLEAR_STR
	   "ISIS network information\n"
	   "ISIS neighbor adjacencies\n"
	   "System id\n")

DEFSH (VTYSH_ISISD,show_lsp_arg_cmd_vtysh,
       "show isis lsp WORD",
       SHOW_STR
       "IS-IS information\n"
       "IS-IS link state database\n"
       "LSPID in the form of ####.####.####.##-##\n"
       "Detailed information\n")

DEFSH (VTYSH_ISISD,show_database_cmd_vtysh,
	   "show isis database",
	   SHOW_STR
	   "IS-IS information\n"
	   "IS-IS link state database\n")

DEFSH (VTYSH_ISISD,show_database_detail_cmd_vtysh,
       "show isis database detail",
       SHOW_STR
       "IS-IS information\n"
       "IS-IS link state database\n"
       "IS-IS information detail\n")

DEFSH (VTYSH_ISISD,isis_passwd_md5_cmd_vtysh,
       "isis password WORD md5",
       "IS-IS commands\n"
       "Configure the authentication password for a circuit\n"
       "Circuit password clear:<1-127>/md5:<1-254>\n"
       "Authentication type\n")

DEFSH (VTYSH_ISISD,isis_passwd_clear_cmd_vtysh,
       "isis password WORD",
       "IS-IS commands\n"
       "Configure the authentication password for a circuit\n"
       "Circuit password clear:<1-127>/md5:<1-254>\n")

DEFSH (VTYSH_ISISD,no_isis_passwd_cmd_vtysh,
       "no isis password",
       NO_STR
       "IS-IS commands\n"
       "Configure the authentication password for a circuit\n")

DEFSH (VTYSH_ISISD,show_isis_debug_cmd_vtysh,
       "show isis debug",
       SHOW_STR
       "ISIS Commands\n"
       "State of each debugging option\n")

DEFSH (VTYSH_ISISD,show_isis_topology_cmd_vtysh,
       "show isis topology",
       SHOW_STR
       "IS-IS information\n"
       "IS-IS paths to Intermediate Systems\n")

DEFSH (VTYSH_ISISD,show_isis_topology_l1_cmd_vtysh,
       "show isis topology level-1",
       SHOW_STR
       "IS-IS information\n"
       "IS-IS paths to Intermediate Systems\n"
       "Paths to all level-1 routers in the area\n")

DEFSH (VTYSH_ISISD,show_isis_topology_l2_cmd_vtysh,
       "show isis topology level-2",
       SHOW_STR
       "IS-IS information\n"
       "IS-IS paths to Intermediate Systems\n"
       "Paths to all level-2 routers in the domain\n")

DEFSH (VTYSH_ISISD,show_hostname_cmd_vtysh,
       "show isis hostname",
       SHOW_STR
       "IS-IS information\n"
       "IS-IS Dynamic hostname mapping\n")

// DEFSH (VTYSH_ISISD,debug_isis_packet_dump_cmd_vtysh,
//        "debug isis packets",
//        DEBUG_STR
//        "IS-IS information\n"
//        "IS-IS packet dump\n")

// DEFSH (VTYSH_ISISD,no_debug_isis_packet_dump_cmd_vtysh,
//        "no debug isis packets",
//        UNDEBUG_STR
//        "IS-IS information\n"
//        "IS-IS packet dump\n")

// DEFSH (VTYSH_ISISD,debug_isis_spf_cmd_vtysh,
//        "debug isis spf",
//        DEBUG_STR
//        "IS-IS information\n"
//        "IS-IS Spf debug\n")

// DEFSH (VTYSH_ISISD,no_debug_isis_spf_cmd_vtysh,
//        "no debug isis spf",
//        UNDEBUG_STR
//        "IS-IS information\n"
//        "IS-IS Spf undebug\n")

// DEFSH (VTYSH_ISISD, debug_isis_lsp_cmd_vtysh,
//        "debug isis lsp",
//        DEBUG_STR
//        "IS-IS information\n"
//        "IS-IS LSPs DEBUG\n")

// DEFSH (VTYSH_ISISD, no_debug_isis_lsp_cmd_vtysh,
//        "no debug isis lsp",
//        UNDEBUG_STR
//        "IS-IS information\n"
//        "IS-IS LSPs UNDEBUG\n")

// DEFSH (VTYSH_ISISD,debug_isis_events_cmd_vtysh,
//        "debug isis fsm",
//        DEBUG_STR
//        "IS-IS information\n"
//        "IS-IS Events\n")
       
// DEFSH (VTYSH_ISISD,no_debug_isis_events_cmd_vtysh,
//        "no debug isis fsm",
//        UNDEBUG_STR
//        "IS-IS information\n"
//        "IS-IS Events\n")

DEFSH(VTYSH_ISISD,show_isis_route_cmd_vtysh,
      "show isis route",
      SHOW_STR
      "ISIS information\n"
      "ISIS routing table\n")

DEFSH(VTYSH_ISISD, show_isis_instance_route_cmd_vtysh,
	  "show isis instance <1-255> route",
	  SHOW_STR
	  "ISIS information\n"
	  "ISIS instance\n"
	  "Instance number\n"
	  "ISIS routing table\n")

DEFSH (VTYSH_ISISD,show_isis_statistics_cmd_vtysh,
       "show isis statistics",
       SHOW_STR
       "ISIS information\n"
       "isis packet,lsp,route statistic\n")

DEFSH (VTYSH_ISISD,hello_padding_enable_cmd_vtysh,
       "hello padding enable",
       "IS-IS commands\n"
       "Add padding to IS-IS hello packets\n"
       "Pad hello packets\n"
       "<cr>\n")
#if 0
DEFSH (VTYSH_ISISD,hello_padding_enable_cmd_vtysh,
       "hello padding enable",
       "IS-IS commands\n"
       "Add padding to IS-IS hello packets\n"
       "Pad hello packets\n"
       "<cr>\n")
#endif
DEFSH (VTYSH_ISISD,no_hello_padding_enable_cmd_vtysh,
       "no hello padding enable",
       NO_STR
       "IS-IS commands\n"
       "Add padding to IS-IS hello packets\n"
       "Pad hello packets\n"
       "<cr>\n")
       

DEFSH (VTYSH_ISISD,
       show_isis_interface_cmd_vtysh,
       "show isis interface",
       SHOW_STR
       "ISIS network information\n"
       "ISIS interface\n")

DEFSH (VTYSH_ISISD,
		show_isis_interface_arg_cmd_vtysh,
		"show isis interface (ethernet|gigabitethernet|xgigabitethernet) USP",
		SHOW_STR
		"ISIS network information\n"
		"ISIS interface\n"
		"Ethernet interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
        "GigabitEthernet interface type\n"
	    "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
	    "10GigabitEthernet interface type\n"
	    "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")

DEFSH (VTYSH_ISISD,
	   show_isis_loopback_interface_cmd_vtysh,
	   "show isis interface loopback <0-128>",
	   SHOW_STR
	   "ISIS network information\n"
	   "ISIS interface\n"
	   "LoopBack interface\n"
	   "LoopBack interface number\n")

DEFSH (VTYSH_ISISD,
	   show_isis_trunk_interface_cmd_vtysh,
	   "show isis interface trunk TRUNK",
	   SHOW_STR
	   "ISIS network information\n"
	   "ISIS interface\n"
	   "Trunk interface\n"
	   "The port/subport of trunk, format: <1-128>[.<1-4095>]\n")

DEFSH (VTYSH_ISISD,
	   show_isis_vlanif_interface_cmd_vtysh,
	   "show isis interface vlanif <1-4094>",
	   SHOW_STR
	   "ISIS network information\n"
	   "ISIS interface\n"
	   "Vlan interface\n"
	   "VLAN interface number\n")

DEFSH (VTYSH_ISISD,distance_cmd_vtysh,
	   "distance <1-255>",
	   "distance value"
	   "ISO IS-IS\n"
	   "ISO Routing area distance")

DEFSH (VTYSH_ISISD,no_distance_cmd_vtysh,
       "no distance",
       "distance value default"
       "ISO IS-IS\n"
       "ISO Routing area distance")
#if 0
DEFSH(VTYSH_ISISD,isis_redistribute_cmd_vtysh,
      "route redistribute" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) {metric <0-16777215>}",
	  "route information\n"
      REDIST_STR
      ISIS_REDIST_HELP_STR_ISISD
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "Metric for redistributed routes\n"
      "ISIS default metric,narrow <0-63>\n")

DEFSH(VTYSH_ISISD,no_isis_redistribute_cmd_vtysh,
      "no route redistribute " ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2)",
      NO_STR
      "route information\n"
      REDIST_STR
      ISIS_REDIST_HELP_STR_ISISD
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n")
      
DEFSH(VTYSH_ISISD,isis_redistribute_instance_cmd_vtysh,
      "route redistribute" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) {metric <0-16777215>}",
	  "route information\n"
      REDIST_STR
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "Metric for redistributed routes\n"
      "ISIS default metric,narrow <0-63>\n")

DEFSH(VTYSH_ISISD,no_isis_redistribute_instance_cmd_vtysh,
      "no route redistribute " ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2)",
      NO_STR
      "route information\n"
      REDIST_STR
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n")
#endif

DEFSH(VTYSH_ISISD,isis_redistribute_cmd_vtysh,
      "route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) {metric <0-4261412864>}",
	  "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "Metric for redistributed routes\n"
      "ISIS default metric,narrow <0-63>\n")

DEFSH(VTYSH_ISISD, no_isis_redistribute_cmd_vtysh,
	  "no route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD
	  " (level-1|level-2|level-1-2)",
	  NO_STR
	  "route information\n"
	  REDIST_STR
	  "Route type :IPV4\n"
      "Route type : IPV6\n"
	  ISIS_REDIST_HELP_STR_ISISD
	  "Redistribute into level-1\n"
	  "Redistribute into level-2\n"
	  "Redistribute into level-1-2\n")

DEFSH(VTYSH_ISISD, isis_redistribute_instance_cmd_vtysh,
      "route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) {metric <0-4261412864>}",
	  "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "Metric for redistributed routes\n"
      "ISIS default metric,narrow <0-63>\n")

DEFSH(VTYSH_ISISD, no_isis_redistribute_instance_cmd_vtysh,
      "no route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2)",
      NO_STR
      "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n")

DEFSH (VTYSH_ISISD,router_summary_cmd_vtysh,
	  "route summary A.B.C.D/M (level-1|level-2|level-1-2) ",
	  "route information\n"
	  "Summary Information\n"
	  "IP Infomation\n"
	  "Summary level-1\n"
	  "Summary level-2\n"
	  "Summary level-1-2\n")
	  
DEFSH (VTYSH_ISISD,
	   router_summary_ipv6_cmd_vtysh,
	   "ipv6 summary X:X::X:X/M (level-1|level-2|level-1-2) ",
	   "Configure IPv6 commands for ISIS\n"
	   "Configure summary address for IPv6\n"
	   "IPv6 address\n"
	   "Summary level-1\n"
	   "Summary level-2\n"
	   "Summary level-1-2\n")

DEFSH (VTYSH_ISISD,no_router_summary_cmd_vtysh,
	  "no route summary A.B.C.D/M (level-1|level-2|level-1-2)",
	  NO_STR
	  "route information\n"
	  "Summary Information\n"
	  "IP Infomation\n"
	  "Summary level-1\n"
	  "Summary level-2\n"
	  "Summary level-1-2\n")

DEFSH (VTYSH_ISISD,
       router_import_into_level2_cmd_vtysh,
       "route-import level-1 into level-2",
       "import routes from other area\n"
       "ISIS level-1 (source)\n"
       "import into\n"
       "ISIS level-2 (destination)\n")

DEFSH (VTYSH_ISISD,
       router_import_into_level1_cmd_vtysh,
       "route-import level-2 into level-1",
       "import routes from other area\n"
       "ISIS level-2 (source)\n"
       "import into\n"
       "ISIS level-1 (destination)\n")

DEFSH (VTYSH_ISISD,
       no_router_import_into_level2_cmd_vtysh,
       "no route-import level-1 into level-2",
	   NO_STR
       "import routes from other area\n"
       "ISIS level-1 (source)\n"
       "import into\n"
       "ISIS level-2 (destination)\n")

DEFSH (VTYSH_ISISD,
       no_router_import_into_level1_cmd_vtysh,
       "no route-import level-2 into level-1",
	   NO_STR
       "import routes from other area\n"
       "ISIS level-2 (source)\n"
       "import into\n"
       "ISIS level-1 (destination)\n")


DEFSH(VTYSH_ISISD, show_isis_redist_route_cmd_vtysh,
	  "show isis instance <1-255> redist route",
	  SHOW_STR
	  "ISIS information\n"
	  "ISIS instance\n"
	  "ISO Routing area tag\n"
	  "ISIS redist information\n"
	  "ISIS routing table\n")

DEFSH (VTYSH_ISISD, show_database_instance_brief_cmd_vtysh,
       "show isis instance <1-255> database",
       SHOW_STR
       "IS-IS information\n"
       "Istance information\n"
       "Instance number\n"
       "IS-IS link state database\n")

DEFSH (VTYSH_ISISD, show_database_instance_detail_cmd_vtysh,
       "show isis instance <1-255> database detail",
       SHOW_STR
       "IS-IS information\n"
       "Istance information\n"
       "Instance number\n"
       "IS-IS link state database\n")

DEFSH (VTYSH_ISISD,show_isis_instance_neighbor_cmd_vtysh,
	   "show isis instance <1-255> neighbor",
	   SHOW_STR
	   "ISIS network information\n"
	   "ISIS instance information\n"
	   "Instance number\n"
	   "ISIS neighbor adjacencies\n")
	
DEFSH (VTYSH_ISISD, show_isis_instance_neighbor_detail_cmd_vtysh,
	   "show isis instance <1-255> neighbor detail",
	   SHOW_STR
	   "ISIS network information\n"
	   "ISIS instance information\n"
	   "Instance number\n"
	   "ISIS neighbor adjacencies\n"
	   "show detailed information\n")

DEFSH (VTYSH_ISISD, show_isis_instance_interface_cmd_vtysh,
	   "show isis instance <1-255> interface",
	   SHOW_STR
	   "ISIS network information\n"
	   "Instance number\n"
	   "Instance number\n"
	   "ISIS interface\n")

DEFSH (VTYSH_ISISD,isis_enable_ipv6_instance_cmd_vtysh,
	   "isis enable ipv6 instance <1-255>",
	   "Interface Internet Protocol config commands\n"
	   "IP router interface commands\n"
	   "IPV6 type\n"
	   "IS-IS Routing for IP\n"
	   "Routing process tag\n")

DEFSH (VTYSH_ISISD,no_isis_enable_ipv6_instance_cmd_vtysh,
	   "no isis enable ipv6 instance <1-255>",
	   NO_STR
	   "IPv6 interface subcommands\n"
	   "IPv6 Router interface commands\n"
	   "IS-IS Routing for IPv6\n"
	   "Routing process tag\n")

DEFSH(VTYSH_ISISD, show_isis_route_ipv6_cmd_vtysh,
	  "show isis ipv6 route",
	  SHOW_STR
	  "ISIS information\n"
	  "ISIS routing table\n")

/* debug ��־���͵� syslog ʹ��״̬���� */
DEFSH (VTYSH_ISISD,isisd_log_level_ctl_cmd_vtysh,	"debug isisd(enable | disable)",		
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n")	

/* debug ��־���͵� syslog ʹ��״̬��ʾ */
DEFSH (VTYSH_ISISD,isisd_show_log_level_ctl_cmd_vtysh,	"show debug isisd",		
	SHOW_STR"Output log of debug level\n""Program name\n")	

DEFSH (VTYSH_ISISD,area_passwd_md5_cmd_vtysh,
    "area-password md5 WORD",
    "Configure the authentication password for an area\n"
    "Authentication type\n"
    "Area password length 1-254\n")

DEFSH (VTYSH_ISISD,area_passwd_md5_snpauth_cmd_vtysh,
   "area-password md5 WORD authenticate snp (send-only|validate)",
   "Configure the authentication password for an area\n"
   "Authentication type\n"
   "Area password length 1-254\n"
   "Authentication\n"
   "SNP PDUs\n"
   "Send but do not check PDUs on receiving\n"
   "Send and check PDUs on receiving\n")


DEFSH (VTYSH_ISISD,area_passwd_clear_cmd_vtysh,
    "area-password clear WORD",
    "Configure the authentication password for an area\n"
    "Authentication type\n"
    "Area password length 1-254\n")

DEFSH (VTYSH_ISISD,area_passwd_clear_snpauth_cmd_vtysh,
    "area-password clear WORD authenticate snp (send-only|validate)",
    "Configure the authentication password for an area\n"
    "Authentication type\n"
    "Area password length 1-254\n"
    "Authentication\n"
    "SNP PDUs\n"
    "Send but do not check PDUs on receiving\n"
    "Send and check PDUs on receiving\n")

DEFSH (VTYSH_ISISD,no_area_passwd_cmd_vtysh,
    "no area-password",
    NO_STR
    "Configure the authentication password for an area\n")

DEFSH (VTYSH_ISISD,domain_passwd_md5_cmd_vtysh,
    "domain-password md5 WORD",
    "Set the authentication password for a routing domain\n"
    "Authentication type\n"
    "Routing domain password length 1-254\n")

DEFSH (VTYSH_ISISD,domain_passwd_md5_snpauth_cmd_vtysh,
    "domain-password md5 WORD authenticate snp (send-only|validate)",
    "Set the authentication password for a routing domain\n"
    "Authentication type\n"
    "Routing domain password length 1-254\n"
    "Authentication\n"
    "SNP PDUs\n"
    "Send but do not check PDUs on receiving\n"
    "Send and check PDUs on receiving\n")

DEFSH (VTYSH_ISISD,domain_passwd_clear_cmd_vtysh,
    "domain-password clear WORD",
    "Set the authentication password for a routing domain\n"
    "Authentication type\n"
    "Routing domain password length 1-254\n")

DEFSH (VTYSH_ISISD,domain_passwd_clear_snpauth_cmd_vtysh,
    "domain-password clear WORD authenticate snp (send-only|validate)",
    "Set the authentication password for a routing domain\n"
    "Authentication type\n"
    "Routing domain password\n"
    "Authentication\n"
    "SNP PDUs\n"
    "Send but do not check PDUs on receiving\n"
    "Send and check PDUs on receiving\n")

DEFSH (VTYSH_ISISD,no_domain_passwd_cmd_vtysh,
    "no domain-password",
    NO_STR
    "Set the authentication password for a routing domain\n")

DEFSH(VTYSH_ISISD,
      isis_redistribute_arg_cmd_vtysh,
      "route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) cost-style (internal|external)",
	  "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n")

DEFSH(VTYSH_ISISD,
      isis_redistribute_two_arg_cmd_vtysh,
      "route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) cost-style (internal|external) metric <0-4261412864>",
	  "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n"
      "Metric for redistributed routes\n"
      "ISIS default metric,narrow <0-63>\n")


DEFSH(VTYSH_ISISD,
      isis_redistribute_instance_arg_cmd_vtysh,
      "route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) cost-type (internal|external)",
	  "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n")

DEFSH(VTYSH_ISISD,
      isis_redistribute_instance_two_arg_cmd_vtysh,
      "route redistribute (ipv4|ipv6)" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) cost-type (internal|external) metric <0-4261412864>",
	  "route information\n"
      REDIST_STR
      "Route type :IPV4\n"
      "Route type : IPV6\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Redistribute into level-1\n"
      "Redistribute into level-2\n"
      "Redistribute into level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n"
      "Metric for redistributed routes\n"
      "ISIS default metric,narrow <0-63>\n")

DEFSH(VTYSH_ISISD,
      isis_redistribute_h3c_cmd_vtysh,
      "import-route" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) {cost <0-4261412864>}",
      "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "Set the cost value\n"
      "narrow <0-63>,wide<0-4261412864>\n")

DEFSH(VTYSH_ISISD,
      no_isis_redistribute_h3c_without_level_cmd_vtysh,
      "undo import-route" ISIS_REDIST_STR_ISISD,
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD)

DEFSH(VTYSH_ISISD,
      isis_redistribute_instance_h3c_cmd_vtysh,
      "import-route" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) {cost <0-4261412864>}",
	  "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "Process ID\n"
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "Set the cost value\n"
      "narrow <0-63>,wide<0-4261412864>\n")

DEFSH(VTYSH_ISISD,
      no_isis_redistribute_instance_h3c_cmd_vtysh,
      "undo import-route" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2)",
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n")

DEFSH(VTYSH_ISISD,
      isis_redistribute_arg_h3c_cmd_vtysh,
      "import-route" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) cost-style (internal|external)",
	  "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n")

DEFSH(VTYSH_ISISD,
      isis_redistribute_two_arg_h3c_cmd_vtysh,
      "import-route" ISIS_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) cost-style (internal|external) cost <0-4261412864>",
	  "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n"
      "Set the cost value\n"
      "narrow <0-63>,wide<0-4261412864>\n")

DEFSH(VTYSH_ISISD,
      isis_redistribute_instance_arg_h3c_cmd_vtysh,
      "import-route" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) cost-type (internal|external)",
	  "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "Process ID\n"
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n")

DEFSH(VTYSH_ISISD,
      isis_redistribute_instance_two_arg_h3c_cmd_vtysh,
      "import-route" ISIS_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) cost-type (internal|external) cost <0-4261412864>",
	  "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "Process ID\n"
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "cost-style for redistributed routes\n"
      "Set ISIS internal cost type\n"
      "Set ISIS external cost type\n"
      "Set the cost value\n"
      "narrow <0-63>,wide<0-4261412864>\n")

DEFSH (VTYSH_ISISD,
       isis_enable_instance_h3c_cmd_vtysh,
       "isis enable",
       "Configure interface parameters for ISIS\n"
       "Enable ISIS processing\n")


DEFSH (VTYSH_ISISD,
       no_isis_enable_instance_h3c_cmd_vtysh,
       "undo isis <1-255>",
       "Cancel current setting\n"
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Process ID\n")

DEFSH (VTYSH_ISISD,
       isis_circuit_type_h3c_cmd_vtysh,
       "isis circuit-level (level-1|level-1-2|level-2)",
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set ISIS level for the interface\n"
       "Level-1\n"
       "Level-1-2\n"
       "Level-2\n")

DEFSH (VTYSH_ISISD,
       no_isis_circuit_type_h3c_cmd_vtysh,
       "undo isis circuit-level",
       "Cancel current setting\n"
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set ISIS level for the interface\n")

DEFSH (VTYSH_ISISD,
       isis_passwd_md5_h3c_cmd_vtysh,
       "isis authentication-mode md5 WORD",
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set the type and password of ISIS interface authentication\n"
       "Authentication type\n"
       "Circuit password clear:<1-127>/md5:<1-254>\n")

DEFSH (VTYSH_ISISD,
       isis_passwd_clear_h3c_cmd_vtysh,
       "isis authentication-mode simple WORD",
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set the type and password of ISIS interface authentication\n"
       "Authentication type\n"
       "Circuit password clear:<1-127>/md5:<1-254>\n")

DEFSH (VTYSH_ISISD,
       no_isis_passwd_h3c_cmd_vtysh,
       "undo isis authentication-mode",
       "Cancel current setting\n"
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set the type and password of ISIS interface authentication\n")

DEFSH (VTYSH_ISISD,
       isis_priority_h3c_cmd_vtysh,
       "isis dis-priority <0-127>",
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set interface DIS election priority\n"
       "Value of priority\n")

DEFSH (VTYSH_ISISD,
       no_isis_priority_h3c_cmd_vtysh,
       "undo isis dis-priority",
       "Cancel current setting\n"
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set interface DIS election priority\n")
       
DEFSH (VTYSH_ISISD,
       isis_priority_l1_h3c_cmd_vtysh,
       "isis dis-priority <0-127> level-1",
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set interface DIS election priority\n"
       "Value of priority\n"
       "Level-1\n")


DEFSH (VTYSH_ISISD,
       no_isis_priority_l1_h3c_cmd_vtysh,
       "undo isis dis-priority level-1",
       "Cancel current setting\n"
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set interface DIS election priority\n"
       "Level-1\n")

DEFSH (VTYSH_ISISD,
       isis_priority_l2_h3c_cmd_vtysh,
       "isis dis-priority <0-127> level-2",
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set interface DIS election priority\n"
       "Value of priority\n"
       "Level-2\n")

DEFSH (VTYSH_ISISD,
       no_isis_priority_l2_h3c_cmd_vtysh,
       "undo isis dis-priority level-2",
       "Cancel current setting\n"
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set interface DIS election priority\n"
       "Level-2\n")

DEFSH(VTYSH_ISISD,
      isis_metric_h3c_cmd_vtysh,
      "isis [ipv6] cost <1-16777215>",
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Configure IPv6 commands for IS-IS\n"
      "Set the cost value\n"
      "Cost value ,narrow <1-63>\n")

DEFSH(VTYSH_ISISD,
      no_isis_metric_h3c_cmd_vtysh,
      "undo isis [ipv6] cost",
      "Cancel current setting\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Configure IPv6 commands for IS-IS\n"
      "Set the cost value\n")


DEFSH (VTYSH_ISISD,
       isis_hello_interval_h3c_cmd_vtysh,
       "isis timer hello <1-3600>",
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set ISIS timer on interface\n"
       "Set hello packet sending interval\n"
       "Hello interval value (Seconds)\n")

DEFSH (VTYSH_ISISD,
       no_isis_hello_interval_h3c_cmd_vtysh,
       "undo isis timer hello",
       NO_STR
       "IS-IS commands\n"
       "Set Hello interval\n")

DEFSH (VTYSH_ISISD,
       isis_hello_multiplier_h3c_cmd_vtysh,
       "isis timer holding-multiplier <3-100>",
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set ISIS timer on interface\n"
       "Set holding multiplier value\n"
       "Holding multiplier value\n")


DEFSH (VTYSH_ISISD,
       no_isis_hello_multiplier_h3c_cmd_vtysh,
       "undo isis timer holding-multiplier",
       "Cancel current setting\n"
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set ISIS timer on interface\n"
       "Set holding multiplier value\n")

DEFSH (VTYSH_ISISD,
       hello_padding_enable_h3c_cmd_vtysh,
       "isis padding-hello",
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Configure to send hello PDUs with padding TLV\n")

DEFSH (VTYSH_ISISD,
       no_hello_padding_enable_h3c_cmd_vtysh,
       "undo isis padding-hello",
       "Cancel current setting"
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Configure to send hello PDUs with padding TLV\n")


DEFSH (VTYSH_ISISD,
       csnp_interval_h3c_cmd_vtysh,
       "isis timer csnp <1-3600>",
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set ISIS timer on interface\n"
       "Set CSNP packet sending interval\n"
       "CSNP packet sending interval value (Seconds)\n")

DEFSH (VTYSH_ISISD,
       no_csnp_interval_h3c_cmd_vtysh,
       "undo isis timer csnp",
       "Cancel current setting\n"
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Set ISIS timer on interface\n"
       "Set CSNP interval in seconds\n")
       
DEFSH (VTYSH_ISISD,
       isis_network_h3c_cmd_vtysh,
       "isis circuit-type p2p",
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Change the network type of the circuit\n"
       "Change the network type of the circuit to P2P\n")

DEFSH (VTYSH_ISISD,
       no_isis_network_h3c_cmd_vtysh,
       "undo isis circuit-type",
       "Cancel current setting\n"
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Change the network type of the circuit\n")


DEFSH (VTYSH_ISISD,
       no_isis_instance_h3c_cmd_vtysh,
       "undo isis <1-255>",
       "Cancel current configuration\n"
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Process ID\n")

DEFSH (VTYSH_ISISD,
       ipv4_distance_h3c_cmd_vtysh,
       "preference <1-255>",
       "Set ISIS route preference\n"
       "Preference value\n")

DEFSH (VTYSH_ISISD,
       no_ipv4_distance_h3c_cmd_vtysh,
       "undo preference",
       "Cancel current configuration\n"
       "Set ISIS route preference\n")

DEFSH (VTYSH_ISISD,
       ipv6_distance_h3c_cmd_vtysh,
       "preference <1-255>",
       "Set ISIS route preference\n"
       "Preference value\n")

DEFSH (VTYSH_ISISD,
       no_ipv6_distance_h3c_cmd_vtysh,
       "undo preference",
       "Cancel current configuration\n"
       "Set ISIS route preference\n")

DEFSH (VTYSH_ISISD,
       net_entity_h3c_cmd_vtysh,
       "network-entity WORD",
       "Set network entity title (NET) for ISIS\n"
       "XX.XXXX. ... .XXX.XX  Network entity title (NET)\n")

DEFSH (VTYSH_ISISD,
       no_net_entity_h3c_cmd_vtysh,
       "undo network-entity WORD",
       "Cancel current configuration\n"
       "Set network entity title (NET) for ISIS\n"
       "XX.XXXX. ... .XXX.XX  Network entity title (NET)\n")

DEFSH (VTYSH_ISISD,
       lsp_length_h3c_cmd_vtysh,
       "lsp-length <128-3000>",
       "Set maximum originated LSP or received LSP size\n"
       "Max LSP size in bytes\n")

DEFSH(VTYSH_ISISD,
      no_lsp_length_h3c_cmd_vtysh,
      "undo lsp-length",
      "Cancel current configuration\n"
      "Set maximum originated LSP or received LSP size\n")

DEFSH (VTYSH_ISISD,
       level_h3c_cmd_vtysh,
       "is-level (level-1|level-1-2|level-2)",
       "Set ISIS level for the system\n"
       "Level-1\n"
       "Level-1-2\n"
       "Level-2\n")

DEFSH (VTYSH_ISISD,
       no_level_h3c_cmd_vtysh,
       "undo is-level",
       "Cancel current configuration\n"
       "Set ISIS level for the system\n")

DEFSH (VTYSH_ISISD,
       lsp_gen_interval_h3c_cmd_vtysh,
       "timer lsp-generation <1-3600>",
       "Set ISIS timer\n"
       "Set the intelligent timer for LSP generation\n"
       "Set the intelligent timer maximum interval (Seconds)\n")

DEFSH (VTYSH_ISISD,
       no_lsp_gen_interval_h3c_cmd_vtysh,
       "undo timer lsp-generation",
       "Cancel current configuration\n"
       "Set ISIS timer\n"
       "Set the intelligent timer for LSP generation\n")

DEFSH (VTYSH_ISISD,
       metric_style_h3c_cmd_vtysh,
	   "cost-style (narrow|wide|wide-compatible|narrow-compatible|compatible)",
	   "Configure cost style for ISIS\n"
	   "Set cost style to narrow\n"
	   "Set cost style to wide\n"
	   "Set cost style to wide-compatible\n"
	   "Set cost style to narrow-compatible\n"
	   "Set cost style to compatible\n")


DEFSH (VTYSH_ISISD,
       no_metric_style_h3c_cmd_vtysh,
       "undo cost-style",
       "Cancel current configuration\n"
       "Configure cost style for ISIS\n")

DEFSH (VTYSH_ISISD,
       overload_bit_enable_h3c_cmd_vtysh,
       "set-overload",
       "Configure the router to advertise itself as overloaded\n")

DEFSH (VTYSH_ISISD,
       no_overload_bit_enable_h3c_cmd_vtysh,
       "undo set-overload",
       "Cancel current configuration\n"
       "Configure the router to advertise itself as overloaded\n")

DEFSH (VTYSH_ISISD,
       spf_interval_h3c_cmd_vtysh,
       "timer spf <1-3600>",
       "Set ISIS timer\n"
       "The timer for SPF calculations\n"
       "Maximum interval between ISPFs (Seconds)\n")

DEFSH (VTYSH_ISISD,
       no_spf_interval_h3c_cmd_vtysh,
       "undo timer spf",
       "Cancel current setting\n"
       "Set ISIS timer\n"
       "The timer for SPF calculations\n")

DEFSH (VTYSH_ISISD,
       lsp_lifetime_h3c_cmd_vtysh,
       "timer lsp-max-age <360-3600>",
       "Set ISIS timer\n"
       "Set maximum lifetime of LSP\n"
       "Maximum lifetime of LSP (Seconds)\n")

DEFSH (VTYSH_ISISD,
       no_lsp_lifetime_h3c_cmd_vtysh,
       "undo timer lsp-max-age",
       "Cancel current setting\n"
       "Set ISIS timer\n"
       "Set maximum lifetime of LSP\n")

DEFSH (VTYSH_ISISD,
       lsp_refresh_interval_h3c_cmd_vtysh,
       "timer lsp-refresh <1-3600>",
       "Set ISIS timer\n"
       "Set LSP refresh interval\n"
       "LSP refresh time (Seconds)\n")

DEFSH (VTYSH_ISISD,
       no_lsp_refresh_interval_h3c_cmd_vtysh,
       "undo timer lsp-refresh ",
       "Cancel current setting\n"
       "Set ISIS timer\n"
       "Set LSP refresh interval\n")

DEFSH (VTYSH_ISISD,
       router_summary_h3c_cmd_vtysh,
       "summary A.B.C.D (A.B.C.D|<0-32>) (level-1|level-2|level-1-2)",  
       "Configure summary address\n"
	   "Summary address\n"
	   "IP address mask\n"
	   "Summary address mask length\n"
       "level-1\n"
       "level-2\n"
       "level-1-2\n")

DEFSH (VTYSH_ISISD,
       no_router_summary_h3c_cmd_vtysh,
       "undo summary A.B.C.D (A.B.C.D|<0-32>) (level-1|level-2|level-1-2)",
       "Cancel current setting\n"
       "Configure summary address\n"
       "Summary address\n"
       "Summary address mask length\n"
       "IP address mask\n"
       "level-1\n"
       "level-2\n"
       "level-1-2\n")

DEFSH (VTYSH_ISISD,
       router_import_into_level2_h3c_cmd_vtysh,
       "import-route isis level-1 into level-2",
       "Import routes from other protocols into ISIS\n"
       "ISIS level-2 (source)\n"
       "Import into level-2\n"
       "ISIS level-1 (destination)\n")

DEFSH (VTYSH_ISISD,
       router_import_into_level1_h3c_cmd_vtysh,
       "import-route isis level-2 into level-1",
       "Import routes from other protocols into ISIS\n"
       "Intermediate System to Intermediate System (ISIS) routes\n"
       "Level-2\n"
       "Import into level-1\n"
       "Level-1\n")

DEFSH (VTYSH_ISISD,
       no_router_import_into_level2_h3c_cmd_vtysh,
       "undo import-route isis level-1 into level-2",
	   "Cancel current configuration\n"
	   "Import routes from other protocols into ISIS\n"
	   "ISIS level-2 (source)\n"
	   "Import into level-2\n"
	   "ISIS level-1 (destination)\n")

DEFSH (VTYSH_ISISD,
       no_router_import_into_level1_h3c_cmd_vtysh,
       "undo import-route isis level-2 into level-1",
	   "Cancel current configuration\n"
	   "Import routes from other protocols into ISIS\n"
	   "Intermediate System to Intermediate System (ISIS) routes\n"
	   "Level-2\n"
	   "Import into level-1\n"
	   "Level-1\n")  

DEFSH (VTYSH_ISISD,
       area_passwd_md5_h3c_cmd_vtysh,
       "area-authentication-mode md5 WORD",
       "Set the type and password of routing area authentication\n"
       "MD5 authentication type\n"
       "Area password length 1-254\n")

DEFSH (VTYSH_ISISD,
       area_passwd_clear_h3c_cmd_vtysh,
       "area-authentication-mode simple WORD",
       "Set the type and password of routing area authentication\n"
       "Plaintext authentication type\n"
       "Area password length 1-254\n")

DEFSH (VTYSH_ISISD,
       no_area_passwd_h3c_cmd_vtysh,
       "undo area-authentication-mode",
	   "Cancel current configuration\n"
       "Set the type and password of routing area authentication\n")

DEFSH (VTYSH_ISISD,
       domain_passwd_md5_h3c_cmd_vtysh,
       "domain-authentication-mode md5 WORD",
       "Set the type and password of routing domain authentication\n"
       "MD5 authentication type\n"
       "Authentication password length <1-254>\n")

DEFSH (VTYSH_ISISD,
       domain_passwd_clear_h3c_cmd_vtysh,
       "domain-authentication-mode simple WORD",
       "Set the type and password of routing domain authentication\n"
       "Plaintext authentication type\n"
       "Authentication password length <1-254>\n")

DEFSH (VTYSH_ISISD,
       no_domain_passwd_h3c_cmd_vtysh,
       "undo domain-authentication-mode",
       "Cancel current configuration\n"
       "Set the type and password of routing domain authentication\n")

DEFSH (VTYSH_ISISD,
       show_isis_instance_interface_h3c_cmd_vtysh,
       "display isis interface <1-255>",
       "Display current system information\n"
       "Intermediate System-to-Intermediate System (IS-IS) module\n"
       "Specify the interface\n"
       "Process ID\n")


DEFSH (VTYSH_ISISD,
       show_isis_interface_h3c_cmd_vtysh,
       "display isis interface",
       "Display current system information\n"
       "Intermediate System-to-Intermediate System (IS-IS) module\n"
       "Specify the interface\n")

DEFSH (VTYSH_ISISD,
		show_isis_interface_arg_h3c_cmd_vtysh,
		"display isis interface (ethernet|gigabitethernet|xgigabitethernet) USP",
		"Display current system information\n"
		"Intermediate System-to-Intermediate System (IS-IS) module\n"
		"Specify the interface\n"
		"Ethernet interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
        "GigabitEthernet interface type\n"
	    "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
	    "10GigabitEthernet interface type\n"
	    "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")


DEFSH (VTYSH_ISISD,
	   show_isis_loopback_interface_h3c_cmd_vtysh,
	   "display isis interface loopback <0-128>",
	   "Display current system information\n"
	   "Intermediate System-to-Intermediate System (IS-IS) module\n"
	   "Specify the interface\n"
	   "LoopBack interface\n"
	   "LoopBack interface number\n")

DEFSH (VTYSH_ISISD,
	   show_isis_vlanif_interface_h3c_cmd_vtysh,
	   "display isis interface vlan-interface <1-4094>",
	   "Display current system information\n"
	   "Intermediate System-to-Intermediate System (IS-IS) module\n"
	   "Specify the interface \n"
	   "Vlan interface\n"
	   "Vlan-interface interface number\n")

DEFSH (VTYSH_ISISD,
       show_isis_instance_neighbor_h3c_cmd_vtysh,
       "display isis peer <1-255>",
       "Display current system information\n"
       "Intermediate System-to-Intermediate System (IS-IS) module\n"
       "IS-IS neighbor\n"
       "Process ID\n")
       
DEFSH (VTYSH_ISISD,
       show_isis_instance_neighbor_detail_h3c_cmd_vtysh,
       "display isis peer verbose <1-255>",
       "Display current system information\n"
       "Intermediate System-to-Intermediate System (IS-IS) module\n"
       "IS-IS neighbor\n"
       "Detailed neighbor information\n"
       "Process ID\n")

DEFSH (VTYSH_ISISD,
       show_isis_neighbor_h3c_cmd_vtysh,
       "display isis peer",
       "Display current system information\n"
       "Intermediate System-to-Intermediate System (IS-IS) module\n"
       "IS-IS neighbor\n")

DEFSH (VTYSH_ISISD,
       show_isis_neighbor_detail_h3c_cmd_vtysh,
       "display isis peer verbose",
       "Display current system information\n"
       "Intermediate System-to-Intermediate System (IS-IS) module\n"
       "IS-IS neighbor\n"
       "Detailed neighbor information\n")

DEFSH (VTYSH_ISISD,
       show_database_h3c_cmd_vtysh,
       "display isis lsdb",
       "Display current system information\n"
       "Intermediate System-to-Intermediate System (IS-IS) module\n"
       "Link state database (LSDB)\n")

DEFSH (VTYSH_ISISD,
       show_database_detail_h3c_cmd_vtysh,
       "display isis lsdb verbose",
       "Display current system information\n"
       "Intermediate System-to-Intermediate System (IS-IS) module\n"
       "Link state database (LSDB)\n"
       "LSDB detailed information\n")

DEFSH (VTYSH_ISISD,
       show_database_instance_brief_h3c_cmd_vtysh,
       "display isis lsdb <1-255>",
       "Display current system information\n"
       "Intermediate System-to-Intermediate System (IS-IS) module\n"
       "Link state database (LSDB)\n"
       "Process ID\n")

DEFSH (VTYSH_ISISD,
       show_database_instance_detail_h3c_cmd_vtysh,
	   "display isis lsdb verbose <1-255>",
	   "Display current system information\n"
	   "Intermediate System-to-Intermediate System (IS-IS) module\n"
	   "Link state database (LSDB)\n"
	   "LSDB detailed information\n"
	   "Process ID\n")

DEFSH(VTYSH_ISISD,
	   show_isis_instance_route_h3c_cmd_vtysh,
       "display isis route <1-255>",
       "Display current system information\n"
       "ISIS status and configuration information\n"
       "Process ID for display\n"
       "ISIS route table\n")

DEFSH(VTYSH_ISISD,
	   show_isis_route_h3c_cmd_vtysh,
       "display isis route",
	   "Display current system information\n"
       "ISIS status and configuration information\n"
       "ISIS route table\n")

DEFSH (VTYSH_ISISD,
       show_isis_topology_l2_h3c_cmd_vtysh,
       "display isis topology level-2",
	   "Display current system information\n"
	   "ISIS status and configuration information\n"
       "IS-IS paths to Intermediate Systems\n"
       "Paths to all level-2 routers in the domain\n")

DEFSH (VTYSH_ISISD,
       show_isis_topology_l1_h3c_cmd_vtysh,
       "display isis topology level-1",
	   "Display current system information\n"
	   "ISIS status and configuration information\n"
       "IS-IS paths to Intermediate Systems\n"
       "Paths to all level-1 routers in the area\n")

DEFSH (VTYSH_ISISD,
       show_isis_topology_h3c_cmd_vtysh,
       "display isis topology",
       "Display current system information\n"
       "ISIS status and configuration information\n"
       "IS-IS paths to Intermediate Systems\n")

DEFSH(VTYSH_ISISD,
	   show_isis_route_ipv6_h3c_cmd_vtysh,
       "display isis ipv6 route",
	   "Display current system information\n"
	   "ISIS status and configuration information\n"
       "ISIS routing table\n")

DEFSH (VTYSH_ISISD,
       show_isis_statistics_h3c_cmd_vtysh,
       "display isis statistics",
 	   "Display current system information\n"
       "ISIS status and configuration information\n"
       "Statistics information\n")

DEFSH (VTYSH_ISISD,
       isis_debug_monitor_cmd_vtysh,
       "debug isis (enable|disable) (hello|spf|lsp|message|events|route|neighbor|psnp|csnp|send|receive|tlv|all)",
       "Debug information to moniter\n"
       "Programe name\n"
       "Enable statue\n"
       "Disatble statue\n"
       "Type name of packets hello\n"
       "Type name of spf messege\n"
       "Type name of lsp messege\n"
       "Type name of message log queue \n"
       "Type name of events log queue \n"
       "Type name of route status\n"
       "Type name of neighbor status\n"
       "Type name of packets psnp\n"
       "Type name of packets csnp\n"
       "Type name of packets send\n"
       "Type name of packets receive\n"
       "Type name of isis tlv\n"
       "Type name of all debug\n")

DEFSH(VTYSH_ISISD,
      no_router_summary_ipv6_cmd_vtysh,
      "undo ipv6 summary X:X::X:X/M (level-1|level-2|level-1-2)",
      "Cancel current setting\n"
      "Configure IPv6 commands for ISIS\n"
      "Configure summary address for IPv6\n"
      "IPv6 address\n"
      "Summary level-1\n"
      "Summary level-2\n"
      "Summary level-1-2\n")

DEFSH(VTYSH_ISISD,
      router_summary_ipv6_h3c_cmd_vtysh,
      "summary X:X::X:X <0-128> (level-1|level-2|level-1-2) ",
      "Configure summary address for IPv6\n"
      "IPv6 address\n"
      "IPv6 address mask length\n"
      "Summary level-1\n"
      "Summary level-2\n"
      "Summary level-1-2\n")

DEFSH(VTYSH_ISISD,
      no_router_summary_ipv6_h3c_cmd_vtysh,
      "undo summary X:X::X:X <0-128> (level-1|level-2|level-1-2)",
      "Cancel current setting\n"
	  "Configure summary address for IPv6\n"
	  "IPv6 address\n"
	  "IPv6 address mask length\n"
      "Summary level-1\n"
      "Summary level-2\n"
      "Summary level-1-2\n")



DEFSH(VTYSH_ISISD,
	  isis_change_all_cost_cmd_vtysh,
	  "circuit-cost <1-16777215>",
	  "Set the global default cost for all the interfaces\n"
	  "Cost value\n")
	  
DEFSH(VTYSH_ISISD,
	  isis_change_all_cost_v6_cmd_vtysh,
	  "circuit-cost <1-16777215>",
	  "Set the global default cost for all the interfaces\n"
	  "Cost value\n")

DEFSH(VTYSH_ISISD,
       no_isis_change_all_cost_cmd_vtysh,
       "undo circuit-cost",
       "Cancel current setting\n"
       "Set the global default cost for all the interfaces\n")
       
DEFSH(VTYSH_ISISD,
	   no_isis_change_all_cost_v6_cmd_vtysh,
	   "undo circuit-cost",
	   "Cancel current setting\n"
	   "Set the global default cost for all the interfaces\n")



DEFSH(VTYSH_ISISD,
		isis_bandwidth_reference_cmd_vtysh,
		"bandwidth-reference <1-2147483648>",
		"Set the bandwidth reference\n"
		"The reference bandwidth (Mbits/s)\n")
		
DEFSH(VTYSH_ISISD,
		isis_bandwidth_reference_v6_cmd_vtysh,
		"bandwidth-reference <1-2147483648>",
		"Set the bandwidth reference\n"
		"The reference bandwidth (Mbits/s)\n")

DEFSH(VTYSH_ISISD,
		undo_isis_bandwidth_reference_cmd_vtysh,
		"undo bandwidth-reference",
		"Cancel current setting\n"
		"Set the bandwidth reference\n")
		
DEFSH(VTYSH_ISISD,
		undo_isis_bandwidth_reference_v6_cmd_vtysh,
		"undo bandwidth-reference",
		"Cancel current setting\n"
		"Set the bandwidth reference\n")



DEFSH(VTYSH_ISISD,
      isis_ipv6_redistribute_h3c_cmd_vtysh,
      "import-route" ISISV6_REDIST_STR_ISISD
      " (level-1|level-2|level-1-2) {cost <0-4261412864>}",
      "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "Set the cost value\n"
      "narrow <0-63>,wide<0-4261412864>\n")

DEFSH(VTYSH_ISISD,
      no_isis_ipv6_redistribute_h3c_without_level_cmd_vtysh,
      "undo import-route" ISISV6_REDIST_STR_ISISD,
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD)

DEFSH(VTYSH_ISISD,
      isis_ipv6_redistribute_instance_h3c_cmd_vtysh,
      "import-route" ISISV6_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2) {cost <0-4261412864>}",
	  "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "Process ID\n"
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n"
      "Set the cost value\n"
      "narrow <0-63>,wide<0-4261412864>\n")

DEFSH(VTYSH_ISISD,
      no_isis_ipv6_redistribute_instance_h3c_cmd_vtysh,
      "undo import-route" ISISV6_REDIST_STR_ISISD_WITH_INSTANCE
      "<1-255> (level-1|level-2|level-1-2)",
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      ISIS_REDIST_HELP_STR_ISISD_WITH_INSTANCE
      "instance number\n"
      "Level-1\n"
      "Level-2\n"
      "Level-1-2\n")




DEFSH(VTYSH_ISISD,
      router_import_v6_into_level2_h3c_cmd_vtysh,
      "import-route isisv6 level-1 into level-2",
      "Import routes from other protocols into ISIS\n"
      "ISIS level-2 (source)\n"
      "Import into level-2\n"
      "ISIS level-1 (destination)\n")

DEFSH(VTYSH_ISISD,
      router_import_v6_into_level1_h3c_cmd_vtysh,
      "import-route isisv6 level-2 into level-1",
      "Import routes from other protocols into ISIS\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Level-2\n"
      "Import into level-1\n"
      "Level-1\n")

DEFSH(VTYSH_ISISD,
      no_router_import_v6_into_level2_h3c_cmd_vtysh,
      "undo import-route isisv6 level-1 into level-2",
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      "ISIS level-2 (source)\n"
      "Import into level-2\n"
      "ISIS level-1 (destination)\n")

DEFSH(VTYSH_ISISD,
      no_router_import_v6_into_level1_h3c_cmd_vtysh,
      "undo import-route isisv6 level-2 into level-1",
      "Cancel current configuration\n"
      "Import routes from other protocols into ISIS\n"
      "Intermediate System to Intermediate System (ISIS) routes\n"
      "Level-2\n"
      "Import into level-1\n"
      "Level-1\n")


void vtysh_init_isis_cmd (void)
{
  install_node (&isis_node, NULL);  
  install_node (&isis_family_v4_node, NULL);  
  install_node (&isis_family_v6_node, NULL);  

  
  vtysh_install_default (ISIS_NODE);
  vtysh_install_default (ISIS_FAMILY_V4_NODE);
  vtysh_install_default (ISIS_FAMILY_V6_NODE);

  install_element_level (ISIS_NODE, &isis_bandwidth_reference_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &undo_isis_bandwidth_reference_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (ISIS_FAMILY_V6_NODE, &isis_bandwidth_reference_v6_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &undo_isis_bandwidth_reference_v6_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &isis_change_all_cost_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_isis_change_all_cost_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &isis_change_all_cost_v6_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &no_isis_change_all_cost_v6_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (CONFIG_NODE, &isis_debug_monitor_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &vtysh_exit_isis_cmd,VISIT_LEVE, CMD_SYNC);
  install_element_level (ISIS_NODE, &vtysh_quit_isis_cmd,VISIT_LEVE, CMD_SYNC);

  install_element_level (ISIS_FAMILY_V4_NODE, &vtysh_exit_isis_cmd,VISIT_LEVE, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V4_NODE, &vtysh_quit_isis_cmd,VISIT_LEVE, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &vtysh_exit_isis_cmd,VISIT_LEVE, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &vtysh_quit_isis_cmd,VISIT_LEVE, CMD_SYNC);

  install_element_level (CONFIG_NODE, &clear_isis_neighbor_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (CONFIG_NODE, &clear_isis_neighbor_arg_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);

  /*isis������ע��*/
  install_element_level (ISIS_NODE, &net_entity_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_net_entity_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (CONFIG_NODE, &isis_instance_cmd, VISIT_LEVE, CMD_SYNC);
  install_element_level (CONFIG_NODE, &no_isis_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &level_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_level_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &metric_style_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_metric_style_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &lsp_gen_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_lsp_gen_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &lsp_refresh_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_lsp_refresh_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &lsp_lifetime_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_lsp_lifetime_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &lsp_length_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_lsp_length_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
   
  install_element_level (ISIS_NODE, &spf_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_spf_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &overload_bit_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_overload_bit_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &router_summary_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_router_summary_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


  install_element_level (ISIS_NODE, &router_summary_ipv6_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_router_summary_ipv6_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &router_import_into_level1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &router_import_into_level2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_router_import_into_level2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);  
  install_element_level (ISIS_NODE, &no_router_import_into_level1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &isis_redistribute_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_isis_redistribute_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &isis_redistribute_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_isis_redistribute_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &isis_redistribute_arg_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &isis_redistribute_two_arg_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &isis_redistribute_instance_arg_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &isis_redistribute_instance_two_arg_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (ISIS_FAMILY_V4_NODE, &isis_redistribute_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V4_NODE, &no_isis_redistribute_h3c_without_level_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V4_NODE, &isis_redistribute_instance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V4_NODE, &no_isis_redistribute_instance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (ISIS_FAMILY_V4_NODE, &isis_redistribute_arg_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V4_NODE, &isis_redistribute_two_arg_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V4_NODE, &isis_redistribute_instance_arg_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V4_NODE, &isis_redistribute_instance_two_arg_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_FAMILY_V6_NODE, &isis_ipv6_redistribute_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &no_isis_ipv6_redistribute_h3c_without_level_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &isis_ipv6_redistribute_instance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &no_isis_ipv6_redistribute_instance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &area_passwd_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &area_passwd_md5_snpauth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &area_passwd_clear_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &area_passwd_clear_snpauth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_area_passwd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (ISIS_NODE, &domain_passwd_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &domain_passwd_md5_snpauth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &domain_passwd_clear_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &domain_passwd_clear_snpauth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_domain_passwd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


  install_element_level (ISIS_NODE, &net_entity_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_net_entity_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (CONFIG_NODE, &isis_instance_h3c_cmd, VISIT_LEVE, CMD_SYNC);
  install_element_level (CONFIG_NODE, &no_isis_instance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &isis_address_family_ipv4_cmd, VISIT_LEVE, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &isis_address_family_ipv4_cmd, VISIT_LEVE, CMD_SYNC);
  install_element_level (ISIS_NODE, &isis_address_family_ipv6_cmd, VISIT_LEVE, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V4_NODE, &isis_address_family_ipv6_cmd, VISIT_LEVE, CMD_SYNC);

  install_element_level (ISIS_NODE, &level_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_level_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &metric_style_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_metric_style_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &lsp_gen_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_lsp_gen_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &lsp_refresh_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_lsp_refresh_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &lsp_lifetime_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_lsp_lifetime_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &lsp_length_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_lsp_length_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
   
  install_element_level (ISIS_NODE, &spf_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_spf_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &overload_bit_enable_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_overload_bit_enable_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &distance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_distance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_FAMILY_V4_NODE, &ipv4_distance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V4_NODE, &no_ipv4_distance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &ipv6_distance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &no_ipv6_distance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_FAMILY_V4_NODE, &router_summary_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V4_NODE, &no_router_summary_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &router_summary_ipv6_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &no_router_summary_ipv6_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_FAMILY_V4_NODE, &router_import_into_level1_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V4_NODE, &router_import_into_level2_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V4_NODE, &no_router_import_into_level2_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);  
  install_element_level (ISIS_FAMILY_V4_NODE, &no_router_import_into_level1_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_FAMILY_V6_NODE, &router_import_v6_into_level1_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &router_import_v6_into_level2_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_FAMILY_V6_NODE, &no_router_import_v6_into_level2_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);  
  install_element_level (ISIS_FAMILY_V6_NODE, &no_router_import_v6_into_level1_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (ISIS_NODE, &area_passwd_md5_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &area_passwd_clear_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_area_passwd_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (ISIS_NODE, &domain_passwd_md5_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &domain_passwd_clear_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (ISIS_NODE, &no_domain_passwd_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_SUBIF_NODE, &isis_enable_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_enable_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &isis_enable_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_enable_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_SUBIF_NODE, &isis_enable_instance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_enable_instance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &isis_enable_instance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_enable_instance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_enable_ipv6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_enable_ipv6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &isis_enable_ipv6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_enable_ipv6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_IF_NODE, &isis_passive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_passive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_passive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_passive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_SUBIF_NODE, &isis_circuit_type_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  //install_element (PHYSICAL_SUBIF_NODE, &no_isis_circuit_type_cmd_vtysh);
  install_element_level (PHYSICAL_IF_NODE, &isis_circuit_type_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  //install_element (PHYSICAL_IF_NODE, &no_isis_circuit_type_cmd_vtysh);

  install_element_level (PHYSICAL_SUBIF_NODE, &isis_circuit_type_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  //install_element (PHYSICAL_SUBIF_NODE, &no_isis_circuit_type_h3c_cmd_vtysh, CONFIG_LEVE_5);
  install_element_level (PHYSICAL_IF_NODE, &isis_circuit_type_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  //install_element (PHYSICAL_IF_NODE, &no_isis_circuit_type_h3c_cmd_vtysh, CONFIG_LEVE_5);
  
  install_element_level (PHYSICAL_IF_NODE, &isis_passwd_clear_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &isis_passwd_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_passwd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_passwd_clear_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_passwd_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_passwd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_IF_NODE, &isis_passwd_clear_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &isis_passwd_md5_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_passwd_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_passwd_clear_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_passwd_md5_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_passwd_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  
  install_element_level (PHYSICAL_IF_NODE, &isis_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_priority_arg_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &isis_priority_l1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_priority_l1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &isis_priority_l2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_priority_l2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_IF_NODE, &isis_priority_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_priority_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &isis_priority_l1_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_priority_l1_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &isis_priority_l2_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_priority_l2_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_priority_arg_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_priority_l1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_priority_l1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_priority_l2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_priority_l2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_SUBIF_NODE, &isis_priority_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_priority_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_priority_l1_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_priority_l1_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_priority_l2_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_priority_l2_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  
  install_element_level (PHYSICAL_IF_NODE, &isis_metric_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_metric_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_metric_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_metric_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_IF_NODE, &isis_metric_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_metric_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_metric_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_metric_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &isis_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_IF_NODE, &isis_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_SUBIF_NODE, &isis_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

  install_element_level (PHYSICAL_IF_NODE, &isis_hello_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_hello_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_IF_NODE, &isis_hello_multiplier_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_hello_multiplier_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_hello_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_hello_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_SUBIF_NODE, &isis_hello_multiplier_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_hello_multiplier_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &csnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_csnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_IF_NODE, &csnp_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_csnp_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

  install_element_level (PHYSICAL_SUBIF_NODE, &csnp_interval_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_csnp_interval_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

  install_element_level (PHYSICAL_SUBIF_NODE, &csnp_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_csnp_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

  install_element_level (PHYSICAL_IF_NODE, &psnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_psnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_SUBIF_NODE, &psnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_psnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_IF_NODE, &isis_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (PHYSICAL_IF_NODE, &isis_network_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_isis_network_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &isis_network_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_isis_network_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &hello_padding_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_hello_padding_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &hello_padding_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_hello_padding_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &hello_padding_enable_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_SUBIF_NODE, &no_hello_padding_enable_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &hello_padding_enable_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (PHYSICAL_IF_NODE, &no_hello_padding_enable_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  /*                                 trunk init                               */
  install_element_level (TRUNK_SUBIF_NODE, &isis_enable_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_enable_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_enable_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_enable_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_enable_instance_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_enable_instance_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_enable_instance_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_enable_instance_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   
  install_element_level (TRUNK_SUBIF_NODE, &isis_enable_ipv6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_enable_ipv6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_enable_ipv6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_enable_ipv6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
   
  install_element_level (TRUNK_IF_NODE, &isis_passive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_passive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_passive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_passive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
   
  install_element_level (TRUNK_SUBIF_NODE, &isis_circuit_type_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_circuit_type_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
   
  install_element_level (TRUNK_SUBIF_NODE, &isis_circuit_type_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_circuit_type_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_passwd_clear_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_passwd_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_passwd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_passwd_clear_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_passwd_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_passwd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
   
  install_element_level (TRUNK_IF_NODE, &isis_passwd_clear_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_passwd_md5_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_passwd_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_passwd_clear_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_passwd_md5_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_passwd_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_priority_arg_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_priority_l1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_priority_l1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_priority_l2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_priority_l2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);  

  install_element_level (TRUNK_IF_NODE, &isis_priority_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_priority_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_priority_l1_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_priority_l1_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_priority_l2_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_priority_l2_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);  
  
  install_element_level (TRUNK_SUBIF_NODE, &isis_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_priority_l1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_priority_l1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_priority_l2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_priority_l2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);  

  install_element_level (TRUNK_SUBIF_NODE, &isis_priority_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_priority_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_priority_l1_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_priority_l1_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_priority_l2_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_priority_l2_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);  
  
  install_element_level (TRUNK_IF_NODE, &isis_metric_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_metric_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_metric_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_metric_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_metric_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_metric_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_metric_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_metric_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  
  install_element_level (TRUNK_IF_NODE, &isis_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (TRUNK_IF_NODE, &isis_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  
  install_element_level (TRUNK_IF_NODE, &isis_hello_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_hello_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC); 
  install_element_level (TRUNK_SUBIF_NODE, &isis_hello_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &isis_hello_multiplier_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_hello_multiplier_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC); 
  install_element_level (TRUNK_SUBIF_NODE, &isis_hello_multiplier_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  
  install_element_level (TRUNK_IF_NODE, &csnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_csnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &csnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_csnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &csnp_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_csnp_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &csnp_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_csnp_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  
  install_element_level (TRUNK_IF_NODE, &psnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_psnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &psnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_psnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (TRUNK_IF_NODE, &isis_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (TRUNK_IF_NODE, &isis_network_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_isis_network_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &isis_network_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_isis_network_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &hello_padding_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_hello_padding_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &hello_padding_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_hello_padding_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (TRUNK_SUBIF_NODE, &hello_padding_enable_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_SUBIF_NODE, &no_hello_padding_enable_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &hello_padding_enable_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  install_element_level (TRUNK_IF_NODE, &no_hello_padding_enable_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  

    /*                         loopback init            */
  install_element_level (LOOPBACK_IF_NODE, &isis_enable_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_isis_enable_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
   
  install_element_level (LOOPBACK_IF_NODE, &isis_enable_ipv6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_isis_enable_ipv6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
   
  install_element_level (LOOPBACK_IF_NODE, &isis_passive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_isis_passive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
   
  install_element_level (LOOPBACK_IF_NODE, &isis_circuit_type_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
   
  install_element_level (LOOPBACK_IF_NODE, &isis_passwd_clear_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &isis_passwd_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_isis_passwd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

  install_element_level (LOOPBACK_IF_NODE, &isis_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_isis_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &isis_priority_l1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_isis_priority_l1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &isis_priority_l2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_isis_priority_l2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);  

  install_element_level (LOOPBACK_IF_NODE, &isis_metric_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_isis_metric_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (LOOPBACK_IF_NODE, &isis_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_isis_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (LOOPBACK_IF_NODE, &isis_hello_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_isis_hello_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC); 
  
  install_element_level (LOOPBACK_IF_NODE, &csnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_csnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (LOOPBACK_IF_NODE, &psnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_psnp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (LOOPBACK_IF_NODE, &isis_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_isis_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  
  install_element_level (LOOPBACK_IF_NODE, &hello_padding_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
  install_element_level (LOOPBACK_IF_NODE, &no_hello_padding_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

   install_element_level (LOOPBACK_IF_NODE, &isis_enable_instance_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &no_isis_enable_instance_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	
   install_element_level (LOOPBACK_IF_NODE, &isis_circuit_type_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	
   install_element_level (LOOPBACK_IF_NODE, &isis_passwd_clear_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &isis_passwd_md5_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &no_isis_passwd_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
  
   install_element_level (LOOPBACK_IF_NODE, &isis_priority_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &no_isis_priority_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &isis_priority_l1_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &no_isis_priority_l1_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &isis_priority_l2_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &no_isis_priority_l2_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);  
  
   install_element_level (LOOPBACK_IF_NODE, &isis_metric_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &no_isis_metric_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   
   install_element_level (LOOPBACK_IF_NODE, &isis_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &no_isis_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   
   install_element_level (LOOPBACK_IF_NODE, &isis_hello_multiplier_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &no_isis_hello_multiplier_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC); 
   
   install_element_level (LOOPBACK_IF_NODE, &csnp_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &no_csnp_interval_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   
   install_element_level (LOOPBACK_IF_NODE, &isis_network_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &no_isis_network_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   
   install_element_level (LOOPBACK_IF_NODE, &hello_padding_enable_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (LOOPBACK_IF_NODE, &no_hello_padding_enable_h3c_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

  	install_element_level (VLANIF_NODE, &isis_enable_instance_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_isis_enable_instance_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

    install_element_level (VLANIF_NODE, &isis_enable_ipv6_instance_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_isis_enable_ipv6_instance_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);		

	install_element_level (VLANIF_NODE, &isis_passive_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_isis_passive_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (VLANIF_NODE, &isis_circuit_type_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (VLANIF_NODE, &isis_passwd_clear_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &isis_passwd_md5_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_isis_passwd_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (VLANIF_NODE, &isis_priority_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_isis_priority_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (VLANIF_NODE, &isis_priority_l1_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_isis_priority_l1_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &isis_priority_l2_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_isis_priority_l2_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	   	
	install_element_level (VLANIF_NODE, &isis_metric_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_isis_metric_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (VLANIF_NODE, &isis_hello_interval_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_isis_hello_interval_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (VLANIF_NODE, &isis_hello_multiplier_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_isis_hello_multiplier_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (VLANIF_NODE, &csnp_interval_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_csnp_interval_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
    install_element_level (VLANIF_NODE, &psnp_interval_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_psnp_interval_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (VLANIF_NODE, &isis_network_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_isis_network_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (VLANIF_NODE, &hello_padding_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_hello_padding_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	    install_element_level (VLANIF_NODE, &isis_enable_instance_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &no_isis_enable_instance_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
		install_element_level (VLANIF_NODE, &isis_circuit_type_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		
		install_element_level (VLANIF_NODE, &isis_passwd_clear_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &isis_passwd_md5_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &no_isis_passwd_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
		install_element_level (VLANIF_NODE, &isis_priority_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &no_isis_priority_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
		install_element_level (VLANIF_NODE, &isis_priority_l1_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &no_isis_priority_l1_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &isis_priority_l2_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &no_isis_priority_l2_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
			
		install_element_level (VLANIF_NODE, &isis_metric_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &no_isis_metric_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
		install_element_level (VLANIF_NODE, &isis_hello_interval_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &no_isis_hello_interval_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
		install_element_level (VLANIF_NODE, &isis_hello_multiplier_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &no_isis_hello_multiplier_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
		install_element_level (VLANIF_NODE, &csnp_interval_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &no_csnp_interval_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
		install_element_level (VLANIF_NODE, &isis_network_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &no_isis_network_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
		install_element_level (VLANIF_NODE, &hello_padding_enable_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (VLANIF_NODE, &no_hello_padding_enable_h3c_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

    
   /*                           show                      */
  install_element_level (CONFIG_NODE, &show_isis_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_interface_arg_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (CONFIG_NODE, &show_isis_loopback_interface_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_trunk_interface_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_vlanif_interface_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);

  //install_element (CONFIG_NODE, &show_isis_interface_gigabit_ethernet_arg_cmd_vtysh);
  //install_element (CONFIG_NODE, &show_isis_interface_xgigabit_ethernet_arg_cmd_vtysh);  
  install_element_level (CONFIG_NODE, &show_isis_instance_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (CONFIG_NODE, &show_isis_neighbor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_neighbor_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (CONFIG_NODE, &show_isis_instance_neighbor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_instance_neighbor_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    
  install_element_level (CONFIG_NODE, &show_database_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_lsp_arg_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_database_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (CONFIG_NODE, &show_isis_debug_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (CONFIG_NODE, &show_isis_topology_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_topology_l1_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_topology_l2_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (CONFIG_NODE, &show_isis_route_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  //install_element (CONFIG_NODE, &show_isis_redist_route_cmd_vtysh);
  install_element_level (CONFIG_NODE, &show_isis_instance_route_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_route_ipv6_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (CONFIG_NODE, &show_database_instance_brief_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_database_instance_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_statistics_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);


  install_element_level (ISIS_NODE, &show_isis_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_interface_arg_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_NODE, &show_isis_loopback_interface_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_trunk_interface_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_vlanif_interface_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);

  //install_element (CONFIG_NODE, &show_isis_interface_gigabit_ethernet_arg_cmd_vtysh);
  //install_element (CONFIG_NODE, &show_isis_interface_xgigabit_ethernet_arg_cmd_vtysh);  
  install_element_level (ISIS_NODE, &show_isis_instance_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (ISIS_NODE, &show_isis_neighbor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_neighbor_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (ISIS_NODE, &show_isis_instance_neighbor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_instance_neighbor_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    
  install_element_level (ISIS_NODE, &show_database_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_lsp_arg_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_database_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_NODE, &show_isis_debug_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (ISIS_NODE, &show_isis_topology_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_topology_l1_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_topology_l2_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_NODE, &show_isis_route_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  //install_element (CONFIG_NODE, &show_isis_redist_route_cmd_vtysh);
  install_element_level (ISIS_NODE, &show_isis_instance_route_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_route_ipv6_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_NODE, &show_database_instance_brief_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_database_instance_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_statistics_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);


  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_interface_arg_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_loopback_interface_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_trunk_interface_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_vlanif_interface_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);

  //install_element (CONFIG_NODE, &show_isis_interface_gigabit_ethernet_arg_cmd_vtysh);
  //install_element (CONFIG_NODE, &show_isis_interface_xgigabit_ethernet_arg_cmd_vtysh);  
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_instance_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_neighbor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_neighbor_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_instance_neighbor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_instance_neighbor_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    
  install_element_level (ISIS_FAMILY_V4_NODE, &show_database_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_lsp_arg_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_database_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_debug_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_topology_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_topology_l1_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_topology_l2_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_route_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  //install_element (CONFIG_NODE, &show_isis_redist_route_cmd_vtysh);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_instance_route_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_route_ipv6_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_FAMILY_V4_NODE, &show_database_instance_brief_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_database_instance_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_statistics_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

   /*                           show  h3c                     */
  install_element_level (CONFIG_NODE, &show_isis_interface_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_interface_arg_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (CONFIG_NODE, &show_isis_loopback_interface_h3c_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_vlanif_interface_h3c_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (CONFIG_NODE, &show_isis_instance_interface_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (CONFIG_NODE, &show_isis_neighbor_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_neighbor_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (CONFIG_NODE, &show_isis_instance_neighbor_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_instance_neighbor_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    
  install_element_level (CONFIG_NODE, &show_database_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_database_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (CONFIG_NODE, &show_isis_topology_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_topology_l1_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_topology_l2_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (CONFIG_NODE, &show_isis_route_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_instance_route_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_route_ipv6_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (CONFIG_NODE, &show_database_instance_brief_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_database_instance_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &show_isis_statistics_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);


  install_element_level (ISIS_NODE, &show_isis_interface_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_interface_arg_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_NODE, &show_isis_loopback_interface_h3c_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_vlanif_interface_h3c_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (ISIS_NODE, &show_isis_instance_interface_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (ISIS_NODE, &show_isis_neighbor_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_neighbor_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (ISIS_NODE, &show_isis_instance_neighbor_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_instance_neighbor_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    
  install_element_level (ISIS_NODE, &show_database_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_database_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  

  install_element_level (ISIS_NODE, &show_isis_topology_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_topology_l1_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_topology_l2_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_NODE, &show_isis_route_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_instance_route_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_route_ipv6_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_NODE, &show_database_instance_brief_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_database_instance_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_NODE, &show_isis_statistics_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);


  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_interface_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_interface_arg_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_loopback_interface_h3c_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_vlanif_interface_h3c_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
 
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_instance_interface_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_neighbor_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_neighbor_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_instance_neighbor_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_instance_neighbor_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    
  install_element_level (ISIS_FAMILY_V4_NODE, &show_database_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_database_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  

  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_topology_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_topology_l1_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_topology_l2_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_route_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_instance_route_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_route_ipv6_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  
  install_element_level (ISIS_FAMILY_V4_NODE, &show_database_instance_brief_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_database_instance_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
  install_element_level (ISIS_FAMILY_V4_NODE, &show_isis_statistics_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

  
  /*              debug                */
//   install_element_level (CONFIG_NODE, &debug_isis_packet_dump_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
//   install_element_level (CONFIG_NODE, &no_debug_isis_packet_dump_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

//   install_element_level (CONFIG_NODE, &debug_isis_spf_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
//   install_element_level (CONFIG_NODE, &no_debug_isis_spf_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

//   install_element_level (CONFIG_NODE, &debug_isis_lsp_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
//   install_element_level (CONFIG_NODE, &no_debug_isis_lsp_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

//   install_element_level (CONFIG_NODE, &debug_isis_events_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
//   install_element_level (CONFIG_NODE, &no_debug_isis_events_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

  

  install_element_level (CONFIG_NODE, &isisd_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
  install_element_level (CONFIG_NODE, &isisd_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

}
