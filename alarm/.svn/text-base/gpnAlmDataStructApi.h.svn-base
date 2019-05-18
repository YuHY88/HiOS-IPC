/**********************************************************
* file name: gpnAlmDataStructApi.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-09-08
* function: 
*    define alarm inner data struct API
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_DATA_STRUCT_API_H_
#define _GPN_ALM_DATA_STRUCT_API_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnGlobalPortIndexDef.h"

#include "type_def.h"
#include "gpnAlmTypeStruct.h"

#define GPN_ALM_DST_OK 								GEN_SYS_OK
#define GPN_ALM_DST_ERR 							GEN_SYS_ERR

#define GPN_ALM_DST_CANCEL 							0
#define GPN_ALM_DST_ENABLE 							GEN_SYS_ENABLE
#define GPN_ALM_DST_DISABLE 						GEN_SYS_DISABLE

#define GPN_ALM_DST_YES 							GEN_SYS_YES
#define GPN_ALM_DST_NO 								GEN_SYS_NO

#define GPN_ALM_DST_08_FFFF							GEN_SYS_08_FFFF
#define GPN_ALM_DST_16_FFFF							GEN_SYS_16_FFFF
#define GPN_ALM_DST_32_FFFF							GEN_SYS_32_FFFF
/*#define GPN_ALM_DST_64_FFFF						GEN_SYS_64_FFFF*/

#define GPN_ALM_DST_PRINT(level, info...) 			GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_ALM_DST_EGP 							GEN_SYS_DEBUG_EGP
#define GPN_ALM_DST_SVP 							GEN_SYS_DEBUG_SVP
#define GPN_ALM_DST_CMP 							GEN_SYS_DEBUG_CMP
#define GPN_ALM_DST_CUP 							GEN_SYS_DEBUG_CUP
#define GPN_ALM_DST_UD5								GEN_SYS_DEBUG_UD5
#define GPN_ALM_DST_UD4								GEN_SYS_DEBUG_UD4
#define GPN_ALM_DST_UD3								GEN_SYS_DEBUG_UD3
#define GPN_ALM_DST_UD2								GEN_SYS_DEBUG_UD2
#define GPN_ALM_DST_UD1								GEN_SYS_DEBUG_UD1
#define GPN_ALM_DST_UD0								GEN_SYS_DEBUG_UD0

#define GPN_ALM_DST_SUFFIX_LEN						64
#define GPN_EVT_DST_SUFFIX_LEN						GPN_ALM_DST_SUFFIX_LEN

#define GPN_ALM_DST_ATTRI_BIT_RANK                  0
#define GPN_ALM_DST_ATTRI_BIT_MASK                  1
#define GPN_ALM_DST_ATTRI_BIT_FILT                  2
#define GPN_ALM_DST_ATTRI_BIT_REPT                  3
#define GPN_ALM_DST_ATTRI_BIT_RECD                  4
#define GPN_ALM_DST_ATTRI_BIT_REVS                  5

typedef struct _stDSAlmAttri_
{
	UINT32 bitMark;
	UINT32 rank;
	UINT32 mask;
	UINT32 filt;
	UINT32 rept;
	UINT32 recd;
	UINT32 revs;
}stDSAlmAttri;

typedef struct _stDSCurrAlm_
{
	UINT32 index;
	
	UINT32 almType;
	UINT32 ifIndex;
	UINT32 ifIndex2;
	UINT32 ifIndex3;
	UINT32 ifIndex4;
	UINT32 ifIndex5;
	UINT32 devIndex;
	UINT32 level;
	UINT32 count;
	UINT32 firstTime;
	UINT32 thisTime;

	/* add for VPLS */
	UINT32 suffixLen;
	UINT8 almSuffix[GPN_ALM_DST_SUFFIX_LEN];
}stDSCurrAlm;

typedef struct _stDSHistAlm_
{
	UINT32 index;
	
	UINT32 almType;
	UINT32 ifIndex;
	UINT32 ifIndex2;
	UINT32 ifIndex3;
	UINT32 ifIndex4;
	UINT32 ifIndex5;
	UINT32 devIndex;
	UINT32 level;
	UINT32 count;
	UINT32 firstTime;
	UINT32 thisTime;
	UINT32 disapTime;

	/* add for VPLS */
	UINT32 suffixLen;
	UINT8 almSuffix[GPN_ALM_DST_SUFFIX_LEN];
	
}stDSHistAlm;

typedef struct _stDSEvent_
{
	UINT32 index;
	
	UINT32 eventType;
	UINT32 ifIndex;
	UINT32 ifIndex2;
	UINT32 ifIndex3;
	UINT32 ifIndex4;
	UINT32 ifIndex5;
	UINT32 devIndex;
	UINT32 level;
	UINT32 detail;

	/*add for embedded software CLI display */
	UINT32 thisTime;

	/* add for display detail like */
	UINT32 suffixLen;
	UINT8 evtSuffix[GPN_EVT_DST_SUFFIX_LEN];
}stDSEvent;

