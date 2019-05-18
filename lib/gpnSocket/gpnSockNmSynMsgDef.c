/**********************************************************
* file name: gpnSockNmSynMsgDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-12-31
* function: 
*    define details about communication between gpn_syn modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_NMSYN_MSG_DEF_C_
#define _GPN_SOCK_NMSYN_MSG_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <sys/types.h>
#include <unistd.h>

#include "socketComm/gpnSockNmSynMsgDef.h"
#include "socketComm/gpnSockCommRoleMan.h"

extern sockCommData gSockCommData;

UINT32 gpnSockNmSynMsgTxProtocolMacReqToNmaSysMgt(UINT32 slot)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_NM_STRATUP_DOWN_REQ;

	/*get master NM ???*/
	nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, GPN_SOCK_ROLE_SYSMGT);
	nmSynMsg.iSrcId = pstFdSet->localSrc;
	
	nmSynMsg.iMsgPara1 = slot;
	nmSynMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxLocalMSSGet(UINT32 dstRole)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg synMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*synMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	synMsg.iMsgCtrl = 0;
	synMsg.iMsgType = GPN_NMSYN_MSG_NM_LOCAL_MS_REQ;

	synMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, dstRole/*GPN_SOCK_ROLE_SDM*/);
	synMsg.iSrcId = pstFdSet->localSrc;
	
	synMsg.iMsgPara1 = 0;
	synMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &synMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxLocalMSSSend(UINT32 dstRole, UINT32 l_mss, UINT32 is_both)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg synMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*synMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	synMsg.iMsgCtrl = 0;
	synMsg.iMsgType = GPN_NMSYN_MSG_NM_LOCAL_MS_RSP;

	synMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, dstRole/*GPN_SOCK_ROLE_NM_SYN*/);
	synMsg.iSrcId = pstFdSet->localSrc;
	
	synMsg.iMsgPara1 = l_mss;
	synMsg.iMsgPara2 = is_both;
	synMsg.msgCellLen = 0;
	
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &synMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxPartnerMSSGet(UINT32 l_sms, UINT32 l_mid_ha, UINT32 l_in, UINT32 p_in, UINT32 delay, UINT32 in_s)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg synMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*synMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	synMsg.iMsgCtrl = 0;
	synMsg.iMsgType = GPN_NMSYN_MSG_NM_PARTNER_MS_REQ;

	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		synMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, pstFdSet->moduId);
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		synMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, pstFdSet->moduId);
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	synMsg.iSrcId = pstFdSet->localSrc;

	synMsg.iMsgPara1 = l_sms;
	synMsg.iMsgPara2 = l_mid_ha;
	synMsg.iMsgPara3 = l_in;
	synMsg.iMsgPara4 = p_in;
	synMsg.iMsgPara5 = delay;
	synMsg.iMsgPara6 = in_s;
	synMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &synMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxPartnerMSSSend(UINT32 l_sms, UINT32 l_mid_ha, UINT32 l_in, UINT32 p_in, UINT32 delay, UINT32 in_s)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg synMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*synMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	synMsg.iMsgCtrl = 0;
	synMsg.iMsgType = GPN_NMSYN_MSG_NM_PARTNER_MS_RSP;

	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		synMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, pstFdSet->moduId);
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		synMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, pstFdSet->moduId);
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	synMsg.iSrcId = pstFdSet->localSrc;
	
	synMsg.iMsgPara1 = l_sms;
	synMsg.iMsgPara2 = l_mid_ha;
	synMsg.iMsgPara3 = l_in;
	synMsg.iMsgPara4 = p_in;
	synMsg.iMsgPara5 = delay;
	synMsg.iMsgPara6 = in_s;
	synMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &synMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}


