/**********************************************************
* file name: gpnStatProcMain.C
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-03-11
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_PROC_MAIN_C_
#define _GPN_STAT_PROC_MAIN_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <sys/types.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#include <lib/syslog.h>
#include <lib/log.h>

#include "socketComm/gpnTimerService.h"
#include "socketComm/gpnSockCommRoleMan.h"
#include "socketComm/gpnSockCommModuDef.h"
#include "socketComm/gpnSockAlmMsgDef.h"
#include "socketComm/gpnAlmTypeDef.h"

#include "gpnStatProcMain.h"
#include "gpnStatTypeStruct.h"
#include "gpnStatDataStruct.h"
#include "gpnStatTimeProc.h"
#include "gpnStatDebug.h"


extern sockCommData gSockCommData;
extern stEQUStatProcSpace *pgstEQUStatProcSpace;
extern gstStatTypeWholwInfo *pgstStatTypeWholwInfo;


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

UINT32 gpnStatProcInit(void)
{
	/*init stat type base template*/
	//gpnStatTypeInit();
	gpnStatTypeStInit();
	//debugStatTypePrint();

	gpnStatDataBaseStructInit();
	//debugGpnStatDataBaseStruct();

	/*delay, then start statMon*/
	//gpnStatStartStatFunc();
	
	return GPN_STAT_PROC_GEN_OK;
}

UINT32 gpnStatStartStatFunc(void)
{
	/*reload config stat para right now */
	/*???*/
	
	/*delay start sata collec and hist data push stack */
	gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(GPN_STAT_FUNC_START_DELAY, second),
		0, (timerOutFuncPtr)gpnStatCoreProcInit, 0, 0);
	

	return GPN_STAT_PROC_GEN_OK;
}

/*
	stat data scan proc :
	1 port base stat data collect; 
*/
void gpnStatCoreProcInit(void)
{
	GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CUP, "%s : start \"gpnStatCoreProc\"!\n\r",\
		__FUNCTION__);
	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_CYC, GPN_TIME_TO_TICKS(GPN_STAT_CORE_PROC_CYC, second),
		0, (timerOutFuncPtr)gpnStatCoreProc, GPN_STAT_CORE_PROC_CYC, 0);*/

	return;
}

int gpnStatCoreProc(void *para)
{
	UINT32 reVal;
	
	/*core proc : renew stat time about */
	reVal = gpnStatTimeAdjust(1);
	if(reVal == GPN_STAT_PROC_GEN_ERR)
	{
		/*time chang for some reason, reset hist data collection */
		/*1 : reset per-port hist data buff; 				*/
		/*2 : clear per-port base hist mark;				*/
		/*3 : re-calculat hist bingo and hist push time;		*/
		gpnStatResetValidPortHistRecordAbout();

		/*log hist data reset */
		GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CMP, "%s : reset hist data collection!\n\r",\
			__FUNCTION__);
		/*???*/
	}

	/*core proc : push hist data  and creat trap mark and random time */
	gpnStatPerTaskPushStackProc();

	/*core proc : trap hist info */
	gpnStatHistDataTrapProc();
	
	/*core proc : port (in valid list base scan type)  base proc */
	gpnStatValidPortProc();

	return 1;
}
UINT32 gpnStatResetValidPortHistRecordAbout(void)
{
	UINT32 hash;
	stStatTaskInfo *ptaskInfo;
	UINT32 taskNum;
	stStatTaskNode *ptaskNode;
	UINT32 objNum;
	stStatMonObjNode *pstatMonObj;
	stStatLocalPortNode *pstatLocalPort;
	stStatScanPortInfo *pstatScanPort;
	
	ptaskInfo = &(pgstEQUStatProcSpace->statTaskInfo);

	for(hash=0;hash<ptaskInfo->hash;hash++)
	{
		/*first find task node */
		taskNum = 0;
		ptaskNode = (stStatTaskNode *)listFirst(&(ptaskInfo->actTaskQuen[hash]));
		while((ptaskNode != NULL)&&(taskNum < ptaskInfo->actTaskNum[hash]))
		{
			/*second find obj node */
			objNum = 0;
			pstatMonObj = (stStatMonObjNode *)listFirst(&(ptaskNode->actMOnOBjQuen));
			while((pstatMonObj != NULL)&&(objNum < ptaskNode->actMOnOBjNum))
			{
				pstatLocalPort = pstatMonObj->pStatLocalPort;
				if(pstatLocalPort == NULL)
				{
					GPN_STAT_PROC_PRINT(GPN_STAT_PROC_EGP, "%s : what a hell!\n\r",\
						__FUNCTION__);
					continue;
				}
				pstatScanPort = pstatLocalPort->pStatScanPort;
				if(pstatScanPort == NULL)
				{
					GPN_STAT_PROC_PRINT(GPN_STAT_PROC_EGP, "%s : what a hell!!\n\r",\
						__FUNCTION__);
					continue;
				}

				/*1 : reset per-port hist data buff; 				*/
				GPN_STAT_PROC_PRINT(GPN_STAT_PROC_EGP, "%s : dev(%08x) local port(%08x|%08x|%08x|%08x|%08x) hist rec reset!\n\r",\
					__FUNCTION__, pstatLocalPort->localPort.devIndex, pstatLocalPort->localPort.portIndex,\
					pstatLocalPort->localPort.portIndex3, pstatLocalPort->localPort.portIndex4,\
					pstatLocalPort->localPort.portIndex5, pstatLocalPort->localPort.portIndex6);
				
				if(pstatScanPort->statLCycRec != NULL)
				{
					gpnStatXCycRecReSet(pstatScanPort->statLCycRec);
				}
				if(pstatScanPort->statSCycRec != NULL)
				{
					gpnStatXCycRecReSet(pstatScanPort->statSCycRec);
				}
				if(pstatScanPort->statUCycRec != NULL)
				{
					gpnStatXCycRecReSet(pstatScanPort->statUCycRec);
				}
				
				objNum = 0;
				pstatMonObj = (stStatMonObjNode *)listNext((NODE *)pstatMonObj);
			}

			/*3 : re-calculat hist bingo and hist push time;		*/
			/*calculat stat sysTime's nearest next bingo			*/
			
			
			taskNum = 0;
			ptaskNode = (stStatTaskNode *)listNext((NODE *)ptaskNode);
		}
	}
	return GPN_STAT_PROC_GEN_OK;
}

UINT32 gpnStatXCycRecReSet(stStatXCycDataRecd *pxCycDataRecd)
{
	UINT32 i;
	
	/*assert */
	if(pxCycDataRecd == NULL)
	{
		return GPN_STAT_PROC_GEN_ERR;
	}

	if(pxCycDataRecd->en != GPN_STAT_PROC_GEN_ENABLE)
	{
		return GPN_STAT_PROC_GEN_ERR;
	}

	/*1 : clear hist data, histRecNum max is histRecBufoNum */
	for(i=0;i<pxCycDataRecd->histRecNum;i++)
	{
		/*so can not search hist data */
		pxCycDataRecd->pstatXCycHistRecd[i]->stopTime = 0;
	}
	pxCycDataRecd->recStart = 0;
	pxCycDataRecd->recEnd = GPN_STAT_PROC_GEN_08_FFFF;
	pxCycDataRecd->histRecNum = 0;

	/*2 : clear hist alarm and event
	    no, should cleared base next hist data pfm */

	/*3 : clear xCyc curr data */

	
	return GPN_STAT_PROC_GEN_OK;
}

UINT32 gpnStatValidPortProc(void)
{
	UINT32 scanTpNum;
	stStatScanTypeDef *pstatScanType;
	UINT32 portNum;
	stStatLocalPortNode *pStatLocalNode;
	stStatTaskInfo *ptaskInfo;

	ptaskInfo = &(pgstEQUStatProcSpace->statTaskInfo);
	
	/*port add in valid list base STAT_SCAN_TYPE */
	scanTpNum = 0;
	pstatScanType = (stStatScanTypeDef *)listFirst(&(pgstStatTypeWholwInfo->statScanTypeQuen));
	while((pstatScanType != NULL)&&(scanTpNum < pgstStatTypeWholwInfo->statScanTypeNum))
	{
		/*find port in valid scan quen */
		portNum = 0;
		pStatLocalNode = (stStatLocalPortNode *)listFirst(&(pstatScanType->statScanPortObjQuen));
		while((pStatLocalNode != NULL)&&(portNum < pstatScanType->statScanQuenPortNum))
		{
			/*	use wtich cycSec to collect stat data : 
				chosse mini cycSec from four cycSec :
				1 scan type's scan cycSec ;
				2 long cycSec if long cyc mon enable ;
				3 short cycSec if short cyc mon enable ;
				4 usr cycSec if usr cyc mon enable   	*/
			if((ptaskInfo->statSysTime % pStatLocalNode->pStatScanPort->miniScanCyc) == 0)
			{
				/*core proc 2 : send collect msg to right modu */
				gpnStatCollectBaseStatData(pstatScanType,pStatLocalNode);
				
				/*core proc 3 : proc stat threshold compare and rise alarm and event (do when rev stat data notify)*/
				gpnStatThresholdProc(pstatScanType,pStatLocalNode);
			}
			
			portNum++;
			pStatLocalNode = (stStatLocalPortNode *)listNext((NODE *)(pStatLocalNode));
		}
		
		scanTpNum++;
		pstatScanType = (stStatScanTypeDef *)listNext((NODE *)(pstatScanType));
	}
	return GPN_STAT_PROC_GEN_OK;
}

