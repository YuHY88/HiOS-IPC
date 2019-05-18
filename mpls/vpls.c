/**
 * @file      : vpls.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月5日 9:45:20
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <zebra.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include "mpls_main.h"
#include "mpls.h"
#include "pw_cmd.h"
#include "vpls.h"
#include "vpls_cmd.h"
#include "tunnel.h"

int vsi_get_vsi_black_white_mac_bulk(struct mpls_mac *pindex, uint32_t data_len, struct mpls_mac*mac_buf);
int vsi_get_pw_l2vc_bulk(uint32_t vsi_id, struct l2vc_entry *pindex, uint32_t data_len, struct l2vc_entry *l2vc_buf);
int vsi_get_smac_bulk(struct mpls_smac *pindex, uint32_t data_len, struct mpls_smac*mac_buf);

struct hash_table vsi_table;


/**
 * @brief      : 计算 vsi 哈希表 key 值
 * @param[in ] : hash_key - vsi id
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月5日 9:49:28
 * @note       :
 */
static unsigned int vsi_compute(void *hash_key)
{
    return (unsigned int)hash_key;
}


/**
 * @brief      : 比较 vsi 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - vsi id
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:00:03
 * @note       :
 */
static int vsi_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;

    if ((pitem == NULL) || (hash_key == NULL))
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
 * @brief      : vsi 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:04:25
 * @note       :
 */
void vsi_table_init(uint32_t size)
{
    hios_hash_init(&vsi_table, size, vsi_compute, vsi_compare);

    return;
}


/**
 * @brief      : 释放 vsi 节点
 * @param[in ] : pvsi - vsi 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:07:23
 * @note       :
 */
void vsi_free(struct vsi_entry *pvsi)
{
    if (NULL != pvsi)
    {
        if (pvsi->aclist != NULL)
        {
            list_free(pvsi->aclist);
        }

        if (pvsi->pwlist != NULL)
        {
            list_free(pvsi->pwlist);
        }

        if (pvsi->blacklist != NULL)
        {
            list_free(pvsi->blacklist);
        }

        if (pvsi->whitelist != NULL)
        {
            list_free(pvsi->whitelist);
        }

        XFREE(MTYPE_VSI_ENTRY, pvsi);
    }

    return;
}


/**
 * @brief      : 创建 vsi 节点
 * @param[in ] : vsi_id - vsi id
 * @param[out] :
 * @return     : 成功返回分配内存的 vsi 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:08:04
 * @note       :
 */
struct vsi_entry *vsi_create(uint16_t vsi_id)
{
    struct vsi_entry *pvsi = NULL;

    MPLS_LOG_DEBUG();

    pvsi = (struct vsi_entry *)XCALLOC(MTYPE_VSI_ENTRY, sizeof(struct vsi_entry));
    if (NULL == pvsi)
    {
        MPLS_LOG_ERROR("malloc\n");

        vsi_free(pvsi);

        return NULL;
    }

    pvsi->failback            = FAILBACK_ENABLE;
    pvsi->wtr                 = 30;
    pvsi->vpls.vsi_id         = vsi_id;
    pvsi->vpls.mac_learn      = MAC_LEARN_ENABLE;
    pvsi->vpls.tag_flag       = VSI_ENCAP_ETHERNET_RAW;
    pvsi->vpls.learning_limit = 0;

    pvsi->pwlist = list_new();
    if (pvsi->pwlist == NULL)
    {
        MPLS_LOG_ERROR("malloc pwlist\n");

        vsi_free(pvsi);

        return NULL;
    }

    pvsi->aclist = list_new();
    if (pvsi->aclist == NULL)
    {
        MPLS_LOG_ERROR("malloc aclist\n");

        vsi_free(pvsi);

        return NULL;
    }

    pvsi->blacklist = list_new();
    if (pvsi->blacklist == NULL)
    {
        MPLS_LOG_ERROR("malloc blacklist\n");

        vsi_free(pvsi);

        return NULL;
    }

    pvsi->whitelist = list_new();
    if (pvsi->whitelist == NULL)
    {
        MPLS_LOG_ERROR("malloc whitelist\n");

        vsi_free(pvsi);

        return NULL;
    }

    return pvsi;
}


