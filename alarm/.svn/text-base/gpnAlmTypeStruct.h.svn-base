/**********************************************************
* file name: gpnAlmStruct.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-02
* function: 
*    define alarm data struct
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_TYPE_STRUCT_H_
#define _GPN_ALM_TYPE_STRUCT_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnCommList.h"
#include "socketComm/gpnAlmTypeDef.h"
#include "devCoreInfo/gpnGlobalDevTypeDef.h"
#include "socketComm/gpnDebugFuncApi.h"

#include "type_def.h"
#include "alarm_debug.h"


/*通用定义*/
#define GPN_ALM_GEN_OK				GEN_SYS_OK
#define GPN_ALM_GEN_ERR 			GEN_SYS_ERR

#define GPN_ALM_GEN_ACTIVE 			GEN_SYS_ACTIVE
#define GPN_ALM_GEN_DEACTIVE 		GEN_SYS_DEACTIVE

#define GPN_ALM_GEN_CANCEL			GEN_SYS_CANCEL
#define GPN_ALM_GEN_ENABLE 			GEN_SYS_ENABLE
#define GPN_ALM_GEN_DISABLE 		GEN_SYS_DISABLE

#define EN							GEN_SYS_ENABLE
#define DIS 						GEN_SYS_DISABLE

#define GPN_ALM_GEN_NULL 			GEN_SYS_NULL
#define GPN_ALM_GEN_YES 			GEN_SYS_YES
#define GPN_ALM_GEN_NO 				GEN_SYS_NO

#define GPN_ALM_TYPE_PRINT(level, info...) 	    GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_ALM_TYPE_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_ALM_TYPE_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_ALM_TYPE_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_ALM_TYPE_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_ALM_TYPE_UD5						GEN_SYS_DEBUG_UD5
#define GPN_ALM_TYPE_UD4						GEN_SYS_DEBUG_UD4
#define GPN_ALM_TYPE_UD3						GEN_SYS_DEBUG_UD3
#define GPN_ALM_TYPE_UD2						GEN_SYS_DEBUG_UD2
#define GPN_ALM_TYPE_UD1						GEN_SYS_DEBUG_UD1
#define GPN_ALM_TYPE_UD0						GEN_SYS_DEBUG_UD0

#define GPN_ALM_NULL_MODE 			0
#define GPN_ALM_FIX_MODE 			1
#define GPN_ALM_VARI_MODE			2

/*alm function type define */
#define GPN_ALM_FUNC_NULL           0
#define GPN_ALM_FUNC_HW             1
#define GPN_ALM_FUNC_SERV           2
#define GPN_ALM_FUNC_COMM           3
#define GPN_ALM_FUNC_ENVT           4
#define GPN_ALM_FUNC_SW             5

/*alm level(alm rank) define*/
#define GPN_ALM_RANK_NULL 			0

#define GPN_ALM_RANK_CUR			1
#define GPN_ALM_RANK_COMMON 		2
#define GPN_ALM_RANK_SEVERE			3
#define GPN_ALM_RANK_EMERG 			4

#define GPN_ALM_RANK_TOTAL_NUM 		4

#define GPN_ALM_RANK_ALL			5

/*lamp LED*/
#define LAMP1						1/*支路告警灯*/
#define LAMP2						2/*机架告警灯*/
#define LAMP3						3/*支路与机架告警灯*/
typedef struct _stAlmSTCharacterInfo_
{
	NODE node;
	GPN_ALM_TYPE almSubType;
	
	UINT8 almFunc;
	UINT8 almRank;
	UINT8 isScreen;
	UINT8 isInver;
	
	UINT8 isFilted;
	UINT8 isReport;
	UINT8 lampNotify;
	UINT8 isFillup;
	
	UINT8 almLevel;
	UINT8 isRecord;
	UINT8 almUseBitPosition;
	UINT8 devOrderInScanTp;

	UINT32 evtDetailsLen;
	UINT32 sysUseIdentify;
	UINT32 almUseIdentify;
	UINT32 restrainMask[GPN_ALM_PAGE_NUM];
	const char * almName;
}stAlmSTCharacterDef;

