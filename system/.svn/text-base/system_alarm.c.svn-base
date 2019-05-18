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
#include <lib/ifm_common.h>
#include <lib/alarm.h>
#include <lib/module_id.h>
#include <lib/memshare.h>
#include <lib/gpnSocket/socketComm/gpnStatTypeDef.h>
#include <lib/gpnSocket/socketComm/gpnSockStatMsgDef.h>

#include "system_usage.h"
#include "system_alarm.h"

extern struct thread_master *system_master;
extern sys_global_conf_t sys_conf;
void SystemAlarmReport(int iAlmType, int iReport, unsigned int ifindex, int iAlmPara)
{
    struct gpnPortInfo gPortInfo;
    memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
    gPortInfo.iAlarmPort = IFM_SOFT_TYPE;
    gPortInfo.iIfindex = 0;

    switch (iAlmType)
    {
        case SYSTEM_CPU_USAGE_HIGH_ALARM:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_CPU_BUSY, iReport);
            break;

        case SYSTEM_MEM_USAGE_DRY_ALARM:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_MEM_DRY, iReport);
            break;

        case SYSTEM_MEM_USAGE_HIGH_ALARM:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_MEM_USAGE_H, iReport);
            break;

        default:
            break;
    }
}

static int32_t gpnSystemSoftCPUMemoryGetRsp (gpnSockMsg *pgnSystemMsgSp)
{
    gpnStatSoftData ssData;		//struct for stat
	optObjOrient portIndex;
    if(NULL == pgnSystemMsgSp)
    {
        return -1;
    }
	portIndex.devIndex   = pgnSystemMsgSp->iMsgPara1;
	portIndex.portIndex  = pgnSystemMsgSp->iMsgPara2;
	portIndex.portIndex3 = pgnSystemMsgSp->iMsgPara3;
	portIndex.portIndex4 = pgnSystemMsgSp->iMsgPara4;
	portIndex.portIndex5 = pgnSystemMsgSp->iMsgPara5;
	portIndex.portIndex6 = pgnSystemMsgSp->iMsgPara6;

	ssData.statSoftCpuUtileza = 10000 - sys_conf.cpu_info[0].idle_usage;
    ssData.statSoftMemUtileza = sys_conf.mem_info.mem_usage;
	
	/* 调用库中的Rsp接口函数，向性能模块回复数据 */
	gpnSockStatMsgApiTxSdmSoftStatGetRsp(&portIndex, GPN_STAT_T_SOFT_TYPE, \
						&ssData, GPN_STAT_PARA_SOFT_STAT_DATA_SIZE);
    return 0;
}
static int32_t gpnSystemEquCPUMemoryGetRsp (gpnSockMsg *pgnSystemMsgSp)
{
    gpnStatEquData seData;		//struct for stat
	optObjOrient portIndex;
    if(NULL == pgnSystemMsgSp)
    {
        return -1;
    }

	portIndex.devIndex   = pgnSystemMsgSp->iMsgPara1;
	portIndex.portIndex  = pgnSystemMsgSp->iMsgPara2;
	portIndex.portIndex3 = pgnSystemMsgSp->iMsgPara3;
	portIndex.portIndex4 = pgnSystemMsgSp->iMsgPara4;
	portIndex.portIndex5 = pgnSystemMsgSp->iMsgPara5;
	portIndex.portIndex6 = pgnSystemMsgSp->iMsgPara6;
	
    memset(&seData, 0, sizeof(gpnStatEquData));
    seData.statEquTemp = 0xffffffff;
    seData.statEquCpuUtileza = 10000 - sys_conf.cpu_info[0].idle_usage;
	
	/* 调用库中的Rsp接口函数，向性能模块回复数据 */
	gpnSockStatMsgApiTxSdmEquStatGetRsp(&portIndex, GPN_STAT_T_EQU_TYPE, \
						&seData, GPN_STAT_PARA_EQU_STAT_DATA_SIZE);
    return 0;
}
static int32_t gpnSystemEnableRsp (gpnSockMsg *pgnSystemMsgSp)
{
	UINT32 statType;
	UINT32 enStat;
	optObjOrient portIndex;
	
	memcpy(&(portIndex.devIndex), &(pgnSystemMsgSp->iMsgPara1), sizeof(objLogicDesc));
	statType = pgnSystemMsgSp->iMsgPara7;

	switch(statType)
	{
		case GPN_STAT_T_EQU_TYPE:
		case GPN_STAT_T_SOFT_TYPE:
			enStat = 1;
			break;

		default:
			enStat = 0;
			break;
	}

	gpnSockStatMsgApiPortStatMonEnableRsp(&portIndex, statType, enStat);

	return 1;
}

int32_t gpnSystemCoreProc (gpnSockMsg *pgnNsmMsgSp, uint32_t len)
{
    
    switch(pgnNsmMsgSp->iMsgType)
    {       
        case GPN_STAT_MSG_PORT_STAT_MON_ENABLE:     //ENABLE
        	/*printf("%s(%d) : enable--->index(%08x|%08x), statType(%x), T(%d)\n\r",\
				__FUNCTION__, __LINE__, pgnNsmMsgSp->iMsgPara2,\
				pgnNsmMsgSp->iMsgPara3, pgnNsmMsgSp->iMsgPara7,\
				pgnNsmMsgSp->iMsgPara8);*/
			 
			gpnSystemEnableRsp(pgnNsmMsgSp);
			break;
            break;
        
        case GPN_STAT_MSG_PORT_STAT_MON_DISABLE:    //DISABLE
            break;

        case GPN_STAT_MSG_SOFT_STAT_GET:                //数据获取
            //封装一个应答函数，获取模块自己的数据并调用应答接口函数
			/*printf("%s(%d) : get--->index(%08x|%08x), statType(%x), T(%d)\n\r",\
				__FUNCTION__, __LINE__, pgnNsmMsgSp->iMsgPara2,\
				pgnNsmMsgSp->iMsgPara3, pgnNsmMsgSp->iMsgPara7,\
				pgnNsmMsgSp->iMsgPara8);*/
			
			gpnSystemSoftCPUMemoryGetRsp(pgnNsmMsgSp);
            break;
        case GPN_STAT_MSG_EQU_STAT_GET:                //数据获取
            //封装一个应答函数，获取模块自己的数据并调用应答接口函数
            gpnSystemEquCPUMemoryGetRsp(pgnNsmMsgSp);
            break;

        default:
            break;
    }
	return 1;
}


uint32_t system_gpn_function_init(void)
{
    struct gpnPortInfo gPortInfo;

    memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_SOFT_TYPE;
	gPortInfo.iIfindex = 0;
    ipran_alarm_port_register(&gPortInfo);
    return 1;
}





