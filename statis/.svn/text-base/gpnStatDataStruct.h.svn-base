/**********************************************************
* file name: gpnStatDataStruct.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-22
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_DATA_STRUCT_H_
#define _GPN_STAT_DATA_STRUCT_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>

#include "socketComm/gpnSockIfmMsgDef.h"
#include "socketComm/gpnSockMsgDef.h"
#include "socketComm/gpnGlobalPortIndexDef.h"
#include "socketComm/gpnVariableLenList.h"
#include "socketComm/gpnStatTypeDef.h"
#include "devCoreInfo/gpnDevStatus.h"

#include "type_def.h"
#include "stat_debug.h"
#include "gpnStatTypeStruct.h"


#define GPN_STAT_DBS_GEN_OK 					GEN_SYS_OK
#define GPN_STAT_DBS_GEN_ERR 					GEN_SYS_ERR

#define GPN_STAT_DBS_GEN_YES 					GEN_SYS_YES
#define GPN_STAT_DBS_GEN_NO						GEN_SYS_NO

#define GPN_STAT_DBS_GEN_ENABLE					GEN_SYS_ENABLE
#define GPN_STAT_DBS_GEN_DISABLE				GEN_SYS_DISABLE
#define GPN_STAT_DBS_GEN_DELETE					GEN_SYS_DELETE

#define GPN_STAT_DBS_PRINT(level, info...) 		GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_STAT_DBS_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_STAT_DBS_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_STAT_DBS_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_STAT_DBS_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_STAT_DBS_UD5						GEN_SYS_DEBUG_UD5
#define GPN_STAT_DBS_UD4						GEN_SYS_DEBUG_UD4
#define GPN_STAT_DBS_UD3						GEN_SYS_DEBUG_UD3
#define GPN_STAT_DBS_UD2						GEN_SYS_DEBUG_UD2
#define GPN_STAT_DBS_UD1						GEN_SYS_DEBUG_UD1
#define GPN_STAT_DBS_UD0						GEN_SYS_DEBUG_UD0

#define GPN_STAT_DBS_08_NULL					GEN_SYS_08_NULL
#define GPN_STAT_DBS_16_NULL					GEN_SYS_16_NULL
#define GPN_STAT_DBS_32_NULL					GEN_SYS_32_NULL
/*#define GPN_STAT_DBS_64_NULL					GEN_SYS_64_NULL*/

#define GPN_STAT_DBS_08_FFFF					GEN_SYS_08_FFFF
#define GPN_STAT_DBS_16_FFFF					GEN_SYS_16_FFFF
#define GPN_STAT_DBS_32_FFFF					GEN_SYS_32_FFFF
/*#define GPN_STAT_DBS_64_FFFF					GEN_SYS_64_FFFF*/

#define GPN_STAT_INVALID_TASK_ID                GPN_STAT_DBS_32_FFFF
#define GPN_STAT_INVALID_TEMPLAT_ID             GPN_STAT_DBS_32_FFFF


/*GPN STAT EQU about define*/
#define EQU_SLOT_MAX_NUM 						32
#define EQU_MAIN_BOD_NUM 						0
#define EQU_SLOT_NUM_TOT 						((EQU_SLOT_MAX_NUM))+1)

#define GPN_STAT_PRESCAN_HASH 					31

/*GPN STAT SYS support port type max num*/
#define SYS_PORT_TYPE_MAX_NUM 					IFM_PORT_TYPE_MAX

/*GPN STAT DEVICE STATUS DEFINE */
#define GPN_STAT_DEV_STA_NULL					GPN_DEV_STA_NULL
#define GPN_STAT_DEV_STA_PULL					GPN_DEV_STA_PULL
#define GPN_STAT_DEV_STA_INSERT					GPN_DEV_STA_INSERT
#define GPN_STAT_DEV_STA_RUN					GPN_DEV_STA_RUN
#define GPN_STAT_DEV_STA_ERR					GPN_DEV_STA_ERR
#define GPN_STAT_DEV_STA_FAKE					GPN_DEV_STA_FAKE


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++ PORT STAT DATA RECD METHOD DEFINE +++++++++++++++++++ */
/*stat hist data fifo size define*/
#define GPN_STAT_HIST_SCYC_REC_NUM				6
#define GPN_STAT_HIST_UCYC_REC_NUM				6
#define GPN_STAT_HIST_LCYC_REC_NUM				2
/*XCYC_REC_NUM ? (SCYC_REC_NUM ? SCYC_REC_NUM : UCYC_REC_NUM)  : LCYC_REC_NUM*/
#define GPN_STAT_HIST_XCYC_REC_NUM				6

#define GPN_STAT_NULL_MODE 						0
#define GPN_STAT_FIX_MODE 						1
#define GPN_STAT_VARI_MODE						2

#define GPN_STAT_CYC_CLASS_SHORT 				1
#define GPN_STAT_CYC_CLASS_LONG					2
#define GPN_STAT_CYC_CLASS_USR					3
#define GPN_STAT_CYC_CLASS_BASE					4

#define GPN_STAT_MON_OPT_ENABLE					1
#define GPN_STAT_MON_OPT_DISABLE				2
#define GPN_STAT_MON_OPT_DELETE					3

#define GPN_STAT_TIME_POSITIVE					1
#define GPN_STAT_TIME_NEGATIVE					2

#define GPN_STAT_COUNT_TYPE_ABSOLUTE			1
#define GPN_STAT_COUNT_TYPE_DELTA				2

#define GPN_STAT_MAX_MON_OBJ_NUM				512
#define GPN_STAT_MAX_TASK_NUM					100
#define GPN_STAT_MAX_EVENT_THRED_TP_NUM			130
#define GPN_STAT_MAX_ALARM_THRED_TP_NUM			130
#define GPN_STAT_MAX_SUB_FILT_TP_NUM			130
#define GPN_STAT_MAX_SUB_REPORT_TP_NUM			130
#define GPN_STAT_MAX_TRAP_DATA_NUM				15/*130*/

#define GPN_STAT_SEARCH_HASH					9

#define GPN_STAT_MAX_SUB_INCLUDE				25

#define GPN_STAT_DATA_DWORD_NUM					2
#define GPN_STAT_DATA_HIGH_DWORD				0
#define GPN_STAT_DATA_LOW_DWORD					1

#define GPN_STAT_MINI_CYC_SECONDS				5

#define GPN_STAT_DATA_BIT_DEEP_32				1
#define GPN_STAT_DATA_BIT_DEEP_64				2

