/**
 * \page cmds_ref_ospfv6 OSPFv6
 * - \subpage modify_log_ospfv6
 * 
 */
 
/**
 * \page modify_log_ospfv6 Modify Log
 * \section ospfv6-v007r004 HiOS-V007R004
 *  -# 
 * \section ospfv6-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"

static struct cmd_node ospf6_node =
{
    OSPF6_NODE,
    "%s(config-ospf6)# "
};

DEFUNSH (VTYSH_OSPF6D,
	 router_ospf6,
	 router_ospf6_cmd,
	 "ospf6 instance <1-255>",
	 "Enable a OSPF6instance\n"
	 "OSPF6instance\n"
	 "Ospf instance ID <1-255>\n")
{
    vty->node = OSPF6_NODE;
    return CMD_SUCCESS;
}

DEFSH (VTYSH_OSPF6D,no_router_ospf6_cmd_vtysh,
	 "no ospf6 instance <1-255>",
	 "Negate a command or set its defaults\n"
	 "Enable a OSPF6instance\n"
	 "OSPF6instance \n"
	 "Ospf instance ID <1-255>\n")

DEFUNSH (VTYSH_OSPF6D,
    vtysh_exit_ospf6d,
    vtysh_exit_ospf6d_cmd,
    "exit",
    "Exit current mode and down to previous mode\n")
{
    return vtysh_exit (vty);
}

ALIAS (vtysh_exit_ospf6d,
       vtysh_quit_ospf6d_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")


DEFSH (VTYSH_OSPF6D, ospf6_router_id_cmd_vtysh,
       "router-id A.B.C.D",
       "Configure OSPF Router-ID\n"
       "specify by IPv4 address notation(e.g. 1.0.0.0)\n")

DEFSH(VTYSH_OSPF6D, distance_ospf6_cmd_vtysh,
      "distance {external} <1-255>",
      "Define an administrative distance\n"
      "Distance for external routes\n"
      "Distance value\n")


DEFSH(VTYSH_OSPF6D, no_distance_ospf6_cmd_vtysh,
      "no distance [external]",
      "Negate a command or set its defaults\n"
      "Define an administrative distance\n"
      "External routes\n")


DEFSH (VTYSH_OSPF6D, area_range_cmd_vtysh,
      "area A.B.C.D abr-summary X:X::X:X/M {not-advertise|cost <0-16777214>}",
      "OSPF area parameters\n"
      "Area ID (as an IPv4 notation)\n"
      "Configured address range\n"
      "Specify IPv6 prefix\n"
      "DoNotAdvertise this range\n"
      "User specified metric for this range\n"
      "Advertised metric for this range\n"
     )

DEFSH (VTYSH_OSPF6D, area_range_advertise_cmd_vtysh,
      "area A.B.C.D abr-summary X:X::X:X/M (advertise|not-advertise)",
      "OSPF area parameters\n"
      "Area ID (as an IPv4 notation)\n"
      "Configured address range\n"
      "Specify IPv6 prefix\n"
     )

DEFSH (VTYSH_OSPF6D, no_area_range_cmd_vtysh,
      "no area A.B.C.D abr-summary X:X::X:X/M",
      "OSPF area parameters\n"
      "Area ID (as an IPv4 notation)\n"
      "Configured address range\n"
      "Specify IPv6 prefix\n"
     )

DEFSH (VTYSH_OSPF6D, ospf6_redistribute_cmd_vtysh,
      "redistribute (connected|static|ebgp|ibgp)"
      " {metric <0-16777214>|type (1|2)}",
      "Redistribute\n"
   	  "Connected routes (directly attached subnet or host)\n" 
   	  "Statically configured routes\n"
   	  "Intermediate System to Intermediate System (IS-IS)\n"
   	  "Border Gateway Protocol (eBGP)\n"
   	  "Border Gateway Protocol (iBGP)\n"
   	  "Metric for redistributed routes\n"
      "OSPF default metric\n"
      "OSPF exterior metric type for redistributed routes\n"
      "Set OSPF External Type 1 metrics\n"
      "Set OSPF External Type 2 metrics\n"
     )

DEFSH (VTYSH_OSPF6D, no_ospf6_redistribute_cmd_vtysh,
      "no redistribute (connected|static|ebgp|ibgp)",
      "Negate a command or set its defaults\n"
      "Redistribute\n"
      "Connected routes (directly attached subnet or host)\n" 
   	  "Statically configured routes\n"
   	  "Intermediate System to Intermediate System (IS-IS)\n"
   	  "Border Gateway Protocol (eBGP)\n"
   	  "Border Gateway Protocol (iBGP)\n"
     )

DEFSH(VTYSH_OSPF6D, ospf6_redistribute_instance_cmd_vtysh,
      "redistribute (ripng |isis |ospf6) <1-255>"
      " {metric <0-16777214>|type (1|2)}",
      "Redistribute\n"
      "Routing Information Protocol next-generation (IPv6) (RIPng)\n" 
      "Intermediate System to Intermediate System (IS-IS)\n"
      "Open Shortest Path First (IPv6) (OSPF6)\n" 
      "instance num\n"
	  "Metric for redistributed routes\n"
	  "OSPF default metric\n"
	  "OSPF exterior metric type for redistributed routes\n"
	  "Set OSPF External Type 1 metrics\n"
	  "Set OSPF External Type 2 metrics\n"
      )

DEFSH(VTYSH_OSPF6D, no_ospf6_redistribute_instance_cmd_vtysh,
      "no redistribute (ripng |isis |ospf6) <1-255>",
      "Negate a command or set its defaults\n"
      "Redistribute\n"
      "Routing Information Protocol next-generation (IPv6) (RIPng)\n" 
      "Intermediate System to Intermediate System (IS-IS)\n"
      "Open Shortest Path First (IPv6) (OSPF6)\n" 
      "instance num\n")

DEFSH (VTYSH_OSPF6D, ipv6_ospf6_cost_cmd_vtysh,
       "ipv6 ospf6 cost <1-65535>", 
       "IPv6 Information\n" 
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Calculate interface cost from bandwidth\n"
       "Outgoing metric of this interface\n"
      )



DEFSH (VTYSH_OSPF6D, no_ipv6_ospf6_cost_cmd_vtysh,
       "no ipv6 ospf6 cost", 
       "Negate a command or set its defaults\n"
       "IPv6 Information\n" 
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Calculate interface cost from bandwidth\n"
      )

DEFSH (VTYSH_OSPF6D, ipv6_ospf6_hellointerval_cmd_vtysh,
       "ipv6 ospf6 hello-interval <1-65535>",
       "IPv6 Information\n" 
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Interval time of Hello packets\n"
       "<1-65535> Seconds\n"
      )

DEFSH (VTYSH_OSPF6D, no_ipv6_ospf6_hellointerval_cmd_vtysh,
       "no ipv6 ospf6 hello-interval",
       "Negate a command or set its defaults\n"
       "IPv6 Information\n" 
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Interval time of Hello packets\n"
      )

      
DEFSH (VTYSH_OSPF6D, ipv6_ospf6_deadinterval_cmd_vtysh,
       "ipv6 ospf6 dead-interval <1-65535>",
       "IPv6 Information\n"        
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Interval time after which a neighbor is declared down\n"
       "<1-65535> Seconds\n"
      )

DEFSH (VTYSH_OSPF6D, no_ipv6_ospf6_deadinterval_cmd_vtysh,
       "no ipv6 ospf6 dead-interval",
       "Negate a command or set its defaults\n"
       "IPv6 Information\n" 
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Interval time after which a neighbor is declared down\n"
      )

DEFSH (VTYSH_OSPF6D, ipv6_ospf6_priority_cmd_vtysh,
       "ipv6 ospf6 priority <0-255>",
       "IPv6 Information\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Router priority\n"
       "Priority value\n"
      )

DEFSH (VTYSH_OSPF6D, no_ipv6_ospf6_priority_cmd_vtysh,
       "no ipv6 ospf6 priority",
       "Negate a command or set its defaults\n"
       "IPv6 Information\n" 
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Router priority\n"
      )


DEFSH (VTYSH_OSPF6D, ipv6_ospf6_instance_cmd_vtysh,
       "ipv6 ospf6 instance-id <0-255>",
       "IPv6 Information\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Instance ID for this interface\n"
       "Instance ID value\n"
      )

DEFSH (VTYSH_OSPF6D, no_ipv6_ospf6_instance_cmd_vtysh,
       "no ipv6 ospf6 instance-id",
       "Negate a command or set its defaults\n"
       "IPv6 Information\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Instance ID for this interface\n"
      )


DEFSH (VTYSH_OSPF6D, ipv6_ospf6_mtu_ignore_cmd_vtysh,
       "ipv6 ospf6 mtu-ignore",
       "IPv6 Information\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Ignore MTU mismatch on this interface\n"
      )

DEFSH (VTYSH_OSPF6D, no_ipv6_ospf6_mtu_ignore_cmd_vtysh,
       "no ipv6 ospf6 mtu-ignore",
       "Negate a command or set its defaults\n"
       "IPv6 Information\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Ignore MTU mismatch on this interface\n"
      )

DEFSH (VTYSH_OSPF6D, ipv6_ospf6_network_cmd_vtysh,
       "ipv6 ospf6 network (broadcast|p2p)",
       "IPv6 Information\n"       
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Network Type\n"
       "Specify OSPFv6 broadcast network\n"
       "Specify OSPF6 point-to-point network\n"
      )

DEFSH (VTYSH_OSPF6D, no_ipv6_ospf6_network_cmd_vtysh,
       "no ipv6 ospf6 network",
       "Negate a command or set its defaults\n"
       "IPv6 Information\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Network Type\n"
      )

DEFSH (VTYSH_OSPF6D, ipv6_ospf6_interface_area_cmd_vtysh,
       "ipv6 ospf6 instance <1-255> area (A.B.C.D |<0-4294967295>)",
       "IPv6 Information\n"
       "Open Shortest Path First (OSPF) for IPv6\n"  
       "OSPF6instance\n"
       "Ospf instance ID <1-255>\n"
       "Specify the OSPF6 area ID\n"
       "OSPF6 area ID in IPv4 address notation value <0-255>\n"
       "OSPF6 area ID as a decimal value <0-255>\n"
      )

DEFSH (VTYSH_OSPF6D, no_ipv6_ospf6_interface_area_cmd_vtysh,
       "no ipv6 ospf6 instance <1-255> area (A.B.C.D |<0-4294967295>)",   
       "Negate a command or set its defaults\n"     
       "IPv6 Information\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "OSPF6instance\n" 
       "Ospf instance ID <1-255>\n"
       "Specify the OSPF6 area ID\n"
       "OSPF6 area ID in IPv4 address notation value <0-255>\n"
       "OSPF6 area ID as a decimal value <0-255>\n"
      )



/* show interface */

