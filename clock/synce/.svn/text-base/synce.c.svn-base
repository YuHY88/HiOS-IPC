/******************************************************************************
 * Filename: synce.c
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: functions for syncE protocal and clock source .
 *
 * History:
 * 2016.10.13  cuiyudong created
 *
******************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "lib/command.h"
#include "lib/module_id.h"
#include "lib/msg_ipc.h"
#include "lib/hash1.h"
#include "lib/zassert.h"
#include "lib/memtypes.h"
#include "lib/memory.h"
#include "lib/linklist.h"
#include "lib/thread.h"
#include "lib/log.h"
#include "lib/alarm.h"
#include "lib/devm_com.h"
#include "lib/pkt_buffer.h"
#include <lib/msg_ipc_n.h>

#include "clock_main.h"
#include "clock/clock_src.h"
#include "clock/clock_if.h"
#include "clock/clock_cmd.h"
#include "clock/clock_alarm.h"

#include "synce_ssm.h"
#include "synce_cmd.h"
#include "synce.h"

extern struct hash_table l2if_table;
extern int flag_recv_ssm_event;
unsigned int synce_dev_id = 0;
unsigned int synce_alarm_global_flag = 0;  /* syncE告警位标识  */
unsigned char e1_if_priority[32] = {0};
static time_t synce_wait_time_counter = 0 ;

int synce_ipc_send_and_wait_ack(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	int ret = 0;
	struct ipc_mesg_n* pMsgRep = NULL;
	
	pMsgRep = ipc_sync_send_n2( pdata , data_len, data_num, module_id, sender_id,
								msg_type, subtype, opcode, msg_index, 1000);
	if (NULL == pMsgRep)
    {
    	zlog_debug (CLOCK_DBG_ERROR, "send sync message fail.\n" );
        return -1;
    }

	if (IPC_OPCODE_ACK == pMsgRep->msghdr.opcode) {
		ret = 0;
	}
    else if (IPC_OPCODE_NACK == pMsgRep->msghdr.opcode){
		memcpy(&ret, pMsgRep->msg_data, sizeof(ret));
	}
	else{
		ret = -1;
	}
	
    mem_share_free(pMsgRep, MODULE_ID_CLOCK);
    return ret;
}


/************************************************
 * Function: synce_update_status
 * Input:    pdata: 从HAL获取的数据指针，指向一个SynceHalGlobalData 数据区
 * Output:   NONE
 * Return:   0 : 数据更新成功  -1 : 数据更新失败
 * Description:  从HAL层获取硬件信息，并更新应用层的全局数据.
 ************************************************/
static int synce_update_status(void *pdata)
{
    struct synce_clk_t *pclk = NULL;
    struct listnode *pnode = NULL;
    enum VALID_STAT new_state;
    uchar chg_flag = 0;
    int soft_alarm = 0;

    if (NULL == pdata)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d] failed\n", __FUNCTION__, __LINE__);
        return -1;
    }

    SynceHalGlobalData *p_synce_hal = (SynceHalGlobalData *)pdata;
    memcpy(&g_synce_hal, p_synce_hal, sizeof(SynceHalGlobalData));

    for (ALL_LIST_ELEMENTS_RO(&g_synce.clk_list, pnode, pclk))
    {
        new_state = synce_get_clk_stat(pclk->ifindex, pclk->type);

        if (pclk->valid_stat != new_state)
        {
            pclk->valid_stat = new_state;

            if (new_state == VALID_STAT_VALID)
            {
                chg_flag = 1;

                if (pclk->synce_alarm_flag & SYNCE_ALARM_FLAG_CLK_INVALID)
                {
                    ClockAlarmReport(SYNCE_ALARM_FLAG_CLK_INVALID, GPN_SOCK_MSG_OPT_CLEAN, pclk->ifindex, 0);
                    pclk->synce_alarm_flag &= ~SYNCE_ALARM_FLAG_CLK_INVALID;
                }
            }
            else
            {
                if (!(pclk->synce_alarm_flag & SYNCE_ALARM_FLAG_CLK_INVALID))
                {
                    ClockAlarmReport(SYNCE_ALARM_FLAG_CLK_INVALID, GPN_SOCK_MSG_OPT_RISE, pclk->ifindex, 0);
                    pclk->synce_alarm_flag |= SYNCE_ALARM_FLAG_CLK_INVALID;
                }
            }
        }

        soft_alarm = synce_get_soft_alarm_stat(pclk->ifindex);

        if (soft_alarm)
        {
            if (!(pclk->synce_alarm_flag & SYNCE_ALARM_FLAG_FREQ_OVERFLOW))
            {
                ClockAlarmReport(SYNCE_ALARM_FLAG_FREQ_OVERFLOW, GPN_SOCK_MSG_OPT_RISE, pclk->ifindex, 0);
                pclk->synce_alarm_flag |= SYNCE_ALARM_FLAG_FREQ_OVERFLOW;
            }
        }
        else
        {
            if (pclk->synce_alarm_flag & SYNCE_ALARM_FLAG_FREQ_OVERFLOW)
            {
                ClockAlarmReport(SYNCE_ALARM_FLAG_FREQ_OVERFLOW, GPN_SOCK_MSG_OPT_CLEAN, pclk->ifindex, 0);
                pclk->synce_alarm_flag &= ~SYNCE_ALARM_FLAG_FREQ_OVERFLOW;
            }
        }

        pclk->status = CLOCK_STAT_LOSS;
    }

    pclk = NULL;


    synce_clock_update_if_status();

    if (chg_flag)
    {
        g_synce.iWaitTimer = g_synce.wtr;
        time(&synce_wait_time_counter);
        //synce_select_clock(SYNCE_EVENT_SEL_IF_DOWN);
    }

    if ((g_synce_hal.cur_clk_source.ifindex != 0) && (g_synce.iWaitTimer <= 0))
    {
        pclk = synce_lookup_clk(g_synce_hal.cur_clk_source.ifindex);

        if (pclk != NULL && pclk->valid_stat == VALID_STAT_VALID && g_synce_hal.cur_clock_state == SYNCE_RUN_STATE_LOCKED)
        {
            pclk->status = CLOCK_STAT_LOCK;

            if ((g_synce.mode == SYNCE_MODE_AUTO) && (!g_synce.ssm_enable))
            {
                g_synce.clk_select = g_synce_hal.cur_clk_source.ifindex;
            }
        }
    }

    return 0;
}

