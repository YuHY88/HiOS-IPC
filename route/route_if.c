/**
 * @file      : route_if.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
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


/* route 模块接口信息哈希表 */
struct hash_table route_if_table;

/* 记录不同 vpn 实例生成的链路本地地址数量 */
static uint32_t link_local_num[L3VPN_SIZE+1] = {0};


/**
 * @brief      : 显示接口状态和数量
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 8:51:06
 * @note       :
 */
static void route_if_num_show(struct vty *vty)
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
 * @brief      : 显示接口简要信息
 * @param[in ] : vty - vty 全局结构
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 8:58:29
 * @note       :
 */
static void route_if_show_ip_interface_brief(struct vty *vty, struct route_if *pif)
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
 * @brief      : 显示接口详细信息
 * @param[in ] : vty - vty 全局结构
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 9:04:58
 * @note       :
 */
static void route_if_show_ip_interface(struct vty *vty, struct route_if *pif)
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
 * @brief      : 检查 IPv4 地址配置冲突
 * @param[in ] : pif_new - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 9:10:19
 * @note       :
 */
int route_if_check_ip_conflict(const struct route_if *pif_new)
{
    struct hash_bucket *pbucket = NULL;
    struct route_if    *pif     = NULL;
    struct route_if     lif;
    int cursor;
    int flag = 0;    // 1: 网段存在包含关系，0: 不存在包含关系
    int ret;

    ROUTE_LOG_DEBUG();

    if (NULL == pif_new)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, route_if_table)
    {
        memset(&lif, 0, sizeof(struct route_if));

        pif = (struct route_if *)pbucket->data;
        if (NULL == pif)
        {
            continue;
        }

        /* vpn 不同，无需检测 */
        if (pif_new->intf.vpn != pif->intf.vpn)
        {
            continue;
        }

        /* 接口 IP 不存在 */
        if ((!ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
            && (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr)))
        {
            continue;
        }

        /* 同一接口下更新 IP */
        if (pif_new->ifindex == pif->ifindex)
        {
            if (ROUTE_IF_IP_EXIST(pif_new->intf.ipv4[0].addr))      // 配置接口主 IP
            {
                /* 新添加的 IP 不能与接口原有的主 IP 相同 */
                if ((pif_new->intf.ipv4[0].addr == pif->intf.ipv4[0].addr)
                    && (pif_new->intf.ipv4[0].prefixlen == pif->intf.ipv4[0].prefixlen))
                {
                    return ERRNO_EXISTED_IP;
                }

                /* 从 IP 存在，主 IP 不能与从 IP 冲突 */
                if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
                {
                    /* 掩码长度相同，网段不能相同 */
                    if (pif_new->intf.ipv4[0].prefixlen == pif->intf.ipv4[1].prefixlen)
                    {
                        ret = ipv4_is_same_subnet(pif_new->intf.ipv4[0].addr,
                                                    pif->intf.ipv4[1].addr,
                                                    pif->intf.ipv4[1].prefixlen);
                        if (1 == ret)
                        {
                            return ERRNO_CONFLICT_IP;
                        }
                    }
                    else
                    {
                        /* IP 不能完全相同，忽略掩码 */
                        ret = ipv4_is_same_subnet(pif_new->intf.ipv4[0].addr,
                                                    pif->intf.ipv4[1].addr,
                                                    IPV4_MAX_PREFIXLEN);
                        if (1 == ret)
                        {
                            return ERRNO_CONFLICT_IP;
                        }

                        /* 网段存在包含关系 */
                        ret = ipv4_is_same_subnet(pif_new->intf.ipv4[0].addr,
                                                    pif->intf.ipv4[1].addr,
                                                    MIN(pif_new->intf.ipv4[0].prefixlen,
                                                    pif->intf.ipv4[1].prefixlen));
                        if (1 == ret)
                        {
                            flag = 1;
                        }
                    }
                }
            }
            else if (ROUTE_IF_IP_EXIST(pif_new->intf.ipv4[1].addr)) // 配置接口从 IP
            {
                /* 从 IP 存在，新添加的 IP 不能与接口原有的从 IP 相同 */
                if ((pif_new->intf.ipv4[1].addr == pif->intf.ipv4[1].addr)
                    && (pif_new->intf.ipv4[1].prefixlen == pif->intf.ipv4[1].prefixlen))
                {
                    return ERRNO_EXISTED_IP;
                }

                /* 不能与接口主 IP 冲突 */
                if (pif_new->intf.ipv4[1].prefixlen == pif->intf.ipv4[0].prefixlen)
                {
                    ret = ipv4_is_same_subnet(pif->intf.ipv4[0].addr,
                                                pif_new->intf.ipv4[1].addr,
                                                pif->intf.ipv4[0].prefixlen);
                    if (1 == ret)
                    {
                        return ERRNO_CONFLICT_IP;
                    }
                }
                else
                {
                    /* IP 不能完全相同，忽略掩码 */
                    ret = ipv4_is_same_subnet(pif_new->intf.ipv4[1].addr,
                                                pif->intf.ipv4[0].addr,
                                                IPV4_MAX_PREFIXLEN);
                    if (1 == ret)
                    {
                        return ERRNO_CONFLICT_IP;
                    }

                    /* 网段不允许存在包含关系 */
                    ret = ipv4_is_same_subnet(pif->intf.ipv4[0].addr,
                                                pif_new->intf.ipv4[1].addr,
                                                MIN(pif->intf.ipv4[0].prefixlen,
                                                pif_new->intf.ipv4[1].prefixlen));
                    if (1 == ret)
                    {
                        flag = 1;
                    }
                }
            }

            continue;
        }

        /* 不同接口 IP 检测 */
        if (ROUTE_IF_IP_EXIST(pif_new->intf.ipv4[0].addr))
        {
            lif.intf.ipv4[0] = pif_new->intf.ipv4[0];
        }
        else if (ROUTE_IF_IP_EXIST(pif_new->intf.ipv4[1].addr))
        {
            lif.intf.ipv4[0] = pif_new->intf.ipv4[1];
        }

        if (lif.intf.ipv4[0].prefixlen == pif->intf.ipv4[0].prefixlen)
        {
            /* 掩码长度相同，网段不能相同 */
            ret = ipv4_is_same_subnet(lif.intf.ipv4[0].addr, pif->intf.ipv4[0].addr,
                                        pif->intf.ipv4[0].prefixlen);
            if (1 == ret)
            {
                return ERRNO_CONFLICT_IP;
            }
        }
        else
        {
            /* IP 不能完全相同，忽略掩码 */
            ret = ipv4_is_same_subnet(lif.intf.ipv4[0].addr, pif->intf.ipv4[0].addr,
                                        IPV4_MAX_PREFIXLEN);
            if (1 == ret)
            {
                return ERRNO_CONFLICT_IP;
            }

            /* 网段不允许存在包含关系 */
            ret = ipv4_is_same_subnet(lif.intf.ipv4[0].addr, pif->intf.ipv4[0].addr,
                                        MIN(lif.intf.ipv4[0].prefixlen,
                                        pif->intf.ipv4[0].prefixlen));
            if (1 == ret)
            {
                flag = 1;
            }
        }

        /* 从 IP 存在，检测冲突 */
        if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
        {
            if (lif.intf.ipv4[0].prefixlen == pif->intf.ipv4[1].prefixlen)
            {
                /* 掩码长度相同，网段不能相同 */
                ret = ipv4_is_same_subnet(lif.intf.ipv4[0].addr, pif->intf.ipv4[1].addr,
                                            pif->intf.ipv4[1].prefixlen);
                if (1 == ret)
                {
                    return ERRNO_CONFLICT_IP;
                }
            }
            else
            {
                /* IP 不能完全相同，忽略掩码 */
                ret = ipv4_is_same_subnet(lif.intf.ipv4[0].addr, pif->intf.ipv4[1].addr,
                                            IPV4_MAX_PREFIXLEN);
                if (1 == ret)
                {
                    return ERRNO_CONFLICT_IP;
                }

                /* 网段不允许存在包含关系 */
                ret = ipv4_is_same_subnet(lif.intf.ipv4[0].addr, pif->intf.ipv4[1].addr,
                                            MIN(lif.intf.ipv4[0].prefixlen,
                                            pif->intf.ipv4[1].prefixlen));
                if (1 == ret)
                {
                    flag = 1;
                }
            }
        }
    }

    /* IP 地址网段存在包含关系，如果有冲突或已经存在先提示 */
    if (1 == flag)
    {
        return ERRNO_SUBNET_SIMILAR;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 检查 IPv6 地址配置冲突
 * @param[in ] : pif_new - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 9:20:18
 * @note       :
 */
int route_if_check_v6_ip_conflict(const struct route_if *pif_new)
{
    struct hash_bucket *pbucket = NULL;
    struct route_if    *pif     = NULL;
    struct route_if     lif;
    struct ipv6_addr    ipaddr1;
    struct ipv6_addr    ipaddr2;
    int cursor;
    int flag = 0;    // 1: 网段存在包含关系，0: 不存在包含关系
    int ret;

    ROUTE_LOG_DEBUG();

    if (NULL == pif_new)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, route_if_table)
    {
        memset(&lif, 0, sizeof(struct route_if));

        pif = (struct route_if *)pbucket->data;
        if (NULL == pif)
        {
            continue;
        }

        /* vpn 不同，无需检测 */
        if (pif_new->intf.vpn != pif->intf.vpn)
        {
            continue;
        }

        /* 接口 IP 不存在 */
        if ((!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
            && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr)))
        {
            continue;
        }

        /* 同一接口下更新 IP */
        if (pif_new->ifindex == pif->ifindex)
        {
            if (ROUTE_IF_IPV6_EXIST(pif_new->intf.ipv6[0].addr))  // 配置接口主 IP
            {
                /* 新添加的 IP 不能与接口原有的主 IP 相同 */
                if ((0 == memcmp(pif_new->intf.ipv6[0].addr, pif->intf.ipv6[0].addr, IPV6_ADDR_LEN))
                    && (pif_new->intf.ipv6[0].prefixlen == pif->intf.ipv6[0].prefixlen))
                {
                    return ERRNO_EXISTED_IP;
                }

                /* 从 IP 存在，主 IP 不能与从 IP 冲突 */
                if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
                {
                    IPV6_ADDR_COPY(ipaddr1.ipv6, pif_new->intf.ipv6[0].addr);
                    IPV6_ADDR_COPY(ipaddr2.ipv6, pif->intf.ipv6[1].addr);

                    /* 掩码长度相同，网段不能相同 */
                    if (pif_new->intf.ipv6[0].prefixlen == pif->intf.ipv6[1].prefixlen)
                    {
                        ret = ipv6_is_same_subnet(&ipaddr1, &ipaddr2, pif_new->intf.ipv6[0].prefixlen);
                        if (1 == ret)
                        {
                            return ERRNO_CONFLICT_IP;
                        }
                    }
                    else
                    {
                        /* IP 不能完全相同，忽略掩码 */
                        ret = ipv6_is_same_subnet(&ipaddr1, &ipaddr2, IPV6_MAX_PREFIXLEN);
                        if (1 == ret)
                        {
                            return ERRNO_CONFLICT_IP;
                        }

                        /* 网段存在包含关系 */
                        ret = ipv6_is_same_subnet(&ipaddr1, &ipaddr2, MIN(pif_new->intf.ipv6[0].prefixlen,
                                                    pif->intf.ipv6[1].prefixlen));
                        if (1 == ret)
                        {
                            flag = 1;
                        }
                    }
                }
            }
            else if (ROUTE_IF_IPV6_EXIST(pif_new->intf.ipv6[1].addr))  // 配置接口从 IP
            {
                /* 从 IP 存在并且新添加的 IP 不能与接口原有的从 IP 相同 */
                if ((0 == memcmp(pif_new->intf.ipv6[1].addr, pif->intf.ipv6[1].addr, IPV6_ADDR_LEN))
                    && (pif_new->intf.ipv6[1].prefixlen == pif->intf.ipv6[1].prefixlen))
                {

                    return ERRNO_EXISTED_IP;
                }

                IPV6_ADDR_COPY(ipaddr1.ipv6, pif_new->intf.ipv6[1].addr);
                IPV6_ADDR_COPY(ipaddr2.ipv6, pif->intf.ipv6[0].addr);

                /* 不能与接口主 IP 冲突 */
                if (pif_new->intf.ipv6[1].prefixlen == pif->intf.ipv6[0].prefixlen)
                {
                    ret = ipv6_is_same_subnet(&ipaddr1, &ipaddr2, pif->intf.ipv6[0].prefixlen);
                    if (1 == ret)
                    {
                        return ERRNO_CONFLICT_IP;
                    }
                }
                else
                {
                    /* IP 不能完全相同，忽略掩码 */
                    ret = ipv6_is_same_subnet(&ipaddr1,&ipaddr2, IPV6_MAX_PREFIXLEN);
                    if (1 == ret)
                    {
                        return ERRNO_CONFLICT_IP;
                    }

                    /* 网段不允许存在包含关系 */
                    ret = ipv6_is_same_subnet(&ipaddr1, &ipaddr2, MIN(pif->intf.ipv6[0].prefixlen,
                                                pif_new->intf.ipv6[1].prefixlen));
                    if (1 == ret)
                    {
                        flag = 1;
                    }
                }
            }

            continue;
        }

        /* 不同接口 IP 检测 */
        if (ROUTE_IF_IPV6_EXIST(pif_new->intf.ipv6[0].addr))
        {
            lif.intf.ipv6[0] = pif_new->intf.ipv6[0];
        }
        else if (ROUTE_IF_IPV6_EXIST(pif_new->intf.ipv6[1].addr))
        {
            lif.intf.ipv6[0] = pif_new->intf.ipv6[1];
        }

        IPV6_ADDR_COPY(ipaddr1.ipv6, lif.intf.ipv6[0].addr);
        IPV6_ADDR_COPY(ipaddr2.ipv6, pif->intf.ipv6[0].addr);

        if (lif.intf.ipv6[0].prefixlen == pif->intf.ipv6[0].prefixlen)
        {
            /* 掩码长度相同，网段不能相同 */
            ret = ipv6_is_same_subnet(&ipaddr1, &ipaddr2, pif->intf.ipv6[0].prefixlen);
            if (1 == ret)
            {
                return ERRNO_CONFLICT_IP;
            }
        }
        else
        {
            /* IP 不能完全相同，忽略掩码 */
            ret = ipv6_is_same_subnet(&ipaddr1, &ipaddr2, IPV6_MAX_PREFIXLEN);
            if (1 == ret)
            {
                return ERRNO_CONFLICT_IP;
            }

            /* 网段不允许存在包含关系 */
            ret = ipv6_is_same_subnet(&ipaddr1, &ipaddr2, MIN(lif.intf.ipv6[0].prefixlen,
                                        pif->intf.ipv6[0].prefixlen));
            if (1 == ret)
            {
                flag = 1;
            }
        }

        /* 从 IP 存在，检测冲突 */
        if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
        {
            IPV6_ADDR_COPY(ipaddr2.ipv6, pif->intf.ipv6[1].addr);

            if (lif.intf.ipv6[0].prefixlen == pif->intf.ipv6[1].prefixlen)
            {
                /* 掩码长度相同，网段不能相同 */
                ret = ipv6_is_same_subnet(&ipaddr1, &ipaddr2, pif->intf.ipv6[1].prefixlen);
                if (1 == ret)
                {
                    return ERRNO_CONFLICT_IP;
                }
            }
            else
            {
                /* IP 不能完全相同，忽略掩码 */
                ret = ipv6_is_same_subnet(&ipaddr1, &ipaddr2, IPV6_MAX_PREFIXLEN);
                if (1 == ret)
                {
                    return ERRNO_CONFLICT_IP;
                }

                /* 网段不允许存在包含关系 */
                ret = ipv6_is_same_subnet(&ipaddr1, &ipaddr2, MIN(lif.intf.ipv6[0].prefixlen,
                                            pif->intf.ipv6[1].prefixlen));
                if (1 == ret)
                {
                    flag = 1;
                }
            }
        }
    }

    /* IP 地址网段存在包含关系，如果有冲突或已经存在先提示 */
    if (1 == flag)
    {
        return ERRNO_SUBNET_SIMILAR;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 获取接口模式
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 如果模式为 L3 则返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年2月27日 9:25:49
 * @note       :
 */
int route_if_get_intf_mode(struct vty *vty)
{
    uint8_t pmode = 0;
    int     ret = 0;
    
    ret = ifm_get_mode((uint32_t)vty->index, MODULE_ID_ROUTE, &pmode);
    
    if (0 != ret)
    {
        vty_error_out(vty, "Get interface mode timeout.%s", VTY_NEWLINE);

        return ERRNO_FAIL;
    }
    else if (pmode != IFNET_MODE_L3)
    {
        vty_error_out(vty, "Please select mode l3.%s", VTY_NEWLINE);

        return ERRNO_FAIL;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 根据接口信息填充路由结构，生成直连路由（IPv4）
 * @param[in ] : proute - 活跃路由结构
 * @param[in ] : pif    - route 模块接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 9:29:17
 * @note       :
 */
static void route_if_set_route_entry(struct route_entry *proute,
                                                const struct route_if *pif)
{
    ROUTE_LOG_DEBUG();

    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
    {
        proute->prefix.addr.ipv4 = ipv4_network_addr(htonl(pif->intf.ipv4[0].addr),
                                                    proute->prefix.prefixlen);
        proute->prefix.addr.ipv4 = ntohl(proute->prefix.addr.ipv4);
    }
    else if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
    {
        proute->prefix.addr.ipv4 = ipv4_network_addr(htonl(pif->intf.ipv4[1].addr),
                                                    proute->prefix.prefixlen);
        proute->prefix.addr.ipv4 = ntohl(proute->prefix.addr.ipv4);
    }

    proute->vpn                 = pif->intf.vpn;
    proute->nhp_num             = 1;
    proute->prefix.type         = INET_FAMILY_IPV4;
    proute->nhp[0].nhp_type     = NHP_TYPE_CONNECT;
    proute->nhp[0].protocol     = ROUTE_PROTO_CONNECT;
    proute->nhp[0].distance     = ROUTE_METRIC_CONNECT;
    proute->nhp[0].ifindex      = pif->ifindex;
    proute->nhp[0].vpn          = pif->intf.vpn;
    proute->nhp[0].nexthop.type = INET_FAMILY_IPV4;
    proute->nhp[0].active       = ROUTE_STATUS_INACTIVE;
    proute->nhp[0].down_flag    = LINK_DOWN;
    proute->nhp[0].cost         = ROUTE_COST;
    proute->nhp[0].nexthop_connect.addr.ipv4 = 0;

    if (proute->prefix.prefixlen == IPV4_MAX_PREFIXLEN)
    {
        /* 主机路由上送 cpu */
        proute->nhp[0].action = NHP_ACTION_TOCPU;
    }
    else
    {
        /* 网段路由转发 */
        proute->nhp[0].action = NHP_ACTION_FORWARD;
    }

    return;
}


/**
 * @brief      : 根据接口信息生成直连路由（IPv4）
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 9:44:48
 * @note       :
 */
static int route_if_add_routing(const struct route_if *pif)
{
    struct route_entry route_net;
    struct route_entry route_host;
    int                ret;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memset(&route_net, 0, sizeof(struct route_entry));
    memset(&route_host, 0, sizeof(struct route_entry));

    /* 填充直连路由的掩码和下一跳 */
    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))      // 主 IP 路由
    {
        route_net.prefix.prefixlen          = pif->intf.ipv4[0].prefixlen;
        route_net.nhp[0].nexthop.addr.ipv4  = 0;

        route_host.prefix.prefixlen         = IPV4_MAX_PREFIXLEN;
        route_host.nhp[0].nexthop.addr.ipv4 = pif->intf.ipv4[0].addr;
    }
    else if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr)) // 从 IP 路由
    {
        route_net.prefix.prefixlen          = pif->intf.ipv4[1].prefixlen;
        route_net.nhp[0].nexthop.addr.ipv4  = 0;

        route_host.prefix.prefixlen         = IPV4_MAX_PREFIXLEN;
        route_host.nhp[0].nexthop.addr.ipv4 = pif->intf.ipv4[1].addr;
    }

    /* 添加直连主机路由 */
    route_if_set_route_entry(&route_host, pif);
    ret = rib_add(&route_host.prefix, route_host.vpn, &route_host.nhp[0]);
    if (ERRNO_SUCCESS != ret)
    {
        ROUTE_LOG_ERROR("errcode = %d\n", ret);

        return ret;
    }

    /* 添加直连网段路由 */
    if (route_net.prefix.prefixlen != IPV4_MAX_PREFIXLEN)
    {
        route_if_set_route_entry(&route_net, pif);
        ret = rib_add(&route_net.prefix, route_net.vpn, &route_net.nhp[0]);
        if (ERRNO_SUCCESS != ret)
        {
            ROUTE_LOG_ERROR("errcode = %d\n", ret);

            return ret;
        }

        ROUTE_LOG_DEBUG("Network routing add: ip: %#x   mask: %d  vpn: %d\n",
                    route_net.prefix.addr.ipv4, route_net.prefix.prefixlen, route_net.vpn);
    }

    ROUTE_LOG_DEBUG("The host routing add: ip: %#x   mask: %d  vpn: %d\n",
            route_host.prefix.addr.ipv4, route_host.prefix.prefixlen, route_host.vpn);
    
    /*add intf's net route,resolve arp's outif and flush to ftm*/
    arp_static_route_change_resolve_outif(pif, IPC_OPCODE_ADD);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 删除接口信息生成的直连路由（IPv4）
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 10:08:37
 * @note       :
 */
