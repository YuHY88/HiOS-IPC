/**
 * \page EFM_COMMOND EFM
 * - \subpage efm_enable_cmd_vtysh
 * - \subpage efm_mode_active_cmd_vtysh
 * - \subpage efm_rem_loopback_cmd_vtysh
 * - \subpage efm_rem_loopback_time_cmd_vtysh
 * - \subpage efm_remote_loopback_start_cmd_vtysh
 * - \subpage efm_remote_loopback_mac_swap_cmd_vtysh
 * - \subpage efm_link_monitor_cmd_vtysh
 * - \subpage efm_link_monitor_on_cmd_vtysh
 * - \subpage efm_event_log_size_cmd_vtysh
 * - \subpage clear_efm_event_log_cmd_vtysh
 * - \subpage efm_sym_period_event_tw_cmd_vtysh
 * - \subpage efm_err_frame_period_event_tw_cmd_vtysh
 * - \subpage efm_err_frame_second_event_tw_cmd_vtysh
 * - \subpage efm_link_event_notify_set_cmd_vtysh
 * - \subpage efm_uni_dir_cmd_vtysh
 * - \subpage efm_link_timer_cmd_vtysh
 * - \subpage efm_pdu_timer_cmd_vtysh
 * - \subpage clear_efm_statsitics_cmd_vtysh
 * - \subpage show_efm_statsitics_cmd_vtysh
 * - \subpage show_efm_ethernet_statsitics_cmd_vtysh
 * - \subpage show_efm_cmd_vtysh
 * - \subpage show_efm_interface_ethernet_cmd_vtysh
 * - \subpage show_efm_discovery_cmd_vtysh
 * - \subpage show_efm_ethernet_discovery_cmd_vtysh
 * - \subpage show_efm_status_cmd_vtysh
 * - \subpage show_efm_ethernet_status_cmd_vtysh
 * - \subpage show_efm_event_log_cmd_vtysh
 * - \subpage show_efm_ethernet_event_log_cmd_vtysh
 * - \subpage efm_err_frame_event_tw_cmd_vtysh
 */
#include <zebra.h>
#include <lib/ifm_common.h>
#include "lib/command.h"
#include "vtysh.h"
#include "../l2/efm/efm_cmd.h"
/*----------------------EFM start-------------------------*/
/**
 * \page efm_enable_cmd_vtysh [no]efm enable
 * - 功能说明 \n
 *   使能/去使能EFM功能
 * - 命令格式 \n
 *   efm enable\n
 *   no efm enable
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   EFM去使能
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *   无
 *
 */

DEFSH (VTYSH_L2,
     efm_enable_cmd_vtysh,
     "efm enable",
     "efm node\n"
     "Enable EFM\n")

DEFSH (VTYSH_L2,
     no_efm_enable_cmd_vtysh,
     "no efm enable",
     NO_STR
    "efm node\n" 
     "Enable EFM\n")
/**
 * \page efm_mode_active_cmd_vtysh efm mode (active |passive)
 * - 功能说明 \n
 *   设置EFM模式
 * - 命令格式 \n
 *   efm mode (active |passive)
 * - 参数说明 \n
 *   |参数   |说明          |
 *   |------|------------ |
 *   |active   |EFM主动模式|
 *   |passive  |EFM被动模式|
 * - 缺省情况 \n
 *   passive模式
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm discovery\n
 *     show efm status\n
 *     show efm discovery interface [trunk]gigabitethernet USP\n
 *     show efm status interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无
 *
 */

DEFSH (VTYSH_L2,
	efm_mode_active_cmd_vtysh,
	"efm mode (active |passive)",
         "efm node\n"
         "EFM Mode\n"
         "Active\n"
        "Passive\n")
        
        DEFSH (VTYSH_L2,
	efm_agent_cmd_vtysh,
			"efm agent enable",
			"efm node\n"
			"EFM Agent\n"
			"Enable\n")
	
	 DEFSH (VTYSH_L2,
	efm_no_agent_cmd_vtysh,
		"no efm agent enable",
		NO_STR
		"efm node\n"
		"EFM Agent\n"
		"Enable\n")


	DEFSH (VTYSH_L2,
		efm_agent_ip_cmd_vtysh,
		"efm agent-ip A.B.C.D",
		"efm node\n"
		"EFM Agent Ip\n"
		"IP format A.B.C.D\n")
		
	DEFSH (VTYSH_L2,
		no_efm_agent_ip_cmd_vtysh,
		"no efm agent-ip",
		NO_STR
		"efm node\n"
		"EFM Agent Ip\n")
	DEFSH (VTYSH_L2,
		efm_agent_id_cmd_vtysh,
		"efm agent-id <65537-16711678>",
		"efm node\n"
		"Set NE ID\n"
  		"Net-Element ID value\n")

/**
 * \page efm_rem_loopback_cmd_vtysh [no]efm remote-loopback support 
 * - 功能说明 \n
 *   配置支持远端环回请求
 * - 命令格式 \n
 *   efm remote-loopback support \n
 *   no  efm remote-loopback support 
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   支持远端环回请求
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm discovery \n
 *     show efm discovery interface [trunk]gigabitethernet USP 
 * - 使用举例 \n
 *   无
 */
	DEFSH (VTYSH_L2,
			efm_agent_hostname_cmd_vtysh,
			"efm agent-hostname  NAMES",
			EFM_STR
			"EFM Agent Hostname\n"
			"Agent Name\n")

  	DEFSH (VTYSH_L2,
		no_efm_agent_hostname_cmd_vtysh,
		"no efm agent-hostname ",
		NO_STR
		EFM_STR
		"EFM Agent Hostname\n")

DEFSH (VTYSH_L2,
	efm_rem_loopback_cmd_vtysh,
	 "efm remote-loopback support ",
	    "Efm node\n"
	    "Remote Loopback\n"
	    "Configure Remote Loopback Support\n")
/**
 * \page efm_rem_loopback_time_cmd_vtysh [no]efm remote-loopback timeout <1-10>
 * - 功能说明 \n
 *   设置远端环回超时时间 
 * - 命令格式 \n
 *   efm remote-loopback timeout <1-10> \n
 *   no efm remote-loopback timeout
 * - 参数说明 \n
 *   |参数       |说明       |
 *   |----------|-----------|
 *   |timeout |超时时间,1-10(seconds) |
 * - 缺省情况 \n
 *   默认超时时间是5s
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm status interface gigabitethernet \n
 *     show efm status interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无
 *
 */

DEFSH (VTYSH_L2,
	 efm_rem_loopback_time_cmd_vtysh,
	"efm remote-loopback timeout <1-10>",
	   "Efm node\n"
	   "Remote Loopback\n"
	   "Remote Loopback Timeout\n"
	   "Timeout value (1-10) secs\n")

DEFSH (VTYSH_L2,
	no_efm_rem_loopback_st_cmd_vtysh,
	"no efm remote-loopback support {timeout}",
	NO_STR
	 "Efm node\n"
	"Remote Loopback\n"
	"Configure Remote Loopback Support\n"
	"Remote Loopback Timeout\n")

