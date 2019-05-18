/**
 * \page cmds_ref_arp ARP
 * - \subpage modify_log_arp
 * - \subpage arp_mode
 * - \subpage undo_configure_static_arp_cmd_vtysh
 * - \subpage configure_static_arp_cmd_vtysh
 * - \subpage configure_static_arp_gigabit_ethernet_cmd_vtysh
 * - \subpage configure_static_arp_xgigabit_ethernet_cmd_vtysh
 * - \subpage configure_vlanif_static_arp_cmd_vtysh
 * - \subpage configure_vlanif_static_arp_gigabit_ethernet_cmd_vtysh
 * - \subpage configure_vlanif_static_arp_xgigabit_ethernet_cmd_vtysh
 * - \subpage configure_trunk_static_arp_cmd_vtysh
 * - \subpage arp_interface_enable_cmd_vtysh
 * - \subpage arp_interface_disable_cmd_vtysh
 * - \subpage arp_proxy_cmd_vtysh
 * - \subpage no_arp_proxy_cmd_vtysh
 * - \subpage arp_num_limit_cmd_vtysh
 * - \subpage arp_age_time_cmd_vtysh
 * - \subpage show_arp_config_cmd_vtysh
 * - \subpage show_arp_static_cmd_vtysh
 * - \subpage show_all_static_arp_cmd_vtysh
 * - \subpage show_arp_ethernet_cmd_vtysh
 * - \subpage show_arp_gigabit_ethernet_cmd_vtysh
 * - \subpage show_arp_xgigabit_ethernet_cmd_vtysh
 * - \subpage show_arp_vlanif_cmd_vtysh
 * - \subpage show_arp_trunk_cmd_vtysh
 * - \subpage show_arp_cmd_vtysh
 * - \subpage show_arp_l3vpn_cmd_vtysh
 * - \subpage show_all_arp_cmd_vtysh
 * - \subpage show_arp_statistics_cmd_vtysh
 * - \subpage clear_arp_ethernet_cmd_vtysh
 * - \subpage clear_arp_gigabit_ethernet_cmd_vtysh
 * - \subpage clear_arp_xgigabit_ethernet_cmd_vtysh
 * - \subpage clear_arp_vlanif_cmd_vtysh
 * - \subpage clear_arp_trunk_cmd_vtysh
 * - \subpage clear_arp_ip_cmd_vtysh
 * - \subpage clear_arp_l3vpn_cmd_vtysh
 * - \subpage clear_arp_cmd_vtysh
 *
 */

/**
 * \page modify_log_arp Modify Log
 * \section arp-v007r004 HiOS-V007R004
 *  -#
 * \section arp-v007r003 HiOS-V007R003
 *  -#
 */
#include <zebra.h>
#include "command.h"
#include "ifm_common.h"
#include "vtysh.h"

/**
 * \page configure_static_arp_gigabit_ethernet_cmd_vtysh arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface gigabitethernet USP {l3vpn <1-1024>}
 * - 功能说明 \n
 *	 添加接口的静态arp表项
 * - 命令格式 \n
 *	 arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface gigabitethernet USP {l3vpn <1-1024>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|--------------|
 *	 |A.B.C.D |IP地址，IP是点分十进制形式。如：192.168.1.1|
 *	 |XX:XX:XX:XX:XX:XX|MAC地址|
 *	 |USP|以太网接口号。如1/1/1|
 *	 |<1-1024>|vpn instance ID。范围：1-1024，缺省值为 0|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无静态arp表项
 * - 命令模式 \n
 *	 命令行配置在arp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令添加以太接口的静态arp表项。
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   1、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n 
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *	 - 相关命令 \n
 *	   show arp static
 * - 使用举例 \n
 *	   配置一条ARP静态映射；\n
 *	   IP地址为192.168.1.1，对应的MAC地址为00:22:aa:00:22:aa，此arp静态映射属于VPN-ID 1，出接口为interface gigabitethernet 1/0/1 \n
 *		   Huahuan(config)# arp \n
 *		   Huahuan(config-arp)# arp  static ip 192.168.1.1 mac 00:22:aa:00:22:aa interface gigabitethernet 1/0/1 l3vpn 1 \n
 *
 */

/**
 * \page configure_static_arp_xgigabit_ethernet_cmd_vtysh arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface xgigabitethernet USP {l3vpn <1-1024>}
 * - 功能说明 \n
 *	 添加接口的静态arp表项
 * - 命令格式 \n
 *	 arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface xgigabitethernet USP {l3vpn <1-1024>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|--------------|
 *	 |A.B.C.D |IP地址，IP是点分十进制形式。如：192.168.1.1|
 *	 |XX:XX:XX:XX:XX:XX|MAC地址|
 *	 |USP|以太网接口号。如1/1/1|
 *	 |<1-1024>|vpn instance ID。范围：1-1024，缺省值为 0|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无静态arp表项
 * - 命令模式 \n
 *	 命令行配置在arp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令添加以太接口的静态arp表项。
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   1、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *	 - 相关命令 \n
 *	   show arp static
 * - 使用举例 \n
 *	   配置一条ARP静态映射；\n
 *	   IP地址为192.168.1.1，对应的MAC地址为00:22:aa:00:22:aa，此arp静态映射属于VPN-ID 1，出接口为interface xgigabitethernet 1/0/1 \n
 *		   Huahuan(config)# arp \n
 *		   Huahuan(config-arp)# arp  static ip 192.168.1.1 mac 00:22:aa:00:22:aa interface xgigabitethernet 1/0/1 l3vpn 1 \n
 *
 */

/**
 * \page configure_vlanif_static_arp_cmd_vtysh arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface vlanif <1-4094> port ethernet USP {l3vpn <1-1024>}
 * - 功能说明 \n
 *	 添加接口的静态arp表项
 * - 命令格式 \n
 *	 arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface vlanif <1-4094> port ethernet USP {l3vpn <1-1024>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|--------------|
 *	 |A.B.C.D |IP地址，IP是点分十进制形式。如：192.168.1.1|
 *	 |XX:XX:XX:XX:XX:XX|MAC地址|
 *	 |<1-4094>|vlanif接口号|
 *	 |<1-1024>|vpn instance ID。范围：1-1024，缺省值为 0|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无静态arp表项
 * - 命令模式 \n
 *	 命令行配置在arp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令添加vlanif接口的静态arp表项。
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   1、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */

