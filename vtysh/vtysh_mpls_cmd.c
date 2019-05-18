/**
 * \page cmds_ref_mpls MPLS
 * - \subpage modify_log_mpls
 * - \subpage mpls_cmd
 * - \subpage mpls_vsi_cmd
 * - \subpage mpls_lsrid_cmd_vtysh
 * - \subpage mpls_ldp_enable_cmd
 * - \subpage no_mpls_ldp_enable_cmd
 * - \subpage ingress_static_lsp_nexthop_cmd_vtysh
 * - \subpage ingress_static_lsp_ethernet_cmd_vtysh
 * - \subpage ingress_static_lsp_gigabit_ethernet_cmd_vtysh
 * - \subpage ingress_static_lsp_xgigabit_ethernet_cmd_vtysh
 * - \subpage ingress_static_lsp_trunk_cmd_vtysh
 * - \subpage transit_static_lsp_nexthop_cmd_vtysh
 * - \subpage transit_static_lsp_ethernet_cmd_vtysh
 * - \subpage transit_static_lsp_gigabit_ethernet_cmd_vtysh
 * - \subpage transit_static_lsp_xgigabit_ethernet_cmd_vtysh
 * - \subpage transit_static_lsp_trunk_cmd_vtysh
 * - \subpage egress_static_lsp_cmd_vtysh
 * - \subpage no_static_lsp_cmd_vtysh
 * - \subpage show_static_lsp_cmd_vtysh
 * - \subpage no_mpls_vsi_cmd_vtysh
 * - \subpage mpls_vsi_name_cmd_vtysh
 * - \subpage vsi_mac_learn_cmd_vtysh
 * - \subpage vsi_mac_learn_limit_cmd_vtysh
 * - \subpage no_vsi_mac_learn_limit_cmd_vtysh
 * - \subpage mpls_vsi_encapsulate_ethernet_raw_cmd_vtysh
 * - \subpage mpls_vsi_encapsulate_ethernet_tag_cmd_vtysh
 * - \subpage mpls_vsi_encapsulate_vlan_raw_cmd_vtysh
 * - \subpage mpls_vsi_encapsulate_vlan_tag_cmd_vtysh
 * - \subpage no_mpls_vsi_encapsulate_cmd_vtysh
 * - \subpage mpls_vsi_storm_suppress_cmd_vtysh
 * - \subpage no_mpls_vsi_storm_suppress_cmd_vtysh
 * - \subpage vsi_mac_blacklist_cmd_vtysh
 * - \subpage no_vsi_mac_blacklist_cmd_vtysh
 * - \subpage no_vsi_mac_blacklist_all_cmd_vtysh
 * - \subpage show_vsi_mac_blacklist_cmd_vtysh
 * - \subpage vsi_mac_whitelist_cmd_vtysh
 * - \subpage no_vsi_mac_whitelist_cmd_vtysh
 * - \subpage no_vsi_mac_whitelist_all_cmd_vtysh
 * - \subpage show_vsi_mac_whitelist_cmd_vtysh
 * - \subpage show_vsi_cmd_vtysh
 * - \subpage mpls_if_bind_vsi_cmd_vtysh
 * - \subpage mpls_if_unbind_vsi_cmd_vtysh
 * - \subpage tunnel_protocol_cmd_vtysh
 * - \subpage no_tunnel_protocol_cmd_vtysh
 * - \subpage tunnel_static_lsp_cmd_vtysh
 * - \subpage no_tunnel_static_lsp_cmd_vtysh
 * - \subpage show_tunnel_cmd_vtysh
 * - \subpage tunnel_source_destination_ip_cmd_vtysh
 * - \subpage no_tunnel_source_destination_ip_cmd_vtysh
 * - \subpage tunnel_outif_ethernet_dmac_vtysh
 * - \subpage tunnel_outif_gigabit_ethernet_dmac_vtysh
 * - \subpage tunnel_outif_xgigabit_ethernet_dmac_vtysh
 * - \subpage tunnel_outif_trunk_dmac_vtysh
 * - \subpage no_tunnel_outif_dmac_vtysh
 * - \subpage mpls_pw_name_cmd
 * - \subpage no_mpls_pw_name_cmd_vtysh
 * - \subpage mpls_pw_protocol_cmd_vtysh
 * - \subpage no_mpls_pw_protocol_cmd_vtysh
 * - \subpage mpls_pw_vc_type_cmd_vtysh
 * - \subpage no_mpls_pw_vc_type_cmd_vtysh
 * - \subpage mpls_pw_peer_vcid_cmd_vtysh
 * - \subpage no_mpls_pw_peer_vcid_cmd_vtysh
 * - \subpage mpls_pw_label_cmd_vtysh
 * - \subpage no_mpls_pw_label_cmd_vtysh
 * - \subpage mpls_pw_mtu_cmd_vtysh
 * - \subpage no_mpls_pw_mtu_cmd_vtysh
 * - \subpage mpls_pw_ctrl_word_cmd_vtysh
 * - \subpage mpls_pw_encapsulate_ethernet_raw_cmd_vtysh
 * - \subpage mpls_pw_encapsulate_ethernet_tag_cmd_vtysh
 * - \subpage mpls_pw_encapsulate_vlan_raw_cmd_vtysh
 * - \subpage mpls_pw_encapsulate_vlan_tag_cmd_vtysh
 * - \subpage no_mpls_pw_encapsulate_cmd_vtysh
 * - \subpage mpls_pw_ces_frame_cmd_vtysh
 * - \subpage no_mpls_pw_ces_frame_cmd_vtysh
 * - \subpage mpls_pw_bind_tunnel_cmd_vtysh
 * - \subpage mpls_pw_bind_lsp_cmd_vtysh
 * - \subpage no_mpls_pw_tunnel_bind_cmd_vtysh
 * - \subpage mpls_pw_tunnel_policy_cmd_vtysh
 * - \subpage no_mpls_pw_tunnel_policy_cmd_vtysh
 * - \subpage mpls_pw_switch_pw_cmd_vtysh
 * - \subpage no_mpls_pw_switch_pw_cmd_vtysh
 * - \subpage mpls_pw_switch_vsi_cmd_vtysh
 * - \subpage no_mpls_pw_switch_vsi_cmd_vtysh
 * - \subpage show_mpls_pw_cmd_vtysh
 * - \subpage show_mpls_pw_cmd_vtysh
 * - \subpage mpls_interface_pw_non_failback_cmd_vtysh
 * - \subpage mpls_if_bind_pw_cmd_vtysh
 * - \subpage no_mpls_if_bind_pw_cmd_vtysh
 * - \subpage show_mpls_interface_cmd_vtysh
 * - \subpage show_mpls_interface_ethernet_cmd_vtysh
 * - \subpage show_mpls_interface_gigabit_ethernet_cmd_vtysh
 * - \subpage show_mpls_interface_xgigabit_ethernet_cmd_vtysh
 * - \subpage mpls_exp_domain_enable_cmd_vtysh
 * - \subpage no_mpls_exp_domain_enable_cmd_vtysh
 * - \subpage mpls_exp_phb_enable_cmd_vtysh
 * - \subpage mpls_pw_exp_phb_enable_cmd_vtysh
 * - \subpage no_mpls_exp_phb_enable_cmd_vtysh
 * - \subpage mpls_pw_statistics_enable_cmd_vtysh
 * - \subpage no_mpls_pw_statistics_enable_cmd_vtysh
 * - \subpage mpls_pw_statistics_clear_cmd_vtysh
 * - \subpage show_mpls_pw_statistics_cmd_vtysh
 * - \subpage mpls_pw_mplstp_oam_session_enable_cmd_vtysh
 * - \subpage no_mpls_pw_mplstp_oam_session_enable_cmd_vtysh
 * - \subpage mpls_pw_bfd_session_enable_cmd_vtysh
 * - \subpage no_mpls_pw_bfd_session_enable_cmd_vtysh
 * - \subpage tunnel_statistics_enable_cmd_vtysh
 * - \subpage no_tunnel_statistics_enable_cmd_vtysh
 * - \subpage tunnel_statistics_clear_cmd_vtysh
 * - \subpage show_tunnel_statistics_cmd_vtysh
 * - \subpage tunnel_mplstp_oam_session_enable_cmd_vtysh
 * - \subpage no_tunnel_mplstp_oam_session_enable_cmd_vtysh
 * - \subpage tunnel_bfd_session_enable_tunnel_cmd_vtysh
 * - \subpage tunnel_bfd_session_enable_lsp_cmd_vtysh
 * - \subpage no_tunnel_bfd_session_enable_tunnel_cmd_vtysh
 * - \subpage no_tunnel_bfd_session_enable_lsp_cmd_vtysh
 * - \subpage mpls_qos_car_cmd_vtysh
 * - \subpage no_mpls_qos_car_cmd_vtysh
 * - \subpage mpls_hqos_queue_apply_cmd_vtysh
 * - \subpage no_mpls_hqos_queue_apply_cmd_vtysh
 * - \subpage ldp_enable_cmd
 * - \subpage no_ldp_enable_cmd
 * - \subpage ldp_label_policy_cmd
 * - \subpage no_ldp_label_policy_cmd
 * - \subpage ldp_advertise_mode_cmd
 * - \subpage no_ldp_advertise_mode_cmd
 * - \subpage ldp_keepalive_interval_cmd
 * - \subpage no_ldp_keepalive_interval_cmd
 * - \subpage ldp_debug_cmd
 * - \subpage no_ldp_debug_cmd
 * - \subpage ldp_show_config_cmd
 * - \subpage ldp_show_session_cmd
 * - \subpage ldp_show_adjance_cmd
 * - \subpage ldp_show_lsp_cmd
 * - \subpage ldp_show_statistics_cmd
 * - \subpage tunnel_non_failback_cmd_vtysh
 * - \subpage tunnel_failback_cmd_vtysh
 * - \subpage mpls_l3vpn_instance_vtysh
 * - \subpage no_mpls_l3vpn_instance_cmd_vtysh
 * - \subpage mpls_l3vpn_name_cmd_vtysh
 * - \subpage mpls_l3vpn_rd_cmd_vtysh
 * - \subpage mpls_l3vpn_target_cmd_vtysh
 * - \subpage mpls_l3vpn_target_arg_cmd_vtysh
 * - \subpage no_mpls_l3vpn_target_cmd_vtysh
 * - \subpage no_mpls_l3vpn_target_arg_cmd_vtysh
 * - \subpage no_mpls_l3vpn_target_all_cmd_vtysh
 * - \subpage mpls_l3vpn_apply_label_cmd_vtysh
 * - \subpage no_mpls_l3vpn_apply_label_cmd_vtysh
 * - \subpage mpls_l3vpn_show_cmd_vtysh
 * - \subpage mpls_l3vpn_show_route_cmd_vtysh
 * - \subpage mplsaps_session_cmd
 * - \subpage mplsaps_keep_alive_cmd
 * - \subpage mplsaps_hold_off_cmd
 * - \subpage mplsaps_backup_failback_cmd
 * - \subpage mplsaps_priority_cmd
 * - \subpage mplsaps_bind_pw_cmd
 * - \subpage mplsaps_bind_tunnel_cmd
 * - \subpage mplsaps_session_enable_cmd
 * - \subpage mplsaps_debug_packet_cmd
 * - \subpage mplsaps_admin_force_cmd
 * - \subpage mplsaps_admin_manual_cmd
 * - \subpage mplsaps_admin_lockout_cmd
 * - \subpage show_mplsaps_config_cmd
 * - \subpage show_mplsaps_session_cmd
 * - \subpage show_mplsaps_debug_cmd
 * - \subpage no_mpls_l3vpn_apply_label_cmd_vtysh
 * - \subpage no_mpls_l3vpn_apply_label_cmd_vtysh
 * - \page bfd_debug_cmd
 * - \page undo_bfd_debug_cmd
 * -\page bfd_priority_cmd
 * -\page undo_bfd_priority_cmd
 * -\page bfd_ttl_cmd
 * -\page undo_bfd_ttl_cmd
 * -\page bfd_session_vtysh
 * -\page undo_bfd_session_cmd
 * -\page bfd_session_local_disc_cmd
 * -\page undo_bfd_session_local_disc_cmd
 * -\page bfd_session_remote_disc_cmd
 * -\page undo_bfd_session_remote_disc_cmd
 * -\page bfd_session_cc_interval_cmd
 * -\page undo_bfd_session_cc_interval_cmd
 * -\page bfd_session_detect_multiplier_cmd
 * -\page undo_bfd_session_detect_multiplier_cmd
 * -\page bfd_intf_session_enable_cmd
 * -\page undo_bfd_intf_session_enable_cmd
 * -\page bfd_info_show_cmd
 *
 */

/**
 * \page modify_log_mpls Modify Log
 * \section mpls-v007r004 HiOS-V007R004
 *  -#
 * \section mpls-v007r003 HiOS-V007R003
 *  -#
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "ifm_common.h"

#include <mpls/bfd/bfd.h>


static struct cmd_node mpls_node =
{
    MPLS_NODE,
    "%s(config-mpls)# ",
};


static struct cmd_node vsi_node =
{
    VSI_NODE,
    "%s(mpls-vsi)# "
};


static struct cmd_node l3vpn_node =
{
    L3VPN_NODE,
    "%s(mpls-l3vpn)# ",
};

static struct cmd_node pw_node =
{
    PW_NODE,
    "%s(mpls-pw)# "
};

struct cmd_node bfd_template_node =
{
    BFD_TEMPLATE_NODE,
    "%s(config-bfd-template)# ",
    1
};

struct cmd_node bfd_session_node =
{
    BFD_SESSION_NODE,
    "%s(config-bfd-session)# ",
    1
};

struct cmd_node mplstp_oam_sess_node =
{
    MPLSTP_OAM_SESS_NODE,
    "%s(mplstp-oam-session)# ",
    1
};


struct cmd_node mplsaps_session_node =
{
    APS_NODE,
    "%s(config-mplsaps-session)# ",
    1
};

struct cmd_node mplstp_oam_meg_node =
{
    MPLSTP_OAM_MEG_NODE,
    "%s(mplstp-oam-meg)# ",
    1
};


/**
 * \page mpls_cmd mpls
 * - 功能说明 \n
 *   进入MPLS视图
 * - 命令格式 \n
 *   mpls
 * - 参数说明 \n
 *   �? * - 缺省情况 \n
 *   �? * - 命令模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     使用该命令进入MPLS视图，配置mpls相关业务
 *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     �? * - 使用举例 \n
 *     Huahuan(config)# mpls \n
 *     Huahuan(config-mpls)# \n
 *
 */
DEFUNSH (VTYSH_MPLS,
        mpls,
        mpls_cmd,
        "mpls",
        "Enter the MPLS view\n")
{
    vty->node = MPLS_NODE;
    return CMD_SUCCESS;
}

/**
 * \page mpls_vsi_cmd vsi <1-1024>
 * - 功能说明 \n
 *	 Mpls视图下创建或进入vsi实例
 * - 命令格式 \n
 *	 vsi <1-1024>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |--------|---------|
 *   |<1-1024>|Vsi实例号|
 *
 * - 缺省情况 \n
 *	 缺省情况下，vsi实例�? * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建或进入vsi实例
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Huahuan(config-mpls)#vsi 1
 *
 */
DEFUNSH (VTYSH_MPLS,
        mpls_vsi,
        mpls_vsi_cmd,
        "vsi <1-1024>",
        "Enter the VSI view\n"
        "Specify VSI ID:<1-1024>\n")
{
    char *pprompt = NULL;

    pprompt = vty->change_prompt;
    if (NULL != pprompt)
    {
        snprintf(pprompt, VTY_BUFSIZ, "%%s(mpls-vsi-%s)# ", argv[0]);
    }

    vty->node = VSI_NODE;
    return CMD_SUCCESS;
}


DEFUNSH (VTYSH_MPLS,
        vtysh_exit_mpls,
        vtysh_exit_mpls_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}


ALIAS (vtysh_exit_mpls,
        vtysh_quit_mpls_cmd,
        "quit",
        "Exit current mode and down to previous mode\n")


DEFUNSH (VTYSH_MPLS,
        vtysh_exit_mpls_vsi,
        vtysh_exit_mpls_vsi_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}


DEFUNSH (VTYSH_MPLS,
        vtysh_exit_mpls_l3vpn,
        vtysh_exit_mpls_l3vpn_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}


DEFUNSH (VTYSH_MPLS,
        vtysh_exit_mpls_pw,
        vtysh_exit_mpls_pw_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}

/**
 * \page mpls_lsrid_cmd_vtysh mpls lsr-id (A.B.C.D | X:X:X:X:X:X:X:X)
 * - 功能说明 \n
 *	 配置lsr-id
 * - 命令格式 \n
 *	 mpls lsr-id (A.B.C.D | X:X:X:X:X:X:X:X)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|---------|
 *	 |<1-1024>|Vsi实例号for IPv4|
 *   |X:X:X:X:X:X:X:X|Vsi实例号for IPv6|
 *
 * - 缺省情况 \n
 *	 lsr-id未配�? * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置lsr-id
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *     Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)#mpls lsr-id 1.1.1.1
 *
 */
DEFSH (VTYSH_MPLS, mpls_lsrid_cmd_vtysh,
        "mpls lsr-id (A.B.C.D | X:X:X:X:X:X:X:X)",
        "Configuration of lsr-id\n"
        "Specify LSR(Label Switched Router) identifier\n"
        "Lsr-id, format: A.B.C.D for IPv4\n"
        "Lsr-id, format: X:X:X:X:X:X:X:X for IPv6\n")

/**
 * \page mpls_ldp_enable_cmd mpls ldp enable
 * - 功能说明 \n
 *	 配置lsr-id
 * - 命令格式 \n
 *	 mpls ldp enable
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，LDP功能为关闭状�? * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   �? *	 - 前置条件 \n
 *	   配置LSR-ID
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show mpls ldp config
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)# mpls ldp enable
 *
 */
DEFSH (VTYSH_MPLS, mpls_ldp_enable_cmd,
        "mpls ldp enable",
        "Mpls view\n"
        "Label Distribution Protocol(LDP)\n"
        "Enable ldp global function\n")

/**
 * \page no_mpls_ldp_enable_cmd no mpls ldp enable
 * - 功能说明 \n
 *	 删除lsr-id
 * - 命令格式 \n
 *	 no mpls ldp enable
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，LDP功能为关闭状�? * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   �? *	 - 前置条件 \n
 *	   删除LSR-ID
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show mpls ldp config
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)# no mpls ldp enable
 *
 */

DEFSH (VTYSH_MPLS, no_mpls_ldp_enable_cmd,
        "no mpls ldp enable",
        NO_STR
        "Mpls view\n"
        "Label Distribution Protocol(LDP)\n"
        "Enable ldp global function\n")

/**
 * \page ingress_static_lsp_nexthop_cmd_vtysh mpls static-lsp NAME ingress destination A.B.C.D/M nexthop A.B.C.D outlabel <16-1015807>
 * - 功能说明 \n
 *	 配置静态lsp的INGRESS节点，指定下一�? * - 命令格式 \n
 *	 mpls static-lsp NAME ingress destination A.B.C.D/M nexthop A.B.C.D outlabel <16-1015807>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *   |A.B.C.D/M|目的地址和掩码长度，IP是点分十进制形式，掩码是大于0小于等于32的数字。如�?92.168.1.1/24|
 *   |A.B.C.D |静态lsp的下一跳IP地址|
 *   |<16-1015807>|静态lsp的出标签|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置lsp
 * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置ingress节点
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)#mpls static-lsp AtoB ingress destination 3.3.3.3/32 nexthop 10.1.1.2 out-label 100
 *
 */
DEFSH (VTYSH_MPLS, ingress_static_lsp_nexthop_cmd_vtysh,
        "mpls static-lsp NAME ingress destination "
        "A.B.C.D/M nexthop A.B.C.D out-label <16-1048575>",
        "Static LSP configuration\n"
        "LSP type\n"
        "LSP name--max.31\n"
        "Ingress node\n"
        "Destination address and mask\n"
        "Format A.B.C.D/M\n"
        "Next hop\n"
        "Format A.B.C.D\n"
        "Out label\n"
        "Out label:<16-1048575>\n")

/**
 * \page ingress_static_lsp_ethernet_cmd_vtysh mpls static-lsp NAME ingress destination A.B.C.D/M outif ethernet USP dmac DMAC out-label <16-1048575>
 * - 功能说明 \n
 *	 配置静态lsp的INGRESS节点，指定出接口和目的MAC
 * - 命令格式 \n
 *	 mpls static-lsp NAME ingress destination A.B.C.D/M outif ethernet USP dmac DMAC out-label <16-1048575>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *	 |A.B.C.D/M|目的地址和掩码长度，IP是点分十进制形式，掩码是大于0小于等于32的数字。如�?92.168.1.1/24|
 *	 |USP|百兆以太接口号|
 *	 |DMAC|目的MAC|
 *	 |<16-1015807>|静态lsp的出标签|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置lsp
 * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置ingress节点
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show mpls static-lsp NAME \n
 *	   no mpls static-lsp NAME \n
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)# mpls static-lsp AtoC ingress destination 4.4.4.4/32 outif ethernet 1/1/6 dmac 00:00:00:00:00:01 out-label 200
 *
 */
DEFSH (VTYSH_MPLS, ingress_static_lsp_ethernet_cmd_vtysh,
        "mpls static-lsp NAME ingress destination "
        "A.B.C.D/M outif ethernet USP dmac DMAC out-label <16-1048575>",
        "Static LSP configuration\n"
        "LSP type\n"
        "LSP name--max.31\n"
        "Ingress node\n"
        "Destination ipaddress and mask\n"
        "Format A.B.C.D/M\n"
        "Outgoing interface\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        "Destination MAC\n"
        "Format HH:HH:HH:HH:HH:HH\n"
        "Out label\n"
        "Out label:<16-1048575>\n")
/**
 * \page ingress_static_lsp_gigabit_ethernet_cmd_vtysh mpls static-lsp NAME ingress destination A.B.C.D/M outif gigabitethernet USP dmac DMAC out-label <16-1048575>
 * - 功能说明 \n
 *	 配置静态lsp的INGRESS节点，指定出接口和目的MAC
 * - 命令格式 \n
 *	 mpls static-lsp NAME ingress destination A.B.C.D/M outif gigabitethernet USP dmac DMAC out-label <16-1048575>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *	 |A.B.C.D/M|目的地址和掩码长度，IP是点分十进制形式，掩码是大于0小于等于32的数字。如�?92.168.1.1/24|
 *	 |USP|千兆以太接口号|
 *	 |DMAC|目的MAC|
 *   |<16-1015807>|静态lsp的出标签|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置lsp
 * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置ingress节点
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show mpls static-lsp NAME \n
 *     no mpls static-lsp NAME \n
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)# mpls static-lsp AtoC ingress destination 4.4.4.4/32 outif gigabitethernet 1/1/6 dmac 00:00:00:00:00:01 out-label 200
 *
 */
DEFSH (VTYSH_MPLS, ingress_static_lsp_gigabit_ethernet_cmd_vtysh,
        "mpls static-lsp NAME ingress destination "
        "A.B.C.D/M outif gigabitethernet USP dmac DMAC out-label <16-1048575>",
        "Static LSP configuration\n"
        "LSP type\n"
        "LSP name--max.31\n"
        "Ingress node\n"
        "Destination ipaddress and mask\n"
        "Format A.B.C.D/M\n"
        "Outgoing interface\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        "Destination MAC\n"
        "Format HH:HH:HH:HH:HH:HH\n"
        "Out label\n"
        "Out label:<16-1048575>\n")

/**
 * \page ingress_static_lsp_xgigabit_ethernet_cmd_vtysh mpls static-lsp NAME ingress destination A.B.C.D/M outif xgigabitethernet USP dmac DMAC out-label <16-1048575>
 * - 功能说明 \n
 *	 配置静态lsp的INGRESS节点，指定出接口和目的MAC
 * - 命令格式 \n
 *	 mpls static-lsp NAME ingress destination A.B.C.D/M outif xgigabitethernet USP dmac DMAC out-label <16-1048575>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *	 |A.B.C.D/M|目的地址和掩码长度，IP是点分十进制形式，掩码是大于0小于等于32的数字。如�?92.168.1.1/24|
 *	 |USP|万兆以太接口号|
 *	 |DMAC|目的MAC|
 *	 |<16-1015807>|静态lsp的出标签|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置lsp
 * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置ingress节点
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show mpls static-lsp NAME \n
 *	   no mpls static-lsp NAME \n
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)# mpls static-lsp AtoC ingress destination 4.4.4.4/32 outif xgigabitethernet 1/1/6 dmac 00:00:00:00:00:01 out-label 200
 *
 */
DEFSH (VTYSH_MPLS, ingress_static_lsp_xgigabit_ethernet_cmd_vtysh,
        "mpls static-lsp NAME ingress destination "
        "A.B.C.D/M outif xgigabitethernet USP dmac DMAC out-label <16-1048575>",
        "Static LSP configuration\n"
        "LSP type\n"
        "LSP name--max.31\n"
        "Ingress node\n"
        "Destination ipaddress and mask\n"
        "Format A.B.C.D/M\n"
        "Outgoing interface\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        "Destination MAC\n"
        "Format HH:HH:HH:HH:HH:HH\n"
        "Out label\n"
        "Out label:<16-1048575>\n")

/**
 * \page ingress_static_lsp_trunk_cmd_vtysh mpls static-lsp NAME ingress destination A.B.C.D/M outif trunk TRUNK dmac DMAC out-label <16-1048575>
 * - 功能说明 \n
 *	 配置静态lsp的INGRESS节点，指定出接口和目的MAC
 * - 命令格式 \n
 *	 mpls static-lsp NAME ingress destination A.B.C.D/M outif trunk TRUNK dmac DMAC out-label <16-1048575>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *	 |A.B.C.D/M|目的地址和掩码长度，IP是点分十进制形式，掩码是大于0小于等于32的数字。如�?92.168.1.1/24|
 *	 |TRUNK|Trunk接口号|
 *	 |DMAC|目的MAC|
 *	 |<16-1015807>|静态lsp的出标签|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置lsp
 * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置ingress节点
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show mpls static-lsp NAME \n
 *	   no mpls static-lsp NAME \n
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)# mpls static-lsp AtoC ingress destination 4.4.4.4/32 outif trunk 1 dmac 00:00:00:00:00:01 out-label 200
 *
 */
DEFSH (VTYSH_MPLS, ingress_static_lsp_trunk_cmd_vtysh,
        "mpls static-lsp NAME ingress destination "
        "A.B.C.D/M outif trunk TRUNK dmac DMAC out-label <16-1048575>",
        "Static LSP configuration\n"
        "LSP type\n"
        "LSP name--max.31\n"
        "Ingress node\n"
        "Destination ipaddress and mask\n"
        "Format A.B.C.D/M\n"
        "Outgoing interface\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        "Destination MAC\n"
        "Format HH:HH:HH:HH:HH:HH\n"
        "Out label\n"
        "Out label:<16-1048575>\n")

/**
 * \page transit_static_lsp_nexthop_cmd_vtysh mpls static-lsp NAME transit destination A.B.C.D/M nexthop A.B.C.D in-label <16-1048575> out-label <16-1048575>
 * - 功能说明 \n
 *	 配置静态lsp的TRANSIT节点，指定下一�? * - 命令格式 \n
 *	 mpls static-lsp NAME transit destination A.B.C.D/M nexthop A.B.C.D in-label <16-1048575> out-label <16-1048575>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *	 |A.B.C.D/M|目的地址和掩码长度，IP是点分十进制形式，掩码是大于0小于等于32的数字。如�?92.168.1.1/24|
 *	 |A.B.C.D |静态lsp的下一跳IP地址|
 *	 |<16-1015807>|静态lsp的入标签|
 *	 |<16-1015807>|静态lsp的出标签|
 *
 * - 缺省情况 \n
 *	 缺省情况下，节点未配置lsp
 * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置transit节点
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show mpls static-lsp NAME \n
 *	   no mpls static-lsp NAME \n
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)# mpls static-lsp AtoB transit destination 3.3.3.3/32 nexthop 10.2.2.2 in-label 100 out-label 300
 *
 */
DEFSH (VTYSH_MPLS, transit_static_lsp_nexthop_cmd_vtysh,
        "mpls static-lsp NAME transit destination A.B.C.D/M nexthop A.B.C.D "
        "in-label <16-1048575> out-label <16-1048575>",
        "Static LSP configuration\n"
        "LSP type\n"
        "LSP name--max.31\n"
        "Transit node\n"
        "Destination ipaddress and mask\n"
        "Format A.B.C.D/M\n"
        "Next hop\n"
        "Format A.B.C.D\n"
        "In label\n"
        "In label:<16-1048575>\n"
        "Out label\n"
        "Out label:<16-1048575>\n")

/**
 * \page transit_static_lsp_ethernet_cmd_vtysh mpls static-lsp NAME transit destination A.B.C.D/M outif ethernet USP dmac DMAC in-label <16-1048575> out-label <16-1048575>
 * - 功能说明 \n
 *	 配置静态lsp的TRANSIT节点，指定出接口和目的MAC
 * - 命令格式 \n
 *	 mpls static-lsp NAME transit destination A.B.C.D/M outif ethernet USP dmac DMAC in-label <16-1048575> out-label <16-1048575>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *	 |A.B.C.D/M|目的地址和掩码长度，IP是点分十进制形式，掩码是大于0小于等于32的数字。如�?92.168.1.1/24|
 *	 |USP|百兆以太接口号|
 *	 |<16-1015807>|静态lsp的入标签|
 *	 |<16-1015807>|静态lsp的出标签|
 *
 * - 缺省情况 \n
 *	 缺省情况下，节点未配置lsp
 * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置transit节点
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show mpls static-lsp NAME \n
 *	   no mpls static-lsp NAME \n
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)# mpls static-lsp AtoC transit destination 4.4.4.4/32 outif ethernet 1/1/6 dmac 00:00:00:00:00:02 in-label 200 out-label 400
 *
 */
DEFSH (VTYSH_MPLS, transit_static_lsp_ethernet_cmd_vtysh,
        "mpls static-lsp NAME transit destination A.B.C.D/M outif ethernet USP "
        "dmac DMAC in-label <16-1048575> out-label <16-1048575>",
        "Static LSP configuration\n"
        "LSP type\n"
        "LSP name--max.31\n"
        "Transit node\n"
        "Destination ipaddress/mask\n"
        "Format A.B.C.D/M\n"
        "Outgoing interface\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        "Destination MAC\n"
        "Format HH:HH:HH:HH:HH:HH\n"
        "In label\n"
        "In label:<16-1048575>\n"
        "Out label\n"
        "Out label:<16-1048575>\n")

/**
 * \page transit_static_lsp_gigabit_ethernet_cmd_vtysh mpls static-lsp NAME transit destination A.B.C.D/M outif gigabitethernet USP dmac DMAC in-label <16-1048575> out-label <16-1048575>
 * - 功能说明 \n
 *	 配置静态lsp的TRANSIT节点，指定出接口和目的MAC
 * - 命令格式 \n
 *	 mpls static-lsp NAME transit destination A.B.C.D/M outif gigabitethernet USP dmac DMAC in-label <16-1048575> out-label <16-1048575>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *	 |A.B.C.D/M|目的地址和掩码长度，IP是点分十进制形式，掩码是大于0小于等于32的数字。如�?92.168.1.1/24|
 *	 |USP|千兆以太接口号|
 *	 |<16-1015807>|静态lsp的入标签|
 *	 |<16-1015807>|静态lsp的出标签|
 *
 * - 缺省情况 \n
 *	 缺省情况下，节点未配置lsp
 * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置transit节点
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show mpls static-lsp NAME \n
 *	   no mpls static-lsp NAME \n
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)# mpls static-lsp AtoC transit destination 4.4.4.4/32 outif gigabitethernet 1/1/6 dmac 00:00:00:00:00:02 in-label 200 out-label 400
 *
 */
DEFSH (VTYSH_MPLS, transit_static_lsp_gigabit_ethernet_cmd_vtysh,
        "mpls static-lsp NAME transit destination A.B.C.D/M outif gigabitethernet USP "
        "dmac DMAC in-label <16-1048575> out-label <16-1048575>",
        "Static LSP configuration\n"
        "LSP type\n"
        "LSP name--max.31\n"
        "Transit node\n"
        "Destination ipaddress/mask\n"
        "Format A.B.C.D/M\n"
        "Outgoing interface\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        "Destination MAC\n"
        "Format HH:HH:HH:HH:HH:HH\n"
        "In label\n"
        "In label:<16-1048575>\n"
        "Out label\n"
        "Out label:<16-1048575>\n")

/**
 * \page transit_static_lsp_xgigabit_ethernet_cmd_vtysh mpls static-lsp NAME transit destination A.B.C.D/M outif xgigabitethernet USP dmac DMAC in-label <16-1048575> out-label <16-1048575>
 * - 功能说明 \n
 *	 配置静态lsp的TRANSIT节点，指定出接口和目的MAC
 * - 命令格式 \n
 *	 mpls static-lsp NAME transit destination A.B.C.D/M outif xgigabitethernet USP dmac DMAC in-label <16-1048575> out-label <16-1048575>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *	 |A.B.C.D/M|目的地址和掩码长度，IP是点分十进制形式，掩码是大于0小于等于32的数字。如�?92.168.1.1/24|
 *	 |USP|万兆以太接口号|
 *	 |<16-1015807>|静态lsp的入标签|
 *	 |<16-1015807>|静态lsp的出标签|
 *
 * - 缺省情况 \n
 *	 缺省情况下，节点未配置lsp
 * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置transit节点
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show mpls static-lsp NAME \n
 *	   no mpls static-lsp NAME \n
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)# mpls static-lsp AtoC transit destination 4.4.4.4/32 outif xgigabitethernet 1/1/6 dmac 00:00:00:00:00:02 in-label 200 out-label 400
 *
 */
