/**
 * \page cmds_ref_dhcp DHCP
 * - \subpage modify_log_dhcp
 * - \subpage pool_name
 * - \subpage show_dhcp_interface_cmd_vtysh
 * - \subpage ip_dhcp_relay_cmd_vtysh
 * - \subpage ip_dhcp_server_cmd_vtysh
 * - \subpage show_pool_info_cmd_vtysh
 * - \subpage show_pool_address_info_cmd_vtysh
 * - \subpage gateway_list_cmd_vtysh
 * - \subpage dns_list_cmd_vtysh
 * - \subpage address_cmd_vtysh
 * - \subpage leasetime_cmd_vtysh
 * - \subpage static_bind_cmd_vtysh
 * 
 */
 
/**
 * \page modify_log_dhcp Modify Log
 * \section dhcp-v007r004 HiOS-V007R004
 *  -# 
 * \section dhcp-v007r003 HiOS-V007R003
 *  -# 
 */
 
#include <zebra.h>
#include "command.h"
#include "vtysh.h"

static struct cmd_node pool_node =
{
  POOL_NODE,
  "%s(config-ip-pool)# ",
  1,
};
  
/**
* \page pool_name ip pool dhcp <1-255>
* - 功能说明 \n
*   使用此命令创建dhcp 全局地址池，或进入已创建的地址池。
* - 命令格式 \n
*   ip pool dhcp NAME
* - 参数说明 \n
*   |参数  |说明          |
*   |----|--------------|
*   |NAME|Ip pool name|
* - 缺省情况 \n
*   无。
* - 命令模式
*   CONFIG模式
* - 用户等级
*   11
* - 使用指南
*  - 应用场景
*    使用此命令创建dhcp 全局地址池，或进入已创建的地址池。
*  - 前置条件
*    无
*  - 后续任务
*    无
*  - 注意事项
*  - 相关命令
*    no ip pool dhcp NAME  //此命令删除名字为NAME的地址池。
* - 使用举例
*  - Huahuan(config)# ip pool dhcp pool1
*  - Info: ip pool name, pool1 
*  - Huahuan(config-ip-pool)#
*/

DEFUNSH(VTYSH_DHCP,
	    pool_name,
		pool_name_cmd,
		"ip pool dhcp  <1-255>",
		"Specify IP configurations for the system\n"
		"Configure a IP pool or enter the IP pool view\n"
		"Dynamic host configure protocol\n"
		"Pool index,<1-255>\n")
{
	vty->node = POOL_NODE;	
	return CMD_SUCCESS;
}
DEFUNSH (VTYSH_DHCP,
	vtysh_exit_pool,
	vtysh_exit_pool_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}

ALIAS (vtysh_exit_pool,
	vtysh_quit_pool_cmd,
	"quit",
	"Exit current mode and down to previous mode\n")


/*DEFSH (VTYSH_DHCP,
       ip_address_dhcp_alloc_cmd_vtysh,
       "ip address dhcp",
       "Specify IP configurations for interfaces\n"
       "Address\n"
       "IP address allocated by DHCP\n")

DEFSH (VTYSH_DHCP,
       no_ip_address_dhcp_alloc_cmd_vtysh,
       "no ip address dhcp",
       NO_STR
       "Specify IP configurations for interfaces\n"
       "Address\n"
       "IP address allocated by DHCP\n")*/
