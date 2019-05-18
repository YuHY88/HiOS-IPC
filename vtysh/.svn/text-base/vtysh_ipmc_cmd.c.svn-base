/**
 * \page cmds_ref_ipmc IPMC
 * - \subpage modify_log_ipmc
 * 
 */
 
/**
 * \page modify_log_ipmc Modify Log
 * \section ipmc-v007r004 HiOS-V007R004
 *  -# 
 * \section ipmc-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "ifm_common.h"

#include <ipmc/pim/pim_cmd.h>
#include <ipmc/igmp/igmp_cmd.h>
#if 0
struct cmd_node pim_instance_node =
{
    PIM_INSTANCE_NODE,
    "%s(config-pim-instance)# ",
    1
};
#endif
static struct cmd_node igmp_node =
{ 
	IGMP_NODE,  
	"%s(config-igmp)# ",  
	1, 
};	
#if 0
DEFUNSH (VTYSH_IPMC,
        pim_instance_vtysh,
        pim_instance_cmd,
        "pim instance <1-255>",
        PIM_STR
		"PIM instance view\n"
		"PIM instance id, <1-255>\n"
        )
{
    uint8_t pim_id = 0;
    char *pprompt = NULL;

    /*判断输入参数是否合法*/
    VTY_GET_INTEGER_RANGE ( "pim_instance", pim_id, argv[0], 1, 255 );
	
    /*进入会话视图*/
    vty->node = PIM_INSTANCE_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt [pim-instance-xx]*/
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-pim-instance-%d)#", pim_id);
    }

    vty->index = pim_id;

    return CMD_SUCCESS;
}

DEFUNSH (VTYSH_IPMC,
        pim_instance_exit_vtysh,
        pim_instance_exit_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}

DEFSH (VTYSH_IPMC,
	undo_pim_instance_cmd,
	"no pim instance <1-255>",
	NO_STR
	PIM_STR
	"PIM instance\n"
	"PIM instance id, <1-255>\n"
	)
#endif
DEFSH (VTYSH_IPMC,
	pim_debug_cmd,
	"debug pim packet",
	DEBUG_STR
	PIM_STR
	"pim packet\n"
	)

DEFSH (VTYSH_IPMC,
	undo_pim_debug_cmd,
	"no debug pim packet",
	NO_STR
	DEBUG_STR
	PIM_STR
	"pim packet\n"
	)
	
DEFSH(VTYSH_IPMC,
	pim_ssm_cmd,
	"pim ssm",
	PIM_STR
	"Configure Source Specific Multicast\n"
	)


DEFSH(VTYSH_IPMC,
	undo_pim_ssm_cmd,
	"no pim ssm",
	NO_STR
	PIM_STR
	"Configure Source Specific Multicast\n"
	)
	
DEFSH (VTYSH_IPMC,
	pim_debug_mfib_cmd,
	"pim switch-entry source A.B.C.D group A.B.C.D iif interface gigabitethernet USP oif interface gigabitethernet USP ",
	PIM_STR
	"Switch-entry\n"
	"Source Address, Unicast address\n"
	"Format A.B.C.D\n"
	"Group Address ,Mulitcast address\n"
	"In port\n"
	CLI_INTERFACE_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	"Out port\n"
	CLI_INTERFACE_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_STR
    CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		)

DEFSH (VTYSH_IPMC,
	pim_instance_mcif_add_cmd,
	"pim (sm|dm)",
	PIM_STR
	"pim sparse mode\n"
	"PIM dense mode\n"
	)

DEFSH (VTYSH_IPMC,
	pim_instance_mcif_del_cmd,
	"no pim (sm|dm)",
	NO_STR
	PIM_STR
	"pim sparse mode\n"
	"PIM dense mode\n"
	)
#if 0
DEFSH (VTYSH_IPMC,
	pim_instance_mode_cmd,
	"pim (sm|dm)",
	PIM_STR
	"pim sparse mode\n"
	"PIM dense mode\n"
	)