#define GPN_STAT_DATA_32BIT_BLEN				4
#define GPN_STAT_DATA_64BIT_BLEN				8

#define GPN_STAT_DATA_32BIT_DWLEN				1
#define GPN_STAT_DATA_64BIT_DWLEN				2
#define GPN_STAT_DATA_HIST_UNIT_DWLEN			(GPN_STAT_DATA_64BIT_DWLEN + 1)

#define GPN_STAT_MAX_TRAP_DELAY_SEC				120

#define GPN_STAT_DBS_ADD_NEW_LINE				1
#define GPN_STAT_DBS_ADD_NEW_UNIT				2

#define GPN_STAT_DBS_MINI_POLL_CYC_DIVI			10
typedef struct _stStatHistRecdUnit
{
	NODE node;
	/*index */
	UINT32 histDataLoc;
	/*UINT32 startTime;*/
	UINT32 stopTime;

	/*is valid data save in */
	UINT32 en;
	/*data */
	UINT32 subWild;
	UINT32 validSub;
	
	void *statDate;
}stStatHistRecdUnit;

typedef struct _stStatLCycDataRecd
{
	UINT8 en;
	UINT8 recStart;
	UINT8 recEnd;
	UINT8 rsvd;

	/*quick to taskNode */
	struct _stStatTaskNode_ *ptaskNode;
	
	UINT32 thredLocation;
	UINT32 subFiltLocation;
	UINT32 histDataLocation;
	
	UINT32 statLongCycUpAlm;	/*one stat scan type most hsa 32 sub stat scan type*/
	UINT32 statLongCycDownAlm;	/*one stat scan type most hsa 32 sub stat scan type*/
	UINT32 statLongCycUpEvent;	/*one stat scan type most hsa 32 sub stat scan type*/
	UINT32 statLongCycDownEvent;/*one stat scan type most hsa 32 sub stat scan type*/
	
	void *statLCycUpThred;
	void *statLCycDownThre;
	
	void *subFiltInfo;
	
	void *statLCycCurrDate;

	UINT32 histRecNum;
	stStatHistRecdUnit statLCycHistRecd[GPN_STAT_HIST_LCYC_REC_NUM];
}stStatLCycDataRecd;

typedef struct _stStatSCycDataRecd
{
	UINT8 en;
	UINT8 recStart;
	UINT8 recEnd;
	UINT8 rsvd;

	/*quick to taskNode */
	struct _stStatTaskNode_  *ptaskNode;

	UINT32 thredLocaion;
	UINT32 subFiltLocaion;

	UINT32 statShortCycUpAlm;		/*one stat scan type most hsa 32 sub stat scan type*/
	UINT32 statShortCycDownAlm;		/*one stat scan type most hsa 32 sub stat scan type*/
	UINT32 statShortCycUpEvent;		/*one stat scan type most hsa 32 sub stat scan type*/
	UINT32 statShortCycDownEvent;	/*one stat scan type most hsa 32 sub stat scan type*/
	
	void *statSCycUpThred;
	void *statSCycDownThre;
	
	void *subFiltInfo;
	
	void *statSCycCurrDate;

	UINT32 histRecNum;
	stStatHistRecdUnit statSCycHistRecd[GPN_STAT_HIST_SCYC_REC_NUM];
}stStatSCycDataRecd;

typedef struct _stStatUCycDataRecd_
{
	UINT8 en;
	UINT8 recStart;
	UINT8 recEnd;
	UINT8 rsvd;

	/*quick to taskNode */
	struct _stStatTaskNode_  *ptaskNode;

	UINT32 thredLocaion;
	UINT32 subFiltLocaion;

	UINT32 statUDCycUpAlm;		/*one stat scan type most hsa 32 sub stat scan type*/
	UINT32 statUDCycDownAlm;	/*one stat scan type most hsa 32 sub stat scan type*/
	UINT32 statUDCycUpEvent;	/*one stat scan type most hsa 32 sub stat scan type*/
	UINT32 statUDCycDownEvent;	/*one stat scan type most hsa 32 sub stat scan type*/

	void *statUCycUpThred;
	void *statSCycDownThre;
	
	void *subFiltInfo;
	
	void *statUCycCurrDate;

	UINT32 histRecNum;
	stStatHistRecdUnit statUCycHistRecd[GPN_STAT_HIST_UCYC_REC_NUM];
}stStatUCycDataRecd;
typedef struct _stStatXCycDataRecd_
{
	UINT8 en;
	UINT8 recStart;
	UINT8 recEnd;
	UINT8 trapEn;
	
	UINT8 dataValid;
	UINT8 rsv8;
	UINT16 rsv16;

	/*how hist data put in hist data table */
	UINT32 xhistDataLoc;
	/*how many hist data in record buff */
	UINT32 histRecNum;
	/*how many record buff mollac */
	UINT32 histRecBufoNum;
	/*max record buff could be mollaced */
	stStatHistRecdUnit *pstatXCycHistRecd[GPN_STAT_HIST_XCYC_REC_NUM];

	UINT32 subStatNum;
	/*subSateType in scanType value atrribute */
	UINT32 dataBitDeep;
	UINT32 valueType;
	UINT32 numType;
	
	void *xCycCurrDate;

	/*base port*/
	UINT32 xCycEvnThId;
	void *xCycEvnThred;
	/*base port*/
	UINT32 xCycsubFiltId;
	void *subFiltInfo;
	/*base task */
	UINT32 xCycAlmThId;
	void *xCycAlmThred;
	/*base task*/
	UINT32 xCycsubReptId;
	void *subReptInfo;

	/*trap time slot */
	UINT32 trapTSlot;
	
	UINT32 statXCycUpAlm;		/*one stat scan type most hsa 32 sub stat scan type*/
	UINT32 statXCycDownAlm;		/*one stat scan type most hsa 32 sub stat scan type*/
	UINT32 statXCycUpEvent;		/*one stat scan type most hsa 32 sub stat scan type*/
	UINT32 statXCycDownEvent;	/*one stat scan type most hsa 32 sub stat scan type*/

	/*quick to taskNode */
	struct _stStatTaskNode_  *ptaskNode;
}stStatXCycDataRecd;