UINT32 gpnSockNmSynMsgTxNotifyBetweenNm(void *notify, UINT32 len, UINT32 sendRole)
{
	char msgBuff[GPN_NMSYN_PARA_MAX_NOTIFY_LEN];
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg *pNmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	pNmSynMsg = (gpnSockMsg *)msgBuff;
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	pNmSynMsg->iMsgCtrl = 0;
	pNmSynMsg->iMsgType = GPN_NMSYN_MSG_NOTIFY_BETWEEN_NM;

	/*get another NM ???
	printf("%s : nma %d nmb %d cpu %d\n\r",\
		__FUNCTION__, pstFdSet->NMaCPUId,\
		pstFdSet->NMbCPUId, pstFdSet->CPUId);*/
	
	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, sendRole);
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, sendRole);
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	pNmSynMsg->iSrcId = pstFdSet->localSrc;

	if(len > GPN_NMSYN_PARA_MAX_NOTIFY_PAY)
	{
		return GPN_SELECT_GEN_ERR;
	}
	pNmSynMsg->msgCellLen = len;
	memcpy((msgBuff + GPN_SOCK_MSG_HEAD_BLEN), notify, len);

	/*printf("%s : send dst %08x src %08x\n\r",\
		__FUNCTION__, pNmSynMsg->iDstId, pNmSynMsg->iSrcId);*/
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pNmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+len);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockNmSynMsgTxCoproNotifyBetweenNm(void *notify, UINT32 len)
{
	gpnSockNmSynMsgTxNotifyBetweenNm(notify, len, GPN_SOCK_ROLE_COPROC);
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockNmSynMsgTxEmdNotifyBetweenNm(void *notify, UINT32 len)
{
	gpnSockNmSynMsgTxNotifyBetweenNm(notify, len, GPN_SOCK_ROLE_SDM);
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockNmSynMsgTxXloadNotifyBetweenNm(void *notify, UINT32 len)
{
	gpnSockNmSynMsgTxNotifyBetweenNm(notify, len, GPN_SOCK_ROLE_XLOAD);
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockNmSynMsgTxImiNotifyBetweenNm(void *notify, UINT32 len)
{
	gpnSockNmSynMsgTxNotifyBetweenNm(notify, len, GPN_SOCK_ROLE_IMI);
	return GPN_SELECT_GEN_OK;
}


UINT32 gpnSockNmSynMsgTxFileTypeSynDataRegister(UINT32 wb_type, UINT32 syn_obj_type, UINT32 file_type)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_REG;

	/*get master NM ???*/
	nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_NM_SYN);
	nmSynMsg.iSrcId = pstFdSet->localSrc;
	
	nmSynMsg.iMsgPara1 = wb_type;
	nmSynMsg.iMsgPara2 = syn_obj_type;
	nmSynMsg.iMsgPara3 = file_type;
	nmSynMsg.msgCellLen = 0;
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxFileTypeSynDataChange(UINT32 wb_type, UINT32 syn_obj_type)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_CHANG;

	/*get master NM ???*/
	nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_NM_SYN);
	nmSynMsg.iSrcId = pstFdSet->localSrc;
	
	nmSynMsg.iMsgPara1 = wb_type;
	nmSynMsg.iMsgPara2 = syn_obj_type;
	nmSynMsg.iMsgPara3 = 0;
	nmSynMsg.msgCellLen = 0;
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockNmSynMsgTxDTSynDataRegister(UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index, void *syn_info, UINT32 len)
{
	char msgBuff[GPN_NMSYN_PARA_MAX_NOTIFY_LEN];
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg *pNmSynMsg;

	/* syn_info should not be NULL when Data Type */
	if( (syn_info == NULL) ||
		(len > GPN_NMSYN_PARA_MAX_NOTIFY_PAY) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	pstFdSet = &(gSockCommData.gstFdSet);
	pNmSynMsg = (gpnSockMsg *)msgBuff;
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	pNmSynMsg->iMsgCtrl = 0;
	pNmSynMsg->iMsgType = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_REG;
	
	pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_NM_SYN);
	pNmSynMsg->iSrcId = pstFdSet->localSrc;

	pNmSynMsg->iMsgPara1 = wb_type;
	pNmSynMsg->iMsgPara2 = obj_type;

	if(p_index != NULL)
	{
		pNmSynMsg->iMsgPara3 = GPN_SELECT_GEN_YES;
		pNmSynMsg->iMsgPara4 = p_index->devIndex;
		pNmSynMsg->iMsgPara5 = p_index->portIndex;
		pNmSynMsg->iMsgPara6 = p_index->portIndex3;
		pNmSynMsg->iMsgPara7 = p_index->portIndex4;
		pNmSynMsg->iMsgPara8 = p_index->portIndex5;
		pNmSynMsg->iMsgPara9 = p_index->portIndex6;
	}
	else
	{
		pNmSynMsg->iMsgPara3 = GPN_SELECT_GEN_NO;
	}
	
	pNmSynMsg->msgCellLen = len;
	memcpy((msgBuff + GPN_SOCK_MSG_HEAD_BLEN), syn_info, len);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pNmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+len);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxDTSynDataChange(UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index, void *syn_info, UINT32 len)
{
	char msgBuff[GPN_NMSYN_PARA_MAX_NOTIFY_LEN];
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg *pNmSynMsg;

	/* syn_info should not be NULL when Data Type */
	if( (syn_info == NULL) ||
		(len > GPN_NMSYN_PARA_MAX_NOTIFY_PAY) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	pstFdSet = &(gSockCommData.gstFdSet);
	pNmSynMsg = (gpnSockMsg *)msgBuff;
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	pNmSynMsg->iMsgCtrl = 0;
	pNmSynMsg->iMsgType = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_CHANG;
	
	pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_NM_SYN);
	pNmSynMsg->iSrcId = pstFdSet->localSrc;

	pNmSynMsg->iMsgPara1 = wb_type;
	pNmSynMsg->iMsgPara2 = obj_type;

	if(p_index != NULL)
	{
		pNmSynMsg->iMsgPara3 = GPN_SELECT_GEN_YES;
		pNmSynMsg->iMsgPara4 = p_index->devIndex;
		pNmSynMsg->iMsgPara5 = p_index->portIndex;
		pNmSynMsg->iMsgPara6 = p_index->portIndex3;
		pNmSynMsg->iMsgPara7 = p_index->portIndex4;
		pNmSynMsg->iMsgPara8 = p_index->portIndex5;
		pNmSynMsg->iMsgPara9 = p_index->portIndex6;
	}
	else
	{
		pNmSynMsg->iMsgPara3 = GPN_SELECT_GEN_NO;
	}
	
	pNmSynMsg->msgCellLen = len;
	memcpy((msgBuff + GPN_SOCK_MSG_HEAD_BLEN), syn_info, len);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pNmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+len);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxDTSynDataDelete(UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_DEL;
	
	nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_NM_SYN);
	nmSynMsg.iSrcId = pstFdSet->localSrc;

	nmSynMsg.iMsgPara1 = wb_type;
	nmSynMsg.iMsgPara2 = obj_type;
	if(p_index != NULL)
	{
		nmSynMsg.iMsgPara3 = GPN_SELECT_GEN_YES;
		nmSynMsg.iMsgPara4 = p_index->devIndex;
		nmSynMsg.iMsgPara5 = p_index->portIndex;
		nmSynMsg.iMsgPara6 = p_index->portIndex3;
		nmSynMsg.iMsgPara7 = p_index->portIndex4;
		nmSynMsg.iMsgPara8 = p_index->portIndex5;
		nmSynMsg.iMsgPara9 = p_index->portIndex6;
	}
	else
	{
		nmSynMsg.iMsgPara3 = GPN_SELECT_GEN_NO;
	}
	
	nmSynMsg.msgCellLen = 0;

	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+GPN_NMSYN_PARA_M5D_LENTH);*/
	
	return GPN_SELECT_GEN_OK;

}
UINT32 gpnSockNmSynMsgTxDTSynDataInitGet(UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;
	
	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_GET;
	
	nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_NM_SYN);
	nmSynMsg.iSrcId = pstFdSet->localSrc;

	nmSynMsg.iMsgPara1 = wb_type;
	nmSynMsg.iMsgPara2 = obj_type;

	if(p_index != NULL)
	{
		nmSynMsg.iMsgPara3 = GPN_SELECT_GEN_YES;
		nmSynMsg.iMsgPara4 = p_index->devIndex;
		nmSynMsg.iMsgPara5 = p_index->portIndex;
		nmSynMsg.iMsgPara6 = p_index->portIndex3;
		nmSynMsg.iMsgPara7 = p_index->portIndex4;
		nmSynMsg.iMsgPara8 = p_index->portIndex5;
		nmSynMsg.iMsgPara9 = p_index->portIndex6;
	}
	else
	{
		nmSynMsg.iMsgPara3 = GPN_SELECT_GEN_NO;
	}
	
	nmSynMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &(nmSynMsg), GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxSynNTDataChange(UINT32 dst_modu, UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index, void *syn_info, UINT32 len)
{
	char msgBuff[GPN_NMSYN_PARA_MAX_NOTIFY_LEN];
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg *pNmSynMsg;

	/* syn_info should not be NULL when Data Type */
	if( (syn_info == NULL) ||
		(len > GPN_NMSYN_PARA_MAX_NOTIFY_PAY) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	pstFdSet = &(gSockCommData.gstFdSet);
	pNmSynMsg = (gpnSockMsg *)msgBuff;
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	pNmSynMsg->iMsgCtrl = 0;
	pNmSynMsg->iMsgType = GPN_NMSYN_MSG_SYN_NT_DATA_SD_CHANG;
	
	pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, dst_modu);
	pNmSynMsg->iSrcId = pstFdSet->localSrc;

	pNmSynMsg->iMsgPara1 = wb_type;
	pNmSynMsg->iMsgPara2 = obj_type;

	if(p_index != NULL)
	{
		pNmSynMsg->iMsgPara3 = GPN_SELECT_GEN_YES;
		pNmSynMsg->iMsgPara4 = p_index->devIndex;
		pNmSynMsg->iMsgPara5 = p_index->portIndex;
		pNmSynMsg->iMsgPara6 = p_index->portIndex3;
		pNmSynMsg->iMsgPara7 = p_index->portIndex4;
		pNmSynMsg->iMsgPara8 = p_index->portIndex5;
		pNmSynMsg->iMsgPara9 = p_index->portIndex6;
	}
	else
	{
		pNmSynMsg->iMsgPara3 = GPN_SELECT_GEN_NO;
	}
	
	pNmSynMsg->msgCellLen = len;
	memcpy((msgBuff + GPN_SOCK_MSG_HEAD_BLEN), syn_info, len);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pNmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+len);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxSynNTDataDelete(UINT32 dst_modu, UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_SYN_NT_DATA_SD_DEL;
	
	nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, dst_modu);
	nmSynMsg.iSrcId = pstFdSet->localSrc;

	nmSynMsg.iMsgPara1 = wb_type;
	nmSynMsg.iMsgPara2 = obj_type;
	if(p_index != NULL)
	{
		nmSynMsg.iMsgPara3 = GPN_SELECT_GEN_YES;
		nmSynMsg.iMsgPara4 = p_index->devIndex;
		nmSynMsg.iMsgPara5 = p_index->portIndex;
		nmSynMsg.iMsgPara6 = p_index->portIndex3;
		nmSynMsg.iMsgPara7 = p_index->portIndex4;
		nmSynMsg.iMsgPara8 = p_index->portIndex5;
		nmSynMsg.iMsgPara9 = p_index->portIndex6;
	}
	else
	{
		nmSynMsg.iMsgPara3 = GPN_SELECT_GEN_NO;
	}
	
	nmSynMsg.msgCellLen = 0;

	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+GPN_NMSYN_PARA_M5D_LENTH);*/
	
	return GPN_SELECT_GEN_OK;

}

