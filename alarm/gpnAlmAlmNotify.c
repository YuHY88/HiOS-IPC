/**********************************************************
* file name: gpnAlmAlmNotify.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-12
* function: 
*    define alarms notify by processes process detail
* modify:
*
***********************************************************/

#ifndef _GPN_ALM_ALM_NOTIFY_C_
#define _GPN_ALM_ALM_NOTIFY_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <lib/syslog.h>
#include <lib/log.h>

#include "gpnAlmAlmNotify.h"

#include "gpnAlmTypeStruct.h"
#include "gpnAlmDataSheet.h"

/*log function include*/
#include "gpnLog/gpnLogFuncApi.h"

#include "gpnAlmSnmp.h"
#include "lib/snmp_common.h"
#include "lib/devm_com.h"
#include "gpnAlmDebug.h"

#include "gpnAlmMasterSlaveSync.h"

extern stEQUAlmProcSpace *pgstAlmPreScanWholwInfo;


/*==================================================*/
/*name :  gpnAlmANtProductReport                                                                       */
/*para :                                                                                                                */
/*retu :  void                                                                                                        */
/*desc :  gpn_alarm  report alarm status to snmp agent                                            */ 
/*==================================================*/
UINT32 gpnAlmANtProductReport(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd)
{
	/* 主备同步开关，当为备盘时，不处理告警上报 */
	if(DEVM_HA_SLAVE == alarm_MS_my_status_get())
	{
		return GPN_ALM_GEN_YES;
	}
	
	stSnmpAlmCurrDSTableInfo stInfo;
	memset (&stInfo, 0, sizeof(stSnmpAlmCurrDSTableInfo));
	stInfo.index.iAlmCurrDSIndex = pAlmValueRecd->index;
	stInfo.data.iCurrAlmType    = pAlmValueRecd->almSubType;
	stInfo.data.iCurrDevIndex   = pViewPort->devIndex;
	stInfo.data.iCurrPortIndex1 = pViewPort->portIndex;
	stInfo.data.iCurrPortIndex2 = pViewPort->portIndex3;
	stInfo.data.iCurrPortIndex3 = pViewPort->portIndex4;
	stInfo.data.iCurrPortIndex4 = pViewPort->portIndex5;
	stInfo.data.iCurrPortIndex5 = pViewPort->portIndex6;
	stInfo.data.iCurrAlmLevel = pAlmValueRecd->portAlmRank;
	stInfo.data.iAlmCurrProductCnt = pAlmValueRecd->prodCnt;
	stInfo.data.iAlmCurrFirstProductTime = pAlmValueRecd->firstTime;
	stInfo.data.iAlmCurrThisProductTime = pAlmValueRecd->thisTime;

	zlog_debug(ALARM_DBG_REPORT, "%s[%d] : send alm rise to snmp (%d|%x|%x|%x|%x|%x|%x|%x|%d|%d|%d|%d)\n", __func__, __LINE__,
		stInfo.index.iAlmCurrDSIndex,
		stInfo.data.iCurrAlmType,
		stInfo.data.iCurrDevIndex,		
		stInfo.data.iCurrPortIndex1,
		stInfo.data.iCurrPortIndex2,
		stInfo.data.iCurrPortIndex3,
		stInfo.data.iCurrPortIndex4,
		stInfo.data.iCurrPortIndex5,
		stInfo.data.iCurrAlmLevel,
		stInfo.data.iAlmCurrProductCnt,
		stInfo.data.iAlmCurrFirstProductTime,
		stInfo.data.iAlmCurrThisProductTime);


	/* First : add data struct to send trap msg to snmp */
	struct snmp_trap_hdr trap_hdr;
	struct snmp_trap_data trap_data;
	memset(&trap_hdr, 0, sizeof(struct snmp_trap_hdr));
	memset(&trap_data, 0, sizeof(struct snmp_trap_data));

	uchar buf[IPC_MSG_LEN];
	memset(buf, 0, IPC_MSG_LEN);

	/* Second : add trap msg header */
	oid oid_alarm_trap_product[] 	 = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 14, 7, 1};

	trap_hdr.trap_oid = oid_alarm_trap_product;
	trap_hdr.len_trap_oid = OID_LENGTH(oid_alarm_trap_product);	
	if(0 == snmp_add_trap_msg_hdr(buf, &trap_hdr))
	{
		return GPN_ALM_GEN_NO;
	}	

	/* Third : add trap msg data */
	oid oid_alarm_trap_product_row[] = {1, 3, 6, 1, 4, 1, 9966, 5, 14, 11, 1, 1, 0, 0};	

	const struct trap_info	trapInfo[]		= {
		{ 1 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stInfo.index.iAlmCurrDSIndex)), 			sizeof(stInfo.index.iAlmCurrDSIndex) },
		{ 2 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stInfo.data.iCurrAlmType)), 				sizeof(stInfo.data.iCurrAlmType) },
		{ 3 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stInfo.data.iCurrDevIndex)),				sizeof(stInfo.data.iCurrDevIndex) },
		{ 4 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stInfo.data.iCurrPortIndex1)),			sizeof(stInfo.data.iCurrPortIndex1) },
		{ 5 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stInfo.data.iCurrPortIndex2)),			sizeof(stInfo.data.iCurrPortIndex2) },
		{ 6 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stInfo.data.iCurrPortIndex3)),			sizeof(stInfo.data.iCurrPortIndex3) },
		{ 7 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stInfo.data.iCurrPortIndex4)),			sizeof(stInfo.data.iCurrPortIndex4) },
		{ 8 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stInfo.data.iCurrPortIndex5)),			sizeof(stInfo.data.iCurrPortIndex5) },
		{ 9 , SNMP_TYPE_INTEGER,  (unsigned char *)(&(stInfo.data.iCurrAlmLevel)),				sizeof(stInfo.data.iCurrAlmLevel) },
		{ 10, SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stInfo.data.iAlmCurrProductCnt)),			sizeof(stInfo.data.iAlmCurrProductCnt) },
		{ 11, SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stInfo.data.iAlmCurrFirstProductTime)),	sizeof(stInfo.data.iAlmCurrFirstProductTime) },
		{ 12, SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stInfo.data.iAlmCurrThisProductTime)),	sizeof(stInfo.data.iAlmCurrThisProductTime) },
	};

	const int NodeNum = sizeof(trapInfo)/sizeof(trapInfo[0]);	
	
	int i = 0;
	size_t len = 0;
	for(i = 0; i < NodeNum; i++)
	{
		// index
		len 								= sizeof(oid_alarm_trap_product_row)/sizeof(oid_alarm_trap_product_row[0]);
		oid_alarm_trap_product_row[len - 2]	= trapInfo[i].iOid;
		oid_alarm_trap_product_row[len - 1]	= stInfo.index.iAlmCurrDSIndex;

		trap_data.type = trapInfo[i].type;
		trap_data.len_data_oid = len;
		trap_data.len_data = trapInfo[i].len;
		trap_data.data_oid = oid_alarm_trap_product_row;
		trap_data.data = trapInfo[i].buf;

		if(0 == snmp_add_trap_msg_data(buf, &trap_data))
		{
			return GPN_ALM_GEN_NO;
		}
	}

	/* Last : send msg to snmp */
	snmp_send_trap_msg(buf, MODULE_ID_ALARM, 0, 0);
	
	//alarm_send_trap ((char *)&stInfo, sizeof(stSnmpAlmCurrDSTableInfo), ALM_PRODUCT_TRAP);
	return GPN_ALM_GEN_YES;
}


