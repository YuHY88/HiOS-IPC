/**********************************************************
* file name: gpnSelectOpt.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-08-16
* function: 
*    define select opt about
* modify:
*
***********************************************************/
#ifndef _GPN_SELECT_OPT_H_
#define _GPN_SELECT_OPT_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <sys/un.h>
#include <netinet/in.h>

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnSockMsgDef.h"
#include "lib/gpnSocket/socketComm/gpnSockAliveMan.h"
	
#define GPN_SELECT_GEN_OK 					GPN_SOCK_SYS_OK
#define GPN_SELECT_GEN_ERR 					GPN_SOCK_SYS_ERR

#define GPN_SELECT_GEN_YES 					GPN_SOCK_SYS_YES
#define GPN_SELECT_GEN_NO 					GPN_SOCK_SYS_NO

#define GPN_SELECT_GEN_ENABLE 				GPN_SOCK_SYS_ENABLE
#define GPN_SELECT_GEN_DISABLE 				GPN_SOCK_SYS_DISABLE

#define GPN_SELECT_PRINT(level, info...) 	GEN_SOCK_SYS_PRINT((level), info)
#define GPN_SELECT_AGP 						GEN_SOCK_SYS_AGP
#define GPN_SELECT_SVP 						GEN_SOCK_SYS_SVP
#define GPN_SELECT_CMP 						GEN_SOCK_SYS_CMP
#define GPN_SELECT_CUP 						GEN_SOCK_SYS_CUP

#define GPN_SELECT_GEN_FILLED				0xeeeeeeed

#define GPN_SELECT_POLL						0
#define GPN_SELECT_1SECOND					1

#define GPN_SOCKET_ROLE_SERVER 				0x00000001
#define GPN_SOCKET_ROLE_CLIENT 				0x00000002
#define GPN_SOCKET_ROLE_ALONE				0x00000004
#define GPN_SOCKET_ROLE_SCMASK				0x0000000F

#define GPN_SOCKET_ROLE_UNIX 				0x00000010
#define GPN_SOCKET_ROLE_INET 				0x00000020
#define GPN_SOCKET_ROLE_UIMASK				0x000000F0

#define GPN_SOCKET_ROLE_TCP 				0x00000100
#define GPN_SOCKET_ROLE_UDP 				0x00000200
#define GPN_SOCKET_ROLE_TUMASK				0x00000F00

#define GPN_SOCKET_ROLE_OLD					0x00001000
#define GPN_SOCKET_ROLE_NEW					0x00002000
#define GPN_SOCKET_ROLE_ONMASK				0x0000F000

#define GPN_SOCKET_ROLE_TIMER				0x80000000
#define GPN_SOCKET_ROLE_AGENT				0x40000000

#define GPN_SOCKET_ROLE_OLD_SERVER			(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_ROLE_OLD_CLIENT			(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_CLIENT)
#define GPN_SOCKET_ROLE_NEW_SERVER			(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_ROLE_NEW_CLIENT			(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_CLIENT)

#define GPN_SOCKET_ROLE_OLD_UDP				(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_UDP)
#define GPN_SOCKET_ROLE_OLD_TCP				(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_TCP)
#define GPN_SOCKET_ROLE_NEW_UDP				(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_UDP)
#define GPN_SOCKET_ROLE_NEW_TCP				(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_TCP)

#define GPN_SOCKET_ROLE_OLD_UDP_UNIX		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_UNIX)
#define GPN_SOCKET_ROLE_OLD_TCP_UNIX		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_UNIX)
#define GPN_SOCKET_ROLE_NEW_UDP_UNIX		(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_UNIX)
#define GPN_SOCKET_ROLE_NEW_TCP_UNIX		(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_UNIX)
#define GPN_SOCKET_ROLE_OLD_UDP_INET		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_INET)
#define GPN_SOCKET_ROLE_OLD_TCP_INET		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_INET)
#define GPN_SOCKET_ROLE_NEW_UDP_INET		(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_INET)
#define GPN_SOCKET_ROLE_NEW_TCP_INET		(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_INET)

#define GPN_SOCKET_OLD_UDP_SERVER			(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_OLD_UDP_CLIENT			(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_CLIENT)
#define GPN_SOCKET_NEW_UDP_SERVER			(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_NEW_UDP_CLIENT			(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_CLIENT)

#define GPN_SOCKET_OLD_TCP_SERVER			(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_OLD_TCP_CLIENT			(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_CLIENT)
#define GPN_SOCKET_NEW_TCP_SERVER			(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_NEW_TCP_CLIENT			(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_CLIENT)