DEFSH (VTYSH_IPMC,
	undo_pim_instance_mode_cmd,
	"no pim (sm|dm)",
	NO_STR
	PIM_STR
	"pim sparse mode\n"
	"PIM dense mode\n"
	)
#endif
DEFSH (VTYSH_IPMC,
	pim_instance_hello_interval_cmd,
	"pim hello interval <1-3600>",
	PIM_STR
	"pim hello message\n"
	"pim hello message interval\n"
	"interval value, <1-3600>, default 30(s)"
	)

DEFSH (VTYSH_IPMC,
	undo_pim_instance_hello_interval_cmd,
	"no pim hello interval",
	NO_STR
	PIM_STR
	"pim hello message\n"
	"pim hello message interval\n"
	)

DEFSH (VTYSH_IPMC,
	pim_instance_dr_priority_cmd,
	"pim dr priority <0-255>",
	PIM_STR
	"pim DR(Designated Router)\n"
	"pim DR priority\n"
	"priority value, <0-255>, default 1"
	)

DEFSH (VTYSH_IPMC,
	undo_pim_instance_dr_priority_cmd,
	"no pim dr priority",
	NO_STR
	PIM_STR
	"pim DR(Designated Router)\n"
	"pim DR priority\n"
	)

DEFSH (VTYSH_IPMC,
	pim_instance_static_rp_cmd,
	"pim static-rp A.B.C.D",
	PIM_STR
	"pim static RP(Rendezvous Point)\n"
	"RP address, IP Unicast address\n"
	)

DEFSH (VTYSH_IPMC,
	undo_pim_instance_static_rp_cmd,
	"no pim static-rp A.B.C.D",
	NO_STR
	PIM_STR
	"pim static RP(Rendezvous Point)\n"
	"RP address, IP Unicast address\n"
	)

DEFSH (VTYSH_IPMC,
	pim_instance_jp_interval_cmd,
	"pim join-prune interval <1-600>",
	PIM_STR
	"pim join/prune message\n"
	"pim join/prune message send interval\n"
	"interval value, <1-600>, default 30(s)\n"
	)

DEFSH(VTYSH_IPMC,
	undo_pim_instance_jp_interval_cmd,
	"no pim join-prune interval",
	NO_STR
	PIM_STR
	"pim join/prune message\n"
	"pim join/prune message send interval\n"
	)
	
DEFSH(VTYSH_IPMC,
	pim_mrt_show_cmd,
	"show pim routing-table",
	SHOW_STR
	PIM_STR
	"PIM routing information\n"
	)
	
DEFSH(VTYSH_IPMC,
	pim_if_nbr_show_cmd,
	"show pim interface neighbor",
	SHOW_STR
	PIM_STR
	"interface\n"
	"pim neighbor information\n"
	)
	
DEFUNSH (VTYSH_IPMC,
	igmp_instance_cfg_vtysh,
	igmp_instance_cfg_cmd,
	"igmp instance [<1-128>]",
	"Igmp instance command\n"
	"Igmp instance\n"
	"Igmp instance id\n")
{
	char *pprompt = NULL;
	uint32_t vpn = 0;

	if (NULL != argv[0])
	{
		vpn = atoi(argv[0]);
	}
	
	/*进入会话视图*/
    vty->node = IGMP_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%s(config-igmp-instance-%d)# ", "huahuan", vpn);
	}
	
	vty->index = (void *)vpn;
	
	return CMD_SUCCESS;
}

DEFSH (VTYSH_IPMC, no_igmp_instance_cmd_vtysh,
	"no igmp instance [<1-128>]",
	"No igmp instance command\n"
	"Igmp\n"
	"Igmp instance\n"
	"Igmp instance id:1-128\n")

DEFUNSH (VTYSH_IPMC,
	 vtysh_exit_igmp,
	 vtysh_exit_igmp_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}