/************************************************
 * Function: synce_send_get_msg_to_hal
 * Input:    NONE
 * Output:   NONE
 * Return:   0 : 数据接收成功  -1 : 数据接收失败
 * Description:  发送IPC消息到HAL层，并调用synce_update_status更新应用层的全局数据.
 ************************************************/

static int synce_send_get_msg_to_hal(void)
{
    void *p = NULL;
    struct ipc_mesg_n * psnd_msg = NULL;

    if ((synce_dev_id != ID_HT201E) && (synce_dev_id != ID_HT2200) && (synce_dev_id != ID_HT2200V2)\
         && (synce_dev_id != ID_HT2100)  && (synce_dev_id != ID_HT2100V2) && (synce_dev_id != ID_HT158)\
		 && (synce_dev_id != ID_H9MOLMXE_VX))
    {
        zlog_debug(CLOCK_DBG_INFO,"%s[%d] : unknown device id : 0x%x!\n", __FILE__, __LINE__, synce_dev_id);
        return -1;
    }




    psnd_msg = ipc_sync_send_n2(NULL,1,1,MODULE_ID_HAL,MODULE_ID_CLOCK,IPC_TYPE_SYNCE,SYNCE_INFO_GET_GLOBAL,IPC_OPCODE_GET,1,2000);
    
    if (psnd_msg == NULL)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d] send IPC get info failed! \n", __FUNCTION__, __LINE__);
        return -1;
    }
    p = (void *)(psnd_msg->msg_data);
    
    synce_update_status(p);
    
    if(psnd_msg)
	{
		mem_share_free(psnd_msg, MODULE_ID_CLOCK);
	}
    return 0;
}
static int synce_alarm_check(void)
{
    static unsigned int history_sel_ifindex = 0;
    static int first_flag = 0;

    if (first_flag == 0)
    {
        history_sel_ifindex = g_synce.clk_select;
        first_flag = 1;
        return 0;
    }

    if (history_sel_ifindex != g_synce.clk_select)
    {
        ClockAlarmReport(SYNCE_ALARM_FLAG_SYNC_CHANAGE, GPN_SOCK_MSG_OPT_RISE, 0, 0);

        history_sel_ifindex = g_synce.clk_select;
        g_synce.iWaitTimer = g_synce.wtr;
        time(&synce_wait_time_counter);
    }

    return 0;
}
/************************************************
 * Function: synce_timer_start
 * Input:    thread :线程数据指针
 * Output:   NONE
 * Return:   0 : successed  -1 : failed
 * Description:  定时器定时执行函数，包括发送SSM报文，更新硬件信息
 ************************************************/

int synce_timer_start(void *thread)
{
    struct synce_if     *synce_port;
    struct clock_src_t *psrc = NULL;
    static int send_msg_cnt = 0;
    static int select_clk_cnt = 0;
    int i = 0;
    time_t synce_cur_wait_time_cnt = 0;


    for (i = 0; i < CLOCK_SRC_NUM_MAX; i++)
    {
        psrc = pclock_src[i];

        if (psrc == NULL)
        {
            continue;
        }

        if (psrc->psynce == NULL)
        {
            continue;
        }

        synce_port = psrc->psynce;

        if ((synce_port != NULL) && IFM_TYPE_IS_METHERNET(psrc->ifindex) && g_synce.ssm_enable)
        {
            if (psrc->down_flag == IFNET_LINKUP)
            {
                synce_port->send_interval_cnt++;

                if (synce_port->send_interval_cnt >= g_synce.ssm_interval)
                {
                    if (g_synce.iSendFlag > 0)
                    {
                        synce_ssm_send(synce_port, psrc->ifindex, PKT_ENEVT); /*ssm报文发送*/
                        g_synce.iSendFlag--;
                    }
                    else
                    {
                        synce_ssm_send(synce_port, psrc->ifindex, PKT_INFO); /*ssm报文发送*/
                    }

                    synce_port->send_interval_cnt = 0;
                }
            }
            else if (psrc->down_flag == IFNET_LINKDOWN)
            {
                synce_port->recv_ql = SSM_DNU;
            }

            synce_port->timeout_cnt++;

            if (synce_port->timeout_cnt > g_synce.ssm_timeout)
            {
                if (synce_port->status != SSM_STATUS_TIMEOUT)
                {
                    synce_ssm_fsm(EVENT_TIMEOUT, synce_port, psrc->ifindex);
                }

                synce_port->timeout_cnt = 0;
            }
        }
    }

    if (send_msg_cnt++ > SYNCE_SEND_MSG_INTERVAL)
    {
        synce_send_get_msg_to_hal();
        send_msg_cnt = 0;
        synce_protocol_run_stat_can();
    }

    if (select_clk_cnt++ > SYNCE_SELECT_SOURCE_INTERVAL)
    {
        select_clk_cnt = 0;
        synce_select_clock(SYNCE_EVENT_SEL_SSM_PKT_EVENT);
    }

    if (flag_recv_ssm_event)
    {
        //synce_select_clock(SYNCE_EVENT_SEL_SSM_PKT_EVENT);
        ClockAlarmReport(SYNCE_ALARM_FLAG_QL_SWITCH, GPN_SOCK_MSG_OPT_RISE, 0, 0);
        flag_recv_ssm_event = 0;
    }

    if (g_synce.iWaitTimer > 0)
    {
        time(&synce_cur_wait_time_cnt);
        g_synce.iWaitTimer -= (synce_cur_wait_time_cnt - synce_wait_time_counter);
        synce_wait_time_counter = synce_cur_wait_time_cnt;
    }

    synce_alarm_check();

    return 0;
}

/************************************************
 * Function: synce_get_lock_status
 * Input:    ifindex : 接口索引
 * Output:   NONE
 * Return:   接口时钟的锁定状态
 * Description:  获取当前端口是否为锁定时钟源
 ************************************************/
enum CLOCK_STAT synce_get_lock_status(uint32_t ifindex)
{
    struct synce_clk_t *psynce_clk = NULL;
    psynce_clk = synce_lookup_clk(ifindex);

    if (NULL != psynce_clk)
    {
        return psynce_clk->status;
    }
    else
    {
        return CLOCK_STAT_INVALID;
    }
}
/************************************************
 * Function: synce_get_valid_status
 * Input:    ifindex : 接口索引  clk_type : 时钟接口类型
 * Output:   NONE
 * Return:   接口时钟的是否有效状态
 * Description:  获取当前端口时钟有效状态
 ************************************************/
