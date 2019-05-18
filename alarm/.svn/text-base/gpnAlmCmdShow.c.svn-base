/******************************************************************************
 * Filename: gpnAlmCmdShow.c
 *	Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.11.18  lipf created
 *
******************************************************************************/

#include <unistd.h>
#include <string.h>
#include <time.h>

#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/thread.h>
#include <lib/ifm_common.h>
#include "socketComm/gpnSockAlmMsgDef.h"
#include "socketComm/gpnGlobalPortIndexDef.h"

#include "gpnAlmTypeStruct.h"
#include "gpnAlmCmdShow.h"
#include "gpnAlmSnmp.h"
#include "gpnAlmSnmpApi.h"
#include "gpnAlmScan.h"

extern stEQUAlmProcSpace *pgstAlmPreScanWholwInfo;


/* 时间格式转换 */
static void gpnGetLocalTimeStr(time_t t, char *str)
{
	const char *wday[] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};
	struct tm *p = NULL;
	p = localtime(&t);
	
	sprintf(str, "%d/%d/%d-%s-%d:%d:%d", 1900 + p->tm_year, 1+ p->tm_mon, p->tm_mday,\
		wday[p->tm_wday], p->tm_hour, p->tm_min, p->tm_sec);
}


/* 将告警端口类型转换为字符串，以供命令行显示 */
static void gpnAlmPortType2Str(UINT32 portIndex, char *str)
{
	if(NULL == str)
	{
		return;
	}
	
	uint32_t portType = PortIndex_GetType(portIndex);
	switch(portType)
	{
		case IFM_FUN_ETH_TYPE:
			strcpy(str, "ETH                     ");
			break;
			
		case IFM_TUNNEL_TYPE:
			strcpy(str, "TUNNEL                  ");
			break;

		case IFM_FUN_ETH_SUB_TYPE:
			strcpy(str, "ETH_SUB                 ");
			break;
		
		case IFM_VUNI_TYPE:
			strcpy(str, "VUNI                    ");
			break;

		case IFM_SOFT_TYPE:
			strcpy(str, "SOFT                    ");
			break;
		
		case IFM_PWE3E1_TYPE:
			strcpy(str, "PWE3E1                  ");
			break;
		
		case IFM_POWN48_TYPE:
			strcpy(str, "POWN48                  ");
			break;
		
		case IFM_STMN_TYPE:
			strcpy(str, "STMN                    ");
			break;

		case IFM_VC4_TYPE:
			strcpy(str, "VC4                     ");
			break;
		
		case IFM_PWE3STMN_TYPE:
			strcpy(str, "PEW3STMN                ");
			break;
		
		case IFM_EQU_TYPE:
			strcpy(str, "EQU                     ");
			break;
		
		case IFM_ENV_TYPE:
			strcpy(str, "ENV                     ");
			break;
		
		case IFM_FAN_TYPE:
			strcpy(str, "FAN                     ");
			break;
		
		case IFM_POW220_TYPE:
			strcpy(str, "POW220                  ");
			break;
		
		case IFM_ETH_CLC_IN_TYPE:
			strcpy(str, "CLC_IN                  ");
			break;
		
		case IFM_ETH_CLC_OUT_TYPE:
			strcpy(str, "CLC_OUT                 ");
			break;
		
		case IFM_ETH_CLC_SUB_TYPE:
			strcpy(str, "CLC_SUB                 ");
			break;
		
		case IFM_ETH_CLC_LINE_TYPE:
			strcpy(str, "CLC_LINE                ");
			break;
		
		case IFM_ETH_SFP_TYPE:
			strcpy(str, "SFP                     ");
			break;

		case IFM_TRUNK_TYPE:
			strcpy(str, "TRUNK                   ");
			break;

		case IFM_TRUNK_SUBIF_TYPE:
			strcpy(str, "TRUNK_SUB               ");
			break;

		case IFM_E1_TYPE:
			strcpy(str, "E1                      ");
			break;

		case IFM_E1_SUB_TYPE:
			strcpy(str, "E1_SUB                  ");
			break;

		case IFM_BFD_TYPE:
			strcpy(str, "BFD                     ");
			break;

		case IFM_VS_2_TYPE:
			strcpy(str, "VS                      ");
			break;
		
		case IFM_LSP_2_TYPE:
			strcpy(str, "LSP                     ");
			break;
		
		case IFM_PW_2_TYPE:
			strcpy(str, "PW                      ");
			break;
		
		case IFM_VPLSPW_TYPE:
			strcpy(str, "VPLSPW                  ");
			break;

		case IFM_VLAN_OAM_TYPE:			
			strcpy(str, "VLAN_OAM                ");
			break;

		case IFM_CFM_MEP_TYPE:
			strcpy(str, "CFM_MEP                 ");
			break;
			
		default:
			return;
	}
}


