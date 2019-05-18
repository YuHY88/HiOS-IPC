/**
 * \page cmds_ref_ndp NDP
 * - \subpage modify_log_ndp
 * - \subpage ndp_mode
 * - \subpage configure_ethernet_static_ndp_cmd_vtysh
 * - \subpage configure_gigabit_ethernet_static_ndp_cmd_vtysh
 * - \subpage configure_xgigabit_ethernet_static_ndp_cmd_vtysh
 * - \subpage configure_trunk_static_ndp_cmd_vtysh
 * - \subpage undo_configure_static_ndp_cmd_vtysh
 * - \subpage ndp_interface_enable_cmd_vtysh
 * - \subpage ndp_proxy_cmd_vtysh
 * - \subpage no_ndp_proxy_cmd_vtysh
 * - \subpage ndp_num_limit_cmd_vtysh
 * - \subpage ndp_reach_time_cmd_vtysh
 * - \subpage ndp_stale_time_cmd_vtysh
 * - \subpage show_ethernet_ndp_config_cmd_vtysh
 * - \subpage show_vlanif_ndp_config_cmd_vtysh
 * - \subpage show_xgigabit_ethernet_ndp_config_cmd_vtysh
 * - \subpage show_gigabit_ethernet_ndp_config_cmd_vtysh
 * - \subpage show_trunk_ndp_config_cmd_vtysh
 * - \subpage show_ipv6_neighbor_static_cmd_vtysh
 * - \subpage show_ipv6_ndp_cmd_vtysh
 * - \subpage show_ipv6_ndp_ip_cmd_vtysh
 * - \subpage show_ipv6_ndp_interface_ethernet_cmd_vtysh
 * - \subpage show_ipv6_ndp_interface_trunk_cmd_vtysh
 * - \subpage show_ipv6_ndp_interface_vlanif_cmd_vtysh
 * - \subpage show_ipv6_ndp_interface_gigabit_ethernet_cmd_vtysh
 * - \subpage show_ipv6_ndp_interface_xgigabit_ethernet_cmd_vtysh
 * - \subpage clear_ipv6_ndp_interface_ethernet_cmd_vtysh
 * - \subpage clear_ipv6_ndp_interface_trunk_cmd_vtysh
 * - \subpage clear_ipv6_ndp_interface_vlanif_cmd_vtysh
 * - \subpage clear_ipv6_ndp_interface_gigabit_ethernet_cmd_vtysh
 * - \subpage clear_ipv6_ndp_interface_xgigabit_ethernet_cmd_vtysh
 * - \subpage clear_ipv6_ndp_all_cmd_vtysh
 *
 */

/**
 * \page modify_log_ndp Modify Log
 * \section ndp-v007r004 HiOS-V007R004
 *  -#
 * \section ndp-v007r003 HiOS-V007R003
 *  -#
 */
#include <lib/command.h>
#include <lib/ifm_common.h>
#include "vtysh.h"

// * - \subpage show_vlanif_ndp_config_cmd_vtysh
// * - \subpage show_xgigabit_ethernet_ndp_config_cmd_vtysh
static struct cmd_node ndp_node =
{
	NDP_NODE,
	"%s(config-ndp)# ",
	1,
};

/**
 * \page ndp_mode ndp
 * - 功能说明 \n
 *   进入ndp视图
 * - 命令格式 \n
 *   ndp
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     使用该命令进入ndp视图，配置全局nd信息或者添加静态/删除nd表项
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *     Hios-v7.3(config)# ndp \n
 *     Hios-v7.3(config-ndp)# \n
 *
 */
DEFUNSH(VTYSH_ROUTE,
             ndp_mode,
        	 ndp_mode_cmd,
   	         "ndp",
   	         "Ndp command\n"
	        )
{
	vty->node = NDP_NODE;
	return CMD_SUCCESS;
}


DEFUNSH (VTYSH_ROUTE,
	         vtysh_exit_ndp,
	         vtysh_exit_ndp_cmd,
	         "exit",
	         "Exit current mode and down to previous mode\n"
	        )
{
	return vtysh_exit(vty);
}

