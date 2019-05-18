/**
 * \page cmds_ref_bgp BGP
 * - \subpage modify_log_bgp
 * 
 */
 
/**
 * \page modify_log_bgp Modify Log
 * \section bgp-v007r004 HiOS-V007R004
 *  -# 
 * \section bgp-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "lib/route_types.h"
#include "lib/ifm_common.h"

#define CMD_AS_RANGE "<1-4294967295>"

static struct cmd_node bgpd_node =
{
  BGP_NODE,
  "%s(config-bgp)# "
};

static struct cmd_node neighbor_node =
{
  NEIGHBOR_NODE,
  "%s(config-bgp-neighbor)# "
};

static struct cmd_node neighbor_node_ipv6 =
{
  NEIGHBOR_NODE_IPV6,
  "%s(config-bgp-neighbor-ipv6)# "
};

static struct cmd_node bgp_vpn_instance_node =
{
	BGP_VPN_INSTANCE_NODE,
	"%s(config-bgp-af-vpn-instance)# "
};

static struct cmd_node bgp_ipv6_unicast_node =
{
	BGP_IPV6_NODE,
	"%s(config-bgp-af-ipv6)# "
};

#if 0
static struct cmd_node bgp_vpnv4_node =
{
	BGP_VPNV4_NODE,
	"%s(config-bgp-af-vpnv4)# ",
};
#endif
DEFUNSH (VTYSH_BGPD,
	 router_bgp,
	 router_bgp_cmd,
	 "bgp as " CMD_AS_RANGE,
	 BGP_STR
	 "Autonomous System\n"
	 AS_STR)
{
  vty->node = BGP_NODE;
  return CMD_SUCCESS;
}

DEFSH (VTYSH_BGPD, no_router_bgp_cmd, 
       "no bgp as " CMD_AS_RANGE,
	   NO_STR
	   BGP_STR	 
	   "Autonomous System\n"
	   AS_STR)
       
DEFUNSH (VTYSH_BGPD,
	 vtysh_exit_bgpd,
	 vtysh_exit_bgpd_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

DEFUNSH (VTYSH_BGPD,
	 vtysh_exit_neighbor,
	 vtysh_exit_neighbor_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

DEFUNSH (VTYSH_BGPD,
	 vtysh_exit_vpn_instance,
	 vtysh_exit_vpn_instance_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

DEFUNSH (VTYSH_BGPD,
	 vtysh_exit_vpnv4,
	 vtysh_exit_vpnv4_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}


DEFUNSH (VTYSH_BGPD,
	 bgp_neighbor,
	 bgp_neighbor_cmd,
	 "bgp neighbor A.B.C.D",
	 BGP_STR
	 "Specify neighbor router\n"
     "Specify an IPv4 peer address\n")
{
  vty->node = NEIGHBOR_NODE;
  return CMD_SUCCESS;
}

DEFSH (VTYSH_BGPD,
	   no_bgp_neighbor_cmd,
       "no bgp neighbor A.B.C.D",
       NO_STR
       BGP_STR
	   "Specify neighbor router\n"
       "Specify an IPv4 peer address\n")     


DEFUNSH (VTYSH_BGPD,
	 bgp_neighbor_ipv6,
	 bgp_neighbor_ipv6_cmd,
	 "bgp neighbor ipv6 X:X::X:X",
	 BGP_STR
	 "Specify neighbor router\n"
	 "Specify an IPv6\n"
     "Specify an IPv6 peer address\n")
{
  vty->node = NEIGHBOR_NODE_IPV6;
  return CMD_SUCCESS;
}

DEFSH (VTYSH_BGPD,
	   no_bgp_neighbor_ipv6_cmd,
       "no bgp neighbor ipv6 X:X::X:X",
       NO_STR
       BGP_STR
	   "Specify neighbor router\n"
	   "Specify an IPv6\n"
       "Specify an IPv6 peer address\n")


DEFSH (VTYSH_BGPD,
       router_id_cmd,
       "router-id A.B.C.D",
       "Override configured router identifier\n"
       "Manually configured router identifier\n")

DEFSH (VTYSH_BGPD,
       no_router_id_cmd,
       "no router-id",
       NO_STR
       "Override configured router identifier\n")
       
DEFSH (VTYSH_BGPD,
	   keepalive_hold_time_cmd,
       "keepalive <0-3600> hold-time <0-65535>",
       "Keepalive timer\n"
	   "Value of Keepalive timer(seconds)\n"
       "Hold timer\n"
       "Value of Hold timer(seconds)\n")

DEFSH (VTYSH_BGPD,
       no_keepalive_hold_time_cmd,
       "no keepalive <0-3600> hold-time <0-65535>",
       NO_STR
       "Keepalive timer\n"
	   "Value of Keepalive timer(seconds)\n"
       "Hold timer\n"
       "Value of Hold timer(seconds)\n")

DEFSH (VTYSH_BGPD,
	   conenct_timers_cmd,
	   "retry-time <1-3600>",
	   "Connect-retry timer\n"
	   "Value of connect-retry timer(seconds)\n")
	   
DEFSH (VTYSH_BGPD,
       no_conenct_timers_cmd,
       "no retry-time",
       NO_STR
	   "Connect-retry timer\n")
	  
DEFSH (VTYSH_BGPD,
       bgp_bestpath_aspath_ignore_cmd,
       "as-path ignore",
       "AS-path attribute\n"
       "Ignore as-path length in selecting a route\n")
       
DEFSH (VTYSH_BGPD,
	   no_bgp_bestpath_aspath_ignore_cmd,
	   "no as-path ignore",
	   NO_STR
	   "AS-path attribute\n"
	   "Ignore as-path length in selecting a route\n")      

DEFSH (VTYSH_BGPD,
       local_preference_cmd,
       "local-preference <0-4294967295>",
       "Local preference (the higher the value, the higher the preference is)\n"
       "Specify a local preference\n")

DEFSH (VTYSH_BGPD,
       no_local_preference_cmd,
       "no local-preference",
       NO_STR
       "Local preference (the higher the value, the higher the preference is)\n")

DEFSH (VTYSH_BGPD,
       distance_ebgp_ibgp_local_cmd,
       "distance (ebgp|ibgp|local) <1-255>",
       "Define an administrative distance\n"
       "EBGP route preference\n"
       "IBGP route preference\n"
       "Local created route preference\n"
       "BGP distance\n")
   
DEFSH (VTYSH_BGPD,
	   no_distance_ebgp_ibgp_local_cmd,
	   "no distance (ebgp|ibgp|local)",
	   NO_STR
	   "Define an administrative distance\n"
	   "EBGP route preference\n"
	   "IBGP route preference\n"
	   "Local created route preference\n"
	   "BGP distance\n")

       
DEFSH (VTYSH_BGPD,
       bgp_redistribute_ipv4_cmd,
       "redistribute " QUAGGA_IP_REDIST_STR_BGPD,
       "Redistribute information from another routing protocol\n"
       QUAGGA_IP_REDIST_HELP_STR_BGPD)
       
DEFSH (VTYSH_BGPD,
	   bgp_redistribute_ipv4_metric_cmd,
	   "redistribute " QUAGGA_IP_REDIST_STR_BGPD " metric <1-4294967295>",
	   "Redistribute information from another routing protocol\n"
	   QUAGGA_IP_REDIST_HELP_STR_BGPD
	   "Metric for redistributed routes\n"
	   "Default metric\n")
	   

DEFSH (VTYSH_BGPD,
       no_bgp_redistribute_ipv4_cmd,
       "no redistribute " QUAGGA_IP_REDIST_STR_BGPD,
       NO_STR
       "Redistribute information from another routing protocol\n"
       QUAGGA_IP_REDIST_HELP_STR_BGPD)

DEFSH (VTYSH_BGPD,
       bgp_redistribute_roi_ipv4_cmd,
       "redistribute " QUAGGA_IP_REDIST_STR_ROI_BGPD,
	   REDIST_STR
       QUAGGA_IP_REDIST_HELP_STR_ROI_BGPD)

DEFSH (VTYSH_BGPD,
       bgp_redistribute_roi_ipv4_metric_cmd,
       "redistribute " QUAGGA_IP_REDIST_STR_ROI_BGPD " metric <1-4294967295>",
	   REDIST_STR
       QUAGGA_IP_REDIST_HELP_STR_ROI_BGPD
       "Metric for redistributed routes\n"
       "Default metric\n")
       
DEFSH (VTYSH_BGPD,
       no_bgp_redistribute_roi_ipv4_cmd,
       "no redistribute " QUAGGA_IP_REDIST_STR_ROI_BGPD,
       NO_STR
       "Redistribute information from another routing protocol\n"
       QUAGGA_IP_REDIST_HELP_STR_ROI_BGPD)

DEFSH (VTYSH_BGPD,
       bgp_redistribute_roi_ipv6_cmd,
       "redistribute " QUAGGA_IP6_REDIST_STR_ROI_BGPD,
       REDIST_STR
       QUAGGA_IP6_REDIST_HELP_STR_ROI_BGPD)


DEFSH (VTYSH_BGPD,
       bgp_redistribute_roi_ipv6_metric_cmd,
       "redistribute " QUAGGA_IP6_REDIST_STR_ROI_BGPD " metric <1-4294967295>",
       REDIST_STR
       QUAGGA_IP6_REDIST_HELP_STR_ROI_BGPD
       "Metric for redistributed routes\n"
       "Default metric\n")

DEFSH (VTYSH_BGPD,
       no_bgp_redistribute_roi_ipv6_cmd,
       "no redistribute " QUAGGA_IP6_REDIST_STR_ROI_BGPD,
       NO_STR
       "Redistribute information from another routing protocol\n"
       QUAGGA_IP6_REDIST_HELP_STR_ROI_BGPD)

DEFSH (VTYSH_BGPD,
       bgp_network_cmd,
       "redistribute network A.B.C.D/M",
       "Redistribute information from another routing protocol\n"
       "Specify a network to announce via BGP\n"
       "IP prefix <network>/<length>, e.g., 35.0.0.0/8\n")

DEFSH (VTYSH_BGPD,
       no_bgp_network_cmd,
       "no redistribute network A.B.C.D/M",
       NO_STR       
       "Redistribute information from another routing protocol\n"
       "Specify a network to announce via BGP\n"
       "IP prefix <network>/<length>, e.g., 35.0.0.0/8\n")

DEFSH (VTYSH_BGPD,
       bgp_network_mask_natural_cmd,
       "redistribute network A.B.C.D",
       "Redistribute information from another routing protocol\n"
       "Specify a network to announce via BGP\n"
       "Network number\n")

DEFSH (VTYSH_BGPD,
       no_bgp_network_mask_natural_cmd,
       "no redistribute network A.B.C.D",
       NO_STR
       "Redistribute information from another routing protocol\n"
       "Specify a network to announce via BGP\n"
       "Network number\n")

DEFSH (VTYSH_BGPD,
       aggregate_address_as_set_summary_cmd,
       "summary A.B.C.D/M {as-set|summary-only}",
       "Configure BGP aggregate entries\n"
       "Aggregate prefix\n"
       "Generate AS set path information\n"
       "Filter more specific routes from updates\n")
       
DEFSH (VTYSH_BGPD,
       no_aggregate_address_as_set_summary_cmd,
       "no summary A.B.C.D/M",
       NO_STR       
       "Configure BGP aggregate entries\n"
       "Aggregate prefix\n")
       
DEFSH (VTYSH_BGPD,
       ipv6_aggregate_address_as_set_summary_cmd,
       "summary X:X::X:X/M {as-set|summary-only}",
       "Configure BGP aggregate entries\n"
       "Aggregate prefix\n"
       "Generate AS set path information\n"
       "Filter more specific routes from updates\n")
       
DEFSH (VTYSH_BGPD,
       no_ipv6_aggregate_address_as_set_summary_cmd,
       "no summary X:X::X:X/M ",
       NO_STR       
       "Configure BGP aggregate entries\n"
       "Aggregate prefix\n")  


DEFSH (VTYSH_BGPD,
       bgp_damp_set3_cmd,
       "dampening",
       "Enable route-flap dampening\n")

DEFSH (VTYSH_BGPD,
       bgp_damp_set_cmd,
       "dampening <1-45> <1-20000> <1-20000> <1-255>",
       "Enable route-flap dampening\n"
       "Half-life time for the penalty(minutes)\n"
       "Value to start reusing a route\n"
       "Value to start suppressing a route\n"
       "Maximum duration to suppress a route(minutes)\n")

DEFSH (VTYSH_BGPD,
       no_bgp_damp_set_cmd,
       "no dampening",
       NO_STR
       "Enable route-flap dampening\n")

DEFSH (VTYSH_BGPD,
       med_default_value_cmd,
       "med default <1-4294967295>",
       "Set the MED attribute\n"
       "Set default MED value for imported routes\n"
       "MED value\n")

DEFSH (VTYSH_BGPD,
       no_med_default_value_cmd,
       "no med default",
       NO_STR
       "Set the MED attribute\n"
       "Set default MED value for imported routes\n")
       
DEFSH (VTYSH_BGPD,
       bgp_always_compare_med_cmd,
       "med compare different-as",
	   "Set the MED attribute\n"
       "Compare MED\n"
       "Allow comparing MED from different neighbors\n")

DEFSH (VTYSH_BGPD,
       no_bgp_always_compare_med_cmd,
       "no med compare different-as",
       NO_STR
	   "Set the MED attribute\n"
       "Compare MED\n"
       "Allow comparing MED from different neighbors\n")

DEFSH (VTYSH_BGPD,
       bgp_deterministic_med_cmd,
       "med compare deterministic",
   	   "Set the MED attribute\n"
       "Compare MED\n"
       "Pick the best-MED path among paths advertised from the neighboring AS\n")

DEFSH (VTYSH_BGPD,
       no_bgp_deterministic_med_cmd,
       "no med compare deterministic",
       NO_STR
   	   "Set the MED attribute\n"
       "Compare MED\n"
       "Pick the best-MED path among paths advertised from the neighboring AS\n")

DEFSH (VTYSH_BGPD,
       description_name_cmd,
       "description STRING",
       "Neighbor specific description\n"
       "Up to 80 characters describing this neighbor\n")

DEFSH (VTYSH_BGPD,
       no_description_name_cmd,
       "no description",
	   NO_STR
       "Neighbor specific description\n")

DEFSH (VTYSH_BGPD,
       neighbor_enable_cmd,
       "neighbor (enable|disable)",
       "Specify a BGP neighbor\n"
       "Enable neighbor\n"
       "Disable neighbor\n")

DEFSH (VTYSH_BGPD,
       remote_as_cmd,
       "remote-as " CMD_AS_RANGE,
       "Specify a BGP neighbor\n"
       AS_STR)

DEFSH (VTYSH_BGPD,
       bgp_auth_md5_password_cmd,
       "auth md5 PASSWORD",
       "Specify an authentication\n"
       "Use MD5 algorithm\n"
       "The BGP password <1-80>\n")

DEFSH (VTYSH_BGPD,
       no_bgp_auth_md5_password_cmd,
	   "no auth md5",
       NO_STR
       "Specify an authentication\n"
       "Use MD5 algorithm\n")
       
DEFSH (VTYSH_BGPD,
       source_address_cmd,
       "source A.B.C.D",
       "source address\n"
       "source A.B.C.D\n")

DEFSH (VTYSH_BGPD,
       source_address_ipv6_cmd,
       "source X:X::X:X",
       "source address\n"
       "source X:X::X:X\n")

DEFSH (VTYSH_BGPD,
       ebgp_multihop_ttl_cmd,
       "ebgp multihop <1-255>",
       "EBGP\n"
       "Allow EBGP neighbors not on directly connected networks\n"
       "maximum hop count\n")
       
DEFSH (VTYSH_BGPD,
       no_ebgp_multihop_ttl_cmd,
       "no ebgp multihop",
       NO_STR
       "EBGP\n"
       "Allow EBGP neighbors not on directly connected networks\n")      

DEFSH (VTYSH_BGPD,
       local_as_cmd,
       "ebgp local-as " CMD_AS_RANGE,
       "EBGP\n"
       "Specify a local-as number\n"
       "AS number used as local AS\n")
       
DEFSH (VTYSH_BGPD,
       no_local_as_cmd,
       "no ebgp local-as",
       NO_STR
       "EBGP\n"
       "Specify a local-as number\n")      

DEFSH (VTYSH_BGPD,
       route_redistribute_default_route_cmd,
       "route redistribute default-route",
       "route\n"
	   REDIST_STR
       "Advertise default route to this neighbor\n")
       
DEFSH (VTYSH_BGPD,
       no_route_redistribute_default_route_cmd,
       "no route redistribute default-route",
       NO_STR
       "route\n"
	   REDIST_STR
       "Advertise default route to this neighbor\n")

DEFSH (VTYSH_BGPD,
       route_redistribute_ibgp_route_cmd,
       "route redistribute ibgp",
       "route\n"
	   REDIST_STR
       "Advertise ibgp route to this neighbor\n")
       
DEFSH (VTYSH_BGPD,
       no_route_redistribute_ibgp_route_cmd,
       "no route redistribute ibgp",
       NO_STR
       "route\n"
	   REDIST_STR
       "Advertise ibgp route to this neighbor\n")
       
DEFSH (VTYSH_BGPD,
       route_limit_cmd,
       "route limit <1-200000>",
       "route\n"
       "Maximum number of prefix accept from this peer\n"
       "maximum no. of prefix limit\n")

DEFSH (VTYSH_BGPD,
       no_route_limit_cmd,
       "no route limit",
       NO_STR
       "route\n"
       "Maximum number of prefix accept from this peer\n")

DEFSH (VTYSH_BGPD,
       route_weight_bgp_cmd,
       "route weight <0-65535>",
       "route\n"
       "Set default weight for routes from this neighbor\n"
       "default weight\n")

DEFSH (VTYSH_BGPD,
       no_route_weight_bgp_cmd,
       "no route weight",
       NO_STR
       "route\n"
       "Set default weight for routes from this neighbor\n")

DEFSH (VTYSH_BGPD,
       route_next_hop_change_local_cmd,
       "route next-hop change local",
       "route\n"
       "Specify local address as the next hop of routes advertised to the neighbor\n"
       "change\n"
       "local\n")
       
DEFSH (VTYSH_BGPD,
       no_route_next_hop_change_local_cmd,
       "no route next-hop change local",
       NO_STR
       "route\n"
       "Specify local address as the next hop of routes advertised to the neighbor\n"
       "change\n"
       "local\n")

DEFSH (VTYSH_BGPD,
       advertise_interval_cmd,
       "advertisement-interval <0-600>",
       "Minimum interval between sending BGP routing updates\n"
       "Time in seconds\n")

DEFSH (VTYSH_BGPD,
       no_advertise_interval_cmd,
       "no advertisement-interval",
       NO_STR
       "Minimum interval between sending BGP routing updates\n")

DEFSH (VTYSH_BGPD,
       as_path_allow_as_loop_cmd,
       "as-path allow as-loop <1-10>",
	   "AS-path attribute\n"
       "Configure permit of as-path loop\n"
       "Accept as-path with my AS present in it\n"
       "Number of occurances of AS number\n")

DEFSH (VTYSH_BGPD,
       no_as_path_allow_as_loop_cmd,
       "no as-path allow as-loop",
       NO_STR
	   "AS-path attribute\n"
       "Configure permit of as-path loop\n"
       "Accept as-path with my AS present in it\n")

DEFSH (VTYSH_BGPD,
       as_path_exclude_private_as_cmd,
       "as-path exclude private-as",
	   "AS-path attribute\n"
       "Remove private AS number\n"
       "Remove private AS number from outbound updates\n")

DEFSH (VTYSH_BGPD,
       no_as_path_exclude_private_as_cmd,
       "no as-path exclude private-as",
       NO_STR
	   "AS-path attribute\n"
       "Remove private AS number\n"
       "Remove private AS number from outbound updates\n")

DEFSH (VTYSH_BGPD,
       ipv4_family_vpnv4_cmd,
       "ipv4-family vpnv4",
       "Specify IPv4 address family\n"
       "Specify VPNv4 address family\n")

DEFSH (VTYSH_BGPD,
       no_ipv4_family_vpnv4_cmd,
       "no ipv4-family vpnv4",
       NO_STR
       "Specify IPv4 address family\n"
       "Specify VPNv4 address family\n")

DEFSH (VTYSH_BGPD,
       ipv4_family_vpn_instance_cmd,
       "ipv4-family vpn-instance <1-128>",
       "Specify IPv4 address family\n"
       "Specify VPN instance\n"
       "Number of the L3VPN instance\n")

DEFSH (VTYSH_BGPD,
       no_ipv4_family_vpn_instance_cmd,
       "no ipv4-family vpn-instance <1-128>",
       NO_STR
       "Specify IPv4 address family\n"
       "Specify VPN instance\n"
       "Number of the L3VPN instance\n")
       

DEFSH (VTYSH_BGPD,
       binding_tunnel_cmd,
       "binding tunnel USP",
       "Enable binding of a tunnel\n"
       CLI_INTERFACE_TUNNEL_STR
       CLI_INTERFACE_TUNNEL_VHELP_STR)

DEFSH (VTYSH_BGPD,
  	  no_binding_tunnel_cmd,
  	  "no binding tunnel",
  	  NO_STR
  	  "Enable binding of a tunnel\n"
  	  CLI_INTERFACE_TUNNEL_STR)
       
DEFUNSH (VTYSH_BGPD,
       address_family_vpn_instance,
       address_family_vpn_instance_cmd,
       "ipv4-family vpn-instance <1-128>",
       "Specify IPv4 address family\n"
       "Specify VPN instance\n"
       "Number of the L3VPN instance\n")
{
	vty->node = BGP_VPN_INSTANCE_NODE;
	return CMD_SUCCESS;
}

#if 0
DEFUNSH (VTYSH_BGPD,
	   address_family_vpnv4,
       address_family_vpnv4_cmd,
       "ipv4-family vpnv4",
       "Specify IPv4 address family\n"
       "Specify VPNv4 address family\n")
{
	  vty->node = BGP_VPNV4_NODE;
	  return CMD_SUCCESS;
}
#endif

DEFSH (VTYSH_BGPD,
       bgp_redistribute_vrf_ipv4_cmd,
       "redistribute " QUAGGA_IP_REDIST_STR_BGPD,
       "Redistribute information from another routing protocol\n"
       QUAGGA_IP_REDIST_HELP_STR_BGPD)

DEFSH (VTYSH_BGPD,
       bgp_redistribute_vrf_ipv4_metric_cmd,
       "redistribute " QUAGGA_IP_REDIST_STR_BGPD " metric <1-4294967295>",
       "Redistribute information from another routing protocol\n"
       QUAGGA_IP_REDIST_HELP_STR_BGPD
       "Metric for redistributed routes\n"
       "Default metric\n")

DEFSH (VTYSH_BGPD,
       no_bgp_redistribute_vrf_ipv4_cmd,
       "no redistribute " QUAGGA_IP_REDIST_STR_BGPD,
       NO_STR
       "Redistribute information from another routing protocol\n"
       QUAGGA_IP_REDIST_HELP_STR_BGPD)     

DEFSH (VTYSH_BGPD,
       bgp_redistribute_vrf_roi_ipv4_cmd,
       "redistribute " QUAGGA_IP_REDIST_STR_ROI_BGPD,
       "Redistribute information from another routing protocol\n"
       QUAGGA_IP_REDIST_HELP_STR_ROI_BGPD)


DEFSH (VTYSH_BGPD,
       bgp_redistribute_vrf_roi_ipv4_metric_cmd,
       "redistribute " QUAGGA_IP_REDIST_STR_ROI_BGPD " metric <1-4294967295>",
       REDIST_STR
       QUAGGA_IP_REDIST_HELP_STR_ROI_BGPD
       "Metric for redistributed routes\n"
       "Default metric\n")
       

DEFSH (VTYSH_BGPD,
       no_bgp_redistribute_vrf_roi_ipv4_cmd,
       "no redistribute " QUAGGA_IP_REDIST_STR_ROI_BGPD,
       NO_STR
       "Redistribute information from another routing protocol\n"
       QUAGGA_IP_REDIST_HELP_STR_ROI_BGPD)

DEFUNSH (VTYSH_BGPD,
       address_family_ipv6_unicast,
       address_family_ipv6_unicast_cmd,
       "ipv6-family unicast",
       "Specify IPv6 unicast address family\n"
       "Specify unicast address family\n")
{
	vty->node = BGP_IPV6_NODE;
	return CMD_SUCCESS;
}
  
DEFSH (VTYSH_BGPD,
       show_bgp_neighbors_cmd,
       "show bgp neighbors",
       SHOW_STR
       BGP_STR
       "Neighbor routers\n")

DEFSH (VTYSH_BGPD,
       show_bgp_neighbors_peer_cmd,
       "show bgp neighbors A.B.C.D",
       SHOW_STR
       BGP_STR
       "Neighbor routers\n"
       "Specify an IPv4 peer address\n")
       
DEFSH (VTYSH_BGPD, 
       show_bgp_ipv4_paths_cmd,
       "show bgp paths",
       SHOW_STR
       BGP_STR
       "Path information\n")

DEFSH (VTYSH_BGPD,
       show_bgp_ipv4_route_cmd,
       "show bgp route",
       SHOW_STR
       BGP_STR
       "BGP routing table\n")

DEFSH (VTYSH_BGPD,
       show_bgp_ipv4_route_neighbor_cmd,
       "show bgp route neighbors A.B.C.D",
       SHOW_STR
       BGP_STR
       "BGP routing table\n"
       "Routes received from the remote neighbors\n"
       "Specify an IPv4 peer address\n")
       
DEFSH (VTYSH_BGPD,
       show_bgp_ipv4_route_dampening_parameter_cmd,
       "show bgp route dampening parameter",
       SHOW_STR
       BGP_STR
       "BGP routing table\n"
       "BGP-dampening status\n"
       "Display dampening parameter\n")

DEFSH (VTYSH_BGPD,
       show_bgp_ipv4_vpn_cmd,
       "show bgp vpnv4 route",
       SHOW_STR
       BGP_STR
       "Specify VPNv4 address family\n"
       "BGP routing table\n")

DEFSH (VTYSH_BGPD,
	  show_bgp_ipv4_vpn_instance_cmd,
	  "show bgp vpnv4 route vpn-instance <1-128>",
	  SHOW_STR
	  BGP_STR
	  "Specify VPNv4 address family\n"
	  "BGP routing table\n"
	  "VPN instance\n"
	  "Number of the L3VPN instance\n")

DEFSH (VTYSH_BGPD ,
	show_bgp_ipv4_vpn_statistics_cmd,
	"show bgp vpnv4 route statistics",
	SHOW_STR
	BGP_STR
	"Specify VPNv4 address family\n"
	"BGP routing table\n"
	"Statistics of route\n")	


DEFSH (VTYSH_BGPD,
       show_bgp_ipv4_vpn_tags_cmd,
       "show bgp vpnv4 route label",
       SHOW_STR
       BGP_STR
       "Specify VPNv4 address family\n"
       "BGP routing table\n"
       "Labeled route information\n")

	   
DEFSH (VTYSH_BGPD,
       show_bgp_ipv6_neighbors_cmd,
       "show bgp ipv6 neighbors",
       SHOW_STR
       BGP_STR
       "Specify IPv6 unicast address family\n"
       "Neighbor routers\n")


DEFSH (VTYSH_BGPD,
       show_bgp_ipv6_neighbors_peer_cmd,
       "show bgp ipv6 neighbors X:X::X:X",
       SHOW_STR
       BGP_STR
       "Specify IPv6 unicast address family\n"
       "Neighbor routers\n"
       "Specify an IPv6 peer address\n")
			  
DEFSH (VTYSH_BGPD,
	  show_bgp_ipv6_route_cmd,
	  "show bgp ipv6 route",
	  SHOW_STR
	  BGP_STR
	  "Specify IPv6 unicast address family\n"
	  "BGP routing table\n")
    
DEFSH (VTYSH_BGPD,
       debug_bgp_fsm_cmd,
       "debug bgp fsm",
       DEBUG_STR
       BGP_STR
       "BGP Finite State Machine\n")
	
DEFSH (VTYSH_BGPD,
	   no_debug_bgp_fsm_cmd,
	   "no debug bgp fsm",
	   NO_STR
	   DEBUG_STR
	   BGP_STR
	   "Finite State Machine\n")

DEFSH (VTYSH_BGPD,
       debug_bgp_events_cmd,
       "debug bgp events",
       DEBUG_STR
       BGP_STR
       "BGP events\n")

DEFSH (VTYSH_BGPD,
       no_debug_bgp_events_cmd,
       "no debug bgp events",
       NO_STR
       DEBUG_STR
       BGP_STR
       "BGP events\n")

DEFSH (VTYSH_BGPD,
       debug_bgp_packet_cmd,
       "debug bgp packet",
       DEBUG_STR
       BGP_STR
       "BGP packet\n")

DEFSH (VTYSH_BGPD,
       no_debug_bgp_packet_cmd,
       "no debug bgp packet",
       NO_STR
       DEBUG_STR
       BGP_STR
       "BGP packet\n")


DEFSH (VTYSH_BGPD,
       show_debugging_bgp_cmd,
       "show bgp debug",
       SHOW_STR
       BGP_STR
       "Debugging functions\n")

DEFSH (VTYSH_BGPD,
	  bgp_show_timerinfo_cmd,
	  "show bgp timer",
	  SHOW_STR
	  "Bgp module information\n"
	  "Bgp module timer\n")

DEFSH (VTYSH_BGPD,
	  bgp_conf_debug_cmd,
	  "debug bgp (enable|disable) (as4|fsm|events|packet|keepalive|update|normal|zebra|all|other)",
	  "Debug config\n"
	  "Bgp config\n"
	  "Bgp debug enable\n"
	  "Bgp debug disable\n"
	  "Debug as4\n"
	  "Debug fsm\n"
	  "Debug events\n"
	  "Debug packet\n"
	  "Debug keepalive\n"
	  "Debug update\n"
	  "Debug normal\n"
	  "Debug zebra\n"
	  "Debug all\n"
	 )

/* debug 日志发送到 syslog 使能状态设置 */
DEFSH (VTYSH_BGPD,bgpd_log_level_ctl_cmd_vtysh,	"debug bgpd(enable | disable)",		
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n")	

/* debug 日志发送到 syslog 使能状态显示 */
DEFSH (VTYSH_BGPD,bgpd_show_log_level_ctl_cmd_vtysh,	"show debug bgpd",		
	SHOW_STR"Output log of debug level\n""Program name\n")	


void vtysh_init_bgp_cmd ()
{
	install_node (&bgpd_node, NULL); 
	install_node (&neighbor_node, NULL); 
	install_node (&neighbor_node_ipv6, NULL); 
	install_node (&bgp_ipv6_unicast_node, NULL); 
	install_node (&bgp_vpn_instance_node, NULL); 
	//install_node (&bgp_vpnv4_node, NULL); 
	
	vtysh_install_default (BGP_NODE);
	vtysh_install_default (NEIGHBOR_NODE);
	vtysh_install_default (NEIGHBOR_NODE_IPV6);
	//vtysh_install_default (BGP_VPNV4_NODE);
	vtysh_install_default (BGP_VPN_INSTANCE_NODE);
	vtysh_install_default (BGP_IPV6_NODE);
	

	install_element_level (BGP_NODE, &vtysh_exit_bgpd_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &vtysh_exit_neighbor_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &vtysh_exit_neighbor_cmd ,VISIT_LEVE, CMD_SYNC);
	//install_element_level (BGP_VPNV4_NODE, &vtysh_exit_neighbor_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (BGP_VPN_INSTANCE_NODE, &vtysh_exit_vpn_instance_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (BGP_IPV6_NODE, &vtysh_exit_vpn_instance_cmd, VISIT_LEVE, CMD_SYNC);
	
 	install_element_level (CONFIG_NODE, &router_bgp_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (CONFIG_NODE, &no_router_bgp_cmd, CONFIG_LEVE_5, CMD_SYNC);

	/*address family*/
    install_element_level (BGP_NODE, &address_family_vpn_instance_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (BGP_NODE, &address_family_ipv6_unicast_cmd, VISIT_LEVE, CMD_SYNC); 
   // install_element (BGP_NODE, &address_family_vpnv4_cmd);
	
    install_element_level (BGP_NODE, &bgp_neighbor_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (BGP_NODE, &no_bgp_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (BGP_NODE, &bgp_neighbor_ipv6_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (BGP_NODE, &no_bgp_neighbor_ipv6_cmd, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (BGP_NODE, &router_id_cmd, CONFIG_LEVE_5, CMD_SYNC);
	//install_element (BGP_NODE, &no_router_id_cmd);
	install_element_level (BGP_NODE, &keepalive_hold_time_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_keepalive_hold_time_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &conenct_timers_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_conenct_timers_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &bgp_bestpath_aspath_ignore_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_bgp_bestpath_aspath_ignore_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &local_preference_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_local_preference_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &distance_ebgp_ibgp_local_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_distance_ebgp_ibgp_local_cmd, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (BGP_NODE, &bgp_redistribute_ipv4_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &bgp_redistribute_ipv4_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_bgp_redistribute_ipv4_cmd, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (BGP_NODE, &bgp_redistribute_roi_ipv4_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &bgp_redistribute_roi_ipv4_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_bgp_redistribute_roi_ipv4_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &bgp_network_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_bgp_network_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &bgp_network_mask_natural_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_bgp_network_mask_natural_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &aggregate_address_as_set_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_aggregate_address_as_set_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &bgp_damp_set_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &bgp_damp_set3_cmd, CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level (BGP_NODE, &no_bgp_damp_set_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &med_default_value_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_med_default_value_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &bgp_always_compare_med_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_bgp_always_compare_med_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &bgp_deterministic_med_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_NODE, &no_bgp_deterministic_med_cmd, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (NEIGHBOR_NODE, &description_name_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_description_name_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &neighbor_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &remote_as_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &bgp_auth_md5_password_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_bgp_auth_md5_password_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &source_address_cmd, CONFIG_LEVE_5, CMD_SYNC);
  	install_element_level (NEIGHBOR_NODE, &ebgp_multihop_ttl_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_ebgp_multihop_ttl_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &local_as_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_local_as_cmd, CONFIG_LEVE_5, CMD_SYNC);
  	install_element_level (NEIGHBOR_NODE, &route_redistribute_default_route_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_route_redistribute_default_route_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &route_redistribute_ibgp_route_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_route_redistribute_ibgp_route_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &route_limit_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_route_limit_cmd, CONFIG_LEVE_5, CMD_SYNC);
  	install_element_level (NEIGHBOR_NODE, &route_weight_bgp_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_route_weight_bgp_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &route_next_hop_change_local_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_route_next_hop_change_local_cmd, CONFIG_LEVE_5, CMD_SYNC);
  	install_element_level (NEIGHBOR_NODE, &advertise_interval_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_advertise_interval_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &as_path_allow_as_loop_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_as_path_allow_as_loop_cmd, CONFIG_LEVE_5, CMD_SYNC);
  	install_element_level (NEIGHBOR_NODE, &as_path_exclude_private_as_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_as_path_exclude_private_as_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &ipv4_family_vpnv4_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_ipv4_family_vpnv4_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &ipv4_family_vpn_instance_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_ipv4_family_vpn_instance_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &binding_tunnel_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE, &no_binding_tunnel_cmd, CONFIG_LEVE_5, CMD_SYNC);
		
	install_element_level (BGP_VPN_INSTANCE_NODE, &bgp_redistribute_vrf_ipv4_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_VPN_INSTANCE_NODE, &bgp_redistribute_vrf_ipv4_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_VPN_INSTANCE_NODE, &no_bgp_redistribute_vrf_ipv4_cmd, CONFIG_LEVE_5, CMD_SYNC);
		
	install_element_level (BGP_VPN_INSTANCE_NODE, &bgp_redistribute_vrf_roi_ipv4_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_VPN_INSTANCE_NODE, &bgp_redistribute_vrf_roi_ipv4_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_VPN_INSTANCE_NODE, &no_bgp_redistribute_vrf_roi_ipv4_cmd, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (BGP_IPV6_NODE, &bgp_redistribute_ipv4_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_IPV6_NODE, &bgp_redistribute_ipv4_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_IPV6_NODE, &no_bgp_redistribute_ipv4_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_IPV6_NODE, &bgp_redistribute_roi_ipv6_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_IPV6_NODE, &bgp_redistribute_roi_ipv6_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_IPV6_NODE, &no_bgp_redistribute_roi_ipv6_cmd, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (BGP_IPV6_NODE, &ipv6_aggregate_address_as_set_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (BGP_IPV6_NODE, &no_ipv6_aggregate_address_as_set_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (NEIGHBOR_NODE_IPV6, &neighbor_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &remote_as_cmd, CONFIG_LEVE_5, CMD_SYNC);
	//install_element_level (NEIGHBOR_NODE_IPV6, &bgp_auth_md5_password_cmd, CONFIG_LEVE_5, CMD_SYNC);
	//install_element_level (NEIGHBOR_NODE_IPV6, &no_bgp_auth_md5_password_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &source_address_ipv6_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &ebgp_multihop_ttl_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &no_ebgp_multihop_ttl_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &description_name_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &no_description_name_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &local_as_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &no_local_as_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &route_limit_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &no_route_limit_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &route_weight_bgp_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &no_route_weight_bgp_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &route_next_hop_change_local_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &no_route_next_hop_change_local_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &advertise_interval_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &no_advertise_interval_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &as_path_allow_as_loop_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &no_as_path_allow_as_loop_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &as_path_exclude_private_as_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &no_as_path_exclude_private_as_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &route_redistribute_default_route_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (NEIGHBOR_NODE_IPV6, &no_route_redistribute_default_route_cmd, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (BGP_NODE, &show_bgp_neighbors_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_neighbors_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_neighbors_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_neighbors_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_IPV6_NODE, &show_bgp_neighbors_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_neighbors_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	
	install_element_level (BGP_NODE, &show_bgp_neighbors_peer_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_neighbors_peer_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_neighbors_peer_cmd, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_neighbors_peer_cmd, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (BGP_IPV6_NODE, &show_bgp_neighbors_peer_cmd, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_neighbors_peer_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	
	install_element_level (BGP_NODE, &show_bgp_ipv4_paths_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_ipv4_paths_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_ipv4_paths_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_ipv4_paths_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_IPV6_NODE, &show_bgp_ipv4_paths_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_ipv4_paths_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	
	install_element_level (BGP_NODE, &show_bgp_ipv4_route_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_ipv4_route_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_ipv4_route_cmd, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_ipv4_route_cmd, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (BGP_IPV6_NODE, &show_bgp_ipv4_route_cmd, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_ipv4_route_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	
	install_element_level (BGP_NODE, &show_bgp_ipv4_route_neighbor_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_ipv4_route_neighbor_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_ipv4_route_neighbor_cmd, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_ipv4_route_neighbor_cmd, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (BGP_IPV6_NODE, &show_bgp_ipv4_route_neighbor_cmd, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_ipv4_route_neighbor_cmd, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (BGP_NODE, &show_bgp_ipv4_route_dampening_parameter_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_ipv4_route_dampening_parameter_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_ipv4_route_dampening_parameter_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_ipv4_route_dampening_parameter_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_IPV6_NODE, &show_bgp_ipv4_route_dampening_parameter_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_ipv4_route_dampening_parameter_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	
	install_element_level (BGP_NODE, &show_bgp_ipv4_vpn_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_ipv4_vpn_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_ipv4_vpn_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_ipv4_vpn_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_IPV6_NODE, &show_bgp_ipv4_vpn_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_ipv4_vpn_cmd, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (BGP_NODE, &show_bgp_ipv4_vpn_instance_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_ipv4_vpn_instance_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_ipv4_vpn_instance_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_ipv4_vpn_instance_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_IPV6_NODE, &show_bgp_ipv4_vpn_instance_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_ipv4_vpn_instance_cmd, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (BGP_NODE, &show_bgp_ipv4_vpn_statistics_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_ipv4_vpn_statistics_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_ipv4_vpn_statistics_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_ipv4_vpn_statistics_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_IPV6_NODE, &show_bgp_ipv4_vpn_statistics_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_ipv4_vpn_statistics_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	
	install_element_level (BGP_NODE, &show_bgp_ipv4_vpn_tags_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_ipv4_vpn_tags_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_ipv4_vpn_tags_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_ipv4_vpn_tags_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_IPV6_NODE, &show_bgp_ipv4_vpn_tags_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_ipv4_vpn_tags_cmd, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (BGP_NODE, &show_bgp_ipv6_neighbors_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_ipv6_neighbors_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_ipv6_neighbors_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_ipv6_neighbors_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_IPV6_NODE, &show_bgp_ipv6_neighbors_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_ipv6_neighbors_cmd, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (BGP_NODE, &show_bgp_ipv6_neighbors_peer_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_ipv6_neighbors_peer_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_ipv6_neighbors_peer_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_ipv6_neighbors_peer_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_IPV6_NODE, &show_bgp_ipv6_neighbors_peer_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_ipv6_neighbors_peer_cmd, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (BGP_NODE, &show_bgp_ipv6_route_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_bgp_ipv6_route_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_bgp_ipv6_route_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE_IPV6, &show_bgp_ipv6_route_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_IPV6_NODE, &show_bgp_ipv6_route_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_bgp_ipv6_route_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	
	install_element_level (CONFIG_NODE, &debug_bgp_fsm_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &no_debug_bgp_fsm_cmd, MANAGE_LEVE, CMD_LOCAL);	
	install_element_level (CONFIG_NODE, &debug_bgp_events_cmd, MANAGE_LEVE, CMD_LOCAL);	
	install_element_level (CONFIG_NODE, &no_debug_bgp_events_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &debug_bgp_packet_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &no_debug_bgp_packet_cmd, MANAGE_LEVE, CMD_LOCAL);
	
	install_element_level (BGP_NODE, &show_debugging_bgp_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_debugging_bgp_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE, &show_debugging_bgp_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (NEIGHBOR_NODE_IPV6, &show_debugging_bgp_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (BGP_IPV6_NODE, &show_debugging_bgp_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (BGP_VPN_INSTANCE_NODE, &show_debugging_bgp_cmd, MANAGE_LEVE, CMD_LOCAL);
	
	install_element_level (CONFIG_NODE, &bgpd_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &bgpd_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

	install_element_level (CONFIG_NODE, &bgp_show_timerinfo_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &bgp_conf_debug_cmd, MANAGE_LEVE, CMD_LOCAL);
}