unsigned char synce_get_soft_alarm_stat(uint32_t ifindex)
{
    int i;

    for (i = 1; i <= SYNCE_CLK_NUM; i++)
    {
        if (g_synce_hal.cur_ic_info[i].if_index == ifindex)
        {
            return g_synce_hal.cur_ic_info[i].ic_softalm;
        }
    }

    zlog_debug(CLOCK_DBG_ERROR,"%s[%d]: can't get soft alarm stat ifindex 0x%x\n", __FUNCTION__, __LINE__, ifindex);
    return 0;
}

/************************************************
 * Function: synce_get_valid_status
 * Input:    ifindex : 接口索引
 * Output:   NONE
 * Return:   接口时钟的是否有效状态
 * Description:  获取当前端口时钟有效状态
 ************************************************/
enum VALID_STAT synce_get_valid_status(uint32_t ifindex)
{
    struct synce_clk_t *psynce_clk = NULL;
    psynce_clk = synce_lookup_clk(ifindex);

    if (NULL != psynce_clk)
    {
        return psynce_clk->valid_stat;
    }
    else
    {
        return VALID_STAT_NULL;
    }
}

/************************************************
 * Function: synce_get_ssm_status
 * Input:    ifindex : 接口索引
 * Output:   NONE
 * Return:   接口时钟的SSM状态值
 * Description:  获取同步以太接口的SSM状态
 ************************************************/
enum SSM_STATUS synce_get_ssm_status(uint32_t ifindex)
{
    struct clock_src_t *p_clksrc = NULL;
    p_clksrc = clock_src_lookup(ifindex);

    if (NULL != p_clksrc)
    {
        if (p_clksrc->psynce != NULL)
        {
            return p_clksrc->psynce->status;
        }
        else
        {
            return SSM_STATUS_INVALID;
        }
    }
    else
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d] l2if lookup failed ifindex=%d\n", __FUNCTION__, __LINE__, ifindex);
        return SSM_STATUS_INVALID;
    }
}

static int synce_clear_ssm_global(void)
{
    g_synce.cur_clk_ssm_ql = SSM_DNU;
    return 0;
}
/************************************************
 * Function: synce_set_info
 * Input:    value : 配置数据的指针   type 配置项参数
 * Output:   NONE
 * Return:   0 : successed  -1 : failed
 * Description:  配置syncE全局数据，通过不同的配置项，
 *               配置不同功能，如有硬件相应操作，需发送IPC消息到HAL层。
 ************************************************/
int synce_set_info(void *value, enum SYNCE_INFO type) /* 配置 synce_t 的内容 */
{
    int ret, mode;
    uint8_t ssm_en;
    struct synce_clk_t *psynce_clk;

    switch (type)
    {
        case SYNCE_INFO_MODE:
            mode = *(enum SYNCE_MODE *)value;

            if (g_synce.mode != (enum SYNCE_MODE)mode)
            {
                g_synce.mode = mode;
                ret = synce_ipc_send_and_wait_ack(value,sizeof(enum SYNCE_MODE), 1,MODULE_ID_HAL, \
                                 MODULE_ID_CLOCK,IPC_TYPE_SYNCE, type, IPC_OPCODE_UPDATE,0);

                if (ret != 0)
                {
                    zlog_debug(CLOCK_DBG_ERROR,"%s[%d] set mode errocode=%d\n", __FUNCTION__, __LINE__, ret);
                    return -1;
                }

                g_synce.clk_force = 0;
                g_synce.clk_manul = 0;
                synce_clear_ssm_global();
                synce_set_clock_run_state(SYNCE_RUN_STATE_FREERUN); //状态变化，都从FREE-RUN开始
                g_synce.iFreeClrTimer = SYNCE_TIMER_FORCEFREE_DELAY;

            }

            break;

        case SYNCE_INFO_SSM_ENABLE:
            ssm_en = *(uint8_t *)value;

            if (g_synce.ssm_enable != ssm_en)
            {
                synce_clear_ssm_global();
                g_synce.ssm_enable = *(uint8_t *)value;
                ret = synce_ipc_send_and_wait_ack(value,sizeof(uint8_t), 1,MODULE_ID_HAL, \
                                                 MODULE_ID_CLOCK,IPC_TYPE_SYNCE, type, IPC_OPCODE_UPDATE,0);

                if (ret != 0)
                {
                    zlog_debug(CLOCK_DBG_ERROR,"%s[%d] set ssm enable errocode=%d\n", __FUNCTION__, __LINE__, ret);
                    return -1;
                }

                synce_set_clock_run_state(SYNCE_RUN_STATE_FREERUN); //状态变化，都从FREE-RUN开始
                g_synce.iFreeClrTimer = SYNCE_TIMER_FORCEFREE_DELAY;
                synce_clear_ssm_global();
            }

            break;

        case SYNCE_INFO_SELECT:
            psynce_clk = (struct synce_clk_t *)value;
            g_synce.clk_select = psynce_clk->ifindex;
            ret = synce_ipc_send_and_wait_ack(value,sizeof(struct synce_clk_t), 1,MODULE_ID_HAL, \
                                   MODULE_ID_CLOCK,IPC_TYPE_SYNCE, type, IPC_OPCODE_UPDATE,0);

            if (ret != 0)
            {
                zlog_debug(CLOCK_DBG_ERROR,"%s[%d] set select errocode=%d\n", __FUNCTION__, __LINE__, ret);
                return -1;
            }

            break;

        case SYNCE_INFO_PREEMPT:
            g_synce.preempt = *(uint8_t *)value;
            ret = synce_ipc_send_and_wait_ack(value,sizeof(uint8_t), 1,MODULE_ID_HAL, \
                                   MODULE_ID_CLOCK,IPC_TYPE_SYNCE, type, IPC_OPCODE_UPDATE,0);

            if (ret != 0)
            {
                zlog_debug(CLOCK_DBG_ERROR,"%s[%d] set preempt errocode=%d\n", __FUNCTION__, __LINE__, ret);
                return -1;
            }

            break;
		case SYNCE_INFO_SET_FREQ_OFFSET:
            ret = synce_ipc_send_and_wait_ack(value,sizeof(int), 1,MODULE_ID_HAL, \
                                   MODULE_ID_CLOCK,IPC_TYPE_SYNCE, type, IPC_OPCODE_UPDATE,0);
            if (ret != 0)
            {
                zlog_debug(CLOCK_DBG_ERROR,"%s[%d] set FREQ_OFFSET errocode=%d\n", __FUNCTION__, __LINE__, ret);
                return -1;
            }

            break;
        default:
            break;
    }

    return 0;
}

