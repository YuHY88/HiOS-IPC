/**
 * @file      : lsp_static.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月2日 14:04:53
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/memory.h>
#include <lib/ifm_common.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/alarm.h>
#include "mpls_main.h"
#include "mpls.h"
#include "lsp_static.h"
#include "lspm.h"
#include "labelm.h"
#include "tunnel.h"
#include "pw.h"
#include "mpls_oam/mpls_oam.h"
#include "mpls_aps/mpls_aps.h"


struct hash_table static_lsp_table;


/**
 * @brief      : 计算静态 lsp 哈希表 key 值
 * @param[in ] : hash_key - 静态 lsp 名称
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:28:06
 * @note       : 通过静态 lsp 名字转换为无符号整数计算获得
 */
static unsigned int static_lsp_name_hash(void *hash_key)
{
    if (NULL == hash_key)
    {
        return 0;
    }

    return hios_hash_string((char *)hash_key);
}


/**
 * @brief      : 比较静态 lsp 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - 静态 lsp 名称
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:29:50
 * @note       :
 */
static int static_lsp_name_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket = (struct hash_bucket *)pitem;

    if ((pitem == NULL) || (pbucket->hash_key == NULL) || (hash_key == NULL))
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
 * @brief      : 填充静态 lsp 结构
 * @param[in ] : plsp_old - 原始静态 lsp 结构
 * @param[out] : plsp_new - 目标静态 lsp 结构
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:33:27
 * @note       :
 */
static void static_lsp_set_ingress(struct static_lsp *plsp_new, struct static_lsp *plsp_old)
{
    plsp_new->direction   = plsp_old->direction;
    plsp_new->lsp_index   = plsp_old->lsp_index;
    plsp_new->outlabel    = plsp_old->outlabel;
    plsp_new->group_index = plsp_old->group_index;
    plsp_new->nhp_index   = plsp_old->nhp_index;
    plsp_new->nhp_type    = plsp_old->nhp_type;
    plsp_new->lsp_type    = plsp_old->lsp_type;
    plsp_new->down_flag   = plsp_old->down_flag;
    plsp_new->admin_down  = plsp_old->admin_down;
    plsp_new->status      = plsp_old->status;
    plsp_new->destip      = plsp_old->destip;
    plsp_new->nexthop     = plsp_old->nexthop;
    memcpy(plsp_new->dmac, plsp_old->dmac, 6);

    return;
}


/**
 * @brief      : 填充静态 lsp 结构
 * @param[in ] : plsp_old - 原始静态 lsp 结构
 * @param[out] : plsp_new - 目标静态 lsp 结构
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:34:54
 * @note       :
 */
static void static_lsp_set_egress(struct static_lsp *plsp_new, struct static_lsp *plsp_old)
{
    plsp_new->direction  = plsp_old->direction;
    plsp_new->lsp_index  = plsp_old->lsp_index;
    plsp_new->inlabel    = plsp_old->inlabel;
    plsp_new->lsp_type   = plsp_old->lsp_type;
    plsp_new->down_flag  = plsp_old->down_flag;
    plsp_new->admin_down = plsp_old->admin_down;
    plsp_new->status     = plsp_old->status;
    plsp_new->destip     = plsp_old->destip;

    return;
}


/**
 * @brief      : 使用静态 lsp 结构填充 lsp 结构
 * @param[in ] : plsp_old - 静态 lsp 结构
 * @param[out] : plsp_new - lsp 结构
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:36:33
 * @note       :
 */