/**
 * \page configure_vlanif_static_arp_gigabit_ethernet_cmd_vtysh arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface vlanif <1-4094> port gigabitethernet USP {l3vpn <1-1024>}
 * - 功能说明 \n
 *	 添加vlanif接口的静态arp表项
 * - 命令格式 \n
 *	 arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface vlanif <1-4094> port gigabitethernet USP {l3vpn <1-1024>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D |IP地址，IP是点分十进制形式。如：192.168.1.1|
 *	 |XX:XX:XX:XX:XX:XX|MAC地址|
 *	 |<1-4094>|Vlanif接口  |
 *	 |USP	|Vlanif的物理成员口|
 *	 |<1-1024>|vpn instance ID。范围：1-1024，缺省值为 0|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在arp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令添加vlanif接口的静态arp表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   1、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */

/**
 * \page configure_vlanif_static_arp_xgigabit_ethernet_cmd_vtysh arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface vlanif <1-4094> port xgigabitethernet USP {l3vpn <1-1024>}
 * - 功能说明 \n
 *	 添加vlanif接口的静态arp表项
 * - 命令格式 \n
 *	 arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface vlanif <1-4094> port xgigabitethernet USP {l3vpn <1-1024>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D |IP地址，IP是点分十进制形式。如：192.168.1.1|
 *	 |XX:XX:XX:XX:XX:XX|MAC地址|
 *	 |<1-4094>|Vlanif接口  |
 *	 |USP	|Vlanif的物理成员口|
 *	 |<1-1024>|vpn instance ID。范围：1-1024，缺省值为 0|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在arp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令添加vlanif接口的静态arp表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   1、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */

/**
 * \page configure_trunk_static_arp_cmd_vtysh arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface trunk TRUNK {l3vpn <1-1024>}
 * - 功能说明 \n
 *	 添加vlanif接口的静态arp表项
 * - 命令格式 \n
 *	 arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface trunk TRUNK {l3vpn <1-1024>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D |IP地址，IP是点分十进制形式。如：192.168.1.1|
 *	 |XX:XX:XX:XX:XX:XX|MAC地址|
 *	 |TRUNK|Trunk接口。如1	|
 *	 |<1-1024>|vpn instance ID。范围：1-1024，缺省值为 0|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在arp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令添加以太接口或trunk接口的静态arp表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   1、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */

/**
 * \page arp_interface_enable_cmd_vtysh arp enable
 * - 功能说明 \n
 *	 配置接口使能动态arp学习功能
 * - 命令格式 \n
 *	 arp enable
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，配置ip后自动使能动态arp学习功能
 * - 命令模式 \n
 *	 命令行配置在三层接口模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   该命令配置三层接口配置模式下动态arp学习的使能功能
 *	 - 前置条件 \n
 *	   已经配置过接口ip地址
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   arp disable
 * - 使用举例 \n
 *	   接口模式下使能arp：\n
 *	   Huahuan(config)# interface  gigabitethernet	1/1/1 \n
 *	   Huahuan(config-gigabitethernet1/1/1)#arp enable \n
 *
 */

static struct cmd_node arp_node =
{
  ARP_NODE,
  "%s(config-arp)# ",
};

/**
 * \page arp_mode arp
 * - 功能说明 \n
 *   进入arp视图
 * - 命令格式 \n
 *   arp
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
 *     使用该命令进入arp视图
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *     无
 *
 */
DEFUNSH (VTYSH_ROUTE,
	arp_mode,
	arp_mode_cmd,
	"arp",
	"Arp command\n")
{
	vty->node = ARP_NODE;
	return CMD_SUCCESS;
}

DEFUNSH (VTYSH_ROUTE,
	vtysh_exit_arp,
	vtysh_exit_arp_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}

/**
 * \page undo_configure_static_arp_cmd_vtysh no arp static ip A.B.C.D {l3vpn <1-1024>}
 * - 功能说明 \n
 *	 删除指定ip的静态arp表项或指定ip和l3vpn的静态arp表项
 * - 命令格式 \n
 *	 no arp static ip A.B.C.D { l3vpn  <1-1024>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|--------------|
 *	 |A.B.C.D |IP地址，IP是点分十进制形式。如：192.168.1.1|
 *	 |<1-1024>|vpn instance ID。范围：1-1024|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无静态arp表项
 * - 命令模式 \n
 *	 命令行配置在arp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除三层接口的arp静态表项
 *	 - 前置条件 \n
 *	   已经添加过三层接口的arp静态表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   1、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *	 - 相关命令 \n
 *	   show arp static
 * - 使用举例 \n
 *	   删除一条ARP静态映射; \n
 *	   指定IP地址为12.1.1.1 且VPN-ID为2。 \n
 *		   Huahuan(config-arp)#  \n
 *		   Huahuan(config-arp)# sho arp static \n
 *		   IP ADDRESS	 MAC ADDRESS		  INTERFACE 		   VPN \n
 *		   12.1.1.1 	 00:E0:4C:12:1A:71	  ethernet 1/1/1	   2 \n
 *		   STATIC NUM:1 \n
 *		   Huahuan(config-arp)# no arp static  ip 12.1.1.1 l3vpn	2 \n
 *		   Huahuan(config-arp)#  \n
 *		   Huahuan(config-arp)# sho arp static \n
 *		   IP ADDRESS	 MAC ADDRESS		  INTERFACE 		   VPN \n
 *		   STATIC NUM:0  \n
 *
 */
DEFSH (VTYSH_ROUTE, undo_configure_static_arp_cmd_vtysh,
 "no arp static ip A.B.C.D {l3vpn <1-1024>}",
 "Delete\n"
 "Arp\n"
 "Static arp\n"
 "IP address\n"
 "IP format A.B.C.D\n"
 "L3vpn\n"
 "L3vpn value <1-1024>\n"
 )

/**
 * \page configure_static_arp_cmd_vtysh arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface ethernet USP {l3vpn <1-1024>}
 *
 * - 功能说明 \n
 *	 添加接口的静态arp表项
 * - 命令格式 \n
 *	 arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface ethernet USP {l3vpn <1-1024>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|--------------|
 *	 |A.B.C.D |IP地址，IP是点分十进制形式。如：192.168.1.1|
 *	 |XX:XX:XX:XX:XX:XX|MAC地址|
 *	 |USP|以太网接口号。如1/1/1|
 *	 |<1-1024>|vpn instance ID。范围：1-1024，缺省值为 0|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无静态arp表项
 * - 命令模式 \n
 *	 命令行配置在arp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令添加以太接口的静态arp表项。
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   1、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n 
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *	 - 相关命令 \n
 *	   show arp static
 * - 使用举例 \n
 *	   配置一条ARP静态映射；\n
 *	   IP地址为192.168.1.1，对应的MAC地址为00:22:aa:00:22:aa，此arp静态映射属于VPN-ID 1，出接口为interface ethernet 1/0/1 \n
 *		   Huahuan(config)# \n
 *		   Huahuan(config-arp)# arp  static ip 192.168.1.1 mac 00:22:aa:00:22:aa interface ethernet 1/0/1 l3vpn 1 \n
 *
 */