UINT32 gpnStatCollectBaseStatData(stStatScanTypeDef *pstatScanType, stStatLocalPortNode *pStatLocalNode)
{
	UINT32 statTypeNum;
	stStatTpNode *pstatTpNode;
	stStatTypeDef *pstatType;

	/*assert */
	if( (pstatScanType == NULL) ||\
		(pStatLocalNode == NULL) )
	{
		return GPN_STAT_PROC_GEN_ERR;
	}

	/*stat scan type include stat type, so use each stat type's callback to get stat data */
	statTypeNum = 0;
	pstatTpNode = (stStatTpNode *)listFirst(&(pstatScanType->statTpsOfScanTpQuen));
	while((pstatTpNode != NULL)&&(statTypeNum < pstatScanType->statTpNumInScanType))
	{
		GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CUP, "%s : statType(%08x) pollCyc(%d) dev(%08x) port(%08x|%08x|%08x|%08x|%08x)!\n\r",\
			__FUNCTION__, pstatTpNode->pStatTpStr->statType, pStatLocalNode->pStatScanPort->miniScanCyc,\
			pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
			pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
			pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6);

		pstatType = pstatTpNode->pStatTpStr;

		if(pstatType->pStatTpBPFunc != NULL)
		{
			pstatType->pStatTpBPFunc(&(pStatLocalNode->pPeerNode->peerPort), pstatType->statType);
		}
		
		statTypeNum++;
		pstatTpNode = (stStatTpNode *)listNext((NODE *)(pstatTpNode));
	}

	return GPN_STAT_PROC_GEN_OK;
}
UINT32 gpnStatThresholdProc(stStatScanTypeDef *pstatScanType, stStatLocalPortNode *pStatLocalNode)
{
	stStatScanPortInfo *pStatScanPort;
	
	/*assert */
	if( (pstatScanType == NULL) ||\
		(pStatLocalNode == NULL) )
	{
		return GPN_STAT_PROC_GEN_ERR;
	}
	
	pStatScanPort = pStatLocalNode->pStatScanPort;
	if( (pStatScanPort->statLCycRec != NULL) &&\
		(pStatScanPort->statLCycRec->en == GPN_STAT_PROC_GEN_ENABLE) &&\
		(pStatScanPort->statLCycRec->dataValid == GPN_STAT_PROC_GEN_ENABLE) )
	{
		gpnStatRiseStatAlarm(pstatScanType, pStatScanPort->statLCycRec,\
			&(pStatLocalNode->pPeerNode->peerPort), GPN_STAT_CYC_CLASS_LONG);
		gpnStatRiseStatEvent(pstatScanType, pStatScanPort->statLCycRec,\
			&(pStatLocalNode->pPeerNode->peerPort), GPN_STAT_CYC_CLASS_LONG);
	}
	
	if( (pStatScanPort->statSCycRec != NULL) &&\
		(pStatScanPort->statSCycRec->en == GPN_STAT_PROC_GEN_ENABLE) &&\
		(pStatScanPort->statSCycRec->dataValid == GPN_STAT_PROC_GEN_ENABLE) )
	{
		gpnStatRiseStatAlarm(pstatScanType, pStatScanPort->statSCycRec,\
			&(pStatLocalNode->pPeerNode->peerPort), GPN_STAT_CYC_CLASS_SHORT);
		gpnStatRiseStatEvent(pstatScanType, pStatScanPort->statSCycRec,\
			&(pStatLocalNode->pPeerNode->peerPort), GPN_STAT_CYC_CLASS_SHORT);
	}
	
	if( (pStatScanPort->statUCycRec != NULL) &&\
		(pStatScanPort->statUCycRec->en == GPN_STAT_PROC_GEN_ENABLE) &&\
		(pStatScanPort->statUCycRec->dataValid == GPN_STAT_PROC_GEN_ENABLE) )
	{
		gpnStatRiseStatAlarm(pstatScanType, pStatScanPort->statUCycRec,\
			&(pStatLocalNode->pPeerNode->peerPort), GPN_STAT_CYC_CLASS_USR);
		gpnStatRiseStatEvent(pstatScanType, pStatScanPort->statUCycRec,\
			&(pStatLocalNode->pPeerNode->peerPort), GPN_STAT_CYC_CLASS_USR);
	}

	return GPN_STAT_PROC_GEN_OK;
}
UINT32 gpnStatRiseStatAlarm(stStatScanTypeDef *pstatScanType,
	stStatXCycDataRecd *statXCycRec, optObjOrient *pPortIndex, UINT32 cycClass)
{
	stStatSTCharacterDef *pStatSubType;
	almThredTpUnit *palmThredInfo;
	subFiltTpUnit *psubFileInfo;
	UINT32 *pxCycCurrData;
	UINT32 currDWStep;
	UINT32 valueType;
	UINT32 numType;
	UINT32 bitDeep;
	UINT32 bitMask;
	UINT32 upChg;
	UINT32 upOpt;
	UINT32 dnChg;
	UINT32 dnOpt;
	UINT32 i;

	/*assert */
	if( (pstatScanType == NULL) ||\
		(statXCycRec == NULL) ||\
		(pPortIndex == NULL) )
	{
		return GPN_STAT_PROC_GEN_ERR;
	}
	if(statXCycRec->en != GPN_STAT_PROC_GEN_ENABLE)
	{
		return GPN_STAT_PROC_GEN_ERR;
	}

	palmThredInfo = (almThredTpUnit *)statXCycRec->xCycAlmThred;
	psubFileInfo = (subFiltTpUnit *)statXCycRec->subFiltInfo;
	pxCycCurrData = (UINT32 *)statXCycRec->xCycCurrDate;

	if (palmThredInfo == NULL || psubFileInfo == NULL)
	{
		zlog_debug(STATIS_DBG_DATA_COLLECT, "%s:%d make trouble for delete almThred or subFile Template\n\r",\
			__FUNCTION__, __LINE__);
		return GPN_STAT_PROC_GEN_ERR;
	}
	
	/*just for left-value init */
	upOpt = GPN_STAT_PROC_GEN_NO;
	dnOpt = GPN_STAT_PROC_GEN_NO;
	for(i=0;i<statXCycRec->subStatNum;i++)
	{
		bitMask = 1U << i;
		upChg = GPN_STAT_PROC_GEN_NO;
		dnChg = GPN_STAT_PROC_GEN_NO;
		
		/*copy data step : default 64bit */
		if(statXCycRec->dataBitDeep & bitMask)
		{
			currDWStep = GPN_STAT_DATA_64BIT_DWLEN;
			bitDeep = DATA_BIT_DEEP_64;
		}
		else /*if(pxCycDataRecd->dataBitDeep == GPN_STAT_DATA_BIT_DEEP_32)*/
		{
			currDWStep = GPN_STAT_DATA_32BIT_DWLEN;
			bitDeep = DATA_BIT_DEEP_32;
		}

		if(statXCycRec->valueType & bitMask)
		{
			valueType = GPN_COUNTER_CLASS;
		}
		else
		{
			valueType = GPN_MEASURE_CLASS;
		}

		if(statXCycRec->numType & bitMask)
		{
			numType = GPN_INTEGER_NUM_CLASS;
		}
		else
		{
			numType = GPN_NATURAL_NUM_CLASS;
		}

//		printf("currDWStep:%d subNum:%d subFileInfo:%d almThredInfo:%d UpAlam:%d CurrData:%d-%d upDispThredL32:%d upRiseThredL32:%d\n\r", 
//			currDWStep, statXCycRec->subStatNum, psubFileInfo->status, palmThredInfo->subType, statXCycRec->statXCycUpAlm, 
//			*pxCycCurrData, *(pxCycCurrData+1), palmThredInfo->upDispThredL32, palmThredInfo->upRiseThredL32);

		/*is subType filt ? */
		if( (psubFileInfo->status == GPN_STAT_PROC_GEN_DISABLE) &&\
			(palmThredInfo->subType != GPN_STAT_SUB_TYPE_INVALID) )
		{
			/*cmp curr data with threshold data */
			if(bitDeep == DATA_BIT_DEEP_64)
			{
				if(statXCycRec->statXCycUpAlm & bitMask)
				{
					if (valueType == GPN_MEASURE_CLASS)
					{
						/*already step out up-rise threshold, so cmp up-clear threshold */
						if( (*pxCycCurrData < palmThredInfo->upDispThredH32) ||\
							((*pxCycCurrData == palmThredInfo->upDispThredH32) &&\
							(*(pxCycCurrData+1) < palmThredInfo->upDispThredL32)) ) 
						{
							/*clear step out up-rise alarm*/
							statXCycRec->statXCycUpAlm &= (~bitMask);
							upChg = GPN_STAT_PROC_GEN_YES;
							upOpt = GPN_STAT_PROC_GEN_NO;
						}
					}
				}
				else
				{
					if( (*pxCycCurrData > palmThredInfo->upRiseThredH32) ||\
						((*pxCycCurrData == palmThredInfo->upRiseThredH32) &&\
						(*(pxCycCurrData+1) > palmThredInfo->upRiseThredL32)) ) 
					{
						/*rise step out up-rise alarm*/
						statXCycRec->statXCycUpAlm |= bitMask;
						upChg = GPN_STAT_PROC_GEN_YES;
						upOpt = GPN_STAT_PROC_GEN_YES;
					}
				}
				
				if(statXCycRec->statXCycDownAlm & bitMask)
				{
					if (valueType == GPN_MEASURE_CLASS)
					{
						/*already step out down-rise threshold, so cmp down-clear threshold */
						if( (*pxCycCurrData > palmThredInfo->dnDispThredH32) ||\
							((*pxCycCurrData == palmThredInfo->dnDispThredH32) &&\
							(*(pxCycCurrData+1) > palmThredInfo->dnDispThredL32)) ) 
						{
							/*clear step out down-rise alarm*/
							statXCycRec->statXCycDownAlm &= (~bitMask);
							dnChg = GPN_STAT_PROC_GEN_YES;
							dnOpt = GPN_STAT_PROC_GEN_NO;
						}
					}
				}
				else
				{
					if( (*pxCycCurrData < palmThredInfo->dnRiseThredH32) ||\
						((*pxCycCurrData == palmThredInfo->dnRiseThredH32) &&\
						(*(pxCycCurrData+1) < palmThredInfo->dnRiseThredL32)) ) 
					{
						/*rise step out down-rise alarm*/
						statXCycRec->statXCycDownAlm |= bitMask;
						dnChg = GPN_STAT_PROC_GEN_YES;
						dnOpt = GPN_STAT_PROC_GEN_YES;
					}
				}
			}
			else /*if(bitDeep == DATA_BIT_DEEP_32)*/
			{
				if(statXCycRec->statXCycUpAlm & bitMask)
				{
					if (valueType == GPN_MEASURE_CLASS)
					{
						/*already step out up-rise threshold, so cmp up-clear threshold */
						if(numType == GPN_NATURAL_NUM_CLASS)
						{
							if(*pxCycCurrData < palmThredInfo->upDispThredL32)
							{
								gpnLog(GPN_LOG_L_INFO, "%s : clear up alarm subType:%08x data:upDisp(%d:%d)\n\r", \
									__FUNCTION__, palmThredInfo->subType, *pxCycCurrData, palmThredInfo->upDispThredL32);
								/*clear step out up-rise alarm*/
								statXCycRec->statXCycUpAlm &= (~bitMask);
								upChg = GPN_STAT_PROC_GEN_YES;
								upOpt = GPN_STAT_PROC_GEN_NO;
							}
						}
						else
						{
							if( ((INT32)(*pxCycCurrData)) < ((INT32)(palmThredInfo->upDispThredL32)) )
							{
								gpnLog(GPN_LOG_L_INFO, "%s : clear up alarm subType:%08x data:upDisp(%d:%d)\n\r", \
									__FUNCTION__, palmThredInfo->subType, *pxCycCurrData, palmThredInfo->upDispThredL32);
								/*clear step out up-rise alarm*/
								statXCycRec->statXCycUpAlm &= (~bitMask);
								upChg = GPN_STAT_PROC_GEN_YES;
								upOpt = GPN_STAT_PROC_GEN_NO;
							}
						}
					}
				}
				else
				{
					if(numType == GPN_NATURAL_NUM_CLASS)
					{
						if(*pxCycCurrData > palmThredInfo->upRiseThredL32)
						{
							gpnLog(GPN_LOG_L_INFO, "%s : rise up alarm subType:%08x data:upRise(%d:%d)\n\r", \
									__FUNCTION__, palmThredInfo->subType, *pxCycCurrData, palmThredInfo->upRiseThredL32);
							/*rise step out up-rise alarm*/
							statXCycRec->statXCycUpAlm |= bitMask;
							upChg = GPN_STAT_PROC_GEN_YES;
							upOpt = GPN_STAT_PROC_GEN_YES;
						}
					}
					else
					{
						if( ((INT32)(*pxCycCurrData)) > ((INT32)(palmThredInfo->upRiseThredL32)) )
						{
							gpnLog(GPN_LOG_L_INFO, "%s : rise up alarm subType:%08x data:upRise(%d:%d)\n\r", \
									__FUNCTION__, palmThredInfo->subType, *pxCycCurrData, palmThredInfo->upRiseThredL32);
							/*rise step out up-rise alarm*/
							statXCycRec->statXCycUpAlm |= bitMask;
							upChg = GPN_STAT_PROC_GEN_YES;
							upOpt = GPN_STAT_PROC_GEN_YES;
						}
					}
				}
				
				if(statXCycRec->statXCycDownAlm & bitMask)
				{
					if (valueType == GPN_MEASURE_CLASS)
					{
						/*already step out down-rise threshold, so cmp down-clear threshold */
						if(numType == GPN_NATURAL_NUM_CLASS)
						{
							if(*pxCycCurrData > palmThredInfo->dnDispThredL32)
							{
								/*clear step out down-rise alarm*/
								statXCycRec->statXCycDownAlm &= (~bitMask);
								dnChg = GPN_STAT_PROC_GEN_YES;
								dnOpt = GPN_STAT_PROC_GEN_NO;
							}
						}
						else
						{
							if( ((INT32)(*pxCycCurrData)) > ((INT32)(palmThredInfo->dnDispThredL32)) )
							{
								/*clear step out down-rise alarm*/
								statXCycRec->statXCycDownAlm &= (~bitMask);
								dnChg = GPN_STAT_PROC_GEN_YES;
								dnOpt = GPN_STAT_PROC_GEN_NO;
							}
						}
					}
				}
				else
				{
					if(numType == GPN_NATURAL_NUM_CLASS)
					{
						if(*pxCycCurrData < palmThredInfo->dnRiseThredL32)
						{
							/*rise step out down-rise alarm*/
							statXCycRec->statXCycDownAlm |= bitMask;
							dnChg = GPN_STAT_PROC_GEN_YES;
							dnOpt = GPN_STAT_PROC_GEN_YES;
						}
					}
					else
					{
						if( ((INT32)(*pxCycCurrData)) < ((INT32)(palmThredInfo->dnRiseThredL32)) )
						{
							/*rise step out down-rise alarm*/
							statXCycRec->statXCycDownAlm |= bitMask;
							dnChg = GPN_STAT_PROC_GEN_YES;
							dnOpt = GPN_STAT_PROC_GEN_YES;
						}
					}
				}
			}
		}

		/*printf("upChg:dnChg(%d:%d) upOpt:dnOpt(%d:%d) upDir:dnDir(%d:%d)\n\r", \
			upChg,dnChg,upOpt,dnOpt,upDir,dnDir);*/

		if(upChg == GPN_STAT_PROC_GEN_YES)
		{
			pStatSubType = NULL;
			gpnStatTypeGetSubStatTypeNodeBaseScanType(palmThredInfo->subType,
				pstatScanType, &pStatSubType);
			if(pStatSubType != NULL)
			{
				if( (cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
					(pStatSubType->longCycUpThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->longCycUpThAlm, upOpt);
					
					gpnLog(GPN_LOG_L_INFO, "%s : statSubType(%08x), long cyc up-thred alarm(%08x) opt(%d)\n\r",\
						__FUNCTION__, palmThredInfo->subType, pStatSubType->longCycUpThAlm, upOpt);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
					(pStatSubType->shortCycUpThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->shortCycUpThAlm, upOpt);
					
					gpnLog(GPN_LOG_L_INFO, "%s : statSubType(%08x), short cyc up-thred alarm(%08x) opt(%d)\n\r",\
						__FUNCTION__, palmThredInfo->subType, pStatSubType->shortCycUpThAlm, upOpt);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_USR) &&\
					(pStatSubType->userDefCycUpThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->userDefCycUpThAlm, upOpt);
					
					gpnLog(GPN_LOG_L_INFO, "%s : statSubType(%08x), user cyc up-thred alarm(%08x) opt(%d)\n\r",\
						__FUNCTION__, palmThredInfo->subType, pStatSubType->userDefCycUpThAlm, upOpt);
				}
			}
		}
		
		if(dnChg == GPN_STAT_PROC_GEN_YES)
		{	
			pStatSubType = NULL;
			gpnStatTypeGetSubStatTypeNodeBaseScanType(palmThredInfo->subType,
				pstatScanType, &pStatSubType);
			if(pStatSubType != NULL)
			{
				if( (cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
					(pStatSubType->longCycDnThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->longCycDnThAlm, dnOpt);
					
					gpnLog(GPN_LOG_L_INFO, "%s : statSubType(%08x), long cyc down-thred alarm(%08x) opt(%d)\n\r",\
						__FUNCTION__, palmThredInfo->subType, pStatSubType->longCycDnThAlm, dnOpt);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
					(pStatSubType->shortCycDnThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->shortCycDnThAlm, dnOpt);
					
					gpnLog(GPN_LOG_L_INFO, "%s : statSubType(%08x), short cyc down-thred alarm(%08x) opt(%d)\n\r",\
						__FUNCTION__, palmThredInfo->subType, pStatSubType->shortCycDnThAlm, dnOpt);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_USR) &&\
					(pStatSubType->userDefCycDnThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->userDefCycDnThAlm, dnOpt);
					
					gpnLog(GPN_LOG_L_INFO, "%s : statSubType(%08x), user cyc down-thred alarm(%08x) opt(%d)\n\r",\
						__FUNCTION__, palmThredInfo->subType, pStatSubType->userDefCycDnThAlm, upOpt);
				}
			}
		}
		
		/*move to next subStat alarm info unit */
		palmThredInfo++;
		/*move to next subStat filt info unit */
		psubFileInfo++;
		/*move to next subStat curr unit */
		pxCycCurrData += currDWStep;
	}

	return GPN_STAT_PROC_GEN_OK;
}
UINT32 gpnStatClearStatAlarm(stStatScanTypeDef *pstatScanType,
	stStatXCycDataRecd *statXCycRec, optObjOrient *pPortIndex, UINT32 cycClass)
{
	stStatSTCharacterDef *pStatSubType;
	almThredTpUnit *palmThredInfo;
	subFiltTpUnit *psubFileInfo;
	UINT32 *pxCycCurrData;
	UINT32 currDWStep;
	UINT32 bitDeep;
	UINT32 numType;
	UINT32 bitMask;
	UINT32 upChg;
	UINT32 dnChg;
	UINT32 i;

	/*assert */
	if( (pstatScanType == NULL) ||\
		(statXCycRec == NULL) ||\
		(pPortIndex == NULL) )
	{
		return GPN_STAT_PROC_GEN_ERR;
	}
	
	if(statXCycRec->en != GPN_STAT_PROC_GEN_ENABLE)
	{
		return GPN_STAT_PROC_GEN_ERR;
	}

	palmThredInfo = (almThredTpUnit *)statXCycRec->xCycAlmThred;
	psubFileInfo = (subFiltTpUnit *)statXCycRec->subFiltInfo;
	pxCycCurrData = (UINT32 *)statXCycRec->xCycCurrDate;

	if (palmThredInfo == NULL || psubFileInfo == NULL)
	{
		zlog_debug(STATIS_DBG_DATA_COLLECT, "%s:%d make trouble for delete almThred or subFile Template\n\r",\
			__FUNCTION__, __LINE__);
		return GPN_STAT_PROC_GEN_ERR;
	}

	for(i=0;i<statXCycRec->subStatNum;i++)
	{
		bitMask = 1U << i;
		upChg = GPN_STAT_PROC_GEN_NO;
		dnChg = GPN_STAT_PROC_GEN_NO;
		
		/*copy data step : default 64bit */
		if(statXCycRec->dataBitDeep & bitMask)
		{
			currDWStep = GPN_STAT_DATA_64BIT_DWLEN;
			bitDeep = DATA_BIT_DEEP_64;
		}
		else /*if(pxCycDataRecd->dataBitDeep == GPN_STAT_DATA_BIT_DEEP_32)*/
		{
			currDWStep = GPN_STAT_DATA_32BIT_DWLEN;
			bitDeep = DATA_BIT_DEEP_32;
		}

		if(statXCycRec->numType & bitMask)
		{
			numType = GPN_INTEGER_NUM_CLASS;
		}
		else
		{
			numType = GPN_NATURAL_NUM_CLASS;
		}

		/*is subType filt ? */
		if( (psubFileInfo->status == GPN_STAT_PROC_GEN_DISABLE) &&\
			(palmThredInfo->subType != GPN_STAT_SUB_TYPE_INVALID) )
		{
			/*cmp curr data with threshold data */
			if(bitDeep == DATA_BIT_DEEP_64)
			{
				if(statXCycRec->statXCycUpAlm & bitMask)
				{
					/*already step out up-rise threshold, so cmp up-clear threshold */
					if( (*pxCycCurrData < palmThredInfo->upDispThredH32) ||\
						((*pxCycCurrData == palmThredInfo->upDispThredH32) &&\
						(*(pxCycCurrData+1) < palmThredInfo->upDispThredL32)) ) 
					{
						/*clear step out up-rise alarm*/
						statXCycRec->statXCycUpAlm &= (~bitMask);
						upChg = GPN_STAT_PROC_GEN_YES;
					}
				}
				
				if(statXCycRec->statXCycDownAlm & bitMask)
				{
					/*already step out down-rise threshold, so cmp down-clear threshold */
					if( (*pxCycCurrData > palmThredInfo->dnDispThredH32) ||\
						((*pxCycCurrData == palmThredInfo->dnDispThredH32) &&\
						(*(pxCycCurrData+1) > palmThredInfo->dnDispThredL32)) ) 
					{
						/*clear step out down-rise alarm*/
						statXCycRec->statXCycDownAlm &= (~bitMask);
						dnChg = GPN_STAT_PROC_GEN_YES;
					}
				}
			}
			else /*if(bitDeep == DATA_BIT_DEEP_32)*/
			{
				if(statXCycRec->statXCycUpAlm & bitMask)
				{
					/*already step out up-rise threshold, so cmp up-clear threshold */
					if(numType == GPN_NATURAL_NUM_CLASS)
					{
						if(*pxCycCurrData < palmThredInfo->upDispThredL32)
						{
							/*clear step out up-rise alarm*/
							statXCycRec->statXCycUpAlm &= (~bitMask);
							upChg = GPN_STAT_PROC_GEN_YES;
						}
					}
					else
					{
						if( ((INT32)(*pxCycCurrData)) < ((INT32)(palmThredInfo->upDispThredL32)) )
						{
							/*clear step out up-rise alarm*/
							statXCycRec->statXCycUpAlm &= (~bitMask);
							upChg = GPN_STAT_PROC_GEN_YES;
						}
					}
				}
				
				if(statXCycRec->statXCycDownAlm & bitMask)
				{
					/*already step out down-rise threshold, so cmp down-clear threshold */
					if(numType == GPN_NATURAL_NUM_CLASS)
					{
						if(*pxCycCurrData > palmThredInfo->dnDispThredL32)
						{
							/*clear step out down-rise alarm*/
							statXCycRec->statXCycDownAlm &= (~bitMask);
							dnChg = GPN_STAT_PROC_GEN_YES;
						}
					}
					else
					{
						if( ((INT32)(*pxCycCurrData)) > ((INT32)(palmThredInfo->dnDispThredL32)) )
						{
							/*clear step out down-rise alarm*/
							statXCycRec->statXCycDownAlm &= (~bitMask);
							dnChg = GPN_STAT_PROC_GEN_YES;
						}
					}
				}
			}
		}

		if(upChg == GPN_STAT_PROC_GEN_YES)
		{
			pStatSubType = NULL;
			gpnStatTypeGetSubStatTypeNodeBaseScanType(palmThredInfo->subType,
				pstatScanType, &pStatSubType);
			if(pStatSubType != NULL)
			{
				if( (cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
					(pStatSubType->longCycUpThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->longCycUpThAlm, GPN_SOCK_MSG_OPT_CLEAN);
					
					gpnLog(GPN_LOG_L_INFO, "%s : statSubType(%08x), clean long cyc up-thred alarm(%08x)\n\r",\
						__FUNCTION__, palmThredInfo->subType, pStatSubType->longCycDnThAlm);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
					(pStatSubType->shortCycUpThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->shortCycUpThAlm, GPN_SOCK_MSG_OPT_CLEAN);
					
					gpnLog(GPN_LOG_L_INFO, "%s : statSubType(%08x), clean short cyc up-thred alarm(%08x)\n\r",\
						__FUNCTION__, palmThredInfo->subType, pStatSubType->shortCycUpThAlm);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_USR) &&\
					(pStatSubType->userDefCycUpThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->userDefCycUpThAlm, GPN_SOCK_MSG_OPT_CLEAN);

					gpnLog(GPN_LOG_L_INFO, "%s : statSubType(%08x), clean user cyc up-thred alarm(%08x)\n\r",\
						__FUNCTION__, palmThredInfo->subType, pStatSubType->userDefCycUpThAlm);
				}
			}
		}
		
		if(dnChg == GPN_STAT_PROC_GEN_YES)
		{
			pStatSubType = NULL;
			gpnStatTypeGetSubStatTypeNodeBaseScanType(palmThredInfo->subType,
				pstatScanType, &pStatSubType);
			if(pStatSubType != NULL)
			{
				if( (cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
					(pStatSubType->longCycDnThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->longCycDnThAlm, GPN_SOCK_MSG_OPT_CLEAN);

					gpnLog(GPN_LOG_L_INFO, "%s : statSubType(%08x), clean long cyc down-thred alarm(%08x)\n\r",\
						__FUNCTION__, palmThredInfo->subType, pStatSubType->longCycDnThAlm);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
					(pStatSubType->shortCycDnThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->shortCycDnThAlm, GPN_SOCK_MSG_OPT_CLEAN);

					gpnLog(GPN_LOG_L_INFO, "%s : statSubType(%08x), clean short cyc down-thred alarm(%08x)\n\r",\
						__FUNCTION__, palmThredInfo->subType, pStatSubType->shortCycDnThAlm);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_USR) &&\
					(pStatSubType->userDefCycDnThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->userDefCycDnThAlm, GPN_SOCK_MSG_OPT_CLEAN);

					gpnLog(GPN_LOG_L_INFO, "%s : statSubType(%08x), clean user cyc down-thred alarm(%08x)\n\r",\
						__FUNCTION__, palmThredInfo->subType, pStatSubType->userDefCycDnThAlm);
				}
			}
		}
		
		/*move to next subStat alarm info unit */
		palmThredInfo++;
		/*move to next subStat filt info unit */
		psubFileInfo++;
		/*move to next subStat curr unit */
		pxCycCurrData += currDWStep;
	}

	return GPN_STAT_PROC_GEN_OK;
}
UINT32 gpnStatDeleteStatAlarm(stStatScanTypeDef *pstatScanType,
	stStatXCycDataRecd *statXCycRec, optObjOrient *pPortIndex, UINT32 cycClass)
{
	stStatSTCharacterDef *pStatSubType;
	almThredTpUnit *palmThredInfo;
	subFiltTpUnit *psubFileInfo;
	UINT32 *pxCycCurrData;
	UINT32 currDWStep;
	UINT32 bitDeep;
	UINT32 bitMask;
	UINT32 upChg;
	UINT32 dnChg;
	UINT32 i;

	/*assert */
	if( (pstatScanType == NULL) ||\
		(statXCycRec == NULL) ||\
		(pPortIndex == NULL) )
	{
		return GPN_STAT_PROC_GEN_ERR;
	}

	/*when CycRec is disable,  allow delete the alarm if the operation is delete the port moni*/
//	if(statXCycRec->en != GPN_STAT_PROC_GEN_ENABLE)
//	{
//		return GPN_STAT_PROC_GEN_ERR;
//	}

	palmThredInfo = (almThredTpUnit *)statXCycRec->xCycAlmThred;
	psubFileInfo = (subFiltTpUnit *)statXCycRec->subFiltInfo;
	pxCycCurrData = (UINT32 *)statXCycRec->xCycCurrDate;

	if (palmThredInfo == NULL || psubFileInfo == NULL)
	{
		printf("%s:%d make trouble for delete almThred or subFile Template\n\r",\
			__FUNCTION__, __LINE__);
		return GPN_STAT_PROC_GEN_ERR;
	}

	for(i=0;i<statXCycRec->subStatNum;i++)
	{
		bitMask = 1U << i;
		upChg = GPN_STAT_PROC_GEN_NO;
		dnChg = GPN_STAT_PROC_GEN_NO;
		
		/*copy data step : default 64bit */
		if(statXCycRec->dataBitDeep & bitMask)
		{
			currDWStep = GPN_STAT_DATA_64BIT_DWLEN;
			bitDeep = DATA_BIT_DEEP_64;
		}
		else /*if(pxCycDataRecd->dataBitDeep == GPN_STAT_DATA_BIT_DEEP_32)*/
		{
			currDWStep = GPN_STAT_DATA_32BIT_DWLEN;
			bitDeep = DATA_BIT_DEEP_32;
		}

//		printf("delete alarm currDWStep:%d subNum:%d subFileInfo:%d almThredInfo:%d UpAlam:%d CurrData:%d-%d upDispThredL32:%d upRiseThredL32:%d\n\r", 
//			currDWStep, statXCycRec->subStatNum, psubFileInfo->status, palmThredInfo->subType, statXCycRec->statXCycUpAlm, 
//			*pxCycCurrData, *(pxCycCurrData+1), palmThredInfo->upDispThredL32, palmThredInfo->upRiseThredL32);
		/*is subType filt ? */
		if( (psubFileInfo->status == GPN_STAT_PROC_GEN_DISABLE) &&\
			(palmThredInfo->subType != GPN_STAT_SUB_TYPE_INVALID) )
		{
			/*cmp curr data with threshold data */
			if(bitDeep == DATA_BIT_DEEP_64)
			{
				if(statXCycRec->statXCycUpAlm & bitMask)
				{
					/*clear step out up-rise alarm*/
					statXCycRec->statXCycUpAlm &= (~bitMask);
					upChg = GPN_STAT_PROC_GEN_YES;
				}
				
				if(statXCycRec->statXCycDownAlm & bitMask)
				{
					/*clear step out down-rise alarm*/
					statXCycRec->statXCycDownAlm &= (~bitMask);
					dnChg = GPN_STAT_PROC_GEN_YES;
				}
			}
			else /*if(bitDeep == DATA_BIT_DEEP_32)*/
			{
				if(statXCycRec->statXCycUpAlm & bitMask)
				{
					statXCycRec->statXCycUpAlm &= (~bitMask);
					upChg = GPN_STAT_PROC_GEN_YES;
				}
				
				if(statXCycRec->statXCycDownAlm & bitMask)
				{
					statXCycRec->statXCycDownAlm &= (~bitMask);
					dnChg = GPN_STAT_PROC_GEN_YES;
				}
			}
		}

		if(upChg == GPN_STAT_PROC_GEN_YES)
		{
			gpnLog(GPN_LOG_L_INFO, "%s : cycClass(%d) subType(%08x), up-thred alarm clear\n\r",\
				__FUNCTION__, cycClass, palmThredInfo->subType);
			
			pStatSubType = NULL;
			gpnStatTypeGetSubStatTypeNodeBaseScanType(palmThredInfo->subType,
				pstatScanType, &pStatSubType);
			if(pStatSubType != NULL)
			{
				if( (cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
					(pStatSubType->longCycUpThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->longCycUpThAlm, GPN_SOCK_MSG_OPT_CLEAN);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
					(pStatSubType->shortCycUpThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->shortCycUpThAlm, GPN_SOCK_MSG_OPT_CLEAN);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_USR) &&\
					(pStatSubType->userDefCycUpThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->userDefCycUpThAlm, GPN_SOCK_MSG_OPT_CLEAN);
					GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CUP, "%s : GPN_STAT_CYC_CLASS_USR subType(%08x), up-thred alarm\n\r",\
						__FUNCTION__, palmThredInfo->subType);
				}
			}
		}
		
		if(dnChg == GPN_STAT_PROC_GEN_YES)
		{
			gpnLog(GPN_LOG_L_INFO, "%s : cycClass(%d) subType(%08x), dn-thred alarm\n\r",\
				__FUNCTION__, cycClass, palmThredInfo->subType);
			
			pStatSubType = NULL;
			gpnStatTypeGetSubStatTypeNodeBaseScanType(palmThredInfo->subType,
				pstatScanType, &pStatSubType);
			if(pStatSubType != NULL)
			{
				if( (cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
					(pStatSubType->longCycDnThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->longCycDnThAlm, GPN_SOCK_MSG_OPT_CLEAN);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
					(pStatSubType->shortCycDnThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->shortCycDnThAlm, GPN_SOCK_MSG_OPT_CLEAN);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_USR) &&\
					(pStatSubType->userDefCycDnThAlm != GPN_ALM_TYPE_NULL) )
				{
					gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->userDefCycDnThAlm, GPN_SOCK_MSG_OPT_CLEAN);
				}
			}
		}
		
		/*move to next subStat alarm info unit */
		palmThredInfo++;
		/*move to next subStat filt info unit */
		psubFileInfo++;
		/*move to next subStat curr unit */
		pxCycCurrData += currDWStep;
	}

	return GPN_STAT_PROC_GEN_OK;
}

