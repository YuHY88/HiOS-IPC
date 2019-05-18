/**
 * \page cmds_ref_alarm ALARM
 * - \subpage modify_log_alarm
 * 
 */
 
/**
 * \page modify_log_alarm Modify Log
 * \section alarm-v007r004 HiOS-V007R004
 *  -# 
 * \section alarm-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"


static struct cmd_node alarm_node =
{
	ALARM_NODE,
	"%s(config-alarm)#",
};


DEFUNSH(VTYSH_ALARM,
		alarm_mode_enable,
		alarm_mode_enable_cmd,
		"alarm",
		"alarm command node\n")
{
	vty->node = ALARM_NODE;

	return CMD_SUCCESS;
}


DEFUNSH(VTYSH_ALARM,
	vtysh_exit_alarm,
	vtysh_exit_alarm_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}


ALIAS (vtysh_exit_alarm,
	   vtysh_quit_alarm_cmd,
	   "quit",
	   "Exit current mode and down to previous mode\n");



DEFSH(VTYSH_ALARM, 
       alarm_show_code_cmd_vtysh,
       "show alarm code",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm code define\n");
       
DEFSH(VTYSH_ALARM, 
       alarm_show_stype_cmd_vtysh,
       "show alarm subtype",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm subtype define\n");
       
DEFSH(VTYSH_ALARM, 
       alarm_show_scount_cmd_vtysh,
       "show alarm statistic",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm statistics(for subtype)\n");

DEFSH(VTYSH_ALARM, 
       alarm_show_config_cmd_vtysh,
       "show alarm config",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm config\n");

DEFSH(VTYSH_ALARM, 
       alarm_show_record_cmd_vtysh,
       "show alarm record (history|current|event)",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm record\n"
       "Display alarm history record\n"
       "Display alarm current record\n"
       "Display alarm event record\n");

DEFSH(VTYSH_ALARM,
		alarm_show_register_info_cmd,
		"show alarm register",
		SHOW_STR
		"Display alarm information\n"
		"Display alarm register information\n");




DEFSH(VTYSH_ALARM, 
       alarm_show_hash_cmd_vtysh,
       "show alarm hash record",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm hash table\n"
       "Display alarm record hash table\n");

DEFSH(VTYSH_ALARM, 
       alarm_show_inhblogic_cmd_vtysh,
       "show alarm suppress logic",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm suppress logic\n"
       "Display alarm suppress logic\n");


DEFSH(VTYSH_ALARM, 
       alarm_conf_repdly_cmd_vtysh,
       "alarm delay <1-10>",
       "Alarm config\n"
       "Alarm report(trap) delay time\n"
       "Input delay time 1-10 seconds\n");


DEFSH(VTYSH_ALARM, 
       alarm_conf_filesize_cmd_vtysh,
       "alarm file size <10-100>",
       "Alarm config\n"
       "Alarm file config\n"
       "Alarm file size config\n"
       "Input file size 10-100M\n");


DEFSH(VTYSH_ALARM, 
       alarm_conf_filewrite_cmd_vtysh,
       "alarm file write",
       "Alarm config\n"
       "Alarm file config\n"
       "Write alarm file buffer to file\n");

       
DEFSH(VTYSH_ALARM, 
       alarm_conf_typemask_cmd_vtysh,
       "alarm shield subtype WORD",
       "Alarm config\n"
       "Alarm shield config\n"
       "Alarm subtype shied config\n"
       "Input subtype name\n");


DEFSH(VTYSH_ALARM, 
       alarm_conf_typenomask_cmd_vtysh,
       "no alarm shield subtype WORD",
       "No operate\n"
       "Alarm config\n"
       "Alarm shield config\n"
       "Alarm subtype shied config\n"
       "Input subtype name\n");

DEFSH(VTYSH_ALARM, 
       alarm_show_ifhash_cmd_vtysh,
       "show alarm ifcount (hash|subtype)",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm if record count\n"
       "Display alarm if record count by hash\n"
       "Display alarm if record count by subtype\n");

DEFSH(VTYSH_ALARM, 
       alarm_show_almstat_cmd_vtysh,
       "show alarm state subtype WORD",
       SHOW_STR
       "Display alarm information\n"
       "Display alarm current state\n"
       "Display special alarm type's alarm state\n"
       "Input special alarm type name\n");
#if 0
DEFSH(VTYSH_ALARM, 
       alarm_conf_typefilt_cmd_vtysh,
       "alarm filt subtype WORD name WORD",
       "Alarm config\n"
       "Alarm filt config\n"
       "Alarm subtype filt config\n"
       "Input subtype name\n"
       "Interface or service name\n"
       "Input name\n")

DEFSH(VTYSH_ALARM, 
       alarm_conf_typenofilt_cmd_vtysh,
       "no alarm filt subtype WORD name WORD",
       "No operate\n"
       "Alarm config\n"
       "Alarm filt config\n"
       "Alarm subtype filt config\n"
       "Input subtype name\n"
       "Interface or service name\n"
       "Input name\n")

DEFSH(VTYSH_ALARM, 
       alarm_conf_debug_cmd_vtysh,
       "debug alarm (proc|ifm|all)",
       "Debuv config\n"
       "Alarm config\n"
       "Alarm process debug\n"
       "Alarm interface debug\n"
       "Alarm all debug\n")


DEFSH(VTYSH_ALARM, 
       alarm_conf_nodebug_cmd_vtysh,
       "no debug alarm (proc|ifm|all)",
       "No operate\n"
       "Debuv config\n"
       "Alarm config\n"
       "Alarm process debug\n"
       "Alarm interface debug\n"
       "Alarm all debug\n")


DEFSH(VTYSH_ALARM, 
       alarm_conf_suppress_cmd_vtysh,
       "alarm suppress (enable|disable)",
       "Alarm config\n"
       "Alarm suppress config\n"
       "Alarm suppress enable\n" 
       "Alarm suppress disable\n")

DEFSH(VTYSH_ALARM, 
       alarm_conf_levelfilt_cmd_vtysh,
       "alarm to server level (all|event|notice|minor|major|critical)",
       "Alarm config\n"
       "Alarm report config\n"
       "Alarm report to MGT config\n"
       "Report to MGT alarm level\n"
       "All alarm Report to MGT\n"
       "Report to MGT lowest level event\n"
       "Report to MGT lowest level notice\n"
       "Report to MGT lowest level minor\n"
       "Report to MGT lowest level major\n"
       "Report to MGT lowest level critical\n")


DEFSH(VTYSH_ALARM, 
	alarm_log_level_ctl_cmd_vtysh, 
	"debug alarm (enable | disable)",
	"Output log of debug level\n"
	"Program name\n"
	"Enable\n"
	"Disable\n");

DEFSH(VTYSH_ALARM, 
	alarm_show_log_level_ctl_cmd_vtysh, 
	"show debug alarm",
	SHOW_STR
	"Output log of debug level\n"
	"Program name\n");
#endif


/********************************************** debug ***********************************************/
DEFSH (VTYSH_ALARM,
	alarm_debug_monitor_cmd,
	"debug alarm (enable|disable) (register|report|get|get-next|set|sync|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Type name of register messege\n"
	"Type name of report messege\n"
	"Type name of get log queue \n"
	"Type name of get-next log queue \n"
	"Type name of set status\n"
	"Type name of sync status\n"
	"Type name of all debug\n");