static int route_if_delete_routing(const struct route_if *pif)
{
    struct route_entry  route_net;
    struct route_entry  route_host;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memset(&route_net, 0, sizeof(struct route_entry));
    memset(&route_host, 0, sizeof(struct route_entry));

    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))      // 主IP路由
    {
        route_net.prefix.prefixlen          = pif->intf.ipv4[0].prefixlen;
        route_net.nhp[0].nexthop.addr.ipv4  = 0;

        route_host.prefix.prefixlen         = IPV4_MAX_PREFIXLEN;
        route_host.nhp[0].nexthop.addr.ipv4 = pif->intf.ipv4[0].addr;
    }
    else if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr)) // 从IP路由
    {
        route_net.prefix.prefixlen          = pif->intf.ipv4[1].prefixlen;
        route_net.nhp[0].nexthop.addr.ipv4  = 0;

        route_host.prefix.prefixlen         = IPV4_MAX_PREFIXLEN;
        route_host.nhp[0].nexthop.addr.ipv4 = pif->intf.ipv4[1].addr;
    }

    /* 删除接口直连主机路由 */
    route_if_set_route_entry(&route_host, pif);
    rib_delete(&route_host.prefix, route_host.vpn, &route_host.nhp[0]);

    ROUTE_LOG_DEBUG("The host routing delete: ip: %#x   mask: %d  vpn: %d\n",
            route_host.prefix.addr.ipv4, route_host.prefix.prefixlen, route_host.vpn);

    /* 删除接口直连网段路由 */
    route_if_set_route_entry(&route_net, pif);
    rib_delete(&route_net.prefix, route_net.vpn, &route_net.nhp[0]);

    ROUTE_LOG_DEBUG("Network routing delete: ip: %#x   mask: %d  vpn: %d\n",
                route_net.prefix.addr.ipv4, route_net.prefix.prefixlen, route_net.vpn);

    /*del intf's net route,resolve arp's outif and flush to ftm*/
    arp_static_route_change_resolve_outif(pif, IPC_OPCODE_DELETE);
    
    return ERRNO_SUCCESS;
}


/**
 * @brief      : 根据接口信息填充路由结构，生成直连路由（IPv6）
 * @param[in ] : proute - 活跃路由结构
 * @param[in ] : pif    - route 模块接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 10:45:13
 * @note       :
 */
static void route_if_set_v6_route_entry(struct route_entry *proute,
                                                const struct route_if *pif)
{
    struct prefix_ipv6 destip;

    ROUTE_LOG_DEBUG();

    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
    {
        destip.prefixlen = proute->prefix.prefixlen;
        memcpy(destip.prefix.s6_addr, pif->intf.ipv6[0].addr, IPV6_ADDR_LEN);

        apply_mask_ipv6(&destip);
        memcpy(proute->prefix.addr.ipv6, destip.prefix.s6_addr, IPV6_ADDR_LEN);
    }
    else if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
    {
        destip.prefixlen = proute->prefix.prefixlen;
        memcpy(destip.prefix.s6_addr, pif->intf.ipv6[1].addr, IPV6_ADDR_LEN);

        apply_mask_ipv6(&destip);
        memcpy(proute->prefix.addr.ipv6, destip.prefix.s6_addr, IPV6_ADDR_LEN);
    }

    proute->vpn                         = pif->intf.vpn;
    proute->nhp_num                     = 1;
    proute->prefix.type                 = INET_FAMILY_IPV6;
    proute->nhp[0].nhp_type             = NHP_TYPE_CONNECT;
    proute->nhp[0].protocol             = ROUTE_PROTO_CONNECT;
    proute->nhp[0].distance             = ROUTE_METRIC_CONNECT;
    proute->nhp[0].ifindex              = pif->ifindex;
    proute->nhp[0].vpn                  = pif->intf.vpn;
    proute->nhp[0].nexthop.type         = INET_FAMILY_IPV6;
    proute->nhp[0].nexthop_connect.type = INET_FAMILY_IPV6;
    proute->nhp[0].active               = ROUTE_STATUS_INACTIVE;
    proute->nhp[0].down_flag            = LINK_DOWN;
    proute->nhp[0].cost                 = ROUTE_COST;
    memset(proute->nhp[0].nexthop_connect.addr.ipv6, 0, IPV6_ADDR_LEN);

    if (proute->prefix.prefixlen == IPV6_MAX_PREFIXLEN)
    {
        /* 主机路由上送 cpu */
        proute->nhp[0].action = NHP_ACTION_TOCPU;
    }
    else
    {
        /* 网段路由转发 */
        proute->nhp[0].action = NHP_ACTION_FORWARD;
    }

    return;
}


/**
 * @brief      : 添加根据接口信息生成的链路本地地址路由
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 10:49:25
 * @note       : 链路本地地址生成的路由为 fe80::/10
 */
static int route_if_add_v6_link_routing(const struct route_if *pif)
{
    struct prefix_ipv6 destip;
    struct route_entry route;
    int ret = 0;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memset(&route, 0, sizeof(struct route_entry));
    route.prefix.prefixlen = 10;

    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
    {
        destip.prefixlen = route.prefix.prefixlen;
        memcpy(destip.prefix.s6_addr, pif->intf.ipv6_link_local.addr, IPV6_ADDR_LEN);

        apply_mask_ipv6(&destip);
        memcpy(route.prefix.addr.ipv6, destip.prefix.s6_addr, IPV6_ADDR_LEN);
    }

    route.nhp_num                     = 1;
    route.prefix.type                 = INET_FAMILY_IPV6;
    route.vpn                         = pif->intf.vpn;
    route.nhp[0].nhp_type             = NHP_TYPE_CONNECT;
    route.nhp[0].protocol             = ROUTE_PROTO_CONNECT;
    route.nhp[0].distance             = ROUTE_METRIC_CONNECT;
    route.nhp[0].action               = NHP_ACTION_TOCPU;
    route.nhp[0].active               = ROUTE_STATUS_INACTIVE;
    route.nhp[0].down_flag            = LINK_DOWN;
    route.nhp[0].cost                 = ROUTE_COST;
    route.nhp[0].vpn                  = pif->intf.vpn;
    route.nhp[0].nexthop.type         = INET_FAMILY_IPV6;
    route.nhp[0].nexthop_connect.type = INET_FAMILY_IPV6;
    memset(route.nhp[0].nexthop_connect.addr.ipv6, 0, IPV6_ADDR_LEN);

    link_local_num[route.vpn]++;

    /* 链路本地地址生成的路由相同，为 fe80::/10，通过计数来保证路由正确 */
    if (NULL == ribv6_lookup(&route.prefix, route.vpn))
    {
        ret = ribv6_add(&route.prefix, route.vpn, &route.nhp[0]);
        if (ERRNO_SUCCESS != ret)
        {
            ROUTE_LOG_ERROR("errcode = %d\n", ret);

            return ret;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 删除根据接口信息生成的链路本地地址路由
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 10:52:00
 * @note       :
 */
static int route_if_delete_v6_link_routing(const struct route_if *pif)
{
    struct prefix_ipv6 destip;
    struct route_entry route;
    int ret = 0;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    /* 计数不为 0，说明还有链路本地地址存在 */
    if (--link_local_num[pif->intf.vpn] > 0)
    {
        return ERRNO_SUCCESS;
    }

    memset(&route, 0, sizeof(struct route_entry));
    route.prefix.prefixlen = 10;

    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
    {
        destip.prefixlen = route.prefix.prefixlen;
        memcpy(destip.prefix.s6_addr, pif->intf.ipv6_link_local.addr, IPV6_ADDR_LEN);

        apply_mask_ipv6(&destip);
        memcpy(route.prefix.addr.ipv6, destip.prefix.s6_addr, IPV6_ADDR_LEN);
    }

    route.nhp_num                     = 1;
    route.prefix.type                 = INET_FAMILY_IPV6;
    route.vpn                         = pif->intf.vpn;
    route.nhp[0].nhp_type             = NHP_TYPE_CONNECT;
    route.nhp[0].protocol             = ROUTE_PROTO_CONNECT;
    route.nhp[0].distance             = ROUTE_METRIC_CONNECT;
    route.nhp[0].action               = NHP_ACTION_TOCPU;
    route.nhp[0].active               = ROUTE_STATUS_INACTIVE;
    route.nhp[0].down_flag            = LINK_DOWN;
    route.nhp[0].cost                 = ROUTE_COST;
    route.nhp[0].vpn                  = pif->intf.vpn;
    route.nhp[0].nexthop.type         = INET_FAMILY_IPV6;
    route.nhp[0].nexthop_connect.type = INET_FAMILY_IPV6;
    memset(route.nhp[0].nexthop_connect.addr.ipv6, 0, IPV6_ADDR_LEN);

    ret = ribv6_delete(&route.prefix, route.vpn, &route.nhp[0]);
    if (ERRNO_SUCCESS != ret)
    {
        ROUTE_LOG_ERROR("errcode = %d\n", ret);

        return ret;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 根据接口信息生成直连路由（IPv6）
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 10:54:00
 * @note       :
 */
static int route_if_add_v6_routing(const struct route_if *pif)
{
    struct route_entry route_net;
    struct route_entry route_host;
    int                ret;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memset(&route_net, 0, sizeof(struct route_entry));
    memset(&route_host, 0, sizeof(struct route_entry));

    /* 填充直连路由的掩码和下一跳 */
    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))        // 主 IP 路由
    {
        route_net.prefix.prefixlen = pif->intf.ipv6[0].prefixlen;
        memset(route_net.nhp[0].nexthop.addr.ipv6, 0, IPV6_ADDR_LEN);

        route_host.prefix.prefixlen = IPV6_MAX_PREFIXLEN;
        memcpy(route_host.nhp[0].nexthop.addr.ipv6, pif->intf.ipv6[0].addr, IPV6_ADDR_LEN);
    }
    else if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))   // 从 IP 路由
    {
        route_net.prefix.prefixlen = pif->intf.ipv6[1].prefixlen;
        memset(route_net.nhp[0].nexthop.addr.ipv6, 0, IPV6_ADDR_LEN);

        route_host.prefix.prefixlen = IPV6_MAX_PREFIXLEN;
        memcpy(route_host.nhp[0].nexthop.addr.ipv6, pif->intf.ipv6[1].addr, IPV6_ADDR_LEN);
    }

    /* 添加直连主机路由 */
    route_if_set_v6_route_entry(&route_host, pif);
    ret = ribv6_add(&route_host.prefix, route_host.vpn, &route_host.nhp[0]);
    if (ERRNO_SUCCESS != ret)
    {
        ROUTE_LOG_ERROR("errcode = %d\n", ret);

        return ret;
    }

    /* 添加直连网段路由 */
    if (route_net.prefix.prefixlen != IPV6_MAX_PREFIXLEN)
    {
        route_if_set_v6_route_entry(&route_net, pif);
        ret = ribv6_add(&route_net.prefix, route_net.vpn, &route_net.nhp[0]);
        if (ERRNO_SUCCESS != ret)
        {
            ROUTE_LOG_ERROR("errcode = %d\n", ret);

            return ret;
        }

        ROUTE_LOG_DEBUG("Network routing add: ip: %#x   mask: %d  vpn: %d\n",
                    route_net.prefix.addr.ipv4, route_net.prefix.prefixlen, route_net.vpn);
    }

    ROUTE_LOG_DEBUG("The host routing add: ip: %#x   mask: %d  vpn: %d\n",
            route_host.prefix.addr.ipv4, route_host.prefix.prefixlen, route_host.vpn);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 删除接口信息生成的直连路由（IPv6）
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 10:55:39
 * @note       :
 */
static int route_if_delete_v6_routing(const struct route_if *pif)
{
    struct route_entry  route_net;
    struct route_entry  route_host;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memset(&route_net, 0, sizeof(struct route_entry));
    memset(&route_host, 0, sizeof(struct route_entry));

    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))        // 主IP路由
    {
        route_net.prefix.prefixlen = pif->intf.ipv6[0].prefixlen;
        memset(route_net.nhp[0].nexthop.addr.ipv6, 0, IPV6_ADDR_LEN);

        route_host.prefix.prefixlen = IPV6_MAX_PREFIXLEN;
        memcpy(route_host.nhp[0].nexthop.addr.ipv6, pif->intf.ipv6[0].addr, IPV6_ADDR_LEN);
    }
    else if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))   // 从IP路由
    {
        route_net.prefix.prefixlen = pif->intf.ipv6[1].prefixlen;
        memset(route_net.nhp[0].nexthop.addr.ipv6, 0, IPV6_ADDR_LEN);

        route_host.prefix.prefixlen = IPV6_MAX_PREFIXLEN;
        memcpy(route_host.nhp[0].nexthop.addr.ipv6, pif->intf.ipv6[1].addr, IPV6_ADDR_LEN);
    }

    /* 删除接口直连主机路由 */
    route_if_set_v6_route_entry(&route_host, pif);
    ribv6_delete(&route_host.prefix, route_host.vpn, &route_host.nhp[0]);

    ROUTE_LOG_DEBUG("The host routing delete: ip: %#x   mask: %d  vpn: %d\n",
            route_host.prefix.addr.ipv4, route_host.prefix.prefixlen, route_host.vpn);

    /* 删除接口直连网段路由 */
    route_if_set_v6_route_entry(&route_net, pif);
    ribv6_delete(&route_net.prefix, route_net.vpn, &route_net.nhp[0]);

    ROUTE_LOG_DEBUG("Network routing delete: ip: %#x   mask: %d  vpn: %d\n",
                route_net.prefix.addr.ipv4, route_net.prefix.prefixlen, route_net.vpn);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 三层接口配置信息下发至 ftm
 * @param[in ] : pif    - route 模块接口结构
 * @param[in ] : opcode - 操作码，add/delete/update
 * @param[in ] : info   - 更新的具体信息，如 IP 地址、urpf 等
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 10:57:24
 * @note       :
 */
static void route_if_send_to_ftm(struct route_if *pif, enum IPC_OPCODE opcode,
                                            enum L3IF_INFO info)
{
    if (NULL == pif)
    {
        return;
    }

    if (ERRNO_SUCCESS != ROUTE_IPC_SENDTO_FTM(&pif->intf, sizeof(struct ifm_l3), 1, MODULE_ID_FTM,
                                    MODULE_ID_ROUTE, IPC_TYPE_L3IF, info, opcode, pif->ifindex))
    {
        ROUTE_LOG_ERROR("IPC send to ftm\n");

        return;
    }

    return;
}


/**
 * @brief      : 三层接口信息变化事件填充
 * @param[in ] : pif    - route 模块接口结构
 * @param[in ] : event  - 通知事件类型
 * @param[in ] : type   - IP 地址类型
 * @param[in ] : info   - 本地事件类型
 * @param[out] : pevent - 通知事件结构
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:00:35
 * @note       :
 */
