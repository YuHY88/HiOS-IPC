/**********************************************************
* file name: gpnStatTimeProc.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-21
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_TIME_PROC_H_
#define _GPN_STAT_TIME_PROC_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>

#include "type_def.h"
#include "stat_debug.h"

#define GPN_STAT_TIME_GEN_OK 					GEN_SYS_OK
#define GPN_STAT_TIME_GEN_ERR 					GEN_SYS_ERR

#define GPN_STAT_TIME_GEN_YES 					GEN_SYS_YES
#define GPN_STAT_TIME_GEN_NO					GEN_SYS_NO

#define GPN_STAT_TIME_PRINT(level, info...) 	GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_STAT_TIME_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_STAT_TIME_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_STAT_TIME_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_STAT_TIME_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_STAT_TIME_UD5						GEN_SYS_DEBUG_UD5
#define GPN_STAT_TIME_UD4						GEN_SYS_DEBUG_UD4
#define GPN_STAT_TIME_UD3						GEN_SYS_DEBUG_UD3
#define GPN_STAT_TIME_UD2						GEN_SYS_DEBUG_UD2
#define GPN_STAT_TIME_UD1						GEN_SYS_DEBUG_UD1
#define GPN_STAT_TIME_UD0						GEN_SYS_DEBUG_UD0

#define GPN_STAT_SYS_TIME_SEVERE_CHANG_TOLERATE	5
#define GPN_STAT_SYS_TIME_SLIGHT_CHANG_TOLERATE	2

#define GPN_STAT_PUSH_STACK_DELAY				1

/*********************************************************************/
/*                          function per-define in gpnStatDatastruct.h                                   */
/*********************************************************************/

UINT32 gpnStatTimeAdjust(UINT32 cycSec);
UINT32 gpnStatHistPushBingoTimeInit(UINT32 sTime, UINT32 cycSecs, UINT32 *prelSTime, UINT32 *ppushTime);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_STAT_TIME_PROC_H_*/


