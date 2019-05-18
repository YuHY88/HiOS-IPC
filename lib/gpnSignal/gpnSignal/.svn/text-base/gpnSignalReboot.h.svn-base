/**********************************************************
* file name: gpnSignalReboot.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-08-15
* function: 
*    define reboot/reload signal process about used by GPN_SIGNAL
* modify:
*
***********************************************************/
#ifndef _GPN_SIGNAL_REBOOT_H_
#define _GPN_SIGNAL_REBOOT_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnDebugFuncApi.h"

#include "gpnSignal/gpnSignalProc.h"

UINT32 gpnSignalFixPowDownSigHandler(SIGHANDLER pdHandler);
UINT32 gpnSignalSendPowDownSig(void);

UINT32 gpnSignalFixRebootSigHandler(SIGHANDLER pdHandler);
UINT32 gpnSignalSendRebootSig(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SIGNAL_REBOOT_H_ */