#define GPN_SOCKET_OLD_UDP_UNIX_SER 		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_UNIX|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_NEW_UDP_UNIX_SER 		(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_UNIX|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_OLD_UDP_INET_SER 		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_INET|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_NEW_UDP_INET_SER 		(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_INET|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_OLD_UDP_UNIX_CLI 		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_UNIX|GPN_SOCKET_ROLE_CLIENT)
#define GPN_SOCKET_NEW_UDP_UNIX_CLI 		(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_UNIX|GPN_SOCKET_ROLE_CLIENT)
#define GPN_SOCKET_OLD_UDP_INET_CLI 		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_INET|GPN_SOCKET_ROLE_CLIENT)
#define GPN_SOCKET_NEW_UDP_INET_CLI 		(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_INET|GPN_SOCKET_ROLE_CLIENT)
#define GPN_SOCKET_OLD_UDP_UNIX_ALONE		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_UDP|GPN_SOCKET_ROLE_UNIX|GPN_SOCKET_ROLE_ALONE)

#define GPN_SOCKET_OLD_TCP_UNIX_SER 		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_UNIX|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_NEW_TCP_UNIX_SER 		(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_UNIX|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_OLD_TCP_INET_SER 		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_INET|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_NEW_TCP_INET_SER 		(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_INET|GPN_SOCKET_ROLE_SERVER)
#define GPN_SOCKET_OLD_TCP_UNIX_CLI 		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_UNIX|GPN_SOCKET_ROLE_CLIENT)
#define GPN_SOCKET_NEW_TCP_UNIX_CLI 		(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_UNIX|GPN_SOCKET_ROLE_CLIENT)
#define GPN_SOCKET_OLD_TCP_INET_CLI 		(GPN_SOCKET_ROLE_OLD|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_INET|GPN_SOCKET_ROLE_CLIENT)
#define GPN_SOCKET_NEW_TCP_INET_CLI 		(GPN_SOCKET_ROLE_NEW|GPN_SOCKET_ROLE_TCP|GPN_SOCKET_ROLE_INET|GPN_SOCKET_ROLE_CLIENT)

#define GPN_SOCKET_OLD_UDP_UNIX_ALONE_TIMER (GPN_SOCKET_OLD_UDP_UNIX_ALONE|GPN_SOCKET_ROLE_TIMER)
#define GPN_SOCKET_OLD_TCP_UNIX_SER_TIMER 	(GPN_SOCKET_OLD_TCP_UNIX_SER|GPN_SOCKET_ROLE_TIMER)
#define GPN_SOCKET_OLD_TCP_INET_CLI_AGENT   (GPN_SOCKET_OLD_TCP_INET_CLI|GPN_SOCKET_ROLE_AGENT)

#define GPN_SOCK_RX_BUFF_BYTE_SIZE			1280
#define GPN_SOCK_RX_BUFF_DWORD_SIZE			(GPN_SOCK_RX_BUFF_BYTE_SIZE/4)
#define	GPN_SOCK_FDSET_MAX					128
#define	GPN_SOCK_FD_TX_MAX					10
#define	GPN_SOCK_FD_RETX_MAX				5
#define	GPN_SOCK_FD_RX_MAX					12

#define	GPN_SOCK_MSG_RETX_TRY				3
#define	GPN_SOCK_MSG_WAIT_ACK_CONUT			3

#define	GPN_SOCK_HEAD_FIFO_NUM				200
#define	GPN_SOCK_HEAD_FIFO_DWORD_SIZE		GPN_SOCK_MSG_HEAD_LEN
#define	GPN_SOCK_HEAD_FIFO_BYTE_SIZE		GPN_SOCK_MSG_HEAD_BLEN
#define	GPN_SOCK_PAYLOAD_FIFO_NUM			20
#define	GPN_SOCK_PAYLOAD_FIFO_DWORD_SIZE	1024
#define	GPN_SOCK_PAYLOAD_FIFO_BYTE_SIZE		(GPN_SOCK_PAYLOAD_FIFO_DWORD_SIZE*4)
#define GPN_SOCK_MAX_MSG_DWORD_LEN			(GPN_SOCK_HEAD_FIFO_DWORD_SIZE+GPN_SOCK_PAYLOAD_FIFO_DWORD_SIZE)
#define GPN_SOCK_MAX_MSG_BYTE_LEN			(GPN_SOCK_HEAD_FIFO_BYTE_SIZE+GPN_SOCK_PAYLOAD_FIFO_BYTE_SIZE)

/*define for special msgs*/
#define GPN_SOCK_ADDR_LEN					16			
#define GPN_SOCK_ADDR_UN_LEN				110
#define GPN_SOCK_ADDR_IN_LEN				16
#define GPN_SOCK_ADDR_MAX_LEN               GPN_SOCK_ADDR_UN_LEN
#define GPN_SP_MSG_NEW_NOTIFY_LEN			(GPN_SOCK_MSG_HEAD_BLEN + GPN_SOCK_ADDR_UN_LEN)

