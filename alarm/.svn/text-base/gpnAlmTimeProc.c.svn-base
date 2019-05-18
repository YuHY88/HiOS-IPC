/**********************************************************
* file name: gpnAlmTimeProc.c
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2015-05-21
* function: 
*    define alarm Time proc Api
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_TIME_PROC_C_
#define _GPN_ALM_TIME_PROC_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "gpnAlmScan.h"
#include "gpnAlmTimeProc.h"
#include "gpnAlmDSheetProc.h"

#include "socketComm/gpnTimerService.h"

#include "socketComm/gpnSockProtocolMsgDef.h"
#include "socketComm/gpnSockCommRoleMan.h"
extern sockCommData gSockCommData;
extern stEQUAlmProcSpace *pgstAlmPreScanWholwInfo;

/*extern void alarmAttributeCfgTable_ifctx_cache_timeout_clear();
extern void alarmCurrDataSheetTable_ifctx_cache_timeout_clear();
extern void alarmHistDataSheetTable_ifctx_cache_timeout_clear();
extern void alarmPortMonitorCfgTable_ifctx_cache_timeout_clear();
extern void eventDataSheetTable_ifctx_cache_timeout_clear();*/

/*static void gpnAlmSubAgentTimeOutProc()
{
	alarmAttributeCfgTable_ifctx_cache_timeout_clear();
	alarmCurrDataSheetTable_ifctx_cache_timeout_clear();
	alarmHistDataSheetTable_ifctx_cache_timeout_clear();
	alarmPortMonitorCfgTable_ifctx_cache_timeout_clear();
	eventDataSheetTable_ifctx_cache_timeout_clear();

	return;
}*/

UINT32 gpnAlmCalendarTimeChangeProc(UINT32 oldTime, UINT32 newTime)
{
	INT32 span;

	//printf("%s : oldTime %10d newTime %10d\n\r", __FUNCTION__, oldTime, newTime);
	
	/* modify gpn_alarm process time */
	pgstAlmPreScanWholwInfo->almTaskTime = newTime;
	pgstAlmPreScanWholwInfo->expTime.tv_sec = newTime;

	/* modify time record in databass */
	span = (INT32)newTime - (INT32)oldTime;
	if(abs(span) > GPN_ALM_TIM_CLC_SPAN_LIMIT)
	{
		/*curr alarm db product time renew */
		gpnAlmDspTimeChangeProc(span);

		/*histroy alarm db product time renew */

		/*subagent timeout clear when calendar time change so mutch*/
		//gpnAlmSubAgentTimeOutProc();
	}

	return GPN_ALM_TIM_OK;
}

