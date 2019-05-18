/**
 * \page cmds_ref_rip RIP
 * - \subpage modify_log_rip
 * 
 */
 
/**
 * \page modify_log_rip Modify Log
 * \section rip-v007r004 HiOS-V007R004
 *  -# 
 * \section rip-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "lib/ifm_common.h"

static struct cmd_node rip_node =
{
    RIP_NODE,
    "%s(config-rip)# "
};

DEFUNSH(VTYSH_RIPD,
        vtysh_exit_ripd,
        vtysh_exit_ripd_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}

DEFUNSH(VTYSH_RIPD, router_rip,
        router_rip_cmd_vtysh,
        "rip instance <1-255>",
        RIP_STR
        "specified an instance\n"
        "Instance number\n")
{
    vty->node = RIP_NODE;
    return CMD_SUCCESS;
}

DEFSH(VTYSH_RIPD,
      no_router_rip_cmd_vtysh,
      "no rip instance <1-255>",
      NO_STR
      RIP_STR
      "specified an instance\n"
      "Instance number\n")

DEFSH(VTYSH_RIPD, ip_rip_enable_cmd,
      "rip enable instance <1-255>",
      RIP_STR
      "Enable RIP on the interface\n"
      "specified an instance\n"
      "Instance number\n")

DEFSH(VTYSH_RIPD, no_ip_rip_enable_cmd,
      "no rip enable instance",
      NO_STR
      RIP_STR
      "Enable RIP on the interface\n"
      "specified an instance\n")

DEFSH(VTYSH_RIPD, rip_version_cmd,
      "version <1-2>",
      "Set RIP version\n"
      "version of RIP instance\n")

DEFSH(VTYSH_RIPD, no_rip_version_cmd,
      "no version",
      NO_STR
      "Set rip version to default\n")

DEFSH(VTYSH_RIPD, rip_distance_cmd,
      "distance <1-255>",
      "Set RIP route distance\n"
      "Distance value\n")

DEFSH(VTYSH_RIPD, no_rip_distance_cmd,
      "no distance",
      NO_STR
      "Set RIP route distance\n")

DEFSH(VTYSH_RIPD, rip_redistribute_instance_type_cmd,
      "redistribute (ospf|isis|rip) <1-255> {metric <0-15>}",
      REDIST_STR
      OSPF_STR 
      ISIS_STR 
      RIP_STR  
      "instance number 1-255\n"                  \
      "redistribute metric, range 0-15\n"         \
      "redistribute metric value\n")

DEFSH(VTYSH_RIPD, no_rip_redistribute_instance_type_cmd,
      "no redistribute (ospf|isis|rip) <1-255>",
      NO_STR
      REDIST_STR
      OSPF_STR 
      ISIS_STR 
      RIP_STR      
      "instance number 1-255\n")

DEFSH(VTYSH_RIPD, rip_redistribute_type_cmd,
      "redistribute (connected|static|ibgp|ebgp) {metric <0-15>}",
      REDIST_STR
      "Connected routes (directly attached subnet or host)\n" \
      "Statically configured routes\n" \
      "Inter Border Gateway Protocol (IBGP)\n" \
      "Exter Border Gateway Protocol (EBGP)\n" \
      "redistribute metric, range 0-15\n"         \
      "redistribute metric value\n")

DEFSH(VTYSH_RIPD, no_rip_redistribute_type_cmd,
      "no redistribute (connected|static|ibgp|ebgp)",
      NO_STR
      REDIST_STR
      "Connected routes (directly attached subnet or host)\n" \
      "Statically configured routes\n" \
      "Inter Border Gateway Protocol (IBGP)\n" \
      "Exter Border Gateway Protocol (EBGP)\n")

DEFSH(VTYSH_RIPD, rip_timers_cmd,
      "timers update <1-3600> age <1-3600> garbage-collect <1-3600>",
      "Set routing timers\n"
      "Basic routing protocol update timers\n"
      "Routing table update timer value in second. Default is 30.\n"
      "Basic routing protocol age timers\n"
      "Routing table update timer value in second. Default is 180.\n"
      "Basic routing protocol garbage-collect timers\n"
      "Routing table update timer value in second. Default is 120.\n")

DEFSH(VTYSH_RIPD, no_rip_timers_cmd,
      "no timers",
      NO_STR
      "Set routing timers\n")

DEFSH(VTYSH_RIPD, rip_summary_all_cmd,
      "summary",
      "RIP summary all subnet routes\n")

DEFSH(VTYSH_RIPD, no_rip_summary_all_cmd,
      "no summary",
      NO_STR
      "RIP summary all subnet routes\n")

DEFSH(VTYSH_RIPD, rip_neighbor_cmd,
      "rip neighbor A.B.C.D",
      RIP_STR
      "Specify a neighbor router\n"
      "IP address <neighbor>, e.g., 35.0.0.1\n")

DEFSH(VTYSH_RIPD, no_rip_neighbor_cmd,
      "no rip neighbor A.B.C.D",
      NO_STR
      RIP_STR
      "Specify a neighbor router\n"
      "IP address <neighbor>, e.g., 35.0.0.1\n")

DEFSH(VTYSH_RIPD, rip_passive_interface_cmd,
      "rip passive",
      RIP_STR
      "Suppress routing updates on this interface\n")

DEFSH(VTYSH_RIPD, no_rip_passive_interface_cmd,
      "no rip passive",
      NO_STR
      RIP_STR
      "Suppress routing updates on this interface\n")

DEFSH(VTYSH_RIPD, ip_rip_send_version_1_cmd,
      "rip version (1|2)",
      RIP_STR
      "Set RIP version\n"
      "RIP version 1 on interface\n"
      "RIP version 2 on interface\n")

DEFSH(VTYSH_RIPD, ip_rip_send_version_2_cmd,
      "rip version 2 (broadcast|multicast)",
      RIP_STR
      "Set RIP version\n"
      "RIP version 2 on interface\n"
      "RIP version 2 broadcast mode\n"
      "RIP version 2 multicast mode\n")

DEFSH(VTYSH_RIPD, no_ip_rip_send_version_cmd,
      "no rip version",
      NO_STR
      RIP_STR
      "Set RIP version\n")

DEFSH(VTYSH_RIPD, route_summary_cmd,
      "rip route-summary A.B.C.D/M",
      RIP_STR
      "Configure summary route on this interface\n"
      "summary prefix\n")

DEFSH(VTYSH_RIPD, no_route_summary_cmd,
      "no rip route-summary A.B.C.D/M",
      NO_STR
      RIP_STR
      "Configure summary route on this interface\n"
      "summary prefix\n")

DEFSH(VTYSH_RIPD, rip_default_metric_cmd,
      "rip metric <0-15>",
      RIP_STR
      "Set metric to inbound and outbound routes\n"
      "The value of metric adding to route")

DEFSH(VTYSH_RIPD, no_rip_default_metric_cmd,
      "no rip metric",
      NO_STR
      RIP_STR
      "Set metric to inbound and outbound routes\n")


DEFSH(VTYSH_RIPD, ip_rip_split_horizon_cmd_vtysh,
      "rip split-horizon",
      RIP_STR
      "Perform split horizon\n")

DEFSH(VTYSH_RIPD, ip_rip_split_horizon_poisoned_reverse_cmd_vtysh,
      "rip poisoned-reverse",
      RIP_STR
      "Perform poisoned-reverse\n")

DEFSH(VTYSH_RIPD, no_ip_rip_split_horizon_cmd_vtysh,
      "no rip split-horizon",
      NO_STR
      RIP_STR
      "Perform split horizon\n")

DEFSH(VTYSH_RIPD, no_ip_rip_split_horizon_poisoned_reverse_cmd_vtysh,
      "no rip poisoned-reverse",
      NO_STR
      RIP_STR
      "Perform poisoned-reverse\n")

DEFSH(VTYSH_RIPD, ip_rip_authentication_mode_cmd,
      "rip auth PASSWORD {md5 <1-255>}",
      RIP_STR
      "Authentication control\n"
      "Authentication password, length <1-16>\n"
      "MD5 authentication mode\n"
      "Keyed message digest\n")


DEFSH(VTYSH_RIPD, no_ip_rip_authentication_mode_cmd,
      "no rip auth",
      NO_STR
      RIP_STR
      "Authentication control\n")

DEFSH(VTYSH_RIPD, show_ip_rip_cmd_vtysh,
      "show rip <1-255> route",
      SHOW_STR
      RIP_STR
      "rip instance number\n"
      "Route information\n")

DEFSH(VTYSH_RIPD, show_ip_rip_status_cmd_vtysh,
      "show rip instance <1-255>",
      SHOW_STR
      RIP_STR
      "RIP routing protocol instance\n"
      "rip instance number\n")

DEFSH(VTYSH_RIPD, show_ip_rip_statistics_cmd,
      "show rip statistics",
      SHOW_STR
      RIP_STR
      "RIP statistics information\n")

DEFSH(VTYSH_RIPD,
      show_rip_neighbor_cmd,
      "show rip <1-255> neighbor [detail]",
      SHOW_STR
      RIP_STR
      "rip instance number\n"
      "neighbor information\n"
      "detail information\n")

DEFSH(VTYSH_RIPD, show_rip_interface_cmd,
      "show rip <1-255> interface (ethernet|gigabitethernet|xgigabitethernet) USP [detail]",
      SHOW_STR
      RIP_STR
      "rip instance number\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_ETHERNET_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      "The port/subport format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
      "show all rip information\n")

DEFSH (VTYSH_RIPD, show_rip_interface_vlan_cmd,
	   "show rip <1-255> interface vlanif <1-4094> [detail]",
	   SHOW_STR
	   RIP_STR
	   "rip instance number\n"
	   CLI_INTERFACE_STR
	   "Vlan interface\n"
	   "VLAN interface number\n"
	   "show all rip information\n")

DEFSH (VTYSH_RIPD, show_rip_interface_trunk_cmd,
	   "show rip <1-255> interface trunk TRUNK [detail]",
	   SHOW_STR
	   RIP_STR
	   "rip instance number\n"
	   CLI_INTERFACE_STR
	   "Trunk interface\n"
	   "The port/subport of trunk, format: <1-128>[.<1-4095>]\n"
	   "show all rip information\n")

DEFSH (VTYSH_RIPD, show_rip_interface_loopback_cmd,
	   "show rip <1-255> interface loopback <0-128> [detail]",
	   SHOW_STR
	   RIP_STR
	   "rip instance number\n"
	   CLI_INTERFACE_STR
	   "LoopBack interface\n"
	   "LoopBack interface number\n"
	   "show all rip information\n")

DEFSH(VTYSH_RIPD, show_rip_interface_all_cmd,
      "show rip <1-255> interface [detail]",
      SHOW_STR
      RIP_STR
      "rip instance number\n"
      CLI_INTERFACE_STR)

#if 0
DEFSH(VTYSH_RIPD, debug_rip_events_cmd_vtysh,
      "debug rip fsm",
      DEBUG_STR
      RIP_STR
      "RIP function status machine\n")

DEFSH(VTYSH_RIPD, no_debug_rip_events_cmd,
      "no debug rip fsm",
      NO_STR
      DEBUG_STR
      RIP_STR
      "RIP function status machine\n")

DEFSH(VTYSH_RIPD, debug_rip_packet_cmd_vtysh,
      "debug rip packet",
      DEBUG_STR
      RIP_STR
      "RIP packet\n")

DEFSH(VTYSH_RIPD, no_debug_rip_packet_cmd,
      "no debug rip packet",
      NO_STR
      DEBUG_STR
      RIP_STR
      "RIP packet\n")

DEFSH(VTYSH_RIPD, show_debugging_rip_cmd,
      "show rip debug",
      SHOW_STR
      RIP_STR
      DEBUG_STR)
#endif

DEFSH(VTYSH_RIPD, rip_debug_monitor_cmd,
	"debug rip (enable|disable) (event|packet|v6-event|v6-packet|v6-zebra|all|v6-all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Event debug messege\n"
	"Packet debug messege\n"
	"ripng event debug messege\n"
	"ripng packet debug messege\n"
	"ripng zebra debug messege\n"
	"rip all debug messege\n"
	"ripng all debug messege\n")


DEFSH(VTYSH_RIPD, show_rip_debug_monitor_cmd,
	"show rip debug",
	SHOW_STR
	"Programe name\n"
	"Debug status\n")


DEFSH(VTYSH_RIPD, ripd_log_level_ctl_cmd_vtysh,
	"debug ripd(enable | disable)",
    "Output log of debug level\n"
    "Program name\n"
    "Enable\n"
    "Disable\n")
/* debug log 发送到 syslog 使能状态显示 */
DEFSH(VTYSH_RIPD, ripd_show_log_level_ctl_cmd_vtysh,
	"show debug ripd",
	SHOW_STR
	"Output log of debug level\n"
	"Program name\n")

