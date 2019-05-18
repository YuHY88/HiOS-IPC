/**
 * \page cmds_ref_syslog SYSLOG
 * - \subpage modify_log_syslog
 * 
 */
 
/**
 * \page modify_log_syslog Modify Log
 * \section syslog-v007r004 HiOS-V007R004
 *  -# 
 * \section syslog-v007r003 HiOS-V007R003
 *  -# 
 */

#include <zebra.h>
#include "command.h"
#include "vtysh.h"

static struct cmd_node syslog_node = 
{
	SYSLOG_NODE,
	"%s(config-syslog)#",
};

DEFUNSH (VTYSH_SYSLOG,
	vtysh_syslog,
	vtysh_syslog_cmd,
	"syslog",
	"Enter syslog node\n")
{
  vty->node = SYSLOG_NODE;
  return CMD_SUCCESS;
}
/*
DEFUNSH (VTYSH_SYSLOG,
	 vtysh_show_log,
	 vtysh_show_log_cmd,
	 "show log",
	 SHOW_STR
	 "Syslog configs\n")
{
  return CMD_SUCCESS;
}
*/
DEFUNSH (VTYSH_SYSLOG,
	vtysh_exit_log,
	vtysh_exit_log_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}

ALIAS (vtysh_exit_log,
	vtysh_quit_log_cmd,
	"quit",
	"Exit current mode and down to previous mode\n")


DEFSH (VTYSH_SYSLOG, show_logging_cmd,
	"show log", SHOW_STR "Logging information\n")

DEFSH (VTYSH_SYSLOG, 
	terminal_monitor_cmd,
	"log terminal",
	"Logging control\n"
	"Set console line to terminal\n")

DEFSH (VTYSH_SYSLOG, 
	terminal_no_monitor_cmd,
	"no log terminal",
	NO_STR
	"Logging control\n"
	"Disable console line to terminal\n")

DEFSH (VTYSH_SYSLOG, 
	log_timestamp_cmd,
	"log timestamp",
	"Logging control\n"
	"Set log timestamp\n")

DEFSH (VTYSH_SYSLOG, 
	log_no_timestamp_cmd,
	"no log timestamp",
	NO_STR
	"Logging control\n"
	"Disable log timestamp\n")

DEFSH (VTYSH_SYSLOG, 
	log_rule_cmd,
	"log <1-100>"
	"(A.B.C.D|memory|console|file) (upto|only|except) (emergency|alert|critical|error|warning|notification|information|debug) [vpn-instance <1-128>]",
	"Logging control\n" 
	"Add log rule ID\n" 
	"Logging to host whose ip address is A.B.C.D\n" "Logging to memory\n"
	"Logging to console\n" "Logging to file\n"
	"Logging include specified priority and all up priorities\n"
	"Logging only specified priority\n"
	"Logging all priorities except specified priority\n"
	"Logging emergencies level -> 0\n" "Logging alerts level -> 1\n"
	"Logging critical level -> 2\n" "Logging errors level -> 3\n"
	"Logging warnings level -> 4\n" "Logging notifications level -> 5\n"
	"Logging information level -> 6\n" "Logging debug level -> 7\n"
	"L3 vpn instance, only for log server\n" "L3 vpn tag\n")


DEFSH (VTYSH_SYSLOG, 
	no_log_rule_cmd,
	"no log <1-100>", 
	NO_STR "Logging control\n"
	"delete log rule\n")

DEFSH (VTYSH_SYSLOG, no_log_cmd, "no log", NO_STR "Logging control\n")


DEFSH (VTYSH_SYSLOG, 
	clear_log_cmd,
	"clear log (memory | file)", 
	CLEAR_STR 
	"clear log information\n"
	"clear memory log\n"
	"clear file log\n")

DEFSH (VTYSH_SYSLOG, 
	log_memsize_cmd,
	"log memory size <4-256>",
	"Logging control \n"
	"Set memory type \n" "Memory log size \n" "Scope of memory size (kilobytes) \n")

