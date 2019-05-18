/**
 * @file      : tunnel.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月7日 10:10:23
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/zassert.h>
#include <lib/log.h>
#include <lib/alarm.h>
#include "mpls.h"
#include "mpls_main.h"
#include "tunnel.h"
#include "lspm.h"
#include "pw.h"
#include "l3vpn.h"
#include "tunnel_cmd.h"
#include "bfd/bfd.h"
#include "bfd/bfd_session.h"
#include "mpls_oam/mpls_oam.h"
#include "mpls_aps/mpls_aps.h"
#include "mpls_aps/mpls_aps_fsm.h"
#include "mpls_aps/mpls_aps.h"


struct hash_table tunnel_table;


/**
 * @brief      : 计算 tunnel 接口哈希表 key 值
 * @param[in ] : hash_key - 接口索引
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:26:24
 * @note       :
 */
static unsigned int tunnel_compute_hash(void *hash_key)
{
    if (NULL == hash_key)
    {
        return ERRNO_FAIL;
    }

    return (unsigned int)hash_key;
}


/**
 * @brief      : 比较 tunnel 接口哈希表 key 值
 * @param[in ] : item     - 哈希桶
 * @param[in ] : hash_key - 接口索引
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:27:08
 * @note       :
 */
static int tunnel_compare(void *item, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)item;

    if ((NULL == item) || (NULL == hash_key))
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
 * @brief      : tunnel 接口哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:28:03
 * @note       :
 */
void tunnel_if_table_init(int size)
{
    hios_hash_init(&tunnel_table, size, tunnel_compute_hash, tunnel_compare);

    return;
}


/**
 * @brief      : tunnel 接口节点创建
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回分配内存后的 tunnel 接口结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:28:49
 * @note       :
 */
struct tunnel_if *tunnel_if_create(uint32_t ifindex)
{
    struct tunnel_if *pif_new = NULL;

    MPLS_LOG_DEBUG();

    if (0 == ifindex)
    {
        return NULL;
    }

    pif_new = (struct tunnel_if *)XCALLOC(MTYPE_IF, sizeof(struct tunnel_if));
    if (NULL == pif_new)
    {
        return NULL;
    }

    pif_new->tunnel.ifindex   = ifindex;
    pif_new->tunnel.down_flag = LINK_DOWN;
    pif_new->tunnel.failback  = FAILBACK_ENABLE;
    pif_new->tunnel.wtr       = 30;

    return pif_new;
}


/**
 * @brief      : tunnel 接口添加
 * @param[in ] : pif - tunnel 接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:30:00
 * @note       :
 */
int tunnel_if_add(struct tunnel_if *pif)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (tunnel_table.hash_size == tunnel_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)pif->tunnel.ifindex;
    pitem->data     = pif;

    hios_hash_add(&tunnel_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tunnel 接口删除
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:30:34
 * @note       :
 */
int tunnel_if_delete(uint32_t ifindex)
{
    struct hash_bucket *pitem    = NULL;
    struct tunnel_if   *pif      = NULL;
    struct lsp_entry   *plsp_tmp = NULL;
    int ret = 0;

    MPLS_LOG_DEBUG();

    pitem = hios_hash_find(&tunnel_table, (void *)ifindex);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    pif = (struct tunnel_if *)pitem->data;
    if (NULL == pif)
    {
        return ERRNO_SUCCESS;
    }

    pif->tunnel.down_flag = LINK_DOWN;

    tunnel_if_down(pif);

    /* 删除 hqos 、car 、phb、domain */
    if ((0 != pif->tunnel.phb_id) && (ENABLE == pif->tunnel.phb_enable))
    {
        qos_phb_ref_operate(pif->tunnel.phb_id, QOS_TYPE_EXP, MODULE_ID_MPLS, REF_OPER_DECREASE);
    }

    if (0 != pif->tunnel.domain_id)
    {
        qos_domain_ref_operate(pif->tunnel.domain_id, QOS_TYPE_EXP, MODULE_ID_MPLS, REF_OPER_DECREASE);
    }

    if (0 != pif->tunnel.hqos_id)
    {
        qos_hqos_delete(pif->tunnel.hqos_id, MODULE_ID_MPLS);
    }

    if (0 != pif->tunnel.bfd_id)
    {
        bfd_unbind_for_lsp_pw(BFD_TYPE_TUNNEL, pif->tunnel.bfd_id, pif->tunnel.ifindex);
    }

    if (0 != pif->tunnel.aps_id)
    {
        mplsaps_unbind_lsp_pw(pif->tunnel.aps_id);
    }

    if (NULL != pif->p_mplstp)
    {
        if (NULL != pif->p_mplstp->ingress_lsp)
        {
            /* 解绑 bfd */
            if (0 != pif->p_mplstp->ingress_lsp->bfd_id)
            {
                bfd_unbind_for_lsp_pw(BFD_TYPE_LSP, pif->p_mplstp->ingress_lsp->bfd_id,
                                    pif->p_mplstp->ingress_lsp->lsp_index);

                pif->p_mplstp->ingress_lsp->bfd_id = 0;
            }

            /* 解绑 oam */
            if (0 != pif->p_mplstp->ingress_lsp->mplsoam_id)
            {
                tunnel_if_unbind_mplstp_oam(pif, TUNNEL_STATUS_MASTER);
            }

            pif->p_mplstp->ingress_lsp->group_index = 0;

            plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->ingress_lsp->lsp_index);
            if (NULL != plsp_tmp)
            {
                plsp_tmp->group_index = 0;
            }
        }

        if (NULL != pif->p_mplstp->egress_lsp)
        {
            pif->p_mplstp->egress_lsp->group_index = 0;

            plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->egress_lsp->lsp_index);
            if (NULL != plsp_tmp)
            {
                plsp_tmp->group_index = 0;
            }
        }

        if (NULL != pif->p_mplstp->backup_ingress_lsp)
        {
            /* 解绑 bfd */
            if (0 != pif->p_mplstp->backup_ingress_lsp->bfd_id)
            {
                bfd_unbind_for_lsp_pw(BFD_TYPE_LSP, pif->p_mplstp->backup_ingress_lsp->bfd_id,
                                    pif->p_mplstp->backup_ingress_lsp->lsp_index);

                pif->p_mplstp->backup_ingress_lsp->bfd_id = 0;
            }

            /* 解绑 oam */
            if (0 != pif->p_mplstp->backup_ingress_lsp->mplsoam_id)
            {
                tunnel_if_unbind_mplstp_oam(pif, TUNNEL_STATUS_BACKUP);
            }

            pif->p_mplstp->backup_ingress_lsp->group_index = 0;

            plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->backup_ingress_lsp->lsp_index);
            if (NULL != plsp_tmp)
            {
                plsp_tmp->group_index = 0;
            }
        }

        if (NULL != pif->p_mplstp->backup_egress_lsp)
        {
            pif->p_mplstp->backup_egress_lsp->group_index = 0;

            plsp_tmp = mpls_lsp_lookup(pif->p_mplstp->backup_egress_lsp->lsp_index);
            if (NULL != plsp_tmp)
            {
                plsp_tmp->group_index = 0;
            }
        }
    }

    ret = tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_INVALID, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
         MPLS_LOG_ERROR("IPC send to hal\n");

         return ret;
    }

    if (NULL != pif->p_mplstp)
    {
        XFREE(MTYPE_LSP_ENTRY, pif->p_mplstp);
    }

    hios_hash_delete(&tunnel_table, pitem);
    XFREE(MTYPE_IF, pitem->data);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tunnel 接口查找
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 tunnel 接口结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:32:13
 * @note       :
 */
