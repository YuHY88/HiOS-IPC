/**********************************************************
* file name: gpnAlmScan.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-09
* function: 
*    define scan and process alarms details
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_SCAN_H_
#define _GPN_ALM_SCAN_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "socketComm/gpnAlmTypeDef.h"
#include "socketComm/gpnSockIfmMsgDef.h"
#include "socketComm/gpnVariableLenList.h"
#include "socketComm/gpnGlobalPortIndexDef.h"
#include "devCoreInfo/gpnDevStatus.h"


#include "alarm_debug.h"
#include "gpnAlmTypeStruct.h"

#define GPN_ALM_SCAN_PRINT(level, info...) 		GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_ALM_SCAN_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_ALM_SCAN_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_ALM_SCAN_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_ALM_SCAN_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_ALM_SCAN_UD5						GEN_SYS_DEBUG_UD5
#define GPN_ALM_SCAN_UD4						GEN_SYS_DEBUG_UD4
#define GPN_ALM_SCAN_UD3						GEN_SYS_DEBUG_UD3
#define GPN_ALM_SCAN_UD2						GEN_SYS_DEBUG_UD2
#define GPN_ALM_SCAN_UD1						GEN_SYS_DEBUG_UD1
#define GPN_ALM_SCAN_UD0						GEN_SYS_DEBUG_UD0

#define GPN_EVT_SCAN_PRINT(level, info...) 		GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_EVT_SCAN_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_EVT_SCAN_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_EVT_SCAN_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_EVT_SCAN_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_EVT_SCAN_UD5						GEN_SYS_DEBUG_UD5
#define GPN_EVT_SCAN_UD4						GEN_SYS_DEBUG_UD4
#define GPN_EVT_SCAN_UD3						GEN_SYS_DEBUG_UD3
#define GPN_EVT_SCAN_UD2						GEN_SYS_DEBUG_UD2
#define GPN_EVT_SCAN_UD1						GEN_SYS_DEBUG_UD1
#define GPN_EVT_SCAN_UD0						GEN_SYS_DEBUG_UD0


#define GPN_ALM_SCAN_08_NULL					GEN_SYS_08_NULL
#define GPN_ALM_SCAN_16_NULL					GEN_SYS_16_NULL
#define GPN_ALM_SCAN_32_NULL					GEN_SYS_32_NULL
/*#define GPN_ALM_SCAN_64_NULL					GEN_SYS_64_NULL*/

#define GPN_ALM_SCAN_08_FFFF					GEN_SYS_08_FFFF
#define GPN_ALM_SCAN_16_FFFF					GEN_SYS_16_FFFF
#define GPN_ALM_SCAN_32_FFFF					GEN_SYS_32_FFFF
/*#define GPN_ALM_SCAN_64_FFFF					GEN_SYS_64_FFFF*/

/*消息参数定义*/
/*槽位设备及远端状态*/
#define GPN_ALM_DEV_STA_NULL					GPN_DEV_STA_NULL
#define GPN_ALM_DEV_STA_PULL					GPN_DEV_STA_PULL
#define GPN_ALM_DEV_STA_INSERT					GPN_DEV_STA_INSERT
#define GPN_ALM_DEV_STA_RUN						GPN_DEV_STA_RUN
#define GPN_ALM_DEV_STA_ERR						GPN_DEV_STA_ERR
#define GPN_ALM_DEV_STA_FAKE					GPN_DEV_STA_FAKE

#define EQU_SLOT_MAX_ID 						IFM_SLOT_MAX_ID
#define EQU_SLOT_BACK_BOAD 						0

#define EQU_SLOT_NUM_TOT 						((EQU_SLOT_MAX_ID))+1)

#define GPN_ALM_PRESCAN_HASH 					20

/*告警动作*/
#define GPN_ALM_UNKOWN 							0
#define GPN_ALM_ARISE 							1
#define GPN_ALM_CLEAN							2

