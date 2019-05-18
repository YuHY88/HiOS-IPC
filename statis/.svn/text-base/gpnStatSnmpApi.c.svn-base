/**********************************************************
* file name: gpnStatSnmpApi.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-25
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_SNMP_API_C_
#define _GPN_STAT_SNMP_API_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockStatMsgDef.h"

#include "gpnStatDataStruct.h"
#include "gpnStatFuncApi.h"
#include "gpnStatSnmpApi.h"
#include "devCoreInfo/gpnDevCoreInfoFunApi.h"


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

/*=============================================*/
/*when get first in XXX_GET_NEXT, use 0xFFFFFFFF(-1) for get start             */
/*=============================================*/

UINT32 gpnStatAgentXIoCtrl(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 reVal;
	
	/* assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : msgType(%08x)\n\r",\
		__FUNCTION__, pgpnStatMsgSpIn->iMsgType);
	
	reVal = GPN_STAT_SNMP_GEN_OK;
	switch(pgpnStatMsgSpIn->iMsgType)
	{	
		/*************************** Scalar GET ***************************/
		case GPN_STAT_MSG_TIME_POLARIT_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_TIME_POLARIT_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatTimePolarityGet();
			break;
			
		case GPN_STAT_MSG_TIME_OFFSET_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_TIME_OFFSET_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatTimeOffsetGet();
			break;
			
		case GPN_STAT_MSG_MAX_OBJ_SRC_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_MAX_OBJ_SRC_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpOut->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatMaxMonObjGet();
			break;
			
		case GPN_STAT_MSG_USE_OBJ_SRC_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_USE_OBJ_SRC_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatRunMonObjGet();
			break;
			
		case GPN_STAT_MSG_LAST_DATA_REPT_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_LAST_DATA_REPT_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatLastHistReportCfgGet();
			break;

		case GPN_STAT_MSG_OLD_DATA_REPT_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_OLD_DATA_REPT_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatOldHistReportCfgGet();
			break;
			
		case GPN_STAT_MSG_ALARM_REPT_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_ALARM_REPT_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatAlarmReportCfgGet();
			break;

		case GPN_STAT_MSG_EVENT_REPT_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_EVENT_REPT_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatEventReportCfgGet();
			break;
			
		case GPN_STAT_MSG_TASK_ID_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_TASK_ID_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatTaskIdGet();
			break;

		case GPN_STAT_MSG_MAX_TASK_NUM_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_MAX_TASK_NUM_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatMaxTaskNumGet();
			break;

		case GPN_STAT_MSG_RUN_TASK_NUM_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_RUN_TASK_NUM_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatRunTaskNumGet();
			break;
		
		case GPN_STAT_MSG_EVN_THRED_TP_ID_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_EVN_THRED_TP_ID_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatEventThredTemplatIdGet();
			break;

		case GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatMaxEventThredTpNumGet();
			break;

		case GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatRunEventThredTpNumGet();
			break;

		case GPN_STAT_MSG_ALM_THRED_TP_ID_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_ALM_THRED_TP_ID_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatAlarmThredTemplatIdGet();
			break;

		case GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatMaxAlarmThredTpNumGet();
			break;

		case GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatRunAlarmThredTpNumGet();
			break;
		
		case GPN_STAT_MSG_SUB_FILT_TP_ID_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_SUB_FILT_TP_ID_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatSubTypeFiltIdGet();
			break;

		case GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatMaxSubFiltTpNumGet();
			break;
			
		case GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatRunSubFiltTpNumGet();
			break;

		case GPN_STAT_MSG_SUB_REPORT_TP_ID_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_SUB_REPORT_TP_ID_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			/*geqian 2015.9.11*/
			//pgpnStatMsgSpOut->iMsgPara1 = gpnStatSubTypeFiltIdGet();
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatSubTypeReportIdGet();
			break;

		case GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			/*geqian 2015.9.11*/
			//pgpnStatMsgSpOut->iMsgPara1 = gpnStatMaxSubFiltTpNumGet();
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatMaxSubReportTpNumGet();
			break;
			
		case GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			/*geqian 2015.9.11*/
			//pgpnStatMsgSpOut->iMsgPara1 = gpnStatRunSubFiltTpNumGet();
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatRunSubReportTpNumGet();
			break;
			
		case GPN_STAT_MSG_COUNT_TYPE_GET:
			pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
			pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_COUNT_TYPE_GET_RSP;
			pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
			pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
			pgpnStatMsgSpOut->iMsgPara1 = gpnStatCountTypeGet();
			break;
			
		/*************************** Table GET ***************************/
		case GPN_STAT_MSG_STAT_TYPE_GET:
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : msgType(%08x) scanType(%08x)\n\r",\
				__FUNCTION__, pgpnStatMsgSpIn->iMsgType, pgpnStatMsgSpIn->iMsgPara1);
			reVal = gpnStatSnmpApiStatScanTypeGet(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_STAT_TYPE_GET_NEXT:
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : msgType(%08x) scanType(%08x)\n\r",\
				__FUNCTION__, pgpnStatMsgSpIn->iMsgType, pgpnStatMsgSpIn->iMsgPara1);
			reVal = gpnStatSnmpApiStatScanTypeGetNext(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_SUB_STAT_TYPE_GET:
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : msgType(%08x) subStatType(%08x)\n\r",\
				__FUNCTION__, pgpnStatMsgSpIn->iMsgType, pgpnStatMsgSpIn->iMsgPara1);
			reVal = gpnStatSnmpApiSubStatTypeGet(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT:
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : msgType(%08x) subStatType(%08x)\n\r",\
				__FUNCTION__, pgpnStatMsgSpIn->iMsgType, pgpnStatMsgSpIn->iMsgPara1);
			reVal = gpnStatSnmpApiSubStatTypeGetNext(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;

		case GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET:
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : msgType(%08x) portType(%08x)\n\r",\
				__FUNCTION__, pgpnStatMsgSpIn->iMsgType, pgpnStatMsgSpIn->iMsgPara1);
			reVal = gpnStatSnmpApiPortType2ScanTypeGet(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT:
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : msgType(%08x) portType(%08x)\n\r",\
				__FUNCTION__, pgpnStatMsgSpIn->iMsgType, pgpnStatMsgSpIn->iMsgPara1);
			reVal = gpnStatSnmpApiPortType2ScanTypeGetNext(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_TASK_GET:
			reVal = gpnStatSnmpApiTaskGet(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_TASK_GET_NEXT:
			reVal = gpnStatSnmpApiTaskGetNext(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_EVN_THRED_TEMP_GET:
			reVal = gpnStatSnmpApiEventThredTpGet(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT:
			reVal = gpnStatSnmpApiEventThredTpGetNext(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;

		case GPN_STAT_MSG_ALM_THRED_TEMP_GET:
			reVal = gpnStatSnmpApiAlarmThredTpGet(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT:
			reVal = gpnStatSnmpApiAlarmThredTpGetNext(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_SUBFILT_TEMP_GET:
			reVal = gpnStatSnmpApiSubFiltTpGet(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT:
			reVal = gpnStatSnmpApiSubFiltTpGetNext(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;

		case GPN_STAT_MSG_SUBREPORT_TEMP_GET:
			reVal = gpnStatSnmpApiSubReportTpGet(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT:
			reVal = gpnStatSnmpApiSubReportTpGetNext(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;

		case GPN_STAT_MSG_MON_GET:
			reVal = gpnStatSnmpApiPortStatMonCfgGet(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_MON_GET_NEXT:
			reVal = gpnStatSnmpApiPortStatMonCfgGetNext(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;

		case GPN_STAT_MSG_CURR_DATA_GET:
			reVal = gpnStatSnmpApiCurrDataBaseSubTypeGet(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;
			
		case GPN_STAT_MSG_CURR_DATA_GET_NEXT:
			reVal = gpnStatSnmpApiCurrDataBaseSubTypeGetNext(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;

		case GPN_STAT_MSG_HIST_DATA_GET:
			reVal = gpnStatSnmpApiHistDataBaseSubTypeGet(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;

		case GPN_STAT_MSG_HIST_DATA_GET_NEXT:
			reVal = gpnStatSnmpApiHistDataBaseSubTypeGetNext(pgpnStatMsgSpIn, inLen, pgpnStatMsgSpOut, outLen);
			break;

		case GPN_STAT_MSG_HIST_TRAP_DATA_GET:
			break;

		case GPN_STAT_MSG_HIST_TRAP_DATA_GET_NEXT:
			break;
/***************                                 CFG                                 ********************/

		case GPN_STAT_MSG_LAST_DATA_REPT_SET:	
		case GPN_STAT_MSG_OLD_DATA_REPT_SET:
		case GPN_STAT_MSG_ALARM_REPT_SET:
		case GPN_STAT_MSG_EVENT_REPT_SET:

		case GPN_STAT_MSG_SUB_STAT_TYPE_MODIFY:
		case GPN_STAT_MSG_CURR_DATA_MODIFY:	

		case GPN_STAT_MSG_TASK_ADD:
		case GPN_STAT_MSG_TASK_MODIFY:
		case GPN_STAT_MSG_TASK_DELETE:
			
		case GPN_STAT_MSG_EVN_THRED_TEMP_ADD:
		case GPN_STAT_MSG_EVN_THRED_TEMP_MODIFY:
		case GPN_STAT_MSG_EVN_THRED_TEMP_DELETE:
			
		case GPN_STAT_MSG_SUBFILT_TEMP_ADD:
		case GPN_STAT_MSG_SUBFILT_TEMP_MODIFY:
		case GPN_STAT_MSG_SUBFILT_TEMP_DELETE:
			
		case GPN_STAT_MSG_ALM_THRED_TEMP_ADD:
		case GPN_STAT_MSG_ALM_THRED_TEMP_MODIFY:
		case GPN_STAT_MSG_ALM_THRED_TEMP_DELETE:
			
		case GPN_STAT_MSG_SUBREPORT_TEMP_ADD:
		case GPN_STAT_MSG_SUBREPORT_TEMP_MODIFY:
		case GPN_STAT_MSG_SUBREPORT_TEMP_DELETE:

		case GPN_STAT_MSG_MON_ADD:
		case GPN_STAT_MSG_MON_DELETE:
		case GPN_STAT_MSG_BASE_MON_OPT:
		case GPN_STAT_MSG_PORT_CYC_MON_OPT:
		case GPN_STAT_MSG_TASK_ALM_THREDTP_CFG:	
			/* use normal msg process API */
			reVal = gpnStatCorePorc(pgpnStatMsgSpIn, inLen);
			break;

		default:
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : iMsgType(%08x) no define\n\r",\
				__FUNCTION__, pgpnStatMsgSpIn->iMsgType);
			break;
	}
	
	return reVal;
}

/*FUNCTION SUPPORT FUNCIONT */
UINT32 gpnStatSnmpApiStatScanTypeGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 statScanType;
	stStatScanTypeDef *pscanTpNode;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	/*this is scan type */
	statScanType = pgpnStatMsgSpIn->iMsgPara1;

	/*valid stat scan type */
	if(statScanType == GPN_STAT_SCAN_TYPE_ALL)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : valid scanType(%08x), err!\n\r",\
			__FUNCTION__, statScanType);
		return GPN_STAT_SNMP_GEN_ERR;
	}

	/*get scan type info */
	pscanTpNode = NULL;
	if(gpnStatTypeGetScanTypeNode(statScanType, &pscanTpNode) != GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : scanType(%08x) to node err!\n\r",\
			__FUNCTION__, statScanType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	if(pscanTpNode == NULL)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : scanType(%08x) to node is NULL\n\r",\
			__FUNCTION__, statScanType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : scanType(%08x) find node(%08x)\n\r",\
		__FUNCTION__, pscanTpNode->statScanType, (UINT32)pscanTpNode);
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_STAT_TYPE_GET_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = pscanTpNode->statScanType;
	pgpnStatMsgSpOut->iMsgPara2 = pscanTpNode->longCycTimes;
	pgpnStatMsgSpOut->iMsgPara3 = pscanTpNode->shortCycTimes;
	pgpnStatMsgSpOut->iMsgPara4 = pscanTpNode->statSubTpNumInScanType;

	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiStatScanTypeGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 statScanType;
	stStatScanTypeDef *pscanTpNode;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	/*this scan type */
	statScanType = pgpnStatMsgSpIn->iMsgPara1;

	//printf ("%s[%d] : statScanType = %d:%d\n", __func__, __LINE__,
	//	statScanType, GPN_STAT_SCAN_TYPE_ALL);

	pscanTpNode = NULL;
	if(statScanType == GPN_STAT_SCAN_TYPE_ALL)
	{
		/*get first stat scan type */
		gpnStatTypeGetFirstScanTypeNode(&pscanTpNode);
	}
	else
	{
		gpnStatTypeGetScanTypeNode(statScanType, &pscanTpNode);
	}
	//printf("%s[%d] : pscanTpNode = %p\n", __func__, __LINE__, pscanTpNode);
	if(pscanTpNode == NULL)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : scanType(%08x) can not find node\n\r",\
			__FUNCTION__, statScanType);
		return GPN_STAT_SNMP_GEN_ERR;
	}

	GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : scanType(%08x)(%s) find node(%08x)\n\r",\
		__FUNCTION__, pscanTpNode->statScanType, pscanTpNode->statScanTypeName, (UINT32)pscanTpNode);
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_STAT_TYPE_GET_NEXT_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = pscanTpNode->statScanType;
	pgpnStatMsgSpOut->iMsgPara2 = pscanTpNode->longCycTimes;
	pgpnStatMsgSpOut->iMsgPara3 = pscanTpNode->shortCycTimes;
	pgpnStatMsgSpOut->iMsgPara4 = pscanTpNode->statSubTpNumInScanType;
	pgpnStatMsgSpOut->iMsgPara5 = pscanTpNode->subNumType;

	/*get next stat scan type */
	if(statScanType == GPN_STAT_SCAN_TYPE_ALL)
	{
		pgpnStatMsgSpOut->iMsgPara6 = pscanTpNode->statScanType;
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : next scanType(%08x)(%s)\n\r",\
			__FUNCTION__, pscanTpNode->statScanType, pscanTpNode->statScanTypeName);
	}
	else
	{
		pscanTpNode = (stStatScanTypeDef *)listNext((NODE *)pscanTpNode);
		if(pscanTpNode != NULL)
		{
			pgpnStatMsgSpOut->iMsgPara6 = pscanTpNode->statScanType;
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : next scanType(%08x)(%s)\n\r",\
				__FUNCTION__, pscanTpNode->statScanType, pscanTpNode->statScanTypeName);
		}
		else
		{
			pgpnStatMsgSpOut->iMsgPara6 = GPN_STAT_SCAN_TYPE_ALL;
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : next scanType is NULL\n\r",\
				__FUNCTION__);
		}
	}
	return GPN_STAT_SNMP_GEN_OK;
}
UINT32 gpnStatSnmpApiSubStatTypeGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 subStatType;
	UINT32 statScanType;
	stStatSubTpNode *pstatSubTpNode;
	stStatSTCharacterDef *psubStatTpDef;
	stSubTypeBoundAlm *psubTpBoundAlm;
	stSubTypeThred *subTypeThred;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	/*out msg should have enough space */
	if(pgpnStatMsgSpOut->msgCellLen < (sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred)))
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : subStatType(%08x) have (%d) < (%d)\n\r",\
			__FUNCTION__, pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->msgCellLen,\
			sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred));
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	/*this sub stat type */
	subStatType = pgpnStatMsgSpIn->iMsgPara1;

	/*valid stat scan type */
	if(subStatType == GPN_STAT_TYPE_INVALID)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : subStatType(%08x), can not use in GET!\n\r",\
			__FUNCTION__, subStatType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	/*get scan type info */
	pstatSubTpNode = NULL;
	if(gpnStatTypeGetSubStatTypeNode(subStatType, &pstatSubTpNode) != GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : subStatType(%08x) to node err!\n\r",\
			__FUNCTION__, subStatType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	if(pstatSubTpNode == NULL)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : subStatType(%08x) to node is NULL!\n\r",\
			__FUNCTION__, subStatType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	psubStatTpDef = pstatSubTpNode->pStatSubTpSt;

	GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : subStatType(%08x) find node(%08x)\n\r",\
		__FUNCTION__, psubStatTpDef->statSubType, (UINT32)psubStatTpDef);
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_SUB_STAT_TYPE_GET_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = subStatType;
	/* sub type belong scan type */
	gpnStatTypeSubStatType2ScanType(subStatType, &statScanType);
	pgpnStatMsgSpOut->iMsgPara2 = statScanType;
	
	pgpnStatMsgSpOut->iMsgPara3 = (UINT32)(psubStatTpDef->countType);
	pgpnStatMsgSpOut->iMsgPara4 = (UINT32)(psubStatTpDef->dataBitDeep);

	psubTpBoundAlm = (stSubTypeBoundAlm *)(&(pgpnStatMsgSpOut->msgCellLen) + 1);
	subTypeThred = (stSubTypeThred *)((UINT8 *)psubTpBoundAlm + sizeof(stSubTypeBoundAlm));

	psubTpBoundAlm->longCycUpAlm = psubStatTpDef->longCycUpThAlm;
	psubTpBoundAlm->longCycLowAlm = psubStatTpDef->longCycDnThAlm;
	psubTpBoundAlm->longCycUpEvt = psubStatTpDef->longCycUpThEvt;
	psubTpBoundAlm->longCycLowEvt = psubStatTpDef->longCycDnThEvt;
	psubTpBoundAlm->shortCycUpAlm = psubStatTpDef->shortCycUpThAlm;
	psubTpBoundAlm->shortCycLowAlm = psubStatTpDef->shortCycDnThAlm;
	psubTpBoundAlm->shortCycUpEvt = psubStatTpDef->shortCycUpThEvt;
	psubTpBoundAlm->shortCycLowEvt = psubStatTpDef->shortCycDnThEvt;
	psubTpBoundAlm->userDefCycUpAlm = psubStatTpDef->userDefCycUpThAlm;
	psubTpBoundAlm->userDefCycLowAlm = psubStatTpDef->userDefCycDnThAlm;
	psubTpBoundAlm->userDefCycUpEvt = psubStatTpDef->userDefCycUpThEvt;
	psubTpBoundAlm->userDefCycLowEvt = psubStatTpDef->userDefCycDnThEvt;

	subTypeThred->longCycUpThredHigh32 = psubStatTpDef->longCycUpThrdH32;
	subTypeThred->longCycUpThredLow32 = psubStatTpDef->longCycUpThrdL32;
	subTypeThred->longCycDnThredHigh32 = psubStatTpDef->longCycDnThrdH32;
	subTypeThred->longCycDnThredLow32 = psubStatTpDef->longCycDnThrdL32;
	subTypeThred->shortCycUpThredHigh32 = psubStatTpDef->shortCycUpThrdH32;
	subTypeThred->shortCycUpThredLow32 = psubStatTpDef->shortCycUpThrdL32;
	subTypeThred->shortCycDnThredHigh32 = psubStatTpDef->shortCycDnThrdH32;
	subTypeThred->shortCycDnThredLow32 = psubStatTpDef->shortCycDnThrdL32;

	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiSubStatTypeGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 subStatType;
	UINT32 statScanType;
	stStatSubTpNode *psubStatTpNode;
	stStatSTCharacterDef *psubStatTpDef;
	stSubTypeBoundAlm *psubTpBoundAlm;
	stSubTypeThred *subTypeThred;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	/*out msg should have enough space */
	if(pgpnStatMsgSpOut->msgCellLen < (sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred)))
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : subStatType(%08x) have (%d) < (%d)\n\r",\
			__FUNCTION__, pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->msgCellLen,\
			sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred));
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	/*get scan type info */
	psubStatTpNode = NULL;
	/*this sub stat type */
	subStatType = pgpnStatMsgSpIn->iMsgPara1;
	if(subStatType == GPN_STAT_TYPE_INVALID)
	{
		/*get first sub stat type */
		gpnStatTypeGetFirstSubStatTypeNode(&psubStatTpNode);
	}
	else
	{
		gpnStatTypeGetSubStatTypeNode(subStatType, &psubStatTpNode);
	}
	if(psubStatTpNode == NULL)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : subStatType(%08x) find node NULL\n\r",\
			__FUNCTION__, subStatType);
		return GPN_STAT_SNMP_GEN_ERR;
	}

	/*get this sub stat type def */
	psubStatTpDef = psubStatTpNode->pStatSubTpSt;
	GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : subStatType(%08x)(%s) find node(%08x)\n\r",\
		__FUNCTION__, psubStatTpDef->statSubType, psubStatTpDef->subStatName, (UINT32)psubStatTpDef);
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = psubStatTpDef->statSubType;
	
	/* sub type belong scan type */
	gpnStatTypeSubStatType2ScanType(psubStatTpDef->statSubType, &statScanType);
	pgpnStatMsgSpOut->iMsgPara2 = statScanType;
	
	pgpnStatMsgSpOut->iMsgPara3 = (UINT32)(psubStatTpDef->countType);
	pgpnStatMsgSpOut->iMsgPara4 = (UINT32)(psubStatTpDef->dataBitDeep);
	
	psubTpBoundAlm = (stSubTypeBoundAlm *)(&(pgpnStatMsgSpOut->msgCellLen) + 1);
	subTypeThred = (stSubTypeThred *)((UINT8 *)psubTpBoundAlm + sizeof(stSubTypeBoundAlm));

	psubTpBoundAlm->longCycUpAlm = psubStatTpDef->longCycUpThAlm;
	psubTpBoundAlm->longCycLowAlm = psubStatTpDef->longCycDnThAlm;
	psubTpBoundAlm->longCycUpEvt = psubStatTpDef->longCycUpThEvt;
	psubTpBoundAlm->longCycLowEvt = psubStatTpDef->longCycDnThEvt;
	psubTpBoundAlm->shortCycUpAlm = psubStatTpDef->shortCycUpThAlm;
	psubTpBoundAlm->shortCycLowAlm = psubStatTpDef->shortCycDnThAlm;
	psubTpBoundAlm->shortCycUpEvt = psubStatTpDef->shortCycUpThEvt;
	psubTpBoundAlm->shortCycLowEvt = psubStatTpDef->shortCycDnThEvt;
	psubTpBoundAlm->userDefCycUpAlm = psubStatTpDef->userDefCycUpThAlm;
	psubTpBoundAlm->userDefCycLowAlm = psubStatTpDef->userDefCycDnThAlm;
	psubTpBoundAlm->userDefCycUpEvt = psubStatTpDef->userDefCycUpThEvt;
	psubTpBoundAlm->userDefCycLowEvt = psubStatTpDef->userDefCycDnThEvt;

	subTypeThred->longCycUpThredHigh32 = psubStatTpDef->longCycUpThrdH32;
	subTypeThred->longCycUpThredLow32 = psubStatTpDef->longCycUpThrdL32;
	subTypeThred->longCycDnThredHigh32 = psubStatTpDef->longCycDnThrdH32;
	subTypeThred->longCycDnThredLow32 = psubStatTpDef->longCycDnThrdL32;
	subTypeThred->shortCycUpThredHigh32 = psubStatTpDef->shortCycUpThrdH32;
	subTypeThred->shortCycUpThredLow32 = psubStatTpDef->shortCycUpThrdL32;
	subTypeThred->shortCycDnThredHigh32 = psubStatTpDef->shortCycDnThrdH32;
	subTypeThred->shortCycDnThredLow32 = psubStatTpDef->shortCycDnThrdL32;

	/*get next sub stat type */
	if(subStatType == GPN_STAT_TYPE_INVALID)
	{
		pgpnStatMsgSpOut->iMsgPara5 = psubStatTpDef->statSubType;
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : next subStatType(%08x)(%s)\n\r",\
			__FUNCTION__, psubStatTpDef->statSubType, psubStatTpDef->subStatName);
	}
	else
	{
		psubStatTpNode = (stStatSubTpNode *)listNext((NODE *)psubStatTpNode);
		if(psubStatTpNode != NULL)
		{
			psubStatTpDef = psubStatTpNode->pStatSubTpSt;
			pgpnStatMsgSpOut->iMsgPara5 = psubStatTpDef->statSubType;
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : next subStatType(%08x)(%s)\n\r",\
				__FUNCTION__, psubStatTpDef->statSubType, psubStatTpDef->subStatName);
		}
		else
		{
			pgpnStatMsgSpOut->iMsgPara5 = GPN_STAT_TYPE_INVALID;
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : next subStatType is NULL\n\r",\
				__FUNCTION__);
		}
	}

	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiPortType2ScanTypeGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 portType;
	stPortTpToStatScanTp *pPortTpToScanTp;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	/*this port type */
	portType = pgpnStatMsgSpIn->iMsgPara1;
	/*we define FUN_ETH_PORT_TYPE = 0, so 0 is not a invalid value ... bagu ... */
	if(portType == IFM_PORT_TYPE_INVALID)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : portType(%08x) is invalid!\n\r",\
			__FUNCTION__, portType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	/*get port type info */
	pPortTpToScanTp = NULL;
	gpnStatSeekPortType2PTVsST(portType, &pPortTpToScanTp);
	if(pPortTpToScanTp == NULL)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : portType(%08x) find node NULL\n\r",\
			__FUNCTION__, portType);
		return GPN_STAT_SNMP_GEN_ERR;
	}

	/*get this port type def */
	GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : portType(%08x)(%s) find node(%08x)\n\r",\
		__FUNCTION__, pPortTpToScanTp->portType, pPortTpToScanTp->portTpName, (UINT32)pPortTpToScanTp);
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = pPortTpToScanTp->portType;
	/*now 1 port type to 1 stat scan type */
	pgpnStatMsgSpOut->iMsgPara2 = 1;
	pgpnStatMsgSpOut->iMsgPara3 = pPortTpToScanTp->statScanType;
	pgpnStatMsgSpOut->iMsgPara4 = GPN_STAT_SCAN_TYPE_ALL;
	pgpnStatMsgSpOut->iMsgPara5 = GPN_STAT_SCAN_TYPE_ALL;
	pgpnStatMsgSpOut->iMsgPara6 = GPN_STAT_SCAN_TYPE_ALL;
	pgpnStatMsgSpOut->iMsgPara7 = GPN_STAT_SCAN_TYPE_ALL;
	
	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiPortType2ScanTypeGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 portType;
	stPortTpToStatScanTp *pPortTpToScanTp;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	/*get port type info */
	pPortTpToScanTp = NULL;
	/*this port type */
	portType = pgpnStatMsgSpIn->iMsgPara1;
	/*we define FUN_ETH_PORT_TYPE = 0, so 0 is not a invalid value ... bagu ... */
	if(portType == IFM_PORT_TYPE_INVALID)
	{
		/*get first sub stat type */
		gpnStatSeekFirstPortType2PTVsST(&pPortTpToScanTp);
	}
	else
	{
		gpnStatSeekPortType2PTVsST(portType, &pPortTpToScanTp);
	}
	if(pPortTpToScanTp == NULL)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : portType(%08x) find node NULL\n\r",\
			__FUNCTION__, portType);
		return GPN_STAT_SNMP_GEN_ERR;
	}

	/*get this port type def */
	GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : portType(%08x)(%s) find node(%08x)\n\r",\
		__FUNCTION__, pPortTpToScanTp->portType, pPortTpToScanTp->portTpName, (UINT32)pPortTpToScanTp);
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = pPortTpToScanTp->portType;
	/*now 1 port type to 1 stat scan type */
	pgpnStatMsgSpOut->iMsgPara2 = 1;
	pgpnStatMsgSpOut->iMsgPara3 = pPortTpToScanTp->statScanType;
	pgpnStatMsgSpOut->iMsgPara4 = GPN_STAT_SCAN_TYPE_ALL;
	pgpnStatMsgSpOut->iMsgPara5 = GPN_STAT_SCAN_TYPE_ALL;
	pgpnStatMsgSpOut->iMsgPara6 = GPN_STAT_SCAN_TYPE_ALL;
	pgpnStatMsgSpOut->iMsgPara7 = GPN_STAT_SCAN_TYPE_ALL;
	
	/*get next port type : if first search(portType is 0), next is first portType */
	if(portType == IFM_PORT_TYPE_INVALID)
	{
		pgpnStatMsgSpOut->iMsgPara8 = pPortTpToScanTp->portType;
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : next portType(%08x)(%s)\n\r",\
			__FUNCTION__, pPortTpToScanTp->portType, pPortTpToScanTp->portTpName);
	}
	else
	{
		pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)pPortTpToScanTp);
		if(pPortTpToScanTp != NULL)
		{
			pgpnStatMsgSpOut->iMsgPara8 = pPortTpToScanTp->portType;
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : next portType(%08x)(%s)\n\r",\
				__FUNCTION__, pPortTpToScanTp->portType, pPortTpToScanTp->portTpName);
		}
		else
		{
			pgpnStatMsgSpOut->iMsgPara8 = IFM_PORT_TYPE_INVALID;
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : next portType is NULL\n\r",\
				__FUNCTION__);
		}
	}
	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiTaskGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 taskId;
	stStatTaskNode *ptaskNode;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_TASK_GET_RSP;
	/*this task id */
	pgpnStatMsgSpOut->iMsgPara1 = 0;
	
	taskId = pgpnStatMsgSpIn->iMsgPara1;
	ptaskNode = NULL;
	if(gpnStatTaskGet(taskId, &ptaskNode) == GPN_STAT_SNMP_GEN_OK)
	{
		if(ptaskNode != NULL)
		{
			pgpnStatMsgSpOut->iMsgPara1 = ptaskNode->taskId;
			pgpnStatMsgSpOut->iMsgPara2 = ptaskNode->statScanTp;
			pgpnStatMsgSpOut->iMsgPara3 = ptaskNode->maxPortInTask;
			pgpnStatMsgSpOut->iMsgPara4 = ptaskNode->cycClass;
			pgpnStatMsgSpOut->iMsgPara5 = ptaskNode->cycSeconds;
			pgpnStatMsgSpOut->iMsgPara6 = ptaskNode->monStartTime;
			pgpnStatMsgSpOut->iMsgPara7 = ptaskNode->monEndTime;
			pgpnStatMsgSpOut->iMsgPara8 = ptaskNode->almThredTpId;
			pgpnStatMsgSpOut->iMsgPara9 = ptaskNode->subReportTpId;
			pgpnStatMsgSpOut->iMsgParaA = ptaskNode->actMOnOBjNum;
		}
		return GPN_STAT_SNMP_GEN_OK;
	}
	else
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}
}

UINT32 gpnStatSnmpApiTaskGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 taskId;
	UINT32 nextTaskId;
	stStatTaskNode *ptaskNode;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_TASK_GET_NEXT_RSP;
	/*this task id */
	pgpnStatMsgSpOut->iMsgPara1 = 0;
	/*next task id */
	pgpnStatMsgSpOut->iMsgPara9 = 0;

	taskId = pgpnStatMsgSpIn->iMsgPara1;
	nextTaskId = 0;
	ptaskNode = NULL;
	if(gpnStatTaskGetNext(taskId, &ptaskNode, &nextTaskId) == GPN_STAT_SNMP_GEN_OK)
	{
		if(ptaskNode != NULL)
		{
			pgpnStatMsgSpOut->iMsgPara1 = ptaskNode->taskId;
			pgpnStatMsgSpOut->iMsgPara2 = ptaskNode->statScanTp;
			pgpnStatMsgSpOut->iMsgPara3 = ptaskNode->actMOnOBjNum;
			pgpnStatMsgSpOut->iMsgPara4 = ptaskNode->cycClass;
			pgpnStatMsgSpOut->iMsgPara5 = ptaskNode->cycSeconds;
			pgpnStatMsgSpOut->iMsgPara6 = ptaskNode->monStartTime;
			pgpnStatMsgSpOut->iMsgPara7 = ptaskNode->monEndTime;
			pgpnStatMsgSpOut->iMsgPara8 = ptaskNode->almThredTpId;
			pgpnStatMsgSpOut->iMsgPara9 = ptaskNode->subReportTpId;

			pgpnStatMsgSpOut->iMsgParaB = ptaskNode->maxPortInTask;	//lipf add
		}
		pgpnStatMsgSpOut->iMsgParaA = nextTaskId;
		
		return GPN_STAT_SNMP_GEN_OK;
	}
	else
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}
}

UINT32 gpnStatSnmpApiTaskGetNext_2(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 taskId;
	stStatTaskNode *ptaskNode;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_TASK_GET_NEXT_RSP;

	taskId = pgpnStatMsgSpIn->iMsgPara1;
	ptaskNode = NULL;
	if(gpnStatTaskGetNext_2(taskId, &ptaskNode) == GPN_STAT_SNMP_GEN_OK)
	{
		if(ptaskNode != NULL)
		{
			pgpnStatMsgSpOut->iMsgPara1 = ptaskNode->taskId;
			pgpnStatMsgSpOut->iMsgPara2 = ptaskNode->statScanTp;
			pgpnStatMsgSpOut->iMsgPara3 = ptaskNode->actMOnOBjNum;
			pgpnStatMsgSpOut->iMsgPara4 = ptaskNode->cycClass;
			pgpnStatMsgSpOut->iMsgPara5 = ptaskNode->cycSeconds;
			pgpnStatMsgSpOut->iMsgPara6 = ptaskNode->monStartTime;
			pgpnStatMsgSpOut->iMsgPara7 = ptaskNode->monEndTime;
			pgpnStatMsgSpOut->iMsgPara8 = ptaskNode->almThredTpId;
			pgpnStatMsgSpOut->iMsgPara9 = ptaskNode->subReportTpId;
			pgpnStatMsgSpOut->iMsgParaA = ptaskNode->maxPortInTask;
		}
		
		return GPN_STAT_SNMP_GEN_OK;
	}
	else
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}
}


UINT32 gpnStatSnmpApiEventThredTpGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 thredTpId;	
	UINT32 subType;
	stStatEvnThredTpNode *pthredTpNode;
	stStatEvnThredTpTable evnThredTpLine;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	thredTpId = pgpnStatMsgSpIn->iMsgPara1;
	subType = pgpnStatMsgSpIn->iMsgPara2;
	
	pthredTpNode = NULL;
	gpnStatEventThredTpGet(thredTpId, &pthredTpNode);
	if(pthredTpNode == NULL)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find pthredTpNode use ID(%08x)\n\r",\
			__FUNCTION__, thredTpId);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	if(gpnStatEventThredTpSubTypeQuery(pthredTpNode,\
		subType, &evnThredTpLine) == GPN_STAT_SNMP_GEN_ERR)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : thredTpId(%08x) subtype(%08x) err!\n\r",\
			__FUNCTION__, thredTpId, subType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_GET_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = evnThredTpLine.thredTpId;
	pgpnStatMsgSpOut->iMsgPara2 = evnThredTpLine.subType;
	pgpnStatMsgSpOut->iMsgPara3 = evnThredTpLine.scanType;
	pgpnStatMsgSpOut->iMsgPara4 = evnThredTpLine.upThredH32;
	pgpnStatMsgSpOut->iMsgPara5 = evnThredTpLine.upThredL32;
	pgpnStatMsgSpOut->iMsgPara6 = evnThredTpLine.dnThredH32;
	pgpnStatMsgSpOut->iMsgPara7 = evnThredTpLine.dnThredL32;

	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiEventThredTpGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 thredTpId;
	UINT32 subType;
	UINT32 nextThredTpId;
	stStatEvnThredTpNode *pevnThredTpNode;
	stStatEvnThredTpTable evnThredTpLine;

	static UINT32 sNextEvnThredTpId;
	static UINT32 sNextSubType;
	static stStatEvnThredTpNode sEvnThredTpNode;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	thredTpId = pgpnStatMsgSpIn->iMsgPara1;
	subType = pgpnStatMsgSpIn->iMsgPara2;

		/*1 static node just init only ?*/
	if( (sEvnThredTpNode.thredTpId != 0) &&\
		/*2 is this get-next a get-next-start ?*/
		(thredTpId != GPN_STAT_INVALID_TEMPLAT_ID) &&\
		/*3 is this get-next at get-next-line-start ?*/
		(subType != GPN_STAT_SUB_TYPE_INVALID) &&\
		/*4 is this get-next has same line whit last ?*/
		(sEvnThredTpNode.thredTpId == thredTpId) &&\
		/*5 is this get-next just not has same unit with last ?*/
		(sNextSubType != subType) )
	{
		/*last memrey save node for this get-next */
		pevnThredTpNode = &sEvnThredTpNode;
	}
	else
	{
		/*re-search for new node */
		pevnThredTpNode = NULL;
		sNextEvnThredTpId = 0;
		gpnStatEventThredTpGetNext(thredTpId, &pevnThredTpNode, &sNextEvnThredTpId);
		if(pevnThredTpNode == NULL)
		{
			/*clear static memrey save space for */
			memset(&sEvnThredTpNode, 0, sizeof(stStatEvnThredTpNode));

			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find pevnThredTpNode use ID(%08x)\n\r",\
				__FUNCTION__, thredTpId);
			return GPN_STAT_SNMP_GEN_ERR;
		}
		else
		{
			/*copy node info to static memrey save space */
			memcpy(&sEvnThredTpNode, pevnThredTpNode, sizeof(stStatEvnThredTpNode));
		}
	}

	if(gpnStatEventThredTpSubTypeQueryNext(pevnThredTpNode, thredTpId, subType, &evnThredTpLine,\
		sNextEvnThredTpId, &nextThredTpId, &sNextSubType) != GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find subType(%08x) report info\n\r",\
			__FUNCTION__, subType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	if(gpnStatEventThredTpSubTypeQueryNextAdjust(&nextThredTpId, &sNextSubType) !=\
		GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : (%d)(%08x) next index adjust err!\n\r",\
			__FUNCTION__, nextThredTpId, sNextSubType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
		
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = evnThredTpLine.thredTpId;
	pgpnStatMsgSpOut->iMsgPara2 = evnThredTpLine.subType;
	pgpnStatMsgSpOut->iMsgPara3 = evnThredTpLine.scanType;
	pgpnStatMsgSpOut->iMsgPara4 = evnThredTpLine.upThredH32;
	pgpnStatMsgSpOut->iMsgPara5 = evnThredTpLine.upThredL32;
	pgpnStatMsgSpOut->iMsgPara6 = evnThredTpLine.dnThredH32;
	pgpnStatMsgSpOut->iMsgPara7 = evnThredTpLine.dnThredL32;
	pgpnStatMsgSpOut->iMsgPara8 = nextThredTpId;
	pgpnStatMsgSpOut->iMsgPara9 = sNextSubType;
	pgpnStatMsgSpOut->msgCellLen= 0;

	return GPN_STAT_SNMP_GEN_OK;
}


UINT32 gpnStatSnmpApiAlarmThredTpGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 thredTpId;	
	UINT32 subType;
	stStatAlmThredTpNode *pthredTpNode;
	stStatAlmThredTpTable almThredTpLine;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	thredTpId = pgpnStatMsgSpIn->iMsgPara1;
	subType = pgpnStatMsgSpIn->iMsgPara2;
	
	pthredTpNode = NULL;
	gpnStatAlarmThredTpGet(thredTpId, &pthredTpNode);
	if(pthredTpNode == NULL)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find pthredTpNode use ID(%08x)\n\r",\
			__FUNCTION__, thredTpId);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	if(gpnStatAlarmThredTpSubTypeQuery(pthredTpNode,\
		subType, &almThredTpLine) == GPN_STAT_SNMP_GEN_ERR)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : thredTpId(%08x) subtype(%08x) err!\n\r",\
			__FUNCTION__, thredTpId, subType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_ALM_THRED_TEMP_GET_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = almThredTpLine.thredTpId;
	pgpnStatMsgSpOut->iMsgPara2 = almThredTpLine.subType;
	pgpnStatMsgSpOut->iMsgPara3 = almThredTpLine.scanType;
	pgpnStatMsgSpOut->iMsgPara4 = almThredTpLine.upRiseThredH32;
	pgpnStatMsgSpOut->iMsgPara5 = almThredTpLine.upRiseThredL32;
	pgpnStatMsgSpOut->iMsgPara6 = almThredTpLine.upDispThredH32;
	pgpnStatMsgSpOut->iMsgPara7 = almThredTpLine.upDispThredL32;
	pgpnStatMsgSpOut->iMsgPara8 = almThredTpLine.dnRiseThredH32;
	pgpnStatMsgSpOut->iMsgPara9 = almThredTpLine.dnRiseThredL32;
	pgpnStatMsgSpOut->iMsgParaA = almThredTpLine.dnDispThredH32;
	pgpnStatMsgSpOut->iMsgParaB = almThredTpLine.dnDispThredL32;

	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiAlarmThredTpGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 thredTpId;
	UINT32 subType;
	UINT32 nextThredTpId;
	stStatAlmThredTpNode *palmThredTpNode;
	stStatAlmThredTpTable almThredTpLine;

	static UINT32 sNextAlmThredTpId;
	static UINT32 sNextSubType;
	static stStatAlmThredTpNode sAlmThredTpNode;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	thredTpId = pgpnStatMsgSpIn->iMsgPara1;
	subType = pgpnStatMsgSpIn->iMsgPara2;

		/*1 static node just init only ?*/
	if( (sAlmThredTpNode.thredTpId != 0) &&\
		/*2 is this get-next a get-next-start ?*/
		(thredTpId != GPN_STAT_INVALID_TEMPLAT_ID) &&\
		/*3 is this get-next at get-next-line-start ?*/
		(subType != GPN_STAT_SUB_TYPE_INVALID) &&\
		/*4 is this get-next has same line with last ?*/
		(sAlmThredTpNode.thredTpId == thredTpId) &&\
		/*5 is this get-next just not has same unit with last ?*/
		(sNextSubType != subType) )
	{
		/*last memrey save node for this get-next */
		palmThredTpNode = &sAlmThredTpNode;
	}
	else
	{
		/*re-search for new node */
		palmThredTpNode = NULL;
		sNextAlmThredTpId = 0;
		gpnStatAlarmThredTpGetNext(thredTpId, &palmThredTpNode, &sNextAlmThredTpId);
		if(palmThredTpNode == NULL)
		{
			/*clear static memrey save space for */
			memset(&sAlmThredTpNode, 0, sizeof(stStatAlmThredTpNode));

			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find pevnThredTpNode use ID(%08x)\n\r",\
				__FUNCTION__, thredTpId);
			return GPN_STAT_SNMP_GEN_ERR;
		}
		else
		{
			/*copy node info to static memrey save space */
			memcpy(&sAlmThredTpNode, palmThredTpNode, sizeof(stStatAlmThredTpNode));
		}
	}

	if(gpnStatAlarmThredTpSubTypeQueryNext(palmThredTpNode, thredTpId, subType, &almThredTpLine,\
		sNextAlmThredTpId, &nextThredTpId, &sNextSubType) != GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find subType(%08x) alarm thred info\n\r",\
			__FUNCTION__, subType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	if(gpnStatAlarmThredTpSubTypeQueryNextAdjust(&nextThredTpId, &sNextSubType) !=\
		GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : (%d)(%08x) next index adjust err!\n\r",\
			__FUNCTION__, nextThredTpId, sNextSubType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = almThredTpLine.thredTpId;
	pgpnStatMsgSpOut->iMsgPara2 = almThredTpLine.subType;
	pgpnStatMsgSpOut->iMsgPara3 = almThredTpLine.scanType;
	pgpnStatMsgSpOut->iMsgPara4 = almThredTpLine.upRiseThredH32;
	pgpnStatMsgSpOut->iMsgPara5 = almThredTpLine.upRiseThredL32;
	pgpnStatMsgSpOut->iMsgPara6 = almThredTpLine.upDispThredH32;
	pgpnStatMsgSpOut->iMsgPara7 = almThredTpLine.upDispThredL32;
	pgpnStatMsgSpOut->iMsgPara8 = almThredTpLine.dnRiseThredH32;
	pgpnStatMsgSpOut->iMsgPara9 = almThredTpLine.dnRiseThredL32;
	pgpnStatMsgSpOut->iMsgParaA = almThredTpLine.dnDispThredH32;
	pgpnStatMsgSpOut->iMsgParaB = almThredTpLine.dnDispThredL32;
	pgpnStatMsgSpOut->iMsgParaC = nextThredTpId;
	pgpnStatMsgSpOut->iMsgParaD = sNextSubType;
	pgpnStatMsgSpOut->msgCellLen= 0;

	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiSubFiltTpGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 subFiltTpId;
	UINT32 subType;
	stStatSubFiltTpNode *psubFiltTpNode;
	stStatSubFiltTpTable subFiltTpLine;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	subFiltTpId = pgpnStatMsgSpIn->iMsgPara1;
	subType = pgpnStatMsgSpIn->iMsgPara2;
	
	psubFiltTpNode = NULL;
	if( (gpnStatSubFiltTpGet(subFiltTpId, &psubFiltTpNode) != GPN_STAT_SNMP_GEN_OK) ||\
		(psubFiltTpNode == NULL) )
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find subFiltTpNode use ID(%08x)\n\r",\
			__FUNCTION__, subFiltTpId);
		return GPN_STAT_SNMP_GEN_ERR;
	}

	if(gpnStatSubFiltTpSubTypeQuery(psubFiltTpNode,\
		subType, &subFiltTpLine) != GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find sunFiltId(%d) subType(%08x) filt info\n\r",\
			__FUNCTION__, psubFiltTpNode->subFiltTpId, subType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_GET_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = subFiltTpLine.subFiltTpId;
	pgpnStatMsgSpOut->iMsgPara2 = subFiltTpLine.subType;
	pgpnStatMsgSpOut->iMsgPara3 = subFiltTpLine.scanType;
	pgpnStatMsgSpOut->iMsgPara4 = subFiltTpLine.status;
	
	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiSubFiltTpGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 subFiltTpId;
	UINT32 subType;
	UINT32 nextSubFiltTpId;
	stStatSubFiltTpNode *psubFiltTpNode;
	stStatSubFiltTpTable subFiltTpLine;

	static UINT32 sNextSubFiltTpId;
	static UINT32 sNextSubType;
	static stStatSubFiltTpNode sSubFiltTpNode;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	subFiltTpId = pgpnStatMsgSpIn->iMsgPara1;
	subType = pgpnStatMsgSpIn->iMsgPara2;

		/*1 static node just init only ?*/
	if( (sSubFiltTpNode.subFiltTpId != 0) &&\
		/*2 is this get-next a get-next-start ?*/
		(subFiltTpId != GPN_STAT_INVALID_TEMPLAT_ID) &&\
		/*3 is this get-next at get-next-line-start ?*/
		(subType != GPN_STAT_SUB_TYPE_INVALID) &&\
		/*4 is this get-next has same line with last ?*/
		(sSubFiltTpNode.subFiltTpId == subFiltTpId) &&\
		/*5 is this get-next just not has same unit with last ?*/
		(sNextSubType != subType) )
	{
		/*last memrey save node for this get-next */
		psubFiltTpNode = &sSubFiltTpNode;
	}
	else
	{
		/*re-search for new node */
		psubFiltTpNode = NULL;
		sNextSubFiltTpId = 0;
		gpnStatSubFiltTpGetNext(subFiltTpId, &psubFiltTpNode, &sNextSubFiltTpId);
		if(psubFiltTpNode == NULL)
		{
			/*clear static memrey save space for */
			memset(&sSubFiltTpNode, 0, sizeof(stStatSubFiltTpNode));

			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find subFiltTpNode use ID(%08x)\n\r",\
				__FUNCTION__, subFiltTpId);
			return GPN_STAT_SNMP_GEN_ERR;
		}
		else
		{
			/*copy node info to static memrey save space */
			memcpy(&sSubFiltTpNode, psubFiltTpNode, sizeof(stStatSubFiltTpNode));
		}
	}

	if(gpnStatSubFiltTpSubTypeQueryNext(psubFiltTpNode, subFiltTpId, subType, &subFiltTpLine,\
		sNextSubFiltTpId, &nextSubFiltTpId, &sNextSubType) != GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find subType(%08x) filt info\n\r",\
			__FUNCTION__, subType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	if(gpnStatSubFiltTpSubTypeQueryNextAdjust(&nextSubFiltTpId, &sNextSubType) !=\
		GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : (%d)(%08x) next index adjust err!\n\r",\
			__FUNCTION__, sNextSubType, sNextSubType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
		
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = subFiltTpLine.subFiltTpId;
	pgpnStatMsgSpOut->iMsgPara2 = subFiltTpLine.subType;
	pgpnStatMsgSpOut->iMsgPara3 = subFiltTpLine.scanType;
	pgpnStatMsgSpOut->iMsgPara4 = subFiltTpLine.status;
	pgpnStatMsgSpOut->iMsgPara5 = nextSubFiltTpId;
	pgpnStatMsgSpOut->iMsgPara6 = sNextSubType;

	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiSubReportTpGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 subReportTpId;
	UINT32 subType;
	stStatSubReportTpNode *psubReportTpNode;
	stStatSubReportTpTable subReportTpLine;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	subReportTpId = pgpnStatMsgSpIn->iMsgPara1;
	subType = pgpnStatMsgSpIn->iMsgPara2;
	
	psubReportTpNode = NULL;
	if( (gpnStatSubReportTpGet(subReportTpId, &psubReportTpNode) != GPN_STAT_SNMP_GEN_OK) ||\
		(psubReportTpNode == NULL) )
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find subReportTpNode use ID(%08x)\n\r",\
			__FUNCTION__, subReportTpId);
		return GPN_STAT_SNMP_GEN_ERR;
	}

	if(gpnStatSubReportTpSubTypeQuery(psubReportTpNode,\
		subType, &subReportTpLine) != GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find sunReportId(%d) subType(%08x) report info\n\r",\
			__FUNCTION__, psubReportTpNode->subReportTpId, subType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_SUBREPORT_TEMP_GET_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = subReportTpLine.subReportTpId;
	pgpnStatMsgSpOut->iMsgPara2 = subReportTpLine.subType;
	pgpnStatMsgSpOut->iMsgPara3 = subReportTpLine.scanType;
	pgpnStatMsgSpOut->iMsgPara4 = subReportTpLine.status;
	pgpnStatMsgSpOut->msgCellLen= 0;
	
	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiSubReportTpGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 subReportTpId;
	UINT32 subType;
	UINT32 nextSubReportTpId;
	stStatSubReportTpNode *psubReportTpNode;
	stStatSubReportTpTable subReportTpLine;

	static UINT32 sNextSubReportTpId;
	static UINT32 sNextSubType;
	static stStatSubReportTpNode sSubReportTpNode;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	subReportTpId = pgpnStatMsgSpIn->iMsgPara1;
	subType = pgpnStatMsgSpIn->iMsgPara2;

		/*1 static node just init only ?*/
	if( (sSubReportTpNode.subReportTpId != 0) &&\
		/*2 is this get-next a get-next-start ?*/
		(subReportTpId != GPN_STAT_INVALID_TEMPLAT_ID) &&\
		/*3 is this get-next at get-next-line-start ?*/
		(subType != GPN_STAT_SUB_TYPE_INVALID) &&\
		/*4 is this get-next has same line whit last ?*/
		(sSubReportTpNode.subReportTpId == subReportTpId) &&\
		/*5 is this get-next just not has same unit with last ?*/
		(sNextSubType != subType) )
	{
		/*last memrey save node for this get-next */
		psubReportTpNode = &sSubReportTpNode;
	}
	else
	{
		/*re-search for new node */
		psubReportTpNode = NULL;
		sNextSubReportTpId = 0;
		gpnStatSubReportTpGetNext(subReportTpId, &psubReportTpNode, &sNextSubReportTpId);
		if(psubReportTpNode == NULL)
		{
			/*clear static memrey save space for */
			memset(&sSubReportTpNode, 0, sizeof(stStatSubReportTpNode));

			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find subReportTpNode use ID(%08x)\n\r",\
				__FUNCTION__, subReportTpId);
			return GPN_STAT_SNMP_GEN_ERR;
		}
		else
		{
			/*copy node info to static memrey save space */
			memcpy(&sSubReportTpNode, psubReportTpNode, sizeof(stStatSubReportTpNode));
		}
	}

	if(gpnStatSubReportTpSubTypeQueryNext(psubReportTpNode, subReportTpId, subType, &subReportTpLine,\
		sNextSubReportTpId, &nextSubReportTpId, &sNextSubType) != GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find subType(%08x) report info\n\r",\
			__FUNCTION__, subType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	if(gpnStatSubReportTpSubTypeQueryNextAdjust(&nextSubReportTpId, &sNextSubType) !=\
		GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : (%d)(%08x) next index adjust err!\n\r",\
			__FUNCTION__, sNextSubType, sNextSubType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
		
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = subReportTpLine.subReportTpId;
	pgpnStatMsgSpOut->iMsgPara2 = subReportTpLine.subType;
	pgpnStatMsgSpOut->iMsgPara3 = subReportTpLine.scanType;
	pgpnStatMsgSpOut->iMsgPara4 = subReportTpLine.status;
	pgpnStatMsgSpOut->iMsgPara5 = nextSubReportTpId;
	pgpnStatMsgSpOut->iMsgPara6 = sNextSubType;
	pgpnStatMsgSpOut->msgCellLen= 0;

	return GPN_STAT_SNMP_GEN_OK;
}
UINT32 gpnStatSnmpApiPortStatMonCfgGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	objLogicDesc localPIndex;
	UINT32 scanType;
	stStatLocalPortNode *pStatLocalNode;
	stStatMsgMonObjCfg *pstatMonObjCfg;
	stStatPortMonMgt portMonMgt;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	localPIndex.devIndex = pgpnStatMsgSpIn->iMsgPara1;	
	/*base portIndex */
	localPIndex.portIndex = pgpnStatMsgSpIn->iMsgPara2;
	localPIndex.portIndex3 = pgpnStatMsgSpIn->iMsgPara3;
	localPIndex.portIndex4 = pgpnStatMsgSpIn->iMsgPara4;
	localPIndex.portIndex5 = pgpnStatMsgSpIn->iMsgPara5;
	localPIndex.portIndex6 = pgpnStatMsgSpIn->iMsgPara6;
	/*scanType index */
	scanType = pgpnStatMsgSpIn->iMsgPara7;

	/*out msg should have enough space */
	if(pgpnStatMsgSpOut->msgCellLen < sizeof(stStatMsgMonObjCfg))
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) scanType(%08x) have (%d) < (%d)\n\r",\
			__FUNCTION__, localPIndex.devIndex, localPIndex.portIndex, localPIndex.portIndex3,\
			 localPIndex.portIndex4, localPIndex.portIndex5, localPIndex.portIndex6,\
			scanType, pgpnStatMsgSpOut->msgCellLen, sizeof(stStatMsgMonObjCfg));
		return GPN_STAT_SNMP_GEN_ERR;
	}

	pStatLocalNode = NULL;
	gpnStatSeekLocalPort2LocalPortNode(&localPIndex, &pStatLocalNode);
	if(pStatLocalNode == NULL)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) to scanIndex err\n\r",\
			__FUNCTION__, localPIndex.devIndex, localPIndex.portIndex, localPIndex.portIndex3,\
			localPIndex.portIndex4, localPIndex.portIndex5, localPIndex.portIndex6);
		return GPN_STAT_SNMP_GEN_ERR;
	}

	if(gpnStatPortMonGet(pStatLocalNode, scanType, &portMonMgt) != GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x) get err!\n\r",\
			__FUNCTION__, localPIndex.devIndex, localPIndex.portIndex, localPIndex.portIndex3,\
			 localPIndex.portIndex4, localPIndex.portIndex5, localPIndex.portIndex6, scanType);
		return GPN_STAT_SNMP_GEN_ERR;
	}

	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_MON_GET_NEXT_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = portMonMgt.portIndex.devIndex;
	pgpnStatMsgSpOut->iMsgPara2 = portMonMgt.portIndex.portIndex;
	pgpnStatMsgSpOut->iMsgPara3 = portMonMgt.portIndex.portIndex3;
	pgpnStatMsgSpOut->iMsgPara4 = portMonMgt.portIndex.portIndex4;
	pgpnStatMsgSpOut->iMsgPara5 = portMonMgt.portIndex.portIndex5;
	pgpnStatMsgSpOut->iMsgPara6 = portMonMgt.portIndex.portIndex6;
	/*scan type*/
	pgpnStatMsgSpOut->iMsgPara7 = portMonMgt.scanType;
	
	pstatMonObjCfg = (stStatMsgMonObjCfg *)(&(pgpnStatMsgSpOut->msgCellLen) + 1);
	memcpy(&(pstatMonObjCfg->portIndex), &(portMonMgt.portIndex), sizeof(objLogicDesc));
	pstatMonObjCfg->scanType = portMonMgt.scanType;
	pstatMonObjCfg->insAddFlag = portMonMgt.insAddFlag;
	pstatMonObjCfg->statMoniEn = portMonMgt.statMoniEn;
	pstatMonObjCfg->currStatMoniEn = portMonMgt.currStatMoniEn;
	pstatMonObjCfg->longCycStatMoniEn = portMonMgt.longCycStatMoniEn;
	pstatMonObjCfg->longCycBelongTask = portMonMgt.longCycBelongTask;
	pstatMonObjCfg->longCycEvnThredTpId = portMonMgt.longCycEvnThredTpId;
	pstatMonObjCfg->longCycSubFiltTpId = portMonMgt.longCycSubFiltTpId;
	pstatMonObjCfg->longCycAlmThredTpId = portMonMgt.longCycAlmThredTpId;
	pstatMonObjCfg->longCycHistReptTpId = portMonMgt.longCycHistReptTpId;
	pstatMonObjCfg->longCycHistDBId = portMonMgt.longCycHistDBId;
	pstatMonObjCfg->shortCycStatMoniEn = portMonMgt.shortCycStatMoniEn;
	pstatMonObjCfg->shortCycBelongTask = portMonMgt.shortCycBelongTask;
	pstatMonObjCfg->shortCycEvnThredTpId = portMonMgt.shortCycEvnThredTpId;
	pstatMonObjCfg->shortCycSubFiltTpId = portMonMgt.shortCycSubFiltTpId;
	pstatMonObjCfg->shortCycAlmThredTpId = portMonMgt.shortCycAlmThredTpId;
	pstatMonObjCfg->shortCycHistReptTpId = portMonMgt.shortCycHistReptTpId;
	pstatMonObjCfg->shortCycHistDBId = portMonMgt.shortCycHistDBId;
	pstatMonObjCfg->udCycStatMoniEn = portMonMgt.udCycStatMoniEn;
	pstatMonObjCfg->udCycBelongTask = portMonMgt.udCycBelongTask;
	pstatMonObjCfg->udCycEvnThredTpId = portMonMgt.udCycEvnThredTpId;
	pstatMonObjCfg->udCycSubFiltTpId = portMonMgt.udCycSubFiltTpId;
	pstatMonObjCfg->udCycAlmThredTpId = portMonMgt.udCycAlmThredTpId;
	pstatMonObjCfg->udCycHistReptTpId = portMonMgt.udCycHistReptTpId;
	pstatMonObjCfg->udCycHistDBId = portMonMgt.udCycHistDBId;
	pstatMonObjCfg->udCycSecs = portMonMgt.udCycSecs;

	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiPortStatMonCfgGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 scanType;
	objLogicDesc localPIndex;
	stStatMsgMonObjCfg *pstatMonObjCfg;
	stStatLocalPortNode *pStatLocalNode;
	stStatObjDesc *pstatObjDesc;
	UINT32 *pnextScanType;
	stStatPortMonMgt portMonMgt;

	static stStatLocalPortNode *sStatLocalNode;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	localPIndex.devIndex = pgpnStatMsgSpIn->iMsgPara1;
	/*base portIndex */
	localPIndex.portIndex = pgpnStatMsgSpIn->iMsgPara2;
	localPIndex.portIndex3 = pgpnStatMsgSpIn->iMsgPara3;
	localPIndex.portIndex4 = pgpnStatMsgSpIn->iMsgPara4;
	localPIndex.portIndex5 = pgpnStatMsgSpIn->iMsgPara5;
	localPIndex.portIndex6 = pgpnStatMsgSpIn->iMsgPara6;
	/*scanType Index */
	scanType = pgpnStatMsgSpIn->iMsgPara7;
	
	/*out msg should have enough space */
	if(pgpnStatMsgSpOut->msgCellLen <\
		(sizeof(stStatMsgMonObjCfg) + sizeof(stStatObjDesc) + sizeof(UINT32)))
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) scanType(%08x) have (%d) < (%d)\n\r",\
			__FUNCTION__, localPIndex.devIndex, localPIndex.portIndex, localPIndex.portIndex3,\
			localPIndex.portIndex4, localPIndex.portIndex5, localPIndex.portIndex6, scanType,\
			pgpnStatMsgSpOut->msgCellLen,sizeof(stStatMsgMonObjCfg) + sizeof(stStatObjDesc));
		return GPN_STAT_SNMP_GEN_ERR;
	}

	if(localPIndex.portIndex == GPN_ILLEGAL_PORT_INDEX)
	{
		pStatLocalNode = NULL;
		gpnStatSeekFirstValidScanNodeIndex(&pStatLocalNode);
		if(pStatLocalNode != NULL)
		{
			gpnStatSeekNextValidScanNodeIndex(pStatLocalNode, &sStatLocalNode);
		}
	}
	else
	{
		pStatLocalNode = NULL;
		gpnStatSeekLocalPort2LocalPortNode(&localPIndex, &pStatLocalNode);
		if( (pStatLocalNode != NULL) &&\
			(pStatLocalNode->pStatScanPort->scanQuenValid == GPN_STAT_DBS_GEN_ENABLE) )
		{
			gpnStatSeekNextValidScanNodeIndex(pStatLocalNode, &sStatLocalNode);
		}
	}

	if(gpnStatPortMonGet(pStatLocalNode, scanType, &portMonMgt) != GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x) get err!\n\r",\
			__FUNCTION__, localPIndex.devIndex, localPIndex.portIndex, localPIndex.portIndex3,\
			 localPIndex.portIndex4, localPIndex.portIndex5, localPIndex.portIndex6, scanType);
		return GPN_STAT_SNMP_GEN_ERR;
	}

	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_MON_GET_NEXT_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = portMonMgt.portIndex.devIndex;
	pgpnStatMsgSpOut->iMsgPara2 = portMonMgt.portIndex.portIndex;
	pgpnStatMsgSpOut->iMsgPara3 = portMonMgt.portIndex.portIndex3;
	pgpnStatMsgSpOut->iMsgPara4 = portMonMgt.portIndex.portIndex4;
	pgpnStatMsgSpOut->iMsgPara5 = portMonMgt.portIndex.portIndex5;
	pgpnStatMsgSpOut->iMsgPara6 = portMonMgt.portIndex.portIndex6;
	/*scan type*/
	pgpnStatMsgSpOut->iMsgPara7 = portMonMgt.scanType;

	/*set payload : protMonInfo */
	pstatMonObjCfg = (stStatMsgMonObjCfg *)(&(pgpnStatMsgSpOut->msgCellLen) + 1);
	memcpy(&(pstatMonObjCfg->portIndex), &(portMonMgt.portIndex), sizeof(objLogicDesc));
	pstatMonObjCfg->scanType = portMonMgt.scanType;
	pstatMonObjCfg->insAddFlag = portMonMgt.insAddFlag;
	pstatMonObjCfg->statMoniEn = portMonMgt.statMoniEn;
	pstatMonObjCfg->currStatMoniEn = portMonMgt.currStatMoniEn;
	pstatMonObjCfg->longCycStatMoniEn = portMonMgt.longCycStatMoniEn;
	pstatMonObjCfg->longCycBelongTask = portMonMgt.longCycBelongTask;
	pstatMonObjCfg->longCycEvnThredTpId = portMonMgt.longCycEvnThredTpId;
	pstatMonObjCfg->longCycSubFiltTpId = portMonMgt.longCycSubFiltTpId;
	pstatMonObjCfg->longCycAlmThredTpId = portMonMgt.longCycAlmThredTpId;
	pstatMonObjCfg->longCycHistReptTpId = portMonMgt.longCycHistReptTpId;
	pstatMonObjCfg->longCycHistDBId = portMonMgt.longCycHistDBId;
	pstatMonObjCfg->shortCycStatMoniEn = portMonMgt.shortCycStatMoniEn;
	pstatMonObjCfg->shortCycBelongTask = portMonMgt.shortCycBelongTask;
	pstatMonObjCfg->shortCycEvnThredTpId = portMonMgt.shortCycEvnThredTpId;
	pstatMonObjCfg->shortCycSubFiltTpId = portMonMgt.shortCycSubFiltTpId;
	pstatMonObjCfg->shortCycAlmThredTpId = portMonMgt.shortCycAlmThredTpId;
	pstatMonObjCfg->shortCycHistReptTpId = portMonMgt.shortCycHistReptTpId;
	pstatMonObjCfg->shortCycHistDBId = portMonMgt.shortCycHistDBId;
	pstatMonObjCfg->udCycStatMoniEn = portMonMgt.udCycStatMoniEn;
	pstatMonObjCfg->udCycBelongTask = portMonMgt.udCycBelongTask;
	pstatMonObjCfg->udCycEvnThredTpId = portMonMgt.udCycEvnThredTpId;
	pstatMonObjCfg->udCycSubFiltTpId = portMonMgt.udCycSubFiltTpId;
	pstatMonObjCfg->udCycAlmThredTpId = portMonMgt.udCycAlmThredTpId;
	pstatMonObjCfg->udCycHistReptTpId = portMonMgt.udCycHistReptTpId;
	pstatMonObjCfg->udCycHistDBId = portMonMgt.udCycHistDBId;
	pstatMonObjCfg->udCycSecs = portMonMgt.udCycSecs;
	
	/*get next stStatObjDesc : if first search(stStatObjDesc->portIndex is 0), next is first stStatObjDesc */
	/*get next scanType */
	pstatObjDesc = (stStatObjDesc *)(pstatMonObjCfg + 1);
	pnextScanType = (UINT32 *)(pstatObjDesc + 1);
	if(localPIndex.portIndex == GPN_ILLEGAL_PORT_INDEX)
	{
		memcpy(pstatObjDesc, &(portMonMgt.portIndex), sizeof(stStatObjDesc));
		/*pStatScanPort's first scanType */
		*pnextScanType = portMonMgt.scanType;
		
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : next dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x)\n\r",\
			__FUNCTION__, pstatObjDesc->devIndex, pstatObjDesc->portIndex,\
			pstatObjDesc->portIndex3, pstatObjDesc->portIndex4,\
			pstatObjDesc->portIndex5, pstatObjDesc->portIndex6,\
			*pnextScanType);
	}
	else
	{
		if (sStatLocalNode != NULL)
		{
			memcpy(pstatObjDesc, &(sStatLocalNode->localPort), sizeof(stStatObjDesc));
			/*pStatScanPort's first scanType */
			*pnextScanType = sStatLocalNode->pStatScanPort->statScanType;
			
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : next dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x)\n\r",\
				__FUNCTION__, pstatObjDesc->devIndex, pstatObjDesc->portIndex,\
				pstatObjDesc->portIndex3, pstatObjDesc->portIndex4,\
				pstatObjDesc->portIndex5, pstatObjDesc->portIndex6,\
				*pnextScanType);	
		}
		else
		{
			pstatObjDesc->devIndex = GPN_ILLEGAL_DEVICE_INDEX;
			pstatObjDesc->portIndex = GPN_ILLEGAL_PORT_INDEX;
			pstatObjDesc->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			pstatObjDesc->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			pstatObjDesc->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			pstatObjDesc->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			/*pStatScanPort's first scanType */
			*pnextScanType = GPN_STAT_SCAN_TYPE_ALL;
			
			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : next port is NULL\n\r",\
				__FUNCTION__);
			
		}
	}
	
	return GPN_STAT_SNMP_GEN_OK;
}
UINT32 gpnStatSnmpApiCurrDataBaseSubTypeGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 subType;
	objLogicDesc localPIndex;
	stStatCurrDataTable currDataLine;
	stStatLocalPortNode *pStatLocalNode;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	/*base portIndex */
	localPIndex.devIndex = pgpnStatMsgSpIn->iMsgPara1;
	localPIndex.portIndex = pgpnStatMsgSpIn->iMsgPara2;
	localPIndex.portIndex3 = pgpnStatMsgSpIn->iMsgPara3;
	localPIndex.portIndex4 = pgpnStatMsgSpIn->iMsgPara4;
	localPIndex.portIndex5 = pgpnStatMsgSpIn->iMsgPara5;
	localPIndex.portIndex6 = pgpnStatMsgSpIn->iMsgPara6;
	
	/*subType index, subType valid ?*/
	subType = pgpnStatMsgSpIn->iMsgPara7;

	pStatLocalNode = NULL;
	gpnStatSeekLocalPort2LocalPortNode(&localPIndex, &pStatLocalNode);
	if(pStatLocalNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) can't find perScanNode!\n\r",\
			__FUNCTION__, localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(gpnStatCurrDataSubTypeQurey(pStatLocalNode, subType, &currDataLine) == GPN_STAT_SNMP_GEN_ERR)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : scanType(%08x)'s subType(%08x) curr data err!\n\r",\
			__FUNCTION__, currDataLine.scanType, subType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_CURR_DATA_GET_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = currDataLine.viewPort.devIndex;
	pgpnStatMsgSpOut->iMsgPara2 = currDataLine.viewPort.portIndex;
	pgpnStatMsgSpOut->iMsgPara3 = currDataLine.viewPort.portIndex3;
	pgpnStatMsgSpOut->iMsgPara4 = currDataLine.viewPort.portIndex4;
	pgpnStatMsgSpOut->iMsgPara5 = currDataLine.viewPort.portIndex5;
	pgpnStatMsgSpOut->iMsgPara6 = currDataLine.viewPort.portIndex6;
	pgpnStatMsgSpOut->iMsgPara7 = currDataLine.subType;
	pgpnStatMsgSpOut->iMsgPara8 = currDataLine.scanType;
	pgpnStatMsgSpOut->iMsgPara9 = currDataLine.currDataH32;
	pgpnStatMsgSpOut->iMsgParaA = currDataLine.currDataL32;
	pgpnStatMsgSpOut->iMsgParaB = currDataLine.currDataClr;
	pgpnStatMsgSpOut->msgCellLen = 0;

	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiCurrDataBaseSubTypeGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 subType;
	objLogicDesc localPIndex;
	objLogicDesc nextPIndex;
	stStatCurrDataTable currDataLine;
	stStatLocalPortNode *pStatLocalNode;

	gpnStatMsgCurrDataIndex *currIndex;
	
	static UINT32 sNextSubType;
	static objLogicDesc sNextPIndex;
	static objLogicDesc sLocalPIndex;
	static stStatLocalPortNode *spStatLocalNode;
	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	/*base portIndex */
	localPIndex.devIndex = pgpnStatMsgSpIn->iMsgPara1;
	localPIndex.portIndex = pgpnStatMsgSpIn->iMsgPara2;
	localPIndex.portIndex3 = pgpnStatMsgSpIn->iMsgPara3;
	localPIndex.portIndex4 = pgpnStatMsgSpIn->iMsgPara4;
	localPIndex.portIndex5 = pgpnStatMsgSpIn->iMsgPara5;
	localPIndex.portIndex6 = pgpnStatMsgSpIn->iMsgPara6;
	
	/*subType index, subType valid ?*/
	subType = pgpnStatMsgSpIn->iMsgPara7;

	/*out msg should have enough space */
	if(pgpnStatMsgSpOut->msgCellLen < sizeof(gpnStatMsgCurrDataIndex))
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CMP, "%s : dev(%08x) port(%08x|%08x|%08x|%08x|%08x) subType(%08x) have (%d) < (%d)\n\r",\
			__FUNCTION__, localPIndex.devIndex, localPIndex.portIndex, localPIndex.portIndex3,\
			localPIndex.portIndex4, localPIndex.portIndex5, localPIndex.portIndex6, subType,\
			pgpnStatMsgSpOut->msgCellLen,sizeof(gpnStatMsgCurrDataIndex));
		return GPN_STAT_SNMP_GEN_ERR;
	}
		/*1 static node just not init only */
	if( (sLocalPIndex.portIndex != 0) &&\
		/*2 is this get-next not a get-next-start */
		(localPIndex.portIndex != GPN_STAT_INVALID_TEMPLAT_ID) &&\
		/*3 is this get-next not at get-next-line-start */
		(subType != GPN_STAT_SUB_TYPE_INVALID) &&\
		/*4 is this get-next has same line whit last ?*/
		(sLocalPIndex.portIndex == localPIndex.portIndex) &&\
		/*5 is this get-next just not has same unit with last */
		(sNextSubType != subType) )
	{
		/*last memrey save node for this get-next */
		pStatLocalNode = spStatLocalNode;
	}
	else
	{
		if(localPIndex.portIndex == GPN_ILLEGAL_PORT_INDEX)
		{
			pStatLocalNode = NULL;
			gpnStatSeekFirstValidScanNodeIndex(&pStatLocalNode);
			if(pStatLocalNode != NULL)
			{
				gpnStatSeekNextValidScanNodeIndex(pStatLocalNode, &spStatLocalNode);
			}
		}
		else
		{
			pStatLocalNode = NULL;
			gpnStatSeekLocalPort2LocalPortNode(&localPIndex, &pStatLocalNode);
			if( (pStatLocalNode != NULL) &&\
				(pStatLocalNode->pStatScanPort->scanQuenValid == GPN_STAT_DBS_GEN_ENABLE) )
			{
				gpnStatSeekNextValidScanNodeIndex(pStatLocalNode, &spStatLocalNode);
			}
		}
		
		if(pStatLocalNode == NULL)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : dev(%08x) port(%08x|%08x|%08x|%08x|%08x) can't find validScanNode!\n\r",\
				__FUNCTION__, localPIndex.devIndex, localPIndex.portIndex,\
				localPIndex.portIndex3, localPIndex.portIndex4,\
				localPIndex.portIndex5, localPIndex.portIndex6);
			return GPN_STAT_DBS_GEN_ERR;
		}
		
		memcpy(&sLocalPIndex, &(pStatLocalNode->localPort), sizeof(objLogicDesc));
		if(spStatLocalNode != NULL)
		{
			memcpy(&sNextPIndex, &(spStatLocalNode->localPort), sizeof(objLogicDesc));
		}
		else
		{
			sNextPIndex.devIndex = GPN_ILLEGAL_DEVICE_INDEX;
			sNextPIndex.portIndex = GPN_ILLEGAL_PORT_INDEX;
			sNextPIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			sNextPIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			sNextPIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			sNextPIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
		}
	}
	
	if(gpnStatCurrDataSubTypeQureyNext(pStatLocalNode, subType, &currDataLine,\
		&sNextPIndex, &nextPIndex, &sNextSubType) == GPN_STAT_SNMP_GEN_ERR)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : dev(%08x) port(%08x|%08x|%08x|%08x|%08x) subType(%08x) curr data err!\n\r",\
			__FUNCTION__, localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			subType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s(%d) : next dev(%08x) port(%08x|%08x|%08x|%08x|%08x) subTyep(%08x)!\n\r",\
		__FUNCTION__, __LINE__, nextPIndex.devIndex, nextPIndex.portIndex,\
		nextPIndex.portIndex3, nextPIndex.portIndex4,\
		nextPIndex.portIndex5, nextPIndex.portIndex6	,\
		sNextSubType);
	
	if(gpnStatCurrDataSubTypeQureyNextAdjust(&nextPIndex, &sNextSubType) !=\
		GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : (%d)(%08x) next index adjust err!\n\r",\
			__FUNCTION__, sNextSubType, sNextSubType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s(%d) : next dev(%08x) port(%08x|%08x|%08x|%08x|%08x) subTyep(%08x)!\n\r",\
		__FUNCTION__, __LINE__, nextPIndex.devIndex, nextPIndex.portIndex,\
		nextPIndex.portIndex3, nextPIndex.portIndex4,\
		nextPIndex.portIndex5, nextPIndex.portIndex6,\
		sNextSubType);
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_CURR_DATA_GET_NEXT_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = currDataLine.viewPort.devIndex;
	pgpnStatMsgSpOut->iMsgPara2 = currDataLine.viewPort.portIndex;
	pgpnStatMsgSpOut->iMsgPara3 = currDataLine.viewPort.portIndex3;
	pgpnStatMsgSpOut->iMsgPara4 = currDataLine.viewPort.portIndex4;
	pgpnStatMsgSpOut->iMsgPara5 = currDataLine.viewPort.portIndex5;
	pgpnStatMsgSpOut->iMsgPara6 = currDataLine.viewPort.portIndex6;
	pgpnStatMsgSpOut->iMsgPara7 = currDataLine.subType;
	pgpnStatMsgSpOut->iMsgPara8 = currDataLine.currDataClr;
	pgpnStatMsgSpOut->iMsgPara9 = currDataLine.currDataH32;
	pgpnStatMsgSpOut->iMsgParaA = currDataLine.currDataL32;
	pgpnStatMsgSpOut->iMsgParaB = currDataLine.currDataClr;
	pgpnStatMsgSpOut->msgCellLen= sizeof(gpnStatMsgCurrDataIndex);
	
	currIndex = (gpnStatMsgCurrDataIndex *)(&(pgpnStatMsgSpOut->msgCellLen) + 1);
	currIndex->portIndex.devIndex = nextPIndex.devIndex;
	currIndex->portIndex.portIndex = nextPIndex.portIndex;
	currIndex->portIndex.portIndex3 = nextPIndex.portIndex3;
	currIndex->portIndex.portIndex4 = nextPIndex.portIndex4;
	currIndex->portIndex.portIndex5 = nextPIndex.portIndex5;
	currIndex->portIndex.portIndex6 = nextPIndex.portIndex6;
	currIndex->statSubType = sNextSubType;

	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiHistDataBaseSubTypeGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 histIndex;
	UINT32 stopTime;
	UINT32 subType;

	stStatHistRecdUnit *phistRecdUnit;
	stStatHistDataTable histDataLine;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	histIndex = pgpnStatMsgSpIn->iMsgPara1;
	stopTime = pgpnStatMsgSpIn->iMsgPara2;
	subType = pgpnStatMsgSpIn->iMsgPara3;

	phistRecdUnit = NULL;
	gpnStatHistDataGet(histIndex, stopTime, &phistRecdUnit);
	if(phistRecdUnit == NULL)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : histIndex(%d) stopTime(%08x) can't find histData!\n\r",\
			__FUNCTION__, histIndex, stopTime);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : histIndex(%d) stopTime(%08x) en(%d) subNum(%d).\n\r",\
		__FUNCTION__, phistRecdUnit->histDataLoc, phistRecdUnit->stopTime,\
		phistRecdUnit->en, phistRecdUnit->subWild);

	if(gpnStatHistDataSubTypeQurey(phistRecdUnit, subType, &histDataLine) == GPN_STAT_SNMP_GEN_ERR)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : subType(%08x) hist data err!\n\r",\
			__FUNCTION__, subType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_HIST_DATA_GET_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = histDataLine.index;
	pgpnStatMsgSpOut->iMsgPara2 = histDataLine.stopTime;
	pgpnStatMsgSpOut->iMsgPara3 = histDataLine.subType;
	pgpnStatMsgSpOut->iMsgPara4 = histDataLine.scanType;
	pgpnStatMsgSpOut->iMsgPara5 = histDataLine.histDataH32;
	pgpnStatMsgSpOut->iMsgPara6 = histDataLine.histDataL32;
	pgpnStatMsgSpOut->msgCellLen = 0;

	return GPN_STAT_SNMP_GEN_OK;
}