UINT32 gpnSockNmSynMsgTxPreSynLeafCompReq(UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_REQ;
	
	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, pstFdSet->moduId);
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, pstFdSet->moduId);
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	nmSynMsg.iSrcId = pstFdSet->localSrc;

	nmSynMsg.iMsgPara1 = wb_type;
	nmSynMsg.iMsgPara2 = obj_type;
	if(p_index != NULL)
	{
		nmSynMsg.iMsgPara3 = GPN_SELECT_GEN_YES;
		nmSynMsg.iMsgPara4 = p_index->devIndex;
		nmSynMsg.iMsgPara5 = p_index->portIndex;
		nmSynMsg.iMsgPara6 = p_index->portIndex3;
		nmSynMsg.iMsgPara7 = p_index->portIndex4;
		nmSynMsg.iMsgPara8 = p_index->portIndex5;
		nmSynMsg.iMsgPara9 = p_index->portIndex6;
	}
	else
	{
		nmSynMsg.iMsgPara3 = GPN_SELECT_GEN_NO;
	}
	
	nmSynMsg.msgCellLen = 0;

	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+GPN_NMSYN_PARA_M5D_LENTH);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockNmSynMsgTxPreSynLeafCompRsp(UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index, char *md5, void *syn_info, UINT32 len)
{
	char msgBuff[GPN_NMSYN_PARA_MAX_NOTIFY_LEN];
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg *pNmSynMsg;
	
	pstFdSet = &(gSockCommData.gstFdSet);
	pNmSynMsg = (gpnSockMsg *)msgBuff;
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	pNmSynMsg->iMsgCtrl = 0;
	pNmSynMsg->iMsgType = GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_RSP;
	
	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, pstFdSet->moduId);
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, pstFdSet->moduId);
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	pNmSynMsg->iSrcId = pstFdSet->localSrc;

	pNmSynMsg->iMsgPara1 = wb_type;
	pNmSynMsg->iMsgPara2 = obj_type;

	if(p_index != NULL)
	{
		pNmSynMsg->iMsgPara3 = GPN_SELECT_GEN_YES;
		pNmSynMsg->iMsgPara4 = p_index->devIndex;
		pNmSynMsg->iMsgPara5 = p_index->portIndex;
		pNmSynMsg->iMsgPara6 = p_index->portIndex3;
		pNmSynMsg->iMsgPara7 = p_index->portIndex4;
		pNmSynMsg->iMsgPara8 = p_index->portIndex5;
		pNmSynMsg->iMsgPara9 = p_index->portIndex6;
	}
	else
	{
		pNmSynMsg->iMsgPara3 = GPN_SELECT_GEN_NO;
	}

	if( (md5 == NULL) ||\
		(syn_info == NULL) )
	{
		/* local not have pre syn leaf */
		pNmSynMsg->msgCellLen = 0;
		
		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pstFdSet, pNmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	}
	else
	{
		if((GPN_NMSYN_PARA_M5D_LENTH + len) > GPN_NMSYN_PARA_MAX_NOTIFY_PAY)
		{
			return GPN_SELECT_GEN_ERR;
		}
		pNmSynMsg->msgCellLen = GPN_NMSYN_PARA_M5D_LENTH + len;
		memcpy((msgBuff + GPN_SOCK_MSG_HEAD_BLEN), md5, GPN_NMSYN_PARA_M5D_LENTH);
		memcpy((msgBuff + GPN_SOCK_MSG_HEAD_BLEN + GPN_NMSYN_PARA_M5D_LENTH), syn_info, len);

		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pstFdSet, pNmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+GPN_NMSYN_PARA_M5D_LENTH+len);
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	}
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxRealSynSend(UINT32 wb_type, UINT32 obj_type, char *md5, objLogicDesc *p_index, void *syn_info, UINT32 len)
{
	char msgBuff[GPN_NMSYN_PARA_MAX_NOTIFY_LEN];
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg *pNmSynMsg;

	/* assert */
	if( (md5 == NULL) ||\
		(syn_info == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	pstFdSet = &(gSockCommData.gstFdSet);
	pNmSynMsg = (gpnSockMsg *)msgBuff;
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	pNmSynMsg->iMsgCtrl = 0;
	pNmSynMsg->iMsgType = GPN_NMSYN_MSG_REAL_SYN_SEND;
	
	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, pstFdSet->moduId);
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, pstFdSet->moduId);
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	pNmSynMsg->iSrcId = pstFdSet->localSrc;

	pNmSynMsg->iMsgPara1 = wb_type;
	pNmSynMsg->iMsgPara2 = obj_type;

	if(p_index != NULL)
	{
		pNmSynMsg->iMsgPara3 = GPN_SELECT_GEN_YES;
		pNmSynMsg->iMsgPara4 = p_index->devIndex;
		pNmSynMsg->iMsgPara5 = p_index->portIndex;
		pNmSynMsg->iMsgPara6 = p_index->portIndex3;
		pNmSynMsg->iMsgPara7 = p_index->portIndex4;
		pNmSynMsg->iMsgPara8 = p_index->portIndex5;
		pNmSynMsg->iMsgPara9 = p_index->portIndex6;
	}
	else
	{
		pNmSynMsg->iMsgPara3 = GPN_SELECT_GEN_NO;
	}
	
	if((GPN_NMSYN_PARA_M5D_LENTH + len) > GPN_NMSYN_PARA_MAX_NOTIFY_PAY)
	{
		return GPN_SELECT_GEN_ERR;
	}
	pNmSynMsg->msgCellLen = GPN_NMSYN_PARA_M5D_LENTH + len;
	memcpy((msgBuff + GPN_SOCK_MSG_HEAD_BLEN), md5, GPN_NMSYN_PARA_M5D_LENTH);
	memcpy((msgBuff + GPN_SOCK_MSG_HEAD_BLEN + GPN_NMSYN_PARA_M5D_LENTH), syn_info, len);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pNmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+GPN_NMSYN_PARA_M5D_LENTH+len);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxPatchSynStartReq(UINT32 ask_role)
{
	UINT32 role;
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_PATCH_SYN_START_REQ;

	nmSynMsg.iSrcId = pstFdSet->localSrc;
	role = GPN_SOCK_DECOM_MODUID(ask_role);
	if(role == pstFdSet->moduId)
	{
		/* X-Module send msg to local gpn_syn */
		nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_NM_SYN);
	}
	else
	{
		/* local gpn_syn send msg to partener gpn_syn */
		if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
		{
			nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, GPN_SOCK_ROLE_NM_SYN);
		}
		else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
		{	
			nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, GPN_SOCK_ROLE_NM_SYN);
		}
		else
		{
			return GPN_SELECT_GEN_ERR;
		}
	}

	nmSynMsg.iMsgPara1 = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, role);
	
	nmSynMsg.msgCellLen = 0;
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockNmSynMsgTxPatchSynStartRsp(UINT32 ask_src, UINT32 ps_stat)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_PATCH_SYN_START_RSP;

	/*get master NM ???*/
	nmSynMsg.iDstId = ask_src;
	nmSynMsg.iSrcId = pstFdSet->localSrc;
	
	nmSynMsg.iMsgPara1 = ps_stat;

	nmSynMsg.msgCellLen = 0;
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxPatSynXNodeCompReq(UINT32 xl_type,UINT32 wb_type, UINT32 obj_type, UINT32 hash_key)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_REQ;
	
	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, pstFdSet->moduId);
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, pstFdSet->moduId);
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	nmSynMsg.iSrcId = pstFdSet->localSrc;

	nmSynMsg.iMsgPara1 = xl_type;
	nmSynMsg.iMsgPara2 = wb_type;
	nmSynMsg.iMsgPara3 = obj_type;
	nmSynMsg.iMsgPara4 = hash_key;
	
	nmSynMsg.msgCellLen = 0;

	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+GPN_NMSYN_PARA_M5D_LENTH);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxPatSynXNodeCompRsp(UINT32 xl_type,UINT32 wb_type,
	UINT32 obj_type, UINT32 hash_key, char *md5)
{
	char msgBuff[GPN_NMSYN_PARA_MAX_NOTIFY_LEN];
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg *pNmSynMsg;

	/* assert */
	if(md5 == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	pstFdSet = &(gSockCommData.gstFdSet);
	pNmSynMsg = (gpnSockMsg *)msgBuff;
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	pNmSynMsg->iMsgCtrl = 0;
	pNmSynMsg->iMsgType = GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_RSP;
	
	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, pstFdSet->moduId);
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, pstFdSet->moduId);
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	pNmSynMsg->iSrcId = pstFdSet->localSrc;

	pNmSynMsg->iMsgPara1 = xl_type;
	pNmSynMsg->iMsgPara2 = wb_type;
	pNmSynMsg->iMsgPara3 = obj_type;
	pNmSynMsg->iMsgPara4 = hash_key;
	
	pNmSynMsg->msgCellLen = GPN_NMSYN_PARA_M5D_LENTH;
	memcpy((msgBuff + GPN_SOCK_MSG_HEAD_BLEN), md5, GPN_NMSYN_PARA_M5D_LENTH);

	/*printf("%s : send dst %08x src %08x\n\r",\
		__FUNCTION__, pNmSynMsg->iDstId, pNmSynMsg->iSrcId);*/
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pNmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+GPN_NMSYN_PARA_M5D_LENTH);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+GPN_NMSYN_PARA_M5D_LENTH+len);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockNmSynMsgTxPatSynLeafCompReq(UINT32 wb_type, UINT32 obj_type, UINT32 hash_key, UINT32 leaf_order)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_REQ;
	
	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, pstFdSet->moduId);
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, pstFdSet->moduId);
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	nmSynMsg.iSrcId = pstFdSet->localSrc;

	nmSynMsg.iMsgPara1 = wb_type;
	nmSynMsg.iMsgPara2 = obj_type;
	nmSynMsg.iMsgPara3 = hash_key;
	nmSynMsg.iMsgPara4 = leaf_order;
	
	nmSynMsg.msgCellLen = 0;

	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+GPN_NMSYN_PARA_M5D_LENTH);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockNmSynMsgTxPatSynLeafCompRsp(UINT32 wb_type, UINT32 obj_type, UINT32 hash_key, objLogicDesc *p_index, char *md5, void *syn_info, UINT32 len, UINT32 leaf_order)
{
	char msgBuff[GPN_NMSYN_PARA_MAX_NOTIFY_LEN];
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg *pNmSynMsg;
	
	pstFdSet = &(gSockCommData.gstFdSet);
	pNmSynMsg = (gpnSockMsg *)msgBuff;
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	pNmSynMsg->iMsgCtrl = 0;
	pNmSynMsg->iMsgType = GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_RSP;
	
	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, pstFdSet->moduId);
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		pNmSynMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, pstFdSet->moduId);
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	pNmSynMsg->iSrcId = pstFdSet->localSrc;

	pNmSynMsg->iMsgPara1 = wb_type;
	pNmSynMsg->iMsgPara2 = obj_type;
	pNmSynMsg->iMsgPara3 = hash_key;
	pNmSynMsg->iMsgPara4 = leaf_order;

	if(p_index != NULL)
	{
		pNmSynMsg->iMsgPara5 = GPN_SELECT_GEN_YES;
		pNmSynMsg->iMsgPara6 = p_index->devIndex;
		pNmSynMsg->iMsgPara7 = p_index->portIndex;
		pNmSynMsg->iMsgPara8 = p_index->portIndex3;
		pNmSynMsg->iMsgPara9 = p_index->portIndex4;
		pNmSynMsg->iMsgParaA = p_index->portIndex5;
		pNmSynMsg->iMsgParaB = p_index->portIndex6;
	}
	else
	{
		pNmSynMsg->iMsgPara5 = GPN_SELECT_GEN_NO;
	}

	if( (md5 == NULL) ||\
		(syn_info == NULL) )
	{
		pNmSynMsg->msgCellLen = 0;
		
		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pstFdSet, pNmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	}
	else
	{
		if((GPN_NMSYN_PARA_M5D_LENTH + len) > GPN_NMSYN_PARA_MAX_NOTIFY_PAY)
		{
			return GPN_SELECT_GEN_ERR;
		}
		pNmSynMsg->msgCellLen = GPN_NMSYN_PARA_M5D_LENTH + len;
		memcpy((msgBuff + GPN_SOCK_MSG_HEAD_BLEN), md5, GPN_NMSYN_PARA_M5D_LENTH);
		memcpy((msgBuff + GPN_SOCK_MSG_HEAD_BLEN + GPN_NMSYN_PARA_M5D_LENTH), syn_info, len);

		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pstFdSet, pNmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+GPN_NMSYN_PARA_M5D_LENTH+len);
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	}
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockNmSynMsgTxAutoPatSynReq(char *top_md5)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_REQ;
	
	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, pstFdSet->moduId);
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, pstFdSet->moduId);
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	nmSynMsg.iSrcId = pstFdSet->localSrc;

	memcpy(&(nmSynMsg.iMsgPara1), top_md5, GPN_NMSYN_PARA_M5D_LENTH);
	
	nmSynMsg.msgCellLen = 0;

	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+GPN_NMSYN_PARA_M5D_LENTH);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockNmSynMsgTxAutoPatSynRsp(void)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg nmSynMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	/*nmSynMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	nmSynMsg.iMsgCtrl = 0;
	nmSynMsg.iMsgType = GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_RSP;
	
	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMbCPUId, pstFdSet->moduId);
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		nmSynMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->NMaCPUId, pstFdSet->moduId);
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	nmSynMsg.iSrcId = pstFdSet->localSrc;
	
	nmSynMsg.msgCellLen = 0;

	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &nmSynMsg, GPN_SOCK_MSG_HEAD_BLEN+GPN_NMSYN_PARA_M5D_LENTH);*/
	
	return GPN_SELECT_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SOCK_NMSYN_MSG_DEF_C_*/