typedef struct _stAlmSubTpNodeForDifQuen_
{
	NODE node;
	stAlmSTCharacterDef *pAlmSubTpStr;
}stAlmSubTpNode;

typedef struct _stAlmRTCharacterDef_
{
	GPN_ALM_TYPE almSubType;
	UINT8 almFunc;
	UINT8 almRank;
	UINT8 isScreen;
	UINT8 isFilted;
	
	UINT8 isReport;
	UINT8 lampNotify;
	UINT8 isFillup;
	UINT8 isRecord;
	
	UINT32 evtDetailsLen;
	
	const char * almName;
}stAlmRTCharacterDef;

typedef struct _stAlmTypeDef_
{
	NODE node;
	GPN_ALM_TYPE almType;
	UINT32 almNumInType;
	LIST almSubTypeCharaQuen;

	UINT32 genAlmNumInType;
	LIST genAlmSubTypeCharaQuen;
	
	const char * almTypeName;
}stAlmTypeDef;

typedef struct _stAlmTpNodeForDifQuen_
{
	NODE node;
	stAlmTypeDef *pAlmTpStr;
}stAlmTpNode;

typedef struct _stAlmScanTypeDef_
{
	NODE node;
	GPN_ALM_TYPE almScanType;
	LIST almScanSubTCharaQuen;
	LIST almTpsOfScanTpQuen;
	LIST almScanPortObjQuen;
	UINT32 almSubTpNumInScanType;
	UINT32 almTpNumInScanType;
	UINT32 almScanQuenPortNum;
	UINT32 almSubTpNumInRestType;
	/*UINT32 portType;*/
	
	UINT32 scanTpOpeCtl;
	UINT32 scanTpCycle;

	UINT32 almUsePag;
	/* in scanType, valid alarm bitPosition set to 1, other set 0, used for screen illegal alarm */
	UINT32 scanTypeAlmMask[GPN_ALM_PAGE_NUM];
	/* scanType's subType opt tmplate, cfg at subType node, relate to scanType, for quick proc */
	/* opt method :&, 0 means screen, 1 means no screen, default for 0, then base template cfg, then base user cfg */
	UINT32 almScreInfo[GPN_ALM_PAGE_NUM];
	/* opt method :&, 0 means filt, 1 means no filt, default for 0, then base template cfg, then base user cfg */
	UINT32 almFiltInfo[GPN_ALM_PAGE_NUM];
	/* opt method :&, 0 means report, 1 means no report, default for 0, then base template cfg, then base user cfg */
	UINT32 almReptInfo[GPN_ALM_PAGE_NUM];
	/* opt method :&, 0 means record, 1 means no record, default for 0, then base template cfg, then base user cfg */
	UINT32 almReckInfo[GPN_ALM_PAGE_NUM];

	/*event about*/
	LIST evtScanSubTCharaQuen;
	LIST evtTpsOfScanTpQuen;
	UINT32 evtSubTpNumInScanType;
	UINT32 evtTpNumInScanType;

	UINT32 evtUsePag;
	UINT32 scanTypeEvtMask[GPN_EVT_PAGE_NUM];
	UINT32 evtScreInfo[GPN_EVT_PAGE_NUM];
	UINT32 evtReptInfo[GPN_EVT_PAGE_NUM];

	LIST genAlmScanSubTCharaQuen;
	LIST genAlmTpsOfScanTpQuen;
	UINT32 genAlmSubTpNumInScanType;
	UINT32 genAlmTpNumInScanType;

	LIST genEvtScanSubTCharaQuen;
	LIST genEvtTpsOfScanTpQuen;
	UINT32 genEvtSubTpNumInScanType;
	UINT32 genEvtTpNumInScanType;
	
	const char * almScanTypeName;
}stAlmScanTypeDef;

/*new add almRestrainType*/
typedef struct _stAlmSTpNodeForDifQuen_
{
	NODE node;
	stAlmScanTypeDef *pAlmSTpStr;
}stAlmSTpNode;

