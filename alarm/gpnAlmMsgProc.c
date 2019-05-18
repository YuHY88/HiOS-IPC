/**********************************************************
* file name: gpnAlmMsgProc.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-11
* function: 
*    define alarm message process detail
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_MSG_PROC_C_
#define _GPN_ALM_MSG_PROC_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>
#include <lib/thread.h>
#include <lib/syslog.h>
#include <lib/log.h>
#include <lib/devm_com.h>

#include "gpnAlmApi.h"

#include "gpnAlmTimeProc.h"
#include "gpnAlmPortNotify.h"
#include "gpnAlmAlmNotify.h"
#include "gpnAlmMsgProc.h"


/*log function include*/
#include "gpnLog/gpnLogFuncApi.h"
#include "socketComm/gpnSockCommRoleMan.h"
#include "socketComm/gpnSockProtocolMsgDef.h"
#include "socketComm/gpnSockCfgMgtMsgDef.h"
#include "socketComm/gpnTimerService.h"

#include "gpnAlmMasterSlaveSync.h"

//debug
#include "testDataStructure.h"
#include "gpnAlmDebug.h"


extern gstAlmTypeWholwInfo *pgstAlmTypeWholwInfo;
extern stEQUAlmProcSpace *pgstAlmPreScanWholwInfo;

extern sockCommData gSockCommData;

