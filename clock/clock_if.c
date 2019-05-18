/******************************************************************************
 * Filename: clock_if.c
 *  Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: functions for clock interface.
 *
 * History:
 * 2016.10.13  cuiyudong created
 *
******************************************************************************/


#include "lib/types.h"
#include "lib/log.h"
#include "lib/memtypes.h"
#include "lib/memory.h"
#include "lib/msg_ipc.h"
#include "lib/msg_ipc_n.h"
#include "lib/linklist.h"

#include "lib/ifm_common.h"
#include "lib/pkt_buffer.h"

#include "clock_src.h"
#include "clock_if.h"
#include "clock_cmd.h"
#include "synce/synce.h"


/* 时钟接口表 */
struct clockif_t *pclkif[CLOCKIF_NUM_MAX]; /* 用接口 id 标志*/


extern int flag_recv_ssm_event;


/* clockif table 操作函数 */
void clockif_init(void)
{
    int i;

    for (i = 0; i < CLOCKIF_NUM_MAX; i++)
    {
        pclkif[i] = NULL;
    }

    clock_cmd_init();
    return;
}

int clockif_set_info(struct clockif_t *pclk, void *data, enum CLOCKIF_INFO info_type)
{
    int ret;
    uint8_t ql_v = 0;

    if (NULL == data || NULL == pclk)
    {
        zlog_err("%s[%d] set failed info type: %d\n", __FUNCTION__, __LINE__, info_type);
    }

    switch (info_type)
    {
        case CLOCKIF_INFO_MODE:
            pclk->mode = *((enum CLOCKIF_MODE *)data);
            ret = synce_ipc_send_and_wait_ack(data, sizeof(enum CLOCKIF_MODE), 1,MODULE_ID_HAL, \
                         MODULE_ID_CLOCK,IPC_TYPE_SYNCE, info_type, IPC_OPCODE_UPDATE,0);

            if (ret != 0)
            {
                zlog_err("%s[%d] ipc_send_hal errocode=%d sub_type:%d\n", __FUNCTION__, __LINE__, ret , info_type);
                return -1;
            }


            break;

        case CLOCKIF_INFO_QLEVEL:
            ql_v = *((uint8_t *)data);

            if (ql_v != pclk->ssm_level)
            {
                pclk->ssm_level = ql_v;
                flag_recv_ssm_event = 1;
                ret = synce_ipc_send_and_wait_ack(data,  sizeof(uint8_t), 1,MODULE_ID_HAL, \
                                         MODULE_ID_CLOCK,IPC_TYPE_SYNCE, info_type, IPC_OPCODE_UPDATE,0);

                if (ret != 0)
                {
                    zlog_err("%s[%d] ipc_send_hal errocode=%d sub_type:%d\n", __FUNCTION__, __LINE__, ret , info_type);
                    return -1;
                }

            }

            break;

        case CLOCKIF_INFO_PRIORITY:
            pclk->priority = *((uint8_t *)data);
            ret = synce_ipc_send_and_wait_ack(data,  sizeof(uint8_t), 1,MODULE_ID_HAL, \
                                   MODULE_ID_CLOCK,IPC_TYPE_SYNCE, info_type, IPC_OPCODE_UPDATE,0);

            if (ret != 0)
            {
                zlog_err("%s[%d] ipc_send_hal errocode=%d sub_type:%d\n", __FUNCTION__, __LINE__, ret , info_type);
                return -1;
            }

            break;

        case CLOCKIF_INFO_SIGNAL:
            pclk->signal = *((enum CLOCKIF_SIGNAL *)data);
            ret = synce_ipc_send_and_wait_ack(data,sizeof(enum CLOCKIF_SIGNAL), 1,MODULE_ID_HAL, \
                                           MODULE_ID_CLOCK,IPC_TYPE_SYNCE, info_type, IPC_OPCODE_UPDATE,0);

            if (ret != 0)
            {
                zlog_err("%s[%d] ipc_send_hal errocode=%d sub_type:%d\n", __FUNCTION__, __LINE__, ret , info_type);
                return -1;
            }

            break;

        case CLOCKIF_INFO_SA_SEL:
            pclk->sa = *((unsigned char *)data);
            ret = synce_ipc_send_and_wait_ack(data,sizeof(uint8_t), 1,MODULE_ID_HAL, \
                                    MODULE_ID_CLOCK,IPC_TYPE_SYNCE, info_type, IPC_OPCODE_UPDATE,0);

            if (ret != 0)
            {
                zlog_err("%s[%d] ipc_send_hal errocode=%d sub_type:%d\n", __FUNCTION__, __LINE__, ret , info_type);
                return -1;
            }

            break;

        case CLOCKIF_INFO_INVALID :

            break;

        case CLOCKIF_INFO_MAX :
            break;

        default:
            break;
    }

    return 0;
}
struct clockif_t *clockif_create(uint32_t ifindex, uint8_t id)
{
    struct clockif_t *pclk = NULL;