static void static_lsp_set_lsp(struct lsp_entry *plsp_new, struct static_lsp *plsp_old)
{
    memset(plsp_new, 0, sizeof(struct lsp_entry));

    plsp_new->direction   = plsp_old->direction;
    plsp_new->lsp_index   = plsp_old->lsp_index;
    plsp_new->outlabel    = plsp_old->outlabel;
    plsp_new->inlabel     = plsp_old->inlabel;
    plsp_new->group_index = plsp_old->group_index;
    plsp_new->nhp_index   = plsp_old->nhp_index;
    plsp_new->nhp_type    = plsp_old->nhp_type;
    plsp_new->lsp_type    = plsp_old->lsp_type;
    plsp_new->ttl         = 255;
    plsp_new->exp         = 1;
    plsp_new->pwlist      = NULL;
    plsp_new->pcouter     = NULL;
    plsp_new->down_flag   = plsp_old->down_flag;
    plsp_new->destip      = plsp_old->destip;
    plsp_new->nexthop     = plsp_old->nexthop;
    memcpy(plsp_new->name, plsp_old->name, NAME_STRING_LEN);
    memcpy(plsp_new->dmac, plsp_old->dmac, 6);

    return;
}


/**
 * @brief      : 静态 lsp 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:37:58
 * @note       :
 */
void static_lsp_table_init(unsigned int size)
{
    hios_hash_init(&static_lsp_table, size, static_lsp_name_hash, static_lsp_name_compare);

    return;
}


/**
 * @brief      : 静态 lsp 结构释放
 * @param[in ] : plsp - 静态 lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:39:49
 * @note       :
 */
void static_lsp_free(struct static_lsp *plsp)
{
    if (plsp != NULL)
    {
        if (0 != plsp->lsp_index)
        {
            mpls_lsp_free_index(plsp->lsp_index);

            plsp->lsp_index = 0;
        }

        if (0 != plsp->inlabel)
        {
            label_free(plsp->inlabel);

            plsp->inlabel = 0;
        }

        XFREE(MTYPE_LSP_ENTRY, plsp);
    }

    return;
}


/**
 * @brief      : 静态 lsp 结构创建
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回分配内存后的静态 lsp 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:40:41
 * @note       :
 */
struct static_lsp *static_lsp_create(void)
{
    struct static_lsp *plsp = NULL;

    plsp = (struct static_lsp *)XCALLOC(MTYPE_LSP_ENTRY, sizeof(struct static_lsp));
    if (NULL == plsp)
    {
        MPLS_LOG_ERROR("malloc\n");
    }

    return plsp;
}


/**
 * @brief      : 静态 lsp 添加
 * @param[in ] : plsp - 静态 lsp 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:41:34
 * @note       :
 */