UINT32 gpnStatRiseStatEvent(stStatScanTypeDef *pstatScanType,
	stStatXCycDataRecd *statXCycRec, optObjOrient *pPortIndex, UINT32 cycClass)
{
	stStatSTCharacterDef *pStatSubType;
	evnThredTpUnit *pevnThredInfo;
	subFiltTpUnit *psubFileInfo;
	UINT32 *pxCycCurrData;
	UINT32 currDWStep;
	UINT32 bitDeep;
	UINT32 numType;
	UINT32 bitMask;
	UINT32 upChg;
	UINT32 dnChg;
	UINT32 i;

	/*assert */
	if( (pstatScanType == NULL) ||\
		(statXCycRec == NULL) ||\
		(pPortIndex == NULL) )
	{
		return GPN_STAT_PROC_GEN_ERR;
	}
	
	if(statXCycRec->en != GPN_STAT_PROC_GEN_ENABLE)
	{
		return GPN_STAT_PROC_GEN_ERR;
	}

	pevnThredInfo = (evnThredTpUnit *)statXCycRec->xCycEvnThred;
	psubFileInfo = (subFiltTpUnit *)statXCycRec->subFiltInfo;
	pxCycCurrData = (UINT32 *)statXCycRec->xCycCurrDate;

	if (pevnThredInfo == NULL || psubFileInfo == NULL)
	{
		printf("incase for delete envThred or subFilt Template incorrect\n\r");
		return GPN_STAT_PROC_GEN_ERR;
	}

	for(i=0;i<statXCycRec->subStatNum;i++)
	{
		bitMask = 1U << i;
		upChg = GPN_STAT_PROC_GEN_NO;
		dnChg = GPN_STAT_PROC_GEN_NO;
		
		/*copy data step : default 64bit */
		if(statXCycRec->dataBitDeep & bitMask)
		{
			currDWStep = GPN_STAT_DATA_64BIT_DWLEN;
			bitDeep = DATA_BIT_DEEP_64;
		}
		else /*if(pxCycDataRecd->dataBitDeep == GPN_STAT_DATA_BIT_DEEP_32)*/
		{
			currDWStep = GPN_STAT_DATA_32BIT_DWLEN;
			bitDeep = DATA_BIT_DEEP_32;
		}

		if(statXCycRec->numType & bitMask)
		{
			numType = GPN_INTEGER_NUM_CLASS;
		}
		else
		{
			numType = GPN_NATURAL_NUM_CLASS;
		}

		/*is subType filt ? */
		if( (psubFileInfo->status == GPN_STAT_PROC_GEN_DISABLE) &&\
			(pevnThredInfo->subType != GPN_STAT_SUB_TYPE_INVALID) )
		{
			/*cmp curr data with threshold data */
			if(bitDeep == DATA_BIT_DEEP_64)
			{
				/*if not report event, then cmp*/
				if(!(statXCycRec->statXCycUpEvent & bitMask))
				{
					/*already not report up evnent, so cmp up threshold */
					if( (*pxCycCurrData > pevnThredInfo->upThredH32) ||\
						((*pxCycCurrData == pevnThredInfo->upThredH32) &&\
						(*(pxCycCurrData+1) > pevnThredInfo->upThredL32)) ) 
					{
						/*rise step out up event*/
						statXCycRec->statXCycUpEvent |= bitMask;
						upChg = GPN_STAT_PROC_GEN_YES;
					}
				}
				
				if(!(statXCycRec->statXCycDownEvent & bitMask))
				{
					/*already not report dn evnent, so cmp dn threshold */
					if( (*pxCycCurrData < pevnThredInfo->dnThredH32) ||\
						((*pxCycCurrData == pevnThredInfo->dnThredH32) &&\
						(*(pxCycCurrData+1) < pevnThredInfo->dnThredL32)) ) 
					{
						/*rise step out dn event*/
						statXCycRec->statXCycDownEvent |= bitMask;
						dnChg = GPN_STAT_PROC_GEN_YES;
					}
				}
			}
			else /*if(bitDeep == DATA_BIT_DEEP_32)*/
			{
				/*if not report event, then cmp*/
				if(!(statXCycRec->statXCycUpEvent & bitMask))
				{
					/*already not report up evnent, so cmp up threshold */
					if(numType == GPN_NATURAL_NUM_CLASS)
					{
						if(*pxCycCurrData > pevnThredInfo->upThredL32)
						{
							/*rise step out up event*/
							statXCycRec->statXCycUpEvent |= bitMask;
							upChg = GPN_STAT_PROC_GEN_YES;
						}
					}
					else
					{
						if( ((INT32)(*pxCycCurrData)) > ((INT32)(pevnThredInfo->upThredL32)) )
						{
							/*rise step out up event*/
							statXCycRec->statXCycUpEvent |= bitMask;
							upChg = GPN_STAT_PROC_GEN_YES;
						}
					}
				}
				
				if(!(statXCycRec->statXCycDownEvent& bitMask))
				{
					/*already not report dn evnent, so cmp dn threshold */
					if(numType == GPN_NATURAL_NUM_CLASS)
					{
						if(*pxCycCurrData < pevnThredInfo->dnThredL32)
						{
							/*rise step out dn event*/
							statXCycRec->statXCycDownEvent |= bitMask;
							dnChg = GPN_STAT_PROC_GEN_YES;
						}
					}
					else
					{
						if( ((INT32)(*pxCycCurrData)) < ((INT32)(pevnThredInfo->dnThredL32)) )
						{
							/*rise step out dn event*/
							statXCycRec->statXCycDownEvent |= bitMask;
							dnChg = GPN_STAT_PROC_GEN_YES;
						}
					}
				}
			}
		}

		if(upChg == GPN_STAT_PROC_GEN_YES)
		{
			GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CMP, "%s : subType(%08x), up-thred event\n\r",\
				__FUNCTION__, pevnThredInfo->subType);
			
			pStatSubType = NULL;
			gpnStatTypeGetSubStatTypeNodeBaseScanType(pevnThredInfo->subType,
				pstatScanType, &pStatSubType);
			if(pStatSubType != NULL)
			{
				if( (cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
					(pStatSubType->longCycUpThEvt != GPN_EVT_TYPE_NULL) )
				{
					gpnSockAlmMsgTxEventNotifyBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->longCycUpThEvt, 0);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
					(pStatSubType->shortCycUpThEvt != GPN_EVT_TYPE_NULL) )
				{
					gpnSockAlmMsgTxEventNotifyBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->shortCycUpThEvt, 0);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_USR) &&\
					(pStatSubType->userDefCycUpThEvt != GPN_EVT_TYPE_NULL) )
				{
					gpnSockAlmMsgTxEventNotifyBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->userDefCycUpThEvt, 0);
				}
			}
		}
		
		if(dnChg == GPN_STAT_PROC_GEN_YES)
		{
			GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CMP, "%s : subType(%08x), dn-thred event\n\r",\
				__FUNCTION__, pevnThredInfo->subType);
			
			pStatSubType = NULL;
			gpnStatTypeGetSubStatTypeNodeBaseScanType(pevnThredInfo->subType,
				pstatScanType, &pStatSubType);
			if(pStatSubType != NULL)
			{
				if( (cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
					(pStatSubType->longCycDnThEvt != GPN_EVT_TYPE_NULL) )
				{
					gpnSockAlmMsgTxEventNotifyBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->longCycDnThEvt, 0);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
					(pStatSubType->shortCycDnThEvt != GPN_EVT_TYPE_NULL) )
				{
					gpnSockAlmMsgTxEventNotifyBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->shortCycDnThEvt, 0);
				}
				else if( (cycClass == GPN_STAT_CYC_CLASS_USR) &&\
					(pStatSubType->userDefCycDnThEvt != GPN_EVT_TYPE_NULL) )
				{
					gpnSockAlmMsgTxEventNotifyBaseSubType((optObjOrient *)pPortIndex,\
						pStatSubType->userDefCycDnThEvt, 0);
				}
			}
		}
		
		/*move to next subStat alarm info unit */
		pevnThredInfo++;
		/*move to next subStat filt info unit */
		psubFileInfo++;
		/*move to next subStat curr unit */
		pxCycCurrData += currDWStep;
	}
	return GPN_STAT_PROC_GEN_OK;
}
UINT32 gpnStatClearStatEventMark(stStatXCycDataRecd *statXCycRec)
{
	/*assert */
	if(statXCycRec == NULL)
	{
		return GPN_STAT_PROC_GEN_ERR;
	}

	statXCycRec->statXCycUpEvent = 0;
	statXCycRec->statXCycDownEvent = 0;
	
	return GPN_STAT_PROC_GEN_OK;
}

