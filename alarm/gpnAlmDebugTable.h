/**********************************************************
* file name: gpnAlmDebugTable.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-19
* function: 
*    define alarm table get/getNext/set debug
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_DEBUG_TABLE_H_
#define _GPN_ALM_DEBUG_TABLE_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef GPN_ALM_INSERT_DEBUG

#endif /* GPN_ALM_INSERT_DEBUG */
#if 1 /* GPN_ALM_INSERT_DEBUG */

#include <stdio.h>

#include "gpnAlmDebug.h"

UINT32 debugGpnAlmDevSelfNotify(void);
UINT32 debugGpnAlmFixPortSelfNotify(void);
UINT32 debugGpnAlmVariPorNotify(void);
UINT32 debugGpnAlmPortMonTableGet(void);
UINT32 debugGpnAlmPortMonTableGetNext(void);
UINT32 debugGpnAlmAttributeTableGet(void);
UINT32 debugGpnAlmAttributeTableGetNext(void);
UINT32 debugGpnAlmAlmReport(void);
UINT32 debugGpnAlmCurrAlmTableGet(void);
UINT32 debugGpnAlmCurrAlmTableGetNext(void);
UINT32 debugGpnAlmHistAlmTableGet(void);
UINT32 debugGpnAlmHistAlmTableGetNext(void);
UINT32 debugGpnAlmEvtReport(void);
UINT32 debugGpnAlmEventTableGet(void);
UINT32 debugGpnAlmEventTableGetNext(void);
UINT32 debugGpnAlmTable(void);
UINT32 debugGpnAlmProductCountClear(void);
UINT32 debugGpnAlmAttributeRankCfg(void);
UINT32 debugGpnAlmAttributeScreenCfg(void);
UINT32 debugGpnAlmAttributeFiltCfg(void);
UINT32 debugGpnAlmAttributeReportCfg(void);
UINT32 debugGpnAlmAttributeRecordCfg(void);
UINT32 debugGpnAlmAttributeReverseCfg(void);
UINT32 debugGpnAlmAttributeFiltCfg(void);
UINT32 debugGpnAlmAttributeReportCfg(void);
UINT32 debugGpnAlmAttributeReverseCfg(void);
UINT32 debugGpnAlmAlmTableCyc(void);


#endif /* GPN_ALM_INSERT_DEBUG */

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_ALM_DEBUG_TABLE_H_ */
