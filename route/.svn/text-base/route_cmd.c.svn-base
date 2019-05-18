/**
 * @file      : route_cmd.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
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
#include "lib/log.h"
#include "lib/memory.h"

/* 存储路由回显行数 */
uint32_t route_line = 0;

/* 存储 vpn 黑洞路由，当 vpn 实例创建时再下发 */
struct list *route_blackhole_list = NULL;
struct list *routev6_blackhole_list = NULL;

/* route 节点 */
static struct cmd_node route_node1 =
{
    ROUTE_NODE,
    "%s(config-route)# ",
    1
};

#define TRAVERSE_ROUTE_LIST_TIME    3

/**
 * 用途：大数量回显时判断条件，一次最多回显 60 行
 * 缺点：效率低
 * 注意：命令行返回时必须手动将 line 置 0
 */
#define ROUTE_LINE_CHECK(line, line_num)\
    if ((line) == ((line_num)+60)) {(line)=0; return CMD_CONTINUE;}\
    if (((line)<((line_num)+60)) && ((++(line))>line_num))

static int route_static_config_write(struct vty *vty);


/**
 * @brief      : 检查 ip 与掩码组合的合法性
 * @param[in ] : ip   - ip 地址
 * @param[in ] : mask - 掩码
 * @param[out] :
 * @return     : 合法返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年2月11日 9:27:38
 * @note       :
 */
