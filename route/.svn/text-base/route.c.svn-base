/**
 * @file      : route.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月26日 10:45:31
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/module_id.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/prefix.h>
#include <lib/route_com.h>
#include <lib/errcode.h>
#include "route_main.h"
#include "route_static.h"
#include "route_if.h"
#include "route_nhp.h"
#include "route_register.h"
#include "route_msg.h"


/* route 树 */
struct ptree *route_tree[L3VPN_SIZE+1];

/* routev6 树 */
struct ptree *routev6_tree[L3VPN_SIZE+1];


/**
 * @brief      : 初始化 IPv4 活跃路由树
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:56:18
 * @note       :
 */
int route_tree_init(void)
{
    int vpn = 0;

    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        route_tree[vpn] = ptree_init(IPV4_MAX_PREFIXLEN);
        if (NULL == route_tree[vpn])
        {
            return ERRNO_MALLOC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 删除指定 vpn 实例的路由树
 * @param[in ] : vpn - 指定的 vpn 实例
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:57:56
 * @note       : 仅释放树节点和树本身内存，用户申请内存未释放
 */
void route_tree_delete(uint16_t vpn)
{
    if (vpn > L3VPN_SIZE)
    {
        return;
    }

    ptree_free(route_tree[vpn]);

    return;
}


/**
 * @brief      : 删除所有 vpn 实例的路由树
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:58:22
 * @note       : 仅释放树节点和树本身内存，用户申请内存未释放
 */
void route_tree_clear(void)
{
    int vpn = 0;

    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        ptree_free(route_tree[vpn]);
    }

    return;
}


/**
 * @brief      : 路由携带出接口，处理路由状态
 * @param[in ] : pnhp - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:59:03
 * @note       :
 */
static int route_check_outif(struct nhp_entry *pnhp)
{
    struct route_if *pif = NULL;

    ROUTE_LOG_DEBUG();

    pif = route_if_lookup(pnhp->ifindex);
    if (NULL == pif)
    {
        return ERRNO_NOT_FOUND;
    }

    if ((pnhp->vpn != pif->intf.vpn) || (pif->down_flag == LINK_DOWN))
    {
        return ERRNO_FAIL;
    }

    /* 不同接口类型处理方式不同 */
    if (IFM_TYPE_IS_LOOPBCK(pnhp->ifindex))
    {
        /* loopback 接口配置 IP ，nhp 状态为 LINK_UP */
        if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
        {
            pnhp->down_flag = LINK_UP;

            return ERRNO_SUCCESS;
        }
    }
    else if (IFM_TYPE_IS_VLANIF(pnhp->ifindex))
    {
        /* vlanif 接口配置 IP，nhp 状态为 LINK_UP */
        if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
        {
            pnhp->nexthop_connect = pnhp->nexthop;
            pnhp->down_flag       = LINK_UP;

            return ERRNO_SUCCESS;
        }

        /* 接口 IP 地址为借用，被借用接口为 LINK_UP 并且配置 IP，nhp 状态为 LINK_UP */
        if (0 != pif->intf.unnumbered_if)
        {
            pif = route_if_lookup(pif->intf.unnumbered_if);
            if (NULL != pif)
            {
                if ((pif->down_flag == LINK_UP) && ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
                {
                    pnhp->nexthop_connect = pnhp->nexthop;
                    pnhp->down_flag       = LINK_UP;

                    return ERRNO_SUCCESS;
                }
            }
        }
    }
    else if (IFM_TYPE_IS_METHERNET(pnhp->ifindex)
            || IFM_TYPE_IS_TRUNK(pnhp->ifindex))
    {
        /* 以太接口、trunk 接口配置 IP，nhp 状态为 LINK_UP */
        if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
        {
            pnhp->nexthop_connect = pnhp->nexthop;
            pnhp->down_flag       = LINK_UP;

            return ERRNO_SUCCESS;
        }

        /* 接口 IP 地址为借用，被借用接口为 LINK_UP 并且配置 IP，nhp 状态为 LINK_UP */
        if (0 != pif->intf.unnumbered_if)
        {
            pif = route_if_lookup(pif->intf.unnumbered_if);
            if (NULL != pif)
            {
                if ((pif->down_flag == LINK_UP) && ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))
                {
                    pnhp->nexthop_connect = pnhp->nexthop;
                    pnhp->down_flag       = LINK_UP;

                    return ERRNO_SUCCESS;
                }
            }
        }
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : 使用原始路由前缀信息填充活跃路由信息
 * @param[in ] : proute - 活跃路由结构
 * @param[out] : prib   - 原始路由结构
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 11:21:45
 * @note       :
 */
static void route_set_prefix(struct route_entry *proute, struct rib_entry *prib)
{
    if ((NULL == proute) || (NULL == prib))
    {
        return;
    }

    proute->prefix = prib->prefix;
    proute->vpn    = prib->vpn;

    return;
}


/**
 * @brief      : IPv4 路由下一跳迭代
 * @param[in ] : proute - 活跃路由结构
 * @param[out] : proute - 活跃路由结构
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 11:23:17
 * @note       : 携带出接口的路由特殊处理，不带出接口的路由需要迭代到直连下一跳
 * @note       : 最多支持三次迭代
 */
static int route_iteration(struct route_entry *proute)
{
    struct nhp_entry   *pnhp         = NULL;
    struct nhp_entry   *pnhp_match   = NULL;
    struct route_entry *proute_match = NULL;
    struct inet_prefix  prefix;
    uint8_t  pdown_flag = 0;
    int      i;
    int      ret;
    int      count = 0;

    for (i=0; i<proute->nhp_num; i++)
    {
        pnhp = &proute->nhp[i];
        if (pnhp->nhp_type == NHP_TYPE_CONNECT)
        {
            /* 直连路由状态为 up */
            pnhp->down_flag       = LINK_UP;
            pnhp->nexthop_connect = pnhp->nexthop;
        }
        else if (pnhp->nhp_type == NHP_TYPE_TUNNEL)
        {
            /* å‡ºæŽ¥å£ä¸º tunnel çš„è·¯ç”±ï¼Œtunnel æŽ¥å£ upï¼Œè·¯ç”±çŠ¶æ€ä¸º up */
            ret = ifm_get_link(pnhp->ifindex, MODULE_ID_ROUTE, &pdown_flag);
            
            if ((0 == ret) && (pdown_flag == IFNET_LINKUP))
            {
                pnhp->down_flag       = LINK_UP;
                pnhp->nexthop_connect = pnhp->nexthop;
            }
        }
        else if (pnhp->nhp_type == NHP_TYPE_LSP)
        {
            /* lsp 路由状态为 up */
            pnhp->down_flag       = LINK_UP;
            pnhp->nexthop_connect = pnhp->nexthop;
        }
        else if (pnhp->nhp_type == NHP_TYPE_IP)
        {
            /* ip 路由根据不同情况再处理 */
            if (ROUTE_IS_BLACKHOLE(pnhp))
            {
                /* 黑洞路由状态为 up */
                pnhp->down_flag = LINK_UP;
            }
            else if ((0 != pnhp->ifindex) && (!ROUTE_PROTO_IS_LSP(pnhp)))
            {
                /* 指定出接口的路由需要根据接口类型做不同处理 */
                ret = route_check_outif(pnhp);
                if (ret != ERRNO_SUCCESS)
                {
                    pnhp->down_flag = LINK_DOWN;
                }
            }
            else
            {
                /* 普通 ip 路由需要在 3 次之内迭代到直连状态才会为 up */
                pnhp_match = pnhp;

next:

                /* 路由迭代次数大于 3 次，迭代失败 */
                if (++count > ROUTE_CONVER_NUM)
                {
                    pnhp->down_flag = LINK_DOWN;

                    return ERRNO_FAIL;
                }

                prefix.addr.ipv4 = pnhp_match->nexthop.addr.ipv4;
                prefix.prefixlen = IPV4_MAX_PREFIXLEN;
                prefix.type      = INET_FAMILY_IPV4;

                proute_match = route_match(&prefix, pnhp_match->vpn);
                if (proute_match == NULL)
                {
                    pnhp->down_flag = LINK_DOWN;
                }
                else if (proute_match->nhp[0].nhp_type == NHP_TYPE_CONNECT)
                {
                    /* 下一跳迭代到本机主机路由，路由不 UP */
                    if (proute_match->prefix.prefixlen == IPV4_MAX_PREFIXLEN)
                    {
                        pnhp->down_flag = LINK_DOWN;

                        return ERRNO_SUCCESS;
                    }

                    /* 迭代到直连路由，状态为 up */
                    pnhp->down_flag       = LINK_UP;
                    pnhp->nexthop_connect = pnhp_match->nexthop;
                    pnhp->ifindex         = proute_match->nhp[0].ifindex;

                    if (count > 1)
                    {
                        pnhp->cost += pnhp_match->cost;
                    }
                }
                else if (proute_match->nhp[0].nhp_type == NHP_TYPE_LSP)
                {
                    /* 迭代到 lsp 路由，状态为 up */
                    pnhp->ifindex   = proute_match->nhp[0].ifindex;
                    pnhp->down_flag = LINK_UP;
                    pnhp->nhp_type  = NHP_TYPE_LSP;
                }
                else if (proute_match->nhp[0].nhp_type == NHP_TYPE_IP)
                {
                    /* 静态路由迭代到自身，状态为 up */
                    if (0 == memcmp(pnhp_match, &proute_match->nhp[0], sizeof(struct nhp_entry)))
                    {
                        pnhp->ifindex   = proute_match->nhp[0].ifindex;
                        pnhp->down_flag = LINK_UP;

                        return ERRNO_SUCCESS;
                    }

                    /* 迭代到默认路由不 up */
                    if ((0 == proute_match->prefix.addr.ipv4) && (0 == proute_match->prefix.prefixlen))
                    {
                        pnhp->down_flag = LINK_DOWN;

                        return ERRNO_SUCCESS;
                    }

                    /* 迭代到非直连路由继续迭代 */
                    pnhp_match = &proute_match->nhp[0];

                    goto next;
                }
                else
                {
                    /* 迭代失败 */
                    pnhp->down_flag = LINK_DOWN;
                }
            }
        }
        else
        {
            /* 未知下一跳类型 */
            pnhp->down_flag = LINK_DOWN;

            return ERRNO_FAIL;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv4 活跃路由添加
 * @param[in ] : prib - 原始路由结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 11:38:24
 * @note       :
 */
int route_add(struct rib_entry *prib)
{
    struct ptree_node  *pnode  = NULL;
    struct route_entry *proute = NULL;
    int i = 0;
    int ret;

    ROUTE_LOG_DEBUG();

    if (prib == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ROUTE_NUMBER_ACTIVE == ROUTE_NUM_MAX)
    {
        return ERRNO_OVERSIZE;
    }

    proute = (struct route_entry *)XCALLOC(MTYPE_ROUTE_TABLE, sizeof(struct route_entry));
    if (NULL == proute)
    {
        ROUTE_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    /* 下一跳优选 */
    route_set_prefix(proute, prib);
    ret = route_select(proute, prib);
    if (ERRNO_SUCCESS != ret)
    {
        XFREE(MTYPE_ROUTE_TABLE, proute);

        return ERRNO_SUCCESS;
    }

    /* 下一跳分离 */
    ret = route_nhp_add_process(proute);
    if ((ERRNO_SUCCESS != ret) && (ERRNO_NOT_UPDATE != ret))
    {
        ROUTE_LOG_ERROR("errcode = %d\n", ret);

        for (i=0; i<proute->nhp_num; i++)
        {
            rib_count_number_active(&proute->nhp[i], OPCODE_DELETE);
        }

        return ret;
    }

    /* ä¸‹å‘é…ç½® */
    route_add_msg_to_send_list(proute, sizeof(struct route_entry), 1, MODULE_ID_FTM,
                        MODULE_ID_ROUTE, IPC_TYPE_FIB, ROUTE_SUBTYPE_IPV4,
                        IPC_OPCODE_ADD, 0);

    /* 活跃路由数据存储 */
    pnode = ptree_node_get(route_tree[proute->vpn], (u_char *)&proute->prefix.addr.ipv4,
                        proute->prefix.prefixlen);
    if (pnode == NULL)
    {
        ROUTE_LOG_ERROR("ptree get\n");

        for (i=0; i<proute->nhp_num; i++)
        {
            rib_count_number_active(&proute->nhp[i], OPCODE_DELETE);
        }

        route_nhp_delete_process(proute);
        XFREE(MTYPE_ROUTE_TABLE, proute);

        return ERRNO_MALLOC;
    }

    pnode->info = (void *)proute;

    /* 活跃路由变化通知 */
    route_event_notify(proute->nhp[0].protocol, proute, IPC_OPCODE_ADD);
    route_event_frr_notify(proute->nhp[0].protocol, proute, IPC_OPCODE_ADD);

    /* 直连路由变化更新静态路由状态 */
    if (g_route.conver_flag == ENABLE)
    {
        if (NULL == g_route.route_timer)
        {
            g_route.route_timer = ROUTE_TIMER_ADD(route_static_update_routing, NULL, 1);
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv4 活跃路由更新
 * @param[in ] : prib - 原始路由结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 11:48:22
 * @note       :
 */
int route_update(struct rib_entry *prib)
{
    struct ptree_node  *pnode  = NULL;
    struct route_entry *proute = NULL;
    struct route_entry  route_del;
    struct route_entry  route_notify;
    int update_flag = ENABLE;
    int ret         = ERRNO_SUCCESS;
    int flag        = DISABLE;
    int i           = 0;
    int j           = 0;

    ROUTE_LOG_DEBUG();

    if (NULL == prib)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pnode = ptree_node_lookup(route_tree[prib->vpn], (u_char *)&prib->prefix.addr.ipv4,
                            prib->prefix.prefixlen);
    if ((pnode == NULL) || (pnode->info == NULL))
    {
        return ERRNO_FAIL;
    }

    proute = (struct route_entry *)pnode->info;
    memcpy(&route_del, proute, sizeof(struct route_entry));

    /* 下一跳优选 */
    ret = route_select(proute, prib);
    if (ERRNO_SUCCESS != ret)
    {
        route_delete(&prib->prefix, prib->vpn);

        return ERRNO_SUCCESS;
    }

    /* 下一跳分离 */
    ret = route_nhp_add_process(proute);
    if (ERRNO_NOT_UPDATE == ret)
    {
        /* 下一跳协议类型或 cost 发生变化，触发通知事件 */
        for (i=0; i<route_del.nhp_num; i++)
        {
            if ((route_del.nhp[i].protocol != proute->nhp[i].protocol)
                || (route_del.nhp[i].cost != proute->nhp[i].cost)
                || (route_del.nhp[i].distance != proute->nhp[i].distance))
            {
                update_flag = DISABLE;

                goto notify;
            }
        }

        /* frr 主备反转，触发通知事件 */
        if (proute->nhpinfo.nhp_type == NHP_TYPE_FRR)
        {
            if ((proute->nhp[0].nexthop.addr.ipv4 != route_del.nhp[0].nexthop.addr.ipv4)
                || (proute->nhp[1].nexthop.addr.ipv4 != route_del.nhp[1].nexthop.addr.ipv4))
            {
                update_flag = DISABLE;

                goto notify;
            }
        }

        return ERRNO_SUCCESS;
    }
    else if (ERRNO_SUCCESS != ret)
    {
        ROUTE_LOG_ERROR("errcode = %d\n", ret);

        for (i=0; i<proute->nhp_num; i++)
        {
            rib_count_number_active(&proute->nhp[i], OPCODE_DELETE);
        }

        return ret;
    }

    /* ä¸‹å‘é…ç½®æ›´æ–° */
    route_add_msg_to_send_list(proute, sizeof(struct route_entry), 1, MODULE_ID_FTM,
                        MODULE_ID_ROUTE, IPC_TYPE_FIB, ROUTE_SUBTYPE_IPV4,
                        IPC_OPCODE_UPDATE, 0);

    /* 删除被更新的下一跳 */
    route_nhp_delete_process(&route_del);

notify:

    /* 活跃路由变化通知，先通知删除事件，再通知添加事件 */
    memcpy(&route_notify, &route_del, sizeof(struct route_entry));

    for (i=0; i<route_del.nhp_num; i++)
    {
        route_notify.nhp[0]  = route_del.nhp[i];
        route_notify.nhp_num = 1;

        for (j=0; j<proute->nhp_num; j++)
        {
            /* 未变化的路由不通知 */
            if ((route_notify.nhp[0].nexthop.addr.ipv4 == proute->nhp[j].nexthop.addr.ipv4)
                && (route_notify.nhp[0].vpn == proute->nhp[j].vpn)
                && (route_notify.nhp[0].protocol == proute->nhp[j].protocol)
                && (route_notify.nhp[0].distance == proute->nhp[j].distance)
                && (route_notify.nhp[0].cost == proute->nhp[j].cost))
            {
                flag = ENABLE;

                break;
            }

            if (proute->nhpinfo.nhp_type == NHP_TYPE_FRR)
            {
                break;
            }
        }

        if (ENABLE == flag)
        {
            flag = DISABLE;
            route_event_frr_notify(route_del.nhp[i].protocol, &route_notify, IPC_OPCODE_DELETE);
            continue;
        }

        if ((i>0) && (route_del.nhpinfo.nhp_type == NHP_TYPE_FRR))
        {
            route_event_frr_notify(route_del.nhp[i].protocol, &route_notify, IPC_OPCODE_DELETE);

            break;
        }

        route_event_notify(route_del.nhp[i].protocol, &route_notify, IPC_OPCODE_DELETE);
        route_event_frr_notify(route_del.nhp[i].protocol, &route_notify, IPC_OPCODE_DELETE);
    }

    memcpy(&route_notify, proute, sizeof(struct route_entry));

    for (i=0; i<proute->nhp_num; i++)
    {
        route_notify.nhp[0]  = proute->nhp[i];
        route_notify.nhp_num = 1;

        for (j=0; j<route_del.nhp_num; j++)
        {
            /* 未变化的路由不通知 */
            if ((route_notify.nhp[0].nexthop.addr.ipv4 == route_del.nhp[j].nexthop.addr.ipv4)
                && (route_notify.nhp[0].vpn == route_del.nhp[j].vpn)
                && (route_notify.nhp[0].protocol == route_del.nhp[j].protocol)
                && (route_notify.nhp[0].distance == route_del.nhp[j].distance)
                && (route_notify.nhp[0].cost == route_del.nhp[j].cost))
            {
                flag = ENABLE;

                break;
            }

            if (route_del.nhpinfo.nhp_type == NHP_TYPE_FRR)
            {
                break;
            }
        }

        if (ENABLE == flag)
        {
            flag = DISABLE;
            route_event_frr_notify(proute->nhp[i].protocol, &route_notify, IPC_OPCODE_ADD);
            continue;
        }

        if ((i>0) && (proute->nhpinfo.nhp_type == NHP_TYPE_FRR))
        {
            route_event_frr_notify(proute->nhp[i].protocol, &route_notify, IPC_OPCODE_ADD);

            break;
        }

        route_event_notify(proute->nhp[i].protocol, &route_notify, IPC_OPCODE_ADD);
        route_event_frr_notify(proute->nhp[i].protocol, &route_notify, IPC_OPCODE_ADD);
    }

    if ((g_route.conver_flag == ENABLE) && (update_flag == ENABLE))
    {
        if (NULL == g_route.route_timer)
        {
            g_route.route_timer = ROUTE_TIMER_ADD(route_static_update_routing, NULL, 1);
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv4 活跃路由删除
 * @param[in ] : prefix - 路由前缀
 * @param[in ] : vpn    - vpn 实例
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 11:52:35
 * @note       :
 */
int route_delete(struct inet_prefix *prefix, uint16_t vpn)
{
    struct ptree_node  *pnode  = NULL;
    struct route_entry *proute = NULL;
    struct route_entry  route_notify;
    int i = 0;

    ROUTE_LOG_DEBUG();

    if (vpn > L3VPN_SIZE)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pnode = ptree_node_lookup(route_tree[vpn], (u_char *)&prefix->addr.ipv4,
                        prefix->prefixlen);
    if ((pnode == NULL) || (pnode->info == NULL))
    {
        return ERRNO_SUCCESS;
    }

    proute = (struct route_entry *)pnode->info;

    /* ä¸‹å‘åˆ é™¤äº‹ä»¶ */
    route_add_msg_to_send_list(proute, sizeof(struct route_entry), 1, MODULE_ID_FTM,
                        MODULE_ID_ROUTE, IPC_TYPE_FIB, ROUTE_SUBTYPE_IPV4,
                        IPC_OPCODE_DELETE, 0);

    /* 活跃路由变化通知 */
    memcpy(&route_notify, proute, sizeof(struct route_entry));

    for (i=0; i<proute->nhp_num; i++)
    {
        route_notify.nhp[0]  = proute->nhp[i];
        route_notify.nhp_num = 1;

        if ((i>0) && (proute->nhpinfo.nhp_type == NHP_TYPE_FRR))
        {
            route_event_frr_notify(proute->nhp[i].protocol, &route_notify, IPC_OPCODE_DELETE);

            break;
        }

        route_event_notify(proute->nhp[i].protocol, &route_notify, IPC_OPCODE_DELETE);
        route_event_frr_notify(proute->nhp[i].protocol, &route_notify, IPC_OPCODE_DELETE);
    }

    /* 删除 fib 引用的 nhp */
    route_nhp_delete_process(proute);

    /* 释放路由节点 */
    pnode->info = NULL;
    pnode->lock = 0;

    ptree_node_delete(pnode);

    if (g_route.conver_flag == ENABLE)
    {
        if (NULL == g_route.route_timer)
        {
            g_route.route_timer = ROUTE_TIMER_ADD(route_static_update_routing, NULL, 1);
        }
    }

    XFREE(MTYPE_ROUTE_TABLE, proute);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv4 活跃路由查找
 * @param[in ] : prefix - 路由前缀
 * @param[in ] : vpn    - vpn 实例
 * @param[out] :
 * @return     : 成功返回活跃路由结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 14:21:50
 * @note       :
 */
struct route_entry *route_lookup(struct inet_prefix *prefix, uint16_t vpn)
{
    struct ptree_node *pnode = NULL;

    ROUTE_LOG_DEBUG();

    if ((NULL == prefix) || (vpn > L3VPN_SIZE))
    {
        return NULL;
    }

    pnode = ptree_node_lookup(route_tree[vpn], (u_char *)&prefix->addr.ipv4,
                                prefix->prefixlen);
    if (pnode == NULL)
    {
        return NULL;
    }

    ptree_unlock_node(pnode);

    return (struct route_entry *)pnode->info;
}


/**
 * @brief      : IPv4 活跃路由最长掩码匹配
 * @param[in ] : prefix - 路由前缀
 * @param[in ] : vpn    - vpn 实例
 * @param[out] :
 * @return     : 成功返回活跃路由结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 14:26:51
 * @note       :
 */
struct route_entry *route_match(struct inet_prefix *prefix, uint16_t vpn)
{
    struct ptree_node *pnode = NULL;

    ROUTE_LOG_DEBUG();

    if (vpn > L3VPN_SIZE)
    {
        return NULL;
    }

    pnode = ptree_node_match1(route_tree[vpn], (u_char *)&prefix->addr.ipv4,
                                prefix->prefixlen);
    if (pnode == NULL)
    {
        return NULL;
    }

    ptree_unlock_node(pnode);

    return (struct route_entry *)pnode->info;
}


/**
 * @brief      : IPv4 路由生成 frr
 * @param[in ] : proute - 活跃路由结构
 * @param[in]  : prib   - 原始路由结构
 * @param[out] : proute - 活跃路由结构
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 14:29:18
 * @note       : 优先级不同出接口不同的两个活跃的 nhp 形成 frr
 */
static int route_select_frr(struct route_entry *proute, struct rib_entry *prib)
{
    struct listnode  *plistnode   = NULL;
    struct nhp_entry *pnhp        = NULL;
    struct nhp_entry *pnhp_select = NULL;
    struct nhp_entry *pnhp_active = NULL;
    int distance = ROUTE_METRIC_UNKNOW+1;

    ROUTE_LOG_DEBUG();

    if (g_route.route_frr_flag == DISABLE)
    {
        return ERRNO_SUCCESS;
    }

    if ((NULL == prib) || (NULL == proute))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ROUTE_NUMBER_ACTIVE == ROUTE_NUM_MAX)
    {
        return ERRNO_OVERSIZE;
    }

    pnhp_active = &proute->nhp[0];

    /* 黑洞路由不存在 frr */
    if (ROUTE_IS_BLACKHOLE(pnhp_active))
    {
        return ERRNO_FAIL;
    }

    /* 直连路由不会形成 frr */
    if (pnhp_active->protocol == ROUTE_PROTO_CONNECT)
    {
        return ERRNO_FAIL;
    }

    for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
    {
        /* 活跃的下一跳已被选中，忽略 */
        if (pnhp->active == ROUTE_STATUS_ACTIVE)
        {
            continue;
        }

        /* 黑洞路由不存在 frr */
        if (ROUTE_IS_BLACKHOLE(pnhp))
        {
            continue;
        }

        /* lsp 路由不能与普通路由形成 frr */
        if ((ROUTE_PROTO_IS_LSP(pnhp_active) && (!ROUTE_PROTO_IS_LSP(pnhp)))
            || (ROUTE_PROTO_IS_LSP(pnhp) && (!ROUTE_PROTO_IS_LSP(pnhp_active))))
        {
            continue;
        }

        /* 下一跳为 lsp 的路由不能与普通路由形成 frr */
        if (((pnhp_active->nhp_type == NHP_TYPE_LSP) && (pnhp->nhp_type != NHP_TYPE_LSP))
            || ((pnhp->nhp_type == NHP_TYPE_LSP) && (pnhp_active->nhp_type != NHP_TYPE_LSP)))
        {
            continue;
        }

        /* 选出优先级次优的 nhp */
        if ((pnhp->distance < distance)
            && (pnhp->distance != pnhp_active->distance)
            && (pnhp->down_flag == LINK_UP)
            && (pnhp->ifindex != pnhp_active->ifindex))
        {
            distance    = pnhp->distance;
            pnhp_select = pnhp;
        }
    }

    if (pnhp_select != NULL)
    {
        ROUTE_LOG_DEBUG("select\n");

        pnhp_select->active = ROUTE_STATUS_ACTIVE;

        rib_count_number_active(pnhp_select, OPCODE_ADD);

        memcpy(&proute->nhp[proute->nhp_num], pnhp_select, sizeof(struct nhp_entry));
        proute->nhpinfo.nhp_type = NHP_TYPE_FRR;
        proute->nhp_num++;

        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : IPv4 路由生成 ecmp
 * @param[in ] : proute - 活跃路由结构
 * @param[in]  : prib   - 原始路由结构
 * @param[out] : proute - 活跃路由结构
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 15:27:10
 * @note       : distance 和 cost 相同，下一跳不同的活跃 nhp 形成 ecmp
 */
static int route_select_ecmp(struct route_entry *proute, struct rib_entry *prib)
{
    struct listnode  *plistnode   = NULL;
    struct nhp_entry *pnhp        = NULL;
    struct nhp_entry *pnhp_select = NULL;
    struct nhp_entry *pnhp_active = NULL;

    ROUTE_LOG_DEBUG();

    if (g_route.route_ecmp_flag == DISABLE)
    {
        return ERRNO_SUCCESS;
    }

    if ((NULL == prib) || (NULL == proute))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ROUTE_NUMBER_ACTIVE == ROUTE_NUM_MAX)
    {
        return ERRNO_OVERSIZE;
    }

    pnhp_active = &proute->nhp[0];

    /* 黑洞路由不存在 ecmp */
    if (ROUTE_IS_BLACKHOLE(pnhp_active))
    {
        return ERRNO_FAIL;
    }

    /* 直连路由不会形成 ecmp */
    if (pnhp_active->protocol == ROUTE_PROTO_CONNECT)
    {
        return ERRNO_FAIL;
    }

    for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
    {
        /* 活跃的 nhp 已被选中，忽略 */
        if (pnhp->active == ROUTE_STATUS_ACTIVE)
        {
            continue;
        }

        /* 黑洞路由不存在 ecmp */
        if (ROUTE_IS_BLACKHOLE(pnhp))
        {
            continue;
        }

        /* lsp 路由不能与普通路由形成 ecmp */
        if ((ROUTE_PROTO_IS_LSP(pnhp_active) && (!ROUTE_PROTO_IS_LSP(pnhp)))
            || (ROUTE_PROTO_IS_LSP(pnhp) && (!ROUTE_PROTO_IS_LSP(pnhp_active))))
        {
            continue;
        }

        /* 下一跳为 lsp 的路由不能与普通路由形成 ecmp */
        if (((pnhp_active->nhp_type == NHP_TYPE_LSP) && (pnhp->nhp_type != NHP_TYPE_LSP))
            || ((pnhp->nhp_type == NHP_TYPE_LSP) && (pnhp_active->nhp_type != NHP_TYPE_LSP)))
        {
            continue;
        }

        /* 选出与第一条 distance、cost 相同，下一跳不同的 nhp */
        if ((pnhp->distance == pnhp_active->distance)
            && (pnhp->cost == pnhp_active->cost)
            && (pnhp->down_flag == LINK_UP)
            && (pnhp->nexthop.addr.ipv4 != pnhp_active->nexthop.addr.ipv4))
        {
            pnhp_select = pnhp;

            break;
        }
    }

    if (pnhp_select != NULL)
    {
        ROUTE_LOG_DEBUG("select\n");

        pnhp_select->active = ROUTE_STATUS_ACTIVE;

        rib_count_number_active(pnhp_select, OPCODE_ADD);

        memcpy(&proute->nhp[proute->nhp_num], pnhp_select, sizeof(struct nhp_entry));
        proute->nhpinfo.nhp_type = NHP_TYPE_ECMP;
        proute->nhp_num++;

        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : IPv4 活跃路由选择 nhp
 * @param[in ] : proute - 活跃路由结构
 * @param[in]  : prib   - 原始路由结构
 * @param[out] : proute - 活跃路由结构
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 15:35:03
 * @note       :
 */
static int route_select_nhp(struct route_entry *proute, struct rib_entry *prib)
{
    struct listnode  *plistnode   = NULL;
    struct nhp_entry *pnhp        = NULL;
    struct nhp_entry *pnhp_select = NULL;
    int               distance    = ROUTE_METRIC_UNKNOW+1;

    if (NULL == prib)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    ROUTE_LOG_DEBUG("prefix = %x, %d\n", prib->prefix.addr.ipv4, prib->prefix.prefixlen);

    if (ROUTE_NUMBER_ACTIVE == ROUTE_NUM_MAX)
    {
        return ERRNO_OVERSIZE;
    }

    /* 选出优先级最高并且状态为 up 的 nhp */
    for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
    {
        if (((pnhp->distance < distance) && (pnhp->down_flag == LINK_UP))
            || (pnhp->distance == distance && ROUTE_IS_BLACKHOLE(pnhp)))
        {
            distance    = pnhp->distance;
            pnhp_select = pnhp;
        }
    }

    if (NULL != pnhp_select)
    {
        ROUTE_LOG_DEBUG("select\n");

        pnhp_select->active = ROUTE_STATUS_ACTIVE;

        rib_count_number_active(pnhp_select, OPCODE_ADD);

        memcpy(&proute->nhp[0], pnhp_select, sizeof(struct nhp_entry));
        proute->nhp_num++;

        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : IPv4 路由下一跳优选
 * @param[in ] : proute - 活跃路由结构
 * @param[in]  : prib   - 原始路由结构
 * @param[out] : proute - 活跃路由结构
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 15:37:11
 * @note       :
 */
int route_select(struct route_entry *proute, struct rib_entry *prib)
{
    struct listnode   *pnode = NULL;
    struct nhp_entry  *pnhp  = NULL;
    struct route_if   *pif   = NULL;
    struct route_entry route;
    struct route_entry route_active;
    int change_flag = DISABLE;
    int ret;
    int i;

    memset(&route, 0, sizeof(struct route_entry));
    memset(&route_active, 0, sizeof(struct route_entry));

    ROUTE_LOG_DEBUG();

    if ((NULL == proute) || (NULL == prib))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    route_set_prefix(&route, prib);

    route.nhp_num = 1;

    /* 重新迭代直连下一跳为 0 的 nhp */
    for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, pnode, pnhp))
    {
        if (pnhp->active == ROUTE_STATUS_ACTIVE)
        {
            rib_count_number_active(pnhp, OPCODE_DELETE);
            pnhp->active = ROUTE_STATUS_INACTIVE;
        }

        if (pnhp->nexthop_connect.addr.ipv4 != 0)
        {
            continue;
        }

        memcpy(&route.nhp[0], pnhp, sizeof(struct nhp_entry));
        route_iteration(&route);

        pnhp->down_flag                 = route.nhp[0].down_flag;
        pnhp->ifindex                   = route.nhp[0].ifindex;
        pnhp->nhp_type                  = route.nhp[0].nhp_type;
        pnhp->nexthop_connect.type      = INET_FAMILY_IPV4;
        pnhp->nexthop_connect.addr.ipv4 = route.nhp[0].nexthop_connect.addr.ipv4;

        /* 非直连路由下一跳为本机接口 IP 地址，禁止 arp 学习 */
        if ((pnhp->ifindex != 0) && (pnhp->nhp_type != NHP_TYPE_CONNECT))
        {
            pif = route_if_lookup(pnhp->ifindex);
            if (NULL != pif)
            {
                if ((pnhp->nexthop_connect.addr.ipv4 != 0)
                    && ((pif->intf.ipv4[0].addr == pnhp->nexthop_connect.addr.ipv4)
                        || (pif->intf.ipv4[1].addr == pnhp->nexthop_connect.addr.ipv4)))
                {
                    pnhp->action = NHP_ACTION_TOCPU;
                    pnhp->nexthop_connect.addr.ipv4 = 0;
                }
            }
        }
    }

    ret = route_select_nhp(&route_active, prib);
    if (ERRNO_SUCCESS == ret)
    {
        /* 生成 ecmp */
        for (i=route_active.nhp_num; i<NHP_ECMP_NUM; i++)
        {
            ret = route_select_ecmp(&route_active, prib);
            if (ERRNO_SUCCESS != ret)
            {
                break;
            }
        }

        /* 生成 frr */
        if (route_active.nhp_num == NHP_NUM_ONE)
        {
            route_select_frr(&route_active, prib);
        }
    }
    else
    {
        return ERRNO_FAIL;
    }

    /* ecmp 数量达到上限，判断是否重选 */
    if ((proute->nhpinfo.nhp_type == NHP_TYPE_ECMP)
        && (proute->nhp_num == NHP_ECMP_NUM)
        && (route_active.nhpinfo.nhp_type == NHP_TYPE_ECMP)
        && (proute->nhp[0].distance == route_active.nhp[0].distance)
        && (proute->nhp[0].cost == route_active.nhp[0].cost))
    {
        for (i=0; i<NHP_ECMP_NUM; i++)
        {
            pnhp = rib_lookup_nhp(&prib->nhplist, &proute->nhp[i]);
            if (NULL == pnhp)
            {
                change_flag = ENABLE;

                break;
            }
            else
            {
                /* 路由有变化 */
                if ((pnhp->down_flag == LINK_DOWN)
                    || (pnhp->distance != proute->nhp[i].distance)
                    || (pnhp->cost != proute->nhp[i].cost)
                    || (pnhp->ifindex != proute->nhp[i].ifindex)
                    || (pnhp->nhp_type != proute->nhp[i].nhp_type))
                {
                    change_flag = ENABLE;

                    break;
                }

                memcpy(&proute->nhp[i], pnhp, sizeof(struct nhp_entry));
            }
        }

        /* 路由未变化，继续引用原来的路由 */
        if (DISABLE == change_flag)
        {
            for (i=0; i<NHP_ECMP_NUM; i++)
            {
                pnhp = rib_lookup_nhp(&prib->nhplist, &route_active.nhp[i]);
                if (NULL != pnhp)
                {
                    pnhp->active = ROUTE_STATUS_INACTIVE;

                    rib_count_number_active(pnhp, OPCODE_DELETE);
                }
            }

            for (i=0; i<NHP_ECMP_NUM; i++)
            {
                pnhp = rib_lookup_nhp(&prib->nhplist, &proute->nhp[i]);
                if (NULL != pnhp)
                {
                    pnhp->active = ROUTE_STATUS_ACTIVE;

                    rib_count_number_active(pnhp, OPCODE_ADD);
                }
            }

            return ERRNO_SUCCESS;
        }
    }

    proute->nhp_num = route_active.nhp_num;
    for (i=0; i<proute->nhp_num; i++)
    {
        proute->nhp[i] = route_active.nhp[i];
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 初始化 IPv6 活跃路由树
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 16:14:27
 * @note       :
 */
int routev6_tree_init(void)
{
    int vpn = 0;

    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        routev6_tree[vpn] = ptree_init(IPV6_MAX_PREFIXLEN);
        if (NULL == routev6_tree[vpn])
        {
            return ERRNO_MALLOC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 删除指定 vpn 实例的路由树
 * @param[in ] : vpn - 指定的 vpn 实例
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 16:14:56
 * @note       : 仅释放树节点和树本身内存，用户申请内存未释放
 */
void routev6_tree_delete(uint16_t vpn)
{
    if (vpn > L3VPN_SIZE)
    {
        return;
    }

    ptree_free(routev6_tree[vpn]);

    return;
}


/**
 * @brief      : 删除所有 vpn 实例的路由树
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 16:15:37
 * @note       : 仅释放树节点和树本身内存，用户申请内存未释放
 */
void routev6_tree_clear(void)
{
    int vpn = 0;

    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        ptree_free(routev6_tree[vpn]);
    }

    return;
}


/**
 * @brief      : 路由携带出接口，处理路由状态
 * @param[in ] : pnhp - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 16:15:57
 * @note       :
 */
static int routev6_check_outif(struct nhp_entry *pnhp)
{
    struct route_if *pif = NULL;

    ROUTE_LOG_DEBUG();

    pif = route_if_lookup(pnhp->ifindex);
    if (NULL == pif)
    {
        return ERRNO_NOT_FOUND;
    }

    if ((pnhp->vpn != pif->intf.vpn) || (pif->down_flag == LINK_DOWN))
    {
        return ERRNO_FAIL;
    }


    /* 不同接口类型处理方式不同 */
    if (IFM_TYPE_IS_LOOPBCK(pnhp->ifindex))
    {
        /* loopback 接口配置 IP ，nhp 状态为 LINK_UP */
        if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
        {
            pnhp->down_flag = LINK_UP;

            return ERRNO_SUCCESS;
        }
    }
    else if (IFM_TYPE_IS_VLANIF(pnhp->ifindex))
    {
        /* vlanif 接口配置 IP，nhp 状态为 LINK_UP */
        if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
        {
            pnhp->nexthop_connect = pnhp->nexthop;
            pnhp->down_flag       = LINK_UP;

            return ERRNO_SUCCESS;
        }

        /* 接口 IP 地址为借用，被借用接口为 LINK_UP 并且配置 IP，nhp 状态为 LINK_UP */
        if (0 != pif->intf.unnumbered_if)
        {
            pif = route_if_lookup(pif->intf.unnumbered_if);
            if (NULL != pif)
            {
                if ((pif->down_flag == LINK_UP) && ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
                {
                    pnhp->nexthop_connect = pnhp->nexthop;
                    pnhp->down_flag       = LINK_UP;

                    return ERRNO_SUCCESS;
                }
            }
        }
    }
    else if (IFM_TYPE_IS_METHERNET(pnhp->ifindex)
            || IFM_TYPE_IS_TRUNK(pnhp->ifindex))
    {
        /* 以太接口、trunk 接口配置 IP，nhp 状态为 LINK_UP */
        if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
        {
            pnhp->nexthop_connect = pnhp->nexthop;
            pnhp->down_flag       = LINK_UP;

            return ERRNO_SUCCESS;
        }

        if (ipv6_is_linklocal((struct ipv6_addr *)&pnhp->nexthop.addr)
            && (!ipv6_is_sitelocal((struct ipv6_addr *)&pnhp->nexthop.addr)))
        {
            if (ROUTE_IF_IPV6_EXIST(pif->intf.ipv6_link_local.addr))
            {
                pnhp->nexthop_connect = pnhp->nexthop;
                pnhp->down_flag       = LINK_UP;

                return ERRNO_SUCCESS;
            }
        }

        /* 接口 IP 地址为借用，被借用接口为 LINK_UP 并且配置 IP，nhp 状态为 LINK_UP */
        if (0 != pif->intf.unnumbered_if)
        {
            pif = route_if_lookup(pif->intf.unnumbered_if);
            if (NULL != pif)
            {
                if ((pif->down_flag == LINK_UP) && ROUTE_IF_IPV6_EXIST(pif->intf.ipv6[0].addr))
                {
                    pnhp->nexthop_connect = pnhp->nexthop;
                    pnhp->down_flag       = LINK_UP;

                    return ERRNO_SUCCESS;
                }
            }
        }
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : 使用原始路由前缀信息填充活跃路由信息
 * @param[in ] : proute - 活跃路由结构
 * @param[out] : prib   - 原始路由结构
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 16:50:45
 * @note       :
 */
static void routev6_set_prefix(struct route_entry *proute,         struct rib_entry *prib)
{
    if ((NULL == proute) || (NULL == prib))
    {
        return;
    }

    proute->prefix = prib->prefix;
    proute->vpn    = prib->vpn;

    return;
}


/**
 * @brief      : IPv4 路由下一跳迭代
 * @param[in ] : proute - 活跃路由结构
 * @param[out] : proute - 活跃路由结构
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 16:52:04
 * @note       : 携带出接口的路由特殊处理，不带出接口的路由需要迭代到直连下一跳
 * @note       : 最多支持三次迭代
 */
static int routev6_iteration(struct route_entry *proute)
{
    struct nhp_entry   *pnhp         = NULL;
    struct nhp_entry   *pnhp_match   = NULL;
    struct route_entry *proute_match = NULL;
    struct inet_prefix  prefix;
    uint8_t  pdown_flag = 0;
    int      i;
    int      ret;
    int      count = 0;

    for (i=0; i<proute->nhp_num; i++)
    {
        pnhp = &proute->nhp[i];
        if (pnhp->nhp_type == NHP_TYPE_CONNECT)
        {
            /* 直连路由状态为 up */
            pnhp->down_flag       = LINK_UP;
            pnhp->nexthop_connect = pnhp->nexthop;
        }
        else if (pnhp->nhp_type == NHP_TYPE_TUNNEL)
        {
            /* å‡ºæŽ¥å£ä¸º tunnel çš„è·¯ç”±ï¼Œtunnel æŽ¥å£ upï¼Œè·¯ç”±çŠ¶æ€ä¸º up */
            ret  = ifm_get_link(pnhp->ifindex, MODULE_ID_ROUTE, &pdown_flag);
            if ((0 == ret) && (pdown_flag == IFNET_LINKUP))
            {
                pnhp->down_flag       = LINK_UP;
                pnhp->nexthop_connect = pnhp->nexthop;
            }
        }
        else if (pnhp->nhp_type == NHP_TYPE_LSP)
        {
            /* lsp 路由状态为 up */
            pnhp->down_flag       = LINK_UP;
            pnhp->nexthop_connect = pnhp->nexthop;
        }
        else if (pnhp->nhp_type == NHP_TYPE_IP)
        {
            /* ip 路由根据不同情况再处理 */
            if ((!ROUTE_IF_IPV6_EXIST(pnhp->nexthop.addr.ipv6))
            && (pnhp->action == NHP_ACTION_DROP))
            {
                /* 黑洞路由状态为 up */
                pnhp->down_flag = LINK_UP;
            }
            else if ((0 != pnhp->ifindex) && (!ROUTE_PROTO_IS_LSP(pnhp)))
            {
                /* 指定出接口的路由需要根据接口类型做不同处理 */
                ret = routev6_check_outif(pnhp);
                if (ret != ERRNO_SUCCESS)
                {
                    pnhp->down_flag = LINK_DOWN;
                }
            }
            else
            {
                /* 普通 ip 路由需要在 3 次之内迭代到直连状态才会为 up */
                pnhp_match = pnhp;

next:

                /* 路由迭代次数大于 3 次，迭代失败 */
                if (++count > ROUTE_CONVER_NUM)
                {
                    pnhp->down_flag = LINK_DOWN;

                    return ERRNO_FAIL;
                }

                memcpy(prefix.addr.ipv6, pnhp_match->nexthop.addr.ipv6, IPV6_ADDR_LEN);
                prefix.prefixlen = IPV6_MAX_PREFIXLEN;
                prefix.type      = INET_FAMILY_IPV6;

                proute_match = routev6_match(&prefix, pnhp_match->vpn);
                if (proute_match == NULL)
                {
                    pnhp->down_flag = LINK_DOWN;
                }
                else if (proute_match->nhp[0].nhp_type == NHP_TYPE_CONNECT)
                {
                    /* 下一跳迭代到本机主机路由，路由不 UP */
                    if (proute_match->prefix.prefixlen == IPV6_MAX_PREFIXLEN)
                    {
                        pnhp->down_flag = LINK_DOWN;

                        return ERRNO_SUCCESS;
                    }

                    /* 迭代到直连路由，状态为 up */
                    pnhp->down_flag       = LINK_UP;
                    pnhp->nexthop_connect = pnhp_match->nexthop;
                    pnhp->ifindex         = proute_match->nhp[0].ifindex;

                    if (count > 1)
                    {
                        pnhp->cost += pnhp_match->cost;
                    }
                }
                else if (proute_match->nhp[0].nhp_type == NHP_TYPE_LSP)
                {
                    /* 迭代到 lsp 路由，状态为 up */
                    pnhp->ifindex   = proute_match->nhp[0].ifindex;
                    pnhp->down_flag = LINK_UP;
                    pnhp->nhp_type  = NHP_TYPE_LSP;
                }
                else if (proute_match->nhp[0].nhp_type == NHP_TYPE_IP)
                {
                    /* 静态路由迭代到自身 */
                    if (0 == memcmp(pnhp_match, &proute_match->nhp[0], sizeof(struct nhp_entry)))
                    {
                        pnhp->ifindex   = proute_match->nhp[0].ifindex;
                        pnhp->down_flag = LINK_UP;

                        return ERRNO_SUCCESS;
                    }

                    /* 迭代到默认路由不 up */
                    if ((!ROUTE_IF_IPV6_EXIST(proute_match->prefix.addr.ipv6))
                        && (0 == proute_match->prefix.prefixlen))
                    {
                        pnhp->down_flag = LINK_DOWN;

                        return ERRNO_SUCCESS;
                    }

                    /* 迭代到下一跳为链路本地地址的路由，状态为 up */
                    if (((proute_match->nhp[0].nexthop.addr.ipv6[0] == 0xfe)
                        && ((proute_match->nhp[0].nexthop.addr.ipv6[1] & 0x80) == 0x80)))
                    {
                        if (0 != proute_match->nhp[0].ifindex)
                        {
                            pnhp->ifindex         = proute_match->nhp[0].ifindex;
                            pnhp->nexthop_connect = proute_match->nhp[0].nexthop;
                            pnhp->down_flag       = LINK_UP;

                            return ERRNO_SUCCESS;
                        }
                    }

                    /* 迭代到非直连路由继续迭代 */
                    pnhp_match = &proute_match->nhp[0];
                    goto next;
                }
                else
                {
                    /* 迭代失败 */
                    pnhp->down_flag = LINK_DOWN;
                }
            }
        }
        else
        {
            /* 未知下一跳类型 */
            pnhp->down_flag = LINK_DOWN;

            return ERRNO_FAIL;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv6 活跃路由添加
 * @param[in ] : prib - 原始路由结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 16:59:10
 * @note       :
 */
int routev6_add(struct rib_entry *prib)
{
    struct ptree_node  *pnode  = NULL;
    struct route_entry *proute = NULL;
    int i = 0;
    int ret;

    ROUTE_LOG_DEBUG();

    if (prib == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ROUTEV6_NUMBER_ACTIVE == ROUTEV6_NUM_MAX)
    {
        return ERRNO_OVERSIZE;
    }

    proute = (struct route_entry *)XCALLOC(MTYPE_ROUTE_TABLE, sizeof(struct route_entry));
    if (NULL == proute)
    {
        ROUTE_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    /* 下一跳优选 */
    routev6_set_prefix(proute, prib);
    ret = routev6_select(proute, prib);
    if (ERRNO_SUCCESS != ret)
    {
        XFREE(MTYPE_ROUTE_TABLE, proute);

        return ERRNO_SUCCESS;
    }

    /* 下一跳分离 */
    ret = route_nhp_add_process(proute);
    if ((ERRNO_SUCCESS != ret) && (ERRNO_NOT_UPDATE != ret))
    {
        ROUTE_LOG_ERROR("errcode = %d\n", ret);

        for (i=0; i<proute->nhp_num; i++)
        {
            ribv6_count_number_active(&proute->nhp[i], OPCODE_DELETE);
        }

        return ret;
    }

    /* ä¸‹å‘é…ç½® */
    route_add_msg_to_send_list(proute, sizeof(struct route_entry), 1, MODULE_ID_FTM,
                        MODULE_ID_ROUTE, IPC_TYPE_FIB, ROUTE_SUBTYPE_IPV6,
                        IPC_OPCODE_ADD, 0);

    /* 路由数据存储 */
    pnode = ptree_node_get(routev6_tree[proute->vpn],
                (u_char *)&proute->prefix.addr.ipv6, proute->prefix.prefixlen);
    if (pnode == NULL)
    {
        ROUTE_LOG_ERROR("ptree get\n");

        for (i=0; i<proute->nhp_num; i++)
        {
            ribv6_count_number_active(&proute->nhp[i], OPCODE_DELETE);
        }

        route_nhp_delete_process(proute);
        XFREE(MTYPE_ROUTE_TABLE, proute);

        return ERRNO_MALLOC;
    }

    pnode->info = (void *)proute;

    /* 活跃路由变化通知 */
    routev6_event_notify(proute->nhp[0].protocol, proute, IPC_OPCODE_ADD);

    if (g_route.conver_flag == ENABLE)
    {
        if (NULL == g_route.routev6_timer)
        {
            g_route.routev6_timer = ROUTE_TIMER_ADD(routev6_static_update_routing, NULL, 1);
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv6 活跃路由更新
 * @param[in ] : prib - 原始路由结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:03:40
 * @note       :
 */
int routev6_update(struct rib_entry *prib)
{
    struct ptree_node  *pnode  = NULL;
    struct route_entry *proute = NULL;
    struct route_entry  route_del;
    struct route_entry  route_notify;
    int ret  = ERRNO_SUCCESS;
    int flag = DISABLE;
    int i    = 0;
    int j    = 0;

    ROUTE_LOG_DEBUG();

    if (NULL == prib)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pnode = ptree_node_lookup(routev6_tree[prib->vpn], (u_char *)&prib->prefix.addr.ipv4,
                                prib->prefix.prefixlen);
    if ((pnode == NULL) || (pnode->info == NULL))
    {
        return ERRNO_FAIL;
    }

    proute = (struct route_entry *)pnode->info;
    memcpy(&route_del, proute, sizeof(struct route_entry));

    /* 下一跳优选 */
    ret = routev6_select(proute, prib);
    if (ERRNO_SUCCESS != ret)
    {
        routev6_delete(&prib->prefix, prib->vpn);

        return ERRNO_SUCCESS;
    }

    /* 下一跳分离 */
    ret = route_nhp_add_process(proute);
    if (ERRNO_NOT_UPDATE == ret)
    {
        /* 下一跳协议类型或 cost 发生变化，触发通知事件 */
        for (i=0; i<route_del.nhp_num; i++)
        {
            if ((route_del.nhp[i].protocol != proute->nhp[i].protocol)
                || (route_del.nhp[i].cost != proute->nhp[i].cost)
                || (route_del.nhp[i].distance != proute->nhp[i].distance))
            {
                goto notify;
            }
        }

        /* frr 主备反转，触发通知事件 */
        if (proute->nhpinfo.nhp_type == NHP_TYPE_FRR)
        {
            if ((memcmp(proute->nhp[0].nexthop.addr.ipv6, route_del.nhp[0].nexthop.addr.ipv6,
                    IPV6_ADDR_LEN))
                || (memcmp(proute->nhp[1].nexthop.addr.ipv6, route_del.nhp[1].nexthop.addr.ipv6,
                    IPV6_ADDR_LEN)))
            {
                goto notify;
            }
        }

        return ERRNO_SUCCESS;
    }
    else if (ERRNO_SUCCESS != ret)
    {
        ROUTE_LOG_ERROR("errcode = %d\n", ret);

        for (i=0; i<proute->nhp_num; i++)
        {
            ribv6_count_number_active(&proute->nhp[i], OPCODE_DELETE);
        }

        return ret;
    }

    /* ä¸‹å‘é…ç½®æ›´æ–° */
    route_add_msg_to_send_list(proute, sizeof(struct route_entry), 1, MODULE_ID_FTM,
                        MODULE_ID_ROUTE, IPC_TYPE_FIB, ROUTE_SUBTYPE_IPV6,
                        IPC_OPCODE_UPDATE, 0);

    /* 删除被更新的下一跳 */
    route_nhp_delete_process(&route_del);

notify:

    /* 活跃路由变化通知，先通知删除事件，再通知添加事件 */
    memcpy(&route_notify, &route_del, sizeof(struct route_entry));

    for (i=0; i<route_del.nhp_num; i++)
    {
        route_notify.nhp[0]  = route_del.nhp[i];
        route_notify.nhp_num = 1;

        for (j=0; j<proute->nhp_num; j++)
        {
            /* 未变化的路由不通知 */
            if ((0 == memcmp(route_notify.nhp[0].nexthop.addr.ipv6,
                            proute->nhp[j].nexthop.addr.ipv6, IPV6_ADDR_LEN))
                && (route_notify.nhp[0].vpn == proute->nhp[j].vpn)
                && (route_notify.nhp[0].protocol == proute->nhp[j].protocol)
                && (route_notify.nhp[0].distance == proute->nhp[j].distance)
                && (route_notify.nhp[0].cost == proute->nhp[j].cost))
            {
                flag = ENABLE;

                break;
            }

            if (proute->nhpinfo.nhp_type == NHP_TYPE_FRR)
            {
                break;
            }
        }

        if (ENABLE == flag)
        {
            flag = DISABLE;

            continue;
        }

        if ((i>0) && (route_del.nhpinfo.nhp_type == NHP_TYPE_FRR))
        {
            break;
        }

        routev6_event_notify(route_del.nhp[i].protocol, &route_notify, IPC_OPCODE_DELETE);
    }

    memcpy(&route_notify, proute, sizeof(struct route_entry));

    for (i=0; i<proute->nhp_num; i++)
    {
        route_notify.nhp[0]  = proute->nhp[i];
        route_notify.nhp_num = 1;

        for (j=0; j<route_del.nhp_num; j++)
        {
            /* 未变化的路由不通知 */
            if ((0 == memcmp(route_notify.nhp[0].nexthop.addr.ipv6,
                route_del.nhp[j].nexthop.addr.ipv6, IPV6_ADDR_LEN))
                && (route_notify.nhp[0].vpn == route_del.nhp[j].vpn)
                && (route_notify.nhp[0].protocol == route_del.nhp[j].protocol)
                && (route_notify.nhp[0].distance == route_del.nhp[j].distance)
                && (route_notify.nhp[0].cost == route_del.nhp[j].cost))
            {
                flag = ENABLE;

                break;
            }

            if (route_del.nhpinfo.nhp_type == NHP_TYPE_FRR)
            {
                break;
            }
        }

        if (ENABLE == flag)
        {
            flag = DISABLE;

            continue;
        }

        if ((i>0) && (proute->nhpinfo.nhp_type == NHP_TYPE_FRR))
        {
            break;
        }

        routev6_event_notify(proute->nhp[i].protocol, &route_notify, IPC_OPCODE_ADD);
    }

    if (g_route.conver_flag == ENABLE)
    {
        if (NULL == g_route.routev6_timer)
        {
            g_route.routev6_timer = ROUTE_TIMER_ADD(routev6_static_update_routing, NULL, 1);
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv6 活跃路由删除
 * @param[in ] : prefix - 路由前缀
 * @param[in ] : vpn    - vpn 实例
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:07:26
 * @note       :
 */
int routev6_delete(struct inet_prefix *prefix, uint16_t vpn)
{
    struct ptree_node  *pnode  = NULL;
    struct route_entry *proute = NULL;

    ROUTE_LOG_DEBUG();

    if (vpn > L3VPN_SIZE)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pnode = ptree_node_lookup(routev6_tree[vpn], (u_char *)&prefix->addr.ipv4,
                        prefix->prefixlen);
    if (pnode == NULL)
    {
        return ERRNO_SUCCESS;
    }

    proute = (struct route_entry *)pnode->info;

    /* ä¸‹å‘åˆ é™¤äº‹ä»¶ */
    route_add_msg_to_send_list(proute, sizeof(struct route_entry), 1, MODULE_ID_FTM,
                        MODULE_ID_ROUTE, IPC_TYPE_FIB, ROUTE_SUBTYPE_IPV6,
                        IPC_OPCODE_DELETE, 0);

    /* 激活路由变化通知事件 */
    routev6_event_notify(proute->nhp[0].protocol, proute, IPC_OPCODE_DELETE);

    /* 删除 fib 引用的 nhp */
    route_nhp_delete_process(proute);

    /* 释放路由节点 */
    pnode->info = NULL;
    pnode->lock = 0;

    ptree_node_delete(pnode);

    if (g_route.conver_flag == ENABLE)
    {
        if (NULL == g_route.routev6_timer)
        {
            g_route.routev6_timer = ROUTE_TIMER_ADD(routev6_static_update_routing, NULL, 1);
        }
    }

    XFREE(MTYPE_ROUTE_TABLE, proute);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv6 活跃路由查找
 * @param[in ] : prefix - 路由前缀
 * @param[in ] : vpn    - vpn 实例
 * @param[out] :
 * @return     : 成功返回活跃路由结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:08:39
 * @note       :
 */
struct route_entry *routev6_lookup(struct inet_prefix *prefix, uint16_t vpn)
{
    struct ptree_node *pnode = NULL;

    ROUTE_LOG_DEBUG();

    if ((NULL == prefix) || (vpn > L3VPN_SIZE))
    {
        return NULL;
    }

    pnode = ptree_node_lookup(routev6_tree[vpn], (u_char *)&prefix->addr.ipv4,
                                prefix->prefixlen);
    if (pnode == NULL)
    {
        return NULL;
    }

    ptree_unlock_node(pnode);

    return (struct route_entry *)pnode->info;
}


/**
 * @brief      : IPv6 活跃路由最长掩码匹配
 * @param[in ] : prefix - 路由前缀
 * @param[in ] : vpn    - vpn 实例
 * @param[out] :
 * @return     : 成功返回活跃路由结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:09:11
 * @note       :
 */
struct route_entry *routev6_match(struct inet_prefix *prefix, uint16_t vpn)
{
    struct ptree_node *pnode = NULL;

    ROUTE_LOG_DEBUG();

    if (vpn > L3VPN_SIZE)
    {
        return NULL;
    }

    pnode = ptree_node_match1(routev6_tree[vpn], (u_char *)&prefix->addr.ipv4,
                        prefix->prefixlen);
    if (pnode == NULL)
    {
        return NULL;
    }

    ptree_unlock_node(pnode);

    return (struct route_entry *)pnode->info;
}


/**
 * @brief      : IPv6 路由生成 frr
 * @param[in ] : proute - 活跃路由结构
 * @param[in]  : prib   - 原始路由结构
 * @param[out] : proute - 活跃路由结构
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:09:36
 * @note       : 优先级不同出接口不同的两个活跃的 nhp 形成 frr
 */
static int routev6_select_frr(struct route_entry *proute, struct rib_entry *prib)
{
    struct listnode  *plistnode   = NULL;
    struct nhp_entry *pnhp        = NULL;
    struct nhp_entry *pnhp_select = NULL;
    struct nhp_entry *pnhp_active = NULL;
    int distance = ROUTE_METRIC_UNKNOW+1;

    ROUTE_LOG_DEBUG();

    if (g_route.routev6_frr_flag == DISABLE)
    {
        return ERRNO_SUCCESS;
    }

    if ((NULL == prib) || (NULL == proute))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ROUTEV6_NUMBER_ACTIVE == ROUTEV6_NUM_MAX)
    {
        return ERRNO_OVERSIZE;
    }

    pnhp_active = &proute->nhp[0];

    /* 黑洞路由不存在 frr */
    if ((!ROUTE_IF_IPV6_EXIST(pnhp_active->nexthop.addr.ipv6))
        && (pnhp_active->action == NHP_ACTION_DROP))
    {
        return ERRNO_FAIL;
    }

    /* 直连路由不会形成 frr */
    if (pnhp_active->protocol == ROUTE_PROTO_CONNECT)
    {
        return ERRNO_FAIL;
    }

    /* 下一跳为链路本地地址不会形成 frr */
    if ((ipv6_is_linklocal((struct ipv6_addr *)&pnhp_active->nexthop.addr)
        && (!ipv6_is_sitelocal((struct ipv6_addr *)&pnhp_active->nexthop.addr))))
    {
        return ERRNO_FAIL;
    }

    for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
    {
        /* 活跃的 nhp 已被选中，忽略 */
        if (pnhp->active == ROUTE_STATUS_ACTIVE)
        {
            continue;
        }

        /* 黑洞路由不存在 frr */
        if ((!ROUTE_IF_IPV6_EXIST(pnhp->nexthop.addr.ipv6))
            && (pnhp->action == NHP_ACTION_DROP))
        {
            continue;
        }

        /* lsp 路由不能与普通路由形成 frr */
        if ((ROUTE_PROTO_IS_LSP(pnhp_active) && (!ROUTE_PROTO_IS_LSP(pnhp)))
            || (ROUTE_PROTO_IS_LSP(pnhp) && (!ROUTE_PROTO_IS_LSP(pnhp_active))))
        {
            continue;
        }

        /* 下一跳为链路本地地址不会形成 frr */
        if ((ipv6_is_linklocal((struct ipv6_addr *)&pnhp->nexthop.addr)
            && (!ipv6_is_sitelocal((struct ipv6_addr *)&pnhp->nexthop.addr))))
        {
            return ERRNO_FAIL;
        }

        /* 选出优先级次优的 nhp */
        if ((pnhp->distance < distance)
            && (pnhp->down_flag == LINK_UP)
            && (pnhp->ifindex != pnhp_active->ifindex))
        {
            distance    = pnhp->distance;
            pnhp_select = pnhp;
        }
    }

    if (pnhp_select != NULL)
    {
        ROUTE_LOG_DEBUG("select\n");

        pnhp_select->active = ROUTE_STATUS_ACTIVE;

        ribv6_count_number_active(pnhp_select, OPCODE_ADD);

        memcpy(&proute->nhp[proute->nhp_num], pnhp_select, sizeof(struct nhp_entry));
        proute->nhpinfo.nhp_type = NHP_TYPE_FRR;
        proute->nhp_num++;

        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : IPv4 路由生成 ecmp
 * @param[in ] : proute - 活跃路由结构
 * @param[in]  : prib   - 原始路由结构
 * @param[out] : proute - 活跃路由结构
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:11:28
 * @note       : distance 和 cost 相同，下一跳不同的活跃 nhp 形成 ecmp
 */
static int routev6_select_ecmp(struct route_entry *proute, struct rib_entry *prib)
{
    struct listnode  *plistnode   = NULL;
    struct nhp_entry *pnhp        = NULL;
    struct nhp_entry *pnhp_select = NULL;
    struct nhp_entry *pnhp_active = NULL;

    ROUTE_LOG_DEBUG();

    if (g_route.routev6_ecmp_flag == DISABLE)
    {
        return ERRNO_SUCCESS;
    }

    if ((NULL == prib) || (NULL == proute))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ROUTEV6_NUMBER_ACTIVE == ROUTEV6_NUM_MAX)
    {
        return ERRNO_OVERSIZE;
    }

    pnhp_active = &proute->nhp[0];

    /* 黑洞路由不存在 ecmp */
    if ((!ROUTE_IF_IPV6_EXIST(pnhp_active->nexthop.addr.ipv6))
        && (pnhp_active->action == NHP_ACTION_DROP))
    {
        return ERRNO_FAIL;
    }

    /* 直连路由不会形成 ecmp */
    if (pnhp_active->protocol == ROUTE_PROTO_CONNECT)
    {
        return ERRNO_FAIL;
    }

    /* 下一跳为链路本地地址不会形成 ecmp */
    if ((ipv6_is_linklocal((struct ipv6_addr *)&pnhp_active->nexthop.addr)
        && (!ipv6_is_sitelocal((struct ipv6_addr *)&pnhp_active->nexthop.addr))))
    {
        return ERRNO_FAIL;
    }

    for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
    {
        /* 活跃的 nhp 已被选中，忽略 */
        if (pnhp->active == ROUTE_STATUS_ACTIVE)
        {
            continue;
        }

        /* 黑洞路由不存在 ecmp */
        if ((!ROUTE_IF_IPV6_EXIST(pnhp->nexthop.addr.ipv6))
            && (pnhp->action == NHP_ACTION_DROP))
        {
            continue;
        }

        /* lsp 路由不能与普通路由形成 ecmp */
        if ((ROUTE_PROTO_IS_LSP(pnhp_active) && (!ROUTE_PROTO_IS_LSP(pnhp)))
            || (ROUTE_PROTO_IS_LSP(pnhp) && (!ROUTE_PROTO_IS_LSP(pnhp_active))))
        {
            continue;
        }

        /* 下一跳为链路本地地址不会形成 ecmp */
        if ((ipv6_is_linklocal((struct ipv6_addr *)&pnhp->nexthop.addr)
            && (!ipv6_is_sitelocal((struct ipv6_addr *)&pnhp->nexthop.addr))))
        {
            return ERRNO_FAIL;
        }

        /* 选出与第一条 distance、cost 相同，下一跳不同的 nhp */
        if ((pnhp->distance == pnhp_active->distance)
            && (pnhp->cost == pnhp_active->cost)
            && (pnhp->down_flag == LINK_UP)
            && (memcmp(pnhp->nexthop.addr.ipv6, pnhp_active->nexthop.addr.ipv6, IPV6_ADDR_LEN)))
        {
            pnhp_select = pnhp;

            break;
        }
    }

    if (pnhp_select != NULL)
    {
        ROUTE_LOG_DEBUG("select\n");

        pnhp_select->active = ROUTE_STATUS_ACTIVE;

        ribv6_count_number_active(pnhp_select, OPCODE_ADD);

        memcpy(&proute->nhp[proute->nhp_num], pnhp_select, sizeof(struct nhp_entry));
        proute->nhpinfo.nhp_type = NHP_TYPE_ECMP;
        proute->nhp_num++;

        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : IPv4 活跃路由选择 nhp
 * @param[in ] : proute - 活跃路由结构
 * @param[in]  : prib   - 原始路由结构
 * @param[out] : proute - 活跃路由结构
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:13:09
 * @note       :
 */
static int routev6_select_nhp(struct route_entry *proute, struct rib_entry *prib)
{
    struct listnode  *plistnode   = NULL;
    struct nhp_entry *pnhp        = NULL;
    struct nhp_entry *pnhp_select = NULL;
    int               distance    = ROUTE_METRIC_UNKNOW+1;

    if ((NULL == prib) || (NULL == proute))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    ROUTE_LOG_DEBUG("prefix = %x, %d\n", prib->prefix.addr.ipv4, prib->prefix.prefixlen);

    if (ROUTEV6_NUMBER_ACTIVE == ROUTEV6_NUM_MAX)
    {
        return ERRNO_OVERSIZE;
    }

    /* 选出优先级最高并且状态为 up 的 nhp */
    for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, plistnode, pnhp))
    {
        if (((pnhp->distance < distance) && (pnhp->down_flag == LINK_UP))
            || ((pnhp->distance == distance )&& ((!ROUTE_IF_IPV6_EXIST(pnhp->nexthop.addr.ipv6))
                && (pnhp->action == NHP_ACTION_DROP))))
        {
            distance    = pnhp->distance;
            pnhp_select = pnhp;
        }
    }

    if (NULL != pnhp_select)
    {
        ROUTE_LOG_DEBUG("select\n");

        pnhp_select->active = ROUTE_STATUS_ACTIVE;

        ribv6_count_number_active(pnhp_select, OPCODE_ADD);

        memcpy(&proute->nhp[0], pnhp_select, sizeof(struct nhp_entry));
        proute->nhp_num++;

        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : IPv4 路由下一跳优选
 * @param[in ] : proute - 活跃路由结构
 * @param[in]  : prib   - 原始路由结构
 * @param[out] : proute - 活跃路由结构
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:14:20
 * @note       :
 */

int routev6_select(struct route_entry *proute, struct rib_entry *prib)
{
    struct listnode   *pnode = NULL;
    struct nhp_entry  *pnhp  = NULL;
    struct route_entry route;
    struct route_entry route_active;
    int change_flag = DISABLE;
    int ret;
    int i;

    memset(&route, 0, sizeof(struct route_entry));
    memset(&route_active, 0, sizeof(struct route_entry));

    ROUTE_LOG_DEBUG();

    if ((NULL == proute) || (NULL == prib))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    routev6_set_prefix(&route, prib);

    route.nhp_num = 1;

    /* 重新迭代直连下一跳为 0 的 nhp */
    for (ALL_LIST_ELEMENTS_RO(&prib->nhplist, pnode, pnhp))
    {
        if (pnhp->active == ROUTE_STATUS_ACTIVE)
        {
            ribv6_count_number_active(pnhp, OPCODE_DELETE);

            pnhp->active = ROUTE_STATUS_INACTIVE;
        }

        if (ROUTE_IF_IPV6_EXIST(pnhp->nexthop_connect.addr.ipv6))
        {
            continue;
        }

        memcpy(&route.nhp[0], pnhp, sizeof(struct nhp_entry));
        routev6_iteration(&route);

        pnhp->down_flag            = route.nhp[0].down_flag;
        pnhp->ifindex              = route.nhp[0].ifindex;
        pnhp->nhp_type             = route.nhp[0].nhp_type;
        pnhp->nexthop_connect.type = INET_FAMILY_IPV6;
        memcpy(pnhp->nexthop_connect.addr.ipv6, route.nhp[0].nexthop_connect.addr.ipv6,
                IPV6_ADDR_LEN);
    }

    ret = routev6_select_nhp(&route_active, prib);
    if (ERRNO_SUCCESS == ret)
    {
        /* 生成 ecmp */
        for (i=route_active.nhp_num; i<NHP_ECMP_NUM; i++)
        {
            ret = routev6_select_ecmp(&route_active, prib);
            if (ERRNO_SUCCESS != ret)
            {
                break;
            }
        }

        /* 生成 frr */
        if (route_active.nhp_num == NHP_NUM_ONE)
        {
            routev6_select_frr(&route_active, prib);
        }
    }
    else
    {
        return ERRNO_FAIL;
    }

    /* ecmp 数量达到上限，判断是否重选 */
    if ((proute->nhpinfo.nhp_type == NHP_TYPE_ECMP)
        && (proute->nhp_num == NHP_ECMP_NUM)
        && (route_active.nhpinfo.nhp_type == NHP_TYPE_ECMP)
        && (proute->nhp[0].distance == route_active.nhp[0].distance)
        && (proute->nhp[0].cost == route_active.nhp[0].cost))
    {
        for (i=0; i<NHP_ECMP_NUM; i++)
        {
            pnhp = ribv6_lookup_nhp(&prib->nhplist, &proute->nhp[i]);
            if (NULL == pnhp)
            {
                change_flag = ENABLE;

                break;
            }
            else
            {
                /* 路由有变化 */
                if ((pnhp->down_flag == LINK_DOWN)
                    || (pnhp->distance != proute->nhp[i].distance)
                    || (pnhp->cost != proute->nhp[i].cost)
                    || (pnhp->ifindex != proute->nhp[i].ifindex)
                    || (pnhp->nhp_type != proute->nhp[i].nhp_type))
                {
                    change_flag = ENABLE;

                    break;
                }

                memcpy(&proute->nhp[i], pnhp, sizeof(struct nhp_entry));
            }
        }

        /* 路由未变化，继续引用原来的路由 */
        if (DISABLE == change_flag)
        {
            for (i=0; i<NHP_ECMP_NUM; i++)
            {
                pnhp = ribv6_lookup_nhp(&prib->nhplist, &route_active.nhp[i]);
                if (NULL != pnhp)
                {
                    pnhp->active = ROUTE_STATUS_INACTIVE;

                    ribv6_count_number_active(pnhp, OPCODE_DELETE);
                }
            }

            for (i=0; i<NHP_ECMP_NUM; i++)
            {
                pnhp = ribv6_lookup_nhp(&prib->nhplist, &proute->nhp[i]);
                if (NULL != pnhp)
                {
                    pnhp->active = ROUTE_STATUS_ACTIVE;

                    ribv6_count_number_active(pnhp, OPCODE_ADD);
                }
            }

            return ERRNO_SUCCESS;
        }
    }

    proute->nhp_num = route_active.nhp_num;
    for (i=0; i<proute->nhp_num; i++)
    {
        proute->nhp[i] = route_active.nhp[i];
    }

    return ERRNO_SUCCESS;
}

/* h3c added */

int route_active_get_bulk_for_mib(struct route_entry *pindex, struct route_entry *padata, uint32_t index_flag)
{
    struct ptree_node  *pnode       = NULL;
    struct ptree_node  *pnodenext   = NULL;
    struct route_entry *proute      = NULL;
    uint32_t msg_num  = IPC_MSG_LEN / sizeof(struct route_entry);
    uint32_t data_num = 0;
    uint32_t vpn = 0;

    if(1 == index_flag)
    {
        PTREE_LOOP(route_tree[vpn], proute, pnode)
        {
            if (NULL != proute)
            {
                memcpy(&padata[data_num], proute, sizeof(struct route_entry));
                if(++data_num >= msg_num)
                {
                    return data_num;
                }
            }
        }
    }
    else
    {
        pnode = ptree_node_lookup(route_tree[vpn], (u_char *)&pindex->prefix.addr.ipv4, pindex->prefix.prefixlen);

        while(pnode)
        {
            pnodenext = ptree_next(pnode);
            pnode = pnodenext;
            if(NULL == pnode)
            {
                return data_num;
            }
            proute = (struct route_entry *)pnode->info;
            if(NULL == proute)
            {
                continue;
            }
            memcpy(&padata[data_num], proute, sizeof(struct route_entry));
            
            if(++data_num == msg_num)
            {
                return data_num;
            }
        }
    }

    return data_num;
}

static struct ptree *route_tree_mib[L3VPN_SIZE+1];


int route_tree_mib_init(void)
{
    int vpn = 0;

    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        route_tree_mib[vpn] = ptree_init(IPV4_MAX_PREFIXLEN);
        if (NULL == route_tree_mib[vpn])
        {
            return ERRNO_MALLOC;
        }
    }
    return ERRNO_SUCCESS;
}



/* rout tree for mib alloc */
void route_tree_mib_copy(void)
{
    struct ptree_node  *pnode       = NULL;
    struct route_entry *proute      = NULL;
    struct route_entry *prepeat       = NULL;
    struct ptree_node  *pnode_mib       = NULL;
    struct route_entry *proute_mib      = NULL;
    struct route_entry  lroute;
    uint32_t vpn = 0;
    int num = 0;
    
    PTREE_LOOP(route_tree[vpn], proute, pnode)
    {
        if (NULL != proute)
        {
            /* get the route from the tree */
            pnode->info = NULL;

            memcpy(&lroute, proute, sizeof(struct route_entry));
            prepeat = route_match(&lroute.prefix, vpn);
            if(NULL == prepeat)
            {
                proute_mib = (struct route_entry *)XCALLOC(MTYPE_ROUTE_TABLE, sizeof(struct route_entry));
                if(proute_mib)
                {
                    num ++;
                    memcpy(proute_mib, proute, sizeof(struct route_entry));

                    pnode_mib = ptree_node_get(route_tree_mib[vpn], (u_char *)&proute_mib->prefix.addr.ipv4,
                                        proute_mib->prefix.prefixlen);

                    if(NULL != pnode_mib)
                    {
                        pnode_mib->info = (void *)proute_mib;
                    }
                    else
                    {
                        XFREE(MTYPE_ROUTE_TABLE, proute_mib);
                    }
                }
            }

            /* recover the route to the tree node */
            pnode->info = proute;
        }
    }
}


/* route tree for mib clear */
void route_tree_mib_free(void)
{
    int vpn = 0;
    struct ptree_node  *pnode_mib       = NULL;
    struct route_entry *proute_mib      = NULL;
    int num = 0;
    /* free pnode -> info */
    PTREE_LOOP(route_tree_mib[vpn], proute_mib, pnode_mib)
    {
        if (NULL != proute_mib)
        {
            XFREE(MTYPE_ROUTE_TABLE, proute_mib);
            pnode_mib->info = NULL;
        }
    }

    /* free ptree_node */
    ptree_free(route_tree_mib[vpn]);
}

int route_active_get_bulk_for_mib_brief(struct route_entry *pindex, struct route_entry *padata, uint32_t index_flag)
{
    /* catche route tree mib for RFC1213_MIB */
    struct ptree_node  *pnode       = NULL;
    struct ptree_node  *pnodenext   = NULL;
    struct route_entry *proute      = NULL;
    uint32_t msg_num  = IPC_MSG_LEN / sizeof(struct route_entry);
    uint32_t data_num = 0;
    uint32_t vpn = 0;

    if(1 == index_flag)
    {
        route_tree_mib_init();
        route_tree_mib_copy();

        PTREE_LOOP(route_tree_mib[vpn], proute, pnode)
        {
            if (NULL != proute)
            {
                memcpy(&padata[data_num], proute, sizeof(struct route_entry));
                if(++data_num >= msg_num)
                {
                    goto out;
                }
            }
        }

        route_tree_mib_free();
    }
    else
    {
        route_tree_mib_init();
        route_tree_mib_copy();
        pnode = ptree_node_lookup(route_tree_mib[vpn], (u_char *)&pindex->prefix.addr.ipv4, pindex->prefix.prefixlen);

        while(pnode)
        {
            pnodenext = ptree_next(pnode);
            pnode = pnodenext;
            if(NULL == pnode)
            {
                goto out;
            }
            proute = (struct route_entry *)pnode->info;
            if(NULL == proute)
            {
                continue;
            }
            memcpy(&padata[data_num], proute, sizeof(struct route_entry));
            
            if(++data_num == msg_num)
            {
                goto out;
            }
        }
        route_tree_mib_free();
    }
    
out:
    return data_num;
}

