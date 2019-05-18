/**********************************************************
* file name: gpnStatCollectProc.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-25
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_TIME_PROC_C_
#define _GPN_STAT_TIME_PROC_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <sys/time.h>

#include "socketComm/gpnTimerService.h"
#include "gpnStatTimeProc.h"
#include "gpnStatDataStruct.h"
#include "gpnLog/gpnLogFuncApi.h"


extern stEQUStatProcSpace *pgstEQUStatProcSpace;


/**************************************************************/
/*				GPN_STAT module relationship					*/
/*						snmp_api							*/
/*						   /								*/
/*					     func_api							*/
/*						/								*/
/*				 maiProc_modu								*/
/*				   /   		   \								*/
/*		    timeProc_modu      data_modu  						*/
/*			\	      	    |      	      /							*/
/*					statType								*/
/*					    |									*/
/*					sysType								*/
/*														*/
/**************************************************************/

extern stEQUStatProcSpace *pgstEQUStatProcSpace;

UINT32 gpnStatTimeAdjust(UINT32 cycSec)
{
	/*	cyc : time adjust cyc, used for calculate expect time;
	 	tolerate : time get from device compare whit expect time;*/
	UINT32 reVal;
	static struct timeval expTime;
	struct timeval nowTime;

	gettimeofday(&nowTime, NULL);

	/*first time adjust, init expTime to nowTime */
	if(expTime.tv_sec == 0)
	{
		expTime.tv_sec = nowTime.tv_sec;
		expTime.tv_usec = nowTime.tv_usec;
	}
	
	/*whatever, renew stat sys time */
	pgstEQUStatProcSpace->statTaskInfo.statSysTime = nowTime.tv_sec;

	/*calculate nowTime compare expTime : nowTime should allmost same with expTime */
	/*GPN_STAT_SYS_TIME_SLIGHT_CHANG_TOLERATE : sys time changed so much, GPN_STAT's time base info should be reset right now */
	reVal = gpnUsecTimeIntervalCheck(&nowTime, &expTime,\
				(GPN_STAT_SYS_TIME_SLIGHT_CHANG_TOLERATE)*(DEF_GPN_TIME_USEC_IN_SEC));
	if(reVal != GPN_STAT_TIME_GEN_OK)
	{
		/*GPN_STAT_SYS_TIME_SEVERE_CHANG_TOLERATE : sys time changed so much, GPN_STAT's time base info should be reset right now */
		reVal = gpnUsecTimeIntervalCheck(&nowTime, &expTime,\
				(GPN_STAT_SYS_TIME_SEVERE_CHANG_TOLERATE)*(DEF_GPN_TIME_USEC_IN_SEC));
		if(reVal != GPN_STAT_TIME_GEN_OK)
		{
			/*out of interval SEVERE_CHANG_TOLERATE, reVal == GPN_STAT_TIME_GEN_ERR */
			
			/*log TIME_SEVERE_CHANG happen */
			gpnLog(GPN_LOG_L_INFO, "%s : out of SEVERE_CHANG_TOLERATE(%d), now(%08x) expect(%08x)\n\r",\
				__FUNCTION__, GPN_STAT_SYS_TIME_SEVERE_CHANG_TOLERATE, (UINT32)nowTime.tv_sec, (UINT32)expTime.tv_sec);
		}
		else
		{
			/*out of interval SLIGHT_CHANG_TOLERATE, reVal == GPN_STAT_TIME_GEN_OK */

			/*log TIME_SLIGHT_CHANG happen */
			gpnLog(GPN_LOG_L_INFO, "%s : out ofSLIGHT_CHANG_TOLERATE(%d), now(%08x) expect(%08x)\n\r",\
				__FUNCTION__, GPN_STAT_SYS_TIME_SLIGHT_CHANG_TOLERATE, (UINT32)nowTime.tv_sec, (UINT32)expTime.tv_sec);
		}
	}
	else
	{
		/*in interval SLIGHT_CHANG_TOLERATE, reVal == GPN_STAT_TIME_GEN_OK*/
			
		/*just do nothing */
		/*GPN_STAT_TIME_PRINT(GPN_STAT_TIME_CUP, "%s : %08x, time just normal\n\r",\
				__FUNCTION__, (UINT32)nowTime.tv_sec);*/
	}
	
	/*real-time not chang so much, little than  */
	expTime.tv_sec = nowTime.tv_sec + cycSec;
	expTime.tv_usec = nowTime.tv_usec;

	return reVal;
}

UINT32 gpnStatHistPushBingoTimeInit(UINT32 sTime, UINT32 cycSecs, UINT32 *prelSTime, UINT32 *ppushTime)
{
	struct timeval nowTime;
	UINT32 gap;
	
	/*assert */
	if( (prelSTime == NULL) ||\
		(ppushTime == NULL) )
	{
		return GPN_STAT_TIME_GEN_ERR;
	}

	/*abtain a xxx time */
	gettimeofday(&nowTime, NULL);
	//printf("hist push bingo time init sTime(%d) nowTime.tv_sec(%d)\n\r", sTime, nowTime.tv_sec);
	if((INT32)sTime <= nowTime.tv_sec)
	{
		/*sTime small than nowTime */
		/*--c----c----c----c---
		    --s-------n----------
		    -------s--n-p----p---
		**************************
		    --c----c----c----c---
		    --s--n---------------
		    --s--n-p----p----p---
		**************************
		    --c----c----c----c---
		    (s)n------------------ 
		    (s)n----p----p----p---
		**************************/
		gap = (nowTime.tv_sec - sTime)/cycSecs;
		*prelSTime = sTime + gap*cycSecs;
		*ppushTime = *prelSTime + cycSecs + GPN_STAT_PUSH_STACK_DELAY;
	}
	else /*if(sTime > nowTime.tv_sec)*/
	{
		/*sTime big than nowTime */
		/* --c----c----c----c---
		    ----n--s------------- 
		    --s-n--p----p----p--- 
		**************************
		    --c----c----c----c---
		    ----n-------s-------- 
		    --s-n--p----p----p--- 
		**************************/
		//gap = (sTime- nowTime.tv_sec)/cycSecs + 1;
		//*prelSTime = sTime - gap*cycSecs;
		*prelSTime = sTime - cycSecs;    // modify by geqian  2016.1.6             EZVIEW   wanted that start time is monitor time
		*ppushTime = *prelSTime + cycSecs + GPN_STAT_PUSH_STACK_DELAY;
	}

	GPN_STAT_TIME_PRINT(GPN_STAT_TIME_CUP, "%s : now(%08x) rSTime(%08x) push(%08x)\n\r",\
		__FUNCTION__, (UINT32)nowTime.tv_sec, *prelSTime, *ppushTime);
	return GPN_STAT_TIME_GEN_OK;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_STAT_TIME_PROC_C_*/