struct tunnel_if *tunnel_if_lookup(uint32_t ifindex)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    pitem = hios_hash_find(&tunnel_table, (void *)ifindex);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct tunnel_if *)pitem->data;
}


/**
 * @brief      : 批量获取 tunnel 接口结构
 * @param[in ] : ifindex     - 接口索引
 * @param[out] : tunnel_buff - 获取到的 tunnel 结构数组
 * @return     : 返回获取到 tunnel 结构的数量
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:32:58
 * @note       : 提供给 snmp 的 get next 接口，空索引从数据结构开始返回数据，
 * @note       : 非空索引从传入索引节点下一个节点开始返回数据
 */
int tunnel_if_get_bulk(uint32_t ifindex, struct tunnel_if tunnel_buff[])
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct tunnel_if   *pif     = NULL;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct tunnel_if);
    int cursor;
    uint8_t link_state = 0;

    MPLS_LOG_DEBUG();

    if (0 == ifindex)
    {
        /* 初次获取 */
        HASH_BUCKET_LOOP(pbucket, cursor, tunnel_table)
        {
            pif = (struct tunnel_if *)pbucket->data;
            if (NULL == pif)
            {
                continue;
            }

            memcpy(&tunnel_buff[data_num], pif, sizeof(struct tunnel_if));
            if(ifm_get_link(pif->tunnel.outif, MODULE_ID_MPLS, &link_state) || (IFNET_LINKDOWN == link_state))
            {
                tunnel_buff[data_num].tunnel.outif = 0;
            }
            data_num ++;
            
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&tunnel_table, (void *)ifindex);

        if (NULL != pbucket)
        {
            for (data_num=0; data_num<msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&tunnel_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                pif = (struct tunnel_if *)pnext->data;
                memcpy(&tunnel_buff[data_num], pif, sizeof(struct tunnel_if));

                if(ifm_get_link(pif->tunnel.outif, MODULE_ID_MPLS, &link_state) || (IFNET_LINKDOWN == link_state))
                {
                    tunnel_buff[data_num].tunnel.outif = 0;
                }

                pbucket = pnext;
            }
        }
    }

    return data_num;
}


static int tunnel_if_get_hqos_snmp(uint32_t ifindex, struct hqos_t *phqos, struct hqos_tunnel *phqos_snmp)
{
    if(NULL == phqos || NULL == phqos_snmp )
    {
        MPLS_LOG_ERROR("Input param check error in func\n");

        return ERRNO_PARAM_ILLEGAL;
    }

    memset(phqos_snmp, 0, sizeof(struct hqos_tunnel));
    phqos_snmp->ifindex = ifindex;
    memcpy (&(phqos_snmp->hqos), phqos, sizeof(struct hqos_t));

    return ERRNO_SUCCESS;
}


