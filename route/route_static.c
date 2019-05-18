/**
 * @file      : route_static.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月26日 9:20:57
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/memory.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include "route_main.h"
#include "route_cmd.h"
#include "route_static.h"
#include "route_rib.h"
#include "route.h"
#include "route_if.h"


struct hash_table static_route_table;
struct hash_table static_routev6_table;


/**
 * @brief      : 使用静态路由信息填充路由下一跳信息
 * @param[in ] : pnhp   - 路由下一跳结构
 * @param[in ] : sroute - 静态路由结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 16:23:34
 * @note       :
 */
void route_static_set_nhp(struct nhp_entry *pnhp, struct route_static *sroute)
{
    ROUTE_LOG_DEBUG();

    if ((NULL == pnhp) || (NULL == sroute))
    {
        return;
    }

    memset(pnhp, 0, sizeof(struct nhp_entry));

    pnhp->protocol          = ROUTE_PROTO_STATIC;
    pnhp->nexthop.addr.ipv4 = sroute->nexthop.addr.ipv4;
    pnhp->nexthop.type      = INET_FAMILY_IPV4;
    pnhp->action            = sroute->action;
    pnhp->active            = ROUTE_STATUS_INACTIVE;
    pnhp->ifindex           = sroute->ifindex;
    pnhp->nhp_type          = sroute->nhp_type;
    pnhp->vpn               = sroute->vpn;
    pnhp->distance          = sroute->distance;
    pnhp->down_flag         = LINK_DOWN;
    pnhp->cost              = ROUTE_COST;

    return;
}


/**
 * @brief      : 计算静态路由哈希表 key 值
 * @param[in ] : hash_key - 静态路由结构
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年2月11日 16:25:11
 * @note       : key 值由（路由前缀+掩码+vpn+下一跳）相加计算获得
 */
static uint32_t route_static_compute_hash(void *hash_key)
{
    struct route_static *sroute = (struct route_static *)hash_key;

    if (NULL == hash_key)
    {
        return 0;
    }

    return (sroute->prefix.addr.ipv4 + sroute->prefix.prefixlen +
            sroute->vpn + sroute->nexthop.addr.ipv4);
}


/**
 * @brief      : 比较静态路由哈希表 key 值
 * @param[in ] : item     - 哈希桶
 * @param[in ] : hash_key - 静态路由结构
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年2月11日 16:29:01
 * @note       : 路由前缀、掩码、vpn、nexthop 必须完全相同，才会返回成功
 */