int static_lsp_add(struct static_lsp *plsp)
{
    struct hash_bucket *pitem       = NULL;
    struct static_lsp  *plsp_lookup = NULL;
    struct static_lsp  *plsp_new    = NULL;
    struct lsp_entry   *plspm       = NULL;
    int                 ret;

    MPLS_LOG_DEBUG();

    if (plsp == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (static_lsp_table.hash_size == static_lsp_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    plsp_lookup = static_lsp_lookup(plsp->name);
    if (NULL != plsp_lookup)
    {
        return ERRNO_EXISTED;
    }

    plsp_new = static_lsp_create();
    if (NULL == plsp_new)
    {
        return ERRNO_MALLOC;
    }

    memcpy(plsp_new->name, plsp->name, strlen((void *)plsp->name)+1);

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        MPLS_LOG_ERROR("malloc\n");

        static_lsp_free(plsp);

        return ERRNO_MALLOC;
    }

    pitem->hash_key = plsp_new->name;
    pitem->data     = plsp_new;

    hios_hash_add(&static_lsp_table, pitem);

    plsp->lsp_index = mpls_lsp_alloc_index();
    if (0 == plsp->lsp_index)
    {
        hios_hash_delete(&static_lsp_table, pitem);
        static_lsp_free(plsp_new);

        HASH_BUCKET_DESTROY(pitem);

        return ERRNO_INDEX_ALLOC;
    }

    /* 根据方向填充静态 lsp */
    if (plsp->direction == LSP_DIRECTION_INGRESS)
    {
        static_lsp_set_ingress(plsp_new, plsp);
    }
    else if (plsp->direction == LSP_DIRECTION_EGRESS)
    {
        static_lsp_set_egress(plsp_new, plsp);
    }
    else
    {
        memcpy(plsp_new, plsp, sizeof(struct static_lsp));
    }

    /* 下一跳方式静态 lsp 先置 down，等待学习到 arp 后再置 up */
    if (0 != plsp_new->nexthop.addr.ipv4)
    {
        plsp_new->admin_down = LINK_DOWN;
        plsp_new->down_flag  = LINK_DOWN;
    }

    if (LINK_UP == plsp_new->down_flag)
    {
        gmpls.static_lsp_up_num++;
    }

    plspm = mpls_lsp_create();
    if (NULL == plspm)
    {
        return ERRNO_MALLOC;
    }

    static_lsp_set_lsp(plspm, plsp_new);
    ret = mpls_lsp_add(plspm);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("errcode = %d\n", ret);

        hios_hash_delete(&static_lsp_table, pitem);
        static_lsp_free(plsp_new);
        mpls_lsp_free(plspm);

        HASH_BUCKET_DESTROY(pitem);

        return ret;
    }

    /* 通知 pw 响应 lsp 隧道变化事件 */
    if ((LSP_DIRECTION_INGRESS == plsp->direction) && (LINK_UP == plsp->down_flag))
    {
        l2vc_process_tunnel_static_lsp_up(plsp);
    }

    //ipran_alarm_port_register(IFM_LSP_2_TYPE, 0, 0, plsp->lsp_index, 0);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
	gPortInfo.iMsgPara1 = plsp->lsp_index;
	ipran_alarm_port_register(&gPortInfo);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 静态 lsp 删除
 * @param[in ] : pname - 静态 lsp 名称
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:46:28
 * @note       :
 */
int static_lsp_delete(uchar *pname)
{
    struct hash_bucket *pitem = NULL;
    struct static_lsp  *plsp  = NULL;

    MPLS_LOG_DEBUG();

    if (pname == NULL)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pitem = hios_hash_find(&static_lsp_table, (void *)pname);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    plsp = (struct static_lsp *)pitem->data;
    if (plsp->group_index != 0)
    {
        tunnel_mplstp_process_lsp_event(plsp, OPCODE_DELETE);
    }

	if (plsp->direction == LSP_DIRECTION_TRANSIT && plsp->mplsoam_id != 0)
    {
        mplsoam_session_unbind_service(plsp->mplsoam_id, plsp);
    }

    if (plsp->aps_id)
    {
        mplsaps_unbind_lsp_pw(plsp->aps_id);
    }

    //ipran_alarm_port_unregister(IFM_LSP_2_TYPE, 0, 0, plsp->lsp_index, 0);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
	gPortInfo.iMsgPara1 = plsp->lsp_index;
	ipran_alarm_port_unregister(&gPortInfo);

    mpls_lsp_delete(plsp->lsp_index);

    if (LINK_UP == plsp->down_flag)
    {
        gmpls.static_lsp_up_num--;
    }

    hios_hash_delete(&static_lsp_table, pitem);

    /* 通知 pw 响应 lsp 隧道变化事件 */
    if ((LSP_DIRECTION_INGRESS == plsp->direction) && (LINK_UP == plsp->down_flag))
    {
        l2vc_process_tunnel_static_lsp_down(plsp);
    }

    static_lsp_free(plsp);

    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 静态 lsp 查找
 * @param[in ] : pname - 静态 lsp 名称
 * @param[out] :
 * @return     : 成功返回静态 lsp 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:49:03
 * @note       :
 */
struct static_lsp *static_lsp_lookup(uchar *pname)
{
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (pname == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pitem = hios_hash_find(&static_lsp_table, pname);
    if (NULL == pitem)
    {
        return NULL;
    }

    return (struct static_lsp *)pitem->data;
}


/**
 * @brief      : 静态 lsp 告警上报
 * @param[in ] : lsp_index - 上报告警的静态 lsp 索引
 * @param[in ] : down_flag - 上报或清除告警
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:49:47
 * @note       :
 */
static void static_lsp_alarm_process(uint32_t lsp_index, uint8_t down_flag)
{
    MPLS_LOG_DEBUG();

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;

    if (LINK_UP == down_flag)
    {
        //ipran_alarm_report(IFM_LSP_2_TYPE, 0, 0, lsp_index, 0, GPN_ALM_TYPE_LSP_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
                
		gPortInfo.iMsgPara1 = lsp_index;
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_LSP_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
    }
    else if (LINK_DOWN == down_flag)
    {
        //ipran_alarm_report(IFM_LSP_2_TYPE, 0, 0, lsp_index, 0, GPN_ALM_TYPE_LSP_DOWN, GPN_SOCK_MSG_OPT_RISE);

		gPortInfo.iMsgPara1 = lsp_index;
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_LSP_DOWN, GPN_SOCK_MSG_OPT_RISE);
    }

//    alarm_report(opcode, MODULE_ID_MPLS, ALM_CODE_RAN_LSP_DOWN, lsp_index, 0, ALM_STYPE_RANTNL, 0);

    return;
}


/**
 * @brief      : 静态 lsp 批量获取
 * @param[in ] : pname     - 静态 lsp 名称
 * @param[in ] : data_len  - 静态 lsp 名称长度
 * @param[out] : slsp_buff - 获取到的静态 lsp 数据
 * @return     : 返回获取到静态 lsp 的数量
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:51:09
 * @note       : 提供给 snmp 的 get next 接口，空索引从数据结构开始返回数据，
 * @note       : 非空索引从传入索引节点下一个节点开始返回数据
 */
int static_lsp_get_bulk(uchar *pname, int data_len, struct static_lsp slsp_buff[])
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct static_lsp  *plsp    = NULL;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct static_lsp);
    int cursor;

    if (NULL == pname)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (0 == data_len)
    {
        /* 初次获取 */
        HASH_BUCKET_LOOP(pbucket, cursor, static_lsp_table)
        {
            plsp = (struct static_lsp *)pbucket->data;
            if (NULL == plsp)
            {
                continue;
            }

            memcpy(&slsp_buff[data_num++], plsp, sizeof(struct static_lsp));
            if (data_num == msg_num)
            {
                MPLS_LOG_DEBUG("data_num %d\n", data_num);
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&static_lsp_table, (void *)pname);

        if (NULL != pbucket)
        {
            for (data_num=0; data_num<msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&static_lsp_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                plsp = pnext->data;
                memcpy(&slsp_buff[data_num], plsp, sizeof(struct static_lsp));
                pbucket = pnext;
            }
        }
    }
    MPLS_LOG_DEBUG("data_num %d\n", data_num);
    return data_num;
}


/**
 * @brief      : 静态 lsp 管理 up
 * @param[in ] : plsp - 静态 lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:53:52
 * @note       : 静态 lsp 管理 up 意味着指定出接口方式的静态 lsp，出接口状态为 up
 */
void static_lsp_admin_up(struct static_lsp *plsp)
{
    struct lsp_entry *plspm = NULL;
    int ret = 0;

    MPLS_LOG_DEBUG();

    if ((NULL == plsp) || (LINK_UP == plsp->admin_down))
    {
        return;
    }

    plsp->admin_down = LINK_UP;

    MPLS_LOG_DEBUG("down_flag = %d\n", plsp->down_flag);

    if (LINK_DOWN == plsp->down_flag)
    {
        plsp->down_flag = LINK_UP;
        gmpls.static_lsp_up_num++;

        plspm = mpls_lsp_lookup(plsp->lsp_index);
        if (NULL != plspm)
        {
            plspm->down_flag = LINK_UP;

            ret = mpls_lsp_update(plspm, LSP_SUBTYPE_UP);
            if (ERRNO_SUCCESS != ret)
            {
                MPLS_LOG_ERROR("errcode = %d\n", ret);
            }
        }

        /* 静态 lsp 被 tunnel 指定，修改工作状态 */
        if (plsp->group_index != 0)
        {
            tunnel_mplstp_process_lsp_event(plsp, OPCODE_UP);
        }

        if (plsp->direction == LSP_DIRECTION_INGRESS)
        {
            l2vc_process_tunnel_static_lsp_up(plsp);
        }
    }

    static_lsp_alarm_process(plsp->lsp_index, LINK_UP);

    return;
}


/**
 * @brief      : 静态 lsp 管理 down
 * @param[in ] : plsp - 静态 lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:57:14
 * @note       : 静态 lsp 管理 down 意味着指定出接口方式的静态 lsp，出接口状态为 down
 */
void static_lsp_admin_down(struct static_lsp *plsp)
{
    struct lsp_entry *plspm = NULL;
    int ret = 0;

    MPLS_LOG_DEBUG();

    if ((NULL == plsp) || (LINK_DOWN == plsp->admin_down))
    {
        return;
    }

    plsp->admin_down = LINK_DOWN;

    MPLS_LOG_DEBUG("down_flag = %d\n", plsp->down_flag);

    if (plsp->down_flag == LINK_UP)
    {
        plsp->down_flag = LINK_DOWN;
        gmpls.static_lsp_up_num--;

        plspm = mpls_lsp_lookup(plsp->lsp_index);
        if (NULL != plspm)
        {
            plspm->down_flag = LINK_DOWN;

            ret = mpls_lsp_update(plspm, LSP_SUBTYPE_DOWN);
            if (ERRNO_SUCCESS != ret)
            {
                MPLS_LOG_ERROR("errcode = %d\n", ret);
            }
        }

        /* 静态 lsp 被 tunnel 指定，修改工作状态 */
        if (plsp->group_index != 0)
        {
            tunnel_mplstp_process_lsp_event(plsp, OPCODE_DOWN);
        }

        if (plsp->direction == LSP_DIRECTION_INGRESS)
        {
            l2vc_process_tunnel_static_lsp_down(plsp);
        }
    }

    static_lsp_alarm_process(plsp->lsp_index, LINK_DOWN);

    return;
}


/**
 * @brief      : 静态 lsp link up
 * @param[in ] : plsp - 静态 lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 14:58:56
 * @note       : 静态 lsp link up 意味着检测机制上报 up 事件
 */
void static_lsp_up(struct static_lsp *plsp)
{
    struct lsp_entry *plspm = NULL;
    int ret = 0;

    MPLS_LOG_DEBUG();

    if (NULL == plsp)
    {
        return;
    }

    if ((plsp->down_flag == LINK_UP) || (plsp->admin_down == LINK_DOWN))
    {
        return;
    }

    plsp->down_flag = LINK_UP;
    gmpls.static_lsp_up_num++;

    plspm = mpls_lsp_lookup(plsp->lsp_index);
    if (NULL != plspm)
    {
        plspm->down_flag = LINK_UP;

        ret = mpls_lsp_update(plspm, LSP_SUBTYPE_UP);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("errcode = %d\n", ret);
        }
    }

    /* 静态 lsp 被 tunnel 指定，修改工作状态 */
    if (plsp->group_index != 0)
    {
        tunnel_mplstp_process_lsp_event(plsp, OPCODE_UP);
    }

    if (plsp->direction == LSP_DIRECTION_INGRESS)
    {
        l2vc_process_tunnel_static_lsp_up(plsp);
    }

    static_lsp_alarm_process(plsp->lsp_index, LINK_UP);

    return;
}


