/**********************************************************
* file name: gpnAlmTimeProc.h
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2015-05-21
* function: 
*    define alarm Time proc Api
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_TIME_PROC_H_
#define _GPN_ALM_TIME_PROC_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>
#include <math.h>


#define GPN_ALM_TIM_OK							GEN_SYS_OK
#define GPN_ALM_TIM_ERR 						GEN_SYS_ERR

#define GPN_ALM_TIM_PRINT(level, info...) 		GEN_SYS_PRINT((level), info)
#define GPN_ALM_TIM_AGP 						GEN_SYS_EGP
#define GPN_ALM_TIM_SVP 						GEN_SYS_SVP
#define GPN_ALM_TIM_CMP 						GEN_SYS_CMP
#define GPN_ALM_TIM_CUP 						GEN_SYS_CUP

#define GPN_ALM_TIM_CLC_SPAN_LIMIT				12
#define GPN_ALM_TIM_SEVERE_CHANG_TOLERATE		12
#define GPN_ALM_TIM_SLIGHT_CHANG_TOLERATE		4

#define GPN_ALM_TIM_CLC_CHG_DELAYED  			10

UINT32 gpnAlmCalendarTimeChangeProc(UINT32 oldTime, UINT32 newTime);
UINT32 gpnAlmTimeAdjust(UINT32 cycSec);
void gpnSockCalendarTimeChgTest(UINT32 p1, UINT32 p2);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_TIME_PROC_H_*/