DEFSH (VTYSH_MPLS, transit_static_lsp_xgigabit_ethernet_cmd_vtysh,
        "mpls static-lsp NAME transit destination A.B.C.D/M outif xgigabitethernet USP "
        "dmac DMAC in-label <16-1048575> out-label <16-1048575>",
        "Static LSP configuration\n"
        "LSP type\n"
        "LSP name--max.31\n"
        "Transit node\n"
        "Destination ipaddress/mask\n"
        "Format A.B.C.D/M\n"
        "Outgoing interface\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        "Destination MAC\n"
        "Format HH:HH:HH:HH:HH:HH\n"
        "In label\n"
        "In label:<16-1048575>\n"
        "Out label\n"
        "Out label:<16-1048575>\n")

/**
 * \page transit_static_lsp_trunk_cmd_vtysh mpls static-lsp NAME transit destination A.B.C.D/M outif trunk TRUNK dmac DMAC in-label <16-1048575> out-label <16-1048575>
 * - 功能说明 \n
 *	 配置静态lsp的TRANSIT节点，指定出接口和目的MAC
 * - 命令格式 \n
 *	 mpls static-lsp NAME transit destination A.B.C.D/M outif trunk TRUNK dmac DMAC in-label <16-1048575> out-label <16-1048575>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *	 |A.B.C.D/M|目的地址和掩码长度，IP是点分十进制形式，掩码是大于0小于等于32的数字。如�?92.168.1.1/24|
 *	 |TRUNK|trunk接口号|
 *	 |<16-1015807>|静态lsp的入标签|
 *	 |<16-1015807>|静态lsp的出标签|
 *
 * - 缺省情况 \n
 *	 缺省情况下，节点未配置lsp
 * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置transit节点
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show mpls static-lsp NAME \n
 *	   no mpls static-lsp NAME \n
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)# mpls static-lsp AtoC transit destination 4.4.4.4/32 outif trunk 1 dmac 00:00:00:00:00:02 in-label 200 out-label 400
 *
 */
DEFSH (VTYSH_MPLS, transit_static_lsp_trunk_cmd_vtysh,
        "mpls static-lsp NAME transit destination A.B.C.D/M outif trunk TRUNK "
        "dmac DMAC in-label <16-1048575> out-label <16-1048575>",
        "Static LSP configuration\n"
        "LSP type\n"
        "LSP name--max.31\n"
        "Transit node\n"
        "Destination ipaddress/mask\n"
        "Format A.B.C.D/M\n"
        "Outgoing interface\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        "Destination MAC\n"
        "Format HH:HH:HH:HH:HH:HH\n"
        "In label\n"
        "In label:<16-1048575>\n"
        "Out label\n"
        "Out label:<16-1048575>\n")

/**
 * \page egress_static_lsp_cmd_vtysh mpls static-lsp NAME egress destination A.B.C.D/M in-label <16-1048575>
 * - 功能说明 \n
 *	 配置静态lsp的EGRESS节点
 * - 命令格式 \n
 *	 mpls static-lsp NAME egress destination A.B.C.D/M in-label <16-1048575>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *	 |A.B.C.D/M|目的地址和掩码长度，IP是点分十进制形式，掩码是大于0小于等于32的数字。如�?92.168.1.1/24|
 *	 |<16-1015807>|静态lsp的入标签|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置lsp
 * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置egress节点
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show mpls static-lsp NAME \n
 *	   no mpls static-lsp NAME \n
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# \n
 *	   Huahuan(config-mpls)# mpls static-lsp AtoB egress destination 3.3.3.3/32 in-label 300
 *
 */
DEFSH (VTYSH_MPLS, egress_static_lsp_cmd_vtysh,
        "mpls static-lsp NAME egress destination A.B.C.D/M in-label <16-1048575>",
        "Static LSP configuration\n"
        "LSP type\n"
        "LSP name--max.31\n"
        "Egress node\n"
        "Destination ipaddress and mask\n"
        "Format A.B.C.D/M\n"
        "In label\n"
        "In label:<16-1048575>\n")

/**
 * \page no_static_lsp_cmd_vtysh no mpls static-lsp NAME
 * - 功能说明 \n
 *	 删除一个节点的LSP配置
 * - 命令格式 \n
 *	 no mpls static-lsp NAME
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |-----|--------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *
 * - 缺省情况 \n
 *	 缺省情况下，节点未配置lsp，配置后未保存，下次启动配置丢失
 * - 命令模式 \n
 *	 mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除一个节点的配置
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   mpls static-lsp NAME ingress destination A.B.C.D/M nexthop A.B.C.D outlabel <16-1015807> \n
 *     mpls static-lsp NAME transit destination A.B.C.D/M nexthop A.B.C.D inlabel <16-1015807> outlabel <16-1015807> \n
 *     mpls static-lsp NAME egress destination A.B.C.D/M inlabel <16-1015807> \n
 * - 使用举例 \n
 *	   Huahuan(config-mpls)#no mpls static-lsp AtoB
 *
 */
DEFSH (VTYSH_MPLS, no_static_lsp_cmd_vtysh,
        "no mpls static-lsp NAME",
        NO_STR
        "Mpls module\n"
        "LSP type\n"
        "LSP name--max.31\n")

/**
 * \page show_static_lsp_cmd_vtysh show mpls static-lsp [NAME]
 * - 功能说明 \n
 *	 显示一个节点的LSP配置
 * - 命令格式 \n
 *	 show mpls static-lsp [NAME]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |----|--------------|
 *	 |NAME|静态lsp名字，由小于32个字符的字符串组成|
 *
 * - 缺省情况 \n
 *	 缺省情况下，节点未配置lsp，配置后未保存，下次启动配置丢失
 * - 命令模式 \n
 *	 Config视图，mpls视图，接口视�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示一个节点的配置
 *	 - 前置条件 \n
 *	   lsr-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   mpls static-lsp NAME ingress destination A.B.C.D/M nexthop A.B.C.D outlabel <16-1015807> \n
 *	   mpls static-lsp NAME transit destination A.B.C.D/M nexthop A.B.C.D inlabel <16-1015807> outlabel <16-1015807> \n
 *	   mpls static-lsp NAME egress destination A.B.C.D/M inlabel <16-1015807> \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, show_static_lsp_cmd_vtysh,
        "show mpls static-lsp [NAME]",
        "Show configure infomation\n"
        "Mpls\n"
        "LSP type\n"
        "LSP name--max.31\n")

DEFUNSH (VTYSH_MPLS,
        bfd_session_exit_vtysh,
        bfd_session_exit_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}

/* DEFSH (VTYSH_MPLS,
        enable_bfd_cmd,
        "bfd enable",
        BFD_STR
        "Enable BFD global function\n")


DEFSH (VTYSH_MPLS,
        disable_bfd_cmd,
        "no bfd enable",
        NO_STR
        BFD_STR
        "disable BFD global function\n")
 */

/**
 * \page bfd_debug_cmd debug bfd (packet | fsm)
 * - 功能说明 \n
 *	 打开bfd协商报文或bfd状态机开�? * - 命令格式 \n
 *	 debug bfd packet
 	 debug bfd fsm
 * - 参数说明 \n
 *	 |参数    |说明		   |
 *	 |------|--------------|
 *	 |packet |bfd 的协商报文打印|
 *	 |fsm   |bfd 的状态机流程打印|
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令打开bfd的协商报文或状态机流程的打�? *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config)#debug bfd packet 
 *
 */
DEFSH (VTYSH_MPLS,
        bfd_debug_cmd,
        "debug bfd (packet | fsm)",
        DEBUG_STR
        BFD_STR
        "BFD packet\n"
        "BFD session fsm\n")

/**
 * \page undo_bfd_debug_cmd no debug bfd (packet | fsm)
 * - 功能说明 \n
 *	 关闭bfd协商报文或bfd状态机开�? * - 命令格式 \n
 *	 no debug bfd packet
	 no debug bfd fsm
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 |packet |bfd 的协商报文打印|
 *	 |fsm	|bfd 的状态机流程打印|
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令关闭bfd的协商报文或状态机流程的打�? *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config)#no debug bfd packet 
 *
 */
DEFSH (VTYSH_MPLS,
        undo_bfd_debug_cmd,
        "no debug bfd (packet | fsm)",
        NO_STR
        DEBUG_STR
        BFD_STR
        "BFD packet\n"
        "BFD session fsm\n")

/**
 * \page bfd_priority_cmd
 * - 功能说明 \n
 *	 设置bfd报文优先�? * - 命令格式 \n
 *	bfd priority <0-7>
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 |<0-7> |bfd 报文的优先级|
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令设置bfd报文的优先级
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config)#bfd priority 5 
 *
 */
DEFSH (VTYSH_MPLS,
        bfd_priority_cmd,
        "bfd priority <0-7>",
        BFD_STR
        "BFD priority\n"
        "BFD priority value, default is 7\n")

/**
 * \page undo_bfd_priority_cmd
 * - 功能说明 \n
 *	 取消bfd报文优先级设置，恢复默认
 * - 命令格式 \n
 *	no bfd priority
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *		�? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令取消bfd报文的优先级设置
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config)#no bfd priority
 *
 */
DEFSH (VTYSH_MPLS,
	undo_bfd_priority_cmd,
	"no bfd priority",
	BFD_STR
	"BFD priority\n"
	"BFD priority value, default is 7\n"
	)


/**
 * \page bfd_up_down_clear_cmd
 * - 功能说明 \n
 *	 设置bfd会话下up/down 和收发报文计�? * - 命令格式 \n
 *	bfd status clear
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 �? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令清楚bfd会话的状�? *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config)#bfd status clear 
 *
 */
DEFSH (VTYSH_MPLS,
	bfd_up_down_clear_cmd,
	"bfd status clear",
	BFD_STR
	"BFD up/down counter\n")

/**
 * \page bfd_ttl_cmd
 * - 功能说明 \n
 *	 设置bfd报文ip头的ttl
 * - 命令格式 \n
 *	bfd ttl <0-255>
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 |<0-255> |bfd 报文ip头的ttl|
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令设置bfd报文ip头的ttl
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config)#bfd ttl 255 
 *
 */
DEFSH (VTYSH_MPLS,
        bfd_ttl_cmd,
        "bfd ttl <1-255>",
        BFD_STR
        "BFD ttl\n"
        "BFD ttl value, default is 255\n")

/**
 * \page undo_bfd_ttl_cmd
 * - 功能说明 \n
 *	 取消设置bfd报文ip头的ttl，恢复默认ttl�?55
 * - 命令格式 \n
 *	no bfd ttl
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 	�? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令取消设置bfd报文ip头的ttl
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config)#no bfd ttl 
 *
 */
DEFSH (VTYSH_MPLS,
	undo_bfd_ttl_cmd,
	"no bfd ttl",
	BFD_STR
	"BFD ttl\n"
	"BFD ttl value, default is 255\n"
	)
		

/**
 * \page bfd_session_vtysh
 * - 功能说明 \n
 *	 创建或进入bfd会话
 * - 命令格式 \n
 *	bfd session <1-65535>
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 |<1-65535> |bfd的会话号|
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建或进入bfd会话
 *	 - 前置条件 \n
 *	   在config模式�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config)#bfd session 3
 *
 */
DEFUNSH (VTYSH_MPLS,
        bfd_session_vtysh,
        bfd_session_cmd,
        "bfd session <1-65535>",
        BFD_STR
        "BFD session\n"
        "config session id, <1-65535>")
{
    uint16_t sess_id = 0;
    char *pprompt = NULL;

    /*åˆ¤æ–­è¾“å…¥å‚æ•°æ˜¯å¦åˆæ�?*/
    VTY_GET_INTEGER_RANGE ( "session", sess_id, argv[0], 1, 65535 );

    /*è¿›å…¥ä¼šè¯è§†å›�? */
    vty->node = BFD_SESSION_NODE;
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt [bfd-session-xx]*/
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-bfd-session-%d)#", sess_id);
    }

    vty->index = (void *)(uint32_t)sess_id;

    return CMD_SUCCESS;
}

/**
 * \page undo_bfd_session_cmd
 * - 功能说明 \n
 *	 创建或进入bfd会话
 * - 命令格式 \n
 *	bfd session <1-65535>
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 |<1-65535> |bfd的会话号|
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除bfd会话
 *	 - 前置条件 \n
 *	   在config模式�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config)#no bfd session 3
 *
 */
DEFSH (VTYSH_MPLS,
        undo_bfd_session_cmd,
        "no bfd session <1-65535>",
        NO_STR
        BFD_STR
        "BFD session\n"
        "config session id, <1-65535>")

/**
 * \page bfd_session_local_disc_cmd
 * - 功能说明 \n
 *	 创建bfd会话的本端标识号
 * - 命令格式 \n
 *	local-discriminator <1-4294967295>
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 |<1-4294967295> |bfd会话的本端标识号|
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 bfd会话视图�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建bfd会话的本端标识号
 *	 - 前置条件 \n
 *	   �?bfd会话视图�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config-bfd-session-1)#local-discriminator 5
 *
 */
DEFSH (VTYSH_MPLS,
		bfd_session_local_disc_cmd,
		"local-discriminator <1-4294967295>",
		"Set BFD session local discriminator\n"
		"local discriminator, <1-4294967295>\n")

/**
 * \page undo_bfd_session_local_disc_cmd
 * - 功能说明 \n
 *	 删除bfd会话的本端标识号
 * - 命令格式 \n
 *	no local-discriminator
 * - 参数说明 \n
  *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 �? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 bfd会话视图�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除bfd会话的本端标识号
 *	 - 前置条件 \n
 *	   �?bfd会话视图�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config-bfd-session-1)#no local-discriminator
 *
 */
DEFSH (VTYSH_MPLS,
		undo_bfd_session_local_disc_cmd,
		"no local-discriminator",
		NO_STR
		"Set BFD session local discriminator\n")

/**
 * \page bfd_session_remote_disc_cmd
 * - 功能说明 \n
 *	 创建bfd会话的远端标识号
 * - 命令格式 \n
 *	remote-discriminator <1-4294967295>
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 |<1-4294967295> |bfd会话的远端标识号|
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 bfd会话视图�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建bfd会话的远端标识号
 *	 - 前置条件 \n
 *	   �?bfd会话视图�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config-bfd-session-1)#remote-discriminator 3
 *
 */
DEFSH (VTYSH_MPLS,
        bfd_session_remote_disc_cmd,
        "remote-discriminator <1-4294967295>",
        "Set BFD session remote discriminator\n"
        "remote discriminator, <1-4294967295>\n")

/**
 * \page undo_bfd_session_remote_disc_cmd
 * - 功能说明 \n
 *	 删除bfd会话的远端标识号
 * - 命令格式 \n
 *	no remote-discriminator
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 �? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 bfd会话视图�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除bfd会话的远端标识号
 *	 - 前置条件 \n
 *	   �?bfd会话视图�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config-bfd-session-1)#no remote-discriminator
 *
 */
DEFSH (VTYSH_MPLS,
        undo_bfd_session_remote_disc_cmd,
        "no remote-discriminator",
        NO_STR
        "Set BFD session remote discriminator\n")

/**
 * \page bfd_session_cc_interval_cmd
 * - 功能说明 \n
 *	 配置bfd会话发送的报文间隔
 * - 命令格式 \n
 *	no remote-discriminator
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 (3 | 10 | 20 | 100 | 300 | 1000) | bfd检测报文的发送间�? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 bfd会话视图下或config模式�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置bfd会话的检测报文发送周期，默认�?.3ms
 *	 - 前置条件 \n
 *	   �?bfd会话视图下或config视图�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   在config模式下配置为全局使用，而bfd会话视图下为专一使用，bfd会话的专一配置优先于全局配置
 *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config-bfd-session-1)#bfd cc-interval 3
 *     Hios(config)#bfd cc-interval 3
 *
 */
DEFSH (VTYSH_MPLS,
        bfd_session_cc_interval_cmd,
        "bfd cc-interval (3 | 10 | 20 | 100 | 300 | 1000)",
        BFD_STR
        "Set BFD session minimum receive interval\n"
        "3.3ms\n"
        "10ms\n"
        "20ms\n"
        "100ms\n"
        "300ms\n"
        "1000ms, default is 1000ms\n")

/**
 * \page undo_bfd_session_cc_interval_cmd
 * - 功能说明 \n
 *	 删除配置的bfd会话检测报文的发送间�? * - 命令格式 \n
 *	no bfd cc-interval
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 �? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 bfd会话视图下或config模式�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除配置的bfd会话的检测报文发送周�? *	 - 前置条件 \n
 *	   �?bfd会话视图下或config视图�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   在config模式下配置为全局使用，而bfd会话视图下为专一使用，bfd会话的专一配置优先于全局配置
 *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config-bfd-session-1)#no bfd cc-interval
 *	   Hios(config)#no bfd cc-interval
 *
 */
DEFSH (VTYSH_MPLS,
        undo_bfd_session_cc_interval_cmd,
        "no bfd cc-interval",
        NO_STR
        BFD_STR
        "Set BFD session minimum receive interval\n")

/**
 * \page bfd_session_detect_multiplier_cmd
 * - 功能说明 \n
 *	 配置的bfd会话的超时检测周期，默認配置�?
 * - 命令格式 \n
 *	bfd detect-multiplier <1-10>
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 <1-10> | bfd會話的超时检测周�? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 bfd会话视图下或config模式�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置bfd会话的超时检测周�? *	 - 前置条件 \n
 *	   �?bfd会话视图下或config视图�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   在config模式下配置为全局使用，而bfd会话视图下为专一使用，bfd会话的专一配置优先于全局配置
 *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config-bfd-session-1)#bfd detect-multiplier 3
 *	   Hios(config)#bfd detect-multiplier 3
 *
 */
DEFSH (VTYSH_MPLS,
        bfd_session_detect_multiplier_cmd,
        "bfd detect-multiplier <1-10>",
        BFD_STR
        "Set BFD session detect multiplier\n"
        "Detect multiplier, default is 5\n")

/**
 * \page undo_bfd_session_detect_multiplier_cmd
 * - 功能说明 \n
 *	 刪除bfd会话的超时检测周�? * - 命令格式 \n
 *	no bfd detect-multiplier
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 �? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 bfd会话视图下或config模式�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令刪除bfd会话配置的超時檢測週期
 *	 - 前置条件 \n
 *	   �?bfd会话视图下或config视图�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   在config模式下配置为全局使用，而bfd会话视图下为专一使用，bfd会话的专一配置优先于全局配置
 *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config-bfd-session-1)#bfd detect-multiplier 3
 *	   Hios(config)#bfd detect-multiplier 3
 *
 */
DEFSH (VTYSH_MPLS,
        undo_bfd_session_detect_multiplier_cmd,
        "no bfd detect-multiplier",
        NO_STR
        BFD_STR
        "Set BFD session detect multiplier\n")

#if 0
DEFSH (VTYSH_MPLS,
        bfd_session_delay_up_cmd,
        "bfd wtr <0-3600>",
        BFD_STR
        "BFD session WTR(Wait To Restore) time\n"
        "Specify the WTR interval in seconds\n")


DEFSH (VTYSH_MPLS,
        undo_bfd_session_delay_up_cmd,
        "no bfd wtr",
        NO_STR
        BFD_STR
        "BFD session WTR(Wait To Restore) time\n")
#endif
/**
 * \page bfd_intf_session_enable_cmd
 * - 功能说明 \n
 *	 接口下使能bfd检测功�? * - 命令格式 \n
 *	bfd enable { session <1-65535> | source-ip A.B.C.D }
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 <1-65535> | bfd会话�? *    A.B.C.D | 发送bfd报文所带的源ip
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 物理接口视图�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令使能某一接口下bfd检测功�? *	 - 前置条件 \n
 *	   �?物理接口下使�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   接口下已绑定bfd会话后不可再次绑�? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config-gigabitethernet1/1/7)#bfd enable 
 *	   Hios(config-gigabitethernet1/1/7)#bfd enable session 1
 *	   Hios(config-gigabitethernet1/1/7)#bfd enable session 1 source-ip 1.1.1.1
 */
DEFSH (VTYSH_MPLS,
        bfd_intf_session_enable_cmd,
        "bfd enable { session <1-65535> | source-ip A.B.C.D }",
        BFD_STR
        "Interface enable BFD session function\n"
        "Specify BFD session\n"
        "BFD session local discriminator\n"
        "Set source IP address of the BFD packet\n"
        "IP Unicast address\n")

/**
 * \page undo_bfd_intf_session_enable_cmd no bfd enable
 * - 功能说明 \n
 *	 接口下去使能bfd检测功�? * - 命令格式 \n
 *	no bfd enable
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	 �? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 物理接口视图�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令去使能某一接口下bfd检测功�? *	 - 前置条件 \n
 *	   �?物理接口下使�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config-gigabitethernet1/1/7)#no bfd enable 
 *
 */
DEFSH (VTYSH_MPLS,
        undo_bfd_intf_session_enable_cmd,
        "no bfd enable",
        NO_STR
        BFD_STR
        "Interface enable BFD session function\n")

/**
 * \page bfd_info_show_cmd
 * - 功能说明 \n
 *	 查看bfd会话的状态及配置
 * - 命令格式 \n
 *	show bfd ( session | statistics | config ) [<1-65535>]
 * - 参数说明 \n
 *	 |参数	|说明 	   |
 *	 |------|--------------|
 *	( session | statistics | config ) | 查询bfd会话的状�?或配�? *	[<1-65535>] | 要查询的具体会话�? *	 �? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 config视图�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   查询bfd的会话状态或配置
 *	 - 前置条件 \n
 *	   config模式�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   命令行使用bfd会话号时显示具体一条会话，不使用时显示全部会话�? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   Hios(config)#show bfd session
 *	   Hios(config)#show bfd session 2
 *	   Hios(config)#show bfd statistics
 *	   Hios(config)#show bfd statistics 2
 *	   Hios(config)#show bfd config
 *	   Hios(config)#show bfd config 2
 *
 */
DEFSH (VTYSH_MPLS,
        bfd_info_show_cmd,
		"show bfd ( session | statistics | config ) [<1-65535>]",
        SHOW_STR
        BFD_STR
        "Display session information\n"
        "Display the statistics information of session\n"
        "Display configuration view information\n"
        "local_id for view information\n")

/**
 * \page no_mpls_vsi_cmd_vtysh no vsi <1-1024>
 * - 功能说明 \n
 *	 删除一个vsi实例
 * - 命令格式 \n
 *	 no vsi <1-1024>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<1-4095>|Vsi实例号|
 *
 * - 缺省情况 \n
 *	 缺省情况下，vsi实例不存�? * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除一个vsi实例
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi <1-1024> \n
 *     show vsi <1-1024> \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, no_mpls_vsi_cmd_vtysh,
        "no vsi <1-1024>",
        NO_STR
        "Specify VSI(Virtual Switching Instance) configuration information\n"
        "Specify VSI ID:<1-1024>\n")

/**
 * \page mpls_vsi_name_cmd_vtysh name NAME
 * - 功能说明 \n
 *	 配置vsi名字
 * - 命令格式 \n
 *	 name NAME
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |NAME|Vsi名字，字符串|
 *
 * - 缺省情况 \n
 *	 缺省情况下，vsi名字未配�? * - 命令模式 \n
 *	 Vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置vsi名字
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   重复配置自动覆盖
 *	 - 相关命令 \n
 *	   vsi <1-1024>
 * - 使用举例 \n
 *	   Huahuan(mpls-vsi)# \n
 *     Huahuan(mpls-vsi)#name vsi-name \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_vsi_name_cmd_vtysh,
        "name NAME",
        "Specify VSI(Virtual Switching Instance) configuration information\n"
        "String<1-31>:Name of VSI\n")


DEFSH (VTYSH_MPLS, mpls_vsi_pw_failback_cmd_vtysh,
        "pw backup failback [wtr <0-3600>]",
        "PW protection switch configuration\n"
        "Backup\n"
        "PW protection failback\n"
        "Working Time Regulations\n"
        "Failback to the time, the default 30s\n")


DEFSH (VTYSH_MPLS, mpls_vsi_pw_non_failback_cmd_vtysh,
        "pw backup non-failback",
        "PW protection switch configuration\n"
        "Backup\n"
        "PW protection non-failback\n")

/**
 * \page vsi_mac_learn_cmd_vtysh mac-learn (enable|disable)
 * - 功能说明 \n
 *	 配置vsi的mac学习使能
 * - 命令格式 \n
 *	 mac-learn (enable|disable)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |enable|使能mac学习|
 *   |disable|去使能mac学习|
 *
 * - 缺省情况 \n
 *	 缺省情况下，mac学习已使�? * - 命令模式 \n
 *	 Vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置vsi的mac学习使能
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi <1-1024>
 * - 使用举例 \n
 *	   Huahuan(mpls-vsi)# \n
 *	   Huahuan(mpls-vsi)#mac-learn disable \n
 *
 */
DEFSH (VTYSH_MPLS, vsi_mac_learn_cmd_vtysh,
        "mac-learn (enable|disable)",
        "VSI MAC learning function\n"
        "Enable VSI MAC learning function\n"
        "Disable VSI MAC learning function\n")

/**
 * \page vsi_mac_learn_limit_cmd_vtysh mac-learn limit <1-65535>
 * - 功能说明 \n
 *	 配置vsi下mac学习限制
 * - 命令格式 \n
 *	 mac-learn limit <1-65535>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<1- 65535>|限制Mac学习的个�?HT201设备最大支�?2767HT2200设备最大支�?5535|
 *
 * - 缺省情况 \n
 *	 缺省情况下，mac学习限制为最大�? * - 命令模式 \n
 *	 Vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置vsi下mac学习个数限制
 *	 - 前置条件 \n
 *	   Mac学习必须先使�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi <1-1024> \n
 *     mac-learn (enable|disable) \n
 *     no mac-learn limit \n
 * - 使用举例 \n
 *	   Huahuan(mpls-vsi)# \n
 *	   Huahuan(mpls-vsi)#mac-learn limit 10 \n
 *
 */
DEFSH (VTYSH_MPLS, vsi_mac_learn_limit_cmd_vtysh,
        "mac-learn limit <1-65535>",
        "VSI MAC learning limit <1-65535> function\n"
        "set VSI MAC learning enable firstly\n"
        "set VSI MAC learning limit <1-65535>\n")

/**
 * \page no_vsi_mac_learn_limit_cmd_vtysh no mac-learn limit
 * - 功能说明 \n
 *	 取消vsi下mac学习个数限制
 * - 命令格式 \n
 *	 no mac-learn limit
 * - 参数说明 \n
 *   �? * - 缺省情况 \n
 *	 缺省情况下，取消mac学习限制未配�? * - 命令模式 \n
 *	 Vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令取消vsi�?mac学习个数的限�? *	 - 前置条件 \n
 *	   Mac学习必须先使�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi <1-1024> \n
 *	   mac-learn (enable|disable) \n
 *	   no mac-learn limit \n
 * - 使用举例 \n
 *	   Huahuan(mpls-vsi)# \n
 *	   Huahuan(mpls-vsi)#no mac-learn limit \n
 *
 */
DEFSH (VTYSH_MPLS, no_vsi_mac_learn_limit_cmd_vtysh,
        "no mac-learn limit",
        "VSI MAC learning unlimit  function\n"
        "if MAC learning enable,set VSI MAC learning max\n"
        "if MAC learning disable,set nothing\n")

/**
 * \page mpls_vsi_encapsulate_ethernet_raw_cmd_vtysh encapsulate ethernet raw
 * - 功能说明 \n
 *	 配置以太接口的封装参�? * - 命令格式 \n
 *	 encapsulate ethernet raw
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置以太接口封装参�? *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no enacpsulate \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, mpls_vsi_encapsulate_ethernet_raw_cmd_vtysh,
        "encapsulate ethernet raw",
        "Specify the encapsulation parameters\n"
        "Ethernet access\n"
        "Without a VLAN tag\n")

/**
 * \page mpls_vsi_encapsulate_ethernet_tag_cmd_vtysh encapsulate ethernet tag tpid (0x8100|0x88a8|0x9100) vlan <0-4094>
 * - 功能说明 \n
 *	 配置以太接口的封装参�? * - 命令格式 \n
 *	 encapsulate ethernet tag tpid (0x8100|0x88a8|0x9100) vlan <0-4094>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |0x8100\|0x88a8\|0x9100|Tpid取值|
 *   |<0-4094>|Vlan范围|
 *
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置以太接口封装参�? *	 - 前置条件 \n
 *	   Vc类型已配置，并且为ethernet
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no enacpsulate \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, mpls_vsi_encapsulate_ethernet_tag_cmd_vtysh,
        "encapsulate ethernet tag tpid (0x8100|0x88a8|0x9100) vlan <0-4094>",
        "Specify the encapsulation parameters\n"
        "Ethernet access\n"
        "Take a VLAN tag\n"
        "Tpid\n"
        "0x8100\n"
        "0x88a8\n"
        "0x9100\n"
        "Specify vlan\n"
        "<0-4094>")

/**
 * \page mpls_vsi_encapsulate_vlan_raw_cmd_vtysh encapsulate vlan raw tpid (0x8100|0x88a8|0x9100) vlan <0-4094>
 * - 功能说明 \n
 *	 配置以太接口的封装参�? * - 命令格式 \n
 *	 encapsulate vlan raw tpid (0x8100|0x88a8|0x9100) vlan <0-4094>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |0x8100\|0x88a8\|0x9100|Tpid取值|
 *	 |<0-4094>|Vlan范围|
 *
 * - 缺省情况 \n
 *	 缺省情况下，为raw
 * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置以太接口封装参�? *	 - 前置条件 \n
 *	   Vc类型已配置，并且为ethernet
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no enacpsulate \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, mpls_vsi_encapsulate_vlan_raw_cmd_vtysh,
        "encapsulate vlan raw tpid (0x8100|0x88a8|0x9100) vlan <0-4094>",
        "Specify the encapsulation parameters\n"
        "Vlan access\n"
        "Without a VLAN tag\n"
        "Tpid\n"
        "0x8100\n"
        "0x88a8\n"
        "0x9100\n"
        "Specify vlan\n"
        "<0-4094>")

/**
 * \page mpls_vsi_encapsulate_vlan_tag_cmd_vtysh encapsulate vlan tag tpid (0x8100|0x88a8|0x9100) vlan <0-4094>
 * - 功能说明 \n
 *	 配置以太接口的封装参�? * - 命令格式 \n
 *	 encapsulate vlan tag tpid (0x8100|0x88a8|0x9100) vlan <0-4094>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |0x8100\|0x88a8\|0x9100|Tpid取值|
 *	 |<0-4094>|Vlan范围|
 *
 * - 缺省情况 \n
 *	 缺省情况下，为raw
 * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置以太接口封装参�? *	 - 前置条件 \n
 *	   Vc类型已配置，并且为ethernet
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no enacpsulate \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, mpls_vsi_encapsulate_vlan_tag_cmd_vtysh,
        "encapsulate vlan tag tpid (0x8100|0x88a8|0x9100) vlan <0-4094>",
        "Specify the encapsulation parameters\n"
        "Vlan access\n"
        "Take a VLAN tag\n"
        "Tpid\n"
        "0x8100\n"
        "0x88a8\n"
        "0x9100\n"
        "Specify vlan\n"
        "<0-4094>")

/**
 * \page no_mpls_vsi_encapsulate_cmd_vtysh no encapsulate
 * - 功能说明 \n
 *	 将pw封装参数恢复默认
 * - 命令格式 \n
 *	 no encapsulate
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，为ethernet+raw模式
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令将pw封装参数恢复默认ethernet+raw
 *	 - 前置条件 \n
 *	   Vsi-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi <1-1024> \n
 *	   encapsulate (ethernet|vlan) (raw|tag) {tpid (0x8100|0x88a8|0x9100) | vlan <0-4094>} \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# no encapsulate \n
 *     Hios(mpls-vsi-1)# \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_vsi_encapsulate_cmd_vtysh,
        "no encapsulate",
        NO_STR
        "Specify the ethernet encapsulation parameters\n")

/**
 * \page mpls_vsi_storm_suppress_cmd_vtysh storm-suppress (unicast|broadcast|multicast) cir <0-10000000> cbs <16-1600>
 * - 功能说明 \n
 *	 配置 vpls的vsi节点下的未知单播、未知多播和广播限�? * - 命令格式 \n
 *	 storm-suppress (unicast|broadcast|multicast) cir <0-10000000> cbs <16-1600>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |unicast|未知单播|
 *   |broadcast|广播|
 *   |multicast|未知多播|
 *   |cir|承诺信息速率|
 *   |cbs|承诺突发速率|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置限�? * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置vpls报文进行抑制(cir=0)或者限�?cir�?)
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi ID \n
 *	   no storm-suppress (unicast|broadcast|muliticast) \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# \n
 *	   Hios(mpls-vsi-1)# storm-suppress unicast cir 100000 cbs 1600\n
 *
 */
DEFSH (VTYSH_MPLS, mpls_vsi_storm_suppress_cmd_vtysh,
		"storm-suppress (unicast | broadcast | multicast) cir <0-10000000> cbs <16-1600>",
		"Specify the storm-suppress parameters\n"
		"Unicast\n"
		"Broadcast\n"
		"Multicast\n"
		"Committed information rate\n"
		"Committed information rate value <0-10000000> (Unit: Kbps)\n"
		"Committed burst size\n"
		"Committed burst size value <16-1600>\n")

/**
 * \page no_mpls_vsi_storm_suppress_cmd_vtysh no storm-suppress (unicast|broadcast|multicast)
 * - 功能说明 \n
 *	 删除vpls的vsi节点下的未知单播、未知多播和广播限�? * - 命令格式 \n
 *	 no storm-suppress (unicast|broadcast|multicast)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |unicast|未知单播|
 *	 |broadcast|广播|
 *	 |multicast|未知多播|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置限�? * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除vpls的vsi节点下报文限�? *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi ID \n
 *	   no storm-suppress (unicast|broadcast|muliticast) \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# \n
 *	   Hios(mpls-vsi-1)# no storm-suppress unicast \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_vsi_storm_suppress_cmd_vtysh,
        "no storm-suppress (unicast | broadcast | multicast)",
        "Specify the storm_suppress parameters\n"
        NO_STR
        "Unicast\n"
        "Broadcast\n"
        "Multicast\n")

/**
 * \page vsi_mac_blacklist_cmd_vtysh mac-blacklist MAC
 * - 功能说明 \n
 *	 配置vpls的vsi节点下黑名单
 * - 命令格式 \n
 *	 mac-blacklist MAC
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |MAC|MAC地址:HH:HH:HH:HH:HH:HH|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置黑名单
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置vpls的vsi节点下黑名单
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi ID \n
 *     show mac-blacklist \n
 *     no mac-blacklist MAC \n
 *     no mac-blacklist all \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# \n
 *	   Hios(mpls-vsi-1)# mac-blacklist 00:00:00:00:00:01 \n
 *
 */
