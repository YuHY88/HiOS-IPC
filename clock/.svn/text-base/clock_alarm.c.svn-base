/******************************************************************************
 * Filename: xxx_alarm.c
 *  Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.12.19  cuiyudong created
 *
******************************************************************************/

#include <string.h>

#include <lib/command.h>
#include <lib/thread.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/linklist.h>
#include <lib/ifm_common.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>

#include <lib/alarm.h>


#include "clock_if.h"
#include "clock_alarm.h"
#include "synce/synce.h"

extern struct thread_master *clock_master;

unsigned int synce_get_clk_alarm_type(unsigned int ifindex)
{
    unsigned int iPortType = 0, iPortAlarmType = 0;
    iPortType = IFM_TYPE_ID_GET(ifindex);

    switch (iPortType)
    {
        case IFNET_TYPE_ETHERNET :
        case IFNET_TYPE_GIGABIT_ETHERNET:
        case IFNET_TYPE_XGIGABIT_ETHERNET:
            iPortAlarmType = IFM_ETH_CLC_LINE_TYPE;
            break;

        case IFNET_TYPE_TDM :
        case IFNET_TYPE_STM :
        case IFNET_TYPE_E1  :
            iPortAlarmType = IFM_ETH_CLC_SUB_TYPE;
            break;

        case IFNET_TYPE_CLOCK :
            iPortAlarmType = IFM_ETH_CLC_OUT_TYPE;
            break;

        default:
            iPortAlarmType = IFM_ETH_CLC_IN_TYPE;
            break;
    }

    return iPortAlarmType;
}

void ClockAlarmReport(int iAlmType, int iReport, unsigned int ifindex, int iAlmPara)
{

    struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
    gPortInfo.iAlarmPort = synce_get_clk_alarm_type(ifindex);
	gPortInfo.iIfindex = ifindex;

    switch (iAlmType)
    {
        case SYNCE_ALARM_FLAG_CLK_INVALID:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_CLC_INVALID, iReport);
            break;

        case SYNCE_ALARM_FLAG_FREQ_OVERFLOW:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_FREQ_OVERFLOW, iReport);
            break;

        case SYNCE_ALARM_FLAG_LOSELOCK:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_CLC_SYN_LOS, iReport);

            break;

        case SYNCE_ALARM_FLAG_QL_SWITCH:
            ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_FUN_NEW_QL_CHANGE, iReport);
            break;

        case SYNCE_ALARM_FLAG_SYNC_CHANAGE:
            ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_FUN_NEW_SYN_SRC, iReport);
            break;

        default:
            break;
    }
}

uint32_t clock_gpn_function_init(void)
{
    struct gpnPortInfo gPortInfo;

    memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
    gPortInfo.iAlarmPort = IFM_ETH_CLC_IN_TYPE;
    gPortInfo.iIfindex = 0;
    ipran_alarm_port_register(&gPortInfo);
    return 1;
}