void
vtysh_init_ripd_cmd(void)
{
    install_node(&rip_node, NULL);
    vtysh_install_default(RIP_NODE);
	
    install_element_level(RIP_NODE, &vtysh_exit_ripd_cmd,  VISIT_LEVE, CMD_SYNC);
    /* rip cmd*/
    install_element_level(CONFIG_NODE, &router_rip_cmd_vtysh, VISIT_LEVE, CMD_SYNC);
    install_element_level(CONFIG_NODE, &no_router_rip_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(RIP_NODE, &rip_version_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIP_NODE, &no_rip_version_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIP_NODE, &rip_distance_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIP_NODE, &no_rip_distance_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIP_NODE, &rip_redistribute_instance_type_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIP_NODE, &no_rip_redistribute_instance_type_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIP_NODE, &rip_redistribute_type_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIP_NODE, &no_rip_redistribute_type_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIP_NODE, &rip_timers_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIP_NODE, &no_rip_timers_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIP_NODE, &rip_summary_all_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIP_NODE, &no_rip_summary_all_cmd, CONFIG_LEVE_5, CMD_SYNC);

    /* install cmd sub interface */
	install_element_level(PHYSICAL_SUBIF_NODE, &ip_rip_split_horizon_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &ip_rip_split_horizon_poisoned_reverse_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &no_ip_rip_split_horizon_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &no_ip_rip_split_horizon_poisoned_reverse_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &ip_rip_authentication_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &no_ip_rip_authentication_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &ip_rip_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &no_ip_rip_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &route_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &no_route_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &rip_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &no_rip_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &rip_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &no_rip_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &rip_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &no_rip_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &ip_rip_send_version_1_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &ip_rip_send_version_2_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &no_ip_rip_send_version_cmd, CONFIG_LEVE_5, CMD_SYNC);

	/***install cmd in if interface** */
	install_element_level(PHYSICAL_IF_NODE, &ip_rip_split_horizon_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &ip_rip_split_horizon_poisoned_reverse_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_ip_rip_split_horizon_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_ip_rip_split_horizon_poisoned_reverse_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &ip_rip_authentication_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_ip_rip_authentication_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &ip_rip_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_ip_rip_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &route_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_route_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &rip_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_rip_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &rip_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_rip_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &rip_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_rip_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &ip_rip_send_version_1_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &ip_rip_send_version_2_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_ip_rip_send_version_cmd, CONFIG_LEVE_5, CMD_SYNC);

	/* install cmd in trunk interface */
	install_element_level(TRUNK_IF_NODE, &ip_rip_split_horizon_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &ip_rip_split_horizon_poisoned_reverse_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_ip_rip_split_horizon_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_ip_rip_split_horizon_poisoned_reverse_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &ip_rip_authentication_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_ip_rip_authentication_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &ip_rip_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_ip_rip_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &route_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_route_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &rip_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_rip_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &rip_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_rip_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &rip_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_rip_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &ip_rip_send_version_1_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &ip_rip_send_version_2_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_ip_rip_send_version_cmd, CONFIG_LEVE_5, CMD_SYNC);
	
	/* isntall cmd in sub trunk interface */
	install_element_level(TRUNK_SUBIF_NODE, &ip_rip_split_horizon_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &ip_rip_split_horizon_poisoned_reverse_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &no_ip_rip_split_horizon_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &no_ip_rip_split_horizon_poisoned_reverse_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &ip_rip_authentication_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &no_ip_rip_authentication_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &ip_rip_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &no_ip_rip_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &route_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &no_route_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &rip_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &no_rip_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &rip_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &no_rip_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &rip_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &no_rip_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &ip_rip_send_version_1_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &ip_rip_send_version_2_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE, &no_ip_rip_send_version_cmd, CONFIG_LEVE_5, CMD_SYNC);

	/* isntall cmd in vlan interface */
	install_element_level(VLANIF_NODE, &ip_rip_split_horizon_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &ip_rip_split_horizon_poisoned_reverse_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &no_ip_rip_split_horizon_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &no_ip_rip_split_horizon_poisoned_reverse_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &ip_rip_authentication_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &no_ip_rip_authentication_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &ip_rip_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &no_ip_rip_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &route_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &no_route_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &rip_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &no_rip_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &rip_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &no_rip_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &rip_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &no_rip_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &ip_rip_send_version_1_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &ip_rip_send_version_2_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &no_ip_rip_send_version_cmd, CONFIG_LEVE_5, CMD_SYNC);

	/* isntall cmd in loopback interface */
	install_element_level(LOOPBACK_IF_NODE, &ip_rip_split_horizon_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &ip_rip_split_horizon_poisoned_reverse_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &no_ip_rip_split_horizon_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &no_ip_rip_split_horizon_poisoned_reverse_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &ip_rip_authentication_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &no_ip_rip_authentication_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &ip_rip_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &no_ip_rip_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &route_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &no_route_summary_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &rip_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &no_rip_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &rip_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &no_rip_neighbor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &rip_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &no_rip_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &ip_rip_send_version_1_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &ip_rip_send_version_2_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &no_ip_rip_send_version_cmd, CONFIG_LEVE_5, CMD_SYNC);

	/*show command*/
	install_element_level(CONFIG_NODE, &show_ip_rip_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ip_rip_status_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_rip_interface_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_rip_interface_vlan_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_rip_interface_trunk_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_rip_interface_loopback_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_rip_interface_all_cmd, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level(CONFIG_NODE, &show_ip_rip_statistics_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_rip_neighbor_cmd, MONITOR_LEVE_2, CMD_LOCAL);

#if 0
	install_element_level(CONFIG_NODE, &show_debugging_rip_cmd,  MANAGE_LEVE, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &debug_rip_events_cmd_vtysh,  MANAGE_LEVE, CMD_SYNC);
	install_element_level(CONFIG_NODE, &debug_rip_packet_cmd_vtysh,  MANAGE_LEVE, CMD_SYNC);

	install_element_level(CONFIG_NODE, &no_debug_rip_events_cmd,  MANAGE_LEVE, CMD_SYNC);
	install_element_level(CONFIG_NODE, &no_debug_rip_packet_cmd,  MANAGE_LEVE, CMD_SYNC);
#endif
	install_element_level(CONFIG_NODE, &show_rip_debug_monitor_cmd,  MANAGE_LEVE, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &rip_debug_monitor_cmd,  MANAGE_LEVE, CMD_LOCAL);

	install_element_level(CONFIG_NODE, &ripd_log_level_ctl_cmd_vtysh,  MANAGE_LEVE, CMD_SYNC);
	install_element_level(CONFIG_NODE, &ripd_show_log_level_ctl_cmd_vtysh,  MANAGE_LEVE, CMD_LOCAL);
}

