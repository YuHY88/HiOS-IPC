/**********************************************************
* file name: gpnSignalPowDown.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-10-23
* function: 
*    define power down signal process about used by GPN_SIGNAL
* modify:
*
***********************************************************/
#ifndef _GPN_SIGNAL_POW_DOWN_H_
#define _GPN_SIGNAL_POW_DOWN_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnDebugFuncApi.h"

#include "gpnSignal/gpnSignalProc.h"

UINT32 gpnSignalFixPowDownSigHandler(SIGHANDLER pdHandler);
UINT32 gpnSignalSendPowDownSig(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SIGNAL_POW_DOWN_H_ */
