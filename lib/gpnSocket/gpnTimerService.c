/**********************************************************
* file name: gpnTimerService.c
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-08-13
* function: 
*    define module cycle timer and once timer
* modify:
*    huahuan liuyf 2013-12-06
***********************************************************/
#ifndef _GPN_TIMER_SERVICE_C_
#define _GPN_TIMER_SERVICE_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include<sys/ipc.h>
#include<sys/msg.h>
#include <stdio.h>

#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>



#include "socketComm/gpnTimerService.h"
#include "socketComm/gpnSockCommRoleMan.h"
#include "socketComm/gpnSpecialWarrper.h"
#include "socketComm/gpnUnifyPathDef.h"


gpnTimerInfo *pgtimerInfo;
static timer_t g_timerid;

const char timerMsgQ[] = "/tmp/gpn_timer";

extern sockCommData gSockCommData;

UINT32 gpnTimerGetProcessPath(char *buff, UINT32 length)
{
	INT32 i;
	INT8 path[GPN_TIMER_PROCESS_PATH_LEN]={0};   
	INT8 filepath[GPN_TIMER_PROCESS_PATH_LEN]={0};   
	INT8 cmd[GPN_TIMER_PROCESS_CMD_LEN]={0};   
	FILE* fp=NULL; 

	/*get process path*/
	sprintf(filepath, "/proc/%d", getpid());

	if( -1 != chdir(filepath) )   
	{   
		/*memset(filepath, 0, GPN_TIMER_PROCESS_PATH_LEN);*/
		
		/*snprintf(cmd,256, "ls -l | grep exe | awk '{print $10}'");*/
		/*snprintf(cmd, GPN_TIMER_PROCESS_CMD_LEN, "ls -l | grep exe | awk '{print $11}'");*/
		snprintf(cmd, GPN_TIMER_PROCESS_CMD_LEN, "ls -l | grep exe ");
		if(NULL == (fp=popen(cmd,"r")))   
		{   
			perror("popen failed.../n");   
			return GPN_TIMER_GEN_ERR;   
		}   
		
		if( NULL == gpnFgets(path, sizeof(path)/sizeof(path[0]), fp) )   
		{   
	 		perror("fgets error.../n");   
			pclose(fp);   
			return GPN_TIMER_GEN_ERR;   
		}
		pclose(fp);

		GPN_TIMER_PRINT(GPN_TIMER_MAN_EGP, "%s(%d) : get process path \"%s\"\n\r",\
			__FUNCTION__, getpid(), path);

		/*fine last '/'*/
		for(i=strlen(path);i>=0;i--)
		{
			if(path[i] == '/')
			{
				snprintf(buff, length, "%s/%s", timerMsgQ, &(path[i+1]));
				return GPN_TIMER_GEN_OK;
			}
			if(path[i] == '\n')
			{
				path[i] = '\0';
			}
		}
	}
	
	snprintf(buff, length, "%s/%d", timerMsgQ, getpid());
	/*LOG*/
	printf("%s : could not get process(%d)'s path, use %s\n\r",\
	 	__FUNCTION__, getpid(), buff);
	
	return GPN_TIMER_GEN_ERR;
}

UINT32 gpnTimerFilePathCheck(const char *path)
{
	/*file path check*/
	if(access(path, 0) == -1)
	{
		if(mkdir(path, 0777) == -1)
		{
			/*mkdir err*/
			printf("%s(%d) : mkdir %s err\n\r",\
				__FUNCTION__, getpid(), path);
			return GPN_TIMER_GEN_ERR;
		}
		else
		{
			GPN_TIMER_PRINT(GPN_TIMER_MAN_EGP, "%s(%d) : mkdir %s good\n\r",\
				__FUNCTION__, getpid(), path);
			return GPN_TIMER_GEN_OK;
		}
	}
	else
	{
		GPN_TIMER_PRINT(GPN_TIMER_MAN_EGP, "%s(%d) : dir %s exist\n\r", \
			__FUNCTION__, getpid(), path);
		return GPN_TIMER_GEN_OK;
	}
	
}
UINT32 gpnTimerSpaceInit(void)
{
	UINT32 i;
	UINT32 j;
	UINT8 *buffPtr;

	i = sizeof(gpnTimerInfo) +/*timer global struct size*/\
		GPN_PROCESS_TIMER_MAX_NUM*sizeof(timeNode);
	pgtimerInfo = (gpnTimerInfo*)malloc(i);
	if(pgtimerInfo == NULL)
	{
		printf("%s:malloc err\n\r", __FUNCTION__);
		perror("malloc");
		return GPN_TIMER_GEN_ERR;
	}
	memset((UINT8 *)pgtimerInfo, 0, i);

	pgtimerInfo->sigTickNum = GPN_TIME_TO_TICKS(1, second);
	pgtimerInfo->sigPeriod.tv_sec = (pgtimerInfo->sigTickNum*GPN_SYS_PER_TICK_MS)/1000;
	pgtimerInfo->sigPeriod.tv_usec = ((pgtimerInfo->sigTickNum*GPN_SYS_PER_TICK_MS)%1000)*1000;
	pgtimerInfo->sigTStamp.tv_sec = 0;
	pgtimerInfo->sigTStamp.tv_usec = 0;
	pgtimerInfo->sigCall = NULL;
	pgtimerInfo->sigCallPara1 = 0;
	pgtimerInfo->sigCallPara1 = 0;
	
	pgtimerInfo->baseTickNum = GPN_TIME_TO_TICKS(1, second);
	pgtimerInfo->basePeriod.tv_sec = (pgtimerInfo->baseTickNum*GPN_SYS_PER_TICK_MS)/1000;
	pgtimerInfo->basePeriod.tv_usec = ((pgtimerInfo->baseTickNum*GPN_SYS_PER_TICK_MS)%1000)*1000;
	
	listInit(&(pgtimerInfo->actTimer));
	pgtimerInfo->actTimerNum = 0;
	listInit(&(pgtimerInfo->idleTimer));
	pgtimerInfo->idleTimerNum = 0;

	pgtimerInfo->en = GPN_SOCK_SYS_ENABLE;
	
	pgtimerInfo->timerMsgid = -1;

	/*set base-timer-fifo init is 0*/
	pgtimerInfo->timerMsgFifo = 0;
	
	pgtimerInfo->procTStamp.tv_sec = 0;
	pgtimerInfo->procTStamp.tv_usec = 0;

	buffPtr = (UINT8 *)pgtimerInfo + sizeof(gpnTimerInfo);
	for(j=0;j<GPN_PROCESS_TIMER_MAX_NUM;j++)
	{
		listAdd(&(pgtimerInfo->idleTimer), (NODE *)buffPtr);
		pgtimerInfo->idleTimerNum++;

		buffPtr += sizeof(timeNode);
	}
	
	pgtimerInfo->calCall = NULL;
	pgtimerInfo->calCallPara1 = 0;
	pgtimerInfo->calCallPara1 = 0;

	/* clc source type set */
#if 0
	pgtimerInfo->clcSrcType = GPN_TIMER_CLC_SRC_POCIX;
#else
	pgtimerInfo->clcSrcType = GPN_TIMER_CLC_SRC_SELECET;
#endif
	
	/*debug*/
	GPN_TIMER_PRINT(GPN_TIMER_MAN_CUP, "%s: timer space init OK!\n\r", __FUNCTION__);
	GPN_TIMER_PRINT(GPN_TIMER_MAN_CUP, "%s: baseTiceNum %d tickLong(ms) %d\n\r",\
		__FUNCTION__, pgtimerInfo->baseTickNum, GPN_SYS_PER_TICK_MS);
	GPN_TIMER_PRINT(GPN_TIMER_MAN_CUP, "%s: basePeriod: %ds %dus\n\r",\
		__FUNCTION__, (UINT32)pgtimerInfo->basePeriod.tv_sec, (UINT32)pgtimerInfo->basePeriod.tv_usec);
	GPN_TIMER_PRINT(GPN_TIMER_MAN_CUP, "%s: maxNode: %d activeNode %d idleNode %d\n\r",\
		__FUNCTION__, GPN_PROCESS_TIMER_MAX_NUM, pgtimerInfo->actTimerNum, pgtimerInfo->idleTimerNum);
	
	return GPN_TIMER_GEN_OK;
}