UINT32 gpnAlmDSApiRiseDelayCfg(UINT32 opt);
UINT32 gpnAlmDSApiRiseDelayGet(UINT32 *opt);
UINT32 gpnAlmDSApiCleanDelayCfg(UINT32 opt);
UINT32 gpnAlmDSApiCleanDelayGet(UINT32 *opt);
UINT32 gpnAlmDSApiBuzzerEnCfg(UINT32 opt);
UINT32 gpnAlmDSApiBuzzerEnGet(UINT32 *opt);
UINT32 gpnAlmDSApiBuzzerClrCfg(UINT32 opt);
UINT32 gpnAlmDSApiBuzzerThreCfg(UINT32 opt);
UINT32 gpnAlmDSApiBuzzerThreGet(UINT32 *opt);
UINT32 gpnAlmDSApiRestModeCfg(UINT32 opt);
UINT32 gpnAlmDSApiRestModeGet(UINT32 *opt);
UINT32 gpnAlmDSApiReverModeCfg(UINT32 opt);
UINT32 gpnAlmDSApiReverModeGet(UINT32 *opt);
UINT32 gpnAlmDSApiCurrAlmDBCycCfg(UINT32 opt);
UINT32 gpnAlmDSApiCurrAlmDBCycGet(UINT32 *opt);
UINT32 gpnAlmDSApiHistAlmDBCycCfg(UINT32 opt);
UINT32 gpnAlmDSApiHistAlmDBCycGet(UINT32 *opt);
UINT32 gpnAlmDSApiEventDBCycCfg(UINT32 opt);
UINT32 gpnAlmDSApiEventDBCycGet(UINT32 *opt);
UINT32 gpnAlmDSApiCurrDBSizeGet(UINT32 *opt);
UINT32 gpnAlmDSApiHistDBSizeGet(UINT32 *opt);
UINT32 gpnAlmDSApiEventDBSizeGet(UINT32 *opt);
UINT32 gpnAlmDSApiPortMonStateGet(objLogicDesc *plocalPIndex, UINT32 *enState);
UINT32 gpnAlmDSApiPortMonStateGetNext(objLogicDesc *plocalPIndex, objLogicDesc *pnextPIndex, UINT32 *enState);
UINT32 gpnAlmDSApiAlmAttributeGet(UINT32 almSubType, stDSAlmAttri *almAttri);
UINT32 gpnAlmDSApiAlmAttributeGetNext(UINT32 almSubType, UINT32 *nextSubType, stDSAlmAttri *almAttri);
UINT32 gpnAlmDSApiPortAlmAttributeGet(objLogicDesc *plocalPIndex, UINT32 almSubType, stDSAlmAttri *almAttri);
UINT32 gpnAlmDSApiPortAlmAttributeGetNext(objLogicDesc *plocalPIndex, 
	UINT32 almSubType, objLogicDesc *pnextPIndex, UINT32 *nextSubType, stDSAlmAttri *almAttri);
UINT32 gpnAlmDSApiAttributeTableGet(objLogicDesc *plocalPIndex, UINT32 almSubType, stDSAlmAttri *almAttri);
UINT32 gpnAlmDSApiAttributeTableGetNext(objLogicDesc *plocalPIndex, 
	UINT32 almSubType, objLogicDesc *pnextPIndex, UINT32 *nextSubType, stDSAlmAttri *almAttri);
UINT32 gpnAlmDSApiCurrAlmGet(UINT32 index, stDSCurrAlm *currAlm);
UINT32 gpnAlmDSApiCurrAlmGetNext(UINT32 index, UINT32 *nextIndex, stDSCurrAlm *currAlm);
UINT32 gpnAlmDSApiHistAlmGet(UINT32 index, stDSHistAlm *histAlm);
UINT32 gpnAlmDSApiHistAlmGetNext(UINT32 index, UINT32 *nextIndex, stDSHistAlm *histAlm);
UINT32 gpnAlmDSApiEventGet(UINT32 index, stDSEvent *event);
UINT32 gpnAlmDSApiEventGetNext(UINT32 index, UINT32 *nextIndex, stDSEvent *event);
UINT32 gpnAlmDSApiCurrAlmPCountClear(UINT32 index);
UINT32 gpnAlmDSApiSubAlmRankCfg(UINT32 almSubType, UINT32 almRank);
UINT32 gpnAlmDSApiPortOfSubAlmRankCfg(objLogicDesc *plocalPIndex, UINT32 almSubType, UINT32 almRank);
UINT32 gpnAlmDSApiSubAlmScreenCfg(UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmDSApiPortOfSubAlmScreenCfg(objLogicDesc *plocalPIndex, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmDSApiSubAlmFiltCfg(UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmDSApiPortOfSubAlmFiltCfg(objLogicDesc *plocalPIndex, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmDSApiSubAlmReportCfg(UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmDSApiPortOfSubAlmReportCfg(objLogicDesc *plocalPIndex, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmDSApiSubAlmRecordCfg(UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmDSApiPortOfSubAlmRecordCfg(objLogicDesc *plocalPIndex, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmDSApiPortOfSubAlmReverseCfg(objLogicDesc *plocalPIndex, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmDSApiPortMonStateCfg(objLogicDesc *plocalPIndex, UINT32 enStat);
UINT32 gpnAlmDSApiCurrAlmProductCountClear(UINT32 currIndex);
UINT32 gpnAlmDSApiAlmAttribModify(objLogicDesc *localPIndex, UINT32 almSubType, stDSAlmAttri *dsAttri);

UINT32 gpnAlmDSApiPortTypeToAlmScanType(UINT32 portType, stAlmScanTypeDef **ppAlmScanType);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_ALM_DATA_STRUCT_API_H__*/
