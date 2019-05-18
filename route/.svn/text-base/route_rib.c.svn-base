/**
 * @file      : route_rib.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月26日 9:25:52
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/memory.h>
#include <lib/prefix.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include "route_main.h"
#include "route_static.h"
#include "route_rib.h"
#include "route.h"

/* IPv4 原始路由树 */
struct ptree *rib_tree[L3VPN_SIZE+1];

/* IPv6 原始路由树 */
struct ptree *ribv6_tree[L3VPN_SIZE+1];

/* IPv4 路由统计信息 */
struct rib_global rib_number = {ROUTE_NUM_MAX};

/* IPv6 路由统计信息 */
struct rib_global ribv6_number = {ROUTE_NUM_MAX};


/**
 * @brief      : 初始化 IPv4 路由树
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 9:29:45
 * @note       :
 */
int rib_tree_init(void)
{
    int vpn = 0;

    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        rib_tree[vpn] = ptree_init(IPV4_MAX_BITLEN);
        if (NULL == rib_tree[vpn])
        {
            ROUTE_LOG_ERROR("malloc\n");

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
 * @date       : 2018年2月26日 9:31:04
 * @note       : 仅释放树节点和树本身内存，用户申请内存未释放
 */
void rib_tree_delete(uint16_t vpn)
{
    if (vpn > L3VPN_SIZE)
    {
        return;
    }

    ptree_free(rib_tree[vpn]);

    return;
}


/**
 * @brief      : 删除所有 vpn 实例的路由树
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 9:34:48
 * @note       : 仅释放树节点和树本身内存，用户申请内存未释放
 */
void rib_tree_clear(void)
{
    int vpn = 0;

    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        ptree_free(rib_tree[vpn]);
    }

    return;
}


/**
 * @brief      : 释放原始路由树中 nhp 链表中的 nhp
 * @param[in ] : prib - 原始路由结构
 * @param[in ] : pnhp - 下一跳结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 9:35:31
 * @note       : 传入的 nhp 必须在原始路由树 nhp 链表中存在
 */
static void rib_nhp_free(struct rib_entry *prib, struct nhp_entry *pnhp)
{
    if ((NULL == prib) || (NULL == pnhp))
    {
        return;
    }

    listnode_delete(&prib->nhplist, pnhp);
    XFREE(MTYPE_NHP_ENTRY, pnhp);

    return;
}


/**
 * @brief      : 原始路由节点释放
 * @param[in ] : pnode - 原始路由节点
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 9:38:01
 * @note       : 传入的原始路由节点必须在原始路由树中
 */
static void rib_node_free(struct ptree_node *pnode)
{
    struct rib_entry *prib = NULL;

    if ((NULL == pnode) || (NULL == pnode->info))
    {
        return;
    }

    prib = (struct rib_entry *)pnode->info;

    pnode->info = NULL;
    pnode->lock = 0;

    ptree_node_delete(pnode);
    XFREE(MTYPE_RIB_TABLE_INFO, prib);

    return;
}


/**
 * @brief      : IPv4 活跃路由统计操作
 * @param[in ] : pnhp   - 下一跳结构
 * @param[in ] : opcode - 操作码，添加或删除
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 9:39:05
 * @note       :
 */
void rib_count_number_active(const struct nhp_entry *pnhp, uint32_t opcode)
{
    if (NULL == pnhp)
    {
        return;
    }

    if (OPCODE_ADD == opcode)
    {
        switch (pnhp->protocol)
        {
            case ROUTE_PROTO_CONNECT:
                rib_number.num_direct_active++;
                break;
            case ROUTE_PROTO_STATIC:
                rib_number.num_static_active++;
                break;
            case ROUTE_PROTO_OSPF:
                rib_number.num_ospf_active++;
                break;
            case ROUTE_PROTO_RIP:
                rib_number.num_rip_active++;
                break;
            case ROUTE_PROTO_ISIS:
                rib_number.num_isis_active++;
                break;
            case ROUTE_PROTO_IBGP:
                rib_number.num_ibgp_active++;
                break;
            case ROUTE_PROTO_EBGP:
                rib_number.num_ebgp_active++;
                break;
            case ROUTE_PROTO_LDP:
                rib_number.num_ldp_active++;
                break;
            case ROUTE_PROTO_MPLSTP:
                rib_number.num_mplstp_active++;
                break;
            case ROUTE_PROTO_RSVPTE:
                rib_number.num_rsvpte_active++;
                break;
            default:
                break;
        }
    }
    else if (OPCODE_DELETE == opcode)
    {
        switch (pnhp->protocol)
        {
            case ROUTE_PROTO_CONNECT:
                rib_number.num_direct_active--;
                break;
            case ROUTE_PROTO_STATIC:
                rib_number.num_static_active--;
                break;
            case ROUTE_PROTO_OSPF:
                rib_number.num_ospf_active--;
                break;
            case ROUTE_PROTO_RIP:
                rib_number.num_rip_active--;
                break;
            case ROUTE_PROTO_ISIS:
                rib_number.num_isis_active--;
                break;
            case ROUTE_PROTO_IBGP:
                rib_number.num_ibgp_active--;
                break;
            case ROUTE_PROTO_EBGP:
                rib_number.num_ebgp_active--;
                break;
            case ROUTE_PROTO_LDP:
                rib_number.num_ldp_active--;
                break;
            case ROUTE_PROTO_MPLSTP:
                rib_number.num_mplstp_active--;
                break;
            case ROUTE_PROTO_RSVPTE:
                rib_number.num_rsvpte_active--;
                break;
            default:
                break;
        }
    }

    return;
}


/**
 * @brief      : IPv4 原始路由统计操作
 * @param[in ] : pnhp   - 下一跳结构
 * @param[in ] : opcode - 操作码，添加或删除
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 9:40:50
 * @note       :
 */
static void rib_count_number(const struct nhp_entry *pnhp, uint32_t opcode)
{
    if (NULL == pnhp)
    {
        return;
    }

    if (OPCODE_ADD == opcode)
    {
        rib_number.num_entries++;

        if (ROUTE_STATUS_ACTIVE == pnhp->active)
        {
            rib_count_number_active(pnhp, OPCODE_ADD);
        }

        switch (pnhp->protocol)
        {
            case ROUTE_PROTO_CONNECT:
                rib_number.num_direct++;
                break;
            case ROUTE_PROTO_STATIC:
                rib_number.num_static++;
                break;
            case ROUTE_PROTO_OSPF:
                rib_number.num_ospf++;
                break;
            case ROUTE_PROTO_RIP:
                rib_number.num_rip++;
                break;
            case ROUTE_PROTO_ISIS:
                rib_number.num_isis++;
                break;
            case ROUTE_PROTO_IBGP:
                rib_number.num_ibgp++;
                break;
            case ROUTE_PROTO_EBGP:
                rib_number.num_ebgp++;
                break;
            case ROUTE_PROTO_LDP:
                rib_number.num_ldp++;
                break;
            case ROUTE_PROTO_MPLSTP:
                rib_number.num_mplstp++;
                break;
            case ROUTE_PROTO_RSVPTE:
                rib_number.num_rsvpte++;
                break;
            default :
                break;
        }
    }
    else if (OPCODE_DELETE == opcode)
    {
        rib_number.num_entries--;

        if (ROUTE_STATUS_ACTIVE == pnhp->active)
        {
            rib_count_number_active(pnhp, OPCODE_DELETE);
        }

        switch (pnhp->protocol)
        {
            case ROUTE_PROTO_CONNECT:
                rib_number.num_direct--;
                break;
            case ROUTE_PROTO_STATIC:
                rib_number.num_static--;
                break;
            case ROUTE_PROTO_OSPF:
                rib_number.num_ospf--;
                break;
            case ROUTE_PROTO_RIP:
                rib_number.num_rip--;
                break;
            case ROUTE_PROTO_ISIS:
                rib_number.num_isis--;
                break;
            case ROUTE_PROTO_IBGP:
                rib_number.num_ibgp--;
                break;
            case ROUTE_PROTO_EBGP:
                rib_number.num_ebgp--;
                break;
            case ROUTE_PROTO_LDP:
                rib_number.num_ldp--;
                break;
            case ROUTE_PROTO_MPLSTP:
                rib_number.num_mplstp--;
                break;
            case ROUTE_PROTO_RSVPTE:
                rib_number.num_rsvpte--;
                break;
            default :
                break;
        }
    }

    return;
}


/**
 * @brief      : IPv4 原始路由表中 nhp 链表节点比较
 * @param[in ] : val1 - 下一跳结构
 * @param[in ] : val2 - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 -1，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年2月26日 9:41:47
 * @note       : 先比较 distance，再比较 cost，两个值都是越小越优先
 */
static int rib_nhp_compare(void *val1, void *val2)
{
    struct nhp_entry *pnhp_new = (struct nhp_entry *)val1;
    struct nhp_entry *pnhp_old = (struct nhp_entry *)val2;

    if ((NULL == val1) || (NULL == val2))
    {
        return 0;
    }

    if (pnhp_new->distance < pnhp_old->distance)
    {
        return -1;
    }
    else if (pnhp_new->distance == pnhp_old->distance)
    {
        if (pnhp_new->cost < pnhp_old->cost)
        {
            return -1;
        }
    }

    return 0;
}


/**
 * @brief      : 向原始路由表的 nhp 链表中添加 nhp
 * @param[in ] : prib - 原始路由结构
 * @param[in ] : pnhp - 下一跳结构
 * @param[out] :
 * @return     : 成功返回加入到 nhp 链表中的 nhp，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 9:46:54
 * @note       :
 */
static struct nhp_entry *rib_add_nhp(struct rib_entry *prib, struct nhp_entry *pnhp)
{
    struct nhp_entry *pnhp_new = NULL;

    ROUTE_LOG_DEBUG();

    if ((NULL == prib) || (NULL == pnhp))
    {
        return NULL;
    }

    pnhp_new = (struct nhp_entry *)XCALLOC(MTYPE_NHP_ENTRY, sizeof(struct nhp_entry));
    if (NULL == pnhp_new)
    {
        ROUTE_LOG_ERROR("malloc\n");

        return NULL;
    }

    memcpy(pnhp_new, pnhp, sizeof(struct nhp_entry));
    listnode_add_sort(&prib->nhplist, pnhp_new);

    return pnhp_new;
}


/**
 * @brief      : 查找原始路由表 nhp 链表中的 nhp
 * @param[in ] : nhplist - 原始路由表 nhp 链表
 * @param[in ] : pnhp    - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 nhp 链表中节点，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 9:49:00
 * @note       : 查找成功意味着下一跳地址、vpn、协议类型完全相同
 */
struct nhp_entry *rib_lookup_nhp(struct list *nhplist, struct nhp_entry *pnhp)
{
    struct listnode  *pnode     = NULL;
    struct nhp_entry *pnhp_loop = NULL;
    struct nhp_entry *pnhp_dest = NULL;

    ROUTE_LOG_DEBUG();

    if ((NULL == nhplist) || (NULL == pnhp))
    {
        return NULL;
    }

    for (ALL_LIST_ELEMENTS_RO(nhplist, pnode, pnhp_loop))
    {
        if ((pnhp_loop->nexthop.addr.ipv4 == pnhp->nexthop.addr.ipv4)
            && (pnhp_loop->vpn == pnhp->vpn)
            && (pnhp_loop->protocol == pnhp->protocol))
        {
            pnhp_dest = pnhp_loop;

            break;
        }
    }

    return pnhp_dest;
}


/**
 * @brief      : 保证直连路由必须下发
 * @param[in ] : prib - 原始路由结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 9:52:08
 * @note       : 添加直连路由失败时调用此函数，随机删除其他协议类型的路由，
 * @node       : 保证直连路由下发成功
 */
static void rib_add_connect(struct rib_entry *prib)
{
    struct listnode   *plistnode = NULL;
    struct ptree_node *pnode     = NULL;
    struct rib_entry  *prib_loop = NULL;
    struct nhp_entry  *pnhp_loop = NULL;
    int delete_flag = 0;
    int vpn         = 0;
    int ret         = ERRNO_SUCCESS;

    ROUTE_LOG_DEBUG();

    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        PTREE_LOOP (rib_tree[vpn], prib_loop, pnode)
        {
            for (ALL_LIST_ELEMENTS_RO(&prib_loop->nhplist, plistnode, pnhp_loop))
            {
                if ((pnhp_loop->active == ROUTE_STATUS_ACTIVE)
                    && ((pnhp_loop->protocol == ROUTE_PROTO_EBGP)
                        || (pnhp_loop->protocol == ROUTE_PROTO_IBGP)
                        || (pnhp_loop->protocol == ROUTE_PROTO_OSPF)
                        || (pnhp_loop->protocol == ROUTE_PROTO_ISIS)
                        || (pnhp_loop->protocol == ROUTE_PROTO_RIP)
                        || (pnhp_loop->protocol == ROUTE_PROTO_LDP)
                        || (pnhp_loop->protocol == ROUTE_PROTO_MPLSTP)
                        || (pnhp_loop->protocol == ROUTE_PROTO_RSVPTE)))
                {
                    delete_flag       = 1;
                    pnhp_loop->active = ROUTE_STATUS_INACTIVE;

                    rib_count_number_active(pnhp_loop, OPCODE_DELETE);
                }
            }

            if (1 == delete_flag)
            {
                route_delete(&prib_loop->prefix, prib_loop->vpn);

                ret = route_add(prib);
                if (ERRNO_SUCCESS != ret)
                {
                    rib_add_connect(prib);
                }

                return;
            }
        }
    }

    return;
}