/* 根据告警端口类型，将索引信息转换为字符串，供命令行显示 */
static void gpnAlmPortIndex2Str(optObjOrient *pPortIndex, char *str)
{
	if(NULL == str)
	{
		return;
	}
	
	struct ifm_usp usp;
	memset(&usp, 0, sizeof(struct ifm_usp));

	uint32_t portType = PortIndex_GetType(pPortIndex->portIndex);
	usp.unit = IFM_DEV_UNIT_GET(pPortIndex->devIndex);
	
	switch(portType)
	{
		case IFM_FUN_ETH_TYPE:
		case IFM_TUNNEL_TYPE:
			usp.slot = IFM_FUN_ETH_SLOT_DECOM(pPortIndex->portIndex);
			usp.port = IFM_FUN_ETH_PORTID_DECOM(pPortIndex->portIndex);
			sprintf (str, "%d/%d/%d               ", usp.unit, usp.slot, usp.port);
			break;

		case IFM_FUN_ETH_SUB_TYPE:
		case IFM_VUNI_TYPE:
			usp.slot = IFM_FUN_ETH_SLOT_DECOM(pPortIndex->portIndex);
			usp.port = IFM_FUN_ETH_PORTID_DECOM(pPortIndex->portIndex);
			usp.sub_port = pPortIndex->portIndex3;
			sprintf (str, "%d/%d/%d.%d             ", usp.unit, usp.slot, usp.port, usp.sub_port);
			break;

		case IFM_SOFT_TYPE:
		case IFM_PWE3E1_TYPE:
		case IFM_POWN48_TYPE:
		case IFM_STMN_TYPE:
		case IFM_PWE3STMN_TYPE:
		case IFM_EQU_TYPE:
		case IFM_ENV_TYPE:
		case IFM_FAN_TYPE:
		case IFM_POW220_TYPE:
		case IFM_ETH_CLC_IN_TYPE:
		case IFM_ETH_CLC_OUT_TYPE:
		case IFM_ETH_CLC_SUB_TYPE:
		case IFM_ETH_CLC_LINE_TYPE:
		case IFM_ETH_SFP_TYPE:
			usp.slot = IFM_SMP_SLOT_DECOM(pPortIndex->portIndex);
			usp.port = IFM_SMP_PORTID_DECOM(pPortIndex->portIndex);
			sprintf (str, "%d/%d/%d             ", usp.unit, usp.slot, usp.port);
			break;

		case IFM_VC4_TYPE:
			usp.slot = IFM_SMP_SLOT_DECOM(pPortIndex->portIndex);
			usp.port = IFM_SMP_PORTID_DECOM(pPortIndex->portIndex);
			sprintf (str, "%d/%d/%d.%d           ", usp.unit, usp.slot, usp.port, pPortIndex->portIndex3);
			break;

		case IFM_TRUNK_TYPE:
			usp.port = IFM_SMP_PORTID_DECOM(pPortIndex->portIndex);
			sprintf (str, "%d/%d                ", usp.unit, usp.port);
			break;

		case IFM_TRUNK_SUBIF_TYPE:
			usp.port = IFM_SMP_PORTID_DECOM(pPortIndex->portIndex);
			usp.sub_port = pPortIndex->portIndex3;
			sprintf (str, "%d/%d.%d              ", usp.unit, usp.port, usp.sub_port);
			break;

		case IFM_E1_TYPE:
			usp.slot = IFM_PDH_SLOT_DECOM(pPortIndex->portIndex);
			usp.port = IFM_PDH_PORTID_DECOM(pPortIndex->portIndex);
			sprintf (str, "%d/%d/%d             ", usp.unit, usp.slot, usp.port);
			break;

		case IFM_E1_SUB_TYPE:
			usp.slot = IFM_PDH_SLOT_DECOM(pPortIndex->portIndex);
			usp.port = IFM_PDH_PORTID_DECOM(pPortIndex->portIndex);
			usp.sub_port = pPortIndex->portIndex3;
			sprintf (str, "%d/%d/%d.%d           ", usp.unit, usp.slot, usp.port, usp.sub_port);
			break;

		case IFM_BFD_TYPE:
			sprintf (str, "%d/%d                 ", usp.unit, IFM_LAB_LAB_DECOM(pPortIndex->portIndex));
			break;

		case IFM_VS_2_TYPE:
		case IFM_LSP_2_TYPE:
		case IFM_PW_2_TYPE:
		case IFM_VPLSPW_TYPE:
			sprintf (str, "%d/%d                 ", usp.unit, pPortIndex->portIndex3);
			break;

		case IFM_VLAN_OAM_TYPE:
			sprintf (str, "%d/%d                 ", usp.unit, VLAN_PortIndex_GetVlan(pPortIndex->portIndex));
			break;

		case IFM_CFM_MEP_TYPE:
			sprintf (str, "%d/%d                 ", usp.unit, IFM_CFM_MEP_MEPID_DECOM(pPortIndex->portIndex));
			break;
			
		default:
			return;
	}
}



