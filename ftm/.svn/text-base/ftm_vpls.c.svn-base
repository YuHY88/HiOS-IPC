/**
 * @file      : ftm_vpls.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月9日 8:55:42
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/memory.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <lib/zassert.h>
#include <ftm/ftm.h>
#include <ftm/ftm_vpls.h>
#include <ftm/ftm_ifm.h>


struct hash_table ftm_vsi_table;


/**
 * @brief      : 计算 ftm vsi 哈希表 key 值
 * @param[in ] : hash_key - vsi id
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月9日 8:57:49
 * @note       :
 */
static unsigned int ftm_vsi_compute(void *hash_key)
{
    return (unsigned int)hash_key;
}


/**
 * @brief      : 比较 ftm vsi 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - vsi id
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月9日 8:58:07
 * @note       :
 */
static int ftm_vsi_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;

    if (pitem == NULL || hash_key == NULL)
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
 * @brief      : ftm vsi 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月9日 8:58:34
 * @note       :
 */
void ftm_vsi_table_init(unsigned int size)
{
    hios_hash_init(&ftm_vsi_table, size, ftm_vsi_compute, ftm_vsi_compare);
}


/**
 * @brief      : 释放 ftm vsi 节点
 * @param[in ] : pftm_vsi - ftm vsi 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月9日 8:59:16
 * @note       :
 */
static void ftm_vsi_free(struct ftm_vsi *pftm_vsi)
{
    if (NULL != pftm_vsi)
    {
        if (pftm_vsi->aclist != NULL)
        {
            list_free(pftm_vsi->aclist);
        }

        if (pftm_vsi->pwlist != NULL)
        {
            list_free(pftm_vsi->pwlist);
        }

        if (pftm_vsi->blacklist != NULL)
        {
            list_free(pftm_vsi->blacklist);
        }

        if (pftm_vsi->whitelist != NULL)
        {
            list_free(pftm_vsi->whitelist);
        }

        XFREE(MTYPE_VSI_ENTRY, pftm_vsi);
    }

    return;
}


/**
 * @brief      : 创建 ftm vsi 节点
 * @param[in ] : vsi_id - vsi id
 * @param[out] :
 * @return     : 成功返回分配内存的 ftm vsi 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:00:17
 * @note       :
 */
struct ftm_vsi *ftm_vsi_create(uint16_t vsi_id)
{
    struct ftm_vsi *pftm_vsi = NULL;

    FTM_LOG_DEBUG();

    pftm_vsi = (struct ftm_vsi *)XCALLOC(MTYPE_VSI_ENTRY, sizeof(struct ftm_vsi));
    if (NULL == pftm_vsi)
    {
        FTM_LOG_ERROR("malloc\n");

        return NULL;
    }

    pftm_vsi->vpls.vsi_id    = vsi_id;
    pftm_vsi->vpls.mac_learn = MAC_LEARN_ENABLE;

    pftm_vsi->pwlist = list_new();
    if (pftm_vsi->pwlist == NULL)
    {
        FTM_LOG_ERROR("pwlist\n");

        ftm_vsi_free(pftm_vsi);

        return NULL;
    }

    pftm_vsi->aclist = list_new();
    if (pftm_vsi->aclist == NULL)
    {
        FTM_LOG_ERROR("aclist\n");

        ftm_vsi_free(pftm_vsi);

        return NULL;
    }

    pftm_vsi->blacklist = list_new();
    if (pftm_vsi->blacklist == NULL)
    {
        FTM_LOG_ERROR("blacklist\n");

        ftm_vsi_free(pftm_vsi);

        return NULL;
    }

    pftm_vsi->whitelist = list_new();
    if (pftm_vsi->whitelist == NULL)
    {
        FTM_LOG_ERROR("whitelist\n");

        ftm_vsi_free(pftm_vsi);

        return NULL;
    }

    return pftm_vsi;
}