DEFSH (VTYSH_L2,
		no_efm_rem_loopback_ts_cmd_vtysh,
		"no efm remote-loopback timeout {support}",
		NO_STR
		 "Efm node\n"
		"Remote Loopback\n"
		"Remote Loopback Timeout\n"
		"Configure Remote Loopback Support\n")
/**
 * \page efm_remote_loopback_start_cmd_vtysh [no]efm remote-loopback enable
 * - 功能说明 \n
 *   使能远端环回测试
 * - 命令格式 \n
 *   efm remote-loopback enable \n
 *   no efm remote-loopback enable
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   远端环回测试去使能
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能，EFM建立连接，EFM模式为active模式
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm \n
 *     show efm interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无
 *
 */

DEFSH (VTYSH_L2,
	efm_remote_loopback_start_cmd_vtysh,
	"efm remote-loopback enable",
	"efm node\n"
	"Remote Loopback\n"
	"Start Remote Loopback\n")
DEFSH (VTYSH_L2,
	no_efm_remote_loopback_start_cmd_vtysh,
	"no efm remote-loopback enable",
	NO_STR
	"efm node\n"
	"Remote Loopback\n"
	"Start Remote Loopback\n")
/**
 * \page efm_remote_loopback_mac_swap_cmd_vtysh [no]efm remote-loopback mac-swap
 * - 功能说明 \n
 *   配置支持远端环回过程中MAC交换
 * - 命令格式 \n
 *   efm remote-loopback mac-swap \n
 *   no efm remote-loopback mac-swap
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   默认远端环回过程不进行MAC交换
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm status \n
 *     show efm status interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无
 */


DEFSH (VTYSH_L2,	
                  efm_remote_loopback_mac_swap_cmd_vtysh,
		"efm remote-loopback mac-swap",
		"efm node\n"
		"Remote Loopback\n"
		"Start Mac Swap\n")

DEFSH (VTYSH_L2,	
                  efm_remote_loopback_no_mac_swap_cmd_vtysh,
		"no efm remote-loopback mac-swap",
		NO_STR
		"efm node\n"
		"Remote Loopback\n"
		"Start Mac Swap\n")

/**
 * \page efm_link_monitor_cmd_vtysh [no]efm link-monitor support
 * - 功能说明 \n
 *   配置支持链路事件检测
 * - 命令格式 \n
 *   efm link-monitor support \n
 *   no efm link-monitor support
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   支持链路事件检测
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *   无
 *
 */


DEFSH (VTYSH_L2,
	efm_link_monitor_cmd_vtysh,
	"efm link-monitor support",
	"efm node\n"
	"Link Monitor\n"
	"Configure Link Monitor Support\n")

DEFSH (VTYSH_L2,
	no_efm_link_monitor_cmd_vtysh,
	"no efm link-monitor support",
	NO_STR
	"efm node\n"
	"Link Monitor\n"
	"Configure Link Monitor Support\n")
/**
 * \page  efm_link_monitor_on_cmd_vtysh [no]efm link-monitor enable 
 * - 功能说明 \n
 *   使能链路检测
 * - 命令格式 \n
 *   efm link-monitor enable \n
 *   no efm link-monitor enable
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   链路检测去使能
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *   无
 *
 */

DEFSH (VTYSH_L2,
	efm_link_monitor_on_cmd_vtysh,
	"efm link-monitor enable",
	"efm node\n"
	"Link Monitor\n"
	"Start Link Monitoring\n")

DEFSH (VTYSH_L2,
	no_efm_link_monitor_on_cmd_vtysh,
	"no efm link-monitor enable",
	NO_STR
	"efm node\n"
	"Link Monitor\n"
	"Start Link Monitoring\n")
/**
 * \page efm_event_log_size_cmd_vtysh [no]efm event-log-number <1-256>
 * - 功能说明 \n
 *   配置记录链路事件日志的条目
 * - 命令格式 \n
 *   efm event-log-number <1-256> \n
 *   no efm event-log-number
 * - 参数说明 \n
 *   |参数       |说明       |
 *   |----------|-----------|
 *   |event-log-number |事件日志条目,1-256 |
 * - 缺省情况 \n
 *   默认记录100条
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     no efm event-log-number \n
 *     clear efm event-log \n
 *     show efm status \n
 *     show efm status interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无
 */

DEFSH (VTYSH_L2,
	efm_event_log_size_cmd_vtysh,
	"efm event-log-number <1-256>",
	"efm node\n"
	"Set the maximum number of log entries to be collected in Event Log\n"
	"Log size (1-256)\n")

DEFSH (VTYSH_L2,
	no_efm_event_log_size_cmd_vtysh,
	"no efm event-log-number",
	NO_STR
	"efm node\n"
	"Set the maximum number of log entries to be collected in Event Log\n")
/**
 * \page clear_efm_event_log_cmd_vtysh clear efm event-log
 * - 功能说明 \n
 *   清空事件日志信息
 * - 命令格式 \n
 *   clear efm event-log
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     efm event-log-number <1-256> \n
 *     no efm event-log-number \n
 *     show efm eventlog \n
 *     show efm eventlog interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无
 *
 */

DEFSH (VTYSH_L2,
	clear_efm_event_log_cmd_vtysh,
	"clear efm event-log",
	CLEAR_STR
	"efm node\n"
	"Event logs\n")
/**
 * \page efm_sym_period_event_tw_cmd_vtysh [no]efm link-monitor symbol-period threshold <0-4294967295> {window <1-4294967295>}
 * - 功能说明 \n
 *   配置错误符号周期事件检测参数,包括检测阈值和检测窗口
 * - 命令格式 \n
 *   efm link-monitor symbol-period threshold <0-4294967295> {window <1-4294967295>} \n
 *   no efm link-monitor symbol-period threshold {window}
 * - 参数说明 \n
 *   |参数             |说明                      |
 *   |----------------|-------------------------|
 *   |threshold       |检测阈值,0-4294967295|
 *   |window          |检测窗口,1-4294967295|
 * - 缺省情况 \n
 *   |参数             |说明                      |
 *   |----------------|-------------------------|
 *   |threshold       |默认值为1             |
 *   |window          |默认值为1000000       |
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm status \n
 *     show efm status interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无 
 *
 */

DEFSH (VTYSH_L2,
	efm_sym_period_event_tw_cmd_vtysh,
	"efm link-monitor symbol-period threshold <0-4294967295> {window <1-4294967295>}",
	"efm node\n"
	"Link Monitor\n"
	"Symbol Period Event Properties\n"
	"Error Threshold\n"
	"Error symbol threshold value, (0-4294967295)\n"
	"Event window\n"
	"Error symbol window value, (1-4294967295)\n")

DEFSH (VTYSH_L2,
	efm_sym_period_event_wt_cmd_vtysh,
	"efm link-monitor symbol-period window <1-4294967295> {threshold <0-4294967295>}",
	"efm node\n"
	"Link Monitor\n"
	"Symbol Period Event Properties\n"
	"Event window\n"
	"Error symbol window value, (1-4294967295)\n"
	"Error Threshold\n"
	"Error symbol threshold value, (0-4294967295)\n")



DEFSH (VTYSH_L2,
	no_efm_sym_period_event_tw_cmd_vtysh,
	"no efm link-monitor symbol-period threshold {window}",
	NO_STR
	"efm node\n"
	"Link Monitor\n"
	"Symbol Period Event Properties\n"
	"Error Threshold\n"
	"Event window\n")

