/**
 * \page cmds_ref_route Route
 * - \subpage modify_log_route
 * - \subpage route_node
 * - \subpage route_router_id_cmd_vtysh
 * - \subpage ip_route_static_nexthop_cmd_vtysh
 * - \subpage ip_route_static_ethernet_cmd_vtysh
 * - \subpage ip_route_static_gigabit_ethernet_cmd_vtysh
 * - \subpage ip_route_static_xgigabit_ethernet_cmd_vtysh
 * - \subpage ip_route_static_tunnel_cmd_vtysh
 * - \subpage ip_route_static_loopback_cmd_vtysh
 * - \subpage ip_route_static_vlanif_cmd_vtysh
 * - \subpage ip_route_static_trunk_cmd_vtysh
 * - \subpage ip_route_static_balckhole_cmd_vtysh
 * - \subpage no_ip_route_static_nexthop_cmd_vtysh
 * - \subpage no_ip_route_static_ethernet_cmd_vtysh
 * - \subpage no_ip_route_static_gigabit_ethernet_cmd_vtysh
 * - \subpage no_ip_route_static_xgigabit_ethernet_cmd_vtysh
 * - \subpage no_ip_route_static_tunnel_cmd_vtysh
 * - \subpage no_ip_route_static_loopback_cmd_vtysh
 * - \subpage no_ip_route_static_vlanif_cmd_vtysh
 * - \subpage no_ip_route_static_trunk_cmd_vtysh
 * - \subpage no_ip_route_static_blackhole_cmd_vtysh
 * - \subpage show_ip_route_single_cmd_vtysh
 * - \subpage show_ip_route_active_cmd_vtysh
 * - \subpage ip_urpf_cmd_vtysh
 * - \subpage no_ip_urpf_cmd_vtysh
 * - \subpage ip_l3vpn_cmd_vtysh
 * - \subpage no_ip_l3vpn_cmd_vtysh
 * - \subpage ip_addr_cmd_vtysh
 * - \subpage ip_addr_unnumbered_ethernet_cmd_vtysh
 * - \subpage ip_addr_unnumbered_gigabit_ethernet_cmd_vtysh
 * - \subpage ip_addr_unnumbered_xgigabit_ethernet_cmd_vtysh
 * - \subpage ip_addr_unnumbered_tunnel_cmd_vtysh
 * - \subpage ip_addr_unnumbered_loopback_cmd_vtysh
 * - \subpage no_ip_addr_cmd_vtysh
 * - \subpage no_ip_addr_dhcp_cmd_vtysh
 * - \subpage ip_address_dhcp_save_cmd_vtysh
 * - \subpage no_ip_addr_dhcp_save_cmd_vtysh
 * - \subpage show_ip_addr_cmd_vtysh
 * - \subpage show_ip_interface_cmd_vtysh
 * - \subpage show_ip_interface_ethernet_cmd_vtysh
 * - \subpage show_ip_interface_gigabit_ethernet_cmd_vtysh
 * - \subpage show_ip_interface_xgigabit_ethernet_cmd_vtysh
 * - \subpage show_ip_interface_tunnel_cmd_vtysh
 * - \subpage show_ip_interface_loopback_cmd_vtysh
 * - \subpage show_ip_interface_vlanif_cmd_vtysh
 * - \subpage show_ip_interface_trunk_cmd_vtysh
 * - \subpage show_ip_route_cmd_vtysh
 * - \subpage ip_address_dhcp_alloc_cmd_vtysh
 * - \subpage show_ip_route_statistics_cmd_vtysh
 *
 */

/**
 * \page modify_log_route Modify Log
 * \section route-v007r004 HiOS-V007R004
 *  -#
 * \section route-v007r003 HiOS-V007R003
 *  -#
 */

#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "ifm_common.h"

static struct cmd_node route_node1 =
{
  ROUTE_NODE,
  "%s(config-route)# ",
};

/**
 * \page route_node route
 * - 功能说明 \n
 *   Config视图切换至route视图
 * - 命令格式 \n
 *   route
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
 *     配置静态路由，首先需要进入route视图
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *     Huahuan(config)# route \n
 *     Huahuan(config-route)# \n
 *
 */
DEFUNSH (VTYSH_ROUTE,
        route_node,
        route_node_cmd,
        "route",
        "Enter the route node\n")
{
    vty->node = ROUTE_NODE;

    return CMD_SUCCESS;
}


DEFUNSH (VTYSH_ROUTE,
        vtysh_exit_route,
        vtysh_exit_route_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}


ALIAS (vtysh_exit_route,
        vtysh_quit_route_cmd,
        "quit",
        "Exit current mode and down to previous mode\n")

/**
 * \page route_router_id_cmd_vtysh router-id (A.B.C.D|X:X:X:X:X:X:X:X)
 * - 功能说明 \n
 *	 配置router-id
 * - 命令格式 \n
 *	 router-id (A.B.C.D|X:X:X:X:X:X:X:X)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|--------------|
 *	 |A.B.C.D |Ipv4:Router-id标识，点分十进制格式|
 *	 |X:X:X:X:X:X:X:X|Ipv6:Router-id标识，点分十进制格式|
 *
 * - 缺省情况 \n
 *	 缺省未配置router-id
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图下，使用该命令配置router-id
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *     Huahuan(config-route)# router-id 1.1.1.1 \n
 *
 */
DEFSH (VTYSH_ROUTE, route_router_id_cmd_vtysh,
        "router-id (A.B.C.D|X:X:X:X:X:X:X:X)",
        "Router id\n"
        "Router-id format A.B.C.D for IPv4\n"
        "Router-id format X:X:X:X:X:X:X:X for IPv6\n")

/**
 * \page ip_route_static_nexthop_cmd_vtysh ip route A.B.C.D/M nexthop A.B.C.D {distance <1-255> | vpn <1-128>}
 * - 功能说明 \n
 *	 使用该命令配置静态路由
 * - 命令格式 \n
 *	 ip route A.B.C.D/M nexthop A.B.C.D {distance <1-255> | vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *   |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *   |A.B.C.D |下一跳地址，点分十进制格式|
 *	 |distance <1-255>|Distance指定路由优先级，可选，默认为1|
 *	 |vpn <1-1024>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图下，使用该命令配置静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   出接口为可选切最多能选一个，下一跳是否选择根据出接口相关
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# ip route 3.3.3.0/24 nexthop 2.2.2.2 distance 20 \n
 *
 */
