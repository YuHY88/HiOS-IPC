/**********************************************************
* file name: gpnStatProcMain.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-03-11
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_PROC_MAIN_H_
#define _GPN_STAT_PROC_MAIN_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>


#include "type_def.h"
#include "stat_debug.h"
#include "gpnStatDataStruct.h"

#define GPN_STAT_PROC_GEN_OK 					GEN_SYS_OK
#define GPN_STAT_PROC_GEN_ERR 					GEN_SYS_ERR

#define GPN_STAT_PROC_GEN_YES 					GEN_SYS_YES
#define GPN_STAT_PROC_GEN_NO					GEN_SYS_NO

#define GPN_STAT_PROC_GEN_ENABLE 				GEN_SYS_ENABLE
#define GPN_STAT_PROC_GEN_DISABLE				GEN_SYS_DISABLE

#define GPN_STAT_PROC_GEN_08_FFFF				GEN_SYS_08_FFFF
#define GPN_STAT_PROC_GEN_16_FFFF				GEN_SYS_16_FFFF
#define GPN_STAT_PROC_GEN_32_FFFF				GEN_SYS_32_FFFF
/*#define GPN_STAT_PROC_GEN_64_FFFF		GEN_SYS_64_FFFF*/

#define GPN_STAT_PROC_PRINT(level, info...) 	GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_STAT_PROC_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_STAT_PROC_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_STAT_PROC_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_STAT_PROC_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_STAT_PROC_UD5						GEN_SYS_DEBUG_UD5
#define GPN_STAT_PROC_UD4						GEN_SYS_DEBUG_UD4
#define GPN_STAT_PROC_UD3						GEN_SYS_DEBUG_UD3
#define GPN_STAT_PROC_UD2						GEN_SYS_DEBUG_UD2
#define GPN_STAT_PROC_UD1						GEN_SYS_DEBUG_UD1
#define GPN_STAT_PROC_UD0						GEN_SYS_DEBUG_UD0


#define GPN_STAT_CORE_PROC_CYC				1

/*wait for other process in normal status */
#define GPN_STAT_FUNC_START_DELAY			40/*60*/

UINT32 gpnStatProcInit(void);
UINT32 gpnStatStartStatFunc(void);
void gpnStatCoreProcInit(void);
//void gpnStatCoreProc(UINT32 cycSec, UINT32 rev);
int gpnStatCoreProc(void *para);

UINT32 gpnStatResetValidPortHistRecordAbout(void);
UINT32 gpnStatXCycRecReSet(stStatXCycDataRecd *pxCycDataRecd);
UINT32 gpnStatValidPortProc(void);
UINT32 gpnStatCollectBaseStatData(stStatScanTypeDef *pstatScanType, stStatLocalPortNode *pStatLocalNode);
UINT32 gpnStatThresholdProc(stStatScanTypeDef *pstatScanType, stStatLocalPortNode *pStatLocalNode);
UINT32 gpnStatRiseStatAlarm(stStatScanTypeDef *pstatScanType,
	stStatXCycDataRecd *statXCycRec, optObjOrient *pPortIndex, UINT32 cycClass);
UINT32 gpnStatClearStatAlarm(stStatScanTypeDef *pstatScanType,
	stStatXCycDataRecd *statXCycRec, optObjOrient *pPortIndex, UINT32 cycClass);
UINT32 gpnStatRiseStatEvent(stStatScanTypeDef *pstatScanType,
	stStatXCycDataRecd *statXCycRec, optObjOrient *pPortIndex, UINT32 cycClass);
UINT32 gpnStatClearStatEventMark(stStatXCycDataRecd *statXCycRec);
UINT32 gpnStatHistDataTrapProc(void);
UINT32 gpnStatPerTaskPushStackProc(void);
UINT32 gpnStatPerPortPushStackProc(stStatTaskNode *ptaskNode);
UINT32 gpnStatPerCycClassPushStackProc(stStatXCycDataRecd *pxCycDataRecd);
UINT32 gpnStatDeleteStatAlarm(stStatScanTypeDef *pstatScanType,
	stStatXCycDataRecd *statXCycRec, optObjOrient *pPortIndex, UINT32 cycClass);


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_STAT_PROC_MAIN_H_*/

