/**********************************************************
* file name: sockAliveMan.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-08-14
* function: 
*    define socket comm details
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_ALIVE_MAN_H_
#define _GPN_SOCK_ALIVE_MAN_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"

#define GPN_ALIVE_MAN_GEN_OK 				GPN_SOCK_SYS_OK
#define GPN_ALIVE_MAN_GEN_ERR 				GPN_SOCK_SYS_ERR

#define GPN_ALIVE_MAN_GEN_ENABLE 			GPN_SOCK_SYS_ENABLE
#define GPN_ALIVE_MAN_GEN_DISABLE			GPN_SOCK_SYS_DISABLE

#define GPN_ALIVE_MAN_PRINT(level, info...) GEN_SOCK_SYS_PRINT((level), info)
#define GPN_ALIVE_MAN_AGP 					GEN_SOCK_SYS_AGP
#define GPN_ALIVE_MAN_SVP 					GEN_SOCK_SYS_SVP
#define GPN_ALIVE_MAN_CMP 					GEN_SOCK_SYS_CMP
#define GPN_ALIVE_MAN_CUP 					GEN_SOCK_SYS_CUP

#define ALIVE_MAN_FDSET_MAX					64

#define ALIVE_MAN_TX_DEF_TXTOLE				5
#define ALIVE_MAN_TX_FREQ_1S				1
#define ALIVE_MAN_TX_FREQ_2S				2
#define ALIVE_MAN_TX_FREQ_10S				10

#define ALIVE_MAN_MAX_RX_MIS				3
typedef struct _sockAliveInfo_
{
	UINT32 en;

	UINT32 sta;

	/*cfg*/
	UINT32 txHolFreq;
	UINT32 toleratMis;
	/*sta*/
	UINT32 rxHolFreq;
	UINT32 misCnt;
	UINT32 rxTimerCnt;
	/*tx hello sta*/
	UINT32 txFlag;
	UINT32 txTimerCnt;
	
}stAliveInfo;


UINT32 gpnAliveManUintInit(stAliveInfo *pstAliveMan);
UINT32 gpnAliveManUintSecondProc(stAliveInfo *pstAliveMan);
UINT32 gpnAliveManRxHello(stAliveInfo *pstAliveMan);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_SOCK_ALIVE_MAN_H_*/

