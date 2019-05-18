/**********************************************************
* file name: gpnLogUnifyPath.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-30
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_LOG_UNIFY_PATH_H_
#define _GPN_LOG_UNIFY_PATH_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include "socketComm/gpnUnifyPathDef.h"
#include "gpnLog/gpnLogFuncApi.h"


#define GPN_LOG_UP_OK 						GPN_SOCK_SYS_OK
#define GPN_LOG_UP_ERR 						GPN_SOCK_SYS_ERR

#define GPN_LOG_UP_PRINT(level, info...) 	GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_LOG_UP_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_LOG_UP_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_LOG_UP_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_LOG_UP_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_LOG_UP_UD5						GEN_SYS_DEBUG_UD5
#define GPN_LOG_UP_UD4						GEN_SYS_DEBUG_UD4
#define GPN_LOG_UP_UD3						GEN_SYS_DEBUG_UD3
#define GPN_LOG_UP_UD2						GEN_SYS_DEBUG_UD2
#define GPN_LOG_UP_UD1						GEN_SYS_DEBUG_UD1
#define GPN_LOG_UP_UD0						GEN_SYS_DEBUG_UD0


#define GPN_LOG_UP_ROM_NODE					GPN_UNIFY_PATH_ROM_NODE(config/gpn/gpn_log/)
#define GPN_LOG_UP_ROM_DIR(name)			GPN_LOG_UP_ROM_NODE#name 

#define GPN_LOG_UP_RAM_NODE					GPN_UNIFY_PATH_RAM_NODE(gpn/gpn_log/)
#define GPN_LOG_UP_RAM_DIR(name)			GPN_LOG_UP_RAM_NODE#name     

/*sys log dirctory start */
#define GPN_LOG_UP_LOG_NODE					GPN_UNIFY_PATH_LOG_NODE()
#define GPN_LOG_UP_LOG_DIR(name)			GPN_LOG_UP_LOG_NODE#name

UINT32 gpnLogUPPathInit(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_LOG_UNIFY_PATH_H_*/