/********************************************************************************/
/*                                                            MSG QUEN OPT                                                         */
/********************************************************************************/
UINT32 gpnTimerMsgQuenForUserOptInit(void)
{
	UINT32 reVal;
	key_t timerKey;
	char programNameP[GPN_TIMER_PROCESS_PATH_LEN];
	
	memset(programNameP, 0, GPN_TIMER_PROCESS_PATH_LEN);

	/*BUSY BOx not support "awk" ??? */
	reVal = gpnTimerGetProcessPath(programNameP, GPN_TIMER_PROCESS_PATH_LEN);
	if(reVal == GPN_TIMER_GEN_ERR)
	{
		/*do self pid unify path */
		snprintf(programNameP, GPN_TIMER_PROCESS_PATH_LEN, "/temp/%d", getpid());
		gpnUnifyPathCreat(programNameP);
		GPN_TIMER_PRINT(GPN_TIMER_MAN_CUP, "%s : defaut temp timer msgQuen path \"%s\"",\
			__FUNCTION__, programNameP);
	}
	
	if(gpnTimerFilePathCheck(timerMsgQ) == GPN_TIMER_GEN_ERR)
	{
		return GPN_TIMER_GEN_ERR;
	}
	if(gpnTimerFilePathCheck(programNameP) == GPN_TIMER_GEN_ERR)
	{
		return GPN_TIMER_GEN_ERR;
	}
	
	timerKey = ftok(programNameP, 't');
	if(timerKey == -1)
	{
		printf("timerKey error\n\r");
		perror("ftok");
		/*exit(EXIT_FAILURE);*/
		GPN_TIMER_PRINT(GPN_TIMER_MAN_EGP, "timerKey errno\n\r");
		
		return GPN_TIMER_GEN_ERR;
	}

	/*why we do this? creat delete creat ?
	   msgget will return Ok if msgQuen is
	   also exit, so we delete it in order to
	   creat a new msgQuen(msg text free)
	 */
	pgtimerInfo->timerMsgid = msgget(timerKey, 0666|IPC_CREAT);
	if(pgtimerInfo->timerMsgid == -1)
	{
		printf("timerMsgid error\n\r");
		perror("msgget");
		/*exit(EXIT_FAILURE);*/
		GPN_TIMER_PRINT(GPN_TIMER_MAN_EGP, "timerMsgid errno\n\r");
		
		return GPN_TIMER_GEN_ERR;
	}
	if(msgctl(pgtimerInfo->timerMsgid, IPC_RMID, NULL) == -1)
	{
		printf("msgctl(IPC_RMID) failed\n\r");
		perror("msgctl");
		/*exit(EXIT_FAILURE);*/
		GPN_TIMER_PRINT(GPN_TIMER_MAN_EGP, "timerMsgid errno\n\r");
		
		return GPN_TIMER_GEN_ERR;
	}
	pgtimerInfo->timerMsgid = msgget(timerKey, 0666|IPC_CREAT);
	if(pgtimerInfo->timerMsgid == -1)
	{
		printf("timerMsgid error\n\r");
		perror("msgget");
		/*exit(EXIT_FAILURE);*/
		GPN_TIMER_PRINT(GPN_TIMER_MAN_EGP, "timerMsgid errno\n\r");
		
		return GPN_TIMER_GEN_ERR;
	}

	GPN_TIMER_PRINT(GPN_TIMER_MAN_CUP, "%s: timerMsgid %d, Init OK!\n\r",\
		__FUNCTION__, pgtimerInfo->timerMsgid);

	return GPN_TIMER_GEN_OK;
}
UINT32 gpnTimerMsgQuenSend(UINT32 msgType, timeOptMsg *pMsg)
{
	UINT32 reVal = GPN_TIMER_GEN_ERR;
	
	if(pMsg != NULL)
	{
		switch(msgType)
		{
			case GPN_TIMR_USER_OPT_ADD:	
			case GPN_TIMR_USER_OPT_DEL:
				if((msgsnd(pgtimerInfo->timerMsgid, (void *)pMsg, GPN_TIMER_USER_OPT_STRUCT_BLEN,\
					/*sizeof(timeOptMsg)*/IPC_NOWAIT)) == -1 )
				{
					reVal  = GPN_TIMER_GEN_ERR;
				}
				else
				{
					reVal  = GPN_TIMER_GEN_OK;
				}
				break;
				
			case GPN_TIMR_USER_OPT_NULL:
			case GPN_TIMR_USER_OPT_ALL:
				break;
				
			default:
				break;
		}
	}
	return reVal;	
}
UINT32 gpnTimerMsgQuenReceive(UINT32 msgType, timeOptMsg *pMsg)
{
	UINT32 reVal = GPN_TIMER_GEN_ERR;
	
	switch(msgType)
	{
		case GPN_TIMR_USER_OPT_ALL:
			if((msgrcv(pgtimerInfo->timerMsgid, (void *)pMsg, GPN_TIMER_USER_OPT_STRUCT_BLEN,\
				 /*sizeof(timeOptMsg)*/ 0, IPC_NOWAIT)) == -1)
			{
				/*printf("timer opt rcv err\n\r");*/
				pMsg = NULL;
				reVal = GPN_TIMER_GEN_ERR;
			}
			else
			{
				/*printf("timer opt rcv ok!\n\r");*/
				reVal = GPN_TIMER_GEN_OK;
			}
			break;
			
		case GPN_TIMR_USER_OPT_ADD:	
		case GPN_TIMR_USER_OPT_DEL:
			if((msgrcv(pgtimerInfo->timerMsgid, (void *)pMsg, GPN_TIMER_USER_OPT_STRUCT_BLEN,\
				 /*sizeof(timeOptMsg)*/ msgType, IPC_NOWAIT)) == -1)
			{
				pMsg = NULL;
				reVal = GPN_TIMER_GEN_ERR;
			}
			else
			{
				reVal = GPN_TIMER_GEN_OK;
			}
			break;
			
		case GPN_TIMR_USER_OPT_NULL:
			pMsg = NULL;
			break;
			
		default:
			pMsg = NULL;
			break;
	}
	
	return reVal;
}
UINT32 gpnTimerMsgQuenDelete(void)
{
	if(msgctl(pgtimerInfo->timerMsgid, IPC_RMID, 0) == -1)
	{
		printf("%s:msgctl", __FUNCTION__);
		perror("msgctl");
		return GPN_TIMER_GEN_ERR;
	}
	
	GPN_TIMER_PRINT(GPN_TIMER_MAN_CUP, "%s: Delete OK! %d\n\r", __FUNCTION__, pgtimerInfo->timerMsgid);
	return GPN_TIMER_GEN_OK;
}
UINT32 gpnTimerAddUserOpt(UINT32 timerType, UINT32 period, UINT32 life,
				timerOutFuncPtr handler, UINT32 para1, UINT32 para2)
{
	static UINT32 gTimeId = 100;
	timeOptMsg timerOpt;
	
	if(	((timerType != GPN_TIMR_TYPE_CYC) &&
		 (timerType != GPN_TIMR_TYPE_TIMES) &&
		 (timerType != GPN_TIMR_TYPE_ONCE)) ||
		((period % pgtimerInfo->baseTickNum) != 0) ||
		(handler == NULL) )
	{
		printf("%s : add timer err, period %d baseTick %d\n\r",
			__FUNCTION__, period, pgtimerInfo->baseTickNum);
		return GPN_TIMER_GEN_ERR;
	}

	/*Global timer ID creat, stat from 100*/
	if(gTimeId >= 100)
	{
		timerOpt.timerId = gTimeId;
		gTimeId++;
	}
	else
	{
		gTimeId = 100;
		timerOpt.timerId = gTimeId;
	}
	
	timerOpt.opt = GPN_TIMR_USER_OPT_ADD;
	timerOpt.tType = timerType;
	
	timerOpt.period.tv_sec = (period*GPN_SYS_PER_TICK_MS)/1000;
	timerOpt.period.tv_usec = ((period*GPN_SYS_PER_TICK_MS)%1000)*1000;
	
	timerOpt.life = life;
	timerOpt.tOFunPtr = handler;
	timerOpt.para1 = para1;
	timerOpt.para2 = para2;

	GPN_TIMER_PRINT(GPN_TIMER_MAN_CUP, "%s: tType %d period %d life %d\n\r",\
		__FUNCTION__, timerType, period, life);

	if(gpnTimerMsgQuenSend(GPN_TIMR_USER_OPT_ADD, &timerOpt) == GPN_TIMER_GEN_OK)
	{
		/*user should save this return, use this to delete timer*/
		return timerOpt.timerId;
	}
	else
	{
		return GPN_TIMER_GEN_ERR;
	}
}
UINT32 gpnTimerDeleteUserOpt(INT32 timerId)
{
	timeOptMsg timerOpt;

	timerOpt.opt = GPN_TIMR_USER_OPT_DEL;
	timerOpt.timerId = timerId;

	if(gpnTimerMsgQuenSend(GPN_TIMR_USER_OPT_DEL, &timerOpt) == GPN_TIMER_GEN_OK)
	{
		return GPN_TIMER_GEN_OK;
	}
	else
	{
		return GPN_TIMER_GEN_ERR;
	}
}