DEFSH(VTYSH_OSPF6D, show_ipv6_ospf6_interface_cmd_vtysh,
      "show ipv6 ospf6 [<1-255>] interface",
      "Show running system information\n"
	  "IPv6 Information\n"
	  "Open Shortest Path First (OSPF) for IPv6\n"
	  "Ospf instance ID <1-255>\n"
	  "Interface infomation\n")
      

DEFSH (VTYSH_OSPF6D, show_ipv6_ospf6_interface_ifname_cmd_vtysh,
	"show ipv6 ospf6 [<1-255>] interface (ethernet |gigabitethernet |xgigabitethernet ) USP",
	"Show running system information\n"
	"IPv6 Information\n"
	"Open Shortest Path First (OSPF) for IPv6\n"
	"Ospf instance ID <1-255>\n"
	"Interface infomation\n"
	"Ethernet interface type\n"
    "GigabitEthernet interface type\n"
    "10GigabitEthernet interface type\n"
    "The port/subport of the interface, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")

DEFSH(VTYSH_OSPF6D, show_ipv6_ospf6_interface_trunk_cmd_vtysh, 
	"show ipv6 ospf6 [<1-255>] interface trunk TRUNK",
	"Show running system information\n"
	"IPv6 Information\n"
	"Open Shortest Path First (OSPF) for IPv6\n"
	"Ospf instance ID <1-255>\n"
	"Interface infomation\n"
	"Trunk interface\n"
	"The port/subport of trunk, format: <1-128>[.<1-4095>]\n")