/**
 * \page configure_ethernet_static_ndp_cmd_vtysh ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface ethernet USP [l3vpn <1-1024>]
 * - 功能说明 \n
 *	 在ndp视图添加物理接口的静态nd表项
 * - 命令格式 \n
 *	 ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface ethernet USP [l3vpn <1-1024>]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |X.X.X.X.X.X.X.X |Ipv6地址，如1000：：1（支持缩写）|
 *	 |XX:XX:XX:XX:XX:XX|Mac地址，如00:00:00:01:02:03|
 *	 |USP|接口名称，如1/1/1  |
 *	 |<1-1024>|Vpnid，l3vpn实例id，范围1—1024，不配置则默认为0|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无静态ndp表项
 * - 命令模式 \n
 *	 ndp模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令添加三层接口的ndp静态表项
 *	 - 前置条件 \n
 *	   已创建三层接口数据结构
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show ipv6 neighbor static \n
 *	   show ipv6 neighbor
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_ROUTE,
      configure_ethernet_static_ndp_cmd_vtysh,
      "ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface ethernet USP [l3vpn <1-1024>]",
      "Ipv6\n"
      "Ipv6 neighbor\n"
      "Ipv6 address format X.X.X.X.X.X.X.X\n"
      "Mac address\n"
      "Mac format XX:XX:XX:XX:XX:XX\n"
      INTERFACE_STR
      CLI_INTERFACE_ETHERNET_STR
      CLI_INTERFACE_ETHERNET_VHELP_STR
      "L3vpn\n"
      "L3vpn value <1-1024>\n")

/**
 * \page configure_gigabit_ethernet_static_ndp_cmd_vtysh ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface gigabitethernet USP [l3vpn <1-1024>]
 * - 功能说明 \n
 *	 在ndp视图添加物理接口的静态nd表项
 * - 命令格式 \n
 *	 ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface gigabitethernet USP [l3vpn <1-1024>]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |X.X.X.X.X.X.X.X |Ipv6地址，如1000：：1（支持缩写）|
 *	 |XX:XX:XX:XX:XX:XX|Mac地址，如00:00:00:01:02:03|
 *	 |USP|接口名称，如1/1/1  |
 *	 |<1-1024>|Vpnid，l3vpn实例id，范围1—1024，不配置则默认为0|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无静态ndp表项
 * - 命令模式 \n
 *	 ndp模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令添加三层接口的ndp静态表项
 *	 - 前置条件 \n
 *	   已创建三层接口数据结构
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show ipv6 neighbor static \n
 *	   show ipv6 neighbor
 * - 使用举例 \n
 *	   Hios-v7.3(config)# ndp \n
 *	   Hios-v7.3(config-ndp)ipv6 neighbor 1000::8 mac 00:00:00:00:01:02 interface gigabitethernet 1/1/5 \n
 *
 */
DEFSH(VTYSH_ROUTE,
      configure_gigabit_ethernet_static_ndp_cmd_vtysh,
      "ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface gigabitethernet USP [l3vpn <1-1024>]",
      "Ipv6\n"
      "Ipv6 neighbor\n"
      "Ipv6 address format X.X.X.X.X.X.X.X\n"
      "Mac address\n"
      "Mac format XX:XX:XX:XX:XX:XX\n"
      INTERFACE_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
      "L3vpn\n"
      "L3vpn value <1-1024>\n")

/**
 * \page configure_xgigabit_ethernet_static_ndp_cmd_vtysh ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface xgigabitethernet USP [l3vpn <1-1024>]
 * - 功能说明 \n
 *	 在ndp视图添加物理接口的静态nd表项
 * - 命令格式 \n
 *	 ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface xgigabitethernet USP [l3vpn <1-1024>]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |X.X.X.X.X.X.X.X |Ipv6地址，如1000：：1（支持缩写）|
 *	 |XX:XX:XX:XX:XX:XX|Mac地址，如00:00:00:01:02:03|
 *	 |USP|接口名称，如1/1/1  |
 *	 |<1-1024>|Vpnid，l3vpn实例id，范围1—1024，不配置则默认为0|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无静态ndp表项
 * - 命令模式 \n
 *	 ndp模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令添加三层接口的ndp静态表项
 *	 - 前置条件 \n
 *	   已创建三层接口数据结构
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show ipv6 neighbor static \n
 *	   show ipv6 neighbor
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_ROUTE,
      configure_xgigabit_ethernet_static_ndp_cmd_vtysh,
      "ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface xgigabitethernet USP [l3vpn <1-1024>]",
      "Ipv6\n"
      "Ipv6 neighbor\n"
      "Ipv6 address format X.X.X.X.X.X.X.X\n"
      "Mac address\n"
      "Mac format XX:XX:XX:XX:XX:XX\n"
      INTERFACE_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
      "L3vpn\n"
      "L3vpn value <1-1024>\n")

/**
* \page configure_trunk_static_ndp_cmd_vtysh ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface trunk TRUNK [l3vpn <1-1024>]
* - 功能说明 \n
*	配置trunk接口的静态ndp表项
* - 命令格式 \n
*	ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface trunk TRUNK [l3vpn <1-1024>]
* - 参数说明 \n
*	|参数  |说明		  |
*	|------|--------------|
*	|X.X.X.X.X.X.X.X |Ipv6地址，如1000：：1（支持缩写）|
*	|XX:XX:XX:XX:XX:XX|Mac地址，如00:00:00:01:02:03|
*	|TRUNK|Trunk接口名称，范围<1-128>[.<1-4095>]，如1.1|
*	|<1-1024>|Vpnid，l3vpn实例id，范围1—1024，不配置则默认为0|
*
* - 缺省情况 \n
*	缺省情况下，无静态ndp表项
* - 命令模式 \n
*	ndp模式
* - 用户等级 \n
*	11
* - 使用指南 \n
*	- 应用场景 \n
*	  该命令配置trunk接口的静态ndp表项
*	- 前置条件 \n
*	  Trunk接口三层数据结构已创建
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	  show ipv6 neighbor interface trunk 1
* - 使用举例 \n
*	  无
*
*/
DEFSH(VTYSH_ROUTE,
      configure_trunk_static_ndp_cmd_vtysh,
      "ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface trunk TRUNK [l3vpn <1-1024>]",
      "Ipv6\n"
      "Ipv6 neighbor\n"
      "Ipv6 address format X.X.X.X.X.X.X.X\n"
      "Mac address\n"
      "Mac format XX:XX:XX:XX:XX:XX\n"
      INTERFACE_STR
      CLI_INTERFACE_TRUNK_STR
      CLI_INTERFACE_TRUNK_VHELP_STR
      "L3vpn\n"
      "L3vpn value <1-1024>\n")

