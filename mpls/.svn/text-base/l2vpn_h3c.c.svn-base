/**
 * @file      : l2vpn_h3c.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年4月27日 9:48:48
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/command.h>
#include <lib/linklist.h>
#include <lib/errcode.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include "mpls_main.h"
#include "mpls_if.h"
#include "l2vpn_h3c.h"
#include "pw.h"
#include "pw_cmd.h"


struct l2vpn_global g_l2vpn;
struct hash_table pw_class_table;
struct hash_table xc_group_table;
struct hash_table service_instance_table;

int l2vpn_pw_class_get_bulk(struct pw_class *pindex, int index_flag, struct pw_class *pbuff);
int l2vpn_service_instance_get_bulk(struct serv_ins_mib *pindex, int index_flag, struct serv_ins_mib *pbuff);
int l2vpn_xc_group_get_bulk(struct xc_group_mib *pindex, int index_flag, struct xc_group_mib *pbuff);

/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月27日 14:52:16
 * @note       :
 */
static unsigned int pw_class_compute(void *hash_key)
{
    return hios_hash_string(hash_key);
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月27日 14:56:32
 * @note       :
 */
static int pw_class_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;

    if ((pitem == NULL) || (hash_key == NULL))
    {
        return ERRNO_FAIL;
    }

    if ((strlen(pbucket->hash_key) == strlen(hash_key))
        && (0 == memcmp(pbucket->hash_key, hash_key, strlen(hash_key))))
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月27日 14:56:15
 * @note       :
 */
void pw_class_table_init(uint32_t size)
{
    hios_hash_init(&pw_class_table, size, pw_class_compute, pw_class_compare);

    return;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月27日 14:55:57
 * @note       :
 */
void pw_class_free(struct pw_class *pclass)
{
    if (NULL != pclass)
    {
        XFREE(MTYPE_PW_ENTRY, pclass);
    }

    return;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月27日 14:55:41
 * @note       :
 */
struct pw_class *pw_class_create(void)
{
    struct pw_class *pclass = NULL;

    MPLS_LOG_DEBUG();

    pclass = (struct pw_class *)XCALLOC(MTYPE_PW_ENTRY, sizeof(struct pw_class));
    if (pclass == NULL)
    {
        MPLS_LOG_ERROR("malloc\n");

        return NULL;
    }

    pclass->ctrlword = DISABLE;
    pclass->pw_type  = PW_TYPE_VLAN;

    return pclass;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月27日 14:54:51
 * @note       :
 */
int pw_class_add(struct pw_class *pclass)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (pclass == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (pw_class_table.hash_size == pw_class_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        MPLS_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;;
    }

    pitem->hash_key = pclass->name;
    pitem->data     = pclass;

    hios_hash_add(&pw_class_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月27日 14:54:29
 * @note       :
 */
int pw_class_delete(uchar *pname)
{
    struct hash_bucket *pitem  = NULL;
    struct pw_class    *pclass = NULL;

    MPLS_LOG_DEBUG();

    if (pname == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pitem = hios_hash_find(&pw_class_table, pname);
    if (pitem == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pclass = (struct pw_class *)pitem->data;
    if (pclass != NULL)
    {
        XFREE(MTYPE_PW_ENTRY, pclass);
    }

    hios_hash_delete(&pw_class_table, pitem);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月27日 14:54:35
 * @note       :
 */
struct pw_class *pw_class_lookup(uchar *pname)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (pname == NULL)
    {
        return NULL;
    }

    pitem = hios_hash_find(&pw_class_table, pname);
    if (pitem == NULL)
    {
        return NULL;
    }

    return (struct pw_class *)pitem->data;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 10:05:31
 * @note       :
 */
struct cont_entry *connection_create(void)
{
    struct cont_entry *pcont = NULL;

    MPLS_LOG_DEBUG();

    pcont = (struct cont_entry *)XCALLOC(MTYPE_PW_ENTRY, sizeof(struct cont_entry));
    if (NULL == pcont)
    {
        MPLS_LOG_ERROR("malloc failed\n");

        return NULL;
    }

    return pcont;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 10:08:25
 * @note       :
 */
void connection_free(struct cont_entry *pcont)
{
    if (NULL != pcont)
    {
        XFREE(MTYPE_PW_ENTRY, pcont);
    }
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 10:03:35
 * @note       :
 */
int connection_add(struct list *plist, struct cont_entry *pcont)
{
    MPLS_LOG_DEBUG();

    if ((NULL == plist) || (NULL == pcont))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    listnode_add(plist, pcont);

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 10:03:41
 * @note       :
 */
int connection_delete(struct list *plist, struct cont_entry *pcont)
{
    MPLS_LOG_DEBUG();

    if ((NULL == plist) || (NULL == pcont))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    listnode_delete(plist, pcont);
    connection_free(pcont);

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月7日 9:48:16
 * @note       :
 */
int connection_delete_uni(struct cont_entry *pcont)
{
    struct serv_ins *psi = NULL;

    if (NULL == pcont)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    psi = service_instance_lookup(pcont->uni.instance.ifindex);
    if (NULL != psi)
    {
        psi->pcont = NULL;
    }

    l2vpn_delete_backup_pw(pcont);
    l2vpn_delete_master_pw(pcont);

    UNSET_FLAG(pcont->flag, UNI_FLAG);

    memset(&pcont->uni, 0, sizeof(struct uni_info));

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月4日 15:07:48
 * @note       :
 */
int connection_delete_nni_master(struct cont_entry *pcont)
{
    if (NULL == pcont)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    l2vpn_delete_master_pw(pcont);

    UNSET_FLAG(pcont->flag, NNI_MASTER_FLAG);

    memset(&pcont->nni_master, 0, sizeof(struct nni_info));
    memset(pcont->master_name, 0, NAME_STRING_LEN);

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月4日 14:46:14
 * @note       :
 */
int connection_delete_nni_backup(struct cont_entry *pcont)
{
    if (NULL == pcont)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    l2vpn_delete_backup_pw(pcont);

    UNSET_FLAG(pcont->flag, NNI_BACKUP_FLAG);

    memset(&pcont->nni_backup, 0, sizeof(struct nni_info));
    memset(pcont->backup_name, 0, NAME_STRING_LEN);

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 9:56:56
 * @note       :
 */
struct cont_entry *connection_lookup(struct list *plist, uchar *pname)
{
    struct cont_entry *pcont = NULL;
    struct listnode   *pnode = NULL;

    if ((NULL == plist) || (NULL == pname))
    {
        return NULL;
    }

    for (ALL_LIST_ELEMENTS_RO(plist, pnode, pcont))
    {
        if (NULL == pcont)
        {
            continue;
        }

        if ((strlen((char *)pname) == strlen((char *)pcont->name))
            && (0 == memcmp(pname, pcont->name, strlen((char *)pname))))
        {
            return pcont;
        }
    }

    return pcont;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 9:43:55
 * @note       :
 */
static unsigned int xc_group_compute(void *hash_key)
{
    return hios_hash_string(hash_key);
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 9:44:12
 * @note       :
 */
static int xc_group_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;

    if ((pitem == NULL) || (hash_key == NULL))
    {
        return ERRNO_FAIL;
    }

    if ((strlen(pbucket->hash_key) == strlen(hash_key))
        && (0 == memcmp(pbucket->hash_key, hash_key, strlen(hash_key))))
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 9:44:32
 * @note       :
 */
void xc_group_table_init(uint32_t size)
{
    hios_hash_init(&xc_group_table, size, xc_group_compute, xc_group_compare);

    return;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 9:44:57
 * @note       :
 */
void xc_group_free(struct xc_group *pgroup)
{
    if (NULL != pgroup)
    {
        XFREE(MTYPE_PW_ENTRY, pgroup);
    }

    return;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 9:45:03
 * @note       :
 */
struct xc_group *xc_group_create(void)
{
    struct xc_group *pgroup = NULL;

    MPLS_LOG_DEBUG();

    pgroup = (struct xc_group *)XCALLOC(MTYPE_PW_ENTRY, sizeof(struct xc_group));
    if (pgroup == NULL)
    {
        MPLS_LOG_ERROR("malloc\n");

        return NULL;
    }

    return pgroup;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 9:45:16
 * @note       :
 */
int xc_group_add(struct xc_group *pgroup)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (pgroup == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (xc_group_table.hash_size == xc_group_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        MPLS_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;;
    }

    pitem->hash_key = pgroup->name;
    pitem->data     = pgroup;

    hios_hash_add(&xc_group_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 9:45:26
 * @note       :
 */
int xc_group_delete(uchar *pname)
{
    struct hash_bucket *pitem  = NULL;
    struct xc_group    *pgroup = NULL;

    MPLS_LOG_DEBUG();

    if (pname == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pitem = hios_hash_find(&xc_group_table, pname);
    if (pitem == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pgroup = (struct xc_group *)pitem->data;
    if (pgroup != NULL)
    {
        XFREE(MTYPE_PW_ENTRY, pgroup);
    }

    hios_hash_delete(&xc_group_table, pitem);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月7日 10:12:23
 * @note       :
 */
int xc_group_delete_connection(struct list *plist)
{
    struct listnode   *pnode = NULL;
    struct listnode   *pnext = NULL;
    struct cont_entry *pcont = NULL;

    if (NULL == plist)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    for (ALL_LIST_ELEMENTS(plist, pnode, pnext, pcont))
    {
        connection_delete_uni(pcont);
        connection_delete_nni_backup(pcont);
        connection_delete_nni_master(pcont);
        connection_delete(plist, pcont);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月28日 9:45:35
 * @note       :
 */
struct xc_group *xc_group_lookup(uchar *pname)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (pname == NULL)
    {
        return NULL;
    }

    pitem = hios_hash_find(&xc_group_table, pname);
    if (pitem == NULL)
    {
        return NULL;
    }

    return (struct xc_group *)pitem->data;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月4日 10:50:23
 * @note       :
 */
int pwid_check(uint32_t pwid)
{
    struct hash_bucket *pbucket = NULL;
    struct xc_group    *pgroup  = NULL;
    struct list        *plist   = NULL;
    struct listnode    *pnode   = NULL;
    struct nni_info    *pnni    = NULL;
    int cursor = 0;

    HASH_BUCKET_LOOP(pbucket, cursor, xc_group_table)
    {
        pgroup = (struct xc_group *)pbucket->data;
        if (NULL == pgroup)
        {
            continue;
        }

        plist = &pgroup->cont_list;

        for (ALL_LIST_ELEMENTS_RO(plist, pnode, pnni))
        {
            if (pwid == pnni->pwid)
            {
                return ERRNO_FAIL;
            }
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月3日 11:45:07
 * @note       :
 */
static uint32_t service_instance_compute(void *hash_key)
{
    if (NULL == hash_key)
    {
        return 0;
    }

    return ((uint32_t)hash_key);
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月3日 11:45:14
 * @note       :
 */
static int service_instance_compare(void *pitem, void *hash_key)
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
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月3日 11:45:21
 * @note       :
 */
void service_instance_table_init(uint32_t size)
{
    hios_hash_init(&service_instance_table, size, service_instance_compute, service_instance_compare);

    return;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月3日 11:45:29
 * @note       :
 */
void service_instance_free(struct serv_ins *psi)
{
    if (NULL != psi)
    {
        XFREE(MTYPE_PW_ENTRY, psi);
    }

    return;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月3日 11:45:38
 * @note       :
 */
struct serv_ins *service_instance_create(void)
{
    struct serv_ins *psi = NULL;

    MPLS_LOG_DEBUG();

    psi = (struct serv_ins *)XCALLOC(MTYPE_PW_ENTRY, sizeof(struct serv_ins));
    if (psi == NULL)
    {
        MPLS_LOG_ERROR("malloc\n");

        return NULL;
    }

    return psi;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月3日 11:45:44
 * @note       :
 */
int service_instance_add(struct serv_ins *psi)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (psi == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (service_instance_table.hash_size == service_instance_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        MPLS_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;;
    }

    pitem->hash_key = (void *)psi->ifindex;
    pitem->data     = psi;

    hios_hash_add(&service_instance_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月3日 11:45:50
 * @note       :
 */
int service_instance_delete(uint32_t ifindex)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (0 == ifindex)
    {
        return ERRNO_SUCCESS;
    }

    pitem = hios_hash_find(&service_instance_table, (void *)ifindex);
    if (pitem == NULL)
    {
        return ERRNO_SUCCESS;
    }

    if (NULL != pitem->data)
    {
        XFREE(MTYPE_PW_ENTRY, pitem->data);
    }

    hios_hash_delete(&service_instance_table, pitem);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月3日 11:45:55
 * @note       :
 */
struct serv_ins *service_instance_lookup(uint32_t ifindex)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (0 == ifindex)
    {
        return NULL;
    }

    pitem = hios_hash_find(&service_instance_table, (void *)ifindex);
    if (pitem == NULL)
    {
        return NULL;
    }

    return (struct serv_ins *)pitem->data;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月15日 14:23:20
 * @note       :
 */
uint32_t l2vpn_get_service_instance_ifindex(uint32_t ifindex, uint16_t instance)
{
    struct ifm_usp usp;
    int ret = ERRNO_SUCCESS;

    ret = ifm_get_usp_by_ifindex(ifindex, &usp);
    if (ERRNO_SUCCESS != ret)
    {
        return ERRNO_FAIL;
    }

    usp.sub_port = instance;

    return ifm_get_ifindex_by_usp(&usp);
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月15日 15:52:51
 * @note       : 同一物理接口下，不同服务实例封装不能相同
 * @note       : 且只能有一个 default，一个 untagged，一个 tagged
 * @note       : 且 tagged 不能与 s-vid 同时配置
 */
int l2vpn_check_service_instance_encap(uint32_t ifindex, enum encap_type type, uint16_t vid)
{
    struct hash_bucket *pbucket = NULL;
    struct serv_ins    *psi     = NULL;
    int cursor = 0;

    if (0 == ifindex)
    {
        return ERRNO_SUCCESS;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, service_instance_table)
    {
        psi = (struct serv_ins *)pbucket->data;
        if (NULL == psi)
        {
            continue;
        }

        if (IFM_PARENT_IFINDEX_GET(psi->ifindex) != IFM_PARENT_IFINDEX_GET(ifindex))
        {
            continue;
        }

        if ((type == ENCAP_TYPE_UNTAGGED)
            || (type == ENCAP_TYPE_TAGGED)
            || (type == ENCAP_TYPE_DEFAULT))
        {
            if ((type == ENCAP_TYPE_TAGGED) && (psi->en_type == ENCAP_TYPE_VID))
            {
                return ERRNO_FAIL;
            }

            if (type == psi->en_type)
            {
                return ERRNO_FAIL;
            }
        }
        else if (type == ENCAP_TYPE_VID)
        {
            if (psi->en_type == ENCAP_TYPE_TAGGED)
            {
                return ERRNO_FAIL;
            }

            if ((psi->en_type == ENCAP_TYPE_VID)
                && (psi->vid == vid))
            {
                return ERRNO_FAIL;
            }
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月8日 10:54:06
 * @note       :
 */
int l2vpn_add_master_pw(struct cont_entry *pcont)
{
    struct l2vc_entry *pl2vc    = NULL;
    struct mpls_if    *pmpls_if = NULL;
    int ret = ERRNO_SUCCESS;

    MPLS_LOG_DEBUG();

    if (NULL == pcont)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (!FLAG_ISSET(pcont->flag, UNI_FLAG)
        || !FLAG_ISSET(pcont->flag, NNI_MASTER_FLAG))
    {
        return ERRNO_SUCCESS;
    }

    pl2vc = l2vc_lookup((uchar *)pcont->master_name);
    if (NULL != pl2vc)
    {
        return ERRNO_SUCCESS;
    }

    pl2vc = l2vc_create();
    if (NULL == pl2vc)
    {
        return ERRNO_MALLOC;
    }

    memcpy(pl2vc->name, pcont->master_name, strlen(pcont->master_name));
    memcpy(pl2vc->pwinfo.name, pl2vc->name, strlen((char *)pl2vc->name));

    pl2vc->ldp_tunnel           = ENABLE;
    pl2vc->lsp_tunnel           = ENABLE;
    pl2vc->pwinfo.pw_type       = PW_TYPE_MASTER;
    pl2vc->pwinfo.ifindex       = pcont->uni.instance.ifindex;
    pl2vc->peerip.addr.ipv4     = pcont->nni_master.peer;
    pl2vc->peerip.type          = INET_FAMILY_IPV4;
    pl2vc->pwinfo.vcid          = pcont->nni_master.pwid;
    pl2vc->pwinfo.pwindex       = pcont->nni_master.pwid;
    pl2vc->inlabel              = pcont->nni_master.inlabel;
    pl2vc->outlabel             = pcont->nni_master.outlabel;
    pl2vc->pwinfo.ctrlword_flag = pcont->nni_master.pw_class.ctrlword;
    pl2vc->pwinfo.tpid          = 0x8100;
    pl2vc->pwinfo.vlan          = 4095;
    pl2vc->failback             = pcont->failback;
    pl2vc->wtr                  = pcont->wtr;
	pl2vc->bfd_id				= pcont->nni_master.bfd_id;

    if (pcont->uni.mode == ACCESS_MODE_ETH)
    {
        if (pcont->nni_master.pw_class.pw_type == PW_TYPE_ETH)
        {
            pl2vc->pwinfo.tag_flag = PW_ENCAP_ETHERNET_RAW;
        }
        else if (pcont->nni_master.pw_class.pw_type == PW_TYPE_VLAN)
        {
            pl2vc->pwinfo.tag_flag = PW_ENCAP_ETHERNET_TAG;
        }
    }
    else if (pcont->uni.mode == ACCESS_MODE_VLAN)
    {
        if (pcont->nni_master.pw_class.pw_type == PW_TYPE_ETH)
        {
            pl2vc->pwinfo.tag_flag = PW_ENCAP_VLAN_RAW;
        }
        else if (pcont->nni_master.pw_class.pw_type == PW_TYPE_VLAN)
        {
            pl2vc->pwinfo.tag_flag = PW_ENCAP_VLAN_TAG;
        }
    }

    ret = l2vc_add(pl2vc);
    if (ERRNO_SUCCESS != ret)
    {
        l2vc_free(pl2vc);

        return ret;
    }

    pw_add(&pl2vc->pwinfo);

    pmpls_if = mpls_if_get(pl2vc->pwinfo.ifindex);
    if (NULL == pmpls_if)
    {
        return ERRNO_MALLOC;
    }

    l2vc_select_tunnel(pl2vc);

    ret = pw_get_lsp_index(&pl2vc->pwinfo);
    if (ERRNO_SUCCESS != ret)
    {
        goto fail;
    }

    ret = mpls_if_bind_pw(pmpls_if, pl2vc);
    if (ERRNO_SUCCESS != ret)
    {
        goto fail;
    }

    ret = mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        goto fail;
    }

    ret = mpls_pw_mplsif_download(pmpls_if->ifindex, &pl2vc->pwinfo.pwindex, MPLSIF_INFO_MASTER_PW, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        goto fail;
    }

    if (LINK_UP == pl2vc->pwinfo.admin_up)
    {
        ret = pw_add_lsp(pl2vc);
        if (ERRNO_SUCCESS != ret)
        {
            goto fail;
        }

        pw_set_pw_status(pmpls_if);
    }
    else
    {
        l2vc_alarm_process(OPCODE_DOWN, pl2vc->pwinfo.pwindex);
    }

    return ERRNO_SUCCESS;

fail:

    mpls_if_unbind_pw(pmpls_if, pl2vc);
    pw_free_lsp_index(&pl2vc->pwinfo);
    l2vc_delete(pl2vc->name);

    return ret;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月8日 14:43:59
 * @note       :
 */
int l2vpn_delete_master_pw(struct cont_entry *pcont)
{
    struct l2vc_entry *pl2vc = NULL;
    struct mpls_if    *pif   = NULL;

    if (NULL == pcont)
    {
        return ERRNO_SUCCESS;
    }

    MPLS_LOG_DEBUG();

    pif = mpls_if_lookup(pcont->uni.instance.ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    pl2vc = pif->ppw_master;
    if (NULL == pl2vc)
    {
        return CMD_SUCCESS;
    }

    if (pif->ppw_slave != NULL)
    {
        l2vpn_delete_backup_pw(pcont);
    }

    mpls_if_unbind_pw(pif, pl2vc);

    pw_set_pw_status(pif);

    mpls_pw_mplsif_download(pif->ifindex, &pl2vc->pwinfo.pwindex, MPLSIF_INFO_MASTER_PW, OPCODE_DELETE);
    mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_DELETE);

    pw_delete_lsp(&pl2vc->pwinfo);
    pw_free_lsp_index(&pl2vc->pwinfo);

    l2vc_delete(pl2vc->name);

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月8日 14:10:18
 * @note       :
 */
int l2vpn_add_backup_pw(struct cont_entry *pcont)
{
    struct l2vc_entry *pl2vc    = NULL;
    struct mpls_if    *pmpls_if = NULL;
    int ret = ERRNO_SUCCESS;

    MPLS_LOG_DEBUG();

    if (NULL == pcont)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (!FLAG_ISSET(pcont->flag, UNI_FLAG)
        || !FLAG_ISSET(pcont->flag, NNI_BACKUP_FLAG))
    {
        return ERRNO_SUCCESS;
    }

    pl2vc = l2vc_lookup((uchar *)pcont->backup_name);
    if (NULL != pl2vc)
    {
        return ERRNO_SUCCESS;
    }

    pl2vc = l2vc_create();
    if (NULL == pl2vc)
    {
        return ERRNO_MALLOC;
    }

    memcpy(pl2vc->name, pcont->backup_name, strlen(pcont->backup_name));
    memcpy(pl2vc->pwinfo.name, pl2vc->name, strlen((char *)pl2vc->name));

    pl2vc->ldp_tunnel           = ENABLE;
    pl2vc->lsp_tunnel           = ENABLE;
    pl2vc->pwinfo.pw_type       = PW_TYPE_BACKUP;
    pl2vc->pwinfo.ifindex       = pcont->uni.instance.ifindex;
    pl2vc->peerip.addr.ipv4     = pcont->nni_backup.peer;
    pl2vc->peerip.type          = INET_FAMILY_IPV4;
    pl2vc->pwinfo.vcid          = pcont->nni_backup.pwid;
    pl2vc->pwinfo.pwindex       = pcont->nni_backup.pwid;
    pl2vc->inlabel              = pcont->nni_backup.inlabel;
    pl2vc->outlabel             = pcont->nni_backup.outlabel;
    pl2vc->pwinfo.ctrlword_flag = pcont->nni_backup.pw_class.ctrlword;
    pl2vc->pwinfo.tpid          = 0x8100;
    pl2vc->pwinfo.vlan          = 4095;
	pl2vc->bfd_id				= pcont->nni_backup.bfd_id;

    if (pcont->uni.mode == ACCESS_MODE_ETH)
    {
        if (pcont->nni_backup.pw_class.pw_type == PW_TYPE_ETH)
        {
            pl2vc->pwinfo.tag_flag = PW_ENCAP_ETHERNET_RAW;
        }
        else if (pcont->nni_backup.pw_class.pw_type == PW_TYPE_VLAN)
        {
            pl2vc->pwinfo.tag_flag = PW_ENCAP_ETHERNET_TAG;
        }
    }
    else if (pcont->uni.mode == ACCESS_MODE_VLAN)
    {
        if (pcont->nni_backup.pw_class.pw_type == PW_TYPE_ETH)
        {
            pl2vc->pwinfo.tag_flag = PW_ENCAP_VLAN_RAW;
        }
        else if (pcont->nni_backup.pw_class.pw_type == PW_TYPE_VLAN)
        {
            pl2vc->pwinfo.tag_flag = PW_ENCAP_VLAN_TAG;
        }
    }

    ret = l2vc_add(pl2vc);
    if (ERRNO_SUCCESS != ret)
    {
        l2vc_free(pl2vc);

        return CMD_WARNING;
    }

    pw_add(&pl2vc->pwinfo);

    pmpls_if = mpls_if_get(pl2vc->pwinfo.ifindex);
    if (NULL == pmpls_if)
    {
        return ERRNO_MALLOC;
    }

    l2vc_select_tunnel(pl2vc);

    ret = pw_get_lsp_index(&pl2vc->pwinfo);
    if (ERRNO_SUCCESS != ret)
    {
        goto fail;
    }

    ret = mpls_if_bind_pw(pmpls_if, pl2vc);
    if (ERRNO_SUCCESS != ret)
    {
        goto fail;
    }

    pl2vc->pwinfo.backup_index       = pmpls_if->ppw_master->pwinfo.pwindex;
    pmpls_if->ppw_master->ppw_backup = pl2vc;

    ret = mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        goto fail;
    }

    ret = mpls_pw_mplsif_download(pmpls_if->ifindex, &pl2vc->pwinfo.pwindex, MPLSIF_INFO_SLAVE_PW, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        goto fail;
    }

    if (LINK_UP == pl2vc->pwinfo.admin_up)
    {
        ret = pw_add_lsp(pl2vc);
        if (ERRNO_SUCCESS != ret)
        {
            goto fail;
        }

        pw_set_pw_status(pmpls_if);
    }
    else
    {
        l2vc_alarm_process(OPCODE_DOWN, pl2vc->pwinfo.pwindex);
    }

    return ERRNO_SUCCESS;

fail:

    mpls_if_unbind_pw(pmpls_if, pl2vc);
    pw_free_lsp_index(&pl2vc->pwinfo);

    l2vc_delete(pl2vc->name);

    return ret;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月8日 14:53:18
 * @note       :
 */
int l2vpn_delete_backup_pw(struct cont_entry *pcont)
{
    struct l2vc_entry *pl2vc = NULL;
    struct mpls_if    *pif   = NULL;

    if (NULL == pcont)
    {
        return ERRNO_SUCCESS;
    }

    MPLS_LOG_DEBUG();

    pif = mpls_if_lookup(pcont->uni.instance.ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    pl2vc = pif->ppw_slave;
    if (NULL == pl2vc)
    {
        return CMD_SUCCESS;
    }

    if (NULL != pif->ppw_master)
    {
        pif->ppw_master->ppw_backup = NULL;
        pl2vc->pwinfo.backup_index  = 0;
    }

    mpls_if_unbind_pw(pif, pl2vc);

    pw_set_pw_status(pif);

    mpls_pw_mplsif_download(pif->ifindex, &pl2vc->pwinfo.pwindex, MPLSIF_INFO_MASTER_PW, OPCODE_DELETE);
    mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_DELETE);

    pw_delete_lsp(&pl2vc->pwinfo);
    pw_free_lsp_index(&pl2vc->pwinfo);

    l2vc_delete(pl2vc->name);

    return ERRNO_SUCCESS;
}

/* add for h3c mpls mib */

    /* pw class */
int l2vpn_pw_class_get_bulk(struct pw_class *pindex, int index_flag, struct pw_class *pbuff)
{
    struct pw_class    *pclass  = NULL;
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_next   = NULL;
    int cursor = 0;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN / sizeof(struct pw_class);

    if(0 == index_flag)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, pw_class_table)
        {
            pclass = (struct pw_class *)pbucket->data;
            if (NULL == pclass)
            {
                continue;
            }
            memcpy(&pbuff[data_num], pclass, sizeof(struct pw_class));
            data_num++;
        }
    }
    else
    {
        pbucket =  hios_hash_find(&pw_class_table, pindex->name);
        if (pbucket)
        {
            for (data_num = 0; data_num < msg_num;)
            {
                pbucket_next = hios_hash_next_cursor(&pw_class_table, pbucket);
                if ((NULL == pbucket_next) || (NULL == pbucket_next->data))
                {
                    break;
                }
                pclass = (struct pw_class *)pbucket_next->data;
                pbucket = pbucket_next;
                if (pclass == NULL)
                {
                    continue;
                }
                memcpy(&pbuff[data_num], pclass, sizeof(struct pw_class));
                data_num++;
            }
        }
        else
        {

        }
    }

    return data_num;
}
    /* service instance ID */
int l2vpn_service_instance_get_bulk(struct serv_ins_mib *pindex, int index_flag, struct serv_ins_mib *pbuff)
{
    struct serv_ins    *pservice_instance  = NULL;
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_next   = NULL;
    int cursor = 0;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN / sizeof(struct serv_ins_mib);

    if(0 == index_flag)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, service_instance_table)
        {
            pservice_instance = (struct serv_ins *)pbucket->data;
            if (NULL == pservice_instance)
            {
                continue;
            }

            memcpy(&pbuff[data_num], pservice_instance, sizeof(struct serv_ins));
            memcpy(&(pbuff[data_num].connection), pservice_instance->pcont, sizeof(struct cont_entry));
            data_num++;
        }
    }
    else
    {
        pbucket =  hios_hash_find(&service_instance_table, (void *)pindex->ifindex);
        if (pbucket)
        {
            for (data_num = 0; data_num < msg_num;)
            {
                pbucket_next = hios_hash_next_cursor(&service_instance_table, pbucket);
                if ((NULL == pbucket_next) || (NULL == pbucket_next->data))
                {
                    break;
                }
                pservice_instance = (struct serv_ins *)pbucket_next->data;
                pbucket = pbucket_next;
                if (pservice_instance == NULL)
                {
                    continue;
                }
                memcpy(&pbuff[data_num], pservice_instance, sizeof(struct serv_ins));
                memcpy(&(pbuff[data_num].connection), pservice_instance->pcont, sizeof(struct cont_entry));
                data_num++;
            }
        }
        else
        {

        }
    }

    return data_num;
}

    /* xc_group */
int l2vpn_xc_group_get_bulk(struct xc_group_mib *pindex, int index_flag, struct xc_group_mib *pbuff)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct xc_group    *pxc_group  = NULL;
    struct listnode    *pnode_list     = NULL;
    struct listnode    *pnextnode_list = NULL;
    struct cont_entry  *pconnection = NULL;
    struct cont_entry  *pconnection_tmp = NULL;
    struct xc_group_mib *pbuff_tmp  = NULL;
    int cursor          = 0;
    int dat_len         = 0;
    int data_num        = 0;
    int xcgroup_len     = 0;
    int connection_num  = 0;

    if(NULL == pbuff)
    {
        goto out;
    }
    pbuff_tmp = pbuff;
    if(0 == index_flag)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, xc_group_table)
        {
            pxc_group = (struct xc_group *)pbucket->data;
            if (NULL == pxc_group)
            {
                continue;
            }
            xcgroup_len = pxc_group->cont_list.count * sizeof(struct cont_entry) + sizeof(struct xc_group_mib);
            /* all the connect in the xc_group will be in the same ipc msg buf */
            if((xcgroup_len + dat_len) > IPC_MSG_LEN)
            {
                goto out;
            }
            connection_num = 0;
            dat_len += xcgroup_len;
            memcpy(pbuff_tmp->name, pxc_group->name, NAME_STRING_LEN);

            /* copy the connections form xcgroup to xc_group_mib buffer */
            pconnection_tmp = pbuff_tmp->connection;
            for (ALL_LIST_ELEMENTS(&pxc_group->cont_list, pnode_list, pnextnode_list, pconnection))
            {
                if(pconnection)
                {
                    memcpy(&pconnection_tmp[connection_num], pconnection, sizeof(struct cont_entry));
                    connection_num ++;
                }
            }
            pbuff_tmp->connection_num = connection_num;
            pbuff_tmp = (struct xc_group_mib *)((char *)pbuff_tmp + xcgroup_len);
            data_num++;
        }
    }
    else
    {
        pbucket =  hios_hash_find(&xc_group_table, pindex->name);
        if (pbucket)
        {
loop:
            pnext = hios_hash_next_cursor(&service_instance_table, pbucket);
            if ((NULL == pnext) || (NULL == pnext->data))
            {
                goto out;
            }
            pxc_group = (struct xc_group *)pnext->data;
            pbucket = pnext;
            if (pxc_group == NULL)
            {
                goto out;
            }
            xcgroup_len = pxc_group->cont_list.count * sizeof(struct cont_entry) + sizeof(struct xc_group_mib);
            /* all the connect in the xc_group will be in the same ipc msg buf */
            if((xcgroup_len + dat_len) > IPC_MSG_LEN)
            {
                goto out;
            }
            connection_num = 0;
            dat_len += xcgroup_len;
            memcpy(pbuff_tmp->name, pxc_group->name, NAME_STRING_LEN);

            pconnection_tmp = pbuff_tmp->connection;
            for (ALL_LIST_ELEMENTS(&pxc_group->cont_list, pnode_list, pnextnode_list, pconnection))
            {
                if(pconnection)
                {
                    memcpy(&pconnection_tmp[connection_num], pconnection, sizeof(struct cont_entry));
                    connection_num ++;
                }
            }
            pbuff_tmp->connection_num = connection_num;
            pbuff_tmp = (struct xc_group_mib *)((char *)pbuff_tmp + xcgroup_len);

            data_num++;
            goto loop;
        }
        else
        {

        }

    }

out:
    return data_num;
}

