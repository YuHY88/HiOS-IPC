/**********************************************************
* file name: gpnSelectOpt.c
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-08-16
* function: 
*    define select opt about
* modify:
*
***********************************************************/
#ifndef _GPN_SELECT_OPT_C_
#define _GPN_SELECT_OPT_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "socketComm/gpnSockCommModuDef.h"
#include "socketComm/gpnSockCommRoleMan.h"
#include "socketComm/gpnSocketMan.h"
#include "socketComm/gpnTimerService.h"
#include "socketComm/gpnSockAliveMan.h"
#include "socketComm/gpnSelectOpt.h"
#include "socketComm/gpnSockProtocolMsgDef.h"


UINT32 gpnSockSelectOptGlobInit(stSockFdSet *pstFdSet)
{
	UINT32 i;
	UINT32 j;
	stFdTxRec *pstFdTx;
	stFdRxRec *pstFdRx;
	
	memset((void*)pstFdSet, 0, sizeof(stSockFdSet));

	pstFdSet->fdRealNum = 0;
	pstFdSet->fdMaxSearchNum = 0;
	pstFdSet->fdMaxSpace = GPN_SOCK_FDSET_MAX;
	pstFdSet->maxFd = 0;
	pstFdSet->moduId = GPN_COMMM_NULL;
	pstFdSet->CPUId = GPN_COMMM_NULL;
	pstFdSet->localSrc = GPN_COMMM_NULL;
	pstFdSet->NMaCPUId = GPN_COMMM_NULL;
	pstFdSet->NMbCPUId = GPN_COMMM_NULL;

	for(i=0; i<GPN_SOCK_FDSET_MAX; i++)
	{
		/*real fd set */
		pstFdSet->pfdOptInfo[i] =  NULL;
		
		pstFdSet->fdOptInfo[i].fd = -1;
		pstFdSet->fdOptInfo[i].localDst = GPN_COMMM_NULL;
		pstFdSet->fdOptInfo[i].en = GPN_SELECT_GEN_DISABLE;
		
		pstFdSet->fdOptInfo[i].fdTxRxRec.txClcCheck = GPN_SELECT_GEN_NO;
		pstFdSet->fdOptInfo[i].fdTxRxRec.rxClcCheck = GPN_SELECT_GEN_NO;
		pstFdSet->fdOptInfo[i].fdTxRxRec.txLosNum = 0;
		
		pstFdSet->fdOptInfo[i].fdTxRxRec.txHash = GPN_SOCK_FD_TX_MAX;
		pstFdSet->fdOptInfo[i].fdTxRxRec.lastTx = 0;
		pstFdTx = pstFdSet->fdOptInfo[i].fdTxRxRec.stFdTx;
		for(j=0;j<GPN_SOCK_FD_TX_MAX;j++)
		{
			pstFdTx[j].FifoSeat = GPN_SELECT_GEN_FILLED;
			pstFdTx[j].reSendCnt = 0;
			pstFdTx[j].isTxOk = GPN_SELECT_GEN_YES;
			pstFdTx[j].waitCount = 0;
			pstFdTx[j].isAckRcv = GPN_SELECT_GEN_YES;
		}
		pstFdSet->fdOptInfo[i].fdTxRxRec.reTxNum = GPN_SOCK_FD_RETX_MAX;
		pstFdSet->fdOptInfo[i].fdTxRxRec.waitAckP = 0;
		pstFdSet->fdOptInfo[i].fdTxRxRec.nowTxP = 0;

		pstFdSet->fdOptInfo[i].fdTxRxRec.rxHash = GPN_SOCK_FD_RX_MAX;
		pstFdSet->fdOptInfo[i].fdTxRxRec.lastRx = 0;
		pstFdRx = pstFdSet->fdOptInfo[i].fdTxRxRec.stFdRx;
		for(j=0;j<GPN_SOCK_FD_RX_MAX;j++)
		{
			pstFdRx[j].isAckSend = GPN_SELECT_GEN_NO;
			pstFdRx[i].reSendCnt = 0;
		}

		pstFdSet->fdOptInfo[i].fdOpt.rdEn = GPN_SELECT_GEN_NO;
		pstFdSet->fdOptInfo[i].fdOpt.wtEn = GPN_SELECT_GEN_NO;
		pstFdSet->fdOptInfo[i].fdOpt.exEn = GPN_SELECT_GEN_NO;

		gpnAliveManUintInit(&(pstFdSet->fdOptInfo[i].fdAlive));
	}

	gpnSockFdSetCommFifoInit(&(pstFdSet->txMsgFifo));
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockSelectFdtoAlineFdSet(stSockFdSet *pstFdSet, stGpnFdOptInfo *pfdOptInfo)
{
	UINT32 i;
	
	/*assert*/
	if( (pstFdSet == NULL) ||\
 		(pfdOptInfo == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	for(i=0;i<pstFdSet->fdRealNum;i++)
	{
		if(pstFdSet->pfdOptInfo[i] == NULL)
		{
			/*fatal err*/
			/*printf("");*/
			return GPN_SELECT_GEN_ERR;
		}

		if(pstFdSet->pfdOptInfo[i]->fd == pfdOptInfo->fd)
		{
			/*re-add to real fd set */
			/*printf("");*/
			return GPN_SELECT_GEN_ERR;
		}
	}

	if(pstFdSet->fdRealNum >= GPN_SOCK_FDSET_MAX)
	{
		/* real fd set full, add err */
		/*printf("");*/
		return GPN_SELECT_GEN_ERR;
	}

	pstFdSet->pfdOptInfo[pstFdSet->fdRealNum] = pfdOptInfo;
	pstFdSet->fdRealNum++;
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSelectFdtoFdSet(stSockFdSet *pstFdSet, INT32 fd, UINT32 fdSeat, stGpnFdAddr *pstFdAddr)
{
	UINT32 i;
	stGpnFdOptInfo *pfdOptInfo;
	stGpnFdTxRxRec *pfdTxRxRec;
	stGpnFdAddr *pfdAddr;
	stFdOptMod *pfdOpt;
	stAliveInfo *pstAliveMan;
	stFdTxRec *pstFdTx;
	stFdRxRec *pstFdRx;
	
	/*assert*/
	if( (pstFdSet == NULL) ||\
		(fd == -1) ||\
		(fdSeat > GPN_SOCK_ALL_ROLE_ID_MAX) ||\
 		(pstFdAddr == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}


	/*used for selecet's max search*/
	if((fdSeat+1) > pstFdSet->fdMaxSearchNum)
	{
		pstFdSet->fdMaxSearchNum = fdSeat+1;
	}
	
	/*get Max fd for selecet*/
	if(fd > pstFdSet->maxFd)
	{
		pstFdSet->maxFd = fd;
	}

	memset((void *)&(pstFdSet->fdOptInfo[fdSeat]),\
		0, sizeof(stGpnFdOptInfo));
	
	pstFdSet->fdOptInfo[fdSeat].fd = fd;
	/*when tx msg, creat right now */
	/*pstFdSet->fdOptInfo[fdSeat].localDst = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, moduId);*/
	pstFdSet->fdOptInfo[fdSeat].en = GPN_SELECT_GEN_ENABLE;
	
	pfdTxRxRec = &(pstFdSet->fdOptInfo[fdSeat].fdTxRxRec);
	pfdAddr = &(pstFdSet->fdOptInfo[fdSeat].fdAddr);
	pfdOpt = &(pstFdSet->fdOptInfo[fdSeat].fdOpt);
	pstAliveMan = &(pstFdSet->fdOptInfo[fdSeat].fdAlive);
	pstFdTx = pstFdSet->fdOptInfo[fdSeat].fdTxRxRec.stFdTx;
	pstFdRx = pstFdSet->fdOptInfo[fdSeat].fdTxRxRec.stFdRx;
	
	pfdTxRxRec->txClcCheck = GPN_SELECT_GEN_NO;
	pfdTxRxRec->rxClcCheck = GPN_SELECT_GEN_NO;

	pfdTxRxRec->txLosNum = 0;
	pfdTxRxRec->rxAckLos = 0;
	
	pfdTxRxRec->txHash = GPN_SOCK_FD_TX_MAX;
	pfdTxRxRec->lastTx = 0;
	for(i=0;i<GPN_SOCK_FD_TX_MAX;i++)
	{
		pstFdTx[i].FifoSeat = GPN_SELECT_GEN_FILLED;
		pstFdTx[i].reSendCnt = 0;
		pstFdTx[i].isTxOk = GPN_SELECT_GEN_YES;
		pstFdTx[i].waitCount = 0;
		pstFdTx[i].isAckRcv = GPN_SELECT_GEN_YES;
	}
	pfdTxRxRec->reTxNum = GPN_SOCK_FD_RETX_MAX;
	pfdTxRxRec->waitAckP = 0;
	pfdTxRxRec->nowTxP = 0;
	pfdTxRxRec->txLimitP = GPN_SOCK_FD_RETX_MAX;

	pfdTxRxRec->rxHash = GPN_SOCK_FD_RX_MAX;
	pfdTxRxRec->lastRx = 0;
	for(i=0;i<GPN_SOCK_FD_RX_MAX;i++)
	{
		pstFdRx[i].isAckSend = GPN_SELECT_GEN_NO;
		pstFdRx[i].reSendCnt = 0;
	}
	
	memcpy((void *)pfdAddr, (void *)pstFdAddr, sizeof(stGpnFdAddr));

	pfdOpt->rdEn = GPN_SELECT_GEN_YES;
	pfdOpt->wtEn = GPN_SELECT_GEN_NO;
	pfdOpt->exEn = GPN_SELECT_GEN_YES;

	/*add fd to real fd*/
	pfdOptInfo = &(pstFdSet->fdOptInfo[fdSeat]);
	gpnSockSelectFdtoAlineFdSet(pstFdSet, pfdOptInfo);
	
	gpnAliveManUintInit(pstAliveMan);
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCommSelectMainProc(stSockFdSet *pstFdSet,struct timeval *timeout)
{
	INT32 reVal;
	UINT32 i;
	stFdOptMod *pfdOpt;
	
	fd_set fdreads;
	fd_set fdwrites;
	fd_set fdexcepts;
	
	/*FdSet clear*/
	FD_ZERO(&fdreads);
   	FD_ZERO(&fdwrites);
    FD_ZERO(&fdexcepts);

#if 0
	for(i=0; i<pstFdSet->fdMaxSearchNum; i++)
	{
		if(pstFdSet->fdOptInfo[i].en == GPN_SELECT_GEN_DISABLE)
		{
			continue;
		}
		pfdOpt = &(pstFdSet->fdOptInfo[i].fdOpt);
		
		if(pfdOpt->rdEn == GPN_SELECT_GEN_YES)
		{
			FD_SET(pstFdSet->fdOptInfo[i].fd, &fdreads);
		}
		if(pfdOpt->wtEn == GPN_SELECT_GEN_YES)
		{
			FD_SET(pstFdSet->fdOptInfo[i].fd, &fdwrites);
		}
		if(pfdOpt->exEn == GPN_SELECT_GEN_YES)
		{
			FD_SET(pstFdSet->fdOptInfo[i].fd, &fdexcepts);
		}
	}
#else
	for(i=0; i<pstFdSet->fdRealNum; i++)
		{
			if(pstFdSet->pfdOptInfo[i]->en == GPN_SELECT_GEN_DISABLE)
			{
				continue;
			}
			pfdOpt = &(pstFdSet->pfdOptInfo[i]->fdOpt);
			if(pfdOpt->rdEn == GPN_SELECT_GEN_YES)
			{
				FD_SET(pstFdSet->pfdOptInfo[i]->fd, &fdreads);
			}
			if(pfdOpt->wtEn == GPN_SELECT_GEN_YES)
			{
				FD_SET(pstFdSet->pfdOptInfo[i]->fd, &fdwrites);
			}
			if(pfdOpt->exEn == GPN_SELECT_GEN_YES)
			{
				FD_SET(pstFdSet->pfdOptInfo[i]->fd, &fdexcepts);
			}
		}
#endif
	/*select:IS BLOCK or NOT*/
	reVal = select(pstFdSet->maxFd+1, &fdreads, &fdwrites, &fdexcepts, timeout);
	switch(reVal)
	{
		case -1:
			/*select err*/
		  	/*perror("select err");*/
		  	break;
			
		case 0:
			/*time out*/
			/*perror("select time_out err");*/
			break;
			
		default:
			/*read write except*/
#if 0
			for(i=0; i<pstFdSet->fdMaxSearchNum; i++)
			{
				if(pstFdSet->fdOptInfo[i].en == GPN_SELECT_GEN_DISABLE)
				{
					continue;
				}
				pfdOpt = &(pstFdSet->fdOptInfo[i].fdOpt);
				if(pfdOpt->rdEn == GPN_SELECT_GEN_YES)
				{
					if(FD_ISSET(pstFdSet->fdOptInfo[i].fd, &fdreads))
					{
						/*read socket*/
						gpnSockFdReadProc(pstFdSet, &(pstFdSet->fdOptInfo[i]));
					}
				}
				if(pfdOpt->wtEn == GPN_SELECT_GEN_YES)
				{
					if(FD_ISSET(pstFdSet->fdOptInfo[i].fd, &fdwrites))
					{
						/*write socket*/
						gpnSockWriteEnProc(pstFdSet, &(pstFdSet->fdOptInfo[i]));
					}
				}
				if(pfdOpt->exEn == GPN_SELECT_GEN_YES)
				{
					if(FD_ISSET(pstFdSet->fdOptInfo[i].fd, &fdexcepts))
					{
						/*socket excepts*/
						/*???*/
						GPN_SELECT_PRINT(GPN_SELECT_AGP, "fd expresion\n\r");
					}
				}
			}
#else
			for(i=0; i<pstFdSet->fdRealNum; i++)
			{
				if(pstFdSet->pfdOptInfo[i]->en == GPN_SELECT_GEN_DISABLE)
				{
					continue;
				}
				pfdOpt = &(pstFdSet->pfdOptInfo[i]->fdOpt);
				if(pfdOpt->rdEn == GPN_SELECT_GEN_YES)
				{
					if(FD_ISSET(pstFdSet->pfdOptInfo[i]->fd, &fdreads))
					{
						/*read socket*/
						gpnSockFdReadProc(pstFdSet, pstFdSet->pfdOptInfo[i]);
					}
				}
				if(pfdOpt->wtEn == GPN_SELECT_GEN_YES)
				{
					if(FD_ISSET(pstFdSet->pfdOptInfo[i]->fd, &fdwrites))
					{
						/*write socket*/
						gpnSockWriteEnProc(pstFdSet, pstFdSet->pfdOptInfo[i]);
					}
				}
				if(pfdOpt->exEn == GPN_SELECT_GEN_YES)
				{
					if(FD_ISSET(pstFdSet->pfdOptInfo[i]->fd, &fdexcepts))
					{
						/*socket excepts*/
						/*???*/
						GPN_SELECT_PRINT(GPN_SELECT_AGP, "fd expresion\n\r");
					}
				}
			}
#endif
			return GPN_SELECT_GEN_OK;
	}

	return GPN_SELECT_GEN_ERR;
}

UINT32 gpnSockFdReadProc(stSockFdSet *pstFdSet, stGpnFdOptInfo *pstFdOptInfo)
{
	INT32 rxByte;
	UINT32 partLen;
	UINT32 rxBuff[GPN_SOCK_RX_BUFF_DWORD_SIZE];
	/*sockaddr-16,sockaddr_in-16,sockaddr_un-110*/
	struct sockaddr_un farend;

	rxByte = 0;
	/*partLen = sizeof(farend);*/
	
	if(pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_UDP)
	{
		/*UDP*/
		
		partLen = GPN_SOCK_ADDR_UN_LEN;
		rxByte = recvfrom(pstFdOptInfo->fd,\
			(void *)rxBuff,GPN_SOCK_RX_BUFF_BYTE_SIZE, 0,\
			(struct sockaddr *)&farend, (socklen_t *)&partLen);
	}
	else
	{
		/*TCP*/
		partLen = 16;
		/*rxByte = read()*/
	}
		
	if(rxByte > 0)
	{
		/*just for test printf*/
		if(pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_UNIX)
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : rxByte(%d) form fd:%08d selfAddr %s defConn %s newConn %s farend %s farAddlen %d\n\r",\
				__FUNCTION__, getpid(), rxByte, pstFdOptInfo->fd,\
				pstFdOptInfo->fdAddr.selfAddr.sun_path,\
				pstFdOptInfo->fdAddr.defConnAddr.sun_path,\
				pstFdOptInfo->fdAddr.newConnAddr.sun_path,\
				farend.sun_path,partLen);
		}
		else if(pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_INET)
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : rxByte(%d) form fd:%08d selfAddr %08x-%d defConn %08x-%d newConn %08x-%d farend %08x-%d farAddlen %d\n\r",\
				__FUNCTION__, getpid(), rxByte, pstFdOptInfo->fd,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.selfAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.selfAddr))->sin_port,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.defConnAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.defConnAddr))->sin_port,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.newConnAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.newConnAddr))->sin_port,\
				((struct sockaddr_in *)&(farend))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(farend))->sin_port,\
				partLen);
		}
		#if 0
		do
		{
			/*msg decompose*/
			msgBuff = gpnSockMsgDecompose(rxBuff, &msgLen);
			gpnSockMsgRxProc(pstFdSet, pstFdOptInfo, partLen, msgBuff, msgLen);
		}
		while(msgBuff)
		#endif

		gpnSockMsgRxProc(pstFdSet, pstFdOptInfo,\
			(struct sockaddr *)(&farend), partLen, rxBuff, rxByte);
		
	}
	return GPN_SELECT_GEN_OK;
}

UINT32 *gpnSockMsgDecompose(UINT32 *rxBuff, UINT32 *pmsgLen)
{
	UINT32 *msgBuff;
	
	msgBuff = rxBuff;
	*pmsgLen = GPN_SOCK_MSG_HEAD_BLEN;
	
	return msgBuff;
}

UINT32 gpnSockMsgRxProc(stSockFdSet *pstFdSet, stGpnFdOptInfo *pstFdOptInfo,
							struct sockaddr *farend, UINT32 addLen, UINT32 *msgBuff, UINT32 msgLen)
{
	UINT32 msgType;
	UINT32 procEn;
	stGpnFdOptInfo *pstFdRealOpt;

	/*arssert */
	if( (pstFdSet ==NULL) ||
		(pstFdOptInfo == NULL) ||
		(farend == NULL) ||
		(msgBuff == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	msgType = msgBuff[GPN_SOCK_MSG_SUFFIX_CMD];
	GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d): index %d CTRL %08x DST %08x SRC %08x CMD %08x\n\r",\
		__FUNCTION__, getpid(),\
		msgBuff[GPN_SOCK_MSG_SUFFIX_INDEX], msgBuff[GPN_SOCK_MSG_SUFFIX_CTRL],\
		msgBuff[GPN_SOCK_MSG_SUFFIX_DST],msgBuff[GPN_SOCK_MSG_SUFFIX_SRC],\
		msgBuff[GPN_SOCK_MSG_SUFFIX_CMD]);
	if(msgType & GPN_SOCK_MSG_ACK_BIT)
	{
		/*RX ACK process*/
		gpnSockACKRxProc(pstFdSet, &(pstFdOptInfo->fdTxRxRec), msgBuff);
	}
	else
	{
		pstFdRealOpt = NULL;
		if(pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_ALONE)
		{
			/*do not tx ACK, do not check src/dst*/
			procEn = GPN_SELECT_GEN_OK;
		}
		else
		{
			if(pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_UNIX)
			{
				GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d): PerProc:fdRole %08x selfAddr %s defConnAddr %s newConnAddr %s\n\r",\
					__FUNCTION__, getpid(),\
					pstFdOptInfo->fdAddr.fdRole,\
					pstFdOptInfo->fdAddr.selfAddr.sun_path,\
					pstFdOptInfo->fdAddr.defConnAddr.sun_path,\
					pstFdOptInfo->fdAddr.newConnAddr.sun_path);
			}
			else if(pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_INET)
			{
				GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d): PerProc:fdRole %08x selfAddr %08x-%d defConnAddr %08x-%d newConnAddr %08x-%d\n\r",\
					__FUNCTION__, getpid(),\
					pstFdOptInfo->fdAddr.fdRole,
					((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.selfAddr))->sin_addr.s_addr,\
					((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.selfAddr))->sin_port,\
					((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.defConnAddr))->sin_addr.s_addr,\
					((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.defConnAddr))->sin_port,\
					((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.newConnAddr))->sin_addr.s_addr,\
					((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.newConnAddr))->sin_port);
			}
			
			if((pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_OLD)\
										   == GPN_SOCKET_ROLE_OLD)
			{
				/*msg to old server: intercurrent new server to keep connect*/
				pstFdRealOpt = gpnSockNewConnect(pstFdSet,\
									msgBuff[GPN_SOCK_MSG_SUFFIX_SRC], farend, addLen);
			}
			else
			{
				pstFdRealOpt = pstFdOptInfo;
			}
			
			/*TX ACK process*/
			procEn = gpnSockMsgRxACKTxProc(pstFdRealOpt, msgBuff);

		}

		if(procEn == GPN_SELECT_GEN_OK)
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d): PerProc:msgType %08x\n\r",\
				__FUNCTION__, getpid(), msgType);
			switch(msgType)
			{
				/*proc comm-protocol-self*/
				case GPN_SELF_MSG_COMM_CHECK:
					if(pstFdRealOpt != NULL)
					{
						gpnAliveManRxHello(&(pstFdRealOpt->fdAlive));
					}
					break;
					
				case GPN_SELF_MSG_COMM_CHECK_RSP:
					if(pstFdRealOpt != NULL)
					{
						gpnAliveManRxHello(&(pstFdRealOpt->fdAlive));
					}
					break;
					
				case GPN_SELF_MSG_NEW_CONNCET_NOTIFY:
					if(pstFdRealOpt != NULL)
					{
						gpnSockSpMsgProcNewConnNotifyPartner(pstFdRealOpt, (gpnSockMsg *)msgBuff, msgLen);
					}
					break;

				case GPN_GEN_MSG_TIMER_BASE:
					gpnBaseTimerBingo();
					break;

				case GPN_SELF_MSG_BASE_TIMER_SYN_OPT:
					GPN_SELECT_PRINT(GPN_SELECT_CUP,  "%s(%d) : rcv GPN_SELF_MSG_BASE_TIMER_SYN_OPT!\n\r",\
						__FUNCTION__, getpid());
					gpnTimerAdjustBaseTime(msgBuff[GPN_SOCK_MSG_SUFFIX_P1], msgBuff[GPN_SOCK_MSG_SUFFIX_P2]);
					break;
					
				/*proc timer*/
				case GPN_GEN_MSG_TIMER_1S:
					/*guarantee or not:not guarantee*/
					/*gpnSockSpMsgProc1sTiming(pstFdSet, GPN_SELECT_1SECOND);*/
					/*pay attention to here: do not have 'break'*/
				
				default:
					if(pstFdSet->gpnSockUsrMsgProc != NULL)
					{
						/*usr messages process*/
						GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : gpnSockUsrMsgProc:type %08x\n\r",\
							__FUNCTION__, getpid(), msgType);
						pstFdSet->gpnSockUsrMsgProc((gpnSockMsg *)msgBuff, msgLen);
					}
					break;
			}
		}
	}
	return GPN_SELECT_GEN_OK;
}

