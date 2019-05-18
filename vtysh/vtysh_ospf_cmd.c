/**
 * \page cmds_ref_ospf OSPF
 * - \subpage modify_log_ospf
 * 
 */
 
/**
 * \page modify_log_ospf Modify Log
 * \section ospf-v007r004 HiOS-V007R004
 *  -# 
 * \section ospf-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"

static struct cmd_node ospf_node =
{
  OSPF_NODE,
  "%s(config-ospf)# "
};
  
static struct cmd_node dcn_node =
{
  OSPF_DCN_NODE,
  "%s(config-dcn)# "
};

static struct cmd_node area_node =
{
  AREA_NODE,
  "%s(config-ospf-area)# "
};


DEFUNSH (VTYSH_OSPFD,
	router_ospf,
	router_ospf_cmd,
	"ospf instance <1-255> {vpn-instance <1-127>}",
	"Enable a OSPF instance\n"
	"Start OSPF configuration\n"
	"OSPF instance number\n"
	"ospf vpn\n"
	"ospf Vpnid format <1-127> value\n")
{
	vty->node = OSPF_NODE;
	return CMD_SUCCESS;
}


DEFUNSH (VTYSH_OSPFD,
	router_ospf_h3c,
	router_ospf_cmd_h3c,
	"ospf <1-65535> {router-id A.B.C.D | vpn-instance STRING}",
	"Enable a OSPF instance\n"
	"OSPF instance number\n"
	"router-id for the OSPF instance\n"
    "OSPF router-id in IP address format\n"
	"ospf vpn\n"
	"VPN instance name\n")
{
	vty->node = OSPF_NODE;
	return CMD_SUCCESS;
}

	 
DEFUNSH (VTYSH_OSPFD,
	dcn_ospf,
	dcn_ospf_cmd,
	"dcn",
	"Enter Data Communication Network and enable\n")
{
	vty->node = OSPF_DCN_NODE;
	return CMD_SUCCESS;
}


DEFUNSH (VTYSH_OSPFD,
	 area_ospf,
	 area_ospf_cmd,
	 "area <0-255>",
	 "enter area node\n"
     "OSPF area ID as a decimal value\n")
{
	vty->node = AREA_NODE;
	return CMD_SUCCESS;
}


DEFUNSH (VTYSH_OSPFD,
	area_ospf_h3c,
	area_ospf_address_format_cmd,
	"area A.B.C.D",
	"enter area node\n"
	"OSPF area ID as a ip address format\n")
{
	vty->node = AREA_NODE;
	return CMD_SUCCESS;
}


DEFSH (VTYSH_OSPFD,no_area_ospf_cmd_vtysh,
       "no area <0-255>",
       "Negate a command or set its defaults\n"
       "diable area node\n"
       "OSPF area ID as a decimal value\n")


DEFSH (VTYSH_OSPFD,no_area_ospf_address_format_cmd_vtysh,
	"no area A.B.C.D",
	"Negate a command or set its defaults\n"
	"diable area node\n"
	"OSPF area ID as a ip address format\n")


DEFSH (VTYSH_OSPFD,	no_area_ospf_h3c_cmd_vtysh,
	"undo area A.B.C.D",
	"Cancel current configuration\n"
	"disable area node\n"
	"OSPF area ID as a ip address format\n")


DEFUNSH (VTYSH_OSPFD,
	 vtysh_exit_ospfd,
	 vtysh_exit_ospfd_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

ALIAS (vtysh_exit_ospfd,
	vtysh_quit_ospfd_cmd,
	"quit",
	"Exit current mode and down to previous mode\n")
			

DEFUNSH (VTYSH_OSPFD,
	vtysh_exit_dcn,
	vtysh_exit_dcn_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit (vty);
}


ALIAS(vtysh_exit_dcn,
	vtysh_quit_dcn_cmd,
	"quit",
	"Exit current mode and down to previous mode\n")


DEFUNSH (VTYSH_OSPFD,
	 vtysh_exit_area,
	 vtysh_exit_area_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}


ALIAS(vtysh_exit_area,
	vtysh_quit_area_cmd,
	"quit",
	"Exit current mode and down to previous mode\n")


DEFSH (VTYSH_OSPFD, no_router_ospf_cmd_vtysh, 
       "no ospf instance <1-255>", 
       "Negate a command or set its defaults\n"
       "Enable a OSPF instance\n"
       "Start OSPF configuration\n"
       "OSPF instance number\n")    


DEFSH (VTYSH_OSPFD, undo_router_ospf_h3c_cmd_vtysh,
	 "undo ospf [<1-65535>]",
	 "Cancel current configuration\n"
	 "Enable a OSPF instance\n"
	 "OSPF instance number\n")


DEFSH (VTYSH_OSPFD, no_dcn_ospf_cmd_vtysh, 
		"no dcn",
		NO_STR
		"OSPF DCN node\n") 


DEFSH (VTYSH_OSPFD, undo_dcn_ospf_cmd_vtysh, 
		"undo dcn",
		"Cancel current setting\n"
		"OSPF DCN node\n")


DEFSH (VTYSH_OSPFD, ospf_reset_instance_cmd_vtysh, 
	   "reset ospf instance <1-255>", 
	   "Reset operation\n"
	   "OSPF information\n"
	   "Start instance\n"
	   "OSPF instance number\n")


DEFSH (VTYSH_OSPFD, ospf_reset_instance_h3c_cmd_vtysh,
	 "reset ospf [ <1-65535> ] process [ graceful-restart ]",
	 "Reset operation\n"
	 "OSPF information\n"
	 "OSPF instance number\n"
	 "OSPF instance\n"
	 "Restart with graceful-restart way\n")
		 

DEFSH (VTYSH_OSPFD, ospf_vpn_cmd_vtysh, 
	   "vpn VPNID",
       "ospf vpn \n"
	   "ospf vpn Vpnid format <0-4095> value\n")
	   
DEFSH (VTYSH_OSPFD, no_ospf_vpn_cmd_vtysh,
       "no vpn",
       "Negate a command or set its defaults\n"
       "ospf vpn\n")


DEFSH (VTYSH_OSPFD, dcn_u0_test_cmd_vtysh,
		"u0 add A.B.C.D ifindex HEX<0x1000001-0xFFFFFFFF>",
		"device name\n"
		"Add action\n"
		"New device ne-ip\n"
		"U0 ifindex\n"
		"ifindex\n")

DEFSH (VTYSH_OSPFD, dcn_u0_test_show_cmd_vtysh,
	 "show u0",
	 "Show running system information\n"
	 "U0 device info\n")
			 

DEFSH (VTYSH_OSPFD, dcn_trap_report_cmd_vtysh,
       "auto-report",
       "Enable network element info trap-report\n")

DEFSH (VTYSH_OSPFD, no_dcn_trap_report_cmd_vtysh,
       "no auto-report",
       "Negate a command or set its defaults\n"
       "network element info trap-report\n")


DEFSH (VTYSH_OSPFD, undo_dcn_trap_report_cmd_vtysh,
       "undo auto-report",
       "Cancel current setting\n"
       "network element info trap-report\n")


DEFSH (VTYSH_OSPFD, dcn_ne_ip_set_cmd_vtysh,
		"ne-ip A.B.C.D/M",
		"Set NE IP address\n"
		"OSPF network prefix\n")


DEFSH (VTYSH_OSPFD, dcn_ne_ip_set_same_h3c_cmd_vtysh,
		"ne-ip A.B.C.D (<0-32>|A.B.C.D)",
		"Set NE IP address\n"
		"Ne_ip prefix\n"
		"Ne_ip netmask length(0~32)\n"
		"Ne_ip netmask\n")


DEFSH (VTYSH_OSPFD, dcn_ne_id_set_cmd_vtysh,
	 "ne-id HEX<0x10001-0xFEFFFE>",
	 "Set NE ID\n"
	 "Net-Element ID value, ne-id=subnetid<<16+baseid,subnetid:0x01~0xfe, baseid:0x0001~0xfffe. SAMPLE: 90001\n")



DEFSH (VTYSH_OSPFD, show_ip_ospf_dcn_cmd_vtysh,
		"show dcn (self|ne-info)",
		SHOW_STR
		"OSPF DCN\n"
		"self DCN brief information\n"
		"ne information")


DEFSH (VTYSH_OSPFD, display_ip_ospf_dcn_cmd_vtysh,
		"display dcn (self|ne-info)",
		SHOW_STR
		"OSPF DCN\n"
		"self DCN brief information\n"
		"ne information")


DEFSH (VTYSH_OSPFD, ospf_bfd_cmd_vtysh,
		"ip ospf bfd enable",
		"IP information\n"
		"OSPF interface commands\n"
		"Enable bfd on this interface\n"
		"Enable BFD\n")


DEFSH (VTYSH_OSPFD, no_ospf_bfd_cmd_vtysh,
		"no ip ospf bfd enable",
		NO_STR
		"IP information\n"
		"OSPF interface commands\n"
		"Enable bfd on this interface\n"
		"Enable BFD\n")

	 
DEFSH (VTYSH_OSPFD, ospf_bfd_sess_cc_interval_set_cmd_vtysh,
		"ip ospf bfd cc-interval (3 | 10 | 20 | 100 | 300 | 1000)",
		"IP information\n"
		"OSPF interface commands\n"
		BFD_STR
		"Set BFD session minimum receive interval\n"
		"3.3ms\n"
		"10ms, default is 10ms\n"
		"20ms\n"
		"100ms\n"
		"300ms\n"
		"1000ms\n")

	 
DEFSH (VTYSH_OSPFD, ospf_bfd_sess_cc_interval_reset_cmd_vtysh,
		"no ip ospf bfd cc-interval",
		NO_STR
		"IP information\n"
		"OSPF interface commands\n"
		BFD_STR
		"Set BFD session minimum receive interval\n")


DEFSH (VTYSH_OSPFD, ospf_bfd_sess_detect_multiplier_set_cmd_vtysh,
		"ip ospf bfd detect-multiplier <3-10>",
		"IP information\n"
		"OSPF interface commands\n"
		BFD_STR
		"Set BFD session detect multiplier\n"
		"Detect multiplier, default is 3\n")


DEFSH (VTYSH_OSPFD, ospf_bfd_sess_detect_multiplier_reset_cmd_vtysh,
		"no ip ospf bfd detect-multiplier",
		NO_STR
		"IP information\n"
		"OSPF interface commands\n"
		BFD_STR
		"Set BFD session detect multiplier\n")


DEFSH (VTYSH_OSPFD, ospf_bfd_info_show_cmd_vtysh,
		"show ip ospf [<1-255>] bfd ( session | statistics | config )",
		SHOW_STR
		"IP information\n"
		"OSPF information\n"
		"OSPF instance number\n"
		BFD_STR
		"Display session information\n"
		"Display the statistics information of session\n"
		"Display configuration view information\n")


DEFSH (VTYSH_OSPFD, display_ospf_bfd_info_h3c_cmd_vtysh,
		"display ospf [ <1-65535> ] bfd ( session | statistics | config )",
		"Display current system information\n"
		"OSPF information\n"
		"OSPF instance number\n"
		BFD_STR
		"Display session information\n"
		"Display the statistics information of session\n"
		"Display configuration view information\n")


DEFSH (VTYSH_OSPFD, ospf_bfd_h3c_cmd_vtysh,
		 "ospf bfd enable [echo]",
		 "OSPF interface commands\n"
		 "Specify BFD configuration\n"
		 "Enable BFD for OSPF\n"
		 "Echo packet mode\n")


DEFSH (VTYSH_OSPFD, no_ospf_bfd_h3c_cmd_vtysh,
		 "undo ospf bfd enable",
		 "Cancel the current setting\n"
		 "OSPF interface commands\n"
		 "Specify BFD configuration\n"
		 "Enable BFD for OSPF\n")
		 

DEFSH (VTYSH_OSPFD, router_ospf_id_cmd_vtysh, 
	"router-id A.B.C.D", 
	"router-id for the OSPF instance\n"
	"OSPF router-id in IP address format\n")


DEFSH (VTYSH_OSPFD, ospf_network_area_cmd_vtysh, 
	"network A.B.C.D/M ", 
	"Enable routing on an IP network\n"
	"OSPF network prefix\n")


DEFSH(VTYSH_OSPFD, ospf_network_area_h3c_cmd_vtysh,
	"network A.B.C.D A.B.C.D",
	"Enable routing on an IP network\n"
	"OSPF network prefix\n"
	"wildcard-mask\n")


DEFSH (VTYSH_OSPFD, no_ospf_network_area_cmd_vtysh, 
	"no network A.B.C.D/M", 
	"Negate a command or set its defaults\n"
	"Enable routing on an IP network\n"
	"OSPF network prefix\n")


DEFSH(VTYSH_OSPFD, no_ospf_network_area_h3c_cmd_vtysh,
	"undo network A.B.C.D A.B.C.D",
	"Clean current config\n"
	"Enable routing on an IP network\n"
	"OSPF network prefix\n"
	"wildcard-mask\n")


DEFSH (VTYSH_OSPFD, capability_opaque_cmd_vtysh, 
       "capability opaque-lsa", 
       "Enable specific OSPF feature\n"
       "Opaque LSA\n")


DEFSH (VTYSH_OSPFD, capability_opaque_h3c_cmd_vtysh,
		"opaque-capability enable",
		"Enable opaque LSA feature\n"
		"Enable specific OSPF feature\n")


DEFSH (VTYSH_OSPFD, no_capability_opaque_cmd_vtysh, 
       "no capability opaque-lsa", 
       "Negate a command or set its defaults\n"
       "Enable specific OSPF feature\n"
       "Opaque LSA\n")


DEFSH (VTYSH_OSPFD, no_capability_opaque_h3c_cmd_vtysh,
		"undo opaque-capability",
		"Cancel the current setting\n"
		"Enable opaque LSA feature\n")
			

DEFSH (VTYSH_OSPFD, ospf_auto_cost_reference_bandwidth_cmd_vtysh, 
       "reference-bandwidth <1-4294967>", 
       "Use reference bandwidth method to assign OSPF cost\n"
       "The reference bandwidth in terms of Mbits per second\n")


DEFSH (VTYSH_OSPFD, ospf_auto_cost_reference_bandwidth_h3c_cmd_vtysh,
	   "bandwidth-reference <1-4294967>",
	   "Use reference bandwidth method to assign OSPF cost\n"
	   "The reference bandwidth in terms of Mbits per second\n")
		   

DEFSH (VTYSH_OSPFD, no_ospf_auto_cost_reference_bandwidth_cmd_vtysh, 
       "no reference-bandwidth", 
       "Negate a command or set its defaults\n"
       "Use reference bandwidth method to assign OSPF cost\n")


DEFSH (VTYSH_OSPFD, no_ospf_auto_cost_reference_bandwidth_h3c_cmd_vtysh,
	 "undo bandwidth-reference",
	 "Cancel the current setting\n"
	 "Use reference bandwidth method to assign OSPF cost\n")


DEFSH (VTYSH_OSPFD, distance_ospf_cmd_vtysh, 
      "distance {external} <1-255>",
       "Define an administrative distance\n"
       "Distance for external routes\n"
       "Distance value\n")


DEFSH (VTYSH_OSPFD, distance_ospf_h3c_cmd_vtysh,
	 "preference {ase} <1-255> {route-policy STRING}",
	 "Specify the preference for OSPF routes\n"
	 "Preference for ASE routes\n"
	 "Preference value\n"
	 "Specify the routing policy\n"
	 "Name of the routing policy(string length <1-63>)\n")


DEFSH (VTYSH_OSPFD, distance_ospf_h3c_adapt_format1_cmd_vtysh,
	 "preference {ase} route-policy STRING [<1-255>]",
	 "Specify the preference for OSPF routes\n"
	 "Preference for ASE routes\n"
	 "Specify the routing policy\n"
	 "Name of the routing policy(string length <1-63>)\n"
	 "Preference value\n")


DEFSH (VTYSH_OSPFD, no_distance_ospf_cmd_vtysh,
	   "no distance {external}",
       "Negate a command or set its defaults\n"
       "Define an administrative distance\n"
       "External routes\n")


DEFSH (VTYSH_OSPFD, no_distance_ospf_h3c_cmd_vtysh,
	   "undo preference {ase}",
	   "Cancel the current setting\n"
	   "Specify the preference for OSPF routes\n"
	   "Preference for ASE routes\n")
		   

DEFSH (VTYSH_OSPFD, ospf_compatible_rfc1583_cmd_vtysh, 
       "compatible rfc1583", 
       "OSPF compatibility list\n"
       "compatible with RFC 1583\n")


DEFSH (VTYSH_OSPFD, ospf_compatible_rfc1583_h3c_cmd_vtysh,
      "rfc1583 compatible",
      "compatible with RFC 1583\n"
      "OSPF compatibility list\n")


DEFSH (VTYSH_OSPFD, no_ospf_compatible_rfc1583_cmd_vtysh, 
       "no compatible rfc1583", 
       "Negate a command or set its defaults\n"
       "OSPF compatibility list\n"
       "compatible with RFC 1583\n")


DEFSH (VTYSH_OSPFD, no_ospf_compatible_rfc1583_h3c_cmd_vtysh,
	   "undo rfc1583 compatible",
	   "Cancel the current setting\n"
	   "compatible with RFC 1583\n"
	   "OSPF compatibility list\n")
		   

DEFSH (VTYSH_OSPFD, ospf_neighbor_poll_interval_cmd_vtysh, 
       "neighbor A.B.C.D {poll-interval <1-65535>}", 
       "Specify neighbor router\n"
       "Neighbor IP address\n"
       "Dead Neighbor Polling interval\n"
       "Seconds\n")


DEFSH (VTYSH_OSPFD, ospf_neighbor_poll_interval_h3c_cmd_vtysh,
	 "peer A.B.C.D {poll-interval <1-65535>}",
	 "Specify a neighbor router\n"
	 "Neighbor IP address\n"
	 "Dead Neighbor Polling interval\n"
	 "Seconds\n")


DEFSH (VTYSH_OSPFD, ospf_neighbor_cost_h3c_cmd_vtysh,
	"peer A.B.C.D cost <1-65535>",
	"Specify a neighbor router\n"
	"Neighbor IP address\n"
	"OSPF cost for point-to-multipoint neighbor\n"
	"Neighbor cost\n")


DEFSH (VTYSH_OSPFD, ospf_neighbor_dr_priority_h3c_cmd_vtysh,
	 "peer A.B.C.D dr-priority <0-255>",
	 "Specify a neighbor router\n"
	 "Neighbor IP address\n"
	 "OSPF cost for point-to-multipoint neighbor\n"
	 "Neighbor cost\n"
	 "Router priority\n"
	 "Router priority value\n")


DEFSH (VTYSH_OSPFD, no_ospf_neighbor_cmd_vtysh, 
       "no neighbor A.B.C.D", 
       "Negate a command or set its defaults\n"
       "Specify neighbor router\n"
       "Neighbor IP address\n")


DEFSH (VTYSH_OSPFD, no_ospf_neighbor_h3c_cmd_vtysh,
	 "undo peer A.B.C.D",
	 "Cancel the current setting\n"
	 "Specify a neighbor router\n"
	 "Neighbor IP address\n")


DEFSH (VTYSH_OSPFD, ospf_redistribute_direct_cmd_vtysh,
       "redistribute connected route-policy (ethernet|gigabitethernet|xgigabitethernet|loopback) IFNAME",
       "Redistribute information from another routing protocol\n"
       "Connected routes (directly attached subnet or host)\n"
       "route policy\n"
       "interface type :ethernet\n"
       "interface type :gigabitethernet\n"
       "interface type :xgigabitethernet\n"
       "interface type :loopback\n"
       "interface name\n"
       )

DEFSH (VTYSH_OSPFD, no_ospf_redistribute_direct_cmd_vtysh,
       "no redistribute connected route-policy (ethernet|gigabitethernet|xgigabitethernet|loopback) IFNAME",
       "Negate a command or set its defaults\n"
       "Redistribute information from another routing protocol\n"
       "Connected routes (directly attached subnet or host)\n"
       "route policy\n"
       "interface type :ethernet\n"
       "interface type :gigabitethernet\n"
       "interface type :xgigabitethernet\n"
       "interface type :loopback\n"
       "interface name\n"
       )



DEFSH (VTYSH_OSPFD, ospf_redistribute_ri_source_cmd_vtysh, 
       "redistribute " "(rip|isis|ospf)" "<1-255>"
         " {metric <0-16777214>|metric-type (1|2)}", 
       "Redistribute information from another routing protocol\n"
       "Routing Information Protocol (RIP)\n" "Intermediate System to Intermediate System (IS-IS)\n" "Open Shortest Path First (OSPFv2)\n" 
       "instance number\n"
       "Metric for redistributed routes\n"
       "OSPF default metric\n"
       "OSPF exterior metric type for redistributed routes\n"
       "Set OSPF External Type 1 metrics\n"
       "Set OSPF External Type 2 metrics\n")

DEFSH (VTYSH_OSPFD, ospf_redistribute_source_cmd_vtysh, 
       "redistribute " "(connected|static|ebgp|ibgp)"
       " {metric <0-16777214>|metric-type (1|2)}", 
       "Redistribute information from another routing protocol\n"
       "Connected routes (directly attached subnet or host)\n" "Statically configured routes\n" "Border Gateway Protocol (EBGP)\n" "Border Gateway Protocol (IBGP)\n"
       "Metric for redistributed routes\n"
       "OSPF default metric\n"
       "OSPF exterior metric type for redistributed routes\n"
       "Set OSPF External Type 1 metrics\n"
       "Set OSPF External Type 2 metrics\n")


DEFSH (VTYSH_OSPFD, no_ospf_redistribute_source_cmd_vtysh, 
       "no redistribute " "(connected|static|ebgp|ibgp)", 
       "Negate a command or set its defaults\n"
       "Redistribute information from another routing protocol\n"
       "Connected routes (directly attached subnet or host)\n" "Statically configured routes\n" "Border Gateway Protocol (BGP)\n" "Border Gateway Protocol (EBGP)\n")

DEFSH (VTYSH_OSPFD, no_ospf_redistribute_ri_source_cmd_vtysh, 
       "no redistribute " "(rip|isis|ospf)" "<1-255>", 
       "Negate a command or set its defaults\n"
       "Redistribute information from another routing protocol\n"
       "Routing Information Protocol (RIP)\n" "Intermediate System to Intermediate System (IS-IS)\n" "Open Shortest Path First (OSPFv2)\n"
       "instance number\n" )

//ospf import-route command for h3c
DEFSH (VTYSH_OSPFD, ospf_redistribute_source_h3c_cmd_vtysh,
	 "import-route (direct|static) {cost <0-16777214>|type (1|2)|tag <0-4294967295>|route-policy STRING|nssa-only}",
	 "Import routes from other protocols into OSPF\n"
	 "Direct routes\n"
	 "Static routes\n"
	 "Metric for imported route\n"
	 "Value of metric\n"
	 "Type value\n"
	 "Set OSPF External Type 1 metrics\n"
	 "Set OSPF External Type 2 metrics\n"
	 "Specify route tag\n"
	 "Value of tag\n"
	 "Apply the specified routing policy to filter routes\n"
	 "Name of the routing policy(string length:<1-63>)\n"
	 "Limit redistributed routes to NSSA areas\n")



DEFSH (VTYSH_OSPFD, ospf_redistribute_source_h3c_bgp_cmd_vtysh,
	"import-route (bgp) <1-4294967295> allow-ibgp "
	"{cost <0-16777214>|type (1|2)|tag <0-4294967295>|route-policy STRING|nssa-only}",
	"Import routes from other protocols into OSPF\n"
	"BGP routes\n"
	"Autonomous system number\n"
	"Import Border Gateway Protocol (IBGP)\n"
	"Metric for imported route\n"
	"Value of metric\n"
	"Type value\n"
	"Set OSPF External Type 1 metrics\n"
	"Set OSPF External Type 2 metrics\n"
	"Specify route tag\n"
	"Value of tag\n"
	"Apply the specified routing policy to filter routes\n"
	"Name of the routing policy(string length:<1-63>)\n"
	"Limit redistributed routes to NSSA areas\n")


DEFSH (VTYSH_OSPFD, ospf_redistribute_source_h3c_bgp_format2_cmd_vtysh,
	 "import-route (bgp) (<1-4294967295> | allow-ibgp) "
	 "{cost <0-16777214>|type (1|2)|tag <0-4294967295>|route-policy STRING|nssa-only}",
	 "Import routes from other protocols into OSPF\n"
	 "BGP routes\n"
	 "Autonomous system number\n"
	 "Import Border Gateway Protocol (IBGP)\n"
	 "Metric for imported route\n"
	 "Value of metric\n"
	 "Type value\n"
	 "Set OSPF External Type 1 metrics\n"
	 "Set OSPF External Type 2 metrics\n"
	 "Specify route tag\n"
	 "Value of tag\n"
	 "Apply the specified routing policy to filter routes\n"
	 "Name of the routing policy(string length:<1-63>)\n"
	 "Limit redistributed routes to NSSA areas\n")


DEFSH (VTYSH_OSPFD, ospf_redistribute_source_h3c_bgp_format3_cmd_vtysh,
	 "import-route (bgp) "
	 "{cost <0-16777214>|type (1|2)|tag <0-4294967295>|route-policy STRING|nssa-only}",
	 "Import routes from other protocols into OSPF\n"
	 "BGP routes\n"
	 "Metric for imported route\n"
	 "Value of metric\n"
	 "Type value\n"
	 "Set OSPF External Type 1 metrics\n"
	 "Set OSPF External Type 2 metrics\n"
	 "Specify route tag\n"
	 "Value of tag\n"
	 "Apply the specified routing policy to filter routes\n"
	 "Name of the routing policy(string length:<1-63>)\n"
	 "Limit redistributed routes to NSSA areas\n")


DEFSH (VTYSH_OSPFD, ospf_redistribute_ri_source_h3c_cmd_vtysh,
	"import-route (rip|isis|ospf) (<1-65535> | all-processes) "
	"{cost <0-16777214>|type (1|2)|tag <0-4294967295>|route-policy STRING|nssa-only|allow-direct}",
	"Import routes from other protocols into OSPF\n"
	"RIP routes\n"
	"IS-IS routes\n"
	"OSPF routes\n"
	"Process ID\n"
	"Redistribute all processes\n"
	"Metric for imported route\n"
	"Value of metric\n"
	"Type value\n"
	"Set OSPF External Type 1 metrics\n"
	"Set OSPF External Type 2 metrics\n"
	"Specify route tag\n"
	"Value of tag\n"
	"Apply the specified routing policy to filter routes\n"
	"Name of the routing policy(string length:<1-63>)\n"
	"Limit redistributed routes to NSSA areas\n"
	"Include direct routes\n")
		 


DEFSH (VTYSH_OSPFD, ospf_redistribute_ri_source_h3c_format2_cmd_vtysh,
	 "import-route (rip|isis|ospf) "
	 "{cost <0-16777214>|type (1|2)|tag <0-4294967295>|route-policy STRING|nssa-only|allow-direct}",
	 "Import routes from other protocols into OSPF\n"
	 "RIP routes\n"
	 "IS-IS routes\n"
	 "OSPF routes\n"
	 "Metric for imported route\n"
	 "Value of metric\n"
	 "Type value\n"
	 "Set OSPF External Type 1 metrics\n"
	 "Set OSPF External Type 2 metrics\n"
	 "Specify route tag\n"
	 "Value of tag\n"
	 "Apply the specified routing policy to filter routes\n"
	 "Name of the routing policy(string length:<1-63>)\n"
	 "Limit redistributed routes to NSSA areas\n"
	 "Include direct routes\n")

	 
DEFSH (VTYSH_OSPFD, no_ospf_redistribute_source_h3c_cmd_vtysh,
	 "undo import-route (direct|static|bgp)",
	 "Cancel the current setting\n"
	 "Import routes from other protocols into OSPF\n"
	 "Direct routes\n"
	 "Static routes\n"
	 "BGP routes\n")


DEFSH (VTYSH_OSPFD, no_ospf_redistribute_ri_source_h3c_cmd_vtysh,
	 "undo import-route (rip |isis |ospf) (<1-65535>|all-processes)",
	 "Cancel the current setting\n"
	 "Import routes from other protocols into OSPF\n"
	 "RIP routes\n"
	 "IS-IS routes\n"
	 "OSPF routes\n"
	 "Process ID\n"
	 "Redistribute all processes\n")


DEFSH (VTYSH_OSPFD, no_ospf_redistribute_ri_source_format2_h3c_cmd_vtysh,
	"undo import-route (rip |isis |ospf)",
	"Cancel the current setting\n"
	"Import routes from other protocols into OSPF\n"
	"RIP routes\n"
	"IS-IS routes\n"
	"OSPF routes\n")


DEFSH (VTYSH_OSPFD, ospf_asbr_summary_cmd_vtysh,
	   "asbr-summary A.B.C.D/M { not-advertise | cost <0-16777214> }",
       "Summarize routes matching address/mask (AS border routers only)\n"
       "Summary prefix\n"
       "DoNotAdvertise this range\n"
       "User specified metric for this range\n"
       "Advertised metric for this range\n")


DEFSH (VTYSH_OSPFD, ospf_asbr_summary_h3c_cmd_vtysh,
		"asbr-summary A.B.C.D ( <0-32> | A.B.C.D ) { not-advertise | cost <0-16777214> | nssa-only | tag <0-4294967295> }",
		"Summarize routes matching address/mask (AS border routers only)\n"
		"Summary prefix\n"
		"netmask length(0~32)\n"
		"netmask format: A.B.C.D\n"
		"DoNotAdvertise this range\n"
		"User specified metric for this range\n"
		"Advertised metric for this range\n"
		"Forbidden peer device convert Type7-lsa to Type5-lsa\n"
		"Set tag of the aggregation route\n"
		"32-bit tag value\n")


DEFSH (VTYSH_OSPFD, no_ospf_asbr_summary_cmd_vtysh,
	   "no asbr-summary A.B.C.D/M ",
	   "Negate a command or set its defaults\n"
	   "Summarize routes matching address/mask (AS border routers only)\n"
       "Summary prefix\n")


DEFSH (VTYSH_OSPFD, no_ospf_asbr_summary_h3c_cmd_vtysh,
		"undo asbr-summary A.B.C.D ( <0-32> | A.B.C.D )",
		"Cancel the current configuration\n"
		"Summarize routes matching address/mask (AS border routers only)\n"
		"Asbr summary prefix\n"
		"Netmask length(0~32)\n"
		"Netmask format: A.B.C.D\n")
		 

DEFSH (VTYSH_OSPFD, ospf_refresh_interval_cmd_vtysh, 
       "refresh interval <10-1800>", 
       "Adjust refresh parameters\n"
       "Set refresh timer\n"
       "Refresh value in seconds\n")

DEFSH (VTYSH_OSPFD, no_ospf_refresh_interval_cmd_vtysh, 
       "no refresh interval",
       "Negate a command or set its defaults\n"
       "Adjust refresh parameters\n"
       "set refresh default interval\n")

DEFSH (VTYSH_OSPFD, no_set_metric_type_cmd_vtysh, 
       "no set metric-type", 
       "Negate a command or set its defaults\n"
       "Set values in destination routing protocol\n"
       "Type of metric for destination routing protocol\n")

DEFSH (VTYSH_OSPFD, ospf_timers_min_ls_interval_cmd_vtysh, 
       "timers throttle lsa all <0-5000>", 
       "Adjust routing timers\n"
       "Throttling adaptive timer\n"
       "LSA delay between transmissions\n"
       "Negate a command or set its defaults\n"
       "Delay (msec) between sending LSAs\n")

DEFSH (VTYSH_OSPFD, no_ospf_timers_min_ls_interval_cmd_vtysh, 
       "no timers throttle lsa all", 
       "Negate a command or set its defaults\n"
       "Adjust routing timers\n"
       "Throttling adaptive timer\n"
       "LSA delay between transmissions\n")


DEFSH (VTYSH_OSPFD, ospf_timers_min_ls_arrival_cmd_vtysh, 
       "timers lsa arrival <0-1000>", 
       "Adjust routing timers\n"
       "Throttling link state advertisement delays\n"
       "OSPF minimum arrival interval delay\n"
       "Delay (msec) between accepted LSAs\n")


DEFSH (VTYSH_OSPFD, no_ospf_timers_min_ls_arrival_cmd_vtysh, 
       "no timers lsa arrival", 
       "Negate a command or set its defaults\n"
       "Adjust routing timers\n"
       "Throttling link state advertisement delays\n"
       "OSPF minimum arrival interval delay\n")

DEFSH (VTYSH_OSPFD, ospf_log_adjacency_changes_cmd_vtysh, 
       "log-adjacency-changes", 
       "Log changes in adjacency state\n")

DEFSH (VTYSH_OSPFD, no_ospf_log_adjacency_changes_cmd_vtysh, 
       "no log-adjacency-changes", 
       "Negate a command or set its defaults\n"
       "Log changes in adjacency state\n")

DEFSH (VTYSH_OSPFD, ospf_log_adjacency_changes_detail_cmd_vtysh, 
       "log-adjacency-changes detail", 
       "Log changes in adjacency state\n"
       "Log all state changes\n")

DEFSH (VTYSH_OSPFD, no_ospf_log_adjacency_changes_detail_cmd_vtysh, 
       "no log-adjacency-changes detail", 
       "Negate a command or set its defaults\n"
       "Log changes in adjacency state\n"
       "Log all state changes\n")


DEFSH (VTYSH_OSPFD, ospf6_routemap_set_metric_type_cmd_vtysh, 
       "set metric-type (type-1|type-2)", 
       "Set value\n"
       "Type of metric\n"
       "OSPF6 external type 1 metric\n"
       "OSPF6 external type 2 metric\n")


DEFSH (VTYSH_OSPFD, ospf6_routemap_no_set_metric_type_cmd_vtysh, 
       "no set metric-type (type-1|type-2)", 
       "Negate a command or set its defaults\n"
       "Set value\n"
       "Type of metric\n"
       "OSPF6 external type 1 metric\n"
       "OSPF6 external type 2 metric\n")

DEFSH (VTYSH_OSPFD, ospf_default_metric_cmd_vtysh, 
       "default-metric <0-16777214>", 
       "Set metric of redistributed routes\n"
       "Default metric\n")

DEFSH (VTYSH_OSPFD, no_ospf_default_metric_cmd_vtysh, 
       "no default-metric", 
       "Negate a command or set its defaults\n"
       "Set metric of redistributed routes\n")


DEFSH (VTYSH_OSPFD, no_ospf_max_metric_router_lsa_startup_cmd_vtysh, 
       "no max-metric router-lsa on-startup", 
       "Negate a command or set its defaults\n"
       "OSPF maximum / infinite-distance metric\n"
       "Advertise own Router-LSA with infinite distance (stub router)\n"
       "Automatically advertise stub Router-LSA on startup of OSPF\n")

DEFSH (VTYSH_OSPFD, ospf_max_metric_router_lsa_startup_cmd_vtysh, 
       "max-metric router-lsa on-startup <5-86400>", 
       "OSPF maximum / infinite-distance metric\n"
       "Advertise own Router-LSA with infinite distance (stub router)\n"
       "Automatically advertise stub Router-LSA on startup of OSPF\n"
       "Time (seconds) to advertise self as stub-router\n")


DEFSH (VTYSH_OSPFD, ospf_timers_throttle_spf_cmd_vtysh, 
       "timers throttle spf <0-600000> <0-600000> <0-600000>", 
       "Adjust routing timers\n"
       "Throttling adaptive timer\n"
       "OSPF SPF timers\n"
       "Delay (msec) from first change received till SPF calculation\n"
       "Initial hold time (msec) between consecutive SPF calculations\n"
       "Maximum hold time (msec)\n")

DEFSH (VTYSH_OSPFD, no_ospf_timers_throttle_spf_cmd_vtysh, 
       "no timers throttle spf", 
       "Negate a command or set its defaults\n"
       "Adjust routing timers\n"
       "Throttling adaptive timer\n"
       "OSPF SPF timers\n")

DEFSH (VTYSH_OSPFD, ospf_max_metric_router_lsa_admin_cmd_vtysh, 
       "max-metric router-lsa administrative", 
       "OSPF maximum / infinite-distance metric\n"
       "Advertise own Router-LSA with infinite distance (stub router)\n"
       "Administratively applied,  for an indefinite period\n")

DEFSH (VTYSH_OSPFD, no_ospf_max_metric_router_lsa_admin_cmd_vtysh, 
       "no max-metric router-lsa administrative", 
       "Negate a command or set its defaults\n"
       "OSPF maximum / infinite-distance metric\n"
       "Advertise own Router-LSA with infinite distance (stub router)\n"
       "Administratively applied,  for an indefinite period\n")


DEFSH (VTYSH_OSPFD, ospf_max_metric_router_lsa_shutdown_cmd_vtysh, 
       "max-metric router-lsa on-shutdown <5-86400>", 
       "OSPF maximum / infinite-distance metric\n"
       "Advertise own Router-LSA with infinite distance (stub router)\n"
       "Advertise stub-router prior to full shutdown of OSPF\n"
       "Time (seconds) to wait till full shutdown\n")

DEFSH (VTYSH_OSPFD, no_ospf_max_metric_router_lsa_shutdown_cmd_vtysh, 
       "no max-metric router-lsa on-shutdown", 
       "Negate a command or set its defaults\n"
       "OSPF maximum / infinite-distance metric\n"
       "Advertise own Router-LSA with infinite distance (stub router)\n"
       "Advertise stub-router prior to full shutdown of OSPF\n")


DEFSH (VTYSH_OSPFD, mpls_te_cmd_vtysh, 
       "mpls-te", 
       "Configure MPLS-TE parameters\n"
       "Enable the MPLS-TE functionality\n")

DEFSH (VTYSH_OSPFD, no_mpls_te_cmd_vtysh, 
       "no mpls-te", 
       "Negate a command or set its defaults\n"
       "Configure MPLS-TE parameters\n"
       "Disable the MPLS-TE functionality\n")


DEFSH (VTYSH_OSPFD, mpls_te_on_cmd_vtysh, 
       "mpls-te on", 
       "Configure MPLS-TE parameters\n"
       "Enable the MPLS-TE functionality\n")

DEFSH (VTYSH_OSPFD, mpls_te_link_metric_cmd_vtysh, 
       "mpls-te link metric <0-4294967295>", 
       "MPLS-TE specific commands\n"
       "Configure MPLS-TE link parameters\n"
       "Link metric for MPLS-TE purpose\n"
       "Metric\n")

DEFSH (VTYSH_OSPFD, mpls_te_link_maxbw_cmd_vtysh, 
       "mpls-te link max-bw BANDWIDTH", 
       "MPLS-TE specific commands\n"
       "Configure MPLS-TE link parameters\n"
       "Maximum bandwidth that can be used\n"
       "Bytes/second (IEEE floating point format)\n")


DEFSH (VTYSH_OSPFD, mpls_te_link_unrsv_bw_cmd_vtysh, 
       "mpls-te link unrsv-bw <0-7> BANDWIDTH", 
       "MPLS-TE specific commands\n"
       "Configure MPLS-TE link parameters\n"
       "Unreserved bandwidth at each priority level\n"
       "Priority\n"
       "Bytes/second (IEEE floating point format)\n")

DEFSH (VTYSH_OSPFD, mpls_te_link_max_rsv_bw_cmd_vtysh, 
       "mpls-te link max-rsv-bw BANDWIDTH", 
       "MPLS-TE specific commands\n"
       "Configure MPLS-TE link parameters\n"
       "Maximum bandwidth that may be reserved\n"
       "Bytes/second (IEEE floating point format)\n")

DEFSH (VTYSH_OSPFD, mpls_te_link_rsc_clsclr_cmd_vtysh, 
       "mpls-te link rsc-clsclr BITPATTERN", 
       "MPLS-TE specific commands\n"
       "Configure MPLS-TE link parameters\n"
       "Administrative group membership\n"
       "32-bit Hexadecimal value (ex. 0xa1)\n")




/* area  node command*/
DEFSH (VTYSH_OSPFD, ospf_area_nssa_cmd_vtysh,
	"nssa { no-summary }",
	"Configure OSPF area as nssa\n"
	"Do not inject inter-area routes into nssa\n")