/**
 * \page undo_configure_static_ndp_cmd_vtysh no ipv6 neighbor X.X.X.X.X.X.X.X [l3vpn <1-1024>]
 * - 功能说明 \n
 *	 指定ip或指定ip和vpn删除配置的静态ndp表项
 * - 命令格式 \n
 *	 no ipv6 neighbor X.X.X.X.X.X.X.X [l3vpn <1-1024>]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |X.X.X.X.X.X.X.X |Ipv6地址，如1000：：1（支持缩写）|
 *	 |<1-1024>|Vpnid，l3vpn实例id，范围1—1024，不配置则默认为0|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无静态ndp表项
 * - 命令模式 \n
 *	 ndp模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   该命令用于删除已配置的静态ndp表项
 *	 - 前置条件 \n
 *	   已经配置过静态ndp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ipv6 neighbor X.X.X.X.X.X.X.X mac XX:XX:XX:XX:XX:XX interface {Ethernet|gigabitethernet|xgigabitethernet} USP [l3vpn <1-1024>]
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_ROUTE,
    	undo_configure_static_ndp_cmd_vtysh,
    	"no ipv6 neighbor X.X.X.X.X.X.X.X [l3vpn <1-1024>]",
    	"Delete\n"
    	"Ipv6\n"
    	"Ipv6 neighbor\n"
    	"Ipv6 address format X.X.X.X.X.X.X.X\n"
    	"L3vpn\n"
    	"L3vpn value <1-1024>\n"
	    )

/**
* \page ndp_interface_enable_cmd_vtysh ipv6 ndp (enable|disable)
* - 功能说明 \n
*	注册在接口模式下，用于使能或去使能接口动态ndp学习功能，默认情况下使能
* - 命令格式 \n
*	ipv6 ndp (enable|disable)
* - 参数说明 \n
*	无
* - 缺省情况 \n
*	缺省情况下，接口配置ip以后，动态ndp学习处于使能状态
* - 命令模式 \n
*	命令行配置在注册在三层接口模式下
* - 用户等级 \n
*	11
* - 使用指南 \n
*	- 应用场景 \n
*	  该命令用于在接口配置ip后，使能/去使能ndp表项学习功能
*	- 前置条件 \n
*	  接口配置ipv6地址
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	  Ipv6 ndp disable
* - 使用举例 \n
*	  Hios-v7.3(config-gigabitethernet1/1/5)#ipv6 ndp enable
*
*/
DEFSH(VTYSH_ROUTE,
		ndp_interface_enable_cmd_vtysh,
        "ipv6 ndp (enable | disable)",
        "Ipv6\n"
        "ndp command\n"
	    "Enable dynamic ndp learning\n"
	    "Disable dynamic ndp learning\n"
       )

/**
 * \page ndp_proxy_cmd_vtysh ipv6 ndp proxy
 * - 功能说明 \n
 *	 接口使能 ndp 代理，注册在接口模式下
 * - 命令格式 \n
 *	 ipv6 ndp proxy
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，禁止ndp代理功能
 * - 命令模式 \n
 *	 命令行安装在三层接口模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   该命令在接口配置ipv6地址后，用于设置ndp代理功能
 *	 - 前置条件 \n
 *	   已经配置过接口ipv6地址
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   no ipv6 ndp proxy
 * - 使用举例 \n
 *	   Hios-v7.3(config-gigabitethernet1/1/5)#ipv6 ndp	proxy
 *
 */
