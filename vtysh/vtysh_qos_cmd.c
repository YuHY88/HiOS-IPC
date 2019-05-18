/**
 * \page cmds_ref_qos QoS
 * - \subpage modify_log_qos
 * - \subpage qos_mapping_phb_cmd
 * - \subpage no_qos_mapping_domain_cmd_vtysh
 * - \subpage no_qos_mapping_phb_cmd_vtysh
 * - \subpage qos_car
 * - \subpage acl_num
 * - \subpage no_acl_num_cmd_vtysh
 * - \subpage acl_name_cmd_vtysh
 * - \subpage qos_mapping_domain_cmd
 * - \subpage no_acl_name_cmd_vtysh
 * - \subpage acl_ip_extend_rule_cmd_vtysh
 * - \subpage acl_ip_extend_2_rule_cmd_vtysh
 * - \subpage acl_ip_extend_3_rule_cmd_vtysh
 * - \subpage acl_mix_rule_cmd_vtysh
 * - \subpage acl_ip_rule_cmd_vtysh
 * - \subpage no_acl_ip_rule_cmd_vtysh
 * - \subpage acl_mac_rule_cmd_vtysh
 * - \subpage show_acl_info_cmd_vtysh
 * - \subpage show_acl_rule_info_cmd_vtysh
 * - \subpage qos_cos_to_queue_cmd_vtysh
 * - \subpage no_qos_cos_to_queue_cmd_vtysh
 * - \subpage qos_tos_to_queue_cmd_vtysh
 * - \subpage no_qos_tos_to_queue_cmd_vtysh
 * - \subpage qos_exp_to_queue_cmd_vtysh
 * - \subpage no_qos_exp_to_queue_cmd_vtysh
 * - \subpage qos_dscp_to_queue_cmd_vtysh
 * - \subpage no_qos_dscp_to_queue_cmd_vtysh
 * - \subpage show_qos_mapping_domain_cmd_vtysh
 * - \subpage show_qos_mapping_configure_cmd_vtysh
 * - \subpage qos_queue_to_cos_cmd_vtysh
 * - \subpage no_qos_queue_to_cos_cmd_vtysh
 * - \subpage qos_queue_to_exp_cmd_vtysh
 * - \subpage no_qos_queue_to_exp_cmd_vtysh
 * - \subpage qos_queue_to_tos_cmd_vtysh
 * - \subpage no_qos_queue_to_tos_cmd_vtysh
 * - \subpage qos_queue_to_dscp_cmd_vtysh
 * - \subpage no_qos_queue_to_dscp_cmd_vtysh
 * - \subpage show_qos_mapping_phb_cmd_vtysh
 * - \subpage qos_policy_acl_filter_cmd_vtysh
 * - \subpage qos_policy_egress_acl_filter_cmd_vtysh
 * - \subpage qos_policy_acl_car_cmd_vtysh
 * - \subpage qos_policy_egress_acl_car_cmd_vtysh
 * - \subpage qos_policy_acl_mirror_eth_cmd_vtysh
 * - \subpage qos_policy_acl_mirror_gigabit_eth_cmd_vtysh
 * - \subpage qos_policy_acl_mirror_xgigabit_eth_cmd_vtysh
 * - \subpage qos_policy_acl_mirror_trunk_cmd_vtysh
 * - \subpage qos_policy_acl_redirect_eth_cmd_vtysh
 * - \subpage qos_policy_acl_redirect_gigabit_eth_cmd_vtysh
 * - \subpage qos_policy_acl_redirect_xgigabit_eth_cmd_vtysh
 * - \subpage qos_policy_acl_redirect_trunk_cmd_vtysh
 * - \subpage qos_policy_acl_queue_cmd_vtysh
 * - \subpage no_qos_policy_ingress_acl_cmd_vtysh
 * - \subpage no_qos_policy_egress_acl_cmd_vtysh
 * - \subpage show_qos_policy_acl_cmd_vtysh
 * - \subpage no_qos_car_cmd_vtysh
 * - \subpage qos_car_rate_limit_cmd_vtysh
 * - \subpage qos_car_color_action_cmd_vtysh
 * - \subpage qos_car_color_mode_cmd_vtysh
 * - \subpage no_qos_car_color_action_cmd_vtysh
 * - \subpage show_qos_car_cmd_vtysh
 * - \subpage qos_car_apply_to_if_cmd_vtysh
 * - \subpage no_qos_car_apply_to_if_cmd_vtysh
 * - \subpage qos_lr_config_if_cmd_vtysh
 * - \subpage qos_car_config_if_cmd_vtysh
 * - \subpage no_qos_lr_config_if_cmd_vtysh
 * - \subpage qos_mirror_group_cmd
 * - \subpage no_qos_mirror_group_cmd_vtysh
 * - \subpage qos_mirror_to_group_cmd_vtysh
 * - \subpage no_qos_mirror_to_group_cmd_vtysh
 * - \subpage qos_mirror_to_ethernet_cmd_vtysh
 * - \subpage qos_mirror_to_gigabit_ethernet_cmd_vtysh
 * - \subpage qos_mirror_to_xgigabit_ethernet_cmd_vtysh
 * - \subpage qos_mirror_to_trunk_cmd_vtysh
 * - \subpage qos_mirror_to_loopback_cmd_vtysh
 * - \subpage qos_mirror_if_to_ethernet_cmd_vtysh
 * - \subpage qos_mirror_if_to_gigabit_ethernet_cmd_vtysh
 * - \subpage qos_mirror_if_to_xgigabit_ethernet_cmd_vtysh
 * - \subpage qos_mirror_if_to_ethernet_egress_cmd_vtysh
 * - \subpage qos_mirror_if_to_gigabit_egress_ethernet_cmd_vtysh
 * - \subpage qos_mirror_if_to_xgigabit_egress_ethernet_cmd_vtysh
 * - \subpage qos_mirror_if_to_trunk_cmd_vtysh
 * - \subpage qos_mirror_if_to_loopback_cmd_vtysh
 * - \subpage no_qos_mirror_to_ethernet_cmd_vtysh
 * - \subpage no_qos_mirror_to_gigabit_ethernet_cmd_vtysh
 * - \subpage no_qos_mirror_to_xgigabit_ethernet_cmd_vtysh
 * - \subpage no_qos_mirror_to_trunk_cmd_vtysh
 * - \subpage no_qos_mirror_to_loopback_cmd_vtysh
 * - \subpage show_qos_mirror_group_cmd_vtysh
 * - \subpage show_qos_mirror_if_cmd_vtysh
 *
 */

/**
 * \page modify_log_qos Modify Log
 * \section qos-v007r004 HiOS-V007R004
 *  -#
 * \section qos-v007r003 HiOS-V007R003
 *  -#
 */
#include <zebra.h>
#include <qos/qos_mapping_cmd.h>
#include <qos/qos_mapping.h>
#include "command.h"
#include "vtysh.h"
#include "ifm_common.h"


extern void vtysh_init_qos_cmd (void);

/**
 * \page acl_ip_extend_3_rule_cmd_vtysh rule <1-65535> ip-extended protocol tcp {sport <1-65535>|dport <1-65535>|sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>} tcp-flag {fin|syn|rst|psh|ack|urg}
 * - 功能说明 \n
 *	 配置扩展 IP acl rule
 * - 命令格式 \n
 *	 rule <1-65535> ip-extended protocol tcp {sport <1-65535>|dport <1-65535>|sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>} tcp-flag {fin|syn|rst|psh|ack|urg}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|Acl ruleid|
 *   |sport <1-65535>|Sport id|
 *   |dport <1-65535>|Dport id|
 *	 |sip A.B.C.D/M|Sip format|
 *	 |dip A.B.C.D/M|Dip format|
 *	 |l3vpn<1-128>|vpnid|
 *	 |dscp<0-63>|Dscp value|
 *	 |tos<0-7>|Tos value|
 *	 |ttl<1-255>|Ttl value|
 *   |(fin\|syn\|rst\|psh\|ack\|urg)|Tcp type|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 ACL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于ACL配置模式下，配置基本 IP acl rule
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show acl [<1-65535>]
 * - 使用举例 \n
 *	   无
 *
 */

/**
 * \page acl_ip_extend_2_rule_cmd_vtysh rule <1-65535> ip-extended protocol (udp|tcp) {sport <1-65535>|dport <1-65535>|sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>}
 * - 功能说明 \n
 *	 配置扩展 IP acl rule
 * - 命令格式 \n
 *	 rule <1-65535> ip-extended protocol (udp|tcp) {sport <1-65535>|dport <1-65535>|sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|Acl ruleid|
 *	 |(udp\|tcp)|Protocol type|
 *   |sport <1-65535>|Sport id|
 *   |dport <1-65535>|Dport id|
 *	 |sip A.B.C.D/M|Sip format|
 *	 |dip A.B.C.D/M|Dip format|
 *	 |l3vpn<1-128>|vpnid|
 *	 |dscp<0-63>|Dscp value|
 *	 |tos<0-7>|Tos value|
 *	 |ttl<1-255>|Ttl value|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 ACL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于ACL配置模式下，配置基本 IP acl rule
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show acl [<1-65535>]
 * - 使用举例 \n
 *	   无
 *
 */

/**
 * \page acl_mix_rule_cmd_vtysh rule <1-65535> mix {ethtype HHHH|smac XX:XX:XX:XX:XX:XX | dmac XX:XX:XX:XX:XX:XX | vlan <1-4095> |cos<0-7>|cvlan <1-4095> | cvlan-cos <0-7>|protocol HH|sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>}
 * - 功能说明 \n
 *	 配置mix acl rule
 * - 命令格式 \n
 *	 rule <1-65535> mix {ethtype HHHH|smac XX:XX:XX:XX:XX:XX | dmac XX:XX:XX:XX:XX:XX | vlan <1-4095> |cos<0-7>|cvlan <1-4095> | cvlan-cos <0-7>|protocol HH|sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|Acl ruleid|
 *	 |ethtype HHHH|Ethernet type in the range <0x0600-0xffff>|
 *   |smac XX:XX:XX:XX:XX:XX|Smac format|
 *   |dmac XX:XX:XX:XX:XX:XX|Dmac format|
 *   |vlan <1-4095>|cvlan|
 *   |cvlan-cos <0-7>|Cvlan_cos|
 *   |protocol HH|Protocol type,HH 范围<0x00-0xff>|
 *	 |sip A.B.C.D/M|Sip format|
 *	 |dip A.B.C.D/M|Dip format|
 *	 |l3vpn<1-128>|vpnid|
 *	 |dscp<0-63>|Dscp value|
 *	 |tos<0-7>|Tos value|
 *	 |ttl<1-255>|Ttl value|
 *
 * - 缺省情况 \n
 *	 缺省情况下,未配置
 * - 命令模式 \n
 *	 ACL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于ACL模式下，用于配置mac-ip mix acl rule
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

/**
 * \page acl_ip_rule_cmd_vtysh rule <1-65535> ip (sip|dip) (A.B.C.D/M|any) [l3vpn <1-128>]
 * - 功能说明 \n
 *	 配置基本 IP acl rule
 * - 命令格式 \n
 *	 rule<1-65535> ip (sip |dip) ( A.B.C.D/M|any) [l3vpn <1-128>]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|Acl ruleid|
 *   |sip\|dip|source ip\|destination ip|
 *   |A.B.C.D/M|sip format|
 *   |<1-128>|vpnid|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 ACL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于ACL配置模式下，配置基本 IP acl rule
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   Dip与sip不同时配，配置any时匹配所有ip；vpnid范围<0-1024>,不配即不下发VPN
 *	 - 相关命令 \n
 *	   show acl [<1-65535>]
 * - 使用举例 \n
 *	   Huahuan(config)# acl 2
 *	   Huahuan(config-acl)# rule 5 ip dip 1.2.3.4/24
 *
 */

/**
 * \page no_acl_ip_rule_cmd_vtysh no rule <1-65535>
 * - 功能说明 \n
 *	 删除一条 rule
 * - 命令格式 \n
 *	 no rule <1-65535>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|ruleid|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 ACL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用在ACL模式下，使用no rule <1-65535>删除对应acl group rule
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show acl <1-65535> {rule <1-65535>}
 * - 使用举例 \n
 *	   Huahuan(config)# acl 2
 *	   Huahuan(config-acl)# rule 5 ip dip 1.2.3.4/24
 *	   Huahuan(config)# no acl 5
 *
 */

/**
 * \page acl_mac_rule_cmd_vtysh rule <1-65535> mac ethtype (HHHH|any){smac XX:XX:XX:XX:XX:XX|dmac XX:XX:XX:XX:XX:XX|untag|vlan <0-4095>|cos <0-7>|cvlan <0-4095>|cvlan-cos <0-7>|offset <0-3> STRING}
 * - 功能说明 \n
 *	 配置mac acl rule
 * - 命令格式 \n
 *	 rule <1-65535> mac ethtype (HHHH|any){smac XX:XX:XX:XX:XX:XX|dmac XX:XX:XX:XX:XX:XX|untag|vlan <0-4095>|cos <0-7>|cvlan <0-4095>|cvlan-cos <0-7>|offset <0-3> STRING}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|Acl ruleid|
 *	 |HHHH|Ethernet type in the range <0x0600-0xffff>|
 *	 |XX:XX:XX:XX:XX:XX|Smac format|
 *	 |XX:XX:XX:XX:XX:XX|Dmac format|
 *	 |<1-4095>|Vlan |
 *	 |<0-7>|Cos|
 *	 |<1-4095>|cvlan|
 *	 |<0-7>|CVlan cos|
 *   |<0-3>|以太头后偏移字节数|
 *   |STRING|自定义匹配报文的内容<0x0-0xffffffff>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 ACL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于ACL模式下，用于配置mac acl rule,默认下到vfp,vfp不支持的情况下下到ifp
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   ethertype配置any时匹配所有协议类型； tag与untag不能同时配置下发，配了 untag，后面的 vlan、cos、cvlan、cvlan-cos都不能配
 *	 - 相关命令 \n
 *	   show acl <1-65535> {rule <1-65535>}
 * - 使用举例 \n
 *	   Huahuan(config)# acl 2
 *	   Huahuan(config-acl)# rule 6 mac smac 00:00:00:11:22:33 vlan  5 cos 6 cvlan 2
 *
 */

/**
 * \page show_acl_info_cmd_vtysh show acl [<1-65535>]
 * - 功能说明 \n
 *	 显示acl group 配置信息
 * - 命令格式 \n
 *	 show acl [<1-65535>]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口未配置acl group
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   显示acl group配置信息
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

static struct cmd_node acl_node =
{
  ACL_NODE,
  "%s(config-acl)# ",
};

static struct cmd_node qos_domain_node =
{
	QOS_DOMAIN_NODE,
	"%s(config-qos-domain)# ",
};

static struct cmd_node qos_phb_node =
{
	QOS_PHB_NODE,
	"%s(config-qos-phb)# ",
};

static struct cmd_node qos_policy_node =
{
  QOS_POLICY_NODE,
  "%s(config-qos-policy)# ",
  1,
};

static struct cmd_node qos_car_node =
{
	QOS_CAR_NODE,
	"%s(config-qos-car)# ",
	1,
};

static struct cmd_node qos_mirror_node =
{
  QOS_MIRROR_NODE,
  "%s(config-mirror)# ",
  1,
};

static struct cmd_node qos_cpcar_node =
{
  QOS_CPCAR_NODE,
  "%s(config-cpu-car)# ",
  1,
};


static struct cmd_node sla_session_node =
{
  SLA_SESSION_NODE,
  "%s(config-sla-session)# ",
  1 /* VTYSH */
};


static struct cmd_node hqos_wred_node =
{
    HQOS_WRED_NODE,
    "%s(config-qos-wred)# ",
    1
};


static struct cmd_node hqos_queue_node =
{
    HQOS_QUEUE_NODE,
    "%s(config-qos-queue)# ",
    1
};