/*告警默认动作延时*/
#define GPN_ALM_DEF_ARISE_DELAY 				3
#define GPN_ALM_DEF_CLEAN_DELAY					1
#define GPN_ALM_DEF_ARISE_DELAY_MAX				20
#define GPN_ALM_DEF_CLEAN_DELAY_MAX				20
#define GPN_ALM_SHAKE_PROC_CYC					((GPN_ALM_DEF_ARISE_DELAY_MAX)*3)
#define GPN_ALM_SHAKE_PROC_POINT				((GPN_ALM_SHAKE_PROC_CYC) - 1)


/*定时器时长设置*/
#define GPN_ALM_TIMER_NORMAL					1000

/*告警反转模式*/
#define GPN_ALM_INVE_MODE_AUTO 					1
#define GPN_ALM_INVE_MODE_MANU 					2
#define GPN_ALM_INVE_MODE_DISABLE				3

/*告警抑制模式*/
#define GPN_ALM_REST_LEVEL_MODE 				1
#define GPN_ALM_REST_TIME_MODE 					2
#define GPN_ALM_REST_MODE_DISABLE				3

typedef void (*ALMRISETRAP)(UINT32 *pInfo);

typedef struct _stAlmShakeUnit
{
	UINT32 stat;
	UINT32 shakeCnt;
	UINT32 riseTime;
	UINT32 dispTime;
}stAlmShakeUnit;

typedef struct _stAlmShakeRecd
{
	stAlmShakeUnit shakeUnit[2];
}stAlmShakeRecd;

typedef struct _stAlmValueRecd
{
	UINT32 almSubType;
	
	UINT8 iAlmValue;
	UINT8 iDelayCount;
	UINT8 portAlmRank;
	UINT8 revd;
	
	UINT32 prodCnt;
	UINT32 firstTime;
	UINT32 thisTime;
	UINT32 disapTime;
	UINT32 index;
}stAlmValueRecd;

typedef struct _stEvtValueRecd
{
	UINT32 evtSubType;
	
	UINT32 detial;
	UINT32 evtRank;
	UINT32 thisTime;
	UINT32 index;
}stEvtValueRecd;

typedef struct _stAlmPreScanQuen
{
	NODE node;
	/* now , local port 1 : 1 peer port */
	/* local port list */
	LIST almPreScanQuen[GPN_ALM_PRESCAN_HASH];
	UINT32 preScanNodeNum[GPN_ALM_PRESCAN_HASH];
	/* peer port list */
	LIST almPeerPortQuen[GPN_ALM_PRESCAN_HASH];
	UINT32 peerPortNodeNum[GPN_ALM_PRESCAN_HASH];
	UINT32 hashKey;
	GPN_ALM_TYPE almScanType;
	stAlmScanTypeDef *almScanTypeNode;

}stAlmPreScanQuen;

