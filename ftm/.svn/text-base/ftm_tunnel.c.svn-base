/**
 * @file      : ftm_tunnel.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月9日 9:21:14
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */


#include <lib/memory.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <lib/mpls_common.h>
#include "ftm.h"
#include "ftm_ifm.h"
#include "ftm_tunnel.h"
#include "ftm_arp.h"


struct hash_table ftm_gre_table;
struct list gre_list;

/* traverse lsp list time 3s */
#define TRAVERSE_GRE_LIST_TIME  3


/**
 * @brief      : gre tunnel 下一跳定时学习 arp 回调
 * @param[in ] : thread - 线程结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:22:16
 * @note       :
 */
static int ftm_traverse_gre_list(void *para)
{
    struct listnode *pnode   = NULL;
    struct tunnel_t *ptunnel = NULL;
    struct list     *plist   = &gre_list;

    FTM_LOG_DEBUG("list_count=%d.\n", gre_list.count);

    for (ALL_LIST_ELEMENTS_RO(plist, pnode, ptunnel))
    {
        if (NULL != ptunnel)
        {
            arp_lookup_active(ptunnel->nexthop, 0);
        }
    }

	if (0 != gre_list.count)
	{
        MPLS_TIMER_ADD(ftm_traverse_gre_list, NULL, TRAVERSE_GRE_LIST_TIME);
	}

    return ERRNO_SUCCESS;
}


/**
 * @brief      : gre tunnel 添加入链表
 * @param[in ] : ptunnel - tunnel 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:23:20
 * @note       :
 */
static void ftm_gre_list_add(struct tunnel_t *ptunnel)
{
    struct listnode *pnode = NULL;

    if (NULL == ptunnel)
    {
        return;
    }

    FTM_LOG_DEBUG("Add gre list node, tunnel_index=%x\n", ptunnel->ifindex);

    pnode = listnode_lookup(&gre_list, (void *)ptunnel);
    if (NULL != pnode)
    {
        FTM_LOG_DEBUG("already exist !");

        return ;
    }

    listnode_add(&gre_list, (void *)ptunnel);
	if (1 == gre_list.count)
	{
        MPLS_TIMER_ADD(ftm_traverse_gre_list, NULL, TRAVERSE_GRE_LIST_TIME);
	}

    return;
}


/**
 * @brief      : gre tunnel 从链表删除
 * @param[in ] : ptunnel - tunnel 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:25:05
 * @note       :
 */
static void ftm_gre_list_delete(struct tunnel_t *ptunnel)
{
    if (NULL == ptunnel)
    {
        return;
    }

    FTM_LOG_DEBUG("Delete gre list node, tunnel_index=%x\n", ptunnel->ifindex);

    listnode_delete(&gre_list, (void *)ptunnel);

    return;
}


/**
 * @brief      : 计算 ftm gre 哈希表 key 值
 * @param[in ] : hash_key - 源地址加目的地址
 * @param[out] :
 * @return     : 计算失败返回 0，否则返回 key 值
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:25:42
 * @note       :
 */
static uint32_t ftm_gre_compute_hash(void *hash_key)
{
    struct tunnel_t *ptunnel = NULL;

    if (NULL == hash_key)
    {
        return 0;
    }

    ptunnel = (struct tunnel_t *)hash_key;

    return (ptunnel->sip.addr.ipv4 + ptunnel->dip.addr.ipv4);
}


/**
 * @brief      : 比较 ftm gre 哈希表 key 值
 * @param[in ] : pitem    - 哈希桶
 * @param[in ] : hash_key - 源地址加目的地址
 * @param[out] :
 * @return     : 相同返回 ERRNO_SUCCESS，否则返回 ERRNO_FAIL
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:26:29
 * @note       :
 */