UINT32 gpnStatHistDataTrapProc(void)
{
	UINT32 hash;
	UINT32 sysTime;
	stStatHistTrapDataNode *ptrapNode;
	stStatHistDataMgt *phistDataMgt;

	if(pgstEQUStatProcSpace->statGlobalInfo.lastHistRept == GPN_STAT_PROC_GEN_ENABLE)
	{
		sysTime = pgstEQUStatProcSpace->statTaskInfo.statSysTime;
		phistDataMgt = &(pgstEQUStatProcSpace->statHistDataMgt);
		
		for(hash=0;hash<phistDataMgt->hash;hash++)
		{
			while(phistDataMgt->actTrapDataNum[hash] > 0)
			{
				ptrapNode = (stStatHistTrapDataNode *)listFirst(&(phistDataMgt->actTrapDataQuen[hash]));
#if 0
				if( (sysTime >= ptrapNode->trapTSlot) &&\
					(sysTime <= ptrapNode->stopTime) )  //when  sysTime more than stopTIme, stop trap
#endif
				if((sysTime >= ptrapNode->trapTSlot) && (sysTime <= ptrapNode->stopTime))
				{
					/*trap node right now */
					GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CUP, "%s : do trap! trapNode(%d) nowTime(%08x) trapSlot(%08x) stopTime(%08x) cycClass(%d) vilSub(%d)!\n\r",\
						__FUNCTION__, ptrapNode->dataIndex, sysTime,\
						ptrapNode->trapTSlot, ptrapNode->stopTime,\
						ptrapNode->cycClass, ptrapNode->validSubNum);

					/*do trap right now */
					gpnStatTrapDataOpt(ptrapNode);

					/*recover node to idleTrapNodeList */
					listDelete(&(phistDataMgt->actTrapDataQuen[hash]), (NODE *)ptrapNode);
					phistDataMgt->actTrapDataNum[hash]--;

					listAdd(&(phistDataMgt->idleTrapDataQuen), (NODE *)ptrapNode);
					phistDataMgt->idleTrapDataNum++;

					/*printf("%s : data real trap index:%d scanType:%08x trapSlot:%d now:%d\n\r",
						__FUNCTION__, ptrapNode->dataIndex, ptrapNode->scanType,\
						ptrapNode->trapTSlot, time(NULL));*/
				}
				else
				{
					break;
				}
			}
		}
	}
	
	return GPN_STAT_PROC_GEN_OK;
}