/**
* \page show_dhcp_interface_cmd_vtysh show ip dhcp (client|relay|server)
* - 功能说明 \n
*	使用此命令显示(client|relay|server)信息。
* - 命令格式 \n
*	show ip dhcp (client|relay|server)
* - 参数说明 \n
*	|参数 	|说明 	  |
*	|-------|--------------|
* - 缺省情况 \n
*	无。
* - 命令模式
*	config模式
* - 用户等级
*	11
* - 使用指南
*  - 应用场景
*	 使用此命令显示(client|relay|server)信息。
*  - 前置条件
*	 无
*  - 后续任务
*	 无
*  - 注意事项
*	 无
*  - 相关命令
*    无
* - 使用举例
*  - Huahuan(config)#show ip dhcp server 
*  - DHCP server information on interface  ethernet 0/0/1
*  - Ip address      :192.168.30.1
*  - Huahuan(config)#
*  - Huahuan(config)# show ip dhcp client
*  - DHCP client lease information on interface  ethernet 0/0/3
*  - Current machine state         : BOUND
*  - Internet address assigned via	   : DHCP
*  - IP address                   : 192.192.40.100
*  - Subnet mask                 : 255.255.255.0
*  - DHCP server                 : 192.192.40.60
*  - Huahuan(config)#
*  - Huahuan(config)# show ip dhcp relay 
*  - DHCP relay information on interface  ethernet 0/0/2
*  - Nexthop   : 192.168.30.1
*  - Huahuan(config)#
*/
DEFSH (VTYSH_DHCP, 														
		show_dhcp_interface_cmd_vtysh,													 
	"show ip dhcp (client|relay|server)",														
	SHOW_STR	 
	"Specify IP configurations for interfaces\n"
	"Dynamic host configure protocol\n"
	"Dhcp interface client information\n" 
	"Dhcp interface relay information\n" 
	"Dhcp interface server information\n" )


/*DEFSH (VTYSH_DHCP,
	   dhcp_select_cmd_vtysh,
	   "dhcp select (global|interface|relay)",
	   "Dynamic host configure protocol\n"
	   "Select\n"
	   "Local server\n"
	   "Interface server pool\n"
	   "DHCP relay\n"
		)*/
/**
* \page ip_dhcp_relay_cmd_vtysh ip dhcp relay nexthop A.B.C.D
* - 功能说明 \n
*	使用此命令使能接口的dhc relay功能并配置下一跳地址
* - 命令格式 \n
*	ip dhcp relay nexthop A.B.C.D
* - 参数说明 \n
*	|参数     |说明		  |
*	|-------|--------------|
*	|A.B.C.D|下一跳IP地址，IP是点分十进制形式 如：192.168.200.1|
* - 缺省情况 \n
*	无。
* - 命令模式
*	接口视图
* - 用户等级
*	11
* - 使用指南
*  - 应用场景
*	 使用此命令使能接口的dhc relay功能并配置下一跳地址。
*  - 前置条件
*	 接口为L3模式
*  - 后续任务
*	 无
*  - 注意事项
*	 接口需要配置静态ip地址使用举例
*  - 相关命令
*	 no ip dhcp (server|relay)  //使用此命令禁止接口的dhcp服务器或中继功能
* - 使用举例
*  - Huahuan(config-if)#  ip dhcp relay nexthop 192.168.30.1
*  - Info: ifindex 10001000,  set dhcp relay ok
*  - Huahuan(config-if)#
*/

DEFSH (VTYSH_DHCP,
	   ip_dhcp_relay_cmd_vtysh,
	   "ip dhcp relay nexthop A.B.C.D",
	   "Specify IP configurations for interfaces\n" 	  
	   "Dynamic host configure protocol\n"
	   "Interface as a  dhcp relay\n"
	   "Next relay address or server address\n"    
	   "IP address\n"
		)
/**
* \page ip_dhcp_server_cmd_vtysh ip dhcp server
* - 功能说明 \n
*	使用此命令使能接口的dhcp服务器功能
* - 命令格式 \n
*	ip dhcp server
* - 参数说明 \n
*	|参数 	|说明 	  |
*	|-------|--------------|
* - 缺省情况 \n
*	无。
* - 命令模式
*	接口视图
* - 用户等级
*	11
* - 使用指南
*  - 应用场景
*	使用此命令使能接口的dhcp服务器功能。
*  - 前置条件
*	 接口为L3模式
*  - 后续任务
*	 无
*  - 注意事项
*	 接口需要配置静态ip地址使用举例
*  - 相关命令
*	 no ip dhcp (server|relay)  //使用此命令禁止接口的dhcp服务器或中继功能
* - 使用举例
*  - Huahuan(config-if)#  ip dhcp server
*  - Info: ifindex 10001000,  set dhcp server ok
*  - Huahuan(config-if)#
*/
DEFSH (VTYSH_DHCP,
       ip_dhcp_server_cmd_vtysh,
       "ip dhcp server",
	   "Specify IP configurations for interfaces\n"       
       "Dynamic host configure protocol\n"
	   "Interface as a  dhcp server\n"
		)	

