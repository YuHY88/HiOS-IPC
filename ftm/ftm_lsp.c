/**
 * @file      : ftm_lsp.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 16:42:55
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/memory.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/ifm_common.h>
#include <lib/oam_common.h>
#include <mpls/mpls.h>
#include <lib/mpls_common.h>
#include "ftm.h"
#include "ftm_ilm.h"
#include "ftm_nhlfe.h"
#include "ftm_lsp.h"
#include "ftm_pw.h"
#include "ftm_vpls.h"
#include "ftm_tunnel.h"
#include "ftm_arp.h"
#include "ftm_ldp.h"

struct hash_table ftm_lsp_table;
struct list lsp_list;           // 需要学习 arp 的 lsp 链表
struct mpls_global g_mpls;


/**
 * @brief      : ftm mpls 模块初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:44:23
 * @note       :
 */
void ftm_mpls_init(void)
{
	memset(&g_mpls, 0, sizeof(struct mpls_global));
	g_mpls.pw_limit     = PW_NUM_MAX;
	g_mpls.lsp_limit    = LSP_NUM_MAX;
	g_mpls.tunnel_limit = TUNNEL_NUM_MAX;

	ftm_lsp_table_init(LSP_NUM_MAX);
	ftm_nhlfe_table_init(LSP_NUM_MAX);
	ftm_ilm_table_init(LSP_NUM_MAX);
	ftm_pw_table_init(PW_NUM_MAX);
	ftm_vsi_table_init(VSI_NUM_MAX);
    ftm_gre_table_init(IFM_NUM_MAX);
    ftm_ldp_frr_table_init(FTM_LDP_FRR_GROUP_MAX);
}


/**
 * @brief      : 下一跳方式 lsp 定时学习 arp 回调
 * @param[in ] : thread - 线程结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:44:43
 * @note       :
 */
static int ftm_traverse_lsp_list(void *para)
{
    struct listnode  *pnode = NULL;
    struct lsp_entry *plsp  = NULL;
    struct list      *plist = &lsp_list;

    FTM_LOG_DEBUG("list_count=%d.\n", lsp_list.count);

    for (ALL_LIST_ELEMENTS_RO(plist, pnode, plsp))
    {
        if (NULL != plsp)
        {
            arp_lookup_active(plsp->nexthop.addr.ipv4, plsp->vpnid);
        }
    }

	if (0 != lsp_list.count)
	{
        MPLS_TIMER_ADD(ftm_traverse_lsp_list, NULL, TRAVERSE_LSP_LIST_TIME);
	}

    return ERRNO_SUCCESS;
}


/**
 * @brief      : lsp 添加入链表
 * @param[in ] : plsp - lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:58:16
 * @note       :
 */
static void ftm_lsp_list_add(struct lsp_entry *plsp)
{
    struct listnode *pnode = NULL;

    if (NULL == plsp)
    {
        return;
    }

    FTM_LOG_DEBUG("Add lsp list node, lsp_index=%u, nexthop=%#x.\n",
                    plsp->lsp_index, plsp->nexthop.addr.ipv4);

    pnode = listnode_lookup(&lsp_list, (void *)plsp);
    if (NULL != pnode)
    {
        FTM_LOG_DEBUG("already exist !");

        return ;
    }

    listnode_add(&lsp_list, (void *)plsp);
	if (1 == lsp_list.count)
	{
        MPLS_TIMER_ADD(ftm_traverse_lsp_list, NULL, TRAVERSE_LSP_LIST_TIME);
	}

    return;
}


/**
 * @brief      : lsp 从链表删除
 * @param[in ] : plsp - lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:59:03
 * @note       :
 */
static void ftm_lsp_list_delete(struct lsp_entry *plsp)
{
    if (NULL == plsp)
    {
        return;
    }

    FTM_LOG_DEBUG("Delete lsp list node, lsp_index=%u, nexthop=%#x.\n",
                    plsp->lsp_index, plsp->nexthop.addr.ipv4);

    listnode_delete(&lsp_list, (void *)plsp);

    return;
}


/**
 * @brief      : 填充 ilm 结构
 * @param[in ] : pilm - ilm 结构
 * @param[in ] : plsp - lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:00:44
 * @note       :
 */