DEFSH (VTYSH_IPMC,
	igmp_inst_version_cmd_vtysh,
	"version (1|2|3)",
	"Igmp vpn instance version command\n"
	"Igmp vpn instance version1\n"
	"Igmp vpn instance version2\n"
	"Igmp vpn instance version3\n")

DEFSH (VTYSH_IPMC,
	no_igmp_inst_version_cmd_vtysh,
	"no version",
	"No command\n"
	"Igmp version default\n")

DEFSH (VTYSH_IPMC,
	igmp_inst_querier_keepalive_cmd_vtysh,
	"other-querier keepalive <60-300>",
	"Igmp other querier keepalive command\n"
	"Igmp querier keepalive\n"
	"Igmp querier keepalive time\n")

DEFSH (VTYSH_IPMC,
	no_igmp_inst_querier_keepalive_cmd_vtysh,
	"no other-querier keepalive",
	"No igmp other-querier keepalive command\n"
	"Igmp other-querier\n"
	"Igmp other-querier keepalive\n")

DEFSH (VTYSH_IPMC,
	igmp_inst_query_interval_cmd_vtysh,
	"query interval <1-18000>",
	"Igmp query interval command\n"
	"Igmp query interval\n"
	"Igmp query interval seconds:1-18000\n")

DEFSH (VTYSH_IPMC,
	no_igmp_inst_query_interval_cmd_vtysh,
	"no query interval",
	"No igmp query interval command\n"
	"Igmp query\n"
	"Igmp query interval\n")

DEFSH (VTYSH_IPMC,
	igmp_isnt_robust_count_cmd_vtysh,
	"robust-count <2-5>",
	"Igmp robust count command\n"
	"Igmp robust-count number:2-5\n")

DEFSH (VTYSH_IPMC,
	no_igmp_inst_robust_count_cmd_vtysh,
	"no roubust-count",
	"No roubust count command\n"
	"Igmp roubust-count\n")

DEFSH (VTYSH_IPMC,
	igmp_inst_last_member_query_interval_cmd_vtysh,
	"last member-query interval <1-5>",
	"Igmp last member query interval command\n"
	"Igmp last member query\n"
	"Igmp last member query interval\n"
	"Igmp last member query interval seconds:1-5\n")

DEFSH (VTYSH_IPMC,
	no_igmp_inst_last_member_query_interval_cmd_vtysh,
	"no last member-query interval",
	"No igmp last member query interval command\n"
	"Igmp last member\n"
	"Igmp last member query\n"
	"Igmp last member query interval seconds:1-5\n")

DEFSH (VTYSH_IPMC,
	igmp_inst_max_resp_time_cmd_vtysh,
	"max-response-time <1-25>",
	"Igmp max response time command\n"
	"Igmp max response time seconds:1-25\n")

DEFSH (VTYSH_IPMC,
	no_igmp_inst_max_resp_time_cmd_vtysh,
	"no max-response-time",
	"No igmp max response time command\n"
	"Igmp max response time\n")

DEFSH (VTYSH_IPMC,
	igmp_inst_send_router_alert_cmd_vtysh,
	"send-router-alter",
	"Igmp send router alter enable command\n")

DEFSH (VTYSH_IPMC,
	no_igmp_inst_send_router_alert_cmd_vtysh,
	"no send-router-alter",
	"No igmp send router alter enable command\n"
	"Igmp send router alter\n")

DEFSH (VTYSH_IPMC,
	igmp_inst_require_router_alert_cmd_vtysh,
	"require-router-alter",
	"Igmp require router alter enable command\n")

DEFSH (VTYSH_IPMC,
	no_igmp_inst_require_router_alert_cmd_vtysh,
	"no require-router-alter",
	"No igmp require router alter enable command\n"
	"Igmp require router alter\n")

