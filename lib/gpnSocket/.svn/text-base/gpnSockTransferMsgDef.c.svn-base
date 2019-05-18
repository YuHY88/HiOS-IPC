/**********************************************************
* file name: gpnSockTransferMsgDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-25
* function: 
*    define details about communication between gpn_transfer modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_TRANSFER_MSG_DEF_C_
#define _GPN_SOCK_TRANSFER_MSG_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <sys/types.h>
#include <unistd.h>

#include "socketComm/gpnSockCommRoleMan.h"
#include "socketComm/gpnSockTransferMsgDef.h"
#include "socketComm/gpnSockCommModuDef.h"
#include "devCoreInfo/gpnDevCoreInfoFunApi.h"

extern sockCommData gSockCommData;

void gpnSockSpMsgTxCPUCommTest(UINT32 dstRole, UINT32 dstSlot)
{
	/*msgHead(64)*/
	gpnSockMsg commTestMsg;
	//stSockFdSet *pstFdSet;

	//pstFdSet = &(gSockCommData.gstFdSet);

	commTestMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	commTestMsg.iDstId = GPN_SOCK_CREAT_DIREC(dstSlot, dstRole);
	commTestMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	commTestMsg.iMsgType = GPN_TRANS_MSG_CPU_COMM_TEST;
	commTestMsg.iMsgPara1 = gSockCommData.gstFdSet.CPUId;
	commTestMsg.iMsgPara2 = dstSlot;
	commTestMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &commTestMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return;
}
void gpnSockSpMsgTxNMCommTest(UINT32 dstRole, UINT32 rsv1)
{
	UINT32 self_slot;
	UINT32 nma_slot;
	UINT32 nmb_slot;
	
	self_slot = gpnDevApiGetSelfSoltNum();
 	gpnDevApiGetNMxSoltNum(&nma_slot, &nmb_slot);
	
	if(self_slot == nma_slot)
	{
		gpnSockSpMsgTxCPUCommTest(dstRole, nmb_slot);
		
		printf("send comm test to nm slot %d role %d\n\r", nmb_slot, dstRole);
	}
	else if(self_slot == nmb_slot)
	{
		gpnSockSpMsgTxCPUCommTest(dstRole, nma_slot);
		
		printf("send comm test to nm slot %d role %d\n\r", nma_slot, dstRole);
	}

	return;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SOCK_TRANSFER_MSG_DEF_C_*/

