/**********************************************************
* file name: gpnAlmApi.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-11
* function: 
*    define alarm message process detail
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_MSG_PROC_H_
#define _GPN_ALM_MSG_PROC_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockMsgDef.h"
#include "socketComm/gpnSockAlmMsgDef.h"
#include "socketComm/gpnSockIfmMsgDef.h"
#include "socketComm/gpnDebugFuncApi.h"

#include "type_def.h"
#include "alarm_debug.h"

#include "gpnAlmTrapNotify.h"
#include "gpnAlmTypeStruct.h"
#include "gpnAlmScan.h"


#define GPN_ALM_MP_OK 								GEN_SYS_OK
#define GPN_ALM_MP_ERR 								GEN_SYS_ERR

#define GPN_ALM_MP_CANCEL 							0
#define GPN_ALM_MP_ENABLE 							GEN_SYS_ENABLE
#define GPN_ALM_MP_DISABLE 							GEN_SYS_DISABLE

#define GPN_ALM_MP_YES 								GEN_SYS_YES
#define GPN_ALM_MP_NO 								GEN_SYS_NO

#define GPN_ALM_MP_08_FFFF							GEN_SYS_08_FFFF
#define GPN_ALM_MP_16_FFFF							GEN_SYS_16_FFFF
#define GPN_ALM_MP_32_FFFF							GEN_SYS_32_FFFF
/*#define GPN_ALM_API_64_FFFF						GEN_SYS_64_FFFF*/


#define GPN_ALM_MP_PRINT(level, info...) 			GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_ALM_MP_EGP 								GEN_SYS_DEBUG_EGP
#define GPN_ALM_MP_SVP 								GEN_SYS_DEBUG_SVP
#define GPN_ALM_MP_CMP 								GEN_SYS_DEBUG_CMP
#define GPN_ALM_MP_CUP 								GEN_SYS_DEBUG_CUP
#define GPN_ALM_MP_UD5								GEN_SYS_DEBUG_UD5
#define GPN_ALM_MP_UD4								GEN_SYS_DEBUG_UD4
#define GPN_ALM_MP_UD3								GEN_SYS_DEBUG_UD3
#define GPN_ALM_MP_UD2								GEN_SYS_DEBUG_UD2
#define GPN_ALM_MP_UD1								GEN_SYS_DEBUG_UD1
#define GPN_ALM_MP_UD0								GEN_SYS_DEBUG_UD0

int gpnAlm1SecondTimerPorc(void *para);
UINT32 gpnAlmSockMsgProc(gpnSockMsg *pgpnAlmMsgSp, UINT32 len);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_MSG_PROC_H_*/

