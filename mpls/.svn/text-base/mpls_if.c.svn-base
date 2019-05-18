/**
 * @file      : mpls_if.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 8:55:17
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/log.h>
#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include "mpls_main.h"
#include "mpls.h"
#include "tunnel.h"
#include "mpls_if.h"
#include "ldp/ldp.h"
#include "ldp/ldp_adjance.h"
#include "vpls_cmd.h"
#include "lsp_static.h"
#include "pw_cmd.h"
#include "bfd/bfd_packet.h"
#include "mpls_oam/mpls_oam.h"
#include "mpls_aps/mpls_aps.h"
#include "mpls_oam/mpls_oam_csf.h"

extern void mplsaps_unbind_lsp_pw(uint16_t sess_id);
extern int mpls_if_ldp_disable(struct mpls_if *pif);

struct hash_table mpls_if_table;


/**
 * @brief      : 显示 mpls 接口配置
 * @param[in ] : vty - vty 全局结构
 * @param[in ] : pif -
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 8:57:36
 * @note       :
 */
static void mpls_if_show(struct vty *vty, struct mpls_if *pif)
{
    char ifname[NAME_STRING_LEN] = "";

    if (pif == NULL)
    {
        return;
    }

    if ((NULL == pif->ppw_slave)
        && (NULL == pif->ppw_master)
        && (0 == pif->vpn)
        && (0 == pif->mplsoam_id)
        && (0 == pif->bfd_sessid))
    {
        return;
    }

    ifm_get_name_by_ifindex(pif->ifindex, ifname);

    vty_out(vty, "%-15s : %s%s", "Interface", ifname, VTY_NEWLINE);

    if (IFM_TYPE_IS_METHERNET(pif->ifindex) && IFM_TYPE_IS_TRUNK(pif->ifindex))
    {
        vty_out(vty, "%-15s : %s%s", "Mode", pif->mode == IFNET_MODE_L2 ? "L2" :
                   pif->mode == IFNET_MODE_SWITCH ? "switch" :
                   pif->mode == IFNET_MODE_L3 ? "L3" : "-", VTY_NEWLINE);
    }
    else if (IFM_TYPE_IS_TDM(pif->ifindex))
    {
        vty_out(vty, "%-15s : %s%s", "Mode", pif->tdm_mode == MPLS_IF_CES_MODE_UNFRAMED ? "unframed" :
                   pif->tdm_mode == MPLS_IF_CES_MODE_FRAMED ? "framed" :
                   pif->tdm_mode == MPLS_IF_CES_MODE_MULTIFRAMED ? "multiframed" : "-", VTY_NEWLINE);
    }

    vty_out(vty, "%-15s : %s%s", "Link status", pif->down_flag == IFNET_LINKUP ?
                "up" : "down", VTY_NEWLINE);
    vty_out(vty, "%-15s : %d%s", "Vpn", pif->vpn, VTY_NEWLINE);

    if ((pif->mode == IFNET_MODE_L2) || IFM_TYPE_IS_TDM(pif->ifindex))
    {
        MPLS_LOG_DEBUG("pif %p ifindex %x pif->ppw_master %p \n",pif, pif->ifindex, pif->ppw_master);
        if (pif->ppw_master != NULL)
        {
            vty_out(vty, "%-15s : %s%s", "PW work status", pif->pw_backup ==
                        BACKUP_STATUS_SLAVE ? "slave" : "master", VTY_NEWLINE);
            vty_out(vty, "%-15s : %s%s", "PW master", pif->ppw_master->name, VTY_NEWLINE);
        }

        if (pif->ppw_slave != NULL)
        {
            vty_out(vty, "%-15s : %s%s", "PW slave", pif->ppw_slave->name, VTY_NEWLINE);
        }

        if (pif->ppw_master != NULL)
        {
            if (pif->ppw_master->failback == FAILBACK_DISABLE)
            {
                vty_out(vty, "%-15s : %s%s", "failback status", "non-failback", VTY_NEWLINE);
            }
            else if (pif->ppw_master->failback == FAILBACK_ENABLE)
            {
                vty_out(vty, "%-15s : %s%s", "failback status", "failback", VTY_NEWLINE);
                vty_out(vty, "%-15s : %us%s", "failback wtr", pif->ppw_master->wtr, VTY_NEWLINE);
            }
        }
    }

    vty_out(vty, "%-15s : %d%s", "Mplsoam-session", pif->mplsoam_id, VTY_NEWLINE);
    vty_out(vty, "%-15s : %d%s", "bfd_sessid", pif->bfd_sessid, VTY_NEWLINE);
    vty_out(vty, "%-15s : %d%s", "aps_sessid", pif->aps_sessid, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);

    return;
}