DEFSH (VTYSH_IPMC,
	igmp_ssm_mapping_group_cmd_vtysh,
	"igmp ssm-mapping group A.B.C.D/<4-32> source A.B.C.D",
	"Igmp ssm-mapping group command\n"
	"Igmp ssm-mapping\n"
	"Igmp ssm-mapping group\n"
	"Igmp ssm-mapping group ip and mask\n"
	"Igmp ssm-mapping source\n"
	"Igmp ssm-mapping source ip\n")

DEFSH (VTYSH_IPMC,
	no_igmp_ssm_mapping_group_cmd_vtysh,
	"no igmp ssm-mapping group A.B.C.D/<4-32> source A.B.C.D",
	"No igmp ssm-mapping group command\n"
	"Igmp\n"
	"Igmp ssm-mapping\n"
	"Igmp ssm-mapping group\n"
	"Igmp ssm-mapping group ip and mask\n"
	"Igmp ssm-mapping source\n"
	"Igmp ssm-mapping source ip\n")

DEFSH (VTYSH_IPMC,
	igmp_enable_instance_cmd_vtysh,
	"igmp enable",
	"Igmp instance enable command\n"
	"Igmp enable\n")

DEFSH (VTYSH_IPMC,
	no_igmp_enable_instance_cmd_vtysh,
	"igmp disable",
	"Igmp disable command\n"
	"Igmp disable\n")

DEFSH (VTYSH_IPMC,
	igmp_version_cmd_vtysh,
	"igmp version (1|2|3)",
	"Igmp version command\n"
	"Igmp version\n"
	"Igmp version1\n"
	"Igmp version2\n"
	"Igmp version3\n")

DEFSH (VTYSH_IPMC,
	no_igmp_version_cmd_vtysh,
	"no igmp version",
	"No igmp version command\n"
	"Igmp\n"
	"Igmp version\n")

DEFSH (VTYSH_IPMC,
	igmp_port_static_group_cmd_vtysh,
	"igmp join group A.B.C.D {source A.B.C.D}",
	"Igmp port static group command\n"
	"Igmp join\n"
	"Igmp group\n"
	"Igmp multicast group ip\n"
	"Igmp source\n"
	"Igmp source ip\n")

DEFSH (VTYSH_IPMC,
	no_igmp_port_static_group_cmd_vtysh,
	"no igmp join group A.B.C.D {source A.B.C.D}",
	"No igmp port static group command\n"
	"Igmp\n"
	"Igmp join\n"
	"Igmp group\n"
	"Igmp multicast group ip and mask length\n"
	"Igmp source\n"
	"Igmp source ip\n")

DEFSH (VTYSH_IPMC,
	igmp_require_router_alert_cmd_vtysh,
	"igmp require-router-alertr",
	"Igmp require router alert enable command\n"
	"Igmp require router alert\n")

DEFSH (VTYSH_IPMC,
	no_igmp_require_router_alert_cmd_vtysh,
	"no igmp require-router-alter",
	"No igmp require router alter enable command\n"
	"Igmp require router alter\n"
	"Require router alter\n")

DEFSH (VTYSH_IPMC,
	igmp_send_router_alert_cmd_vtysh,
	"igmp send-router-alter",
	"Igmp send router alter enable command\n"
	"Igmp send router alter\n")

DEFSH (VTYSH_IPMC,
	no_igmp_send_router_alert_cmd_vtysh,
	"no igmp send-router-alter",
	"No igmp send router alter enable command\n"
	"Igmp send router alter\n"
	"Send router alter\n")

DEFSH (VTYSH_IPMC,
	igmp_other_querier_keepalive_cmd_vtysh,
	"igmp other-querier keepalive <60-300>",
	"Igmp querier keepalive command\n"
	"Igmp other-querier\n"
	"Igmp other-querier keepalive\n"
	"Igmp other-querier keepalive time\n")

DEFSH (VTYSH_IPMC,
	no_igmp_other_querier_keepalive_cmd_vtysh,
	"no igmp other-querier keepalive",
	"No igmp other-querier keepalive command\n"
	"Igmp other-querier keepalive config\n"
	"Igmp other-querier\n"
	"Igmp other-querier keepalive\n")