DEFSH (VTYSH_DHCP,
       no_ip_dhcp_server_cmd_vtysh,
		"no ip dhcp (server|relay)",
		 NO_STR 	  
		"Specify IP configurations for interfaces\n"	   
		"Dynamic host configure protocol\n"
		"Interface as a  dhcp server\n"
		"Interface as a  dhcp relay\n"		
		)
		
		
/*DEFSH (VTYSH_DHCP,
	   no_dhcp_select_cmd_vtysh,
	   "no dhcp select (global|interface|relay)",
	   NO_STR
	   "Dynamic host configure protocol\n"
	   "Select\n"
	   "Local server\n"
	   "Interface server pool\n"
	   "DHCP relay\n"
		)*/
		
DEFSH(VTYSH_DHCP,
		no_pool_name_cmd_vtysh,
		"no ip pool dhcp <1-255>",
		NO_STR
		"Specify IP configurations for the system\n"
		"Configure a IP pool or enter the IP pool view\n"
		"Dynamic host configure protocol\n" 	
		"Pool index\n")

/*DEFSH (VTYSH_DHCP,
       dhcp_enable_cmd_vtysh,
       "dhcp enable",
       "Dynamic host configure protocol\n"
       "Enable\n")


DEFSH (VTYSH_DHCP,
       no_dhcp_enable_cmd_vtysh,
       "no dhcp enable",
		NO_STR       
       "Dynamic host configure protocol\n"
       "Enable\n")*/

/**
* \page show_pool_info_cmd_vtysh show ip pool {index <1-255>}
* - 功能说明 \n
*	使用此命令显示dhcp地址池信息。
* - 命令格式 \n
*	show ip pool {index <1-255>}
* - 参数说明 \n
*	|参数 	|说明 	  |
*	|-------|--------------|
*	|NAME   |指定地址池的名字，仅显示该地址池的信息|
* - 缺省情况 \n
*	无。
* - 命令模式
*	config模式
* - 用户等级
*	11
* - 使用指南
*  - 应用场景
*	 使用此命令显示dhcp地址池信息。
*  - 前置条件
*	 无
*  - 后续任务
*	 无
*  - 注意事项
*	 无
* - 使用举例
*  - Huahuan(config)#show ip pool
*  - Pool-name      : pool1
*  - Gateway        : 192.168.60.2/24 
*  - Address-num    : 5 
*  - Start-address  : 192.168.60.5 
*  - End-address    : 192.168.60.9 
*  - Lease          : 10 minites
*  - DNS-server     : --  
*  - Huahuan(config)#
*/
DEFSH(VTYSH_DHCP,
	show_pool_info_cmd_vtysh,
	"show ip pool {index <1-255>}",
	SHOW_STR
	"Specify IP configurations for the system\n"
	"Configure a IP pool or enter the IP pool view\n"
	"index\n"
	"Pool index\n")
