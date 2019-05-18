/**********************************************************
* file name: gpnStatType.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-03-12
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_TYPE_H_
#define _GPN_STAT_TYPE_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>

#include <lib/syslog.h>
#include <lib/log.h>

#include "type_def.h"
#include "stat_debug.h"
#include "socketComm/gpnSockMsgDef.h"
#include "socketComm/gpnSockStatMsgDef.h"
#include "socketComm/gpnStatTypeDef.h"


#define GPN_STAT_TYPE_OK 					GEN_SYS_OK
#define GPN_STAT_TYPE_ERR 					GEN_SYS_ERR

#define GPN_STAT_TYPE_YES 					GEN_SYS_YES
#define GPN_STAT_TYPE_NO					GEN_SYS_NO

#define GPN_STAT_PRINT(level, info...) 		GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_STAT_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_STAT_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_STAT_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_STAT_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_STAT_UD5						GEN_SYS_DEBUG_UD5
#define GPN_STAT_UD4						GEN_SYS_DEBUG_UD4
#define GPN_STAT_UD3						GEN_SYS_DEBUG_UD3
#define GPN_STAT_UD2						GEN_SYS_DEBUG_UD2
#define GPN_STAT_UD1						GEN_SYS_DEBUG_UD1
#define GPN_STAT_UD0						GEN_SYS_DEBUG_UD0

//#define _ALL_F								GEN_STAT_GEN_32_FFFF
#define _ALL_F								0x12345678

#define COUNTER								1
#define MEASURE								2

#define DATA_BIT_DEEP_32					1
#define DATA_BIT_DEEP_64					2

#define GPN_STAT_MAX_SUB_NUM_IN_SCAN_TYPE	20
typedef UINT32 GPN_STAT_SCAN_TYPE;

/*stat scan type define */
#define GPN_STAT_SCAN_TYPE_NULL				0x00000000
#define GPN_STAT_SCAN_TYPE_SDH_OH			0x00000001
#define GPN_STAT_SCAN_TYPE_SDH_HP			0x00000002
#define GPN_STAT_SCAN_TYPE_SDH_LP			0x00000003
#define GPN_STAT_SCAN_TYPE_PDH				0x00000004
#define GPN_STAT_SCAN_TYPE_ETH				0x00000005/*RMON ??? or include RMON*/
#if 0 /* delete VCG stat in PTN stat */
#define GPN_STAT_SCAN_TYPE_VCG				0x00000006
#endif
#define GPN_STAT_SCAN_TYPE_PTN_VS			0x00000007
#define GPN_STAT_SCAN_TYPE_PTN_LSP			0x00000008
#define GPN_STAT_SCAN_TYPE_PTN_PW			0x00000009
#define GPN_STAT_SCAN_TYPE_FOLLOW			0x0000000A
#define GPN_STAT_SCAN_TYPE_PWE3				0x0000000B
#define GPN_STAT_SCAN_TYPE_POW				0x0000000C
#define GPN_STAT_SCAN_TYPE_EQU				0x0000000D
#define GPN_STAT_SCAN_TYPE_SOFT 			0x0000000E
#define GPN_STAT_SCAN_TYPE_ENV				0x0000000F
#define GPN_STAT_SCAN_TYPE_ETH_SFP			0x00000010
#define GPN_STAT_SCAN_TYPE_PTN_VPLSPW		0x00000011
#define GPN_STAT_SCAN_TYPE_PTN_V_UNI		0x00000012
#define GPN_STAT_SCAN_TYPE_PTN_MEP			0x00000013
#define GPN_STAT_SCAN_TYPE_ALL				0xFFFFFFFF


typedef UINT32 (*STATPOOLFUN)(optObjOrient *peerPort, UINT32 statType);
typedef UINT32 (*STATUNOTIFYFUN)(optObjOrient *peerPort, UINT32 en);

