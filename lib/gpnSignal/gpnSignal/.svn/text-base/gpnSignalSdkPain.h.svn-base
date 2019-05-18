/**********************************************************
* file name: gpnSignalSdkPain.h
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2015-05-01
* function: 
*    define SDK pain signal process about used by GPN_SIGNAL
* modify:
*
***********************************************************/
#ifndef _GPN_SIGNAL_SDK_PAIN_H_
#define _GPN_SIGNAL_SDK_PAIN_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnDebugFuncApi.h"

#include "gpnSignal/gpnSignalProc.h"

UINT32 gpnSignalFixSdkPainSigHandler(SIGHANDLER pdHandler);
UINT32 gpnSignalSendSdkPainSig(void);
void gpnSignalProcSdkPainSigInCPX10(INT32 sig);
void gpnNetlinkSdkPainProc(void);
void gpnNetlinkSdkPainMsgRecv(UINT32 p1, UINT32 p2);
UINT32 gpnNetlinkSdkPainHandle(void);
UINT32 gpnNetlinkSdkPainReboot(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SIGNAL_SDK_PAIN_H_ */