static int ftm_gre_compare(void *pitem, void *hash_key)
{
    struct hash_bucket *pbucket  = (struct hash_bucket *)pitem;
    struct tunnel_t    *ptunnel1 = NULL;
    struct tunnel_t    *ptunnel2 = NULL;

    if ((NULL == pitem) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

    ptunnel1 = (struct tunnel_t *)pbucket->hash_key;
    ptunnel2 = (struct tunnel_t *)hash_key;

    if ((ptunnel1->sip.addr.ipv4 == ptunnel2->sip.addr.ipv4)
    && (ptunnel1->dip.addr.ipv4 == ptunnel2->dip.addr.ipv4))
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : ftm gre 哈希表初始化
 * @param[in ] : size - 哈希表容量
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:28:23
 * @note       :
 */
void ftm_gre_table_init(int size)
{
    hios_hash_init(&ftm_gre_table, size, ftm_gre_compute_hash, ftm_gre_compare);
}


/**
 * @brief      : gre tunnel 添加
 * @param[in ] : ptunnel - tunnel 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:28:48
 * @note       :
 */
int ftm_gre_tunnel_add(struct tunnel_t *ptunnel)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    if ((NULL == ptunnel) || (ptunnel->sip.type == INET_FAMILY_INVALID))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (ftm_gre_table.num_entries == ftm_gre_table.hash_size)
    {
        return ERRNO_OVERSIZE;
    }

    HASH_BUCKET_CREATE(pitem);
    if (NULL == pitem)
    {
        return ERRNO_MALLOC;
    }

    pitem->hash_key = ptunnel;
    pitem->data     = ptunnel;

    hios_hash_add(&ftm_gre_table, pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : gre tunnel 删除
 * @param[in ] : ptunnel - tunnel 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:29:33
 * @note       :
 */
int ftm_gre_tunnel_delete(struct tunnel_t *tunnel)
{
    struct hash_bucket *pitem = NULL;

    FTM_LOG_DEBUG();

    if (NULL == tunnel)
    {
        return ERRNO_SUCCESS;
    }

    pitem = hios_hash_find(&ftm_gre_table, tunnel);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    hios_hash_delete(&ftm_gre_table, pitem);
    HASH_BUCKET_DESTROY(pitem);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : gre tunnel 查找
 * @param[in ] : psip - 源地址
 * @param[in ] : pdip - 目的地址
 * @param[out] :
 * @return     : 成功返回 gre tunnel 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:29:54
 * @note       :
 */
struct tunnel_t *ftm_gre_tunnel_lookup(struct inet_addr *psip, struct inet_addr *pdip)
{
    struct hash_bucket *pitem = NULL;
    struct tunnel_t     tunnel;

    FTM_LOG_DEBUG();

    if ((NULL == psip) || (NULL == pdip))
    {
        return NULL;
    }

    tunnel.sip.addr.ipv4 = psip->addr.ipv4;
    tunnel.dip.addr.ipv4 = pdip->addr.ipv4;

    pitem = hios_hash_find(&ftm_gre_table, &tunnel);
    if (NULL == pitem)
    {
        return ERRNO_SUCCESS;
    }

    return (struct tunnel_t *)pitem->data;
}


/**
 * @brief      : tunnel 源、目的地址更新
 * @param[in ] : ptunnel_old - tunnel 原有结构
 * @param[in ] : ptunnel     - tunnel 新增结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:31:06
 * @note       :
 */
int ftm_gre_tunnel_update(struct tunnel_t *ptunnel_old, struct tunnel_t *ptunnel)
{
    int ret = ERRNO_SUCCESS;

    FTM_LOG_DEBUG();

    if ((NULL == ptunnel) || (NULL == ptunnel_old))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if ((0 == ptunnel_old->sip.addr.ipv4) && (0 != ptunnel->sip.addr.ipv4))      // IP 配置
    {
        memcpy(ptunnel_old, ptunnel, sizeof(struct tunnel_t));

        ret = ftm_gre_tunnel_add(ptunnel_old);
    }
    else if ((0 != ptunnel_old->sip.addr.ipv4) && (0 != ptunnel->sip.addr.ipv4)) // IP 修改
    {
        ftm_gre_tunnel_delete(ptunnel_old);

        ptunnel_old->sip = ptunnel->sip;
        ptunnel_old->dip = ptunnel->dip;

        ret = ftm_gre_tunnel_add(ptunnel_old);
    }
    else if ((0 != ptunnel_old->sip.addr.ipv4) && (0 == ptunnel->sip.addr.ipv4)) // IP 删除
    {
        ftm_gre_tunnel_delete(ptunnel_old);
    }

    return ret;
}


/**
 * @brief      : gre tunnel 添加下一跳
 * @param[in ] : tunnel - tunnel 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:32:42
 * @note       :
 */
static int ftm_gre_tunnel_add_nexthop(struct tunnel_t *tunnel)
{
    struct ftm_ifm   *pifm    = NULL;
    struct arp_entry *parp    = NULL;
    struct tunnel_t  *ptunnel = NULL;

    FTM_LOG_DEBUG();

    if (NULL == tunnel)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pifm = ftm_ifm_lookup(tunnel->ifindex);
    if ((NULL == pifm) || (NULL == pifm->ptunnel))
    {
        return ERRNO_NOT_FOUND;
    }

    ptunnel          = pifm->ptunnel;
    ptunnel->nexthop = tunnel->nexthop;

    parp = arp_lookup_active(tunnel->nexthop, 0);
    if (NULL != parp)
    {
        memcpy(ptunnel->dmac, parp->mac, MAC_LEN);
        ptunnel->outif = parp->ifindex;

        /*ipc_send_hal(ptunnel, sizeof(struct tunnel_t), 1, MODULE_ID_HAL, MODULE_ID_FTM,
                    IPC_TYPE_TUNNEL, TUNNEL_INFO_OUTIF, IPC_OPCODE_UPDATE, ptunnel->ifindex);*/
        ftm_msg_send_to_hal(ptunnel, sizeof(struct tunnel_t), 1,
                    IPC_TYPE_TUNNEL, TUNNEL_INFO_OUTIF, IPC_OPCODE_UPDATE, ptunnel->ifindex);            
    }
    else
    {
        ftm_gre_list_add(ptunnel);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : gre tunnel 删除下一跳
 * @param[in ] : tunnel - tunnel 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:33:24
 * @note       :
 */
static int ftm_gre_tunnel_delete_nexthop(struct tunnel_t *tunnel)
{
    struct ftm_ifm   *pifm    = NULL;
    struct tunnel_t  *ptunnel = NULL;

    FTM_LOG_DEBUG();

    if (NULL == tunnel)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pifm = ftm_ifm_lookup(tunnel->ifindex);
    if ((NULL == pifm) || (NULL == pifm->ptunnel))
    {
        return ERRNO_SUCCESS;
    }

    ptunnel = pifm->ptunnel;
    if (0 != ptunnel->nexthop)
    {
        ptunnel->nexthop = 0;
        ptunnel->outif   = 0;
        memset(ptunnel->dmac, 0, MAC_LEN);

        /*ipc_send_hal(ptunnel, sizeof(struct tunnel_t), 1, MODULE_ID_HAL, MODULE_ID_FTM,
                        IPC_TYPE_TUNNEL, TUNNEL_INFO_OUTIF, IPC_OPCODE_UPDATE, ptunnel->ifindex);*/
		ftm_msg_send_to_hal(ptunnel, sizeof(struct tunnel_t), 1,
                        IPC_TYPE_TUNNEL, TUNNEL_INFO_OUTIF, IPC_OPCODE_UPDATE, ptunnel->ifindex);
	}

    ftm_gre_list_delete(ptunnel);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : gre tunnel 响应 arp 事件
 * @param[in ] : parp   - arp 结构
 * @param[in ] : opcode - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:33:49
 * @note       :
 */
int ftm_tunnel_process_arp_event(struct arp_entry *parp, enum OPCODE_E opcode)
{
    struct ftm_ifm     *pifm    = NULL;
    struct tunnel_t    *ptunnel = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;

    FTM_LOG_DEBUG();

    HASH_BUCKET_LOOP(pbucket, cursor, ftm_ifm_table)
    {
        pifm = (struct ftm_ifm *)pbucket->data;
        if ((NULL == pifm) || (NULL == pifm->ptunnel))
        {
            continue;
        }

        ptunnel = pifm->ptunnel;
        if (ptunnel->nexthop == parp->key.ipaddr)
        {
            if (opcode == OPCODE_DELETE)
            {
                memset(ptunnel->dmac, 0, MAC_LEN);
                ptunnel->outif = 0;

                /*ipc_send_hal(ptunnel, sizeof(struct tunnel_t), 1, MODULE_ID_HAL, MODULE_ID_FTM,
                            IPC_TYPE_TUNNEL, TUNNEL_INFO_OUTIF, IPC_OPCODE_UPDATE, ptunnel->ifindex);*/
				ftm_msg_send_to_hal(ptunnel, sizeof(struct tunnel_t), 1,
                            IPC_TYPE_TUNNEL, TUNNEL_INFO_OUTIF, IPC_OPCODE_UPDATE, ptunnel->ifindex);

                ftm_gre_list_add(ptunnel);
            }
            else
            {
                memcpy(ptunnel->dmac, parp->mac, MAC_LEN);
                ptunnel->outif = parp->ifindex;

                ftm_gre_list_delete(ptunnel);

                /*ipc_send_hal(ptunnel, sizeof(struct tunnel_t), 1, MODULE_ID_HAL, MODULE_ID_FTM,
                            IPC_TYPE_TUNNEL, TUNNEL_INFO_OUTIF, IPC_OPCODE_UPDATE, ptunnel->ifindex);*/
				ftm_msg_send_to_hal(ptunnel, sizeof(struct tunnel_t), 1,
                            IPC_TYPE_TUNNEL, TUNNEL_INFO_OUTIF, IPC_OPCODE_UPDATE, ptunnel->ifindex);
			}
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm tunnel 添加
 * @param[in ] : tunnel - tunnel 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:34:28
 * @note       :
 */
int ftm_tunnel_add(struct tunnel_t *tunnel)
{
    struct ftm_ifm  *pifm    = NULL;
    struct tunnel_t *ptunnel = NULL;

    FTM_LOG_DEBUG();

    if (NULL == tunnel)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pifm = ftm_ifm_lookup(tunnel->ifindex);
    if (NULL == pifm)
    {
        return ERRNO_NOT_FOUND;
    }

    if (NULL != pifm->ptunnel)
    {
        return ERRNO_EXISTED;
    }

    ptunnel = (struct tunnel_t *)XCALLOC(MTYPE_IF, sizeof(struct tunnel_t));
    if (NULL == ptunnel)
    {
        return ERRNO_MALLOC;
    }

    pifm->ptunnel = ptunnel;
    memcpy(ptunnel, tunnel, sizeof(struct tunnel_t));

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm tunnel 删除
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:34:57
 * @note       :
 */
int ftm_tunnel_delete(uint32_t ifindex)
{
    struct ftm_ifm *pifm = NULL;

    FTM_LOG_DEBUG();

    if (0 == ifindex)
    {
        return ERRNO_SUCCESS;
    }

    pifm = ftm_ifm_lookup(ifindex);
    if (NULL == pifm)
    {
        return ERRNO_SUCCESS;
    }

    if (NULL != pifm->ptunnel)
    {
        ftm_gre_tunnel_delete(pifm->ptunnel);
        XFREE(MTYPE_IF, pifm->ptunnel);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : ftm tunnel 查找
 * @param[in ] : ifindex - 接口索引
 * @param[out] :
 * @return     : 成功返回 tunnel 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:35:18
 * @note       :
 */
struct tunnel_t *ftm_tunnel_lookup(uint32_t ifindex)
{
    struct ftm_ifm *pifm = NULL;

    FTM_LOG_DEBUG();

    if (0 == ifindex)
    {
        return NULL;
    }

    pifm = ftm_ifm_lookup(ifindex);
    if (NULL == pifm)
    {
        return NULL;
    }

    return pifm->ptunnel;
}


/**
 * @brief      : ftm tunnel 更新
 * @param[in ] : tunnel - tunnel 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:35:49
 * @note       :
 */
int ftm_tunnel_update(struct tunnel_t *tunnel)
{
    struct tunnel_t *ptunnel = NULL;
    uchar dmac[MAC_LEN] = "";
    uint32_t outif = 0;

    FTM_LOG_DEBUG();

    if (NULL == tunnel)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    ptunnel = ftm_tunnel_lookup(tunnel->ifindex);
    if (NULL == ptunnel)
    {
        ftm_tunnel_add(tunnel);

        ptunnel = ftm_tunnel_lookup(tunnel->ifindex);
        if (NULL == ptunnel)
        {
            return ERRNO_SUCCESS;
        }
    }

    if (0 != ptunnel->outif)
    {
        memcpy(dmac, ptunnel->dmac, MAC_LEN);
        outif = ptunnel->outif;
    }

    if (TUNNEL_PRO_GRE == tunnel->protocol)
	{
		ftm_gre_tunnel_update(ptunnel, tunnel);
	}
    else if (TUNNEL_PRO_INVALID == tunnel->protocol)
    {
        ftm_gre_tunnel_delete(ptunnel);
    }

    memcpy(ptunnel, tunnel, sizeof(struct tunnel_t));

    if (0 != ptunnel->outif)
    {
        memcpy(ptunnel->dmac, dmac, MAC_LEN);
        ptunnel->outif = outif;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : tunnel 消息接收
 * @param[in ] : pdata    - 接收数据
 * @param[in ] : data_len - 数据长度
 * @param[in ] : data_num - 数据个数
 * @param[in ] : subtype  - 消息子类型
 * @param[in ] : opcode   - 操作码
 * @param[in ] : index    - 索引
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月9日 9:36:20
 * @note       :
 */
int ftm_tunnel_msg(void *pdata, int data_len, int data_num, uint8_t subtype,
                            enum IPC_OPCODE opcode, uint32_t ifindex)
{
    struct tunnel_t *ptunnel = NULL;
    int ret = ERRNO_FAIL;
    int i   = 0;

    FTM_LOG_DEBUG("subtype = %d, opcode = %d, ifindex = %x\n", subtype, opcode, ifindex);

    for (i=0; i<data_num; i++)
    {
        ptunnel = (struct tunnel_t *)pdata + i;
        if (TUNNEL_INFO_NEXTHOP == subtype)
        {
            if (opcode == IPC_OPCODE_ADD)
            {
                ftm_gre_tunnel_add_nexthop(ptunnel);
            }
            else if (opcode == IPC_OPCODE_DELETE)
            {
                ftm_gre_tunnel_delete_nexthop(ptunnel);
            }
        }
        else
        {
            if (opcode == IPC_OPCODE_UPDATE)
            {
                ret = ftm_tunnel_update(ptunnel);
                if (ERRNO_SUCCESS != ret)
                {
                    FTM_LOG_ERROR("tunnel %x update failed, errcode = %d\n", ifindex, ret);
                }
            }
            else if (opcode == IPC_OPCODE_DELETE)
            {
                ftm_tunnel_delete(ifindex);
            }
            else if (opcode == IPC_OPCODE_ADD)
            {
                ret = ftm_tunnel_add(ptunnel);
                if (ERRNO_SUCCESS != ret)
                {
                    FTM_LOG_ERROR("tunnel %x add failed, errcode = %d\n", ifindex, ret);
                }
            }
        }
    }

    return ERRNO_SUCCESS;
}


