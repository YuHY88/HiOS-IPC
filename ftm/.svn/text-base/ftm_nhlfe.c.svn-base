/**
 * @file      : ftm_nhlfe.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 17:27:19
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/errcode.h>
#include <lib/memory.h>
#include <lib/log.h>
#include "ftm_nhlfe.h"
#include "ftm.h"
#include "ftm_arp.h"


struct hash_table nhlfe_table;


/**
 * @brief      : 计算 nhlfe 哈希表 key 值
 * @param[in ] : hash_key - lsp 索引
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:27:59
 * @note       :
 */
static unsigned int ftm_nhlfe_index_hash(void *hash_key)
{
    return (unsigned int)hash_key;
}


/**
 * @brief      : 比较 nhlfe 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - lsp 索引
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:28:29
 * @note       :
 */
static int ftm_nhlfe_index_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;

    if (pitem == NULL)
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
 * @brief      : nhlfe 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:29:05
 * @note       :
 */
void ftm_nhlfe_table_init(unsigned int size)
{
    hios_hash_init(&nhlfe_table, size, ftm_nhlfe_index_hash, ftm_nhlfe_index_compare);
}


/**
 * @brief      : nhlfe 下发至 hal
 * @param[in ] : pnhlfe  - nhlfe 结构
 * @param[in ] : opcode  - 操作码
 * @param[in ] : subtype - 消息子类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:29:22
 * @note       :
 */
int ftm_nhlfe_send_to_hal(struct nhlfe_entry *pnhlfe, enum IPC_OPCODE opcode,
                                    enum LSP_SUBTYPE subtype)
{
    int ret;