/*new add almRestrainType*/
typedef struct _stAlmRestTypeDef_
{
	NODE node;
	GPN_ALM_TYPE almRestType;
	
	LIST almRestScanTpQuen;
	LIST almRestAlmTpQuen;
	LIST almRestSubTCharaQuen;
	
	UINT32 almScanTpNumInRestType;
	UINT32 almTpNumInRestType;
	UINT32 almSubTpNumInRestType;

	LIST genAlmRestAlmTpQuen;
	LIST genAlmRestSubTCharaQuen;
	
	UINT32 genAlmTpNumInRestType;
	UINT32 genAlmSubTpNumInRestType;
	
	const char * almRestTypeName;
}stAlmRestTypeDef;

typedef struct _almScanTpTemplate_
{
	GPN_ALM_TYPE almScanType;
	GPN_ALM_TYPE almTpyeMemb;
	UINT32 almScanIOCtlCode;
	UINT32 almScanCycle;
	const char *almScanTpName;
}almScanTpTemp;

typedef struct _sysSupportAlmScanTpTemplate_
{
	UINT32 almTpNumInScanTp;
	almScanTpTemp *palmScanTpTemp;
}sysSupAlmScanTpTemp;

typedef struct _almRestTpTemplate_
{
	GPN_ALM_TYPE almRestType;
	GPN_ALM_TYPE almScanTpMemb;
	const char *almRestTpName;
}almRestTpTemp;

typedef struct _restrainTemplate
{
	UINT32 indexNum;
	UINT32 SingOrMulRes;
	GPN_ALM_TYPE beResAlmTp;
	GPN_ALM_TYPE whoResMe;
}restrainTemp;

typedef struct _sysSupportAlmRestionTemplate
{
	UINT32 almScanNumInRestTp;
	almRestTpTemp *palmRestTpTemp;
	UINT32 almRestRuleNum;	
	restrainTemp *SingAndMulResTemp;
}sysSupAlmRestTpTemp;

typedef struct _formRestrainShip_
{
	GPN_ALM_TYPE almSubTp;
	UINT32 level;
	UINT32 bitPosition;
	UINT32 restMask[GPN_ALM_PAGE_NUM];
}formRestrainShip;


typedef struct _stAlmSubTypeDevSup_
{
	UINT8 support[GPN_EQU_TYPE_MAX+2];
}stAlmSubTypeDevSup;

typedef struct _stAlmSubTypeBaseDev_
{
	GPN_ALM_TYPE almSubType;
	stAlmSubTypeDevSup devSup;
}stAlmSubTypeBaseDev;


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                                       event struct define start                                                   */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define GPN_EVT_GEN_OK							GEN_SYS_OK
#define GPN_EVT_GEN_ERR 						GEN_SYS_ERR

/*alm function type define */
#define GPN_EVT_FUNC_NULL                       GPN_ALM_FUNC_NULL
#define GPN_EVT_FUNC_HW                         GPN_ALM_FUNC_HW
#define GPN_EVT_FUNC_SERV                       GPN_ALM_FUNC_SERV
#define GPN_EVT_FUNC_COMM                       GPN_ALM_FUNC_COMM
#define GPN_EVT_FUNC_ENVT                       GPN_ALM_FUNC_ENVT
#define GPN_EVT_FUNC_SW                         GPN_ALM_FUNC_SW

/*event level(evt rank) define*/
#define GPN_EVT_RANK_NULL 						GPN_ALM_RANK_NULL
#define GPN_EVT_RANK_CUR						GPN_ALM_RANK_CUR
#define GPN_EVT_RANK_COMMON 					GPN_ALM_RANK_COMMON
#define GPN_EVT_RANK_SEVERE						GPN_ALM_RANK_SEVERE
#define GPN_EVT_RANK_EMERG 						GPN_ALM_RANK_EMERG
#define GPN_EVT_RANK_ALL						GPN_ALM_RANK_ALL

