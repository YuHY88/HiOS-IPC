/**
 * @file      : ftm_pw.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 17:45:01
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */


#include <lib/memory.h>
#include <lib/errcode.h>
#include "ftm.h"
#include "ftm_ifm.h"
#include "ftm_pw.h"


struct hash_table ftm_pw_table;


/**
 * @brief      : 计算 ftm pw 哈希表 key 值
 * @param[in ] : hash_key - pw 索引
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:45:46
 * @note       :
 */
static unsigned int ftm_pw_compute(void *hash_key)
{
    return (uint32_t)hash_key;
}


/**
 * @brief      : 比较 ftm pw 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - pw 索引
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:46:23
 * @note       :
 */
static int ftm_pw_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;

    if (pbucket->hash_key == hash_key)
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : ftm pw 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:46:47
 * @note       :
 */
void ftm_pw_table_init(uint32_t size)
{
    hios_hash_init(&ftm_pw_table, size, ftm_pw_compute, ftm_pw_compare);
}


/**
 * @brief      : ftm pw 创建
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 pw 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:47:02
 * @note       :
 */
struct pw_info *ftm_pw_create(void)
{
    struct pw_info *ppwinfo = NULL;

    FTM_LOG_DEBUG();

    ppwinfo = (struct pw_info *)XCALLOC(MTYPE_PW_ENTRY, sizeof(struct pw_info));
    if (ppwinfo == NULL)
    {
        FTM_LOG_ERROR("malloc\n");

        return NULL;
    }

    return ppwinfo;
}


/**
 * @brief      : ftm pw 添加
 * @param[in ] : ppwinfo - pw 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:47:45
 * @note       :
 */