    ret = ftm_msg_send_to_hal(pnhlfe, sizeof(struct nhlfe_entry), 1,
                                IPC_TYPE_NHLFE, subtype, opcode, 0);
    if (ERRNO_SUCCESS != ret)
    {
        FTM_LOG_ERROR("send to hal\n");

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : nhlfe 发送至 mpls
 * @param[in ] : pnhlfe  - nhlfe 结构
 * @param[in ] : opcode  - 操作码
 * @param[in ] : subtype - 消息子类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:30:08
 * @note       : 下一跳方式 lsp 学习到 arp 通知至 mpls
 */
int ftm_nhlfe_send_to_mpls(struct nhlfe_entry *pnhlfe, enum IPC_OPCODE opcode,
                                    enum LSP_SUBTYPE subtype)
{
    int ret;

    ret = ipc_send_msg_n2(&pnhlfe->lsp_index, sizeof(pnhlfe->lsp_index), 1, MODULE_ID_MPLS,
                            MODULE_ID_FTM, IPC_TYPE_NHLFE, subtype, opcode, pnhlfe->nhp_index);
    if (ERRNO_SUCCESS != ret)
    {
        FTM_LOG_ERROR("send to mpls\n");

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : nhlfe 添加
 * @param[in ] : pnhlfe - nhlfe 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:31:34
 * @note       :
 */
int ftm_nhlfe_add(struct nhlfe_entry *pnhlfe)
{
    struct hash_bucket *pitem = NULL;
    struct arp_entry   *parp  = NULL;

    FTM_LOG_DEBUG();

    if (NULL == pnhlfe)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (nhlfe_table.hash_size == nhlfe_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        FTM_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)pnhlfe->lsp_index;
    pitem->data     = pnhlfe;

    hios_hash_add(&nhlfe_table, pitem);

    if (0 != pnhlfe->nexthop)
    {
        parp = arp_lookup_active(pnhlfe->nexthop, 0);
        if (NULL == parp)
        {
            return ERRNO_NOT_FOUND;
        }

        /* arp 出接口为 vlanif 接口直接返回 */
        if (IFM_TYPE_IS_VLANIF(parp->ifindex))
        {
            return ERRNO_NOT_FOUND;
        }

        pnhlfe->flag      = 1;
        pnhlfe->nhp_index = parp->ifindex;
        pnhlfe->down_flag = LINK_UP;
        memcpy(pnhlfe->dmac, parp->mac, 6);

        ftm_nhlfe_send_to_mpls(pnhlfe, IPC_OPCODE_UPDATE, LSP_SUBTYPE_UP);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : nhlfe 表项刷新
 * @param[in ] : pnhlfe     - 原有 nhlfe 结构
 * @param[in ] : pnhlfe_new - 新增 nhlfe 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:32:20
 * @note       :
 */
int ftm_nhlfe_update_config(struct nhlfe_entry *pnhlfe, struct nhlfe_entry *pnhlfe_new)
{
    struct arp_entry *parp = NULL;

    FTM_LOG_DEBUG();

    if ((NULL == pnhlfe) || (NULL == pnhlfe_new))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memcpy(pnhlfe, pnhlfe_new, sizeof(struct nhlfe_entry));

    if (pnhlfe->nexthop != 0)
    {
        parp = arp_lookup_active(pnhlfe->nexthop, 0);
        if (NULL == parp)
        {
            return ERRNO_NOT_FOUND;
        }

        /* arp 出接口为 vlanif 接口直接返回 */
        if (IFM_TYPE_IS_VLANIF(parp->ifindex))
        {
            return ERRNO_NOT_FOUND;
        }

        pnhlfe->flag      = 1;
        pnhlfe->down_flag = LINK_UP;
        pnhlfe->nhp_index = parp->ifindex;
        memcpy(pnhlfe->dmac, parp->mac, MAC_LEN);

        ftm_nhlfe_send_to_mpls(pnhlfe, IPC_OPCODE_UPDATE, LSP_SUBTYPE_UP);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : nhlfe 更新
 * @param[in ] : pnhlfe  - nhlfe 结构
 * @param[in ] : subtype - 更新消息子类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:33:15
 * @note       :
 */
int ftm_nhlfe_update(struct nhlfe_entry *pnhlfe, enum LSP_SUBTYPE subtype)
{
    struct pw_info     *ppw        = NULL;
    struct nhlfe_entry *pnhlfe_old = NULL;
    struct arp_entry   *parp       = NULL;
    int down_flag = LINK_DOWN;

    FTM_LOG_DEBUG();

    if (pnhlfe == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pnhlfe_old = ftm_nhlfe_lookup(pnhlfe->lsp_index);
    if (NULL == pnhlfe_old)
    {
        return ERRNO_FAIL;
    }

    switch (subtype)
    {
        case LSP_SUBTYPE_GROUP:
            pnhlfe_old->group_index = pnhlfe->group_index;
            break;
        case LSP_SUBTYPE_DOWN:
            pnhlfe_old->down_flag = LINK_DOWN;
            break;
        case LSP_SUBTYPE_UP:
            pnhlfe_old->down_flag = LINK_UP;
            break;
        case LSP_SUBTYPE_QOS:
            pnhlfe_old->phb_enable = pnhlfe->phb_enable;
            pnhlfe_old->phb_id     = pnhlfe->phb_id;
            break;
        case LSP_SUBTYPE_CAR:
            pnhlfe_old->car_cir = pnhlfe->car_cir;
            pnhlfe_old->car_pir = pnhlfe->car_pir;
            break;
        case LSP_SUBTYPE_COUNTER:
            pnhlfe_old->statis_enable = pnhlfe->statis_enable;
            break;
        case LSP_SUBTYPE_MPLSTP_OAM:
            pnhlfe_old->mplsoam_id    = pnhlfe->mplsoam_id;
            pnhlfe_old->statis_enable = pnhlfe->statis_enable;
            break;
        case LSP_SUBTYPE_HQOS:
            pnhlfe_old->hqos_id = pnhlfe->hqos_id;
            break;
        case LSP_SUBTYPE_INLABEL:
            pnhlfe_old->inlabel = pnhlfe->inlabel;
            break;
        case LSP_SUBTYPE_TUNNEL:
            if (pnhlfe->lsp_type == LSP_TYPE_L2VC)
            {
                pnhlfe_old->nhp_index   = pnhlfe->nhp_index;
                pnhlfe_old->group_index = pnhlfe->group_index;

                ppw = ftm_pw_lookup(pnhlfe_old->pw_index);
                if (NULL != ppw)
                {
                    ppw->nhp_index = pnhlfe->nhp_index;
                }
            }
            else
            {
                pnhlfe_old->group_index   = pnhlfe->group_index;
                pnhlfe_old->phb_enable    = pnhlfe->phb_enable;
                pnhlfe_old->phb_id        = pnhlfe->phb_id;
                pnhlfe_old->car_cir       = pnhlfe->car_cir;
                pnhlfe_old->car_pir       = pnhlfe->car_pir;
                pnhlfe_old->statis_enable = pnhlfe->statis_enable;
                pnhlfe_old->mplsoam_id    = pnhlfe->mplsoam_id;
				pnhlfe_old->hqos_id       = pnhlfe->hqos_id;
            }
            break;
        default:
            return ERRNO_SUCCESS;
    }

    if (LSP_SUBTYPE_UP == subtype)
    {
        down_flag = pnhlfe_old->down_flag;
        memcpy(pnhlfe_old, pnhlfe, sizeof(struct nhlfe_entry));

        if (pnhlfe->nexthop != 0)
        {
            parp = arp_lookup_active(pnhlfe->nexthop, 0);
            if (NULL == parp)
            {
                return ERRNO_NOT_FOUND;
            }

            /* arp 出接口为 vlanif 接口直接返回 */
            if (IFM_TYPE_IS_VLANIF(parp->ifindex))
            {
                return ERRNO_NOT_FOUND;
            }

            pnhlfe_old->flag      = 1;
            pnhlfe_old->down_flag = LINK_UP;
            pnhlfe_old->nhp_index = parp->ifindex;
            memcpy(pnhlfe_old->dmac, parp->mac, MAC_LEN);

            if (down_flag == LINK_DOWN)
            {
                ftm_nhlfe_send_to_mpls(pnhlfe, IPC_OPCODE_UPDATE, LSP_SUBTYPE_UP);
            }
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : nhlfe 更新出接口
 * @param[in ] : pnhlfe - nhlfe 结构
 * @param[in ] : parp   - arp 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:34:04
 * @note       :
 */
int ftm_nhlfe_update_arp(struct nhlfe_entry *pnhlfe, struct arp_entry *parp)
{
    int flag = 0;
    int ret  = ERRNO_SUCCESS;

    FTM_LOG_DEBUG();

    if ((NULL == pnhlfe) || (NULL == parp))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    flag = pnhlfe->flag;

    pnhlfe->flag      = 1;
    pnhlfe->down_flag = LINK_UP;
    pnhlfe->nhp_index = parp->ifindex;
    memcpy(pnhlfe->dmac, parp->mac, MAC_LEN);

    if (0 == flag)
    {
        ret = ftm_nhlfe_send_to_hal(pnhlfe, IPC_OPCODE_ADD, 0);
        if (ERRNO_SUCCESS != ret)
        {
            FTM_LOG_ERROR("send to hal add\n");

            return ERRNO_IPC;
        }
    }
    else
    {
        ret = ftm_nhlfe_send_to_hal(pnhlfe, IPC_OPCODE_UPDATE, LSP_SUBTYPE_UP);
        if (ERRNO_SUCCESS != ret)
        {
            FTM_LOG_ERROR("send to hal update\n");

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : nhlfe 删除
 * @param[in ] : lsp_index - lsp 索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:35:09
 * @note       :
 */
int ftm_nhlfe_delete(uint32_t lsp_index)
{
    struct hash_bucket *pitem = NULL;
    int                 ret;

    FTM_LOG_DEBUG();

    pitem = hios_hash_find(&nhlfe_table, (void *)lsp_index);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    ret = ftm_nhlfe_send_to_hal(pitem->data, IPC_OPCODE_DELETE, 0);
    if (ERRNO_SUCCESS != ret)
    {
        FTM_LOG_ERROR("send to hal\n");

        return ERRNO_IPC;
    }

    hios_hash_delete(&nhlfe_table, pitem);

    XFREE(MTYPE_FTM_NHLFE_ENTRY, pitem->data);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : nhlfe 查找
 * @param[in ] : lsp_index - lsp 索引
 * @param[out] :
 * @return     : 成功返回 nhlfe 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:35:37
 * @note       :
 */
struct nhlfe_entry *ftm_nhlfe_lookup(uint32_t lsp_index)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    pitem = hios_hash_find(&nhlfe_table, (void *)lsp_index);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct nhlfe_entry *)pitem->data;
}


