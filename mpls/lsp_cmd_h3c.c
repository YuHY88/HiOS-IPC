/**
 * @file      : static_lsp_cmd.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年4月25日 15:10:02
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
#include "lsp_cmd_h3c.h"
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

static void h3c_static_lsp_show_single(struct vty *vty, struct static_lsp *plsp)
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
                     ENABLE ? "enable" : "disable", VTY_NEWLINE);

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
static void h3c_static_lsp_show_all(struct vty *vty, struct static_lsp *plsp)
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
        vty_out(vty, "%-31s", "-");
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
DEFUN (static_lsp_ingress_nexthop_h3c,
    static_lsp_ingress_nexthop_cmd_h3c,
    "static-lsp ingress NAME destination A.B.C.D (A.B.C.D | <0-32>) nexthop A.B.C.D "
    "out-label <16-1048575>",
    MPLS_CLI_INFO)
{
    struct static_lsp  lsp;
    struct prefix_ipv4 destip;
    struct prefix_ipv4 mask;
    uint32_t           nexthop;
    int                ret;

    memset(&lsp, 0, sizeof(struct static_lsp));
    lsp.direction  = LSP_DIRECTION_INGRESS;
    lsp.lsp_type   = LSP_TYPE_STATIC;
    lsp.nhp_type   = NHP_TYPE_CONNECT;
    lsp.admin_down = LINK_DOWN;
    lsp.down_flag  = LINK_DOWN;
    lsp.status     = ENABLE;

    /* argv[0]: lsp name */
    memcpy(lsp.name, argv[0], strlen(argv[0])+1);

    /* argv[1]: prefix */
    destip.prefix.s_addr = inet_strtoipv4((char *)argv[1]);
    if ((0 == destip.prefix.s_addr) || (FALSE == inet_valid_network(destip.prefix.s_addr)))
    {
        vty_error_out(vty, "The specified destination is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[2]: mask/masklen */
    if (all_digit(argv[2]))
    {
        destip.prefixlen = atoi(argv[2]);
    }
    else
    {
        mask.prefix.s_addr = inet_strtoipv4((char *)argv[2]);

        if (!inet_valid_mask(mask.prefix.s_addr))
        {
            vty_error_out(vty, "The specified mask address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        mask.prefix.s_addr = htonl(mask.prefix.s_addr);
        destip.prefixlen = ip_masklen(mask.prefix);
    }

    lsp.destip.addr.ipv4 = ipv4_network_addr(htonl(destip.prefix.s_addr), destip.prefixlen);
    lsp.destip.addr.ipv4 = ntohl(lsp.destip.addr.ipv4);
    lsp.destip.prefixlen = destip.prefixlen;

    /* argv[3]: nexthop */
    nexthop = inet_strtoipv4((char *)argv[3]);
    if ((0 == nexthop) || (FALSE == inet_valid_network(nexthop)))
    {
        vty_error_out(vty, "The specified nexthop address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lsp.nexthop.addr.ipv4 = nexthop;

    /* argv[4]: outlabel */
    lsp.outlabel = atoi(argv[4]);

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
DEFUN(static_lsp_transit_nexthop_h3c,
    static_lsp_transit_nexthop_cmd_h3c,
    "static-lsp transit NAME in-label <16-1048575> nexthop A.B.C.D "
    "out-label <16-1048575>",
    MPLS_CLI_INFO)
{
    struct static_lsp  lsp;
    uint32_t           nexthop;
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

    /* argv[1]: in-label */
    lsp.inlabel = atoi(argv[1]);
    ret = label_get(lsp.inlabel, MODULE_ID_SLSP);
    if (0 == ret)
    {
        vty_error_out(vty, "The in label being used.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[2]: nexthop */
    nexthop = inet_strtoipv4((char *)argv[2]);
    if ((0 == nexthop) || (FALSE == inet_valid_network(nexthop)))
    {
        vty_error_out(vty, "The specified nexthop address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lsp.nexthop.addr.ipv4 = nexthop;

    /* argv[3]: out-label */
    lsp.outlabel = atoi(argv[3]);

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
DEFUN(static_lsp_egress_h3c,
    static_lsp_egress_cmd_h3c,
    "static-lsp egress NAME in-label <16-1048575>",
    MPLS_CLI_INFO)
{
    struct static_lsp  lsp;
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

    /* argv[1] : label */
    lsp.inlabel = atoi(argv[1]);

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
DEFUN(no_static_lsp_h3c,
    no_static_lsp_cmd_h3c,
    "no static-lsp (ingress | transit | egress) NAME",
    MPLS_CLI_INFO)
{
    struct static_lsp *plsp = NULL;
    enum LSP_DIRECTION direction = LSP_DIRECTION_INGRESS;
    int ret;

    if (argv[0][0] == 'i')
    {
        direction = LSP_DIRECTION_INGRESS;
    }
    else if (argv[0][0] == 't')
    {
        direction = LSP_DIRECTION_TRANSIT;
    }
    else if (argv[0][0] == 'e')
    {
        direction = LSP_DIRECTION_EGRESS;
    }

    plsp = static_lsp_lookup((uchar *)argv[1]);
    if (NULL == plsp)
    {
        return CMD_SUCCESS;
    }

    if (plsp->direction != direction)
    {
        return CMD_SUCCESS;
    }

    ret = static_lsp_delete((uchar *)argv[1]);
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
DEFUN(show_static_lsp_h3c,
    show_static_lsp_cmd_h3c,
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

        h3c_static_lsp_show_single(vty, plsp);

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
                vty_out(vty, "%-31s", "out-interface");
                vty_out(vty, "%-7s", "status");
                vty_out(vty, "%s", VTY_NEWLINE);
            }

            h3c_static_lsp_show_all(vty, plsp);
        }

        if (0 != num)
        {
            vty_out(vty, "%s", VTY_NEWLINE);
        }
    }

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
int h3c_lsp_config_write(struct vty *vty)
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

        h3c_static_lsp_config_write(vty, plsp);
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
int h3c_static_lsp_config_write(struct vty *vty, struct static_lsp *plsp)
{
    char destip[INET_ADDRSTRLEN]  = "";
    char nexthop[INET_ADDRSTRLEN] = "";

    if (NULL == plsp)
    {
        return ERRNO_SUCCESS;
    }

    if (plsp->direction == LSP_DIRECTION_INGRESS)
    {
        inet_ipv4tostr(plsp->destip.addr.ipv4, destip);
        inet_ipv4tostr(plsp->nexthop.addr.ipv4, nexthop);

        vty_out(vty, "static-lsp ingress %s destination %s %d nexthop %s out-label %u%s",
                    plsp->name, destip, plsp->destip.prefixlen, nexthop, plsp->outlabel, VTY_NEWLINE);
    }
    else if (plsp->direction == LSP_DIRECTION_TRANSIT)
    {
        inet_ipv4tostr(plsp->nexthop.addr.ipv4, nexthop);

        vty_out(vty, "static-lsp transit %s in-label %u nexthop %s out-label %u%s",
                    plsp->name, plsp->inlabel, nexthop, plsp->outlabel, VTY_NEWLINE);
    }
    else if (plsp->direction == LSP_DIRECTION_EGRESS)
    {
        vty_out(vty, "static-lsp egress %s in-label %u%s",
                    plsp->name, plsp->inlabel, VTY_NEWLINE);
    }

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
void h3c_static_lsp_cmd_init(void)
{
    install_element (CONFIG_NODE, &static_lsp_ingress_nexthop_cmd_h3c, CMD_SYNC);
    install_element (CONFIG_NODE, &static_lsp_transit_nexthop_cmd_h3c, CMD_SYNC);
    install_element (CONFIG_NODE, &static_lsp_egress_cmd_h3c, CMD_SYNC);
    install_element (CONFIG_NODE, &no_static_lsp_cmd_h3c, CMD_SYNC);

#if 0
    install_element (CONFIG_NODE, &show_static_lsp_cmd_h3c, CMD_LOCAL);
    install_element (PHYSICAL_IF_NODE, &show_static_lsp_cmd_h3c, CMD_LOCAL);
    install_element (PHYSICAL_SUBIF_NODE, &show_static_lsp_cmd_h3c, CMD_LOCAL);
    install_element (TRUNK_IF_NODE, &show_static_lsp_cmd_h3c, CMD_LOCAL);
    install_element (TRUNK_SUBIF_NODE, &show_static_lsp_cmd_h3c, CMD_LOCAL);
    install_element (TUNNEL_IF_NODE, &show_static_lsp_cmd_h3c, CMD_LOCAL);
#endif

    return;
}