DEFUNSH (VTYSH_QOS,
	 vtysh_exit_acl_num,
	 vtysh_exit_acl_num_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

ALIAS (vtysh_exit_acl_num,
       vtysh_quit_acl_num_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")

DEFUNSH (VTYSH_QOS,
	 vtysh_exit_qos_mapping_domain,
	 vtysh_exit_qos_mapping_domain_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

ALIAS (vtysh_exit_qos_mapping_domain,
       vtysh_quit_qos_mapping_domain_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")

DEFUNSH (VTYSH_QOS,
	 vtysh_exit_qos_mapping_phb,
	 vtysh_exit_qos_mapping_phb_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

ALIAS (vtysh_exit_qos_mapping_phb,
       vtysh_quit_qos_mapping_phb_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")

DEFUNSH (VTYSH_QOS,
	 vtysh_exit_qos_car,
	 vtysh_exit_qos_car_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

ALIAS (vtysh_exit_qos_car,
       vtysh_quit_qos_car_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")

DEFUNSH (VTYSH_QOS,
	 vtysh_exit_qos_mirror,
	 vtysh_exit_qos_mirror_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

ALIAS (vtysh_exit_qos_mirror,
       vtysh_quit_qos_mirror_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")

DEFUNSH (VTYSH_QOS,
	 vtysh_exit_qos_cpcar,
	 vtysh_exit_qos_cpcar_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

ALIAS (vtysh_exit_qos_cpcar,
       vtysh_quit_qos_cpcar_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")

DEFUNSH (VTYSH_QOS,
	 vtysh_exit_qos_wred,
	 vtysh_exit_qos_wred_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

ALIAS (vtysh_exit_qos_wred,
       vtysh_quit_qos_wred_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")

DEFUNSH (VTYSH_QOS,
	 vtysh_exit_qos_queue,
	 vtysh_exit_qos_queue_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

ALIAS (vtysh_exit_qos_queue,
       vtysh_quit_qos_queue_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")

/**
 * \page acl_num acl <1-65535>
 * - 功能说明 \n
 *	 acl <1-65535>创建acl group
 * - 命令格式 \n
 *	 acl <1-65535>
 * - 参数说明 \n
 *	 |参数  |说明			|
 *	 |----------|--------------|
 *	 |<1-65535> |group num|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用acl <1-65535>命令创建acl group,使用no格式删除该acl group
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show acl <1-65535> {rule <1-65535>}
 * - 使用举例 \n
 *	 无
 */

DEFUNSH (VTYSH_QOS,
		acl_num,
		acl_num_cmd,
		"acl <1-65535>",
		"Acl rule\n"
		"Acl num\n")
{
	vty->node = ACL_NODE;
	return CMD_SUCCESS;
}

/**
 * \page qos_mapping_domain_cmd qos mapping (cos-domain|tos-domain|exp-domain|dscp-domain) <1-5>
 * - 功能说明 \n
 *	 创建优先级映射域模板，进入优先级映射域配置模式
 * - 命令格式 \n
 *	 qos mapping (cos-domain|tos-domain|exp-domain|dscp-domain) <1-5>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |-----------|--------------|
 *	 |cos-domain |报文CoS域到内部优先级的映射		   |
 *	 |exp-domain |MPLS标签EXP位到内部优先级的映射	  |
 *	 |tos-domain |IP首部ToS字段到内部优先级的映射	  |
 *	 |dscp-domain|DSCP到内部优先级的映射	 |
 *	 |<1-5> 	 |表示映射模版的范围为<1-5>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，四种映射均存在默认模板“0”
 * - 命令模式 \n
 *	 命令行配置在全局配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建CoS/ToS/EXP/DSCP到内部优先级和颜色的映射模板，用于QoS
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domian) <0-5>
 * - 使用举例 \n
 *	   创建CoS映射域“1” \n
 *	   Hios(config)# qos mapping cos-domain 1 \n
 *	   Hios(config-qos-domain-cos)#  exit \n
 */
DEFUNSH(VTYSH_QOS,
		qos_mapping_domain,
		qos_mapping_domain_cmd,
		"qos mapping (cos-domain|tos-domain|exp-domain|dscp-domain) <1-5>",
		"Quality of Service\n"
		"QoS mapping\n"
		"Cos to queue mapping domain\n"
		"Tos to queue mapping domain\n"
		"Exp to queue mapping domain\n"
		"Dscp to queue mapping domain\n"
		"QoS mapping domain ID\n")
{
	char *pprompt = NULL;
	const char *prom_str = NULL;
	int type = 0;

	if (argv[0][0] == 'c')
	{
		type = QOS_TYPE_COS;
	}
	else if (argv[0][0] == 't')
	{
		type = QOS_TYPE_TOS;
	}
	else if (argv[0][0] == 'e')
	{
		type = QOS_TYPE_EXP;
	}
	else if (argv[0][0] == 'd')
	{
		type = QOS_TYPE_DSCP;
	}

	vty->node = QOS_DOMAIN_NODE;
	pprompt = vty->change_prompt;
	if ( pprompt )
	{
		/* format the prompt */
		QOS_MAPPING_PROMPT_GET(type, prom_str);
		
		if (NULL != prom_str)
		{
			snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-qos-domain-%s)#", prom_str );
		}
	}

	return CMD_SUCCESS;
}
/**
 * \page qos_mapping_phb_cmd qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <1-5>
 * - 功能说明 \n
 *	 创建配置优先级映射phb模板，进入优先级映射phb配置模式
 * - 命令格式 \n
 *	 qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <1-5>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |-----------|--------------|
 *	 |cos-phb |内部优先级到报文CoS域的映射		   |
 *	 |exp-phb |内部优先级MPLS标签到EXP位的映射	  |
 *	 |tos-phb |内部优先级IP首部到ToS字段的映射	  |
 *	 |dscp-phb|内部优先级到DSCP的映射|
 *   |<1-5>   |表示映射模版的范围为<1-5>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，四种映射均存在默认模板“0”
 * - 命令模式 \n
 *	 命令行配置在全局配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建内部优先级和颜色到CoS/ToS/EXP/DSCP的映射模板，用于QoS
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <0-5>
 * - 使用举例 \n
 *	   创建CoS映射域“1” \n
 *	   Hios(config)# qos mapping cos-phb 1 \n
 *	   Hios(config-qos-phb-cos)# exit \n
 */
DEFUNSH(VTYSH_QOS,
		qos_mapping_phb,
		qos_mapping_phb_cmd,
		"qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <1-5>",
		"Quality of Service\n"
		"QoS mapping\n"
		"Queue to cos mapping\n"
		"Queue to exp mapping\n"
		"Queue to tos mapping\n"
		"Queue to dscp mapping\n"
		"QoS mapping phb ID\n")
{
	char *pprompt = NULL;
	const char *prom_str = NULL;
	int type = 0;

	if (argv[0][0] == 'c')
	{
		type = QOS_TYPE_COS;
	}
	else if (argv[0][0] == 't')
	{
		type = QOS_TYPE_TOS;
	}
	else if (argv[0][0] == 'e')
	{
		type = QOS_TYPE_EXP;
	}
	else if (argv[0][0] == 'd')
	{
		type = QOS_TYPE_DSCP;
	}


	vty->node = QOS_PHB_NODE;

	pprompt = vty->change_prompt;
	if ( pprompt )
	{
		/* format the prompt */
		QOS_MAPPING_PROMPT_GET(type, prom_str);

		if (NULL != prom_str)
		{
			snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-qos-phb-%s)#", prom_str );
		}
	}

	return CMD_SUCCESS;
}

/**
 * \page qos_car qos car-profile <1-1000>
 * - 功能说明 \n
 *	 创建CAR模板，进入CAR配置视图
 * - 命令格式 \n
 *	 qos car-profile <1-1000>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|--------------|
 *	 |<1-1000>|模板范围是<1-1000>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，没有car模板被创建
 * - 命令模式 \n
 *	 命令配置在全局配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建QoS CAR模板，用于QoS
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   创建car模板后，必须在节点下配置具体的限速参数，否则模板将不能被应用到接口
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   创建CAR模板1
 *     Huahuan(config)# qos car-profile 1
 *
 */
DEFUNSH(VTYSH_QOS,
		qos_car,
		qos_car_cmd,
		"qos car-profile <1-1000>",
		"Quality of Service\n"
		"QoS car-profile\n"
		"QoS car-profile ID\n")
{
	vty->node = QOS_CAR_NODE;
	return CMD_SUCCESS;
}

/**
 * \page no_acl_num_cmd_vtysh no acl <1-65535>
 * - 功能说明 \n
 *	 删除acl group
 * - 命令格式 \n
 *	 no acl <1-65535>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   删除已创建的acl group
 *	 - 前置条件 \n
 *	   已创建acl group
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# acl 2 \n
 *     Huahuan(config-acl)# rule 5 ip dip 1.2.3.4/24 \n
 *     Huahuan(config)# no acl 2 \n
 *
 */
DEFSH (VTYSH_QOS,no_acl_num_cmd_vtysh,
		"no acl <1-65535>",
		NO_STR
		"Acl rule\n"
		"Acl num\n")

/**
 * \page acl_name_cmd_vtysh name NAME
 * - 功能说明 \n
 *	 配置acl group 名字
 * - 命令格式 \n
 *	 name NAME
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|Acl group name|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 ACL模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于ACL模式下，使用name NAME配置acl group 名字
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# acl 2 \n
 *	   Huahuan(config-acl)# name  nnnnn \n
 *	   Huahuan(config-acl)# no	name \n
 *
 */
DEFSH (VTYSH_QOS, acl_name_cmd_vtysh,
		"name NAME",
		"Name\n"
		"Acl name\n")

/**
 * \page no_acl_name_cmd_vtysh no name
 * - 功能说明 \n
 *	 使用no name删除acl group 名字
 * - 命令格式 \n
 *	 no name
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 ACL模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于ACL模式下,使用no格式删除acl group 名字
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show acl [<1-65535>]
 * - 使用举例 \n
 *	   Huahuan(config)# acl 2 \n
 *	   Huahuan(config-acl)# name nnnnn \n
 *	   Huahuan(config-acl)# no name \n
 *
 */
DEFSH (VTYSH_QOS, no_acl_name_cmd_vtysh,
  "no name",
   NO_STR
  "Acl name\n")

/**
 * \page acl_ip_extend_rule_cmd_vtysh rule <1-65535> ip-extended protocol (HH|icmp|igmp|ip|any) {sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>}
 * - 功能说明 \n
 *	 配置扩展 IP acl rule
 * - 命令格式 \n
 *	 rule <1-65535> ip-extended protocol (HH|icmp|igmp|ip|any) {sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|Acl ruleid|
 *	 |(HH\|icmp\|igmp\|ip\|any)|Protocol type,HH 范围<0x00-0xff>|
 *	 |sip A.B.C.D/M|Sip format|
 *	 |dip A.B.C.D/M|Dip format|
 *   |l3vpn<1-128>|vpnid|
 *   |dscp<0-63>|Dscp value|
 *   |tos<0-7>|Tos value|
 *   |ttl<1-255>|Ttl value|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 ACL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于ACL配置模式下，配置基本 IP acl rule
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show acl [<1-65535>]
 * - 使用举例 \n
 *	   Huahuan(config)# acl 2 \n
 *	   Huahuan(config-acl)# rule 5 ip-extended protocol ip sip 192.192.1.1/24 l3vpn 2 dscp 3 ttl 5
 *
 */
DEFSH (VTYSH_QOS, acl_ip_extend_rule_cmd_vtysh,
 	"rule <1-65535> ip-extended protocol (HH|icmp|igmp|ip|any) {sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>}",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl rule for ip-extended\n"
	"Acl ip-extended protocal\n"
	"Acl ip-extended protocal: <0x00-0xff>\n"
	"Acl ip-extended protocal: icmp\n"
	"Acl ip-extended protocal: igmp\n"
	"Acl ip-extended protocal: ip\n"
	"Acl ip-extended protocal: any protocal\n"
	"Acl ip-extended source ip\n"
	"Acl ip-extended source ip:A.B.C.D/M\n"
	"Acl ip-extended destination ip\n"
	"Acl ip-extended destination ip:A.B.C.D/M\n"
	"Acl ip-extended l3vpn\n"
	"Acl ip-extended l3vpn vpnid format<1-128>\n"
	"Acl ip-extended dscp\n"
	"Acl ip-extended dscp in the range from 0 to 63\n"
	"Acl ip-extended tos\n"
	"Acl ip-extended tos in the range from 0 to 7\n"
	"Acl ip-extended ttl\n"
	"Acl ip-extended ttl in the range from 1 to 255\n")

 DEFSH (VTYSH_QOS, acl_ip_extend_2_rule_cmd_vtysh,
	"rule <1-65535> ip-extended protocol (udp|tcp) {sport <1-65535>|dport <1-65535>|sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>}",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl rule for ip-extended\n"
	"Acl ip-extended protocal\n"
	"Acl ip-extended protocal: udp\n"
	"Acl ip-extended protocal: tcp\n"
	"Source port\n"
	"Source port in the range from 1 to 65535\n"
	"Destination port\n"
	"Destination port in the range from 1 to 65535\n"
	"Acl ip-extended source ip\n"
	"Acl ip-extended source ip : A.B.C.D/M\n"
	"Acl ip-extended destination ip\n"
	"Acl ip-extended destination ip: A.B.C.D/M\n"
	"Acl ip-extended l3vpn\n"
	"Acl ip-extended l3vpn vpnid format<1-128>\n"
	"Acl ip-extended dscp\n"
	"Acl ip-extended dscp in the range from 0 to 63\n"
	"Acl ip-extended tos\n"
	"Acl ip-extended tos in the range from 0 to 7\n"
	"Acl ip-extended ttl\n"
	"Acl ip-extended ttl in the range from 1 to 255\n")

DEFSH (VTYSH_QOS, acl_ip_extend_3_rule_cmd_vtysh,
   "rule <1-65535> ip-extended protocol tcp {sport <1-65535>|dport <1-65535>|sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>} tcp-flag {fin|syn|rst|psh|ack|urg}",
   "Acl rule\n"
   "Acl rule number\n"
   "Acl rule for ip-extended\n"
   "Acl ip-extended protocal\n"
   "Acl ip-extended protocal: tcp\n"
   "Source port\n"
   "Source port in the range from 1 to 65535\n"
   "Destination port\n"
   "Destination port in the range from 1 to 65535\n"
   "Acl ip-extended source ip\n"
   "Acl ip-extended source ip : A.B.C.D/M\n"
   "Acl ip-extended destination ip\n"
   "Acl ip-extended destination ip: A.B.C.D/M\n"
   "Acl ip-extended l3vpn\n"
   "Acl ip-extended l3vpn vpnid format<1-128>\n"
   "Acl ip-extended dscp\n"
   "Acl ip-extended dscp in the range from 0 to 63\n"
   "Acl ip-extended tos\n"
   "Acl ip-extended tos in the range from 0 to 7\n"
   "Acl ip-extended ttl\n"
   "Acl ip-extended ttl in the range from 1 to 255\n"
   "Acl ip-extended tcp flag\n"
   "Acl ip-extended tcp flag: fin\n"
   "Acl ip-extended tcp flag: syn\n"
   "Acl ip-extended tcp flag: rst\n"
   "Acl ip-extended tcp flag: psh\n"
   "Acl ip-extended tcp flag: ack\n"
   "Acl ip-extended tcp flag: urg\n")

DEFSH (VTYSH_QOS, acl_mix_rule_cmd_vtysh,
	"rule <1-65535> mix {ethtype HHHH|smac XX:XX:XX:XX:XX:XX|dmac XX:XX:XX:XX:XX:XX | "
	"vlan <1-4095>|cos <0-7>|cvlan <1-4095>|cvlan-cos <0-7>|"
	"protocol HH|sip A.B.C.D/M|dip A.B.C.D/M|l3vpn <1-128>|dscp <0-63>|tos <0-7>|ttl <1-255>}",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl rule for mix\n"
	"Ethernet type\n"
	"Ethernet type in the range <0x0600-0xffff>\n"
	"Source mac address\n"
	"Source mac address:XX:XX:XX:XX:XX:XX\n"
	"Destination mac address\n"
	"Destination mac address:XX:XX:XX:XX:XX:XX\n"
	"Vlan id\n"
	"Vlan id in the range from 1 to 4095\n"
	"Cos id\n"
	"Cos id in the range from 0 to 7\n"
	"Cvlan id\n"
	"Cvlan id in the range from 1 to 4095\n"
	"Cvlan-cos id\n"
	"Cvlan-cos id in the range from 0 to 7\n"
	"Acl ip-extended protocal\n"
	"Acl ip-extended protocal: <0x00-0xff>\n"
	"Acl ip-extended source ip\n"
	"Acl ip-extended source ip:A.B.C.D/M\n"
	"Acl ip-extended destination ip\n"
	"Acl ip-extended destination ip:A.B.C.D/M\n"
	"Acl ip-extended l3vpn\n"
	"Acl ip-extended l3vpn vpnid format<1-128>\n"
	"Acl ip-extended dscp\n"
	"Acl ip-extended dscp in the range from 0 to 63\n"
	"Acl ip-extended tos\n"
	"Acl ip-extended tos in the range from 0 to 7\n"
	"Acl ip-extended ttl\n"
	"Acl ip-extended ttl in the range from 1 to 255\n")

DEFSH (VTYSH_QOS, acl_ip_rule_cmd_vtysh,
	"rule <1-65535> ip (sip|dip) (A.B.C.D/M|any) [l3vpn <1-128>]",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl rule for ip\n"
	"Sip\n"
	"Dip\n"
	"Ip address:A.B.C.D/M\n"
	"Ip address:any\n"
	"l3vpn \n"
	"l3vpn Vpnid format<1-128>\n")

DEFSH (VTYSH_QOS, no_acl_ip_rule_cmd_vtysh,
	"no rule <1-65535>",
	 NO_STR
	"Acl ip rule\n"
	"Acl rule\n")

DEFSH (VTYSH_QOS, acl_mac_rule_cmd_vtysh,
	"rule <1-65535> mac ethtype (HHHH|any){smac XX:XX:XX:XX:XX:XX|dmac XX:XX:XX:XX:XX:XX|untag|vlan <1-4095>|cos <0-7>|cvlan <1-4095>|cvlan-cos <0-7>|offset <0-3> STRING}",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl rule for mac\n"
	"Ethernet type\n"
	"Ethernet type in the range <0x0600-0xffff>\n"
	"Ethernet type any\n"
	"Source mac address\n"
	"Source mac address:XX:XX:XX:XX:XX:XX\n"
	"Destination mac address\n"
	"Destination mac address:XX:XX:XX:XX:XX:XX\n"
	"Untag\n"
	"Vlan id\n"
	"Vlan id in the range from 1 to 4095\n"
	"Cos id\n"
	"Cos id in the range from 0 to 7\n"
	"Cvlan id\n"
	"Cvlan id in the range from 1 to 4095\n"
	"Cvlan-cos id\n"
	"Cvlan-cos id in the range from 0 to 7\n"
	"Offset: After ethernet header, custom matching message content\n"
	"Offset: Ethernet header offset 0 to 3 bytes\n"
	"Offset_info:Ethernet head offset and offset content not exceeding 4 bytes format <0x0-0xffffffff>\n")

DEFSH (VTYSH_QOS,show_acl_info_cmd_vtysh,
	"show acl [<1-65535>]",
	"Show\n"
	"Acl rule\n"
	"Acl num\n")

/**
 * \page show_acl_rule_info_cmd_vtysh show acl <1-65535> rule <1-65535>
 * - 功能说明 \n
 *	 显示指定acl group rule配置信息
 * - 命令格式 \n
 *	 show acl <1-65535> rule <1-65535>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *	 |<1-65535>|rule num|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口未配置acl group rule
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   显示acl group rule配置信息
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
DEFSH (VTYSH_QOS,show_acl_rule_info_cmd_vtysh,
	"show acl <1-65535> rule <1-65535>",
	"Show\n"
	"Acl rule\n"
	"Acl num\n"
	"Show acl rule infomation\n"
	"Acl rule number\n")


/**
 * \page no_qos_mapping_domain_cmd_vtysh no qos mapping (cos-domain|tos-domain|exp-domain|dscp-domain) <1-5>
 * - 功能说明 \n
 *	 删除优先级映射域模板
 * - 命令格式 \n
 *	 no qos mapping (cos-domain|tos-domain|exp-domain|dscp-domain) <1-5>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |-----------|--------------|
 *	 |cos-domain |报文CoS域到内部优先级的映射		   |
 *	 |exp-domain |MPLS标签EXP位到内部优先级的映射	  |
 *	 |tos-domain |IP首部ToS字段到内部优先级的映射	  |
 *	 |dscp-domain|DSCP到内部优先级的映射	 |
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在全局配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除CoS/ToS/EXP/DSCP到内部优先级和颜色的映射模板，用于QoS
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
 */
DEFSH(VTYSH_QOS,
		no_qos_mapping_domain_cmd_vtysh,
		"no qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <1-5>",
		NO_STR
		"Quality of Service\n"
		"QoS mapping\n"
		"Cos to queue mapping domain\n"
		"Tos to queue mapping domain\n"
		"Exp to queue mapping domain\n"
		"Dscp to queue mapping domain\n"
		"QoS mapping domain ID\n")

/**
 * \page no_qos_mapping_phb_cmd_vtysh no qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <1-5>
 * - 功能说明 \n
 *	 删除优先级映射phb模板
 * - 命令格式 \n
 *	 no qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <1-5>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |-----------|--------------|
 *	 |cos-phb |内部优先级到报文CoS域的映射		   |
 *	 |exp-phb |内部优先级MPLS标签到EXP位的映射	  |
 *	 |tos-phb |内部优先级IP首部到ToS字段的映射   |
 *	 |dscp-phb|内部优先级到DSCP的映射|
 *	 |<1-5>   |表示映射模版的范围为<1-5>|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在全局配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除内部优先级和颜色到CoS/ToS/EXP/DSCP的映射模板，用于QoS
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
 */
DEFSH(VTYSH_QOS,
		no_qos_mapping_phb_cmd_vtysh,
		"no qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <1-5>",
		NO_STR
		"Quality of Service\n"
		"QoS mapping\n"
		"Queue to cos mapping\n"
		"Queue to exp mapping\n"
		"Queue to tos mapping\n"
		"Queue to dscp mapping\n"
		"QoS mapping phb ID\n")

/**
 * \page qos_cos_to_queue_cmd_vtysh cos <0-7> to queue <0-7> {color (green|yellow|red)}
 * - 功能说明 \n
 *	 配置CoS到内部优先级队列和颜色的映射
 * - 命令格式 \n
 *	 cos <0-7> to queue <0-7> {color (green|yellow|red)}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<0-7> |表示CoS的有效范围为<0-7>		|
 *	 |<0-7> |表示内部优先级队列的有效范围为<0-7>|
 *	 |green |标记报文为绿色|
 *	 |yellow|标记报文为黄色|
 *   |red   |标记报文为红色|
 *
 * - 缺省情况 \n
 *   Cos到内部优先级和颜色的默认映射 \n
 *   |CoS|优先级队列|颜色 |
 *	 |---|----------|-----|
 *	 |  0|         0|Green|
 *	 |  1|         1|Green|
 *	 |  2|         2|Green|
 *	 |  3|         3|Green|
 *	 |  4|         4|Green|
 *	 |  5|         5|Green|
 *	 |  6|         6|Green|
 *	 |  7|         7|Green|
 *
 * - 命令模式 \n
 *	 命令行配置在优先级映射域配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建一条CoS到内部优先级和颜色的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个CoS到内部优先级和颜色的映射域
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_cos_to_queue_cmd_vtysh,
		"cos <0-7> to queue <0-7> {color (green|yellow|red)}",
		"Class of Service\n"
		"Specify cos value\n"
		"Mapping\n"
		"Queue\n"
		"Specify queue value\n"
		"Color\n"
		"Green\n"
		"Yellow\n"
		"Red\n")

/**
 * \page no_qos_cos_to_queue_cmd_vtysh no cos <0-7>
 * - 功能说明 \n
 *	 删除CoS到内部优先级队列和颜色的映射
 * - 命令格式 \n
 *	 no cos <0-7>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<0-7> |表示CoS的有效范围为<0-7>		|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在优先级映射域配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除一条CoS到内部优先级和颜色的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个CoS到内部优先级和颜色的映射
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
DEFSH(VTYSH_QOS,
		no_qos_cos_to_queue_cmd_vtysh,
		"no cos <0-7>",
		NO_STR
		"Class of Service\n"
		"Specify cos value\n")

/**
 * \page qos_tos_to_queue_cmd_vtysh tos <0-7> to queue <0-7> {color (green|yellow|red)}
 * - 功能说明 \n
 *	 配置ToS到内部优先级队列和颜色的映射
 * - 命令格式 \n
 *	 tos <0-7> to queue <0-7> {color (green|yellow|red)}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<0-7> |表示ToS的有效范围为<0-7>		|
 *	 |<0-7> |表示内部优先级队列的有效范围为<0-7>|
 *	 |green |标记报文为绿色|
 *	 |yellow|标记报文为黄色|
 *	 |red	|标记报文为红色|
 *
 * - 缺省情况 \n
 *   Tos到内部优先级和颜色的默认映射 \n
 *	 |ToS|优先级队列|颜色 |
 *	 |---|----------|-----|
 *	 |	0|		   0|Green|
 *	 |	1|		   1|Green|
 *	 |	2|		   2|Green|
 *	 |	3|		   3|Green|
 *	 |	4|		   4|Green|
 *	 |	5|		   5|Green|
 *	 |	6|		   6|Green|
 *	 |	7|		   7|Green|
 *
 * - 命令模式 \n
 *	 命令行配置在优先级映射域配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建一条ToS到内部优先级和颜色的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个ToS到内部优先级和颜色的映射域
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_tos_to_queue_cmd_vtysh,
		"tos <0-7> to queue <0-7> {color (green|yellow|red)}",
		"Type of Service\n"
		"Specify tos value\n"
		"Mapping\n"
		"Queue\n"
		"Specify queue value\n"
		"Color\n"
		"Green\n"
		"Yellow\n"
		"Red\n")

/**
 * \page no_qos_tos_to_queue_cmd_vtysh no tos <0-7>
 * - 功能说明 \n
 *	 删除ToS到内部优先级队列和颜色的映射
 * - 命令格式 \n
 *	 no tos <0-7>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<0-7> |表示ToS的有效范围为<0-7>		|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在优先级映射域配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除一条ToS到内部优先级和颜色的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个ToS到内部优先级和颜色的映射
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
DEFSH(VTYSH_QOS,
		no_qos_tos_to_queue_cmd_vtysh,
		"no tos <0-7>",
		NO_STR
		"Type of Service\n"
		"Specify tos value\n")

/**
 * \page qos_exp_to_queue_cmd_vtysh exp <0-7> to queue <0-7> {color (green|yellow|red)}
 * - 功能说明 \n
 *	 配置MPLS标签EXP位到内部优先级队列和颜色的映射
 * - 命令格式 \n
 *	 exp <0-7> to queue <0-7> {color (green|yellow|red)}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<0-7> |表示MPLS EXP的有效范围为<0-7>		|
 *	 |<0-7> |表示内部优先级队列的有效范围为<0-7>|
 *	 |green |标记报文为绿色|
 *	 |yellow|标记报文为黄色|
 *	 |red	|标记报文为红色|
 *
 * - 缺省情况 \n
 *   MPLS标签EXP位到内部优先级和颜色的默认映射 \n
 *	 |EXP|优先级队列|颜色 |
 *	 |---|----------|-----|
 *	 |	0|		   0|Green|
 *	 |	1|		   1|Green|
 *	 |	2|		   2|Green|
 *	 |	3|		   3|Green|
 *	 |	4|		   4|Green|
 *	 |	5|		   5|Green|
 *	 |	6|		   6|Green|
 *	 |	7|		   7|Green|
 *
 * - 命令模式 \n
 *	 命令行配置在优先级映射域配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建一条MPLS EXP到内部优先级和颜色的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个MPLS EXP到内部优先级和颜色的映射域
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_exp_to_queue_cmd_vtysh,
		"exp <0-7> to queue <0-7> {color (green|yellow|red)}",
		"MPLS label experimental bits\n"
		"Specify exp value\n"
		"Mapping\n"
		"Queue\n"
		"Specify queue value\n"
		"Color\n"
		"Green\n"
		"Yellow\n"
		"Red\n")

/**
 * \page no_qos_exp_to_queue_cmd_vtysh no exp <0-7>
 * - 功能说明 \n
 *	 删除MPLS标签EXP到内部优先级队列和颜色的映射
 * - 命令格式 \n
 *	 no exp <0-7>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<0-7> |表示ToS的有效范围为<0-7>		|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在优先级映射域配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除一条MPLS EXP到内部优先级和颜色的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个MPLS EXP到内部优先级和颜色的映射
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
DEFSH(VTYSH_QOS,
		no_qos_exp_to_queue_cmd_vtysh,
		"no exp <0-7>",
		NO_STR
		"MPLS label experimental bits\n"
		"Specify exp value\n")

/**
 * \page qos_dscp_to_queue_cmd_vtysh dscp <0-63> to queue <0-7> {color (green|yellow|red)}
 * - 功能说明 \n
 *	 配置DSCP到内部优先级队列和颜色的映射
 * - 命令格式 \n
 *	 dscp <0-63> to queue <0-7> {color (green|yellow|red)}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<0-63> |表示DSCP的有效范围为<0-7>		|
 *	 |<0-7> |表示内部优先级队列的有效范围为<0-7>|
 *	 |green |标记报文为绿色|
 *	 |yellow|标记报文为黄色|
 *	 |red	|标记报文为红色|
 *
 * - 缺省情况 \n
 *	 不指定报文颜色参数时，颜色参数默认为绿色“green” \n
 *   DSCP到内部优先级和颜色的默认映射 \n
 *	 |            DSCP          |优先级队列|颜色|
 *	 |--------------------------|--------|------|
 *	 | 0  1  2  3  4  5  6  7  9|		 |      |
 *   |11 13 15 17 19 21 23 25 27|        |      |
 *   |29 31 33 35 37 39 41 42 43|        |      |
 *   |44 45 47 49 50 51 52 53 54|        |      |
 *   |55 57 58 59 60 61 62 63   |       0| Green|
 *	 |	                    8 10|		1| Green|
 *	 |	                      12|		1|Yellow|
 *	 |	                      14|	    1|   Red|
 *	 |	                   16 18|		2| Green|
 *	 |	                      20|		2|Yellow|
 *	 |	                      22|	    2|   Red|
 *	 |	                   24 26|		3| Green|
 *	 |	                      28|		3|Yellow|
 *	 |	                      30|	    3|   Red|
 *	 |	                   32 34|		4| Green|
 *	 |	                      36|		4|Yellow|
 *	 |	                      38|	    4|   Red|
 *	 |	                   40 46|		5| Green|
 *	 |	                      48|	    6| Green|
 *	 |	                      56|		7| Green|
 *
 * - 命令模式 \n
 *	 命令行配置在优先级映射域配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建一条DSCP到内部优先级和颜色的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个DSCP到内部优先级和颜色的映射域
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_dscp_to_queue_cmd_vtysh,
		"dscp <0-63> to queue <0-7> {color (green|yellow|red)}",
		"Differentiated Services Code Point\n"
		"Specify dscp value\n"
		"Mapping\n"
		"Queue\n"
		"Specify queue value\n"
		"Color\n"
		"Green\n"
		"Yellow\n"
		"Red\n")

/**
 * \page no_qos_dscp_to_queue_cmd_vtysh no dscp <0-63>
 * - 功能说明 \n
 *	 删除DSCP到内部优先级队列和颜色的映射
 * - 命令格式 \n
 *	 no dscp <0-63>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<0-63> |表示DSCP的有效范围为<0-63>		|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在优先级映射域配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除一条DSCP到内部优先级和颜色的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个DSCP到内部优先级和颜色的映射
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
DEFSH(VTYSH_QOS,
		no_qos_dscp_to_queue_cmd_vtysh,
		"no dscp <0-63>",
		NO_STR
		"Differentiated Services Code Point\n"
		"Specify dscp value\n")

/**
 * \page show_qos_mapping_domain_cmd_vtysh show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>
 * - 功能说明 \n
 *	 显示当前QoS mapping domain映射模板的配置情况
 * - 命令格式 \n
 *	 show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |-----------|--------------|
 *	 |cos-domain |报文CoS域到内部优先级的映射		   |
 *	 |exp-domain |MPLS标签EXP位到内部优先级的映射	  |
 *	 |tos-domain |IP首部ToS字段到内部优先级的映射	  |
 *	 |dscp-domain|DSCP到内部优先级的映射	 |
 *   |<0-5>      |表示可显示的映射模版的范围为<0-5>|
 *
 * - 缺省情况 \n
 *	 缺省情况下只能显示各个映射域的默认模板“0”
 * - 命令模式 \n
 *	 命令行配置在全局配置模式和特权模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   显示当前QoS mapping domain映射模板的配置情况，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建优先级映射域，否则只能显示默认模板信息
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   创建CoS到内部优先级的映射模板，并修改一组默认映射后显示配置情况 \n
 *	   Hios(config)# qos mapping cos-domain 1 \n
 *	   Hios(config-qos-domain-cos)# cos 7 to queue 3 color red \n
 *     Hios(config-qos-domain-cos)# exit \n
 *     Hios(config)# show qos mapping cos-domain 1 \n
 *     ID: 1 \n
 *	   |EXP|优先级队列|颜色 |
 *	   |-----|----------|-----|
 *	   |	0|		   0|Green|
 *	   |	1|		   1|Green|
 *	   |	2|		   2|Green|
 *	   |	3|		   3|Green|
 *	   |	4|		   4|Green|
 *	   |	5|		   5|Green|
 *	   |	6|		   6|Green|
 *	   |	7|		   3|Red  |

 *
 */
DEFSH(VTYSH_QOS,
		show_qos_mapping_domain_cmd_vtysh,
		"show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>",
		"Show qos mapping domain infomation\n"
		"Quality of Service\n"
		"QoS mapping\n"
		"Cos to queue mapping domain\n"
		"Exp to queue mapping domain\n"
		"Tos to queue mapping domain\n"
		"Dscp to queue mapping domain\n"
		"QoS mapping domain ID\n")

/**
 * \page show_qos_mapping_configure_cmd_vtysh show qos mapping configure
 * - 功能说明 \n
 *	 显示当前qos mapping 映射模板的配置情况
 * - 命令格式 \n
 *	 show qos mapping configure
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下只能显示各个映射的默认模板“0”
 * - 命令模式 \n
 *	 命令行配置在全局配置模式和特权模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   显示当前QoS mapping 映射模板的配置情况，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建优先级映射，否则只能显示默认模板信息
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *     Hios(config)# qos mapping cos-domain 1 \n
 *     Hios(config-qos-domain-cos)#exit \n
 *     Hios(config)# qos mapping cos-domain 2 \n
 *     Hios(config-qos-domain-cos)#exit \n
 *     Hios(config)# show qos mapping configure \n
 *     cos-domain  1 \n
 *     cos-domain  2 \n
 *
 */
DEFSH(VTYSH_QOS,
		show_qos_mapping_configure_cmd_vtysh,
        "show qos mapping configure",
        "Show qos mapping infomation\n"
        "Quality of Service\n"
        "QoS mapping\n"
        "Mapping of configured\n")

/**
 * \page qos_queue_to_cos_cmd_vtysh queue <0-7> {color (green|yellow|red)} to cos <0-7>
 * - 功能说明 \n
 *	 配置内部优先级队列和颜色到CoS的映射
 * - 命令格式 \n
 *	 queue <0-7> {color (green|yellow|red)} to cos <0-7>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<0-7> |表示内部优先级队列的有效范围为<0-7>|
 *	 |green |标记报文为绿色|
 *	 |yellow|标记报文为黄色|
 *	 |red	|标记报文为红色|
 *	 |<0-7> |表示CoS的有效范围为<0-7>		|
 *
 * - 缺省情况 \n
 *	 内部优先级和颜色到CoS的默认映射PHB: \n
 * 	 |优先级队列|颜色|CoS|
 *	 |----------|----|---|
 * 	 |0 		|  绿|   0|
 * 	 |			|  黄|   0|
 * 	 |			|  红|   0|
 * 	 |1 		|  绿|   1|
 * 	 |			|  黄|   1|
 * 	 |			|  红|   1|
 * 	 |2 		|  绿|   2|
 * 	 |			|  黄|   2|
 * 	 |			|  红|   2|
 * 	 |3 		|  绿|   3|
 * 	 |			|  黄|   3|
 * 	 |			|  红|   3|
 * 	 |4 		|  绿|   4|
 * 	 |			|  黄|   4|
 * 	 |			|  红|   4|
 * 	 |5 		|  绿|   5|
 * 	 |			|  黄|   5|
 * 	 |			|  红|   5|
 * 	 |6 		|  绿|   6|
 * 	 |			|  黄|   6|
 * 	 |			|  红|   6|
 * 	 |7 		|  绿|   7|
 * 	 |			|  黄|   7|
 * 	 |			|  红|   7|
 *
 * - 命令模式 \n
 *	 命令行配置在优先级映射phb配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建一条内部优先级和颜色到CoS的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个内部优先级和颜色到CoS的映射域PHB
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_queue_to_cos_cmd_vtysh,
		"queue <0-7> {color (green|yellow|red)} to cos <0-7>",
		"Queue\n"
		"Specify queue value\n"
		"Color\n"
		"Green\n"
		"Yellow\n"
		"Red\n"
		"Mapping\n"
		"Class of Service\n"
		"Specify cos value\n")

/**
 * \page no_qos_queue_to_cos_cmd_vtysh no queue <0-7> {color (green|yellow|red)} to cos
 * - 功能说明 \n
 *	 删除内部优先级队列和颜色到CoS的映射
 * - 命令格式 \n
 *	 no queue <0-7> {color (green|yellow|red)} to cos
 * - 参数说明 \n
 *	 |参数    |说明		       |
 *	 |------|--------------|
 *	 |<0-7> |表示内部优先级队列的有效范围为<0-7>|
 *	 |green |标记报文为绿色|
 *	 |yellow|标记报文为黄色|
 *	 |red	|标记报文为红色|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在优先级映射phb配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除一条内部优先级和颜色到CoS的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个内部优先级和颜色到CoS的映射
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
DEFSH(VTYSH_QOS,
		no_qos_queue_to_cos_cmd_vtysh,
		"no queue <0-7> {color (green|yellow|red)} to cos",
		NO_STR
		"Queue\n"
		"Specify queue value\n"
		"Color\n"
		"Green\n"
		"Yellow\n"
		"Red\n"
		"Mapping\n"
		"Class of Service\n")

/**
 * \page qos_queue_to_exp_cmd_vtysh queue <0-7> {color (green|yellow|red)} to exp <0-7>
 * - 功能说明 \n
 *	 配置内部优先级队列和颜色到MPLS EXP的映射
 * - 命令格式 \n
 *	 queue <0-7> {color (green|yellow|red)} to exp <0-7>
 * - 参数说明 \n
 *	 |参数    |说明		       |
 *	 |------|--------------|
 *	 |<0-7> |表示内部优先级队列的有效范围为<0-7>|
 *	 |green |标记报文为绿色|
 *	 |yellow|标记报文为黄色|
 *	 |red	|标记报文为红色|
 *	 |<0-7> |表示MPLS EXP的有效范围为<0-7>|
 *
 * - 缺省情况 \n
 *	 内部优先级队列和颜色到EXP的默认映射情况: \n
 *	 |优先级队列|颜色|EXP|
 *	 |----------|----|---|
 *	 |0 		|  绿|   0|
 *	 |			|  黄|   0|
 *	 |			|  红|   0|
 *	 |1 		|  绿|   1|
 *	 |			|  黄|   1|
 *	 |			|  红|   1|
 *	 |2 		|  绿|   2|
 *	 |			|  黄|   2|
 *	 |			|  红|   2|
 *	 |3 		|  绿|   3|
 *	 |			|  黄|   3|
 *	 |			|  红|   3|
 *	 |4 		|  绿|   4|
 *	 |			|  黄|   4|
 *	 |			|  红|   4|
 *	 |5 		|  绿|   5|
 *	 |			|  黄|   5|
 *	 |			|  红|   5|
 *	 |6 		|  绿|   6|
 *	 |			|  黄|   6|
 *	 |			|  红|   6|
 *	 |7 		|  绿|   7|
 *	 |			|  黄|   7|
 *	 |			|  红|   7|
 *
 * - 命令模式 \n
 *	 命令行配置在优先级映射phb配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建一条内部优先级和颜色到MPLS EXP的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个内部优先级和颜色到MPLS EXP的映射域PHB
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_queue_to_exp_cmd_vtysh,
		"queue <0-7> {color (green|yellow|red)} to exp <0-7>",
		"Queue\n"
		"Specify queue value\n"
		"Color\n"
		"Green\n"
		"Yellow\n"
		"Red\n"
		"Mapping\n"
		"MPLS label experimental bits\n"
		"Specify exp value\n")

/**
 * \page no_qos_queue_to_exp_cmd_vtysh no queue <0-7> {color (green|yellow|red)} to exp
 * - 功能说明 \n
 *	 删除内部优先级队列和颜色到MPLS EXP的映射
 * - 命令格式 \n
 *	 no queue <0-7> {color (green|yellow|red)} to exp
 * - 参数说明 \n
 *	 |参数	|说明 		   |
 *	 |------|--------------|
 *	 |<0-7> |表示内部优先级队列的有效范围为<0-7>|
 *	 |green |标记报文为绿色|
 *	 |yellow|标记报文为黄色|
 *	 |red	|标记报文为红色|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在优先级映射phb配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除一条内部优先级和颜色到MPLS EXP的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个内部优先级和颜色到MPLS EXP的映射
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
DEFSH(VTYSH_QOS,
		no_qos_queue_to_exp_cmd_vtysh,
		"no queue <0-7> {color (green|yellow|red)} to exp",
		NO_STR
		"Queue\n"
		"Specify queue value\n"
		"Color\n"
		"Green\n"
		"Yellow\n"
		"Red\n"
		"Mapping\n"
		"MPLS label experimental bits\n")

/**
 * \page qos_queue_to_tos_cmd_vtysh queue <0-7> {color (green|yellow|red)} to tos <0-7>
 * - 功能说明 \n
 *	 配置内部优先级队列和颜色到ToS的映射
 * - 命令格式 \n
 *	 queue <0-7> {color (green|yellow|red)} to tos <0-7>
 * - 参数说明 \n
 *	 |参数    |说明	    	   |
 *	 |------|--------------|
 *	 |<0-7> |表示内部优先级队列的有效范围为<0-7>|
 *	 |green |标记报文为绿色|
 *	 |yellow|标记报文为黄色|
 *	 |red	|标记报文为红色|
 *	 |<0-7> |表示ToS的有效范围为<0-7>		|

 * - 缺省情况 \n
 *	 内部优先级和颜色到ToS的默认映射PHB: \n
 *	 |优先级队列|颜色|ToS|
 *	 |----------|----|---|
 *	 |0 		|  绿|   0|
 *	 |			|  黄|   0|
 *	 |			|  红|   0|
 *	 |1 		|  绿|   1|
 *	 |			|  黄|   1|
 *	 |			|  红|   1|
 *	 |2 		|  绿|   2|
 *	 |			|  黄|   2|
 *	 |			|  红|   2|
 *	 |3 		|  绿|   3|
 *	 |			|  黄|   3|
 *	 |			|  红|   3|
 *	 |4 		|  绿|   4|
 *	 |			|  黄|   4|
 *	 |			|  红|   4|
 *	 |5 		|  绿|   5|
 *	 |			|  黄|   5|
 *	 |			|  红|   5|
 *	 |6 		|  绿|   6|
 *	 |			|  黄|   6|
 *	 |			|  红|   6|
 *	 |7 		|  绿|   7|
 *	 |			|  黄|   7|
 *	 |			|  红|   7|
 *
 * - 命令模式 \n
 *	 命令行配置在优先级映射phb配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建一条内部优先级和颜色到ToS的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个内部优先级和颜色到ToS的映射域PHB
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_queue_to_tos_cmd_vtysh,
		"queue <0-7> {color (green|yellow|red)} to tos <0-7>",
		"Queue\n"
		"Specify queue value\n"
		"Color\n"
		"Green\n"
		"Yellow\n"
		"Red\n"
		"Mapping\n"
		"Type of Service\n"
		"Specify tos value\n")

/**
 * \page no_qos_queue_to_tos_cmd_vtysh no queue <0-7> {color (green|yellow|red)} to tos
 * - 功能说明 \n
 *	 删除内部优先级队列和颜色到ToS的映射
 * - 命令格式 \n
 *	 no queue <0-7> {color (green|yellow|red)} to tos
 * - 参数说明 \n
 *	 |参数	|说明 		   |
 *	 |------|--------------|
 *	 |<0-7> |表示内部优先级队列的有效范围为<0-7>|
 *	 |green |标记报文为绿色|
 *	 |yellow|标记报文为黄色|
 *	 |red	|标记报文为红色|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在优先级映射phb配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除一条内部优先级和颜色到ToS的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个内部优先级和颜色到ToS的映射
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
DEFSH(VTYSH_QOS,
		no_qos_queue_to_tos_cmd_vtysh,
		"no queue <0-7> {color (green|yellow|red)} to tos",
		NO_STR
		"Queue\n"
		"Specify queue value\n"
		"Color\n"
		"Green\n"
		"Yellow\n"
		"Red\n"
		"Mapping\n"
		"Type of service\n")

/**
 * \page qos_queue_to_dscp_cmd_vtysh queue <0-7> {color (green|yellow|red)} to dscp <0-63>
 * - 功能说明 \n
 *	 配置内部优先级队列和颜色到DSCP的映射
 * - 命令格式 \n
 *	 queue <0-7> {color (green|yellow|red)} to dscp <0-7>
 * - 参数说明 \n
 *	 |参数	|说明 		   |
 *	 |------|--------------|
 *	 |<0-7> |表示内部优先级队列的有效范围为<0-7>|
 *	 |green |标记报文为绿色|
 *	 |yellow|标记报文为黄色|
 *	 |red	|标记报文为红色|
 *	 |<0-7> |表示ToS的有效范围为<0-7>		|
 *
 * - 缺省情况 \n
 *	 内部优先级和颜色到DSCP的默认映射情况: \n
 *	 |优先级队列|颜色|DSCP|
 *	 |----------|----|----|
 *   |         0|  绿|    0|
 *   | 		    |  黄|    0|
 *	 | 		    |  红|    0|
 *	 | 		   1|  绿|   10|
 *	 | 		    |  黄|   12|
 *	 | 		    |  红|   14|
 *	 | 		   2|  绿|   18|
 *	 | 		    |  黄|   20|
 *	 | 		    |  红|   22|
 *	 | 		   3|  绿|   26|
 *	 | 		    |  黄|   28|
 *	 | 		    |  红|   30|
 *	 | 		   4|  绿|   34|
 *	 | 		    |  黄|   36|
 *	 | 		    |  红|   38|
 *	 | 		   5|  绿|   46|
 *	 | 		    |  黄|   46|
 *	 | 		    |  红|   46|
 *	 | 		   6|  绿|   48|
 *	 | 		    |  黄|   48|
 *	 | 		    |  红|   48|
 *	 | 		   7|  绿|   56|
 *	 | 		    |  黄|   56|
 *	 |  		|  红|   56|
 *
 * - 命令模式 \n
 *	 命令行配置在优先级映射dscp配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建一条内部优先级和颜色到DSCP的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个内部优先级和颜色到DSCP的映射域PHB
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_queue_to_dscp_cmd_vtysh,
		"queue <0-7> {color (green|yellow|red)} to dscp <0-63>",
		"Queue\n"
		"Specify queue value\n"
		"Color\n"
		"Green\n"
		"Yellow\n"
		"Red\n"
		"Mapping\n"
		"Differentiated Services Code Point\n"
		"Specify dscp value\n")

/**
 * \page no_qos_queue_to_dscp_cmd_vtysh no queue <0-7> {color (green|yellow|red)} to dscp
 * - 功能说明 \n
 *	 删除内部优先级队列和颜色到DSCP的映射
 * - 命令格式 \n
 *	 no queue <0-7> {color (green|yellow|red)} to dscp
 * - 参数说明 \n
 *	 |参数	|说明 		   |
 *	 |------|--------------|
 *	 |<0-7> |表示内部优先级队列的有效范围为<0-7>|
 *	 |green |标记报文为绿色|
 *	 |yellow|标记报文为黄色|
 *	 |red	|标记报文为红色|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令行配置在优先级映射phb配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除一条内部优先级和颜色到DSCP的映射规则，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建一个内部优先级和颜色到DSCP的映射
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
DEFSH(VTYSH_QOS,
		no_qos_queue_to_dscp_cmd_vtysh,
		"no queue <0-7> {color (green|yellow|red)} to dscp",
		NO_STR
		"Queue\n"
		"Specify queue value\n"
		"Color\n"
		"Green\n"
		"Yellow\n"
		"Red\n"
		"Mapping\n"
		"Differentiated Services Code Point\n")

/**
 * \page show_qos_mapping_phb_cmd_vtysh show qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <0-5>
 * - 功能说明 \n
 *	 显示当前QoS mapping phb映射模板的配置情况
 * - 命令格式 \n
 *	 show qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <0-5>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |-----------|--------------|
 *	 |cos-phb |内部优先级到报文CoS域的映射		   |
 *	 |exp-phb |内部优先级MPLS标签到EXP位的映射	  |
 *	 |tos-phb |内部优先级IP首部到ToS字段的映射   |
 *	 |dscp-phb|内部优先级到DSCP的映射|
 *	 |<1-5>   |表示可显示的映射模版的范围为<0-5>|
 *
 * - 缺省情况 \n
 *	 缺省情况下只能显示各个映射phb的默认模板“0”
 * - 命令模式 \n
 *	 命令行配置在全局配置模式和特权模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   显示当前QoS mapping phb映射模板的配置情况，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建优先级映射phb，否则只能显示默认模板信息
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   创建CoS映射域“1” \n
 *	   Hios(config)# qos mapping cos-phb 3 \n
 *	   Hios(config-qos-phb-cos)# queue 7 color green to exp 3 \n
 *     Hios(config-qos-phb-cos)# exit \n
 *     Hios(config)# show qos mapping exp-phb 3 \n
 *     ID: 3 \n
 *	   |优先级队列|颜色|EXP|
 *	   |----------|----|---|
 *	   |0 		  |  绿|   0|
 *	   |		  |  黄|   0|
 *	   |		  |  红|   0|
 *	   |1 		  |  绿|   1|
 *	   |		  |  黄|   1|
 *	   |		  |  红|   1|
 *	   |2 		  |  绿|   2|
 *	   |		  |  黄|   2|
 *	   |		  |  红|   2|
 *	   |3 		  |  绿|   3|
 *	   |		  |  黄|   3|
 *	   |		  |  红|   3|
 *	   |4 		  |  绿|   4|
 *	   |		  |  黄|   4|
 *	   |		  |  红|   4|
 *	   |5 		  |  绿|   5|
 *	   |		  |  黄|   5|
 *	   |		  |  红|   5|
 *	   |6 		  |  绿|   6|
 *	   |		  |  黄|   6|
 *	   |		  |  红|   6|
 *	   |7 		  |  绿|   3|
 *	   |		  |  黄|   7|
 *	   |		  |  红|   7|
 *
 */
DEFSH(VTYSH_QOS,
		show_qos_mapping_phb_cmd_vtysh,
		"show qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <0-5>",
		"Show qos mapping phb infomation\n"
		"Quality of Service\n"
		"QoS mapping\n"
		"Queue to cos mapping\n"
		"Queue to exp mapping\n"
		"Queue to tos mapping\n"
		"Queue to dscp mapping\n"
		"QoS mapping phb ID\n")

/**
 * \page qos_policy_acl_filter_cmd_vtysh qos policy ingress acl <1-65535> filter (permit|deny)
 * - 功能说明 \n
 *	 全局或接口下配置出入方向filter 策略
 * - 命令格式 \n
 *	 qos policy ingress acl <1-65535> filter (permit|deny)
 * - 参数说明 \n
 *	 |参数  |说明|
 *	 |------|------------|
 *	 |(ingress\|egress) |策略方向|
 *   |<1-65535> |group num|
 *   |(permit\|deny) |Filter action|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太物理口、以太子接口、trunk口、vlanif口配置模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或以太物理口、以太子接口、trunk口、vlanif口配置模式，对特定接口或所有接口绑定acl group并配置filter策略
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos policy {acl <1-65535>}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_acl_filter_cmd_vtysh,
       "qos policy ingress acl <1-65535> filter (permit|deny)",
       "Qos\n"
       "Qos policy\n"
       "Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: filter\n"
	   "Permit acl rule filter\n"
	   "Deny acl rule filter\n")

/**
 * \page qos_policy_egress_acl_filter_cmd_vtysh qos policy egress acl <1-65535> filter (permit|deny)
 * - 功能说明 \n
 *	 全局或接口下配置出方向filter 策略
 * - 命令格式 \n
 *	 qos policy egress acl <1-65535> filter (permit|deny)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *   |(ingress\|egress) |策略方向|
 *	 |<1-65535> |group num|
 *   |(permit\|deny) |Filter action|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太物理口、以太子接口、trunk口、vlanif口配置模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或以太物理口、以太子接口、trunk口、vlanif口配置模式，对特定接口或所有接口绑定acl group并配置filter策略
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos policy {acl <1-65535>}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_egress_acl_filter_cmd_vtysh,
       "qos policy egress acl <1-65535> filter (permit|deny)",
       "Qos\n"
       "Qos policy\n"
       "Egress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: filter\n"
	   "Permit acl rule filter\n"
	   "Deny acl rule filter\n")

/**
 * \page qos_policy_acl_car_cmd_vtysh qos policy ingress acl <1-65535> car <1-1000>
 * - 功能说明 \n
 *	 全局或接口下配置入方向car 策略
 * - 命令格式 \n
 *	 qos policy ingress acl <1-65535> car <1-1000>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *   |(ingress\|egress) |策略方向|
 *	 |<1-65535> |group num|
 *   |<1-1000> |car-profile id|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太物理口、以太子接口、trunk口、vlanif口配置模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或接口配置模式，对特定接口或所有接口绑定acl group并配置car策略
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos policy {acl <1-65535>}
 * - 使用举例 \n
 *	   Huahuan(config)# acl 2 \n
 *     Huahuan(config-acl)# rule 5 ip dip 1.2.3.4/24 \n
 *     Huahuan(config-acl)#exit \n
 *     Huahuan(config)# qos car-profile 2 \n
 *     Huahuan(config-qos-car)# cir 100 cbs 200 \n
 *     Huahuan(config)# qos policy ingress acl 2 car 2 \n
 *     Huahuan(config)# interface ethernet 1/1/2 \n
 *     Huahuan(config-ethernet1/1/2)#qos policy ingress acl 2 car 2 \n
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_acl_car_cmd_vtysh,
       "qos policy ingress acl <1-65535> car <1-1000>",
       "Qos\n"
       "Qos policy\n"
       "Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: car\n"
	   "Qos car-profile id <1-1000>\n")

/**
 * \page qos_policy_egress_acl_car_cmd_vtysh qos policy egress acl <1-65535> car <1-1000>
 * - 功能说明 \n
 *	 全局或接口下配置出方向car 策略
 * - 命令格式 \n
 *	 qos policy egress acl <1-65535> car <1-1000>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |(ingress\|egress) |策略方向|
 *	 |<1-65535> |group num|
 *	 |<1-1000> |car-profile id|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太物理口、以太子接口、trunk口、vlanif口配置模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或接口配置模式，对特定接口或所有接口绑定acl group并配置car策略
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos policy {acl <1-65535>}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_egress_acl_car_cmd_vtysh,
       "qos policy egress acl <1-65535> car <1-1000>",
       "Qos\n"
       "Qos policy\n"
       "Egress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: car\n"
	   "Qos car-profile id <1-1000>\n")

/**
 * \page qos_policy_acl_mirror_eth_cmd_vtysh qos policy ingress acl <1-65535> mirror-to interface ethernet USP
 * - 功能说明 \n
 *	 全局或接口下配置入方向重定向策略
 * - 命令格式 \n
 *	 qos policy ingress acl <1-65535> mirror-to interface ethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535> |group num|
 *	 |USP|Interface gigabitethernet usp|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太物理口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或接口配置模式，对特定接口或所有接口绑定acl group并配置流镜像到指定接口
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show acl <1-65535> {rule <1-65535>}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_acl_mirror_eth_cmd_vtysh,
       "qos policy ingress acl <1-65535> mirror-to interface ethernet USP",
       "Qos\n"
       "Qos policy\n"
       "Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: mirror-to\n"
	    CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR)

/**
 * \page qos_policy_acl_mirror_gigabit_eth_cmd_vtysh qos policy ingress acl <1-65535> mirror-to interface gigabitethernet USP
 * - 功能说明 \n
 *	 全局或接口下配置入方向重定向策略
 * - 命令格式 \n
 *	 qos policy ingress acl <1-65535> mirror-to interface gigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535> |group num|
 *	 |USP|Interface gigabitethernet usp|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太物理口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或接口配置模式，对特定接口或所有接口绑定acl group并配置流镜像到指定接口
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show acl <1-65535> {rule <1-65535>}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_acl_mirror_gigabit_eth_cmd_vtysh,
	   "qos policy ingress acl <1-65535> mirror-to interface gigabitethernet USP",
	   "Qos\n"
	   "Qos policy\n"
	   "Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)
/**
 * \page qos_policy_acl_mirror_xgigabit_eth_cmd_vtysh qos policy ingress acl <1-65535> mirror-to interface xgigabitethernet USP
 * - 功能说明 \n
 *	 全局或接口下配置入方向重定向策略
 * - 命令格式 \n
 *	 qos policy ingress acl <1-65535> mirror-to interface xgigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535> |group num|
 *	 |USP|Interface xgigabitethernet usp|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太物理口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或接口配置模式，对特定接口或所有接口绑定acl group并配置流镜像到指定接口
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show acl <1-65535> {rule <1-65535>}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_acl_mirror_xgigabit_eth_cmd_vtysh,
       "qos policy ingress acl <1-65535> mirror-to interface xgigabitethernet USP",
       "Qos\n"
       "Qos policy\n"
       "Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: mirror-to\n"
	    CLI_INTERFACE_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)

/**
 * \page qos_policy_acl_mirror_trunk_cmd_vtysh qos policy ingress acl <1-65535> mirror-to interface trunk TRUNK
 * - 功能说明 \n
 *	 全局或接口下配置入方向重定向策略
 * - 命令格式 \n
 *	 qos policy ingress acl <1-65535> mirror-to interface trunk TRUNK
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *	 |TRUNK|Interface trunk TRUNK|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以trunk口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或接口配置模式，对特定接口或所有接口绑定acl group并配置流镜像到指定接口
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show acl <1-65535> {rule <1-65535>}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_acl_mirror_trunk_cmd_vtysh,
       "qos policy ingress acl <1-65535> mirror-to interface trunk TRUNK",
       "Qos\n"
       "Qos policy\n"
       "Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: mirror-to\n"
	    CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR)

/**
 * \page qos_policy_acl_redirect_eth_cmd_vtysh qos policy ingress acl <1-65535> redirect-to interface ethernet USP
 * - 功能说明 \n
 *	 全局或接口下配置入方向重定向策略
 * - 命令格式 \n
 *	 qos policy ingress acl <1-65535> redirect-to interface ethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *	 |USP|Interface usp or subusp|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或接口配置模式，对特定接口或所有接口绑定acl group并配置流镜像到指定接口
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos policy
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_acl_redirect_eth_cmd_vtysh,
       "qos policy ingress acl <1-65535> redirect-to interface ethernet USP",
       "Qos\n"
       "Qos policy\n"
       "Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: redirect-to\n"
	  	CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR)
/**
 * \page qos_policy_acl_redirect_gigabit_eth_cmd_vtysh qos policy ingress acl <1-65535> redirect-to interface gigabitethernet USP
 * - 功能说明 \n
 *	 全局或接口下配置入方向重定向策略
 * - 命令格式 \n
 *	 qos policy ingress acl <1-65535> redirect-to interface gigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *	 |USP|Interface usp or subusp|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或接口配置模式，对特定接口或所有接口绑定acl group并配置流镜像到指定接口
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos policy
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_acl_redirect_gigabit_eth_cmd_vtysh,
	   "qos policy ingress acl <1-65535> redirect-to interface gigabitethernet USP",
	   "Qos\n"
	   "Qos policy\n"
	   "Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: redirect-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)

/**
 * \page qos_policy_acl_redirect_xgigabit_eth_cmd_vtysh qos policy ingress acl <1-65535> redirect-to interface xgigabitethernet USP
 * - 功能说明 \n
 *	 全局或接口下配置入方向重定向策略
 * - 命令格式 \n
 *	 qos policy ingress acl <1-65535> redirect-to interface xgigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *	 |USP|Interface usp or subusp|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或接口配置模式，对特定接口或所有接口绑定acl group并配置流镜像到指定接口
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos policy
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_acl_redirect_xgigabit_eth_cmd_vtysh,
       "qos policy ingress acl <1-65535> redirect-to interface xgigabitethernet USP",
       "Qos\n"
       "Qos policy\n"
       "Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: redirect-to\n"
	  	CLI_INTERFACE_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)

/**
 * \page qos_policy_acl_redirect_trunk_cmd_vtysh qos policy ingress acl <1-65535> redirect-to interface trunk TRUNK
 * - 功能说明 \n
 *	 全局或接口下配置入方向重定向策略
 * - 命令格式 \n
 *	 qos policy ingress acl <1-65535> redirect-to interface trunk TRUNK
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *	 |TRUNK|Interface trunk TRUNK|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或trunk口模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或接口配置模式，对特定接口或所有接口绑定acl group并配置流镜像到指定接口
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos policy
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_acl_redirect_trunk_cmd_vtysh,
       "qos policy ingress acl <1-65535> redirect-to interface trunk TRUNK",
       "Qos\n"
       "Qos policy\n"
       "Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: redirect-to\n"
	  	CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR)

/**
 * \page qos_policy_acl_queue_cmd_vtysh qos policy ingress acl <1-65535> qos-mapping queue <0-7>
 * - 功能说明 \n
 *	 全局或接口下配置qos-mapping策略
 * - 命令格式 \n
 *	 qos policy ingress acl <1-65535> qos-mapping queue <0-7>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *	 |<0-7>|queue priority|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太物理口、以太子接口、trunk口、vlanif口配置模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于CONFIG模式或接口配置模式，对特定接口或所有接口绑定acl group并qos-mapping到队列优先级
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos policy
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_policy_acl_queue_cmd_vtysh,
       "qos policy ingress acl <1-65535> qos-mapping queue <0-7>",
       "Qos\n"
       "Qos policy\n"
       "Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: qos-mapping\n"
	   "Qos mapping to queue\n"
	   "Queue  number <0-7>\n")

/**
 * \page no_qos_policy_ingress_acl_cmd_vtysh no qos policy ingress acl <1-65535> ( filter|mirror-to|redirect-to|qos-mapping|car)
 * - 功能说明 \n
 *	 删除 qos 策略
 * - 命令格式 \n
 *	 no qos policy ingress acl <1-65535> ( filter|mirror-to|redirect-to|qos-mapping|car)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *	 |{ filter \| mirror-to \| redirect-to \| qos-mapping \| car}|queue priority|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太物理口、以太子接口、trunk口、vlanif口配置模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   CONFIG模式或接口配置模式下，用于删除 qos 策略
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos policy {acl <1-65535>}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		no_qos_policy_ingress_acl_cmd_vtysh,
       "no qos policy ingress acl <1-65535> ( filter|mirror-to|redirect-to|qos-mapping|car)",
       NO_STR
		"Qos\n"
		"Qos policy\n"
		"Ingress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: filter\n"
	   "Acl action: mirror\n"
	   "Acl action: redirect\n"
	   "Acl action: qos-mapping\n"
	   "Acl action: car\n")

/**
 * \page no_qos_policy_egress_acl_cmd_vtysh no qos policy egress acl <1-65535> ( filter|car)
 * - 功能说明 \n
 *	 删除 qos 策略
 * - 命令格式 \n
 *	 no qos policy egress acl <1-65535> ( filter|car)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *	 |{ filter\| car}|queue priority|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或以太物理口、以太子接口、trunk口、vlanif口配置模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   CONFIG模式或接口配置模式下，用于删除 qos 策略
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos policy {acl <1-65535>}
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		no_qos_policy_egress_acl_cmd_vtysh,
       "no qos policy egress acl <1-65535> ( filter|car)",
       NO_STR
		"Qos\n"
		"Qos policy\n"
	   "Egress\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n"
	   "Acl action: filter\n"
	   "Acl action: car\n")

/**
 * \page show_qos_policy_acl_cmd_vtysh show qos policy {acl <1-65535>}
 * - 功能说明 \n
 *	 显示应用于全局qos policy, 显示接口和全局所有policy
 * - 命令格式 \n
 *	 show qos policy {acl <1-65535>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或接口配置模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   配置在CONFIG模式下为显示全局及接口应用的策略
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show qos policy {acl <1-65535>}
 * - 使用举例 \n
 *	   show acl
 *
 */
DEFSH(VTYSH_QOS,
		show_qos_policy_acl_cmd_vtysh,
       "show qos policy {acl <1-65535>} ",
       "show qos policy\n"
		"Qos\n"
		"Qos policy\n"
	   "Acl rule\n"
	   "Acl number <1-65535>\n")
#if 0
/* debug ÈÕÖ¾·¢ËÍµ½ syslog Ê¹ÄÜ×´Ì¬ÉèÖÃ */
DEFSH (VTYSH_QOS,qos_log_level_ctl_cmd_vtysh,"debug qos(enable | disable)",
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n")

/* debug ÈÕÖ¾·¢ËÍµ½ syslog Ê¹ÄÜ×´Ì¬ÏÔÊ¾ */
DEFSH (VTYSH_QOS,qos_show_log_level_ctl_cmd_vtysh,	"show debug qos",
	SHOW_STR"Output log of debug level\n""Program name\n")
#endif
/**
 * \page no_qos_car_cmd_vtysh no qos car-profile <1-1000>
 * - 功能说明 \n
 *	 删除该CAR模板
 * - 命令格式 \n
 *	 no qos car-profile <1-1000>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-65535>|group num|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置
 * - 命令模式 \n
 *	 CONFIG模式或接口配置模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除QoS CAR模板，用于QoS
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   show acl
 *
 */
DEFSH(VTYSH_QOS,
		no_qos_car_cmd_vtysh,
		"no qos car-profile <1-1000>",
		NO_STR
		"Quality of Service\n"
		"QoS car-profile\n"
		"QoS car-profile ID\n")

/**
 * \page qos_car_rate_limit_cmd_vtysh cir <0-10000000> cbs <0-10000000> {pir <0-10000000> pbs <0-10000000>}
 * - 功能说明 \n
 *	 CAR模板视图下配置具体的限速参数
 * - 命令格式 \n
 *	 cir <0-10000000> cbs <0-10000000> {pir <0-10000000> pbs <0-10000000>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<0-10000000>|可配置的cir范围是<0-10000000>kbps|
 *   |<0-10000000>|可配置的cbs范围是<0-10000000>kb|
 *   |<0-10000000>|可配置的pir范围是<0-10000000>kbps|
 *   |<0-10000000>|可配置的pbs范围是<0-10000000>kb|
 *
 * - 缺省情况 \n
 *	 Pir和pbs参数未做配置时，默认pir等于cir，pbs为512kb
 * - 命令模式 \n
 *	 命令配置在CAR模板配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置QoS CAR模板的限速参数，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建CAR模板
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# qos car-profile 1 \n
 *     Huahuan(config-qos-car)# cir 50000 cbs 200 pir 80000 pbs 300 \n
 *
 */
DEFSH(VTYSH_QOS,
		qos_car_rate_limit_cmd_vtysh,
		"cir <0-10000000> cbs <0-10000000> {pir <0-10000000> pbs <0-10000000>}",
		"Committed information rate\n"
		"Committed information rate value <0-10000000> (Unit: Kbps)\n"
		"Committed burst size\n"
		"Committed burst size <0-10000000> (Unit: Kb)\n"
		"Peak information rate\n"
		"Peak information rate value <0-10000000> (Unit: Kbps)\n"
		"Peak burst size\n"
		"Peak burst size <0-10000000> (Unit: Kb)\n")

/**
 * \page qos_car_color_action_cmd_vtysh color (green | yellow | red) (discard | pass) {remark-cos <0-7>| remark-dscp <0-63> | queue <0-7>}
 * - 功能说明 \n
 *	 在CAR模板视图下，配置着色报文的动作
 * - 命令格式 \n
 *	 color (green | yellow | red) (discard | pass) {remark-cos <0-7>| remark-dscp <0-63> | queue <0-7>}
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |Green|选择被标记为绿色的报文|
 *	 |Red|选择被标记为红色的报文|
 *	 |Yellow|选择被标记为黄色的报文|
 *	 |Discard|配置报文动作为丢弃|
 *   |pass|配置报文动作为转发|
 *   |<0-7>|重标记指定颜色报文的cos|
 *   |<0-63>|重标记指定颜色报文的dscp|
 *   |<0-7>|重标记指定颜色报文的queue|
 *
 * - 缺省情况 \n
 *	 缺省情况下，绿色和黄色报文会被转发，红色报文被丢弃
 * - 命令模式 \n
 *	 命令配置在CAR模板配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置QoS CAR模板下着色报文的动作，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建CAR模板
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# qos car-profile 1 \n
 *     Huahuan(config-qos-car)# cir 50000 cbs 200 pir 80000 pbs 300 \n
 *     Huahuan(config-qos-car)# color yellow discard \n
 *
 */
DEFSH(VTYSH_QOS,
        qos_car_color_action_cmd_vtysh,
        "color (green | yellow | red) (discard | pass) {remark-cos <0-7>| remark-dscp <0-63> | queue <0-7>}",
        "Packet color\n"
        "Green packets\n"
        "Red packets\n"
        "Yellow packets\n"
        "Discard packets\n"
        "Forward packets\n"
        "Remark pass packet cos\n"
        "cos value"
        "Remark pass packet dscp\n"
        "dscp value"
        "Remark packet queue\n"
        "queue value")

/**
 * \page qos_car_color_mode_cmd_vtysh color blind
 * - 功能说明 \n
 *	 配置QoS CAR模板的染色模式为色盲模式
 * - 命令格式 \n
 *	 color blind
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *	 缺省情况下，模板被配置为色敏感模式
 * - 命令模式 \n
 *	 命令配置在CAR模板视图模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置QoS CAR模板的染色模式，用于QoS
 *	 - 前置条件 \n
 *	   需要先创建car模板
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# qos car-profile 1
 *     Huahuan(config-qos-car)# cir 50000 cbs 200 pir 80000 pbs 300
 *     Huahuan(config-qos-car)# color blind
 *
 */
DEFSH(VTYSH_QOS,
		qos_car_color_mode_cmd_vtysh,
		"color blind",
		"Configure qos car color mode\n"
		"color-blind mode\n")

/**
 * \page no_qos_car_color_action_cmd_vtysh no color (green|red|yellow|blind)
 * - 功能说明 \n
 *	 CAR模板视图下，恢复着色报文的默认动作或恢复颜色模式
 * - 命令格式 \n
 *	 no color (green|red|yellow|blind)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |Green|选择恢复绿色报文默认动作|
 *   |Red|选择恢复红色报文默认动作|
 *   |Yellow|选择恢复黄色报文默认动作|
 *   |Blind|选择恢复色敏感模式|
 *
 * - 缺省情况 \n
 *	 缺省情况下，绿色和黄色报文被转发，红色报文被丢弃，模板使用色敏感模式
 * - 命令模式 \n
 *	 命令配置在CAR模板视图模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令恢复QoS CAR模板着色报文的默认动作和颜色模式，用于QoS
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# qos car-profile 1 \n
 *     Huahuan(config-qos-car)# color yellow discard \n
 *     Huahuan(config-qos-car)# no color yellow \n
 *
 */
DEFSH(VTYSH_QOS,
		no_qos_car_color_action_cmd_vtysh,
		"no color (green | red | yellow| blind)",
		NO_STR
		"Packet color\n"
		"Green packets\n"
		"Red packets\n"
		"Yellow packets\n"
		"color-blind mode\n")

/**
 * \page show_qos_car_cmd_vtysh show qos car-profile [<1-1000>]
 * - 功能说明 \n
 *	 显示配置的car模板信息
 * - 命令格式 \n
 *	 show qos car-profile [<1-1000>]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-1000>|指定要显示的模板ID，参数可选，参数不选时显示所有已创建的car模板|
 *
 * - 缺省情况 \n
 *	 Car模板的缺省情况为：绿色和黄色着色报文默认转发，黄色报文默认丢弃。染色模式默认使用色敏感模式
 * - 命令模式 \n
 *	 命令配置在全局配配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示配置的car模板，用于QoS限速
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
DEFSH(VTYSH_QOS,
		show_qos_car_cmd_vtysh,
		"show qos car-profile [<1-1000>]",
		SHOW_STR
		"Quality of Service\n"
		"QoS car-profile\n"
		"QoS car-profile ID\n")


/**
 * \page qos_car_apply_to_if_cmd_vtysh qos car (ingress|egress) profile <1-1000>
 * - 功能说明 \n
 *	 在接口的入方向或出方向应用CAR模板，多个接口应用同一个模板时，共享模板的限速值。接口的出方向和入方向不允许引用同一个CAR模板
 * - 命令格式 \n
 *	 qos car (ingress|egress) profile <1-1000>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |Ingress|将CAR模板应用到接口的入方向|
 *   |Egress|将CAR模板应用到接口的出方向|
 *   |<1-1000>|可应用的模板范围为<1-1000>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口上没有应用任何CAR参数
 * - 命令模式 \n
 *	 命令配置在以太物理接口和物理子接口视图下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令将已经创建并配置的CAR模板应用到接口的出方向或入方向，用于QoS接口上流量监管
 *	 - 前置条件 \n
 *	   CAR模板需要提前创建，并一定要配置具体限速参数
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   本命令与qos car (ingress | egress) cir <0-10000000> cbs <0-512> {pir <1-10000000> pbs <0-512>}冲突，在接口的一个方向上，只能选择一种进行配置
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   无
 *
 */
DEFSH(VTYSH_QOS,
		qos_car_apply_to_if_cmd_vtysh,
		"qos car (ingress | egress) profile <1-1000>",
		"Quality of Service\n"
		"Committed Access Rate\n"
		"Ingress\n"
		"Egress\n"
		"QoS car-profile\n"
		"QoS car-profile ID\n")

/**
 * \page no_qos_car_apply_to_if_cmd_vtysh no qos car (ingress|egress)
 * - 功能说明 \n
 *	 删除接口上指定方向上的CAR配置
 * - 命令格式 \n
 *	 no qos car (ingress|egress)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |Ingress|指定删除接口入方向上的car配置|
 *	 |Egress|指定删除接口出方向上的car配置|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 命令配置在接口视图模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除接口上的CAR配置，用于QoS删除接口上的限速
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# interface ethernet 1/1/1 \n
 *     Huahuan(config-ethernet1/1/1)# no qos car ingress \n
 *     Huahuan(config-ethernet1/1/1)# no qos car egress \n
 *
 */
DEFSH(VTYSH_QOS,
		no_qos_car_apply_to_if_cmd_vtysh,
		"no qos car (ingress | egress)",
		NO_STR
		"Quality of Service\n"
		"Committed Access Rate\n"
		"Ingress\n"
		"Egress\n")

/**
 * \page qos_lr_config_if_cmd_vtysh qos lr (ingress | egress) cir <0-10000000> cbs <0-16000>
 * - 功能说明 \n
 *	 在接口下直接配置限速参数
 * - 命令格式 \n
 *	 qos lr (ingress | egress) cir <0-10000000> cbs <0-16000>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |Ingress|指定在接口入方向配置限速参数|
 *	 |Egress|指定在接口出方向配置限速参数|
 *   |<0-10000000>|Cir配置范围为<0-10000000> kbps|
 *   |<0-16000>|Cbs配置范围为<0-16000>kb|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口下没有配置任何car参数
 * - 命令模式 \n
 *	 命令配置在物理接口视图模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令接口上的CAR参数，用于QoS接口上流量监管
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   本命令与qos car (ingress | egress) profile <1-1000> 冲突，在接口的一个方向上只能选择一种进行配置
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# interface ethernet 1/1/1 \n
 *     Huahuan(config-ethernet1/1/1)# qos car ingress cir 50000 cbs 200 \n
 *     Huahuan(config-ethernet1/1/1)# qos car egress cir 50000 cbs 200 \n
 *
 */
DEFSH(VTYSH_QOS,
		qos_lr_config_if_cmd_vtysh,
		"qos lr (ingress | egress) cir <0-10000000> cbs <0-16000>",
		"Quality of Service\n"
		"Limit Rate\n"
		"Ingress\n"
		"Egress\n"
		"Committed information rate\n"
		"Committed information rate value <0-10000000> (Unit: Kbps)\n"
		"Committed burst size\n"
		"Committed burst size <0-16000> (Unit: Kb)\n")

/**
 * \page no_qos_lr_config_if_cmd_vtysh no qos lr (ingress | egress)
 * - 功能说明 \n
 *   删除物理接口限速
 * - 命令格式 \n
 *   no qos lr (ingress | egress)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|------------|
 *   |Ingress|指定在接口入方向配置限速参数|
 *   |Egress|指定在接口出方向配置限速参数|
 *
 * - 缺省情况 \n
 *   缺省情况下，接口下没有配置任何car参数
 * - 命令模式 \n
 *   命令配置在物理接口视图模式下
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     使用该命令删除物理接口上的流量监管参数
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *     Huahuan(config)# interface ethernet 1/1/1 \n
 *     Huahuan(config-ethernet1/1/1)# no qos lr ingress \n
 *     Huahuan(config-ethernet1/1/1)# no qos lr egress \n
 *
 */
DEFSH(VTYSH_QOS,
        no_qos_lr_config_if_cmd_vtysh,
        "no qos lr (ingress | egress)",
        NO_STR
        "Quality of Service\n"
        "Limit Rate\n"
        "Ingress\n"
        "Egress\n")        
		
/**
 * \page qos_car_config_if_cmd_vtysh qos car (ingress | egress) cir <0-10000000> cbs <0-10000000> pir <0-10000000> pbs <0-10000000>...
 * - 功能说明 \n
 *	 在接口下直接配置限速参数
 * - 命令格式 \n
 *	 qos car (ingress | egress) cir <0-10000000> cbs <0-10000000> pir <0-10000000> pbs <0-10000000> (color-blind | color-aware)\n
 *   {green-remark-cos <0-7> | green-remark-dscp <0-63> | green-queue <0-7> | green-drop | \n
 *	 yellow-remark-cos <0-7> | yellow-remark-dscp <0-63> | yellow-queue <0-7> | yellow-drop |\n 
 *	 red-remark-cos <0-7> | red-remark-dscp <0-63> | red-queue <0-7> | red-pass} 
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |Ingress|指定在接口入方向配置限速参数|
 *	 |Egress|指定在接口出方向配置限速参数|
 *	 |<0-10000000>|Cir配置范围为<0-10000000> kbps|
 *	 |<0-10000000>|Cbs配置范围为<0-10000000> kbps|
 *	 |<0-10000000>|Pir配置范围为<0-10000000> kbps|
 *	 |<0-10000000>|Pbs配置范围为<0-10000000> kbps|
 *   |color-blind|色盲模式|
 *   |color-aware|感色模式|
 *   |<0-7>|重标记绿色报文cos为<0-7>|
 *   |<0-63>|重标记绿色报文dscp为<0-63>|
 *   |<0-7>|重标记绿色报文queue为<0-7>|
 *   |green-drop|配置丢弃绿色报文|
 *   |<0-7>|重标记绿色报文cos为<0-7>|
 *   |<0-63>|重标记绿色报文dscp为<0-63>|
 *   |<0-7>|重标记绿色报文queue为<0-7>|
 *   |yellow-drop|配置丢弃绿色报文|
 *   |<0-7>|重标记绿色报文cos为<0-7>|
 *   |<0-63>|重标记绿色报文dscp为<0-63>|
 *   |<0-7>|重标记绿色报文queue为<0-7>|
 *   |red-pass|配置允许红色报文转发|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口下没有配置任何car参数
 * - 命令模式 \n
 *	 命令配置在物理接口和物理子接口视图模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令接口上的CAR参数，用于QoS接口上流量监管
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   本命令与qos car (ingress|egress) profile <1-1000> 冲突，在接口的一个方向上只能选择一种进行配置
 *	 - 相关命令 \n
 *	   无
 * - 使用举例 \n
 *	   Huahuan(config)# interface ethernet 1/1/1 \n
 *	   Huahuan(config-ethernet1/1/1)# qos car ingress cir 50000 cbs 200 pir 80000 pbs 300 \n
 *     Huahuan(config-ethernet1/1/1)# qos car egress cir 50000 cbs 200 pir 80000 pbs 300 \n
 *
 */
DEFSH(VTYSH_QOS,
		qos_car_config_if_cmd_vtysh,
		"qos car (ingress | egress) cir <0-10000000> cbs <0-10000000> pir <0-10000000> pbs <0-10000000> (color-blind | color-aware) "
		"{green-remark-cos <0-7> | green-remark-dscp <0-63> | green-queue <0-7> | green-drop | "
		"yellow-remark-cos <0-7> | yellow-remark-dscp <0-63> | yellow-queue <0-7> | yellow-drop | "
		"red-remark-cos <0-7> | red-remark-dscp <0-63> | red-queue <0-7> | red-pass}",
		"Quality of Service\n"
		"Committed Access Rate\n"
		"Ingress\n"
		"Egress\n"
		"Committed information rate\n"
		"Committed information rate value <0-10000000> (Unit: Kbps)\n"
		"Committed burst size\n"
		"Committed burst size <0-10000000> (Unit: Kb)\n"
		"Peak information rate(Invalide for physical interface)\n"
		"Peak information rate value <0-10000000> (Unit: Kbps)\n"
		"Peak burst size(Invalide for physical interface)\n"
		"Peak burst size <0-10000000> (Unit: Kb)\n"
		"Color-blind mode\n"
		"Color-aware mode\n"
		"Remark green-packets cos\n"
		"Remark green-packets cos value <0-7>\n"
		"Remark green-packets dscp\n"
		"Remark green-packets dscp value <0-63>\n"
		"Remark green-packets queue\n"
		"Remark green-packets queue value <0-7>\n"
		"Drop green-packets\n"
		"Remark yellow-packets cos\n"
		"Remark yellow-packets cos value <0-7>\n"
		"Remark yellow-packets dscp\n"
		"Remark yellow-packets dscp value <0-63>\n"
		"Remark yellow-packets queue\n"
		"Remark yellow-packets queue value <0-7>\n"
		"Drop yellow-packets\n"
		"Remark red-packets cos\n"
		"Remark red-packets cos value <0-7>\n"
		"Remark red-packets dscp\n"
		"Remark red-packets dscp value <0-63>\n"
		"Remark red-packets queue\n"
		"Remark red-packets queue value <0-7>\n"
		"Forward red-packets\n")				

DEFSH (VTYSH_QOS,
		show_qos_interface_cmd_vtysh,
		"show qos interface",
		SHOW_STR
		"Qos\n"
		"Qos interface information\n")

/**
 * \page qos_mirror_group_cmd mirror-group <1-3>
 * - 功能说明 \n
 *	 mirror group创建及进入mirror group配置模式
 * - 命令格式 \n
 *	 mirror-group <1-3>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |-----|--------------|
 *   |<1-3>|Mirror group num|
 *
 * - 缺省情况 \n
 *	 缺省情况下，mirror group 未创建，配置命令行mirror-group <1-3>创建并进入mirror group配置模式下
 * - 命令模式 \n
 *	 config 模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   无
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group
 * - 使用举例 \n
 *	   Hios(config)# mirror-group 2 \n
 *     Hios(config-mirror)# mirror-to interface gigabitethernet 1/1/5 \n
 *     Hios(config-mirror)# exit \n
 *
 */
DEFUNSH (VTYSH_QOS,
		qos_mirror_group,
		qos_mirror_group_cmd,
		"mirror-group <1-3>",
		"Qos mirror group\n"
		"Qos mirror group id\n")
{
	vty->node = QOS_MIRROR_NODE;
	return CMD_SUCCESS;
}

/**
 * \page no_qos_mirror_group_cmd_vtysh no mirror-group <1-3>
 * - 功能说明 \n
 *	 删除mirror group
 * - 命令格式 \n
 *	 no mirror-group <1-3>
 * - 参数说明 \n
 *	 |参数   |说明	        	|
 *	 |-----|----------------|
 *	 |<1-3>|Mirror group num|
 *
 * - 缺省情况 \n
 *	 无
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   无
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group
 * - 使用举例 \n
 *	   Hios(config)# mirror-group 2 \n
 *     Hios(config-mirror)# exit \n
 *	   Hios(config)# no mirror-group 2 \n
 *
 */
DEFSH (VTYSH_QOS,
		no_qos_mirror_group_cmd_vtysh,
		"no mirror-group <1-3>",
		NO_STR
		"Qos mirror group\n"
		"Qos mirror group id\n")

/**
 * \page qos_mirror_to_group_cmd_vtysh mirror-to group <1-3> direction (ingress|egress|all)
 * - 功能说明 \n
 *	 接口模式下配置 mirror_to group及镜像口的镜像方向
 * - 命令格式 \n
 *	 mirror-to group <1-3> direction (ingress|egress|all)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<1-3>|Mirror group num|
 *	 |{all\|egress\|ingress}|表示镜像方向，all:双向，egress:出方向,ingress:入方向|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口下未配置mirror
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于接口配置模式下，使用mirror_to group<1-3> direction{all|egress|ingress}配置接口根据镜像方向镜像到监视口 group。接口下mirror_to group 和mirror_to interface不能同时配置。Mirror 方向为双向、入方向、出方向
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group
 *     show mirror interface
 *     no mirror-to group
 * - 使用举例 \n
 *	    Hios(config)# mirror-group 2 \n
 *      Hios(config-mirror)# mirror-to interface gigabitethernet 1/1/5 \n
 *      Hios(config-mirror)# exit \n
 *      Hios(config)# interface gigabitethernet 1/1/6 \n
 *      Hios(config-gigabitethernet1/1/6)# mirror-to group 2 direction all \n
 *
 */
DEFSH(VTYSH_QOS,
		qos_mirror_to_group_cmd_vtysh,
		"mirror-to group <1-3> direction (ingress|egress|all)",
		"Qos mirror-to\n"
		"Qos mirror group\n"
		"Qos mirror group id\n"
		"Direct\n"
        "Ingress direction\n"
        "Egress direction\n"
        "All direction\n")

/**
 * \page no_qos_mirror_to_group_cmd_vtysh no mirror-to group
 * - 功能说明 \n
 *	 配置在接口模式下。接口下删除镜像的监视口
 * - 命令格式 \n
 *	 no mirror-to group
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于接口配置模式下，使用no mirror-to group删除接口下mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   mirror-to group <1-3> direction {all|egress|ingress} \n
 * - 使用举例 \n
 *		Hios(config)# mirror-group 2 \n
 *		Hios(config-mirror)# mirror-to interface gigabitethernet 1/1/5 \n
 *		Hios(config-mirror)# exit \n
 *		Hios(config)# interface gigabitethernet 1/1/6 \n
 *		Hios(config-gigabitethernet1/1/6)# mirror-to group 2 direction all \n
 *      Hios(config-gigabitethernet1/1/6)# no mirror-to group \n
 */
DEFSH(VTYSH_QOS,
		no_qos_mirror_to_group_cmd_vtysh,
		"no mirror-to group",
		NO_STR
		"Qos mirror-to\n"
		"Qos mirror group\n")

/**
 * \page qos_mirror_to_ethernet_cmd_vtysh mirror-to interface ethernet USP
 * - 功能说明 \n
 *	 配置在group模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface ethernet USP
 * - 参数说明 \n
 *	 |参数    |说明			  |
 *	 |------|--------------|
 *	 |USP   |USP:表示ethernet逻辑端口号，格式：<0-7>/<0-31>/<1-255>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 group配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于group配置模式下，使用mirror-to interface ethernet USP添加接口下mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface ethernet USP \n
 * - 使用举例 \n
 *	   Hios(config)# mirror-group 2 \n
 *	   Hios(config-mirror)# mirror-to interface ethernet 1/1/5 \n
 *	   Hios(config-mirror)# exit \n
 *	   Hios(config)# interface ethernet 1/1/6 \n
 *	   Hios(config-ethernet1/1/6)# mirror-to group 2 direction all \n
 *	   Hios(config-ethernet1/1/6)# no mirror-to group \n
 */

DEFSH(VTYSH_QOS,
		qos_mirror_to_ethernet_cmd_vtysh,
		"mirror-to interface ethernet USP",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
        "The port of ethernet, format: <0-7>/<0-31>/<1-255>\n")
 /**
 * \page qos_mirror_to_gigabit_ethernet_cmd_vtysh mirror-to interface gigabitethernet USP
 * - 功能说明 \n
 *	 配置在group模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface gigabitethernet USP
 * - 参数说明 \n
 *	 |参数    |说明		       |
 *	 |------|--------------|
 *	 |USP   |USP:表示gigabitethernet逻辑端口号，格式：<0-7>/<0-31>/<1-255>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 group配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于group配置模式下，使用mirror-to interface gigabitethernet USP添加接口下mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface gigabitethernet USP \n
 * - 使用举例 \n
 *		Hios(config)# mirror-group 2 \n
 *		Hios(config-mirror)# mirror-to interface gigabitethernet 1/1/5 \n
 *		Hios(config-mirror)# exit \n
 *		Hios(config)# interface gigabitethernet 1/1/6 \n
 *		Hios(config-gigabitethernet1/1/6)# mirror-to group 2 direction all \n
 *      Hios(config-gigabitethernet1/1/6)# no mirror-to group \n
 */
DEFSH(VTYSH_QOS,
		qos_mirror_to_gigabit_ethernet_cmd_vtysh,
		"mirror-to interface gigabitethernet USP",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
        "The port of gigabitethernet, format: <0-7>/<0-31>/<1-255>\n")
/**
 * \page qos_mirror_to_xgigabit_ethernet_cmd_vtysh mirror-to interface xgigabitethernet USP
 * - 功能说明 \n
 *	 配置在group模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface xgigabitethernet USP
 * - 参数说明 \n
 *	 |参数	|说明 		   |
 *	 |------|--------------|
 *	 |USP	|USP:表示xgigabitethernet逻辑端口号，格式：<0-7>/<0-31>/<1-255>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 group配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于group配置模式下，使用mirror-to interface xgigabitethernet USP添加接口下mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface xgigabitethernet USP \n
 * - 使用举例 \n
 *		Hios(config)# mirror-group 2 \n
 *		Hios(config-mirror)# mirror-to interface xgigabitethernet 1/1/5 \n
 *		Hios(config-mirror)# exit \n
 *		Hios(config)# interface xgigabitethernet 1/1/6 \n
 *		Hios(config-xgigabitethernet1/1/6)# mirror-to group 2 direction all \n
 *		Hios(config-xgigabitethernet1/1/6)# no mirror-to group \n
 */

DEFSH(VTYSH_QOS,
		qos_mirror_to_xgigabit_ethernet_cmd_vtysh,
		"mirror-to interface xgigabitethernet USP",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        "The port of xgigabitethernet, format: <0-7>/<0-31>/<1-255>\n")

/**
 * \page qos_mirror_to_trunk_cmd_vtysh mirror-to interface trunk TRUNK
 * - 功能说明 \n
 *	 配置在group模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface trunk TRUNK
 * - 参数说明 \n
 *	 |参数	|说明 		   |
 *	 |------|--------------|
 *	 |TRUNK	|TRUNK：表示trunk端口号，格式：<1-128>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 group配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于group配置模式下，使用mirror-to interface trunk TRUNK添加接口下mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface trunk TRUNK \n
 * - 使用举例 \n
 *		Hios(config)# mirror-group 2 \n
 *		Hios(config-mirror)# mirror-to interface trunk 2 \n
 *		Hios(config-mirror)# exit \n
 *		Hios (config)# interface trunk 2 \n
 *		Hios(config-trunk2)# mirror-to group 2 direction all \n
 *		Hios(config-trunk2)# no mirror-to group \n
 */
DEFSH(VTYSH_QOS,
		qos_mirror_to_trunk_cmd_vtysh,
		"mirror-to interface trunk TRUNK",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n")

/**
 * \page qos_mirror_to_loopback_cmd_vtysh mirror-to interface loopback 0
 * - 功能说明 \n
 *	 配置在group模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface loopback 0
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 group配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于group配置模式下，使用mirror-to interface loopback 0添加接口下mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface loopback 0 \n
 * - 使用举例 \n
 *		Hios(config)# mirror-group 2 \n
 *		Hios(config-mirror)# mirror-to interface loopback 0 \n
 *		Hios(config-mirror)# exit \n
 *		Hios (config)# interface loopback 0 \n
 *		Hios(config-loopback0)# mirror-to group 2 direction all \n
 *		Hios(config-loopback0)# no mirror-to group \n
 */
DEFSH(VTYSH_QOS,
		qos_mirror_to_loopback_cmd_vtysh,
		"mirror-to interface loopback 0",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
        CLI_INTERFACE_LOOPBACK_STR
        "Interface loopback 0\n")

/**
 * \page qos_mirror_if_to_ethernet_cmd_vtysh mirror-to interface ethernet USP direction (ingress|all)
 * - 功能说明 \n
 *	 配置在group模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface ethernet USP direction (ingress|all)
 * - 参数说明 \n
 *	 |参数	|说明 		|
 *	 |------|-----------|
 *	 |USP	|USP:表示ethernet逻辑端口号，格式：<0-7>/<0-31>/<1-255>|
 *	 |ingress\|all |ingress\|all：表示镜像方向分别为：入方向、双向|
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于接口配置模式下，使用mirror-to interface ethernet USP添加接口下mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface ethernet USP \n
 * - 使用举例 \n
 *		Hios(config)# interface ethernet 1/1/6 \n
 *		Hios(config-ethernet1/1/6)# mirror-to interface ethernet 1/1/5 direction ingress \n
 *		Hios(config-ethernet1/1/6)# no mirror-to interface ethernet 1/1/5 \n
 *		Hios(config-ethernet1/1/6)# exit \n
 *
 */
DEFSH(VTYSH_QOS,
		qos_mirror_if_to_ethernet_cmd_vtysh,
		"mirror-to interface ethernet USP direction (ingress|all)",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
        "The port of ethernet, format: <0-7>/<0-31>/<1-255>\n"
        "Direction\n"
        "Ingress direction\n"
        "All direction\n")

/**
 * \page qos_mirror_if_to_gigabit_ethernet_cmd_vtysh mirror-to interface gigabitethernet USP direction (ingress|all)
 * - 功能说明 \n
 *	 配置在group模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface gigabitethernet USP direction (ingress|all)
 * - 参数说明 \n
 *	 |参数	|说明			|
 *   |------|-----------|
 *   |USP	|USP:表示gigabitethernet逻辑端口号，格式：<0-7>/<0-31>/<1-255>|
 *   |ingress\|all |ingress\|all：表示镜像方向分别为：入方向、双向|
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于接口配置模式下，使用mirror-to interface gigabitethernet USP添加接口下mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface gigabitethernet USP \n
 * - 使用举例 \n
 *		Hios(config)# interface gigabitethernet 1/1/6 \n
 *      Hios(config-gigabitethernet1/1/6)# mirror-to interface gigabitethernet 1/1/5 direction ingress \n
 *      Hios(config-gigabitethernet1/1/6)# no mirror-to interface gigabitethernet 1/1/5 \n
 *      Hios(config-gigabitethernet1/1/6)# exit \n
 *
 */
DEFSH(VTYSH_QOS,
		qos_mirror_if_to_gigabit_ethernet_cmd_vtysh,
		"mirror-to interface gigabitethernet USP direction (ingress|all)",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
        "The port of gigabitethernet, format: <0-7>/<0-31>/<1-255>\n"
        "Direction\n"
        "Ingress direction\n"
        "All direction\n")

/**
 * \page qos_mirror_if_to_xgigabit_ethernet_cmd_vtysh mirror-to interface xgigabitethernet USP direction (ingress|all)
 * - 功能说明 \n
 *	 配置在group模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface xgigabitethernet USP direction (ingress|all)
 * - 参数说明 \n
 *	 |参数	|说明 		|
 *	 |------|-----------|
 *	 |USP	|USP:表示xgigabitethernet逻辑端口号，格式：<0-7>/<0-31>/<1-255>|
 *	 |ingress\|all \|ingress\|all：表示镜像方向分别为：入方向、双向|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于接口配置模式下，使用mirror-to interface xgigabitethernet USP添加接口下mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface xgigabitethernet USP \n
 * - 使用举例 \n
 *		Hios(config)# interface gigabitethernet 1/1/6 \n
 *		Hios(config-xgigabitethernet1/1/6)# mirror-to interface xgigabitethernet 1/1/5 direction ingress \n
 *		Hios(config-xgigabitethernet1/1/6)# no mirror-to interface xgigabitethernet 1/1/5 \n
 *		Hios(config-xgigabitethernet1/1/6)# exit \n
 *
 */
DEFSH(VTYSH_QOS,
		qos_mirror_if_to_xgigabit_ethernet_cmd_vtysh,
		"mirror-to interface xgigabitethernet USP direction (ingress|all)",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        "The port of xgigabitethernet, format: <0-7>/<0-31>/<1-255>\n"
        "Direction\n"
        "Ingress direction\n"
        "All direction\n")

/**
 * \page qos_mirror_if_to_ethernet_egress_cmd_vtysh mirror-to interface ethernet USP direction egress
 * - 功能说明 \n
 *	 配置在group模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface ethernet USP direction egress
 * - 参数说明 \n
 *	 |参数	|说明 		|
 *	 |------|-----------|
 *	 |USP	|USP:表示ethernet逻辑端口号，格式：<0-7>/<0-31>/<1-255>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于接口配置模式下，使用mirror-to interface ethernet USP direction egress添加接口下mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface ethernet USP \n
 * - 使用举例 \n
 *		Hios(config)# interface ethernet 1/1/6 \n
 *		Hios(config-ethernet1/1/6)# mirror-to interface ethernet 1/1/5 direction egress \n
 *		Hios(config-ethernet1/1/6)# no mirror-to interface ethernet 1/1/5 \n
 *		Hios(config-ethernet1/1/6)# exit \n
 *
 */
DEFSH(VTYSH_QOS,
		qos_mirror_if_to_ethernet_egress_cmd_vtysh,
		"mirror-to interface ethernet USP direction egress",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
        "The port of ethernet, format: <0-7>/<0-31>/<1-255>\n"
        "Direction\n"
        "Egress direction\n")

/**
 * \page qos_mirror_if_to_gigabit_egress_ethernet_cmd_vtysh mirror-to interface gigabitethernet USP direction egress
 * - 功能说明 \n
 *	 配置在group模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface gigabitethernet USP direction egress
 * - 参数说明 \n
 *	 |参数	|说明 		|
 *	 |------|-----------|
 *	 |USP	|USP:表示gigabitethernet逻辑端口号，格式：<0-7>/<0-31>/<1-255>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于接口配置模式下，使用mirror-to interface gigabitethernet USP direction egress添加接口下mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface gigabitethernet USP \n
 * - 使用举例 \n
 *		Hios(config)# interface gigabitethernet 1/1/6 \n
 *		Hios(config-gigabitethernet1/1/6)# mirror-to interface gigabitethernet 1/1/5 direction egress \n
 *		Hios(config-gigabitethernet1/1/6)# no mirror-to interface gigabitethernet 1/1/5 \n
 *		Hios(config-gigabitethernet1/1/6)# exit \n
 *
 */
DEFSH(VTYSH_QOS,
		qos_mirror_if_to_gigabit_ethernet_egress_cmd_vtysh,
		"mirror-to interface gigabitethernet USP direction egress",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
        "The port of gigabitethernet, format: <0-7>/<0-31>/<1-255>\n"
        "Direction\n"
        "Egress direction\n")

/**
 * \page qos_mirror_if_to_xgigabit_egress_ethernet_cmd_vtysh mirror-to interface xgigabitethernet USP direction egress
 * - 功能说明 \n
 *	 配置在group模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface xgigabitethernet USP direction egress
 * - 参数说明 \n
 *	 |参数	|说明 		|
 *	 |------|-----------|
 *	 |USP	|USP:表示xgigabitethernet逻辑端口号，格式：<0-7>/<0-31>/<1-255>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于接口配置模式下，使用mirror-to interface xgigabitethernet USP direction egress添加接口下mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface xgigabitethernet USP \n
 * - 使用举例 \n
 *		Hios(config)# interface gigabitethernet 1/1/6 \n
 *		Hios(config-xgigabitethernet1/1/6)# mirror-to interface xgigabitethernet 1/1/5 direction egress \n
 *		Hios(config-xgigabitethernet1/1/6)# no mirror-to interface xgigabitethernet 1/1/5 \n
 *		Hios(config-xgigabitethernet1/1/6)# exit\n
 *
 */
DEFSH(VTYSH_QOS,
		qos_mirror_if_to_xgigabit_ethernet_egress_cmd_vtysh,
		"mirror-to interface xgigabitethernet USP direction egress",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        "The port of xgigabitethernet, format: <0-7>/<0-31>/<1-255>\n"
        "Direction\n"
        "Egress direction\n")

/**
 * \page qos_mirror_if_to_trunk_cmd_vtysh mirror-to interface trunk TRUNK direction (ingress|egress|all)
 * - 功能说明 \n
 *	 配置在接口模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface trunk TRUNK direction (ingress|egress|all)
 * - 参数说明 \n
 *	 |参数	|说明 		   |
 *	 |------|--------------|
 *	 |TRUNK |TRUNK：表示trunk端口号，格式：<1-128>|
 *   |all\|egress\|ingress|all\|egress\|ingress：表示镜像方向分别为：双向、出方向、入方向|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于接口配置模式下，使用mirror-to interface trunk TRUNK direction (ingress|egress|all)添加mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface trunk TRUNK \n
 * - 使用举例 \n
 *		Hios(config)# interface trunk 1 \n
 *		Hios(config-trunk1)# mirror-to interface trunk 1 direction egress \n
 *		Hios(config-trunk1)# no mirror-to interface trunk 1 \n
 *		Hios(config-trunk1)# exit \n
 */
DEFSH(VTYSH_QOS,
		qos_mirror_if_to_trunk_cmd_vtysh,
		"mirror-to interface trunk TRUNK direction (ingress|egress|all)",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n"
        "Direction\n"
        "Ingress direction\n"
        "Egress direction\n"
        "All direction\n")

/**
 * \page qos_mirror_if_to_loopback_cmd_vtysh mirror-to interface loopback 0 direction (ingress|egress|all)
 * - 功能说明 \n
 *	 配置在接口模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 mirror-to interface loopback 0 direction (ingress|egress|all)
 * - 参数说明 \n
 *	 |参数	|说明 		   |
 *	 |------|--------------|
 *   |all\|egress\|ingress|all\|egress\|ingress：表示镜像方向分别为：双向、出方向、入方向|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于接口配置模式下，使用mirror-to interface loopback 0 direction (ingress|egress|all)添mirror配置
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   no mirror-to interface loopback 0 \n
 * - 使用举例 \n
 *		Hios(config)# interface loopback 0 \n
 *		Hios(config-loopback0)# mirror-to interface trunk 1 direction egress \n
 *		Hios(config-loopback0)# no mirror-to interface trunk 1 \n
 *		Hios(config-loopback0)# exit \n
 */
DEFSH(VTYSH_QOS,
		qos_mirror_if_to_loopback_cmd_vtysh,
		"mirror-to interface loopback 0 direction (ingress|egress|all)",
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
        CLI_INTERFACE_LOOPBACK_STR
        "Interface loopback 0\n"
        "Direction\n"
        "Ingress direction\n"
        "Egress direction\n"
        "All direction\n")

/**
* \page no_qos_mirror_to_ethernet_cmd_vtysh no mirror-to interface ethernet USP
* - 功能说明 \n
*	配置在接口模式或者group模式下，删除镜像的监视口
* - 命令格式 \n
*	no mirror-to interface ethernet USP
* - 参数说明 \n
*	|参数    |说明		   |
*	|------|-----------|
*	|USP   |USP:表示ethernet逻辑端口号，格式：<0-7>/<0-31>/<1-255>|
*
* - 缺省情况 \n
*	缺省情况下，未配置镜像的监视口
* - 命令模式 \n
*	接口配置模式下
* - 用户等级 \n
*	11
* - 使用指南 \n
*	- 应用场景 \n
*	 应用于接口配置模式下或者group模式下，应用在接口下使用no命令删除接口下mirror配置。应用在group模式下删除group监视口成员
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	  show mirror-group \n
*	  show mirror interface \n
*	  mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} \n
*	  mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} direction {all|egress|ingress} \n
*	- 使用举例 \n
*	   Hios(config)# interface ethernet 1/1/6 \n
*	   Hios(config-ethernet1/1/6)# mirror-to interface ethernet 1/1/5 direction egress \n
*	   Hios(config-ethernet1/1/6)# no mirror-to interface ethernet 1/1/5 \n
*	   Hios(config-ethernet1/1/6)# exit \n
*	   Hios(config)# mirror-group 1 \n
*	   Hios(config-mirror)# mirror-to interface ethernet 1/1/6 \n
*	   Hios(config-mirror)# no mirror-to interface ethernet \n
*	   Hios(config-mirror)# exit \n
*
*/
DEFSH(VTYSH_QOS,
		no_qos_mirror_to_ethernet_cmd_vtysh,
		"no mirror-to interface ethernet USP",
		NO_STR
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_ETHERNET_STR
        "The port of ethernet, format: <0-7>/<0-31>/<1-255>\n")

/**
 * \page no_qos_mirror_to_gigabit_ethernet_cmd_vtysh no mirror-to interface gigabitethernet USP
 * - 功能说明 \n
 *	 配置在接口模式或者group模式下，删除镜像的监视口
 * - 命令格式 \n
 *	 no mirror-to interface gigabitethernet USP
 * - 参数说明 \n
 *	 |参数	|说明 		|
 *	 |------|-----------|
 *	 |USP	|USP:表示gigabitethernet逻辑端口号，格式：<0-7>/<0-31>/<1-255>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	  应用于接口配置模式下或者group模式下，应用在接口下使用no命令删除接口下mirror配置。应用在group模式下删除group监视口成员
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 * 	   show mirror-group \n
 * 	   show mirror interface \n
 * 	   mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} \n
 * 	   mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} direction {all|egress|ingress} \n
 *   - 使用举例 \n
 *		Hios(config)# interface gigabitethernet 1/1/6 \n
 *      Hios(config-gigabitethernet1/1/6)# mirror-to interface gigabitethernet 1/1/5 direction egress \n
 *      Hios(config-gigabitethernet1/1/6)# no mirror-to interface gigabitethernet 1/1/5 \n
 *      Hios(config-gigabitethernet1/1/6)# exit \n
 *      Hios(config)# mirror-group 1 \n
 *      Hios(config-mirror)# mirror-to interface gigabitethernet 1/1/6 \n
 *      Hios(config-mirror)# no mirror-to interface gigabitethernet \n
 *      Hios(config-mirror)# exit \n
 *
 */
DEFSH(VTYSH_QOS,
		no_qos_mirror_to_gigabit_ethernet_cmd_vtysh,
		"no mirror-to interface gigabitethernet USP",
		NO_STR
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
        "The port of gigabitethernet, format: <0-7>/<0-31>/<1-255>\n")
/**
 * \page no_qos_mirror_to_xgigabit_ethernet_cmd_vtysh no mirror-to interface xgigabitethernet USP
 * - 功能说明 \n
 *	 配置在接口模式或者group模式下，删除镜像的监视口
 * - 命令格式 \n
 *	 no mirror-to interface xgigabitethernet USP
 * - 参数说明 \n
 *	 |参数	|说明 		|
 *	 |------|-----------|
 *	 |USP	|USP:表示xgigabitethernet逻辑端口号，格式：<0-7>/<0-31>/<1-255>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	  应用于接口配置模式下或者group模式下，应用在接口下使用no命令删除接口下mirror配置。应用在group模式下删除group监视口成员
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} \n
 *	   mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} direction {all|egress|ingress} \n
 * - 使用举例 \n
 *	   Hios(config)# interface xgigabitethernet 1/1/6 \n
 *	   Hios(config-xgigabitethernet1/1/6)# mirror-to interface xgigabitethernet 1/1/5 direction egress \n
 *	   Hios(config-xgigabitethernet1/1/6)# no mirror-to interface xgigabitethernet 1/1/5 \n
 *	   Hios(config-xgigabitethernet1/1/6)# exit \n
 *	   Hios(config)# mirror-group 1 \n
 *	   Hios(config-mirror)# mirror-to interface xgigabitethernet 1/1/6 \n
 *	   Hios(config-mirror)# no mirror-to interface xgigabitethernet 1/1/6 \n
 *	   Hios(config-mirror)# exit \n
 *
 */

DEFSH(VTYSH_QOS,
		no_qos_mirror_to_xgigabit_ethernet_cmd_vtysh,
		"no mirror-to interface xgigabitethernet USP",
		NO_STR
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        "The port of xgigabitethernet, format: <0-7>/<0-31>/<1-255>\n")

/**
 * \page no_qos_mirror_to_trunk_cmd_vtysh no mirror-to interface trunk TRUNK
 * - 功能说明 \n
 *	 配置在接口模式下,添加镜像的监视口
 * - 命令格式 \n
 *	 no mirror-to interface trunk TRUNK
 * - 参数说明 \n
 *	 |参数	|说明 		   |
 *	 |------|--------------|
 *	 |TRUNK |TRUNK：表示trunk端口号，格式：<1-128>|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下或者group模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于接口配置模式下或者group模式下，应用在接口下使用no命令删除接口下mirror配置。应用在group模式下删除group监视口成员
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 * 	   show mirror-group \n
 *     show mirror interface \n
 * 	   mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} \n
 * 	   mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} direction {all|egress|ingress} \n
 * - 使用举例 \n
 *		无
 *
 */
DEFSH(VTYSH_QOS,
		no_qos_mirror_to_trunk_cmd_vtysh,
		"no mirror-to interface trunk TRUNK",
		NO_STR
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n")

/**
 * \page no_qos_mirror_to_loopback_cmd_vtysh no mirror-to interface loopback 0
 *	 配置在接口模式下,删除镜像的监视口
 * - 命令格式 \n
 *	 no mirror-to interface loopback 0
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 接口配置模式下
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   应用于接口配置模式下或者group模式下，应用在接口下使用no命令删除接口下mirror配置。应用在group模式下删除group监视口成员
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   show mirror-group \n
 *	   show mirror interface \n
 *	   mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} \n
 *     mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} direction {all|egress|ingress} \n
 * - 使用举例 \n
 *		无
 */
DEFSH(VTYSH_QOS,
		no_qos_mirror_to_loopback_cmd_vtysh,
		"no mirror-to interface loopback 0",
		NO_STR
		"Qos mirror-to\n"
		CLI_INTERFACE_STR
        CLI_INTERFACE_LOOPBACK_STR
        "Interface loopback 0\n")

/**
 * \page show_qos_mirror_group_cmd_vtysh show mirror-group
 * - 功能说明 \n
 *	 显示mirror-group信息
 * - 命令格式 \n
 *	 show mirror-group
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，未配置镜像的监视口
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   config模式下，用于显示mirror-group 监视口信息
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   mirror-group <1-3> \n
 *	   mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} \n
 *	   mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} direction {all|egress|ingress} \n
 * - 使用举例 \n
 *	   Hios(config)# mirror-group 2 \n
 *	   Hios(config-mirror)# mirror-to interface gigabitethernet 1/1/5 \n
 *	   Hios(config-mirror)# mirror-to interface gigabitethernet 1/1/6 \n
 *	   Hios(config-mirror)# exit \n
 *	   Hios(config)# show mirror-group \n
 *	   ----------------------------------------------------- \n
 *	   mirror-group 2 \n
 *		mirror-to interface gigabitethernet 1/1/5 \n
 *		mirror-to interface gigabitethernet 1/1/6 \n
 *	   ----------------------------------------------------- \n
 *
 */
DEFSH(VTYSH_QOS,
		show_qos_mirror_group_cmd_vtysh,
       "show mirror-group ",
       SHOW_STR
       "Qos mirror-group\n")

/**
 * \page show_qos_mirror_if_cmd_vtysh show mirror interface
 * - 功能说明 \n
 *	 显示接口镜像信息
 * - 命令格式 \n
 *	 show mirror interface
 * - 参数说明 \n
 *	 无
 * - 缺省情况 \n
 *	 缺省情况下，未配置mirror
 * - 命令模式 \n
 *	 config模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   config模式下，用于显示接口应用mirror 配置信息
 *	 - 前置条件 \n
 *	   无
 *	 - 后续任务 \n
 *	   无
 *	 - 注意事项 \n
 *	   无
 *	 - 相关命令 \n
 *	   mirror-group <1-3> \n
 *	   mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} \n
 *	   mirror-to interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP|trunk TRUNK|lookback 0} direction {all|egress|ingress} \n
 * - 使用举例 \n
 *      Hios(config)# mirror-group 2 \n
 *      Hios(config-mirror)# mirror-to interface gigabitethernet 1/1/5 \n
 *      Hios(config-mirror)# mirror-to interface gigabitethernet 1/1/6 \n
 *      Hios(config-mirror)# exit \n
 *      Hios(config)# interface gigabitethernet 1/1/7 \n
 *      Hios(config-gigabitethernet1/1/7)#mirror-to group 2 \n
 *      Hios(config-gigabitethernet1/1/7)#exit \n
 *      Hios(config)# interface gigabitethernet 1/1/8 \n
 *      Hios(config-gigabitethernet1/1/8)#mirror-to interface gigabitethernet 1/1/5 direction all \n
 *      Hios(config-gigabitethernet1/1/8)#exit \n
 *      Hios(config)# show mirror interface \n
 *       Interface 		          Mirror_to_group   Mirror_to_if		      Direction \n
 *      ----------------------------------------------------------------------------------------- \n
 *       gigabitethernet 1/1/8    --	            gigabitethernet 1/1/5     all \n
 *       gigabitethernet 1/1/7    2   	            --    			          egress \n
 *
 */
DEFSH(VTYSH_QOS,
		show_qos_mirror_if_cmd_vtysh,
       "show  mirror interface ",
       SHOW_STR
       "Qos mirror\n"
       CLI_INTERFACE_STR)

/* sla */
DEFUNSH(VTYSH_QOS,
	sla_session_get,
	sla_session_get_cmd_vtysh,
	"sla session <1-65535>",
	SLA_STR
	"Sla session\n"
	"Session id <1-65535>\n"
	)
{
	char *pprompt = NULL;
	uint16_t sla_id = 0;

	sla_id = atoi(argv[0]);
	vty->node = SLA_SESSION_NODE;
	pprompt = cmd_prompt(vty->node);
    if (pprompt)
    {
        /* format the prompt */
        snprintf (pprompt, VTY_BUFSIZ, "%%s(config-sla-session %d)#", sla_id);
    }

	return CMD_SUCCESS;
}

DEFUNSH (VTYSH_QOS,
	 vtysh_exit_sla_session,
	 vtysh_exit_sla_session_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

	
DEFSH(VTYSH_QOS,
	no_sla_session_get_cmd_vtysh,
	"no sla session <1-65535> {to <1-65535>}",
	"No command\n"
	SLA_STR
	"Sla session\n"
	"Session id <1-65535>\n"
	"To"
	"Session id <1-65535>\n"
	)


DEFSH(VTYSH_QOS,
	sla_protocol_select_cmd_vtysh,
	"protocol (rfc2544| y1564)",
	"Sla protocol\n"
	"Protocol rfc2544\n"
	"Protocol y1564\n"
	)
	
DEFSH(VTYSH_QOS,
	no_sla_protocol_select_cmd_vtysh,
	"no protocol",
	"No command\n"
	"Sla protocol\n"
	)

DEFSH(VTYSH_QOS,
	sla_ethernet_format_get_cmd_vtysh,
	"service ethernet {smac XX:XX:XX:XX:XX:XX | dmac XX:XX:XX:XX:XX:XX | cvlan <1-4094> | c-tpid (0x9100|0x9200|0x88a8|0x8100) | c-cos <0-7> | svlan <1-4094> | s-tpid (0x9100|0x9200|0x88a8|0x8100) | s-cos <0-7>}",
	"Service\n"
	"Ethernet protocol\n"
	"Smac\n"
	"Smac XX:XX:XX:XX:XX:XX\n"
	"Dmac\n"
	"Dmac XX:XX:XX:XX:XX:XX\n"
	"Cvlan\n"
	"Cvlan <1-4094>\n"
	"C-tpid\n"
	"C-tpid 0x9100\n"
	"C-tpid 0x9200\n"
	"C-tpid 0x88a8\n"
	"C-tpid 0x8100\n"
	"C-cos\n"
	"C-cos <0-7>\n"
	"Svlan\n"
	"Svlan <1-4094>\n"
	"S-tpid\n"
	"S-tpid 0x9100\n"
	"S-tpid 0x9200\n"
	"S-tpid 0x88a8\n"
	"S-tpid 0x8100\n"
	"S-cos\n"
	"S-cos <0-7>\n"
	)

DEFSH(VTYSH_QOS,
	no_sla_ethernet_format_get_cmd_vtysh,
	"no service ethernet",
	"No command\n"
	"Service\n"
	"Ethernet protocol\n"
	)

DEFSH(VTYSH_QOS,
	sla_ip_format_get_cmd_vtysh,
	"service ip {sip A.B.C.D | dip A.B.C.D | sport <1-65535> | dport <1-65535> | ttl <1-255> | dscp <0-63>}",
	"Service\n"
	"Ip protocol\n"
	"Sip\n"
	"Sip A.B.C.D\n"
	"Dip\n"
	"Dip A.B.C.D\n"
	"Sport\n"
	"Sport <1-65535>\n"
	"Dport\n"
	"Dport <1-65535>\n"
	"Ttl\n"
	"Ttl <1-255>\n"
	"Dscp\n"
	"Dscp <0-63>\n"
	)

DEFSH(VTYSH_QOS,
	no_sla_ip_format_get_cmd_vtysh,
	"no service ip",
	"No command\n"
	"Service\n"
	"Ip protocol\n"
	)

DEFSH(VTYSH_QOS,
	sla_y1564_packet_size_get_cmd_vtysh,
	"y1564 packet size <64-9600>",
    "Y1564\n"
	"Packet\n"
	"Packet size\n"
	"Range of packet size:<64-9600>\n")
	
DEFSH(VTYSH_QOS,
	no_sla_y1564_packet_size_get_cmd_vtysh,
	"no packet size",
	"No command\n"
    "Y1564\n"
	"Packet\n"
	"Packet size\n")

DEFSH (VTYSH_QOS,
	sla_2544_packet_size_get_cmd_vtysh,
	"rfc2544 packet {size1 <64-9600> | size2 <64-9600> | size3 <64-9600> | size4 <64-9600> | size5 <64-9600> | size6 <64-9600> | size7 <64-9600>}",
	"Rfc2544\n"
    "Packet\n"
	"Size1\n"
	"Range of size1:<64-9600>\n"
	"Size2\n"
	"Range of size2:<64-9600>\n"
	"Size3\n"
	"Range of size3:<64-9600>\n"
	"Size4\n"
	"Range of size4:<64-9600>\n"
	"Size5\n"
	"Range of size5:<64-9600>\n"
	"Size6\n"
	"Range of size6:<64-9600>\n"
	"Size7\n"
	"Range of size7:<64-9600>\n")

DEFSH (VTYSH_QOS,
	no_sla_2544_packet_size_get_cmd_vtysh,
     "no rfc2544 packet size",
     "Delete\n"
     "Rfc2544\n"
     "Packet\n"
     "Size\n")



#if 0
DEFSH(VTYSH_QOS,
	sla_packet_rate_get_cmd_vtysh,
	"packet rate <1-1000> {to <1-1000>}",
	"Packet\n"
	"Packet rate\n"
	"Packet rate upper limit <1-1000> Mbps\n"
	"To\n"
	"Packet rate lower limit <1-1000> Mbps\n"
	)
	
DEFSH(VTYSH_QOS,
	no_sla_packet_rate_get_cmd_vtysh,
	"no packet rate",
	"No command\n"
	"Packet\n"
	"Packet rate\n"
	)
#endif

DEFSH (VTYSH_QOS,
	sla_packet_rate_get_cmd_vtysh,
	"packet rate <1-10000000> {to <1-10000000>}",
	"Packet\n"
	"Packet rate\n"
	"Packet rate upper limit <1-10000000> kbps\n"
	"To\n"
	"Packet rate lower limit <1-10000000> kbps\n")

DEFSH (VTYSH_QOS,
	no_sla_packet_rate_get_cmd_vtysh,
	"no packet rate",
	"No command\n"
	"Packet\n"
	"Packet rate\n")

DEFSH(VTYSH_QOS,
	sla_packet_loss_cmd_vtysh,
	"measure packet-loss",
	"Measure\n"
	"Packet-loss\n"
	)
	
DEFSH(VTYSH_QOS,
	no_sla_packet_loss_cmd_vtysh,
	"no measure packet-loss",
	"No command\n"
	"Measure\n"
	"Packet-loss\n"
	)
	
DEFSH(VTYSH_QOS,
	sla_packet_delay_cmd_vtysh,
	"measure packet-delay",
	"Measure\n"
	"Packet-delay\n"
	)

DEFSH(VTYSH_QOS,
	no_sla_packet_delay_cmd_vtysh,
	"no measure packet-delay",
	"No command\n"
	"Measure\n"
	"Packet-delay\n"
	)


DEFSH (VTYSH_QOS,
	sla_sac_cmd_vtysh,
	"sac {loss <0-100000> | delay <1-10000000> | jitter <1-10000000>}",
	SAC_STR
	"Frame loss ratio\n"
	"loss <0-100000> Unit: hundred thousandth\n"
	"Frame transfer delay\n"
	"delay <1-10000000> Unit: us\n"
	"Frame delay variation\n"
	"jitter <1-10000000> Unit: us\n")


DEFSH(VTYSH_QOS,
	no_sla_sac_cmd_vtysh,
	"no sac",
	"No command\n"
	SAC_STR)

#if 0
DEFSH(VTYSH_QOS,
	sla_bandwidth_profile_cmd_vtysh,
	 "bandwidth profile cir <1-1000> eir <0-1000>",
	 "Bandwidth\n"
	 "Profile\n"
	 "Committed information rate\n"
	 "1-1000 Mbps\n"
	 "Excess information rate\n"
	 "0-1000 Mbps\n"
	 )
	 
DEFSH(VTYSH_QOS,
	no_sla_bandwidth_profile_cmd_vtysh,
	 "no bandwidth profile",
	 "No command\n"
	 "Bandwidth\n"
	 "Profile\n"
	 )
#endif

DEFSH (VTYSH_QOS,
	sla_bandwidth_profile_cmd_vtysh,
	"bandwidth profile cir <256-1000000> eir <0-1000000>",
	"Bandwidth\n"
	"Profile\n"
	"Committed information rate\n"
	"256-1000000 kbps\n"
	"Excess information rate\n"
	"0-1000000 kbps\n")


DEFSH (VTYSH_QOS,
	no_sla_bandwidth_profile_cmd_vtysh,
	"no bandwidth profile",
	"No command\n"
	"Bandwidth\n"
	"Profile\n") 






DEFSH(VTYSH_QOS,
	sla_packet_throughput_cmd_vtysh,
	"measure throughput",
	"Measure\n"
	"Throughput\n"
	)

DEFSH(VTYSH_QOS,
	no_sla_packet_throughput_cmd_vtysh,
	"no measure throughput",
	"No command\n"
	"Measure\n"
	"Throughput\n"
	)



DEFSH (VTYSH_QOS,
	 sla_y1564_test_type_cmd_vtysh,
	 "y1564 enable (configuration-test | performance-test)",
	 "Protocol:y.1564\n"
	 "Enable test\n"
	 "configuration-test\n"
	 "performance-test\n")

DEFSH (VTYSH_QOS,
	 no_sla_y1564_test_type_cmd_vtysh,
	 "no y1564 enable",
	 "Delete\n"
	 "Protocol:y.1564\n"
	 "Enable test\n")



DEFSH (VTYSH_QOS,
	 sla_y1564_traffic_policing_enable_cmd_vtysh,
	 "traffic-policing-test enable",
	 "Traffic-policing test of Configuration-test\n"
	 "Enable test\n")

DEFSH (VTYSH_QOS,
	 no_sla_y1564_traffic_policing_enable_cmd_vtysh,
	 "no traffic-policing-test enable",
	 "Delete\n"
	 "Traffic-policing test of Configuration-test\n"
	 "Enable test\n")

DEFSH (VTYSH_QOS,
	 sla_packet_duration_cmd_vtysh,
	 "duration <1-60>",
	 "Duration\n"
	 "Duration <1-60> Unit: s\n")

DEFSH (VTYSH_QOS,
	 no_sla_packet_duration_cmd_vtysh,
	 "no duration",
	 "Delete\n"
	 "Duration\n")

DEFSH(VTYSH_QOS | VTYSH_IFM,
	sla_interface_session_enable_cmd_vtysh,
	"sla enable session <1-65535> {to <1-65535>}",
	SLA_STR
	"Enable\n"
	"Session\n"
	"Session <1-65535>\n"
	"To\n"
	"Session <1-65535>\n"
	"To\n"
	"Session <1-65535>\n"
	)

DEFSH(VTYSH_QOS | VTYSH_IFM,
	no_sla_interface_session_enable_cmd_vtysh,
	"no sla enable session <1-65535> {to <1-65535>}",
	"No command\n"
	SLA_STR
	"Enable\n"
	"Session\n"
	"Session <1-65535>\n"
	"To\n"
	"Session <1-65535>\n"
	"To\n"
	"Session <1-65535>\n"
	)

DEFSH(VTYSH_QOS | VTYSH_IFM,
	sla_sub_interface_session_enable_cmd_vtysh,
	"sla enable session <1-65535> {to <1-65535>}",
	SLA_STR
	"Enable\n"
	"Session\n"
	"Session <1-65535>\n"
	"To\n"
	"Session <1-65535>\n"
	)

DEFSH(VTYSH_QOS | VTYSH_IFM,
	no_sla_sub_interface_session_enable_cmd_vtysh,
	"no sla enable session <1-65535> {to <1-65535>}",
	"No command\n"
	SLA_STR
	"Enable\n"
	"Session\n"
	"Session <1-65535>\n"
	"To\n"
	"Session <1-65535>\n"
	)

DEFSH(VTYSH_QOS,
	sla_show_config_cmd_vtysh,
	"show sla config [session <1-65535>]",
	"Show command\n"
	SLA_STR
	"Config\n"
	"Session\n"
	"Session <1-65535>\n"
	)
	
DEFSH(VTYSH_QOS,
	sla_show_session_cmd_vtysh,
	"show sla session <1-65535> {to <1-65535>}",
	"Show command\n"
	SLA_STR
	"Session\n"
	"Session <1-65535>\n"
	"To\n"
	"Session <1-65535>\n"
	)


DEFUNSH (VTYSH_QOS,
	qos_cpcar_mode,
	qos_cpcar_mode_cmd,
	"cpu-car",
	"Cpu-car command\n")
{
	vty->node = QOS_CPCAR_NODE;
	return CMD_SUCCESS;
}

DEFSH(VTYSH_QOS,
		qos_cpu_car_cmd_vtysh,
		"cpu-car (arp|dhcp|icmp|tcp|igmp|all) <1-65535>",
		"Cpu car config\n"
		"ARP protocol pps enter cpu queue 8\n"
		"DHCP protocol pps enter cpu queue 6\n"
		"ICMP protocol pps enter cpu queue 9\n"
		"TCP protocol pps enter cpu queue 10\n"
		"IGMP protocol pps enter cpu queue 7\n"
		"Global all cpu queue pps\n"
		"CPU queue pps value <1-65535>\n")


DEFSH(VTYSH_QOS,
		qos_cpu_car_outband_cmd_vtysh,
		"cpu-car outband (arp|ipv6|dhcp|tftp|ssh|telnet|snmp|icmp|ip-other) pps <1-2000>",
		"Cpu car config\n"
		"Outband interface\n"
		"ARP protocol outband pps\n"
		"Ipv6 protocol outband pps\n"
		"Dhcp protocol outband pps\n"
		"Tftp protocol outband pps\n"
		"Ssh protocol outband pps\n"
		"Telnet protocol outband pps\n"
		"Snmp protocol outband pps\n"
		"Icmp protocol outband pps\n"
		"Ip-other outband pps\n"
		"Pps\n"
		"Outband interface pps value <1-2000>\n")


DEFSH(VTYSH_QOS,
		no_qos_cpu_car_outband_cmd_vtysh,
		"no cpu-car outband (arp|ipv6|dhcp|tftp|ssh|telnet|snmp|icmp|ip-other)",
		NO_STR
		"Cpu car config\n"
		"Outband interface\n"
		"ARP protocol outband initial pps setting 300\n"
		"Ipv6 protocol outband initial pps setting 300\n"
		"Dhcp protocol outband initial pps setting 200\n"
		"Tftp protocol outband initial pps setting 500\n"
		"Ssh protocol outband initial pps setting 500\n"
		"Telnet protocol outband initial pps setting 500\n"
		"Snmp protocol outband initial pps setting 500\n"
		"Icmp protocol outband initial pps setting 200\n"
		"Ip-other outband pps initial pps setting 500\n")



DEFSH(VTYSH_QOS,
		show_qos_cpu_car_outband_cmd_vtysh,
       "show cpu-car outband config",
       SHOW_STR
       "Cpu car pps\n"
       "Outband interface\n"
       "Cpu car pps config\n")


DEFSH(VTYSH_QOS,
		no_qos_cpu_car_cmd_vtysh,
		"no cpu-car (arp|dhcp|icmp|tcp|igmp|all)",
		NO_STR
		"Qos cpu car\n"
		"ARP protocol pps\n"
		"DHCP protocol pps\n"
		"ICMP protocol pps\n"
		"TCP protocol pps\n"
		"IGMP protocol pps\n"
		"Global cpu queue pps\n")


DEFSH(VTYSH_QOS,
		qos_cpu_car_queue_cmd_vtysh,
		"cpu-car queue <0-15> <1-65535>",
		"Cpu car config\n"
		"Cpu queue\n"
		"Cpu queue id <0-15> except queue <6-10>\n"
		"CPU queue pps value <1-65535>\n")

DEFSH(VTYSH_QOS,
		no_qos_cpu_car_queue_cmd_vtysh,
		"no cpu-car queue <0-15>",
		NO_STR
		"Cpu car config\n"
		"Cpu queue\n"
		"Cpu queue id <0-15> except queue <6-10>\n")


DEFSH(VTYSH_QOS,
		show_qos_cpu_car_cmd_vtysh,
       "show cpu-car config",
       SHOW_STR
       "Cpu car pps\n"
       "Cpu car pps config\n")


DEFSH(VTYSH_QOS,
		show_qos_cpu_car_queue_cmd_vtysh,
       "show cpu-car queue",
       SHOW_STR
       "Qos Cpu car \n"
       "Cpu queue config add statistics\n")


DEFSH(VTYSH_QOS,
		qos_cpu_car_clear_cmd_vtysh,
       "cpu-car clear queue <0-15>",
       "Cpu car\n"
       "Statistics clear\n"
       "Qos cpu queue\n"
       "Cpu queue id<0-15>\n")


DEFSH(VTYSH_QOS,
		qos_cpu_car_clear_all_cmd_vtysh,
       "cpu-car clear all",
       "Cpu car\n"
       "Statistics clear\n"
       "All cpu queue statistics \n")



DEFUNSH(VTYSH_QOS,
		hqos_wred_profile,
        hqos_wred_profile_cmd_vtysh,
        "qos wred-profile <1-10>",
        "Quality of Service\n"
        "QoS wred-profile\n"
        "QoS wred-profile ID\n")
{
	vty->node = HQOS_WRED_NODE;
	return CMD_SUCCESS;
}

DEFSH(VTYSH_QOS,
        no_hqos_wred_profile_cmd_vtysh,
        "no qos wred-profile <1-10>",
        NO_STR
        "Quality of Service\n"
        "QoS wred-profile\n"
        "QoS wred-profile ID\n")

DEFSH(VTYSH_QOS,
        hqos_wred_rule_cmd_vtysh,
        "color (green|yellow|red) protocol (tcp|non-tcp|all) "
        "drop-threshold low <1-100> high <1-100> ratio <1-100>",
        "Specify color\n"
        "Green\n"
        "Yellow\n"
        "Red\n"
        "Specify protocol\n"
        "Tcp\n"
        "None-Tcp\n"
        "Do not care protocol\n"
        "Packet drop threshold\n"
        "Low limit\n"
        "Low limit Percentage\n"
        "High limit\n"
        "High limit Percentage\n"
        "Max drop ratio\n"
        "Max drop ratio value\n")

        
DEFSH(VTYSH_QOS,
      no_hqos_wred_rule_cmd_vtysh,
      "no color (green|yellow|red) protocol (tcp|non-tcp|all)",
      NO_STR
      "Specify color\n"
      "Green\n"
      "Yellow\n"
      "Red\n"
      "Specify protocol\n"
      "Tcp\n"
      "None-Tcp\n"
      "Do not care protocol\n")


DEFUNSH(VTYSH_QOS,
		hqos_queue_profile,
        hqos_queue_profile_cmd_vtysh,
        "qos queue-profile <1-100>",
        "Quality of Service\n"
        "QoS queue-profile\n"
        "QoS queue-profile ID\n")
{
    vty->node = HQOS_QUEUE_NODE;
    return CMD_SUCCESS;
}

DEFSH(VTYSH_QOS,
        no_hqos_queue_profile_cmd_vtysh,
        "no qos queue-profile <1-100>",
        NO_STR
        "Quality of Service\n"
        "QoS queue-profile\n"
        "QoS queue-profile ID\n")


DEFSH(VTYSH_QOS,
        hqos_queue_rule_cmd_vtysh,
        "queue <0-7> weight <0-127> cir <0-10000000> {pir <0-10000000> | wred-profile <1-10>}",
        "Queue\n"
        "Queue value\n"
        "Weight\n"
        "Weight value\n"
        "Committed information rate\n"
        "Committed information rate value (Unit: Kbps)\n"
        "Peak information rate\n"
        "Peak information rate value (Unit: Kbps)\n"
        "Wred profile\n"
        "Wred profile ID\n")
        
DEFSH(VTYSH_QOS,
        no_hqos_queue_rule_cmd_vtysh,
        "no queue <0-7>",
        NO_STR
        "Queue\n"
        "Queue value\n")


DEFSH(VTYSH_QOS,
        hqos_queue_scheduler_cmd_vtysh,
        "scheduler (pq|wrr|wdrr)",
        "Queue scheduler\n"
        "Priority queue\n"
        "Weight Round Robin\n"
        "Weight Deficit Round Robin\n")

DEFSH(VTYSH_QOS,
        no_hqos_queue_scheduler_cmd_vtysh,
        "no scheduler",
        NO_STR
        "Queue scheduler\n")

DEFSH(VTYSH_QOS,
        hqos_queue_apply_cmd_vtysh,
        "hqos pir <0-10000000> {pbs <16-16000> | queue-profile <1-100>}",
        "Hierarchical QoS\n"
        "Peak information rate\n"
        "Peak information rate value (Unit: Kbps)\n"
        "Peak burst size\n"
        "Peak burst size value (Unit: Kbits)\n"
        "Queue profile\n"
        "Queue profile ID\n")

DEFSH(VTYSH_QOS,
        no_hqos_queue_apply_cmd_vtysh,
        "no hqos",
        NO_STR
        "Hierarchical QoS\n")

DEFSH(VTYSH_QOS,
        show_hqos_wred_profile_cmd_vtysh,
        "show qos wred-profile [<1-10>]",
        SHOW_STR
        "Quality of Service\n"
        "QoS wred-profile\n"
        "QoS wred-profile ID\n")

DEFSH(VTYSH_QOS,
        show_hqos_queue_profile_cmd_vtysh,
        "show qos queue-profile [<1-100>]",
        SHOW_STR
        "Quality of Service\n"
        "QoS queue-profile\n"
        "QoS queue-profile ID\n")

DEFSH(VTYSH_QOS,
        qos_ifg_cmd_vtysh,
        "packet ifg-length <0-50>",
        "Packet\n"
        "Interframe Gap Length\n"
        "Interframe Gap Length value (Unit: bytes)\n")

DEFSH(VTYSH_QOS,
        no_qos_ifg_cmd_vtysh,
        "no packet ifg-length",
        NO_STR
        "Packet\n"
        "Interframe Gap Length\n")
    
DEFSH (VTYSH_QOS,
	qos_debug_monitor_cmd_vtysh,
	"debug qos bitmap <0-65535>",
	"Debug information to moniter\n"
	"Module name\n"
	"Debug control bitmap\n"
	"Debug bitmap value")

DEFSH (VTYSH_QOS,
	show_qos_debug_monitor_cmd_vtysh,
	"show qos debug",
	SHOW_STR
	"QoS"
	"Debug status\n")




void
vtysh_init_qos_cmd ()
{
	/* ACL */
	install_node (&acl_node, NULL);
	install_node (&qos_policy_node, NULL);

	vtysh_install_default (ACL_NODE);
	vtysh_install_default (QOS_POLICY_NODE);

	install_element_level (ACL_NODE, &vtysh_exit_acl_num_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (ACL_NODE, &vtysh_quit_acl_num_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &acl_num_cmd, VISIT_LEVE, CMD_SYNC);

	install_element_level (CONFIG_NODE, &no_acl_num_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &show_acl_info_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_acl_rule_info_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (ACL_NODE, &acl_ip_extend_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_NODE, &acl_ip_extend_2_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_NODE, &acl_ip_extend_3_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_NODE, &acl_mix_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_NODE, &acl_mac_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_NODE, &acl_ip_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_NODE, &no_acl_ip_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_NODE, &acl_name_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_NODE, &no_acl_name_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (CONFIG_NODE, &qos_policy_acl_filter_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_policy_egress_acl_filter_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_policy_acl_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_policy_egress_acl_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_policy_acl_mirror_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_policy_acl_mirror_gigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_policy_acl_mirror_xgigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_policy_acl_mirror_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_policy_acl_redirect_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_policy_acl_redirect_gigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_policy_acl_redirect_xgigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_policy_acl_redirect_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_policy_acl_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &no_qos_policy_ingress_acl_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &no_qos_policy_egress_acl_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &show_qos_policy_acl_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (PHYSICAL_IF_NODE, &qos_policy_acl_filter_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &qos_policy_egress_acl_filter_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_acl_filter_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_egress_acl_filter_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &qos_policy_acl_filter_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &qos_policy_acl_filter_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &qos_policy_acl_filter_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &qos_policy_acl_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &qos_policy_egress_acl_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_acl_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_egress_acl_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &qos_policy_acl_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &qos_policy_acl_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &qos_policy_acl_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &qos_policy_acl_mirror_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_acl_mirror_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &qos_policy_acl_mirror_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &qos_policy_acl_mirror_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &qos_policy_acl_mirror_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &qos_policy_acl_mirror_gigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_acl_mirror_gigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &qos_policy_acl_mirror_gigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &qos_policy_acl_mirror_gigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &qos_policy_acl_mirror_gigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &qos_policy_acl_mirror_xgigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_acl_mirror_xgigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &qos_policy_acl_mirror_xgigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &qos_policy_acl_mirror_xgigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &qos_policy_acl_mirror_xgigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &qos_policy_acl_mirror_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_acl_mirror_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &qos_policy_acl_mirror_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &qos_policy_acl_mirror_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &qos_policy_acl_mirror_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &qos_policy_acl_redirect_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_acl_redirect_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &qos_policy_acl_redirect_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &qos_policy_acl_redirect_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &qos_policy_acl_redirect_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &qos_policy_acl_redirect_gigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_acl_redirect_gigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &qos_policy_acl_redirect_gigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &qos_policy_acl_redirect_gigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &qos_policy_acl_redirect_gigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &qos_policy_acl_redirect_xgigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_acl_redirect_xgigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &qos_policy_acl_redirect_xgigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &qos_policy_acl_redirect_xgigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &qos_policy_acl_redirect_xgigabit_eth_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &qos_policy_acl_redirect_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_acl_redirect_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &qos_policy_acl_redirect_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &qos_policy_acl_redirect_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &qos_policy_acl_redirect_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &qos_policy_acl_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_policy_acl_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &qos_policy_acl_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &qos_policy_acl_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &qos_policy_acl_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &no_qos_policy_ingress_acl_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_qos_policy_egress_acl_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &no_qos_policy_ingress_acl_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &no_qos_policy_egress_acl_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_SUBIF_NODE, &no_qos_policy_ingress_acl_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_qos_policy_ingress_acl_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLANIF_NODE, &no_qos_policy_ingress_acl_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/*qos mapping*/
	install_node (&qos_domain_node, NULL);
	vtysh_install_default (QOS_DOMAIN_NODE);
	install_element_level (QOS_DOMAIN_NODE, &vtysh_exit_qos_mapping_domain_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (QOS_DOMAIN_NODE, &vtysh_quit_qos_mapping_domain_cmd, VISIT_LEVE, CMD_SYNC);
	install_node (&qos_phb_node, NULL);
	vtysh_install_default (QOS_PHB_NODE);
	install_element_level (QOS_PHB_NODE, &vtysh_exit_qos_mapping_phb_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (QOS_PHB_NODE, &vtysh_quit_qos_mapping_phb_cmd, VISIT_LEVE, CMD_SYNC);

 	install_element_level (CONFIG_NODE, &show_qos_mapping_domain_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
 	install_element_level (CONFIG_NODE, &show_qos_mapping_phb_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (QOS_DOMAIN_NODE, &show_qos_mapping_domain_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
 	install_element_level (QOS_PHB_NODE, &show_qos_mapping_phb_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_qos_mapping_configure_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);


	install_element_level(CONFIG_NODE, &qos_mapping_domain_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level(CONFIG_NODE, &no_qos_mapping_domain_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &qos_mapping_phb_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level(CONFIG_NODE, &no_qos_mapping_phb_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level(QOS_DOMAIN_NODE, &qos_cos_to_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_DOMAIN_NODE, &no_qos_cos_to_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_DOMAIN_NODE, &qos_tos_to_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_DOMAIN_NODE, &no_qos_tos_to_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_DOMAIN_NODE, &qos_exp_to_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_DOMAIN_NODE, &no_qos_exp_to_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_DOMAIN_NODE, &qos_dscp_to_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_DOMAIN_NODE, &no_qos_dscp_to_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level(QOS_PHB_NODE, &qos_queue_to_cos_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_PHB_NODE, &no_qos_queue_to_cos_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_PHB_NODE, &qos_queue_to_tos_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_PHB_NODE, &no_qos_queue_to_tos_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_PHB_NODE, &qos_queue_to_exp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_PHB_NODE, &no_qos_queue_to_exp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_PHB_NODE, &qos_queue_to_dscp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_PHB_NODE, &no_qos_queue_to_dscp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* QoS car */
	install_node (&qos_car_node, NULL);
	vtysh_install_default (QOS_CAR_NODE);
	install_element_level (QOS_CAR_NODE, &vtysh_exit_qos_car_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (QOS_CAR_NODE, &vtysh_quit_qos_car_cmd, VISIT_LEVE, CMD_SYNC);

	install_element_level(CONFIG_NODE, &show_qos_car_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level(CONFIG_NODE, &qos_car_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level(CONFIG_NODE, &no_qos_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_CAR_NODE, &qos_car_rate_limit_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_CAR_NODE, &qos_car_color_action_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_CAR_NODE, &qos_car_color_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_CAR_NODE, &no_qos_car_color_action_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* QoS mirror */
	install_node (&qos_mirror_node, NULL);
	vtysh_install_default (QOS_MIRROR_NODE);
	install_element_level (QOS_MIRROR_NODE, &vtysh_exit_qos_mirror_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (QOS_MIRROR_NODE, &vtysh_quit_qos_mirror_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level(QOS_MIRROR_NODE, &qos_mirror_to_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_MIRROR_NODE, &qos_mirror_to_gigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_MIRROR_NODE, &qos_mirror_to_xgigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_MIRROR_NODE, &qos_mirror_to_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_MIRROR_NODE, &qos_mirror_to_loopback_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_MIRROR_NODE, &no_qos_mirror_to_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_MIRROR_NODE, &no_qos_mirror_to_gigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_MIRROR_NODE, &no_qos_mirror_to_xgigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_MIRROR_NODE, &no_qos_mirror_to_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(QOS_MIRROR_NODE, &no_qos_mirror_to_loopback_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level(CONFIG_NODE, &qos_mirror_group_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level(CONFIG_NODE, &no_qos_mirror_group_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &show_qos_mirror_group_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_qos_mirror_if_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);


	install_element_level(LOOPBACK_IF_NODE, &qos_mirror_if_to_ethernet_egress_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &qos_mirror_if_to_gigabit_ethernet_egress_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &qos_mirror_if_to_xgigabit_ethernet_egress_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &no_qos_mirror_to_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &no_qos_mirror_to_gigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(LOOPBACK_IF_NODE, &no_qos_mirror_to_xgigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


	install_element_level(PHYSICAL_IF_NODE, &qos_mirror_to_group_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_qos_mirror_to_group_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &qos_mirror_if_to_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &qos_mirror_if_to_gigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &qos_mirror_if_to_xgigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &qos_mirror_if_to_ethernet_egress_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &qos_mirror_if_to_gigabit_ethernet_egress_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &qos_mirror_if_to_xgigabit_ethernet_egress_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &qos_mirror_if_to_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &qos_mirror_if_to_loopback_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_qos_mirror_to_trunk_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_qos_mirror_to_loopback_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_qos_mirror_to_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_qos_mirror_to_gigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_qos_mirror_to_xgigabit_ethernet_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* qos if */
	install_element_level(PHYSICAL_IF_NODE, &qos_car_apply_to_if_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &qos_car_config_if_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &no_qos_car_apply_to_if_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &qos_lr_config_if_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &no_qos_lr_config_if_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_SUBIF_NODE, &qos_car_apply_to_if_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_SUBIF_NODE, &qos_car_config_if_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_SUBIF_NODE, &no_qos_car_apply_to_if_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &qos_ifg_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_qos_ifg_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &show_qos_interface_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
	
	/* log */
#if 0
	install_element_level (CONFIG_NODE, &qos_log_level_ctl_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_show_log_level_ctl_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
#endif

	/*sla session*/
	install_node (&sla_session_node, NULL);
	vtysh_install_default (SLA_SESSION_NODE);
	install_element_level (SLA_SESSION_NODE, &vtysh_exit_sla_session_cmd, VISIT_LEVE, CMD_SYNC);
	
    install_element_level (CONFIG_NODE, &sla_session_get_cmd_vtysh, VISIT_LEVE, CMD_SYNC);
    install_element_level (CONFIG_NODE, &no_sla_session_get_cmd_vtysh, VISIT_LEVE, CMD_SYNC);
	
    install_element_level (SLA_SESSION_NODE, &sla_protocol_select_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_protocol_select_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_level (SLA_SESSION_NODE, &sla_ethernet_format_get_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_ethernet_format_get_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_level (SLA_SESSION_NODE, &sla_ip_format_get_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_ip_format_get_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_level (SLA_SESSION_NODE, &sla_y1564_packet_size_get_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_y1564_packet_size_get_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (SLA_SESSION_NODE, &sla_2544_packet_size_get_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_2544_packet_size_get_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_level (SLA_SESSION_NODE, &sla_packet_rate_get_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_packet_rate_get_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_level (SLA_SESSION_NODE, &sla_packet_loss_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_packet_loss_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_level (SLA_SESSION_NODE, &sla_packet_delay_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_packet_delay_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_level (SLA_SESSION_NODE, &sla_packet_throughput_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_packet_throughput_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (SLA_SESSION_NODE, &sla_y1564_test_type_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_y1564_test_type_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_level (SLA_SESSION_NODE, &sla_y1564_traffic_policing_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_y1564_traffic_policing_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (SLA_SESSION_NODE, &sla_packet_duration_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_packet_duration_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	
    install_element_level (PHYSICAL_IF_NODE, &sla_interface_session_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_sla_interface_session_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	/*ÃüÁîÍ¨ÖªÏÈºóË³Ðò*/
    //install_element_daemon_order_level ( PHYSICAL_IF_NODE, &sla_interface_session_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC, 2, VTYSH_QOS, VTYSH_IFM );
    //install_element_daemon_order_level ( PHYSICAL_IF_NODE, &no_sla_interface_session_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC, 2, VTYSH_QOS, VTYSH_IFM );
	
    install_element_level (SLA_SESSION_NODE, &sla_sac_cmd_vtysh, CONFIG_LEVE_5,  CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_sac_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_level (SLA_SESSION_NODE, &sla_bandwidth_profile_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (SLA_SESSION_NODE, &no_sla_bandwidth_profile_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	/*×Ó½Ó¿Ú*/
    install_element_level (PHYSICAL_SUBIF_NODE, &sla_sub_interface_session_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_SUBIF_NODE, &no_sla_sub_interface_session_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/*ÃüÁîÍ¨ÖªÏÈºóË³Ðò*/
    //install_element_daemon_order_level ( PHYSICAL_SUBIF_NODE, &sla_sub_interface_session_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC, 2, VTYSH_QOS, VTYSH_IFM );
    //install_element_daemon_order_level ( PHYSICAL_SUBIF_NODE, &no_sla_sub_interface_session_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC, 2, VTYSH_QOS, VTYSH_IFM );
	
    install_element_level (CONFIG_NODE, &sla_show_config_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &sla_show_session_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/*cpu car*/
	install_node (&qos_cpcar_node, NULL);
	vtysh_install_default (QOS_CPCAR_NODE);
	install_element_level (QOS_CPCAR_NODE, &vtysh_exit_qos_cpcar_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (QOS_CPCAR_NODE, &vtysh_quit_qos_cpcar_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_cpu_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &no_qos_cpu_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_cpu_car_outband_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &no_qos_cpu_car_outband_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_cpu_car_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &no_qos_cpu_car_queue_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_cpu_car_clear_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_cpu_car_clear_all_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &show_qos_cpu_car_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_qos_cpu_car_queue_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_qos_cpu_car_outband_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	/* hqos */
	install_node(&hqos_wred_node, NULL);
    vtysh_install_default(HQOS_WRED_NODE);
    install_element_level (HQOS_WRED_NODE, &vtysh_exit_qos_wred_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (HQOS_WRED_NODE, &vtysh_quit_qos_wred_cmd, VISIT_LEVE, CMD_SYNC);

    install_node(&hqos_queue_node, NULL);
    vtysh_install_default(HQOS_QUEUE_NODE);
    install_element_level (HQOS_QUEUE_NODE, &vtysh_exit_qos_queue_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (HQOS_QUEUE_NODE, &vtysh_quit_qos_queue_cmd, VISIT_LEVE, CMD_SYNC);

    install_element_level(CONFIG_NODE, &show_hqos_wred_profile_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_hqos_queue_profile_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

    install_element_level(CONFIG_NODE, &hqos_wred_profile_cmd_vtysh, VISIT_LEVE, CMD_SYNC);
    install_element_level(CONFIG_NODE, &no_hqos_wred_profile_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(HQOS_WRED_NODE, &hqos_wred_rule_cmd_vtysh, VISIT_LEVE, CMD_SYNC);
    install_element_level(HQOS_WRED_NODE, &no_hqos_wred_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

 	install_element_level(CONFIG_NODE, &hqos_queue_profile_cmd_vtysh, VISIT_LEVE, CMD_SYNC);
 	install_element_level(CONFIG_NODE, &no_hqos_queue_profile_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(HQOS_QUEUE_NODE, &hqos_queue_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(HQOS_QUEUE_NODE, &no_hqos_queue_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
 	install_element_level(HQOS_QUEUE_NODE, &hqos_queue_scheduler_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
 	install_element_level(HQOS_QUEUE_NODE, &no_hqos_queue_scheduler_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level(PHYSICAL_IF_NODE, &hqos_queue_apply_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(PHYSICAL_IF_NODE, &no_hqos_queue_apply_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (CONFIG_NODE, &qos_debug_monitor_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_qos_debug_monitor_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
}