UINT32 gpnStatPerTaskPushStackProc(void)
{
	UINT32 hash;
	UINT32 taskNum;
	stStatTaskNode *ptaskNode;
	stStatTaskInfo *ptaskInfo;
	
	ptaskInfo = &(pgstEQUStatProcSpace->statTaskInfo);
	
	/*per task hist push time slot check */
	for(hash=0;hash<ptaskInfo->hash;hash++)
	{
		taskNum = 0;
		ptaskNode = (stStatTaskNode *)listFirst(&(ptaskInfo->actTaskQuen[hash]));
		while((ptaskNode != NULL)&&(taskNum < ptaskInfo->actTaskNum[hash]))
		{
			if( (ptaskInfo->statSysTime >= ptaskNode->thisPush) &&\
				(ptaskInfo->statSysTime <= ptaskNode->monEndTime) )  // modify by geqian  2016.1.6       ezview wanted trap time is between start time and stop time
			{
				GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CUP, "%s : task(%d) push time bingo(%08x), sysTime(%08x) pushTime(%08x)\n\r",\
					__FUNCTION__, ptaskNode->taskId, ptaskNode->cycBingo, ptaskInfo->statSysTime,\
					ptaskNode->thisPush);

				zlog_debug(STATIS_DBG_DATA_COLLECT, "%s[%d] : task(%d) push time bingo(%08x), sysTime(%08x) pushTime(%08x)\n\r",\
					__FUNCTION__, __LINE__, ptaskNode->taskId, ptaskNode->cycBingo, ptaskInfo->statSysTime,\
					ptaskNode->thisPush);
				
				/*per port push stack proc */
				gpnStatPerPortPushStackProc(ptaskNode);
				
				/*renew : cycBingo & thisPush should be init when task creat and time adjust */
				ptaskNode->cycBingo += ptaskNode->cycSeconds;
				ptaskNode->thisPush += ptaskNode->cycSeconds;
			}
			
			taskNum++;
			ptaskNode = (stStatTaskNode *)listNext((NODE *)ptaskNode);
		}
	}
	
	return GPN_STAT_PROC_GEN_OK;
}

