/**********************************************************
* file name: gpnAlmDebugScalar.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-19
* function: 
*    define alarm scalar get/set debug
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_DEBUG_SCALAR_C_
#define _GPN_ALM_DEBUG_SCALAR_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef GPN_ALM_INSERT_DEBUG

#endif /* GPN_ALM_INSERT_DEBUG */
#if 1 /* GPN_ALM_INSERT_DEBUG */

#include "socketComm/gpnGlobalPortIndexDef.h"

#include "gpnAlmDebugScalar.h"
#include "gpnAlmPortNotify.h"
#include "gpnAlmAlmNotify.h"
#include "gpnAlmDataStructApi.h"
#include "gpnAlmMsgProc.h"
#include "gpnAlmDataSheet.h"

UINT32 debugGpnAlmAllScalarGet(void)
{
	UINT32 rDelay;
	UINT32 cDelay;
	UINT32 bEn;
	UINT32 bThr;
	UINT32 restMode;
	UINT32 reveMode;
	UINT32 cCyc;
	UINT32 hCyc;
	UINT32 eCyc;
	UINT32 cSize;
	UINT32 hSize;
	UINT32 eSize;
	
	gpnAlmDSApiRiseDelayGet(&rDelay);
	gpnAlmDSApiCleanDelayGet(&cDelay);
	gpnAlmDSApiBuzzerEnGet(&bEn);
	gpnAlmDSApiBuzzerThreGet(&bThr);
	gpnAlmDSApiRestModeGet(&restMode);
	gpnAlmDSApiReverModeGet(&reveMode);
	gpnAlmDSApiCurrAlmDBCycGet(&cCyc);
	gpnAlmDSApiHistAlmDBCycGet(&hCyc);
	gpnAlmDSApiEventDBCycGet(&eCyc);
	gpnAlmDSApiCurrDBSizeGet(&cSize);
	gpnAlmDSApiHistDBSizeGet(&hSize);
	gpnAlmDSApiEventDBSizeGet(&eSize);

	printf("debug alarm scalar :\n\r"
		"rDelay(%d) cDelay(%d) bEn(%d) bThr(%d)\n\r"
		"restMode(%d) reveMode(%d) cCyc(%d) hCyc(%d)\n\r"
		"eCyc(%d) cSize(%d) hSize(%d) eSize(%d)\n\r",\
		rDelay, cDelay, bEn, bThr,\
		restMode, reveMode, cCyc, hCyc,\
		eCyc, cSize, hSize, eSize);
	
	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmSmpScalarCfg(void)
{
	UINT32 rDelay;
	UINT32 cDelay;
	UINT32 bEn;
	UINT32 bThr;
	UINT32 restMode;
	UINT32 reveMode;

	rDelay = 5;
	gpnAlmDSApiRiseDelayCfg(rDelay);
	gpnAlmDSApiRiseDelayGet(&rDelay);
	cDelay = 2;
	gpnAlmDSApiCleanDelayCfg(cDelay);
	gpnAlmDSApiCleanDelayGet(&cDelay);
	bEn = GPN_ALM_DEBUG_DISABLE;
	gpnAlmDSApiBuzzerEnCfg(bEn);
	gpnAlmDSApiBuzzerEnGet(&bEn);
	bThr = 4;
	gpnAlmDSApiBuzzerThreCfg(bThr);
	gpnAlmDSApiBuzzerThreGet(&bThr);
	restMode = 2;
	gpnAlmDSApiRestModeCfg(restMode);
	gpnAlmDSApiRestModeGet(&restMode);
	reveMode = 2;
	gpnAlmDSApiReverModeCfg(reveMode);
	gpnAlmDSApiReverModeGet(&reveMode);

	printf("debug alarm scalar :\n\r"
		"rDelay(%d) cDelay(%d) bEn(%d) bThr(%d)\n\r"
		"restMode(%d) reveMode(%d)\n\r",\
		rDelay, cDelay, bEn, bThr,\
		restMode, reveMode);

	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmScalar(void)
{
	debugGpnAlmAllScalarGet();
	debugGpnAlmSmpScalarCfg();
	
	return GPN_ALM_DEBUG_OK;
}

#endif /* GPN_ALM_INSERT_DEBUG */

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_DEBUG_SCALAR_C_*/

