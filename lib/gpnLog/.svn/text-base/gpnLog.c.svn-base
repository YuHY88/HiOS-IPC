/**********************************************************
* file name: gpnLog.c
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-10-30
* function: 
*    define GPN_LOG method
* modify:
*
***********************************************************/
#ifndef _GPN_LOG_C_
#define _GPN_LOG_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <syslog.h>
#include <log.h>

#include "gpnLog/gpnLogFuncApi.h"
#include "gpnLog/gpnLogUnifyPath.h"

#include "socketComm/gpnSpecialWarrper.h"

#ifndef GPN_LOG_ERR_LEVEL
const INT8 *logLevel[GPN_LOG_L_ALL+1] = 
{
	" <debug> ",
	" <info> ",
	" <notice> ",
	" <warning> ",
	" <err> ",
	" <crit> ",
	" <alert> ",
	" <emerg> ",
	" <err level> "
};
#endif

/*static INT8 gpnlogname[64] = GPN_DEFAULTB_LOG_ID;*/
stGpnLogInfo *pgGpnLogInfo = NULL;

static UINT32 gpnUnregisterLoghandler(gpnLogMethod *pLogM);
static gpnLogMethod *gpnRegisterLoghandler(UINT8 type, UINT8 high_pri, UINT8 low_pri);
static UINT32 gpnLoghandlerStdouterr(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str);
static UINT32 gpnLoghandlerSelfBFile(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str);
static UINT32 gpnLoghandlerSelfLFile(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str);
static UINT32 gpnLoghandlerSysLog(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str);
static UINT32 gpnLoghandlerAppFile(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str);
static UINT32 gpnLoghandlerCallBack(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str);
static UINT32 gpnLoghandlerNone(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str);
static UINT32 gpnLogDisable(gpnLogMethod *pLogM);
static UINT32 gpnDisableStdouterrLog(gpnLogMethod *pLogM);
static UINT32 gpnDisableSelfBFileLog(gpnLogMethod *pLogM);
static UINT32 gpnDisableSelfLFileLog(gpnLogMethod *pLogM);
static UINT32 gpnDisableSysLog(gpnLogMethod *pLogM);
static UINT32 gpnDisableAppFileLog(gpnLogMethod *pLogM);
static UINT32 gpnDisableCallBackLog(gpnLogMethod *pLogM);
static UINT32 gpnDisableNoneLog(gpnLogMethod *pLogM);
static UINT32 gpnLogEnable(gpnLogMethod *pLogM, INT8 *token, UINT8 type);
static UINT32 gpnEnableStdouterrLog(gpnLogMethod *pLogM, INT8 *token);
static UINT32 gpnEnableSelfBFileLog(gpnLogMethod *pLogM, INT8 *token);
static UINT32 gpnEnableSelfLFileLog(gpnLogMethod *pLogM, INT8 *token);
static UINT32 gpnEnableSysLog(gpnLogMethod *pLogM, INT8 *token);
static UINT32 gpnEnableAppFileLog(gpnLogMethod *pLogM, INT8 *token);
static UINT32 gpnEnableCallBackLog(gpnLogMethod *pLogM, INT8 *token);
static UINT32 gpnEnableNoneLog(gpnLogMethod *pLogM, INT8 *token);
static UINT32 gpnLogOptions(INT8 *token, UINT8 type, UINT8 high_pri, UINT8 low_pri, UINT32 enable);
static UINT32 gpnLogFindMethodNode(INT8 *token, UINT8 type, gpnLogMethod **ppLogM);
static UINT32 gpnFileLogFindRecordPoint(gpnLogMethod *pLogM);
static UINT32 gpnBFileLogSetLineMark(gpnLogMethod *pLogM);
static UINT32 gpnBFileLogRAMFlushInFile(gpnLogMethod *pLogM);
static UINT32 gpnLogGetProcessName(char *pname, UINT32 length);
static INT8* gpnSprintfSecStamp(time_t *now, INT8 *sbuf);
static INT8* gpnSprintfUsecStamp(struct timeval *now, INT8 *sbuf);
static UINT32 gpnLogString(UINT8 priority, const INT8 *str);


UINT32 gpnLogInit(void)
{
	UINT32 i;
	gpnLogMethod *pGpnLogM;
	stGpnLogInfo *pGpnLogInfo;

	/* init gpn_log global data struct */
	if(pgGpnLogInfo != NULL)
	{
		fprintf(stderr, "%s : gpn_log just inited before, err!\n\r",\
			__FUNCTION__);
		return GPN_LOG_ERR;
	}

	pgGpnLogInfo = (stGpnLogInfo *)malloc(sizeof(stGpnLogInfo));
	if(pgGpnLogInfo == NULL)
	{
		perror("malloc :\n\r");
		return GPN_LOG_ERR;
	}
	pGpnLogInfo = pgGpnLogInfo;
	
	listInit(&(pGpnLogInfo->gpnMUseList));
	pGpnLogInfo->useNum = 0;
	listInit(&(pGpnLogInfo->gpnMIdleList));
	pGpnLogInfo->idleNum = 0;
	
	for(i=0;i<GPN_LOG_METHOD_MAX_NODE;i++)
	{
		pGpnLogM = &(pGpnLogInfo->gLogM[i]);
		listAdd(&(pGpnLogInfo->gpnMIdleList), (NODE *)pGpnLogM);
		pGpnLogInfo->idleNum++;
	}

	/*get process name*/
	gpnLogGetProcessName(pGpnLogInfo->pcName, GPN_LOG_MAX_TOKEN);
	
	return GPN_LOG_OK;
}
UINT32 gpnLogFuncEnable(char *token, UINT32 logType, UINT8 high_pri, UINT8 low_pri)
{
	UINT32 reVal;
	char name[GPN_LOG_MAX_TOKEN];
	char userLog[GPN_LOG_MAX_TOKEN];
	
	if(token == NULL)
	{
		/*use default config log file : creat log file name and creat log file */
		gpnLogGetProcessName(name, GPN_LOG_MAX_TOKEN);
		snprintf(userLog, GPN_LOG_MAX_TOKEN, "%s%s",\
			GPN_LOG_UP_LOG_NODE, name);

		reVal = gpnLogOptions(userLog, logType, high_pri, low_pri, GPN_LOG_ENABLE);
		if(reVal == GPN_LOG_FUNC_API_GEN_OK)
		{
			gpnLog(low_pri, "%s : default log_type(%d) to \"%s\", care priority %d~%d\n\r",\
				__FUNCTION__, logType, userLog, high_pri, low_pri);
			GPN_LOG_FUNC_API_PRINT(GPN_LOG_FUNC_API_CMP, "%s : default log_type(%d) to \"%s\", care priority %d~%d\n\r",\
				__FUNCTION__, logType, userLog, high_pri, low_pri);
		}
		else
		{
			GPN_LOG_FUNC_API_PRINT(GPN_LOG_FUNC_API_CUP, "%s : default log_type(%d) to \"%s\", err!\n\r",\
				__FUNCTION__, logType, userLog);
			return GPN_LOG_FUNC_API_GEN_ERR;
		}
	}
	else
	{
		/*use user define */
		reVal = gpnLogOptions(token, logType, high_pri, low_pri, GPN_LOG_ENABLE);
		if(reVal == GPN_LOG_FUNC_API_GEN_OK)
		{
			gpnLog(low_pri, "%s : user define log_type(%d) to \"%s\", care priority %d~%d\n\r",\
				__FUNCTION__, logType, token, high_pri, low_pri);
			GPN_LOG_FUNC_API_PRINT(GPN_LOG_FUNC_API_CMP, "%s : user define log_type(%d) to \"%s\", care priority %d~%d\n\r",\
				__FUNCTION__, logType, token, high_pri, low_pri);
		}
		else
		{
			GPN_LOG_FUNC_API_PRINT(GPN_LOG_FUNC_API_CUP, "%s : user define log_type(%d) to \"%s\", err!\n\r",\
				__FUNCTION__, logType, token);
			return GPN_LOG_FUNC_API_GEN_ERR;
		}
	}

	return GPN_LOG_FUNC_API_GEN_OK;
}
UINT32 gpnLogFuncDisable(char *token, UINT32 logType)
{
	UINT32 reVal;
	char name[GPN_LOG_MAX_TOKEN];
	char userLog[GPN_LOG_MAX_TOKEN];

	if(token == NULL)
	{
		/*use default config log file : creat log file name and creat log file */
		gpnLogGetProcessName(name, GPN_LOG_MAX_TOKEN);
		snprintf(userLog, GPN_LOG_MAX_TOKEN, "%s%s",\
			GPN_LOG_UP_LOG_NODE, name);
		
		reVal = gpnLogOptions(token, logType, 0, 0, GPN_LOG_DISABLE);
	}
	else
	{
		reVal = gpnLogOptions(token, logType, 0, 0, GPN_LOG_DISABLE);
	}
	if(reVal == GPN_LOG_FUNC_API_GEN_OK)
	{
		GPN_LOG_FUNC_API_PRINT(GPN_LOG_FUNC_API_CUP, "%s : disable log_type(%d) to \"%s\", ok!\n\r",\
			__FUNCTION__, logType, token);
	}
	else
	{
		GPN_LOG_FUNC_API_PRINT(GPN_LOG_FUNC_API_CMP, "%s : disable log_type(%d) to \"%s\", err!\n\r",\
			__FUNCTION__, logType, token);
	}
	
	return GPN_LOG_FUNC_API_GEN_OK;
}

