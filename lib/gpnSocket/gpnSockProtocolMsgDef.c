/**********************************************************
* file name: gpnSockProtocolMsgDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-06-11
* function: 
*    define details about gpn_socket protocol msg
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_PROTOCOL_MSG_DEF_C_
#define _GPN_SOCK_PROTOCOL_MSG_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <sys/types.h>
#include <unistd.h>

#include "socketComm/gpnSockProtocolMsgDef.h"
#include "socketComm/gpnSockCommModuDef.h"
#include "socketComm/gpnSockCommRoleMan.h"


UINT32 gpnSockSpMsgTxNewConnNotifyPartner(stSockFdSet *pstFdSet, UINT32 dstId, void *newConnAddr)
{
	/*msgHead(64)+sockaddr_un(110)*/
	UINT8 msgBuff[GPN_SP_MSG_NEW_NOTIFY_LEN];
	gpnSockMsg *pgpnMsg;

	/*assert*/
	if( (pstFdSet == NULL) ||\
		(newConnAddr == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pgpnMsg = (gpnSockMsg *)msgBuff;
	pgpnMsg->iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	pgpnMsg->iMsgType = GPN_SELF_MSG_NEW_CONNCET_NOTIFY;
	
	pgpnMsg->iDstId = dstId;
	pgpnMsg->iSrcId = pstFdSet->localSrc;
	
	pgpnMsg->msgCellLen = GPN_SOCK_ADDR_MAX_LEN;/*sizeof(sockaddr_un),max sockaddr lenth*/
	memcpy((msgBuff+GPN_SOCK_MSG_HEAD_BLEN), newConnAddr, GPN_SOCK_ADDR_MAX_LEN);
	
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pgpnMsg, GPN_SP_MSG_NEW_NOTIFY_LEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, (gpnSockMsg *)msgBuff, GPN_SP_MSG_NEW_NOTIFY_LEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSpMsgTxBaseTimerSyn2ValidModule(stSockFdSet *pstFdSet, INT32 oldTime, INT32 newTime)
{
	UINT32 i;
	
	for(i=GPN_SOCK_ROLE_TRANS;i<(GPN_SOCK_INS_ROLE_ID_MAX+1);i++)
	{
		if(gpnSockRoleModuValidCheck(i) == GPN_SELECT_GEN_OK)
		{
			gpnSockSpMsgTxBaseTimerSyn2OtherModule(pstFdSet, i, oldTime, newTime);
		}
	}

	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSpMsgTxBaseTimerSyn2OtherModule(stSockFdSet *pstFdSet, UINT32 MODU_ID, INT32 oldTime, INT32 newTime)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_SELF_MSG_BASE_TIMER_SYN_OPT;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, MODU_ID);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;

	cfgMgtMsg.iMsgPara1 = oldTime;
	cfgMgtMsg.iMsgPara2 = newTime;
	
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SOCK_PROTOCOL_MSG_DEF_C_*/