/************************************************
 * Function: synce_get_info
 * Input:    type 要获取的参数项
 * Output:   NONE
 * Return:   要获取的参数的数据指针。
 * Description:  根据不同的参数项，发送相应的IPC消息，获取底层硬件信息。
 ************************************************/
void *synce_get_info(enum SYNCE_INFO type)
{
    void *value = NULL;

    switch (type)
    {
        case SYNCE_INFO_GET_GLOBAL :
        case SYNCE_INFO_GET_CLOCK_STATE:
            break;

        default:
            break;
    }

    return value;
}

/************************************************
 * Function: synce_get_priority
 * Input:    ifindex : 接口索引
 * Output:   NONE
 * Return:   接口时钟的优先级
 * Description:  获取接口的优先级
 ************************************************/
uint8_t synce_get_priority(uint32_t ifindex)
{
    struct clock_src_t *pclk_src = NULL;
    uint8_t ssm_pri = 0;
    pclk_src = clock_src_lookup(ifindex);

    if (pclk_src == NULL)
    {
        return ssm_pri;
    }

    if (pclk_src->psynce)
    {
        ssm_pri = pclk_src->psynce->priority;
    }

    return ssm_pri;
}

/************************************************
 * Function: synce_get_quality_level
 * Input:    ifindex : 接口索引
 * Output:   NONE
 * Return:   接口时钟的质量等级值
 * Description:  获取接口的质量等级
 ************************************************/
uint8_t synce_get_quality_level(uint32_t ifindex)
{
    uint8_t ssm_ql = 0, id;
    struct clockif_t *pdate = NULL;
    struct clock_src_t *pclk_src = NULL;
    id = IFM_PORT_ID_GET(ifindex);

    if (IFM_TYPE_IS_METHERNET(ifindex) || IFM_TYPE_IS_TDM(ifindex))
    {
        pclk_src = clock_src_lookup(ifindex);

        if (pclk_src)
        {
            ssm_ql = pclk_src->psynce->recv_ql;
        }
        else
        {
            zlog_debug(CLOCK_DBG_INFO,"%s[%d] clocksrc lookup failed ifindex=%d\n", __FUNCTION__, __LINE__, ifindex);
        }
    }
    else if (IFM_TYPE_IS_CLOCK(ifindex))
    {
        pdate = clockif_lookup(id);

        if (pdate)
        {
            ssm_ql = pdate->ssm_level;
        }
        else
        {
            zlog_debug(CLOCK_DBG_ERROR,"%s[%d] clockif lookup failed ifindex=%d\n", __FUNCTION__, __LINE__, ifindex);
        }
    }

    return ssm_ql;
}

/************************************************
 * Function: synce_auto_select_clk
 * Input:    NONE
 * Output:   NONE
 * Return:   0 : successed -1 : failed
 * Description:  自动模式下，时钟选择算法，
 *            SSM使能时，通过比较各个有效的时钟源的质量等级，确定最优时钟源；
 *            SSM关闭时，通过DPLL选择最优时钟源。
 ************************************************/
static int synce_auto_select_clk(void)
{
    struct synce_clk_t *pclk = NULL;
    uint8_t cur_select_ssm_ql = 0x0f ;
    struct listnode *pnode = NULL;
    uint8_t if_clk_ssm_ql = 0xff;
    struct synce_clk_t clk_info;
    uint8_t sel_flag = 1;
    uint8_t pri_sel = 0, pri_new = 0;
    uint32_t cur_select_src_ifindex = 0;
    zlog_debug(CLOCK_DBG_EVENT,"%s[%d]:enter %s,\n", __FILE__, __LINE__, __func__);

    if (g_synce.ssm_enable)
    {

        for (ALL_LIST_ELEMENTS_RO(&g_synce.clk_list, pnode, pclk))
        {
            if (NULL == pclk)
            {
                continue;
            }

            if_clk_ssm_ql = synce_get_quality_level(pclk->ifindex);

            if (pclk->valid_stat == VALID_STAT_VALID)
            {
                if (sel_flag && if_clk_ssm_ql > SSM_UNK && if_clk_ssm_ql < SSM_DNU)
                {
                    cur_select_src_ifindex = pclk->ifindex;
                    cur_select_ssm_ql = if_clk_ssm_ql;
                    sel_flag = 0;
                    continue;
                }

                if (if_clk_ssm_ql <= SSM_UNK || if_clk_ssm_ql >= SSM_DNU)
                {
                    continue;
                }

                if (if_clk_ssm_ql < cur_select_ssm_ql)
                {
                    cur_select_src_ifindex = pclk->ifindex;
                    cur_select_ssm_ql = if_clk_ssm_ql;
                }
                else if (if_clk_ssm_ql == cur_select_ssm_ql)
                {
                    pri_sel = synce_get_priority(cur_select_src_ifindex);
                    pri_new = synce_get_priority(pclk->ifindex);

                    if (pri_sel > pri_new && pri_sel != 0 && pri_new != 0)
                    {
                        cur_select_src_ifindex = pclk->ifindex;
                    }
                }

            }
        }

        if (cur_select_src_ifindex != g_synce.clk_select)
        {
            g_synce.clk_select = cur_select_src_ifindex;

            if (cur_select_src_ifindex)
            {
                if_clk_ssm_ql = synce_get_quality_level(cur_select_src_ifindex);

                if ((if_clk_ssm_ql > SSM_UNK) && (if_clk_ssm_ql < SSM_DNU))
                {
                    clk_info.ifindex = cur_select_src_ifindex;

                    if (g_synce.cur_clk_ssm_ql != if_clk_ssm_ql)
                    {
                        g_synce.cur_clk_ssm_ql = if_clk_ssm_ql;
                        g_synce.iSendFlag = SYNCE_SSM_EVENT_PKT_SEND_CNT;
                    }
                }
                else
                {
                    //g_synce.cur_clk_ssm_ql = SSM_DNU;
                    clk_info.ifindex = 0;
                }

                synce_set_info(&clk_info, SYNCE_INFO_SELECT);
            }
            else
            {
                //g_synce.cur_clk_ssm_ql = SSM_DNU;
                clk_info.ifindex = 0;
                synce_set_info(&clk_info, SYNCE_INFO_SELECT);
            }
        }
        else
        {
            if (cur_select_src_ifindex) //when clock src not change,need update current ssm ql
            {
                if(cur_select_ssm_ql != g_synce.cur_clk_ssm_ql)
                {
                    g_synce.cur_clk_ssm_ql = cur_select_ssm_ql;
                    g_synce.iSendFlag = SYNCE_SSM_EVENT_PKT_SEND_CNT;
                }
            }
        }
    }
    else
    {
        g_synce.clk_select = g_synce_hal.cur_clk_source.ifindex; //由DPLL选源
    }

    zlog_debug(CLOCK_DBG_EVENT,"%s[%d]:leaver %s,\n", __FILE__, __LINE__, __func__);
    return 0;
}

