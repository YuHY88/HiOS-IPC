/******************************************************************************
 * Filename: synce_ssm.c
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: functions for syncE protocal and ssm pkt .
 *
 * History:
 * 2016.10.13  cuiyudong created
 *
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "lib/log.h"
#include "lib/types.h"
#include "lib/pkt_buffer.h"
#include "lib/module_id.h"
#include "lib/msg_ipc.h"
#include "lib/ifm_common.h"
#include "lib/hash1.h"
#include "lib/linklist.h"
#include "lib/pkt_type.h"
#include "lib/alarm.h"
#include "lib/pkt_buffer.h"

#include "clock/clock_src.h"
#include "clock/clock_if.h"
#include "clock/clock_cmd.h"
#include "clock/clock_alarm.h"


#include "synce_ssm.h"
#include "synce.h"

extern unsigned char synce_alarm_global_flag;

/************************************************
 * Function: synce_ssm_pkt_register
 * Input:    none
 * Output:   none
 * Return:   none
 * Description:  synce SSM报文注册
 ************************************************/
void synce_ssm_pkt_register(void)
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));

    proto.ethreg.ethtype = SSM_ETH_TYPE;
    proto.ethreg.sub_ethtype = SSM_ETH_SUBTYPE;

    pkt_register(MODULE_ID_CLOCK, PROTO_TYPE_ETH, &proto);

    return;
}
/************************************************
 * Function: synce_ssm_port_up
 * Input:    psynce_info L2接口下的数据结构指针  ifindex 接口索引
 * Output:   none
 * Return:   none
 * Description:  synce 端口UP事件 并触发状态机的状态切换
 ************************************************/
int synce_ssm_port_up(struct synce_if *psynce_info, uint32_t ifindex)
{
    struct synce_clk_t synce_clk;

    if (NULL == psynce_info)
    {
        return 0;
    }

    psynce_info->port_status = 1;
    synce_ssm_fsm(EVENT_UP, psynce_info, ifindex);

    memset(&synce_clk, 0, sizeof(struct synce_clk_t));
    synce_clk.ifindex = ifindex;
    synce_add_clock(&synce_clk); //端口UP时重新下发一遍端口的配置，使其底层FPGA的配置能够恢复 add by cuiyudong 20170705
    return 0;
}

/************************************************
 * Function: synce_ssm_port_down
 * Input:    psynce_info L2接口下的数据结构指针  ifindex 接口索引
 * Output:   none
 * Return:   none
 * Description:  synce 端口DOWN事件 并触发状态机的状态切换
 ************************************************/
int synce_ssm_port_down(struct synce_if *psynce_info, uint32_t ifindex)
{
    if (NULL == psynce_info)
    {
        return 0;
    }

    psynce_info->port_status = 0;
    synce_ssm_fsm(EVENT_DOWN, psynce_info, ifindex);
    return 0;
}
int flag_recv_ssm_event = 0; // can use this paramater report alarm ql change event
/************************************************
 * Function: synce_ssm_port_down
 * Input:    pif : L2接口数据结构指针  ppkt 报文数据结构指针
 * Output:   none
 * Return:   none
 * Description:  synce SSM报文接收处理函数，根据不同的报文数据，触发不同的时钟选择操作 EVENT 报文事件，质量等级发生变化。
 ************************************************/
int synce_ssm_recv(struct pkt_buffer *ppkt)
{
    struct synce_if     *synce_port;
    unsigned char       *buf = NULL;
    struct ssm_pkt pkt_ssm;
    struct clock_src_t *pclk_src = NULL;

    if (ppkt == NULL)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d]:leave %s,ppkt or pif is NULL\n", __FILE__, __LINE__, __func__);
        return -1;
    }

    pclk_src = clock_src_lookup(ppkt->in_ifindex);

    if (pclk_src == NULL)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d]:leave %s,ppkt or pif is NULL\n", __FILE__, __LINE__, __func__);
        return -1;
    }

    if (!g_synce.ssm_enable)
    {
        return 0;
    }

    buf = ppkt->data;

    synce_port = pclk_src->psynce;
    memcpy(&pkt_ssm, buf, sizeof(pkt_ssm));

    if ((synce_port != NULL) && (buf[0] == SSM_ETH_SUBTYPE))
    {
        if (buf[6] & 0x08) //EVENT PKT
        {
            zlog_debug(CLOCK_DBG_PACKAGE,"%s[%d]: %s,recv event ssm pkt ifindex : 0x%x\n", __FILE__, __LINE__, __func__, ppkt->in_ifindex);
            synce_ssm_fsm(EVENT_RCV_EVENT_PKT, synce_port, ppkt->in_ifindex);
        }
        else //information pkt
        {
            synce_ssm_fsm(EVENT_RECV_INFO_PKT, synce_port, ppkt->in_ifindex);
        }

        if (synce_port->recv_ql != (pkt_ssm.ssm_code & 0x0F))
        {
            synce_port->recv_ql = pkt_ssm.ssm_code & 0x0F;
            flag_recv_ssm_event = 1;
            //synce_select_clock(SYNCE_EVENT_SEL_SSM_PKT_EVENT);
        }

        synce_port->pkt_recv_cnt++;
    }

    return 0;
}