/**
 * @brief      : ftm vsi 添加
 * @param[in ] : pftm_vsi - ftm vsi 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:01:03
 * @note       :
 */
int ftm_vsi_add(struct ftm_vsi *pftm_vsi)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    if (pftm_vsi == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ftm_vsi_table.hash_size == ftm_vsi_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        FTM_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)(uint32_t)pftm_vsi->vpls.vsi_id;
    pitem->data     = pftm_vsi;

    hios_hash_add(&ftm_vsi_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi 删除
 * @param[in ] : vsi_id - vsi id
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:02:51
 * @note       :
 */
int ftm_vsi_delete(uint16_t vsi_id)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    pitem = hios_hash_find(&ftm_vsi_table, (void *)(uint32_t)vsi_id);
    if (pitem == NULL)
    {
        return ERRNO_SUCCESS;
    }

    ftm_vsi_delete_pw_all(vsi_id);
    ftm_vsi_delete_if_all(vsi_id);
    ftm_vsi_delete_mac_all(vsi_id, VSI_INFO_MAC_BLACKLIST);
    ftm_vsi_delete_mac_all(vsi_id, VSI_INFO_MAC_WHITELIST);

    hios_hash_delete(&ftm_vsi_table, pitem);
    ftm_vsi_free(pitem->data);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi 查找
 * @param[in ] : vsi_id - vsi id
 * @param[out] :
 * @return     : 成功返回 vsi 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:03:31
 * @note       :
 */
struct ftm_vsi *ftm_vsi_lookup(uint16_t vsi_id)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    pitem = hios_hash_find(&ftm_vsi_table, (void *)(uint32_t)vsi_id);
    if (pitem == NULL)
    {
        return NULL;
    }

    return (struct ftm_vsi *)pitem->data;
}


/**
 * @brief      : ftm vsi 添加 mac 黑/白名单
 * @param[in ] : vsi_id - vsi id
 * @param[in ] : info   - 消息类型
 * @param[in ] : pmac   - mac 地址
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:03:52
 * @note       :
 */