UINT32 gpnAlmTimeAdjust(UINT32 cycSec)
{
	/*	cyc : time adjust cyc, used for calculate expect time;
	 	tolerate : time get from device compare whit expect time;*/
	UINT32 reVal;
	static UINT32 errEvt;
	struct timeval nowTime;

	gettimeofday(&nowTime, NULL);

	/*first time adjust */
	if(pgstAlmPreScanWholwInfo->almTaskTime == 0)
	{
		/* init gpn_alarm process time */
		pgstAlmPreScanWholwInfo->almTaskTime = nowTime.tv_sec;
		
		pgstAlmPreScanWholwInfo->expTime.tv_sec = nowTime.tv_sec + cycSec;
		pgstAlmPreScanWholwInfo->expTime.tv_usec = nowTime.tv_usec;

		return GPN_ALM_TIM_OK;
	}

	/*calculate nowTime compare expTime : nowTime should allmost same with expTime */
	/*0 ~ slight change ~ severe change ~ endless : ingore -sync right now - conuter errEvt */
	/*GPN_ALM_TIM_SLIGHT_CHANG_TOLERATE : sys time changed not so much, just renew process time and expTime */
	reVal = gpnUsecTimeIntervalCheck(&nowTime, &pgstAlmPreScanWholwInfo->expTime,\
				(GPN_ALM_TIM_SLIGHT_CHANG_TOLERATE)*(DEF_GPN_TIME_USEC_IN_SEC));
	if(reVal != GPN_ALM_TIM_OK)
	{
		/*GPN_ALM_TIM_SEVERE_CHANG_TOLERATE : sys time changed so much, just counter errEvt and wait timeOut */
		reVal = gpnUsecTimeIntervalCheck(&nowTime, &pgstAlmPreScanWholwInfo->expTime,\
				(GPN_ALM_TIM_SEVERE_CHANG_TOLERATE)*(DEF_GPN_TIME_USEC_IN_SEC));
		if(reVal != GPN_ALM_TIM_OK)
		{
			errEvt++;
			
			/*out of interval SEVERE_CHANG_TOLERATE, reVal == GPN_STAT_TIME_GEN_ERR */
			if(errEvt > GPN_ALM_TIM_CLC_CHG_DELAYED)
			{
				/* log */
				GPN_ALM_TIM_PRINT(GPN_ALM_TIM_SVP, "%s : out of SEVERE_CHANG_TOLERATE(%d), now(%08x) expect(%08x)\n\r",\
					__FUNCTION__, GPN_ALM_TIM_SEVERE_CHANG_TOLERATE, (UINT32)nowTime.tv_sec,\
					(UINT32)pgstAlmPreScanWholwInfo->expTime.tv_sec);

				gpnAlmCalendarTimeChangeProc(pgstAlmPreScanWholwInfo->almTaskTime, nowTime.tv_sec);
				
				/* force reset gpn_alarm process time to nowTime */
				pgstAlmPreScanWholwInfo->almTaskTime = nowTime.tv_sec;
				
				pgstAlmPreScanWholwInfo->expTime.tv_sec = nowTime.tv_sec + cycSec;
				pgstAlmPreScanWholwInfo->expTime.tv_usec = nowTime.tv_usec;

			}
			else
			{
				/* wait for timeOut or other func modify right time */
				pgstAlmPreScanWholwInfo->almTaskTime += cycSec;
				
				pgstAlmPreScanWholwInfo->expTime.tv_sec += cycSec;
			}	
		}
		else
		{
			errEvt = 0;
			
			/*out of interval SLIGHT_CHANG_TOLERATE, reVal == GPN_STAT_TIME_GEN_OK */
			GPN_ALM_TIM_PRINT(GPN_ALM_TIM_SVP, "%s : out ofSLIGHT_CHANG_TOLERATE(%d), now(%08x) expect(%08x)\n\r",\
					__FUNCTION__, GPN_ALM_TIM_SLIGHT_CHANG_TOLERATE, (UINT32)nowTime.tv_sec,\
					(UINT32)pgstAlmPreScanWholwInfo->expTime.tv_sec);

			/* adjust time right */
			pgstAlmPreScanWholwInfo->almTaskTime = nowTime.tv_sec;
				
			pgstAlmPreScanWholwInfo->expTime.tv_sec = nowTime.tv_sec + cycSec;
			pgstAlmPreScanWholwInfo->expTime.tv_usec = nowTime.tv_usec;		
		}
	}
	else
	{
		errEvt = 0;
		
		/*in interval SLIGHT_CHANG_TOLERATE, reVal == GPN_STAT_TIME_GEN_OK*/
		GPN_ALM_TIM_PRINT(GPN_ALM_TIM_CUP, "%s : %08x, time just normal\n\r",\
			__FUNCTION__, (UINT32)nowTime.tv_sec);

		/* keep in normal step */
		pgstAlmPreScanWholwInfo->almTaskTime += cycSec;
		pgstAlmPreScanWholwInfo->expTime.tv_sec += cycSec;
		
	}

	return GPN_ALM_TIM_OK;
}



void gpnSockCalendarTimeChgTest(UINT32 p1, UINT32 p2)
{
	INT32 oldTime;
	INT32 newTime;

	oldTime = time(NULL);
	newTime = oldTime - 1000;
	gpnSockSpMsgTxBaseTimerSyn2OtherModule(\
		&(gSockCommData.gstFdSet), GPN_SOCK_ROLE_ALM, oldTime, newTime);
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_TIME_PROC_C_*/