static int gpnShowCurrAlarmInfo(struct vty *vty, stSnmpAlmCurrDSTableInfo *pCurrTableInfo)
{
	if(NULL == pCurrTableInfo)
	{
		return -1;
	}

	struct ifm_usp usp;
	memset(&usp, 0, sizeof(struct ifm_usp));

	char show_buf[100];
	memset(show_buf, 0, sizeof(show_buf));
	char *pAlarmPort  = &show_buf[0];
	char *pAlarmType  = &show_buf[10];
	char *pAlarmName  = &show_buf[18];
	char *pAlarmIndex = &show_buf[44];
	char *pAlarmTime  = &show_buf[55];

	optObjOrient portIndex;
	portIndex.devIndex   = pCurrTableInfo->data.iCurrDevIndex;
	portIndex.portIndex  = pCurrTableInfo->data.iCurrPortIndex1;
	portIndex.portIndex3 = pCurrTableInfo->data.iCurrPortIndex2;
	portIndex.portIndex4 = pCurrTableInfo->data.iCurrPortIndex3;
	portIndex.portIndex5 = pCurrTableInfo->data.iCurrPortIndex4;
	portIndex.portIndex6 = pCurrTableInfo->data.iCurrPortIndex5;	

	gpnAlmPortType2Str(pCurrTableInfo->data.iCurrPortIndex1, pAlarmPort);
	sprintf(pAlarmType, "%x                           ", pCurrTableInfo->data.iCurrAlmType);
	sprintf(pAlarmName, "%s                           ", gpnAlmTypeNameStrGet(pCurrTableInfo->data.iCurrAlmType));
	
	gpnAlmPortIndex2Str(&portIndex, pAlarmIndex);

	//gpnGetLocalTimeStr(pCurrTableInfo->data.iAlmCurrThisProductTime, pAlarmTime);

	strftime(pAlarmTime, 22, "%Y-%m-%d %H:%M:%S", localtime(&pCurrTableInfo->data.iAlmCurrThisProductTime));

	vty_out (vty, "%s %s", show_buf, VTY_NEWLINE);
	
	return 0;
}


