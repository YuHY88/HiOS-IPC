/**
 * @file      : h3croute_if.c
 * @brief     :
 * @details   :
 * @author    : wusong
 * @date      : 2018年2月27日 8:47:27
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/memory.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/inet_ip.h>
#include <lib/log.h>
#include <lib/route_com.h>
#include <lib/mpls_common.h>
#include <lib/errcode.h>
#include "route_register.h"
#include "route_cmd.h"
#include "route_main.h"
#include "route_if.h"
#include "route_static.h"
#include "arp_static.h"
#include "ndp_static.h"


extern int route_if_get_intf_mode(struct vty *vty);
extern int route_if_add_slave_ip(struct route_if *pif);
extern int route_if_delete_slave_ip(struct route_if *pif);

/**
 * @brief      : 显示接口状态和数量
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 8:51:06
 * @note       :
 */
static void h3c_route_if_num_show(struct vty *vty)
{
    struct hash_bucket *pbucket    = NULL;
    struct route_if    *pif        = NULL;
    struct route_if    *pif_lookup = NULL;
    uint32_t phy_up_num    = 0;
    uint32_t phy_down_num  = 0;
    uint32_t pro_up_num    = 0;
    uint32_t pro_down_num  = 0;
    int      cursor        = 0;

    HASH_BUCKET_LOOP(pbucket, cursor, route_if_table)
    {
        pif = pbucket->data;
        if (NULL == pif)
        {
            continue;
        }

        if ((!ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
            && (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
            && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
            && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
            && (pif->intf.vpn == 0)
            && (pif->urpf == URPF_TYPE_INVALID)
            && (IP_TYPE_INVALID == pif->intf.ipv4_flag)
            && (IP_TYPE_INVALID == pif->intf.ipv6_flag))
        {
            continue;
        }

        if (LINK_UP == pif->down_flag)
        {
            phy_up_num++;

            if ((ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
                || (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr)))
            {
                pro_up_num++;
            }
            else
            {
                if (0 != pif->intf.unnumbered_if)
                {
                    pif_lookup = route_if_lookup(pif->intf.unnumbered_if);
                    if ((NULL != pif_lookup) && ROUTE_IF_IP_EXIST(pif_lookup->intf.ipv4[0].addr))
                    {
                        pro_up_num++;
                    }
                    else
                    {
                        pro_down_num++;
                    }
                }
                else
                {
                    pro_down_num++;
                }
            }
        }
        else
        {
            phy_down_num++;
            pro_down_num++;
        }
    }

    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "The number of interface that is UP in Physical is %u%s", phy_up_num, VTY_NEWLINE);
    vty_out(vty, "The number of interface that is DOWN in Physical is %u%s", phy_down_num, VTY_NEWLINE);
    vty_out(vty, "The number of interface that is UP in Protocol is %u%s", pro_up_num, VTY_NEWLINE);
    vty_out(vty, "The number of interface that is DOWN in Protocol is %u%s", pro_down_num, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);

    return;
}


/**
 * @brief      : 接口 IP 地址配置命令（IPv4）
 * @param[in ] : A.B.C.D/M - 点分十进制 IP 地址和掩码长度
 * @param[in ] : slave     - 配置 IP 地址为从 IP
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:17:16
 * @note       : 中括号 '[]' 作用为可选可不选
 * @note       : 不选时配置主 IP 地址，选中时配置从 IP 地址，配置从地址时主地址必须存在
 */
DEFUN(h3c_ip_addr_config,
    h3c_ip_addr_config_cmd,
    "ip address A.B.C.D (M | A.B.C.D) [sub]",
    ROUTE_CLI_INFO)
{
    struct route_if *pif_lookup = NULL;
    struct route_if lif;
    struct prefix_ipv4 ip_addr;
    struct prefix_ipv4 ip_mask;
    int ret;

    memset(&lif, 0, sizeof(struct route_if));
    lif.ifindex        = (uint32_t)vty->index;
    lif.intf.ipv4_flag = IP_TYPE_STATIC;

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    pif_lookup = route_if_lookup((uint32_t)vty->index);

    if (NULL == argv[2])
    {
        /* DHCP 使能不允许配置静态接口主 ip */
        if ((NULL != pif_lookup) && (pif_lookup->intf.ipv4_flag == IP_TYPE_DHCP))
        {
            vty_error_out(vty, "DHCP is enabled. Can't configure IP.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        /* 接口借用 ip 不允许配置静态 ip */
        if ((NULL != pif_lookup) && (pif_lookup->intf.ipv4_flag == IP_TYPE_UNNUMBERED))
        {
            vty_error_out(vty, "Internet Address is unnumbered. Can't configure IP.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else
    {
        /* 配置从IP必须配置接口主IP */
        if ((NULL == pif_lookup) || (!ROUTE_IF_IP_EXIST(pif_lookup->intf.ipv4[0].addr)))
        {
            vty_error_out(vty, "Please configure the primary address.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
 
    if(NULL != argv[0])
    {
        inet_aton(argv[0], &ip_addr.prefix);
    }
    
    if(NULL != argv[1])
    {
        if(NULL != strstr(argv[1], "."))
        {
            inet_aton(argv[1], &ip_mask.prefix);
            ip_addr.prefixlen= ip_masklen(ip_mask.prefix);
        }
        else
        {
            ip_addr.prefixlen = atoi(argv[1]);
        }
    }

    if ((ip_addr.prefixlen > 32) || (ip_addr.prefixlen < 0))
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }


    /* loopback 接口只允许配置 32 位掩码 IP 地址 */
    if ((IFM_TYPE_IS_LOOPBCK(lif.ifindex)) && (ip_addr.prefixlen != 32))
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ip_addr.prefix.s_addr = ntohl(ip_addr.prefix.s_addr);

    /* 检测 ip 合法性 */
    if ((ip_addr.prefixlen == 32) || (ip_addr.prefixlen == 31))
    {
        ret = inet_valid_network(ip_addr.prefix.s_addr);
        if ((!ret) || (ip_addr.prefix.s_addr == 0))
        {
            vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        /* 掩码长度为 31 时，只有两个 IP 地址可用，打印提示 */
        if (ip_addr.prefixlen == 31)
        {
            vty_warning_out(vty, "Please be cautious when using the IP address "
                            "with 31-bit mask on non-P2P interfaces.%s", VTY_NEWLINE);
        }
    }
    else
    {
        ret = inet_valid_ipv4(ip_addr.prefix.s_addr);
        if (!ret)
        {
            vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        ret = route_valid_ip_mask(htonl(ip_addr.prefix.s_addr), ip_addr.prefixlen);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    if (NULL == argv[2])
    {
        lif.intf.ipv4[0].addr      = ip_addr.prefix.s_addr;
        lif.intf.ipv4[0].prefixlen = ip_addr.prefixlen;
    }
    else
    {
        lif.intf.ipv4[1].addr      = ip_addr.prefix.s_addr;
        lif.intf.ipv4[1].prefixlen = ip_addr.prefixlen;
    }

    /* 检查 IP 地址是否冲突 */
    if (NULL != pif_lookup)
    {
        lif.intf.vpn = pif_lookup->intf.vpn;
    }

    ret = route_if_check_ip_conflict(&lif);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL == argv[2])
    {
        ret = route_if_add_ip(&lif);
        if (ERRNO_SUCCESS != ret)
        {
            return CMD_WARNING;
        }
    }
    else
    {
        ret = route_if_add_slave_ip(&lif);
        if (ERRNO_SUCCESS != ret)
        {
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 华三接口 IP 地址删除命令（IPv4）
 * @param[in ] : sub - 删除 IP 地址为从 IP
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : sunjl
 * @date       : 2018年7月30日 15:36:57
 * @note       : 删除主 IP 地址时，从 IP 地址一并清除
 */
DEFUN(undo_h3c_ip_addr_config,
    undo_h3c_ip_addr_config_cmd,
    "undo ip address [sub]",
    ROUTE_CLI_INFO)
{
    struct route_if *pif = NULL;
    int ret;

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    pif = route_if_lookup((uint32_t)vty->index);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if (NULL != argv[0])
    {
        /* 删除接口从 IP 地址 */
        if (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
        {
            return CMD_SUCCESS;
        }

        route_if_delete_slave_ip(pif);

        return CMD_SUCCESS;
    }
    else
    {
        if (pif->intf.ipv4_flag != IP_TYPE_STATIC)
        {
            return CMD_SUCCESS;
        }
    }

    /* 删除接口主 IP 地址 */
    if (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr)
        && (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
        && (pif->intf.ipv4_flag == IP_TYPE_INVALID))
    {
        return CMD_SUCCESS;
    }

    route_if_delete_ip(pif);

    return CMD_SUCCESS;
}


/**
 * @brief      : 华三接口 IP 地址删除命令,需指定ip及掩码（IPv4）
 * @param[in ] : sub - 删除 IP 地址为从 IP
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : sunjl
 * @date       : 2018年7月31日 09:36:57
 * @note       : 删除主 IP 地址时，从 IP 地址一并清除
 */
DEFUN(undo_h3c_ip_addr_config_ip,
    undo_h3c_ip_addr_config_ip_cmd,
    "undo ip address A.B.C.D (M | A.B.C.D)[sub]",
    ROUTE_CLI_INFO)
{
    struct route_if *pif = NULL;
    struct prefix_ipv4 ip_addr;
    struct prefix_ipv4 ip_mask;
    int ret;

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    pif = route_if_lookup((uint32_t)vty->index);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    memset(&ip_addr, 0, sizeof(struct prefix_ipv4));
    memset(&ip_mask, 0, sizeof(struct prefix_ipv4));
    
    if(NULL != argv[0])
    {
        inet_aton(argv[0], &ip_addr.prefix);
    }
    
    if(NULL != argv[1])
    {
        if(NULL != strstr(argv[1], "."))
        {
            inet_aton(argv[1], &ip_mask.prefix);
            ip_addr.prefixlen= ip_masklen(ip_mask.prefix);
        }
        else
        {
            ip_addr.prefixlen = atoi(argv[1]);
        }
    }

    if ((ip_addr.prefixlen > 32) || (ip_addr.prefixlen < 0))
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ((pif->intf.ipv4[0].prefixlen == ip_addr.prefixlen) &&
        (pif->intf.ipv4[0].addr == ntohl(ip_addr.prefix.s_addr)))
    {
        if (NULL != argv[2])
        {
            vty_error_out(vty, "Address type not match!%s", VTY_NEWLINE);
            return CMD_SUCCESS;
        }

        if (pif->intf.ipv4_flag != IP_TYPE_STATIC)
        {
            return CMD_SUCCESS;
        }
    
        /* 删除接口主 IP 地址 */
        if (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr)
            && (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
            && (pif->intf.ipv4_flag == IP_TYPE_INVALID))
        {
            return CMD_SUCCESS;
        }

        route_if_delete_ip(pif);
        
        return CMD_SUCCESS;
    }
    else if ((pif->intf.ipv4[1].prefixlen == ip_addr.prefixlen) &&
        (pif->intf.ipv4[1].addr == ntohl(ip_addr.prefix.s_addr)))
    {
        if (NULL == argv[2])
        {
            vty_error_out(vty, "Address type not match!%s", VTY_NEWLINE);
            return CMD_SUCCESS;
        }

        /* 删除接口从 IP 地址 */
        if (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
        {
            return CMD_SUCCESS;
        }

        route_if_delete_slave_ip(pif);

        return CMD_SUCCESS;
    }
    else
    {
        //vty_error_out(vty, "ip:0x%x mask:%d slave_ip:0x%x slave_ip_mask:%x%s", 
        //              pif->intf.ipv4[0].addr, pif->intf.ipv4[0].prefixlen,pif->intf.ipv4[1].addr, pif->intf.ipv4[1].prefixlen,VTY_NEWLINE);
        return CMD_SUCCESS;
    }
}



/* delete the no ip add cmd for repeate with the huahuan cmd */

# if 0
/**
 * @brief      : 接口 IP 地址删除命令（IPv4）
 * @param[in ] : slave - 删除 IP 地址为从 IP
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:36:57
 * @note       : 删除主 IP 地址时，从 IP 地址一并清除
 */
DEFUN(no_ip_addr,
    no_ip_addr_cmd,
    "no ip address sub",
    ROUTE_CLI_INFO)
{
    struct route_if *pif = NULL;
    int ret;

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    pif = route_if_lookup((uint32_t)vty->index);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if (NULL != argv[0])
    {
        /* 删除接口从 IP 地址 */
        if (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
        {
            return CMD_SUCCESS;
        }

        route_if_delete_slave_ip(pif);

        return CMD_SUCCESS;
    }
    else
    {
        if (pif->intf.ipv4_flag != IP_TYPE_STATIC)
        {
            return CMD_SUCCESS;
        }
    }

    /* 删除接口主 IP 地址 */
    if (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr)
        && (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
        && (pif->intf.ipv4_flag == IP_TYPE_INVALID))
    {
        return CMD_SUCCESS;
    }

    route_if_delete_ip(pif);

    return CMD_SUCCESS;
}

#endif
#if 0


/**
 * @brief      : 接口 IP 地址自动获取固话删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:42:28
 * @note       :
 */
DEFUN(no_ip_addr_dhcp_save,
    no_ip_addr_dhcp_save_cmd,
    "no ip address dhcp save",
    ROUTE_CLI_INFO)
{
    struct route_if *pif = NULL;
    int ret;

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_SUCCESS;
    }

    pif = route_if_lookup((uint32_t)vty->index);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if ((pif->intf.ipv4_flag != IP_TYPE_DHCP))
    {
        return CMD_SUCCESS;
    }

    ret = ipc_send_common1(NULL, 0, 1, MODULE_ID_DHCP, MODULE_ID_ROUTE,
                            IPC_TYPE_L3IF, IFNET_EVENT_IP_DELETE,
                            IPC_OPCODE_UPDATE, (uint32_t)vty->index);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    pif->dhcp_save_flag = DISABLE;

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口借用 IP 地址删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:43:18
 * @note       :
 */
DEFUN(no_ip_addr_unnumbered,
    no_ip_addr_unnumbered_cmd,
    "no ip address unnumbered",
    ROUTE_CLI_INFO)
{
    struct route_if *pif       = NULL;
    struct route_if *pif_nunum = NULL;
    struct ifm_event event;
    int ret;

    memset(&event, 0, sizeof(struct ifm_event));

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    pif = route_if_lookup((uint32_t)vty->index);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if (pif->intf.ipv4_flag != IP_TYPE_UNNUMBERED)
    {
        return CMD_SUCCESS;
    }

    /*借用ip 初始化arp 控制结构*/
    ROUTE_ARPIF_CFG_CLR(pif);

    route_if_send_to_ftm(pif, IPC_OPCODE_DELETE, L3IF_INFO_IPV4);

    pif_nunum = route_if_lookup(pif->intf.unnumbered_if);
    if (NULL != pif_nunum)
    {
        event.ipaddr.type      = INET_FAMILY_IPV4;
        event.ipaddr.addr.ipv4 = pif_nunum->intf.ipv4[0].addr;
        event.ipaddr.prefixlen = pif_nunum->intf.ipv4[0].prefixlen;

        listnode_delete(&pif_nunum->unnumbered_list, (void *)pif->ifindex);
    }

    /* 通知其他模块接口借用 IP 删除 */
    event.event   = IFNET_EVENT_IP_DELETE;
    event.ifindex = pif->ifindex;
    event.up_flag = pif->down_flag;
    event.mode    = IFNET_MODE_L3;
    event.ipflag  = IP_TYPE_UNNUMBERED;
    event.vpn     = pif->intf.vpn;

    l3if_event_notify(event.event, &event);

    ROUTE_IF_VAL_RESET(pif->intf.unnumbered_if);
    ROUTE_IF_VAL_RESET(pif->intf.ipv4_flag);

    return CMD_SUCCESS;
}

#endif

#if 0
/**
 * @brief      : 接口下配置显示命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:05:02
 * @note       :
 */
DEFUN(h3c_display_ip_addr,
    h3c_display_ip_addr_cmd,
    "display ip address",
    ROUTE_CLI_INFO)
{
    struct route_if *pif        = NULL;
    struct route_if *pif_lookup = NULL;
    char ip_buf[INET_ADDRSTRLEN]           = "";
    char ip_slave[INET_ADDRSTRLEN]         = "";
    char ipv6_buf[INET6_ADDRSTRLEN]        = "";
    char ipv6_slave[INET6_ADDRSTRLEN]      = "";
    char ifname[IFNET_NAMESIZE]            = "";
    char unnumbered_ifname[IFNET_NAMESIZE] = "";
    uint8_t prefixlen = 0;
    int ret;

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    pif = route_if_lookup((uint32_t)vty->index);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if ((!ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
        && (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
        && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
        && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
        && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
        && (pif->intf.vpn == 0)
        && (pif->urpf == URPF_TYPE_INVALID)
        && (pif->intf.ipv4_flag == IP_TYPE_INVALID)
        && (pif->intf.ipv6_flag == IP_TYPE_INVALID))
    {
        return CMD_SUCCESS;
    }

    ifm_get_name_by_ifindex(pif->ifindex, ifname);
    inet_ipv4tostr(pif->intf.ipv4[1].addr, ip_slave);

    vty_out(vty, "%-16s: %s%s", "interface", ifname, VTY_NEWLINE);

    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
    {
        inet_ipv4tostr(pif->intf.ipv4[0].addr, ip_buf);

        vty_out(vty, "%-16s: %s/%d %s%s", "ipv4 address", ip_buf, pif->intf.ipv4[0].prefixlen,
                        pif->intf.ipv4_flag == IP_TYPE_STATIC ?
                        "static":pif->intf.ipv4_flag == IP_TYPE_DHCP ?
                        "dhcp":"invalid", VTY_NEWLINE);
    }
    else
    {
        if (pif->intf.ipv4_flag == IP_TYPE_UNNUMBERED)
        {
            pif_lookup = route_if_lookup(pif->intf.unnumbered_if);
            if (pif_lookup != NULL)
            {
                inet_ipv4tostr(pif_lookup->intf.ipv4[0].addr, ip_buf);

                prefixlen = pif_lookup->intf.ipv4[0].prefixlen;
            }
            else
            {
                inet_ipv4tostr(0, ip_buf);

                prefixlen = 0;
            }

            ifm_get_name_by_ifindex(pif->intf.unnumbered_if, unnumbered_ifname);

            vty_out(vty, "%-16s: %s/%d %s, using address of %s.%s", "ipv4 address",
                        ip_buf, prefixlen, "unnumbered", unnumbered_ifname, VTY_NEWLINE);
        }
        else
        {
            if (IP_TYPE_DHCP == pif->intf.ipv4_flag)
            {
                vty_out(vty, "%-16s: %s %s%s", "ipv4 address", "--", "dhcp", VTY_NEWLINE);
            }
            else if (IP_TYPE_DHCP_ZERO == pif->intf.ipv4_flag)
            {
                vty_out(vty, "%-16s: %s %s%s", "ipv4 address", "--", "dhcp zero", VTY_NEWLINE);
            }
        }
    }

    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
    {
        vty_out(vty, "%-16s: %s/%d %s%s", "ipv4 address", ip_slave,
                        pif->intf.ipv4[1].prefixlen, "slave", VTY_NEWLINE);
    }

    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
    {
        inet_ntop(AF_INET6, pif->intf.ipv6[0].addr, ipv6_buf, INET6_ADDRSTRLEN);

        vty_out(vty, "%-16s: %s/%d %s%s", "ipv6 address", ipv6_buf,
                        pif->intf.ipv6[0].prefixlen,
                        pif->intf.ipv6_flag == IP_TYPE_STATIC ?
                        "static":pif->intf.ipv6_flag == IP_TYPE_DHCP ?
                        "dhcp":"invalid", VTY_NEWLINE);
    }
    else
    {
        if (IP_TYPE_DHCP == pif->intf.ipv6_flag)
        {
            vty_out(vty, "%-16s: %s %s%s", "ipv6 address", "--", "dhcp", VTY_NEWLINE);
        }
    }

    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
    {
        inet_ntop(AF_INET6, pif->intf.ipv6[1].addr, ipv6_slave, INET6_ADDRSTRLEN);

        vty_out(vty, "%-16s: %s/%d %s%s", "ipv6 address", ipv6_slave,
                        pif->intf.ipv6[1].prefixlen, "slave", VTY_NEWLINE);
    }

    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
    {
        memset(ipv6_slave, 0, sizeof(ipv6_slave));
        inet_ntop(AF_INET6, pif->intf.ipv6_link_local.addr, ipv6_slave, INET6_ADDRSTRLEN);

        vty_out(vty, "%-16s: %s%s", "ipv6 link-local", ipv6_slave, VTY_NEWLINE);
    }

    if (0 != pif->intf.vpn)
    {
        vty_out(vty, "%-16s: %d%s", "vpn", pif->intf.vpn, VTY_NEWLINE);
    }

    if (URPF_TYPE_INVALID != pif->urpf)
    {
        vty_out(vty, "%-16s: %s%s", "urpf", pif->urpf == URPF_TYPE_LOOSE ?
                    "loose":"strict", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}
#endif



/**
 * @brief      : 显示接口详细信息
 * @param[in ] : vty - vty 全局结构
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 9:04:58
 * @note       :
 */
static void h3c_ip_interface_verbose_show(struct vty *vty, struct route_if *pif)
{
    char ip[INET_ADDRSTRLEN]               = "";
    char ip_buf[INET_ADDRSTRLEN]           = "";
    char ip_slave[INET_ADDRSTRLEN]         = "";
    char ipv6_buf[INET6_ADDRSTRLEN]        = "";
    char ipv6_slave[INET6_ADDRSTRLEN]      = "";
    char ifname[IFNET_NAMESIZE]            = "";
    char unnumbered_ifname[IFNET_NAMESIZE] = "";
    struct route_if *pif_lookup = NULL;
    uint8_t prefixlen = 0;

    if (NULL == pif)
    {
        return;
    }

    if ((!ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
        && (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
        && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
        && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
        && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
        && (pif->intf.vpn == 0)
        && (pif->urpf == URPF_TYPE_INVALID)
        && (IP_TYPE_INVALID == pif->intf.ipv4_flag)
        && (IP_TYPE_INVALID == pif->intf.ipv6_flag))
    {
        return;
    }

    ifm_get_name_by_ifindex(pif->ifindex, ifname);

    vty_out(vty, "interface %s current state : %s%s", ifname,
            pif->down_flag == IFNET_NO_SHUTDOWN ? "UP":"DOWN", VTY_NEWLINE);

    if (LINK_UP == pif->down_flag)
    {
        if ((ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
            || (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr)))
        {
            vty_out(vty, "Line protocol current state : %s%s", "UP", VTY_NEWLINE);
        }
        else
        {
            if (0 != pif->intf.unnumbered_if)
            {
                pif_lookup = route_if_lookup(pif->intf.unnumbered_if);
                if ((NULL != pif_lookup) && ROUTE_IF_IP_EXIST(pif_lookup->intf.ipv4[0].addr))
                {
                    vty_out(vty, "Line protocol current state : %s%s", "UP", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "Line protocol current state : %s%s", "DOWN", VTY_NEWLINE);
                }
            }
            else
            {
                vty_out(vty, "Line protocol current state : %s%s", "DOWN", VTY_NEWLINE);
            }
        }
    }
    else
    {
        vty_out(vty, "Line protocol current state : %s%s", "DOWN", VTY_NEWLINE);
    }

    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
    {
        inet_ipv4tostr(pif->intf.ipv4[0].addr, ip);

        vty_out(vty, "%-16s: %s/%d %s%s", "ipv4 address", ip, pif->intf.ipv4[0].prefixlen,
                        pif->intf.ipv4_flag == IP_TYPE_DHCP ? "dhcp":
                        pif->intf.ipv4_flag == IP_TYPE_STATIC ? "static":
                        pif->intf.ipv4_flag == IP_TYPE_DHCP_ZERO ? "dhcp zero":
                        "invalid", VTY_NEWLINE);
    }
    else
    {
        if (pif->intf.ipv4_flag == IP_TYPE_UNNUMBERED)
        {
            pif_lookup = route_if_lookup(pif->intf.unnumbered_if);
            if (pif_lookup != NULL)
            {
                inet_ipv4tostr(pif_lookup->intf.ipv4[0].addr, ip_buf);

                prefixlen = pif_lookup->intf.ipv4[0].prefixlen;
            }
            else
            {
                inet_ipv4tostr(0, ip_buf);
                prefixlen = 0;
            }

            ifm_get_name_by_ifindex(pif->intf.unnumbered_if, unnumbered_ifname);

            vty_out(vty, "%-16s: %s/%d %s, using interface of %s.%s", "ipv4 address",
                            ip_buf, prefixlen, "unnumbered", unnumbered_ifname, VTY_NEWLINE);
        }
        else
        {
            if (IP_TYPE_DHCP == pif->intf.ipv4_flag)
            {
                vty_out(vty, "%-16s: %s%s", "ipv4 address", "dhcp", VTY_NEWLINE);
            }
            else if (IP_TYPE_DHCP_ZERO == pif->intf.ipv4_flag)
            {
                vty_out(vty, "%-16s: %s%s", "ipv4 address", "dhcp zero", VTY_NEWLINE);
            }
        }
    }

    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
    {
        inet_ipv4tostr(pif->intf.ipv4[1].addr, ip_slave);

        vty_out(vty, "%-16s: %s/%d %s%s", "ipv4 address", ip_slave, pif->intf.ipv4[1].prefixlen,
                        "slave", VTY_NEWLINE);
    }

    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
    {
        inet_ntop(AF_INET6, pif->intf.ipv6[0].addr, ipv6_buf, INET6_ADDRSTRLEN);

        vty_out(vty, "%-16s: %s/%d %s%s", "ipv6 address", ipv6_buf,
                        pif->intf.ipv6[0].prefixlen,
                        pif->intf.ipv6_flag == IP_TYPE_DHCP ? "dhcp":
                        pif->intf.ipv6_flag == IP_TYPE_STATIC ? "static":
                        "invalid", VTY_NEWLINE);
    }

    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
    {
        inet_ntop(AF_INET6, pif->intf.ipv6[1].addr, ipv6_slave, INET6_ADDRSTRLEN);

        vty_out(vty, "%-16s: %s/%d %s%s", "ipv6 address", ipv6_slave,
                        pif->intf.ipv6[1].prefixlen, "slave", VTY_NEWLINE);
    }

    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
    {
        memset(ipv6_slave, 0, sizeof(ipv6_slave));
        inet_ntop(AF_INET6, pif->intf.ipv6_link_local.addr, ipv6_slave, INET6_ADDRSTRLEN);

        vty_out(vty, "%-16s: %s%s", "ipv6 link-local", ipv6_slave, VTY_NEWLINE);
    }

    if (0 != pif->intf.vpn)
    {
        vty_out(vty, "%-16s: %d%s", "vpn", pif->intf.vpn, VTY_NEWLINE);
    }

    if (URPF_TYPE_INVALID != pif->urpf)
    {
        vty_out(vty, "%-16s: %s%s", "urpf", pif->urpf == URPF_TYPE_LOOSE ?
                    "loose" : "strict", VTY_NEWLINE);
    }

    vty_out(vty, "%s", VTY_NEWLINE);

    return;
}


/**
 * @brief      : 显示接口简要信息
 * @param[in ] : vty - vty 全局结构
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 8:58:29
 * @note       :
 */
static void h3c_ip_interface_brief_show(struct vty *vty, struct route_if *pif)
{
    char ip[INET_ADDRSTRLEN]     = "";
    char ip_buf[INET_ADDRSTRLEN] = "";
    char ifname[IFNET_NAMESIZE]  = "";
    struct route_if *pif_lookup  = NULL;

    if (NULL == pif)
    {
        return;
    }

    if ((!ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
        && (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
        && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
        && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
        && (pif->intf.vpn == 0)
        && (pif->urpf == URPF_TYPE_INVALID)
        && (IP_TYPE_INVALID == pif->intf.ipv4_flag)
        && (IP_TYPE_INVALID == pif->intf.ipv6_flag))
    {
        return;
    }

    ifm_get_name_by_ifindex(pif->ifindex, ifname);
    inet_ipv4tostr(pif->intf.ipv4[0].addr, ip);

    vty_out(vty, "%-31s", ifname);
    vty_out(vty, "%-10s", pif->down_flag == IFNET_NO_SHUTDOWN ? "up":"down");

    if (LINK_UP == pif->down_flag)
    {
        if ((ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
            || (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr)))
        {
            vty_out(vty, "%-10s", "up");
        }
        else
        {
            if (0 != pif->intf.unnumbered_if)
            {
                pif_lookup = route_if_lookup(pif->intf.unnumbered_if);
                if ((NULL != pif_lookup) && ROUTE_IF_IP_EXIST(pif_lookup->intf.ipv4[0].addr))
                {
                    vty_out(vty, "%-10s", "up");
                }
                else
                {
                    vty_out(vty, "%-10s", "down");
                }
            }
            else
            {
                vty_out(vty, "%-10s", "down");
            }
        }
    }
    else
    {
        vty_out(vty, "%-10s", "down");
    }

    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
    {
        vty_out(vty, "%-19s", ip);
    }
    else
    {
        if (pif->intf.ipv4_flag == IP_TYPE_UNNUMBERED)
        {
            pif_lookup = route_if_lookup(pif->intf.unnumbered_if);
            if (pif_lookup != NULL)
            {
                inet_ipv4tostr(pif_lookup->intf.ipv4[0].addr, ip_buf);
            }
            else
            {
                inet_ipv4tostr(0, ip_buf);
            }

            vty_out(vty, "%-19s", ip_buf);
        }
        else
        {
            vty_out(vty, "%-19s", "--");
        }
    }

    vty_out(vty, "%s", VTY_NEWLINE);

    return;
}

/**
 * @brief      : 显示三层接口配置命令
 * @param[in ] : ethernet USP         - 出接口类型为 ethernet
 * @param[in ] : gigabitethernet USP  - 出接口类型为 gigabitethernet
 * @param[in ] : xgigabitethernet USP - 出接口类型为 xgigabitethernet
 * @param[in ] : tunnel USP           - 出接口类型为 tunnel
 * @param[in ] : loopback <0-128>     - 出接口类型为 loopback
 * @param[in ] : vlanif <1-4095>      - 出接口类型为 vlanif
 * @param[in ] : trunk TRUNK          - 出接口类型为 trunk
 * @param[in ] : brief                - 显示接口简要信息
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:09:57
 * @note       :
 */
DEFUN(h3c_display_ip_interface,
    h3c_display_ip_interface_cmd,
    "display ip interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP | "
    "tunnel USP | loopback <0-128> | vlanif <1-4094> | trunk TRUNK | brief}",
    ROUTE_CLI_INFO)
{
    struct route_if    *pif       = NULL;
    struct hash_bucket *pbucket   = NULL;
    const char         *pintf     = NULL;
    const char         *pintf_num = NULL;
    uint32_t ifindex = 0;
    int      num     = 0;
    int      cursor;

    /*
     * 指定显示的接口类型，最多可选一
     * argv[0]: Ethernet
     * argv[1]: Gigabitethernet
     * argv[2]: Xgigabitethernet
     * argv[3]: Tunnel
     * argv[4]: Loopback
     * argv[5]: Vlanif
     * argv[6]: Trunk
     */
    if ((NULL != argv[0]) || (NULL != argv[1]) || (NULL != argv[2]) ||
        (NULL != argv[3]) || (NULL != argv[4]) || (NULL != argv[5]) || (NULL != argv[6]))
    {
        if (NULL != argv[0])
        {
            pintf     = "ethernet";
            pintf_num = argv[0];
        }
		else if (NULL != argv[1])
        {
            pintf     = "gigabitethernet";
            pintf_num = argv[1];
        }
		else if (NULL != argv[2])
        {
            pintf     = "xgigabitethernet";
            pintf_num = argv[2];
        }
        else if (NULL != argv[3])
        {
            pintf     = "tunnel";
            pintf_num = argv[3];
        }
        else if (NULL != argv[4])
        {
            pintf     = "loopback";
            pintf_num = argv[4];
        }
        else if (NULL != argv[5])
        {
            pintf     = "vlanif";
            pintf_num = argv[5];
        }
        else if (NULL != argv[6])
        {
            pintf     = "trunk";
            pintf_num = argv[6];
        }

        if (NULL != pintf)
        {
            ifindex = ifm_get_ifindex_by_name(pintf, pintf_num);
            if (ifindex == 0)
            {
                vty_error_out(vty, "Specify the illegal interface.%s", VTY_NEWLINE);

                return CMD_WARNING;
            }
        }

        pif = route_if_lookup(ifindex);
        if (NULL != pif)
        {
            h3c_ip_interface_verbose_show(vty, pif);
        }
    }
    else
    {
        HASH_BUCKET_LOOP(pbucket, cursor, route_if_table)
        {
            pif = pbucket->data;
            if (NULL == pif)
            {
                continue;
            }

            if (NULL != argv[7])
            {
                if (++num == 1)
                {
                    h3c_route_if_num_show(vty);

                    vty_out(vty, "%-31s", "Interface");
                    vty_out(vty, "%-10s", "Physical");
                    vty_out(vty, "%-10s", "Protocol");
                    vty_out(vty, "%-19s", "IP Address");
                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                h3c_ip_interface_brief_show(vty, pif);
            }
            else
            {
                h3c_ip_interface_verbose_show(vty, pif);
            }
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 三层接口配置命令行初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:18:36
 * @note       :
 */
void h3c_route_if_cmd_init(void)
{
#if 1
 #define ROUTE_INSTALL_ELEMENT_L3IF(cmd, flag)\
        install_element (PHYSICAL_IF_NODE, (cmd), (flag));\
        install_element (PHYSICAL_SUBIF_NODE, (cmd), (flag));\
        install_element (LOOPBACK_IF_NODE, (cmd), (flag));\
        install_element (TUNNEL_IF_NODE, (cmd), (flag));\
        install_element (TRUNK_IF_NODE, (cmd), (flag));\
        install_element (TRUNK_SUBIF_NODE, (cmd), (flag));\
        install_element (VLANIF_NODE, (cmd), (flag));


#define H3C_ROUTE_IF_INSTALL_ELEMENT_SHOW(cmd, flag)\
            install_element (PHYSICAL_IF_NODE, (cmd), (flag));\
            install_element (PHYSICAL_SUBIF_NODE, (cmd), (flag));\
            install_element (LOOPBACK_IF_NODE, (cmd), (flag));\
            install_element (TUNNEL_IF_NODE, (cmd), (flag));\
            install_element (TRUNK_IF_NODE, (cmd), (flag));\
            install_element (TRUNK_SUBIF_NODE, (cmd), (flag));\
            install_element (VLANIF_NODE, (cmd), (flag));\
            install_element (ROUTE_NODE, (cmd), (flag));\
            install_element (CONFIG_NODE, (cmd), (flag));

        ROUTE_INSTALL_ELEMENT_L3IF(&h3c_ip_addr_config_cmd, CMD_SYNC);
        ROUTE_INSTALL_ELEMENT_L3IF(&undo_h3c_ip_addr_config_cmd, CMD_SYNC);
        ROUTE_INSTALL_ELEMENT_L3IF(&undo_h3c_ip_addr_config_ip_cmd, CMD_SYNC);
        H3C_ROUTE_IF_INSTALL_ELEMENT_SHOW(&h3c_display_ip_interface_cmd, CMD_LOCAL);
 #endif

}

