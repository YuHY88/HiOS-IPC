/**********************************************************
* file name: gpnTimerService.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-08-13
* function: 
*    define module cycle timer and once timer
* modify:
*
***********************************************************/
#ifndef _GPN_TIMER_SERVICE_H_
#define _GPN_TIMER_SERVICE_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnCommList.h"
#include "socketComm/gpnDebugFuncApi.h"


#define GPN_TIMER_GEN_OK 				GPN_SOCK_SYS_OK
#define GPN_TIMER_GEN_ERR 				GPN_SOCK_SYS_ERR

#define GPN_TIMER_GEN_ENABLE 			GPN_SOCK_SYS_ENABLE
#define GPN_TIMER_GEN_DISABLE 			GPN_SOCK_SYS_DISABLE

#if 0
#define GPN_TIMER_PRINT(level, info...) GEN_SYS_DEBUG_PRINT((level), info)
#else
#define GPN_TIMER_PRINT(level, info...)
#endif

#define GPN_TIMER_MAN_EGP 				GEN_SYS_DEBUG_EGP
#define GPN_TIMER_MAN_SVP 				GEN_SYS_DEBUG_SVP
#define GPN_TIMER_MAN_CMP 				GEN_SYS_DEBUG_CMP
#define GPN_TIMER_MAN_CUP 				GEN_SYS_DEBUG_CUP
#define GPN_TIMER_MAN_UD5				GEN_SYS_DEBUG_UD5
#define GPN_TIMER_MAN_UD4				GEN_SYS_DEBUG_UD4
#define GPN_TIMER_MAN_UD3				GEN_SYS_DEBUG_UD3
#define GPN_TIMER_MAN_UD2				GEN_SYS_DEBUG_UD2
#define GPN_TIMER_MAN_UD1				GEN_SYS_DEBUG_UD1
#define GPN_TIMER_MAN_UD0				GEN_SYS_DEBUG_UD0


typedef void (* baseTimerFuncPtr)(INT32);
typedef void (* timerOutFuncPtr)(UINT32, UINT32);

/* 假定tick时长*/
#define GPN_SYS_TICK_10MS				1

#if GPN_SYS_TICK_10MS
/* 每个tick所含的ms*/
#define GPN_SYS_PER_TICK_MS				10

#define DEF_TIMER_UNITS					1
/* 10ms的tick数*/
#define DEF_GPN_TIME_UNIT_10MS			1
#define DEF_GPN_TIME_UNIT_10ms			1
/* 100ms的tick数*/
#define DEF_GPN_TIME_UNIT_100MS			10
#define DEF_GPN_TIME_UNIT_100ms			10
/* 每秒的tick数*/
#define DEF_GPN_TIME_UNIT_SECOND		100
#define DEF_GPN_TIME_UNIT_second		100
/* 每分钟的tick数*/
#define DEF_GPN_TIME_UNIT_MINUTE		( DEF_GPN_TIME_UNIT_SECOND * 60)
#define DEF_GPN_TIME_UNIT_minute		( DEF_GPN_TIME_UNIT_SECOND * 60)
/* 每小时的tick数*/
#define DEF_GPN_TIME_UNIT_HOUR			(DEF_GPN_TIME_UNIT_MINUTE * 60)
#define DEF_GPN_TIME_UNIT_hour			(DEF_GPN_TIME_UNIT_MINUTE * 60)
/* 每天的tick数*/
#define DEF_GPN_TIME_UNIT_DAY			(DEF_GPN_TIME_UNIT_HOUR * 24)
#define DEF_GPN_TIME_UNIT_day			(DEF_GPN_TIME_UNIT_HOUR * 24)
/* 每月的tick数*/
#define DEF_GPN_TIME_UNIT_MONTH			(DEF_GPN_TIME_UNIT_DAY * 30)
#define DEF_GPN_TIME_UNIT_month			(DEF_GPN_TIME_UNIT_DAY * 30)
/* 每年的tick数*/
#define DEF_GPN_TIME_UNIT_YEAR			(DEF_GPN_TIME_UNIT_MONTH * 12)
#define DEF_GPN_TIME_UNIT_year			(DEF_GPN_TIME_UNIT_MONTH * 12)
#else