typedef struct _stStatScanActionPortInfo_
{
	/*base mon en*/
	UINT8 scanQuenValid;
	/*fix port add to per mon*/
	UINT8 preQuenValid;
	/*add mon : fix & vairl*/
	UINT8 addQuenMode;
	/*reserved*/
	UINT8 rsd;

	/*==========================*/
	/*
		follow 's base scanType, portType maybe
		vs n scanType, in this statuation, here wo-
		uld be modify
	 */
	/*==========================*/
	UINT32 statScanType;
	stStatScanTypeDef *pscanTypeNode;

	/*space mollac when add port stat init */
	UINT32 dataSize;
	
	/*subSateType in scanType value atrribute */
	UINT32 dataBitDeep;
	UINT32 valueType;
	UINT32 numType;

	/*max scan cyc, use witch calculat ???*/
	UINT32 miniScanCyc;
	
	UINT32 isBaseInit;
	void *statBaseDate;
	
	UINT32 currMonEn;
	void *statCurrDate;

	/*add by lipf, 2018/3/27, used for record count of current data, to calc average*/
	UINT32 currDataCnt;

	/*add by geqian  2016.5.19     use for statPortMoniCtrlTable*/
	UINT32 insAddFlag;

	/*space new mollac when XCyc stat enable*/
	stStatXCycDataRecd *statLCycRec;
	stStatXCycDataRecd *statSCycRec;
	stStatXCycDataRecd *statUCycRec;
}stStatScanPortInfo;

typedef struct _stStatPeerNodeDef_
{
	NODE node;
	/* stat comm with processes */
	optObjOrient peerPort;

	/* port data struct */
	stStatScanPortInfo *pStatScanPort;
}stStatPeerPortNode;

typedef struct _stStatLocalPortNodeDef_
{
	NODE node;
	/* stat comm with DA( easy view ) */
	objLogicDesc localPort;
	/* stat record, trap and show port */
	objLogicDesc viewPort;
	/* local 1 :1 peer port */
	stStatPeerPortNode *pPeerNode;
	
	/* port data struct */
	stStatScanPortInfo *pStatScanPort;
}stStatLocalPortNode;

/*+++++++++ PORT STAT DATA RECD METHOD DEFINE END+++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++                                                                 +++++++++++++++++++*/

typedef struct _stStatPortTpVsScanTpTemplate
{
	UINT32 portType;
	GPN_STAT_TYPE statScanType;
	const char *portTpName;
}stStatPTpVsSTpTemp;

typedef struct _stStatPreScanIndex
{
	NODE node;
	stStatLocalPortNode *pStatLocalPort;
}stStatPreScanIndex;

typedef struct _stStatPreScanQuen
{
	NODE node;	
	/* now , local port 1 : 1 peer port */
	/* local port list */
	LIST statPreScanQuen[GPN_STAT_PRESCAN_HASH];
	UINT32 preScanNodeNum[GPN_STAT_PRESCAN_HASH];
	/* peer port list */
	LIST statPeerPortQuen[GPN_STAT_PRESCAN_HASH];
	UINT32 peerPortNodeNum[GPN_STAT_PRESCAN_HASH];
	UINT32 hashKey;
	GPN_STAT_TYPE statScanType;
	stStatScanTypeDef *statScanTypeNode;
	
	stVLQDataSt *pVariPTList;
}stStatPreScanQuen;

typedef struct _stRemDevStatProcNode_
{
	NODE node;
	UINT32 devIndex;
	UINT32 godDevIndex;
	objLogicDesc godPortIndex;
	void *pRemDevSelfSpace;
	void *pRemPreScanPortNodeMemSpace;
	UINT32 remotDevSta;
	
}stRemDevStatProcNode;

typedef struct _stUnitStatProcSpace
{
	LIST statPreScanTpQuen;
	UINT32 quenNodeNum;
	
	UINT32 slotDevSta;
	/*usr for memrey-free : slot status will chang, so space ...*/
	stStatPreScanIndex *pCenterPreScanPortNodeMemSpace;
	
	/*remot agent about*/
	LIST RemDevStatPorcNodeQuen;
	UINT32 RemDevStatPorcNodeNum;
}stUnitStatProcSpace;

typedef struct _stStatGlobalInfo_
{
	/*time info*/
	UINT32 polarity;
	UINT32 offSet;

	/*auto report config */
	UINT32 lastHistRept;
	UINT32 oldHistRept;
	UINT32 alarmRept;
	UINT32 eventRept;
	
	/*stat data is absoluteValue or deltaValue */
	UINT32 conutType;
	
}stStatGlobalInfo;

typedef struct _stStatTaskNode_
{
	NODE node;
	/*can not modify after init */
	UINT32 taskId;

	UINT32 statScanTp;
	
	UINT32 cycClass;
	UINT32 cycSeconds;

	UINT32 monStartTime;
	UINT32 monEndTime;

	/*alarm template threshold Id */
	UINT32 almThredTpId;
	UINT32 subReportTpId;
	/*
	UINT32 evnThredTpId;
	UINT32 subFiltTpId;
	*/
	
	/*push stack time about */
	UINT32 cycStart;
	UINT32 cycBingo;
	UINT32 thisPush;

	/*include port Num */	
	LIST actMOnOBjQuen;
	UINT32 actMOnOBjNum;
	
	UINT32 maxPortInTask;
}stStatTaskNode;

typedef struct _stStatMonObjNode_
{
	NODE node;
	UINT32 taskId;
	
	/*scan port node (local port base)*/
	stStatLocalPortNode *pStatLocalPort;
}stStatMonObjNode;

typedef struct _stStatTaskInfo_
{
	UINT32 hash;

	UINT32 maxTaskNum;
	UINT32 runTaskNum;

	/*use hash(base taskId) to be fast ??? */
	LIST actTaskQuen[GPN_STAT_SEARCH_HASH];
	UINT32 actTaskNum[GPN_STAT_SEARCH_HASH];
	
	LIST idleTaskQuen;
	UINT32 idleTaskNum;

	/*task include obj(port) */
	UINT32 maxMonObjNum;
	UINT32 runMonObjNum;

	LIST idleMOnOBjQuen;
	UINT32 idleMOnOBjNum;

	/*mini cyc seconds */
	UINT32 miniCycSecs;

	/*stat use sys time, renew per-1s-time bingo */
	UINT32 statSysTime;
}stStatTaskInfo;

typedef struct _evnThredTpUnit_
{
	UINT32 subType;
	UINT32 upThredH32;
	UINT32 upThredL32;
	UINT32 dnThredH32;
	UINT32 dnThredL32;
}evnThredTpUnit;

typedef struct _evnThredTp_
{
	UINT32 subNum;
	evnThredTpUnit *pevnThred;
}evnThredTp;

typedef struct _stStatEvnThredTpNode_
{
	NODE node;
	/*can not modify after init */
	UINT32 thredTpId;
	UINT32 scanType;
	
 	/*include sub stat event threshold type Num */	
	evnThredTp *pevnThredTp;
}stStatEvnThredTpNode;