/**
* \page show_pool_address_info_cmd_vtysh show ip pool address {index <1-255>}
* - 功能说明 \n
*	使用此命令显示dhcp地址池地址信息
* - 命令格式 \n
*	show ip pool address {index <1-255>}
* - 参数说明 \n
*	|参数 	|说明 	  |
*	|-------|--------------|
*	|NAME	|指定地址池的名字，仅显示该地址池的信息|
* - 缺省情况 \n
*	无。
* - 命令模式
*	config模式
* - 用户等级
*	11
* - 使用指南
*  - 应用场景
*	 使用此命令显示dhcp地址池信息。
*  - 前置条件
*	 无
*  - 后续任务
*	 无
*  - 注意事项
*	 无
* - 使用举例
*  - Huahuan(config)#show ip pool
*  - Pool-name      : pool1
*  - Address-num    : 5                    //pool total num
*  - Static-num     : 1 
*  - Dynamic-num  : 1 
*  - Expired-num    : 1 
*  - remaining-num  : 4 
*  - Static-address : 
*  - Static-bind     : ip 192.168.60.9   , mac 00:1d:80:00:00:11 
*  - OfferedAddr    : ip 192.168.60.5   , mac 00:44:80:01:93:13, remaining time 0 minites      --  
*  - Huahuan(config)#
*/
DEFSH(VTYSH_DHCP,
	show_pool_address_info_cmd_vtysh,
	"show ip pool address {index <1-255>}",
	SHOW_STR
	"Specify IP configurations for the system\n"
	"Configure a IP pool or enter the IP pool view\n"	
	"ip address\n"
	"index\n"
	"Pool index\n")

/**
* \page gateway_list_cmd_vtysh gateway A.B.C.D/<1-30>
* - 功能说明 \n
*	使用此命令配置地址池网关网段地址
* - 命令格式 \n
*	 gateway  A.B.C.D/<1-31>
* - 参数说明 \n
*	|参数 	       |说明 	  |
*	|--------------|--------------|
*	|A.B.C.D/<1-31>|IP地址和掩码长度，IP是点分十进制形式，掩码长度是大于等于1小于等于32的数字。如：192.168.20.1/24|
* - 缺省情况 \n
*	无。
* - 命令模式
*	config-ip-pool模式
* - 用户等级
*	11
* - 使用指南
*  - 应用场景
*	 使用此命令创建地址池网关网段地址。
*  - 前置条件
*	 Ip pool已配置
*  - 后续任务
*	无
*  - 注意事项
*	 address A.B.C.D NUM配置的地址段要是gateway  A.B.C.D/<1-31>的子集
*  - 相关命令
*	 no gateway //此命令删除已配置的网关网段
* - 使用举例
*  - Huahuan(config-ip-pool)# 
*  - Huahuan(config-ip-pool)# gateway  192.168.20.1/24
*  - Info: gateway is 192.168.20.1 ,mask_len is 24
*  - Huahuan(config-ip-pool)#	
*/
DEFSH(VTYSH_DHCP,
	gateway_list_cmd_vtysh,
	"gateway A.B.C.D/<1-30>",
	"Configure the gateway\n"
	"Gateway's IP address/mask_len\n")

DEFSH(VTYSH_DHCP,
	no_gateway_list_cmd_vtysh,
	//"no gateway A.B.C.D/<1-32>",	
	"no gateway",		
	NO_STR
	"Configure the gateway\n")
	//"Gateway's IP address/mask_len\n"


/**
* \page dns_list_cmd_vtysh dns-server A.B.C.D
* - 功能说明 \n
*	使用此命令配置地址池dns服务器地址。
* - 命令格式 \n
*	dns-server A.B.C.D
* - 参数说明 \n
*	|参数     |说明	  |
*	|-------|--------------|
*	|A.B.C.D|IP地址，IP是点分十进制形式 如：172.16.95.10|
* - 缺省情况 \n
*	无。
* - 命令模式
*	config-ip-pool模式
* - 用户等级
*	11
* - 使用指南
*  - 应用场景
*	 使用此命令配置地址池dns服务器地址。
*  - 前置条件
*	 Ip pool已配置
*  - 后续任务
*	无
*  - 注意事项
*	 address A.B.C.D NUM配置的地址段要是gateway	A.B.C.D/<1-31>的子集
*  - 相关命令
*	 no dns-server  A.B.C.D  //此命令删除已配置的dns服务器。
* - 使用举例
*  - Huahuan(config-ip-pool)# 
*  - Huahuan(config-ip-pool)# dns-server 172.16.95.10
*  - Info: dns is 172.16.95.10
*  - Huahuan(config-ip-pool)#	
*/
DEFSH(VTYSH_DHCP,
	dns_list_cmd_vtysh,
	"dns-server A.B.C.D",
	"Configure DNS servers\n"
	"IP address\n")


