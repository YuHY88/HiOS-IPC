/**
 * @file      : pw_cmd.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月28日 8:51:59
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/prefix.h>
#include <lib/log.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <lib/oam_common.h>
#include "mpls_main.h"
#include "mpls.h"
#include "labelm.h"
#include "tunnel.h"
#include "vpls_cmd.h"
#include "pw_cmd.h"
#include "mpls_oam/mpls_oam.h"
#include "bfd/bfd_session.h"
#include "lspm.h"
#include "lsp_static.h"
#include "mpls_aps/mpls_aps.h"


/* 初始化 pw node */
static struct cmd_node pw_node =
{
    PW_NODE,
    "%s(mpls-pw)# ",
    1
};


/**
 * @brief      : 下发 pw 状态更新
 * @param[in ] : ppwinfo - pw 结构
 * @param[in ] : subtype - 消息子类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_IPC
 * @author     : ZhangFj
 * @date       : 2018年2月28日 8:52:42
 * @note       :
 */
//int mpls_pw_send_pw_status(struct pw_info *ppwinfo, enum PW_SUBTYPE subtype)
int mpls_pw_status_download(struct pw_info *ppwinfo, enum PW_SUBTYPE subtype)

{
    int ret;

    MPLS_LOG_DEBUG();

    ret = MPLS_IPC_SENDTO_HAL(ppwinfo, sizeof(struct pw_info), 1, MODULE_ID_MPLS,
                        IPC_TYPE_PW, IPC_OPCODE_UPDATE, subtype, ppwinfo->pwindex);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to hal\n");

        return ERRNO_IPC;
    }

    ret = MPLS_IPC_SENDTO_FTM(ppwinfo, sizeof(struct pw_info), 1, MODULE_ID_MPLS,
                        IPC_TYPE_PW, IPC_OPCODE_UPDATE, subtype, ppwinfo->pwindex);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to ftm\n");

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 下发 pw 配置信息
 * @param[in ] : ppwinfo - pw 结构
 * @param[in ] : opcode  - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 8:54:25
 * @note       : 添加先下发至 hal 并等待返回，在下发至 ftm，删除消息不等待返回
 */
int mpls_pw_pwinfo_download(struct pw_info *ppwinfo, enum OPCODE_E opcode)
{
    int ret;

    MPLS_LOG_DEBUG();

    if (opcode == OPCODE_ADD)
    {
        ret = mpls_ipc_send_hal_wait_ack(ppwinfo, sizeof(struct pw_info), 1, 
                                MODULE_ID_MPLS, IPC_TYPE_PW, IPC_OPCODE_ADD, 0, 0);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal ret %d\n", ret);

            return ret;
        }

        ret = MPLS_IPC_SENDTO_FTM(ppwinfo, sizeof(struct pw_info), 1,
                            MODULE_ID_MPLS, IPC_TYPE_PW, IPC_OPCODE_ADD, 0, 0);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to ftm ret %d\n", ret);

            return ERRNO_IPC;
        }
    }
    else if (opcode == OPCODE_DELETE)
    {
        ret = MPLS_IPC_SENDTO_HAL(ppwinfo, sizeof(struct pw_info), 1,
                            MODULE_ID_MPLS, IPC_TYPE_PW, IPC_OPCODE_DELETE, 0, 0);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal ret %d\n", ret);

            return ERRNO_IPC;
        }

        ret = MPLS_IPC_SENDTO_FTM(ppwinfo, sizeof(struct pw_info), 1,
                            MODULE_ID_MPLS, IPC_TYPE_PW, IPC_OPCODE_DELETE, 0, 0);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to ftm ret %d\n", ret);

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 下发 pw 绑定接口消息
 * @param[in ] : ifindex - 绑定的接口索引
 * @param[in ] : pinfo   - 绑定消息携带的数据
 * @param[in ] : info    - 绑定的类型
 * @param[in ] : opcode  - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 8:58:06
 * @note       : 只有添加消息发送至 hal 才等待返回
 */
int mpls_pw_mplsif_download(uint32_t ifindex, void *pinfo, enum MPLSIF_INFO info,
                        enum OPCODE_E opcode)
{
    int ret;

    MPLS_LOG_DEBUG();

