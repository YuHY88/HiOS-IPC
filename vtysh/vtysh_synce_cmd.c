/**
 * \page cmds_ref_synce SYNCE
 * - \subpage modify_log_synce
 * 
 */
 
/**
 * \page modify_log_synce Modify Log
 * \section synce-v007r004 HiOS-V007R004
 *  -# 
 * \section synce-v007r003 HiOS-V007R003
 *  -# 
 */

#include <zebra.h>
#include "command.h"
#include "ifm_common.h"
#include "vtysh.h"
static struct cmd_node synce_node =
{ 
SYNCE_NODE,  
"%s(config-synce)# ",  
1, 
};


DEFUNSH(VTYSH_CLOCK,
		synce_enable,
		synce_enable_cmd,
		"synce",
		"enter synce command node\n"
		)
{
	vty->node = SYNCE_NODE;
	return CMD_SUCCESS;
}

DEFSH(VTYSH_CLOCK,
		synce_mode_select_cmd,
		"synce mode (auto|force|manual)",
		"synce function set\n"
		"set current run mode\n"
		"auto mode\n"
		"force mode\n"
		"manual mode\n"
		)


DEFSH(VTYSH_CLOCK,
		synce_ssm_enable_cmd,
		"synce ssm (disable|enable)",
		"synce function set\n"
		"ssm function \n"
		"set ssm disable \n"
		"set ssm enable \n"
		)

DEFSH(VTYSH_CLOCK,
		synce_ssm_interval_cmd,
		"synce ssm interval <1-10>",
		"synce function set\n"
		"ssm function\n"
		"send interval\n"
		"send interval value(second)\n"
		)
		
DEFSH(VTYSH_CLOCK,
		synce_preempt_enable_cmd,
		"synce preempt (disable|enable)",
		"set synce function\n"
		"set preempt function\n"
		"set preempt function disable\n"
		"set preempt function enable\n"
		)

DEFSH(VTYSH_CLOCK,
		synce_failback_wtr_cmd,
		"synce failback wtr <0-3600>",
		"set synce function\n"
		"failback\n"
		"set wtr\n"
		"failback time value(second)\n"
		)

DEFSH(VTYSH_CLOCK,
		synce_holdtime_cmd,
		"synce hold-time <300-604800>",
		"set synce function\n"
		"set clock hold-time \n"
		"clock hold-time value(second)\n"
		)

DEFSH(VTYSH_CLOCK,
        synce_freqoffset_cmd,
        "clock frequency deviation <-75-75>",
        "set clock function\n"
        "set clock frequency \n"
        "set clock deviation \n"
        "clock frequency deviation value\n"
        )


DEFSH(VTYSH_CLOCK,
		synce_clock_priority_cmd,
		"synce priority <1-8>",
		"set synce function\n"
		"clock priority\n"
		"clock priority value\n"
		)
	
DEFSH(VTYSH_CLOCK,
		no_synce_mode_select_cmd,
		"no synce mode",
		NO_STR
		"synce\n"
		"set synce default mode \n"
		)


DEFSH(VTYSH_CLOCK,
		no_synce_ssm_enable_cmd,
		"no synce ssm",
		NO_STR
		"synce\n"
		"set synce ssm function default\n"
		)

DEFSH(VTYSH_CLOCK,
		no_synce_ssm_interval_cmd,
		"no synce ssm interval",
		NO_STR
		"synce\n"
		"set ssm function\n"
		"set synce ssm interval default\n"
		)
		
DEFSH(VTYSH_CLOCK,
		no_synce_preempt_enable_cmd,
		"no synce preempt",
		NO_STR
		"synce\n"
		"set preempt function\n"
		"set synce preempt default\n"
		)

DEFSH(VTYSH_CLOCK,
		no_synce_failback_wtr_cmd,
		"no synce failback wtr",
		NO_STR
		"synce\n"
		"set synce failback\n"
		"set synce failback wtr default\n"
		)

DEFSH(VTYSH_CLOCK,
		no_synce_holdtime_cmd,
		"no synce hold-time",
		NO_STR
		"synce\n"
		"set synce hold-time default\n"
		)
DEFSH(VTYSH_CLOCK,
        no_synce_freqoffset_cmd,
        "no clock frequency deviation",
        NO_STR
        "clock\n"
        "set clock frequency \n"
        "set clock frequency deviation default\n"
        )

DEFSH(VTYSH_CLOCK,
		no_synce_clock_priority_cmd,
		"no synce priority",
		NO_STR
		"synce\n"
		"set current port synce fucntion disable\n"
		)

DEFSH(VTYSH_CLOCK,
		synce_show_config_cmd,
		"show synce config",
		SHOW_STR
		"synce function\n"
		"show current synce configure\n"
		)