typedef struct _stStatEvnThredTpTable_
{
	/*modify disable after init */
	UINT32 thredTpId;
	UINT32 subType;
	UINT32 scanType;

	/*modify enable */
	UINT32 upThredH32;
	UINT32 upThredL32;
	UINT32 dnThredH32;
	UINT32 dnThredL32;
}stStatEvnThredTpTable;

typedef struct _almThredTpUnit_
{
	UINT32 subType;
	UINT32 upRiseThredH32;
	UINT32 upRiseThredL32;
	UINT32 upDispThredH32;
	UINT32 upDispThredL32;
	UINT32 dnRiseThredH32;
	UINT32 dnRiseThredL32;
	UINT32 dnDispThredH32;
	UINT32 dnDispThredL32;
}almThredTpUnit;

typedef struct _almThredTp_
{
	UINT32 subNum;
	almThredTpUnit *palmThred;
}almThredTp;

typedef struct _stStatAlmThredTpNode_
{
	NODE node;
	/*can not modify after init */
	UINT32 thredTpId;
	UINT32 scanType;
	
 	/*include sub stat alarm threshold type Num */	
	almThredTp *palmThredTp;
}stStatAlmThredTpNode;

typedef struct _stStatAlmThredTpTable_
{
	/*modify disable after init */
	UINT32 thredTpId;
	UINT32 subType;
	UINT32 scanType;

	/*modify enable */
	UINT32 upRiseThredH32;
	UINT32 upRiseThredL32;
	UINT32 upDispThredH32;
	UINT32 upDispThredL32;
	UINT32 dnRiseThredH32;
	UINT32 dnRiseThredL32;
	UINT32 dnDispThredH32;
	UINT32 dnDispThredL32;
}stStatAlmThredTpTable;

typedef struct _stStatThredTemp_
{
	UINT32 thredTpIdDsp;

	UINT32 maxThredTpNum;
	UINT32 runThredTpNum;

	UINT32 hash;
	/*use hash to be fast ??? */
	LIST actThredTpQuen[GPN_STAT_SEARCH_HASH];
	UINT32 actThredTpNum[GPN_STAT_SEARCH_HASH];

	LIST idleThredTpQuen;
	UINT32 idleThredTpNum;
}stStatThredTemp;

typedef struct _subFiltTpUnit_
{
	UINT32 subType;
	UINT32 status;
}subFiltTpUnit;

typedef struct _subFiltTp_
{
	UINT32 subNum;
	subFiltTpUnit *psubFilt;
}subFiltTp;

typedef struct _stStatSubFiltTpNode_
{
	NODE node;
	/*can not modify after init */
	UINT32 subFiltTpId;
	
	UINT32 scanType;
 	/*include sub stat Num */
	
	/*include sub stat type Num */	
	subFiltTp *psubFiltTp;

}stStatSubFiltTpNode;

typedef struct _stStatSubFiltTpTable_
{
	/*modify disable after init */
	UINT32 subFiltTpId;
	UINT32 subType;
	UINT32 scanType;

	/*modify enable*/
	UINT32 status;
}stStatSubFiltTpTable;

typedef struct _stStatSubFiltTemp_
{
	UINT32 subFiltIdDsp;
	
	UINT32 maxSubFiltTpNum;
	UINT32 runSubFiltTpNum;

	UINT32 hash;
	/*use hash to be fast ??? */
	LIST actSubFiltTpQuen[GPN_STAT_SEARCH_HASH];
	UINT32 actSubFiltTpNum[GPN_STAT_SEARCH_HASH];

	LIST idleSubFiltTpQuen;
	UINT32 idleSubFiltTpNum;
}stStatSubFiltTemp;

typedef struct _subReportTpUnit_
{
	UINT32 subType;
	UINT32 status;
}subReportTpUnit;

typedef struct _subReportTp_
{
	/*subNum can not be '0' */
	UINT32 subNum;
	subReportTpUnit *psubReport;
}subReportTp;

typedef struct _stStatSubReportTpNode_
{
	NODE node;
	/*can not modify after init */
	UINT32 subReportTpId;
	
	UINT32 scanType;
 	/*include sub stat Num */
	
	/*include sub stat type Num */	
	subReportTp *psubReportTp;

}stStatSubReportTpNode;

typedef struct _stStatSubReportTpTable_
{
	/*modify disable after init */
	UINT32 subReportTpId;
	UINT32 subType;
	UINT32 scanType;

	/*modify enable*/
	UINT32 status;
}stStatSubReportTpTable;

typedef struct _stStatSubReportTemp_
{
	UINT32 subReportIdDsp;
	
	UINT32 maxSubReportTpNum;
	UINT32 runSubReportTpNum;

	UINT32 hash;
	/*use hash to be fast ??? */
	LIST actSubReportTpQuen[GPN_STAT_SEARCH_HASH];
	UINT32 actSubReportTpNum[GPN_STAT_SEARCH_HASH];

	LIST idleSubReportTpQuen;
	UINT32 idleSubReportTpNum;
}stStatSubReportTemp;

typedef struct _stStatDataElement_
{
	UINT32 subType;
	UINT32 dataHigh32;
	UINT32 dataLow32;
}stStatDataElement;

typedef struct _stStatHistTrapDataNode_
{
	NODE node;
	/*record hist data key lable, used match this node in  datastruce */
	UINT32 dataIndex;
	/*used for when trap do */
	UINT32 trapTSlot;

	/*trap's data part */
	UINT32 trapIndex;
	UINT32 scanType;
	UINT32 cycClass;
	UINT32 stopTime;
	/*system location port~~~~viewPort*/
	objLogicDesc viewPort;	
 	/*sub type data bufo */
	UINT32 validSubNum;
	stStatDataElement data[GPN_STAT_MAX_SUB_INCLUDE];
}stStatHistTrapDataNode;

typedef struct _stStatTrapDataTable_
{
	/*trap's data part */
	UINT32 trapIndex;
	UINT32 subType;
	UINT32 stopTime;
	UINT32 scanType;
	UINT32 cycClass;
	
	/*system location port ~~~view port*/
	objLogicDesc viewPort;	
 	/*sub type data bufo */
	UINT32 dataHigh32;
	UINT32 dataLow32;
}stStatTrapDataTable;

typedef UINT32 (*STATTRAPFUN)(stStatTrapDataTable *ptrapLine);