static int gpnShowHistAlarmInfo(struct vty *vty, stSnmpAlmHistDSTableInfo *pHistTableInfo)
{
	if(NULL == pHistTableInfo)
	{
		return -1;
	}

	struct ifm_usp usp;
	memset(&usp, 0, sizeof(struct ifm_usp));

	char show_buf[100];
	memset(show_buf, 0, sizeof(show_buf));
	char *pAlarmPort  = &show_buf[0];
	char *pAlarmType  = &show_buf[10];
	char *pAlarmName  = &show_buf[18];
	char *pAlarmIndex = &show_buf[44];
	char *pAlarmTime  = &show_buf[55];

	optObjOrient portIndex;
	portIndex.devIndex   = pHistTableInfo->data.iHistDevIndex;
	portIndex.portIndex  = pHistTableInfo->data.iHistPortIndex1;
	portIndex.portIndex3 = pHistTableInfo->data.iHistPortIndex2;
	portIndex.portIndex4 = pHistTableInfo->data.iHistPortIndex3;
	portIndex.portIndex5 = pHistTableInfo->data.iHistPortIndex4;
	portIndex.portIndex6 = pHistTableInfo->data.iHistPortIndex5;

	gpnAlmPortType2Str(pHistTableInfo->data.iHistPortIndex1, pAlarmPort);
	sprintf(pAlarmType, "%x                           ", pHistTableInfo->data.iHistAlmType);
	sprintf(pAlarmName, "%s                           ",   gpnAlmTypeNameStrGet(pHistTableInfo->data.iHistAlmType));	
	
	gpnAlmPortIndex2Str(&portIndex, pAlarmIndex);
	//gpnGetLocalTimeStr(pHistTableInfo->data.iAlmHistThisDisappearTime, pAlarmTime);

	strftime(pAlarmTime, 22, "%Y-%m-%d %H:%M:%S", localtime(&pHistTableInfo->data.iAlmHistThisProductTime));

	vty_out (vty, "%s %s", show_buf, VTY_NEWLINE);
	
	return 0;
}


static int gpnShowEventInfo(struct vty *vty, stSnmpEventDSTableInfo *pEventTableInfo)
{
	if(NULL == pEventTableInfo)
	{
		return -1;
	}

	struct ifm_usp usp;
	memset(&usp, 0, sizeof(struct ifm_usp));

	char show_buf[100];
	memset(show_buf, 0, sizeof(show_buf));

	char *pAlarmPort  = &show_buf[0];
	char *pEventType  = &show_buf[10];
	char *pEventName  = &show_buf[19];
	char *pEventIndex = &show_buf[45];
	char *pEventTime  = &show_buf[56];

	optObjOrient portIndex;
	portIndex.devIndex   = pEventTableInfo->data.iEventDevIndex;
	portIndex.portIndex  = pEventTableInfo->data.iEventPortIndex1;
	portIndex.portIndex3 = pEventTableInfo->data.iEventPortIndex2;
	portIndex.portIndex4 = pEventTableInfo->data.iEventPortIndex3;
	portIndex.portIndex5 = pEventTableInfo->data.iEventPortIndex4;
	portIndex.portIndex6 = pEventTableInfo->data.iEventPortIndex5;

	gpnAlmPortType2Str(pEventTableInfo->data.iEventPortIndex1, pAlarmPort);
	sprintf(pEventType, "%x                           ", pEventTableInfo->data.iEventAlmType);
	sprintf(pEventName, "%s                           ", gpnEvtTypeNameStrGet(pEventTableInfo->data.iEventAlmType));

	gpnAlmPortIndex2Str(&portIndex, pEventIndex);
	
	//gpnGetLocalTimeStr(pEventTableInfo->data.iEventTime, pEventTime);

	strftime(pEventTime, 22, "%Y-%m-%d %H:%M:%S", localtime(&pEventTableInfo->data.iEventTime));
	
	vty_out (vty, "%s %s", show_buf, VTY_NEWLINE);
	return 0;
}