DEFSH (VTYSH_ROUTE, configure_static_arp_cmd_vtysh,
  "arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface ethernet USP {l3vpn <1-1024>}",
  "Arp\n"
  "Static arp \n"
  "IP address\n"
  "IP format A.B.C.D\n"
  "Mac address\n"
  "Mac format XX:XX:XX:XX:XX:XX\n"
  "Interface\n"
  "Ethernet interface type\n"
  CLI_INTERFACE_ETHERNET_VHELP_STR
  "L3vpn \n"
  "L3vpn value <1-1024>\n")

DEFSH (VTYSH_ROUTE, configure_static_arp_gigabit_ethernet_cmd_vtysh,
	"arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface gigabitethernet USP {l3vpn <1-1024>}",
	"Arp\n"
	"Static arp \n"
	"IP address\n"
	"IP format A.B.C.D\n"
	"Mac address\n"
	"Mac format XX:XX:XX:XX:XX:XX\n"
	"Interface\n"
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	"L3vpn \n"
	"L3vpn value <1-1024>\n")

DEFSH (VTYSH_ROUTE, configure_static_arp_xgigabit_ethernet_cmd_vtysh,
	"arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface xgigabitethernet USP {l3vpn <1-1024>}",
	"Arp\n"
	"Static arp \n"
	"IP address\n"
	"IP format A.B.C.D\n"
	"Mac address\n"
	"Mac format XX:XX:XX:XX:XX:XX\n"
	"Interface\n"
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
	"L3vpn \n"
	"L3vpn value <1-1024>\n")

DEFSH(VTYSH_ROUTE,configure_vlanif_static_arp_cmd_vtysh,
	"arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface vlanif <1-4094> port ethernet USP {l3vpn <1-1024>}",
	"Arp\n"
	"Static arp \n"
	"IP address\n"
	"IP format A.B.C.D\n"
	"Mac address\n"
	"Mac format XX:XX:XX:XX:XX:XX\n"
	"Interface\n"
	"Vlanif interface\n"
	"Vlanif interface number\n"
    "The physical member port of vlanif\n"
    "Ethernet interface type\n"
	"The subport of the interface, format: <0-7>/<0-31>/<1-255>\n"
    "L3vpn \n"
	"L3vpn value <1-1024>\n")

DEFSH(VTYSH_ROUTE,configure_vlanif_static_arp_gigabit_ethernet_cmd_vtysh,
	"arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface vlanif <1-4094> port gigabitethernet USP {l3vpn <1-1024>}",
	"Arp\n"
	"Static arp \n"
	"IP address\n"
	"IP format A.B.C.D\n"
	"Mac address\n"
	"Mac format XX:XX:XX:XX:XX:XX\n"
	"Interface\n"
	"Vlanif interface\n"
	"Vlanif interface number\n"
	"The physical member port of vlanif\n"
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	"L3vpn \n"
	"L3vpn value <1-1024>\n")

DEFSH(VTYSH_ROUTE,configure_vlanif_static_arp_xgigabit_ethernet_cmd_vtysh,
	"arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface vlanif <1-4094> port xgigabitethernet USP {l3vpn <1-1024>}",
	"Arp\n"
	"Static arp \n"
	"IP address\n"
	"IP format A.B.C.D\n"
	"Mac address\n"
	"Mac format XX:XX:XX:XX:XX:XX\n"
	"Interface\n"
	"Vlanif interface\n"
	"Vlanif interface number\n"
	"The physical member port of vlanif\n"
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
	"L3vpn \n"
	"L3vpn value <1-1024>\n")

DEFSH(VTYSH_ROUTE,configure_trunk_static_arp_cmd_vtysh,
	"arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface trunk TRUNK {l3vpn <1-1024>}",
	"Arp\n"
	"Static arp \n"
	"IP address\n"
	"IP format A.B.C.D\n"
	"Mac address\n"
	"Mac format XX:XX:XX:XX:XX:XX\n"
	"Interface\n"
	"Trunk interface\n"
	"Trunk interface number <1-128>[.<1-4095>]\n"
    "L3vpn \n"
	"L3vpn value <1-1024>\n")

DEFSH (VTYSH_ROUTE, arp_interface_enable_cmd_vtysh,
		"arp enable",
		"Arp command\n"
	 "Enable dynamic arp learning\n")

/**
 * \page arp_interface_disable_cmd_vtysh arp disable
 * - 功能说明 \n
 *	 配置接口禁止动态arp学习功能
 * - 命令格式 \n
 *	 arp disable
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，配置ip后自动使能动态arp学习功能
 * - 命令模式 \n
 *	 命令行配置在三层接口模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   该命令禁止三层接口配置模式下动态arp学习的使能功能
 *	 - 前置条件 \n
 *	   已经配置过接口ip地址
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   arp enable
 * - 使用举例 \n
 *	   Huahuan(config)# interface  ethernet  1/1/1 \n
 *	   Huahuan(config-ethernet1/1/1)#arp disable \n
 *
 */
DEFSH (VTYSH_ROUTE, arp_interface_disable_cmd_vtysh,
	  "arp disable",
	  "Arp command\n"
	  "Disable dynamic arp learning\n")

/**
 * \page arp_proxy_cmd_vtysh arp proxy
 * - 功能说明 \n
 *	 接口使能 arp 代理
 * - 命令格式 \n
 *	 arp proxy
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，禁止arp代理功能
 * - 命令模式 \n
 *	 命令行配置在三层接口模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   该命令配置三层接口配置模式下使能arp代理功能
 *	 - 前置条件 \n
 *	   已经配置过接口ip地址
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   no arp proxy
 * - 使用举例 \n
 *	   接口模式使能arp代理：\n
 *	   Huahuan(config-ethernet1/1/1)# \n
 *	   Huahuan(config-ethernet1/1/1)#arp proxy \n
 *
 */
DEFSH (VTYSH_ROUTE, arp_proxy_cmd_vtysh,
        "arp proxy ",
        "Arp command\n"
        "Arp proxy\n")