DEFSH (VTYSH_SYSLOG, 
	log_filesize_cmd,
	"log file size <1-20000>",
	"Logging control \n"
	"Set file type \n" "File log size\n"  "Scope of file size (kilobytes) \n")

DEFSH (VTYSH_SYSLOG, 
	no_log_memsize_cmd,
	"no log memory size", NO_STR "Logging control \n" "Memroy type\n"  "Unset memory size \n")

DEFSH (VTYSH_SYSLOG, 
	no_log_filesize_cmd,
	"no log file size", NO_STR "Logging control \n" "File type\n" "Unset file size \n")

DEFSH (VTYSH_SYSLOG, 
	show_log_file_or_memroy_cmd,
	/*"show log (memory | file)", */
	"show log memory",
	SHOW_STR 
	"Display logging information\n"
	"memory information\n"
	/*"File information\n"*/)  

DEFSH (VTYSH_SYSLOG, 
	ratelimit_rule_cmd,
	"log ratelimit (A.B.C.D|memory|console|file) <1-10000>",
	"Logging control\n"
	"Rate limit of specified facility \n"
	"Logging to host whose ip address is A.B.C.D\n" "Logging to memory\n"
	"Logging to console\n" "Logging to file\n"
	"Seconds per syslog information \n")


DEFSH (VTYSH_SYSLOG, 
	no_ratelimit_rule_cmd,
	"no log ratelimit (A.B.C.D|memory|console|file)",
	NO_STR
	"Logging control\n" "Rate limit of specified facility \n"
	"Logging to host whose ip address is A.B.C.D\n" "Logging to memory\n"
	"Logging to console\n" "Logging to file\n")

DEFSH (VTYSH_SYSLOG,
	no_ratelimit_all_cmd,
	"no log ratelimit",
	NO_STR "Logging control\n" "Rate limit of specified facility \n")


DEFSH (VTYSH_SYSLOG,
	config_logmsg_cmd,
	"logmsg "LOG_LEVELS" .MESSAGE",
	"Send a message to enabled logging destinations\n"
	LOG_LEVEL_DESC
	"The message to send\n")


DEFSH (VTYSH_SYSLOG,
	syslog_debug_monitor_cmd_vtysh,
	"debug syslog (enable|disable) (recvmsg|domask|lock|cli|all)",
	"Dbug information to moniter\n"
	"Programe name\n"
	"Enable/disatble statue\n"
	"Debug informations type name\n"
	"Type name of recive messege\n"
	"Type name of flush log queue \n"
	"Type name of lock status\n"
	"Type name of all debug\n")

DEFSH (VTYSH_SYSLOG,
	show_syslog_debug_monitor_cmd_vtysh,
	"show syslog debug",
	SHOW_STR
	"Syslog"
	"Debug status\n")
		

void vtysh_init_syslog_cmd (void)
{
	install_node (&syslog_node, NULL);
	vtysh_install_default (SYSLOG_NODE);

	//install_element (CONFIG_NODE, &vtysh_show_log_cmd);
	install_element_level (CONFIG_NODE, &show_logging_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &vtysh_syslog_cmd, VISIT_LEVE, CMD_SYNC);

	install_element_level (SYSLOG_NODE, &vtysh_exit_log_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &show_logging_cmd, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (SYSLOG_NODE, &terminal_monitor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &terminal_no_monitor_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &log_rule_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &no_log_rule_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &no_log_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &clear_log_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &log_timestamp_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &log_no_timestamp_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &show_log_file_or_memroy_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (SYSLOG_NODE, &ratelimit_rule_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &no_ratelimit_rule_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &no_ratelimit_all_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &log_filesize_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &no_log_filesize_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &log_memsize_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &no_log_memsize_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (SYSLOG_NODE, &config_logmsg_cmd, MANAGE_LEVE, CMD_LOCAL);	
	/* 注册 log 输出开关相关命令 */
	install_element_level (CONFIG_NODE, &syslog_debug_monitor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_syslog_debug_monitor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

}

