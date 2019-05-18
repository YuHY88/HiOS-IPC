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
#ifndef _GPN_SOCK_COMM_FUNC_API_H_
#define _GPN_SOCK_COMM_FUNC_API_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnSelectOpt.h"
/*debug function include*/
#include "lib/gpnSocket/socketComm/gpnDebugFuncApi.h"

#define GPN_SC_FUNC_API_GEN_OK 						GPN_SOCK_SYS_OK
#define GPN_SC_FUNC_API_GEN_ERR  					GPN_SOCK_SYS_ERR

#define GPN_SC_FUNC_API_GEN_YES 					GPN_SOCK_SYS_YES
#define GPN_SC_FUNC_API_GEN_NO 						GPN_SOCK_SYS_NO

#define GPN_SC_FUNC_API_PRINT(level, info...) 		GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_SC_FUNC_API_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_SC_FUNC_API_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_SC_FUNC_API_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_SC_FUNC_API_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_SC_FUNC_API_UD5							GEN_SYS_DEBUG_UD5
#define GPN_SC_FUNC_API_UD4							GEN_SYS_DEBUG_UD4
#define GPN_SC_FUNC_API_UD3							GEN_SYS_DEBUG_UD3
#define GPN_SC_FUNC_API_UD2							GEN_SYS_DEBUG_UD2
#define GPN_SC_FUNC_API_UD1							GEN_SYS_DEBUG_UD1
#define GPN_SC_FUNC_API_UD0							GEN_SYS_DEBUG_UD0

#define GPN_SC_FUNC_API_PROC_CYC					1
#define GPN_SC_FUNC_API_ONCE_MAX_PROC				10

UINT32 gpnSockCommApiSockCommStartup(UINT32 funcRole, usrMsgProc funUsrMsgProc);
UINT32 gpnSockCommApiNoBlockProc(void);
UINT32 gpnSockCommApiBlockProc(void);
UINT32 gpnSockCommApiSelectTimerNoBlockProc(void);
UINT32 gpnSockCommApiSelectTimerBlockProc(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_SOCK_COMM_FUNC_API_H_*/