DEFSH(VTYSH_OSPF6D, show_ipv6_ospf6_interface_vlanif_cmd_vtysh, 
	"show ipv6 ospf6 [<1-255>] interface vlanif <1-4094>",
	"Show running system information\n"
	"IPv6 Information\n"
	"Open Shortest Path First (OSPF) for IPv6\n"
	"Ospf instance ID <1-255>\n"
	"Interface infomation\n"    
	"Vlan interface\n"
	"VLAN interface number\n")

DEFSH(VTYSH_OSPF6D, show_ipv6_ospf6_interface_loopback_cmd_vtysh, 
	"show ipv6 ospf6 [<1-255>] interface loopback <0-128>",
	"Show running system information\n"
	"IPv6 Information\n"
	"Open Shortest Path First (OSPF) for IPv6\n"
	"Ospf instance ID <1-255>\n"
	"Interface infomation\n"
	"LoopBack interface\n"
	"LoopBack interface number\n")


DEFSH (VTYSH_OSPF6D, show_ipv6_ospf6_route_cmd_vtysh,
        "show ipv6 ospf6 [<1-255>] route",
        "Show running system information\n"
        "IPv6 Information\n"
        "Open Shortest Path First (OSPF) for IPv6\n"
        "Ospf instance ID <1-255>\n"
        "Routing Table\n"
      )

DEFSH (VTYSH_OSPF6D, show_ipv6_ospf6_database_cmd_vtysh,
        "show ipv6 ospf6 [<1-255>] lsdb",
        "Show running system information\n"
        "IPv6 Information\n"
        "Open Shortest Path First (OSPF) for IPv6\n"
        "Ospf instance ID <1-255>\n"
        "Display Link state database\n"
      )


DEFSH(VTYSH_OSPF6D,
      show_ipv6_ospf6_database_self_originated_cmd_vtysh,
      "show ipv6 ospf6 [<1-255>] lsdb self-originated",
      "Show running system information\n"
      "IPv6 Information\n"
      "Open Shortest Path First (OSPF) for IPv6\n"
      "Ospf instance ID <1-255>\n"
      "Display Self-originated LSAs\n"
     )



DEFSH (VTYSH_OSPF6D, show_ipv6_ospf6_database_type_cmd_vtysh,
       "show ipv6 ospf6 [<1-255>] lsdb "
       "(router|network|inter-prefix|inter-router|as-external|"
       "group-membership|type-7|link|intra-prefix)",
       "Show running system information\n"
       "IPv6 Information\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Ospf instance ID <1-255>\n"
       "Display Link state database\n"
       "Display Router LSAs\n"
       "Display Network LSAs\n"
       "Display Inter-Area-Prefix LSAs\n"
       "Display Inter-Area-Router LSAs\n"
       "Display As-External LSAs\n"
       "Display Group-Membership LSAs\n"
       "Display Type-7 LSAs\n"
       "Display Link LSAs\n"
       "Display Intra-Area-Prefix LSAs\n"
      )

DEFSH(VTYSH_OSPF6D, show_ipv6_ospf6_database_type_self_originated_cmd_vtysh,
      "show ipv6 ospf6 [<1-255>] lsdb "
      "(router|network|inter-prefix|inter-router|as-external|"
      "group-membership|type-7|link|intra-prefix) self-originated",
      "Show running system information\n"
      "IPv6 Information\n"
      "Open Shortest Path First (OSPF) for IPv6\n"
      "Ospf instance ID <1-255>\n"
      "Display Link state database\n"
      "Display Router LSAs\n"
      "Display Network LSAs\n"
      "Display Inter-Area-Prefix LSAs\n"
      "Display Inter-Area-Router LSAs\n"
      "Display As-External LSAs\n"
      "Display Group-Membership LSAs\n"
      "Display Type-7 LSAs\n"
      "Display Link LSAs\n"
      "Display Intra-Area-Prefix LSAs\n"
      "Display Self-originated LSAs\n"
     )


DEFSH (VTYSH_OSPF6D, show_ipv6_ospf6_neighbor_cmd_vtysh,
       "show ipv6 ospf6 [<1-255>] neighbor",
       "Show running system information\n"
       "IPv6 Information\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Ospf instance ID <1-255>\n"
       "Neighbor list\n"
      )

DEFSH (VTYSH_OSPF6D, show_ipv6_ospf6_neighbor_detail_cmd_vtysh,
       "show ipv6 ospf6 [<1-255>] neighbor (detail|drchoice)",
       "Show running system information\n"
       "IPv6 Information\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Ospf instance ID <1-255>\n"
       "Neighbor list\n"
       "Display details\n"
       "Display DR choices\n"
      )

