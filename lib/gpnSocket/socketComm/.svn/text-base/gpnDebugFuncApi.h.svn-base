/**********************************************************
* file name: gpnSockCommFuncApi.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-29
* function: 
*    define gpn_socket comm apply in xxx modules
* modify:
*
***********************************************************/
#ifndef _GPN_DEBUG_FUNC_API_H_
#define _GPN_DEBUG_FUNC_API_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnSockMsgDef.h"
/*debug function include*/
#include "lib/gpnSocket/socketComm/gpnDebug.h"


#define GPN_DEBUG_FUNC_API_GEN_OK 					GPN_SOCK_SYS_OK
#define GPN_DEBUG_FUNC_API_ERR  					GPN_SOCK_SYS_ERR

#define GPN_DEBUG_FUNC_API_GEN_YES 					GPN_SOCK_SYS_YES
#define GPN_DEBUG_FUNC_API_GEN_NO 					GPN_SOCK_SYS_NO

#define GPN_DEBUG_FUNC_API_PRINT(level, info...) 	GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_DEBUG_FUNC_API_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_DEBUG_FUNC_API_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_DEBUG_FUNC_API_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_DEBUG_FUNC_API_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_DEBUG_FUNC_API_UD5						GEN_SYS_DEBUG_UD5
#define GPN_DEBUG_FUNC_API_UD4						GEN_SYS_DEBUG_UD4
#define GPN_DEBUG_FUNC_API_UD3						GEN_SYS_DEBUG_UD3
#define GPN_DEBUG_FUNC_API_UD2						GEN_SYS_DEBUG_UD2
#define GPN_DEBUG_FUNC_API_UD1						GEN_SYS_DEBUG_UD1
#define GPN_DEBUG_FUNC_API_UD0						GEN_SYS_DEBUG_UD0

#define GPN_SMP_DEBUG_DEFAULT_MODE					1
#define GPN_SMP_DEBUG_USER_MODE						2

#define GPN_SMP_EGP_DEBUG_EN					 	(1U<<(GEN_SYS_DEBUG_EGP))
#define GPN_SMP_SVP_DEBUG_EN 						(1U<<(GEN_SYS_DEBUG_SVP))
#define GPN_SMP_CMP_DEBUG_EN 						(1U<<(GEN_SYS_DEBUG_CMP))
#define GPN_SMP_CUP_DEBUG_EN 						(1U<<(GEN_SYS_DEBUG_CUP))
#define GPN_SMP_UD5_DEBUG_EN 						(1U<<(GEN_SYS_DEBUG_UD5))
#define GPN_SMP_UD4_DEBUG_EN						(1U<<(GEN_SYS_DEBUG_UD4))
#define GPN_SMP_UD3_DEBUG_EN						(1U<<(GEN_SYS_DEBUG_UD3))
#define GPN_SMP_UD2_DEBUG_EN						(1U<<(GEN_SYS_DEBUG_UD2))
#define GPN_SMP_UD1_DEBUG_EN						(1U<<(GEN_SYS_DEBUG_UD1))
#define GPN_SMP_UD0_DEBUG_EN						(1U<<(GEN_SYS_DEBUG_UD0))

#define GPN_SMP_RELEASE_DEBUG						GPN_SMP_EGP_DEBUG_EN 

UINT32 gpnDebugApiSmpStelyInit(UINT32 mode, UINT32 debugCtrl);



#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_DEBUG_FUNC_API_H_*/