int tunnel_if_get_hqos_bulk(uint32_t ifindex, struct hqos_tunnel hqos_array[])
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct tunnel_if   *pif     = NULL;
    struct hqos_t      *phqos   = NULL;
    struct hqos_tunnel  hqos_snmp;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct hqos_t);
    int cursor;

    MPLS_LOG_DEBUG();

    if (0 == ifindex)   /* 初次获取 */
    {
        HASH_BUCKET_LOOP(pbucket, cursor, tunnel_table)
        {
            pif = (struct tunnel_if *)pbucket->data;
            if ((NULL == pif) ||
                (0 == pif->tunnel.hqos_id))
            {
                continue;
            }

            phqos = qos_hqos_get(pif->tunnel.hqos_id, MODULE_ID_MPLS);
            if (NULL == phqos)
            {
                continue;
            }

            if (tunnel_if_get_hqos_snmp(pif->tunnel.ifindex, phqos, &hqos_snmp))
            {
                continue;
            }

            memcpy(&hqos_array[data_num++], &hqos_snmp, sizeof(struct hqos_tunnel));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&tunnel_table, (void *)ifindex);

        while (pbucket)
        {
            pnext = hios_hash_next_cursor(&tunnel_table, pbucket);
            if ((NULL == pnext) || (NULL == pnext->data))
            {
                break;
            }

            pbucket = pnext;

            pif = (struct tunnel_if *)(pnext->data);

            if (0 == pif->tunnel.hqos_id)
            {
                continue;
            }

            phqos = qos_hqos_get(pif->tunnel.hqos_id, MODULE_ID_MPLS);
            if (NULL == phqos)
            {
                continue;
            }

            if (tunnel_if_get_hqos_snmp(pif->tunnel.ifindex, phqos, &hqos_snmp))
            {
                continue;
            }

            memcpy(&hqos_array[data_num++], &hqos_snmp, sizeof(struct hqos_tunnel));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }

    return data_num;
}


/**
 * @brief      : 获取 tunnel 统计
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 tunnel 接口统计信息，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:35:53
 * @note       :
 */
struct counter_t *tunnel_if_get_counter(uint32_t ifindex)
{
    struct tunnel_if *pif      = NULL;
    struct ipc_mesg_n *pmesg = NULL;

    MPLS_LOG_DEBUG();

    pif = tunnel_if_lookup(ifindex);
    if ((NULL == pif) || (DISABLE == pif->tunnel.statis_enable))
    {
        return NULL;
    }

    pmesg = ipc_sync_send_n2(&pif->tunnel, sizeof(struct tunnel_t), 1,
                                    MODULE_ID_HAL, MODULE_ID_MPLS, IPC_TYPE_TUNNEL,
                                    TUNNEL_INFO_COUNTER, IPC_OPCODE_GET, ifindex, 1);
    if(NULL != pmesg)
    {
        return (struct counter_t *)pmesg->msg_data;
    }
    return NULL;
}


/**
 * @brief      : tp tunnel 添加静态 lsp
 * @param[in ] : pif         - tunnel 接口结构
 * @param[in ] : plsp        - 静态 lsp 结构
 * @param[in ] : backup_flag - 主或备 lsp
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:37:18
 * @note       :
 */
int tunnel_mplstp_add_lsp(struct tunnel_if *pif, struct static_lsp *plsp,
                                    enum TUNNEL_STATUS backup_flag)
{
    struct tunnel_mplstp *pmpls    = NULL;
    struct lsp_entry     *plsp_tmp = NULL;

    MPLS_LOG_DEBUG();

    if ((NULL == pif) || (NULL == plsp))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pmpls = pif->p_mplstp;
    if (NULL == pmpls)
    {
        return ERRNO_FAIL;
    }

    if (TUNNEL_STATUS_MASTER == backup_flag)        // 添加 tunnel 主 lsp
    {
        if (LSP_DIRECTION_INGRESS == plsp->direction)        // 配置 ingress 方向 lsp
        {
            pmpls->ingress_lsp = plsp;
        }
        else if (LSP_DIRECTION_EGRESS == plsp->direction)    // 配置 egress方向 lsp
        {
            pmpls->egress_lsp = plsp;

            if (NULL != pmpls->ingress_lsp)
            {
                plsp_tmp = mpls_lsp_lookup(pmpls->ingress_lsp->lsp_index);
                if (NULL != plsp_tmp)
                {
                    plsp_tmp->inlabel = pmpls->egress_lsp->inlabel;

                    mpls_lsp_update(plsp_tmp, LSP_SUBTYPE_INLABEL);
                }
            }
        }
        else
        {
            return ERRNO_PARAM_ILLEGAL;
        }
    }
    else if (TUNNEL_STATUS_BACKUP == backup_flag)    // 添加 tunnel 备 lsp
    {
        if (LSP_DIRECTION_INGRESS == plsp->direction)
        {
            pmpls->backup_ingress_lsp = plsp;
        }
        else if (LSP_DIRECTION_EGRESS == plsp->direction)
        {
            pmpls->backup_egress_lsp = plsp;

            if (NULL != pmpls->backup_ingress_lsp)
            {
                plsp_tmp = mpls_lsp_lookup(pmpls->backup_ingress_lsp->lsp_index);
                if (NULL != plsp_tmp)
                {
                    plsp_tmp->inlabel = pmpls->backup_egress_lsp->inlabel;

                    mpls_lsp_update(plsp_tmp, LSP_SUBTYPE_INLABEL);
                }
            }
        }
        else
        {
            return ERRNO_PARAM_ILLEGAL;
        }
    }

    plsp->group_index = pif->tunnel.ifindex;

    plsp_tmp = mpls_lsp_lookup(plsp->lsp_index);
    if (NULL != plsp_tmp)
    {
        plsp_tmp->group_index   = pif->tunnel.ifindex;
        plsp_tmp->domain_id     = pif->tunnel.domain_id;
        plsp_tmp->phb_id        = pif->tunnel.phb_id;
        plsp_tmp->phb_enable    = pif->tunnel.phb_enable;
        plsp_tmp->car_cir[0]    = pif->tunnel.car_cir[0];
        plsp_tmp->car_pir[0]    = pif->tunnel.car_pir[0];
        plsp_tmp->car_cir[1]    = pif->tunnel.car_cir[1];
        plsp_tmp->car_pir[1]    = pif->tunnel.car_pir[1];
        plsp_tmp->hqos_id       = pif->tunnel.hqos_id;
        plsp_tmp->statis_enable = pif->tunnel.statis_enable;

        mpls_lsp_update(plsp_tmp, LSP_SUBTYPE_TUNNEL);
    }