/**
 * @brief      : 静态 lsp link down
 * @param[in ] : plsp - 静态 lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:00:56
 * @note       : 静态 lsp link down 意味着检测机制上报 down 事件
 */
void static_lsp_down(struct static_lsp *plsp)
{
    struct lsp_entry *plspm = NULL;
    int ret = 0;

    MPLS_LOG_DEBUG();

    if (NULL == plsp)
    {
        return;
    }

    if (plsp->down_flag == LINK_DOWN)
    {
        return ;
    }

    plsp->down_flag = LINK_DOWN;
    gmpls.static_lsp_up_num--;

    plspm = mpls_lsp_lookup(plsp->lsp_index);
    if (NULL != plspm)
    {
        plspm->down_flag = LINK_DOWN;

        ret = mpls_lsp_update(plspm, LSP_SUBTYPE_DOWN);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("errcode = %d\n", ret);
        }
    }

    /* 静态 lsp 被 tunnel 指定，修改工作状态 */
    if (plsp->group_index != 0)
    {
        tunnel_mplstp_process_lsp_event(plsp, OPCODE_DOWN);
    }

    if (plsp->direction == LSP_DIRECTION_INGRESS)
    {
        l2vc_process_tunnel_static_lsp_down(plsp);
    }

    static_lsp_alarm_process(plsp->lsp_index, LINK_DOWN);

    return;
}