DEFSH (VTYSH_OSPF6D, show_ipv6_ospf6_neighbor_one_cmd_vtysh,
      "show ipv6 ospf6 [<1-255>] neighbor A.B.C.D",
      "Show running system information\n"
      "IPv6 Information\n"
      "Open Shortest Path First (OSPF) for IPv6\n"
      "Ospf instance ID <1-255>\n"
      "Neighbor list\n"
      "Specify Router-ID as IPv4 address notation\n"
     )



DEFSH (VTYSH_OSPF6D, show_ipv6_ospf6_error_cmd_vtysh,
       "show ipv6 ospf6 [<1-255>] error",
       "Show running system information\n"
       "IPv6 Information\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Ospf instance ID <1-255>\n"
       "Error information\n")

DEFSH(VTYSH_OSPF6D, show_ipv6_ospf6_linkstate_cmd_vtysh,
      "show ipv6 ospf6 [<1-255>] linkstate",
   	  "Show running system information\n"
   	  "IPv6 Information\n"
   	  "Open Shortest Path First (OSPF) for IPv6\n"
   	  "Ospf instance ID <1-255>\n"
      "Display linkstate routing table\n"
     )

DEFSH(VTYSH_OSPF6D, show_ipv6_ospf6_linkstate_detail_cmd_vtysh,
      "show ipv6 ospf6 [<1-255>] linkstate detail",
      "Show running system information\n"
      "IPv6 Information\n"
      "Open Shortest Path First (OSPF) for IPv6\n"
      "Ospf instance ID <1-255>\n"
      "Display linkstate routing table\n"
     )

DEFSH(VTYSH_OSPF6D, show_ipv6_ospf6_route_detail_cmd_vtysh,
      "show ipv6 ospf6 [<1-255>] route (X:X::X:X|X:X::X:X/M|detail|summary)",
   	  "Show running system information\n"
   	  "IPv6 Information\n"
   	  "Open Shortest Path First (OSPF) for IPv6\n"
   	  "Ospf instance ID <1-255>\n"
      "Specify IPv6 address\n"
      "Specify IPv6 prefix\n"
      "Detailed information\n"
      "Summary of route table\n"
     )

DEFSH(VTYSH_OSPF6D, show_ipv6_ospf6_cmd_vtysh,
      "show ipv6 ospf6 [<1-255>] instance",
      "Show running system information\n"
      "IPv6 Information\n"
      "Open Shortest Path First (OSPF) for IPv6\n"
      "Ospf instance ID <1-255>\n"
      "OSPF6 Instance\n")

DEFSH(VTYSH_OSPF6D, show_ipv6_ospf6_statics_cmd_vtysh,
      "show ipv6 ospf6 statistics ",
	  "Show running system information\n"
	  "IPv6 Information\n"
	  "Open Shortest Path First (OSPF) for IPv6\n"
      "ospf packet,lsa,route statics\n")

DEFSH(VTYSH_OSPF6D, show_ipv6_ospf6_border_routers_cmd_vtysh,
      "show ipv6 ospf6 [<1-255>] border-routers",
	  "show ipv6 ospf6 statistics "
	  "Show running system information\n"
	  "IPv6 Information\n"
	  "Open Shortest Path First (OSPF) for IPv6\n"
	  "Ospf instance ID <1-255>\n"
      "Display routing table for ABR and ASBR\n"
     )





/* debug */
DEFSH (VTYSH_OSPF6D, debug_ospf6_message_cmd_vtysh,
       "debug ospf6 packet (unknown|hello|dbdesc|lsreq|lsupdate|lsack|all)",
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Debug OSPF6packet\n"
       "Debug Unknown packet\n"
       "Debug Hello packet\n"
       "Debug Database Description packet\n"
       "Debug Link State Request packet\n"
       "Debug Link State Update packet\n"
       "Debug Link State Acknowledgement packet\n"
       "Debug All packet\n"
      )

DEFSH (VTYSH_OSPF6D, debug_ospf6_message_sendrecv_cmd_vtysh,
       "debug ospf6 packet (unknown|hello|dbdesc|lsreq|lsupdate|lsack|all) (send|recv)",
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Debug OSPF6packet\n"
       "Debug Unknown packet\n"
       "Debug Hello packet\n"
       "Debug Database Description packet\n"
       "Debug Link State Request packet\n"
       "Debug Link State Update packet\n"
       "Debug Link State Acknowledgement packet\n"
       "Debug All packet\n"
       "Debug only sending packet\n"
       "Debug only receiving packet\n"
      )

#if 0
DEFSH (VTYSH_OSPF6D, no_debug_ospf6_message_cmd_vtysh,
       "no debug ospf6 packet (unknown|hello|dbdesc|lsreq|lsupdate|lsack|all)",
       "Negate a command or set its defaults\n"
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Debug OSPF6packet\n"
       "Debug Unknown packet\n"
       "Debug Hello packet\n"
       "Debug Database Description packet\n"
       "Debug Link State Request packet\n"
       "Debug Link State Update packet\n"
       "Debug Link State Acknowledgement packet\n"
       "Debug All packet\n"
      )

DEFSH (VTYSH_OSPF6D, no_debug_ospf6_message_sendrecv_cmd_vtysh,
       "no debug ospf6 packet "
       "(unknown|hello|dbdesc|lsreq|lsupdate|lsack|all) (send|recv)",
       "Negate a command or set its defaults\n"
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Debug OSPF6packet\n"
       "Debug Unknown packet\n"
       "Debug Hello packet\n"
       "Debug Database Description packet\n"
       "Debug Link State Request packet\n"
       "Debug Link State Update packet\n"
       "Debug Link State Acknowledgement packet\n"
       "Debug All packet\n"
       "Debug only sending packet\n"
       "Debug only receiving packet\n"
      )
#endif

DEFSH (VTYSH_OSPF6D, debug_ospf6_neighbor_cmd_vtysh,
      "debug ospf6 (enable|disable) neighbor",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 Neighbor\n"
     )