DEFSH (VTYSH_MPLS, vsi_mac_blacklist_cmd_vtysh,
        "mac-blacklist MAC",
        "MAC address blacklist for vsi\n"
        "MAC address, format: HH:HH:HH:HH:HH:HH\n")

/**
 * \page no_vsi_mac_blacklist_cmd_vtysh no mac-blacklist MAC
 * - 功能说明 \n
 *	 删除vpls的vsi节点下指定的黑名�? * - 命令格式 \n
 *	 no mac-blacklist MAC
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |MAC|MAC地址:HH:HH:HH:HH:HH:HH|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置黑名单
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除vpls的vsi节点下指定的黑名�? *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi ID \n
 *	   show mac-blacklist \n
 *	   mac-blacklist MAC \n
 *	   mac-blacklist all \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# \n
 *	   Hios(mpls-vsi-1)# no mac-blacklist 00:00:00:00:00:01 \n
 *
 */
DEFSH (VTYSH_MPLS, no_vsi_mac_blacklist_cmd_vtysh,
        "no mac-blacklist MAC",
        NO_STR
        "MAC address blacklist for vsi\n"
        "MAC address, format: HH:HH:HH:HH:HH:HH\n")

/**
 * \page no_vsi_mac_blacklist_all_cmd_vtysh no mac-blacklist all
 * - 功能说明 \n
 *	 删除vpls的vsi节点下所有黑名单
 * - 命令格式 \n
 *	 no mac-blacklist all
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |MAC|MAC地址:HH:HH:HH:HH:HH:HH|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置黑名单
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除vpls的vsi节点下所有黑名单
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi ID \n
 *	   show mac-blacklist \n
 *	   no mac-blacklist HH:HH:HH:HH:HH:HH \n
 *	   mac-blacklist HH:HH:HH:HH:HH:HH \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# \n
 *	   Hios(mpls-vsi-1)# no mac-blacklist all \n
 *
 */
DEFSH (VTYSH_MPLS, no_vsi_mac_blacklist_all_cmd_vtysh,
        "no mac-blacklist all",
        NO_STR
        "MAC address blacklist for vsi\n"
        "All the MAC address of the current VSI\n")

/**
 * \page show_vsi_mac_blacklist_cmd_vtysh show mac-blacklist
 * - 功能说明 \n
 *	 显示vpls的vsi节点下配置的所有黑名单
 * - 命令格式 \n
 *	 show mac-blacklist
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，未配置黑名单
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示vpls的vsi节点下配置的所有黑名单
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi ID \n
 *	   show mac-blacklist \n
 *	   no mac-blacklist MAC \n
 *	   mac-blacklist MAC \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# \n
 *	   Hios(mpls-vsi-1)# show mac-blacklist \n
 *
 */
DEFSH (VTYSH_MPLS, show_vsi_mac_blacklist_cmd_vtysh,
        "show mac-blacklist",
        SHOW_STR
        "MAC address blacklist for vsi\n")

/**
 * \page vsi_mac_whitelist_cmd_vtysh mac-whitelist MAC
 * - 功能说明 \n
 *	 配置vpls的vsi节点下的白名�? * - 命令格式 \n
 *	 mac-whitelist MAC
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |---|--------------|
 *   |MAC|MAC地址HH:HH:HH:HH:HH:HH|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置白名单
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置vpls的vsi节点下白名单
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi ID \n
 *	   show mac-whitelist \n
 *	   no mac-whitelist MAC \n
 *	   mac-whitelist MAC \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# \n
 *	   Hios(mpls-vsi-1)# mac-whitelist 00:00:00:00:00:11 \n
 *
 */
DEFSH (VTYSH_MPLS, vsi_mac_whitelist_cmd_vtysh,
        "mac-whitelist MAC",
        "MAC address whitelist for vsi\n"
        "MAC address, format: HH:HH:HH:HH:HH:HH\n")

/**
 * \page no_vsi_mac_whitelist_cmd_vtysh no mac-whitelist MAC
 * - 功能说明 \n
 *	 删除vpls的vsi节点下的白名�? * - 命令格式 \n
 *	 no mac-whitelist MAC
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |MAC|MAC地址HH:HH:HH:HH:HH:HH|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配置白名单
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除vpls的vsi节点下白名单
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi ID \n
 *	   show mac-whitelist \n
 *	   mac-whitelist MAC \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# \n
 *	   Hios(mpls-vsi-1)# no mac-whitelist \n
 *
 */
DEFSH (VTYSH_MPLS, no_vsi_mac_whitelist_cmd_vtysh,
        "no mac-whitelist MAC",
        NO_STR
        "MAC address whitelist for vsi\n"
        "MAC address, format: HH:HH:HH:HH:HH:HH\n")

/**
 * \page no_vsi_mac_whitelist_all_cmd_vtysh no mac-whitelist all
 * - 功能说明 \n
 *	 删除vpls的vsi节点下配置的所有白名单
 * - 命令格式 \n
 *	 no mac-whitelist all
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，未配置白名单
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除vpls的vsi节点下配置的所有白名单
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi ID \n
 *     mac-whitelist MAC \n
 *     no mac-whitelist MAC \n
 *     show mac-whitelist \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# \n
 *	   Hios(mpls-vsi-1)# no mac-whitelist all \n
 *
 */
DEFSH (VTYSH_MPLS, no_vsi_mac_whitelist_all_cmd_vtysh,
        "no mac-whitelist all",
        NO_STR
        "MAC address whitelist for vsi\n"
        "All the MAC address of the current VSI\n")

/**
 * \page show_vsi_mac_whitelist_cmd_vtysh show mac-whitelist
 * - 功能说明 \n
 *	 显示vpls的vsi节点下配置的所有白名单
 * - 命令格式 \n
 *	 show mac-whitelist
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，未配置白名单
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示vpls的vsi节点下配置的所有白名单
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi ID \n
 *     mac-whitelist MAC \n
 *     no mac-whitelist MAC \n
 *     no mac-whitelist all \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# \n
 *	   Hios(mpls-vsi-1)# show mac-whitelist \n
 *
 */
DEFSH (VTYSH_MPLS, show_vsi_mac_whitelist_cmd_vtysh,
        "show mac-whitelist",
        SHOW_STR
        "MAC address whitelist for vsi\n")

/**
 * \page show_vsi_cmd_vtysh show vsi <1-1024>
 * - 功能说明 \n
 *	 显示指定vsi实例信息
 * - 命令格式 \n
 *	 show vsi <1-1024>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<1-1024>|Vsi实例号|
 *
 * - 缺省情况 \n
 *	 缺省情况下，vsi未创�? * - 命令模式 \n
 *	 Config视图、mpls视图、vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示vsi信息
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi ID \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, show_vsi_cmd_vtysh,
        "show vsi [<1-1024>]",
        "Display information\n"
        "Specify VSI(Virtual Switching Instance) configuration information\n"
        "Range of vsi-id:<1-1024>\n")

/**
 * \page mpls_if_bind_vsi_cmd_vtysh mpls vsi <1-1024>
 * - 功能说明 \n
 *	 配置接口绑定vsi
 * - 命令格式 \n
 *	 mpls vsi <1-1024>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |<1-1024>|Vsi实例号|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口未绑定vsi
 * - 命令模式 \n
 *	 2层以太口及子接口、trunk接口
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置接口绑定vsi
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   同一接口只能绑定一个vsi，多个接口可绑定同一vsi
 *	 - 相关命令 \n
 *	   vsi ID \n
 *	   no mpls vsi <1-1024> \n
 * - 使用举例 \n
 *	   Huahuan(config-ethernet1/1/1)#mpls vsi 1 \n
 *     Huahuan(config-ethernet1/1/1)# \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_if_bind_vsi_cmd_vtysh,
        "mpls vsi <1-1024>",
        "MPLS(Multiprotocol Label Switching) configuration information\n"
        "Specify VSI(Virtual Switching Instance) configuration information\n"
        "<1-1024>:The range of VSI\n")

/**
 * \page mpls_if_unbind_vsi_cmd_vtysh no mpls vsi <1-1024>
 * - 功能说明 \n
 *	 配置接口解绑vsi
 * - 命令格式 \n
 *	 no mpls vsi <1-1024>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<1-1024>|Vsi实例号|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口未绑定vsi
 * - 命令模式 \n
 *	 2层以太口及子接口、trunk接口
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置接口解绑vsi
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi ID \n
 *	   mpls vsi <1-1024> \n
 * - 使用举例 \n
 *	   Huahuan(config-ethernet1/1/1)#no mpls vsi 1 \n
 *	   Huahuan(config-ethernet1/1/1)# \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_if_unbind_vsi_cmd_vtysh,
        "no mpls vsi <1-1024>",
        NO_STR
        "MPLS(Multiprotocol Label Switching) configuration information\n"
        "Specify VSI(Virtual Switching Instance) configuration information\n"
        "<1-1024>:The range of VSI\n")

/**
 * \page tunnel_protocol_cmd_vtysh protocol (mpls-tp|gre|mpls-te)
 * - 功能说明 \n
 *	 配置tunnel的协议类�? * - 命令格式 \n
 *	 protocol (mpls-tp|gre|mpls-te)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |mpls-tp\|gre\|mpls-te|分为mpls-tp、gre以及mpls-te三种类型|
 *
 * - 缺省情况 \n
 *	 缺省情况下，协议未配�? * - 命令模式 \n
 *	 TUNNEL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置tunnel协议类型
 *	 - 前置条件 \n
 *	   tunnel接口已存�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   no protocol \n
 *     show tunnel USP \n
 * - 使用举例 \n
 *	   进入tunnel视图并配置协议：\n
 *     Huahuan(config)# interface tunnel 1/1/1 \n
 *     Huahuan(config-tunnel1/1/1)#protocol mpls-tp \n
 *
 */
DEFSH (VTYSH_MPLS, tunnel_protocol_cmd_vtysh,
        "protocol (mpls-tp|gre|mpls-te)",
        "Specify the interface encapsulation protocol type\n"
        "Encapsulation protocol type mpls-tp\n"
        "Encapsulation protocol type gre\n"
        "Encapsulation protocol type mpls-te\n")

/**
 * \page no_tunnel_protocol_cmd_vtysh no protocol
 * - 功能说明 \n
 *	 删除tunnel的协议类�? * - 命令格式 \n
 *	 no protocol
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 TUNNEL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除tunnel协议类型
 *	 - 前置条件 \n
 *	   tunnel接口已存�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   protocol (mpls-tp|gre|mpls-te) \n
 *	   show tunnel USP \n
 * - 使用举例 \n
 *	   进入tunnel视图并配置协议：\n
 *	   Huahuan(config)# interface tunnel 1/1/1 \n
 *	   Huahuan(config-tunnel1/1/1)#protocol mpls-tp \n
 *	   Huahuan(config-tunnel1/1/1)#no protocol \n
 *
 */
DEFSH (VTYSH_MPLS | VTYSH_IFM, no_tunnel_protocol_cmd_vtysh,
        "no protocol",
        NO_STR
        "Specify the interface encapsulation protocol type\n")

/**
 * \page tunnel_static_lsp_cmd_vtysh mpls static-lsp ingress NAME {egress NAME|backup}
 * - 功能说明 \n
 *	 在tunnel上添加静态lsp，使用no命令删除该LSP配置
 * - 命令格式 \n
 *	 mpls static-lsp ingress NAME {egress NAME|backup}
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |LSP-NAME|Ingress方向静态lsp名字，小�?2个字符的字符串|
 *   |LSP-NAME|可选项（双向lsp）Egress 方向静态lsp名字，小�?2个字符的字符串|
 *   |{backup}|可选项，未配置表示主，配置表示备|
 *
 * - 缺省情况 \n
 *	 缺省情况下，tunnel上未添加lsp节点
 * - 命令模式 \n
 *	 TUNNEL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令在tunnel上添加静态lsp
 *	 - 前置条件 \n
 *	   tunnel接口已存在，并配置协议类�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   no mpls static-lsp [backup] \n
 *	   show tunnel USP \n
 * - 使用举例 \n
 *	   进入tunnel视图并配置协议：\n
 *	   Huahuan(config)# interface tunnel 1/1/1 \n
 *	   Huahuan(config-tunnel1/1/1)#protocol mpls-tp \n
 *	   Huahuan(config-tunnel1/1/1)#source 1.1.1.1 destination 3.3.3.3 \n
 *	   Huahuan(config-tunnel1/1/1)#mpls static-lsp ingress lsp1 egress lsp2 backup \n
 *
 */
DEFSH (VTYSH_MPLS, tunnel_static_lsp_cmd_vtysh,
        "mpls static-lsp ingress NAME {egress NAME | backup}",
        "Specified under static LSP tunnel interface\n"
        "LSP type\n"
        "LSP direction ingress\n"
        "Specifies the static LSP name--max.31\n"
        "LSP direction egress\n"
        "Specifies the static LSP name--max.31\n"
        "Specifies the static LSP for backup\n")

/**
 * \page no_tunnel_static_lsp_cmd_vtysh no mpls static-lsp [backup]
 * - 功能说明 \n
 *	 使用no命令删除tunnel上添加的LSP配置
 * - 命令格式 \n
 *	 no mpls static-lsp [backup]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |backup|可选项，若指定删除备lsp|
 *
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 TUNNEL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除tunnel上的lsp
 *	 - 前置条件 \n
 *	   tunnel接口已配置lsp
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   mpls static-lsp ingress LSP-NAME {egress LSP-NAME|backup} \n
 *	   show tunnel USP \n
 * - 使用举例 \n
 *	   删除tunnel上的lsp：\n
 *	   Huahuan(config)# interface tunnel 1/1/1 \n
 *	   Huahuan(config-tunnel1/1/1)#no mpls static-lsp backup \n
 *
 */
DEFSH (VTYSH_MPLS, no_tunnel_static_lsp_cmd_vtysh,
        "no mpls static-lsp [backup]",
        NO_STR
        "Delete the specified static LSP\n"
        "Lsp type\n"
        "Specify the static LSP for backup\n")

/**
 * \page show_tunnel_cmd_vtysh show tunnel [USP]
 * - 功能说明 \n
 *	 显示tunnel的基本信�? * - 命令格式 \n
 *	 show tunnel [USP]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|接口号|
 *
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 TUNNEL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令查看tunnel的配置信息和状�? *	 - 前置条件 \n
 *	   tunnel存在
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   mpls static-lsp ingress LSP-NAME {egress LSP-NAME|backup} \n
 *	   protocol (mpls-tp|gre|mpls-te) \n
 *	   source A.B.C.D destination A.B.C.D \n
 *	   no mpls static-lsp [backup] \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, show_tunnel_cmd_vtysh,
        "show tunnel [USP]",
        "Show configure infomation\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR)

/**
 * \page tunnel_source_destination_ip_cmd_vtysh source A.B.C.D destination A.B.C.D
 * - 功能说明 \n
 *	 配置tunnel的源IP、目的IP
 * - 命令格式 \n
 *	 source A.B.C.D destination A.B.C.D
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |A.B.C.D |IP地址|
 *   |A.B.C.D |IP地址|
 *
 * - 缺省情况 \n
 *	 缺省情况下，tunnel源IP、目的IP未配�? * - 命令模式 \n
 *	 TUNNEL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置tunnel源IP、目的IP
 *	 - 前置条件 \n
 *	   tunnel接口已存在，并配置协议类�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   no source destination \n
 *	   show tunnel USP \n
 * - 使用举例 \n
 *	   Huahuan(config)# interface tunnel 1/1/1 \n
 *     Huahuan(config-tunnel1/1/1)#protocol mpls-tp \n
 *     Huahuan(config-tunnel1/1/1)#source 192.168.1.1  destination 3.3.3.3 \n
 *
 */
DEFSH (VTYSH_MPLS, tunnel_source_destination_ip_cmd_vtysh,
        "source A.B.C.D destination A.B.C.D",
        "Source address configuration interface\n"
        "IP address format A.B.C.D\n"
        "Destination address configuration interface\n"
        "IP address format A.B.C.D\n")

/**
 * \page no_tunnel_source_destination_ip_cmd_vtysh no source destination
 * - 功能说明 \n
 *	 删除tunnel的源IP、目的IP
 * - 命令格式 \n
 *	 no source destination
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，tunnel源IP、目的IP未配�? * - 命令模式 \n
 *	 TUNNEL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除tunnel源IP、目的IP
 *	 - 前置条件 \n
 *	   tunnel接口已存在，并配置协议类�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   source A.B.C.D destination A.B.C.D \n
 *	   show tunnel USP \n
 * - 使用举例 \n
 *	   删除源IP�? *     Huahuan(config)# interface tunnel 1/1/1 \n
 *     Huahuan(config-tunnel1/1/1)# protocol mpls-tp \n
 *     Huahuan(config-tunnel1/1/1)# no source destination \n
 *
 */
DEFSH (VTYSH_MPLS, no_tunnel_source_destination_ip_cmd_vtysh,
        "no source destination",
        NO_STR
        "Source address configuration interface\n"
        "Destination address configuration interface\n")


DEFSH (VTYSH_MPLS, tunnel_nexthop_cmd_vtysh,
        "nexthop A.B.C.D",
        "Nexthop for tunnel\n"
        "A.B.C.D\n")


DEFSH (VTYSH_MPLS, no_tunnel_nexthop_cmd_vtysh,
        "no nexthop",
        NO_STR
        "Nexthop for tunnel\n")

/**
 * \page tunnel_outif_ethernet_dmac_vtysh outif ethernet USP dmac XX:XX:XX:XX:XX:XX
 * - 功能说明 \n
 *	 配置静态gre tunnel的实际出接口
 * - 命令格式 \n
 *	 outif ethernet USP dmac XX:XX:XX:XX:XX:XX
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|百兆以太出接口|
 *   |XX:XX:XX:XX:XX:XX|目的mac|
 *
 * - 缺省情况 \n
 *	 缺省情况下，出接口目的mac未配�? * - 命令模式 \n
 *	 TUNNEL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置gre tunnel的实际出接口
 *	 - 前置条件 \n
 *	   tunnel接口已存在，并配置协议类型为GRE
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   protocol (mpls-tp|gre|mpls-te) \n
 *	   no outif \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, tunnel_outif_ethernet_dmac_vtysh,
        "outif ethernet USP dmac XX:XX:XX:XX:XX:XX",
        "Out-going interface for tunnel\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        "Destination MAC\n"
        "Mac address\n")

/**
 * \page tunnel_outif_gigabit_ethernet_dmac_vtysh outif gigabitethernet USP dmac XX:XX:XX:XX:XX:XX
 * - 功能说明 \n
 *	 配置静态gre tunnel的实际出接口
 * - 命令格式 \n
 *	 outif gigabitethernet USP dmac XX:XX:XX:XX:XX:XX
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|千兆以太出接口|
 *	 |XX:XX:XX:XX:XX:XX|目的mac|
 *
 * - 缺省情况 \n
 *	 缺省情况下，出接口目的mac未配�? * - 命令模式 \n
 *	 TUNNEL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置gre tunnel的实际出接口
 *	 - 前置条件 \n
 *	   tunnel接口已存在，并配置协议类型为GRE
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   protocol (mpls-tp|gre|mpls-te) \n
 *	   no outif \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, tunnel_outif_gigabit_ethernet_dmac_vtysh,
        "outif gigabitethernet USP dmac XX:XX:XX:XX:XX:XX",
        "Out-going interface for tunnel\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
        "Destination MAC\n"
        "Mac address\n")

/**
 * \page tunnel_outif_xgigabit_ethernet_dmac_vtysh outif xgigabitethernet USP dmac XX:XX:XX:XX:XX:XX
 * - 功能说明 \n
 *	 配置静态gre tunnel的实际出接口
 * - 命令格式 \n
 *	 outif xgigabitethernet USP dmac XX:XX:XX:XX:XX:XX
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|千兆以太出接口|
 *	 |XX:XX:XX:XX:XX:XX|目的mac|
 *
 * - 缺省情况 \n
 *	 缺省情况下，出接口目的mac未配�? * - 命令模式 \n
 *	 TUNNEL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置gre tunnel的实际出接口
 *	 - 前置条件 \n
 *	   tunnel接口已存在，并配置协议类型为GRE
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   protocol (mpls-tp|gre|mpls-te) \n
 *	   no outif \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, tunnel_outif_xgigabit_ethernet_dmac_vtysh,
        "outif xgigabitethernet USP dmac XX:XX:XX:XX:XX:XX",
        "Out-going interface for tunnel\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
        "Destination MAC\n"
        "Mac address\n")

/**
 * \page tunnel_outif_trunk_dmac_vtysh outif trunk TRUNK dmac XX:XX:XX:XX:XX:XX
 * - 功能说明 \n
 *	 配置静态gre tunnel的实际出接口
 * - 命令格式 \n
 *	 outif trunk TRUNK dmac XX:XX:XX:XX:XX:XX
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |TRUNK|Trunk出接口|
 *	 |XX:XX:XX:XX:XX:XX|目的mac|
 *
 * - 缺省情况 \n
 *	 缺省情况下，出接口目的mac未配�? * - 命令模式 \n
 *	 TUNNEL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置gre tunnel的实际出接口
 *	 - 前置条件 \n
 *	   tunnel接口已存在，并配置协议类型为GRE
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   protocol (mpls-tp|gre|mpls-te) \n
 *	   no outif \n
 * - 使用举例 \n
 *	   Huahuan(config)# interface tunnel 1/1/1 \n
 *     Huahuan(config-tunnel1/1/1)#protocol gre \n
 *     Huahuan(config-tunnel1/1/1)#outif ethernet 1/1/5 dmac 00:00:00:00:00:01 \n
 *
 */
DEFSH (VTYSH_MPLS, tunnel_outif_trunk_dmac_vtysh,
        "outif trunk TRUNK dmac XX:XX:XX:XX:XX:XX",
        "Out-going interface for tunnel\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR
        "Destination MAC\n"
        "Mac address\n")

/**
 * \page no_tunnel_outif_dmac_vtysh no outif
 * - 功能说明 \n
 *	 删除tunnel接口的实际出接口
 * - 命令格式 \n
 *	 no outif
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，出接口目的mac未配�? * - 命令模式 \n
 *	 TUNNEL模式
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除tunnel接口的实际出接口
 *	 - 前置条件 \n
 *	   tunnel接口已存在，并配置协议类型为GRE
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   outif {Ethernet USP | trunk TRUNK} dmac XX:XX:XX:XX:XX:XX \n
 *	   show tunnel USP \n
 * - 使用举例 \n
 *	   Huahuan(config)# interface tunnel 1/1/1 \n
 *	   Huahuan(config-tunnel1/1/1)# protocol gre \n
 *	   Huahuan(config-tunnel1/1/1)# outif ethernet 1/1/5 dmac 00:00:00:00:00:01 \n
 *     Huahuan(config-tunnel1/1/1)# no outif
 *
 */
DEFSH (VTYSH_MPLS, no_tunnel_outif_dmac_vtysh,
        "no outif",
        NO_STR
        "Out-going interface for tunnel\n")

/**
 * \page tunnel_failback_cmd_vtysh backup failback [wtr <0-3600>]
 * - 功能说明 \n
 *	 使用该命令配置tunnel上主备lsp的延时时�? * - 命令格式 \n
 *	 backup failback [wtr <0-3600>]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<0-3600>|回切延时时间(默认30s)|
 *
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 tunnel视图�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置tunnel上主备lsp的切换模式及延时时间
 *	 - 前置条件 \n
 *	   tunnel接口已配置主备lsp
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show tunnel USP \n
 *	   mpls static-lsp ingress LSP-NAME {egress LSP-NAME | backup} \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, tunnel_failback_cmd_vtysh,
        "backup failback [wtr <0-3600>]",
        "Tunnel protection switch configuration\n"
        "LSP protection failback\n"
        "Working Time Regulations\n"
        "Failback to the time, the default 30s\n")

/**
 * \page tunnel_non_failback_cmd_vtysh backup non-failback
 * - 功能说明 \n
 *	 使用该命令配置tunnel上主备lsp不切�? * - 命令格式 \n
 *	 backup non-failback
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 tunnel视图�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置tunnel上主备lsp不切�? *	 - 前置条件 \n
 *	   tunnel接口已配置主备lsp
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show tunnel USP \n
 *	   mpls static-lsp ingress LSP-NAME {egress LSP-NAME | backup} \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, tunnel_non_failback_cmd_vtysh,
        "backup non-failback",
        "Tunnel protection switch configuration\n"
        "LSP protection not failback\n")

/**
 * \page mpls_pw_name_cmd pw NAME
 * - 功能说明 \n
 *	 Mpls视图下创建或进入pw视图
 * - 命令格式 \n
 *	 pw NAME
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |NAME|Pw的名字|
 *
 * - 缺省情况 \n
 *	 缺省情况下，pw 未创�? * - 命令模式 \n
 *	 Mpls视图�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置一条pw
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   no pw NAME \n
 *     show mpls pw [NAME] \n
 *     mpls l2vpn name NAME \n
 * - 使用举例 \n
 *	   Huahuan(config-mpls)# pw name1 \n
 *	   Huahuan(mpls-pw)#
 *
 */

/* pw 新增命令�?*/
DEFUNSH (VTYSH_MPLS,
        mpls_pw_name,
        mpls_pw_name_cmd,
        "pw NAME",
        "Enter the pw view\n"
        "The name for pw -- max.31\n")
{
    char *pprompt = NULL;

    pprompt = vty->change_prompt;
    if (NULL != pprompt)
    {
        snprintf(pprompt, VTY_BUFSIZ, "%%s(mpls-pw-%s)# ", argv[0]);
    }

    vty->node = PW_NODE;
    return CMD_SUCCESS;
}

/**
 * \page no_mpls_pw_name_cmd_vtysh no pw NAME
 * - 功能说明 \n
 *	 配置接口的IP地址，使用no格式删除该IP配置
 * - 命令格式 \n
 *	 no pw NAME
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |NAME|Pw的名字|
 *
 * - 缺省情况 \n
 *	 缺省情况下，pw 未创�? * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除一�?pw
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   show mpls pw [NAME] \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# no pw name1 \n
 *	   Huahuan(config-mpls)#
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_pw_name_cmd_vtysh,
        "no pw NAME",
        NO_STR
        "Pw view\n"
        "The name for pw -- max.31\n")

/**
 * \page mpls_pw_protocol_cmd_vtysh protocol (svc|martini)
 * - 功能说明 \n
 *	 配置 pw 协议类型
 * - 命令格式 \n
 *	 protocol (svc|martini )
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |svc|静�?pw|
 *   |martini|动�?pw|
 *
 * - 缺省情况 \n
 *	 缺省情况下，协议类型未配�? * - 命令模式 \n
 *	 Pw 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置pw 的协议类�? *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no protocol \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# protocol svc \n
 *	   Huahuan(mpls-pw)#
 *
 */
DEFSH (VTYSH_MPLS, mpls_pw_protocol_cmd_vtysh,
        "protocol (svc | martini)",
        "Specify the pw protocol type\n"
        "Svc: static pw\n"
        "Martini: ldp pw\n")