#define GPN_EVT_TYPE_PRINT(level, info...) 	    GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_EVT_TYPE_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_EVT_TYPE_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_EVT_TYPE_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_EVT_TYPE_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_EVT_TYPE_UD5						GEN_SYS_DEBUG_UD5
#define GPN_EVT_TYPE_UD4						GEN_SYS_DEBUG_UD4
#define GPN_EVT_TYPE_UD3						GEN_SYS_DEBUG_UD3
#define GPN_EVT_TYPE_UD2						GEN_SYS_DEBUG_UD2
#define GPN_EVT_TYPE_UD1						GEN_SYS_DEBUG_UD1
#define GPN_EVT_TYPE_UD0						GEN_SYS_DEBUG_UD0

typedef struct _stEvtSTCharacterInfo_
{
	NODE node;
	GPN_EVT_TYPE evtSubType;

	UINT8 evtFunc;
	UINT8 evtRank;
	UINT8 isScreen;
	UINT8 isReport;
	
	UINT32 detial;

	UINT32 evtUseBitPosition;
	UINT32 sysUseIdentify;
	const char * evtName;
}stEvtSTCharacterDef;

typedef struct _stEvtSubTpNodeForDifQuen_
{
	NODE node;
	stEvtSTCharacterDef *pEvtSubTpStr;
}stEvtSubTpNode;

typedef struct _stEvtRTCharacterDef_
{
	GPN_EVT_TYPE evtSubType;
	
	UINT8 evtFunc;
	UINT8 evtRank;
	UINT8 isScreen;
	UINT8 isReport;
	
	UINT32 detial;

	UINT32 sysUseIdentify;
	const char * evtName;
}stEvtRTCharacterDef;

typedef struct _stEvtTypeDef_
{
	NODE node;
	GPN_EVT_TYPE evtType;
	
	UINT32 evtNumInType;
	LIST evtSubTypeCharaQuen;
	
	UINT32 genEvtNumInType;
	LIST genEvtSubTypeCharaQuen;

	const char * evtTypeName;
}stEvtTypeDef;

typedef struct _stEvtTpNodeForDifQuen_
{
	NODE node;
	stEvtTypeDef *pEvtTpStr;
}stEvtTpNode;

typedef struct _stEvtSubTypeDevSup_
{
	UINT8 support[GPN_EQU_TYPE_MAX+2];
}stEvtSubTypeDevSup;

typedef struct _stEvtSubTypeBaseDev_
{
	GPN_EVT_TYPE evtSubType;
	stEvtSubTypeDevSup devSup;
}stEvtSubTypeBaseDev;


typedef struct _gstAlmTypeAndSubTypeInfo_
{
	/*alarm about : device base alarm type */
	LIST almRestTypeQuen;
	LIST almScanTypeQuen;
	LIST almTypeQuen;
	LIST almSubTypeQuen;

	UINT32 almRestTypeNum;
	UINT32 almScanTypeNum;
	UINT32 almTypeNum;
	UINT32 almSubTypeNum;	
	
	stAlmRestTypeDef *pAlmRestTypeNodePool;
	stAlmScanTypeDef *pAlmScanTypeNodePool;
	stAlmTypeDef *pAlmTypeNodePool;
	stAlmSTCharacterDef *pAlmSubTypeNodePool;
	
	stAlmSTpNode *pAlmSTpIndexPoolforRestTpQ;

	stAlmTpNode *pAlmTpIndexPoolforRestTpQ;
	stAlmTpNode *pAlmTpIndexPoolforScanTpQ;
	
	stAlmSubTpNode *pAlmSubTpIndexPoolforSubTpQ;
	stAlmSubTpNode *pAlmSubTpIndexPoolforAlmTpQ;
	stAlmSubTpNode *pAlmSubTpIndexPoolforRestTpQ;

	/*alarm about : general use alarm type, special 
	support almType & subType base device type */
	LIST genAlmTypeQuen;
	LIST genAlmSubTypeQuen;
	
	UINT32 genAlmTypeNum;
	UINT32 genAlmSubTypeNum;

	stAlmTpNode *pAlmTpIndexPoolforGenAlmTpQ;
	stAlmTpNode *pAlmTpIndexPoolforGenScanTpQ;
	stAlmTpNode *pAlmTpIndexPoolforGenRestTpQ;
	stAlmSubTpNode *pAlmSubTpIndexPoolforGenSubTpQ;
	stAlmSubTpNode *pAlmSubTpIndexPoolforGenAlmTpQ;
	stAlmSubTpNode *pAlmSubTpIndexPoolforGenScanTpQ;
	stAlmSubTpNode *pAlmSubTpIndexPoolforGenRestTpQ;

	/*event about : device bace event type */
	LIST evtTypeQuen;
	LIST evtSubTypeQuen;
	UINT32 evtTypeNum;
	UINT32 evtSubTypeNum;

	stEvtTypeDef *pEvtTypeNodePool;
	stEvtSTCharacterDef *pEvtSubTypeNodePool;
	
	stEvtTpNode *pEvtTpIndexPoolforScanTpQ;
	
	stEvtSubTpNode *pEvtSubTpIndexPoolforEvtTpQ;
	stEvtSubTpNode *pEvtSubTpIndexPoolforSubTpQ;

	/*event about : general use event type*/
	LIST genEvtTypeQuen;
	LIST genEvtSubTypeQuen;
	UINT32 genEvtTypeNum;
	UINT32 genEvtSubTypeNum; 

	stEvtTpNode *pEvtTpIndexPoolforGenEvtTpQ;
	stEvtSubTpNode *pEvtSubTpIndexPoolforGenSubTpQ;
	stEvtSubTpNode *pEvtSubTpIndexPoolforGenEvtTpQ;
	
	stEvtTpNode *pEvtTpIndexPoolforGenScanTpQ;
	stEvtSubTpNode *pEvtSubTpIndexPoolforGenScanTpQ;

}gstAlmTypeWholwInfo;


