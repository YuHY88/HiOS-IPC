#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "lib/ifm_common.h"


static struct cmd_node statis_node =
{
	STATIS_NODE,
	"%s(config-statis)#",
};


DEFUNSH (VTYSH_STATIS,
		statis_mode_enable,
		statis_mode_enable_cmd,
		"statis",
		"statis command node\n")
{
	vty->node = STATIS_NODE;

	return CMD_SUCCESS;
}

DEFUNSH (VTYSH_STATIS,
	vtysh_exit_statis,
	vtysh_exit_statis_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}

ALIAS (vtysh_exit_statis,
	   vtysh_quit_statis_cmd,
	   "quit",
	   "Exit current mode and down to previous mode\n");

		

DEFSH (VTYSH_STATIS,
       statis_test_cmd,
       "statis test",
       "Statistic\n"
       "Test\n");


DEFSH (VTYSH_STATIS,
		statis_monitor_add_cmd,
		"statis port-type (eth|eth-sfp|equ|soft|env|vuni|e1|tunnel) USP",
		"Statistic\n"
		"Port-type\n"		
		"ETH\n"
		"ETH SFP\n"
		"EQU\n"
		"SOFT\n"
		"ENV\n"
		"VUNI\n"
		"E1\n"
		"TUNNEL\n"
		"USP\n");



DEFSH (VTYSH_STATIS,
		statis_monitor_add_cmd2,
		"statis port-type (cfm|vs|vplspw|pw) <1-65535>",
		"Statistic\n"
		"Port-type\n"
		"CFM\n"
		"VS\n"
		"VPLSPW\n"
		"PW\n"
		"ID\n");

DEFSH(VTYSH_STATIS,
	statis_show_register_info_cmd,
	"show statis register",
	SHOW_STR
	"Statis\n"
	"Register info\n");



DEFSH(VTYSH_STATIS,
	statis_debug_monitor_cmd,
	"debug statis (enable|disable) (register|data-collect|get|get-next|set|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Type name of register messege\n"
	"Type name of data-collect messege\n"
	"Type name of get log queue \n"
	"Type name of get-next log queue \n"
	"Type name of set status\n"
	"Type name of all debug\n");


DEFSH(VTYSH_STATIS,
	show_statis_debug_monitor_cmd,
	"show statis debug",
	SHOW_STR
	"Syslog"
	"Debug status\n");





void vtysh_init_statis_cmd (void)
{
	install_node (&statis_node, NULL);
	vtysh_install_default (STATIS_NODE);

	/* statis enter cmd */
	install_element(CONFIG_NODE, &statis_mode_enable_cmd, CMD_LOCAL);

	/* common cmd */
	install_element(STATIS_NODE, &vtysh_exit_statis_cmd, CMD_LOCAL);
	install_element(STATIS_NODE, &vtysh_quit_statis_cmd, CMD_LOCAL);
	install_element(STATIS_NODE, &statis_test_cmd, CMD_LOCAL);
	install_element(STATIS_NODE, &statis_monitor_add_cmd, CMD_LOCAL);
	install_element(STATIS_NODE, &statis_monitor_add_cmd2, CMD_LOCAL);
	install_element(STATIS_NODE, &statis_show_register_info_cmd, CMD_LOCAL);

	/* new debug */
	install_element(CONFIG_NODE, &statis_debug_monitor_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &show_statis_debug_monitor_cmd, CMD_LOCAL);
}