typedef struct _stAlmScanProcPortDef_
{	
	struct _stAlmScanProcPortDef_ *pUppeAlmScanPort;/*抑制关系根节点端口*/
	struct _stAlmScanProcPortDef_ *pBackAlmScanPort;
	
	UINT8 scanQuenValid;
	UINT8 preQuenValid;
	UINT8 addQuenMode;
	UINT8 revd2;

	UINT32 iScanAlmDate[GPN_ALM_PAGE_NUM];
	UINT32 iBaseAlmDate[GPN_ALM_PAGE_NUM];
	UINT32 iRestAlmDate[GPN_ALM_PAGE_NUM];
	UINT32 iInveAlmDate[GPN_ALM_PAGE_NUM];
	UINT32 iReptAlmDate[GPN_ALM_PAGE_NUM];
	UINT32 iDbSvAlmDate[GPN_ALM_PAGE_NUM];
	
	UINT32 iFrehAlmMark[GPN_ALM_PAGE_NUM+1];
	UINT32 iRestAlmMark;
	UINT32 iDelyAlmMark[GPN_ALM_PAGE_NUM+1];
	UINT32 iSureAlmMark[GPN_ALM_PAGE_NUM+1];
	
	/*动作属性是否基于端口*/
	/*告警屏蔽是否按端口:0为按类操作，1为按端口操作，默认为按类操作*/
	UINT32 isScreBasePort[GPN_ALM_PAGE_NUM];
	/*告警过滤是否按端口:0为按类操作，1为按端口操作，默认为按类操作*/
	UINT32 isFiltBasePort[GPN_ALM_PAGE_NUM];
	/*告警上报是否按端口:0为按类操作，1为按端口操作，默认为按类操作*/
	UINT32 isReptBasePort[GPN_ALM_PAGE_NUM];
	/*告警级别是否按端口:0为按类操作，1为按端口操作，默认为按类操作(按端口时，配置值在AlmValueBuff保存)*/
	UINT32 isRankBasePort[GPN_ALM_PAGE_NUM];
	/*告警记录是否按端口:0为按类操作，1为按端口操作，默认为按类操作*/
	UINT32 isReckBasePort[GPN_ALM_PAGE_NUM];
	
	/*基于端口的配置信息*/
	/* 0 means "screen", 1 means "no screen(default)", then base template, then base cfg */
	UINT32 iScreAlmInfo[GPN_ALM_PAGE_NUM];
	/* 0 means "not reverse(default)", 1 means "reverse", then base template, then base cfg */
	UINT32 iInveAlmInfo[GPN_ALM_PAGE_NUM];
	/* 0 means "filt", 1 means "no filt(default)", then base template, then base cfg */
	UINT32 iFiltAlmInfo[GPN_ALM_PAGE_NUM];
	/* 0 means "not report", 1 means "report(default)", then base template, then base cfg */
	UINT32 iReptAlmInfo[GPN_ALM_PAGE_NUM];
	/* 0 means "not record", 1 means "record(default)", then base template, then base cfg */
	UINT32 iReckAlmInfo[GPN_ALM_PAGE_NUM];

	/*UINT32 iScanEvtDate[GPN_EVT_PAGE_NUM];
	UINT32 iScreEvtDate[GPN_EVT_PAGE_NUM];
	UINT32 iReptEvtDate[GPN_EVT_PAGE_NUM];*/
	UINT32 iFrehEvtMark[GPN_EVT_PAGE_NUM+1];
	/*UINT32 iScreEvtInfo;			此处限制事件的多少,这里限制为64*/
	/*UINT32 iReptEvtInfo;			此处限制事件的多少,这里限制为64*/

	UINT32 almNumInScanPort;
	stAlmValueRecd *AlmValueBuff;

	UINT32 valShakeUnit;
	UINT32 iShakAlmMark[GPN_ALM_PAGE_NUM+1];
	UINT32 iShakOldMark[GPN_ALM_PAGE_NUM+1];
	stAlmShakeRecd *AlmShakeBuff;
	
}stAlmScanPortInfo;

typedef struct _stAlmScanPeerPortNodeDef
{
	NODE node;
	/* alarm comm with processes */
	optObjOrient peerPort;

	/* port data struct */
	stAlmScanPortInfo *pAlmScanPort;
}stAlmPeerNode;

typedef struct _stAlmScanLocalPortNodeDef
{
	NODE node;
	/* alarm comm with DA( easy view ) */
	objLogicDesc localPort;
	/* alarm record and show port */
	objLogicDesc viewPort;
	/* local 1 :1 peer port */
	stAlmPeerNode *pPeerNode;
	
	/* port data struct */
	stAlmScanPortInfo *pAlmScanPort;
}stAlmLocalNode;

typedef struct _stAlmPreScanIndex
{
	NODE node;
	stAlmLocalNode *pAlmLocalPort;
}stAlmPreScanIndex;