typedef struct _stGpnFdAddr_
{
	UINT32 fdRole;
	UINT32 saLen;
	UINT32 daLen;
	UINT32 naLen;
	struct sockaddr_un selfAddr;
	struct sockaddr_un defConnAddr;
	struct sockaddr_un newConnAddr;

}stGpnFdAddr;

typedef struct _stGpnFdOptMode_
{
	UINT8 rdEn;
	UINT8 wtEn;
	UINT8 exEn;
	UINT8 rsd;
}stFdOptMod;

typedef struct _stFdTxRec_
{
	UINT32 FifoSeat;
	UINT32 txCmdIndex;
	UINT32 txCmdType;
	UINT8 reSendCnt;
	UINT8 isTxOk;
	UINT8 waitCount;
	UINT8 isAckRcv;
}stFdTxRec;

typedef struct _stFdRxRec_
{
	UINT32 rxCmdIndex;
	UINT32 rxCmdType;
	UINT32 CmdDst;
	UINT32 CmdSrc;
	UINT8 isMsgRxOk;
	UINT8 isAckSend;
	UINT8 reSendCnt;
	UINT8 rsd1;

}stFdRxRec;

typedef struct _stGpnFdTxRxRec_
{	
	UINT32 txClcCheck;
	UINT32 rxClcCheck;
	
	UINT32 txLosNum;
	UINT32 rxAckLos;
	
	UINT32 txHash;
	UINT32 lastTx;
	stFdTxRec stFdTx[GPN_SOCK_FD_TX_MAX];
	UINT32 reTxNum;
	UINT32 waitAckP;
	UINT32 nowTxP;
	UINT32 txLimitP;

	UINT32 rxLosNum;
	UINT32 txAckLos;
	
	UINT32 rxHash;
	UINT32 lastRx;
	stFdRxRec stFdRx[GPN_SOCK_FD_RX_MAX];
}stGpnFdTxRxRec;

typedef struct _stGpnFdOptInfo_
{
	INT32 fd;
	UINT32 en;
	UINT32 localDst;
	stFdOptMod fdOpt;
	stGpnFdAddr fdAddr;
	stGpnFdTxRxRec fdTxRxRec;

	stAliveInfo fdAlive;
}stGpnFdOptInfo;

typedef struct 
{
	UINT32 maxBloc;
	UINT32 idleBloc;
	UINT32 resourc[GPN_SOCK_HEAD_FIFO_NUM];
	UINT32 seatPosit[GPN_SOCK_HEAD_FIFO_NUM];
	UINT32 payloadSeat[GPN_SOCK_HEAD_FIFO_NUM];
	UINT32 payloadSize[GPN_SOCK_HEAD_FIFO_NUM];
	UINT32 fifo[GPN_SOCK_HEAD_FIFO_NUM][GPN_SOCK_HEAD_FIFO_DWORD_SIZE];

	UINT32 maxCellB;
	UINT32 idleCellB;
	UINT32 cellRsc[GPN_SOCK_PAYLOAD_FIFO_NUM];
	UINT32 cellSeatP[GPN_SOCK_PAYLOAD_FIFO_NUM];
	UINT32 cellFifo[GPN_SOCK_PAYLOAD_FIFO_NUM][GPN_SOCK_PAYLOAD_FIFO_DWORD_SIZE];
}stGpnFdCommFifo;

typedef UINT32 (*usrMsgProc)(gpnSockMsg*, UINT32);

typedef struct _stGpnAlmSockFdSet_
{
	/*selecet about */
	UINT32 fdRealNum;
	UINT32 fdMaxSearchNum;
	UINT32 fdMaxSpace;
	INT32 maxFd;

	/*local msg src/dst about */
	UINT32 moduId;
	UINT32 CPUId;
	UINT32 localSrc;

	/*assist for nm-syn msg comm*/
	UINT32 NMaCPUId;
	UINT32 NMbCPUId;

	/* roler's pid */
	pid_t pid;

	/*fd info buff */
	stGpnFdOptInfo *pfdOptInfo[GPN_SOCK_FDSET_MAX];
	stGpnFdOptInfo fdOptInfo[GPN_SOCK_FDSET_MAX];

	/*common tx msg fifo*/
	stGpnFdCommFifo txMsgFifo;

	/*call back: usr messages process*/
	usrMsgProc gpnSockUsrMsgProc;
}stSockFdSet;