DEFSH (VTYSH_OSPFD, ospf_area_stub_h3c_cmd_vtysh,
	"stub { default-route-advertise-always | no-summary }",
	"Configure OSPF area as stub\n"
	"Always originate type 3 default LSA into stub area\n"
	"Do not inject inter-area routes into stub\n")


DEFSH (VTYSH_OSPFD, no_ospf_area_stub_nssa_cmd_vtysh, 
       "no (stub|nssa)",
       "Negate a command or set its defaults\n"
       "Configure OSPF area as stub\n"
       "Configure OSPF area as nssa\n")


DEFSH (VTYSH_OSPFD, no_ospf_area_stub_nssa_h3c_cmd_vtysh,
      "undo (stub|nssa) ",
      "Cancel current configuration\n"
      "Configure OSPF area as stub\n"
      "Configure OSPF area as nssa\n")


DEFSH (VTYSH_OSPFD, ospf_area_name_cmd_vtysh, 
	   "name STRING",
       "Configure OSPF area name\n"
       "OSPF area name string<1-100>\n")


DEFSH (VTYSH_OSPFD, ospf_area_name_h3c_cmd_vtysh,
	   "description STRING",
	   "Specify OSPF area description\n"
	   "OSPF area description (no more than 100 characters)\n")


DEFSH (VTYSH_OSPFD, no_ospf_area_name_cmd_vtysh, 
	   "no name",
	   "Negate a command or set its defaults\n"
       "Configure OSPF area name\n")


