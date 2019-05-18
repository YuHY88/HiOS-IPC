/**
 * @file      : l3vpn.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 11:31:59
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/memory.h>
#include <lib/log.h>
#include <lib/prefix.h>
#include <lib/errcode.h>
#include "mpls_main.h"
#include "labelm.h"
#include "lspm.h"
#include "l3vpn.h"
#include "tunnel.h"


struct hash_table l3vpn_table;


/**
 * @brief      : 计算 l3vpn 哈希表 key 值
 * @param[in ] : hash_key - l3vpn 实例号
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:32:48
 * @note       :
 */
static uint32_t mpls_l3vpn_compute_hash(void *hash_key)
{
    return (uint32_t)hash_key;
}


/**
 * @brief      : 比较 l3vpn 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - l3vpn 实例号
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:33:43
 * @note       :
 */
static int mpls_l3vpn_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;

    if ((pitem == NULL) || (pbucket->hash_key == NULL) || (hash_key == NULL))
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
 * @brief      : l3vpn 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:41:44
 * @note       :
 */
void mpls_l3vpn_table_init(uint32_t size)
{
    hios_hash_init(&l3vpn_table, size, mpls_l3vpn_compute_hash, mpls_l3vpn_compare);

    return;
}


/**
 * @brief      : l3vpn 结构释放
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:42:05
 * @note       :
 */
void mpls_l3vpn_free(struct l3vpn_entry *pl3vpn)
{
    if (NULL != pl3vpn)
    {
        XFREE(MTYPE_VRF, pl3vpn);
    }

    return;
}


/**
 * @brief      : l3vpn 结构创建
 * @param[in ] : vrf_id - vpn 实例号
 * @param[out] :
 * @return     : 成功返回分配内存的 l3vpn 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:42:24
 * @note       :
 */
struct l3vpn_entry *mpls_l3vpn_create(uint16_t vrf_id)
{
    struct l3vpn_entry *pl3vpn = NULL;

    MPLS_LOG_DEBUG();

    if (0 == vrf_id)
    {
        return NULL;
    }

    pl3vpn = (struct l3vpn_entry *)XCALLOC(MTYPE_VRF, sizeof(struct l3vpn_entry));
    if (NULL == pl3vpn)
    {
        MPLS_LOG_ERROR("malloc\n");

        return NULL;
    }

    pl3vpn->label_apply            = LABEL_APPLY_PER_ROUTE;
    pl3vpn->l3vpn.vrf_id           = vrf_id;
    pl3vpn->route_tree.max_key_len = IPV4_MAX_PREFIXLEN;

    return pl3vpn;
}


/**
 * @brief      : l3vpn 添加
 * @param[in ] : pl3vpn - l3vpn 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:43:26
 * @note       :
 */
int mpls_l3vpn_add(struct l3vpn_entry *pl3vpn)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (NULL == pl3vpn)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (l3vpn_table.hash_size == l3vpn_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        MPLS_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)(int)pl3vpn->l3vpn.vrf_id;
    pitem->data     = pl3vpn;

    hios_hash_add(&l3vpn_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 删除
 * @param[in ] : vrf_id - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:44:07
 * @note       :
 */
int mpls_l3vpn_delete(uint16_t vrf_id)
{
    struct hash_bucket *pitem     = NULL;
    struct l3vpn_entry *pl3vpn    = NULL;
    struct listnode    *pnode     = NULL;
    struct listnode    *pnextnode = NULL;
    uchar              *ptarget   = NULL;

    MPLS_LOG_DEBUG();

    if (0 == vrf_id)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pitem = hios_hash_find(&l3vpn_table, (void *)(int)vrf_id);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    pl3vpn = (struct l3vpn_entry *)pitem->data;
    if (NULL != pl3vpn)
    {
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

        mpls_l3vpn_route_tree_free(&pl3vpn->route_tree);

        if ((0 != pl3vpn->inlabel) && (LABEL_APPLY_PER_VPN == pl3vpn->label_apply))
        {
            label_free(pl3vpn->inlabel);
        }

        XFREE(MTYPE_VRF, pl3vpn);
    }

    hios_hash_delete(&l3vpn_table, pitem);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 查找
 * @param[in ] : vrf_id - vpn 实例号
 * @param[out] :
 * @return     : 成功返回 l3vpn 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:44:48
 * @note       :
 */
struct l3vpn_entry *mpls_l3vpn_lookup(uint16_t vrf_id)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (0 == vrf_id)
    {
        return NULL;
    }

    pitem = hios_hash_find(&l3vpn_table, (void *)(int)vrf_id);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct l3vpn_entry *)pitem->data;
}


/**
 * @brief      : l3vpn 添加私网 lsp
 * @param[in ] : pl3vpn  - l3vpn 结构
 * @param[in ] : pvroute - l3vpn 路由结构
 * @param[in ] : pnhp    - l3vpn 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:46:49
 * @note       :
 */
