/**
 * \page cmds_ref_web WEB
 * - \subpage modify_log_web
 * 
 */
 
/**
 * \page modify_log_web Modify Log
 * \section web-v007r004 HiOS-V007R004
 *  -# 
 * \section web-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"

static struct cmd_node web_node =
{
	WEB_NODE,
	"%s(config-web)#",
};

DEFUNSH(VTYSH_WEB,
		web_mode_enable,
		web_mode_enable_cmd,
		"web",
		"web command node\n")
{
	vty->node = WEB_NODE;

	return CMD_SUCCESS;
}

DEFUNSH(VTYSH_WEB,
	vtysh_exit_web,
	vtysh_exit_web_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	printf("%s %d\n",__FUNCTION__,__LINE__);
	return vtysh_exit(vty);
}

DEFSH(VTYSH_WEB,
	web_version_config_cmd,
	"web version <1-3>",
	"web\n"
	"version of web\n"
	"version number\n");


DEFSH(VTYSH_WEB, web_log_level_ctl_cmd_vtysh, "debug web(enable | disable)",
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n");

DEFSH(VTYSH_WEB, web_show_log_level_ctl_cmd_vtysh, "show debug web",
	SHOW_STR"Output log of debug level\n""Program name\n");

DEFSH(VTYSH_WEB,
       debug_web_packet_enable_cmd,
       "debug web packet (receive|send)  enable",
	   "Debug control\n"
	   "web module\n"
	   "packet debug\n"
	   "receive\n"
	   "send\n"
	   "enable\n");



DEFSH(VTYSH_WEB,
       debug_web_packet_disable_cmd,
       "debug web packet (receive|send)  disable",
	   "Debug control\n"
	   "web module\n"
	   "packet debug\n"
	   "receive\n"
	   "send\n"
	   "disable\n");

DEFSH(VTYSH_WEB,
       debug_web_packet_display_cmd,
       "debug web packet  mode (char | int | both)",
	   "Debug control\n"
	   "web module\n"
	   "packet debug\n"
	   "display mode\n"
	   "display format char \n"
	   "display format int  \n"
	   "display format both \n");

DEFSH(VTYSH_WEB,
       show_debug_web_packet_display_cmd,
       "show debug web packet",
	   SHOW_STR
	   "Debug control\n"
	   "web module\n"
	   "packet debug\n"
	   "display mode\n");

void vtysh_init_web_cmd()
{
	install_node(&web_node, NULL);
	vtysh_install_default(WEB_NODE);

	install_element_level(CONFIG_NODE, &web_mode_enable_cmd, VISIT_LEVE,CMD_SYNC);

	install_element_level(WEB_NODE, &vtysh_exit_web_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level(WEB_NODE, &web_version_config_cmd, CONFIG_LEVE_5, CMD_LOCAL);

	/* debug cmd */
	install_element_level(CONFIG_NODE, &web_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_SYNC); 
	install_element_level(CONFIG_NODE, &web_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

	install_element_level(CONFIG_NODE, &debug_web_packet_enable_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level(CONFIG_NODE, &debug_web_packet_disable_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level(CONFIG_NODE, &debug_web_packet_display_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_debug_web_packet_display_cmd, MANAGE_LEVE, CMD_LOCAL);
}