DEFSH(VTYSH_DHCP,
	no_dns_list_cmd_vtysh,
	"no dns-server A.B.C.D",
	NO_STR
	"Delete primary and secondary DNS's IP address\n"
	"IP address\n")


/**
* \page address_cmd_vtysh address  A.B.C.D  <1-65535>
* - 功能说明 \n
*	使用此命令配置地址池起始IP地址和数目。
* - 命令格式 \n
*	address  A.B.C.D  <1-65535>
* - 参数说明 \n
*	|参数 	  |说明   |
*	|---------|--------------|
*	|A.B.C.D  |IP地址，IP是点分十进制形式 如：172.16.95.10|
*	|<1-65535>|IP地址数目|
* - 缺省情况 \n
*	无。
* - 命令模式
*	config-ip-pool模式
* - 用户等级
*	11
* - 使用指南
*  - 应用场景
*	 使用此命令配置地址池起始IP地址和数目。
*  - 前置条件
*	 Ip pool已配置
*  - 后续任务
*	无
*  - 注意事项
*	 address A.B.C.D  <1-65535> 配置的地址段要是gateway  A.B.C.D/<1-31>的子集
*  - 相关命令
*	 no address   //此命令删除起始ip地址和数目
* - 使用举例
*  - Huahuan(config-ip-pool)# 
*  - Huahuan(config-ip-pool)# address 192.168.20.1 10
*  - Info: start address 192.168.20.1, end address 192.168.20.10
*  - Huahuan(config-ip-pool)#	
*/
DEFSH(VTYSH_DHCP,
	address_cmd_vtysh,
	"address A.B.C.D <1-65535>",
	"Add a network\n"
	"Start ip address\n"
	"Ip number")

DEFSH(VTYSH_DHCP,
	no_address_cmd_vtysh,
	"no address",
	NO_STR
	"Delete network section\n")

/**
* \page leasetime_cmd_vtysh lease <0-65535>
* - 功能说明 \n
*	使用此命令配置地址池租约期限。
* - 命令格式 \n
*	lease <0-65535>
* - 参数说明 \n
*	|参数 	  |说明	|
*	|---------|--------------|
*	|<0-65535>|分钟，0为无限期|
* - 缺省情况 \n
*	缺省为1440分钟,  即1day。
* - 命令模式
*	config-ip-pool模式
* - 用户等级
*	11
* - 使用指南
*  - 应用场景
*	 使用此命令配置地址池租约期限。
*  - 前置条件
*	 Ip pool已配置
*  - 后续任务
*	 无
*  - 注意事项
*	 无
*  - 相关命令
*	 no lease  //使用此命令恢复lease的默认值为1440分钟，即 1day
* - 使用举例
*  - Huahuan(config-ip-pool)# 
*  - Huahuan(config-ip-pool)#lease 60
*  - Info: Lease time  is 60 minutes
*  - Huahuan(config-ip-pool)#	
*/
DEFSH(VTYSH_DHCP,
	leasetime_cmd_vtysh,
	"lease <0-65535>",
	"Configure the lease of the IP pool\n"
	"Minute, from 0 to 65535\n")

DEFSH(VTYSH_DHCP,
	no_leasetime_cmd_vtysh,
	"no lease",	
	NO_STR
	"Restore the default lease\n")