DEFSH (VTYSH_OSPF6D, debug_ospf6_neighbor_detail_cmd_vtysh,
      "debug ospf6 (enable|disable) neighbor (state|event)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 Neighbor\n"
      "Debug OSPF6 Neighbor State Change\n"
      "Debug OSPF6 Neighbor Event\n"
     )

#if 0
DEFSH (VTYSH_OSPF6D, no_debug_ospf6_neighbor_cmd_vtysh,
       "no debug ospf6 neighbor",
       "Negate a command or set its defaults\n"
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Debug OSPF6Neighbor\n"
      )

DEFSH (VTYSH_OSPF6D, no_debug_ospf6_neighbor_detail_cmd_vtysh,
       "no debug ospf6 neighbor (state|event)",
       "Negate a command or set its defaults\n"
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Debug OSPF6Neighbor\n"
       "Debug OSPF6Neighbor State Change\n"
       "Debug OSPF6Neighbor Event\n"
      )
      
#endif

DEFSH (VTYSH_OSPF6D, debug_ospf6_route_cmd_vtysh,
      "debug ospf6 (enable|disable) route (table|intra-area|inter-area|memory)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug route table calculation\n"
      "Debug detail\n"
      "Debug intra-area route calculation\n"
      "Debug inter-area route calculation\n"
      "Debug route memory use\n"
     )

#if 0
DEFSH (VTYSH_OSPF6D, no_debug_ospf6_route_cmd_vtysh,
       "no debug ospf6 route (table|intra-area|inter-area|memory)",
       "Negate a command or set its defaults\n"
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Debug route table calculation\n"
       "Debug intra-area route calculation\n"
       "Debug route memory use\n")

#endif

DEFSH (VTYSH_OSPF6D, debug_ospf6_interface_cmd_vtysh,
      "debug ospf6 (enable|disable) interface",
	  DEBUG_STR
	  OSPF6_STR
	  "Ospf debug enable\n"
      "Ospf debug disable\n"
	  "Debug OSPF6 Interface\n"
	 )

#if 0
DEFSH (VTYSH_OSPF6D, no_debug_ospf6_interface_cmd_vtysh,
       "no debug ospf6 interface",
       "Negate a command or set its defaults\n"
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Debug OSPF6Interface\n"
      )
#endif

DEFSH (VTYSH_OSPF6D, debug_ospf6_flooding_cmd_vtysh,
      "debug ospf6 (enable|disable) flooding",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 flooding function\n"
     )

#if 0
DEFSH (VTYSH_OSPF6D, no_debug_ospf6_flooding_cmd_vtysh,
       "no debug ospf6 flooding",
       "Negate a command or set its defaults\n"
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Debug OSPF6flooding function\n"
      )
#endif

DEFSH (VTYSH_OSPF6D, debug_ospf6_spf_process_cmd_vtysh,
      "debug ospf6 (enable|disable) spf (process | time | lsdb)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug SPF Calculation\n"
      "Debug Detailed SPF Process\n"
      "Measure time taken by SPF Calculation\n"
      "Log number of LSAs at SPF Calculation time\n"
     )

#if 0
DEFSH (VTYSH_OSPF6D, debug_ospf6_spf_time_cmd_vtysh,
       "debug ospf6 spf time",
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Debug SPF Calculation\n"
       "Measure time taken by SPF Calculation\n"
      )


DEFSH (VTYSH_OSPF6D, debug_ospf6_spf_database_cmd_vtysh,
       "debug ospf6 spf lsdb",
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Debug SPF Calculation\n"
       "Log number of LSAs at SPF Calculation time\n"
      )


DEFSH (VTYSH_OSPF6D, no_debug_ospf6_spf_process_cmd_vtysh,
       "no debug ospf6 spf process",
       "Negate a command or set its defaults\n"
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Quit Debugging SPF Calculation\n"
       "Quit Debugging Detailed SPF Process\n"
      )


DEFSH (VTYSH_OSPF6D, no_debug_ospf6_spf_time_cmd_vtysh,
       "no debug ospf6 spf time",
       "Negate a command or set its defaults\n"
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Quit Debugging SPF Calculation\n"
       "Quit Measuring time taken by SPF Calculation\n"
      )


DEFSH (VTYSH_OSPF6D, no_debug_ospf6_spf_database_cmd_vtysh,
       "no debug ospf6 spf lsdb",
       "Negate a command or set its defaults\n"
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "Debug SPF Calculation\n"
       "Quit Logging number of LSAs at SPF Calculation time\n"
      )

#endif

DEFSH (VTYSH_OSPF6D, debug_ospf6_route_sendrecv_cmd_vtysh,
      "debug ospf6 (enable|disable) msg (send|recv|all)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "message between route and ospf6"
      "Debug connection between route\n"
      "Debug Sending route\n"
      "Debug Receiving route\n"
      "Debug connection between route send and recv\n"
     )
      
#if 0
DEFSH (VTYSH_OSPF6D, no_debug_ospf6_route_sendrecv_cmd_vtysh,
       "no debug ospf6 msg route (send|recv|all)",
       "Negate a command or set its defaults\n"
       "Debugging functions (see also 'undebug')\n"
       "Open Shortest Path First (OSPF) for IPv6\n"
       "message between route and ospf6\n"
       "Debug connection between route\n"
       "Debug Sending route\n"
       "Debug Receiving route\n"
       "Debug connection between route send and recv\n"
      )
#endif