DEFSH (VTYSH_IPMC,
	igmp_query_interval_cmd_vtysh,
	"igmp timer query <1-18000>",
	"Igmp query interval command\n"
	"Igmp query timer\n"
	"Igmp query interval\n"
	"Igmp query interval seconds:1-18000\n")

DEFSH (VTYSH_IPMC,
	no_igmp_query_interval_cmd_vtysh,
	"no igmp timer query",
	"No igmp query interval command\n"
	"Igmp query interval config\n"
	"Igmp timer\n"
	"Igmp timer query\n")

DEFSH (VTYSH_IPMC,
	igmp_robust_count_cmd_vtysh,
	"igmp robust-count <2-5>",
	"Igmp robust count command\n"
	"Igmp robust-count\n"
	"Igmp robust-count number:2-5\n")

DEFSH (VTYSH_IPMC,
	no_igmp_robust_count_cmd_vtysh,
	"no igmp roubust-count",
	"No igmp roubust count command\n"
	"Igmp roubust-count config\n"
	"Igmp roubust-count\n")

DEFSH (VTYSH_IPMC,
	igmp_max_resp_time_cmd_vtysh,
	"igmp max-response-time <1-25>",
	"Igmp max response time command\n"
	"Igmp max response time\n"
	"Igmp max response time seconds:1-25\n")

DEFSH (VTYSH_IPMC,
	no_igmp_max_resp_time_cmd_vtysh,
	"no igmp max-response-time",
	"No igmp max response time command\n"
	"Igmp configure\n"
	"Igmp max response time\n")

DEFSH (VTYSH_IPMC,
	igmp_last_member_query_interval_cmd_vtysh,
	"igmp last member-query interval <1-5>",
	"Igmp last member query interval command\n"
	"Igmp last member\n"
	"Igmp last member query\n"
	"Igmp last member query interval\n"
	"Igmp last member query interval seconds:1-5\n")

DEFSH (VTYSH_IPMC,
	no_igmp_last_member_query_interval_cmd_vtysh,
	"no igmp last member-query interval",
	"No igmp last member query interval command\n"
	"Igmp configure\n"
	"Igmp last member\n"
	"Igmp last member query\n"
	"Igmp last member query interval\n")

DEFSH(VTYSH_IPMC,
	igmp_fast_leave_cmd_vtysh,
	"igmp fast-leave",
	"Igmp fast leave command\n"
	"Igmp fast-leave\n")

DEFSH(VTYSH_IPMC,
	no_igmp_fast_leave_cmd_vtysh,
	"no igmp fast-leave",
	"No igmp fast leave command\n"
	"Igmp fast-leave config\n"
	"Igmp fast-leave\n")

DEFSH(VTYSH_IPMC,
	igmp_ssm_mapping_cmd_vtysh,
	"igmp ssm-mapping enable",
	"Igmp ssm-mapping command\n"
	"Igmp ssm-mapping\n"
	"Ssm mapping enable\n")

DEFSH(VTYSH_IPMC,
	no_igmp_ssm_mapping_cmd_vtysh,
	"no igmp ssm-mapping enable",
	"No igmp ssm-mapping command\n"
	"Igmp configure\n"
	"Igmp ssm-mapping\n"
	"Ssm mapping enable\n")

DEFSH(VTYSH_IPMC,
	debug_igmp_packet_cmd_vtysh,
	"debug igmp packet {recv|send}",
	"Debug igmp packet command\n"
	"Igmp\n"
	"Igmp packet\n"
	"Igmp packet receive\n"
	"Igmp packet send\n")

DEFSH(VTYSH_IPMC,
	no_debug_igmp_packet_cmd_vtysh,
	"no debug igmp packet {recv|send}",
	"No debug igmp packet command\n"
	"Debug\n"
	"Igmp\n"
	"Igmp packet\n"
	"Igmp packet receive\n"
	"Igmp packet send\n")