static int mpls_l3vpn_route_add_lsp(struct l3vpn_entry *pl3vpn,
                                                struct l3vpn_route *pvroute,
                                                struct l3vpn_nhp *pnhp)
{
    struct lsp_entry *plspm = NULL;
    int ret = ERRNO_SUCCESS;

    MPLS_LOG_DEBUG();

    if ((NULL == pl3vpn) || (NULL == pvroute) || (NULL == pnhp))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if ((LABEL_APPLY_PER_VPN == pl3vpn->label_apply)
        && (pl3vpn->inlabel == pnhp->inlabel))
    {
        return ERRNO_SUCCESS;
    }

    plspm = mpls_lsp_new(pnhp->inlabel, pnhp->outlabel, LSP_TYPE_L3VPN);
    if (NULL == plspm)
    {
        MPLS_LOG_ERROR("get lsp new\n");

        return ERRNO_MALLOC;
    }

    pnhp->lsp_index   = plspm->lsp_index;
    plspm->vpnid      = pvroute->vrf_id;
    plspm->nhp_index  = pnhp->tunnel_if;
    plspm->phb_enable = ENABLE;
    plspm->nhp_type   = NHP_TYPE_TUNNEL;

    ret = mpls_lsp_add(plspm);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("lsp add failed\n");

        mpls_lsp_free(plspm);
        mpls_lsp_free_index(plspm->lsp_index);

