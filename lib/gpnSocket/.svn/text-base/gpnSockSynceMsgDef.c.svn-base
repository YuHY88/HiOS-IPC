/**********************************************************
* file name: gpnSockSynceMsgDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan Chengquan 2014-11-07
* function: 
*   define details about communication between synce modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_SYNCE_MSG_DEF_C_
#define _GPN_SOCK_SYNCE_MSG_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <sys/types.h>
#include <unistd.h>

#include "socketComm/gpnSockCommRoleMan.h"
#include "socketComm/gpnSockSdmMsgDef.h"
#include "socketComm/gpnSockSynceMsgDef.h"
extern sockCommData gSockCommData;

#define NM_SLOT			17

UINT32 gpnSockSYNCEMsgTxS1OfSSMReq(UINT32 isSsmEnable, UINT32 ssmSa , UINT32 slot)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg synceMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);
	
	synceMsg.iMsgCtrl = 0; /*GPN_SOCK_MSG_ACK_NEED_BIT*/
	synceMsg.iDstId = GPN_SOCK_CREAT_DIREC(NM_SLOT, GPN_SOCK_ROLE_SYSMGT);
	synceMsg.iSrcId = pgstFdSet->localSrc;
	synceMsg.iMsgType = GPN_SYNCE_MSG_SEND_LOCAL_SSM_2_SYSMGT_CMD;
	synceMsg.iMsgPara1 = isSsmEnable;
	synceMsg.iMsgPara2 = ssmSa;
	synceMsg.iMsgPara3 = slot;
	synceMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pgstFdSet, (gpnSockMsg *)(&synceMsg), GPN_SOCK_MSG_HEAD_BLEN);
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSYNCEMsgTxS1OfSSMRsp(UINT32 slot, UINT32 isOK)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg synceMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);
	
	synceMsg.iMsgCtrl = 0; /*GPN_SOCK_MSG_ACK_NEED_BIT*/
	synceMsg.iDstId = GPN_SOCK_CREAT_DIREC(NM_SLOT, GPN_SOCK_ROLE_SYSMGT);
	synceMsg.iSrcId = pgstFdSet->localSrc;
	synceMsg.iMsgType = GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_RSP;
	synceMsg.iMsgPara1 = isOK;
	synceMsg.iMsgPara2 = slot;
	synceMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pgstFdSet, (gpnSockMsg *)(&synceMsg), GPN_SOCK_MSG_HEAD_BLEN);
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSYNCEMsgTxClcSrcPortList(synceClcSrcPortList *portList)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg synceMsg;

	/* assert */
	if(portList == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	if(portList->srcPortNum > 13)
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	pgstFdSet = &(gSockCommData.gstFdSet);
	
	synceMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	
	synceMsg.iDstId = GPN_SOCK_CREAT_DIREC(NM_SLOT, GPN_SOCK_ROLE_SYSMGT);
	synceMsg.iSrcId = pgstFdSet->localSrc;
	synceMsg.iMsgType = GPN_SYNCE_SST_SEND_CLCSRC_PORT_2_SYSMGT_CMD;
	synceMsg.iMsgPara1 = pgstFdSet->CPUId;
	synceMsg.iMsgPara2 = portList->srcPortNum;
	synceMsg.iMsgPara3 = portList->srcPort1;
	synceMsg.iMsgPara4 = portList->srcPort2;
	synceMsg.iMsgPara5 = portList->srcPort3;
	synceMsg.iMsgPara6 = portList->srcPort4;
	synceMsg.iMsgPara7 = portList->srcPort5;
	synceMsg.iMsgPara8 = portList->srcPort6;
	synceMsg.iMsgPara9 = portList->srcPort7;
	synceMsg.iMsgParaA = portList->srcPort8;
	synceMsg.iMsgParaB = portList->srcPort9;
	synceMsg.iMsgParaC = portList->srcPort10;
	synceMsg.iMsgParaD = portList->srcPort11;
	synceMsg.iMsgParaE = portList->srcPort12;
	synceMsg.iMsgParaF = portList->srcPort13;
	synceMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pgstFdSet, (gpnSockMsg *)(&synceMsg), GPN_SOCK_MSG_HEAD_BLEN);
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSYSMGTMsgTxS1OfSsmReq(UINT32 slot, UINT32 loadSsmSa, UINT32 ssmSaConfig)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg synceMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);
	
	synceMsg.iMsgCtrl = 0; /*GPN_SOCK_MSG_ACK_NEED_BIT*/
	synceMsg.iDstId = GPN_SOCK_CREAT_DIREC(slot, GPN_SOCK_ROLE_SYNCED);
	synceMsg.iSrcId = pgstFdSet->localSrc;
	synceMsg.iMsgType = GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_CMD;
	synceMsg.iMsgPara1 = loadSsmSa;
	synceMsg.iMsgPara2 = ssmSaConfig;
	synceMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pgstFdSet, (gpnSockMsg *)(&synceMsg), GPN_SOCK_MSG_HEAD_BLEN);
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSYSMGTMsgTxS1OfSsmRsp(UINT32 slot, UINT32 isOK)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg synceMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);
	
	synceMsg.iMsgCtrl = 0; /*GPN_SOCK_MSG_ACK_NEED_BIT*/
	synceMsg.iDstId = GPN_SOCK_CREAT_DIREC(slot, GPN_SOCK_ROLE_SYNCED);
	synceMsg.iSrcId = pgstFdSet->localSrc;
	synceMsg.iMsgType = GPN_SYNCE_MSG_SEND_LOCAL_SSM_2_SYSMGT_RSP;
	synceMsg.iMsgPara1 = isOK;
	synceMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pgstFdSet, (gpnSockMsg *)(&synceMsg), GPN_SOCK_MSG_HEAD_BLEN);
	
	return GPN_SELECT_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SOCK_SYNCE_MSG_DEF_C_*/