/*端口类型与扫描类型对应相关*/
typedef struct _stAlmPortTpVsScanTpTemplate
{
	UINT32 portType;
	GPN_ALM_TYPE almScanType;
	const char * portTpName;
}stAlmPTpVsSTpTemp;
typedef struct _stppAlmPreScanNode
{
	UINT32 slot;
	stAlmPreScanQuen *pAlmPreScanNode;
}stppAlmPreScanNode;
typedef struct _stPortTpToAlmScanTp
{
	NODE node;
	UINT32 almSubTpNumInPortType;
	stppAlmPreScanNode *ppAlmPreScanNode;
	UINT32 portType;
	const char * portTpName;
}stPortTpToAlmScanTp;
typedef struct _stPortTpVsAlmScanTpRelation
{
	LIST PTpVsSTpQuen;
	UINT32 sysSupPortTpNum;
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stppAlmPreScanNode *pAlmPreScanNode;
}stPTpVsAlmSTpRelation;

typedef struct _stPortObjCapalityList
{
	UINT32 portType;
	UINT32 portNum;
}stPortObjCL;


typedef stFixPorcReg stAlmPortObjCL;

typedef struct _stUnitAlmProcSpace
{
	/* slot perScan Port about */
	LIST almPreScanTpQuen;
	UINT32 quenNodeNum;

	/* localDev about */
	UINT32 devIndex;
	UINT32 slotDevSta;

	/* mem space about */
	stAlmPreScanQuen *pAlmPreScanQuenMemSpace;
	
	/* remote about */
	LIST RemDevAlmPorcNodeQuen;
	UINT32 RemDevAlmPorcNodeNum;
}stUnitAlmProcSpace;
typedef struct _stRemDevAlmProcNode_
{
	NODE node;
	UINT32 devIndex;
	UINT32 godDevIndex;
	objLogicDesc godPortIndex;
	void *pRemDevSelfSpace;
	UINT32 remotDevSta;
	
}stRemDevAlmProcNode;
typedef struct _stAlmGlobalCfg_
{
	UINT8 prodDelay;
	UINT8 disapDelay;
	UINT8 buzzerEn;
	UINT8 buzzerClr;
	
	UINT8 buzzerThre;
	UINT8 currDBCyc;
	UINT8 histDBCyc;
	UINT8 eventDBCyc;
	
	UINT32 currAlmIndex;
	UINT32 histAlmIndex;
	UINT32 eventIndex;
	
	UINT8 restMode;
	UINT8 reverMode;
	UINT16 rsd;
	
	UINT32 currDBSize;
	UINT32 histDBSize;
	UINT32 eventDBSize;

	UINT32 almAttribCfgNum;
	UINT32 almPortBaseMoniCfgNum;

	UINT32 rankAlmCnt[GPN_ALM_RANK_TOTAL_NUM];
	UINT32 highRank;
}stAlmGlobalCfg;
typedef struct _stEQUAlmProcSpace
{
	time_t almTaskTime;
	struct timeval expTime;
	
	stPTpVsAlmSTpRelation PTpVsSTpRelation;
	stUnitAlmProcSpace *unitAlmProcSpace;
	stAlmGlobalCfg almGlobalCfg;
}stEQUAlmProcSpace;

/*ALM消失结构特殊定义*/
typedef struct _stGPNAlmMsgSp_
{
	UINT32 msgId;
	UINT32 msgSubId;
	UINT64 portIndex;
	UINT64 portSubIndex;
	UINT32 optObj;
	UINT32 optSubObj;
	UINT32 optCode;
	UINT32 optSubCode;
	void *pCell;
}stGPNAlmMsgSp;
UINT32 gpnAlmMakePortIndex(UINT32 devIndex, UINT32 devPortTpNum,
	stAlmPortObjCL *pPortObjCL, objLogicDesc *portIndexBuff, objLogicDesc *upPtBuff);