/**
 * @brief      : 计算 mpls 接口哈希表 key 值
 * @param[in ] : hash_key - 接口索引
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:00:20
 * @note       :
 */
static unsigned int mpls_if_compute_hash(void *hash_key)
{
     return (uint32_t)hash_key;
}


/**
 * @brief      : 比较 mpls 接口哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - 接口索引
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:01:11
 * @note       :
 */
static int mpls_if_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = pitem;

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
 * @brief      : mpls 接口哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:01:55
 * @note       :
 */
void mpls_if_table_init ( int size )
{
    hios_hash_init(&mpls_if_table, size, mpls_if_compute_hash, mpls_if_compare);

    return;
}


/**
 * @brief      : mpls 接口结构获取
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 mpls 接口结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:02:20
 * @note       : 如果结构存在则返回结构，如果不存在则创建新的结构并返回
 */
struct mpls_if *mpls_if_get(uint32_t ifindex)
{
    struct mpls_if *pif = NULL;

    MPLS_LOG_DEBUG();

    pif = mpls_if_lookup(ifindex);
    if (pif == NULL)
    {
        pif = mpls_if_create(ifindex);
    }

    return pif;
}


/**
 * @brief      : mpls 接口创建
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回分配内存后的 mpls 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:03:46
 * @note       :
 */
struct mpls_if *mpls_if_create(uint32_t ifindex)
{
    struct mpls_if  *pif  = NULL;
    struct ifm_info pifm;
    int ret;

    MPLS_LOG_DEBUG("ifindex\n");

    /* 获取接口信息 */
    if(ifm_get_all_info(ifindex, MODULE_ID_MPLS, &pifm))
    {
        MPLS_LOG_ERROR("Get interface info failed!\n");
        return NULL;
    }

    pif = (struct mpls_if *)XCALLOC(MTYPE_IF, sizeof(struct mpls_if));
    if (pif == NULL)
    {
        MPLS_LOG_ERROR("malloc\n");

        return NULL;
    }

    pif->ifindex   = ifindex;
    pif->mode      = pifm.mode;
    pif->down_flag = pifm.status;

    ret = mpls_if_add(pif);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("mpls if add !\n");

        XFREE(MTYPE_IF, pif);

        return NULL;
    }

    return pif;
}


/**
 * @brief      : mpls 接口添加
 * @param[in ] : pif - mpls 接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:06:42
 * @note       :
 */