/************************************************
 * Function: synce_manual_select_clk
 * Input:    NONE
 * Output:   NONE
 * Return:   0 : successed -1 : failed
 * Description:  人工选择模式下，时钟选择算法，
 *            人工选择的时钟源有效时，选择的端口为时钟源
 *            人工选择的时钟源无效时，SSM使能，通过判断SSM质量等级从有效的时钟端口中选择最优时钟源；
 *            SSM关闭时，判断优先级 选择最优时钟源。
 ************************************************/
static int synce_manual_select_clk(void)
{
    struct synce_clk_t *pclk = NULL;
    struct listnode *pnode = NULL;
    uint8_t if_clk_ssm_ql = 0x00;
    struct synce_clk_t clk_info = {0, 0, 0};
    uint8_t sel_flag = 1;
    uint8_t pri_sel = 0, pri_new = 0;
    uint32_t cur_select_src_ifindex = 0;

    if (g_synce.clk_manul == 0)
    {
        return 0;
    }

    if_clk_ssm_ql = synce_get_quality_level(g_synce.clk_manul);

    if (g_synce.ssm_enable)
    {
        if ((synce_get_valid_status(g_synce.clk_manul) == VALID_STAT_VALID)
                && (if_clk_ssm_ql > SSM_UNK) && (if_clk_ssm_ql < SSM_DNU))
        {
            cur_select_src_ifindex = g_synce.clk_manul;
            goto out;
        }
    }
    else
    {
        if (synce_get_valid_status(g_synce.clk_manul) == VALID_STAT_VALID)
        {
            cur_select_src_ifindex = g_synce.clk_manul;
            goto out;
        }
    }

    for (ALL_LIST_ELEMENTS_RO(&g_synce.clk_list, pnode, pclk))
    {
        if (NULL == pclk)
        {
            continue;
        }

        if (pclk->ifindex != g_synce.clk_manul && pclk->valid_stat == VALID_STAT_VALID)
        {
            if (g_synce.ssm_enable)
            {
                if_clk_ssm_ql = synce_get_quality_level(pclk->ifindex);

                if (sel_flag && if_clk_ssm_ql != SSM_UNK && if_clk_ssm_ql != SSM_DNU)
                {
                    cur_select_src_ifindex = pclk->ifindex;
                    g_synce.cur_clk_ssm_ql = if_clk_ssm_ql;
                    sel_flag = 0;
                    continue;
                }

                if (if_clk_ssm_ql <= SSM_UNK || if_clk_ssm_ql >= SSM_DNU)
                {
                    continue;
                }

                if (if_clk_ssm_ql < g_synce.cur_clk_ssm_ql)
                {
                    cur_select_src_ifindex = pclk->ifindex;
                }
                else if (if_clk_ssm_ql == g_synce.cur_clk_ssm_ql)
                {
                    pri_sel = synce_get_priority(cur_select_src_ifindex);
                    pri_new = synce_get_priority(pclk->ifindex);

                    if (pri_sel > pri_new && pri_sel != 0 && pri_new != 0)
                    {
                        cur_select_src_ifindex = pclk->ifindex;
                    }
                }
            }
            else
            {
                if (sel_flag)
                {
                    cur_select_src_ifindex = pclk->ifindex;
                    sel_flag = 0;
                    continue;
                }

                pri_sel = synce_get_priority(cur_select_src_ifindex);
                pri_new = synce_get_priority(pclk->ifindex);

                if (pri_sel > pri_new && pri_sel != 0 && pri_new != 0)
                {
                    cur_select_src_ifindex = pclk->ifindex;
                }
            }
        }
    }

out:

    if (cur_select_src_ifindex != g_synce.clk_select)
    {
        g_synce.clk_select = cur_select_src_ifindex;
        pclk = synce_lookup_clk(g_synce.clk_select);

        if (pclk != NULL)
        {
            clk_info.type   = pclk->type;

            if (g_synce.ssm_enable)
            {
                if_clk_ssm_ql = synce_get_quality_level(g_synce.clk_select);

                if ((if_clk_ssm_ql > SSM_UNK) && (if_clk_ssm_ql < SSM_DNU))
                {
                    clk_info.ifindex = g_synce.clk_select;

                    if (g_synce.cur_clk_ssm_ql != if_clk_ssm_ql)
                    {
                        g_synce.cur_clk_ssm_ql = if_clk_ssm_ql;
                        g_synce.iSendFlag = SYNCE_SSM_EVENT_PKT_SEND_CNT;
                    }
                }
                else
                {
                    clk_info.ifindex = 0;
                }
            }

            else
            {
                clk_info.ifindex = g_synce.clk_select;
            }

            clk_info.status = synce_get_clk_stat(g_synce.clk_select, clk_info.type);
            synce_set_info(&clk_info, SYNCE_INFO_SELECT);
        }
    }
    else
    {
        if(g_synce.clk_select)
        {
            if_clk_ssm_ql = synce_get_quality_level(g_synce.clk_select);
            if(if_clk_ssm_ql != g_synce.cur_clk_ssm_ql)
            {
                g_synce.cur_clk_ssm_ql = if_clk_ssm_ql;
                g_synce.iSendFlag = SYNCE_SSM_EVENT_PKT_SEND_CNT;
            }
        }
    }

    return 0;
}
/************************************************
 * Function: synce_force_select_clk
 * Input:    NONE
 * Output:   NONE
 * Return:   0 : successed -1 : failed
 * Description: q强制设置某一端口为时钟源
 ************************************************/