DEFSH (VTYSH_ROUTE,
       ndp_proxy_cmd_vtysh,
       "ipv6 ndp proxy ",
       "Ipv6\n"
       "Ndp command\n"
       "Ndp proxy\n"
      )

/**
 * \page no_ndp_proxy_cmd_vtysh no ipv6 ndp proxy
 * - 功能说明 \n
 *	接口去使能 ndp 代理，注册在接口模式下
 * - 命令格式 \n
 *	no ipv6 ndp proxy
 * - 参数说明 \n
 *	无
 * - 缺省情况 \n
 *	缺省情况下，禁止ndp代理功能
 * - 命令模式 \n
 *	命令行配置接口模式下
 * - 用户等级 \n
 *	11
 * - 使用指南 \n
 *	- 应用场景 \n
 *	  该命令行安装在接口模式下，用于去使能ndp代理功能
 *	- 前置条件 \n
 *	  无
 *	- 后续任务 \n
 *	  无
 *	- 注意事项 \n
 *	  无
 *	- 相关命令 \n
 *	  无
 * - 使用举例 \n
 *	  Hios-v7.3(config-gigabitethernet1/1/5)#ipv6 ndp  proxy \n
 *	  Hios-v7.3(config-gigabitethernet1/1/5)#no ipv6 ndp proxy
 *
 */
DEFSH (VTYSH_ROUTE,
       no_ndp_proxy_cmd_vtysh,
       "no ipv6 ndp proxy ",
       "Cancel current setting\n"
       "Ipv6\n"
       "Ndp\n"
       "Ndp proxy"
	  )

/**
 * \page ndp_num_limit_cmd_vtysh ipv6 neighbor num-limit <0-1024>
 * - 功能说明 \n
 *	 设置全局或接口ndp表项数量限制
 * - 命令格式 \n
 *	 ipv6 neighbor num-limit <0-1024>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |---|--------------|
 *   |0-1024|限制ndp表项数量范围|
 *
 * - 缺省情况 \n
 *	 缺省情况下，ndp表项限制值为最大规格1024
 * - 命令模式 \n
 *	 命令行安装在接口模式和ndp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   Ndp模式下使用该命令时无限制，接口模式下需接口先创建三层接口数据结构
 *	 - 前置条件 \n
 *	   接口三层数据结构已创建
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Hios-v7.3(config-ndp)# ipv6 neighbor num-limit 1024 \n
 *     Hios-v7.3(config-ndp)# ipv6 neighbor num-limit 1023
 *
 */
DEFSH (VTYSH_ROUTE,
       ndp_num_limit_cmd_vtysh,
       "ipv6 neighbor num-limit <0-1024>",
       "Ipv6 \n"
       "Ipv6 neighbor\n"
       "Ndp num-limit number\n"
      )



/*配置全局nd 表项可达时间*/
/**
 * \page ndp_reach_time_cmd_vtysh ipv6 neighbor reachable-time <60-65535>
 * - 功能说明 \n
 *	 配置邻居表项可达状态时间
 * - 命令格式 \n
 *	 ipv6 neighbor reachable-time <60-65535>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |60--65535|邻居表项可达时间范围，单位秒|
 *
 * - 缺省情况 \n
 *	 缺省情况下，可达时间为1800秒
 * - 命令模式 \n
 *	 命令行安装在ndp视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置全局动态邻居表项的可达时间
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Hios-v7.3(config-ndp)# ipv6 neighbor reachable-time 180
 *
 */
DEFSH (VTYSH_ROUTE,
          ndp_reach_time_cmd_vtysh,
          "ipv6 neighbor reachable-time <60-65535>",
          "Ipv6\n"
          "Ipv6 neighbor\n"
          "Ndp reachable-time number(in seconds)\n")


/*配置nd 表项无效状态等待时间*/
/**
 * \page ndp_stale_time_cmd_vtysh ipv6 neighbor stale-time <60-65535>
 * - 功能说明 \n
 *	 配置邻居表项切换到stale状态后的等待时间
 * - 命令格式 \n
 *	 ipv6 neighbor stale-time <60-65535>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |60--65535|邻居表项stale状态可达时间范围，单位秒|
 *
 * - 缺省情况 \n
 *	 缺省情况下，为180秒
 * - 命令模式 \n
 *	 命令行安装在ndp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置邻居表项stale状态等待时间
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Hios-v7.3(config-ndp)# ipv6 neighbor stale-time 180
 *
 */
DEFSH (VTYSH_ROUTE,
          ndp_stale_time_cmd_vtysh,
          "ipv6 neighbor stale-time <60-65535>",
          "Ipv6\n"
          "Ipv6 neighbor\n"
          "Ndp stale-timeout number(in seconds)\n")