/************************************************
 * Function: synce_encap_frame
 * Input:    pssm_pkt : SSM报文数据结构指针  pkt_type 报文类型  ql : 要发送的质量等级值
 * Output:   none
 * Return:   0 successed  -1 ；failed
 * Description:  synce SSM报文数据填充
 ************************************************/
static int synce_encap_frame(struct ssm_pkt *pssm_pkt, enum SSM_PKT_TYPE pkt_type, unsigned char ql)
{
    pssm_pkt->subtype = 0x0A;
    pssm_pkt->itu_oui[0] = 0x00;
    pssm_pkt->itu_oui[1] = 0x19;
    pssm_pkt->itu_oui[2] = 0xA7;
    pssm_pkt->itu_subtype = htons(0x0001);

    if (pkt_type == PKT_INFO)
    {
        pssm_pkt->flag = 0x10;
    }
    else
    {
        pssm_pkt->flag = 0x18;
    }

    pssm_pkt->tlv_type = 0x01;
    pssm_pkt->tlv_len  = htons(0x04);
    pssm_pkt->ssm_code = ql;

    return 0;
}

/************************************************
 * Function: synce_ssm_send
 * Input:    psynce_if : L2接口下的synce数据结构指针  ifindex 接口索引 pkt_type : 报文类型
 * Output:   none
 * Return:   0 successed  -1 ；failed
 * Description:  synce SSM报文发送，从某个接口下发送ssm报文
 ************************************************/
int synce_ssm_send(struct synce_if *psynce_if, uint32_t ifindex, enum SSM_PKT_TYPE pkt_type) /* 发送 ssm 报文 */
{
    int ret = 0;
    struct ssm_pkt ssm_pkt_send;
    union pkt_control pktcontrol;
    uint8_t ssm_mac[SSM_MAC_ADDR_LEN] = SSM_MAC;

    memset(&ssm_pkt_send, 0, sizeof(struct ssm_pkt));
    memset(&pktcontrol, 0, sizeof(pktcontrol));
    memcpy(pktcontrol.ethcb.dmac, ssm_mac, SSM_MAC_ADDR_LEN);
    pktcontrol.ethcb.ethtype = SSM_ETH_TYPE;
    pktcontrol.ethcb.sub_ethtype = SSM_ETH_SUBTYPE;
    pktcontrol.ethcb.smac_valid = 0;
    pktcontrol.ethcb.is_changed = 1;
    pktcontrol.ethcb.ifindex = ifindex;
    pktcontrol.ethcb.cos = 5;

    if (ifindex == g_synce.clk_select)
    {
        synce_encap_frame(&ssm_pkt_send, pkt_type, SSM_DNU);
    }
    else
    {
        synce_encap_frame(&ssm_pkt_send, pkt_type, g_synce.cur_clk_ssm_ql);
    }

    ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &ssm_pkt_send, sizeof(struct ssm_pkt));

    if (ret != 0)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d]:leave %s,fail to send pkt\n", __FILE__, __LINE__, __func__);
        return -1;
    }

    psynce_if->pkt_send_cnt++;

    return ret ;
}

/************************************************
 * Function: synce_ssm_fsm
 * Input:    en_type : 触发状态机切换的事件类型  psynce_if : L2接口下的synce数据结构指针  ifindex 接口索引
 * Output:   none
 * Return:   0 successed  -1 ；failed
 * Description:  synce 某个以太接口的SSM状态切换，根据不同的事件，进行相应的状态切换
 ************************************************/
