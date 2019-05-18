/**
 * @file      : tunnel_cmd.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月7日 10:11:19
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/memory.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include <lib/alarm.h>
#include "lspm.h"
#include "mpls_main.h"
#include "lsp_static.h"
#include "tunnel.h"
#include "tunnel_cmd.h"
#include "mpls_oam/mpls_oam.h"
#include "bfd/bfd_session.h"
#include "bfd/bfd.h"


/**
 * @brief      : gre tunnel 下一跳下发
 * @param[in ] : ptunnel - tunnel 结构
 * @param[in ] : opcode  - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 10:12:44
 * @note       :
 */
static int tunnel_if_nexthop_download(struct tunnel_t *ptunnel, enum OPCODE_E opcode)
{
    int ret = ERRNO_SUCCESS;

    if (NULL == ptunnel)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (OPCODE_ADD == opcode)
    {
        ret = MPLS_IPC_SENDTO_FTM(ptunnel, sizeof(struct tunnel_t), 1, MODULE_ID_MPLS,
                            IPC_TYPE_TUNNEL, IPC_OPCODE_ADD, TUNNEL_INFO_NEXTHOP, ptunnel->ifindex);
        if (ERRNO_SUCCESS != ret)
        {
             MPLS_LOG_ERROR("IPC send to ftm\n");

             return ERRNO_IPC;
        }
    }
    else if (OPCODE_DELETE == opcode)
    {
        ret = MPLS_IPC_SENDTO_FTM(ptunnel, sizeof(struct tunnel_t), 1, MODULE_ID_MPLS,
                            IPC_TYPE_TUNNEL, IPC_OPCODE_DELETE, TUNNEL_INFO_NEXTHOP, ptunnel->ifindex);
        if (ERRNO_SUCCESS != ret)
        {
             MPLS_LOG_ERROR("IPC send to ftm\n");

             return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 下发 tunnel 配置信息
 * @param[in ] : ptunnel - tunnel 结构
 * @param[in ] : subtype - 消息子类型
 * @param[in ] : opcode  - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 10:14:22
 * @note       :
 */
int tunnel_if_tunnel_t_download(struct tunnel_t *ptunnel, enum TUNNEL_INFO subtype,
                            enum OPCODE_E opcode)
{
    struct tunnel_if *pif = NULL;
    struct tunnel_t tunnel;
    int ret;

    MPLS_LOG_DEBUG();

    if (NULL == ptunnel)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memcpy(&tunnel, ptunnel, sizeof(struct tunnel_t));

    /* tp tunnel 下发消息时将 egress lsp 索引换成 inlabel */
    if (TUNNEL_PRO_MPLSTP == ptunnel->protocol)
    {
        pif = tunnel_if_lookup(ptunnel->ifindex);
        if (NULL != pif)
        {
            if (NULL != pif->p_mplstp->egress_lsp)
            {
                tunnel.master_egress_index = pif->p_mplstp->egress_lsp->inlabel;
            }

            if (NULL != pif->p_mplstp->backup_egress_lsp)
            {
                tunnel.backup_egress_index = pif->p_mplstp->backup_egress_lsp->inlabel;
            }
        }
    }

    if (OPCODE_UPDATE == opcode)
    {
        if ((subtype == TUNNEL_INFO_IP)
            || (subtype == TUNNEL_INFO_PROTOCOL)
            || (subtype == TUNNEL_INFO_OUTIF))
        {
            ret = mpls_ipc_send_hal_wait_ack(&tunnel, sizeof(struct tunnel_t), 1, 
                                        MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
                                        IPC_OPCODE_UPDATE, subtype, tunnel.ifindex);
            if (ERRNO_SUCCESS != ret)
            {
                 MPLS_LOG_ERROR("IPC send to hal %d\n", ret);

                 return ret;
            }
        }
        else
        {
            ret = MPLS_IPC_SENDTO_HAL(&tunnel, sizeof(struct tunnel_t), 1, MODULE_ID_MPLS,
                                IPC_TYPE_TUNNEL, IPC_OPCODE_UPDATE, subtype, tunnel.ifindex);
            if (ERRNO_SUCCESS != ret)
            {
                 MPLS_LOG_ERROR("IPC send to hal\n");

                 return ERRNO_IPC;
            }
        }

        ret = MPLS_IPC_SENDTO_FTM(&tunnel, sizeof(struct tunnel_t), 1, MODULE_ID_MPLS,
                            IPC_TYPE_TUNNEL, IPC_OPCODE_UPDATE, subtype, tunnel.ifindex);
        if (ERRNO_SUCCESS != ret)
        {
             MPLS_LOG_ERROR("IPC send to ftm\n");

             return ERRNO_IPC;
        }
    }
    else if (OPCODE_ADD == opcode)
    {
        ret = mpls_ipc_send_hal_wait_ack(&tunnel, sizeof(struct tunnel_t), 1,
                                    MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
                                    IPC_OPCODE_ADD, subtype, tunnel.ifindex);
        if (ERRNO_SUCCESS != ret)
        {
             MPLS_LOG_ERROR("IPC send to hal\n");

             return ret;
        }

        ret = MPLS_IPC_SENDTO_FTM(&tunnel, sizeof(struct tunnel_t), 1, MODULE_ID_MPLS,
                            IPC_TYPE_TUNNEL, IPC_OPCODE_ADD, subtype, tunnel.ifindex);
        if (ERRNO_SUCCESS != ret)
        {
             MPLS_LOG_ERROR("IPC send to ftm\n");

             return ERRNO_IPC;
        }
    }
    else if (OPCODE_DELETE == opcode)
    {
        ret = MPLS_IPC_SENDTO_HAL(&tunnel, sizeof(struct tunnel_t), 1, MODULE_ID_MPLS,
                            IPC_TYPE_TUNNEL, IPC_OPCODE_DELETE, subtype, tunnel.ifindex);
        if (ERRNO_SUCCESS != ret)
        {
             MPLS_LOG_ERROR("IPC send to hal\n");

             return ERRNO_IPC;
        }

        ret = MPLS_IPC_SENDTO_FTM(&tunnel, sizeof(struct tunnel_t), 1, MODULE_ID_MPLS,
                            IPC_TYPE_TUNNEL, IPC_OPCODE_DELETE, subtype, tunnel.ifindex);
        if (ERRNO_SUCCESS != ret)
        {
             MPLS_LOG_ERROR("IPC send to ftm\n");

             return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 显示 tunnel 配置
 * @param[in ] : vty - vty 结构
 * @param[in ] : pif - tunnel 接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 13:47:03
 * @note       :
 */
static int tunnel_if_show_brief(struct vty *vty, struct tunnel_if *pif)
{
    const char *protocol = NULL;
    char ifname[IFNET_NAMESIZE]  = "";
    char srcip[INET_ADDRSTRLEN]  = "";
    char destip[INET_ADDRSTRLEN] = "";

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    ifm_get_name_by_ifindex(pif->tunnel.ifindex, ifname);

    switch (pif->tunnel.protocol)
    {
        case TUNNEL_PRO_MPLSTP:
            protocol = "mplstp";
            break;
        case TUNNEL_PRO_MPLSTE:
            protocol = "mplste";
            break;
        case TUNNEL_PRO_GRE:
            protocol = "gre";
            break;
        default:
            protocol = "-";
            break;
    }

    vty_out(vty, "%-16s", ifname);
    vty_out(vty, "%-9s", protocol);

    if (0 != pif->tunnel.sip.addr.ipv4)
    {
        inet_ipv4tostr(pif->tunnel.sip.addr.ipv4, srcip);

        vty_out(vty, "%-16s", srcip);
    }
    else
    {
        vty_out(vty, "%-16s", "-");
    }

    if (0 != pif->tunnel.dip.addr.ipv4)
    {
        inet_ipv4tostr(pif->tunnel.dip.addr.ipv4, destip);

        vty_out(vty, "%-16s", destip);
    }
    else
    {
        vty_out(vty, "%-16s", "-");
    }

    vty_out(vty, "%-7s", pif->tunnel.down_flag == LINK_DOWN?"down":"up");
    vty_out(vty, "%s", VTY_NEWLINE);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 显示 tunnel 详细配置
 * @param[in ] : vty - vty 结构
 * @param[in ] : pif - tunnel 接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 13:48:45
 * @note       :
 */
static int tunnel_if_show_verbose(struct vty *vty, struct tunnel_if *pif)
{
    struct hqos_t *phqos = NULL;
    char ifname[IFNET_NAMESIZE]   = "";
    char outif[IFNET_NAMESIZE]    = "";
    char srcip[INET_ADDRSTRLEN]   = "";
    char destip[INET_ADDRSTRLEN]  = "";
    char nexthop[INET_ADDRSTRLEN] = "";

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    ifm_get_name_by_ifindex(pif->tunnel.ifindex, ifname);
    inet_ipv4tostr(pif->tunnel.sip.addr.ipv4, srcip);
    inet_ipv4tostr(pif->tunnel.dip.addr.ipv4, destip);

    vty_out(vty, "%-20s : %s%s", "interface", ifname, VTY_NEWLINE);
    vty_out(vty, "%-20s : %s%s", "protocol", pif->tunnel.protocol == TUNNEL_PRO_MPLSTP ?
            "mpls-tp" : pif->tunnel.protocol == TUNNEL_PRO_MPLSTE ? "mpls-te" :
            pif->tunnel.protocol == TUNNEL_PRO_GRE ? "gre" : "-", VTY_NEWLINE);
    vty_out(vty, "%-20s : %s%s", "link state", pif->tunnel.down_flag == LINK_DOWN ?
                    "down" : "up", VTY_NEWLINE);
    vty_out(vty, "%-20s : %s%s", "source ip", pif->tunnel.sip.addr.ipv4 == 0 ? "-" :
                    srcip, VTY_NEWLINE);
    vty_out(vty, "%-20s : %s%s", "destination ip", pif->tunnel.dip.addr.ipv4 == 0 ? "-" :
                    destip, VTY_NEWLINE);

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)
    {
        if (pif->tunnel.down_flag == LINK_DOWN)
        {
            vty_out(vty, "%-20s : %s%s", "working status", "master", VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%-20s : %s%s", "working status", pif->tunnel.backup_status ==
                        TUNNEL_STATUS_MASTER ? "master" : pif->tunnel.backup_status ==
                        TUNNEL_STATUS_BACKUP ? "backup" : "frr", VTY_NEWLINE);
        }

        if (pif->p_mplstp->ingress_lsp == NULL)
        {
            vty_out(vty, "%-20s : %s%s", "master-ingress-lsp", " ", VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%-20s : %s%s", "master-ingress-lsp",
                    pif->p_mplstp->ingress_lsp->name, VTY_NEWLINE);
        }

        if (pif->p_mplstp->egress_lsp == NULL)
        {
            vty_out(vty, "%-20s : %s%s", "master-egress-lsp", " ", VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%-20s : %s%s", "master-egress-lsp",
                    pif->p_mplstp->egress_lsp->name, VTY_NEWLINE);
        }

        if (pif->p_mplstp->backup_ingress_lsp == NULL)
        {
            vty_out(vty, "%-20s : %s%s", "backup-ingress-lsp", " ", VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%-20s : %s%s", "backup-ingress-lsp",
                    pif->p_mplstp->backup_ingress_lsp->name, VTY_NEWLINE);
        }

        if (pif->p_mplstp->backup_egress_lsp == NULL)
        {
            vty_out(vty, "%-20s : %s%s", "backup-egress-lsp", " ", VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%-20s : %s%s", "backup-egress-lsp",
                    pif->p_mplstp->backup_egress_lsp->name, VTY_NEWLINE);
        }

        if (0 != pif->tunnel.bfd_id)
        {
            vty_out(vty, "%-20s : %u%s", "tunnel bfd id", pif->tunnel.bfd_id, VTY_NEWLINE);
        }

        if (0 != pif->tunnel.mplsoam_id)
        {
            vty_out(vty, "%-20s : %u%s", "tunnel mplstp-oam id", pif->tunnel.mplsoam_id, VTY_NEWLINE);
        }

        if (pif->p_mplstp->ingress_lsp != NULL)
        {
            if (0 != pif->p_mplstp->ingress_lsp->bfd_id)
            {
                vty_out(vty, "%-20s : %u%s", "master lsp bfd id",
                        pif->p_mplstp->ingress_lsp->bfd_id, VTY_NEWLINE);
            }

            if (0 != pif->p_mplstp->ingress_lsp->mplsoam_id)
            {
                vty_out(vty, "%-20s : %u%s", "master lsp mplstp-oam id",
                        pif->p_mplstp->ingress_lsp->mplsoam_id, VTY_NEWLINE);
            }
        }

        if (pif->p_mplstp->backup_ingress_lsp != NULL)
        {
            if (0 != pif->p_mplstp->backup_ingress_lsp->bfd_id)
            {
                vty_out(vty, "%-20s : %u%s", "backup lsp bfd id",
                        pif->p_mplstp->backup_ingress_lsp->bfd_id, VTY_NEWLINE);
            }

            if (0 != pif->p_mplstp->backup_ingress_lsp->mplsoam_id)
            {
                vty_out(vty, "%-20s : %u%s", "backup lsp mplstp-oam id",
                        pif->p_mplstp->backup_ingress_lsp->mplsoam_id, VTY_NEWLINE);
            }
        }

        if (pif->tunnel.failback == FAILBACK_DISABLE)
        {
            vty_out(vty, "%-20s : %s%s", "failback status", "non-failback", VTY_NEWLINE);
        }
        else if (pif->tunnel.failback == FAILBACK_ENABLE)
        {
            vty_out(vty, "%-20s : %s%s", "failback status", "failback", VTY_NEWLINE);
            vty_out(vty, "%-20s : %us%s", "failback wtr", pif->tunnel.wtr, VTY_NEWLINE);
        }
    }
    else if (TUNNEL_PRO_GRE == pif->tunnel.protocol)
    {
        uint8_t link_state = 0;
        if ((0 == pif->tunnel.outif) || (ifm_get_link(pif->tunnel.outif, MODULE_ID_MPLS, &link_state)) || (IFNET_LINKDOWN == link_state))
        {
            vty_out(vty, "%-20s : %s%s", "out-going interface", "-", VTY_NEWLINE);
            vty_out(vty, "%-20s : %s%s", "destination mac", "-", VTY_NEWLINE);
        }
        else
        {
            ifm_get_name_by_ifindex(pif->tunnel.outif, outif);
            vty_out(vty, "%-20s : %s%s", "out-going interface", outif, VTY_NEWLINE);
            vty_out(vty, "%-20s : %02x:%02x:%02x:%02x:%02x:%02x%s", "destination mac",
                    pif->tunnel.dmac[0], pif->tunnel.dmac[1], pif->tunnel.dmac[2],
                    pif->tunnel.dmac[3], pif->tunnel.dmac[4], pif->tunnel.dmac[5], VTY_NEWLINE);
        }

        if (0 != pif->tunnel.nexthop)
        {
            inet_ipv4tostr(pif->tunnel.nexthop, nexthop);
            vty_out(vty, "%-20s : %s%s", "nexthop", nexthop, VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%-20s : %s%s", "nexthop", "-", VTY_NEWLINE);
        }
    }

    vty_out(vty, "%-20s : %d%s", "domain id", pif->tunnel.domain_id ,VTY_NEWLINE);

    if (ENABLE == pif->tunnel.phb_enable)
    {
        vty_out(vty, "%-20s : %d%s", "phb id", pif->tunnel.phb_id, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-20s : %s%s", "phb id", "-", VTY_NEWLINE);
    }

    if (0 != pif->tunnel.car_cir[0])
    {
        if ((pif->tunnel.car_cir[0] & CAR_VALUE_L1_FLAG) == CAR_VALUE_L1_FLAG)
        {
            vty_out(vty, "%-20s : cir %u%s", "ingress car l1",
                        pif->tunnel.car_cir[0]&(~CAR_VALUE_L1_FLAG), VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%-20s : cir %u  pir %u%s", "ingress car", pif->tunnel.car_cir[0], pif->tunnel.car_pir[0], VTY_NEWLINE);
        }
    }
    else
    {
        vty_out(vty, "%-20s : cir %s  pir %s%s", "ingress car", "-", "-", VTY_NEWLINE);
    }

    if (0 != pif->tunnel.car_cir[1])
    {
        if ((pif->tunnel.car_cir[1] & CAR_VALUE_L1_FLAG) == CAR_VALUE_L1_FLAG)
        {
            vty_out(vty, "%-20s : cir %u%s", "egress car l1",
                        pif->tunnel.car_cir[1]&(~CAR_VALUE_L1_FLAG), VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "%-20s : cir %u  pir %u%s", "egress car", pif->tunnel.car_cir[1], pif->tunnel.car_pir[1], VTY_NEWLINE);
        }
    }
    else
    {
        vty_out(vty, "%-20s : cir %s  pir %s%s", "egress car", "-", "-", VTY_NEWLINE);
    }

    if (0 != pif->tunnel.hqos_id)
    {
        phqos = qos_hqos_get(pif->tunnel.hqos_id, MODULE_ID_MPLS);
        if (NULL != phqos)
        {
            vty_out(vty, "%-20s : %u%s", "hqos cir", phqos->cir, VTY_NEWLINE);

            if (phqos->pir != 0)
            {
                vty_out(vty, "%-20s : %u%s", "hqos pir", phqos->pir, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%-20s : %s%s", "hqos pir", "-", VTY_NEWLINE);
            }

            if (phqos->hqos_id != 0)
            {
                vty_out(vty, "%-20s : %u%s", "hqos queue-profile", phqos->hqos_id, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%-20s : %s%s", "hqos queue-profile", "-", VTY_NEWLINE);
            }
        }
    }
    else
    {
        vty_out(vty, "%-20s : %s%s", "hqos cir", "-", VTY_NEWLINE);
        vty_out(vty, "%-20s : %s%s", "hqos pir", "-", VTY_NEWLINE);
        vty_out(vty, "%-20s : %s%s", "hqos queue-profile", "-", VTY_NEWLINE);
    }

    vty_out(vty, "%-20s : %s%s", "statistics", pif->tunnel.statis_enable == ENABLE ?
                "enable" : "disable", VTY_NEWLINE);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tunnel 接口删除命令
 * @param[in ] : tunnel USP - tunnel 接口号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 13:51:19
 * @note       :
 */
DEFUN(no_tunnel_interface,
    no_tunnel_interface_cmd,
    "no interface tunnel USP",
    MPLS_CLI_INFO)
{
    uint32_t ifindex = 0;
    //struct ifm_usp if_usp;

    //memset(&if_usp, 0, sizeof(struct ifm_usp));

	ifindex = ifm_get_ifindex_by_name((char *)"tunnel", (char *)argv[0]);
    if (0 == ifindex)
    {
        vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[0], VTY_NEWLINE);

        return CMD_WARNING;
    }

    //ifm_get_usp_by_ifindex(ifindex, &if_usp);
    //ipran_alarm_port_unregister(IFM_TUNNEL_TYPE, if_usp.slot, if_usp.sub_port, 0, 0);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_TUNNEL_TYPE;
	gPortInfo.iIfindex = ifindex;
	ipran_alarm_port_unregister(&gPortInfo);
	
    tunnel_if_delete(ifindex);

    return CMD_SUCCESS;
}


/**
 * @brief      : tunnel 配置协议类型命令
 * @param[in ] : mpls-tp - mpls-tp 隧道
 * @param[in ] : gre     - gre 隧道
 * @param[in ] : mpls-te - te 隧道（暂时不支持）
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 13:56:52
 * @note       : 配置 tunnel 协议类型时创建 tunnel 接口结构
 */
DEFUN(tunnel_protocol,
    tunnel_protocol_cmd,
    "protocol (mpls-tp | gre | mpls-te)",
    MPLS_CLI_INFO)
{
    //struct ifm_usp    if_usp;
    struct tunnel_if  lif;
    struct tunnel_if *pif = NULL;
    int ret = 0;

    //memset(&if_usp, 0, sizeof(struct ifm_usp));
    memset(&lif, 0, sizeof(struct tunnel_if));

    lif.tunnel.ifindex = (uint32_t)vty->index;

    /* argv[0]: tunnel 协议类型 */
    if (0 == strncmp(argv[0], "mpls-tp", strlen("mpls-tp")))
    {
        lif.tunnel.protocol = TUNNEL_PRO_MPLSTP;
    }
    else if (0 == strncmp(argv[0], "mpls-te", strlen("mpls-te")))
    {
        lif.tunnel.protocol = TUNNEL_PRO_MPLSTE;
    }
    else if (argv[0][0] == 'g')
    {
        lif.tunnel.protocol = TUNNEL_PRO_GRE;
    }

    pif = tunnel_if_lookup(lif.tunnel.ifindex);
    if (NULL == pif)
    {
        pif = tunnel_if_create(lif.tunnel.ifindex);
        if (NULL == pif)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_MALLOC), VTY_NEWLINE);

            return CMD_WARNING;
        }

        pif->tunnel.protocol = lif.tunnel.protocol;

        if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)
        {
            /* tp tunnel 创建存储 lsp 结构 */
            pif->p_mplstp = (struct tunnel_mplstp *)XCALLOC(MTYPE_LSP_ENTRY,
                            sizeof(struct tunnel_mplstp));
            if (NULL == pif->p_mplstp)
            {
                vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_MALLOC), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }

        ret = tunnel_if_add(pif);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }

        ret = tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_INVALID, OPCODE_ADD);
        if (ERRNO_SUCCESS != ret)
        {
            pif->tunnel.protocol = TUNNEL_PRO_INVALID;

            tunnel_if_delete(lif.tunnel.ifindex);

            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else
    {
        /* 协议类型不允许重复配置 */
        if (TUNNEL_PRO_INVALID != pif->tunnel.protocol)
        {
            vty_error_out(vty, "Protocol has been configured.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        pif->tunnel.down_flag = LINK_DOWN;
        pif->tunnel.protocol  = lif.tunnel.protocol;
        pif->tunnel.failback  = FAILBACK_ENABLE;
        pif->tunnel.wtr       = 30;

        if ((TUNNEL_PRO_MPLSTP == pif->tunnel.protocol) && (NULL == pif->p_mplstp))
        {
            pif->p_mplstp = (struct tunnel_mplstp *)XCALLOC(MTYPE_LSP_ENTRY,
                            sizeof(struct tunnel_mplstp));
            if (NULL == pif->p_mplstp)
            {
                pif->tunnel.protocol = TUNNEL_PRO_INVALID;

                vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_MALLOC), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }

        ret = tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_PROTOCOL, OPCODE_UPDATE);
        if (ERRNO_SUCCESS != ret)
        {
            pif->tunnel.protocol = TUNNEL_PRO_INVALID;

            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    //ifm_get_usp_by_ifindex(pif->tunnel.ifindex, &if_usp);
    //ipran_alarm_port_register(IFM_TUNNEL_TYPE, if_usp.slot, if_usp.sub_port, 0, 0);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_TUNNEL_TYPE;
	gPortInfo.iIfindex = pif->tunnel.ifindex;
	ipran_alarm_port_register(&gPortInfo);

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除 tunnel 协议类型命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:25:26
 * @note       : 删除 tunnel 协议类型时删除 tunnel 接口结构
 */
DEFUN(no_tunnel_protocol,
    no_tunnel_protocol_cmd,
    "no protocol",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif      = NULL;
    struct lsp_entry *plsp_tmp = NULL;
    //struct ifm_usp    if_usp;
    uint32_t ifindex = (uint32_t)vty->index;
    int      ret     = 0;

    //memset(&if_usp, 0, sizeof(struct ifm_usp));

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }
    else
    {
        if (TUNNEL_PRO_INVALID == pif->tunnel.protocol)
        {
            return CMD_SUCCESS;
        }

        /* 删除 hqos、car、phb、domain */
        if ((0 != pif->tunnel.phb_id) && (ENABLE == pif->tunnel.phb_enable))
        {
            qos_phb_ref_operate(pif->tunnel.phb_id, QOS_TYPE_EXP, MODULE_ID_MPLS,
                                REF_OPER_DECREASE);
        }

        if (0 != pif->tunnel.domain_id)
        {
            qos_domain_ref_operate(pif->tunnel.domain_id, QOS_TYPE_EXP, MODULE_ID_MPLS,
                                    REF_OPER_DECREASE);
        }

        if (0 != pif->tunnel.hqos_id)
        {
            qos_hqos_delete(pif->tunnel.hqos_id, MODULE_ID_MPLS);
        }

        if (0 != pif->tunnel.bfd_id)
        {
            bfd_unbind_for_lsp_pw(BFD_TYPE_TUNNEL, pif->tunnel.bfd_id, pif->tunnel.ifindex);
        }

        if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)
        {
            if (NULL == pif->p_mplstp)
            {
                return CMD_SUCCESS;
            }

            if (NULL != pif->p_mplstp->ingress_lsp)
            {
                /* 解绑 bfd */
                if (0 != pif->p_mplstp->ingress_lsp->bfd_id)
                {
                    bfd_unbind_for_lsp_pw(BFD_TYPE_LSP, pif->p_mplstp->ingress_lsp->bfd_id,
                                        pif->p_mplstp->ingress_lsp->lsp_index);

                    pif->p_mplstp->ingress_lsp->bfd_id = 0;
                }

                /* 解绑 oam */
                if (0 != pif->p_mplstp->ingress_lsp->mplsoam_id)
                {
                    tunnel_if_unbind_mplstp_oam(pif, TUNNEL_STATUS_MASTER);
                }

                pif->p_mplstp->ingress_lsp->group_index = 0;

                plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->ingress_lsp->lsp_index);
                if (NULL != plsp_tmp)
                {
                    plsp_tmp->group_index = 0;
                }
            }

            if (NULL != pif->p_mplstp->egress_lsp)
            {
                pif->p_mplstp->egress_lsp->group_index = 0;

                plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->egress_lsp->lsp_index);
                if (NULL != plsp_tmp)
                {
                    plsp_tmp->group_index = 0;
                }
            }

            if (NULL != pif->p_mplstp->backup_ingress_lsp)
            {
                /* 解绑 bfd */
                if (0 != pif->p_mplstp->backup_ingress_lsp->bfd_id)
                {
                    bfd_unbind_for_lsp_pw(BFD_TYPE_LSP, pif->p_mplstp->backup_ingress_lsp->bfd_id,
                                        pif->p_mplstp->backup_ingress_lsp->lsp_index);

                    pif->p_mplstp->backup_ingress_lsp->bfd_id = 0;
                }

                /* 解绑 oam */
                if (0 != pif->p_mplstp->backup_ingress_lsp->mplsoam_id)
                {
                    tunnel_if_unbind_mplstp_oam(pif, TUNNEL_STATUS_BACKUP);
                }

                pif->p_mplstp->backup_ingress_lsp->group_index = 0;

                plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->backup_ingress_lsp->lsp_index);
                if (NULL != plsp_tmp)
                {
                    plsp_tmp->group_index = 0;
                }
            }

            if (NULL != pif->p_mplstp->backup_egress_lsp)
            {
                pif->p_mplstp->backup_egress_lsp->group_index = 0;

                plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->backup_egress_lsp->lsp_index);
                if (NULL != plsp_tmp)
                {
                    plsp_tmp->group_index = 0;
                }
            }

            if (LINK_UP == pif->tunnel.down_flag)
            {
                pif->tunnel.down_flag = LINK_DOWN;

                tunnel_if_down(pif);
            }

            memset(&pif->tunnel, 0, sizeof(struct tunnel_t));
            memset(pif->p_mplstp, 0, sizeof(struct tunnel_mplstp));
            pif->tunnel.ifindex   = ifindex;
            pif->tunnel.down_flag = LINK_DOWN;

            ret = tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_PROTOCOL, OPCODE_UPDATE);
            if (ERRNO_SUCCESS != ret)
            {
                vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else if (TUNNEL_PRO_GRE == pif->tunnel.protocol)
        {
            if (LINK_UP == pif->tunnel.down_flag)
            {
                pif->tunnel.down_flag = LINK_DOWN;

                tunnel_if_down(pif);
            }

            memset(&pif->tunnel, 0, sizeof(struct tunnel_t));
            pif->tunnel.ifindex   = ifindex;
            pif->tunnel.down_flag = LINK_DOWN;

            ret = tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_PROTOCOL, OPCODE_UPDATE);
            if (ERRNO_SUCCESS != ret)
            {
                vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
        else if (TUNNEL_PRO_MPLSTE == pif->tunnel.protocol)
        {
            pif->tunnel.protocol = TUNNEL_PRO_INVALID;

            /* 暂时不支持 */
        }
    }

    //ifm_get_usp_by_ifindex(ifindex, &if_usp);
    //ipran_alarm_port_unregister(IFM_TUNNEL_TYPE, if_usp.slot, if_usp.sub_port, 0, 0);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_TUNNEL_TYPE;
	gPortInfo.iIfindex = ifindex;
	ipran_alarm_port_unregister(&gPortInfo);

    return CMD_SUCCESS;
}


/**
 * @brief      : 配置 tunnel source destination ip 命令
 * @param[in ] : source A.B.C.D      - tunnel 源地址
 * @param[in ] : destination A.B.C.D - tunnel 目的地址
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:28:29
 * @note       :
 */
DEFUN(tunnel_source_destination_ip,
    tunnel_source_destination_ip_cmd,
    "source A.B.C.D destination A.B.C.D",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif = NULL;
    uint32_t source_ip;
    uint32_t dest_ip;
    uint32_t ifindex   = (uint32_t)vty->index;
    uint8_t  down_flag = LINK_DOWN;
    int      ret       = 0;

    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (TUNNEL_PRO_INVALID == pif->tunnel.protocol))
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    /* argv[0] : 原 IP */
    source_ip = inet_strtoipv4((char *)argv[0]);
    if (0 == source_ip)
    {
        vty_error_out(vty, "The source IP is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = inet_valid_network(source_ip);
    if (!ret)
    {
        vty_error_out(vty, "The source IP is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[1] : 目的 IP */
    dest_ip = inet_strtoipv4((char *)argv[1]);
    if (0 == dest_ip)
    {
        vty_error_out(vty, "The destination IP is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = inet_valid_network(dest_ip);
    if (!ret)
    {
        vty_error_out(vty, "The destination IP is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ((0 != pif->tunnel.sip.addr.ipv4) || (0 != pif->tunnel.dip.addr.ipv4))
    {
        vty_error_out(vty, "The tunnel IP already exist.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 原 IP 与新配置的相同 */
    if ((pif->tunnel.sip.addr.ipv4 == source_ip)
        && (pif->tunnel.dip.addr.ipv4 == dest_ip))
    {
        vty_error_out(vty, "The IP is repeated.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 原 IP 与目的 IP 相同 */
    if (source_ip == dest_ip)
    {
        vty_error_out(vty, "Source IP can't be the same as destination IP.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)      // mplstp tunnel 隧道 IP 更改限制
    {
        /* tunnel 绑定 lsp 不允许修改隧道 IP */
        if ((pif->tunnel.sip.addr.ipv4 != 0)
            && ((NULL != pif->p_mplstp->ingress_lsp)
                || (NULL != pif->p_mplstp->egress_lsp)
                || (NULL != pif->p_mplstp->backup_ingress_lsp)
                || (NULL != pif->p_mplstp->backup_egress_lsp)))
        {
            vty_warning_out(vty, "Please delete the bind lsp before change IP.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else if (TUNNEL_PRO_GRE == pif->tunnel.protocol)    // gre tunnel 判断 tunnel link 状态
    {
        if (LINK_DOWN == pif->tunnel.down_flag)
        {
            pif->tunnel.down_flag = LINK_UP;
        }

        down_flag = LINK_UP;
    }

    pif->tunnel.sip.addr.ipv4 = source_ip;
    pif->tunnel.sip.type      = INET_FAMILY_IPV4;
    pif->tunnel.dip.addr.ipv4 = dest_ip;
    pif->tunnel.dip.type      = INET_FAMILY_IPV4;

    ret = tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_IP, OPCODE_UPDATE);
    if (ERRNO_SUCCESS != ret)
    {
        pif->tunnel.sip.addr.ipv4 = 0;
        pif->tunnel.sip.type      = INET_FAMILY_INVALID;
        pif->tunnel.dip.addr.ipv4 = 0;
        pif->tunnel.dip.type      = INET_FAMILY_INVALID;
        pif->tunnel.down_flag     = LINK_DOWN;
        down_flag                 = LINK_DOWN;

        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (LINK_UP == down_flag)
    {
        tunnel_if_up(pif);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除 tunnel source destination ip 命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:32:40
 * @note       :
 */
DEFUN(no_tunnel_source_destination_ip,
    no_tunnel_source_destination_ip_cmd,
    "no source destination",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    int      ret     = 0;

    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (TUNNEL_PRO_INVALID == pif->tunnel.protocol))
    {
        return CMD_SUCCESS;
    }

    if ((0 == pif->tunnel.sip.addr.ipv4) && (0 == pif->tunnel.dip.addr.ipv4))
    {
        return CMD_SUCCESS;
    }

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)      // mplstp tunnel 删除隧道 IP 限制
    {
        if ((NULL != pif->p_mplstp->ingress_lsp)
            || (NULL != pif->p_mplstp->egress_lsp)
            || (NULL != pif->p_mplstp->backup_ingress_lsp)
            || (NULL != pif->p_mplstp->backup_egress_lsp))
        {
            vty_warning_out(vty, "Please delete the bind lsp before delete source IP.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else if (TUNNEL_PRO_GRE == pif->tunnel.protocol)    // gre tunnel 删除隧道 IP tunnel down
    {
        if (LINK_UP == pif->tunnel.down_flag)
        {
            pif->tunnel.sip.addr.ipv4 = 0;
            pif->tunnel.sip.type      = INET_FAMILY_INVALID;
            pif->tunnel.dip.addr.ipv4 = 0;
            pif->tunnel.dip.type      = INET_FAMILY_INVALID;
            pif->tunnel.down_flag     = LINK_DOWN;

            tunnel_if_down(pif);
        }
    }

    pif->tunnel.sip.addr.ipv4 = 0;
    pif->tunnel.sip.type      = INET_FAMILY_INVALID;
    pif->tunnel.dip.addr.ipv4 = 0;
    pif->tunnel.dip.type      = INET_FAMILY_INVALID;

    ret = tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_IP, OPCODE_UPDATE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : gre tunnel 配置下一跳命令
 * @param[in ] : nexthop A.B.C.D - 下一跳地址
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:34:32
 * @note       :
 */
DEFUN(tunnel_nexthop,
    tunnel_nexthop_cmd,
    "nexthop A.B.C.D",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    uint32_t nexthop = 0;
    int      ret     = 0;

    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (TUNNEL_PRO_INVALID == pif->tunnel.protocol))
    {
        vty_out(vty, "Warning: Please configure the interface"
                    " encapsulation protocol.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (TUNNEL_PRO_GRE != pif->tunnel.protocol)
    {
        vty_out(vty, "Error: Please configure interface encapsulation"
                    " protocol of GRE.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (0 != pif->tunnel.nexthop)
    {
        vty_out(vty, "Error: Nexthop has been configured.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    nexthop = inet_strtoipv4((char *)argv[0]);
    if (0 == nexthop)
    {
        vty_out(vty, "Error: The nexthop is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = inet_valid_network(nexthop);
    if (!ret)
    {
        vty_out(vty, "Error: The nexthop is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    pif->tunnel.nexthop = nexthop;

    ret = tunnel_if_nexthop_download(&pif->tunnel, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_out(vty, "Error: %s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : gre tunnel 删除下一跳命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:35:42
 * @note       :
 */
DEFUN(no_tunnel_nexthop,
    no_tunnel_nexthop_cmd,
    "no nexthop",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    int      ret     = 0;

    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (TUNNEL_PRO_INVALID == pif->tunnel.protocol))
    {
        return CMD_SUCCESS;
    }

    if (0 == pif->tunnel.nexthop)
    {
        return CMD_SUCCESS;
    }

    pif->tunnel.nexthop = 0;

    ret = tunnel_if_nexthop_download(&pif->tunnel, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_out(vty, "Error: %s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : gre tunnel 配置出接口命令
 * @param[in ] : ethernet USP           - 出接口类型为 ethernet
 * @param[in ] : gigabitethernet USP    - 出接口类型为 gigabitethernet
 * @param[in ] : xgigabitethernet USP   - 出接口类型为 xgigabitethernet
 * @param[in ] : trunk TRUNK            - 出接口类型为 trunk
 * @param[in ] : dmac XX:XX:XX:XX:XX:XX - 目的 mac 地址
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:36:09
 * @note       :
 */
DEFUN(tunnel_outif_dmac,
    tunnel_outif_dmac_cmd,
    "outif {ethernet USP | gigabitethernet USP | xgigabitethernet USP | trunk TRUNK} "
    "dmac XX:XX:XX:XX:XX:XX",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif = NULL;
    uint32_t ifindex   = (uint32_t)vty->index;
    uint32_t outif     = 0;
    int      ret       = 0;
    uchar dmac[6] = "";

    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (TUNNEL_PRO_INVALID == pif->tunnel.protocol))
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (TUNNEL_PRO_GRE != pif->tunnel.protocol)
    {
        vty_error_out(vty, "Please configure interface encapsulation"
                            " protocol of GRE.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (0 != pif->tunnel.outif)
    {
        vty_error_out(vty, "Out-going interface has been configured.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[0]、[1] : 出接口 */
    if (NULL != argv[0])
    {
        outif = ifm_get_ifindex_by_name("ethernet", argv[0]);
        if (0 == outif)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[0], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
	else if (NULL != argv[1])
    {
        outif = ifm_get_ifindex_by_name("gigabitethernet", argv[1]);
        if (0 == outif)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[1], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
	else if (NULL != argv[2])
    {
        outif = ifm_get_ifindex_by_name("xgigabitethernet", argv[2]);
        if (0 == outif)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[2], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else if (NULL != argv[3])
    {
        outif = ifm_get_ifindex_by_name("trunk", argv[3]);
        if (0 == outif)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[3], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    /* argv[2] : 目的 mac */
    if (ether_valid_mac((char *)argv[4]))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ether_string_to_mac((char *)argv[4], dmac);

    /* mac 地址全0、组播、广播 mac 无效 */
    if ((0 == ether_is_broadcast_mac(dmac))
        || (0 == ether_is_muticast_mac(dmac))
        || (0 == ether_is_zero_mac(dmac)))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    pif->set_flag     = 1;
    pif->tunnel.outif = outif;
    memcpy(pif->tunnel.dmac, dmac, 6);

    ret = tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_OUTIF, OPCODE_UPDATE);
    if (ERRNO_SUCCESS != ret)
    {
        pif->set_flag = 0;

        if (ERRNO_INTERFACE_NOT_EXIST != ret)
        {
            memset(pif->tunnel.dmac, 0, 6);
            pif->tunnel.outif = 0;

            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
        else if ((ERRNO_INTERFACE_NOT_EXIST == ret) && (1 != vty->config_read_flag))
        {
            memset(pif->tunnel.dmac, 0, 6);
            pif->tunnel.outif = 0;

            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : gre tunnel 删除出接口命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:40:07
 * @note       :
 */
DEFUN(no_tunnel_outif_dmac,
    no_tunnel_outif_dmac_cmd,
    "no outif",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    int      ret     = 0;

    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (TUNNEL_PRO_INVALID == pif->tunnel.protocol))
    {
        return CMD_SUCCESS;
    }

    if ((0 == pif->tunnel.outif) || (0 == pif->set_flag))
    {
        pif->set_flag     = 0;
        pif->tunnel.outif = 0;
        memset(pif->tunnel.dmac, 0, MAC_LEN);

        return CMD_SUCCESS;
    }

    pif->set_flag     = 0;
    pif->tunnel.outif = 0;
    memset(pif->tunnel.dmac, 0, MAC_LEN);

    ret = tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_OUTIF, OPCODE_UPDATE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tp tunnel 配置静态 lsp 命令
 * @param[in ] : ingress NAME - 配置 ingress lsp
 * @param[in ] : egress NAME  - 配置 egress lsp
 * @param[in ] : backup       - 配置备份 lsp
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:40:42
 * @note       :
 */
DEFUN(tunnel_static_lsp,
    tunnel_static_lsp_cmd,
    "mpls static-lsp ingress NAME {egress NAME | backup}",
    MPLS_CLI_INFO)
{
    struct static_lsp *plsp_ingress = NULL;
    struct static_lsp *plsp_egress  = NULL;
    struct tunnel_if  *pif          = NULL;
    enum TUNNEL_STATUS backup_flag = TUNNEL_STATUS_MASTER;
    uint32_t ifindex = (uint32_t)vty->index;

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (TUNNEL_PRO_MPLSTP != pif->tunnel.protocol)
    {
        vty_error_out(vty, "Please configure interface encapsulation"
                            " protocol of MPLS-TP.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 配置主备 lsp */
    if (NULL == argv[2])
    {
        if (NULL != pif->p_mplstp->ingress_lsp)
        {
            vty_error_out(vty, "The master LSP already exists, repeat configuration"
                            " please delete.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        backup_flag = TUNNEL_STATUS_MASTER;
    }
    else
    {
        if (NULL != pif->p_mplstp->backup_ingress_lsp)
        {
            vty_error_out(vty, "The backup LSP already exists, repeat configuration"
                            " please delete.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        backup_flag = TUNNEL_STATUS_BACKUP;
    }

    plsp_ingress = static_lsp_lookup((uchar *)argv[0]);
    if (NULL == plsp_ingress)
    {
        vty_error_out(vty, "Specify the static lsp '%s' does not exist.%s", argv[0], VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (LSP_DIRECTION_INGRESS != plsp_ingress->direction)
    {
        vty_error_out(vty, "The specified static Lsp is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 静态 lsp 已配置在其他 tunnel 上 */
    if (0 != plsp_ingress->group_index)
    {
        vty_error_out(vty, "The specified static lsp '%s' has been occupied.%s",
                        argv[0], VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 关联双向 lsp */
    if (NULL != argv[1])
    {
        plsp_egress = static_lsp_lookup((uchar *)argv[1]);
        if (NULL == plsp_egress)
        {
            vty_error_out(vty, "Specify the static lsp '%s' does not exist.%s",
                            argv[1], VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (LSP_DIRECTION_EGRESS != plsp_egress->direction)
        {
            vty_error_out(vty, "The specified static Lsp is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        /* 静态 lsp 已被其他接口选中 */
        if (0 != plsp_egress->group_index)
        {
            vty_error_out(vty, "The specified static lsp '%s' has been occupied.%s",
                                argv[1], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    /* tunnel destination 必须属于 ingress lsp destination */
    if (0 == ipv4_is_same_subnet(plsp_ingress->destip.addr.ipv4,
                                pif->tunnel.dip.addr.ipv4,
                                plsp_ingress->destip.prefixlen))
    {
        vty_error_out(vty, "The destination ip of tunnel is not belong "
                            "to static lsp '%s'.%s", plsp_ingress->name, VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != plsp_egress)
    {
        /* tunnel source 必须属于 egress lsp destination */
        if (0 == ipv4_is_same_subnet(plsp_egress->destip.addr.ipv4,
                                    pif->tunnel.sip.addr.ipv4,
                                    plsp_egress->destip.prefixlen))
        {
            vty_error_out(vty, "The source ip of tunnel is not belong to "
                            " static lsp '%s'.%s", plsp_egress->name, VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    tunnel_mplstp_add_lsp(pif, plsp_ingress, backup_flag);

    if (NULL != plsp_egress)
    {
        tunnel_mplstp_add_lsp(pif, plsp_egress, backup_flag);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tp tunnel 删除静态 lsp 命令
 * @param[in ] : backup - 删除备份 lsp
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:45:10
 * @note       :
 */
DEFUN(no_tunnel_static_lsp,
    no_tunnel_static_lsp_cmd,
    "no mpls static-lsp [backup]",
    MPLS_CLI_INFO)
{
    struct static_lsp *plsp_ingress = NULL;
    struct static_lsp *plsp_egress  = NULL;
    struct tunnel_if  *pif          = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    int      ret;

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if (TUNNEL_PRO_MPLSTP != pif->tunnel.protocol)
    {
        return CMD_SUCCESS;
    }

    if (NULL == argv[0])
    {
        plsp_ingress = pif->p_mplstp->ingress_lsp;
        plsp_egress  = pif->p_mplstp->egress_lsp;
    }
    else
    {
        plsp_ingress = pif->p_mplstp->backup_ingress_lsp;
        plsp_egress  = pif->p_mplstp->backup_egress_lsp;
    }

    if (NULL == plsp_ingress)
    {
        return ERRNO_SUCCESS;
    }

    if (NULL != plsp_egress)
    {
        ret = tunnel_mplstp_delete_lsp(pif, plsp_egress);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    ret = tunnel_mplstp_delete_lsp(pif, plsp_ingress);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 配置 tp tunnel 主备 lsp 回切模式及时间命令
 * @param[in ] : non-failback - 非回切模式
 * @param[in ] : failback     - 回切模式
 * @param[in ] : wtr <0-3600> - 回切时间
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:46:27
 * @note       :
 */
DEFUN(tunnel_failback,
    tunnel_failback_cmd,
    "backup {non-failback | failback | wtr <0-3600>}",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (TUNNEL_PRO_MPLSTP != pif->tunnel.protocol)
    {
        vty_error_out(vty, "Please configure interface encapsulation"
                            " protocol of MPLS-TP.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != argv[0])
    {
        pif->tunnel.failback = FAILBACK_DISABLE;
    }
    else if (NULL != argv[1])
    {
        pif->tunnel.failback = FAILBACK_ENABLE;
        pif->tunnel.wtr      = 30;

        if (NULL != argv[2])
        {
            pif->tunnel.wtr = atoi(argv[2]);
        }

        tunnel_mplstp_process_status(pif);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tunnel 配置 qos exp-domain 命令
 * @param[in ] : exp-domain <1-5> - exp-domain 模板
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:48:01
 * @note       :
 */
DEFUN(tunnel_qos_mapping_exp_domain_enable,
    tunnel_qos_mapping_exp_domain_enable_cmd,
    "qos mapping enable exp-domain <1-5>",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif      = NULL;
    struct lsp_entry *plsp_tmp = NULL;
    int      *pexist  = NULL;
    uint32_t  ifindex = (uint32_t)vty->index;
    uint8_t   index   = 0;

    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (TUNNEL_PRO_INVALID == pif->tunnel.protocol))
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    if (TUNNEL_PRO_GRE == pif->tunnel.protocol)
    {
        vty_error_out(vty, "GRE does not support the configuration of DOMAIN !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (0 != pif->tunnel.domain_id)
    {
        vty_error_out(vty, "domain already enable !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    index = atoi(argv[0]);

    pexist = qos_domain_exist_get(index, QOS_TYPE_EXP, MODULE_ID_MPLS);
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

    pif->tunnel.domain_id = index;

    qos_domain_ref_operate(index, QOS_TYPE_EXP, MODULE_ID_MPLS, REF_OPER_INCREASE);

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)
    {
        if (NULL != pif->p_mplstp->egress_lsp)
        {
            plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->egress_lsp->lsp_index);
            if (NULL != plsp_tmp)
            {
                plsp_tmp->domain_id = pif->tunnel.domain_id;

                mpls_lsp_update(plsp_tmp, LSP_SUBTYPE_QOS);
            }
        }

        if (NULL != pif->p_mplstp->backup_egress_lsp)
        {
            plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->backup_egress_lsp->lsp_index);
            if (NULL != plsp_tmp)
            {
                plsp_tmp->domain_id = pif->tunnel.domain_id;

                mpls_lsp_update(plsp_tmp, LSP_SUBTYPE_QOS);
            }
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tunnel 删除 qos exp-domain 命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:49:58
 * @note       :
 */
DEFUN(no_tunnel_qos_mapping_exp_domain_enable,
    no_tunnel_qos_mapping_exp_domain_enable_cmd,
    "no qos mapping enable exp-domain",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif      = NULL;
    struct lsp_entry *plsp_tmp = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    if (0 == pif->tunnel.domain_id)
    {
        return CMD_SUCCESS;
    }

    qos_domain_ref_operate(pif->tunnel.domain_id, QOS_TYPE_EXP, MODULE_ID_MPLS,
                            REF_OPER_DECREASE);

    pif->tunnel.domain_id = 0;

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)
    {
        if (NULL != pif->p_mplstp->egress_lsp)
        {
            plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->egress_lsp->lsp_index);
            if (NULL != plsp_tmp)
            {
                plsp_tmp->domain_id = 0;

                mpls_lsp_update(plsp_tmp, LSP_SUBTYPE_QOS);
            }
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tunnel 配置 qos exp-phb 命令
 * @param[in ] : exp-domain <1-5> - exp-phb 模板
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:50:33
 * @note       :
 */
DEFUN(tunnel_qos_mapping_exp_phb_enable,
    tunnel_qos_mapping_exp_phb_enable_cmd,
    "qos mapping enable exp-phb <0-5>",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif      = NULL;
    struct lsp_entry *plsp_tmp = NULL;
    int              *pexist   = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    uint8_t  index   = 0;

    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (TUNNEL_PRO_INVALID == pif->tunnel.protocol))
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    if (TUNNEL_PRO_GRE == pif->tunnel.protocol)
    {
        vty_error_out(vty, "GRE does not support the configuration of PHB !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ((ENABLE == pif->tunnel.phb_enable) && (0 != pif->tunnel.phb_id))
    {
        vty_error_out(vty, "phb already enable !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    index = atoi(argv[0]);

    pexist = qos_phb_exist_get(index, QOS_TYPE_EXP, MODULE_ID_MPLS);
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

    pif->tunnel.phb_id     = index;
    pif->tunnel.phb_enable = ENABLE;

    qos_phb_ref_operate(index, QOS_TYPE_EXP, MODULE_ID_MPLS, REF_OPER_INCREASE);

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)
    {
        if (NULL != pif->p_mplstp->ingress_lsp)
        {
            plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->ingress_lsp->lsp_index);
            if (NULL != plsp_tmp)
            {
                plsp_tmp->phb_id     = pif->tunnel.phb_id;
                plsp_tmp->phb_enable = ENABLE;

                mpls_lsp_update(plsp_tmp, LSP_SUBTYPE_QOS);
            }
        }

        if (NULL != pif->p_mplstp->backup_ingress_lsp)
        {
            plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->backup_ingress_lsp->lsp_index);
            if (NULL != plsp_tmp)
            {
                plsp_tmp->phb_id     = pif->tunnel.phb_id;
                plsp_tmp->phb_enable = ENABLE;

                mpls_lsp_update(plsp_tmp, LSP_SUBTYPE_QOS);
            }
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tunnel 删除 qos exp-phb 命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:51:54
 * @note       :
 */
DEFUN(no_tunnel_qos_mapping_exp_phb_enable,
    no_tunnel_qos_mapping_exp_phb_enable_cmd,
    "no qos mapping enable exp-phb",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif      = NULL;
    struct lsp_entry *plsp_tmp = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    if (DISABLE == pif->tunnel.phb_enable)
    {
        return CMD_SUCCESS;
    }

    qos_phb_ref_operate(pif->tunnel.phb_id, QOS_TYPE_EXP, MODULE_ID_MPLS, REF_OPER_DECREASE);

    pif->tunnel.phb_id     = 0;
    pif->tunnel.phb_enable = DISABLE;

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)
    {
        if (NULL != pif->p_mplstp->ingress_lsp)
        {
            plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->ingress_lsp->lsp_index);
            if (NULL != plsp_tmp)
            {
                plsp_tmp->phb_id     = 0;
                plsp_tmp->phb_enable = DISABLE;

                mpls_lsp_update(plsp_tmp, LSP_SUBTYPE_QOS);
            }
        }

        if (NULL != pif->p_mplstp->backup_ingress_lsp)
        {
            plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->backup_ingress_lsp->lsp_index);
            if (NULL != plsp_tmp)
            {
                plsp_tmp->phb_id     = 0;
                plsp_tmp->phb_enable = DISABLE;

                mpls_lsp_update(plsp_tmp, LSP_SUBTYPE_QOS);
            }
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 tunnel 配置信息命令
 * @param[in ] : USP - tunnel 接口号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:53:32
 * @note       :
 */
DEFUN(tunnel_mpls_show,
    tunnel_mpls_show_cmd,
    "show tunnel [USP]",
    MPLS_CLI_INFO)
{
    struct hash_bucket *pbucket = NULL;
    struct tunnel_if   *pif     = NULL;
    uint32_t ifindex = 0;
    int      cursor  = 0;
    int      num     = 0;

    if (NULL == argv[0])
    {
        HASH_BUCKET_LOOP(pbucket, cursor, tunnel_table)
        {
            pif = pbucket->data;

            if (NULL != pif)
            {
                if (++num == 1)
                {
                    vty_out(vty, "%s", VTY_NEWLINE);
                    vty_out(vty, "%s %d:%s", "total", tunnel_table.num_entries, VTY_NEWLINE);
                    vty_out(vty, "%s", VTY_NEWLINE);
                    vty_out(vty, "%-16s", "interface");
                    vty_out(vty, "%-9s", "protocol");
                    vty_out(vty, "%-16s", "source ip");
                    vty_out(vty, "%-16s", "destination ip");
                    vty_out(vty, "%-7s", "status");
                    vty_out(vty, "%s", VTY_NEWLINE);
                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                tunnel_if_show_brief(vty, pif);
            }
        }

        if (0 != num)
        {
            vty_out(vty, "%s", VTY_NEWLINE);
        }
    }
    else
    {
        ifindex = ifm_get_ifindex_by_name ((char *)"tunnel", (char *)argv[0]);

        pif = tunnel_if_lookup(ifindex);
        if (NULL == pif)
        {
            return CMD_SUCCESS;
        }

        tunnel_if_show_verbose(vty, pif);

        vty_out(vty, "%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tunnel 统计使能命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:57:07
 * @note       :
 */
DEFUN(tunnel_statistics_enable,
    tunnel_statistics_enable_cmd,
    "statistics enable",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif   = NULL;
    struct lsp_entry *plspm = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    int      ret     = 0;

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_warning_out(vty, "Please configure the interface"
                    " encapsulation protocol.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    if (ENABLE == pif->tunnel.statis_enable)
    {
        return CMD_SUCCESS;
    }

    pif->tunnel.statis_enable = ENABLE;

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)
    {
        if (NULL == pif->p_mplstp)
        {
            return CMD_SUCCESS;
        }

        if (NULL != pif->p_mplstp->ingress_lsp)
        {
            plspm = mpls_lsp_lookup(pif->p_mplstp->ingress_lsp->lsp_index);
            if (NULL != plspm)
            {
                plspm->statis_enable = ENABLE;

                mpls_lsp_update(plspm, LSP_SUBTYPE_COUNTER);
            }
        }

        if (NULL != pif->p_mplstp->egress_lsp)
        {
            plspm = mpls_lsp_lookup(pif->p_mplstp->egress_lsp->lsp_index);
            if (NULL != plspm)
            {
                plspm->statis_enable = ENABLE;

                mpls_lsp_update(plspm, LSP_SUBTYPE_COUNTER);
            }
        }

        if (NULL != pif->p_mplstp->backup_ingress_lsp)
        {
            plspm = mpls_lsp_lookup(pif->p_mplstp->backup_ingress_lsp->lsp_index);
            if (NULL != plspm)
            {
                plspm->statis_enable = ENABLE;

                mpls_lsp_update(plspm, LSP_SUBTYPE_COUNTER);
            }
        }

        if (NULL != pif->p_mplstp->backup_egress_lsp)
        {
            plspm = mpls_lsp_lookup(pif->p_mplstp->backup_egress_lsp->lsp_index);
            if (NULL != plspm)
            {
                plspm->statis_enable = ENABLE;

                mpls_lsp_update(plspm, LSP_SUBTYPE_COUNTER);
            }
        }
    }

    ret = MPLS_IPC_SENDTO_HAL(&pif->tunnel, sizeof(struct tunnel_t), 1,
                        MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
                        IPC_OPCODE_UPDATE, TUNNEL_INFO_COUNTER, pif->tunnel.ifindex);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    tunnel_if_set_pw_statistics(pif, ENABLE);

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除 tunnel 统计使能命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 15:59:01
 * @note       :
 */
DEFUN(no_tunnel_statistics_enable,
    no_tunnel_statistics_enable_cmd,
    "no statistics enable",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif   = NULL;
    struct lsp_entry *plspm = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    int      ret     = 0;

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    if (DISABLE == pif->tunnel.statis_enable)
    {
        return CMD_SUCCESS;
    }

    pif->tunnel.statis_enable = DISABLE;

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)
    {
        if (NULL == pif->p_mplstp)
        {
            return CMD_SUCCESS;
        }

        if (NULL != pif->p_mplstp->ingress_lsp)
        {
            plspm = mpls_lsp_lookup(pif->p_mplstp->ingress_lsp->lsp_index);
            if (NULL != plspm)
            {
                plspm->statis_enable = DISABLE;

                mpls_lsp_update(plspm, LSP_SUBTYPE_COUNTER);
            }
        }

        if (NULL != pif->p_mplstp->egress_lsp)
        {
            plspm = mpls_lsp_lookup(pif->p_mplstp->egress_lsp->lsp_index);
            if (NULL != plspm)
            {
                plspm->statis_enable = DISABLE;

                mpls_lsp_update(plspm, LSP_SUBTYPE_COUNTER);
            }
        }

        if (NULL != pif->p_mplstp->backup_ingress_lsp)
        {
            plspm = mpls_lsp_lookup(pif->p_mplstp->backup_ingress_lsp->lsp_index);
            if (NULL != plspm)
            {
                plspm->statis_enable = DISABLE;

                mpls_lsp_update(plspm, LSP_SUBTYPE_COUNTER);
            }
        }

        if (NULL != pif->p_mplstp->backup_egress_lsp)
        {
            plspm = mpls_lsp_lookup(pif->p_mplstp->backup_egress_lsp->lsp_index);
            if (NULL != plspm)
            {
                plspm->statis_enable = DISABLE;

                mpls_lsp_update(plspm, LSP_SUBTYPE_COUNTER);
            }
        }
    }

	ret = MPLS_IPC_SENDTO_HAL(&pif->tunnel, sizeof(struct tunnel_t), 1,
                        MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
                        IPC_OPCODE_UPDATE, TUNNEL_INFO_COUNTER, pif->tunnel.ifindex);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    tunnel_if_set_pw_statistics(pif, DISABLE);

    return CMD_SUCCESS;
}


/**
 * @brief      : 清除 tunnel 统计计数命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:00:05
 * @note       :
 */
DEFUN(tunnel_statistics_clear,
    tunnel_statistics_clear_cmd,
    "statistics clear",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    int      ret     = 0;

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    ret = MPLS_IPC_SENDTO_HAL(&pif->tunnel, sizeof(struct tunnel_t), 1,
                        MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
                        IPC_OPCODE_DELETE, TUNNEL_INFO_COUNTER, ifindex);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : config 视图清除 tunnel 统计命令
 * @param[in ] : tunnel USP - tunnel 接口号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:01:00
 * @note       :
 */
DEFUN(tunnel_statistics_clear_interface,
    tunnel_statistics_clear_interface_cmd,
    "statistics clear interface tunnel USP",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif = NULL;
    uint32_t ifindex = 0;
    int      ret     = 0;

    ifindex = ifm_get_ifindex_by_name("tunnel", (char *)argv[0]);
    if (0 == ifindex)
    {
        vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[0], VTY_NEWLINE);

        return CMD_WARNING;
    }

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    ret = MPLS_IPC_SENDTO_HAL(&pif->tunnel, sizeof(struct tunnel_t), 1,
                        MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
                        IPC_OPCODE_DELETE, TUNNEL_INFO_COUNTER, ifindex);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tunnel 统计显示命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:05:09
 * @note       :
 */
DEFUN(show_tunnel_statistics,
    show_tunnel_statistics_cmd,
    "show statistics",
    MPLS_CLI_INFO)
{
    char ifname[STRING_LEN]    = "";
    struct tunnel_if *pif      = NULL;
    struct counter_t *pcounter = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    struct ipc_mesg_n  *pmesg = NULL;

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    if (DISABLE == pif->tunnel.statis_enable)
    {
        vty_warning_out(vty, "Statistics is disabled !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ifm_get_name_by_ifindex(ifindex, ifname);

    pmesg = ipc_sync_send_n2(&pif->tunnel, sizeof(struct tunnel_t), 1,
                                        MODULE_ID_HAL, MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
                                        TUNNEL_INFO_COUNTER, IPC_OPCODE_GET, ifindex, 1);
    if(NULL == pmesg)
    {
        return CMD_WARNING;
    }
    pcounter = (struct counter_t *)pmesg->msg_data;
    if (pmesg->msghdr.data_len)
    {
        vty_out(vty, "Interface %s statistics%s", ifname, VTY_NEWLINE);
        vty_out(vty, " Input  Total: %llu bytes, %llu packets, Dorp: %llu packets%s",
                pcounter->rx_bytes, pcounter->rx_packets, pcounter->rx_dropped, VTY_NEWLINE);
        vty_out(vty, " Output Total: %llu bytes, %llu packets, Dorp: %llu packets%s",
                pcounter->tx_bytes, pcounter->tx_packets, pcounter->tx_dropped, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "Interface %s statistics%s", ifname, VTY_NEWLINE);
        vty_out(vty, " Input  Total: 0 bytes, 0 packets, Dorp: 0 packets%s", VTY_NEWLINE);
        vty_out(vty, " Output Total: 0 bytes, 0 packets, Dorp: 0 packets%s", VTY_NEWLINE);
    }

    vty_out(vty,"%s",VTY_NEWLINE);
    mem_share_free(pmesg, MODULE_ID_MPLS);
    return CMD_SUCCESS;
}


/**
 * @brief      : tp tunnel 的 lsp 上绑定 oam 命令
 * @param[in ] : session <1-65535> - oam 会话 id
 * @param[in ] : master-lsp        - 主 lsp
 * @param[in ] : slave-lsp         - 备 lsp
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:02:25
 * @note       :
 */
DEFUN(tunnel_mplstp_oam_session_enable,
    tunnel_mplstp_oam_session_enable_cmd,
    "mplstp-oam enable session <1-65535> (master-lsp | slave-lsp)",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    uint16_t oam_id  = 0;
    int      ret     = 0;
	struct oam_session *psess = NULL;

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (TUNNEL_PRO_MPLSTP != pif->tunnel.protocol)
    {
        vty_error_out(vty, "Please configure interface encapsulation"
                            " protocol of MPLS-TP.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

	oam_id = atoi(argv[0]);

	psess = mplsoam_session_lookup(oam_id);
    if(NULL == psess)
    {
		vty_error_out(vty, "mplsoam session %d not exist %s",oam_id, VTY_NEWLINE);
		return CMD_WARNING;
    }	

    /* 配置 tunnel 的 mplstp-oam */
    if (NULL == argv[1])
    {
        if (0 != pif->tunnel.mplsoam_id)
        {
            vty_error_out(vty, "Tunnel already bind mplstp-oam: %d%s",
                                pif->tunnel.mplsoam_id, VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (0 != pif->tunnel.bfd_id)
        {
            vty_error_out(vty, "Tunnel already bind bfd: %d%s",
                                pif->tunnel.bfd_id, VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (TUNNEL_IS_NOT_BIDIRICTIONAL(pif))
        {
            vty_error_out(vty, "Configuration tpoam, tunnel must be directional !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        ret = mplsoam_session_enable(oam_id, pif->tunnel.ifindex, OAM_TYPE_TUNNEL);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }

        /* tunnel 配置 tp-oam 自动使能统计 */
        pif->tunnel.statis_enable = ENABLE;

        ret = tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_MPLSTP_OAM, OPCODE_UPDATE);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

            return CMD_WARNING;
        }

        pif->tunnel.mplsoam_id = oam_id;

        return CMD_SUCCESS;
    }

    /* 配置 tunnel 下 lsp 的 mplstp-oam */
    if (argv[1][0] == 'm')
    {
        if ((NULL == pif->p_mplstp->ingress_lsp) || (NULL == pif->p_mplstp->egress_lsp))
        {
            vty_error_out(vty, "Configuration tpoam, tunnel must be directional !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        pif->tunnel.statis_enable = ENABLE;

		if(vty->config_read_flag)
		{
			psess->info.index = pif->p_mplstp->ingress_lsp->lsp_index;
			psess->info.type  = OAM_TYPE_LSP;
			
			return CMD_SUCCESS;
		}

        ret = tunnel_if_bind_mplstp_oam(pif, oam_id, TUNNEL_STATUS_MASTER);
    }
    else if (argv[1][0] == 's')
    {
        if ((NULL == pif->p_mplstp->backup_ingress_lsp)
            || (NULL == pif->p_mplstp->backup_egress_lsp))
        {
            vty_error_out(vty, "Configuration tpoam, tunnel must be directional !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        pif->tunnel.statis_enable = ENABLE;

		if(vty->config_read_flag)
		{
			psess->info.index = pif->p_mplstp->backup_ingress_lsp->lsp_index;
			psess->info.type  = OAM_TYPE_LSP;
			
			return CMD_SUCCESS;
		}
		

        ret = tunnel_if_bind_mplstp_oam(pif, oam_id, TUNNEL_STATUS_BACKUP);
    }

	if (ERRNO_SUCCESS != ret)
	{
		vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

		return CMD_WARNING;
	}

    return CMD_SUCCESS;
}


/**
 * @brief      : tp tunnel 的 lsp 上解绑 oam 命令
 * @param[in ] : master-lsp - 主 lsp
 * @param[in ] : slave-lsp  - 备 lsp
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:08:31
 * @note       :
 */
DEFUN(no_tunnel_mplstp_oam_session_enable,
    no_tunnel_mplstp_oam_session_enable_cmd,
    "no mplstp-oam enable session (master-lsp | slave-lsp)",
    MPLS_CLI_INFO)
{
    struct tunnel_if  *pif   = NULL;
    struct static_lsp *pslsp = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (pif->p_mplstp == NULL))
    {
		return CMD_SUCCESS;
    }

    if (NULL == argv[0])
    {
        if (0 != pif->tunnel.mplsoam_id)
        {
            mplsoam_session_disable(pif->tunnel.mplsoam_id);

            pif->tunnel.mplsoam_id = 0;

            tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_MPLSTP_OAM, OPCODE_UPDATE);
        }

        return CMD_SUCCESS;
    }

    if (argv[0][0] == 'm')
    {
        tunnel_if_unbind_mplstp_oam(pif, TUNNEL_STATUS_MASTER);

        pslsp = pif->p_mplstp->ingress_lsp;
    }
    else if (argv[0][0] == 's')
    {
        tunnel_if_unbind_mplstp_oam(pif, TUNNEL_STATUS_BACKUP);

        pslsp = pif->p_mplstp->backup_ingress_lsp;
    }

    if (NULL != pslsp)
    {
        static_lsp_enable(pslsp);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tp tunnel 的 lsp 上绑定 bfd 命令
 * @param[in ] : session <1-65535> - bfd 会话 id
 * @param[in ] : master-lsp        - 主 lsp
 * @param[in ] : slave-lsp         - 备 lsp
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:10:13
 * @note       :
 */
DEFUN(tunnel_bfd_session_enable,
    tunnel_bfd_session_enable_cmd,
    "bfd enable session <1-65535> (master-lsp | slave-lsp)",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    uint16_t bfd_id  = 0;
    int      ret     = 0;

    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (TUNNEL_PRO_MPLSTP != pif->tunnel.protocol)
    {
        vty_error_out(vty, "Please configure interface encapsulation"
                            " protocol of MPLS-TP.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

	bfd_id = atoi(argv[0]);

    if (NULL == argv[1])
    {
        vty_warning_out(vty, "bfd for tunnel isn`t support now, Please check command.%s", VTY_NEWLINE);

        return CMD_WARNING;

        if (0 != pif->tunnel.bfd_id)
        {
            vty_error_out(vty, "Tunnel already bind bfd: %d%s",
                            pif->tunnel.bfd_id, VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (0 != pif->tunnel.mplsoam_id)
        {
            vty_error_out(vty, "Tunnel already bind mplstp-oam: %d%s",
                            pif->tunnel.mplsoam_id, VTY_NEWLINE);

            return CMD_WARNING;
        }

        ret = bfd_bind_for_lsp_pw(BFD_TYPE_TUNNEL, bfd_id, pif->tunnel.ifindex, 0);
        if (ERRNO_SUCCESS != ret)
    	{
    		vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

    		return CMD_WARNING;
    	}

        pif->tunnel.bfd_id = bfd_id;

        return CMD_SUCCESS;
    }

    if ( 0 == strncmp(argv[1], "m", 1) )
    {
        if (pif->p_mplstp->ingress_lsp == NULL)
        {
            vty_error_out(vty, "The master lsp dose not exist !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

		if(pif->p_mplstp->egress_lsp == NULL)
		{
			vty_error_out(vty, "This tunnel haven`t bound egress_lsp, please bind!%s", VTY_NEWLINE);

            return CMD_WARNING;
		}

        if (pif->p_mplstp->ingress_lsp->bfd_id != 0)
        {
            vty_error_out(vty, "The master lsp already bind bfd : %d.%s",
                        pif->p_mplstp->ingress_lsp->bfd_id, VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (pif->p_mplstp->ingress_lsp->mplsoam_id != 0)
        {
            vty_error_out(vty, "The master lsp already bind mplstp-oam : %d.%s",
                        pif->p_mplstp->ingress_lsp->mplsoam_id, VTY_NEWLINE);

            return CMD_WARNING;
        }

        ret = bfd_bind_for_lsp_pw(BFD_TYPE_LSP, bfd_id, pif->p_mplstp->ingress_lsp->lsp_index, 0);
        if (ERRNO_SUCCESS != ret)
    	{
    		vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

    		return CMD_WARNING;
    	}

        pif->p_mplstp->ingress_lsp->bfd_id = bfd_id;
    }
    if ( 0 == strncmp(argv[1], "s", 1) )
    {
        if (pif->p_mplstp->backup_ingress_lsp == NULL)
        {
            vty_error_out(vty, "The slave lsp dose not exist !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

		if(pif->p_mplstp->backup_egress_lsp == NULL)
		{
			vty_error_out(vty, "This tunnel haven`t bound backup_egress_lsp, please bind!%s", VTY_NEWLINE);

            return CMD_WARNING;
		}

        if (pif->p_mplstp->backup_ingress_lsp->bfd_id != 0)
        {
            vty_error_out(vty, "The slave lsp already bind bfd : %d.%s",
                            pif->p_mplstp->backup_ingress_lsp->bfd_id, VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (pif->p_mplstp->backup_ingress_lsp->mplsoam_id != 0)
        {
            vty_error_out(vty, "The slave lsp already bind mplstp-oam : %d.%s",
                            pif->p_mplstp->backup_ingress_lsp->mplsoam_id, VTY_NEWLINE);

            return CMD_WARNING;
        }

        ret = bfd_bind_for_lsp_pw(BFD_TYPE_LSP, bfd_id, pif->p_mplstp->backup_ingress_lsp->lsp_index, 0);
        if (ERRNO_SUCCESS != ret)
    	{
    		vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
    	}

        pif->p_mplstp->backup_ingress_lsp->bfd_id = bfd_id;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tp tunnel 的 lsp 上解绑 bfd 命令
 * @param[in ] : master-lsp - 主 lsp
 * @param[in ] : slave-lsp  - 备 lsp
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:13:20
 * @note       :
 */
DEFUN(no_tunnel_bfd_session_enable,
    no_tunnel_bfd_session_enable_cmd,
    "no bfd enable session {master-lsp | slave-lsp}",
    MPLS_CLI_INFO)
{
    struct tunnel_if  *pif   = NULL;
    struct static_lsp *pslsp = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (pif->p_mplstp == NULL))
    {
		return CMD_SUCCESS;
    }

    if ((NULL == argv[0]) && (NULL == argv[1]))
    {
        vty_warning_out(vty, "bfd for tunnel isn`t support now, Please check command.%s", VTY_NEWLINE);

        return CMD_WARNING;

        if (0 != pif->tunnel.bfd_id)
        {
            bfd_unbind_for_lsp_pw(BFD_TYPE_TUNNEL, pif->tunnel.bfd_id, pif->tunnel.ifindex);

            pif->tunnel.bfd_id = 0;
        }

        return CMD_SUCCESS;
    }

    if (NULL != argv[0])
    {
        if ((pif->p_mplstp->ingress_lsp != NULL) && (0 != pif->p_mplstp->ingress_lsp->bfd_id))
        {
            bfd_unbind_for_lsp_pw(BFD_TYPE_LSP, pif->p_mplstp->ingress_lsp->bfd_id,
                                pif->p_mplstp->ingress_lsp->lsp_index);

            pif->p_mplstp->ingress_lsp->bfd_id = 0;

            pslsp = pif->p_mplstp->ingress_lsp;
        }
    }
    else if (NULL != argv[1])
    {
        if ((pif->p_mplstp->backup_ingress_lsp != NULL)
        && (0 != pif->p_mplstp->backup_ingress_lsp->bfd_id))
        {
            bfd_unbind_for_lsp_pw(BFD_TYPE_LSP, pif->p_mplstp->backup_ingress_lsp->bfd_id,
                                pif->p_mplstp->backup_ingress_lsp->lsp_index);

            pif->p_mplstp->backup_ingress_lsp->bfd_id = 0;

            pslsp = pif->p_mplstp->backup_ingress_lsp;
        }
    }

    if (NULL != pslsp)
    {
        static_lsp_enable(pslsp);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tunnel 配置 qos car 命令
 * @param[in ] : ingress          - 入方向限速
 * @param[in ] : egress           - 出方向限速
 * @param[in ] : cir <1-10000000> - 承诺信息率
 * @param[in ] : pir <1-10000000> - 峰值信息率
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:15:52
 * @note       :
 */
DEFUN(tunnel_qos_car,
    tunnel_qos_car_cmd,
    "qos car (ingress | egress) cir <1-10000000> pir <1-10000000>",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif  = NULL;
    struct lsp_entry *plsp = NULL;
    enum LSP_DIRECTION dir = LSP_DIRECTION_INGRESS;
    uint32_t ifindex = 0;
    uint32_t cir     = 0;
    uint32_t pir     = 0;
    int      ret     = 0;

    ifindex = (uint32_t)vty->index;
    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (TUNNEL_PRO_INVALID == pif->tunnel.protocol))
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    /* argv[0] : direction */
    if (argv[0][0] == 'i')
    {
        if (0 != pif->tunnel.car_cir[0])
        {
            vty_error_out(vty, "Ingress car already configuration !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        dir = LSP_DIRECTION_EGRESS;
    }
    else if (argv[0][0] == 'e')
    {
        if (0 != pif->tunnel.car_cir[1])
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

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)
    {
        if (LSP_DIRECTION_INGRESS == dir)
        {
            if (cir < pif->tunnel.car_cir_sum[1])
            {
                vty_out(vty, "egress CAR CAC check failed !%s", VTY_NEWLINE);

                return CMD_WARNING;
            }

            pif->tunnel.car_cir[1] = cir;
            pif->tunnel.car_pir[1] = pir;

            if (NULL != pif->p_mplstp->ingress_lsp)
            {
                plsp = mpls_lsp_lookup(pif->p_mplstp->ingress_lsp->lsp_index);
                if (NULL != plsp)
                {
                    plsp->car_cir[1] = cir;
                    plsp->car_pir[1] = pir;

                    mpls_lsp_update(plsp, LSP_SUBTYPE_CAR);
                }
            }

            if (NULL != pif->p_mplstp->backup_ingress_lsp)
            {
                plsp = mpls_lsp_lookup(pif->p_mplstp->backup_ingress_lsp->lsp_index);
                if (NULL != plsp)
                {
                    plsp->car_cir[1] = cir;
                    plsp->car_pir[1] = pir;

                    mpls_lsp_update(plsp, LSP_SUBTYPE_CAR);
                }
            }
        }
        else if (LSP_DIRECTION_EGRESS == dir)
        {
            if (cir < pif->tunnel.car_cir_sum[0])
            {
                vty_out(vty, "ingress CAR CAC check failed !%s", VTY_NEWLINE);

                return CMD_WARNING;
            }

            pif->tunnel.car_cir[0] = cir;
            pif->tunnel.car_pir[0] = pir;

            if (NULL != pif->p_mplstp->egress_lsp)
            {
                plsp = mpls_lsp_lookup(pif->p_mplstp->egress_lsp->lsp_index);
                if (NULL != plsp)
                {
                    plsp->car_cir[0] = cir;
                    plsp->car_pir[0] = pir;

                    mpls_lsp_update(plsp, LSP_SUBTYPE_CAR);
                }
            }

            if (NULL != pif->p_mplstp->backup_egress_lsp)
            {
                plsp = mpls_lsp_lookup(pif->p_mplstp->backup_egress_lsp->lsp_index);
                if (NULL != plsp)
                {
                    plsp->car_cir[0] = cir;
                    plsp->car_pir[0] = pir;

                    mpls_lsp_update(plsp, LSP_SUBTYPE_CAR);
                }
            }
        }
    }
    else if (TUNNEL_PRO_GRE == pif->tunnel.protocol)
    {
        if (LSP_DIRECTION_INGRESS == dir)
        {
            if (cir < pif->tunnel.car_cir_sum[1])
            {
                vty_out(vty, "egress CAR CAC check failed !%s", VTY_NEWLINE);

                return CMD_WARNING;
            }

            pif->tunnel.car_cir[1] = cir;
            pif->tunnel.car_pir[1] = pir;
        }
        else if (LSP_DIRECTION_EGRESS == dir)
        {
            if (cir < pif->tunnel.car_cir_sum[0])
            {
                vty_out(vty, "ingress CAR CAC check failed !%s", VTY_NEWLINE);

                return CMD_WARNING;
            }

            pif->tunnel.car_cir[0] = cir;
            pif->tunnel.car_pir[0] = pir;
        }

        ret = MPLS_IPC_SENDTO_HAL(&pif->tunnel, sizeof(struct tunnel_t), 1,
                            MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
                            IPC_OPCODE_UPDATE, TUNNEL_INFO_CAR, pif->tunnel.ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}


DEFUN(tunnel_qos_car_l1,
    tunnel_qos_car_l1_cmd,
    "qos car include-interframe (ingress | egress) cir <1-10000000>",
    "Quality of Service\n"
    "Committed Access Rate\n"
    "Include-interframe\n"
    "Ingress\n"
    "Egress\n"
    "Committed information rate\n"
    "Committed information rate value <1-10000000> (Unit: Kbps)\n")
{
    struct tunnel_if *pif  = NULL;
    struct lsp_entry *plsp = NULL;
    enum LSP_DIRECTION dir = LSP_DIRECTION_INGRESS;
    uint32_t ifindex = 0;
    uint32_t cir     = 0;
    int      ret     = 0;

    ifindex = (uint32_t)vty->index;
    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (TUNNEL_PRO_INVALID == pif->tunnel.protocol))
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    /* argv[0] : direction */
    if (argv[0][0] == 'i')
    {
        if (0 != pif->tunnel.car_cir[0])
        {
            vty_error_out(vty, "Ingress car already configuration !%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        dir = LSP_DIRECTION_EGRESS;
    }
    else if (argv[0][0] == 'e')
    {
        if (0 != pif->tunnel.car_cir[1])
        {
            vty_error_out(vty, "Egress car already configuration !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        dir = LSP_DIRECTION_INGRESS;
    }

    /* argv[1] : cir */
    cir = atoi(argv[1]);
    cir |= CAR_VALUE_L1_FLAG;

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)
    {
        if (LSP_DIRECTION_INGRESS == dir)
        {
            if ((cir & (~CAR_VALUE_L1_FLAG)) < pif->tunnel.car_cir_sum[1])
            {
                vty_out(vty, "egress CAR CAC check failed !%s", VTY_NEWLINE);

                return CMD_WARNING;
            }

            pif->tunnel.car_cir[1] = cir;

            if (NULL != pif->p_mplstp->ingress_lsp)
            {
                plsp = mpls_lsp_lookup(pif->p_mplstp->ingress_lsp->lsp_index);
                if (NULL != plsp)
                {
                    plsp->car_cir[1] = cir;

                    mpls_lsp_update(plsp, LSP_SUBTYPE_CAR);
                }
            }

            if (NULL != pif->p_mplstp->backup_ingress_lsp)
            {
                plsp = mpls_lsp_lookup(pif->p_mplstp->backup_ingress_lsp->lsp_index);
                if (NULL != plsp)
                {
                    plsp->car_cir[1] = cir;

                    mpls_lsp_update(plsp, LSP_SUBTYPE_CAR);
                }
            }
        }
        else if (LSP_DIRECTION_EGRESS == dir)
        {
            if ((cir & (~CAR_VALUE_L1_FLAG)) < pif->tunnel.car_cir_sum[0])
            {
                vty_out(vty, "ingress CAR CAC check failed !%s", VTY_NEWLINE);

                return CMD_WARNING;
            }

            pif->tunnel.car_cir[0] = cir;

            if (NULL != pif->p_mplstp->egress_lsp)
            {
                plsp = mpls_lsp_lookup(pif->p_mplstp->egress_lsp->lsp_index);
                if (NULL != plsp)
                {
                    plsp->car_cir[0] = cir;

                    mpls_lsp_update(plsp, LSP_SUBTYPE_CAR);
                }
            }

            if (NULL != pif->p_mplstp->backup_egress_lsp)
            {
                plsp = mpls_lsp_lookup(pif->p_mplstp->backup_egress_lsp->lsp_index);
                if (NULL != plsp)
                {
                    plsp->car_cir[0] = cir;

                    mpls_lsp_update(plsp, LSP_SUBTYPE_CAR);
                }
            }
        }
    }
    else if (TUNNEL_PRO_GRE == pif->tunnel.protocol)
    {
        if (LSP_DIRECTION_INGRESS == dir)
        {
            if ((cir & (~CAR_VALUE_L1_FLAG)) < pif->tunnel.car_cir_sum[1])
            {
                vty_out(vty, "egress CAR CAC check failed !%s", VTY_NEWLINE);

                return CMD_WARNING;
            }

            pif->tunnel.car_cir[1] = cir;
        }
        else if (LSP_DIRECTION_EGRESS == dir)
        {
            if ((cir & (~CAR_VALUE_L1_FLAG)) < pif->tunnel.car_cir_sum[0])
            {
                vty_out(vty, "ingress CAR CAC check failed !%s", VTY_NEWLINE);

                return CMD_WARNING;
            }

            pif->tunnel.car_cir[0] = cir;
        }

        ret = MPLS_IPC_SENDTO_HAL(&pif->tunnel, sizeof(struct tunnel_t), 1,
                            MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
                            IPC_OPCODE_UPDATE, TUNNEL_INFO_CAR, pif->tunnel.ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tunnel 删除 qos car 命令
 * @param[in ] : ingress - 入方向限速
 * @param[in ] : egress  - 出方向限速
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:17:14
 * @note       :
 */
DEFUN(no_tunnel_qos_car,
    no_tunnel_qos_car_cmd,
    "no qos car (ingress | egress)",
    MPLS_CLI_INFO)
{
    struct tunnel_if *pif  = NULL;
    struct lsp_entry *plsp = NULL;
    enum LSP_DIRECTION dir;
    uint32_t ifindex = 0;
    int      ret     = 0;

    ifindex = (uint32_t)vty->index;
    pif = tunnel_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    if ('i' == argv[0][0])
    {
        if (0 == pif->tunnel.car_cir[0])
        {
            return CMD_SUCCESS;
        }

        dir = LSP_DIRECTION_EGRESS;
    }
    else
    {
        if (0 == pif->tunnel.car_cir[1])
        {
            return CMD_SUCCESS;
        }

        dir = LSP_DIRECTION_INGRESS;
    }

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)      // mplstp 协议走公网 lsp 更新流程
    {
        if (LSP_DIRECTION_INGRESS == dir)
        {
            if (NULL != pif->p_mplstp->ingress_lsp)
            {
                plsp = mpls_lsp_lookup(pif->p_mplstp->ingress_lsp->lsp_index);
                if (NULL != plsp)
                {
                    plsp->car_cir[1] = 0;
                    plsp->car_pir[1] = 0;

                    mpls_lsp_update(plsp, LSP_SUBTYPE_CAR);
                }
            }

            if (NULL != pif->p_mplstp->backup_ingress_lsp)
            {
                plsp = mpls_lsp_lookup(pif->p_mplstp->backup_ingress_lsp->lsp_index);
                if (NULL != plsp)
                {
                    plsp->car_cir[1] = 0;
                    plsp->car_pir[1] = 0;

                    mpls_lsp_update(plsp, LSP_SUBTYPE_CAR);
                }
            }

            pif->tunnel.car_cir[1] = 0;
            pif->tunnel.car_pir[1] = 0;
        }
        else if (LSP_DIRECTION_EGRESS == dir)
        {
            if (NULL != pif->p_mplstp->egress_lsp)
            {
                plsp = mpls_lsp_lookup(pif->p_mplstp->egress_lsp->lsp_index);
                if (NULL != plsp)
                {
                    plsp->car_cir[0] = 0;
                    plsp->car_pir[0] = 0;

                    mpls_lsp_update(plsp, LSP_SUBTYPE_CAR);
                }
            }

            if (NULL != pif->p_mplstp->backup_egress_lsp)
            {
                plsp = mpls_lsp_lookup(pif->p_mplstp->backup_egress_lsp->lsp_index);
                if (NULL != plsp)
                {
                    plsp->car_cir[0] = 0;
                    plsp->car_pir[0] = 0;

                    mpls_lsp_update(plsp, LSP_SUBTYPE_CAR);
                }
            }

            pif->tunnel.car_cir[0] = 0;
            pif->tunnel.car_pir[0] = 0;
        }
    }
    else if (TUNNEL_PRO_GRE == pif->tunnel.protocol)    // gre 协议走 tunnel 更新流程
    {
        if (LSP_DIRECTION_INGRESS == dir)
        {
            pif->tunnel.car_cir[1] = 0;
            pif->tunnel.car_pir[1] = 0;
        }
        else if (LSP_DIRECTION_EGRESS == dir)
        {
            pif->tunnel.car_cir[0] = 0;
            pif->tunnel.car_pir[0] = 0;
        }

        ret = MPLS_IPC_SENDTO_HAL(&pif->tunnel, sizeof(struct tunnel_t), 1,
                            MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
                            IPC_OPCODE_UPDATE, TUNNEL_INFO_CAR, pif->tunnel.ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}


DEFUN(tunnel_hqos_queue_apply,
    tunnel_hqos_queue_apply_cmd,
    "hqos cir <0-10000000> {pir <0-10000000> | queue-profile <1-100>}",
    "Hierarchical QoS\n"
    "Committed information rate\n"
    "Committed information rate value (Unit: Kbps)\n"
    "Peak information rate\n"
    "Peak information rate value (Unit: Kbps)\n"
    "Queue profile\n"
    "Queue profile ID\n")
{
    struct tunnel_if *pif    = NULL;
    struct lsp_entry *plsp   = NULL;
    int              *pexist = NULL;
    struct hqos_t hqos;
    uint32_t ifindex = 0;
    uint16_t *phqos_id = NULL;
    int ret = ERRNO_SUCCESS;

    ifindex = (uint32_t)vty->index;
    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (TUNNEL_PRO_INVALID == pif->tunnel.protocol))
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

	memset(&hqos, 0, sizeof(struct hqos_t));

    if (0 != pif->tunnel.hqos_id)
    {
        vty_error_out(vty, "Hqos already configuration !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    hqos.cir = atoi(argv[0]);

    if ((0 == hqos.cir) && (NULL == argv[1]) && (NULL == argv[2]))
    {
        return CMD_SUCCESS;
    }

	if (NULL != argv[1])
	{
        hqos.pir = atoi(argv[1]);
        if (hqos.pir < hqos.cir)
		{
			vty_error_out(vty, "PIR must larger than CIR or equal to CIR.%s", VTY_NEWLINE);

            return CMD_WARNING;
		}
	}

	if (NULL != argv[2])
	{
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

    phqos_id = qos_hqosid_get(&hqos, MODULE_ID_MPLS);
    if ((NULL == phqos_id) || (0 == *phqos_id))
    {
        vty_error_out(vty, "get hqos id failed !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    pif->tunnel.hqos_id = *phqos_id;

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)      /* mplstp 协议走公网 lsp 更新流程 */
    {
        if (NULL != pif->p_mplstp->ingress_lsp)
        {
            plsp = mpls_lsp_lookup(pif->p_mplstp->ingress_lsp->lsp_index);
            if (NULL != plsp)
            {
                plsp->hqos_id = pif->tunnel.hqos_id;

                mpls_lsp_update(plsp, LSP_SUBTYPE_HQOS);
            }
        }

        if (NULL != pif->p_mplstp->backup_ingress_lsp)
        {
            plsp = mpls_lsp_lookup(pif->p_mplstp->backup_ingress_lsp->lsp_index);
            if (NULL != plsp)
            {
                plsp->hqos_id = pif->tunnel.hqos_id;

                mpls_lsp_update(plsp, LSP_SUBTYPE_HQOS);
            }
        }
    }
    else if (TUNNEL_PRO_GRE == pif->tunnel.protocol)      /* gre 协议走 tunnel 更新流程 */
    {
        ret = MPLS_IPC_SENDTO_HAL(&pif->tunnel, sizeof(struct tunnel_t), 1, MODULE_ID_MPLS,
                            IPC_TYPE_TUNNEL, IPC_OPCODE_UPDATE, TUNNEL_INFO_HQOS, pif->tunnel.ifindex);
        if (ERRNO_SUCCESS != ret)
        {
             MPLS_LOG_ERROR("IPC send to hal\n");

             return ERRNO_IPC;
        }
    }

	return CMD_SUCCESS;
}


DEFUN(no_tunnel_hqos_queue_apply,
    no_tunnel_hqos_queue_apply_cmd,
    "no hqos",
    NO_STR
    "Hierarchical QoS\n")
{
    struct tunnel_if *pif    = NULL;
    struct lsp_entry *plsp   = NULL;
    uint32_t ifindex = 0;
    int ret = ERRNO_SUCCESS;

    ifindex = (uint32_t)vty->index;
    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (TUNNEL_PRO_INVALID == pif->tunnel.protocol))
    {
        vty_warning_out(vty, "Please configure the interface"
                            " encapsulation protocol.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    TUNNEL_PROTOCOL_IS_MPLSTE(pif->tunnel.protocol);

    if (0 == pif->tunnel.hqos_id)
    {
        return CMD_SUCCESS;
    }

    qos_hqos_delete(pif->tunnel.hqos_id, MODULE_ID_MPLS);

    pif->tunnel.hqos_id = 0;

    if (TUNNEL_PRO_MPLSTP == pif->tunnel.protocol)      // mplstp 协议走公网 lsp 更新流程
    {
        if (NULL != pif->p_mplstp->ingress_lsp)
        {
            plsp = mpls_lsp_lookup(pif->p_mplstp->ingress_lsp->lsp_index);
            if (NULL != plsp)
            {
                plsp->hqos_id = 0;

                mpls_lsp_update(plsp, LSP_SUBTYPE_HQOS);
            }
        }

        if (NULL != pif->p_mplstp->backup_ingress_lsp)
        {
            plsp = mpls_lsp_lookup(pif->p_mplstp->backup_ingress_lsp->lsp_index);
            if (NULL != plsp)
            {
                plsp->hqos_id = 0;

                mpls_lsp_update(plsp, LSP_SUBTYPE_HQOS);
            }
        }
    }
    else if (TUNNEL_PRO_GRE == pif->tunnel.protocol)      // gre 协议走 tunnel 更新流程
    {
        ret = MPLS_IPC_SENDTO_HAL(&pif->tunnel, sizeof(struct tunnel_t), 1, MODULE_ID_MPLS,
                            IPC_TYPE_TUNNEL, IPC_OPCODE_UPDATE, TUNNEL_INFO_HQOS, pif->tunnel.ifindex);
        if (ERRNO_SUCCESS != ret)
        {
             MPLS_LOG_ERROR("IPC send to hal\n");

             return ERRNO_IPC;
        }
    }

	return CMD_SUCCESS;
}


/**
 * @brief      : tunnel 配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:18:37
 * @note       :
 */
static int tunnel_if_config_write(struct vty *vty)
{
    struct hash_bucket *pbucket   = NULL;
    struct tunnel_if   *pif       = NULL;
    struct hqos_t      *phqos     = NULL;
    char ifname[IFNET_NAMESIZE]   = "";
    char outif[IFNET_NAMESIZE]    = "";
    char srcip[INET_ADDRSTRLEN]   = "";
    char destip[INET_ADDRSTRLEN]  = "";
    char nexthop[INET_ADDRSTRLEN] = "";
    int  cursor;

    HASH_BUCKET_LOOP(pbucket, cursor, tunnel_table)
    {
        pif = (struct tunnel_if *)pbucket->data;
        if (NULL == pif)
        {
            continue;
        }

        ifm_get_name_by_ifindex(pif->tunnel.ifindex, ifname);

        vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);

        if (pif->tunnel.protocol == TUNNEL_PRO_INVALID)
        {
            continue;
        }

        /* 暂时不支持 mplste */
        if (pif->tunnel.protocol == TUNNEL_PRO_MPLSTE)
        {
            continue;
        }

        vty_out(vty, " protocol %s%s", pif->tunnel.protocol == TUNNEL_PRO_MPLSTP ?
                        "mpls-tp" : pif->tunnel.protocol == TUNNEL_PRO_MPLSTE ?
                        "mpls-te" : "gre", VTY_NEWLINE);

        if ((pif->tunnel.sip.addr.ipv4 != 0) && (pif->tunnel.dip.addr.ipv4 != 0))
        {
            inet_ipv4tostr(pif->tunnel.sip.addr.ipv4, srcip);
            inet_ipv4tostr(pif->tunnel.dip.addr.ipv4, destip);

            vty_out(vty, " source %s destination %s%s", srcip, destip, VTY_NEWLINE);
        }

        if (pif->tunnel.protocol == TUNNEL_PRO_GRE)
        {
            uint8_t link_state = 0;
            if ((0 == pif->tunnel.outif) || (ifm_get_link(pif->tunnel.outif, MODULE_ID_MPLS, &link_state)) || (IFNET_LINKDOWN == link_state))
            {

            }
            else
            {
                ifm_get_name_by_ifindex(pif->tunnel.outif, outif);

                vty_out(vty, " outif %s dmac %02x:%02x:%02x:%02x:%02x:%02x%s", outif,
                        pif->tunnel.dmac[0], pif->tunnel.dmac[1], pif->tunnel.dmac[2],
                        pif->tunnel.dmac[3], pif->tunnel.dmac[4], pif->tunnel.dmac[5], VTY_NEWLINE);
            }

            if (0 != pif->tunnel.nexthop)
            {
                inet_ipv4tostr(pif->tunnel.nexthop, nexthop);

                vty_out(vty, " nexthop %s%s", nexthop, VTY_NEWLINE);
            }
        }
        else if (pif->tunnel.protocol == TUNNEL_PRO_MPLSTP)
        {
            if (pif->p_mplstp->ingress_lsp != NULL)
            {
                if (pif->p_mplstp->egress_lsp != NULL)
                {
                    vty_out(vty, " mpls static-lsp ingress %s egress %s%s",
                            pif->p_mplstp->ingress_lsp->name,
                            pif->p_mplstp->egress_lsp->name, VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, " mpls static-lsp ingress %s%s",
                            pif->p_mplstp->ingress_lsp->name, VTY_NEWLINE);
                }

                if (0 != pif->p_mplstp->ingress_lsp->mplsoam_id)
                {
                    vty_out(vty, " mplstp-oam enable session %u master-lsp%s",
                            pif->p_mplstp->ingress_lsp->mplsoam_id, VTY_NEWLINE);
                }

                if (0 != pif->p_mplstp->ingress_lsp->bfd_id)
                {
                    vty_out(vty, " bfd enable session %u master-lsp%s",
                            pif->p_mplstp->ingress_lsp->bfd_id, VTY_NEWLINE);
                }
            }

            if (pif->p_mplstp->backup_ingress_lsp != NULL)
            {
                if (pif->p_mplstp->backup_egress_lsp != NULL)
                {
                    vty_out(vty, " mpls static-lsp ingress %s egress %s backup%s",
                            pif->p_mplstp->backup_ingress_lsp->name,
                            pif->p_mplstp->backup_egress_lsp->name, VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, " mpls static-lsp ingress %s backup%s",
                            pif->p_mplstp->backup_ingress_lsp->name, VTY_NEWLINE);
                }

                if (0 != pif->p_mplstp->backup_ingress_lsp->mplsoam_id)
                {
                    vty_out(vty, " mplstp-oam enable session %u slave-lsp%s",
                            pif->p_mplstp->backup_ingress_lsp->mplsoam_id, VTY_NEWLINE);
                }

                if (0 != pif->p_mplstp->backup_ingress_lsp->bfd_id)
                {
                    vty_out(vty, " bfd enable session %u slave-lsp%s",
                            pif->p_mplstp->backup_ingress_lsp->bfd_id, VTY_NEWLINE);
                }
            }

            if (0 != pif->tunnel.mplsoam_id)
            {
                vty_out(vty, " mplstp-oam enable session %u%s", pif->tunnel.mplsoam_id, VTY_NEWLINE);
            }

            if (0 != pif->tunnel.bfd_id)
            {
                vty_out(vty, " bfd enable session %u%s", pif->tunnel.bfd_id, VTY_NEWLINE);
            }

            if ((pif->tunnel.failback == FAILBACK_ENABLE) && (pif->tunnel.wtr != 30))
            {
                vty_out(vty, " backup failback wtr %u%s", pif->tunnel.wtr, VTY_NEWLINE);
            }
            else if (pif->tunnel.failback == FAILBACK_DISABLE)
            {
                vty_out(vty, " backup non-failback%s", VTY_NEWLINE);
            }
        }

        if (0 != pif->tunnel.domain_id)
        {
            vty_out(vty, " qos mapping enable exp-domain %d%s", pif->tunnel.domain_id, VTY_NEWLINE);
        }

        if (ENABLE == pif->tunnel.phb_enable)
        {
            vty_out(vty, " qos mapping enable exp-phb %d%s", pif->tunnel.phb_id, VTY_NEWLINE);
        }

        if (0 != pif->tunnel.car_cir[0])
        {
            if ((pif->tunnel.car_cir[0] & CAR_VALUE_L1_FLAG) == CAR_VALUE_L1_FLAG)
            {
                vty_out(vty, "  qos car include-interframe ingress cir %u%s",
                            pif->tunnel.car_cir[0]&(~CAR_VALUE_L1_FLAG), VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "  qos car ingress cir %u pir %u%s", pif->tunnel.car_cir[0], pif->tunnel.car_pir[0], VTY_NEWLINE);
            }
        }

        if (0 != pif->tunnel.car_cir[1])
        {
            if ((pif->tunnel.car_cir[1] & CAR_VALUE_L1_FLAG) == CAR_VALUE_L1_FLAG)
            {
                vty_out(vty, "  qos car include-interframe egress cir %u%s",
                             pif->tunnel.car_cir[1]&(~CAR_VALUE_L1_FLAG), VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "  qos car egress cir %u pir %u%s", pif->tunnel.car_cir[1], pif->tunnel.car_pir[1], VTY_NEWLINE);
            }
        }

        if (pif->tunnel.hqos_id != 0)
        {
            phqos = qos_hqos_get(pif->tunnel.hqos_id, MODULE_ID_MPLS);
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

        if (ENABLE == pif->tunnel.statis_enable)
        {
            vty_out(vty, " statistics enable%s", VTY_NEWLINE);
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tunnel 命令行初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:19:59
 * @note       :
 */
void tunnel_if_cmd_init(void)
{
    install_node (&tunnel_if_node, tunnel_if_config_write);

    install_default (TUNNEL_IF_NODE);

    install_element (CONFIG_NODE, &tunnel_if_common_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &tunnel_mpls_show_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &no_tunnel_interface_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &tunnel_statistics_clear_interface_cmd, CMD_SYNC);

    install_element (TUNNEL_IF_NODE, &tunnel_protocol_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_protocol_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &tunnel_source_destination_ip_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_source_destination_ip_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &tunnel_nexthop_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_nexthop_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &tunnel_outif_dmac_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_outif_dmac_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &tunnel_static_lsp_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_static_lsp_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &tunnel_failback_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &tunnel_mpls_show_cmd, CMD_LOCAL);

    install_element (TUNNEL_IF_NODE, &tunnel_qos_mapping_exp_domain_enable_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_qos_mapping_exp_domain_enable_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &tunnel_qos_mapping_exp_phb_enable_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_qos_mapping_exp_phb_enable_cmd, CMD_SYNC);

    install_element (TUNNEL_IF_NODE, &tunnel_statistics_enable_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_statistics_enable_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &tunnel_statistics_clear_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &show_tunnel_statistics_cmd, CMD_LOCAL);

    install_element (TUNNEL_IF_NODE, &tunnel_mplstp_oam_session_enable_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_mplstp_oam_session_enable_cmd, CMD_SYNC);

    install_element (TUNNEL_IF_NODE, &tunnel_bfd_session_enable_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_bfd_session_enable_cmd, CMD_SYNC);

    /* car */
    install_element (TUNNEL_IF_NODE, &tunnel_qos_car_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &tunnel_qos_car_l1_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_qos_car_cmd, CMD_SYNC);

    /* hqos */
    install_element (TUNNEL_IF_NODE, &tunnel_hqos_queue_apply_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_tunnel_hqos_queue_apply_cmd, CMD_SYNC);

    return;
}

