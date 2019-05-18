/**********************************************************
* file name: gpnSignalPowDown.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-10-23
* function: 
*    define power down signal process about used by GPN_SIGNAL
* modify:
*
***********************************************************/
#ifndef _GPN_SIGNAL_POW_DOWN_C_
#define _GPN_SIGNAL_POW_DOWN_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include "gpnSignal/gpnSignalPowDown.h"

UINT32 gpnSignalFixPowDownSigHandler(SIGHANDLER pdHandler)
{
	/*assert */
	if(pdHandler == NULL)
	{
		GPN_SIG_PROC_PRINT(GPN_SIG_PROC_EGP, "%s : para err!",\
			__FUNCTION__);
		return GPN_SIG_PROC_GEN_ERR;
	}

	/*gpnSignalFixSigWithHandler(GPN_SIG_TYPE_POW_DOWN, pdHandler);*/
	
	gpnSignalFixSigWithHandlerMaskAll(GPN_SIG_TYPE_POW_DOWN, pdHandler);

	return GPN_SIG_PROC_GEN_OK;
}

UINT32 gpnSignalSendPowDownSig(void)
{
	/*gpnSignalSendSmpSig2AllPID(GPN_SIG_TYPE_POW_DOWN);*/
	gpnSignalSendSmpSig2AllPID(GPN_SIG_TYPE_POW_DOWN);

	return GPN_SIG_PROC_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SIGNAL_POW_DOWN_C_ */