    if (OPCODE_ADD == opcode)
    {
        ret = mpls_ipc_send_hal_wait_ack(pinfo, 4, 1, MODULE_ID_MPLS,
                                    IPC_TYPE_MPLSIF, IPC_OPCODE_ADD, info, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal ret %d\n", ret);

            return ret;
        }

        ret = MPLS_IPC_SENDTO_FTM(pinfo, 4, 1, MODULE_ID_MPLS,
                            IPC_TYPE_MPLSIF, IPC_OPCODE_ADD, info, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to ftm ret %d\n", ret);

            return ERRNO_IPC;
        }
    }
    else if (OPCODE_DELETE == opcode)
    {
        ret = MPLS_IPC_SENDTO_HAL(pinfo, 4, 1, MODULE_ID_MPLS,
                                IPC_TYPE_MPLSIF, IPC_OPCODE_DELETE, info, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal ret %d\n", ret);

            return ERRNO_IPC;
        }

        ret = MPLS_IPC_SENDTO_FTM(pinfo, 4, 1, MODULE_ID_MPLS,
                            IPC_TYPE_MPLSIF, IPC_OPCODE_DELETE, info, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to ftm ret %d\n", ret);

            return ERRNO_IPC;
        }
    }
    else if (OPCODE_UPDATE == opcode)
    {
        ret = MPLS_IPC_SENDTO_HAL(pinfo, 4, 1, MODULE_ID_MPLS,
                                IPC_TYPE_MPLSIF, IPC_OPCODE_UPDATE, info, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal ret %d\n", ret);

            return ERRNO_IPC;
        }

        ret = MPLS_IPC_SENDTO_FTM(pinfo, 4, 1, MODULE_ID_MPLS,
                            IPC_TYPE_MPLSIF, IPC_OPCODE_UPDATE, info, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to ftm ret %d\n", ret);

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 显示 pw 配置信息
 * @param[in ] : vty   - vty 全局结构
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:03:27
 * @note       :
 */
static void mpls_pw_show_verbose(struct vty *vty, struct l2vc_entry *pl2vc)
{
    char acname[NAME_STRING_LEN]        = "";
    char tunnel[NAME_STRING_LEN]        = "";
    char static_tunnel[NAME_STRING_LEN] = "";
    char peerip[INET_ADDRSTRLEN]        = "";
    struct pw_info  *ppwinfo = NULL;
    struct hqos_t   *phqos   = NULL;
    struct listnode *pnode   = NULL;
    uchar           *pmac    = NULL;
    int num = 0;

    MPLS_LOG_DEBUG();

    ppwinfo = &pl2vc->pwinfo;

    vty_out(vty, "%-20s: %s%s", "PW name", pl2vc->name, VTY_NEWLINE);

    if (0 != pl2vc->pwinfo.mspw_index)
    {
        vty_out(vty, "%-20s: %s%s", "MS-PW name", pl2vc->pswitch_pw->name, VTY_NEWLINE);
    }

    vty_out(vty, "%-20s: %s%s", "protocol", ppwinfo->protocol == PW_PROTO_SVC ?
                "svc" : ppwinfo->protocol == PW_PROTO_MARTINI ? "martini" :
                "-", VTY_NEWLINE);

    vty_out(vty, "%-20s: %u%s", "in label", pl2vc->inlabel, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "out label", pl2vc->outlabel, VTY_NEWLINE);

    vty_out(vty, "%-20s: %u%s", "VC ID", ppwinfo->vcid, VTY_NEWLINE);
    vty_out(vty, "%-20s: %s%s", "VC type", ppwinfo->ac_type == AC_TYPE_ETH ?
                "ethernet" : ppwinfo->ac_type == AC_TYPE_TDM ? "tdm" : "-", VTY_NEWLINE);

    if (0 == pl2vc->peerip.addr.ipv4)
    {
        vty_out(vty, "%-20s: %s%s", "destination", "-", VTY_NEWLINE);
    }
    else
    {
        inet_ipv4tostr(pl2vc->peerip.addr.ipv4, peerip);

        vty_out(vty, "%-20s: %s%s", "destination", peerip, VTY_NEWLINE);
    }

    if (ppwinfo->tag_flag == PW_ENCAP_ETHERNET_RAW)
    {
        vty_out(vty, "%-20s: %s%s", "encapsulate", "ethernet raw", VTY_NEWLINE);
    }
    else if (ppwinfo->tag_flag == PW_ENCAP_ETHERNET_TAG)
    {
        vty_out(vty, "%-20s: %s%s", "encapsulate", "ethernet tag", VTY_NEWLINE);
    }
    else if (ppwinfo->tag_flag == PW_ENCAP_VLAN_RAW)
    {
        vty_out(vty, "%-20s: %s%s", "encapsulate", "vlan raw", VTY_NEWLINE);
    }
    else if (ppwinfo->tag_flag == PW_ENCAP_VLAN_TAG)
    {
        vty_out(vty, "%-20s: %s%s", "encapsulate", "vlan tag", VTY_NEWLINE);
    }

    if (ppwinfo->tag_flag != PW_ENCAP_ETHERNET_RAW)
    {
        vty_out(vty, "%-20s: %#x%s", "tpid", ppwinfo->tpid, VTY_NEWLINE);
        vty_out(vty, "%-20s: %u%s", "vlan", ppwinfo->vlan, VTY_NEWLINE);
    }

    if (AC_TYPE_ETH == ppwinfo->ac_type)
    {
        vty_out(vty, "%-20s: %u%s", "mtu", ppwinfo->mtu, VTY_NEWLINE);
    }

    vty_out(vty, "%-20s: %s%s", "ctrl-word", ppwinfo->ctrlword_flag == ENABLE ?
                "enable" : "disable", VTY_NEWLINE);

    if (AC_TYPE_TDM == ppwinfo->ac_type)
    {
        vty_out(vty, "%-20s: %u%s", "ces frame-num", ppwinfo->ces.frame_num, VTY_NEWLINE);
    }

    vty_out(vty, "%-20s: %s%s", "statistics", pl2vc->statis_enable == ENABLE ?
                    "enable" : "disable", VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "domain id", pl2vc->domain_id, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "phb id", pl2vc->phb_id, VTY_NEWLINE);

    if (0 != pl2vc->car_cir[0])
    {
        if ((pl2vc->car_cir[0] & CAR_VALUE_L1_FLAG) == CAR_VALUE_L1_FLAG)
        {
            vty_out(vty, "%-20s: cir %u  pir %u%s", "ingress car l1",
                        pl2vc->car_cir[0]&(~CAR_VALUE_L1_FLAG),
                        pl2vc->car_pir[0]&(~CAR_VALUE_L1_FLAG), VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%-20s: cir %u  pir %u%s", "ingress car", pl2vc->car_cir[0], pl2vc->car_pir[0], VTY_NEWLINE);
        }
    }
    else
    {
        vty_out(vty, "%-20s: cir %s  pir %s%s", "ingress car", "-", "-", VTY_NEWLINE);
    }

    if (0 != pl2vc->car_cir[1])
    {
        if ((pl2vc->car_cir[1] & CAR_VALUE_L1_FLAG) == CAR_VALUE_L1_FLAG)
        {
            vty_out(vty, "%-20s: cir %u  pir %u%s", "egress car l1",
                        pl2vc->car_cir[1]&(~CAR_VALUE_L1_FLAG),
                        pl2vc->car_pir[1]&(~CAR_VALUE_L1_FLAG), VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%-20s: cir %u  pir %u%s", "egress car", pl2vc->car_cir[1], pl2vc->car_pir[1], VTY_NEWLINE);
        }
    }
    else
    {
        vty_out(vty, "%-20s: cir %s  pir %s%s", "egress car", "-", "-", VTY_NEWLINE);
    }

    if (0 != pl2vc->hqos_id)
    {
        phqos = qos_hqos_get(pl2vc->hqos_id, MODULE_ID_MPLS);
        if (NULL != phqos)
        {
            vty_out(vty, "%-20s: %u%s", "hqos cir", phqos->cir, VTY_NEWLINE);

            if (phqos->pir != 0)
            {
                vty_out(vty, "%-20s: %u%s", "hqos pir", phqos->pir, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%-20s: %s%s", "hqos pir", "-", VTY_NEWLINE);
            }

            if (phqos->hqos_id != 0)
            {
                vty_out(vty, "%-20s: %u%s", "hqos queue-profile", phqos->hqos_id, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%-20s: %s%s", "hqos queue-profile", "-", VTY_NEWLINE);
            }
        }
    }
    else
    {
        vty_out(vty, "%-20s: %s%s", "hqos cir", "-", VTY_NEWLINE);
        vty_out(vty, "%-20s: %s%s", "hqos pir", "-", VTY_NEWLINE);
        vty_out(vty, "%-20s: %s%s", "hqos queue-profile", "-", VTY_NEWLINE);
    }

    ifm_get_name_by_ifindex(ppwinfo->ifindex, acname);

    vty_out(vty, "%-20s: %u%s", "mplstp-oam id", pl2vc->mplsoam_id, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "bfd id", pl2vc->bfd_id, VTY_NEWLINE);
    vty_out(vty, "%-20s: %u%s", "aps id", pl2vc->aps_id, VTY_NEWLINE);

    if (0 != ppwinfo->vpls.vsi_id)
    {
        vty_out(vty, "%-20s: %u%s", "VSI id", ppwinfo->vpls.vsi_id, VTY_NEWLINE);
        vty_out(vty, "%-20s: %s%s", "VSI type", ppwinfo->vpls.upe==0?"spe":"upe", VTY_NEWLINE);
    }

    if (PW_ALREADY_CONFIG_STATIC_TUNNEL(pl2vc))
    {
        ifm_get_name_by_ifindex(pl2vc->tunl_index, static_tunnel);

        vty_out(vty, "%-20s: %s%s", "static tunnel", static_tunnel, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-20s: %s%s", "static tunnel", "-", VTY_NEWLINE);
    }

    if (PW_ALREADY_CONFIG_LSP_TUNNEL(pl2vc))
    {
        vty_out(vty, "%-20s: %s%s", "lsp tunnel", pl2vc->lsp_name, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-20s: %s%s", "lsp tunnel", " ", VTY_NEWLINE);
    }

    vty_out(vty, "%-20s: ", "tunnel policy");
    vty_out(vty, "%-14s : %s%s", "ldp tunnel", pl2vc->ldp_tunnel==ENABLE?"enable":"disable", VTY_NEWLINE);
    vty_out(vty, "%-20s: %-14s : %s%s", " ", "gre tunnel", pl2vc->gre_tunnel==ENABLE?"enable":"disable", VTY_NEWLINE);
    vty_out(vty, "%-20s: %-14s : %s%s", " ", "mpls_tp tunnel", pl2vc->mplstp_tunnel==ENABLE?"enable":"disable", VTY_NEWLINE);
    vty_out(vty, "%-20s: %-14s : %s%s", " ", "mpls_te tunnel", pl2vc->mplste_tunnel==ENABLE?"enable":"disable", VTY_NEWLINE);

    if (0 == pl2vc->pwinfo.nhp_index)
    {
        vty_out(vty, "%-20s: %s%s", "tunnel", "-", VTY_NEWLINE);
    }
    else
    {
        if (NHP_TYPE_TUNNEL == pl2vc->pwinfo.nhp_type)
        {
            ifm_get_name_by_ifindex(pl2vc->pwinfo.nhp_index, tunnel);
            vty_out(vty, "%-20s: %s%s", "tunnel", tunnel, VTY_NEWLINE);
        }
        else if (NHP_TYPE_LSP == pl2vc->pwinfo.nhp_type)
        {
            if (0 != pl2vc->tunl_index)
            {
                vty_out(vty, "%-20s: %s%s", "tunnel", pl2vc->lsp_name, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%-20s: %s%s", "tunnel", "Ldp lsp", VTY_NEWLINE);
            }
        }
    }

    vty_out(vty, "%-20s: %s%s", "AC interface", ppwinfo->ifindex==0?"-":acname, VTY_NEWLINE);
    if(ppwinfo->ifindex != 0)
    {
        vty_out(vty, "%-20s: %s%s", "AC status", pl2vc->ac_flag==LINK_UP?"up":"down", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-20s: %s%s", "AC status", "-", VTY_NEWLINE);
    }
    vty_out(vty, "%-20s: %s%s", "admin status", ppwinfo->admin_up==LINK_UP?"up":"down", VTY_NEWLINE);
    vty_out(vty, "%-20s: %s%s", "link status", ppwinfo->up_flag==LINK_UP?"up":"down", VTY_NEWLINE);

    for (ALL_LIST_ELEMENTS_RO(pl2vc->mac_list, pnode, pmac))
    {
        if (++num == 1)
        {
            vty_out(vty, "%-20s: %s", "static mac", VTY_NEWLINE);
        }

        vty_out(vty, "%-20s: %02x:%02x:%02x:%02x:%02x:%02x%s", "",
                    pmac[0], pmac[1], pmac[2], pmac[3], pmac[4], pmac[5], VTY_NEWLINE);
    }

    return;
}


/**
 * @brief      : 显示 pw 简略信息
 * @param[in ] : vty   - vty 全局结构
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:05:21
 * @note       :
 */
static void mpls_pw_show_brief(struct vty *vty, struct l2vc_entry *pl2vc)
{
    char acname[NAME_STRING_LEN] = "";
    char tunnel[NAME_STRING_LEN] = "";

    vty_out(vty, "%-32s", pl2vc->name);

    if (0 == pl2vc->pwinfo.ifindex)
    {
        vty_out(vty, "%-31s", "-");
        vty_out(vty, "%-10s", "-");
    }
    else
    {
        ifm_get_name_by_ifindex(pl2vc->pwinfo.ifindex, acname);

        vty_out(vty, "%-31s", acname);
        vty_out(vty, "%-10s", pl2vc->ac_flag == LINK_UP ? "up" : "down");
    }

    if (0 == pl2vc->pwinfo.nhp_index)
    {
        vty_out(vty, "%-15s", "-");
        vty_out(vty, "%-10s", "-");
    }
    else
    {
        if (NHP_TYPE_TUNNEL == pl2vc->pwinfo.nhp_type)
        {
            ifm_get_name_by_ifindex(pl2vc->pwinfo.nhp_index, tunnel);

            vty_out(vty, "%-15s", tunnel);
        }
        else if (NHP_TYPE_LSP == pl2vc->pwinfo.nhp_type)
        {
            if (pl2vc->tunl_index != 0)
            {
                vty_out(vty, "%-15s", pl2vc->lsp_name);
            }
            else
            {
                vty_out(vty, "%-15s", "Ldp lsp");
            }
        }

        vty_out(vty, "%-10s", pl2vc->pwinfo.up_flag == LINK_UP ? "up" : "down");
    }

    vty_out(vty, "%s", VTY_NEWLINE);

    return;
}


/**
 * @brief      : 显示 pw 数量
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:06:28
 * @note       :
 */
static void mpls_pw_show_summary(struct vty *vty)
{
    struct hash_bucket *pbucket = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    uint32_t pw_up_num   = 0;
    uint32_t pw_down_num = 0;
    int      cursor      = 0;

    HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
    {
        pl2vc = (struct l2vc_entry *)pbucket->data;
        if (pl2vc == NULL)
        {
            continue;
        }

        if ((LINK_UP == pl2vc->pwinfo.admin_up) && (LINK_UP == pl2vc->pwinfo.up_flag))
        {
            pw_up_num++;
        }
        else
        {
            pw_down_num++;
        }
    }

    vty_out(vty, "%-10s:     %d %s", "total", l2vc_table.num_entries, VTY_NEWLINE);
    vty_out(vty, "%-10s:     %d %s", "up", pw_up_num, VTY_NEWLINE);
    vty_out(vty, "%-10s:     %d %s", "down", pw_down_num, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);

    return;
}


/**
 * @brief      : pw 配置管理
 * @param[in ] : vty   - vty 全局结构
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:09:36
 * @note       :
 */
static void mpls_pw_config_write_process(struct vty *vty, struct l2vc_entry *pl2vc)
{
    char peer_ip[NAME_STRING_LEN]   = "";
    char tunnel[NAME_STRING_LEN]    = "";
    struct l2vc_entry *pl2vc_ms     = NULL;
    struct l2vc_entry *pl2vc_master = NULL;
    struct l2vc_entry *pl2vc_backup = NULL;
    struct pw_info    *ppwinfo      = NULL;
    struct hqos_t     *phqos        = NULL;
    struct bfd_sess   *psess        = NULL;

    if (NULL == pl2vc)
    {
        return;
    }

    ppwinfo = &pl2vc->pwinfo;

    vty_out(vty, " pw %s%s", pl2vc->name, VTY_NEWLINE);

    if (ppwinfo->protocol == PW_PROTO_MARTINI)
    {
        vty_out(vty, "  protocol martini%s", VTY_NEWLINE);
    }

    if (ppwinfo->ac_type == AC_TYPE_TDM)
    {
        vty_out(vty, "  vc-type tdm%s", VTY_NEWLINE);

        if (ppwinfo->ces.frame_num != 8)
        {
            vty_out(vty, "  ces frame-num %u%s", ppwinfo->ces.frame_num, VTY_NEWLINE);
        }
    }
    else
    {
        if (ppwinfo->ctrlword_flag == ENABLE)
        {
            vty_out(vty, "  ctrl-word enable%s", VTY_NEWLINE);
        }

        if (ppwinfo->mtu != 1500)
        {
            vty_out(vty, "  mtu %u%s", ppwinfo->mtu, VTY_NEWLINE);
        }
    }

	if(ppwinfo->sample_interval != 5)
	{
		vty_out(vty, "  sample interval %u%s", ppwinfo->sample_interval, VTY_NEWLINE);
	}
    
    if ((pl2vc->peerip.addr.ipv4 != 0) && (ppwinfo->vcid != 0))
    {
        inet_ipv4tostr(pl2vc->peerip.addr.ipv4, peer_ip);

        vty_out(vty, "  peer %s vc-id %u%s", peer_ip, ppwinfo->vcid, VTY_NEWLINE);
    }

    if (PW_PROTO_IS_SVC(&pl2vc->pwinfo) && (pl2vc->inlabel != 0) && (pl2vc->outlabel != 0))
    {
        vty_out(vty, "  label in %u out %u%s", pl2vc->inlabel,
                    pl2vc->outlabel, VTY_NEWLINE);
    }

    if (PW_ALREADY_CONFIG_STATIC_TUNNEL(pl2vc))
    {
        ifm_get_name_by_ifindex(pl2vc->tunl_index, tunnel);

        vty_out(vty, "  tunnel bind interface %s%s", tunnel, VTY_NEWLINE);
    }
    else if (PW_ALREADY_CONFIG_LSP_TUNNEL(pl2vc))
    {
        vty_out(vty, "  tunnel bind lsp %s%s", pl2vc->lsp_name, VTY_NEWLINE);
    }
    else
    {
        if (PW_ALREADY_CONFIG_LDP_TUNNEL(pl2vc))
        {
            vty_out(vty, "  tunnel policy ldp%s", VTY_NEWLINE);
        }

        if (PW_ALREADY_CONFIG_GRE_TUNNEL(pl2vc))
        {
            vty_out(vty, "  tunnel policy gre%s", VTY_NEWLINE);
        }

        if (PW_ALREADY_CONFIG_TP_TUNNEL(pl2vc))
        {
            vty_out(vty, "  tunnel policy mpls-tp%s", VTY_NEWLINE);
        }

        if (PW_ALREADY_CONFIG_TE_TUNNEL(pl2vc))
        {
            vty_out(vty, "  tunnel policy mpls-te%s", VTY_NEWLINE);
        }
    }

    if (ppwinfo->tag_flag == PW_ENCAP_ETHERNET_TAG)
    {
        vty_out(vty, "  encapsulate ethernet tag tpid %#x vlan %u%s",
                    ppwinfo->tpid, ppwinfo->vlan, VTY_NEWLINE);
    }
    else if (ppwinfo->tag_flag == PW_ENCAP_VLAN_RAW)
    {
        vty_out(vty, "  encapsulate vlan raw tpid %#x vlan %u%s",
                    ppwinfo->tpid, ppwinfo->vlan, VTY_NEWLINE);
    }
    else if (ppwinfo->tag_flag == PW_ENCAP_VLAN_TAG)
    {
        vty_out(vty, "  encapsulate vlan  tag tpid %#x vlan %u%s",
                    ppwinfo->tpid, ppwinfo->vlan, VTY_NEWLINE);
    }

    if (pl2vc->domain_id != 0)
    {
        vty_out(vty, "  qos mapping enable exp-domain %u%s", pl2vc->domain_id, VTY_NEWLINE);
    }

    if (pl2vc->phb_id != 0)
    {
        vty_out(vty, "  qos mapping enable exp-phb %u%s", pl2vc->phb_id, VTY_NEWLINE);
    }

    if (pl2vc->statis_enable == ENABLE)
    {
        vty_out(vty, "  statistics enable%s", VTY_NEWLINE);
    }

    if (pl2vc->bfd_id != 0)
    {
    	psess = bfd_session_lookup(pl2vc->bfd_id);
		if(NULL != psess)
		{
			if(psess->without_ip)
			{
				if(0 != psess->session_id)
				{
					vty_out(vty, "  bfd enable raw%s", VTY_NEWLINE);
				}
				else
				{
					vty_out(vty, "  bfd enable session %u raw%s", pl2vc->bfd_id, VTY_NEWLINE);
				}
			}
			else
			{				
				if(0 != psess->session_id)
				{
					vty_out(vty, "  bfd enable%s", VTY_NEWLINE);
				}
				else
				{
					vty_out(vty, "  bfd enable session %u%s", pl2vc->bfd_id, VTY_NEWLINE);
				}
			}
		}
    }

    if (pl2vc->car_cir[0] != 0)
    {
        if ((pl2vc->car_cir[0] & CAR_VALUE_L1_FLAG) == CAR_VALUE_L1_FLAG)
        {
            vty_out(vty, "  qos car include-interframe ingress cir %u pir %u%s",
                        pl2vc->car_cir[0]&(~CAR_VALUE_L1_FLAG),
                        pl2vc->car_pir[0]&(~CAR_VALUE_L1_FLAG), VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "  qos car ingress cir %u pir %u%s", pl2vc->car_cir[0], pl2vc->car_pir[0], VTY_NEWLINE);
        }
    }

    if (pl2vc->car_cir[1] != 0)
    {
        if ((pl2vc->car_cir[1] & CAR_VALUE_L1_FLAG) == CAR_VALUE_L1_FLAG)
        {
            vty_out(vty, "  qos car include-interframe egress cir %u pir %u%s",
                        pl2vc->car_cir[1]&(~CAR_VALUE_L1_FLAG),
                        pl2vc->car_pir[1]&(~CAR_VALUE_L1_FLAG), VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "  qos car egress cir %u pir %u%s", pl2vc->car_cir[1], pl2vc->car_pir[1], VTY_NEWLINE);
        }
    }

    if (pl2vc->hqos_id != 0)
    {
        phqos = qos_hqos_get(pl2vc->hqos_id, MODULE_ID_MPLS);
        if (NULL != phqos)
        {
            vty_out(vty, "  hqos cir %u", phqos->cir);

            if (phqos->pir != 0)
            {
                vty_out(vty, " pir %u", phqos->pir);
            }

            if (phqos->hqos_id != 0)
            {
                vty_out(vty, " queue-profile %u", phqos->hqos_id);
            }

            vty_out(vty, "%s", VTY_NEWLINE);
        }
    }

    if (ppwinfo->vpls.vsi_id != 0)
    {
        if (ppwinfo->backup_index != 0)
        {
            if (ppwinfo->pw_type == PW_TYPE_MASTER)
            {
                pl2vc_backup = pw_get_l2vc(pw_lookup(ppwinfo->backup_index));
                if (NULL != pl2vc_backup)
                {
                    vty_out(vty, "  switch vsi %u backup pw %s%s", ppwinfo->vpls.vsi_id,
                            pl2vc_backup->name, VTY_NEWLINE);
                }
            }
            else if (ppwinfo->pw_type == PW_TYPE_BACKUP)
            {
                pl2vc_master = pw_get_l2vc(pw_lookup(ppwinfo->backup_index));
                if (NULL != pl2vc_master)
                {
                    vty_out(vty, "  switch vsi %u master pw %s%s", ppwinfo->vpls.vsi_id,
                            pl2vc_master->name, VTY_NEWLINE);
                }
            }
        }
        else
        {
            vty_out(vty, "  switch vsi %u %s%s", ppwinfo->vpls.vsi_id,
                        ppwinfo->vpls.upe == 0 ? "spe" : "upe", VTY_NEWLINE);
        }
    }

    if (ppwinfo->mspw_index != 0)
    {
        pl2vc_ms = pw_get_l2vc(pw_lookup(ppwinfo->mspw_index));
        if (NULL != pl2vc_ms)
        {
            vty_out(vty, "  switch pw %s%s", pl2vc->pswitch_pw->name, VTY_NEWLINE);
        }
    }

	if ((pl2vc->mplsoam_id >= 1) && (pl2vc->mplsoam_id <= OAM_MEP_MAX))
    {
        vty_out(vty, "  mplstp-oam enable session %u%s", pl2vc->mplsoam_id, VTY_NEWLINE);
    }

    return;
}


/**
 * @brief      : pw 创建命令
 * @param[in ] : NAME - pw 名字
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:12:05
 * @note       : pw 名字唯一
 */
DEFUN (mpls_pw_name,
    mpls_pw_name_cmd,
    "pw NAME",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc   = NULL;
    char              *pprompt = NULL;
    int ret;

    MPLS_LSRID_CHECK(vty);

    pl2vc = l2vc_lookup((uchar *)argv[0]);
    if (NULL == pl2vc)
    {
        pl2vc = l2vc_create();
        if (NULL == pl2vc)
        {
            vty_error_out(vty, "Memory alloc failed !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        memcpy(pl2vc->name, argv[0], strlen(argv[0]));
        memcpy(pl2vc->pwinfo.name, pl2vc->name, strlen(argv[0]));

        ret = l2vc_add(pl2vc);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            l2vc_free(pl2vc);

            return CMD_WARNING;
        }
    }

    vty->index = pl2vc;
    vty->node  = PW_NODE;

    pprompt = vty->change_prompt;
    if (NULL != pprompt)
    {
        snprintf(pprompt, VTY_BUFSIZ, "%%s(mpls-pw-%s)# ", argv[0]);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 删除命令
 * @param[in ] : NAME - pw 名字
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:41:59
 * @note       : 处于绑定状态的 pw 不可被删除
 */
DEFUN (no_mpls_pw_name,
    no_mpls_pw_name_cmd,
    "no pw NAME",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = NULL;

    pl2vc = l2vc_lookup((uchar *)argv[0]);
    if (NULL == pl2vc)
    {
        return CMD_SUCCESS;
    }

    if (pl2vc->pwinfo.ifindex != 0)
    {
        vty_error_out(vty, "PW is binding under the interface, "
                    "please first unbundling !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (pl2vc->pwinfo.vpls.vsi_id != 0)
    {
        vty_error_out(vty, "PW has joined the VPN, please remove it !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (pl2vc->pwinfo.mspw_index != 0)
    {
        vty_error_out(vty, "MS-PW can't be deleted !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (pl2vc->domain_id != 0)
    {
        qos_domain_ref_operate(pl2vc->domain_id, QOS_TYPE_EXP, MODULE_ID_MPLS, REF_OPER_DECREASE);
    }

    if (pl2vc->phb_id != 0)
    {
        qos_phb_ref_operate(pl2vc->phb_id, QOS_TYPE_EXP, MODULE_ID_MPLS, REF_OPER_DECREASE);
    }

    if (pl2vc->hqos_id != 0)
    {
        qos_hqos_delete(pl2vc->hqos_id, MODULE_ID_MPLS);
    }

    if (PW_ALREADY_BIND_BFD(pl2vc))
    {
        bfd_unbind_for_lsp_pw(BFD_TYPE_PW, pl2vc->bfd_id, pl2vc->pwinfo.pwindex);
    }

    if (PW_ALREADY_BIND_MPLSTP_OAM(pl2vc))
    {
        mplsoam_session_disable(pl2vc->mplsoam_id);
    }

    if (pl2vc->aps_id != 0)
    {
        mplsaps_unbind_lsp_pw(pl2vc->aps_id);
    }

    l2vc_delete((uchar *)argv[0]);

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 协议类型配置命令
 * @param[in ] : svc     - 静态 pw（Switching Virtual Circuit）
 * @param[in ] : martini - 基于 ldp 信令的动态 pw
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:44:08
 * @note       : pw 处于绑定状态不允许配置该命令，默认为 svc
 */
DEFUN (mpls_pw_protocol,
    mpls_pw_protocol_cmd,
    "protocol (svc | martini)",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    if (argv[0][0] == 's')
    {
        pl2vc->pwinfo.protocol = PW_PROTO_SVC;
    }
    else if (argv[0][0] == 'm')
    {
        if (PW_ALREADY_BIND_BFD(pl2vc) || PW_ALREADY_BIND_MPLSTP_OAM(pl2vc))
        {
            vty_error_out(vty, "bfd or oam has been binded %s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        pl2vc->pwinfo.protocol = PW_PROTO_MARTINI;

        /* martini 方式不需要配置标签 */
        if (0 != pl2vc->inlabel)
        {
            label_free(pl2vc->inlabel);
        }

        pl2vc->inlabel  = 0;
        pl2vc->outlabel = 0;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 协议类型删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:48:38
 * @note       : pw 处于绑定状态不允许配置该命令，默认为 svc
 */
DEFUN (no_mpls_pw_protocol,
    no_mpls_pw_protocol_cmd,
    "no protocol",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    if (0 != pl2vc->inlabel)
    {
        label_free(pl2vc->inlabel);
    }

    pl2vc->pwinfo.protocol = PW_PROTO_SVC;
    pl2vc->inlabel         = 0;
    pl2vc->outlabel        = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw vc 类型配置命令
 * @param[in ] : ethernet - ethernet 类型
 * @param[in ] : tdm      - tdm 类型
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:50:34
 * @note       : pw 处于绑定状态不允许配置该命令，默认为 ethernet
 */
DEFUN (mpls_pw_vc_type,
    mpls_pw_vc_type_cmd,
    "vc-type (ethernet | tdm)",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    if (argv[0][0] == 'e')
    {
        pl2vc->pwinfo.ac_type = AC_TYPE_ETH;

        /* ethernet 类型清除 tdm 类型属性 */
        pl2vc->pwinfo.ces.frame_num = 0;
    }
    else if (argv[0][0] == 't')
    {
        pl2vc->pwinfo.ac_type       = AC_TYPE_TDM;
        pl2vc->pwinfo.ces.frame_num = 8;

        /* tdm 类型清除 ethernet 类型属性 */
        pl2vc->pwinfo.mtu = 1500;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw vc 类型删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:53:07
 * @note       : pw 处于绑定状态不允许配置该命令，默认为 ethernet
 */
DEFUN (no_mpls_pw_vc_type,
    no_mpls_pw_vc_type_cmd,
    "no vc-type",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    pl2vc->pwinfo.ac_type = AC_TYPE_ETH;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw peer ip 和 vc-id 配置命令
 * @param[in ] : A.B.C.D        - peer ip
 * @param[in ] : <1-4294967295> - vc id
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:55:20
 * @note       : pw 处于绑定状态不允许配置该命令，配置 vc-id 同时创建 pw 结构
 * @note       : pw 索引为 vc-id
 */
DEFUN (mpls_pw_peer_vcid,
    mpls_pw_peer_vcid_cmd,
    "peer A.B.C.D vc-id <1-4294967295>",
    MPLS_CLI_INFO)
{
    struct l2vc_entry  *pl2vc   = (struct l2vc_entry *)vty->index;
    struct pw_info     *ppwinfo = NULL;
    struct hash_bucket *pbucket = NULL;
    struct prefix_ipv4  peer_ip;
    uint32_t            vcid;
    int                 cursor;
    int                 ret;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    /* argv[0]: peer ip */
    if (0 == str2prefix_ipv4(argv[0], &peer_ip))
    {
        vty_error_out(vty, "The specified peer ip is invalid !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    peer_ip.prefix.s_addr = ntohl(peer_ip.prefix.s_addr);

    ret = inet_valid_network(peer_ip.prefix.s_addr);
    if ((!ret) || (peer_ip.prefix.s_addr == 0))
    {
        vty_error_out(vty, "The specified peer ip is invalid !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[1]: vc-id */
    vcid = strtoul(argv[1], NULL, 10);

    if (pl2vc->pwinfo.vcid != 0)
    {
        vty_error_out(vty, "The vc-id have been configured !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* vcid 重复检测 */
    HASH_BUCKET_LOOP(pbucket, cursor, pw_table)
    {
        ppwinfo = (struct pw_info *)pbucket->data;
        if (ppwinfo == NULL)
        {
            continue;
        }

        if (ppwinfo->vcid == vcid)
        {
            vty_error_out(vty, "The vc-id being used !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    pl2vc->pwinfo.pwindex = vcid;

    ret = pw_add(&pl2vc->pwinfo);
    if (ERRNO_SUCCESS != ret)
    {
        pl2vc->pwinfo.pwindex = 0;

        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    pl2vc->peerip.addr.ipv4 = peer_ip.prefix.s_addr;
    pl2vc->peerip.type      = INET_FAMILY_IPV4;
    pl2vc->pwinfo.vcid      = vcid;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw peer ip 和 vc-id 删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:58:05
 * @note       : pw 处于绑定状态不允许配置该命令，删除 vc-id 同时删除 pw 结构
 */
DEFUN (no_mpls_pw_peer_vcid,
    no_mpls_pw_peer_vcid_cmd,
    "no peer vc-id",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    if (PW_ALREADY_BIND_BFD(pl2vc))
    {
        vty_warning_out(vty, "Please unbundling BFD at first !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    pw_delete(pl2vc->pwinfo.pwindex);

    pl2vc->peerip.addr.ipv4 = 0;
    pl2vc->peerip.type      = INET_FAMILY_INVALID;
    pl2vc->pwinfo.vcid      = 0;
    pl2vc->pwinfo.pwindex   = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 标签配置命令
 * @param[in ] : <16-1048575> - pw 入标签
 * @param[in ] : <16-1048575> - pw 出标签
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 9:59:31
 * @note       : pw 处于绑定状态不允许配置该命令，只有协议类型为 svc 时才允许配置该命令
 */
DEFUN (mpls_pw_label,
    mpls_pw_label_cmd,
    "label in <16-1048575> out <16-1048575>",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    uint32_t inlabel;
    int      ret;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);
    PW_PROTOCOL_NOT_CONFIG(vty, pl2vc->pwinfo.protocol);

    if (pl2vc->pwinfo.protocol == PW_PROTO_MARTINI)
    {
        vty_error_out(vty, "Can't configuration pw label !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    inlabel = atoi((char *)argv[0]);
    if (inlabel != pl2vc->inlabel)
    {
        ret = label_get(inlabel, MODULE_ID_L2VPN);
        if (0 == ret)
        {
            vty_error_out(vty, "The in label being used !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (0 != pl2vc->inlabel)
        {
            label_free(pl2vc->inlabel);
        }
    }

    /* argv[0]: in label */
    pl2vc->inlabel = inlabel;

    /* argv[1]: out label */
    pl2vc->outlabel = atoi((char *)argv[1]);

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 标签删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 10:01:14
 * @note       : pw 处于绑定状态不允许配置该命令
 */
DEFUN (no_mpls_pw_label,
    no_mpls_pw_label_cmd,
    "no label",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    label_free(pl2vc->inlabel);

    pl2vc->inlabel  = 0;
    pl2vc->outlabel = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw mtu 配置命令
 * @param[in ] : <46-9600> - mtu
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 10:02:31
 * @note       : pw 处于绑定状态不允许配置该命令，默认为 1500
 */
DEFUN (mpls_pw_mtu,
    mpls_pw_mtu_cmd,
    "mtu <46-12288>",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);
    PW_VC_TYPE_NOT_CONFIG(vty, pl2vc->pwinfo.ac_type);
    PW_VC_TYPE_NOT_ETH(vty, pl2vc->pwinfo.ac_type);

    pl2vc->pwinfo.mtu = atoi(argv[0]);

    return CMD_SUCCESS;
}


/**
 * @brief      : pw mtu 删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 10:08:18
 * @note       : pw 处于绑定状态不允许配置该命令，默认为 1500
 */
DEFUN (no_mpls_pw_mtu,
    no_mpls_pw_mtu_cmd,
    "no mtu",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);
    PW_VC_TYPE_NOT_ETH(vty, pl2vc->pwinfo.ac_type);

    pl2vc->pwinfo.mtu = 1500;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 控制字配置命令
 * @param[in ] : enable  - 使能控制字
 * @param[in ] : disable - 不使能控制字
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 10:09:00
 * @note       : pw 处于绑定状态不允许配置该命令，默认为 DISABLE
 */
DEFUN (mpls_pw_ctrl_word,
    mpls_pw_ctrl_word_cmd,
    "ctrl-word (enable | disable)",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    if (argv[0][0] == 'e')
    {
        pl2vc->pwinfo.ctrlword_flag = ENABLE;
    }
    else if (argv[0][0] == 'd')
    {
        pl2vc->pwinfo.ctrlword_flag = DISABLE;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 接入方式配置命令
 * @param[in ] : ethernet - ethernet 接入
 * @param[in ] : vlan     - vlan 接入
 * @param[in ] : raw      - raw 方式
 * @param[in ] : tag      - tag 方式
 * @param[in ] : 0x8100   - tpid
 * @param[in ] : 0x88a8   - tpid
 * @param[in ] : 0x9100   - tpid
 * @param[in ] : <0-4094> - vlan id
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 10:11:42
 * @note       : pw 处于绑定状态不允许配置该命令，默认为 ethernet raw
 */
DEFUN (mpls_pw_encapsulate,
    mpls_pw_encapsulate_cmd,
    "encapsulate (ethernet | vlan) (raw | tag) {tpid (0x8100|0x88a8|0x9100) | vlan <0-4094>}",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    /* argv[0]: encapsulate type */
    if (argv[0][0] == 'e')
    {
        if (argv[1][0] == 'r')
        {
            pl2vc->pwinfo.tag_flag = PW_ENCAP_ETHERNET_RAW;
            pl2vc->pwinfo.tpid     = 0;
            pl2vc->pwinfo.vlan     = 0;
        }
        else if (argv[1][0] == 't')
        {
            pl2vc->pwinfo.tag_flag = PW_ENCAP_ETHERNET_TAG;
        }
    }
    else if (argv[0][0] == 'v')
    {
        if (argv[1][0] == 'r')
        {
            pl2vc->pwinfo.tag_flag = PW_ENCAP_VLAN_RAW;
        }
        else if (argv[1][0] == 't')
        {
            pl2vc->pwinfo.tag_flag = PW_ENCAP_VLAN_TAG;
        }
    }

    if (NULL != argv[2])
    {
        pl2vc->pwinfo.tpid = strtol(argv[2], NULL, 16);
    }

    if (NULL != argv[3])
    {
        pl2vc->pwinfo.vlan = atoi(argv[3]);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 接入方式删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 10:15:45
 * @note       : pw 处于绑定状态不允许配置该命令，默认为 ethernet raw
 */
DEFUN (no_mpls_pw_encapsulate,
    no_mpls_pw_encapsulate_cmd,
    "no encapsulate",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    pl2vc->pwinfo.tag_flag = PW_ENCAP_ETHERNET_RAW;
    pl2vc->pwinfo.tpid     = 0;
    pl2vc->pwinfo.vlan     = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 帧数配置命令
 * @param[in ] : <1-40> - 帧数
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 10:16:31
 * @note       : pw 处于绑定状态不允许配置该命令，默认为 8
 */
DEFUN (mpls_pw_ces_frame,
    mpls_pw_ces_frame_cmd,
    "ces frame-num <1-40>",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);
    PW_VC_TYPE_NOT_TDM(vty, pl2vc->pwinfo.ac_type);

    pl2vc->pwinfo.ces.frame_num = atoi(argv[0]);

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 帧数删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 10:17:31
 * @note       : pw 处于绑定状态不允许配置该命令，默认为 8
 */
DEFUN (no_mpls_pw_ces_frame,
    no_mpls_pw_ces_frame_cmd,
    "no ces frame-num",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);
    PW_VC_TYPE_NOT_TDM(vty, pl2vc->pwinfo.ac_type);

    pl2vc->pwinfo.ces.frame_num = 8;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 隧道配置命令
 * @param[in ] : tunnel USP - tunnel 隧道号
 * @param[in ] : lsp NAME   - lsp 隧道名字
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 10:19:48
 * @note       : pw 处于绑定状态不允许配置该命令，与隧道策略不可同时配置
 */
DEFUN (mpls_pw_tunnel_bind,
    mpls_pw_tunnel_bind_cmd,
    "tunnel bind {interface tunnel USP | lsp NAME}",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct static_lsp *plsp  = NULL;
    uint32_t ifindex;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    if (pl2vc->tunl_index != 0)
    {
        vty_error_out(vty, "The tunnel have been configured !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ((ENABLE == pl2vc->ldp_tunnel)
        || (ENABLE == pl2vc->gre_tunnel)
        || (ENABLE == pl2vc->mplstp_tunnel)
        || (ENABLE == pl2vc->mplste_tunnel))
    {
        vty_error_out(vty, "Already specify tunnel policy, Can't configuration tunnel !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != argv[0])
    {
        ifindex = ifm_get_ifindex_by_name((char *)"tunnel", (char *)argv[0]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[0], VTY_NEWLINE);

            return CMD_WARNING;
        }

        pl2vc->tunl_index      = ifindex;
        pl2vc->pwinfo.nhp_type = NHP_TYPE_TUNNEL;
    }
    else if (NULL != argv[1])
    {
        if (1 != vty->config_read_flag)
        {
            plsp = static_lsp_lookup((uchar *)argv[1]);
            if (NULL == plsp)
            {
                vty_error_out(vty, "Specify the static lsp '%s' does not exist !%s", argv[1], VTY_NEWLINE);

                return CMD_WARNING;
            }

            if (plsp->direction != LSP_DIRECTION_INGRESS)
            {
                vty_error_out(vty, "Specify the static lsp '%s' is invalid !%s", argv[1], VTY_NEWLINE);

                return CMD_WARNING;
            }

            if (!ipv4_is_same_subnet(plsp->destip.addr.ipv4, pl2vc->peerip.addr.ipv4, plsp->destip.prefixlen))
            {
                vty_error_out(vty, "Peer ip and LSP dip don't match !%s", VTY_NEWLINE);

                return CMD_WARNING;
            }

            pl2vc->tunl_index      = plsp->lsp_index;
            pl2vc->pwinfo.nhp_type = NHP_TYPE_LSP;
            memcpy(pl2vc->lsp_name, plsp->name, NAME_STRING_LEN);
        }
        else
        {
            plsp = static_lsp_lookup((uchar *)argv[1]);
            if (NULL != plsp)
            {
                pl2vc->tunl_index = plsp->lsp_index;
            }

            pl2vc->pwinfo.nhp_type = NHP_TYPE_LSP;
            memcpy(pl2vc->lsp_name, argv[1], strlen(argv[1])+1);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 隧道删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 10:26:43
 * @note       : pw 处于绑定状态不允许配置该命令
 */
DEFUN (no_mpls_pw_tunnel_bind,
    no_mpls_pw_tunnel_bind_cmd,
    "no tunnel bind",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    memset(pl2vc->lsp_name, 0, NAME_STRING_LEN);
    pl2vc->tunl_index       = 0;
    pl2vc->pwinfo.admin_up  = LINK_DOWN;
    pl2vc->pwinfo.up_flag   = LINK_DOWN;
    pl2vc->pwinfo.nhp_type  = NHP_TYPE_INVALID;
    pl2vc->pwinfo.nhp_index = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 隧道策略配置命令
 * @param[in ] : ldp     - 使能 ldp 隧道策略
 * @param[in ] : mpls-tp - 使能 mpls-tp 隧道策略
 * @param[in ] : gre     - 使能 gre 隧道策略
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 10:28:16
 * @note       : pw 处于绑定状态不允许配置该命令
 * @note       : 隧道策略与静态隧道不允许同时配置，隧道策略可以多选
 */
DEFUN (mpls_pw_tunnel_policy,
    mpls_pw_tunnel_policy_cmd,
    "tunnel policy (ldp | mpls-tp | gre)",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    char ifname[NAME_STRING_LEN] = "";

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    if (0 != pl2vc->tunl_index)
    {
        ifm_get_name_by_ifindex(pl2vc->tunl_index, ifname);

        vty_error_out(vty, "Already specify %s, Can't configuration tunnel policy !%s",
                        ifname, VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ('l' == argv[0][0])
    {
        pl2vc->ldp_tunnel = ENABLE;
    }
    else if ('m' == argv[0][0])
    {
        pl2vc->mplstp_tunnel = ENABLE;
    }
    else if ('g' == argv[0][0])
    {
        pl2vc->gre_tunnel = ENABLE;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 隧道策略删除命令
 * @param[in ] : ldp     - 去使能 ldp 隧道策略
 * @param[in ] : mpls-tp - 去使能 mpls-tp 隧道策略
 * @param[in ] : gre     - 去使能 gre 隧道策略
 * @param[in ] : all     - 去使能全部隧道策略
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 11:16:32
 * @note       : pw 处于绑定状态不允许配置该命令
 */
DEFUN (no_mpls_pw_tunnel_policy,
    no_mpls_pw_tunnel_policy_cmd,
    "no tunnel policy (ldp | mpls-tp | gre | all)",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    if ('l' == argv[0][0])
    {
        pl2vc->ldp_tunnel = DISABLE;
    }
    else if ('m' == argv[0][0])
    {
        pl2vc->mplstp_tunnel = DISABLE;
    }
    else if ('g' == argv[0][0])
    {
        pl2vc->gre_tunnel = DISABLE;
    }
    else if ('a' == argv[0][0])
    {
        pl2vc->ldp_tunnel    = DISABLE;
        pl2vc->mplste_tunnel = DISABLE;
        pl2vc->mplstp_tunnel = DISABLE;
        pl2vc->gre_tunnel    = DISABLE;
    }

    return CMD_SUCCESS;
}

/**
 * @brief      : 多段 pw 配置命令
 * @param[in ] : NAME - 指定另一段 pw 的名字
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月28日 11:33:50
 * @note       : pw 处于绑定状态不允许配置该命令
 */
DEFUN (mpls_pw_switch_pw,
    mpls_pw_switch_pw_cmd,
    "switch pw NAME",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc        = (struct l2vc_entry *)vty->index;
    struct l2vc_entry *pl2vc_switch = NULL;
    int ret = 0;

    MPLS_LOG_DEBUG();
    
    /* 交换前段 PW 配置检测 */
    if (PW_ALREADY_BIND_AC(&pl2vc->pwinfo)
        || PW_ALREADY_BIND_VSI(&pl2vc->pwinfo)
        || PW_ALREADY_BIND_MSPW(&pl2vc->pwinfo))
    {
        vty_error_out(vty, "The PW '%s' has been bound !%s", pl2vc->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (pl2vc->mplsoam_id > 0)
    {
        mplsoam_session_disable(pl2vc->mplsoam_id);

        pl2vc->mplsoam_id = 0;
    }

    PW_LABEL_NOT_CONFIG(vty, pl2vc);

    if (PW_IS_INCOMPLETE(pl2vc))
    {
        vty_error_out(vty, "The PW '%s' is incomplete, Please check the configuration !%s", pl2vc->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 交换后段 PW 配置检测 */
    pl2vc_switch = l2vc_lookup((uchar *)argv[0]);
    if (pl2vc_switch == NULL)
    {
        vty_error_out(vty, "The PW '%s' specified not exist !%s", argv[0], VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (pl2vc->pwinfo.pwindex == pl2vc_switch->pwinfo.pwindex)
    {
        vty_error_out(vty, "Can't specify their !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (pl2vc_switch->mplsoam_id > 0)
    {
        mplsoam_session_disable(pl2vc_switch->mplsoam_id);

        pl2vc_switch->mplsoam_id = 0;
    }

    if (PW_ALREADY_BIND_AC(&pl2vc_switch->pwinfo)
        || PW_ALREADY_BIND_VSI(&pl2vc_switch->pwinfo)
        || PW_ALREADY_BIND_MSPW(&pl2vc_switch->pwinfo))
    {
        vty_error_out(vty, "The PW '%s' has been bound !%s", pl2vc_switch->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    PW_LABEL_NOT_CONFIG(vty, pl2vc_switch);

    if (PW_IS_INCOMPLETE(pl2vc_switch))
    {
        vty_error_out(vty, "The PW '%s' is incomplete, please check the configuration !%s", pl2vc_switch->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 前后段 PW 不能绑定到同一 tunnel */
    if ((0 != pl2vc->tunl_index) && (pl2vc->tunl_index == pl2vc_switch->tunl_index))
    {
        vty_error_out(vty, "Switch pws can not select the same tunnel !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_PROTO_IS_SVC(&pl2vc->pwinfo))
    {
        /* 非配置恢复过程，检查 tunnel 限制 */
        if (1 != vty->config_read_flag)
        {
            ret = l2vc_select_tunnel(pl2vc);
            if (ERRNO_SUCCESS != ret)
            {
                if (ERRNO_NOT_FOUND == ret)
                {
                    vty_error_out(vty, "The L2VC %s select the tunnel failed for the tunnel specified not exist!%s", pl2vc->name, VTY_NEWLINE);

                    return CMD_WARNING;
                }

                vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else
        {
            l2vc_select_tunnel(pl2vc);
        }
    }

    if (PW_PROTO_IS_SVC(&pl2vc_switch->pwinfo))
    {
        /* 非配置恢复过程，检查 tunnel 限制 */
        if (1 != vty->config_read_flag)
        {
            ret = l2vc_select_tunnel(pl2vc_switch);
            if (ERRNO_SUCCESS != ret)
            {
                if (ERRNO_NOT_FOUND == ret)
                {
                    vty_error_out(vty, "The L2VC %s select the tunnel failed for the tunnel specified not exist!%s",
                                pl2vc_switch->name, VTY_NEWLINE);

                    return CMD_WARNING;
                }

                vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else
        {
            l2vc_select_tunnel(pl2vc_switch);
        }
    }

    ret = pw_get_lsp_index(&pl2vc->pwinfo);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW malloc private lsp failed !%s", VTY_NEWLINE);
        goto fail;
    }

    pl2vc->pswitch_pw                = pl2vc_switch;
    pl2vc->pwinfo.mspw_index         = pl2vc_switch->pwinfo.pwindex;

    pl2vc_switch->pwinfo.ingress_lsp = pl2vc->pwinfo.egress_lsp;
    pl2vc_switch->pwinfo.egress_lsp  = pl2vc->pwinfo.ingress_lsp;
    pl2vc_switch->pswitch_pw         = pl2vc;
    pl2vc_switch->pwinfo.mspw_index  = pl2vc->pwinfo.pwindex;

    /* 配置为分段、将 ac-type 置为无效 */
    pl2vc->pwinfo.ac_type            = AC_TYPE_INVALID;
    pl2vc->pwinfo.pw_type            = PW_TYPE_MASTER;
    pl2vc_switch->pwinfo.ac_type     = AC_TYPE_INVALID;
    pl2vc_switch->pwinfo.pw_type     = PW_TYPE_MASTER;

    ret = mpls_pw_pwinfo_download(&pl2vc_switch->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "L2VC send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    ret = mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "L2VC send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    if ((LINK_UP == pl2vc->pwinfo.admin_up) || (LINK_UP == pl2vc_switch->pwinfo.admin_up))
    {
        ret = pw_add_lsp_transit(pl2vc, pl2vc_switch);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "PW add transmit lsp failed !%s", VTY_NEWLINE);
            goto fail;
        }
    }
    else
    {
        if (LINK_DOWN == pl2vc->pwinfo.admin_up)
        {
            l2vc_alarm_process(OPCODE_DOWN, pl2vc->pwinfo.pwindex);
        }

        if (LINK_DOWN == pl2vc_switch->pwinfo.admin_up)
        {
            l2vc_alarm_process(OPCODE_DOWN, pl2vc_switch->pwinfo.pwindex);
        }
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_add_martini(pl2vc);
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc_switch->pwinfo))
    {
        l2vc_add_martini(pl2vc_switch);
    }

    return CMD_SUCCESS;

fail:

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_delete_martini(pl2vc);
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc_switch->pwinfo))
    {
        l2vc_delete_martini(pl2vc_switch);
    }

    l2vc_unbind_lsp_tunnel(pl2vc);
    l2vc_unbind_lsp_tunnel(pl2vc_switch);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc->pwinfo.nhp_index), pl2vc);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc_switch->pwinfo.nhp_index), pl2vc_switch);
    pw_free_lsp_index(&pl2vc->pwinfo);

    pl2vc->pswitch_pw                = NULL;
    pl2vc->pwinfo.mspw_index         = 0;

    pl2vc_switch->pwinfo.ingress_lsp = 0;
    pl2vc_switch->pwinfo.egress_lsp  = 0;
    pl2vc_switch->pswitch_pw         = NULL;
    pl2vc_switch->pwinfo.mspw_index  = 0;

    pl2vc->pwinfo.ac_type            = AC_TYPE_ETH;
    pl2vc_switch->pwinfo.ac_type     = AC_TYPE_ETH;

    return CMD_WARNING;
}


/**
 * @brief      : 多段 pw 删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 8:53:50
 * @note       :
 */
DEFUN (no_mpls_pw_switch_pw,
    no_mpls_pw_switch_pw_cmd,
    "no switch pw",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc        = (struct l2vc_entry *)vty->index;
    struct l2vc_entry *pl2vc_switch = NULL;
    struct pw_info    *ppwinfo      = NULL;
    MPLS_LOG_DEBUG();
    if ((0 == pl2vc->pwinfo.mspw_index) || (NULL == pl2vc->pswitch_pw))
    {
        pl2vc->pwinfo.mspw_index = 0;
        pl2vc->pswitch_pw        = NULL;

        return CMD_SUCCESS;
    }

    ppwinfo = pw_lookup(pl2vc->pwinfo.mspw_index);
    if (NULL == ppwinfo)
    {
        pl2vc->pwinfo.mspw_index = 0;
        pl2vc->pswitch_pw        = NULL;

        return CMD_SUCCESS;
    }

	if(pl2vc->mplsoam_id != 0)
	{
	//	mplsoam_session_unbind_service(pl2vc->mplsoam_id, pl2vc);
		mpls_pw_unbind_oam_mip(pl2vc->mplsoam_id, pl2vc);
	}

    pl2vc_switch                 = pl2vc->pswitch_pw;
    pl2vc->pwinfo.ac_type        = AC_TYPE_ETH;
    pl2vc_switch->pwinfo.ac_type = AC_TYPE_ETH;

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_delete_martini(pl2vc);
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc_switch->pwinfo))
    {
        l2vc_delete_martini(pl2vc_switch);
    }

    l2vc_unbind_lsp_tunnel(pl2vc);
    l2vc_unbind_lsp_tunnel(pl2vc_switch);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc->pwinfo.nhp_index), pl2vc);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc_switch->pwinfo.nhp_index), pl2vc_switch);

    mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_DELETE);
    mpls_pw_pwinfo_download(&pl2vc_switch->pwinfo, OPCODE_DELETE);

    pw_delete_lsp(&pl2vc->pwinfo);
    pw_delete_lsp(&pl2vc_switch->pwinfo);
    pw_free_lsp_index(&pl2vc->pwinfo);

    pl2vc->pswitch_pw               = NULL;
    pl2vc->pwinfo.mspw_index        = 0;
    pl2vc_switch->pswitch_pw        = NULL;
    pl2vc_switch->pwinfo.mspw_index = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 关联 vsi 配置命令
 * @param[in ] : <1-1024> - vsi 实例号
 * @param[in ] : upe      - pw 在 vsi 中角色，不使用水平分割
 * @param[in ] : spe      - pw 在 vsi 中角色，使用水平分割
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 8:57:46
 * @note       :
 */
DEFUN (mpls_pw_switch_vsi,
    mpls_pw_switch_vsi_cmd,
    "switch vsi <1-1024> (upe | spe)",
    MPLS_CLI_INFO)
{
    struct vsi_entry  *pvsi       = NULL;
    struct l2vc_entry *pl2vc      = (struct l2vc_entry *)vty->index;
    uint16_t vsi_id = 0;
    uint8_t  vsi_pe = 0;
    int      ret    = ERRNO_SUCCESS;
    MPLS_LOG_DEBUG();
    /* argv[0]: vsi id */
    vsi_id = atoi(argv[0]);

    /* argv[1]: pe */
    if (argv[1][0] == 'u')
    {
        vsi_pe = VPLS_TYPE_UPE;
    }
    else if (argv[1][0] == 's')
    {
        vsi_pe = VPLS_TYPE_SPE;
    }

    pvsi = vsi_lookup(vsi_id);
    if (NULL == pvsi)
    {
        vty_error_out(vty, "The VSI specified not exist !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_ALREADY_BIND_AC(&pl2vc->pwinfo)
        || PW_ALREADY_BIND_VSI(&pl2vc->pwinfo)
        || PW_ALREADY_BIND_MSPW(&pl2vc->pwinfo))
    {
        vty_error_out(vty, "The PW '%s' has been bound !%s", pl2vc->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    PW_LABEL_NOT_CONFIG(vty, pl2vc);
    PW_VC_TYPE_NOT_ETH(vty, pl2vc->pwinfo.ac_type);

    if (PW_IS_INCOMPLETE(pl2vc))
    {
        vty_error_out(vty, "PW is incomplete, please check the configuration !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_PROTO_IS_SVC(&pl2vc->pwinfo))
    {
        if (1 != vty->config_read_flag)
        {
            ret = l2vc_select_tunnel(pl2vc);
            if (ERRNO_SUCCESS != ret)
            {
                if (ERRNO_NOT_FOUND == ret)
                {
                    vty_error_out(vty, "The L2VC %s select the tunnel failed for the tunnel specified not exist!%s",
                                pl2vc->pwinfo.name, VTY_NEWLINE);

                    return CMD_WARNING;
                }

                vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else
        {
            l2vc_select_tunnel(pl2vc);
        }
    }

    ret = pw_get_lsp_index(&pl2vc->pwinfo);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW malloc private lsp failed !%s", VTY_NEWLINE);
        goto fail;
    }

    pl2vc->pwinfo.vpls.upe = vsi_pe;
    pl2vc->pwinfo.pw_type  = PW_TYPE_MASTER;

    ret = vsi_add_pw(pvsi, pl2vc);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW join a vpls instance failed !%s", VTY_NEWLINE);
        goto fail;
    }

    ret = mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "L2VC send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    ret = vsi_pwinfo_download(&pl2vc->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "VPLS send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    vsi_pw_name_download(pl2vc, OPCODE_ADD);

    if (LINK_UP == pl2vc->pwinfo.admin_up)
    {
        ret = pw_add_lsp(pl2vc);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "PW add private lsp failed for malloc failed !%s", VTY_NEWLINE);
            goto fail;
        }
    }
    else
    {
        l2vc_alarm_process(OPCODE_DOWN, pl2vc->pwinfo.pwindex);
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_add_martini(pl2vc);
    }

    return CMD_SUCCESS;

fail:

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_delete_martini(pl2vc);
    }

    l2vc_unbind_lsp_tunnel(pl2vc);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc->pwinfo.nhp_index), pl2vc);
    vsi_delete_pw(pvsi, pl2vc);
    pw_free_lsp_index(&pl2vc->pwinfo);

    return CMD_WARNING;
}


/**
 * @brief      : pw 关联 vsi 删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 9:07:58
 * @note       :
 */
DEFUN (no_mpls_pw_switch_vsi,
    no_mpls_pw_switch_vsi_cmd,
    "no switch vsi",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct vsi_entry  *pvsi  = NULL;
    int ret;
    MPLS_LOG_DEBUG();
    if (pl2vc->pwinfo.vpls.vsi_id == 0)
    {
        return CMD_SUCCESS;
    }

    if (0 != pl2vc->pwinfo.backup_index)
    {
        vty_error_out(vty, "Please delete the master and backup PW at the same time !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != pl2vc->mac_list)
    {
        vty_error_out(vty, "Please clear the static MAC at first !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_delete_martini(pl2vc);
    }

    l2vc_unbind_lsp_tunnel(pl2vc);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc->pwinfo.nhp_index), pl2vc);

    pvsi = vsi_lookup(pl2vc->pwinfo.vpls.vsi_id);
    if (pvsi == NULL)
    {
        pl2vc->pwinfo.vpls.vsi_id = 0;

        return CMD_SUCCESS;
    }

    vsi_pw_name_download(pl2vc, OPCODE_DELETE);

    ret = vsi_pwinfo_download(&pl2vc->pwinfo, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    vsi_delete_pw(pvsi, pl2vc);

    ret = mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    pw_delete_lsp(&pl2vc->pwinfo);
    pw_free_lsp_index(&pl2vc->pwinfo);

    return CMD_SUCCESS;
}


/**
 * @brief      : 配置 vpls 保护命令
 * @param[in ] : vsi <1-1024> - vsi id
 * @param[in ] : pw NAME      - 备 pw 名称
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年4月11日 14:14:17
 * @note       : 配置保护功能需在 pw 视图下，同时配置主备。
 */
DEFUN (mpls_pw_switch_vsi_backup,
    mpls_pw_switch_vsi_backup_cmd,
    "switch vsi <1-1024> backup pw NAME",
    MPLS_CLI_INFO)
{
    struct vsi_entry  *pvsi         = NULL;
    struct l2vc_entry *pl2vc        = (struct l2vc_entry *)vty->index;
    struct l2vc_entry *pl2vc_backup = NULL;
    uint16_t vsi_id = 0;
    int      ret    = ERRNO_SUCCESS;
    MPLS_LOG_DEBUG();
    /* argv[0]: vsi id */
    vsi_id = atoi(argv[0]);

    pvsi = vsi_lookup(vsi_id);
    if (NULL == pvsi)
    {
        vty_error_out(vty, "The VSI '%u' does not exist !%s", vsi_id, VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 主 pw 检测 */
    if (PW_ALREADY_BIND_AC(&pl2vc->pwinfo)
        || PW_ALREADY_BIND_VSI(&pl2vc->pwinfo)
        || PW_ALREADY_BIND_MSPW(&pl2vc->pwinfo))
    {
        vty_error_out(vty, "The PW '%s' has been bound !%s", pl2vc->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    PW_LABEL_NOT_CONFIG(vty, pl2vc);
    PW_VC_TYPE_NOT_ETH(vty, pl2vc->pwinfo.ac_type);

    if (PW_IS_INCOMPLETE(pl2vc))
    {
        vty_error_out(vty, "PW '%s' is incomplete, please check the configuration !%s",
                    pl2vc->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 备 pw 检测 */
    pl2vc_backup = l2vc_lookup((uchar *)argv[1]);
    if (NULL == pl2vc_backup)
    {
        vty_error_out(vty, "The PW '%s' specified not exist !%s", argv[1], VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (pl2vc->pwinfo.pwindex == pl2vc_backup->pwinfo.pwindex)
    {
        vty_error_out(vty, "Can't specify their !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_ALREADY_BIND_AC(&pl2vc_backup->pwinfo)
        || PW_ALREADY_BIND_VSI(&pl2vc_backup->pwinfo)
        || PW_ALREADY_BIND_MSPW(&pl2vc_backup->pwinfo))
    {
        vty_error_out(vty, "The PW '%s' has been bound !%s", pl2vc_backup->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    PW_LABEL_NOT_CONFIG(vty, pl2vc_backup);
    PW_VC_TYPE_NOT_ETH(vty, pl2vc_backup->pwinfo.ac_type);

    if (PW_IS_INCOMPLETE(pl2vc_backup))
    {
        vty_error_out(vty, "The PW '%s' is incomplete, please check the configuration !%s", pl2vc_backup->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_PROTO_IS_SVC(&pl2vc->pwinfo))
    {
        /* 非配置恢复过程，检查 tunnel 限制 */
        if (1 != vty->config_read_flag)
        {
            ret = l2vc_select_tunnel(pl2vc);
            if (ERRNO_SUCCESS != ret)
            {
                if (ERRNO_NOT_FOUND == ret)
                {
                    vty_error_out(vty, "The L2VC %s select the tunnel failed for the tunnel specified not exist!%s", pl2vc->name, VTY_NEWLINE);

                    return CMD_WARNING;
                }

                vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else
        {
            l2vc_select_tunnel(pl2vc);
        }
    }

    if (PW_PROTO_IS_SVC(&pl2vc_backup->pwinfo))
    {
        /* 非配置恢复过程，检查 tunnel 限制 */
        if (1 != vty->config_read_flag)
        {
            ret = l2vc_select_tunnel(pl2vc_backup);
            if (ERRNO_SUCCESS != ret)
            {
                if (ERRNO_NOT_FOUND == ret)
                {
                    vty_error_out(vty, "The L2VC %s select the tunnel failed for the tunnel specified not exist!%s", pl2vc_backup->name, VTY_NEWLINE);

                    return CMD_WARNING;
                }

                vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else
        {
            l2vc_select_tunnel(pl2vc_backup);
        }
    }

    ret = pw_get_lsp_index(&pl2vc->pwinfo);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW malloc private lsp failed !%s", VTY_NEWLINE);
        goto fail;
    }

    ret = pw_get_lsp_index(&pl2vc_backup->pwinfo);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW malloc private lsp failed !%s", VTY_NEWLINE);
        goto fail;
    }

    pl2vc->ppw_backup                 = pl2vc_backup;
    pl2vc->pwinfo.backup_index        = pl2vc_backup->pwinfo.pwindex;
    pl2vc_backup->ppw_backup          = pl2vc;
    pl2vc_backup->pwinfo.backup_index = pl2vc->pwinfo.pwindex;

    pl2vc->pwinfo.pw_type             = PW_TYPE_MASTER;
    pl2vc_backup->pwinfo.pw_type      = PW_TYPE_BACKUP;

    ret = vsi_add_pw(pvsi, pl2vc);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW join a vpls instance failed !%s", VTY_NEWLINE);
        goto fail;
    }

    ret = vsi_add_pw(pvsi, pl2vc_backup);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW join a vpls instance failed !%s", VTY_NEWLINE);
        goto fail;
    }

    ret = mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "L2VC send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    ret = mpls_pw_pwinfo_download(&pl2vc_backup->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "L2VC send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    ret = vsi_pwinfo_download(&pl2vc->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "VPLS send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    ret = vsi_pwinfo_download(&pl2vc_backup->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "VPLS send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    vsi_pw_name_download(pl2vc, OPCODE_ADD);
    vsi_pw_name_download(pl2vc_backup, OPCODE_ADD);

    if (LINK_UP == pl2vc->pwinfo.admin_up)
    {
        ret = pw_add_lsp(pl2vc);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "PW add private lsp failed for malloc failed !%s", VTY_NEWLINE);
            goto fail;
        }
    }
    else
    {
        l2vc_alarm_process(OPCODE_DOWN, pl2vc->pwinfo.pwindex);
    }

    if (LINK_UP == pl2vc_backup->pwinfo.admin_up)
    {
        ret = pw_add_lsp(pl2vc_backup);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "PW add private lsp failed for malloc failed !%s", VTY_NEWLINE);
            goto fail;
        }
    }
    else
    {
        l2vc_alarm_process(OPCODE_DOWN, pl2vc_backup->pwinfo.pwindex);
    }

    vsi_set_pw_status(pl2vc);

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_add_martini(pl2vc);
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc_backup->pwinfo))
    {
        l2vc_add_martini(pl2vc_backup);
    }

    return CMD_SUCCESS;

fail:

    if (PW_PROTO_IS_MARTINI(&pl2vc_backup->pwinfo))
    {
        l2vc_delete_martini(pl2vc_backup);
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_delete_martini(pl2vc);
    }

    l2vc_unbind_lsp_tunnel(pl2vc_backup);
    l2vc_unbind_lsp_tunnel(pl2vc);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc_backup->pwinfo.nhp_index), pl2vc_backup);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc->pwinfo.nhp_index), pl2vc);
    vsi_delete_pw(pvsi, pl2vc_backup);
    vsi_delete_pw(pvsi, pl2vc);
    pw_free_lsp_index(&pl2vc_backup->pwinfo);
    pw_free_lsp_index(&pl2vc->pwinfo);

    return CMD_WARNING;
}


/**
 * @brief      : 配置 vpls 保护命令
 * @param[in ] : vsi <1-1024> - vsi id
 * @param[in ] : pw NAME      - 主 pw 名称
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年4月11日 14:15:13
 * @note       : 配置保护功能需在 pw 视图下，同时配置主备。
 */
DEFUN (mpls_pw_switch_vsi_master,
    mpls_pw_switch_vsi_master_cmd,
    "switch vsi <1-1024> master pw NAME",
    MPLS_CLI_INFO)
{
    struct vsi_entry  *pvsi         = NULL;
    struct l2vc_entry *pl2vc        = (struct l2vc_entry *)vty->index;
    struct l2vc_entry *pl2vc_master = NULL;
    uint16_t vsi_id = 0;
    int      ret    = ERRNO_SUCCESS;

    MPLS_LOG_DEBUG();
    /* argv[0]: vsi id */
    vsi_id = atoi(argv[0]);

    pvsi = vsi_lookup(vsi_id);
    if (NULL == pvsi)
    {
        vty_error_out(vty, "The VSI '%u' does not exist !%s", vsi_id, VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 备 pw 检测 */
    if (PW_ALREADY_BIND_AC(&pl2vc->pwinfo)
        || PW_ALREADY_BIND_VSI(&pl2vc->pwinfo)
        || PW_ALREADY_BIND_MSPW(&pl2vc->pwinfo))
    {
        vty_error_out(vty, "The PW '%s' has been bound !%s", pl2vc->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    PW_LABEL_NOT_CONFIG(vty, pl2vc);
    PW_VC_TYPE_NOT_ETH(vty, pl2vc->pwinfo.ac_type);

    if (PW_IS_INCOMPLETE(pl2vc))
    {
        vty_error_out(vty, "PW '%s' is incomplete, please check the configuration !%s",
                    pl2vc->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 主 pw 检测 */
    pl2vc_master = l2vc_lookup((uchar *)argv[1]);
    if (NULL == pl2vc_master)
    {
        vty_error_out(vty, "The PW '%s' specified not exist !%s", argv[1], VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (pl2vc->pwinfo.pwindex == pl2vc_master->pwinfo.pwindex)
    {
        vty_error_out(vty, "Can't specify their !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_ALREADY_BIND_AC(&pl2vc_master->pwinfo)
        || PW_ALREADY_BIND_VSI(&pl2vc_master->pwinfo)
        || PW_ALREADY_BIND_MSPW(&pl2vc_master->pwinfo))
    {
        vty_error_out(vty, "The PW '%s' has been bound !%s", pl2vc_master->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    PW_LABEL_NOT_CONFIG(vty, pl2vc_master);
    PW_VC_TYPE_NOT_ETH(vty, pl2vc_master->pwinfo.ac_type);

    if (PW_IS_INCOMPLETE(pl2vc_master))
    {
        vty_error_out(vty, "The PW '%s' is incomplete, please check the configuration !%s", pl2vc_master->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_PROTO_IS_SVC(&pl2vc->pwinfo))
    {
        /* 非配置恢复过程，检查 tunnel 限制 */
        if (1 != vty->config_read_flag)
        {
            ret = l2vc_select_tunnel(pl2vc);
            if (ERRNO_SUCCESS != ret)
            {
                if (ERRNO_NOT_FOUND == ret)
                {
                    vty_error_out(vty, "The L2VC %s select the tunnel failed for the tunnel specified not exist!%s",
                                pl2vc->name, VTY_NEWLINE);

                    return CMD_WARNING;
                }

                vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else
        {
            l2vc_select_tunnel(pl2vc);
        }
    }

    if (PW_PROTO_IS_SVC(&pl2vc_master->pwinfo))
    {
        /* 非配置恢复过程，检查 tunnel 限制 */
        if (1 != vty->config_read_flag)
        {
            ret = l2vc_select_tunnel(pl2vc_master);
            if (ERRNO_SUCCESS != ret)
            {
                if (ERRNO_NOT_FOUND == ret)
                {
                    vty_error_out(vty, "The L2VC %s select the tunnel failed for the tunnel specified not exist!%s",
                                pl2vc_master->name, VTY_NEWLINE);

                    return CMD_WARNING;
                }

                vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else
        {
            l2vc_select_tunnel(pl2vc_master);
        }
    }

    ret = pw_get_lsp_index(&pl2vc_master->pwinfo);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW malloc private lsp failed !%s", VTY_NEWLINE);
        goto fail;
    }

    ret = pw_get_lsp_index(&pl2vc->pwinfo);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW malloc private lsp failed !%s", VTY_NEWLINE);
        goto fail;
    }

    pl2vc->ppw_backup                 = pl2vc_master;
    pl2vc->pwinfo.backup_index        = pl2vc_master->pwinfo.pwindex;
    pl2vc_master->ppw_backup          = pl2vc;
    pl2vc_master->pwinfo.backup_index = pl2vc->pwinfo.pwindex;

    pl2vc->pwinfo.pw_type             = PW_TYPE_BACKUP;
    pl2vc_master->pwinfo.pw_type      = PW_TYPE_MASTER;

    ret = vsi_add_pw(pvsi, pl2vc_master);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW join a vpls instance failed !%s", VTY_NEWLINE);
        goto fail;
    }

    ret = vsi_add_pw(pvsi, pl2vc);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW join a vpls instance failed !%s", VTY_NEWLINE);
        goto fail;
    }

    ret = mpls_pw_pwinfo_download(&pl2vc_master->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "L2VC send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    ret = mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "L2VC send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    ret = vsi_pwinfo_download(&pl2vc_master->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "VPLS send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    ret = vsi_pwinfo_download(&pl2vc->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "VPLS send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    vsi_pw_name_download(pl2vc_master, OPCODE_ADD);
    vsi_pw_name_download(pl2vc, OPCODE_ADD);

    if (LINK_UP == pl2vc_master->pwinfo.admin_up)
    {
        ret = pw_add_lsp(pl2vc_master);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "PW add private lsp failed for malloc failed !%s", VTY_NEWLINE);
            goto fail;
        }
    }
    else
    {
        l2vc_alarm_process(OPCODE_DOWN, pl2vc_master->pwinfo.pwindex);
    }

    if (LINK_UP == pl2vc->pwinfo.admin_up)
    {
        ret = pw_add_lsp(pl2vc);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "PW add private lsp failed for malloc failed !%s", VTY_NEWLINE);
            goto fail;
        }
    }
    else
    {
        l2vc_alarm_process(OPCODE_DOWN, pl2vc->pwinfo.pwindex);
    }

    vsi_set_pw_status(pl2vc_master);

    if (PW_PROTO_IS_MARTINI(&pl2vc_master->pwinfo))
    {
        l2vc_add_martini(pl2vc_master);
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_add_martini(pl2vc);
    }

    return CMD_SUCCESS;

fail:

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_delete_martini(pl2vc);
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc_master->pwinfo))
    {
        l2vc_delete_martini(pl2vc_master);
    }

    l2vc_unbind_lsp_tunnel(pl2vc);
    l2vc_unbind_lsp_tunnel(pl2vc_master);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc->pwinfo.nhp_index), pl2vc);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc_master->pwinfo.nhp_index), pl2vc_master);
    vsi_delete_pw(pvsi, pl2vc);
    vsi_delete_pw(pvsi, pl2vc_master);
    pw_free_lsp_index(&pl2vc->pwinfo);
    pw_free_lsp_index(&pl2vc_master->pwinfo);

    return CMD_WARNING;
}


/**
 * @brief      : 删除 vpls 保护命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年4月11日 16:18:35
 * @note       :
 */
DEFUN (no_mpls_pw_switch_vsi_backup,
    no_mpls_pw_switch_vsi_backup_cmd,
    "no switch vsi backup pw",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc        = (struct l2vc_entry *)vty->index;
    struct l2vc_entry *pl2vc_backup = pl2vc->ppw_backup;
    struct vsi_entry  *pvsi         = NULL;
    int ret;
    MPLS_LOG_DEBUG();
    if (pl2vc->pwinfo.vpls.vsi_id == 0)
    {
        return CMD_SUCCESS;
    }

    if (NULL == pl2vc_backup)
    {
        vty_error_out(vty, "The backup pw dose not exist !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_TYPE_MASTER == pl2vc_backup->pwinfo.pw_type)
    {
        vty_error_out(vty, "The specified PW is backup, command is not support !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != pl2vc->mac_list)
    {
        vty_error_out(vty, "Please clear the pw '%s' static MAC at first !%s",
                        pl2vc->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != pl2vc_backup->mac_list)
    {
        vty_error_out(vty, "Please clear the pw '%s' static MAC at first !%s",
                        pl2vc_backup->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc_backup->pwinfo))
    {
        l2vc_delete_martini(pl2vc_backup);
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_delete_martini(pl2vc);
    }

    l2vc_unbind_lsp_tunnel(pl2vc_backup);
    l2vc_unbind_lsp_tunnel(pl2vc);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc_backup->pwinfo.nhp_index), pl2vc_backup);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc->pwinfo.nhp_index), pl2vc);

    pvsi = vsi_lookup(pl2vc->pwinfo.vpls.vsi_id);
    if (pvsi == NULL)
    {
        pl2vc->pwinfo.vpls.vsi_id        = 0;
        pl2vc_backup->pwinfo.vpls.vsi_id = 0;

        return CMD_SUCCESS;
    }

    vsi_pw_name_download(pl2vc, OPCODE_DELETE);
    vsi_pw_name_download(pl2vc_backup, OPCODE_DELETE);

    ret = vsi_pwinfo_download(&pl2vc_backup->pwinfo, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = vsi_pwinfo_download(&pl2vc->pwinfo, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    vsi_delete_pw(pvsi, pl2vc_backup);
    vsi_delete_pw(pvsi, pl2vc);

    ret = mpls_pw_pwinfo_download(&pl2vc_backup->pwinfo, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    pw_delete_lsp(&pl2vc_backup->pwinfo);
    pw_delete_lsp(&pl2vc->pwinfo);
    pw_free_lsp_index(&pl2vc_backup->pwinfo);
    pw_free_lsp_index(&pl2vc->pwinfo);

    pl2vc->ppw_backup                 = NULL;
    pl2vc->pwinfo.backup_index        = 0;
    pl2vc_backup->ppw_backup          = NULL;
    pl2vc_backup->pwinfo.backup_index = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 回切模式及回切时延配置命令
 * @param[in ] : non-failback - 不回切模式
 * @param[in ] : failback     - 回切模式
 * @param[in ] : wtr <0-3600> - 回切时延
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 9:11:38
 * @note       :
 */
DEFUN (mpls_interface_pw_failback,
    mpls_interface_pw_failback_cmd,
    "mpls pw backup {non-failback | failback | wtr <0-3600>}",
    MPLS_CLI_INFO)
{
    struct mpls_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    pif = mpls_if_lookup(ifindex);
    if ((NULL == pif) || (NULL == pif->ppw_master))
    {
        vty_error_out(vty, "Please bind PW first !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != argv[0])
    {
        pif->ppw_master->failback = FAILBACK_DISABLE;
    }
    else if (NULL != argv[1])
    {
        pif->ppw_master->failback = FAILBACK_ENABLE;
        pif->ppw_master->wtr      = 30;

        if (NULL != argv[2])
        {
            pif->ppw_master->wtr = atoi(argv[2]);
        }

        pw_set_pw_status(pif);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 绑定接口配置命令
 * @param[in ] : NAME   - pw 名字
 * @param[in ] : backup - 配置备 pw
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 9:13:29
 * @note       :
 */
DEFUN (mpls_interface_bind_pw,
    mpls_if_bind_pw_cmd,
    "mpls l2vpn pw NAME [backup]",
    MPLS_CLI_INFO)
{
    struct mpls_if    *pmpls_if = NULL;
    struct pw_info    *ppwinfo  = NULL;
    struct l2vc_entry *pl2vc    = NULL;
    enum MPLSIF_INFO   info;
    enum PW_TYPE_E     pw_type;
    uint32_t           ifindex  = (uint32_t)vty->index;
    int                ret      = ERRNO_SUCCESS;

    MPLS_LOG_DEBUG();
    /* argv[1]: 绑定主或备 */
    if (argv[1] == NULL)
    {
        pw_type = PW_TYPE_MASTER;
        info    = MPLSIF_INFO_MASTER_PW;
    }
    else
    {
        pw_type = PW_TYPE_BACKUP;
        info    = MPLSIF_INFO_SLAVE_PW;
    }

    pmpls_if = mpls_if_get(ifindex);
    if (NULL == pmpls_if)
    {
        vty_error_out(vty, "Failed to get the mpls interface !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (IFM_TYPE_IS_METHERNET(ifindex) || IFM_TYPE_IS_TRUNK(ifindex))
    {
        if (pmpls_if->mode != IFNET_MODE_L2)
        {
            vty_error_out(vty, "Only a l2 layer interface can be a ac for a pw!%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else if (IFM_TYPE_IS_TDM(ifindex))
    {
        if (pmpls_if->tdm_mode == MPLS_IF_CES_MODE_INVALID)
        {
            vty_error_out(vty, "A TDM interface must config mode before it is used!%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    pl2vc = l2vc_lookup((uchar *)argv[0]);
    if (NULL == pl2vc)
    {
        vty_error_out(vty, "The pw specified is not exist !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_ALREADY_BIND_AC(&pl2vc->pwinfo)
        || PW_ALREADY_BIND_VSI(&pl2vc->pwinfo)
        || PW_ALREADY_BIND_MSPW(&pl2vc->pwinfo))
    {
        vty_error_out(vty, "The PW '%s' is binding !%s", pl2vc->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    PW_LABEL_NOT_CONFIG(vty, pl2vc);
    PW_VC_TYPE_NOT_CONFIG(vty, pl2vc->pwinfo.ac_type);

    if (PW_IS_INCOMPLETE(pl2vc))
    {
        vty_error_out(vty, "L2VC is incomplete, please check the configuration !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (IFM_TYPE_IS_METHERNET(ifindex) && (pl2vc->pwinfo.ac_type != AC_TYPE_ETH))
    {
        vty_error_out(vty, "The actype on the interface is not match with the PW has specified !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }
    else if (IFM_TYPE_IS_TDM(ifindex) && (pl2vc->pwinfo.ac_type != AC_TYPE_TDM))
    {
        vty_error_out(vty, "The actype on the interface is not match with the PW has specified !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_PROTO_IS_SVC(&pl2vc->pwinfo))
    {
        /* 非配置恢复过程保证 tunnel 存在 */
        if (1 != vty->config_read_flag)
        {
            ret = l2vc_select_tunnel(pl2vc);
            if (ERRNO_SUCCESS != ret)
            {
                if (ERRNO_NOT_FOUND == ret)
                {
                    vty_error_out(vty, "L2VC select the tunnel failed for the tunnel specified not exist!%s",
                                VTY_NEWLINE);

                    return CMD_WARNING;
                }

                vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else
        {
            l2vc_select_tunnel(pl2vc);
        }
    }

    ret = pw_get_lsp_index(&pl2vc->pwinfo);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW malloc private lsp failed !%s", VTY_NEWLINE);
        goto fail;
    }

    pl2vc->pwinfo.pw_type = pw_type;

    ret = mpls_if_bind_pw(pmpls_if, pl2vc);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);
        goto fail;
    }

    /* 绑定备时关联主 */
    if (pw_type == PW_TYPE_BACKUP)
    {
        ppwinfo                          = &pmpls_if->ppw_master->pwinfo;
        pl2vc->pwinfo.backup_index       = ppwinfo->pwindex;
        pmpls_if->ppw_master->ppw_backup = pl2vc;
    }

    ret = mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "L2VC send PW failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    ret = mpls_pw_mplsif_download(pmpls_if->ifindex, &pl2vc->pwinfo.pwindex, info, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "PW send ac failed for ipc error!%s", VTY_NEWLINE);
        goto fail;
    }

    if (LINK_UP == pl2vc->pwinfo.admin_up)
    {
        ret = pw_add_lsp(pl2vc);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "PW add private lsp failed for malloc failed !%s", VTY_NEWLINE);
            goto fail;
        }

        pw_set_pw_status(pmpls_if);
    }
    else
    {
        l2vc_alarm_process(OPCODE_DOWN, pl2vc->pwinfo.pwindex);
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_add_martini(pl2vc);
    }

    return CMD_SUCCESS;

fail:

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_delete_martini(pl2vc);
    }

    l2vc_unbind_lsp_tunnel(pl2vc);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc->pwinfo.nhp_index), pl2vc);
    mpls_if_unbind_pw(pmpls_if, pl2vc);
    pw_free_lsp_index(&pl2vc->pwinfo);

    if (pw_type == PW_TYPE_BACKUP)
    {
        pl2vc->pwinfo.backup_index = 0;
    }

    return CMD_WARNING;
}


/**
 * @brief      : pw 绑定接口删除命令
 * @param[in ] : backup -解绑备 pw
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 9:20:24
 * @note       :
 */
DEFUN (no_mpls_if_bind_pw,
    no_mpls_if_bind_pw_cmd,
    "no mpls l2vpn pw [backup]",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = NULL;
    struct mpls_if    *pif   = NULL;
    enum MPLSIF_INFO   info;
    int                ret   = ERRNO_SUCCESS;

    pif = mpls_if_lookup((uint32_t)vty->index);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    /* 接口属于 vpls 未绑定 PW */
    if (pif->vpn != 0)
    {
        return CMD_SUCCESS;
    }

    if(pif->aps_sessid)
    {
        mplsaps_unbind_lsp_pw(pif->aps_sessid);
    }

    if (argv[0] == NULL)
    {
        pl2vc = pif->ppw_master;
        if (NULL == pl2vc)
        {
            return CMD_SUCCESS;
        }

        info = MPLSIF_INFO_MASTER_PW;

        if (pif->ppw_slave != NULL)
        {
            vty_error_out(vty, "Please unbinding the slave PW !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else
    {
        pl2vc = pif->ppw_slave;
        if (NULL == pl2vc)
        {
            return CMD_SUCCESS;
        }

        info = MPLSIF_INFO_SLAVE_PW;

        pif->ppw_master->ppw_backup = NULL;
        pl2vc->pwinfo.backup_index  = 0;
    }

    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        l2vc_delete_martini(pl2vc);
    }

    l2vc_unbind_lsp_tunnel(pl2vc);
    tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc->pwinfo.nhp_index), pl2vc);
    mpls_if_unbind_pw(pif, pl2vc);

    pw_set_pw_status(pif);

    ret = mpls_pw_mplsif_download(pif->ifindex, &pl2vc->pwinfo.pwindex, info, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_warning_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);
    }

    ret = mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_warning_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);
    }

    pw_delete_lsp(&pl2vc->pwinfo);
    pw_free_lsp_index(&pl2vc->pwinfo);

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 配置 exp-domain 命令
 * @param[in ] : exp-domain <1-5> - exp-domain 模板号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 9:38:50
 * @note       :
 */
DEFUN(mpls_pw_exp_domain_enable,
    mpls_pw_exp_domain_enable_cmd,
    "qos mapping enable exp-domain <1-5>",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc  = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm  = NULL;
    int               *pexist = NULL;
    uint8_t domain_id;

    domain_id = atoi(argv[0]);

    if (0 != pl2vc->domain_id)
    {
        vty_error_out(vty, "domain already enable !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    pexist = qos_domain_exist_get(domain_id, QOS_TYPE_EXP, MODULE_ID_MPLS);
    if (NULL == pexist)
    {
        vty_error_out(vty, "Get qos domain info timeout!%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (0 == *pexist)
    {
        vty_error_out(vty, "The appointed qos domain is not exist!%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ((pl2vc->pwinfo.admin_up == LINK_UP) && (pl2vc->pwinfo.egress_lsp != 0))
    {
        plspm = mpls_lsp_lookup(pl2vc->pwinfo.egress_lsp);
        if (NULL != plspm)
        {
            plspm->domain_id = domain_id;

            mpls_lsp_update(plspm, LSP_SUBTYPE_QOS);
        }
    }

    qos_domain_ref_operate(domain_id, QOS_TYPE_EXP, MODULE_ID_MPLS, REF_OPER_INCREASE);

    pl2vc->domain_id = domain_id;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 删除 exp-domain 命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 9:41:30
 * @note       :
 */
DEFUN(no_mpls_pw_exp_domain_enable,
    no_mpls_pw_exp_domain_enable_cmd,
    "no qos mapping enable exp-domain",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm = NULL;

    if (pl2vc->domain_id == 0)
    {
        return CMD_SUCCESS;
    }

    if ((pl2vc->pwinfo.admin_up == LINK_UP) && (pl2vc->pwinfo.egress_lsp != 0))
    {
        plspm = mpls_lsp_lookup(pl2vc->pwinfo.egress_lsp);
        if (NULL != plspm)
        {
            plspm->domain_id = 0;

            mpls_lsp_update(plspm, LSP_SUBTYPE_QOS);
        }
    }

    qos_domain_ref_operate(pl2vc->domain_id, QOS_TYPE_EXP, MODULE_ID_MPLS, REF_OPER_DECREASE);

    pl2vc->domain_id = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 配置 exp-phb 命令
 * @param[in ] : exp-phb <1-5> - exp-phb 模板号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 9:48:47
 * @note       :
 */
DEFUN(mpls_pw_exp_phb_enable,
    mpls_pw_exp_phb_enable_cmd,
    "qos mapping enable exp-phb <1-5>",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc  = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm  = NULL;
    int               *pexist = NULL;
    uint8_t phb_id;

    phb_id = atoi(argv[0]);

    if ((ENABLE == pl2vc->phb_enable) && (0 != pl2vc->phb_id))
    {
        vty_error_out(vty, "phb already enable !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    pexist = qos_phb_exist_get(phb_id, QOS_TYPE_EXP, MODULE_ID_MPLS);
    if (NULL == pexist)
    {
        vty_error_out(vty, "Get qos phb info timeout!%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (0 == *pexist)
    {
        vty_error_out(vty, "The appointed qos phb is not exist!%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ((pl2vc->pwinfo.admin_up == LINK_UP) && (pl2vc->pwinfo.ingress_lsp != 0))
    {
        plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
        if (NULL != plspm)
        {
            plspm->phb_enable = ENABLE;
            plspm->phb_id     = phb_id;

            mpls_lsp_update(plspm, LSP_SUBTYPE_QOS);
        }
    }

    qos_phb_ref_operate(phb_id, QOS_TYPE_EXP, MODULE_ID_MPLS, REF_OPER_INCREASE);

    pl2vc->phb_enable = ENABLE;
    pl2vc->phb_id     = phb_id;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 删除 exp-phb 命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 9:53:49
 * @note       :
 */
DEFUN(no_mpls_pw_exp_phb_enable,
    no_mpls_pw_exp_phb_enable_cmd,
    "no qos mapping enable exp-phb",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm = NULL;

    if (pl2vc->phb_id == 0)
    {
        return CMD_SUCCESS;
    }

    if ((pl2vc->pwinfo.admin_up == LINK_UP) && (pl2vc->pwinfo.ingress_lsp != 0))
    {
        plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
        if (NULL != plspm)
        {
            plspm->phb_enable = ENABLE;
            plspm->phb_id     = 0;

            mpls_lsp_update(plspm, LSP_SUBTYPE_QOS);
        }
    }

    qos_phb_ref_operate(pl2vc->phb_id, QOS_TYPE_EXP, MODULE_ID_MPLS, REF_OPER_DECREASE);

    pl2vc->phb_enable = ENABLE;
    pl2vc->phb_id     = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 统计使能命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 9:57:39
 * @note       :
 */
DEFUN(mpls_pw_statistics_enable,
    mpls_pw_statistics_enable_cmd,
    "statistics enable",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm = NULL;

    if (ENABLE == pl2vc->statis_enable)
    {
        return CMD_SUCCESS;
    }

    plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
    if (NULL != plspm)
    {
        plspm->statis_enable = ENABLE;

        mpls_lsp_update(plspm, LSP_SUBTYPE_COUNTER);
    }

    plspm = mpls_lsp_lookup(pl2vc->pwinfo.egress_lsp);
    if (NULL != plspm)
    {
        plspm->statis_enable = ENABLE;

        mpls_lsp_update(plspm, LSP_SUBTYPE_COUNTER);
    }

    pl2vc->statis_enable = ENABLE;

    if (NULL != pl2vc->pswitch_pw)
    {
        pl2vc->pswitch_pw->statis_enable = ENABLE;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 统计去使能命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:01:50
 * @note       :
 */
DEFUN(no_mpls_pw_statistics_enable,
    no_mpls_pw_statistics_enable_cmd,
    "no statistics enable",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm = NULL;

    if (DISABLE == pl2vc->statis_enable)
    {
        return CMD_SUCCESS;
    }

    plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
    if (NULL != plspm)
    {
        plspm->statis_enable = DISABLE;

        mpls_lsp_update(plspm, LSP_SUBTYPE_COUNTER);
    }

    plspm = mpls_lsp_lookup(pl2vc->pwinfo.egress_lsp);
    if (NULL != plspm)
    {
        plspm->statis_enable = DISABLE;

        mpls_lsp_update(plspm, LSP_SUBTYPE_COUNTER);
    }

    pl2vc->statis_enable = DISABLE;

    if (NULL != pl2vc->pswitch_pw)
    {
        pl2vc->pswitch_pw->statis_enable = DISABLE;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 统计计数清除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:02:28
 * @note       :
 */
DEFUN(mpls_pw_statistics_clear,
    mpls_pw_statistics_clear_cmd,
    "statistics clear",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    int ret;

    ret = MPLS_IPC_SENDTO_HAL(&pl2vc->pwinfo, sizeof(struct pw_info), 1,
                        MODULE_ID_MPLS, IPC_TYPE_PW,
                        IPC_OPCODE_UPDATE, PW_SUBTYPE_COUNTER, pl2vc->pwinfo.pwindex);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (pl2vc->pswitch_pw != NULL)
    {
        ret = MPLS_IPC_SENDTO_HAL(&pl2vc->pswitch_pw->pwinfo, sizeof(struct pw_info), 1,
                            MODULE_ID_MPLS, IPC_TYPE_PW,
                            IPC_OPCODE_UPDATE, PW_SUBTYPE_COUNTER, pl2vc->pswitch_pw->pwinfo.pwindex);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 统计计数显示命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:03:25
 * @note       :
 */
DEFUN(show_mpls_pw_statistics,
    show_mpls_pw_statistics_cmd,
    "show statistics",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc    = (struct l2vc_entry *)vty->index;
    struct counter_t  *pcounter = NULL;
    struct ipc_mesg_n  *pmesg = NULL;


    if (DISABLE == pl2vc->statis_enable)
    {
        vty_warning_out(vty, "Statistics is disabled !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    pmesg = ipc_sync_send_n2(&pl2vc->pwinfo, sizeof(struct pw_info), 1, MODULE_ID_HAL,
                                        MODULE_ID_MPLS, IPC_TYPE_PW, PW_SUBTYPE_COUNTER,
                                        IPC_OPCODE_GET, pl2vc->pwinfo.pwindex , 1);
    if(NULL == pmesg)
    {
        return CMD_WARNING;
    }
    pcounter = (struct counter_t  *)pmesg->msg_data;
    if (pmesg->msghdr.data_len)
    {
        vty_out(vty, "PW %s statistics%s", pl2vc->name, VTY_NEWLINE);
        vty_out(vty, " Input  Total: %llu bytes, %llu packets, Dorp: %llu packets%s",
                pcounter->rx_bytes, pcounter->rx_packets, pcounter->rx_dropped, VTY_NEWLINE);
        vty_out(vty, " Output Total: %llu bytes, %llu packets, Dorp: %llu packets%s",
                pcounter->tx_bytes, pcounter->tx_packets, pcounter->tx_dropped, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "PW %s statistics%s", pl2vc->name, VTY_NEWLINE);
        vty_out(vty, " Input  Total: 0 bytes, 0 packets, Dorp: 0 packets%s", VTY_NEWLINE);
        vty_out(vty, " Output Total: 0 bytes, 0 packets, Dorp: 0 packets%s", VTY_NEWLINE);
    }
    mem_share_free(pmesg, MODULE_ID_MPLS);
    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 绑定 oam 会话命令
 * @param[in ] : session <1-65535> - oam 会话 id
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:04:35
 * @note       :
 */
DEFUN(mpls_pw_mplstp_oam_session_enable,
    mpls_pw_mplstp_oam_session_enable_cmd,
    "mplstp-oam enable session <1-65535>",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm = NULL;
    uint16_t oam_id = 0;
    int      ret    = 0;

    if (PW_ALREADY_BIND_MPLSTP_OAM(pl2vc))
    {
        vty_error_out(vty, "The pw '%s' is already bind MPLSTP-OAM session %d !%s",
                        pl2vc->name, pl2vc->mplsoam_id, VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_ALREADY_BIND_BFD(pl2vc))
    {
        vty_error_out(vty, "The pw '%s' is already bind BFD session %d !%s",
                        pl2vc->name, pl2vc->bfd_id, VTY_NEWLINE);

        return CMD_WARNING;
    }

	oam_id = atoi(argv[0]);

    if (PW_ALREADY_BIND_MSPW(&pl2vc->pwinfo))
    {
		ret = mpls_pw_bind_oam_mip(oam_id, pl2vc);
		if(ERRNO_SUCCESS != ret)
		{
			vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

			return CMD_WARNING;
		}
        return CMD_SUCCESS;
    }

	if(1 == pl2vc->gre_tunnel)
	{
        vty_error_out(vty, "The pw bind gre tunnel ,tpoam is not support!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}


    pl2vc->statis_enable = ENABLE;
    pl2vc->mplsoam_id    = oam_id;

	if(vty->config_read_flag)
	{
		return CMD_SUCCESS; 		
	}
	
    ret = mplsoam_session_enable(oam_id, pl2vc->pwinfo.pwindex, OAM_TYPE_PW);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

		pl2vc->statis_enable = DISABLE;
		pl2vc->mplsoam_id	 = 0;

        return CMD_WARNING;
    }

    if (pl2vc->pwinfo.admin_up == LINK_UP)
    {
        plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
        if (NULL != plspm)
        {
            plspm->mplsoam_id    = oam_id;
            plspm->statis_enable = ENABLE;

            mpls_lsp_update(plspm, LSP_SUBTYPE_MPLSTP_OAM);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 解绑 oam 命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:05:43
 * @note       :
 */
DEFUN(no_mpls_pw_mplstp_oam_session_enable,
    no_mpls_pw_mplstp_oam_session_enable_cmd,
    "no mplstp-oam enable session",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm = NULL;
    uint16_t oam_id = 0;

    oam_id = pl2vc->mplsoam_id;
    if (0 == oam_id)
    {
        return CMD_SUCCESS;
    }

	if(PW_ALREADY_BIND_MSPW(&pl2vc->pwinfo))
	{
		mpls_pw_unbind_oam_mip(oam_id, pl2vc);

		return CMD_SUCCESS;
	}

    mplsoam_session_disable(oam_id);

    pl2vc->mplsoam_id = 0;

    if (pl2vc->pwinfo.admin_up == LINK_UP)
    {
        plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
        if (NULL != plspm)
        {
            plspm->mplsoam_id = 0;

            mpls_lsp_update(plspm, LSP_SUBTYPE_MPLSTP_OAM);
        }

        pw_detection_protocal_up(&pl2vc->pwinfo);
    }
    else
    {
        //pw_detection_protocal_down(&pl2vc->pwinfo);
        pl2vc->pwinfo.status = ENABLE;
    }

    return CMD_SUCCESS;
}


DEFUN(mpls_pw_bfd__dynamic_enable,
	mpls_pw_bfd_dynamic_enable_cmd,
	"bfd enable [raw]",
	MPLS_CLI_INFO)
{
	struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
	uint16_t sess_id = 0;
	uint8_t ip_flag = 0;
	int 	 ret;

	if (pl2vc->pwinfo.vcid == 0)
	{
		vty_error_out(vty, "PW is incomplete, Can't bind the BFD !%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	if (PW_ALREADY_BIND_BFD(pl2vc))
	{
		vty_error_out(vty, "The pw '%s' is already bind BFD session %d !%s",
				pl2vc->name, pl2vc->bfd_id, VTY_NEWLINE);

		return CMD_WARNING;
	}

	if (PW_ALREADY_BIND_MPLSTP_OAM(pl2vc))
	{
		vty_error_out(vty, "The pw '%s' is already bind MPLSTP-OAM session %d !%s",
				pl2vc->name, pl2vc->mplsoam_id, VTY_NEWLINE);

		return CMD_WARNING;
	}

	if(NULL != argv[0])
	{
		ip_flag = 1;
	}

	ret = bfd_bind_for_lsp_pw(BFD_TYPE_PW, sess_id, pl2vc->pwinfo.pwindex, ip_flag);
	if (ERRNO_SUCCESS != ret)
	{
		vty_error_out(vty, "BFD %s%s", errcode_get_string(ret), VTY_NEWLINE);

		return CMD_WARNING;
	}

	/* */
	if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
	{
		if(PW_ALREADY_BIND_AC(&pl2vc->pwinfo))
		{
			l2vc_delete_martini(pl2vc);
			l2vc_add_martini(pl2vc);
		}
	}

	return CMD_SUCCESS;
}

/**
 * @brief      : pw 绑定 bfd 命令
 * @param[in ] : session <1-65535> - bfd 会话 id
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:06:18
 * @note       :
 */
DEFUN(mpls_pw_bfd_session_enable,
    mpls_pw_bfd_session_enable_cmd,
    "bfd enable session <1-65535> [raw]",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    uint16_t local_id = 0;
	uint8_t ip_flag = 0;
    int      ret;

    if (pl2vc->pwinfo.vcid == 0)
    {
        vty_error_out(vty, "PW is incomplete, Can't bind the BFD !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_ALREADY_BIND_BFD(pl2vc))
    {
        vty_error_out(vty, "The pw '%s' is already bind BFD session %d !%s",
                pl2vc->name, pl2vc->bfd_id, VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (PW_ALREADY_BIND_MPLSTP_OAM(pl2vc))
    {
        vty_error_out(vty, "The pw '%s' is already bind MPLSTP-OAM session %d !%s",
                pl2vc->name, pl2vc->mplsoam_id, VTY_NEWLINE);

        return CMD_WARNING;
    }

    local_id = atoi(argv[0]);

	if(NULL != argv[1])
	{
		ip_flag = 1;
	}

    ret = bfd_bind_for_lsp_pw(BFD_TYPE_PW, local_id, pl2vc->pwinfo.pwindex, ip_flag);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "BFD %s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    pl2vc->bfd_id = local_id;

    /* */
    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        if(PW_ALREADY_BIND_AC(&pl2vc->pwinfo))
        {
            l2vc_delete_martini(pl2vc);
            l2vc_add_martini(pl2vc);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 解绑 bfd 命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:07:13
 * @note       :
 */
DEFUN(no_mpls_pw_bfd_session_enable,
    no_mpls_pw_bfd_session_enable_cmd,
    "no bfd enable session",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    uint16_t local_id = 0;
    int      ret;

    local_id = pl2vc->bfd_id;
    if (0 == local_id)
    {
        return CMD_SUCCESS;
    }

    ret = bfd_unbind_for_lsp_pw(BFD_TYPE_PW, local_id, pl2vc->pwinfo.pwindex);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    pl2vc->bfd_id = 0;

    if (pl2vc->pwinfo.admin_up == LINK_UP)
    {
        pw_detection_protocal_up(&pl2vc->pwinfo);
    }
    else
    {
        //pw_detection_protocal_down(&pl2vc->pwinfo);
        pl2vc->pwinfo.status = ENABLE;
    }
    if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
    {
        if(PW_ALREADY_BIND_AC(&pl2vc->pwinfo))
        {
            l2vc_delete_martini(pl2vc);
            l2vc_add_martini(pl2vc);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 配置 car 命令
 * @param[in ] : ingress          - 入方向限速
 * @param[in ] : egress           - 出方向限速
 * @param[in ] : cir <1-10000000> - 承诺信息率
 * @param[in ] : pir <1-10000000> - 峰值信息率
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:07:36
 * @note       :
 */
DEFUN(mpls_pw_qos_car,
    mpls_pw_qos_car_cmd,
    "qos car (ingress | egress) cir <1-10000000> pir <1-10000000>",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm = NULL;
    enum LSP_DIRECTION dir   = LSP_DIRECTION_INGRESS;
    uint32_t cir = 0;
    uint32_t pir = 0;
    int      ret = 0;

    /* argv[0] : direction */
    if (argv[0][0] == 'i')
    {
        if (0 != pl2vc->car_cir[0])
        {
            vty_error_out(vty, "Ingress car already configuration !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        dir = LSP_DIRECTION_EGRESS;
    }
    else if (argv[0][0] == 'e')
    {
        if (0 != pl2vc->car_cir[1])
        {
            vty_error_out(vty, "Egress car already configuration !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        dir = LSP_DIRECTION_INGRESS;
    }

    /* argv[1] : cir */
    cir = atoi(argv[1]);

    /* argv[2] : pir */
    pir = atoi(argv[2]);

    if (pir < cir)
    {
        vty_error_out(vty, "PIR must larger than CIR or equal to CIR.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (LSP_DIRECTION_INGRESS == dir)
    {
        if (pl2vc->pwinfo.admin_up == LINK_UP)
        {
            if (pl2vc->pwinfo.nhp_type == NHP_TYPE_TUNNEL)
            {
                ret = tunnel_if_process_pw_car(tunnel_if_lookup(pl2vc->pwinfo.nhp_index),
                                                cir, OPCODE_ADD, 1);
                if (ERRNO_FAIL == ret)
                {
                    vty_error_out(vty, "egress CAR CAC check failed !%s", VTY_NEWLINE);

                    return CMD_WARNING;
                }
            }

            plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
            if (NULL != plspm)
            {
                plspm->car_cir[1] = cir;
                plspm->car_pir[1] = pir;

                mpls_lsp_update(plspm, LSP_SUBTYPE_CAR);
            }
        }

        pl2vc->car_cir[1] = cir;
        pl2vc->car_pir[1] = pir;
    }
    else if (LSP_DIRECTION_EGRESS == dir)
    {
        if (pl2vc->pwinfo.admin_up == LINK_UP)
        {
            if (pl2vc->pwinfo.nhp_type == NHP_TYPE_TUNNEL)
            {
                ret = tunnel_if_process_pw_car(tunnel_if_lookup(pl2vc->pwinfo.nhp_index),
                                                cir, OPCODE_ADD, 0);
                if (ERRNO_FAIL == ret)
                {
                    vty_error_out(vty, "ingress CAR CAC check failed !%s", VTY_NEWLINE);

                    return CMD_WARNING;
                }
            }

            plspm = mpls_lsp_lookup(pl2vc->pwinfo.egress_lsp);
            if (NULL != plspm)
            {
                plspm->car_cir[0] = cir;
                plspm->car_pir[0] = pir;

                mpls_lsp_update(plspm, LSP_SUBTYPE_CAR);
            }
        }

        pl2vc->car_cir[0] = cir;
        pl2vc->car_pir[0] = pir;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 删除 car 命令
 * @param[in ] : ingress - 入方向限速
 * @param[in ] : egress  - 出方向限速
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:10:49
 * @note       :
 */
DEFUN(no_mpls_pw_qos_car,
    no_mpls_pw_qos_car_cmd,
    "no qos car (ingress | egress)",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm = NULL;
    enum LSP_DIRECTION dir   = LSP_DIRECTION_INGRESS;

    if ('i' == argv[0][0])
    {
        if (pl2vc->car_cir[0] == 0)
        {
            return CMD_SUCCESS;
        }

        dir = LSP_DIRECTION_EGRESS;
    }
    else if ('e' == argv[0][0])
    {
        if (pl2vc->car_cir[1] == 0)
        {
            return CMD_SUCCESS;
        }

        dir = LSP_DIRECTION_INGRESS;
    }

    if (LSP_DIRECTION_INGRESS == dir)
    {
        if (pl2vc->pwinfo.admin_up == LINK_UP)
        {
            if (pl2vc->pwinfo.nhp_type == NHP_TYPE_TUNNEL)
            {
                tunnel_if_process_pw_car(tunnel_if_lookup(pl2vc->pwinfo.nhp_index),
                                            pl2vc->car_cir[1], OPCODE_DELETE, 1);
            }

            plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
            if (NULL != plspm)
            {
                plspm->car_cir[1] = 0;
                plspm->car_pir[1] = 0;

                mpls_lsp_update(plspm, LSP_SUBTYPE_CAR);
            }
        }

        pl2vc->car_cir[1] = 0;
        pl2vc->car_pir[1] = 0;
    }
    else if (LSP_DIRECTION_EGRESS == dir)
    {
        if (pl2vc->pwinfo.admin_up == LINK_UP)
        {
            if (pl2vc->pwinfo.nhp_type == NHP_TYPE_TUNNEL)
            {
                tunnel_if_process_pw_car(tunnel_if_lookup(pl2vc->pwinfo.nhp_index),
                                            pl2vc->car_cir[0], OPCODE_DELETE, 0);
            }

            plspm = mpls_lsp_lookup(pl2vc->pwinfo.egress_lsp);
            if (NULL != plspm)
            {
                plspm->car_cir[0] = 0;
                plspm->car_pir[0] = 0;

                mpls_lsp_update(plspm, LSP_SUBTYPE_CAR);
            }
        }

        pl2vc->car_cir[0] = 0;
        pl2vc->car_pir[0] = 0;
    }

    return CMD_SUCCESS;
}


DEFUN(mpls_pw_qos_car_l1,
    mpls_pw_qos_car_l1_cmd,
    "qos car include-interframe (ingress | egress) cir <1-10000000> pir <1-10000000>",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm = NULL;
    enum LSP_DIRECTION dir   = LSP_DIRECTION_INGRESS;
    uint32_t cir = 0;
    uint32_t pir = 0;
    int      ret = 0;

    /* argv[0] : direction */
    if (argv[0][0] == 'i')
    {
        if (0 != pl2vc->car_cir[0])
        {
            vty_error_out(vty, "Ingress car already configuration !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        dir = LSP_DIRECTION_EGRESS;
    }
    else if (argv[0][0] == 'e')
    {
        if (0 != pl2vc->car_cir[1])
        {
            vty_error_out(vty, "Egress car already configuration !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        dir = LSP_DIRECTION_INGRESS;
    }

    /* argv[1] : cir */
    cir = atoi(argv[1]);
    cir |= CAR_VALUE_L1_FLAG;

    /* argv[2] : pir */
    pir = atoi(argv[2]);
    pir |= CAR_VALUE_L1_FLAG;

    if (pir < cir)
    {
        vty_error_out(vty, "PIR must larger than CIR or equal to CIR.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (LSP_DIRECTION_INGRESS == dir)
    {
        if (pl2vc->pwinfo.admin_up == LINK_UP)
        {
            if (pl2vc->pwinfo.nhp_type == NHP_TYPE_TUNNEL)
            {
                ret = tunnel_if_process_pw_car(tunnel_if_lookup(pl2vc->pwinfo.nhp_index),
                                                cir, OPCODE_ADD, 1);
                if (ERRNO_FAIL == ret)
                {
                    vty_error_out(vty, "egress CAR CAC check failed !%s", VTY_NEWLINE);

                    return CMD_WARNING;
                }
            }

            plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
            if (NULL != plspm)
            {
                plspm->car_cir[1] = cir;
                plspm->car_pir[1] = pir;

                mpls_lsp_update(plspm, LSP_SUBTYPE_CAR);
            }
        }

        pl2vc->car_cir[1] = cir;
        pl2vc->car_pir[1] = pir;
    }
    else if (LSP_DIRECTION_EGRESS == dir)
    {
        if (pl2vc->pwinfo.admin_up == LINK_UP)
        {
            if (pl2vc->pwinfo.nhp_type == NHP_TYPE_TUNNEL)
            {
                ret = tunnel_if_process_pw_car(tunnel_if_lookup(pl2vc->pwinfo.nhp_index),
                                                cir, OPCODE_ADD, 0);
                if (ERRNO_FAIL == ret)
                {
                    vty_error_out(vty, "ingress CAR CAC check failed !%s", VTY_NEWLINE);

                    return CMD_WARNING;
                }
            }

            plspm = mpls_lsp_lookup(pl2vc->pwinfo.egress_lsp);
            if (NULL != plspm)
            {
                plspm->car_cir[0] = cir;
                plspm->car_pir[0] = pir;

                mpls_lsp_update(plspm, LSP_SUBTYPE_CAR);
            }
        }

        pl2vc->car_cir[0] = cir;
        pl2vc->car_pir[0] = pir;
    }

    return CMD_SUCCESS;
}


DEFUN(mpls_pw_hqos_queue_apply,
    mpls_pw_hqos_queue_apply_cmd,
    "hqos cir <0-10000000> {pir <0-10000000> | queue-profile <1-100>}",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm = NULL;
    struct hqos_t hqos;
    int      *pexist   = NULL;
    uint16_t *phqos_id = NULL;

    memset(&hqos, 0, sizeof(struct hqos_t));

    if (0 != pl2vc->hqos_id)
    {
        vty_error_out(vty, "Hqos already configuration !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[0]: cir */
    hqos.cir = atoi(argv[0]);

    /* 只配置 cir 为 0 无效 */
    if ((0 == hqos.cir) && (NULL == argv[1]) && (NULL == argv[2]))
    {
        return CMD_SUCCESS;
    }

    /* argv[1]: pir */
    if (NULL != argv[1])
    {
        hqos.pir = atoi(argv[1]);
        if (hqos.pir < hqos.cir)
        {
            vty_error_out(vty, "PIR must larger than CIR or equal to CIR.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    /* argv[2]: hqos id */
    if (NULL != argv[2])
    {
        /* 检查模板是否存在 */
        pexist = qos_queue_profile_exist_get(atoi(argv[2]), MODULE_ID_MPLS);
        if (NULL == pexist)
        {
            vty_error_out(vty, "Get hqos queue profile info timeout!%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (0 == *pexist)
        {
            vty_error_out(vty, "The appointed queue profile is not exist!%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        hqos.hqos_id = atoi(argv[2]);
    }

    /* 获取配置索引 */
    phqos_id = qos_hqosid_get(&hqos, MODULE_ID_MPLS);
    if ((NULL == phqos_id) || (0 == *phqos_id))
    {
        vty_error_out(vty, "get hqos id failed !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    pl2vc->hqos_id = *phqos_id;

    if (pl2vc->pwinfo.admin_up == LINK_UP)
    {
        plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
        if (NULL != plspm)
        {
            plspm->hqos_id = pl2vc->hqos_id;

            mpls_lsp_update(plspm, LSP_SUBTYPE_HQOS);
        }
    }

    return CMD_SUCCESS;
}


DEFUN(no_mpls_pw_hqos_queue_apply,
    no_mpls_pw_hqos_queue_apply_cmd,
    "no hqos",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;
    struct lsp_entry  *plspm = NULL;

    if (0 == pl2vc->hqos_id)
    {
        return CMD_SUCCESS;
    }

    if (pl2vc->pwinfo.admin_up == LINK_UP)
    {
        plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
        if (NULL != plspm)
        {
            plspm->hqos_id = 0;

            mpls_lsp_update(plspm, LSP_SUBTYPE_HQOS);
        }
    }

    /* 用配置索引删除配置参数 */
    qos_hqos_delete(pl2vc->hqos_id, MODULE_ID_MPLS);

    pl2vc->hqos_id = 0;

    return CMD_SUCCESS;
}

/**
 * @brief      : pw 带宽采样间隔 配置命令
 * @param[in ] : <5-20> - s
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : 
 * @date       : 
 * @note       : pw 处于绑定状态不允许配置该命令，默认为 5s
 */
DEFUN (mpls_pw_sample_interval,
    mpls_pw_sample_interval_cmd,
    "sample interval <5-20>",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    PW_ALREADY_BIND(vty, &pl2vc->pwinfo);

    pl2vc->pwinfo.sample_interval = atoi(argv[0]);

    return CMD_SUCCESS;
}

/**
 * @brief      : pw 配置信息显示命令
 * @param[in ] : NAME - pw 名字
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:11:57
 * @note       :
 */
DEFUN (show_mpls_pw,
    show_mpls_pw_cmd,
    "show mpls pw [NAME]",
    MPLS_CLI_INFO)
{
    struct hash_bucket *pbucket = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    int num = 0;
    int cursor;

    if (argv[0] != NULL)
    {
        pl2vc = l2vc_lookup((uchar *)argv[0]);
        if (NULL == pl2vc)
        {
            return CMD_SUCCESS;
        }

        mpls_pw_show_verbose(vty, pl2vc);

        vty_out(vty, "%s", VTY_NEWLINE);
    }
    else
    {
        HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
        {
            pl2vc = (struct l2vc_entry *)pbucket->data;
            if (pl2vc == NULL)
            {
                continue;
            }

            if (++num == 1)
            {
                mpls_pw_show_summary(vty);

                vty_out(vty, "%-32s", "name");
                vty_out(vty, "%-31s", "AC-interface");
                vty_out(vty, "%-10s", "AC-status");
                vty_out(vty, "%-15s", "tunnel");
                vty_out(vty, "%-10s", "PW-status");
                vty_out(vty, "%s", VTY_NEWLINE);
            }

            mpls_pw_show_brief(vty, pl2vc);
        }

        if (0 != num)
        {
            vty_out(vty, "%s", VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示当前 pw 节点下配置命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:13:02
 * @note       :
 */
DEFUN(show_this_pw,
    show_this_pw_cmd,
    "show this",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)vty->index;

    vty_out(vty, "#%s", VTY_NEWLINE);
    vty_out(vty, " #%s", VTY_NEWLINE);

    mpls_pw_config_write_process(vty, pl2vc);

    vty_out(vty, "#%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 节点下全部配置显示
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:13:45
 * @note       :
 */
int mpls_pw_config_show_this(struct vty *vty)
{
    struct hash_bucket *pbucket = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    int cursor;

    HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
    {
        pl2vc = (struct l2vc_entry *)pbucket->data;
        if (NULL == pl2vc)
        {
            continue;
        }

        mpls_pw_config_write_process(vty, pl2vc);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:14:38
 * @note       :
 */
static int mpls_pw_config_write(struct vty *vty)
{
    struct hash_bucket *pbucket = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    int cursor;

    if (DISABLE == gmpls.enable)
    {
        vty_out(vty, "mpls%s", VTY_NEWLINE);
    }

    HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
    {
        pl2vc = (struct l2vc_entry *)pbucket->data;
        if (NULL == pl2vc)
        {
            continue;
        }

        mpls_pw_config_write_process(vty, pl2vc);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 命令行初始化
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:15:06
 * @note       :
 */
void mpls_pw_cmd_init(void)
{
    install_node(&pw_node, mpls_pw_config_write);
    install_default(PW_NODE);

    install_element (MPLS_NODE, &mpls_pw_name_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &no_mpls_pw_name_cmd, CMD_SYNC);

    install_element (PW_NODE, &show_this_pw_cmd, CMD_LOCAL);
    install_element (PW_NODE, &mpls_pw_protocol_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_vc_type_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_peer_vcid_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_label_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_mtu_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_ctrl_word_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_encapsulate_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_ces_frame_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_tunnel_bind_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_tunnel_policy_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_switch_pw_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_switch_vsi_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_switch_vsi_backup_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_switch_vsi_master_cmd, CMD_SYNC);

    install_element (PW_NODE, &no_mpls_pw_protocol_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_vc_type_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_peer_vcid_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_label_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_mtu_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_encapsulate_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_ces_frame_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_tunnel_bind_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_tunnel_policy_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_switch_pw_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_switch_vsi_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_switch_vsi_backup_cmd, CMD_SYNC);

    install_element (PW_NODE, &show_mpls_pw_cmd, CMD_LOCAL);
    install_element (MPLS_NODE, &show_mpls_pw_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &show_mpls_pw_cmd, CMD_LOCAL);
    install_element (PHYSICAL_IF_NODE, &show_mpls_pw_cmd, CMD_LOCAL);
    install_element (PHYSICAL_SUBIF_NODE, &show_mpls_pw_cmd, CMD_LOCAL);
    install_element (TRUNK_IF_NODE, &show_mpls_pw_cmd, CMD_LOCAL);
    install_element (TDM_IF_NODE, &show_mpls_pw_cmd, CMD_LOCAL);
    install_element (TDM_SUBIF_NODE, &show_mpls_pw_cmd, CMD_LOCAL);

    install_element (PHYSICAL_IF_NODE, &mpls_if_bind_pw_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &mpls_if_bind_pw_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &mpls_if_bind_pw_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &mpls_if_bind_pw_cmd, CMD_SYNC);
    install_element (TDM_IF_NODE, &mpls_if_bind_pw_cmd, CMD_SYNC);
    install_element (TDM_SUBIF_NODE, &mpls_if_bind_pw_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &no_mpls_if_bind_pw_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_mpls_if_bind_pw_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &no_mpls_if_bind_pw_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &no_mpls_if_bind_pw_cmd, CMD_SYNC);
    install_element (TDM_IF_NODE, &no_mpls_if_bind_pw_cmd, CMD_SYNC);
    install_element (TDM_SUBIF_NODE, &no_mpls_if_bind_pw_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &mpls_interface_pw_failback_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &mpls_interface_pw_failback_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &mpls_interface_pw_failback_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &mpls_interface_pw_failback_cmd, CMD_SYNC);
    install_element (TDM_IF_NODE, &mpls_interface_pw_failback_cmd, CMD_SYNC);
    install_element (TDM_SUBIF_NODE, &mpls_interface_pw_failback_cmd, CMD_SYNC);

    install_element (PW_NODE, &mpls_pw_exp_domain_enable_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_exp_domain_enable_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_exp_phb_enable_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_exp_phb_enable_cmd, CMD_SYNC);

    install_element (PW_NODE, &mpls_pw_statistics_enable_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_statistics_enable_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_statistics_clear_cmd, CMD_SYNC);
    install_element (PW_NODE, &show_mpls_pw_statistics_cmd, CMD_LOCAL);

    install_element (PW_NODE, &mpls_pw_mplstp_oam_session_enable_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_mplstp_oam_session_enable_cmd, CMD_SYNC);

    install_element (PW_NODE, &mpls_pw_bfd_session_enable_cmd, CMD_SYNC);
    install_element (PW_NODE, &mpls_pw_bfd_dynamic_enable_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_bfd_session_enable_cmd, CMD_SYNC);

    install_element (PW_NODE, &mpls_pw_qos_car_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_qos_car_cmd, CMD_SYNC);

    install_element (PW_NODE, &mpls_pw_qos_car_l1_cmd, CMD_SYNC);

    install_element (PW_NODE, &mpls_pw_hqos_queue_apply_cmd, CMD_SYNC);
    install_element (PW_NODE, &no_mpls_pw_hqos_queue_apply_cmd, CMD_SYNC);

    install_element (PW_NODE, &mpls_pw_sample_interval_cmd, CMD_SYNC);

    return;
}


/**
 * @brief      :
 * @param[in ] : 
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年6月19日
 * @note       : 
 */

void  mpls_config_finish_func(void * a)
{
    struct hash_bucket *pbucket = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    int cursor;
	int ret;
    struct lsp_entry  *plspm = NULL;
    uint16_t oam_id = 0;
	struct oam_session *psess = NULL;
    struct tunnel_if *pif = NULL;


    HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
    {
        pl2vc = (struct l2vc_entry *)pbucket->data;
        if (NULL == pl2vc)
        {
            continue;
        }

		if (PW_ALREADY_BIND_MSPW(&pl2vc->pwinfo))
		{
			printf ("%s,%s , pw %s , already bind mspw, continue \n",__FILE__,__FUNCTION__,pl2vc->name);
			continue;
		}

		if ((pl2vc->mplsoam_id >= 1) && (pl2vc->mplsoam_id <= OAM_MEP_MAX))
		{			

		   oam_id = pl2vc->mplsoam_id;
			
		   ret = mplsoam_session_enable(oam_id, pl2vc->pwinfo.pwindex, OAM_TYPE_PW);
		   if (ERRNO_SUCCESS != ret)
		   {
			   printf ("%s,%s , pw %s, mplsoam_session_enable() return error\n",__FILE__,__FUNCTION__,pl2vc->name);
		
			   pl2vc->statis_enable = DISABLE;
			   pl2vc->mplsoam_id	= 0;
			   
			   continue;
		   }
		
		   if (pl2vc->pwinfo.admin_up == LINK_UP)
		   {
			   plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
			   if (NULL != plspm)
			   {
				   plspm->mplsoam_id	= oam_id;
				   plspm->statis_enable = ENABLE;
		
				   mpls_lsp_update(plspm, LSP_SUBTYPE_MPLSTP_OAM);
			   }
		   }
		
		}
    }

	HASH_BUCKET_LOOP(pbucket, cursor, mplsoam_session_table)
	{
		psess = (struct oam_session *)pbucket->data;

		if(psess->info.type == OAM_TYPE_LSP)
		{
			plspm = mpls_lsp_lookup(psess->info.index);
			if(plspm == NULL)
			{
				printf("error. plspm is NULL,session_id:%d \n",psess->info.session_id);
				continue;
			}

			pif = tunnel_if_lookup(plspm->group_index);
			if(pif == NULL || pif->p_mplstp == NULL)
			{
				printf("error. pif:0x%p, pif->p_mplstp:0x%p, session_id:%d \n",pif,pif->p_mplstp,psess->info.session_id);
				continue;
			}

			if(pif->p_mplstp->ingress_lsp && (pif->p_mplstp->ingress_lsp->lsp_index ==  psess->info.index))
			{
				tunnel_if_bind_mplstp_oam(pif, psess->info.session_id, TUNNEL_STATUS_MASTER);
			}
			else if(pif->p_mplstp->backup_ingress_lsp &&(pif->p_mplstp->backup_ingress_lsp->lsp_index == psess->info.index))
			{
				tunnel_if_bind_mplstp_oam(pif, psess->info.session_id, TUNNEL_STATUS_BACKUP);
			}
			else
			{
				printf("error. index:%x,ingress_lsp:%p,backup_ingress_lsp:%p \n",psess->info.index,pif->p_mplstp->ingress_lsp,pif->p_mplstp->backup_ingress_lsp);
			}
			
		}	 
	}

	

	return ;

}

