/**
 * @file      : l3vpn_cmd.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 9:41:19
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/hash1.h>
#include <lib/memory.h>
#include <lib/prefix.h>
#include <lib/command.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/log.h>
#include "lib/ospf_common.h"
#include "mpls_main.h"
#include "l3vpn.h"
#include "l3vpn_cmd.h"
#include "mpls_register.h"


extern int inet_aton(const char *cp, struct in_addr *addr);


/* 初始化 mpls node */
static struct cmd_node l3vpn_node =
{
    L3VPN_NODE,
    "%s(mpls-l3vpn)# ",
    1
};


/**
 * @brief      : l3vpn 实例创建删除下发
 * @param[in ] : pinfo  - l3vpn 信息结构
 * @param[in ] : opcode - 操作码
 * @param[in ] : info   - 消息类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:44:13
 * @note       :
 */
static int mpls_l3vpn_send(struct l3vpn_info *pinfo, enum OPCODE_E opcode,
                            enum L3VPN_INFO_E info)
{
    int ret = ERRNO_SUCCESS;

    if (NULL == pinfo)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (OPCODE_ADD == opcode)
    {
        ret = mpls_ipc_send_hal_wait_ack(pinfo, sizeof(struct l3vpn_info), 1,
                                    MODULE_ID_MPLS, IPC_TYPE_L3VPN, IPC_OPCODE_ADD, info, pinfo->vrf_id);
        if (ERRNO_SUCCESS != ret)
        {
            return ret;
        }
    }
    else if (OPCODE_UPDATE == opcode)
    {
        ret = MPLS_IPC_SENDTO_HAL(pinfo, sizeof(struct l3vpn_info), 1, MODULE_ID_MPLS,
                            IPC_TYPE_L3VPN, IPC_OPCODE_UPDATE, info, pinfo->vrf_id);
        if (ERRNO_SUCCESS != ret)
        {
            return ERRNO_IPC;
        }
    }
    else if (OPCODE_DELETE == opcode)
    {
        ret = MPLS_IPC_SENDTO_HAL(pinfo, sizeof(struct l3vpn_info), 1, MODULE_ID_MPLS,
                            IPC_TYPE_L3VPN, IPC_OPCODE_DELETE, info, pinfo->vrf_id);
        if (ERRNO_SUCCESS != ret)
        {
            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 配置信息发送
 * @param[in ] : pl3vpn  - l3vpn 结构
 * @param[in ] : subtype - 消息子类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:48:09
 * @note       :
 */
static int mpls_l3vpn_send_config(struct l3vpn_entry *pl3vpn, enum L3VPN_SUBTYPE subtype)
{
    int ret;

    ret = ipc_send_msg_n2(pl3vpn, sizeof(struct l3vpn_entry), 1, MODULE_ID_BGP,
                        MODULE_ID_MPLS, IPC_TYPE_L3VPN, subtype, IPC_OPCODE_EVENT, 0);
    if (ERRNO_SUCCESS != ret)
    {
        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn target 发送
 * @param[in ] : pl3vpn  - l3vpn 结构
 * @param[in ] : ptarget - target 内容
 * @param[in ] : subtype - 消息子类型
 * @param[in ] : opcode  - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:49:12
 * @note       :
 */
static int mpls_l3vpn_send_target(struct l3vpn_entry *pl3vpn, uchar *ptarget,
                                    enum L3VPN_SUBTYPE subtype, enum OPCODE_E opcode)
{
    int ret = 0;
    int msg_len = NAME_STRING_LEN;

    if (NULL == pl3vpn)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (OPCODE_ADD == opcode)
    {
        if (NULL == ptarget)
        {
            msg_len = 0;
        }

        ret = ipc_send_msg_n2(ptarget, msg_len, 1, MODULE_ID_BGP, MODULE_ID_MPLS,
                            IPC_TYPE_L3VPN, subtype, IPC_OPCODE_ADD, pl3vpn->l3vpn.vrf_id);
        if (ERRNO_SUCCESS != ret)
        {
            return ERRNO_IPC;
        }
    }
    else if (OPCODE_DELETE == opcode)
    {
        if (NULL == ptarget)
        {
            msg_len = 0;
        }

        ret = ipc_send_msg_n2(ptarget, msg_len, 1, MODULE_ID_BGP, MODULE_ID_MPLS,
                            IPC_TYPE_L3VPN, subtype, IPC_OPCODE_DELETE, pl3vpn->l3vpn.vrf_id);
        if (ERRNO_SUCCESS != ret)
        {
            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 显示 l3vpn 详细配置信息
 * @param[in ] : vty    - vty 全局结构
 * @param[in ] : pl3vpn - l3vpn 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:50:35
 * @note       :
 */
static void mpls_l3vpn_show_single(struct vty *vty, struct l3vpn_entry *pl3vpn)
{
    struct listnode *pnode   = NULL;
    uchar           *ptarget = NULL;

    vty_out(vty, "%-20s : %u%s", "VPN-instance", pl3vpn->l3vpn.vrf_id, VTY_NEWLINE);
    vty_out(vty, "%-20s : %s%s", "VPN-instance name", pl3vpn->name, VTY_NEWLINE);
    vty_out(vty, "%-20s : %s%s", "Route Distinguisher", pl3vpn->rd, VTY_NEWLINE);
    vty_out(vty, "%-20s : %s%s", "Label apply", pl3vpn->label_apply==LABEL_APPLY_PER_ROUTE?
                "route":"vpn", VTY_NEWLINE);

    if (LABEL_APPLY_PER_VPN == pl3vpn->label_apply)
    {
        vty_out(vty, "%-20s : %u%s", "Label", pl3vpn->inlabel, VTY_NEWLINE);
    }

    if (0 != pl3vpn->export_list.count)
    {
        vty_out(vty, "%-20s : %s", "Export VPN Targets", VTY_NEWLINE);
    }

    for (ALL_LIST_ELEMENTS_RO(&pl3vpn->export_list, pnode, ptarget))
    {
        vty_out(vty, "%-20s   %s%s", "", ptarget, VTY_NEWLINE);
    }

    if (0 != pl3vpn->import_list.count)
    {
        vty_out(vty, "%-20s : %s", "Import VPN Targets", VTY_NEWLINE);
    }

    for (ALL_LIST_ELEMENTS_RO(&pl3vpn->import_list, pnode, ptarget))
    {
        vty_out(vty, "%-20s   %s%s", "", ptarget, VTY_NEWLINE);
    }

    return;
}


/**
 * @brief      : 显示 l3vpn 信息
 * @param[in ] : vty    - vty 全局结构
 * @param[in ] : pl3vpn - l3vpn 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:51:35
 * @note       :
 */
static void mpls_l3vpn_show_all(struct vty *vty, struct l3vpn_entry *pl3vpn)
{
    vty_out(vty, "%-20u", pl3vpn->l3vpn.vrf_id);
    vty_out(vty, "%-20s", pl3vpn->rd);
    vty_out(vty, "%s", VTY_NEWLINE);

    return;
}


/**
 * @brief      : 显示 l3vpn 实例内所有路由
 * @param[in ] : vty     - vty 全局结构
 * @param[in ] : pl3vpn  - l3vpn 结构
 * @param[in ] : pvroute - l3vpn 路由结构
 * @param[in ] : pnhp    - l3vpn 下一跳结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:52:12
 * @note       :
 */
static void mpls_l3vpn_show_route_all(struct vty *vty, struct l3vpn_entry *pl3vpn,
                                        struct l3vpn_route *pvroute, struct l3vpn_nhp *pnhp)
{
    char ip_mask[INET_ADDRSTRLEN+5] = "";
    char destip[INET_ADDRSTRLEN]    = "";
    char nexthop[INET_ADDRSTRLEN]   = "";
    char label_buf[NAME_STRING_LEN] = "";
    int  len = 0;

    if ((NULL == pl3vpn) || (NULL == pvroute) || (NULL == pnhp))
    {
        return;
    }

    inet_ipv4tostr(pvroute->prefix.addr.ipv4, destip);
    inet_ipv4tostr(pnhp->nexthop.addr.ipv4, nexthop);

    sprintf(ip_mask, "%s/%d", destip, pvroute->prefix.prefixlen);

    if (pnhp->inlabel != 0)
    {
        len = sprintf(label_buf, "%u/", pnhp->inlabel);
    }
    else
    {
        len = sprintf(label_buf, "-/");
    }

    if (pnhp->outlabel != 0)
    {
        sprintf(label_buf+len, "%u", pnhp->outlabel);
    }
    else
    {
        sprintf(label_buf+len, "-");
    }

    vty_out(vty, "%-5u" , pl3vpn->l3vpn.vrf_id);
    vty_out(vty, "%-19s", ip_mask);
    vty_out(vty, "%-16s", nexthop);
    vty_out(vty, "%-16s", label_buf);
    vty_out(vty, "%-6s", LINK_DOWN==pnhp->down_flag?"DOWN":"UP");
    vty_out(vty, "%s", VTY_NEWLINE);

    return;
}


/**
 * @brief      : l3vpn target 格式检查
 * @param[in ] : str - target 内容
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:54:05
 * @note       : target 只支持 6 字节长度
 */
static int mpls_l3vpn_rd_target_check(char *str)
{
    char    *str1 = NULL;   // 指向冒号后的字符串
    char    *str2 = NULL;   // 指向冒号前的字符串
    char    *str3 = NULL;   // 指向  '.'
    char    *str4 = NULL;   // 指向 str2，做指针偏移
    char     c    = '0';
    uint64_t val1 = 0;      // 存储冒号后整数
    uint64_t val2 = 0;      // 存储冒号前整数
    int      dot  = 0;
    int      ret  = ERRNO_SUCCESS;

    if (NULL == str)
    {
        return ERRNO_FAIL;
    }

    /* 未检测到冒号，返回失败 */
    str1 = strchr(str, ':');
    if (NULL == str1)
    {
        return ERRNO_FAIL;
    }

    /* 冒号后无内容 */
    if (*(str1+1) == '\0')
    {
        return ERRNO_FAIL;
    }

    /* 冒号后有非数字字符 */
    if (!all_digit(str1+1))
    {
        return ERRNO_FAIL;
    }

    /* 冒号后整数大于 uint32_t */
    val1 = strtoull(str1+1, NULL, 10);
    if (val1 > 0xffffffff)
    {
        return ERRNO_FAIL;
    }

    str2 = malloc((str1-str)+1);

    memcpy (str2, str, (str1-str));
    str2[str1-str] = '\0';

    str3 = strchr(str, '.');
    if (NULL == str3)
    {
        /* 冒号前不是点分十进制格式，有非数字字符 */
        if (!all_digit(str2))
        {
            goto fail;
        }

        /* 冒号前整数大于 uint32_t */
        val2 = strtoull(str2, NULL, 10);
        if (val2 > 0xffffffff)
        {
            goto fail;
        }

        /* 冒号前后整数都为0 */
        if ((0 == val1) && (0 == val2))
        {
            goto fail;
        }

        /* 长度超过 6 字节 */
        if ((val1 > 65535) && (val2 > 65535))
        {
            goto fail;
        }
    }
    else
    {
        /* 冒号前点分十进制格式不正确 */
        ret = inet_aton(str2, NULL);
        if (0 == ret)
        {
            goto fail;
        }

        str4 = str2;

        /* 点分十进制格式点数不为 3 */
        while (*str4 != '\0')
        {
            c = *str4++;
            if (c == '.')
            {
                dot += 1;
            }
        }

        if (3 != dot)
        {
            goto fail;
        }

        /* 冒号后数字大于 2 字节，总字节长度超过 6 字节 */
        if (val1 > 65535)
        {
            goto fail;
        }
    }

    free(str2);

    return ERRNO_SUCCESS;

fail:

    free(str2);

    return ERRNO_FAIL;
}


/**
 * @brief      : l3vpn rd 冲突检查
 * @param[in ] : prd - rd 内容
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:57:59
 * @note       :
 */
static int mpls_l3vpn_check_rd_conflict(uchar *prd)
{
    struct l3vpn_entry *pl3vpn  = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;

    if (NULL == prd)
    {
        return ERRNO_SUCCESS;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, l3vpn_table)
    {
        pl3vpn = (struct l3vpn_entry *)pbucket->data;
        if (NULL == pl3vpn)
        {
            continue;
        }

        if (strlen((const void *)prd) != strlen((const void *)pl3vpn->rd))
        {
            continue;
        }

        if (0 == memcmp(prd, pl3vpn->rd, strlen((const void *)prd)))
        {
            return ERRNO_FAIL;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 配置管理
 * @param[in ] : vty    - vty 全局结构
 * @param[in ] : pl3vpn - l3vpn 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:58:55
 * @note       :
 */
static void mpls_l3vpn_config_write_process(struct vty *vty, struct l3vpn_entry *pl3vpn)
{
    struct listnode *pnode   = NULL;
    uchar           *ptarget = NULL;

    vty_out(vty, " l3vpn instance %u%s", pl3vpn->l3vpn.vrf_id, VTY_NEWLINE);

    if ('\0' != pl3vpn->name[0])
    {
        vty_out(vty, "  name %s%s", pl3vpn->name, VTY_NEWLINE);
    }

    if ('\0' != pl3vpn->rd[0])
    {
        vty_out(vty, "  route-distinguisher %s%s", pl3vpn->rd, VTY_NEWLINE);
    }

    for (ALL_LIST_ELEMENTS_RO(&pl3vpn->import_list, pnode, ptarget))
    {
        vty_out(vty, "  vpn-target %s import-extcommunity%s", ptarget, VTY_NEWLINE);
    }

    for (ALL_LIST_ELEMENTS_RO(&pl3vpn->export_list, pnode, ptarget))
    {
        vty_out(vty, "  vpn-target %s export-extcommunity%s", ptarget, VTY_NEWLINE);
    }

    if (LABEL_APPLY_PER_ROUTE != pl3vpn->label_apply)
    {
        vty_out(vty, "  apply-label per-instance%s", VTY_NEWLINE);
    }

    return;
}


/**
 * @brief      : l3vpn 实例创建命令
 * @param[in ] : instance <1-128> - l3vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:59:47
 * @note       :
 */
DEFUN(mpls_l3vpn_instance,
    mpls_l3vpn_instance_cmd,
    "l3vpn instance <1-128>",
    MPLS_CLI_INFO)
{
    struct l3vpn_entry *pl3vpn  = NULL;
    char               *pprompt = NULL;
    uint16_t vrf_id = 0;
    int      ret    = ERRNO_SUCCESS;

    vrf_id = atoi(argv[0]);

    pl3vpn = mpls_l3vpn_lookup(vrf_id);
    if (NULL == pl3vpn)
    {
        pl3vpn = mpls_l3vpn_create(vrf_id);
        if (NULL == pl3vpn)
        {
            vty_error_out(vty, "Memory alloc failed !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        ret = mpls_l3vpn_add(pl3vpn);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            mpls_l3vpn_free(pl3vpn);

            return CMD_WARNING;
        }

        ret = mpls_l3vpn_send(&pl3vpn->l3vpn, OPCODE_ADD, L3VPN_INFO_INVALID);
        if (ERRNO_SUCCESS != ret)
        {
            mpls_l3vpn_delete(vrf_id);

            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    vty->index = pl3vpn;
    vty->node  = L3VPN_NODE;

    pprompt = vty->change_prompt;
    if (NULL != pprompt)
    {
        snprintf(pprompt, VTY_BUFSIZ, "%%s(mpls-l3vpn-%s)# ", argv[0]);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : l3vpn 实例删除命令
 * @param[in ] : instance <1-128> - l3vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:00:52
 * @note       :
 */
DEFUN(no_mpls_l3vpn_instance,
    no_mpls_l3vpn_instance_cmd,
    "no l3vpn instance <1-128>",
    MPLS_CLI_INFO)
{
    struct l3vpn_entry *pl3vpn = NULL;
    uint16_t vrf_id = 0;
    int      ret    = ERRNO_SUCCESS;

    vrf_id = atoi(argv[0]);

    pl3vpn = mpls_l3vpn_lookup(vrf_id);
    if (NULL == pl3vpn)
    {
        return CMD_SUCCESS;
    }

    ret = mpls_l3vpn_send(&pl3vpn->l3vpn, OPCODE_DELETE, L3VPN_INFO_INVALID);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_SUCCESS;
    }

    l3vpn_event_notify(pl3vpn, IPC_OPCODE_DELETE);
    mpls_l3vpn_delete(vrf_id);

    return CMD_SUCCESS;
}


/**
 * @brief      : l3vpn 实例名称配置命令
 * @param[in ] : name NAME - l3vpn 实例名称
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:01:42
 * @note       :
 */
DEFUN(mpls_l3vpn_name,
    mpls_l3vpn_name_cmd,
    "name NAME",
    MPLS_CLI_INFO)
{
    struct l3vpn_entry *pl3vpn = (struct l3vpn_entry *)vty->index;

    memcpy(pl3vpn->name, argv[0], NAME_STRING_LEN);

    return CMD_SUCCESS;
}


/**
 * @brief      : l3vpn rd 配置命令
 * @param[in ] : route-distinguisher RD - route distinguisher
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:02:37
 * @note       :
 */
DEFUN(mpls_l3vpn_rd,
    mpls_l3vpn_rd_cmd,
    "route-distinguisher RD",
    MPLS_CLI_INFO)
{
    struct l3vpn_entry *pl3vpn = (struct l3vpn_entry *)vty->index;
    int ret = ERRNO_SUCCESS;

    ret = mpls_l3vpn_rd_target_check((char *)argv[0]);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "The specified RD '%s' is invalid.%s", argv[0], VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ('\0' != pl3vpn->rd[0])
    {
        vty_error_out(vty, "The VPN instance already has an RD.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = mpls_l3vpn_check_rd_conflict((uchar *)argv[0]);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "An RD must be unique.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    memcpy(pl3vpn->rd, argv[0], sizeof(pl3vpn->rd));

    mpls_l3vpn_send_config(pl3vpn, L3VPN_SUBTYPE_RD);

    return CMD_SUCCESS;
}


/**
 * @brief      : l3vpn target 配置命令
 * @param[in ] : both                - 内部、外部属性同时配置
 * @param[in ] : export-extcommunity - 外部属性配置
 * @param[in ] : import-extcommunity - 内部属性配置
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:03:51
 * @note       :
 */
DEFUN(mpls_l3vpn_target,
    mpls_l3vpn_target_cmd,
    "vpn-target TARGET {both | export-extcommunity | import-extcommunity}",
    MPLS_CLI_INFO)
{
    struct l3vpn_entry *pl3vpn     = (struct l3vpn_entry *)vty->index;
    struct listnode    *pnode      = NULL;
    uchar              *ptarget    = NULL;
    uchar              *ptarget_im = NULL;
    uchar              *ptarget_ex = NULL;
    int config_import = 0;
    int config_export = 0;
    int import_flag   = 0;
    int export_flag   = 0;
    int ret           = ERRNO_SUCCESS;

    ret = mpls_l3vpn_rd_target_check((char *)argv[0]);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "The specified target '%s' is invalid.%s", argv[0], VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ('\0' == pl3vpn->rd[0])
    {
        vty_error_out(vty, "Please configure an RD at first !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ((NULL == argv[2]) && (NULL == argv[3]))
    {
        /* 命令行参数选择 both 或者都不选 */
        config_import = 1;
        config_export = 1;

        if (pl3vpn->export_list.count == L3VPN_TARGET_EXPORT_MAX)
        {
            vty_error_out(vty, "VPN-Target export list is full.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (pl3vpn->import_list.count == L3VPN_TARGET_IMPORT_MAX)
        {
            vty_error_out(vty, "VPN-Target import list is full.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else if (NULL != argv[2])
    {
        /* 命令行参数选择 export-extcommunity */
        config_export = 1;

        if (pl3vpn->export_list.count == L3VPN_TARGET_EXPORT_MAX)
        {
            vty_error_out(vty, "VPN-Target export list is full.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else if (NULL != argv[3])
    {
        /* 命令行参数选择 import-extcommunity */
        config_import = 1;

        if (pl3vpn->import_list.count == L3VPN_TARGET_IMPORT_MAX)
        {
            vty_error_out(vty, "VPN-Target import list is full.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    /* 配置 import */
    if (1 == config_import)
    {
        for (ALL_LIST_ELEMENTS_RO(&pl3vpn->import_list, pnode, ptarget))
        {
            if ((strlen((const char *)argv[0]) == strlen((const char *)ptarget))
                && (0 == memcmp(argv[0], ptarget, strlen((const char *)argv[0]))))
            {
                import_flag = 1;

                break;
            }
        }

        if (0 == import_flag)
        {
            ptarget_im = (uchar *)XCALLOC(MTYPE_VRF, NAME_STRING_LEN);
            if (NULL == ptarget_im)
            {
                vty_error_out(vty, "Out of memory.%s", VTY_NEWLINE);

                return CMD_WARNING;
            }

            memcpy(ptarget_im, argv[0], NAME_STRING_LEN);

            listnode_add(&pl3vpn->import_list, ptarget_im);
            mpls_l3vpn_send_target(pl3vpn, ptarget_im, L3VPN_SUBTYPE_IMPORT, OPCODE_ADD);
        }
    }

    /* 配置 export */
    if (1 == config_export)
    {
        for (ALL_LIST_ELEMENTS_RO(&pl3vpn->export_list, pnode, ptarget))
        {
            if ((strlen((const char *)argv[0]) == strlen((const char *)ptarget))
                && (0 == memcmp(argv[0], ptarget, strlen((const char *)argv[0]))))
            {
                export_flag = 1;

                break;
            }
        }

        if (0 == export_flag)
        {
            ptarget_ex = (uchar *)XCALLOC(MTYPE_VRF, NAME_STRING_LEN);
            if (NULL == ptarget_ex)
            {
                vty_error_out(vty, "Out of memory.%s", VTY_NEWLINE);

                return CMD_WARNING;
            }

            memcpy(ptarget_ex, argv[0], NAME_STRING_LEN);

            listnode_add(&pl3vpn->export_list, ptarget_ex);
            mpls_l3vpn_send_target(pl3vpn, ptarget_ex, L3VPN_SUBTYPE_EXPORT, OPCODE_ADD);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : l3vpn target 删除命令
 * @param[in ] : both                - 内部、外部属性同时删除
 * @param[in ] : export-extcommunity - 外部属性删除
 * @param[in ] : import-extcommunity - 内部属性删除
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:07:19
 * @note       :
 */
DEFUN(no_mpls_l3vpn_target,
    no_mpls_l3vpn_target_cmd,
    "no vpn-target TARGET {both | export-extcommunity | import-extcommunity}",
    MPLS_CLI_INFO)
{
    struct l3vpn_entry *pl3vpn     = (struct l3vpn_entry *)vty->index;
    struct listnode    *pnode      = NULL;
    uchar              *ptarget_im = NULL;
    uchar              *ptarget_ex = NULL;
    int config_import = 0;
    int config_export = 0;
    int import_flag   = 0;
    int export_flag   = 0;
    int ret           = ERRNO_SUCCESS;

    ret = mpls_l3vpn_rd_target_check((char *)argv[0]);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "The specified target '%s' is invalid.%s", argv[0], VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ((NULL == argv[2]) && (NULL == argv[3]))
    {
        /* 命令行参数选择 both 或者都不选 */
        config_import = 1;
        config_export = 1;
    }
    else if (NULL != argv[2])
    {
        /* 命令行参数选择 export-extcommunity */
        config_export = 1;
    }
    else if (NULL != argv[3])
    {
        /* 命令行参数选择 import-extcommunity */
        config_import = 1;
    }

    /* 删除 import */
    if (1 == config_import)
    {
        for (ALL_LIST_ELEMENTS_RO(&pl3vpn->import_list, pnode, ptarget_im))
        {
            if ((strlen((const char *)argv[0]) == strlen((const char *)ptarget_im))
                && (0 == memcmp(argv[0], ptarget_im, strlen((const char *)argv[0]))))
            {
                import_flag = 1;

                break;
            }
        }

        if (1 == import_flag)
        {
            listnode_delete(&pl3vpn->import_list, ptarget_im);
            mpls_l3vpn_send_target(pl3vpn, ptarget_im, L3VPN_SUBTYPE_IMPORT, OPCODE_DELETE);
        }
    }

    /* 删除 export */
    if (1 == config_export)
    {
        for (ALL_LIST_ELEMENTS_RO(&pl3vpn->export_list, pnode, ptarget_ex))
        {
            if ((strlen((const char *)argv[0]) == strlen((const char *)ptarget_ex))
                && (0 == memcmp(argv[0], ptarget_ex, strlen((const char *)argv[0]))))
            {
                export_flag = 1;

                break;
            }
        }

        if (1 == export_flag)
        {
            listnode_delete(&pl3vpn->export_list, ptarget_ex);
            mpls_l3vpn_send_target(pl3vpn, ptarget_ex, L3VPN_SUBTYPE_EXPORT, OPCODE_DELETE);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除 vpn 实例内所有 target
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:11:42
 * @note       :
 */
DEFUN(no_mpls_l3vpn_target_all,
    no_mpls_l3vpn_target_all_cmd,
    "no vpn-target all",
    MPLS_CLI_INFO)
{
    struct l3vpn_entry *pl3vpn    = (struct l3vpn_entry *)vty->index;
    struct listnode    *pnode     = NULL;
    struct listnode    *pnextnode = NULL;
    uchar              *ptarget   = NULL;

    if ((pl3vpn->import_list.count != 0) || (pl3vpn->export_list.count))
    {
        mpls_l3vpn_send_target(pl3vpn, NULL, L3VPN_SUBTYPE_TARGET, OPCODE_DELETE);
    }

    for (ALL_LIST_ELEMENTS(&pl3vpn->import_list, pnode, pnextnode, ptarget))
    {
        XFREE(MTYPE_VRF, ptarget);
        list_delete_node(&pl3vpn->import_list, pnode);
    }

    for (ALL_LIST_ELEMENTS(&pl3vpn->export_list, pnode, pnextnode, ptarget))
    {
        XFREE(MTYPE_VRF, ptarget);
        list_delete_node(&pl3vpn->export_list, pnode);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : l3vpn 标签分配方式配置命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:13:59
 * @note       : 默认一条路由一个标签
 */
DEFUN(mpls_l3vpn_apply_label,
    mpls_l3vpn_apply_label_cmd,
    "apply-label per-instance",
    MPLS_CLI_INFO)
{
    struct l3vpn_entry *pl3vpn = (struct l3vpn_entry *)vty->index;

    if (LABEL_APPLY_PER_VPN == pl3vpn->label_apply)
    {
        return CMD_SUCCESS;
    }

    pl3vpn->label_apply = LABEL_APPLY_PER_VPN;

    mpls_l3vpn_send_config(pl3vpn, L3VPN_SUBTYPE_APPLY);

    return CMD_SUCCESS;
}


/**
 * @brief      : l3vpn 标签分配方式恢复默认命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:14:52
 * @note       : 默认一条路由一个标签
 */
DEFUN(no_mpls_l3vpn_apply_label,
    no_mpls_l3vpn_apply_label_cmd,
    "no apply-label per-instance",
    MPLS_CLI_INFO)
{
    struct l3vpn_entry *pl3vpn = (struct l3vpn_entry *)vty->index;

    if (LABEL_APPLY_PER_ROUTE == pl3vpn->label_apply)
    {
        return CMD_SUCCESS;
    }

    pl3vpn->inlabel     = 0;
    pl3vpn->label_apply = LABEL_APPLY_PER_ROUTE;

    mpls_l3vpn_send_config(pl3vpn, L3VPN_SUBTYPE_APPLY);

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 l3vpn 实例命令
 * @param[in ] : instance <1-128> - l3vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:15:40
 * @note       :
 */
DEFUN(mpls_l3vpn_show,
    mpls_l3vpn_show_cmd,
    "show l3vpn instance [<1-128>]",
    MPLS_CLI_INFO)
{
    struct l3vpn_entry *pl3vpn  = NULL;
    struct hash_bucket *pbucket = NULL;
    uint16_t vrf_id = 0;
    int cursor;
    int num = 0;

    if (NULL == argv[0])
    {
        HASH_BUCKET_LOOP(pbucket, cursor, l3vpn_table)
        {
            pl3vpn = (struct l3vpn_entry *)pbucket->data;
            if (NULL == pl3vpn)
            {
                continue;
            }

            if (++num == 1)
            {
                vty_out(vty, "Total : %u%s", l3vpn_table.num_entries, VTY_NEWLINE);
                vty_out(vty, "%s", VTY_NEWLINE);
                vty_out(vty, "%-20s", "VPN-instance");
                vty_out(vty, "%-20s", "RD");
                vty_out(vty, "%s", VTY_NEWLINE);
            }

            mpls_l3vpn_show_all(vty, pl3vpn);
        }
    }
    else
    {
        vrf_id = atoi(argv[0]);

        pl3vpn = mpls_l3vpn_lookup(vrf_id);
        if (NULL != pl3vpn)
        {
            mpls_l3vpn_show_single(vty, pl3vpn);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 l3vpn 路由命令
 * @param[in ] : instance <1-128> - l3vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:16:29
 * @note       :
 */
DEFUN(mpls_l3vpn_show_route,
    mpls_l3vpn_show_route_cmd,
    "show l3vpn route [instance <1-128>]",
    MPLS_CLI_INFO)
{
    struct l3vpn_entry *pl3vpn    = NULL;
    struct hash_bucket *pbucket   = NULL;
    struct l3vpn_route *pvroute   = NULL;
    struct ptree_node  *pnode     = NULL;
    struct listnode    *plistnode = NULL;
    struct l3vpn_nhp   *pnhp      = NULL;
    uint16_t vrf_id = 0;
    int cursor;
    int num = 0;

    if (NULL == argv[0])
    {
        HASH_BUCKET_LOOP(pbucket, cursor, l3vpn_table)
        {
            pl3vpn = (struct l3vpn_entry *)pbucket->data;
            if (NULL == pl3vpn)
            {
                continue;
            }

            PTREE_LOOP(&pl3vpn->route_tree, pvroute, pnode)
            {
                for (ALL_LIST_ELEMENTS_RO(&pvroute->nhplist, plistnode, pnhp))
                {
                    if (++num == 1)
                    {
                        vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
                        vty_out(vty, "MPLS l3vpn routing Tables: All%s", VTY_NEWLINE);
                        vty_out(vty, "%s", VTY_NEWLINE);
                        vty_out(vty, "%-5s" , "Vpn");
                        vty_out(vty, "%-19s", "Destination/Mask");
                        vty_out(vty, "%-16s", "Nexthop");
                        vty_out(vty, "%-16s", "I/O label");
                        vty_out(vty, "%-6s", "status");
                        vty_out(vty, "%s", VTY_NEWLINE);
                    }

                    mpls_l3vpn_show_route_all(vty, pl3vpn, pvroute, pnhp);
                }
            }
        }
    }
    else
    {
        vrf_id = atoi(argv[0]);

        pl3vpn = mpls_l3vpn_lookup(vrf_id);
        if (NULL != pl3vpn)
        {
            PTREE_LOOP(&pl3vpn->route_tree, pvroute, pnode)
            {
                for (ALL_LIST_ELEMENTS_RO(&pvroute->nhplist, plistnode, pnhp))
                {
                    if (++num == 1)
                    {
                        vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
                        vty_out(vty, "MPLS l3vpn routing Tables: All%s", VTY_NEWLINE);
                        vty_out(vty, "%s", VTY_NEWLINE);
                        vty_out(vty, "%-5s" , "Vpn");
                        vty_out(vty, "%-19s", "Destination/Mask");
                        vty_out(vty, "%-16s", "Nexthop");
                        vty_out(vty, "%-16s", "I/O label");
                        vty_out(vty, "%-6s", "status");
                        vty_out(vty, "%s", VTY_NEWLINE);
                    }

                    mpls_l3vpn_show_route_all(vty, pl3vpn, pvroute, pnhp);
                }
            }
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 l3vpn 节点配置命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:17:05
 * @note       :
 */
DEFUN(show_this_l3vpn,
    show_this_l3vpn_cmd,
    "show this",
    SHOW_STR
    "This\n")
{
    struct l3vpn_entry *pl3vpn = (struct l3vpn_entry *)vty->index;

    vty_out(vty, "#%s", VTY_NEWLINE);
    vty_out(vty, " #%s", VTY_NEWLINE);

    mpls_l3vpn_config_write_process(vty, pl3vpn);

    vty_out(vty, "#%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 l3vpn 节点配置
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:17:50
 * @note       :
 */
int mpls_l3vpn_config_show_this(struct vty *vty)
{
    struct l3vpn_entry *pl3vpn  = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;

    HASH_BUCKET_LOOP(pbucket, cursor, l3vpn_table)
    {
        pl3vpn = (struct l3vpn_entry *)pbucket->data;
        if (NULL == pl3vpn)
        {
            continue;
        }

        mpls_l3vpn_config_write_process(vty, pl3vpn);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:18:24
 * @note       :
 */
static int mpls_l3vpn_config_write(struct vty *vty)
{
    struct l3vpn_entry *pl3vpn  = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;

    vty_out(vty, "mpls%s", VTY_NEWLINE);

    HASH_BUCKET_LOOP(pbucket, cursor, l3vpn_table)
    {
        pl3vpn = (struct l3vpn_entry *)pbucket->data;
        if (NULL == pl3vpn)
        {
            continue;
        }

        mpls_l3vpn_config_write_process(vty, pl3vpn);
    }

    return ERRNO_SUCCESS;
}

static int mpls_l3vpn_config_write_factory(struct vty *vty)
{
    if(get_ospf_dcn_status(MODULE_ID_MPLS))
    {
        vty_out(vty, "mpls%s", VTY_NEWLINE);
        
        vty_out(vty, " l3vpn instance %u%s", 127, VTY_NEWLINE);

        vty_out(vty, "  name %s%s", "vpn_dcn", VTY_NEWLINE);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 命令行初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 10:13:29
 * @note       :
 */
void mpls_l3vpn_cmd_init(void)
{
    install_node(&l3vpn_node, mpls_l3vpn_config_write);
    install_node_factory(&l3vpn_node, mpls_l3vpn_config_write_factory);
    install_default(L3VPN_NODE);

    install_element (CONFIG_NODE, &mpls_l3vpn_show_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &mpls_l3vpn_show_route_cmd, CMD_LOCAL);

    install_element (MPLS_NODE, &mpls_l3vpn_instance_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &no_mpls_l3vpn_instance_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &mpls_l3vpn_show_cmd, CMD_LOCAL);
    install_element (MPLS_NODE, &mpls_l3vpn_show_route_cmd, CMD_LOCAL);

    install_element (L3VPN_NODE, &show_this_l3vpn_cmd, CMD_LOCAL);
    install_element (L3VPN_NODE, &mpls_l3vpn_rd_cmd, CMD_SYNC);
    install_element (L3VPN_NODE, &mpls_l3vpn_name_cmd, CMD_SYNC);
    install_element (L3VPN_NODE, &mpls_l3vpn_target_cmd, CMD_SYNC);
    install_element (L3VPN_NODE, &no_mpls_l3vpn_target_cmd, CMD_SYNC);
    install_element (L3VPN_NODE, &no_mpls_l3vpn_target_all_cmd, CMD_SYNC);
    install_element (L3VPN_NODE, &mpls_l3vpn_apply_label_cmd, CMD_SYNC);
    install_element (L3VPN_NODE, &no_mpls_l3vpn_apply_label_cmd, CMD_SYNC);
    install_element (L3VPN_NODE, &mpls_l3vpn_show_cmd, CMD_LOCAL);
    install_element (L3VPN_NODE, &mpls_l3vpn_show_route_cmd, CMD_LOCAL);

    return;
}


