/**
 * @file      : pw.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月1日 10:21:42
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/command.h>
#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/zassert.h>
#include <lib/log.h>
#include <lib/alarm.h>
#include "mpls_main.h"
#include "pw.h"
#include "pw_cmd.h"
#include "tunnel.h"
#include "labelm.h"
#include "mpls.h"
#include "lspm.h"
#include "lsp_fec.h"
#include "lsp_static.h"
#include "vpls.h"
#include "ldp/ldp_pw.h"
#include "mpls_aps/mpls_aps_fsm.h"
#include "bfd/bfd_session.h"


struct hash_table l2vc_table;
struct hash_table pw_table;


/**
 * @brief      : 计算 l2vc 哈希表 key 值
 * @param[in ] : hash_key - pw 名称
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:22:18
 * @note       : 通过 pw 名字转换为无符号整数计算获得
 */
static unsigned int l2vc_compute(void *hash_key)
{
    return hios_hash_string(hash_key);
}


/**
 * @brief      : 比较 l2vc 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - pw 名称
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:25:59
 * @note       :
 */
static int l2vc_compare(void *pitem, void *hash_key)
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
 * @brief      : l2vc 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:28:26
 * @note       :
 */
void l2vc_table_init(uint32_t size)
{
    hios_hash_init(&l2vc_table, size, l2vc_compute, l2vc_compare);

    return;
}


/**
 * @brief      : l2vc 节点内存释放
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:28:55
 * @note       :
 */
void l2vc_free(struct l2vc_entry *pl2vc)
{
    if (NULL != pl2vc)
    {
        XFREE(MTYPE_PW_ENTRY, pl2vc);
    }

    return;
}


/**
 * @brief      : l2vc 节点创建并初始化
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回分配内存后的 l2vc 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:29:46
 * @note       :
 */
struct l2vc_entry *l2vc_create(void)
{
    struct l2vc_entry *pl2vc = NULL;

    MPLS_LOG_DEBUG();

    pl2vc = (struct l2vc_entry *)XCALLOC(MTYPE_PW_ENTRY, sizeof(struct l2vc_entry));
    if (pl2vc == NULL)
    {
        MPLS_LOG_ERROR("malloc\n");

        return NULL;
    }

    /* 初始化基本信息 */
    pl2vc->ac_flag         = LINK_DOWN;
    pl2vc->pwinfo.admin_up = LINK_DOWN;
    pl2vc->pwinfo.up_flag  = LINK_DOWN;
    pl2vc->pwinfo.status = ENABLE;
    pl2vc->pwinfo.protocol = PW_PROTO_SVC;
    pl2vc->pwinfo.ac_type  = AC_TYPE_ETH;
    pl2vc->pwinfo.pw_type  = PW_TYPE_MASTER;
    pl2vc->pwinfo.mtu      = 1500;
    pl2vc->pwinfo.sample_interval = 5;
    pl2vc->phb_enable      = ENABLE;
    pl2vc->phb_id          = 0;
    pl2vc->failback        = FAILBACK_ENABLE;
    pl2vc->wtr             = 30;

    return pl2vc;
}


/**
 * @brief      : l2vc 节点添加
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:30:52
 * @note       :
 */