static int synce_force_select_clk(void)
{
    struct synce_clk_t *pclk = NULL;
    struct synce_clk_t clk_info = {0, 0, 0};
    uint8_t if_clk_ssm_ql = 0x00;
    uint32_t cur_select_src_ifindex = 0;

    if (g_synce.clk_force == 0)
    {
        return 0;
    }

    if (g_synce.ssm_enable)
    {
        pclk = synce_lookup_clk(g_synce.clk_force);

        if (pclk != NULL)
        {
            clk_info.type   = pclk->type;
            clk_info.status = synce_get_clk_stat(pclk->ifindex, pclk->type);
            if_clk_ssm_ql = synce_get_quality_level(pclk->ifindex);

            if ((if_clk_ssm_ql > SSM_UNK) && (if_clk_ssm_ql < SSM_DNU) &&
                    (synce_get_valid_status(g_synce.clk_force) == VALID_STAT_VALID))
            {
                clk_info.ifindex = pclk->ifindex;

                if (g_synce.cur_clk_ssm_ql != if_clk_ssm_ql)
                {
                    g_synce.cur_clk_ssm_ql = if_clk_ssm_ql;
                    g_synce.iSendFlag = SYNCE_SSM_EVENT_PKT_SEND_CNT;
                }
            }
            else
            {
                clk_info.ifindex = 0;
            }

            if (g_synce.clk_select != clk_info.ifindex)
            {
                g_synce.clk_select = clk_info.ifindex;
                synce_set_info(&clk_info, SYNCE_INFO_SELECT);
            }
        }
    }
    else
    {
        pclk = synce_lookup_clk(g_synce.clk_force);

        if (pclk != NULL)
        {
            clk_info.type = pclk->type;

            if (synce_get_valid_status(g_synce.clk_force) == VALID_STAT_VALID)
            {
                cur_select_src_ifindex = g_synce.clk_force;
            }
            else
            {
                cur_select_src_ifindex = 0;
            }

            clk_info.ifindex = cur_select_src_ifindex;
            clk_info.status = synce_get_clk_stat(cur_select_src_ifindex, clk_info.type);

            if (cur_select_src_ifindex != g_synce.clk_select)
            {
                g_synce.clk_select = cur_select_src_ifindex;
                synce_set_info(&clk_info, SYNCE_INFO_SELECT);
            }
        }
    }

    return 0;
}

/************************************************
 * Function: synce_select_clock
 * Input:    entype : 不同的触发时钟选择的事件类型
 * Output:   NONE
 * Return:   0 : successed -1 : failed
 * Description:  通过不同的事件类型，触发不同的时钟选择算法。
 ************************************************/
int synce_select_clock(enum SYNCE_SEL_CLK_EVENT entype)                          /* 选择时钟 */
{
    switch (entype)
    {
        case SYNCE_EVENT_SEL_SSM_PKT_EVENT:
        case SYNCE_EVENT_SEL_IF_DOWN:
        case SYNCE_EVENT_SEL_PORT_ADD:
        case SYNCE_EVENT_SEL_PORT_DELETE:
            if (g_synce.mode == SYNCE_MODE_MANUAL)
            {
                synce_manual_select_clk();
            }
            else if (g_synce.mode == SYNCE_MODE_AUTO)
            {
                synce_auto_select_clk();
            }
            else if (g_synce.mode == SYNCE_MODE_FORCE)
            {
                synce_force_select_clk();
            }

            break;

        case SYNCE_EVENT_SEL_CMD_MANUAL_SELECT:
            synce_manual_select_clk();
            break;

        case SYNCE_EVENT_SEL_CMD_FORCE_SELECT:
            synce_force_select_clk();
            break;

        case SYNCE_EVENT_SEL_CMD_AUTO_SELECT:
            synce_auto_select_clk();
            break;

        default :
            break;
    }

    return 0;
}

/************************************************
 * Function: synce_add_clock_to_list
 * Input:    pclk
 * Output:   NONE
 * Return:   0 : successed -1 : failed
 * Description:  添加一个时钟源数据到时钟源链表
 ************************************************/
static int synce_add_clock_to_list(struct synce_clk_t *pclk)
{
    if (NULL == pclk)
    {
        return -1;
    }

    if (synce_lookup_clk(pclk->ifindex) == NULL)
    {
        listnode_add(&g_synce.clk_list, pclk);
    }

    return 0;
}
/************************************************
 * Function: synce_add_clock
 * Input:    pclk 时钟源数据指针
 * Output:   NONE
 * Return:   0 : successed -1 : failed
 * Description:  添加一个时钟端口，并发送端口信息到HAL层
 ************************************************/
int synce_add_clock(struct synce_clk_t *pclk)      /* 添加一个时钟源 */
{
    struct synce_clk_t *pclk_if = NULL;
    int ret;
    SynceClkICData synce_hal_entry;
    struct gpnPortInfo gPortInfo;
    pclk_if = XCALLOC(MTYPE_CLOCK_ENTRY, sizeof(struct synce_clk_t));

    if ((NULL == pclk_if) || (NULL == pclk))
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s: calloc memory failed or clk source is NULL !\n", __FUNCTION__);
        return -1;
    }

    pclk_if->ifindex    = pclk->ifindex;
    pclk_if->type   = pclk->type;
    pclk_if->status = pclk->status;

    if (synce_add_clock_to_list(pclk_if))
    {
        XFREE(MTYPE_CLOCK_ENTRY, pclk_if);
        zlog_debug(CLOCK_DBG_ERROR,"%s: add clock list failed!\n", __FUNCTION__);
        return -1;
    }

    memset(&synce_hal_entry, 0, sizeof(SynceClkICData));
    synce_hal_entry.port_num = IFM_PORT_ID_GET(pclk->ifindex);
    synce_hal_entry.if_index = pclk->ifindex;
    synce_hal_entry.if_type = pclk->type;
    synce_hal_entry.ic_clk_enable = SYNCE_ENABLE;
    synce_hal_entry.ic_priority = synce_get_priority(pclk->ifindex);
    ret = synce_ipc_send_and_wait_ack(&synce_hal_entry, sizeof(SynceClkICData),1,MODULE_ID_HAL,MODULE_ID_CLOCK,IPC_TYPE_SYNCE,SYNCE_INFO_ADD,IPC_OPCODE_ADD,0);

    if(ret < 0)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__,ret);
        return -1;
    }

    //synce_select_clock(SYNCE_EVENT_SEL_PORT_ADD);
    memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = synce_get_clk_alarm_type(pclk->ifindex);
	gPortInfo.iIfindex = pclk->ifindex;
    ipran_alarm_port_register(&gPortInfo);
    return 0;

}

