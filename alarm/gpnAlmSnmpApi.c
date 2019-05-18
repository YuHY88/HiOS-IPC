/**********************************************************
* file name: gpnAlmSnmpApi.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-09-18
* function: 
*    define alarm snmp API
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_SNMP_API_C_
#define _GPN_ALM_SNMP_API_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <lib/syslog.h>
#include <lib/log.h>

#include "gpnAlmTypeStruct.h"
#include "gpnAlmSnmpApi.h"
#include "gpnAlmDataStructApi.h"

#include "socketComm/gpnGlobalPortIndexDef.h"
#include "socketComm/gpnSockAlmMsgDef.h"
#include "gpnAlmSnmpApi.h"
#include "testDataStructure.h"
#include "gpnAlmDebug.h"

/* first : table line get/getNext/modify */
static UINT32 gpnAlmSnmpApiPortMonGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	objLogicDesc localPIndex;
	UINT32 enState;

	/* fix default return */
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iIndex;
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_RSP;
	pgpnAlmMsgSpOut->iMsgPara1 	= pgpnAlmMsgSpIn->iMsgPara1;
	pgpnAlmMsgSpOut->iMsgPara2 	= pgpnAlmMsgSpIn->iMsgPara2;
	pgpnAlmMsgSpOut->iMsgPara3 	= pgpnAlmMsgSpIn->iMsgPara3;
	pgpnAlmMsgSpOut->iMsgPara4 	= pgpnAlmMsgSpIn->iMsgPara4;
	pgpnAlmMsgSpOut->iMsgPara5 	= pgpnAlmMsgSpIn->iMsgPara5;
	pgpnAlmMsgSpOut->iMsgPara6 	= pgpnAlmMsgSpIn->iMsgPara6;
	pgpnAlmMsgSpOut->msgCellLen	= 0;

	/* make portIndex */
	//localPIndex.devIndex		= DeviceIndex_Create( 1, 0 );/* why fix  ??? */
	localPIndex.devIndex		= pgpnAlmMsgSpIn->iMsgPara6;
	localPIndex.portIndex		= pgpnAlmMsgSpIn->iMsgPara1;
	localPIndex.portIndex3		= pgpnAlmMsgSpIn->iMsgPara2;
	localPIndex.portIndex4		= pgpnAlmMsgSpIn->iMsgPara3;
	localPIndex.portIndex5		= pgpnAlmMsgSpIn->iMsgPara4;
	localPIndex.portIndex6		= pgpnAlmMsgSpIn->iMsgPara5;

	/* get port mon state */
	if(GPN_ALM_SNMP_OK == gpnAlmDSApiPortMonStateGet(&localPIndex, &enState))
	{
		pgpnAlmMsgSpOut->iMsgPara7 = enState;
		
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		zlog_err( "%s : get port(%08x|%08x|%08x|%08x|%08x|%08x) mon state error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6);

		pgpnAlmMsgSpOut->iMsgPara7 	= GPN_ALM_SNMP_DISABLE;
		
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiPortMonGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	objLogicDesc localPIndex;
	objLogicDesc nextLocalPIndex;
	UINT32 enState;

	/* fix default return */
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iIndex;
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;

	/* make portIndex */
	//localPIndex.devIndex		= DeviceIndex_Create( 1, 0 );/* why fix  ??? */
	localPIndex.devIndex		= pgpnAlmMsgSpIn->iMsgPara6;
	localPIndex.portIndex		= pgpnAlmMsgSpIn->iMsgPara1;
	localPIndex.portIndex3		= pgpnAlmMsgSpIn->iMsgPara2;
	localPIndex.portIndex4		= pgpnAlmMsgSpIn->iMsgPara3;
	localPIndex.portIndex5		= pgpnAlmMsgSpIn->iMsgPara4;
	localPIndex.portIndex6		= pgpnAlmMsgSpIn->iMsgPara5;
	
	/* get port mon state */
	if(GPN_ALM_SNMP_OK == gpnAlmDSApiPortMonStateGetNext(&localPIndex, &nextLocalPIndex, &enState))
	{
		pgpnAlmMsgSpOut->iMsgPara1 = nextLocalPIndex.portIndex;
		pgpnAlmMsgSpOut->iMsgPara2 = nextLocalPIndex.portIndex3;
		pgpnAlmMsgSpOut->iMsgPara3 = nextLocalPIndex.portIndex4;
		pgpnAlmMsgSpOut->iMsgPara4 = nextLocalPIndex.portIndex5;
		pgpnAlmMsgSpOut->iMsgPara5 = nextLocalPIndex.portIndex6;
		pgpnAlmMsgSpOut->iMsgPara6 = nextLocalPIndex.devIndex;
		pgpnAlmMsgSpOut->iMsgPara7 = enState;
		
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		zlog_debug(ALARM_DBG_GET_NEXT, "%s : get port(%08x|%08x|%08x|%08x|%08x|%08x)'s next mon state error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6);

		pgpnAlmMsgSpOut->iMsgPara1 	= GPN_ILLEGAL_PORT_INDEX;
		pgpnAlmMsgSpOut->iMsgPara2 	= GPN_ILLEGAL_PORT_INDEX;
		pgpnAlmMsgSpOut->iMsgPara3 	= GPN_ILLEGAL_PORT_INDEX;
		pgpnAlmMsgSpOut->iMsgPara4 	= GPN_ILLEGAL_PORT_INDEX;
		pgpnAlmMsgSpOut->iMsgPara5 	= GPN_ILLEGAL_PORT_INDEX;
		pgpnAlmMsgSpOut->iMsgPara6 	= GPN_ILLEGAL_DEVICE_INDEX;
		pgpnAlmMsgSpOut->iMsgPara7 	= GPN_ALM_SNMP_DISABLE;
			
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiPortMonModify(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	objLogicDesc localPIndex;
	UINT32 enState;

	/* make portIndex */
	//localPIndex.devIndex		= DeviceIndex_Create( 1, 0 );/* why fix  ??? */
	localPIndex.devIndex		= pgpnAlmMsgSpIn->iMsgPara6;
	localPIndex.portIndex		= pgpnAlmMsgSpIn->iMsgPara1;
	localPIndex.portIndex3		= pgpnAlmMsgSpIn->iMsgPara2;
	localPIndex.portIndex4		= pgpnAlmMsgSpIn->iMsgPara3;
	localPIndex.portIndex5		= pgpnAlmMsgSpIn->iMsgPara4;
	localPIndex.portIndex6		= pgpnAlmMsgSpIn->iMsgPara5;
	
	enState						= pgpnAlmMsgSpIn->iMsgPara7;

	/* get port mon state */
	if(GPN_ALM_SNMP_OK != gpnAlmDSApiPortMonStateCfg(&localPIndex, enState))
	{
		zlog_err( "%s : get port(%08x|%08x|%08x|%08x|%08x|%08x) mon state cfg error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6);
		
		return GPN_ALM_SNMP_ERR;
	}
	
	return GPN_ALM_SNMP_OK;
}

static UINT32 gpnAlmSnmpApiAlmAttribGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	objLogicDesc localPIndex;
	stDSAlmAttri almAttri;
	UINT32 almSubType;
	UINT32 reVal;
	
	/* fix default return */
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iIndex;
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET_RSP;
	pgpnAlmMsgSpOut->iMsgPara1 	= pgpnAlmMsgSpIn->iMsgPara1;
	pgpnAlmMsgSpOut->iMsgPara2 	= pgpnAlmMsgSpIn->iMsgPara2;
	pgpnAlmMsgSpOut->iMsgPara3 	= pgpnAlmMsgSpIn->iMsgPara3;
	pgpnAlmMsgSpOut->iMsgPara4 	= pgpnAlmMsgSpIn->iMsgPara4;
	pgpnAlmMsgSpOut->iMsgPara5 	= pgpnAlmMsgSpIn->iMsgPara5;
	pgpnAlmMsgSpOut->iMsgPara6 	= pgpnAlmMsgSpIn->iMsgPara6;
	pgpnAlmMsgSpOut->iMsgPara7 	= pgpnAlmMsgSpIn->iMsgPara7;
	pgpnAlmMsgSpOut->iMsgPara8 	= pgpnAlmMsgSpIn->iMsgPara8;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	/* make Index */
	almSubType					= pgpnAlmMsgSpIn->iMsgPara1;
	localPIndex.portIndex		= pgpnAlmMsgSpIn->iMsgPara2;
	localPIndex.portIndex3		= pgpnAlmMsgSpIn->iMsgPara3;
	localPIndex.portIndex4		= pgpnAlmMsgSpIn->iMsgPara4;
	localPIndex.portIndex5		= pgpnAlmMsgSpIn->iMsgPara5;
	localPIndex.portIndex6		= pgpnAlmMsgSpIn->iMsgPara6;
	//localPIndex.devIndex		= DeviceIndex_Create( 1, 0 );/* why fix  ??? */
	localPIndex.devIndex		= pgpnAlmMsgSpIn->iMsgPara7;

	/* get alarm type attribute */
	reVal = gpnAlmDSApiAttributeTableGet(&localPIndex, almSubType, &almAttri);
	if(reVal == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara8 = almAttri.bitMark;
		pgpnAlmMsgSpOut->iMsgPara9 = almAttri.rank;
		pgpnAlmMsgSpOut->iMsgParaA = almAttri.mask;
		pgpnAlmMsgSpOut->iMsgParaB = almAttri.filt;
		pgpnAlmMsgSpOut->iMsgParaC = almAttri.rept;
		pgpnAlmMsgSpOut->iMsgParaD = almAttri.recd;
		pgpnAlmMsgSpOut->iMsgParaE = almAttri.revs;
		
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		zlog_err( "%s : get almType(%08x) port(%08x|%08x|%08x|%08x|%08x|%08x)'s attribute error\n\r",\
			__FUNCTION__, almSubType,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6);
		
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiAlmAttribGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	objLogicDesc localPIndex;
	objLogicDesc nextLocalPIndex;
	stDSAlmAttri nextAlmAttri;
	UINT32 almSubType;
	UINT32 nextSubType;
	UINT32 reVal;
	
	/* fix default return */
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iIndex;
	//pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET_NEXT_RSP;
	pgpnAlmMsgSpOut->msgCellLen = 0;
	
	/* make Index */
	almSubType					= pgpnAlmMsgSpIn->iMsgPara1;
	localPIndex.portIndex		= pgpnAlmMsgSpIn->iMsgPara2;
	localPIndex.portIndex3		= pgpnAlmMsgSpIn->iMsgPara3;
	localPIndex.portIndex4		= pgpnAlmMsgSpIn->iMsgPara4;
	localPIndex.portIndex5		= pgpnAlmMsgSpIn->iMsgPara5;
	localPIndex.portIndex6		= pgpnAlmMsgSpIn->iMsgPara6;
	//localPIndex.devIndex		= DeviceIndex_Create( 1, 0 );/* why fix  ??? */
	localPIndex.devIndex		= pgpnAlmMsgSpIn->iMsgPara7;

	/* get alarm type attribute */
	reVal = gpnAlmDSApiAttributeTableGetNext(&localPIndex,\
		almSubType, &nextLocalPIndex, &nextSubType, &nextAlmAttri);	
	if(reVal == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = nextSubType;
		pgpnAlmMsgSpOut->iMsgPara2 = nextLocalPIndex.portIndex;
		pgpnAlmMsgSpOut->iMsgPara3 = nextLocalPIndex.portIndex3;
		pgpnAlmMsgSpOut->iMsgPara4 = nextLocalPIndex.portIndex4;
		pgpnAlmMsgSpOut->iMsgPara5 = nextLocalPIndex.portIndex5;
		pgpnAlmMsgSpOut->iMsgPara6 = nextLocalPIndex.portIndex6;
		pgpnAlmMsgSpOut->iMsgPara7 = nextLocalPIndex.devIndex;
		pgpnAlmMsgSpOut->iMsgPara8 = nextAlmAttri.bitMark;
		pgpnAlmMsgSpOut->iMsgPara9 = nextAlmAttri.rank;
		pgpnAlmMsgSpOut->iMsgParaA = nextAlmAttri.mask;
		pgpnAlmMsgSpOut->iMsgParaB = nextAlmAttri.filt;
		pgpnAlmMsgSpOut->iMsgParaC = nextAlmAttri.rept;
		pgpnAlmMsgSpOut->iMsgParaD = nextAlmAttri.recd;
		pgpnAlmMsgSpOut->iMsgParaE = nextAlmAttri.revs;
		
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		zlog_debug(ALARM_DBG_GET_NEXT, "%s : get almType(%08x) port(%08x-%08x|%08x|%08x|%08x|%08x)'s attribute error\n\r",\
			__FUNCTION__, almSubType,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6);
		
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiAlmAttribModify(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	objLogicDesc localPIndex;
	stDSAlmAttri dsAttri;
	UINT32 almSubType;
	UINT32 reVal;

	/* make Index */
	almSubType					= pgpnAlmMsgSpIn->iMsgPara1;
	localPIndex.portIndex		= pgpnAlmMsgSpIn->iMsgPara2;
	localPIndex.portIndex3		= pgpnAlmMsgSpIn->iMsgPara3;
	localPIndex.portIndex4		= pgpnAlmMsgSpIn->iMsgPara4;
	localPIndex.portIndex5		= pgpnAlmMsgSpIn->iMsgPara5;
	localPIndex.portIndex6 		= pgpnAlmMsgSpIn->iMsgPara6;
	//localPIndex.devIndex		= DeviceIndex_Create( 1, 0 );/* why fix  ??? */
	localPIndex.devIndex		= pgpnAlmMsgSpIn->iMsgPara7;

	/* make sub attribute */
	dsAttri.bitMark = pgpnAlmMsgSpIn->iMsgPara8;
	dsAttri.rank = pgpnAlmMsgSpIn->iMsgPara9;
	dsAttri.mask = pgpnAlmMsgSpIn->iMsgParaA;
	dsAttri.filt = pgpnAlmMsgSpIn->iMsgParaB;
	dsAttri.rept = pgpnAlmMsgSpIn->iMsgParaC;
	dsAttri.recd = pgpnAlmMsgSpIn->iMsgParaD;
	dsAttri.revs = pgpnAlmMsgSpIn->iMsgParaE;

	/* master location index */
	reVal = gpnAlmDSApiAlmAttribModify(&localPIndex, almSubType, &dsAttri);
	if(reVal != GPN_ALM_SNMP_OK)
	{
		zlog_err( "%s : modify port(%08x-%08x|%08x|%08x|%08x|%08x) "
			"almType(%08x) almAttri(%d %d %d %d %d %d %08x) error\n\r",\
			__FUNCTION__, almSubType,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			dsAttri.rank, dsAttri.mask, dsAttri.filt,\
			dsAttri.rept, dsAttri.recd, dsAttri.revs,\
			dsAttri.bitMark);
		
		return GPN_ALM_SNMP_ERR;
	}

	return GPN_ALM_SNMP_OK;
}

static UINT32 gpnAlmSnmpApiCurrAlmGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	stDSCurrAlm currAlm;
	UINT32 currIndex;
	UINT32 reVal;
	
	/* fix default return */
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iIndex;
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_GET_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	/* make Index */
	currIndex					= pgpnAlmMsgSpIn->iMsgPara1;

	/* get alarm type attribute */
	reVal = gpnAlmDSApiCurrAlmGet(currIndex, &currAlm);
	if(reVal != GPN_ALM_SNMP_OK)
	{
		zlog_debug(ALARM_DBG_GET, "%s(%d) : get currAlmIndex(%08x) error\n\r",\
			__FUNCTION__, __LINE__, currIndex);
		return GPN_ALM_SNMP_ERR;
	}
	
	pgpnAlmMsgSpOut->iMsgPara1 = currAlm.index;
	pgpnAlmMsgSpOut->iMsgPara2 = currAlm.almType;
	pgpnAlmMsgSpOut->iMsgPara3 = currAlm.devIndex;
	pgpnAlmMsgSpOut->iMsgPara4 = currAlm.ifIndex;
	pgpnAlmMsgSpOut->iMsgPara5 = currAlm.ifIndex2;
	pgpnAlmMsgSpOut->iMsgPara6 = currAlm.ifIndex3;
	pgpnAlmMsgSpOut->iMsgPara7 = currAlm.ifIndex4;
	pgpnAlmMsgSpOut->iMsgPara8 = currAlm.ifIndex5;
	pgpnAlmMsgSpOut->iMsgPara9 = currAlm.level;
	pgpnAlmMsgSpOut->iMsgParaA = currAlm.count;
	pgpnAlmMsgSpOut->iMsgParaB = currAlm.firstTime;
	pgpnAlmMsgSpOut->iMsgParaC = currAlm.thisTime;
	pgpnAlmMsgSpOut->iMsgParaD = currAlm.suffixLen;

	pgpnAlmMsgSpOut->msgCellLen = currAlm.suffixLen;
	if(outLen < (sizeof(gpnSockMsg) + currAlm.suffixLen))
	{
		zlog_debug(ALARM_DBG_GET, "%s(%d) : get currAlmIndex(%08x) error\n\r",\
			__FUNCTION__, __LINE__, currIndex);
		return GPN_ALM_SNMP_ERR;
	}

	memcpy((char *)(&(pgpnAlmMsgSpOut->msgCellLen) + 1), currAlm.almSuffix, currAlm.suffixLen);
	
	return GPN_ALM_SNMP_OK;
}
static UINT32 gpnAlmSnmpApiCurrAlmGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	stDSCurrAlm currAlm;
	UINT32 currIndex;
	UINT32 nextIndex;
	UINT32 reVal;
	
	/* fix default return */
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT_RSP;
	
	/* make Index */
	currIndex					= pgpnAlmMsgSpIn->iMsgPara1;

	/* get alarm type attribute */
	reVal = gpnAlmDSApiCurrAlmGetNext(currIndex, &nextIndex, &currAlm);
	if(reVal != GPN_ALM_SNMP_OK)
	{
		zlog_debug(ALARM_DBG_GET_NEXT,  "%s(%d) : get currAlmIndex(%08x) error\n\r",\
			__FUNCTION__, __LINE__, currIndex);
		return GPN_ALM_SNMP_ERR;
	}
	
	pgpnAlmMsgSpOut->iMsgPara1 = nextIndex;
	pgpnAlmMsgSpOut->iMsgPara2 = currAlm.almType;
	pgpnAlmMsgSpOut->iMsgPara3 = currAlm.devIndex;
	pgpnAlmMsgSpOut->iMsgPara4 = currAlm.ifIndex;
	pgpnAlmMsgSpOut->iMsgPara5 = currAlm.ifIndex2;
	pgpnAlmMsgSpOut->iMsgPara6 = currAlm.ifIndex3;
	pgpnAlmMsgSpOut->iMsgPara7 = currAlm.ifIndex4;
	pgpnAlmMsgSpOut->iMsgPara8 = currAlm.ifIndex5;
	pgpnAlmMsgSpOut->iMsgPara9 = currAlm.level;
	pgpnAlmMsgSpOut->iMsgParaA = currAlm.count;
	pgpnAlmMsgSpOut->iMsgParaB = currAlm.firstTime;
	pgpnAlmMsgSpOut->iMsgParaC = currAlm.thisTime;
	pgpnAlmMsgSpOut->iMsgParaD = currAlm.suffixLen;

	pgpnAlmMsgSpOut->msgCellLen = currAlm.suffixLen;
	if(outLen < (sizeof(gpnSockMsg) + currAlm.suffixLen))
	{
		zlog_debug(ALARM_DBG_GET_NEXT, "%s(%d) : get currAlmIndex(%08x) error\n\r",\
			__FUNCTION__, __LINE__, currIndex);
		return GPN_ALM_SNMP_ERR;
	}
	
	memcpy((char *)(&(pgpnAlmMsgSpOut->msgCellLen) + 1), currAlm.almSuffix, currAlm.suffixLen);
	
	return GPN_ALM_SNMP_OK;
}
static UINT32 gpnAlmSnmpApiCurrAlmModify(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 currIndex;
	UINT32 reVal;

	/* make Index */
	currIndex					= pgpnAlmMsgSpIn->iMsgPara1;

	/* clear curr alarm product count */
	reVal = gpnAlmDSApiCurrAlmProductCountClear(currIndex);
	if(reVal != GPN_ALM_SNMP_OK)
	{
		zlog_debug(ALARM_DBG_SET, "%s(%d) : clear currAlmIndex(%08x) product count error\n\r",\
			__FUNCTION__, __LINE__, currIndex);
		return GPN_ALM_SNMP_ERR;
	}
	
	return GPN_ALM_SNMP_OK;
}
static UINT32 gpnAlmSnmpApiHistAlmGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	stDSHistAlm histAlm;
	UINT32 histIndex;
	UINT32 reVal;
	
	/* fix default return */
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iIndex;
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_GET_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	/* make Index */
	histIndex					= pgpnAlmMsgSpIn->iMsgPara1;

	/* get alarm type attribute */
	reVal = gpnAlmDSApiHistAlmGet(histIndex, &histAlm);
	if(reVal != GPN_ALM_SNMP_OK)
	{
		zlog_debug(ALARM_DBG_GET, "%s(%d) : get histAlmIndex(%08x) error\n\r",\
			__FUNCTION__, __LINE__, histIndex);
		return GPN_ALM_SNMP_ERR;
	}
	
	pgpnAlmMsgSpOut->iMsgPara1 = histAlm.index;
	pgpnAlmMsgSpOut->iMsgPara2 = histAlm.almType;
	pgpnAlmMsgSpOut->iMsgPara3 = histAlm.devIndex;
	pgpnAlmMsgSpOut->iMsgPara4 = histAlm.ifIndex;
	pgpnAlmMsgSpOut->iMsgPara5 = histAlm.ifIndex2;
	pgpnAlmMsgSpOut->iMsgPara6 = histAlm.ifIndex3;
	pgpnAlmMsgSpOut->iMsgPara7 = histAlm.ifIndex4;
	pgpnAlmMsgSpOut->iMsgPara8 = histAlm.ifIndex5;
	pgpnAlmMsgSpOut->iMsgPara9 = histAlm.level;
	pgpnAlmMsgSpOut->iMsgParaA = histAlm.count;
	pgpnAlmMsgSpOut->iMsgParaB = histAlm.firstTime;
	pgpnAlmMsgSpOut->iMsgParaC = histAlm.thisTime;
	pgpnAlmMsgSpOut->iMsgParaD = histAlm.disapTime;
	pgpnAlmMsgSpOut->iMsgParaE = histAlm.suffixLen;

	pgpnAlmMsgSpOut->msgCellLen = histAlm.suffixLen;
	if(outLen < (sizeof(gpnSockMsg) + histAlm.suffixLen))
	{
		zlog_debug(ALARM_DBG_GET, "%s(%d) : get currAlmIndex(%08x) error\n\r",\
			__FUNCTION__, __LINE__, histIndex);
		return GPN_ALM_SNMP_ERR;
	}

	memcpy((char *)(&(pgpnAlmMsgSpOut->msgCellLen) + 1), histAlm.almSuffix, histAlm.suffixLen);
	return GPN_ALM_SNMP_OK;
}
static UINT32 gpnAlmSnmpApiHistAlmGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	stDSHistAlm histAlm;
	UINT32 histIndex;
	UINT32 nextIndex;
	UINT32 reVal;
	
	/* fix default return */
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iIndex;
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT_RSP;
	
	/* make Index */
	histIndex					= pgpnAlmMsgSpIn->iMsgPara1;

	/* get alarm type attribute */
	reVal = gpnAlmDSApiHistAlmGetNext(histIndex, &nextIndex, &histAlm);
	if(reVal != GPN_ALM_SNMP_OK)
	{
		zlog_debug(ALARM_DBG_GET_NEXT, "%s(%d) : get currAlmIndex(%08x) error\n\r",\
			__FUNCTION__, __LINE__, histIndex);
		return GPN_ALM_SNMP_ERR;
	}
	
	pgpnAlmMsgSpOut->iMsgPara1 = nextIndex;
	pgpnAlmMsgSpOut->iMsgPara2 = histAlm.almType;
	pgpnAlmMsgSpOut->iMsgPara3 = histAlm.devIndex;
	pgpnAlmMsgSpOut->iMsgPara4 = histAlm.ifIndex;
	pgpnAlmMsgSpOut->iMsgPara5 = histAlm.ifIndex2;
	pgpnAlmMsgSpOut->iMsgPara6 = histAlm.ifIndex3;
	pgpnAlmMsgSpOut->iMsgPara7 = histAlm.ifIndex4;
	pgpnAlmMsgSpOut->iMsgPara8 = histAlm.ifIndex5;
	pgpnAlmMsgSpOut->iMsgPara9 = histAlm.level;
	pgpnAlmMsgSpOut->iMsgParaA = histAlm.count;
	pgpnAlmMsgSpOut->iMsgParaB = histAlm.firstTime;
	pgpnAlmMsgSpOut->iMsgParaC = histAlm.thisTime;
	pgpnAlmMsgSpOut->iMsgParaD = histAlm.disapTime;
	pgpnAlmMsgSpOut->iMsgParaE = histAlm.suffixLen;

	pgpnAlmMsgSpOut->msgCellLen = histAlm.suffixLen;
	if(outLen < (sizeof(gpnSockMsg) + histAlm.suffixLen))
	{
		zlog_debug(ALARM_DBG_GET_NEXT, "%s(%d) : get histAlmIndex(%08x) error\n\r",\
			__FUNCTION__, __LINE__, histIndex);
		return GPN_ALM_SNMP_ERR;
	}
	
	memcpy((char *)(&(pgpnAlmMsgSpOut->msgCellLen) + 1), histAlm.almSuffix, histAlm.suffixLen);
	return GPN_ALM_SNMP_OK;
}
static UINT32 gpnAlmSnmpApiEventGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	stDSEvent event;
	UINT32 evtIndex;
	UINT32 reVal;
	
	/* fix default return */
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iIndex;
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_EVT_DATA_DB_GET_RSP;
	pgpnAlmMsgSpOut->msgCellLen	= 0;
	
	/* make Index */
	evtIndex					= pgpnAlmMsgSpIn->iMsgPara1;

	/* get alarm type attribute */
	reVal = gpnAlmDSApiEventGet(evtIndex, &event);
	if(reVal != GPN_ALM_SNMP_OK)
	{
		zlog_debug(ALARM_DBG_GET, "%s(%d) : get eventIndex(%08x) error\n\r",\
			__FUNCTION__, __LINE__, evtIndex);
		return GPN_ALM_SNMP_ERR;
	}
	
	pgpnAlmMsgSpOut->iMsgPara1 = event.index;
	pgpnAlmMsgSpOut->iMsgPara2 = event.eventType;
	pgpnAlmMsgSpOut->iMsgPara3 = event.ifIndex;
	pgpnAlmMsgSpOut->iMsgPara4 = event.ifIndex2;
	pgpnAlmMsgSpOut->iMsgPara5 = event.ifIndex3;
	pgpnAlmMsgSpOut->iMsgPara6 = event.ifIndex4;
	pgpnAlmMsgSpOut->iMsgPara7 = event.ifIndex5;
	pgpnAlmMsgSpOut->iMsgPara8 = event.level;
	pgpnAlmMsgSpOut->iMsgPara9 = event.detail;
	pgpnAlmMsgSpOut->iMsgParaA = event.thisTime;
	pgpnAlmMsgSpOut->iMsgParaB = event.suffixLen;

	pgpnAlmMsgSpOut->msgCellLen = event.suffixLen;
	if(outLen < (sizeof(gpnSockMsg) + event.suffixLen))
	{
		zlog_debug(ALARM_DBG_GET, "%s(%d) : get currAlmIndex(%08x) error\n\r",\
			__FUNCTION__, __LINE__, evtIndex);
		return GPN_ALM_SNMP_ERR;
	}

	memcpy((char *)(&(pgpnAlmMsgSpOut->msgCellLen) + 1), event.evtSuffix, event.suffixLen);
	return GPN_ALM_SNMP_OK;
}
static UINT32 gpnAlmSnmpApiEventGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	stDSEvent event;
	UINT32 evtIndex;
	UINT32 nextIndex;
	UINT32 reVal;
	
	/* fix default return */
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iIndex;
	pgpnAlmMsgSpOut->iIndex		= pgpnAlmMsgSpOut->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iDstId		= GPN_COMMM_STAT;
	pgpnAlmMsgSpOut->iMsgType	= GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT_RSP;
	
	/* make Index */
	evtIndex					= pgpnAlmMsgSpIn->iMsgPara1;

	/* get alarm type attribute */
	reVal = gpnAlmDSApiEventGetNext(evtIndex, &nextIndex, &event);
	if(reVal != GPN_ALM_SNMP_OK)
	{
		zlog_debug(ALARM_DBG_GET_NEXT, "%s(%d) : get evnetIndex(%08x) error\n\r",\
			__FUNCTION__, __LINE__, evtIndex);
		return GPN_ALM_SNMP_ERR;
	}
	
	pgpnAlmMsgSpOut->iMsgPara1 = nextIndex;
	pgpnAlmMsgSpOut->iMsgPara2 = event.eventType;
	pgpnAlmMsgSpOut->iMsgPara3 = event.devIndex;
	pgpnAlmMsgSpOut->iMsgPara4 = event.ifIndex;
	pgpnAlmMsgSpOut->iMsgPara5 = event.ifIndex2;
	pgpnAlmMsgSpOut->iMsgPara6 = event.ifIndex3;
	pgpnAlmMsgSpOut->iMsgPara7 = event.ifIndex4;
	pgpnAlmMsgSpOut->iMsgPara8 = event.ifIndex5;
	pgpnAlmMsgSpOut->iMsgPara9 = event.level;
	pgpnAlmMsgSpOut->iMsgParaA = event.detail;
	pgpnAlmMsgSpOut->iMsgParaB = event.thisTime;
	pgpnAlmMsgSpOut->iMsgParaC = event.suffixLen;

	pgpnAlmMsgSpOut->msgCellLen = event.suffixLen;
	if(outLen < (sizeof(gpnSockMsg) + event.suffixLen))
	{
		zlog_debug(ALARM_DBG_GET_NEXT, "%s(%d) : get currAlmIndex(%08x) error\n\r",\
			__FUNCTION__, __LINE__, evtIndex);
		return GPN_ALM_SNMP_ERR;
	}
	
	memcpy((char *)(&(pgpnAlmMsgSpOut->msgCellLen) + 1), event.evtSuffix, event.suffixLen);
	return GPN_ALM_SNMP_OK;
}

/* second : scalar get/set */
static UINT32 gpnAlmSnmpApiRiseDelayGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 rDelay;
	
	/* assert */

	/* set default retrun */
	pgpnAlmMsgSpOut->iIndex = pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iMsgCtrl= pgpnAlmMsgSpIn->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId = pgpnAlmMsgSpIn->iDstId;
	pgpnAlmMsgSpOut->iDstId = pgpnAlmMsgSpIn->iSrcId;
	pgpnAlmMsgSpOut->iMsgType = GPN_ALM_MSG_GLO_RISE_DELAY_GET_RSP;
	
	if(gpnAlmDSApiRiseDelayGet(&rDelay) == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = rDelay;
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		pgpnAlmMsgSpOut->iMsgPara1 = 0;
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiRiseDelayCfg(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen)
{
	/* assert */
	
	return gpnAlmDSApiRiseDelayCfg(pgpnAlmMsgSpIn->iMsgPara1);
}

static UINT32 gpnAlmSnmpApiCleanDelayGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 cDelay;
		
	/* assert */

	/* set default retrun */
	pgpnAlmMsgSpOut->iIndex = pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iMsgCtrl= pgpnAlmMsgSpIn->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId = pgpnAlmMsgSpIn->iDstId;
	pgpnAlmMsgSpOut->iDstId = pgpnAlmMsgSpIn->iSrcId;
	pgpnAlmMsgSpOut->iMsgType = GPN_ALM_MSG_GLO_CLEAN_DELAY_GET_RSP;
	
	if(gpnAlmDSApiCleanDelayGet(&cDelay) == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = cDelay;
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		pgpnAlmMsgSpOut->iMsgPara1 = 0;
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiCleanDelayCfg(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen)
{
	/* assert */
	return gpnAlmDSApiCleanDelayCfg(pgpnAlmMsgSpIn->iMsgPara1);
}

static UINT32 gpnAlmSnmpApiBuzzerEnGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 bEn;
		
	/* assert */

	/* set default retrun */
	pgpnAlmMsgSpOut->iIndex = pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iMsgCtrl= pgpnAlmMsgSpIn->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId = pgpnAlmMsgSpIn->iDstId;
	pgpnAlmMsgSpOut->iDstId = pgpnAlmMsgSpIn->iSrcId;
	pgpnAlmMsgSpOut->iMsgType = GPN_ALM_MSG_GLO_RING_EN_GET_RSP;
	
	if(gpnAlmDSApiBuzzerEnGet(&bEn) == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = bEn;
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		pgpnAlmMsgSpOut->iMsgPara1 = 0;
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiBuzzerEnCfg(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen)
{
	/* assert */
	
	return gpnAlmDSApiBuzzerEnCfg(pgpnAlmMsgSpIn->iMsgPara1);
}
static UINT32 gpnAlmSnmpApiBuzzerClrCfg(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen)
{
	/* assert */
	
	return gpnAlmDSApiBuzzerClrCfg(pgpnAlmMsgSpIn->iMsgPara1);
}

static UINT32 gpnAlmSnmpApiBuzzerThreGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 bThr;
		
	/* assert */

	/* set default retrun */
	pgpnAlmMsgSpOut->iIndex = pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iMsgCtrl= pgpnAlmMsgSpIn->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId = pgpnAlmMsgSpIn->iDstId;
	pgpnAlmMsgSpOut->iDstId = pgpnAlmMsgSpIn->iSrcId;
	pgpnAlmMsgSpOut->iMsgType = GPN_ALM_MSG_GLO_RING_RANK_GET_RSP;
	
	if(gpnAlmDSApiBuzzerThreGet(&bThr) == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = bThr;
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		pgpnAlmMsgSpOut->iMsgPara1 = 0;
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiBuzzerThreCfg(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen)
{
	/* assert */
	
	return gpnAlmDSApiBuzzerThreCfg(pgpnAlmMsgSpIn->iMsgPara1);
}
static UINT32 gpnAlmSnmpApiCurrDBSizeGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 cDbSize;
			
	/* assert */

	/* set default retrun */
	pgpnAlmMsgSpOut->iIndex = pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iMsgCtrl= pgpnAlmMsgSpIn->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId = pgpnAlmMsgSpIn->iDstId;
	pgpnAlmMsgSpOut->iDstId = pgpnAlmMsgSpIn->iSrcId;
	pgpnAlmMsgSpOut->iMsgType = GPN_ALM_MSG_CURR_DB_SIZE_GET_RSP;
	
	if(gpnAlmDSApiCurrDBSizeGet(&cDbSize) == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = cDbSize;
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		pgpnAlmMsgSpOut->iMsgPara1 = 0;
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiCurrDBCycGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 cDbCyc;
			
	/* assert */

	/* set default retrun */
	pgpnAlmMsgSpOut->iIndex = pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iMsgCtrl= pgpnAlmMsgSpIn->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId = pgpnAlmMsgSpIn->iDstId;
	pgpnAlmMsgSpOut->iDstId = pgpnAlmMsgSpIn->iSrcId;
	pgpnAlmMsgSpOut->iMsgType = GPN_ALM_MSG_CURR_DB_CYC_GET_RSP;
	
	if(gpnAlmDSApiCurrAlmDBCycGet(&cDbCyc) == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = cDbCyc;
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		pgpnAlmMsgSpOut->iMsgPara1 = 0;
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiCurrDBCycCfg(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen)
{
	/* assert */
	
	return gpnAlmDSApiCurrAlmDBCycCfg(pgpnAlmMsgSpIn->iMsgPara1);
}

static UINT32 gpnAlmSnmpApiHistDBSizeGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 hDbSize;
			
	/* assert */

	/* set default retrun */
	pgpnAlmMsgSpOut->iIndex = pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iMsgCtrl= pgpnAlmMsgSpIn->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId = pgpnAlmMsgSpIn->iDstId;
	pgpnAlmMsgSpOut->iDstId = pgpnAlmMsgSpIn->iSrcId;
	pgpnAlmMsgSpOut->iMsgType = GPN_ALM_MSG_HIST_DB_SIZE_GET_RSP;
	
	if(gpnAlmDSApiHistDBSizeGet(&hDbSize) == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = hDbSize;
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		pgpnAlmMsgSpOut->iMsgPara1 = 0;
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiHistDBCycGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 hDbCyc;
			
	/* assert */

	/* set default retrun */
	pgpnAlmMsgSpOut->iIndex = pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iMsgCtrl= pgpnAlmMsgSpIn->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId = pgpnAlmMsgSpIn->iDstId;
	pgpnAlmMsgSpOut->iDstId = pgpnAlmMsgSpIn->iSrcId;
	pgpnAlmMsgSpOut->iMsgType = GPN_ALM_MSG_HIST_DB_CYC_GET_RSP;
	
	if(gpnAlmDSApiHistAlmDBCycGet(&hDbCyc) == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = hDbCyc;
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		pgpnAlmMsgSpOut->iMsgPara1 = 0;
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiHistDBCycCfg(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen)
{
	/* assert */
	
	return gpnAlmDSApiHistAlmDBCycCfg(pgpnAlmMsgSpIn->iMsgPara1);
}

static UINT32 gpnAlmSnmpApiEventDBSizeGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 eDbSize;
			
	/* assert */

	/* set default retrun */
	pgpnAlmMsgSpOut->iIndex = pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iMsgCtrl= pgpnAlmMsgSpIn->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId = pgpnAlmMsgSpIn->iDstId;
	pgpnAlmMsgSpOut->iDstId = pgpnAlmMsgSpIn->iSrcId;
	pgpnAlmMsgSpOut->iMsgType = GPN_ALM_MSG_EVT_DB_SIZE_GET_RSP;
	
	if(gpnAlmDSApiEventDBSizeGet(&eDbSize) == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = eDbSize;
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		pgpnAlmMsgSpOut->iMsgPara1 = 0;
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiEventDBCycGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 eDbCyc;
			
	/* assert */

	/* set default retrun */
	pgpnAlmMsgSpOut->iIndex = pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iMsgCtrl= pgpnAlmMsgSpIn->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId = pgpnAlmMsgSpIn->iDstId;
	pgpnAlmMsgSpOut->iDstId = pgpnAlmMsgSpIn->iSrcId;
	pgpnAlmMsgSpOut->iMsgType = GPN_ALM_MSG_EVT_DB_CYC_GET_RSP;
	
	if(gpnAlmDSApiEventDBCycGet(&eDbCyc) == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = eDbCyc;
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		pgpnAlmMsgSpOut->iMsgPara1 = 0;
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiEventDBCycCfg(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen)
{
	/* assert */
	
	return gpnAlmDSApiEventDBCycCfg(pgpnAlmMsgSpIn->iMsgPara1);
}

static UINT32 gpnAlmSnmpApiRestModeGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 restMode;
		
	/* assert */

	/* set default retrun */
	pgpnAlmMsgSpOut->iIndex = pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iMsgCtrl= pgpnAlmMsgSpIn->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId = pgpnAlmMsgSpIn->iDstId;
	pgpnAlmMsgSpOut->iDstId = pgpnAlmMsgSpIn->iSrcId;
	pgpnAlmMsgSpOut->iMsgType = GPN_ALM_MSG_GLO_RSET_EN_GET_RSP;
	
	if(gpnAlmDSApiRestModeGet(&restMode) == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = restMode;
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		pgpnAlmMsgSpOut->iMsgPara1 = 0;
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiRestModeCfg(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen)
{
	/* assert */
	
	return gpnAlmDSApiRestModeCfg(pgpnAlmMsgSpIn->iMsgPara1);
}
static UINT32 gpnAlmSnmpApiReverModeGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 rever;
		
	/* assert */

	/* set default retrun */
	pgpnAlmMsgSpOut->iIndex = pgpnAlmMsgSpIn->iIndex;
	pgpnAlmMsgSpOut->iMsgCtrl= pgpnAlmMsgSpIn->iMsgCtrl;
	pgpnAlmMsgSpOut->iSrcId = pgpnAlmMsgSpIn->iDstId;
	pgpnAlmMsgSpOut->iDstId = pgpnAlmMsgSpIn->iSrcId;
	pgpnAlmMsgSpOut->iMsgType = GPN_ALM_MSG_GLO_REVER_EN_GET_RSP;
	
	if(gpnAlmDSApiReverModeGet(&rever) == GPN_ALM_SNMP_OK)
	{
		pgpnAlmMsgSpOut->iMsgPara1 = rever;
		return GPN_ALM_SNMP_OK;
	}
	else
	{
		pgpnAlmMsgSpOut->iMsgPara1 = 0;
		return GPN_ALM_SNMP_ERR;
	}
}
static UINT32 gpnAlmSnmpApiReverModeCfg(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen)
{
	/* assert */
	
	return gpnAlmDSApiReverModeCfg(pgpnAlmMsgSpIn->iMsgPara1);
}

UINT32 gpnAlmAgentxIoCtrl(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen)
{
	UINT32 reVal;

	switch(pgpnAlmMsgSpIn->iMsgType)
	{
		/*********************************  table get *************************************/
		case GPN_ALM_MSG_PORT_MONI_DB_GET:	
			reVal = gpnAlmSnmpApiPortMonGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT:	
			reVal = gpnAlmSnmpApiPortMonGetNext(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_ATTRIB_DB_GET:	
			reVal = gpnAlmSnmpApiAlmAttribGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_ATTRIB_DB_GET_NEXT:	
			reVal = gpnAlmSnmpApiAlmAttribGetNext(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;
			
		case GPN_ALM_MSG_CURR_ALM_DB_GET:
			reVal = gpnAlmSnmpApiCurrAlmGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT:			
			reVal = gpnAlmSnmpApiCurrAlmGetNext(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_HIST_ALM_DB_GET:	
			reVal = gpnAlmSnmpApiHistAlmGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT:			
			reVal = gpnAlmSnmpApiHistAlmGetNext(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_EVT_DATA_DB_GET:	
			reVal = gpnAlmSnmpApiEventGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT:			
			reVal = gpnAlmSnmpApiEventGetNext(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		/********************************* debug table get *************************************/

		case GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET:	
			reVal = gpnAlmSnmpApiDebugCurrAlmDBGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT:	
			reVal = gpnAlmSnmpApiDebugCurrAlmDBGetNext(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET:	
			reVal = gpnAlmSnmpApiDebugHistAlmDBGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT:	
			reVal = gpnAlmSnmpApiDebugHistAlmDBGetNext(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET:	
			reVal = gpnAlmSnmpApiDebugEventDBGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT:
			reVal = gpnAlmSnmpApiDebugEventDBGetNext(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		/*********************************  scalar get *************************************/		

		case GPN_ALM_MSG_GLO_RISE_DELAY_GET:
			reVal = gpnAlmSnmpApiRiseDelayGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;
			 
		case GPN_ALM_MSG_GLO_CLEAN_DELAY_GET:
			reVal = gpnAlmSnmpApiCleanDelayGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		case GPN_ALM_MSG_GLO_RING_EN_GET:
			reVal = gpnAlmSnmpApiBuzzerEnGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;
			 
		case GPN_ALM_MSG_GLO_RING_RANK_GET:
			reVal = gpnAlmSnmpApiBuzzerThreGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;
			 
		case GPN_ALM_MSG_GLO_REVER_EN_GET:
			reVal = gpnAlmSnmpApiReverModeGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;
			 
		case GPN_ALM_MSG_GLO_RSET_EN_GET:
			reVal = gpnAlmSnmpApiRestModeGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;
			 
		case GPN_ALM_MSG_CURR_DB_SIZE_GET:
			reVal = gpnAlmSnmpApiCurrDBSizeGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;
			 
		case GPN_ALM_MSG_CURR_DB_CYC_GET:
			reVal = gpnAlmSnmpApiCurrDBCycGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;
			 
		case GPN_ALM_MSG_HIST_DB_SIZE_GET:
			reVal = gpnAlmSnmpApiHistDBSizeGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;
			 
		case GPN_ALM_MSG_HIST_DB_CYC_GET:
			reVal = gpnAlmSnmpApiHistDBCycGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;
			 
		case GPN_ALM_MSG_EVT_DB_SIZE_GET:
			reVal = gpnAlmSnmpApiEventDBSizeGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;
			 
		case GPN_ALM_MSG_EVT_DB_CYC_GET:
			reVal = gpnAlmSnmpApiEventDBCycGet(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;			

		/*********************************  table config *************************************/
		/* alarm monitor table config */
		case GPN_ALM_MSG_PORT_ALM_MON_CFG:		/* alarm monitor enable and disable config */
			reVal = gpnAlmSnmpApiPortMonModify(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;
			
		/* alarm attribute table config */
		case GPN_ALM_MSG_ATTRIB_DB_MODIFY:		/* alarm attribute like rank, screen etc config */
			reVal = gpnAlmSnmpApiAlmAttribModify(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		/* alarm current alarm table config */
		case GPN_ALM_MSG_CURR_ALM_DB_MODIFY:/* alarm product time clear config */
			reVal = gpnAlmSnmpApiCurrAlmModify(pgpnAlmMsgSpIn, inLen, pgpnAlmMsgSpOut, outLen);
			break;

		/*********************************  scalar config *************************************/
		/* alarm global scalar config */
		case GPN_ALM_MSG_GLO_RISE_DELAY_CFG:
			reVal = gpnAlmSnmpApiRiseDelayCfg(pgpnAlmMsgSpIn, inLen);
			break;
			
		case GPN_ALM_MSG_GLO_CLEAN_DELAY_CFG:
			reVal = gpnAlmSnmpApiCleanDelayCfg(pgpnAlmMsgSpIn, inLen);
			break;
			
		case GPN_ALM_MSG_GLO_RING_EN_CFG:
			reVal = gpnAlmSnmpApiBuzzerEnCfg(pgpnAlmMsgSpIn, inLen);
			break;
			
		case GPN_ALM_MSG_GLO_RING_SURE:
			reVal = gpnAlmSnmpApiBuzzerClrCfg(pgpnAlmMsgSpIn, inLen);
			break;
			
		case GPN_ALM_MSG_GLO_RING_RANK_CFG:	
			reVal = gpnAlmSnmpApiBuzzerThreCfg(pgpnAlmMsgSpIn, inLen);
			break;
			
		case GPN_ALM_MSG_GLO_REVER_EN_CFG:
			reVal = gpnAlmSnmpApiReverModeCfg(pgpnAlmMsgSpIn, inLen);
			break;
			
		case GPN_ALM_MSG_GLO_RSET_EN_CFG:
			reVal = gpnAlmSnmpApiRestModeCfg(pgpnAlmMsgSpIn, inLen);
			break;
			
		case GPN_ALM_MSG_CURR_DB_CYC_CFG:
			reVal = gpnAlmSnmpApiCurrDBCycCfg(pgpnAlmMsgSpIn, inLen);
			break;
			
		case GPN_ALM_MSG_HIST_DB_CYC_CFG:
			reVal = gpnAlmSnmpApiHistDBCycCfg(pgpnAlmMsgSpIn, inLen);
			break;
			
		case GPN_ALM_MSG_EVT_DB_CYC_CFG:
			reVal = gpnAlmSnmpApiEventDBCycCfg(pgpnAlmMsgSpIn, inLen);
			break;
			
		default:
			reVal = GPN_ALM_SNMP_ERR;
			break;
	}

	return reVal;
}
#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_SNMP_API_C_*/