    pclk = XCALLOC(MTYPE_CLOCK_ENTRY, sizeof(struct clockif_t));

    if (NULL == pclk)
    {
        return pclk;
    }

    pclk->ifindex = ifindex;
    pclk->id = id;

    return pclk;
}
int clockif_add(struct clockif_t *pif)         /* 添加到接口表 */
{
    if (NULL == pif)
    {
        return -1;
    }

    if (pif->id > 0 && pif->id <= CLOCKIF_NUM_MAX)
    {
        pclkif[pif->id - 1] = pif;
    }

    return 0;
}
int clockif_delete(uint8_t id)                /* 从接口表删除 */
{
    int ret = 0;

    if (id > 0 && id <= CLOCKIF_NUM_MAX)
    {
        XFREE(MTYPE_CLOCK_ENTRY, pclkif[id - 1]);
        pclkif[id - 1] = NULL;
    }

    return ret;
}
struct clockif_t *clockif_lookup(uint8_t id)  /* 查找一个时钟接口 */
{
    if (id > 0 && id <= CLOCKIF_NUM_MAX)
    {
        return pclkif[id - 1];
    }

    zlog_debug(CLOCK_DBG_INFO,"%s[%d] id err : %d\n", __FUNCTION__, __LINE__, id);
    return NULL;
}
unsigned int synce_snmp_clk_if_next_ifindex_get(struct ipc_msghdr_n *phdr)
{
    int     ret, i;
    uint32_t index_next = 0;
    int index_next_flag = 0;

    for (i = 0; i < CLOCKIF_NUM_MAX; i++)
    {
        if (pclkif[i] != NULL)
        {
            if (index_next_flag)
            {
                index_next = pclkif[i]->ifindex ;
                break;
            }

            if (phdr->msg_index == 0)
            {
                index_next = pclkif[i]->ifindex ;
                break;
            }
            else if (phdr->msg_index == pclkif[i]->ifindex)
            {
                index_next_flag = 1;
            }
        }
    }

    ret = ipc_send_reply_n2(&index_next, sizeof(uint32_t),1,phdr->sender_id,MODULE_ID_CLOCK,\
                            IPC_TYPE_SYNCE,phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);

    if (ret < 0)
    {
        zlog_debug(CLOCK_DBG_ERROR,"[%s %d]ERROR: ipc_send_reply ret(%d)", __FUNCTION__, __LINE__, ret);

        return -1;
    }

    return 0;
}
unsigned int synce_snmp_clk_if_info_get(struct ipc_msghdr_n *phdr)
{
    int     ret;
    uint8_t id;
    struct clockif_t *pclk_if_info = NULL;
    id = IFM_PORT_ID_GET(phdr->msg_index);
    pclk_if_info = clockif_lookup(id);

    if (pclk_if_info)
    {
        ret = ipc_send_reply_n2(pclk_if_info, sizeof(struct clockif_t),1,phdr->sender_id,MODULE_ID_CLOCK,\
                                    IPC_TYPE_SYNCE,phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);

        if (ret < 0)
        {
            zlog_debug(CLOCK_DBG_ERROR,"[%s %d]ERROR: ipc_send_reply ret(%d)", __FUNCTION__, __LINE__, ret);
            return -1;
        }
    }
    else
    {
        zlog_debug(CLOCK_DBG_INFO,"[%s %d]pclk_if_info is NULL id : %d  phdr->msg_index:0x%x\n", __FUNCTION__, __LINE__, id, phdr->msg_index);
        ret = ipc_send_reply_n2(NULL,0,1,phdr->sender_id,MODULE_ID_CLOCK,\
                                 IPC_TYPE_SYNCE,phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_NACK);
    }

    return 0;
}