static UINT32 gpnUnregisterLoghandler(gpnLogMethod *pLogM)
{
	if(pLogM != NULL)
	{
		if(0 != listFind(&(pgGpnLogInfo->gpnMUseList), (NODE *)pLogM))
		{
			listDelete(&(pgGpnLogInfo->gpnMUseList), (NODE *)pLogM);
			if(pgGpnLogInfo->useNum > 0)
			{
				pgGpnLogInfo->useNum--;
			}
			listAdd(&(pgGpnLogInfo->gpnMIdleList), (NODE *)pLogM);
			pgGpnLogInfo->idleNum++;
			
			return GPN_LOG_OK;
		}
	}
	
	return GPN_LOG_ERR;
}
static gpnLogMethod *gpnRegisterLoghandler(UINT8 type, UINT8 high_pri, UINT8 low_pri)
{
	UINT32 logMNum;
	stGpnLogInfo *pGpnLogInfo;
    gpnLogMethod *pLogM;
	gpnLogMethod *pLogM2;

	pGpnLogInfo = pgGpnLogInfo;
	
    pLogM = (gpnLogMethod *)listGet(&(pGpnLogInfo->gpnMIdleList));
    if(pLogM == NULL)
    {
        return NULL;
    }
	if(pgGpnLogInfo->idleNum > 0)
	{
		pgGpnLogInfo->idleNum--;
	}

	pLogM->max_pri = high_pri;
	pLogM->min_pri = low_pri;
    pLogM->enable = GPN_LOG_DISABLE;
	memset(pLogM->token, 0, GPN_LOG_MAX_TOKEN);
	memcpy(pLogM->pcName, pgGpnLogInfo->pcName, GPN_LOG_MAX_TOKEN);

	pLogM->reckInfo.blockTh = 0;
	pLogM->reckInfo.buffUse = GPN_LOG_FILE_BLCK_MARK_SIZE;
	pLogM->reckInfo.buffWtStar = GPN_LOG_FILE_BLCK_MARK_SIZE;
	memset(pLogM->reckInfo.bkBuff, 0, GPN_LOG_FILE_BLOCK_BUFF_SIZE);
	
    pLogM->type = type;
    switch( type )
	{
    	case GPN_LOGHANDLER_STDOUT:
        	pLogM->magic  = GPN_LOG_NO;
        	/* fallthrough */
    	case GPN_LOGHANDLER_STDERR:
			pLogM->magic  = GPN_LOG_YES;
        	pLogM->handler = gpnLoghandlerStdouterr;
        	break;

	    case GPN_LOGHANDLER_BFILE:
	        pLogM->handler = gpnLoghandlerSelfBFile;
	        break;

		case GPN_LOGHANDLER_LFILE:
	        pLogM->handler = gpnLoghandlerSelfLFile;
	        break;
			
	    case GPN_LOGHANDLER_SYSLOG:
	        pLogM->handler = gpnLoghandlerSysLog;
	        break;
			
		case GPN_LOGHANDLER_APPLOG:
	        pLogM->handler = gpnLoghandlerAppFile;
	        break;
			
	    case GPN_LOGHANDLER_CALLBACK:
	        pLogM->handler = gpnLoghandlerCallBack;
	        break;
			
	    case GPN_LOGHANDLER_NONE:
	        pLogM->handler = gpnLoghandlerNone;
	        break;
			
	    default:
			listAdd(&(pGpnLogInfo->gpnMIdleList), (NODE *)pLogM);
			pgGpnLogInfo->idleNum++;
	        return NULL;
    }
	
	/* low priority ahead, high priority behind */
	logMNum = 0;
	pLogM2 = (gpnLogMethod *)listFirst(&(pGpnLogInfo->gpnMUseList));
	while((pLogM2 != NULL)&&(logMNum<pGpnLogInfo->useNum))
	{
		if(pLogM2->min_pri > pLogM->min_pri)
		{
			break;
		}
		logMNum++;
		pLogM2 = (gpnLogMethod *)listNext((NODE *)(pLogM2));
	}
	
	if(pLogM2 == NULL)
	{
		/*gpnMUseList is empty or pLogM should add at the list tail*/
		listAdd(&(pGpnLogInfo->gpnMUseList), (NODE *)pLogM);
		pgGpnLogInfo->useNum++;
	}
	else if(logMNum < pGpnLogInfo->useNum)
	{
		/*insert in middle*/
		/*if pLogM2 is first Node ,it's previous Node is NULL*/
		pLogM2 = (gpnLogMethod *)listPrevious((NODE *)pLogM2);
		listInsert(&(pGpnLogInfo->gpnMUseList), (NODE *)pLogM2, (NODE *)pLogM);
		pgGpnLogInfo->useNum++;
	}
    else
    {
		/*err*/
		listAdd(&(pGpnLogInfo->gpnMIdleList), (NODE *)pLogM);
		pgGpnLogInfo->idleNum++;
        return NULL;
    }

    return pLogM;
}