static void ftm_ilm_set(struct ilm_entry *pilm, struct lsp_entry *plsp)
{
    pilm->lsp_index     = plsp->lsp_index;
    pilm->inlabel       = plsp->inlabel;
    pilm->lsp_type      = plsp->lsp_type;
    pilm->pw_index      = plsp->pw_index;
    pilm->group_index   = plsp->group_index;
    pilm->ac_index      = plsp->ac_index;
    pilm->vpnid         = plsp->vpnid;
    pilm->domain_id     = plsp->domain_id;
    pilm->statis_enable = plsp->statis_enable;
    pilm->car_cir       = plsp->car_cir[0];
    pilm->car_pir       = plsp->car_pir[0];
    pilm->couter        = NULL;
    pilm->nhp_type      = NHP_TYPE_LSP;
    pilm->nhp_index     = plsp->nhp_index;
    pilm->ttl           = plsp->ttl;
    pilm->exp           = plsp->exp;

    if (plsp->direction == LSP_DIRECTION_EGRESS)
    {
        if ((plsp->lsp_type == LSP_TYPE_L3VPN)
        || (plsp->lsp_type == LSP_TYPE_L2VC))
        {
            pilm->action = LABEL_ACTION_POPGO;
        }
        else
        {
            pilm->action = LABEL_ACTION_POP;
        }
    }
    else if (plsp->direction == LSP_DIRECTION_TRANSIT)
    {
        pilm->action = LABEL_ACTION_SWAP;
    }

    return;
}


/**
 * @brief      : nhlfe 结构填充
 * @param[in ] : pnhlfe - nhlfe 结构
 * @param[in ] : plsp   - lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:04:12
 * @note       :
 */
static void ftm_nhlfe_set(struct nhlfe_entry *pnhlfe, struct lsp_entry *plsp)
{
    pnhlfe->lsp_index     = plsp->lsp_index;
    pnhlfe->group_index   = plsp->group_index;
    pnhlfe->nexthop       = plsp->nexthop.addr.ipv4;
    pnhlfe->lsp_type      = plsp->lsp_type;
    pnhlfe->nhp_index     = plsp->nhp_index;
    pnhlfe->nhp_type      = plsp->nhp_type;
    pnhlfe->inlabel       = plsp->inlabel;
    pnhlfe->outlabel      = plsp->outlabel;
    pnhlfe->pw_index      = plsp->pw_index;
    pnhlfe->vpnid         = plsp->vpnid;
    pnhlfe->phb_enable    = plsp->phb_enable;
    pnhlfe->mplsoam_id    = plsp->mplsoam_id;
    pnhlfe->statis_enable = plsp->statis_enable;
    pnhlfe->phb_id        = plsp->phb_id;
    pnhlfe->car_cir       = plsp->car_cir[1];
    pnhlfe->car_pir       = plsp->car_pir[1];
    pnhlfe->hqos_id       = plsp->hqos_id;
    pnhlfe->flag          = 0;
    pnhlfe->couter        = NULL;
    pnhlfe->down_flag     = plsp->down_flag;
    pnhlfe->ttl           = plsp->ttl;
    pnhlfe->ttl_copy      = plsp->ttl_copy;
    pnhlfe->exp           = plsp->exp;

    memcpy(pnhlfe->dmac, plsp->dmac, 6);

    if (plsp->direction == LSP_DIRECTION_TRANSIT)
    {
        pnhlfe->action = LABEL_ACTION_SWAP;
    }
    else
    {
        pnhlfe->action = LABEL_ACTION_PUSH;
    }

    return;
}


/**
 * @brief      : 计算 ftm lsp 哈希表 key 值
 * @param[in ] : hash_key - lsp 索引
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:05:44
 * @note       :
 */
static unsigned int ftm_lsp_hash_compute(void *hash_key)
{
    return (unsigned int)hash_key;
}


/**
 * @brief      : 比较 ftm lsp 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - lsp 索引
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:06:13
 * @note       :
 */
static int ftm_lsp_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;

    if (NULL == pitem)
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
 * @brief      : ftm lsp 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:06:37
 * @note       :
 */