UINT32 gpnStatSnmpApiHistDataBaseSubTypeGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen)
{
	UINT32 histId;
	UINT32 stopTime;
	UINT32 subType;
	UINT32 nextHistId;
	UINT32 nextStopTime;
	stStatHistRecdUnit *phistRecdUnit;
	stStatHistDataTable histDataLine;

	static UINT32 sNextHistId;
	static UINT32 sNextStopTime;
	static UINT32 sNextSubType;
	static stStatHistRecdUnit sHistRecdUnit;

	/*assert */
	if( (pgpnStatMsgSpIn == NULL) ||\
		(pgpnStatMsgSpOut == NULL) )
	{
		return GPN_STAT_SNMP_GEN_ERR;
	}

	histId = pgpnStatMsgSpIn->iMsgPara1;
	stopTime = pgpnStatMsgSpIn->iMsgPara2;
	subType = pgpnStatMsgSpIn->iMsgPara3;

		/*1 static node just init only ?*/
	if( (sHistRecdUnit.histDataLoc != 0) &&\
		/*2 is this get-next a get-next-start ?*/
		(histId != GPN_STAT_INVALID_TEMPLAT_ID) &&\
		/*3 is this get-next at get-next-line-start ?*/
		(subType != GPN_STAT_SUB_TYPE_INVALID) &&\
		/*4 is this get-next has same line with last ?*/
		(sHistRecdUnit.histDataLoc == histId) &&\
		/*5 is this get-next just not has same unit with last ?*/
		(sNextSubType != subType) )
	{
		/*last memrey save node for this get-next */
		phistRecdUnit = &sHistRecdUnit;
	}
	else
	{
		/*re-search for new node */
		phistRecdUnit = NULL;
		nextHistId = GPN_STAT_INVALID_TEMPLAT_ID;
		nextStopTime = GPN_STAT_DBS_32_FFFF;
		gpnStatHistDataGetNext(histId, stopTime, &phistRecdUnit, &sNextHistId, &sNextStopTime);
		if(phistRecdUnit == NULL)
		{
			/*clear static memrey save space for */
			memset(&sHistRecdUnit, 0, sizeof(stStatHistRecdUnit));

			GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : histId(%d) stopTime(%08x) can't find hsitData\n\r",\
				__FUNCTION__, histId, stopTime);
			return GPN_STAT_SNMP_GEN_ERR;
		}
		else
		{
			/*copy node info to static memrey save space */
			memcpy(&sHistRecdUnit, phistRecdUnit, sizeof(stStatHistRecdUnit));
		}
	}

	if(gpnStatHistDataSubTypeQureyNext(phistRecdUnit, histId, stopTime, subType, &histDataLine,\
		sNextHistId, sNextStopTime, &nextHistId, &nextStopTime, &sNextSubType) != GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : can't find subType(%08x) histData info\n\r",\
			__FUNCTION__, subType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	if(gpnStatHistDataSubTypeQureyNextAdjust(&nextHistId, &nextStopTime, &sNextSubType) !=\
		GPN_STAT_SNMP_GEN_OK)
	{
		GPN_STAT_SNMP_PRINT(GPN_STAT_SNMP_CUP, "%s : (%d)(%08x)(%08x) next index adjust err!\n\r",\
			__FUNCTION__, nextHistId, nextStopTime, sNextSubType);
		return GPN_STAT_SNMP_GEN_ERR;
	}
	
	pgpnStatMsgSpOut->iIndex = pgpnStatMsgSpIn->iIndex;
	pgpnStatMsgSpOut->iDstId = pgpnStatMsgSpIn->iSrcId;
	pgpnStatMsgSpOut->iSrcId = pgpnStatMsgSpIn->iDstId;
	pgpnStatMsgSpOut->iMsgType = GPN_STAT_MSG_HIST_DATA_GET_NEXT_RSP;
	pgpnStatMsgSpOut->iMsgPara1 = histDataLine.index;
	pgpnStatMsgSpOut->iMsgPara2 = histDataLine.stopTime;
	pgpnStatMsgSpOut->iMsgPara3 = histDataLine.subType;
	pgpnStatMsgSpOut->iMsgPara4 = histDataLine.scanType;
	pgpnStatMsgSpOut->iMsgPara5 = histDataLine.histDataH32;
	pgpnStatMsgSpOut->iMsgPara6 = histDataLine.histDataL32;
	pgpnStatMsgSpOut->iMsgPara7 = nextHistId;
	pgpnStatMsgSpOut->iMsgPara8 = nextStopTime;
	pgpnStatMsgSpOut->iMsgPara9 = sNextSubType;
	pgpnStatMsgSpOut->msgCellLen= 0;

	return GPN_STAT_SNMP_GEN_OK;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_STAT_SNMP_API_C_*/