/*UINT32 gpnAlmStructInitClean(void);*/
UINT32 gpnAlmStructSpaceInit(void);
UINT32 gpnAlmStructInit(void);
UINT32 gpnAlmGenBaseAlmTypeNumGet(UINT32 *pAmTypeNum, UINT32 *pAmSubTypeNum);
UINT32 gpnAlmStructGenAlmTypeInit(void);
UINT32 gpnAlmDevBaseAlmTypeNumGet(UINT32 *pAmTypeNum, UINT32 *pAmSubTypeNum);
UINT32 gpnAlmStructDevBaseAlmTypeInit(void);
UINT32 gpnAlmAlmScanTypeNumGet(UINT32 *pScanTypeNum);
UINT32 gpnAlmStructAlmScanTypeInit(void);
UINT32 gpnAlmAlmRestTypeNumGet(UINT32 *pRestTypeNum);
UINT32 gpnAlmStructAlmRestTypeInit(void);
UINT32 gpnAlmAlmSubTypeSysIdentifyCreat(void);
UINT32 gpnAlmStructAlmRestRuleCheck(void);
UINT32 gpnAlmStructAlmRestRuleInit(void);
UINT32 gpnAlmpRestShipPoolInit(sysSupAlmRestTpTemp *pgAlmRestTpTemp, formRestrainShip **ppRestShipPoolHead);
UINT32 gpnAlmpRestRulerReadInRestShipPool(sysSupAlmRestTpTemp *pgAlmRestTpTemp, formRestrainShip *pRestShipPoolHead);
UINT32 gpnAlmpRestRulerBeRestAlmReadIn(sysSupAlmRestTpTemp *pAlmRestTpTemp, restrainTemp *pRestrainTemp,
	formRestrainShip *pRestShipPoolHead, formRestrainShip **ppRestShipBeRest);
UINT32 gpnAlmpRestRulerCanRestAlmReadIn(sysSupAlmRestTpTemp *pAlmRestTpTemp, restrainTemp *pRestrainTemp,
	formRestrainShip *pRestShipPoolHead, formRestrainShip *pRestShipBeRest, UINT32 *pTopLevelNow, UINT32 *pTopLevelMach);