DEFSH(VTYSH_IPMC,
	show_igmp_debug_cmd_vtysh,
	"show igmp debug",
	"Show igmp debug command\n"
	"Igmp\n"
	"Igmp debug info\n")

DEFSH(VTYSH_IPMC,
	show_igmp_group_cmd_vtysh,
	"show igmp group {A.B.C.D |source A.B.C.D}",
	"Show igmp group command\n"
	"Igmp\n"
	"Igmp group\n"
	"Igmp group ip address\n"
	"Igmp source\n"
	"Igmp source ip address\n")

DEFSH(VTYSH_IPMC,
	show_igmp_routing_table_cmd_vtysh,
	"show igmp routing-table",
	"Show igmp routing table command\n"
	"Igmp\n"
	"Igmp routing table\n")

DEFSH(VTYSH_IPMC,
	show_igmp_statistics_cmd_vtysh,
	"show igmp statistics interface ethernet USP",
	"Show igmp interface statistics command\n"
	"Igmp\n"
	"Igmp statistics\n"
	"interface\n"
	"The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")

DEFSH(VTYSH_IPMC,
	show_igmp_statistics_gigabit_eth_cmd_vtysh,
	"show igmp statistics interface gigabitethernet USP",
	"Show igmp interface statistics command\n"
	"Igmp\n"
	"Igmp statistics\n"
	"interface\n"
	"The port/subport of GigabitEthernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")

DEFSH(VTYSH_IPMC,
	show_igmp_statistics_xgigabit_eth_cmd_vtysh,
	"show igmp statistics interface xgigabitethernet USP",
	"Show igmp interface statistics command\n"
	"Igmp\n"
	"Igmp statistics\n"
	"interface\n"
	"The port/subport of 10GigabitEthernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")

DEFSH(VTYSH_IPMC,
	show_igmp_config_instance_cmd_vtysh,
	"show igmp config {instance <1-128>}",
	"Show igmp config command\n"
	"Igmp\n"
	"Igmp config\n"
	"Igmp instance\n"
	"Igmp instance id:1-128\n")


DEFSH(VTYSH_IPMC,
	show_igmp_config_interface_cmd_vtysh,
	"show igmp config interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP}",
	"Show igmp config command\n"
	"Igmp\n"
	"Igmp config\n"
	"Interface\n"
	"Ethernet\n"
	"Interface Name\n"
	"Gigabitethernet\n"
	"Interface Name\n"
	"xgigabitethernet\n"
	"Interface Name\n")
	
DEFSH(VTYSH_IPMC,
	show_igmp_config_instance_interface_cmd_vtysh,
	"show igmp config instance <1-128> interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP}",
	"Show igmp config command\n"
	"Igmp\n"
	"Igmp config\n"
	"Igmp instance\n"
	"Igmp instance id:1-128\n"
	"Interface\n"
	"Ethernet\n"
	"Interface Name\n"
	"Gigabitethernet\n"
	"Interface Name\n"
	"xgigabitethernet\n"
	"Interface Name\n")


#if 1
/* debug 日志发送到 syslog 使能状态设置 */
DEFSH (VTYSH_IPMC,ipmc_log_level_ctl_cmd_vtysh,
        "debug ipmc(enable | disable)",
        "Output log of debug level\n"
        "Program name\n"
        "Enable\n"
        "Disable\n")

/* debug 日志发送到 syslog 使能状态显示 */
DEFSH (VTYSH_IPMC,ipmc_show_log_level_ctl_cmd_vtysh,
        "show debug ipmc",
    	SHOW_STR
    	"Output log of debug level\n"
    	"Program name\n")
#endif