static int gpnAlmShowRegisterInfo(struct vty *vty, objLogicDesc *pLocalPort)
{
	if(NULL == pLocalPort)
	{
		return -1;
	}

	struct ifm_usp usp;
	memset(&usp, 0, sizeof(struct ifm_usp));

	char show_buf[82];
	memset(show_buf, 0, sizeof(show_buf));
	
	char *pAlarmPort  = &show_buf[0];
	char *pAlarmIndex = &show_buf[16];

	gpnAlmPortType2Str(pLocalPort->portIndex, pAlarmPort);
	gpnAlmPortIndex2Str(pLocalPort, pAlarmIndex);

	vty_out (vty, "%s %s", show_buf, VTY_NEWLINE);
	
	return 0;
}





void alarm_show_alarm_code (struct vty *vty)
{
	gpnAlmCodeShow (vty);	
}

void alarm_show_curr_alarm (struct vty *vty)
{
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (&gSockMsgOut, 0, sizeof (gpnSockMsg));

	stSnmpAlmCurrDSTableInfo tableInfo;
	memset (&tableInfo, 0, sizeof (stSnmpAlmCurrDSTableInfo));

	int table_cnt = 0;

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType		= GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= GPN_ILLEGAL_PORT_INDEX;

	uint32_t ret = gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));

	vty_out (vty, "---------------------------------Current alarm---------------------------------%s", VTY_NEWLINE);
	vty_out (vty, "Type      AlmCode Name                      Index      Product Time %s", VTY_NEWLINE);
	vty_out (vty, "--------- ------- ------------------------- ---------- ------------------------%s", VTY_NEWLINE);
	
	while (GPN_ALM_SNMP_OK == ret)
	{
		if (gSockMsgOut.iMsgType == GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT_RSP)
		{
			memset (&tableInfo, 0, sizeof (stSnmpAlmCurrDSTableInfo));
			tableInfo.index.iAlmCurrDSIndex = gSockMsgOut.iMsgPara1;
			tableInfo.data.iCurrAlmType 	= gSockMsgOut.iMsgPara2;
			tableInfo.data.iCurrDevIndex 	= gSockMsgOut.iMsgPara3;
			tableInfo.data.iCurrPortIndex1 	= gSockMsgOut.iMsgPara4;
			tableInfo.data.iCurrPortIndex2 	= gSockMsgOut.iMsgPara5;
			tableInfo.data.iCurrPortIndex3 	= gSockMsgOut.iMsgPara6;
			tableInfo.data.iCurrPortIndex4 	= gSockMsgOut.iMsgPara7;
			tableInfo.data.iCurrPortIndex5 	= gSockMsgOut.iMsgPara8;
			tableInfo.data.iCurrAlmLevel	= gSockMsgOut.iMsgPara9;
			tableInfo.data.iAlmCurrProductCnt = gSockMsgOut.iMsgParaA;
			tableInfo.data.iAlmCurrFirstProductTime = gSockMsgOut.iMsgParaB;
			tableInfo.data.iAlmCurrThisProductTime = gSockMsgOut.iMsgParaC;

			gpnShowCurrAlarmInfo(vty, &tableInfo);
			table_cnt++;
		}		
		
		memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));
		memset (&gSockMsgOut, 0, sizeof (gpnSockMsg));

		gSockMsgIn.iIndex		= 0;
		gSockMsgIn.iMsgType		= GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT;
		gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
		gSockMsgIn.iDstId		= GPN_COMMM_ALM;
		gSockMsgIn.iMsgPara1	= tableInfo.index.iAlmCurrDSIndex;

		ret = gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
	}
	
	vty_out (vty, "--------- ------- ------------------------- ---------- ------------------------%s", VTY_NEWLINE);
	vty_out (vty, "Total number : %d %s", table_cnt, VTY_NEWLINE);
	vty_out (vty, "-------------------------------------------------------------------------------%s", VTY_NEWLINE);
}


