/**
 * @file      : lsp_cmd.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月2日 10:54:12
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/ether.h>
#include <lib/prefix.h>
#include <lib/errcode.h>
#include "mpls_main.h"
#include "lsp_cmd.h"
#include "mpls.h"
#include "labelm.h"
#include "lsp_static.h"
#include "lspm.h"
#include "pw.h"


/**
 * @brief      : 静态 lsp 详细显示
 * @param[in ] : vty  - vty 全局结构
 * @param[in ] : plsp - 静态 lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:55:23
 * @note       :
 */
static void mpls_static_lsp_show_verbose(struct vty *vty, struct static_lsp *plsp)
{
    char fec_buf[INET_ADDRSTRLEN+5]  = "";
    char destip[INET_ADDRSTRLEN]     = "";
    char nexthop[INET_ADDRSTRLEN]    = "";
    char ifname[IFNET_NAMESIZE]      = "";
    char tunnel_name[IFNET_NAMESIZE] = "";

    inet_ipv4tostr(plsp->destip.addr.ipv4, destip);
    sprintf(fec_buf, "%s/%d", destip, plsp->destip.prefixlen);

    inet_ipv4tostr(plsp->nexthop.addr.ipv4, nexthop);
    ifm_get_name_by_ifindex(plsp->nhp_index, ifname);
    ifm_get_name_by_ifindex(plsp->group_index, tunnel_name);

    vty_out(vty, "%-20s:    %s%s", "name", plsp->name, VTY_NEWLINE);
    vty_out(vty, "%-20s:    %s%s", "direction", plsp->direction ==
                LSP_DIRECTION_INGRESS ? "ingress":plsp->direction ==
                LSP_DIRECTION_TRANSIT ? "transit" : "egress", VTY_NEWLINE);
    vty_out(vty, "%-20s:    %s%s", "fec", fec_buf, VTY_NEWLINE);

    if (plsp->inlabel == 0)
    {
        vty_out(vty, "%-20s:    %s%s", "in-label", "-", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-20s:    %u%s", "in-label", plsp->inlabel, VTY_NEWLINE);
    }

    if (plsp->outlabel == 0)
    {
        vty_out(vty, "%-20s:    %s%s", "out-label", "-", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-20s:    %u%s", "out-label", plsp->outlabel, VTY_NEWLINE);
    }

    vty_out(vty, "%-20s:    %s%s", "next-hop", plsp->nexthop.addr.ipv4 ==
                    0 ? "-" : nexthop, VTY_NEWLINE);
    vty_out(vty, "%-20s:    %s%s", "out-interface", plsp->nhp_index ==
                    0 ? "-" : ifname, VTY_NEWLINE);
    vty_out(vty, "%-20s:    %s%s", "tunnel-interface", plsp->group_index ==
                    0 ? "-" : tunnel_name, VTY_NEWLINE);

    if ((plsp->dmac[0]==0) && (plsp->dmac[1]==0) && (plsp->dmac[2]==0)
        && (plsp->dmac[3]==0) && (plsp->dmac[4]==0) && (plsp->dmac[5]==0))
    {
        vty_out(vty, "%-20s:    %s%s", "dmac", "-", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-20s:    %02x:%02x:%02x:%02x:%02x:%02x%s", "dmac",
                        plsp->dmac[0], plsp->dmac[1], plsp->dmac[2],
                        plsp->dmac[3], plsp->dmac[4], plsp->dmac[5], VTY_NEWLINE);
    }

    vty_out(vty, "%-20s:    %s%s", "admin status", plsp->admin_down ==
                        LINK_UP ? "up" : "down", VTY_NEWLINE);
    vty_out(vty, "%-20s:    %s%s", "link status", plsp->status ==
                        ENABLE ? "up" : "down", VTY_NEWLINE);

    return;
}


/**
 * @brief      : 静态 lsp 简要显示
 * @param[in ] : vty  - vty 全局结构
 * @param[in ] : plsp - 静态 lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:57:28
 * @note       :
 */
static void mpls_static_lsp_show_brief(struct vty *vty, struct static_lsp *plsp)
{
    char fec_buf[INET_ADDRSTRLEN+5] = "";
    char label_buf[NAME_STRING_LEN] = "";
    char destip[INET_ADDRSTRLEN]    = "";
    char nexthop[INET_ADDRSTRLEN]   = "";
    char ifname[IFNET_NAMESIZE]     = "";

    inet_ipv4tostr(plsp->destip.addr.ipv4, destip);

    if (0 != plsp->nexthop.addr.ipv4)
    {
        inet_ipv4tostr(plsp->nexthop.addr.ipv4, nexthop);
    }

    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "%-32s", plsp->name);

    sprintf(fec_buf, "%s/%d", destip, plsp->destip.prefixlen);

    vty_out(vty, "%-19s", fec_buf);

    if (plsp->direction == LSP_DIRECTION_INGRESS)
    {
        sprintf(label_buf, "%s/%u", "-", plsp->outlabel);

        vty_out(vty, "%-16s", label_buf);
    }
    else if (plsp->direction == LSP_DIRECTION_TRANSIT)
    {
        sprintf(label_buf, "%u/%u", plsp->inlabel, plsp->outlabel);

        vty_out(vty, "%-16s", label_buf);
    }
    else if (plsp->direction == LSP_DIRECTION_EGRESS)
    {
        sprintf(label_buf, "%u/%s", plsp->inlabel, "-");

        vty_out(vty, "%-16s", label_buf);
    }

    if (plsp->nhp_index != 0)
    {
        ifm_get_name_by_ifindex(plsp->nhp_index, ifname);

        vty_out(vty, "%-31s", ifname);
    }
    else
    {
        vty_out(vty, "%-31s", nexthop);
    }

    vty_out(vty, "%-7s", plsp->down_flag == LINK_UP ? "up" : "down");

    return;
}


/**
 * @brief      : 静态 lsp ingress 指定下一跳方式配置命令
 * @param[in ] : NAME                   - 静态 lsp 名称
 * @param[in ] : destination A.B.C.D/M  - 静态 lsp 目的
 * @param[in ] : nexthop A.B.C.D        - 静态 lsp 下一跳
 * @param[in ] : out-label <16-1048575> - 静态 lsp 出标签
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:59:16
 * @note       :
 */
DEFUN (ingress_static_lsp_nexthop,
    ingress_static_lsp_nexthop_cmd,
    "mpls static-lsp NAME ingress destination A.B.C.D/M nexthop A.B.C.D "
    "out-label <16-1048575>",
    MPLS_CLI_INFO)
{
    struct static_lsp  lsp;
    struct prefix_ipv4 destip;
    struct prefix_ipv4 nexthop;
    int                ret;

    memset(&lsp, 0, sizeof(struct static_lsp));
    lsp.direction  = LSP_DIRECTION_INGRESS;
    lsp.lsp_type   = LSP_TYPE_STATIC;
    lsp.nhp_type   = NHP_TYPE_CONNECT;
    lsp.admin_down = LINK_DOWN;
    lsp.down_flag  = LINK_DOWN;
    lsp.status     = ENABLE;

    MPLS_LSRID_CHECK(vty);

    /* argv[0]: lsp name */
    memcpy(lsp.name, argv[0], strlen(argv[0])+1);

    /* argv[1]: destination */
    ret = str2prefix_ipv4(argv[1], &destip);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lsp.destip.addr.ipv4 = ipv4_network_addr(destip.prefix.s_addr, destip.prefixlen);
    lsp.destip.addr.ipv4 = ntohl(lsp.destip.addr.ipv4);
    lsp.destip.prefixlen = destip.prefixlen;

    if ((0 == lsp.destip.addr.ipv4) || (FALSE == inet_valid_network(lsp.destip.addr.ipv4)))
    {
        vty_error_out(vty, "The specified destination is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[2]: nexthop */
    ret = str2prefix_ipv4(argv[2], &nexthop);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified nexthop address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lsp.nexthop.addr.ipv4 = nexthop.prefix.s_addr;
    lsp.nexthop.addr.ipv4 = ntohl(lsp.nexthop.addr.ipv4);

    ret = inet_valid_network(lsp.nexthop.addr.ipv4);
    if ((!ret) || (lsp.nexthop.addr.ipv4 == 0))
    {
        vty_error_out(vty, "The specified nexthop address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[3]: outlabel */
    lsp.outlabel = atoi(argv[3]);

    ret = static_lsp_add(&lsp);
    if (ret != ERRNO_SUCCESS)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 静态 pw ingress 指定出接口方式配置命令
 * @param[in ] : NAME                   - 静态 lsp 名称
 * @param[in ] : destination A.B.C.D/M  - 静态 lsp 目的
 * @param[in ] : ethernet USP           - 出接口类型为 ethernet
 * @param[in ] : gigabitethernet USP    - 出接口类型为 gigabitethernet
 * @param[in ] : xgigabitethernet USP   - 出接口类型为 xgigabitethernet
 * @param[in ] : trunk TRUNK            - 出接口类型为 trunk
 * @param[in ] : dmac DMAC              - 目的 mac 地址
 * @param[in ] : out-label <16-1048575> - 静态 lsp 出标签
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 11:04:40
 * @note       :
 */
DEFUN (ingress_static_lsp_outif,
    ingress_static_lsp_outif_cmd,
    "mpls static-lsp NAME ingress destination A.B.C.D/M outif {ethernet USP | gigabitethernet USP "
    "| xgigabitethernet USP | trunk TRUNK} dmac DMAC out-label <16-1048575>",
    MPLS_CLI_INFO)
{
    struct static_lsp  lsp;
    struct prefix_ipv4 destip;
    uint8_t pdown_flag = 0;
    uint8_t pmode      = 0;
    int      ret;

    memset(&lsp, 0, sizeof(struct static_lsp));
    lsp.direction  = LSP_DIRECTION_INGRESS;
    lsp.lsp_type   = LSP_TYPE_STATIC;
    lsp.nhp_type   = NHP_TYPE_CONNECT;
    lsp.admin_down = LINK_DOWN;
    lsp.down_flag  = LINK_DOWN;
    lsp.status     = ENABLE;

    MPLS_LSRID_CHECK(vty);

    /* argv[0]: lsp name */
    memcpy(lsp.name, argv[0], strlen(argv[0])+1);

    /* argv[1]: destip */
    ret = str2prefix_ipv4(argv[1], &destip);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lsp.destip.addr.ipv4 = ipv4_network_addr(destip.prefix.s_addr, destip.prefixlen);
    lsp.destip.addr.ipv4 = ntohl(lsp.destip.addr.ipv4);
    lsp.destip.prefixlen = destip.prefixlen;

    if ((0 == lsp.destip.addr.ipv4) || (FALSE == inet_valid_network(lsp.destip.addr.ipv4)))
    {
        vty_error_out(vty, "The specified destination is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[2]、[3]: interface name */
    if (NULL != argv[2])
    {
        lsp.nhp_index = ifm_get_ifindex_by_name("ethernet", argv[2]);
        if (0 == lsp.nhp_index)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[2], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
	else if (NULL != argv[3])
    {
        lsp.nhp_index = ifm_get_ifindex_by_name("gigabitethernet", argv[3]);
        if (0 == lsp.nhp_index)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[3], VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (lsp.nhp_index == ifm_get_ifindex_by_name("gigabitethernet", "1/0/1"))
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", "1/0/1", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
	else if (NULL != argv[4])
    {
        lsp.nhp_index = ifm_get_ifindex_by_name("xgigabitethernet", argv[4]);
        if (0 == lsp.nhp_index)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[4], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else if (NULL != argv[5])
    {
        lsp.nhp_index = ifm_get_ifindex_by_name("trunk", argv[5]);
        if (0 == lsp.nhp_index)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[5], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    if (1 != vty->config_read_flag) // 非配置恢复过程，需要保证接口存在
    {
        if (ifm_get_link(lsp.nhp_index, MODULE_ID_MPLS, &pdown_flag))
        {
            vty_error_out(vty, "Interface does not exist.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
        if (pdown_flag == IFNET_LINKUP)
        {
            lsp.admin_down = LINK_UP;
            lsp.down_flag  = LINK_UP;
        }

        if (ifm_get_mode(lsp.nhp_index, MODULE_ID_MPLS, &pmode))
        {
            vty_error_out(vty, "Static lsp get out interface mode failed.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
        if (pmode != IFNET_MODE_L3)
        {
            lsp.admin_down = LINK_DOWN;
            lsp.down_flag  = LINK_DOWN;
        }

    }
    else                            // 配置恢复过程获取出接口信息
    {
        if(!ifm_get_link(lsp.nhp_index, MODULE_ID_MPLS, &pdown_flag))
        {
            if (pdown_flag == IFNET_LINKUP)
            {
                if(!ifm_get_mode(lsp.nhp_index, MODULE_ID_MPLS,&pmode))
                {
                    if (pmode == IFNET_MODE_L3)
                    {
                        lsp.admin_down = LINK_UP;
                        lsp.down_flag  = LINK_UP;
                    }
                }
            }
        }
    }

    /* argv[4]: dmac */
    if (ether_valid_mac((char *)argv[6]))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ether_string_to_mac((char *)argv[6], lsp.dmac);

    /* mac 地址全0、组播、广播 mac 无效 */
    if ((0 == ether_is_broadcast_mac(lsp.dmac))
        || (0 == ether_is_muticast_mac(lsp.dmac))
        || (0 == ether_is_zero_mac(lsp.dmac)))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[5]: label */
    lsp.outlabel = atoi(argv[7]);

	/*用于tpoam告警支持*/
	lsp.oam_nhp_index = lsp.nhp_index;

    ret = static_lsp_add(&lsp);
    if (ret != ERRNO_SUCCESS)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 静态 lsp transit 指定下一跳方式配置命令
 * @param[in ] : NAME                   - 静态 lsp 名称
 * @param[in ] : destination A.B.C.D/M  - 静态 lsp 目的
 * @param[in ] : nexthop A.B.C.D        - 静态 lsp 下一跳
 * @param[in ] : in-label <16-1048575>  - 静态 lsp 入标签
 * @param[in ] : out-label <16-1048575> - 静态 lsp 出标签
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 11:13:38
 * @note       :
 */
DEFUN(transit_static_lsp_nexthop,
    transit_static_lsp_nexthop_cmd,
    "mpls static-lsp NAME transit destination A.B.C.D/M nexthop A.B.C.D "
    "in-label <16-1048575> out-label <16-1048575>",
    MPLS_CLI_INFO)
{
    struct static_lsp  lsp;
    struct prefix_ipv4 destip;
    struct prefix_ipv4 nexthop;
    int                ret;

    memset(&lsp, 0, sizeof(struct static_lsp));
    lsp.direction  = LSP_DIRECTION_TRANSIT;
    lsp.lsp_type   = LSP_TYPE_STATIC;
    lsp.nhp_type   = NHP_TYPE_CONNECT;
    lsp.admin_down = LINK_DOWN;
    lsp.down_flag  = LINK_DOWN;
    lsp.status     = ENABLE;

    MPLS_LSRID_CHECK(vty);

    /* argv[0]: lsp name */
    memcpy(lsp.name, argv[0], strlen(argv[0])+1);

    /* argv[1]: destip */
    ret = str2prefix_ipv4(argv[1], &destip);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lsp.destip.addr.ipv4 = ipv4_network_addr(destip.prefix.s_addr, destip.prefixlen);
    lsp.destip.addr.ipv4 = ntohl(lsp.destip.addr.ipv4);
    lsp.destip.prefixlen = destip.prefixlen;

    if ((0 == lsp.destip.addr.ipv4) || (FALSE == inet_valid_network(lsp.destip.addr.ipv4)))
    {
        vty_error_out(vty, "The specified destination is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[2]: nexthop */
    ret = str2prefix_ipv4(argv[2], &nexthop);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified nexthop address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lsp.nexthop.addr.ipv4 = nexthop.prefix.s_addr;
    lsp.nexthop.addr.ipv4 = ntohl(lsp.nexthop.addr.ipv4);

    ret = inet_valid_network(lsp.nexthop.addr.ipv4);
    if ((!ret) || (lsp.nexthop.addr.ipv4 == 0))
    {
        vty_error_out(vty, "The specified nexthop address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[3] [4]: I/O label */
    lsp.inlabel = atoi(argv[3]);
    ret = label_get(lsp.inlabel, MODULE_ID_SLSP);
    if (0 == ret)
    {
        vty_error_out(vty, "The in label being used.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lsp.outlabel = atoi(argv[4]);

    ret = static_lsp_add(&lsp);
    if (ret != ERRNO_SUCCESS)
    {
        label_free(lsp.inlabel);

        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 静态 lsp transit 指定出接口方式配置命令
 * @param[in ] : NAME                   - 静态 lsp 名称
 * @param[in ] : destination A.B.C.D/M  - 静态 lsp 目的
 * @param[in ] : ethernet USP           - 出接口类型为 ethernet
 * @param[in ] : gigabitethernet USP    - 出接口类型为 gigabitethernet
 * @param[in ] : xgigabitethernet USP   - 出接口类型为 xgigabitethernet
 * @param[in ] : trunk TRUNK            - 出接口类型为 trunk
 * @param[in ] : dmac DMAC              - 目的 mac 地址
 * @param[in ] : in-label <16-1048575>  - 静态 lsp 入标签
 * @param[in ] : out-label <16-1048575> - 静态 lsp 出标签
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 11:18:45
 * @note       :
 */
DEFUN(transit_static_lsp_outif,
    transit_static_lsp_outif_cmd,
    "mpls static-lsp NAME transit destination A.B.C.D/M outif {ethernet USP | "
    "gigabitethernet USP | xgigabitethernet USP | trunk TRUNK} dmac DMAC in-label "
    "<16-1048575> out-label <16-1048575>",
    MPLS_CLI_INFO)
{
    struct static_lsp  lsp;
    struct prefix_ipv4 destip;
    uint8_t pdown_flag = 0;
    uint8_t pmode      = 0;
    int      ret;

    memset(&lsp, 0, sizeof(struct static_lsp));
    lsp.direction  = LSP_DIRECTION_TRANSIT;
    lsp.lsp_type   = LSP_TYPE_STATIC;
    lsp.nhp_type   = NHP_TYPE_CONNECT;
    lsp.admin_down = LINK_DOWN;
    lsp.down_flag  = LINK_DOWN;
    lsp.status     = ENABLE;

    MPLS_LSRID_CHECK(vty);

    /* argv[0]: lsp name */
    memcpy(lsp.name, argv[0], strlen(argv[0])+1);

    /* argv[1]: destip */
    ret = str2prefix_ipv4(argv[1], &destip);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lsp.destip.addr.ipv4 = ipv4_network_addr(destip.prefix.s_addr, destip.prefixlen);
    lsp.destip.addr.ipv4 = ntohl(lsp.destip.addr.ipv4);
    lsp.destip.prefixlen = destip.prefixlen;

    if ((0 == lsp.destip.addr.ipv4) || (FALSE == inet_valid_network(lsp.destip.addr.ipv4)))
    {
        vty_error_out(vty, "The specified destination is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[2]、[3]: interface name */
    if (NULL != argv[2])
    {
        lsp.nhp_index = ifm_get_ifindex_by_name("ethernet", argv[2]);
        if (0 == lsp.nhp_index)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[2], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
	else if (NULL != argv[3])
    {
        lsp.nhp_index = ifm_get_ifindex_by_name("gigabitethernet", argv[3]);
        if (0 == lsp.nhp_index)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[3], VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (lsp.nhp_index == ifm_get_ifindex_by_name("gigabitethernet", "1/0/1"))
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", "1/0/1", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
	else if (NULL != argv[4])
    {
        lsp.nhp_index = ifm_get_ifindex_by_name("xgigabitethernet", argv[4]);
        if (0 == lsp.nhp_index)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[4], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else if (NULL != argv[5])
    {
        lsp.nhp_index = ifm_get_ifindex_by_name("trunk", argv[5]);
        if (0 == lsp.nhp_index)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[5], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    if (1 != vty->config_read_flag)
    {
        /* 非配置恢复过程，需要保证接口存在 */
        
        if (ifm_get_link(lsp.nhp_index, MODULE_ID_MPLS, &pdown_flag))
        {
            vty_error_out(vty, "Interface does not exist.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
        
        if (pdown_flag == IFNET_LINKUP)
        {
            lsp.admin_down = LINK_UP;
            lsp.down_flag  = LINK_UP;
        }

        if (ifm_get_mode(lsp.nhp_index, MODULE_ID_MPLS, &pmode))
        {
            vty_error_out(vty, "Interface mode is not L3.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
        if (pmode != IFNET_MODE_L3)
        {
            lsp.admin_down = LINK_DOWN;
            lsp.down_flag  = LINK_DOWN;
        }

    }
    else
    {
        ifm_get_link(lsp.nhp_index, MODULE_ID_MPLS, &pdown_flag);
        if (pdown_flag == IFNET_LINKUP)
        {
            ifm_get_mode(lsp.nhp_index, MODULE_ID_MPLS, &pmode);
            if (pmode == IFNET_MODE_L3)
            {
                lsp.admin_down = LINK_UP;
                lsp.down_flag  = LINK_UP;
            }
        }
    }

    /* argv[4]: dmac */
    if (ether_valid_mac((char *)argv[6]))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ether_string_to_mac((char *)argv[6], lsp.dmac);

    /* mac 地址全0、组播、广播 mac 无效 */
    if ((0 == ether_is_broadcast_mac(lsp.dmac))
        || (0 == ether_is_muticast_mac(lsp.dmac))
        || (0 == ether_is_zero_mac(lsp.dmac)))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[5] [6]: I/O label */
    lsp.inlabel = atoi(argv[7]);

    ret = label_get(lsp.inlabel, MODULE_ID_SLSP);
    if (0 == ret)
    {
        vty_error_out(vty, "The in label being used.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lsp.outlabel = atoi(argv[8]);

	/*用于tpoam告警支持*/
	lsp.oam_nhp_index = lsp.nhp_index;

    ret = static_lsp_add(&lsp);
    if (ret != ERRNO_SUCCESS)
    {
        label_free(lsp.inlabel);

        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 静态 lsp egress 配置命令
 * @param[in ] : NAME                  - 静态 lsp 名称
 * @param[in ] : destination A.B.C.D/M - 静态 lsp 目的
 * @param[in ] : in-label <16-1048575> - 静态 lsp 入标签
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 11:23:47
 * @note       :
 */
DEFUN(egress_static_lsp,
    egress_static_lsp_cmd,
    "mpls static-lsp NAME egress destination A.B.C.D/M in-label <16-1048575>",
    MPLS_CLI_INFO)
{
    struct static_lsp  lsp;
    struct prefix_ipv4 destip;
    int                ret;

    memset(&lsp, 0, sizeof(struct static_lsp));
    lsp.direction  = LSP_DIRECTION_EGRESS;
    lsp.lsp_type   = LSP_TYPE_STATIC;
    lsp.nhp_type   = NHP_TYPE_CONNECT;
    lsp.admin_down = LINK_UP;
    lsp.down_flag  = LINK_UP;
    lsp.status     = ENABLE;

    MPLS_LSRID_CHECK(vty);

    /* argv[0]: lsp name */
    memcpy(lsp.name, argv[0], strlen(argv[0])+1);

    /* argv[1]: destip */
    ret = str2prefix_ipv4(argv[1], &destip);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lsp.destip.addr.ipv4 = ipv4_network_addr(destip.prefix.s_addr, destip.prefixlen);
    lsp.destip.addr.ipv4 = ntohl(lsp.destip.addr.ipv4);
    lsp.destip.prefixlen = destip.prefixlen;

    if ((0 == lsp.destip.addr.ipv4) || (FALSE == inet_valid_network(lsp.destip.addr.ipv4)))
    {
        vty_error_out(vty, "The specified destination is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[2] : label */
    lsp.inlabel = atoi(argv[2]);

    ret = label_get(lsp.inlabel, MODULE_ID_SLSP);
    if (0 == ret)
    {
        vty_error_out(vty, "The in label being used.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = static_lsp_add(&lsp);
    if (ret != ERRNO_SUCCESS)
    {
        label_free(lsp.inlabel);

        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 静态 lsp 删除命令
 * @param[in ] : NAME - 静态 lsp 名称
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 11:25:46
 * @note       :
 */
DEFUN(no_static_lsp,
    no_static_lsp_cmd,
    "no mpls static-lsp NAME",
    MPLS_CLI_INFO)
{
    int ret;

    ret = static_lsp_delete((uchar *)argv[0]);
    if (ret != ERRNO_SUCCESS)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 静态 lsp 显示命令
 * @param[in ] : NAME - 静态 lsp 名称
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 11:26:38
 * @note       :
 */
DEFUN(show_static_lsp,
    show_static_lsp_cmd,
    "show mpls static-lsp [NAME]",
    MPLS_CLI_INFO)
{
    struct static_lsp  *plsp    = NULL;
    struct hash_bucket *pbucket = NULL;
    int num = 0;
    int cursor;

    if (argv[0] != NULL)
    {
        plsp = static_lsp_lookup((uchar *)argv[0]);
        if (NULL == plsp)
        {
            return CMD_WARNING;
        }

        mpls_static_lsp_show_verbose(vty, plsp);

        vty_out(vty, "%s", VTY_NEWLINE);
    }
    else
    {
        HASH_BUCKET_LOOP(pbucket, cursor, static_lsp_table)
        {
            plsp = (struct static_lsp *)pbucket->data;
            if (plsp == NULL)
            {
                continue;
            }

            if (++num == 1)
            {
                vty_out(vty, "%-10s:     %d %s", "total", static_lsp_table.num_entries, VTY_NEWLINE);
                vty_out(vty, "%-10s:     %d %s", "up", gmpls.static_lsp_up_num, VTY_NEWLINE);
                vty_out(vty, "%-10s:     %d %s", "down", static_lsp_table.num_entries
                                                        -gmpls.static_lsp_up_num, VTY_NEWLINE);
                vty_out(vty, "%s", VTY_NEWLINE);
                vty_out(vty, "%-32s", "name");
                vty_out(vty, "%-19s", "fec");
                vty_out(vty, "%-16s", "I/O label");
                vty_out(vty, "%-31s", "out-interface/nexthop");
                vty_out(vty, "%-7s", "status");
                vty_out(vty, "%s", VTY_NEWLINE);
            }

            mpls_static_lsp_show_brief(vty, plsp);
        }

        if (0 != num)
        {
            vty_out(vty, "%s", VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 标签池显示命令
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 11:27:44
 * @note       :
 */
DEFUN(show_label_pool,
    show_label_pool_cmd,
    "show mpls label pool",
    MPLS_CLI_INFO)
{
    uint32_t *pbitmap = NULL;
    uint32_t label = 0;
    uint32_t i     = 0;
    uint32_t j     = 0;
    uint32_t k     = 0;

    vty_out(vty, "-----------------label pool-------------%s", VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "%20s: %-15s%s", "Label pool", "used", VTY_NEWLINE);
    vty_out(vty, "%20s: %-15s%s", "Label range", "16~1048575", VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "-------------------label----------------%s", VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);

    for (i=0; i<LABEL_POOL_MAX; i++)
    {
        pbitmap = label_pool[i].pbitmap;
        if (NULL == pbitmap)
        {
            continue;
        }

        for (j=0; j<label_pool[i].bitmap_size; j++)
        {
            for (k=0; k<32; k++)
            {
                if ((*pbitmap & (0x80000000>>k)) == 0)
                {
                    label = i*label_pool[i].index_size;
                    label += k+j*32+15;
                    if((label > 15) && (label < 1048576))
                    vty_out(vty, "%20s: %-15u%s", "Label", label, VTY_NEWLINE);
                }
            }

            pbitmap++;
        }
    }

    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "--------------------end-----------------%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


/**
 * @brief      : lsp 配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 11:28:16
 * @note       :
 */
int mpls_static_lsp_config_write(struct vty *vty)
{
    struct static_lsp  *plsp    = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;

    HASH_BUCKET_LOOP(pbucket, cursor, static_lsp_table)
    {
        plsp = pbucket->data;
        if (NULL == plsp)
        {
            continue;
        }

        //h3c_static_lsp_config_write(vty, plsp);
        static_lsp_config_write(vty, plsp);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 静态 lsp 配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 11:30:18
 * @note       :
 */
int static_lsp_config_write(struct vty *vty, struct static_lsp *plsp)
{
    char destip[INET_ADDRSTRLEN]  = "";
    char nexthop[INET_ADDRSTRLEN] = "";
    char ifname[IFNET_NAMESIZE]   = "";
    char lsp_cmd[STRING_LEN]      = "";
    int  len = 0;

    if (NULL == plsp)
    {
        return ERRNO_SUCCESS;
    }

    inet_ipv4tostr(plsp->destip.addr.ipv4, destip);

    len += sprintf(lsp_cmd+len, " mpls static-lsp %s", plsp->name);
    len += sprintf(lsp_cmd+len, " %s", plsp->direction == LSP_DIRECTION_INGRESS ?
        "ingress" : plsp->direction == LSP_DIRECTION_TRANSIT ? "transit" : "egress");
    len += sprintf(lsp_cmd+len, " destination %s/%d", destip, plsp->destip.prefixlen);

    if (plsp->nexthop.addr.ipv4 != 0)
    {
        inet_ipv4tostr(plsp->nexthop.addr.ipv4, nexthop);

        len += sprintf(lsp_cmd+len, " nexthop %s", nexthop);
    }
    else if (plsp->nhp_index != 0)
    {
        ifm_get_name_by_ifindex(plsp->nhp_index, ifname);

        len += sprintf(lsp_cmd+len, " outif %s", ifname);
        len += sprintf(lsp_cmd+len, " dmac %02x:%02x:%02x:%02x:%02x:%02x", plsp->dmac[0],
                plsp->dmac[1], plsp->dmac[2], plsp->dmac[3], plsp->dmac[4], plsp->dmac[5]);
    }

    if ((plsp->direction == LSP_DIRECTION_EGRESS)
        || (plsp->direction == LSP_DIRECTION_TRANSIT))
    {
        len += sprintf(lsp_cmd+len, " in-label %u", plsp->inlabel);
    }

    if ((plsp->direction == LSP_DIRECTION_INGRESS)
        || (plsp->direction == LSP_DIRECTION_TRANSIT))
    {
        len += sprintf(lsp_cmd+len, " out-label %u", plsp->outlabel);
    }

    lsp_cmd[len] = '\0';

    vty_out(vty, "%s%s", lsp_cmd, VTY_NEWLINE);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 静态 lsp 命令行初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 11:31:03
 * @note       :
 */
void mpls_lsp_cmd_init(void)
{
    install_element (MPLS_NODE, &ingress_static_lsp_nexthop_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &ingress_static_lsp_outif_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &transit_static_lsp_nexthop_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &transit_static_lsp_outif_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &egress_static_lsp_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &no_static_lsp_cmd, CMD_SYNC);

    install_element (MPLS_NODE, &show_static_lsp_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &show_static_lsp_cmd, CMD_LOCAL);
    install_element (PHYSICAL_IF_NODE, &show_static_lsp_cmd, CMD_LOCAL);
    install_element (PHYSICAL_SUBIF_NODE, &show_static_lsp_cmd, CMD_LOCAL);
    install_element (TRUNK_IF_NODE, &show_static_lsp_cmd, CMD_LOCAL);
    install_element (TRUNK_SUBIF_NODE, &show_static_lsp_cmd, CMD_LOCAL);
    install_element (TUNNEL_IF_NODE, &show_static_lsp_cmd, CMD_LOCAL);

    install_element (MPLS_NODE, &show_label_pool_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &show_label_pool_cmd, CMD_LOCAL);
    install_element (PHYSICAL_IF_NODE, &show_label_pool_cmd, CMD_LOCAL);
    install_element (PHYSICAL_SUBIF_NODE, &show_label_pool_cmd, CMD_LOCAL);
    install_element (TRUNK_IF_NODE, &show_label_pool_cmd, CMD_LOCAL);
    install_element (TRUNK_SUBIF_NODE, &show_label_pool_cmd, CMD_LOCAL);
    install_element (TUNNEL_IF_NODE, &show_label_pool_cmd, CMD_LOCAL);

    return;
}