/********************************************************************************/
/*                                                            REAL TIMER OPT                                                       */
/********************************************************************************/
UINT32 gpnRealTimerAdd(timeOptMsg *ptimerOpt)
{
	timeNode *ptmpNode;
	timeNode *paddNode;
	UINT32 actTimerNum;

	if(pgtimerInfo->idleTimerNum > 0)
	{
		paddNode = (timeNode *)listGet(&(pgtimerInfo->idleTimer));
		if(paddNode == NULL)
		{
			return GPN_TIMER_GEN_ERR;
		}
		pgtimerInfo->idleTimerNum--;

		paddNode->opt = ptimerOpt->opt;
		paddNode->tType = ptimerOpt->tType;
		paddNode->timerId = ptimerOpt->timerId;
		paddNode->period = ptimerOpt->period;
		paddNode->life = ptimerOpt->life;
		paddNode->tOFunPtr = ptimerOpt->tOFunPtr;
		paddNode->para1 = ptimerOpt->para1;
		paddNode->para2 = ptimerOpt->para2;
		paddNode->timerStamp.tv_sec =\
			pgtimerInfo->procTStamp.tv_sec + ptimerOpt->period.tv_sec;
		paddNode->timerStamp.tv_usec =\
			pgtimerInfo->procTStamp.tv_usec + ptimerOpt->period.tv_usec;
	}
	else
	{
		GPN_TIMER_PRINT(GPN_TIMER_MAN_EGP, "%s : idleTemerNum = 0\n\r",\
			__FUNCTION__);
		return GPN_TIMER_GEN_ERR;
	}
	
	actTimerNum = 0;
	ptmpNode = (timeNode *)listFirst(&(pgtimerInfo->actTimer));
	while((ptmpNode != NULL)&&(actTimerNum < pgtimerInfo->actTimerNum))
	{
		if(gpnUsecTimeCmp(&(ptmpNode->timerStamp), &(paddNode->timerStamp)) >= 0)
		{
			/*if ptmpNode is first Node ,it's previous Node is NULL*/
			ptmpNode = (timeNode *)listPrevious((NODE *)(ptmpNode));
			listInsert(&(pgtimerInfo->actTimer), (NODE *)(ptmpNode),(NODE *)(paddNode));
			pgtimerInfo->actTimerNum++;

			/*here we can't use 'break;' for what? if ptmpNode be insert first place, it is NULL*/
			return GPN_TIMER_GEN_OK;
		}
		
		actTimerNum++;
		ptmpNode = (timeNode *)listNext((NODE *)(ptmpNode));
	}
	
	if((ptmpNode == NULL)||(actTimerNum == pgtimerInfo->actTimerNum))
	{
		/*add at last*/
		listAdd(&(pgtimerInfo->actTimer),(NODE *)(paddNode));
		pgtimerInfo->actTimerNum++;
	}
	
	return GPN_TIMER_GEN_OK;
}
UINT32 gpnRealTimerDelete(timeOptMsg *ptimerOpt)
{
	timeNode *ptmpNode;
	UINT32 actTimerNum;

	actTimerNum = 0;
	ptmpNode = (timeNode *)listFirst(&(pgtimerInfo->actTimer));
	while((ptmpNode != NULL)&&(actTimerNum < pgtimerInfo->actTimerNum))
	{
		if(ptmpNode->timerId == ptimerOpt->timerId)
		{
			listDelete(&(pgtimerInfo->actTimer), (NODE *)(ptmpNode));
			pgtimerInfo->actTimerNum--;
			listAdd(&(pgtimerInfo->idleTimer),(NODE *)(ptmpNode));
			pgtimerInfo->idleTimerNum++;
			
			break;
		}
		
		actTimerNum++;
		ptmpNode = (timeNode *)listNext((NODE *)(ptmpNode));
	}
	
	return GPN_TIMER_GEN_OK;
}
void gpnTimerSigEventProc(INT32 signo)
{
	/*part0 : if gpn_timer is enbale*/
	if(pgtimerInfo->en != GPN_TIMER_GEN_ENABLE)
	{
		return;
	}
	
	/*part1 : signal cyc callback*/
	if(pgtimerInfo->sigCall != NULL)
	{
		pgtimerInfo->sigCall(pgtimerInfo->sigCallPara1, pgtimerInfo->sigCallPara2);
	}

	/*part2 : sigTimer to base Timer*/
	if(pgtimerInfo->sigTickNum != pgtimerInfo->baseTickNum)
	{
		/*global signal timer stamp add*/
		pgtimerInfo->sigTStamp.tv_sec += pgtimerInfo->sigPeriod.tv_sec;
		pgtimerInfo->sigTStamp.tv_usec += pgtimerInfo->sigPeriod.tv_usec;
		/*1s = 1000000ms*/
		pgtimerInfo->sigTStamp.tv_sec += pgtimerInfo->sigTStamp.tv_usec/1000000; 
		pgtimerInfo->sigTStamp.tv_usec = pgtimerInfo->sigTStamp.tv_usec%1000000;

		if( (pgtimerInfo->sigTStamp.tv_sec < pgtimerInfo->basePeriod.tv_sec) ||\
			((pgtimerInfo->sigTStamp.tv_sec == pgtimerInfo->basePeriod.tv_sec) &&\
			(pgtimerInfo->sigTStamp.tv_usec < pgtimerInfo->basePeriod.tv_usec)) )
		{
			/*base timer not bingo, over timer process*/
			return;
		}
		else
		{
			/*global signal timer stamp clear up, restart counter sig timer bingo*/
			pgtimerInfo->sigTStamp.tv_sec = 0;
			pgtimerInfo->sigTStamp.tv_usec = 0;
		}
	}
	
	/*part4 : do base Timer*/
	/*process signal interrupt timer service:
	  tx timer msg to timer server self
	  GPN_TIME_PRINT(GPN_TIME_CUP, "gpnTimerSigEventProc:signo-%d\n\r", signo);*/

	/*when we send a base-timer-msg, we make timerMsgFifo increace one
	    when GPN_SOCKET receive a base-timer-msg, we decreace timerMsgFifo
	    when timerMsgFifo >= GPN_TIMER_BASE_TIMER_FIFO, we do not send*/
	/*printf("%s : msg fifo %d <? %d\n\r", __FUNCTION__, pgtimerInfo->timerMsgFifo, GPN_TIMER_BASE_TIMER_FIFO);*/
	if(pgtimerInfo->timerMsgFifo < GPN_TIMER_BASE_TIMER_FIFO)
	{
		if(gpnSockSpMsgTxBaseTiming(&(gSockCommData.gstFdSet)) == GPN_TIMER_GEN_OK)
		{
			/*when base timer bingo send ok, we do ++*/
			pgtimerInfo->timerMsgFifo++;
		}
	}
}