/**
 * \page no_mpls_pw_protocol_cmd_vtysh no protocol
 * - 功能说明 \n
 *	 删除 pw 协议类型
 * - 命令格式 \n
 *	 no protocol
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，协议类型未配�? * - 命令模式 \n
 *	 Pw 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删�?pw 协议类型
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   protocol (svc|martini) \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)#protocol svc \n
 *	   Huahuan(mpls-pw)#no protocol \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_pw_protocol_cmd_vtysh,
        "no protocol",
        NO_STR
        "The pw protocol type\n")

/**
 * \page mpls_pw_vc_type_cmd_vtysh vc-type (ethernet|tdm)
 * - 功能说明 \n
 *	 配置 pw �?vc 类型
 * - 命令格式 \n
 *	 vc-type (ethernet|tdm)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |ethernet|以太类型|
 *   |tdm|Tdm类型|
 *
 * - 缺省情况 \n
 *	 缺省情况下，vc类型未配�? * - 命令模式 \n
 *	 Pw 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置vc类型
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no vc-type \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# vc-type ethernet \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_pw_vc_type_cmd_vtysh,
        "vc-type (ethernet | tdm)",
        "Specify the vc type\n"
        "Ethernet type\n"
        "Tdm type\n")

/**
 * \page no_mpls_pw_vc_type_cmd_vtysh no vc-type
 * - 功能说明 \n
 *	 删除 vc 类型
 * - 命令格式 \n
 *	 no vc-type
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，vc类型未配�? * - 命令模式 \n
 *	 Pw 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除vc类型
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   vc-type (ethernet|tdm) \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# no vc-type \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_pw_vc_type_cmd_vtysh,
        "no vc-type",
        NO_STR
        "The vc type\n")

/**
 * \page mpls_pw_peer_vcid_cmd_vtysh peer A.B.C.D vc-id <1-4294967295>
 * - 功能说明 \n
 *	 配置pw�?vc-id �?对端 IP
 * - 命令格式 \n
 *	 peer A.B.C.D vc-id <1-4294967295>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |A.B.C.D |IP地址，点分十进制，如10.0.0.1|
 *   |<1-4294967295>|Vc-id，无符号整形范围|
 *
 * - 缺省情况 \n
 *	 缺省情况下，对端IP �?vc-id未配�? * - 命令模式 \n
 *	 Pw 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置vc-id和对端IP
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no peer vcid \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)#peer 3.3.3.3 vc-id 100 \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_pw_peer_vcid_cmd_vtysh,
        "peer A.B.C.D vc-id <1-4294967295>",
        "The end equipment ip address\n"
        "Ip address, format: A.B.C.D\n"
        "Vc-id\n"
        "Vc-id number\n")

/**
 * \page no_mpls_pw_peer_vcid_cmd_vtysh no peer vc-id
 * - 功能说明 \n
 *	 删除 vc-id和对端IP
 * - 命令格式 \n
 *	 no peer vc-id
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，对端IP �?vc-id未配�? * - 命令模式 \n
 *	 Pw 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除对端IP和vc-id
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   peer A.B.C.D vc-id NUM \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# no peer vc-id \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_pw_peer_vcid_cmd_vtysh,
        "no peer vc-id",
        NO_STR
        "The end equipment ip address\n"
        "Vc-id\n")

/**
 * \page mpls_pw_label_cmd_vtysh label in <16-1048575> out <16-1048575>
 * - 功能说明 \n
 *	 配置静�?pw 入出标签
 * - 命令格式 \n
 *	 label in <16-1048575> out <16-1048575>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |<16-1015807>|入标签范围|
 *   |<16-1015807>|出标签范围|
 *
 * - 缺省情况 \n
 *	 缺省情况下，入出标签未配�? * - 命令模式 \n
 *	 Pw 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置静态pw的入出标�? *	 - 前置条件 \n
 *	   协议类型配置，并且为svc
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no label \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)#label in 111 out 222 \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_pw_label_cmd_vtysh,
        "label in <16-1048575> out <16-1048575>",
        "Specify the I/O label\n"
        "In label\n"
        "<16-1048575>\n"
        "Out label\n"
        "<16-1048575>\n")

/**
 * \page no_mpls_pw_label_cmd_vtysh no label
 * - 功能说明 \n
 *	 删除静�?pw 入出标签
 * - 命令格式 \n
 *	 no label
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，入出标签未配�? * - 命令模式 \n
 *	 Pw 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除静态pw的入出标�? *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   label in <16-1015807> out <16-1015807> \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# no label \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_pw_label_cmd_vtysh,
        "no label",
        NO_STR
        "The label for PW\n")

/**
 * \page mpls_pw_mtu_cmd_vtysh mtu <46-9600>
 * - 功能说明 \n
 *	 配置 Pw �?mtu
 * - 命令格式 \n
 *	 mtu <46-9600>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |<46-9600>|Pw mtu范围，默�?500|
 *
 * - 缺省情况 \n
 *	 缺省情况下，mtu�?500
 * - 命令模式 \n
 *	 Pw 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置pw的mtu
 *	 - 前置条件 \n
 *	   Vc类型已配置，并且为ethernet
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   vc-type (ethernet|tdm) \n
 *     no mtu \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# mtu 2000 \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_pw_mtu_cmd_vtysh,
        "mtu <46-12288>",
        "Specify the mtu\n"
        "<46-9600>\n")

/**
 * \page no_mpls_pw_mtu_cmd_vtysh no mtu
 * - 功能说明 \n
 *	 Pw的mtu恢复默认
 * - 命令格式 \n
 *	 no mtu
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，mtu�?500
 * - 命令模式 \n
 *	 Pw 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令将pw �?mtu恢复默认
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   vc-type (ethernet|tdm) \n
 *	   mtu <46-9600> \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# no mtu \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_pw_mtu_cmd_vtysh,
        "no mtu",
        NO_STR
        "Specify the mtu\n")

/**
 * \page mpls_pw_ctrl_word_cmd_vtysh ctrl-word (enable|disable)
 * - 功能说明 \n
 *	 配置 pw 控制字使�? * - 命令格式 \n
 *	 ctrl-word (enable|disable)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |enable|使能控制字|
 *   |disable|去使能控制字|
 *
 * - 缺省情况 \n
 *	 缺省情况下，控制字未使能
 * - 命令模式 \n
 *	 Pw 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置pw控制字使�? *	 - 前置条件 \n
 *	   Vc类型已配置，并且为ethernet
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# ctrl-word enable \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_pw_ctrl_word_cmd_vtysh,
        "ctrl-word (enable | disable)",
        "The pw control word\n"
        "Enable\n"
        "Disable\n")

/**
 * \page mpls_pw_encapsulate_ethernet_raw_cmd_vtysh encapsulate ethernet raw
 * - 功能说明 \n
 *	 封装ethernet类型不携带tag的pw报文
 * - 命令格式 \n
 *	 encapsulate ethernet raw
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，为ethernet类型不携带tag的pw报文
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置pw封装参数
 *	 - 前置条件 \n
 *	   Vsi-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi <1-1024> \n
 *     no enacpsulate \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# encapsulate ethernet tag tpid 0x8100 vlan 100 \n
 *	   Hios(mpls-vsi-1)# \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_pw_encapsulate_ethernet_raw_cmd_vtysh,
        "encapsulate ethernet raw",
        "Specify the encapsulation parameters\n"
        "Ethernet access\n"
        "Without a VLAN tag\n")

/**
 * \page mpls_pw_encapsulate_ethernet_tag_cmd_vtysh encapsulate ethernet tag tpid (0x8100|0x88a8|0x9100) vlan <0-4094>
 * - 功能说明 \n
 *	 封装ethernet类型携带tag的pw报文
 * - 命令格式 \n
 *	 encapsulate ethernet tag tpid (0x8100|0x88a8|0x9100) vlan <0-4094>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |(0x8100\|0x88a8\|0x9100)|Tpid取值|
 *   |<0-4094>|Vlan范围|
 *
 * - 缺省情况 \n
 *	 缺省情况下，为ethernet类型不携带tag的pw报文
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置pw封装参数
 *	 - 前置条件 \n
 *	   Vsi-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi <1-1024> \n
 *	   no enacpsulate \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, mpls_pw_encapsulate_ethernet_tag_cmd_vtysh,
        "encapsulate ethernet tag tpid (0x8100|0x88a8|0x9100) vlan <0-4094>",
        "Specify the encapsulation parameters\n"
        "Ethernet access\n"
        "Take a VLAN tag\n"
        "Tpid\n"
        "0x8100\n"
        "0x88a8\n"
        "0x9100\n"
        "Specify vlan\n"
        "<0-4094>")

/**
 * \page mpls_pw_encapsulate_vlan_raw_cmd_vtysh encapsulate vlan raw tpid (0x8100|0x88a8|0x9100) vlan <0-4094>
 * - 功能说明 \n
 *	 封装vlan类型不携带tag的pw报文
 * - 命令格式 \n
 *	 encapsulate vlan raw tpid (0x8100|0x88a8|0x9100) vlan <0-4094>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |(0x8100\|0x88a8\|0x9100)|Tpid取值|
 *	 |<0-4094>|Vlan范围|
 *
 * - 缺省情况 \n
 *	 缺省情况下，为ethernet类型不携带tag的pw报文
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置pw封装参数
 *	 - 前置条件 \n
 *	   Vsi-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi <1-1024> \n
 *	   no enacpsulate \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, mpls_pw_encapsulate_vlan_raw_cmd_vtysh,
        "encapsulate vlan raw tpid (0x8100|0x88a8|0x9100) vlan <0-4094>",
        "Specify the encapsulation parameters\n"
        "Vlan access\n"
        "Without a VLAN tag\n"
        "Tpid\n"
        "0x8100\n"
        "0x88a8\n"
        "0x9100\n"
        "Specify vlan\n"
        "<0-4094>")

/**
 * \page mpls_pw_encapsulate_vlan_tag_cmd_vtysh encapsulate vlan tag tpid (0x8100|0x88a8|0x9100) vlan <0-4094>
 * - 功能说明 \n
 *	 封装vlan类型携带tag的pw报文
 * - 命令格式 \n
 *	 encapsulate vlan tag tpid (0x8100|0x88a8|0x9100) vlan <0-4094>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |(0x8100\|0x88a8\|0x9100)|Tpid取值|
 *	 |<0-4094>|Vlan范围|
 *
 * - 缺省情况 \n
 *	 缺省情况下，为ethernet类型不携带tag的pw报文
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置pw封装参数
 *	 - 前置条件 \n
 *	   Vsi-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi <1-1024> \n
 *	   no enacpsulate \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, mpls_pw_encapsulate_vlan_tag_cmd_vtysh,
        "encapsulate vlan tag tpid (0x8100|0x88a8|0x9100) vlan <0-4094>",
        "Specify the encapsulation parameters\n"
        "Vlan access\n"
        "Take a VLAN tag\n"
        "Tpid\n"
        "0x8100\n"
        "0x88a8\n"
        "0x9100\n"
        "Specify vlan\n"
        "<0-4094>")

/**
 * \page no_mpls_pw_encapsulate_cmd_vtysh no encapsulate
 * - 功能说明 \n
 *	 将pw封装参数恢复默认
 * - 命令格式 \n
 *	 no encapsulate
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，为ethernet类型不携带tag的pw报文
 * - 命令模式 \n
 *	 vsi视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令将pw封装参数恢复默认ethernet类型不携带tag的pw报文
 *	 - 前置条件 \n
 *	   Vsi-id已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   vsi <1-1024> \n
 *	   encapsulate (ethernet|vlan) (raw|tag) {tpid (0x8100|0x88a8|0x9100) | vlan <0-4094>} \n
 * - 使用举例 \n
 *	   Hios(mpls-vsi-1)# no encapsulate \n
 *     Hios(mpls-vsi-1)# \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_pw_encapsulate_cmd_vtysh,
        "no encapsulate",
        NO_STR
        "Specify the ethernet encapsulation parameters\n")

/**
 * \page mpls_pw_ces_frame_cmd_vtysh ces frame-num <1-40>
 * - 功能说明 \n
 *	 配置ces帧数
 * - 命令格式 \n
 *	 ces frame-num <1-40>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |<1-40>|帧数值|
 *
 * - 缺省情况 \n
 *	 缺省情况下，帧数值未配置
 * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置ces帧数
 *	 - 前置条件 \n
 *	   Vc类型已配置，并且为tdm
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no ces frame-num \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# ces frame-num 15 \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_pw_ces_frame_cmd_vtysh,
        "ces frame-num <1-40>",
        "Specify the ces frame\n"
        "Frame number\n"
        "<1-40>\n")

/**
 * \page no_mpls_pw_ces_frame_cmd_vtysh no ces frame-num
 * - 功能说明 \n
 *	 删除ces
 * - 命令格式 \n
 *	 no ces frame-num
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，帧数值未配置
 * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除ces帧数
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   ces frame-num <1-40>
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# no ces frame-num \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_pw_ces_frame_cmd_vtysh,
        "no ces frame-num",
        NO_STR
        "Specify the ces frame\n"
        "Frame number\n")

/**
 * \page mpls_pw_bind_tunnel_cmd_vtysh tunnel bind interface tunnel USP
 * - 功能说明 \n
 *	 配置pw 公网隧道
 * - 命令格式 \n
 *	 tunnel bind interface tunnel USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |USP|tunnel接口的接口号|
 *
 * - 缺省情况 \n
 *	 缺省情况下，公网隧道未指�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置pw 公网隧道
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no tunnel bind \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# tunnel bind interface tunnel 1/1/1 \n
 *     Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_pw_bind_tunnel_cmd_vtysh,
        "tunnel bind interface tunnel USP",
        "Specify the public tunnel\n"
        "PW bind tunnel\n"
        "Interface\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR)

/**
 * \page mpls_pw_bind_lsp_cmd_vtysh tunnel bind lsp NAME
 * - 功能说明 \n
 *	 配置pw 公网隧道
 * - 命令格式 \n
 *	 tunnel bind lsp NAME
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |---|--------------|
 *	 |lsp NAME|lsp名字|
 *
 * - 缺省情况 \n
 *	 缺省情况下，公网隧道未指�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置pw 公网隧道
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no tunnel bind \n
 * - 使用举例 \n
 *     �? *
 */
DEFSH (VTYSH_MPLS, mpls_pw_bind_lsp_cmd_vtysh,
        "tunnel bind lsp NAME",
        "Specify the public tunnel\n"
        "PW bind tunnel\n"
        "Specify the LSP\n"
        "LSP name\n")

/**
 * \page no_mpls_pw_tunnel_bind_cmd_vtysh no tunnel bind
 * - 功能说明 \n
 *	 Pw解绑定公网隧�? * - 命令格式 \n
 *	 no tunnel bind
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，公网隧道未指�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令解绑公网隧�? *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   tunnel bind {interface tunnel USP|lsp NAME} \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# no tunnel bind \n
 *     Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_pw_tunnel_bind_cmd_vtysh,
        "no tunnel bind",
        NO_STR
        "Specify the public tunnel\n"
        "no tunnel bind\n")

/**
 * \page mpls_pw_tunnel_policy_cmd_vtysh tunnel policy (ldp|mpls-tp|gre)
 * - 功能说明 \n
 *	 配置 pw 隧道策略
 * - 命令格式 \n
 *	 tunnel policy (ldp|mpls-tp|gre)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |ldp|配置ldp隧道策略|
 *   |mpls-tp|配置mpls-tp隧道策略|
 *   |gre|配置gre隧道策略  |
 *
 * - 缺省情况 \n
 *	 缺省情况下，隧道策略未配�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配�?pw 隧道策略
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no tunnel policy (ldp|mpls-tp|gre|all) \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# tunnel policy gre \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_pw_tunnel_policy_cmd_vtysh,
        "tunnel policy (ldp|mpls-tp|gre)",
        "Specify the public tunnel policy\n"
        "Specify the public tunnel policy\n"
        "Policy type: Ldp\n"
        "Policy type: Mpls-tp\n"
        "Policy type: Gre\n")

/**
 * \page no_mpls_pw_tunnel_policy_cmd_vtysh no tunnel policy (ldp|mpls-tp|gre|all)
 * - 功能说明 \n
 *	 删除 pw 隧道策略
 * - 命令格式 \n
 *	 no tunnel policy (ldp|mpls-tp|gre|all)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |ldp|删除ldp隧道策略|
 *	 |mpls-tp|配置mpls-tp隧道策略|
 *	 |gre|配置gre隧道策略  |
 *   |all|删除所有隧道策略|
 *
 * - 缺省情况 \n
 *	 缺省情况下，隧道策略未配�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删�?pw 隧道策略
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw 被绑定不可修�? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   tunnel policy (ldp|mpls-tp|gre|all) \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# no tunnel policy gre \n
 *	   Huahuan(mpls-pw)# \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_pw_tunnel_policy_cmd_vtysh,
        "no tunnel policy (ldp|mpls-tp|gre|all)",
        NO_STR
        "Delete the public tunnel policy\n"
        "Delete the public tunnel policy\n"
        "Policy type: Ldp\n"
        "Policy type: Mpls-tp\n"
        "Policy type: Gre\n"
        "Delete all policy\n")

/**
 * \page mpls_pw_switch_pw_cmd_vtysh switch pw NAME
 * - 功能说明 \n
 *	 配置pw交换
 * - 命令格式 \n
 *	 switch pw NAME
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |----|--------------|
 *	 |NAME|Pw的名字|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置pw交换
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, mpls_pw_switch_pw_cmd_vtysh,
        "switch pw NAME",
        "Specify the pw exchange\n"
        "Pseudo wire\n"
        "The name for pw -- max.31\n")

/**
 * \page no_mpls_pw_switch_pw_cmd_vtysh no switch pw
 * - 功能说明 \n
 *	 取消pw交换
 * - 命令格式 \n
 *	 no switch pw
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |----|--------------|
 *	 |NAME|Pw的名字|
 *
 * - 缺省情况 \n
 *	 缺省情况下，未配�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令取消pw交换
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, no_mpls_pw_switch_pw_cmd_vtysh,
        "no switch pw",
        NO_STR
        "Specify the pw exchange\n"
        "Pseudo wire\n"
        "The name for pw -- max.31\n")

/**
 * \page mpls_pw_switch_vsi_cmd_vtysh switch vsi <1-1024> (upe|spe)
 * - 功能说明 \n
 *	 配置Vsi下添加pw
 * - 命令格式 \n
 *	 switch vsi <1-1024> (upe|spe)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |----|--------------|
 *	 |<1-1024>|Vsi实例号|
 *   |upe|靠近用户侧的pe设备|
 *   |spe|网络核心pe设备|
 *
 * - 缺省情况 \n
 *	 缺省情况下，vsi中未添加pw
 * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置vsi下添加pw
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw不可复用
 *	 - 相关命令 \n
 *	   pw NAME \n
 *     no switch vsi \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)# switch vsi 1 spe \n
 *     Huahuan(mpls-pw)#
 *
 */
DEFSH (VTYSH_MPLS, mpls_pw_switch_vsi_cmd_vtysh,
        "switch vsi <1-1024> (upe | spe)",
        "Specify the pw exchange\n"
        "Virtual Switching Instance\n"
        "<1-1024>\n"
        "Upe\n"
        "Spe\n")

/**
 * \page no_mpls_pw_switch_vsi_cmd_vtysh no switch vsi
 * - 功能说明 \n
 *	 将pw从vsi中移�? * - 命令格式 \n
 *	 no switch vsi
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，vsi中未添加pw
 * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令将pw从vsi实例中移�? *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   switch vsi <1-1024> (upe|spe) \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw)#no switch vsi \n
 *	   Huahuan(mpls-pw)#
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_pw_switch_vsi_cmd_vtysh,
        "no switch vsi",
        NO_STR
        "Specify the pw exchange\n"
        "Virtual Switching Instance\n")


DEFSH (VTYSH_MPLS, mpls_pw_switch_vsi_backup_cmd_vtysh,
        "switch vsi <1-1024> backup pw NAME",
        "Specify the pw exchange\n"
        "Virtual Switching Instance\n"
        "<1-1024>\n"
        "Specify the backup pw\n"
        "\n"
        "PW name\n")


DEFSH (VTYSH_MPLS, mpls_pw_switch_vsi_master_cmd_vtysh,
        "switch vsi <1-1024> master pw NAME",
        "Specify the pw exchange\n"
        "Virtual Switching Instance\n"
        "<1-1024>\n"
        "Specify the master pw\n"
        "\n"
        "PW name\n")


DEFSH (VTYSH_MPLS, no_mpls_pw_switch_vsi_backup_cmd_vtysh,
        "no switch vsi backup pw",
        NO_STR
        "Specify the pw exchange\n"
        "Virtual Switching Instance\n"
        "<1-1024>\n"
        "Specify the backup pw\n"
        "\n")

/**
 * \page show_mpls_pw_cmd_vtysh show mpls pw [NAME]
 * - 功能说明 \n
 *	 显示pw配置信息
 * - 命令格式 \n
 *	 show mpls pw [NAME]
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |NAME|Pw名字（可选）|
 *
 * - 缺省情况 \n
 *	 缺省情况下，pw未配�? * - 命令模式 \n
 *	 Config视图、mpls视图、pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示pw配置信息
 *	 - 前置条件 \n
 *	   Pw已配�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, show_mpls_pw_cmd_vtysh,
        "show mpls pw [NAME]",
        SHOW_STR
        "Mpls\n"
        "Pseudo wire\n"
        "The name for pw\n")

/**
 * \page show_mpls_pw_cmd_vtysh show mpls pw [NAME]
 * - 功能说明 \n
 *	 接口下配置主备pw切换模式及切换延时时�? * - 命令格式 \n
 *	 show mpls pw [NAME]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |failback|回切模式（默认）|
 *   |<0-3600>|回切模式延时时间（默�?0s）|
 *
 * - 缺省情况 \n
 *	 缺省情况下，为回切模式，延时30s
 * - 命令模式 \n
 *	 2层以太口及子接口，tdm�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置接口下主备pw切换模式及切换延时时�? *	 - 前置条件 \n
 *	   主备pw必须都配置后才可以配置此命令
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   mpls l2vpn pw NAME [backup] \n
 * - 使用举例 \n
 *	   Huahuan(config-ethernet1/1/1)#pw backup non-failback \n
 *     Huahuan(config-ethernet1/1/1)#pw backup failback wtr 10 \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_interface_pw_failback_cmd_vtysh,
        "mpls pw backup failback [wtr <0-3600>]",
        "Mpls configuration\n"
        "PW protection switch configuration\n"
        "Backup\n"
        "PW protection failback\n"
        "Working Time Regulations\n"
        "Failback to the time, the default 30s\n")

/**
 * \page mpls_interface_pw_non_failback_cmd_vtysh mpls pw backup non-failback
 * - 功能说明 \n
 *	 接口下配置主备pw切换模式及切换延时时�? * - 命令格式 \n
 *	 mpls pw backup non-failback
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |non-failback|不回切模式|
 *	 |<0-3600>|回切模式延时时间（默�?0s）|
 *
 * - 缺省情况 \n
 *	 缺省情况下，为回切模式，延时30s
 * - 命令模式 \n
 *	 2层以太口及子接口，tdm�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置接口下主备pw切换模式及切换延时时�? *	 - 前置条件 \n
 *	   主备pw必须都配置后才可以配置此命令
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   mpls l2vpn pw NAME [backup] \n
 * - 使用举例 \n
 *	   Huahuan(config-ethernet1/1/1)#pw backup non-failback \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_interface_pw_non_failback_cmd_vtysh,
        "mpls pw backup non-failback",
        "Mpls configuration\n"
        "PW protection switch configuration\n"
        "Backup\n"
        "PW protection non-failback")

/**
 * \page mpls_if_bind_pw_cmd_vtysh mpls l2vpn pw NAME [backup]
 * - 功能说明 \n
 *	 接口配置pw
 * - 命令格式 \n
 *	 mpls l2vpn pw NAME [backup]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |NAME|Pw的名字|
 *	 |Backup|接口配置备pw|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口未配置pw
 * - 命令模式 \n
 *	 2层以太口及子接口，tdm�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置接口下pw，主或备
 *	 - 前置条件 \n
 *	   Pw相关配置完整
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   Pw不能复用 \n
 *     必须先配置主才能配置�?\n
 *	 - 相关命令 \n
 *     pw NAME \n
 *	   no mpls l2vpn pw NAME [backup] \n
 * - 使用举例 \n
 *	   Huahuan(config-ethernet1/1/1)# mpls l2vpn pw name \n
 *	   Huahuan(config-ethernet1/1/1)# mpls l2vpn pw name1 backup \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_if_bind_pw_cmd_vtysh,
        "mpls l2vpn pw NAME [backup]",
        "Mpls\n"
        "L2 vpn\n"
        "Pseudo wire\n"
        "Specify the pw name\n"
        "Specofy the pw is backup\n")

/**
 * \page no_mpls_if_bind_pw_cmd_vtysh no mpls l2vpn pw [backup]
 * - 功能说明 \n
 *	 接口下取消pw配置
 * - 命令格式 \n
 *	 no mpls l2vpn pw [backup]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |Backup|取消备pw配置|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口未配置pw
 * - 命令模式 \n
 *	 2层以太口及子接口，tdm�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令取消接口下pw配置
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   必须先取消备pw配置，才能取消主pw配置 \n
 *	 - 相关命令 \n
 *	   pw NAME \n
 *	   mpls l2vpn pw NAME [backup] \n
 * - 使用举例 \n
 *	   Huahuan(config-ethernet1/1/1)# no mpls l2vpn pw \n
 *	   Huahuan(config-ethernet1/1/1)# no mpls l2vpn pw name1 backup \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_if_bind_pw_cmd_vtysh,
        "no mpls l2vpn pw [backup]",
        NO_STR
        "Mpls\n"
        "L2 vpn\n"
        "Pseudo wire\n"
        "Specofy the pw is backup\n")

/**
 * \page show_mpls_interface_cmd_vtysh show mpls interface
 * - 功能说明 \n
 *	 显示mpls接口配置信息
 * - 命令格式 \n
 *	 show mpls interface
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，接口无配�? * - 命令模式 \n
 *	 Config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示mpls接口配置信息
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   mpls vsi <1-1024> \n
 *	   mpls l2vpn pw NAME [backup] \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, show_mpls_interface_cmd_vtysh,
        "show mpls interface",
        SHOW_STR
        "Mpls information\n"
        "Mpls interface information\n")

/**
 * \page show_mpls_interface_ethernet_cmd_vtysh show mpls interface ethernet USP
 * - 功能说明 \n
 *	 显示百兆以太接口mpls配置信息
 * - 命令格式 \n
 *	 show mpls interface ethernet USP
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |USP|百兆以太接口号|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口无配�? * - 命令模式 \n
 *	 Config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示百兆以太接口mpls配置信息
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, show_mpls_interface_ethernet_cmd_vtysh,
        "show mpls interface ethernet USP",
        SHOW_STR
        "Mpls information\n"
        "Mpls interface information\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR)

/**
 * \page show_mpls_interface_gigabit_ethernet_cmd_vtysh show mpls interface gigabitethernet USP
 * - 功能说明 \n
 *	 显示千兆以太接口mpls配置信息
 * - 命令格式 \n
 *	 show mpls interface gigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|千兆以太接口号|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口无配�? * - 命令模式 \n
 *	 Config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示千兆以太接口mpls配置信息
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, show_mpls_interface_gigabit_ethernet_cmd_vtysh,
        "show mpls interface gigabitethernet USP",
        SHOW_STR
        "Mpls information\n"
        "Mpls interface information\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)

/**
 * \page show_mpls_interface_xgigabit_ethernet_cmd_vtysh show mpls interface xgigabitethernet USP
 * - 功能说明 \n
 *	 显示万兆以太接口mpls配置信息
 * - 命令格式 \n
 *	 show mpls interface xgigabitethernet USP
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |USP|万兆以太接口号|
 *
 * - 缺省情况 \n
 *	 缺省情况下，接口无配�? * - 命令模式 \n
 *	 Config视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示万兆以太接口mpls配置信息
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   �? * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, show_mpls_interface_xgigabit_ethernet_cmd_vtysh,
        "show mpls interface xgigabitethernet USP",
        SHOW_STR
        "Mpls information\n"
        "Mpls interface information\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)

DEFSH (VTYSH_MPLS, show_mpls_interface_tdm_cmd_vtysh,
        "show mpls interface tdm USP",
        SHOW_STR
        "Mpls information\n"
        "Mpls interface information\n"
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_TDM_VHELP_STR)

/**
 * \page mpls_exp_domain_enable_cmd_vtysh qos mapping enable exp-domain <1-5>
 * - 功能说明 \n
 *	 将已创建的QoS Domain应用到接�? * - 命令格式 \n
 *	 qos mapping enable exp-domain <1-5>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<1-5>|表示可应用的映射模版范围�?1-5>|
 *
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 命令行配置在接口模式�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令将一个已创建的QoS Domain应用到接口，用于QoS
 *	 - 前置条件 \n
 *	   应用到接口之前需要先创建映射模板
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show qos interface \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH(VTYSH_MPLS | VTYSH_IFM, mpls_exp_domain_enable_cmd_vtysh,
        "qos mapping enable exp-domain <1-5>",
        "Quality of Service\n"
        "QoS mapping\n"
        "Enable\n"
        "Exp to queue mapping domain\n"
        "QoS mapping domain ID\n")

/**
 * \page no_mpls_exp_domain_enable_cmd_vtysh no qos mapping enable exp-domain
 * - 功能说明 \n
 *	 取消应用到接口的QoS Domain
 * - 命令格式 \n
 *	 no qos mapping enable exp-domain
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 命令行配置在接口模式�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   取消一个应用到接口的QoS Domain，用于QoS
 *	 - 前置条件 \n
 *	   接口已应用QoS Domain
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show qos interface \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH(VTYSH_MPLS | VTYSH_IFM, no_mpls_exp_domain_enable_cmd_vtysh,
        "no qos mapping enable exp-domain",
        NO_STR
        "Quality of Service\n"
        "QoS mapping\n"
        "Enable\n"
        "Exp to queue mapping domain\n")

/**
 * \page mpls_exp_phb_enable_cmd_vtysh qos mapping enable exp-phb <1-5>
 * - 功能说明 \n
 *	 将已创建的QoS Domain应用到接�? * - 命令格式 \n
 *	 qos mapping enable exp-phb <1-5>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<1-5>|表示可应用的映射模版范围�?1-5>|
 *
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 命令行配置在接口模式�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令将一个已创建的QoS phb应用到接口，用于QoS
 *	 - 前置条件 \n
 *	   应用到接口之前需要先创建映射模板
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show qos interface \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH(VTYSH_MPLS | VTYSH_IFM, mpls_exp_phb_enable_cmd_vtysh,
        "qos mapping enable exp-phb <0-5>",
        "Quality of Service\n"
        "QoS mapping\n"
        "Enable\n"
        "Queue to exp mapping\n"
        "QoS mapping phb ID\n")

/**
 * \page mpls_pw_exp_phb_enable_cmd_vtysh qos mapping enable exp-phb <1-5>
 * - 功能说明 \n
 *	 将已创建的QoS Domain应用到接�? * - 命令格式 \n
 *	 qos mapping enable exp-phb <1-5>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|--------------|
 *	 |<1-5>|表示可应用的映射模版范围�?1-5>|
 *
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 命令行配置在接口模式�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令将一个已创建的QoS phb应用到接口，用于QoS
 *	 - 前置条件 \n
 *	   应用到接口之前需要先创建映射模板
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show qos interface \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH(VTYSH_MPLS, mpls_pw_exp_domain_enable_cmd_vtysh,
        "qos mapping enable exp-domain <1-5>",
        "Quality of Service\n"
        "QoS mapping\n"
        "Enable\n"
        "Exp to queue mapping domain\n"
        "QoS mapping domain ID\n")


DEFSH(VTYSH_MPLS, no_mpls_pw_exp_domain_enable_cmd_vtysh,
        "no qos mapping enable exp-domain",
        NO_STR
        "Quality of Service\n"
        "QoS mapping\n"
        "Enable\n"
        "Exp to queue mapping domain\n")



DEFSH (VTYSH_MPLS, mpls_pw_exp_phb_enable_cmd_vtysh,
        "qos mapping enable exp-phb <1-5>",
        "Quality of Service\n"
        "QoS mapping\n"
        "Enable\n"
        "Queue to exp mapping\n"
        "QoS mapping phb ID\n")


DEFSH(VTYSH_MPLS, no_mpls_pw_exp_phb_enable_cmd_vtysh,
        "no qos mapping enable exp-phb",
        NO_STR
        "Quality of Service\n"
        "QoS mapping\n"
        "Enable\n"
        "Queue to exp mapping\n")
/**
 * \page no_mpls_exp_phb_enable_cmd_vtysh no qos mapping enable exp-phb
 * - 功能说明 \n
 *	 取消应用到接口的QoS phb
 * - 命令格式 \n
 *	 no qos mapping enable exp-phb
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 命令行配置在接口模式�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   取消一个应用到接口的QoS phb，用于QoS
 *	 - 前置条件 \n
 *	   接口已应用QoS Domain
 *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show qos interface \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH(VTYSH_MPLS | VTYSH_IFM, no_mpls_exp_phb_enable_cmd_vtysh,
        "no qos mapping enable exp-phb",
        NO_STR
        "Quality of Service\n"
        "QoS mapping\n"
        "Enable\n"
        "Queue to exp mapping\n")

/**
 * \page mpls_pw_statistics_enable_cmd_vtysh statistics enable
 * - 功能说明 \n
 *	 使能pw统计功能
 * - 命令格式 \n
 *	 statistics enable
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，pw统计未使�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令使能Pw统计
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *     no statistics enable \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH(VTYSH_MPLS, mpls_pw_statistics_enable_cmd_vtysh,
        "statistics enable",
        "PW statistics\n"
        "Enable statistics of the PW\n")

/**
 * \page no_mpls_pw_statistics_enable_cmd_vtysh no statistics enable
 * - 功能说明 \n
 *	 取消使能pw统计功能
 * - 命令格式 \n
 *	 no statistics enable
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，pw统计未使�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   取消使用该命令使能Pw统计
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   statistics enable \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH(VTYSH_MPLS, no_mpls_pw_statistics_enable_cmd_vtysh,
        "no statistics enable",
        NO_STR
        "PW statistics\n"
        "Enable statistics of the PW\n")

/**
 * \page mpls_pw_statistics_clear_cmd_vtysh statistics clear
 * - 功能说明 \n
 *	 清除pw统计计数
 * - 命令格式 \n
 *	 statistics clear
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，pw统计未使�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   取消使用该命令清除Pw统计
 *	 - 前置条件 \n
 *	   Pw统计已使�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   statistics enable \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# statistics enable \n
 *     Huahuan(mpls-pw-pw1)# statistics clear \n
 *
 */
DEFSH(VTYSH_MPLS, mpls_pw_statistics_clear_cmd_vtysh,
        "statistics clear",
        "PW statistics\n"
        "Clear statistics of the PW\n")

/**
 * \page show_mpls_pw_statistics_cmd_vtysh show statistics
 * - 功能说明 \n
 *	 显示pw统计
 * - 命令格式 \n
 *	 show statistics
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，pw统计未使�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示Pw统计
 *	 - 前置条件 \n
 *	   Pw统计已使�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   statistics enable \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# statistics enable \n
 *	   Huahuan(mpls-pw-pw1)# show statistics \n
 *
 */
DEFSH(VTYSH_MPLS, show_mpls_pw_statistics_cmd_vtysh,
        "show statistics",
        SHOW_STR
        "PW statistics\n")

/**
 * \page mpls_pw_mplstp_oam_session_enable_cmd_vtysh mplstp-oam enable session <1-65535>
 * - 功能说明 \n
 *	 绑定oam 会话，检测pw
 * - 命令格式 \n
 *	 mplstp-oam enable session <1-65535>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |<1-65535>|oam 会话 id|
 *
 * - 缺省情况 \n
 *	 缺省情况下，oam会话未绑�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令绑�?oam 会话
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no mplstp-oam enable session \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# \n
 *	   Huahuan(mpls-pw-pw1)# mplstp-oam enable session 1 \n
 *
 */
DEFSH(VTYSH_MPLS, mpls_pw_mplstp_oam_session_enable_cmd_vtysh,
        "mplstp-oam enable session <1-65535>",
        "Mpls transport profile OAM\n"
        "Enable\n"
        "Session\n"
        "Range of session:<1-65535>\n")

/**
 * \page no_mpls_pw_mplstp_oam_session_enable_cmd_vtysh no mplstp-oam enable session <1-65535>
 * - 功能说明 \n
 *	 绑定oam 会话
 * - 命令格式 \n
 *	 no mplstp-oam enable session <1-65535>
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，oam会话未绑�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令解�?oam 会话
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   mplstp-oam enable session <1-65535> \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# \n
 *	   Huahuan(mpls-pw-pw1)# no mplstp-oam enable session \n
 *
 */
DEFSH(VTYSH_MPLS, no_mpls_pw_mplstp_oam_session_enable_cmd_vtysh,
        "no mplstp-oam enable session",
        NO_STR
        "Mpls transport profile OAM\n"
        "Enable\n"
        "Session\n")

/**
 * \page mpls_pw_bfd_session_enable_cmd_vtysh bfd enable session <1-65535>
 * - 功能说明 \n
 *	 绑定bfd 会话，检测pw
 * - 命令格式 \n
 *	 bfd enable session <1-65535>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |<1-65535>|bfd 会话 id|
 *
 * - 缺省情况 \n
 *	 缺省情况下，bfd会话未绑�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令绑�?bfd 会话
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no bfd enable session \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# \n
 *	   Huahuan(mpls-pw-pw1)# bfd enable session 1 \n
 *
 */
DEFSH(VTYSH_MPLS, mpls_pw_bfd_session_enable_cmd_vtysh,
        "bfd enable session <1-65535> [raw]",
        BFD_STR
        "Enable BFD for PW\n"
        "Session for BFD\n"
        "<1-65535>\n"
        "BFD pkt encape with ip/udp header\n"
		"BFD pkt encape without ip/udp header\n")

DEFSH(VTYSH_MPLS, mpls_pw_bfd_dynamic_enable_cmd_vtysh,
	"bfd enable [raw]",
	BFD_STR
	"Enable dynamic BFD for PW\n"
	"BFD pkt encape without ip/udp header\n")

/**
 * \page no_mpls_pw_bfd_session_enable_cmd_vtysh no bfd enable session
 * - 功能说明 \n
 *	 解绑bfd 会话
 * - 命令格式 \n
 *	 no bfd enable session
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，bfd会话未绑�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令解�?bfd 会话
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   bfd enable session <1-65535> \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# \n
 *	   Huahuan(mpls-pw-pw1)# no bfd enable session \n
 *
 */
DEFSH(VTYSH_MPLS, no_mpls_pw_bfd_session_enable_cmd_vtysh,
        "no bfd enable session",
        BFD_STR
        "Enable BFD for PW\n"
        "Enable BFD for PW\n"
        "Session for BFD\n")

/**
 * \page tunnel_statistics_enable_cmd_vtysh statistics enable
 * - 功能说明 \n
 *	 使能pw统计功能
 * - 命令格式 \n
 *	 statistics enable
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，pw统计未使�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令使能Pw统计
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no statistics enable \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# \n
 *	   Huahuan(mpls-pw-pw1)# statistics enable \n
 *
 */
DEFSH(VTYSH_MPLS | VTYSH_IFM, tunnel_statistics_enable_cmd_vtysh,
        "statistics enable",
        "Tunnel statistics\n"
        "Enable statistics of the tunnel\n")

/**
 * \page no_tunnel_statistics_enable_cmd_vtysh no statistics enable
 * - 功能说明 \n
 *	 取消使能pw统计功能
 * - 命令格式 \n
 *	 no statistics enable
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，pw统计未使�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令使能Pw统计
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   no statistics enable \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# \n
 *	   Huahuan(mpls-pw-pw1)# no statistics enable \n
 *
 */
DEFSH(VTYSH_MPLS | VTYSH_IFM, no_tunnel_statistics_enable_cmd_vtysh,
        "no statistics enable",
        NO_STR
        "Tunnel statistics\n"
        "Enable statistics of the tunnel\n")