static UINT32 gpnAlmMsgProcDevStaChg(gpnSockMsg *pgpnAlmMsgSp, UINT32 len)
{
	objLogicDesc localPIndex;
	UINT32 reVal;
	
	/* comm use local port */
	localPIndex.devIndex = pgpnAlmMsgSp->iMsgPara2;
	localPIndex.portIndex = pgpnAlmMsgSp->iMsgPara3;
	localPIndex.portIndex3 = pgpnAlmMsgSp->iMsgPara4;
	localPIndex.portIndex4 = pgpnAlmMsgSp->iMsgPara5;
	localPIndex.portIndex6 = pgpnAlmMsgSp->iMsgPara6;
	localPIndex.portIndex6 = pgpnAlmMsgSp->iMsgPara7;
	
	reVal = gpnAlmPNtDevStaNotify(pgpnAlmMsgSp->iMsgPara1,\
		&localPIndex, pgpnAlmMsgSp->iMsgPara8);

	gpnLog(GPN_LOG_L_CRIT,\
		"%s : (pig-%d) notify dev(%08x) sta(%d), "
		"gadPort(%08x-%08x|%08x|%08x|%08x|%08x) reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara9,\
		pgpnAlmMsgSp->iMsgPara1, pgpnAlmMsgSp->iMsgPara8,\
		localPIndex.devIndex, localPIndex.portIndex,\
		localPIndex.portIndex3, localPIndex.portIndex4,\
		localPIndex.portIndex5, localPIndex.portIndex6,\
		reVal);

	GPN_ALM_MP_PRINT((GPN_ALM_API_CUP)|(GPN_ALM_API_UD0),\
		"%s : (pig-%d) notify dev(%08x) sta(%d), "
		"gadPort(%08x-%08x|%08x|%08x|%08x|%08x) reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara9,\
		pgpnAlmMsgSp->iMsgPara1, pgpnAlmMsgSp->iMsgPara8,\
		localPIndex.devIndex, localPIndex.portIndex,\
		localPIndex.portIndex3, localPIndex.portIndex4,\
		localPIndex.portIndex5, localPIndex.portIndex6,\
		reVal);

	return reVal;
}
static UINT32 gpnAlmMsgProcFixPortNotify(gpnSockMsg *pgpnAlmMsgSp, UINT32 len)
{
	stAlmPortObjCL *pPortObjCL;
	UINT32 portTpNum;
	UINT32 devIndex;
	UINT32 reVal;

	devIndex = pgpnAlmMsgSp->iMsgPara1;
	pPortObjCL = (stAlmPortObjCL *)(&(pgpnAlmMsgSp->msgCellLen) + 1);
	portTpNum = pgpnAlmMsgSp->msgCellLen / sizeof(stAlmPortObjCL);
	reVal = gpnAlmPNtFixPortUserNotify(devIndex, portTpNum, pPortObjCL);

	gpnLog(GPN_LOG_L_CRIT,\
		"%s : (pig-%d) notify dev(%08x)'s fix prot, reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara2,\
		pgpnAlmMsgSp->iMsgPara1, reVal);

	GPN_ALM_MP_PRINT((GPN_ALM_API_CUP)|(GPN_ALM_API_UD0),\
		"%s : (pig-%d) notify dev(%08x)'s fix prot, reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara2,\
		pgpnAlmMsgSp->iMsgPara1, reVal);
	
	return reVal;
}

static UINT32 gpnAlmMsgProcVariPortReg(gpnSockMsg *pgpnAlmMsgSp, UINT32 len)
{
	objLogicDesc localPIndex;
	UINT32 reVal;
	
	/* port creat use local port */
	//localPIndex.devIndex = DeviceIndex_Create( 1, 0 ); /* why fix ??? */
	localPIndex.devIndex   = pgpnAlmMsgSp->iMsgPara6;	//modify by lipf, 2018/4/23
	localPIndex.portIndex  = pgpnAlmMsgSp->iMsgPara1;
	localPIndex.portIndex3 = pgpnAlmMsgSp->iMsgPara2;
	localPIndex.portIndex4 = pgpnAlmMsgSp->iMsgPara3;
	localPIndex.portIndex5 = pgpnAlmMsgSp->iMsgPara4;
	localPIndex.portIndex6 = pgpnAlmMsgSp->iMsgPara5;	
	reVal = gpnAlmPNtVariPortUserNotify(&localPIndex);

	gpnLog(GPN_LOG_L_CRIT,\
		"%s : vari port reg(pig-%d) : %08x-%08x|%08x|%08x|%08x|%08x reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara7,\
		localPIndex.devIndex, localPIndex.portIndex,\
		localPIndex.portIndex3, localPIndex.portIndex4,\
		localPIndex.portIndex5, localPIndex.portIndex6,\
		reVal);

	zlog_debug(ALARM_DBG_REGISTER, "%s : vari port reg(pig-%d) : (%08x-%08x|%08x|%08x|%08x|%08x) %s\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara7,\
		localPIndex.devIndex, localPIndex.portIndex,\
		localPIndex.portIndex3, localPIndex.portIndex4,\
		localPIndex.portIndex5, localPIndex.portIndex6,\
		(GPN_ALM_GEN_OK == reVal)?"success":"fail");

	GPN_ALM_MP_PRINT((GPN_ALM_API_CUP)|(GPN_ALM_API_UD0),\
		"%s : vari port reg(pig-%d) : %08x-%08x|%08x|%08x|%08x|%08x reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara7,\
		localPIndex.devIndex, localPIndex.portIndex,\
		localPIndex.portIndex3, localPIndex.portIndex4,\
		localPIndex.portIndex5, localPIndex.portIndex6,\
		reVal);

	return reVal;
}
static UINT32 gpnAlmMsgProcVariPortUnReg(gpnSockMsg *pgpnAlmMsgSp, UINT32 len)
{
	objLogicDesc localPIndex;
	UINT32 reVal;
	
	/* port creat use local port */
	//localPIndex.devIndex = DeviceIndex_Create( 1, 0 ); /* why fix ??? */
	localPIndex.devIndex   = pgpnAlmMsgSp->iMsgPara6;	//modify for ipran, by lipf 2018/5/2
	localPIndex.portIndex  = pgpnAlmMsgSp->iMsgPara1;
	localPIndex.portIndex3 = pgpnAlmMsgSp->iMsgPara2;
	localPIndex.portIndex4 = pgpnAlmMsgSp->iMsgPara3;
	localPIndex.portIndex5 = pgpnAlmMsgSp->iMsgPara4;
	localPIndex.portIndex6 = pgpnAlmMsgSp->iMsgPara5;
	reVal = gpnAlmPNtVariPortUnReg(&localPIndex);

	gpnLog(GPN_LOG_L_CRIT,\
		"%s : vari port unreg(pig-%d) : %08x-%08x|%08x|%08x|%08x|%08x reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara7,\
		localPIndex.devIndex, localPIndex.portIndex,\
		localPIndex.portIndex3, localPIndex.portIndex4,\
		localPIndex.portIndex5, localPIndex.portIndex6,\
		reVal);

	zlog_debug(ALARM_DBG_REGISTER, "%s : vari port unreg(pig-%d) : %08x-%08x|%08x|%08x|%08x|%08x reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara7,\
		localPIndex.devIndex, localPIndex.portIndex,\
		localPIndex.portIndex3, localPIndex.portIndex4,\
		localPIndex.portIndex5, localPIndex.portIndex6,\
		reVal);

	GPN_ALM_MP_PRINT((GPN_ALM_API_CUP)|(GPN_ALM_API_UD0),\
		"%s : vari port unreg(pig-%d) : %08x-%08x|%08x|%08x|%08x|%08x reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara7,\
		localPIndex.devIndex, localPIndex.portIndex,\
		localPIndex.portIndex3, localPIndex.portIndex4,\
		localPIndex.portIndex5, localPIndex.portIndex6,\
		reVal);

	return reVal;
}
static UINT32 gpnAlmMsgProcPeerPortReplace(gpnSockMsg *pgpnAlmMsgSp, UINT32 len)
{
	optObjOrient peerPIndex;
	optObjOrient peerPRep;
	UINT32 reVal;
	
	/* peer port replace use peer port */
	//peerPIndex.devIndex = DeviceIndex_Create( 1, 0 ); /* why fix ??? */
	peerPIndex.devIndex   = pgpnAlmMsgSp->iMsgPara6;	//modify for ipran, by lipf 2018/5/2
	peerPIndex.portIndex  = pgpnAlmMsgSp->iMsgPara1;
	peerPIndex.portIndex3 = pgpnAlmMsgSp->iMsgPara2;
	peerPIndex.portIndex4 = pgpnAlmMsgSp->iMsgPara3;
	peerPIndex.portIndex5 = pgpnAlmMsgSp->iMsgPara4;
	peerPIndex.portIndex6 = pgpnAlmMsgSp->iMsgPara5;
	
	//peerPRep.devIndex = DeviceIndex_Create( 1, 0 ); /* why fix ??? */
	peerPIndex.devIndex = pgpnAlmMsgSp->iMsgParaC;	//modify for ipran, by lipf 2018/5/2
	peerPRep.portIndex  = pgpnAlmMsgSp->iMsgPara7;
	peerPRep.portIndex3 = pgpnAlmMsgSp->iMsgPara8;
	peerPRep.portIndex4 = pgpnAlmMsgSp->iMsgPara9;
	peerPRep.portIndex5 = pgpnAlmMsgSp->iMsgParaA;
	peerPRep.portIndex6 = pgpnAlmMsgSp->iMsgParaB;
	
	reVal = gpnAlmPeerPortReplaceNotify(&peerPIndex, &peerPRep);

	gpnLog(GPN_LOG_L_ALERT,\
		"%s(%d) : port(%08x-%08x|%08x|%08x|%08x|%08x) "
		"replaced by port(%08x-%08x|%08x|%08x|%08x|%08x)\n",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgParaC,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		peerPRep.devIndex, peerPRep.portIndex,\
		peerPRep.portIndex3, peerPRep.portIndex4,\
		peerPRep.portIndex5, peerPRep.portIndex6);
	
	GPN_ALM_MP_PRINT(GPN_ALM_SCAN_CUP,\
		"%s(%d) : port(%08x-%08x|%08x|%08x|%08x|%08x) "
		"replaced by port(%08x-%08x|%08x|%08x|%08x|%08x)\n",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgParaC,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		peerPRep.devIndex, peerPRep.portIndex,\
		peerPRep.portIndex3, peerPRep.portIndex4,\
		peerPRep.portIndex5, peerPRep.portIndex6);

	return reVal;
}
static UINT32 gpnAlmMsgProcViewPortReplace(gpnSockMsg *pgpnAlmMsgSp, UINT32 len)
{
	objLogicDesc localPIndex;
	objLogicDesc viewPIndex;
	UINT32 reVal;
	
	//localPIndex.devIndex = DeviceIndex_Create( 1, 0 ); /* why fix ??? */
	localPIndex.devIndex    = pgpnAlmMsgSp->iMsgPara6;	//modify for ipran, by lipf 2018/5/2
	localPIndex.portIndex  = pgpnAlmMsgSp->iMsgPara1;
	localPIndex.portIndex3 = pgpnAlmMsgSp->iMsgPara2;
	localPIndex.portIndex4 = pgpnAlmMsgSp->iMsgPara3;
	localPIndex.portIndex5 = pgpnAlmMsgSp->iMsgPara4;
	localPIndex.portIndex6 = pgpnAlmMsgSp->iMsgPara5;
	
	//viewPIndex.devIndex = DeviceIndex_Create( 1, 0 ); /* why fix ??? */
	viewPIndex.devIndex   = pgpnAlmMsgSp->iMsgParaC;	//modify for ipran, by lipf 2018/5/2
	viewPIndex.portIndex  = pgpnAlmMsgSp->iMsgPara7;
	viewPIndex.portIndex3 = pgpnAlmMsgSp->iMsgPara8;
	viewPIndex.portIndex4 = pgpnAlmMsgSp->iMsgPara9;
	viewPIndex.portIndex5 = pgpnAlmMsgSp->iMsgParaA;
	viewPIndex.portIndex6 = pgpnAlmMsgSp->iMsgParaB;
	
	reVal = gpnAlmPNtViewPortReplace(&localPIndex, &viewPIndex);
	
	gpnLog(GPN_LOG_L_CRIT, \
		"%s(%d) : local port(%08x-%08x|%08x|%08x|%08x|%08x) "
		"view replaced to(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgParaC,\
		localPIndex.devIndex, localPIndex.portIndex,\
		localPIndex.portIndex3, localPIndex.portIndex4,\
		localPIndex.portIndex5, localPIndex.portIndex6,\
		viewPIndex.devIndex, viewPIndex.portIndex,\
		viewPIndex.portIndex3, viewPIndex.portIndex4,\
		viewPIndex.portIndex5, viewPIndex.portIndex6);

	return reVal;
}

static UINT32 gpnAlmMsgProcAlmNotifyBaseSubType(gpnSockMsg *pgpnAlmMsgSp, UINT32 len)
{
	optObjOrient peerPIndex;
	UINT32 reVal;

	/* 主备同步开关，当为备盘时，不处理告警上报 */
	if(DEVM_HA_SLAVE == alarm_MS_my_status_get())
	{
		return GPN_ALM_GEN_YES;
	}
	
	/* stat notify use peer port */
	//peerPIndex.devIndex  = DeviceIndex_Create( 1, 0 ); /* why fix ??? */
	peerPIndex.devIndex   = pgpnAlmMsgSp->iMsgPara6;
	peerPIndex.portIndex  = pgpnAlmMsgSp->iMsgPara1;
	peerPIndex.portIndex3 = pgpnAlmMsgSp->iMsgPara2;
	peerPIndex.portIndex4 = pgpnAlmMsgSp->iMsgPara3;
	peerPIndex.portIndex5 = pgpnAlmMsgSp->iMsgPara4;
	peerPIndex.portIndex6 = pgpnAlmMsgSp->iMsgPara5;

	/* para7 : 告警子类型，如0x00030002
	 * para8 : 告警子类型值	*/
	reVal = gpnAlmANtBaseSubTypeProc(&peerPIndex,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8);
	
	GPN_ALM_MP_PRINT((GPN_ALM_API_CUP)|(GPN_ALM_API_UD0),\
		"%s(%d) : COM_ALM peer(%08x-%08x|%08x|%08x|%08x|%08x),almTp(%08x) opt(%d) reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara9,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8,\
		reVal);
	
	zlog_debug(ALARM_DBG_REPORT, "%s(%d) : index(%08x-%08x|%08x|%08x|%08x|%08x),almTp(%08x) %s %s\n\r",\
		__FUNCTION__, __LINE__,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara7, 
		(GPN_SOCK_MSG_OPT_RISE == pgpnAlmMsgSp->iMsgPara8)?"rise":"clear",\
		(GPN_ALM_GEN_YES == reVal)?"success":"fail");

	gpnLog(GPN_LOG_L_CRIT,\
		"%s(%d) : COM_ALM peer(%08x-%08x|%08x|%08x|%08x|%08x),almTp(%08x) opt(%d) reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara9,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8,\
		reVal);
	/*gpnLog(GPN_LOG_L_NOTICE,\
		"%s(%d) : COM_ALM peer(%08x-%08x|%08x|%08x|%08x|%08x),almTp(%08x) opt(%d) reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara9,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8,\
		reVal);*/

	return reVal;
}
static UINT32 gpnAlmMsgProcCFMAlmNotifyBaseSubType(gpnSockMsg *pgpnAlmMsgSp, UINT32 len)
{
	optObjOrient peerPIndex;
	UINT32 reVal;
	
	/* stat notify use peer port */
	//peerPIndex.devIndex = DeviceIndex_Create( 1, 0 ); /* why fix ??? */
	peerPIndex.devIndex   = pgpnAlmMsgSp->iMsgPara6;	//modify for ipran, by lipf 2018/5/2
	peerPIndex.portIndex  = pgpnAlmMsgSp->iMsgPara1;
	peerPIndex.portIndex3 = pgpnAlmMsgSp->iMsgPara2;
	peerPIndex.portIndex4 = pgpnAlmMsgSp->iMsgPara3;
	peerPIndex.portIndex5 = pgpnAlmMsgSp->iMsgPara4;
	peerPIndex.portIndex6 = pgpnAlmMsgSp->iMsgPara5;

	/*reVal = gpnMsgCfmANtBaseSubTypeProc(&peerPIndex,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8);*/
	reVal = gpnAlmANtBaseSubTypeProc(&peerPIndex,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8);

	GPN_ALM_MP_PRINT((GPN_ALM_API_CUP)|(GPN_ALM_API_UD0),\
		"%s(%d) : CFM_ALM peer(%08x-%08x|%08x|%08x|%08x|%08x),almTp(%08x) opt(%d) reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara9,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8,\
		reVal);
	
	/*
	printf("%s : CFM_ALM peer(%08x-%08x|%08x|%08x|%08x|%08x),almTp %08x opt %d\n\r",\
		"%s(%d) : CFM_ALM peer(%08x-%08x|%08x|%08x|%08x|%08x),almTp(%08x) opt(%d) reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara9,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8,\
		reVal);*/

	gpnLog(GPN_LOG_L_CRIT, \
		"%s(%d) : CFM_ALM peer(%08x-%08x|%08x|%08x|%08x|%08x),almTp(%08x) opt(%d) reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara9,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8,\
		reVal);
	
	/*gpnLog(GPN_LOG_L_NOTICE, \
		"%s(%d) : CFM_ALM peer(%08x-%08x|%08x|%08x|%08x|%08x),almTp(%08x) opt(%d) reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara9,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8,\
		reVal);*/

	return reVal;
}
static UINT32 gpnAlmMsgProcAlmNotifyBaseScanType(gpnSockMsg *pgpnAlmMsgSp, UINT32 len)
{
	optObjOrient peerPIndex;
	UINT32 reVal;

	/* stat notify use peer port */
	//peerPIndex.devIndex = DeviceIndex_Create( 1, 0 ); /* why fix ??? */
	peerPIndex.devIndex   = pgpnAlmMsgSp->iMsgPara6;	//modify for ipran, by lipf 2018/5/2
	peerPIndex.portIndex  = pgpnAlmMsgSp->iMsgPara1;
	peerPIndex.portIndex3 = pgpnAlmMsgSp->iMsgPara2;
	peerPIndex.portIndex4 = pgpnAlmMsgSp->iMsgPara3;
	peerPIndex.portIndex5 = pgpnAlmMsgSp->iMsgPara4;
	peerPIndex.portIndex6 = pgpnAlmMsgSp->iMsgPara5;
	
	reVal = gpnAlmANtBaseScanTypeProc(&peerPIndex,\
		&pgpnAlmMsgSp->iMsgPara8, pgpnAlmMsgSp->iMsgPara7);

	GPN_ALM_MP_PRINT((GPN_ALM_API_CUP)|(GPN_ALM_API_UD0),\
		"%s : ALM BITS peer(%08x-%08x|%08x|%08x|%08x|%08x) "
		"P1-4(%08x %08x %08x %08x) P5-8(%08x %08x %08x %08x)\n\r",\
		__FUNCTION__, peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara8, pgpnAlmMsgSp->iMsgPara9,\
		pgpnAlmMsgSp->iMsgParaA, pgpnAlmMsgSp->iMsgParaB,\
		pgpnAlmMsgSp->iMsgParaC, pgpnAlmMsgSp->iMsgParaD,\
		pgpnAlmMsgSp->iMsgParaE, pgpnAlmMsgSp->iMsgParaF);

	gpnLog(GPN_LOG_L_CRIT, \
		"%s : ALM BITS peer(%08x-%08x|%08x|%08x|%08x|%08x) "
		"P1-4(%08x %08x %08x %08x) P5-8(%08x %08x %08x %08x)\n\r",\
		__FUNCTION__, peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara8, pgpnAlmMsgSp->iMsgPara9,\
		pgpnAlmMsgSp->iMsgParaA, pgpnAlmMsgSp->iMsgParaB,\
		pgpnAlmMsgSp->iMsgParaC, pgpnAlmMsgSp->iMsgParaD,\
		pgpnAlmMsgSp->iMsgParaE, pgpnAlmMsgSp->iMsgParaF);

	return reVal;
}
static UINT32 gpnAlmMsgProcEventNotifyBaseSubType(gpnSockMsg *pgpnAlmMsgSp, UINT32 len)
{
	optObjOrient peerPIndex;
	UINT32 reVal;

	/* 主备同步开关，当为备盘时，不处理事件上报 */
	if(DEVM_HA_SLAVE == alarm_MS_my_status_get())
	{
		return GPN_ALM_GEN_YES;
	}

	/* stat notify use peer port */
	//peerPIndex.devIndex = DeviceIndex_Create( 1, 0 ); /* why fix ??? */
	peerPIndex.devIndex   = pgpnAlmMsgSp->iMsgPara6;	//modify for ipran, by lipf 2018/5/2
	peerPIndex.portIndex  = pgpnAlmMsgSp->iMsgPara1;
	peerPIndex.portIndex3 = pgpnAlmMsgSp->iMsgPara2;
	peerPIndex.portIndex4 = pgpnAlmMsgSp->iMsgPara3;
	peerPIndex.portIndex5 = pgpnAlmMsgSp->iMsgPara4;
	peerPIndex.portIndex6 = pgpnAlmMsgSp->iMsgPara5;

	reVal = gpnAlmANtEvtBaseSubTypeProc(&peerPIndex,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8);

	zlog_debug(ALARM_DBG_REPORT, "%s(%d) : EVT REPT peer(%08x-%08x|%08x|%08x|%08x|%08x),evtTp(%08x) detail(%d) reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara9,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8,\
		reVal);

	GPN_ALM_MP_PRINT((GPN_ALM_API_CUP)|(GPN_ALM_API_UD0),\
		"%s(%d) : EVT REPT peer(%08x-%08x|%08x|%08x|%08x|%08x),evtTp(%08x) detail(%d) reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara9,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8,\
		reVal);

	gpnLog(GPN_LOG_L_CRIT,\
		"%s(%d) : EVT REPT peer(%08x-%08x|%08x|%08x|%08x|%08x),evtTp(%08x) detail(%d) reVal(%d)\n\r",\
		__FUNCTION__, pgpnAlmMsgSp->iMsgPara9,\
		peerPIndex.devIndex, peerPIndex.portIndex,\
		peerPIndex.portIndex3, peerPIndex.portIndex4,\
		peerPIndex.portIndex5, peerPIndex.portIndex6,\
		pgpnAlmMsgSp->iMsgPara7, pgpnAlmMsgSp->iMsgPara8,\
		reVal);

	return reVal;
}

extern struct thread_master *alarm_master;

int gpnAlm1SecondTimerPorc(void *para)
{
	/* timer mark */
	static UINT32 tick;
	tick++;
	
	/* 1s cyc maintain gpn_alarm process sys time */
	gpnAlmTimeAdjust(1);
	
	/*1s cyc proc alarm value */
	gpnAlmScanProc();

	/* GPN_ALM_SHAKE_PROC_CYCs cyc proc alarm shake */
	if((tick % GPN_ALM_SHAKE_PROC_CYC) == GPN_ALM_SHAKE_PROC_POINT)
	{
		gpnAlmShakeProc();
	}
	
	if(tick == 10)
	{
		//debugGpnAlmTable();
		//debugGpnAlmScalar();
	}

	//thread_add_timer (alarm_master, gpnAlm1SecondTimerPorc, NULL, 1);
	return 0;
}

UINT32 gpnAlmSockMsgProc(gpnSockMsg *pgpnAlmMsgSp, UINT32 len)
{
	switch(pgpnAlmMsgSp->iMsgType)
	{
		case GPN_GEN_MSG_TIMER_1S:
			/* now not use, use gpn_timer */
			/*gpnAlm1SecondTimerPorc(0, 0);*/
			GPN_ALM_MP_PRINT(GPN_ALM_API_CUP, "%s : 1 seconds timer\n\r",\
				__FUNCTION__);
			break;

			/* comm sub alarm tyep notify base SMP portIndex */
		case GPN_ALM_MSG_ALM_REPT_TYPE_BASE:
			gpnAlmMsgProcAlmNotifyBaseSubType(pgpnAlmMsgSp, len);
			break;

			/* special for CFM alarm */
		case GPN_ALM_MSG_ALM_REPT_CFM_BASE:
			gpnAlmMsgProcCFMAlmNotifyBaseSubType(pgpnAlmMsgSp, len);
			break;

		case GPN_ALM_MSG_ALM_REPT_BITS_BASE:
			gpnAlmMsgProcAlmNotifyBaseScanType(pgpnAlmMsgSp, len);
			break;

		case GPN_ALM_MSG_EVT_REPT_TYPE_BASE:
			zlog_debug(ALARM_DBG_REPORT, "%s[%d] : alarm recvice event report\n", __func__, __LINE__);
			gpnAlmMsgProcEventNotifyBaseSubType(pgpnAlmMsgSp, len);
			break;

		case GPN_IFM_MSG_PEER_PORT_REPLACE:
			gpnAlmMsgProcPeerPortReplace(pgpnAlmMsgSp, len);
			break;

		case GPN_IFM_MSG_VIEW_PORT_REPLACE:
			gpnAlmMsgProcViewPortReplace(pgpnAlmMsgSp, len);
			break;
			
		case GPN_IFM_MSG_VARI_PORT_REG:
			gpnAlmMsgProcVariPortReg(pgpnAlmMsgSp, len);
			break;
		
		case GPN_IFM_MSG_VARI_PORT_UNREG:
			gpnAlmMsgProcVariPortUnReg(pgpnAlmMsgSp, len);
			break;
			
		case GPN_IFM_MSG_FIX_PORT_REG:
			gpnAlmMsgProcFixPortNotify(pgpnAlmMsgSp, len);
			break;
			
		case GPN_IFM_MSG_DEV_STATUS_CHANG:
			gpnAlmMsgProcDevStaChg(pgpnAlmMsgSp, len);
			break;
		
		default:
			GPN_ALM_MP_PRINT(GPN_ALM_API_CMP,\
				"%s : unknow msgType(%08x)!\n\r",\
				__FUNCTION__, pgpnAlmMsgSp->iMsgType);
			break;
	}

	return GPN_ALM_MP_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_MSG_PROC_C_*/