/************************************************
 * Function: synce_delete_clock
 * Input:    pclk 时钟源数据指针
 * Output:   NONE
 * Return:   0 : successed -1 : failed
 * Description:  删除一个时钟端口，并发送端口信息到HAL层
 ************************************************/
int synce_delete_clock(struct synce_clk_t *pclk)  /* 删除一个时钟源 */
{
    struct listnode *pclk_list_node = NULL;
    struct synce_clk_t *p_clk_del = NULL;
    int ret;
    SynceClkICData synce_hal_entry;
    p_clk_del = synce_lookup_clk(pclk->ifindex);

    if (NULL == p_clk_del)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d] can not find the synce source! \n", __FUNCTION__, __LINE__);
        return -1;
    }

    if (g_synce.mode != SYNCE_MODE_AUTO)
    {
        if (g_synce.clk_force == pclk->ifindex)
        {
            g_synce.clk_force = 0;
        }

        if (g_synce.clk_manul == pclk->ifindex)
        {
            g_synce.clk_manul = 0;
        }
    }

    if (p_clk_del->synce_alarm_flag & SYNCE_ALARM_FLAG_CLK_INVALID)
    {
        ClockAlarmReport(SYNCE_ALARM_FLAG_CLK_INVALID, GPN_SOCK_MSG_OPT_CLEAN, p_clk_del->ifindex, 0);
        p_clk_del->synce_alarm_flag &= ~SYNCE_ALARM_FLAG_CLK_INVALID;
    }

    if (p_clk_del->synce_alarm_flag & SYNCE_ALARM_FLAG_FREQ_OVERFLOW)
    {
        ClockAlarmReport(SYNCE_ALARM_FLAG_FREQ_OVERFLOW, GPN_SOCK_MSG_OPT_CLEAN, p_clk_del->ifindex, 0);
        p_clk_del->synce_alarm_flag &= ~SYNCE_ALARM_FLAG_FREQ_OVERFLOW;
    }

    pclk_list_node = listnode_lookup(&g_synce.clk_list, p_clk_del);

    if (NULL == pclk_list_node)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d] the pclk_list_node is NULL! \n", __FUNCTION__, __LINE__);
        return -1;
    }

    list_delete_node(&g_synce.clk_list, pclk_list_node);
    synce_hal_entry.port_num = IFM_PORT_ID_GET(pclk->ifindex);
    synce_hal_entry.if_index = pclk->ifindex;
    synce_hal_entry.if_type = pclk->type;
    synce_hal_entry.ic_clk_enable = SYNCE_DISABLE;
    synce_hal_entry.ic_priority = SYNCE_DISABLE;
    ret = synce_ipc_send_and_wait_ack(&synce_hal_entry, sizeof(SynceClkICData), 1,MODULE_ID_HAL, \
                                     MODULE_ID_CLOCK,IPC_TYPE_SYNCE, SYNCE_INFO_DELETE, IPC_OPCODE_DELETE,0);

    if (ret != 0)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return -1;
    }

    //synce_select_clock(SYNCE_EVENT_SEL_PORT_DELETE);
    return 0;
}

/************************************************
 * Function: synce_get_valid_status
 * Input:    ifindex : 接口索引  clk_type : 时钟接口类型
 * Output:   NONE
 * Return:   接口时钟的是否有效状态
 * Description:  获取当前端口时钟有效状态
 ************************************************/

enum VALID_STAT synce_get_clk_stat(uint32_t ifindex, enum SYNCE_CLK_TYPE clk_type)
{
    int i;

    if (ifindex == 0)
    {
        return CLOCK_STAT_INVALID;
    }

    for (i = 1; i <= SYNCE_CLK_NUM; i++)
    {
        if ((g_synce_hal.cur_ic_info[i].if_index == ifindex))
        {
            return g_synce_hal.cur_ic_info[i].ic_clkstat;
        }
    }

    zlog_debug(CLOCK_DBG_ERROR,"%s[%d]: can't find the clk ifindex 0x%x clktype: %d\n", __FUNCTION__, __LINE__, ifindex, clk_type);
    return CLOCK_STAT_INVALID;
}
/************************************************
 * Function: synce_get_valid_status
 * Input:    ifindex : 接口索引  clk_type : 时钟接口类型
 * Output:   NONE
 * Return:   接口时钟的是否有效状态
 * Description:  获取当前端口时钟有效状态
 ************************************************/

E_SYNCE_CLK_TYPE synce_get_clk_type(uint32_t ifindex)
{
    E_SYNCE_CLK_TYPE clk_type = SYNCE_CLK_TYPE_INVALID;

    if (IFM_TYPE_IS_METHERNET(ifindex))
    {
        clk_type = SYNCE_CLK_TYPE_SYNCE;
    }
    else if (IFM_TYPE_IS_TDM(ifindex) || IFM_TYPE_IS_E1(ifindex))
    {
        clk_type = SYNCE_CLK_TYPE_E1;
    }
    else if (IFM_TYPE_IS_CLOCK(ifindex))
    {
        clk_type = SYNCE_CLK_TYPE_CLOCKIF;
    }
    else if (IFM_TYPE_IS_STM(ifindex))
    {
        clk_type = SYNCE_CLK_TYPE_STM;
    }

    return clk_type;
}

/************************************************
 * Function: synce_lookup_clk
 * Input:    ifindex : 接口索引
 * Output:   NONE
 * Return:   时钟接口指针   NULL : 未查到有效接口
 * Description:  获取当前时钟链表中某一个时钟接口的信息。
 ************************************************/
struct synce_clk_t *synce_lookup_clk(uint32_t ifindex)
{
    struct synce_clk_t *pclk = NULL;
    struct listnode *pnode = NULL;

    for (ALL_LIST_ELEMENTS_RO(&g_synce.clk_list, pnode, pclk))
    {
        if (pclk->ifindex == ifindex)
        {
            return pclk;
        }
    }

    return NULL;
}

/************************************************
 * Function: synce_global_config_init
 * Input:    NONE
 * Output:   NONE
 * Return:   NONE
 * Description:  初始化synce全局数据结构，如需更新底层硬件，则调用synce_set_info 做相应配置。
 ************************************************/
