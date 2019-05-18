/**********************************************************
* file name: gpnSignalProc.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-10-23
* function: 
*    define signal process about used by GPN_SIGNAL
* modify:
*
***********************************************************/
#ifndef _GPN_SIGNAL_PROC_C_
#define _GPN_SIGNAL_PROC_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>

#include "gpnSignal/gpnSignalProc.h"

UINT32 gpnSignalFixSigWithHandler(INT32 sig, SIGHANDLER handler)
{
	if(handler != NULL)
	{
		signal(sig, handler);
		return GPN_SIG_PROC_GEN_OK;
	}
	else
	{
		GPN_SIG_PROC_PRINT(GPN_SIG_PROC_EGP, "%s : para err!",\
			__FUNCTION__);
		return GPN_SIG_PROC_GEN_ERR;
	}
}

UINT32 gpnSignalFixSigWithHandlerMaskAll(INT32 sig, SIGHANDLER handler)
{
	struct sigaction act;

	/*assert */
	if(handler == NULL)
	{
		GPN_SIG_PROC_PRINT(GPN_SIG_PROC_EGP, "%s : para err!",\
			__FUNCTION__);
		return GPN_SIG_PROC_GEN_ERR;
	}

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(sig, &act, NULL);

	return GPN_SIG_PROC_GEN_OK;
}

UINT32 gpnSignalSendSmpSig2AllPID(INT32 sig)
{
	kill(-1, sig);
	
	return GPN_SIG_PROC_GEN_OK;
}
UINT32 gpnSignalSendSmpSig2SelfPGID(INT32 sig)
{
	kill(0, sig);
	
	return GPN_SIG_PROC_GEN_OK;
}

UINT32 gpnSignalSendSmpSig2PID(INT32 sig, UINT32 pid)
{
	kill(pid, sig);
	
	return GPN_SIG_PROC_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SIGNAL_PROC_C_ */
