/**********************************************************
* file name: gpnDebugFuncApi.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-30
* function: 
*    define gpn_socket comm apply in xxx module
* modify:
*
***********************************************************/
#ifndef _GPN_DEBUG_FUNC_API_C_
#define _GPN_DEBUG_FUNC_API_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*log function include*/
#include "socketComm/gpnDebug.h"
#include "socketComm/gpnDebugFuncApi.h"

/*demo:
	gpnDebugApiSmpStelyInit(GPN_SMP_DEBUG_USER_MODE,\
		GPN_SMP_EGP_DEBUG_EN|GPN_SMP_EGP_DEBUG_UD4);
		
 	gpnDebugApiSmpStelyInit(GPN_SMP_DEBUG_DEFAULT_MODE, 0)
 */
UINT32 gpnDebugApiSmpStelyInit(UINT32 mode, UINT32 debugCtrl)
{
	if(mode == GPN_SMP_DEBUG_DEFAULT_MODE)
	{
		gpnSmpDebugDefaultInit();
	}
	else /*if(mode == GPN_SMP_DEBUG_USER_MODE)*/
	{
		gpnSmpDebugUserDefInit(debugCtrl);
	}
	return GPN_DEBUG_FUNC_API_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_DEBUG_FUNC_API_C_*/

