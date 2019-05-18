/**********************************************************
* file name: gpnStatDebug.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-10-11
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_DEBUG_H_
#define _GPN_STAT_DEBUG_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "type_def.h"
#include "stat_debug.h"



/***************************************************************/
/* 按位定义debug信息的类型 */
#define STATIS_DBG_REGISTER		(1 << 0)		//statis register debug
#define STATIS_DBG_DATA_COLLECT	(1 << 1)		//statis data collect debug
#define STATIS_DBG_GET			(1 << 2)		//snmp get
#define STATIS_DBG_GET_NEXT		(1 << 3)		//snmp get-bulk
#define STATIS_DBG_SET			(1 << 4)		//snmp set

#define STATIS_DBG_ALL			(STATIS_DBG_REGISTER |\
							 	STATIS_DBG_DATA_COLLECT |\
							 	STATIS_DBG_GET |\
							 	STATIS_DBG_GET_NEXT |\
							 	STATIS_DBG_SET)	//所有debug


/***************************************************************/




#define GPN_STAT_DEBUG_OK 			GEN_SYS_OK
#define GPN_STAT_DEBUG_ERR 			GEN_SYS_ERR

#define GPN_STAT_DEBUG_YES 			GEN_SYS_YES
#define GPN_STAT_DEBUG_NO 			GEN_SYS_NO

#define GPN_STAT_DEBUG_ENABLE 		GEN_SYS_ENABLE
#define GPN_STAT_DEBUG_DISABLE 		GEN_SYS_DISABLE

typedef struct _stgpnStatTestData_
{
	UINT32 h1;
	UINT32 l1;
	UINT32 h2;
	UINT32 l2;

	UINT32 x3;

	UINT32 h4;
	UINT32 l4;

	UINT32 x5;
}stgpnStatTestData;

void gpnStatForNsmFollowTest(UINT32 para1, UINT32 para2);
void gpnStatForSdmMsgTest(UINT32 para1, UINT32 para2);
void gpnStatForOamMsgTest(UINT32 para1, UINT32 para2);
void gpnStatForOnmdMsgTest(UINT32 para1, UINT32 para2);
void gpnStatForEvnThredTpTest(UINT32 para1, UINT32 para2);
void gpnStatForNsmMsgTest(UINT32 para1, UINT32 para2);
void gpnStatForFixTableTest(UINT32 para1, UINT32 para2);
void gpnStatForAlarmThredTpTest(UINT32 para1, UINT32 para2);
void gpnStatForEventThredTpTest(UINT32 para1, UINT32 para2);
void gpnStatForSubFiltTpTest(UINT32 para1, UINT32 para2);
void gpnStatForSubReportTpTest(UINT32 para1, UINT32 para2);
void gpnStatForTaskTest(UINT32 para1, UINT32 para2);
void gpnStatForPortMonTest(UINT32 para1, UINT32 para2);
void gpnStatForCurrDataGet(UINT32 para1, UINT32 para2);
void gpnStatForHistDataGet(UINT32 para1, UINT32 para2);
void gpnStatForCalculateTest(UINT32 para1, UINT32 para2);
void gpnStatForXPtnLspDataGetRsp(UINT32 para1, UINT32 para2);
void gpnStatMsgForPtnLspPStatGet(UINT32 para1, UINT32 para2);
void gpnStatMsgForPtnEthPStatGet(UINT32 para1, UINT32 para2);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_STAT_DEBUG_H_*/