DEFSH(VTYSH_ROUTE,
         ndp_age_time_cmd_vtysh,
         "ipv6 neighbor age-time <3-65535>",
         "Ipv6\n"
         "Ipv6 neighbor\n"
         "Ndp age-time number(in seconds)\n"
        )

 /*显示ethernet 接口或全局ndp 配置*/
/**
 * \page show_ethernet_ndp_config_cmd_vtysh show ipv6 ndp config {interface ethernet USP}
 * - 功能说明 \n
 *	 Config视图下显示指定接口配置的ndp控制信息
 * - 命令格式 \n
 *	 show ipv6 ndp config {interface ethernet USP}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |USP|接口名称，如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口配置完ipv6地址以后，ndp动态学习功能使能，代理功能关闭，表项数量限制为1024
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可显示全部或者指定接口的ndp控制信息
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE,
        show_ethernet_ndp_config_cmd_vtysh,
        "show ipv6 ndp config {interface ethernet USP}",
        SHOW_STR
        IPV6_STR
        "Ipv6 neighbor\n"
        "Ipv6 neighbor config\n"
        INTERFACE_STR
        "Ethernet interface type\n"
    	"The port/subport of the interface, format: <0-7>/<0-31>/<1-255>.[<1-4095>]\n"
		)

/**
 * \page show_gigabit_ethernet_ndp_config_cmd_vtysh show ipv6 ndp config {interface gigabitethernet USP}
 * - 功能说明 \n
 *	 Config视图下显示指定接口配置的ndp控制信息
 * - 命令格式 \n
 *	 show ipv6 ndp config {interface gigabitethernet USP}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |USP|接口名称，如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口配置完ipv6地址以后，ndp动态学习功能使能，代理功能关闭，表项数量限制为1024
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可显示全部或者指定接口的ndp控制信息
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE,
		show_gigabit_ethernet_ndp_config_cmd_vtysh,
		"show ipv6 ndp config {interface gigabitethernet USP}",
		SHOW_STR
		IPV6_STR
		"Ipv6 neighbor\n"
		"Ipv6 neighbor config\n"
		INTERFACE_STR
		"GigabitEthernet interface type\n"
		"The port/subport of the interface, format: <0-7>/<0-31>/<1-255>.[<1-4095>]\n"
		)

/**
 * \page show_xgigabit_ethernet_ndp_config_cmd_vtysh show ipv6 ndp config {interface xgigabitethernet USP}
 * - 功能说明 \n
 *	 Config视图下显示指定接口配置的ndp控制信息
 * - 命令格式 \n
 *	 show ipv6 ndp config {interface xgigabitethernet USP}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |USP|接口名称，如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口配置完ipv6地址以后，ndp动态学习功能使能，代理功能关闭，表项数量限制为1024
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可显示全部或者指定接口的ndp控制信息
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE,
		show_xgigabit_ethernet_ndp_config_cmd_vtysh,
		"show ipv6 ndp config {interface xgigabitethernet USP}",
		SHOW_STR
		IPV6_STR
		"Ipv6 neighbor\n"
		"Ipv6 neighbor config\n"
		INTERFACE_STR
		"10GigabitEthernet interface type\n"
		"The port/subport of the interface, format: <0-7>/<0-31>/<1-255>.[<1-4095>]\n"
		)

/*显示vlanif 接口ndp 配置*/
/**
 * \page show_vlanif_ndp_config_cmd_vtysh show ipv6 ndp config interface vlanif <1-4095>
 * - 功能说明 \n
 *	 Config视图下显示指定vlanif接口的ndp控制信息
 * - 命令格式 \n
 *	 show ipv6 ndp config interface vlanif <1-4095>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |<1-4095>|vlanif接口|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口配置完ipv6地址以后，ndp动态学习功能使能，代理功能关闭，表项数量限制为1024
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可显示指定vlanif接口的ndp控制信息
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE,
        show_vlanif_ndp_config_cmd_vtysh,
        "show ipv6 ndp config interface vlanif <1-4095>",
        SHOW_STR
        IPV6_STR
        "Ipv6 neighbor\n"
        "Ipv6 neighbor config\n"
        INTERFACE_STR
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
		)

/*显示trunk 接口ndp 配置*/
/**
 * \page show_trunk_ndp_config_cmd_vtysh show ipv6 ndp config interface trunk TRUNK
 * - 功能说明 \n
 *	 Config视图下显示指定trunk接口的ndp控制信息
 * - 命令格式 \n
 *	 show ipv6 ndp config interface trunk TRUNK
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |TRUNK|Trunk接口名称，如1.1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口配置完ipv6地址以后，ndp动态学习功能使能，代理功能关闭，表项数量限制为1024
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可显示指定trunk接口的ndp控制信息
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE,
        show_trunk_ndp_config_cmd_vtysh,
        "show ipv6 ndp config interface trunk TRUNK",
        SHOW_STR
        IPV6_STR
        "Ipv6 neighbor\n"
        "Ipv6 neighbor config\n"
        INTERFACE_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        )


/*显示静态nd 表项*/
/**
 * \page show_ipv6_neighbor_static_cmd_vtysh show ipv6 neighbor static
 * - 功能说明 \n
 *	 显示静态邻居表项，以及数量统计信息
 * - 命令格式 \n
 *	 show ipv6 neighbor static
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，无静态邻居表项
 * - 命令模式 \n
 *	 命令行安装在ndp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示配置的静态邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show ipv6 neighbor
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_ROUTE,
         show_ipv6_neighbor_static_cmd_vtysh,
         "show ipv6 neighbor static",
         SHOW_STR
         IPV6_STR
         "Ipv6 neighbor\n"
         "static ipv6 neighbor\n"
         )

//ftm 命令行

/*显示所有活跃ndp 表项*/
/**
 * \page show_ipv6_ndp_cmd_vtysh show ipv6 neighbor
 * - 功能说明 \n
 *	 Config视图下显示动态和静态邻居表项
 * - 命令格式 \n
 *	 show ipv6 neighbor
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，无动态和静态邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可显示所有邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
      show_ipv6_ndp_cmd_vtysh,
      "show ipv6 neighbor",
      SHOW_STR
      IPV6_STR
      "Ipv6 neighbor\n")


/*显示指定的ndp表项*/
/**
 * \page show_ipv6_ndp_ip_cmd_vtysh show ipv6 neighbor X.X.X.X.X.X.X.X {l3vpn <1-1024>}
 * - 功能说明 \n
 *	 Config视图下显示指定ip或指定ip和vpn的邻居表项
 * - 命令格式 \n
 *	 show ipv6 neighbor X.X.X.X.X.X.X.X {l3vpn <1-1024>}
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |---|--------------|
 *   |X.X.X.X.X.X.X.X |Ipv6地址，如1000：：2|
 *   |<1-1024>|Vpn实例id|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无动态和静态邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可显示指定邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
      show_ipv6_ndp_ip_cmd_vtysh,
      "show ipv6 neighbor X.X.X.X.X.X.X.X {l3vpn <1-1024>}",
      "Show arp info\n"
      "Arp\n"
      "IP address\n"
      "IP format A.B.C.D \n"
      "L3vpn \n"
      "L3vpn value <1-1024> \n"
	)


/*显示指定接口的nd 表项*/
/**
 * \page show_ipv6_ndp_interface_ethernet_cmd_vtysh show ipv6 neighbor interface ethernet USP
 * - 功能说明 \n
 *	 Config视图下显示指定接口的邻居表项
 * - 命令格式 \n
 *	 show ipv6 neighbor interface ethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |USP|接口名称，如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无动态和静态邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可显示指定接口的邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
      show_ipv6_ndp_interface_ethernet_cmd_vtysh,
      "show ipv6 neighbor interface ethernet USP",
      SHOW_STR
      IPV6_STR
      "Ipv6 neighbor\n"
      INTERFACE_STR
      CLI_INTERFACE_ETHERNET_STR
      CLI_INTERFACE_ETHERNET_VHELP_STR
    )

/**
 * \page show_ipv6_ndp_interface_trunk_cmd_vtysh show ipv6 neighbor interface trunk TRUNK
 * - 功能说明 \n
 *	 Config视图下显示指定trunk接口的邻居表项
 * - 命令格式 \n
 *	 show ipv6 neighbor interface trunk TRUNK
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |TRUNK|Trunk接口名称，如1.1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无trunk接口邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可显示指定trunk接口邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
      show_ipv6_ndp_interface_trunk_cmd_vtysh,
      "show ipv6 neighbor interface trunk TRUNK",
      SHOW_STR
      IPV6_STR
      "Ipv6 neighbor\n"
      INTERFACE_STR
      CLI_INTERFACE_TRUNK_STR
      CLI_INTERFACE_TRUNK_VHELP_STR
    )

/**
 * \page show_ipv6_ndp_interface_vlanif_cmd_vtysh show ipv6 neighbor interface vlanif <1-4095>
 * - 功能说明 \n
 *	 Config视图下显示指定vlanif接口的邻居表项
 * - 命令格式 \n
 *	 show ipv6 neighbor interface vlanif <1-4095>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |<1-4095>|vlanif接口号|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无vlanif接口邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可显示指定vlanif接口邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
      show_ipv6_ndp_interface_vlanif_cmd_vtysh,
      "show ipv6 neighbor interface vlanif <1-4095>",
      SHOW_STR
      IPV6_STR
      "Ipv6 neighbor\n"
      INTERFACE_STR
      CLI_INTERFACE_VLANIF_STR
      CLI_INTERFACE_VLANIF_VHELP_STR
    )

/**
 * \page show_ipv6_ndp_interface_gigabit_ethernet_cmd_vtysh show ipv6 neighbor interface gigabitethernet USP
 * - 功能说明 \n
 *	 Config视图下显示指定接口的邻居表项
 * - 命令格式 \n
 *	 show ipv6 neighbor interface gigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |USP|接口名称，如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无动态和静态邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可显示指定接口的邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
	  show_ipv6_ndp_interface_gigabit_ethernet_cmd_vtysh,
	  "show ipv6 neighbor interface gigabitethernet USP",
	  SHOW_STR
	  IPV6_STR
	  "Ipv6 neighbor\n"
	  INTERFACE_STR
	  CLI_INTERFACE_GIGABIT_ETHERNET_STR
	  CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	)

/**
 * \page show_ipv6_ndp_interface_xgigabit_ethernet_cmd_vtysh show ipv6 neighbor interface xgigabitethernet USP
 * - 功能说明 \n
 *	 Config视图下显示指定接口的邻居表项
 * - 命令格式 \n
 *	 show ipv6 neighbor interface xgigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |USP|接口名称，如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无动态和静态邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可显示指定接口的邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
      show_ipv6_ndp_interface_xgigabit_ethernet_cmd_vtysh,
      "show ipv6 neighbor interface xgigabitethernet USP",
      SHOW_STR
      IPV6_STR
      "Ipv6 neighbor\n"
      INTERFACE_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	  CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
    )

/**
 * \page clear_ipv6_ndp_interface_ethernet_cmd_vtysh clear ipv6 neighbor interface ethernet USP
 * - 功能说明 \n
 *	 老化指定接口的动态邻居表项
 * - 命令格式 \n
 *	 clear ipv6 neighbor interface ethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |USP|接口名称，如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无动态邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可老化指定接口的动态邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
     clear_ipv6_ndp_interface_ethernet_cmd_vtysh,
     "clear ipv6 neighbor interface ethernet USP",
     CLEAR_STR
     IPV6_STR
     "Ipv6 neighbor\n"
     INTERFACE_STR
     CLI_INTERFACE_ETHERNET_STR
     CLI_INTERFACE_ETHERNET_VHELP_STR
     )

/**
 * \page clear_ipv6_ndp_interface_trunk_cmd_vtysh clear ipv6 neighbor interface trunk TRUNK
 * - 功能说明 \n
 *	 Config视图下老化指定trunk接口的邻居表项
 * - 命令格式 \n
 *	 clear ipv6 neighbor interface trunk TRUNK
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |TRUNK|Trunk接口名称，如1.1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无动态邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可老化指定trunk接口的邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
     clear_ipv6_ndp_interface_trunk_cmd_vtysh,
     "clear ipv6 neighbor interface trunk TRUNK",
     CLEAR_STR
     IPV6_STR
     "Ipv6 neighbor\n"
     INTERFACE_STR
     CLI_INTERFACE_TRUNK_STR
     CLI_INTERFACE_TRUNK_VHELP_STR
     )

/**
 * \page clear_ipv6_ndp_interface_vlanif_cmd_vtysh clear ipv6 neighbor interface vlanif <1-4095>
 * - 功能说明 \n
 *	 Config视图下老化指定vlanif接口的邻居表项
 * - 命令格式 \n
 *	 clear ipv6 neighbor interface vlanif <1-4095>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |<1-4095>|vlanif接口|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无动态邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可老化指定vlanif接口的邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
     clear_ipv6_ndp_interface_vlanif_cmd_vtysh,
     "clear ipv6 neighbor interface vlanif <1-4095>",
     CLEAR_STR
     IPV6_STR
     "Ipv6 neighbor\n"
     INTERFACE_STR
     CLI_INTERFACE_VLANIF_STR
     CLI_INTERFACE_VLANIF_VHELP_STR
     )

/**
 * \page clear_ipv6_ndp_interface_gigabit_ethernet_cmd_vtysh clear ipv6 neighbor interface gigabitethernet USP
 * - 功能说明 \n
 *	 老化指定接口的动态邻居表项
 * - 命令格式 \n
 *	 clear ipv6 neighbor interface gigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |USP|接口名称，如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无动态邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可老化指定接口的动态邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
	 clear_ipv6_ndp_interface_gigabit_ethernet_cmd_vtysh,
	 "clear ipv6 neighbor interface gigabitethernet USP",
	 CLEAR_STR
	 IPV6_STR
	 "Ipv6 neighbor\n"
	 INTERFACE_STR
	 CLI_INTERFACE_GIGABIT_ETHERNET_STR
	 CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	 )

/**
 * \page clear_ipv6_ndp_interface_xgigabit_ethernet_cmd_vtysh clear ipv6 neighbor interface xgigabitethernet USP
 * - 功能说明 \n
 *	 老化指定接口的动态邻居表项
 * - 命令格式 \n
 *	 clear ipv6 neighbor interface xgigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |USP|接口名称，如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无动态邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可老化指定接口的动态邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
     clear_ipv6_ndp_interface_xgigabit_ethernet_cmd_vtysh,
     "clear ipv6 neighbor interface xgigabitethernet USP",
     CLEAR_STR
     IPV6_STR
     "Ipv6 neighbor\n"
     INTERFACE_STR
     CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	 CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
     )

/**
 * \page clear_ipv6_ndp_all_cmd_vtysh clear ipv6 neighbor [l3vpn <1-1024>]
 * - 功能说明 \n
 *	 Config视图下老化所有或者指定vpn下的邻居表项
 * - 命令格式 \n
 *	 clear ipv6 neighbor [l3vpn <1-1024>]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |<1-1024>|Vpn实例id|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无动态邻居表项
 * - 命令模式 \n
 *	 命令行安装在config视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可老化全部或者指定vpn的邻居表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,
     clear_ipv6_ndp_all_cmd_vtysh,
     "clear ipv6 neighbor [l3vpn <1-1024>]",
     CLEAR_STR
     IPV6_STR
     "Ipv6 neighbor\n"
     "L3vpn \n"
	 "L3vpn value <1-1024> \n"
     )

DEFSH (VTYSH_ROUTE,
       ndp_conf_debug_cmd_vtysh,
       "debug ndp (enable|disable) (all|ndp|other)",
       "Debug config\n"
       "Ndp config\n"
       "Ndp debug enable\n"
       "Ndp debug disable\n"
       "Ndp debug type all\n"
       "Ndp debug type arp\n"
       "Ndp debug type other\n")

void vtysh_init_ndp_cmd()
{
    install_node(&ndp_node, NULL);
    vtysh_install_default(NDP_NODE);

    install_element_level(CONFIG_NODE, &ndp_mode_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level(CONFIG_NODE, &show_ethernet_ndp_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_gigabit_ethernet_ndp_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_xgigabit_ethernet_ndp_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_trunk_ndp_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_vlanif_ndp_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

    install_element_level(CONFIG_NODE, &show_ipv6_ndp_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ipv6_ndp_ip_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ipv6_ndp_interface_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ipv6_ndp_interface_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ipv6_ndp_interface_vlanif_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ipv6_ndp_interface_gigabit_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ipv6_ndp_interface_xgigabit_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &clear_ipv6_ndp_interface_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &clear_ipv6_ndp_interface_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &clear_ipv6_ndp_interface_vlanif_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &clear_ipv6_ndp_interface_gigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &clear_ipv6_ndp_interface_xgigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &clear_ipv6_ndp_all_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


    install_element_level(NDP_NODE, &show_ipv6_neighbor_static_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(NDP_NODE, &vtysh_exit_ndp_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level(NDP_NODE, &ndp_num_limit_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(NDP_NODE, &ndp_reach_time_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(NDP_NODE, &ndp_stale_time_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(NDP_NODE, &configure_ethernet_static_ndp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(NDP_NODE, &configure_gigabit_ethernet_static_ndp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(NDP_NODE, &configure_xgigabit_ethernet_static_ndp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(NDP_NODE, &configure_trunk_static_ndp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(NDP_NODE, &undo_configure_static_ndp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(NDP_NODE, &ndp_conf_debug_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);

    #define NDP_INSTALL_ELEMENT_L3IF(cmd)\
        install_element_level(PHYSICAL_IF_NODE, cmd, CONFIG_LEVE_5, CMD_SYNC);\
        install_element_level(PHYSICAL_SUBIF_NODE, cmd, CONFIG_LEVE_5, CMD_SYNC);\
        install_element_level(TRUNK_IF_NODE, cmd, CONFIG_LEVE_5, CMD_SYNC);\
        install_element_level(TRUNK_SUBIF_NODE, cmd, CONFIG_LEVE_5, CMD_SYNC);\
        install_element_level(VLANIF_NODE, cmd, CONFIG_LEVE_5, CMD_SYNC);

    NDP_INSTALL_ELEMENT_L3IF(&ndp_interface_enable_cmd_vtysh);
    NDP_INSTALL_ELEMENT_L3IF(&ndp_num_limit_cmd_vtysh);
    NDP_INSTALL_ELEMENT_L3IF(&ndp_proxy_cmd_vtysh);
    NDP_INSTALL_ELEMENT_L3IF(&no_ndp_proxy_cmd_vtysh);
}