        pnhp->lsp_index = 0;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 删除私网 lsp
 * @param[in ] : pnhp - l3vpn 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:48:21
 * @note       :
 */
static int mpls_l3vpn_route_delete_lsp(struct l3vpn_nhp *pnhp)
{
    MPLS_LOG_DEBUG();

    if (NULL == pnhp)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (0 != pnhp->lsp_index)
    {
        mpls_lsp_delete(pnhp->lsp_index);
        mpls_lsp_free_index(pnhp->lsp_index);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 实例添加私网 lsp
 * @param[in ] : pl3vpn - l3vpn 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月23日 10:42:16
 * @note       : vpn 实例共用相同入标签时调用
 */
static int mpls_l3vpn_add_lsp(struct l3vpn_entry *pl3vpn)
{
    struct lsp_entry *plspm = NULL;
    int ret = ERRNO_SUCCESS;

    MPLS_LOG_DEBUG();

    if (NULL == pl3vpn)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plspm = mpls_lsp_new(pl3vpn->inlabel, 0, LSP_TYPE_L3VPN);
    if (NULL == plspm)
    {
        MPLS_LOG_ERROR("get lsp new\n");

        return ERRNO_MALLOC;
    }

    plspm->vpnid      = pl3vpn->l3vpn.vrf_id;
    plspm->phb_enable = ENABLE;

    ret = mpls_lsp_add(plspm);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("lsp add failed\n");

        mpls_lsp_free(plspm);
        mpls_lsp_free_index(plspm->lsp_index);
    }

    pl3vpn->lsp_index = plspm->lsp_index;

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 实例删除私网 lsp
 * @param[in ] : pl3vpn - l3vpn 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月23日 10:55:21
 * @note       : vpn 实例共用相同入标签时调用
 */
int mpls_l3vpn_delete_lsp(struct l3vpn_entry *pl3vpn)
{
    MPLS_LOG_DEBUG();

    if (NULL == pl3vpn)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (0 != pl3vpn->lsp_index)
    {
        mpls_lsp_delete(pl3vpn->lsp_index);
        mpls_lsp_free_index(pl3vpn->lsp_index);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 路由获取标签
 * @param[in ] : vrf_id - vpn 实例号
 * @param[out] :
 * @return     : 成功返回标签值，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:45:41
 * @note       :
 */
uint32_t mpls_l3vpn_route_get_label(uint16_t vrf_id)
{
    struct l3vpn_entry *pl3vpn = NULL;
    uint32_t inlabel = 0;

    MPLS_LOG_DEBUG();

    if (0 == vrf_id)
    {
        return 0;
    }

    pl3vpn = mpls_l3vpn_lookup(vrf_id);
    if (NULL == pl3vpn)
    {
        MPLS_LOG_ERROR("vpn route is NULL\n");

        return 0;
    }

    if (pl3vpn->label_apply == LABEL_APPLY_PER_VPN)
    {
        if (pl3vpn->inlabel != 0)
        {
            return pl3vpn->inlabel;
        }
    }

    inlabel = label_alloc(MODULE_ID_L3VPN);
    if (0 == inlabel)
    {
        MPLS_LOG_ERROR("label alloc failed\n");

        return 0;
    }

    if (pl3vpn->label_apply == LABEL_APPLY_PER_VPN)
    {
        pl3vpn->inlabel = inlabel;

        mpls_l3vpn_add_lsp(pl3vpn);
    }

    return inlabel;
}


/**
 * @brief      : l3vpn 路由释放标签
 * @param[in ] : pnhp - l3vpn 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:46:23
 * @note       :
 */
int mpls_l3vpn_route_free_label(struct l3vpn_nhp *pnhp)
{
    MPLS_LOG_DEBUG();

    if (NULL == pnhp)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    label_free(pnhp->inlabel);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 填充路由结构
 * @param[in ] : proute  - 活跃路由结构
 * @param[in ] : pvroute - l3vpn 路由结构
 * @param[in ] : pnhp    - l3vpn 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:48:56
 * @note       :
 */
static int mpls_l3vpn_route_set_route(struct route_entry *proute,
                                                    struct l3vpn_route *pvroute,
                                                    struct l3vpn_nhp *pnhp)
{
    MPLS_LOG_DEBUG();

    if ((NULL== proute) || (NULL == pvroute) || (NULL == pnhp))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    proute->prefix           = pvroute->prefix;
    proute->vpn              = pvroute->vrf_id;
    proute->nhp_num          = 1;
    proute->nhp[0].distance  = pnhp->distance;
    proute->nhp[0].cost      = pnhp->cost;
    proute->nhp[0].down_flag = LINK_DOWN;
    proute->nhp[0].nexthop   = pnhp->nexthop;
    proute->nhp[0].vpn       = pvroute->vrf_id;
    proute->nhp[0].protocol  = pnhp->protocol;
    proute->nhp[0].action    = NHP_ACTION_FORWARD;
    proute->nhp[0].nhp_type  = NHP_TYPE_IP;

    if ((pnhp->outlabel != 0) && (pnhp->inlabel == 0))
    {
        proute->nhp[0].ifindex  = pnhp->lsp_index;
        proute->nhp[0].nhp_type = NHP_TYPE_LSP;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 路由下一跳结构比较
 * @param[in ] : val1 - l3vpn 下一跳结构
 * @param[in ] : val2 - l3vpn 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:50:06
 * @note       :
 */
static int mpls_l3vpn_route_nhp_cmp(void *val1, void *val2)
{
    struct l3vpn_nhp *pnhp_new = (struct l3vpn_nhp *)val1;
    struct l3vpn_nhp *pnhp_old = (struct l3vpn_nhp *)val2;

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
        if (pnhp_new->nexthop.addr.ipv4 < pnhp_old->nexthop.addr.ipv4)
        {
            return -1;
        }
    }

    return 0;
}


/**
 * @brief      : l3vpn 路由下一跳添加
 * @param[in ] : plist - 下一跳链表
 * @param[in ] : pnhp  - l3vpn 下一跳结构
 * @param[out] :
 * @return     : 成功返回加入链表的 l3vpn 下一跳结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 11:50:49
 * @note       :
 */
struct l3vpn_nhp *mpls_l3vpn_route_nhplist_add(struct list *plist, struct l3vpn_nhp *pnhp)
{
    struct l3vpn_nhp *pnhp_new = NULL;

    MPLS_LOG_DEBUG();

    if ((NULL== plist) || (NULL == pnhp))
    {
        return NULL;
    }

    pnhp_new = (struct l3vpn_nhp *)XCALLOC(MTYPE_VRF, sizeof(struct l3vpn_nhp));
    if (NULL == pnhp_new)
    {
        MPLS_LOG_ERROR("malloc\n");

        return NULL;
    }

    memcpy(pnhp_new, pnhp, sizeof(struct l3vpn_nhp));
    pnhp_new->down_flag = LINK_DOWN;

    listnode_add_sort(plist, pnhp_new);

    return pnhp_new;
}


/**
 * @brief      : l3vpn 路由下一跳删除
 * @param[in ] : plist - 下一跳链表
 * @param[in ] : pnhp  - l3vpn 下一跳结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 13:58:10
 * @note       :
 */
int mpls_l3vpn_route_nhplist_delete(struct list *plist, struct l3vpn_nhp *pnhp)
{
    MPLS_LOG_DEBUG();

    if ((NULL== plist) || (NULL == pnhp))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    listnode_delete(plist, pnhp);
    XFREE(MTYPE_VRF, pnhp);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 路由下一跳查找
 * @param[in ] : plist - 下一跳链表
 * @param[in ] : pnhp  - l3vpn 下一跳结构
 * @param[out] :
 * @return     : 成功返回 l3vpn 下一跳结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 13:58:53
 * @note       :
 */
struct l3vpn_nhp *mpls_l3vpn_route_nhp_lookup(struct list *plist, struct l3vpn_nhp *pnhp)
{
    struct listnode   *pnode       = NULL;
    struct l3vpn_nhp  *pnhp_lookup = NULL;

    MPLS_LOG_DEBUG();

    if ((NULL== plist) || (NULL == pnhp))
    {
        return NULL;
    }

    for (ALL_LIST_ELEMENTS_RO(plist, pnode, pnhp_lookup))
    {
        if (NULL == pnhp_lookup)
        {
            continue;
        }

        if ((pnhp->nexthop.type == pnhp_lookup->nexthop.type)
            && (pnhp->nexthop.addr.ipv4 == pnhp_lookup->nexthop.addr.ipv4)
            && (pnhp->inlabel == pnhp_lookup->inlabel)
            && (pnhp->outlabel == pnhp_lookup->outlabel))
        {
            return pnhp_lookup;
        }
    }

    return NULL;
}


/**
 * @brief      : l3vpn 路由添加
 * @param[in ] : proute_tree - 路由树结构
 * @param[in ] : pvroute     - l3vpn 路由结构
 * @param[out] :
 * @return     : 成功返回加入 tree 的 l3vpn 路由结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 13:59:42
 * @note       :
 */
struct l3vpn_route *mpls_l3vpn_route_tree_add(struct ptree *proute_tree,
                                                            struct l3vpn_route *pvroute)
{
    struct ptree_node  *pnode       = NULL;
    struct l3vpn_route *pvroute_new = NULL;

    MPLS_LOG_DEBUG();

    if ((NULL == pvroute) || (NULL == proute_tree))
    {
        return NULL;
    }

    pnode = ptree_node_get(proute_tree, (uchar *)&pvroute->prefix.addr.ipv4,
                            pvroute->prefix.prefixlen);
    if (pnode == NULL)
    {
        MPLS_LOG_ERROR("ptree get failed.\n");

        return NULL;
    }

    if (pnode->info == NULL)
    {
        MPLS_LOG_DEBUG("Add route\n");

        pvroute_new = (struct l3vpn_route *)XCALLOC(MTYPE_VRF, sizeof(struct l3vpn_route));
        if (NULL == pvroute_new)
        {
            MPLS_LOG_ERROR("route info malloc failed.\n");

            return NULL;
        }

        memcpy(pvroute_new, pvroute, sizeof(struct l3vpn_route));
        pvroute_new->nhplist.cmp = mpls_l3vpn_route_nhp_cmp;

        pnode->info = (void *)pvroute_new;
    }

    return (struct l3vpn_route *)pnode->info;
}


/**
 * @brief      : l3vpn 路由删除
 * @param[in ] : proute_tree - 路由树结构
 * @param[in ] : prefix      - 路由前缀结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:01:58
 * @note       :
 */
int mpls_l3vpn_route_tree_delete(struct ptree *proute_tree, struct inet_prefix *prefix)
{
    struct ptree_node  *pnode   = NULL;
    struct l3vpn_route *pvroute = NULL;

    MPLS_LOG_DEBUG();

    if ((NULL == prefix) || (NULL == proute_tree))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pnode = ptree_node_lookup(proute_tree, (u_char *)&prefix->addr.ipv4, prefix->prefixlen);
    if ((NULL == pnode) || (NULL == pnode->info))
    {
        return ERRNO_SUCCESS;
    }

    pvroute = (struct l3vpn_route *)pnode->info;

    pnode->info = NULL;
    pnode->lock = 0;

    ptree_node_delete(pnode);
    XFREE(MTYPE_VRF, pvroute);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 路由查找
 * @param[in ] : proute_tree - 路由树结构
 * @param[in ] : prefix      - 路由前缀结构
 * @param[out] :
 * @return     : 成功返回 l3vpn 路由结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:03:26
 * @note       :
 */
struct l3vpn_route *mpls_l3vpn_route_tree_lookup(struct ptree *proute_tree,
                                                                struct inet_prefix *prefix)
{
    struct ptree_node *pnode = NULL;

    MPLS_LOG_DEBUG();

    if ((NULL == prefix) || (NULL == proute_tree))
    {
        return NULL;
    }

    pnode = ptree_node_lookup(proute_tree, (u_char *)&prefix->addr.ipv4, prefix->prefixlen);
    if ((NULL == pnode) || (NULL == pnode->info))
    {
        return NULL;
    }

    return (struct l3vpn_route *)pnode->info;
}


/**
 * @brief      : l3vpn 路由添加消息
 * @param[in ] : pvroute - l3vpn 路由结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:05:40
 * @note       :
 */
int mpls_l3vpn_route_node_add(struct l3vpn_route *pvroute)
{
    struct l3vpn_entry *pl3vpn      = NULL;
    struct l3vpn_route *pvroute_new = NULL;
    struct l3vpn_nhp   *pnhp_new    = NULL;
    struct tunnel_if   *pif         = NULL;
    struct route_entry  route;
    int ret = ERRNO_SUCCESS;

    MPLS_LOG_DEBUG();

    if (NULL == pvroute)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pl3vpn = mpls_l3vpn_lookup(pvroute->vrf_id);
    if (NULL == pl3vpn)
    {
        MPLS_LOG_ERROR("vpn entry is NULL.\n");

        return ERRNO_NOT_FOUND;
    }

    /* 添加路由信息到 route_tree */
    pvroute_new = mpls_l3vpn_route_tree_lookup(&pl3vpn->route_tree, &pvroute->prefix);
    if (NULL == pvroute_new)
    {
        pvroute_new = mpls_l3vpn_route_tree_add(&pl3vpn->route_tree, pvroute);
        if (NULL == pvroute_new)
        {
            MPLS_LOG_ERROR("vpn route add failed.\n");

            return ERRNO_MALLOC;
        }
    }

    /* 添加下一跳信息到链表 */
    if (NULL != mpls_l3vpn_route_nhp_lookup(&pvroute_new->nhplist, &pvroute->nhp))
    {
        return ERRNO_EXISTED;
    }

    pnhp_new = mpls_l3vpn_route_nhplist_add(&pvroute_new->nhplist, &pvroute->nhp);
    if (NULL == pnhp_new)
    {
        MPLS_LOG_ERROR("vpn route nhp add failed.\n");

        return ERRNO_MALLOC;
    }

    /* 出标签路由下发 up 路由，入标签路由下发非引入路由 */
    if (pnhp_new->outlabel != 0)
    {
        /* 隧道 up ，携带出标签的下路由 */
        pif = tunnel_if_lookup(pnhp_new->tunnel_if);
        if ((NULL == pif)
            || (LINK_DOWN == pif->tunnel.down_flag))
        {
            return ERRNO_SUCCESS;
        }

        pnhp_new->down_flag = LINK_UP;

        /* 添加私网 l3vpn lsp */
        ret = mpls_l3vpn_route_add_lsp(pl3vpn, pvroute_new, pnhp_new);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("add lsp failed\n");

            return ret;
        }

        /* 只有出标签时下路由 */
        if (pnhp_new->inlabel == 0)
        {
            memset(&route, 0, sizeof(struct route_entry));

            mpls_l3vpn_route_set_route(&route, pvroute_new, pnhp_new);
            route_com_update(&route, IPC_OPCODE_ADD, MODULE_ID_MPLS);
        }
    }
    else if ((pnhp_new->outlabel == 0) && (pnhp_new->inlabel != 0))
    {
        pnhp_new->down_flag = LINK_UP;

        /* 添加私网 l3vpn lsp */
        ret = mpls_l3vpn_route_add_lsp(pl3vpn, pvroute_new, pnhp_new);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("add lsp failed\n");

            return ret;
        }

        /* 非引入 nhp 下发路由 */
        if (pnhp_new->import_flag == 0)
        {
            memset(&route, 0, sizeof(struct route_entry));

            mpls_l3vpn_route_set_route(&route, pvroute_new, pnhp_new);
            route_com_update(&route, IPC_OPCODE_ADD, MODULE_ID_MPLS);
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 路由删除消息
 * @param[in ] : pvroute - l3vpn 路由结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:09:22
 * @note       :
 */
int mpls_l3vpn_route_node_delete(struct l3vpn_route *pvroute)
{
    struct l3vpn_entry *pl3vpn         = NULL;
    struct l3vpn_route *pvroute_lookup = NULL;
    struct l3vpn_nhp   *pnhp_lookup    = NULL;
    struct route_entry  route;

    MPLS_LOG_DEBUG();

    if (NULL == pvroute)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pl3vpn = mpls_l3vpn_lookup(pvroute->vrf_id);
    if (NULL == pl3vpn)
    {
        return ERRNO_SUCCESS;
    }

    pvroute_lookup = mpls_l3vpn_route_tree_lookup(&pl3vpn->route_tree, &pvroute->prefix);
    if (NULL == pvroute_lookup)
    {
        return ERRNO_SUCCESS;
    }

    pnhp_lookup = mpls_l3vpn_route_nhp_lookup(&pvroute_lookup->nhplist, &pvroute->nhp);
    if (NULL == pnhp_lookup)
    {
        return ERRNO_SUCCESS;
    }

    /* 出标签路由下发 up 路由，入标签路由下发非引入路由 */
    if ((pnhp_lookup->outlabel != 0) && (LINK_UP == pnhp_lookup->down_flag))
    {
        pnhp_lookup->down_flag = LINK_DOWN;

        /* 只有出标签时下路由 */
        if (pnhp_lookup->inlabel == 0)
        {
            memset(&route, 0, sizeof(struct route_entry));

            mpls_l3vpn_route_set_route(&route, pvroute_lookup, pnhp_lookup);
            route_com_update(&route, IPC_OPCODE_DELETE, MODULE_ID_MPLS);
        }

        mpls_l3vpn_route_delete_lsp(pnhp_lookup);
    }
    else if ((pnhp_lookup->outlabel == 0) && (pnhp_lookup->inlabel != 0))
    {
        pnhp_lookup->down_flag = LINK_DOWN;

        /* 非引入 nhp 下发路由 */
        if (pnhp_lookup->import_flag == 0)
        {
            memset(&route, 0, sizeof(struct route_entry));

            mpls_l3vpn_route_set_route(&route, pvroute_lookup, pnhp_lookup);
            route_com_update(&route, IPC_OPCODE_DELETE, MODULE_ID_MPLS);
        }

        mpls_l3vpn_route_delete_lsp(pnhp_lookup);
    }

    /* 从链表删除下一跳信息 */
    mpls_l3vpn_route_nhplist_delete(&pvroute_lookup->nhplist, pnhp_lookup);

    /* 释放入标签 */
    if (pl3vpn->label_apply == LABEL_APPLY_PER_ROUTE)
    {
        mpls_l3vpn_route_free_label(pnhp_lookup);
    }

    if (list_isempty(&pvroute_lookup->nhplist))
    {
        mpls_l3vpn_route_tree_delete(&pl3vpn->route_tree, &pvroute_lookup->prefix);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 路由释放
 * @param[in ] : proute_tree - 路由树结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:11:47
 * @note       :
 */
void mpls_l3vpn_route_tree_free(struct ptree *proute_tree)
{
    struct l3vpn_route *pvroute    = NULL;
    struct l3vpn_nhp   *pnhp       = NULL;
    struct ptree_node  *pnode1     = NULL;
    struct ptree_node  *pnode2     = NULL;
    struct listnode    *pnode      = NULL;
    struct listnode    *pnode_next = NULL;
    struct l3vpn_route  vroute;

    MPLS_LOG_DEBUG();

    if (NULL == proute_tree)
    {
        return;
    }

    for (pnode2 = ptree_top(proute_tree); pnode2 != NULL;)
    {
        pnode1 = pnode2;
		pnode2 = ptree_next(pnode2);

        pvroute = pnode1->info;
        if (NULL == pvroute)
		{
			continue;
		}

        for (ALL_LIST_ELEMENTS(&pvroute->nhplist, pnode, pnode_next, pnhp))
        {
            if(NULL != pnhp)
            {
                memcpy(&vroute, pvroute, sizeof(struct l3vpn_route));
                memcpy(&vroute.nhp, pnhp, sizeof(struct l3vpn_nhp));

    		    mpls_l3vpn_route_node_delete(&vroute);
            }
        }
	}

    return;
}


/**
 * @brief      : tunnel 接口 up/down 处理 l3vpn 路由
 * @param[in ] : pl3vpn  - l3vpn 结构
 * @param[in ] : pvroute - l3vpn 路由结构
 * @param[in ] : pnhp    - l3vpn 下一跳结构
 * @param[in ] : opcode  - 操作码
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:14:26
 * @note       :
 */
void mpls_l3vpn_process_route(struct l3vpn_entry *pl3vpn,
                                            struct l3vpn_route *pvroute,
                                            struct l3vpn_nhp *pnhp,
                                            enum OPCODE_E opcode)
{
    struct route_entry route;

    MPLS_LOG_DEBUG();

    if ((NULL == pl3vpn) || (NULL == pvroute) || (NULL == pnhp))
    {
        return;
    }

    /* 出标签不为 0 下发 lsp */
    if (0 != pnhp->outlabel)
    {
        if (OPCODE_DOWN == opcode)
        {
            pnhp->down_flag = LINK_DOWN;

            mpls_l3vpn_route_delete_lsp(pnhp);
        }
        else if (OPCODE_UP == opcode)
        {
            pnhp->down_flag = LINK_UP;

            mpls_l3vpn_route_add_lsp(pl3vpn, pvroute, pnhp);
        }
    }

    /* 出标签不为 0 ，入标签为 0 ，下发路由 */
    if ((0 != pnhp->outlabel) && (0 == pnhp->inlabel))
    {
        memset(&route, 0, sizeof(struct route_entry));
        mpls_l3vpn_route_set_route(&route, pvroute, pnhp);

        if (OPCODE_DOWN == opcode)
        {
            route_com_update(&route, IPC_OPCODE_DELETE, MODULE_ID_MPLS);
        }
        else if (OPCODE_UP == opcode)
        {
            route_com_update(&route, IPC_OPCODE_ADD, MODULE_ID_MPLS);
        }
    }

    return;
}


/**
 * @brief      : l3vpn 响应 tunnel 接口 up
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:15:59
 * @note       :
 */
void mpls_l3vpn_process_tunnel_up(uint32_t ifindex)
{
    struct l3vpn_entry *pl3vpn    = NULL;
    struct hash_bucket *pbucket   = NULL;
    struct ptree_node  *ptreenode = NULL;
    struct listnode    *plistnode = NULL;
    struct l3vpn_route *pvroute   = NULL;
    struct l3vpn_nhp   *pnhp      = NULL;
    int cursor = 0;

    MPLS_LOG_DEBUG();

    if (0 == ifindex)
    {
        return;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, l3vpn_table)
    {
        pl3vpn = (struct l3vpn_entry *)pbucket->data;
        if (NULL == pl3vpn)
        {
            continue;
        }

        PTREE_LOOP(&pl3vpn->route_tree, pvroute, ptreenode)
        {
            for (ALL_LIST_ELEMENTS_RO(&pvroute->nhplist, plistnode, pnhp))
            {
                if ((ifindex == pnhp->tunnel_if) && (LINK_DOWN == pnhp->down_flag))
                {
                    mpls_l3vpn_process_route(pl3vpn, pvroute, pnhp, OPCODE_UP);
                }
            }
        }
    }

    return;
}


/**
 * @brief      : l3vpn 响应 tunnel 接口 down
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:16:47
 * @note       :
 */
void mpls_l3vpn_process_tunnel_down(uint32_t ifindex)
{
    struct l3vpn_entry *pl3vpn    = NULL;
    struct hash_bucket *pbucket   = NULL;
    struct ptree_node  *ptreenode = NULL;
    struct listnode    *plistnode = NULL;
    struct l3vpn_route *pvroute   = NULL;
    struct l3vpn_nhp   *pnhp      = NULL;
    int cursor = 0;

    MPLS_LOG_DEBUG();

    if (0 == ifindex)
    {
        return;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, l3vpn_table)
    {
        pl3vpn = (struct l3vpn_entry *)pbucket->data;
        if (NULL == pl3vpn)
        {
            continue;
        }

        PTREE_LOOP(&pl3vpn->route_tree, pvroute, ptreenode)
        {
            for (ALL_LIST_ELEMENTS_RO(&pvroute->nhplist, plistnode, pnhp))
            {
                if ((ifindex == pnhp->tunnel_if) && (LINK_UP == pnhp->down_flag))
                {
                    mpls_l3vpn_process_route(pl3vpn, pvroute, pnhp, OPCODE_DOWN);
                }
            }
        }
    }

    return;
}


/**
 * @brief      : 批量获取 l3vpn 实例
 * @param[in ] : vfr_id     - l3vpn 实例号
 * @param[out] : l3vpn_buff - 获取到的 l3vpn 实例数组
 * @return     : 返回获取到 l3vpn 实例的数量
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:17:37
 * @note       : 提供给 snmp 的 get next 接口，空索引从数据结构开始返回数据，
 * @note       : 非空索引从传入索引节点下一个节点开始返回数据
 */
int mpls_l3vpn_get_instance_bulk(uint32_t vrf_id, struct l3vpn_entry l3vpn_buff[])
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct l3vpn_entry *pl3vpn  = NULL;
    int msg_num  = IPC_MSG_LEN/sizeof(struct l3vpn_entry);
    int data_num = 0;
    int cursor;
    int val = 0;

    if (0 == vrf_id)
    {
        /* 初次获取 */
        HASH_BUCKET_LOOP(pbucket, cursor, l3vpn_table)
        {
            pl3vpn = (struct l3vpn_entry *)pbucket->data;
            if (NULL == pl3vpn)
            {
                continue;
            }

            memcpy(&l3vpn_buff[data_num++], pl3vpn, sizeof(struct l3vpn_entry));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&l3vpn_table, (void *)vrf_id);
        if (NULL == pbucket)
        {
            /*
             * 当前数据可能已经被删除，返回到当前哈希桶的链表头部
             * 如果当前哈希桶链表头为空，向后面的哈希桶继续查找
             */
            val = l3vpn_table.compute_hash((void *)vrf_id)%HASHTAB_SIZE;

            if (NULL != l3vpn_table.buckets[val])
            {
                pbucket = l3vpn_table.buckets[val];
            }
            else
            {
                for (++val; val<HASHTAB_SIZE; ++val)
            	{
            		if (NULL != l3vpn_table.buckets[val])
            		{
            			pbucket = l3vpn_table.buckets[val];
                        break;
            		}
                }
            }
        }

        if (NULL != pbucket)
        {
            for (data_num=0; data_num<msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&l3vpn_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                pl3vpn = pnext->data;
                memcpy(&l3vpn_buff[data_num], pl3vpn, sizeof(struct l3vpn_entry));
                pbucket = pnext;
            }
        }
    }

    return data_num;
}


/**
 * @brief      : 批量获取 l3vpn 实例内 target
 * @param[in ] : vrf_id   - l3vpn 实例号
 * @param[in ] : ptarget  - target 内容
 * @param[in ] : data_len - target 长度
 * @param[in ] : subtype  - import/export
 * @param[out] : target   - 获取到的 target 数组结构
 * @return     : 返回获取到 target 的数量
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:20:18
 * @note       : 获取 vpn 实例中 target，
 * @note       : 如果 vrf_id 为0，那么从哈希表遍历每个实例，
 * @note       : 依次返回各实例下的 target，
 * @note       : 先读取 import，再读取 export，
 * @note       : 如果 vrf_id 非0，那么找到 vrf_id 对应的 vpn 实例，
 * @note       : 再根据 ptarget、subtype 找到对应的 import 或 export，
 * @note       : 跳过当前匹配的 target ，从链表下一个节点开始返回数据。
 */
int mpls_l3vpn_get_target_bulk(uint32_t vrf_id, uchar *ptarget, int data_len,
                                            enum L3VPN_SUBTYPE subtype, struct l3vpn_target target[])
{
    struct list        *plist   = NULL;
    struct listnode    *pnode   = NULL;
    struct hash_bucket *pbucket = NULL;
    struct l3vpn_entry *pl3vpn  = NULL;
    uchar              *pdata   = NULL;
    int msg_len     = IPC_MSG_LEN/sizeof(struct l3vpn_target);
    int data_num    = 0;
    int l3vpn_flag  = 0;
    int import_flag = 0;
    int export_flag = 0;
    int cursor;

    if (0 == vrf_id)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, l3vpn_table)
        {
            pl3vpn = (struct l3vpn_entry *)pbucket->data;
            if (NULL == pl3vpn)
            {
                continue;
            }

            plist = &pl3vpn->import_list;
            for (ALL_LIST_ELEMENTS_RO(plist, pnode, pdata))
            {
                target[data_num].vrf_id      = pl3vpn->l3vpn.vrf_id;
                target[data_num].target_type = L3VPN_SUBTYPE_IMPORT;

                memcpy(target[data_num++].target, pdata, NAME_STRING_LEN);
                if (data_num == msg_len)
                {
                    return data_num;
                }
            }

            plist = &pl3vpn->export_list;
            for (ALL_LIST_ELEMENTS_RO(plist, pnode, pdata))
            {
                target[data_num].vrf_id      = pl3vpn->l3vpn.vrf_id;
                target[data_num].target_type = L3VPN_SUBTYPE_EXPORT;

                memcpy(target[data_num++].target, pdata, NAME_STRING_LEN);
                if (data_num == msg_len)
                {
                    return data_num;
                }
            }
        }
    }
    else
    {
        if ((0 == data_len)
            || ((subtype != L3VPN_SUBTYPE_IMPORT) && (subtype != L3VPN_SUBTYPE_EXPORT)))
        {
            return data_num;
        }

        HASH_BUCKET_LOOP(pbucket, cursor, l3vpn_table)
        {
            pl3vpn = (struct l3vpn_entry *)pbucket->data;
            if (NULL == pl3vpn)
            {
                continue;
            }

            if (pl3vpn->l3vpn.vrf_id == vrf_id)
            {
                l3vpn_flag = 1;
            }

            if (0 == l3vpn_flag)
            {
                continue;
            }

            if (L3VPN_SUBTYPE_IMPORT == subtype)
            {
                plist = &pl3vpn->import_list;
                for (ALL_LIST_ELEMENTS_RO(plist, pnode, pdata))
                {
                    if (1 == l3vpn_flag)
                    {
                        if ((strlen((char *)ptarget) == strlen((char *)pdata))
                        && (0 == memcmp(ptarget, pdata, strlen((char *)ptarget))))
                        {
                            import_flag = 1;

                            /* 匹配到 import，跳过匹配的 target，获取下一个 target */
                            continue;
                        }

                        if (0 == import_flag)
                        {
                            continue;
                        }
                    }

                    target[data_num].vrf_id      = pl3vpn->l3vpn.vrf_id;
                    target[data_num].target_type = L3VPN_SUBTYPE_IMPORT;

                    memcpy(target[data_num++].target, pdata, NAME_STRING_LEN);
                    if (data_num == msg_len)
                    {
                        return data_num;
                    }
                }

                subtype = L3VPN_SUBTYPE_EXPORT;

                /* 匹配到 import 后续不需要继续匹配，直接获取后面数据 */
                l3vpn_flag = 2;
            }

            if (L3VPN_SUBTYPE_EXPORT == subtype)
            {
                plist = &pl3vpn->export_list;
                for (ALL_LIST_ELEMENTS_RO(plist, pnode, pdata))
                {
                    if (1 == l3vpn_flag)
                    {
                        if ((strlen((char *)ptarget) == strlen((char *)pdata))
                        && (0 == memcmp(ptarget, pdata, strlen((char *)ptarget))))
                        {
                            export_flag = 1;

                            /* 匹配到 export target，获取下一个 target */
                            continue;
                        }

                        if (0 == export_flag)
                        {
                            continue;
                        }
                    }

                    target[data_num].vrf_id      = pl3vpn->l3vpn.vrf_id;
                    target[data_num].target_type = L3VPN_SUBTYPE_EXPORT;

                    memcpy(target[data_num++].target, pdata, NAME_STRING_LEN);
                    if (data_num == msg_len)
                    {
                        return data_num;
                    }
                }

                subtype = L3VPN_SUBTYPE_IMPORT;

                /* 匹配到 export 后续不需要继续匹配，直接获取后面数据 */
                l3vpn_flag = 2;
            }
        }
    }

    return data_num;
}