void ftm_lsp_table_init(unsigned int size)
{
    hios_hash_init(&ftm_lsp_table, size, ftm_lsp_hash_compute, ftm_lsp_compare);
}


/**
 * @brief      : lsp 添加
 * @param[in ] : plsp - lsp 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:07:02
 * @note       :
 */
int ftm_lsp_add(struct lsp_entry *plsp)
{
    struct hash_bucket *pitem    = NULL;
    struct lsp_entry   *plsp_new = NULL;

    FTM_LOG_DEBUG();

    if (NULL == plsp)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ftm_lsp_table.hash_size == ftm_lsp_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    plsp_new = (struct lsp_entry *)XCALLOC(MTYPE_LSP_ENTRY, sizeof(struct lsp_entry));
    if (NULL == plsp_new)
    {
        FTM_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    memcpy(plsp_new, plsp, sizeof(struct lsp_entry));

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        FTM_LOG_ERROR("malloc\n");

        XFREE(MTYPE_LSP_ENTRY, plsp_new);

        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)plsp_new->lsp_index;
    pitem->data     = plsp_new;

    hios_hash_add(&ftm_lsp_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 刷新 lsp 结构
 * @param[in ] : plsp_old - 原有 lsp 结构
 * @param[in ] : plsp_new - 新增 lsp 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:07:55
 * @note       :
 */
int ftm_lsp_update_config(struct lsp_entry *plsp_old, struct lsp_entry *plsp_new)
{
    FTM_LOG_DEBUG();

    if ((NULL == plsp_old) || (NULL == plsp_new))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memcpy(plsp_old, plsp_new, sizeof(struct lsp_entry));

    return ERRNO_SUCCESS;
}


/**
 * @brief      : lsp 删除
 * @param[in ] : lsp_index - lsp 索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:08:42
 * @note       :
 */
int ftm_lsp_delete(uint32_t lsp_index)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    pitem = hios_hash_find(&ftm_lsp_table, (void *)lsp_index);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    hios_hash_delete(&ftm_lsp_table, pitem);
    XFREE(MTYPE_LSP_ENTRY, pitem->data);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : lsp 查找
 * @param[in ] : lsp_index - lsp 索引
 * @param[out] :
 * @return     : 成功返回 lsp 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:09:04
 * @note       :
 */
struct lsp_entry *ftm_lsp_lookup(uint32_t lsp_index)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    pitem = hios_hash_find(&ftm_lsp_table, (void *)lsp_index);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct lsp_entry *)pitem->data;
}


/**
 * @brief      : lsp 响应 arp 事件
 * @param[in ] : parp   - arp 结构
 * @param[in ] : opcode - 操作码
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:09:32
 * @note       :
 */