stGpnFdOptInfo *gpnSockNewConnect(stSockFdSet *pstFdSet, UINT32 msgSrc, struct sockaddr *pfarend, UINT32 addLen)
{
	UINT32 cmpVal;
	UINT32 fdSeat;
	UINT32 selfFdSeat;
	stGpnFdOptInfo *pstFdRealOpt;
	stGpnFdOptInfo *pstSelfFdRealOpt;
	
	/*assert*/
	if( (pstFdSet ==NULL) ||
		(pfarend == NULL) )
	{
		pstFdRealOpt = NULL;
		return NULL;
	}
	
	pstFdRealOpt = NULL;
	/*get fdSeat from iSrcId*/
	if(gpnSockCommMsgDst2RoleSeat(pstFdSet, msgSrc, &fdSeat) != GPN_SELECT_GEN_OK)
	{
		GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s(%d) : get fdSeat err local cpuId(%d) srcId(%08x)!\n\r",\
			__FUNCTION__, getpid(), pstFdSet->CPUId, msgSrc);
		
		return pstFdRealOpt;
	}
	if(gpnSockCommMsgDst2RoleSeat(pstFdSet, pstFdSet->localSrc, &selfFdSeat) != GPN_SELECT_GEN_OK)
	{
		GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s(%d) : get fdSeat err local cpuId(%d) srcId(%08x)!\n\r",\
			__FUNCTION__, getpid(), pstFdSet->CPUId, msgSrc);
		
		return pstFdRealOpt;
	}
	if( (fdSeat >= pstFdSet->fdMaxSpace) ||\
		(selfFdSeat >= pstFdSet->fdMaxSpace) )
	{
		GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s(%d) : fdSeat err (%d %d), srcId(%08x) localSrc(%08x)!\n\r",\
			__FUNCTION__, getpid(), fdSeat, selfFdSeat, msgSrc, pstFdSet->localSrc);
		return pstFdRealOpt;
	}

	pstFdRealOpt = &(pstFdSet->fdOptInfo[fdSeat]);
	pstSelfFdRealOpt = &(pstFdSet->fdOptInfo[selfFdSeat]);

	if(pstFdRealOpt->en == GPN_SELECT_GEN_DISABLE)
	{
		GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s(%d) : coonecter disable (%d %d), srcId(%08x) localSrc(%08x)!\n\r",\
			__FUNCTION__, getpid(), fdSeat, selfFdSeat, msgSrc, pstFdSet->localSrc);
		return pstFdRealOpt;
	}
	
	if(pstFdRealOpt->fdAddr.fdRole & GPN_SOCKET_ROLE_UNIX)
	{
		GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : befor NewConnect, before memcmp: role %08x self %s defConnAddr %s newConnAddr %s partner %s partAddrLen %d \n\r",\
		__FUNCTION__, getpid(),\
		pstFdRealOpt->fdAddr.fdRole,\
		pstFdRealOpt->fdAddr.selfAddr.sun_path,\
		pstFdRealOpt->fdAddr.defConnAddr.sun_path,\
		pstFdRealOpt->fdAddr.newConnAddr.sun_path,\
		((struct sockaddr_un *)pfarend)->sun_path,\
		addLen);
	}
	else if(pstFdRealOpt->fdAddr.fdRole & GPN_SOCKET_ROLE_INET)
	{
		GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : befor NewConnect, before memcmp: role %08x self %08x-%d defConnAddr %08x-%d newConnAddr %08x-%d partner %08x-%d partAddrLen %d \n\r",\
			__FUNCTION__, getpid(),\
			pstFdRealOpt->fdAddr.fdRole,\
			((struct sockaddr_in *)&(pstFdRealOpt->fdAddr.selfAddr))->sin_addr.s_addr,\
			((struct sockaddr_in *)&(pstFdRealOpt->fdAddr.selfAddr))->sin_port,\
			((struct sockaddr_in *)&(pstFdRealOpt->fdAddr.defConnAddr))->sin_addr.s_addr,\
			((struct sockaddr_in *)&(pstFdRealOpt->fdAddr.defConnAddr))->sin_port,\
			((struct sockaddr_in *)&(pstFdRealOpt->fdAddr.newConnAddr))->sin_addr.s_addr,\
			((struct sockaddr_in *)&(pstFdRealOpt->fdAddr.newConnAddr))->sin_port,\
			((struct sockaddr_in *)(pfarend))->sin_addr.s_addr,\
			((struct sockaddr_in *)(pfarend))->sin_port,\
			addLen);
	}

	/*find new connect addr */
	cmpVal = memcmp((void *)(&(pstSelfFdRealOpt->fdAddr.selfAddr)), (void *)pfarend, addLen);
	if(cmpVal != 0)
	{	
		/*msg between different fd */
		cmpVal = memcmp((void *)(&(pstFdRealOpt->fdAddr.newConnAddr)), (void *)pfarend, addLen);
		if(cmpVal != 0)
		{
			/*receiver not have this new connect, so record it and change role to NEW */
			memcpy((void *)(&(pstFdRealOpt->fdAddr.newConnAddr)), (void *)pfarend, addLen);

			pstFdRealOpt->fdAddr.fdRole &= (~GPN_SOCKET_ROLE_ONMASK);
			pstFdRealOpt->fdAddr.fdRole |= GPN_SOCKET_ROLE_NEW;

			if(pstFdRealOpt->fdAddr.fdRole & GPN_SOCKET_ROLE_UNIX)
			{
				GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : after NewConnect: role %08x self %s defConnAddr %s newConnAddr %s partner %s addrLen %d \n\r",\
					__FUNCTION__, getpid(),\
					pstFdRealOpt->fdAddr.fdRole,\
					pstFdRealOpt->fdAddr.selfAddr.sun_path,\
					pstFdRealOpt->fdAddr.defConnAddr.sun_path,\
					pstFdRealOpt->fdAddr.newConnAddr.sun_path,\
					((struct sockaddr_un *)pfarend)->sun_path,\
					addLen);
			}
			else if(pstFdRealOpt->fdAddr.fdRole & GPN_SOCKET_ROLE_INET)
			{
				GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : after NewConnect: role %08x self %08x-%d defConnAddr %08x-%d newConnAddr %08x-%d partner %08x-%d addrLen %d \n\r",\
					__FUNCTION__, getpid(),\
					pstFdRealOpt->fdAddr.fdRole,\
					((struct sockaddr_in *)&(pstFdRealOpt->fdAddr.selfAddr))->sin_addr.s_addr,\
					((struct sockaddr_in *)&(pstFdRealOpt->fdAddr.selfAddr))->sin_port,\
					((struct sockaddr_in *)&(pstFdRealOpt->fdAddr.defConnAddr))->sin_addr.s_addr,\
					((struct sockaddr_in *)&(pstFdRealOpt->fdAddr.defConnAddr))->sin_port,\
					((struct sockaddr_in *)&(pstFdRealOpt->fdAddr.newConnAddr))->sin_addr.s_addr,\
					((struct sockaddr_in *)&(pstFdRealOpt->fdAddr.newConnAddr))->sin_port,\
					((struct sockaddr_in *)(pfarend))->sin_addr.s_addr,\
					((struct sockaddr_in *)(pfarend))->sin_port,\
					addLen);
			}

			/*tx msg to notify link partner new connect addr*/
			gpnSockSpMsgTxNewConnNotifyPartner(pstFdSet, msgSrc, (void *)(&(pstFdRealOpt->fdAddr.selfAddr)));
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : send newConn notify\n\r", __FUNCTION__, getpid());
		}
	}
	else
	{
		/*self to self*/
		if(pstFdRealOpt->fdAddr.fdRole & GPN_SOCKET_ROLE_UNIX)
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : after NewConnect:self to self msg, can not connect!\n\rmodule server selfAddr %s defConnAddr %s\n\r",\
				__FUNCTION__, getpid(),\
				pstSelfFdRealOpt->fdAddr.selfAddr.sun_path,\
				pstSelfFdRealOpt->fdAddr.defConnAddr.sun_path);
		}
		else if(pstFdRealOpt->fdAddr.fdRole & GPN_SOCKET_ROLE_INET)
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : after NewConnect:self to self msg, can not connect!\n\rmodule server selfAddr %08x-%d defConnAddr %08x-%d\n\r",\
				__FUNCTION__, getpid(),\
				((struct sockaddr_in *)&(pstSelfFdRealOpt->fdAddr.selfAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstSelfFdRealOpt->fdAddr.selfAddr))->sin_port,\
				((struct sockaddr_in *)&(pstSelfFdRealOpt->fdAddr.defConnAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstSelfFdRealOpt->fdAddr.defConnAddr))->sin_port);

		}
	}
	
	return pstFdRealOpt;
}

