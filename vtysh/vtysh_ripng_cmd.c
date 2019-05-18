/**
 * \page cmds_ref_ripng RIPng
 * - \subpage modify_log_ripng
 * 
 */
 
/**
 * \page modify_log_ripng Modify Log
 * \section ripng-v007r004 HiOS-V007R004
 *  -# 
 * \section ripng-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "lib/ifm_common.h"


static struct cmd_node ripng_node =
{
    RIPNG_NODE,
    "%s(config-ripng)# "
};

DEFUNSH(VTYSH_RIPD,
        vtysh_exit_ripngd,
        vtysh_exit_ripngd_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}

DEFUNSH(VTYSH_RIPD, router_ripng,
        router_ripng_cmd,
        "ripng instance <1-255>",
        RIPNG_STR
        "instance configure\n"
        "instance number <1-255>\n")
{
    vty->node = RIPNG_NODE;
    return CMD_SUCCESS;
}

DEFSH(VTYSH_RIPD,
      no_router_ripng_cmd,
      "no ripng instance <1-255>",
      NO_STR
      RIPNG_STR
      "instance configure\n"
      "instance number <1-255>\n")

DEFSH(VTYSH_RIPD,
      ipv6_ripng_enable_cmd,
      "ripng enable instance <1-255>",
      RIPNG_STR
      "Enable RIPng on the interface\n"
      "specified an instance\n"
      "Instance number\n")

DEFSH(VTYSH_RIPD,
      no_ipv6_ripng_enable_cmd,
      "no ripng enable instance",
      NO_STR
      RIPNG_STR
      "Enable RIPng on the interface\n"
      "specified an instance\n")

DEFSH(VTYSH_RIPD, show_ipv6_ripng_cmd,
      "show ripng <1-255> route",
      SHOW_STR
      RIPNG_STR
      "instance number\n"
      "Route information\n")

DEFSH(VTYSH_RIPD, show_ipv6_ripng_status_cmd,
	  "show ripng instance <1-255>",
	  SHOW_STR
	  RIPNG_STR
	  "RIPng routing protocol instance\n"
	  "instance number <1-255>\n")

DEFSH(VTYSH_RIPD, show_ripng_interface_cmd,
      "show ripng <1-255> interface (ethernet|gigabitethernet|xgigabitethernet) USP",
      SHOW_STR
      RIPNG_STR
      "ripng instance number\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_ETHERNET_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      "The port/subport format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")

DEFSH(VTYSH_RIPD, show_ripng_interface_all_cmd,
      "show ripng <1-255> interface",
      SHOW_STR
      RIPNG_STR
      "ripng instance number\n"
      CLI_INTERFACE_STR)

DEFSH(VTYSH_RIPD, show_ipv6_ripng_statistics_cmd,
      "show ripng statistics",
      SHOW_STR
      RIPNG_STR
      "RIPNG statistics information\n")

DEFSH(VTYSH_RIPD, ripng_aggregate_address_cmd,
      "aggregate-address X:X::X:X/M",
      "Set aggregate RIPng route announcement\n"
      "Aggregate network\n")

DEFSH(VTYSH_RIPD, no_ripng_aggregate_address_cmd,
      "no aggregate-address X:X::X:X/M",
      NO_STR
      "Delete aggregate RIPng route announcement\n"
      "Aggregate network")

DEFSH(VTYSH_RIPD, ripng_default_metric_cmd,
      "ripng metric <0-15>",
      RIPNG_STR
      "Set metric to inbound and outbound routes\n"
      "The value of metric adding to route")

DEFSH(VTYSH_RIPD, no_ripng_default_metric_cmd,
      "no ripng metric",
      NO_STR
      RIPNG_STR
      "Set metric to inbound and outbound routes\n")

DEFSH(VTYSH_RIPD, ripng_timers_cmd,
      "timers <0-3600> <0-3600> <0-3600>",
      "Set RIPng timers\n"
      "Routing table update timer value in second. Default is 30.\n"
      "Routing information timeout timer. Default is 180.\n"
      "Garbage collection timer. Default is 120.\n")

DEFSH(VTYSH_RIPD, no_ripng_timers_cmd,
      "no timers",
      NO_STR
      "Set RIPng timers\n")

DEFSH(VTYSH_RIPD, ripng_distance_cmd,
      "distance <1-255>",
      "Set RIPng distance\n"
      "Distance value\n")

DEFSH(VTYSH_RIPD, no_ripng_distance_cmd,
      "no distance",
      NO_STR
      "Set RIPng distance\n")

DEFSH(VTYSH_RIPD, ipv6_ripng_split_horizon_cmd,
      "ripng split-horizon",
      RIPNG_STR
      "Perform split horizon\n")

DEFSH(VTYSH_RIPD, ipv6_ripng_split_horizon_poisoned_reverse_cmd,
      "ripng poisoned-reverse",
      RIPNG_STR
      "Perform poisoned-reverse\n")

DEFSH(VTYSH_RIPD, no_ipv6_ripng_split_horizon_cmd,
      "no ripng split-horizon",
      NO_STR
      RIPNG_STR
      "Perform split horizon\n")

DEFSH(VTYSH_RIPD, no_ipv6_ripng_split_horizon_poisoned_reverse_cmd,
      "no ripng poisoned-reverse",
      NO_STR
      RIPNG_STR
      "Perform poisoned-reverse\n")

DEFSH(VTYSH_RIPD, ripng_passive_interface_cmd,
      "ripng passive",
      RIPNG_STR
      "Suppress routing updates on this interface\n")

DEFSH(VTYSH_RIPD, no_ripng_passive_interface_cmd,
      "no ripng passive",
      NO_STR
      RIPNG_STR
      "Suppress routing updates on this interface\n")

DEFSH(VTYSH_RIPD, ripng_redistribute_instance_type_cmd,
      "redistribute (ospf6|isis6|ripng) <1-255> {metric <0-15>}",
      REDIST_STR
      OSPF6_STR 
      "Intermediate System to Intermediate System for ipv6 (IS-IS6)\n" \
      "Routing Information Protocol next generation (RIPng)\n"     \
      "instance number 1-255\n"                  \
      "redistribute metric, range 0-15\n"         \
      "redistribute metric value\n")

DEFSH(VTYSH_RIPD, no_ripng_redistribute_instance_type_cmd,
      "no redistribute (ospf6|isis6|ripng) <1-255>",
      NO_STR
      REDIST_STR
      OSPF6_STR 
      "Intermediate System to Intermediate System for ipv6 (IS-IS6)\n" \
      "Routing Information Protocol (RIPng)\n"      \
      "instance number 1-255\n")

DEFSH(VTYSH_RIPD, ripng_redistribute_type_cmd,
      "redistribute (connected|static|ibgp|ebgp) {metric <0-15>}",
      REDIST_STR
      "Connected routes (directly attached subnet or host)\n" \
      "Statically configured routes\n" \
      "Inter Border Gateway Protocol for ipv6 (IBGP)\n" \
      "Exter Border Gateway Protocol for ipv6 (EBGP)\n" \
      "redistribute metric, range 0-15\n"         \
      "redistribute metric value\n")

DEFSH(VTYSH_RIPD, no_ripng_redistribute_type_cmd,
      "no redistribute (connected|static|ibgp|ebgp)",
      NO_STR
      REDIST_STR
      "Connected routes (directly attached subnet or host)\n" \
      "Statically configured routes\n" \
      "Inter Border Gateway Protocol for ipv6 (IBGP)\n" \
      "Exter Border Gateway Protocol for ipv6 (EBGP)\n")

#if 0
DEFSH(VTYSH_RIPD, show_debugging_ripng_cmd,
   	  "show ripng debug",
   	  SHOW_STR
   	  DEBUG_STR
   	  RIPNG_STR)

DEFSH(VTYSH_RIPD, debug_ripng_events_cmd,
	  "debug ripng fsm",
	  DEBUG_STR
      RIPNG_STR
	  "Debug option set for ripng fsm\n")

DEFSH(VTYSH_RIPD, debug_ripng_packet_cmd,
      "debug ripng packet",
      DEBUG_STR
	  RIPNG_STR
      "Debug option set for ripng packet\n")

DEFSH(VTYSH_RIPD, no_debug_ripng_events_cmd,
	  "no debug ripng events",
	  NO_STR
	  DEBUG_STR
	  RIPNG_STR
	  "Debug option set for ripng events\n")

DEFSH(VTYSH_RIPD, no_debug_ripng_packet_cmd,
	  "no debug ripng packet",
	  NO_STR
	  DEBUG_STR
	  RIPNG_STR
	  "Debug option set for ripng packet\n")

#endif

DEFSH(VTYSH_RIPD, show_ripng_neighbor_cmd,
      "show ripng <1-255> neighbor [detail]",
      SHOW_STR
      RIPNG_STR
      "ripng instance number\n"
      "neighbor information\n"
      "detail information\n")

DEFSH(VTYSH_RIPD, ripngd_log_level_ctl_cmd_vtysh, "debug ripngd(enable | disable)",
      "Output log of debug level\n""Program name\n""Enable\n""Disable\n")
/* debug log 发送到 syslog 使能状态显示 */
DEFSH(VTYSH_RIPD, ripngd_show_log_level_ctl_cmd_vtysh,    "show debug ripngd",
      SHOW_STR"Output log of debug level\n""Program name\n")