/**
 * \page no_arp_proxy_cmd_vtysh no arp proxy
 * - 功能说明 \n
 *	 接口禁止 arp 代理
 * - 命令格式 \n
 *	 no arp proxy
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，禁止arp代理功能
 * - 命令模式 \n
 *	 命令行配置在三层接口模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   该命令配置三层接口配置模式下禁止arp代理功能
 *	 - 前置条件 \n
 *	   已经配置过接口ip地址且已经使能过接口arp代理
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   arp proxy
 * - 使用举例 \n
 *	   接口模式去使能arp代理：\n
 *	   Huahuan(config-ethernet1/1/1)# \n
 *	   Huahuan(config-ethernet1/1/1)#no arp proxy \n
 *
 */
DEFSH (VTYSH_ROUTE, no_arp_proxy_cmd_vtysh,
        "no arp proxy",
        "Negate a command or set its defaults\n"
        "Arp\n"
        "Arp proxy\n")

/**
 * \page arp_num_limit_cmd_vtysh arp num-limit <0-4096>
 * - 功能说明 \n
 *	 配置全局或接口下的动态arp数量限制
 * - 命令格式 \n
 *	 arp num_limit <0-4096>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|---------------------------|
 *	 |<0-4096>|配置动态 arp 的学习数量限制|
 *
 * - 缺省情况 \n
 *	 缺省情况下，限制默认值为4096
 * - 命令模式 \n
 *	 命令行配置在arp模式下或接口模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   该命令配置全局或接口下动态arp学习数量限制
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   arp proxy
 * - 使用举例 \n
 *	   Huahuan(config)# arp \n
 *	   Huahuan(config-arp)# arp num_limit  4095
 *
 */
DEFSH (VTYSH_ROUTE, arp_num_limit_cmd_vtysh,
        "arp num-limit <0-4096> ",
        "Arp \n"
        "Arp num-limit\n"
        "Arp num-limit number\n")

/**
 * \page arp_age_time_cmd_vtysh arp age-time <180-65535>
 * - 功能说明 \n
 *	 配置动态 arp 的老化时间
 * - 命令格式 \n
 *	 arp age-time <180-65535>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |-----------|------------------------------|
 *	 |<180-65535>|配置动态 arp 的老化时间,单位秒|
 *
 * - 缺省情况 \n
 *	 缺省情况下，动态 arp 的老化时间为30分钟，半连接arp表项老化时间3分钟
 * - 命令模式 \n
 *	 命令行配置在arp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   该命令配置动态arp老化时间的设置值。动态arp表项老化时间小于或等于老化时间设定值的1/3时进入老化探测，老化时间小于1min时删除该arp表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# arp \n
 *	   Huahuan(config-arp)# arp age-time 180
 *
 */
DEFSH (VTYSH_ROUTE,arp_age_time_cmd_vtysh,
        "arp age-time <180-65535>",
        "Arp \n"
        "Arp age-time\n"
        "Arp age-time number(in seconds)\n")

/**
 * \page show_arp_config_cmd_vtysh show arp config
 * - 功能说明 \n
 *	 显示全局或接口arp配置信息
 * - 命令格式 \n
 *	 show arp config
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 全局下显示全局arp默认配置信息及所有接口arp配置信息；接口模式下显示接口arp默认配置信息
 * - 命令模式 \n
 *	 命令行配置在arp模式下或接口模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示接口或全局arp配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   arp模式下显示配置：\n
 *			Huahuan(config-arp)# sho arp config \n
 *			Global arp config \n
 *			 arp age-time 30 \n
 *			 arp num-limit 1 \n
 *			 interface gigabitethernet 1/1/1 \n
 *			 arp enable \n
 *			 arp proxy disable \n
 *			 arp num-limit 4096 \n
 *
 */
DEFSH (VTYSH_ROUTE, show_arp_config_cmd_vtysh,
		 "show arp config",
		 "Show running system information\n"
		 "Arp information\n"
		 "Arp config information\n")

/**
 * \page show_arp_static_cmd_vtysh show arp static ip A.B.C.D {l3vpn <1-1024>}
 * - 功能说明 \n
 *	 显示静态arp表项
 * - 命令格式 \n
 *	 show arp static ip A.B.C.D {l3vpn <1-1024>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|-------------------------------------------|
 *	 |A.B.C.D |IP地址，IP是点分十进制形式。如：192.168.1.1|
 *	 |<1-1024>|vpn instance ID。范围：1-1024|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无静态arp表项
 * - 命令模式 \n
 *	 命令行配置在arp模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示指定三层以太网接口的arp静态表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   1、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *	 - 相关命令 \n
 *	   show arp static
 * - 使用举例 \n
 *	   显示一条ARP静态映射；\n
 *	   指定IP地址为192.168.1.1且VPN-ID为8 \n
 *		   Huahuan # arp \n
 *		   Huahuan (config-arp)# show arp static ip 192.168.1.1 l3vpn 8 \n
 *		   IP ADDRESS	 MAC ADDRESS		  INTERFACE 		 VPN	   STATUS \n
 *		   192.168.1.1	 11:22:33:44:55:66	  ethernet 1/1/1.1		 8	   STATIC \n
 *
 */
DEFSH(VTYSH_ROUTE,show_arp_static_cmd_vtysh,
	   "show arp static ip A.B.C.D {l3vpn <1-1024>}",
	   "Show running arp information\n"
	   "Arp\n"
	   "Static arp\n"
	   "IP address\n"
	   "IP format A.B.C.D\n"
	   "L3vpn\n"
	   "L3vpn value <1-1024>\n")

/**
 * \page show_all_static_arp_cmd_vtysh show arp static
 * - 功能说明 \n
 *	 显示所有静态arp表项
 * - 命令格式 \n
 *	 show arp static
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，无静态arp表项
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示所有的arp静态表项
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   arp static ip A.B.C.D mac XX:XX:XX:XX:XX:XX interface ethernet USP {l3vpn <1-1024>} \n
 *	   show arp static ip A.B.C.D {l3vpn <1-1024>} \n
 *	   show arp \n
 * - 使用举例 \n
 *	   显示所有ARP静态映射及条数；\n
 *	   Huahuan(config-arp)# \n
 *	   Huahuan(config-arp)# sho arp static \n
 *	   IP ADDRESS	 MAC ADDRESS		  INTERFACE 		   VPN	\n
 *	   12.1.1.1 	 00:E0:4C:12:1A:71	  ethernet 1/1/1	   2 \n
 *	   STATIC NUM:1
 *
 */
DEFSH(VTYSH_ROUTE,show_all_static_arp_cmd_vtysh,
	"show arp static",
	"Show running arp information\n"
	"Arp\n"
	"Static arp\n")