UINT32 gpnSockMsgRxACKTxProc(stGpnFdOptInfo *pstFdOptInfo, UINT32 *msgBuff)
{
	UINT32 seat;
	UINT32 reVal;
	UINT32 sameRx;
	UINT32 msgType;
	UINT32 msgIndex;
	stGpnFdTxRxRec *pstfdTxRxRec;
	gpnSockAckMsg txAck;

	/*assert*/
	if( (pstFdOptInfo ==NULL) ||
		(msgBuff == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	msgIndex = msgBuff[GPN_SOCK_MSG_SUFFIX_INDEX];
	msgType = msgBuff[GPN_SOCK_MSG_SUFFIX_CMD];

	pstfdTxRxRec = &(pstFdOptInfo->fdTxRxRec);
	seat = msgIndex % pstfdTxRxRec->rxHash;

	sameRx = GPN_SELECT_GEN_NO;
	if( (pstfdTxRxRec->stFdRx[seat].rxCmdIndex == msgIndex) &&
		(pstfdTxRxRec->stFdRx[seat].rxCmdType == msgType) )
	{
		GPN_SELECT_PRINT(GPN_SELECT_CUP, "ACKTxProc:re rx,index %d cmdType %08x msgCtrl %08x\n\r",\
				msgBuff[GPN_SOCK_MSG_SUFFIX_INDEX],pstfdTxRxRec->stFdRx[seat].rxCmdType,msgBuff[GPN_SOCK_MSG_SUFFIX_CTRL]);
		/*rcv same msg more than once, twice? */
		if( (msgBuff[GPN_SOCK_MSG_SUFFIX_CTRL] & GPN_SOCK_MSG_RE_TX_RX_BIT)\
			        						  == GPN_SOCK_MSG_RE_TX_RX_BIT)
		{
			/*do not re porc this msg*/
			sameRx = GPN_SELECT_GEN_YES;
		}
	}
	
	pstfdTxRxRec->lastRx = msgIndex;
	pstfdTxRxRec->stFdRx[seat].rxCmdIndex = msgIndex;
	pstfdTxRxRec->stFdRx[seat].rxCmdType = msgType;
	pstfdTxRxRec->stFdRx[seat].CmdSrc = msgBuff[GPN_SOCK_MSG_SUFFIX_SRC];
	pstfdTxRxRec->stFdRx[seat].CmdDst = msgBuff[GPN_SOCK_MSG_SUFFIX_DST];
	pstfdTxRxRec->stFdRx[seat].isAckSend = GPN_SELECT_GEN_NO;
	pstfdTxRxRec->stFdRx[seat].reSendCnt = 0;
	
	if(msgBuff[GPN_SOCK_MSG_SUFFIX_CTRL] & GPN_SOCK_MSG_ACK_NEED_BIT)
	{
		/*ACK tx*/
		txAck.iIndex = msgIndex;
		txAck.iMsgCtrl = 0;
		txAck.iDstId = msgBuff[GPN_SOCK_MSG_SUFFIX_SRC];
		txAck.iSrcId = msgBuff[GPN_SOCK_MSG_SUFFIX_DST];
		txAck.iMsgType = msgType|GPN_SOCK_MSG_ACK_BIT;
		
		reVal = gpnSocketUseSendTo(pstFdOptInfo, (void *)(&txAck), GPN_SOCK_ACK_BLEN);
		if(reVal == GPN_SELECT_GEN_OK)
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "ACKTxProc:send ok,cmdType %08x reSendCnt %d\n\r",\
				pstfdTxRxRec->stFdRx[seat].rxCmdType,pstfdTxRxRec->stFdRx[seat].reSendCnt);
			pstfdTxRxRec->stFdRx[seat].isAckSend = GPN_SELECT_GEN_YES;
		}
		else
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "ACKTxProc:send err,cmdType %08x reSendCnt %d\n\r",\
				pstfdTxRxRec->stFdRx[seat].rxCmdType,pstfdTxRxRec->stFdRx[seat].reSendCnt);
			pstfdTxRxRec->rxClcCheck = GPN_SELECT_GEN_YES;
		}
	}
	else
	{
		GPN_SELECT_PRINT(GPN_SELECT_CUP, "ACKTxProc:no need,cmdType %08x reSendCnt %d\n\r",\
				pstfdTxRxRec->stFdRx[seat].rxCmdType,pstfdTxRxRec->stFdRx[seat].reSendCnt);
		pstfdTxRxRec->stFdRx[seat].isAckSend = GPN_SELECT_GEN_YES;
	}

	if(sameRx == GPN_SELECT_GEN_YES)
	{
		return GPN_SELECT_GEN_ERR;
	}
	else
	{
		return GPN_SELECT_GEN_OK;
	}
}
UINT32 gpnSockACKRxProc(stSockFdSet *pstFdSet, stGpnFdTxRxRec *pstfdTxRxRec, UINT32 *msgBuff)
{
	UINT32 i;
	UINT32 msgIndex;
	UINT32 msgType;

	/*assert*/
	if( (pstFdSet == NULL) ||
		(pstfdTxRxRec == NULL) ||
		(msgBuff == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	msgIndex = msgBuff[GPN_SOCK_ACK_SUFFIX_INDEX];
	msgType = msgBuff[GPN_SOCK_ACK_SUFFIX_CMD];
	msgType &= GPN_SOCK_MSG_ACK_MASK;
	/*ACK check*/
	i = msgIndex % pstfdTxRxRec->txHash;
	if( (pstfdTxRxRec->stFdTx[i].txCmdIndex == msgIndex) &&
		(pstfdTxRxRec->stFdTx[i].txCmdType == msgType) )
	{
		pstfdTxRxRec->stFdTx[i].isAckRcv = GPN_SELECT_GEN_YES;
		gpnSockTxFifoAckRxProc(pstFdSet, pstfdTxRxRec);
	}
	else
	{
		/*tx too many msg, so fifo out*/
		GPN_SELECT_PRINT(GPN_SELECT_CUP, "ACKRxProc:save-txIndex %d save-txCmd %08x rxIndex %d rxCmd %08x fifoSeat %08x\n\r",\
			pstfdTxRxRec->stFdTx[i].txCmdIndex,pstfdTxRxRec->stFdTx[i].txCmdType,\
			msgIndex,msgType,pstfdTxRxRec->stFdTx[i].FifoSeat);
	}

	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockAckTxPorc(stGpnFdOptInfo *pstFdOptInfo, stGpnFdTxRxRec *pFdTxRxRec)
{
	UINT32 i;
	UINT32 reVal;
	gpnSockAckMsg txAck;

	/*assert*/
	if(pFdTxRxRec == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	if(pstFdOptInfo->fdAlive.sta == GPN_SELECT_GEN_OK)
	{
		for(i=0;i<pFdTxRxRec->rxHash;i++)
		{
			if(pFdTxRxRec->stFdRx[i].isAckSend == GPN_SELECT_GEN_OK)
			{
				continue;
			}
			/*ACK tx*/
			txAck.iIndex = pFdTxRxRec->stFdRx[i].rxCmdIndex;
			txAck.iMsgCtrl = 0;
			txAck.iDstId = pFdTxRxRec->stFdRx[i].CmdSrc;
			txAck.iSrcId = pFdTxRxRec->stFdRx[i].CmdDst;
			txAck.iMsgType = pFdTxRxRec->stFdRx[i].rxCmdType|GPN_SOCK_MSG_ACK_BIT;
			
			reVal = gpnSocketUseSendTo(pstFdOptInfo, (void *)(&txAck), GPN_SOCK_ACK_BLEN);
			if(reVal == GPN_SELECT_GEN_OK)
			{
				pFdTxRxRec->stFdRx[i].isAckSend = GPN_SELECT_GEN_YES;
			}
			else
			{
				pFdTxRxRec->stFdRx[i].reSendCnt++;
				if(pFdTxRxRec->stFdTx[i].reSendCnt > GPN_SOCK_MSG_RETX_TRY)
				{
					pFdTxRxRec->stFdRx[i].isAckSend = GPN_SELECT_GEN_YES;
					pFdTxRxRec->rxAckLos++;
					
				}
				
				return GPN_SELECT_GEN_ERR;
			}
		}
		pFdTxRxRec->rxClcCheck = GPN_SELECT_GEN_NO;

		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockMsgTxPorc(stSockFdSet *pstFdSet, stGpnFdOptInfo *pstFdOptInfo)
{
	UINT32 msgBuff[GPN_SOCK_MAX_MSG_DWORD_LEN];
	UINT32 buffSize;
	UINT32 fifoSeat;
	UINT32 seat;
	UINT32 dog;
	stGpnFdTxRxRec *pfdTxRxRec;
	
	/*assert*/
	if( (pstFdSet == NULL) ||
		(pstFdOptInfo == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	pfdTxRxRec = &(pstFdOptInfo->fdTxRxRec);

	GPN_SELECT_PRINT(GPN_SELECT_CUP, "befor MsgTxProc:     sta %d nowTxP %d waitAckP %d txLimitP %d txIndex %d\n\r",\
		pstFdOptInfo->fdAlive.sta,pfdTxRxRec->nowTxP,pfdTxRxRec->waitAckP,pfdTxRxRec->txLimitP,pfdTxRxRec->lastTx);
	if(pstFdOptInfo->fdAlive.sta == GPN_SELECT_GEN_OK)
	{
		/*connect ok, can tx msg*/
		for(dog=0;dog<pfdTxRxRec->reTxNum;dog++)
		{
			/*renew seat for every nowTxP change*/
			seat = pfdTxRxRec->nowTxP;
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "middl MsgTxProc:seat %d isTxOk %d txLimitP %d txIndex %d\n\r",\
				seat,pfdTxRxRec->stFdTx[seat].isTxOk,pfdTxRxRec->txLimitP,pfdTxRxRec->lastTx);
			if( (seat == pfdTxRxRec->txLimitP) ||\
				(pfdTxRxRec->stFdTx[seat].isTxOk == GPN_SELECT_GEN_YES) )
			{
				/*msgs witch need tx are complete*/
				break;
			}
			else
			{
				/*saved msg out of fifo, re tx*/
				buffSize = GPN_SOCK_MAX_MSG_BYTE_LEN;
				fifoSeat = pfdTxRxRec->stFdTx[seat].FifoSeat;
				if(gpnSockFdSetCommFifoOut(&(pstFdSet->txMsgFifo), fifoSeat, msgBuff, &buffSize) == GPN_SELECT_GEN_ERR)
				{
					/*err*/
					GPN_SELECT_PRINT(GPN_SELECT_CUP, "tx fifo save err seat %d fifoSeat %d\n\r", pfdTxRxRec->nowTxP, fifoSeat);
					gpnSockTxFifoMsgTxRenewProc(pstFdSet, pfdTxRxRec, GPN_SELECT_GEN_OK);
					continue;
				}
				if(gpnSocketUseSendTo(pstFdOptInfo, (void *)msgBuff, buffSize) == GPN_SELECT_GEN_OK)
				{
					gpnSockTxFifoMsgTxRenewProc(pstFdSet, pfdTxRxRec, GPN_SELECT_GEN_OK);
				}
				else
				{
					gpnSockTxFifoMsgTxRenewProc(pstFdSet, pfdTxRxRec, GPN_SELECT_GEN_ERR);
					return GPN_SELECT_GEN_ERR;
				}
			}
		}
		/*msgs witch need tx are complete*/
		pfdTxRxRec->txClcCheck = GPN_SELECT_GEN_NO;

		GPN_SELECT_PRINT(GPN_SELECT_CUP, "after MsgTxProc:ok!  sta %d nowTxP %d waitAckP %d txLimitP %d txIndex %d\n\r",\
		pstFdOptInfo->fdAlive.sta,pfdTxRxRec->nowTxP,pfdTxRxRec->waitAckP,pfdTxRxRec->txLimitP,pfdTxRxRec->lastTx);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		GPN_SELECT_PRINT(GPN_SELECT_AGP, "after MsgTxProc:err! sta %d nowTxP %d waitAckP %d txLimitP %d txIndex %d\n\r",\
		pstFdOptInfo->fdAlive.sta,pfdTxRxRec->nowTxP,pfdTxRxRec->waitAckP,pfdTxRxRec->txLimitP,pfdTxRxRec->lastTx);
		/*connect err or reach retx Max, can not tx msg*/
		/*just fifo msg, opt msg tx in 1s timer proc   */
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockTransferModuMsgProc(stSockFdSet *pstFdSet, gpnSockMsg *msgBuff, UINT32 len)
{
	UINT32 dstId;

	switch(msgBuff->iMsgType)
	{
		/*proc comm-protocol-self*/
		case GPN_SELF_MSG_COMM_CHECK:
		case GPN_SELF_MSG_COMM_CHECK_RSP:
		case GPN_GEN_MSG_TIMER_1S:
			return GPN_SELECT_GEN_OK;

		default:
			break;
	}

	/*how to transfer*/
	dstId = msgBuff->iDstId;

	if(dstId < pstFdSet->fdMaxSpace)
	{
		if(pstFdSet->fdOptInfo[dstId].en ==  GPN_SELECT_GEN_ENABLE)
		{
			/*transfer MSG*/
			/*guarantee or not:not guarantee*/
			gpnSocketUseSendTo(&(pstFdSet->fdOptInfo[dstId]), (void *)msgBuff, len);
			/*gpnSockGuaranteeMsgTxProc(pstFdSet, msgBuff, len);*/
		}
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
	
}
UINT32 gpnSockWriteEnProc(stSockFdSet *pstFdSet, stGpnFdOptInfo *pstFdOptInfo)
{
	UINT32 reVal;
	stGpnFdTxRxRec *pfdTxRxRec;

	pstFdOptInfo->fdOpt.wtEn = GPN_SELECT_GEN_NO;

	pfdTxRxRec = &(pstFdOptInfo->fdTxRxRec);
	if(pfdTxRxRec->txClcCheck == GPN_SELECT_GEN_YES)
	{
		reVal = gpnSockMsgTxPorc(pstFdSet, pstFdOptInfo);
		if(reVal == GPN_SELECT_GEN_ERR)
		{
			return GPN_SELECT_GEN_ERR;
		}
		else
		{
			pfdTxRxRec->txClcCheck = GPN_SELECT_GEN_NO;
		}
	}

	if(pfdTxRxRec->rxClcCheck == GPN_SELECT_GEN_YES)
	{
		reVal = gpnSockAckTxPorc(pstFdOptInfo, pfdTxRxRec);
		if(reVal == GPN_SELECT_GEN_ERR)
		{
			return GPN_SELECT_GEN_ERR;
		}
		else
		{
			pfdTxRxRec->rxClcCheck = GPN_SELECT_GEN_NO;
		}
	}

	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNoGuaranteeMsgTxProc(stSockFdSet *pstFdSet, gpnSockMsg *msgBuff, UINT32 byteLen)
{
	UINT32 fdSeat;
	stGpnFdOptInfo *pstFdOptInfo;

	/*assert*/
	if( (pstFdSet == NULL) ||\
		(msgBuff == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	/*get fdSeat from iDstId*/
	if(gpnSockCommMsgDst2RoleSeat(pstFdSet, msgBuff->iDstId, &fdSeat) != GPN_SELECT_GEN_OK)
	{
		GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s(%d) : get fdSeat err local cpuId(%d) dstId(%08x)!\n\r",\
			__FUNCTION__, getpid(), pstFdSet->CPUId, msgBuff->iDstId);
		printf("%s(%d) : get fdSeat err local cpuId(%d) dstId(%08x)!\n\r",\
			__FUNCTION__, getpid(), pstFdSet->CPUId, msgBuff->iDstId);
		
		return GPN_SELECT_GEN_ERR;
	}

	/*if re-choose fdSeat, because maybe we should use gpn_transfer */
	if(fdSeat >= pstFdSet->fdMaxSearchNum)
	{
		return GPN_SELECT_GEN_ERR;
	}
	pstFdOptInfo = &(pstFdSet->fdOptInfo[fdSeat]);
	if(pstFdOptInfo->en == GPN_SELECT_GEN_DISABLE)
	{
		/*fd in fdSeat is disable, so if self is not transfer modu, re-choose fdSeat*/
		if(pstFdSet->moduId != GPN_SOCK_ROLE_TRANS)
		{
			GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s(%d) : re-choose Msg dst, msgSrc %08x msgDst %08x\n\r",\
				__FUNCTION__, getpid(), msgBuff->iSrcId, msgBuff->iDstId);
			
			pstFdOptInfo = &(pstFdSet->fdOptInfo[GPN_SOCK_ROLE_TRANS]);
			if( (pstFdOptInfo == NULL) ||\
				(pstFdOptInfo->en == GPN_SELECT_GEN_DISABLE) )
			{
				/*re-choose tx-dst error, but this error should not happen, because gpn_transfer always enable*/
				GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s(%d) : re-choose tx-dst err!\n\r", __FUNCTION__, getpid());
				
				return GPN_SELECT_GEN_ERR;
			}
		}
		else
		{
			/*self is transfer modu, can't transfer, so drop this msg */
			GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s(%d) : mstType(%08x) msgSrc(%08x) msgDst(%08x) dorp msg! err!\n\r",\
				__FUNCTION__, getpid(), msgBuff->iMsgType, msgBuff->iSrcId, msgBuff->iDstId);
			
			return GPN_SELECT_GEN_ERR;
		}
	}

	/*real tx msg use socket*/
	gpnSocketUseSendTo(pstFdOptInfo, (void *)msgBuff, byteLen);
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSocketUseSendTo(stGpnFdOptInfo *pstFdOptInfo, void *txCmd, UINT32 byteLen)
{
	INT32 byte;
	gpnSockMsg *pgpnSockMsg;

	/*assert*/
	if( (pstFdOptInfo == NULL) ||\
		(txCmd == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	byte = -1;
	pgpnSockMsg = (gpnSockMsg *)txCmd;

	if((pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_OLD_UDP_UNIX)
									==GPN_SOCKET_ROLE_OLD_UDP_UNIX)	
	{
		/*UDP send ACK-use defConnAddr*/
		byte = sendto(pstFdOptInfo->fd, txCmd, byteLen, 0,
						(struct sockaddr *)&(pstFdOptInfo->fdAddr.defConnAddr),
						(socklen_t)GPN_SOCK_ADDR_UN_LEN);
		if(byte < 0)
		{
			//perror("OLD_UDP_UNIX NoGuarantee sendto");
			//printf("OLD_UDP_UNIX process(%d): err-code %d\n\r", getpid(), byte);
			GPN_SELECT_PRINT(GPN_SELECT_AGP, "OLD_UDP_UNIX NoGuarantee sendto");
			GPN_SELECT_PRINT(GPN_SELECT_AGP, "OLD_UDP_UNIX process(%d): err-code %d\n\r", getpid(), byte);
			
			GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s(%d) : err, fd %d fdRole %08x index %d  iMsgCtrl %08x type %08x len %d scr \"%s\" dst \"%s\"\n\r",\
				__FUNCTION__, getpid(), pstFdOptInfo->fd, pstFdOptInfo->fdAddr.fdRole,\
				pgpnSockMsg->iIndex,pgpnSockMsg->iMsgCtrl,pgpnSockMsg->iMsgType,byteLen,\
				pstFdOptInfo->fdAddr.selfAddr.sun_path,\
				pstFdOptInfo->fdAddr.defConnAddr.sun_path);
		}
		else
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : ok, fd %d fdRole %08x index %d  iMsgCtrl %08x type %08x len %d scr %s dst %s\n\r",\
				__FUNCTION__, getpid(), pstFdOptInfo->fd, pstFdOptInfo->fdAddr.fdRole,\
				pgpnSockMsg->iIndex,pgpnSockMsg->iMsgCtrl,pgpnSockMsg->iMsgType,byteLen,\
				pstFdOptInfo->fdAddr.selfAddr.sun_path,\
				pstFdOptInfo->fdAddr.defConnAddr.sun_path);
		}
	}
	else if((pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_NEW_UDP_UNIX)
										 ==GPN_SOCKET_ROLE_NEW_UDP_UNIX)
	{
		/*UDP send ACK-ues newConnectAddr*/
		byte = sendto(pstFdOptInfo->fd, txCmd, byteLen, 0,
						/* temp modify for if newConnAddr not same with defConnAddr*/
						(struct sockaddr *)&(pstFdOptInfo->fdAddr.defConnAddr),
						/*(struct sockaddr *)&(pstFdOptInfo->fdAddr.newConnAddr),*/
						(socklen_t)GPN_SOCK_ADDR_UN_LEN);
		if(byte < 0)
		{
//			perror("NEW_UDP_UNIX NoGuarantee sendto");
//			printf("NEW_UDP_UNIX process(%d): err-code %d\n\r", getpid(), byte);
			GPN_SELECT_PRINT(GPN_SELECT_AGP, "OLD_UDP_UNIX NoGuarantee sendto");
			GPN_SELECT_PRINT(GPN_SELECT_AGP, "NEW_UDP_UNIX process(%d): err-code %d\n\r", getpid(), byte);
			
			GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s(%d) : err, fd %d fdRole %08x index %d  iMsgCtrl %08x type %08x len %d scr %s dst %s\n\r",\
				__FUNCTION__, getpid(), pstFdOptInfo->fd, pstFdOptInfo->fdAddr.fdRole,\
				pgpnSockMsg->iIndex,pgpnSockMsg->iMsgCtrl,pgpnSockMsg->iMsgType,byteLen,\
				pstFdOptInfo->fdAddr.selfAddr.sun_path,\
				pstFdOptInfo->fdAddr.newConnAddr.sun_path);
		}
		else
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : ok, fd %d fdRole %08x index %d  iMsgCtrl %08x type %08x len %d scr %s dst %s\n\r",\
				__FUNCTION__, getpid(), pstFdOptInfo->fd, pstFdOptInfo->fdAddr.fdRole,\
				pgpnSockMsg->iIndex,pgpnSockMsg->iMsgCtrl,pgpnSockMsg->iMsgType,byteLen,
				pstFdOptInfo->fdAddr.selfAddr.sun_path,\
				pstFdOptInfo->fdAddr.newConnAddr.sun_path);
		}
	}
	else if((pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_OLD_UDP_INET)
										==GPN_SOCKET_ROLE_OLD_UDP_INET)	
	{
		/*UDP send ACK-use defConnAddr*/
		byte = sendto(pstFdOptInfo->fd, txCmd, byteLen, 0,
						(struct sockaddr *)&(pstFdOptInfo->fdAddr.defConnAddr),
						(socklen_t)GPN_SOCK_ADDR_UN_LEN);
		if(byte < 0)
		{
//			perror("OLD_UDP_INET NoGuarantee sendto");
//			printf("OLD_UDP_INET process(%d): err-code %d\n\r", getpid(), byte);
			GPN_SELECT_PRINT(GPN_SELECT_AGP, "OLD_UDP_UNIX NoGuarantee sendto");
			GPN_SELECT_PRINT(GPN_SELECT_AGP, "OLD_UDP_INET process(%d): err-code %d\n\r", getpid(), byte);

			GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s(%d) : err, fd %d fdRole %08x index %d  iMsgCtrl %08x type %08x len %d scr %08x-%d dst %08x-%d\n\r",\
				__FUNCTION__, getpid(), pstFdOptInfo->fd, pstFdOptInfo->fdAddr.fdRole,\
				pgpnSockMsg->iIndex,pgpnSockMsg->iMsgCtrl,pgpnSockMsg->iMsgType,byteLen,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.selfAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.selfAddr))->sin_port,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.defConnAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.defConnAddr))->sin_port);
		}
		else
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : ok, fd %d  fdRole %08x index %d  iMsgCtrl %08x type %08x len %d scr %08x-%d dst %08x-%d\n\r",\
				__FUNCTION__, getpid(), pstFdOptInfo->fd, pstFdOptInfo->fdAddr.fdRole,\
				pgpnSockMsg->iIndex,pgpnSockMsg->iMsgCtrl,pgpnSockMsg->iMsgType,byteLen,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.selfAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.selfAddr))->sin_port,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.defConnAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.defConnAddr))->sin_port);
		}
	}
	else if((pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_NEW_UDP_INET)
										 ==GPN_SOCKET_ROLE_NEW_UDP_INET)
	{
		/*UDP send ACK-ues newConnectAddr*/
		byte = sendto(pstFdOptInfo->fd, txCmd, byteLen, 0,
						/* temp modify for if newConnAddr not same with defConnAddr*/
						(struct sockaddr *)&(pstFdOptInfo->fdAddr.defConnAddr),
						/*(struct sockaddr *)&(pstFdOptInfo->fdAddr.newConnAddr),*/
						(socklen_t)GPN_SOCK_ADDR_UN_LEN);
		if(byte < 0)
		{
			//perror("NEW_UDP_INET NoGuarantee sendto");
			//printf("NEW_UDP_INET process(%d): err-code %d\n\r", getpid(), byte);
			GPN_SELECT_PRINT(GPN_SELECT_AGP, "NEW_UDP_INET NoGuarantee sendto");
			GPN_SELECT_PRINT(GPN_SELECT_AGP, "NEW_UDP_INET process(%d): err-code %d\n\r", getpid(), byte);

			GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s(%d) : err, fd %d fdRole %08x index %d  iMsgCtrl %08x type %08x len %d scr %08x-%d dst %08x-%d\n\r",\
				__FUNCTION__, getpid(), pstFdOptInfo->fd, pstFdOptInfo->fdAddr.fdRole,\
				pgpnSockMsg->iIndex,pgpnSockMsg->iMsgCtrl,pgpnSockMsg->iMsgType,byteLen,
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.selfAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.selfAddr))->sin_port,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.newConnAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.newConnAddr))->sin_port);
		}
		else
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "%s(%d) : ok, fd %d fdRole %08x index %d  iMsgCtrl %08x type %08x len %d scr %08x-%d dst %08x-%d\n\r",\
				__FUNCTION__, getpid(), pstFdOptInfo->fd, pstFdOptInfo->fdAddr.fdRole,\
				pgpnSockMsg->iIndex,pgpnSockMsg->iMsgCtrl,pgpnSockMsg->iMsgType,byteLen,
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.selfAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.selfAddr))->sin_port,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.newConnAddr))->sin_addr.s_addr,\
				((struct sockaddr_in *)&(pstFdOptInfo->fdAddr.newConnAddr))->sin_port);
		}
	}
	else if((pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_OLD_TCP_UNIX)
										 ==GPN_SOCKET_ROLE_OLD_TCP_UNIX)	
	{
		/*TCP send ACK-use defConnAddr*/
	}
	else if((pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_NEW_TCP_UNIX)
										 ==GPN_SOCKET_ROLE_NEW_TCP_UNIX)
	{
		/*TCP send ACK-use newConnectAddr*/
	}
	else if((pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_OLD_TCP_INET)
										 ==GPN_SOCKET_ROLE_OLD_TCP_INET)	
	{
		/*TCP send ACK-use defConnAddr*/
	}
	else if((pstFdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_NEW_TCP_INET)
										 ==GPN_SOCKET_ROLE_NEW_TCP_INET)
	{
		/*TCP send ACK-use newConnectAddr*/
	}
	else
	{
		GPN_SELECT_PRINT(GPN_SELECT_SVP, "%s(%d) : not have right fdRole %08x\n\r", \
			__FUNCTION__, getpid(), pstFdOptInfo->fdAddr.fdRole);
		/*do not have this kind role*/
	}

	if(byte < 0)
	{
		return GPN_SELECT_GEN_ERR;
	}
	else
	{
		return GPN_SELECT_GEN_OK;
	}
}
UINT32 gpnSockGuaranteeMsgTxProc(stSockFdSet *pstFdSet, gpnSockMsg *msgBuff, UINT32 len)
{
	UINT32 dstId;
	UINT32 reVal;
	UINT32 gSeat;
	stGpnFdOptInfo *pstFdOptInfo;

	dstId = msgBuff->iDstId;
	if(dstId >= pstFdSet->fdMaxSearchNum)
	{
		return GPN_SELECT_GEN_ERR;
	}

	/*txMsg limit:choice output module fd-pstFdOptInfo->fd*/

	
	pstFdOptInfo = &(pstFdSet->fdOptInfo[dstId]);
	/*iIndex add*/
	msgBuff->iIndex = pstFdOptInfo->fdTxRxRec.lastTx;
	/*save, then transfer*/
	reVal = gpnSockFdSetCommFifoAbtain(&(pstFdSet->txMsgFifo), (UINT32 *)msgBuff, len, &gSeat);
	if(reVal == GPN_SELECT_GEN_OK)
	{
		gpnSockTxFifoMsgTxPreProc(pstFdSet, &(pstFdOptInfo->fdTxRxRec), msgBuff->iMsgType, gSeat);
		
		gpnSockMsgTxPorc(pstFdSet, pstFdOptInfo);
	}
	else
	{	
		GPN_SELECT_PRINT(GPN_SELECT_AGP, "abtain err\n\r");
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFdSetCommFifoInit(stGpnFdCommFifo *ptxFifo)
{
	UINT32 i;
	
	/*assert*/
	if(ptxFifo == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	ptxFifo->maxBloc = GPN_SOCK_HEAD_FIFO_NUM;
	ptxFifo->idleBloc = GPN_SOCK_HEAD_FIFO_NUM;
	
	for(i=0; i<GPN_SOCK_HEAD_FIFO_NUM; i++)
	{
		ptxFifo->resourc[i] = i;
		ptxFifo->seatPosit[i] = i;
		ptxFifo->payloadSeat[i] = GPN_SELECT_GEN_FILLED;
		ptxFifo->payloadSize[i] = 0;
	}

	ptxFifo->maxCellB = GPN_SOCK_PAYLOAD_FIFO_NUM;
	ptxFifo->idleCellB = GPN_SOCK_PAYLOAD_FIFO_NUM;
	
	for(i=0; i<GPN_SOCK_PAYLOAD_FIFO_NUM; i++)
	{
		ptxFifo->cellRsc[i] = i;
		ptxFifo->cellSeatP[i] = i;
	}
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFdSetCommFifoAbtain(stGpnFdCommFifo *ptxFifo, UINT32 *msgBuff, UINT32 byteLen, UINT32 *pseat)
{
	UINT32 seat;
	UINT32 nextSeat;

	GPN_SELECT_PRINT(GPN_SELECT_CUP, "befor abtain byteLen %d MAX %d %d %d %d\n\r",byteLen,GPN_SOCK_MAX_MSG_BYTE_LEN,ptxFifo->idleBloc,ptxFifo->idleCellB,ptxFifo->maxCellB);
	/*assert*/
	if( (ptxFifo == NULL) ||
		(msgBuff == NULL) ||
		(pseat == NULL) ||
		(byteLen > GPN_SOCK_MAX_MSG_BYTE_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	if(ptxFifo->idleBloc == 0)
	{
		return GPN_SELECT_GEN_ERR;
	}

	if( (byteLen > GPN_SOCK_MSG_HEAD_BLEN) &&
		(ptxFifo->idleCellB == 0) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	/*msg head*/
	seat = ptxFifo->resourc[0];
	nextSeat = ptxFifo->resourc[ptxFifo->idleBloc - 1];
	
	ptxFifo->seatPosit[seat] = nextSeat;
	ptxFifo->seatPosit[nextSeat] = 0;
	
	ptxFifo->resourc[0] = ptxFifo->resourc[nextSeat];
	ptxFifo->resourc[nextSeat] = seat;
	
	memcpy((void *)&(ptxFifo->fifo[seat]), (void *)msgBuff, GPN_SOCK_MSG_HEAD_BLEN);
	ptxFifo->payloadSeat[seat] = GPN_SELECT_GEN_FILLED;
	ptxFifo->idleBloc--;
	*pseat = seat;
	
	
	if(byteLen > GPN_SOCK_MSG_HEAD_BLEN)
	{
		/*msg payload*/
		seat = ptxFifo->cellRsc[0];
		nextSeat = ptxFifo->cellRsc[ptxFifo->idleBloc - 1];
		
		ptxFifo->cellSeatP[seat] = nextSeat;
		ptxFifo->cellSeatP[nextSeat] = 0;
		
		ptxFifo->cellRsc[0] = ptxFifo->cellRsc[nextSeat];
		ptxFifo->cellRsc[nextSeat] = seat;
		
		memcpy((void *)&(ptxFifo->cellFifo[seat]),
			(void *)(msgBuff+GPN_SOCK_MSG_HEAD_LEN), (byteLen-GPN_SOCK_MSG_HEAD_BLEN));
		ptxFifo->payloadSize[*pseat] = byteLen-GPN_SOCK_MSG_HEAD_BLEN;
		ptxFifo->payloadSeat[*pseat] = seat;
		ptxFifo->idleCellB--;
	}
	GPN_SELECT_PRINT(GPN_SELECT_CUP, "after abtain byteLen %d MAX %d %d %d %d seat %d\n\r",\
		byteLen,GPN_SOCK_MAX_MSG_BYTE_LEN,ptxFifo->idleBloc,ptxFifo->idleCellB,ptxFifo->maxCellB,*pseat);
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFdSetCommFifoOut(stGpnFdCommFifo *ptxFifo, UINT32 seat, UINT32 *msgBuff, UINT32 *buffSize)
{
	UINT32 payLSeat;
	UINT32 payLLen;
	
	GPN_SELECT_PRINT(GPN_SELECT_CUP, "befor fifoOut byteLen %d seat %d %d %d %d\n\r",*buffSize,seat,ptxFifo->idleBloc,ptxFifo->idleCellB,ptxFifo->maxCellB);
	/*assert*/
	if( (msgBuff == NULL) ||
		(buffSize == NULL) ||
		(seat >= ptxFifo->maxBloc) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	if((*buffSize) < GPN_SOCK_MAX_MSG_BYTE_LEN)
	{
		return GPN_SELECT_GEN_ERR;
	}

	if(ptxFifo->payloadSeat[seat] != GPN_SELECT_GEN_FILLED)
	{
		payLSeat = ptxFifo->payloadSeat[seat];
		payLLen = ptxFifo->payloadSize[seat];
		memcpy((void *)msgBuff, (void *)(&(ptxFifo->fifo[seat])), GPN_SOCK_HEAD_FIFO_BYTE_SIZE);
		memcpy((void *)(((UINT8 *)msgBuff)+GPN_SOCK_HEAD_FIFO_BYTE_SIZE), (void *)&(ptxFifo->cellFifo[payLSeat]), payLLen);
		*buffSize = GPN_SOCK_HEAD_FIFO_BYTE_SIZE + payLLen;
	}
	else
	{
		memcpy((void *)msgBuff, (void *)&(ptxFifo->fifo[seat]), GPN_SOCK_HEAD_FIFO_BYTE_SIZE);  
		*buffSize = GPN_SOCK_HEAD_FIFO_BYTE_SIZE;
	}
	GPN_SELECT_PRINT(GPN_SELECT_CUP, "after fifoOut byteLen %d seat %d %d %d %d\n\r",*buffSize,seat,ptxFifo->idleBloc,ptxFifo->idleCellB,ptxFifo->maxCellB);
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFdSetCommFifoRelease(stGpnFdCommFifo *ptxFifo, UINT32 seat)
{
	UINT32 temp;
	UINT32 pSeat;
	UINT32 relBlocP;/*release block position*/
	UINT32 movBloc;/*move block*/
	
	GPN_SELECT_PRINT(GPN_SELECT_CUP, "befor fifoRelease seat %d %d %d\n\r",seat,ptxFifo->idleBloc,ptxFifo->idleCellB);
	/*assert*/
	if( (ptxFifo == NULL) ||
		(seat >= ptxFifo->maxBloc) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	relBlocP = ptxFifo->seatPosit[seat];
	movBloc = ptxFifo->resourc[ptxFifo->idleBloc];
	
	ptxFifo->seatPosit[seat]= ptxFifo->idleBloc;
	ptxFifo->seatPosit[movBloc]= relBlocP;
	
	temp = ptxFifo->resourc[relBlocP];
	ptxFifo->resourc[relBlocP] = ptxFifo->resourc[ptxFifo->idleBloc];
	ptxFifo->resourc[ptxFifo->idleBloc] = temp;

	ptxFifo->idleBloc++;

	if(ptxFifo->payloadSeat[seat] != GPN_SELECT_GEN_FILLED)
	{
		pSeat = ptxFifo->payloadSeat[seat];
		
		relBlocP = ptxFifo->cellSeatP[pSeat];
		movBloc = ptxFifo->cellRsc[ptxFifo->idleCellB];
		
		ptxFifo->cellSeatP[pSeat]= ptxFifo->idleCellB;
		ptxFifo->cellSeatP[movBloc]= relBlocP;
		
		temp = ptxFifo->cellRsc[relBlocP];
		ptxFifo->cellRsc[relBlocP] = ptxFifo->cellRsc[ptxFifo->idleCellB];
		ptxFifo->cellRsc[ptxFifo->idleCellB] = temp;

		ptxFifo->idleCellB++;
		
		ptxFifo->payloadSeat[seat] = GPN_SELECT_GEN_FILLED;
	}
	GPN_SELECT_PRINT(GPN_SELECT_CUP, "after fifoRelease seat %d %d %d\n\r",seat,ptxFifo->idleBloc,ptxFifo->idleCellB);
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockTxFifoMsgTxPreProc(stSockFdSet *pstFdSet, stGpnFdTxRxRec *pfdTxRxRec, UINT32 msgType, UINT32 gSeat)
{
	UINT32 seat;

	GPN_SELECT_PRINT(GPN_SELECT_CUP, "befor MsgTxPreProc:gSeat %d type %08x index %d nowTxP %d waitAckP %d\n\r",\
		gSeat, msgType, pfdTxRxRec->lastTx,\
		pfdTxRxRec->nowTxP, pfdTxRxRec->waitAckP);
	seat = pfdTxRxRec->lastTx % pfdTxRxRec->txHash;

	if(pfdTxRxRec->stFdTx[seat].isTxOk == GPN_SELECT_GEN_NO)
	{
		/*test*/
		if(pfdTxRxRec->nowTxP != seat)
		{
			perror("gpnSockTxFifoMsgTxPreProc:nowTxP");
		}
		pfdTxRxRec->txLosNum++;
		
		pfdTxRxRec->nowTxP++;
		pfdTxRxRec->nowTxP %=  pfdTxRxRec->txHash;
		
		pfdTxRxRec->waitAckP = pfdTxRxRec->nowTxP;
		
		pfdTxRxRec->txLimitP = pfdTxRxRec->waitAckP + GPN_SOCK_FD_RETX_MAX;
		pfdTxRxRec->txLimitP %= pfdTxRxRec->txHash;
	}
	else if(pfdTxRxRec->stFdTx[seat].isAckRcv == GPN_SELECT_GEN_NO)
	{
		pfdTxRxRec->rxAckLos++;

		/*test*/
		if(pfdTxRxRec->waitAckP != seat)
		{
			perror("gpnSockTxFifoMsgTxPreProc:nowTxP");
		}
		pfdTxRxRec->stFdTx[seat].isAckRcv = GPN_SELECT_GEN_YES;
		gpnSockTxFifoAckRxProc(pstFdSet, pfdTxRxRec);
	}

	pfdTxRxRec->stFdTx[seat].FifoSeat = gSeat;
	pfdTxRxRec->stFdTx[seat].txCmdIndex = pfdTxRxRec->lastTx;
	pfdTxRxRec->stFdTx[seat].txCmdType = msgType;
	pfdTxRxRec->stFdTx[seat].reSendCnt = 0;
	pfdTxRxRec->stFdTx[seat].isTxOk = GPN_SELECT_GEN_NO;
	pfdTxRxRec->stFdTx[seat].waitCount = 0;
	pfdTxRxRec->stFdTx[seat].isAckRcv = GPN_SELECT_GEN_NO;
	
	/*renew lastTx*/
	pfdTxRxRec->lastTx++;
	
	GPN_SELECT_PRINT(GPN_SELECT_CUP, "after MsgTxPreProc:gSeat %d type %08x index %d nowTxP %d waitAckP %d\n\r",\
		gSeat, msgType, pfdTxRxRec->lastTx,\
		pfdTxRxRec->nowTxP, pfdTxRxRec->waitAckP);
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockTxFifoMsgTxRenewProc(stSockFdSet *pstFdSet, stGpnFdTxRxRec *pfdTxRxRec, UINT32 txResult)
{
	UINT32 seat;
	
	GPN_SELECT_PRINT(GPN_SELECT_CUP, "befor RenewProc:nowTxP %d waitAckP %d txLimitP %d rtxResult %d\n\r",\
		pfdTxRxRec->nowTxP,pfdTxRxRec->waitAckP,pfdTxRxRec->txLimitP,txResult);
	seat = pfdTxRxRec->nowTxP;
	if(txResult == GPN_SELECT_GEN_ERR)
	{
		
		pfdTxRxRec->stFdTx[seat].reSendCnt++;
		if(pfdTxRxRec->stFdTx[seat].reSendCnt > GPN_SOCK_MSG_RETX_TRY)
		{
			pfdTxRxRec->stFdTx[seat].isTxOk = GPN_SELECT_GEN_YES;

			pfdTxRxRec->nowTxP++;
			pfdTxRxRec->nowTxP %= pfdTxRxRec->txHash;	
			
			pfdTxRxRec->stFdTx[seat].isAckRcv = GPN_SELECT_GEN_YES;
			gpnSockTxFifoAckRxProc(pstFdSet, pfdTxRxRec);
			
			pfdTxRxRec->txLosNum++;
		}
		else
		{
			pfdTxRxRec->txClcCheck = GPN_SELECT_GEN_YES;
		}
	}
	else
	{
		pfdTxRxRec->stFdTx[seat].isTxOk = GPN_SELECT_GEN_YES;
		
		pfdTxRxRec->nowTxP++;
		pfdTxRxRec->nowTxP %= pfdTxRxRec->txHash;		
	}
	GPN_SELECT_PRINT(GPN_SELECT_CUP, "after RenewProc:nowTxP %d waitAckP %d txLimitP %d rtxResult %d\n\r",\
		pfdTxRxRec->nowTxP,pfdTxRxRec->waitAckP,pfdTxRxRec->txLimitP,txResult);
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockTxFifoAckRxProc(stSockFdSet *pstFdSet, stGpnFdTxRxRec *pfdTxRxRec)
{
	UINT32 dog;
	UINT32 seat;
	
	GPN_SELECT_PRINT(GPN_SELECT_CUP, "befor AckRxProc:nowTxP %d fifoSeat %08x waitAckP %d index %d\n\r",\
		pfdTxRxRec->nowTxP,\
		pfdTxRxRec->stFdTx[pfdTxRxRec->nowTxP].FifoSeat,\
		pfdTxRxRec->waitAckP,pfdTxRxRec->lastTx);
	seat = pfdTxRxRec->waitAckP;
	if(pfdTxRxRec->stFdTx[seat].isAckRcv == GPN_SELECT_GEN_YES)
	{
		/*go to nowTxP if we can*/
		for(dog=0;dog<pfdTxRxRec->reTxNum;dog++)
		{
			if(pfdTxRxRec->nowTxP == seat)
			{
				pfdTxRxRec->waitAckP = seat;
				pfdTxRxRec->txLimitP = seat + GPN_SOCK_FD_RETX_MAX;
				pfdTxRxRec->txLimitP %= pfdTxRxRec->txHash;
				break;
			}
			if(pfdTxRxRec->stFdTx[seat].isAckRcv == GPN_SELECT_GEN_YES)
			{
				/*release msg save fifo*/
				if(pfdTxRxRec->stFdTx[seat].FifoSeat != GPN_SELECT_GEN_FILLED)
				{
					gpnSockFdSetCommFifoRelease(&(pstFdSet->txMsgFifo), pfdTxRxRec->stFdTx[seat].FifoSeat);
					pfdTxRxRec->stFdTx[seat].FifoSeat = GPN_SELECT_GEN_FILLED;
				}
				seat++;
				seat %= pfdTxRxRec->txHash;
				continue;
			}
			else
			{
				pfdTxRxRec->waitAckP = seat;
				pfdTxRxRec->txLimitP = seat + GPN_SOCK_FD_RETX_MAX;
				pfdTxRxRec->txLimitP %= pfdTxRxRec->txHash;
				break;
			}
		}
	}
	GPN_SELECT_PRINT(GPN_SELECT_CUP, "after AckRxProc:nowTxP %d fifoSeat %08x waitAckP %d index %d\n\r",\
		pfdTxRxRec->nowTxP,\
		pfdTxRxRec->stFdTx[pfdTxRxRec->nowTxP].FifoSeat,\
		pfdTxRxRec->waitAckP,pfdTxRxRec->lastTx);
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockTxFifoRxAckCheck(stGpnFdTxRxRec *pfdTxRxRec)
{
	UINT32 seat;
	UINT32 dog;
	
	/*one second one called*/
	seat = pfdTxRxRec->waitAckP;

	GPN_SELECT_PRINT(GPN_SELECT_CUP, "befor RxAckCheck:waitAckP %d seat %d waitPCmd %08x waitPTxSta %d waitPRxAckSta %d waitFifoSeat %08x waitCount %d index %d\n\r",\
		pfdTxRxRec->waitAckP,seat,\
		pfdTxRxRec->stFdTx[seat].txCmdType,\
		pfdTxRxRec->stFdTx[seat].isTxOk,\
		pfdTxRxRec->stFdTx[seat].isAckRcv,\
		pfdTxRxRec->stFdTx[seat].FifoSeat,\
		pfdTxRxRec->stFdTx[seat].waitCount,\
		pfdTxRxRec->lastTx);
	if(pfdTxRxRec->stFdTx[seat].isTxOk == GPN_SELECT_GEN_YES)
	{
		if(pfdTxRxRec->stFdTx[seat].isAckRcv == GPN_SELECT_GEN_NO)
		{
			pfdTxRxRec->stFdTx[seat].waitCount++;
			if(pfdTxRxRec->stFdTx[seat].waitCount > GPN_SOCK_MSG_WAIT_ACK_CONUT)
			{
				/*retx msgs*/
				for(dog=0;dog<pfdTxRxRec->reTxNum;dog++)
				{
					GPN_SELECT_PRINT(GPN_SELECT_CUP, "in RxAckCheck:seat %d nowTxP %d fifoSeat %d index %d\n\r",\
						seat,pfdTxRxRec->nowTxP,pfdTxRxRec->stFdTx[seat].FifoSeat,pfdTxRxRec->lastTx);
					if(pfdTxRxRec->nowTxP == seat)
					{
						if(pfdTxRxRec->stFdTx[seat].FifoSeat != GPN_SELECT_GEN_FILLED)
						{
							pfdTxRxRec->stFdTx[seat].isTxOk = GPN_SELECT_GEN_NO;
							pfdTxRxRec->stFdTx[seat].reSendCnt = 0;
							pfdTxRxRec->stFdTx[seat].isAckRcv = GPN_SELECT_GEN_NO;
							pfdTxRxRec->stFdTx[seat].waitCount = 0;
						}
						break;
					}
					pfdTxRxRec->stFdTx[seat].isTxOk = GPN_SELECT_GEN_NO;
					pfdTxRxRec->stFdTx[seat].reSendCnt = 0;
					pfdTxRxRec->stFdTx[seat].isAckRcv = GPN_SELECT_GEN_NO;
					pfdTxRxRec->stFdTx[seat].waitCount = 0;
					
					seat++;
					seat %= pfdTxRxRec->txHash;
				}
				pfdTxRxRec->nowTxP = pfdTxRxRec->waitAckP;
			}
		}
	}
	GPN_SELECT_PRINT(GPN_SELECT_CUP, "after RxAckCheck:waitAckP %d seat %d waitPCmd %08x waitPTxSta %d waitPRxAckSta %d waitFifoSeat %08x waitCount %d index %d\n\r",\
		pfdTxRxRec->waitAckP,seat,\
		pfdTxRxRec->stFdTx[seat].txCmdType,\
		pfdTxRxRec->stFdTx[seat].isTxOk,\
		pfdTxRxRec->stFdTx[seat].isAckRcv,\
		pfdTxRxRec->stFdTx[seat].FifoSeat,\
		pfdTxRxRec->stFdTx[seat].waitCount,\
		pfdTxRxRec->lastTx);
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockUsrMsgProcFunRegister(stSockFdSet *pstFdSet, usrMsgProc funUsrMsgProc)
{
	/*assert*/
	if( (pstFdSet == NULL) ||
		(funUsrMsgProc == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstFdSet->gpnSockUsrMsgProc = funUsrMsgProc;
	
	return GPN_SELECT_GEN_OK;
}
/*Special Messages process func*/
UINT32 gpnSockSpMsgProc1sTiming(stSockFdSet *pstFdSet, UINT32 timeMode)
{
	UINT32 i;
	UINT32 reVal;
	stGpnFdOptInfo *pfdOptInfo;
	static struct timeval  lastTime;
	struct timeval  newTime;
	/*time event process order:
	  * alive status check
	  * alive error porcess
	  * alive Txmsg process
	  * rxACK timeout process*/

	if(timeMode != GPN_SELECT_1SECOND)
	{
		/*get 1 second gap*/
		gettimeofday(&newTime, NULL);
		reVal = gpnUsecTimePassCheck(&lastTime, &newTime, DEF_GPN_TIME_USEC_IN_SEC);
		lastTime = newTime;
		if(reVal != GPN_SELECT_GEN_OK)
		{
			return GPN_SELECT_GEN_ERR;
		}
	}

#if 0
	for(i=0; i<pstFdSet->fdMaxSearchNum; i++)
	{
		if(pstFdSet->fdOptInfo[i].en == GPN_SELECT_GEN_DISABLE)
		{
			continue;
		}
		pfdOptInfo = &(pstFdSet->fdOptInfo[i]);
#else
	for(i=0; i<pstFdSet->fdRealNum; i++)
	{
		if(pstFdSet->pfdOptInfo[i]->en == GPN_SELECT_GEN_DISABLE)
		{
			continue;
		}
		pfdOptInfo = pstFdSet->pfdOptInfo[i];
#endif

		if(pfdOptInfo->fdAlive.en == GPN_SELECT_GEN_DISABLE)
		{
			continue;
		}
		
		/*rxACK timeOut check*/
		gpnSockTxFifoRxAckCheck(&(pfdOptInfo->fdTxRxRec));
		
		if( (pfdOptInfo->fdTxRxRec.txClcCheck == GPN_SELECT_GEN_YES) ||
			(pfdOptInfo->fdTxRxRec.rxClcCheck == GPN_SELECT_GEN_YES) )
		{
			/*fail tx reTx*/
			/*retx ACK*/
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "Proc1sTiming:fdOpt.wtEn txClcCheck %d rxClcCheck %d\n\r",
				pfdOptInfo->fdTxRxRec.txClcCheck,pfdOptInfo->fdTxRxRec.rxClcCheck);
			pfdOptInfo->fdOpt.wtEn = GPN_SELECT_GEN_YES;
		}
		
		if(pfdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_CLIENT)
		{
			/*when role is client,do alive check*/
		
			/*connect alive check enable*/
			gpnAliveManUintSecondProc(&(pfdOptInfo->fdAlive));
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "Proc1sTiming:fd %d sta %d toleMIs %d misCnt %d rxTimeCnt %d txTimCnt %d txFlag %d\n\r",\
				pfdOptInfo->fd,\
				pfdOptInfo->fdAlive.sta,\
				pfdOptInfo->fdAlive.toleratMis,\
				pfdOptInfo->fdAlive.misCnt,\
				pfdOptInfo->fdAlive.rxTimerCnt,\
				pfdOptInfo->fdAlive.txTimerCnt,\
				pfdOptInfo->fdAlive.txFlag);
			if(pfdOptInfo->fdAlive.sta != GPN_SELECT_GEN_OK)
			{
				GPN_SELECT_PRINT(GPN_SELECT_CUP, "gpnAliveManUintSecondProc: connect down self %s def %s new %s\n\r",\
					pfdOptInfo->fdAddr.selfAddr.sun_path,\
					pfdOptInfo->fdAddr.defConnAddr.sun_path,\
					pfdOptInfo->fdAddr.newConnAddr.sun_path);
				/*connect down*/
				if((pfdOptInfo->fdAddr.fdRole & GPN_SOCKET_ROLE_NEW_CLIENT)\
											  ==GPN_SOCKET_ROLE_NEW_CLIENT)
				{
					memset(&(pfdOptInfo->fdAddr.newConnAddr), 0, sizeof(struct sockaddr_un));
					pfdOptInfo->fdAddr.fdRole &= (~GPN_SOCKET_ROLE_ONMASK);
					pfdOptInfo->fdAddr.fdRole |= GPN_SOCKET_ROLE_OLD;
				}
				else
				{
					/*others ???*/
				}
			}
			
			/*tx alive msg*/
			if(pfdOptInfo->fdAlive.txFlag == GPN_SELECT_GEN_ENABLE)
			{
				gpnSockSpMsgTxKeepAlive(pstFdSet, pfdOptInfo);
				pfdOptInfo->fdAlive.txFlag = GPN_SELECT_GEN_DISABLE;
			}
		}
	}

	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockSpMsgTx1sTiming(stSockFdSet *pstFdSet)
{
	INT32 reVal;
	static UINT32 tickNum = 0;
	/*msgHead(64)*/
	gpnSockMsg timerMsg;
	stGpnFdOptInfo *pstFdRealOpt;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	if(pstFdSet->fdOptInfo[GPN_COMMM_TIMER_SEV].en == GPN_SELECT_GEN_ENABLE)
	{		
		pstFdRealOpt = &(pstFdSet->fdOptInfo[GPN_COMMM_TIMER_SEV]);

		timerMsg.iIndex = 0;
		timerMsg.iMsgCtrl = 0;
		timerMsg.iDstId = GPN_COMMM_TIMER_SEV;
		timerMsg.iSrcId = pstFdSet->localSrc;
		timerMsg.iMsgType = GPN_GEN_MSG_TIMER_1S;
		timerMsg.iMsgPara1 = GPN_GEN_MSG_TIMER_1S;
		timerMsg.iMsgPara2 = tickNum++;
		timerMsg.msgCellLen = 0;

		reVal = gpnSocketUseSendTo(pstFdRealOpt, &timerMsg, GPN_SOCK_MSG_HEAD_BLEN);

		if(reVal == GPN_SELECT_GEN_ERR)
		{
			perror("alarm-tick(1s)-sendto");
			printf("alarm-tick-num: %d\n\r", tickNum);

			GPN_SELECT_PRINT(GPN_SELECT_SVP, "fd-%08x-%s\n\r",\
				pstFdRealOpt->fd,\
				pstFdRealOpt->fdAddr.defConnAddr.sun_path);
		}
		else
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "1s timer pstFdRealOpt->fdAddr.defConnAddr %s %d\n\r",\
				pstFdRealOpt->fdAddr.defConnAddr.sun_path,sizeof(pstFdRealOpt->fdAddr.defConnAddr));
			
			return GPN_SELECT_GEN_OK;
		}
	}

	return GPN_SELECT_GEN_ERR;
	
}
UINT32 gpnSockSpMsgTxBaseTiming(stSockFdSet *pstFdSet)
{
	INT32 reVal;
	static UINT32 tickNum = 0;
	/*msgHead(64)*/
	gpnSockMsg timerMsg;
	stGpnFdOptInfo *pstFdRealOpt;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	if(pstFdSet->fdOptInfo[GPN_COMMM_TIMER_SEV].en == GPN_SELECT_GEN_ENABLE)
	{		
		pstFdRealOpt = &(pstFdSet->fdOptInfo[GPN_COMMM_TIMER_SEV]);

		timerMsg.iIndex = 0;
		timerMsg.iMsgCtrl = 0;
		timerMsg.iDstId = GPN_COMMM_TIMER_SEV;
		timerMsg.iSrcId = pstFdSet->localSrc;
		timerMsg.iMsgType = GPN_GEN_MSG_TIMER_BASE;
		timerMsg.iMsgPara1 = GPN_GEN_MSG_TIMER_BASE;
		timerMsg.iMsgPara2 = tickNum++;
		timerMsg.msgCellLen = 0;

		reVal = gpnSocketUseSendTo(pstFdRealOpt, &timerMsg, GPN_SOCK_MSG_HEAD_BLEN);

		/*test*/
		if(reVal == GPN_SELECT_GEN_ERR)
		{
			perror("gpn-timer-tick(base)-sendto");
			printf("gpn-timer-tick(base)-num: %d\n\r", tickNum);

			GPN_SELECT_PRINT(GPN_SELECT_SVP, "fd-%08x-%s\n\r",\
				pstFdRealOpt->fd,\
				pstFdRealOpt->fdAddr.defConnAddr.sun_path);
		}
		else
		{
			GPN_SELECT_PRINT(GPN_SELECT_CUP, "base timer pstFdRealOpt->fdAddr.defConnAddr %s %d\n\r",\
				pstFdRealOpt->fdAddr.defConnAddr.sun_path,sizeof(pstFdRealOpt->fdAddr.defConnAddr));
			
			return GPN_SELECT_GEN_OK;
		}
	}

	return GPN_SELECT_GEN_ERR;
	
}

UINT32 gpnSockSpMsgTxKeepAlive(stSockFdSet *pstFdSet, stGpnFdOptInfo *pstFdRealOpt)
{
	UINT32 reVal;
	/*msgHead(64)*/
	gpnSockMsg aliveMsg;

	/*assert*/
	if( (pstFdSet == NULL) ||\
		(pstFdRealOpt == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	if(pstFdRealOpt->fdAddr.fdRole & GPN_SOCKET_ROLE_SERVER)
	{
		aliveMsg.iMsgType = GPN_SELF_MSG_COMM_CHECK;
	}
	else
	{
		aliveMsg.iMsgType = GPN_SELF_MSG_COMM_CHECK_RSP;
	}

	aliveMsg.iIndex = 0;
	aliveMsg.iMsgCtrl = 0;
	aliveMsg.iDstId = pstFdRealOpt->localDst;
	aliveMsg.iSrcId = pstFdSet->localSrc;
	aliveMsg.msgCellLen = 0;

	reVal = gpnSocketUseSendTo(pstFdRealOpt, &aliveMsg, GPN_SOCK_MSG_HEAD_BLEN);
	if(reVal == GPN_SELECT_GEN_ERR)
	{
		return GPN_SELECT_GEN_ERR;
	}
	else
	{
		return GPN_SELECT_GEN_OK;
	}
}


UINT32 gpnSockSpMsgProcNewConnNotifyPartner(stGpnFdOptInfo *pstFdRealOpt, gpnSockMsg *msgBuff, UINT32 len)
{
	/*assert*/
	if( (pstFdRealOpt == NULL) ||
		(msgBuff == NULL) ||
		(len != GPN_SP_MSG_NEW_NOTIFY_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	if(pstFdRealOpt->localDst == msgBuff->iSrcId)
	{
		memcpy((void *)(&(pstFdRealOpt->fdAddr.newConnAddr)),\
			(void *)((UINT8 *)msgBuff + GPN_SOCK_MSG_HEAD_BLEN), GPN_SOCK_ADDR_MAX_LEN);

		pstFdRealOpt->fdAddr.fdRole &= (~GPN_SOCKET_ROLE_ONMASK);
		pstFdRealOpt->fdAddr.fdRole |= GPN_SOCKET_ROLE_NEW;
	}
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockProcName2ModuId(stSockFdSet *pstFdSet, char *pPName, UINT32 *moduId)
{
	/*assert*/
	if( (pstFdSet == NULL) ||\
		(pPName == NULL) ||\
		(moduId == NULL) )
	{
		GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s : porint NULL, err!\n\r",\
			__FUNCTION__);
		return GPN_SELECT_GEN_ERR;
	}
	
	if(gpnSockRoleIdGet(pPName, moduId) != GPN_SOCK_COMM_GEN_OK)
	{
		GPN_SELECT_PRINT(GPN_SELECT_AGP, "%s : \"%s\" not means a process!\n\r",\
			__FUNCTION__, pPName);
		return GPN_SELECT_GEN_ERR;
	}

	GPN_SELECT_PRINT(GPN_SELECT_CMP, "%s : \"%s\"'s COMM ID is %d!\n\r",\
		__FUNCTION__, pPName, *moduId);
	
	return GPN_SOCK_COMM_GEN_OK;
}

extern sockCommData gSockCommData;

void debugGpnSockDisplayGlobalFdSet(void)
{
	UINT32 i;
	UINT32 selfIp;
	UINT32 defIp;
	UINT32 newIp;
	char *proleName;
	stSockFdSet *pstFdSet;
	char seatName[64];

	pstFdSet = &(gSockCommData.gstFdSet);
	
	printf("##### display gSockCommData info #####\n\r");
	printf("fdMaxSearchNum %d fdMaxSpace %d maxFd %d moduId %d CPUId %d localSrc %08x\n\r",\
		pstFdSet->fdMaxSearchNum, pstFdSet->fdMaxSpace,\
		pstFdSet->maxFd, pstFdSet->moduId,\
		pstFdSet->CPUId, pstFdSet->localSrc);
	for(i=0;i<pstFdSet->fdMaxSpace;i++)
	{
		proleName = NULL;
		gpnSockRoleShortNameGet(i, &proleName, NULL);
		if(proleName != NULL)
		{
			snprintf(seatName, 63, "%s", proleName);
		}
		else
		{
			snprintf(seatName, 63, "%s", "unknow");
		}
		
		if(pstFdSet->fdOptInfo[i].en == GPN_SELECT_GEN_ENABLE)
		{
			printf("%03d seatName %s,fdRole %08x localDst 0x%08x ",\
				i, seatName,\
				pstFdSet->fdOptInfo[i].fdAddr.fdRole,\
				pstFdSet->fdOptInfo[i].localDst);
			if(pstFdSet->fdOptInfo[i].fdAddr.fdRole & GPN_SOCKET_ROLE_UNIX)
			{
				printf("fd:%08d selfAddr %s defConn %s newConn %s\n\r",\
					pstFdSet->fdOptInfo[i].fd,\
					pstFdSet->fdOptInfo[i].fdAddr.selfAddr.sun_path,\
					pstFdSet->fdOptInfo[i].fdAddr.defConnAddr.sun_path,\
					pstFdSet->fdOptInfo[i].fdAddr.newConnAddr.sun_path);
			}
			else if(pstFdSet->fdOptInfo[i].fdAddr.fdRole & GPN_SOCKET_ROLE_INET)
			{
				selfIp = ((struct sockaddr_in *)&(pstFdSet->fdOptInfo[i].fdAddr.selfAddr))->sin_addr.s_addr;
				defIp = ((struct sockaddr_in *)&(pstFdSet->fdOptInfo[i].fdAddr.defConnAddr))->sin_addr.s_addr;
				newIp = ((struct sockaddr_in *)&(pstFdSet->fdOptInfo[i].fdAddr.newConnAddr))->sin_addr.s_addr;
				printf("fd:%08d selfAddr %d.%d.%d.%d-%d defConn %d.%d.%d.%d-%d newConn %d.%d.%d.%d-%d\n\r",\
					pstFdSet->fdOptInfo[i].fd,\
					((selfIp >> 24) & 0x000000FF), ((selfIp >> 16) & 0x000000FF),\
					((selfIp >>  8) & 0x000000FF), ((selfIp >>  0) & 0x000000FF),\
					((struct sockaddr_in *)&(pstFdSet->fdOptInfo[i].fdAddr.selfAddr))->sin_port,\
					((defIp >> 24) & 0x000000FF), ((defIp >> 16) & 0x000000FF),\
					((defIp >>  8) & 0x000000FF), ((defIp >>  0) & 0x000000FF),\
					((struct sockaddr_in *)&(pstFdSet->fdOptInfo[i].fdAddr.defConnAddr))->sin_port,\
					((newIp >> 24) & 0x000000FF), ((newIp >> 16) & 0x000000FF),\
					((newIp >>  8) & 0x000000FF), ((newIp >>  0) & 0x000000FF),\
					((struct sockaddr_in *)&(pstFdSet->fdOptInfo[i].fdAddr.newConnAddr))->sin_port);
			}
			else
			{
				printf(" ---fdRole socket type err\n\r");
			}
		}
		else
		{
			printf("%03d seatName %s ---disabled\n\r",\
				i, seatName);
		}
	}

	printf("##### display real fd info #####\n\r");
	printf("fdRealNum %d\n\r",\
		pstFdSet->fdRealNum);
	for(i=0;i<pstFdSet->fdRealNum;i++)
	{
		if(pstFdSet->pfdOptInfo[i] == NULL)
		{
			printf("##### fatal err #####\n\r");
			return;
		}
		
		printf("%03d of fdRealNum %d, en %d localDst 0x%08x ",\
			i, pstFdSet->fdRealNum, pstFdSet->pfdOptInfo[i]->en,\
			pstFdSet->pfdOptInfo[i]->localDst);
		if(pstFdSet->pfdOptInfo[i]->fdAddr.fdRole & GPN_SOCKET_ROLE_UNIX)
		{
			printf("fd:%08d selfAddr %s defConn %s newConn %s\n\r",\
				pstFdSet->pfdOptInfo[i]->fd,\
				pstFdSet->pfdOptInfo[i]->fdAddr.selfAddr.sun_path,\
				pstFdSet->pfdOptInfo[i]->fdAddr.defConnAddr.sun_path,\
				pstFdSet->pfdOptInfo[i]->fdAddr.newConnAddr.sun_path);
		}
		else if(pstFdSet->pfdOptInfo[i]->fdAddr.fdRole & GPN_SOCKET_ROLE_INET)
		{
			selfIp = ((struct sockaddr_in *)&(pstFdSet->pfdOptInfo[i]->fdAddr.selfAddr))->sin_addr.s_addr;
			defIp = ((struct sockaddr_in *)&(pstFdSet->pfdOptInfo[i]->fdAddr.defConnAddr))->sin_addr.s_addr;
			newIp = ((struct sockaddr_in *)&(pstFdSet->pfdOptInfo[i]->fdAddr.newConnAddr))->sin_addr.s_addr;
			printf("fd:%08d selfAddr %d.%d.%d.%d-%d defConn %d.%d.%d.%d-%d newConn %d.%d.%d.%d-%d\n\r",\
				pstFdSet->pfdOptInfo[i]->fd,\
				((selfIp >> 24) & 0x000000FF), ((selfIp >> 16) & 0x000000FF),\
				((selfIp >>  8) & 0x000000FF), ((selfIp >>  0) & 0x000000FF),\
				((struct sockaddr_in *)&(pstFdSet->pfdOptInfo[i]->fdAddr.selfAddr))->sin_port,\
				((defIp >> 24) & 0x000000FF), ((defIp >> 16) & 0x000000FF),\
				((defIp >>  8) & 0x000000FF), ((defIp >>  0) & 0x000000FF),\
				((struct sockaddr_in *)&(pstFdSet->pfdOptInfo[i]->fdAddr.defConnAddr))->sin_port,\
				((newIp >> 24) & 0x000000FF), ((newIp >> 16) & 0x000000FF),\
				((newIp >>  8) & 0x000000FF), ((newIp >>  0) & 0x000000FF),\
				((struct sockaddr_in *)&(pstFdSet->pfdOptInfo[i]->fdAddr.newConnAddr))->sin_port);
		}
		else
		{
			printf(" ---fdRole socket type err\n\r");
		}
	}
	return;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_SELECT_OPT_C_ */