int mpls_if_add(struct mpls_if *pif)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_FAIL;
    }

    if (mpls_if_table.hash_size == mpls_if_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
        MPLS_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)pif->ifindex;
    pitem->data     = pif;

    hios_hash_add(&mpls_if_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : mpls 接口删除
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:07:43
 * @note       :
 */
void mpls_if_delete(uint32_t ifindex)
{
    struct hash_bucket *pbucket = NULL;
    struct mpls_if     *pif     = NULL;

    MPLS_LOG_DEBUG(" ifindex %x\n", ifindex);

    static_lsp_process_ifdown(ifindex);
    tunnel_if_process_ifevent(ifindex, OPCODE_DELETE);

    pbucket = hios_hash_find(&mpls_if_table, (void *)ifindex);
    if (NULL == pbucket)
    {
        return;
    }

    pif = (struct mpls_if *)(pbucket->data);
    if (NULL != pif)
    {
        mpls_if_delete_interface_bfd(pif);
        mplsaps_unbind_lsp_pw(pif->aps_sessid);
        mpls_if_delete_pw(pif);
        mpls_if_delete_vpls(pif);
        mpls_if_delete_local_ldp(pif);
        mpls_if_unbind_mplsoam_sess(pif);
    }

    hios_hash_delete(&mpls_if_table, pbucket);
    XFREE(MTYPE_IF, pif);
    XFREE(MTYPE_HASH_BACKET, pbucket);

    return;
}


/**
 * @brief      : mpls 接口查找
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 mpls 接口结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:10:10
 * @note       :
 */
struct mpls_if *mpls_if_lookup(uint32_t ifindex)
{
    struct hash_bucket *pbucket = NULL;
    struct mpls_if *pif = NULL;

    MPLS_LOG_DEBUG();

    pbucket = hios_hash_find(&mpls_if_table, (void *)ifindex);
    if (NULL == pbucket)
    {
        return NULL;
    }
    pif = (struct mpls_if *)pbucket->data;
    MPLS_LOG_DEBUG(" ifinde %x successful mode %d\n",ifindex, pif->mode);
    
    return (struct mpls_if *)pbucket->data;
}


/**
 * @brief      : mpls 接口 down
 * @param[in ] : ifindex - 接口索引
 * @param[in ] : mode    - 接口模式
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:10:45
 * @note       :
 */
void mpls_if_down(uint32_t ifindex, int mode)
{
    struct mpls_if *pif;

    MPLS_LOG_DEBUG();

    static_lsp_process_ifevent(ifindex, mode, OPCODE_DOWN);
    tunnel_if_process_ifevent(ifindex, OPCODE_DOWN);

    pif = mpls_if_lookup(ifindex);
    if ((NULL == pif) || (pif->down_flag == LINK_DOWN))
    {
        return;
    }

    pif->down_flag = LINK_DOWN;

    mpls_if_down_for_pw(pif);
	mpls_if_update_tpoam_csf(pif);

    return;
}


/**
 * @brief      : mpls 接口 up
 * @param[in ] : ifindex - 接口索引
 * @param[in ] : mode    - 接口模式
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:11:48
 * @note       :
 */
void mpls_if_up(uint32_t ifindex, int mode)
{
    struct mpls_if *pif;

    MPLS_LOG_DEBUG();

    static_lsp_process_ifevent(ifindex, mode, OPCODE_UP);
    tunnel_if_process_ifevent(ifindex, OPCODE_ADD);

    pif = mpls_if_lookup(ifindex);
    if ((NULL == pif) || (pif->down_flag == LINK_UP))
    {
        return;
    }

    pif->down_flag = LINK_UP;

    mpls_if_up_for_pw(pif);
	mpls_if_update_tpoam_csf(pif);

    return;
}


/**
 * @brief      : mpls 接口模式改变
 * @param[in ] : ifindex - 接口索引
 * @param[in ] : mode    - 接口模式
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:14:11
 * @note       :
 */
void mpls_if_mode_change(uint32_t ifindex, int mode)
{
    struct mpls_if *pif = NULL;

    MPLS_LOG_DEBUG(" ifindex %x\n", ifindex);

    static_lsp_process_ifevent(ifindex, mode, OPCODE_UPDATE);

    pif = mpls_if_lookup(ifindex);
    if ((NULL == pif))
    {
        MPLS_LOG_DEBUG(" ifindex %x error\n", ifindex);
        return;
    }
    if(pif->mode == mode)
    {
        MPLS_LOG_DEBUG(" ifindex %x\n", ifindex);
        MPLS_LOG_ERROR(" error ifindex %x\n", ifindex);
        return;
    }

    if (IFNET_MODE_L2 == pif->mode)
    {
        MPLS_LOG_DEBUG();
        mplsaps_unbind_lsp_pw(pif->aps_sessid);
        mpls_if_delete_pw(pif);
        mpls_if_delete_vpls(pif);

        pif->pw_backup = BACKUP_STATUS_INVALID;
    }

    if (IFNET_MODE_L3 == pif->mode)
    {
        MPLS_LOG_DEBUG();
        mpls_if_unbind_mplsoam_sess(pif);
        mpls_if_delete_local_ldp(pif);
    }
    MPLS_LOG_DEBUG(" finished ifindex %x\n", ifindex);
    pif->mode = mode;

    return;
}


/**
 * @brief      : mpls 接口 down，将所有 pw ac 置 down
 * @param[in ] : pif - mpls 接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:15:34
 * @note       :
 */
void mpls_if_down_for_pw(struct mpls_if *pif)
{
    MPLS_LOG_DEBUG();

    if (pif == NULL)
    {
        return;
    }

    if (NULL != pif->ppw_slave)
    {
        pif->ppw_slave->ac_flag = LINK_DOWN;
    }

    if (NULL != pif->ppw_master)
    {
        pif->ppw_master->ac_flag = LINK_DOWN;
    }

    return;
}


/**
 * @brief      : mpls 接口 up，将所有 pw ac 置 up
 * @param[in ] : pif - mpls 接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:16:31
 * @note       :
 */
void mpls_if_up_for_pw(struct mpls_if *pif)
{
    MPLS_LOG_DEBUG();

    if (pif == NULL)
    {
        return;
    }

    if (NULL != pif->ppw_slave)
    {
        pif->ppw_slave->ac_flag = LINK_UP;
    }

    if (NULL != pif->ppw_master)
    {
        pif->ppw_master->ac_flag = LINK_UP;
    }

    return;
}


/*删除接口时，使接口下的bfd会话管理down*/
int mpls_if_delete_interface_bfd(struct mpls_if *pif)
{
    struct bfd_sess *psess = NULL;
    struct bfd_sess sess_del;

    if(NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    if(0 == pif->bfd_sessid)
    {
        return ERRNO_NOT_FOUND;
    }

    psess = bfd_session_lookup(pif->bfd_sessid);
    if(NULL == psess)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    memset(&sess_del, 0 , sizeof(struct bfd_sess));
    sess_del.index = psess->index;
    sess_del.type = BFD_TYPE_INTERFACE;

    if(psess->local_id)
    {
        sess_del.local_id = psess->local_id;
        bfd_fsm(BFD_EVENT_SESSION_DELETE, psess);
    }
    else if(psess->local_id_cfg)
    {
        sess_del.local_id = psess->local_id_cfg;
        bfd_fsm(BFD_EVENT_DISABLE, psess);
    }

    bfd_send_hal(&sess_del, IPC_OPCODE_DELETE, 0);

    return ERRNO_SUCCESS;
}


/* 删除接口或接口模式改变时删除本地LDP配置 */
int mpls_if_delete_local_ldp(struct mpls_if *pif)
{
    MPLS_LOG_DEBUG();
    pif->ldp_conf.ldp_enable = DISABLE;
    return mpls_if_ldp_disable(pif);
}


/**
 * @brief      : mpls 接口删除，删除 pw 配置
 * @param[in ] : pif - mpls 接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:17:48
 * @note       :
 */
int mpls_if_delete_pw(struct mpls_if *pif)
{
    struct l2vc_entry *pl2vc = NULL;

    MPLS_LOG_DEBUG("ifindex %x\n", pif->ifindex);

    if (pif == NULL)
    {
        MPLS_LOG_DEBUG();
        return ERRNO_PARAM_ILLEGAL;
    }

    if (pif->vpn != 0)
    {
        MPLS_LOG_DEBUG();
        return ERRNO_SUCCESS;
    }

    if (NULL != pif->ppw_slave)
    {
        MPLS_LOG_DEBUG();
        pl2vc = pif->ppw_slave;

        if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
        {
            l2vc_delete_martini(pl2vc);
        }

        l2vc_unbind_lsp_tunnel(pl2vc);
        tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc->pwinfo.nhp_index), pl2vc);
        mpls_pw_mplsif_download(pif->ifindex, &pl2vc->pwinfo.pwindex, MPLSIF_INFO_SLAVE_PW, OPCODE_DELETE);
        mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_DELETE);
        pw_delete_lsp(&pl2vc->pwinfo);
        pw_free_lsp_index(&pl2vc->pwinfo);
        mpls_if_unbind_pw(pif, pl2vc);
    }

    MPLS_LOG_DEBUG("pif %p pif->ppw_master %p ----------\n", pif, pif->ppw_master);

    if (NULL != pif->ppw_master)
    {
        MPLS_LOG_DEBUG();
        pl2vc = pif->ppw_master;
        if (0 != pif->ppw_master->pwtr_timer)
        {
            /* 接口删除，一定要将未跑完的定时器 cancle 掉 */
            MPLS_TIMER_DEL(pif->ppw_master->pwtr_timer);
        }

        if (PW_PROTO_IS_MARTINI(&pl2vc->pwinfo))
        {
            l2vc_delete_martini(pl2vc);
        }

        l2vc_unbind_lsp_tunnel(pl2vc);
        tunnel_if_unbind_pw(tunnel_if_lookup(pl2vc->pwinfo.nhp_index), pl2vc);
        mpls_pw_mplsif_download(pif->ifindex, &pl2vc->pwinfo.pwindex, MPLSIF_INFO_MASTER_PW, OPCODE_DELETE);
        mpls_pw_pwinfo_download(&pl2vc->pwinfo, OPCODE_DELETE);
        pw_delete_lsp(&pl2vc->pwinfo);
        pw_free_lsp_index(&pl2vc->pwinfo);
        mpls_if_unbind_pw(pif, pl2vc);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : mpls 接口删除，删除 vpls 配置
 * @param[in ] : pif - mpls 接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:26:32
 * @note       :
 */
int mpls_if_delete_vpls(struct mpls_if *pif)
{
    struct vsi_entry *pvsi      = NULL;
    struct listnode  *pnode     = NULL;
    struct listnode  *pnextnode = NULL;
    uchar            *pmac      = NULL;

    MPLS_LOG_DEBUG();

    if (pif == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (0 != pif->vpn)
    {
        if (!list_isempty(&pif->mac_list))
        {
            vsi_interface_mac_download(pif->ifindex, NULL, OPCODE_DOWN);
        }

        for (ALL_LIST_ELEMENTS(&pif->mac_list, pnode, pnextnode, pmac))
        {
            XFREE(MTYPE_VSI_ENTRY, pmac);
            list_delete_node(&pif->mac_list, pnode);

            gmpls.vsi_pw_num--;
        }

        pvsi = vsi_lookup(pif->vpn);
        if (NULL != pvsi)
        {
            vsi_ac_interface_download(pvsi, pif->ifindex, OPCODE_DELETE);
            vsi_delete_if(pvsi, pif->ifindex);
        }
    }

    return ERRNO_SUCCESS;
}



/**
 * @brief      : mpls 接口绑定 pw
 * @param[in ] : pif   - mpls 接口结构
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:29:14
 * @note       :
 */
int mpls_if_bind_pw(struct mpls_if *pif, struct l2vc_entry *pl2vc)
{
    struct pw_info *ppwinfo = NULL;

    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (0 != pif->vpn)
    {
        return ERRNO_ALREADY_BIND_L2VPN;
    }

    ppwinfo = &(pl2vc->pwinfo);
    if (ppwinfo->pw_type == PW_TYPE_MASTER)
    {
        if (NULL != pif->ppw_master)
        {
            return ERRNO_EXISTED;
        }

        pif->ppw_master = pl2vc;
    }
    else if (ppwinfo->pw_type == PW_TYPE_BACKUP)
    {
        if (NULL == pif->ppw_master)
        {
            return ERRNO_MASTER_NOT_EXISTED;
        }

        if (NULL != pif->ppw_slave)
        {
            return ERRNO_EXISTED;
        }

        pif->ppw_slave = pl2vc;
    }

    if (pif->down_flag == LINK_DOWN)
    {
        pl2vc->ac_flag = LINK_DOWN;
    }
    else if (pif->down_flag == LINK_UP)
    {
        pl2vc->ac_flag = LINK_UP;
    }

    pl2vc->pwinfo.ifindex = pif->ifindex;

    return ERRNO_SUCCESS;
}


/**
 * @brief      : mpls 接口解绑 pw
 * @param[in ] : pif   - mpls 接口结构
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:30:23
 * @note       :
 */
int mpls_if_unbind_pw(struct mpls_if *pif, struct l2vc_entry *pl2vc)
{
    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (pif->ppw_master == pl2vc)
    {
        MPLS_LOG_DEBUG();
        pif->ppw_master = NULL;
    }
    else if (pif->ppw_slave == pl2vc)
    {
        MPLS_LOG_DEBUG();
        pif->ppw_slave = NULL;
    }

    pl2vc->pwinfo.ifindex = 0;
    pl2vc->ac_flag        = LINK_DOWN;
    pl2vc->failback       = FAILBACK_ENABLE;
    pl2vc->wtr            = 30;

    return ERRNO_SUCCESS;
}


int mpls_if_bind_bfd_sess(uint32_t ifindex, uint16_t sess_id)
{
    struct mpls_if *pif;

    pif = mpls_if_get(ifindex);
    if (pif == NULL)
    {
        return ERRNO_FAIL;
    }

    pif->bfd_sessid = sess_id;

    return ERRNO_SUCCESS;
}


int mpls_if_unbind_bfd_sess(uint32_t ifindex)
{
    struct mpls_if *pif;

    pif = mpls_if_lookup(ifindex);
    if (pif == NULL)
    {
        return ERRNO_NOT_FOUND;
    }

    pif->bfd_sessid = 0;

    return ERRNO_SUCCESS;
}


int mpls_if_bind_mplsoam_sess(struct mpls_if *pif, uint16_t session_id)
{
    int ret;

    if(NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if(pif->mplsoam_id != 0)
    {
        return ERRNO_ALREADY_ENABLE_OAM;
    }

    ret = mplsoam_session_enable(session_id, pif->ifindex, OAM_TYPE_INTERFACE);
    if(ret == 0)
    {
        pif->mplsoam_id = session_id;
    }

    return ret;
}


int mpls_if_unbind_mplsoam_sess(struct mpls_if *pif)
{
    if(NULL == pif || 0 == pif->mplsoam_id)
    {
        MPLS_LOG_DEBUG();
        return ERRNO_PARAM_ILLEGAL;
    }
    MPLS_LOG_DEBUG();
    /* 禁止oam 会话*/
    mplsoam_session_disable(pif->mplsoam_id);
    pif->mplsoam_id = 0;

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tdm 接口模式配置
 * @param[in ] : unframed    - 非成帧模式
 * @param[in ] : framed      - 成帧模式
 * @param[in ] : multiframed - 多成帧模式
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:31:49
 * @note       :
 */
DEFUN (mpls_if_tdm_mode,
    mpls_if_tdm_mode_cmd,
    "mode (unframed | framed | multiframed)",
    MPLS_CLI_INFO)
{
    struct mpls_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    pif = mpls_if_get(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if ('u' == argv[0][0])
    {
        pif->tdm_mode = MPLS_IF_CES_MODE_UNFRAMED;
    }
    else if ('f' == argv[0][0])
    {
        pif->tdm_mode = MPLS_IF_CES_MODE_FRAMED;
    }
    else if ('m' == argv[0][0])
    {
        pif->tdm_mode = MPLS_IF_CES_MODE_MULTIFRAMED;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : tdm 接口模式删除
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:34:28
 * @note       :
 */
DEFUN (no_mpls_if_tdm_mode,
    no_mpls_if_tdm_mode_cmd,
    "no mode",
    MPLS_CLI_INFO)
{
    struct mpls_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    pif = mpls_if_get(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if (pif->tdm_mode != MPLS_IF_CES_MODE_INVALID)
    {
        mpls_if_delete_pw(pif);
    }

    pif->tdm_mode = MPLS_IF_CES_MODE_INVALID;

    return CMD_SUCCESS;
}


/**
 * @brief      : mpls 接口配置显示
 * @param[in ] : ethernet USP         - 出接口类型为 ethernet
 * @param[in ] : gigabitethernet USP  - 出接口类型为 gigabitethernet
 * @param[in ] : xgigabitethernet USP - 出接口类型为 xgigabitethernet
 * @param[in ] : tdm USP              - 出接口类型为 tdm
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:35:30
 * @note       :
 */
DEFUN (show_mpls_interface,
    show_mpls_interface_cmd,
    "show mpls interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP | tdm USP}",
    MPLS_CLI_INFO)
{
    struct hash_bucket *pbucket = NULL;
    struct mpls_if     *pif     = NULL;
    int                 cursor;
    uint32_t            ifindex = 0;

    if ((argv[0] == NULL) && (argv[1] == NULL) && (argv[2] == NULL) && (argv[3] == NULL))
    {
        HASH_BUCKET_LOOP(pbucket, cursor, mpls_if_table)
        {
            pif = (struct mpls_if *)pbucket->data;
            if (pif == NULL)
            {
                continue;
            }

            mpls_if_show(vty, pif);
        }
    }
    else
    {
        if (argv[0] != NULL)
        {
            ifindex = ifm_get_ifindex_by_name("ethernet", argv[0]);
        }
        else if (argv[1] != NULL)
        {
            ifindex = ifm_get_ifindex_by_name("gigabitethernet", argv[1]);
        }
        else if (argv[2] != NULL)
        {
            ifindex = ifm_get_ifindex_by_name("xgigabitethernet", argv[2]);
        }
        else if (argv[3] != NULL)
        {
            ifindex = ifm_get_ifindex_by_name("tdm", argv[3]);
        }

        if (ifindex == 0)
        {
            vty_out(vty, "%%Wrong format, please check out !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        pif = mpls_if_lookup(ifindex);
        if (pif == NULL)
        {
            return CMD_SUCCESS;
        }

        mpls_if_show(vty, pif);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : mpls 接口配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:37:54
 * @note       :
 */
static int mpls_if_config_write(struct vty *vty)
{
    char ifname[IFNET_NAMESIZE] = "";
    char srcip[INET_ADDRSTRLEN] = "";
    struct hash_bucket *pbucket     = NULL;
    struct hash_bucket *pbucket_bfd = NULL;
    struct mpls_if     *pif         = NULL;
    struct bfd_sess    *psess       = NULL;
    uint32_t ifindex;
    int      cursor = 0;

    HASH_BUCKET_LOOP(pbucket, cursor, mpls_if_table)
    {
        pif = (struct mpls_if *)pbucket->data;
        if (NULL == pif)
        {
            continue;
        }

        ifindex = pif->ifindex;

        ifm_get_name_by_ifindex(ifindex, ifname);

        vty_out (vty, "interface %s%s", ifname, VTY_NEWLINE);

        MPLS_LOG_DEBUG("pif %p ifindex %x pif->ppw_master %p 111111111111111111\n",pif ,ifindex,  pif->ppw_master);

        if (NULL != pif->ppw_master)
        {
            vty_out(vty," mpls l2vpn pw %s%s", pif->ppw_master->name, VTY_NEWLINE);
        }

        if (NULL != pif->ppw_slave)
        {
            vty_out(vty," mpls l2vpn pw %s backup%s", pif->ppw_slave->name, VTY_NEWLINE);
        }

        if (0 != pif->vpn)
        {
            vty_out(vty, " mpls vsi %d%s", pif->vpn, VTY_NEWLINE);
        }

        if (pif->mplsoam_id != 0)
        {
            vty_out(vty, " mplstp-oam enable session %d%s", pif->mplsoam_id, VTY_NEWLINE);
        }

        if ((NULL != pif->ppw_master)
            && (pif->ppw_master->failback == FAILBACK_ENABLE)
            && (pif->ppw_master->wtr != 30))
        {
            vty_out(vty, " mpls pw backup failback wtr %u%s", pif->ppw_master->wtr, VTY_NEWLINE);
        }
        else if ((NULL != pif->ppw_master)
            &&(pif->ppw_master->failback == FAILBACK_DISABLE))
        {
            vty_out(vty, " mpls pw backup non-failback%s", VTY_NEWLINE);
        }

        if (DISABLE != pif->ldp_conf.ldp_enable)
        {
            vty_out(vty, " mpls ldp enable%s", VTY_NEWLINE);
        }

    	if (gbfd.enable)
        {
        	if(!(pif->bfd_sessid))
            {
            	continue;
            }

        	pbucket_bfd = hios_hash_find(&bfd_session_table, (void *)(uint32_t)(pif->bfd_sessid));
        	if(!pbucket_bfd)
            {
                continue;
            }
            if(!(pbucket_bfd->data))
            {
                continue;
            }

        	psess = (struct bfd_sess *)pbucket_bfd->data;
        	if(psess->session_id_cfg)
            {
            	if(psess->srcip)
                {
                	inet_ipv4tostr(psess->srcip, srcip);
                	vty_out(vty, " bfd enable session %d source-ip %s%s", psess->session_id_cfg, srcip, VTY_NEWLINE);
                }
            	else
                {
                	vty_out(vty, " bfd enable session %d%s", psess->session_id_cfg, VTY_NEWLINE);
                }
            }
        	else if(psess->session_id)
            {
            	if(psess->srcip)
                {
                	inet_ipv4tostr(psess->srcip, srcip);
                	vty_out(vty, " bfd enable source-ip %s%s", srcip, VTY_NEWLINE);
                }
            	else
                {
                	vty_out(vty, " bfd enable%s", VTY_NEWLINE);
                }
            }
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : mpls 接口命令行初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 9:39:41
 * @note       :
 */
void mpls_if_cmd_init(void)
{
    ifm_l2if_init (mpls_if_config_write);

    install_element (CONFIG_NODE, &show_mpls_interface_cmd, CMD_LOCAL);
    install_element (PHYSICAL_IF_NODE, &show_mpls_interface_cmd, CMD_LOCAL);
    install_element (PHYSICAL_SUBIF_NODE, &show_mpls_interface_cmd, CMD_LOCAL);
    install_element (TRUNK_IF_NODE, &show_mpls_interface_cmd, CMD_LOCAL);
    install_element (TRUNK_SUBIF_NODE, &show_mpls_interface_cmd, CMD_LOCAL);
    install_element (TDM_IF_NODE, &show_mpls_interface_cmd, CMD_LOCAL);
    install_element (TDM_SUBIF_NODE, &show_mpls_interface_cmd, CMD_LOCAL);

    install_element (TDM_IF_NODE, &mpls_if_tdm_mode_cmd, CMD_SYNC);
    install_element (TDM_IF_NODE, &no_mpls_if_tdm_mode_cmd, CMD_SYNC);

    return;
}