DEFSH (VTYSH_ROUTE, ip_route_static_nexthop_cmd_vtysh,
        "ip route A.B.C.D/M nexthop A.B.C.D {distance <1-255> | vpn <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        "Next hop\n"
        "IP address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page ip_route_static_ethernet_cmd_vtysh ip route A.B.C.D/M ethernet USP {nexthop A.B.C.D | distance <1-255> | vpn <1-128>}
 * - 功能说明 \n
 *	 使用该命令配置百兆以太接口静态路由
 * - 命令格式 \n
 *	 ip route A.B.C.D/M ethernet USP {nexthop A.B.C.D | distance <1-255> | vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *   |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *   |USP|百兆以太接口|
 *   |nexthop A.B.C.D |下一跳地址，点分十进制格式|
 *	 |distance <1-255>|Distance指定路由优先级，可选，默认为1|
 *	 |vpn <1-128>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图下，使用该命令配置静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   出接口为可选切最多能选一个，下一跳是否选择根据出接口相关
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# ip route 3.3.3.0/24 ethernet 1/1/6 nexthop 2.2.2.2 distance 20 vpn 10 \n
 *
 */
DEFSH (VTYSH_ROUTE, ip_route_static_ethernet_cmd_vtysh,
        "ip route A.B.C.D/M ethernet USP nexthop A.B.C.D {distance <1-255> | vpn <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        "Next hop\n"
        "IP address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page ip_route_static_gigabit_ethernet_cmd_vtysh ip route A.B.C.D/M gigabitethernet USP {nexthop A.B.C.D | distance <1-255> | vpn <1-1024>}
 * - 功能说明 \n
 *	 使用该命令配置千兆以太接口静态路由
 * - 命令格式 \n
 *	 ip route A.B.C.D/M gigabitethernet USP {nexthop A.B.C.D | distance <1-255> | vpn <1-1024>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *   |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *   |USP |千兆以太接口|
 *	 |nexthop A.B.C.D |下一跳地址，点分十进制格式|
 *	 |distance <1-255>|Distance指定路由优先级，可选，默认为1|
 *	 |vpn <1-1024>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图下，使用该命令配置静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   出接口为可选切最多能选一个，下一跳是否选择根据出接口相关
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# ip route 3.3.3.0/24 gigabitethernet 1/1/6 nexthop 2.2.2.2 distance 20 vpn 10 \n
 *
 */
DEFSH (VTYSH_ROUTE, ip_route_static_gigabit_ethernet_cmd_vtysh,
		"ip route A.B.C.D/M gigabitethernet USP nexthop A.B.C.D {distance <1-255> | vpn <1-128>}",
		"IP command\n"
		"Route command\n"
		"IP address and mask\n"
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		"Next hop\n"
		"IP address\n"
		"Routing priority\n"
		"<1-255>\n"
		"Vpn\n"
		"<1-128>\n")

/**
 * \page ip_route_static_xgigabit_ethernet_cmd_vtysh ip route A.B.C.D/M xgigabitethernet USP {nexthop A.B.C.D | distance <1-255> | vpn <1-128>}
 * - 功能说明 \n
 *	 使用该命令配置万兆以太接口静态路由
 * - 命令格式 \n
 *	 ip route A.B.C.D/M xgigabitethernet USP {nexthop A.B.C.D | distance <1-255> | vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *   |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *   |USP|万兆以太接口|
 *	 |nexthop A.B.C.D |下一跳地址，点分十进制格式|
 *	 |distance <1-255>|Distance指定路由优先级，可选，默认为1|
 *	 |vpn <1-128>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图下，使用该命令配置静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   出接口为可选切最多能选一个，下一跳是否选择根据出接口相关
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# ip route 3.3.3.0/24 xgigabitethernet 1/1/6 nexthop 2.2.2.2 distance 20 vpn 10 \n
 *
 */
DEFSH (VTYSH_ROUTE, ip_route_static_xgigabit_ethernet_cmd_vtysh,
        "ip route A.B.C.D/M xgigabitethernet USP nexthop A.B.C.D {distance <1-255> | vpn <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        "Next hop\n"
        "IP address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page ip_route_static_tunnel_cmd_vtysh ip route A.B.C.D/M tunnel USP {distance <1-255> | vpn <1-128>}
 * - 功能说明 \n
 *	 使用该命令配置trunk接口静态路由
 * - 命令格式 \n
 *	 ip route A.B.C.D/M tunnel USP {distance <1-255> | vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *   |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |USP|出接口为trunk口|
 *	 |distance <1-255>|Distance指定路由优先级，可选，默认为1|
 *	 |vpn <1-1024>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图下，使用该命令配置静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   出接口为可选切最多能选一个，下一跳是否选择根据出接口相关
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# ip route 3.3.3.0/24 tunnel 1/1/1 distance 20 vpn 10 \n
 *
 */
DEFSH (VTYSH_ROUTE, ip_route_static_tunnel_cmd_vtysh,
        "ip route A.B.C.D/M tunnel USP {distance <1-255> | vpn <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page ip_route_static_loopback_cmd_vtysh ip route A.B.C.D/M loopback <0-128> {distance <1-255> | vpn <1-128>}
 * - 功能说明 \n
 *	 使用该命令配置trunk接口静态路由
 * - 命令格式 \n
 *	 ip route A.B.C.D/M loopback <0-128> {distance <1-255> | vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |<0-128>|出接口为loopback口|
 *	 |distance <1-255>|Distance指定路由优先级，可选，默认为1|
 *	 |vpn <1-1024>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图下，使用该命令配置静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   出接口为可选切最多能选一个，下一跳是否选择根据出接口相关
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# ip route 3.3.3.0/24 loopback 0 distance 20 vpn 10 \n
 *
 */
DEFSH (VTYSH_ROUTE, ip_route_static_loopback_cmd_vtysh,
        "ip route A.B.C.D/M loopback <0-128> {distance <1-255> | vpn <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page ip_route_static_vlanif_cmd_vtysh ip route A.B.C.D/M vlanif nexthop A.B.C.D <1-4095> {distance <1-255> | vpn <1-128>}
 * - 功能说明 \n
 *	 使用该命令配置vlanif接口静态路由
 * - 命令格式 \n
 *	 ip route A.B.C.D/M vlanif <1-4095> nexthop A.B.C.D {distance <1-255> | vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |<1-4095>|出接口为vlanif口|
 *   |A.B.C.D |下一跳地址，点分十进制格式|
 *	 |distance <1-255>|Distance指定路由优先级，可选，默认为1|
 *	 |vpn <1-1024>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图下，使用该命令配置静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   出接口为可选切最多能选一个，下一跳是否选择根据出接口相关
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# ip route 3.3.3.0/24 vlanif 100 distance 20 vpn 10 \n
 *
 */
DEFSH (VTYSH_ROUTE, ip_route_static_vlanif_cmd_vtysh,
        "ip route A.B.C.D/M vlanif <1-4095> nexthop A.B.C.D {distance <1-255> | vpn <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
        "Next hop\n"
        "IP address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page ip_route_static_trunk_cmd_vtysh ip route A.B.C.D/M trunk TRUNK nexthop A.B.C.D {distance <1-255> | vpn <1-128>}
 * - 功能说明 \n
 *	 使用该命令配置vlanif接口静态路由
 * - 命令格式 \n
 *	 ip route A.B.C.D/M vlanif <1-4095> nexthop A.B.C.D {distance <1-255> | vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |TRUNK|出接口为trunk口|
 *   |A.B.C.D |下一跳地址，点分十进制格式|
 *	 |distance <1-255>|Distance指定路由优先级，可选，默认为1|
 *	 |vpn <1-1024>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图下，使用该命令配置静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   出接口为可选切最多能选一个，下一跳是否选择根据出接口相关
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# ip route 3.3.3.0/24 trunk 1 nexthop 1.1.1.1 distance 20 vpn 10 \n
 *
 */
DEFSH (VTYSH_ROUTE, ip_route_static_trunk_cmd_vtysh,
        "ip route A.B.C.D/M trunk TRUNK nexthop A.B.C.D {distance <1-255> | vpn <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        "Next hop\n"
        "IP address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page ip_route_static_balckhole_cmd_vtysh ip route A.B.C.D/M blackhole {vpn <1-128> | distance <1-255>}
 * - 功能说明 \n
 *	 添加一条黑洞路由
 * - 命令格式 \n
 *	 ip route A.B.C.D/M blackhole {vpn <1-128> | distance <1-255>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |distance <1-255>|Distance指定路由优先级，可选，默认为1|
 *	 |vpn <1-128>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图下，使用该命令添加一条黑洞路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   出接口为可选切最多能选一个，下一跳是否选择根据出接口相关
 *	 - 相关命令 \n
 *	   no ip route A.B.C.D/M blackhole {vpn <1-1024>} \n
 *     show ip route
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# ip route 4.4.4.0/24 blackhole distance 5 vpn 12 \n
 *
 */
DEFSH (VTYSH_ROUTE, ip_route_static_balckhole_cmd_vtysh,
        "ip route A.B.C.D/M blackhole {vpn <1-128> | distance <1-255>}",
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        "Blackhole routing\n"
        "Vpn\n"
        "<1-128>\n"
        "Routing priority\n"
        "<1-255>\n")

/**
 * \page no_ip_route_static_nexthop_cmd_vtysh no ip route A.B.C.D/M nexthop A.B.C.D {vpn <1-128>}
 * - 功能说明 \n
 *	 删除一条静态路由
 * - 命令格式 \n
 *	 no ip route A.B.C.D/M nexthop A.B.C.D {vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |A.B.C.D |下一跳地址，点分十进制格式|
 *	 |vpn <1-1024>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图下，使用该命令删除一条静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# no ip route 3.3.3.0/24 ethernet 1/1/2 nexthop 2.2.2.2 vpn 10 \n
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_route_static_nexthop_cmd_vtysh,
        "no ip route A.B.C.D/M nexthop A.B.C.D {vpn <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        "Nexthop\n"
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page no_ip_route_static_ethernet_cmd_vtysh no ip route A.B.C.D/M ethernet USP {nexthop A.B.C.D | vpn <1-128>}
 * - 功能说明 \n
 *	 删除百兆以太接口的静态路由
 * - 命令格式 \n
 *	 no ip route A.B.C.D/M ethernet USP {nexthop A.B.C.D | vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |USP|百兆以太接口|
 *	 |nexthop A.B.C.D |下一跳地址，点分十进制格式|
 *	 |vpn <1-1024>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在route视图下，使用该命令删除百兆以太接口的静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# no ip route 3.3.3.0/24 ethernet 1/1/2 nexthop 2.2.2.2 vpn 10 \n
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_route_static_ethernet_cmd_vtysh,
        "no ip route A.B.C.D/M ethernet USP nexthop A.B.C.D {vpn <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        "Nexthop\n"
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page no_ip_route_static_gigabit_ethernet_cmd_vtysh no ip route A.B.C.D/M gigabitethernet USP {nexthop A.B.C.D | vpn <1-1024>}
 * - 功能说明 \n
 *	 删除千兆以太接口的静态路由
 * - 命令格式 \n
 *	 no ip route A.B.C.D/M gigabitethernet USP {nexthop A.B.C.D | vpn <1-1024>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |USP|千兆以太接口|
 *	 |nexthop A.B.C.D |下一跳地址，点分十进制格式|
 *	 |vpn <1-1024>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在route视图下，使用该命令删除千兆以太接口的静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# no ip route 3.3.3.0/24 gigabitethernet 1/1/2 nexthop 2.2.2.2 vpn 10 \n
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_route_static_gigabit_ethernet_cmd_vtysh,
		"no ip route A.B.C.D/M gigabitethernet USP nexthop A.B.C.D {vpn <1-128>}",
		"Delete command\n"
		"IP command\n"
		"Route command\n"
		"IP address and mask\n"
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		"Nexthop\n"
		"Nexthop address\n"
		"Vpn\n"
		"<1-128>\n")

/**
 * \page no_ip_route_static_xgigabit_ethernet_cmd_vtysh no ip route A.B.C.D/M xgigabitethernet USP {nexthop A.B.C.D | vpn <1-128>}
 * - 功能说明 \n
 *	 删除万兆以太接口的静态路由
 * - 命令格式 \n
 *	 no ip route A.B.C.D/M xgigabitethernet USP {nexthop A.B.C.D | vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |USP|万兆以太接口|
 *	 |nexthop A.B.C.D |下一跳地址，点分十进制格式|
 *	 |vpn <1-1024>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在route视图下，使用该命令删除万兆以太接口的静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# no ip route 3.3.3.0/24 xgigabitethernet 1/1/2 nexthop 2.2.2.2 vpn 10 \n
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_route_static_xgigabit_ethernet_cmd_vtysh,
		"no ip route A.B.C.D/M xgigabitethernet USP nexthop A.B.C.D {vpn <1-128>}",
		"Delete command\n"
		"IP command\n"
		"Route command\n"
		"IP address and mask\n"
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		"Nexthop\n"
		"Nexthop address\n"
		"Vpn\n"
		"<1-128>\n")

/**
 * \page no_ip_route_static_tunnel_cmd_vtysh no ip route A.B.C.D/M tunnel USP {vpn <1-128>}
 * - 功能说明 \n
 *	 删除tunnel口的静态路由
 * - 命令格式 \n
 *	 no ip route A.B.C.D/M tunnel USP {vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |USP|出接口为tunnel口|
 *	 |vpn <1-1024>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在route视图下，使用该命令删除tunnel口的静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# no ip route A.B.C.D/M tunnel 1/1/1 vpn 1 \n
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_route_static_tunnel_cmd_vtysh,
        "no ip route A.B.C.D/M tunnel USP {vpn <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR
        "Vpn\n"
        "<1-128>\n")

/**
 * \page no_ip_route_static_loopback_cmd_vtysh no ip route A.B.C.D/M loopback <0-128> {vpn <1-128>}
 * - 功能说明 \n
 *	 删除loopback口的静态路由
 * - 命令格式 \n
 *	 no ip route A.B.C.D/M loopback <0-128> {vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |USP|出接口为tunnel口|
 *	 |vpn <1-1024>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在route视图下，使用该命令删除tunnel口的静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# no ip route A.B.C.D/M tunnel 1/1/1 vpn 1 \n
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_route_static_loopback_cmd_vtysh,
        "no ip route A.B.C.D/M loopback <0-128> {vpn <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR
        "Vpn\n"
        "<1-128>\n")

/**
 * \page no_ip_route_static_vlanif_cmd_vtysh no ip route A.B.C.D/M vlanif <1-4095> nexthop A.B.C.D {vpn <1-128>}
 * - 功能说明 \n
 *	 删除静态路由
 * - 命令格式 \n
 *	 no ip route A.B.C.D/M vlanif <1-4095> nexthop A.B.C.D {vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |<1-4095>|vlanif口|
 *   |A.B.C.D |下一跳地址，点分十进制格式|
 *	 |vpn <1-128>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在route视图下，使用该命令删除vlanif接口的静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# no ip route 3.3.3.0/24 vlanif 100 nexthop 1.1.1.1 vpn 1 \n
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_route_static_vlanif_cmd_vtysh,
        "no ip route A.B.C.D/M vlanif <1-4095> nexthop A.B.C.D {vpn <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
        "Nexthop\n"
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page no_ip_route_static_trunk_cmd_vtysh no ip route A.B.C.D/M trunk TRUNK nexthop A.B.C.D {vpn <1-128>}
 * - 功能说明 \n
 *	 删除静态路由
 * - 命令格式 \n
 *	 no ip route A.B.C.D/M trunk TRUNK nexthop A.B.C.D {vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |TRUNK|trunk口|
 *	 |A.B.C.D |下一跳地址，点分十进制格式|
 *	 |vpn <1-128>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在route视图下，使用该命令删除trunk接口的静态路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# no ip route 3.3.3.0/24 trunk 1 nexthop 1.1.1.1 vpn 1 \n
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_route_static_trunk_cmd_vtysh,
        "no ip route A.B.C.D/M trunk TRUNK nexthop A.B.C.D {vpn <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        "Nexthop\n"
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page no_ip_route_static_blackhole_cmd_vtysh no ip route A.B.C.D/M blackhole {vpn <1-128>}
 * - 功能说明 \n
 *	 删除一条黑洞路由
 * - 命令格式 \n
 *	 no ip route A.B.C.D/M blackhole {vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |<1-128>|路由所属vpn，可选，默认为0|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在route视图下，使用该命令删除一条黑洞路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# \n
 *	   Huahuan(config-route)# no ip route 4.4.4.0/24 blackhole vpn 12 \n
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_route_static_blackhole_cmd_vtysh,
        "no ip route A.B.C.D/M blackhole {vpn <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        "Specify the delete routing for blackhole\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page show_ip_route_single_cmd_vtysh show ip route (A.B.C.D/M|A.B.C.D) {vpn <1-128>}
 * - 功能说明 \n
 *	 查找指定路由
 * - 命令格式 \n
 *	 show ip route (A.B.C.D/M|A.B.C.D) {vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *   |A.B.C.D |点分十进制格式|
 *	 |A.B.C.D/M|网段地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |<1-128>|路由所属vpn，可选，默认为0|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图、config视图、接口视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图、config视图、接口视图下，使用该命令查找指定路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip route A.B.C.D/M {ethernet USP | tunnel USP | loopback <0-128> | vlanif <1-4095> | trunk TRUNK | nexthop A.B.C.D | distance <1-255>| vpn <1-1025>} \n
 *     show ip route \n
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, show_ip_route_single_cmd_vtysh,
        "show ip route (A.B.C.D/M|A.B.C.D) {vpn <1-128>}",
        "Show command\n"
        "IP command\n"
        "Route command\n"
        "IP address and mask\n"
        "IP address\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page show_ip_route_active_cmd_vtysh show ip route active {vpn <1-128>}
 * - 功能说明 \n
 *	 查看所有下发至fib路由
 * - 命令格式 \n
 *	 show ip route active {vpn <1-128>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<1-128>|路由所属vpn，可选，默认为0|
 *
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图、config视图、接口视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图、config视图、接口视图下，使用该命令查看所有下发至fib路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip route A.B.C.D/M {ethernet USP | tunnel USP | loopback <0-128> | vlanif <1-4095> | trunk TRUNK | nexthop A.B.C.D | distance <1-255>| vpn <1-1025>} \n
 *	   show ip route \n
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, show_ip_route_active_cmd_vtysh,
        "show ip route active {vpn <1-128>}",
        "Show command\n"
        "IP command\n"
        "Routing\n"
        "Routing is active\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page ip_urpf_cmd_vtysh ip urpf (strict|loose)
 * - 功能说明 \n
 *	 接口配置urpf功能
 * - 命令格式 \n
 *	 ip urpf (strict|loose)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |loose|配置urpf宽松检测|
 *   |strict|配置urpf严格检测|
 *
 * - 缺省情况 \n
 *	 urpf未配置
 * - 命令模式 \n
 *	 三层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在三层接口模式下，使用命令在接口配置urpf功能
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   no ip urpf \n
 *	   show ip route \n
 * - 使用举例 \n
 *	   Hios(config-gigabitethernet1/1/5)# ip urpf loose
 *
 */
DEFSH (VTYSH_ROUTE, ip_urpf_cmd_vtysh,
        "ip urpf (strict|loose)",
        "IP command\n"
        "Unicast Reverse Path Forwarding\n"
        "Strict mode\n"
        "Loose mode\n")

/**
 * \page no_ip_urpf_cmd_vtysh no ip urpf
 * - 功能说明 \n
 *	 接口urpf去使能
 * - 命令格式 \n
 *	 no ip urpf
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 urpf未配置
 * - 命令模式 \n
 *	 三层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在三层接口模式下，使用命令在接口urpf去使能
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	    ip urpf (loose|strict)
 * - 使用举例 \n
 *	   Hios(config-gigabitethernet1/1/5)# no ip urpf
 *
 */
DEFSH(VTYSH_ROUTE, no_ip_urpf_cmd_vtysh,
        "no ip urpf",
        "disable urpf\n"
        "IP command\n"
        "Unicast Reverse Path Forwarding\n")

/**
 * \page ip_l3vpn_cmd_vtysh ip l3vpn <1-128>
 * - 功能说明 \n
 *	 配置接口所属vpn
 * - 命令格式 \n
 *	 ip l3vpn <1-128>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<1-128>|绑定接口VPN|
 *
 * - 缺省情况 \n
 *	 vpn为0
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令配置接口所属vpn
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   no ip l3vpn \n
 *     show ip address \n
 * - 使用举例 \n
 *	   route(config-subif)# ip l3vpn 11
 *
 */
DEFSH (VTYSH_ROUTE, ip_l3vpn_cmd_vtysh,
        "ip l3vpn <1-128>",
        "IP command\n"
        "L3VPN command\n"
        "Number of the L3VPN instance\n")

/**
 * \page no_ip_l3vpn_cmd_vtysh no ip l3vpn
 * - 功能说明 \n
 *	 删除接口绑定vpn
 * - 命令格式 \n
 *	 no ip l3vpn
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 vpn为0
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令删除接口绑定vpn
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip l3vpn <1-128> \n
 *	   show ip address \n
 * - 使用举例 \n
 *	   route(config-subif)# no ip l3vpn
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_l3vpn_cmd_vtysh,
        "no ip l3vpn",
        "Delete command\n"
        "IP command\n"
        "L3VPN command\n")

/**
 * \page ip_addr_cmd_vtysh ip address A.B.C.D/M [slave]
 * - 功能说明 \n
 *	 配置接口主、从IP
 * - 命令格式 \n
 *	 ip address A.B.C.D/M [slave]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D/M|IP地址和掩码长度，IP是点分十进制格式，掩码长度是大于0小于等于32的数字。如：192.168.1.0/24|
 *	 |slave|指定配置IP为从IP|
 *
 * - 缺省情况 \n
 *	 主、从IP未配置
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令配置接口主、从IP
 *	 - 前置条件 \n
 *	   配置从IP时必须保证接口主IP存在
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   DHCP使能不允许配置接口静态主IP
 *	 - 相关命令 \n
 *	   no ip address {dhcp|slave}
 * - 使用举例 \n
 *	   route(config)# interface ethernet 0/0/2.1 \n
 *     route(config-subif)# ip address 10.1.1.1/24 \n
 *     route(config-subif)# ip address 10.1.2.1/24 slave \n
 *
 */
DEFSH (VTYSH_ROUTE, ip_addr_cmd_vtysh,
        "ip address A.B.C.D/M [slave]",
        "IP command\n"
        "Address\n"
        "Format A.B.C.D/M\n"
        "It is slave IP address\n")

/**
 * \page ip_addr_unnumbered_ethernet_cmd_vtysh ip address unnumbered interface ethernet USP
 * - 功能说明 \n
 *	 以太接口配置IP为DHCP分配
 * - 命令格式 \n
 *	 ip address unnumbered interface ethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|接口号|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令配置IP为DHCP分配
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   接口存在静态配置的主IP不允许使能DHCP
 *	 - 相关命令 \n
 *	   no ip address unnumbered
 * - 使用举例 \n
 *	   ip address unnumbered interface ethernet 0/0/1
 *
 */
DEFSH (VTYSH_ROUTE, ip_addr_unnumbered_ethernet_cmd_vtysh,
        "ip address unnumbered interface ethernet USP",
        "Specify IP configurations for interfaces\n"
        "Address\n"
        "Unnumbered\n"
        "Specify the interface whose IP address was unnumbered\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR)

/**
 * \page ip_addr_unnumbered_gigabit_ethernet_cmd_vtysh ip address unnumbered interface gigabitethernet USP
 * - 功能说明 \n
 *	 千兆以太接口配置IP为DHCP分配
 * - 命令格式 \n
 *	 ip address unnumbered interface gigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|接口号|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令配置IP为DHCP分配
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   接口存在静态配置的主IP不允许使能DHCP
 *	 - 相关命令 \n
 *	   no ip address unnumbered
 * - 使用举例 \n
 *	   ip address unnumbered interface gigabitethernet 0/0/1
 *
 */
DEFSH (VTYSH_ROUTE, ip_addr_unnumbered_gigabit_ethernet_cmd_vtysh,
		"ip address unnumbered interface gigabitethernet USP",
		"Specify IP configurations for interfaces\n"
		"Address\n"
		"Unnumbered\n"
		"Specify the interface whose IP address was unnumbered\n"
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)

/**
 * \page ip_addr_unnumbered_xgigabit_ethernet_cmd_vtysh ip address unnumbered interface xgigabitethernet USP
 * - 功能说明 \n
 *	 万兆以太接口配置IP为DHCP分配
 * - 命令格式 \n
 *	 ip address unnumbered interface xgigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|接口号|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令配置IP为DHCP分配
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   接口存在静态配置的主IP不允许使能DHCP
 *	 - 相关命令 \n
 *	   no ip address unnumbered
 * - 使用举例 \n
 *	   ip address unnumbered interface xgigabitethernet 0/0/1
 *
 */
DEFSH (VTYSH_ROUTE, ip_addr_unnumbered_xgigabit_ethernet_cmd_vtysh,
        "ip address unnumbered interface xgigabitethernet USP",
        "Specify IP configurations for interfaces\n"
        "Address\n"
        "Unnumbered\n"
        "Specify the interface whose IP address was unnumbered\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)

/**
 * \page ip_addr_unnumbered_tunnel_cmd_vtysh ip address unnumbered tunnel USP
 * - 功能说明 \n
 *	 隧道配置IP为DHCP分配
 * - 命令格式 \n
 *	 ip address unnumbered tunnel USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|接口号|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令配置IP为DHCP分配
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   接口存在静态配置的主IP不允许使能DHCP
 *	 - 相关命令 \n
 *	   no ip address unnumbered
 * - 使用举例 \n
 *	   ip address unnumbered interface tunnel 0/0/1
 *
 */
DEFSH (VTYSH_ROUTE, ip_addr_unnumbered_tunnel_cmd_vtysh,
        "ip address unnumbered interface tunnel USP",
        "Specify IP configurations for interfaces\n"
        "Address\n"
        "Unnumbered\n"
        "Specify the interface whose IP address was unnumbered\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR)

/**
 * \page ip_addr_unnumbered_loopback_cmd_vtysh ip address unnumbered loopback USP
 * - 功能说明 \n
 *	 环回口配置IP为DHCP分配
 * - 命令格式 \n
 *	 ip address unnumbered loopback USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|接口号|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令配置IP为DHCP分配
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   接口存在静态配置的主IP不允许使能DHCP
 *	 - 相关命令 \n
 *	   no ip address unnumbered
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, ip_addr_unnumbered_loopback_cmd_vtysh,
        "ip address unnumbered interface loopback <0-128>",
        "Specify IP configurations for interfaces\n"
        "Address\n"
        "Unnumbered\n"
        "Specify the interface whose IP address was unnumbered\n"
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR)

/**
 * \page no_ip_addr_cmd_vtysh no ip address [slave]
 * - 功能说明 \n
 *	 删除接口静态主、从IP
 * - 命令格式 \n
 *	 no ip address [slave]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |Slave|删除接口从IP，可选（未选删除接口静态主IP|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令删除接口静态主、从IP
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip address A.B.C.D/M {slave}
 * - 使用举例 \n
 *	   route(config-subif)# no ip address \n
 *     route(config-subif)# no ip address slave
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_addr_cmd_vtysh,
        "no ip address [slave]",
        "Delete command\n"
        "IP command\n"
        "Address\n"
        "IP address is slave\n")

/**
 * \page no_ip_addr_dhcp_cmd_vtysh no ip address dhcp
 * - 功能说明 \n
 *	 接口DHCP去使能
 * - 命令格式 \n
 *	 no ip address dhcp
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令在接口进行DHCP去使能
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip address dhcp
 * - 使用举例 \n
 *	   route(config-subif)# no ip address \n
 *	   route(config-subif)# no ip address dhcp
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_addr_dhcp_cmd_vtysh,
        "no ip address dhcp",
        "Delete command\n"
        "IP command\n"
        "Address\n"
        "IP address allocated by DHCP\n")

/**
 * \page no_ip_addr_dhcp_save_cmd_vtysh no ip address dhcp save
 * - 功能说明 \n
 *	 取消DHCP地址固化
 * - 命令格式 \n
 *	 no ip address dhcp save
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 地址未固化
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令取消DHCP地址固化
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip address dhcp save
 * - 使用举例 \n
 *	   Hios(config-gigabitethernet1/1/5)# no ip address dhcp save
 *
 */
DEFSH (VTYSH_ROUTE, no_ip_addr_dhcp_save_cmd_vtysh,
        "no ip address dhcp save",
        "Delete command\n"
        "IP command\n"
        "Address\n"
        "IP address allocated by DHCP\n"
        "Save IP\n")


DEFSH (VTYSH_ROUTE, no_ip_addr_unnumbered_cmd_vtysh,
        "no ip address unnumbered",
        "Delete command\n"
        "IP command\n"
        "Address\n"
        "IP address is unnumbered\n")


DEFSH(VTYSH_ROUTE, ipv6_addr_auto_link_local_cmd_vtysh,
        "ipv6 address auto link-local",
        "Specify IPV6 configurations for interfaces\n"
        "Configure IPv6 address on interface\n"
        "Auto-Config Address\n"
        "Use link-local address\n")

DEFSH(VTYSH_ROUTE,ipv6_addr_auto_generate_cmd_vtysh,
		"ipv6 address X:X:X:X:X:X:X:X <1-64> eui-64",
		"Specify IPV6 configurations for interfaces\n"
		"Configure IPv6 address on interface\n"
		"Format X:X:X:X:X:X:X:X(X:X::X:X)\n"
		"IPv6 prefix length <1-64>\n"
		"Use eui-64 interface identifier\n")

DEFSH(VTYSH_ROUTE, ipv6_addr_link_local_cmd_vtysh,
        "ipv6 address X:X:X:X:X:X:X:X link-local",
        "Specify IPV6 configurations for interfaces\n"
        "Configure IPv6 address on interface\n"
        "Format X:X:X:X:X:X:X:X(X:X::X:X)\n"
        "Use link-local address\n")


DEFSH(VTYSH_ROUTE, no_ipv6_addr_auto_link_local_cmd_vtysh,
        "no ipv6 address auto link-local",
        NO_STR
        "Specify IPV6 configurations for interfaces\n"
        "Configure IPv6 address on interface\n"
        "Auto-Config Address\n"
        "Use link-local address\n")


DEFSH(VTYSH_ROUTE, no_ipv6_addr_link_local_cmd_vtysh,
        "no ipv6 address link-local",
        NO_STR
        "Specify IPV6 configurations for interfaces\n"
        "Configure IPv6 address on interface\n"
        "Use link-local address\n")


DEFSH (VTYSH_ROUTE, ipv6_addr_cmd_vtysh,
        "ipv6 address X:X:X:X:X:X:X:X/M [slave]",
        "Specify IPV6 configurations for interfaces\n"
        "Address\n"
        "Format X:X:X:X:X:X:X:X/<1-128>(X:X::X:X/<1-128>)\n"
        "It is slave IPV6 address\n")


DEFSH (VTYSH_ROUTE, no_ipv6_addr_cmd_vtysh,
        "no ipv6 address [slave]",
        NO_STR
        "Specify IPV6 configurations for interfaces\n"
        "Address\n"
        "It is slave IPV6 address\n")

/**
 * \page show_ip_addr_cmd_vtysh show ip address
 * - 功能说明 \n
 *	 显示接口IP
 * - 命令格式 \n
 *	 show ip address
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 route模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在route模式下，使用命令显示接口IP
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip address A.B.C.D/M {slave} \n
 *     ip address dhcp \n
 *     no ip address
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, show_ip_addr_cmd_vtysh,
        "show ip address",
        "Show command\n"
        "IP command\n"
        "Address\n")

/**
 * \page show_ip_interface_cmd_vtysh show ip interface
 * - 功能说明 \n
 *	 显示所有三层接口配置
 * - 命令格式 \n
 *	 show ip interface
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在config模式下，使用命令显示所有三层接口配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip address A.B.C.D/M {slave} \n
 *	   ip address dhcp \n
 *	   no ip address{dhcp|slave}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, show_ip_interface_cmd_vtysh,
        "show ip interface",
        "Show command\n"
        "IP command\n"
        "Interface mode L3\n")


DEFSH (VTYSH_ROUTE, show_ip_interface_brief_cmd_vtysh,
        "show ip interface brief",
        "Show command\n"
        "IP command\n"
        "Interface mode L3\n"
        "Brief summary of IP status and configuration\n")

/**
 * \page show_ip_interface_ethernet_cmd_vtysh show ip interface ethernet USP
 * - 功能说明 \n
 *	 显示指定三层接口配置
 * - 命令格式 \n
 *	 show ip interface ethernet USP
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |USP|指定接口|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在config模式下，使用命令显示指定三层接口配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip address A.B.C.D/M {slave} \n
 *	   ip address dhcp \n
 *	   no ip address{dhcp|slave}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, show_ip_interface_ethernet_cmd_vtysh,
        "show ip interface ethernet USP",
        "Show command\n"
        "IP command\n"
        "Interface mode L3\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR)

/**
 * \page show_ip_interface_gigabit_ethernet_cmd_vtysh show ip interface gigabitethernet USP
 * - 功能说明 \n
 *	 显示指定三层接口配置
 * - 命令格式 \n
 *	 show ip interface gigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|指定接口|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在config模式下，使用命令显示指定三层接口配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip address A.B.C.D/M {slave} \n
 *	   ip address dhcp \n
 *	   no ip address{dhcp|slave}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, show_ip_interface_gigabit_ethernet_cmd_vtysh,
		"show ip interface gigabitethernet USP",
		"Show command\n"
		"IP command\n"
		"Interface mode L3\n"
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)

/**
 * \page show_ip_interface_xgigabit_ethernet_cmd_vtysh show ip interface xgigabitethernet USP
 * - 功能说明 \n
 *	 显示指定三层接口配置
 * - 命令格式 \n
 *	 show ip interface xgigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|指定接口|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在config模式下，使用命令显示指定三层接口配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip address A.B.C.D/M {slave} \n
 *	   ip address dhcp \n
 *	   no ip address{dhcp|slave}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, show_ip_interface_xgigabit_ethernet_cmd_vtysh,
        "show ip interface xgigabitethernet USP",
        "Show command\n"
        "IP command\n"
        "Interface mode L3\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)

/**
 * \page show_ip_interface_tunnel_cmd_vtysh show ip interface tunnel USP
 * - 功能说明 \n
 *	 显示指定三层接口配置
 * - 命令格式 \n
 *	 show ip interface tunnel USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|指定接口|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在config模式下，使用命令显示指定三层接口配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip address A.B.C.D/M {slave} \n
 *	   ip address dhcp \n
 *	   no ip address{dhcp|slave}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, show_ip_interface_tunnel_cmd_vtysh,
        "show ip interface tunnel USP",
        "Show command\n"
        "IP command\n"
        "Interface mode L3\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR)

/**
 * \page show_ip_interface_loopback_cmd_vtysh show ip interface loopback <0-128>
 * - 功能说明 \n
 *	 显示环回接口配置
 * - 命令格式 \n
 *	 show ip interface loopback <0-128>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<0-128>|loopback接口|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在config模式下，使用命令显示显示环回接口配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip address A.B.C.D/M {slave} \n
 *	   ip address dhcp \n
 *	   no ip address{dhcp|slave}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, show_ip_interface_loopback_cmd_vtysh,
        "show ip interface loopback <0-128>",
        "Show command\n"
        "IP command\n"
        "Interface mode L3\n"
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR)

/**
 * \page show_ip_interface_vlanif_cmd_vtysh show ip interface vlanif <1-4094>
 * - 功能说明 \n
 *	 显示vlanif接口配置
 * - 命令格式 \n
 *	 show ip interface vlanif <1-4094>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<1-4094>|vlanif接口|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在config模式下，使用命令显示显示环回接口配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip address A.B.C.D/M {slave} \n
 *	   ip address dhcp \n
 *	   no ip address{dhcp|slave}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, show_ip_interface_vlanif_cmd_vtysh,
        "show ip interface vlanif <1-4094>",
        "Show command\n"
        "IP command\n"
        "Interface mode L3\n"
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR)

/**
 * \page show_ip_interface_trunk_cmd_vtysh show ip interface trunk TRUNK
 * - 功能说明 \n
 *	 显示trunk接口配置
 * - 命令格式 \n
 *	 show ip interface trunk TRUNK
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |TRUNK|trunk接口|
 *
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在config模式下，使用命令显示trunk接口配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip address A.B.C.D/M {slave} \n
 *	   ip address dhcp \n
 *	   no ip address{dhcp|slave}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, show_ip_interface_trunk_cmd_vtysh,
        "show ip interface trunk TRUNK",
        "Show command\n"
        "IP command\n"
        "Interface mode L3\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR)

/**
 * \page show_ip_route_cmd_vtysh show ip route {vpn <1-128>}
 * - 功能说明 \n
 *	 查看所有路由
 * - 命令格式 \n
 *	 show ip route {vpn <1-128>}
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 路由不存在
 * - 命令模式 \n
 *	 Route视图、config视图、接口视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图、config视图、接口视图下，使用命令查看路由
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   ip route A.B.C.D/M {ethernet USP | tunnel USP | loopback <0-128> | vlanif <1-4095> | trunk TRUNK | nexthop A.B.C.D | distance <1-255>| vpn <1-1025>} \n
 *     show ip route
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH (VTYSH_ROUTE, show_ip_route_cmd_vtysh,
        "show ip route {vpn <1-128>}",
        "Show command\n"
        "IP command\n"
        "Routing table\n"
        "Vpn\n"
        "<1-128>\n")

/**
 * \page ip_address_dhcp_alloc_cmd_vtysh ip address dhcp
 * - 功能说明 \n
 *	 接口配置IP为DHCP分配
 * - 命令格式 \n
 *	 ip address dhcp
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 IP未配置
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令配置IP为DHCP分配
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   接口存在静态配置的主IP不允许使能DHCP
 *	 - 相关命令 \n
 *	   no ip address {dhcp|slave}
 * - 使用举例 \n
 *	   route(config-subif)# ip address dhcp \n
 *
 */
DEFSH (VTYSH_ROUTE, ip_address_dhcp_alloc_cmd_vtysh,
        "ip address dhcp",
        "Specify IP configurations for interfaces\n"
        "Address\n"
        "IP address allocated by DHCP\n")

/**
 * \page ip_address_dhcp_save_cmd_vtysh ip address dhcp save
 * - 功能说明 \n
 *	 配置DHCP地址固化
 * - 命令格式 \n
 *	 ip address dhcp save
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 地址未固化
 * - 命令模式 \n
 *	 3层接口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在3层接口模式下，使用命令配置DHCP地址固化
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   no ip address dhcp save
 * - 使用举例 \n
 *	   Hios(config-gigabitethernet1/1/5)# ip address dhcp save
 *
 */
DEFSH (VTYSH_ROUTE, ip_address_dhcp_save_cmd_vtysh,
        "ip address dhcp save",
        "Specify IP configurations for interfaces\n"
        "Address\n"
        "IP address allocated by DHCP\n"
        "Save IP\n")

/**
 * \page show_ip_route_statistics_cmd_vtysh show ip route statistics {vpn <0-128>}
 * - 功能说明 \n
 *	 显示路由所有统计条目
 * - 命令格式 \n
 *	 show ip route statistics {vpn <0-128>}
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |<0-128>|路由所属vpn，可选|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 Route视图、config视图、接口视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   在Route视图、config视图、接口视图下，使用该命令显示路由统计条目
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
DEFSH (VTYSH_ROUTE, show_ip_route_statistics_cmd_vtysh,
        "show ip route statistics {vpn <1-128>}",
        SHOW_STR
        "IP command\n"
        "Routing\n"
        "Statistics routing\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, ipv6_address_dhcp_alloc_cmd_vtysh,
        "ipv6 address dhcp",
        "Specify IPv6 configurations for interfaces\n"
        "Address\n"
        "IPv6 address allocated by DHCP\n")
        
DEFSH (VTYSH_ROUTE, no_ipv6_addr_dhcp_cmd_vtysh,
        "no ipv6 address dhcp",
        "Delete command\n"
        "IPv6 command\n"
        "Address\n"
        "IPv6 address allocated by DHCP\n")
        
DEFSH (VTYSH_ROUTE, ipv6_address_dhcp_pd_alloc_cmd_vtysh,
		"ipv6 address dhcp pd",
		"Specify IPv6 configurations for interfaces\n"
		"Address\n"
		"IPv6 address allocated by DHCP\n"
		"IPv6 address prefix\n")
		
DEFSH (VTYSH_ROUTE, no_ipv6_addr_dhcp_pd_cmd_vtysh,
		"no ipv6 address dhcp pd",
		"Delete command\n"
		"IPv6 command\n"
		"Address\n"
		"IPv6 address allocated by DHCP\n"
		"IPv6 address prefix\n")

/* IPv6 路由配置命令行 */
DEFSH (VTYSH_ROUTE, ipv6_route_static_nexthop_cmd_vtysh,
        "ipv6 route X:X:X:X:X:X:X:X/M nexthop X:X:X:X:X:X:X:X {distance <1-255> | vpn <1-128>}",
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        "Next hop\n"
        "IPv6 address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, ipv6_route_static_ethernet_cmd_vtysh,
        "ipv6 route X:X:X:X:X:X:X:X/M ethernet USP nexthop X:X:X:X:X:X:X:X {distance <1-255> | vpn <1-128>}",
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        "Next hop\n"
        "IPv6 address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, ipv6_route_static_gigabit_ethernet_cmd_vtysh,
		"ipv6 route X:X:X:X:X:X:X:X/M gigabitethernet USP nexthop X:X:X:X:X:X:X:X {distance <1-255> | vpn <1-128>}",
		"IPv6 command\n"
		"Route command\n"
		"IPv6 address and mask\n"
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		"Next hop\n"
		"IPv6 address\n"
		"Routing priority\n"
		"<1-255>\n"
		"Vpn\n"
		"<1-128>\n")


DEFSH (VTYSH_ROUTE, ipv6_route_static_xgigabit_ethernet_cmd_vtysh,
		"ipv6 route X:X:X:X:X:X:X:X/M xgigabitethernet USP nexthop X:X:X:X:X:X:X:X {distance <1-255> | vpn <1-128>}",
		"IPv6 command\n"
		"Route command\n"
		"IPv6 address and mask\n"
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		"Next hop\n"
		"IPv6 address\n"
		"Routing priority\n"
		"<1-255>\n"
		"Vpn\n"
		"<1-128>\n")


DEFSH (VTYSH_ROUTE, ipv6_route_static_tunnel_cmd_vtysh,
        "ipv6 route X:X:X:X:X:X:X:X/M tunnel USP {distance <1-255> | vpn <1-128>}",
        "IPv6 command\n"
        "Route command\n"
        "IPv6address and mask\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, ipv6_route_static_loopback_cmd_vtysh,
        "ipv6 route X:X:X:X:X:X:X:X/M loopback <0-128> {distance <1-255> | vpn <1-128>}",
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, ipv6_route_static_vlanif_cmd_vtysh,
        "ipv6 route X:X:X:X:X:X:X:X/M vlanif <1-4095> nexthop X:X:X:X:X:X:X:X {distance <1-255> | vpn <1-128>}",
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
        "Next hop\n"
        "IPv6 address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, ipv6_route_static_trunk_cmd_vtysh,
        "ipv6 route X:X:X:X:X:X:X:X/M trunk TRUNK nexthop X:X:X:X:X:X:X:X {distance <1-255> | vpn <1-128>}",
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        "Next hop\n"
        "IPv6 address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, ipv6_route_static_balckhole_cmd_vtysh,
        "ipv6 route X:X:X:X:X:X:X:X/M blackhole {vpn <1-128> | distance <1-255>}",
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        "Blackhole routing\n"
        "Vpn\n"
        "<1-128>\n"
        "Routing priority\n"
        "<1-255>\n")


DEFSH (VTYSH_ROUTE, no_ipv6_route_static_nexthop_cmd_vtysh,
        "no ipv6 route X:X:X:X:X:X:X:X/M nexthop X:X:X:X:X:X:X:X {vpn <1-128>}",
        "Delete command\n"
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        "Nexthop\n"
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, no_ipv6_route_static_ethernet_cmd_vtysh,
        "no ipv6 route X:X:X:X:X:X:X:X/M ethernet USP {nexthop X:X:X:X:X:X:X:X | vpn <1-128>}",
        "Delete command\n"
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        "Nexthop\n"
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")

DEFSH (VTYSH_ROUTE, no_ipv6_route_static_gigabit_ethernet_cmd_vtysh,
		"no ipv6 route X:X:X:X:X:X:X:X/M gigabitethernet USP {nexthop X:X:X:X:X:X:X:X | vpn <1-128>}",
		"Delete command\n"
		"IPv6 command\n"
		"Route command\n"
		"IPv6 address and mask\n"
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		"Nexthop\n"
		"Nexthop address\n"
		"Vpn\n"
		"<1-128>\n")

DEFSH (VTYSH_ROUTE, no_ipv6_route_static_xgigabit_ethernet_cmd_vtysh,
        "no ipv6 route X:X:X:X:X:X:X:X/M xgigabitethernet USP {nexthop X:X:X:X:X:X:X:X | vpn <1-128>}",
        "Delete command\n"
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        "Nexthop\n"
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, no_ipv6_route_static_tunnel_cmd_vtysh,
        "no ipv6 route X:X:X:X:X:X:X:X/M tunnel USP {vpn <1-128>}",
        "Delete command\n"
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, no_ipv6_route_static_loopback_cmd_vtysh,
        "no ipv6 route X:X:X:X:X:X:X:X/M loopback <0-128> {vpn <1-128>}",
        "Delete command\n"
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, no_ipv6_route_static_vlanif_cmd_vtysh,
        "no ipv6 route X:X:X:X:X:X:X:X/M vlanif <1-4095> nexthop X:X:X:X:X:X:X:X {vpn <1-128>}",
        "Delete command\n"
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
        "Nexthop\n"
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, no_ipv6_route_static_trunk_cmd_vtysh,
        "no ipv6 route X:X:X:X:X:X:X:X/M trunk TRUNK nexthop X:X:X:X:X:X:X:X {vpn <1-128>}",
        "Delete command\n"
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        "Nexthop\n"
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, no_ipv6_route_static_blackhole_cmd_vtysh,
        "no ipv6 route X:X:X:X:X:X:X:X/M blackhole {vpn <1-128>}",
        "Delete command\n"
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address and mask\n"
        "Specify the delete routing for blackhole\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, show_ipv6_route_single_cmd_vtysh,
        "show ipv6 route (X:X:X:X:X:X:X:X|X:X:X:X:X:X:X:X/M) {vpn <1-128>}",
        "Show command\n"
        "IPv6 command\n"
        "Route command\n"
        "IPv6 address\n"
        "IPv6 address and mask\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, show_ipv6_route_active_cmd_vtysh,
        "show ipv6 route active {vpn <1-128>}",
        "Show command\n"
        "IPv6 command\n"
        "Routing\n"
        "Routing is active\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, show_ipv6_route_cmd_vtysh,
        "show ipv6 route {vpn <1-128>}",
        "Show command\n"
        "IPv6 command\n"
        "Routing table\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, show_ipv6_route_statistics_cmd_vtysh,
        "show ipv6 route statistics {vpn <1-128>}",
        "Show command\n"
        "IPv6 command\n"
        "Routing\n"
        "Statistics routing\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, route_ecmp_enable_cmd_vtysh,
        "ip route ecmp enable",
        "IP command\n"
        "Routing\n"
        "Equal Cost Multipath Routing\n"
        "Enable\n")


DEFSH (VTYSH_ROUTE, no_route_ecmp_enable_cmd_vtysh,
        "no ip route ecmp enable",
        NO_STR
        "IP command\n"
        "Routing\n"
        "Equal Cost Multipath Routing\n"
        "Enable\n")


DEFSH (VTYSH_ROUTE, route_frr_enable_cmd_vtysh,
        "ip route frr enable",
        "IP command\n"
        "Routing\n"
        "Fast Reroute\n"
        "Enable\n")


DEFSH (VTYSH_ROUTE, no_route_frr_enable_cmd_vtysh,
        "no ip route frr enable",
        NO_STR
        "IP command\n"
        "Routing\n"
        "Fast Reroute\n"
        "Enable\n")


DEFSH (VTYSH_ROUTE, routev6_ecmp_enable_cmd_vtysh,
        "ip routev6 ecmp enable",
        "IPv6 command\n"
        "Routing\n"
        "Equal Cost Multipath Routing\n"
        "Enable\n")


DEFSH (VTYSH_ROUTE, no_routev6_ecmp_enable_cmd_vtysh,
        "no ip routev6 ecmp enable",
        NO_STR
        "IPv6 command\n"
        "Routing\n"
        "Equal Cost Multipath Routing\n"
        "Enable\n")


DEFSH (VTYSH_ROUTE, routev6_frr_enable_cmd_vtysh,
        "ip routev6 frr enable",
        "IPv6 command\n"
        "Routing\n"
        "Fast Reroute\n"
        "Enable\n")


DEFSH (VTYSH_ROUTE, no_routev6_frr_enable_cmd_vtysh,
        "no ip routev6 frr enable",
        NO_STR
        "IPv6 command\n"
        "Routing\n"
        "Fast Reroute\n"
        "Enable\n")


/* debug 日志发送到 syslog 使能状态设置 */
// DEFSH (VTYSH_ROUTE, route_log_level_ctl_cmd_vtysh,
//         "debug route(enable | disable)",
//         "Output log of debug level\n"
//         "Program name\n"
//         "Enable\n"
//         "Disable\n")

DEFSH(VTYSH_ROUTE,
    route_debug_monitor_cmd_vtysh,
    "debug route (enable|disable)(all)",
    "Debug information to moniter\n"
    "Programe name\n"
    "Enable statue\n"
    "Disatble statue\n"
    "Type name of all debug\n")

/* debug 日志发送到 syslog 使能状态显示 */
// DEFSH (VTYSH_ROUTE,route_show_log_level_ctl_cmd_vtysh,
//         "show debug route",
//         SHOW_STR
//         "Output log of debug level\n"
//         "Program name\n")


DEFSH (VTYSH_ROUTE, show_this_route_cmd_vtysh,
        "show this",
        SHOW_STR
        "This\n")

/* h3c route cmd added begin */




DEFSH (VTYSH_ROUTE, h3c_ip_route_static_nexthop_cmd_vtysh,
        "ip route-static A.B.C.D (M | A.B.C.D) A.B.C.D {distance <1-255> | vpn-instance <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        "Next hop\n"
        "IP address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, h3c_ip_route_static_ethernet_cmd_vtysh,
        "ip route-static A.B.C.D (M | A.B.C.D) ethernet USP {A.B.C.D | distance <1-255> | vpn-instance <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        "IP address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")

DEFSH (VTYSH_ROUTE, h3c_ip_route_static_gigabit_ethernet_cmd_vtysh,
        "ip route-static A.B.C.D (M | A.B.C.D) gigabitethernet USP {A.B.C.D | distance <1-255> | vpn-instance <1-1024>}",
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        "IP address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-1024>\n")

DEFSH (VTYSH_ROUTE, h3c_ip_route_static_xgigabit_ethernet_cmd_vtysh,
        "ip route-static A.B.C.D (M | A.B.C.D) xgigabitethernet USP {A.B.C.D | distance <1-255> | vpn-instance <1-1024>}",
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        "IP address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-1024>\n")

DEFSH (VTYSH_ROUTE, h3c_ip_route_static_tunnel_cmd_vtysh,
        "ip route-static A.B.C.D (M | A.B.C.D) tunnel USP {distance <1-255> | vpn-instance <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, h3c_ip_route_static_loopback_cmd_vtysh,
        "ip route-static A.B.C.D (M | A.B.C.D) loopback <0-128> {distance <1-255> | vpn-instance <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, h3c_ip_route_static_vlanif_cmd_vtysh,
        "ip route-static A.B.C.D (M | A.B.C.D) vlanif <1-4095> A.B.C.D {distance <1-255> | vpn-instance <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
        "IP address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, h3c_ip_route_static_trunk_cmd_vtysh,
        "ip route-static A.B.C.D (M | A.B.C.D) trunk TRUNK A.B.C.D {distance <1-255> | vpn-insrtance <1-128>}",
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        "IP address\n"
        "Routing priority\n"
        "<1-255>\n"
        "Vpn\n"
        "<1-128>\n")



DEFSH (VTYSH_ROUTE, no_h3c_ip_route_static_nexthop_cmd_vtysh,
        "no ip route-static A.B.C.D (M | A.B.C.D) A.B.C.D {vpn-insrtance <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, no_h3c_ip_route_static_ethernet_cmd_vtysh,
        "no ip route-static A.B.C.D (M | A.B.C.D) ethernet USP {A.B.C.D | vpn-insrtance <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")

DEFSH (VTYSH_ROUTE, no_h3c_ip_route_static_gigabit_ethernet_cmd_vtysh,
        "no ip route-static A.B.C.D (M | A.B.C.D) gigabitethernet USP {A.B.C.D | vpn-insrtance <1-1024>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        "Nexthop address\n"
        "Vpn\n"
        "<1-1024>\n")

DEFSH (VTYSH_ROUTE, no_h3c_ip_route_static_xgigabit_ethernet_cmd_vtysh,
        "no ip route-static A.B.C.D (M | A.B.C.D) xgigabitethernet USP {A.B.C.D | vpn-insrtance <1-1024>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        "Nexthop address\n"
        "Vpn\n"
        "<1-1024>\n")


DEFSH (VTYSH_ROUTE, no_h3c_ip_route_static_tunnel_cmd_vtysh,
        "no ip route-static A.B.C.D (M | A.B.C.D) tunnel USP {vpn-insrtance <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, no_h3c_ip_route_static_loopback_cmd_vtysh,
        "no ip route-static A.B.C.D (M | A.B.C.D) loopback <0-128> {vpn-insrtance <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, no_h3c_ip_route_static_vlanif_cmd_vtysh,
        "no ip route-static A.B.C.D (M | A.B.C.D) vlanif <1-4095> A.B.C.D {vpn-insrtance <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, no_h3c_ip_route_static_trunk_cmd_vtysh,
        "no ip route-static A.B.C.D (M | A.B.C.D) trunk TRUNK A.B.C.D {vpn-insrtance <1-128>}",
        "Delete command\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")


/*static route undo cmd*/
DEFSH (VTYSH_ROUTE, undo_h3c_ip_route_static_nexthop_cmd_vtysh,
        "undo ip route-static A.B.C.D (M | A.B.C.D) A.B.C.D {vpn-insrtance <1-128>}",
        "Cancel current setting\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, undo_h3c_ip_route_static_ethernet_cmd_vtysh,
        "undo ip route-static A.B.C.D (M | A.B.C.D) ethernet USP {A.B.C.D | vpn-insrtance <1-128>}",
        "Cancel current setting\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")

DEFSH (VTYSH_ROUTE, undo_h3c_ip_route_static_gigabit_ethernet_cmd_vtysh,
        "undo ip route-static A.B.C.D (M | A.B.C.D) gigabitethernet USP {A.B.C.D | vpn-insrtance <1-1024>}",
        "Cancel current setting\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        "Nexthop address\n"
        "Vpn\n"
        "<1-1024>\n")

DEFSH (VTYSH_ROUTE, undo_h3c_ip_route_static_xgigabit_ethernet_cmd_vtysh,
        "undo ip route-static A.B.C.D (M | A.B.C.D) xgigabitethernet USP {A.B.C.D | vpn-insrtance <1-1024>}",
        "Cancel current setting\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        "Nexthop address\n"
        "Vpn\n"
        "<1-1024>\n")


DEFSH (VTYSH_ROUTE, undo_h3c_ip_route_static_tunnel_cmd_vtysh,
        "undo ip route-static A.B.C.D (M | A.B.C.D) tunnel USP {vpn-insrtance <1-128>}",
        "Cancel current setting\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, undo_h3c_ip_route_static_loopback_cmd_vtysh,
        "undo ip route-static A.B.C.D (M | A.B.C.D) loopback <0-128> {vpn-insrtance <1-128>}",
        "Cancel current setting\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, undo_h3c_ip_route_static_vlanif_cmd_vtysh,
        "undo ip route-static A.B.C.D (M | A.B.C.D) vlanif <1-4095> A.B.C.D {vpn-insrtance <1-128>}",
        "Cancel current setting\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, undo_h3c_ip_route_static_trunk_cmd_vtysh,
        "undo ip route-static A.B.C.D (M | A.B.C.D) trunk TRUNK A.B.C.D {vpn-insrtance <1-128>}",
        "Cancel current setting\n"
        "IP command\n"
        "Route command\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        "Nexthop address\n"
        "Vpn\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, h3c_ip_addr_config_cmd,
        "ip address A.B.C.D (M | A.B.C.D) [sub]",
        "IP command\n"
        "Address\n"
        "IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        "Indicate a subordinate address\n")


DEFSH(VTYSH_ROUTE,undo_h3c_ip_addr_config_cmd,
      "undo ip address [sub]",
      "Cancel current setting\n"
      "Specify IP configuration\n"
      "Set the IP address of an interface\n"
      "Indicate a subordinate address\n"
    )

DEFSH(VTYSH_ROUTE, undo_h3c_ip_addr_config_ip_cmd,
     "undo ip address A.B.C.D (M | A.B.C.D)[sub]",
     "Cancel current setting\n"
     "Specify IP configuration\n"
     "Set the IP address of an interface\n"
     "IP address\n"
     "IP mask length\n"
     "IP mask\n"
     "Indicate a subordinate address\n"
     )



DEFSH (VTYSH_ROUTE, h3c_display_ip_route_single_cmd_vtysh,
        "display ip routing-table vpn-instance <1-128> A.B.C.D (M | A.B.C.D) {verbose}",
        "Display current system information\n"
        "IP information\n"
        "IP routing table\n"
        "Destination IP address and mask\n"
        "Destination IP address\n"
        "Specify a VPN instance\n"
        "<1-128>\n")


DEFSH (VTYSH_ROUTE, h3c_display_ip_route_single_cmd_vtysh2,
        "display ip routing-table vpn-instance <1-128> {verbose}",
        "Display current system information\n"
        "IP information\n"
        "IP routing table\n"
        "Destination IP address and mask\n"
        "Destination IP address\n"
        "Specify a VPN instance\n"
        "<1-128>\n")

DEFSH (VTYSH_ROUTE, h3c_display_ip_route_active_cmd_vtysh,
        "display ip routing-table A.B.C.D (M | A.B.C.D) {verbose}",
        "Display current system information\n"
        "IP information\n"
        "IP routing table\n"
        "Destination IP address\n"
        "Mask length of the IP address\n"
        "Mask of the IP address\n"
        "Detailed information\n")

DEFSH (VTYSH_ROUTE, h3c_display_ip_route_active_cmd_vtysh2,
        "display ip routing-table {verbose}",
        "Display current system information\n"
        "IP information\n"
        "IP routing table\n"
        "Detailed information\n")

/* h3c route cmd added end */

/* h3c route if cmd add begiin */
DEFSH(VTYSH_ROUTE, h3c_display_ip_interface_brief_cmd,
    "display ip interface brief",
    "Display current system information\n"
    "IP information\n"
    "Display information of interfaces\n"
    "Brief summary of IP status and configuration\n")


DEFSH(VTYSH_ROUTE, h3c_display_ip_interface_ethernet_cmd,
    "display ip interface ethernet USP",
    "Display current system information\n"
    "IP information\n"
    "Display information of interfaces\n"
    "Display information of ethernet interfaces\n")

        
DEFSH(VTYSH_ROUTE, h3c_display_ip_interface_gigabitethernet_cmd,
    "display ip interface gigabitethernet USP",
    "Display current system information\n"
    "IP information\n"
    "Display information of interfaces\n"
    "Display information of gigabitethernet interfaces\n")


DEFSH(VTYSH_ROUTE, h3c_display_ip_interface_xgigabitethernet_cmd,
    "display ip interface xgigabitethernet USP",
    "Display current system information\n"
    "IP information\n"
    "Display information of interfaces\n"
    "Display information of xgigabitethernet interfaces\n")

DEFSH(VTYSH_ROUTE, h3c_display_ip_interface_tunnel_cmd,
    "display ip interface tunnel USP",
    "Display current system information\n"
    "IP information\n"
    "Display information of interfaces\n"
    "Display information of tunnel interfaces\n")


DEFSH(VTYSH_ROUTE, h3c_display_ip_interface_loopback_cmd,
    "display ip interface loopback <0-128>",
    "Display current system information\n"
    "IP information\n"
    "Display information of interfaces\n"
    "Display information of loopback interfaces\n")

DEFSH(VTYSH_ROUTE, h3c_display_ip_interface_vlanif_cmd,
    "display ip interface vlanif <1-4094>",
    "Display current system information\n"
    "IP information\n"
    "Display information of interfaces\n"
    "Display information of vlanif interfaces\n")


DEFSH(VTYSH_ROUTE, h3c_display_ip_interface_trunk_cmd,
    "display ip interface trunk TRUNK",
    "Display current system information\n"
    "IP information\n"
    "Display information of interfaces\n"
    "Display information of trunk interfaces\n")

                




void vtysh_init_route_cmd()
{
#define ROUTE_INSTALL_ELEMENT_CONFIG_NODE(cmd, flag) \
    install_element_level (CONFIG_NODE, (cmd), VISIT_LEVE, (flag));

#define ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(cmd, flag) \
    install_element_level (CONFIG_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(cmd, flag) \
    install_element_level (CONFIG_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define ROUTE_INSTALL_ELEMENT_CONFIG_DEBUG(cmd, flag) \
    install_element_level (CONFIG_NODE, (cmd), MANAGE_LEVE, (flag));

#define ROUTE_INSTALL_ELEMENT_ROUTE_NODE(cmd, flag) \
    install_element_level (ROUTE_NODE, (cmd), VISIT_LEVE, (flag));

#define ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(cmd, flag) \
    install_element_level (ROUTE_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define ROUTE_INSTALL_ELEMENT_ROUTE_SHOW(cmd, flag) \
    install_element_level (ROUTE_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(cmd, flag) \
    install_element_level (PHYSICAL_IF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define ROUTE_INSTALL_ELEMENT_PHY_IF_SHOW(cmd, flag) \
    install_element_level (PHYSICAL_IF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(cmd, flag) \
    install_element_level (PHYSICAL_SUBIF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define ROUTE_INSTALL_ELEMENT_PHY_SUBIF_SHOW(cmd, flag) \
    install_element_level (PHYSICAL_SUBIF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define ROUTE_INSTALL_ELEMENT_LOOP_IF_CONFIG(cmd, flag) \
    install_element_level (LOOPBACK_IF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define ROUTE_INSTALL_ELEMENT_LOOP_IF_SHOW(cmd, flag) \
    install_element_level (LOOPBACK_IF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define ROUTE_INSTALL_ELEMENT_TNL_IF_CONFIG(cmd, flag) \
    install_element_level (TUNNEL_IF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define ROUTE_INSTALL_ELEMENT_TNL_IF_SHOW(cmd, flag) \
    install_element_level (TUNNEL_IF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(cmd, flag) \
    install_element_level (TRUNK_IF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define ROUTE_INSTALL_ELEMENT_TRK_IF_SHOW(cmd, flag) \
    install_element_level (TRUNK_IF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(cmd, flag) \
    install_element_level (TRUNK_SUBIF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define ROUTE_INSTALL_ELEMENT_TRK_SUBIF_SHOW(cmd, flag) \
    install_element_level (TRUNK_SUBIF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(cmd, flag) \
    install_element_level (VLANIF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define ROUTE_INSTALL_ELEMENT_VLAN_IF_SHOW(cmd, flag) \
    install_element_level (VLANIF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(cmd, flag)\
    ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(cmd, flag) \
    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(cmd, flag) \
    ROUTE_INSTALL_ELEMENT_LOOP_IF_CONFIG(cmd, flag) \
    ROUTE_INSTALL_ELEMENT_TNL_IF_CONFIG(cmd, flag) \
    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(cmd, flag) \
    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(cmd, flag) \
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(cmd, flag)

#define ROUTE_INSTALL_ELEMENT_L3IF_SHOW(cmd, flag)\
    ROUTE_INSTALL_ELEMENT_PHY_IF_SHOW(cmd, flag) \
    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_SHOW(cmd, flag) \
    ROUTE_INSTALL_ELEMENT_LOOP_IF_SHOW(cmd, flag) \
    ROUTE_INSTALL_ELEMENT_TNL_IF_SHOW(cmd, flag) \
    ROUTE_INSTALL_ELEMENT_TRK_IF_SHOW(cmd, flag) \
    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_SHOW(cmd, flag) \
    ROUTE_INSTALL_ELEMENT_VLAN_IF_SHOW(cmd, flag)

    install_node (&route_node1, NULL);
    vtysh_install_default (ROUTE_NODE);

    ROUTE_INSTALL_ELEMENT_ROUTE_NODE(&vtysh_exit_route_cmd, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_NODE(&route_node_cmd, CMD_SYNC);

    /* show this 命令行注册 */
    ROUTE_INSTALL_ELEMENT_ROUTE_SHOW(&show_this_route_cmd_vtysh, CMD_LOCAL);

    /* 静态路由相关命令行注册 */
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&route_router_id_cmd_vtysh, CMD_SYNC);

    /* IPv4 静态路由命令行注册 */
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ip_route_static_nexthop_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ip_route_static_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ip_route_static_gigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ip_route_static_xgigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ip_route_static_vlanif_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ip_route_static_tunnel_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ip_route_static_loopback_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ip_route_static_trunk_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ip_route_static_balckhole_cmd_vtysh, CMD_SYNC);

    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ip_route_static_nexthop_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ip_route_static_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ip_route_static_gigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ip_route_static_xgigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ip_route_static_tunnel_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ip_route_static_loopback_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ip_route_static_vlanif_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ip_route_static_trunk_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ip_route_static_blackhole_cmd_vtysh, CMD_SYNC);

    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&route_ecmp_enable_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_route_ecmp_enable_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&route_frr_enable_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_route_frr_enable_cmd_vtysh, CMD_SYNC);

    ROUTE_INSTALL_ELEMENT_ROUTE_SHOW(&show_ip_route_single_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_ROUTE_SHOW(&show_ip_route_active_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_ROUTE_SHOW(&show_ip_route_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_ROUTE_SHOW(&show_ip_route_statistics_cmd_vtysh, CMD_LOCAL);

    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_route_single_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_route_active_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_route_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_route_statistics_cmd_vtysh, CMD_LOCAL);

    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_route_single_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_route_active_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_route_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_route_statistics_cmd_vtysh, CMD_LOCAL);

    /* IPv6 静态路由命令行注册 */
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ipv6_route_static_nexthop_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ipv6_route_static_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ipv6_route_static_gigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ipv6_route_static_xgigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ipv6_route_static_vlanif_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ipv6_route_static_tunnel_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ipv6_route_static_loopback_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ipv6_route_static_trunk_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&ipv6_route_static_balckhole_cmd_vtysh, CMD_SYNC);

    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&routev6_ecmp_enable_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_routev6_ecmp_enable_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&routev6_frr_enable_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_routev6_frr_enable_cmd_vtysh, CMD_SYNC);

    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ipv6_route_static_nexthop_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ipv6_route_static_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ipv6_route_static_gigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ipv6_route_static_xgigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ipv6_route_static_tunnel_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ipv6_route_static_loopback_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ipv6_route_static_vlanif_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ipv6_route_static_trunk_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_ROUTE_CONFIG(&no_ipv6_route_static_blackhole_cmd_vtysh, CMD_SYNC);

    ROUTE_INSTALL_ELEMENT_ROUTE_SHOW(&show_ipv6_route_single_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_ROUTE_SHOW(&show_ipv6_route_active_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_ROUTE_SHOW(&show_ipv6_route_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_ROUTE_SHOW(&show_ipv6_route_statistics_cmd_vtysh, CMD_LOCAL);

    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ipv6_route_single_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ipv6_route_active_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ipv6_route_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ipv6_route_statistics_cmd_vtysh, CMD_LOCAL);

    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ipv6_route_single_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ipv6_route_active_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ipv6_route_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ipv6_route_statistics_cmd_vtysh, CMD_LOCAL);

    /* 三层接口下命令行注册 */
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&ip_l3vpn_cmd_vtysh, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&no_ip_l3vpn_cmd_vtysh, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&ip_addr_cmd_vtysh, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&no_ip_addr_cmd_vtysh, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&ipv6_addr_cmd_vtysh, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&no_ipv6_addr_cmd_vtysh, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&ipv6_addr_auto_link_local_cmd_vtysh, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&ipv6_addr_auto_generate_cmd_vtysh, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&no_ipv6_addr_auto_link_local_cmd_vtysh, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&ipv6_addr_link_local_cmd_vtysh, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&no_ipv6_addr_link_local_cmd_vtysh, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_addr_cmd_vtysh, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_interface_cmd_vtysh, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_interface_brief_cmd_vtysh, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_interface_ethernet_cmd_vtysh, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_interface_gigabit_ethernet_cmd_vtysh, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_interface_xgigabit_ethernet_cmd_vtysh, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_interface_tunnel_cmd_vtysh, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_interface_loopback_cmd_vtysh, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_interface_vlanif_cmd_vtysh, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_L3IF_SHOW(&show_ip_interface_trunk_cmd_vtysh, CMD_LOCAL)

    ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(&ip_urpf_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(&no_ip_urpf_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(&ip_address_dhcp_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(&ip_address_dhcp_save_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(&ipv6_address_dhcp_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(&no_ipv6_addr_dhcp_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(&ipv6_address_dhcp_pd_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(&no_ipv6_addr_dhcp_pd_cmd_vtysh, CMD_SYNC);
	ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(&ip_addr_unnumbered_loopback_cmd_vtysh, CMD_SYNC);
//    ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(&no_ip_addr_dhcp_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(&no_ip_addr_dhcp_save_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_IF_CONFIG(&no_ip_addr_unnumbered_cmd_vtysh, CMD_SYNC);

    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&ip_urpf_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&no_ip_urpf_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&ip_address_dhcp_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&ip_address_dhcp_save_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&ipv6_address_dhcp_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&no_ipv6_addr_dhcp_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&ipv6_address_dhcp_pd_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&no_ipv6_addr_dhcp_pd_cmd_vtysh, CMD_SYNC);
	ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&ip_addr_unnumbered_loopback_cmd_vtysh, CMD_SYNC);
//    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&no_ip_addr_dhcp_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&no_ip_addr_dhcp_save_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&no_ip_addr_unnumbered_cmd_vtysh, CMD_SYNC);

    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(&ip_urpf_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(&no_ip_urpf_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(&ip_address_dhcp_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(&ip_address_dhcp_save_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(&ipv6_address_dhcp_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(&no_ipv6_addr_dhcp_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(&ipv6_address_dhcp_pd_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(&no_ipv6_addr_dhcp_pd_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(&ip_addr_unnumbered_loopback_cmd_vtysh, CMD_SYNC);
//    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(&no_ip_addr_dhcp_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(&no_ip_addr_dhcp_save_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_IF_CONFIG(&no_ip_addr_unnumbered_cmd_vtysh, CMD_SYNC);

    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&ip_urpf_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&no_ip_urpf_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&ip_address_dhcp_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&ip_address_dhcp_save_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&ipv6_address_dhcp_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&no_ipv6_addr_dhcp_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&ipv6_address_dhcp_pd_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&no_ipv6_addr_dhcp_pd_cmd_vtysh, CMD_SYNC);
	ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&ip_addr_unnumbered_loopback_cmd_vtysh, CMD_SYNC);
//    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&no_ip_addr_dhcp_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&no_ip_addr_dhcp_save_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&no_ip_addr_unnumbered_cmd_vtysh, CMD_SYNC);

    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&ip_urpf_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&no_ip_urpf_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&ip_address_dhcp_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&ip_address_dhcp_save_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&ip_addr_unnumbered_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&ipv6_address_dhcp_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&no_ipv6_addr_dhcp_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&ipv6_address_dhcp_pd_alloc_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&no_ipv6_addr_dhcp_pd_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&ip_addr_unnumbered_gigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&ip_addr_unnumbered_xgigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&ip_addr_unnumbered_tunnel_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&ip_addr_unnumbered_loopback_cmd_vtysh, CMD_SYNC);
//    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&no_ip_addr_dhcp_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&no_ip_addr_dhcp_save_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_VLAN_IF_CONFIG(&no_ip_addr_unnumbered_cmd_vtysh, CMD_SYNC);

    ROUTE_INSTALL_ELEMENT_TNL_IF_CONFIG(&ip_addr_unnumbered_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TNL_IF_CONFIG(&ip_addr_unnumbered_gigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TNL_IF_CONFIG(&ip_addr_unnumbered_xgigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TNL_IF_CONFIG(&ip_addr_unnumbered_tunnel_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TNL_IF_CONFIG(&ip_addr_unnumbered_loopback_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_TNL_IF_CONFIG(&no_ip_addr_unnumbered_cmd_vtysh, CMD_SYNC);

    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_interface_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_interface_brief_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_interface_ethernet_cmd_vtysh, CMD_LOCAL);
	ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_interface_gigabit_ethernet_cmd_vtysh, CMD_LOCAL);
	ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_interface_xgigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_interface_tunnel_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_interface_loopback_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_interface_vlanif_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&show_ip_interface_trunk_cmd_vtysh, CMD_LOCAL);

    ROUTE_INSTALL_ELEMENT_CONFIG_DEBUG(&route_debug_monitor_cmd_vtysh, CMD_LOCAL);


#if 1
/* h3c cmd register begin */
    /* static route cmd */
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&h3c_ip_route_static_nexthop_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&h3c_ip_route_static_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&h3c_ip_route_static_gigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&h3c_ip_route_static_xgigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&h3c_ip_route_static_tunnel_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&h3c_ip_route_static_loopback_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&h3c_ip_route_static_vlanif_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&h3c_ip_route_static_trunk_cmd_vtysh, CMD_SYNC);
    
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&no_h3c_ip_route_static_nexthop_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&no_h3c_ip_route_static_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&no_h3c_ip_route_static_gigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&no_h3c_ip_route_static_xgigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&no_h3c_ip_route_static_tunnel_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&no_h3c_ip_route_static_loopback_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&no_h3c_ip_route_static_vlanif_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&no_h3c_ip_route_static_trunk_cmd_vtysh, CMD_SYNC);
    
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&undo_h3c_ip_route_static_nexthop_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&undo_h3c_ip_route_static_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&undo_h3c_ip_route_static_gigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&undo_h3c_ip_route_static_xgigabit_ethernet_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&undo_h3c_ip_route_static_tunnel_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&undo_h3c_ip_route_static_loopback_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&undo_h3c_ip_route_static_vlanif_cmd_vtysh, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_CONFIG_CONFIG(&undo_h3c_ip_route_static_trunk_cmd_vtysh, CMD_SYNC);

    /* route if cmd */
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&h3c_ip_addr_config_cmd, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&undo_h3c_ip_addr_config_cmd, CMD_SYNC);
    ROUTE_INSTALL_ELEMENT_L3IF_CONFIG(&undo_h3c_ip_addr_config_ip_cmd, CMD_SYNC);


    /* ip route show */
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&h3c_display_ip_route_single_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&h3c_display_ip_route_active_cmd_vtysh, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&h3c_display_ip_route_single_cmd_vtysh2, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&h3c_display_ip_route_active_cmd_vtysh2, CMD_LOCAL);
    /* ip interface show */
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&h3c_display_ip_interface_brief_cmd, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&h3c_display_ip_interface_ethernet_cmd, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&h3c_display_ip_interface_gigabitethernet_cmd, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&h3c_display_ip_interface_xgigabitethernet_cmd, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&h3c_display_ip_interface_tunnel_cmd, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&h3c_display_ip_interface_loopback_cmd, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&h3c_display_ip_interface_vlanif_cmd, CMD_LOCAL);
    ROUTE_INSTALL_ELEMENT_CONFIG_SHOW(&h3c_display_ip_interface_trunk_cmd, CMD_LOCAL);

/* h3c cmd register end */
#endif
}