/**
 * \page tunnel_statistics_clear_cmd_vtysh statistics clear
 * - 功能说明 \n
 *	 清除pw统计计数
 * - 命令格式 \n
 *	 statistics clear
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，pw统计未使�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令清除Pw统计
 *	 - 前置条件 \n
 *	   Pw统计已使�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   statistics enable \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# statistics enable \n
 *	   Huahuan(mpls-pw-pw1)# statistics clear \n
 *
 */
DEFSH(VTYSH_MPLS, tunnel_statistics_clear_cmd_vtysh,
        "statistics clear",
        "Tunnel statistics\n"
        "Clear statistics of the tunnel\n")

/**
 * \page show_tunnel_statistics_cmd_vtysh show statistics
 * - 功能说明 \n
 *	 显示pw统计
 * - 命令格式 \n
 *	 show statistics
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，pw统计未使�? * - 命令模式 \n
 *	 Pw视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示Pw统计
 *	 - 前置条件 \n
 *	   Pw统计已使�? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   pw NAME \n
 *	   statistics enable \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# statistics enable \n
 *	   Huahuan(mpls-pw-pw1)# show statistics \n
 *
 */
DEFSH(VTYSH_MPLS, show_tunnel_statistics_cmd_vtysh,
        "show statistics",
        SHOW_STR
        "Tunnel statistics\n")

#if 0
DEFSH(VTYSH_MPLS, tunnel_mplstp_oam_session_enable_tunnel_cmd_vtysh,
        "mplstp-oam enable session <1-65535>",
        "Mpls transport profile OAM\n"
        "Enable\n"
        "Session\n"
        "Range of session:<1-65535>\n")
#endif

/**
 * \page tunnel_mplstp_oam_session_enable_cmd_vtysh mplstp-oam enable session <1-65535> (master-lsp|slave-lsp)
 * - 功能说明 \n
 *	 绑定oam 会话
 * - 命令格式 \n
 *	 mplstp-oam enable session <1-65535> (master-lsp|slave-lsp)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |<1-65535>|oam 会话 id|
 *   |master-lsp|主lsp|
 *   |slave-lsp|备lsp|
 *
 * - 缺省情况 \n
 *	 缺省情况下，oam会话未绑�? * - 命令模式 \n
 *	 tunnel视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令绑�?oam 会话
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   interface tunnel USP \n
 *	   no mplstp-oam enable session {master-lsp|slave-lsp} \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# \n
 *	   Huahuan(mpls-pw-pw1)# mplstp-oam enable session 1 master-lsp \n
 *
 */
DEFSH(VTYSH_MPLS, tunnel_mplstp_oam_session_enable_cmd_vtysh,
        "mplstp-oam enable session <1-65535> (master-lsp | slave-lsp)",
        "Mpls transport profile OAM\n"
        "Enable\n"
        "Session\n"
        "Range of session:<1-65535>\n"
        "Master-lsp\n"
        "Backup-lsp\n")

#if 0
DEFSH(VTYSH_MPLS, no_tunnel_mplstp_oam_session_enable_tunnel_cmd_vtysh,
        "no mplstp-oam enable session",
        NO_STR
        "Mpls transport profile OAM\n"
        "Enable\n"
        "Session\n")
#endif

/**
 * \page no_tunnel_mplstp_oam_session_enable_cmd_vtysh no mplstp-oam enable session (master-lsp|slave-lsp)
 * - 功能说明 \n
 *	 解绑oam 会话
 * - 命令格式 \n
 *	 no mplstp-oam enable session (master-lsp|slave-lsp)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |master-lsp|主lsp|
 *   |slave-lsp|备lsp|
 *
 * - 缺省情况 \n
 *	 缺省情况下，oam会话未绑�? * - 命令模式 \n
 *	 tunnel视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令解�?oam 会话
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   interface tunnel USP \n
 *	   mplstp-oam enable session <1-65535> {master-lsp|slave-lsp} \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# \n
 *	   Huahuan(mpls-pw-pw1)# no mplstp-oam enable session master-lsp \n
 *
 */
DEFSH(VTYSH_MPLS, no_tunnel_mplstp_oam_session_enable_cmd_vtysh,
        "no mplstp-oam enable session (master-lsp | slave-lsp)",
        NO_STR
        "Mpls transport profile OAM\n"
        "Enable\n"
        "Session\n"
        "Master-lsp\n"
        "Backup-lsp\n")

/**
 * \page tunnel_bfd_session_enable_tunnel_cmd_vtysh bfd enable session <1-65535>
 * - 功能说明 \n
 *	 绑定bfd 会话
 * - 命令格式 \n
 *	 bfd enable session <1-65535>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |<1-65535>|bfd 会话 id|
 *   |master-lsp|主lsp|
 *   |slave-lsp|备lsp|
 *
 * - 缺省情况 \n
 *	 缺省情况下，bfd会话未绑�? * - 命令模式 \n
 *	 tunnel视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令绑定bfd 会话
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   interface tunnel USP \n
 *	   no bfd enable session {master-lsp|slave-lsp} \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# \n
 *	   Huahuan(mpls-pw-pw1)# bfd enable session 1 master-lsp \n
 *
 */
 #if 0
DEFSH(VTYSH_MPLS, tunnel_bfd_session_enable_tunnel_cmd_vtysh,
        "bfd enable session <1-65535>",
        BFD_STR
        "Enable\n"
        "Session\n"
        "Session id:<1-65535>\n")
#endif
/**
 * \page tunnel_bfd_session_enable_lsp_cmd_vtysh bfd enable session <1-65535> (master-lsp|slave-lsp)
 * - 功能说明 \n
 *	 绑定bfd 会话
 * - 命令格式 \n
 *	 bfd enable session <1-65535> (master-lsp|slave-lsp)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |<1-65535>|bfd 会话 id|
 *   |master-lsp|主lsp|
 *   |slave-lsp|备lsp|
 *
 * - 缺省情况 \n
 *	 缺省情况下，bfd会话未绑�? * - 命令模式 \n
 *	 tunnel视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令绑定bfd 会话
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   interface tunnel USP \n
 *	   no bfd enable session {master-lsp|slave-lsp} \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# \n
 *	   Huahuan(mpls-pw-pw1)# bfd enable session 1 master-lsp \n
 *
 */
DEFSH(VTYSH_MPLS, tunnel_bfd_session_enable_lsp_cmd_vtysh,
        "bfd enable session <1-65535> (master-lsp | slave-lsp)",
        BFD_STR
        "Enable\n"
        "Session\n"
        "Session idp:<1-65535>\n"
        "Master-lsp\n"
        "Backup-lsp\n")

/**
 * \page no_tunnel_bfd_session_enable_tunnel_cmd_vtysh no bfd enable session
 * - 功能说明 \n
 *	 解绑bfd 会话
 * - 命令格式 \n
 *	 no bfd enable session
 * - 参数说明 \n
 *   �? * - 缺省情况 \n
 *	 缺省情况下，bfd会话未绑�? * - 命令模式 \n
 *	 tunnel视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令解绑bfd 会话
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   interface tunnel USP \n
 * - 使用举例 \n
 *	   �? *
 */

DEFSH(VTYSH_MPLS, no_tunnel_bfd_session_enable_tunnel_cmd_vtysh,
        "no bfd enable session",
        NO_STR
        BFD_STR
        "Enable\n"
        "Session\n")


/**
 * \page no_tunnel_bfd_session_enable_lsp_cmd_vtysh no bfd enable session (master-lsp|slave-lsp)
 * - 功能说明 \n
 *	 解绑oam 会话
 * - 命令格式 \n
 *	 no bfd enable session (master-lsp|slave-lsp)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |master-lsp|主lsp|
 *   |slave-lsp|备lsp|
 *
 * - 缺省情况 \n
 *	 缺省情况下，bfd 会话未绑�? * - 命令模式 \n
 *	 tunnel视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令解�?bfd 会话
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   interface tunnel USP \n
 *	   bfd enable session <1-65535> {master-lsp|slave-lsp} \n
 * - 使用举例 \n
 *	   Huahuan(mpls-pw-pw1)# \n
 *	   Huahuan(mpls-pw-pw1)# no bfd enable session master-lsp \n
 *
 */
DEFSH(VTYSH_MPLS, no_tunnel_bfd_session_enable_lsp_cmd_vtysh,
        "no bfd enable session (master-lsp | slave-lsp)",
        NO_STR
        BFD_STR
        "Enable\n"
        "Session\n"
        "Master-lsp\n"
        "Backup-lsp\n")



/* tp-oam新增命令*/
DEFUNSH(VTYSH_MPLS,
        mplstp_oam_session,
    	mplstp_oam_session_cmd,
    	"mplstp-oam session <1-65535>",
    	"Mpls transport profile OAM\n"
    	"Session\n"
    	"Range of session:<1-65535>\n")
{
    uint16_t session = 0;
    char *pprompt = NULL;

    /*进入会话视图*/
    session = atoi(argv[0]);

    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-oam-session-%d)#", session);
    }
    vty->node = MPLSTP_OAM_SESS_NODE;

    return CMD_SUCCESS;
}

DEFUNSH (VTYSH_MPLS,
        vtysh_exit_mplsoam_session,
        vtysh_exit_mplsoam_session_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}

ALIAS (vtysh_exit_mplsoam_session,
        vtysh_quit_mplsoam_session_cmd,
        "quit",
        "Exit current mode and down to previous mode\n")

DEFSH(VTYSH_MPLS,
    	no_mplstp_oam_session_cmd_vtysh,
    	"no mplstp-oam session <1-65535>",
    	NO_STR
    	"Mpls transport profile OAM\n"
    	"Session\n"
    	"Range of session:<1-65535>\n")



/* mplstp-oam meg */
DEFUNSH(VTYSH_MPLS,
		mplstp_oam_meg,
		mplstp_oam_meg_cmd,
		"mplstp-oam meg MEGNAME",
		"Mpls transport profile OAM\n"
		"Maintenance entity group\n"
		"String of meg name:<1-12>\n")
{
	uchar name[NAME_STRING_LEN] = {0};
    char *pprompt = NULL;

    /*è¿›å…¥ä¼šè¯è§†å›�? */
	memcpy(name, argv[0], strlen(argv[0]));
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-oam-meg-%s)#", name);
    }
    vty->node = MPLSTP_OAM_MEG_NODE;

    return CMD_SUCCESS;
}