static void synce_global_config_init(void)
{
    int mode = 0 ;
    g_synce.mode = SYNCE_MODE_UNKNOWN;
    mode = SYNCE_CONFIG_MODE_DEFAULT;
    synce_set_info(&mode, SYNCE_INFO_MODE);

    g_synce.ssm_enable = SYNCE_CONFIG_SSM_ENALBE_DEFAULT;
    synce_set_info(&g_synce.ssm_enable, SYNCE_INFO_SSM_ENABLE);

    g_synce.preempt = SYNCE_CONFIG_PREEMPT_DEFAULT;
    synce_set_info(&g_synce.preempt, SYNCE_INFO_PREEMPT);

    g_synce.hold_time    = SYNCE_CONFIG_HOLDTIME_DEFAULT;
    g_synce.wtr          = SYNCE_CONFIG_WTR_DEFAULT;
    g_synce.ssm_interval = SYNCE_CONFIG_SSM_INTERVAL_DEFAULT;
    g_synce.ssm_timeout  = SYNCE_CONFIG_SSM_TIMEOUT_DEFAULT;

}

/************************************************
 * Function: synce_init
 * Input:    NONE
 * Output:   NONE
 * Return:   NONE
 * Description:  syncE模块初始化操作，包括命令行、全局变量、报文类型注册，以及运行定时执行线程。
 ************************************************/
void synce_init(void)
{
    synce_cmd_init();
    synce_global_config_init();
    synce_ssm_pkt_register();
    devm_comm_get_id(1, 0, MODULE_ID_CLOCK, &synce_dev_id);

}

/************************************************
 * Function: synce_set_clock_run_state
 * Input:    state : 要强制设置的状态值
 * Output:   NONE
 * Return:   NONE
 * Description:  强制设置当前时钟运行在某一状态
 ************************************************/
int synce_set_clock_run_state(enum SYNCE_STAT state)
{
    int ret;
    ret = synce_ipc_send_and_wait_ack(&state, sizeof(enum SYNCE_STAT), 1,MODULE_ID_HAL, \
                                     MODULE_ID_CLOCK,IPC_TYPE_SYNCE, SYNCE_INFO_FORCE_STATE, IPC_OPCODE_UPDATE,0);

    if (ret != 0)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d] ipc_send_hal errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return -1;
    }

    return 0;
}

/************************************************
 * Function: synce_clock_update_if_status
 * Input:    NONE
 * Output:   NONE
 * Return:   NONE
 * Description:  更新外时钟接口的状态，外时钟接口的link/down状态，即为时钟的有效无效状态。
 ************************************************/
int synce_clock_update_if_status(void)
{
    int index;
    struct clockif_t *pclk;

    for (index = 1; index < CLOCKIF_NUM_MAX; index++)
    {
        pclk = clockif_lookup(index);

        if (pclk != NULL)
        {
            if (synce_get_clk_stat(pclk->ifindex, SYNCE_CLK_TYPE_CLOCKIF) == VALID_STAT_VALID)
            {
                pclk->status = 1;
            }
            else
            {
                pclk->status = 0;
            }

            if (pclk->ql_set_flag == 0) //未手动配置质量等级，用FPGA中读取到的收到的质量等级值
            {
                if (pclk->ssm_level != g_synce_hal.cur_extern_clk_recv_ql)
                {
                    pclk->ssm_level = g_synce_hal.cur_extern_clk_recv_ql;
                    //synce_select_clock(SYNCE_EVENT_SEL_IF_DOWN);
                    g_synce.iSendFlag = SYNCE_SSM_EVENT_PKT_SEND_CNT;
                }
            }
        }
    }

    return 0;
}


/*get synce local global cfg*/
unsigned int synce_snmp_global_cfg_get(struct ipc_msghdr_n *phdr)
{
    int     ret;
    ret = ipc_send_reply_n2(&g_synce, sizeof(struct synce_t),1,phdr->sender_id,MODULE_ID_CLOCK,\
                            IPC_TYPE_SYNCE,phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);

    if (ret < 0)
    {
        zlog_debug(CLOCK_DBG_ERROR,"[%s %d]ERROR: ipc_send_reply ret(%d)", __FUNCTION__, __LINE__, ret);

        return -1;
    }

    return 0;
}

unsigned int synce_snmp_clk_src_next_ifindex_get(struct ipc_msghdr_n *phdr)
{
    int     ret;
    struct synce_clk_t *pclk = NULL;
    struct listnode *pnode = NULL;
    uint32_t index_next = 0;
    int index_next_flag = 0;
    char str_value[32];

    for (ALL_LIST_ELEMENTS_RO(&g_synce.clk_list, pnode, pclk))
    {
        if (phdr->msg_index == 0)
        {
            index_next = pclk->ifindex;
            break;
        }
        else
        {
            if (index_next_flag == 1)
            {
                index_next = pclk->ifindex;
                break;
            }

            if (pclk->ifindex == phdr->msg_index)
            {
                index_next_flag = 1;
            }
        }
    }

    ifm_get_name_by_ifindex(index_next, str_value);
    zlog_debug(CLOCK_DBG_INFO,"[%s %d]: index_next: 0x%x : %s\n", __FUNCTION__, __LINE__, index_next, str_value);
    ret = ipc_send_reply_n2(&index_next, sizeof(uint32_t),1,phdr->sender_id,MODULE_ID_CLOCK,\
                            IPC_TYPE_SYNCE,phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);

    if (ret < 0)
    {
        zlog_debug(CLOCK_DBG_ERROR,"[%s %d]ERROR: ipc_send_reply ret(%d)", __FUNCTION__, __LINE__, ret);

        return -1;
    }

    return 0;
}
unsigned int synce_snmp_clk_src_info_get(struct ipc_msghdr_n *phdr)
{
    int     ret;
    struct synce_snmp_clock_src_t clk_src_info;
    clk_src_info.ifindex = phdr->msg_index;
    clk_src_info.priority = synce_get_priority(phdr->msg_index);
    clk_src_info.quality_level = synce_get_quality_level(phdr->msg_index);
    clk_src_info.status = synce_get_lock_status(phdr->msg_index);
    clk_src_info.valid_stat = synce_get_valid_status(phdr->msg_index);
    ret = ipc_send_reply_n2(&clk_src_info, sizeof(struct synce_snmp_clock_src_t),1,phdr->sender_id, \
                  MODULE_ID_CLOCK,IPC_TYPE_SYNCE,phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);

    if (ret < 0)
    {
        zlog_debug(CLOCK_DBG_ERROR,"[%s %d]ERROR: ipc_send_reply ret(%d)", __FUNCTION__, __LINE__, ret);

        return -1;
    }

    return 0;
}