DEFSH (VTYSH_L2,
	no_efm_sym_period_event_wt_cmd_vtysh,
	"no efm link-monitor symbol-period window {threshold }",
	NO_STR
	"efm node\n"
	"Link Monitor\n"
	"Symbol Period Event Properties\n"
	"Event window\n"
	"Error Threshold\n")
 
DEFSH (VTYSH_L2,
	efm_err_frame_event_wt_cmd_vtysh,
	"efm link-monitor frame window <1-60> {threshold <0-4294967295>}",
	"efm node\n"
	"Link Monitor\n"
	"Errored Frame Event Properties\n"
	"Event window\n"
	"Error frame window value, 1-60(sec)\n"
	"Error Threshold\n"
	"Error frame threshold value, (0-4294967295)\n")




DEFSH (VTYSH_L2,
	no_efm_err_frame_event_tw_cmd_vtysh,
	"no efm link-monitor frame threshold {window}",
	NO_STR
	"efm node\n"
	"Link Monitor\n"
	"Errored Frame Event Properties\n"
	"Error Threshold\n"
	"Event window\n")

DEFSH (VTYSH_L2,
	no_efm_err_frame_event_wt_cmd_vtysh,
	"no efm link-monitor frame window {threshold}",
	NO_STR
	"efm node\n"
	"Link Monitor\n"
	"Errored Frame Event Properties\n"
	"Event window\n"
	"Error Threshold\n")

/**
 * \page efm_err_frame_period_event_tw_cmd_vtysh [no]efm link-monitor frame-period threshold <0-4294967295> {window <10000-4294967295>} 
 * - 功能说明 \n
 *   配置错误帧周期事件检测参数,包括检测阈值和检测窗口
 * - 命令格式 \n
 *   efm link-monitor frame-period threshold <0-4294967295> {window <10000-4294967295>} \n
 *   no efm link-monitor frame-period threshold {window}
 * - 参数说明 \n
 *   |参数             |说明                          |
 *   |----------------|-------------------------------|
 *   |threshold       |检测阈值,0-4294967295     |
 *   |window          |检测窗口,10000-4294967295 |
 * - 缺省情况 \n
 *   |参数             |说明       |
 *   |----------------|----------|
 *   |threshold       |默认值为1 |
 *   |window          |默认值为1000000 |
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm status \n
 *     show efm status interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无
 *
 */

DEFSH (VTYSH_L2,
	efm_err_frame_period_event_tw_cmd_vtysh,
	"efm link-monitor frame-period threshold <0-4294967295> {window <10000-4294967295>}",
	"efm node\n"
	"Link Monitor\n"
	"Errored Frame Period Event Properties\n"
	"Error Threshold\n"
	"Error frame period threshold value, (0-4294967295)\n"
	"Event window\n"
	"Error frame period window value, 10000-4294967295(frame)\n")

DEFSH (VTYSH_L2,
	efm_err_frame_period_event_wt_cmd_vtysh,
	"efm link-monitor frame-period  window <10000-4294967295> {threshold <0-4294967295>}",
	"efm node\n"
	"Link Monitor\n"
	"Errored Frame Period Event Properties\n"
	"Event window\n"
	"Error frame period window value, 10000-4294967295(frame)\n"
	"Error Threshold\n"
	"Error frame period threshold value, (0-4294967295)\n")


DEFSH (VTYSH_L2,
	no_efm_err_frame_period_event_tw_cmd_vtysh,
	"no efm link-monitor frame-period threshold {window}",
	NO_STR
	"efm node\n"
	"Link Monitor\n"
	"Errored Frame Period Event Properties\n"
	"Error Threshold\n"
	"Event window\n")

DEFSH (VTYSH_L2,
	no_efm_err_frame_period_event_wt_cmd_vtysh,
	"no efm link-monitor frame-period  window { threshold}",
	NO_STR
	"efm node\n"
	"Link Monitor\n"
	"Errored Frame Period Event Properties\n"
	"Event window\n"
	"Error Threshold\n")

/**
 * \page efm_err_frame_second_event_tw_cmd_vtysh [no]efm link-monitor frame-seconds threshold <0-65535> {window <10-900>}
 * - 功能说明 \n
 *   配置错误帧秒事件检测参数，包括检测阈值和检测窗口
 * - 命令格式 \n
 *   efm link-monitor frame-seconds threshold <0-65535> {window <10-900>} \n
 *   no efm link-monitor frame-seconds threshold {window}
 * - 参数说明 \n
 *   |参数             |说明                     |
 *   |----------------|------------------------|
 *   |threshold       |检测阈值,0-65535    |
 *   |window          |检测窗口,10-900(seconds)     |
 * - 缺省情况 \n
 *   |参数             |说明         |
 *   |----------------|-------------|
 *   |threshold       |默认值为1    |
 *   |window          |默认值为60s   |
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     efm使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm status \n
 *     show efm status interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无
 *
 */


DEFSH (VTYSH_L2,
	efm_err_frame_second_event_tw_cmd_vtysh,
	"efm link-monitor frame-seconds threshold <0-65535> {window <10-900>}",
	"efm node\n"
	"Link Monitor\n"
	"Errored Frame Seconds Event Properties\n"
	"Error Threshold\n"
	"Error frame seconds threshold value, (0-65535)\n"
	"Event window\n"
	"Error frame seconds window value, 10-900(sec)\n")

DEFSH (VTYSH_L2,
	efm_err_frame_second_event_wt_cmd_vtysh,
	"efm link-monitor frame-seconds  window <10-900>{threshold <0-65535>}",
	"efm node\n"
	"Link Monitor\n"
	"Errored Frame Seconds Event Properties\n"
	"Event window\n"
	"Error frame seconds window value, 10-900(sec)\n"
	"Error Threshold\n"
	"Error frame seconds threshold value, (0-65535)\n")

DEFSH (VTYSH_L2,
	no_efm_err_frame_second_event_tw_cmd_vtysh,
	"no efm link-monitor frame-seconds threshold {window}",
	NO_STR
	"efm node\n"
	"Link Monitor\n"
	"Errored Frame Seconds Event Properties\n"
	"Error Threshold\n"
	"Event window\n")

DEFSH (VTYSH_L2,
	no_efm_err_frame_second_event_wt_cmd_vtysh,
	"no efm link-monitor frame-seconds  window {threshold}",
	NO_STR
	"efm node\n"
	"Link Monitor\n"
	"Errored Frame Seconds Event Properties\n"
	"Event window\n"
	"Error Threshold\n")
/**
 * \page efm_link_event_notify_set_cmd_vtysh [no]efm event-notify (symbol |frame|frame-period |frame-second |dying-gasp|critical-event)
 * - 功能说明 \n
 *   配置局端链路事件发生是否通告远端
 * - 命令格式 \n
 *   efm event-notify (symbol |frame|frame-period |frame-second |dying-gasp|critical-event) \n
 *   no  efm event-notify (symbol |frame|frame-period |frame-second |dying-gasp|critical-event)
 * - 参数说明 \n
 *   |参数             |说明          |
 *   |----------------|-------------|
 *   |symbol          |错误符号事件    |
 *   |frame           |错误帧事件     |
 *   |frame-period    |错误帧周期事件  |
 *   |frame-second    |错误帧秒事件    |
 *   |dying-gasp      |致命故障       |
 *   |critical-event  |紧急事件       |
 * - 缺省情况 \n
 *   默认所有局端链路事件发生后通告远端
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *   无
 *
 */