void ftm_lsp_process_arp_event(struct arp_entry *parp, enum OPCODE_E opcode)
{
    struct hash_bucket *pbucket = NULL;
    struct lsp_entry   *plsp    = NULL;
    struct nhlfe_entry *pnhlfe  = NULL;
    int                 cursor;

    FTM_LOG_DEBUG();

    if (NULL == parp)
    {
        return;
    }

    /* lsp 出接口不允许学习到 valnif 接口上 */
    if (IFM_TYPE_IS_VLANIF(parp->ifindex))
    {
        return;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, ftm_lsp_table)
    {
        plsp = (struct lsp_entry *)pbucket->data;
        if (NULL == plsp)
        {
            continue;
        }

        /* 不相关的公网 lsp */
        if ((plsp->nexthop.addr.ipv4 == 0)
        || (plsp->vpnid != parp->key.vpnid)
        || (plsp->nexthop.addr.ipv4 != parp->key.ipaddr))
        {
            continue;
        }

        pnhlfe = ftm_nhlfe_lookup(plsp->lsp_index);
        if (NULL == pnhlfe)
        {
            continue;
        }

        /* 更新公网 lsp 的 nhlfe 表项 */
        if (LSP_DIRECTION_INGRESS == plsp->direction)
        {
            if (OPCODE_DELETE == opcode)
            {
                pnhlfe->flag      = 1;
                pnhlfe->down_flag = LINK_DOWN;
                pnhlfe->nhp_index = 0;
                memset(pnhlfe->dmac, 0, MAC_LEN);

                ftm_nhlfe_send_to_hal(pnhlfe, IPC_OPCODE_UPDATE, LSP_SUBTYPE_DOWN);
                ftm_nhlfe_send_to_mpls(pnhlfe, IPC_OPCODE_UPDATE, LSP_SUBTYPE_DOWN);
                ftm_lsp_list_add(plsp);
            }
            else
            {
                ftm_lsp_list_delete(plsp);
                ftm_nhlfe_update_arp(pnhlfe, parp);
                ftm_nhlfe_send_to_mpls(pnhlfe, IPC_OPCODE_UPDATE, LSP_SUBTYPE_UP);
            }
        }
        else if (LSP_DIRECTION_TRANSIT == plsp->direction)
        {
            if (OPCODE_DELETE == opcode)
            {
                pnhlfe->flag      = 0;
                pnhlfe->down_flag = LINK_DOWN;
                pnhlfe->nhp_index = 0;
                memset(pnhlfe->dmac, 0, MAC_LEN);

                ftm_ilm_send_to_hal(ftm_ilm_lookup(plsp->inlabel), IPC_OPCODE_DELETE, 0);
                ftm_nhlfe_send_to_hal(pnhlfe, IPC_OPCODE_DELETE, 0);
                ftm_nhlfe_send_to_mpls(pnhlfe, IPC_OPCODE_UPDATE, LSP_SUBTYPE_DOWN);
                ftm_lsp_list_add(plsp);
            }
            else
            {
                ftm_lsp_list_delete(plsp);
                ftm_nhlfe_update_arp(pnhlfe, parp);
                ftm_ilm_send_to_hal(ftm_ilm_lookup(plsp->inlabel), IPC_OPCODE_ADD, 0);
                ftm_nhlfe_send_to_mpls(pnhlfe, IPC_OPCODE_UPDATE, LSP_SUBTYPE_UP);
            }
        }
    }

    return;
}


/**
 * @brief      : lsp 删除事件
 * @param[in ] : plsp - lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:18:35
 * @note       :
 */
static void ftm_lsp_process_delete(struct lsp_entry *plsp)
{
    if (NULL == plsp)
    {
        return;
    }

    /* 删除 ilm */
    if ((plsp->direction == LSP_DIRECTION_EGRESS)
    || (plsp->direction == LSP_DIRECTION_TRANSIT))
    {
        ftm_ilm_delete(plsp->inlabel);
    }

    /* 删除 nhlfe */
    if ((plsp->direction == LSP_DIRECTION_INGRESS)
    || (plsp->direction == LSP_DIRECTION_TRANSIT))
    {
        ftm_nhlfe_delete(plsp->lsp_index);
    }

    /* 删除公网 lsp */
    if ((plsp->lsp_type == LSP_TYPE_STATIC)
    || (plsp->lsp_type == LSP_TYPE_LDP))
    {
        ftm_lsp_list_delete(ftm_lsp_lookup(plsp->lsp_index));
        ftm_lsp_delete(plsp->lsp_index);
    }
}


/**
 * @brief      : lsp 添加事件
 * @param[in ] : plsp - lsp 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:19:16
 * @note       :
 */