#error Must define default system ticks number per second

#endif

#define GPN_TIME_TO_TICKS(timeLen, timeUnit)	((timeLen) * (DEF_GPN_TIME_UNIT_##timeUnit))

#define DEF_GPN_TIME_MSEC_IN_SEC		1000
#define DEF_GPN_TIME_USEC_IN_SEC		1000000

/*user opt type define*/
#define GPN_TIMR_USER_OPT_NULL			0
#define GPN_TIMR_USER_OPT_ADD			1
#define GPN_TIMR_USER_OPT_DEL			2
#define GPN_TIMR_USER_OPT_ALL			3

/*timer type define*/
#define GPN_TIMR_TYPE_CYC				1
#define GPN_TIMR_TYPE_TIMES				2
#define GPN_TIMR_TYPE_ONCE				3

#define GPN_PROCESS_TIMER_MAX_NUM		64
#define GPN_PROCESS_TIMER_LEN			((GPN_PROCESS_TIMER_MAX_NUM)/32)

#define GPN_TIMER_PROCESS_PATH_LEN 		256
#define GPN_TIMER_PROCESS_CMD_LEN 		256

/*base-timer-fifo size*/
#define GPN_TIMER_BASE_TIMER_FIFO		6

/*clc source type define */
#define GPN_TIMER_CLC_SRC_POCIX			1
#define GPN_TIMER_CLC_SRC_SELECET		2

typedef UINT32 (*sigTimerCallback)(UINT32, UINT32);

typedef UINT32 (*calChgCallback)(UINT32, UINT32);

typedef struct _gpnTimerInfo_
{	
	LIST actTimer;
	UINT32 actTimerNum;
	LIST idleTimer;
	UINT32 idleTimerNum;

	UINT32 en;
	
	/*if SIGALRM be catched for several times when process be blocked(
	   or busy) in one of funtions(like system call), SIGALRM will not triger
	   base-timer-msg send, so porcess will not passby a huge time gap
	   in a short realy time gap*/
	UINT32 timerMsgFifo;

	UINT32 sigTickNum;
	struct timeval sigPeriod;
	struct timeval sigTStamp;
	
	/*sig function callback*/
	sigTimerCallback sigCall;
	UINT32 sigCallPara1;
	UINT32 sigCallPara2;
	
	UINT32 baseTickNum;
	struct timeval basePeriod;
	struct timeval procTStamp;
	
	/*callback for calendar time change */
	calChgCallback calCall;
	UINT32 calCallPara1;
	UINT32 calCallPara2;

	/* timer source type : POCIX  or selecet() */
	UINT32 clcSrcType;
	
	/* for POCIX timer use */
	INT32 timerMsgid;

	/* for SELECET timer use */
	
}gpnTimerInfo;

#define GPN_TIMER_USER_OPT_STRUCT_BLEN	32 /*sizeof(timeOptMsg)*/
typedef struct _timeOptMsg_
{
	UINT32 opt;
	UINT32 tType;
	UINT32 timerId;
	struct timeval period;
	UINT32 life;
	timerOutFuncPtr tOFunPtr;
	UINT32 para1;
	UINT32 para2;
}timeOptMsg;/*sizeof(timeOptMsg) = 32*/

typedef struct _timeNode_
{
	NODE node;
	UINT32 opt;
	UINT32 tType;
	UINT32 timerId;
	struct timeval period;
	UINT32 life;
	timerOutFuncPtr tOFunPtr;
	UINT32 para1;
	UINT32 para2;
	/*never extravasation*/
	struct timeval timerStamp;
}timeNode;