void gpnBaseTimerBingo(void)
{
	timeNode *ptimerNode;
	timeNode *ptmpNode;
	timeNode *pPrevNode;
	timeOptMsg optMsg;
	UINT32 actTimerNum;
	UINT32 i;

	
	/*when we send a base-timer-msg, we make timerMsgFifo increace one
	   when GPN_SOCKET receive a base-timer-msg, we decreace timerMsgFifo
	   when timerMsgFifo >= GPN_TIMER_BASE_TIMER_FIFO, we do not send*/
	   
	/*printf("%s : msg fifo %d <? %d\n\r", __FUNCTION__, pgtimerInfo->timerMsgFifo, GPN_TIMER_BASE_TIMER_FIFO);*/
	if(pgtimerInfo->timerMsgFifo > 0)
	{
		pgtimerInfo->timerMsgFifo--;
	}
	
	/*part0:global timer stamp add*/
	pgtimerInfo->procTStamp.tv_sec += pgtimerInfo->basePeriod.tv_sec;
	pgtimerInfo->procTStamp.tv_usec += pgtimerInfo->basePeriod.tv_usec;
	/*1s = 1000000ms*/
	pgtimerInfo->procTStamp.tv_sec += pgtimerInfo->procTStamp.tv_usec/1000000; 
	pgtimerInfo->procTStamp.tv_usec = pgtimerInfo->procTStamp.tv_usec%1000000;

	GPN_TIMER_PRINT(GPN_TIMER_MAN_UD0, "%s(%d) : actTimer(%d) timeNow(sec %08x usec %08x)\n\r",\
		__FUNCTION__, getpid(), pgtimerInfo->actTimerNum,\
		(UINT32)(pgtimerInfo->procTStamp.tv_sec),\
		(UINT32)(pgtimerInfo->procTStamp.tv_usec));

	/*test
	actTimerNum = 0;
	ptmpNode = (timeNode *)listFirst(&(pgtimerInfo->actTimer));
	while((ptmpNode != NULL)&&(actTimerNum < pgtimerInfo->actTimerNum))
	{
		GPN_TIMER_PRINT(GPN_TIMER_MAN_EGP, "%s : timerId %d sec %08x usec %08x\n\r",\
				__FUNCTION__, ptmpNode->timerId,\
				(UINT32)(ptmpNode->timerStamp.tv_sec), (UINT32)(ptmpNode->timerStamp.tv_usec));
		
		actTimerNum++;
		ptmpNode = (timeNode *)listNext((NODE *)(ptmpNode));
	}
	test end*/
	
	/*part1:time out porc*/
	for(i=0; i<pgtimerInfo->actTimerNum; i++)
	{
		/*next time out node*/
		ptimerNode = (timeNode *)listFirst(&(pgtimerInfo->actTimer));
		if(ptimerNode == NULL)
		{
			break;
		}
		
		if(gpnUsecTimeCmp(&(pgtimerInfo->procTStamp), &(ptimerNode->timerStamp)) >= 0)
		{
			/*this time out*/
			ptimerNode = (timeNode *)listGet(&(pgtimerInfo->actTimer));
			if(ptimerNode == NULL)
			{
				break;
			}
			
			/*call back*/
			ptimerNode->tOFunPtr(ptimerNode->para1, ptimerNode->para2);
			
			/*re-timer*/
			if( (ptimerNode->tType == GPN_TIMR_TYPE_CYC) ||
				((ptimerNode->tType == GPN_TIMR_TYPE_TIMES) &&
				 (ptimerNode->life > 0)) )
			{
				ptimerNode->timerStamp.tv_sec += ptimerNode->period.tv_sec;
				ptimerNode->timerStamp.tv_usec += ptimerNode->period.tv_usec;
				/*1s = 1000000ms*/
				ptimerNode->timerStamp.tv_sec += ptimerNode->timerStamp.tv_usec/1000000; 
				ptimerNode->timerStamp.tv_usec = ptimerNode->timerStamp.tv_usec%1000000;
				
				/*just for GPN_TIMR_TYPE_TIMES*/
				ptimerNode->life--;
				
				actTimerNum = 0;
				ptmpNode = (timeNode *)listFirst(&(pgtimerInfo->actTimer));
				while((ptmpNode != NULL)&&(actTimerNum < pgtimerInfo->actTimerNum))
				{
					if(gpnUsecTimeCmp(&(ptmpNode->timerStamp), &(ptimerNode->timerStamp)) >= 0)
					{
						/*if ptmpNode is first Node ,it's previous Node is NULL*/
						pPrevNode = (timeNode *)listPrevious((NODE *)(ptmpNode));
						listInsert(&(pgtimerInfo->actTimer), (NODE *)(pPrevNode), (NODE *)(ptimerNode));
						
						break;
					}
					
					actTimerNum++;
					ptmpNode = (timeNode *)listNext((NODE *)(ptmpNode));
				}
				if((ptmpNode == NULL)||(actTimerNum == pgtimerInfo->actTimerNum))
				{
					/*add at last*/
					listAdd(&(pgtimerInfo->actTimer),(NODE *)(ptimerNode));
				}
			}
			else if(((ptimerNode->tType == GPN_TIMR_TYPE_TIMES) &&
					 (ptimerNode->life == 0)) ||
				   	(ptimerNode->tType == GPN_TIMR_TYPE_ONCE))
			{
				ptimerNode->timerStamp.tv_sec += ptimerNode->period.tv_sec;
				ptimerNode->timerStamp.tv_usec += ptimerNode->period.tv_usec;
				/*1s = 1000000ms*/
				ptimerNode->timerStamp.tv_sec += ptimerNode->timerStamp.tv_usec/1000000; 
				ptimerNode->timerStamp.tv_usec = ptimerNode->timerStamp.tv_usec%1000000;
				
				/*just for GPN_TIMR_TYPE_TIMES
				ptimerNode->life--;*/
				if(pgtimerInfo->actTimerNum > 0)
				{
					pgtimerInfo->actTimerNum--;
				}
				listAdd(&(pgtimerInfo->idleTimer),(NODE *)(ptimerNode));
				pgtimerInfo->idleTimerNum++;
			}
			else
			{
				/*do nothing, can not be here*/
			}
		}
		else
		{
			break;
		}
	}

	/*part2:user opt proc*/
	for(i=0; i<GPN_PROCESS_TIMER_MAX_NUM; i++)
	{
		if(gpnTimerMsgQuenReceive(GPN_TIMR_USER_OPT_ALL, &optMsg) == GPN_TIMER_GEN_OK)
		{
			if(optMsg.opt == GPN_TIMR_USER_OPT_ADD)
			{
				gpnRealTimerAdd(&optMsg);
				GPN_TIMER_PRINT(GPN_TIMER_MAN_EGP, "%s(%d) : real add timer, activTimer %d, idleTimer %d\n\r",\
					__FUNCTION__, getpid(), pgtimerInfo->actTimerNum, pgtimerInfo->idleTimerNum);
			}
			else
			{
				gpnRealTimerDelete(&optMsg);
				GPN_TIMER_PRINT(GPN_TIMER_MAN_EGP, "%s(%d) : real delete timer, activTimer %d, idleTimer %d\n\r",\
					__FUNCTION__, getpid(), pgtimerInfo->actTimerNum, pgtimerInfo->idleTimerNum);
			}
		}
		else
		{
			break;
		}
	}
}

