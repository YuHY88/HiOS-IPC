/**********************************************************
* file name: gpnAlmTrapNotify.c
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-16
* function: 
*    define alarm Trap method
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_TRAP_NOTIFY_C_
#define _GPN_ALM_TRAP_NOTIFY_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include<sys/ipc.h>
#include<sys/msg.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>

#include "gpnAlmTrapNotify.h"

INT32 gtrapMsgid = -1;
const char trapMsgQ[] = "/home/";

UINT32 gpnAlmTrapMsgQuenInit()
{
	key_t trapKey;
	
	trapKey = ftok(trapMsgQ, 't');
	if(trapKey == -1)
	{
		perror("ftok");
		/*exit(EXIT_FAILURE);*/
		GPN_ALM_TRAP_PRINT(GPN_ALM_TRAP_AGP, "trapKey errno\n\r");
		
		return GPN_ALM_TRAP_ERR;
	}
	gtrapMsgid = msgget(trapKey, 0666|IPC_CREAT);
	if(gtrapMsgid == -1)
	{
		perror("msgget");
		/*exit(EXIT_FAILURE);*/
		GPN_ALM_TRAP_PRINT(GPN_ALM_TRAP_AGP, "gtrapMsgid errno\n\r");
		
		return GPN_ALM_TRAP_ERR;
	}

	GPN_ALM_TRAP_PRINT(GPN_ALM_TRAP_CUP, "gpnAlmTrapMsgQuenInit: Init OK! %d\n\r", gtrapMsgid);
	return GPN_ALM_TRAP_OK;
}
UINT32 gpnAlmTrapMsgQuenSend(UINT32 trapType, stAlmTrapMsgSt *ptrapMsg, UINT32 bLen)
{
	if(bLen > GPN_ALM_TRAP_STRUCT_BLEN/*sizeof(stAlmTrapMsgSt)*/)
	{
		return GPN_ALM_TRAP_ERR;
	}
	
	switch(trapType)
	{
		case GPN_ALM_TRAP_THYE_CURR:	
		case GPN_ALM_TRAP_THYE_HIST:
		case GPN_ALM_TRAP_THYE_EVENT:
		case GPN_ALM_TRAP_THYE_AUTO_R:
			if((msgsnd(gtrapMsgid, (void *)ptrapMsg, bLen, IPC_NOWAIT)) == -1 )
			{
				return GPN_ALM_TRAP_ERR;
			}
			else
			{
				return GPN_ALM_TRAP_OK;
			}
			
		case GPN_ALM_TRAP_THYE_NULL:
		case GPN_ALM_TRAP_THYE_ALL:
			break;
			
		default:
			break;
	}

	return GPN_ALM_TRAP_ERR;	
}
UINT32 gpnAlmTrapMsgQuenReceive(UINT32 trapType, stAlmTrapMsgSt *ptrapMsg)
{
	switch(trapType)
	{
		case GPN_ALM_TRAP_THYE_ALL:
			if((msgrcv(gtrapMsgid, (void *)ptrapMsg, GPN_ALM_TRAP_STRUCT_BLEN,\
				 /*sizeof(stAlmTrapMsgSt)*/ 0, IPC_NOWAIT)) == -1)
			{
				return GPN_ALM_TRAP_ERR;
			}
			else
			{
				return GPN_ALM_TRAP_OK;
			}
		case GPN_ALM_TRAP_THYE_CURR:	
		case GPN_ALM_TRAP_THYE_HIST:
		case GPN_ALM_TRAP_THYE_EVENT:
		case GPN_ALM_TRAP_THYE_AUTO_R:
			if((msgrcv(gtrapMsgid, (void *)ptrapMsg, GPN_ALM_TRAP_STRUCT_BLEN,\
				 /*sizeof(stAlmTrapMsgSt)*/ trapType, IPC_NOWAIT)) == -1)
			{
				return GPN_ALM_TRAP_ERR;
			}
			else
			{
				return GPN_ALM_TRAP_OK;
			}
			
		case GPN_ALM_TRAP_THYE_NULL:
			break;
			
		default:
			break;
	}
	
	return GPN_ALM_TRAP_ERR;
}
UINT32 gpnAlmTrapMsgQuenDelete(void)
{
	if(msgctl(gtrapMsgid, IPC_RMID, 0) == -1)
	{
		perror("msgctl");
		return GPN_ALM_TRAP_ERR;
	}
	
	GPN_ALM_TRAP_PRINT(GPN_ALM_TRAP_CUP, "gpnAlmTrapMsgQuenDelete: Delete OK! %d\n\r", gtrapMsgid);
	return GPN_ALM_TRAP_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_TRAP_NOTIFY_C_*/

