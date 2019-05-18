/**********************************************************
* file name: gpnAlmTmpRestrain.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-08-24
* function: 
*    define temp restrain
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_TMP_RESTRAIN_H_
#define _GPN_ALM_TMP_RESTRAIN_H_

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

#include "gpnAlmTypeStruct.h"
#include "gpnAlmScan.h"

UINT32 gpnAlmEthSfpPassAlm2EthPhy(stAlmLocalNode *almLocalNode);
UINT32 gpnAlmDiffScanTypeRestrainProc(stAlmLocalNode *mastLocalNode, stAlmLocalNode *slavLocalNode);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif/* _GPN_ALM_TMP_RESTRAIN_H_ */