int ftm_vsi_add_mac(uint16_t vsi_id, enum VSI_INFO info, uint8_t *pmac)
{
    struct ftm_vsi *pftm_vsi = NULL;
    struct list    *plist    = NULL;
    uint8_t        *pmac_new = NULL;

    FTM_LOG_DEBUG();

    if ((0 == vsi_id) || (NULL == pmac))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pftm_vsi = ftm_vsi_lookup(vsi_id);
    if (NULL == pftm_vsi)
    {
        return ERRNO_SUCCESS;
    }

    if (VSI_INFO_MAC_BLACKLIST == info)
    {
        plist = pftm_vsi->blacklist;
    }
    else if (VSI_INFO_MAC_WHITELIST == info)
    {
        plist = pftm_vsi->whitelist;
    }

    if (NULL == plist)
    {
        return ERRNO_SUCCESS;
    }

    if (NULL != ftm_vsi_lookup_mac(plist, pmac))
    {
        return ERRNO_SUCCESS;
    }

    pmac_new = (uint8_t *)XCALLOC(MTYPE_VSI_ENTRY, MAC_LEN);
    if (NULL == pmac_new)
    {
        FTM_LOG_ERROR("malloc mac list\n");

        return ERRNO_MALLOC;
    }

    memcpy(pmac_new, pmac, MAC_LEN);
    listnode_add(plist, pmac_new);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi 删除 mac 黑/白名单
 * @param[in ] : vsi_id - vsi id
 * @param[in ] : info   - 消息类型
 * @param[in ] : pmac   - mac 地址
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:04:58
 * @note       :
 */
int ftm_vsi_delete_mac(uint16_t vsi_id, enum VSI_INFO info, uint8_t *pmac)
{
    struct ftm_vsi *pftm_vsi    = NULL;
    struct list    *plist       = NULL;
    uint8_t        *pmac_lookup = NULL;

    FTM_LOG_DEBUG();

    if ((0 == vsi_id) || (NULL == pmac))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pftm_vsi = ftm_vsi_lookup(vsi_id);
    if (NULL == pftm_vsi)
    {
        return ERRNO_SUCCESS;
    }

    if (VSI_INFO_MAC_BLACKLIST == info)
    {
        plist = pftm_vsi->blacklist;
    }
    else if (VSI_INFO_MAC_WHITELIST == info)
    {
        plist = pftm_vsi->whitelist;
    }

    if (NULL == plist)
    {
        return ERRNO_SUCCESS;
    }

    pmac_lookup = ftm_vsi_lookup_mac(plist, pmac);
    if (NULL != pmac_lookup)
    {
        listnode_delete(plist, pmac_lookup);
        XFREE(MTYPE_VSI_ENTRY, pmac_lookup);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi 清空 mac 黑/白名单
 * @param[in ] : vsi_id - vsi id
 * @param[in ] : info   - 消息类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:06:16
 * @note       :
 */
int ftm_vsi_delete_mac_all(uint16_t vsi_id, enum VSI_INFO info)
{
    struct ftm_vsi  *pftm_vsi  = NULL;
    struct list     *plist     = NULL;
    struct listnode *pnode     = NULL;
    struct listnode *pnodenext = NULL;
    uint8_t         *pmac      = NULL;

    FTM_LOG_DEBUG();

    pftm_vsi = ftm_vsi_lookup(vsi_id);
    if (NULL == pftm_vsi)
    {
        return ERRNO_SUCCESS;
    }

    if (VSI_INFO_MAC_BLACKLIST_ALL == info)
    {
        plist = pftm_vsi->blacklist;
    }
    else if (VSI_INFO_MAC_WHITELIST_ALL == info)
    {
        plist = pftm_vsi->whitelist;
    }

    if (NULL == plist)
    {
        return ERRNO_SUCCESS;
    }

    for (ALL_LIST_ELEMENTS(plist, pnode, pnodenext, pmac))
    {
        list_delete_node(plist, pnode);
        XFREE(MTYPE_VSI_ENTRY, pmac);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi 查询 mac 黑/白名单
 * @param[in ] : plist - 链表结构
 * @param[in ] : pmac  - mac 地址
 * @param[out] :
 * @return     : 成功返回 mac 地址，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:07:00
 * @note       :
 */
uint8_t *ftm_vsi_lookup_mac(struct list *plist, uint8_t *pmac)
{
    struct listnode *pnode = NULL;
    uint8_t         *smac  = NULL;

    FTM_LOG_DEBUG();

    if ((NULL == plist) || (NULL == pmac))
    {
        return NULL;
    }

    for (ALL_LIST_ELEMENTS_RO(plist, pnode, smac))
    {
        if (0 == memcmp(pmac, smac, MAC_LEN))
        {
            return smac;
        }
    }

    return NULL;
}


/**
 * @brief      : ftm vsi 添加 pw
 * @param[in ] : vsi_id  - vsi id
 * @param[in ] : pwindex - pw 索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:07:28
 * @note       :
 */
int ftm_vsi_add_pw(uint16_t vsi_id, uint32_t pwindex)
{
    struct ftm_vsi *pftm_vsi = NULL;
    struct pw_info *ppwinfo  = NULL;

    FTM_LOG_DEBUG();

    pftm_vsi = ftm_vsi_lookup(vsi_id);
    if (pftm_vsi == NULL)
    {
        return ERRNO_SUCCESS;
    }

    ppwinfo = ftm_vsi_lookup_pw(vsi_id, pwindex);
    if (ppwinfo != NULL)
    {
        listnode_delete(pftm_vsi->pwlist, ppwinfo);
    }

    ppwinfo = ftm_pw_lookup(pwindex);
    if (ppwinfo == NULL)
    {
        return ERRNO_FAIL;
    }

    listnode_add(pftm_vsi->pwlist, ppwinfo);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi 删除 pw
 * @param[in ] : vsi_id  - vsi id
 * @param[in ] : pwindex - pw 索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:08:08
 * @note       :
 */
int ftm_vsi_delete_pw(uint16_t vsi_id, uint32_t pwindex)
{
    struct ftm_vsi *pftm_vsi = NULL;
    struct pw_info *ppwinfo  = NULL;

    FTM_LOG_DEBUG();

    pftm_vsi = ftm_vsi_lookup(vsi_id);
    if (pftm_vsi == NULL)
    {
        return ERRNO_SUCCESS;
    }

    ppwinfo = ftm_vsi_lookup_pw(vsi_id, pwindex);
    if (ppwinfo == NULL)
    {
        return ERRNO_SUCCESS;
    }

    ppwinfo->vpls.vsi_id = 0;

    listnode_delete(pftm_vsi->pwlist, ppwinfo);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi 删除所有 pw
 * @param[in ] : vsi_id  - vsi id
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:08:36
 * @note       :
 */
int ftm_vsi_delete_pw_all(uint16_t vsi_id)
{
    struct listnode *pnode     = NULL;
    struct listnode *pnextnode = NULL;
    struct ftm_vsi  *pftm_vsi  = NULL;
    struct pw_info  *ppwinfo   = NULL;

    FTM_LOG_DEBUG();

    pftm_vsi = ftm_vsi_lookup(vsi_id);
    if ((pftm_vsi == NULL) || (pftm_vsi->pwlist == NULL))
    {
        return ERRNO_SUCCESS;
    }

    for (ALL_LIST_ELEMENTS(pftm_vsi->pwlist, pnode, pnextnode, ppwinfo))
    {
        ftm_pw_delete(ppwinfo->pwindex);
        list_delete_node(pftm_vsi->pwlist, pnode);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi 查找 pw
 * @param[in ] : vsi_id  - vsi id
 * @param[in ] : pwindex - pw 索引
 * @param[out] :
 * @return     : 成功返回 pw 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:09:17
 * @note       :
 */
struct pw_info *ftm_vsi_lookup_pw(uint16_t vsi_id, uint32_t pwindex)
{
    struct listnode *pnode    = NULL;
    struct ftm_vsi  *pftm_vsi = NULL;
    struct pw_info  *ppw      = NULL;

    FTM_LOG_DEBUG();

    pftm_vsi = ftm_vsi_lookup(vsi_id);
    if (pftm_vsi == NULL)
    {
        return NULL;
    }

    for (ALL_LIST_ELEMENTS_RO(pftm_vsi->pwlist, pnode, ppw))
    {
        if (ppw->pwindex == pwindex)
        {
            break;
        }
    }

    return ppw;
}


/**
 * @brief      : ftm vsi 添加接口
 * @param[in ] : vsi_id  - vsi id
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:10:54
 * @note       :
 */
int ftm_vsi_add_if(uint16_t vsi_id, uint32_t ifindex)
{
    struct listnode *pnode    = NULL;
    struct ftm_vsi  *pftm_vsi = NULL;
    struct ftm_ifm  *pifm     = NULL;

    FTM_LOG_DEBUG();

    pftm_vsi = ftm_vsi_lookup(vsi_id);
    if (pftm_vsi == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pnode = listnode_lookup(pftm_vsi->aclist, (void *)ifindex);
    if (pnode != NULL)
    {
        return ERRNO_SUCCESS;
    }

    listnode_add(pftm_vsi->aclist, (void *)ifindex);

    /* 将 vsi 添加至 l2if */
    pifm = ftm_ifm_lookup(ifindex);
    if (NULL == pifm)
    {
        return ERRNO_SUCCESS;
    }

    if (pifm->pl2if == NULL)
    {
        pifm->pl2if = ftm_ifm_create_l2if(pifm);
        if (NULL == pifm->pl2if)
        {
            FTM_LOG_ERROR("creat l2if\n");

            return ERRNO_MALLOC;
        }
    }

    pifm->pl2if->vsi = vsi_id;

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi 删除接口
 * @param[in ] : vsi_id  - vsi id
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:11:45
 * @note       :
 */
int ftm_vsi_delete_if(uint16_t vsi_id, uint32_t ifindex)
{
    struct listnode  *pnode      = NULL;
    struct ftm_vsi   *pftm_vsi   = NULL;
    struct ftm_ifm   *pifm       = NULL;

    FTM_LOG_DEBUG();

    pftm_vsi = ftm_vsi_lookup(vsi_id);
    if (pftm_vsi == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pnode = listnode_lookup(pftm_vsi->aclist, (void *)ifindex);
    if (pnode == NULL)
    {
        return ERRNO_SUCCESS;
    }

    listnode_delete(pftm_vsi->aclist, (void *)ifindex);

    /* 从 l2if 中清除 vsi */
    pifm = ftm_ifm_lookup(ifindex);
    if (NULL == pifm)
    {
        return ERRNO_SUCCESS;
    }

    if (pifm->pl2if == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pifm->pl2if->vsi = 0;

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi 删除所有接口
 * @param[in ] : vsi_id  - vsi id
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:12:13
 * @note       :
 */
int ftm_vsi_delete_if_all(uint16_t vsi_id)
{
    struct listnode *pnode     = NULL;
    struct listnode *pnextnode = NULL;
    struct ftm_vsi  *pftm_vsi  = NULL;
    struct ftm_ifm  *pifm      = NULL;
    uint32_t        *pifindex  = NULL;

    FTM_LOG_DEBUG();

    pftm_vsi = ftm_vsi_lookup(vsi_id);
    if (pftm_vsi == NULL)
    {
        return ERRNO_SUCCESS;
    }
    else if (pftm_vsi->aclist == NULL)
    {
        return ERRNO_SUCCESS;
    }

    for (ALL_LIST_ELEMENTS(pftm_vsi->aclist, pnode, pnextnode, pifindex))
    {
        list_delete_node(pftm_vsi->aclist, pnode);

        pifm = ftm_ifm_lookup((uint32_t)pifindex);
        if ((pifm != NULL) && (pifm->pl2if != NULL))
        {
            pifm->pl2if->vsi = 0;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi mac 学习使能更新
 * @param[in ] : vsi_id    - vsi id
 * @param[in ] : mac_learn - mac 学习使能状态
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:12:36
 * @note       :
 */
int ftm_vsi_mac_learn(uint16_t vsi_id, enum MAC_LEARN mac_learn)
{
    struct ftm_vsi *pftm_vsi = NULL;

    FTM_LOG_DEBUG();

    pftm_vsi = ftm_vsi_lookup(vsi_id);
    if (pftm_vsi == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pftm_vsi->vpls.mac_learn = mac_learn;

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi mac 学习上限更新
 * @param[in ] : vsi_id    - vsi id
 * @param[in ] : limit_num - 上限数
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:13:22
 * @note       :
 */
int ftm_vsi_mac_learn_limit(uint16_t vsi_id, int limit_num)
{
    struct ftm_vsi *pftm_vsi = NULL;

    FTM_LOG_DEBUG();

    pftm_vsi = ftm_vsi_lookup(vsi_id);
    if (pftm_vsi == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pftm_vsi->vpls.learning_limit = limit_num;

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm vsi 更新 sd-tag
 * @param[in ] : pvpls_info - vpls 信息结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:16:00
 * @note       :
 */
int ftm_vsi_update_pw_tag(struct vpls_info *pvpls_info)
{
    struct ftm_vsi *pftm_vsi = NULL;

    FTM_LOG_DEBUG();

    if (NULL == pvpls_info)
    {
        return ERRNO_SUCCESS;
    }

    pftm_vsi = ftm_vsi_lookup(pvpls_info->vsi_id);
    if (pftm_vsi == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pftm_vsi->vpls.tag_flag = pvpls_info->tag_flag;
    pftm_vsi->vpls.tpid     = pvpls_info->tpid;
    pftm_vsi->vpls.vlan     = pvpls_info->vlan;

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 消息接收
 * @param[in ] : pdata    - 接收数据
 * @param[in ] : data_len - 数据长度
 * @param[in ] : data_num - 数据个数
 * @param[in ] : subtype  - 消息子类型
 * @param[in ] : opcode   - 操作码
 * @param[in ] : index    - 索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:16:58
 * @note       :
 */
int ftm_vsi_msg(void *pdata, int data_len, int data_num, uint8_t subtype,
                        enum IPC_OPCODE opcode, uint32_t index)
{
    struct vpls_info *pvpls_info = NULL;
    struct pw_info   *ppw        = NULL;
    struct ftm_vsi   *pftm_vsi   = NULL;
    uint16_t          vsi_id;
    int               i;

    FTM_LOG_DEBUG();

    for (i=0; i<data_num; i++)
    {
        if (subtype == VSI_INFO_PW)
        {
            ppw = (struct pw_info *)pdata;
            if (opcode == IPC_OPCODE_DELETE)
            {
                ftm_vsi_delete_pw(ppw->vpls.vsi_id, ppw->pwindex);
            }
            else if (opcode == IPC_OPCODE_ADD)
            {
                ftm_vsi_add_pw(ppw->vpls.vsi_id, ppw->pwindex);
            }
        }
        else if (subtype == VSI_INFO_AC)
        {
            vsi_id = *(uint16_t *)pdata;

            if (opcode == IPC_OPCODE_DELETE)
            {
                ftm_vsi_delete_if(vsi_id, index);
            }
            else if (opcode == IPC_OPCODE_ADD)
            {
                ftm_vsi_add_if(vsi_id, index);
            }
        }
        else if ((subtype == VSI_INFO_MAC_BLACKLIST) || (subtype == VSI_INFO_MAC_WHITELIST))
        {
            if (opcode == IPC_OPCODE_ADD)
            {
                ftm_vsi_add_mac(index, subtype, pdata);
            }
            else if (opcode == IPC_OPCODE_DELETE)
            {
                ftm_vsi_delete_mac(index, subtype, pdata);
            }
        }
        else if ((subtype == VSI_INFO_MAC_BLACKLIST_ALL) || (subtype == VSI_INFO_MAC_WHITELIST_ALL))
        {
            ftm_vsi_delete_mac_all(index, subtype);
        }
        else if (subtype == VSI_INFO_MAC_LEARN)
        {
            pvpls_info = (struct vpls_info *)pdata;

            ftm_vsi_mac_learn(pvpls_info->vsi_id, pvpls_info->mac_learn);
        }
        else if (subtype == VSI_INFO_LEARN_LIMIT)
        {
            pvpls_info = (struct vpls_info *)pdata;

            ftm_vsi_mac_learn_limit(pvpls_info->vsi_id,pvpls_info->learning_limit);
        }
        else if (subtype == VSI_INFO_ENCAP )
        {
            if (opcode == IPC_OPCODE_UPDATE )
            {
                 pvpls_info = ( struct vpls_info * ) pdata;

                 ftm_vsi_update_pw_tag ( pvpls_info );
            }
        }
        else if (subtype == VSI_INFO_VSI)
        {
            vsi_id = *(uint16_t *)pdata;

            if (opcode == IPC_OPCODE_DELETE)
            {
                ftm_vsi_delete(vsi_id);
            }
            else if (opcode == IPC_OPCODE_ADD)
            {
                pftm_vsi = ftm_vsi_lookup(vsi_id);
                if (NULL != pftm_vsi)
                {
                    ftm_vsi_delete(vsi_id);
                }

                pftm_vsi = ftm_vsi_create(vsi_id);
                if (pftm_vsi != NULL)
                {
                    ftm_vsi_add(pftm_vsi);
                }
            }
        }
    }

    return ERRNO_SUCCESS;
}