DEFSH (VTYSH_L2,
	efm_link_event_notify_set_cmd_vtysh,
	"efm event-notify (symbol |frame|frame-period |frame-second |dying-gasp|critical-event)",
	"efm node\n"
	"Event notify\n"
	"Error symbol\n"
	"Error frame\n"
	"Error frame period\n"
	"Error frame second\n"
	"Dying gasp\n"
	"Critical event\n")

DEFSH (VTYSH_L2,
	no_efm_link_event_notify_cmd_vtysh,
	"no efm event-notify (symbol |frame|frame-period |frame-second|dying-gasp|critical-event)",
	NO_STR
	"efm node\n"
	"Event notify\n"
	"Error symbol\n"
	"Error frame\n"
	"Error frame period\n"
	"Error frame second\n"
	"Dying gasp\n"
	"Critical event\n")
/**
 * \page efm_uni_dir_cmd_vtysh [no]efm unidirectional-link support
 * - 功能说明 \n
 *   配置EFM单向链接支持，单纤时强制发包
 * - 命令格式 \n
 *   efm unidirectional-link support \n
 *   no efm unidirectional-link support
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   不支持单向链接，不支持单纤强制发包
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     链路link fault时，需要先配置单向链接支持，单纤时可强制发包。
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     可能影响端口协商状态
 *   - 相关命令 \n
 *     show efm discovery \n
 *     show efm discovery interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无
 *
 */

DEFSH (VTYSH_L2,
	efm_uni_dir_cmd_vtysh,
	"efm unidirectional-link support",
	"efm node\n"
	"Unidirectional link\n"
	"Configure undirectional link Support\n")

DEFSH (VTYSH_L2,
	no_efm_uni_dir_cmd_vtysh,
	"no efm unidirectional-link support",
	NO_STR
	"efm node\n"
	"Unidirectional link\n"
	"Configure undirectional link Support\n")
/**
 * \page  efm_link_timer_cmd_vtysh [no]efm timeout <2-30>
 * - 功能说明 \n
 *   配置链路发现超时时间
 * - 命令格式 \n
 *   efm timeout <2-30> \n
 *   no efm timeout
 * - 参数说明 \n
 * - 参数说明 \n
 *   |参数        |说明              |
 *   |-----------|------------------|
 *   |timeout    |超时时间,2-30(seconds) |
 * - 缺省情况 \n
 *   超时时间默认为5s
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm status \n
 *     show efm status interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无
 *
 */

DEFSH (VTYSH_L2,
	efm_link_timer_cmd_vtysh,
	"efm timeout <2-30>",
	"efm node\n"
	"Local link lost timer\n"
	"Timer value (2-30)sec\n")


DEFSH (VTYSH_L2,
	no_efm_link_timer_cmd_vtysh,
	"no efm timeout",
	NO_STR
	"efm node\n"
	"Local link lost timer\n")
/**
 * \page efm_pdu_timer_cmd_vtysh [no]efm rate <1-10>
 * - 功能说明 \n
 *   配置EFM协议报文周期
 * - 命令格式 \n
 *   efm rate <1-10> \n
 *   no efm rate
 * - 参数说明 \n
 *   |参数        |说明            |
 *   |-----------|-------------- |
 *   |rate       |发包周期，1-10(seconds) |
 * - 缺省情况 \n
 *   默认发包周期为1s
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm status \n
 *     show efm status interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无
 *
 */

DEFSH (VTYSH_L2,
	efm_pdu_timer_cmd_vtysh,
	"efm rate <1-10>",
	"efm node\n"
	"Set the time for pdu-timer\n"
	"Timer value (1-10)sec\n")

DEFSH (VTYSH_L2,
	no_efm_pdu_timer_cmd_vtysh,
	"no efm rate",
	NO_STR
	"efm node\n"
	"Set the time for pdu-timer\n")
/**
 * \page  clear_efm_statsitics_cmd_vtysh clear efm statistics
 * - 功能说明 \n
 *   清空EFM统计信息
 * - 命令格式 \n
 *   clear efm statistics
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm \n
 *     show efm interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *   无
 *      
 */

DEFSH (VTYSH_L2,
	clear_efm_statsitics_cmd_vtysh,
	"clear efm statistics",
	CLEAR_STR
	"efm node\n"
	"EFM statistics\n")
/**
 * \page show_efm_statsitics_cmd_vtysh show efm statistics
 * - 功能说明 \n
 *   显示EFM统计信息
 * - 命令格式 \n
 *   show efm statistics
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   interface模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm statistics interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *     Hios-v7.3(config)# show efm statistics               \n
 *	gigabitethernet 1/1/1                               \n
 *                                                          \n
 *	Counters:                                           \n
 *	----------                                          \n
 *	   Information OAMPDU Tx                  : 5647    \n
 *	   Information OAMPDU Rx                  : 0       \n
 *	   Event Notification OAMPDU Tx           : 0       \n
 *	   Event Notification OAMPDU Rx           : 0       \n
 *	   Loopback Control OAMPDU Tx             : 0       \n
 *	   Loopback Control OAMPDU Rx             : 0       \n
 *	   Unsupported OAMPDU Rx                  : 0       \n
 *                                                          \n
 *	Local event logs:                                   \n
 *	-----------------                                   \n
 *	   0 Errored Symbol Period records                  \n
 * 	   0 Errored Frame records                          \n
 *	   0 Errored Frame Period records                   \n
 *	   0 Errored Frame Seconds records                  \n
 *                                                          \n
 *	Remote event logs:                                  \n
 *	-----------------                                   \n
 *	   0 Errored Symbol Period records                  \n
 *	   0 Errored Frame records                          \n
 *	   0 Errored Frame Period records                   \n
 *	   0 Errored Frame Seconds records                  \n
 *                                                          \n        
 *	-----------------                                   \n     
 *
 */

DEFSH (VTYSH_L2,
	show_efm_statsitics_cmd_vtysh,
	"show efm statistics",
	CLEAR_STR
	"efm node\n"
	"EFM statistics\n")