DEFSH (VTYSH_OSPFD, no_ospf_area_name_h3c_cmd_vtysh,
		"undo description",
		"Cancel the current configuration\n"
		"Specify OSPF area description\n")


DEFSH (VTYSH_OSPFD, ospf_area_summary_cmd_vtysh,
	   "abr-summary A.B.C.D/M { not-advertise | cost <0-16777214> }",
       "Summarize routes matching address/mask (border routers only)\n"
       "Area summary prefix\n"
       "DoNotAdvertise this range\n"
       "User specified metric for this range\n"
       "Advertised metric for this range\n")


DEFSH (VTYSH_OSPFD, ospf_area_summary_h3c_cmd_vtysh,
	"abr-summary A.B.C.D ( <0-32> | A.B.C.D ) { not-advertise | cost <0-16777214> }",
	"Summarize routes matching address/mask (border routers only)\n"
	"Area summary prefix\n"
	"netmask length(0~32)\n"
	"netmask format: A.B.C.D\n"
	"DoNotAdvertise this range\n"
	"User specified metric for this range\n"
	"Advertised metric for this range\n")


DEFSH (VTYSH_OSPFD, ospf_area_summary_format2_h3c_cmd_vtysh,
	"abr-summary A.B.C.D ( <0-32> | A.B.C.D ) advertise { cost <0-16777214> }",
	"Summarize routes matching address/mask (border routers only)\n"
	"Area summary prefix\n"
	"netmask length(0~32)\n"
	"netmask format: A.B.C.D\n"
	"Advertise this range(default)\n"
	"User specified metric for this range\n"
	"Advertised metric for this range\n")


DEFSH (VTYSH_OSPFD, no_ospf_area_summary_cmd_vtysh,
	"no abr-summary A.B.C.D/M",
	"Negate a command or set its defaults\n"
	"Summarize routes matching address/mask (border routers only)\n"
	"Area summary prefix\n")


DEFSH (VTYSH_OSPFD, no_ospf_area_summary_h3c_cmd_vtysh,
	"undo abr-summary A.B.C.D ( <0-32> | A.B.C.D )",
	"Cancel the current configuration\n"
	"Summarize routes matching address/mask (border routers only)\n"
	"Area summary prefix\n"
	"netmask length(0~32)\n"
	"netmask format: A.B.C.D\n")


DEFSH (VTYSH_OSPFD, ospf_area_default_cost_cmd_vtysh, 
       "default-cost <0-16777214>", 
       "Set the summary-default cost of a NSSA or stub area\n"
       "Stub's advertised default summary cost\n")

DEFSH (VTYSH_OSPFD, no_ospf_area_default_cost_cmd_vtysh, 
       "no default-cost", 
       "Negate a command or set its defaults\n"
       "Set the summary-default cost of a NSSA or stub area\n")


DEFSH (VTYSH_OSPFD,	no_ospf_area_default_cost_h3c_cmd_vtysh,
	"undo default-cost",
	"Cancel current configuration\n"
	"Set the summary-default cost of a NSSA or stub area\n")



DEFSH (VTYSH_OSPFD, ospf_area_authentication_message_digest_cmd_vtysh,
      "authentication-mode (hmac-md5|md5) <1-255> (cipher|plain) PASSWORD",
      "Specify an authentication mode\n"
      "Use HMAC-MD5 algorithm\n"
      "Use MD5 algorithm\n"
	  "Key ID\n"
      "Encryption type (Cryptogram)\n"
	  "Encryption type (Plain text)\n"
	  "The password (length 1~255 bytes)\n")

	 
DEFSH (VTYSH_OSPFD, ospf_area_authentication_cmd_vtysh, 
       "authentication-mode simple (cipher|plain) PASSWORD",
       "Specify an authentication mode\n"
       "Simple authentication mode\n"
	   "Encryption type (Cryptogram)\n"
	   "Encryption type (Plain text)\n"
	   "The password (length 1~8 bytes)\n")


DEFSH (VTYSH_OSPFD, no_ospf_area_authentication_cmd_vtysh, 
       "no authentication-mode",
       "Negate a command or set its defaults\n"
       "Specify an authentication mode\n")


DEFSH (VTYSH_OSPFD, undo_ospf_area_simple_authentication_cmd_vtysh,
		"undo authentication-mode",
		"Cancel current configuration\n"
		"Specify an authentication mode\n")

	 
DEFSH (VTYSH_OSPFD, undo_ospf_area_md5_authentication_cmd_vtysh,
		"undo authentication-mode (hmac-md5|md5) <1-255>",
		"Cancel current configuration\n"
		"Specify an authentication mode\n"
		"Use HMAC-MD5 algorithm\n"
		"Use MD5 algorithm\n"
		"Key ID\n")
		