/**
 * @brief      : 静态 lsp 切换状态 enable
 * @param[in ] : plsp - 静态 lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月12日 17:36:48
 * @note       : 静态 lsp enable 意味着检测机制上报 up 事件
 */
void static_lsp_enable(struct static_lsp *plsp)
{
    MPLS_LOG_DEBUG();

    if (NULL == plsp)
    {
        return;
    }

    if (plsp->status == ENABLE)
    {
        return;
    }

    plsp->status = ENABLE;

    if (plsp->group_index != 0)
    {
        tunnel_mplstp_process_lsp_event(plsp, OPCODE_UP);
    }

    return;
}


/**
 * @brief      : 静态 lsp 切换状态 disable
 * @param[in ] : plsp - 静态 lsp 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月12日 17:37:46
 * @note       : 静态 lsp disable 意味着检测机制上报 down 事件
 */
void static_lsp_disable(struct static_lsp *plsp)
{
    MPLS_LOG_DEBUG();

    if (NULL == plsp)
    {
        return;
    }

    if (plsp->status == DISABLE)
    {
        return;
    }

    plsp->status = DISABLE;

    if (plsp->group_index != 0)
    {
        tunnel_mplstp_process_lsp_event(plsp, OPCODE_DOWN);
    }

    return;
}


/**
 * @brief      : 静态 lsp 相应接口 up 事件
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:02:11
 * @note       :
 */
