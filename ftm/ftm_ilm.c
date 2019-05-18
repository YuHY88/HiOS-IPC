/**
 * @file      : ftm_ilm.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 17:37:38
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include "ftm.h"
#include "ftm_ilm.h"
#include "ftm_lsp.h"


struct hash_table ilm_table;


/**
 * @brief      : 计算 ilm 哈希表 key 值
 * @param[in ] : hash_key - lsp 入标签
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:38:13
 * @note       :
 */
static unsigned int ftm_ilm_label_hash(void *hash_key)
{
    return (unsigned int)hash_key;
}


/**
 * @brief      : 比较 ilm 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - lsp 入标签
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:38:38
 * @note       :
 */
static int ftm_ilm_label_compare(void *pitem, void *hash_key)
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
 * @brief      : ilm 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:39:07
 * @note       :
 */
void ftm_ilm_table_init(unsigned int size)
{
    hios_hash_init(&ilm_table, size, ftm_ilm_label_hash, ftm_ilm_label_compare);
}


/**
 * @brief      : ilm 下发至 hal
 * @param[in ] : ilm     - ilm 结构
 * @param[in ] : opcode  - 操作码
 * @param[in ] : subtype - 消息子类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:39:22
 * @note       :
 */
int ftm_ilm_send_to_hal(struct ilm_entry *pilm, enum IPC_OPCODE opcode,
                                    enum LSP_SUBTYPE subtype)
{
    int ret;

    ret = ftm_msg_send_to_hal(pilm, sizeof(struct ilm_entry), 1,
                                IPC_TYPE_ILM, subtype, opcode, 0);
    if (ERRNO_SUCCESS != ret)
    {
        FTM_LOG_ERROR("send to hal\n");

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ilm 添加
 * @param[in ] : pilm - ilm 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:40:09
 * @note       :
 */
int ftm_ilm_add(struct ilm_entry *pilm)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    if (NULL == pilm)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ilm_table.hash_size == ilm_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        FTM_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)pilm->inlabel;
    pitem->data     = pilm;

    hios_hash_add(&ilm_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ilm 表项刷新
 * @param[in ] : pilm     - 原有 ilm 结构
 * @param[in ] : pilm_new - 新增 ilm 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:40:33
 * @note       :
 */
int ftm_ilm_update_config(struct ilm_entry *pilm, struct ilm_entry *pilm_new)
{
    FTM_LOG_DEBUG();

    if ((NULL == pilm) || (NULL == pilm_new))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memcpy(pilm, pilm_new, sizeof(struct ilm_entry));

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ilm 更新
 * @param[in ] : pilm    - ilm 结构
 * @param[in ] : subtype - 更新子类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:41:18
 * @note       :
 */
int ftm_ilm_update(struct ilm_entry *pilm, enum LSP_SUBTYPE subtype)
{
    struct ilm_entry *pilm_old = NULL;

    pilm_old = ftm_ilm_lookup(pilm->inlabel);
    if (NULL == pilm_old)
    {
        return ERRNO_NOT_FOUND;
    }

    memcpy(pilm_old, pilm, sizeof(struct ilm_entry));

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ilm 删除
 * @param[in ] : label - lsp 入标签
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:42:01
 * @note       :
 */
int ftm_ilm_delete(uint32_t label)
{
    struct hash_bucket *pitem = NULL;
    int                 ret;

    FTM_LOG_DEBUG();

    pitem = hios_hash_find(&ilm_table, (void *)label);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    ret = ftm_ilm_send_to_hal(pitem->data, IPC_OPCODE_DELETE, 0);
    if (ERRNO_SUCCESS != ret)
    {
        FTM_LOG_ERROR("send to hal\n");

        return ERRNO_IPC;
    }

    hios_hash_delete(&ilm_table, pitem);

    XFREE(MTYPE_FTM_ILM_ENTRY, pitem->data);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ilm 查找
 * @param[in ] : label - lsp 入标签
 * @param[out] :
 * @return     : 成功返回 ilm 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:42:32
 * @note       :
 */
struct ilm_entry *ftm_ilm_lookup(uint32_t label)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    pitem = hios_hash_find(&ilm_table, (void *)label);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct ilm_entry *)pitem->data;
}