/**
 * \page show_arp_ethernet_cmd_vtysh show arp interface ethernet USP
 * - 功能说明 \n
 *	 显示指定接口下的arp表项,包括动态与静态
 * - 命令格式 \n
 *	 show arp interface ethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |----|---------------------|
 *	 |USP |以太网接口号。如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示指定接口下的arp表项,包括动态与静态的
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,show_arp_ethernet_cmd_vtysh,
	"show arp interface ethernet USP",
	SHOW_STR
	"Show arp info\n"
	"Interface\n"
	"Ethernet interface type\n"
	CLI_INTERFACE_ETHERNET_VHELP_STR)

/**
 * \page show_arp_gigabit_ethernet_cmd_vtysh show arp interface gigabitethernet USP
 * - 功能说明 \n
 *	 显示指定接口下的arp表项,包括动态与静态
 * - 命令格式 \n
 *	 show arp interface gigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |----|---------------------|
 *	 |USP |以太网接口号。如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示指定接口下的arp表项,包括动态与静态的
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,show_arp_gigabit_ethernet_cmd_vtysh,
	"show arp interface gigabitethernet USP",
	SHOW_STR
	"Show arp info\n"
	"Interface\n"
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)

/**
 * \page show_arp_xgigabit_ethernet_cmd_vtysh show arp interface xgigabitethernet USP
 * - 功能说明 \n
 *	 显示指定接口下的arp表项,包括动态与静态
 * - 命令格式 \n
 *	 show arp interface xgigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |----|---------------------|
 *	 |USP |以太网接口号。如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示指定接口下的arp表项,包括动态与静态的
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,show_arp_xgigabit_ethernet_cmd_vtysh,
	"show arp interface xgigabitethernet USP",
	SHOW_STR
	"Show arp info\n"
	"Interface\n"
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)

/**
 * \page show_arp_vlanif_cmd_vtysh show arp interface vlanif <1-4094>
 * - 功能说明 \n
 *	 显示指定接口下的arp表项,包括动态与静态
 * - 命令格式 \n
 *	 show arp interface vlanif <1-4094>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|-----------------------------------------|
 *	 |<1-4094>|vpn instance ID。范围：1-1024，缺省值为 0|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示指定接口下的arp表项,包括动态与静态的
 *	 - 前置条件 \n
 *	   存在arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   显示指定接口的arp表项: \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config)# show arp interface ethernet 1/1/3 \n
 *	   IP ADDRESS	 MAC ADDRESS		  INTERFACE 		   VPN		STATUS \n
 *	   192.168.1.2	 00:E0:4C:12:1A:71	  ethernet 1/1/3	   0		COMPLETE \n
 *
 */
DEFSH(VTYSH_FTM,show_arp_vlanif_cmd_vtysh,
	"show arp interface vlanif <1-4094>",
	SHOW_STR
	"Show arp info\n"
	"Interface\n"
	"Vlanif interface type\n"
	CLI_INTERFACE_VLANIF_VHELP_STR)

/**
 * \page show_arp_trunk_cmd_vtysh show arp interface trunk TRUNK
 * - 功能说明 \n
 *	 显示指定trunk接口下的arp表项,包括动态与静态
 * - 命令格式 \n
 *	 show arp interface trunk TRUNK
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |-----|--------------|
 *	 |TRUNK|Trunk接口。如1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示指定接口下的arp表项,包括动态与静态的
 *	 - 前置条件 \n
 *	   存在arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,show_arp_trunk_cmd_vtysh,
	"show arp interface trunk TRUNK",
	SHOW_STR
	"Show arp info\n"
	"Interface\n"
	"Trunk interface type\n"
	CLI_INTERFACE_TRUNK_VHELP_STR)

/**
 * \page show_arp_cmd_vtysh show arp ip A.B.C.D {l3vpn <1-1024>}
 * - 功能说明 \n
 *	 显示指定ip的arp表项或显示指定ip和l3vpn的arp表项
 * - 命令格式 \n
 *	 show arp ip A.B.C.D {l3vpn <1-1024>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|--------------|
 *	 |A.B.C.D |IP地址，IP是点分十进制形式。如：192.168.1.1|
 *   |<1-1024>|vpn instance ID。范围：1-1024|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示指定接口下的arp表项,包括动态与静态的
 *	 - 前置条件 \n
 *	   存在arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   1、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,show_arp_cmd_vtysh,
	"show arp ip A.B.C.D {l3vpn <1-1024>}",
	"Show arp info\n"
	"Arp\n"
	"IP address\n"
	"IP format A.B.C.D \n"
	"L3vpn\n"
	"L3vpn value <1-1024> \n"
	)

/**
 * \page show_arp_l3vpn_cmd_vtysh show arp l3vpn <1-1024>
 * - 功能说明 \n
 *	 显示指定l3vpn的arp表项
 * - 命令格式 \n
 *	 show arp l3vpn <1-1024>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|--------------|
 *	 |<1-1024>|vpn instance ID。范围：1-1024|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示指定l3vpn下的arp表项,包括动态与静态的
 *	 - 前置条件 \n
 *	   存在arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   1、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,show_arp_l3vpn_cmd_vtysh,
	"show arp l3vpn <1-1024>",
	"Show arp info\n"
	"Arp\n"
	"L3vpn\n"
	"L3vpn value <1-1024> \n"
	)

/**
 * \page show_all_arp_cmd_vtysh show arp
 * - 功能说明 \n
 *	 显示所有arp表项,包括动态与静态的，以及对应统计信息
 * - 命令格式 \n
 *	 show arp
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示所有arp表项,包括动态与静态的，以及对应条数
 *	 - 前置条件 \n
 *	   存在arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp  ip A.B.C.D
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,show_all_arp_cmd_vtysh,
	"show arp",
	"Show running arp information\n"
	"Arp\n")

/**
 * \page show_arp_statistics_cmd_vtysh show arp statistics
 * - 功能说明 \n
 *	 显示指定所有arp表项的统计信息
 * - 命令格式 \n
 *	 show arp statistics
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项，统计信息为空
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示统计信息，包括动态与静态的
 *	 - 前置条件 \n
 *	   存在arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,show_arp_statistics_cmd_vtysh,
	"show arp statistics",
	"Show running arp information\n"
	"Arp\n"
	"Arp statistics num\n")

/**
 * \page clear_arp_ethernet_cmd_vtysh clear arp interface ethernet USP
 * - 功能说明 \n
 *	 接口下执行命令时，清除ethernet接口下的所有动态arp
 * - 命令格式 \n
 *	 clear arp interface ethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |USP|以太网接口号。如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项，统计信息为空
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示统计信息，包括动态与静态的
 *	 - 前置条件 \n
 *	   存在arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_FTM,clear_arp_ethernet_cmd_vtysh,
        "clear arp interface ethernet USP",
		"Clear\n"
		"Arp\n"
		"Interface\n"
		"Ethernet interface type\n"
		CLI_INTERFACE_ETHERNET_VHELP_STR
		)
/**
 * \page clear_arp_gigabit_ethernet_cmd_vtysh clear arp interface gigabitethernet USP
 * - 功能说明 \n
 *	 接口下执行命令时，清除gigabitethernet接口下的所有动态arp
 * - 命令格式 \n
 *	 clear arp interface gigabitethernet USP
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |---|--------------|
 *   |USP|以太网接口号。如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项，统计信息为空
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示统计信息，包括动态与静态的
 *	 - 前置条件 \n
 *	   存在arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_FTM,clear_arp_gigabit_ethernet_cmd_vtysh,
		"clear arp interface gigabitethernet USP",
		"Clear\n"
		"Arp\n"
		"Interface\n"
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		)
/**
 * \page clear_arp_xgigabit_ethernet_cmd_vtysh clear arp interface xgigabitethernet USP
 * - 功能说明 \n
 *	 接口下执行命令时，清除xgigabitethernet接口下的所有动态arp
 * - 命令格式 \n
 *	 clear arp interface xgigabitethernet USP
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |---|--------------|
 *   |USP|以太网接口号。如1/1/1|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项，统计信息为空
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示统计信息，包括动态与静态的
 *	 - 前置条件 \n
 *	   存在arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */

DEFSH (VTYSH_FTM,clear_arp_xgigabit_ethernet_cmd_vtysh,
        "clear arp interface xgigabitethernet USP",
		"Clear\n"
		"Arp\n"
		"Interface\n"
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		)

/**
 * \page clear_arp_vlanif_cmd_vtysh clear arp interface vlanif <1-4094>
 * - 功能说明 \n
 *	 接口下执行命令时，清除vlanif接口下的所有动态arp
 * - 命令格式 \n
 *	 clear arp interface vlanif <1-4094>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|----------|
 *	 |<1-4094>|Vlanif接口|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项，统计信息为空
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示统计信息，包括动态与静态的
 *	 - 前置条件 \n
 *	   存在arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_FTM,clear_arp_vlanif_cmd_vtysh,
        "clear arp interface vlanif <1-4094>",
		"Clear\n"
		"Arp\n"
		"Interface\n"
		"Vlanif interface type\n"
		CLI_INTERFACE_VLANIF_VHELP_STR
		)

/**
 * \page clear_arp_trunk_cmd_vtysh clear arp interface trunk TRUNK
 * - 功能说明 \n
 *	 接口下执行命令时，清除trunk接口下的所有动态arp
 * - 命令格式 \n
 *	 clear arp interface trunk TRUNK
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|----------|
 *	 |TRUNK|Vlanif接口|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无arp表项，统计信息为空
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示统计信息，包括动态与静态的
 *	 - 前置条件 \n
 *	   存在arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_FTM,clear_arp_trunk_cmd_vtysh,
        "clear arp interface trunk TRUNK",
		"Clear\n"
		"Arp\n"
		"Interface\n"
		"Trunk interface type\n"
		CLI_INTERFACE_TRUNK_VHELP_STR
		)

/**
 * \page clear_arp_ip_cmd_vtysh clear arp ip A.B.C.D
 * - 功能说明 \n
 *	 清除指定ip的arp表项
 * - 命令格式 \n
 *	 clear arp ip A.B.C.D
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |-------|----------|
 *	 |A.B.C.D |vpn instance ID。范围：1-1024|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无动态arp表项
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可实现清除指定的arp表项
 *	 - 前置条件 \n
 *	   接口下或全局存在相关arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_FTM,clear_arp_ip_cmd_vtysh,
		"clear arp ip A.B.C.D",
		"Clear\n"
		"Arp\n"
		"IP address\n"
		"IP format A.B.C.D\n"
		)
/**
 * \page clear_arp_l3vpn_cmd_vtysh clear arp l3vpn <1-1024>
 * - 功能说明 \n
 *	 清除指定l3vpn的arp表项
 * - 命令格式 \n
 *	 clear arp l3vpn <1-1024>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|----------|
 *	 |<1-1024>|l3vpn取值范围<1-1024>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，无动态arp表项
 * - 命令模式 \n
 *	 命令行配置在全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可实现清除指定l3vpn的arp表项
 *	 - 前置条件 \n
 *	   接口下或全局存在相关arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   1、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_FTM,clear_arp_l3vpn_cmd_vtysh,
        "clear arp l3vpn <1-1024>",
		"Clear\n"
		"Arp\n"
		"L3vpn\n"
		"L3vpn value <1-1024>\n")

/**
 * \page clear_arp_cmd_vtysh clear arp
 * - 功能说明 \n
 *	 接口下执行命令时，清除以太接口下的所有动态 arp；全局执行时，清除所有接口的arp表项；注册在接口和全局模式下
 * - 命令格式 \n
 *	 clear arp
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，无动态arp表项
 * - 命令模式 \n
 *	 命令行配置接口模式和全局模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令可实现清除接口下或全局下的所有动态arp
 *	 - 前置条件 \n
 *	   存在相关arp表项
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show arp
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_FTM,clear_arp_cmd_vtysh,
		"clear arp ",
		"Clear\n"
		"Arp\n")


DEFSH (VTYSH_FTM,
        config_arp_miss_anti_sip_cmd_vtysh,
        "arp-miss speed-limit source-ip [A.B.C.D] maxinum <0-1500>",
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        "Arp-miss packet source ip\n"
        "Ip format\n"
        "The time allowed\n"
        "The range of arp-miss count\n"
        )
		
		
DEFSH (VTYSH_FTM,
        undo_config_arp_miss_anti_sip_cmd_vtysh,
        "no arp-miss speed-limit source-ip A.B.C.D",
        "Undo\n"
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        "Arp-miss packet source ip\n"
        "Ip format\n"
        )
		
DEFSH (VTYSH_FTM,
        config_arp_miss_anti_vlan_cmd_vtysh,
        "arp-miss speed-limit vlan <1-4095> maxinum <0-1500>",
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        "Vlan"
        "Vlan value\n"
        "The time allowed\n"
        "The range of arp-miss count\n"
        )

DEFSH (VTYSH_FTM,
        undo_config_arp_miss_anti_vlan_cmd_vtysh,
        "no arp-miss speed-limit vlan <1-4095>",
        "Undo\n"
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        "Vlan\n"
        "Vlan value\n"
        )


DEFSH (VTYSH_FTM,
        config_arp_miss_anti_eth_ifidx_cmd_vtysh,
        "arp-miss speed-limit interface ethernet USP maxinum <0-1500>",
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        INTERFACE_STR
    	CLI_INTERFACE_ETHERNET_STR
    	CLI_INTERFACE_ETHERNET_VHELP_STR
        "The time allowed\n"
        "The range of arp-miss count\n"
        )

DEFSH (VTYSH_FTM,
        config_arp_miss_anti_geth_ifidx_cmd_vtysh,
        "arp-miss speed-limit interface gigabitethernet USP maxinum <0-1500>",
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        INTERFACE_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        "The time allowed\n"
        "The range of arp-miss count\n"
        )
        
DEFSH (VTYSH_FTM,
        config_arp_miss_anti_xgeth_ifidx_cmd_vtysh,
        "arp-miss speed-limit interface xgigabitethernet USP maxinum <0-1500>",
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        INTERFACE_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        "The time allowed\n"
        "The range of arp-miss count\n"
        )


DEFSH  (VTYSH_FTM,
        undo_config_arp_miss_anti_eth_ifidx_cmd_vtysh,
        "no arp-miss speed-limit interface ethernet USP",
        "Undo\n"
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        INTERFACE_STR
    	CLI_INTERFACE_ETHERNET_STR
    	CLI_INTERFACE_ETHERNET_VHELP_STR
        )
		
DEFSH  (VTYSH_FTM,
        undo_config_arp_miss_anti_geth_ifidx_cmd_vtysh,
        "no arp-miss speed-limit interface gigabitethernet USP",
        "Undo\n"
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        INTERFACE_STR
    	CLI_INTERFACE_GIGABIT_ETHERNET_STR
    	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        )

DEFSH  (VTYSH_FTM,
        undo_config_arp_miss_anti_xgeth_ifidx_cmd_vtysh,
        "no arp-miss speed-limit interface xgigabitethernet USP",
        "Undo\n"
        "Arp miss action\n"
        "Arp-miss speed limit\n"
        INTERFACE_STR
    	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
    	CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        )


DEFSH (VTYSH_ROUTE,
      config_arp_fake_expire_time_cmd_vtysh,
      "arp-fake expire-time <1-30>",
      "Fake arp entry\n"
      "Arp entry expire time\n"
      "The range of expire time\n"
    )


DEFSH (VTYSH_ROUTE,
      config_arp_entry_fixed_all_cmd_vtysh,
      "arp anti-attack entry-check fixed-all",
      "Arp\n"
      "Arp attack anti\n"
      "Arp entry check\n"
      "Arp entry fixed all\n"
)


DEFSH (VTYSH_ROUTE,
      config_arp_entry_fixed_mac_cmd_vtysh,
      "arp anti-attack entry-check fixed-mac",
      "Arp\n"
      "Arp attack anti\n"
      "Arp entry check\n"
      "Arp entry fixed mac\n"
)

DEFSH (VTYSH_ROUTE,
    config_arp_entry_fixed_sendack_cmd_vtysh,
    "arp anti-attack entry-check send-ack",
    "Arp\n"
    "Arp attack anti\n"
    "Arp entry check\n"
    "Arp entry send ack\n"        
)

DEFSH (VTYSH_ROUTE,
	  no_config_arp_entry_fixed_cmd_vtysh,
	  "no arp anti-attack entry-check",
	  "No\n"
	  "Arp\n"
	  "Arp attack anti\n"
	  "Arp entry check\n"
)

DEFSH(VTYSH_HAL,
      debug_hal_send_pkt_cmd_vtysh,
      "debug hal send pkt",
      "Debug hal packet info\n"
      "Hal module\n"
      "Send direct\n"
      "Packet\n" )


/*for h3c command*/
DEFSH(VTYSH_ROUTE,
	  configure_h3c_short_static_arp_cmd_vtysh,
	  "arp static X.X.X.X H-H-H {vpn-instance <1-128>}",
	  "Address Resolution Protocol (ARP) module\n"
	  "Static ARP entry\n"
	  "Specify the IP address\n"
	  "MAC address\n"
	  "Specify the Vpn-instance\n"
	  "STRING<1-128>\n")