UINT32 gpnTimerGetProcessPath(char *buff, UINT32 length);
UINT32 gpnTimerFilePathCheck(const char *path);
UINT32 gpnTimerSpaceInit(void);
/********************************************************************************/
/*                                                            MSG QUEN OPT                                                         */
/********************************************************************************/
UINT32 gpnTimerMsgQuenForUserOptInit(void);
UINT32 gpnTimerMsgQuenSend(UINT32 msgType, timeOptMsg *pMsg);
UINT32 gpnTimerMsgQuenReceive(UINT32 msgType, timeOptMsg *pMsg);
UINT32 gpnTimerMsgQuenDelete(void);
UINT32 gpnTimerAddUserOpt(UINT32 timerType, UINT32 period, UINT32 life,
				timerOutFuncPtr handler, UINT32 para1, UINT32 para2);
UINT32 gpnTimerDeleteUserOpt(INT32 timerId);

/********************************************************************************/
/*                                                            REAL TIMER OPT                                                       */
/********************************************************************************/
UINT32 gpnRealTimerAdd(timeOptMsg *ptimerOpt);
UINT32 gpnRealTimerDelete(timeOptMsg *ptimerOpt);
void gpnTimerSigEventProc(INT32 signo);
void gpnBaseTimerBingo(void);

/********************************************************************************/
/*                                                    TIMER CALLBACK OPT                                                       */
/********************************************************************************/
UINT32 gpnTimerAddSigTimerCallBack(sigTimerCallback handler, UINT32 para1, UINT32 para2);
UINT32 gpnTimerDeleteSigTimerCallBack(void);
UINT32 gpnTimerAddCalendarChangeCallBack(calChgCallback handler, UINT32 para1, UINT32 para2);
UINT32 gpnTimerDeleteCalendarChangeCallBack(void);
/********************************************************************************/
/*                                                    POCIX TIMER OPT                                                              */
/********************************************************************************/
UINT32 gpnTimerPOCIXTimerCreat(UINT32 sigLen, baseTimerFuncPtr timerProc);
void gpnTimerPOCIXTimerSigactionInit(baseTimerFuncPtr timerProc);
void gpnTimerPOCIXTimerBaseTimeInit(UINT32 tickNum);
void gpnTimerPOCIXTimerAdjustTime(void);
void gpnGetBaseTimer(struct itimerspec *pIt);
void gpnSetBaseTimer(struct itimerspec it);

/********************************************************************************/
/*                                                    SELECT TIMER OPT                                                            */
/********************************************************************************/
UINT32 gpnTimerSelectTimerNoBlockBingoCheck(void);
UINT32 gpnTimerSelectTimerBlockBingoCheck(struct timeval *bTime);

/********************************************************************************/
/*                                                    BASE Timer OPT                                                                */
/********************************************************************************/
UINT32 gpnBaseCycleTimerEnable(void);
UINT32 gpnBaseCycleTimerDisable(void);
void gpnTimerAdjustBaseTime(UINT32 oldTime, UINT32 newTime);
UINT32 gpnTimerSignalCycleCreat(UINT32 sigLen, UINT32 baseLen, baseTimerFuncPtr timerProc);


/********************************************************************************/
/*                                                   GPN TIMER INIT                                                                 */
/********************************************************************************/
UINT32 gpnTimerInit(UINT32 sigLen, UINT32 baseLen);
INT32 gpnTimerFree(void);

/********************************************************************************/
/*                                                    TIME CALCULATE                                                              */
/********************************************************************************/
UINT32 gpnUsecTimeIntervalCheck(struct timeval *firstUsec, struct timeval *secondUsec, UINT32 usecVal);
UINT32 gpnUsecTimePassCheck(struct timeval *firstUsec, struct timeval *secondUsec, UINT32 usecVal);
UINT32 gpnUsecTimePassCalculate(struct timeval *firstUsec, struct timeval *secondUsec, struct timeval *passTime);
INT32 gpnUsecTimeCmp(struct timeval * firstUsec, struct timeval * secondUsec);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_TIMER_SERVICE_H_*/