static int route_static_compare(void *item, void *hash_key)
{
    struct hash_bucket  *pbucket    = (struct hash_bucket *)item;
    struct route_static *sroute_old = (struct route_static *)pbucket->data;
    struct route_static *sroute_new = (struct route_static *)hash_key;

    if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

    if ((sroute_old->prefix.addr.ipv4 == sroute_new->prefix.addr.ipv4)
        && (sroute_old->prefix.prefixlen == sroute_new->prefix.prefixlen)
        && (sroute_old->vpn == sroute_new->vpn)
        && (sroute_old->nexthop.addr.ipv4 == sroute_new->nexthop.addr.ipv4))
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : 静态路由哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 16:33:23
 * @note       :
 */
void route_static_table_init(uint32_t size)
{
    hios_hash_init(&static_route_table, size, route_static_compute_hash, route_static_compare);

    return;
}


/**
 * @brief      : 静态路由条目分配内存并返回
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回分配内存后的静态路由条目，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月11日 16:40:13
 * @note       :
 */
struct route_static *route_static_create(void)
{
    struct route_static *proute = NULL;

    proute = (struct route_static *)XCALLOC(MTYPE_STATIC_ROUTE, sizeof(struct route_static));
    if (NULL == proute)
    {
        ROUTE_LOG_ERROR("malloc");
    }

    return proute;
}


/**
 * @brief      : 添加静态路由
 * @param[in ] : proute - 静态路由结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 16:34:13
 * @note       :
 */
int route_static_add(struct route_static *proute)
{
    struct hash_bucket  *pitem  = NULL;
    struct route_static *sroute = NULL;
    struct nhp_entry nhp;
    int ret;

    ROUTE_LOG_DEBUG();

    if (NULL == proute)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    /* route-id 不存在自动生成 */
    route_set_router_id();

    /* 静态路由更新操作 */
    sroute = route_static_lookup(proute);
    if (NULL != sroute)
    {
        if (sroute->distance != proute->distance)
        {
            sroute->distance = proute->distance;

            route_static_set_nhp(&nhp, sroute);
            rib_update(&sroute->prefix, sroute->vpn, &nhp);

            return ERRNO_SUCCESS_UPDATE;
        }

        return ERRNO_EXISTED_STATIC_ROUTE;
    }

    /* 路由数量检测 */
    if (static_route_table.hash_size == static_route_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    sroute = route_static_create();
    if (NULL == sroute)
    {
        ROUTE_LOG_ERROR("malloc");

        return ERRNO_MALLOC;
    }

    memcpy(sroute, proute, sizeof(struct route_static));

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        ROUTE_LOG_ERROR("malloc");

        route_static_free(sroute);

        return ERRNO_MALLOC;
    }

    pitem->hash_key = sroute;
    pitem->data     = sroute;

    hios_hash_add(&static_route_table, pitem);

    /* 向 rib 添加路由 */
    route_static_set_nhp(&nhp, sroute);
    ret = rib_add(&sroute->prefix, sroute->vpn, &nhp);
    if (ERRNO_SUCCESS != ret)
    {
        hios_hash_delete(&static_route_table, pitem);
        route_static_free(sroute);
        HASH_BUCKET_DESTROY(pitem);

        return ret;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 静态路由删除
 * @param[in ] : proute - 静态路由结构
 * @param[out] :
 * @return     : 删除成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 16:54:02
 * @note       :
 */
int route_static_delete(struct route_static *proute)
{
    struct hash_bucket  *pitem  = NULL;
    struct route_static *sroute = NULL;
    struct nhp_entry nhp;

    ROUTE_LOG_DEBUG();

    if (NULL == proute)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pitem = hios_hash_find(&static_route_table, proute);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    sroute = (struct route_static *)pitem->data;

    /* 从 rib 表删除路由 */
    route_static_set_nhp(&nhp, sroute);
    rib_delete(&sroute->prefix, sroute->vpn, &nhp);

    /* 从 hash 表中删除静态路由 */
    hios_hash_delete(&static_route_table, pitem);
    route_static_free(sroute);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 释放静态路由条目内存
 * @param[in ] : proute - 静态路由结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 16:56:53
 * @note       : 不可传入未分配内存的指针
 */
void route_static_free(struct route_static *proute)
{
    if (NULL != proute)
    {
        XFREE(MTYPE_STATIC_ROUTE, proute);
    }

    return;
}


/**
 * @brief      : 静态路由条目查找
 * @param[in ] : proute - 静态路由结构(包含哈希 key 值)
 * @param[out] :
 * @return     : 找到目标数据返回静态路由结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月11日 16:58:30
 * @note       :
 */
struct route_static *route_static_lookup(struct route_static *proute)
{
    struct hash_bucket *pitem = NULL;

    ROUTE_LOG_DEBUG();

    if (NULL == proute)
    {
        return NULL;
    }

    pitem = hios_hash_find(&static_route_table, proute);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct route_static *)pitem->data;
}


/**
 * @brief      : 静态路由批量获取
 * @param[in ] : psroute  - 静态路由结构
 * @param[out] : sroute[] - 返回查询到的数据(多个静态路由结构)
 * @return     : 返回静态路由数量
 * @author     : ZhangFj
 * @date       : 2018年2月11日 16:59:36
 * @note       : 提供给 snmp 的 get next 接口，空索引从数据结构开始返回数据，
 * @note       : 非空索引从传入索引节点下一个节点开始返回数据
 */
int route_static_get_bulk(struct route_static *psroute, struct route_static sroute[], uint32_t index_flag)
{
    struct hash_bucket  *pbucket = NULL;
    struct hash_bucket  *pnext   = NULL;
    struct route_static *proute  = NULL;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct route_static);
    int cursor;
    int val      = 0;

    if (NULL == psroute)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

#if 0
    if ((0 == psroute->prefix.addr.ipv4)
        && (0 == psroute->prefix.prefixlen)
        && (0 == psroute->nexthop.addr.ipv4)
        && (0 == psroute->vpn))
#endif
    if (0 == index_flag)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, static_route_table)
        {
            proute = (struct route_static *)pbucket->data;
            if (NULL == proute)
            {
                continue;
            }

            memcpy(&sroute[data_num++], proute, sizeof(struct route_static));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&static_route_table, (void *)psroute);
        if (NULL == pbucket)
        {
            /*
             * 当前数据可能已经被删除，返回到当前哈希桶的链表头部
             * 如果当前哈希桶链表头为空，向后面的哈希桶继续查找
             */
            val = static_route_table.compute_hash((void *)psroute)%HASHTAB_SIZE;
            if (NULL != static_route_table.buckets[val])
            {
                pbucket = static_route_table.buckets[val];
            }
            else
            {
                for (++val; val<HASHTAB_SIZE; ++val)
            	{
            		if (NULL != static_route_table.buckets[val])
            		{
            			pbucket = static_route_table.buckets[val];

                        break;
            		}
                }
            }
        }

        if (NULL != pbucket)
        {
            for (data_num=0; data_num<msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&static_route_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                proute = pnext->data;
                memcpy(&sroute[data_num], proute, sizeof(struct route_static));
                pbucket = pnext;
            }
        }
    }

    return data_num;
}