DEFSH(VTYSH_CLOCK,
		synce_show_source_cmd,
		"show synce source",
		SHOW_STR
		"synce function\n"
		"show current synce clock source\n"
		)

DEFSH(VTYSH_CLOCK,
		synce_show_ssm_pkt_cmd,
		"show synce ssm",
		SHOW_STR
		"synce function\n"
		"show current port synce ssm state\n"
		)




DEFSH (VTYSH_CLOCK,
		synce_clk_source_ifm_set_cmd,
		"synce source (force | manual) interface ethernet USP",
		"set synce function\n"
		"set clock soure\n"
		"under force mode set clock source\n"
		"under manual mode set clock source\n"
		"interface select\n"
		"ethernet interface\n"
		"The port of the interface, format: <0-7>/<0-31>/<1-255>\n"
		)
		
DEFSH (VTYSH_CLOCK,
		synce_clk_source_ifm_gigabit_ethernet_set_cmd,
		"synce source (force | manual) interface gigabitethernet USP",
		"set synce function\n"
		"set clock soure\n"
		"under force mode set clock source\n"
		"under manual mode set clock source\n"
		"interface select\n"
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		)

DEFSH (VTYSH_CLOCK,
		synce_clk_source_ifm_xgigabit_ethernet_set_cmd,
		"synce source (force | manual) interface xgigabitethernet USP",
		"set synce function\n"
		"set clock soure\n"
		"under force mode set clock source\n"
		"under manual mode set clock source\n"
		"interface select\n"
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		)
		
DEFSH (VTYSH_CLOCK,
		synce_clk_source_clkif_set_cmd,
		"synce source (force | manual) interface clock <1-2>",
		"set synce function\n"
		"set clock soure\n"
		"under force mode set clock source\n"
		"under manual mode set clock source\n"
		"interface select\n"
		"clock interface\n"
		"clock interface value\n"
		)
DEFSH (VTYSH_CLOCK,
		synce_clk_source_tdmif_set_cmd,
		"synce source (force | manual) interface tdm USP",
		"set synce function\n"
		"set clock soure\n"
		"under force mode set clock source\n"
		"under manual mode set clock source\n"
		"interface select\n"
		CLI_INTERFACE_TDM_STR
		CLI_INTERFACE_TDM_VHELP_STR
		)


DEFSH(VTYSH_CLOCK,
		synce_clock_priority_e1_cmd_vtysh,
		"e1 priority <1-8>",
		"set e1 function\n"
		"clock priority\n"
		"clock priority value\n"
		)
DEFSH(VTYSH_CLOCK,
		no_synce_clock_priority_e1_cmd_vtysh,
		"no e1 priority",
		NO_STR
		"set e1\n"
		"set current port synce fucntion disable\n"
		)

DEFUNSH (VTYSH_CLOCK,
		vtysh_exit_synce,
		vtysh_exit_synce_cmd,
		"exit",
		"Exit current mode and down to previous mode\n")
{
	return vtysh_exit (vty);
}

void vtysh_init_synce_cmd(void)
{
	install_node ( &synce_node, NULL);
	vtysh_install_default ( SYNCE_NODE );

	install_element_level (CONFIG_NODE, &synce_enable_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &synce_mode_select_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &synce_ssm_enable_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &synce_ssm_interval_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &synce_preempt_enable_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &synce_failback_wtr_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &synce_holdtime_cmd,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (SYNCE_NODE, &synce_freqoffset_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &synce_clk_source_ifm_set_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &synce_clk_source_ifm_gigabit_ethernet_set_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &synce_clk_source_ifm_xgigabit_ethernet_set_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &synce_clk_source_clkif_set_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &synce_clk_source_tdmif_set_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &synce_clock_priority_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TDM_IF_NODE, &synce_clock_priority_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (E1_IF_NODE, &synce_clock_priority_cmd,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (STM_IF_NODE, &synce_clock_priority_cmd,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (SYNCE_NODE, &no_synce_mode_select_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &no_synce_ssm_enable_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &no_synce_ssm_interval_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &no_synce_preempt_enable_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &no_synce_failback_wtr_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYNCE_NODE, &no_synce_holdtime_cmd,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (SYNCE_NODE, &no_synce_freqoffset_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_synce_clock_priority_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TDM_IF_NODE, &no_synce_clock_priority_cmd,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (E1_IF_NODE, &no_synce_clock_priority_cmd,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (STM_IF_NODE, &no_synce_clock_priority_cmd,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (SYNCE_NODE, &synce_show_config_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (SYNCE_NODE, &synce_show_source_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (PHYSICAL_IF_NODE, &synce_show_ssm_pkt_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	
	install_element_level (CONFIG_NODE, &synce_show_config_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &synce_show_source_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	
	install_element_level (SYNCE_NODE, &vtysh_exit_synce_cmd,VISIT_LEVE,CMD_SYNC);

	return;
}