    tunnel_mplstp_set_status(pif);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tp tunnel 删除静态 lsp
 * @param[in ] : pif  - tunnel 接口结构
 * @param[in ] : plsp - 静态 lsp 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:43:39
 * @note       :
 */
int tunnel_mplstp_delete_lsp(struct tunnel_if *pif, struct static_lsp *plsp)
{
    struct tunnel_mplstp *pmpls    = NULL;
    struct lsp_entry     *plsp_tmp = NULL;

    MPLS_LOG_DEBUG();

    if ((NULL == pif) || (NULL == plsp))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pmpls = pif->p_mplstp;
    if (NULL == pmpls)
    {
        return ERRNO_SUCCESS;
    }

    if (pmpls->ingress_lsp == plsp)                // 删除 ingress 主 lsp
    {
        tunnel_if_unbind_mplstp_oam(pif, TUNNEL_STATUS_MASTER);

        if (0 != plsp->bfd_id)
        {
            bfd_unbind_for_lsp_pw(BFD_TYPE_LSP, plsp->bfd_id, plsp->lsp_index);
        }

        pmpls->ingress_lsp = NULL;
    }
    else if (pmpls->egress_lsp == plsp)            // 删除 egress 主 lsp
    {
        tunnel_if_unbind_mplstp_oam(pif, TUNNEL_STATUS_MASTER);

        pmpls->egress_lsp = NULL;
    }
    else if (pmpls->backup_ingress_lsp == plsp)    // 删除 ingress 备 lsp
    {
        tunnel_if_unbind_mplstp_oam(pif, TUNNEL_STATUS_BACKUP);

        if (0 != plsp->bfd_id)
        {
            bfd_unbind_for_lsp_pw(BFD_TYPE_LSP, plsp->bfd_id, plsp->lsp_index);
        }

        pmpls->backup_ingress_lsp = NULL;
    }
    else if (pmpls->backup_egress_lsp == plsp)     // 删除 egress 备 lsp
    {
        tunnel_if_unbind_mplstp_oam(pif, TUNNEL_STATUS_BACKUP);

        pmpls->backup_egress_lsp = NULL;
    }
    else
    {
        return ERRNO_SUCCESS;
    }

    plsp->group_index = 0;

    plsp_tmp = mpls_lsp_lookup(plsp->lsp_index);
    if (NULL != plsp_tmp)
    {
        plsp_tmp->group_index   = pif->tunnel.ifindex;
        plsp_tmp->domain_id     = pif->tunnel.domain_id;
        plsp_tmp->phb_id        = pif->tunnel.phb_id;
        plsp_tmp->phb_enable    = pif->tunnel.phb_enable;
        plsp_tmp->car_cir[0]    = pif->tunnel.car_cir[0];
        plsp_tmp->car_pir[0]    = pif->tunnel.car_pir[0];
        plsp_tmp->car_cir[1]    = pif->tunnel.car_cir[1];
        plsp_tmp->car_pir[1]    = pif->tunnel.car_pir[1];
        plsp_tmp->hqos_id       = pif->tunnel.hqos_id;
        plsp_tmp->statis_enable = pif->tunnel.statis_enable;

        mpls_lsp_update(plsp_tmp, LSP_SUBTYPE_TUNNEL);
    }

    tunnel_mplstp_set_status(pif);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tp tunnel 主备 lsp 回切
 * @param[in ] : pthread - 线程结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:45:40
 * @note       :
 */
int tunnel_mplstp_set_status_callback(void *para)
{
    struct tunnel_if *pif = NULL;

    pif = (struct tunnel_if *)(para);
    if (NULL == pif)
    {
        return ERRNO_FAIL;
    }

    /* 定时器超时置 NULL */
    pif->pwtr_timer = 0;

    pif->tunnel.backup_status = TUNNEL_STATUS_MASTER;

    tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_BACKUP, OPCODE_UPDATE);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tp tunnel 主备 lsp 默认切换规则
 * @param[in ] : pif - tunnel 接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:46:34
 * @note       :
 */
static void tunnel_mplstp_set_status_default(struct tunnel_if *pif)
{
    struct tunnel_mplstp *pmpls   = NULL;
    struct tunnel_t      *ptunnel = NULL;
    uint32_t backup = TUNNEL_STATUS_MASTER;

    if (NULL == pif)
    {
        return;
    }

    pmpls   = pif->p_mplstp;
    ptunnel = &(pif->tunnel);

    if (NULL != pmpls->backup_ingress_lsp)
    {
        if ((ENABLE == pmpls->backup_ingress_lsp->status)
            && (LINK_UP == pmpls->backup_ingress_lsp->down_flag))
        {
            if ((NULL != pmpls->ingress_lsp)
                && ((DISABLE == pmpls->ingress_lsp->status)
                    || (LINK_DOWN == pmpls->ingress_lsp->down_flag)))
            {
                backup = TUNNEL_STATUS_BACKUP;
            }
        }

        if (NULL == pmpls->ingress_lsp)
        {
            backup = TUNNEL_STATUS_BACKUP;
        }
    }

    if (ptunnel->backup_status != backup)
    {
        ptunnel->backup_status = backup;

        tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_BACKUP, OPCODE_UPDATE);
    }

    return ;
}


/**
 * @brief      : tp tunnel 主备 lsp 非回切处理
 * @param[in ] : pif - tunnel 接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:47:47
 * @note       :
 */
static void tunnel_mplstp_set_status_non_failback(struct tunnel_if *pif)
{
    struct tunnel_mplstp *pmpls   = NULL;
    struct tunnel_t      *ptunnel = NULL;
    uint32_t backup = TUNNEL_STATUS_MASTER;

    if (NULL == pif)
    {
        return;
    }

    pmpls   = pif->p_mplstp;
    ptunnel = &(pif->tunnel);

    if ((ENABLE == pmpls->backup_ingress_lsp->status)
        && (LINK_UP == pmpls->backup_ingress_lsp->down_flag))
    {
        if ((DISABLE == pmpls->ingress_lsp->status)
            || (LINK_DOWN == pmpls->ingress_lsp->down_flag))
        {
            backup = TUNNEL_STATUS_BACKUP;
        }
        else
        {
            if (ptunnel->backup_status == TUNNEL_STATUS_BACKUP)
            {
                backup = TUNNEL_STATUS_BACKUP;
            }
        }
    }

    if (ptunnel->backup_status != backup)
    {
        ptunnel->backup_status = backup;

        tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_BACKUP, OPCODE_UPDATE);
    }