UINT32 gpnStatPerPortPushStackProc(stStatTaskNode *ptaskNode)
{
	UINT32 objNum;
	stStatMonObjNode *pstatMonObj;
	stStatXCycDataRecd *pxCycDataRecd;
	objLogicDesc *pMonObjIndex;
	optObjOrient *peerPortIndex;
	stStatScanPortInfo *pStatScanPort;

	/*assert */
	if(ptaskNode == NULL)
	{
		return GPN_STAT_PROC_GEN_ERR;
	}

	GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CUP, "%s : task(%d) push bingo!\n\r",\
		__FUNCTION__, ptaskNode->taskId);
	GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CUP, "%s : scanType(%08x) cycClass(%d) actObj(%d)!\n\r",\
		__FUNCTION__, ptaskNode->statScanTp, ptaskNode->cycClass, ptaskNode->actMOnOBjNum);
	
	objNum = 0;
	pstatMonObj = (stStatMonObjNode *)listFirst(&(ptaskNode->actMOnOBjQuen));
	while((pstatMonObj != NULL)&&(objNum < ptaskNode->actMOnOBjNum))
	{
		pStatScanPort = pstatMonObj->pStatLocalPort->pStatScanPort;
		if(ptaskNode->cycClass == GPN_STAT_CYC_CLASS_LONG)
		{
			pxCycDataRecd = pStatScanPort->statLCycRec;
		}
		else if(ptaskNode->cycClass == GPN_STAT_CYC_CLASS_SHORT)
		{
			pxCycDataRecd = pStatScanPort->statSCycRec;
		}
		else if(ptaskNode->cycClass == GPN_STAT_CYC_CLASS_USR)
		{
			/*port x-cyc data push in buff */
			pxCycDataRecd = pStatScanPort->statUCycRec;
		}
		else
		{
			/*cycClass err OR para NULL */
			GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CMP, "%s : task(%d) cycClass(%d) err!\n\r",\
				__FUNCTION__, ptaskNode->taskId, ptaskNode->cycClass);
			
			return GPN_STAT_PROC_GEN_ERR;
		}
		
		peerPortIndex = &(pstatMonObj->pStatLocalPort->pPeerNode->peerPort);
		/*port x-cyc if hsit alarm clear*/
		gpnStatClearStatAlarm(pStatScanPort->pscanTypeNode,\
			pxCycDataRecd, peerPortIndex, ptaskNode->cycClass);
		/*port x-cyc if hsit event clear*/
		gpnStatClearStatEventMark(pxCycDataRecd);
		
		/*port x-cyc data push in buff */
		gpnStatPerCycClassPushStackProc(pxCycDataRecd);
		
		/*port x-cyc hist data trap abut */
		pMonObjIndex = &(pstatMonObj->pStatLocalPort->viewPort);
		gpnStatTrapDataAddInMgt(pxCycDataRecd, ptaskNode, pMonObjIndex);
		
		objNum++;
		pstatMonObj = (stStatMonObjNode *)listNext((NODE *)pstatMonObj);
	}

	return GPN_STAT_PROC_GEN_OK;
}