static UINT32 gpnLoghandlerStdouterr(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str)
{
	INT8 sbuf[GPN_LOG_FILE_TIME_STAMP_LEN];
	stGpnLogInfo *pGpnLogInfo;

	if( (pLogM != NULL) &&\
		((pLogM->type == GPN_LOGHANDLER_STDOUT)||\
		 (pLogM->type == GPN_LOGHANDLER_STDERR)) &&\
		(str != NULL) )
	{
		return GPN_LOG_ERR;
	}
	
	pGpnLogInfo = pgGpnLogInfo;
	
	gpnSprintfSecStamp(NULL, sbuf);
	/*gpnSprintfUsecStamp(NULL, sbuf);*/

	if(priority > GPN_LOG_L_ALL)
	{
		priority = GPN_LOG_L_ALL;
	}
	
	if(pLogM->enable == GPN_LOG_ENABLE)
	{
	    if(pLogM->magic == GPN_LOG_NO)
	    {
	       printf(         "%s%s %s:%s%s", sbuf, pGpnLogInfo->pcName, pLogM->token, logLevel[priority], str);
	    }
	    else
	    {
	       fprintf(stderr, "%s%s %s:%s%s", sbuf, pGpnLogInfo->pcName, pLogM->token, logLevel[priority], str);
	    }
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnLoghandlerSelfBFile(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str)
{
	UINT32 lineLen;
	INT8 sbuf[GPN_LOG_FILE_TIME_STAMP_LEN];
	INT8 line[GPN_LOG_FILE_MAX_LINE];
	
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_BFILE) &&\
		(pLogM->enable == GPN_LOG_ENABLE) &&\
		(str != NULL) )
	{
		gpnSprintfSecStamp(NULL, sbuf);
		/*gpnSprintfUsecStamp(NULL, sbuf);*/

		if(priority > GPN_LOG_L_ALL)
		{
			priority = GPN_LOG_L_ALL;
		}

		/*we should check is file opened ???*/
		
		/*if str too long,cut short*/
		snprintf(line, GPN_LOG_FILE_MAX_LINE, "%s%s %s:%s%s", sbuf, pLogM->pcName, pLogM->token, logLevel[priority], str);
		lineLen = strlen(line);
		if( (pLogM->reckInfo.buffUse+lineLen)\
			<= (GPN_LOG_FILE_BLOCK_SIZE) )
		{
			/*in one block*/
			sprintf(&(pLogM->reckInfo.bkBuff[pLogM->reckInfo.buffUse]), "%s", line);
			pLogM->reckInfo.buffUse += lineLen;
		}
		else
		{
			/*write whole block*/
			memcpy(pLogM->reckInfo.bkBuff,\
				GPN_LOG_FILE_BLCK_MARK, GPN_LOG_FILE_BLCK_MARK_SIZE);
			memcpy(&(pLogM->reckInfo.bkBuff[GPN_LOG_FILE_BLOCK_SIZE]),\
				GPN_LOG_FILE_BLCK_FREE_MARK, GPN_LOG_FILE_BLCK_MARK_SIZE);
				
			if((pLogM->reckInfo.blockTh+1) == GPN_LOG_FILE_BLOCK_NUM)
			{
				/*last block:write first block mark first*/
				fseek(pLogM->fd, 0, SEEK_SET);
				fwrite(GPN_LOG_FILE_BLCK_FREE_MARK,GPN_LOG_FILE_BLCK_MARK_SIZE, 1, pLogM->fd);
				fseek(pLogM->fd, (pLogM->reckInfo.blockTh*GPN_LOG_FILE_BLOCK_SIZE), SEEK_SET);
				fwrite(pLogM->reckInfo.bkBuff,GPN_LOG_FILE_BLOCK_SIZE, 1, pLogM->fd);
			}
			else
			{
				/*normal block*/
				fseek(pLogM->fd, (pLogM->reckInfo.blockTh*GPN_LOG_FILE_BLOCK_SIZE), SEEK_SET);
				fwrite(pLogM->reckInfo.bkBuff,GPN_LOG_FILE_BLOCK_BUFF_SIZE, 1, pLogM->fd);
			}

			/*for new block*/
			memset(pLogM->reckInfo.bkBuff, 0, GPN_LOG_FILE_BLOCK_BUFF_SIZE);
			pLogM->reckInfo.blockTh = ((pLogM->reckInfo.blockTh+1)%GPN_LOG_FILE_BLOCK_NUM);
			pLogM->reckInfo.buffUse = GPN_LOG_FILE_BLCK_MARK_SIZE;
			pLogM->reckInfo.buffWtStar = GPN_LOG_FILE_BLCK_MARK_SIZE;
			fseek(pLogM->fd, (pLogM->reckInfo.blockTh*GPN_LOG_FILE_BLOCK_SIZE), SEEK_SET);

			/*in new block*/
			sprintf(&(pLogM->reckInfo.bkBuff[pLogM->reckInfo.buffUse]), "%s", line);
			pLogM->reckInfo.buffUse += lineLen;
		}
		gpnBFileLogRAMFlushInFile(pLogM);
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnLoghandlerSelfLFile(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str)
{
	UINT32 lineLen;
	INT8 sbuf[GPN_LOG_FILE_TIME_STAMP_LEN];
	INT8 line[GPN_LOG_FILE_MAX_LINE];
	
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_LFILE) &&\
		(pLogM->enable == GPN_LOG_ENABLE) &&\
		(str != NULL) )
	{
		/*gpnSprintfSecStamp(NULL, sbuf);*/
		gpnSprintfUsecStamp(NULL, sbuf);

		if(priority > GPN_LOG_L_ALL)
		{
			priority = GPN_LOG_L_ALL;
		}

		/*we should check is file opened ???*/
		if(pLogM->magic != GPN_LOG_YES)
		{
        	pLogM->fd = fopen(pLogM->token, "w+");
        	if(!pLogM->fd)
        	{
            	return GPN_LOG_ERR;
        	}
        	pLogM->magic = GPN_LOG_YES;
    	}
		/*if str too long,cut short*/
		snprintf(line, GPN_LOG_FILE_MAX_LINE, "%s%s %s:%s%s", sbuf, pLogM->pcName, pLogM->token, logLevel[priority], str);
		lineLen = strlen(line);
		if( (pLogM->reckInfo.buffUse+lineLen)\
				> (GPN_LOG_FILE_BLOCK_SIZE) )
		{
			/*write this info in next block, so make  "block mark" happen*/
			pLogM->reckInfo.blockTh = ((pLogM->reckInfo.blockTh+1)%GPN_LOG_FILE_BLOCK_NUM);
			pLogM->reckInfo.buffUse = 0;
			pLogM->reckInfo.buffWtStar = 0;
		}
		
		if(pLogM->reckInfo.buffUse == 0)
		{
			/*here we do LFile file-start Mark proc*/
			/*write whole block*/
			memset(pLogM->reckInfo.bkBuff, 0, GPN_LOG_FILE_BLOCK_BUFF_SIZE);
			memcpy(pLogM->reckInfo.bkBuff,\
				GPN_LOG_FILE_BLCK_MARK, GPN_LOG_FILE_BLCK_MARK_SIZE);
			memcpy(&(pLogM->reckInfo.bkBuff[GPN_LOG_FILE_BLOCK_SIZE]),\
				GPN_LOG_FILE_BLCK_FREE_MARK, GPN_LOG_FILE_BLCK_MARK_SIZE);
			
			if((pLogM->reckInfo.blockTh+1) == GPN_LOG_FILE_BLOCK_NUM)
			{
				/*block start :write first block free-mark first, for next use*/
				fseek(pLogM->fd, 0, SEEK_SET);
				fwrite(GPN_LOG_FILE_BLCK_FREE_MARK,GPN_LOG_FILE_BLCK_MARK_SIZE, 1, pLogM->fd);
				/*clear now block, for now use*/
				fseek(pLogM->fd, (pLogM->reckInfo.blockTh*GPN_LOG_FILE_BLOCK_SIZE), SEEK_SET);
				fwrite(pLogM->reckInfo.bkBuff,GPN_LOG_FILE_BLOCK_SIZE, 1, pLogM->fd);
			}
			else
			{
				/*normal block*/
				fseek(pLogM->fd, (pLogM->reckInfo.blockTh*GPN_LOG_FILE_BLOCK_SIZE), SEEK_SET);
				fwrite(pLogM->reckInfo.bkBuff,GPN_LOG_FILE_BLOCK_BUFF_SIZE, 1, pLogM->fd);
			}

			/*for now block*/
			pLogM->reckInfo.buffUse = GPN_LOG_FILE_BLCK_MARK_SIZE;
			pLogM->reckInfo.buffWtStar = GPN_LOG_FILE_BLCK_MARK_SIZE;
			fseek(pLogM->fd, (pLogM->reckInfo.blockTh*GPN_LOG_FILE_BLOCK_SIZE +\
				GPN_LOG_FILE_BLCK_MARK_SIZE), SEEK_SET);
		}
		
		/*write line info to block*/
		fwrite(line,lineLen, 1, pLogM->fd);
		fflush(pLogM->fd);
		pLogM->reckInfo.buffUse += lineLen;
		pLogM->reckInfo.buffWtStar += lineLen;
		
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnLoghandlerSysLog(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_SYSLOG) &&\
		(pLogM->enable == GPN_LOG_ENABLE) &&\
		(str != NULL) )
	{
		if(pLogM->magic != GPN_LOG_YES)
		{
			openlog(pLogM->token, LOG_CONS | LOG_PID, LOG_USER);
			pLogM->magic = GPN_LOG_YES;
		}
		syslog(priority, "%s", str);
		
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnLoghandlerAppFile(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str)
{
	//UINT32 lineLen;
	INT8 sbuf[GPN_LOG_FILE_TIME_STAMP_LEN];
	INT8 line[GPN_LOG_FILE_MAX_LINE];
	stGpnLogInfo *pGpnLogInfo;

	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_APPLOG) &&\
		(pLogM->enable == GPN_LOG_ENABLE) &&\
		(str != NULL) )
	{
		pGpnLogInfo = pgGpnLogInfo;
		
		/*gpnSprintfSecStamp(NULL, sbuf);*/
		gpnSprintfUsecStamp(NULL, sbuf);

		if(priority > GPN_LOG_L_ALL)
		{
			priority = GPN_LOG_L_ALL;
		}
		
		/*if str too long,cut short*/
		snprintf(line, GPN_LOG_FILE_MAX_LINE, "%s%s %s:%s%s", sbuf, pGpnLogInfo->pcName, pLogM->token, logLevel[priority], str);
		//lineLen = strlen(line);

		/*tx log text to log-process*/
		printf("%s", line);

		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnLoghandlerCallBack(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str)
{
	//UINT32 lineLen;
	INT8 sbuf[GPN_LOG_FILE_TIME_STAMP_LEN];
	INT8 line[GPN_LOG_FILE_MAX_LINE];
	stGpnLogInfo *pGpnLogInfo;

	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_APPLOG) &&\
		(pLogM->enable == GPN_LOG_ENABLE) &&\
		(str != NULL) )
	{
		pGpnLogInfo = pgGpnLogInfo;
		
		/*gpnSprintfSecStamp(NULL, sbuf);*/
		gpnSprintfUsecStamp(NULL, sbuf);

		if(priority > GPN_LOG_L_ALL)
		{
			priority = GPN_LOG_L_ALL;
		}
		
		/*if str too long,cut short*/
		snprintf(line, GPN_LOG_FILE_MAX_LINE, "%s%s %s:%s%s", sbuf, pGpnLogInfo->pcName, pLogM->token, logLevel[priority], str);
		//lineLen = strlen(line);

		/*CALLBACK ADD*/
		
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnLoghandlerNone(gpnLogMethod *pLogM, UINT8 priority, const INT8 *str)
{
	/*do nothing, just test code*/
	return GPN_LOG_OK;
}

static UINT32 gpnLogDisable(gpnLogMethod *pLogM)
{
	if(pLogM == NULL)
	{
		return GPN_LOG_ERR;
	}
	switch(pLogM->type)
	{
    	case GPN_LOGHANDLER_STDOUT:
        	/*fallthrough */
    	case GPN_LOGHANDLER_STDERR:
        	gpnDisableStdouterrLog(pLogM);
        	break;

	    case GPN_LOGHANDLER_BFILE:
	        gpnDisableSelfBFileLog(pLogM);
	        break;

		case GPN_LOGHANDLER_LFILE:
	        gpnDisableSelfLFileLog(pLogM);
	        break;
			
	    case GPN_LOGHANDLER_SYSLOG:
	        gpnDisableSysLog(pLogM);
	        break;
			
		case GPN_LOGHANDLER_APPLOG:
	        gpnDisableAppFileLog(pLogM);
	        break;
			
	    case GPN_LOGHANDLER_CALLBACK:
	       	gpnDisableCallBackLog(pLogM);
	        break;
			
	    case GPN_LOGHANDLER_NONE:
	       	gpnDisableNoneLog(pLogM);
	        break;
			
	    default:
	        return GPN_LOG_ERR;
    }
	return GPN_LOG_OK;
}
static UINT32 gpnDisableStdouterrLog(gpnLogMethod *pLogM)
{
	if( (pLogM != NULL) &&\
		((pLogM->type == GPN_LOGHANDLER_STDOUT)||\
		 (pLogM->type == GPN_LOGHANDLER_STDERR)) )
	{
		pLogM->enable = GPN_LOG_DISABLE;
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnDisableSelfBFileLog(gpnLogMethod *pLogM)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_BFILE) )
	{
		/*write what we have
		fflush(pLogM->fd);*/
		pLogM->enable = GPN_LOG_DISABLE;
		gpnBFileLogSetLineMark(pLogM);
		fclose(pLogM->fd);
        pLogM->magic = GPN_LOG_NO;
    	
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnDisableSelfLFileLog(gpnLogMethod *pLogM)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_LFILE) )
	{
		/*write what we have*/
		pLogM->enable = GPN_LOG_DISABLE;
		fclose(pLogM->fd);
        pLogM->magic = GPN_LOG_NO;
    	
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnDisableSysLog(gpnLogMethod *pLogM)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_SYSLOG) )
	{
		pLogM->enable = GPN_LOG_DISABLE;
		closelog();
    	pLogM->magic = GPN_LOG_NO;
    	
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnDisableAppFileLog(gpnLogMethod *pLogM)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_APPLOG) )
	{
		pLogM->enable = GPN_LOG_DISABLE;
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnDisableCallBackLog(gpnLogMethod *pLogM)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_CALLBACK) )
	{
		pLogM->enable = GPN_LOG_DISABLE;
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnDisableNoneLog(gpnLogMethod *pLogM)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_NONE) )
	{
		pLogM->enable = GPN_LOG_DISABLE;
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}