/*function pre-def*/
UINT32 gpnSockSelectOptGlobInit(stSockFdSet *pstFdSet);
UINT32 gpnSockSelectFdtoAlineFdSet(stSockFdSet *pstFdSet, stGpnFdOptInfo *pfdOptInfo);
UINT32 gpnSockSelectFdSeatGet(UINT32 selfCPUId, UINT32 CPUId, UINT32 moduId, UINT32 *pfdSeat);
UINT32 gpnSockSelectFdtoFdSet(stSockFdSet *pstFdSet, INT32 fd, UINT32 fdSeat, stGpnFdAddr *pstFdAddr);
UINT32 gpnSockCommSelectMainProc(stSockFdSet *pstFdSet,struct timeval *timeout);
UINT32 gpnSockFdReadProc(stSockFdSet *pstFdSet, stGpnFdOptInfo *pstFdOptInfo);
UINT32 *gpnSockMsgDecompose(UINT32 *rxBuff, UINT32 *pmsgLen);
UINT32 gpnSockMsgRxProc(stSockFdSet *pstFdSet, stGpnFdOptInfo *pstFdOptInfo, struct sockaddr *farend, UINT32 addLen, UINT32 *msgBuff, UINT32 msgLen);
stGpnFdOptInfo *gpnSockNewConnect(stSockFdSet *pstFdSet, UINT32 msgSrc, struct sockaddr *farend, UINT32 addLen);
UINT32 gpnSockMsgRxACKTxProc(stGpnFdOptInfo *pstFdOptInfo, UINT32 *msgBuff);
UINT32 gpnSockACKRxProc(stSockFdSet *pstFdSet, stGpnFdTxRxRec *pstfdTxRxRec, UINT32 *msgBuff);
UINT32 gpnSockAckTxPorc(stGpnFdOptInfo *pstFdOptInfo, stGpnFdTxRxRec *pFdTxRxRec);
UINT32 gpnSockMsgTxPorc(stSockFdSet *pstFdSet, stGpnFdOptInfo *pstFdOptInfo);
UINT32 gpnSockTransferModuMsgProc(stSockFdSet *pstFdSet, gpnSockMsg *msgBuff, UINT32 len);
UINT32 gpnSockWriteEnProc(stSockFdSet *pstFdSet, stGpnFdOptInfo *pstFdOptInfo);
UINT32 gpnSockNoGuaranteeMsgTxProc(stSockFdSet *pstFdSet, gpnSockMsg *msgBuff, UINT32 byteLen);
UINT32 gpnSocketUseSendTo(stGpnFdOptInfo *pstFdOptInfo, void *txCmd, UINT32 byteLen);
UINT32 gpnSockGuaranteeMsgTxProc(stSockFdSet *pstFdSet, gpnSockMsg *msgBuff, UINT32 len);
UINT32 gpnSockFdSetCommFifoInit(stGpnFdCommFifo *ptxFifo);
UINT32 gpnSockFdSetCommFifoAbtain(stGpnFdCommFifo *ptxFifo, UINT32 *msgBuff, UINT32 byteLen, UINT32 *pseat);
UINT32 gpnSockFdSetCommFifoOut(stGpnFdCommFifo *ptxFifo, UINT32 seat, UINT32 *msgBuff, UINT32 *buffSize);
UINT32 gpnSockFdSetCommFifoRelease(stGpnFdCommFifo *ptxFifo, UINT32 seat);
UINT32 gpnSockTxFifoMsgTxPreProc(stSockFdSet *pstFdSet, stGpnFdTxRxRec *pfdTxRxRec, UINT32 msgType, UINT32 gSeat);
UINT32 gpnSockTxFifoMsgTxRenewProc(stSockFdSet *pstFdSet, stGpnFdTxRxRec *pfdTxRxRec, UINT32 txResult);
UINT32 gpnSockTxFifoAckRxProc(stSockFdSet *pstFdSet, stGpnFdTxRxRec *pfdTxRxRec);
UINT32 gpnSockTxFifoRxAckCheck(stGpnFdTxRxRec *pfdTxRxRec);
UINT32 gpnSockUsrMsgProcFunRegister(stSockFdSet *pstFdSet, usrMsgProc funUsrMsgProc);
UINT32 gpnSockSpMsgTx1sTiming(stSockFdSet *pstFdSet);
UINT32 gpnSockSpMsgTxBaseTiming(stSockFdSet *pstFdSet);
UINT32 gpnSockSpMsgProc1sTiming(stSockFdSet *pstFdSet, UINT32 timeMode);
UINT32 gpnSockSpMsgProcNewConnNotifyPartner(stGpnFdOptInfo *pstFdRealOpt, gpnSockMsg *msgBuff, UINT32 len);
UINT32 gpnSockSpMsgTxKeepAlive(stSockFdSet *pstFdSet, stGpnFdOptInfo *pstFdRealOpt);
UINT32 gpnSockProcName2ModuId(stSockFdSet *pstFdSet, char *pPName, UINT32 *moduId);
void debugGpnSockDisplayGlobalFdSet(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SELECT_OPT_H_*/