/**
* \page static_bind_cmd_vtysh static-bind ip A.B.C.D mac XX:XX:XX:XX:XX:XX
* - 功能说明 \n
*	使用此命令配置地址池中静态IP地址与mac绑定。
* - 命令格式 \n
*	static-bind  ip  A.B.C.D  mac  XX:XX:XX:XX:XX:XX
* - 参数说明 \n
*	|参数 	          |说明	|
*	|-----------------|--------------|
*	|A.B.C.D          |IP地址，IP是点分十进制形式 如：192.168.20.2|
*	|XX:XX:XX:XX:XX:XX|Mac地址，如00:1d:80:10:10:10|
* - 缺省情况 \n
*	无。
* - 命令模式
*	config-ip-pool模式
* - 用户等级
*	11
* - 使用指南
*  - 应用场景
*	 使用此命令配置地址池起始IP地址和数目。
*  - 前置条件
*	 Ip pool已配置,  Address A.B.C.D <1-65535>已配置
*  - 后续任务
*	 无
*  - 注意事项
*	 无
*  - 相关命令
*	 no static-bind ip A.B.C.D  //此命令匹配删除静态ip地址相关条目。
* - 使用举例
*  - Huahuan(config-ip-pool)# 
*  - Huahuan(config-ip-pool)# static-bind  ip 192.168.20.2 mac 00:1d:80:10:10:10
*  - Info: static-bind,ip[192.168.20.2] ,mac[00:1D:80:10:10:10] 
*  - Huahuan(config-ip-pool)#	
*/
DEFSH(VTYSH_DHCP,
	static_bind_cmd_vtysh,
	"static-bind ip A.B.C.D mac XX:XX:XX:XX:XX:XX",
	"Static bind\n"
	"IP address for static bind\n"
	"IP address\n"
	"MAC address for static bind\n"
	"MAC address\n")


DEFSH(VTYSH_DHCP,
	no_static_bind_cmd_ip_vtysh,
	"no static-bind ip A.B.C.D ",
	NO_STR
	"Static bind\n"
	"IP address for static bind\n"
	"IP address\n")

/**
* \page dhcp_option60_cmd_vtysh dhcp option60 STRING
* - 功能说明 \n
*	全局配置dhcp option60。
* - 命令格式 \n
*	dhcp option60 STRING
* - 参数说明 \n
*	|参数    |说明	|
*	|------|--------------|
*	|STRING|Dhcp option60字符串|
* - 缺省情况 \n
*	无。
* - 命令模式
*	config模式
* - 用户等级
*	11
* - 使用指南
*  - 应用场景
*	 使用此命令全局配置dhcp option60。
*  - 前置条件
*	 无
*  - 后续任务
*	 无
*  - 注意事项
*	 无
*  - 相关命令
*	 show dhcp option60	//显示dhcp option60字段。
* - 使用举例
*  - Huahuan(config-ip-pool)# 
*  - Huahuan(config-ip-pool)# dhcp option60 99660000
*  - INFO: Info :set dhcp option60 is success!
*  - Huahuan(config-ip-pool)#	
*/
DEFSH(VTYSH_DHCP,
	dhcp_option60_cmd_vtysh,
	"dhcp option60 STRING",
	"Dynamic host configure protocol\n"
	"dhcp option60\n"
	"vendor identification\n")


DEFSH(VTYSH_DHCP,
	dhcp_option60_show_cmd_vtysh,
	"show dhcp option60",
	SHOW_STR
	"Dynamic host configure protocol\n"
	"dhcp option60vendor identification\n")

DEFSH (VTYSH_DHCP | VTYSH_ROUTE, 
	no_ip_address_dhcp_cmd_vtysh,
	"no ip address dhcp",
	"Delete command\n"
	"IP command\n"
	"Address\n"
	"IP address allocated by DHCP\n")
	
DEFSH(VTYSH_DHCP,
	ip_dhcp_zero_cmd_vtysh,
	"ip address dhcp zero",
	"Specify IP configurations for interfaces\n" 
	"Address\n"
	"Dynamic host configure protocol\n"
	"This interface is null\n")

DEFSH(VTYSH_DHCP,
	no_ip_dhcp_zero_cmd_vtysh,
	"no ip address dhcp zero",
	"disable dhcp zero\n"
	"Specify IP configurations for interfaces\n" 
	"Address\n"
	"Dynamic host configure protocol\n"
	"This interface is null\n")