static UINT32 gpnLogEnable(gpnLogMethod *pLogM, INT8 *token, UINT8 type)
{
	if( (pLogM == NULL) ||\
		(token == NULL) )
	{
		return GPN_LOG_ERR;
	}
	switch( type )
	{
    	case GPN_LOGHANDLER_STDOUT:
        	/*fallthrough */
    	case GPN_LOGHANDLER_STDERR:
        	gpnEnableStdouterrLog(pLogM, token);
        	break;

	    case GPN_LOGHANDLER_BFILE:
	        gpnEnableSelfBFileLog(pLogM, token);
	        break;
			
		case GPN_LOGHANDLER_LFILE:
	        gpnEnableSelfLFileLog(pLogM, token);
	        break;
			
	    case GPN_LOGHANDLER_SYSLOG:
	        gpnEnableSysLog(pLogM, token);
	        break;
			
		case GPN_LOGHANDLER_APPLOG:
	        gpnEnableAppFileLog(pLogM, token);
	        break;
			
	    case GPN_LOGHANDLER_CALLBACK:
	        gpnEnableCallBackLog(pLogM, token);
	        break;
			
	    case GPN_LOGHANDLER_NONE:
	        gpnEnableNoneLog(pLogM, token);
	        break;
			
	    default:
	        return GPN_LOG_ERR;
    }
	return GPN_LOG_OK;
}
static UINT32 gpnEnableStdouterrLog(gpnLogMethod *pLogM, INT8 *token)
{
	if( (pLogM != NULL) &&\
		((pLogM->type == GPN_LOGHANDLER_STDOUT) ||\
		(pLogM->type == GPN_LOGHANDLER_STDERR)) &&\
		(token != NULL) )
	{
		pLogM->enable = GPN_LOG_ENABLE;
		snprintf(pLogM->token, GPN_LOG_MAX_TOKEN, "%s", token);
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnEnableSelfBFileLog(gpnLogMethod *pLogM, INT8 *token)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_BFILE) &&\
		(token != NULL) )
	{
		snprintf(pLogM->token, GPN_LOG_MAX_TOKEN, "%s", token);
		if(access(pLogM->token,F_OK)==0)
		{
			pLogM->fd = fopen(pLogM->token,"rw+");
			if(pLogM->fd == NULL)
			{
				perror("open");
				return GPN_LOG_ERR;
			}
		}
		else
		{
			/*creat new file,fix size */
			pLogM->fd = fopen(pLogM->token,"w+");
			if(pLogM->fd == NULL)
			{
				perror("creat open");
				return GPN_LOG_ERR;
			}
			fseek(pLogM->fd, GPN_LOG_FILE_SIZE-1, SEEK_SET);
			fwrite(pLogM->reckInfo.bkBuff,1, 1, pLogM->fd);
			fflush(pLogM->fd);
			fseek(pLogM->fd, 0, SEEK_SET);
		}
		/*open file ok*/
		pLogM->magic = GPN_LOG_YES;
		
		/*buff set: BOLCK fifo
		setvbuf(pLogM->fd, NULL, _IOFBF, GPN_LOG_FILE_BLOCK_SIZE);
		pLogM->imagic = GPN_LOG_BLOCK_FIFO;*/
		/*buff set: line fifo
		setvbuf(pLogM->fd, NULL, _IOLBF, GPN_LOG_FILE_BLOCK_SIZE);
		pLogM->imagic = GPN_LOG_LINE_FIFO;*/
		/*buff set: BOLCK fifo*/
		setvbuf(pLogM->fd, NULL, _IONBF, 0);
		pLogM->imagic = GPN_LOG_NONE_FIFO;

		/*find record point*/
		gpnFileLogFindRecordPoint(pLogM);
		
		pLogM->enable = GPN_LOG_ENABLE;
		
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnEnableSelfLFileLog(gpnLogMethod *pLogM, INT8 *token)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_LFILE) &&\
		(token != NULL) )
	{
		snprintf(pLogM->token, GPN_LOG_MAX_TOKEN, "%s", token);
		if(access(pLogM->token,F_OK)==0)
		{
			pLogM->fd = fopen(pLogM->token,"rw+");
			if(pLogM->fd == NULL)
			{
				perror("open");
				return GPN_LOG_ERR;
			}
		}
		else
		{
			/*creat new file,fix size */
			pLogM->fd = fopen(pLogM->token,"w+");
			if(pLogM->fd == NULL)
			{
				perror("creat open");
				return GPN_LOG_ERR;
			}
			fseek(pLogM->fd, GPN_LOG_FILE_SIZE-1, SEEK_SET);
			fwrite(pLogM->reckInfo.bkBuff,1, 1, pLogM->fd);
			fflush(pLogM->fd);
			fseek(pLogM->fd, 0, SEEK_SET);
		}
		/*open file ok*/
		pLogM->magic = GPN_LOG_YES;
		
		/*buff set: BOLCK fifo
		setvbuf(pLogM->fd, NULL, _IOFBF, GPN_LOG_FILE_BLOCK_SIZE);
		pLogM->imagic = GPN_LOG_BLOCK_FIFO;*/
		/*buff set: line fifo*/
		setvbuf(pLogM->fd, NULL, _IOLBF, GPN_LOG_FILE_MAX_LINE);
		pLogM->imagic = GPN_LOG_LINE_FIFO;
		/*buff set: BOLCK fifo
		setvbuf(pLogM->fd, NULL, _IONBF, 0);
		pLogM->imagic = GPN_LOG_NONE_FIFO;*/

		/*find record point*/
		gpnFileLogFindRecordPoint(pLogM);
		
		pLogM->enable = GPN_LOG_ENABLE;
		
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnEnableSysLog(gpnLogMethod *pLogM, INT8 *token)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_SYSLOG) )
	{
		gpnLogDisable(pLogM);
		pLogM->enable = GPN_LOG_ENABLE;
		if(token != NULL)
		{
			snprintf(pLogM->token, GPN_LOG_MAX_TOKEN, "%s", token);
		}
		else
		{
			snprintf(pLogM->token, GPN_LOG_MAX_TOKEN, "%s", pgGpnLogInfo->pcName);
		}
		openlog(pLogM->token, LOG_CONS | LOG_PID, LOG_USER);
		pLogM->magic = GPN_LOG_YES;

		return GPN_LOG_OK;
    }
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnEnableAppFileLog(gpnLogMethod *pLogM, INT8 *token)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_APPLOG) &&\
		(token != NULL) )
	{
		pLogM->enable = GPN_LOG_ENABLE;
		snprintf(pLogM->token, GPN_LOG_MAX_TOKEN, "%s", token);
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnEnableCallBackLog(gpnLogMethod *pLogM, INT8 *token)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_CALLBACK) &&\
		(token != NULL) )
	{
		pLogM->enable = GPN_LOG_ENABLE;
		snprintf(pLogM->token, GPN_LOG_MAX_TOKEN, "%s", token);
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnEnableNoneLog(gpnLogMethod *pLogM, INT8 *token)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_NONE) &&\
		(token != NULL) )
	{
		pLogM->enable = GPN_LOG_ENABLE;
		snprintf(pLogM->token, GPN_LOG_MAX_TOKEN, "%s", token);
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}

