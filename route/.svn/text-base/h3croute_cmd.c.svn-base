/**
 * @file      : route_cmd.c
 * @brief     :
 * @details   :
 * @author    : wusong
 * @date      : 2018年2月11日 17:55:55
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/ifm_common.h>
#include <lib/mpls_common.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/errcode.h>
#include "route_main.h"
#include "route_static.h"
#include "route_cmd.h"
#include "route.h"
#include "route_if.h"



/* 存储路由回显行数 */
uint32_t h3c_route_line = 0;


/**
 * 用途：大数量回显时判断条件，一次最多回显 60 行
 * 缺点：效率低
 * 注意：命令行返回时必须手动将 line 置 0
 */
#define h3c_route_line_CHECK(line, line_num)\
    if ((line) == ((line_num)+60)) {(line)=0; return CMD_CONTINUE;}\
    if (((line)<((line_num)+60)) && ((++(line))>line_num))


/**
 * @brief      : 检测 show 命令前缀合法性
 * @param[in ] : ip - ip 地址
 * @param[out] :
 * @return     : 合法返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年2月11日 9:36:45
 * @note       :
 */
static int h3c_route_prefix_invalid_check(int ip)
{
    unsigned int a1 = 0;
    unsigned int a2 = 0;
    unsigned int a3 = 0;
    unsigned int a4 = 0;

    a1 = ((unsigned char *)&ip)[0];
    a2 = ((unsigned char *)&ip)[1];
    a3 = ((unsigned char *)&ip)[2];
    a4 = ((unsigned char *)&ip)[3];

    if (0 == a1)
    {
        if (0 != ip)
        {
            return ERRNO_CLI;
        }
    }
    else if ((255==a1) && (255==a2) && (255==a3) && (255==a4))
    {
        return ERRNO_CLI;
    }
    else if ((a1>=224) && (a1<=239))
    {
        return ERRNO_CLI;
    }
    else if ((a1>=240) && (a1<=255))
    {
        return ERRNO_CLI;
    }

    return ERRNO_SUCCESS;
}

/**
 * @brief      : 统计指定 vpn 实例的活跃路由数目
 * @param[in ] : vty - vty 全局结构
 * @param[in ] : vpn - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:02:29
 * @note       : 需要轮询活跃路由表，流程中活跃路由数量统计不准确
 */
