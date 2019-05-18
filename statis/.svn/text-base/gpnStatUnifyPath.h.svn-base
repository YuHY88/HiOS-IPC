/**********************************************************
* file name: gpnStatUnifyPath.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2016-03-08
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_UNIFY_PATH_H_
#define _GPN_STAT_UNIFY_PATH_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "type_def.h"
#include "stat_debug.h"

#include "socketComm/gpnUnifyPathDef.h"
#include "gpnLog/gpnLogFuncApi.h"


#define GPN_STAT_UP_OK 						GEN_SYS_OK
#define GPN_STAT_UP_ERR						GEN_SYS_ERR

#define GPN_STAT_UP_PRINT(level, info...) 	GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_STAT_UP_EGP 					GEN_SYS_DEBUG_EGP
#define GPN_STAT_UP_SVP 					GEN_SYS_DEBUG_SVP
#define GPN_STAT_UP_CMP 					GEN_SYS_DEBUG_CMP
#define GPN_STAT_UP_CUP 					GEN_SYS_DEBUG_CUP
#define GPN_STAT_UP_UD5						GEN_SYS_DEBUG_UD5
#define GPN_STAT_UP_UD4						GEN_SYS_DEBUG_UD4
#define GPN_STAT_UP_UD3						GEN_SYS_DEBUG_UD3
#define GPN_STAT_UP_UD2						GEN_SYS_DEBUG_UD2
#define GPN_STAT_UP_UD1						GEN_SYS_DEBUG_UD1
#define GPN_STAT_UP_UD0						GEN_SYS_DEBUG_UD0

#define GPN_STAT_UP_ROM_NODE				GPN_UNIFY_PATH_ROM_NODE(config/gpn/gpn_stat/)
#define GPN_STAT_UP_ROM_DIR(name)			GPN_STAT_UP_ROM_NODE#name 

#define GPN_STAT_UP_RAM_NODE				GPN_UNIFY_PATH_RAM_NODE(gpn/gpn_stat/)
#define GPN_STAT_UP_RAM_DIR(name)			GPN_STAT_UP_RAM_NODE#name


UINT32 gpnStatUPPathInit(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_STAT_UNIFY_PATH_H_*/

