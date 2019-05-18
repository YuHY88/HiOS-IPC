/**********************************************************
* file name: gpnAlmDebug.h
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2015-05-22
* function: 
*    define alarm Time proc Api
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_DEBUG_H_
#define _GPN_ALM_DEBUG_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>
#include <lib/alarm.h>


/***************************************************************/
/* 按位定义debug信息的类型 */
#define ALARM_DBG_REGISTER	(1 << 0)		//alarm register debug
#define ALARM_DBG_REPORT	(1 << 1)		//alarm report 	 debug
#define ALARM_DBG_GET		(1 << 2)		//snmp get
#define ALARM_DBG_GET_NEXT	(1 << 3)		//snmp get-bulk
#define ALARM_DBG_SET		(1 << 4)		//snmp set
#define ALARM_DBG_SYNC		(1 << 5)		//snmp sync
#define ALARM_DBG_ALL		(ALARM_DBG_REGISTER |\
							 ALARM_DBG_REPORT   |\
							 ALARM_DBG_GET      |\
							 ALARM_DBG_GET_NEXT |\
							 ALARM_DBG_SET      |\
							 ALARM_DBG_SYNC)	    //所有debug


/***************************************************************/


#define GPN_ALM_DEBUG_OK						GEN_SYS_OK
#define GPN_ALM_DEBUG_ERR 						GEN_SYS_ERR

#define GPN_ALM_DEBUG_ENABLE					GEN_SYS_OK
#define GPN_ALM_DEBUG_DISABLE					GEN_SYS_ERR


#define GPN_ALM_DEBUG_PRINT(level, info...) 	GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_ALM_DEBUG_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_ALM_DEBUG_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_ALM_DEBUG_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_ALM_DEBUG_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_ALM_DEBUG_UD5						GEN_SYS_DEBUG_UD5
#define GPN_ALM_DEBUG_UD4						GEN_SYS_DEBUG_UD4
#define GPN_ALM_DEBUG_UD3						GEN_SYS_DEBUG_UD3
#define GPN_ALM_DEBUG_UD2						GEN_SYS_DEBUG_UD2
#define GPN_ALM_DEBUG_UD1						GEN_SYS_DEBUG_UD1
#define GPN_ALM_DEBUG_UD0						GEN_SYS_DEBUG_UD0

/*
#ifdef GPN_ALM_INSERT_DEBUG
#undef GPN_ALM_INSERT_DEBUG
#endif
*/


#ifndef GPN_ALM_INSERT_DEBUG
#define GPN_ALM_INSERT_DEBUG
#endif


void gpnAlmNotifyStateDebug(void);
void gpnAlmEventRiseDebug(void);
void gpnAlmProduct2ClearDebug(void);
UINT32 gpnAlmMakeFakePortIndex(UINT32 portType, UINT32 devIndex, objLogicDesc *plocalPort);
void gpnAlmDebugTickProduct(UINT32 para1, UINT32 para2);
void gpnAlmPeerPortReplaceNotifyDebug(void);
void gpnAlmSFPViewPortReplaceNotifyDebug(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_DEBUG_H_*/