/********************************************************************************/
/*                                                    TIMER CALLBACK OPT                                                       */
/********************************************************************************/
UINT32 gpnTimerAddSigTimerCallBack(sigTimerCallback handler, UINT32 para1, UINT32 para2)
{
	UINT32 enSta;

	/*assert*/
	if(pgtimerInfo == NULL)
	{
		return GPN_TIMER_GEN_ERR;
	}

	/* prevent : when add handler, sigal come */
	enSta = pgtimerInfo->en;
	pgtimerInfo->en = GPN_TIMER_GEN_DISABLE;

	pgtimerInfo->sigCall = handler;
	pgtimerInfo->sigCallPara1 = para1;
	pgtimerInfo->sigCallPara2 = para2;

	pgtimerInfo->en = enSta;

	return GPN_TIMER_GEN_OK;
}
UINT32 gpnTimerDeleteSigTimerCallBack(void)
{
	UINT32 enSta;

	/*assert*/
	if(pgtimerInfo == NULL)
	{
		return GPN_TIMER_GEN_ERR;
	}
	
	enSta = pgtimerInfo->en;
	pgtimerInfo->en = GPN_TIMER_GEN_DISABLE;

	pgtimerInfo->sigCall = NULL;
	pgtimerInfo->sigCallPara1 = 0;
	pgtimerInfo->sigCallPara2 = 0;

	pgtimerInfo->en = enSta;

	return GPN_TIMER_GEN_OK;
}
UINT32 gpnTimerAddCalendarChangeCallBack(calChgCallback handler, UINT32 para1, UINT32 para2)
{
	/*assert*/
	if(pgtimerInfo == NULL)
	{
		return GPN_TIMER_GEN_ERR;
	}

	pgtimerInfo->calCall = handler;
	pgtimerInfo->calCallPara1 = para1;
	pgtimerInfo->calCallPara2 = para2;

	return GPN_TIMER_GEN_OK;
}
UINT32 gpnTimerDeleteCalendarChangeCallBack(void)
{
	/*assert*/
	if(pgtimerInfo == NULL)
	{
		return GPN_TIMER_GEN_ERR;
	}

	pgtimerInfo->calCall = NULL;
	pgtimerInfo->calCallPara1 = 0;
	pgtimerInfo->calCallPara2 = 0;

	return GPN_TIMER_GEN_OK;
}