static int ftm_lsp_process_add(struct lsp_entry *plsp)
{
    struct nhlfe_entry *pnhlfe      = NULL;
    struct ilm_entry   *pilm        = NULL;
    struct lsp_entry   *plsp_lookup = NULL;
    struct ilm_entry    ilm;
    struct nhlfe_entry  nhlfe;
    int nhlfe_flag  = 0;    // 1: nhlfe 未下发
    int ret;

    memset(&ilm, 0, sizeof(struct ilm_entry));
    memset(&nhlfe, 0, sizeof(struct nhlfe_entry));

    if (NULL == plsp)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    /* 生成 nhlfe 表项 */
    if ((plsp->direction == LSP_DIRECTION_INGRESS)
    || (plsp->direction == LSP_DIRECTION_TRANSIT))
    {
        pnhlfe = ftm_nhlfe_lookup(plsp->lsp_index);
        if (NULL != pnhlfe)
        {
            ftm_nhlfe_set(&nhlfe, plsp);
            ret = ftm_nhlfe_update_config(pnhlfe, &nhlfe);
            if (ERRNO_NOT_FOUND == ret)
            {
                nhlfe_flag = 1;
            }
            else if (ERRNO_SUCCESS != ret)
            {
                FTM_LOG_ERROR("errcode = %d\n", ret);

                return ret;
            }
            else
            {
                ret = ftm_nhlfe_send_to_hal(pnhlfe, IPC_OPCODE_ADD, 0);
                if (ERRNO_SUCCESS != ret)
                {
                    FTM_LOG_ERROR("nhlfe send to hal\n");

                    nhlfe_flag = 1;
                }
            }

            if ((nhlfe_flag == 1) && (LSP_DIRECTION_INGRESS == plsp->direction))
            {
                pnhlfe->down_flag = LINK_DOWN;
                pnhlfe->flag      = 1;

                ret = ftm_nhlfe_send_to_hal(pnhlfe, IPC_OPCODE_ADD, 0);
                if (ERRNO_SUCCESS != ret)
                {
                    FTM_LOG_ERROR("nhlfe send to hal\n");

                    nhlfe_flag = 1;
                }
            }
        }
        else
        {
            pnhlfe = (struct nhlfe_entry *)XCALLOC(MTYPE_FTM_NHLFE_ENTRY,
                                                    sizeof(struct nhlfe_entry));
            if (NULL == pnhlfe)
            {
                FTM_LOG_ERROR("malloc\n");

                return ERRNO_MALLOC;
            }

            ftm_nhlfe_set(pnhlfe, plsp);
            ret = ftm_nhlfe_add(pnhlfe);
            if (ERRNO_NOT_FOUND == ret)
            {
                nhlfe_flag = 1;
            }
            else if (ERRNO_SUCCESS != ret)
            {
                FTM_LOG_ERROR("errcode = %d\n", ret);

                XFREE(MTYPE_FTM_NHLFE_ENTRY, pnhlfe);

                return ret;
            }
            else
            {
                ret = ftm_nhlfe_send_to_hal(pnhlfe, IPC_OPCODE_ADD, 0);
                if (ERRNO_SUCCESS != ret)
                {
                    FTM_LOG_ERROR("nhlfe send to hal\n");

                    nhlfe_flag = 1;
                }
            }

            /* ingress lsp 未学到 arp 先下 nhlfe */
            if ((nhlfe_flag == 1) && (LSP_DIRECTION_INGRESS == plsp->direction))
            {
                pnhlfe->down_flag = LINK_DOWN;
                pnhlfe->flag = 1;

                ret = ftm_nhlfe_send_to_hal(pnhlfe, IPC_OPCODE_ADD, 0);
                if (ERRNO_SUCCESS != ret)
                {
                    FTM_LOG_ERROR("nhlfe send to hal\n");

                    nhlfe_flag = 1;
                }
            }
        }
    }

    /* 生成 ilm 表项 */
    if ((plsp->direction == LSP_DIRECTION_EGRESS)
    || (plsp->direction == LSP_DIRECTION_TRANSIT))
    {
        pilm = ftm_ilm_lookup(plsp->inlabel);
        if (NULL != pilm)
        {
            ftm_ilm_set(&ilm, plsp);
            ret = ftm_ilm_update_config(pilm, &ilm);
            if ((ERRNO_SUCCESS == ret) && (1 != nhlfe_flag))
            {
                ret = ftm_ilm_send_to_hal(pilm, IPC_OPCODE_ADD, 0);
                if (ERRNO_SUCCESS != ret)
                {
                    FTM_LOG_ERROR("ilm send to hal\n");
                }
            }
        }
        else
        {
            pilm = (struct ilm_entry *)XCALLOC(MTYPE_FTM_ILM_ENTRY,
                                                sizeof(struct ilm_entry));
            if (NULL == pilm)
            {
                FTM_LOG_ERROR("malloc\n");

                return ERRNO_MALLOC;
            }

            ftm_ilm_set(pilm, plsp);
            ret = ftm_ilm_add(pilm);
            if (ERRNO_SUCCESS != ret)
            {
                XFREE(MTYPE_FTM_ILM_ENTRY, pilm);

                return ret;
            }

            if (nhlfe_flag != 1)
            {
                ret = ftm_ilm_send_to_hal(pilm, IPC_OPCODE_ADD, 0);
                if (ERRNO_SUCCESS != ret)
                {
                    FTM_LOG_ERROR("ilm send to hal\n");
                }
            }
        }
    }

    /* 公网 lsp 存储，用于更新 arp */
    if ((plsp->lsp_type == LSP_TYPE_STATIC)
    || (plsp->lsp_type == LSP_TYPE_LDP))
    {
        plsp_lookup = ftm_lsp_lookup(plsp->lsp_index);
        if (NULL != plsp_lookup)
        {
            ftm_lsp_update_config(plsp_lookup, plsp);
        }
        else
        {
            ftm_lsp_add(plsp);
        }

        /* 需要触发 arp 学习的 lsp */
        if ((0 != plsp->nexthop.addr.ipv4) && (1 == nhlfe_flag))
        {
            plsp_lookup = ftm_lsp_lookup(plsp->lsp_index);
            if (NULL != plsp_lookup)
            {
               ftm_lsp_list_add(plsp_lookup);
            }
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : lsp 更新事件
 * @param[in ] : plsp    - lsp 结构
 * @param[in ] : subtype - 更新子类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:21:26
 * @note       :
 */
static int ftm_lsp_process_update(struct lsp_entry *plsp, enum LSP_SUBTYPE subtype)
{
    struct ilm_entry   ilm;
    struct nhlfe_entry nhlfe;
    struct nhlfe_entry *pnhlfe = NULL;
    int ret = ERRNO_SUCCESS;
    int nhlfe_flag = 0;     // 1: nhlfe 未下发

    memset(&ilm, 0, sizeof(struct ilm_entry));
    memset(&nhlfe, 0, sizeof(struct nhlfe_entry));

    if (NULL == plsp)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    switch (subtype)
    {
        case LSP_SUBTYPE_UP:
        case LSP_SUBTYPE_DOWN:
        case LSP_SUBTYPE_HQOS:
        case LSP_SUBTYPE_INLABEL:
            if ((plsp->direction == LSP_DIRECTION_INGRESS)
            || (plsp->direction == LSP_DIRECTION_TRANSIT))
            {
                ftm_nhlfe_set(&nhlfe, plsp);
                ret = ftm_nhlfe_update(&nhlfe, subtype);
                if (ERRNO_NOT_FOUND == ret)
                {
                    nhlfe_flag = 1;
                }
                if (ERRNO_SUCCESS != ret)
                {
                    return ret;
                }
                else
                {
                    ret = ftm_nhlfe_send_to_hal(ftm_nhlfe_lookup(nhlfe.lsp_index),
                                                IPC_OPCODE_UPDATE, subtype);
                    if (ERRNO_SUCCESS != ret)
                    {
                        FTM_LOG_ERROR("nhlfe send to hal\n");

                        return ret;
                    }
                }

                if ((1 == nhlfe_flag) && (LSP_DIRECTION_INGRESS == plsp->direction))
                {
                    pnhlfe = ftm_nhlfe_lookup(nhlfe.lsp_index);
                    if (NULL != pnhlfe)
                    {
                        pnhlfe->down_flag = LINK_DOWN;
                        pnhlfe->flag      = 1;
                    }

                    ret = ftm_nhlfe_send_to_hal(pnhlfe, IPC_OPCODE_UPDATE, subtype);
                    if (ERRNO_SUCCESS != ret)
                    {
                        FTM_LOG_ERROR("nhlfe send to hal\n");

                        return ret;
                    }
                }
            }
            break;
        case LSP_SUBTYPE_MPLSTP_OAM:
        case LSP_SUBTYPE_QOS:
        case LSP_SUBTYPE_GROUP:
        case LSP_SUBTYPE_COUNTER:
        case LSP_SUBTYPE_CAR:
        case LSP_SUBTYPE_TUNNEL:
            if ((plsp->direction == LSP_DIRECTION_INGRESS)
            || (plsp->direction == LSP_DIRECTION_TRANSIT))
            {
                ftm_nhlfe_set(&nhlfe, plsp);
                ret = ftm_nhlfe_update(&nhlfe, subtype);
                if (ERRNO_SUCCESS == ret)
                {
                    ret = ftm_nhlfe_send_to_hal(ftm_nhlfe_lookup(nhlfe.lsp_index),
                                                IPC_OPCODE_UPDATE, subtype);
                    if (ERRNO_SUCCESS != ret)
                    {
                        FTM_LOG_ERROR("nhlfe update send to hal\n");
                    }
                }
            }

            if ((plsp->direction == LSP_DIRECTION_EGRESS)
            || (plsp->direction == LSP_DIRECTION_TRANSIT))
            {
                ftm_ilm_set(&ilm, plsp);
                ret = ftm_ilm_update(&ilm, subtype);
                if (ERRNO_SUCCESS == ret)
                {
                    ret = ftm_ilm_send_to_hal(ftm_ilm_lookup(ilm.inlabel),
                                                IPC_OPCODE_UPDATE, subtype);
                    if (ERRNO_SUCCESS != ret)
                    {
                        FTM_LOG_ERROR("ilm update send to hal\n");
                    }
                }
            }
            break;
        default:
            break;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : lsp 消息接收
 * @param[in ] : pdata    - 接收数据
 * @param[in ] : data_len - 数据长度
 * @param[in ] : data_num - 数据个数
 * @param[in ] : subtype  - 消息子类型
 * @param[in ] : opcode   - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:23:18
 * @note       :
 */
int ftm_lsp_msg(void *pdata, int data_len, int data_num, uint8_t subtype,
                        enum IPC_OPCODE opcode)
{
    struct lsp_entry *plsp    = NULL;
    uint32_t         *plsr_id = NULL;
    int i;

    FTM_LOG_DEBUG();

    for (i=0; i<data_num; i++)
    {
        plsp = (struct lsp_entry *)pdata + i;
        if (NULL == plsp)
        {
            FTM_LOG_ERROR("data is empty\n");

            continue;
        }

        if (opcode == IPC_OPCODE_DELETE)
        {
            ftm_lsp_process_delete(plsp);
        }
        else if (opcode == IPC_OPCODE_ADD)
        {
            ftm_lsp_process_add(plsp);
        }
        else if (opcode == IPC_OPCODE_UPDATE)
        {
            ftm_lsp_process_update(plsp, subtype);
        }
        else if (opcode == IPC_OPCODE_ENABLE)
        {
            if (subtype == LSP_SUBTYPE_LSR_ID)
            {
                plsr_id = (uint32_t *)pdata;

                if (NULL != plsr_id)
                {
                    g_mpls.lsr_id = *plsr_id;
                }
            }
            else if (subtype == LSP_SUBTYPE_LSRV6_ID)
            {
                memcpy(g_mpls.lsrv6_id.ipv6, pdata, IPV6_ADDR_LEN);
            }
        }
        else if (opcode == IPC_OPCODE_DISABLE)
        {
            if (subtype == LSP_SUBTYPE_LSR_ID)
            {
                g_mpls.lsr_id = 0;
            }
            else if (subtype == LSP_SUBTYPE_LSRV6_ID)
            {
                memset(g_mpls.lsrv6_id.ipv6, 0, IPV6_ADDR_LEN);
            }
        }
    }

    return ERRNO_SUCCESS;
}


int ftm_tpoam_msg(struct ipc_msghdr_n *phdr, void *pdata, int data_len, uint8_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
    if(!pdata || !msg_index)
    {
        return 1;
    }

    if(msg_subtype == OAM_SUBTYPE_AIS || msg_subtype == OAM_SUBTYPE_LOCK)
    {
        if(opcode == IPC_OPCODE_GET_BULK)
        {
            ftm_msg_rcv_get_bulk_lsp(phdr, msg_subtype, msg_index);
        }
    }

	return 0;
}