void static_lsp_process_ifup(uint32_t ifindex)
{
    struct hash_bucket *pbucket = NULL;
    struct static_lsp  *plsp    = NULL;
    int cursor;

    MPLS_LOG_DEBUG();

    HASH_BUCKET_LOOP(pbucket, cursor, static_lsp_table)
    {
        if (pbucket == NULL)
        {
            continue;
        }

        plsp = (struct static_lsp *)pbucket->data;

        if ((plsp->nhp_index == ifindex) && (plsp->admin_down == LINK_DOWN))
        {
            static_lsp_admin_up(plsp);
        }
    }

    return;
}


/**
 * @brief      : 静态 lsp 相应接口 down 事件
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:03:00
 * @note       :
 */
void static_lsp_process_ifdown(uint32_t ifindex)
{
    struct hash_bucket *pbucket = NULL;
    struct static_lsp  *plsp    = NULL;
    int cursor;

    MPLS_LOG_DEBUG();

    HASH_BUCKET_LOOP(pbucket, cursor, static_lsp_table)
    {
        plsp = (struct static_lsp *)pbucket->data;

        if (plsp == NULL)
        {
            continue;
        }

        if ((plsp->nhp_index == ifindex) && (plsp->admin_down == LINK_UP))
        {
            static_lsp_admin_down(plsp);
        }
    }

    return;
}