void vtysh_init_ipmc_cmd(void)
{
    /* pim命令注册 */
    //install_node (&pim_instance_node, NULL);
    //vtysh_install_default (PIM_INSTANCE_NODE);

    //install_element (PIM_INSTANCE_NODE, &pim_instance_exit_cmd);

	/*在配置模式下注册命令*/
	/*[no] pim instance <1-255>*/
	//install_element(CONFIG_NODE, &pim_instance_cmd);
	//install_element(CONFIG_NODE, &undo_pim_instance_cmd);

	/*[no] debug pim packet*/
	install_element_level(CONFIG_NODE, &pim_debug_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &undo_pim_debug_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &pim_debug_mfib_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &pim_ssm_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &undo_pim_ssm_cmd, CONFIG_LEVE_5, CMD_LOCAL);

	/*接口加入到pim instance*/
	/*[no] pim enable instance <1-255>*/
	install_element_level(PHYSICAL_IF_NODE, &pim_instance_mcif_add_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(PHYSICAL_IF_NODE, &pim_instance_mcif_del_cmd, CONFIG_LEVE_5, CMD_LOCAL);

	/*PIM instance视图下配置pim*/
	/*[no] pim (sm|dm)*/
	//install_element(PIM_INSTANCE_NODE, &pim_instance_mode_cmd);
	//install_element(PIM_INSTANCE_NODE, &undo_pim_instance_mode_cmd);
	//install_element(PHYSICAL_IF_NODE, &pim_instance_mode_cmd);
	//install_element(PHYSICAL_IF_NODE, &undo_pim_instance_mode_cmd);	

	/*[no] pim hello interval <1-3600>*/
	//install_element(PIM_INSTANCE_NODE, &pim_instance_hello_interval_cmd);
	//install_element(PIM_INSTANCE_NODE, &undo_pim_instance_hello_interval_cmd);
	install_element_level(CONFIG_NODE, &pim_instance_hello_interval_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &undo_pim_instance_hello_interval_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(PHYSICAL_IF_NODE, &pim_instance_hello_interval_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(PHYSICAL_IF_NODE, &undo_pim_instance_hello_interval_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	/*[no] pim dr priority <0-255>*/
	//install_element(PIM_INSTANCE_NODE, &pim_instance_dr_priority_cmd);
	//install_element(PIM_INSTANCE_NODE, &undo_pim_instance_dr_priority_cmd);
	install_element_level(CONFIG_NODE, &pim_instance_dr_priority_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &undo_pim_instance_dr_priority_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(PHYSICAL_IF_NODE, &pim_instance_dr_priority_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(PHYSICAL_IF_NODE, &undo_pim_instance_dr_priority_cmd, CONFIG_LEVE_5, CMD_LOCAL);

	/*[no] pim static-rp A.B.C.D*/
	//install_element(PIM_INSTANCE_NODE, &pim_instance_static_rp_cmd);
	//install_element(PIM_INSTANCE_NODE, &undo_pim_instance_static_rp_cmd);
	install_element_level(CONFIG_NODE, &pim_instance_static_rp_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &undo_pim_instance_static_rp_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(PHYSICAL_IF_NODE, &pim_instance_static_rp_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(PHYSICAL_IF_NODE, &undo_pim_instance_static_rp_cmd, CONFIG_LEVE_5, CMD_LOCAL);

	/*[no] pim join-prune interval <1-600>*/
	//install_element(PIM_INSTANCE_NODE, &pim_instance_jp_interval_cmd);
	//install_element(PIM_INSTANCE_NODE, &undo_pim_instance_jp_interval_cmd);
	install_element_level(CONFIG_NODE, &pim_instance_jp_interval_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &undo_pim_instance_jp_interval_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(PHYSICAL_IF_NODE, &pim_instance_jp_interval_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(PHYSICAL_IF_NODE, &undo_pim_instance_jp_interval_cmd, CONFIG_LEVE_5, CMD_LOCAL);

	/*show pim if neighbor info*/	
	install_element_level(CONFIG_NODE, &pim_if_nbr_show_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &pim_mrt_show_cmd, MONITOR_LEVE_2, CMD_LOCAL);
#if 0

	/*[no] pim spt-switch threshold <0-1000>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_spt_switch_threshold_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_spt_switch_threshold_cmd);

	/*[no] pim prune delay <1-3600>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_prune_delay_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_prune_delay_cmd);

	/*[no] pim prune hold-time <1-3600>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_prune_holdtime_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_prune_holdtime_cmd);

	/*[no] pim source age-time <1-3600>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_src_agetime_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_src_agetime_cmd);

	/*[no] pim group  A.B.C.D/<1-32>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_pim_group_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_pim_group_cmd);

	/*[no] pim c-rp interface if*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_crp_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_crp_cmd);

	/*[no] pim c-rp priority <0-255>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_crp_priority_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_crp_priority_cmd);

	/*[no] pim advertise interval <1-3600>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_crp_adv_interval_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_crp_adv_interval_cmd);

	/*[no] pim c-bsr  interface if*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_cbsr_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_cbsr_cmd);

	/*[no] pim c-bsr priority <0-255>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_cbsr_priority_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_cbsr_priority_cmd);

	/*[no] pim c-bsr hash <1-255>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_cbsr_hash_cmd);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_cbsr_hash_cmd);

	/*[no] pim bootstrap interval <1-3600>*/
	install_element(PIM_INSTANCE_NODE, &pim_instance_bsr_bootstrap_interval_cmd, CMD_LOCAL);
	install_element(PIM_INSTANCE_NODE, &undo_pim_instance_bsr_bootstrap_interval_cmd, CMD_LOCAL);
#endif

	/*IGMP command*/

	install_node (&igmp_node, NULL);  

 	vtysh_install_default (IGMP_NODE);

	/*igmp instance command*/
	install_element_level (IGMP_NODE, &vtysh_exit_igmp_cmd, VISIT_LEVE, CMD_SYNC);
	
	install_element_level (CONFIG_NODE, &igmp_instance_cfg_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &no_igmp_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &igmp_inst_version_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &no_igmp_inst_version_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &igmp_inst_querier_keepalive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &no_igmp_inst_querier_keepalive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &igmp_inst_query_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &no_igmp_inst_query_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &igmp_isnt_robust_count_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &no_igmp_inst_robust_count_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &igmp_inst_last_member_query_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &no_igmp_inst_last_member_query_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &igmp_inst_max_resp_time_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &no_igmp_inst_max_resp_time_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &igmp_inst_send_router_alert_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &no_igmp_inst_send_router_alert_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &igmp_inst_require_router_alert_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &no_igmp_inst_require_router_alert_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &igmp_ssm_mapping_group_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (IGMP_NODE, &no_igmp_ssm_mapping_group_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/*interface command*/
	install_element_level (PHYSICAL_IF_NODE, &igmp_enable_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_igmp_enable_instance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &igmp_version_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_igmp_version_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &igmp_port_static_group_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_igmp_port_static_group_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &igmp_require_router_alert_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_igmp_require_router_alert_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &igmp_send_router_alert_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_igmp_send_router_alert_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &igmp_other_querier_keepalive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_igmp_other_querier_keepalive_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &igmp_query_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_igmp_query_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &igmp_robust_count_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_igmp_robust_count_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &igmp_max_resp_time_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_igmp_max_resp_time_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &igmp_last_member_query_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_igmp_last_member_query_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &igmp_fast_leave_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_igmp_fast_leave_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &igmp_ssm_mapping_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_igmp_ssm_mapping_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/*debug and show igmp command*/
	install_element_level (CONFIG_NODE, &debug_igmp_packet_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &no_debug_igmp_packet_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_igmp_debug_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_igmp_group_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_igmp_config_instance_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_igmp_config_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_igmp_config_instance_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (CONFIG_NODE, &show_igmp_routing_table_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_igmp_statistics_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_igmp_statistics_gigabit_eth_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_igmp_statistics_xgigabit_eth_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level(CONFIG_NODE, &ipmc_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &ipmc_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
}
