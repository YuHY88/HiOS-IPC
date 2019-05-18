/**********************************************************
* file name: gpnSignalReboot.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-08-15
* function: 
*    define reboot/reload signal process about used by GPN_SIGNAL
* modify:
*
***********************************************************/
#ifndef _GPN_SIGNAL_REBOOT_C_
#define _GPN_SIGNAL_REBOOT_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include "gpnSignal/gpnSignalReboot.h"

UINT32 gpnSignalFixRebootSigHandler(SIGHANDLER pdHandler)
{
	/*assert */
	if(pdHandler == NULL)
	{
		GPN_SIG_PROC_PRINT(GPN_SIG_PROC_EGP, "%s : para err!",\
			__FUNCTION__);
		return GPN_SIG_PROC_GEN_ERR;
	}

	/*gpnSignalFixSigWithHandler(GPN_SIG_TYPE_REBOOT, pdHandler);*/
	
	gpnSignalFixSigWithHandlerMaskAll(GPN_SIG_TYPE_REBOOT, pdHandler);

	return GPN_SIG_PROC_GEN_OK;
}

UINT32 gpnSignalSendRebootSig(void)
{
	/*gpnSignalSendSmpSig2AllPID(GPN_SIG_TYPE_REBOOT);*/
	//gpnSignalSendSmpSig2AllPID(GPN_SIG_TYPE_REBOOT);

	return GPN_SIG_PROC_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SIGNAL_REBOOT_C_ */