void alarm_show_hist_alarm (struct vty *vty)
{
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (&gSockMsgOut, 0, sizeof (gpnSockMsg));

	stSnmpAlmHistDSTableInfo tableInfo;
	memset (&tableInfo, 0, sizeof (stSnmpAlmHistDSTableInfo));

	int table_cnt = 0;

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType		= GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= GPN_ILLEGAL_PORT_INDEX;

	uint32_t ret = gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));

	vty_out (vty, "---------------------------------History alarm---------------------------------%s", VTY_NEWLINE);
	vty_out (vty, "Type      AlmCode Name                      Index      Disappear Time %s", VTY_NEWLINE);
	vty_out (vty, "--------- ------- ------------------------- ---------- ------------------------%s", VTY_NEWLINE);
	
	while (GPN_ALM_SNMP_OK == ret)
	{
		if (gSockMsgOut.iMsgType == GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT_RSP)
		{
			memset (&tableInfo, 0, sizeof (stSnmpAlmHistDSTableInfo));
			tableInfo.index.iAlmHistDSIndex = gSockMsgOut.iMsgPara1;
			tableInfo.data.iHistAlmType 	= gSockMsgOut.iMsgPara2;
			tableInfo.data.iHistDevIndex 	= gSockMsgOut.iMsgPara3;
			tableInfo.data.iHistPortIndex1 	= gSockMsgOut.iMsgPara4;
			tableInfo.data.iHistPortIndex2 	= gSockMsgOut.iMsgPara5;
			tableInfo.data.iHistPortIndex3 	= gSockMsgOut.iMsgPara6;
			tableInfo.data.iHistPortIndex4 	= gSockMsgOut.iMsgPara7;
			tableInfo.data.iHistPortIndex5 	= gSockMsgOut.iMsgPara8;
			tableInfo.data.iHistAlmLevel	= gSockMsgOut.iMsgPara9;
			tableInfo.data.iAlmHistProductCnt = gSockMsgOut.iMsgParaA;
			tableInfo.data.iAlmHistFirstProductTime = gSockMsgOut.iMsgParaB;
			tableInfo.data.iAlmHistThisProductTime  = gSockMsgOut.iMsgParaC;
			tableInfo.data.iAlmHistThisDisappearTime = gSockMsgOut.iMsgParaD;

			gpnShowHistAlarmInfo(vty, &tableInfo);
			table_cnt++;
		}		
		
		memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));
		memset (&gSockMsgOut, 0, sizeof (gpnSockMsg));

		gSockMsgIn.iIndex		= 0;
		gSockMsgIn.iMsgType		= GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT;
		gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
		gSockMsgIn.iDstId		= GPN_COMMM_ALM;
		gSockMsgIn.iMsgPara1	= tableInfo.index.iAlmHistDSIndex;

		ret = gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
	}
	vty_out (vty, "--------- ------- ------------------------- ---------- ------------------------%s", VTY_NEWLINE);
	vty_out (vty, "Total number : %d %s", table_cnt, VTY_NEWLINE);
	vty_out (vty, "-------------------------------------------------------------------------------%s", VTY_NEWLINE);
}