DEFSH (VTYSH_ALARM,
	show_alarm_debug_monitor_cmd,
	"show alarm debug",
	SHOW_STR
	"Syslog"
	"Debug status\n");


/*******************************************************************************************************/


void vtysh_init_alarm_cmd(void)
{
	install_node (&alarm_node, NULL);
	vtysh_install_default (ALARM_NODE);
	/* aaa enter cmd */
	install_element(CONFIG_NODE, &alarm_mode_enable_cmd, CMD_LOCAL);
	install_element(ALARM_NODE, &vtysh_exit_alarm_cmd, CMD_LOCAL);
	install_element(ALARM_NODE, &vtysh_quit_alarm_cmd, CMD_LOCAL);
    install_element(ALARM_NODE, &alarm_show_code_cmd_vtysh,CMD_LOCAL);
	install_element(ALARM_NODE, &alarm_show_record_cmd_vtysh, CMD_LOCAL);
	install_element(ALARM_NODE, &alarm_show_register_info_cmd, CMD_LOCAL);
#if 0
    install_element(CONFIG_NODE, &alarm_show_scount_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &alarm_show_config_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &alarm_show_record_cmd_vtysh,CMD_LOCAL);
 // install_element(CONFIG_NODE, &alarm_show_hash_cmd_vtysh,CMD_LOCAL);
 // install_element(CONFIG_NODE, &alarm_show_ifhash_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &alarm_show_almstat_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &alarm_show_inhblogic_cmd_vtysh,CMD_LOCAL);

    install_element(CONFIG_NODE, &alarm_conf_repdly_cmd_vtysh,CMD_SYNC);
    install_element(CONFIG_NODE, &alarm_conf_filewrite_cmd_vtysh,CMD_SYNC);
    install_element(CONFIG_NODE, &alarm_conf_filesize_cmd_vtysh,CMD_SYNC);
    install_element(CONFIG_NODE, &alarm_conf_levelfilt_cmd_vtysh,CMD_SYNC);
    install_element(CONFIG_NODE, &alarm_conf_typemask_cmd_vtysh,CMD_SYNC);
    install_element(CONFIG_NODE, &alarm_conf_typenomask_cmd_vtysh,CMD_SYNC);
//  install_element(CONFIG_NODE, &alarm_conf_typenofilt_cmd_vtysh,CMD_SYNC);
//  install_element(CONFIG_NODE, &alarm_conf_typefilt_cmd_vtysh,CMD_SYNC);
    install_element(CONFIG_NODE, &alarm_conf_debug_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &alarm_conf_nodebug_cmd_vtysh,CMD_LOCAL);
    install_element(CONFIG_NODE, &alarm_conf_suppress_cmd_vtysh,CMD_SYNC);
#endif

	/* debug cmd */
	//install_element(CONFIG_NODE, &alarm_log_level_ctl_cmd_vtysh, CMD_SYNC);
	//install_element(CONFIG_NODE, &alarm_show_log_level_ctl_cmd_vtysh, CMD_LOCAL);
	
	/* new debug */
	install_element_level (CONFIG_NODE, &alarm_debug_monitor_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &show_alarm_debug_monitor_cmd, MANAGE_LEVE, CMD_SYNC);
}