static void route_if_set_event(const struct route_if *pif, struct ifm_event *pevent,
                                        enum IFNET_EVENT event, enum IP_TYPE type, enum L3IF_INFO info)
{
    ROUTE_LOG_DEBUG();

    if ((NULL == pif) || (NULL == pevent))
    {
        return;
    }

    memset(pevent, 0, sizeof(struct ifm_event));

    /* 填充通知事件基本信息 */
    pevent->event   = event;
    pevent->up_flag = pif->down_flag;
    pevent->mode    = IFNET_MODE_L3;
    pevent->ifindex = pif->ifindex;
    pevent->vpn     = pif->intf.vpn;

    if (L3IF_INFO_IPV4 == info)
    {
        /* IPv4 信息变化通知 */
        if (IP_TYPE_SLAVE == type)
        {
            pevent->ipaddr.type      = INET_FAMILY_IPV4;
            pevent->ipaddr.addr.ipv4 = pif->intf.ipv4[1].addr;
            pevent->ipaddr.prefixlen = pif->intf.ipv4[1].prefixlen;
            pevent->ipflag           = type;
        }
        else
        {
            pevent->ipaddr.type      = INET_FAMILY_IPV4;
            pevent->ipaddr.addr.ipv4 = pif->intf.ipv4[0].addr;
            pevent->ipaddr.prefixlen = pif->intf.ipv4[0].prefixlen;
            pevent->ipflag           = pif->intf.ipv4_flag;
        }
    }
    else if (L3IF_INFO_IPV6 == info)
    {
        /* IPv6 信息变化通知 */
        if (IP_TYPE_SLAVE == type)
        {
            memcpy(pevent->ipaddr.addr.ipv6, pif->intf.ipv6[1].addr, IPV6_ADDR_LEN);
            pevent->ipaddr.type      = INET_FAMILY_IPV6;
            pevent->ipaddr.prefixlen = pif->intf.ipv6[1].prefixlen;
            pevent->ipflag           = type;
        }
        else
        {
            memcpy(pevent->ipaddr.addr.ipv6, pif->intf.ipv6[0].addr, IPV6_ADDR_LEN);
            pevent->ipaddr.type      = INET_FAMILY_IPV6;
            pevent->ipaddr.prefixlen = pif->intf.ipv6[0].prefixlen;
            pevent->ipflag           = pif->intf.ipv6_flag;
        }
    }
    else if (L3IF_INFO_IPV6_LINK_LOCAL == info)
    {
        /* 链路本地地址变化通知 */
        memcpy(pevent->ipaddr.addr.ipv6, pif->intf.ipv6_link_local.addr, IPV6_ADDR_LEN);
        pevent->ipaddr.type = INET_FAMILY_IPV6;
        pevent->ipflag      = IP_TYPE_LINK_LOCAL;
    }

    return;
}


/**
 * @brief      : 将配置信息存储到临时结构中
 * @param[in ] : pif_src - 原始接口结构
 * @param[in ] : type    - IP 地址类型
 * @param[in ] : info    - 更新信息类型
 * @param[out] : pif_dst - 目标接口结构
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:13:29
 * @note       : 配置主 IP 时将从 IP 清掉，反之亦然
 */
static void route_if_process_data(const struct route_if *pif_src, struct route_if *pif_dst,
                                                enum IP_TYPE type, enum L3IF_INFO info)
{
    if ((NULL == pif_src) || (NULL == pif_dst))
    {
        return;
    }

    memcpy(pif_dst, pif_src, sizeof(struct route_if));

    if (L3IF_INFO_IPV4 == info)
    {
        if (IP_TYPE_SLAVE == type)
        {
            ROUTE_IF_VAL_RESET(pif_dst->intf.ipv4[0].addr);
            ROUTE_IF_VAL_RESET(pif_dst->intf.ipv4[0].prefixlen);
        }
        else
        {
            ROUTE_IF_VAL_RESET(pif_dst->intf.ipv4[1].addr);
            ROUTE_IF_VAL_RESET(pif_dst->intf.ipv4[1].prefixlen);
        }
    }
    else if (L3IF_INFO_IPV6 == info)
    {
        if (IP_TYPE_SLAVE == type)
        {
            memset(pif_dst->intf.ipv6[0].addr, 0, sizeof(pif_dst->intf.ipv6[0].addr));
            ROUTE_IF_VAL_RESET(pif_dst->intf.ipv6[0].prefixlen);
        }
        else
        {
            memset(pif_dst->intf.ipv6[1].addr, 0, sizeof(pif_dst->intf.ipv6[1].addr));
            ROUTE_IF_VAL_RESET(pif_dst->intf.ipv6[1].prefixlen);
        }
    }

    return;
}


/**
 * @brief      : 处理所有通知事件及路由的增删
 * @param[in ] : pif    - route 模块接口结构
 * @param[in ] : opcode - 操作码
 * @param[in ] : event  - 事件类型
 * @param[in ] : type   - IP 地址类型
 * @param[in ] : info   - 消息类型
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:20:10
 * @note       :
 */
static void route_if_notify(struct route_if *pif, enum IPC_OPCODE opcode,
                                enum IFNET_EVENT event, enum IP_TYPE type, enum L3IF_INFO info)
{
    struct listnode *pnode      = NULL;
    uint32_t        *pifindex   = NULL;
    struct route_if *pif_lookup = NULL;
    struct route_if  lif;
    struct ifm_event levent;

    if (NULL == pif)
    {
        return;
    }

    ROUTE_LOG_DEBUG("OPCODE = %d, EVENT = %d, TYPE = %d, down_flag = %d.\n",
                            opcode, event, type, pif->down_flag);

    route_if_process_data(pif, &lif, type, info);

    /* 先删除接口 IP，否则影响路由 */
    if (IPC_OPCODE_DELETE == opcode)
    {
        if (L3IF_INFO_IPV4 == info)
        {
            if (IP_TYPE_SLAVE == type)
            {
                ROUTE_IF_VAL_RESET(pif->intf.ipv4[1].addr);
                ROUTE_IF_VAL_RESET(pif->intf.ipv4[1].prefixlen);
            }
            else
            {
                ROUTE_IF_VAL_RESET(pif->intf.ipv4[0].addr);
                ROUTE_IF_VAL_RESET(pif->intf.ipv4[0].prefixlen);
            }
        }
        else if (L3IF_INFO_IPV6 == info)
        {
            if (IP_TYPE_SLAVE == type)
            {
                memset(pif->intf.ipv6[1].addr, 0, sizeof(pif->intf.ipv6[1].addr));
                ROUTE_IF_VAL_RESET(pif->intf.ipv6[1].prefixlen);
            }
            else
            {
                memset(pif->intf.ipv6[0].addr, 0, sizeof(pif->intf.ipv6[0].addr));
                ROUTE_IF_VAL_RESET(pif->intf.ipv6[0].prefixlen);
            }
        }
    }

    route_if_send_to_ftm(&lif, opcode, info);
    route_if_set_event(&lif, &levent, event, type, info);
    l3if_event_notify(levent.event, &levent);

    if (L3IF_INFO_IPV4 == info)
    {
        if (IPC_OPCODE_DELETE == opcode)
        {
            route_if_delete_routing(&lif);
        }
        else if (IPC_OPCODE_ADD == opcode)
        {
            if (lif.down_flag == LINK_UP)
            {
                route_if_add_routing(&lif);
            }
        }
    }
    else if (L3IF_INFO_IPV6 == info)
    {
        if (IPC_OPCODE_DELETE == opcode)
        {
            route_if_delete_v6_routing(&lif);
        }
        else if (IPC_OPCODE_ADD == opcode)
        {
            if (lif.down_flag == LINK_UP)
            {
                route_if_add_v6_routing(&lif);
            }
        }
    }

    /* 处理接口借用 IP 事件通知 */
    if ((L3IF_INFO_IPV4 == info) && (IP_TYPE_SLAVE != type))
    {
        for (ALL_LIST_ELEMENTS_RO(&pif->unnumbered_list, pnode, pifindex))
        {
            pif_lookup = route_if_lookup((uint32_t)pifindex);
            if (NULL == pif_lookup)
            {
                continue;
            }

            memset(&levent, 0, sizeof(struct ifm_event));
            levent.ipaddr.type      = INET_FAMILY_IPV4;
            levent.ipaddr.addr.ipv4 = lif.intf.ipv4[0].addr;
            levent.ipaddr.prefixlen = lif.intf.ipv4[0].prefixlen;
            levent.ifindex          = pif_lookup->ifindex;
            levent.up_flag          = pif_lookup->down_flag;
            levent.mode             = IFNET_MODE_L3;
            levent.ipflag           = IP_TYPE_UNNUMBERED;
            levent.vpn              = pif_lookup->intf.vpn;

            if (IPC_OPCODE_DELETE == opcode)
            {
                levent.event = IFNET_EVENT_IP_DELETE;
            }
            else if (IPC_OPCODE_ADD == opcode)
            {
                levent.event = IFNET_EVENT_IP_ADD;
            }

            l3if_event_notify(levent.event, &levent);
        }
    }

    return;
}


/**
 * @brief      : IP 地址更新处理
 * @param[in ] : pif_old - 原始接口结构
 * @param[in ] : pif_new - 新配置接口结构
 * @param[in ] : type    - IP 地址类型
 * @param[in ] : info    - 消息类型
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:31:52
 * @note       :
 */
static void route_if_update_ip(struct route_if *pif_old, struct route_if *pif_new,
                                        enum IP_TYPE type, enum L3IF_INFO info)
{
    ROUTE_LOG_DEBUG();

    if ((NULL == pif_old) || (NULL == pif_new))
    {
        return;
    }

    /* 删除原有接口 IP 配置 */
    route_if_notify(pif_old, IPC_OPCODE_DELETE, IFNET_EVENT_IP_DELETE, type, info);

    /* 添加新接口主 IP 配置 */
    if (L3IF_INFO_IPV4 == info)
    {
        if (IP_TYPE_SLAVE == type)
        {
            pif_old->intf.ipv4[1] = pif_new->intf.ipv4[1];
        }
        else
        {
            pif_old->intf.ipv4[0] = pif_new->intf.ipv4[0];
			ROUTE_ARPIF_CFG_INIT(pif_old, pif_old->ifindex);//仅在主ip更新时更新arp配置
        }
    }
    else if (L3IF_INFO_IPV6 == info)
    {
        if (IP_TYPE_SLAVE == type)
        {
            pif_old->intf.ipv6[1] = pif_new->intf.ipv6[1];
        }
        else
        {
            pif_old->intf.ipv6[0] = pif_new->intf.ipv6[0];
			//ROUTE_NDPIF_CFG_INIT(pif_old, pif_old->ifindex);//仅在主ip更新时更新NDP配置
        }
    }

    pif_old->intf.vpn = pif_new->intf.vpn;

    route_if_notify(pif_old, IPC_OPCODE_ADD, IFNET_EVENT_IP_ADD, type, info);

    return;
}


/**
 * @brief      : 计算 route 模块接口哈希表 key 值
 * @param[in ] : hash_key - 接口索引
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:36:22
 * @note       :
 */
static unsigned int route_if_compute_hash(void *hash_key)
{
    return (uint32_t)hash_key;
}


/**
 * @brief      : 比较 route 模块接口哈希表 key 值
 * @param[in ] : item     - 哈希桶
 * @param[in ] : hash_key - 接口索引
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:37:17
 * @note       :
 */
static int route_if_compare(void *item, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)item;

    if (NULL == item)
    {
        return ERRNO_FAIL;
    }

    if (pbucket->hash_key == hash_key)
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : route 模块接口结构创建
 * @param[in ] : pif - route 模块结构结构
 * @param[out] :
 * @return     : 成功返回分配内存的 route 模块接口结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:37:54
 * @note       :
 */
static struct route_if *route_if_create(struct route_if *pif)
{
    struct route_if *pif_new = NULL;
    struct ifm_info  pifm    = {0};
    int ret;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return NULL;
    }

    pif_new = (struct route_if *)XCALLOC(MTYPE_IF, sizeof(struct route_if));
    if (NULL == pif_new)
    {
        ROUTE_LOG_ERROR("malloc\n");

        return NULL;
    }

    /* 鍒涘缓 route if 鏃惰幏鍙栦竴娆℃帴鍙ｇ姸鎬? */
    if (ifm_get_all_info(pif->ifindex, MODULE_ID_ROUTE, &pifm) != 0)
    {
        pif->down_flag  = LINK_DOWN;
        pif->encap_flag = 0;
    }
    else
    {
        pif->down_flag = pifm.status;

        if (pifm.encap.type != IFNET_ENCAP_INVALID)
        {
            pif->encap_flag = 1;
        }

        if (pifm.shutdown == IFNET_SHUTDOWN)
        {
            pif->shutdown_flag = 1;
        }
    }

    /* 先通知 ftm 创建接口结构 */
    pif_new->ifindex = pif->ifindex;
    route_if_send_to_ftm(pif_new, IPC_OPCODE_ADD, L3IF_INFO_INVALID);

    memcpy(pif_new, pif, sizeof(struct route_if));
    pif_new->intf.ifindex = pif->ifindex;

    arp_static_process_l3if_add(pif->ifindex);

    ret = route_if_add(pif_new);
    if (ERRNO_SUCCESS != ret)
    {
        XFREE(MTYPE_IF, pif_new);

        return NULL;
    }

    return pif_new;
}


/**
 * @brief      : route 模块接口哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:40:21
 * @note       :
 */
void route_if_table_init (int size)
{
    hios_hash_init(&route_if_table, size, route_if_compute_hash, route_if_compare);

    return;
}


/**
 * @brief      : route 模块接口条目添加
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:40:56
 * @note       :
 */