int l2vc_add(struct l2vc_entry *pl2vc)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (pl2vc == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (l2vc_table.hash_size == l2vc_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        MPLS_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;;
    }

    pitem->hash_key = pl2vc->name;
    pitem->data     = pl2vc;

    hios_hash_add(&l2vc_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l2vc 节点删除
 * @param[in ] : pname - pw 名称
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:32:44
 * @note       :
 */
int l2vc_delete(uchar *pname)
{
    struct hash_bucket *pitem = NULL;
    struct l2vc_entry  *pl2vc = NULL;

    MPLS_LOG_DEBUG();

    if (pname == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pitem = hios_hash_find(&l2vc_table, pname);
    if (pitem == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pl2vc = (struct l2vc_entry *)pitem->data;
    if (pl2vc != NULL)
    {
        if (LINK_UP == pl2vc->pwinfo.up_flag)
        {
            gmpls.pw_up_num--;
        }

        pw_delete(pl2vc->pwinfo.pwindex);
        label_free(pl2vc->inlabel);
        XFREE(MTYPE_PW_ENTRY, pl2vc);
    }

    hios_hash_delete(&l2vc_table, pitem);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l2vc 节点查找
 * @param[in ] : pname - pw 名称
 * @param[out] :
 * @return     : 成功返回 l2vc 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:33:30
 * @note       :
 */
struct l2vc_entry *l2vc_lookup(uchar *pname)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (pname == NULL)
    {
        return NULL;
    }

    pitem = hios_hash_find(&l2vc_table, pname);
    if (pitem == NULL)
    {
        return NULL;
    }

    return (struct l2vc_entry *)pitem->data;
}


/**
 * @brief      : 获取 pw 统计信息
 * @param[in ] : pname - pw 名称
 * @param[out] :
 * @return     : 成功返回统计信息结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:36:28
 * @note       :
 */
struct counter_t *l2vc_get_counter(uchar *pname)
{
    struct ipc_mesg_n *pmesg = NULL;
    struct l2vc_entry *pl2vc    = NULL;

    pl2vc = l2vc_lookup(pname);
    if ((NULL == pl2vc) || (DISABLE == pl2vc->statis_enable))
    {
        return NULL;
    }

    pmesg = ipc_sync_send_n2(&pl2vc->pwinfo, sizeof(struct pw_info), 1, MODULE_ID_HAL,
                                        MODULE_ID_MPLS, IPC_TYPE_PW, PW_SUBTYPE_COUNTER,
                                        IPC_OPCODE_GET, pl2vc->pwinfo.pwindex, 1);
    if(NULL != pmesg)
    {
        //pmesg->msghdr.data_num == 1;
        return (struct counter_t  *)pmesg->msg_data;
    }
    return NULL;
}


/**
 * @brief      : pw 解绑 lsp 隧道
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:37:11
 * @note       :
 */
int l2vc_unbind_lsp_tunnel(struct l2vc_entry *pl2vc)
{
    if (NULL == pl2vc)
    {
        return ERRNO_SUCCESS;
    }
    MPLS_LOG_DEBUG();
    if ((pl2vc->tunl_index != 0) && (pl2vc->pwinfo.nhp_type == NHP_TYPE_LSP))
    {
        pl2vc->pwinfo.admin_up  = LINK_DOWN;
        pl2vc->pwinfo.nhp_index = 0;

        if (pl2vc->pwinfo.up_flag == LINK_UP)
        {
            pl2vc->pwinfo.up_flag = LINK_DOWN;
            gmpls.pw_up_num--;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 选择 ldp lsp 隧道
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 lsp 隧道索引，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月1日 15:02:51
 * @note       :
 */
uint32_t l2vc_select_tunnel_ldp_lsp(struct l2vc_entry *pl2vc)
{
    struct lsp_fec_t  *pfec  = NULL;
    struct lsp_nhp_t  *pnhp  = NULL;
    struct listnode   *pnode = NULL;
    struct inet_prefix prefix;

    MPLS_LOG_DEBUG();

    if ((NULL == pl2vc) || (0 == pl2vc->ldp_tunnel))
    {
        return ERRNO_SUCCESS;
    }

    prefix.type      = pl2vc->peerip.type;
    prefix.addr.ipv4 = pl2vc->peerip.addr.ipv4;
    prefix.prefixlen = 32;

    pfec = lsp_fec_match(&prefix);
    if ((pfec == NULL) || (pfec->nhplist.count == 0))
    {
        return ERRNO_SUCCESS;
    }
    MPLS_LOG_DEBUG("pw name %s \n", pl2vc->pwinfo.name);
    
    for (ALL_LIST_ELEMENTS_RO(&pfec->nhplist, pnode, pnhp))
    {
        if ((NULL == pnhp) || (0 == pnhp->ingress_lsp))
        {
            continue;
        }
        MPLS_LOG_DEBUG("pw name %s select lsp success nhp_index %x\n", pl2vc->pwinfo.name, pnhp->ingress_lsp);
        pl2vc->pwinfo.nhp_type  = NHP_TYPE_LSP;
        pl2vc->pwinfo.nhp_index = pnhp->ingress_lsp;
        pl2vc->pwinfo.admin_up  = LINK_UP;

        if (LINK_DOWN == pl2vc->pwinfo.up_flag)
        {
            pl2vc->pwinfo.up_flag = LINK_UP;
            gmpls.pw_up_num++;
        }

        return pnhp->ingress_lsp;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月8日 16:44:44
 * @note       :
 */
static int l2vc_select_tunnel_static_lsp(struct l2vc_entry *pl2vc)
{
    struct static_lsp *plsp = NULL;

    MPLS_LOG_DEBUG();

    if (NULL == pl2vc)
    {
        return ERRNO_FAIL;
    }

    plsp = static_lsp_select(pl2vc->peerip.addr.ipv4);
    if (NULL == plsp)
    {
        return ERRNO_FAIL;
    }

    pl2vc->pwinfo.nhp_type  = NHP_TYPE_LSP;
    pl2vc->pwinfo.nhp_index = plsp->lsp_index;
    pl2vc->pwinfo.admin_up  = LINK_UP;

    if (LINK_DOWN == pl2vc->pwinfo.up_flag)
    {
        pl2vc->pwinfo.up_flag = LINK_UP;
        gmpls.pw_up_num++;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 配置隧道策略时，选中隧道 down，进行隧道重选
 * @param[in ] : ppwinfo - pw 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月1日 15:18:33
 * @note       :
 */
void l2vc_select_tunnel_new(struct pw_info *ppwinfo)
{
    struct l2vc_entry *pl2vc = NULL;

    MPLS_LOG_DEBUG();

    if (NULL == ppwinfo)
    {
        return;
    }

    pl2vc = pw_get_l2vc(ppwinfo);
    if (NULL == pl2vc)
    {
        return;
    }

    if (pl2vc->outlabel == 0)
    {
        return;
    }

    if (PW_ALREADY_BIND_AC(ppwinfo)
        || PW_ALREADY_BIND_VSI(ppwinfo)
        || PW_ALREADY_BIND_MSPW(ppwinfo))
    {
        if (PW_ALREADY_CONFIG_LDP_TUNNEL(pl2vc)
            || PW_ALREADY_CONFIG_SLSP_TUNNEL(pl2vc)
            || PW_ALREADY_CONFIG_TP_TUNNEL(pl2vc)
            || PW_ALREADY_CONFIG_GRE_TUNNEL(pl2vc)
            || PW_ALREADY_CONFIG_TE_TUNNEL(pl2vc))
        {
            l2vc_select_tunnel(pl2vc);
            if (LINK_UP == pl2vc->pwinfo.admin_up)
            {
                pl2vc->pwinfo.admin_up = LINK_DOWN;
                pw_tunnel_up(&pl2vc->pwinfo);
            }
        }
    }

    return;
}


/**
 * @brief      : pw 选择隧道（静态/动态 tunnel）
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 tunnel 结构，失败返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月1日 15:25:57
 * @note       :
 */
struct tunnel_if *l2vc_select_tunnel_interface(struct l2vc_entry *pl2vc)
{
    struct hash_bucket *pbucket = NULL;
    struct tunnel_if   *pif     = NULL;
    int cursor;

    MPLS_LOG_DEBUG();

    /* 静态绑定 tunnel */
    if (0 != pl2vc->tunl_index)
    {
        pif = tunnel_if_lookup(pl2vc->tunl_index);
        if (NULL == pif)
        {
            return NULL;
        }

        if (pif->tunnel.dip.addr.ipv4 != pl2vc->peerip.addr.ipv4)
        {
            mpls_errno = ERRNO_PEER_IP_UNMATCH;

            return pif;
        }

        if (LINK_UP == pif->tunnel.down_flag)
        {
            pl2vc->pwinfo.nhp_index = pif->tunnel.ifindex;
            pl2vc->pwinfo.nhp_type  = NHP_TYPE_TUNNEL;

            tunnel_if_bind_pw(pif, pl2vc);
        }

        return pif;
    }

    /* 根据隧道策略选隧道 */
    if (PW_ALREADY_CONFIG_TP_TUNNEL(pl2vc) || PW_ALREADY_CONFIG_GRE_TUNNEL(pl2vc))
    {
        /* 遍历 tunnel 表，找到 dip 与 pw peerip 相同的 */
        HASH_BUCKET_LOOP(pbucket, cursor, tunnel_table)
        {
            pif = (struct tunnel_if *)pbucket->data;
            if (NULL == pif)
            {
                continue;
            }

            /* 隧道策略未使能 mpls-tp 隧道 */
            if ((!PW_ALREADY_CONFIG_TP_TUNNEL(pl2vc))
                && (pif->tunnel.protocol == TUNNEL_PRO_MPLSTP))
            {
                continue;
            }

            /* 隧道策略未使能 gre 隧道 */
            if ((!PW_ALREADY_CONFIG_GRE_TUNNEL(pl2vc))
                && (pif->tunnel.protocol == TUNNEL_PRO_GRE))
            {
                continue;
            }

            if (LINK_DOWN == pif->tunnel.down_flag)
            {
                continue;
            }

            if (pif->tunnel.dip.addr.ipv4 == pl2vc->peerip.addr.ipv4)
            {
                pl2vc->pwinfo.nhp_index = pif->tunnel.ifindex;
                pl2vc->pwinfo.nhp_type  = NHP_TYPE_TUNNEL;

                tunnel_if_bind_pw(pif, pl2vc);

                return pif;
            }
        }
    }

    return NULL;
}


/**
 * @brief      : pw 选择隧道处理
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月1日 15:33:32
 * @note       :
 */
int l2vc_select_tunnel(struct l2vc_entry *pl2vc)
{
    struct tunnel_if  *pif  = NULL;
    struct static_lsp *plsp = NULL;
    int ret = 0;

    MPLS_LOG_DEBUG();

    if (PW_ALREADY_CONFIG_LDP_TUNNEL(pl2vc))
    {
        ret = l2vc_select_tunnel_ldp_lsp(pl2vc);
        if (ERRNO_SUCCESS != ret)
        {
            return ERRNO_SUCCESS;
        }
    }

    if (PW_ALREADY_CONFIG_SLSP_TUNNEL(pl2vc))
    {
        ret = l2vc_select_tunnel_static_lsp(pl2vc);
        if (ERRNO_SUCCESS == ret)
        {
            return ERRNO_SUCCESS;
        }
    }

    if (PW_ALREADY_CONFIG_STATIC_TUNNEL(pl2vc)
        || PW_ALREADY_CONFIG_TP_TUNNEL(pl2vc)
        || PW_ALREADY_CONFIG_GRE_TUNNEL(pl2vc))
    {
        pif = l2vc_select_tunnel_interface(pl2vc);
        if (PW_ALREADY_CONFIG_STATIC_TUNNEL(pl2vc))
        {
            if (NULL == pif)
            {
                return ERRNO_NOT_FOUND;
            }

            if (mpls_errno == ERRNO_PEER_IP_UNMATCH)
            {
                mpls_errno = ERRNO_SUCCESS;

                return ERRNO_PEER_IP_UNMATCH;
            }
            else if (mpls_errno == ERRNO_PW_INGRESS_CIR)
            {
                mpls_errno = ERRNO_SUCCESS;

                return ERRNO_PW_INGRESS_CIR;
            }
            else if (mpls_errno == ERRNO_PW_EGRESS_CIR)
            {
                mpls_errno = ERRNO_SUCCESS;

                return ERRNO_PW_EGRESS_CIR;
            }
        }

        return ERRNO_SUCCESS;
    }

    if (PW_ALREADY_CONFIG_LSP_TUNNEL(pl2vc))
    {
        plsp = static_lsp_lookup(pl2vc->lsp_name);
        if ((NULL != plsp)
            && (LSP_DIRECTION_INGRESS == plsp->direction)
            && (LINK_UP == plsp->down_flag))
        {
            pl2vc->pwinfo.admin_up  = LINK_UP;
            pl2vc->pwinfo.nhp_type  = NHP_TYPE_LSP;
            pl2vc->pwinfo.nhp_index = plsp->lsp_index;

            if (LINK_DOWN == pl2vc->pwinfo.up_flag)
            {
                pl2vc->pwinfo.up_flag = LINK_UP;
                gmpls.pw_up_num++;
            }
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 处理 tunnel up 事件
 * @param[in ] : pif - tunnel 接口结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月1日 16:04:03
 * @note       :
 */
void l2vc_process_tunnel_interface_up(struct tunnel_if *pif)
{
    struct hash_bucket *pbucket = NULL;
    struct pw_info     *pwinfo  = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    int cursor;

    MPLS_LOG_DEBUG();

    HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
    {
        pl2vc = (struct l2vc_entry *)(pbucket->data);
        if (NULL == pl2vc)
        {
            continue;
        }

        pwinfo = &(pl2vc->pwinfo);

        /*
         * 1、pw 必须绑定才有选择隧道的权利
         * 2、对于 up 的 pw 隧道不进行重选
         * 3、动态 pw 出标签不存在，不进行隧道选择
         * 4、pw 的 peer 与 tunnel 的 destination 必须保证一致
         * 5、配置的隧道类型与当前隧道类型不匹配时，则选择失败
         */
        if ((PW_ALREADY_BIND_AC(pwinfo) || PW_ALREADY_BIND_VSI(pwinfo) || PW_ALREADY_BIND_MSPW(pwinfo))
            && (pwinfo->admin_up == LINK_DOWN)
            && (pl2vc->outlabel != 0)
            && (memcmp(&(pl2vc->peerip), &(pif->tunnel.dip), sizeof(struct inet_addr)) == 0))
        {
            if ((PW_ALREADY_CONFIG_STATIC_TUNNEL(pl2vc) && (pif->tunnel.ifindex == pl2vc->tunl_index))
                || (PW_ALREADY_CONFIG_TP_TUNNEL(pl2vc) && (pif->tunnel.protocol == TUNNEL_PRO_MPLSTP))
                || (PW_ALREADY_CONFIG_TE_TUNNEL(pl2vc) && (pif->tunnel.protocol == TUNNEL_PRO_MPLSTE))
                || (PW_ALREADY_CONFIG_GRE_TUNNEL(pl2vc) && (pif->tunnel.protocol == TUNNEL_PRO_GRE)))
            {
                /* 隧道策略未使能 mpls-tp 隧道 */
                if (!PW_ALREADY_CONFIG_STATIC_TUNNEL(pl2vc)
                    && (!PW_ALREADY_CONFIG_TP_TUNNEL(pl2vc))
                    && (pif->tunnel.protocol == TUNNEL_PRO_MPLSTP))
                {
                    continue;
                }

                /* 隧道策略未使能 gre 隧道 */
                if (!PW_ALREADY_CONFIG_STATIC_TUNNEL(pl2vc)
                    && (!PW_ALREADY_CONFIG_GRE_TUNNEL(pl2vc))
                    && (pif->tunnel.protocol == TUNNEL_PRO_GRE))
                {
                    continue;
                }

                pl2vc->pwinfo.nhp_type  = NHP_TYPE_TUNNEL;
                pl2vc->pwinfo.nhp_index = pif->tunnel.ifindex;

                pw_tunnel_up(pwinfo);
                tunnel_if_bind_pw(pif, pl2vc);
            }
        }
    }

    return;
}


/**
 * @brief      : pw 处理 lsp 隧道 up 事件
 * @param[in ] : plsp - 静态 lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月1日 16:12:06
 * @note       :
 */
void l2vc_process_tunnel_static_lsp_up(struct static_lsp *plsp)
{
    struct hash_bucket *pbucket = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    int cursor;

    MPLS_LOG_DEBUG();

    if (NULL == plsp)
    {
        return;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
    {
        pl2vc = (struct l2vc_entry *)(pbucket->data);
        if (NULL == pl2vc)
        {
            continue;
        }

        /*
         * 1、pw 必须绑定才有选择隧道的权利
         * 2、对于 up 的 pw 不进行隧道重选
         * 3、该隧道必须与 pw 绑定的隧道相同
         * 4、pw 的 peer 与 lsp 隧道的 destination 必须同网段
         * 5、动态 pw 出标签不存在，不进行隧道选择
         */
        if ((PW_ALREADY_BIND_AC(&pl2vc->pwinfo)
                || PW_ALREADY_BIND_VSI(&pl2vc->pwinfo)
                || PW_ALREADY_BIND_MSPW(&pl2vc->pwinfo))
            && PW_ALREADY_CONFIG_LSP_TUNNEL(pl2vc)
            && (0 == memcmp(pl2vc->lsp_name, plsp->name, NAME_STRING_LEN))
            && (ipv4_is_same_subnet(plsp->destip.addr.ipv4, pl2vc->peerip.addr.ipv4,
                                    plsp->destip.prefixlen))
            && (pl2vc->pwinfo.admin_up == LINK_DOWN)
            && (pl2vc->outlabel != 0))
        {
            pl2vc->tunl_index       = plsp->lsp_index;
            pl2vc->pwinfo.nhp_index = plsp->lsp_index;
            pl2vc->pwinfo.nhp_type  = NHP_TYPE_LSP;

            pw_tunnel_up(&pl2vc->pwinfo);
        }

        if ((PW_ALREADY_BIND_AC(&pl2vc->pwinfo)
                || PW_ALREADY_BIND_VSI(&pl2vc->pwinfo)
                || PW_ALREADY_BIND_MSPW(&pl2vc->pwinfo))
            && (PW_ALREADY_CONFIG_SLSP_TUNNEL(pl2vc))
            && (ipv4_is_same_subnet(plsp->destip.addr.ipv4, pl2vc->peerip.addr.ipv4,
                                    plsp->destip.prefixlen))
            && (pl2vc->pwinfo.admin_up == LINK_DOWN)
            && (pl2vc->outlabel != 0))
        {
            pl2vc->pwinfo.nhp_index = plsp->lsp_index;
            pl2vc->pwinfo.nhp_type  = NHP_TYPE_LSP;

            pw_tunnel_up(&pl2vc->pwinfo);
        }
    }

    return;
}


/**
 * @brief      : pw 处理 lsp 隧道 down 事件
 * @param[in ] : lsp_name - lsp 隧道名称
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月1日 16:31:21
 * @note       :
 */
void l2vc_process_tunnel_static_lsp_down(struct static_lsp *plsp)
{
    struct hash_bucket *pbucket = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    int cursor;

    MPLS_LOG_DEBUG();

    if (NULL == plsp)
    {
        return;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
    {
        pl2vc = (struct l2vc_entry *)(pbucket->data);
        if (NULL == pl2vc)
        {
            continue;
        }

        if (PW_ALREADY_CONFIG_LSP_TUNNEL(pl2vc)
            && (0 == memcmp(pl2vc->lsp_name, plsp->name, NAME_STRING_LEN))
            && (pl2vc->pwinfo.admin_up == LINK_UP))
        {
            pl2vc->pwinfo.nhp_index = 0;
            pl2vc->pwinfo.nhp_type  = NHP_TYPE_LSP;

            pw_tunnel_down(&pl2vc->pwinfo);
        }

        if (PW_ALREADY_CONFIG_SLSP_TUNNEL(pl2vc)
            && (pl2vc->pwinfo.admin_up == LINK_UP)
            && (pl2vc->pwinfo.nhp_index == plsp->lsp_index))
        {
            pl2vc->pwinfo.nhp_index = 0;
            pl2vc->pwinfo.nhp_type  = NHP_TYPE_LSP;

            pw_tunnel_down(&pl2vc->pwinfo);
        }
    }

    return;
}


/**
 * @brief      : pw 处理 ldp lsp 隧道 up 事件
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 9:47:44
 * @note       :
 */
void l2vc_process_tunnel_ldp_lsp_add(void)
{
    struct hash_bucket *pbucket = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    int cursor;

    MPLS_LOG_DEBUG();

    HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
    {
        pl2vc = (struct l2vc_entry *)(pbucket->data);
        if (NULL == pl2vc)
        {
            continue;
        }

        if ((PW_ALREADY_BIND_AC(&pl2vc->pwinfo)
            || PW_ALREADY_BIND_VSI(&pl2vc->pwinfo)
            || PW_ALREADY_BIND_MSPW(&pl2vc->pwinfo))
                && PW_ALREADY_CONFIG_LDP_TUNNEL(pl2vc)
                && (pl2vc->pwinfo.admin_up == LINK_DOWN)
                && (pl2vc->outlabel != 0))
        {
            l2vc_select_tunnel_ldp_lsp(pl2vc);

            if (LINK_UP == pl2vc->pwinfo.admin_up)
            {
                pl2vc->pwinfo.admin_up = LINK_DOWN;

                pw_tunnel_up(&pl2vc->pwinfo);
            }
        }
    }

    return;
}


/**
 * @brief      : pw 处理 ldp lsp 隧道 down 事件
 * @param[in ] : lsp_index - ldp lsp 索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 9:50:14
 * @note       :
 */
void l2vc_process_tunnel_ldp_lsp_del(uint32_t lsp_index)
{
    struct hash_bucket *pbucket = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    int cursor;

    MPLS_LOG_DEBUG();

    HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
    {
        pl2vc = (struct l2vc_entry *)(pbucket->data);
        if (NULL == pl2vc)
        {
            continue;
        }

        if (PW_ALREADY_CONFIG_LDP_TUNNEL(pl2vc)
            && (pl2vc->pwinfo.nhp_index == lsp_index)
            && (pl2vc->pwinfo.admin_up == LINK_UP))
        {
            pl2vc->pwinfo.nhp_index = 0;
            pl2vc->pwinfo.nhp_type  = NHP_TYPE_LSP;

            pw_tunnel_down(&pl2vc->pwinfo);
        }
    }

    return;
}


/**
 * @brief      : ldp lsp 形成 frr 后更新引用的 lsp 索引
 * @param[in ] : lsp_index_old - 变更前使用的 lsp 索引
 * @param[in ] : lsp_index_new - 变更后使用的 lsp 索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年4月2日 14:29:09
 * @note       :
 */
void l2vc_process_tunnel_ldp_lsp_frr(uint32_t lsp_index_old, uint32_t lsp_index_new, uint32_t group_index)
{
    struct hash_bucket *pbucket = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    struct lsp_entry   *plspm   = NULL;
    int cursor;

    MPLS_LOG_DEBUG();

    HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
    {
        pl2vc = (struct l2vc_entry *)(pbucket->data);
        if (NULL == pl2vc)
        {
            continue;
        }

        if (PW_ALREADY_CONFIG_LDP_TUNNEL(pl2vc)
            && (pl2vc->pwinfo.nhp_index == lsp_index_old)
            && (pl2vc->pwinfo.admin_up == LINK_UP))
        {
            pl2vc->pwinfo.nhp_index = lsp_index_new;

            plspm = mpls_lsp_lookup(pl2vc->pwinfo.ingress_lsp);
            if (NULL != plspm)
            {
                plspm->nhp_index   = lsp_index_new;
                plspm->group_index = group_index;

                mpls_lsp_update(plspm, LSP_SUBTYPE_TUNNEL);
            }
        }
    }

    return;
}


/**
 * @brief      : 动态 pw 添加
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 9:51:54
 * @note       :
 */
int l2vc_add_martini(struct l2vc_entry *pl2vc)
{
    MPLS_LOG_DEBUG();

    if (NULL == pl2vc)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    /* 分配入标签 */
    if (pl2vc->inlabel == 0)
    {
        pl2vc->inlabel = label_alloc(MODULE_ID_LDP);
    }

    /* 创建 remote ldp session */
    pw_join_ldp(pl2vc);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 动态 pw 删除
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 9:54:01
 * @note       :
 */
int l2vc_delete_martini(struct l2vc_entry *pl2vc)
{
    if (NULL == pl2vc)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

     MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_PW, "pw %s vcid %d outlabel %d.\n", pl2vc->name, pl2vc->pwinfo.vcid, pl2vc->outlabel);

    /* 删除 remote ldp session */
    pw_leave_ldp(pl2vc);

    /* 释放入标签 */
    if (0 != pl2vc->inlabel)
    {
        label_free(pl2vc->inlabel);

        pl2vc->inlabel = 0;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 动态 pw 参数协商和出标签添加
 * @param[in ] : pl2vc_local - 本地 l2vc 结构
 * @param[in ] : pl2vc       - 远端 l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:00:35
 * @note       :
 */
int l2vc_add_label(struct ldp_pwinfo *pldp_pwinfo)
{
    struct l2vc_entry *pl2vc_local  = pldp_pwinfo->local_pwindex;
    struct l2vc_entry *pl2vc_peer   = &pldp_pwinfo->peer_pwinfo;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_PW, "pl2vc %s\n", pl2vc_local->name);

    if(pldp_pwinfo->reconnect_num > MPLS_MARTINI_PW_RECONNECT_NUM_MAX)
    {
        pldp_pwinfo->reconnect_num ++;
        MPLS_LDP_ERROR("pl2vc %s reconnet %d failed more than 3 times\n", pl2vc_local->name, pldp_pwinfo->reconnect_num);
    }
    /* 动态 pw 参数协商 */
    if ((pl2vc_peer->pwinfo.vcid != pl2vc_local->pwinfo.vcid)
        || (pl2vc_peer->pwinfo.mtu != pl2vc_local->pwinfo.mtu)
        || (pl2vc_peer->pwinfo.ctrlword_flag != pl2vc_local->pwinfo.ctrlword_flag))
    {
        snprintf(pldp_pwinfo->reason, 128, "negotiate failed for vcid mtu or ctrlword not match\n");
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_PW, " pw %s negotiate failed for vcid mtu or ctrlword not match\n", pl2vc_local->name);
        pldp_pwinfo->state = MPSL_MARTINI_PW_DOWN;
        return ERRNO_FAIL;
    }
    pldp_pwinfo->state = MPLS_MARTINI_PW_UP;
    snprintf(pldp_pwinfo->reason, 128, "negotiate successful\n");
    pldp_pwinfo->reconnect_num = 0;
    
    pl2vc_local->outlabel = pl2vc_peer->inlabel;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_PW, "pw %s vcid %d outlabel %d.\n", pl2vc_local->name, pl2vc_local->pwinfo.vcid, pl2vc_local->outlabel);

    /* 入出标签都有之后，选择公网隧道 */
    if ((pl2vc_local->inlabel > 0) && (pl2vc_local->outlabel > 0))
    {
        l2vc_select_tunnel(pl2vc_local);
        if (LINK_UP == pl2vc_local->pwinfo.admin_up)
        {
            /* pw_tunnel_up 接口中需要 pw 管理状态为 down，所以先置 down */
            pl2vc_local->pwinfo.admin_up = LINK_DOWN;

            pw_tunnel_up(&pl2vc_local->pwinfo);
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 动态 pw 标签撤销
 * @param[in ] : pl2vc - 本地 l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:03:07
 * @note       :
 */
int l2vc_delete_label(struct l2vc_entry *pl2vc)
{
    if (NULL == pl2vc)
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_PW, "pl2vc %s\n", pl2vc->name);
    /* 出标签清除在 down 接口之前，保证 pw down 后不会重选隧道 */
    pl2vc->outlabel = 0;

    if (LINK_UP == pl2vc->pwinfo.admin_up)
    {
        pw_tunnel_down(&pl2vc->pwinfo);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 告警上报
 * @param[in ] : opcode  - 告警上报或清除
 * @param[in ] : pwindex - 上报告警的 pw 索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:04:56
 * @note       :
 */
void l2vc_alarm_process(enum OPCODE_E opcode, uint32_t pwindex)
{
    MPLS_LOG_DEBUG();

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
	gPortInfo.iIfindex = 0;
	gPortInfo.iMsgPara1 = pwindex;

    if (OPCODE_UP == opcode)
    {
        //ipran_alarm_report(IFM_PW_2_TYPE, 0, 0, pwindex, 0, GPN_ALM_TYPE_PW_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
        ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PW_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
    }
    else if (OPCODE_DOWN == opcode)
    {
        //ipran_alarm_report(IFM_PW_2_TYPE, 0, 0, pwindex, 0, GPN_ALM_TYPE_PW_DOWN, GPN_SOCK_MSG_OPT_RISE);
        ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PW_DOWN, GPN_SOCK_MSG_OPT_RISE);
    }

//    alarm_report(opcode_send, MODULE_ID_MPLS, ALM_CODE_RAN_PW_DOWN, pwindex, 0, ALM_STYPE_RANLSP, 0);
}


/**
 * @brief      : 计算 pw 哈希表 key 值
 * @param[in ] : hash_key - pw 索引
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:05:49
 * @note       : pw 索引等同于 vc-id
 */
static unsigned int pw_compute(void *hash_key)
{
    return (uint32_t)hash_key;
}


/**
 * @brief      : 比较 pw 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - pw 索引
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:07:48
 * @note       :
 */
static int pw_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;

    if (pbucket->hash_key == hash_key)
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : pw 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:08:55
 * @note       :
 */
void pw_table_init(uint32_t size)
{
    hios_hash_init(&pw_table, size, pw_compute, pw_compare);

    return;
}


/**
 * @brief      : pw 添加
 * @param[in ] : ppwinfo - pw 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:09:18
 * @note       :
 */
int pw_add(struct pw_info *ppwinfo)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (ppwinfo == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (pw_table.hash_size == pw_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (pitem == NULL)
    {
        MPLS_LOG_ERROR("malloc\n");

        return ERRNO_MALLOC;
    }

    pitem->hash_key = (void *)(ppwinfo->pwindex);
    pitem->data     = ppwinfo;

    hios_hash_add(&pw_table, pitem);
    //ipran_alarm_port_register(IFM_PW_2_TYPE, 0, 0, ppwinfo->pwindex, 0);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
	gPortInfo.iMsgPara1 = ppwinfo->pwindex;
	ipran_alarm_port_register(&gPortInfo);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 删除
 * @param[in ] : pwindex - pw 索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:10:47
 * @note       :
 */
int pw_delete(uint32_t pwindex)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    pitem = hios_hash_find(&pw_table, (void *)pwindex);
    if (pitem == NULL)
    {
        return ERRNO_SUCCESS;
    }

    //ipran_alarm_port_unregister(IFM_PW_2_TYPE, 0, 0, pwindex, 0);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
	gPortInfo.iMsgPara1 = pwindex;
	ipran_alarm_port_unregister(&gPortInfo);

	hios_hash_delete(&pw_table, pitem);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 查找
 * @param[in ] : pwindex - pw 索引
 * @param[out] :
 * @return     : 成功返回 pw 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:11:27
 * @note       :
 */
struct pw_info *pw_lookup(uint32_t pwindex)
{
    struct hash_bucket *pbucket = NULL;

    MPLS_LOG_DEBUG();

    pbucket = hios_hash_find(&pw_table, (void *)pwindex);
    if (pbucket == NULL)
    {
        return NULL;
    }

    return ((struct pw_info *)(pbucket->data));
}


/**
 * @brief      : 获取 pw 生成的私网 lsp 索引
 * @param[in ] : ppwinfo - pw 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:12:30
 * @note       :
 */
int pw_get_lsp_index(struct pw_info *ppwinfo)
{
    uint32_t lsp_index1 = 0;
    uint32_t lsp_index2 = 0;

    lsp_index1 = mpls_lsp_alloc_index();
    if (0 == lsp_index1)
    {
        return ERRNO_INDEX_ALLOC;
    }

    lsp_index2 = mpls_lsp_alloc_index();
    if (0 == lsp_index2)
    {
        mpls_lsp_free_index(lsp_index1);

        return ERRNO_INDEX_ALLOC;
    }

    ppwinfo->ingress_lsp = lsp_index1;
    ppwinfo->egress_lsp  = lsp_index2;

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 释放 pw 生成的私网 lsp 索引
 * @param[in ] : ppwinfo - pw 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:13:58
 * @note       :
 */
int pw_free_lsp_index(struct pw_info *ppwinfo)
{
    MPLS_LOG_DEBUG();
    if (ppwinfo->ingress_lsp != 0)
    {
        mpls_lsp_free_index(ppwinfo->ingress_lsp);

        ppwinfo->ingress_lsp = 0;
    }

    if (ppwinfo->egress_lsp != 0)
    {
        mpls_lsp_free_index(ppwinfo->egress_lsp);

        ppwinfo->egress_lsp = 0;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 添加私网 lsp
 * @param[in ] : pl2vc - l2vc 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:14:53
 * @note       :
 */
int pw_add_lsp(struct l2vc_entry *pl2vc)
{
    struct pw_info   *ppwinfo      = NULL;
    struct lsp_entry *plsp_ingress = NULL;
    struct lsp_entry *plsp_egress  = NULL;
    int ret;

    MPLS_LOG_DEBUG();

    if (pl2vc == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plsp_ingress = mpls_lsp_create();
    if (plsp_ingress == NULL)
    {
        return ERRNO_MALLOC;
    }

    plsp_egress = mpls_lsp_create();
    if (plsp_egress == NULL)
    {
        ret = ERRNO_MALLOC;

        goto fail;
    }

    ppwinfo = &pl2vc->pwinfo;

    plsp_ingress->lsp_index     = ppwinfo->ingress_lsp;
    plsp_ingress->ac_index      = ppwinfo->ifindex;
    plsp_ingress->pw_index      = ppwinfo->pwindex;
    plsp_ingress->lsp_type      = LSP_TYPE_L2VC;
    plsp_ingress->direction     = LSP_DIRECTION_INGRESS;
    plsp_ingress->outlabel      = pl2vc->outlabel;
    plsp_ingress->nhp_type      = ppwinfo->nhp_type;
    plsp_ingress->nhp_index     = ppwinfo->nhp_index;
    plsp_ingress->vpnid         = ppwinfo->vpls.vsi_id;
    plsp_ingress->phb_enable    = pl2vc->phb_enable;
    plsp_ingress->phb_id        = pl2vc->phb_id;
    plsp_ingress->domain_id     = pl2vc->domain_id;
    plsp_ingress->hqos_id       = pl2vc->hqos_id;
    plsp_ingress->car_cir[0]    = pl2vc->car_cir[0];
    plsp_ingress->car_cir[1]    = pl2vc->car_cir[1];
    plsp_ingress->car_pir[0]    = pl2vc->car_pir[0];
    plsp_ingress->car_pir[1]    = pl2vc->car_pir[1];
    plsp_ingress->mplsoam_id    = pl2vc->mplsoam_id;
    plsp_ingress->bfd_id        = pl2vc->bfd_id;
    plsp_ingress->statis_enable = pl2vc->statis_enable;
    plsp_ingress->pwlist        = NULL;

    ret = mpls_lsp_add(plsp_ingress);
    if ((ERRNO_SUCCESS != ret) && (ERRNO_EXISTED != ret))
    {
        MPLS_LOG_ERROR("errcode = %d\n", ret);

        goto fail;
    }

    /* 填充 egress lsp */
    plsp_egress->lsp_index     = ppwinfo->egress_lsp;
    plsp_egress->ac_index      = ppwinfo->ifindex;
    plsp_egress->pw_index      = ppwinfo->pwindex;
    plsp_egress->lsp_type      = LSP_TYPE_L2VC;
    plsp_egress->direction     = LSP_DIRECTION_EGRESS;
    plsp_egress->vpnid         = ppwinfo->vpls.vsi_id;
    plsp_egress->nhp_type      = ppwinfo->nhp_type;
    plsp_egress->nhp_index     = ppwinfo->nhp_index;
    plsp_egress->inlabel       = pl2vc->inlabel;
    plsp_egress->phb_enable    = pl2vc->phb_enable;
    plsp_egress->phb_id        = pl2vc->phb_id;
    plsp_egress->domain_id     = pl2vc->domain_id;
    plsp_egress->hqos_id       = pl2vc->hqos_id;
    plsp_egress->car_cir[0]    = pl2vc->car_cir[0];
    plsp_egress->car_cir[1]    = pl2vc->car_cir[1];
    plsp_egress->car_pir[0]    = pl2vc->car_pir[0];
    plsp_egress->car_pir[1]    = pl2vc->car_pir[1];
    plsp_egress->mplsoam_id    = pl2vc->mplsoam_id;
    plsp_egress->bfd_id        = pl2vc->bfd_id;
    plsp_egress->statis_enable = pl2vc->statis_enable;
    plsp_egress->pwlist        = NULL;

    ret = mpls_lsp_add(plsp_egress);
    if ((ERRNO_SUCCESS != ret) && (ERRNO_EXISTED != ret))
    {
        MPLS_LOG_ERROR("errcode = %d\n", ret);

        goto fail;
    }

    return ERRNO_SUCCESS;

fail:

    mpls_lsp_free(plsp_ingress);
    mpls_lsp_free(plsp_egress);

    return ret;
}


/**
 * @brief      : 分段 pw 时，添加两条 transit lsp
 * @param[in ] : pl2vc        - 分段 pw 前段
 * @param[in ] : pl2vc_switch - 分段 pw 后段
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:17:06
 * @note       :
 */
int pw_add_lsp_transit(struct l2vc_entry *pl2vc, struct l2vc_entry *pl2vc_switch)
{
    struct pw_info   *ppwinfo1 = NULL;
    struct pw_info   *ppwinfo2 = NULL;
    struct lsp_entry *plsp1    = NULL;
    struct lsp_entry *plsp2    = NULL;
    int ret;

    MPLS_LOG_DEBUG();

    if ((NULL == pl2vc) || (NULL == pl2vc_switch))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    ppwinfo1 = &pl2vc->pwinfo;
    ppwinfo2 = &pl2vc_switch->pwinfo;

    if ((LINK_UP == pl2vc->pwinfo.admin_up)
        && (pl2vc->outlabel !=0)
        && (pl2vc_switch->inlabel != 0))
    {
        MPLS_LOG_DEBUG("Add pw index = %d\n", ppwinfo1->pwindex);

        plsp1 = mpls_lsp_lookup(ppwinfo1->ingress_lsp);
        if (NULL != plsp1)
        {
            goto next;
        }

        plsp1 = mpls_lsp_create();
        if (plsp1 == NULL)
        {
            return ERRNO_MALLOC;
        }

        /* 填充私网 lsp1, pw1 指定的 tunnel 对应 pw1 的 outlabel */
        plsp1->lsp_index     = ppwinfo1->ingress_lsp;
        plsp1->ac_index      = ppwinfo1->ifindex;
        plsp1->pw_index      = ppwinfo1->pwindex;
        plsp1->lsp_type      = LSP_TYPE_L2VC;
        plsp1->direction     = LSP_DIRECTION_TRANSIT;
        plsp1->inlabel       = pl2vc_switch->inlabel;
        plsp1->outlabel      = pl2vc->outlabel;
        plsp1->nhp_type      = ppwinfo1->nhp_type;
        plsp1->nhp_index     = ppwinfo1->nhp_index;
        plsp1->vpnid         = ppwinfo1->vpls.vsi_id;
        plsp1->phb_enable    = pl2vc->phb_enable;
        plsp1->phb_id        = pl2vc->phb_id;
        plsp1->domain_id     = pl2vc->domain_id;
        plsp1->hqos_id       = pl2vc->hqos_id;
        plsp1->car_cir[0]    = pl2vc->car_cir[0];
        plsp1->car_cir[1]    = pl2vc->car_cir[1];
        plsp1->car_pir[0]    = pl2vc->car_pir[0];
        plsp1->car_pir[1]    = pl2vc->car_pir[1];
        plsp1->mplsoam_id    = pl2vc->mplsoam_id;
        plsp1->bfd_id        = pl2vc->bfd_id;
        plsp1->statis_enable = pl2vc->statis_enable;
        plsp1->pwlist        = NULL;

        ret = mpls_lsp_add(plsp1);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("errcode = %d\n", ret);

            mpls_lsp_free(plsp1);

            return ret;
        }
    }

next:

    if ((LINK_UP == pl2vc_switch->pwinfo.admin_up)
        && (pl2vc_switch->outlabel != 0)
        && (pl2vc->inlabel != 0))
    {
        MPLS_LOG_DEBUG("Add pw index = %d\n", ppwinfo2->pwindex);

        plsp1 = mpls_lsp_lookup(ppwinfo1->egress_lsp);
        if (NULL != plsp1)
        {
            return ERRNO_SUCCESS;
        }

        plsp2 = mpls_lsp_create();
        if (plsp2 == NULL)
        {
            return ERRNO_MALLOC;
        }

        /* 填充私网 lsp2, pw2 指定的 tunnel 对应 pw2 的 outlabel */
        plsp2->lsp_index     = ppwinfo1->egress_lsp;
        plsp2->ac_index      = ppwinfo2->ifindex;
        plsp2->pw_index      = ppwinfo2->pwindex;
        plsp2->lsp_type      = LSP_TYPE_L2VC;
        plsp2->direction     = LSP_DIRECTION_TRANSIT;
        plsp2->inlabel       = pl2vc->inlabel;
        plsp2->outlabel      = pl2vc_switch->outlabel;
        plsp2->nhp_type      = ppwinfo2->nhp_type;
        plsp2->nhp_index     = ppwinfo2->nhp_index;
        plsp2->vpnid         = ppwinfo2->vpls.vsi_id;
        plsp2->phb_enable    = pl2vc_switch->phb_enable;
        plsp2->phb_id        = pl2vc_switch->phb_id;
        plsp2->domain_id     = pl2vc_switch->domain_id;
        plsp2->hqos_id       = pl2vc_switch->hqos_id;
        plsp2->car_cir[0]    = pl2vc->car_cir[0];
        plsp2->car_cir[1]    = pl2vc->car_cir[1];
        plsp2->car_pir[0]    = pl2vc->car_pir[0];
        plsp2->car_pir[1]    = pl2vc->car_pir[1];
        plsp2->mplsoam_id    = pl2vc_switch->mplsoam_id;
        plsp2->bfd_id        = pl2vc_switch->bfd_id;
        plsp2->statis_enable = pl2vc_switch->statis_enable;
        plsp2->pwlist        = NULL;

        ret = mpls_lsp_add(plsp2);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("errcode = %d\n", ret);

            mpls_lsp_free(plsp2);

            return ret;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 删除私网 lsp
 * @param[in ] : ppwinfo - pw 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:19:34
 * @note       :
 */
int pw_delete_lsp(struct pw_info *ppwinfo)
{
    MPLS_LOG_DEBUG();

    if (0 != ppwinfo->mspw_index)
    {
        /* 多段 pw 只删自己的入方向 lsp */
        mpls_lsp_delete(ppwinfo->ingress_lsp);
    }
    else
    {
        if (0 != ppwinfo->egress_lsp)
        {
            mpls_lsp_delete(ppwinfo->egress_lsp);
        }

        if (0 != ppwinfo->ingress_lsp)
        {
           mpls_lsp_delete(ppwinfo->ingress_lsp);
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 默认切换处理
 * @param[in ] : pif - pw ac 接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:20:39
 * @note       :
 */
static int pw_set_pw_status_default(struct mpls_if *pif)
{
    enum BACKUP_E pw_backup = BACKUP_STATUS_MASTER;
    int ret;

    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    /* 备存在情况下，只有主 down 备 up 才切换至备 */
    if (NULL != pif->ppw_slave)
    {
        if ((LINK_UP == pif->ppw_slave->pwinfo.up_flag)
            &&(ENABLE == pif->ppw_slave->pwinfo.status)
            &&( (LINK_DOWN == pif->ppw_master->pwinfo.up_flag)
            ||(DISABLE == pif->ppw_master->pwinfo.status)))
        {
            pw_backup = BACKUP_STATUS_SLAVE;
        }
    }

    /* 状态发生变化，切换 */
    if (pif->pw_backup != pw_backup)
    {
        pif->pw_backup = pw_backup;
        if (NULL != pif->ppw_master)
        {
            pif->ppw_master->pw_backup = pw_backup;

            ret = mpls_pw_mplsif_download(pif->ppw_master->pwinfo.pwindex, &pif->pw_backup,
                                        MPLSIF_INFO_PW_BACKUP, OPCODE_UPDATE);
            if (ERRNO_SUCCESS != ret)
            {
                MPLS_LOG_ERROR("IPC send pw status\n");
            }
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 主备 pw 不回切处理
 * @param[in ] : pif - pw ac 接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:22:27
 * @note       :
 */
static int pw_set_pw_status_non_failback(struct mpls_if *pif)
{
    enum BACKUP_E pw_backup = BACKUP_STATUS_MASTER;
    int ret;

    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    /* pw 备 up，主 down，或者当前工作在备，则工作在备，否则工作在主 */
    if( (LINK_UP == pif->ppw_slave->pwinfo.up_flag)
        &&(ENABLE == pif->ppw_slave->pwinfo.status))
    {
        if( (LINK_DOWN == pif->ppw_master->pwinfo.up_flag)
            ||(DISABLE == pif->ppw_master->pwinfo.status))
        {
            pw_backup = BACKUP_STATUS_SLAVE;
        }
        else
        {
            if (pif->pw_backup == BACKUP_STATUS_SLAVE)
            {
                pw_backup = BACKUP_STATUS_SLAVE;
            }
        }
    }

    /* 状态发生变化，切换 */
    if (pif->pw_backup != pw_backup)
    {
        pif->pw_backup = pw_backup;
        pif->ppw_master->pw_backup = pw_backup;

        ret = mpls_pw_mplsif_download(pif->ppw_master->pwinfo.pwindex, &pif->pw_backup,
                                    MPLSIF_INFO_PW_BACKUP, OPCODE_UPDATE);
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
 * @date       : 2018年3月2日 10:26:40
 * @note       : 回切事件一定是备切换至主
 */
static int pw_set_pw_status_callback(void *para)
{
    struct mpls_if *pif = (struct mpls_if *)para;
    int ret;

    MPLS_LOG_DEBUG();

    if (NULL == pif)
    {
        return ERRNO_FAIL;
    }

    /* 定时器超时置 NULL */
    if (NULL != pif->ppw_master)
    {
        pif->ppw_master->pwtr_timer = 0;
    }

    /* 状态发生变化，切换 */
    pif->pw_backup             = BACKUP_STATUS_MASTER;
    pif->ppw_master->pw_backup = BACKUP_STATUS_MASTER;

    ret = mpls_pw_mplsif_download(pif->ppw_master->pwinfo.pwindex, &pif->pw_backup,
                                MPLSIF_INFO_PW_BACKUP, OPCODE_UPDATE);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("IPC send pw status\n");
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 处理 pw 主备切换事件
 * @param[in ] : pif - pw ac 接口结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:30:48
 * @note       :
 */
int pw_set_pw_status(struct mpls_if *pif)
{
    MPLS_LOG_DEBUG();

    if ((NULL == pif) || (NULL == pif->ppw_master))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (PW_FAILBACK_IS_VALID(pif))
    {
        if (pif->ppw_master->failback == FAILBACK_ENABLE)
        {
            if ((pif->pw_backup == BACKUP_STATUS_SLAVE)
                && (pif->ppw_master->pwinfo.up_flag == LINK_UP)
                &&(pif->ppw_master->pwinfo.status == ENABLE)
                && (pif->ppw_slave->pwinfo.up_flag == LINK_UP)
                &&(pif->ppw_slave->pwinfo.status == ENABLE))
            {
                MPLS_TIMER_DEL(pif->ppw_master->pwtr_timer);
                pif->ppw_master->pwtr_timer = MPLS_TIMER_ADD(pw_set_pw_status_callback, pif, pif->ppw_master->wtr);

                return ERRNO_SUCCESS;
            }
        }
        else if (pif->ppw_master->failback == FAILBACK_DISABLE)
        {
            MPLS_TIMER_DEL(pif->ppw_master->pwtr_timer);
            pw_set_pw_status_non_failback(pif);

            return ERRNO_SUCCESS;
        }
    }

    if ((NULL != pif->ppw_master))
    {
        MPLS_TIMER_DEL(pif->ppw_master->pwtr_timer);
    }

    pw_set_pw_status_default(pif);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : pw 管理 down
 * @param[in ] : ppwinfo - pw 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:34:01
 * @note       : pw 管理 down 则意味着 pw 选中的隧道 down 掉
 */
void pw_tunnel_down(struct pw_info *ppwinfo)
{
    struct mpls_if *pif = NULL;
    int ret = ERRNO_NOT_FOUND;

    MPLS_LOG_DEBUG();

    if (LINK_DOWN == ppwinfo->admin_up)
    {
        return;
    }

    ppwinfo->admin_up  = LINK_DOWN;
    ppwinfo->nhp_index = 0;

    if (ppwinfo->up_flag == LINK_UP)
    {
        ppwinfo->up_flag = LINK_DOWN;
        gmpls.pw_up_num--;

        l2vc_alarm_process(OPCODE_DOWN, ppwinfo->pwindex);
    }

    pw_delete_lsp(ppwinfo);

    if (0 != ppwinfo->vpls.vsi_id)
    {
        vsi_set_pw_status(pw_get_l2vc(ppwinfo));

        return;
    }

    if (0 == ppwinfo->ifindex)
    {
        return;
    }

    pif = mpls_if_lookup(ppwinfo->ifindex);
    if (NULL == pif)
    {
        return;
    }

    if (0 != pif->aps_sessid)
    {
        if (pif->ppw_master->pwinfo.pwindex == ppwinfo->pwindex)
        {
            ret = mplsaps_state_update(pif->aps_sessid, APS_OAM_STATE_SF_DOWN_W);
        }
        else if (pif->ppw_slave->pwinfo.pwindex == ppwinfo->pwindex)
        {
            ret = mplsaps_state_update(pif->aps_sessid, APS_OAM_STATE_SF_DOWN_P);
        }
    }
    
    if(ERRNO_NOT_FOUND == ret)
    {
        pw_set_pw_status(pif);
    }

    l2vc_select_tunnel_new(ppwinfo);

    return;
}


/**
 * @brief      : pw 管理 up
 * @param[in ] : ppwinfo - pw 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:36:50
 * @note       : pw 管理 up 意味着 pw 选中隧道
 */
void pw_tunnel_up(struct pw_info *ppwinfo)
{
    struct l2vc_entry *pl2vc = NULL;
    struct mpls_if    *pif   = NULL;
    int ret = ERRNO_NOT_FOUND;

    MPLS_LOG_DEBUG();

    if (LINK_UP == ppwinfo->admin_up)
    {
        return;
    }

    ppwinfo->admin_up = LINK_UP;

    if (ppwinfo->up_flag == LINK_DOWN)
    {
        ppwinfo->up_flag = LINK_UP;
        gmpls.pw_up_num++;
    }

    pl2vc = pw_get_l2vc(ppwinfo);
    if (0 != ppwinfo->mspw_index)
    {
        pw_add_lsp_transit(pl2vc, pl2vc->pswitch_pw);
    }
    else
    {
        pw_add_lsp(pl2vc);
    }

    l2vc_alarm_process(OPCODE_UP, ppwinfo->pwindex);

    if (0 != ppwinfo->vpls.vsi_id)
    {
        vsi_set_pw_status(pl2vc);

        return;
    }

    if (0 == ppwinfo->ifindex)
    {
        return;
    }

    pif = mpls_if_lookup(ppwinfo->ifindex);
    if (NULL == pif)
    {
        return;
    }

    if (0 != pif->aps_sessid)
    {
        if (pif->ppw_master->pwinfo.pwindex == ppwinfo->pwindex)
        {
            ret = mplsaps_state_update(pif->aps_sessid, APS_OAM_STATE_SF_UP_W);
        }
        else if (pif->ppw_slave->pwinfo.pwindex == ppwinfo->pwindex)
        {
            ret = mplsaps_state_update(pif->aps_sessid, APS_OAM_STATE_SF_UP_P);
        }
    }
    
    if(ERRNO_NOT_FOUND == ret)
    {
        pw_set_pw_status(pif);
    }

    return;
}


/**
 * @brief      : pw link down
 * @param[in ] : ppwinfo - pw 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:40:19
 * @note       : pw link down 意味着检测机制上报 down 事件
 */
void pw_detection_protocal_down(struct pw_info *ppwinfo)
{
    struct mpls_if *pif = NULL;
    int ret = ERRNO_NOT_FOUND;
    MPLS_LOG_DEBUG();

    if (DISABLE == ppwinfo->status)
    {
        return;
    }

    ppwinfo->status = DISABLE;
    
#if 0
    gmpls.pw_up_num--;

    mpls_pw_status_download(ppwinfo, PW_SUBTYPE_DOWN);

    l2vc_alarm_process(OPCODE_DOWN, ppwinfo->pwindex);

    if (0 != ppwinfo->vpls.vsi_id)
    {
        vsi_set_pw_status(pw_get_l2vc(ppwinfo));

        return;
    }
    
#endif

    if (0 == ppwinfo->ifindex)
    {
        return;
    }

    pif = mpls_if_lookup(ppwinfo->ifindex);
    if (NULL == pif)
    {
        return;
    }

    if (0 != pif->aps_sessid)
    {
        if (pif->ppw_master->pwinfo.pwindex == ppwinfo->pwindex)
        {
            ret = mplsaps_state_update(pif->aps_sessid,APS_OAM_STATE_SF_DOWN_W);
        }
        else if (pif->ppw_slave->pwinfo.pwindex == ppwinfo->pwindex)
        {
            ret = mplsaps_state_update(pif->aps_sessid,APS_OAM_STATE_SF_DOWN_P);
        }
    }
    
    if(ERRNO_NOT_FOUND == ret)
    {
        pw_set_pw_status(pif);
    }

    return;
}


/**
 * @brief      : pw link up
 * @param[in ] : ppwinfo - pw 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:41:54
 * @note       : pw link up 意味着检测机制上报 up 事件
 */
void pw_detection_protocal_up(struct pw_info *ppwinfo)
{
    struct mpls_if *pif = NULL;
    int ret = ERRNO_NOT_FOUND;
    MPLS_LOG_DEBUG();

    if (ENABLE == ppwinfo->status)
    {
        return;
    }

    ppwinfo->status = ENABLE;
    
#if 0
    gmpls.pw_up_num++;

    mpls_pw_status_download(ppwinfo, PW_SUBTYPE_UP);

    l2vc_alarm_process(OPCODE_UP, ppwinfo->pwindex);

    if (0 != ppwinfo->vpls.vsi_id)
    {
        vsi_set_pw_status(pw_get_l2vc(ppwinfo));

        return;
    }
#endif

    if (0 == ppwinfo->ifindex)
    {
        return;
    }

    pif = mpls_if_lookup(ppwinfo->ifindex);
    if (NULL == pif)
    {
        return;
    }

    if (0 != pif->aps_sessid)
    {
        if (pif->ppw_master->pwinfo.pwindex == ppwinfo->pwindex)
        {
            ret = mplsaps_state_update(pif->aps_sessid, APS_OAM_STATE_SF_UP_W);
        }
        else if (pif->ppw_slave->pwinfo.pwindex == ppwinfo->pwindex)
        {
            ret = mplsaps_state_update(pif->aps_sessid, APS_OAM_STATE_SF_UP_P);
        }
    }
    if(ERRNO_NOT_FOUND == ret)
    {
        pw_set_pw_status(pif);
    }

    return;
}


/**
 * @brief      : 通过 pw 结构获取 l2vc 结构
 * @param[in ] : ppwinfo - pw 结构
 * @param[out] :
 * @return     : 成功返回 l2vc 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:47:30
 * @note       : 传入的 pw 结构必须是哈希表中获取来的，通过地址偏移计算获得 l2vc
 */
struct l2vc_entry *pw_get_l2vc(struct pw_info *ppwinfo)
{
    uchar *p = NULL;
    int    offset;

    MPLS_LOG_DEBUG();

    if (NULL == ppwinfo)
    {
        return NULL;
    }

    offset = sizeof(struct l2vc_entry) - sizeof(struct pw_info);

    p = (uchar *)ppwinfo - offset; // 地址向前偏移

    return (struct l2vc_entry *)p;
}


/**
 * @brief      : 通过 pw 索引获取 pw 名称
 * @param[in ] : pwindex - pw 索引
 * @param[out] :
 * @return     : 成功返回 pw 名称，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:49:53
 * @note       :
 */
uchar *pw_get_name(uint32_t pwindex)
{
    struct l2vc_entry *pl2vc   = NULL;
    struct pw_info    *ppwinfo = NULL;

    ppwinfo = pw_lookup(pwindex);
    if (NULL == ppwinfo)
    {
        return NULL;
    }

    pl2vc = pw_get_l2vc(ppwinfo);
    if (NULL == pl2vc)
    {
        return NULL;
    }

    return pl2vc->name;
}


/**
 * @brief      : 通过 pw 名称获取 pw 索引
 * @param[in ] : pname - pw 名称
 * @param[out] :
 * @return     : 成功返回 pw 索引，失败返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月2日 10:51:19
 * @note       :
 */
uint32_t pw_get_index(uchar *pname)
{
    struct l2vc_entry *pl2vc = NULL;

    pl2vc = l2vc_lookup(pname);
    if (NULL == pl2vc)
    {
        /* 0 为无效索引 */
        return 0;
    }

    return pl2vc->pwinfo.pwindex;
}


static int pw_get_hqos_snmp(uchar *pname, struct hqos_t *phqos, struct hqos_pw *phqos_snmp)
{
    if (NULL == phqos || NULL == phqos_snmp || NULL == pname)
    {
        MPLS_LOG_ERROR("Input param check error in func\n");

        return ERRNO_PARAM_ILLEGAL;
    }

    memset(phqos_snmp, 0, sizeof(struct hqos_pw));
    memcpy(phqos_snmp->name, pname, NAME_STRING_LEN);
    memcpy (&(phqos_snmp->hqos), phqos, sizeof(struct hqos_t));

    return ERRNO_SUCCESS;
}



/**
 * @brief      : 批量获取 l2vc 结构
 * @param[in ] : pname     - pw 名称
 * @param[in ] : data_len  - pw 名称长度
 * @param[out] : l2vc_buff - 获取到的 l2vc 结构数组
 * @return     : 返回获取到的 l2vc 数量
 * @author     : ZhangFj
 * @date       : 2018年3月1日 10:34:03
 * @note       : 提供给 snmp 的 get next 接口，空索引从数据结构开始返回数据，
 * @note       : 非空索引从传入索引节点下一个节点开始返回数据
 */
int l2vc_get_bulk(uchar *pname, int data_len, struct l2vc_entry l2vc_buff[])
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct l2vc_entry);
    int cursor;
    uint32_t val = 0;

    if (0 == data_len)
    {
        /* 初次获取 */
        HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
        {
            pl2vc = (struct l2vc_entry *)pbucket->data;
            if (NULL == pl2vc)
            {
                continue;
            }

            memcpy(&l2vc_buff[data_num++], pl2vc, sizeof(struct l2vc_entry));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&l2vc_table, (void *)pname);
        if (NULL == pbucket)
        {
            /*
             * 当前数据可能已经被删除，返回到当前哈希桶的链表头部
             * 如果当前哈希桶链表头为空，向后面的哈希桶继续查找
             */
            val = l2vc_table.compute_hash((void *)pname)%HASHTAB_SIZE;

            if (NULL != l2vc_table.buckets[val])
            {
                pbucket = l2vc_table.buckets[val];
            }
            else
            {
                for (++val; val<HASHTAB_SIZE; ++val)
                {
                    if (NULL != l2vc_table.buckets[val])
                    {
                        pbucket = l2vc_table.buckets[val];
                        break;
                    }
                }
            }
        }

        if (NULL != pbucket)
        {
            for (data_num=0; data_num<msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&l2vc_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                pl2vc = pnext->data;
                memcpy(&l2vc_buff[data_num], pl2vc, sizeof(struct l2vc_entry));
                pbucket = pnext;
            }
        }
    }

    return data_num;
}

int pw_get_hqos_bulk(uchar *pname, int data_len, struct hqos_pw *hqos_array)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct l2vc_entry  *pl2vc   = NULL;
    struct hqos_t      *phqos   = NULL;
    struct hqos_pw      hqos_snmp;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct hqos_t);
    uint32_t val = 0;
    int cursor;

    if (NULL == pname)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (0 == data_len)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
        {
            pl2vc = (struct l2vc_entry *)pbucket->data;
            if ( (NULL == pl2vc) ||
                 (0 == pl2vc->hqos_id))
            {
                continue;
            }

            phqos = qos_hqos_get(pl2vc->hqos_id, MODULE_ID_MPLS);
            if(NULL == phqos)
            {
                continue;
            }

            if(pw_get_hqos_snmp(pl2vc->name, phqos, &hqos_snmp))
            {
                continue;
            }

            memcpy(&hqos_array[data_num++], &hqos_snmp, sizeof(struct hqos_pw));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&l2vc_table, (void *)pname);
        if (NULL == pbucket)
        {
            /* 查找失败返回到当前哈希桶的链表头部 */
            val = l2vc_table.compute_hash((void *)pname)%HASHTAB_SIZE;

            if (NULL != l2vc_table.buckets[val])
            {
                pbucket = l2vc_table.buckets[val];
            }
            else
            {
                for (++val; val<HASHTAB_SIZE; ++val)
                {
                    if (NULL != l2vc_table.buckets[val])
                    {
                        pbucket = l2vc_table.buckets[val];
                        break;
                    }
                }
            }
        }

        while (pbucket)
        {
            pnext = hios_hash_next_cursor(&l2vc_table, pbucket);
            if ((NULL == pnext) || (NULL == pnext->data))
            {
                break;
            }
            pbucket = pnext;

            pl2vc = (struct l2vc_entry *)(pnext->data);

            if(0 == pl2vc->hqos_id)
            {
                continue;
            }

            phqos = qos_hqos_get(pl2vc->hqos_id, MODULE_ID_MPLS);
            if(NULL == phqos)
            {
                continue;
            }

            if(pw_get_hqos_snmp(pl2vc->name, phqos, &hqos_snmp))
            {
                continue;
            }

            memcpy(&hqos_array[data_num++], phqos, sizeof(struct hqos_pw));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }

    return data_num;
}


