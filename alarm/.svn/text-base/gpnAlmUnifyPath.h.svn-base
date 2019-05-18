/**********************************************************
* file name: gpnAlmUnifyPath.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-06-30
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_UNIFY_PATH_H_
#define _GPN_ALM_UNIFY_PATH_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "type_def.h"
#include "alarm_debug.h"

#include "socketComm/gpnUnifyPathDef.h"
#include "gpnLog/gpnLogFuncApi.h"


#define GPN_ALM_UP_OK 						GEN_SYS_OK
#define GPN_ALM_UP_ERR						GEN_SYS_ERR

#define GPN_ALM_UP_PRINT(level, info...) 	GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_ALM_UP_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_ALM_UP_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_ALM_UP_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_ALM_UP_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_ALM_UP_UD5						GEN_SYS_DEBUG_UD5
#define GPN_ALM_UP_UD4						GEN_SYS_DEBUG_UD4
#define GPN_ALM_UP_UD3						GEN_SYS_DEBUG_UD3
#define GPN_ALM_UP_UD2						GEN_SYS_DEBUG_UD2
#define GPN_ALM_UP_UD1						GEN_SYS_DEBUG_UD1
#define GPN_ALM_UP_UD0						GEN_SYS_DEBUG_UD0

#define GPN_ALM_UP_ROM_NODE					GPN_UNIFY_PATH_ROM_NODE(config/gpn/gpn_alarm/)
#define GPN_ALM_UP_ROM_DIR(name)			GPN_ALM_UP_ROM_NODE#name 

#define GPN_ALM_UP_RAM_NODE					GPN_UNIFY_PATH_RAM_NODE(gpn/gpn_alarm/)
#define GPN_ALM_UP_RAM_DIR(name)			GPN_ALM_UP_RAM_NODE#name     

UINT32 gpnAlmUPPathInit(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_UNIFY_PATH_H_*/

