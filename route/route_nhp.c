/**
 * @file      : route_nhp.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月26日 17:18:29
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/memory.h>
#include <lib/hash1.h>
#include <lib/index.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include "route_main.h"
#include "route_nhp.h"
#include "route.h"
#include "route_msg.h"


/* nhp 表，存储下一跳和 ecmp */
struct hash_table route_nhp_table;

/* 下一跳表，存储路由下一跳信息 */
struct hash_table route_nexthop_table;

/* ecmp表，存储 ecmp_group */
struct hash_table route_ecmp_table;


/**
 * @brief      : 计算 nhp 哈希表 key 值
 * @param[in ] : hash_key - nhp 索引
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:21:26
 * @note       : key 值为 nhp 索引
 */
static uint32_t route_nhp_compute_hash(void *hash_key)
{
    if (NULL == hash_key)
    {
        return 0;
    }

    return (uint32_t)hash_key;
}


/**
 * @brief      : 比较 nhp 哈希表 key 值
 * @param[in ] : item     - 哈希桶
 * @param[in ] : hash_key - nhp 索引
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:22:27
 * @note       :
 */
static int route_nhp_compare(void *item, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)item;

    if ((NULL == pbucket) || (NULL == pbucket->data) || (NULL == hash_key))
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
 * @brief      : nhp 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:23:16
 * @note       :
 */
void route_nhp_table_init(uint32_t size)
{
    hios_hash_init(&route_nhp_table, size, route_nhp_compute_hash, route_nhp_compare);

    return;
}


/**
 * @brief      : nhp 条目分配内存，添加至哈希表中并返回
 * @param[in ] : pinfo - nhp 结构
 * @param[out] :
 * @return     : 成功返回分配内存后的 nhp 条目，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:23:38
 * @note       :
 */
static struct nhp_info *route_nhp_create(struct nhp_info *pinfo)
{
    struct nhp_info *pinfo_new = NULL;
    int ret;

    ROUTE_LOG_DEBUG();

    if (NULL == pinfo)
    {
        return NULL;
    }

    pinfo_new = (struct nhp_info *)XCALLOC(MTYPE_NHP_ENTRY, sizeof(struct nhp_info));
    if (NULL == pinfo_new)
    {
        ROUTE_LOG_ERROR("malloc\n");

        return NULL;
    }

    pinfo_new->nhp_index = pinfo->nhp_index;
    pinfo_new->nhp_type  = pinfo->nhp_type;
    pinfo_new->refcount  = 1;
    pinfo_new->pnhp      = pinfo->pnhp;

    ret = route_nhp_add(pinfo_new);
    if (ERRNO_SUCCESS != ret)
    {
        ROUTE_LOG_ERROR("errcode = %d\n", ret);

        XFREE(MTYPE_NHP_ENTRY, pinfo_new);

        return NULL;
    }

    return pinfo_new;
}


/**
 * @brief      : nhp 添加
 * @param[in ] : pinfo - nhp 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:26:33
 * @note       :
 */