DEFSH(VTYSH_OSPF6D, debug_ospf6_lsa_hex_cmd_vtysh,
      "debug ospf6 (enable|disable) lsa (router|network|inter-prefix|inter-router|as-ext|grp-mbr|type7|link|intra-prefix|unknown)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug Link State Advertisements (LSAs)\n"
      "Specify LS type is router-lsa\n"
      "Specify LS type is network-lsa\n"
      "Specify LS type is inter-prefix-lsa\n"
      "Specify LS type is inter-router-lsa\n"
      "Specify LS type is as-ext-lsa\n"
      "Specify LS type is grp-mbr-lsa\n"
      "Specify LS type is type7-lsa\n"
      "Specify LS type is link-lsa\n"
      "Specify LS type is intra-prefix-lsa\n"
      "Specify LS type is unknown-lsa\n"
     )

DEFSH(VTYSH_OSPF6D,
      debug_ospf6_lsa_hex_detail_cmd_vtysh,
      "debug ospf6 (enable|disable) lsa (router|network|inter-prefix|inter-router|as-ext|grp-mbr|type7|link|intra-prefix|unknown) (originate|examine|flooding)",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug Link State Advertisements (LSAs)\n"
      "Specify LS type is router-lsa\n"
      "Specify LS type is network-lsa\n"
      "Specify LS type is inter-prefix-lsa\n"
      "Specify LS type is inter-router-lsa\n"
      "Specify LS type is as-ext-lsa\n"
      "Specify LS type is grp-mbr-lsa\n"
      "Specify LS type is type7-lsa\n"
      "Specify LS type is link-lsa\n"
      "Specify LS type is intra-prefix-lsa\n"
      "Specify LS type is unknown-lsa\n"
      "Debug option is originate\n"
      "Debug option is examine\n"
      "Debug option is flooding\n"
     )
     
#if 0
DEFSH(VTYSH_OSPF6D,
      no_debug_ospf6_lsa_hex_cmd_vtysh,
      "no debug ospf6 lsa (router|network|inter-prefix|inter-router|as-ext|grp-mbr|type7|link|intra-prefix|unknown)", 
	  "Negate a command or set its defaults\n" 
	  "Debugging functions (see also 'undebug')\n"
	  "Open Shortest Path First (OSPF) for IPv6\n"
      "Debug Link State Advertisements (LSAs)\n"
      "Specify LS type as Hexadecimal\n"
     )

DEFSH(VTYSH_OSPF6D,
      no_debug_ospf6_lsa_hex_detail_cmd_vtysh,
      "no debug ospf6 lsa (router|network|inter-prefix|inter-router|as-ext|grp-mbr|type7|link|intra-prefix) (originate|examine|flooding)",
	  "Negate a command or set its defaults\n"
	  "Debugging functions (see also 'undebug')\n"
	  "Open Shortest Path First (OSPF) for IPv6\n"
      "Debug Link State Advertisements (LSAs)\n"
      "Specify LS type as Hexadecimal\n"
     )
#endif

DEFSH(VTYSH_OSPF6D, debug_ospf6_abr_cmd_vtysh,
      "debug ospf6 (enable|disable) abr",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 ABR function\n"
     )

#if 0
DEFSH(VTYSH_OSPF6D, no_debug_ospf6_abr_cmd_vtysh,
      "no debug ospf6 abr",
	  "Negate a command or set its defaults\n"
	  "Debugging functions (see also 'undebug')\n"
	  "Open Shortest Path First (OSPF) for IPv6\n"
      "Debug OSPF6ABR function\n"
     )
#endif

DEFSH(VTYSH_OSPF6D, debug_ospf6_brouter_cmd_vtysh,
      "debug ospf6 (enable|disable) border-routers",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug border router\n"
     )

#if 0
DEFSH(VTYSH_OSPF6D, no_debug_ospf6_brouter_cmd_vtysh,
      "no debug ospf6 border-routers",
  	  "Negate a command or set its defaults\n"
  	  "Debugging functions (see also 'undebug')\n"
  	  "Open Shortest Path First (OSPF) for IPv6\n"
      "Debug border router\n"
     )
#endif


DEFSH(VTYSH_OSPF6D, debug_ospf6_brouter_router_cmd_vtysh,
      "debug ospf6 (enable|disable) border-routers (router-id|area-id) {A.B.C.D}",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug border router\n"
      "Debug specific border router\n"
      "Debug border routers in specific Area\n"
      "Specify border-router's router-id or Area-ID\n"
     )

#if 0
DEFSH(VTYSH_OSPF6D, no_debug_ospf6_brouter_router_cmd_vtysh,
      "no debug ospf6 border-routers router-id",
      "Negate a command or set its defaults\n"
	  "Debugging functions (see also 'undebug')\n"
	  "Open Shortest Path First (OSPF) for IPv6\n"
      "Debug border router\n"
      "Debug specific border router\n"
     )



DEFSH(VTYSH_OSPF6D, debug_ospf6_brouter_area_cmd_vtysh,
      "debug ospf6 border-routers area-id A.B.C.D",
	  "Negate a command or set its defaults\n"
	  "Debugging functions (see also 'undebug')\n"
	  "Open Shortest Path First (OSPF) for IPv6\n"
      "Debug border router\n"
      "Debug border routers in specific Area\n"
      "Specify Area-ID\n"
     )

DEFSH(VTYSH_OSPF6D, no_debug_ospf6_brouter_area_cmd_vtysh,
      "no debug ospf6 border-routers area-id",
	  "Negate a command or set its defaults\n"
	  "Debugging functions (see also 'undebug')\n"
	  "Open Shortest Path First (OSPF) for IPv6\n"
      "Debug border router\n"
      "Debug border routers in specific Area\n"
     )

#endif

DEFSH(VTYSH_OSPF6D, debug_ospf6_asbr_cmd_vtysh,
      "debug ospf6 (enable|disable) asbr",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 ASBR function\n"
     )

#if 0
DEFSH(VTYSH_OSPF6D, no_debug_ospf6_asbr_cmd_vtysh,
      "no debug ospf6 asbr",
	  "Negate a command or set its defaults\n"
	  "Debugging functions (see also 'undebug')\n"
	  "Open Shortest Path First (OSPF) for IPv6\n"
      "Debug OSPF6ASBR function\n"
     )
