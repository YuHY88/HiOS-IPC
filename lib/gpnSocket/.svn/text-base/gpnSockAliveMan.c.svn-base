/**********************************************************
* file name: sockAliveMan.c
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-08-14
* function: 
*    define socket comm details
* modify:
*
***********************************************************/
#ifndef _GPN_SOCKET_ALIVE_MAN_C_
#define _GPN_SOCKET_ALIVE_MAN_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include  "socketComm/gpnSockAliveMan.h"

UINT32 gpnAliveManUintInit(stAliveInfo *pstAliveMan)
{
	UINT32 fakeRandom;
	
	fakeRandom = 0;
	pstAliveMan->en = GPN_ALIVE_MAN_GEN_ENABLE;

	pstAliveMan->sta = GPN_ALIVE_MAN_GEN_OK;
	
	pstAliveMan->txFlag = GPN_ALIVE_MAN_GEN_ENABLE;
	pstAliveMan->txHolFreq = ALIVE_MAN_TX_FREQ_10S;
	pstAliveMan->txTimerCnt = fakeRandom++;
	
	pstAliveMan->toleratMis = ALIVE_MAN_MAX_RX_MIS;
	pstAliveMan->rxHolFreq = ALIVE_MAN_TX_FREQ_10S;
	pstAliveMan->rxTimerCnt = 0;
	pstAliveMan->misCnt = 0;

	return GPN_ALIVE_MAN_GEN_OK;
}

UINT32 gpnAliveManUintSecondProc(stAliveInfo *pstAliveMan)
{
	
	if(pstAliveMan->en == GPN_ALIVE_MAN_GEN_ENABLE)
	{
		/*tx alive enable*/
		pstAliveMan->txTimerCnt++;
		if(pstAliveMan->txTimerCnt >= pstAliveMan->txHolFreq)
		{
			pstAliveMan->txFlag = GPN_ALIVE_MAN_GEN_ENABLE;
			pstAliveMan->txTimerCnt = 0;
		}

		/*rx alive check*/
		pstAliveMan->rxTimerCnt++;
		pstAliveMan->misCnt = (pstAliveMan->rxTimerCnt - 1) / pstAliveMan->rxHolFreq;
		
		if(pstAliveMan->misCnt > pstAliveMan->toleratMis)
		{
			pstAliveMan->sta = GPN_ALIVE_MAN_GEN_ERR;
			return GPN_ALIVE_MAN_GEN_ERR;
		}
		else
		{
			return GPN_ALIVE_MAN_GEN_OK;
		}
	}
	else
	{
		return GPN_ALIVE_MAN_GEN_ERR;
	}
	
}
UINT32 gpnAliveManRxHello(stAliveInfo *pstAliveMan)
{	
	if(pstAliveMan->en == GPN_ALIVE_MAN_GEN_ENABLE)
	{
		pstAliveMan->sta = GPN_ALIVE_MAN_GEN_OK;
		pstAliveMan->rxTimerCnt = 0;
		pstAliveMan->misCnt = 0;
	}

	return GPN_ALIVE_MAN_GEN_OK;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_SOCKET_ALIVE_MAN_C_ */