static UINT32 gpnLogOptions(INT8 *token, UINT8 type, UINT8 high_pri, UINT8 low_pri, UINT32 enable)
{
	//stGpnLogInfo *pGpnLogInfo;
    gpnLogMethod *pLogM;

	/*assert */
	if(token == NULL)
	{
		return GPN_LOG_ERR;
	}
	if(strlen(token) > GPN_LOG_MAX_TOKEN)
	{
		return GPN_LOG_ERR;
	}

	if(high_pri > GPN_LOG_L_ALL)
	{
		high_pri = GPN_LOG_L_EMERG;
	}
	if(low_pri > high_pri)
	{
		low_pri = high_pri;
	}
	
	//pGpnLogInfo = pgGpnLogInfo;
	
	if(gpnLogFindMethodNode(token, type, &pLogM) == GPN_LOG_ERR)
	{
		return GPN_LOG_ERR;
	}

	/* enable new log pLogM */
	if( (pLogM == NULL) &&\
		(enable == GPN_LOG_ENABLE) )
	{
		pLogM = gpnRegisterLoghandler(type, high_pri, low_pri);
		if(pLogM == NULL)
		{
			return GPN_LOG_ERR;
		}
		gpnLogEnable(pLogM, token, type);
	}
	/* disable old log pLogM */
	else if((pLogM != NULL) &&\
			(enable == GPN_LOG_DISABLE))
	{
		gpnLogDisable(pLogM);
		gpnUnregisterLoghandler(pLogM);
	}
	/* reenable old log pLogM, error */
	else if((pLogM != NULL) &&\
			(enable == GPN_LOG_ENABLE))
	{
		return GPN_LOG_ERR;
	}
	/* disable empty log pLogM, error*/
	else if((pLogM == NULL) &&\
			(enable == GPN_LOG_DISABLE))
	{
		return GPN_LOG_ERR;
	}
	
	return GPN_LOG_OK;
}