void
vtysh_init_ripngd_cmd(void)
{
    install_node(&ripng_node, NULL);
    vtysh_install_default(RIPNG_NODE);
    install_element_level(RIPNG_NODE, &vtysh_exit_ripngd_cmd, VISIT_LEVE, CMD_SYNC);

    install_element_level(CONFIG_NODE, &router_ripng_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level(CONFIG_NODE, &no_router_ripng_cmd, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(RIPNG_NODE, &ripng_aggregate_address_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIPNG_NODE, &no_ripng_aggregate_address_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIPNG_NODE, &ripng_redistribute_instance_type_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIPNG_NODE, &no_ripng_redistribute_instance_type_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIPNG_NODE, &ripng_redistribute_type_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIPNG_NODE, &no_ripng_redistribute_type_cmd, CONFIG_LEVE_5, CMD_SYNC);

//    install_element_level(RIPNG_NODE, &ripng_default_metric_cmd, CONFIG_LEVE_5);
//    install_element_level(RIPNG_NODE, &no_ripng_default_metric_cmd, CONFIG_LEVE_5);
    install_element_level(RIPNG_NODE, &ripng_timers_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIPNG_NODE, &no_ripng_timers_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIPNG_NODE, &ripng_distance_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(RIPNG_NODE, &no_ripng_distance_cmd, CONFIG_LEVE_5, CMD_SYNC);

    /* install cmd to interface */
    install_element_level(PHYSICAL_IF_NODE, &ipv6_ripng_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &no_ipv6_ripng_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &ipv6_ripng_split_horizon_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &ipv6_ripng_split_horizon_poisoned_reverse_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &no_ipv6_ripng_split_horizon_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &no_ipv6_ripng_split_horizon_poisoned_reverse_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &ripng_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &no_ripng_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &ripng_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_ripng_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);

    /* install cmd to sub interface */
    install_element_level(PHYSICAL_SUBIF_NODE, &ipv6_ripng_split_horizon_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_SUBIF_NODE, &ipv6_ripng_split_horizon_poisoned_reverse_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_SUBIF_NODE, &no_ipv6_ripng_split_horizon_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_SUBIF_NODE, &no_ipv6_ripng_split_horizon_poisoned_reverse_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_SUBIF_NODE, &ripng_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_SUBIF_NODE, &no_ripng_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_SUBIF_NODE, &ipv6_ripng_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_SUBIF_NODE, &no_ipv6_ripng_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &ripng_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &no_ripng_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);

    /* install cmd to trunk interface */
    install_element_level(TRUNK_IF_NODE, &ipv6_ripng_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(TRUNK_IF_NODE, &no_ipv6_ripng_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(TRUNK_IF_NODE, &ipv6_ripng_split_horizon_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(TRUNK_IF_NODE, &ipv6_ripng_split_horizon_poisoned_reverse_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(TRUNK_IF_NODE, &no_ipv6_ripng_split_horizon_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(TRUNK_IF_NODE, &no_ipv6_ripng_split_horizon_poisoned_reverse_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(TRUNK_IF_NODE, &ripng_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(TRUNK_IF_NODE, &no_ripng_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &ripng_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE, &no_ripng_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
#if 0
    /* install cmd to vlanif interface */
    install_element_level(VLANIF_NODE, &ipv6_ripng_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(VLANIF_NODE, &no_ipv6_ripng_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(VLANIF_NODE, &ipv6_ripng_split_horizon_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(VLANIF_NODE, &ipv6_ripng_split_horizon_poisoned_reverse_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(VLANIF_NODE, &no_ipv6_ripng_split_horizon_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(VLANIF_NODE, &no_ipv6_ripng_split_horizon_poisoned_reverse_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(VLANIF_NODE, &ripng_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(VLANIF_NODE, &no_ripng_passive_interface_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &ripng_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE, &no_ripng_default_metric_cmd, CONFIG_LEVE_5, CMD_SYNC);
#endif
    /* install show cmd*/
    install_element_level(CONFIG_NODE, &show_ipv6_ripng_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ipv6_ripng_status_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ipv6_ripng_statistics_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ripng_interface_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ripng_interface_all_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ripng_neighbor_cmd, MONITOR_LEVE_2, CMD_LOCAL);

#if 0
    install_element_level(CONFIG_NODE, &show_debugging_ripng_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &debug_ripng_events_cmd, MANAGE_LEVE, CMD_SYNC);
    install_element_level(CONFIG_NODE, &debug_ripng_packet_cmd, MANAGE_LEVE, CMD_SYNC);
    install_element_level(CONFIG_NODE, &no_debug_ripng_events_cmd, MANAGE_LEVE, CMD_SYNC);
    install_element_level(CONFIG_NODE, &no_debug_ripng_packet_cmd, MANAGE_LEVE, CMD_SYNC);
#endif
	
	install_element_level(CONFIG_NODE, &ripngd_log_level_ctl_cmd_vtysh,  MANAGE_LEVE, CMD_SYNC);
	install_element_level(CONFIG_NODE, &ripngd_show_log_level_ctl_cmd_vtysh,  MANAGE_LEVE, CMD_SYNC);
}