/**
 * \page show_efm_ethernet_statsitics_cmd_vtysh show efm statistics interface [trunk]gigabitethernet USP
 * - 功能说明 \n
 *   显示某接口下EFM统计信息
 * - 命令格式 \n
 *   show efm statistics interface gigabitethernet USP
 * - 参数说明 \n
 *   |参数                |说明                           |
 *   |-------------------|-------------------------------|
 *   |gigabitethernet USP|<0-7>/<0-31>/<1-255>[.<1-4095>]|
 *   |trunk USP          |<1-128>[.<1-4095>]             |
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm statistics
 * - 使用举例 \n
 *     Hios-v7.3(config)# show efm statistics interface gigabitethernet 1/1/1        \n
 *	gigabitethernet 1/1/1                               \n
 *                                                          \n
 *	Counters:                                           \n
 *	----------                                          \n
 *	   Information OAMPDU Tx                  : 5647    \n
 *	   Information OAMPDU Rx                  : 0       \n
 *	   Event Notification OAMPDU Tx           : 0       \n
 *	   Event Notification OAMPDU Rx           : 0       \n
 *	   Loopback Control OAMPDU Tx             : 0       \n
 *	   Loopback Control OAMPDU Rx             : 0       \n
 *	   Unsupported OAMPDU Rx                  : 0       \n
 *                                                          \n
 *	Local event logs:                                   \n
 *	-----------------                                   \n
 *	   0 Errored Symbol Period records                  \n
 * 	   0 Errored Frame records                          \n
 *	   0 Errored Frame Period records                   \n
 *	   0 Errored Frame Seconds records                  \n
 *                                                          \n
 *	Remote event logs:                                  \n
 *	-----------------                                   \n
 *	   0 Errored Symbol Period records                  \n
 *	   0 Errored Frame records                          \n
 *	   0 Errored Frame Period records                   \n
 *	   0 Errored Frame Seconds records                  \n
 *                                                          \n        
 *	-----------------                                   \n    
 *
 */