UINT32 gpnAlmANtDisappReport(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd)
{
	/* 主备同步开关，当为备盘时，不处理告警上报 */
	if(DEVM_HA_SLAVE == alarm_MS_my_status_get())
	{
		return GPN_ALM_GEN_YES;
	}

	stSnmpAlmHistDSTableInfo stInfo;
	memset (&stInfo, 0, sizeof(stSnmpAlmHistDSTableInfo));
	stInfo.index.iAlmHistDSIndex = pAlmValueRecd->index;
	stInfo.data.iHistAlmType = pAlmValueRecd->almSubType;
	stInfo.data.iHistDevIndex   = pViewPort->devIndex;
	stInfo.data.iHistPortIndex1 = pViewPort->portIndex;
	stInfo.data.iHistPortIndex2 = pViewPort->portIndex3;
	stInfo.data.iHistPortIndex3 = pViewPort->portIndex4;
	stInfo.data.iHistPortIndex4 = pViewPort->portIndex5;
	stInfo.data.iHistPortIndex5 = pViewPort->portIndex6;
	stInfo.data.iHistAlmLevel = pAlmValueRecd->portAlmRank;
	stInfo.data.iAlmHistProductCnt = pAlmValueRecd->prodCnt;
	stInfo.data.iAlmHistFirstProductTime = pAlmValueRecd->firstTime;
	stInfo.data.iAlmHistThisProductTime = pAlmValueRecd->thisTime;
	stInfo.data.iAlmHistThisDisappearTime = pAlmValueRecd->disapTime;

	zlog_debug(ALARM_DBG_REPORT, "%s[%d] : send alm down to snmp (%d|%x|%x|%x|%x|%x|%x|%x|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
		stInfo.index.iAlmHistDSIndex,
		stInfo.data.iHistAlmType,
		stInfo.data.iHistDevIndex,
		stInfo.data.iHistPortIndex1,
		stInfo.data.iHistPortIndex2,
		stInfo.data.iHistPortIndex3,
		stInfo.data.iHistPortIndex4,
		stInfo.data.iHistPortIndex5,
		stInfo.data.iHistAlmLevel,
		stInfo.data.iAlmHistProductCnt,
		stInfo.data.iAlmHistFirstProductTime,
		stInfo.data.iAlmHistThisProductTime,
		stInfo.data.iAlmHistThisDisappearTime);
	
	alarm_send_trap ((char *)&stInfo, ALM_DISAPPEAR_TRAP);	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmANtEvtProductReport(objLogicDesc *pViewPort, stEvtValueRecd *pEvtValueRecd)
{
	/* 主备同步开关，当为备盘时，不处理告警上报 */
	if(DEVM_HA_SLAVE == alarm_MS_my_status_get())
	{
		return GPN_ALM_GEN_YES;
	}

	stSnmpEventDSTableInfo stInfo;
	memset (&stInfo, 0, sizeof(stSnmpEventDSTableInfo));
	stInfo.index.iEventDSIndex = pEvtValueRecd->index;
	stInfo.data.iEventAlmType = pEvtValueRecd->evtSubType;
	stInfo.data.iEventDevIndex   = pViewPort->devIndex;
	stInfo.data.iEventPortIndex1 = pViewPort->portIndex;
	stInfo.data.iEventPortIndex2 = pViewPort->portIndex3;
	stInfo.data.iEventPortIndex3 = pViewPort->portIndex4;
	stInfo.data.iEventPortIndex4 = pViewPort->portIndex5;
	stInfo.data.iEventPortIndex5 = pViewPort->portIndex6;
	stInfo.data.iEventAlmLevel = pEvtValueRecd->evtRank;
	stInfo.data.iEventDetail = pEvtValueRecd->detial;
	stInfo.data.iEventTime = pEvtValueRecd->thisTime;

	zlog_debug(ALARM_DBG_REPORT, "%s[%d] : send event report to snmp (%d|%x|%x|%x|%x|%x|%x|%x|%d|%d)\n", __func__, __LINE__,
		stInfo.index.iEventDSIndex,
		stInfo.data.iEventAlmType,
		stInfo.data.iEventDevIndex,
		stInfo.data.iEventPortIndex1,
		stInfo.data.iEventPortIndex2,
		stInfo.data.iEventPortIndex3,
		stInfo.data.iEventPortIndex4,
		stInfo.data.iEventPortIndex5,
		stInfo.data.iEventAlmLevel,
		stInfo.data.iEventDetail,
		stInfo.data.iEventTime);

	alarm_send_trap ((char *)&stInfo, ALM_EVENT_REPORT_TRAP);	
	return GPN_ALM_GEN_YES;
}

/*==================================================*/
/*name :  gpnAlmANtCurrAlmInsertDS                                                                   */
/*para :                                                                                                                */
/*retu :  void                                                                                                        */
/*desc :  gpn_alarm  record alarm status and process shaking                                    */ 
/*==================================================*/
UINT32 gpnAlmANtCurrAlmInsertDS(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd)
{
	stCurrAlmDB currAlm;

	/* pAlmValueRecd's index renew at here */
	pAlmValueRecd->index = pgstAlmPreScanWholwInfo->almGlobalCfg.currAlmIndex++;
	
	currAlm.index = pAlmValueRecd->index;
	currAlm.almType = pAlmValueRecd->almSubType;
	currAlm.devIndex = pViewPort->devIndex;
	currAlm.ifIndex = pViewPort->portIndex;
	currAlm.ifIndex2 = pViewPort->portIndex3;
	currAlm.ifIndex3 = pViewPort->portIndex4;
	currAlm.ifIndex4 = pViewPort->portIndex5;
	currAlm.ifIndex5 = pViewPort->portIndex6;
	currAlm.level = pAlmValueRecd->portAlmRank;

	/* 备盘从主盘获取时间、产生次数等，2018/8/16 */
	if(DEVM_HA_MASTER == alarm_MS_my_status_get())
	{
		/* pAlmValueRecd's prodCnt/firstTime/thisTime renew at here */
		if(pAlmValueRecd->prodCnt > 0)
		{
			pAlmValueRecd->prodCnt++;
			/*pAlmValueRecd->thisTime = (UINT32)time(NULL);*/
			pAlmValueRecd->thisTime = (UINT32)pgstAlmPreScanWholwInfo->almTaskTime;
		}
		else
		{
			pAlmValueRecd->prodCnt++;
			/*pAlmValueRecd->firstTime = (UINT32)time(NULL);*/
			pAlmValueRecd->firstTime = (UINT32)pgstAlmPreScanWholwInfo->almTaskTime;
			pAlmValueRecd->thisTime = pAlmValueRecd->firstTime;
		}
	}
			
	currAlm.count = pAlmValueRecd->prodCnt;
	currAlm.firstTime = pAlmValueRecd->firstTime;
	currAlm.thisTime = pAlmValueRecd->thisTime;

	/* for VPLS alm Add, TODO ??? f*/
	currAlm.suffixLen = 0;

	GPN_ALM_ANT_PRINT(GPN_ALM_ANT_CUP, "%s : prot(%08x-%08x|%08x|%08x|%08x|%08x) "
		"almType(%08x) opt(%d) count(%d) time(%08x)\n\r",\
		__FUNCTION__,\
		pViewPort->devIndex, pViewPort->portIndex,\
		pViewPort->portIndex3, pViewPort->portIndex4,\
		pViewPort->portIndex5, pViewPort->portIndex6,\
		pAlmValueRecd->almSubType, pAlmValueRecd->iAlmValue,\
		currAlm.count, currAlm.thisTime);

	zlog_debug(ALARM_DBG_REPORT, "%s[%d] : prot(%08x-%08x|%08x|%08x|%08x|%08x) "
		"almType(%08x) opt(%d) count(%d) time(%08x)\n\r",\
		__FUNCTION__, __LINE__,\
		pViewPort->devIndex, pViewPort->portIndex,\
		pViewPort->portIndex3, pViewPort->portIndex4,\
		pViewPort->portIndex5, pViewPort->portIndex6,\
		pAlmValueRecd->almSubType, pAlmValueRecd->iAlmValue,\
		currAlm.count, currAlm.thisTime);

	gpnAlmDataSheetAdd(GPN_ALM_CURR_ALM_DSHEET, currAlm.index, (void *)(&currAlm), sizeof(stCurrAlmDB));

	return GPN_ALM_GEN_OK;
}
UINT32 gpnAlmANtCurrAlmOutDS(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd)
{
	UINT32 reVal;

	GPN_ALM_ANT_PRINT(GPN_ALM_ANT_CUP, "%s : prot(%08x-%08x|%08x|%08x|%08x|%08x) "
		"almType(%08x) opt(%d) count(%d) time(%08x)\n\r",\
		__FUNCTION__,\
		pViewPort->devIndex, pViewPort->portIndex,\
		pViewPort->portIndex3, pViewPort->portIndex4,\
		pViewPort->portIndex5, pViewPort->portIndex6,\
		pAlmValueRecd->almSubType, pAlmValueRecd->iAlmValue,\
		pAlmValueRecd->prodCnt, pAlmValueRecd->thisTime);

	zlog_debug(ALARM_DBG_REPORT, "%s[%d] : prot(%08x-%08x|%08x|%08x|%08x|%08x) "
		"almType(%08x) opt(%d) count(%d) time(%08x)\n\r",\
		__FUNCTION__, __LINE__,\
		pViewPort->devIndex, pViewPort->portIndex,\
		pViewPort->portIndex3, pViewPort->portIndex4,\
		pViewPort->portIndex5, pViewPort->portIndex6,\
		pAlmValueRecd->almSubType, pAlmValueRecd->iAlmValue,\
		pAlmValueRecd->prodCnt, pAlmValueRecd->thisTime);
	
	reVal = gpnAlmDataSheetDelete(GPN_ALM_CURR_ALM_DSHEET, pAlmValueRecd->index);
	if(reVal != GPN_ALM_GEN_OK)
	{
		gpnLog(GPN_LOG_L_ALERT,\
			"%s : search curr alm err almTp %08x "
			"protIndex(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__, pAlmValueRecd->almSubType,\
			pViewPort->devIndex, pViewPort->portIndex,\
			pViewPort->portIndex3, pViewPort->portIndex4,\
			pViewPort->portIndex5, pViewPort->portIndex6);
		
		return GPN_ALM_GEN_ERR;
	}
	else
	{
		return GPN_ALM_GEN_OK;
	}
}

UINT32 gpnAlmANtHistAlmInsertDS(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd)
{
	stHistAlmDB histAlm;

	histAlm.index = pgstAlmPreScanWholwInfo->almGlobalCfg.histAlmIndex++;
	histAlm.almType = pAlmValueRecd->almSubType;
	histAlm.devIndex = pViewPort->devIndex;
	histAlm.ifIndex = pViewPort->portIndex;
	histAlm.ifIndex2 = pViewPort->portIndex3;
	histAlm.ifIndex3 = pViewPort->portIndex4;
	histAlm.ifIndex4 = pViewPort->portIndex5;
	histAlm.ifIndex5 = pViewPort->portIndex6;
	histAlm.level = pAlmValueRecd->portAlmRank;

	/* pAlmValueRecd's index/disapTime renew at here */
	pAlmValueRecd->index = histAlm.index;
	/*pAlmValueRecd->disapTime = (UINT32)time(NULL);*/
	
	/* 备盘从主盘获取时间、产生次数等，2018/8/16 */
	if(DEVM_HA_MASTER == alarm_MS_my_status_get())
	{
		pAlmValueRecd->disapTime = (UINT32)pgstAlmPreScanWholwInfo->almTaskTime;
	}
	
	histAlm.count = pAlmValueRecd->prodCnt;
	histAlm.firstTime = pAlmValueRecd->firstTime;
	histAlm.thisTime = pAlmValueRecd->thisTime;
	histAlm.disapTime = pAlmValueRecd->disapTime;

	/* for VPLS alm Add, TODO ??? f*/
	histAlm.suffixLen = 0;

	GPN_ALM_ANT_PRINT(GPN_ALM_ANT_CUP, "%s : prot(%08x-%08x|%08x|%08x|%08x|%08x) "
		"almType(%08x) opt(%d) count(%d) time(%08x)\n\r",\
		__FUNCTION__,\
		pViewPort->devIndex, pViewPort->portIndex,\
		pViewPort->portIndex3, pViewPort->portIndex4,\
		pViewPort->portIndex5, pViewPort->portIndex6,\
		pAlmValueRecd->almSubType, pAlmValueRecd->iAlmValue,\
		pAlmValueRecd->prodCnt, pAlmValueRecd->thisTime);

	zlog_debug(ALARM_DBG_REPORT, "%s[%d] : prot(%08x-%08x|%08x|%08x|%08x|%08x) "
		"almType(%08x) opt(%d) count(%d) time(%08x)\n\r",\
		__FUNCTION__, __LINE__,\
		pViewPort->devIndex, pViewPort->portIndex,\
		pViewPort->portIndex3, pViewPort->portIndex4,\
		pViewPort->portIndex5, pViewPort->portIndex6,\
		pAlmValueRecd->almSubType, pAlmValueRecd->iAlmValue,\
		pAlmValueRecd->prodCnt, pAlmValueRecd->thisTime);
	
	gpnAlmDataSheetAdd(GPN_ALM_HIST_ALM_DSHEET, histAlm.index,  &histAlm, sizeof(stHistAlmDB));
	
	return GPN_ALM_GEN_YES;
}

/*==================================================*/
/*name :  gpnAlmANtSubAlmStatRecord                                                                */
/*para :                                                                                                                */
/*retu :  void                                                                                                        */
/*desc :  gpn_alarm  record alarm status and process shaking                                    */ 
/*==================================================*/
UINT32 gpnAlmANtSubAlmStatRecord(stAlmScanTypeDef *pAlmScanType,
						stAlmPeerNode *pAlmPeerPort, UINT32 almSubType, UINT32 almSta)
{
	UINT32 iAlmData;
	UINT32 iAlmBitV;
	UINT32 page;
	
	stAlmSTCharacterDef *pAlmSubTpStr;
	stAlmShakeRecd *pAlmShakeRecd;
	stAlmShakeUnit *pAlmShakeUnit;

	stAlmScanPortInfo *pAlmScanPort;

	pAlmScanPort = pAlmPeerPort->pAlmScanPort;
	
	/* alarm location port shoud first in valid scan quen */
	if(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_ENABLE)
	{
		/* find sub alarm type node base sub type */
		pAlmSubTpStr = NULL;
		gpnAlmSeekAlmSubTpToAlmSubTpBitValue(almSubType, pAlmScanType, &pAlmSubTpStr);
		if(pAlmSubTpStr != NULL)
		{
			page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
			iAlmBitV = pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP;
			
			pAlmShakeRecd = &(pAlmScanPort->AlmShakeBuff[pAlmSubTpStr->devOrderInScanTp]);
			pAlmShakeUnit = &(pAlmShakeRecd->shakeUnit[pAlmScanPort->valShakeUnit%2]);

			/* check if alarm is exist in alarm scan type 
			iAlmBitV &= pAlmPreScanNode->almScanTypeNode->scanTypeAlmMask[page];*/
			
			if(almSta == GPN_ALM_ARISE)
			{
				iAlmData = iAlmBitV & pAlmScanPort->iScanAlmDate[page];				
				if(iAlmData == 0)
				{
					/*1 renew alarm status */
					pAlmScanPort->iScanAlmDate[page] |= iAlmBitV;
					pAlmScanPort->iFrehAlmMark[page] |= iAlmBitV;
					pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM]++;
					
					/*2 record alarm shake */
					if(pAlmShakeUnit->stat == GPN_ALM_CLEAN)
					{
						pAlmShakeUnit->stat = GPN_ALM_ARISE;
						
						pAlmScanPort->iShakAlmMark[page] |= iAlmBitV;
						pAlmScanPort->iShakAlmMark[GPN_ALM_PAGE_NUM]++;
					}
					else if(pAlmShakeUnit->stat == GPN_ALM_UNKOWN)
					{
						pAlmShakeUnit->stat = GPN_ALM_ARISE;
						pAlmShakeUnit->shakeCnt = 0; 
						pAlmShakeUnit->riseTime = pgstAlmPreScanWholwInfo->almTaskTime;
						
						pAlmScanPort->iShakAlmMark[page] |= iAlmBitV;
						pAlmScanPort->iShakAlmMark[GPN_ALM_PAGE_NUM]++;
					}
					else
					{
						/* do nothing */
					}
					
				}
			}
			else if(almSta == GPN_ALM_CLEAN)
			{
				iAlmData = iAlmBitV & pAlmScanPort->iScanAlmDate[page];				
				if(iAlmData == iAlmBitV)
				{
					/*1 renew alarm status */
					pAlmScanPort->iScanAlmDate[page] &= (~iAlmBitV);
					pAlmScanPort->iFrehAlmMark[page] |= iAlmBitV;
					pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM]++;					
					
					/*2 record alarm shake */
					if(pAlmShakeUnit->stat == GPN_ALM_ARISE)
					{
						pAlmShakeUnit->stat = GPN_ALM_CLEAN;
						pAlmShakeUnit->shakeCnt++; 
						pAlmShakeUnit->dispTime = pgstAlmPreScanWholwInfo->almTaskTime;
						
						pAlmScanPort->iShakAlmMark[page] |= iAlmBitV;
						pAlmScanPort->iShakAlmMark[GPN_ALM_PAGE_NUM]++;
					}
					else if(pAlmShakeUnit->stat == GPN_ALM_UNKOWN)
					{
						/* do nothing */
					}
					else
					{
						/* do nothing */
					}
				}
			}
			else
			{
				return GPN_ALM_GEN_NO;
			}
			
			/* if port have fresh alarm hapen, then chang port position in alarm scan list */
			if(pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM] != 0)
			{
				/* will do and to do */
			}
			
			GPN_ALM_ANT_PRINT(GPN_ALM_SCAN_CUP,\
				"%s : almSubType(%08x) opt(%d) perr port%08x|%08x|%08x|%08x|%08x|%08x)\n\r",\
				__FUNCTION__, almSubType, almSta,\
				pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
				pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
				pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6);

			return GPN_ALM_GEN_OK;
		}
		else
		{
			GPN_ALM_ANT_PRINT((GPN_ALM_SCAN_CUP)|(GPN_ALM_SCAN_UD0),\
				"%s : find subTp err, perr port(%08x|%08x|%08x|%08x|%08x|%08x) almSubType(%08x) opt(%d) \n\r",\
				__FUNCTION__, \
				pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
				pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
				pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6,\
				almSubType, almSta);
			
			gpnLog(GPN_LOG_L_ALERT, "%s : find subTp err, perr port(%08x|%08x|%08x|%08x|%08x|%08x) almSubType(%08x) opt(%d) \n\r",\
				__FUNCTION__, \
				pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
				pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
				pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6,\
				almSubType, almSta);;
			return GPN_ALM_GEN_NO;
		}
	}
	else
	{
		/* port in perScanQuen, but not in validScanQuen */
		gpnLog(GPN_LOG_L_ALERT,\
			"%s : not in valid list, port(%08x|%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__,\
			pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
			pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
			pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6);
		
		GPN_ALM_ANT_PRINT(GPN_ALM_SCAN_SVP, "%s : not in valid list, port(%08x|%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__,\
			pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
			pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
			pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6);

		return GPN_ALM_GEN_NO;
	}
}