int synce_ssm_fsm(enum EVENT_SSM_PKT en_type, struct synce_if *psynce_if, uint32_t ifindex)  /* ssm 状态机处理 */
{
    if (NULL == psynce_if)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d]:leave %s,fail to recv synce_if ptr\n", __FILE__, __LINE__, __func__);
        return -1;
    }

    switch (en_type)
    {
        case EVENT_RECV_INFO_PKT :
            if (psynce_if->status != SSM_STATUS_NORMAL)
            {
                psynce_if->status = SSM_STATUS_NORMAL;
            }

            psynce_if->timeout_cnt = 0;
            break;

        case EVENT_RCV_EVENT_PKT :
            if (psynce_if->status != SSM_STATUS_NORMAL)
            {
                psynce_if->status = SSM_STATUS_NORMAL;
            }

            //synce_select_clock(SYNCE_EVENT_SEL_SSM_PKT_EVENT);
            break;

        case EVENT_UP :
            psynce_if->status = SSM_STATUS_TIMEOUT;
            psynce_if->timeout_cnt = 0;
            break;

        case EVENT_DOWN :
            psynce_if->status = SSM_STATUS_INVALID;
            psynce_if->recv_ql = SSM_DNU;
            psynce_if->timeout_cnt = 0;
            //synce_select_clock(SYNCE_EVENT_SEL_IF_DOWN);
            break;

        case EVENT_TIMEOUT :
            psynce_if->status = SSM_STATUS_TIMEOUT;
            psynce_if->recv_ql = SSM_DNU;
            psynce_if->timeout_cnt = 0;
            //synce_select_clock(SYNCE_EVENT_SEL_IF_DOWN);
            break;
    }

    return 0;
}
int synce_ql_val_to_str(char *str, unsigned char ql)
{
    int ret = 0;

    switch (ql)
    {
        case SSM_DNU :
            strcpy(str, SSM_DNU_STR);
            break;

        case SSM_PRC :
            strcpy(str, SSM_PRC_STR);
            break;

        case SSM_SEC :
            strcpy(str, SSM_SEC_STR);
            break;

        case SSM_SSUA :
            strcpy(str, SSM_SSUA_STR);
            break;

        case SSM_SSUB :
            strcpy(str, SSM_SSUB_STR);
            break;

        case SSM_UNK :
            strcpy(str, SSM_UNK_STR);
            break;

        default:
            strcpy(str, SSM_UNK_STR);
            break;
    }

    return ret;
}
int synce_sa_val_to_str(char *str, unsigned char sa)
{
    int ret = 0;

    switch (sa)
    {
        case 4 :
            strcpy(str, "sa4");
            break;

        case 3 :
            strcpy(str, "sa5");
            break;

        case 2 :
            strcpy(str, "sa6");
            break;

        case 1 :
            strcpy(str, "sa7");
            break;

        case 0 :
            strcpy(str, "sa8");
            break;

        default:
            strcpy(str, "sa4");
            break;
    }

    return ret;
}

static time_t history_time_counter = 0;
/************************************************
 * Function: synce_protocol_run_stat_can
 * Input:    none
 * Output:   none
 * Return:   0 successed  -1 ；failed
 * Description:  时钟运行状态的切换，保持时间的逻辑，以及是否强制某一时期在自由震荡。
 ************************************************/