/**
 * @brief      : 静态路由更新
 * @param[in ] : pthread - 线程结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:06:39
 * @note       : 由于静态路由支持迭代 3 次，所以每次更新需要轮询 3 次哈希表
 */
int route_static_update_routing(void)
{
    struct hash_bucket  *pbucket = NULL;
    struct route_static *sroute  = NULL;
    struct nhp_entry nhp;
    int cursor = 0;
    int i      = 0;

    ROUTE_LOG_DEBUG("starting\n");

    g_route.route_timer = NULL;

    for (i=0; i<ROUTE_CONVER_NUM; i++)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, static_route_table)
        {
            sroute = (struct route_static *)pbucket->data;
            if (NULL == sroute)
            {
                continue;
            }

            route_static_set_nhp(&nhp, sroute);
            rib_update(&sroute->prefix, sroute->vpn, &nhp);
        }
    }

    ROUTE_LOG_DEBUG("ending\n");

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tunnel 接口状态变化，更新出接口为 tunnel 的路由
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:09:19
 * @note       :
 */
void route_static_process_routing(uint32_t ifindex)
{
    struct hash_bucket  *pbucket = NULL;
    struct route_static *sroute  = NULL;
    struct nhp_entry nhp;
    int cursor = 0;

    ROUTE_LOG_DEBUG("starting\n");

    if (!IFM_TYPE_IS_TUNNEL(ifindex))
    {
        return;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, static_route_table)
    {
        sroute = (struct route_static *)pbucket->data;
        if (NULL == sroute)
        {
            continue;
        }

        if (ifindex == sroute->ifindex)
        {
            route_static_set_nhp(&nhp, sroute);
            rib_update(&sroute->prefix, sroute->vpn, &nhp);
        }
    }

    ROUTE_LOG_DEBUG("ending\n");

    return;
}


/**
 * @brief      : l3vpn 实例删除，直接删除静态路由
 * @param[in ] : vrf_id - vpn 实例号
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:10:39
 * @note       :
 */
void route_static_process_l3vpn_delete(uint16_t vrf_id)
{
    struct hash_bucket  *pbucket      = NULL;
    struct hash_bucket  *pbucket_next = NULL;
    struct route_static *sroute       = NULL;
    int cursor = 0;

    ROUTE_LOG_DEBUG("starting\n");

    HASH_BUCKET_LOOPW(pbucket, cursor, static_route_table)
    {
        pbucket_next = pbucket->next;

        sroute = (struct route_static *)pbucket->data;
        if (NULL == sroute)
        {
            continue;
        }

        if (sroute->vpn == vrf_id)
        {
            route_static_delete(sroute);
        }

        pbucket = pbucket_next;
    }

    return;
}


/**
 * @brief      : 使用静态路由信息填充路由下一跳信息
 * @param[in ] : pnhp   - 路由下一跳结构
 * @param[in ] : sroute - 静态路由结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:12:47
 * @note       :
 */
void routev6_static_set_nhp(struct nhp_entry *pnhp, struct route_static *sroute)
{
    ROUTE_LOG_DEBUG();

    if ((NULL == pnhp) || (NULL == sroute))
    {
        return;
    }

    memset(pnhp, 0, sizeof(struct nhp_entry));

    memcpy(&pnhp->nexthop, &sroute->nexthop, sizeof(struct inet_addr));
    pnhp->protocol  = ROUTE_PROTO_STATIC;
    pnhp->action    = sroute->action;
    pnhp->active    = ROUTE_STATUS_INACTIVE;
    pnhp->ifindex   = sroute->ifindex;
    pnhp->nhp_type  = sroute->nhp_type;
    pnhp->vpn       = sroute->vpn;
    pnhp->distance  = sroute->distance;
    pnhp->down_flag = LINK_DOWN;
    pnhp->cost      = ROUTE_COST;

    return;
}


/**
 * @brief      : 计算静态路由哈希表 key 值
 * @param[in ] : hash_key - 静态路由结构
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:14:31
 * @note       : key 值由（路由前缀 + 掩码 + vpn + 下一跳）相加计算获得
 */