/*==================================================*/
/*name :  gpnAlmANtBaseSubTypeProc                                                                  */
/*para :                                                                                                                */
/*retu :  void                                                                                                        */
/*desc :  p-process notify alarm base portIndex, almSubType, almOpt ; gpn_alarm     */ 
/*          record alarm status and process shaking                                                     */
/*==================================================*/
UINT32 gpnAlmANtBaseSubTypeProc(optObjOrient *pPortInfo, UINT32 almSubType, UINT32 almSta)
{
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stAlmPreScanQuen *pAlmPreScanNode;
	stAlmPeerNode *pAlmPeerPort;
	optObjOrient *pstProtInfo;
	
	UINT32 peerScanNodeNum;
	UINT32 protTypeNum;
	UINT32 portType;
	UINT32 slotId;
	UINT32 hashKey;

	/* get port type from first(master)-portIndex，端口类型，如 IFM_ENV_TYPE */
	portType = PortIndex_GetType(pPortInfo->portIndex);
	
	/* get scan type base port type */
	protTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));

	while((pPortTpToScanTp != NULL)&&(protTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{		
		if(pPortTpToScanTp->portType == portType)
		{
			/* get slot from first(master)-portIndex */
			slotId = PortIndex_GetSlot(pPortInfo->portIndex);
			pAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
			/* get hash key by first(master)-portIndex */
			hashKey = (pPortInfo->portIndex)%(pAlmPreScanNode->hashKey);
			
			zlog_debug(ALARM_DBG_REPORT, "portType %d slot %d hasKey %d pAlmPreScanNode->peerPortNodeNum[hashKey] %d\n\r",\
				portType, slotId, hashKey,\
				pAlmPreScanNode->peerPortNodeNum[hashKey]);
			
			/* here through 3 level hash(port type, slot, per scan hash quen), find port */
			peerScanNodeNum = 0;
			pAlmPeerPort = (stAlmPeerNode *)listFirst(&(pAlmPreScanNode->almPeerPortQuen[hashKey]));
			while((pAlmPeerPort != NULL)&&(peerScanNodeNum < pAlmPreScanNode->peerPortNodeNum[hashKey]))
			{
				pstProtInfo = &(pAlmPeerPort->peerPort);
					
				/* here devIndex should do what modify */
				if( (pstProtInfo->devIndex   == pPortInfo->devIndex) &&\
					(pstProtInfo->portIndex  == pPortInfo->portIndex) &&\
					(pstProtInfo->portIndex3 == pPortInfo->portIndex3) &&\
					(pstProtInfo->portIndex4 == pPortInfo->portIndex4) &&\
					(pstProtInfo->portIndex5 == pPortInfo->portIndex5) &&\
					(pstProtInfo->portIndex6 == pPortInfo->portIndex6) )
				{
					GPN_ALM_ANT_PRINT(GPN_ALM_SCAN_CUP,\
						"%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) opt(%d)\n\r",\
						__FUNCTION__,\
						pstProtInfo->devIndex, pstProtInfo->portIndex,\
						pstProtInfo->portIndex3, pstProtInfo->portIndex4,\
						pstProtInfo->portIndex5, pstProtInfo->portIndex6,\
						almSubType, almSta);

					zlog_debug(ALARM_DBG_REPORT, "%s[%d] : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) opt(%d)\n\r",\
						__FUNCTION__, __LINE__,\
						pstProtInfo->devIndex, pstProtInfo->portIndex,\
						pstProtInfo->portIndex3, pstProtInfo->portIndex4,\
						pstProtInfo->portIndex5, pstProtInfo->portIndex6,\
						almSubType, almSta);
					
					/* gpn_alarm record alarm status and process shaking */
					gpnAlmANtSubAlmStatRecord(pAlmPreScanNode->almScanTypeNode,\
						pAlmPeerPort, almSubType, almSta);

					break;
				}
				
				peerScanNodeNum++;
				pAlmPeerPort = (stAlmPeerNode *)listNext((NODE *)(pAlmPeerPort));
			}
			
			if((pAlmPeerPort == NULL)||(peerScanNodeNum >= pAlmPreScanNode->peerPortNodeNum[hashKey]))
			{
				/* in pre scan quen, can not find valid port base portIndex */
				GPN_ALM_ANT_PRINT(GPN_ALM_SCAN_CUP,\
					"%s : no port(%08x-%08x|%08x|%08x|%08x|%08x) "
					"in scanType(%08x)'s hash(%d) quen, err!\n\r",\
					__FUNCTION__,\
					pPortInfo->devIndex, pPortInfo->portIndex,\
					pPortInfo->portIndex3, pPortInfo->portIndex4,\
					pPortInfo->portIndex5, pPortInfo->portIndex6,\
					pAlmPreScanNode->almScanType, hashKey);
				
				gpnLog(GPN_LOG_L_ALERT,\
					"%s : no port(%08x-%08x|%08x|%08x|%08x|%08x) "
					"in scanType(%08x)'s hash(%d) quen, err!\n\r",\
					__FUNCTION__,\
					pPortInfo->devIndex, pPortInfo->portIndex,\
					pPortInfo->portIndex3, pPortInfo->portIndex4,\
					pPortInfo->portIndex5, pPortInfo->portIndex6,\
					pAlmPreScanNode->almScanType, hashKey);

				zlog_err("%s[%d] : no port(%08x-%08x|%08x|%08x|%08x|%08x) "
					"in scanType(%08x)'s hash(%d) quen, err!\n\r",\
					__FUNCTION__, __LINE__,\
					pPortInfo->devIndex, pPortInfo->portIndex,\
					pPortInfo->portIndex3, pPortInfo->portIndex4,\
					pPortInfo->portIndex5, pPortInfo->portIndex6,\
					pAlmPreScanNode->almScanType, hashKey);
				
				return GPN_ALM_GEN_NO;
			}

			break;
		}
		
		protTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	
	if((pPortTpToScanTp == NULL)||(protTypeNum >= pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		/* can not find scan type base port type */
		GPN_ALM_ANT_PRINT(GPN_ALM_SCAN_CUP,\
			"%s : no protType(%08x), when notify almSubType(%08x)'(%d) "
			"in port(%08x|%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__, portType, almSubType, almSta, \
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);
		
		gpnLog(GPN_LOG_L_ALERT,\
			"%s : no protType(%08x), when notify almSubType(%08x)'(%d) "
			"in port(%08x|%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__, portType, almSubType, almSta, \
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);

		zlog_err("%s[%d] : no protType(%08x), when notify almSubType(%08x)'(%d) "
			"in port(%08x|%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__, __LINE__, portType, almSubType, almSta, \
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);
		
		return GPN_ALM_GEN_NO;
	}
	
	return GPN_ALM_GEN_YES;
}
/*==================================================*/
/*name :  gpnAlmANtCfmBaseSubTypeProc                                                             */
/*para :                                                                                                                */
/*retu :  void                                                                                                        */
/*desc :  p-process notify alarm base portIndex, almSubType, almOpt ; gpn_alarm     */ 
/*          record alarm status and process shaking.                                                    */
/*          special for CFM alarm                                                                                */
/*==================================================*/
UINT32 gpnAlmANtCfmBaseSubTypeProc(optObjOrient *pPortInfo, UINT32 almSubType, UINT32 almSta)
{
	/* special process for CFM prot, do not use anymore */
#if 0
	optObjOrient protInfo;
	stAlmPeerNode *pAlmPeerPort;

	gpnAlmSeekPeerPortIndexToAlmPeerNode(pPortInfo, &pAlmPeerPort);
	if(pAlmPeerPort == NULL)
	{
		/*check portInfo except rem_mepId*/
		protInfo = *pPortInfo;
		protInfo.portIndex6 = GPN_ALM_API_32_FFFF;
		gpnAlmSeekPeerPortIndexToAlmPeerNode(&protInfo, &pAlmPeerPort);
		if(pAlmPeerPort != NULL)
		{
			gpnAlmVariPortUserNotify((objLogicDesc *)pPortInfo, GPN_ALM_API_ENABLE);
		}
		else
		{
			/*drive report err:err portInfo*/
			GPN_ALM_ANT_PRINT(GPN_ALM_API_CMP, "CFM_ALM_NOTIFY:%08x|%08x|%08x|%08x|%08x|%08x:alm %08x opt %d\n\r",\
				pPortInfo->devIndex,pPortInfo->portIndex,pPortInfo->portIndex3,\
				pPortInfo->portIndex,pPortInfo->portIndex5,pPortInfo->portIndex6,\
				almSubType,almSta);
			return GPN_ALM_API_ERR;
		}
	}
#endif

	return gpnAlmANtBaseSubTypeProc(pPortInfo, almSubType, almSta);
}
/*==============================================*/
/*name :  gpnAlmANtBaseScanTypeProc                                                       */
/*para :                                                                                                      */
/*retu :  void                                                                                               */
/*desc :                                                                                                      */
/*==============================================*/
UINT32 gpnAlmANtBaseScanTypeProc(optObjOrient *pPortInfo, UINT32 *pAlmData, UINT32 len)
{
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stAlmPreScanQuen *pAlmPreScanNode;
	stAlmPeerNode *pAlmPeerPort;
	stAlmScanPortInfo *pAlmScanPort;
	optObjOrient *pstProtInfo;
	UINT32 peerScanNodeNum;
	UINT32 protTypeNum;
	UINT32 portType;
	UINT32 slotId;
	UINT32 hashKey;
	UINT32 tmpAlmData;
	UINT32 i;

	/* get port type from first(master)-portIndex */
	portType = PortIndex_GetType(pPortInfo->portIndex);
	/* is port type supported */
	protTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(protTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		if(pPortTpToScanTp->portType == portType)
		{
			/* get slot hsah key from first(master)-portIndex */
			slotId = PortIndex_GetSlot(pPortInfo->portIndex);
			pAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
			hashKey = (pPortInfo->portIndex)%(pAlmPreScanNode->hashKey);

			/* check if notify port in per scan list, compare with peer port */
			peerScanNodeNum = 0;
			pAlmPeerPort = (stAlmPeerNode *)listFirst(&(pAlmPreScanNode->almPeerPortQuen[hashKey]));
			while((pAlmPeerPort != NULL)&&(peerScanNodeNum < pAlmPreScanNode->peerPortNodeNum[hashKey]))
			{
				pstProtInfo = &(pAlmPeerPort->peerPort);
				/* here devIndex should do what modify */
				if( /*(pstProtInfo->devIndex == pPortInfo->devIndex) &&\*/
					(pstProtInfo->portIndex == pPortInfo->portIndex) &&\
					(pstProtInfo->portIndex3 == pPortInfo->portIndex3) &&\
					(pstProtInfo->portIndex4 == pPortInfo->portIndex4) &&\
					(pstProtInfo->portIndex5 == pPortInfo->portIndex5) &&\
					(pstProtInfo->portIndex6 == pPortInfo->portIndex6) )
				{
					pAlmScanPort = pAlmPeerPort->pAlmScanPort;
					/* is port in valid scan list */
					if(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_ENABLE)
					{
						/* renew alm data, chang sys support bit to alarm support bit */
						gpnAlmSysUseBitPToAlmUseBitP(pAlmPreScanNode->almScanTypeNode, pAlmData, len);
						for(i=0; i<pAlmPreScanNode->almScanTypeNode->almUsePag; i++)
						{
							pAlmData[i] &= pAlmPreScanNode->almScanTypeNode->scanTypeAlmMask[i];
							/* find new chang */
							tmpAlmData = pAlmData[i] ^ pAlmScanPort->iScanAlmDate[i];
							if(tmpAlmData != 0)
							{
								pAlmScanPort->iScanAlmDate[i] = pAlmData[i];
								pAlmScanPort->iFrehAlmMark[i] |= tmpAlmData;
								pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM]++;
							}
		
							/* if port have fresh alarm hapen, then chang port position in alarm scan list */
							if(pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM] != 0)
							{
								/* will do and to do */
							}
						}
					}
					else
					{
						/* port in perScanQuen, but not in validScanQuen */
						gpnLog(GPN_LOG_L_ALERT,\
							"%s : not in valid list, port(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
							__FUNCTION__,\
							pPortInfo->devIndex, pPortInfo->portIndex,\
							pPortInfo->portIndex3, pPortInfo->portIndex4,\
							pPortInfo->portIndex5, pPortInfo->portIndex6);
						
						GPN_ALM_ANT_PRINT(GPN_ALM_SCAN_SVP,\
							"%s : not in valid list, port(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
							__FUNCTION__,\
							pPortInfo->devIndex, pPortInfo->portIndex,\
							pPortInfo->portIndex3, pPortInfo->portIndex4,\
							pPortInfo->portIndex5, pPortInfo->portIndex6);
					}
					break;
				}
				
				peerScanNodeNum++;
				pAlmPeerPort = (stAlmPeerNode *)listNext((NODE *)(pAlmPeerPort));
			}
			
			if((pAlmPeerPort == NULL)||(peerScanNodeNum >= pAlmPreScanNode->peerPortNodeNum[hashKey]))
			{
				/* port type support, but not found port in slot hash list */
				gpnLog(GPN_LOG_L_ALERT,\
					"%s : not found port(%08x|%08x|%08x|%08x|%08x|%08x) "
					"in scanType(%08x)'s slot(%d) pre list\n",\
					__FUNCTION__,\
					pPortInfo->devIndex, pPortInfo->portIndex,\
					pPortInfo->portIndex3, pPortInfo->portIndex4,\
					pPortInfo->portIndex5, pPortInfo->portIndex6,\
					pPortTpToScanTp->ppAlmPreScanNode->pAlmPreScanNode[0].almScanType,\
					slotId);
				
				GPN_ALM_ANT_PRINT(GPN_ALM_SCAN_SVP,\
					"%s : not found port(%08x|%08x|%08x|%08x|%08x|%08x) "
					"in scanType(%08x)'s slot(%d) pre list\n",\
					__FUNCTION__,\
					pPortInfo->devIndex, pPortInfo->portIndex,\
					pPortInfo->portIndex3, pPortInfo->portIndex4,\
					pPortInfo->portIndex5, pPortInfo->portIndex6,\
					pPortTpToScanTp->ppAlmPreScanNode->pAlmPreScanNode[0].almScanType,\
					slotId);
				
				return GPN_ALM_GEN_NO;
			}

			break;
		}
		
		protTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	if((pPortTpToScanTp == NULL)||(protTypeNum >= pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		/* can not find scan type base port type */
		GPN_ALM_ANT_PRINT(GPN_ALM_SCAN_CUP,\
			"%s : no protType(%08x), when notify bit alarm "
			"in port(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__, portType, \
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);
		
		gpnLog(GPN_LOG_L_ALERT,\
			"%s : no protType(%08x), when notify bit alarm "
			"in port(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__, portType, \
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);
		
		return GPN_ALM_GEN_NO;
	}
	return GPN_ALM_GEN_YES;
}
/*==============================================*/
/*name :  gpnAlmANtBaseScanTypeProc                                                       */
/*para :                                                                                                      */
/*retu :  void                                                                                               */
/*desc :                                                                                                      */
/*==============================================*/
UINT32 gpnAlmANtEvtInsertDS(objLogicDesc *pViewPort, stEvtValueRecd *pEvtValueRecd)
{
	stEventAlmDB eventDB;

	/* pEvtValueRecd's index renew at here */
	pEvtValueRecd->index = pgstAlmPreScanWholwInfo->almGlobalCfg.eventIndex++;
	
	eventDB.index = pEvtValueRecd->index;
	eventDB.eventType = pEvtValueRecd->evtSubType;
	eventDB.devIndex = pViewPort->devIndex;
	eventDB.ifIndex = pViewPort->portIndex;
	eventDB.ifIndex2 = pViewPort->portIndex3;
	eventDB.ifIndex3 = pViewPort->portIndex4;
	eventDB.ifIndex4 = pViewPort->portIndex5;
	eventDB.ifIndex5 = pViewPort->portIndex6;
	eventDB.level = pEvtValueRecd->evtRank;
	eventDB.detial = pEvtValueRecd->detial;

	/*todo*/
	eventDB.suffixLen = 0;

	/* pEvtValueRecd's thisTime renew at here */
	/*pEvtValueRecd->thisTime = (UINT32)time(NULL);*/
	
	/* 备盘从主盘获取时间、产生次数等，2018/8/16 */
	if(DEVM_HA_MASTER == alarm_MS_my_status_get())
	{
		pEvtValueRecd->thisTime = (UINT32)pgstAlmPreScanWholwInfo->almTaskTime;
	}
	
	eventDB.thisTime = pEvtValueRecd->thisTime;

	GPN_ALM_ANT_PRINT(GPN_ALM_ANT_CUP, "%s : index(%d) prot(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) detial(%d)\n\r",\
		__FUNCTION__, pEvtValueRecd->index,\
		pViewPort->devIndex, pViewPort->portIndex,\
		pViewPort->portIndex3, pViewPort->portIndex4,\
		pViewPort->portIndex5, pViewPort->portIndex6,\
		pEvtValueRecd->evtSubType, pEvtValueRecd->detial);
	
	gpnAlmDataSheetAdd(GPN_ALM_EVENT_DSHEET, eventDB.index, &eventDB, sizeof(stEventAlmDB));
	
	return GPN_ALM_GEN_YES;
}


/*==================================================*/
/*name :  gpnAlmANtSubEvtStatusRecord                                                              */
/*para :                                                                                                                */
/*retu :  void                                                                                                        */
/*desc :  gpn_alarm  record event status and process event detial                               */ 
/*==================================================*/
UINT32 gpnAlmANtSubEvtStatusRecord(stAlmScanTypeDef *pAlmScanType,
					stAlmPeerNode *pAlmPeerPort, UINT32 evtSubType, UINT32 evtDetial)
{
	stEvtSTCharacterDef *pEvtSubTpStr;
	stAlmScanPortInfo *pAlmScanPort;
	UINT32 evtBitSize;
	UINT32 iEvtBitV;
	UINT32 page;

	pAlmScanPort = pAlmPeerPort->pAlmScanPort;
	/* is port in valid scan list */
	if(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_ENABLE)
	{
		/* find event type node */
		pEvtSubTpStr = NULL;
		gpnAlmSeekEvtSubTpToEvtSubTpBitValue(evtSubType, pAlmScanType, &pEvtSubTpStr);
		if(pEvtSubTpStr != NULL)
		{
			/* screen compiler warning: left shift count >= width of type*/
			evtBitSize = GPN_EVT_PAGE_BIT_SIZE;
			if(evtBitSize == 32)
			{
				page = 0;
			}
			else
			{
				page = pEvtSubTpStr->sysUseIdentify >> evtBitSize;
			}
			iEvtBitV = pEvtSubTpStr->sysUseIdentify & GPN_EVT_IN_PAGE_BITP;

			/*pAlmScanPort->iScanEvtDate[page] |= iEvtBitV;*/
			pAlmScanPort->iFrehEvtMark[page] |= iEvtBitV;
			pAlmScanPort->iFrehEvtMark[GPN_EVT_PAGE_NUM]++;
			/*printf("%s : page %d event Type%08x %08x %08x sysId %08x page %d iEbit %08x evtBitSize %d\n\r",\
				__FUNCTION__, page, evtSubType,\
				pAlmScanPort->iFrehEvtMark[0], pAlmScanPort->iFrehEvtMark[1],\
				pEvtSubTpStr->sysUseIdentify, page, iEvtBitV, evtBitSize);*/
				
			/* detial process ??? add like pAlmScanPort->AlmValueBuff */
			pEvtSubTpStr->detial = evtDetial;

			/* if port have fresh alarm hapen, then chang port position in alarm scan list */
			if(pAlmScanPort->iFrehEvtMark[GPN_ALM_PAGE_NUM] != 0)
			{
				/* will do and to do */
			}

			GPN_EVT_SCAN_PRINT(GPN_EVT_SCAN_CUP,\
				"%s : evtSubType(%08x) detail(%d) perr port(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
				__FUNCTION__, evtSubType, evtDetial,\
				pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
				pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
				pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6);
		}
		else
		{
			zlog_debug(ALARM_DBG_REPORT, "%s : find subTp err, perr port(%08x-%08x|%08x|%08x|%08x|%08x) evtSubType(%08x) detail(%d)\n\r",\
				__FUNCTION__,\
				pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
				pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
				pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6,\
				evtSubType, evtDetial);
			
			gpnLog(GPN_LOG_L_ALERT,\
				"%s : find subTp err, perr port(%08x-%08x|%08x|%08x|%08x|%08x) evtSubType(%08x) detail(%d)\n\r",\
				__FUNCTION__,\
				pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
				pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
				pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6,\
				evtSubType, evtDetial);
			GPN_EVT_SCAN_PRINT((GPN_EVT_SCAN_CUP)|(GPN_EVT_SCAN_UD0),\
				"%s : find subTp err, perr port(%08x-%08x|%08x|%08x|%08x|%08x) evtSubType(%08x) detail(%d)\n\r",\
				__FUNCTION__,\
				pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
				pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
				pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6,\
				evtSubType, evtDetial);
		}
	}
	else
	{
		zlog_debug(ALARM_DBG_REPORT, "%s : port no in valid list, port(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__,\
			pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
			pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
			pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6);
		gpnLog(GPN_LOG_L_ALERT, 
			"%s : port no in valid list, port(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__,\
			pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
			pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
			pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6);
		GPN_EVT_SCAN_PRINT(GPN_ALM_SCAN_CUP,\
			"%s : port no in valid list, port(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__,\
			pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
			pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
			pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6);
	}

	return GPN_ALM_GEN_OK;
}

/*==================================================*/
/*name :  gpnAlmANtEvtBaseSubTypeProc                                                             */
/*para :                                                                                                               */
/*retu :  void                                                                                                       */
/*desc :                                                                                                               */
/*==================================================*/
UINT32 gpnAlmANtEvtBaseSubTypeProc(optObjOrient *pPortInfo, UINT32 evtSubType, UINT32 detial)
{
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stAlmPreScanQuen *pAlmPreScanNode;
	stAlmPeerNode *pAlmPeerPort;
	optObjOrient *pstProtInfo;
	
	UINT32 peerScanNodeNum;
	UINT32 protTypeNum;
	UINT32 portType;
	UINT32 slotId;
	UINT32 hashKey;

	/* get port type from first(master)-portIndex */
	portType = PortIndex_GetType(pPortInfo->portIndex);
	/* is port type supported */
	protTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(protTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		if(pPortTpToScanTp->portType == portType)
		{
			/* get slot hsah key from first(master)-portIndex */
			slotId = PortIndex_GetSlot(pPortInfo->portIndex);

			pAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
			hashKey = (pPortInfo->portIndex)%(pAlmPreScanNode->hashKey);
			
			/* check if notify port in per scan list, compare with peer port */
			peerScanNodeNum = 0;
			pAlmPeerPort = (stAlmPeerNode *)listFirst(&(pAlmPreScanNode->almPeerPortQuen[hashKey]));
			while((pAlmPeerPort != NULL)&&(peerScanNodeNum < pAlmPreScanNode->peerPortNodeNum[hashKey]))
			{
				pstProtInfo = &(pAlmPeerPort->peerPort);
				/* here devIndex should do what modify */
				if( (pstProtInfo->devIndex == pPortInfo->devIndex) &&\
					(pstProtInfo->portIndex == pPortInfo->portIndex) &&\
					(pstProtInfo->portIndex3 == pPortInfo->portIndex3) &&\
					(pstProtInfo->portIndex4 == pPortInfo->portIndex4) &&\
					(pstProtInfo->portIndex5 == pPortInfo->portIndex5) &&\
					(pstProtInfo->portIndex6 == pPortInfo->portIndex6) )
				{
					gpnAlmANtSubEvtStatusRecord(pAlmPreScanNode->almScanTypeNode,
						pAlmPeerPort, evtSubType, detial);
					
					break;
				}
				
				peerScanNodeNum++;
				pAlmPeerPort = (stAlmPeerNode *)listNext((NODE *)(pAlmPeerPort));
			}
			if((pAlmPeerPort == NULL)||(peerScanNodeNum >= pAlmPreScanNode->preScanNodeNum[hashKey]))
			{
				/* port type support, but not found port in slot hash list */
				zlog_debug(ALARM_DBG_REPORT, "%s : when report event(%08x), not found port(%08x-%08x|%08x"
					"|%08x|%08x|%08x) in scanType(%08x)'s slot(%d) pre list\n",\
					__FUNCTION__, evtSubType,\
					pPortInfo->devIndex, pPortInfo->portIndex,\
					pPortInfo->portIndex3, pPortInfo->portIndex4,\
					pPortInfo->portIndex5, pPortInfo->portIndex6,\
					pPortTpToScanTp->ppAlmPreScanNode->pAlmPreScanNode[0].almScanType,\
					slotId);
				gpnLog(GPN_LOG_L_ALERT,\
					"%s : when report event(%08x), not found port(%08x-%08x|%08x"
					"|%08x|%08x|%08x) in scanType(%08x)'s slot(%d) pre list\n",\
					__FUNCTION__, evtSubType,\
					pPortInfo->devIndex, pPortInfo->portIndex,\
					pPortInfo->portIndex3, pPortInfo->portIndex4,\
					pPortInfo->portIndex5, pPortInfo->portIndex6,\
					pPortTpToScanTp->ppAlmPreScanNode->pAlmPreScanNode[0].almScanType,\
					slotId);
				
				GPN_ALM_ANT_PRINT(GPN_ALM_SCAN_CUP,\
					"%s : when report event(%08x), not found port(%08x-%08x|%08x"
					"|%08x|%08x|%08x) in scanType(%08x)'s slot(%d) pre list\n",\
					__FUNCTION__, evtSubType,\
					pPortInfo->devIndex, pPortInfo->portIndex,\
					pPortInfo->portIndex3, pPortInfo->portIndex4,\
					pPortInfo->portIndex5, pPortInfo->portIndex6,\
					pPortTpToScanTp->ppAlmPreScanNode->pAlmPreScanNode[0].almScanType,\
					slotId);
				return GPN_ALM_GEN_NO;
			}

			break;
		}
		
		protTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	
	if((pPortTpToScanTp == NULL)||(protTypeNum >= pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		/* port type not support */
		gpnLog(GPN_LOG_L_ALERT,\
			"%s : when report event(%08x), portType(%08x) not "
			"support for port(%08x-%08x|%08x|%08x|%08x|%08x)\n",\
			__FUNCTION__, evtSubType, portType,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);
		
		GPN_ALM_ANT_PRINT(GPN_ALM_SCAN_SVP,\
			"%s : when report event(%08x), portType(%08x) not "
			"support for port(%08x-%08x|%08x|%08x|%08x|%08x)\n",\
			__FUNCTION__, evtSubType, portType,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);
		
		return GPN_ALM_GEN_NO;
	}
	
	return GPN_ALM_GEN_YES;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif/* _GPN_ALM_ALM_NOTIFY_C_ */