DEFSH (VTYSH_OSPFD, ospf_area_shortcut_cmd_vtysh, 
       "area shortcut (default|enable|disable)", 
       "OSPF area parameters\n"
       "Configure the area's shortcutting mode\n"
       "Set default shortcutting behavior\n"
       "Enable shortcutting through the area\n"
       "Disable shortcutting through the area\n")


DEFSH (VTYSH_OSPFD, no_ospf_area_shortcut_cmd_vtysh, 
       "no area shortcut (enable|disable)", 
       "Negate a command or set its defaults\n"
       "OSPF area parameters\n"
       "Deconfigure the area's shortcutting mode\n"
       "Deconfigure enabled shortcutting through the area\n"
       "Deconfigure disabled shortcutting through the area\n")

	  

/* show node command*/	  
DEFSH (VTYSH_OSPFD, show_ip_ospf_route_cmd_vtysh, 
	   "show ip ospf [<1-255>] route", 
	   "Show running system information\n"
	   "IP information\n"
	   "OSPF information\n"
	   "OSPF instance number\n"
	   "OSPF routing table\n")


DEFSH (VTYSH_OSPFD, display_ospf_route_h3c_cmd_vtysh,
	 "display ospf [ <1-65535> ] routing",
	 "Display current system information\n"
	 "OSPF information\n"
	 "OSPF instance number\n"
	 "OSPF routing table\n")


DEFSH (VTYSH_OSPFD, show_ip_ospf_lsdb_all_cmd_vtysh, 
       "show ip ospf [<1-255>] lsdb {self-originate-all|max-age}", 
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "OSPF instance number\n"
       "Database summary\n"
       "self originate lsa all\n"
       "LSAs in MaxAge list\n")


DEFSH (VTYSH_OSPFD, display_ospf_lsdb_all_h3c_cmd_vtysh,
	 "display ospf [ <1-65535> ] lsdb { self-originate | brief }",
	 "Display current system information\n"
	 "OSPF information\n"
	 "OSPF instance number\n"
	 "Database summary\n"
	 "Self originate lsa all\n"
	 "Display in brief format\n")


DEFSH (VTYSH_OSPFD, display_ospf_lsdb_all_h3c_format2_cmd_vtysh,
	 "display ospf [ <1-65535> ] lsdb (max-age)",
	 "Display current system information\n"
	 "OSPF information\n"
	 "OSPF instance number\n"
	 "Database summary\n"
	 "LSAs in MaxAge list\n")


DEFSH (VTYSH_OSPFD, show_ip_ospf_lsdb_self_cmd_vtysh, 
       "show ip ospf [<1-255>] lsdb (" "asbr-summary|external|network|router|summary" "|nssa-external" "|opaque-link|opaque-area|opaque-as" ") {self-originate-single}", 
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "OSPF instance number\n"
       "Database summary\n"
       "ASBR summary link states\n" "External link states\n" "Network link states\n" "Router link states\n" "Network summary link states\n" "NSSA external link state\n" "Link local Opaque-LSA\n" "Link area Opaque-LSA\n" "Link AS Opaque-LSA\n"
       "Self-originated link states\n")


DEFSH (VTYSH_OSPFD, display_ospf_lsdb_detail_h3c_cmd_vtysh,
	 "display ospf [ <1-65535> ] lsdb (router | network | summary | asbr | ase | nssa | opaque-link "\
	 "| opaque-area | opaque-as) {self-originate}",
	 "Display current system information\n"
	 "OSPF information\n"
	 "OSPF instance number\n"
	 "Database summary\n"
	 "Router link states\n"
	 "Network link states\n"  
	 "Network summary link states\n"
	 "ASBR summary link states\n"
	 "External link states\n"
	 "NSSA external link state\n"
	 "Link local Opaque-LSA\n"
	 "Link area Opaque-LSA\n"
	 "Link AS Opaque-LSA\n"
	 "Self-originated link states\n")
		 

DEFSH (VTYSH_OSPFD, show_ip_ospf_interface_cmd_vtysh, 
       "show ip ospf [<1-255>] interface", 
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "OSPF instance number\n"
       "Interface information\n")
       

DEFSH (VTYSH_OSPFD, show_ip_ospf_interface_ifname_cmd_vtysh, 
       "show ip ospf [<1-255>] interface (ethernet | gigabitethernet | xgigabitethernet) USP", 
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "OSPF instance number\n"
       "Interface information\n"
       "Ethernet interface type\n"
       "GigabitEthernet interface type\n"
	   "10GigabitEthernet interface type\n"
	   "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")	


DEFSH (VTYSH_OSPFD, display_ospf_interface_h3c_cmd_vtysh,
	 "display ospf [ <1-65535> ] interface { verbose }",
	 "Display current system information\n"
	 "OSPF information\n"
	 "Specify OSPF instance number\n"
	 "Interface information\n"
	 "Detail information about Interfaces\n")
			 

DEFSH (VTYSH_OSPFD, display_ospf_interface_ifname_h3c_cmd_vtysh,
	 "display ospf [ <1-65535> ] interface (ethernet |gigabitethernet |xgigabitethernet ) USP",
	 "Display current system information\n"
	 "OSPF information\n"
	 "Specify OSPF instance number\n"
	 "Interface information\n"
	 "Ethernet interface type\n"
	 "Gigabit Ethernet interface\n"
	 "10Gigabit Ethernet interface\n"
	 "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")
			 

DEFSH(VTYSH_OSPFD, show_ip_ospf_interface_trunk_cmd_vtysh, 
	  "show ip ospf [<1-255>] interface trunk TRUNK",
	  "Show running system information\n"
	  "IP information\n"
	  "OSPF information\n"
	  "OSPF instance number\n"
	  "Interface information\n"
	  "Trunk interface\n"
	  "The port/subport of trunk, format: <1-128>[.<1-4095>]\n")

DEFSH(VTYSH_OSPFD, show_ip_ospf_interface_vlanif_cmd_vtysh, 
	  "show ip ospf [<1-255>] interface vlanif <1-4094>",
	  "Show running system information\n"
	  "IP information\n"
	  "OSPF information\n"
	  "OSPF instance number\n"
	  "Interface information\n"     
	  "Vlan interface\n"
      "VLAN interface number\n")

DEFSH(VTYSH_OSPFD, show_ip_ospf_interface_loopback_cmd_vtysh, 
	  "show ip ospf [<1-255>] interface loopback <0-128>",
	  "Show running system information\n"
	  "IP information\n"
	  "OSPF information\n"
	  "OSPF instance number\n"
	  "Interface information\n" 
	  "LoopBack interface\n"
      "LoopBack interface number\n")


DEFSH(VTYSH_OSPFD, display_ospf_if_trunk_h3c_cmd_vtysh, 
	 "display ospf [ <1-65535> ] interface (trunk) TRUNK",
	 "Display current system information\n"
	 "OSPF information\n"
	 "Specify OSPF instance number\n"
	 "Interface information\n"
	 "Trunk interface\n"
	 "The port/subport of trunk, format: <1-128>[.<1-4095>]\n")


DEFSH(VTYSH_OSPFD, display_ospf_if_vlanif_h3c_cmd_vtysh, 
	 "display ospf [ <1-65535> ] interface (vlanif) <1-4094>",
	 "Display current system information\n"
	 "OSPF information\n"
	 "Specify OSPF instance number\n"
	 "Interface information\n"
	 "Vlan interface\n"
	 "VLAN interface number\n")


DEFSH(VTYSH_OSPFD, display_ospf_if_looplack_h3c_cmd_vtysh, 
	"display ospf [ <1-65535> ] interface (loopback) <0-128>",
	"Display current system information\n"
	"OSPF information\n"
	"Specify OSPF instance number\n"
	"Interface information\n"
	"LoopBack interface\n"
	"LoopBack interface number\n")


DEFSH (VTYSH_OSPFD, show_ip_ospf_neighbor_cmd_vtysh, 
		"show ip ospf [<1-255>] neighbor { detail | all}",
		"Show running system information\n"
		"IP information\n"
		"OSPF information\n"
		"OSPF instance number\n"
		"Neighbor list\n"
		"detail of all neighbors\n"
		"include down status neighbor\n")


DEFSH (VTYSH_OSPFD, show_ip_ospf_neighbor_h3c_cmd_vtysh,
	 "display ospf [<1-65535>] peer { verbose |all }",
	 "Display current system information\n"
	 "OSPF information\n"
	 "OSPF instance number\n"
	 "Specify a neighbor router\n"
	 "Detail information about neighbors\n"
	 "include down status neighbor\n")

DEFSH (VTYSH_OSPFD, show_ip_ospf_neighbor_id_cmd_vtysh, 
       "show ip ospf [<1-255>] neighbor A.B.C.D", 
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "OSPF instance number\n"
       "Neighbor list\n"
       "Neighbor router ID\n")


DEFSH (VTYSH_OSPFD, show_ip_ospf_neighbor_id_h3c_cmd_vtysh,
	 "display ospf [ <1-65535> ] peer A.B.C.D",
	 "Display current system information\n"
	 "OSPF information\n"
	 "OSPF instance number\n"
	 "Specify a neighbor router\n"
	 "Neighbor router ID\n")


DEFSH (VTYSH_OSPFD, show_ip_ospf_neighbor_int_detail_cmd_vtysh, 
       "show ip ospf [<1-255>] neighbor ethernet USP {detail}", 
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "OSPF instance number\n"
   	   "Ethernet interface type\n"
       "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
       "GigabitEthernet interface type\n"
	   "detail of neighbors")

DEFSH (VTYSH_OSPFD, show_ip_ospf_neighbor_int_gigabitethernet_detail_cmd_vtysh, 
       "show ip ospf [<1-255>] neighbor gigabitethernet USP {detail}", 
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "OSPF instance number\n"
   	   "Ethernet interface type\n"
	   "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
	   "detail of neighbors")

DEFSH (VTYSH_OSPFD, show_ip_ospf_neighbor_int_xgigabitethernet_detail_cmd_vtysh, 
       "show ip ospf [<1-255>] neighbor xgigabitethernet USP {detail}", 
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "OSPF instance number\n"
   	   "Ethernet interface type\n"
	   "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
	   "detail of neighbors")


DEFSH (VTYSH_OSPFD, show_ip_ospf_neighbor_by_intface_detail_h3c_cmd_vtysh,
	"display ospf [ <1-65535> ] peer ( ethernet | gigabitethernet | xgigabitethernet ) USP { verbose }",
	"Display current system information\n"
	"OSPF information\n"
	"OSPF instance number\n"
	"Neighbor router information\n"
	"Ethernet interface type\n"
	"Gigabit Ethernet interface\n"
	"10Gigabit Ethernet interface\n"
	"The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
	"Detail information about neighbors\n")



DEFSH (VTYSH_OSPFD, show_ip_ospf_statics_cmd_vtysh, 
       "show ip ospf statistics ", 
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "ospf packet,lsa,route statics\n")

DEFSH (VTYSH_OSPFD, display_ospf_statics_h3c_cmd_vtysh,
	 "display ospf statistics",
	 "Display current system information\n"
	 "OSPF information\n"
	 "ospf packet,lsa,route statics\n")


DEFSH (VTYSH_OSPFD, show_ip_ospf_error_cmd_vtysh,
       "show ip ospf [<1-255>] error",
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "OSPF instance number\n"
       "Error information\n")


DEFSH (VTYSH_OSPFD, display_ospf_statistics_error_h3c_cmd_vtysh,
		"display ospf [<1-65535>] statistics error",
		"Display current system information\n"
		"OSPF information\n"
		"OSPF instance number\n"
		"OSPF info statistics\n"
		"Error information\n")


DEFSH (VTYSH_OSPFD, show_ip_ospf_cmd_vtysh, 
       "show ip ospf instance [<1-255>]", 
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "OSPF instance information\n"
       "OSPF instance number\n")


DEFSH (VTYSH_OSPFD, show_ip_ospf_h3c_cmd_vtysh,
	"display ospf [<1-65535>] verbose",
	"Display current system information\n"
	"OSPF information\n"
	"Specify OSPF instance number\n"
	"Detail information about specify OSPF instance\n")


DEFSH (VTYSH_OSPFD, show_ip_ospf_h3c_format_cmd_vtysh,
	"display ospf [<1-65535>] ",
	"Display current system information\n"
	"OSPF information\n"
	"Specify OSPF instance number\n")


DEFSH (VTYSH_OSPFD, show_ip_ospf_border_routers_cmd_vtysh, 
       "show ip ospf [<1-255>] border-routers", 
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "OSPF instance number\n"
       "show all the ABR's and ASBR's\n")


DEFSH (VTYSH_OSPFD, show_mpls_te_link_cmd_vtysh, 
       "show mpls-te interface [INTERFACE]", 
       "Show running system information\n"
       "MPLS-TE information\n"
       "Interface information\n"
       "Interface name\n")

DEFSH (VTYSH_OSPFD, show_mpls_te_router_cmd_vtysh, 
       "show mpls-te router", 
       "Show running system information\n"
       "MPLS-TE information\n"
       "Router information\n")

#if 0
DEFSH (VTYSH_OSPFD, show_debugging_ospf_cmd_vtysh, 
       "show debugging ospf", 
       "Show running system information\n"
       "Debugging functions (see also 'undebug')\n"
       "OSPF information\n")
#endif

DEFSH (VTYSH_OSPFD, show_debugging_ospf_cmd_vtysh,
       "show ospf debug",
       SHOW_STR
       OSPF_STR
       DEBUG_STR)

DEFSH (VTYSH_OSPFD, show_ip_ospf_refesh_list_cmd_vtysh,
       "show ip ospf [<1-255>] refresh-list",
       "Show running system information\n"
       "IP information\n"
       "OSPF information\n"
       "OSPF instance number\n"
       "refresh-list\n")


DEFSH (VTYSH_OSPFD, display_ospf_refesh_list_h3c_cmd_vtysh,
	 "display ospf [ <1-65535> ] refresh-list",
	 "Display current system information\n"
	 "OSPF information\n"
	 "OSPF instance number\n"
	 "Lsa refresh-list")



/* interface node command */

DEFSH (VTYSH_OSPFD, ip_ospf_network_cmd_vtysh, 
       "ip ospf network (broadcast|nbma|p2mp|p2p)", 
       "IP Information\n"
       "OSPF interface commands\n"
       "Network type\n"
       "Specify OSPF broadcast multi-access network\n"
       "Specify OSPF NBMA network\n"
       "Specify OSPF point-to-multipoint network\n"
       "Specify OSPF point-to-point network\n") 


DEFSH (VTYSH_OSPFD, ip_ospf_network_h3c_cmd_vtysh,
	"ospf network-type (broadcast|nbma)",
	"OSPF interface commands\n"
	"Specify OSPF network type\n"
	"Specify OSPF broadcast multi-access network\n"
	"Specify OSPF NBMA network\n")


DEFSH (VTYSH_OSPFD, ip_ospf_network_p2mp_h3c_cmd_vtysh,
	"ospf network-type (p2mp) [unicast]",
	"OSPF interface commands\n"
	"Specify OSPF network type\n"
	"Specify OSPF point-to-multipoint network\n"
	"Specify unicast point-to-multipoint network\n")


DEFSH (VTYSH_OSPFD, ip_ospf_network_p2p_h3c_cmd_vtysh,
	 "ospf network-type (p2p) [peer-address-check]",
	 "OSPF interface commands\n"
	 "Specify OSPF network type\n"
	 "Specify OSPF point-to-point network\n"
	 "Specify check for nbr on the same network\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_network_h3c_cmd_vtysh,
	 "undo ospf network-type",
	 "Cancel the current setting\n"
	 "OSPF interface commands\n"
	 "Specify OSPF network type\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_network_cmd_vtysh, 
	  "no ip ospf network", 
	  "Negate a command or set its defaults\n"
	  "IP Information\n"
	  "OSPF interface commands\n"
	  "Network type\n")


