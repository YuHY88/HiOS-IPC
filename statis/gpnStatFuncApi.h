/**********************************************************
* file name: gpnStatFuncApi.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-25
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_FUNC_API_H_
#define _GPN_STAT_FUNC_API_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>

#include "type_def.h"
#include "stat_debug.h"

#include "socketComm/gpnSockMsgDef.h"
#include "socketComm/gpnGlobalPortIndexDef.h"
#include "gpnStatDataStruct.h"

#define GPN_STAT_API_GEN_OK 					GEN_SYS_OK
#define GPN_STAT_API_GEN_ERR 					GEN_SYS_ERR

#define GPN_STAT_API_GEN_YES 					GEN_SYS_YES
#define GPN_STAT_API_GEN_NO						GEN_SYS_NO

#define GPN_STAT_API_GEN_ENABLE 				GEN_SYS_ENABLE
#define GPN_STAT_API_GEN_DISABLE				GEN_SYS_DISABLE

#define GPN_STAT_API_PRINT(level, info...) 		GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_STAT_API_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_STAT_API_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_STAT_API_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_STAT_API_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_STAT_API_UD5						GEN_SYS_DEBUG_UD5
#define GPN_STAT_API_UD4						GEN_SYS_DEBUG_UD4
#define GPN_STAT_API_UD3						GEN_SYS_DEBUG_UD3
#define GPN_STAT_API_UD2						GEN_SYS_DEBUG_UD2
#define GPN_STAT_API_UD1						GEN_SYS_DEBUG_UD1
#define GPN_STAT_API_UD0						GEN_SYS_DEBUG_UD0

UINT32 gpnStatCorePorc(gpnSockMsg *pgpnStatMsgSp, UINT32 len);
UINT32 gpnStatGlobalPorc(gpnSockMsg *pgpnStatMsgSp, UINT32 len);
UINT32 gpnStatCollectPorc(gpnSockMsg *pgpnStatMsgSp, UINT32 len);
UINT32 gpnStatFuncApiSubTypeThredModify(UINT32 subType, stSubTypeThred *psubTypeThred, UINT32 len);
UINT32 gpnStatFuncApiCurrStatModify(objLogicDesc *pmonObjInfo, UINT32 subType, UINT32 dataClear);
UINT32 gpnStatFuncApiTaskAdd(UINT32 taskId,
	UINT32 statScanType, UINT32 subTaskNum, UINT32 cycClass, UINT32 cycSeconds,
	UINT32 monStartTime, UINT32 monEndTime, UINT32 almThredTpId, UINT32 subReportTpId);
UINT32 gpnStatFuncApiTaskDelete(UINT32 taskId);
UINT32 gpnStatFuncApiTaskModify(UINT32 taskId, UINT32 cycSeconds,
	UINT32 monStartTime, UINT32 monEndTime, UINT32 almThredTpId, UINT32 subReportTpId);
UINT32 gpnStatFuncApiEvnThredTpAdd(UINT32 thredTpId,
	UINT32 subType, UINT32 scanType, UINT32 upThredH32,
	UINT32 upThredL32, UINT32 downThredH32, UINT32 downThredL32);
UINT32 gpnStatFuncApiEvnThredTpDelete(UINT32 thredTpId);
UINT32 gpnStatFuncApiEvnThredTpModify(UINT32 thredTpId,
	UINT32 subType, UINT32 upThredH,  UINT32 upThredL,
	UINT32 dnThredH, UINT32 dnThredL);
UINT32 gpnStatFuncApiAlmThredTpAdd(UINT32 thredTpId,
	UINT32 subType, UINT32 scanType, UINT32 upRiseThreshH, UINT32 upRiseThreshL,
	UINT32 upDispThreshH, UINT32 upDispThreshL, UINT32 dnRiseThreshH,
	UINT32 dnRiseThreshL, UINT32 dnDispThreshH, UINT32 dnDispThreshL);
UINT32 gpnStatFuncApiAlmThredTpDelete(UINT32 thredTpId);
UINT32 gpnStatFuncApiAlmThredTpModify(UINT32 thredTpId,
	UINT32 subType, UINT32 upRiseThredH, UINT32 upRiseThredL,
	UINT32 upDispThredH, UINT32 upDispThredL, UINT32 dnRiseThredH,
	UINT32 dnRiseThredL, UINT32 dnDispThredH, UINT32 dnDispThredL);
UINT32 gpnStatFuncApiSubFiltTpAdd(UINT32 subFiltTpId,
	UINT32 subType, UINT32 scanType, UINT32 status);
UINT32 gpnStatFuncApiSubFiltTpDelete(UINT32 subFiltTpId);
UINT32 gpnStatFuncApiSubFiltTpModify(UINT32 subFiltTpId, UINT32 subType, UINT32 status);
UINT32 gpnStatFuncApiSubReportTpAdd(UINT32 subReportTpId,
	UINT32 subType, UINT32 scanType, UINT32 status);
UINT32 gpnStatFuncApiSubReportTpDelete(UINT32 subReportTpId);
UINT32 gpnStatFuncApiSubReportTpModify(UINT32 subReportTpId, UINT32 subType, UINT32 status);
UINT32 gpnStatFuncApiMonAdd(objLogicDesc *plocalPIndex, UINT32 scanType);
UINT32 gpnStatFuncApiMonDelete(objLogicDesc *plocalPIndex, UINT32 scanType);
UINT32 gpnStatFuncApiBaseMonOpt(objLogicDesc *plocalPIndex, UINT32 scanType, UINT32 opt, UINT32 currOpt);
UINT32 gpnStatFuncApiXCycMonOpt(objLogicDesc *plocalPIndex, UINT32 scanType, UINT32 taskId,
	UINT32 opt, UINT32 evnThTpId, UINT32 subFiltTpId, INT32 histReport);
/*UINT32 gpnStatFuncApiTaskSubFiltTpCfg(UINT32 taskId, UINT32 subFiltTpId);*/
UINT32 gpnStatFuncApiDevStausNotify(UINT32 devIndex, UINT32 sta);
UINT32 gpnStatFuncApiFixPortRegist(gpnSockMsg *pgpnStatMsgSp, UINT32 len);
UINT32 gpnStatFuncApiVariPortRegist(gpnSockMsg *pgpnStatMsgSp, UINT32 len);
UINT32 gpnStatFuncApiVariPortUnRegist(gpnSockMsg *pgpnStatMsgSp, UINT32 len);
UINT32 gpnStatFuncApiPeerPortReplaceNotify(gpnSockMsg *pgpnStatMsgSp, UINT32 len);
UINT32 gpnStatFuncApiViewPortReplaceNotify(gpnSockMsg *pgpnStatMsgSp, UINT32 len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_STAT_FUNC_API_H_*/