/**
 * @brief      : 静态 lsp 响应接口事件
 * @param[in ] : ifindex - 接口索引
 * @param[in ] : mode    - 接口模式
 * @param[in ] : opcode  - 接口状态
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:10:48
 * @note       :
 */
void static_lsp_process_ifevent(uint32_t ifindex, int mode, enum OPCODE_E opcode)
{
    uint8_t pstatus = 0;

    MPLS_LOG_DEBUG("mode = %d\n", mode);

    if ((opcode == OPCODE_UP) && (mode == IFNET_MODE_L3))
    {
        static_lsp_process_ifup(ifindex);
    }
    else if (opcode == OPCODE_DOWN)
    {
        static_lsp_process_ifdown(ifindex);
    }
    else if (opcode == OPCODE_UPDATE)
    {
        if (mode != IFNET_MODE_L3)
        {
            static_lsp_process_ifdown(ifindex);
        }
        else
        {
            if (ifm_get_link(ifindex, MODULE_ID_MPLS, &pstatus))
            {
                MPLS_LOG_ERROR("get interface link status\n");

                return;
            }

            if (pstatus == IFNET_LINKUP)
            {
                static_lsp_process_ifup(ifindex);
            }
        }
    }

    return;
}


/**
 * @brief      : 使用静态 lsp 名称获取静态 lsp 索引
 * @param[in ] : pname - 静态 lsp 名称
 * @param[out] :
 * @return     : 成功返回静态 lsp 索引，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:13:27
 * @note       :
 */
uint32_t static_lsp_get_index(uchar *pname)
{
    struct static_lsp  *lsp   = NULL;
    struct hash_bucket *pitem = NULL;

    MPLS_LOG_DEBUG();

    if (pname == NULL)
    {
        return ERRNO_SUCCESS;
    }

    pitem = hios_hash_find(&static_lsp_table, pname);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    lsp = (struct static_lsp *)pitem->data;
    if (lsp == NULL)
    {
        return ERRNO_SUCCESS;
    }

    return lsp->lsp_index;
}


/**
 * @brief      : 使用静态 lsp 索引获取静态 lsp 名称
 * @param[in ] : index - 静态 lsp 索引
 * @param[out] :
 * @return     : 成功返回静态 lsp 名称，失败返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:14:35
 * @note       :
 */
uchar *static_lsp_get_name(uint32_t index)
{
    return mpls_lsp_get_name(index);
}


/**
 * @brief      : 静态 lsp 响应 arp 事件
 * @param[in ] : lsp_index - 静态 lsp 索引
 * @param[in ] : opcode    - arp 添加或删除操作
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 15:15:13
 * @note       :
 */
void static_lsp_process_arp(uint32_t lsp_index, enum OPCODE_E opcode, uint32_t nhp_index)
{
    struct static_lsp *pslsp = NULL;

    pslsp = static_lsp_lookup(static_lsp_get_name(lsp_index));
    if (NULL == pslsp)
    {
        return;
    }

    if (OPCODE_UP == opcode)
    {
    	pslsp->oam_nhp_index = nhp_index;
        static_lsp_admin_up(pslsp);
    }
    else if (OPCODE_DOWN == opcode)
    {
        static_lsp_admin_down(pslsp);
    }

    return;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月8日 16:53:33
 * @note       : 静态LSP是用name作hash，如果存在两条pw都能使用的lsp，该函数存在选错路的可能
 */
struct static_lsp *static_lsp_select(uint32_t destip)
{
    struct hash_bucket *pbucket = NULL;
    struct static_lsp  *plsp    = NULL;
    int cursor = 0;

    if (0 == destip)
    {
        return NULL;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, static_lsp_table)
    {
        plsp = (struct static_lsp *)pbucket->data;
        if (NULL == plsp)
        {
            continue;
        }

        if ((plsp->direction == LSP_DIRECTION_INGRESS)
            && (plsp->admin_down == LINK_UP)
            && (plsp->destip.addr.ipv4 == destip))
        {
            return plsp;
        }
    }

    return NULL;
}


