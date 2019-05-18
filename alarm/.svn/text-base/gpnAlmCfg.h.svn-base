/**********************************************************
* file name: gpnAlmCfg.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-13
* function: 
*    define alarm Config detail process
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_CFG_H_
#define _GPN_ALM_CFG_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include "type_def.h"
#include "socketComm/gpnGlobalPortIndexDef.h"

#define GPN_ALM_CFG_OK 								GEN_SYS_OK
#define GPN_ALM_CFG_ERR 							GEN_SYS_ERR

#define GPN_ALM_CFG_CANCEL 							0
#define GPN_ALM_CFG_ENABLE 							GEN_SYS_ENABLE
#define GPN_ALM_CFG_DISABLE 						GEN_SYS_DISABLE

#define GPN_ALM_CFG_YES 							GEN_SYS_YES
#define GPN_ALM_CFG_NO 								GEN_SYS_NO

#define GPN_ALM_CFG_08_FFFF							GEN_SYS_08_FFFF
#define GPN_ALM_CFG_16_FFFF							GEN_SYS_16_FFFF
#define GPN_ALM_CFG_32_FFFF							GEN_SYS_32_FFFF
/*#define GPN_ALM_CFG_64_FFFF						GEN_SYS_64_FFFF*/


#define GPN_ALM_CFG_PRINT(level, info...) 			GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_ALM_CFG_EGP 							GEN_SYS_DEBUG_EGP
#define GPN_ALM_CFG_SVP 							GEN_SYS_DEBUG_SVP
#define GPN_ALM_CFG_CMP 							GEN_SYS_DEBUG_CMP
#define GPN_ALM_CFG_CUP 							GEN_SYS_DEBUG_CUP
#define GPN_ALM_CFG_UD5								GEN_SYS_DEBUG_UD5
#define GPN_ALM_CFG_UD4								GEN_SYS_DEBUG_UD4
#define GPN_ALM_CFG_UD3								GEN_SYS_DEBUG_UD3
#define GPN_ALM_CFG_UD2								GEN_SYS_DEBUG_UD2
#define GPN_ALM_CFG_UD1								GEN_SYS_DEBUG_UD1
#define GPN_ALM_CFG_UD0								GEN_SYS_DEBUG_UD0

UINT32 gpnAlmCfgScalarInitCfg(void);
UINT32 gpnAlmCfgTableInitCfg(void);
UINT32 gpnAlmCfgCurrAlmIndexNewStart(UINT32 sIndex);
UINT32 gpnAlmCfgHistAlmIndexNewStart(UINT32 sIndex);
UINT32 gpnAlmCfgEventIndexNewStart(UINT32 sIndex);
UINT32 gpnAlmCfgRiseDelay(UINT32 opt);
UINT32 gpnAlmGetRiseDelay(UINT32 *opt);
UINT32 gpnAlmCfgCleanDelay(UINT32 opt);
UINT32 gpnAlmGetCleanDelay(UINT32 *opt);
UINT32 gpnAlmCfgBuzzerEn(UINT32 opt);
UINT32 gpnAlmGetBuzzerEn(UINT32 *opt);
UINT32 gpnAlmCfgBuzzerClr(UINT32 opt);
UINT32 gpnAlmCfgBuzzerThre(UINT32 opt);
UINT32 gpnAlmGetBuzzerThre(UINT32 *opt);
UINT32 gpnAlmCfgRestMode(UINT32 opt);
UINT32 gpnAlmCfgRestModeChgProc(UINT32 opt);
UINT32 gpnAlmGetRestMode(UINT32 *opt);
UINT32 gpnAlmCfgReverMode(UINT32 opt);
UINT32 gpnAlmCfgReverModeChgProc(UINT32 opt);
UINT32 gpnAlmGetReverMode(UINT32 *opt);
UINT32 gpnAlmCfgCurrAlmDBCyc(UINT32 opt);
UINT32 gpnAlmGetCurrAlmDBCyc(UINT32 *opt);
UINT32 gpnAlmCfgHistAlmDBCyc(UINT32 opt);
UINT32 gpnAlmGetHistAlmDBCyc(UINT32 *opt);
UINT32 gpnAlmCfgEventDBCyc(UINT32 opt);
UINT32 gpnAlmGetEventDBCyc(UINT32 *opt);
UINT32 gpnAlmGetCurrDBSize(UINT32 *opt);
UINT32 gpnAlmGetHistDBSize(UINT32 *opt);
UINT32 gpnAlmGetEventDBSize(UINT32 *opt);
UINT32 gpnAlmCfgSubAlmRank(UINT32 almSubType, UINT32 almRank);
UINT32 gpnAlmCfgSubAlmScreen(UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmCfgSubAlmFilt(UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmCfgSubAlmReport(UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmCfgSubAlmRecord(UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmCfgPortOfSubAlmRank(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 almRank);
UINT32 gpnAlmCfgPortOfSubAlmScreen(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmCfgPortOfSubAlmFilt(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmCfgPortOfSubAlmReport(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmCfgPortOfSubAlmRecord(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmCfgPortOfSubAlmReverse(objLogicDesc *pPortInfo, UINT32 almSubType,UINT32 opt);
UINT32 gpnAlmCfgPortOfSubAlmPCountClear(objLogicDesc *pPortInfo, UINT32 almSubType);
UINT32 gpnAlmCfgPortAlmMon(objLogicDesc *plocalPIndex, UINT32 opt);
UINT32 gpnAlmCfgAlmRankSummary(UINT32 almRank,UINT32 opt);
UINT32 gpnAlmUppePortNodeSet(objLogicDesc downPort, objLogicDesc uppePort);

UINT32 gpnAlmCfgInitCfg(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_CFG_H_*/