DEFSH (VTYSH_OSPFD, ip_ospf_cost_u32_cmd_vtysh, 
       "ip ospf cost <1-65535>", 
       "IP Information\n"
       "OSPF interface commands\n"
       "Interface cost\n"
       "Cost")


DEFSH (VTYSH_OSPFD, ip_ospf_cost_u32_h3c_cmd_vtysh,
	 "ospf cost <1-65535>",
	 "OSPF interface commands\n"
	 "Interface cost\n"
	 "Cost value\n")
		 

DEFSH (VTYSH_OSPFD, no_ip_ospf_cost_cmd_vtysh, 
       "no ip ospf cost", 
       "Negate a command or set its defaults\n"
       "IP Information\n"
       "OSPF interface commands\n"
       "Interface cost\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_cost_h3c_cmd_vtysh,
	 "undo ospf cost",
	 "Cancel the current setting\n"
	 "OSPF interface commands\n"
	 "Interface cost\n")


DEFSH (VTYSH_OSPFD, ip_ospf_auth_simple_cmd_vtysh,
	   "ip ospf auth simple PASSWORD",
       "IP Information\n"
       "OSPF interface commands\n"
       "Enable authentication on this interface\n"
       "Use simple authentication on this interface\n"
       "The simple authentication password<1-8>\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_auth_cmd_vtysh,
		"no ip ospf auth",
		"Negate a command or set its defaults\n"
        "IP Information\n"
        "OSPF interface commands\n"
        "Disable authentication on this interface\n")

DEFSH (VTYSH_OSPFD, ip_ospf_auth_md5_cmd_vtysh,
	   "ip ospf auth md5 <1-255> PASSWORD",
       "IP Information\n"
       "OSPF interface commands\n"
       "Enable authentication on this interface\n"
       "Use MD5 algorithm\n"
       "Key ID\n"
       "The OSPF password (key)<1-255>")


DEFSH (VTYSH_OSPFD, no_ip_ospf_auth_md5_cmd_vtysh,
	  "no ip ospf auth md5 <1-255>",
	  "Negate a command or set its defaults\n"
	  "IP Information\n"
      "OSPF interface commands\n"
      "Disable authentication on this interface\n"
      "Disable MD5 algorithm\n"
      "Key ID\n")


DEFSH (VTYSH_OSPFD, ospf_interface_authentication_h3c_cmd_vtysh,
	 "ospf authentication-mode simple (cipher|plain) PASSWORD",
	 "OSPF interface commands\n"
	 "Specify an authentication mode\n"
	 "Simple authentication mode\n"
	 "Encryption type (Cryptogram)\n"
	 "Encryption type (Plain text)\n"
	 "The password (length 1~8 bytes)\n")
		 

DEFSH (VTYSH_OSPFD, ospf_interface_authentication_md5_h3c_cmd_vtysh,
	"ospf authentication-mode (hmac-md5|md5) <1-255> (cipher|plain) PASSWORD",
	"OSPF interface commands\n"
	"Specify an authentication mode\n"
	"Use HMAC-MD5 algorithm\n"
	"Use MD5 algorithm\n"
	"Key ID\n"
	"Encryption type (Cryptogram)\n"
	"Encryption type (Plain text)\n"
	"The password (length 1~255 bytes)\n")
		   

DEFSH (VTYSH_OSPFD, no_ospf_interface_authentication_h3c_cmd_vtysh,
	 "undo ospf authentication-mode",
	 "Cancel the current setting\n"
	 "OSPF interface commands\n"
	 "Specify an authentication mode\n")
		 

DEFSH (VTYSH_OSPFD, undo_ospf_interface_simple_authentication_h3c_cmd_vtysh,
	 "undo ospf authentication-mode simple",
	 "Cancel current configuration\n"
	 "OSPF interface commands\n"
	 "Specify an authentication mode\n"
	 "Simple authentication mode\n")
		 

DEFSH (VTYSH_OSPFD, undo_ospf_interface_md5_authentication_h3c_cmd_vtysh,
	 "undo ospf authentication-mode (hmac-md5|md5) <1-255>",
	 "Cancel current configuration\n"
	 "OSPF interface commands\n"
	 "Specify an authentication mode\n"
	 "Use HMAC-MD5 algorithm\n"
	 "Use MD5 algorithm\n"
	 "Key ID\n")
		 

DEFSH (VTYSH_OSPFD, undo_ospf_interface_keychain_authentication_h3c_cmd_vtysh,
	 "undo ospf authentication-mode keychain",
	 "Cancel current configuration\n"
	 "OSPF interface commands\n"
	 "Specify an authentication mode\n"
	 "Use a keychain for authentication\n")
		 

DEFSH (VTYSH_OSPFD, ip_ospf_transmit_delay_cmd_vtysh, 
       "ip ospf transmit-delay <1-65535>", 
       "IP Information\n"
       "OSPF interface commands\n"
       "Link state transmit delay\n"
       "Seconds\n")

DEFSH (VTYSH_OSPFD, no_ip_ospf_transmit_delay_cmd_vtysh, 
       "no ip ospf transmit-delay", 
       "Negate a command or set its defaults\n"
       "IP Information\n"
       "OSPF interface commands\n"
       "Link state transmit delay\n")


DEFSH (VTYSH_OSPFD, ip_ospf_priority_cmd_vtysh, 
       "ip ospf priority <0-255>", 
       "IP Information\n"
       "OSPF interface commands\n"
       "Router priority\n"
       "Priority\n")


DEFSH (VTYSH_OSPFD, ip_ospf_priority_h3c_cmd_vtysh,
	 "ospf dr-priority <0-255>",
	 "OSPF interface commands\n"
	 "Router priority\n"
	 "Router Priority value\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_priority_h3c_cmd_vtysh,
	 "undo ospf dr-priority",
	 "Cancel the current setting\n"
	 "OSPF interface commands\n"
	 "Router priority\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_priority_cmd_vtysh, 
       "no ip ospf priority", 
       "Negate a command or set its defaults\n"
       "IP Information\n"
       "OSPF interface commands\n"
       "Router priority\n")

DEFSH (VTYSH_OSPFD, ip_ospf_dead_interval_minimal_cmd_vtysh, 
       "ip ospf dead-interval minimal hello-multiplier <1-10>", 
       "IP Information\n"
       "OSPF interface commands\n"
       "Interval after which a neighbor is declared dead\n"
       "Minimal 1s dead-interval with fast sub-second hellos\n"
       "Hello multiplier factor\n"
       "Number of Hellos to send each second\n")


DEFSH (VTYSH_OSPFD, ip_ospf_dead_interval_cmd_vtysh, 
       "ip ospf dead-interval <1-65535>", 
       "IP Information\n"
       "OSPF interface commands\n"
       "Interval after which a neighbor is declared dead\n"
       "Seconds\n")


DEFSH (VTYSH_OSPFD, ip_ospf_dead_interval_h3c_cmd_vtysh,
	 "ospf timer dead <1-2147483647>",
	 "OSPF interface commands\n"
	 "Specify timer interval\n"
	 "Specify the interval after which a neighbor is declared dead\n"
	 "Seconds\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_dead_interval_h3c_cmd_vtysh,
	 "undo ospf timer dead",
	 "Cancel the current setting\n"
	 "OSPF interface commands\n"
	 "Specify timer interval\n"
	 "Interval after which a neighbor is declared dead\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_dead_interval_cmd_vtysh, 
       "no ip ospf dead-interval", 
       "Negate a command or set its defaults\n"
       "IP Information\n"
       "OSPF interface commands\n"
       "Interval after which a neighbor is declared dead\n")

DEFSH (VTYSH_OSPFD, ip_ospf_retransmit_interval_cmd_vtysh, 
       "ip ospf retransmit-interval <1-3600>", 
       "IP Information\n"
       "OSPF interface commands\n"
       "Time between retransmitting lost link state advertisements\n"
       "Seconds\n")


DEFSH (VTYSH_OSPFD, ip_ospf_retransmit_interval_h3c_cmd_vtysh,
	 "ospf timer retransmit <1-3600>",
	 "OSPF interface commands\n"
	 "Specify timer interval\n"
	 "Specify the interval at which the interface retransmits LSAs\n"
	 "Seconds\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_retransmit_interval_h3c_cmd_vtysh,
	 "undo ospf timer retransmit",
	 "Cancel the current setting\n"
	 "OSPF interface commands\n"
	 "Specify timer interval\n"
	 "Specify the interval at which the interface retransmits LSAs\n")
		 

DEFSH (VTYSH_OSPFD, no_ip_ospf_retransmit_interval_cmd_vtysh, 
       "no ip ospf retransmit-interval", 
       "Negate a command or set its defaults\n"
       "IP Information\n"
       "OSPF interface commands\n"
       "Time between retransmitting lost link state advertisements\n")

DEFSH (VTYSH_OSPFD, ip_ospf_hello_interval_cmd_vtysh, 
       "ip ospf hello-interval <1-65535>", 
       "IP Information\n"
       "OSPF interface commands\n"
       "Time between HELLO packets\n"
       "Seconds\n")


DEFSH (VTYSH_OSPFD, ip_ospf_hello_interval_h3c_cmd_vtysh,
	 "ospf timer hello <1-65535>",
	 "OSPF interface commands\n"
	 "Specify timer interval\n"
	 "Specify the interval at which the interface sends hello packets\n"
	 "Seconds\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_hello_interval_h3c_cmd_vtysh,
	 "undo ospf timer hello",
	 "Cancel the current setting\n"
	 "OSPF interface commands\n"
	 "Specify timer interval\n"
	 "Specify the interval at which the interface sends hello packets\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_hello_interval_cmd_vtysh, 
       "no ip ospf hello-interval", 
       "Negate a command or set its defaults\n"
       "IP Information\n"
       "OSPF interface commands\n"
       "Time between HELLO packets\n")

DEFSH (VTYSH_OSPFD, ip_ospf_mtu_ignore_cmd_vtysh, 
      "ip ospf mtu-enable", 
      "IP Information\n"
      "OSPF interface commands\n"
      "Enable mtu mismatch detection\n")


DEFSH (VTYSH_OSPFD, ip_ospf_mtu_ignore_h3c_cmd_vtysh,
	"ospf mtu-enable",
	"OSPF interface commands\n"
	"Enable mtu mismatch detection\n")
	

DEFSH (VTYSH_OSPFD, no_ip_ospf_mtu_ignore_h3c_cmd_vtysh,
	 "undo ospf mtu-enable",
	 "Cancel the current setting\n"
	 "OSPF interface commands\n"
	 "Enable mtu mismatch detection\n")
		 

DEFSH (VTYSH_OSPFD, no_ip_ospf_mtu_ignore_cmd_vtysh, 
      "no ip ospf mtu-enable", 
      "Negate a command or set its defaults\n"
      "IP Information\n"
      "OSPF interface commands\n"
      "Enable mtu mismatch detection\n")


DEFSH (VTYSH_OSPFD, ip_ospf_dead_interval_minimal_addr_cmd_vtysh, 
       "ip ospf dead-interval minimal hello-multiplier <1-10> A.B.C.D", 
       "IP Information\n"
       "OSPF interface commands\n"
       "Interval after which a neighbor is declared dead\n"
       "Minimal 1s dead-interval with fast sub-second hellos\n"
       "Hello multiplier factor\n"
       "Number of Hellos to send each second\n"
       "Address of interface\n")

DEFSH (VTYSH_OSPFD, ip_ospf_area_cmd_vtysh, 
       "ip ospf area (A.B.C.D|<0-4294967295>) [A.B.C.D]", 
       "IP Information\n"
       "OSPF interface commands\n"
       "Enable OSPF on this interface\n"
       "OSPF area ID in IP address format\n"
       "OSPF area ID as a decimal value\n"
       "Address of interface\n")

DEFSH (VTYSH_OSPFD, no_ip_ospf_area_cmd_vtysh, 
       "no ip ospf area [A.B.C.D]", 
       "Negate a command or set its defaults\n"
       "IP Information\n"
       "OSPF interface commands\n"
       "Disable OSPF on this interface\n"
       "Address of interface\n")


DEFSH (VTYSH_OSPFD, ip_ospf_ldp_sync_cmd_vtysh,
       "ip ospf ldp-sync",
       "IP Information\n"
       "OSPF interface commands\n"
       "Enable LDP-OSPF synchronization\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_ldp_sync_cmd_vtysh,
       "no ip ospf ldp-sync",
       "Negate a command or set its defaults\n"
       "IP Information\n"
       "OSPF interface commands\n"
       "Enable LDP-OSPF synchronization\n")


DEFSH (VTYSH_OSPFD, ip_ospf_ldp_hold_down_cmd_vtysh,
       "ip ospf ldp-sync hold-down interval <0-65535>",
       "IP Information\n"
       "OSPF interface commands\n"
       "Specify LDP-OSPF synchronization timer interval\n"
       "HoldDown timer\n"
       "HoldDown interval\n"
       "Second(s)\n")


DEFSH (VTYSH_OSPFD, no_ip_ospf_ldp_hold_down_cmd_vtysh,
       "no ip ospf ldp-sync hold-down interval",
       "Negate a command or set its defaults\n"
       "IP Information\n"
       "OSPF interface commands\n"
       "Specify LDP-OSPF synchronization timer interval\n"
       "HoldDown timer\n"
       "HoldDown interval\n")


DEFSH (VTYSH_OSPFD, ip_ospf_ldp_max_cost_cmd_vtysh,
       "ip ospf ldp-sync hold-max-cost interval <0-65535>",
       "IP Information\n"
       "OSPF interface commands\n"
       "Specify LDP-OSPF synchronization timer interval\n"
       "HoldMaxCost timer\n"
       "HoldMaxCost interval\n"
       "Second(s)\n")


DEFSH (VTYSH_OSPFD, ip_ospf_ldp_max_cost_infinite_cmd_vtysh,
       "ip ospf ldp-sync hold-max-cost interval infinite",
       "IP Information\n"
       "OSPF interface commands\n"
       "Specify LDP-OSPF synchronization timer interval\n"
       "HoldMaxCost timer\n"
       "HoldMaxCost interval\n"
       "Always advertise the maximum cost\n")



DEFSH (VTYSH_OSPFD, no_ip_ospf_ldp_max_cost_cmd_vtysh,
       "no ip ospf ldp-sync hold-max-cost interval",
       "Negate a command or set its defaults\n"
       "IP Information\n"
       "OSPF interface commands\n"
       "Specify LDP-OSPF synchronization timer interval\n"
       "HoldMaxCost timer\n"
       "HoldMaxCost interval\n")



/*New debug node commande*/
DEFSH (VTYSH_OSPFD, debug_ospf_packet_all_cmd_vtysh,
       "debug ospf (enable|disable) packet (hello|dd|ls-request|ls-update|ls-ack|all)",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF packets\n"
       "OSPF Hello\n"
       "OSPF Database Description\n"
       "OSPF Link State Request\n"
       "OSPF Link State Update\n"
       "OSPF Link State Acknowledgment\n"
       "OSPF all packets\n")
       

DEFSH (VTYSH_OSPFD, debug_ospf_packet_send_recv_cmd_vtysh,
       "debug ospf (enable|disable) packet (hello|dd|ls-request|ls-update|ls-ack|all) (send|recv|detail)",
       "Debugging functions\n"
       "OSPF information\n"
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF packets\n"
       "OSPF Hello\n"
       "OSPF Database Description\n"
       "OSPF Link State Request\n"
       "OSPF Link State Update\n"
       "OSPF Link State Acknowledgment\n"
       "OSPF all packets\n"
       "Packet sent\n"
       "Packet received\n"
       "Detail information\n")
       