DEFSH (VTYSH_L2,
	show_efm_ethernet_statsitics_cmd_vtysh,
	"show efm statistics interface gigabitethernet USP",
	CLEAR_STR
	"efm node\n"
	"EFM statistics\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR)

DEFSH (VTYSH_L2,
	show_efm_trunk_statsitics_cmd_vtysh,
	"show efm statistics interface trunk TRUNK",
	CLEAR_STR
	"efm node\n"
	"EFM statistics\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_TRUNK_STR
	CLI_INTERFACE_TRUNK_VHELP_STR)
/**
 * \page show_efm_cmd_vtysh show efm 
 * - 功能说明 \n
 *   显示EFM状态信息
 * - 命令格式 \n
 *   show efm
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   interface模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *     Hios-v7.3(config)# show efm                                \n
 *	gigabitethernet 1/1/1                                     \n
 *                                                                \n   
 *	Discovery State Machine Details:                          \n
 *	--------------------------------                          \n
 *	EFM Discovery Machine State:              Fault           \n
 *	Local Parser State:                       Forward         \n
 *	Local Multiplexer State:                  Forward         \n
 *	Remote Parser State:                      Forward         \n
 *	Remote Multiplexer State:                 Forward         \n
 *                                                                \n
 *	Local Client:                                             \n
 *	-------------                                             \n
 *                                                                \n
 *	    Symbol Period Error:                                  \n
 *	       Window:                           1000000 Symbols  \n
 *	       Threshold:                        1 Symbols        \n
 *	       Last Window Symbols Errors:       0 Symbols        \n
 *	       Total Symbols Errors:             0 Symbols        \n
 *	       Total Symbols Errors Events:      0 Events         \n
 *	       Relative Timestamp of the Event:  0 x 100 milliseconds \n
 *	                                                              \n
 *	    Frame Error:                                              \n
 *	       Window:                           1 seconds            \n
 *	       Threshold:                        1 Error Frames       \n
 *	       Last Window Frame Errors:         0 Frames             \n
 *	       Total Frame Errors:               0 Frames             \n
 *	       Total Frame Errors Events:        0 Events             \n
 *	       Relative Timestamp of the Event:  0 x 100 milliseconds \n
 *	                                                              \n
 *	    Frame Period Error:                                       \n
 *	       Window:                           1000000 Frames       \n
 *	       Threshold:                        1 Error Frames       \n
 *	       Last Window Frame Errors:         0 Frames             \n
 *	       Total Frame Errors:               0 Frames             \n
 *	       Total Frame Period Errors Events: 0 Events             \n
 *	       Relative Timestamp of the Event:  0 x 100 milliseconds \n
 *                                                                    \n
 *	    Frame Seconds Error:                                      \n
 *	       Window:                           60 seconds           \n
 *	       Threshold:                        1 Error Seconds      \n
 *	       Last Window Frame Second Errors:  0 Error Seconds      \n
 * 	       Total Frame Second Errors:        0 Error Seconds      \n
 *	       Total Frame Second Errors Events: 0 Events             \n
 *	       Relative Timestamp of the Event:  0 x 100 milliseconds \n
 *
 */

DEFSH (VTYSH_L2,
	show_efm_cmd_vtysh,
	"show efm",
	SHOW_STR
	"efm node\n")
/**
 * \page show_efm_interface_ethernet_cmd_vtysh show efm interface [trunk]gigabitethernet USP
 * - 功能说明 \n
 *   显示某接口下EFM状态信息
 * - 命令格式 \n
 *   show efm interface [trunk]gigabitethernet USP
 * - 参数说明 \n
 *   |参数                |说明                           |
 *   |-------------------|-------------------------------|
 *   |gigabitethernet USP|<0-7>/<0-31>/<1-255>[.<1-4095>]|
 *   |trunk USP          |<1-128>[.<1-4095>]             |
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm
 * - 使用举例 \n
 *     Hios-v7.3(config)# show efm interface gigabitethernet 1/1/3 \n
 *	gigabitethernet 1/1/3                                     \n
 *                                                                \n   
 *	Discovery State Machine Details:                          \n
 *	--------------------------------                          \n
 *	EFM Discovery Machine State:              Fault           \n
 *	Local Parser State:                       Forward         \n
 *	Local Multiplexer State:                  Forward         \n
 *	Remote Parser State:                      Forward         \n
 *	Remote Multiplexer State:                 Forward         \n
 *                                                                \n
 *	Local Client:                                             \n
 *	-------------                                             \n
 *                                                                \n
 *	    Symbol Period Error:                                  \n
 *	       Window:                           1000000 Symbols  \n
 *	       Threshold:                        1 Symbols        \n
 *	       Last Window Symbols Errors:       0 Symbols        \n
 *	       Total Symbols Errors:             0 Symbols        \n
 *	       Total Symbols Errors Events:      0 Events         \n
 *	       Relative Timestamp of the Event:  0 x 100 milliseconds \n
 *	                                                              \n
 *	    Frame Error:                                              \n
 *	       Window:                           1 seconds            \n
 *	       Threshold:                        1 Error Frames       \n
 *	       Last Window Frame Errors:         0 Frames             \n
 *	       Total Frame Errors:               0 Frames             \n
 *	       Total Frame Errors Events:        0 Events             \n
 *	       Relative Timestamp of the Event:  0 x 100 milliseconds \n
 *	                                                              \n
 *	    Frame Period Error:                                       \n
 *	       Window:                           1000000 Frames       \n
 *	       Threshold:                        1 Error Frames       \n
 *	       Last Window Frame Errors:         0 Frames             \n
 *	       Total Frame Errors:               0 Frames             \n
 *	       Total Frame Period Errors Events: 0 Events             \n
 *	       Relative Timestamp of the Event:  0 x 100 milliseconds \n
 *                                                                    \n
 *	    Frame Seconds Error:                                      \n
 *	       Window:                           60 seconds           \n
 *	       Threshold:                        1 Error Seconds      \n
 *	       Last Window Frame Second Errors:  0 Error Seconds      \n
 * 	       Total Frame Second Errors:        0 Error Seconds      \n
 *	       Total Frame Second Errors Events: 0 Events             \n
 *	       Relative Timestamp of the Event:  0 x 100 milliseconds \n
 *
 */

DEFSH (VTYSH_L2,
	show_efm_interface_ethernet_cmd_vtysh,
	"show efm interface gigabitethernet USP",
	SHOW_STR
	"efm node\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR)

DEFSH (VTYSH_L2,
	show_efm_interface_trunk_cmd_vtysh,
	"show efm interface trunk TRUNK",

	SHOW_STR
	"efm node\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_TRUNK_STR
	CLI_INTERFACE_TRUNK_VHELP_STR)
/**
 * \page show_efm_discovery_cmd_vtysh show efm discovery
 * - 功能说明 \n
 *   显示EFM发现信息
 * - 命令格式 \n
 *   show efm discovery
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   interface模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm discovery interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *     Hios-v7.3(config)# show efm discovery                   \n
 * 	gigabitethernet 1/1/1                                  \n                             
 *                                                             \n
 * 	Local client:                                          \n
 * 	-------------                                          \n
 * 	Administrative configurations:                         \n
 *   	Mode:                              passive             \n
 *   	Unidirection:                      not support         \n
 *   	Link monitor:                      support             \n
 *   	Remote Loopback:                   support             \n
 *   	MIB retrieval:                     not support         \n
 *   	MTU Size    :                      1518                \n
 *   	Operational status:                                    \n
 *   	Port status:                       not operational     \n   
 *
 */

DEFSH (VTYSH_L2,
	show_efm_discovery_cmd_vtysh,
	"show efm discovery",
	SHOW_STR
	"efm node\n"
	"EFM discovery state information\n")
/**
 * \page show_efm_ethernet_discovery_cmd_vtysh show efm discovery interface [trunk]gigabitethernet USP
 * - 功能说明 \n
 *   显示某接口下EFM发现信息
 * - 命令格式 \n
 *   show efm discovery interface [trunk]gigabitethernet US
 * - 参数说明 \n
 *   |参数                |说明                           |
 *   |-------------------|-------------------------------|
 *   |gigabitethernet USP|<0-7>/<0-31>/<1-255>[.<1-4095>]|
 *   |trunk USP          |<1-128>[.<1-4095>]             |
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm discovery
 * - 使用举例 \n
 *     Hios-v7.3(config)# show efm discovery interface gigabitethernet 1/1/4 \n
 * 	gigabitethernet 1/1/4                                  \n                             
 *                                                             \n
 * 	Local client:                                          \n
 * 	-------------                                          \n
 * 	Administrative configurations:                         \n
 *   	Mode:                              passive             \n
 *   	Unidirection:                      not support         \n
 *   	Link monitor:                      support             \n
 *   	Remote Loopback:                   support             \n
 *   	MIB retrieval:                     not support         \n
 *   	MTU Size    :                      1518                \n
 *   	Operational status:                                    \n
 *   	Port status:                       not operational     \n 
 *
 */

DEFSH (VTYSH_L2,
	show_efm_ethernet_discovery_cmd_vtysh,
	"show efm discovery interface gigabitethernet USP",
	SHOW_STR
	"efm node\n"
	"EFM discovery state information\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR)

DEFSH (VTYSH_L2,
	show_efm_trunk_discovery_cmd_vtysh,
	"show efm discovery interface trunk TRUNK",
	SHOW_STR
	"efm node\n"
	"EFM discovery state information\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_TRUNK_STR
	CLI_INTERFACE_TRUNK_VHELP_STR)
/**
 * \page show_efm_status_cmd_vtysh show efm status
 * - 功能说明 \n
 *   显示EFM参数配置信息
 * - 命令格式 \n
 *   show efm status
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   interface模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm status interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *     Hios-v7.3(config)# show efm status                              \n
 *	gigabitethernet 1/1/1                                          \n
 *	                                                               \n
 *	General:                                                       \n
 *	-------                                                        \n
 *	  Mode:                              passive                   \n
 *	  PDU max rate:                      10 packets per second     \n
 *	  PDU min rate:                      1 packet per 1 second     \n
 *	  Link timeout:                      5 seconds                 \n
 *	  Remote loopback  timeout:          5 seconds                 \n
 *	  Remote loopback mac swap:          not support               \n
 *	                                                               \n
 *	Link Monitoring:                                               \n
 *	----------------                                               \n
 *	    Status:                          support                   \n 
 *                                                                     \n
 *	    Event log size:                  100 Entries               \n
 *	                                                               \n
 *	    Symbol Period Error:                                       \n
 *	       Window:                       1000000 symbols           \n
 *	       Threshold:                    1 error symbols           \n
 *                                                                     \n
 *	    Frame Error:                                               \n
 *	       Window:                       1 seconds                 \n
 *	       Threshold:                    1 error frames            \n
 *	                                                               \n
 *	    Frame Period Error:                                        \n
 *	       Window:                       1000000  frames           \n
 *	       Threshold:                    1 error frames            \n
 *	                                                               \n
 *	    Frame Seconds Error:                                       \n
 *	       Window:                       60 seconds                \n
 *	       Threshold:                    1 error seconds           \n
 *	----------------                                               \n
 *
 */

DEFSH (VTYSH_L2,
     show_efm_status_cmd_vtysh,
     "show efm status",
	SHOW_STR
	"efm node\n"
	"EFM status\n")
/**
 * \page show_efm_ethernet_status_cmd_vtysh show efm status interface [trunk]gigabitethernet USP
 * - 功能说明 \n
 *   显示某接口下的EFM参数配置信息
 * - 命令格式 \n
 *   show efm status interface [trunk]gigabitethernet USP
 * - 参数说明 \n
 *   |参数                |说明                           |
 *   |-------------------|-------------------------------|
 *   |gigabitethernet USP|<0-7>/<0-31>/<1-255>[.<1-4095>]|
 *   |trunk USP          |<1-128>[.<1-4095>]             |
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm status
 * - 使用举例 \n
 *     Hios-v7.3(config)# show efm status interface gigabitethernet 1/1/3 \n 
 *	gigabitethernet 1/1/3                                          \n
 *	                                                               \n
 *	General:                                                       \n
 *	-------                                                        \n
 *	  Mode:                              passive                   \n
 *	  PDU max rate:                      10 packets per second     \n
 *	  PDU min rate:                      1 packet per 1 second     \n
 *	  Link timeout:                      5 seconds                 \n
 *	  Remote loopback  timeout:          5 seconds                 \n
 *	  Remote loopback mac swap:          not support               \n
 *	                                                               \n
 *	Link Monitoring:                                               \n
 *	----------------                                               \n
 *	    Status:                          support                   \n 
 *                                                                     \n
 *	    Event log size:                  100 Entries               \n
 *	                                                               \n
 *	    Symbol Period Error:                                       \n
 *	       Window:                       1000000 symbols           \n
 *	       Threshold:                    1 error symbols           \n
 *                                                                     \n
 *	    Frame Error:                                               \n
 *	       Window:                       1 seconds                 \n
 *	       Threshold:                    1 error frames            \n
 *	                                                               \n
 *	    Frame Period Error:                                        \n
 *	       Window:                       1000000  frames           \n
 *	       Threshold:                    1 error frames            \n
 *	                                                               \n
 *	    Frame Seconds Error:                                       \n
 *	       Window:                       60 seconds                \n
 *	       Threshold:                    1 error seconds           \n
 *	----------------                                               \n
 *
 */

DEFSH (VTYSH_L2,
     show_efm_ethernet_status_cmd_vtysh,
     "show efm status interface gigabitethernet USP",
	SHOW_STR
	"efm node\n"
	"EFM status\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR)

DEFSH (VTYSH_L2,
     show_efm_trunk_status_cmd_vtysh,
     "show efm status interface trunk TRUNK",
	SHOW_STR
	"efm node\n"
	"EFM status\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_TRUNK_STR
	CLI_INTERFACE_TRUNK_VHELP_STR)
/**
 * \page show_efm_event_log_cmd_vtysh show efm eventlog
 * - 功能说明 \n
 *   显示EFM链路事件日志信息
 * - 命令格式 \n
 *   show efm eventlog
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   interface模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm eventlog interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *     Hios-v7.3(config)# show efm eventlog                  \n
 *                                                           \n
 *	 IFNAME: gigabitethernet 1/1/1                       \n
 * 	--------------------------------                     \n
 * 	Event Log details for logIndex: 1                    \n
 * 	-----------------------------------                  \n
 * 	Time stamp of event       : 1211                     \n
 * 	oui                       : 64 1e 81                 \n
 * 	event type                : LINK_FAULT_EVENT         \n
 * 	event location            : LOCAL                    \n
 *
 */

DEFSH (VTYSH_L2,
	show_efm_event_log_cmd_vtysh,
	"show efm eventlog",
	SHOW_STR
	"efm node\n"
	"EFM event logs\n")
/**
 * \page show_efm_ethernet_event_log_cmd_vtysh show efm eventlog interface [trunk]gigabitethernet USP
 * - 功能说明 \n
 *   显示某接口下的EFM链路事件日志信息
 * - 命令格式 \n
 *   show efm eventlog interface [trunk]gigabitethernet USP
 * - 参数说明 \n
 *   |参数                |说明                           |
 *   |-------------------|-------------------------------|
 *   |gigabitethernet USP|<0-7>/<0-31>/<1-255>[.<1-4095>]|
 *   |trunk USP          |<1-128>[.<1-4095>]             |
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm eventlog
 * - 使用举例 \n
 *     Hios-v7.3(config)# show efm eventlog interface gigabitethernet 1/1/3  \n
 *                                                           \n
 *	 IFNAME: gigabitethernet 1/1/3                       \n
 * 	--------------------------------                     \n
 * 	Event Log details for logIndex: 1                    \n
 * 	-----------------------------------                  \n
 * 	Time stamp of event       : 1211                     \n
 * 	oui                       : 64 1e 81                 \n
 * 	event type                : LINK_FAULT_EVENT         \n
 * 	event location            : LOCAL                    \n
 *
 */

DEFSH (VTYSH_L2,
	show_efm_ethernet_event_log_cmd_vtysh,
	"show efm eventlog interface gigabitethernet USP",
	SHOW_STR
	"efm node\n"
	"EFM event logs\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR)

DEFSH (VTYSH_L2,
	show_efm_trunk_event_log_cmd_vtysh,
	"show efm eventlog interface trunk TRUNK",
	SHOW_STR
	"efm node\n"
	"EFM event logs\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_TRUNK_STR
	CLI_INTERFACE_TRUNK_VHELP_STR)

/**
 * \page efm_err_frame_event_tw_cmd_vtysh [no]efm link-monitor frame threshold <0-4294967295> {window <1-60>}
 * - 功能说明 \n
 *   配置错误帧事件检测参数，包括检测阈值和检测窗口
 * - 命令格式 \n
 *   efm link-monitor frame threshold <0-4294967295> {window <1-60>} \n
 *   no efm link-monitor frame threshold {window}
 * - 参数说明 \n
 *   |参数             |说明                  |
 *   |----------------|----------------------|
 *   |threshold       |检测阈值,0-4294967295|
 *   |window          |检测窗口,1-60(seconds)        |
 * - 缺省情况 \n
 *   |参数             |说明                  |
 *   |----------------|----------------------|
 *   |threshold       |默认值为1             |
 *   |window          |默认值为1s             |
 * - 命令模式 \n
 *   interface模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     无
 *   - 前置条件 \n
 *     配置EFM使能
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     show efm status \n
 *     show efm status interface [trunk]gigabitethernet USP
 * - 使用举例 \n
 *     无
 * 
 */

DEFSH (VTYSH_L2,
	efm_err_frame_event_tw_cmd_vtysh,
	"efm link-monitor frame threshold <0-4294967295> {window <1-60>}",
	"efm node\n"
	"Link Monitor\n"
	"Errored Frame Event Properties\n"
	"Error Threshold\n"
	"Error frame threshold value, (0-4294967295)\n"
	"Event window\n"
	"Error frame window value, 1-60(sec)\n")
	
//EFM end//
/*-------------------------------efm---------------------------------*/
		
void
vtysh_init_l2if_efm_cmd ()
{	

         install_element_level (PHYSICAL_IF_NODE, &efm_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);


	install_element_level (PHYSICAL_IF_NODE, &efm_mode_active_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &efm_agent_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &efm_no_agent_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &efm_agent_hostname_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_agent_hostname_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (PHYSICAL_IF_NODE, &efm_agent_ip_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_agent_ip_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (PHYSICAL_IF_NODE, &efm_agent_id_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &efm_rem_loopback_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &efm_rem_loopback_time_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_rem_loopback_st_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_rem_loopback_ts_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &efm_remote_loopback_start_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_remote_loopback_start_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &efm_remote_loopback_mac_swap_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &efm_remote_loopback_no_mac_swap_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);


	/* efm Link Monitor related commands */
	install_element_level (PHYSICAL_IF_NODE, &efm_link_monitor_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_link_monitor_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &efm_link_monitor_on_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_link_monitor_on_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	/*efm event log commands*/
	install_element_level (PHYSICAL_IF_NODE, &efm_event_log_size_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_event_log_size_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &clear_efm_event_log_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	/*efm link monitor for err-frame and err-symbol*/
	install_element_level (PHYSICAL_IF_NODE, &efm_sym_period_event_tw_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &efm_sym_period_event_wt_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &no_efm_sym_period_event_tw_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_sym_period_event_wt_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (PHYSICAL_IF_NODE, &efm_err_frame_event_tw_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &efm_err_frame_event_wt_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (PHYSICAL_IF_NODE, &no_efm_err_frame_event_tw_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_err_frame_event_wt_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (PHYSICAL_IF_NODE, &efm_err_frame_period_event_tw_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
         install_element_level (PHYSICAL_IF_NODE, &efm_err_frame_period_event_wt_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (PHYSICAL_IF_NODE, &no_efm_err_frame_period_event_tw_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
         install_element_level (PHYSICAL_IF_NODE, &no_efm_err_frame_period_event_wt_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (PHYSICAL_IF_NODE, &efm_err_frame_second_event_tw_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &efm_err_frame_second_event_wt_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (PHYSICAL_IF_NODE, &no_efm_err_frame_second_event_tw_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_err_frame_second_event_wt_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &efm_link_event_notify_set_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_link_event_notify_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	/*efm unidirection link support commands*/
	install_element_level (PHYSICAL_IF_NODE, &efm_uni_dir_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_uni_dir_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	/* timers commands. */
	install_element_level (PHYSICAL_IF_NODE, &efm_link_timer_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE,& no_efm_link_timer_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &efm_pdu_timer_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_efm_pdu_timer_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &clear_efm_statsitics_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	/* display commands */
	install_element_level (PHYSICAL_IF_NODE, &show_efm_statsitics_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	//install_element_level (PHYSICAL_IF_NODE, &show_efm_ethernet_statsitics_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (PHYSICAL_IF_NODE, &show_efm_trunk_statsitics_cmd_vtysh,MONITOR_LEVE_2);
	install_element_level (PHYSICAL_IF_NODE, &show_efm_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	//install_element_level (PHYSICAL_IF_NODE, &show_efm_interface_ethernet_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (PHYSICAL_IF_NODE, &show_efm_interface_trunk_cmd_vtysh,MONITOR_LEVE_2);
	install_element_level (PHYSICAL_IF_NODE, &show_efm_discovery_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	//install_element_level (PHYSICAL_IF_NODE, &show_efm_ethernet_discovery_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (PHYSICAL_IF_NODE, &show_efm_trunk_discovery_cmd_vtysh,MONITOR_LEVE_2);
	install_element_level (PHYSICAL_IF_NODE, &show_efm_status_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	//install_element_level (PHYSICAL_IF_NODE, &show_efm_ethernet_status_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (PHYSICAL_IF_NODE, &show_efm_trunk_status_cmd_vtysh,MONITOR_LEVE_2);
	install_element_level (PHYSICAL_IF_NODE, &show_efm_event_log_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	//install_element_level (PHYSICAL_IF_NODE, &show_efm_ethernet_event_log_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (PHYSICAL_IF_NODE, &show_efm_trunk_event_log_cmd_vtysh,MONITOR_LEVE_2);

	install_element_level (CONFIG_NODE, &show_efm_statsitics_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_efm_ethernet_statsitics_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	//install_element_level (CONFIG_NODE, &show_efm_trunk_statsitics_cmd_vtysh,MONITOR_LEVE_2);
	install_element_level (CONFIG_NODE, &show_efm_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_efm_interface_ethernet_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	//install_element_level (CONFIG_NODE, &show_efm_interface_trunk_cmd_vtysh,MONITOR_LEVE_2);
	install_element_level (CONFIG_NODE, &show_efm_discovery_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_efm_ethernet_discovery_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	//install_element_level (CONFIG_NODE, &show_efm_trunk_discovery_cmd_vtysh,MONITOR_LEVE_2);
	install_element_level (CONFIG_NODE, &show_efm_status_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_efm_ethernet_status_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	//install_element_level (CONFIG_NODE, &show_efm_trunk_status_cmd_vtysh,MONITOR_LEVE_2);
	install_element_level (CONFIG_NODE, &show_efm_event_log_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_efm_ethernet_event_log_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	//install_element_level (CONFIG_NODE, &show_efm_trunk_event_log_cmd_vtysh,MONITOR_LEVE_2);

	#if 0
	/* efm	enable/disable commands */
	install_element_level (TRUNK_IF_NODE, &efm_enable_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_enable_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

	/* efm mode set commands */
	install_element_level (TRUNK_IF_NODE, &efm_mode_active_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	//install_element (TRUNK_IF_NODE, &efm_mode_passive_cmd);

	/* efm remote loopback	enable/disable commands */
	install_element_level (TRUNK_IF_NODE, &efm_rem_loopback_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &efm_rem_loopback_time_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_rem_loopback_st_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_rem_loopback_ts_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &efm_remote_loopback_start_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_remote_loopback_start_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &efm_remote_loopback_mac_swap_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &efm_remote_loopback_no_mac_swap_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);


	/* efm Link Monitor related commands */
	install_element_level (TRUNK_IF_NODE, &efm_link_monitor_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_link_monitor_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &efm_link_monitor_on_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_link_monitor_on_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

	/*efm event log commands*/
	install_element_level (TRUNK_IF_NODE, &efm_event_log_size_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_event_log_size_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &clear_efm_event_log_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

	/*efm link monitor for err-frame and err-symbol*/
	install_element_level (TRUNK_IF_NODE, &efm_sym_period_event_tw_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &efm_sym_period_event_wt_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (TRUNK_IF_NODE, &no_efm_sym_period_event_tw_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_sym_period_event_wt_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (TRUNK_IF_NODE, &efm_err_frame_event_tw_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &efm_err_frame_event_wt_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (TRUNK_IF_NODE, &no_efm_err_frame_event_tw_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_err_frame_event_wt_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (TRUNK_IF_NODE, &efm_err_frame_period_event_tw_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &efm_err_frame_period_event_wt_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (TRUNK_IF_NODE, &no_efm_err_frame_period_event_tw_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (TRUNK_IF_NODE, &no_efm_err_frame_period_event_wt_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
		
	install_element_level (TRUNK_IF_NODE, &efm_err_frame_second_event_tw_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &efm_err_frame_second_event_wt_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	
	install_element_level (TRUNK_IF_NODE, &no_efm_err_frame_second_event_tw_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_err_frame_second_event_wt_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (TRUNK_IF_NODE, &efm_link_event_notify_set_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_link_event_notify_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

	/*efm unidirection link support commands*/
	install_element_level (TRUNK_IF_NODE, &efm_uni_dir_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_uni_dir_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

	/* timers commands. */
	install_element_level (TRUNK_IF_NODE, &efm_link_timer_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_link_timer_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &efm_pdu_timer_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_efm_pdu_timer_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (TRUNK_IF_NODE, &clear_efm_statsitics_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	/* display commands */
	install_element_level (TRUNK_IF_NODE, &show_efm_statsitics_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (TRUNK_IF_NODE, &show_efm_ethernet_statsitics_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (TRUNK_IF_NODE, &show_efm_trunk_statsitics_cmd_vtysh,MONITOR_LEVE_2);
	install_element_level (TRUNK_IF_NODE, &show_efm_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (TRUNK_IF_NODE, &show_efm_interface_ethernet_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (TRUNK_IF_NODE, &show_efm_interface_trunk_cmd_vtysh,MONITOR_LEVE_2);
	install_element_level (TRUNK_IF_NODE, &show_efm_discovery_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (TRUNK_IF_NODE, &show_efm_ethernet_discovery_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (TRUNK_IF_NODE, &show_efm_trunk_discovery_cmd_vtysh,MONITOR_LEVE_2);
	install_element_level (TRUNK_IF_NODE, &show_efm_status_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (TRUNK_IF_NODE, &show_efm_ethernet_status_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (TRUNK_IF_NODE, &show_efm_trunk_status_cmd_vtysh,MONITOR_LEVE_2);
	install_element_level (TRUNK_IF_NODE, &show_efm_event_log_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (TRUNK_IF_NODE, &show_efm_ethernet_event_log_cmd_vtysh,MONITOR_LEVE_2);
	//install_element_level (TRUNK_IF_NODE, &show_efm_trunk_event_log_cmd_vtysh,MONITOR_LEVE_2);
	#endif
}