static int h3c_route_active_num_show(struct vty *vty, int vpn)
{
    struct ptree_node  *node   = NULL;
    struct route_entry *proute = NULL;
    uint32_t total_num = 0;
    int      i         = 0;

    PTREE_LOOP(route_tree[vpn], proute, node)
    {
        for (i=0; i<proute->nhp_num; i++)
        {
            total_num++;
        }
    }
    vty_out(vty, "%20s: %-10d%s", "Routes", total_num, VTY_NEWLINE);
    
    vty_out(vty, "%s", VTY_NEWLINE);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 显示指定 vpn 实例的路由总数目
 * @param[in ] : vty - vty 全局结构
 * @param[in ] : vpn - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:08:37
 * @note       : 需要轮询路由表，流程中路由数量统计不准确
 */
static int h3c_route_total_num_show(struct vty *vty, int vpn)
{
    struct listnode   *plistnode = NULL;
    struct ptree_node *node      = NULL;
    struct rib_entry  *prib      = NULL;
    struct nhp_entry  *pnhp      = NULL;
    uint32_t total_num = 0;

    PTREE_LOOP(rib_tree[vpn], prib, node)
    {
        for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
        {
            if (pnhp != NULL)
            {
                total_num++;
            }
        }
    }

    vty_out(vty, "%20s: %-10u%s", "Routes", total_num, VTY_NEWLINE);
    
    vty_out(vty, "%s", VTY_NEWLINE);

    return ERRNO_SUCCESS;
}

/**
 * @brief      : 配置静态路由命令
 * @param[in ] : A.B.C.D/M            - 路由前缀和掩码
 * @param[in ] : ethernet USP         - 出接口类型为 ethernet
 * @param[in ] : gigabitethernet USP  - 出接口类型为 gigabitethernet
 * @param[in ] : xgigabitethernet USP - 出接口类型为 xgigabitethernet
 * @param[in ] : tunnel USP           - 出接口类型为 tunnel
 * @param[in ] : loopback <0-128>     - 出接口类型为 loopback
 * @param[in ] : vlanif <1-4095>      - 出接口类型为 vlanif
 * @param[in ] : trunk TRUNK          - 出接口类型为 trunk
 * @param[in ] : nexthop A.B.C.D      - 路由下一跳地址
 * @param[in ] : distance <1-255>     - 路由优先级，默认为 ROUTE_METRIC_STATIC
 * @param[in ] : vpn <1-128>          - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:52:30
 * @note       : 大括号 '{}' 表示多选 n(n大于等于 0，小于等于 n)，但这里接口类型最多只能选一个
 */
DEFUN(h3c_ip_route_static,
    h3c_ip_route_static_cmd,
    "ip route-static A.B.C.D (M | A.B.C.D) {ethernet USP | gigabitethernet USP | xgigabitethernet USP | "
    "tunnel USP | loopback <0-128> | vlanif <1-4095> | trunk TRUNK } "
    "[A.B.C.D] { distance <1-255>| vpn-instance <1-128>}",
    ROUTE_CLI_INFO)
{
    char prefix[INET_ADDRSTRLEN] = "";
    u_int16_t vpn_temp = 0;
    struct route_static lroute;
    struct prefix_ipv4  destip;
    struct prefix_ipv4  ip_mask;
    struct prefix_ipv4  nexthop;
    const char *pintf     = NULL;
    const char *pintf_num = NULL;
    int ret;

    memset(&lroute, 0, sizeof(struct route_static));
    lroute.action   = NHP_ACTION_FORWARD;
    lroute.nhp_type = NHP_TYPE_IP;

    if(NULL != argv[0])
    {
        inet_aton(argv[0], &destip.prefix);
    }
    
    if(NULL != argv[1])
    {
        if(NULL != strstr(argv[1], "."))
        {
            inet_aton(argv[1], &ip_mask.prefix);
            destip.prefixlen= ip_masklen(ip_mask.prefix);
        }
        else
        {
            destip.prefixlen = atoi(argv[1]);
        }
    }

    if ((destip.prefixlen > 32) || (destip.prefixlen < 0))
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lroute.prefix.type      = INET_FAMILY_IPV4;
    lroute.prefix.addr.ipv4 = ipv4_network_addr(destip.prefix.s_addr, destip.prefixlen);
    lroute.prefix.addr.ipv4 = ntohl(lroute.prefix.addr.ipv4);
    lroute.prefix.prefixlen = destip.prefixlen;

    ret = inet_valid_network(lroute.prefix.addr.ipv4);
    if (!ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /*
     * 配置接口类型，最多可选一
     * argv[1]: Ethernet
     * argv[2]: Gigabitethernet
     * argv[3]: Xgigabitethernet
     * argv[4]: Tunnel
     * argv[5]: Loopback
     * argv[6]: Vlanif
     * argv[7]: Trunk
     */
    if (NULL != argv[2])
    {
        pintf     = "ethernet";
        pintf_num = argv[2];
    }
	else if (NULL != argv[3])
    {
        pintf     = "gigabitethernet";
        pintf_num = argv[3];
    }
	else if (NULL != argv[4])
    {
        pintf     = "xgigabitethernet";
        pintf_num = argv[4];
    }
    else if (NULL != argv[5])
    {
        pintf           = "tunnel";
        pintf_num       = argv[5];
        lroute.nhp_type = NHP_TYPE_TUNNEL;
    }
    else if (NULL != argv[6])
    {
        pintf         = "loopback";
        pintf_num     = argv[6];
        lroute.action = NHP_ACTION_TOCPU;
    }
    else if (NULL != argv[7])
    {
        pintf     = "vlanif";
        pintf_num = argv[7];
    }
    else if (NULL != argv[8])
    {
        pintf     = "trunk";
        pintf_num = argv[8];
    }

    if (NULL != pintf)
    {
        lroute.ifindex = ifm_get_ifindex_by_name(pintf, pintf_num);
        if (lroute.ifindex == 0)
        {
            vty_error_out(vty, "Specify the illegal interface.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        /* 配置出接口为以太类型，但未配置下一跳时，路由前缀的掩码必须是 32 */
        if (((NULL != argv[2]) || (NULL != argv[3]) || (NULL != argv[4]))
            && (NULL == argv[9]))
        {
            if (lroute.prefix.prefixlen != IPV4_MAX_BITLEN)
            {
                vty_error_out(vty, "Only the host routing, allowing not configured next-hop%s",
                            VTY_NEWLINE);

                return CMD_WARNING;
            }

            vty_warning_out(vty, "A next hop is not configured for the static route, "
                        "which may result in forwarding failure.%s", VTY_NEWLINE);
        }
    }

    /* argv[8]: 下一跳 */
    if (NULL != argv[9])
    {
        ret = str2prefix_ipv4(argv[9], &nexthop);
        if (0 == ret)
        {
            vty_error_out(vty, "The next-hop address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        lroute.nexthop.type      = INET_FAMILY_IPV4;
        lroute.nexthop.addr.ipv4 = ntohl(nexthop.prefix.s_addr);

        ret = inet_valid_network(lroute.nexthop.addr.ipv4);
        if ((!ret) || (lroute.nexthop.addr.ipv4 == 0))
        {
            vty_error_out(vty, "The next-hop address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    /* argv[9]: distance */
    if (NULL != argv[10])
    {
        lroute.distance = atoi(argv[10]);
    }
    else
    {
        lroute.distance = ROUTE_METRIC_STATIC;
    }

    /* argv[10]: vpn */
    if (NULL != argv[11])
    {
        lroute.vpn = (uint16_t)atoi(argv[11]);

        if (1 != vty->config_read_flag)
        {
            if(0 == mpls_com_get_l3vpn_instance2(lroute.vpn, MODULE_ID_ROUTE))
            {
                vty_error_out(vty, "VPN-Instance does not exist.%s", VTY_NEWLINE);
                return CMD_WARNING;
            }
        }
    }

    /* 静态路由添加 */
    ret = route_static_add(&lroute);
    if (ERRNO_SUCCESS_UPDATE == ret)
    {
        vty_info_out(vty, "Succeeded in modifying route.%s", VTY_NEWLINE);
    }
    else if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 配置的路由网段与掩码不匹配，提示 */
    destip.prefix.s_addr = ntohl(destip.prefix.s_addr);

    if (destip.prefix.s_addr != lroute.prefix.addr.ipv4)
    {
        vty_info_out(vty, "The destination address and mask of the configured"
            " static route mismatched, and the static route %s/%d was generated.%s",
            inet_ipv4tostr(lroute.prefix.addr.ipv4, prefix),
            lroute.prefix.prefixlen, VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除静态路由命令
 * @param[in ] : A.B.C.D/M            - 路由前缀和掩码
 * @param[in ] : ethernet USP         - 出接口类型为 ethernet
 * @param[in ] : gigabitethernet USP  - 出接口类型为 gigabitethernet
 * @param[in ] : xgigabitethernet USP - 出接口类型为 xgigabitethernet
 * @param[in ] : tunnel USP           - 出接口类型为 tunnel
 * @param[in ] : loopback <0-128>     - 出接口类型为 loopback
 * @param[in ] : vlanif <1-4095>      - 出接口类型为 vlanif
 * @param[in ] : trunk TRUNK          - 出接口类型为 trunk
 * @param[in ] : nexthop A.B.C.D      - 路由下一跳地址
 * @param[in ] : vpn <1-128>          - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 11:42:37
 * @note       : 大括号 '{}' 表示多选 n(n大于等于 0，小于等于 n)，但这里接口类型最多只能选一个
 */
DEFUN(no_h3c_ip_route_static,
    no_h3c_ip_route_static_cmd,
    "no ip route-static A.B.C.D (M | A.B.C.D) {ethernet USP |gigabitethernet USP |xgigabitethernet USP | "
    "tunnel USP | loopback <0-128> | vlanif <1-4095> | trunk TRUNK } "
    "[A.B.C.D] { vpn-instance <1-128>}",
    ROUTE_CLI_INFO)
{
    struct route_static *sroute = NULL;
    struct route_static  lroute;
    struct prefix_ipv4   destip;
    struct prefix_ipv4   ip_mask;
    struct prefix_ipv4   nexthop;
    const char *pintf     = NULL;
    const char *pintf_num = NULL;
    int                  ret;

    memset(&lroute, 0, sizeof(struct route_static));

    if(NULL != argv[0])
    {
        inet_aton(argv[0], &destip.prefix);
    }
    
    if(NULL != argv[1])
    {
        if(NULL != strstr(argv[1], "."))
        {
            inet_aton(argv[1], &ip_mask.prefix);
            destip.prefixlen= ip_masklen(ip_mask.prefix);
        }
        else
        {
            destip.prefixlen = atoi(argv[1]);
        }
        
    }

    if ((destip.prefixlen > 32) || (destip.prefixlen < 0))
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lroute.prefix.type      = INET_FAMILY_IPV4;
    lroute.prefix.addr.ipv4 = destip.prefix.s_addr;
    lroute.prefix.addr.ipv4 = ntohl(lroute.prefix.addr.ipv4);
    lroute.prefix.prefixlen = destip.prefixlen;

    ret = inet_valid_network(lroute.prefix.addr.ipv4);
    if (!ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /*
     * 配置接口类型，最多可选一
     * argv[1]: Ethernet
     * argv[2]: Gigabitethernet
     * argv[3]: Xgigabitethernet
     * argv[4]: Tunnel
     * argv[5]: Loopback
     * argv[6]: Vlanif
     * argv[7]: Trunk
     */
    if (NULL != argv[2])
    {
        pintf     = "ethernet";
        pintf_num = argv[2];
    }
	else if (NULL != argv[3])
    {
        pintf     = "gigabitethernet";
        pintf_num = argv[3];
    }
	else if (NULL != argv[4])
    {
        pintf     = "xgigabitethernet";
        pintf_num = argv[4];
    }
    else if (NULL != argv[5])
    {
        pintf     = "tunnel";
        pintf_num = argv[5];
    }
    else if (NULL != argv[6])
    {
        pintf     = "loopback";
        pintf_num = argv[6];
    }
    else if (NULL != argv[7])
    {
        pintf     = "vlanif";
        pintf_num = argv[7];
    }
    else if (NULL != argv[8])
    {
        pintf     = "trunk";
        pintf_num = argv[8];
    }

    if (NULL != pintf)
    {
        lroute.ifindex = ifm_get_ifindex_by_name(pintf, pintf_num);
        if (lroute.ifindex == 0)
        {
            vty_error_out(vty, "Specify the illegal interface.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    /* argv[8]: 下一跳 */
    if (NULL != argv[9])
    {
        ret = str2prefix_ipv4(argv[9], &nexthop);
        if (0 == ret)
        {
            vty_error_out(vty, "The next-hop address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        lroute.nexthop.addr.ipv4 = ntohl(nexthop.prefix.s_addr);

        ret = inet_valid_network(lroute.nexthop.addr.ipv4);
        if ((!ret) || (lroute.nexthop.addr.ipv4 == 0))
        {
            vty_error_out(vty, "The next-hop address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    /* argv[9]: vpn */
    if (NULL != argv[10])
    {
        lroute.vpn = (uint16_t)atoi(argv[10]);
    }

    /* 该命令行不能删除黑洞路由 */
    sroute = route_static_lookup(&lroute);
    if ((NULL == sroute) || ((sroute->ifindex == 0) && (sroute->action == NHP_ACTION_DROP)))
    {
        vty_error_out(vty, "The specified static routing does not exist.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (sroute->ifindex != lroute.ifindex)
    {
        vty_error_out(vty, "The specified static routing does not exist.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 静态路由删除 */
    ret = route_static_delete(&lroute);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

/*no命令的undo实现*/
ALIAS(no_h3c_ip_route_static,
    undo_h3c_ip_route_static_cmd,
    "undo ip route-static A.B.C.D (M | A.B.C.D) {ethernet USP |gigabitethernet USP |xgigabitethernet USP | "
    "tunnel USP | loopback <0-128> | vlanif <1-4095> | trunk TRUNK } "
    "[A.B.C.D] { vpn-instance <1-128>}",
    ROUTE_CLI_INFO)


static int h3c_routing_table_display_brief(struct vty *vty, struct route_entry *proute)
{
    char ip_mask[INET_ADDRSTRLEN+5] = "";
    char destip[INET_ADDRSTRLEN]    = "";
    char nexthop[INET_ADDRSTRLEN]   = "";
    char ifname[IFNET_NAMESIZE]     = "";
    struct nhp_entry *pnhp     = NULL;
    const char       *protocol = NULL;
    int i;

    if (NULL == proute)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    inet_ipv4tostr(proute->prefix.addr.ipv4, destip);
    sprintf(ip_mask, "%s/%d", destip, proute->prefix.prefixlen);

    /* 同一前缀可能对应多个 nhp，依次轮序 */
    for (i=0; i<proute->nhp_num; i++)
    {
            pnhp = &proute->nhp[i];

            memset(nexthop, 0, sizeof(nexthop));
            memset(ifname, 0, sizeof(ifname));

            switch (pnhp->protocol)
            {
                case ROUTE_PROTO_STATIC:
                    protocol = "static";
                    break;
                case ROUTE_PROTO_CONNECT:
                    protocol = "direct";
                    break;
                case ROUTE_PROTO_OSPF:
                    protocol = "ospf";
                    break;
                case ROUTE_PROTO_ISIS:
                    protocol = "isis";
                    break;
                case ROUTE_PROTO_RIP:
                    protocol = "rip";
                    break;
                case ROUTE_PROTO_IBGP:
                    protocol = "ibgp";
                    break;
                case ROUTE_PROTO_EBGP:
                    protocol = "ebgp";
                    break;
                case ROUTE_PROTO_MPLSTP:
                    protocol = "mplstp";
                    break;
                case ROUTE_PROTO_LDP:
                    protocol = "ldp";
                    break;
                case ROUTE_PROTO_RSVPTE:
                    protocol = "rsvpte";
                    break;
                default :
                    protocol = "Unknow";
                    break;
            }

            /* 第一行前缀显示 ip 前缀 */
            if (i == 0)
            {
                vty_out(vty, "%18s ", ip_mask);
            }
            else
            {
                vty_out(vty, "%18s ", " ");
            }

            vty_out(vty, "%-5u", pnhp->vpn);
            vty_out(vty, "%-7s", protocol);
            vty_out(vty, "%-4u", pnhp->distance);
            vty_out(vty, "%-5u", pnhp->cost);

            if (proute->nhpinfo.nhp_type == NHP_TYPE_ECMP)
            {
                vty_out(vty, "%-5s", "ecmp");
            }
            else if (proute->nhpinfo.nhp_type == NHP_TYPE_FRR)
            {
                vty_out(vty, "%-5s", "frr");
            }
            else
            {
                vty_out(vty, "%-5s", " ");
            }

            inet_ipv4tostr(pnhp->nexthop.addr.ipv4, nexthop);

            vty_out(vty, "%-16s", nexthop);

            if (pnhp->action == NHP_ACTION_DROP)
            {
                vty_out(vty, "%-16s", "blackhole");
            }
            else
            {
                if (NHP_TYPE_LSP == pnhp->nhp_type)
                {
                    memset(ifname, 0, sizeof(ifname));
                    memcpy(ifname, "lsp", 4);
                }
                else
                {
                    ifm_get_name_by_ifindex(pnhp->ifindex, ifname);
                }

                vty_out(vty, "%-16s", pnhp->ifindex == 0 ? "-" : ifname);
            }

            vty_out(vty, "%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}


static int h3c_routing_table_display_verbose(struct vty *vty, struct rib_entry *prib,
                                struct nhp_entry *pnhp)
{
    char ip_mask[INET_ADDRSTRLEN+5] = "";
    char destip[INET_ADDRSTRLEN]    = "";
    char nexthop[INET_ADDRSTRLEN]   = "";
    char ifname[IFNET_NAMESIZE]     = "";
    const char *protocol = NULL;

    if ((NULL == prib) || (NULL == pnhp))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    inet_ipv4tostr(prib->prefix.addr.ipv4, destip);
    sprintf(ip_mask, "%s/%d", destip, prib->prefix.prefixlen);

    inet_ipv4tostr(pnhp->nexthop.addr.ipv4, nexthop);

    if (NHP_TYPE_LSP == pnhp->nhp_type)
    {
        memcpy(ifname, "lsp", sizeof(ifname));
    }
    else
    {
        ifm_get_name_by_ifindex(pnhp->ifindex, ifname);
    }

        vty_out(vty, "%s: %s%s", "Destip", ip_mask, VTY_NEWLINE);

        switch (pnhp->protocol)
        {
            case ROUTE_PROTO_STATIC:
                protocol = "static";
                break;
            case ROUTE_PROTO_CONNECT:
                protocol = "direct";
                break;
            case ROUTE_PROTO_OSPF:
                protocol = "ospf";
                break;
            case ROUTE_PROTO_ISIS:
                protocol = "isis";
                break;
            case ROUTE_PROTO_RIP:
                protocol = "rip";
                break;
            case ROUTE_PROTO_IBGP:
                protocol = "ibgp";
                break;
            case ROUTE_PROTO_EBGP:
                protocol = "ebgp";
                break;
            case ROUTE_PROTO_MPLSTP:
                protocol = "mplstp";
                break;
            case ROUTE_PROTO_LDP:
                protocol = "ldp";
                break;
            case ROUTE_PROTO_RSVPTE:
                protocol = "rsvpte";
                break;
            default :
                protocol = "Unknow";
                break;
        }

        vty_out(vty, "%15s: %-17s", "Protocol", protocol);
        vty_out(vty, "%15s: %-19d", "Preference", pnhp->distance);
        vty_out(vty, "%s", VTY_NEWLINE);


        vty_out(vty, "%15s: %-17s", "Nexthop", nexthop);

        if (pnhp->action == NHP_ACTION_DROP)
        {
            vty_out(vty, "%15s: %-19s", "Interface", "blackhole");
        }
        else
        {
            vty_out(vty, "%15s: %-19s", "Interface", ifname);
        }

        vty_out(vty, "%s", VTY_NEWLINE);


        vty_out(vty, "%15s: %-17s", "State", pnhp->down_flag == LINK_UP ? "UP":"DOWN");
        vty_out(vty, "%15s: %-19u", "Vpn", pnhp->vpn);
        vty_out(vty, "%s", VTY_NEWLINE);
    
        vty_out(vty, "%15s: %-17u", "Cost", pnhp->cost);
        vty_out(vty, "%15s: %-19s", "Description", pnhp->active == ROUTE_STATUS_ACTIVE ?
                        "download to fib":" ");
        vty_out(vty, "%s", VTY_NEWLINE);
    
        vty_out(vty, "%15s: %-17u", "instance", pnhp->instance);
        vty_out(vty, "%s", VTY_NEWLINE);
 
        vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

/**
 * @brief      : 显示指定前缀的路由命令
 * @param[in ] : A.B.C.D/M   - 路由前缀和掩码
 * @param[in ] : A.B.C.D     - 路由前缀
 * @param[in ] : vpn <1-128> - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 12:15:50
 * @note       : 小括号 '()' 作用为多选1
 */
DEFUN(h3c_display_ip_route_single,
    h3c_display_ip_route_single_cmd,
    "display ip routing-table vpn-instance <1-128>  A.B.C.D (M | A.B.C.D) [verbose]",
    ROUTE_CLI_INFO)
{
    struct listnode     *pnode      = NULL;
    struct listnode     *pnextnode  = NULL;
    struct rib_entry    *prib       = NULL;
    struct nhp_entry    *pnhp       = NULL;
    struct route_entry *proute      = NULL;
    struct route_static lroute;
    struct prefix_ipv4  destip;
    struct prefix_ipv4  ip_mask;

    int ret;

    memset(&lroute, 0 ,sizeof(struct route_static));
    /* vpn-instance */
    if(NULL != argv[0])
    {
        lroute.vpn = (uint16_t)atoi(argv[0]);
    }
    /* ip-prefix and mask */

    if(NULL != argv[1])
    {
        inet_aton(argv[1], &destip.prefix);
    }
    
    if(NULL != argv[2])
    {
        if(NULL != strstr(argv[2], "."))
        {
            inet_aton(argv[2], &ip_mask.prefix);
            destip.prefixlen= ip_masklen(ip_mask.prefix);
        }
        else
        {
            destip.prefixlen = atoi(argv[2]);
        }
    }

    lroute.prefix.type      = INET_FAMILY_IPV4;
    lroute.prefix.addr.ipv4 = destip.prefix.s_addr;
    lroute.prefix.prefixlen = destip.prefixlen;

    ret = h3c_route_prefix_invalid_check(lroute.prefix.addr.ipv4);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if((destip.prefixlen > 32) || (destip.prefixlen < 0))
    {
        vty_error_out(vty, "The specified IP mask is invalid.%s",VTY_NEWLINE);
        return CMD_WARNING;
    }
    lroute.prefix.addr.ipv4 = ntohl(lroute.prefix.addr.ipv4);
    if(NULL != argv[3])
    {
        prib = rib_lookup(&lroute.prefix, lroute.vpn);
        if(NULL == prib)
        {
            return ERRNO_SUCCESS;
        }
        /* list for each nhplist in prib */
        for (ALL_LIST_ELEMENTS(&prib->nhplist, pnode, pnextnode, pnhp))
        {
            if(pnhp)
            {
                /* display the verbose information from rib_tree */
                h3c_routing_table_display_verbose(vty, prib, pnhp);
            }
        }
    }
    else
    {
        proute = route_lookup(&lroute.prefix, lroute.vpn);
        if(NULL == proute)
        {
            return ERRNO_SUCCESS;
        }

        /* 输出路由信息 */
        vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
        vty_out(vty, "Routing Tables: Single%s", VTY_NEWLINE);
        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty, "%20s: %-10d", "Routes", 1);
        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty, "%-19s", "Destination/Mask");
        vty_out(vty, "%-5s", "Vpn");
        vty_out(vty, "%-7s", "Proto");
        vty_out(vty, "%-4s", "Pre");
        vty_out(vty, "%-5s", "Cost");
        vty_out(vty, "%-5s", "Flag");
        vty_out(vty, "%-16s", "Nexthop");
        vty_out(vty, "%-16s", "Interface");
        vty_out(vty, "%s", VTY_NEWLINE);

        /* display the brief information from route_tree */
        h3c_routing_table_display_brief(vty, proute);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示指定前缀的路由命令
 * @param[in ] : A.B.C.D/M   - 路由前缀和掩码
 * @param[in ] : A.B.C.D     - 路由前缀
 * @param[in ] : vpn <1-128> - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 12:15:50
 * @note       : 小括号 '()' 作用为多选1
 */
DEFUN(h3c_display_ip_route_single2,
    h3c_display_ip_route_single_cmd2,
    "display ip routing-table vpn-instance <1-128> [verbose]",
    ROUTE_CLI_INFO)
{
    struct ptree_node  *node   = NULL;
    struct route_entry *proute = NULL;
    struct rib_entry    *prib   = NULL;
    struct listnode     *plistnode  = NULL;
    struct nhp_entry    *pnhp       = NULL;
    uint32_t    vpn = 0;
    uint32_t    num = 0;

    int ret;

    /* argv[0]: vpn */
    if (NULL != argv[0])
    {
        vpn = atoi(argv[0]);
    }

    if(NULL != argv[1])
    {
        /* display the verbose information from rib_tree */

        PTREE_LOOP(rib_tree[vpn], prib, node)
        {
            /* num 等于 1，输出一次信息头 */
            if (++num == 1)
            {
                vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
        
                vty_out(vty, "Routing Tables: All%s", VTY_NEWLINE);
        
                vty_out(vty, "%s", VTY_NEWLINE);
        
                h3c_route_total_num_show(vty, vpn);
        
                vty_out(vty, "%s", VTY_NEWLINE);
                
            }

            for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
            {
                ret = h3c_routing_table_display_verbose(vty, prib, pnhp);
                if (ret == CMD_CONTINUE)
                {
                    return ret;
                }
            }
            h3c_route_line = 0;
        }
    }
    else
    {
        /* display the brief information from route_tree */

        PTREE_LOOP(route_tree[vpn], proute, node)
        {
            /* num 等于 1，输出一次信息头 */
            if (++num == 1)
            {

                    vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
                

                    vty_out(vty, "Routing Tables: Active%s", VTY_NEWLINE);
                
        
                    vty_out(vty, "%s", VTY_NEWLINE);
                
        
                h3c_route_active_num_show(vty, vpn);
        

                    vty_out(vty, "%-19s", "Destination/Mask");
                    vty_out(vty, "%-5s", "Vpn");
                    vty_out(vty, "%-7s", "Proto");
                    vty_out(vty, "%-4s", "Pre");
                    vty_out(vty, "%-5s", "Cost");
                    vty_out(vty, "%-5s", "Flag");
                    vty_out(vty, "%-16s", "Nexthop");
                    vty_out(vty, "%-16s", "Interface");
                    vty_out(vty, "%s", VTY_NEWLINE);
                

                    vty_out(vty, "%s", VTY_NEWLINE);
                
            }
        
            ret = h3c_routing_table_display_brief(vty, proute);
            if (CMD_CONTINUE == ret)
            {
                return ret;
            }
            h3c_route_line = 0;
            
        }

    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示活跃路由命令
 * @param[in ] : vpn <1-128> - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:02:21
 * @note       : 不指定 vpn 只显示公网路由
 */
DEFUN(h3c_display_ip_route_active,
    h3c_display_ip_route_active_cmd,
    "display ip routing-table A.B.C.D (M | A.B.C.D) [verbose]",
    ROUTE_CLI_INFO)
{
    struct listnode     *pnode      = NULL;
    struct listnode     *pnextnode  = NULL;
    struct rib_entry    *prib       = NULL;
    struct nhp_entry    *pnhp       = NULL;
    struct route_entry *proute = NULL;
    struct route_static lroute;
    struct prefix_ipv4  destip;
    struct prefix_ipv4  ip_mask;
    int ret;

    memset(&lroute, 0 ,sizeof(struct route_static));
    /* ip-prefix and mask */

    if(NULL != argv[0])
    {
        inet_aton(argv[0], &destip.prefix);
    }
    
    if(NULL != argv[1])
    {
        if(NULL != strstr(argv[1], "."))
        {
            inet_aton(argv[1], &ip_mask.prefix);
            destip.prefixlen= ip_masklen(ip_mask.prefix);
        }
        else
        {
            destip.prefixlen = atoi(argv[1]);
        }
    }

    lroute.prefix.type      = INET_FAMILY_IPV4;
    lroute.prefix.addr.ipv4 = destip.prefix.s_addr;
    lroute.prefix.prefixlen = destip.prefixlen;

    ret = h3c_route_prefix_invalid_check(lroute.prefix.addr.ipv4);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if((destip.prefixlen > 32) || (destip.prefixlen < 0))
    {
        vty_error_out(vty, "The specified IP mask is invalid.%s",VTY_NEWLINE);
        return CMD_WARNING;
    }

    lroute.prefix.addr.ipv4 = ntohl(lroute.prefix.addr.ipv4);

    if(NULL != argv[2])
    {
        vty_out(vty, "verbose 0 %s", VTY_NEWLINE);
        prib = rib_lookup(&lroute.prefix, lroute.vpn);
        if(NULL == prib)
        {
            return ERRNO_SUCCESS;
        }
        vty_out(vty, "verbose 1 %s", VTY_NEWLINE);
        /* list for each nhplist in prib */
        for (ALL_LIST_ELEMENTS(&prib->nhplist, pnode, pnextnode, pnhp))
        {
            if(pnhp)
            {
                vty_out(vty, "verbose 2 %s", VTY_NEWLINE);
                /* display the verbose information from rib_tree */
                h3c_routing_table_display_verbose(vty, prib, pnhp);
            }
        }
    }
    else
    {
        vty_out(vty, "brief 0 %s", VTY_NEWLINE);
        proute = route_lookup(&lroute.prefix, lroute.vpn);
        if(NULL == proute)
        {
            return ERRNO_SUCCESS;
        }

        /* 输出路由信息 */
        vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
        vty_out(vty, "Routing Tables: Single%s", VTY_NEWLINE);
        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty, "%20s: %-10d", "Routes", 1);
        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty, "%-19s", "Destination/Mask");
        vty_out(vty, "%-5s", "Vpn");
        vty_out(vty, "%-7s", "Proto");
        vty_out(vty, "%-4s", "Pre");
        vty_out(vty, "%-5s", "Cost");
        vty_out(vty, "%-5s", "Flag");
        vty_out(vty, "%-16s", "Nexthop");
        vty_out(vty, "%-16s", "Interface");
        vty_out(vty, "%s", VTY_NEWLINE);
        
        vty_out(vty, "brief 2 %s", VTY_NEWLINE);
        /* display the brief information from route_tree */
        h3c_routing_table_display_brief(vty, proute);
    }

    return CMD_SUCCESS;
}





/**
 * @brief      : 显示活跃路由命令
 * @param[in ] : vpn <1-128> - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:02:21
 * @note       : 不指定 vpn 只显示公网路由
 */
DEFUN(h3c_display_ip_route_active2,
    h3c_display_ip_route_active_cmd2,
    "display ip routing-table [verbose]",
    ROUTE_CLI_INFO)
{
    struct listnode     *plistnode  = NULL;
    struct rib_entry    *prib       = NULL;
    struct nhp_entry    *pnhp       = NULL;
    struct ptree_node  *node   = NULL;
    struct route_entry *proute = NULL;
    int num = 0;
    int vpn = 0;
    int ret;


    if(NULL == argv[0])
    {

        PTREE_LOOP(route_tree[vpn], proute, node)
        {
            /* num 等于 1，输出一次信息头 */
            if (++num == 1)
            {

                vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
                
				vty_out(vty, "Routing Tables: Active%s", VTY_NEWLINE);
                
				vty_out(vty, "%s", VTY_NEWLINE);
                
                h3c_route_active_num_show(vty, vpn);

                vty_out(vty, "%-19s", "Destination/Mask");
                vty_out(vty, "%-5s", "Vpn");
                vty_out(vty, "%-7s", "Proto");
                vty_out(vty, "%-4s", "Pre");
                vty_out(vty, "%-5s", "Cost");
                vty_out(vty, "%-5s", "Flag");
                vty_out(vty, "%-16s", "Nexthop");
                vty_out(vty, "%-16s", "Interface");
                vty_out(vty, "%s", VTY_NEWLINE);
                
                vty_out(vty, "%s", VTY_NEWLINE);
               
            }

            ret = h3c_routing_table_display_brief(vty, proute);
            if (CMD_CONTINUE == ret)
            {
                return ret;
            }
        }

        /* 批量回显命令行返回成功，需要将该变量置 0 */
        h3c_route_line = 0;
    }
    else
    {
        /* display the verbose information from rib_tree */

        PTREE_LOOP(rib_tree[vpn], prib, node)
        {
            /* num 等于 1，输出一次信息头 */
            if (++num == 1)
            {
                vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
                
                vty_out(vty, "Routing Tables: All%s", VTY_NEWLINE);
                
                vty_out(vty, "%s", VTY_NEWLINE);
        
                h3c_route_total_num_show(vty, vpn);
        
                vty_out(vty, "%s", VTY_NEWLINE);
                
            }

            for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
            {
                ret = h3c_routing_table_display_verbose(vty, prib, pnhp);
                if (ret == CMD_CONTINUE)
                {
                    return ret;
                }
            }
            h3c_route_line = 0;
        }
    }

    return CMD_SUCCESS;
}

#if 1
#define H3C_ROUTE_INSTALL_ELEMENT_SHOW(cmd, flag)\
    install_element (PHYSICAL_IF_NODE, (cmd), (flag));\
    install_element (PHYSICAL_SUBIF_NODE, (cmd), (flag));\
    install_element (LOOPBACK_IF_NODE, (cmd), (flag));\
    install_element (TUNNEL_IF_NODE, (cmd), (flag));\
    install_element (TRUNK_IF_NODE, (cmd), (flag));\
    install_element (TRUNK_SUBIF_NODE, (cmd), (flag));\
    install_element (VLANIF_NODE, (cmd), (flag));\
    install_element (ROUTE_NODE, (cmd), (flag));\
    install_element (CONFIG_NODE, (cmd), (flag));
#endif

void h3c_route_static_cmd_init()
{
#if 1
    /* h3c cmd register */
#define ROUTE_INSTALL_H3C_CONFIG_CMD(cmd, flag)      install_element(CONFIG_NODE, (cmd), (flag))
    
        ROUTE_INSTALL_H3C_CONFIG_CMD (&h3c_ip_route_static_cmd, CMD_SYNC);
        ROUTE_INSTALL_H3C_CONFIG_CMD (&no_h3c_ip_route_static_cmd, CMD_SYNC);
        ROUTE_INSTALL_H3C_CONFIG_CMD (&undo_h3c_ip_route_static_cmd, CMD_SYNC);

        H3C_ROUTE_INSTALL_ELEMENT_SHOW(&h3c_display_ip_route_single_cmd, CMD_LOCAL);
        H3C_ROUTE_INSTALL_ELEMENT_SHOW(&h3c_display_ip_route_active_cmd, CMD_LOCAL);
        H3C_ROUTE_INSTALL_ELEMENT_SHOW(&h3c_display_ip_route_single_cmd2, CMD_LOCAL);
        H3C_ROUTE_INSTALL_ELEMENT_SHOW(&h3c_display_ip_route_active_cmd2, CMD_LOCAL);
    /* h3c cmd register finished */
#endif
}