UINT32 gpnAlmAutoInversNotify(stAlmLocalNode *pAlmLocalPort, UINT32 almType);
UINT32 gpnAlmAutoInversRecoverDSModify(stAlmLocalNode *pAlmLocalPort, UINT32 almSubType);
UINT32 gpnAlmScreenProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort);
UINT32 gpnAlmRestrainProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort);
UINT32 gpnAlmInvesalDlyInProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort);
UINT32 gpnAlmDlyProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort);
UINT32 gpnAlmInvesalProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort);
UINT32 gpnAlmFilterProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort);
UINT32 gpnAlmReportProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort);
UINT32 gpnAlmFreshAlmProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort);
UINT32 gpnAlmShakeAlmProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort);
UINT32 gpnAlmScanProc(void);
UINT32 gpnAlmShakeProc(void);
UINT32 gpnAlmPoolGetBaseAlmScanTypeProc(stAlmScanTypeDef *pAlmScanType, stAlmPeerNode *pAlmPeerPort);
UINT32 gpnMsgNotifyAlmBaseAlmScanTypeProc(optObjOrient* pPortInfo, UINT32* pAlmData, UINT32 len);
UINT32 gpnMsgNotifyAlmBaseSubTypeProc(optObjOrient* pPortInfo, UINT32 almSubType, UINT32 almSta);
UINT32 gpnAlmSubAlmNotifiedStatusRecord(stAlmScanTypeDef *pAlmScanType,
						stAlmPeerNode *pAlmPeerPort, UINT32 almSubType, UINT32 almSta);
UINT32 gpnMsgNotifyEvtBaseSubTypeProc(optObjOrient *pPortInfo, UINT32 evtSubType, UINT32 detial);
UINT32 gpnAlmSubEvtNotifiedStatusRecord(stAlmScanTypeDef *pAlmScanType,
						stAlmPeerNode *pAlmPeerPort, UINT32 evtSubType, UINT32 evtDetial);