typedef struct _stStatHistDataMgt_
{
	UINT32 hash;

	/*this used for histDataTalbe (index is histDataLoc and histStopTime) */
	LIST histDataQuen[GPN_STAT_SEARCH_HASH];
	UINT32 histDataNum[GPN_STAT_SEARCH_HASH];

	/*this used for standTrapDataTable (index is histDataIndex) */
	LIST actTrapDataQuen[GPN_STAT_SEARCH_HASH];
	UINT32 actTrapDataNum[GPN_STAT_SEARCH_HASH];

	LIST idleTrapDataQuen;
	UINT32 idleTrapDataNum;

	UINT32 trapNodeNum;
}stStatHistDataMgt;

typedef struct _stStatPortMonMgt_
{
	/*table index*/
	objLogicDesc portIndex;
	UINT32 scanType;
	UINT32 insAddFlag;       //modify by geqian   2016.5.19        use for statPortMoniCtrlTable  operation
	
	UINT32 statMoniEn;
	UINT32 currStatMoniEn;
	UINT32 longCycStatMoniEn;
	UINT32 longCycBelongTask;
	UINT32 longCycEvnThredTpId;
	UINT32 longCycSubFiltTpId;
	UINT32 longCycAlmThredTpId;
	UINT32 longCycHistReptTpId;
	UINT32 longCycHistDBId;
	UINT32 shortCycStatMoniEn;
	UINT32 shortCycBelongTask;
	UINT32 shortCycEvnThredTpId;
	UINT32 shortCycSubFiltTpId;
	UINT32 shortCycAlmThredTpId;
	UINT32 shortCycHistReptTpId;
	UINT32 shortCycHistDBId;
	UINT32 udCycStatMoniEn;
	UINT32 udCycBelongTask;
	UINT32 udCycEvnThredTpId;
	UINT32 udCycSubFiltTpId;
	UINT32 udCycAlmThredTpId;
	UINT32 udCycHistReptTpId;
	UINT32 udCycHistDBId;
	UINT32 udCycSecs;
}stStatPortMonMgt;

typedef struct _stStatCurrDataTable_
{
	/*modify disable after init */
	objLogicDesc viewPort;
	UINT32 subType;
	UINT32 scanType;/*should be here ?*/

	/*modify enable*/
	UINT32 currDataClr;
	UINT32 currDataH32;
	UINT32 currDataL32;
}stStatCurrDataTable;

typedef struct _stStatHistDataTable_
{
	/*3 table index, modify disable after init */
	UINT32 index;
	UINT32 stopTime;
	UINT32 subType;
	
	UINT32 scanType;/*should be here ?*/
	/*modify enable*/
	UINT32 histDataH32;
	UINT32 histDataL32;
}stStatHistDataTable;

typedef struct _stPortTpToStatScanTp
{
	NODE node;
	
	UINT32 statDataSizeForPort;
	/* first : we use port type
             second : we use slot 
             thred : we use hash*/
	stStatPreScanQuen *pStatPreScanNode[EQU_SLOT_MAX_NUM+1];

	stStatScanTypeDef *statScanTypeNode;
	GPN_STAT_TYPE statScanType;
	UINT32 portType;
	const char * portTpName;
}stPortTpToStatScanTp;

typedef struct _stPortTpVsStatScanTpRelation
{
	LIST PTpVsSTpQuen;
	UINT32 sysSupPortTpNum;
	/*we did not know how many sys support port type,
	   so we use max port type num ???*/
	stPortTpToStatScanTp *pPortTpToScanTpMemspace;
}stPTpVsStatSTpRelation;

typedef struct _stEQUStatProcSpace
{
	/*global info */
	stStatGlobalInfo statGlobalInfo;
	
	/*task info */
	stStatTaskInfo statTaskInfo;
	/*event threshold templat info */
	stStatThredTemp statEventThredTemp;
	/*alarm threshold templat info */
	stStatThredTemp statAlarmThredTemp;
	/*sub Stat Type filt templat */
	stStatSubFiltTemp statSubFiltTemp;
	/*sub Stat Type report templat */
	stStatSubReportTemp statSubReportTemp;
	/*hist data mgt */
	stStatHistDataMgt statHistDataMgt;
	STATTRAPFUN trapMethodFunc;

	/*port type Vs stat type */
	stPTpVsStatSTpRelation PTpVsStatSTpRelation;
	/*per slot for Stat data to port */
	stUnitStatProcSpace *unitStatProcSpace[EQU_SLOT_MAX_NUM+1];

	
}stEQUStatProcSpace;

typedef stFixPorcReg stStatPortObjCL;

UINT32 gpnStatMakePortIndex(UINT32 devIndex, UINT32 devPortTpNum,
	stStatPortObjCL *pPortObjCL, objLogicDesc *portIndexBuff, objLogicDesc *upPtBuff);
UINT32 gpnStatTimePolarityGet(void);
UINT32 gpnStatTimeOffsetGet(void);
UINT32 gpnStatMaxMonObjGet(void);
UINT32 gpnStatRunMonObjGet(void);
UINT32 gpnStatLastHistReportCfgGet(void);
UINT32 gpnStatLastHistReportCfgSet(UINT32 opt);
UINT32 gpnStatOldHistReportCfgGet(void);
UINT32 gpnStatOldHistReportCfgSet(UINT32 opt);
UINT32 gpnStatAlarmReportCfgGet(void);
UINT32 gpnStatAlarmReportCfgSet(UINT32 opt);
UINT32 gpnStatEventReportCfgGet(void);
UINT32 gpnStatEvnetReportCfgSet(UINT32 opt);
UINT32 gpnStatTaskIdGet(void);
UINT32 gpnStatTaskIdCheck(UINT32 taskId);
UINT32 gpnStatMaxTaskNumGet(void);
UINT32 gpnStatRunTaskNumGet(void);
UINT32 gpnStatEventThredTemplatIdGet(void);
UINT32 gpnStatEventThredTemplatIdCheck(UINT32 evnTherdId);
UINT32 gpnStatMaxEventThredTpNumGet(void);
UINT32 gpnStatRunEventThredTpNumGet(void);
UINT32 gpnStatAlarmThredTemplatIdGet(void);
UINT32 gpnStatAlarmThredTemplatIdCheck(UINT32 almThredId);
UINT32 gpnStatMaxAlarmThredTpNumGet(void);
UINT32 gpnStatRunAlarmThredTpNumGet(void);
UINT32 gpnStatSubTypeFiltIdGet(void);
UINT32 gpnStatSubTypeFiltIdCheck(UINT32 subFiltId);
UINT32 gpnStatMaxSubFiltTpNumGet(void);
UINT32 gpnStatRunSubFiltTpNumGet(void);
UINT32 gpnStatSubTypeReportIdGet(void);
UINT32 gpnStatSubTypeReportIdCheck(UINT32 subReportId);
UINT32 gpnStatMaxSubReportTpNumGet(void);
UINT32 gpnStatRunSubReportTpNumGet(void);
UINT32 gpnStatCountTypeGet(void);
UINT32 gpnStatDeviceRegist(UINT32 devIndex, UINT32 sta);
UINT32 gpnStatDeviceDistribution(UINT32 devIndex);
UINT32 gpnStatDeviceDestroy(UINT32 devIndex);
UINT32 gpnStatPeerPortReplaceNotify(optObjOrient *pOldPeerP, optObjOrient *pNewPeerP);
UINT32 gpnStatViewPortReplaceNotify(objLogicDesc *pLocalPort, objLogicDesc *pNewViewP);
UINT32 gpnStatVariPortRegist(objLogicDesc *pLocalPort, UINT32 opt);
UINT32 gpnStatVariPortDistribution(objLogicDesc *pLocalPort);
UINT32 gpnStatVariPortDestroy(objLogicDesc *pLocalPort);
UINT32 gpnStatFixPortRegist(UINT32 devIndex,
	UINT32 devPortTpNum, stStatPortObjCL *pPortObjCL);