typedef struct _stStatRTCharacterDef_
{
	GPN_SUB_STAT_TYPE statSubType;
	
	UINT8 isStatDataRept;	/*hist stat data auto report to DA*/
	UINT8 revd1;
	UINT8 revd2;
	UINT8 revd3;

	UINT32 longCycUpThAlm; /*SYS USE alarm sub type code for long cyc up threshold alarm*/
	UINT32 longCycDnThAlm; /*SYS USE alarm sub type code for long cyc down threshold alarm*/
	UINT32 longCycUpThEvt; /*SYS USE event sub type code for long cyc up threshold event*/
	UINT32 longCycDnThEvt; /*SYS USE event sub type code for long cyc down event*/
	
	UINT32 shortCycUpThAlm; /*SYS USE alarm sub type code for short cyc up threshold alarm*/
	UINT32 shortCycDnThAlm; /*SYS USE alarm sub type code for short cyc down threshold alarm*/
	UINT32 shortCycUpThEvt; /*SYS USE event sub type code for short cyc up threshold event*/
	UINT32 shortCycDnThEvt; /*SYS USE event sub type code for short cyc down threshold event*/

	UINT32 userDefCycUpThAlm; /*SYS USE alarm sub type code for user define cyc up threshold alarm*/
	UINT32 userDefCycDnThAlm; /*SYS USE alarm sub type code for user define cyc down threshold alarm*/
	UINT32 userDefCycUpThEvt; /*SYS USE event sub type code for user define cyc up threshold event*/
	UINT32 userDefCycDnThEvt; /*SYS USE event sub type code for user define cyc down threshold event*/

	UINT32 longCycUpThrdH32;	/*default long cyc stat up threshold high32*/
	UINT32 longCycUpThrdL32;	/*default long cyc stat up threshold low32*/
	UINT32 longCycDnThrdH32;	/*default long cyc stat down threshold high32*/
	UINT32 longCycDnThrdL32;	/*default long cyc stat down threshold low32*/
	
	UINT32 shortCycUpThrdH32;	/*default short cyc stat up threshold high32*/
	UINT32 shortCycUpThrdL32;	/*default short cyc stat up threshold low32*/
	UINT32 shortCycDnThrdH32;	/*default short cyc stat down threshold high32*/
	UINT32 shortCycDnThrdL32;	/*default short cyc stat down threshold low32*/
	
	UINT32 devDataSize;

	STATPOOLFUN pStatTpBPFunc;
	STATUNOTIFYFUN pStatuNotfyFunc;
	
	const char * statName;
}stStatRTCharacterDef;

typedef struct _stStatSTCharacterDef_
{
	NODE node;
	GPN_SUB_STAT_TYPE statSubType;
	
	UINT8 countType;	/*is sub stat type counter class or measure class*/
	UINT8 dataBitDeep;	/**/
	UINT8 isAlmRise;
	UINT8 isEvtRise;
	UINT32 numType;     /*is sub stat type natural number or integer number*/
	
	UINT32 longCycUpThAlm; /*SYS USE alarm sub type code for long cyc up threshold alarm*/
	UINT32 longCycDnThAlm; /*SYS USE alarm sub type code for long cyc down threshold alarm*/
	UINT32 longCycUpThEvt; /*SYS USE event sub type code for long cyc up threshold event*/
	UINT32 longCycDnThEvt; /*SYS USE event sub type code for long cyc down event*/
	
	UINT32 shortCycUpThAlm; /*SYS USE alarm sub type code for short cyc up threshold alarm*/
	UINT32 shortCycDnThAlm; /*SYS USE alarm sub type code for short cyc down threshold alarm*/
	UINT32 shortCycUpThEvt; /*SYS USE event sub type code for short cyc up threshold event*/
	UINT32 shortCycDnThEvt; /*SYS USE event sub type code for short cyc down threshold event*/

	UINT32 userDefCycUpThAlm; /*SYS USE alarm sub type code for user define cyc up threshold alarm*/
	UINT32 userDefCycDnThAlm; /*SYS USE alarm sub type code for user define cyc down threshold alarm*/
	UINT32 userDefCycUpThEvt; /*SYS USE event sub type code for user define cyc up threshold event*/
	UINT32 userDefCycDnThEvt; /*SYS USE event sub type code for user define cyc down threshold event*/

	UINT32 longCycUpThrdH32;	/*default long cyc stat up threshold high32*/
	UINT32 longCycUpThrdL32;	/*default long cyc stat up threshold low32*/
	UINT32 longCycDnThrdH32;	/*default long cyc stat down threshold high32*/
	UINT32 longCycDnThrdL32;	/*default long cyc stat down threshold low32*/
	
	UINT32 shortCycUpThrdH32;	/*default short cyc stat up threshold high32*/
	UINT32 shortCycUpThrdL32;	/*default short cyc stat up threshold low32*/
	UINT32 shortCycDnThrdH32;	/*default short cyc stat down threshold high32*/
	UINT32 shortCycDnThrdL32;	/*default short cyc stat down threshold low32*/
	
	UINT32 devDataSize;

	const char * subStatName;
}stStatSTCharacterDef;

typedef struct _stStatTypeDef_
{
	NODE node;
	GPN_STAT_TYPE statType;
	
	UINT32 subStatNumInType;
	LIST subStatTypeCharaQuen;
	
	UINT8 isDataRept;	/*hist stat data auto report to DA*/
	UINT8 revd1;
	UINT8 revd2;
	UINT8 revd3;

	UINT32 isAlmRise;/*so stat alarms in one stat type max 32*/
	UINT32 isEvtRise;/*so stat events in one stat type max 32*/
	
	UINT32 statTpSpSize;
	
	void *pStatLongUpThrd;
	void *pStatLongDnThrd;
	void *pStatShortUpThrd;
	void *pStatShortDnThrd;

	STATPOOLFUN pStatTpBPFunc;
	STATUNOTIFYFUN pStatuNotfyFunc;
	
	const char *statTypeName;
}stStatTypeDef;