DEFSH (VTYSH_OSPFD, debug_ospf_packet_send_recv_detail_cmd_vtysh,
       "debug ospf (enable|disable) packet (hello|dd|ls-request|ls-update|ls-ack|all) (send|recv) (detail|)",
       "Debugging functions\n"
       "OSPF information\n"
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF packets\n"
       "OSPF Hello\n"
       "OSPF Database Description\n"
       "OSPF Link State Request\n"
       "OSPF Link State Update\n"
       "OSPF Link State Acknowledgment\n"
       "OSPF all packets\n"
       "Packet sent\n"
       "Packet received\n"
       "Detail Information\n")


DEFSH (VTYSH_OSPFD, debug_ospf_ism_cmd_vtysh,
       "debug ospf (enable|disable) ifsm (events|timers)",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF Interface State Machine\n"
       "ISM Event Information\n"
       "ISM TImer Information\n")
       
       
	 
DEFSH (VTYSH_OSPFD, debug_ospf_nsm_cmd_vtysh,
       "debug ospf (enable|disable) nfsm (events|timers)",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF Neighbor State Machine\n"
       "NSM Event Information\n"
       "NSM Timer Information\n")
       

DEFSH (VTYSH_OSPFD, debug_ospf_lsa_cmd_vtysh,
       "debug ospf (enable|disable) lsa",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF Link State Advertisement\n")

	 
DEFSH (VTYSH_OSPFD, debug_ospf_lsa_sub_cmd_vtysh,
       "debug ospf (enable|disable) lsa (generate|flooding|refresh)",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF Link State Advertisement\n"
       "LSA Generation\n"
       "LSA Flooding\n"
       "LSA Refresh\n")

	
DEFSH (VTYSH_OSPFD, debug_ospf_rib_cmd_vtysh,
		"debug ospf (enable|disable) msg (interface|redistribute)",
		DEBUG_STR
		OSPF_STR
		"Ospf debug enable\n"
		"Ospf debug disable\n"
		"OSPF msg information\n"
		"rib interface\n"
		"rib redistribute\n")

	 
DEFSH (VTYSH_OSPFD, debug_ospf_event_cmd_vtysh,
       "debug ospf (enable|disable) event",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF event information\n")

	 
DEFSH (VTYSH_OSPFD, debug_ospf_nssa_cmd_vtysh,
			"debug ospf (enable|disable) nssa",
			DEBUG_STR
			OSPF_STR
			"Ospf debug enable\n"
			"Ospf debug disable\n"
			"OSPF nssa information\n")
			

DEFSH (VTYSH_OSPFD, debug_ospf_dcn_cmd_vtysh,
		   "debug ospf (enable|disable) dcn",
		   DEBUG_STR
		   OSPF_STR
		   "Ospf debug enable\n"
		   "Ospf debug disable\n"
		   "OSPF dcn information\n")
		   

DEFSH (VTYSH_OSPFD, debug_ospf_bfd_cmd_vtysh,
		 "debug ospf (enable|disable) bfd",
		 DEBUG_STR
		 OSPF_STR
		 "Ospf debug enable\n"
		 "Ospf debug disable\n"
		 "OSPF bfd information\n")
		 

DEFSH (VTYSH_OSPFD, debug_ospf_other_cmd_vtysh,
		 "debug ospf (enable|disable) other ",
		 DEBUG_STR
		 OSPF_STR
		 "Ospf debug enable\n"
		 "Ospf debug disable\n"
		 "Thread output information\n")


/*area virture link*/

#if 0
DEFSH (VTYSH_OSPFD, ospf_area_vlink_cmd_vtysh, 
	"vlink-peer A.B.C.D", 
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n")

DEFSH (VTYSH_OSPFD, no_ospf_area_vlink_cmd_vtysh, 
	"no vlink-peer A.B.C.D", 
	"Negate a command or set its defaults\n"
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n")
			
DEFSH (VTYSH_OSPFD, ospf_area_vlink_authkey_cmd_vtysh, 
	"vlink-peer A.B.C.D "
	"(auth-simple-key|) AUTH_KEY", 
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Authentication simple password (key)\n"
	"dummy string \n"
	"The OSPF password (password length 1~8)")

DEFSH (VTYSH_OSPFD, no_ospf_area_vlink_authkey_cmd_vtysh, 
	"no vlink-peer A.B.C.D "
	"(auth-simple-key|)", 
	"Negate a command or set its defaults\n"
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Authentication simple password (key)\n"
	"dummy string \n"
	"The OSPF password (password length 1~8)")


DEFSH (VTYSH_OSPFD, ospf_area_vlink_param1_cmd_vtysh, 
	"vlink-peer A.B.C.D "
	"(hello-interval|dead-interval) <1-65535>", 
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Time between HELLO packets\n"
	"Interval after which a neighbor is declared dead\n"
	"Seconds\n")

DEFSH (VTYSH_OSPFD, no_ospf_area_vlink_param1_cmd_vtysh, 
	"no vlink-peer A.B.C.D "
	"(hello-interval|dead-interval)", 
	"Negate a command or set its defaults\n"
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Time between HELLO packets\n"
	"Interval after which a neighbor is declared dead\n"
	"Seconds\n")

DEFSH (VTYSH_OSPFD, ospf_area_vlink_param2_cmd_vtysh, 
	"vlink-peer A.B.C.D "
	"(hello-interval|dead-interval) <1-65535> "
	"(hello-interval|dead-interval) <1-65535>", 
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Time between HELLO packets\n"
	"Interval after which a neighbor is declared dead\n"
	"Seconds\n"
	"Time between HELLO packets\n"
	"Interval after which a neighbor is declared dead\n"
	"Seconds\n")

DEFSH (VTYSH_OSPFD, no_ospf_area_vlink_param2_cmd_vtysh, 
	"no vlink-peer A.B.C.D "
	"(hello-interval|dead-interval) "
	"(hello-interval|dead-interval)", 
	"Negate a command or set its defaults\n"
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Time between HELLO packets\n"
	"Interval after which a neighbor is declared dead\n"
	"Seconds\n"
	"Time between HELLO packets\n"
	"Interval after which a neighbor is declared dead\n"
	"Seconds\n")


DEFSH (VTYSH_OSPFD, ospf_area_vlink_retrans_interval_cmd_vtysh, 
	"vlink-peer A.B.C.D "
	"(retransmit-interval) <1-3600>", 
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Time between retransmitting lost link state advertisements\n"
	"Seconds\n")

DEFSH (VTYSH_OSPFD, no_ospf_area_vlink_retrans_interval_cmd_vtysh, 
	"no vlink-peer A.B.C.D "
	"(retransmit-interval)", 
	"Negate a command or set its defaults\n"
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Time between retransmitting lost link state advertisements\n"
	"Seconds\n")

DEFSH (VTYSH_OSPFD, ospf_area_vlink_trans_delay_cmd_vtysh, 
	"vlink-peer A.B.C.D "
	"(transmit-delay) <1-3600>", 
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Link state transmit delay\n"
	"Seconds\n")

DEFSH (VTYSH_OSPFD, no_ospf_area_vlink_trans_delay_cmd_vtysh, 
	"no vlink-peer A.B.C.D "
	"(transmit-delay)", 
	"Negate a command or set its defaults\n"
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Link state transmit delay\n"
	"Seconds\n")


DEFSH (VTYSH_OSPFD, ospf_area_vlink_md5_cmd_vtysh, 
	"vlink-peer A.B.C.D "
	"(message-digest-md5-key|) <1-255> md5 KEY", 
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Message digest authentication (MD5) password (key)\n"
	"dummy string \n"
	"Key ID\n"
	"Use MD5 algorithm\n"
	"The OSPF password (password length 1~256)")

DEFSH (VTYSH_OSPFD, no_ospf_area_vlink_md5_cmd_vtysh, 
	"no vlink-peer A.B.C.D "
	"(message-digest-md5-key|) <1-255>", 
	"Negate a command or set its defaults\n"
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Message digest authentication (MD5) password (key)\n"
	"dummy string \n"
	"Key ID\n"
	"Use MD5 algorithm\n"
	"The OSPF password (password length 1~256)")
#endif


DEFSH(VTYSH_OSPFD, show_ip_ospf_vlink_cmd_vtysh,
	"show ip ospf [<1-255>] vlink",
	SHOW_STR
	"IP information\n"
	"OSPF information\n"
	"OSPF instance number\n"
	"Virtual link information\n")


/*area virture link for h3c */
DEFSH(VTYSH_OSPFD, ospf_area_vlink_h3c_cmd_vtysh,
	 "vlink-peer A.B.C.D {dead <1-32768>|hello <1-8192>|retransmit <1-3600>|trans-delay <1-3600>}",
	 "Configure a virtual link\n"
     "Router ID of the remote ABR\n"
	 "Interval after which a neighbor is declared dead\n"
	 "Seconds\n"
	 "Time between HELLO packets\n"
	 "Seconds\n"
	 "Time between retransmitting lost link state advertisements\n"
	 "Seconds\n"
	 "Link state transmit delay\n"
	 "Seconds\n")


DEFSH(VTYSH_OSPFD, no_ospf_area_vlink_h3c_cmd_vtysh,
	 "undo vlink-peer A.B.C.D {dead|hello|retransmit|trans-delay|simple}",
	 "Cancel current setting\n"
	 "Configure a virtual link\n"
     "Router ID of the remote ABR\n"
	 "Interval after which a neighbor is declared dead\n"
	 "Time between HELLO packets\n"
	 "Time between retransmitting lost link state advertisements\n"
	 "Link state transmit delay\n"
	 "Simple authentication mode\n")


DEFSH(VTYSH_OSPFD, no_ospf_area_vlink_cmd_vtysh,
	"no vlink-peer A.B.C.D {dead|hello|retransmit|trans-delay|simple}",
	"Negate a command or set its defaults\n"
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Interval after which a neighbor is declared dead\n"
	"Time between HELLO packets\n"
	"Time between retransmitting lost link state advertisements\n"
	"Link state transmit delay\n"
	"Simple authentication mode\n")


DEFSH(VTYSH_OSPFD, ospf_area_vlink_auth_md5_h3c_cmd_vtysh,
	 "vlink-peer A.B.C.D (hmac-md5|md5) <1-255> (cipher|plain) PASSWORD",
	 "Configure a virtual link\n"
     "Router ID of the remote ABR\n"
	 "Use HMAC-MD5 algorithm\n"
	 "Use MD5 algorithm\n"
	 "Key ID\n"
	 "Encryption type (Cryptogram)\n"
	 "Encryption type (Plain text)\n"
	 "The password (length 1-255 bytes)\n")

	 
DEFSH(VTYSH_OSPFD, ospf_area_vlink_auth_simple_h3c_cmd_vtysh,
	 "vlink-peer A.B.C.D simple (cipher|plain) PASSWORD",
	 "Configure a virtual link\n"
     "Router ID of the remote ABR\n"
	 "Simple authentication mode\n"
	 "Encryption type (Cryptogram)\n"
	 "Encryption type (Plain text)\n"
	 "The password (length 1-8 bytes)\n")


DEFSH(VTYSH_OSPFD, no_ospf_area_vlink_auth_md5_h3c_cmd_vtysh,
	 "undo vlink-peer A.B.C.D (hmac-md5|md5) <1-255>",
	 "Cancel current setting\n"
	 "Configure a virtual link\n"
     "Router ID of the remote ABR\n"
	 "Use HMAC-MD5 algorithm\n"
	 "Use MD5 algorithm\n"
	 "Key ID\n")


DEFSH(VTYSH_OSPFD, no_ospf_area_vlink_auth_md5_cmd_vtysh,
	"no vlink-peer A.B.C.D (hmac-md5|md5) <1-255>",
	"Negate a command or set its defaults\n"
	"Configure a virtual link\n"
	"Router ID of the remote ABR\n"
	"Use HMAC-MD5 algorithm\n"
	"Use MD5 algorithm\n"
	"Key ID\n")


DEFSH(VTYSH_OSPFD, show_ip_ospf_vlink_h3c_cmd_vtysh,
	"display ospf [<1-65535>] vlink",
	SHOW_STR
	"OSPF information\n"
	"OSPF instance number\n"
	"Virtual link information\n")



/* debug 日志发送到 syslog 使能状态设置 */
DEFSH (VTYSH_OSPFD,ospfd_log_level_ctl_cmd_vtysh,	
        "debug ospfd (enable | disable)",		
	    "Output log of debug level\n""Program name\n""Enable\n""Disable\n")	
 
/* debug 日志发送到 syslog 使能状态显示 */
DEFSH (VTYSH_OSPFD,ospfd_show_log_level_ctl_cmd_vtysh,	"show debug ospfd",		
	SHOW_STR"Output log of debug level\n""Program name\n")	


void
vtysh_init_ospf_cmd (void)
{
    install_node (&ospf_node, NULL);  
    install_node (&area_node, NULL);
	install_node (&dcn_node, NULL);
    vtysh_install_default (OSPF_NODE);
    vtysh_install_default (AREA_NODE);
	vtysh_install_default (OSPF_DCN_NODE);
    install_element_level (OSPF_NODE, &vtysh_exit_ospfd_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (OSPF_NODE, &vtysh_quit_ospfd_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (AREA_NODE, &vtysh_exit_area_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (AREA_NODE, &vtysh_quit_area_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (OSPF_DCN_NODE, &vtysh_exit_dcn_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (OSPF_DCN_NODE, &vtysh_quit_dcn_cmd, VISIT_LEVE, CMD_SYNC);

    /* ospf instance */  
    install_element_level (CONFIG_NODE, &router_ospf_cmd, VISIT_LEVE, CMD_SYNC); 
	install_element_level (CONFIG_NODE, &router_ospf_cmd_h3c, VISIT_LEVE, CMD_SYNC);
    install_element_level (CONFIG_NODE, &no_router_ospf_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &undo_router_ospf_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	 
	/* dcn ospf node*/ 
    install_element_level (CONFIG_NODE, &dcn_ospf_cmd, VISIT_LEVE, CMD_SYNC);    
    install_element_level (CONFIG_NODE, &no_dcn_ospf_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &undo_dcn_ospf_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	//install_element_level (CONFIG_NODE, &dcn_u0_test_cmd_vtysh, VISIT_LEVE, CMD_SYNC);   
	install_element_level (CONFIG_NODE, &dcn_u0_test_show_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	
    install_element_level (CONFIG_NODE, &ospfd_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &ospfd_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	
    /* ospf reset */ 
    install_element_level (CONFIG_NODE, &ospf_reset_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &ospf_reset_instance_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* router id */
    install_element_level (OSPF_NODE, &router_ospf_id_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* vpn id */
    //install_element_level (OSPF_NODE, &ospf_vpn_cmd_vtysh);
    //install_element_level (OSPF_NODE, &no_ospf_vpn_cmd_vtysh);

    /* ospf dcn trap report */
	install_element_level (OSPF_DCN_NODE, &dcn_trap_report_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_DCN_NODE, &no_dcn_trap_report_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_DCN_NODE, &undo_dcn_trap_report_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* ospf dcn ne-ip set*/
	install_element_level (OSPF_DCN_NODE, &dcn_ne_ip_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_DCN_NODE, &dcn_ne_ip_set_same_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_DCN_NODE, &dcn_ne_id_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* auto cost reference*/
    install_element_level (OSPF_NODE, &ospf_auto_cost_reference_bandwidth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_ospf_auto_cost_reference_bandwidth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* auto cost reference for h3c*/
	install_element_level (OSPF_NODE, &ospf_auto_cost_reference_bandwidth_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &no_ospf_auto_cost_reference_bandwidth_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (OSPF_NODE, &ospf_refresh_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &no_ospf_refresh_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* opaque capable*/
    install_element_level (OSPF_NODE, &capability_opaque_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_capability_opaque_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* opaque capable for h3c*/
	install_element_level (OSPF_NODE, &capability_opaque_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_capability_opaque_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    /* rfc1583*/
    install_element_level (OSPF_NODE, &ospf_compatible_rfc1583_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_ospf_compatible_rfc1583_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* rfc1583 for h3c*/
    install_element_level (OSPF_NODE, &ospf_compatible_rfc1583_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_ospf_compatible_rfc1583_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* distance*/
    install_element_level (OSPF_NODE, &distance_ospf_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_distance_ospf_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/* distance for h3c*/
    install_element_level (OSPF_NODE, &distance_ospf_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &distance_ospf_h3c_adapt_format1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_distance_ospf_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* redistribute*/
    install_element_level (OSPF_NODE, &ospf_redistribute_source_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_ospf_redistribute_source_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &ospf_redistribute_ri_source_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_ospf_redistribute_ri_source_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &ospf_redistribute_direct_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_ospf_redistribute_direct_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
	/* redistribute command for h3c*/
	install_element_level (OSPF_NODE, &ospf_redistribute_source_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &ospf_redistribute_source_h3c_bgp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &ospf_redistribute_source_h3c_bgp_format2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &ospf_redistribute_source_h3c_bgp_format3_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &ospf_redistribute_ri_source_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &ospf_redistribute_ri_source_h3c_format2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &no_ospf_redistribute_source_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &no_ospf_redistribute_ri_source_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &no_ospf_redistribute_ri_source_format2_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    /* neighbor */
    install_element_level (OSPF_NODE, &ospf_neighbor_poll_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_ospf_neighbor_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/* neighbor for h3c*/
    install_element_level (OSPF_NODE, &ospf_neighbor_poll_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &ospf_neighbor_cost_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &ospf_neighbor_dr_priority_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &no_ospf_neighbor_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* asbr summary */
    install_element_level (OSPF_NODE, &ospf_asbr_summary_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_ospf_asbr_summary_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (OSPF_NODE, &ospf_asbr_summary_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_ospf_asbr_summary_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/*timers min ls arrival */
    install_element_level (OSPF_NODE, &ospf_timers_min_ls_arrival_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_ospf_timers_min_ls_arrival_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ip ospf network*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	/* ip ospf network for h3c*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_network_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_network_p2mp_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_network_p2p_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_network_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	/* ip ospf network for h3c*/
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_network_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &ip_ospf_network_p2mp_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &ip_ospf_network_p2p_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_network_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ip ospf auth simple*/
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_auth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_auth_simple_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_auth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_auth_simple_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	


    /* ip ospf auth md5*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_auth_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_auth_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* ip ospf auth command for h3c*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ospf_interface_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &ospf_interface_authentication_md5_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &no_ospf_interface_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &undo_ospf_interface_simple_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &undo_ospf_interface_md5_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &undo_ospf_interface_keychain_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &ospf_interface_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ospf_interface_authentication_md5_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_ospf_interface_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &undo_ospf_interface_simple_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &undo_ospf_interface_md5_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &undo_ospf_interface_keychain_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    /* ip ospf cost*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_cost_u32_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_cost_u32_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* ip ospf cost for h3c*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_cost_u32_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_cost_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_cost_u32_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_cost_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    /* ip ospf dead interval*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_dead_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_dead_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_dead_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_dead_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/* ip ospf dead interval for h3c*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_dead_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_dead_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &ip_ospf_dead_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_dead_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


    /* ip ospf hello interval*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/* ip ospf hello interval for h3c*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


    /* ip mtu ignore*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	/* ip mtu ignore for h3c*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_mtu_ignore_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_mtu_ignore_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_mtu_ignore_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_mtu_ignore_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ip ospf priority*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* ip ospf priority for h3c*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_priority_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_priority_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &ip_ospf_priority_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_priority_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ip ospf retransmit interval */
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_retransmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_retransmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_retransmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_retransmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/* ip ospf retransmit interval for h3c*/
	install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_retransmit_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_retransmit_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &ip_ospf_retransmit_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_retransmit_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* bfd for ospf config */
	install_element_level (PHYSICAL_IF_NODE, &ospf_bfd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_ospf_bfd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &ospf_bfd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &no_ospf_bfd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (PHYSICAL_IF_NODE, &ospf_bfd_sess_cc_interval_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &ospf_bfd_sess_cc_interval_reset_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &ospf_bfd_sess_cc_interval_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &ospf_bfd_sess_cc_interval_reset_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &ospf_bfd_sess_detect_multiplier_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &ospf_bfd_sess_detect_multiplier_reset_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &ospf_bfd_sess_detect_multiplier_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &ospf_bfd_sess_detect_multiplier_reset_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* bfd for ospf config for h3c*/
	install_element_level (PHYSICAL_IF_NODE, &ospf_bfd_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_ospf_bfd_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &ospf_bfd_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &no_ospf_bfd_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
#if 0   
    /* ip ospf ldp sync */
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_ldp_sync_cmd_vtysh);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_ldp_sync_cmd_vtysh);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_ldp_sync_cmd_vtysh);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_ldp_sync_cmd_vtysh);
    
    /* ip ospf ldp sync hold down */
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_ldp_hold_down_cmd_vtysh);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_ldp_hold_down_cmd_vtysh);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_ldp_hold_down_cmd_vtysh);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_ldp_hold_down_cmd_vtysh);

    /* ip ospf ldp sync max cost */
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_ldp_max_cost_cmd_vtysh);
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_ldp_max_cost_infinite_cmd_vtysh);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_ldp_max_cost_cmd_vtysh);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_ldp_max_cost_cmd_vtysh);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_ldp_max_cost_infinite_cmd_vtysh);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_ldp_max_cost_cmd_vtysh);
#endif

    /*************************** trunk interface ***********************/
    /* ip ospf network*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	/* ip ospf network for h3c*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_network_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &ip_ospf_network_p2mp_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &ip_ospf_network_p2p_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_ip_ospf_network_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	/* ip ospf network for h3c*/
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_network_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_network_p2mp_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_network_p2p_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_network_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ip ospf auth simple*/
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_auth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &ip_ospf_auth_simple_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_auth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_auth_simple_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


    /* ip ospf auth md5*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_auth_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_auth_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* ip ospf auth command for h3c*/
    install_element_level (TRUNK_IF_NODE, &ospf_interface_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &ospf_interface_authentication_md5_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_ospf_interface_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &undo_ospf_interface_simple_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &undo_ospf_interface_md5_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &undo_ospf_interface_keychain_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (TRUNK_SUBIF_NODE, &ospf_interface_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ospf_interface_authentication_md5_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &no_ospf_interface_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &undo_ospf_interface_simple_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &undo_ospf_interface_md5_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &undo_ospf_interface_keychain_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    /* ip ospf cost*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_cost_u32_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_cost_u32_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/* ip ospf cost for h3c*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_cost_u32_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_cost_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_cost_u32_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_cost_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


    /* ip ospf dead interval*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_dead_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_dead_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_dead_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_dead_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/* ip ospf dead interval for h3c*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_dead_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_dead_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_dead_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_dead_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


    /* ip ospf hello interval*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/* ip ospf hello interval for h3c*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


    /* ip mtu ignore*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* ip mtu ignore for h3c*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_mtu_ignore_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_mtu_ignore_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_mtu_ignore_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_mtu_ignore_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    /* ip ospf priority*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* ip ospf priority for h3c*/
    install_element_level (TRUNK_IF_NODE, &ip_ospf_priority_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_priority_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_priority_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_priority_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ip ospf retransmit interval */
    install_element_level (TRUNK_IF_NODE, &ip_ospf_retransmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_retransmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_retransmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_retransmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* ip ospf retransmit interval for h3c*/
	install_element_level (TRUNK_IF_NODE, &ip_ospf_retransmit_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_ip_ospf_retransmit_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_retransmit_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_retransmit_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* bfd for ospf config */
	install_element_level (TRUNK_IF_NODE, &ospf_bfd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_ospf_bfd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &ospf_bfd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &no_ospf_bfd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (TRUNK_IF_NODE, &ospf_bfd_sess_cc_interval_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &ospf_bfd_sess_cc_interval_reset_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &ospf_bfd_sess_cc_interval_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &ospf_bfd_sess_cc_interval_reset_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (TRUNK_IF_NODE, &ospf_bfd_sess_detect_multiplier_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &ospf_bfd_sess_detect_multiplier_reset_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &ospf_bfd_sess_detect_multiplier_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &ospf_bfd_sess_detect_multiplier_reset_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* bfd for ospf config for h3c*/
	install_element_level (TRUNK_IF_NODE, &ospf_bfd_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_ospf_bfd_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &ospf_bfd_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &no_ospf_bfd_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	 /*************************** vlanif interface ***********************/
    /* ip ospf network*/
    install_element_level (VLANIF_NODE, &ip_ospf_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ip_ospf_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	/* ip ospf network for h3c*/
    install_element_level (VLANIF_NODE, &ip_ospf_network_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &ip_ospf_network_p2mp_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &ip_ospf_network_p2p_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_ip_ospf_network_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    /* ip ospf auth simple*/
    install_element_level (VLANIF_NODE, &no_ip_ospf_auth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &ip_ospf_auth_simple_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ip ospf auth md5*/
    install_element_level (VLANIF_NODE, &ip_ospf_auth_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* ip ospf auth command for h3c*/
    install_element_level (VLANIF_NODE, &ospf_interface_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &ospf_interface_authentication_md5_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_ospf_interface_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &undo_ospf_interface_simple_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &undo_ospf_interface_md5_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &undo_ospf_interface_keychain_authentication_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    /* ip ospf cost*/
    install_element_level (VLANIF_NODE, &ip_ospf_cost_u32_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ip_ospf_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/* ip ospf cost for h3c*/
    install_element_level (VLANIF_NODE, &ip_ospf_cost_u32_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ip_ospf_cost_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    /* ip ospf dead interval*/
    install_element_level (VLANIF_NODE, &ip_ospf_dead_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ip_ospf_dead_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* ip ospf dead interval for h3c*/
    install_element_level (VLANIF_NODE, &ip_ospf_dead_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ip_ospf_dead_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ip ospf hello interval*/
    install_element_level (VLANIF_NODE, &ip_ospf_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ip_ospf_hello_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* ip ospf hello interval for h3c*/
    install_element_level (VLANIF_NODE, &ip_ospf_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ip_ospf_hello_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ip mtu ignore*/
    install_element_level (VLANIF_NODE, &ip_ospf_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ip_ospf_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/* ip mtu ignore for h3c*/
	install_element_level (VLANIF_NODE, &ip_ospf_mtu_ignore_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_ip_ospf_mtu_ignore_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ip ospf priority*/
    install_element_level (VLANIF_NODE, &ip_ospf_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ip_ospf_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/* ip ospf priority for h3c*/
    install_element_level (VLANIF_NODE, &ip_ospf_priority_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ip_ospf_priority_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ip ospf retransmit interval */
    install_element_level (VLANIF_NODE, &ip_ospf_retransmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ip_ospf_retransmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* ip ospf retransmit interval for h3c*/
	install_element_level (VLANIF_NODE, &ip_ospf_retransmit_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_ip_ospf_retransmit_interval_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* bfd for ospf config */
	install_element_level (VLANIF_NODE, &ospf_bfd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_ospf_bfd_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (VLANIF_NODE, &ospf_bfd_sess_cc_interval_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &ospf_bfd_sess_cc_interval_reset_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (VLANIF_NODE, &ospf_bfd_sess_detect_multiplier_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &ospf_bfd_sess_detect_multiplier_reset_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/* bfd for ospf config for h3c*/
	install_element_level (VLANIF_NODE, &ospf_bfd_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_ospf_bfd_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

 #if 0  
    /* ip ospf ldp sync */
    install_element_level (TRUNK_IF_NODE, &ip_ospf_ldp_sync_cmd_vtysh);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_ldp_sync_cmd_vtysh);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_ldp_sync_cmd_vtysh);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_ldp_sync_cmd_vtysh);
    
    /* ip ospf ldp sync hold down */
    install_element_level (TRUNK_IF_NODE, &ip_ospf_ldp_hold_down_cmd_vtysh);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_ldp_hold_down_cmd_vtysh);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_ldp_hold_down_cmd_vtysh);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_ldp_hold_down_cmd_vtysh);

    /* ip ospf ldp sync max cost */
    install_element_level (TRUNK_IF_NODE, &ip_ospf_ldp_max_cost_cmd_vtysh);
    install_element_level (TRUNK_IF_NODE, &ip_ospf_ldp_max_cost_infinite_cmd_vtysh);
    install_element_level (TRUNK_IF_NODE, &no_ip_ospf_ldp_max_cost_cmd_vtysh);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_ldp_max_cost_cmd_vtysh);
    install_element_level (TRUNK_SUBIF_NODE, &ip_ospf_ldp_max_cost_infinite_cmd_vtysh);
    install_element_level (TRUNK_SUBIF_NODE, &no_ip_ospf_ldp_max_cost_cmd_vtysh);
#endif
    

#if 0

    /*ip ospf transmit delay*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_transmit_delay_cmd_vtysh);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_transmit_delay_cmd_vtysh);
    install_element_level (PHYSICAL_IF_NODE, &ip_ospf_transmit_delay_cmd_vtysh);
    install_element_level (PHYSICAL_IF_NODE, &no_ip_ospf_transmit_delay_cmd_vtysh);
    

    /*passive*/
    install_element_level (OSPF_NODE, &no_ospf_passive_interface_addr_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_passive_interface_cmd_vtysh);
    install_element_level (OSPF_NODE, &ospf_passive_interface_addr_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_passive_interface_default_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_passive_interface_default_cmd_vtysh);
    install_element_level (OSPF_NODE, &ospf_passive_interface_default_cmd_vtysh);

#endif

    /*area node*/
    install_element_level (OSPF_NODE, &area_ospf_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (OSPF_NODE, &no_area_ospf_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &area_ospf_address_format_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (OSPF_NODE, &no_area_ospf_address_format_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF_NODE, &no_area_ospf_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    /*area network*/
    install_element_level (AREA_NODE, &ospf_network_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_network_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (AREA_NODE, &ospf_network_area_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_network_area_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    /*area default cost*/
    install_element_level (AREA_NODE, &ospf_area_default_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_default_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (AREA_NODE, &no_ospf_area_default_cost_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* "area stub or nssa no_summury" commands. */
    install_element_level (AREA_NODE, &ospf_area_nssa_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_stub_nssa_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (AREA_NODE, &ospf_area_stub_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (AREA_NODE, &no_ospf_area_stub_nssa_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* "area name" commands. */
    install_element_level (AREA_NODE, &ospf_area_name_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_name_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (AREA_NODE, &ospf_area_name_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_name_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* "area summary" commands. */
    install_element_level (AREA_NODE, &ospf_area_summary_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_summary_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* "area summary" commands for h3c. */
	install_element_level (AREA_NODE, &ospf_area_summary_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (AREA_NODE, &ospf_area_summary_format2_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (AREA_NODE, &no_ospf_area_summary_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /*show ip ospf route or instance command*/
    install_element_level (OSPF_NODE, &show_ip_ospf_route_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_route_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_route_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF_NODE, &show_ip_ospf_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*show ip ospf route command for h3c*/
    install_element_level (OSPF_NODE, &display_ospf_route_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &display_ospf_route_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &display_ospf_route_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	
	/*show ip ospf instance command for h3c*/
	install_element_level (CONFIG_NODE, &show_ip_ospf_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF_NODE, &show_ip_ospf_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_ip_ospf_h3c_format_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF_NODE, &show_ip_ospf_h3c_format_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_h3c_format_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

    /*show ip ospf database command*/
    install_element_level (OSPF_NODE, &show_ip_ospf_lsdb_all_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_lsdb_all_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_lsdb_all_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF_NODE, &show_ip_ospf_lsdb_self_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_lsdb_self_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_lsdb_self_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*show ip ospf database command for h3c*/
	install_element_level (OSPF_NODE, &display_ospf_lsdb_all_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &display_ospf_lsdb_all_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &display_ospf_lsdb_all_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (OSPF_NODE, &display_ospf_lsdb_all_h3c_format2_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &display_ospf_lsdb_all_h3c_format2_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &display_ospf_lsdb_all_h3c_format2_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (OSPF_NODE, &display_ospf_lsdb_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &display_ospf_lsdb_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &display_ospf_lsdb_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

    /*show ip ospf interface command*/
    install_element_level (OSPF_NODE, &show_ip_ospf_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL); 
    install_element_level (OSPF_NODE, &show_ip_ospf_interface_ifname_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_interface_ifname_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_interface_ifname_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL); 
	install_element_level (OSPF_NODE, &show_ip_ospf_interface_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_interface_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_interface_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL); 
	install_element_level (OSPF_NODE, &show_ip_ospf_interface_vlanif_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_interface_vlanif_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_interface_vlanif_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL); 
	install_element_level (OSPF_NODE, &show_ip_ospf_interface_loopback_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_interface_loopback_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_interface_loopback_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL); 

	/*show ip ospf interface command for h3c*/
    install_element_level (OSPF_NODE, &display_ospf_interface_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &display_ospf_interface_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &display_ospf_interface_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL); 
	install_element_level (OSPF_NODE, &display_ospf_interface_ifname_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &display_ospf_interface_ifname_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &display_ospf_interface_ifname_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL); 
	install_element_level (OSPF_NODE, &display_ospf_if_trunk_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &display_ospf_if_trunk_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &display_ospf_if_trunk_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (OSPF_NODE, &display_ospf_if_vlanif_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &display_ospf_if_vlanif_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &display_ospf_if_vlanif_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL); 
	install_element_level (OSPF_NODE, &display_ospf_if_looplack_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &display_ospf_if_looplack_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &display_ospf_if_looplack_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL); 

    /*show ip ospf neighbor command*/
    install_element_level (OSPF_NODE, &show_ip_ospf_neighbor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_neighbor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_neighbor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF_NODE, &show_ip_ospf_neighbor_int_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF_NODE, &show_ip_ospf_neighbor_int_gigabitethernet_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF_NODE, &show_ip_ospf_neighbor_int_xgigabitethernet_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_neighbor_int_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_neighbor_int_gigabitethernet_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_neighbor_int_xgigabitethernet_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_neighbor_int_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_neighbor_int_gigabitethernet_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_neighbor_int_xgigabitethernet_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF_NODE, &show_ip_ospf_neighbor_id_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_neighbor_id_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_neighbor_id_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	 /*show ip ospf neighbor command for h3c*/
    install_element_level (OSPF_NODE, &show_ip_ospf_neighbor_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_neighbor_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_neighbor_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (OSPF_NODE, &show_ip_ospf_neighbor_id_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_neighbor_id_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_neighbor_id_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	
	install_element_level (OSPF_NODE, &show_ip_ospf_neighbor_by_intface_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_neighbor_by_intface_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_neighbor_by_intface_detail_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	
    /*show ip ospf statics command*/
    install_element_level (OSPF_NODE, &show_ip_ospf_statics_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_statics_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_statics_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*show ip ospf statics command for h3c*/
    install_element_level (OSPF_NODE, &display_ospf_statics_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &display_ospf_statics_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &display_ospf_statics_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	
    /*show debugging command*/
    //install_element_level (OSPF_NODE, &show_debugging_ospf_cmd_vtysh);
    install_element_level (CONFIG_NODE, &show_debugging_ospf_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
    //install_element_level (AREA_NODE, &show_debugging_ospf_cmd_vtysh);
    
    /*show refresh list command*/
    install_element_level (OSPF_NODE, &show_ip_ospf_refesh_list_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ip_ospf_refesh_list_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &show_ip_ospf_refesh_list_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*show refresh list command for h3c*/
    install_element_level (OSPF_NODE, &display_ospf_refesh_list_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &display_ospf_refesh_list_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (AREA_NODE, &display_ospf_refesh_list_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*show ospf dcn info command*/
    install_element_level (OSPF_NODE, &show_ip_ospf_dcn_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_ip_ospf_dcn_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (AREA_NODE, &show_ip_ospf_dcn_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF_DCN_NODE, &show_ip_ospf_dcn_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (OSPF_NODE, &display_ip_ospf_dcn_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &display_ip_ospf_dcn_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (AREA_NODE, &display_ip_ospf_dcn_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF_DCN_NODE, &display_ip_ospf_dcn_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	
	/*show ospf-bfd session info command*/
	install_element_level (CONFIG_NODE, &ospf_bfd_info_show_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (OSPF_NODE, &ospf_bfd_info_show_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (AREA_NODE, &ospf_bfd_info_show_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*show ospf-bfd session info command for h3c*/
	install_element_level (CONFIG_NODE, &display_ospf_bfd_info_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (OSPF_NODE, &display_ospf_bfd_info_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (AREA_NODE, &display_ospf_bfd_info_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*show ospf vlink info command*/
	install_element_level (CONFIG_NODE, &show_ip_ospf_vlink_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (OSPF_NODE, &show_ip_ospf_vlink_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (AREA_NODE, &show_ip_ospf_vlink_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*show ospf vlink info command for h3c*/
	install_element_level (CONFIG_NODE, &show_ip_ospf_vlink_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (OSPF_NODE, &show_ip_ospf_vlink_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (AREA_NODE, &show_ip_ospf_vlink_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*show ospf err command*/
    install_element_level (OSPF_NODE, &show_ip_ospf_error_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_ip_ospf_error_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (AREA_NODE, &show_ip_ospf_error_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*show ospf err command for h3c*/
    install_element_level (OSPF_NODE, &display_ospf_statistics_error_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &display_ospf_statistics_error_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (AREA_NODE, &display_ospf_statistics_error_h3c_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

    /*debug ism*/
    install_element_level (CONFIG_NODE, &debug_ospf_ism_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

    /*debug nsm*/
    install_element_level (CONFIG_NODE, &debug_ospf_nsm_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

    /*debug packet*/ 
    install_element_level (CONFIG_NODE, &debug_ospf_packet_all_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf_packet_all_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    install_element_level (CONFIG_NODE, &debug_ospf_packet_send_recv_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf_packet_send_recv_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

    /*debug thread*/
    //install_element_level (CONFIG_NODE, &debug_ospf_thread_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf_thread_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    install_element_level (CONFIG_NODE, &debug_ospf_other_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

    /* debug lsa */
    install_element_level (CONFIG_NODE, &debug_ospf_lsa_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf_lsa_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    install_element_level (CONFIG_NODE, &debug_ospf_lsa_sub_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf_lsa_sub_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

	/*debug rib*/
    //install_element_level (CONFIG_NODE, &debug_ospf_rib_sub_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf_rib_sub_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    install_element_level (CONFIG_NODE, &debug_ospf_rib_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf_rib_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    
    //install_element_level (OSPF_NODE, &debug_ospf_packet_all_cmd_vtysh);
    //install_element_level (OSPF_NODE, &no_debug_ospf_packet_all_cmd_vtysh);
    //install_element_level (OSPF_NODE, &debug_ospf_packet_send_recv_cmd_vtysh);
    //install_element_level (OSPF_NODE, &no_debug_ospf_packet_send_recv_cmd_vtysh);

	/*debug ospf dcn*/
    install_element_level (CONFIG_NODE, &debug_ospf_dcn_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf_dcn_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

	/*debug ospf bfd*/
    install_element_level (CONFIG_NODE, &debug_ospf_bfd_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf_bfd_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

	/*area vlink*/
	/*install_element_level (AREA_NODE, &ospf_area_vlink_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_level (AREA_NODE, &ospf_area_vlink_authkey_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_authkey_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level (AREA_NODE, &ospf_area_vlink_param1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_param1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &ospf_area_vlink_param2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_param2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &ospf_area_vlink_retrans_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_retrans_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &ospf_area_vlink_trans_delay_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_trans_delay_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level (AREA_NODE, &ospf_area_vlink_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	*/
	/*area vlink for h3c */
	install_element_level (AREA_NODE, &ospf_area_vlink_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (AREA_NODE, &no_ospf_area_vlink_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (AREA_NODE, &no_ospf_area_vlink_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (AREA_NODE, &ospf_area_vlink_auth_md5_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (AREA_NODE, &ospf_area_vlink_auth_simple_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (AREA_NODE, &no_ospf_area_vlink_auth_md5_h3c_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (AREA_NODE, &no_ospf_area_vlink_auth_md5_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	/*area authentication*/
    install_element_level (AREA_NODE, &ospf_area_authentication_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &ospf_area_authentication_message_digest_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (AREA_NODE, &no_ospf_area_authentication_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (AREA_NODE, &undo_ospf_area_simple_authentication_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (AREA_NODE, &undo_ospf_area_md5_authentication_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/*debug event*/
	//install_element_level (OSPF_NODE, &debug_ospf_event_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &debug_ospf_event_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
	//install_element_level (CONFIG_NODE, &no_debug_ospf_event_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
	//install_element_level (OSPF_NODE, &no_debug_ospf_event_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

	/*debug nssa*/
    install_element_level (CONFIG_NODE, &debug_ospf_nssa_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

#if 0 
    /*default metric*/
    install_element_level (OSPF_NODE, &ospf_default_metric_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_default_metric_cmd_vtysh);
    install_element_level (OSPF_NODE, &ospf_default_information_originate_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_default_information_originate_cmd_vtysh);

    /*ip ospf area*/
    install_element_level (PHYSICAL_SUBIF_NODE, &ip_ospf_area_cmd_vtysh);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ip_ospf_area_cmd_vtysh);

    /*log adjacency changes*/
    install_element_level (OSPF_NODE, &ospf_log_adjacency_changes_cmd_vtysh);
    install_element_level (OSPF_NODE, &ospf_log_adjacency_changes_detail_cmd_vtysh);

    /*max metric router lsa*/
    install_element_level (OSPF_NODE, &ospf_max_metric_router_lsa_shutdown_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_max_metric_router_lsa_shutdown_cmd_vtysh);
    install_element_level (OSPF_NODE, &ospf_max_metric_router_lsa_admin_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_max_metric_router_lsa_admin_cmd_vtysh);

    /*poll interval*/
    install_element_level (OSPF_NODE, &ospf_neighbor_poll_interval_priority_cmd_vtysh);
    install_element_level (OSPF_NODE, &ospf_neighbor_priority_poll_interval_cmd_vtysh);
    install_element_level (OSPF_NODE, &ospf_neighbor_priority_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_neighbor_priority_cmd_vtysh);
    install_element_level (OSPF_NODE, &ospf_neighbor_poll_interval_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_neighbor_poll_interval_cmd_vtysh);
	
    install_element_level (OSPF_NODE, &ospf_timers_min_ls_interval_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_timers_min_ls_interval_cmd_vtysh);

    install_element_level (OSPF_NODE, &ospf_max_metric_router_lsa_shutdown_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_max_metric_router_lsa_shutdown_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_debug_ospf_zebra_sub_cmd_vtysh);

    /*abr type*/
    install_element_level (OSPF_NODE, &ospf_abr_type_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_abr_type_cmd_vtysh);


    /*ospf timers throttle spf*/
    install_element_level (OSPF_NODE, &ospf_timers_throttle_spf_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_timers_throttle_spf_cmd_vtysh);

    /*area vlink*/
    install_element_level (AREA_NODE, &ospf_area_vlink_authkey_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_authkey_cmd_vtysh);
    install_element_level (AREA_NODE, &ospf_area_vlink_authtype_authkey_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_authtype_authkey_cmd_vtysh);
    install_element_level (AREA_NODE, &ospf_area_vlink_authtype_args_authkey_cmd_vtysh);
    install_element_level (AREA_NODE, &ospf_area_vlink_authtype_args_cmd_vtysh);

    install_element_level (AREA_NODE, &ospf_area_vlink_param1_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_param1_cmd_vtysh);
    install_element_level (AREA_NODE, &ospf_area_vlink_param2_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_param2_cmd_vtysh);
    install_element_level (AREA_NODE, &ospf_area_vlink_param3_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_param3_cmd_vtysh);
    install_element_level (AREA_NODE, &ospf_area_vlink_param4_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_param4_cmd_vtysh);

    install_element_level (AREA_NODE, &ospf_area_vlink_md5_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_md5_cmd_vtysh);
    install_element_level (AREA_NODE, &ospf_area_vlink_authtype_args_md5_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_authtype_md5_cmd_vtysh);
    install_element_level (AREA_NODE, &ospf_area_vlink_authtype_md5_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_authtype_md5_cmd_vtysh);
    install_element_level (AREA_NODE, &ospf_area_vlink_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_vlink_cmd_vtysh);


    /*debug lsa*/
    install_element_level (OSPF_NODE, &no_debug_ospf_lsa_sub_cmd_vtysh);
    install_element_level (CONFIG_NODE, &no_debug_ospf_lsa_sub_cmd_vtysh);

    /*debug nsm*/
    install_element_level (CONFIG_NODE, &no_debug_ospf_nsm_sub_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_debug_ospf_nsm_sub_cmd_vtysh);

    /*debug nssa*/
    install_element_level (CONFIG_NODE, &debug_ospf_nssa_cmd_vtysh);
    install_element_level (OSPF_NODE, &debug_ospf_nssa_cmd_vtysh);
    install_element_level (CONFIG_NODE, &no_debug_ospf_nssa_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_debug_ospf_nssa_cmd_vtysh);

    /*debug event*/
    install_element_level (OSPF_NODE, &debug_ospf_event_cmd_vtysh);
    install_element_level (CONFIG_NODE, &debug_ospf_event_cmd_vtysh);
    install_element_level (CONFIG_NODE, &no_debug_ospf_event_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_debug_ospf_event_cmd_vtysh);

    /*area export list*/
    install_element_level (AREA_NODE, &ospf_area_export_list_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_export_list_cmd_vtysh);

    /*area filter list*/
    install_element_level (AREA_NODE, &ospf_area_filter_list_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_filter_list_cmd_vtysh);

    /*area import list*/
    install_element_level (AREA_NODE, &ospf_area_import_list_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_import_list_cmd_vtysh);

    /*distribute list*/
    install_element_level (OSPF_NODE, &ospf_distribute_list_out_cmd_vtysh);
    install_element_level (OSPF_NODE, &no_ospf_distribute_list_out_cmd_vtysh);

    /*area shortcut*/
    install_element_level (AREA_NODE, &ospf_area_shortcut_cmd_vtysh);
    install_element_level (AREA_NODE, &no_ospf_area_shortcut_cmd_vtysh);

    install_element_level (OSPF_NODE, &show_ip_ospf_border_routers_cmd_vtysh);
    install_element_level (CONFIG_NODE, &show_ip_ospf_border_routers_cmd_vtysh);

#endif  
}