UINT32 gpnStatScanPortNodeInit(objLogicDesc *pLocalPort, stStatLocalPortNode *pStatLocalNode, 
	stStatPeerPortNode *pStatPeerNode, stStatScanPortInfo *pStatScanPort);
UINT32 gpnStatLocalPortNodeInit(objLogicDesc *pLocalPort, stStatLocalPortNode *pStatLocalNode, 
	stStatPeerPortNode *pStatPeerNode, stStatScanPortInfo *pStatScanPort);
UINT32 gpnStatPeerPortNodeInit(optObjOrient *pPeerPort,
	stStatPeerPortNode *pStatPeerNode, stStatScanPortInfo *pStatScanPort);
UINT32 gpnStatScanPortInfoInit(stStatScanPortInfo *pStatScanPort, optObjOrient *pPeerPort);
UINT32 gpnStatLocalPortIndexInit(objLogicDesc *pLocalPort);
UINT32 gpnStatViewPortIndexInit(objLogicDesc *pViewPort);
UINT32 gpnStatScanPortNodeDestroy(stStatPreScanIndex *pStatPreScanIndex);
UINT32 gpnStatNodeRelevChg2PreScanQuen(stStatPreScanIndex *pStatPreScanIndex, UINT32 opt);
UINT32 gpnStatNodeRelevChg2ValidScanQuen(stStatLocalPortNode *pStatLocalNode, UINT32 opt);
UINT32 gpnStatMonOpt(objLogicDesc *pPortInfo, UINT32 scanType,
	UINT32 cycClass, UINT32 cycSec, UINT32 taskId, UINT32 almThredTpId,
	UINT32 subReptTpId, UINT32 evnThredTpId, UINT32 subFiltTpId, INT32 histReport, UINT32 opt, UINT32 currOpt);
UINT32 gpnStatCycClassMemoryMalloc(stStatLocalPortNode *pStatLocalNode, UINT32 cycClass);
UINT32 gpnStatCycClassMemoryInit(stStatLocalPortNode *pStatLocalNode, UINT32 cycClass);
UINT32 gpnStatCycClassMemoryFree(stStatLocalPortNode *pStatLocalNode, UINT32 cycClass);
UINT32 gpnStatCycClassMemoryReset(stStatLocalPortNode *pStatLocalNode, UINT32 cycClass);
UINT32 gpnStatCheckPortMonInTask(stStatLocalPortNode *pStatLocalNode, UINT32 taskId);
UINT32 gpnStatPortMon2Task(stStatLocalPortNode *pStatLocalNode, UINT32 taskId);
UINT32 gpnStatPortMonOutTask(stStatLocalPortNode *pStatLocalNode, UINT32 taskId);
UINT32 gpnStatRenewPortPollCyc(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc);
UINT32 gpnStatPortMonEnNotifyFuncModu(stStatLocalPortNode *pStatLocalNode, UINT32 en);
UINT32 gpnStatCycClassTemplatSet(stStatXCycDataRecd *statXCycRec,
	UINT32 almThredTpId, UINT32 subReptTpId, UINT32 evnThredTpId, UINT32 subFiltTpId);
UINT32 gpnStatCycClassEnable(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc,
	UINT32 cycClass, UINT32 almThredTpId, UINT32 subReptTpId, UINT32 evnThredTpId, UINT32 subFiltTpId, INT32 histReport);
UINT32 gpnStatCycClassDisable(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc, UINT32 cycClass);
UINT32 gpnStatCycClassDelete(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc, UINT32 cycClass);
UINT32 gpnStatRelevCheckIsLocalInPerScanQuen(objLogicDesc *pLocalPort, stStatPreScanQuen *pStatPreScanNode);
UINT32 gpnStatRelevCheckIsPeerInPeerPortQuen(optObjOrient *pPeerPort, stStatPreScanQuen *pStatPreScanNode);
UINT32 gpnStatSeekLocalPort2PreScanNodeIndex(objLogicDesc *pLocalPort, stStatPreScanIndex **ppStatPreScanIndex);
UINT32 gpnStatSeekLocalPort2LocalPortNode(objLogicDesc *pLocalPort, stStatLocalPortNode **ppStatLocalNode);
UINT32 gpnStatSeekPeerPort2PeerPortNode(optObjOrient *pPeerPort, stStatPeerPortNode **ppStatPeerNode);
UINT32 gpnStatSeekFirstPreScanNodeIndex(stStatPreScanIndex **ppStatPreScanIndex);
UINT32 gpnStatSeekNextPreScanNodeIndex(stStatPreScanIndex *pStatPreScanIndex, stStatPreScanIndex **ppStatPreScanIndex);
UINT32 gpnStatSeekFirstValidScanNodeIndex(stStatLocalPortNode **ppStatLocalNode);
UINT32 gpnStatSeekNextValidScanNodeIndex(stStatLocalPortNode *pStatLocalNode, stStatLocalPortNode **ppStatLocalNode);
UINT32 gpnStatSeekFirstSubTypeInPortScanNode(stStatLocalPortNode *pStatLocalNode, UINT32 *psubType);
UINT32 gpnStatSeekNextSubTypeInPortScanNode(stStatLocalPortNode *pStatLocalNode, UINT32 subType, UINT32  *psubType);
UINT32 gpnStatSeekPortType2ScanTypeNode(UINT32 portType, stStatScanTypeDef **ppStatScanTypeNode);
UINT32 gpnStatSeekFirstPortType2PTVsST(stPortTpToStatScanTp **ppPortTpToScanTp);
UINT32 gpnStatSeekPortType2PTVsST(UINT32 portType, stPortTpToStatScanTp **ppPortTpToScanTp);
UINT32 gpnStatDataStructMonAdd(objLogicDesc *pLocalPort);
UINT32 gpnStatDataStructMonDelete(objLogicDesc *pLocalPort);
UINT32 gpnStatTaskAdd(stStatTaskNode *ptaskNode);
UINT32 gpnStatTaskDelete(UINT32 taskId);
UINT32 gpnStatTaskModify(stStatTaskNode *pnewTaskNode);
UINT32 gpnStatTaskGet(UINT32 taskId, stStatTaskNode **pptaskNode);
UINT32 gpnStatTaskGetNext(UINT32 taskId, stStatTaskNode **pptaskNode, UINT32 *pnextTask);
UINT32 gpnStatTaskGetNext_2(UINT32 taskId, stStatTaskNode **pptaskNode);