/**
 * @brief      : vsi 添加
 * @param[in ] : pvsi - vsi 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:09:06
 * @note       :
 */
int vsi_add(struct vsi_entry *pvsi)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (NULL == pvsi)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (vsi_table.hash_size == vsi_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        MPLS_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)(int)pvsi->vpls.vsi_id;
    pitem->data     = pvsi;

    hios_hash_add(&vsi_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 删除
 * @param[in ] : vsi_id - vsi id
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:09:58
 * @note       :
 */
int vsi_delete(uint16_t vsi_id)
{
    struct hash_bucket *pitem = NULL;
    struct vsi_entry   *pvsi  = NULL;

    MPLS_LOG_DEBUG();

    pitem = hios_hash_find(&vsi_table, (void *)(int)vsi_id);
    if (pitem == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pvsi = (struct vsi_entry *)pitem->data;

    hios_hash_delete(&vsi_table, pitem);
    vsi_free(pvsi);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 查找
 * @param[in ] : vsi_id - vsi id
 * @param[out] :
 * @return     : 成功返回 vsi 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:10:43
 * @note       :
 */
struct vsi_entry *vsi_lookup(uint16_t vsi_id)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    pitem = hios_hash_find(&vsi_table, (void *)(int)vsi_id);
    if (pitem == NULL)
    {
        return NULL;
    }

    return (struct vsi_entry *)pitem->data;
}


/**
 * @brief      : vsi 添加 mac 黑/白名单
 * @param[in ] : plist - 链表结构
 * @param[in ] : pmac  - mac 地址
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:12:02
 * @note       :
 */
int vsi_add_mac(struct list *plist, uint8_t *pmac)
{
    uint8_t *pmac_new = NULL;

    if ((NULL == plist) || (NULL == pmac))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pmac_new = (uint8_t *)XCALLOC(MTYPE_VSI_ENTRY, MAC_LEN);
    if (NULL == pmac_new)
    {
        return ERRNO_MALLOC;
    }

    memcpy(pmac_new, pmac, MAC_LEN);
    listnode_add(plist, pmac_new);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 删除 mac 黑/白名单
 * @param[in ] : plist - 链表结构
 * @param[in ] : pmac  - mac 地址
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:16:07
 * @note       :
 */
int vsi_delete_mac(struct list *plist, uint8_t *pmac)
{
    uint8_t *pmac_lookup = NULL;

    if ((NULL == plist) || (NULL == pmac))
    {
        return ERRNO_SUCCESS;
    }

    pmac_lookup = vsi_lookup_mac(plist, pmac);
    if (NULL != pmac_lookup)
    {
        listnode_delete(plist, pmac_lookup);
        XFREE(MTYPE_VSI_ENTRY, pmac_lookup);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 清空 mac 黑/白名单
 * @param[in ] : plist - 链表结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:16:38
 * @note       :
 */
int vsi_delete_mac_all(struct list *plist)
{
    struct listnode *pnode     = NULL;
    struct listnode *pnodenext = NULL;
    uint8_t         *pmac      = NULL;

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
 * @brief      : vsi 查询 mac 黑/白名单
 * @param[in ] : plist - 链表结构
 * @param[in ] : pmac  - mac 地址
 * @param[out] :
 * @return     : 成功返回 mac 地址，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:18:04
 * @note       :
 */
uint8_t *vsi_lookup_mac(struct list *plist, uint8_t *pmac)
{
    struct listnode *pnode = NULL;
    uint8_t         *smac  = NULL;

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
 * @brief      : vsi 添加 pw
 * @param[in ] : pvsi  - vsi 结构
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:19:24
 * @note       :
 */
int vsi_add_pw(struct vsi_entry *pvsi, struct l2vc_entry *pl2vc)
{
    MPLS_LOG_DEBUG();

    if ((pl2vc == NULL) || (pvsi == NULL))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pl2vc->pwinfo.vpls.vsi_id    = pvsi->vpls.vsi_id;
    pl2vc->pwinfo.vpls.mac_learn = pvsi->vpls.mac_learn;
    pl2vc->pwinfo.vpls.tpid      = pvsi->vpls.tpid;
    pl2vc->pwinfo.vpls.vlan      = pvsi->vpls.vlan;
    pl2vc->pwinfo.vpls.tag_flag  = pvsi->vpls.tag_flag;

    listnode_add(pvsi->pwlist, pl2vc);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 删除 pw
 * @param[in ] : pvsi  - vsi 结构
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:20:36
 * @note       :
 */
int vsi_delete_pw(struct vsi_entry *pvsi, struct l2vc_entry *pl2vc)
{
    MPLS_LOG_DEBUG();

    if ((pl2vc == NULL) || (pvsi == NULL))
    {
        return ERRNO_FAIL;
    }

    pl2vc->pwinfo.vpls.vsi_id = 0;

    if (pl2vc->pwinfo.up_flag == LINK_UP)
    {
        pl2vc->pwinfo.up_flag = LINK_DOWN;
        gmpls.pw_up_num--;
    }

    listnode_delete(pvsi->pwlist, pl2vc);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 删除所有 pw
 * @param[in ] : pvsi - vsi 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:25:20
 * @note       :
 */
int vsi_delete_pw_all(struct vsi_entry *pvsi)
{
    struct l2vc_entry *pl2vc     = NULL;
    struct listnode   *pnode     = NULL;
    struct listnode   *pnextnode = NULL;

    MPLS_LOG_DEBUG();

    if ((pvsi == NULL) || (pvsi->pwlist == NULL))
    {
        return ERRNO_SUCCESS;
    }

    for (ALL_LIST_ELEMENTS(pvsi->pwlist, pnode, pnextnode, pl2vc))
    {
        pl2vc->pwinfo.vpls.vsi_id  = 0;
        pl2vc->ppw_backup          = NULL;
        pl2vc->pwinfo.backup_index = 0;

        MPLS_TIMER_DEL(pl2vc->pwtr_timer);

        pw_delete_lsp(&pl2vc->pwinfo);
        pw_free_lsp_index(&pl2vc->pwinfo);

        pl2vc->pwinfo.up_flag  = LINK_DOWN;
        pl2vc->pwinfo.admin_up = LINK_DOWN;

        if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
        {
            l2vc_delete_martini(pl2vc);
        }
        else
        {
            if (pl2vc->pwinfo.up_flag == LINK_UP)
            {
                l2vc_unbind_lsp_tunnel(pl2vc);
                tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc->pwinfo.nhp_index), pl2vc);
            }
        }

        list_delete_node(pvsi->pwlist, pnode);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 添加接口
 * @param[in ] : pvsi    - vsi 结构
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:26:29
 * @note       :
 */
int vsi_add_if(struct vsi_entry *pvsi, uint32_t ifindex)
{
    struct mpls_if *pif = NULL;

    MPLS_LOG_DEBUG();

    if ((pvsi == NULL) || (ifindex == 0))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    /* AC 类型检查 */
    switch (IFM_TYPE_ID_GET(ifindex))
    {
        case IFNET_TYPE_ETHERNET:
		case IFNET_TYPE_GIGABIT_ETHERNET:
		case IFNET_TYPE_XGIGABIT_ETHERNET:
        case IFNET_TYPE_ETH_SUBPORT:
        case IFNET_TYPE_TRUNK:
            break;
        default:
            return ERRNO_FAIL;
    }

    pif = mpls_if_get(ifindex);
    if (pif == NULL)
    {
        return ERRNO_FAIL;
    }

    if (pif->mode != IFNET_MODE_L2)
    {
        return ERRNO_MODE_NOT_L2;
    }

    if (pif->vpn != 0)
    {
        return ERRNO_ALREADY_BIND_L2VPN;
    }

    if ((pif->ppw_master != NULL) || (pif->ppw_slave != NULL))
    {
        return ERRNO_ALREADY_BIND_PW;
    }

    pif->vpn = pvsi->vpls.vsi_id;

    if (NULL == listnode_lookup(pvsi->aclist, (void *)ifindex))
    {
        listnode_add(pvsi->aclist, (void *)ifindex);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 删除接口
 * @param[in ] : pvsi    - vsi 结构
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:27:28
 * @note       :
 */
int vsi_delete_if(struct vsi_entry *pvsi, uint32_t ifindex)
{
    struct mpls_if *pif = NULL;

    MPLS_LOG_DEBUG();

    if ((pvsi == NULL) || (ifindex == 0))
    {
        return ERRNO_FAIL;
    }

    pif = mpls_if_lookup(ifindex);
    if (pif == NULL)
    {
        return ERRNO_SUCCESS;
    }

    if (pif->vpn != pvsi->vpls.vsi_id)
    {
        return ERRNO_SUCCESS;
    }

    pif->vpn = 0;

    listnode_delete(pvsi->aclist, (void *)ifindex);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 删除所有接口
 * @param[in ] : pvsi - vsi 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:28:13
 * @note       :
 */
int vsi_delete_if_all(struct vsi_entry *pvsi)
{
    struct listnode *pnode     = NULL;
    struct listnode *pnextnode = NULL;
    struct mpls_if  *pif       = NULL;
    uint32_t        *pifindex  = NULL;

    MPLS_LOG_DEBUG();

    if ((pvsi == NULL) || (pvsi->aclist == NULL))
    {
        return ERRNO_SUCCESS;
    }

    for (ALL_LIST_ELEMENTS(pvsi->aclist, pnode, pnextnode, pifindex))
    {
        list_delete_node(pvsi->aclist, pnode);

        pif = mpls_if_lookup((uint32_t)pifindex);
        if (pif != NULL)
        {
            pif->vpn = 0;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 默认切换处理
 * @param[in ] : pl2vc_master - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年4月12日 10:41:01
 * @note       :
 */
static int vsi_set_pw_status_default(struct l2vc_entry *pl2vc_master)
{
    struct l2vc_entry *pl2vc_backup = NULL;
    enum BACKUP_E pw_backup = BACKUP_STATUS_MASTER;
    int ret;

    MPLS_LOG_DEBUG();

    if (NULL == pl2vc_master)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pl2vc_backup = pl2vc_master->ppw_backup;
    if (NULL == pl2vc_backup)
    {
        return ERRNO_SUCCESS;
    }

    /* 备存在情况下，只有主 down 备 up 才切换至备 */
    if ((LINK_UP == pl2vc_backup->pwinfo.up_flag)
        && (LINK_DOWN == pl2vc_master->pwinfo.up_flag))
    {
        pw_backup = BACKUP_STATUS_SLAVE;
    }

    /* 状态发生变化，切换 */
    if (pl2vc_master->pw_backup != pw_backup)
    {
        pl2vc_master->pw_backup = pw_backup;

        ret = vsi_pw_status_download(pl2vc_master->pwinfo.pwindex, &pl2vc_master->pw_backup);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("IPC send pw status\n");
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 主备 pw 不回切处理
 * @param[in ] : pl2vc_master - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年4月12日 10:40:48
 * @note       :
 */
static int vsi_set_pw_status_non_failback(struct l2vc_entry *pl2vc_master)
{
    struct l2vc_entry *pl2vc_backup = NULL;
    enum BACKUP_E pw_backup = BACKUP_STATUS_MASTER;
    int ret;

    MPLS_LOG_DEBUG();

    if (NULL == pl2vc_master)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pl2vc_backup = pl2vc_master->ppw_backup;
    if (NULL == pl2vc_backup)
    {
        return ERRNO_SUCCESS;
    }

    /* pw 备 up，主 down，或者当前工作在备，则工作在备，否则工作在主 */
    if (LINK_UP == pl2vc_backup->pwinfo.up_flag)
    {
        if (LINK_DOWN == pl2vc_master->pwinfo.up_flag)
        {
            pw_backup = BACKUP_STATUS_SLAVE;
        }
        else
        {
            if (pl2vc_master->pw_backup == BACKUP_STATUS_SLAVE)
            {
                pw_backup = BACKUP_STATUS_SLAVE;
            }
        }
    }

    /* 状态发生变化，切换 */
    if (pl2vc_master->pw_backup != pw_backup)
    {
        pl2vc_master->pw_backup = pw_backup;

        ret = vsi_pw_status_download(pl2vc_master->pwinfo.pwindex, &pl2vc_master->pw_backup);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("IPC send pw status\n");
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 回切处理
 * @param[in ] : pthread - 线程结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年4月12日 10:40:14
 * @note       : 回切事件一定是备切换至主
 */
static int vsi_set_pw_status_callback(void *para)
{
    struct l2vc_entry *pl2vc = (struct l2vc_entry *)para;
    int ret;

    MPLS_LOG_DEBUG();

    if (NULL == pl2vc)
    {
        return ERRNO_FAIL;
    }

    /* 定时器超时置 NULL */
    pl2vc->pwtr_timer = 0;

    /* 状态发生变化，切换 */
    pl2vc->pw_backup = BACKUP_STATUS_MASTER;

    ret = vsi_pw_status_download(pl2vc->pwinfo.pwindex, &pl2vc->pw_backup);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("IPC send pw status\n");
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 处理 pw 主备切换事件
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年4月12日 10:39:39
 * @note       :
 */
int vsi_set_pw_status(struct l2vc_entry *pl2vc)
{
    struct l2vc_entry *pl2vc_master = NULL;
    struct l2vc_entry *pl2vc_backup = NULL;
    struct vsi_entry  *pvsi         = NULL;

    MPLS_LOG_DEBUG();

    if ((NULL == pl2vc) || (NULL == pl2vc->ppw_backup) || (0 == pl2vc->pwinfo.backup_index))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pvsi = vsi_lookup(pl2vc->pwinfo.vpls.vsi_id);
    if (NULL == pvsi)
    {
        return ERRNO_SUCCESS;
    }

    /* 确认主备 pw 角色 */
    if (PW_TYPE_MASTER == pl2vc->pwinfo.pw_type)
    {
        pl2vc_master = pl2vc;
        pl2vc_backup = pl2vc->ppw_backup;
    }
    else
    {
        pl2vc_master = pl2vc->ppw_backup;
        pl2vc_backup = pl2vc;
    }

    if (pvsi->failback == FAILBACK_ENABLE)
    {
        if ((pl2vc_master->pw_backup == BACKUP_STATUS_SLAVE)
            && (pl2vc_master->pwinfo.up_flag == LINK_UP)
            && (pl2vc_backup->pwinfo.up_flag == LINK_UP))
        {
            MPLS_TIMER_DEL(pl2vc_master->pwtr_timer);
            pl2vc_master->pwtr_timer = MPLS_TIMER_ADD(vsi_set_pw_status_callback, pl2vc_master, pvsi->wtr);

            return ERRNO_SUCCESS;
        }
    }
    else if (pvsi->failback == FAILBACK_DISABLE)
    {
        MPLS_TIMER_DEL(pl2vc_master->pwtr_timer);
        vsi_set_pw_status_non_failback(pl2vc_master);

        return ERRNO_SUCCESS;
    }

    MPLS_TIMER_DEL(pl2vc_master->pwtr_timer);
    vsi_set_pw_status_default(pl2vc_master);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 批量获取 vsi 结构
 * @param[in ] : vsi_id   - vsi id
 * @param[out] : vsi_buff - 获取到的 vsi 结构数组
 * @return     : 返回获取到的 vsi 数量
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:28:49
 * @note       : 提供给 snmp 的 get next 接口，空索引从数据结构开始返回数据，
 * @note       : 非空索引从传入索引节点下一个节点开始返回数据
 */
int vsi_get_bulk(uint32_t vsi_id, struct vsi_entry vsi_buff[])
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct vsi_entry   *pvsi    = NULL;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct vsi_entry);
    int cursor;

    if (0 == vsi_id)
    {
        /* 初次获取 */
        HASH_BUCKET_LOOP(pbucket, cursor, vsi_table)
        {
            pvsi = (struct vsi_entry *)pbucket->data;
            if (NULL == pvsi)
            {
                continue;
            }

            memcpy(&vsi_buff[data_num++], pvsi, sizeof(struct vsi_entry));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&vsi_table, (void *)vsi_id);

        if (NULL != pbucket)
        {
            for (data_num=0; data_num<msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&vsi_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                pvsi = pnext->data;
                memcpy(&vsi_buff[data_num], pvsi, sizeof(struct vsi_entry));
                pbucket = pnext;
            }
        }
    }

    return data_num;
}


/**
 * @brief      : 批量获取 vsi 中接口
 * @param[in ] : vsi_id  - vsi id
 * @param[in ] : ifindex - 接口索引
 * @param[out] : if_buff - 获取到的接口索引数组
 * @return     : 返回获取到的接口索引数量
 * @author     : ZhangFj
 * @date       : 2018年3月5日 10:31:31
 * @note       :
 */
int vsi_get_vsiif_bulk(uint32_t vsi_id, uint32_t ifindex, uint32_t if_buff[])
{
    struct listnode  *pnode = NULL;
    struct vsi_entry *pvsi  = NULL;
    uint32_t         *pdata = NULL;
    int msg_len  = IPC_MSG_LEN/sizeof(uint32_t);
    int data_num = 0;

    pvsi = vsi_lookup(vsi_id);
    if (NULL == pvsi)
    {
        return data_num;
    }

    if (0 == ifindex)
    {
        for (ALL_LIST_ELEMENTS_RO(pvsi->aclist, pnode, pdata))
        {
            if_buff[data_num++] = (uint32_t)pdata;

            if (data_num == msg_len)
            {
                return data_num;
            }
        }
    }
    else
    {
        if (NULL != pvsi->aclist)
        {
            pnode = listnode_lookup(pvsi->aclist, (void *)ifindex);
            if (NULL != pnode)
            {
                for (pnode=listnextnode(pnode); pnode!=NULL; pnode=listnextnode(pnode))
                {
                    if_buff[data_num++] = (uint32_t)listgetdata(pnode);

                    if (data_num == msg_len)
                    {
                        return data_num;
                    }
                }
            }
        }
    }

    return data_num;
}

int vsi_get_vsi_black_white_mac_bulk(struct mpls_mac *pindex, uint32_t data_len, struct mpls_mac*mac_buf)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct listnode  *pnode = NULL;
    struct vsi_entry *pvsi  = NULL;
    char             *pdata = NULL;
    int msg_len  = IPC_MSG_LEN/sizeof(struct mpls_mac);
    int data_num = 0;
    int cursor = 0;

    if (0 == data_len)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, vsi_table)
        {
            pvsi = (struct vsi_entry *)pbucket->data;
            if (NULL == pvsi)
            {
                continue;
            }

            for (ALL_LIST_ELEMENTS_RO(pvsi->blacklist, pnode, pdata))
            {
                memcpy(mac_buf[data_num].mac, pdata, MAC_LEN);
                mac_buf[data_num].type = 1;
                mac_buf[data_num].vplsid = pvsi->vpls.vsi_id;
                data_num ++;
                if (data_num == msg_len)
                {
                    MPLS_LOG_DEBUG("data_num %d\n", data_num);
                    return data_num;
                }
            }
            
            for (ALL_LIST_ELEMENTS_RO(pvsi->whitelist, pnode, pdata))
            {
                memcpy(mac_buf[data_num].mac, pdata, MAC_LEN);
                mac_buf[data_num].type = 2;
                mac_buf[data_num].vplsid = pvsi->vpls.vsi_id;
                data_num ++;
                if (data_num == msg_len)
                {
                    MPLS_LOG_DEBUG("data_num %d\n", data_num);
                    return data_num;
                }
            }
            MPLS_LOG_DEBUG("data_num %d\n", data_num);
            return data_num;/*每次只返回一组*/
        }
    }
    else
    {
        pbucket = hios_hash_find(&vsi_table, (void *)pindex->vplsid);

        if (NULL != pbucket)
        {
                pnext = hios_hash_next_cursor(&vsi_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    return data_num;
                }
                pvsi = pnext->data;

                if (NULL != pvsi->blacklist)
                {
                    for (ALL_LIST_ELEMENTS_RO(pvsi->blacklist, pnode, pdata))
                    {
                        if (NULL != pnode)
                        {
                            for ( ; pnode!=NULL; pnode=listnextnode(pnode))
                            {
                                memcpy(mac_buf[data_num].mac, listgetdata(pnode), MAC_LEN);
                                mac_buf[data_num].type = 1;
                                mac_buf[data_num].vplsid = pvsi->vpls.vsi_id;
                                data_num ++;
                                if (data_num == msg_len)
                                {
                                    MPLS_LOG_DEBUG("data_num %d\n", data_num);
                                    return data_num;
                                }
                            }
                        }
                    }
                }
            
                if (NULL != pvsi->whitelist)
                {
                    for (ALL_LIST_ELEMENTS_RO(pvsi->whitelist, pnode, pdata))
                    {
                        if (NULL != pnode)
                        {
                            for ( ; pnode!=NULL; pnode=listnextnode(pnode))
                            {
                                memcpy(mac_buf[data_num].mac, listgetdata(pnode), MAC_LEN);
                                mac_buf[data_num].type = 2;
                                mac_buf[data_num].vplsid = pvsi->vpls.vsi_id;
                                data_num ++;
                        
                                if (data_num == msg_len)
                                {
                                    MPLS_LOG_DEBUG("data_num %d\n", data_num);
                                    return data_num;
                                }
                            }
                        }
                    }
                }
            }
        }
    
    MPLS_LOG_DEBUG("data_num %d\n", data_num);
    return data_num;
}



int vsi_get_pw_l2vc_bulk(uint32_t vsi_id, struct l2vc_entry *pindex, uint32_t data_len, struct l2vc_entry *l2vc_buf)
{
    struct listnode  *pnode = NULL;
    struct vsi_entry *pvsi  = NULL;
    struct l2vc_entry *pl2vc = NULL;
    struct l2vc_entry *pl2vc_backup = NULL;
    int msg_len  = IPC_MSG_LEN/sizeof(struct l2vc_entry);
    
    int data_num = 0;

    pvsi = vsi_lookup(vsi_id);
    if (NULL == pvsi)
    {
        return data_num;
    }

    if(0 == data_len)
    {
        for (ALL_LIST_ELEMENTS_RO(pvsi->pwlist, pnode, pl2vc))
        {
            if (pl2vc == NULL)
            {
                continue;
            }

            if ((0 == pl2vc->pwinfo.backup_index)
                || (PW_TYPE_BACKUP == pl2vc->pwinfo.pw_type))
            {
                continue;
            }

            /* copy the master and backup pw to the buf */
            pl2vc_backup = pl2vc->ppw_backup;
            if((data_num + 2) <= msg_len)
            {
                memcpy(l2vc_buf, pl2vc, sizeof(struct l2vc_entry));
                data_num++;
                memcpy(l2vc_buf, pl2vc_backup, sizeof(struct l2vc_entry));
                data_num++;
            }
            else
            {
                return data_num;
            }
        }
    }
    else
    {
        for (ALL_LIST_ELEMENTS_RO(pvsi->pwlist, pnode, pl2vc))
        {
            if (pl2vc == NULL)
            {
                continue;
            }

            if ((0 == pl2vc->pwinfo.backup_index)
                || (PW_TYPE_BACKUP == pl2vc->pwinfo.pw_type))
            {
                continue;
            }

            /* copy the master and backup pw to the buf */
            pl2vc_backup = pl2vc->ppw_backup;

            if((pl2vc->pwinfo.vcid == pindex->pwinfo.vcid) || (pl2vc_backup->pwinfo.vcid == pindex->pwinfo.vcid))
            {
                continue;
            }
            
            if((data_num + 2) <= msg_len)
            {
                memcpy(l2vc_buf, pl2vc, sizeof(struct l2vc_entry));
                data_num++;
                memcpy(l2vc_buf, pl2vc_backup, sizeof(struct l2vc_entry));
                data_num++;
            }
            else
            {
                return data_num;
            }
        }
    }
    return data_num;
}

int vsi_get_smac_bulk(struct mpls_smac *pindex, uint32_t data_len, struct mpls_smac*mac_buf)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    int                 cursor = 0;
    struct vsi_entry   *pvsi     = NULL;
    struct listnode    *pnode1   = NULL;
    struct l2vc_entry  *pl2vc    = NULL;
    struct listnode    *pnode2   = NULL;
    uint8_t            *smac     = NULL;

    int msg_len  = IPC_MSG_LEN/sizeof(struct mpls_smac);
    int data_num = 0;

    struct listnode    *pnode3     = NULL;
    uint32_t           *pifindex  = NULL;
    struct mpls_if     *pif       = NULL;
    struct listnode    *pnode_mac = NULL;
    uchar              *pmac      = NULL;
    uchar               ifname[NAME_STRING_LEN];
    
    if (0 == data_len)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, vsi_table)
        {
            pvsi = (struct vsi_entry *)pbucket->data;
            if ((NULL == pvsi) || (NULL == pvsi->pwlist))
            {
                continue;
            }
            /* get the static mac from the pw list */
            for (ALL_LIST_ELEMENTS_RO(pvsi->pwlist, pnode1, pl2vc))
            {
                if ((NULL == pl2vc) || (NULL == pl2vc->mac_list))
                {
                    continue;
                }
            
                for (ALL_LIST_ELEMENTS_RO(pl2vc->mac_list, pnode2, smac))
                {
                    memcpy(mac_buf[data_num].mac, smac, MAC_LEN);
                    mac_buf[data_num].vplsid = pvsi->vpls.vsi_id;
                    mac_buf[data_num].pwid = pl2vc->pwinfo.pwindex;
                    
                    mac_buf[data_num].type = 2;
                    data_num ++;
                    if (data_num == msg_len)
                    {
                        return data_num;
                    }
                }
            }

            /* get the static mac from the ac list */
            for (ALL_LIST_ELEMENTS_RO(pvsi->aclist, pnode3, pifindex))
            {
                pif = mpls_if_get((uint32_t)pifindex);
                if (NULL == pif)
                {
                    continue;
                }
                for (ALL_LIST_ELEMENTS_RO(&pif->mac_list, pnode_mac, pmac))
                {
                    memcpy(mac_buf[data_num].mac, pmac, MAC_LEN);
                    mac_buf[data_num].vplsid = pvsi->vpls.vsi_id;

                    memset(ifname, 0, NAME_STRING_LEN);
                    ifm_get_name_by_ifindex((uint32_t)pifindex, (char *)ifname);
                    
                    memcpy(mac_buf[data_num].ifname, ifname, NAME_STRING_LEN);
                    
                    mac_buf[data_num].type = 1;
                    data_num ++;
                    if (data_num == msg_len)
                    {
                        return data_num;
                    }
                }
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&vsi_table, (void *)pindex->vplsid);

        if (NULL != pbucket)
        {
            int find_flag = 0;
            for (data_num=0; data_num < msg_len; data_num++)
            {
                pnext = hios_hash_next_cursor(&vsi_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }
                pvsi = pnext->data;
                
                for (ALL_LIST_ELEMENTS_RO(pvsi->pwlist, pnode1, pl2vc))
                {
                    if ((NULL == pl2vc) || (NULL == pl2vc->mac_list))
                    {
                        continue;
                    }
                
                    for (ALL_LIST_ELEMENTS_RO(pl2vc->mac_list, pnode2, smac))
                    {
                        if(memcmp(smac, pindex->mac, MAC_LEN))
                        {
                            find_flag = 1;
                        }

                        if(!find_flag)
                        {
                            continue;
                        }
                        memcpy(mac_buf[data_num].mac, smac, MAC_LEN);
                        mac_buf[data_num].vplsid = pvsi->vpls.vsi_id;
                        mac_buf[data_num].pwid = pl2vc->pwinfo.pwindex;
                        mac_buf[data_num].type = 2;
                        data_num ++;
                        if (data_num == msg_len)
                        {
                            return data_num;
                        }
                    }
                }

                /* get the static mac from the ac list */
                for (ALL_LIST_ELEMENTS_RO(pvsi->aclist, pnode3, pifindex))
                {
                    pif = mpls_if_get((uint32_t)pifindex);
                    if (NULL == pif)
                    {
                        continue;
                    }
                    for (ALL_LIST_ELEMENTS_RO(&pif->mac_list, pnode_mac, pmac))
                    {
                        if(memcmp(pmac, pindex->mac, MAC_LEN))
                        {
                            find_flag = 1;
                        }

                        if(!find_flag)
                        {
                            continue;
                        }
                        
                        memcpy(mac_buf[data_num].mac, pmac, MAC_LEN);
                        mac_buf[data_num].vplsid = pvsi->vpls.vsi_id;

                        memset(ifname, 0, NAME_STRING_LEN);
                        ifm_get_name_by_ifindex((uint32_t)pifindex, (char *)ifname);

                        
                        memcpy(mac_buf[data_num].ifname, ifname, NAME_STRING_LEN);
                        mac_buf[data_num].type = 1;
                        data_num ++;
                        if (data_num == msg_len)
                        {
                            return data_num;
                        }
                    }
                }

                
            }
            return  data_num;
        }
        pbucket = pnext;
    }
    return data_num;
}