static UINT32 gpnLogFindMethodNode(INT8 *token, UINT8 type, gpnLogMethod **ppLogM)
{
	UINT32 logMNum;
	gpnLogMethod *pLogM;
	stGpnLogInfo *pGpnLogInfo;

	/* assert */
	if(ppLogM == NULL)
	{
		return GPN_LOG_ERR;
	}
	
	pGpnLogInfo = pgGpnLogInfo;
	
	logMNum = 0;
	pLogM = (gpnLogMethod *)listFirst(&(pGpnLogInfo->gpnMUseList));
	while((pLogM != NULL)&&(logMNum<pGpnLogInfo->useNum))
	{
		if(pLogM->type == type)
		{
			if( (type == GPN_LOGHANDLER_STDOUT) ||\
		    	(type == GPN_LOGHANDLER_STDERR) ||\
				(type == GPN_LOGHANDLER_SYSLOG) ||\
			    (type == GPN_LOGHANDLER_CALLBACK) ||\
			    (type == GPN_LOGHANDLER_NONE) )
			{
				/*these log methods do not have more
				  than one pLogM struct, and new do 
				  not accept new token             */
				  
				*ppLogM = pLogM;
			    return GPN_LOG_OK;
			}
			else if((type == GPN_LOGHANDLER_BFILE) ||\
					(type == GPN_LOGHANDLER_LFILE) ||\
		    		(type == GPN_LOGHANDLER_APPLOG))
			{
				/*these log methods can register 
				  more than one pLogM struct, but
				  token should not NULL and can not
				  be same with other pLogM */
				if(token == NULL)
				{
					*ppLogM = NULL;
					return GPN_LOG_ERR;
				}
		        if( (strncmp(pLogM->token, token, strlen(pLogM->token)) == 0) &&\
					(strncmp(token, pLogM->token, strlen(token)) == 0) )
				{
					*ppLogM = pLogM;
				    return GPN_LOG_OK;
				}
		    }
		}
		
		logMNum++;
		pLogM = (gpnLogMethod *)listNext((NODE *)(pLogM));
	}

	*ppLogM = NULL;
	return GPN_LOG_OK;
}
static UINT32 gpnFileLogFindRecordPoint(gpnLogMethod *pLogM)
{
	UINT32 i;
	UINT32 j;
	UINT32 bkMarkOff;
	UINT32 lMarkOff;
	UINT32 isBFind;
	UINT32 isLFind;
	INT8 blckMark[GPN_LOG_FILE_BLCK_MARK_SIZE];

	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_BFILE) )
	{
		isBFind = GPN_LOG_NO;
		isLFind = GPN_LOG_NO;
		
		for(i=0;i<GPN_LOG_FILE_BLOCK_NUM;i++)
		{
			bkMarkOff = GPN_LOG_FILE_BLOCK_SIZE*i;
			fseek(pLogM->fd, bkMarkOff, SEEK_SET);
			if(fread(blckMark, GPN_LOG_FILE_BLCK_MARK_SIZE, 1, pLogM->fd) == 0)
			{
				continue;
			}
			if(strncmp(blckMark, GPN_LOG_FILE_BLCK_MARK, GPN_LOG_FILE_BLCK_MARK_SIZE-2))
			{
				isBFind = GPN_LOG_YES;
				/*here find block, then find line*/
				bkMarkOff = GPN_LOG_FILE_BLOCK_SIZE*i;
				fseek(pLogM->fd, bkMarkOff, SEEK_SET);
				/*read whole block info into RAM*/
				if(fread(pLogM->reckInfo.bkBuff, GPN_LOG_FILE_BLOCK_SIZE, 1, pLogM->fd) == 0)
				{
					return GPN_LOG_ERR;
				}

				for(j=0;j<GPN_LOG_FILE_VLINE_NUM;j++)
				{
					lMarkOff = GPN_LOG_FILE_VLINE_SIZE*j;
					
					if(!strncmp(&(pLogM->reckInfo.bkBuff[lMarkOff]),\
						GPN_LOG_FILE_VLINE_MARK, GPN_LOG_FILE_VLINE_MARK_SIZE-2))
					{
						isLFind = GPN_LOG_YES;
						break;
					}
				}
				/*init record info*/
				if(isLFind == GPN_LOG_NO)
				{
					/*not find rigth line, start new*/
					pLogM->reckInfo.blockTh = i;
					pLogM->reckInfo.buffUse = GPN_LOG_FILE_BLCK_MARK_SIZE;
					pLogM->reckInfo.buffWtStar = GPN_LOG_FILE_BLCK_MARK_SIZE;
				}
				else
				{
					pLogM->reckInfo.blockTh = i;
					pLogM->reckInfo.buffUse = (GPN_LOG_FILE_VLINE_SIZE*j);
					pLogM->reckInfo.buffWtStar = (GPN_LOG_FILE_VLINE_SIZE*j);
				}
				
				break;
			}
		}
		if(isBFind == GPN_LOG_NO)
		{
			/*not find rigth block, start new*/
			pLogM->reckInfo.blockTh = 0;
			pLogM->reckInfo.buffUse = GPN_LOG_FILE_BLCK_MARK_SIZE;
			pLogM->reckInfo.buffWtStar = GPN_LOG_FILE_BLCK_MARK_SIZE;
			fseek(pLogM->fd, 0, SEEK_SET);
		}
		else
		{
			fseek(pLogM->fd, (GPN_LOG_FILE_BLOCK_SIZE*i), SEEK_SET);
		}
		return GPN_LOG_OK;
	}
	else if((pLogM != NULL) &&\
			(pLogM->type == GPN_LOGHANDLER_LFILE))
	{
		isBFind = GPN_LOG_NO;
		
		for(i=0;i<GPN_LOG_FILE_BLOCK_NUM;i++)
		{
			bkMarkOff = GPN_LOG_FILE_BLOCK_SIZE*i;
			fseek(pLogM->fd, bkMarkOff, SEEK_SET);
			if(fread(blckMark, GPN_LOG_FILE_BLCK_MARK_SIZE, 1, pLogM->fd) == 0)
			{
				continue;
			}
			if(strncmp(blckMark, GPN_LOG_FILE_BLCK_MARK, GPN_LOG_FILE_BLCK_MARK_SIZE-2))
			{
				isBFind = GPN_LOG_YES;
				
				break;
			}
		}
		if(isBFind == GPN_LOG_NO)
		{
			/*not find rigth block, start new*/
			pLogM->reckInfo.blockTh = 0;
			pLogM->reckInfo.buffUse = 0;
			pLogM->reckInfo.buffWtStar = 0;
			fseek(pLogM->fd, 0, SEEK_SET);
		}
		else
		{
			pLogM->reckInfo.blockTh = i;
			pLogM->reckInfo.buffUse = 0;
			pLogM->reckInfo.buffWtStar = 0;

			/*here find block, and this block is used block, then next block is new start*/
			i = (i+1)%GPN_LOG_FILE_BLOCK_NUM;
			bkMarkOff = GPN_LOG_FILE_BLOCK_SIZE*i;
			fseek(pLogM->fd, bkMarkOff, SEEK_SET);
		}
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
	
}
static UINT32 gpnBFileLogSetLineMark(gpnLogMethod *pLogM)
{
	UINT32 tmpOff;

	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_BFILE) )
	{
		tmpOff = pLogM->reckInfo.buffUse/GPN_LOG_FILE_VLINE_SIZE;
		if((pLogM->reckInfo.buffUse%GPN_LOG_FILE_VLINE_SIZE)==0)
		{
			tmpOff = tmpOff*GPN_LOG_FILE_VLINE_SIZE;
		}
		else
		{
			tmpOff = (tmpOff+1)*GPN_LOG_FILE_VLINE_SIZE;
		}
		if( (tmpOff+GPN_LOG_FILE_VLINE_MARK_SIZE) <= (GPN_LOG_FILE_BLOCK_SIZE) )
		{
			/*save line Mark in one block*/
			snprintf(&(pLogM->reckInfo.bkBuff[tmpOff]),\
				GPN_LOG_FILE_VLINE_MARK_SIZE, "%s", GPN_LOG_FILE_VLINE_MARK);
			pLogM->reckInfo.buffUse += GPN_LOG_FILE_VLINE_MARK_SIZE;

			memcpy(pLogM->reckInfo.bkBuff,\
				GPN_LOG_FILE_BLCK_FREE_MARK, GPN_LOG_FILE_BLCK_MARK_SIZE);
			fseek(pLogM->fd, (pLogM->reckInfo.blockTh*GPN_LOG_FILE_BLOCK_SIZE), SEEK_SET);
			fwrite(pLogM->reckInfo.bkBuff, GPN_LOG_FILE_BLOCK_SIZE, 1, pLogM->fd);
		}
		else
		{
			/*save now block, clear next block*/
			memcpy(pLogM->reckInfo.bkBuff,\
				GPN_LOG_FILE_BLCK_MARK, GPN_LOG_FILE_BLCK_MARK_SIZE);
			memcpy(&(pLogM->reckInfo.bkBuff[GPN_LOG_FILE_BLOCK_SIZE]),\
				GPN_LOG_FILE_BLCK_FREE_MARK, GPN_LOG_FILE_BLCK_MARK_SIZE);
				
			if((pLogM->reckInfo.blockTh+1) == GPN_LOG_FILE_BLOCK_NUM)
			{
				/*last block:write first block mark first*/
				fseek(pLogM->fd, 0, SEEK_SET);
				fwrite(GPN_LOG_FILE_BLCK_FREE_MARK,GPN_LOG_FILE_BLCK_MARK_SIZE, 1, pLogM->fd);
				fseek(pLogM->fd, (pLogM->reckInfo.blockTh*GPN_LOG_FILE_BLOCK_SIZE), SEEK_SET);
				fwrite(pLogM->reckInfo.bkBuff,GPN_LOG_FILE_BLOCK_SIZE, 1, pLogM->fd);
			}
			else
			{
				/*normal block*/
				fseek(pLogM->fd, (pLogM->reckInfo.blockTh*GPN_LOG_FILE_BLOCK_SIZE), SEEK_SET);
				fwrite(pLogM->reckInfo.bkBuff,GPN_LOG_FILE_BLOCK_BUFF_SIZE, 1, pLogM->fd);
			}
		}

		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnBFileLogRAMFlushInFile(gpnLogMethod *pLogM)
{
	if( (pLogM != NULL) &&\
		(pLogM->type == GPN_LOGHANDLER_BFILE) )
	{
		fseek(pLogM->fd, (pLogM->reckInfo.blockTh*GPN_LOG_FILE_BLOCK_SIZE), SEEK_SET);
		fwrite(pLogM->reckInfo.bkBuff, pLogM->reckInfo.buffUse, 1, pLogM->fd);
		pLogM->reckInfo.buffWtStar = pLogM->reckInfo.buffUse;
		
		return GPN_LOG_OK;
	}
	else
	{
		return GPN_LOG_ERR;
	}
}
static UINT32 gpnLogGetProcessName(char *pname, UINT32 length)
{
	INT32 i;
	INT8 path[256]={0};   
	INT8 filepath[256]={0};   
	INT8 cmd[256]={0};   
	FILE* fp=NULL;   

	/*assert */
	if(pname == NULL)
	{
		return GPN_LOG_ERR;
	}
	
	/*get process path*/
	sprintf(filepath, "/proc/%d", getpid());

	if( -1 != chdir(filepath) )   
	{   
		/*memset(filepath, 0, 256);*/ 

		/*snprintf(cmd,256, "ls -l | grep exe | awk '{print $10}'");*/
		snprintf(cmd,256, "ls -l | grep exe | awk '{print $11}'");
		
		if(NULL == (fp=popen(cmd,"r")))   
		{   
			perror("gpnGetProcessName popen failed...");   
			
			return GPN_LOG_ERR;   
		}   
		
		if( NULL == gpnFgets(path, sizeof(path)/sizeof(path[0]), fp) )   
		{   
	 		perror("gpnGetProcessName fgets error...");   
			pclose(fp);   
			return GPN_LOG_ERR;   
		}
		pclose(fp);

		/*fine last '/'*/
		for(i=strlen(path);i>=0;i--)
		{
			if(path[i] == '/')
			{
				snprintf(pname, length, "%s", &(path[i+1]));

				return GPN_LOG_OK;
			}
			if(path[i] == '\n')
			{
				path[i] = '\0';
			}
		}
	}
	
	snprintf(pname, length, "%d", getpid());
	/*LOG*/
	printf("gpnLog:could not get process(%d) name\n\r", getpid());
	
	return GPN_LOG_ERR;
}

static INT8* gpnSprintfSecStamp(time_t *now, INT8 *sbuf)
{
    time_t inNow;
    struct tm *tm;

    if(now == NULL)
	{
        now = &inNow;
        time(now);
    }
    tm = localtime(now);
    sprintf(sbuf, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d ",
    	tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
       	tm->tm_hour, tm->tm_min, tm->tm_sec);
	
    return sbuf;
}

static INT8* gpnSprintfUsecStamp(struct timeval *now, INT8 *sbuf)
{
    struct timeval inNow;
    struct tm *tm;

    if(now == NULL)
	{
        now = &inNow;
        gettimeofday(&inNow, NULL);
    }
    tm = localtime(&(now->tv_sec));
    sprintf(sbuf, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d:%.6d ",
    	tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
       	tm->tm_hour, tm->tm_min, tm->tm_sec, (UINT32)(now->tv_usec));
	
    return sbuf;
}

static UINT32 gpnLogString(UINT8 priority, const INT8 *str)
{
	UINT32 logMNum;
   	gpnLogMethod *pLogM;
	stGpnLogInfo *pGpnLogInfo;

	pGpnLogInfo = pgGpnLogInfo;
    /* We've got to be able to log messages *somewhere*!
       so before log something ,enbale some log method.*/

    /* Start from lowest priority */
    logMNum = 0;
	pLogM = (gpnLogMethod *)listFirst(&(pGpnLogInfo->gpnMUseList));
	while((pLogM != NULL)&&(logMNum<pGpnLogInfo->useNum))
	{
		if( (priority >= pLogM->min_pri) &&\
			(priority <= pLogM->max_pri) )
		{
			if(pLogM->enable == GPN_LOG_ENABLE)
			{
				pLogM->handler(pLogM, priority, str);
			}
		}
		
		logMNum++;
		pLogM = (gpnLogMethod *)listNext((NODE *)(pLogM));
    }

	return GPN_LOG_OK;
}
UINT32 gpnVLog(UINT8 priority, const INT8 *format, va_list ap)
{
    INT8 buffer[GPN_LOG_TEXT_MAX_LINE];
    INT32 length;

    length = vsnprintf(buffer, GPN_LOG_TEXT_MAX_LINE, format, ap);
    va_end(ap);

    if(length == 0)
	{
        return GPN_LOG_ERR;             /* Empty string */
    }

    if(length == -1)
	{
        gpnLogString(GPN_LOG_L_ERR, "Could not format log-string\n\r");
        return GPN_LOG_ERR;
    }

    if(length <= GPN_LOG_TEXT_MAX_LINE)
	{
        gpnLogString(priority, buffer);
        return GPN_LOG_OK;
    }
	else
	{
		gpnLogString(GPN_LOG_L_ERR, "log string too long to save\n\r");
        return GPN_LOG_ERR;
	}
}

UINT32 debugLogGlobalInfo(void)
{
	UINT32 logMNum;
	gpnLogMethod *pLogM;
	stGpnLogInfo *pGpnLogInfo;

	pGpnLogInfo = pgGpnLogInfo;

	printf("maxMethod idleMethod useMethod process\n\r");
	printf("%.9d %.10d %.9d %s\n\r",\
		GPN_LOG_METHOD_MAX_NODE,\
		pGpnLogInfo->idleNum,\
		pGpnLogInfo->useNum,\
		pGpnLogInfo->pcName);

	printf("type En hPriv lPriv magic imagic rcBTh useBuff bWtPosit token\n\r");
	logMNum = 0;
	pLogM = (gpnLogMethod *)listFirst(&(pGpnLogInfo->gpnMUseList));
	while((pLogM != NULL)&&(logMNum<pGpnLogInfo->useNum))
	{
		printf("%.4d %.2d %.5d %.5d %.5d %.6d %.5d %.7d %.8d %s\n\r",\
			pLogM->type,\
			pLogM->enable,\
			pLogM->max_pri,\
			pLogM->min_pri,\
			pLogM->magic,\
			pLogM->imagic,\
			pLogM->reckInfo.blockTh,\
			pLogM->reckInfo.buffUse,\
			pLogM->reckInfo.buffWtStar,\
			pLogM->token);
		logMNum++;
		pLogM = (gpnLogMethod *)listNext((NODE *)(pLogM));
	}

	return GPN_LOG_ERR;
}
#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_LOG_C_*/