/********************************************************************************/
/*                                                    POCIX TIMER OPT                                                              */
/********************************************************************************/
UINT32 gpnTimerPOCIXTimerCreat(UINT32 sigLen, baseTimerFuncPtr timerProc)
{
	gpnTimerPOCIXTimerSigactionInit(timerProc);
	gpnTimerPOCIXTimerBaseTimeInit(sigLen);

	return GPN_TIMER_GEN_OK;
}
void gpnTimerPOCIXTimerSigactionInit(baseTimerFuncPtr timerProc)
{
	struct sigevent evp;
	struct sigaction act;

	memset(&act, 0, sizeof(act));
	act.sa_handler = timerProc;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGUSR2, &act, NULL) == -1)
	{
		perror("fail to sigaction");
		exit(-1);
	}
	
	memset(&evp, 0, sizeof(struct sigevent));
	evp.sigev_signo = SIGUSR2;
	evp.sigev_notify = SIGEV_SIGNAL;
	
	evp.sigev_value.sival_ptr = &g_timerid;
	
	if (timer_create(CLOCK_REALTIME, &evp, &g_timerid) == -1)
	{
		perror("fail to timer_create");
		exit(-1);
	}

}
void gpnTimerPOCIXTimerBaseTimeInit(UINT32 tickNum)
{
    /*struct itimerspec;*/
	struct itimerspec it;
	UINT32 sec;
	UINT32 usec;

	sec = (tickNum*GPN_SYS_PER_TICK_MS)/1000;
	usec = ((tickNum*GPN_SYS_PER_TICK_MS)%1000)*1000;
	
	
	it.it_interval.tv_sec = sec;
	it.it_interval.tv_nsec = usec * 1000;
	it.it_value.tv_sec = sec;
	it.it_value.tv_nsec = usec * 1000;
	timer_settime(g_timerid, 0, &it, NULL);
}
void gpnTimerPOCIXTimerAdjustTime(void)
{
	struct itimerspec it;
	
	timer_gettime(g_timerid, &it);

	it.it_value.tv_sec = (it.it_interval.tv_sec) / 2;
	it.it_value.tv_nsec = (it.it_interval.tv_nsec) / 2;

	timer_settime(g_timerid, 0, &it, 0);
	
	/*printf("%s(%d) : adjust timer %d, interval(%d/%d) next-bingo(%d/%d) oldTime(%d) newTime(%d)!\n\r",\
		__FUNCTION__, getpid(), (UINT32)g_timerid,\
		(UINT32)(it.it_interval.tv_sec), (UINT32)(it.it_interval.tv_nsec),\
		(UINT32)(it.it_value.tv_sec), (UINT32)(it.it_value.tv_nsec),\
			oldTime, newTime);
	*/
	GPN_TIMER_PRINT(GPN_TIMER_MAN_CUP, "%s(%d) : adjust timer %d, interval(%d/%d) next-bingo(%d/%d)!\n\r",\
		__FUNCTION__, getpid(), (UINT32)g_timerid,\
		(UINT32)(it.it_interval.tv_sec), (UINT32)(it.it_interval.tv_nsec),\
		(UINT32)(it.it_value.tv_sec), (UINT32)(it.it_value.tv_nsec));
	
}

void gpnGetBaseTimer(struct itimerspec *pIt)
{
	timer_gettime(g_timerid, pIt);
}

void gpnSetBaseTimer(struct itimerspec it)
{
	timer_settime(g_timerid, 0, &it, 0);
}

#if 0
strcut timeval
{
	long tv_sec; /**/
	long tv_usec; /**/
};
struct itimerval
{
	struct timeval it_interval; /**/
	struct timeval it_value;   /**/
};

struct timespec {
	time_t tv_sec;                /* Seconds */
	long   tv_nsec;               /* Nanoseconds */
};

struct itimerspec {
	struct timespec it_interval;  /* Timer interval */
	struct timespec it_value;     /* Initial expiration */
};

#endif
/********************************************************************************/
/*                                                     SELECT TIMER OPT                                                           */
/********************************************************************************/
UINT32 gpnTimerSelectTimerNoBlockBingoCheck(void)
{
	static struct timeval oldTime;
	struct timeval nowTime;
	UINT32 usec;
	/*static UINT32 count; */
	
	/*assert*/
	if(pgtimerInfo == NULL)
	{
		return GPN_TIMER_GEN_ERR;
	}

    if( (oldTime.tv_sec == 0) &&\
		(oldTime.tv_usec == 0) )
	{
		/* start up, init first */
		gettimeofday(&oldTime, NULL);
		
		/*printf("%s : select init first : oldTime(tv_sec %10d tv_usec %10d)\n\r",
			__FUNCTION__, oldTime.tv_sec, oldTime.tv_usec);*/
    }
	else
	{
		/* calculate time pass for new select time out */
		gettimeofday(&nowTime, NULL);
		
		if(gpnUsecTimeCmp(&oldTime, &nowTime) == -1)
		{
			/* oldTime < nowTime */
			usec = pgtimerInfo->sigPeriod.tv_sec*1000*1000;
			usec += pgtimerInfo->sigPeriod.tv_usec;
			if(gpnUsecTimePassCheck(&oldTime, &nowTime, usec) == GPN_TIMER_GEN_OK)
			{
				/* oldTime < nowTime  more than sigPeriod */
				/* select timer bingo  */
				gpnTimerSigEventProc(0);
				/* do now calculate */
				if( (nowTime.tv_sec - oldTime.tv_sec) < 10)
				{
					oldTime.tv_sec += pgtimerInfo->sigPeriod.tv_sec;
					oldTime.tv_usec += pgtimerInfo->sigPeriod.tv_usec;
					
					oldTime.tv_sec += oldTime.tv_usec / 1000000;
					oldTime.tv_usec += oldTime.tv_usec % 1000000;
				}
				else
				{
					oldTime.tv_sec = nowTime.tv_sec;
					oldTime.tv_usec = nowTime.tv_usec;
				}
				

				/*printf("%s : time pass too long : nowTime(tv_sec %10d tv_usec %10d) oldTime(tv_sec %10d tv_usec %10d)\n\r",
					__FUNCTION__, nowTime.tv_sec, nowTime.tv_usec,\
					oldTime.tv_sec, oldTime.tv_usec);*/
			}
		}
		else
		{
			/* oldTime >= nowTime */
			/* do now calculate */
			oldTime.tv_sec = nowTime.tv_sec;
			oldTime.tv_usec = nowTime.tv_usec;
			
			/*count++;
			if(count > 30)
			{
				printf("%s : time miss match : nowTime(tv_sec %10d tv_usec %10d)\n\r",
 					__FUNCTION__, nowTime.tv_sec, nowTime.tv_usec);
				count = 0;
			}*/
		}
	}
	
	return GPN_TIMER_GEN_OK;
}