    return ;
}


/**
 * @brief      : tp tunnel 处理主备工作状态
 * @param[in ] : pif - tunnel 接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:49:11
 * @note       :
 */
int tunnel_mplstp_process_status(struct tunnel_if *pif)
{
    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (TUNNEL_FAILBACK_IS_VALID(pif))
    {
        if (pif->tunnel.failback == FAILBACK_ENABLE)
        {
            if ((pif->tunnel.backup_status == TUNNEL_STATUS_BACKUP)
                && ((pif->p_mplstp->ingress_lsp->status == ENABLE)
                    && (pif->p_mplstp->ingress_lsp->down_flag == LINK_UP))
                && ((pif->p_mplstp->backup_ingress_lsp->status == ENABLE)
                    && (pif->p_mplstp->backup_ingress_lsp->down_flag == LINK_UP)))
            {
                MPLS_TIMER_DEL(pif->pwtr_timer);
                pif->pwtr_timer = MPLS_TIMER_ADD(tunnel_mplstp_set_status_callback, pif, pif->tunnel.wtr);

                return ERRNO_SUCCESS;
            }
        }
        else if (pif->tunnel.failback == FAILBACK_DISABLE)
        {
            MPLS_TIMER_DEL(pif->pwtr_timer);
            tunnel_mplstp_set_status_non_failback(pif);

            return ERRNO_SUCCESS;
        }
    }

    MPLS_TIMER_DEL(pif->pwtr_timer);
    tunnel_mplstp_set_status_default(pif);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tp tunnel 设置主备工作状态
 * @param[in ] : pif - tunnel 接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:50:34
 * @note       :
 */
int tunnel_mplstp_set_status(struct tunnel_if *pif)
{
    struct tunnel_mplstp *pmpls   = NULL;
    struct tunnel_t      *ptunnel = NULL;
    int down_flag = LINK_DOWN;
    int change    = 0;

    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pmpls = pif->p_mplstp;
    ptunnel = &(pif->tunnel);

    if (NULL != pmpls->ingress_lsp)         // 主非空
    {
        ptunnel->master_index = pmpls->ingress_lsp->lsp_index;
        memcpy(pif->inlsp_name, pmpls->ingress_lsp->name, NAME_STRING_LEN);

        if (LINK_UP == pmpls->ingress_lsp->down_flag)
        {
            down_flag = LINK_UP;
        }
    }
    else
    {
        ptunnel->master_index = 0;
        memset(pif->inlsp_name, 0, NAME_STRING_LEN);
    }

    if (NULL != pmpls->backup_ingress_lsp)  // 备非空
    {
        ptunnel->backup_index = pmpls->backup_ingress_lsp->lsp_index;
        memcpy(pif->backup_inlsp_name, pmpls->backup_ingress_lsp->name, NAME_STRING_LEN);

        if (LINK_UP == pmpls->backup_ingress_lsp->down_flag)
        {
            down_flag = LINK_UP;
        }
    }
    else
    {
        ptunnel->backup_index = 0;
        memset(pif->backup_inlsp_name, 0, NAME_STRING_LEN);
    }

    if (NULL != pmpls->egress_lsp)          // 反向主非空
    {
        ptunnel->master_egress_index = pmpls->egress_lsp->lsp_index;
        memcpy(pif->elsp_name, pmpls->egress_lsp->name, NAME_STRING_LEN);
    }
    else
    {
        ptunnel->master_egress_index = 0;
        memset(pif->elsp_name, 0, NAME_STRING_LEN);
    }

    if (NULL != pmpls->backup_egress_lsp)   // 反向备非空
    {
        ptunnel->backup_egress_index = pmpls->backup_egress_lsp->lsp_index;
        memcpy(pif->backup_elsp_name, pmpls->backup_egress_lsp->name, NAME_STRING_LEN);
    }
    else
    {
        ptunnel->backup_egress_index = 0;
        memset(pif->backup_elsp_name, 0, NAME_STRING_LEN);
    }

    /* link 状态改变 */
    if (ptunnel->down_flag != down_flag)
    {
        ptunnel->down_flag = down_flag;
        change             = 1;
    }

    tunnel_if_tunnel_t_download(&pif->tunnel, TUNNEL_INFO_BACKUP, OPCODE_UPDATE);
    tunnel_mplstp_process_status(pif);

    /* link 状态通知 */
    if (0 != change)
    {
        if (down_flag == LINK_UP)
        {
            tunnel_if_up(pif);
        }
        else
        {
            tunnel_if_down(pif);
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tp tunnel 处理静态 lsp 变更事件
 * @param[in ] : plsp   - 静态 lsp 结构
 * @param[in ] : opcode - 操作码
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:52:14
 * @note       :
 */
void tunnel_mplstp_process_lsp_event(struct static_lsp *plsp, enum OPCODE_E opcode)
{
    struct tunnel_if *pif = NULL;
    int ret = ERRNO_NOT_FOUND;

    MPLS_LOG_DEBUG();

    if (plsp->group_index == 0)
    {
        return;
    }

    pif = tunnel_if_lookup(plsp->group_index);
    if (pif == NULL)
    {
        return;
    }

    if ((opcode == OPCODE_UP) || (opcode == OPCODE_DOWN))
    {
        MPLS_LOG_DEBUG("pif->tunnel.aps_id %d ",pif->tunnel.aps_id);
        if (0 != pif->tunnel.aps_id)
        {
            if ((pif->p_mplstp == NULL)
            || (pif->p_mplstp->ingress_lsp == NULL)
            || (pif->p_mplstp->backup_ingress_lsp == NULL))
            {
                return;
            }

            MPLS_LOG_DEBUG("opcode %d ",opcode);
            if (pif->p_mplstp->ingress_lsp->lsp_index == plsp->lsp_index)
            {
                if (opcode == OPCODE_DOWN)
                {
                    ret = mplsaps_state_update(pif->tunnel.aps_id, APS_OAM_STATE_SF_DOWN_W);
                }
                else if (opcode == OPCODE_UP)
                {
                    ret = mplsaps_state_update(pif->tunnel.aps_id, APS_OAM_STATE_SF_UP_W);
                }
            }
            else if(pif->p_mplstp->backup_ingress_lsp->lsp_index == plsp->lsp_index)
            {
                if (opcode == OPCODE_DOWN)
                {
                    ret = mplsaps_state_update(pif->tunnel.aps_id,APS_OAM_STATE_SF_DOWN_P);
                }
                else if (opcode == OPCODE_UP)
                {
                    ret = mplsaps_state_update(pif->tunnel.aps_id,APS_OAM_STATE_SF_UP_P);
                }
            }
        }
        if(ERRNO_NOT_FOUND == ret)
        {
            tunnel_mplstp_set_status(pif);
        }
    }
    else if (opcode == OPCODE_DELETE)
    {
        if (plsp == pif->p_mplstp->ingress_lsp)
        {
            tunnel_mplstp_delete_lsp(pif, pif->p_mplstp->egress_lsp);
        }
        else if (plsp == pif->p_mplstp->backup_ingress_lsp)
        {
            tunnel_mplstp_delete_lsp(pif, pif->p_mplstp->backup_egress_lsp);
        }

        tunnel_mplstp_delete_lsp(pif, plsp);
    }

    return;
}


/**
 * @brief      : pw 绑定 tunnel
 * @param[in ] : pif - tunnel 接口结构
 * @param[in ] : ppw - pw 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:56:12
 * @note       :
 */
int tunnel_if_bind_pw(struct tunnel_if *pif, struct l2vc_entry *pl2vc)
{
    struct pw_info *ppw = NULL;
    MPLS_LOG_DEBUG();

    if ((NULL == pif) || (NULL == pl2vc))
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    ppw = &(pl2vc->pwinfo);

    /* pw ingress car cir 大于 tunnel ingress car cir */
    if(ERRNO_FAIL == tunnel_if_process_pw_car(pif, pl2vc->car_cir[0], OPCODE_ADD, 0))
    {
        MPLS_LOG_DEBUG("process pw car error\n");
    }

    /* pw egress car cir 大于 tunnel egress car cir */
    if(ERRNO_FAIL == tunnel_if_process_pw_car(pif, pl2vc->car_cir[1], OPCODE_ADD, 1))
    {
        MPLS_LOG_DEBUG("process pw car error\n");
    }
    
    if (pif->pwlist == NULL)
    {
        pif->pwlist = list_new();
    }

    if (NULL == listnode_lookup(pif->pwlist, ppw))
    {
        listnode_add(pif->pwlist, ppw);
    }

    ppw->admin_up = LINK_UP;

    if (LINK_DOWN == ppw->up_flag)
    {
        ppw->up_flag = LINK_UP;
        gmpls.pw_up_num++;
    }

    /* tunnel 使能统计计数，将 pw 也使能 */
    if (pif->tunnel.statis_enable == ENABLE)
    {
        pl2vc->statis_enable = ENABLE;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 解绑 tunnel
 * @param[in ] : pif - tunnel 接口结构
 * @param[in ] : ppw - pw 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:57:27
 * @note       :
 */
int tunnel_if_unbind_pw(struct tunnel_if *pif, struct l2vc_entry *pl2vc)
{
    struct pw_info *ppw = NULL;
    MPLS_LOG_DEBUG();

    if ((NULL == pif) || (NULL == pl2vc) || (NULL == pif->pwlist))
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    ppw = &(pl2vc->pwinfo);

    if(ERRNO_FAIL == tunnel_if_process_pw_car(pif, pl2vc->car_cir[0], OPCODE_DELETE, 0))
    {
        MPLS_LOG_DEBUG("process pw car error\n");
    }
    
    if(ERRNO_FAIL == tunnel_if_process_pw_car(pif, pl2vc->car_cir[1], OPCODE_DELETE, 1))
    {
        MPLS_LOG_DEBUG("process pw car error\n");
    }

    listnode_delete(pif->pwlist, ppw);

    if (list_isempty(pif->pwlist))
    {
        list_free(pif->pwlist);

        pif->pwlist = NULL;
    }

    ppw->admin_up  = LINK_DOWN;
    ppw->nhp_index = 0;

    if (ppw->up_flag == LINK_UP)
    {
        ppw->up_flag = LINK_DOWN;
        gmpls.pw_up_num--;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tunnel 绑定 oam
 * @param[in ] : pif         - tunnel 接口结构
 * @param[in ] : oam_id      - oam 会话 id
 * @param[in ] : backup_flag - 主或备 lsp
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:58:12
 * @note       :
 */
int tunnel_if_bind_mplstp_oam(struct tunnel_if *pif, uint16_t oam_id,
                                        enum TUNNEL_STATUS backup_flag)
{
    struct lsp_entry  *plspm     = NULL;
    struct static_lsp *pslsp     = NULL;
    struct static_lsp *pslsp_egr = NULL;
    int ret;

    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (TUNNEL_STATUS_MASTER == backup_flag)
    {
        pslsp     = pif->p_mplstp->ingress_lsp;
        pslsp_egr = pif->p_mplstp->egress_lsp;
    }
    else if (TUNNEL_STATUS_BACKUP == backup_flag)
    {
        pslsp     = pif->p_mplstp->backup_ingress_lsp;
        pslsp_egr = pif->p_mplstp->backup_egress_lsp;
    }

    if (NULL == pslsp)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (0 != pslsp->mplsoam_id)
    {
        return ERRNO_ALREADY_BIND_TPOAM;
    }

    if (0 != pslsp->bfd_id)
    {
        return ERRNO_EXISTED;
    }

    ret = mplsoam_session_enable(oam_id, pslsp->lsp_index, OAM_TYPE_LSP);
    if (ERRNO_SUCCESS != ret)
    {
        return ret;
    }

    pslsp->mplsoam_id = oam_id;
    if (NULL != pslsp_egr)
    {
        pslsp_egr->mplsoam_id = oam_id;
    }

    plspm = mpls_lsp_lookup(pslsp->lsp_index);
    if (NULL != plspm)
    {
        plspm->mplsoam_id    = oam_id;
        plspm->statis_enable = ENABLE;

        mpls_lsp_update(plspm, LSP_SUBTYPE_MPLSTP_OAM);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tunnel 解绑 oam
 * @param[in ] : pif         - tunnel 接口结构
 * @param[in ] : backup_flag - 主或备 lsp
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 16:59:41
 * @note       :
 */
int tunnel_if_unbind_mplstp_oam(struct tunnel_if *pif, enum TUNNEL_STATUS backup_flag)
{
    struct lsp_entry  *plspm     = NULL;
    struct static_lsp *pslsp     = NULL;
    struct static_lsp *pslsp_egr = NULL;

    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (TUNNEL_STATUS_MASTER == backup_flag)
    {
        pslsp     = pif->p_mplstp->ingress_lsp;
        pslsp_egr = pif->p_mplstp->egress_lsp;
    }
    else if (TUNNEL_STATUS_BACKUP == backup_flag)
    {
        pslsp     = pif->p_mplstp->backup_ingress_lsp;
        pslsp_egr = pif->p_mplstp->backup_egress_lsp;
    }

    if ((NULL == pslsp) || (pslsp->mplsoam_id == 0))
    {
        return ERRNO_SUCCESS;
    }

    mplsoam_session_disable(pslsp->mplsoam_id);

    plspm = mpls_lsp_lookup(pslsp->lsp_index);
    if (NULL != plspm)
    {
        plspm->mplsoam_id = 0;

        mpls_lsp_update(plspm, LSP_SUBTYPE_MPLSTP_OAM);
    }

    pslsp->mplsoam_id = 0;
    if (NULL != pslsp_egr)
    {
        pslsp_egr->mplsoam_id = 0;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tunnel 接口 up
 * @param[in ] : pif - tunnel 接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月7日 17:00:54
 * @note       :
 */
void tunnel_if_up(struct tunnel_if *pif)
{
    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return;
    }

    l2vc_process_tunnel_interface_up(pif);
    mpls_l3vpn_process_tunnel_up(pif->tunnel.ifindex);
    tunnel_if_alarm_process(LINK_UP, pif->tunnel.ifindex);

    return;
}


/**
 * @brief      : tunnel 接口 down
 * @param[in ] : pif - tunnel 接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月7日 17:06:02
 * @note       :
 */
void tunnel_if_down(struct tunnel_if *pif)
{
    struct list       *plist    = NULL;
    struct listnode   *node     = NULL;
    struct listnode   *nextnode = NULL;
    struct l2vc_entry *pl2vc    = NULL;
    struct pw_info    *ppw      = NULL;
    int               *pdata    = NULL;

    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return;
    }

    if (NULL != pif->pwlist)
    {
        plist = pif->pwlist;
        for (ALL_LIST_ELEMENTS(plist, node, nextnode, pdata))
        {
            ppw = (struct pw_info *)pdata;

            /* 处理 tunnel car cir 剩余 */
            pl2vc = pw_get_l2vc(ppw);
            if (NULL != pl2vc)
            {
                tunnel_if_process_pw_car(pif, pl2vc->car_cir[0], OPCODE_DELETE, 0);
                tunnel_if_process_pw_car(pif, pl2vc->car_cir[1], OPCODE_DELETE, 1);
            }

            pw_tunnel_down(ppw);
            list_delete_node(plist, node);
        }
    }

    mpls_l3vpn_process_tunnel_down(pif->tunnel.ifindex);
    tunnel_if_alarm_process(LINK_DOWN, pif->tunnel.ifindex);

    return;
}


/**
 * @brief      : tunnel 使能统计，将绑定该 tunnel 的所有 pw 都使能统计
 * @param[in ] : pif  - tunnel 接口结构
 * @param[in ] : flag - 使能或去使能
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月7日 17:08:05
 * @note       :
 */
void tunnel_if_set_pw_statistics(struct tunnel_if *pif, int flag)
{
    struct listnode   *pnode   = NULL;
    struct pw_info    *ppwinfo = NULL;
    struct l2vc_entry *pl2vc   = NULL;

    MPLS_LOG_DEBUG();

    if ((NULL == pif) || (NULL == pif->pwlist))
    {
        return;
    }

    for (ALL_LIST_ELEMENTS_RO(pif->pwlist, pnode, ppwinfo))
    {
        if (ppwinfo == NULL)
        {
            continue;
        }

        pl2vc = pw_get_l2vc(ppwinfo);
        if (NULL != pl2vc)
        {
            pl2vc->statis_enable = flag;
        }
    }

    return;
}


/**
 * @brief      : tunnel 上报告警
 * @param[in ] : down_flag - up/down 状态
 * @param[in ] : ifindex   - 接口索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月7日 17:09:56
 * @note       :
 */
void tunnel_if_alarm_process(uint8_t down_flag, uint32_t ifindex)
{

    MPLS_LOG_DEBUG();

    //memset(&if_usp, 0, sizeof(struct ifm_usp));
    //ifm_get_usp_by_ifindex(ifindex, &if_usp);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_TUNNEL_TYPE;
	gPortInfo.iIfindex = ifindex;

    if (LINK_UP == down_flag)
    {
        //ipran_alarm_report(IFM_TUNNEL_TYPE, if_usp.slot, if_usp.sub_port, 0, 0, GPN_ALM_TYPE_TUNNEL_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
        ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_TUNNEL_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
    }
    else if (LINK_DOWN == down_flag)
    {
        //ipran_alarm_report(IFM_TUNNEL_TYPE, if_usp.slot, if_usp.sub_port, 0, 0, GPN_ALM_TYPE_TUNNEL_DOWN, GPN_SOCK_MSG_OPT_RISE);
        ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_TUNNEL_DOWN, GPN_SOCK_MSG_OPT_RISE);
    }

//    alarm_report(opcode, MODULE_ID_MPLS, ALM_CODE_RAN_TNL_DOWN, ifindex, 0, ALM_STYPE_ETH, 0);
}


/**
 * @brief      : 计算当前 tunnel 下所有 pw 限速总和
 * @param[in ] : pif       - tunnel 接口结构
 * @param[in ] : cir       - 承诺信息率
 * @param[in ] : opcode    - 操作码
 * @param[in ] : direction - 入或出方向
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月7日 17:10:33
 * @note       :
 */
int tunnel_if_process_pw_car(struct tunnel_if *pif, uint32_t cir,
                                        enum OPCODE_E opcode, int direction)
{
    uint32_t tunnel_car_cir = 0;
    uint32_t tunnel_car_cir_sum = 0;

    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (0 == cir)
    {
        return ERRNO_SUCCESS;
    }

    tunnel_car_cir = pif->tunnel.car_cir[direction];
    tunnel_car_cir_sum = pif->tunnel.car_cir_sum[direction];

    cir &= ~CAR_VALUE_L1_FLAG;
    tunnel_car_cir &= ~CAR_VALUE_L1_FLAG;
    tunnel_car_cir_sum &= ~CAR_VALUE_L1_FLAG;

    if (opcode == OPCODE_ADD)
    {
        if ((tunnel_car_cir != 0) && (tunnel_car_cir < (tunnel_car_cir_sum+cir)))
        {
            return ERRNO_FAIL;
        }

        pif->tunnel.car_cir_sum[direction] += cir;
    }
    else if (opcode == OPCODE_DELETE)
    {
        pif->tunnel.car_cir_sum[direction] -= cir;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : gre tunnel 响应 outif 接口状态
 * @param[in ] : ifidnex - 接口索引
 * @param[in ] : opcode  - 操作码
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月7日 17:12:40
 * @note       :
 */
void tunnel_if_process_ifevent(uint32_t ifindex, enum OPCODE_E opcode)
{
    struct hash_bucket *pbucket = NULL;
    struct tunnel_if   *pif     = NULL;
    struct tunnel_if    lif;
    int cursor = 0;

    MPLS_LOG_DEBUG();

    if (0 == ifindex)
    {
        return;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, tunnel_table)
    {
        pif = (struct tunnel_if *)pbucket->data;
        if (NULL == pif)
        {
            continue;
        }

        if (pif->tunnel.outif == ifindex)
        {
            memcpy(&lif, pif, sizeof(struct tunnel_if));

            MPLS_LOG_DEBUG("flag = %d, opcode = %d\n", pif->set_flag, opcode);

            if ((0 == pif->set_flag) && (OPCODE_ADD == opcode))
            {
                pif->set_flag = 1;

                tunnel_if_tunnel_t_download(&lif.tunnel, TUNNEL_INFO_OUTIF, OPCODE_UPDATE);
            }
            else if ((1 == pif->set_flag) && (OPCODE_DELETE == opcode))
            {
                pif->set_flag    = 0;
                lif.tunnel.outif = 0;
                memset(lif.tunnel.dmac, 0, MAC_LEN);

                tunnel_if_tunnel_t_download(&lif.tunnel, TUNNEL_INFO_OUTIF, OPCODE_UPDATE);
            }

            if (OPCODE_ADD == opcode)           // interface up event
            {
                tunnel_if_alarm_process(LINK_UP, pif->tunnel.ifindex);
            }
            else if (OPCODE_DOWN == opcode)     // interface down event
            {
                tunnel_if_alarm_process(LINK_DOWN, pif->tunnel.ifindex);
            }
            else if (OPCODE_DELETE == opcode)   // interface delete event
            {
                tunnel_if_alarm_process(LINK_DOWN, pif->tunnel.ifindex);
            }
        }
    }
}


