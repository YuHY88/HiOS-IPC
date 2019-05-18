/**********************************************************
* file name: gpnSignalProc.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-10-23
* function: 
*    define signal process about used by GPN_SIGNAL
* modify:
*
***********************************************************/
#ifndef _GPN_SIGNAL_PROC_H_
#define _GPN_SIGNAL_PROC_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <signal.h>

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnDebugFuncApi.h"

#include "gpnSignal/gpnSignalType.h"


#define GPN_SIG_PROC_GEN_OK                             GPN_SOCK_SYS_OK
#define GPN_SIG_PROC_GEN_ERR                            GPN_SOCK_SYS_ERR

#define GPN_SIG_PROC_GEN_YES                            GPN_SOCK_SYS_YES
#define GPN_SIG_PROC_GEN_NO                             GPN_SOCK_SYS_NO

#define GPN_SIG_PROC_PRINT(level, info...)              GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_SIG_PROC_EGP                                GEN_SYS_DEBUG_EGP
#define GPN_SIG_PROC_SVP                                GEN_SYS_DEBUG_SVP
#define GPN_SIG_PROC_CMP                                GEN_SYS_DEBUG_CMP
#define GPN_SIG_PROC_CUP                                GEN_SYS_DEBUG_CUP
#define GPN_SIG_PROC_UD5                                GEN_SYS_DEBUG_UD5
#define GPN_SIG_PROC_UD4                                GEN_SYS_DEBUG_UD4
#define GPN_SIG_PROC_UD3                                GEN_SYS_DEBUG_UD3
#define GPN_SIG_PROC_UD2                                GEN_SYS_DEBUG_UD2
#define GPN_SIG_PROC_UD1                                GEN_SYS_DEBUG_UD1
#define GPN_SIG_PROC_UD0                                GEN_SYS_DEBUG_UD0

typedef __sighandler_t SIGHANDLER;

UINT32 gpnSignalFixSigWithHandler(INT32 sig, SIGHANDLER handler);
UINT32 gpnSignalFixSigWithHandlerMaskAll(INT32 sig, SIGHANDLER handler);
UINT32 gpnSignalSendSmpSig2AllPID(INT32 sig);
UINT32 gpnSignalSendSmpSig2SelfPGID(INT32 sig);
UINT32 gpnSignalSendSmpSig2PID(INT32 sig, UINT32 pid);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SIGNAL_PROC_H_ */