int ftm_pw_add(struct pw_info *ppwinfo)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    if (ppwinfo == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (pitem == NULL)
    {
        FTM_LOG_ERROR("malloc\n");
        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)ppwinfo->pwindex;
    pitem->data     = ppwinfo;

    hios_hash_add(&ftm_pw_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm pw 状态更新
 * @param[in ] : pwindex - pw 索引
 * @param[in ] : subtype - 更新消息子类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:48:23
 * @note       :
 */
int ftm_pw_upadte_status(uint32_t pwindex, enum PW_SUBTYPE subtype)
{
    struct pw_info *ppwinfo = NULL;

    FTM_LOG_DEBUG();

    ppwinfo = ftm_pw_lookup(pwindex);
    if (NULL == ppwinfo)
    {
        return ERRNO_SUCCESS;
    }

    if (subtype == PW_SUBTYPE_UP)
    {
        ppwinfo->up_flag = LINK_UP;
    }
    else if (subtype == PW_SUBTYPE_DOWN)
    {
        ppwinfo->up_flag = LINK_DOWN;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm pw 表项刷新
 * @param[in ] : ppw_old - 原有 pw 结构
 * @param[in ] : ppw_new - 新增 pw 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:49:31
 * @note       :
 */
int ftm_pw_update(struct pw_info *ppw_old, struct pw_info *ppw_new)
{
    FTM_LOG_DEBUG();

    if ((NULL == ppw_old) || (NULL == ppw_new))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memcpy(ppw_old, ppw_new, sizeof(struct pw_info));

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm pw 删除
 * @param[in ] : pwindex - pw 索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:50:08
 * @note       :
 */
int ftm_pw_delete(uint32_t pwindex)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    pitem = hios_hash_find(&ftm_pw_table, (void *)pwindex);
    if (pitem == NULL)
    {
        return ERRNO_SUCCESS;
    }

    hios_hash_delete(&ftm_pw_table, pitem);

    XFREE(MTYPE_PW_ENTRY, pitem->data);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm pw 查找
 * @param[in ] : pwindex - pw 索引
 * @param[out] :
 * @return     : 成功返回 pw 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:51:05
 * @note       :
 */
struct pw_info *ftm_pw_lookup(uint32_t pwindex)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    pitem = hios_hash_find(&ftm_pw_table, (void *)pwindex);
    if (pitem == NULL)
    {
        return NULL;
    }

    return ((struct pw_info *)pitem->data);
}


/**
 * @brief      : pw 绑定 ac 接口
 * @param[in ] : ifindex    - 接口索引
 * @param[in ] : mpls_index - pw 索引
 * @param[in ] : subtype    - 消息子类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:51:28
 * @note       :
 */
int ftm_pw_bind_if(uint32_t ifindex, uint32_t mpls_index, enum MPLSIF_INFO subtype)
{
    struct ftm_ifm_l2 *pl2if = NULL;
    struct ftm_ifm    *pifm  = NULL;

    FTM_LOG_DEBUG();

    pifm = ftm_ifm_lookup(ifindex);
    if (NULL == pifm)
    {
        return ERRNO_SUCCESS;
    }

    pl2if = pifm->pl2if;
    if (pl2if == NULL)
    {
        pl2if = ftm_ifm_create_l2if(pifm);
        if (NULL == pl2if)
        {
            FTM_LOG_ERROR("creat l2if\n");

            return ERRNO_MALLOC;
        }
    }

    if (subtype == MPLSIF_INFO_MASTER_PW)
    {
        pl2if->master_index = mpls_index;
    }
    else if (subtype == MPLSIF_INFO_SLAVE_PW)
    {
        pl2if->backup_index = mpls_index;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 解绑 ac 接口
 * @param[in ] : ifindex    - 接口索引
 * @param[in ] : mpls_index - pw 索引
 * @param[in ] : subtype    - 消息子类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:52:26
 * @note       :
 */
int ftm_pw_unbind_if(uint32_t ifindex, uint32_t mpls_index, enum MPLSIF_INFO subtype)
{
    struct ftm_ifm_l2 *pl2if = NULL;
    struct ftm_ifm    *pifm  = NULL;

    FTM_LOG_DEBUG();

    pifm = ftm_ifm_lookup(ifindex);
    if (NULL == pifm)
    {
        return ERRNO_SUCCESS;
    }

    pl2if = pifm->pl2if;
    if (pl2if == NULL)
    {
        return ERRNO_SUCCESS;
    }

    if (subtype == MPLSIF_INFO_MASTER_PW)
    {
        if (pl2if->master_index == mpls_index)
        {
            pl2if->master_index = 0;
        }
    }
    else if (subtype == MPLSIF_INFO_SLAVE_PW)
    {
        if (pl2if->backup_index == mpls_index)
        {
            pl2if->backup_index = 0;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 工作状态更新
 * @param[in ] : pwindex   - pw 索引
 * @param[in ] : pw_backup - pw 工作状态
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:52:44
 * @note       :
 */
int ftm_pw_backup_status_update(uint32_t pwindex, enum BACKUP_E pw_backup)
{
	struct pw_info *ppw  = NULL;
    struct ftm_ifm *pifm = NULL;

    FTM_LOG_DEBUG();

    ppw = ftm_pw_lookup(pwindex);
    if (NULL == ppw)
    {
        return ERRNO_SUCCESS;
    }

    pifm = ftm_ifm_lookup(ppw->ifindex);
    if (NULL == pifm)
    {
        return ERRNO_SUCCESS;
    }

    if (pifm->pl2if != NULL)
    {
        pifm->pl2if->pw_backup = pw_backup;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 消息接收
 * @param[in ] : pdata    - 接收数据
 * @param[in ] : data_len - 数据长度
 * @param[in ] : data_num - 数据个数
 * @param[in ] : subtype  - 消息子类型
 * @param[in ] : opcode   - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 17:53:21
 * @note       :
 */
int ftm_pw_msg(void *pdata, int data_len, int data_num, uint8_t subtype,
                        enum IPC_OPCODE opcode)
{
    struct pw_info *ppwinfo_new = NULL;
    struct pw_info *ppwinfo     = NULL;
    int ret;
    int i;

    FTM_LOG_DEBUG();

    for (i=0; i<data_num; i++)
    {
        ppwinfo = (struct pw_info *)pdata + i;

        if (opcode == IPC_OPCODE_DELETE)
        {
            ftm_pw_delete(ppwinfo->pwindex);
        }
        else if (opcode == IPC_OPCODE_ADD)
        {
            ppwinfo_new = ftm_pw_lookup(ppwinfo->pwindex);
            if (NULL == ppwinfo_new)
            {
                ppwinfo_new = ftm_pw_create();
                if (NULL == ppwinfo_new)
                {
                    return ERRNO_MALLOC;
                }

                memcpy(ppwinfo_new, ppwinfo, sizeof(struct pw_info));

                ret = ftm_pw_add(ppwinfo_new);
                if (ERRNO_SUCCESS != ret)
                {
                    XFREE(MTYPE_PW_ENTRY, ppwinfo_new);

                    return ret;
                }
            }
            else
            {
                ftm_pw_update(ppwinfo_new, ppwinfo);
            }
        }
        else if (opcode == IPC_OPCODE_UPDATE)
        {
            ftm_pw_upadte_status(ppwinfo->pwindex, subtype);
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : mpls 接口消息接收
 * @param[in ] : pdata    - 接收数据
 * @param[in ] : data_len - 数据长度
 * @param[in ] : data_num - 数据个数
 * @param[in ] : subtype  - 消息子类型
 * @param[in ] : opcode   - 操作码
 * @param[in ] : index    - 索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 18:09:53
 * @note       :
 */
int ftm_mplsif_msg(void *pdata, int data_len, int data_num, uint8_t subtype,
                        enum IPC_OPCODE opcode, uint32_t index)
{
    enum BACKUP_E pw_backup = BACKUP_STATUS_INVALID;
    uint32_t      pwindex   = 0;
    int           i         = 0;

    FTM_LOG_DEBUG();

    for (i=0; i<data_num; i++)
    {
        if (opcode == IPC_OPCODE_ADD)
        {
            pwindex = *(uint32_t *)pdata;

            ftm_pw_bind_if(index, pwindex, subtype);
        }
        else if (opcode == IPC_OPCODE_DELETE)
        {
            pwindex = *(uint32_t *)pdata;

            ftm_pw_unbind_if(index, pwindex, subtype);
        }
        else if (opcode == IPC_OPCODE_UPDATE)
        {
            if (MPLSIF_INFO_PW_BACKUP == subtype)
            {
                pw_backup = *(uint32_t *)pdata;

                ftm_pw_backup_status_update(index, pw_backup);
            }
        }
    }

    return ERRNO_SUCCESS;
}