UINT32 gpnStatEventThredTpAdd(stStatEvnThredTpTable *pthredTpLine);
UINT32 gpnStatEventThredTpDelete(UINT32 thredTpId);
UINT32 gpnStatEventThredTpModify(stStatEvnThredTpTable *pthredTpLine);
UINT32 gpnStatEventThredTpGet(UINT32 thredTpId, stStatEvnThredTpNode **ppthredTpNode);
UINT32 gpnStatEventThredTpSubTypeQuery(stStatEvnThredTpNode *pevnThredTpNode/*in*/,
	UINT32 subType, stStatEvnThredTpTable *pevnThredTpLine/*out*/);
UINT32 gpnStatEventThredTpGetNext(UINT32 thredTpId, stStatEvnThredTpNode **ppthredTpNode, UINT32 *pnextThredTp);
UINT32 gpnStatEventThredTpSubTypeQueryNext(stStatEvnThredTpNode *pevnThredTpNode/*in*/,
	UINT32 thredTpId, UINT32 subType, stStatEvnThredTpTable *pevnThredTpLine/*out*/,
	INT32 sNextEvnThredTpId, UINT32 *pnextEvnThredTpId, UINT32 *pnextSubType);
UINT32 gpnStatEventThredTpSubTypeQueryNextAdjust(UINT32 *pnextEvnThredTpId, UINT32 *pnextSubType);
UINT32 gpnStatAlarmThredTpAdd(stStatAlmThredTpTable *pthredTpLine);
UINT32 gpnStatAlarmThredTpDelete(UINT32 thredTpId);
UINT32 gpnStatAlarmThredTpModify(stStatAlmThredTpTable *pthredTpLine);
UINT32 gpnStatAlarmThredTpGet(UINT32 thredTpId, stStatAlmThredTpNode **ppthredTpNode);
UINT32 gpnStatAlarmThredTpSubTypeQuery(stStatAlmThredTpNode *palmThredTpNode/*in*/,
	UINT32 subType, stStatAlmThredTpTable *palmThredTpLine/*out*/);
UINT32 gpnStatAlarmThredTpGetNext(UINT32 thredTpId, stStatAlmThredTpNode **ppthredTpNode, UINT32 *pnextThredTp);
UINT32 gpnStatAlarmThredTpSubTypeQueryNext(stStatAlmThredTpNode *palmThredTpNode/*in*/,
	UINT32 thredTpId, UINT32 subType, stStatAlmThredTpTable *palmThredTpLine/*out*/,
	UINT32 sNextAlmThredTpId, UINT32 *pnextAlmThredTpId, UINT32 *pnextSubType);
UINT32 gpnStatAlarmThredTpSubTypeQueryNextAdjust(UINT32 *pnextAlmThredTpId, UINT32 *pnextSubType);
UINT32 gpnStatSubFiltTpAdd(stStatSubFiltTpTable *psubFiltTpLine);
UINT32 gpnStatSubFiltTpDelete(UINT32 subFiltTpId);
UINT32 gpnStatSubFiltTpModify(stStatSubFiltTpTable *psubFiltTpLine);
UINT32 gpnStatSubFiltTpGet(UINT32 subFiltTpId, stStatSubFiltTpNode **ppsubFiltTpNode);
UINT32 gpnStatSubFiltTpSubTypeQuery(stStatSubFiltTpNode *psubFiltTpNode/*in*/,
	UINT32 subType, stStatSubFiltTpTable *psubFiltTpLine/*out*/);
UINT32 gpnStatSubFiltTpGetNext(UINT32 subFiltTpId, stStatSubFiltTpNode **ppsubFiltTpNode, UINT32 *pnextSubFiltTp);
UINT32 gpnStatSubFiltTpSubTypeQueryNext(stStatSubFiltTpNode *psubFiltTpNode/*in*/,
	UINT32 subFiltTpId, UINT32 subType, stStatSubFiltTpTable *psubFiltTpLine/*out*/,
	UINT32 sNextSubFiltTpId, UINT32 *pnextSubFiltTpId, UINT32 *pnextSubType);
UINT32 gpnStatSubFiltTpSubTypeQueryNextAdjust(UINT32 *pnextSubFiltTpId, UINT32 *pnextSubType);
UINT32 gpnStatSubReportTpAdd(stStatSubReportTpTable *psubReportTpLine);
UINT32 gpnStatSubReportTpDelete(UINT32 subReportTpId);
UINT32 gpnStatSubReportTpModify(stStatSubReportTpTable *psubReportTpLine);
UINT32 gpnStatSubReportTpGet(UINT32 subReportTpId, stStatSubReportTpNode **ppsubReportTpNode);
UINT32 gpnStatSubReportTpSubTypeQuery(stStatSubReportTpNode *psubReportTpNode/*in*/,
	UINT32 subType, stStatSubReportTpTable *psubReportTpLine/*out*/);
UINT32 gpnStatSubReportTpGetNext(UINT32 subReportTpId, stStatSubReportTpNode **ppsubReportTpNode, UINT32 *pnextSubReportTp);
UINT32 gpnStatSubReportTpSubTypeQueryNext(stStatSubReportTpNode *psubReportTpNode/*in*/,
	UINT32 subReportTpId, UINT32 subType, stStatSubReportTpTable *psubReportTpLine/*out*/,
	UINT32 sNextSubReportTpId, UINT32 *pnextSubReportTpId, UINT32 *pnextSubType);