int route_valid_ip_mask(int ip, int mask)
{
    struct prefix_ipv4 ip_old;
    struct prefix_ipv4 ip_new;
    struct prefix_ipv4 ip_check;
    struct in_addr     mask_addr;

    ip_old.prefix.s_addr = ip;
    ip_old.prefixlen     = mask;
    ip_new.prefix.s_addr = ip;
    ip_new.prefixlen     = mask;

    apply_mask_ipv4(&ip_new);

    /* 网络号为 0，非法 ip */
    if (ip_new.prefix.s_addr == 0)
    {
        return ERRNO_FAIL;
    }

    /* 主机号为 0，非法 ip */
    if (ip_new.prefix.s_addr == ip_old.prefix.s_addr)
    {
        return ERRNO_FAIL;
    }

    /* 主机号全为 1，非法 ip */
    ip_check.prefix.s_addr = ip;
    ip_check.prefixlen     = mask;

    masklen2ip(ip_check.prefixlen, &mask_addr);
    ip_check.prefix.s_addr &= ~mask_addr.s_addr;

    if (ip_check.prefix.s_addr == ~mask_addr.s_addr)
    {
        return ERRNO_FAIL;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 检测 show 命令前缀合法性
 * @param[in ] : ip - ip 地址
 * @param[out] :
 * @return     : 合法返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年2月11日 9:36:45
 * @note       :
 */
static int route_valid_show_prefix(int ip)
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
 * @brief      : 显示活跃路由简略信息
 * @param[in ] : vty    - vty 全局结构
 * @param[in ] : proute - 活跃路由信息
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 9:41:11
 * @note       :
 */
static int route_static_show_active(struct vty *vty, struct route_entry *proute)
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


/**
 * @brief      : 显示路由表中所有路由的简略信息
 * @param[in ] : vty  - vty 全局结构
 * @param[in ] : prib - 原始路由前缀信息
 * @param[in ] : pnhp - 路由下一跳信息
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 9:54:59
 * @note       :
 */
static int route_static_show_all(struct vty *vty, struct rib_entry *prib,
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
 * @brief      : 统计指定 vpn 实例的活跃路由数目
 * @param[in ] : vty - vty 全局结构
 * @param[in ] : vpn - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:02:29
 * @note       : 需要轮询活跃路由表，流程中活跃路由数量统计不准确
 */
static int route_active_num_show(struct vty *vty, int vpn)
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
static int route_total_num_show(struct vty *vty, int vpn)
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
 * @brief      : 按协议类型显示指定 vpn 实例的路由数量
 * @param[in ] : vty - vty 全局结构
 * @param[in ] : vpn - vpn 实例号
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:12:06
 * @note       : 需要同时轮询路由表和活跃路由表
 */
static void route_statistics_num_show(struct vty *vty, int vpn)
{
    struct ptree_node  *node1     = NULL;
    struct route_entry *proute    = NULL;
    struct listnode    *plistnode = NULL;
    struct ptree_node  *node2     = NULL;
    struct rib_entry   *prib      = NULL;
    struct nhp_entry   *pnhp      = NULL;
    struct rib_global num = {ROUTE_NUM_MAX};
    uint32_t total_num  = 0;
    uint32_t active_num = 0;
    int      i          = 0;

    PTREE_LOOP(route_tree[vpn], proute, node1)
    {
        for (i=0; i<proute->nhp_num; i++)
        {
            switch (proute->nhp[i].protocol)
            {
                case ROUTE_PROTO_CONNECT:
                    num.num_direct_active++;
                    break;
                case ROUTE_PROTO_STATIC:
                    num.num_static_active++;
                    break;
                case ROUTE_PROTO_OSPF:
                    num.num_ospf_active++;
                    break;
                case ROUTE_PROTO_RIP:
                    num.num_rip_active++;
                    break;
                case ROUTE_PROTO_ISIS:
                    num.num_isis_active++;
                    break;
                case ROUTE_PROTO_IBGP:
                    num.num_ibgp_active++;
                    break;
                case ROUTE_PROTO_EBGP:
                    num.num_ebgp_active++;
                    break;
                case ROUTE_PROTO_LDP:
                    num.num_ldp_active++;
                    break;
                case ROUTE_PROTO_MPLSTP:
                    num.num_mplstp_active++;
                    break;
                case ROUTE_PROTO_RSVPTE:
                    num.num_rsvpte_active++;
                    break;
                default :
                    break;
            }
        }
    }

    PTREE_LOOP(rib_tree[vpn], prib, node2)
    {
        for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
        {
            if (pnhp != NULL)
            {
                switch (pnhp->protocol)
                {
                    case ROUTE_PROTO_CONNECT:
                        num.num_direct++;
                        break;
                    case ROUTE_PROTO_STATIC:
                        num.num_static++;
                        break;
                    case ROUTE_PROTO_OSPF:
                        num.num_ospf++;
                        break;
                    case ROUTE_PROTO_RIP:
                        num.num_rip++;
                        break;
                    case ROUTE_PROTO_ISIS:
                        num.num_isis++;
                        break;
                    case ROUTE_PROTO_IBGP:
                        num.num_ibgp++;
                        break;
                    case ROUTE_PROTO_EBGP:
                        num.num_ebgp++;
                        break;
                    case ROUTE_PROTO_LDP:
                        num.num_ldp++;
                        break;
                    case ROUTE_PROTO_MPLSTP:
                        num.num_mplstp++;
                        break;
                    case ROUTE_PROTO_RSVPTE:
                        num.num_rsvpte++;
                        break;
                    default :
                        break;
                }
            }
        }
    }

    total_num = num.num_direct + num.num_static + num.num_ospf + num.num_isis
                + num.num_rip + num.num_ibgp + num.num_ebgp + num.num_ldp
                + num.num_mplstp + num.num_rsvpte;
    active_num = num.num_direct_active + num.num_static_active + num.num_ospf_active
                + num.num_isis_active + num.num_rip_active + num.num_ibgp_active
                + num.num_ebgp_active + num.num_ldp_active + num.num_mplstp_active
                + num.num_rsvpte_active;

    vty_out(vty, "%-10s%-10s%-10s%s", "Protocol", "Total", "Active", VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "DIRECT", num.num_direct,
                num.num_direct_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "STATIC", num.num_static,
                num.num_static_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "OSPF", num.num_ospf,
                num.num_ospf_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "ISIS", num.num_isis,
                num.num_isis_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "RIP", num.num_rip,
                num.num_rip_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "IBGP", num.num_ibgp,
                num.num_ibgp_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "EBGP", num.num_ebgp,
                num.num_ebgp_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "LDP", num.num_ldp,
                num.num_ldp_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "MPLSTP", num.num_mplstp,
                num.num_mplstp_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "RSVPTE", num.num_rsvpte,
                num.num_rsvpte_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "Total", total_num,
                active_num, VTY_NEWLINE);
    vty_out(vty,"%s",VTY_NEWLINE);

    return;
}


/**
 * @brief      : 自动生成 router_id
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:14:14
 * @note       :
 */
void route_set_router_id(void)
{
    struct route_if    *pif     = NULL;
    struct hash_bucket *pbucket = NULL;
    int      cursor  = 0;
    uint32_t ifindex = 0;

    if (ROUTE_IPV4_ADDR_EXIST(g_route.router_id))
    {
        return;
    }

    /* 优先选用 loopback 0 接口 ip */
    ifindex = ifm_get_ifindex_by_name("loopback", (char *)"0");

    pif = route_if_lookup(ifindex);
    if ((NULL != pif) && (ROUTE_IPV4_ADDR_EXIST(pif->intf.ipv4[0].addr)))
    {
        g_route.router_id = pif->intf.ipv4[0].addr;

        ROUTE_IPC_SENDTO_FTM(&g_route.router_id, IP_ADDR_LEN, 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
                    IPC_TYPE_FIB, ROUTE_SUBTYPE_ROUTER_ID, IPC_OPCODE_ADD, 0);

        return;
    }

    /* 次选 loopback 1 接口 ip */
    ifindex = ifm_get_ifindex_by_name("loopback", "1");

    pif = route_if_lookup(ifindex);
    if ((NULL != pif) && (ROUTE_IPV4_ADDR_EXIST(pif->intf.ipv4[0].addr)))
    {
        g_route.router_id = pif->intf.ipv4[0].addr;

        ROUTE_IPC_SENDTO_FTM(&g_route.router_id, IP_ADDR_LEN, 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
                    IPC_TYPE_FIB, ROUTE_SUBTYPE_ROUTER_ID, IPC_OPCODE_ADD, 0);

        return;
    }

    ifindex = ifm_get_ifindex_by_name("gigabitethernet", "1/0/1");

    /* 选任意接口 ip ，不包含管理口 */
    HASH_BUCKET_LOOP(pbucket, cursor, route_if_table)
    {
        pif = (struct route_if *)pbucket->data;
        if ((NULL == pif) || (!ROUTE_IPV4_ADDR_EXIST(pif->intf.ipv4[0].addr)))
        {
            continue;
        }

        /* router-id 不选管理口 ip 地址 */
        if (pif->ifindex == ifindex)
        {
            continue;
        }

        g_route.router_id = pif->intf.ipv4[0].addr;

        ROUTE_IPC_SENDTO_FTM(&g_route.router_id, IP_ADDR_LEN, 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
                    IPC_TYPE_FIB, ROUTE_SUBTYPE_ROUTER_ID, IPC_OPCODE_ADD, 0);

        return;
    }

    return;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月30日 15:45:34
 * @note       :
 */
static int route_traverse_route_blackhole_list(struct thread *thread)
{
    struct route_static *proute = NULL;
    struct listnode     *pnode  = NULL;
    struct listnode     *pnext  = NULL;

    for (ALL_LIST_ELEMENTS(route_blackhole_list, pnode, pnext, proute))
    {
        if (proute == NULL)
        {
            continue;
        }

        if(0 == mpls_com_get_l3vpn_instance2(proute->vpn, MODULE_ID_ROUTE))
        {
            continue;
        }

        list_delete_node(route_blackhole_list, pnode);

        route_static_add(proute);

        route_static_free(proute);
    }

    if (0 != route_blackhole_list->count)
    {
        ROUTE_TIMER_ADD(route_traverse_route_blackhole_list, NULL, TRAVERSE_ROUTE_LIST_TIME);
    }
    else
    {
        list_free(route_blackhole_list);

        route_blackhole_list = NULL;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 显示 IPv6 活跃路由简略信息
 * @param[in ] : vty    - vty 全局结构
 * @param[in ] : proute - 活跃路由信息
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:35:12
 * @note       :
 */
static int routev6_static_show_active(struct vty *vty, struct route_entry *proute)
{
    char ip_mask[INET6_ADDRSTRLEN+5] = "";
    char destip[INET6_ADDRSTRLEN]    = "";
    char nexthop[INET6_ADDRSTRLEN]   = "";
    char ifname[IFNET_NAMESIZE]      = "";
    struct nhp_entry *pnhp     = NULL;
    const char       *protocol = NULL;
    int i;

    if (NULL == proute)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    /* 同一前缀可能对应多个 nhp，依次轮询 */
    for (i=0; i<proute->nhp_num; i++)
    {
        pnhp = &proute->nhp[i];

        memset(ifname, 0, sizeof(ifname));

        inet_ntop(AF_INET6, proute->prefix.addr.ipv6, destip, INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, pnhp->nexthop.addr.ipv6, nexthop, INET6_ADDRSTRLEN);
        sprintf(ip_mask, "%s/%d", destip, proute->prefix.prefixlen);

        vty_out(vty, "%s: %s%s", "Destip", ip_mask, VTY_NEWLINE);
       
        vty_out(vty, "%15s: %-50s%s", "Nexthop", nexthop, VTY_NEWLINE);

        switch (pnhp->protocol)
            {
                case ROUTE_PROTO_STATIC:
                    protocol = "static";
                    break;
                case ROUTE_PROTO_CONNECT:
                    protocol = "direct";
                    break;
                case ROUTE_PROTO_OSPF:
                case ROUTE_PROTO_OSPF6:
                    protocol = "ospf";
                    break;
                case ROUTE_PROTO_ISIS:
                case ROUTE_PROTO_ISIS6:
                    protocol = "isis";
                    break;
                case ROUTE_PROTO_RIP:
                case ROUTE_PROTO_RIPNG:
                    protocol = "rip";
                    break;
                case ROUTE_PROTO_IBGP:
                case ROUTE_PROTO_IBGP6:
                    protocol = "ibgp";
                    break;
                case ROUTE_PROTO_EBGP:
                case ROUTE_PROTO_EBGP6:
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

            vty_out(vty, "%15s: %-25s", "protocol", protocol);
            vty_out(vty, "%15s: %-19u", "vpn", pnhp->vpn);
            vty_out(vty, "%s", VTY_NEWLINE);
       
            vty_out(vty, "%15s: %-25u", "distance", pnhp->distance);
            vty_out(vty, "%15s: %-19u", "cost", pnhp->cost);
            vty_out(vty, "%s", VTY_NEWLINE);
      
            if (pnhp->action == NHP_ACTION_DROP)
            {
                vty_out(vty, "%15s: %-25s", "interface", "blackhole");
            }
            else
            {
                if (NHP_TYPE_LSP == pnhp->nhp_type)
                {
                    memcpy(ifname, "lsp", sizeof(ifname));
                }
                else
                {
                    ifm_get_name_by_ifindex(pnhp->ifindex, ifname);
                }

                vty_out(vty, "%15s: %-25s", "interface", pnhp->ifindex == 0 ? "-" : ifname);
            }

            if (proute->nhpinfo.nhp_type == NHP_TYPE_ECMP)
            {
                vty_out(vty, "%15s: %-19s", "flag", "ecmp");
            }
            else if (proute->nhpinfo.nhp_type == NHP_TYPE_FRR)
            {
                vty_out(vty, "%15s: %-19s", "flag", "frr");
            }
            else
            {
                vty_out(vty, "%15s: %-19s", "flag", " ");
            }

            vty_out(vty, "%s", VTY_NEWLINE);
       
            vty_out(vty, "%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 IPv6 路由表中所有路由的简略信息
 * @param[in ] : vty  - vty 全局结构
 * @param[in ] : prib - 原始路由前缀信息
 * @param[in ] : pnhp - 路由下一跳信息
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:35:59
 * @note       :
 */
static int routev6_static_show_all(struct vty *vty, struct rib_entry *prib,
                                    struct nhp_entry *pnhp)
{
    char ip_mask[INET6_ADDRSTRLEN+5] = "";
    char destip[INET6_ADDRSTRLEN]    = "";
    char nexthop[INET6_ADDRSTRLEN]   = "";
    char ifname[IFNET_NAMESIZE]      = "";
    const char *protocol = NULL;

    if ((NULL == prib) || (NULL == pnhp))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    inet_ntop(AF_INET6, prib->prefix.addr.ipv6, destip, INET6_ADDRSTRLEN);
    sprintf(ip_mask, "%s/%d", destip, prib->prefix.prefixlen);

    inet_ntop(AF_INET6, pnhp->nexthop.addr.ipv6, nexthop, INET6_ADDRSTRLEN);

    if (NHP_TYPE_LSP == pnhp->nhp_type)
    {
        memcpy(ifname, "lsp", sizeof(ifname));
    }
    else
    {
        ifm_get_name_by_ifindex(pnhp->ifindex, ifname);
    }

        vty_out(vty, "%s: %s%s", "Destip", ip_mask, VTY_NEWLINE);
    
        vty_out(vty, "%15s: %-50s%s", "Nexthop", nexthop, VTY_NEWLINE);
    
        switch (pnhp->protocol)
        {
            case ROUTE_PROTO_STATIC:
                protocol = "static";
                break;
            case ROUTE_PROTO_CONNECT:
                protocol = "direct";
                break;
            case ROUTE_PROTO_OSPF:
            case ROUTE_PROTO_OSPF6:
                protocol = "ospf";
                break;
            case ROUTE_PROTO_ISIS:
            case ROUTE_PROTO_ISIS6:
                protocol = "isis";
                break;
            case ROUTE_PROTO_RIP:
            case ROUTE_PROTO_RIPNG:
                protocol = "rip";
                break;
            case ROUTE_PROTO_IBGP:
            case ROUTE_PROTO_IBGP6:
                protocol = "ibgp";
                break;
            case ROUTE_PROTO_EBGP:
            case ROUTE_PROTO_EBGP6:
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

        vty_out(vty, "%15s: %-25s", "Protocol", protocol);
        vty_out(vty, "%15s: %-19d", "Preference", pnhp->distance);
        vty_out(vty, "%s", VTY_NEWLINE);
   
        if (pnhp->action == NHP_ACTION_DROP)
        {
            vty_out(vty, "%15s: %-25s", "Interface", "blackhole");
        }
        else
        {
            vty_out(vty, "%15s: %-25s", "interface", pnhp->ifindex == 0 ? "-" : ifname);
        }

        vty_out(vty, "%15s: %-19u", "instance", pnhp->instance);
        vty_out(vty, "%s", VTY_NEWLINE);

        vty_out(vty, "%15s: %-25s", "State", pnhp->down_flag ==
                    LINK_UP ? "UP":"DOWN");
        vty_out(vty, "%15s: %-19u", "Vpn", pnhp->vpn);
        vty_out(vty, "%s", VTY_NEWLINE);
    
        vty_out(vty, "%15s: %-25u", "Cost", pnhp->cost);
        vty_out(vty, "%15s: %-19s", "Description", pnhp->active == ROUTE_STATUS_ACTIVE ?
                    "download to fib":" ");
        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty, "%s", VTY_NEWLINE);
    
    return CMD_SUCCESS;
}


/**
* @brief      : 统计指定 vpn 实例的 IPv6 活跃路由数目
* @param[in ] : vty - vty 全局结构
* @param[in ] : vpn - vpn 实例号
* @param[out] :
* @return     : 成功返回 ERRNO_SUCCESS
* @author     : ZhangFj
* @date       : 2018年2月11日 10:40:29
* @note       : 需要轮询活跃路由表，流程中活跃路由数量统计不准确
*/
static int routev6_active_num_show(struct vty *vty, int vpn)
{
    struct ptree_node  *node   = NULL;
    struct route_entry *proute = NULL;
    uint32_t total_num = 0;
    int      i         = 0;

    PTREE_LOOP(routev6_tree[vpn], proute, node)
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
 * @brief      : 显示指定 vpn 实例的 IPv6 路由总数目
 * @param[in ] : vty - vty 全局结构
 * @param[in ] : vpn - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:08:37
 * @note       : 需要轮询路由表，流程中路由数量统计不准确
 */
static int routev6_total_num_show(struct vty *vty, int vpn)
{
    struct listnode   *plistnode = NULL;
    struct ptree_node *node      = NULL;
    struct rib_entry  *prib      = NULL;
    struct nhp_entry  *pnhp      = NULL;
    uint32_t total_num = 0;

    PTREE_LOOP(ribv6_tree[vpn], prib, node)
    {
        for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
        {
            if (pnhp != NULL)
            {
                total_num++;
            }
        }
    }

    vty_out(vty, "%20s: %-10d%s", "Routes", total_num, VTY_NEWLINE);
    
    vty_out(vty, "%s", VTY_NEWLINE);
  
    return ERRNO_SUCCESS;
}


/**
 * @brief      : 按协议类型显示指定 vpn 实例的 IPv6 路由数量
 * @param[in ] : vty - vty 全局结构
 * @param[in ] : vpn - vpn 实例号
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:42:06
 * @note       : 需要同时轮询路由表和活跃路由表
 */
static void routev6_statistics_num_show(struct vty *vty, int vpn)
{
    struct ptree_node  *node1     = NULL;
    struct route_entry *proute    = NULL;
    struct listnode    *plistnode = NULL;
    struct ptree_node  *node2     = NULL;
    struct rib_entry   *prib      = NULL;
    struct nhp_entry   *pnhp      = NULL;
    struct rib_global num = {ROUTE_NUM_MAX};
    uint32_t total_num  = 0;
    uint32_t active_num = 0;
    int      i          = 0;

    PTREE_LOOP(routev6_tree[vpn], proute, node1)
    {
        for (i=0; i<proute->nhp_num; i++)
        {
            switch (proute->nhp[i].protocol)
            {
                case ROUTE_PROTO_CONNECT:
                    num.num_direct_active++;
                    break;
                case ROUTE_PROTO_STATIC:
                    num.num_static_active++;
                    break;
                case ROUTE_PROTO_OSPF:
                case ROUTE_PROTO_OSPF6:
                    num.num_ospf_active++;
                    break;
                case ROUTE_PROTO_RIP:
                case ROUTE_PROTO_RIPNG:
                    num.num_rip_active++;
                    break;
                case ROUTE_PROTO_ISIS:
                case ROUTE_PROTO_ISIS6:
                    num.num_isis_active++;
                    break;
                case ROUTE_PROTO_IBGP:
                case ROUTE_PROTO_IBGP6:
                    num.num_ibgp_active++;
                    break;
                case ROUTE_PROTO_EBGP:
                case ROUTE_PROTO_EBGP6:
                    num.num_ebgp_active++;
                    break;
                case ROUTE_PROTO_LDP:
                    num.num_ldp_active++;
                    break;
                case ROUTE_PROTO_MPLSTP:
                    num.num_mplstp_active++;
                    break;
                case ROUTE_PROTO_RSVPTE:
                    num.num_rsvpte_active++;
                    break;
                default :
                    break;
            }
        }
    }

    PTREE_LOOP(ribv6_tree[vpn], prib, node2)
    {
        for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
        {
            if (pnhp != NULL)
            {
                switch (pnhp->protocol)
                {
                    case ROUTE_PROTO_CONNECT:
                        num.num_direct++;
                        break;
                    case ROUTE_PROTO_STATIC:
                        num.num_static++;
                        break;
                    case ROUTE_PROTO_OSPF:
                    case ROUTE_PROTO_OSPF6:
                        num.num_ospf++;
                        break;
                    case ROUTE_PROTO_RIP:
                    case ROUTE_PROTO_RIPNG:
                        num.num_rip++;
                        break;
                    case ROUTE_PROTO_ISIS:
                    case ROUTE_PROTO_ISIS6:
                        num.num_isis++;
                        break;
                    case ROUTE_PROTO_IBGP:
                    case ROUTE_PROTO_IBGP6:
                        num.num_ibgp++;
                        break;
                    case ROUTE_PROTO_EBGP:
                    case ROUTE_PROTO_EBGP6:
                        num.num_ebgp++;
                        break;
                    case ROUTE_PROTO_LDP:
                        num.num_ldp++;
                        break;
                    case ROUTE_PROTO_MPLSTP:
                        num.num_mplstp++;
                        break;
                    case ROUTE_PROTO_RSVPTE:
                        num.num_rsvpte++;
                        break;
                    default :
                        break;
                }
            }
        }
    }

    total_num = num.num_direct + num.num_static + num.num_ospf + num.num_isis
                + num.num_rip + num.num_ibgp + num.num_ebgp + num.num_ldp
                + num.num_mplstp + num.num_rsvpte;
    active_num = num.num_direct_active + num.num_static_active + num.num_ospf_active
                + num.num_isis_active + num.num_rip_active + num.num_ibgp_active
                + num.num_ebgp_active + num.num_ldp_active + num.num_mplstp_active
                + num.num_rsvpte_active;

    vty_out(vty, "%-10s%-10s%-10s%s", "Protocol", "Total", "Active", VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "DIRECT", num.num_direct,
                num.num_direct_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "STATIC", num.num_static,
                num.num_static_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "OSPF", num.num_ospf,
                num.num_ospf_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "ISIS", num.num_isis,
                num.num_isis_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "RIP", num.num_rip,
                num.num_rip_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "IBGP", num.num_ibgp,
                num.num_ibgp_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "EBGP", num.num_ebgp,
                num.num_ebgp_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "LDP", num.num_ldp,
                num.num_ldp_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "MPLSTP", num.num_mplstp,
                num.num_mplstp_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "RSVPTE", num.num_rsvpte,
                num.num_rsvpte_active, VTY_NEWLINE);
    vty_out(vty, "%-10s%-10d%-10d%s", "Total", total_num,
                active_num, VTY_NEWLINE);
    vty_out(vty,"%s",VTY_NEWLINE);

    return;
}


/**
 * @brief      : 自动生成 IPv6 router_id
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:14:14
 * @note       :
 */
void routev6_set_router_id(void)
{
    struct route_if    *pif     = NULL;
    struct hash_bucket *pbucket = NULL;
    int      cursor  = 0;
    uint32_t ifindex = 0;

    if (ROUTE_IPV6_ADDR_EXIST(g_route.routerv6_id.ipv6))
    {
        return;
    }

    /* 优先选用 loopback 0 接口 ip */
    ifindex = ifm_get_ifindex_by_name("loopback", "0");

    pif = route_if_lookup(ifindex);
    if ((NULL != pif) && (ROUTE_IPV6_ADDR_EXIST(pif->intf.ipv6[0].addr)))
    {
        memcpy(g_route.routerv6_id.ipv6, pif->intf.ipv6[0].addr, IPV6_ADDR_LEN);

        ROUTE_IPC_SENDTO_FTM(&g_route.routerv6_id.ipv6, IPV6_ADDR_LEN, 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
                    IPC_TYPE_FIB, ROUTE_SUBTYPE_ROUTERV6_ID, IPC_OPCODE_ADD, 0);

        return;
    }

    /* 次选 loopback 1 接口 ip */
    ifindex = ifm_get_ifindex_by_name("loopback", "1");

    pif = route_if_lookup(ifindex);
    if ((NULL != pif) && (ROUTE_IPV6_ADDR_EXIST(pif->intf.ipv6[0].addr)))
    {
        memcpy(g_route.routerv6_id.ipv6, pif->intf.ipv6[0].addr, IPV6_ADDR_LEN);

        ROUTE_IPC_SENDTO_FTM(&g_route.routerv6_id.ipv6, IPV6_ADDR_LEN, 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
                    IPC_TYPE_FIB, ROUTE_SUBTYPE_ROUTERV6_ID, IPC_OPCODE_ADD, 0);

        return;
    }

    ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)"1/0/1");

    /* 选任意接口 ip ，不包含管理口 */
    HASH_BUCKET_LOOP(pbucket, cursor, route_if_table)
    {
        pif = (struct route_if *)pbucket->data;
        if ((NULL == pif) || (!ROUTE_IPV6_ADDR_EXIST(pif->intf.ipv6[0].addr)))
        {
            continue;
        }

        /* router-id 不选管理口 ip 地址 */
        if (pif->ifindex == ifindex)
        {
            continue;
        }

        memcpy(g_route.routerv6_id.ipv6, pif->intf.ipv6[0].addr, IPV6_ADDR_LEN);

        ROUTE_IPC_SENDTO_FTM(&g_route.routerv6_id.ipv6, IPV6_ADDR_LEN, 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
                    IPC_TYPE_FIB, ROUTE_SUBTYPE_ROUTERV6_ID, IPC_OPCODE_ADD, 0);

        return;
    }

    return;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月30日 16:12:15
 * @note       :
 */
static int routev6_traverse_route_blackhole_list(struct thread *thread)
{
    struct route_static *proute = NULL;
    struct listnode     *pnode  = NULL;
    struct listnode     *pnext  = NULL;

    for (ALL_LIST_ELEMENTS(routev6_blackhole_list, pnode, pnext, proute))
    {
        if (proute == NULL)
        {
            continue;
        }

        if(0 == mpls_com_get_l3vpn_instance2(proute->vpn, MODULE_ID_ROUTE))
        {
            continue;
        }

        list_delete_node(routev6_blackhole_list, pnode);

        routev6_static_add(proute);

        routev6_static_free(proute);
    }

    if (0 != routev6_blackhole_list->count)
    {
        ROUTE_TIMER_ADD(routev6_traverse_route_blackhole_list, NULL, TRAVERSE_ROUTE_LIST_TIME);
    }
    else
    {
        list_free(routev6_blackhole_list);

        routev6_blackhole_list = NULL;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 检查 IPv6 地址中冒号数量
 * @param[in ] : src - IPv6 地址字符串
 * @param[out] :
 * @return     : 合法返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:48:00
 * @note       :
 */
int routev6_check_ipv6_str(const char *src)
{
    int colon = 0;

    if (NULL == src)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    while(*src != '\0')
    {
        if ((*src++ == ':') && (++colon > 7))
        {
            return ERRNO_FAIL;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 切换至 route 节点命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:50:08
 * @note       :
 */
DEFUN(route_node,
    route_node_cmd,
    "route",
    ROUTE_CLI_INFO)
{
    vty->node = ROUTE_NODE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月31日 11:04:00
 * @note       :
 */
DEFUN(route_ecmp_enable,
    route_ecmp_enable_cmd,
    "ip route ecmp enable",
    ROUTE_CLI_INFO)
{
    g_route.route_ecmp_flag = ENABLE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月31日 11:04:45
 * @note       :
 */
DEFUN(no_route_ecmp_enable,
    no_route_ecmp_enable_cmd,
    "no ip route ecmp enable",
    ROUTE_CLI_INFO)
{
    g_route.route_ecmp_flag = DISABLE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月31日 11:04:14
 * @note       :
 */
DEFUN(route_frr_enable,
    route_frr_enable_cmd,
    "ip route frr enable",
    ROUTE_CLI_INFO)
{
    g_route.route_frr_flag = ENABLE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月31日 11:05:16
 * @note       :
 */
DEFUN(no_route_frr_enable,
    no_route_frr_enable_cmd,
    "no ip route frr enable",
    ROUTE_CLI_INFO)
{
    g_route.route_frr_flag = DISABLE;

    return CMD_SUCCESS;
}


/**
 * @brief      : 配置 router-id 命令
 * @param[in ] : A.B.C.D - 点分十进制 ip 地址
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 10:51:07
 * @note       :
 */
DEFUN(route_router_id,
    route_router_id_cmd,
    "router-id A.B.C.D",
    ROUTE_CLI_INFO)
{
    uint32_t router_id = 0;
    int      ret;

    /* argv[0]: ip 地址 */
    router_id = inet_strtoipv4((char *)argv[0]);
    if (0 == router_id)
    {
        vty_error_out(vty, "The router-id is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (!inet_valid_network(router_id))
    {
        vty_error_out(vty, "The router-id is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    g_route.router_id = router_id;

    ret = ROUTE_IPC_SENDTO_FTM(&g_route.router_id, IP_ADDR_LEN, 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
                        IPC_TYPE_FIB, ROUTE_SUBTYPE_ROUTER_ID, IPC_OPCODE_ADD, 0);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
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
DEFUN(ip_route_static,
    ip_route_static_cmd,
    "ip route A.B.C.D/M {ethernet USP | gigabitethernet USP | xgigabitethernet USP | "
    "tunnel USP | loopback <0-128> | vlanif <1-4095> | trunk TRUNK | "
    "nexthop A.B.C.D | distance <1-255>| vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    char prefix[INET_ADDRSTRLEN] = "";
    struct route_static lroute;
    struct prefix_ipv4  destip;
    struct prefix_ipv4  nexthop;
    const char *pintf     = NULL;
    const char *pintf_num = NULL;
    int ret;

    memset(&lroute, 0, sizeof(struct route_static));
    lroute.action   = NHP_ACTION_FORWARD;
    lroute.nhp_type = NHP_TYPE_IP;

    /* argv[0]: 路由前缀和掩码 */
    ret = str2prefix_ipv4(argv[0], &destip);
    if (0 == ret)
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
    if (NULL != argv[1])
    {
        pintf     = "ethernet";
        pintf_num = argv[1];
    }
	else if (NULL != argv[2])
    {
        pintf     = "gigabitethernet";
        pintf_num = argv[2];
    }
	else if (NULL != argv[3])
    {
        pintf     = "xgigabitethernet";
        pintf_num = argv[3];
    }
    else if (NULL != argv[4])
    {
        pintf           = "tunnel";
        pintf_num       = argv[4];
        lroute.nhp_type = NHP_TYPE_TUNNEL;
    }
    else if (NULL != argv[5])
    {
        pintf         = "loopback";
        pintf_num     = argv[5];
        lroute.action = NHP_ACTION_TOCPU;
    }
    else if (NULL != argv[6])
    {
        pintf     = "vlanif";
        pintf_num = argv[6];
    }
    else if (NULL != argv[7])
    {
        pintf     = "trunk";
        pintf_num = argv[7];
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
        if (((NULL != argv[1]) || (NULL != argv[2]) || (NULL != argv[3]))
            && (NULL == argv[8]))
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
    if (NULL != argv[8])
    {
        ret = str2prefix_ipv4(argv[8], &nexthop);
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
    if (NULL != argv[9])
    {
        lroute.distance = atoi(argv[9]);
    }
    else
    {
        lroute.distance = ROUTE_METRIC_STATIC;
    }

    /* argv[10]: vpn */
    if (NULL != argv[10])
    {
        lroute.vpn = (uint16_t)atoi(argv[10]);

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
 * @brief      : 配置黑洞路由命令
 * @param[in ] : A.B.C.D/M        - 路由前缀和掩码
 * @param[in ] : vpn <1-128>      - vpn 实例号
 * @param[in ] : distance <1-255> - 路由优先级，默认为 ROUTE_METRIC_STATIC
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 11:35:09
 * @note       :
 */
DEFUN(ip_route_balckhole,
    ip_route_balckhole_cmd,
    "ip route A.B.C.D/M blackhole {vpn <1-128> | distance <1-255>}",
    ROUTE_CLI_INFO)
{
    char prefix[INET_ADDRSTRLEN] = "";
    struct route_static *proute = NULL;
    struct route_static  lroute;
    struct prefix_ipv4   destip;
    int                  ret;

    memset(&lroute, 0, sizeof(struct route_static));
    lroute.action   = NHP_ACTION_DROP;
    lroute.nhp_type = NHP_TYPE_IP;
    lroute.distance = ROUTE_METRIC_STATIC;

    /* argv[0]: 路由前缀和掩码 */
    ret = str2prefix_ipv4(argv[0], &destip);
    if (0 == ret)
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

    /* argv[1]: vpn */
    if (NULL != argv[1])
    {
        lroute.vpn = (uint16_t)atoi(argv[1]);

        if (1 != vty->config_read_flag)
        {
            if(0 == mpls_com_get_l3vpn_instance2(lroute.vpn, MODULE_ID_ROUTE))

            {
                vty_error_out(vty, "VPN-Instance does not exist.%s", VTY_NEWLINE);
                return CMD_WARNING;
            }

        }
    }

    /* argv[2]: distance */
    if (NULL != argv[2])
    {
        lroute.distance = (uint16_t)atoi(argv[2]);
    }

    if (1 == vty->config_read_flag)
    {
        if (NULL == route_blackhole_list)
        {
            route_blackhole_list = list_new();
        }

        if (NULL != route_blackhole_list)
        {
            proute = route_static_create();
            if (NULL != proute)
            {
                memcpy(proute, &lroute, sizeof(struct route_static));

                listnode_add(route_blackhole_list, proute);

                if (1 == route_blackhole_list->count)
            	{
            		ROUTE_TIMER_ADD(route_traverse_route_blackhole_list, NULL, TRAVERSE_ROUTE_LIST_TIME);
            	}
            }
        }
    }

    /* 前缀相同的静态路由和黑洞路由不能同时配置 */
    proute = route_static_lookup(&lroute);
    if (NULL != proute)
    {
        if (!((NHP_ACTION_DROP == proute->action) && (0 == proute->nexthop.addr.ipv4)))
        {
            vty_error_out(vty, "Static routing already exists !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    /* 黑洞路由添加 */
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
DEFUN(no_ip_route_static,
    no_ip_route_static_cmd,
    "no ip route A.B.C.D/M {ethernet USP |gigabitethernet USP |xgigabitethernet USP | "
    "tunnel USP | loopback <0-128> | vlanif <1-4095> | trunk TRUNK | "
    "nexthop A.B.C.D | vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    struct route_static *sroute = NULL;
    struct route_static  lroute;
    struct prefix_ipv4   destip;
    struct prefix_ipv4   nexthop;
    const char *pintf     = NULL;
    const char *pintf_num = NULL;
    int                  ret;

    memset(&lroute, 0, sizeof(struct route_static));

    /* argv[0]: 路由前缀和掩码 */
    ret = str2prefix_ipv4(argv[0], &destip);
    if (0 == ret)
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
    if (NULL != argv[1])
    {
        pintf     = "ethernet";
        pintf_num = argv[1];
    }
	else if (NULL != argv[2])
    {
        pintf     = "gigabitethernet";
        pintf_num = argv[2];
    }
	else if (NULL != argv[3])
    {
        pintf     = "xgigabitethernet";
        pintf_num = argv[3];
    }
    else if (NULL != argv[4])
    {
        pintf     = "tunnel";
        pintf_num = argv[4];
    }
    else if (NULL != argv[5])
    {
        pintf     = "loopback";
        pintf_num = argv[5];
    }
    else if (NULL != argv[6])
    {
        pintf     = "vlanif";
        pintf_num = argv[6];
    }
    else if (NULL != argv[7])
    {
        pintf     = "trunk";
        pintf_num = argv[7];
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
    if (NULL != argv[8])
    {
        ret = str2prefix_ipv4(argv[8], &nexthop);
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
    if (NULL != argv[9])
    {
        lroute.vpn = (uint16_t)atoi(argv[9]);
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


/**
 * @brief      : 删除黑洞路由命令
 * @param[in ] : A.B.C.D/M   - 路由前缀和掩码
 * @param[in ] : vpn <1-128> - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 11:50:33
 * @note       : 该命令行只能删除黑洞路由
 */
DEFUN(no_ip_route_blackhole,
    no_ip_route_blackhole_cmd,
    "no ip route A.B.C.D/M blackhole {vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    struct route_static *sroute = NULL;
    struct route_static  lroute;
    struct prefix_ipv4   destip;
    int                  ret;

    memset(&lroute, 0, sizeof(struct route_static));

    /* argv[0]: 路由前缀和掩码 */
    ret = str2prefix_ipv4(argv[0], &destip);
    if (0 == ret)
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

    /* argv[1]: vpn */
    if (NULL != argv[1])
    {
        lroute.vpn = (uint16_t)atoi(argv[1]);
    }

    /* 该命令行不能删除静态路由 */
    sroute = route_static_lookup(&lroute);
    if (NULL == sroute)
    {
        vty_error_out(vty, "The specified static routing does not exist.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ((sroute->ifindex == 0) && (sroute->action == NHP_ACTION_DROP))
    {
        ret = route_static_delete(&lroute);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else
    {
        vty_error_out(vty, "The specified static routing does not exist.%s", VTY_NEWLINE);

        return CMD_WARNING;
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
DEFUN(show_ip_route_single,
    show_ip_route_single_cmd,
    "show ip route (A.B.C.D/M|A.B.C.D) {vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    struct route_entry *proute = NULL;
    struct route_static lroute;
    struct prefix_ipv4  destip;
    char *pnt = NULL;
    int ret;

    memset(&lroute, 0 ,sizeof(struct route_static));

    /* argv[0]: 路由前缀和掩码 */
    ret = str2prefix_ipv4(argv[0], &destip);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lroute.prefix.type      = INET_FAMILY_IPV4;
    lroute.prefix.addr.ipv4 = destip.prefix.s_addr;
    lroute.prefix.prefixlen = destip.prefixlen;

    ret = route_valid_show_prefix(lroute.prefix.addr.ipv4);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lroute.prefix.addr.ipv4 = ntohl(lroute.prefix.addr.ipv4);

    /* argv[1]: vpn */
    if (NULL != argv[1])
    {
        lroute.vpn = (uint16_t)atoi(argv[1]);
    }

    /* 解析 argv[0] 格式，pnt 为 NULL 不带 '/' */
    pnt = strchr(argv[0], '/');
    if (NULL == pnt)
    {
        proute = route_match(&lroute.prefix, lroute.vpn);
    }
    else
    {
        proute = route_lookup(&lroute.prefix, lroute.vpn);
    }

    if (NULL == proute)
    {
        return CMD_SUCCESS;
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

    route_static_show_active(vty, proute);

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
DEFUN(show_ip_route_active,
    show_ip_route_active_cmd,
    "show ip route active {vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    struct ptree_node  *node   = NULL;
    struct route_entry *proute = NULL;
    int num = 0;
    int vpn = 0;
    int ret;


    /* argv[0]: vpn */
    if (NULL != argv[0])
    {
        vpn = atoi(argv[0]);
    }

    PTREE_LOOP(route_tree[vpn], proute, node)
    {
        /* num 等于 1，输出一次信息头 */
        if (++num == 1)
        {
            vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
            
			vty_out(vty, "Routing Tables: Active%s", VTY_NEWLINE);
            
			vty_out(vty, "%s", VTY_NEWLINE);
            
            route_active_num_show(vty, vpn);

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

        ret = route_static_show_active(vty, proute);
        if (CMD_CONTINUE == ret)
        {
            return ret;
        }
    }

    /* 批量回显命令行返回成功，需要将该变量置 0 */
    route_line = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示所有路由命令
 * @param[in ] : vpn <1-128> - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:12:13
 * @note       : 不指定 vpn 只显示公网路由
 */
DEFUN(show_ip_route,
    show_ip_route_cmd,
    "show ip route {vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    struct listnode   *plistnode = NULL;
    struct ptree_node *node      = NULL;
    struct rib_entry  *prib      = NULL;
    struct nhp_entry  *pnhp      = NULL;
    int num = 0;
    int vpn = 0;
    int ret;


    /* argv[0]: vpn */
    if (NULL != argv[0])
    {
        vpn = atoi(argv[0]);
    }

    PTREE_LOOP(rib_tree[vpn], prib, node)
    {
        /* num 等于 1，输出一次信息头 */
        if (++num == 1)
        {

            vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
            
			vty_out(vty, "Routing Tables: All%s", VTY_NEWLINE);            

			vty_out(vty, "%s", VTY_NEWLINE);
			            
			route_total_num_show(vty, vpn);

            vty_out(vty, "%s", VTY_NEWLINE);
            
        }

        for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
        {
            ret = route_static_show_all(vty, prib, pnhp);
            if (ret == CMD_CONTINUE)
            {
                return ret;
            }
        }
    }

    /* 批量回显命令行返回成功，需要将该变量置 0 */
    route_line = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示路由统计信息命令
 * @param[in ] : vpn <1-128> - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:15:42
 * @note       : 不指定 vpn 只显示公网路由统计信息
 */
DEFUN(show_ip_route_statistics,
    show_ip_route_statistics_cmd,
    "show ip route statistics {vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    int vpn = 0;

    /* argv[0]: vpn */
    if (NULL != argv[0])
    {
        vpn = atoi(argv[0]);
    }

    route_statistics_num_show(vty, vpn);

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月31日 11:08:13
 * @note       :
 */
DEFUN(routev6_ecmp_enable,
    routev6_ecmp_enable_cmd,
    "ipv6 route ecmp enable",
    ROUTE_CLI_INFO)
{
    g_route.routev6_ecmp_flag = ENABLE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月31日 11:09:02
 * @note       :
 */
DEFUN(no_routev6_ecmp_enable,
    no_routev6_ecmp_enable_cmd,
    "no ipv6 route ecmp enable",
    ROUTE_CLI_INFO)
{
    g_route.routev6_ecmp_flag = DISABLE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月31日 11:09:25
 * @note       :
 */
DEFUN(routev6_frr_enable,
    routev6_frr_enable_cmd,
    "ipv6 route frr enable",
    ROUTE_CLI_INFO)
{
    g_route.routev6_frr_flag = ENABLE;

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月31日 11:09:57
 * @note       :
 */
DEFUN(no_routev6_frr_enable,
    no_routev6_frr_enable_cmd,
    "no ipv6 route frr enable",
    ROUTE_CLI_INFO)
{
    g_route.routev6_frr_flag = DISABLE;

    return CMD_SUCCESS;
}


/**
 * @brief      : 配置 IPv6 router-id 命令
 * @param[in ] : X:X:X:X:X:X:X:X - IPv6 地址，以冒号分隔
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:17:07
 * @note       :
 */
DEFUN(route_routerv6_id,
    route_routerv6_id_cmd,
    "router-id X:X:X:X:X:X:X:X",
    ROUTE_CLI_INFO)
{
    struct prefix_ipv6 routev6_id;
    int ret;

    /* argv[0]: IPv6 地址 */
    ret = routev6_check_ipv6_str(argv[0]);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "The router-id address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = str2prefix_ipv6(argv[0], &routev6_id);
    if (0 == ret)
    {
        vty_error_out(vty, "The router-id address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 未指定地址 */
    if (!ROUTE_IF_IPV6_EXIST(routev6_id.prefix.s6_addr))
    {
        vty_error_out(vty, "The router-id address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* loopback 地址 */
    if (IN6_IS_ADDR_LOOPBACK(&routev6_id.prefix))
    {
        vty_error_out(vty, "The router-id address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    memcpy(g_route.routerv6_id.ipv6, routev6_id.prefix.s6_addr, IPV6_ADDR_LEN);

    ret = ROUTE_IPC_SENDTO_FTM(&g_route.routerv6_id.ipv6, IPV6_ADDR_LEN, 1, MODULE_ID_FTM,
                        MODULE_ID_ROUTE, IPC_TYPE_FIB, ROUTE_SUBTYPE_ROUTERV6_ID,
                        IPC_OPCODE_ADD, 0);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 配置 IPv6 静态路由命令
 * @param[in ] : X:X:X:X:X:X:X:X/M       - 路由前缀和掩码
 * @param[in ] : ethernet USP            - 出接口类型为 ethernet
 * @param[in ] : gigabitethernet USP     - 出接口类型为 gigabitethernet
 * @param[in ] : xgigabitethernet USP    - 出接口类型为 xgigabitethernet
 * @param[in ] : tunnel USP              - 出接口类型为 tunnel
 * @param[in ] : loopback <0-128>        - 出接口类型为 loopback
 * @param[in ] : vlanif <1-4095>         - 出接口类型为 vlanif
 * @param[in ] : trunk TRUNK             - 出接口类型为 trunk
 * @param[in ] : nexthop X:X:X:X:X:X:X:X - 路由下一跳地址
 * @param[in ] : distance <1-255>        - 路由优先级，默认为 ROUTE_METRIC_STATIC
 * @param[in ] : vpn <1-128>             - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:23:56
 * @note       : 大括号 '{}' 表示多选 n(n大于等于 0，小于等于 n)，但这里接口类型最多只能选一个
 */
DEFUN(ipv6_route_static,
    ipv6_route_static_cmd,
    "ipv6 route X:X:X:X:X:X:X:X/M {ethernet USP | gigabitethernet USP | xgigabitethernet "
    "USP | tunnel USP | loopback <0-128> | vlanif <1-4095> | trunk TRUNK | "
    "nexthop X:X:X:X:X:X:X:X | distance <1-255>| vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    char prefix[INET6_ADDRSTRLEN] = "";
    struct l3vpn_entry *pl3vpn = NULL;
    struct route_static lroute;
    struct prefix_ipv6  srcip;
    struct prefix_ipv6  destip;
    struct prefix_ipv6  nexthop;
    const char *pintf     = NULL;
    const char *pintf_num = NULL;
    char       *pnt       = NULL;
    int mask_len = 0;
    int ret;

    memset(&lroute, 0, sizeof(struct route_static));
    lroute.action   = NHP_ACTION_FORWARD;
    lroute.nhp_type = NHP_TYPE_IP;
    lroute.distance = ROUTE_METRIC_STATIC;

    /* argv[0]: 路由前缀和掩码 */
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

    mask_len = atoi(++pnt);
    if ((mask_len<0) || (mask_len>IPV6_MAX_PREFIXLEN))
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = str2prefix_ipv6(argv[0], &destip);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 链路本地地址 */
    if ((ipv6_is_linklocal((struct ipv6_addr *)&destip.prefix.s6_addr)
        && (!ipv6_is_sitelocal((struct ipv6_addr *)&destip.prefix.s6_addr))))
    {
        vty_error_out(vty, "The destination address cannot be a link-local address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 多播地址 */
    if (ipv6_is_multicast((struct ipv6_addr *)&destip.prefix.s6_addr))
    {
        vty_error_out(vty, "The destination address cannot be a multicast address.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    srcip = destip;
    apply_mask_ipv6(&destip);

    lroute.prefix.type      = INET_FAMILY_IPV6;
    lroute.prefix.prefixlen = destip.prefixlen;
    memcpy(lroute.prefix.addr.ipv6, destip.prefix.s6_addr, IPV6_ADDR_LEN);

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
    if (NULL != argv[1])
    {
        pintf     = "ethernet";
        pintf_num = argv[1];
    }
	else if (NULL != argv[2])
    {
        pintf     = "gigabitethernet";
        pintf_num = argv[2];
    }
	else if (NULL != argv[3])
    {
        pintf     = "xgigabitethernet";
        pintf_num = argv[3];
    }
    else if (NULL != argv[4])
    {
        pintf           = "tunnel";
        pintf_num       = argv[4];
        lroute.nhp_type = NHP_TYPE_TUNNEL;
    }
    else if (NULL != argv[5])
    {
        pintf         = "loopback";
        pintf_num     = argv[5];
        lroute.action = NHP_ACTION_TOCPU;
    }
    else if (NULL != argv[6])
    {
        pintf     = "vlanif";
        pintf_num = argv[6];
    }
    else if (NULL != argv[7])
    {
        pintf     = "trunk";
        pintf_num = argv[7];
    }

    if (NULL != pintf)
    {
        lroute.ifindex = ifm_get_ifindex_by_name(pintf, pintf_num);
        if (lroute.ifindex == 0)
        {
            vty_error_out(vty, "Specify the illegal interface.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        /* 配置出接口为以太类型，但未配置下一跳时，路由前缀的掩码必须是 128 */
        if (((NULL != argv[1]) || (NULL != argv[2]) || (NULL != argv[3]))
            && (NULL == argv[8]))
        {
            if (lroute.prefix.prefixlen != IPV6_MAX_BITLEN)
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
    if (NULL != argv[8])
    {
        ret = routev6_check_ipv6_str(argv[8]);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "The next-hop address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        ret = str2prefix_ipv6(argv[8], &nexthop);
        if (0 == ret)
        {
            vty_error_out(vty, "The next-hop address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        /* 未指定地址 */
        if (!ROUTE_IF_IPV6_EXIST(nexthop.prefix.s6_addr))
        {
            vty_error_out(vty, "The next-hop address cannot be zero.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        /* loopback 地址 */
        if (IN6_IS_ADDR_LOOPBACK(&nexthop.prefix))
        {
            vty_error_out(vty, "The next-hop address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        /* 多播地址 */
        if (ipv6_is_multicast((struct ipv6_addr *)&nexthop.prefix.s6_addr))
        {
            vty_error_out(vty, "The next-hop cannot be multicast address.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        /* 下一跳为链路本地地址必须指定出接口 */
        if (((ipv6_is_linklocal((struct ipv6_addr *)&nexthop.prefix.s6_addr))
            && (!ipv6_is_sitelocal((struct ipv6_addr *)&nexthop.prefix.s6_addr)))
            && (0 == lroute.ifindex))
        {
            vty_error_out(vty, "An interface must be specefied for a link-local next-hop.%s",
                            VTY_NEWLINE);

            return CMD_WARNING;
        }

        lroute.nexthop.type = INET_FAMILY_IPV6;
        memcpy(lroute.nexthop.addr.ipv6, nexthop.prefix.s6_addr, IPV6_ADDR_LEN);
    }

    /* argv[9]: distance */
    if (NULL != argv[9])
    {
        lroute.distance = atoi(argv[9]);
    }

    /* argv[10]: vpn */
    if (NULL != argv[10])
    {
        lroute.vpn = (uint16_t)atoi(argv[10]);

        if (1 != vty->config_read_flag)
        {
            if(0 == mpls_com_get_l3vpn_instance2(lroute.vpn, MODULE_ID_ROUTE))
            {
                vty_error_out(vty, "VPN-Instance does not exist.%s", VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
    }

    /* IPv6 静态路由添加 */
    ret = routev6_static_add(&lroute);
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
    if (memcmp(&srcip, &destip, sizeof(struct prefix_ipv6)))
    {
        inet_ntop(AF_INET6, destip.prefix.s6_addr, prefix, INET6_ADDRSTRLEN);

        vty_info_out(vty, "The destination address and mask of the configured"
            " static route mismatched, and the static route %s/%d was generated.%s",
            prefix, lroute.prefix.prefixlen, VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 配置 IPv6 黑洞路由命令
 * @param[in ] : X:X:X:X:X:X:X:X/M - 路由前缀和掩码
 * @param[in ] : distance <1-255>  - 路由优先级，默认为 ROUTE_METRIC_STATIC
 * @param[in ] : vpn <1-128>       - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:32:59
 * @note       :
 */
DEFUN(ipv6_route_balckhole,
    ipv6_route_balckhole_cmd,
    "ipv6 route X:X:X:X:X:X:X:X/M blackhole {vpn <1-128> | distance <1-255>}",
    ROUTE_CLI_INFO)
{
    char prefix[INET6_ADDRSTRLEN] = "";
    struct route_static *proute = NULL;
    struct route_static  lroute;
    struct prefix_ipv6   srcip;
    struct prefix_ipv6   destip;
    char *pnt = NULL;
    int   mask_len = 0;
    int   ret;

    memset(&lroute, 0, sizeof(struct route_static));
    lroute.action   = NHP_ACTION_DROP;
    lroute.nhp_type = NHP_TYPE_IP;
    lroute.distance = ROUTE_METRIC_STATIC;

    /* argv[0]: 路由前缀和掩码 */
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

    mask_len = atoi(++pnt);
    if ((mask_len<0) || (mask_len>IPV6_MAX_PREFIXLEN))
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = str2prefix_ipv6(argv[0], &destip);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 链路本地地址 */
    if ((ipv6_is_linklocal((struct ipv6_addr *)&destip.prefix.s6_addr)
        && (!ipv6_is_sitelocal((struct ipv6_addr *)&destip.prefix.s6_addr))))
    {
        vty_error_out(vty, "The destination address cannot be a link-local address.%s",
                        VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* 多播地址 */
    if (ipv6_is_multicast((struct ipv6_addr *)&destip.prefix.s6_addr))
    {
        vty_error_out(vty, "The destination address cannot be a multicast address.%s",
                        VTY_NEWLINE);

        return CMD_WARNING;
    }

    srcip = destip;
    apply_mask_ipv6(&destip);

    lroute.prefix.type      = INET_FAMILY_IPV6;
    lroute.prefix.prefixlen = destip.prefixlen;
    memcpy(lroute.prefix.addr.ipv6, destip.prefix.s6_addr, IPV6_ADDR_LEN);

    /* argv[1]: vpn */
    if (NULL != argv[1])
    {
        lroute.vpn = (uint16_t)atoi(argv[1]);

        if (1 != vty->config_read_flag)
        {
            if(0 == mpls_com_get_l3vpn_instance2(lroute.vpn, MODULE_ID_ROUTE))
            {
                vty_error_out(vty, "VPN-Instance does not exist.%s", VTY_NEWLINE);

                return CMD_WARNING;
            }
        }
    }

    /* argv[2]: distance */
    if (NULL != argv[2])
    {
        lroute.distance = (uint16_t)atoi(argv[2]);
    }

    if (1 == vty->config_read_flag)
    {
        if (NULL == routev6_blackhole_list)
        {
            routev6_blackhole_list = list_new();
        }

        if (NULL != routev6_blackhole_list)
        {
            proute = route_static_create();
            if (NULL != proute)
            {
                memcpy(proute, &lroute, sizeof(struct route_static));

                listnode_add(routev6_blackhole_list, proute);

                if (1 == routev6_blackhole_list->count)
            	{
            		ROUTE_TIMER_ADD(routev6_traverse_route_blackhole_list, NULL, TRAVERSE_ROUTE_LIST_TIME);
            	}
            }
        }
    }

    /* 前缀相同的静态路由与黑洞路由不能同时配置 */
    proute = routev6_static_lookup(&lroute);
    if ((NULL != proute) && (NHP_ACTION_DROP != proute->action))
    {
        vty_error_out(vty, "Static routing already exists !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* IPv6 黑洞路由添加 */
    ret = routev6_static_add(&lroute);
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
    if (memcmp(&srcip, &destip, sizeof(struct prefix_ipv6)))
    {
        inet_ntop(AF_INET6, destip.prefix.s6_addr, prefix, INET6_ADDRSTRLEN);

        vty_info_out(vty, "The destination address and mask of the configured"
            " static route mismatched, and the static route %s/%d was generated.%s",
            prefix, lroute.prefix.prefixlen, VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除 IPv6 静态路由命令
 * @param[in ] : X:X:X:X:X:X:X:X/M       - 路由前缀和掩码
 * @param[in ] : ethernet USP            - 出接口类型为 ethernet
 * @param[in ] : gigabitethernet USP     - 出接口类型为 gigabitethernet
 * @param[in ] : xgigabitethernet USP    - 出接口类型为 xgigabitethernet
 * @param[in ] : tunnel USP              - 出接口类型为 tunnel
 * @param[in ] : loopback <0-128>        - 出接口类型为 loopback
 * @param[in ] : vlanif <1-4095>         - 出接口类型为 vlanif
 * @param[in ] : trunk TRUNK             - 出接口类型为 trunk
 * @param[in ] : nexthop X:X:X:X:X:X:X:X - 路由下一跳地址
 * @param[in ] : vpn <1-128>             - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:37:31
 * @note       : 大括号 '{}' 表示多选 n(n大于等于 0，小于等于 n)，但这里接口类型最多只能选一个
 */
DEFUN(no_ipv6_route_static,
    no_ipv6_route_static_cmd,
    "no ipv6 route X:X:X:X:X:X:X:X/M {ethernet USP | gigabitethernet USP | xgigabitethernet "
    "USP | tunnel USP | loopback <0-128> | vlanif <1-4095> | trunk TRUNK | "
    "nexthop X:X:X:X:X:X:X:X | vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    struct route_static *sroute = NULL;
    struct route_static  lroute;
    struct prefix_ipv6   destip;
    struct prefix_ipv6   nexthop;
    const char *pintf     = NULL;
    const char *pintf_num = NULL;
    char       *pnt       = NULL;
    int   mask_len = 0;
    int   ret;

    memset(&lroute, 0, sizeof(struct route_static));

    /* argv[0]: 路由前缀和掩码 */
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

    mask_len = atoi(++pnt);
    if ((mask_len<0) || (mask_len>IPV6_MAX_PREFIXLEN))
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = str2prefix_ipv6(argv[0], &destip);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lroute.prefix.type      = INET_FAMILY_IPV6;
    lroute.prefix.prefixlen = destip.prefixlen;
    memcpy(lroute.prefix.addr.ipv6, destip.prefix.s6_addr, IPV6_ADDR_LEN);

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
    if (NULL != argv[1])
    {
        pintf     = "ethernet";
        pintf_num = argv[1];
    }
	else if (NULL != argv[2])
    {
        pintf     = "gigabitethernet";
        pintf_num = argv[2];
    }
	else if (NULL != argv[3])
    {
        pintf     = "xgigabitethernet";
        pintf_num = argv[3];
    }
    else if (NULL != argv[4])
    {
        pintf     = "tunnel";
        pintf_num = argv[4];
    }
    else if (NULL != argv[5])
    {
        pintf     = "loopback";
        pintf_num = argv[5];
    }
    else if (NULL != argv[6])
    {
        pintf     = "vlanif";
        pintf_num = argv[6];
    }
    else if (NULL != argv[7])
    {
        pintf     = "trunk";
        pintf_num = argv[7];
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
    if (NULL != argv[8])
    {
        ret = routev6_check_ipv6_str(argv[8]);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "The next-hop address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        ret = str2prefix_ipv6(argv[8], &nexthop);
        if (0 == ret)
        {
            vty_error_out(vty, "The next-hop address is invalid.%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        lroute.nexthop.type = INET_FAMILY_IPV6;
        memcpy(lroute.nexthop.addr.ipv6, nexthop.prefix.s6_addr, IPV6_ADDR_LEN);
    }

    /* argv[9]: vpn */
    if (NULL != argv[9])
    {
        lroute.vpn = (uint16_t)atoi(argv[9]);
    }

    /* 该命令行无法删除 IPv6 黑洞路由 */
    sroute = routev6_static_lookup(&lroute);
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

    /* IPv6 静态路由删除 */
    ret = routev6_static_delete(&lroute);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除 IPv6 黑洞路由命令
 * @param[in ] : X:X:X:X:X:X:X:X/M - 路由前缀和掩码
 * @param[in ] : vpn <1-128>       - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:45:13
 * @note       : 该命令行只能删除 IPv6 黑洞路由
 */
DEFUN (no_ipv6_route_blackhole,
    no_ipv6_route_blackhole_cmd,
    "no ipv6 route X:X:X:X:X:X:X:X/M blackhole {vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    struct route_static *sroute = NULL;
    struct route_static  lroute;
    struct prefix_ipv6   destip;
    char *pnt = NULL;
    int   mask_len = 0;
    int   ret;

    memset(&lroute, 0, sizeof(struct route_static));

    /* argv[0]: 路由前缀和掩码 */
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

    mask_len = atoi(++pnt);
    if ((mask_len<0) || (mask_len>IPV6_MAX_PREFIXLEN))
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = str2prefix_ipv6(argv[0], &destip);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    lroute.prefix.type      = INET_FAMILY_IPV6;
    lroute.prefix.prefixlen = destip.prefixlen;
    memcpy(lroute.prefix.addr.ipv6, destip.prefix.s6_addr, IPV6_ADDR_LEN);

    /* argv[1]: vpn */
    if (NULL != argv[1])
    {
        lroute.vpn = (uint16_t)atoi(argv[1]);
    }

    /* 该命令行无法删除 IPv6 静态路由 */
    sroute = routev6_static_lookup(&lroute);
    if (NULL == sroute)
    {
        vty_error_out(vty, "The specified static routing does not exist.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ((sroute->ifindex == 0) && (sroute->action == NHP_ACTION_DROP))
    {
        ret = routev6_static_delete(&lroute);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else
    {
        vty_error_out(vty, "The specified static routing does not exist.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示指定前缀的 IPv6 路由命令
 * @param[in ] : X:X:X:X:X:X:X:X/M - 路由前缀和掩码
 * @param[in ] : X:X:X:X:X:X:X:X   - 路由前缀
 * @param[in ] : vpn <1-128>       - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:50:01
 * @note       :
 */
DEFUN(show_ipv6_route_single,
    show_ipv6_route_single_cmd,
    "show ipv6 route (X:X:X:X:X:X:X:X|X:X:X:X:X:X:X:X/M) {vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    struct route_static lroute;
    struct route_entry *proute = NULL;
    struct prefix_ipv6  destip;
    char *pnt = NULL;
    int   ret;

    memset(&lroute, 0, sizeof(struct route_static));

    /* argv[0]: 路由前缀和掩码 */
    ret = routev6_check_ipv6_str(argv[0]);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "The specified IPV6 address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = str2prefix_ipv6(argv[0], &destip);
    if (0 == ret)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    memcpy(lroute.prefix.addr.ipv6, destip.prefix.s6_addr, IPV6_ADDR_LEN);
    lroute.prefix.prefixlen = destip.prefixlen;
    lroute.prefix.type      = INET_FAMILY_IPV6;

    /* argv[1]: vpn */
    if (NULL != argv[1])
    {
        lroute.vpn = (uint16_t)atoi(argv[1]);
    }

    /* 解析 argv[0] 格式，pnt 为 NULL 不带 '/' */
    pnt = strchr(argv[0], '/');
    if (NULL == pnt)
    {
        proute = routev6_match(&lroute.prefix, lroute.vpn);
        if (NULL == proute)
        {
            return CMD_SUCCESS;
        }
    }
    else
    {
        proute = routev6_lookup(&lroute.prefix, lroute.vpn);
        if (NULL == proute)
        {
            return CMD_SUCCESS;
        }
    }

    /* 输出路由信息 */
    vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
    vty_out(vty, "Routing Tables: Single%s", VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "%20s: %-10d", "Routes", 1);
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);

    routev6_static_show_active(vty, proute);

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 IPv6 活跃路由命令
 * @param[in ] : vpn <1-128> - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:52:37
 * @note       : 不指定 vpn 只显示公网路由
 */
DEFUN(show_ipv6_route_active,
    show_ipv6_route_active_cmd,
    "show ipv6 route active {vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    struct ptree_node  *node   = NULL;
    struct route_entry *proute = NULL;
    int num = 0;
    int vpn = 0;
    int ret;

    /* argv[0]: vpn */
    if (NULL != argv[0])
    {
        vpn = atoi(argv[0]);
    }

    PTREE_LOOP(routev6_tree[vpn], proute, node)
    {
        /* num 等于 1，输出一次信息头 */
        if (++num == 1)
        {
            vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
            
			vty_out(vty, "Routing Tables: Active%s", VTY_NEWLINE);
            
			vty_out(vty, "%s", VTY_NEWLINE);

            routev6_active_num_show(vty, vpn);
        }

        ret = routev6_static_show_active(vty, proute);
        if (CMD_CONTINUE == ret)
        {
            return ret;
        }
    }

    /* 批量回显命令行返回成功，需要将该变量置 0 */
    route_line = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 IPv6 路由命令
 * @param[in ] : vpn <1-128> - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:56:57
 * @note       : 不指定 vpn 只显示公网路由
 */
DEFUN(show_ipv6_route,
    show_ipv6_route_cmd,
    "show ipv6 route {vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    struct listnode   *plistnode = NULL;
    struct ptree_node *node      = NULL;
    struct rib_entry  *prib      = NULL;
    struct nhp_entry  *pnhp      = NULL;
    int num = 0;
    int vpn = 0;
    int ret;

    /* argv[0]: vpn */
    if (NULL != argv[0])
    {
        vpn = atoi(argv[0]);
    }

    PTREE_LOOP (ribv6_tree[vpn], prib, node)
    {
        /* num 等于 1，输出一次信息头 */
        if (++num == 1)
        {
            vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);

            vty_out(vty, "Routing Tables: All%s", VTY_NEWLINE);
            
            vty_out(vty, "%s", VTY_NEWLINE);

            routev6_total_num_show(vty, vpn);
        }

        for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
        {
            ret = routev6_static_show_all(vty, prib, pnhp);
            if (ret == CMD_CONTINUE)
            {
                return ret;
            }
        }
    }

    /* 批量回显命令行返回成功，需要将该变量置 0 */
    route_line = 0;

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 IPv6 路由统计信息命令
 * @param[in ] : vpn <1-128> - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS
 * @author     : ZhangFj
 * @date       : 2018年2月11日 14:59:53
 * @note       : 不指定 vpn 只显示公网路由统计信息
 */
DEFUN(show_ipv6_route_statistics,
    show_ipv6_route_statistics_cmd,
    "show ipv6 route statistics {vpn <1-128>}",
    ROUTE_CLI_INFO)
{
    int vpn = 0;

    /* argv[0]: vpn */
    if (NULL != argv[0])
    {
        vpn = atoi(argv[0]);
    }

    routev6_statistics_num_show(vty, vpn);

    return CMD_SUCCESS;
}


/**
 * @brief      : route 节点显示当前节点配置命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:03:17
 * @note       :
 */
DEFUN(show_this_route,
    show_this_route_cmd,
    "show this",
    ROUTE_CLI_INFO)
{
    vty_out(vty, "#%s", VTY_NEWLINE);

    route_static_config_write(vty);

    vty_out(vty, "#%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


const struct message route_debug_name[] =
{
    {.key = ROUTE_DEBUG_TYPE_ALL,		.str = "all"}
};

/**
 * @brief      : route 节点显示当前节点配置命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS
 * @author     : ZhouDW
 * @date       : 2019年3月19日 15:03:17
 * @note       :
 */
DEFUN(route_debug_monitor,
    route_debug_monitor_cmd,
    "debug route (enable|disable)(all)",
    "Debug information to moniter\n"
    "Programe name\n"
    "Enable statue\n"
    "Disatble statue\n"
    "Type name of all debug\n")
{
    unsigned int zlog_num;

    for(zlog_num = 0; zlog_num < array_size(route_debug_name); zlog_num++)
    {
        if(!strncmp(argv[1], route_debug_name[zlog_num].str, 3))
        {
            zlog_debug_set( vty, route_debug_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

            return CMD_SUCCESS;
        }
    }

    vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


/**
 * @brief      : IPv4 静态路由配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:04:09
 * @note       :
 */
static int routev4_static_config_write(struct vty *vty)
{
    struct route_static *proute     = NULL;
    struct hash_bucket  *pbucket    = NULL;
    char router_ip[INET_ADDRSTRLEN] = "";
    char destip[INET_ADDRSTRLEN]    = "";
    char nexthop[INET_ADDRSTRLEN]   = "";
    char ifname[IFNET_NAMESIZE]     = "";
    char route_cmd[STRING_LEN]      = "";
    int  len = 0;
    int  cursor;

    if (g_route.router_id != 0)
    {
        vty_out(vty, " router-id %s%s", inet_ipv4tostr(g_route.router_id, router_ip), VTY_NEWLINE);
    }

    if(ENABLE != g_route.route_frr_flag)
    {
        vty_out(vty, " no ip route frr enable%s", VTY_NEWLINE);
    }

    if(ENABLE != g_route.route_ecmp_flag)
    {
        vty_out(vty, " no ip route ecmp enable%s", VTY_NEWLINE);
    }
    HASH_BUCKET_LOOP(pbucket, cursor, static_route_table)
    {
        proute = pbucket->data;
        if (NULL == proute)
        {
            continue;
        }

        inet_ipv4tostr(proute->prefix.addr.ipv4, destip);
        len += sprintf(route_cmd+len, "ip route %s/%d", destip, proute->prefix.prefixlen);

        if (0 != proute->ifindex)
        {
            ifm_get_name_by_ifindex(proute->ifindex, ifname);
            len += sprintf(route_cmd+len, " %s", ifname);
        }

        if (NHP_ACTION_DROP == proute->action)
        {
            len += sprintf(route_cmd+len, " blackhole");
        }

        if (0 != proute->nexthop.addr.ipv4)
        {
            inet_ipv4tostr(proute->nexthop.addr.ipv4, nexthop);
            len += sprintf(route_cmd+len, " nexthop %s", nexthop);
        }

        if (proute->distance != ROUTE_METRIC_STATIC)
        {
            len += sprintf(route_cmd+len, " distance %d", proute->distance);
        }

        if (0 != proute->vpn)
        {
            len += sprintf(route_cmd+len, " vpn %d", proute->vpn);
        }

        route_cmd[len] = '\0';

        vty_out(vty, " %s%s", route_cmd, VTY_NEWLINE);

        len = 0;
        memset(route_cmd, 0, sizeof(route_cmd));
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv6 静态路由配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:05:51
 * @note       :
 */
static int routev6_static_config_write(struct vty *vty)
{
    struct route_static *proute      = NULL;
    struct hash_bucket  *pbucket     = NULL;
    char destip[INET6_ADDRSTRLEN]    = "";
    char nexthop[INET6_ADDRSTRLEN]   = "";
    char router_id[INET6_ADDRSTRLEN] = "";
    char ifname[IFNET_NAMESIZE]      = "";
    char route_cmd[STRING_LEN]       = "";
    int  len = 0;
    int  cursor;

    if (ROUTE_IPV6_ADDR_EXIST(g_route.routerv6_id.ipv6))
    {
        inet_ntop(AF_INET6, g_route.routerv6_id.ipv6, router_id, INET6_ADDRSTRLEN);

        vty_out(vty, " router-id %s%s", router_id, VTY_NEWLINE);
    }

    HASH_BUCKET_LOOP(pbucket, cursor, static_routev6_table)
    {
        proute = pbucket->data;
        if (NULL == proute)
        {
            continue;
        }

        inet_ntop(AF_INET6, proute->prefix.addr.ipv6, destip, INET6_ADDRSTRLEN);
        len += sprintf(route_cmd+len, "ipv6 route %s/%d", destip, proute->prefix.prefixlen);

        if (0 != proute->ifindex)
        {
            ifm_get_name_by_ifindex(proute->ifindex, ifname);
            len += sprintf(route_cmd+len, " %s", ifname);
        }

        if (NHP_ACTION_DROP == proute->action)
        {
            len += sprintf(route_cmd+len, " blackhole");
        }

        if (ROUTE_IF_IPV6_EXIST(proute->nexthop.addr.ipv6))
        {
            inet_ntop(AF_INET6, proute->nexthop.addr.ipv6, nexthop, INET6_ADDRSTRLEN);
            len += sprintf(route_cmd+len, " nexthop %s", nexthop);
        }

        if (proute->distance != ROUTE_METRIC_STATIC)
        {
            len += sprintf(route_cmd+len, " distance %d", proute->distance);
        }

        if (0 != proute->vpn)
        {
            len += sprintf(route_cmd+len, " vpn %d", proute->vpn);
        }

        route_cmd[len] = '\0';

        vty_out(vty, " %s%s", route_cmd, VTY_NEWLINE);

        len = 0;
        memset(route_cmd, 0, sizeof(route_cmd));
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : route 节点配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:06:39
 * @note       :
 */
static int route_static_config_write(struct vty *vty)
{
    vty_out(vty, "route%s", VTY_NEWLINE);

    routev4_static_config_write(vty);
    routev6_static_config_write(vty);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 静态路由命令行初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 15:07:17
 * @note       :
 */
void route_static_cmd_init(void)
{
    install_node(&route_node1, route_static_config_write);

    install_default(ROUTE_NODE);

    install_element (CONFIG_NODE, &route_node_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &route_debug_monitor_cmd, CMD_SYNC);

    install_element (ROUTE_NODE, &show_this_route_cmd, CMD_LOCAL);
    install_element (ROUTE_NODE, &route_ecmp_enable_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &no_route_ecmp_enable_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &route_frr_enable_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &no_route_frr_enable_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &route_router_id_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &ip_route_static_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &ip_route_balckhole_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &no_ip_route_static_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &no_ip_route_blackhole_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &routev6_ecmp_enable_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &no_routev6_ecmp_enable_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &routev6_frr_enable_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &no_routev6_frr_enable_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &route_routerv6_id_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &ipv6_route_static_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &ipv6_route_balckhole_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &no_ipv6_route_static_cmd, CMD_SYNC);
    install_element (ROUTE_NODE, &no_ipv6_route_blackhole_cmd, CMD_SYNC);

#define ROUTE_INSTALL_ELEMENT_SHOW(cmd, flag)\
    install_element (PHYSICAL_IF_NODE, (cmd), (flag));\
    install_element (PHYSICAL_SUBIF_NODE, (cmd), (flag));\
    install_element (LOOPBACK_IF_NODE, (cmd), (flag));\
    install_element (TUNNEL_IF_NODE, (cmd), (flag));\
    install_element (TRUNK_IF_NODE, (cmd), (flag));\
    install_element (TRUNK_SUBIF_NODE, (cmd), (flag));\
    install_element (VLANIF_NODE, (cmd), (flag));\
    install_element (ROUTE_NODE, (cmd), (flag));\
    install_element (CONFIG_NODE, (cmd), (flag));

    ROUTE_INSTALL_ELEMENT_SHOW(&show_ip_route_single_cmd, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_SHOW(&show_ip_route_active_cmd, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_SHOW(&show_ip_route_cmd, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_SHOW(&show_ip_route_statistics_cmd, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_SHOW(&show_ipv6_route_single_cmd, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_SHOW(&show_ipv6_route_active_cmd, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_SHOW(&show_ipv6_route_cmd, CMD_LOCAL)
    ROUTE_INSTALL_ELEMENT_SHOW(&show_ipv6_route_statistics_cmd, CMD_LOCAL)

    return;
}