UINT32 gpnTimerSelectTimerBlockBingoCheck(struct timeval *bTime)
{
	static struct timeval oldTime;
	struct timeval nowTime;
	UINT32 usec;
	
	/*assert*/
	if(pgtimerInfo == NULL)
	{
		return GPN_TIMER_GEN_ERR;
	}

	if(bTime == NULL)
	{
		return GPN_TIMER_GEN_ERR;
	}

    if( (oldTime.tv_sec == 0) &&\
		(oldTime.tv_usec == 0) )
	{
		/* start up, init first */
		gettimeofday(&oldTime, NULL);
		/* init select time out as sigPeriod */
		bTime->tv_sec = pgtimerInfo->sigPeriod.tv_sec;
		bTime->tv_usec = pgtimerInfo->sigPeriod.tv_usec;
		
		/*printf("%s : select init first : bTime->tv_sec %d bTime->tv_usec %d\n\r",
			__FUNCTION__, bTime->tv_sec, bTime->tv_usec,\
			oldTime.tv_sec, oldTime.tv_usec);*/
    }
	else if((bTime->tv_sec == 0) &&\
			(bTime->tv_usec == 0))
	{
		/* select timer bingo  */
		gpnTimerSigEventProc(0);
		/* do now calculate */
		gettimeofday(&oldTime, NULL);
		/* renew select timer out */
		bTime->tv_sec = pgtimerInfo->sigPeriod.tv_sec;
		bTime->tv_usec = pgtimerInfo->sigPeriod.tv_usec;

		/*printf("%s : select time out : bTime->tv_sec %d bTime->tv_usec %d\n\r",
			__FUNCTION__, bTime->tv_sec, bTime->tv_usec,\
			oldTime.tv_sec, oldTime.tv_usec);*/
	}
	else
	{
		/* calculate time pass for new select time out */
		gettimeofday(&nowTime, NULL);
		
		if(gpnUsecTimeCmp(&oldTime, &nowTime) == -1)
		{
			/* oldTime < nowTime */
			usec = pgtimerInfo->sigPeriod.tv_sec*1000*1000;
			usec += pgtimerInfo->sigPeriod.tv_usec;
			if(gpnUsecTimePassCheck(&oldTime, &nowTime, usec) == GPN_TIMER_GEN_OK)
			{
				/* oldTime < nowTime  more than sigPeriod */
				/* select timer bingo  */
				gpnTimerSigEventProc(0);
				/* do now calculate */
				oldTime.tv_sec = nowTime.tv_sec;
				oldTime.tv_usec = nowTime.tv_usec;
				/* renew select timer out */
				bTime->tv_sec = pgtimerInfo->sigPeriod.tv_sec;
				bTime->tv_usec = pgtimerInfo->sigPeriod.tv_usec;

				/*printf("%s : time pass too long : bTime(tv_sec %d tv_usec %d) nowTime(tv_sec %d tv_usec %d)\n\r",
					__FUNCTION__, bTime->tv_sec, bTime->tv_usec,\
					nowTime.tv_sec, nowTime.tv_usec);*/
			}
			else
			{
				/* calculate select block time left */
				oldTime.tv_sec += pgtimerInfo->sigPeriod.tv_sec;
				oldTime.tv_usec += pgtimerInfo->sigPeriod.tv_usec;
				
				oldTime.tv_sec += oldTime.tv_usec / 1000000;
				oldTime.tv_usec += oldTime.tv_usec % 1000000;
				gpnUsecTimePassCalculate(&oldTime, &nowTime, bTime);
				oldTime.tv_sec = nowTime.tv_sec;
				oldTime.tv_usec = nowTime.tv_usec;

				/*printf("%s : time pass not enoght : bTime(tv_sec %d tv_usec %d) oldTime(tv_sec %d tv_usec %d) nowTime(tv_sec %d tv_usec %d)\n\r",
					__FUNCTION__, bTime->tv_sec, bTime->tv_usec,\
					nowTime.tv_sec, nowTime.tv_usec,\
					oldTime.tv_sec, oldTime.tv_usec);*/
			}
		}
		else
		{
			/* oldTime >= nowTime */
			/* do now calculate */
			oldTime.tv_sec = nowTime.tv_sec;
			oldTime.tv_usec = nowTime.tv_usec;
			/* renew select timer out */
			bTime->tv_sec = pgtimerInfo->sigPeriod.tv_sec;
			bTime->tv_usec = pgtimerInfo->sigPeriod.tv_usec;
			
			/*printf("%s : time miss match : bTime(tv_sec %d tv_usec %d) nowTime(tv_sec %d tv_usec %d)\n\r",
				__FUNCTION__, bTime->tv_sec, bTime->tv_usec,\
				nowTime.tv_sec, nowTime.tv_usec);*/
		}
	}
	
	return GPN_TIMER_GEN_OK;
}

/********************************************************************************/
/*                                                    BASE Timer OPT                                                                */
/********************************************************************************/
UINT32 gpnBaseCycleTimerEnable(void)
{
	if(pgtimerInfo != NULL)
	{
		pgtimerInfo->en = GPN_TIMER_GEN_ENABLE;
		return GPN_TIMER_GEN_OK;
	}
	else
	{
		return GPN_TIMER_GEN_ERR;
	}
}
UINT32 gpnBaseCycleTimerDisable(void)
{
	if(pgtimerInfo != NULL)
	{
		pgtimerInfo->en = GPN_TIMER_GEN_DISABLE;
		return GPN_TIMER_GEN_OK;
	}
	else
	{
		return GPN_TIMER_GEN_ERR;
	}
}

void gpnTimerAdjustBaseTime(UINT32 oldTime, UINT32 newTime)
{	
	if(pgtimerInfo != NULL)
	{
		/* POCIX timer reset */
	    if( (pgtimerInfo->en == GPN_TIMER_GEN_ENABLE) &&\
			(pgtimerInfo->clcSrcType == GPN_TIMER_CLC_SRC_POCIX) )
		{
			gpnTimerPOCIXTimerAdjustTime();
		}

		/* calendar time change callback */
		if(pgtimerInfo->calCall != NULL)
		{
			pgtimerInfo->calCall(oldTime, newTime);
		}
	}
}

UINT32 gpnTimerSignalCycleCreat(UINT32 sigLen, UINT32 baseLen, baseTimerFuncPtr timerProc)
{
	/*signal lantch base timer start*/
	if(sigLen < GPN_TIME_TO_TICKS(1, 10ms))
	{
		sigLen = GPN_TIME_TO_TICKS(1, 10ms);
	}
	
	if(baseLen < sigLen)
	{
		baseLen = sigLen;
	}
	if( (baseLen % sigLen) != 0)
	{
		GPN_TIMER_PRINT(GPN_TIMER_MAN_CUP, "%s : sigTime(%d) baseTime(%d) error!\n\r",\
			__FUNCTION__, sigLen, baseLen);
		return GPN_TIMER_GEN_ERR;
	}

	pgtimerInfo->sigTickNum = sigLen;
	pgtimerInfo->sigPeriod.tv_sec = (sigLen*GPN_SYS_PER_TICK_MS)/1000;
	pgtimerInfo->sigPeriod.tv_usec = ((sigLen*GPN_SYS_PER_TICK_MS)%1000)*1000;
	
	/* reset timer base info*/
	pgtimerInfo->baseTickNum = baseLen;
	pgtimerInfo->basePeriod.tv_sec = (baseLen*GPN_SYS_PER_TICK_MS)/1000;
	pgtimerInfo->basePeriod.tv_usec = ((baseLen*GPN_SYS_PER_TICK_MS)%1000)*1000;

	if(pgtimerInfo->clcSrcType == GPN_TIMER_CLC_SRC_POCIX)
	{
		gpnTimerPOCIXTimerCreat(sigLen, timerProc);
	}

	return GPN_TIMER_GEN_OK;
}