static unsigned int routev6_static_compute_hash(void *hash_key)
{
    struct route_static *sroute = (struct route_static *)hash_key;
    uint32_t key = 0;
    uint32_t i   = 0;

    if (NULL == hash_key)
    {
        return 0;
    }

    for(i=0; i<IPV6_ADDR_LEN; i++)
    {
        key += sroute->prefix.addr.ipv6[i];
        key += sroute->nexthop.addr.ipv6[i];
    }

    key += sroute->prefix.prefixlen;
    key += sroute->vpn;

    return key;
}


/**
 * @brief      : 比较静态路由哈希表 key 值
 * @param[in ] : item     - 哈希桶
 * @param[in ] : hash_key - 静态路由结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:15:21
 * @note       : 路由前缀、掩码、vpn、nexthop 必须完全相同，才会返回成功
 */
static int routev6_static_compare(void *item, void *hash_key)
{
    struct hash_bucket  *pbucket    = (struct hash_bucket *)item;
    struct route_static *sroute_old = (struct route_static *)pbucket->data;
    struct route_static *sroute_new = (struct route_static *)hash_key;

    if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

    if ((0 == memcmp(&sroute_old->prefix, &sroute_new->prefix, sizeof(struct inet_prefix)))
        && (sroute_old->prefix.prefixlen == sroute_new->prefix.prefixlen)
        && (sroute_old->vpn == sroute_new->vpn)
        && (0 == memcmp(&sroute_old->nexthop, &sroute_new->nexthop, sizeof(struct inet_addr))))
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : 静态路由哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:17:55
 * @note       :
 */
void routev6_static_table_init(unsigned int size)
{
    hios_hash_init(&static_routev6_table, size, routev6_static_compute_hash, routev6_static_compare);

    return;
}


/**
 * @brief      : 静态路由条目分配内存并返回
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回分配内存后的静态路由条目，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:19:56
 * @note       :
 */
struct route_static *routev6_static_create(void)
{
    struct route_static *proute = NULL;

    proute = (struct route_static *)XCALLOC(MTYPE_STATIC_ROUTE, sizeof(struct route_static));
    if (NULL == proute)
    {
        ROUTE_LOG_ERROR("malloc");

        return NULL;
    }

    return proute;
}


/**
 * @brief      : 添加静态路由
 * @param[in ] : proute - 静态路由结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:18:18
 * @note       :
 */
int routev6_static_add(struct route_static *proute)
{
    struct hash_bucket  *pitem  = NULL;
    struct route_static *sroute = NULL;
    struct nhp_entry nhp;
    int ret;

    ROUTE_LOG_DEBUG();

    if (NULL == proute)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    /* route-id 不存在自动生成 */
    routev6_set_router_id();

    /* 静态路由更新操作 */
    sroute = routev6_static_lookup(proute);
    if (NULL != sroute)
    {
        if (sroute->distance != proute->distance)
        {
            sroute->distance = proute->distance;

            routev6_static_set_nhp(&nhp, sroute);
            ribv6_update(&sroute->prefix, sroute->vpn, &nhp);

            return ERRNO_SUCCESS_UPDATE;
        }

        return ERRNO_EXISTED_STATIC_ROUTE;
    }

    /* 路由数量检测 */
    if (static_routev6_table.hash_size == static_routev6_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    sroute = routev6_static_create();
    if (NULL == sroute)
    {
        ROUTE_LOG_ERROR("malloc");

        return ERRNO_MALLOC;
    }

    memcpy(sroute, proute, sizeof(struct route_static));

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        ROUTE_LOG_ERROR("malloc");

        routev6_static_free(sroute);

        return ERRNO_MALLOC;
    }

    pitem->hash_key = sroute;
    pitem->data     = sroute;

    hios_hash_add(&static_routev6_table, pitem);

    /* 向 rib 添加路由 */
    routev6_static_set_nhp(&nhp, sroute);
    ret = ribv6_add(&sroute->prefix, sroute->vpn, &nhp);
    if (ERRNO_SUCCESS != ret)
    {
        hios_hash_delete(&static_routev6_table, pitem);
        routev6_static_free(sroute);
        HASH_BUCKET_DESTROY(pitem);

        return ret;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 静态路由删除
 * @param[in ] : proute - 静态路由结构
 * @param[out] :
 * @return     : 删除成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:26:51
 * @note       :
 */
int routev6_static_delete(struct route_static *proute)
{
    struct hash_bucket  *pitem  = NULL;
    struct route_static *sroute = NULL;
    struct nhp_entry nhp;

    ROUTE_LOG_DEBUG();

    if (NULL == proute)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pitem = hios_hash_find(&static_routev6_table, proute);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    sroute = (struct route_static *)pitem->data;

    /* 从 rib 表删除路由 */
    routev6_static_set_nhp(&nhp, sroute);
    ribv6_delete(&sroute->prefix, sroute->vpn, &nhp);

    /* 从 hash 表中删除静态路由 */
    hios_hash_delete(&static_routev6_table, pitem);
    routev6_static_free(sroute);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 释放静态路由条目内存
 * @param[in ] : proute - 静态路由结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:27:56
 * @note       : 不可传入未分配内存的指针
 */
void routev6_static_free(struct route_static *proute)
{
    if (NULL != proute)
    {
        XFREE(MTYPE_STATIC_ROUTE, proute);
    }

    return;
}


/**
 * @brief      : 静态路由条目查找
 * @param[in ] : proute - 静态路由结构(包含哈希 key 值)
 * @param[out] :
 * @return     : 找到目标数据返回静态路由结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:28:53
 * @note       :
 */
struct route_static *routev6_static_lookup(struct route_static *proute)
{
    struct hash_bucket *pitem = NULL;

    ROUTE_LOG_DEBUG();

    if (NULL == proute)
    {
        return NULL;
    }

    pitem = hios_hash_find(&static_routev6_table, proute);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct route_static *)pitem->data;
}


/**
 * @brief      : 静态路由批量获取
 * @param[in ] : psroute  - 静态路由结构
 * @param[out] : sroute[] - 返回查询到的数据(多个静态路由结构)
 * @return     : 返回静态路由数量
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:30:23
 * @note       : 提供给 snmp 的 get next 接口，空索引从数据结构开始返回数据，
 * @note       : 非空索引从传入索引节点下一个节点开始返回数据
 */
int routev6_static_get_bulk(struct route_static *psroute, struct route_static sroute[])
{
    struct hash_bucket  *pbucket = NULL;
    struct hash_bucket  *pnext   = NULL;
    struct route_static *proute  = NULL;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct route_static);
    int cursor;
    int val      = 0;

    if (NULL == psroute)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if ((ROUTE_IF_IPV6_EXIST(psroute->prefix.addr.ipv6))
        && (0 == psroute->prefix.prefixlen)
        && (ROUTE_IF_IPV6_EXIST(psroute->nexthop.addr.ipv6))
        && (0 == psroute->vpn))
    {
        HASH_BUCKET_LOOP(pbucket, cursor, static_routev6_table)
        {
            proute = (struct route_static *)pbucket->data;
            if (NULL == proute)
            {
                continue;
            }

            memcpy(&sroute[data_num++], proute, sizeof(struct route_static));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&static_routev6_table, (void *)psroute);
        if (NULL == pbucket)
        {
            /*
             * 当前数据可能已经被删除，返回到当前哈希桶的链表头部
             * 如果当前哈希桶链表头为空，向后面的哈希桶继续查找
             */
            val = static_routev6_table.compute_hash((void *)psroute)%HASHTAB_SIZE;
            if (NULL != static_routev6_table.buckets[val])
            {
                pbucket = static_routev6_table.buckets[val];
            }
            else
            {
                for (++val; val<HASHTAB_SIZE; ++val)
            	{
            		if (NULL != static_routev6_table.buckets[val])
            		{
            			pbucket = static_routev6_table.buckets[val];
            		}
                }
            }
        }

        if (NULL != pbucket)
        {
            for (data_num=0; data_num<msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&static_routev6_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                proute = pnext->data;
                memcpy(&sroute[data_num], proute, sizeof(struct route_static));
                pbucket = pnext;
            }
        }
    }

    return data_num;
}


/**
 * @brief      : 静态路由更新
 * @param[in ] : pthread - 线程结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS
 * @author     : ZhangFj
 * @date       : 2018年2月11日 17:31:53
 * @note       : 由于静态路由支持迭代 3 次，所以每次更新需要轮询 3 次哈希表
 */
int routev6_static_update_routing(void)
{
    struct hash_bucket  *pbucket = NULL;
    struct route_static *sroute  = NULL;
    struct nhp_entry nhp;
    int cursor = 0;
    int i      = 0;

    ROUTE_LOG_DEBUG("starting\n");

    g_route.routev6_timer = NULL;

    for (i=0; i<ROUTE_CONVER_NUM; i++)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, static_routev6_table)
        {
            sroute = (struct route_static *)pbucket->data;

            if (NULL == sroute)
            {
                continue;
            }

            routev6_static_set_nhp(&nhp, sroute);
            ribv6_update(&sroute->prefix, sroute->vpn, &nhp);
        }
    }

    ROUTE_LOG_DEBUG("ending\n");

    return ERRNO_SUCCESS;
}