UINT32 gpnStatPerCycClassPushStackProc(stStatXCycDataRecd *pxCycDataRecd)
{
	UINT32 i;
	UINT32 currDWStep;
	UINT32 valueType;
	UINT32 bitMask;
	UINT32 bitDeep;
	subFiltTpUnit *psubFileInfo;
	UINT32 *pxCycCurrData;
	stStatDataElement *pxCycXHistData;
	stStatHistRecdUnit *precordUnit;

	/*assert */
	if(pxCycDataRecd == NULL)
	{
		return GPN_STAT_PROC_GEN_ERR;
	}

	GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CMP, "%s : en(%d) xCycRecd(%08x) xCycEvnThId(%d) xCycsubFiltId(%d) xCycAlmThId(%d)\n\r",\
		__FUNCTION__, pxCycDataRecd->en, (UINT32)pxCycDataRecd, pxCycDataRecd->xCycEvnThId,\
		pxCycDataRecd->xCycsubFiltId, pxCycDataRecd->xCycAlmThId);
	
	if(pxCycDataRecd->en != GPN_STAT_PROC_GEN_ENABLE)
	{
		return GPN_STAT_PROC_GEN_ERR;
	}
	
	/*renew hist record mark */
	if(pxCycDataRecd->histRecNum < pxCycDataRecd->histRecBufoNum)
	{
		pxCycDataRecd->histRecNum++;
	}
	
	pxCycDataRecd->recEnd += 1;
	pxCycDataRecd->recEnd %= pxCycDataRecd->histRecBufoNum;
	
	if( (pxCycDataRecd->histRecNum == pxCycDataRecd->histRecBufoNum) &&\
		(pxCycDataRecd->recEnd == pxCycDataRecd->recStart) )
	{
		pxCycDataRecd->recStart += 1;
		pxCycDataRecd->recStart %= pxCycDataRecd->histRecBufoNum;
	}
	/*find record space */
	precordUnit = pxCycDataRecd->pstatXCycHistRecd[pxCycDataRecd->recEnd];
	
	/*add to hist data quen : already do it when xCyc mon enable */
	/*listAdd(&(xList), precordUnit->node); */

	/*set hist data Location : already do it when xCyc mon enable */
	/*precordUnit->histDataLoc = pxCycDataRecd->xhistDataLoc;*/
	
	/*record hist data identification */
	/*precordUnit->startTime = pxCycDataRecd->ptaskNode->cycStart;*/
	if(pxCycDataRecd->ptaskNode != NULL)
	{
		precordUnit->stopTime = pxCycDataRecd->ptaskNode->cycBingo;
	}
	else
	{
		precordUnit->stopTime = 0;
		/*return err !!!!*/
	}

	precordUnit->en = GPN_STAT_PROC_GEN_ENABLE;
	/*clear xCycXHist valid subType num */
	/*precordUnit->subWild = pxCycDataRecd->subStatNum;*/
	precordUnit->validSub = 0;

	psubFileInfo = (subFiltTpUnit *)pxCycDataRecd->subFiltInfo;
	pxCycCurrData = (UINT32 *)pxCycDataRecd->xCycCurrDate;
	pxCycXHistData = (stStatDataElement *)precordUnit->statDate;
	
	if (psubFileInfo == NULL)
	{
		printf("%s:%d make trouble for delete subFile Template\n\r",\
			__FUNCTION__, __LINE__);
		return GPN_STAT_PROC_GEN_ERR;
	}
	
	for(i=0;i<pxCycDataRecd->subStatNum;i++)
	{
		bitMask = 1U << i;
		/*copy data step : default 64bit */
		if(pxCycDataRecd->dataBitDeep & bitMask)
		{
			currDWStep = GPN_STAT_DATA_64BIT_DWLEN;
			bitDeep = DATA_BIT_DEEP_64;
		}
		else /*if(pxCycDataRecd->dataBitDeep == GPN_STAT_DATA_BIT_DEEP_32)*/
		{
			currDWStep = GPN_STAT_DATA_32BIT_DWLEN;
			bitDeep = DATA_BIT_DEEP_32;
		}

		if(pxCycDataRecd->valueType & bitMask)
		{
			valueType = GPN_COUNTER_CLASS;
		}
		else
		{
			valueType = GPN_MEASURE_CLASS;
		}

		/*is subType filt ? */
		if(psubFileInfo->status == GPN_STAT_PROC_GEN_DISABLE)
		{
			/*add valid subHist num */
			precordUnit->validSub++;

			/*fill curr data to hist buff*/
			pxCycXHistData->subType = psubFileInfo->subType;
			/*copy data step : default 64bit */
			if(bitDeep == DATA_BIT_DEEP_64)
			{
				pxCycXHistData->dataHigh32 = *pxCycCurrData;
				pxCycXHistData->dataLow32 = *(pxCycCurrData + 1);

				/*clear current data */
				//*pxCycCurrData = 0;
				//*(pxCycCurrData + 1) = 0;
			}
			else /*if(bitDeep == DATA_BIT_DEEP_32)*/
			{
				pxCycXHistData->dataHigh32 = 0;
				pxCycXHistData->dataLow32 = *pxCycCurrData;

				/*clear current data */
				//*pxCycCurrData = 0;
			}

			GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CMP, "%s : bitDeep(%d) push subType(%08x) h(%08x) l(%08x) to stash\n\r",\
					__FUNCTION__, bitDeep, psubFileInfo->subType, pxCycXHistData->dataHigh32, pxCycXHistData->dataLow32);
		}
		else
		{
			pxCycXHistData->subType = GPN_STAT_SUB_TYPE_INVALID;
			/*pxCycXHistData->dataHigh32 = 0;
			pxCycXHistData->dataHigh32 = 0;*/
		}

		/* 
			just clear GPN_COUNTER_CLASS current data:
			when stat value comp with threshold,
			proccesses may not give fresh stat date to gpn_stat,
			so, for GPN_MEASURE_CLASS stat value,
			clear will cause error stat_alarm
		*/
		if(valueType == GPN_COUNTER_CLASS)
		{
			if(bitDeep == DATA_BIT_DEEP_64)
			{
				*pxCycCurrData = 0;
				*(pxCycCurrData + 1) = 0;
			}
			else /*if(bitDeep == DATA_BIT_DEEP_32)*/
			{
				*pxCycCurrData = 0;
			}
		}
		
		/*move to next subStat filt info unit */
		psubFileInfo++;
		/*move to next subStat curr unit */
		pxCycCurrData += currDWStep;
		/*move to next subHist data buff */
		pxCycXHistData++;
	}

	GPN_STAT_PROC_PRINT(GPN_STAT_PROC_CMP, "%s : hist in stack, deep(%08x) sunNum(%d) vilSub(%d) start(%d) end(%d) recNum(%d/%d)\n\r",\
		__FUNCTION__, pxCycDataRecd->dataBitDeep, precordUnit->subWild, precordUnit->validSub,\
		pxCycDataRecd->recStart, pxCycDataRecd->recEnd,\
		pxCycDataRecd->histRecNum, pxCycDataRecd->histRecBufoNum);
	/*printf("%s : hist in stack, deep(%08x) sunNum(%d) vilSub(%d) start(%d) end(%d) recNum(%d/%d)\n\r",\
		__FUNCTION__, pxCycDataRecd->dataBitDeep, precordUnit->subWild, precordUnit->validSub,\
		pxCycDataRecd->recStart, pxCycDataRecd->recEnd,\
		pxCycDataRecd->histRecNum, pxCycDataRecd->histRecBufoNum);
	printf("%s : precordUnit->stopTime(%d) now time(%d)\n\r",\
		__FUNCTION__, precordUnit->stopTime, time(NULL));*/
	
	return GPN_STAT_PROC_GEN_OK;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_STAT_PROC_MAIN_C_ */

