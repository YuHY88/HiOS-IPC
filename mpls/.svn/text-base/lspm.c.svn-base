/**
 * @file      : lspm.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月2日 15:20:22
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/index.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include "mpls_main.h"
#include "lspm.h"
#include "lsp_static.h"


struct hash_table lsp_table;


/**
 * @brief      : 计算 lsp 哈希表 key 值
 * @param[in ] : hash_key - lsp 索引
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:21:55
 * @note       :
 */
static unsigned int mpls_lsp_index_hash(void *hash_key)
{
    return (unsigned int)hash_key;
}


/**
 * @brief      : 比较 lsp 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - lsp 索引
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:25:44
 * @note       :
 */
static int mpls_lsp_index_compare(void *pitem, void *hash_key)
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
 * @brief      : lsp 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:26:24
 * @note       :
 */
void mpls_lsp_table_init(unsigned int size)
{
    hios_hash_init(&lsp_table, size, mpls_lsp_index_hash, mpls_lsp_index_compare);

    return;
}


/**
 * @brief      : lsp 更新
 * @param[in ] : plsp    - lsp 结构
 * @param[in ] : subtype - 更新子类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:31:48
 * @note       :
 */
int mpls_lsp_update(struct lsp_entry *plsp, enum LSP_SUBTYPE subtype)
{
    int ret;

    MPLS_LOG_DEBUG();

    if (plsp == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    ret = MPLS_IPC_SENDTO_FTM(plsp, sizeof(struct lsp_entry), 1, MODULE_ID_MPLS,
                        IPC_TYPE_LSP, IPC_OPCODE_UPDATE, subtype, 0);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to common\n");

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : lsp 结构释放
 * @param[in ] : plsp - lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:33:03
 * @note       :
 */
void mpls_lsp_free(struct lsp_entry *plsp)
{
    if (plsp != NULL)
    {
        XFREE(MTYPE_LSP_ENTRY, plsp);
    }

    return;
}


/**
 * @brief      : lsp 结构创建
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回分配内存的 lsp 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:38:06
 * @note       :
 */
struct lsp_entry *mpls_lsp_create(void)
{
    struct lsp_entry *plsp = NULL;

    plsp = (struct lsp_entry *)XCALLOC(MTYPE_LSP_ENTRY, sizeof(struct lsp_entry));
    if (NULL == plsp)
    {
        MPLS_LOG_ERROR("malloc\n");
    }

    return plsp;
}


/**
 * @brief      : 根据对应的标签生成 lsp
 * @param[in ] : inlabel  - 入标签
 * @param[in ] : outlabel - 出标签
 * @param[in ] : type     - lsp 类型
 * @param[out] :
 * @return     : 成功返回创建后的 lsp 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:41:58
 * @note       :
 */
struct lsp_entry *mpls_lsp_new(uint32_t inlabel, uint32_t outlabel, enum LSP_TYPE type)
{
    struct lsp_entry *plspm = NULL;
    uint32_t lsp_index = 0;

    lsp_index = mpls_lsp_alloc_index();
    if (0 == lsp_index)
    {
        return NULL;
    }

    plspm = (struct lsp_entry *)XCALLOC(MTYPE_LSP_ENTRY, sizeof(struct lsp_entry));
    if (NULL == plspm)
    {
        MPLS_LOG_ERROR("malloc\n");
    }

    plspm->lsp_index = lsp_index;
    plspm->lsp_type  = type;
    plspm->nhp_type  = NHP_TYPE_INVALID;
    plspm->down_flag = LINK_UP;
    plspm->ttl       = 255;
    plspm->exp       = 1;

    if ((inlabel > 0) && (outlabel > 0))
    {
        plspm->direction = LSP_DIRECTION_TRANSIT;
        plspm->inlabel   = inlabel;
        plspm->outlabel  = outlabel;
    }
    else if (0 != inlabel)
    {
        plspm->direction = LSP_DIRECTION_EGRESS;
        plspm->inlabel   = inlabel;
    }
    else
    {
        plspm->direction = LSP_DIRECTION_INGRESS;
        plspm->outlabel  = outlabel;
    }

    return plspm;
}


/**
 * @brief      : lsp 添加
 * @param[in ] : plsp - lsp 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:44:34
 * @note       :
 */
int mpls_lsp_add(struct lsp_entry *plsp)
{
    struct hash_bucket *pitem = NULL;
    int                 ret;

    MPLS_LOG_DEBUG();

    if (plsp == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (lsp_table.hash_size == lsp_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    if (NULL != mpls_lsp_lookup(plsp->lsp_index))
    {
        return ERRNO_EXISTED;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        MPLS_LOG_ERROR("malloc\n");

        mpls_lsp_free(plsp);

        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)plsp->lsp_index;
    pitem->data     = plsp;

    hios_hash_add(&lsp_table, pitem);

    ret = MPLS_IPC_SENDTO_FTM(plsp, sizeof(struct lsp_entry), 1,
                        MODULE_ID_MPLS, IPC_TYPE_LSP, IPC_OPCODE_ADD, 0, 0);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to common\n");

        hios_hash_delete(&lsp_table, pitem);
        HASH_BUCKET_DESTROY(pitem);

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : lsp 删除
 * @param[in ] : lsp_index - lsp 索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:50:55
 * @note       :
 */
int mpls_lsp_delete(uint32_t lsp_index)
{
    struct hash_bucket *pitem = NULL;
    struct lsp_entry   *plsp  = NULL;
    int                 ret   = ERRNO_SUCCESS;

    MPLS_LOG_DEBUG();

    if (lsp_index == 0)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pitem = hios_hash_find(&lsp_table, (void *)lsp_index);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    plsp = (struct lsp_entry *)pitem->data;

    ret = MPLS_IPC_SENDTO_FTM(plsp, sizeof(struct lsp_entry), 1,
                        MODULE_ID_MPLS, IPC_TYPE_LSP, IPC_OPCODE_DELETE, 0, 0);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to common\n");

        return ret;
    }

    hios_hash_delete(&lsp_table, pitem);
    HASH_BUCKET_DESTROY(pitem);

    mpls_lsp_free(plsp);

    return ret;
}


/**
 * @brief      : lsp 查找
 * @param[in ] : lsp_index - lsp 索引
 * @param[out] :
 * @return     : 成功返回 lsp 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:51:31
 * @note       :
 */
struct lsp_entry *mpls_lsp_lookup(uint32_t lsp_index)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    pitem = hios_hash_find(&lsp_table, (void *)lsp_index);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct lsp_entry *)pitem->data;
}


/**
 * @brief      : lsp 索引申请
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 lsp 索引，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:52:06
 * @note       :
 */
int mpls_lsp_alloc_index(void)
{
    uint32_t lsp_index = 0;

    lsp_index = index_alloc(INDEX_TYPE_LSP);
    if (0 != lsp_index)
    {
        return lsp_index + LSP_RESERVED_BASE;
    }

    return 0;
}


/**
 * @brief      : lsp 索引释放
 * @param[in ] : lsp_index - lsp 索引
 * @param[out] :
 * @return     : 成功返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:52:53
 * @note       :
 */
int mpls_lsp_free_index(uint32_t lsp_index)
{
    if (lsp_index > LSP_RESERVED_BASE)
    {
        lsp_index = lsp_index - LSP_RESERVED_BASE;

        index_free(INDEX_TYPE_LSP, lsp_index);
    }

    return 0;
}


/**
 * @brief      : lsp down
 * @param[in ] : lsp_index - lsp 索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:53:42
 * @note       :
 */
void mpls_lsp_down(uint32_t lsp_index)
{
    struct lsp_entry  *plsp  = NULL;
    struct static_lsp *splsp = NULL;

    plsp = mpls_lsp_lookup(lsp_index);
    if (plsp == NULL)
    {
        return;
    }

    if (plsp->lsp_type == LSP_TYPE_STATIC)
    {
        splsp = static_lsp_lookup(plsp->name);
        if(NULL != splsp)
        {
            static_lsp_disable(splsp);
        }
    }
    else
    {
        if (plsp->down_flag == LINK_DOWN)
        {
            return;
        }

        plsp->down_flag = LINK_DOWN;

        mpls_lsp_update(plsp, LSP_SUBTYPE_DOWN);
    }

    return;
}


/**
 * @brief      : lsp up
 * @param[in ] : lsp_index - lsp 索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:54:19
 * @note       :
 */
void mpls_lsp_up(uint32_t lsp_index)
{
    struct lsp_entry  *plsp  = NULL;
    struct static_lsp *pslsp = NULL;

    plsp = mpls_lsp_lookup(lsp_index);
    if (plsp == NULL)
    {
        return;
    }

    if (plsp->lsp_type == LSP_TYPE_STATIC)
    {
        pslsp = static_lsp_lookup(plsp->name);
        if (NULL != pslsp)
        {
            static_lsp_enable(pslsp);
        }
    }
    else
    {
        if (plsp->down_flag == LINK_UP)
        {
            return;
        }

        plsp->down_flag = LINK_UP;

        mpls_lsp_update(plsp, LSP_SUBTYPE_UP);
    }

    return;
}


/**
 * @brief      : 通过 lsp 索引获取 lsp 名称
 * @param[in ] : lsp_index - lsp 索引
 * @param[out] :
 * @return     : 成功返回 lsp 名称，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:54:46
 * @note       :
 */
uchar *mpls_lsp_get_name(uint32_t lsp_index)
{
    struct lsp_entry *plsp = NULL;

    plsp = mpls_lsp_lookup(lsp_index);
    if (NULL == plsp)
    {
        return NULL;
    }

    return plsp->name;
}


/**
 * @brief      : 初始化入标签 1~15 的 lsp
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:00:04
 * @note       :
 */
void mpls_lsp_init(void)
{
    struct lsp_entry *plspm = NULL;
    int label = 0;

    for (label=1; label<=15; label++)
    {
        plspm = mpls_lsp_create();
        if (plspm == NULL)
        {
            return;
        }

        plspm->lsp_index = mpls_lsp_alloc_index();
        plspm->lsp_type  = LSP_TYPE_RESERVED;
        plspm->direction = LSP_DIRECTION_EGRESS;
        plspm->inlabel   = label;
        plspm->nhp_type  = NHP_TYPE_INVALID;
        plspm->down_flag = LINK_UP;
        plspm->ttl       = 255;
        plspm->exp       = 1;
        plspm->pwlist    = NULL;

        mpls_lsp_add(plspm);
    }

    return;
}


