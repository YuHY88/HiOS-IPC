/**********************************************************
* file name: gpnAlmTrapNotify.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-29
* function: 
*    define alarm Trap method
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_TRAP_NOTIFY_H_
#define _GPN_ALM_TRAP_NOTIFY_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "type_def.h"
#include "alarm_debug.h"

#define GPN_ALM_TRAP_OK 						GEN_SYS_OK
#define GPN_ALM_TRAP_ERR 						GEN_SYS_ERR

#define GPN_ALM_TRAP_YES 						GEN_SYS_YES
#define GPN_ALM_TRAP_NO 						GEN_SYS_NO

#define GPN_ALM_TRAP_PRINT(level, info...) 		GEN_SYS_PRINT((level), info)
#define GPN_ALM_TRAP_AGP 						GEN_SYS_EGP
#define GPN_ALM_TRAP_SVP 						GEN_SYS_SVP
#define GPN_ALM_TRAP_CMP 						GEN_SYS_CMP
#define GPN_ALM_TRAP_CUP 						GEN_SYS_CUP

/*trap msg type define*/
#define GPN_ALM_TRAP_THYE_NULL 		0
#define GPN_ALM_TRAP_THYE_CURR 		1
#define GPN_ALM_TRAP_THYE_HIST 		2
#define GPN_ALM_TRAP_THYE_EVENT 	3
#define GPN_ALM_TRAP_THYE_AUTO_R 	4
#define GPN_ALM_TRAP_THYE_ALL 		0xeeeeeeed

#define GPN_ALM_TRAP_PAYL_MAX_BLEN 	64
#define GPN_ALM_TRAP_OVER_HEAD		8
#define GPN_ALM_TRAP_STRUCT_BLEN	(GPN_ALM_TRAP_OVER_HEAD+GPN_ALM_TRAP_PAYL_MAX_BLEN)

typedef struct _stAlmTrapMsgSt_
{
	UINT32 msgType;
	UINT32 msgCtrl;
	UINT32 msgInfo[GPN_ALM_TRAP_PAYL_MAX_BLEN/4];
}stAlmTrapMsgSt;



typedef struct _stAlmProducTrap_
{
	UINT32 order;
	UINT32 almType;
	UINT32 ifIndex;
	UINT32 ifIndex2;
	UINT32 ifIndex3;
	UINT32 ifIndex4;
	UINT32 ifIndex5;
	UINT32 level;
	UINT32 count;
	UINT32 firstTime;
	UINT32 thisTime;
}stAlmProducTrap;

typedef struct _stAlmDisapTrap_
{
	UINT32 order;
	UINT32 almType;
	UINT32 ifIndex;
	UINT32 ifIndex2;
	UINT32 ifIndex3;
	UINT32 ifIndex4;
	UINT32 ifIndex5;
	UINT32 level;
	UINT32 count;
	UINT32 firstTime;
	UINT32 thisTime;
	UINT32 disapTime;
}stAlmDisapTrap;

typedef struct _stEventTrap_
{
	UINT32 order;
	UINT32 eventType;
	UINT32 ifIndex;
	UINT32 ifIndex2;
	UINT32 ifIndex3;
	UINT32 ifIndex4;
	UINT32 ifIndex5;
	UINT32 level;
	UINT32 detial;
}stEventTrap;

typedef struct _stAutoRevsTrap_
{
	UINT32 ifIndex;
	UINT32 almType;
	UINT32 ifIndex2;
	UINT32 ifIndex3;
	UINT32 ifIndex4;
	UINT32 ifIndex5;
}stAutoRevsTrap;

UINT32 gpnAlmTrapMsgQuenInit(void);
UINT32 gpnAlmTrapMsgQuenSend(UINT32 trapType, stAlmTrapMsgSt *ptrapMsg, UINT32 bLen);
UINT32 gpnAlmTrapMsgQuenReceive(UINT32 trapType, stAlmTrapMsgSt *ptrapMsg);
UINT32 gpnAlmTrapMsgQuenDelete(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_TRAP_NOTIFY_H_*/