void alarm_show_event (struct vty *vty)
{
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (&gSockMsgOut, 0, sizeof (gpnSockMsg));

	stSnmpEventDSTableInfo tableInfo;
	memset (&tableInfo, 0, sizeof (stSnmpEventDSTableInfo));

	int table_cnt = 0;

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType		= GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= GPN_ILLEGAL_PORT_INDEX;

	uint32_t ret = gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
	
	vty_out (vty, "--------------------------------------Event------------------------------------%s", VTY_NEWLINE);
	vty_out (vty, "Type      EvtCode  Name                      Index      Product Time %s", VTY_NEWLINE);
	vty_out (vty, "--------- -------- ------------------------- ---------- -----------------------%s", VTY_NEWLINE);
			
	while (GPN_ALM_SNMP_OK == ret)
	{
		if (gSockMsgOut.iMsgType == GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT_RSP)
		{
			memset (&tableInfo, 0, sizeof (stSnmpEventDSTableInfo));
			tableInfo.index.iEventDSIndex 	= gSockMsgOut.iMsgPara1;
			tableInfo.data.iEventAlmType 	= gSockMsgOut.iMsgPara2;
			tableInfo.data.iEventDevIndex   = gSockMsgOut.iMsgPara3;
			tableInfo.data.iEventPortIndex1 = gSockMsgOut.iMsgPara4;
			tableInfo.data.iEventPortIndex2 = gSockMsgOut.iMsgPara5;
			tableInfo.data.iEventPortIndex3 = gSockMsgOut.iMsgPara6;
			tableInfo.data.iEventPortIndex4 = gSockMsgOut.iMsgPara7;
			tableInfo.data.iEventPortIndex5 = gSockMsgOut.iMsgPara8;
			tableInfo.data.iEventAlmLevel	= gSockMsgOut.iMsgPara9;
			tableInfo.data.iEventDetail 	= gSockMsgOut.iMsgParaA;
			tableInfo.data.iEventTime 		= gSockMsgOut.iMsgParaB;

			gpnShowEventInfo(vty, &tableInfo);
			table_cnt++;
		}		
		
		memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));
		memset (&gSockMsgOut, 0, sizeof (gpnSockMsg));

		gSockMsgIn.iIndex		= 0;
		gSockMsgIn.iMsgType		= GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT;
		gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
		gSockMsgIn.iDstId		= GPN_COMMM_ALM;
		gSockMsgIn.iMsgPara1	= tableInfo.index.iEventDSIndex;

		ret = gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
	}
	vty_out (vty, "--------- -------- ------------------------- ---------- -----------------------%s", VTY_NEWLINE);
	vty_out (vty, "Total number : %d %s", table_cnt, VTY_NEWLINE);
	vty_out (vty, "-------------------------------------------------------------------------------%s", VTY_NEWLINE);
}


void alarm_show_alarm_mainType (struct vty *vty)
{
}


void alarm_show_alarm_subType (struct vty *vty)
{
}



void alarm_show_register_info(struct vty *vty)
{
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stAlmPreScanQuen *pAlmPreScanNode;
	stAlmPreScanIndex *pAlmPreScanIndex;
	stAlmLocalNode *pAlmLocalPort;
	UINT32 sysPortTypeNum;
	UINT32 hashKey;
	UINT32 slotId;
	UINT32 portType_temp = 0;
	
	UINT32 itemCnt = 0;

	vty_out (vty, "----------------------Register info-------------------%s", VTY_NEWLINE);
	vty_out (vty, "AlarmPort       Index                                 %s", VTY_NEWLINE);
	vty_out (vty, "--------------- --------------------------------------%s", VTY_NEWLINE);
	
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen)); 
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		for(slotId=0; slotId<=EQU_SLOT_MAX_ID; slotId++)
		{
			pAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
			for(hashKey=0; hashKey<GPN_ALM_PRESCAN_HASH; hashKey++)
			{
				/* compare with local port index */
				pAlmPreScanIndex = (stAlmPreScanIndex *)listFirst(&(pAlmPreScanNode->almPreScanQuen[hashKey]));
				 
				while(pAlmPreScanIndex != NULL)
				{
					/* get next locaPortNode */
					portType_temp = PortIndex_GetType(pAlmPreScanIndex->pAlmLocalPort->localPort.portIndex);
					if(pPortTpToScanTp->portType == portType_temp)
					{
						pAlmLocalPort = pAlmPreScanIndex->pAlmLocalPort;
						gpnAlmShowRegisterInfo(vty, &pAlmLocalPort->localPort);
						itemCnt++;
					}
					
					pAlmPreScanIndex = (stAlmPreScanIndex *)listNext((NODE *)(pAlmPreScanIndex));
				}
			}
		}
		
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	vty_out (vty, "--------------- --------------------------------------%s", VTY_NEWLINE);
	vty_out (vty, "Total number  : %d %s", itemCnt, VTY_NEWLINE);
	vty_out (vty, "------------------------------------------------------%s", VTY_NEWLINE);
}