DEFSH(VTYSH_ROUTE,
	configure_h3c_long_static_arp_cmd_vtysh,
	"arp static X.X.X.X H-H-H <1-4094> gigabitethernet USP {vpn-instance <1-128>}",
	"Address Resolution Protocol (ARP) module\n"
	"Static ARP entry\n"
	"Specify the IP address\n"
	"MAC address\n"
    "INTEGER<1-4094>\n"
    "GigabitEthernet interface\n"
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	"Specify the Vpn-instance\n"
	"STRING<1-128>\n")

DEFSH(VTYSH_ROUTE,
    h3c_undo_arp_cmd_vtysh,
    "undo arp X.X.X.X {<1-128>}",
    "Cancel current setting\n"
    "Address Resolution Protocol (ARP) module\n"
    "Specify the IP address\n"
    "Vpn-instance name\n"
    )


DEFSH(VTYSH_ROUTE,
      h3c_reset_arp_cmd_vtysh,
      "reset arp ( all | dynamic | multiport | static )",
      "Reset operation\n"
      "Address Resolution Protocol (ARP) module\n"
      "Reset all ARP entry\n"
      "Reset dynamic ARP entry\n"
      "Specify the interface\n"
      "Clear multiport ARP entries\n"
      "Reset static ARP entry\n" 
    )

DEFSH(VTYSH_ROUTE,
      h3c_reset_intf_cmd_vtysh,
      "reset arp interface gigabitethernet USP",
      "Reset operation\n"
      "Address Resolution Protocol (ARP) module\n"
      "Specify the interface\n"
	  CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
    )

DEFSH(VTYSH_ROUTE,
      h3c_reset_slot_cmd_vtysh,
      "reset arp slot <1-8>",
      "Reset operation\n"
      "Address Resolution Protocol (ARP) module\n"
      "Specify the slot number\n"
      "Slot number\n"
    )


DEFSH(VTYSH_ROUTE,
      h3c_gdynamic_arp_maxnum_cmd_vtysh,
      "arp max-learning-num <0-4096> slot <1-8>",
      "Address Resolution Protocol (ARP) module\n"
      "Set the maximum number of dynamic ARP entries that can be learned\n"
      "Value for the maximum number\n"
      "Specify the slot number\n"
      "Slot number\n"
     )


DEFSH(VTYSH_ROUTE,
      h3c_intf_dynamic_arp_maxnum_cmd_vtysh,
      "arp max-learning-num <0-4096>",
      "Address Resolution Protocol (ARP) module\n"
      "Set the maximum number of dynamic ARP entries that can be learned\n"
      "Value for the maximum number\n"
      "Specify the slot number\n"
      "Slot number\n"
     )