int route_nhp_add(struct nhp_info *pinfo)
{
    struct hash_bucket *pitem = NULL;

    ROUTE_LOG_DEBUG();

    if (pinfo == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (route_nhp_table.hash_size == route_nhp_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        ROUTE_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)pinfo->nhp_index;
    pitem->data     = pinfo;

    hios_hash_add(&route_nhp_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : nhp 删除
 * @param[in ] : nhp_index - nhp 索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:28:04
 * @note       :
 */
int route_nhp_delete(uint32_t nhp_index)
{
    struct hash_bucket *pitem = NULL;

    ROUTE_LOG_DEBUG();

    pitem = hios_hash_find(&route_nhp_table, (void *)nhp_index);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    hios_hash_delete(&route_nhp_table, pitem);
    XFREE(MTYPE_NHP_ENTRY, pitem->data);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : nhp 查找
 * @param[in ] : nhp_index - nhp 索引
 * @param[out] :
 * @return     : 成功返回 nhp 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:28:44
 * @note       :
 */
struct nhp_info *route_nhp_lookup(uint32_t nhp_index)
{
    struct hash_bucket *pitem = NULL;

    ROUTE_LOG_DEBUG();

    pitem = hios_hash_find(&route_nhp_table, (void *)nhp_index);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct nhp_info *)pitem->data;
}


/**
 * @brief      : 计算 nexthop 哈希表 key 值
 * @param[in ] : hash_key - 下一跳结构
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:29:26
 * @note       : key 值由（IP 地址 + vpn）相加计算获得
 */
static uint32_t route_nexthop_compute_hash(void *hash_key)
{
    struct nhp_entry *pnhp = NULL;
    uint32_t key = 0;
    int i;

    if (NULL == hash_key)
    {
        return 0;
    }

    pnhp = (struct nhp_entry *)hash_key;

    if (INET_FAMILY_IPV6 == pnhp->nexthop.type)
    {
        for(i=0; i<IPV6_ADDR_LEN; i++)
        {
            key += pnhp->nexthop.addr.ipv6[i];
        }

        key += pnhp->ifindex;
        key += pnhp->vpn;
    }
    else
    {
        key = pnhp->nexthop.addr.ipv4 + pnhp->ifindex + pnhp->vpn;
    }

    return key;
}


/**
 * @brief      : 比较 nexthop 哈希表 key 值
 * @param[in ] : item     - 哈希桶
 * @param[in ] : hash_key - 下一跳结构
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:31:46
 * @note       : IP 地址与 vpn 必须完全相同，才会返回成功
 */
static int route_nexthop_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;
    struct nhp_entry *pnhp_old, *pnhp_new;

    if ((NULL == pbucket) || (NULL == pbucket->hash_key) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

    pnhp_old = (struct nhp_entry *)pbucket->hash_key;
    pnhp_new = (struct nhp_entry *)hash_key;

    if (pnhp_old->nexthop.type != pnhp_new->nexthop.type)
    {
        return ERRNO_FAIL;
    }

    if (INET_FAMILY_IPV6 == pnhp_new->nexthop.type)
    {
        if (0 == memcmp(pnhp_old->nexthop.addr.ipv6, pnhp_new->nexthop.addr.ipv6, IPV6_ADDR_LEN)
            && (pnhp_old->ifindex == pnhp_new->ifindex)
            && (pnhp_old->vpn == pnhp_new->vpn))
        {
            return ERRNO_SUCCESS;
        }
    }
    else
    {
        if ((pnhp_old->nexthop.addr.ipv4 == pnhp_new->nexthop.addr.ipv4)
            && (pnhp_old->ifindex == pnhp_new->ifindex)
            && (pnhp_old->vpn == pnhp_new->vpn))
        {
            return ERRNO_SUCCESS;
        }
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : nexthop 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:34:15
 * @note       :
 */
void route_nexthop_table_init(uint32_t size)
{
    hios_hash_init(&route_nexthop_table, size, route_nexthop_compute_hash,
                        route_nexthop_compare);

    return;
}


/**
 * @brief      : nexthop 条目分配内存，添加至哈希表中并返回
 * @param[in ] : pnhp - 下一跳结构
 * @param[out] :
 * @return     : 成功返回分配内存后的 nexthop 条目，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:34:53
 * @note       :
 */
static struct nhp_entry *route_nexthop_create(struct nhp_entry *pnhp)
{
    struct nhp_entry *pnhp_new = NULL;
    int ret;

    ROUTE_LOG_DEBUG();

    if (NULL == pnhp)
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

    /* 为 nhp 申请索引*/
    pnhp_new->nhp_index = index_alloc(INDEX_TYPE_NHP);
    if (0 == pnhp_new->nhp_index)
    {
        ROUTE_LOG_ERROR("index alloc\n");

        XFREE(MTYPE_NHP_ENTRY, pnhp_new);

        return NULL;
    }

    /* 下发配置信息 */
    route_add_msg_to_send_list(pnhp_new, sizeof(struct nhp_entry), 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
                        IPC_TYPE_NHP, pnhp_new->nhp_type, IPC_OPCODE_ADD, 0);

    ret = route_nexthop_add(pnhp_new);
    if (ERRNO_SUCCESS != ret)
    {
        index_free(INDEX_TYPE_NHP, pnhp_new->nhp_index);
        XFREE(MTYPE_NHP_ENTRY, pnhp_new);

        return NULL;
    }

    return pnhp_new;
}


/**
 * @brief      : nexthop 添加
 * @param[in ] : pnhp - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:36:39
 * @note       :
 */
int route_nexthop_add(struct nhp_entry *pnhp)
{
    struct hash_bucket *pitem = NULL;

    ROUTE_LOG_DEBUG();

    if (pnhp == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (route_nexthop_table.hash_size == route_nexthop_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)pnhp;
    pitem->data     = pnhp;

    hios_hash_add(&route_nexthop_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : nexthop 更新
 * @param[in ] : pnhp_new - 被更新的下一跳条目
 * @param[in ] : pnhp_old - 携带新的下一跳信息下一跳结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:39:01
 * @note       :
 */
void route_nexthop_update(struct nhp_entry *pnhp_old, struct nhp_entry *pnhp_new)
{
    ROUTE_LOG_DEBUG();

    if ((pnhp_old == NULL) || (pnhp_new == NULL))
    {
        return ;
    }

    /* 更新 nhp_entry */
    pnhp_old->distance = pnhp_new->distance;
    pnhp_old->nhp_type = pnhp_new->nhp_type;
    pnhp_old->action   = pnhp_new->action;
    pnhp_old->protocol = pnhp_new->protocol;

    route_add_msg_to_send_list(pnhp_old, sizeof(struct nhp_entry), 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
                        IPC_TYPE_NHP, pnhp_old->nhp_type, IPC_OPCODE_UPDATE, 0);

    return;
}


/**
 * @brief      : nexthop 删除
 * @param[in ] : pnhp - 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:41:01
 * @note       :
 */
int route_nexthop_delete(struct nhp_entry *pnhp)
{
    struct hash_bucket *pitem       = NULL;
    struct nhp_entry   *pnhp_lookup = NULL;

    ROUTE_LOG_DEBUG();

    if (NULL == pnhp)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pitem = hios_hash_find(&route_nexthop_table, (void *)pnhp);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    pnhp_lookup = (struct nhp_entry *)pitem->data;

    route_add_msg_to_send_list(pnhp_lookup, sizeof(struct nhp_entry), 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
                        IPC_TYPE_NHP, pnhp_lookup->nhp_type, IPC_OPCODE_DELETE, 0);

    hios_hash_delete(&route_nexthop_table, pitem);
    index_free(INDEX_TYPE_NHP, pnhp->nhp_index);
    XFREE(MTYPE_NHP_ENTRY, pnhp_lookup);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : nexthop 查找
 * @param[in ] : pnhp - 下一跳结构
 * @param[out] :
 * @return     : 成功返回下一跳结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:42:12
 * @note       :
 */
struct nhp_entry *route_nexthop_lookup(struct nhp_entry *pnhp)
{
    struct hash_bucket *pitem = NULL;

    ROUTE_LOG_DEBUG();

    if (NULL == pnhp)
    {
        return NULL;
    }

    pitem = hios_hash_find(&route_nexthop_table, (void *)pnhp);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct nhp_entry *)pitem->data;
}


/**
 * @brief      : 计算 ecmp 哈希表 key 值
 * @param[in ] : hash_key - ecmp 结构
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:43:15
 * @note       : key 值由（所有 nhp 索引和 group_type）相加计算获得
 */
static uint32_t route_ecmp_compute_hash(void *hash_key)
{
    struct ecmp_group *pecmp = NULL;
    uint32_t key = 0;
    int i;

    if (NULL == hash_key)
    {
        return 0;
    }

    pecmp = (struct ecmp_group *)hash_key;

    for (i=0; i<pecmp->nhp_num; i++)
    {
        key += pecmp->nhp[i].nhp_index;
    }

    key += pecmp->group_type;

    return key;
}


/**
 * @brief      : 比较 ecmp 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - ecmp 结构
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:44:30
 * @note       : 所有 nhp 索引必须完全相同，才会返回成功
 */
static int route_ecmp_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;
    struct ecmp_group  *pecmp_old = NULL;
    struct ecmp_group  *pecmp_new = NULL;
    int i, j;
    int flag_cmp = 0;

    if ((NULL == pbucket) || (NULL == pbucket->hash_key) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

    pecmp_old = (struct ecmp_group *)pbucket->hash_key;
    pecmp_new = (struct ecmp_group *)hash_key;

    if (pecmp_old->nhp_num != pecmp_new->nhp_num)
    {
        return ERRNO_FAIL;
    }

    if (pecmp_old->group_type != pecmp_new->group_type)
    {
        return ERRNO_FAIL;
    }

    if (pecmp_old->group_type == ECMP_TYPE_ECMP)
    {
        for (i=0; i<pecmp_old->nhp_num; i++)
        {
            for (j=0; j<pecmp_new->nhp_num; j++)
            {
                if (pecmp_old->nhp[i].nhp_index == pecmp_new->nhp[j].nhp_index)
                {
                    flag_cmp = 1;

                    break;
                }
            }

            if (flag_cmp == 0)
            {
                return ERRNO_FAIL;
            }

            flag_cmp = 0;
        }
    }
    else if (pecmp_old->group_type == ECMP_TYPE_FRR)
    {
        if ((pecmp_old->nhp[0].nhp_index == pecmp_new->nhp[0].nhp_index)
            && (pecmp_old->nhp[1].nhp_index == pecmp_new->nhp[1].nhp_index))
        {
            return ERRNO_SUCCESS;
        }

        return ERRNO_FAIL;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ecmp 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:45:36
 * @note       :
 */
void route_ecmp_table_init(unsigned int size)
{
    hios_hash_init(&route_ecmp_table, size, route_ecmp_compute_hash, route_ecmp_compare);

    return;
}


/**
 * @brief      : ecmp 条目分配内存，添加至哈希表中并返回
 * @param[in ] : pecmp - ecmp 结构
 * @param[out] :
 * @return     : 成功返回分配内存的 ecmp 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:46:07
 * @note       :
 */
struct ecmp_group *route_ecmp_create(struct ecmp_group *pecmp)
{
    struct ecmp_group *pecmp_new = NULL;
    int ret;

    ROUTE_LOG_DEBUG();

    if (NULL == pecmp)
    {
        return NULL;
    }

    pecmp_new = (struct ecmp_group *)XCALLOC(MTYPE_ECMP_ENTRY, sizeof(struct ecmp_group));
    if (NULL == pecmp_new)
    {
        ROUTE_LOG_ERROR("malloc\n");

        return NULL;
    }

    memcpy(pecmp_new, pecmp, sizeof(struct ecmp_group));

    pecmp_new->group_index = index_alloc(INDEX_TYPE_NHP);
    if (0 == pecmp_new->group_index)
    {
        ROUTE_LOG_ERROR("index alloc\n");

        XFREE(MTYPE_ECMP_ENTRY, pecmp_new);

        return NULL;
    }

    route_add_msg_to_send_list(pecmp_new, sizeof(struct ecmp_group), 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
                        IPC_TYPE_NHP, NHP_TYPE_ECMP, IPC_OPCODE_ADD, 0);

    ret = route_ecmp_add(pecmp_new);
    if (ERRNO_SUCCESS != ret)
    {
        index_free(INDEX_TYPE_NHP, pecmp_new->group_index);
        XFREE(MTYPE_ECMP_ENTRY, pecmp_new);

        return NULL;
    }

    return pecmp_new;
}


/**
 * @brief      : ecmp 添加
 * @param[in ] : pecmp - ecmp 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:49:04
 * @note       :
 */
int route_ecmp_add(struct ecmp_group *pecmp)
{
    struct hash_bucket *pitem = NULL;

    ROUTE_LOG_DEBUG();

    if (NULL == pecmp)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (route_ecmp_table.hash_size == route_ecmp_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        ROUTE_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)pecmp;
    pitem->data     = pecmp;

    hios_hash_add(&route_ecmp_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ecmp 更新
 * @param[in ] : pecmp_old - 被更新的 ecmp 条目
 * @param[in ] : pecmp_new - 携带新的 ecmp 信息 ecmp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:49:57
 * @note       :
 */
void route_ecmp_update(struct ecmp_group *pecmp_old, struct ecmp_group *pecmp_new)
{
    ROUTE_LOG_DEBUG();

    if ((NULL == pecmp_old) || (NULL == pecmp_new))
    {
        return ;
    }

    pecmp_new->group_index = pecmp_old->group_index;

    route_add_msg_to_send_list(pecmp_new, sizeof(struct ecmp_group), 1, MODULE_ID_FTM, MODULE_ID_ROUTE,
                        IPC_TYPE_NHP, NHP_TYPE_ECMP, IPC_OPCODE_ADD, 0);

    pecmp_old->frr_flag   = pecmp_new->frr_flag;
    pecmp_old->group_type = pecmp_new->group_type;

    return;
}


/**
 * @brief      : ecmp 删除
 * @param[in ] : pecmp - ecmp 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:51:58
 * @note       :
 */
int route_ecmp_delete(struct ecmp_group *pecmp)
{
    struct hash_bucket *pitem        = NULL;
    struct ecmp_group  *pecmp_lookup = NULL;

    ROUTE_LOG_DEBUG();

    if (NULL == pecmp)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pitem = hios_hash_find(&route_ecmp_table, pecmp);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    pecmp_lookup = (struct ecmp_group *)pitem->data;

    route_add_msg_to_send_list(pecmp_lookup, sizeof(struct ecmp_group), 1, MODULE_ID_FTM,
                        MODULE_ID_ROUTE, IPC_TYPE_NHP, NHP_TYPE_ECMP, IPC_OPCODE_DELETE, 0);

    hios_hash_delete(&route_ecmp_table, pitem);
    index_free(INDEX_TYPE_NHP, pecmp_lookup->group_index);
    XFREE(MTYPE_ECMP_ENTRY, pitem->data);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ecmp 查找
 * @param[in ] : pecmp - ecmp 结构
 * @param[out] :
 * @return     : 成功返回 ecmp 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月26日 17:52:35
 * @note       :
 */
struct ecmp_group *route_ecmp_lookup(struct ecmp_group *pecmp)
{
    struct hash_bucket *pitem = NULL;

    ROUTE_LOG_DEBUG();

    if (NULL == pecmp)
    {
        return NULL;
    }

    pitem = hios_hash_find(&route_ecmp_table, pecmp);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct ecmp_group *)pitem->data;
}


/**
 * @brief      : 下一跳分离处理
 * @param[in ] : proute - 活跃路由结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 18:08:11
 * @note       :
 */
int route_nhp_add_process(struct route_entry *proute)
{
    struct nhp_entry  *pnhp_lookup  = NULL;
    struct nhp_entry  *pnhp         = NULL;
    struct nhp_info   *pinfo        = NULL;
    struct nhp_info    info;
    struct ecmp_group *pecmp        = NULL;
    struct ecmp_group *pecmp_lookup = NULL;
    struct ecmp_group  ecmp;
    int i;

    ROUTE_LOG_DEBUG();

    if (proute == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memset(&info, 0, sizeof(struct nhp_info));
    memset(&ecmp, 0, sizeof(struct ecmp_group));

    /* 分离所有下一跳 */
    for (i=0; i<proute->nhp_num; i++)
    {
        pnhp_lookup = route_nexthop_lookup(&proute->nhp[i]);
        if (NULL == pnhp_lookup)
        {
            /* 创建 nexthop */
            pnhp = route_nexthop_create(&proute->nhp[i]);
            if (NULL == pnhp)
            {
                ROUTE_LOG_ERROR("create nexthop\n");

                return ERRNO_FAIL;
            }

            ecmp.nhp[i].nhp_index = pnhp->nhp_index;
            ecmp.nhp[i].nhp_type  = pnhp->nhp_type;
            ecmp.nhp_num++;

            info.nhp_index = pnhp->nhp_index;
            info.nhp_type  = pnhp->nhp_type;
            info.pnhp      = pnhp;

            /* 创建 nhp */
            pinfo = route_nhp_create(&info);
            if (NULL == pinfo)
            {
                ROUTE_LOG_ERROR("create nhp\n");

                route_nexthop_delete(pnhp);

                return ERRNO_FAIL;
            }
        }
        else
        {
            /* 更新 nexthop */
            ecmp.nhp[i].nhp_index = pnhp_lookup->nhp_index;
            ecmp.nhp[i].nhp_type  = pnhp_lookup->nhp_type;
            ecmp.nhp_num++;

            pinfo = route_nhp_lookup(pnhp_lookup->nhp_index);
            if (NULL == pinfo)
            {
                ROUTE_LOG_ERROR("pinfo is NULL !!!\n");

                return ERRNO_FAIL;
            }

            pinfo->refcount++;

            /* 新添加的 nhp 优先级高，触发更新 */
            if (proute->nhp[i].distance < pnhp_lookup->distance)
            {
                route_nexthop_update(pnhp_lookup, &proute->nhp[i]);

                pinfo->nhp_type = pnhp_lookup->nhp_type;
            }
        }
    }

    /* nhp 数量大于 1，创建 ecmp 组 */
    if (proute->nhp_num > NHP_NUM_ONE)
    {
        if ((proute->nhp_num == NHP_NUM_TWO)
            && ((proute->nhp[0].distance != proute->nhp[1].distance)
                || (proute->nhp[0].cost != proute->nhp[1].cost)))
        {
            ecmp.group_type = ECMP_TYPE_FRR;
            ecmp.frr_flag   = 0;
            info.nhp_type   = NHP_TYPE_FRR;
        }
        else
        {
            ecmp.group_type = ECMP_TYPE_ECMP;
            info.nhp_type   = NHP_TYPE_ECMP;
        }

        /* 对 ecmp 相关操作 */
        pecmp_lookup = route_ecmp_lookup(&ecmp);
        if (NULL == pecmp_lookup)
        {
            /* 创建 ecmp */
            pecmp = route_ecmp_create(&ecmp);
            if (NULL == pecmp)
            {
                ROUTE_LOG_ERROR("create ecmp\n");

                return ERRNO_FAIL;
            }

            info.nhp_index = pecmp->group_index;
            info.pnhp      = pecmp;

            /* 创建 nhp */
            pinfo = route_nhp_create(&info);
            if (NULL == pinfo)
            {
                ROUTE_LOG_ERROR("create ecmp nhp\n");

                route_ecmp_delete(pecmp);

                return ERRNO_FAIL;
            }
        }
        else
        {
            /* 更新 ecmp */
            pinfo = route_nhp_lookup(pecmp_lookup->group_index);
            if (NULL == pinfo)
            {
                ROUTE_LOG_ERROR("Assert !!!\n");

                return ERRNO_FAIL;
            }

            pinfo->nhp_type = info.nhp_type;
            pinfo->refcount++;
        }
    }

    /* 更新后优选 nhp 与原来相同，减去引用计数 */
    if (proute->nhpinfo.nhp_index == pinfo->nhp_index)
    {
        route_nhp_delete_process(proute);

        return ERRNO_NOT_UPDATE;
    }

    proute->nhpinfo.nhp_type  = pinfo->nhp_type;
    proute->nhpinfo.nhp_index = pinfo->nhp_index;

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 下一跳删除处理
 * @param[in ] : proute - 活跃路由结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月26日 18:11:12
 * @note       :
 */
int route_nhp_delete_process(struct route_entry *proute)
{
    struct nhp_info   *pinfo     = NULL;
    struct nhp_info   *pinfo_del = NULL;
    struct ecmp_group *pecmp     = NULL;
    struct ecmp_group  ecmp_del;
    int i;

    ROUTE_LOG_DEBUG();

    if (NULL == proute)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memset(&ecmp_del, 0, sizeof(struct ecmp_group));

    pinfo = route_nhp_lookup(proute->nhpinfo.nhp_index);
    if (pinfo == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pinfo->refcount--;
    if (0 == pinfo->refcount)        // 引用计数为0时，删除 nhp
    {
        if ((pinfo->nhp_type == NHP_TYPE_CONNECT)
            || (pinfo->nhp_type == NHP_TYPE_IP)
            || (pinfo->nhp_type == NHP_TYPE_LSP)
            || (pinfo->nhp_type == NHP_TYPE_TUNNEL))     // 单下一跳直接删除 nhp
        {
            route_nexthop_delete(pinfo->pnhp);
            route_nhp_delete(pinfo->nhp_index);
        }
        else if ((pinfo->nhp_type == NHP_TYPE_ECMP)
                || (pinfo->nhp_type == NHP_TYPE_FRR))    // 多下一跳逐条删除 nhp
        {
            pecmp = (struct ecmp_group *)pinfo->pnhp;

            memcpy(&ecmp_del, pecmp, sizeof(struct ecmp_group));

            route_ecmp_delete(pecmp);

            for (i=0; i<ecmp_del.nhp_num; i++)
            {
                pinfo_del = route_nhp_lookup(ecmp_del.nhp[i].nhp_index);
                if (NULL != pinfo_del)
                {
                    pinfo_del->refcount--;

                    if (0 == pinfo_del->refcount)
                    {
                        route_nexthop_delete(pinfo_del->pnhp);
                        route_nhp_delete(pinfo_del->nhp_index);
                    }
                }
            }

            route_nhp_delete(pinfo->nhp_index);
        }
    }
    else                            // 引用计数不为0
    {
        /* 下一跳类型为 ecmp、frr 时，减掉被引用 nhp 的引用计数 */
        if ((pinfo->nhp_type == NHP_TYPE_ECMP)
            || (pinfo->nhp_type == NHP_TYPE_FRR))
        {
            pecmp = (struct ecmp_group *)pinfo->pnhp;
            for (i=0; i<pecmp->nhp_num; i++)
            {
                pinfo_del = route_nhp_lookup(pecmp->nhp[i].nhp_index);
                if (NULL != pinfo_del)
                {
                    pinfo_del->refcount--;
                }
            }
        }
    }

    return ERRNO_SUCCESS;
}