UINT32 gpnAlmpRestShipPoolReorderBaseLevel(sysSupAlmRestTpTemp *pAlmRestTpTemp, formRestrainShip *pRestShipPoolHead);
UINT32 gpnAlmpRestShipPoolReorderBaseBitPosition(sysSupAlmRestTpTemp *pAlmRestTpTemp, formRestrainShip *pRestShipPoolHead);
UINT32 gpnAlmpRestShipPoolRestTransmit(sysSupAlmRestTpTemp *pAlmRestTpTemp, formRestrainShip *pRestShipPoolHead);
UINT32 gpnAlmRestSubTypeInfoFinish(sysSupAlmRestTpTemp *pAlmRestTpTemp, formRestrainShip *pRestShipPool);
UINT32 gpnAlmRestScanTypeInfoFinish(sysSupAlmRestTpTemp *pAlmRestTpTemp, formRestrainShip *pRestShipPool);
UINT32 debugGpnAlmRestShipPool(sysSupAlmRestTpTemp *pAlmRestTpTemp, formRestrainShip *pRestShipPoolHead);

UINT32 gpnAlmRestSpecialArrayInit(UINT32 *array, UINT32 arraySize);
UINT32 gpnAlmRestSpecialArrayClear(UINT32 *array, UINT32 arraySize);
UINT32 gpnAlmRestSpecialArrayBitIdGet(UINT32 *bitId, UINT32 arraySize, UINT32 bitPosit);
UINT32 gpnAlmRestSpecialArrayIsZero(UINT32 *array, UINT32 arraySize);
UINT32 gpnAlmRestSpecialArrayIsBitSet(UINT32 *array, UINT32 arraySize, UINT32 bitPosit);
UINT32 gpnAlmRestSpecialArrayBitWithAndOpt(UINT32 *arrayDst, UINT32 *arraySrc, UINT32 arraySize);
UINT32 gpnAlmRestSpecialArrayBitWithOrOpt(UINT32 *arrayDst, UINT32 *arraySrc, UINT32 arraySize);
UINT32 gpnAlmRestSpecialArrayBitReverseOpt(UINT32 *array, UINT32 arraySize);
UINT32 gpnAlmRestSpecialArrayCpyOpt(UINT32 *arrayDst, UINT32 *arraySrc, UINT32 arraySize);
UINT32 gpnAlmRestSpecialArrayCmpOpt(UINT32 *arrayDst, UINT32 *arraySrc, UINT32 arraySize);
UINT32 gpnAlmRestSpecialArrayClearBit(UINT32 *array, UINT32 arraySize, UINT32 bitPosit);
UINT32 gpnAlmRestSpecialArraySetBit(UINT32 *array, UINT32 arraySize, UINT32 bitPosit);
UINT32 gpnAlmRestSpecialArrayClearBits(UINT32 *array, UINT32 arraySize, UINT32 bitNum, UINT32 offset);
UINT32 gpnAlmRestSpecialArraySetBits(UINT32 *array, UINT32 arraySize, UINT32 bitNum, UINT32 offset);
UINT32 gpnAlmRestSpecialArrayLeftMov(UINT32 *array, UINT32 arraySize, UINT32 scale, UINT32 fillBit);
UINT32 degubGpnGenAlmSubTypeStrutCheck(void);
UINT32 degubGpnGenAlmTypeStrutCheck(void);
UINT32 degubGpnAlmSubTypeStrutCheck(void);
UINT32 degubGpnAlmTypeStrutCheck(void);
UINT32 degubGpnAlmScanTypeStrutCheck(void);
UINT32 degubGpnAlmRestTypeStrutCheck(void);
UINT32 degubGpnAlmStrutGlobalCheck(void);

UINT32 gpnEvtStructInit(void);
UINT32 gpnEvtStructGenEvtTypeInit(void);
UINT32 gpnEvtStructDevBaseEvtTypeInit(void);
UINT32 gpnEvtStruct2ScanTypeInit(void);
UINT32 gpnEvtEvtSubTypeSysIdentifyCreat(void);
UINT32 gpnEvtScanTypeEvtAtrribInit(void);
UINT32 degubGpnEvtTypeStrutCheck(void);

UINT32 IS_REVERSE_ALM(UINT32 subAlmType);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_ALM_TYPE_STRUCT_H_ */