DEFSH(VTYSH_ROUTE,
      h3c_arp_timer_age_cmd_vtysh,
      "arp timer aging <1-1440>",
      "Address Resolution Protocol (ARP) module\n"
      "Specify ARP timer\n"
      "Specify ARP aging timer in minutes\n"
      "The value of timer(minutes)\n"
     )

DEFSH (VTYSH_FTM,
	 config_dynamic_arp_to_static_cmd_vtysh,
	 "arp to-static {ip A.B.C.D} {l3vpn <1-1024>}",
	 "Arp\n"
	 "Dynamic arp to static\n"
	 "IP address\n"
	 "IP format A.B.C.D\n"
	 "L3vpn\n"
	 "L3vpn value <1-1024>\n")

DEFSH (VTYSH_FTM,
	no_config_dynamic_arp_to_static_cmd_vtysh,
	"no arp to-static {ip A.B.C.D} {l3vpn <1-1024>}",
	NO_STR
	"Arp\n"
	"Dynamic arp to static\n"
	"IP address\n"
	"IP format A.B.C.D\n"
	"L3vpn\n"
	"L3vpn value <1-1024>\n")

DEFSH (VTYSH_FTM,
	show_dynamic_arp_to_static_cmd_vtysh,
	"show arp to-static status",
	SHOW_STR
	"Arp\n"
	"Dynamic arp to static\n"
	"status info\n")

DEFSH (VTYSH_ROUTE,
       arp_conf_debug_cmd_vtysh,
       "debug arp (enable|disable) (all|arp|other)",
       "Debug config\n"
       "Arp config\n"
       "Arp debug enable\n"
       "Arp debug disable\n"
       "Arp debug type all\n"
       "Arp debug type arp\n"
       "Arp debug type other\n")


#if 0
/* debug ÈÕÖ¾·¢ËÍµ½ syslog Ê¹ÄÜ×´Ì¬ÉèÖÃ */
DEFSH (VTYSH_ROUTE,arpd_log_level_ctl_cmd_vtysh,	"debug arpd(enable | disable)",
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n")

/* debug ÈÕÖ¾·¢ËÍµ½ syslog Ê¹ÄÜ×´Ì¬ÏÔÊ¾ */
DEFSH (VTYSH_ROUTE,arpd_show_log_level_ctl_cmd_vtysh,	"show debug arpd",
	SHOW_STR"Output log of debug level\n""Program name\n")
#endif


void
vtysh_init_arp_cmd ()
{
  	install_node (&arp_node, NULL);
	vtysh_install_default (ARP_NODE);

	install_element_level (CONFIG_NODE, &arp_mode_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (ARP_NODE, &vtysh_exit_arp_cmd, VISIT_LEVE, CMD_LOCAL);

	install_element_level (CONFIG_NODE, &show_arp_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_arp_gigabit_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_arp_xgigabit_ethernet_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_arp_vlanif_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_arp_trunk_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (CONFIG_NODE, &show_arp_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_arp_l3vpn_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_all_arp_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_arp_statistics_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (CONFIG_NODE, &clear_arp_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &clear_arp_gigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &clear_arp_xgigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &clear_arp_vlanif_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &clear_arp_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &clear_arp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &clear_arp_ip_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &clear_arp_l3vpn_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &config_dynamic_arp_to_static_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &no_config_dynamic_arp_to_static_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &show_dynamic_arp_to_static_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (ARP_NODE, &configure_static_arp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ARP_NODE, &configure_static_arp_gigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ARP_NODE, &configure_static_arp_xgigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (ARP_NODE, &configure_vlanif_static_arp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ARP_NODE, &configure_vlanif_static_arp_gigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ARP_NODE, &configure_vlanif_static_arp_xgigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (ARP_NODE, &configure_trunk_static_arp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ARP_NODE, &undo_configure_static_arp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ARP_NODE, &arp_num_limit_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ARP_NODE, &arp_age_time_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (ARP_NODE, &show_all_static_arp_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (ARP_NODE, &show_arp_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (ARP_NODE, &show_arp_static_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);


	install_element_level (PHYSICAL_IF_NODE, &arp_interface_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &arp_interface_disable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &arp_num_limit_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &arp_proxy_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_arp_proxy_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &clear_arp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &show_arp_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (PHYSICAL_SUBIF_NODE, &arp_interface_disable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &arp_interface_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &arp_num_limit_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &arp_proxy_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &no_arp_proxy_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &clear_arp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &show_arp_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (TRUNK_IF_NODE, &arp_interface_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &arp_interface_disable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &arp_num_limit_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &arp_proxy_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_arp_proxy_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &clear_arp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &show_arp_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (TRUNK_SUBIF_NODE, &arp_interface_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &arp_interface_disable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &arp_num_limit_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &arp_proxy_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &no_arp_proxy_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &show_arp_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (VLANIF_NODE, &arp_interface_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &arp_interface_disable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &arp_num_limit_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &arp_proxy_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_arp_proxy_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &clear_arp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &show_arp_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	//install_element_level (CONFIG_NODE, &arpd_log_level_ctl_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	//install_element_level (CONFIG_NODE, &arpd_show_log_level_ctl_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &config_arp_miss_anti_sip_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &undo_config_arp_miss_anti_sip_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &config_arp_miss_anti_vlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &undo_config_arp_miss_anti_vlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &config_arp_miss_anti_eth_ifidx_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &config_arp_miss_anti_geth_ifidx_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &config_arp_miss_anti_xgeth_ifidx_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &undo_config_arp_miss_anti_eth_ifidx_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &undo_config_arp_miss_anti_geth_ifidx_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &undo_config_arp_miss_anti_xgeth_ifidx_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (ARP_NODE, &config_arp_fake_expire_time_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (ARP_NODE, &config_arp_entry_fixed_all_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (ARP_NODE, &config_arp_entry_fixed_mac_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (ARP_NODE, &config_arp_entry_fixed_sendack_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ARP_NODE, &no_config_arp_entry_fixed_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &debug_hal_send_pkt_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &arp_conf_debug_cmd_vtysh,CONFIG_LEVE_5, CMD_LOCAL);
#if 0
    /*for h3c cmd*/
    install_element_level (CONFIG_NODE,&configure_h3c_short_static_arp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE,&configure_h3c_long_static_arp_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE,&h3c_undo_arp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE,&h3c_reset_arp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE,&h3c_reset_intf_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE,&h3c_reset_slot_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE,&h3c_gdynamic_arp_maxnum_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE,&h3c_intf_dynamic_arp_maxnum_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE,&h3c_arp_timer_age_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
#endif



}