UINT32 gpnAlmPeerPortReplaceNotify(optObjOrient *pOldPeerP, optObjOrient *pNewPeerP);
UINT32 gpnMasterToSlaveAlmInfoSynchProc(optObjOrient* pPortInfo, UINT32 almSubType, UINT32 almSta);
UINT32 gpnAlmDevStaNotifyPorc(UINT32 devIndex, objLogicDesc *pgodPortIndex, UINT32 sta);
UINT32 gpnAlmMonAlmCfg(objLogicDesc *plocalPIndex, UINT32 opt);
UINT32 gpnAlmMomAlmChgPorc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort, UINT32 opt);
UINT32 gpnAlmSubAlmScreenCfg(UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmSubBaseScreenChgPorc(stAlmScanTypeDef *pAlmScanType, stAlmSTCharacterDef *pAlmSubTpStr);
UINT32 gpnAlmSubAlmFiltCfg(UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmSubBaseFiltChgPorc(stAlmScanTypeDef *pAlmScanType, stAlmSTCharacterDef *pAlmSubTpStr);
UINT32 gpnAlmSubAlmReportCfg(UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmSubBaseReportChgPorc(stAlmScanTypeDef *pAlmScanType, stAlmSTCharacterDef *pAlmSubTpStr);
UINT32 gpnAlmSubAlmRankCfg(UINT32 almSubType, UINT32 almRank);
UINT32 gpnAlmSubAlmRankChgPorc(stAlmScanTypeDef *pAlmScanType, stAlmSTCharacterDef *pAlmSubTpStr, UINT32 almRank);
UINT32 gpnAlmSubAlmRecordCfg(UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmPortOfSubAlmRankCfg(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 almRank);
UINT32 gpnAlmPortBaseRankChgPorc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmPeerPort, stAlmSTCharacterDef *pAlmSubTpStr);
UINT32 gpnAlmPortOfSubAlmScreenCfg(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmPortBaseScreenChgPorc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmPeerPort, stAlmSTCharacterDef *pAlmSubTpStr);
UINT32 gpnAlmPortOfSubAlmReverseCfg(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmPortBaseInverChgPorc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmPeerPort, stAlmSTCharacterDef *pAlmSubTpStr);
UINT32 gpnAlmPortOfSubAlmFiltCfg(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmPortBaseFiltChgPorc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmPeerPort, stAlmSTCharacterDef *pAlmSubTpStr);
UINT32 gpnAlmPortOfSubAlmReportCfg(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmPortBaseReportChgPorc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmPeerPort, stAlmSTCharacterDef *pAlmSubTpStr);
UINT32 gpnAlmPortOfSubAlmRecordCfg(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt);
UINT32 gpnAlmRiseDelayCfg(UINT32 opt);
UINT32 gpnAlmRiseDelayGet(UINT32 *opt);
UINT32 gpnAlmCleanDelayCfg(UINT32 opt);
UINT32 gpnAlmCleanDelayGet(UINT32 *opt);
UINT32 gpnAlmBuzzerEnCfg(UINT32 opt);
UINT32 gpnAlmBuzzerEnGet(UINT32 *opt);
UINT32 gpnAlmBuzzerClrCfg(UINT32 opt);
UINT32 gpnAlmBuzzerThreCfg(UINT32 opt);
UINT32 gpnAlmBuzzerThreGet(UINT32 *opt);
UINT32 gpnAlmRestModeCfg(UINT32 opt);
UINT32 gpnAlmRestModeChgProc(UINT32 opt);
UINT32 gpnAlmRestModeGet(UINT32 *opt);
UINT32 gpnAlmReverModeCfg(UINT32 opt);
UINT32 gpnAlmReverModeChgProc(UINT32 opt);
UINT32 gpnAlmReverModeGet(UINT32 *opt);
UINT32 gpnAlmCurrAlmDBCycCfg(UINT32 opt);
UINT32 gpnAlmCurrAlmDBCycGet(UINT32 *opt);
UINT32 gpnAlmHistAlmDBCycCfg(UINT32 opt);
UINT32 gpnAlmHistAlmDBCycGet(UINT32 *opt);
UINT32 gpnAlmEventDBCycCfg(UINT32 opt);
UINT32 gpnAlmEventDBCycGet(UINT32 *opt);
UINT32 gpnAlmMonDBSizeGet(UINT32 *opt);
UINT32 gpnAlmAtrribDBSizeGet(UINT32 *opt);
UINT32 gpnAlmCurrDBSizeGet(UINT32 *opt);
UINT32 gpnAlmHistDBSizeGet(UINT32 *opt);
UINT32 gpnAlmEventDBSizeGet(UINT32 *opt);
UINT32 gpnAlmRelevChgAlmScanPortToPreScanQuen(stAlmPreScanIndex * pAlmPreScanIndex, UINT32 opt);
UINT32 gpnAlmRelevChgAlmScanPortToValidScanQuen(stAlmLocalNode *pAlmLocalPort, UINT32 opt);
UINT32 gpnAlmScanVariMonitorSpacePorc(objLogicDesc *pPortInfo,UINT32 opt);
UINT32 gpnAlmFixMonitorsSpacePerdistribution(UINT32 devIndex,UINT32 devPortTpNum,stAlmPortObjCL *pPortObjCL);
UINT32 gpnEQUAlmPreScanProc(void);
UINT32 gpnAlmScanDateStInit(void);
UINT32 gpnAlmPreScanInitClean(void);
UINT32 gpnAlmSysUseBitPToAlmUseBitP(stAlmScanTypeDef *pAlmScanType, UINT32 *pAlmDate, UINT32 len);
UINT32 gpnAlmSeekAlmUseBitPToSubTypeNumInScanType(stAlmScanTypeDef *pAlmScanType, UINT32 almUseBitP, UINT32 *pSubTpIndexInST);
UINT32 gpnAlmSeekAlmSubTpToOrderInScanTp(UINT32 almSubType, UINT32 *porderInScanTp);
UINT32 gpnAlmSeekScanTypeToSubType(stAlmScanTypeDef *pAlmScanType, UINT32 almUseBitp, stAlmSTCharacterDef **ppAlmSubTpStr);
UINT32 gpnEvtSeekScanTypeToSubType(stAlmScanTypeDef *pAlmScanType, UINT32 evtSysIndentify, stEvtSTCharacterDef **ppEvtSubTpStr);
UINT32 gpnAlmSeekPortTypeToAlmScanType(UINT32 portType, stAlmScanTypeDef **ppAlmScanType);
UINT32 gpnAlmSeekLocalPortIndexToAlmLocalPortNode(objLogicDesc *pPortInfo, stAlmLocalNode **ppAlmLocalPort);
UINT32 gpnAlmSeekLocalPortIndexToAlmScanPortIndex(objLogicDesc *pPortInfo, stAlmPreScanIndex **ppAlmPreScanIndex);
UINT32 gpnAlmSeekPeerPortIndexToAlmPeerNode(optObjOrient *pPortInfo, stAlmPeerNode **ppAlmPeerPort);
UINT32 gpnAlmSeekPortIndexToAlmPreScanNode(objLogicDesc *pPortInfo, stAlmPreScanQuen **ppAlmPreScanNode);
UINT32 gpnAlmSeekAlmSubTpToAlmSubTpNode(UINT32 almSubType, stAlmSTCharacterDef **ppAlmSubTpStr);
UINT32 gpnAlmSeekAlmSubTpToAlmScanTp(UINT32 almSubType, stAlmScanTypeDef **ppAlmScanType);
UINT32 gpnAlmSeekAlmSubTpToAlmSubTpBitValue(UINT32 almSubType, stAlmScanTypeDef *pAlmScanType, stAlmSTCharacterDef **ppAlmSubTpStr);
UINT32 gpnAlmSeekEvtSubTpToEvtSubTpNode(UINT32 evtSubType, stEvtSTCharacterDef **ppEvtSubTpStr);
UINT32 gpnAlmSeekEvtSubTpToEvtSubTpBitValue(UINT32 evtSubType, stAlmScanTypeDef *pAlmScanType, stEvtSTCharacterDef **ppEvtSubTpStr);
UINT32 gpnAlmRelevCheckIsPeerInPeerPortQuen(optObjOrient *pPortInfo, stAlmPreScanQuen *pAlmPreScanNode);
UINT32 gpnAlmRelevCheckIsLocalInPerScanQuen(objLogicDesc *pPortInfo, stAlmPreScanQuen *pAlmPreScanNode);
UINT32 gpnAlmScanPortInit(objLogicDesc *pPortInfo,
		stAlmScanTypeDef *pAlmScanType, stAlmPreScanIndex * pAlmPreScanIndex,
		stAlmLocalNode *pAlmLocalPort, stAlmPeerNode *pAlmPeerPort,
		stAlmScanPortInfo *pAlmScanPort, stAlmValueRecd *pAlmValueRecd,
		stAlmShakeRecd *pAlmShakeRecd);
UINT32 gpnAlmScanPortInfoClean(stAlmScanPortInfo *pAlmScanPort);
UINT32 debugGPNAlmPreScanNode(void);
UINT32 debugGpnAlmNowScanNode(void);
UINT32 debugGPNAlmScanStruct(void);
UINT32 debugGPNAlmScanTypeInfo(void);

UINT32 gpnAlmDataStInit(void);

UINT32 gpnAlmBackPortNodeGet(void);
UINT32 gpnAlmUpperAlmPassProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort);
UINT32 gpnEventScreenProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort);
UINT32 gpnEventReportProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif/* _GPN_ALM_SCAN_H_ */

