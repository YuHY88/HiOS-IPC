/**********************************************************
* file name: gpnAlmAlmNotify.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-12
* function: 
*    define alarms notify by processes process detail
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_ALM_NOTIFY_H_
#define _GPN_ALM_ALM_NOTIFY_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alarm_debug.h"

#include "socketComm/gpnGlobalPortIndexDef.h"
#include "gpnAlmScan.h"


#define GPN_ALM_ANT_PRINT(level, info...) 		GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_ALM_ANT_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_ALM_ANT_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_ALM_ANT_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_ALM_ANT_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_ALM_ANT_UD5						 	GEN_SYS_DEBUG_UD5
#define GPN_ALM_ANT_UD4							GEN_SYS_DEBUG_UD4
#define GPN_ALM_ANT_UD3							GEN_SYS_DEBUG_UD3
#define GPN_ALM_ANT_UD2							GEN_SYS_DEBUG_UD2
#define GPN_ALM_ANT_UD1							GEN_SYS_DEBUG_UD1
#define GPN_ALM_ANT_UD0							GEN_SYS_DEBUG_UD0


#define GPN_ALM_ANT_08_NULL						GEN_SYS_08_NULL
#define GPN_ALM_ANT_16_NULL						GEN_SYS_16_NULL
#define GPN_ALM_ANT_32_NULL						GEN_SYS_32_NULL
/*#define GPN_ALM_ANT_64_NULL						GEN_SYS_64_NULL*/

#define GPN_ALM_ANT_08_FFFF						GEN_SYS_08_FFFF
#define GPN_ALM_ANT_16_FFFF						GEN_SYS_16_FFFF
#define GPN_ALM_ANT_32_FFFF						GEN_SYS_32_FFFF
/*#define GPN_ALM_ANT_64_FFFF						GEN_SYS_64_FFFF*/


#define GPN_ALM_UNKOWN 							0
#define GPN_ALM_ARISE 							1
#define GPN_ALM_CLEAN							2

UINT32 gpnAlmANtProductReport(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd);
UINT32 gpnAlmANtDisappReport(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd);
UINT32 gpnAlmANtEvtProductReport(objLogicDesc *pViewPort, stEvtValueRecd *pEvtValueRecd);

UINT32 gpnAlmANtCurrAlmInsertDS(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd);
UINT32 gpnAlmANtCurrAlmOutDS(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd);
UINT32 gpnAlmANtHistAlmInsertDS(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd);

UINT32 gpnAlmANtSubAlmStatRecord(stAlmScanTypeDef *pAlmScanType,
						stAlmPeerNode *pAlmPeerPort, UINT32 almSubType, UINT32 almSta);
UINT32 gpnAlmANtBaseSubTypeProc(optObjOrient *pPortInfo, UINT32 almSubType, UINT32 almSta);
UINT32 gpnAlmANtCfmBaseSubTypeProc(optObjOrient *pPortInfo, UINT32 almSubType, UINT32 almSta);
UINT32 gpnAlmANtBaseScanTypeProc(optObjOrient *pPortInfo, UINT32 *pAlmData, UINT32 len);

UINT32 gpnAlmANtEvtInsertDS(objLogicDesc *pViewPort, stEvtValueRecd *pEvtValueRecd);

UINT32 gpnAlmANtSubEvtStatusRecord(stAlmScanTypeDef *pAlmScanType,
					stAlmPeerNode *pAlmPeerPort, UINT32 evtSubType, UINT32 evtDetial);
UINT32 gpnAlmANtEvtBaseSubTypeProc(optObjOrient *pPortInfo, UINT32 evtSubType, UINT32 detial);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif/* _GPN_ALM_ALM_NOTIFY_H_ */