UINT32 gpnStatSubReportTpSubTypeQueryNextAdjust(UINT32 *pnextSubReportTpId, UINT32 *pnextSubType);
UINT32 gpnStatMonObjSubFiltTpCfg(objLogicDesc *pmonObjInfo, UINT32 cycClass, void *subFiltTp);
UINT32 gpnStatPortMonGet(stStatLocalPortNode *pStatLocalNode, UINT32 scanType, stStatPortMonMgt *pportMonMgt);
UINT32 gpnStatCurrDataClear(objLogicDesc *pmonObjInfo);
UINT32 gpnStatCurrDataSubTypeQurey(stStatLocalPortNode *pStatLocalNode, UINT32 subType, stStatCurrDataTable *pcurrDataLine);
UINT32 gpnStatCurrDataSubTypeQureyNext(stStatLocalPortNode *pStatLocalNode, UINT32 subType,
 	stStatCurrDataTable *pcurrDataLine, objLogicDesc *psNextIndex,
 	objLogicDesc *pnextPIndex, UINT32 *psNextSubType);
UINT32 gpnStatCurrDataSubTypeQureyNextAdjust(objLogicDesc *pnextPIndex, UINT32 *pnextSubType);
UINT32 gpnStatHistDataTpIdGet(void);
UINT32 gpnStatHistDataAddInMgt(stStatHistRecdUnit *phistRecdUnit);
UINT32 gpnStatHistDataDelOutMgt(UINT32 histLocal);
UINT32 gpnStatHistDataGet(UINT32 histDataLoc, UINT32 stopTime, stStatHistRecdUnit **pphistRecdUnit);
UINT32 gpnStatHistDataSubTypeQurey(stStatHistRecdUnit *phistRecdUnit,
	UINT32 subType, stStatHistDataTable *phistDataLine);
UINT32 gpnStatHistDataGetNext(UINT32 histDataLoc, UINT32 stopTime,
	stStatHistRecdUnit **pphistRecdUnit, UINT32 *pnextHistDataLoc, UINT32 *pnextStopTime);
UINT32 gpnStatHistDataSubTypeQureyNext(stStatHistRecdUnit *phistRecdUnit/*in*/,
	UINT32 histId, UINT32 stopTime, UINT32 subType, stStatHistDataTable *phistDataLine/*out*/,
	UINT32 sNextHistId, UINT32 sNextStopTime, UINT32 *pnextHistId, UINT32 *pnextStopTime,
	UINT32 *pnextSubType);
UINT32 gpnStatHistDataSubTypeQureyNextAdjust(UINT32 *pnextHistId, UINT32 *pnextStopTim, UINT32 *pnextSubType);
UINT32 gpnStatTrapDataAddInMgt(stStatXCycDataRecd *pxCycDataRecd, stStatTaskNode *ptaskNode, objLogicDesc *pMonObjIndex);
UINT32 gpnStatCreatTrapTimeSlot(stStatXCycDataRecd *pxCycDataRecd);
UINT32 gpnStatTrapNodeSetInfo(stStatHistTrapDataNode *ptrapNode,
	stStatXCycDataRecd *pxCycDataRecd, stStatTaskNode *ptaskNode,
	objLogicDesc *pMonObjIndex);
UINT32 gpnStatTrapDataClear(void);
UINT32 gpnStatTrapDataOpt(stStatHistTrapDataNode *ptrapNode);
UINT32 gpnStatTrapDataMethodRegist(STATTRAPFUN trapMethodFunc);
UINT32 gpnStatTrapDataGet(UINT32 histDataIndex, stStatHistTrapDataNode **pptrapData);
UINT32 gpnStatTrapDataGetNext(UINT32 histDataIndex, stStatHistTrapDataNode **pptrapData, UINT32 *pnextIndex);
UINT32 gpnStatEQUStatProcSpaceMollac(stEQUStatProcSpace **ppEQUStatProcSpace);
UINT32 gpnStatGlobalInfoInit(stEQUStatProcSpace *pEQUStatProcSpace);
UINT32 gpnStatTaskInfoInit(stEQUStatProcSpace *pEQUStatProcSpace);
UINT32 gpnStatEventThredTempInit(stEQUStatProcSpace *pEQUStatProcSpace);
UINT32 gpnStatAlarmThredTempInit(stEQUStatProcSpace *pEQUStatProcSpace);
UINT32 gpnStatSubFiltTempInit(stEQUStatProcSpace *pEQUStatProcSpace);
UINT32 gpnStatSubReportTempInit(stEQUStatProcSpace *pEQUStatProcSpace);
UINT32 gpnStatHistDataMgtInit(stEQUStatProcSpace *pEQUStatProcSpace);
UINT32 gpnStatPerScanQuenInit(stEQUStatProcSpace *pEQUStatProcSpace);
UINT32 gpnStatPortTypeVsStatScanTypeInit(stEQUStatProcSpace *pEQUStatProcSpace);
UINT32 gpnStatDataBaseStructInit(void);
UINT32 debugGpnStatDataBaseStruct(void);

UINT32 gpnStatCycClassAlmTpSet(stStatXCycDataRecd *statXCycRec, 
				UINT32 almThredTpId, UINT32 evnThredTpId);
UINT32 gpnStatCycClassChgAlmTp(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc,
	     UINT32 cycClass, UINT32 almThredTpId, UINT32 evnThredTpId);
UINT32 gpnStatCycClassDataTpSet(stStatXCycDataRecd *statXCycRec,
					UINT32 subReptTpId, UINT32 subFiltTpId);
UINT32 gpnStatCycClassChgDataTp(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc,
	UINT32 cycClass, UINT32 subReptTpId, UINT32 subFiltTpId);
UINT32 gpnStatCycClassHistAutoReport(stStatLocalPortNode *pStatLocalNode, 
	UINT32 scanTpPollCyc, UINT32 cycClass, INT32 histReport);

UINT32 gpnStatCycClassEnableRestore(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc, UINT32 cycClass);
UINT32 gpnStatEventTpDelCheckPortDel(stStatEvnThredTpNode *pthredTpNode);
UINT32 gpnStatAlarmTpDelCheckPortDel(stStatAlmThredTpNode *pthredTpNode);
UINT32 gpnStatFiltTpDelCheckPortDel(stStatSubFiltTpNode *pFiltTpNode);
UINT32 gpnStatReportTpDelCheckPortDel(stStatSubReportTpNode *psubReportTpNode);


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_STAT_DATA_STRUCT_H_*/