/********************************************************************************/
/*                                                   GPN TIMER INIT                                                                 */
/********************************************************************************/
UINT32 gpnTimerInit(UINT32 sigLen, UINT32 baseLen)
{
	/*space malloc, data struct init*/
	gpnTimerSpaceInit();
	/*creat msg-quen for timer's use opt*/
	gpnTimerMsgQuenForUserOptInit();
	
	gpnTimerSignalCycleCreat(sigLen, baseLen, gpnTimerSigEventProc);
	
	return GPN_TIMER_GEN_OK;
}
INT32 gpnTimerFree(void)
{
	if(pgtimerInfo != NULL)
	{
		free(pgtimerInfo);
		pgtimerInfo	= NULL;
	}

	timer_delete(g_timerid);

	return GPN_TIMER_GEN_OK;
}

/********************************************************************************/
/*                                                    TIME CALCULATE                                                              */
/********************************************************************************/
UINT32 gpnUsecTimeIntervalCheck(struct timeval *firstUsec, struct timeval *secondUsec, UINT32 usecVal)
{	
	UINT32 usec;
	struct timeval usecA;
	struct timeval usecB;

	/*assert */
	if(( secondUsec->tv_sec < firstUsec->tv_sec ) ||
	   ( (secondUsec->tv_sec == firstUsec->tv_sec) &&
		 (secondUsec->tv_usec < firstUsec->tv_usec) ))
	{
		usecA.tv_sec = firstUsec->tv_sec;
		usecA.tv_usec = firstUsec->tv_usec;

		usecB.tv_sec = secondUsec->tv_sec;
		usecB.tv_usec = secondUsec->tv_usec;
	}
	else
	{
		usecA.tv_sec = secondUsec->tv_sec;
		usecA.tv_usec = secondUsec->tv_usec;

		usecB.tv_sec = firstUsec->tv_sec;
		usecB.tv_usec = firstUsec->tv_usec;
	}
	
	usec = (usecA.tv_sec - usecB.tv_sec) * 1000 * 1000;

	if( usecA.tv_usec > usecB.tv_usec )
	{
		/*both usecA's sec and usec do not small than usecB's, 
		  like 3.4 and 4.5, 4.4 and 4.5*/
		usec += (usecA.tv_usec - usecB.tv_usec);
	}
	else
	{
		/*usecA's sec do not small than usecB's sec 
		  but usecB's usec do not big than usecA' usec'
		  like 3.8 and 4.2 :
		  x.2 - y.8 	= (x-y) + (0.2 - 0.8)
		  		= (z + y -y) + (0.2 - 0.8)
		  		= z + (0.2 - 0.8)
		  		= z +1 + 0.2 - 0.8 -1;
		  x >= y ;
		  x - y = z ; 
		  z >= 0 							*/
			
		usec += 1000*1000;
		usec += usecA.tv_usec;
		usec -= usecB.tv_usec;
		usec -= 1000*1000;
	}

	if( usec < usecVal )
	{
		/*in interval usecVal */
		return GPN_TIMER_GEN_OK;
	}
	else
	{
		/*out of interval usecVal */
		return GPN_TIMER_GEN_ERR;
	}
}

UINT32 gpnUsecTimePassCheck(struct timeval *firstUsec, struct timeval *secondUsec, UINT32 usecVal)
{	
	UINT32 usec;

	/*assert */
	
	if(( secondUsec->tv_sec < firstUsec->tv_sec ) ||
	   ( (secondUsec->tv_sec == firstUsec->tv_sec) &&
		 (secondUsec->tv_usec < firstUsec->tv_usec) ))
	{
		/*second small than first, temp pass is nigetive */
		return GPN_TIMER_GEN_ERR;
	}
	else
	{
		usec = (secondUsec->tv_sec - firstUsec->tv_sec) * 1000 * 1000;

		if( secondUsec->tv_usec > firstUsec->tv_usec )
		{
			/*both second's sec and usec do not small than first's, 
			  like 3.4 and 4.5, 4.4 and 4.5*/
			usec += (secondUsec->tv_usec - firstUsec->tv_usec);
		}
		else
		{
			/*second's sec do not small than first's sec 
			  but second's usec do not big than first' usec'
			  like 3.8 and 4.2, */

			usec += (firstUsec->tv_usec - secondUsec->tv_usec);
			usec -= 1000*1000;
		}

		if( usec >= usecVal )
		{
			return GPN_TIMER_GEN_OK;
		}
		else
		{
			return GPN_TIMER_GEN_ERR;
		}
	}
}
UINT32 gpnUsecTimePassCalculate(struct timeval *firstUsec, struct timeval *secondUsec, struct timeval *passTime)
{	
	/*assert */
	
	if(( secondUsec->tv_sec < firstUsec->tv_sec ) ||
	   ( (secondUsec->tv_sec == firstUsec->tv_sec) &&
		 (secondUsec->tv_usec < firstUsec->tv_usec) ))
	{
		/*second small than first, temp pass is nigetive */
		
		return GPN_TIMER_GEN_ERR;
	}
	else
	{
		passTime->tv_sec = secondUsec->tv_sec - firstUsec->tv_sec;

		if( secondUsec->tv_usec > firstUsec->tv_usec )
		{
			/*both second's sec and usec do not small than first's, 
			  like 3.4 and 4.5, 4.4 and 4.5*/
			
			passTime->tv_usec = secondUsec->tv_usec - firstUsec->tv_usec;
		}
		else
		{
			/*second's sec do not small than first's sec 
			  but second's usec do not big than first' usec'
			  like 3.8 and 4.2, */
			passTime->tv_sec -= 1;  
			passTime->tv_usec = firstUsec->tv_usec - secondUsec->tv_usec;
		}

		return GPN_TIMER_GEN_OK;
	}
}

INT32 gpnUsecTimeCmp(struct timeval * firstUsec, struct timeval * secondUsec)
{	
	if(( secondUsec->tv_sec < firstUsec->tv_sec ) ||
	   ( (secondUsec->tv_sec == firstUsec->tv_sec) &&
		 (secondUsec->tv_usec < firstUsec->tv_usec) ))
	{
		return 1;/*firstUsec > secondUsec*/
	}
	else if((secondUsec->tv_sec == firstUsec->tv_sec) &&
		 	(secondUsec->tv_usec == firstUsec->tv_usec))
	{
		return 0;/*firstUsec == secondUsec*/
	}
	else
	{
		return -1;/*firstUsec < secondUsec*/
	}
}



#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_TIMER_SERVICE_C_ */