void synce_protocol_run_stat_can(void)
{
    enum SYNCE_STAT new_state = SYNCE_RUN_STATE_AUTO;
    time_t current_time_counter = 0;
    new_state = (enum SYNCE_STAT)g_synce_hal.cur_clock_state; /*获取底层硬件的时钟状态*/

    if ((g_synce.cur_clk_stat != new_state) && (g_synce.iWaitTimer <= 0))
    {

        if (new_state != SYNCE_RUN_STATE_LOCKED)
        {
            if (!(synce_alarm_global_flag & SYNCE_ALARM_FLAG_LOSELOCK))
            {
                ClockAlarmReport(SYNCE_ALARM_FLAG_LOSELOCK, GPN_SOCK_MSG_OPT_RISE, 0, 0);
                synce_alarm_global_flag |= SYNCE_ALARM_FLAG_LOSELOCK;
            }
        }
        else
        {
            if (synce_alarm_global_flag & SYNCE_ALARM_FLAG_LOSELOCK)
            {
                ClockAlarmReport(SYNCE_ALARM_FLAG_LOSELOCK, GPN_SOCK_MSG_OPT_CLEAN, 0, 0);
                synce_alarm_global_flag &= ~SYNCE_ALARM_FLAG_LOSELOCK;
            }
        }

        g_synce.cur_clk_stat = new_state;

        switch (new_state)
        {
            case SYNCE_RUN_STATE_FREERUN:
            case SYNCE_RUN_STATE_LOCKED:
                if ((new_state == SYNCE_RUN_STATE_FREERUN) && (g_synce.clk_select != 0))
                {
                    g_synce.clk_select = 0;
                    g_synce.cur_clk_ssm_ql = SSM_DNU;
                }

                if (g_synce.iFreeClrTimer > 0)
                {
                    g_synce.iFreeClrTimer -= SYNCE_SEND_MSG_INTERVAL;

                    if (g_synce.iFreeClrTimer <= 0)
                    {
                        g_synce.iFreeClrTimer = 0;

                        synce_set_clock_run_state(SYNCE_RUN_STATE_AUTO); /*清除FREE强制*/
                        //synce_select_clock(SYNCE_EVENT_SEL_IF_DOWN); /* 恢复自动以后，做一次选源操作*/
                    }
                }

                if (g_synce.iHoldOverTimer > 0)
                {
                    g_synce.iHoldOverTimer = 0;
                }

                break;

            case SYNCE_RUN_STATE_HOLDOVER:

                if (g_synce.iHoldOverTimer <= 0)
                {
                    /*启动保持超时*/
                    g_synce.iHoldOverTimer = g_synce.hold_time;
                    time(&history_time_counter);
                }

                break;

            case SYNCE_RUN_STATE_LOSELOCK:      /*过度状态,什么也不做*/
            default:
                break;
        }
    }
    else if (g_synce.iWaitTimer <= 0)
    {
        switch (new_state)
        {
            case SYNCE_RUN_STATE_FREERUN:
            case SYNCE_RUN_STATE_LOCKED:
                if ((new_state == SYNCE_RUN_STATE_FREERUN) && (g_synce.clk_select != 0))
                {
                    g_synce.clk_select = 0;
                }

                if (g_synce.iFreeClrTimer > 0)
                {
                    g_synce.iFreeClrTimer -= SYNCE_SEND_MSG_INTERVAL;

                    if (g_synce.iFreeClrTimer <= 0)
                    {
                        g_synce.iFreeClrTimer = 0;

                        synce_set_clock_run_state(SYNCE_RUN_STATE_AUTO);    /*清除FREE强制*/
                    }
                }

                if (g_synce.iHoldOverTimer > 0)
                {
                    g_synce.iHoldOverTimer = 0;
                }

                break;

            case SYNCE_RUN_STATE_HOLDOVER:

                if (g_synce.iHoldOverTimer > 0)
                {
                    time(&current_time_counter);
                    
                    g_synce.iHoldOverTimer -= (int)(current_time_counter - history_time_counter);
                    history_time_counter = current_time_counter;

                    if (g_synce.iHoldOverTimer <= 0)
                    {
                        g_synce.iHoldOverTimer = 0;

                        g_synce.iFreeClrTimer = SYNCE_TIMER_FORCEFREE_DELAY;
                        /*启动free强制延迟*/
                        synce_set_clock_run_state(SYNCE_RUN_STATE_FREERUN);  /*FREE强制*/

                        if (g_synce.ssm_enable)
                        {
                            g_synce.cur_clk_ssm_ql = SSM_DNU;
                            g_synce.uc_send_ql = g_synce.cur_clk_ssm_ql;
                            g_synce.iSendFlag = SYNCE_SSM_EVENT_PKT_SEND_CNT;
                            g_synce.iFlgClrTimer = SYNCE_TIMER_EVENT_CHAGE_DELAY;

                        }

                    }
                }

                break;

            case SYNCE_RUN_STATE_LOSELOCK:      /*过度状态,什么也不做*/

            default:
                break;
        }
    }
    else
    {
        g_synce.cur_clk_stat = SYNCE_RUN_STATE_HOLDOVER;

        if (g_synce.iHoldOverTimer <= 0)
        {
            /*启动保持超时*/
            g_synce.iHoldOverTimer = g_synce.hold_time;
            time(&history_time_counter);
        }
    }

}