/**
 * @brief      : IPv4 原始路由添加
 * @param[in ] : prefix - 路由前缀
 * @param[in ] : vpn    - vpn 实例
 * @param[in ] : pnhp   - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 9:55:32
 * @note       :
 */
int rib_add(struct inet_prefix *prefix, uint16_t vpn, struct nhp_entry *pnhp)
{
    struct ptree_node  *pnode    = NULL;
    struct rib_entry   *prib     = NULL;
    struct nhp_entry   *pnhp_new = NULL;
    struct route_entry *proute   = NULL;
    int ret;

    if ((prefix == NULL) || (pnhp == NULL) || (vpn > L3VPN_SIZE))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    ROUTE_LOG_DEBUG("ip:%x, mask:%d, vpn:%d, nexthop:%x, ifindex:%x.\n",
                    prefix->addr.ipv4, prefix->prefixlen, vpn, pnhp->nexthop.addr.ipv4,
                    pnhp->ifindex);

    if (rib_number.num_entries == RIB_NUM_MAX)
    {
        return ERRNO_OVERSIZE;
    }

    pnode = ptree_node_get(rib_tree[vpn], (u_char *)&prefix->addr.ipv4, prefix->prefixlen);
    if (NULL == pnode)
    {
        ROUTE_LOG_ERROR("get tree\n");

        return ERRNO_MALLOC;
    }

    /* 如果路由信息不存在则创建，如果存在则新增 nhp */
    if (pnode->info == NULL)
    {
        ROUTE_LOG_DEBUG("add rib\n");

        prib = (struct rib_entry *)XCALLOC(MTYPE_RIB_TABLE_INFO, sizeof(struct rib_entry));
        if (NULL == prib)
        {
            ROUTE_LOG_ERROR("malloc rib\n");

            return ERRNO_MALLOC;
        }

        memcpy(&prib->prefix, prefix, sizeof(struct inet_prefix));
        prib->vpn         = vpn;
        prib->nhplist.cmp = rib_nhp_compare;

        pnhp_new = rib_add_nhp(prib, pnhp);
        if (NULL == pnhp_new)
        {
            return ERRNO_MALLOC;
        }

        pnode->info = (void *)prib;
    }
    else
    {
        ROUTE_LOG_DEBUG("add nhp\n");

        prib = (struct rib_entry *)pnode->info;
        if (NULL != rib_lookup_nhp(&prib->nhplist, pnhp))
        {
            return ERRNO_EXISTED_NHP;
        }

        pnhp_new = rib_add_nhp(prib, pnhp);
        if (NULL == pnhp_new)
        {
            return ERRNO_MALLOC;
        }
    }

    /* 如果活跃路由存在则更新，如果不存在则添加 */
    proute = route_lookup(&prib->prefix, prib->vpn);
    if (NULL == proute)
    {
        ret = route_add(prib);
        if ((ERRNO_OVERSIZE != ret) && (ERRNO_SUCCESS != ret))
        {
            rib_nhp_free(prib, pnhp_new);

            if (list_isempty(&prib->nhplist))
            {
                rib_node_free(pnode);
            }

            return ret;
        }
        else if ((ERRNO_OVERSIZE == ret) && (pnhp->protocol == ROUTE_PROTO_CONNECT))
        {
            rib_add_connect(prib);
        }
    }
    else
    {
        ret = route_update(prib);
        if ((ERRNO_OVERSIZE != ret) && (ERRNO_SUCCESS != ret))
        {
            rib_nhp_free(prib, pnhp_new);

            if (list_isempty(&prib->nhplist))
            {
                rib_node_free(pnode);
            }

            return ret;
        }
    }

    rib_count_number(pnhp, OPCODE_ADD);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv4 原始路由更新
 * @param[in ] : prefix   - 路由前缀
 * @param[in ] : vpn      - vpn 实例
 * @param[in ] : pnhp_new - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:00:38
 * @note       :
 */
int rib_update(struct inet_prefix *prefix, uint16_t vpn, struct nhp_entry *pnhp_new)
{
    struct nhp_entry   *pnhp   = NULL;
    struct rib_entry   *prib   = NULL;
    struct route_entry *proute = NULL;
    uint8_t  distance;
    uint32_t cost;
    int      ret;

    ROUTE_LOG_DEBUG();

    if ((prefix == NULL) || (pnhp_new == NULL) || (vpn > L3VPN_SIZE))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    /* 如果更新的原始路由不存在则添加 */
    prib = rib_lookup(prefix, vpn);
    if (NULL == prib)
    {
        ROUTE_LOG_DEBUG("update rib not exist and add it.\n");

        return rib_add(prefix, vpn, pnhp_new);
    }

    pnhp = rib_lookup_nhp(&prib->nhplist, pnhp_new);
    if (pnhp == NULL)
    {
        ROUTE_LOG_DEBUG("update nhp not exist and add it.\n");

        return rib_add(prefix, vpn, pnhp_new);
    }

    /* 更新路由 nhp 参数 */
    distance        = pnhp->distance;
    cost            = pnhp->cost;
    pnhp->action    = pnhp_new->action;
    pnhp->ifindex   = pnhp_new->ifindex;
    pnhp->distance  = pnhp_new->distance;
    pnhp->nhp_type  = pnhp_new->nhp_type;
    pnhp->cost      = pnhp_new->cost;
    pnhp->down_flag = LINK_DOWN;
    pnhp->nexthop_connect.addr.ipv4 = 0;

    /* 如果 distance 或 cost 更新，则修改 nhp 在 rib 链表中的顺序 */
    if ((distance != pnhp->distance) || (cost != pnhp->cost))
    {
        listnode_delete(&prib->nhplist, (void *)pnhp);
        listnode_add_sort(&prib->nhplist, (void *)pnhp);
    }

    /* 如果活跃路由存在则更新，不存在则添加 */
    proute = route_lookup(&prib->prefix, prib->vpn);
    if (NULL == proute)
    {
        ret = route_add(prib);
        if ((ERRNO_OVERSIZE != ret) && (ERRNO_SUCCESS != ret))
        {
            return ret;
        }
    }
    else
    {
        ret = route_update(prib);
        if ((ERRNO_OVERSIZE != ret) && (ERRNO_SUCCESS != ret))
        {
            return ret;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv4 原始路由删除
 * @param[in ] : prefix - 路由前缀
 * @param[in ] : vpn    - vpn 实例
 * @param[in ] : pnhp   - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:05:14
 * @note       :
 */
int rib_delete(struct inet_prefix *prefix, uint16_t vpn, struct nhp_entry *pnhp)
{
    struct ptree_node *pnode    = NULL;
    struct rib_entry  *prib     = NULL;
    struct nhp_entry  *pnhp_del = NULL;
    struct nhp_entry   nhp_del;

    memset(&nhp_del, 0, sizeof(struct nhp_entry));

    if ((prefix == NULL) || (pnhp == NULL) || (vpn > L3VPN_SIZE))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    ROUTE_LOG_DEBUG("ip:%x, mask:%d, vpn:%d, nexthop:%x, ifindex:%x.\n",
                        prefix->addr.ipv4, prefix->prefixlen, vpn,
                        pnhp->nexthop.addr.ipv4, pnhp->ifindex);

    pnode = ptree_node_lookup(rib_tree[vpn], (u_char *)&prefix->addr.ipv4, prefix->prefixlen);
    if ((NULL == pnode) || (NULL == pnode->info))
    {
        return ERRNO_SUCCESS;
    }

    prib = (struct rib_entry *)pnode->info;

    pnhp_del = rib_lookup_nhp(&prib->nhplist, pnhp);
    if (pnhp_del != NULL)
    {
        memcpy(&nhp_del, pnhp_del, sizeof(struct nhp_entry));

        rib_count_number(pnhp_del, OPCODE_DELETE);
        rib_nhp_free(prib, pnhp_del);

        if (nhp_del.active == ROUTE_STATUS_ACTIVE)
        {
            if (list_isempty(&prib->nhplist))
            {
                route_delete(&prib->prefix, prib->vpn);
            }
            else
            {
                /* 删除直连路由先将路由从激活路由表移除，有些情况会影响迭代 */
                if (nhp_del.protocol == ROUTE_PROTO_CONNECT)
                {
                    route_delete(&prib->prefix, prib->vpn);
                    if (NULL == route_lookup(&prib->prefix, prib->vpn))
                    {
                        route_add(prib);
                    }
                }
                else
                {
                    route_update(prib);
                }
            }
        }
    }

    if (list_isempty(&prib->nhplist))
    {
        rib_node_free(pnode);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv4 原始路由查找
 * @param[in ] : prefix - 路由前缀
 * @param[in ] : vpn    - vpn 实例
 * @param[out] :
 * @return     : 成功返回原始路由结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:10:15
 * @note       :
 */
struct rib_entry *rib_lookup(struct inet_prefix *prefix, uint16_t vpn)
{
    struct ptree_node *pnode = NULL;

    ROUTE_LOG_DEBUG();

    if ((NULL == prefix) || (vpn > L3VPN_SIZE))
    {
        return NULL;
    }

    pnode = ptree_node_lookup(rib_tree[vpn], (u_char *)&prefix->addr.ipv4,
                                prefix->prefixlen);
    if (NULL == pnode)
    {
        return NULL;
    }

    return (struct rib_entry *)pnode->info;
}


/**
 * @brief      : 初始化 IPv6 路由树
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:11:17
 * @note       :
 */
int ribv6_tree_init(void)
{
    int vpn = 0;

    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        ribv6_tree[vpn] = ptree_init(IPV6_MAX_BITLEN);
        if (NULL == ribv6_tree[vpn])
        {
            ROUTE_LOG_ERROR("malloc\n");
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
 * @date       : 2018年2月26日 10:11:57
 * @note       : 仅释放树节点和树本身内存，用户申请内存未释放
 */
void ribv6_tree_delete(uint16_t vpn)
{
    if (vpn > L3VPN_SIZE)
    {
        return;
    }

    ptree_free(ribv6_tree[vpn]);

    return;
}


/**
 * @brief      : 删除所有 vpn 实例的路由树
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:12:40
 * @note       : 仅释放树节点和树本身内存，用户申请内存未释放
 */
void ribv6_tree_clear(void)
{
    int vpn = 0;

    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        ptree_free(ribv6_tree[vpn]);
    }

    return;
}


/**
 * @brief      : 释放原始路由树中 nhp 链表中的 nhp
 * @param[in ] : prib - 原始路由结构
 * @param[in ] : pnhp - 下一跳结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:13:07
 * @note       : 传入的 nhp 必须在原始路由树 nhp 链表中存在
 */
static void ribv6_nhp_free(struct rib_entry *prib, struct nhp_entry *pnhp)
{
    if ((NULL == prib) || (NULL == pnhp))
    {
        return;
    }

    listnode_delete(&prib->nhplist, pnhp);
    XFREE(MTYPE_NHP_ENTRY, pnhp);

    return;
}


/**
 * @brief      : 原始路由节点释放
 * @param[in ] : pnode - 原始路由节点
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:13:43
 * @note       : 传入的原始路由节点必须在原始路由树中
 */
static void ribv6_node_free(struct ptree_node *pnode)
{
    struct rib_entry *prib = NULL;

    if ((NULL == pnode) || (NULL == pnode->info))
    {
        return;
    }

    prib = (struct rib_entry *)pnode->info;

    pnode->info = NULL;
    pnode->lock = 0;

    ptree_node_delete(pnode);
    XFREE(MTYPE_RIB_TABLE_INFO, prib);

    return;
}


/**
 * @brief      : IPv6 活跃路由统计操作
 * @param[in ] : pnhp   - 下一跳结构
 * @param[in ] : opcode - 操作码，添加或删除
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:14:39
 * @note       :
 */
void ribv6_count_number_active(const struct nhp_entry *pnhp, uint32_t opcode)
{
    if (NULL == pnhp)
    {
        return;
    }

    if (OPCODE_ADD == opcode)
    {
        switch (pnhp->protocol)
        {
            case ROUTE_PROTO_CONNECT:
                ribv6_number.num_direct_active++;
                break;
            case ROUTE_PROTO_STATIC:
                ribv6_number.num_static_active++;
                break;
            case ROUTE_PROTO_OSPF:
            case ROUTE_PROTO_OSPF6:
                ribv6_number.num_ospf_active++;
                break;
            case ROUTE_PROTO_RIP:
            case ROUTE_PROTO_RIPNG:
                ribv6_number.num_rip_active++;
                break;
            case ROUTE_PROTO_ISIS:
            case ROUTE_PROTO_ISIS6:
                ribv6_number.num_isis_active++;
                break;
            case ROUTE_PROTO_IBGP:
            case ROUTE_PROTO_IBGP6:
                ribv6_number.num_ibgp_active++;
                break;
            case ROUTE_PROTO_EBGP:
            case ROUTE_PROTO_EBGP6:
                ribv6_number.num_ebgp_active++;
                break;
            case ROUTE_PROTO_LDP:
                ribv6_number.num_ldp_active++;
                break;
            case ROUTE_PROTO_MPLSTP:
                ribv6_number.num_mplstp_active++;
                break;
            case ROUTE_PROTO_RSVPTE:
                ribv6_number.num_rsvpte_active++;
                break;
            default :
                break;
        }
    }
    else if (OPCODE_DELETE == opcode)
    {
        switch (pnhp->protocol)
        {
            case ROUTE_PROTO_CONNECT:
                ribv6_number.num_direct_active--;
                break;
            case ROUTE_PROTO_STATIC:
                ribv6_number.num_static_active--;
                break;
            case ROUTE_PROTO_OSPF:
            case ROUTE_PROTO_OSPF6:
                ribv6_number.num_ospf_active--;
                break;
            case ROUTE_PROTO_RIP:
            case ROUTE_PROTO_RIPNG:
                ribv6_number.num_rip_active--;
                break;
            case ROUTE_PROTO_ISIS:
            case ROUTE_PROTO_ISIS6:
                ribv6_number.num_isis_active--;
                break;
            case ROUTE_PROTO_IBGP:
            case ROUTE_PROTO_IBGP6:
                ribv6_number.num_ibgp_active--;
                break;
            case ROUTE_PROTO_EBGP:
            case ROUTE_PROTO_EBGP6:
                ribv6_number.num_ebgp_active--;
                break;
            case ROUTE_PROTO_LDP:
                ribv6_number.num_ldp_active--;
                break;
            case ROUTE_PROTO_MPLSTP:
                ribv6_number.num_mplstp_active--;
                break;
            case ROUTE_PROTO_RSVPTE:
                ribv6_number.num_rsvpte_active--;
                break;
            default :
                break;
        }
    }

    return;
}


/**
 * @brief      : IPv6 原始路由统计操作
 * @param[in ] : pnhp   - 下一跳结构
 * @param[in ] : opcode - 操作码，添加或删除
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:16:02
 * @note       :
 */
static void ribv6_count_number(const struct nhp_entry *pnhp, uint32_t opcode)
{
    if (NULL == pnhp)
    {
        return;
    }

    if (OPCODE_ADD == opcode)
    {
        ribv6_number.num_entries++;

        if (ROUTE_STATUS_ACTIVE == pnhp->active)
        {
            ribv6_count_number_active(pnhp, OPCODE_ADD);
        }

        switch (pnhp->protocol)
        {
            case ROUTE_PROTO_CONNECT:
                ribv6_number.num_direct++;
                break;
            case ROUTE_PROTO_STATIC:
                ribv6_number.num_static++;
                break;
            case ROUTE_PROTO_OSPF:
            case ROUTE_PROTO_OSPF6:
                ribv6_number.num_ospf++;
                break;
            case ROUTE_PROTO_RIP:
            case ROUTE_PROTO_RIPNG:
                ribv6_number.num_rip++;
                break;
            case ROUTE_PROTO_ISIS:
            case ROUTE_PROTO_ISIS6:
                ribv6_number.num_isis++;
                break;
            case ROUTE_PROTO_IBGP:
            case ROUTE_PROTO_IBGP6:
                ribv6_number.num_ibgp++;
                break;
            case ROUTE_PROTO_EBGP:
            case ROUTE_PROTO_EBGP6:
                ribv6_number.num_ebgp++;
                break;
            case ROUTE_PROTO_LDP:
                ribv6_number.num_ldp++;
                break;
            case ROUTE_PROTO_MPLSTP:
                ribv6_number.num_mplstp++;
                break;
            case ROUTE_PROTO_RSVPTE:
                ribv6_number.num_rsvpte++;
                break;
            default :
                break;
        }
    }
    else if (OPCODE_DELETE == opcode)
    {
        ribv6_number.num_entries--;

        if (ROUTE_STATUS_ACTIVE == pnhp->active)
        {
            ribv6_count_number_active(pnhp, OPCODE_DELETE);
        }

        switch (pnhp->protocol)
        {
            case ROUTE_PROTO_CONNECT:
                ribv6_number.num_direct--;
                break;
            case ROUTE_PROTO_STATIC:
                ribv6_number.num_static--;
                break;
            case ROUTE_PROTO_OSPF:
            case ROUTE_PROTO_OSPF6:
                ribv6_number.num_ospf--;
                break;
            case ROUTE_PROTO_RIP:
            case ROUTE_PROTO_RIPNG:
                ribv6_number.num_rip--;
                break;
            case ROUTE_PROTO_ISIS:
            case ROUTE_PROTO_ISIS6:
                ribv6_number.num_isis--;
                break;
            case ROUTE_PROTO_IBGP:
            case ROUTE_PROTO_IBGP6:
                ribv6_number.num_ibgp--;
                break;
            case ROUTE_PROTO_EBGP:
            case ROUTE_PROTO_EBGP6:
                ribv6_number.num_ebgp--;
                break;
            case ROUTE_PROTO_LDP:
                ribv6_number.num_ldp--;
                break;
            case ROUTE_PROTO_MPLSTP:
                ribv6_number.num_mplstp--;
                break;
            case ROUTE_PROTO_RSVPTE:
                ribv6_number.num_rsvpte--;
                break;
            default :
                break;
        }
    }

    return;
}


/**
 * @brief      : IPv6 原始路由表中 nhp 链表节点比较
 * @param[in ] : val1 - 下一跳结构
 * @param[in ] : val2 - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 -1，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:16:41
 * @note       : 先比较 distance，再比较 cost，两个值都是越小越优先
 */
static int ribv6_nhp_compare(void *val1, void *val2)
{
    struct nhp_entry *pnhp_new = (struct nhp_entry *)val1;
    struct nhp_entry *pnhp_old = (struct nhp_entry *)val2;

    if ((NULL == val1) || (NULL == val2))
    {
        return 0;
    }

    if (pnhp_new->distance < pnhp_old->distance)
    {
        return -1;
    }
    else if (pnhp_new->distance == pnhp_old->distance)
    {
        if (pnhp_new->cost < pnhp_old->cost)
        {
            return -1;
        }
    }

    return 0;
}


/**
 * @brief      : 向原始路由表的 nhp 链表中添加 nhp
 * @param[in ] : prib - 原始路由结构
 * @param[in ] : pnhp - 下一跳结构
 * @param[out] :
 * @return     : 成功返回加入到 nhp 链表中的 nhp，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:17:58
 * @note       :
 */
static struct nhp_entry *ribv6_add_nhp(struct rib_entry *prib, struct nhp_entry *pnhp)
{
    struct nhp_entry *pnhp_new = NULL;

    ROUTE_LOG_DEBUG();

    if ((NULL == prib) || (NULL == pnhp))
    {
        return NULL;
    }

    pnhp_new = (struct nhp_entry *)XCALLOC(MTYPE_NHP_ENTRY, sizeof(struct nhp_entry));
    if (NULL == pnhp_new)
    {
        ROUTE_LOG_ERROR("malloc\n");

        return NULL;
    }

    memcpy(pnhp_new, pnhp, sizeof(struct nhp_entry));
    listnode_add_sort(&prib->nhplist, pnhp_new);

    return pnhp_new;
}


/**
 * @brief      : 查找原始路由表 nhp 链表中的 nhp
 * @param[in ] : nhplist - 原始路由表 nhp 链表
 * @param[in ] : pnhp    - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 nhp 链表中节点，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:18:38
 * @note       : 查找成功意味着下一跳地址、vpn、协议类型完全相同
 */
struct nhp_entry *ribv6_lookup_nhp(struct list *nhplist, struct nhp_entry *pnhp)
{
    struct listnode  *pnode     = NULL;
    struct nhp_entry *pnhp_loop = NULL;
    struct nhp_entry *pnhp_dest = NULL;

    ROUTE_LOG_DEBUG();

    if ((NULL == nhplist) || (NULL == pnhp))
    {
        return NULL;
    }

    for (ALL_LIST_ELEMENTS_RO(nhplist, pnode, pnhp_loop))
    {
        if ((0 == memcmp(&pnhp_loop->nexthop, &pnhp->nexthop, sizeof(struct inet_addr)))
            && (pnhp_loop->vpn == pnhp->vpn)
            && (pnhp_loop->protocol == pnhp->protocol))
        {
            pnhp_dest = pnhp_loop;

            break;
        }
    }

    return pnhp_dest;
}


/**
 * @brief      : 保证直连路由必须下发
 * @param[in ] : prib - 原始路由结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:22:58
 * @note       : 添加直连路由失败时调用此函数，随机删除其他协议类型的路由，
 * @node       : 保证直连路由下发成功
 */
static void ribv6_add_connect(struct rib_entry *prib)
{
    struct listnode   *plistnode = NULL;
    struct ptree_node *pnode     = NULL;
    struct rib_entry  *prib_loop = NULL;
    struct nhp_entry  *pnhp_loop = NULL;
    int delete_flag = 0;
    int vpn         = 0;
    int ret         = ERRNO_SUCCESS;

    ROUTE_LOG_DEBUG();

    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        PTREE_LOOP (ribv6_tree[vpn], prib_loop, pnode)
        {
            for (ALL_LIST_ELEMENTS_RO(&prib_loop->nhplist, plistnode, pnhp_loop))
            {
                if ((pnhp_loop->active == ROUTE_STATUS_ACTIVE)
                    && ((pnhp_loop->protocol == ROUTE_PROTO_EBGP6)
                         || (pnhp_loop->protocol == ROUTE_PROTO_IBGP6)
                         || (pnhp_loop->protocol == ROUTE_PROTO_OSPF6)
                         || (pnhp_loop->protocol == ROUTE_PROTO_ISIS6)
                         || (pnhp_loop->protocol == ROUTE_PROTO_RIPNG)
                         || (pnhp_loop->protocol == ROUTE_PROTO_LDP)
                         || (pnhp_loop->protocol == ROUTE_PROTO_MPLSTP)
                         || (pnhp_loop->protocol == ROUTE_PROTO_RSVPTE)))
                {
                    delete_flag       = 1;
                    pnhp_loop->active = ROUTE_STATUS_INACTIVE;

                    ribv6_count_number_active(pnhp_loop, OPCODE_DELETE);
                }
            }

            if (1 == delete_flag)
            {
                routev6_delete(&prib_loop->prefix, prib_loop->vpn);

                ret = routev6_add(prib);
                if (ERRNO_SUCCESS != ret)
                {
                    ribv6_add_connect(prib);
                }

                return;
            }
        }
    }

    return;
}


/**
 * @brief      : IPv6 原始路由添加
 * @param[in ] : prefix - 路由前缀
 * @param[in ] : vpn    - vpn 实例
 * @param[in ] : pnhp   - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:25:27
 * @note       :
 */
int ribv6_add(struct inet_prefix *prefix, uint16_t vpn, struct nhp_entry *pnhp)
{
    struct ptree_node  *pnode    = NULL;
    struct rib_entry   *prib     = NULL;
    struct nhp_entry   *pnhp_new = NULL;
    struct route_entry *proute   = NULL;
    int                 ret;

    if ((prefix == NULL) || (pnhp == NULL) || (vpn > L3VPN_SIZE))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    ROUTE_LOG_DEBUG("ip:%x, mask:%d, vpn:%d, nexthop:%x, ifindex:%x.\n",
                    prefix->addr.ipv4, prefix->prefixlen, vpn, pnhp->nexthop.addr.ipv4,
                    pnhp->ifindex);

    if (ribv6_number.num_entries == RIBV6_NUM_MAX)
    {
        return ERRNO_OVERSIZE;
    }

    pnode = ptree_node_get(ribv6_tree[vpn], (u_char *)prefix->addr.ipv6, prefix->prefixlen);
    if (NULL == pnode)
    {
        ROUTE_LOG_ERROR("get tree\n");

        return ERRNO_MALLOC;
    }

    /* 如果路由信息不存在则创建，如果存在则新增 nhp */
    if (pnode->info == NULL)
    {
        ROUTE_LOG_DEBUG("add rib\n");

        prib = (struct rib_entry *)XCALLOC(MTYPE_RIB_TABLE_INFO, sizeof(struct rib_entry));
        if (NULL == prib)
        {
            ROUTE_LOG_ERROR("malloc rib\n");

            return ERRNO_MALLOC;
        }

        memcpy(&prib->prefix, prefix, sizeof(struct inet_prefix));
        prib->vpn         = vpn;
        prib->nhplist.cmp = ribv6_nhp_compare;

        pnhp_new = ribv6_add_nhp(prib, pnhp);
        if (NULL == pnhp_new)
        {
            return ERRNO_MALLOC;
        }

        pnode->info = (void *)prib;
    }
    else
    {
        ROUTE_LOG_DEBUG("add nhp\n");

        prib = (struct rib_entry *)pnode->info;
        if (NULL != ribv6_lookup_nhp(&prib->nhplist, pnhp))
        {
            return ERRNO_EXISTED_NHP;
        }

        pnhp_new = ribv6_add_nhp(prib, pnhp);
        if (NULL == pnhp_new)
        {
            return ERRNO_MALLOC;
        }
    }

    /* 如果活跃路由存在则更新，如果不存在则添加 */
    proute = routev6_lookup(&prib->prefix, prib->vpn);
    if (NULL == proute)
    {
        ret = routev6_add(prib);
        if ((ERRNO_OVERSIZE != ret) && (ERRNO_SUCCESS != ret))
        {
            ribv6_nhp_free(prib, pnhp_new);

            if (list_isempty(&prib->nhplist))
            {
                rib_node_free(pnode);
            }

            return ret;
        }
        else if ((ERRNO_OVERSIZE == ret) && (pnhp->protocol == ROUTE_PROTO_CONNECT))
        {
            ribv6_add_connect(prib);
        }
    }
    else
    {
        ret = routev6_update(prib);
        if ((ERRNO_OVERSIZE != ret) && (ERRNO_SUCCESS != ret))
        {
            ribv6_nhp_free(prib, pnhp_new);

            if (list_isempty(&prib->nhplist))
            {
                rib_node_free(pnode);
            }

            return ret;
        }
    }

    ribv6_count_number(pnhp, OPCODE_ADD);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv6 原始路由更新
 * @param[in ] : prefix   - 路由前缀
 * @param[in ] : vpn      - vpn 实例
 * @param[in ] : pnhp_new - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:28:41
 * @note       :
 */
int ribv6_update(struct inet_prefix *prefix, uint16_t vpn, struct nhp_entry *pnhp_new)
{
    struct nhp_entry   *pnhp   = NULL;
    struct rib_entry   *prib   = NULL;
    struct route_entry *proute = NULL;
    uint8_t  distance;
    uint32_t cost;
    int      ret;

    ROUTE_LOG_DEBUG();

    if ((prefix == NULL) || (pnhp_new == NULL) || (vpn > L3VPN_SIZE))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    prib = ribv6_lookup(prefix, vpn);
    if (NULL == prib)
    {
        ROUTE_LOG_DEBUG("update rib not exist and add it.\n");

        return ribv6_add(prefix, vpn, pnhp_new);
    }

    /* 如果更新的原始路由不存在则添加 */
    pnhp = ribv6_lookup_nhp(&prib->nhplist, pnhp_new);
    if (pnhp == NULL)
    {
        ROUTE_LOG_DEBUG("update nhp not exist and add it.\n");

        return ribv6_add(prefix, vpn, pnhp_new);
    }

    /* 更新路由 nhp 参数 */
    distance        = pnhp->distance;
    cost            = pnhp->cost;
    pnhp->action    = pnhp_new->action;
    pnhp->ifindex   = pnhp_new->ifindex;
    pnhp->distance  = pnhp_new->distance;
    pnhp->nhp_type  = pnhp_new->nhp_type;
    pnhp->cost      = pnhp_new->cost;
    pnhp->down_flag = LINK_DOWN;
    memset(&pnhp->nexthop_connect, 0, sizeof(struct inet_addr));

    /* 如果 distance 或 cost 更新，则修改 nhp 在 rib 链表中的顺序 */
    if ((distance != pnhp->distance) || (cost != pnhp->cost))
    {
        listnode_delete(&prib->nhplist, (void *)pnhp);
        listnode_add_sort(&prib->nhplist, (void *)pnhp);
    }

    /* 如果活跃路由存在则更新，不存在则添加 */
    proute = routev6_lookup(&prib->prefix, prib->vpn);
    if (NULL == proute)
    {
        ret = routev6_add(prib);
        if ((ERRNO_OVERSIZE != ret) && (ERRNO_SUCCESS != ret))
        {
            return ret;
        }
    }
    else
    {
        ret = routev6_update(prib);
        if ((ERRNO_OVERSIZE != ret) && (ERRNO_SUCCESS != ret))
        {
            return ret;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv6 原始路由删除
 * @param[in ] : prefix - 路由前缀
 * @param[in ] : vpn    - vpn 实例
 * @param[in ] : pnhp   - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:30:52
 * @note       :
 */
int ribv6_delete(struct inet_prefix *prefix, uint16_t vpn, struct nhp_entry *pnhp)
{
    struct ptree_node *pnode    = NULL;
    struct rib_entry  *prib     = NULL;
    struct nhp_entry  *pnhp_del = NULL;
    struct nhp_entry   nhp_del;

    memset(&nhp_del, 0, sizeof(struct nhp_entry));

    if ((prefix == NULL) || (pnhp == NULL) || (vpn > L3VPN_SIZE))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    ROUTE_LOG_DEBUG("ip:%x, mask:%d, vpn:%d, nexthop:%x, ifindex:%x.\n",
                    prefix->addr.ipv4, prefix->prefixlen, vpn,
                    pnhp->nexthop.addr.ipv4, pnhp->ifindex);

    pnode = ptree_node_lookup(ribv6_tree[vpn], (u_char *)prefix->addr.ipv6, prefix->prefixlen);
    if ((NULL == pnode) || (NULL == pnode->info))
    {
        return ERRNO_SUCCESS;
    }

    prib = (struct rib_entry *)pnode->info;

    pnhp_del = ribv6_lookup_nhp(&prib->nhplist, pnhp);
    if (pnhp_del != NULL)
    {
        memcpy(&nhp_del, pnhp_del, sizeof(struct nhp_entry));

        ribv6_count_number(pnhp_del, OPCODE_DELETE);
        ribv6_nhp_free(prib, pnhp_del);

        if (nhp_del.active == ROUTE_STATUS_ACTIVE)
        {
            if (list_isempty(&prib->nhplist))
            {
                routev6_delete(&prib->prefix, prib->vpn);
            }
            else
            {
                /* 删除直连路由先将路由从激活路由表移除，有些情况会影响迭代 */
                if (nhp_del.protocol == ROUTE_PROTO_CONNECT)
                {
                    routev6_delete(&prib->prefix, prib->vpn);

                    if (NULL == routev6_lookup(&prib->prefix, prib->vpn))
                    {
                        routev6_add(prib);
                    }
                }
                else
                {
                    routev6_update(prib);
                }
            }
        }
    }

    if (list_isempty(&prib->nhplist))
    {
        ribv6_node_free(pnode);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : IPv6 原始路由查找
 * @param[in ] : prefix - 路由前缀
 * @param[in ] : vpn    - vpn 实例
 * @param[out] :
 * @return     : 成功返回原始路由结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 10:32:00
 * @note       :
 */
struct rib_entry *ribv6_lookup(struct inet_prefix *prefix, uint16_t vpn)
{
    struct ptree_node *pnode = NULL;

    ROUTE_LOG_DEBUG();

    if ((NULL == prefix) || (vpn > L3VPN_SIZE))
    {
        return NULL;
    }

    pnode = ptree_node_lookup(ribv6_tree[vpn], (u_char *)prefix->addr.ipv6, prefix->prefixlen);
    if (NULL == pnode)
    {
        return NULL;
    }

    return (struct rib_entry *)pnode->info;
}


/* h3c added */

int route_rib_get_bulk_for_mib(struct route_rib_mib *pindex, struct route_rib_mib *pdata, uint32_t index_flag)
{
    struct ptree_node   *pnode          = NULL;
    struct ptree_node   *pnodenext      = NULL;
    struct listnode     *pnode_list     = NULL;
    struct listnode     *pnextnode_list = NULL;
    struct rib_entry *prib = NULL;
    struct nhp_entry *pnhp_lookup = NULL;
    uint32_t msg_num  = IPC_MSG_LEN / sizeof(struct route_rib_mib);
    uint32_t vpn = 0;
    uint32_t data_num = 0;
    uint32_t flag = 0;

    if(1 == index_flag)
    {
        PTREE_LOOP(rib_tree[vpn], prib, pnode)
        {
            if (NULL != prib)
            {
                for (ALL_LIST_ELEMENTS(&prib->nhplist, pnode_list, pnextnode_list, pnhp_lookup))
                {
                    if(pnhp_lookup)
                    {
                        memcpy(&pdata[data_num].prefix, &prib->prefix, sizeof(struct inet_prefix));
                        memcpy(&pdata[data_num].nhp, pnhp_lookup, sizeof(struct nhp_entry));
                        pdata[data_num].vpn = prib->vpn;

                        if(++data_num == msg_num)
                        {
                            return data_num;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
    else
    {
        pnode = ptree_node_lookup(rib_tree[vpn], (u_char *)&pindex->prefix.addr.ipv4, pindex->prefix.prefixlen);

        while(pnode)
        {
            pnodenext = ptree_next(pnode);
            pnode = pnodenext;
            if(NULL == pnode)
            {
                return data_num;
            }
            prib = (struct rib_entry *)pnode->info;
            if(NULL == prib)
            {
                continue;
            }
            for (ALL_LIST_ELEMENTS(&prib->nhplist, pnode_list, pnextnode_list, pnhp_lookup))
            {
                if(pnhp_lookup)
                {
                    if((0 == flag) && (0 == data_num) && (pnhp_lookup->nhp_index == pindex->nhp.nhp_index))
                    {
                        flag = 1;
                        continue;
                    }
                    if(flag)
                    {
                        memcpy(&pdata[data_num].prefix, &prib->prefix, sizeof(struct inet_prefix));
                        memcpy(&pdata[data_num].nhp, pnhp_lookup, sizeof(struct nhp_entry));
                        pdata[data_num].vpn = prib->vpn;
                        
                        if(++data_num == msg_num)
                        {
                            return data_num;
                        }
                    }
                }
            }
        }
    }

    return data_num;
}




void route_count_set(struct route_count_mib *pcount, uint32_t vpn)
{
    struct ptree_node  *node1     = NULL;
    struct route_entry *proute    = NULL;
    struct listnode    *plistnode = NULL;
    struct ptree_node  *node2     = NULL;
    struct rib_entry   *prib      = NULL;
    struct nhp_entry   *pnhp      = NULL;
    int      i          = 0;
    memset(pcount , 0, sizeof(struct route_count_mib ));
    pcount->vpn_instance = vpn;
    
    PTREE_LOOP(route_tree[vpn], proute, node1)
    {
        for (i=0; i<proute->nhp_num; i++)
        {
            switch (proute->nhp[i].protocol)
            {
                case ROUTE_PROTO_CONNECT:
                    pcount->num_direct_active++;
                    break;
                case ROUTE_PROTO_STATIC:
                    pcount->num_static_active++;
                    break;
                case ROUTE_PROTO_OSPF:
                    pcount->num_ospf_active++;
                    break;
                case ROUTE_PROTO_RIP:
                    pcount->num_rip_active++;
                    break;
                case ROUTE_PROTO_ISIS:
                    pcount->num_isis_active++;
                    break;
                case ROUTE_PROTO_IBGP:
                    pcount->num_ibgp_active++;
                    break;
                case ROUTE_PROTO_EBGP:
                    pcount->num_ebgp_active++;
                    break;
                case ROUTE_PROTO_LDP:
                    pcount->num_ldp_active++;
                    break;
                case ROUTE_PROTO_MPLSTP:
                    pcount->num_mplstp_active++;
                    break;
                case ROUTE_PROTO_RSVPTE:
                    pcount->num_rsvpte_active++;
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
                        pcount->num_direct++;
                        break;
                    case ROUTE_PROTO_STATIC:
                        pcount->num_static++;
                        break;
                    case ROUTE_PROTO_OSPF:
                        pcount->num_ospf++;
                        break;
                    case ROUTE_PROTO_RIP:
                        pcount->num_rip++;
                        break;
                    case ROUTE_PROTO_ISIS:
                        pcount->num_isis++;
                        break;
                    case ROUTE_PROTO_IBGP:
                        pcount->num_ibgp++;
                        break;
                    case ROUTE_PROTO_EBGP:
                        pcount->num_ebgp++;
                        break;
                    case ROUTE_PROTO_LDP:
                        pcount->num_ldp++;
                        break;
                    case ROUTE_PROTO_MPLSTP:
                        pcount->num_mplstp++;
                        break;
                    case ROUTE_PROTO_RSVPTE:
                        pcount->num_rsvpte++;
                        break;
                    default :
                        break;
                }
            }
        }
    }

    pcount->total_num = pcount->num_direct + pcount->num_static + pcount->num_ospf + pcount->num_isis
                + pcount->num_rip + pcount->num_ibgp + pcount->num_ebgp + pcount->num_ldp
                + pcount->num_mplstp + pcount->num_rsvpte;
    pcount->active_num = pcount->num_direct_active + pcount->num_static_active + pcount->num_ospf_active
                + pcount->num_isis_active + pcount->num_rip_active + pcount->num_ibgp_active
                + pcount->num_ebgp_active + pcount->num_ldp_active + pcount->num_mplstp_active
                + pcount->num_rsvpte_active;

    return;
}

uint32_t route_rib_get_count_for_mib(struct route_count_mib *pdata, uint32_t index_flag)
{
    uint32_t msg_num  = IPC_MSG_LEN / sizeof(struct route_count_mib);
    uint32_t vpn =  0;
    uint32_t data_num = 0;

   // if(0 == index_flag)
    if(1 == index_flag)
    {
        vpn = 0;
        route_count_set(&pdata[data_num], vpn);
        data_num++;
    }
    else
    {
        vpn = pdata->vpn_instance;
 
        for( vpn++; vpn < (L3VPN_SIZE + 1); vpn ++)
        {
            route_count_set(&pdata[data_num], vpn);
            if(pdata[data_num].total_num > 0)
            {
                data_num++;
                break;
            }
        }
    }
    return data_num;
}