int route_if_add(struct route_if *pif)
{
    struct hash_bucket *pitem = NULL;

    ROUTE_LOG_DEBUG();

    if (pif == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (route_if_table.hash_size == route_if_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
        ROUTE_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)pif->ifindex;
    pitem->data     = pif;

    hios_hash_add(&route_if_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : route 模块接口条目删除
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:42:12
 * @note       :
 */
int route_if_delete(uint32_t ifindex)
{
    struct hash_bucket *pitem = NULL;

    ROUTE_LOG_DEBUG();

    pitem = hios_hash_find(&route_if_table, (void *)ifindex);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    hios_hash_delete(&route_if_table, pitem);
    XFREE(MTYPE_IF, pitem->data);
    XFREE(MTYPE_HASH_BACKET, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : route 模块接口条目查找
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 route 模块接口条目，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:42:40
 * @note       :
 */
struct route_if *route_if_lookup(uint32_t ifindex)
{
    struct hash_bucket *pitem = NULL;

    ROUTE_LOG_DEBUG();

    pitem = hios_hash_find(&route_if_table, (void *)ifindex);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct route_if *)pitem->data;
}


/**
 * @brief      : 批量获取 route 模块接口结构
 * @param[in ] : ifindex - 接口索引
 * @param[out] : l3if[]  - 多个 route 模块接口结构
 * @return     : 返回获取到的 route 模块接口结构的数量
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:44:24
 * @note       : 提供给 snmp 的 get next 接口，空索引从数据结构开始返回数据，
 * @note       : 非空索引从传入索引节点下一个节点开始返回数据
 */
int route_if_get_bulk(uint32_t ifindex, struct ifm_l3 l3if[])
{
    struct hash_bucket *pbucket    = NULL;
    struct hash_bucket *pnext      = NULL;
    struct route_if    *pif        = NULL;
    struct route_if    *pif_lookup = NULL;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct ifm_l3);
    int cursor;
    int val = 0;

    if (0 == ifindex)
    {
        /* 初次获取 */
        HASH_BUCKET_LOOP(pbucket, cursor, route_if_table)
        {
            pif = (struct route_if *)pbucket->data;
            if (NULL == pif)
            {
                continue;
            }

            l3if[data_num] = pif->intf;

            if (0 != pif->intf.unnumbered_if)
            {
                pif_lookup = route_if_lookup(pif->intf.unnumbered_if);
                if (NULL != pif_lookup)
                {
                    l3if[data_num].ipv4[0] = pif_lookup->intf.ipv4[0];
                }
            }

            data_num++;

            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        /* 找到 ifindex 后面第一个数据 */
        pbucket = hios_hash_find(&route_if_table, (void *)ifindex);
        if (NULL == pbucket)
        {
            /*
             * 当前数据可能已经被删除，返回到当前哈希桶的链表头部
             * 如果当前哈希桶链表头为空，向后面的哈希桶继续查找
             */
            val = route_if_table.compute_hash((void *)ifindex)%HASHTAB_SIZE;

            if (NULL != route_if_table.buckets[val])
            {
                pbucket = route_if_table.buckets[val];
            }
            else
            {
                for (++val; val<HASHTAB_SIZE; ++val)
            	{
            		if (NULL != route_if_table.buckets[val])
            		{
            			pbucket = route_if_table.buckets[val];
                        break;
            		}
                }
            }
        }

        if (NULL != pbucket)
        {
            for (data_num=0; data_num<msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&route_if_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                pif = pnext->data;
                l3if[data_num] = pif->intf;

                if (0 != pif->intf.unnumbered_if)
                {
                    pif_lookup = route_if_lookup(pif->intf.unnumbered_if);
                    if (NULL != pif_lookup)
                    {
                        l3if[data_num].ipv4[0] = pif_lookup->intf.ipv4[0];
                    }
                }

                pbucket = pnext;
            }
        }
    }

    return data_num;
}


/**
 * @brief      : 物理接口删除时，删除子接口，保证路由收敛速度
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:47:44
 * @note       : 先将所有隶属于该物理接口的子接口的直连路由清除，最后再更新一次静态路由
 */
void route_if_process_subif_delete(uint32_t ifindex)
{
    struct route_if    *pif          = NULL;
    struct hash_bucket *pbucket      = NULL;
    struct hash_bucket *pbucket_next = NULL;
    int cursor;

    g_route.conver_flag = DISABLE;

    HASH_BUCKET_LOOPW(pbucket, cursor, route_if_table)
    {
        pbucket_next = pbucket->next;

        pif = pbucket->data;
        if (NULL == pif)
        {
            pbucket = pbucket_next;
            continue;
        }

        if ((IFM_TYPE_IS_METHERNET(pif->ifindex) && !IFM_IS_SUBPORT(pif->ifindex))
            || ((IFM_TYPE_IS_TRUNK(pif->ifindex) && !IFM_IS_SUBPORT(pif->ifindex))))
        {
            pbucket = pbucket_next;
            continue;
        }

        if (ifindex == IFM_PARENT_IFINDEX_GET(pif->ifindex))
        {
            route_if_delete_config(pif);
        }

        pbucket = pbucket_next;
    }

    g_route.conver_flag = ENABLE;

    if (0 == g_route.route_timer)
    {
        g_route.route_timer = ROUTE_TIMER_ADD(route_static_update_routing, NULL, 1);
    }

    if (0 == g_route.routev6_timer)
    {
        g_route.routev6_timer = ROUTE_TIMER_ADD(routev6_static_update_routing, NULL, 1);
    }

    return;
}


/**
 * @brief      : 物理接口状态变化时，处理子接口状态，保证路由收敛速度
 * @param[in ] : ifindex - 接口索引
 * @param[in ] : opcode  - 操作码
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:49:10
 * @note       : 先将所有隶属于该物理接口的子接口的直连路由增加或删除，最后再更新一次静态路由
 */
static void route_if_process_subif_status(uint32_t ifindex, enum OPCODE_E opcode)
{
    struct route_if    *pif     = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;

    g_route.conver_flag = DISABLE;

    HASH_BUCKET_LOOP(pbucket, cursor, route_if_table)
    {
        pif = pbucket->data;
        if (NULL == pif)
        {
            continue;
        }

        if ((IFM_TYPE_IS_METHERNET(pif->ifindex) && !IFM_IS_SUBPORT(pif->ifindex))
            || ((IFM_TYPE_IS_TRUNK(pif->ifindex) && !IFM_IS_SUBPORT(pif->ifindex))))
        {
            continue;
        }

        if (ifindex == IFM_PARENT_IFINDEX_GET(pif->ifindex))
        {
            if (OPCODE_UP == opcode)
            {
                if ((1 == pif->encap_flag) && (0 == pif->shutdown_flag))
                {
                    route_if_up(pif->ifindex);
                }
            }
            else if (OPCODE_DOWN == opcode)
            {
                route_if_down(pif->ifindex);
            }
        }
    }

    g_route.conver_flag = ENABLE;

    if (0 == g_route.route_timer)
    {
        g_route.route_timer = ROUTE_TIMER_ADD(route_static_update_routing, NULL, 1);
    }

    if (0 == g_route.routev6_timer)
    {
        g_route.routev6_timer = ROUTE_TIMER_ADD(routev6_static_update_routing, NULL, 1);
    }

    return;
}


/**
 * @brief      : route 模块接口 down 事件
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 11:51:24
 * @note       :
 */
int route_if_down(uint32_t ifindex)
{
    struct route_if *pif = NULL;
    struct route_if  if_del;

    ROUTE_LOG_DEBUG();

    /* 如果是物理接口 down，先将该物理接口下所有子接口置 down */
    if ((IFM_TYPE_IS_METHERNET(ifindex) && !IFM_IS_SUBPORT(ifindex))
        || ((IFM_TYPE_IS_TRUNK(ifindex) && !IFM_IS_SUBPORT(ifindex))))
    {
        route_if_process_subif_status(ifindex, OPCODE_DOWN);
        route_static_process_routing(ifindex);
    }

    pif = route_if_lookup(ifindex);
    if ((NULL == pif) || (LINK_DOWN == pif->down_flag))
    {
        return ERRNO_SUCCESS;
    }

    pif->down_flag = LINK_DOWN;

    /* 检测接口是否配置从 V4 IP */
    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
    {
        route_if_process_data(pif, &if_del, IP_TYPE_SLAVE, L3IF_INFO_IPV4);
        route_if_delete_routing(&if_del);
    }

    /* 检测接口是否配置主 V4 IP */
    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
    {
        route_if_process_data(pif, &if_del, IP_TYPE_STATIC, L3IF_INFO_IPV4);
        route_if_delete_routing(&if_del);
    }

    /* 检测接口是否配置从 V6 IP */
    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
    {
        route_if_process_data(pif, &if_del, IP_TYPE_SLAVE, L3IF_INFO_IPV6);
        route_if_delete_v6_routing(&if_del);
    }

    /* 检测接口是否配置主 V6 IP */
    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
    {
        route_if_process_data(pif, &if_del, IP_TYPE_STATIC, L3IF_INFO_IPV6);
        route_if_delete_v6_routing(&if_del);
    }

    /* 检测接口是否配置链路本地地址 */
    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
    {
        route_if_delete_v6_link_routing(pif);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : route 模块接口 up 事件
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 13:51:19
 * @note       :
 */
int route_if_up(uint32_t ifindex)
{
    struct route_if *pif = NULL;
    struct route_if if_add;

    ROUTE_LOG_DEBUG();

    /* 物理接口 up，先判断该物理接口下所有子接口是否 up */
    if ((IFM_TYPE_IS_METHERNET(ifindex) && !IFM_IS_SUBPORT(ifindex))
        || ((IFM_TYPE_IS_TRUNK(ifindex) && !IFM_IS_SUBPORT(ifindex))))
    {
        route_if_process_subif_status(ifindex, OPCODE_UP);
        route_static_process_routing(ifindex);
    }

    pif = route_if_lookup(ifindex);
    if ((NULL == pif) || (LINK_UP == pif->down_flag))
    {
        return ERRNO_SUCCESS;
    }

    pif->down_flag = LINK_UP;

    /* 检测接口是否配置主 V4 IP */
    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
    {
        route_if_process_data(pif, &if_add, IP_TYPE_STATIC, L3IF_INFO_IPV4);
        route_if_add_routing(&if_add);
    }

    /* 检测接口是否配置从 V4 IP */
    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
    {
        route_if_process_data(pif, &if_add, IP_TYPE_SLAVE, L3IF_INFO_IPV4);
        route_if_add_routing(&if_add);
    }

    /* 检测接口是否配置主 V6 IP */
    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
    {
        route_if_process_data(pif, &if_add, IP_TYPE_STATIC, L3IF_INFO_IPV6);
        route_if_add_v6_routing(&if_add);
    }

    /* 检测接口是否配置从 V4 IP */
    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
    {
        route_if_process_data(pif, &if_add, IP_TYPE_SLAVE, L3IF_INFO_IPV6);
        route_if_add_v6_routing(&if_add);
    }

    /* 检测接口是否配置链路本地地址 */
    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
    {
        route_if_add_v6_link_routing(pif);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口主 IP 地址添加（IPv4）
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:08:26
 * @note       :
 */
int route_if_add_ip(struct route_if *pif)
{
    struct route_if *pif_new    = NULL;
    struct route_if *pif_lookup = NULL;
    struct route_if  pif_router_id;

    ROUTE_LOG_DEBUG();

    pif_lookup = route_if_lookup(pif->ifindex);
    if (NULL == pif_lookup)
    {
        pif_new = route_if_create(pif);
        if (NULL == pif_new)
        {
            ROUTE_LOG_ERROR("create route if\n");

            return ERRNO_MALLOC;
        }

        if (ROUTE_IF_IP_EXIST(pif_new->intf.ipv4[0].addr))
        {
			/* 只有在添加主 IP 时候才会添加接口 ARP 信息 */
            ROUTE_ARPIF_CFG_INIT(pif_new,pif->ifindex);

			route_if_notify(pif_new, IPC_OPCODE_ADD, IFNET_EVENT_IP_ADD,
                            pif_new->intf.ipv4_flag, L3IF_INFO_IPV4);
        }
    }
    else
    {
        pif->intf.vpn = pif_lookup->intf.vpn;
        pif_lookup->intf.ipv4_flag = pif->intf.ipv4_flag;

        if (ROUTE_IF_IP_EXIST(pif_lookup->intf.ipv4[0].addr)
            && ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))           // 主 IP 已配置
        {
            memcpy(&pif_router_id, pif_lookup, sizeof(struct route_if));

            route_if_update_ip(pif_lookup, pif, pif->intf.ipv4_flag, L3IF_INFO_IPV4);

            /* 更新 router-id */
            if (g_route.router_id == pif_router_id.intf.ipv4[0].addr)
            {
                g_route.router_id = 0;
            }
        }
        else                                                     // 主 IP 未配置
        {
            if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
            {
                pif_lookup->intf.ipv4[0] = pif->intf.ipv4[0];

				/* 只有在添加主 IP 时候才会添加接口 ARP 信息 */
                ROUTE_ARPIF_CFG_INIT(pif_lookup, pif->ifindex);

                route_if_notify(pif_lookup, IPC_OPCODE_ADD, IFNET_EVENT_IP_ADD,
                                pif_lookup->intf.ipv4_flag, L3IF_INFO_IPV4);
            }
        }
    }

    /* 生成 router-id */
    route_set_router_id();

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口从 IP 地址添加（IPv4）
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:11:01
 * @note       : 从地址添加时，主 IP 地址必须存在
 */
int route_if_add_slave_ip(struct route_if *pif)
{
    struct route_if *pif_lookup = NULL;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pif_lookup = route_if_lookup(pif->ifindex);
    if (pif_lookup == NULL)
    {
        return ERRNO_FAIL;
    }

    if (!ROUTE_IF_IP_EXIST(pif_lookup->intf.ipv4[0].addr))
    {
        return ERRNO_FAIL;
    }

    if (!ROUTE_IF_IP_EXIST(pif_lookup->intf.ipv4[1].addr))  // 从 IP 未配置
    {
        pif_lookup->intf.ipv4[1] = pif->intf.ipv4[1];

        route_if_notify(pif_lookup, IPC_OPCODE_ADD, IFNET_EVENT_IP_ADD,
                        IP_TYPE_SLAVE, L3IF_INFO_IPV4);
    }
    else                                                    // 从 IP 已配置
    {
        pif->intf.vpn = pif_lookup->intf.vpn;

        route_if_update_ip(pif_lookup, pif, IP_TYPE_SLAVE, L3IF_INFO_IPV4);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口主 IP 地址删除（IPv4）
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:12:59
 * @note       : 删除主 IP 地址时将从 IP 地址一并清除
 */
int route_if_delete_ip(struct route_if *pif)
{
    struct route_if *pif_unnumbered = NULL;
    struct route_if  lif;
    struct ifm_event levent;

    ROUTE_LOG_DEBUG();

    if (pif == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memcpy(&lif, pif, sizeof(struct route_if));

    /* 接口 IP 为借用 */
    if (0 != pif->intf.unnumbered_if)
    {
        pif_unnumbered = route_if_lookup(pif->intf.unnumbered_if);
        if (NULL != pif_unnumbered)
        {
            memset(&levent, 0, sizeof(struct ifm_event));
            levent.ipaddr.type      = INET_FAMILY_IPV4;
            levent.ipaddr.addr.ipv4 = pif_unnumbered->intf.ipv4[0].addr;
            levent.ipaddr.prefixlen = pif_unnumbered->intf.ipv4[0].prefixlen;
            levent.ifindex          = pif->ifindex;
            levent.up_flag          = pif->down_flag;
            levent.mode             = IFNET_MODE_L3;
            levent.ipflag           = IP_TYPE_UNNUMBERED;
            levent.vpn              = pif->intf.vpn;
            levent.event            = IFNET_EVENT_IP_DELETE;
            l3if_event_notify(levent.event, &levent);

            listnode_delete(&pif_unnumbered->unnumbered_list, (void *)pif->ifindex);
        }
    }

    /* 从 IP 存在删除从 IP */
    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
    {
        route_if_notify(pif, IPC_OPCODE_DELETE, IFNET_EVENT_IP_DELETE,
                        IP_TYPE_SLAVE, L3IF_INFO_IPV4);
    }

    /* 主 IP 存在删除主 IP */
    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
    {
		route_if_notify(pif, IPC_OPCODE_DELETE, IFNET_EVENT_IP_DELETE,
                        pif->intf.ipv4_flag, L3IF_INFO_IPV4);
    }

    /* 清除 IP 标志 */
    ROUTE_IF_VAL_RESET(pif->intf.ipv4_flag);

    /*清除arp 接口信息*/
    ROUTE_ARPIF_CFG_CLR(pif);

    /* 更新 router-id */
    if ((g_route.router_id == lif.intf.ipv4[0].addr)
        || (g_route.router_id == lif.intf.ipv4[1].addr))
    {
        g_route.router_id = 0;

        route_set_router_id();
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口从 IP 地址删除（IPv4）
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:42:25
 * @note       :
 */
int route_if_delete_slave_ip(struct route_if *pif)
{
    ROUTE_LOG_DEBUG();

    if (pif == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
    {
        route_if_notify(pif, IPC_OPCODE_DELETE, IFNET_EVENT_IP_DELETE,
                        IP_TYPE_SLAVE, L3IF_INFO_IPV4);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 根据 MAC 地址生成链路本地地址
 * @param[in ] : pmac  - MAC 地址
 * @param[out] : pipv6 - 生成的链路本地地址
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:43:53
 * @note       : u 位未翻转
 */
static int route_if_get_linklocal(uint8_t *pipv6, uint8_t *pmac)
{
    if (NULL == pipv6)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pipv6[0] = 0xfe;                    // 第 0 个字节 fe
    pipv6[1] = 0x80;                    // 第 1 个字节 80
    memcpy(&pipv6[8], pmac, 3);         // 8 - 10 字节拷贝 mac 的前 3 个字节
    pipv6[11] = 0xff;                   // 第 11 字节固定为 ff
    pipv6[12] = 0xfe;                   // 第 12 字节固定为 fe
    memcpy(&pipv6[13], &pmac[3], 3);    // 13 - 15 字节拷贝 mac 的后 3 个字节

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口主 IP 地址添加（IPv6）
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:47:13
 * @note       :
 */
int route_if_add_v6_ip(struct route_if *pif)
{
    struct route_if *pif_new    = NULL;
    struct route_if *pif_lookup = NULL;
    struct route_if  pif_router_id;

    ROUTE_LOG_DEBUG();

    pif_lookup = route_if_lookup(pif->ifindex);
    if (NULL == pif_lookup)
    {
        pif_new = route_if_create(pif);
        if (NULL == pif_new)
        {
            ROUTE_LOG_ERROR("create route if\n");

            return ERRNO_MALLOC;
        }

        if (ROUTE_IF_IPV6_EXIST(pif_new->intf.ipv6[0].addr))
        {
            /*添加ipv6 地址时初始化接口nd 控制信息*/
            if (pif->link_local_flag == DISABLE)
                ROUTE_NDPIF_CFG_INIT(pif_new, pif->ifindex);

			route_if_notify(pif_new, IPC_OPCODE_ADD, IFNET_EVENT_IP_ADD,
                            pif_new->intf.ipv6_flag, L3IF_INFO_IPV6);
        }
    }
    else
    {
        pif->intf.vpn = pif_lookup->intf.vpn;
        pif_lookup->intf.ipv6_flag = pif->intf.ipv6_flag;

        if (ROUTE_IF_IPV6_EXIST(pif_lookup->intf.ipv6[0].addr)
            && ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))    // 主 IP 已配置
        {
            memcpy(&pif_router_id, pif_lookup, sizeof(struct route_if));

            route_if_update_ip(pif_lookup, pif, pif->intf.ipv6_flag, L3IF_INFO_IPV6);

            /* 更新 routerv6-id */
            if (0 == memcmp(g_route.routerv6_id.ipv6, pif_router_id.intf.ipv6[0].addr, IPV6_ADDR_LEN))
            {
                memset(g_route.routerv6_id.ipv6, 0, IPV6_ADDR_LEN);
            }
        }
        else                                                // 主 IP 未配置
        {
            if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
            {

                if (pif->link_local_flag == DISABLE)
                    ROUTE_NDPIF_CFG_INIT(pif_lookup, pif->ifindex);

                pif_lookup->intf.ipv6[0] = pif->intf.ipv6[0];

                route_if_notify(pif_lookup, IPC_OPCODE_ADD, IFNET_EVENT_IP_ADD,
                                pif_lookup->intf.ipv6_flag, L3IF_INFO_IPV6);
            }
        }
    }

    /* 生成 routerv6-id */
    routev6_set_router_id();

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口从 IP 地址添加（IPv6）
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:48:34
 * @note       : 从地址添加时，主 IP 地址必须存在
 */
static int route_if_add_slave_v6_ip(struct route_if *pif)
{
    struct route_if *pif_lookup = NULL;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pif_lookup = route_if_lookup(pif->ifindex);
    if (pif_lookup == NULL)
    {
        return ERRNO_FAIL;
    }

    if (!ROUTE_IF_IPV6_EXIST(pif_lookup->intf.ipv6[1].addr))     // 从 IP 未配置
    {
        pif_lookup->intf.ipv6[1] = pif->intf.ipv6[1];

        route_if_notify(pif_lookup, IPC_OPCODE_ADD, IFNET_EVENT_IP_ADD,
                        IP_TYPE_SLAVE, L3IF_INFO_IPV6);
    }
    else                                                         // 从 IP 已配置
    {
        pif->intf.vpn = pif_lookup->intf.vpn;

        route_if_update_ip(pif_lookup, pif, IP_TYPE_SLAVE, L3IF_INFO_IPV6);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口链路本地地址添加
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:49:25
 * @note       :
 */
static int route_if_add_link_local(struct route_if *pif)
{
    struct ifm_event levent;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
    {
        return ERRNO_SUCCESS;
    }

    if (pif->down_flag == LINK_UP)
    {
        route_if_add_v6_link_routing(pif);
    }

    if (pif->intf.ipv6_flag == IP_TYPE_INVALID)
    {
        ROUTE_LOG_DEBUG();
        ROUTE_NDPIF_CFG_INIT(pif, pif->ifindex);
    }

    route_if_send_to_ftm(pif, IPC_OPCODE_ADD, L3IF_INFO_IPV6_LINK_LOCAL);
    route_if_set_event(pif, &levent, IFNET_EVENT_IP_ADD, IP_TYPE_LINK_LOCAL, L3IF_INFO_IPV6_LINK_LOCAL);
    l3if_event_notify(levent.event, &levent);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口主 IP 地址删除（IPv6）
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:50:26
 * @note       : 删除主 IP 地址时将从 IP 地址一并清除
 */
int route_if_delete_v6_ip(struct route_if *pif)
{
    struct route_if lif;

    ROUTE_LOG_DEBUG();

    if (pif == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memcpy(&lif, pif, sizeof(struct route_if));

    /* 从 IP 存在删除从 IP */
    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
    {
        route_if_notify(pif, IPC_OPCODE_DELETE, IFNET_EVENT_IP_DELETE,
                        IP_TYPE_SLAVE, L3IF_INFO_IPV6);
    }

    /* 主 IP 存在删除主 IP */
    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
    {
		route_if_notify(pif, IPC_OPCODE_DELETE, IFNET_EVENT_IP_DELETE,
                        pif->intf.ipv6_flag, L3IF_INFO_IPV6);
    }

    /* 清除 IP 标志 */
    ROUTE_IF_VAL_RESET(pif->intf.ipv6_flag);

    /* 更新 routerv6-id */
    if (0 == memcmp(g_route.routerv6_id.ipv6, lif.intf.ipv6[0].addr, IPV6_ADDR_LEN)
        || 0 == memcmp(g_route.routerv6_id.ipv6, lif.intf.ipv6[1].addr, IPV6_ADDR_LEN))
    {
        memset(g_route.routerv6_id.ipv6, 0, IPV6_ADDR_LEN);

        routev6_set_router_id();
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口从 IP 地址删除（IPv6）
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:51:22
 * @note       :
 */
static int route_if_delete_slave_v6_ip(struct route_if *pif)
{
    ROUTE_LOG_DEBUG();

    if (pif == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
    {
        route_if_notify(pif, IPC_OPCODE_DELETE, IFNET_EVENT_IP_DELETE,
                        IP_TYPE_SLAVE, L3IF_INFO_IPV6);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口链路本地地址删除
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:51:42
 * @note       :
 */
static int route_if_delete_link_local(struct route_if *pif)
{
    struct ifm_event levent;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
    {
        return ERRNO_SUCCESS;
    }

    if (pif->down_flag == LINK_UP)
    {
        route_if_delete_v6_link_routing(pif);
    }

    route_if_send_to_ftm(pif, IPC_OPCODE_DELETE, L3IF_INFO_IPV6_LINK_LOCAL);
    route_if_set_event(pif, &levent, IFNET_EVENT_IP_DELETE, IP_TYPE_LINK_LOCAL, L3IF_INFO_IPV6_LINK_LOCAL);
    l3if_event_notify(levent.event, &levent);

    memset(pif->intf.ipv6_link_local.addr, 0, IPV6_ADDR_LEN);
    pif->link_local_flag = DISABLE;

    return ERRNO_SUCCESS;
}



/**
 * @brief      : 接口 vpn 配置
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:52:51
 * @note       :
 */
static int route_if_add_vpn(struct route_if *pif)
{
    struct route_if *pif_new    = NULL;
    struct route_if *pif_lookup = NULL;
    struct ifm_event levent;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pif_lookup = route_if_lookup(pif->ifindex);
    if (NULL == pif_lookup)
    {
        pif_new = route_if_create(pif);
        if (NULL == pif_new)
        {
            ROUTE_LOG_ERROR("create route if\n");

            return ERRNO_MALLOC;
        }

        pif_lookup = pif_new;
    }

    pif_lookup->intf.vpn = pif->intf.vpn;

    route_if_send_to_ftm(pif_lookup, IPC_OPCODE_ADD, L3IF_INFO_VPN);
    route_if_set_event(pif_lookup, &levent, IFNET_EVENT_L3VPN, IP_TYPE_INVALID, L3IF_INFO_VPN);
    l3if_event_notify(levent.event, &levent);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口 vpn 删除
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:54:11
 * @note       :
 */
static int route_if_delete_vpn(struct route_if *pif)
{
    struct ifm_event levent;

    ROUTE_LOG_DEBUG();

    if (pif == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (0 != pif->intf.vpn)
    {
        ROUTE_IF_VAL_RESET(pif->intf.vpn);

        route_if_send_to_ftm(pif, IPC_OPCODE_DELETE, L3IF_INFO_VPN);
        route_if_set_event(pif, &levent, IFNET_EVENT_L3VPN, IP_TYPE_INVALID, L3IF_INFO_VPN);
        l3if_event_notify(levent.event, &levent);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口 urpf 配置
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:54:37
 * @note       :
 */
static int route_if_add_urpf(struct route_if *pif)
{
    struct route_if *pif_lookup = NULL;
    struct route_if *pif_new    = NULL;
    int ret = ERRNO_SUCCESS;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pif_lookup = route_if_lookup(pif->ifindex);
    if (NULL == pif_lookup)
    {
        pif_new = route_if_create(pif);
        if (NULL == pif_new)
        {
            ROUTE_LOG_ERROR("Malloc failed\n");

            return ERRNO_MALLOC;
        }

        pif_lookup = pif_new;
    }

    pif_lookup->urpf = pif->urpf;

    ret = ROUTE_IPC_SENDTO_HAL(&pif_lookup->urpf, 4, 1, MODULE_ID_HAL, MODULE_ID_ROUTE,
                        IPC_TYPE_IFM, IFNET_INFO_URPF, IPC_OPCODE_UPDATE, pif_lookup->ifindex);
    if (ERRNO_SUCCESS != ret)
    {
        ROUTE_LOG_ERROR("IPC send failed.\n");

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 接口 urpf 删除
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:55:08
 * @note       :
 */
static int route_if_delete_urpf(struct route_if *pif)
{
    int ret = ERRNO_SUCCESS;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (pif->urpf != URPF_TYPE_INVALID)
    {
        pif->urpf = URPF_TYPE_INVALID;

        ret = ROUTE_IPC_SENDTO_HAL(&pif->urpf, 4, 1, MODULE_ID_HAL, MODULE_ID_ROUTE,
                            IPC_TYPE_IFM, IFNET_INFO_URPF, IPC_OPCODE_UPDATE, pif->ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            ROUTE_LOG_ERROR("IPC send failed.\n");

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 删除三层接口下所有配置
 * @param[in ] : pif - route 模块接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:55:33
 * @note       :
 */
int route_if_delete_config(struct route_if *pif)
{
    struct route_if *pif_lookup = NULL;
    struct list     *plist      = NULL;

    ROUTE_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    route_static_process_routing(pif->ifindex);

    pif_lookup = route_if_lookup(pif->ifindex);
    if (NULL == pif_lookup)
    {
        return ERRNO_SUCCESS;
    }

    route_if_delete_ip(pif_lookup);
    route_if_delete_v6_ip(pif_lookup);
    route_if_delete_link_local(pif_lookup);
    route_if_delete_urpf(pif_lookup);
    route_if_delete_vpn(pif_lookup);

    plist = &pif_lookup->unnumbered_list;
    if (list_isempty(plist))
    {
        route_if_delete(pif_lookup->ifindex);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 三层接口响应 l3vpn 实例删除处理
 * @param[in ] : vrf_id - vpn 实例
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:56:14
 * @note       : 清除接口 vpn 配置及基本信息配置
 */
void route_if_process_l3vpn_delete(uint16_t vrf_id)
{
    struct route_if    *pif     = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;

    HASH_BUCKET_LOOP(pbucket, cursor, route_if_table)
    {
        pif = (struct route_if *)pbucket->data;
        if (NULL == pif)
        {
            continue;
        }

        if (pif->intf.vpn == vrf_id)
        {
            route_if_delete_ip(pif);
            route_if_delete_v6_ip(pif);
            route_if_delete_urpf(pif);
            route_if_delete_link_local(pif);
            route_if_delete_vpn(pif);
        }
    }

    return;
}


/**
 * @brief      : 接口 shutdown 命令
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 14:57:35
 * @note       : 只记录子接口 shutdown 状态，无需 ifm 通知子接口状态，减少 IPC 通信
 */
DEFUN(subif_shutdown,
    subif_shutdown_cmd,
    "shutdown",
    ROUTE_CLI_INFO)
{
    struct route_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    if ((!(IFM_TYPE_IS_METHERNET(ifindex) && IFM_IS_SUBPORT(ifindex)))
        && (!(IFM_TYPE_IS_TRUNK(ifindex) && IFM_IS_SUBPORT(ifindex))))
    {
        return CMD_SUCCESS;
    }

    pif = route_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if ((1 == pif->shutdown_flag) || (LINK_DOWN == pif->down_flag))
    {
        pif->shutdown_flag = 1;

        return CMD_SUCCESS;
    }

    pif->shutdown_flag = 1;

    route_if_down(ifindex);

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口 no shutdown 命令
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:00:35
 * @note       : 只记录子接口 shutdown 状态，无需 ifm 通知子接口状态，减少 IPC 通信
 */
DEFUN(no_subif_shutdown,
    no_subif_shutdown_cmd,
    "no shutdown",
    ROUTE_CLI_INFO)
{
    struct route_if *pif        = NULL;
    struct route_if *pif_parent = NULL;
    struct route_if  lif;
    uint32_t ifindex        = (uint32_t)vty->index;
    uint32_t parent_ifindex = 0;

    if ((!(IFM_TYPE_IS_METHERNET(ifindex) && IFM_IS_SUBPORT(ifindex)))
        && (!(IFM_TYPE_IS_TRUNK(ifindex) && IFM_IS_SUBPORT(ifindex))))
    {
        return CMD_SUCCESS;
    }

    pif = route_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if ((0 == pif->shutdown_flag) || (LINK_UP == pif->down_flag))
    {
        pif->shutdown_flag = 0;

        return CMD_SUCCESS;
    }

    pif->shutdown_flag = 0;

    parent_ifindex = IFM_PARENT_IFINDEX_GET(ifindex);

    pif_parent = route_if_lookup(parent_ifindex);
    if (NULL == pif_parent)
    {
        memset(&lif, 0, sizeof(struct route_if));

        lif.ifindex = parent_ifindex;
        pif_parent  = route_if_create(&lif);
    }

    if ((NULL != pif_parent) && (LINK_UP == pif_parent->down_flag))
    {
        if (1 == pif->encap_flag)
        {
            route_if_up(ifindex);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口封装配置
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:02:13
 * @note       : 只记录子接口封装，无需 ifm 通知子接口状态，减少 IPC 通信
 */
DEFUN(subif_encapsulate,
    subif_encapsulate_untag_cmd,
    "encapsulate untag",
    ROUTE_CLI_INFO)
{
    struct route_if *pif        = NULL;
    struct route_if *pif_parent = NULL;
    struct route_if  lif;
    uint32_t ifindex        = (uint32_t)vty->index;
    uint32_t parent_ifindex = 0;

    if (!(IFM_TYPE_IS_METHERNET(ifindex) && IFM_IS_SUBPORT(ifindex))
        && !(IFM_TYPE_IS_TRUNK(ifindex) && IFM_IS_SUBPORT(ifindex)))
    {
        return CMD_SUCCESS;
    }

    pif = route_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if ((1 == pif->encap_flag) || (LINK_UP == pif->down_flag))
    {
        pif->encap_flag = 1;

        return CMD_SUCCESS;
    }

    pif->encap_flag = 1;

    parent_ifindex = IFM_PARENT_IFINDEX_GET(ifindex);

    pif_parent = route_if_lookup(parent_ifindex);
    if (NULL == pif_parent)
    {
        memset(&lif, 0, sizeof(struct route_if));

        lif.ifindex = parent_ifindex;
        pif_parent  = route_if_create(&lif);
    }

    if ((NULL != pif_parent) && (LINK_UP == pif_parent->down_flag))
    {
        if (0 == pif->shutdown_flag)
        {
            route_if_up(ifindex);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口封装删除
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:05:04
 * @note       : 只记录子接口封装，无需 ifm 通知子接口状态，减少 IPC 通信
 */
DEFUN(no_subif_encapsulate,
    no_subif_encapsulate_cmd,
    "no encapsulate",
    ROUTE_CLI_INFO)
{
    struct route_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    if (!(IFM_TYPE_IS_METHERNET(ifindex) && IFM_IS_SUBPORT(ifindex))
        && !(IFM_TYPE_IS_TRUNK(ifindex) && IFM_IS_SUBPORT(ifindex)))
    {
        return CMD_SUCCESS;
    }

    pif = route_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if ((0 == pif->encap_flag) || (LINK_DOWN == pif->down_flag))
    {
        pif->encap_flag = 0;

        return CMD_SUCCESS;
    }

    pif->encap_flag = 0;

    route_if_down(ifindex);

    return CMD_SUCCESS;
}


/* --- 其他接口封装命令注册，开始 --- */

ALIAS(subif_encapsulate,
    subif_encapsulate_dot1q_cmd,
    "encapsulate dot1q <1-4094> {to <1-4094>}",
    "\n")


ALIAS(subif_encapsulate,
    subif_encapsulate_qinq_cmd,
    "encapsulate qinq svlan <1-4094> cvlan <1-4094> {to <1-4094>}",
    "\n")


ALIAS(subif_encapsulate,
    subif_encapsulate_untag_add_cmd,
    "encapsulate untag add vlan <1-4094> cos <0-7>",
    "\n")


ALIAS(subif_encapsulate,
    subif_encapsulate_dot1q_add_cmd,
    "encapsulate dot1q <1-4094> {to <1-4094>} add svlan <1-4094> {cos <0-7>}",
    "\n")


ALIAS(subif_encapsulate,
    subif_encapsulate_dot1q_translate_single_cmd,
    "encapsulate dot1q <1-4094> translate vlan <1-4094> {cos <0-7>}",
    "\n")


ALIAS(subif_encapsulate,
    subif_encapsulate_dot1q_translate_double_cmd,
    "encapsulate dot1q <1-4094> translate svlan <1-4094> {cos <0-7>} cvlan <1-4094> {cos <0-7>}",
    "\n")


ALIAS(subif_encapsulate,
    subif_encapsulate_qinq_translate_single__cmd,
    "encapsulate qinq svlan <1-4094> cvlan <1-4094> translate vlan <1-4094> {cos <0-7>}",
    "\n")


ALIAS(subif_encapsulate,
    subif_encapsulate_qinq_translate_double_cmd,
    "encapsulate qinq svlan <1-4094> cvlan <1-4094> translate svlan <1-4094> {cos <0-7>} cvlan <1-4094> {cos <0-7>}",
    "\n")


ALIAS(subif_encapsulate,
    subif_encapsulate_qinq_translate_outer_cmd,
    "encapsulate qinq svlan <1-4094> cvlan <1-4094> translate svlan <1-4094> {cos <0-7>} cvlan *",
    "\n")


ALIAS(subif_encapsulate,
    subif_encapsulate_qinq_translate_delete_cmd,
    "encapsulate qinq svlan <1-4094> cvlan <1-4094> {to <1-4094>} delete svlan",
    "\n")

/* --- 其他接口封装命令注册，结束 --- */


/**
 * @brief      : 接口 urpf 配置命令
 * @param[in ] : strict - 严格模式
 * @param[in ] : loose  - 松散模式
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:07:31
 * @note       : 松散模式检查报文的源 IP 地址，在本地路由表中是否可达
 * @note       : 严格模式检查报文的源 IP 地址，在本地路由表中是否可达并且出接口是否是报文入口
 */
DEFUN(ip_urpf,
    ip_urpf_cmd,
    "ip urpf (strict|loose)",
    ROUTE_CLI_INFO)
{
    struct route_if lif;
    uint32_t ifindex = (uint32_t)vty->index;
    int ret;

    memset(&lif, 0, sizeof(struct route_if));
    lif.ifindex = ifindex;

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    if (argv[0][0] == 's')
    {
        lif.urpf = URPF_TYPE_STRICT;
    }
    else if (argv[0][0] == 'l')
    {
        lif.urpf = URPF_TYPE_LOOSE;
    }

    ret = route_if_add_urpf(&lif);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口 urpf 删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:10:37
 * @note       :
 */
DEFUN(no_ip_urpf,
    no_ip_urpf_cmd,
    "no ip urpf",
    ROUTE_CLI_INFO)
{
    struct route_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    int ret;

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    pif = route_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    ret = route_if_delete_urpf(pif);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口 vpn 配置命令
 * @param[in ] : <1-128> - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:11:23
 * @note       : 接口配置 vpn 时，必须先创建 vpn 实例
 * @note       : 接口相关配置全部清除
 */
DEFUN(ip_l3vpn,
    ip_l3vpn_cmd,
    "ip l3vpn <1-128>",
    ROUTE_CLI_INFO)
{
    struct route_if    *pif    = NULL;
    struct route_if     lif;
	uint16_t vpn_temp = 0;
    int ret;

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    /* argv[0]: vrf id */
    lif.intf.vpn = atoi(argv[0]);

    vpn_temp = mpls_com_get_l3vpn_instance2(lif.intf.vpn, MODULE_ID_ROUTE);
	
    if (vpn_temp == 0)
    {
        vty_error_out(vty, "VPN-Instance does not exist.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    pif = route_if_lookup((uint32_t)vty->index);
    if (NULL != pif)
    {
        if (0 != pif->intf.vpn)
        {
            vty_error_out(vty, "The interface is bound with another VPN instance.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (pif->intf.ipv4_flag == IP_TYPE_DHCP)
        {
            ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_DHCP, MODULE_ID_ROUTE,
                                    IPC_TYPE_L3IF, IFNET_EVENT_IP_DELETE,
                                    IPC_OPCODE_EVENT, (uint32_t)vty->index);
            if (ERRNO_SUCCESS != ret)
            {
                vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }

        if (pif->intf.ipv6_flag == IP_TYPE_DHCP)
        {
            ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_DHCPV6, MODULE_ID_ROUTE,
                                    IPC_TYPE_L3IF, IFNET_EVENT_IP_DELETE,
                                    IPC_OPCODE_EVENT, (uint32_t)vty->index);
            if (ERRNO_SUCCESS != ret)
            {
                vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

                return CMD_WARNING;
            }
        }

        route_if_delete_ip(pif);
        route_if_delete_urpf(pif);
        route_if_delete_v6_ip(pif);
        route_if_delete_link_local(pif);
    }

    memset(&lif, 0, sizeof(struct route_if));
    lif.ifindex  = (uint32_t)vty->index;
    lif.intf.vpn = vpn_temp;

    ret = route_if_add_vpn(&lif);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    vty_out(vty, "Some configurations on the interface are removed.%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口 vpn 删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:16:16
 * @note       : 接口相关配置全部清除
 */
DEFUN(no_ip_l3vpn,
    no_ip_l3vpn_cmd,
    "no ip l3vpn",
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

    if (0 == pif->intf.vpn)
    {
        return CMD_SUCCESS;
    }

    if (pif->intf.ipv4_flag == IP_TYPE_DHCP)
    {
        ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_DHCP, MODULE_ID_ROUTE,
                                IPC_TYPE_L3IF, IFNET_EVENT_IP_DELETE,
                                IPC_OPCODE_EVENT, (uint32_t)vty->index);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    if (pif->intf.ipv6_flag == IP_TYPE_DHCP)
    {
        ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_DHCPV6, MODULE_ID_ROUTE,
                                IPC_TYPE_L3IF, IFNET_EVENT_IP_DELETE,
                                IPC_OPCODE_EVENT, (uint32_t)vty->index);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    route_if_delete_ip(pif);
    route_if_delete_urpf(pif);
    route_if_delete_v6_ip(pif);
    route_if_delete_link_local(pif);

    ret = route_if_delete_vpn(pif);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    vty_out(vty, "Some configurations on the interface are removed.%s", VTY_NEWLINE);

    return CMD_SUCCESS;
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
DEFUN(ip_addr,
    ip_addr_cmd,
    "ip address A.B.C.D/M [slave]",
    ROUTE_CLI_INFO)
{
    struct route_if *pif_lookup = NULL;
    struct route_if lif;
    struct prefix_ipv4 ip_addr;
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

    if (NULL == argv[1])
    {
        /* DHCP 使能不允许配置静态接口主 ip */
        if ((NULL != pif_lookup) && ((pif_lookup->intf.ipv4_flag == IP_TYPE_DHCP) ||(pif_lookup->intf.ipv4_flag == IP_TYPE_DHCP_ZERO) ))
        {
            vty_error_out(vty, "DHCP or DHCP zero nabled. Can't configure IP.%s", VTY_NEWLINE);

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

    /* argv[0] : IP 地址和掩码长度 */
    ret = str2prefix_ipv4(argv[0], &ip_addr);
    if (0 == ret)
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

    if (NULL == argv[1])
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

    if (NULL == argv[1])
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
 * @brief      : 接口 IP 地址自动获取配置命令（IPv4）
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:23:21
 * @note       : 该命令与静态 IP 命令不可同时配置
 */
DEFUN (ip_address_dhcp_alloc,
    ip_address_dhcp_alloc_cmd,
    "ip address dhcp",
    ROUTE_CLI_INFO)
{
    struct route_if *pif = NULL;
    struct route_if lif;
    int ret;

    memset(&lif, 0, sizeof(struct route_if));

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    pif = route_if_lookup((uint32_t)vty->index);
    if ((pif != NULL) && ((pif->intf.ipv4_flag == IP_TYPE_STATIC)
                            || (pif->intf.ipv4_flag == IP_TYPE_UNNUMBERED)))
    {
        /* 接口已经配置静态 IP 或借用 IP 不允许配置自动获取命令 */
        vty_error_out(vty, "The interface IP address already exist. "
                        "Can't enable the DHCP.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }
	if ((pif != NULL) && (pif->intf.ipv4_flag == IP_TYPE_DHCP_ZERO))
	{
		/* 接口已经配置DHCP零配置不允许配置自动获取命令 */
		vty_error_out(vty, "The interface already enable dhcp zero. "
						"Can't enable the DHCP.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

    ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_DHCP, MODULE_ID_ROUTE, IPC_TYPE_L3IF,
                            IFNET_EVENT_IP_ADD, IPC_OPCODE_EVENT, (uint32_t)vty->index);
    if (0 != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口 IP 地址自动获取固话配置命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:26:56
 * @note       : 执行该命令必须保证 IP 已经自动获取到
 */
DEFUN (ip_address_dhcp_save,
    ip_address_dhcp_save_cmd,
    "ip address dhcp save",
    ROUTE_CLI_INFO)
{
    struct route_if *pif = NULL;
    struct route_if lif;
    int ret;

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    pif = route_if_lookup((uint32_t)vty->index);
    if (pif == NULL)
    {
        if (1 == vty->config_read_flag)
        {
            memset(&lif, 0, sizeof(struct route_if));
            lif.ifindex = (uint32_t)vty->index;

            pif = route_if_create(&lif);
            if (NULL == pif)
            {
                return CMD_SUCCESS;
            }
        }
        else
        {
            return CMD_SUCCESS;
        }
    }

    if (1 != vty->config_read_flag)
    {
        if (((pif->intf.ipv4_flag != IP_TYPE_DHCP)
                || (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr)))
            && (pif->intf.ipv4_flag != IP_TYPE_DHCP_ZERO))
        {
            vty_error_out(vty, "%s%s", "IP address dose not exist !", VTY_NEWLINE);

            return CMD_SUCCESS;
        }
    }

    ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_DHCP, MODULE_ID_ROUTE, IPC_TYPE_L3IF,
                            IFNET_EVENT_IP_ADD, IPC_OPCODE_UPDATE, (uint32_t)vty->index);
    if (0 != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    pif->dhcp_save_flag = ENABLE;

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口借用 IP 地址配置命令
 * @param[in ] : ethernet USP         - 出接口类型为 ethernet
 * @param[in ] : gigabitethernet USP  - 出接口类型为 gigabitethernet
 * @param[in ] : xgigabitethernet USP - 出接口类型为 xgigabitethernet
 * @param[in ] : tunnel USP           - 出接口类型为 tunnel
 * @param[in ] : loopback <0-128>     - 出接口类型为 loopback
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:28:10
 * @note       :
 */
DEFUN (ip_addr_unnumbered,
    ip_addr_unnumbered_cmd,
    "ip address unnumbered interface {ethernet USP | gigabitethernet USP "
    "| xgigabitethernet USP | tunnel USP | loopback <0-128>}",
    ROUTE_CLI_INFO)
{
    struct route_if  lif;
    struct route_if *pif        = NULL;
    struct route_if *pif_new    = NULL;
    struct route_if *pif_lookup = NULL;
    const char      *pintf      = NULL;
    const char      *pintf_num  = NULL;
    struct ifm_event event;
    uint32_t ifindex = 0;
    int      ret;

    memset(&lif, 0, sizeof(struct route_if));
    memset(&event, 0, sizeof(struct ifm_event));

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    lif.ifindex = (uint32_t)vty->index;

    /*
     * 配置借用接口类型，最多可选一
     * argv[0]: Ethernet
     * argv[1]: Gigabitethernet
     * argv[2]: Xgigabitethernet
     * argv[3]: Tunnel
     * argv[4]: Loopback
     */
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

    if (NULL != pintf)
    {
        ifindex = ifm_get_ifindex_by_name(pintf, pintf_num);
        if (ifindex == 0)
        {
            vty_error_out(vty, "Specify the illegal interface.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    lif.intf.unnumbered_if = ifindex;
    lif.intf.ipv4_flag     = IP_TYPE_UNNUMBERED;

    pif = route_if_lookup(lif.ifindex);
    if (NULL == pif)
    {
        pif = route_if_create(&lif);
        if (NULL == pif)
        {
            vty_error_out(vty, "route if create failed.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else
    {
        if ((ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr)) ||
            (pif->intf.ipv4_flag != IP_TYPE_INVALID))
        {
            vty_error_out(vty, "Interface IP already exist.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        pif->intf.ipv4_flag     = lif.intf.ipv4_flag;
        pif->intf.unnumbered_if = ifindex;
    }

    /*借用ip 初始化arp 控制结构*/
    ROUTE_ARPIF_CFG_INIT(pif, pif->ifindex);

    route_if_send_to_ftm(pif, IPC_OPCODE_ADD, L3IF_INFO_IPV4);

    /* 将借用者存储到被借用者链表中 */
    pif_lookup = route_if_lookup(ifindex);
    if (NULL != pif_lookup)
    {
        event.ipaddr.type      = INET_FAMILY_IPV4;
        event.ipaddr.addr.ipv4 = pif_lookup->intf.ipv4[0].addr;
        event.ipaddr.prefixlen = pif_lookup->intf.ipv4[0].prefixlen;

        pif_new = pif_lookup;
    }
    else
    {
        memset(&lif, 0, sizeof(struct route_if));
        lif.ifindex = ifindex;

        pif_new = route_if_create(&lif);
        if (NULL == pif_new)
        {
            ROUTE_LOG_ERROR("malloc\n");
        }
    }

    if (NULL != pif_new)
    {
        listnode_add(&pif_new->unnumbered_list, (void *)pif->ifindex);
    }

    /* 通知其他模块接口 IP 为借用 */
    event.event   = IFNET_EVENT_IP_ADD;
    event.up_flag = pif->down_flag;
    event.ifindex = pif->ifindex;
    event.mode    = IFNET_MODE_L3;
    event.ipflag  = IP_TYPE_UNNUMBERED;
    event.vpn     = pif->intf.vpn;

    l3if_event_notify(event.event, &event);

    return CMD_SUCCESS;
}


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
    "no ip address [slave]",
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
 * @brief      : 接口 IP 地址自动获取删除命令（IPv4）
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:40:00
 * @note       :
 */
DEFUN(no_ip_addr_dhcp,
    no_ip_addr_dhcp_cmd,
    "no ip address dhcp",
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
    if ((NULL != pif) && ((pif->intf.ipv4_flag == IP_TYPE_STATIC) || (pif->intf.ipv4_flag == IP_TYPE_DHCP_ZERO)))
    {
        return CMD_SUCCESS;
    }

    if (NULL != pif)
    {
        if (ENABLE == pif->dhcp_save_flag)
        {
            vty_error_out(vty, "DHCP address already save, Please no save ! %s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_DHCP, MODULE_ID_ROUTE,
                            IPC_TYPE_L3IF, IFNET_EVENT_IP_DELETE,
                            IPC_OPCODE_EVENT, (uint32_t)vty->index);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != pif)
    {
        if (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr)
            && (!ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
            && (pif->intf.ipv4_flag == IP_TYPE_INVALID))
        {
            return CMD_SUCCESS;
        }

        ret = route_if_delete_ip(pif);
        if (ERRNO_SUCCESS != ret)
        {
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}


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

    if ((pif->intf.ipv4_flag != IP_TYPE_DHCP) && (pif->intf.ipv4_flag != IP_TYPE_DHCP_ZERO))
    {
        return CMD_SUCCESS;
    }

    ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_DHCP, MODULE_ID_ROUTE,
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


/**
 * @brief      : 接口链路本地地址自动生成配置命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:44:23
 * @note       : 根据接口 MAC 地址自动生成，手动配置链路本地地址优先级高于自动生成地址
 */
DEFUN(ipv6_addr_auto_link_local,
    ipv6_addr_auto_link_local_cmd,
    "ipv6 address auto link-local",
    ROUTE_CLI_INFO)
{
    struct route_if *pif = NULL;
    struct route_if  lif;
    uint8_t ipv6[16] = "";
    uint8_t  pmac[6];
    int ret;

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    memset(&lif, 0, sizeof(struct route_if));

    lif.ifindex      = (uint32_t)vty->index;
    lif.intf.ifindex = lif.ifindex;

    if (ifm_get_mac(lif.ifindex, MODULE_ID_ROUTE,  pmac) != 0)
    {
        vty_error_out(vty, "Get MAC address failed !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    route_if_get_linklocal(ipv6, pmac);
    memcpy(lif.intf.ipv6_link_local.addr, ipv6, IPV6_ADDR_LEN);

    pif = route_if_lookup(lif.ifindex);
    if (NULL == pif)
    {
        pif = route_if_create(&lif);
        if (NULL == pif)
        {
            vty_error_out(vty, "route if create failed.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else
    {
        if ((pif->link_local_flag == ENABLE)
            || (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr)))
        {
            vty_error_out(vty, "The address has already existed on this interface.%s", VTY_NEWLINE);

            return CMD_SUCCESS;
        }

        memcpy(pif->intf.ipv6_link_local.addr, lif.intf.ipv6_link_local.addr, IPV6_ADDR_LEN);
    }

    pif->link_local_flag = ENABLE;

    ret = route_if_add_link_local(pif);
    if (ERRNO_SUCCESS != ret)
    {
        pif->link_local_flag = DISABLE;

        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief	   : 自动生成eui-64格式的全球单播地址
 * @param[in ] : X:X:X:X:X:X:X:X - 全球单播地址
 * @param[in ] : <1-128> - 掩码长度
 * @param[out] :
 * @return	   : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author	   : ZhouDW
 * @date	   : 2019年4月9日 15:47:53
 */
DEFUN(ipv6_addr_auto_generate,
	ipv6_addr_auto_generate_cmd,
	"ipv6 address X:X:X:X:X:X:X:X <1-64> eui-64",
	ROUTE_CLI_INFO)
{
	struct prefix_ipv6 ipv6_address;
	struct prefix_ipv6 ipv6_check;
	struct prefix_ipv6 mask_addr;
	struct route_if   *pif_lookup = NULL;
    struct route_if    lif;
	struct ipv6_addr ipv6_add;
	unsigned char buf[64] ;
	unsigned char pmac[6];
	int mask_lenth = 0;
	int ret = -1;

	
	if(route_if_get_intf_mode(vty) != ERRNO_SUCCESS)
	{
		return CMD_WARNING;
	}
	
	memset(buf , 0 , 64);
	inet_pton6(argv[0], ipv6_add.ipv6);

	if(ifm_get_mac((uint32_t)vty->index, MODULE_ID_ROUTE, pmac))
	{
        vty_error_out(vty, "Get MAC address failed !%s", VTY_NEWLINE);

        return CMD_WARNING;
	}

	memcpy(&ipv6_add.ipv6[8] , pmac , 3);
	ipv6_add.ipv6[11] = 0xff;
	ipv6_add.ipv6[12] = 0xfe;
	memcpy(&ipv6_add.ipv6[13] , &pmac[3] , 3);
	
    sprintf(buf, "%X:%X:%X:%X:%X:%X:%X:%X",  ((uint16_t)ipv6_add.ipv6[0] << 8) + ipv6_add.ipv6[1],
                                            ((uint16_t)ipv6_add.ipv6[2] << 8) + ipv6_add.ipv6[3],
                                            ((uint16_t)ipv6_add.ipv6[4] << 8) + ipv6_add.ipv6[5],
                                            ((uint16_t)ipv6_add.ipv6[6] << 8) + ipv6_add.ipv6[7],
                                            ((uint16_t)ipv6_add.ipv6[8] << 8) + ipv6_add.ipv6[9],
                                            ((uint16_t)ipv6_add.ipv6[10] << 8) + ipv6_add.ipv6[11],
                                            ((uint16_t)ipv6_add.ipv6[12] << 8) + ipv6_add.ipv6[13],
                                            ((uint16_t)ipv6_add.ipv6[14] << 8) + ipv6_add.ipv6[15]);
											 

	mask_lenth = atoi(argv[1]);
	
    if (ERRNO_SUCCESS != routev6_check_ipv6_str((const char *)buf))
    {
        vty_error_out(vty, "The specified address is not a global-unicast address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (str2prefix_ipv6((const   char *)buf, &ipv6_address) == 0)
    {
        vty_error_out(vty, "The specified address is not a global-unicast address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }
	
	if(!(ROUTE_IF_IPV6_EXIST(ipv6_address.prefix.s6_addr)))
	{
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
	}
	
	 /* param shouldn't be link-local address , copy from h3c*/
    if (ipv6_is_linklocal((struct ipv6_addr *)&ipv6_address.prefix.s6_addr))
    {
        vty_error_out(vty, "Invalid attempt to assign a link-local address as a global unicast address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

	ipv6_address.prefixlen = mask_lenth;
    ipv6_check = ipv6_address;
	apply_mask_ipv6(&ipv6_check);
	masklen2ip6(mask_lenth, &mask_addr.prefix);
	
    if (0 == memcmp(&ipv6_check.prefix, &mask_addr.prefix, ((mask_lenth/8)+1)))
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    memset(&lif, 0, sizeof(struct route_if));
    lif.ifindex        = (uint32_t)vty->index;
    lif.intf.ipv6_flag = IP_TYPE_STATIC;
	
    pif_lookup = route_if_lookup((uint32_t)vty->index);

    /* DHCP 使能不允许配置静态接口主 IP */
    if ((NULL != pif_lookup) && (pif_lookup->intf.ipv6_flag == IP_TYPE_DHCP))
    {
        vty_error_out(vty, "DHCP is enabled. Can't configure IPv6 address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 接口借用 IP 不允许配置静态 IP */
    if ((NULL != pif_lookup) && (pif_lookup->intf.ipv6_flag == IP_TYPE_UNNUMBERED))
    {
        vty_error_out(vty, "Internet Address is unnumbered. Can't configure IPv6 address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    memcpy(lif.intf.ipv6[0].addr, &ipv6_address.prefix.s6_addr[0], IPV6_ADDR_LEN);
    lif.intf.ipv6[0].prefixlen = ipv6_address.prefixlen;

    /* 检查接口 IP 是否冲突 */
    if (NULL != pif_lookup)
    {
        lif.intf.vpn = pif_lookup->intf.vpn;
    }

    ret = route_if_check_v6_ip_conflict(&lif);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = route_if_add_v6_ip(&lif);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    return CMD_SUCCESS;	
}


/**
 * @brief      : 接口链路本地地址手动配置配置命令
 * @param[in ] : X:X:X:X:X:X:X:X - 链路本地地址
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:47:53
 * @note       : 手动配置链路本地地址优先级高于自动生成链路本地地址
 */
DEFUN(ipv6_addr_link_local,
    ipv6_addr_link_local_cmd,
    "ipv6 address X:X:X:X:X:X:X:X link-local",
    ROUTE_CLI_INFO)
{
    struct route_if   *pif = NULL;
    struct route_if    lif;
    struct prefix_ipv6 ip_addr;
    char *pnt = NULL;
    int ret;

    memset(&lif, 0, sizeof(struct route_if));

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    /* argv[0]: 链路本地地址 */
    pnt = strchr (argv[0], '/');
    if (NULL != pnt)
    {
        vty_error_out(vty, "The specified address is not a link-local address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = routev6_check_ipv6_str(argv[0]);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "The specified address is not a link-local address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = str2prefix_ipv6(argv[0], &ip_addr);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified address is not a link-local address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 站点本地地址 */
    if (!(ipv6_is_linklocal((struct ipv6_addr *)&ip_addr.prefix.s6_addr)
        && (!ipv6_is_sitelocal((struct ipv6_addr *)&ip_addr.prefix.s6_addr))))
    {
        vty_error_out(vty, "The specified address is not a link-local address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lif.ifindex = (uint32_t)vty->index;
    memcpy(lif.intf.ipv6_link_local.addr, ip_addr.prefix.s6_addr, IPV6_ADDR_LEN);

    pif = route_if_lookup(lif.ifindex);
    if (NULL == pif)
    {
        pif = route_if_create(&lif);
        if (NULL == pif)
        {
            vty_error_out(vty, "route if create failed.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else
    {
        if (0 == memcmp(lif.intf.ipv6_link_local.addr, pif->intf.ipv6_link_local.addr, IPV6_ADDR_LEN))
        {
            vty_error_out(vty, "The link-local address already exist !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        if ((pif->link_local_flag == ENABLE)
            || (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr)))
        {
            route_if_delete_link_local(pif);
        }

        memcpy(pif->intf.ipv6_link_local.addr, lif.intf.ipv6_link_local.addr, IPV6_ADDR_LEN);
    }

    ret = route_if_add_link_local(pif);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口链路本地地址自动生成删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:53:08
 * @note       :
 */
DEFUN(no_ipv6_addr_auto_link_local,
    no_ipv6_addr_auto_link_local_cmd,
    "no ipv6 address auto link-local",
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
    if ((NULL == pif) || (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr)))
    {
        if (NULL != pif)
        {
            pif->link_local_flag = DISABLE;
        }

        return CMD_SUCCESS;
    }

    route_if_delete_link_local(pif);

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口链路本地地址手动配置删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:54:52
 * @note       :
 */
DEFUN(no_ipv6_addr_link_local,
    no_ipv6_addr_link_local_cmd,
    "no ipv6 address link-local",
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
    if ((NULL == pif)
        || (pif->link_local_flag == ENABLE)
        || (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr)))
    {
        return CMD_SUCCESS;
    }

    route_if_delete_link_local(pif);

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口 IP 地址配置命令（IPv6）
 * @param[in ] : X:X:X:X:X:X:X:X/M - IPv6 地址和掩码长度
 * @param[in ] : slave             - 配置 IP 地址为从 IP
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 15:56:49
 * @note       :
 */
DEFUN(ipv6_addr,
    ipv6_addr_cmd,
    "ipv6 address X:X:X:X:X:X:X:X/M [slave]",
    ROUTE_CLI_INFO)
{
    struct route_if *pif_lookup = NULL;
    struct route_if    lif;
    struct prefix_ipv6 ip_addr;
    struct prefix_ipv6 ip_check;
    struct prefix_ipv6 mask_addr;
    char *pnt = NULL;
    int mask_len = 0;
    int ret;

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    /* argv[0]: IPv6 地址和掩码长度 */
    pnt = strchr (argv[0], '/');
    if (NULL == pnt)
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = routev6_check_ipv6_str(argv[0]);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 掩码长度检查 */
    mask_len = atoi(++pnt);
    if ((mask_len<1) || (mask_len>IPV6_MAX_PREFIXLEN))
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = str2prefix_ipv6(argv[0], &ip_addr);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 未指定地址 */
    if (!ROUTE_IF_IPV6_EXIST(ip_addr.prefix.s6_addr))
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* loopback 地址 */
    if (IN6_IS_ADDR_LOOPBACK(&ip_addr.prefix))
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 链路本地地址 */
    if ((ipv6_is_linklocal((struct ipv6_addr *)&ip_addr.prefix.s6_addr)
        && (!ipv6_is_sitelocal((struct ipv6_addr *)&ip_addr.prefix.s6_addr))))
    {
        vty_error_out(vty, "The address entered is a link-local address "
                        "when configure global unicast address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 多播地址 */
    if (ipv6_is_multicast((struct ipv6_addr *)&ip_addr.prefix.s6_addr))
    {
        vty_error_out(vty, "The address entered is multicast IPv6 address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 掩码覆盖位置全为 1 */
    ip_check = ip_addr;
    apply_mask_ipv6(&ip_check);
    masklen2ip6(mask_len, &mask_addr.prefix);

    if (0 == memcmp(&ip_check.prefix, &mask_addr.prefix, ((mask_len/8)+1)))
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    memset(&lif, 0, sizeof(struct route_if));
    lif.ifindex        = (uint32_t)vty->index;
    lif.intf.ipv6_flag = IP_TYPE_STATIC;

    pif_lookup = route_if_lookup((uint32_t)vty->index);
    if (NULL == argv[1])
    {
        /* DHCP 使能不允许配置静态接口主 IP */
        if ((NULL != pif_lookup) && (pif_lookup->intf.ipv6_flag == IP_TYPE_DHCP))
        {
            vty_error_out(vty, "DHCP is enabled. Can't configure IPv6 address.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        /* 接口借用 IP 不允许配置静态 IP */
        if ((NULL != pif_lookup) && (pif_lookup->intf.ipv6_flag == IP_TYPE_UNNUMBERED))
        {
            vty_error_out(vty, "Internet Address is unnumbered. Can't configure IPv6 address.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else
    {
        /* 配置从 IP 必须配置接口主 IP */
        if ((NULL == pif_lookup) || (!ROUTE_IF_IPV6_EXIST(pif_lookup->intf.ipv6[0].addr)))
        {
            vty_error_out(vty, "Please configure the primary address.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    if (NULL == argv[1])
    {
        memcpy(lif.intf.ipv6[0].addr, &ip_addr.prefix.s6_addr[0], IPV6_ADDR_LEN);
        lif.intf.ipv6[0].prefixlen = ip_addr.prefixlen;
    }
    else
    {
        memcpy(lif.intf.ipv6[1].addr, &ip_addr.prefix.s6_addr[0], IPV6_ADDR_LEN);
        lif.intf.ipv6[1].prefixlen = ip_addr.prefixlen;
    }

    /* 检查接口 IP 是否冲突 */
    if (NULL != pif_lookup)
    {
        lif.intf.vpn = pif_lookup->intf.vpn;
    }

    ret = route_if_check_v6_ip_conflict(&lif);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL == argv[1])
    {
        ret = route_if_add_v6_ip(&lif);
        if (ERRNO_SUCCESS != ret)
        {
            return CMD_WARNING;
        }
    }
    else
    {
        ret = route_if_add_slave_v6_ip(&lif);
        if (ERRNO_SUCCESS != ret)
        {
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口 IP 地址自动获取配置命令（IPv6）
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:00:27
 * @note       : 该命令与静态 IP 命令不可同时配置
 */
DEFUN(ipv6_address_dhcp_alloc,
    ipv6_address_dhcp_alloc_cmd,
    "ipv6 address dhcp",
    ROUTE_CLI_INFO)
{
    struct route_if *pif = NULL;
    struct route_if lif;
    int ret;

    memset(&lif, 0, sizeof(struct route_if));

    ret = route_if_get_intf_mode(vty);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    pif = route_if_lookup((uint32_t)vty->index);
    if ((pif != NULL)
        && ((pif->intf.ipv6_flag == IP_TYPE_STATIC)
            || (pif->intf.ipv6_flag == IP_TYPE_UNNUMBERED)
            || (pif->intf.ipv6_flag == IP_TYPE_DHCP_PD)))
    {
        vty_error_out(vty, "The interface IPv6 address already exist. "
                        "Can't enable the DHCP.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_DHCPV6, MODULE_ID_ROUTE, IPC_TYPE_L3IF,
                            IFNET_EVENT_IP_ADD, IPC_OPCODE_EVENT, (uint32_t)vty->index);
    if (0 != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

/**
 * @brief	   : 接口 IP 地址自动获取配置命令（IPv6）
 * @param[in ] :
 * @param[out] :
 * @return	   : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author	   : ZhangFj
 * @date	   : 2018年2月27日 16:00:27
 * @note	   : 该命令与静态 IP 命令不可同时配置
 */
DEFUN(ipv6_address_dhcp_pd_alloc,
	ipv6_address_dhcp_pd_alloc_cmd,
	"ipv6 address dhcp pd",
	ROUTE_CLI_INFO)
{
	struct route_if *pif = NULL;
	struct route_if lif;
	int ret;

	memset(&lif, 0, sizeof(struct route_if));

	ret = route_if_get_intf_mode(vty);
	if (ERRNO_SUCCESS != ret)
	{
		return CMD_WARNING;
	}

	pif = route_if_lookup((uint32_t)vty->index);
	if ((pif != NULL)
		&& ((pif->intf.ipv6_flag == IP_TYPE_STATIC)
			|| (pif->intf.ipv6_flag == IP_TYPE_UNNUMBERED)
			|| (pif->intf.ipv6_flag == IP_TYPE_DHCP)))
	{
		vty_error_out(vty, "The interface IPv6 address already exist. "
						"Can't enable the DHCP.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}
	ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_DHCPV6, MODULE_ID_ROUTE, IPC_TYPE_L3IF,
							IFNET_EVENT_IP_PD_ADD, IPC_OPCODE_EVENT, (uint32_t)vty->index);
	if (0 != ret)
	{
		vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}
/**
 * @brief      : 接口 IP 地址删除命令（IPv6）
 * @param[in ] : slave - 删除 IP 地址为从 IP
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:02:02
 * @note       : 删除主 IP 地址时，从 IP 地址一并清除
 */
DEFUN(no_ipv6_addr,
    no_ipv6_addr_cmd,
    "no ipv6 address [slave]",
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
        if (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
        {
            return CMD_SUCCESS;
        }

        route_if_delete_slave_v6_ip(pif);

        return CMD_SUCCESS;
    }
    else
    {
        if (pif->intf.ipv6_flag != IP_TYPE_STATIC)
        {
            return CMD_SUCCESS;
        }
    }

    if (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr)
        && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
        && (pif->intf.ipv6_flag == IP_TYPE_INVALID))
    {
        return CMD_SUCCESS;
    }

    route_if_delete_v6_ip(pif);

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口 IP 地址自动获取删除命令（IPv4）
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:04:01
 * @note       :
 */
DEFUN(no_ipv6_addr_dhcp,
    no_ipv6_addr_dhcp_cmd,
    "no ipv6 address dhcp",
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
    if ((NULL != pif) && (pif->intf.ipv6_flag == IP_TYPE_STATIC))
    {
        return CMD_SUCCESS;
    }

    ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_DHCPV6, MODULE_ID_ROUTE,
                           IPC_TYPE_L3IF, IFNET_EVENT_IP_DELETE,
                           IPC_OPCODE_EVENT, (uint32_t)vty->index);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != pif)
    {
        if (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr)
            && (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
            && (pif->intf.ipv6_flag == IP_TYPE_INVALID))
        {
            return CMD_SUCCESS;
        }

        ret = route_if_delete_v6_ip(pif);
        if (ERRNO_SUCCESS != ret)
        {
            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}

/**
 * @brief	   : 接口 IP 地址自动获取删除命令（IPv4）
 * @param[in ] :
 * @param[out] :
 * @return	   : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author	   : ZhangFj
 * @date	   : 2018年2月27日 16:04:01
 * @note	   :
 */
DEFUN(no_ipv6_addr_dhcp_pd,
	no_ipv6_addr_dhcp_pd_cmd,
	"no ipv6 address dhcp pd",
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
	if ((NULL != pif) && ((pif->intf.ipv6_flag == IP_TYPE_STATIC)
		|| (pif->intf.ipv6_flag == IP_TYPE_DHCP)))
	{
		return CMD_SUCCESS;
	}

	ret = ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_DHCPV6, MODULE_ID_ROUTE,
						   IPC_TYPE_L3IF, IFNET_EVENT_IP_PD_DELETE,
						   IPC_OPCODE_EVENT, (uint32_t)vty->index);
	if (ERRNO_SUCCESS != ret)
	{
		vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

		return CMD_WARNING;
	}

	if (NULL != pif)
	{
		if (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr)
			&& (!ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
			&& (pif->intf.ipv6_flag == IP_TYPE_INVALID))
		{
			return CMD_SUCCESS;
		}

		ret = route_if_delete_v6_ip(pif);
		if (ERRNO_SUCCESS != ret)
		{
			return CMD_WARNING;
		}
	}

	return CMD_SUCCESS;
}
/**
 * @brief      : 接口下配置显示命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:05:02
 * @note       :
 */
DEFUN(show_ip_addr,
    show_ip_addr_cmd,
    "show ip address",
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
								"dhcp":pif->intf.ipv4_flag == IP_TYPE_DHCP_ZERO ?
								"dhcp zero":"invalid", VTY_NEWLINE);
		
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
                        "dhcp":pif->intf.ipv6_flag == IP_TYPE_DHCP_PD ?
                        "dhcp pd":"invalid", VTY_NEWLINE);
    }
    else
    {
        if (IP_TYPE_DHCP == pif->intf.ipv6_flag)
        {
            vty_out(vty, "%-16s: %s %s%s", "ipv6 address", "--", "dhcp", VTY_NEWLINE);
        }else if (IP_TYPE_DHCP_PD == pif->intf.ipv6_flag)
        {
            vty_out(vty, "%-16s: %s %s%s", "ipv6 address", "--", "dhcp pd", VTY_NEWLINE);
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
DEFUN(show_ip_interface,
    show_ip_interface_cmd,
    "show ip interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP | "
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
            route_if_show_ip_interface(vty, pif);
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
                    route_if_num_show(vty);

                    vty_out(vty, "%-31s", "Interface");
                    vty_out(vty, "%-10s", "Physical");
                    vty_out(vty, "%-10s", "Protocol");
                    vty_out(vty, "%-19s", "IP Address");
                    vty_out(vty, "%s", VTY_NEWLINE);
                }

                route_if_show_ip_interface_brief(vty, pif);
            }
            else
            {
                route_if_show_ip_interface(vty, pif);
            }
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 三层接口1/0/1信息配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhouDW
 * @date       : 2019年1月22日 18:45:00
 * @note       :
 */
static int route_if_config_write_for_factory(struct vty *vty)
{
    struct route_if    *pif     = NULL;
    struct ifm_arp     *parp    = NULL;
    struct hash_bucket *pbucket = NULL;
    char ip[INET_ADDRSTRLEN]               = "";
    char ip_slave[INET_ADDRSTRLEN]         = "";
    char ipv6_buf[INET6_ADDRSTRLEN]        = "";
    char ipv6_slave[INET6_ADDRSTRLEN]      = "";
    char ipv6_local[INET6_ADDRSTRLEN]      = "";
    char ifname[IFNET_NAMESIZE]            = "";
    char unnumbered_ifname[IFNET_NAMESIZE] = "";
    int cursor;
	uint32_t ifindex = 0;

	ifindex = ifm_get_ifindex_by_name("gigabitethernet", "1/0/1");
	
	if(ifindex == 0)
	{
		ROUTE_LOG_ERROR("route get 1/0/1 ifindex fail!\n");
		return ERRNO_FAIL;
	}
	
    HASH_BUCKET_LOOP(pbucket, cursor, route_if_table)
    {
        pif = pbucket->data;
        if (NULL == pif)
        {
            continue;
        }

		if(pif->ifindex != ifindex)
		{
			continue;
		}
		
        ifm_get_name_by_ifindex(pif->ifindex, ifname);
		
        vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);

        if (URPF_TYPE_INVALID != pif->urpf)
        {
            vty_out(vty, " ip urpf %s%s", pif->urpf == URPF_TYPE_STRICT ?
                        "strict" : "loose", VTY_NEWLINE);
        }

        if (0 != pif->intf.vpn)
        {
            vty_out(vty, " ip l3vpn %d%s", pif->intf.vpn, VTY_NEWLINE);
        }

        if (pif->intf.ipv4_flag == IP_TYPE_STATIC)
        {
            inet_ipv4tostr(pif->intf.ipv4[0].addr, ip);

            vty_out(vty, " ip address %s/%d%s", ip,
                        pif->intf.ipv4[0].prefixlen, VTY_NEWLINE);
        }
        else if (pif->intf.ipv4_flag == IP_TYPE_DHCP)
        {
            vty_out(vty, " ip address dhcp%s", VTY_NEWLINE);

            if (pif->dhcp_save_flag == ENABLE)
            {
                vty_out(vty, " ip address dhcp save%s", VTY_NEWLINE);
            }
        }
        else if (pif->intf.ipv4_flag == IP_TYPE_DHCP_ZERO)
        {
            if (pif->dhcp_save_flag == ENABLE)
            {
                vty_out(vty, " ip address dhcp save%s", VTY_NEWLINE);
            }
        }

        else if (pif->intf.ipv4_flag == IP_TYPE_UNNUMBERED)
        {
            ifm_get_name_by_ifindex(pif->intf.unnumbered_if, unnumbered_ifname);

            vty_out(vty, " ip address unnumbered interface %s%s", unnumbered_ifname,
                        VTY_NEWLINE);
        }

        if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
        {
            inet_ipv4tostr(pif->intf.ipv4[1].addr, ip_slave);

            vty_out(vty, " ip address %s/%d slave%s", ip_slave,
                        pif->intf.ipv4[1].prefixlen, VTY_NEWLINE);
        }

        if (pif->intf.ipv6_flag == IP_TYPE_STATIC)
        {
            if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
            {
                inet_ntop(AF_INET6, pif->intf.ipv6[0].addr, ipv6_buf, INET6_ADDRSTRLEN);

                vty_out(vty, " ipv6 address %s/%d%s", ipv6_buf,
                            pif->intf.ipv6[0].prefixlen, VTY_NEWLINE);
            }
        }
        else if (pif->intf.ipv6_flag == IP_TYPE_DHCP)
        {
            vty_out(vty, " ipv6 address dhcp%s", VTY_NEWLINE);
        }

        if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
        {
            inet_ntop(AF_INET6, pif->intf.ipv6[1].addr, ipv6_slave, INET6_ADDRSTRLEN);

            vty_out(vty, " ipv6 address %s/%d slave%s", ipv6_slave,
                        pif->intf.ipv6[1].prefixlen, VTY_NEWLINE);
        }

        if (pif->link_local_flag == ENABLE)
        {
            vty_out(vty, " ipv6 address auto link-local%s", VTY_NEWLINE);
        }
        else if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
        {
            inet_ntop(AF_INET6, pif->intf.ipv6_link_local.addr, ipv6_local, INET6_ADDRSTRLEN);

            vty_out(vty, " ipv6 address %s link-local%s", ipv6_local, VTY_NEWLINE);
        }

        /* 接口主 ip 或借用 ip 存在，写接口 arp 配置 */
        if ((ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr)
            || (pif->intf.unnumbered_if)) && (0 != pif->arp.ifindex))
        {
            if (0 == pif->arp.ifindex)
                continue;

            /* loopback 接口、tunnel 接口不存在arp配置信息 */
            if (IFM_TYPE_IS_LOOPBCK(pif->arp.ifindex) || IFM_TYPE_IS_TUNNEL(pif->arp.ifindex))
                continue;

            parp = &(pif->arp);
            if (parp->arp_disable !=  ARP_DISABLE_DEF)
			{
				vty_out ( vty, " arp disable %s", VTY_NEWLINE );
            }
			else
			{
				if (parp->arp_proxy != ARP_PROXY_DEF)
					vty_out ( vty, " arp proxy %s", VTY_NEWLINE );

				if (parp->arp_num_max != ARP_NUM_MAX_DEF)
					vty_out ( vty, " arp num-limit %d%s",parp->arp_num_max, VTY_NEWLINE );
			}

        }

        /* 接口主 ipv6 地址存在，写接口 ndp 配置 */
        if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr) && (0 != pif->arp.ifindex))
        {
            /* loopback 接口、tunnel 接口不存在 ndp 配置信息 */
            if(IFM_TYPE_IS_LOOPBCK(pif->arp.ifindex) || IFM_TYPE_IS_TUNNEL(pif->arp.ifindex))
                continue;

            parp = &(pif->arp);
            if (parp->ndp_disable != NDP_ENABLE_DEF)
                vty_out(vty, " ipv6 ndp disable %s", VTY_NEWLINE);
			else
        	{
        		if (parp->ndp_proxy != NDP_NO_PROXY_DEF)
                	vty_out(vty, " ipv6 ndp proxy %s", VTY_NEWLINE);

            	if (parp->neighbor_limit != ND_TOTAL_NUM)
                	vty_out(vty, " ipv6 neighbor num-limit %d%s",parp->neighbor_limit, VTY_NEWLINE);
        	}
        }
    }

    return ERRNO_SUCCESS;
}

/**
 * @brief      : 三层接口信息配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangDW
 * @date       : 2018年2月27日 16:16:56
 * @note       :
 */
static int route_if_config_write(struct vty *vty)
{
    struct route_if    *pif     = NULL;
    struct ifm_arp     *parp    = NULL;
    struct hash_bucket *pbucket = NULL;
    char ip[INET_ADDRSTRLEN]               = "";
    char ip_slave[INET_ADDRSTRLEN]         = "";
    char ipv6_buf[INET6_ADDRSTRLEN]        = "";
    char ipv6_slave[INET6_ADDRSTRLEN]      = "";
    char ipv6_local[INET6_ADDRSTRLEN]      = "";
    char ifname[IFNET_NAMESIZE]            = "";
    char unnumbered_ifname[IFNET_NAMESIZE] = "";
    int cursor;

    HASH_BUCKET_LOOP(pbucket, cursor, route_if_table)
    {
        pif = pbucket->data;
        if (NULL == pif)
        {
            continue;
        }

        ifm_get_name_by_ifindex(pif->ifindex, ifname);

        vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);

        if (URPF_TYPE_INVALID != pif->urpf)
        {
            vty_out(vty, " ip urpf %s%s", pif->urpf == URPF_TYPE_STRICT ?
                        "strict" : "loose", VTY_NEWLINE);
        }

        if (0 != pif->intf.vpn)
        {
            vty_out(vty, " ip l3vpn %d%s", pif->intf.vpn, VTY_NEWLINE);
        }

        if (pif->intf.ipv4_flag == IP_TYPE_STATIC)
        {
            inet_ipv4tostr(pif->intf.ipv4[0].addr, ip);

            vty_out(vty, " ip address %s/%d%s", ip,
                        pif->intf.ipv4[0].prefixlen, VTY_NEWLINE);
        }
        else if (pif->intf.ipv4_flag == IP_TYPE_DHCP)
        {
            vty_out(vty, " ip address dhcp%s", VTY_NEWLINE);

            if (pif->dhcp_save_flag == ENABLE)
            {
                vty_out(vty, " ip address dhcp save%s", VTY_NEWLINE);
            }
        }
        else if (pif->intf.ipv4_flag == IP_TYPE_DHCP_ZERO)
        {
            if (pif->dhcp_save_flag == ENABLE)
            {
                vty_out(vty, " ip address dhcp save%s", VTY_NEWLINE);
            }
        }

        else if (pif->intf.ipv4_flag == IP_TYPE_UNNUMBERED)
        {
            ifm_get_name_by_ifindex(pif->intf.unnumbered_if, unnumbered_ifname);

            vty_out(vty, " ip address unnumbered interface %s%s", unnumbered_ifname,
                        VTY_NEWLINE);
        }

        if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr))
        {
            inet_ipv4tostr(pif->intf.ipv4[1].addr, ip_slave);

            vty_out(vty, " ip address %s/%d slave%s", ip_slave,
                        pif->intf.ipv4[1].prefixlen, VTY_NEWLINE);
        }

        if (pif->intf.ipv6_flag == IP_TYPE_STATIC)
        {
            if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
            {
                inet_ntop(AF_INET6, pif->intf.ipv6[0].addr, ipv6_buf, INET6_ADDRSTRLEN);

                vty_out(vty, " ipv6 address %s/%d%s", ipv6_buf,
                            pif->intf.ipv6[0].prefixlen, VTY_NEWLINE);
            }
        }
        else if (pif->intf.ipv6_flag == IP_TYPE_DHCP)
        {
            vty_out(vty, " ipv6 address dhcp%s", VTY_NEWLINE);
        }

        if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[1].addr))
        {
            inet_ntop(AF_INET6, pif->intf.ipv6[1].addr, ipv6_slave, INET6_ADDRSTRLEN);

            vty_out(vty, " ipv6 address %s/%d slave%s", ipv6_slave,
                        pif->intf.ipv6[1].prefixlen, VTY_NEWLINE);
        }

        if (pif->link_local_flag == ENABLE)
        {
            vty_out(vty, " ipv6 address auto link-local%s", VTY_NEWLINE);
        }
        else if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
        {
            inet_ntop(AF_INET6, pif->intf.ipv6_link_local.addr, ipv6_local, INET6_ADDRSTRLEN);

            vty_out(vty, " ipv6 address %s link-local%s", ipv6_local, VTY_NEWLINE);
        }

        /* 接口主 ip 或借用 ip 存在，写接口 arp 配置 */
        if ((ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr)
            || (pif->intf.unnumbered_if)) && (0 != pif->arp.ifindex))
        {
            if (0 == pif->arp.ifindex)
                continue;

            /* loopback 接口、tunnel 接口不存在arp配置信息 */
            if (IFM_TYPE_IS_LOOPBCK(pif->arp.ifindex) || IFM_TYPE_IS_TUNNEL(pif->arp.ifindex))
                continue;

            parp = &(pif->arp);
            if (parp->arp_disable !=  ARP_DISABLE_DEF)
			{
				vty_out ( vty, " arp disable %s", VTY_NEWLINE );
            }
			else
			{
				if (parp->arp_proxy != ARP_PROXY_DEF)
					vty_out ( vty, " arp proxy %s", VTY_NEWLINE );

				if (parp->arp_num_max != ARP_NUM_MAX_DEF)
					vty_out ( vty, " arp num-limit %d%s",parp->arp_num_max, VTY_NEWLINE );
			}

        }

        /* 接口主 ipv6 地址存在，写接口 ndp 配置 */
        if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr) && (0 != pif->arp.ifindex))
        {
            /* loopback 接口、tunnel 接口不存在 ndp 配置信息 */
            if(IFM_TYPE_IS_LOOPBCK(pif->arp.ifindex) || IFM_TYPE_IS_TUNNEL(pif->arp.ifindex))
                continue;

            parp = &(pif->arp);
            if (parp->ndp_disable != NDP_ENABLE_DEF)
                vty_out(vty, " ipv6 ndp disable %s", VTY_NEWLINE);
			else
        	{
        		if (parp->ndp_proxy != NDP_NO_PROXY_DEF)
                	vty_out(vty, " ipv6 ndp proxy %s", VTY_NEWLINE);

            	if (parp->neighbor_limit != ND_TOTAL_NUM)
                	vty_out(vty, " ipv6 neighbor num-limit %d%s",parp->neighbor_limit, VTY_NEWLINE);
        	}
        }
    }

    return ERRNO_SUCCESS;
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
void route_if_cmd_init(void)
{
    ifm_l3if_init(route_if_config_write);
	install_node_factory(&physical_if_node , route_if_config_write_for_factory);
		
#define ROUTE_INSTALL_ELEMENT_L3IF(cmd, flag)\
    install_element (PHYSICAL_IF_NODE, (cmd), (flag));\
    install_element (PHYSICAL_SUBIF_NODE, (cmd), (flag));\
    install_element (LOOPBACK_IF_NODE, (cmd), (flag));\
    install_element (TUNNEL_IF_NODE, (cmd), (flag));\
    install_element (TRUNK_IF_NODE, (cmd), (flag));\
    install_element (TRUNK_SUBIF_NODE, (cmd), (flag));\
    install_element (VLANIF_NODE, (cmd), (flag));

    ROUTE_INSTALL_ELEMENT_L3IF(&ip_l3vpn_cmd, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF(&no_ip_l3vpn_cmd, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF(&ip_addr_cmd, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF(&no_ip_addr_cmd, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF(&ipv6_addr_cmd, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF(&no_ipv6_addr_cmd, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF(&ipv6_addr_auto_link_local_cmd, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF(&no_ipv6_addr_auto_link_local_cmd, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF(&ipv6_addr_link_local_cmd, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF(&ipv6_addr_auto_generate_cmd, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF(&no_ipv6_addr_link_local_cmd, CMD_SYNC)
    ROUTE_INSTALL_ELEMENT_L3IF(&show_ip_addr_cmd, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_L3IF(&show_ip_interface_cmd, CMD_LOCAL)

    install_element (PHYSICAL_IF_NODE, &ip_urpf_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &ip_urpf_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &ip_urpf_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &ip_urpf_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &ip_urpf_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &no_ip_urpf_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_ip_urpf_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &no_ip_urpf_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &no_ip_urpf_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &no_ip_urpf_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &ip_address_dhcp_alloc_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &ip_address_dhcp_alloc_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &ip_address_dhcp_alloc_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &ip_address_dhcp_alloc_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &ip_address_dhcp_alloc_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &ip_address_dhcp_save_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &ip_address_dhcp_save_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &ip_address_dhcp_save_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &ip_address_dhcp_save_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &ip_address_dhcp_save_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &ipv6_address_dhcp_alloc_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &ipv6_address_dhcp_alloc_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &ipv6_address_dhcp_alloc_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &ipv6_address_dhcp_alloc_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &ipv6_address_dhcp_alloc_cmd, CMD_SYNC);

	install_element (PHYSICAL_IF_NODE, &ipv6_address_dhcp_pd_alloc_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &ipv6_address_dhcp_pd_alloc_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &ipv6_address_dhcp_pd_alloc_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &ipv6_address_dhcp_pd_alloc_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &ipv6_address_dhcp_pd_alloc_cmd, CMD_SYNC);

	install_element (PHYSICAL_IF_NODE, &ip_addr_unnumbered_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &ip_addr_unnumbered_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &ip_addr_unnumbered_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &ip_addr_unnumbered_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &ip_addr_unnumbered_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &ip_addr_unnumbered_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &no_ip_addr_dhcp_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_ip_addr_dhcp_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &no_ip_addr_dhcp_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &no_ip_addr_dhcp_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &no_ip_addr_dhcp_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &no_ip_addr_dhcp_save_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_ip_addr_dhcp_save_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &no_ip_addr_dhcp_save_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &no_ip_addr_dhcp_save_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &no_ip_addr_dhcp_save_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &no_ipv6_addr_dhcp_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_ipv6_addr_dhcp_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &no_ipv6_addr_dhcp_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &no_ipv6_addr_dhcp_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &no_ipv6_addr_dhcp_cmd, CMD_SYNC);

	install_element (PHYSICAL_IF_NODE, &no_ipv6_addr_dhcp_pd_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_ipv6_addr_dhcp_pd_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &no_ipv6_addr_dhcp_pd_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &no_ipv6_addr_dhcp_pd_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &no_ipv6_addr_dhcp_pd_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &no_ip_addr_unnumbered_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_ip_addr_unnumbered_cmd, CMD_SYNC);
    install_element (TUNNEL_IF_NODE, &no_ip_addr_unnumbered_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &no_ip_addr_unnumbered_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &no_ip_addr_unnumbered_cmd, CMD_SYNC);
    install_element (VLANIF_NODE, &no_ip_addr_unnumbered_cmd, CMD_SYNC);

    install_element (CONFIG_NODE, &show_ip_interface_cmd, CMD_LOCAL);

    /* 子接口封装命令注册 */
    install_element (PHYSICAL_SUBIF_NODE, &subif_encapsulate_untag_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &subif_encapsulate_dot1q_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &subif_encapsulate_qinq_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &subif_encapsulate_untag_add_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &subif_encapsulate_dot1q_add_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &subif_encapsulate_dot1q_translate_single_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &subif_encapsulate_dot1q_translate_double_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &subif_encapsulate_qinq_translate_single__cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &subif_encapsulate_qinq_translate_double_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &subif_encapsulate_qinq_translate_outer_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &subif_encapsulate_qinq_translate_delete_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_subif_encapsulate_cmd, CMD_SYNC);

    install_element (TRUNK_SUBIF_NODE, &subif_encapsulate_untag_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &subif_encapsulate_dot1q_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &subif_encapsulate_qinq_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &subif_encapsulate_untag_add_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &subif_encapsulate_dot1q_add_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &subif_encapsulate_dot1q_translate_single_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &subif_encapsulate_dot1q_translate_double_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &subif_encapsulate_qinq_translate_single__cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &subif_encapsulate_qinq_translate_double_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &subif_encapsulate_qinq_translate_outer_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &subif_encapsulate_qinq_translate_delete_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &no_subif_encapsulate_cmd, CMD_SYNC);

    /* 子接口 shutdown 命令注册 */
    install_element (PHYSICAL_IF_NODE, &subif_shutdown_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &subif_shutdown_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &subif_shutdown_cmd, CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &no_subif_shutdown_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_subif_shutdown_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &no_subif_shutdown_cmd, CMD_SYNC);

    return;
}


/* 更新接口arp信息 */
int arp_if_update(struct ifm_arp *parp, enum ARP_INFO type)
{
    struct route_if *pif = NULL;
	struct ifm_arp *parp_old = NULL;
    int ret;

	if(NULL == parp)
	{
	    ROUTE_LOG_ERROR("parp is NULL\n");
		return ERRNO_FAIL;
	}

	pif = route_if_lookup(parp->ifindex);
    if ( NULL == pif )
    {
        ROUTE_LOG_ERROR("route_if is NULL\n");
		return ERRNO_FAIL;
    }

    parp_old = (struct ifm_arp *)(&(pif->arp));
    if ( NULL == parp_old )
    {
        ROUTE_LOG_ERROR("arp_if is NULL.arp_if_set_info fail\n");
		return ERRNO_FAIL;
    }
    else
    {
        switch ( type )
	    {
		    case ARP_INFO_LEARN_LIMIT:
			    parp_old->arp_disable = parp->arp_disable;
			    break;
		    case ARP_INFO_AGENT:
			    parp_old->arp_proxy = parp->arp_proxy;
			    break;
		    case ARP_INFO_NUM_LIMIT:
		    	parp_old->arp_num_max = parp->arp_num_max;
			    break;
		    default:
			    return ERRNO_FAIL;
	    }

		ret = ipc_send_msg_n2(parp, sizeof(struct ifm_arp), 1, MODULE_ID_FTM, MODULE_ID_ARP,
								   IPC_TYPE_ARPIF, type, IPC_OPCODE_UPDATE, parp->ifindex);
	    if( ERRNO_SUCCESS != ret )
	    {
           ROUTE_LOG_ERROR("ipc_send_ftm fail\n");
           return ERRNO_IPC;
	    }

	    return ERRNO_SUCCESS;
    }

}


int ndp_if_update(struct ifm_arp *pndp, enum NDP_INFO type)
{
    struct route_if *pif = NULL;
    struct ifm_arp  *pndp_old = NULL;
    int ret;

	if(NULL == pndp)
	{
	    ROUTE_LOG_ERROR("pndp is NULL\n");
		return ERRNO_FAIL;
	}

	pif = route_if_lookup(pndp->ifindex);
    if ( NULL == pif )
    {
        ROUTE_LOG_ERROR("route_if is NULL\n");
		return ERRNO_FAIL;
    }

    pndp_old = (struct ifm_arp *)(&(pif->arp));
    if ( NULL == pndp_old )
    {
        ROUTE_LOG_ERROR("ifm_arp is NULL.ndp_if_update fail\n");
		return ERRNO_FAIL;
    }
    else
    {
        switch ( type )
	    {
		    case NDP_INFO_LEARN_LIMIT:
			    pndp_old->ndp_disable = pndp->ndp_disable;
			    break;
		    case NDP_INFO_AGENT:
			    pndp_old->ndp_proxy = pndp->ndp_proxy;
			    break;
		    case NDP_INFO_NUM_LIMIT:
		    	pndp_old->neighbor_limit = pndp->neighbor_limit;
			    break;
		    default:
			    return ERRNO_FAIL;
	    }

		ret = ROUTE_IPC_SENDTO_FTM(pndp, sizeof(struct ifm_arp), 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
								   IPC_TYPE_NDPIF, type, IPC_OPCODE_UPDATE, pndp->ifindex);
	    if(ERRNO_SUCCESS != ret )
	    {
           ROUTE_LOG_ERROR("ipc_send_ftm fail\n");
           return ERRNO_IPC;
	    }

	    return ERRNO_SUCCESS;
    }
}