DEFUNSH (VTYSH_MPLS,
		vtysh_exit_mplsoam_meg,
		vtysh_exit_mplsoam_meg_cmd,
		"exit",
		"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}


ALIAS (vtysh_exit_mplsoam_meg,
		vtysh_quit_mplsoam_meg_cmd,
		"quit",
		"Exit current mode and down to previous mode\n")


DEFSH(VTYSH_MPLS,
		no_mplstp_oam_meg_cmd_vtysh,
		"no mplstp-oam meg MEGNAME",
		"Delete\n"
		"Mpls transport profile OAM\n"
		"Maintenance entity group\n"
		"String of meg name:<1-12>\n")

DEFSH(VTYSH_MPLS,
		mplstp_oam_meg_level_cmd_vtysh,
		"level <0-7>",
		"Meg level\n"
		"Range of meg level:<0-7>\n")

DEFSH(VTYSH_MPLS,
		no_mplstp_oam_meg_level_cmd_vtysh,
		"no level",
		"Delete\n"
		"Level\n")


DEFSH(VTYSH_MPLS,
		mplstp_oam_service_meg_cmd_vtysh,
		"service meg MEGNAME",
		"Service\n"
		"Maintenance entity group\n"
		"String of meg name:<1-12>\n")

DEFSH(VTYSH_MPLS,
		no_mplstp_oam_service_meg_cmd_vtysh,
		"no service meg",
		"Delete\n"
		"Service\n"
		"Maintenance entity group\n")

DEFSH(VTYSH_MPLS,
		mplstp_oam_mp_cmd_vtysh,
		"(mep-id | mip-id) <1-8191>",
		"MEP id\n"
		"MIP id\n"
		"Range of mp-id:<1-8191>\n")

DEFSH(VTYSH_MPLS,
		no_mplstp_oam_mp_cmd_vtysh,
		"no (mep-id | mip-id) <1-8191>",
		"Delete\n"
		"MEP id\n"
		"MIP id\n"
		"Range of mp-id:<1-8191>\n")



DEFSH(VTYSH_MPLS,
        mplstp_oam_session_priority_cmd_vtysh,
        "priority <0-7>",
        "Oam packet priority\n"
        "Range of priority:<0-7>\n")


DEFSH(VTYSH_MPLS,
        no_mplstp_oam_session_priority_cmd_vtysh,
        "no priority",
        NO_STR
        "Oam packet priority\n")


DEFSH(VTYSH_MPLS,
        mplstp_oam_session_rmep_cmd_vtysh,
        "remote-mep <1-8191>",
        "Remote-mep\n"
        "Range of remote-mep:<1-8191>\n")


DEFSH(VTYSH_MPLS,
        no_mplstp_oam_session_rmep_cmd_vtysh,
        "no remote-mep <1-8191>",
        NO_STR
        "Remote-mep\n"
        "Range of remote-mep:<1-8191>\n")


DEFSH(VTYSH_MPLS,
		mplstp_oam_session_cc_enable_cmd_vtysh,
		"cc enable (3 | 10 | 100 | 1000 | 10000 | 60000 | 600000)",
		"Continuity check\n"
		"Enable.\n"
		"3.3ms.\n"
		"10ms.\n"
		"100ms.\n"
		"1000ms.\n"
		"10s.\n"
		"1min.\n"
		"10min.\n")


DEFSH(VTYSH_MPLS,
        no_mplstp_oam_session_cc_enable_cmd_vtysh,
        "no cc enable",
        NO_STR
        "Continuity check\n"
        "Enable\n")

DEFSH(VTYSH_MPLS,
	mplstp_oam_session_ais_enable_cmd_vtysh,
	"ais enable level <0-7> interval (1s | 60s)",
	"Alarm indication signal\n"
	"Enable.\n"
	"Ais level\n"
	"Range of ais level:<0-7>\n"
	"Interval.\n"
	"Value of interval:1s\n"
	"Value of interval:60s\n")


DEFSH(VTYSH_MPLS,
        no_mplstp_oam_session_ais_enable_cmd_vtysh,
        "no ais enable",
        NO_STR
        "Alarm indication signal.\n"
        "Enable.\n")


DEFSH(VTYSH_MPLS,
	mplstp_oam_session_csf_enable_cmd_vtysh,
	"csf enable level <0-7> interval (1s | 60s)",
	"Alarm indication signal\n"
	"Enable.\n"
	"Csf level\n"
	"Range of csf level:<0-7>\n"
	"Interval.\n"
	"Value of interval:1s\n"
	"Value of interval:60s\n")


DEFSH(VTYSH_MPLS,
		no_mplstp_oam_session_csf_enable_cmd_vtysh,
		"no csf enable",
		NO_STR
		"Alarm indication signal.\n"
		"Enable.\n")


DEFSH(VTYSH_MPLS,
	     mplstp_oam_session_lock_enable_cmd_vtysh,
	     "lock enable level <0-7> interval (1s | 60s) ",
	     "Lock\n"
	     "Enable\n"
	     "Lock level\n"
		 "Range of lock level:<0-7>\n"
		 "Interval.\n"
		 "Value of interval:1s\n"
		 "Value of interval:60s\n")

DEFSH(VTYSH_MPLS,
	     no_mplstp_oam_session_lock_enable_cmd_vtysh,
	     "no lock enable",
	     "Delete\n"
	     "Lock\n"
	     "Enable\n")


DEFSH(VTYSH_MPLS,
        mplstp_oam_section_session_mac_cmd_vtysh,
        "section mac XX:XX:XX:XX:XX:XX [vlan <1-4094>]",
        "Section layer\n"
        "Destination mac address\n"
        "Mac format:XX:XX:XX:XX:XX:XX\n"
        "Vlan\n"
        "Range of vlan-id:<1-4094>\n")


DEFSH(VTYSH_MPLS,
        no_mplstp_oam_section_session_mac_cmd_vtysh,
        "no section mac",
        "Delete\n"
        "Section layer\n"
        "Destination mac address\n")


DEFSH(VTYSH_MPLS,
    	mplstp_oam_section_session_enable_cmd_vtysh,
    	"mplstp-oam enable session <1-65535>",
    	"Mpls transport profile OAM\n"
    	"Enable\n"
    	"Session\n"
    	"Range of session:<1-65535>\n")


DEFSH(VTYSH_MPLS,
        no_mplstp_oam_section_session_enable_cmd_vtysh,
        "no mplstp-oam enable session",
        NO_STR
        "Mpls transport profile OAM\n"
        "Enable\n"
        "Session\n")


DEFSH(VTYSH_MPLS,
        mplstp_oam_lm_enable_cmd_vtysh,
        "lm enable {interval <1-3600> | frequency <0-65535>}",
        "Loss Measurement\n"
        "Enable\n"
        "Interval of Loss Measurement Message\n"
        "Range of interval(s):<1-3600>\n"
        "Frequency\n"
        "Range of frequency(times)\n")


DEFSH(VTYSH_MPLS,
        no_mplstp_oam_lm_enable_cmd_vtysh,
        "no lm enable",
        "Delete\n"
        "Loss Measurement\n"
        "Enable\n")


DEFSH(VTYSH_MPLS,
        mplstp_oam_dm_enable_cmd_vtysh,
        "dm enable {interval <1-3600> | frequency <0-65535> | exp <0-7> | tlv-size <1-400>}",
        "Delay Measurement\n"
        "Enable\n"
        "Interval of Delay Measurement Message\n"
        "Range of interval(s):<1-3600>\n"
        "Frequency\n"
        "Range of frequency(times)\n"
        "Priority of sending delay measurement packets, the default is 7\n"
	    "Range of exp-value:<0-7>\n"
	    "Tlv size of dmm\n"
	    "Range of data-tlv size:<1-400>\n")


DEFSH(VTYSH_MPLS,
        no_mplstp_oam_dm_enable_cmd_vtysh,
        "no dm enable",
        "Delete\n"
        "Delay Measurement\n"
        "Enable\n")

#if 0
DEFSH(VTYSH_MPLS,
        mplstp_oam_lb_enable_cmd_vtysh,
        "lb (enable | disable)",
        "Oam Loopback.\n"
        "Enable.\n"
        "Disable.\n")

DEFSH(VTYSH_MPLS,
	    mplstp_oam_lb_enable_cmd_vtysh,
		"lb enable {mip-ttl <1-255> | node-id A.B.C.D | packet-size <64-1500> | request-tlv}",
		"Oam Loopback.\n"
		"Enable\n"
		"TTL values from local to mip.\n"
		"Range of ttl:<1-255>.\n"
		"The ID of node.\n"
		"Specify the ID of node.\n"
		"Size of an LBM packet.\n"
		"Range of packet-size:<64-1500>\n"
		"Optional requesting MEP ID TLV\n")
#endif
/* lb for pw or lsp transit node */
DEFSH(VTYSH_MPLS,
	    mplstp_oam_lb_enable_mip_cmd_vtysh,
		"lb enable mip-ttl <1-255> node-id A.B.C.D {packet-size <64-1500> | request-tlv | exp <0-7>}",
		"Oam Loopback.\n"
		"Enable\n"
		"TTL values from local to mip.\n"
		"Range of ttl:<1-255>.\n"
		"The ID of node.\n"
		"Specify the ID of node.\n"
		"Size of an LBM packet.\n"
		"Range of packet-size:<64-1500>\n"
		"Optional requesting MEP ID TLV\n"
		"Exp\n"
   		"Range of exp:<0-7>\n")


/* lb for edge node */
DEFSH(VTYSH_MPLS,
	    mplstp_oam_lb_enable_mep_cmd_vtysh,
		"lb enable {packet-size <64-1500> | request-tlv | exp <0-7>}",
		"Oam Loopback.\n"
		"Enable\n"
		"Size of an LBM packet.\n"
		"Range of packet-size:<64-1500>\n"
		"Optional requesting MEP ID TLV\n"
		"Exp\n"
    		"Range of exp:<0-7>\n")

/* lb for edge node */
DEFSH(VTYSH_MPLS,
	    mplstp_oam_lb_enable_sec_cmd_vtysh,
		"lb enable mip-ttl <1-255> {packet-size <64-1500> | request-tlv | exp <0-7>}",
		"Oam Loopback.\n"
		"Enable\n"
		"TTL values from local to mip.\n"
		"Range of ttl:<1-255>.\n"
		"Size of an LBM packet.\n"
		"Range of packet-size:<64-1500>\n"
		"Optional requesting MEP ID TLV\n"
		"Exp\n"
    		"Range of exp:<0-7>\n")


DEFSH(VTYSH_MPLS,
    mplstp_oam_lt_enable_cmd_vtysh,
    "lt enable ttl <1-255> {packet-size <64-1500> | request-tlv | exp <0-7>}",
    "Oam link tracing.\n"
	"Enable\n"
	"Ttl\n"
	"Range of ttl:<1-255>\n"
	"Packet size\n"
	"Range of packet size:<64-1500>\n"
	"Request tlv\n"
	"Exp\n"
	"Range of exp:<0-7>\n")

#if 0
DEFSH(VTYSH_MPLS,
        mplstp_oam_lt_enable_cmd_vtysh,
        "lt (enable | disable)",
        "Oam link tracing.\n"
        "Enable.\n"
        "Disable.\n")
#endif

DEFSH(VTYSH_MPLS,
    	mplstp_oam_session_alarm_report_enable_cmd_vtysh,
    	"alarm-report enable",
    	"Alarm report\n"
    	"Enable\n")

DEFSH(VTYSH_MPLS,
    	no_mplstp_oam_session_alarm_report_enable_cmd_vtysh,
    	"no alarm-report enable",
    	"Delete\n"
    	"Alarm report\n"
    	"Enable")

DEFSH(VTYSH_MPLS,
        mplstp_oam_session_channel_type_cmd_vtysh,
        "channel-type (0x8902 | 0x7FFA)",
        "Oam channel-type\n"
        "0x8902\n"
        "0x7FFA\n")


DEFSH(VTYSH_MPLS,
        no_mplstp_oam_session_channel_type_cmd_vtysh,
        "no channel-type",
        "Delete\n"
        "Oam channel-type\n")

DEFSH(VTYSH_MPLS,
	mplstp_oam_session_global_channel_type_cmd_vtysh,
	"oam global channel-type (0x8902 | 0x7FFA)",
	"Oam.\n"
	"Global.\n"
	"Oam channel-type.\n"
	"0x8902\n"
	"0x7FFA\n")

DEFSH(VTYSH_MPLS,
	no_mplstp_oam_session_global_channel_type_cmd_vtysh,
	"no oam global channel-type",
	"Delete.\n"
	"Oam.\n"
	"Global.\n"
	"Oam channel-type\n")


DEFSH(VTYSH_MPLS,
    	show_mplstp_oam_session_cmd_vtysh,
    	"show mplstp-oam session [<1-65535>]",
    	"Display\n"
    	"Mpls transport profile OAM\n"
    	"Session\n"
    	"Range of session:<1-65535>\n")


DEFSH(VTYSH_MPLS,
		show_mplstp_oam_meg_cmd_vtysh,
		"show mplstp-oam meg [MEGNAME]",
		"Display.\n"
		"Mpls transport profile OAM\n"
		"Maintenance entity group\n"
		"String of meg name:<1-12>\n")




DEFSH(VTYSH_MPLS,
        mplstp_oam_mip_rmep_config_cmd_vtysh,
        "mip fwd-rmep <1-8191> rev-rmep <1-8191>",
        "Maintenance entity group intermediate Point\n"
        "Forward remote id\n"
        "Range of forward remote-id:<1-8191>\n"
        "Reverse remote id\n"
        "Range of reverse remote-id:<1-8191>\n")

DEFSH(VTYSH_MPLS,
        no_mplstp_oam_mip_rmep_config_cmd_vtysh,
        "no mip remote-mep",
        "Delete\n"
        "Maintenance entity group intermediate Point\n"
        "Remote mep\n")

DEFSH(VTYSH_MPLS,
        mplstp_oam_mip_bind_lsp_cmd_vtysh,
        "mip bind lsp LSPNAME (fwd | rev)",
        "Maintenance entity group intermediate Point\n"
		"Bind\n"
		"Lsp\n"
		"String of lspname:<1-31>\n"
		"Forward transit lsp\n"
		"Reverse transit lsp\n")

DEFSH(VTYSH_MPLS,
        no_mplstp_oam_mip_bind_lsp_cmd_vtysh,
        "no mip bind lsp (fwd | rev)",
        "Delete\n"
        "Maintenance entity group intermediate Point\n"
		"Bind\n"
		"Lsp\n"
		"Forward direction\n"
	    "Reverse direction\n")

#if 0
DEFSH(VTYSH_MPLS,
        mplstp_oam_mip_bind_pw_cmd_vtysh,
        "mip bind pw PWNAME",
        "Maintenance entity group intermediate Point\n"
        "Bind\n"
        "Pw\n"
        "String of pwname:<1-31>\n")

DEFSH(VTYSH_MPLS,
        no_mplstp_oam_mip_bind_pw_cmd_vtysh,
        "no mip bind pw",
        "Delete\n"
        "Maintenance entity group intermediate Point\n"
        "Bind\n"
        "Pw\n")

#endif



DEFSH (VTYSH_MPLS, mpls_log_level_ctl_cmd_vtysh,
        "debug mpls(enable | disable)",
        "Output log of debug level\n"
        "Program name\n"
        "Enable\n"
        "Disable\n")


DEFSH (VTYSH_MPLS, mpls_show_log_level_ctl_cmd_vtysh,
        "show debug mpls",
        SHOW_STR
        "Output log of debug level\n"
        "Program name\n")


DEFSH (VTYSH_MPLS,
        ping_tunnel_cmd_vtysh,
        "ping tunnel USP",
        "Ping\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR)


DEFSH (VTYSH_MPLS,
        ping_lsp_cmd_vtysh,
        "ping lsp name NAME",
        "Ping\n"
        "Lsp\n"
        "Lspname\n"
        "The name for lsp -- max.31\n")

DEFSH(VTYSH_MPLS,
		ping_ldp_lsp_cmd_vtysh,
		"ping lsp ip A.B.C.D/M nexthop A.B.C.D",
		"Ping\n"
		"Lsp\n"
		"Specify IPv4 LSP's destination\n"
		"IPv4 LSP's destination\n"
		"Next hop\n"
		"Next hop address\n")

DEFSH (VTYSH_MPLS,
        ping_pw_cmd_vtysh,
        "ping pw NAME",
        "Ping\n"
        "Pw\n"
        "The name for pw -- max.31\n")

DEFSH (VTYSH_MPLS,
        ping_pw_remote_cmd_vtysh,
        "ping pw NAME  remote A.B.C.D pw-id <1-4294967295>",
        "Ping\n"
        "Pw\n"
        "The name for pw -- max.31\n"
        "Remote PW information\n"
		"Remote ip:X.X.X.X\n"
		"Remote pw id\n"
		"Range of remote pw id:<1-4294967295>\n")

DEFSH (VTYSH_MPLS,
        ping_pw_reply_cmd_vtysh,
        "ping pw NAME  -r (2|4)",
        "Ping\n"
        "Pw\n"
        "The name for pw -- max.31\n"
		"Reply mode\n"
		"Reply via an IPv4/IPv6 UDP packet\n"
		"Reply via application level control channel\n")

DEFSH (VTYSH_MPLS,
		ping_pw_remote_reply_cmd_vtysh,
		"ping pw NAME  remote A.B.C.D pw-id <1-4294967295> -r (2|4)",
		"Ping\n"
		"Pw\n"
		"The name for pw -- max.31\n"
		"Remote PW information\n"
		"Remote ip:X.X.X.X\n"
		"Remote pw id\n"
		"Range of remote pw id:<1-4294967295>\n"		
		"Reply mode\n"
		"Reply via an IPv4/IPv6 UDP packet\n"
		"Reply via application level control channel\n")

DEFSH (VTYSH_MPLS,
		ping_pw_reply_remote_cmd_vtysh,
		"ping pw NAME  -r (2|4) remote A.B.C.D pw-id <1-4294967295> ",
		"Ping\n"
		"Pw\n"
		"The name for pw -- max.31\n"
		"Reply mode\n"
		"Reply via an IPv4/IPv6 UDP packet\n"
		"Reply via application level control channel\n"
		"Remote PW information\n"
		"Remote ip:X.X.X.X\n"
		"Remote pw id\n"
		"Range of remote pw id:<1-4294967295>\n")	

DEFSH (VTYSH_MPLS,
        tracert_static_lsp_cmd_vtysh,
        "tracert lsp name NAME -t <1-255>",
        "Tracert\n"
        "Lsp\n"
        "Lspname\n"
        "The name -- max.31\n"
        "Ttl\n"
        "<1-255>\n")

DEFSH (VTYSH_MPLS,
        tracert_ldp_lsp_cmd_vtysh,
        "tracert lsp ip A.B.C.D/M nexthop A.B.C.D -t <1-255>",
        "Tracert\n"
        "Lsp\n"
        "Specify IPv4 LSP's destination\n"
		"IPv4 LSP's destination\n"
		"Next hop\n"
		"Next hop address\n"
        "Ttl\n"
        "<1-255>\n")

DEFSH (VTYSH_MPLS,
        tracert_pw_cmd_vtysh,
        "tracert pw NAME -t <1-255> {remote A.B.C.D}",
        "Tracert\n"
        "Pw\n"
        "The name -- max.31\n"
        "Ttl\n"
        "<1-255>\n"
        "Remote ip\n"
        "IP address format A.B.C.D\n")


DEFSH (VTYSH_MPLS,
        tracert_tunnel_cmd_vtysh,
        "tracert tunnel USP -t <1-255>",
        "Tracert\n"
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR
        "Ttl\n"
        "<1-255>\n")

/**
 * \page mpls_qos_car_cmd_vtysh qos car (ingress|egress) cir <1-10000000> pir <1-10000000>
 * - 功能说明 \n
 *   配置 pw 入或出方向限�? * - 命令格式 \n
 *   qos car (ingress|egress) cir <1-10000000> pir <1-10000000>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |ingress|报文入方向|
 *   |egress|报文出方向|
 *   |cir|承诺信息速率  |
 *   |pir|峰值信息速率|
 *
 * - 缺省情况 \n
 *   缺省情况下，未配置限�? * - 命令模式 \n
 *   Pw视图
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     使用该命令配置pw报文限�? *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     pw NAME \n
 *     no qos car (ingress|egress) \n
 * - 使用举例 \n
 *     Huahuan(mpls-pw-pw1)# \n
 *     Huahuan(mpls-pw-pw1)#qos car ingress cir 100000 pir 120000 \n
 *
 */
DEFSH(VTYSH_MPLS, mpls_qos_car_cmd_vtysh,
        "qos car (ingress | egress) cir <1-10000000> pir <1-10000000>",
        "Quality of Service\n"
        "Committed Access Rate\n"
        "Ingress\n"
        "Egress\n"
        "Committed information rate\n"
        "Committed information rate value <1-10000000> (Unit: Kbps)\n"
        "Peak information rate\n"
        "Peak information rate value <1-10000000> (Unit: Kbps)\n")


DEFSH(VTYSH_MPLS, mpls_pw_qos_car_l1_cmd_vtysh,
        "qos car include-interframe (ingress | egress) cir <1-10000000> pir <1-10000000>",
        "Quality of Service\n"
        "Committed Access Rate\n"
        "Include-interframe\n"
        "Ingress\n"
        "Egress\n"
        "Committed information rate\n"
        "Committed information rate value <1-10000000> (Unit: Kbps)\n"
        "Peak information rate\n"
        "Peak information rate value <1-10000000> (Unit: Kbps)\n")


DEFSH(VTYSH_MPLS, mpls_tunnel_qos_car_l1_cmd_vtysh,
        "qos car include-interframe (ingress | egress) cir <1-10000000>",
        "Quality of Service\n"
        "Committed Access Rate\n"
        "Include-interframe\n"
        "Ingress\n"
        "Egress\n"
        "Committed information rate\n"
        "Committed information rate value <1-10000000> (Unit: Kbps)\n")



/**
 * \page no_mpls_qos_car_cmd_vtysh no qos car (ingress|egress)
 * - 功能说明 \n
 *   删除pw下的限速配�? * - 命令格式 \n
 *   no qos car (ingress|egress)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |ingress|报文入方向|
 *   |egress|报文出方向|
 *
 * - 缺省情况 \n
 *   缺省情况下，未配置限�? * - 命令模式 \n
 *   Pw视图
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     使用该命令删除限速配�? *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     pw NAME \n
 *     qos car (ingress|egress) cir <1-10000000> cbs <0-512> {pir <1-10000000> pbs <0-512>} \n
 * - 使用举例 \n
 *     Huahuan(mpls-pw-pw1)# \n
 *     Huahuan(mpls-pw-pw1)#no qos car ingress \n
 *
 */
DEFSH(VTYSH_MPLS, no_mpls_qos_car_cmd_vtysh,
        "no qos car (ingress | egress)",
        NO_STR
        "Quality of Service\n"
        "Committed Access Rate\n"
        "Ingress\n"
        "Egress\n")

/**
 * \page mpls_hqos_queue_apply_cmd_vtysh hqos cir <0-10000000> {pir <0-10000000>|queue-profile <1-100>}
 * - 功能说明 \n
 *   配置 pw 拥塞避免，流量整形，队列调度
 * - 命令格式 \n
 *   hqos cir <0-10000000> {pir <0-10000000>|queue-profile <1-100>}
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |cir|承诺信息速率|
 *   |pir|峰值信息速率|
 *   |queue-profile|队列模板|
 *
 * - 缺省情况 \n
 *   缺省情况下，hqos未配�? * - 命令模式 \n
 *   Pw视图
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     使用该命令配�?pw �?hqos
 *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     pw NAME \n
 *     no hqos \n
 * - 使用举例 \n
 *     Huahuan(mpls-pw-pw1)# \n
 *     Huahuan(mpls-pw-pw1)# hqos cir 100000 pir 120000 queue-profile 1 \n
 *
 */
DEFSH(VTYSH_MPLS,
        mpls_hqos_queue_apply_cmd_vtysh,
        "hqos cir <0-10000000> {pir <0-10000000> | queue-profile <1-100>}",
        "Hierarchical QoS\n"
        "Committed information rate\n"
        "Committed information rate value (Unit: Kbps)\n"
        "Peak information rate\n"
        "Peak information rate value (Unit: Kbps)\n"
        "Queue profile\n"
        "Queue profile ID\n")

/**
 * \page no_mpls_hqos_queue_apply_cmd_vtysh no hqos
 * - 功能说明 \n
 *   删除pw下的 hqos
 * - 命令格式 \n
 *   no hqos
 * - 参数说明 \n
 *   �? * - 缺省情况 \n
 *   缺省情况下，hqos未配�? * - 命令模式 \n
 *   Pw视图
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     使用该命令删除pw下hqos配置
 *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     pw NAME \n
 *     no hqos \n
 * - 使用举例 \n
 *     Huahuan(mpls-pw-pw1)# \n
 *     Huahuan(mpls-pw-pw1)#hqos cir 100000 pir 120000 queue-profile 1 \n
 *
 */
DEFSH(VTYSH_MPLS,
        no_mpls_hqos_queue_apply_cmd_vtysh,
        "no hqos",
        NO_STR
        "Hierarchical QoS\n")


/* 调试命令，查看标签池 */
DEFSH(VTYSH_MPLS,
        show_label_pool_cmd_vtysh,
        "show mpls label pool",
        SHOW_STR
        "This is a temporary command to view the label pool\n"
        "This is a temporary command to view the label pool\n"
        "This is a temporary command to view the label pool\n")

/**
 * \page ldp_enable_cmd mpls ldp enable
 * - 功能说明 \n
 *   配置LDP全局开
 * - 命令格式 \n
 *   mpls ldp enable
 * - 参数说明 \n
 *   �? * - 缺省情况 \n
 *   缺省情况下，LDP功能为关闭状�? * - 命令模式 \n
 *   MPLS视图
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     �? *   - 前置条件 \n
 *     配置LSR-ID
 *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     show mpls ldp config
 * - 使用举例 \n
 *     Huahuan(config-mpls)# mpls ldp enable \n
 *
 */
DEFSH (VTYSH_MPLS,ldp_enable_cmd,
        "mpls ldp enable",
        "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
        "Label Distribution Protocol(LDP)\n"
        "Enable interface local ldp function\n")

/**
 * \page no_ldp_enable_cmd no mpls ldp enable
 * - 功能说明 \n
 *   配置LDP全局�? * - 命令格式 \n
 *   no mpls ldp enable
 * - 参数说明 \n
 *   �? * - 缺省情况 \n
 *   缺省情况下，LDP功能为关闭状�? * - 命令模式 \n
 *   MPLS视图
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     �? *   - 前置条件 \n
 *     配置LSR-ID
 *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     show mpls ldp config
 * - 使用举例 \n
 *     Huahuan(config-mpls)# no mpls ldp enable \n
 *
 */
DEFSH (VTYSH_MPLS,no_ldp_enable_cmd,
        "no mpls ldp enable",
        "Delete\n"
        "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
        "Label Distribution Protocol(LDP)\n"
        "Enable interface local ldp function\n")

/**
 * \page ldp_label_policy_cmd ldp label-policy (all|host|none)
 * - 功能说明 \n
 *   配置LSP触发策略
 * - 命令格式 \n
 *   ldp label-policy (all|host|none)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |all|为所有路由产生LSP|
 *   |Ip-prefix|为通过LDP前缀列表过滤的路由产生LSP|
 *   |host|为主机路由产生LSP  |
 *   |none|所有路由均不产生LSP|
 *
 * - 缺省情况 \n
 *   缺省情况下，为HOST路由分配标签
 * - 命令模式 \n
 *   三层物理接口配置模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     通过配置LSP建立的触发策略，可以使符合条件的路由触发LDP建立LSP
 *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     Ip-prefix模式需要提前在MPLS视图下配置LDP前缀列表，否则将不产生LSP
 *   - 相关命令 \n
 *     show mpls ldp config \n
 *     mpls ldp ip-prefix [A.B.C.D/M] \n
 * - 使用举例 \n
 *     �? *
 */
DEFSH (VTYSH_MPLS,ldp_label_policy_cmd,
        "ldp label-policy (all | ip-prefix | host | none)",
        "Label Distribution Protocol(LDP)\n"
        "Policy for label distribution\n"
        "Distribute label for all the route\n"
        "Distribute label for route match the ip-prefix\n"
        "Distribute label for the host route\n"
        "Never distribute label\n")

/**
 * \page no_ldp_label_policy_cmd no ldp label-policy
 * - 功能说明 \n
 *   删除LSP触发策略
 * - 命令格式 \n
 *   no ldp label-policy
 * - 参数说明 \n
 *   �? * - 缺省情况 \n
 *   缺省情况下，为HOST路由分配标签
 * - 命令模式 \n
 *   三层物理接口配置模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     删除路由触发LDP建立的LSP
 *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     Ip-prefix模式需要提前在MPLS视图下配置LDP前缀列表，否则将不产生LSP
 *   - 相关命令 \n
 *     show mpls ldp config \n
 *     mpls ldp ip-prefix [A.B.C.D/M] \n
 * - 使用举例 \n
 *     �? *
 */
DEFSH (VTYSH_MPLS,no_ldp_label_policy_cmd,
        "no ldp label-policy",
        "Delete\n"
        "Label Distribution Protocol(LDP)\n"
        "Policy for label distribution\n")

/**
 * \page ldp_advertise_mode_cmd mpls ldp advertise (dod|du)
 * - 功能说明 \n
 *   配置标签发布方式
 * - 命令格式 \n
 *   mpls ldp advertise (dod|du)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |dod|按需标签发布方式|
 *   |du|独立标签发布方式|
 *
 * - 缺省情况 \n
 *   缺省情况下，为独立标签发布方�?du)
 * - 命令模式 \n
 *   MPLS模式�? * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     使用该命令配置标签通告模式
 *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     修改标签发布方式会导致LDP会话重建
 *   - 相关命令 \n
 *     show mpls ldp config \n
 * - 使用举例 \n
 *     �? *
 */
DEFSH (VTYSH_MPLS,ldp_advertise_mode_cmd,
        "mpls ldp advertise (dod | du)",
        "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
        "Label Distribution Protocol(LDP)\n"
        "Label Advertisement Mode\n"
        "Downstream on Demand Mode\n"
        "Downstream Unsolicited Mode\n")

/**
 * \page no_ldp_advertise_mode_cmd no mpls ldp advertise
 * - 功能说明 \n
 *   删除标签发布方式
 * - 命令格式 \n
 *   no mpls ldp advertise
 * - 参数说明 \n
 *   �? * - 缺省情况 \n
 *   缺省情况下，为独立标签发布方�?du)
 * - 命令模式 \n
 *   MPLS模式�? * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     使用该命令删除标签通告模式
 *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     修改标签发布方式会导致LDP会话重建
 *   - 相关命令 \n
 *     show mpls ldp config \n
 * - 使用举例 \n
 *     �? *
 */
DEFSH (VTYSH_MPLS,no_ldp_advertise_mode_cmd,
        "no mpls ldp advertise",
        "Delete\n"
        "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
        "Label Distribution Protocol(LDP)\n"
        "Label Advertisement Mode\n")

/**
 * \page ldp_keepalive_interval_cmd mpls ldp keepalive <1-3600>
 * - 功能说明 \n
 *   LDP会话的KeepAlive发送定时间�? * - 命令格式 \n
 *   mpls ldp keepalive <1-3600>
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |<1-3600>|Ldp会话保活报文的发送间隔|
 *
 * - 缺省情况 \n
 *   缺省情况下，默认300s
 * - 命令模式 \n
 *   MPLS模式�? * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     �? *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     show mpls ldp config \n
 * - 使用举例 \n
 *     Hios(config-mpls)# mpls ldp keepalive \n
 *       <1-3600>  The range of keepalive time
 *
 */
DEFSH (VTYSH_MPLS,ldp_keepalive_interval_cmd,
        "mpls ldp keepalive <1-3600>",
        "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
        "Label Distribution Protocol(LDP)\n"
        "Keepalive timer\n"
        "The range of keepalive time\n")

/**
 * \page no_ldp_keepalive_interval_cmd no mpls ldp keepalive
 * - 功能说明 \n
 *   删除LDP会话的KeepAlive发送定时间�? * - 命令格式 \n
 *   no mpls ldp keepalive
 * - 参数说明 \n
 *   �? * - 缺省情况 \n
 *   缺省情况下，默认300s
 * - 命令模式 \n
 *   MPLS模式�? * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     �? *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     show mpls ldp config \n
 * - 使用举例 \n
 *     Hios(config-mpls)# mpls ldp keepalive \n
 *       <1-3600>  The range of keepalive time \n
 *     Hios(config-mpls)# no mpls ldp keepalive \n
 *
 */
DEFSH (VTYSH_MPLS,no_ldp_keepalive_interval_cmd,
        "no mpls ldp keepalive",
        "Delete\n"
        "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
        "Label Distribution Protocol(LDP)\n"
        "Keepalive timer\n")

/*ldp  new cmd add here */
DEFSH (VTYSH_MPLS,ldp_hello_interval_cmd,
    	"mpls ldp hello <1-3600>",
    	"Specify MPLS(Multiprotocol Label Switching) configuration information\n"
    	"Label Distribution Protocol(LDP)\n"
    	"Specify interval of Hello-send time(in seconds)\n"
    	"INTEGER<1-3600> Interval of Hello-send timer(in seconds)\n")

DEFSH (VTYSH_MPLS,no_ldp_hello_interval_cmd,
    	"no mpls ldp hello",
    	"Delete\n"
    	"Specify MPLS(Multiprotocol Label Switching) configuration information\n"
    	"Label Distribution Protocol(LDP)\n"
    	"Specify interval of Hello-send time(in seconds)\n")

DEFSH(VTYSH_MPLS,mpls_ldp_prefix_cmd,
	"mpls ldp ip-prefix A.B.C.D/M",
	"Specify MPLS(Multiprotocol Label Switching) configuration information\n"
	"Label Distribution Protocol(LDP)\n"
	"Ip prefix that need to create lsp\n"
	"Ip prefix, format: A.B.C.D for IPv4\n")


DEFSH(VTYSH_MPLS,no_mpls_ldp_prefix_cmd,
	"no mpls ldp ip-prefix [A.B.C.D/M]",
	"Delete\n"
	"Specify MPLS(Multiprotocol Label Switching) configuration information\n"
	"Label Distribution Protocol(LDP)\n"
	"Ip prefix that need to create lsp\n"
	"Ip prefix, format: A.B.C.D/M for IPv4\n")

DEFSH (VTYSH_MPLS,ldp_bfd_enable_cmd,
	"ldp bfd enable",
	"Label Distribution Protocol(LDP)\n"
	"BFD(Bidirectional Forwarding Detection) function\n"
	"Enable ability of ldp to create bfd session\n")

DEFSH (VTYSH_MPLS,no_ldp_bfd_enable_cmd,
	"no ldp bfd enable",
	"Delete\n"
	"Label Distribution Protocol(LDP)\n"
	"Disable ability of ldp to create bfd session\n")

DEFSH (VTYSH_MPLS,mpls_ldp_remote_peer_cmd,
	"mpls ldp remote-peer A.B.C.D",
	"Specify MPLS(Multiprotocol Label Switching) configuration information\n"
	"Label Distribution Protocol(LDP)\n"
	"Specify remote peer\n"
	"Peer lsrid format: A.B.C.D\n")

DEFSH (VTYSH_MPLS,no_mpls_ldp_remote_peer_cmd,
	"no mpls ldp remote-peer A.B.C.D",
	"Delete\n"
	"Specify MPLS(Multiprotocol Label Switching) configuration information\n"
	"Label Distribution Protocol(LDP)\n"
	"Specify remote peer\n"
	"Peer lsrid format: A.B.C.D\n")


DEFSH (VTYSH_MPLS,ldp_peer_lsrid_debug_cmd,
    "debug ldp peer A.B.C.D",
    DEBUG_STR
    "Label Distribution Protocol(LDP)\n"
    "Peer lsrid to be debuged\n"
    "Ip address, format: A.B.C.D\n")

DEFSH (VTYSH_MPLS,no_ldp_peer_lsrid_debug_cmd,
    "no debug ldp peer",
    DEBUG_STR
    "Label Distribution Protocol(LDP)\n"
    "Peer lsrid to be debuged\n")

DEFSH (VTYSH_MPLS,ldp_procedures_debug_cmd,
	"debug ldp {discovery | fsm | notify | label | pw | main | all}",
	DEBUG_STR
	"Label Distribution Protocol(LDP)\n"
	"Discovery debugging function\n"
	"Fsm debugging function\n"
	"Notification debugging function\n"
	"label debugging function\n"
	"PW debugging function\n"
	"Main debugging function\n"
	"All debugging functions\n")


DEFSH (VTYSH_MPLS,no_ldp_procedures_debug_cmd,
	"no debug ldp {discovery | fsm | notify | label | pw | main | all}",
	"Delete\n"
	DEBUG_STR
	"Label Distribution Protocol(LDP)\n"
    "Discovery debugging function\n"
    "Fsm debugging function\n"
    "Notification debugging function\n"
    "label debugging function\n"
    "PW debugging function\n"
    "Main debugging function\n"
    "All debugging functions\n")


/**
 * \page ldp_debug_cmd debug ldp (packet|fsm)
 * - 功能说明 \n
 *   开启LDP调试功能
 * - 命令格式 \n
 *   debug ldp (packet|fsm)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |packet|开启LDP报文流程调试|
 *   |fsm|开启LDP会话状态机流程调试|
 *
 * - 缺省情况 \n
 *   缺省情况下，默认不开�? * - 命令模式 \n
 *   MPLS模式�? * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     用于LDP流程调试
 *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     �? * - 使用举例 \n
 *     �? *
 */
DEFSH (VTYSH_MPLS,ldp_debug_cmd,
        "debug ldp (packet | fsm)",
        DEBUG_STR
        "Label Distribution Protocol(LDP)\n"
        "Packet receive and analysize producure\n"
        "The state machine\n")

/**
 * \page no_ldp_debug_cmd no debug ldp (packet|fsm)
 * - 功能说明 \n
 *   关闭LDP调试功能
 * - 命令格式 \n
 *   debug ldp (packet|fsm)
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |packet|开启LDP报文流程调试|
 *   |fsm|开启LDP会话状态机流程调试|
 *
 * - 缺省情况 \n
 *   缺省情况下，默认不开�? * - 命令模式 \n
 *   MPLS模式�? * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     用于LDP流程调试
 *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     �? * - 使用举例 \n
 *     �? *
 */
DEFSH (VTYSH_MPLS,no_ldp_debug_cmd,
        "no debug ldp (packet | fsm)",
        "Delete\n"
        DEBUG_STR
        "Label Distribution Protocol(LDP)\n"
        "Packet receive and analysize producure\n"
        "The state machine\n")

/**
 * \page ldp_show_config_cmd show mpls ldp config
 * - 功能说明 \n
 *   显示全局LDP配置
 * - 命令格式 \n
 *   show mpls ldp config
 * - 参数说明 \n
 *   �? * - 缺省情况 \n
 *   �? * - 命令模式 \n
 *   配置模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     查看LDP全局配置
 *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     �? * - 使用举例 \n
 *     Huahuan(config)# show mpls ldp config \n
 *       Policy for label distribution: host \n
 *       Label Advertisement Mode: du \n
 *       Ldp label distribution control mode: order \n
 *
 */
DEFSH (VTYSH_MPLS,ldp_show_config_cmd,
        "show mpls ldp config",
        "Show information\n"
        "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
        "Label Distribution Protocol(LDP)\n"
        "Show configuration view information\n")

/**
 * \page ldp_show_session_cmd show mpls ldp session [peer A.B.C.D]
 * - 功能说明 \n
 *   显示LSP触发策略
 * - 命令格式 \n
 *   show mpls ldp session [peer A.B.C.D]
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |peer|表示需要显示的会话是远端会话|
 *   |A.B.C.D |远端会话LSRID|
 *
 * - 缺省情况 \n
 *   缺省情况下，无默认�? * - 命令模式 \n
 *   配置模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     配置结果查看，显示生成的LDP会话
 *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     �? * - 使用举例 \n
 *     �? *
 */
DEFSH (VTYSH_MPLS,ldp_show_session_cmd,
        "show mpls ldp session [peer A.B.C.D]",
        "Show information\n"
        "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
        "Label Distribution Protocol(LDP)\n"
        "Session information\n"
        "Specify peer IP\n"
        "IP_ADDR<X.X.X.X>\n")

/**
 * \page ldp_show_adjance_cmd show mpls ldp adjance [peer A.B.C.D]
 * - 功能说明 \n
 *   配置LSP触发策略
 * - 命令格式 \n
 *   show mpls ldp adjance [peer A.B.C.D]
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |peer|表示需要显示的邻接体的LSRID|
 *   |A.B.C.D |远端会话LSRID|
 * - 缺省情况 \n
 *   缺省情况下，无默认�? * - 命令模式 \n
 *   配置模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     配置结果查看，显示生成的LDP邻接�? *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     �? * - 使用举例 \n
 *     �? *
 */
DEFSH (VTYSH_MPLS,ldp_show_adjance_cmd,
        "show mpls ldp adjance [peer A.B.C.D]",
        "Show information\n"
        "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
        "Label Distribution Protocol(LDP)\n"
        "Adjance information\n"
        "Specify peer IP\n"
        "IP_ADDR<X.X.X.X>\n")

/**
 * \page ldp_show_lsp_cmd show mpls ldp lsp
 * - 功能说明 \n
 *   显示生成的LDP LSP
 * - 命令格式 \n
 *   show mpls ldp lsp
 * - 参数说明 \n
 *   �? * - 缺省情况 \n
 *   �? * - 命令模式 \n
 *   配置模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     查看配置生成的LDP LSP表项
 *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     �? * - 使用举例 \n
 *     �? *
 */
DEFSH (VTYSH_MPLS,ldp_show_lsp_cmd,
        "show mpls ldp lsp",
        "Show information\n"
        "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
        "Label Distribution Protocol(LDP)\n"
        "Information about the label switched path (LSP)\n")

/**
 * \page ldp_show_statistics_cmd show mpls ldp statistics
 * - 功能说明 \n
 *   显示LDP运行产生的收发包、状态机震荡、会话数目等静态计�? * - 命令格式 \n
 *   show mpls ldp statistics
 * - 参数说明 \n
 *   �? * - 缺省情况 \n
 *   �? * - 命令模式 \n
 *   配置模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     用于显示设备运行过程中的静态计数，了解设备运行状�? *   - 前置条件 \n
 *     �? *   - 后续任务 \n
 *     �? *   - 注意事项 \n
 *     �? *   - 相关命令 \n
 *     show mpls ldp statistics
 * - 使用举例 \n
 *     �? *
 */
DEFSH (VTYSH_MPLS,ldp_show_statistics_cmd,
        "show mpls ldp statistics",
        "Show information\n"
        "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
        "Label Distribution Protocol(LDP)\n"
        "Statistics about LDP session\n")

DEFSH(VTYSH_MPLS,ldp_show_ip_prefix_cmd,
	"show mpls ldp ip-prefix",
	"Show information\n"
	"Specify MPLS(Multiprotocol Label Switching) configuration information\n"
	"Label Distribution Protocol(LDP)\n"
	"Ip prefix, format: A.B.C.D/M information\n"
	)
/* LDP cmd add end */



/* MPLSAPS cmd add start*/

/**
* \page mplsaps_session_cmd mpls-aps session
* - 功能说明 \n
*   mpls 视图下创建一个aps会话�?* - 命令格式 \n
*   mpls-aps session <1-255>
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-255>   |每一个session用一个数字来标识，取值范围是1~255|
* - 缺省情况 \n
*   �?* - 命令模式 \n
*   mpls模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     使用该命令配置aps会话�?*   - 前置条件 \n
*     �?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*     no mpls-aps session <1-255>
* - 使用举例 \n
*     Hios(config)# mpls  
*     Hios(config-mpls)# mpls-aps session 1
*
*/

DEFUNSH (VTYSH_MPLS,
        mplsaps_session,
        mplsaps_session_cmd,
        "mpls-aps session <1-255>",
    "Mpls-aps management\n"
    "Session\n"
    "Range of sess_id:<1-255>\n")
{
    vty->node = APS_NODE;
    return CMD_SUCCESS;
}

DEFUNSH (VTYSH_MPLS,
        vtysh_exit_mpls_aps,
        vtysh_exit_mpls_aps_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}

ALIAS (vtysh_exit_mpls_aps,
        vtysh_quit_mpls_aps_cmd,
        "quit",
        "Exit current mode and down to previous mode\n")

DEFSH (VTYSH_MPLS,
    mplsaps_cmd,
    "mpls-aps",
    "Mpls-aps management\n")

DEFSH (VTYSH_MPLS,
    no_mplsaps_session_cmd,
    "no mpls-aps session <1-255>",
    "Delete\n"
    "Mpls-aps management\n"
    "Session\n"
    "Range of sess_id:<1-255>\n")

/**
* \page mplsaps_keep_alive_cmd mpls-aps keep-alive
* - 功能说明 \n
*   配置keep-alive�?* - 命令格式 \n
*   mpls-aps keep-alive <1-600>
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-600>   |Aps报文发送间隔，单位为秒（s），默认�?|
* - 缺省情况 \n
*   缺省情况下，aps报文发送间隔为5s�?* - 命令模式 \n
*   aps模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     使用该命令配置keep-alive�?*   - 前置条件 \n
*     已经配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*     no mpls-aps keep-alive
* - 使用举例 \n
*     Hios(config-mplsaps-session)# mpls-aps keep-alive 60
*
*/

DEFSH (VTYSH_MPLS,
    mplsaps_keep_alive_cmd,
    "mpls-aps keep-alive <1-600>",
    "Mpls-aps management\n"
    "Keepalive\n"
    "Range of keepalive:<1s-600s>\n")

DEFSH (VTYSH_MPLS,
    no_mplsaps_keep_alive_cmd,
    "no mpls-aps keep-alive",
    "Delete\n"
    "Mpls-aps management\n"
    "Keepalive\n")


        
/**
* \page mplsaps_hold_off_cmd mpls-aps hold-off
* - 功能说明 \n
*   配置会话hold-off�?* - 命令格式 \n
*   mpls-aps hold-off <0-600>
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<0-600>   |hold-off时间，单位为秒（s），默认�?|
* - 缺省情况 \n
*   缺省情况下，aps session的hold-off时间�?s�?* - 命令模式 \n
*   aps模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     使用该命令配置aps会话hold-off�?*   - 前置条件 \n
*     已经配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*     no mpls-aps hold-off
* - 使用举例 \n
*     Hios(config-mplsaps-session)# mpls-aps hold-off 60
*
*/

DEFSH (VTYSH_MPLS,
    mplsaps_hold_off_cmd,
    "mpls-aps hold-off <0-600>",
    "Mpls-aps management\n"
    "Hold off\n"
    "Range of hold off:<0s-600s>\n")

DEFSH (VTYSH_MPLS,
    no_mplsaps_hold_off_cmd,
    "no mpls-aps hold-off",
    "Delete\n"
    "Mpls-aps management\n"
    "Hold off\n")


/**
* \page mplsaps_backup_failback_cmd mplsaps backup
* - 功能说明 \n
*   配置切换模式和回切时间�?* - 命令格式 \n
*   mpls-aps backup { non-failback | failback [ wtr <1-12> ] }
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |non-failback   |非返回模式|
*   |failback       |返回模式|
*   |<1-12>         |wtr时间，单位为秒（min），默认�?min|
* - 缺省情况 \n
*   缺省情况下，aps session 的切换模式为failback时，wtr的时间为5min�?* - 命令模式 \n
*   aps模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     使用该命令配置会话工作模式�?*   - 前置条件 \n
*     �?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*    no mpls-aps backup
* - 使用举例 \n
*    Hios(config-mplsaps-session)# mpls-aps backup failback
*/  

DEFSH (VTYSH_MPLS,
    mplsaps_backup_create_cmd,
    "mpls-aps backup non-failback ",
    "Mpls-aps management\n"
    "Protection restoration mode\n"
    "Non-revertive mode\n")

DEFSH (VTYSH_MPLS,
    mplsaps_backup_failback_cmd,
    "mpls-aps backup failback [ wtr <1-12> ]",
    "Mpls-aps management\n"
    "Protection restoration mode\n"
    "Revertive mode\n"
    "Wait to restore\n"
    "Range of wtr:<1min-12min>\n")

DEFSH (VTYSH_MPLS,
    no_mplsaps_backup_create_cmd,
    "no mpls-aps backup",
    "Delete\n"
    "Mpls-aps management\n"
    "Protection restoration mode\n")

/**
* \page mplsaps_priority_cmd mpls-aps priority
* - 功能说明 \n
*   配置会话优先级�?* - 命令格式 \n
*   mpls-aps priority <0-7>
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<0-7>   |优先级范围|
* - 缺省情况 \n
*   无�?* - 命令模式 \n
*   aps模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     使用该命令配置会话优先级�?*   - 前置条件 \n
*     配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*      no mpls-aps priority
* - 使用举例 \n
*     Hios(config-mplsaps-session)# mpls-aps priority 1
*
*/

DEFSH (VTYSH_MPLS,
    mplsaps_priority_cmd,
    "mpls-aps priority <0-7>",
    "Mpls-aps management\n"
    "Mpls-aps packet priority\n"
    "Range of priority:<0-7>\n")

DEFSH (VTYSH_MPLS,
    no_mplsaps_priority_cmd,
     "no mpls-aps priority",
     "Delete\n"
     "Mpls-aps management\n"
     "Mpls-aps packet priority\n")

/**
* \page mplsaps_bind_pw_cmd mpls-aps bind pw
* - 功能说明 \n
*   会话绑定pw�?* - 命令格式 \n
*   mpls-aps bind pw NAME
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |NAME   |Pw名称|
* - 缺省情况 \n
*   无�?* - 命令模式 \n
*   aps模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     使用该命令配置会话绑定pw�?*   - 前置条件 \n
*     配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*      no mpls-aps bind pw
* - 使用举例 \n
*     Hios(config-mplsaps-session)# mpls-aps bind pw 1
*
*/

DEFSH (VTYSH_MPLS,
    mplsaps_bind_pw_cmd,
    "mpls-aps bind pw NAME",
    "Mpls-aps management\n"
    "Bind\n"
    "Pw\n"
    "The name for pw -- max.31\n"
    )

/**
* \page mplsaps_bind_tunnel_cmd mpls-aps bind tunnel
* - 功能说明 \n
*   会话绑定tunnel�?* - 命令格式 \n
*   mpls-aps bind tunnel USP
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |USP  |Tunnel ID|
* - 缺省情况 \n
*   无�?* - 命令模式 \n
*   aps模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     使用该命令配置会话绑定tunnel�?*   - 前置条件 \n
*     配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*      no mpls-aps bind tunnel
* - 使用举例 \n
*     Hios(config-mplsaps-session)# mpls-aps bind tunnel 1/1/1
*
*/
DEFSH(VTYSH_MPLS,
	mplsaps_bind_tunnel_cmd,
	"mpls-aps bind tunnel USP",
	"Mpls-aps management\n"
	"Bind\n"
	CLI_INTERFACE_TUNNEL_STR
	CLI_INTERFACE_TUNNEL_VHELP_STR
	)
DEFSH(VTYSH_MPLS,
	no_mplsaps_bind_tunnel_cmd,
	"no mpls-aps bind tunnel",
	"Delete\n"
	"Mpls-aps management\n"
	"Bind\n"
	CLI_INTERFACE_TUNNEL_STR)

DEFSH (VTYSH_MPLS,
	 no_mplsaps_bind_cmd,
	 "no mpls-aps bind pw",
	 "Delete\n"
	 "Mpls-aps management\n"
	 "Bind\n"
	 "Pw\n")

/**
* \page mplsaps_session_enable_cmd mpls-aps (enable | disable)
* - 功能说明 \n
*   使能或去使能aps会话�?* - 命令格式 \n
*   mpls-aps (enable | disable)
* - 参数说明 \n
*   �?* - 缺省情况 \n
*   无�?* - 命令模式 \n
*   aps模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     使用该命令使能或去使能aps会话�?*   - 前置条件 \n
*     配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*      �?* - 使用举例 \n
*     Hios(config-mplsaps-session)# mpls-aps enable
*
*/

DEFSH (VTYSH_MPLS,
    mplsaps_session_enable_cmd,
    "mpls-aps (enable | disable)",
    "Mpls-aps management\n"
    "Enable\n"
    "Disable\n")

/**
* \page mplsaps_debug_packet_cmd debug mpls-aps packet
* - 功能说明 \n
*   显示aps报文信息的调试开�?* - 命令格式 \n
*   debug mpls-aps packet
* - 参数说明 \n
*   �?* - 缺省情况 \n
*   无�?* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     使用该命令显�?aps报文的信息�?*   - 前置条件 \n
*     配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*      no debug mpls-aps packet
* - 使用举例 \n
*     Hios(config)# debug mpls-aps packet
*
*/

DEFSH (VTYSH_MPLS,
    mplsaps_debug_packet_cmd,
    "debug mpls-aps packet",
    "DEBUG_STR"
    "Mpls-aps management\n"
    "Mpls-aps packet\n" )

DEFSH (VTYSH_MPLS,
    no_mplsaps_debug_packet_cmd,
    "no debug mpls-aps packet",
    "Delete\n"
    "DEBUG_STR"
    "Mpls-aps management\n"
    "Mpls-aps packet\n" )


/**
* \page mplsaps_admin_force_cmd  mpls-aps force-switch
* - 功能说明 \n
*   配置会话强制切换到备�?* - 命令格式 \n
*    mpls-aps force-switch
* - 参数说明 \n
*   �?* - 缺省情况 \n
*   无�?* - 命令模式 \n
*   aps模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     使用该命令配置会话强制切换到备�?*   - 前置条件 \n
*     配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*      no mpls-aps force-switch
* - 使用举例 \n
*     Hios(config-mplsaps-session)# mpls-aps force-switch
*
*/

DEFSH (VTYSH_MPLS,
    mplsaps_admin_force_cmd,
    "mpls-aps force-switch",
    "Mpls-aps management\n"
    "Force switching\n" )


DEFSH (VTYSH_MPLS,
    no_mplsaps_admin_force_cmd,
    "no mpls-aps force-switch",
    "Delete\n"
    "Mpls-aps management\n"
    "Force switching\n" )


/**
* \page mplsaps_admin_manual_cmd  mpls-aps manual-switch
* - 功能说明 \n
*   配置手动切换到备�?* - 命令格式 \n
*    mpls-aps manual-switch
* - 参数说明 \n
*   �?* - 缺省情况 \n
*   无�?* - 命令模式 \n
*   aps模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     使用该命令配置手动切换到备�?*   - 前置条件 \n
*     配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*      no mpls-aps manual-switch
* - 使用举例 \n
*     Hios(config-mplsaps-session)# mpls-aps manual-switch
*
*/

DEFSH (VTYSH_MPLS,
    mplsaps_admin_manual_cmd,
    "mpls-aps manual-switch",
    "Mpls-aps management\n"
    "Manual switching\n" )

DEFSH (VTYSH_MPLS,
    no_mplsaps_admin_manual_cmd,
    "no mpls-aps manual-switch",
    "Delete\n"
    "Mpls-aps management\n"
    "Manual switching\n" )

/**
* \page mplsaps_admin_lockout_cmd  mpls-aps lockout
* - 功能说明 \n
*   配置 lock�?* - 命令格式 \n
*    mpls-aps lockout
* - 参数说明 \n
*   �?* - 缺省情况 \n
*   无�?* - 命令模式 \n
*   aps模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置 lock�?*   - 前置条件 \n
*     配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*      no mpls-aps lockout
* - 使用举例 \n
*     Hios(config-mplsaps-session)# mpls-aps lockout
*
*/

DEFSH (VTYSH_MPLS,
    mplsaps_admin_lockout_cmd,
    "mpls-aps lockout",
    "Mpls-aps management\n"
    "Lockout of protection\n" )

DEFSH (VTYSH_MPLS,
    no_mplsaps_admin_lockout_cmd,
    "no mpls-aps lockout",
    "Delete\n"
    "Mpls-aps management\n"
    "Lockout of protection\n" )

/**
* \page show_mplsaps_config_cmd  show mpls-aps config
* - 功能说明 \n
*   显示 aps的配置�?* - 命令格式 \n
*    show mpls-aps config
* - 参数说明 \n
*   �?* - 缺省情况 \n
*   无�?* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     显示 aps的配置�?*   - 前置条件 \n
*     配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*     �?* - 使用举例 \n
*     Hios(config-mplsaps-session)# show mpls-aps config
*     Num:1
*        session-id        : 1
*        master-pw         : pw1
*        backup-pw         : pw2
*        keep-alive        : 5
*        hold-off          : 0
*        backup-mode       : Failback
*        wtr               : 60
*        priority          : 6
*        session-status    : enabl
*
*
*/

DEFSH (VTYSH_MPLS,
    show_mplsaps_config_cmd,
    "show mpls-aps config",
    "Display.\n"
    "Mpls-aps management\n"
    "Config\n")

/**
* \page show_mplsaps_config_cmd  show mpls-aps session
* - 功能说明 \n
*   显示 aps session的信息�?* - 命令格式 \n
*    show mpls-aps session [<1-255>]
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-255> |会话id范围|
* - 缺省情况 \n
*   无�?* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     显示 aps session的信息�?*   - 前置条件 \n
*     配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*     �?* - 使用举例 \n
*     Hios(config)# show mpls-aps session 
*     Total             : 1
*     enable_num        : 1
*     Num:1
*     session-id    master-pw         backup-pw         current-state       
*     ------------------------------------------------------------------------
*     1             pw1               pw2               Signal Fail for Protection
*
*       active-pw         : Master-pw
*       keep-alive        : 5
*       hold-off          : 0
*       backup-mode       : Failback
*       wtr               : 60
*       priority          : 6
*       session-status    : enable
*
*/

DEFSH (VTYSH_MPLS,
    show_mplsaps_session_cmd,
    "show mpls-aps session [<1-255>]",
    "Display.\n"
    "Mpls-aps management\n"
    "Session\n"
    "Range of local_mep:<1-255>\n")


/**
* \page show_mplsaps_debug_cmd  show mpls-aps debug
* - 功能说明 \n
*   显示APS报文收发包数量及debug开关状态�?* - 命令格式 \n
*    show mpls-aps debug
* - 参数说明 \n
*   �?* - 缺省情况 \n
*   无�?* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     显示APS报文收发包数量及debug开关状态�?*   - 前置条件 \n
*     配置aps会话�?*   - 后续任务 \n
*     �?*   - 注意事项 \n
*     �?*   - 相关命令 \n
*      �?* - 使用举例 \n
*     Hios (config)# show mpls-aps debug
*     aps recv total 159
*     aps send total 159
*     aps error total 0
*     aps debug packet is disable
*
*/

DEFSH (VTYSH_MPLS,
    show_mplsaps_debug_cmd,
    "show mpls-aps debug",
    "Display.\n"
    "Mpls-aps management\n"
    "Debug\n")

/* MPLSAPS cmd add end*/


/* æ–°å¢�?l3vpn å‘½ä»¤è�?*/
/**
 * \page mpls_l3vpn_instance_vtysh l3vpn instance <1-128>
 * - 功能说明 \n
 *	 mpls视图下创建或进入l3vpn 视图
 * - 命令格式 \n
 *	 l3vpn instance <1-128>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-128>|L3vpn实例范围|
 *
 * - 缺省情况 \n
 *	 缺省情况下，vpn实例未创�? * - 命令模式 \n
 *	 Mpls视图�? * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令创建一个l3vpn实例
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   命令行给定vpn实例范围�?1-128>;
 *     实际生效可配置范围如下：HT201/HT201E <1-127>; 
 *     HT2200 <1-128>;
 *	 - 相关命令 \n
 *	   no l3vpn instance <1-128> \n
 *     show l3vpn instance [<1-128>] \n
 * - 使用举例 \n
 *	   Hios(config-mpls)# l3vpn instance 1 \n
 *     Hios(mpls-l3vpn-1)# \n
 *
 */
DEFUNSH (VTYSH_MPLS,
        mpls_l3vpn_instance_vtysh,
        mpls_l3vpn_instance_cmd_vtysh,
        "l3vpn instance <1-128>",
        "Specify L3VPN configuration\n"
        "Specify a L3VPN instance\n"
        "Number of the L3VPN instance\n")
{
    char *pprompt = NULL;

    pprompt = vty->change_prompt;
    if (NULL != pprompt)
    {
        snprintf(pprompt, VTY_BUFSIZ, "%%s(mpls-l3vpn-%s)# ", argv[0]);
    }

    vty->node = L3VPN_NODE;
    return CMD_SUCCESS;
}

/**
 * \page no_mpls_l3vpn_instance_cmd_vtysh no l3vpn instance <1-128>
 * - 功能说明 \n
 *	 使用该命令删除l3vpn实例
 * - 命令格式 \n
 *	 no l3vpn instance <1-128>
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |<1-128>|L3vpn实例范围|
 *
 * - 缺省情况 \n
 *	 缺省情况下，vpn实例未创�? * - 命令模式 \n
 *	 Mpls视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除一个vpn实例
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   命令行给定vpn实例范围�?1-128>;
 *     实际生效可配置范围如下：HT201/HT201E <1-127>; 
 *     HT2200 <1-128>;
 *	 - 相关命令 \n
 *	   l3vpn instance <1-128> \n
 *	   show l3vpn instance [<1-128>] \n
 * - 使用举例 \n
 *	   Hios(config-mpls)# no l3vpn  instance  1 \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_l3vpn_instance_cmd_vtysh,
        "no l3vpn instance <1-128>",
        NO_STR
        "Specify L3VPN configuration\n"
        "Specify a L3VPN instance\n"
        "Number of the L3VPN instance\n")

/**
 * \page mpls_l3vpn_name_cmd_vtysh name NAME
 * - 功能说明 \n
 *	 配置 l3vpn实例名称
 * - 命令格式 \n
 *	 name NAME
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |NAME|Vpn实例名称|
 *
 * - 缺省情况 \n
 *	 缺省情况下，名称未配�? * - 命令模式 \n
 *	 L3vpn 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置l3vpn实例名称
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   l3vpn instance <1-128> \n
 *	   show l3vpn instance [<1-128>] \n
 * - 使用举例 \n
 *	   Hios(mpls-l3vpn-1)# name instance1 \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_l3vpn_name_cmd_vtysh,
        "name NAME",
        "Name for l3vpn\n"
        "L3vpn name--max.31\n")

/**
 * \page mpls_l3vpn_rd_cmd_vtysh route-distinguisher RD
 * - 功能说明 \n
 *	 配置vpn实例的RD
 * - 命令格式 \n
 *	 route-distinguisher RD
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |RD|vpn实例RD|
 *
 * - 缺省情况 \n
 *	 缺省情况下，RD未配�? * - 命令模式 \n
 *	 L3vpn 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置vpn实例RD
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   l3vpn instance <1-128> \n
 *	   show l3vpn instance [<1-128>] \n
 * - 使用举例 \n
 *	   Hios(mpls-l3vpn-1)# route-distinguisher 100:1 \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_l3vpn_rd_cmd_vtysh,
        "route-distinguisher RD",
        "Configure a Route Distinguisher for the VPN instance\n"
        "Format: X.X.X.X:number<0-65535> or number<0-65535>:number<0-4294967295> "
        "or number<65536-4294967295>:number<0-65535> but not support 0:0\n")

/**
 * \page mpls_l3vpn_target_cmd_vtysh vpn-target TARGET
 * - 功能说明 \n
 *	 配置 vpn实例的target
 * - 命令格式 \n
 *	 vpn-target TARGET
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |TARGET|vpn实例target|
 *
 * - 缺省情况 \n
 *	 缺省情况下，target未配�? * - 命令模式 \n
 *	 L3vpn 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置l3vpn的target
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   l3vpn instance <1-128> \n
 *	   show l3vpn instance [<1-128>] \n
 *     no vpn-target TARGET {both | export-extcommunity | import-extcommunity} \n
 *     no vpn-target TARGET all \n
 * - 使用举例 \n
 *	   Hios(mpls-l3vpn-1)# vpn-target 100:1 \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_l3vpn_target_cmd_vtysh,
        "vpn-target TARGET",
        "Configure VPN targets for the VPN instance\n"
        "Format: X.X.X.X:number<0-65535> or number<0-65535>:number<0-4294967295> "
        "or number<65536-4294967295>:number<0-65535> but not support 0:0\n")

/**
 * \page mpls_l3vpn_target_arg_cmd_vtysh vpn-target TARGET (both|export-extcommunity|import-extcommunity)
 * - 功能说明 \n
 *	 配置 vpn实例的target
 * - 命令格式 \n
 *	 vpn-target TARGET (both | export-extcommunity|import-extcommunity)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |TARGET|vpn实例target|
 *   |both|内部外部都生效|
 *   |export-extcommunity|外部生效|
 *   |import-extcommunity|内部生效|
 *
 * - 缺省情况 \n
 *	 缺省情况下，target未配�? * - 命令模式 \n
 *	 L3vpn 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置l3vpn的target
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   l3vpn instance <1-128> \n
 *	   show l3vpn instance [<1-128>] \n
 *	   no vpn-target TARGET {both|export-extcommunity|import-extcommunity} \n
 *	   no vpn-target TARGET all \n
 * - 使用举例 \n
 *	   Hios(mpls-l3vpn-1)# vpn-target 100:1 both \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_l3vpn_target_arg_cmd_vtysh,
        "vpn-target TARGET (both | export-extcommunity | import-extcommunity)",
        "Configure VPN targets for the VPN instance\n"
        "Format: X.X.X.X:number<0-65535> or number<0-65535>:number<0-4294967295> "
        "or number<65536-4294967295>:number<0-65535> but not support 0:0\n"
        "Use the VPN targets as both export and import targets\n"
        "Use the VPN targets as export targets\n"
        "Use the VPN targets as import targets\n")

/**
 * \page no_mpls_l3vpn_target_cmd_vtysh no vpn-target TARGET
 * - 功能说明 \n
 *	 删除vpn实例中指定的target
 * - 命令格式 \n
 *	 no vpn-target TARGET
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |TARGET|vpn实例target|
 *
 * - 缺省情况 \n
 *	 缺省情况下，target未配�? * - 命令模式 \n
 *	 L3vpn 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除l3vpn的target
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show l3vpn instance [<1-128>] \n
 *	   vpn-target TARGET {both|export-extcommunity|import-extcommunity} \n
 * - 使用举例 \n
 *	   Hios(mpls-l3vpn-1)# no vpn-target 100:1 \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_l3vpn_target_cmd_vtysh,
        "no vpn-target TARGET",
        "Configure VPN targets for the VPN instance\n"
        "Format: X.X.X.X:number<0-65535> or number<0-65535>:number<0-4294967295> "
        "or number<65536-4294967295>:number<0-65535> but not support 0:0\n")

/**
 * \page no_mpls_l3vpn_target_arg_cmd_vtysh no vpn-target TARGET (both|export-extcommunity|import-extcommunity)
 * - 功能说明 \n
 *	 删除vpn实例中指定的target
 * - 命令格式 \n
 *	 no vpn-target TARGET (both|export-extcommunity|import-extcommunity)
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |------|------------|
 *	 |TARGET|vpn实例target|
 *	 |both|内部外部都生效|
 *	 |export-extcommunity|外部生效|
 *	 |import-extcommunity|内部生效|
 *
 * - 缺省情况 \n
 *	 缺省情况下，target未配�? * - 命令模式 \n
 *	 L3vpn 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除vpn实例内指定target
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show l3vpn instance [<1-128>] \n
 *	   vpn-target TARGET {both|export-extcommunity|import-extcommunity} \n
 * - 使用举例 \n
 *	   Hios(mpls-l3vpn-1)# no vpn-target 100:1 both \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_l3vpn_target_arg_cmd_vtysh,
        "no vpn-target TARGET (both | export-extcommunity | import-extcommunity)",
        "Configure VPN targets for the VPN instance\n"
        "Format: X.X.X.X:number<0-65535> or number<0-65535>:number<0-4294967295> "
        "or number<65536-4294967295>:number<0-65535> but not support 0:0\n"
        "Use the VPN targets as both export and import targets\n"
        "Use the VPN targets as export targets\n"
        "Use the VPN targets as import targets\n")

/**
 * \page no_mpls_l3vpn_target_all_cmd_vtysh no vpn-target all
 * - 功能说明 \n
 *	 删除vpn实例下所有target
 * - 命令格式 \n
 *	 no vpn-target all
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，target未配�? * - 命令模式 \n
 *	 L3vpn 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令删除l3vpn实例下所有的target
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   show l3vpn instance [<1-128>] \n
 *	   vpn-target TARGET {both|export-extcommunity|import-extcommunity} \n
 * - 使用举例 \n
 *	   Hios(mpls-l3vpn-1)# no vpn-target all \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_l3vpn_target_all_cmd_vtysh,
        "no vpn-target all",
        "Configure VPN targets for the VPN instance\n"
        "Delete all VPN targets\n")

/*
DEFSH (VTYSH_MPLS, mpls_l3vpn_bind_tunnel_cmd_vtysh,
        "tunnel USP",
        "Configure the tunnel\n"
        CLI_INTERFACE_TUNNEL_VHELP_STR)


DEFSH (VTYSH_MPLS, no_mpls_l3vpn_bind_tunnel_cmd_vtysh,
        "no tunnel",
        NO_STR
        "Configure the tunnel\n")
*/

/**
 * \page mpls_l3vpn_apply_label_cmd_vtysh apply-label per-instance
 * - 功能说明 \n
 *	 配置每个vpn实例分配一个标�? * - 命令格式 \n
 *	 apply-label per-instance
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，每条路由分配一个标�? * - 命令模式 \n
 *	 L3vpn 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令配置每个vpn实例分配一个标�? *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *     l3vpn instance <1-128> \n
 *     no apply-label per-instance \n
 *	   show l3vpn instance [<1-128>] \n
 * - 使用举例 \n
 *	   Hios(mpls-l3vpn-1)# apply-label per-instance \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_l3vpn_apply_label_cmd_vtysh,
        "apply-label per-instance",
        "Label apply configuration\n"
        "The same L3VPN instance use the same label\n")

/**
 * \page no_mpls_l3vpn_apply_label_cmd_vtysh no apply-label per-instance
 * - 功能说明 \n
 *	 将标签分配方式恢复至默认，没条路由一个标�? * - 命令格式 \n
 *	 no apply-label per-instance
 * - 参数说明 \n
 *	 �? * - 缺省情况 \n
 *	 缺省情况下，每条路由分配一个标�? * - 命令模式 \n
 *	 L3vpn 视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令将标签分配方式恢复为默�? *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   l3vpn instance <1-128> \n
 *	   apply-label per-instance \n
 *	   show l3vpn instance [<1-128>] \n
 * - 使用举例 \n
 *	   Hios(mpls-l3vpn-1)# no apply-label per-instance \n
 *
 */
DEFSH (VTYSH_MPLS, no_mpls_l3vpn_apply_label_cmd_vtysh,
        "no apply-label per-instance",
        NO_STR
        "Label apply configuration\n"
        "The same L3VPN instance use the same label\n")

/**
 * \page mpls_l3vpn_show_cmd_vtysh show l3vpn instance [<1-128>]
 * - 功能说明 \n
 *	 显示l3vpn实例内容
 * - 命令格式 \n
 *	 show l3vpn instance [<1-128>]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|----------|
 *	 |<1-128>|可选参数，指定显示的vpn实例号|
 *
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 Config视图、Mpls视图、L3vpn视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示l3vpn实例内容
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   l3vpn instance <1-128> \n
 * - 使用举例 \n
 *	   �? *
 */
DEFSH (VTYSH_MPLS, mpls_l3vpn_show_cmd_vtysh,
        "show l3vpn instance [<1-128>]",
        SHOW_STR
        "Specify L3VPN configuration\n"
        "Specify a L3VPN instance\n"
        "Number of the L3VPN instance\n")

/**
 * \page mpls_l3vpn_show_route_cmd_vtysh show l3vpn route [instance <1-128>]
 * - 功能说明 \n
 *	 显示l3vpn实例下的路由
 * - 命令格式 \n
 *	 show l3vpn route [instance <1-128>]
 * - 参数说明 \n
 *	 |参数  |说明		   |
 *	 |--------|----------|
 *	 |<1-128>|可选参数，指定显示的vpn实例号|
 *
 * - 缺省情况 \n
 *	 �? * - 命令模式 \n
 *	 L3vpn视图
 * - 用户等级 \n
 *	 11
 * - 使用指南 \n
 *	 - 应用场景 \n
 *	   使用该命令显示l3vpn实例下的路由信息
 *	 - 前置条件 \n
 *	   �? *	 - 后续任务 \n
 *	   �? *	 - 注意事项 \n
 *	   �? *	 - 相关命令 \n
 *	   l3vpn instance <1-128> \n
 * - 使用举例 \n
 *	   Hios(mpls-l3vpn-1)# show l3vpn route \n
 *     Hios(mpls-l3vpn-1)# \n
 *
 */
DEFSH (VTYSH_MPLS, mpls_l3vpn_show_route_cmd_vtysh,
        "show l3vpn route [instance <1-128>]",
        SHOW_STR
        "Specify L3VPN configuration\n"
        "Routing information\n"
        "Specify a L3VPN instance\n"
        "Number of the L3VPN instance\n")



/* vpls pw é…ç½®é™æ�?MAC å‘½ä»�?*/
DEFSH(VTYSH_MPLS, vsi_pw_static_mac_cmd_vtysh,
        "vpls mac static XX:XX:XX:XX:XX:XX pw NAME",
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "Mac format XX:XX:XX:XX:XX:XX\n"
        "Configuration static MAC for VPLS PW\n"
        "Special PW name string--max.31\n")


DEFSH(VTYSH_MPLS, no_vsi_pw_static_mac_cmd_vtysh,
        "no vpls mac static XX:XX:XX:XX:XX:XX pw NAME",
        NO_STR
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "Mac format XX:XX:XX:XX:XX:XX\n"
        "Delete static MAC for VPLS PW\n"
        "Specify PW name string--max.31\n")


DEFSH(VTYSH_MPLS, no_vsi_pw_static_mac_all_cmd_vtysh,
        "no vpls mac static all pw NAME",
        NO_STR
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "All static MAC for specify PW\n"
        "Clear the static MAC for VPLS PW\n"
        "Specify PW name string--max.31\n")


DEFSH(VTYSH_MPLS, vsi_interface_ethernet_static_mac_cmd_vtysh,
        "vpls mac static XX:XX:XX:XX:XX:XX interface ethernet USP",
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "Mac format XX:XX:XX:XX:XX:XX\n"
        "Configuration static MAC for VPLS AC\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR)


DEFSH(VTYSH_MPLS, vsi_interface_gigabitethernet_static_mac_cmd_vtysh,
        "vpls mac static XX:XX:XX:XX:XX:XX interface gigabitethernet USP",
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "Mac format XX:XX:XX:XX:XX:XX\n"
        "Configuration static MAC for VPLS AC\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)


DEFSH(VTYSH_MPLS, vsi_interface_xgigabitethernet_static_mac_cmd_vtysh,
        "vpls mac static XX:XX:XX:XX:XX:XX interface xgigabitethernet USP",
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "Mac format XX:XX:XX:XX:XX:XX\n"
        "Configuration static MAC for VPLS AC\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)


DEFSH(VTYSH_MPLS, vsi_interface_trunk_static_mac_cmd_vtysh,
        "vpls mac static XX:XX:XX:XX:XX:XX interface trunk TRUNK",
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "Mac format XX:XX:XX:XX:XX:XX\n"
        "Configuration static MAC for VPLS AC\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR)


DEFSH(VTYSH_MPLS, no_vsi_interface_ethernet_static_mac_cmd_vtysh,
        "no vpls mac static XX:XX:XX:XX:XX:XX interface ethernet USP",
        NO_STR
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "Mac format XX:XX:XX:XX:XX:XX\n"
        "Configuration static MAC for VPLS AC\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR)


DEFSH(VTYSH_MPLS, no_vsi_interface_gigabitethernet_static_mac_cmd_vtysh,
        "no vpls mac static XX:XX:XX:XX:XX:XX interface gigabitethernet USP",
        NO_STR
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "Mac format XX:XX:XX:XX:XX:XX\n"
        "Configuration static MAC for VPLS AC\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)


DEFSH(VTYSH_MPLS, no_vsi_interface_xgigabitethernet_static_mac_cmd_vtysh,
        "no vpls mac static XX:XX:XX:XX:XX:XX interface xgigabitethernet USP",
        NO_STR
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "Mac format XX:XX:XX:XX:XX:XX\n"
        "Configuration static MAC for VPLS AC\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)


DEFSH(VTYSH_MPLS, no_vsi_interface_trunk_static_mac_cmd_vtysh,
        "no vpls mac static XX:XX:XX:XX:XX:XX interface trunk TRUNK",
        NO_STR
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "Mac format XX:XX:XX:XX:XX:XX\n"
        "Configuration static MAC for VPLS AC\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR)


DEFSH(VTYSH_MPLS, no_vsi_interface_ethernet_static_mac_all_cmd_vtysh,
        "no vpls mac static all interface ethernet USP",
        NO_STR
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "All static MAC for specify PW\n"
        "Clear the static MAC for VPLS AC\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR)


DEFSH(VTYSH_MPLS, no_vsi_interface_gigabitethernet_static_mac_all_cmd_vtysh,
        "no vpls mac static all interface gigabitethernet USP",
        NO_STR
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "All static MAC for specify PW\n"
        "Clear the static MAC for VPLS AC\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)


DEFSH(VTYSH_MPLS, no_vsi_interface_xgigabitethernet_static_mac_all_cmd_vtysh,
        "no vpls mac static all interface xgigabitethernet USP",
        NO_STR
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "All static MAC for specify PW\n"
        "Clear the static MAC for VPLS AC\n"
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)


DEFSH(VTYSH_MPLS, no_vsi_interface_trunk_static_mac_all_cmd_vtysh,
        "no vpls mac static all interface trunk TRUNK",
        NO_STR
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "All static MAC for specify PW\n"
        "Clear the static MAC for VPLS AC\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR)


DEFSH(VTYSH_MPLS, no_vsi_static_mac_all_cmd_vtysh,
        "no vpls mac static all vsi <1-1024>",
        NO_STR
        "Virtual Private Lan Service\n"
        "Mac\n"
        "Static mac \n"
        "All static MAC for specify PW\n"
        "Clear the static MAC for VPLS PW\n"
        "VSI instance number <1-1024>\n")


DEFSH (VTYSH_MPLS, show_this_mpls_cmd_vtysh,
        "show this",
        SHOW_STR
        "This\n")


DEFUNSH (VTYSH_MPLS,
		bfd_template_exit_vtysh,
		bfd_template_exit_cmd,
		"exit",
		"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}

DEFUNSH (VTYSH_MPLS,
		bfd_template_config,
		bfd_template_config_cmd_vtysh,
		"bfd template NAME",
		"Enter the BFD template view\n"
		"The name for bfd template\n")
{
	char *pprompt = NULL;

	pprompt = vty->change_prompt;
	if (NULL != pprompt)
	{
		snprintf(pprompt, VTY_BUFSIZ, "%%s(config-bfd-template-%s)# ", argv[0]);
	}

	vty->node = BFD_TEMPLATE_NODE;
	return CMD_SUCCESS;
}

DEFSH (VTYSH_MPLS,
	bfd_ip_router_alert_enable_cmd_vtysh,
	"bfd ip-router-alert ",
	BFD_STR
	"Enable BFD ip-router-alert function\n")

DEFSH (VTYSH_MPLS,
	undo_bfd_ip_router_alert_enable_cmd_vtysh,
	"undo bfd ip-router-alert ",
	BFD_STR
	"Disable BFD ip-router-alert function\n")

DEFSH(VTYSH_MPLS,
	mpls_bfd_enable_cmd_vtysh,
	"mpls bfd enable ",
	BFD_STR
	"Enable MPLS BFD function\n")

DEFSH (VTYSH_MPLS,
	undo_mpls_bfd_enable_cmd_vtysh,
	"undo mpls bfd enable ",
	BFD_STR
	"Disable MPLS BFD function\n")

DEFSH (VTYSH_MPLS,
	undo_bfd_template_config_cmd_vtysh,
	"undo bfd template NAME",
	BFD_STR
	"Delete BFD template\n"
	"Template name\n")

DEFSH (VTYSH_MPLS,
	bfd_detect_multiplier_cmd_vtysh,
	"bfd detect-multiplier <3-50>",
	BFD_STR
	"BFD detect-multiplier function\n"
	"detect_mult <3-50>, default 5\n")
		
DEFSH (VTYSH_MPLS,
	undo_bfd_detect_multiplier_cmd_vtysh,
	"undo bfd detect-multiplier",
	BFD_STR
	"Restore BFD detect-multiplier function\n")

DEFSH (VTYSH_MPLS,
	bfd_min_receive_interval_cmd_vtysh,
	"bfd min-receive-interval <3-10000>",
	BFD_STR
	"Bfd min-receive-interval function\n"
	"interval <3-10000> ms,default 1000ms\n")

DEFSH (VTYSH_MPLS,
	undo_bfd_min_receive_interval_cmd_vtysh,
	"undo bfd min-receive-interval",
	BFD_STR
	"Restore Bfd min-receive-interval function\n")
		
DEFSH (VTYSH_MPLS,
	bfd_min_transmit_interval_cmd_vtysh,
	"bfd min-transmit-interval <3-10000>",
	BFD_STR
	"Bfd min-transmit-interval function\n"
	"interval <3-10000> ms,default 1000ms\n")

DEFSH (VTYSH_MPLS,
	undo_bfd_min_transmit_interval_cmd_vtysh,
	"undo bfd min-transmit-interval",
	BFD_STR
	"Restore Bfd min-transmit-interval function\n")

DEFSH (VTYSH_MPLS,
	bfd_multi_hop_authentication_mode_cmd_vtysh,
	"bfd multi-hop authentication-mode (m-md5 | m-sha1 | md5 | sha1 | simple)",
	BFD_STR
	"BFD multi-hop authentication-mode function\n")

DEFSH (VTYSH_MPLS,
	undo_bfd_multi_hop_authentication_mode_cmd_vtysh,
	"undo bfd multi-hop authentication-mode",
	BFD_STR
	"BFD multi-hop authentication-mode function\n")

DEFSH (VTYSH_MPLS,
	bfd_multi_hop_dst_port_cmd_vtysh,
	"bfd multi-hop destination-port (3784 | 4784)",
	BFD_STR
	"BFD multi-hop dest-port function\n"
	"port(3784 | 4784),default 4784\n")

DEFSH (VTYSH_MPLS,
	undo_bfd_multi_hop_dst_port_cmd_vtysh,
	"undo bfd multi-hop destination-port",
	BFD_STR
	"Restore BFD multi-hop dest-port function\n")

DEFSH (VTYSH_MPLS,
	bfd_multi_hop_multiplier_cmd_vtysh,
	"bfd multi-hop detect-multiplier <3-50>",
	BFD_STR
	"BFD multi-hop detect-multiplier function\n"
	"detect_mult <3-50>, default 5\n")

DEFSH (VTYSH_MPLS,
	undo_bfd_multi_hop_multiplier_cmd_vtysh,
	"undo_bfd multi-hop detect-multiplier",
	BFD_STR
	"Restore BFD multi-hop detect-multiplier function\n")

DEFSH (VTYSH_MPLS,
	bfd_multi_hop_min_receive_interval_cmd_vtysh,
	"bfd multi-hop min-receive-interval <3-10000>",
	BFD_STR
	"BFD multi-hop min-receive-interval function\n"
	"interval <3-10000> ms, default 1000ms\n")

DEFSH (VTYSH_MPLS,
	undo_bfd_multi_hop_min_receive_interval_cmd_vtysh,
	"undo bfd multi-hop min-receive-interval",
	BFD_STR
	"Restore BFD multi-hop min-receive-interval function\n")

DEFSH (VTYSH_MPLS,
	bfd_multi_hop_min_transmit_interval_cmd_vtysh,
	"bfd multi-hop min-transmit-interval <3-10000>",
	BFD_STR
	"BFD multi-hop min-transmit-interval function\n"
	"interval <3-10000> ms, default 1000ms\n")

DEFSH (VTYSH_MPLS,
	undo_bfd_multi_hop_min_transmit_interval_cmd_vtysh,
	"undo bfd multi-hop min-transmit-interval",
	BFD_STR
	"Restore BFD multi-hop min-transmit-interval function\n")

DEFSH (VTYSH_MPLS,
	bfd_session_init_mode_cmd_vtysh,
	"bfd session init-mode (active | passive)",
	BFD_STR
	"BFD session init-mode function\n"
	"(active | passive), default mode active\n")

DEFSH (VTYSH_MPLS,
	undo_bfd_session_init_mode_cmd_vtysh,
	"undo_bfd session init-mode",
	BFD_STR
	"Restore BFD session init-mode default function\n")

DEFSH (VTYSH_MPLS,
    mpls_service_id_cmd_vtysh1,
    "mpls-service service-id <1-4294967295> service-name NAME service-type (e-line|e-lan|e-tree) service-vpn vc-id <1-4294967295> root-port trunk TRUNK",

    "Configure MPLS Service\n"
    "Configure Service Id\n"
    "the value of service id\n"
    "Configure Service Name\n"
    "Service Name\n"
    "Configure Service Type\n"
    "E-Line Service\n"
    "E-LAN Service\n"
    "E-TREE Service\n"
    "Configure Service VPN \n"
    "Configure VC ID\n"
    "the value of VC ID\n"
    "Configure root port\n"
    "Trunk interface\n"
    "The port/subport of trunk, format: <1-128>[.<1-4095>]\n")

DEFSH (VTYSH_MPLS,
    mpls_service_id_cmd_vtysh2,
    "mpls-service service-id <1-4294967295> service-name NAME service-type (e-line|e-lan|e-tree) service-vpn vc-id <1-4294967295> root-port (ethernet | gigabitethernet | xgigabitethernet | tdm) USP",

    "Configure MPLS Service\n"
    "Configure Service Id\n"
    "the value of service id \n"
    "Configure Service Name\n"
    "Service Name\n"
    "Configure Service Type\n"
    "E-Line Service\n"
    "E-LAN Service\n"
    "E-TREE Service\n"
    "Configure Service VPN \n"
    "Configure VC ID\n"
    "the value of VC ID\n"
    "Configure root port\n"
    "Ethernet interface\n"
    "Gigabit Ethernet interface\n"
    "10Gigabit Ethernet interface\n"
    "TDM interface\n"
    "The port/subport of gigabit ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")
    
DEFSH (VTYSH_MPLS,
    mpls_service_id_cmd_vtysh3,
    "mpls-service service-id <1-4294967295> service-name NAME service-type (e-line|e-lan|e-tree) service-vpn vsi <1-1024> "
    "root-port trunk TRUNK",

    "Configure MPLS Service\n"
    "Configure Service Id\n"
    "the value of service id \n"
    "Configure Service Name\n"
    "Service Name\n"
    "Configure Service Type\n"
    "E-Line Service\n"
    "E-LAN Service\n"
    "E-TREE Service\n"
    "Configure Service VPN \n"
    "Configure VPLS(VSI) ID\n"
    "the value of VSI ID\n"
    "Configure root port\n"
    "Trunk interface\n"
    "The port/subport of trunk, format: <1-128>[.<1-4095>]\n")

DEFSH (VTYSH_MPLS,
    mpls_service_id_cmd_vtysh4,
    "mpls-service service-id <1-4294967295> service-name NAME service-type (e-line|e-lan|e-tree) service-vpn vsi <1-1024> "
    "root-port (ethernet | gigabitethernet | xgigabitethernet | tdm) USP",

    "Configure MPLS Service\n"
    "Configure Service Id\n"
    "the value of service id \n"
    "Configure Service Name\n"
    "Service Name\n"
    "Configure Service Type\n"
    "E-Line Service\n"
    "E-LAN Service\n"
    "E-TREE Service\n"
    "Configure Service VPN \n"
    "Configure VPLS(VSI) ID\n"
    "the value of VSI ID\n"
    "Configure root port\n"
    "Ethernet interface\n"
    "Gigabit Ethernet interface\n"
    "10Gigabit Ethernet interface\n"
    "TDM interface\n"
    "The port/subport of gigabit ethernet, format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")

DEFSH (VTYSH_MPLS,
    mpls_service_id_cmd_vtysh5,
    "mpls-service service-id <1-4294967295> service-name NAME service-type (e-line|e-lan|e-tree) service-vpn vsi <1-1024> "
    "root-port pw-name NAME",
    
    "Configure MPLS Service\n"
    "Configure Service Id\n"
    "the value of service id \n"
    "Configure Service Name\n"
    "Service Name\n"
    "Configure Service Type\n"
    "E-Line Service\n"
    "E-LAN Service\n"
    "E-TREE Service\n"
    "Configure Service VPN \n"
    "Configure VPLS(VSI) ID\n"
    "the value of VSI ID\n"
    "Configure root port\n"
    "PW name\n"
    "PW name string\n")

DEFSH (VTYSH_MPLS,
    mpls_service_id_cmd_vtysh6,
    "mpls-service service-id <1-4294967295> service-name NAME service-type (e-line|e-lan|e-tree) service-vpn vc-id <1-4294967295> root-port pw-name NAME",

    "Configure MPLS Service\n"
    "Configure Service Id\n"
    "the value of service id\n"
    "Configure Service Name\n"
    "Service Name\n"
    "Configure Service Type\n"
    "E-Line Service\n"
    "E-LAN Service\n"
    "E-TREE Service\n"
    "Configure Service VPN \n"
    "Configure VC ID\n"
    "the value of VC ID\n"
    "Configure root port\n"
    "PW name\n"
    "PW name string\n")


DEFSH (VTYSH_MPLS,
    no_mpls_serive_id_cmd_vtysh,
    "no mpls-service service-id ID",
    NO_STR
    "Configure MPLS Service\n"
    "Configure Service Id\n"
    "the value of service id \n")

DEFSH (VTYSH_MPLS,
    show_mpls_service_cmd_vtysh,
    "show mpls-service",
    "show configuration\n"
    "show mpls service configuration")

DEFSH (VTYSH_MPLS,
    mpls_pw_sample_interval_cmd_vtysh,
	"sample interval <5-20>",
    "Specify pw bandwidth sample interval\n"
    "Specify pw bandwidth sample interval\n"
    "Value of interval, the default 5s\n")

DEFSH (VTYSH_MPLS,
	mpls_debug_monitor_cmd_vtysh,
    "debug mpls (enable|disable) (bfd|oam|lsp|l2vc|vpls|tunnel|l3vpn|ldp|ping)",
    "Debug information to moniter\n"
    "Programe name mpls\n"
    "Enable debug\n"
    "Disable debug\n"
    "Debug bfd informations\n"
    "Debug oam informations\n"
    "Debug lsp informations\n"
    "Debug l2vc informations\n"
    "Debug vpls informations\n"
    "Debug tunnel informations\n"
    "Debug l3vpn informations\n"
    "Debug ldp informations\n"
    "Debug ping informations\n")


DEFSH (VTYSH_MPLS,
	show_mpls_debug_monitor_cmd_vtysh,
	"show mpls debug",
	SHOW_STR
	"Mpls"
	"Debug status\n")
		
void vtysh_init_mpls_cmd(void)
{
#define MPLS_INSTALL_ELEMENT_CONFIG_NODE(cmd, flag) \
    install_element_level (CONFIG_NODE, (cmd), VISIT_LEVE, (flag));

#define MPLS_INSTALL_ELEMENT_CONFIG_CONFIG(cmd, flag) \
    install_element_level (CONFIG_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_CONFIG_SHOW(cmd, flag) \
    install_element_level (CONFIG_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_CONFIG_DEBUG(cmd, flag) \
    install_element_level (CONFIG_NODE, (cmd), MANAGE_LEVE, (flag));

#define MPLS_INSTALL_ELEMENT_MPLS_NODE(cmd, flag) \
    install_element_level (MPLS_NODE, (cmd), VISIT_LEVE, (flag));

#define MPLS_INSTALL_ELEMENT_MPLS_CONFIG(cmd, flag) \
    install_element_level (MPLS_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_MPLS_SHOW(cmd, flag) \
    install_element_level (MPLS_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_PW_NODE(cmd, flag) \
    install_element_level (PW_NODE, (cmd), VISIT_LEVE, (flag));

#define MPLS_INSTALL_ELEMENT_PW_CONFIG(cmd, flag) \
    install_element_level (PW_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_PW_SHOW(cmd, flag) \
    install_element_level (PW_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_VSI_NODE(cmd, flag) \
    install_element_level (VSI_NODE, (cmd), VISIT_LEVE, (flag));

#define MPLS_INSTALL_ELEMENT_VSI_CONFIG(cmd, flag) \
    install_element_level (VSI_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_VSI_SHOW(cmd, flag) \
    install_element_level (VSI_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_L3VPN_NODE(cmd, flag) \
    install_element_level (L3VPN_NODE, (cmd), VISIT_LEVE, (flag));

#define MPLS_INSTALL_ELEMENT_L3VPN_CONFIG(cmd, flag) \
    install_element_level (L3VPN_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_L3VPN_SHOW(cmd, flag) \
    install_element_level (L3VPN_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_MAC_NODE(cmd, flag) \
    install_element_level (MAC_NODE, (cmd), VISIT_LEVE, (flag));

#define MPLS_INSTALL_ELEMENT_MAC_CONFIG(cmd, flag) \
    install_element_level (MAC_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_PHY_IF_CONFIG(cmd, flag) \
    install_element_level (PHYSICAL_IF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_PHY_IF_SHOW(cmd, flag) \
    install_element_level (PHYSICAL_IF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(cmd, flag) \
    install_element_level (PHYSICAL_SUBIF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_PHY_SUBIF_SHOW(cmd, flag) \
    install_element_level (PHYSICAL_SUBIF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_LOOP_IF_CONFIG(cmd, flag) \
    install_element_level (LOOPBACK_IF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_LOOP_IF_SHOW(cmd, flag) \
    install_element_level (LOOPBACK_IF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(cmd, flag) \
    install_element_level (TUNNEL_IF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_TNL_IF_ORDER(cmd, flag) \
    install_element_daemon_order_level (TUNNEL_IF_NODE, (cmd), CONFIG_LEVE_5, (flag), 2, VTYSH_MPLS, VTYSH_IFM);

#define MPLS_INSTALL_ELEMENT_TNL_IF_SHOW(cmd, flag) \
    install_element_level (TUNNEL_IF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_TRK_IF_CONFIG(cmd, flag) \
    install_element_level (TRUNK_IF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_TRK_IF_SHOW(cmd, flag) \
    install_element_level (TRUNK_IF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(cmd, flag) \
    install_element_level (TRUNK_SUBIF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_TRK_SUBIF_SHOW(cmd, flag) \
    install_element_level (TRUNK_SUBIF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_VLAN_IF_CONFIG(cmd, flag) \
    install_element_level (VLANIF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_VLAN_IF_SHOW(cmd, flag) \
    install_element_level (VLANIF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_TDM_IF_CONFIG(cmd, flag) \
    install_element_level (TDM_IF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_TDM_IF_SHOW(cmd, flag) \
    install_element_level (TDM_IF_NODE, (cmd), MONITOR_LEVE_2, (flag));

#define MPLS_INSTALL_ELEMENT_TDM_SUBIF_CONFIG(cmd, flag) \
    install_element_level (TDM_SUBIF_NODE, (cmd), CONFIG_LEVE_5, (flag));

#define MPLS_INSTALL_ELEMENT_TDM_SUBIF_SHOW(cmd, flag) \
    install_element_level (TDM_SUBIF_NODE, (cmd), MONITOR_LEVE_2, (flag));

    /* mpls è¿›ç¨‹èŠ‚ç‚¹å®‰è£�?*/
    install_node (&mpls_node, NULL);
    vtysh_install_default (MPLS_NODE);

    install_node (&vsi_node, NULL);
    vtysh_install_default (VSI_NODE);

    install_node (&l3vpn_node, NULL);
    vtysh_install_default (L3VPN_NODE);

    install_node (&pw_node, NULL);
    vtysh_install_default (PW_NODE);

    install_node(&mplstp_oam_sess_node, NULL);
    vtysh_install_default(MPLSTP_OAM_SESS_NODE);
    install_node(&mplsaps_session_node, NULL);
    vtysh_install_default(APS_NODE);

	install_node(&mplstp_oam_meg_node, NULL);
	vtysh_install_default(MPLSTP_OAM_MEG_NODE);

	install_element_level(CONFIG_NODE, &mpls_debug_monitor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_mpls_debug_monitor_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	
    /* èŠ‚ç‚¹é€€å‡ºå‡½æ•°æ³¨å�?*/
    MPLS_INSTALL_ELEMENT_MPLS_NODE(&vtysh_exit_mpls_cmd, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_NODE(&vtysh_exit_mpls_vsi_cmd, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_L3VPN_NODE(&vtysh_exit_mpls_l3vpn_cmd, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_NODE(&vtysh_exit_mpls_pw_cmd, CMD_SYNC);

    install_element_level (MPLSTP_OAM_SESS_NODE, &vtysh_exit_mplsoam_session_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (APS_NODE, &vtysh_exit_mpls_aps_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level (MPLSTP_OAM_MEG_NODE, &vtysh_exit_mplsoam_meg_cmd, VISIT_LEVE, CMD_SYNC);


    /* mpls å…¨å±€å‘½ä»¤æ³¨å†�?*/
    MPLS_INSTALL_ELEMENT_CONFIG_NODE(&mpls_cmd, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&mpls_lsrid_cmd_vtysh, CMD_SYNC);

    /* show this å‘½ä»¤æ³¨å†�?*/
    MPLS_INSTALL_ELEMENT_MPLS_SHOW(&show_this_mpls_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PW_SHOW(&show_this_mpls_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_VSI_SHOW(&show_this_mpls_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_L3VPN_SHOW(&show_this_mpls_cmd_vtysh, CMD_LOCAL);

    /* é™æ€?lsp å‘½ä»¤æ³¨å†�?*/
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&ingress_static_lsp_nexthop_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&ingress_static_lsp_ethernet_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&ingress_static_lsp_gigabit_ethernet_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&ingress_static_lsp_xgigabit_ethernet_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&ingress_static_lsp_trunk_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&transit_static_lsp_nexthop_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&transit_static_lsp_ethernet_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&transit_static_lsp_gigabit_ethernet_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&transit_static_lsp_xgigabit_ethernet_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&transit_static_lsp_trunk_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&egress_static_lsp_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&no_static_lsp_cmd_vtysh, CMD_SYNC);

    MPLS_INSTALL_ELEMENT_MPLS_SHOW(&show_static_lsp_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_MPLS_SHOW(&show_label_pool_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_CONFIG_SHOW(&show_static_lsp_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_CONFIG_SHOW(&show_label_pool_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_PHY_IF_SHOW(&show_static_lsp_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_IF_SHOW(&show_label_pool_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_PHY_SUBIF_SHOW(&show_static_lsp_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_SUBIF_SHOW(&show_label_pool_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_TRK_IF_SHOW(&show_static_lsp_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_IF_SHOW(&show_label_pool_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_TRK_SUBIF_SHOW(&show_static_lsp_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_SUBIF_SHOW(&show_label_pool_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_TNL_IF_SHOW(&show_static_lsp_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TNL_IF_SHOW(&show_label_pool_cmd_vtysh, CMD_LOCAL);

    /* tunnel å‘½ä»¤æ³¨å†�?*/
    MPLS_INSTALL_ELEMENT_CONFIG_SHOW(&show_tunnel_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_protocol_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_source_destination_ip_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_nexthop_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&no_tunnel_nexthop_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_outif_ethernet_dmac_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_outif_gigabit_ethernet_dmac_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_outif_xgigabit_ethernet_dmac_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_outif_trunk_dmac_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_static_lsp_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_failback_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_non_failback_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_statistics_clear_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_mplstp_oam_session_enable_cmd_vtysh, CMD_SYNC);
 //   MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_mplstp_oam_session_enable_lsp_cmd_vtysh, CMD_SYNC);
   // MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_bfd_session_enable_tunnel_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&tunnel_bfd_session_enable_lsp_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&mpls_qos_car_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&mpls_tunnel_qos_car_l1_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&mpls_hqos_queue_apply_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_ORDER(&no_tunnel_protocol_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&no_tunnel_source_destination_ip_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&no_tunnel_outif_dmac_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&no_tunnel_static_lsp_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&no_tunnel_mplstp_oam_session_enable_cmd_vtysh, CMD_SYNC);
 //   MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&no_tunnel_mplstp_oam_session_enable_lsp_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&no_tunnel_bfd_session_enable_tunnel_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&no_tunnel_bfd_session_enable_lsp_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&no_mpls_qos_car_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_CONFIG(&no_mpls_hqos_queue_apply_cmd_vtysh, CMD_SYNC);

    MPLS_INSTALL_ELEMENT_TNL_IF_ORDER(&mpls_exp_domain_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_ORDER(&mpls_exp_phb_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_ORDER(&no_mpls_exp_domain_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_ORDER(&no_mpls_exp_phb_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_ORDER(&tunnel_statistics_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_ORDER(&no_tunnel_statistics_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TNL_IF_SHOW(&show_tunnel_statistics_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TNL_IF_SHOW(&show_tunnel_cmd_vtysh, CMD_LOCAL);

    /* pw å‘½ä»¤æ³¨å†�?*/
    MPLS_INSTALL_ELEMENT_CONFIG_SHOW(&show_mpls_pw_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_MPLS_NODE(&mpls_pw_name_cmd, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&no_mpls_pw_name_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_SHOW(&show_mpls_pw_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_protocol_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_vc_type_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_peer_vcid_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_label_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_mtu_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_ctrl_word_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_encapsulate_ethernet_raw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_encapsulate_ethernet_tag_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_encapsulate_vlan_raw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_encapsulate_vlan_tag_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_ces_frame_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_bind_tunnel_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_bind_lsp_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_tunnel_policy_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_switch_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_switch_vsi_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_switch_vsi_backup_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_switch_vsi_master_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_statistics_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_statistics_clear_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_mplstp_oam_session_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_bfd_session_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_bfd_dynamic_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_exp_domain_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_exp_phb_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_qos_car_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_qos_car_l1_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_hqos_queue_apply_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_protocol_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_vc_type_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_peer_vcid_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_label_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_mtu_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_encapsulate_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_ces_frame_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_tunnel_bind_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_tunnel_policy_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_switch_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_switch_vsi_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_switch_vsi_backup_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_statistics_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_mplstp_oam_session_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_bfd_session_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_exp_domain_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_pw_exp_phb_enable_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_qos_car_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&no_mpls_hqos_queue_apply_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_CONFIG(&mpls_pw_sample_interval_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PW_SHOW(&show_mpls_pw_statistics_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PW_SHOW(&show_mpls_pw_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_PHY_IF_SHOW(&show_mpls_pw_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_IF_CONFIG(&mpls_if_bind_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PHY_IF_CONFIG(&no_mpls_if_bind_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PHY_IF_CONFIG(&mpls_interface_pw_failback_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PHY_IF_CONFIG(&mpls_interface_pw_non_failback_cmd_vtysh, CMD_SYNC);

    MPLS_INSTALL_ELEMENT_PHY_SUBIF_SHOW(&show_mpls_pw_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&mpls_if_bind_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&no_mpls_if_bind_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&mpls_interface_pw_failback_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&mpls_interface_pw_non_failback_cmd_vtysh, CMD_SYNC);

    MPLS_INSTALL_ELEMENT_TRK_IF_SHOW(&show_mpls_pw_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_IF_CONFIG(&mpls_if_bind_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TRK_IF_CONFIG(&no_mpls_if_bind_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TRK_IF_CONFIG(&mpls_interface_pw_failback_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TRK_IF_CONFIG(&mpls_interface_pw_non_failback_cmd_vtysh, CMD_SYNC);

    MPLS_INSTALL_ELEMENT_TRK_SUBIF_SHOW(&show_mpls_pw_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&mpls_if_bind_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&no_mpls_if_bind_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&mpls_interface_pw_failback_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&mpls_interface_pw_non_failback_cmd_vtysh, CMD_SYNC);

    MPLS_INSTALL_ELEMENT_TDM_IF_SHOW(&show_mpls_pw_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TDM_IF_CONFIG(&mpls_if_bind_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TDM_IF_CONFIG(&no_mpls_if_bind_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TDM_IF_CONFIG(&mpls_interface_pw_failback_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TDM_IF_CONFIG(&mpls_interface_pw_non_failback_cmd_vtysh, CMD_SYNC);

    MPLS_INSTALL_ELEMENT_TDM_SUBIF_SHOW(&show_mpls_pw_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TDM_SUBIF_CONFIG(&mpls_if_bind_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TDM_SUBIF_CONFIG(&no_mpls_if_bind_pw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TDM_SUBIF_CONFIG(&mpls_interface_pw_failback_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TDM_SUBIF_CONFIG(&mpls_interface_pw_non_failback_cmd_vtysh, CMD_SYNC);

    /* vpls å‘½ä»¤æ³¨å†�?*/
    MPLS_INSTALL_ELEMENT_CONFIG_SHOW(&show_vsi_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_MPLS_NODE(&mpls_vsi_cmd, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&no_mpls_vsi_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_SHOW(&show_vsi_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&mpls_vsi_name_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&mpls_vsi_pw_failback_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&mpls_vsi_pw_non_failback_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&vsi_mac_learn_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&vsi_mac_learn_limit_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&no_vsi_mac_learn_limit_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&vsi_mac_blacklist_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&no_vsi_mac_blacklist_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&no_vsi_mac_blacklist_all_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&vsi_mac_whitelist_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&no_vsi_mac_whitelist_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&no_vsi_mac_whitelist_all_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&mpls_vsi_encapsulate_ethernet_raw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&mpls_vsi_encapsulate_ethernet_tag_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&mpls_vsi_encapsulate_vlan_raw_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&mpls_vsi_encapsulate_vlan_tag_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&no_mpls_vsi_encapsulate_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_VSI_SHOW(&show_vsi_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&show_vsi_mac_blacklist_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_VSI_CONFIG(&show_vsi_mac_whitelist_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_PHY_IF_SHOW(&show_vsi_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_IF_CONFIG(&mpls_if_bind_vsi_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PHY_IF_CONFIG(&mpls_if_unbind_vsi_cmd_vtysh, CMD_SYNC);

    MPLS_INSTALL_ELEMENT_PHY_SUBIF_SHOW(&show_vsi_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&mpls_if_bind_vsi_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_PHY_SUBIF_CONFIG(&mpls_if_unbind_vsi_cmd_vtysh, CMD_SYNC);

    MPLS_INSTALL_ELEMENT_TRK_IF_SHOW(&show_vsi_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_IF_CONFIG(&mpls_if_bind_vsi_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TRK_IF_CONFIG(&mpls_if_unbind_vsi_cmd_vtysh, CMD_SYNC);

    MPLS_INSTALL_ELEMENT_TRK_SUBIF_SHOW(&show_vsi_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&mpls_if_bind_vsi_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_TRK_SUBIF_CONFIG(&mpls_if_unbind_vsi_cmd_vtysh, CMD_SYNC);

    /* vpls pw é™æ€?mac å‘½ä»¤æ³¨å†�?*/
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&vsi_pw_static_mac_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&no_vsi_pw_static_mac_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&no_vsi_pw_static_mac_all_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&vsi_interface_ethernet_static_mac_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&vsi_interface_gigabitethernet_static_mac_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&vsi_interface_xgigabitethernet_static_mac_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&vsi_interface_trunk_static_mac_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&no_vsi_interface_ethernet_static_mac_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&no_vsi_interface_gigabitethernet_static_mac_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&no_vsi_interface_xgigabitethernet_static_mac_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&no_vsi_interface_trunk_static_mac_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&no_vsi_interface_ethernet_static_mac_all_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&no_vsi_interface_gigabitethernet_static_mac_all_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&no_vsi_interface_xgigabitethernet_static_mac_all_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&no_vsi_interface_trunk_static_mac_all_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MAC_CONFIG(&no_vsi_static_mac_all_cmd_vtysh, CMD_SYNC);

    /* l3vpn å‘½ä»¤æ³¨å†�?*/
    MPLS_INSTALL_ELEMENT_CONFIG_SHOW(&mpls_l3vpn_show_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_CONFIG_SHOW(&mpls_l3vpn_show_route_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_MPLS_NODE(&mpls_l3vpn_instance_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_CONFIG(&no_mpls_l3vpn_instance_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_MPLS_SHOW(&mpls_l3vpn_show_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_MPLS_SHOW(&mpls_l3vpn_show_route_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_L3VPN_CONFIG(&mpls_l3vpn_rd_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_L3VPN_CONFIG(&mpls_l3vpn_name_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_L3VPN_CONFIG(&mpls_l3vpn_target_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_L3VPN_CONFIG(&mpls_l3vpn_target_arg_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_L3VPN_CONFIG(&mpls_l3vpn_apply_label_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_L3VPN_CONFIG(&no_mpls_l3vpn_target_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_L3VPN_CONFIG(&no_mpls_l3vpn_target_arg_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_L3VPN_CONFIG(&no_mpls_l3vpn_target_all_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_L3VPN_CONFIG(&no_mpls_l3vpn_apply_label_cmd_vtysh, CMD_SYNC);
    MPLS_INSTALL_ELEMENT_L3VPN_SHOW(&mpls_l3vpn_show_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_L3VPN_SHOW(&mpls_l3vpn_show_route_cmd_vtysh, CMD_LOCAL);

    /* mplsif å‘½ä»¤æ³¨å†�?*/
    MPLS_INSTALL_ELEMENT_CONFIG_SHOW(&show_mpls_interface_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_CONFIG_SHOW(&show_mpls_interface_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_CONFIG_SHOW(&show_mpls_interface_gigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_CONFIG_SHOW(&show_mpls_interface_xgigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_CONFIG_SHOW(&show_mpls_interface_tdm_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_PHY_IF_SHOW(&show_mpls_interface_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_IF_SHOW(&show_mpls_interface_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_IF_SHOW(&show_mpls_interface_gigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_IF_SHOW(&show_mpls_interface_xgigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_IF_SHOW(&show_mpls_interface_tdm_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_PHY_SUBIF_SHOW(&show_mpls_interface_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_SUBIF_SHOW(&show_mpls_interface_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_SUBIF_SHOW(&show_mpls_interface_gigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_SUBIF_SHOW(&show_mpls_interface_xgigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_PHY_SUBIF_SHOW(&show_mpls_interface_tdm_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_TRK_IF_SHOW(&show_mpls_interface_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_IF_SHOW(&show_mpls_interface_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_IF_SHOW(&show_mpls_interface_gigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_IF_SHOW(&show_mpls_interface_xgigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_IF_SHOW(&show_mpls_interface_tdm_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_TRK_SUBIF_SHOW(&show_mpls_interface_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_SUBIF_SHOW(&show_mpls_interface_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_SUBIF_SHOW(&show_mpls_interface_gigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_SUBIF_SHOW(&show_mpls_interface_xgigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TRK_SUBIF_SHOW(&show_mpls_interface_tdm_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_TDM_IF_SHOW(&show_mpls_interface_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TDM_IF_SHOW(&show_mpls_interface_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TDM_IF_SHOW(&show_mpls_interface_gigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TDM_IF_SHOW(&show_mpls_interface_xgigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TDM_IF_SHOW(&show_mpls_interface_tdm_cmd_vtysh, CMD_LOCAL);

    MPLS_INSTALL_ELEMENT_TDM_SUBIF_SHOW(&show_mpls_interface_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TDM_SUBIF_SHOW(&show_mpls_interface_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TDM_SUBIF_SHOW(&show_mpls_interface_gigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TDM_SUBIF_SHOW(&show_mpls_interface_xgigabit_ethernet_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_TDM_SUBIF_SHOW(&show_mpls_interface_tdm_cmd_vtysh, CMD_LOCAL);

    /* mpls log å¼€å…³æ³¨å�?*/
    MPLS_INSTALL_ELEMENT_CONFIG_DEBUG(&mpls_log_level_ctl_cmd_vtysh, CMD_LOCAL);
    MPLS_INSTALL_ELEMENT_CONFIG_DEBUG(&mpls_show_log_level_ctl_cmd_vtysh, CMD_LOCAL);

	/*bfd h3c command*/
    install_node (&bfd_template_node, NULL);
    vtysh_install_default (BFD_TEMPLATE_NODE);
    install_element_level (BFD_TEMPLATE_NODE, &bfd_template_exit_cmd, VISIT_LEVE, CMD_SYNC);

	install_element_level(CONFIG_NODE, &bfd_ip_router_alert_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &undo_bfd_ip_router_alert_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &mpls_bfd_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &undo_mpls_bfd_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &bfd_template_config_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &undo_bfd_template_config_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &bfd_multi_hop_authentication_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &undo_bfd_multi_hop_authentication_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &bfd_multi_hop_dst_port_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &undo_bfd_multi_hop_dst_port_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &bfd_multi_hop_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &undo_bfd_multi_hop_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &bfd_multi_hop_min_receive_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &undo_bfd_multi_hop_min_receive_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &bfd_multi_hop_min_transmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &undo_bfd_multi_hop_min_transmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &bfd_session_init_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &undo_bfd_session_init_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(BFD_TEMPLATE_NODE, &bfd_detect_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(BFD_TEMPLATE_NODE, &undo_bfd_detect_multiplier_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(BFD_TEMPLATE_NODE, &bfd_min_receive_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(BFD_TEMPLATE_NODE, &undo_bfd_min_receive_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(BFD_TEMPLATE_NODE, &bfd_min_transmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(BFD_TEMPLATE_NODE, &undo_bfd_min_transmit_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /* mpls service */
    install_element_level(MPLS_NODE, &mpls_service_id_cmd_vtysh5, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &mpls_service_id_cmd_vtysh6, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &mpls_service_id_cmd_vtysh1, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &mpls_service_id_cmd_vtysh2, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &mpls_service_id_cmd_vtysh3, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &mpls_service_id_cmd_vtysh4, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(MPLS_NODE, &no_mpls_serive_id_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &show_mpls_service_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_mpls_service_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

    
    /* bfd view default command*/
    install_node (&bfd_session_node, NULL);
    vtysh_install_default (BFD_SESSION_NODE);

    install_element_level (BFD_SESSION_NODE, &bfd_session_exit_cmd, VISIT_LEVE, CMD_SYNC);

    //install_element(CONFIG_NODE, &enable_bfd_cmd);
    //install_element(CONFIG_NODE, &disable_bfd_cmd);

    /*[no] debug bfd {packet | fsm }*/
    install_element_level(CONFIG_NODE, &bfd_debug_cmd, MANAGE_LEVE, CMD_SYNC);
    install_element_level(CONFIG_NODE, &undo_bfd_debug_cmd, MANAGE_LEVE, CMD_SYNC);

	install_element_level(CONFIG_NODE, &bfd_up_down_clear_cmd, MANAGE_LEVE, CMD_SYNC);

    /*bfd priority <0-7>*/
    install_element_level(CONFIG_NODE, &bfd_priority_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &undo_bfd_priority_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &bfd_ttl_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &undo_bfd_ttl_cmd, CONFIG_LEVE_5, CMD_SYNC);

    /*bfd session <1-65535>, create a session and enter the session view*/
    install_element_level(CONFIG_NODE, &bfd_session_cmd, CONFIG_LEVE_5, CMD_SYNC);

    /*no bfd session <1-65535>, delete a session*/
    install_element_level(CONFIG_NODE, &undo_bfd_session_cmd, CONFIG_LEVE_5, CMD_SYNC);

    /*[no] bfd session remote-discriminator <1-65535>*/
    install_element_level(BFD_SESSION_NODE, &bfd_session_remote_disc_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(BFD_SESSION_NODE, &undo_bfd_session_remote_disc_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(BFD_SESSION_NODE, &bfd_session_local_disc_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(BFD_SESSION_NODE, &undo_bfd_session_local_disc_cmd, CONFIG_LEVE_5, CMD_SYNC);
    /*[no] bfd recv-interval (3 | 10 | 30 | 100 | 300 | 1000)*/
    install_element_level(CONFIG_NODE, &bfd_session_cc_interval_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &undo_bfd_session_cc_interval_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(BFD_SESSION_NODE, &bfd_session_cc_interval_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(BFD_SESSION_NODE, &undo_bfd_session_cc_interval_cmd, CONFIG_LEVE_5, CMD_SYNC);

    /*[no] bfd send-interval (3 | 10 | 30 | 100 | 300 | 1000)*/
    /*[no] bfd detect-multiplier <3-5>*/
    install_element_level(CONFIG_NODE, &bfd_session_detect_multiplier_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &undo_bfd_session_detect_multiplier_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(BFD_SESSION_NODE, &bfd_session_detect_multiplier_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(BFD_SESSION_NODE, &undo_bfd_session_detect_multiplier_cmd, CONFIG_LEVE_5, CMD_SYNC);

    /*[no] bfd delay-up <0-3600>*/
    //install_element(CONFIG_NODE, &bfd_session_delay_up_cmd);
    //install_element(CONFIG_NODE, &undo_bfd_session_delay_up_cmd);
    //install_element(BFD_SESSION_NODE, &bfd_session_delay_up_cmd);
    //install_element(BFD_SESSION_NODE, &undo_bfd_session_delay_up_cmd);

    /*[no] bfd enable used by interface*/
    /*bfd enable {session <1-65535> | source-ip A.B.C.D }*/
    //install_element(PHYSICAL_SUBIF_NODE, &bfd_intf_session_enable_cmd);
    //install_element(TRUNK_IF_NODE, &bfd_intf_session_enable_cmd);
    //install_element(PHYSICAL_SUBIF_NODE, &undo_bfd_intf_session_enable_cmd);
    //install_element(TRUNK_IF_NODE, &undo_bfd_intf_session_enable_cmd);
    install_element_level(PHYSICAL_IF_NODE, &bfd_intf_session_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &undo_bfd_intf_session_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);

    /*bfd config and state show*/
    install_element_level(CONFIG_NODE, &bfd_info_show_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(BFD_SESSION_NODE, &bfd_info_show_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(PHYSICAL_IF_NODE, &bfd_info_show_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    //install_element(TRUNK_IF_NODE, &bfd_info_show_cmd);
    //install_element(PHYSICAL_SUBIF_NODE, &bfd_info_show_cmd);

    /*for vpls car*/
    install_element_level (VSI_NODE, &mpls_vsi_storm_suppress_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (VSI_NODE, &no_mpls_vsi_storm_suppress_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    /*ping and tracert*/
    install_element_level (CONFIG_NODE, &ping_tunnel_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &ping_lsp_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &ping_ldp_lsp_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &ping_pw_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &ping_pw_remote_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &ping_pw_reply_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &ping_pw_remote_reply_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &ping_pw_reply_remote_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &tracert_static_lsp_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &tracert_ldp_lsp_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &tracert_pw_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &tracert_tunnel_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);

    /* mpls-oamå‘½ä»¤æ³¨å†�?*/
    install_element_level(MPLS_NODE, &mplstp_oam_session_cmd, VISIT_LEVE, CMD_LOCAL);
    install_element_level(MPLS_NODE, &no_mplstp_oam_session_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);

	install_element_level(MPLS_NODE, &show_mplstp_oam_session_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(MPLS_NODE, &show_mplstp_oam_meg_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(MPLSTP_OAM_SESS_NODE, &show_mplstp_oam_session_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(MPLSTP_OAM_SESS_NODE, &show_mplstp_oam_meg_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(MPLSTP_OAM_MEG_NODE, &show_mplstp_oam_meg_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_service_meg_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_service_meg_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_mp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_mp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_rmep_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_rmep_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_cc_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_cc_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_ais_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_ais_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_csf_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_csf_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_lock_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_lock_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_section_session_mac_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_section_session_mac_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_alarm_report_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_alarm_report_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_session_channel_type_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_session_channel_type_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MPLS_NODE, &mplstp_oam_session_global_channel_type_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MPLS_NODE, &no_mplstp_oam_session_global_channel_type_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/* oam mip */
	install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_mip_rmep_config_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_mip_rmep_config_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_mip_bind_lsp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_mip_bind_lsp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
#if 0
	install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_mip_bind_pw_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_mip_bind_pw_cmd_vtysh, CONFIG_LEVE_5);
#endif

    install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_lm_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_lm_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_dm_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLSTP_OAM_SESS_NODE, &no_mplstp_oam_dm_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_lb_enable_mep_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
	install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_lb_enable_mip_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
	install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_lb_enable_sec_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);
    install_element_level(MPLSTP_OAM_SESS_NODE, &mplstp_oam_lt_enable_cmd_vtysh, VISIT_LEVE, CMD_LOCAL);

    install_element_level(PHYSICAL_IF_NODE, &mplstp_oam_section_session_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &no_mplstp_oam_section_session_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/*oam meg*/
	install_element_level(MPLS_NODE, &mplstp_oam_meg_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level(MPLS_NODE, &no_mplstp_oam_meg_cmd_vtysh, VISIT_LEVE, CMD_SYNC);

	install_element_level(MPLSTP_OAM_MEG_NODE, &mplstp_oam_meg_level_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MPLSTP_OAM_MEG_NODE, &no_mplstp_oam_meg_level_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);


    /*ldp config command*/
    install_element_level(PHYSICAL_IF_NODE, &ldp_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &no_ldp_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_SUBIF_NODE, &ldp_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_SUBIF_NODE, &no_ldp_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(TRUNK_IF_NODE, &ldp_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(TRUNK_IF_NODE, &no_ldp_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(TRUNK_SUBIF_NODE, &ldp_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(TRUNK_SUBIF_NODE, &no_ldp_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &mpls_ldp_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &no_mpls_ldp_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &ldp_label_policy_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &no_ldp_label_policy_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &ldp_advertise_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &no_ldp_advertise_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &ldp_keepalive_interval_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &no_ldp_keepalive_interval_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &mpls_ldp_prefix_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &no_mpls_ldp_prefix_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &ldp_bfd_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &no_ldp_bfd_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &ldp_hello_interval_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &no_ldp_hello_interval_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &mpls_ldp_remote_peer_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MPLS_NODE, &no_mpls_ldp_remote_peer_cmd, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(CONFIG_NODE, &ldp_peer_lsrid_debug_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &no_ldp_peer_lsrid_debug_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &ldp_procedures_debug_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &no_ldp_procedures_debug_cmd, MANAGE_LEVE, CMD_LOCAL);


    install_element_level(CONFIG_NODE, &ldp_debug_cmd, MANAGE_LEVE, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &no_ldp_debug_cmd, MANAGE_LEVE, CMD_LOCAL);

    install_element_level(CONFIG_NODE, &ldp_show_config_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &ldp_show_adjance_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &ldp_show_session_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &ldp_show_lsp_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &ldp_show_statistics_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &ldp_show_ip_prefix_cmd, MONITOR_LEVE_2, CMD_LOCAL);

    /*ldp config command*/


    /*aps config command*/

    install_element_level (CONFIG_NODE, &mplsaps_cmd,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (MPLS_NODE, &mplsaps_session_cmd,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (MPLS_NODE, &no_mplsaps_session_cmd,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (CONFIG_NODE, &mplsaps_debug_packet_cmd,MANAGE_LEVE,CMD_LOCAL);
    install_element_level (CONFIG_NODE, &no_mplsaps_debug_packet_cmd,MANAGE_LEVE,CMD_LOCAL);

    //show commands
    install_element_level (CONFIG_NODE, &show_mplsaps_config_cmd,MONITOR_LEVE_2,CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_mplsaps_session_cmd,MONITOR_LEVE_2,CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_mplsaps_debug_cmd,MONITOR_LEVE_2,CMD_LOCAL);
    install_element_level (MPLS_NODE, &show_mplsaps_config_cmd,MONITOR_LEVE_2,CMD_LOCAL);
    install_element_level (MPLS_NODE, &show_mplsaps_session_cmd,MONITOR_LEVE_2,CMD_LOCAL);

	install_element_level (APS_NODE, &mplsaps_keep_alive_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &no_mplsaps_keep_alive_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &mplsaps_hold_off_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &no_mplsaps_hold_off_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &mplsaps_backup_create_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &mplsaps_backup_failback_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &no_mplsaps_backup_create_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &mplsaps_priority_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &no_mplsaps_priority_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &mplsaps_bind_pw_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &no_mplsaps_bind_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &mplsaps_bind_tunnel_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &no_mplsaps_bind_tunnel_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (APS_NODE, &mplsaps_session_enable_cmd,CONFIG_LEVE_5,CMD_SYNC);

    //admin commands
    install_element_level (APS_NODE, &mplsaps_admin_force_cmd,CONFIG_LEVE_5,CMD_LOCAL);
    install_element_level (APS_NODE, &no_mplsaps_admin_force_cmd,CONFIG_LEVE_5,CMD_LOCAL);
    install_element_level (APS_NODE, &mplsaps_admin_manual_cmd,CONFIG_LEVE_5,CMD_LOCAL);
    install_element_level (APS_NODE, &no_mplsaps_admin_manual_cmd,CONFIG_LEVE_5,CMD_LOCAL);
    install_element_level (APS_NODE, &mplsaps_admin_lockout_cmd,CONFIG_LEVE_5,CMD_LOCAL);
    install_element_level (APS_NODE, &no_mplsaps_admin_lockout_cmd,CONFIG_LEVE_5,CMD_LOCAL);
}



