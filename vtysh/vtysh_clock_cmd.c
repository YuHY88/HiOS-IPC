/**
 * \page cmds_ref_clock CLOCK
 * - \subpage modify_log_clock
 * 
 */
 
/**
 * \page modify_log_clock Modify Log
 * \section clock-v007r004 HiOS-V007R004
 *  -# 
 * \section clock-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "lib/ifm_common.h"
#include "vtysh.h"


/* debug 日志发送到 syslog 使能状态设置 */
DEFSH (VTYSH_CLOCK,clock_log_level_ctl_cmd_vtysh,
    "debug clock (enable|disable) (common|event|info|package|error|all)",
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
DEFSH (VTYSH_CLOCK,clock_show_log_level_ctl_cmd_vtysh,"show debug clock",		
	SHOW_STR"Output log of debug level\n""Program name\n")	

DEFSH(VTYSH_CLOCK,
		clock_mode_cmd,
		"mode (2mb|2mh)",
		"Clock mode\n"
		"Set clock 2Mbis mode \n"
		"Set clock 2MH mode \n"
		)

DEFSH(VTYSH_CLOCK,
		clock_signal_cmd,
		"signal (in|out|in-out)",
		"Clock signal set\n"
		"Set clock signal in mode\n"
		"Set clock signal out mode\n"
		"Set clock signal in-out mode\n"
		)
DEFSH(VTYSH_CLOCK,
		clock_sa_sel_cmd,
		"sa (sa4|sa5|sa6|sa7|sa8)",
		"Clock sa channel set\n"
		"Set sa channel is sa4\n"
		"Set sa channel is sa5\n"
		"Set sa channel is sa6\n"
		"Set sa channel is sa7\n"
		"Set sa channel is sa8\n"
		)

DEFSH(VTYSH_CLOCK,
		clock_quality_level_cmd,
		"synce quality-level (dnu|prc|sec|ssua|ssub|unk)",
		"Set synce function\n"
		"Clock quality-level\n"
		"QL value is DNU\n"
		"QL value is PRC\n"
		"QL value is SEC\n"
		"QL value is SSU-A\n"
		"QL value is SSU-B\n"
		"QL value is UNK\n"
		)

DEFSH(VTYSH_CLOCK,
		clock_priority_cmd,
		"synce priority <1-8>",
		"Set synce function\n"
		"Clock priority set\n"
		"Priority value\n"
		)
	
	
DEFSH(VTYSH_CLOCK,
		no_clock_mode_cmd,
		"no mode",
		NO_STR
		"Set clock mode default\n"
		)

DEFSH(VTYSH_CLOCK,
		no_clock_signal_cmd,
		"no signal",
		NO_STR
		"Set clock signal default mode\n"
		)
DEFSH(VTYSH_CLOCK,
		no_clock_sa_sel_cmd,
		"no sa",
		NO_STR
		"Set clock sa channel default mode\n"
			)

DEFSH(VTYSH_CLOCK,
		no_clock_quality_level_cmd,
		"no synce quality-level",
		NO_STR
		"Synce\n"
		"Set clock quality-level default\n"
		)

DEFSH(VTYSH_CLOCK,
		no_clock_priority_cmd,
		"no synce priority",
		NO_STR
		"Synce\n"
		"Set port synce function disable\n"
		)

void vtysh_init_clock_cmd(void)
{

	install_element_level (CLOCK_NODE, &clock_mode_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CLOCK_NODE, &clock_signal_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CLOCK_NODE, &clock_sa_sel_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CLOCK_NODE, &clock_quality_level_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CLOCK_NODE, &clock_priority_cmd,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (CLOCK_NODE, &no_clock_mode_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CLOCK_NODE, &no_clock_signal_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CLOCK_NODE, &no_clock_sa_sel_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CLOCK_NODE, &no_clock_quality_level_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CLOCK_NODE, &no_clock_priority_cmd,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (CONFIG_NODE, &clock_log_level_ctl_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &clock_show_log_level_ctl_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);

}