#endif


DEFSH(VTYSH_OSPF6D, debug_ospf6_other_cmd_vtysh,
      "debug ospf6 (enable|disable) other",
      DEBUG_STR
      OSPF6_STR
      "Ospf debug enable\n"
      "Ospf debug disable\n"
      "Debug OSPF6 no classified, scattered debugging information.\n"
     )



DEFSH (VTYSH_OSPF6D,ospf6d_log_level_ctl_cmd_vtysh,	
        "debug ospf6d (enable | disable)",		
	    "Output log of debug level\n""Program name\n""Enable\n""Disable\n")	
 
DEFSH (VTYSH_OSPF6D,ospf6d_show_log_level_ctl_cmd_vtysh,	"show debug ospf6d",		
	SHOW_STR"Output log of debug level\n""Program name\n")	


void
vtysh_init_ospf6_cmd (void)
{
    install_node (&ospf6_node, NULL);  
    vtysh_install_default (OSPF6_NODE);
    install_element_level (OSPF6_NODE, &vtysh_exit_ospf6d_cmd, VISIT_LEVE, CMD_SYNC);

    /* ospf instance */  
    install_element_level (CONFIG_NODE, &router_ospf6_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (CONFIG_NODE, &no_router_ospf6_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level (CONFIG_NODE, &ospf6d_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &ospf6d_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

    /* ospf router-id */ 
    install_element_level (OSPF6_NODE, &ospf6_router_id_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* distance */
	install_element_level (OSPF6_NODE, &distance_ospf6_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (OSPF6_NODE, &no_distance_ospf6_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    /* abr-summary */
    install_element_level (OSPF6_NODE, &area_range_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    //install_element_level (OSPF6_NODE, &area_range_advertise_cmd_vtysh, CONFIG_LEVE_5);    
    install_element_level (OSPF6_NODE, &no_area_range_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* redistribute */
	install_element_level (OSPF6_NODE, &ospf6_redistribute_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF6_NODE, &no_ospf6_redistribute_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF6_NODE, &ospf6_redistribute_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (OSPF6_NODE, &no_ospf6_redistribute_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    /* ospf eable */     
    //install_element_level (OSPF6_NODE, &ospf6_interface_area_cmd_vtysh);    
    //install_element_level (OSPF6_NODE, &ospf6_interface_ethernet_area_cmd_vtysh); 
    //install_element_level (OSPF6_NODE, &ospf6_interface_loopback_area_cmd_vtysh);
    //install_element_level (OSPF6_NODE, &no_ospf6_interface_area_cmd_vtysh);

    /*show ipv6 ospf6 interface*/
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_interface_ifname_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_interface_ifname_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (OSPF6_NODE, &show_ipv6_ospf6_interface_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_interface_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (OSPF6_NODE, &show_ipv6_ospf6_interface_vlanif_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_interface_vlanif_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (OSPF6_NODE, &show_ipv6_ospf6_interface_loopback_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_interface_loopback_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

    /*show ipv6 ospf6 route*/
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_route_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_route_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

    /*show ipv6 ospf6 lsdb*/
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_database_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_database_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_database_self_originated_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_database_self_originated_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_database_type_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_database_type_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_database_type_self_originated_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_database_type_self_originated_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

    /*show ipv6 ospf6 neighbor*/
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_neighbor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_neighbor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_neighbor_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_neighbor_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_neighbor_one_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_neighbor_one_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

    /*show ipv6 ospf6 error*/
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_error_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_error_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);   
	
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_linkstate_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_linkstate_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);  
    install_element_level (OSPF6_NODE, &show_ipv6_ospf6_linkstate_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_linkstate_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);      
	install_element_level (OSPF6_NODE, &show_ipv6_ospf6_route_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ipv6_ospf6_route_detail_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);  

	/* show ipv6 ospf6 instance */
	install_element_level (OSPF6_NODE, &show_ipv6_ospf6_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_ipv6_ospf6_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);  

	/* show ipv6 ospf6 statics */
	install_element_level (OSPF6_NODE, &show_ipv6_ospf6_statics_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_ipv6_ospf6_statics_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	/* show ipv6 ospf6 border */
	install_element_level (OSPF6_NODE, &show_ipv6_ospf6_border_routers_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_ipv6_ospf6_border_routers_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*************************** physical interface init ***************************************/
    /* ipv6 ospf6 cost */
    install_element_level (PHYSICAL_SUBIF_NODE, &ipv6_ospf6_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ipv6_ospf6_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ipv6_ospf6_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ipv6 ospf6 hellointerval */
    install_element_level (PHYSICAL_SUBIF_NODE, &ipv6_ospf6_hellointerval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_hellointerval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ipv6_ospf6_hellointerval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ipv6_ospf6_hellointerval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 deadinterval */
    install_element_level (PHYSICAL_SUBIF_NODE, &ipv6_ospf6_deadinterval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_deadinterval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ipv6_ospf6_deadinterval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC); 
    install_element_level (PHYSICAL_IF_NODE, &no_ipv6_ospf6_deadinterval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 priority */
    install_element_level (PHYSICAL_SUBIF_NODE, &ipv6_ospf6_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ipv6_ospf6_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ipv6_ospf6_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 instance id */
    install_element_level (PHYSICAL_SUBIF_NODE, &ipv6_ospf6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ipv6_ospf6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ipv6_ospf6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 mtu ignore */
    install_element_level (PHYSICAL_SUBIF_NODE, &ipv6_ospf6_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ipv6_ospf6_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ipv6_ospf6_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 mtu network */
    install_element_level (PHYSICAL_SUBIF_NODE, &ipv6_ospf6_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ipv6_ospf6_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ipv6_ospf6_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ipv6 ospf6 instance area */
    install_element_level (PHYSICAL_SUBIF_NODE, &ipv6_ospf6_interface_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_ipv6_ospf6_interface_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &ipv6_ospf6_interface_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_ipv6_ospf6_interface_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (LOOPBACK_IF_NODE, &ipv6_ospf6_interface_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (LOOPBACK_IF_NODE, &no_ipv6_ospf6_interface_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/****************************** trunk interface init **********************************/
	/* ipv6 ospf6 cost */
    install_element_level (TRUNK_IF_NODE, &ipv6_ospf6_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ipv6_ospf6_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ipv6_ospf6_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ipv6_ospf6_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ipv6 ospf6 hellointerval */
    install_element_level (TRUNK_IF_NODE, &ipv6_ospf6_hellointerval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ipv6_ospf6_hellointerval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ipv6_ospf6_hellointerval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ipv6_ospf6_hellointerval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 deadinterval */
    install_element_level (TRUNK_IF_NODE, &ipv6_ospf6_deadinterval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ipv6_ospf6_deadinterval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ipv6_ospf6_deadinterval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC); 
    install_element_level (TRUNK_SUBIF_NODE, &no_ipv6_ospf6_deadinterval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 priority */
    install_element_level (TRUNK_IF_NODE, &ipv6_ospf6_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ipv6_ospf6_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ipv6_ospf6_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ipv6_ospf6_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 instance id */
    install_element_level (TRUNK_IF_NODE, &ipv6_ospf6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ipv6_ospf6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ipv6_ospf6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ipv6_ospf6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 mtu ignore */
    install_element_level (TRUNK_IF_NODE, &ipv6_ospf6_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ipv6_ospf6_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ipv6_ospf6_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ipv6_ospf6_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 mtu network */
    install_element_level (TRUNK_IF_NODE, &ipv6_ospf6_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ipv6_ospf6_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ipv6_ospf6_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ipv6_ospf6_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ipv6 ospf6 instance area */
    install_element_level (TRUNK_IF_NODE, &ipv6_ospf6_interface_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_IF_NODE, &no_ipv6_ospf6_interface_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &ipv6_ospf6_interface_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TRUNK_SUBIF_NODE, &no_ipv6_ospf6_interface_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


	/****************************** vlanif interface init **********************************/
	/* ipv6 ospf6 cost */
    install_element_level (VLANIF_NODE, &ipv6_ospf6_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ipv6_ospf6_cost_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ipv6 ospf6 hellointerval */
    install_element_level (VLANIF_NODE, &ipv6_ospf6_hellointerval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ipv6_ospf6_hellointerval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 deadinterval */
    install_element_level (VLANIF_NODE, &ipv6_ospf6_deadinterval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ipv6_ospf6_deadinterval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ipv6 ospf6 priority */
    install_element_level (VLANIF_NODE, &ipv6_ospf6_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ipv6_ospf6_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 instance id */
    install_element_level (VLANIF_NODE, &ipv6_ospf6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ipv6_ospf6_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 mtu ignore */
    install_element_level (VLANIF_NODE, &ipv6_ospf6_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ipv6_ospf6_mtu_ignore_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    /* ipv6 ospf6 mtu network */
    install_element_level (VLANIF_NODE, &ipv6_ospf6_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ipv6_ospf6_network_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* ipv6 ospf6 instance area */
    install_element_level (VLANIF_NODE, &ipv6_ospf6_interface_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VLANIF_NODE, &no_ipv6_ospf6_interface_area_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);



    /* debug ospf6 packet */
    install_element_level (CONFIG_NODE, &debug_ospf6_message_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_message_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    install_element_level (CONFIG_NODE, &debug_ospf6_message_sendrecv_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_message_sendrecv_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

    /* debug ospf6 neighbor */
    install_element_level (CONFIG_NODE, &debug_ospf6_neighbor_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    install_element_level (CONFIG_NODE, &debug_ospf6_neighbor_detail_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_neighbor_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_neighbor_detail_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    
    /* debug ospf6 route */
    install_element_level (CONFIG_NODE, &debug_ospf6_route_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_route_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

    /* debug ospf6 interface */
    install_element_level (CONFIG_NODE, &debug_ospf6_interface_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_interface_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

    /* debug ospf6 flooding */
    install_element_level (CONFIG_NODE, &debug_ospf6_flooding_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_flooding_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

    /* debug ospf6 neighbor */
    install_element_level (CONFIG_NODE, &debug_ospf6_spf_process_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_spf_process_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &debug_ospf6_spf_time_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_spf_time_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &debug_ospf6_spf_database_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_spf_database_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    
    /*debug ospf6 route*/
    install_element_level (CONFIG_NODE, &debug_ospf6_route_sendrecv_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_route_sendrecv_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
	
	/*debug ospf6 route*/
    install_element_level (CONFIG_NODE, &debug_ospf6_lsa_hex_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_lsa_hex_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &debug_ospf6_lsa_hex_detail_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_lsa_hex_detail_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

	/* debug ospf6 abr */
	install_element_level (CONFIG_NODE, &debug_ospf6_abr_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_abr_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

	/* debug ospf6  border-router*/
	install_element_level (CONFIG_NODE, &debug_ospf6_brouter_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_brouter_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &debug_ospf6_brouter_router_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_brouter_router_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
	//install_element_level (CONFIG_NODE, &debug_ospf6_brouter_area_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_brouter_area_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);

	/* debug ospf6 asbr */
	install_element_level (CONFIG_NODE, &debug_ospf6_asbr_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    //install_element_level (CONFIG_NODE, &no_debug_ospf6_asbr_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
    
    /* debug ospf6 other */
	install_element_level (CONFIG_NODE, &debug_ospf6_other_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
}