typedef struct _stStatScanTypeDef_
{
	NODE node;
	GPN_STAT_SCAN_TYPE statScanType;
	
	UINT32 statSubTpNumInScanType;
	LIST statScanSubTCharaQuen;
	
	UINT32 statTpNumInScanType;
	LIST statTpsOfScanTpQuen;
	
	UINT32 statScanQuenPortNum;
	LIST statScanPortObjQuen;

	UINT32 statScanTpCycle;
	
	UINT32 longCycTimes;
	UINT32 shortCycTimes;

	UINT32 subBitDeep;
	UINT32 subValueType;
	UINT32 subNumType;
	
	void * pStatLongUpThrd;
	void * pStatLongDnThrd;
	void * pStatShortUpThrd;
	void * pStatShortDnThrd;

	UINT32 statScanThrdSize;
	UINT32 statSTDTotalSize;
	
	const char * statScanTypeName;
}stStatScanTypeDef;

typedef struct _stStatSubTpNodeForDifQuen
{
	NODE node;
	stStatSTCharacterDef *pStatSubTpSt;
}stStatSubTpNode;

typedef struct _stStatTpNodeForDifQuen
{
	NODE node;
	stStatTypeDef *pStatTpStr;
}stStatTpNode;

typedef struct _statScanTpTemplate
{
	GPN_STAT_SCAN_TYPE statScanType;
	GPN_STAT_TYPE statTpyeMemb;
	UINT32 statScanCycle;
	const char *statScanTpName;
}statScanTpTemp;

typedef struct _sysSupportStatScanTpTemplate
{
	UINT32 statTpNumInScanTp;
	statScanTpTemp *statScanTpTemp;
}sysSupStatScanTpTemp;

typedef struct _gstStatTypeAndSubTypeInfo
{
	LIST statSubTypeQuen;
	UINT32 statSubTypeNum;
	
	LIST statTypeQuen;
	UINT32 statTypeNum;
	
	LIST statScanTypeQuen;
	UINT32 statScanTypeNum;
	
	stStatSTCharacterDef *pStatSubTypeNodePool;
	stStatTypeDef *pStatTypeNodePool;
	stStatScanTypeDef *pStatScanTypeNodePool;
	
	stStatSubTpNode *pStatSubTpIndexPoolForSubTpQ;
	stStatSubTpNode *pStatSubTpIndexPoolForTpQ;
	stStatTpNode *pStatTpIndexPoolForScanTpQ;

	/*memory pointer*/
	void *pStatTpThrdSp;
	void *pStatScanThrdSp;
	/*here use for stat data notify base msg */
	void *pStatPoolDate;
}gstStatTypeWholwInfo;

UINT32 gpnStatTypeScanTpThresholdCollect(void);
UINT32 gpnStatTypeScanTypeInit(void);
UINT32 gpnStatTypeStatTpThresholdCollect(void);
UINT32 gpnStatTypeSubTypeOrderCheck(void);
UINT32 gpnStatTypeBaseNodeInit(void);
UINT32 gpnStatTypeWholeStInit(void);
UINT32 gpnStatTypeStInit(void);

UINT32 gpnStatTypeInit(void);
UINT32 gpnStatTypeGetScanTypeNode(UINT32 statScanTp, stStatScanTypeDef **ppStatScanTpNode);
UINT32 gpnStatTypeGetFirstScanTypeNode(stStatScanTypeDef **ppStatScanTpNode);
UINT32 gpnStatTypeGetNextScanTypeNode(stStatScanTypeDef *pStatScanTpNode, stStatScanTypeDef **ppStatScanTpNode);
UINT32 gpnStatTypeSubStatType2ScanType(UINT32 subStatTp, UINT32 *statScanTp);
UINT32 gpnStatTypeGetSubStatTypeNode(UINT32 subStatTp, stStatSubTpNode **ppSubStatTpNode);
UINT32 gpnStatTypeGetSubStatTypeNodeBaseScanType(UINT32 subStatTp,
	stStatScanTypeDef *pstatScanType, stStatSTCharacterDef **ppStatSubTypeNode);
UINT32 gpnStatTypeGetFirstSubStatTypeNode(stStatSubTpNode **ppSubStatTpNode);
UINT32 gpnStatTypeGetSubTpOrderBaseScanTypeNode(UINT32 subStatTp, stStatScanTypeDef *pStatScanTpNode, UINT32 *porder);
UINT32 gpnStatTypeGetSubTpOrderBaseScanType(UINT32 subStatTp, UINT32 statScanTp, UINT32 *porder);
UINT32 gpnStatTypeSubThredModify(UINT32 subStatTp, stSubTypeThred *psubTypeThred);
UINT32 debugStatTypePrint(void);

#endif /*_GPN_STAT_TYPE_H_*/

