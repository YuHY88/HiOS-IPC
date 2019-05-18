/**
 * \page cmds_ref_system SYSTEM
 * - \subpage modify_log_system
 * 
 */
 
/**
 * \page modify_log_system Modify Log
 * \section system-v007r004 HiOS-V007R004
 *  -# 
 * \section system-v007r003 HiOS-V007R003
 *  -# 
 */

#include <zebra.h>
#include "command.h"
#include "vtysh.h"

static struct cmd_node stSystemNode = { SYSTEM_NODE,  "%s(config-system)#",  1 };

DEFUNSH (VTYSH_SYSTEM,
	 system_name,
	 system_node_cmd,
	 "system",
	 "enter system node\n")
{
   vty->node = SYSTEM_NODE;
   return CMD_SUCCESS;
}
DEFUNSH (VTYSH_SYSTEM,
         vtysh_exit_system,
         vtysh_exit_system_cmd,
         "exit",
         "Exit current mode and down to previous mode\n"
         )
{
    return vtysh_exit(vty);
}

DEFSH (VTYSH_SYSTEM,
        system_vty_show_cpu_cmd,
		"show system cpu",
		"show\n"
		"system\n"
		"cpu usage statistics\n"
)
	
DEFSH (VTYSH_SYSTEM,
		system_vty_set_time_cmd,
		"system datetime YYYY:MM:DD:HH:MM:SS (local|utc)",
		"set system\n"
		"system time\n"
		"year:mon:day:hour:min:sec\n"
		"set local time\n"
		"set utc time\n"
)
DEFSH (VTYSH_SYSTEM,
     system_vty_set_time_2_cmd,
     "system datetime YYYY:MM:DD:HH:MM:SS",
     "set system\n"
     "system time\n"
     "year:mon:day:hour:min:sec\n"
)

DEFSH (VTYSH_SYSTEM,
		system_vty_set_default_time_cmd,
		"no system datetime",
		NO_STR
		"set system\n"
		"default datetime\n"
)

DEFSH (VTYSH_SYSTEM,
		system_vty_show_time_cmd,
		"show system datetime (local|utc)",
		"show\n"
		"system\n"
		"system datetime\n"
		"display local time\n"
		"display utc time\n"
)
DEFSH (VTYSH_SYSTEM,
		system_vty_show_time_2_cmd,
		"show system datetime",
		"show\n"
		"system\n"
		"system datetime\n"
)	
DEFSH (VTYSH_SYSTEM,
		system_vty_display_cpu_cmd,
		"show cpu-usage",
		"show\n"
		"cpu usage statistics\n"
)
		
DEFSH (VTYSH_SYSTEM,
		system_vty_display_health_cmd,
		"show health",
		"show\n"
		"cpu usage and memory statistics\n"
)
	
DEFSH(VTYSH_SYSTEM,
	system_show_threshold_cmd,
	"show system threshold",
	"show\n"
	"system statue\n"
	"system cpu and memory alarm threshold\n"
	)



DEFSH(VTYSH_SYSTEM,
	system_set_threshold_cmd,
	"system (cpu | memory) threshold VALUE",
	"Set system\n"
	"Cpu set\n"
	"Memory set\n"
	"threshold set\n"
	"Input value 0-99\n"
	)


DEFSH(VTYSH_SYSTEM,
	system_set_default_threshold_cmd,
	"no system (cpu | memory) threshold",
	NO_STR
	"set system\n"
	"Cpu set\n"
	"Memory set\n"
	"default time\n"
	)
             
 DEFSH(VTYSH_SYSTEM,
     system_set_timezone_cmd,
     "system timezone <-12-12>",
    "Set system\n"
    "System timezone set\n"
    "Input value -12 - 12\n"
     )
 
 
 DEFSH(VTYSH_SYSTEM,
     system_set_default_timezone_cmd,
     "no system zone",
     NO_STR
     "set system\n"
     "system timezone set default\n"
     )


/* debug 日志发送到 syslog 使能状态设置 */
DEFSH (VTYSH_SYSTEM,system_log_level_ctl_cmd_vtysh,
 "debug system (enable|disable) (common|event|info|package|error|all)",
 "Debug information to moniter\n"
 "Programe name\n"
 "Enable statue\n"
 "Disatble statue\n"
 "Type name of common messege\n"
 "Type name of event messege\n"
 "Type name of info messege\n"
 "Type name of package messege \n"
 "Type name of error messege \n"
 "Type name of all debug\n")


/* debug 日志发送到 syslog 使能状态显示 */
DEFSH (VTYSH_SYSTEM,system_show_log_level_ctl_cmd_vtysh, "show debug system",		
	SHOW_STR"Output log of debug level\n""Program name\n")	


void vtysh_init_system_cmd(void)
{
    install_node (&stSystemNode, NULL);
    vtysh_install_default (SYSTEM_NODE);
    
    install_element_level (SYSTEM_NODE, &vtysh_exit_system_cmd,VISIT_LEVE,CMD_SYNC);
	install_element_level(CONFIG_NODE,	&system_node_cmd,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (CONFIG_NODE, &system_vty_show_cpu_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &system_vty_show_time_cmd,MONITOR_LEVE_2,CMD_LOCAL);
    install_element_level (CONFIG_NODE, &system_vty_show_time_2_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &system_vty_display_cpu_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &system_vty_display_health_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &system_show_threshold_cmd,MONITOR_LEVE_2,CMD_LOCAL);

	install_element_level (SYSTEM_NODE, &system_vty_show_cpu_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (SYSTEM_NODE, &system_vty_show_time_cmd,MONITOR_LEVE_2,CMD_LOCAL);
    install_element_level (SYSTEM_NODE, &system_vty_show_time_2_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (SYSTEM_NODE, &system_vty_display_cpu_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (SYSTEM_NODE, &system_vty_display_health_cmd,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (SYSTEM_NODE, &system_show_threshold_cmd,MONITOR_LEVE_2,CMD_LOCAL);

	install_element_level (SYSTEM_NODE, &system_set_threshold_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYSTEM_NODE, &system_set_default_threshold_cmd,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (SYSTEM_NODE, &system_set_timezone_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYSTEM_NODE, &system_set_default_timezone_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYSTEM_NODE, &system_vty_set_time_cmd,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (SYSTEM_NODE, &system_vty_set_time_2_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (SYSTEM_NODE, &system_vty_set_default_time_cmd,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (CONFIG_NODE, &system_log_level_ctl_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &system_show_log_level_ctl_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);

}