DEFSH(VTYSH_DHCP,
	dhcp_conf_debug_cmd_vtysh,
	"debug dhcp (enable|disable) (all|client|relay|server)",
	"Debug config\n"
	"dhcp config\n"
	"dhcp debug enable\n"
	"dhcp debug disable\n"
	"dhcp debug type all\n"
	"dhcp debug type client\n"
	"dhcp debug type relay\n"
	"dhcp debug type server\n")

void
vtysh_init_dhcp_cmd ()
{
  	install_node(&pool_node, NULL);
	vtysh_install_default(POOL_NODE);
	install_element_level(POOL_NODE, &vtysh_exit_pool_cmd, VISIT_LEVE, CMD_SYNC);

	install_element_level(CONFIG_NODE, &pool_name_cmd, VISIT_LEVE, CMD_SYNC); 

	install_element_level(PHYSICAL_IF_NODE,&ip_dhcp_relay_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level(PHYSICAL_IF_NODE,&ip_dhcp_server_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE,&no_ip_dhcp_server_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_daemon_order_level(PHYSICAL_IF_NODE, &no_ip_address_dhcp_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC, 2, VTYSH_DHCP, VTYSH_ROUTE );
	install_element_level(PHYSICAL_IF_NODE,&ip_dhcp_zero_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE,&no_ip_dhcp_zero_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level(PHYSICAL_SUBIF_NODE,&ip_dhcp_relay_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level(PHYSICAL_SUBIF_NODE,&ip_dhcp_server_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE,&no_ip_dhcp_server_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_daemon_order_level(PHYSICAL_SUBIF_NODE, &no_ip_address_dhcp_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC, 2, VTYSH_DHCP, VTYSH_ROUTE );
	install_element_level(PHYSICAL_SUBIF_NODE,&ip_dhcp_zero_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE,&no_ip_dhcp_zero_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level(TRUNK_IF_NODE,&ip_dhcp_relay_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level(TRUNK_IF_NODE,&ip_dhcp_server_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE,&no_ip_dhcp_server_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_daemon_order_level(TRUNK_IF_NODE, &no_ip_address_dhcp_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC, 2, VTYSH_DHCP, VTYSH_ROUTE );
	install_element_level(TRUNK_IF_NODE,&ip_dhcp_zero_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_IF_NODE,&no_ip_dhcp_zero_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level(TRUNK_SUBIF_NODE,&ip_dhcp_relay_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level(TRUNK_SUBIF_NODE,&ip_dhcp_server_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE,&no_ip_dhcp_server_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_daemon_order_level(TRUNK_SUBIF_NODE, &no_ip_address_dhcp_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC, 2, VTYSH_DHCP, VTYSH_ROUTE );
	install_element_level(TRUNK_SUBIF_NODE,&ip_dhcp_zero_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(TRUNK_SUBIF_NODE,&no_ip_dhcp_zero_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


	install_element_level(VLANIF_NODE,&ip_dhcp_relay_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level(VLANIF_NODE,&ip_dhcp_server_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE,&no_ip_dhcp_server_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_daemon_order_level(VLANIF_NODE, &no_ip_address_dhcp_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC, 2, VTYSH_DHCP, VTYSH_ROUTE );
	install_element_level(VLANIF_NODE,&ip_dhcp_zero_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(VLANIF_NODE,&no_ip_dhcp_zero_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (CONFIG_NODE, &show_dhcp_interface_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &dhcp_option60_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &dhcp_option60_show_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &dhcp_conf_debug_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (CONFIG_NODE, &no_pool_name_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (POOL_NODE, &gateway_list_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC); 
	install_element_level (POOL_NODE, &dns_list_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC); 
	install_element_level (POOL_NODE, &address_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC); 
	install_element_level (POOL_NODE, &static_bind_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (POOL_NODE, &leasetime_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);	

	install_element_level (POOL_NODE, &no_gateway_list_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (POOL_NODE, &no_dns_list_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (POOL_NODE, &no_address_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (POOL_NODE, &no_static_bind_cmd_ip_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (POOL_NODE, &no_leasetime_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC); 
	install_element_level (CONFIG_NODE, &show_pool_info_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_pool_address_info_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
}

