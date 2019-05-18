/**********************************************************
* file name: gpnLogFuncApi.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-29
* function: 
*    define gpn_log apply in xxx modules
* modify:
*
***********************************************************/
#ifndef _GPN_LOG_FUNC_API_H_
#define _GPN_LOG_FUNC_API_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnSockMsgDef.h"
/*debug function include*/
#include "lib/gpnSocket/socketComm/gpnDebugFuncApi.h"
#include "lib/gpnLog/gpnLog/gpnLog.h"

#define GPN_LOG_FUNC_API_GEN_OK 					GPN_SOCK_SYS_OK
#define GPN_LOG_FUNC_API_GEN_ERR  					GPN_SOCK_SYS_ERR

#define GPN_LOG_FUNC_API_GEN_YES 					GPN_SOCK_SYS_YES
#define GPN_LOG_FUNC_API_GEN_NO 					GPN_SOCK_SYS_NO

#define GPN_LOG_FUNC_API_GEN_ENABLE 				GPN_SOCK_SYS_ENABLE
#define GPN_LOG_FUNC_API_GEN_DISABLE				GPN_SOCK_SYS_DISABLE

#define GPN_LOG_FUNC_API_PRINT(level, info...)
#define GPN_LOG_FUNC_API_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_LOG_FUNC_API_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_LOG_FUNC_API_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_LOG_FUNC_API_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_LOG_FUNC_API_UD5						GEN_SYS_DEBUG_UD5
#define GPN_LOG_FUNC_API_UD4						GEN_SYS_DEBUG_UD4
#define GPN_LOG_FUNC_API_UD3						GEN_SYS_DEBUG_UD3
#define GPN_LOG_FUNC_API_UD2						GEN_SYS_DEBUG_UD2
#define GPN_LOG_FUNC_API_UD1						GEN_SYS_DEBUG_UD1
#define GPN_LOG_FUNC_API_UD0						GEN_SYS_DEBUG_UD0

/* suggest module log file name */


UINT32 gpnLogFuncApiLogEnable(char *token, UINT32 logType, UINT8 high_pri, UINT8 low_pri);
UINT32 gpnLogFuncApiLogDisable(char *token, UINT32 logType);
UINT32 gpnLog(UINT8 priority, const INT8 *format, ...);
UINT32 gpnPerrorLog(const INT8 *errFun);

#define GPN_FUNC_SUP_LOG_EN
/*#undef GPN_MEM_DEP_MGT_EN*/

#if defined(GPN_FUNC_SUP_LOG_EN)
#define GPN_LOG(priority, format, ...) gpnLog(priority, format)
#define GPN_PERROR_LOG(errFun) gpnPerrorLog(errFun)

#else
#define GPN_LOG(priority, format, ...)
#define GPN_PERROR_LOG(errFun)

#endif



#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_LOG_FUNC_API_H_*/

