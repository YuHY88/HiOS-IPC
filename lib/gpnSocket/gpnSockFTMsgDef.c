/**********************************************************
* file name: gpnSockFTMsgDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-05-30
* function: 
*    define details about communication between gpn_ft modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_FT_MSG_DEF_C_
#define _GPN_SOCK_FT_MSG_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <sys/types.h>
#include <unistd.h>

#include "socketComm/gpnSockFTMsgDef.h"
#include "socketComm/gpnSockCommModuDef.h"
#include "socketComm/gpnSockCommRoleMan.h"

/*socket communication global data:define in socket communication module*/
extern sockCommData gSockCommData;

/****************************************************************************************/
/*                               REQ  API                                                                                                             */
/****************************************************************************************/
UINT32 gpnSockFTMsgFileSynReqCreat(UINT32 fileType)
{
	UINT32 src_slot;
	UINT32 dst_slot;
	stSockFdSet *pstFdSet;
	
	pstFdSet = &(gSockCommData.gstFdSet);

	if(pstFdSet->NMaCPUId == pstFdSet->CPUId)
	{
		src_slot = pstFdSet->NMbCPUId;
		dst_slot = pstFdSet->NMaCPUId;
	}
	else if(pstFdSet->NMbCPUId == pstFdSet->CPUId)
	{	
		src_slot = pstFdSet->NMaCPUId;
		dst_slot = pstFdSet->NMbCPUId;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	gpnSockFTMsgFtReqCreat(pstFdSet, fileType, src_slot, dst_slot);
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtReqCreatNew(UINT32 fileType, UINT32 src, UINT32 dst)
{
	gpnSockFTMsgFtReqCreat(&(gSockCommData.gstFdSet), fileType, src, dst);
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtReqCreat(stSockFdSet *pstFdSet, UINT32 fileType, UINT32 src, UINT32 dst)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd local CUP, so..., and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_TRANS);
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_FT_MSG_FILE_TRANS_REQ_CREAT;
	
	FTMsg.iMsgPara1 = fileType;
	FTMsg.iMsgPara2 = src;
	FTMsg.iMsgPara3 = dst;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFTMsgFtSpReqCreatNew(UINT32 fileType,
	UINT32 src, char *srcPath, UINT32 spLen, UINT32 dst, char *dstPath, UINT32 dpLen)
{
	gpnSockFTMsgFtSpReqCreat(&(gSockCommData.gstFdSet), fileType,
		src, srcPath, spLen, dst, dstPath, dpLen);
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFTMsgFtSpReqCreat(stSockFdSet *pstFdSet,  UINT32 fileType,
	UINT32 src, char *srcPath, UINT32 spLen, UINT32 dst, char *dstPath, UINT32 dpLen)
{
	UINT8 payload[GPN_FT_PARA_SPFILE_TR_CREAT_MSG_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pFTMsg;
	char *tmp;
	
	/*assert*/
	if( (pstFdSet == NULL) ||\
		(srcPath == NULL) ||\
		(dstPath == NULL) ||\
		(spLen > GPN_FT_PARA_MAX_PATH_LEN) ||\
		(dpLen > GPN_FT_PARA_MAX_PATH_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pFTMsg = (gpnSockMsg *)payload;
	
	pFTMsg->iMsgCtrl = 0;
	
	/*this msg is transferd local CUP, so..., and GPN_SOCK_ROLE_TRANS is dst modu*/
	pFTMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_TRANS);
	pFTMsg->iSrcId = pstFdSet->localSrc;
	
	pFTMsg->iMsgType = GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT;
	
	pFTMsg->iMsgPara1 = fileType;
	pFTMsg->iMsgPara2 = src;
	pFTMsg->iMsgPara3 = dst;
	pFTMsg->iMsgPara4 = spLen;
	pFTMsg->iMsgPara5 = dpLen;
	pFTMsg->msgCellLen = spLen+dpLen;

	tmp = (char *)(&(pFTMsg->msgCellLen)+1);
	memcpy(tmp, srcPath, spLen);

	tmp += spLen;
	memcpy(tmp, dstPath, dpLen);
	
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pFTMsg, GPN_SOCK_MSG_HEAD_BLEN+pFTMsg->msgCellLen);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtReqCreatRsp(stSockFdSet *pstFdSet, UINT32 reqOwner, UINT32 fileType,
	UINT32 src, UINT32 dst, UINT32 reqId, UINT32 maxTimer, UINT32 sta, UINT32 errCode)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = reqOwner;
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_FT_MSG_FILE_TRANS_REQ_CREAT_RSP;
	
	FTMsg.iMsgPara1 = fileType;
	FTMsg.iMsgPara2 = src;
	FTMsg.iMsgPara3 = dst;
	FTMsg.iMsgPara4 = reqId;
	FTMsg.iMsgPara5 = maxTimer;
	FTMsg.iMsgPara6 = sta;
	FTMsg.iMsgPara7 = errCode;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtSpReqCreatRsp(stSockFdSet *pstFdSet, UINT32 reqOwner, UINT32 fileType,
	UINT32 src, char *srcPath, UINT32 spLen, UINT32 dst, char *dstPath, UINT32 dpLen,
	UINT32 reqId, UINT32 maxTimer, UINT32 sta, UINT32 errCode)
{
	UINT8 payload[GPN_FT_PARA_SPFILE_TR_CREAT_MSG_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pFTMsg;
	char *tmp;
	
	/*assert*/
	if( (pstFdSet == NULL) ||\
		(srcPath == NULL) ||\
		(dstPath == NULL) ||\
		(spLen > GPN_FT_PARA_MAX_PATH_LEN) ||\
		(dpLen > GPN_FT_PARA_MAX_PATH_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pFTMsg = (gpnSockMsg *)payload;
	
	pFTMsg->iMsgCtrl = 0;
	
	/*this msg is transferd local CUP, so..., and GPN_SOCK_ROLE_TRANS is dst modu*/
	pFTMsg->iDstId = reqOwner;
	pFTMsg->iSrcId = pstFdSet->localSrc;
	
	pFTMsg->iMsgType = GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT_RSP;
	
	pFTMsg->iMsgPara1 = fileType;
	pFTMsg->iMsgPara2 = src;
	pFTMsg->iMsgPara3 = dst;
	pFTMsg->iMsgPara4 = spLen;
	pFTMsg->iMsgPara5 = dpLen;
	pFTMsg->iMsgPara6 = reqId;
	pFTMsg->iMsgPara7 = maxTimer;
	pFTMsg->iMsgPara8 = sta;
	pFTMsg->iMsgPara9 = errCode;
	pFTMsg->msgCellLen = spLen+dpLen;

	tmp = (char *)(&(pFTMsg->msgCellLen)+1);
	memcpy(tmp, srcPath, spLen);

	tmp += spLen;
	memcpy(tmp, dstPath, dpLen);
	
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pFTMsg, GPN_SOCK_MSG_HEAD_BLEN+pFTMsg->msgCellLen);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFTMsgFtReqDeleteNew(UINT32 reqId)
{
	gpnSockFTMsgFtReqDelete(&(gSockCommData.gstFdSet), reqId);
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFTMsgFtReqDelete(stSockFdSet *pstFdSet, UINT32 reqId)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	FTMsg.iMsgCtrl = 0;

	/*this msg is transferd local CUP, so..., and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_TRANS);
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_FT_MSG_FILE_TRANS_REQ_DEL;
	
	FTMsg.iMsgPara1 = reqId;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtReqDeleteRsp(stSockFdSet *pstFdSet, UINT32 reqOwner,
	UINT32 reqId, UINT32 sta, UINT32 errCode)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = reqOwner;
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_FT_MSG_FILE_TRANS_REQ_DEL_RSP;
	
	FTMsg.iMsgPara1 = reqId;
	FTMsg.iMsgPara2 = sta;
	FTMsg.iMsgPara3 = errCode;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtReqStaGetNew(UINT32 reqId)
{
	gpnSockFTMsgFtReqStaGet(&(gSockCommData.gstFdSet), reqId);
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFTMsgFtReqStaGet(stSockFdSet *pstFdSet, UINT32 reqId)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	FTMsg.iMsgCtrl = 0;

	/*this msg is transferd local CUP, so..., and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_TRANS);
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_FT_MSG_REQ_STA_GET;
	
	FTMsg.iMsgPara1 = reqId;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtReqStaNotify(stSockFdSet *pstFdSet, UINT32 reqOwner, UINT32 reqId,
	UINT32 sta, UINT32 errCode, UINT32 percent, UINT32 sufix)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = reqOwner;
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_FT_MSG_REQ_STA_NOTIFY;
	
	FTMsg.iMsgPara1 = reqId;
	FTMsg.iMsgPara2 = sta;
	FTMsg.iMsgPara3 = errCode;
	FTMsg.iMsgPara4 = percent;
	FTMsg.iMsgPara5 = sufix;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}


/****************************************************************************************/
/*                               TASK  API                                                                                                            */
/****************************************************************************************/
UINT32 gpnSockFTMsgFtTaskCreat(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 reqId, UINT32 fileType, UINT32 src, UINT32 dst)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd local CUP, so..., and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = GPN_SOCK_CREAT_DIREC(dstCPU, GPN_SOCK_ROLE_FT);
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_FT_MSG_FILE_TRANS_TASK_CREAT;
	
	FTMsg.iMsgPara1 = pstFdSet->localSrc;
	FTMsg.iMsgPara2 = reqId;
	FTMsg.iMsgPara3 = fileType;
	FTMsg.iMsgPara4 = src;
	FTMsg.iMsgPara5 = dst;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFTMsgFtSpTaskCreat(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 reqId, UINT32 fileType, UINT32 src, char *srcPath, UINT32 spLen,
	UINT32 dst, char *dstPath, UINT32 dpLen)
{
	UINT8 payload[GPN_FT_PARA_SPFILE_TT_CREAT_MSG_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pFTMsg;
	char *tmp;
	
	/*assert*/
	if( (pstFdSet == NULL) ||\
		(srcPath == NULL) ||\
		(dstPath == NULL) ||\
		(spLen > GPN_FT_PARA_MAX_PATH_LEN) ||\
		(dpLen > GPN_FT_PARA_MAX_PATH_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pFTMsg = (gpnSockMsg *)payload;
	
	pFTMsg->iMsgCtrl = 0;
	
	/*this msg is transferd local CUP, so..., and GPN_SOCK_ROLE_TRANS is dst modu*/
	pFTMsg->iDstId = GPN_SOCK_CREAT_DIREC(dstCPU, GPN_SOCK_ROLE_FT);
	pFTMsg->iSrcId = pstFdSet->localSrc;
	
	pFTMsg->iMsgType = GPN_FT_MSG_SPFILE_TRANS_TASK_CREAT;
	
	pFTMsg->iMsgPara1 = pstFdSet->localSrc;
	pFTMsg->iMsgPara2 = reqId;
	pFTMsg->iMsgPara3 = fileType;
	pFTMsg->iMsgPara4 = src;
	pFTMsg->iMsgPara5 = dst;
	pFTMsg->iMsgPara6 = spLen;
	pFTMsg->iMsgPara7 = dpLen;
	pFTMsg->msgCellLen = spLen+dpLen;

	tmp = (char *)(&(pFTMsg->msgCellLen)+1);
	memcpy(tmp, srcPath, spLen);

	tmp += spLen;
	memcpy(tmp, dstPath, dpLen);
	
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pFTMsg, GPN_SOCK_MSG_HEAD_BLEN+pFTMsg->msgCellLen);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtTaskCreatRsp(stSockFdSet *pstFdSet, UINT32 taskOwner, UINT32 reqId,
	UINT32 localTaskId, UINT32 maxTime, UINT32 taskSta, UINT32 errCode)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = taskOwner;
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_FT_MSG_FILE_TRANS_TASK_CREAT_RSP;
	
	FTMsg.iMsgPara1 = reqId;
	FTMsg.iMsgPara2 = localTaskId;
	FTMsg.iMsgPara3 = maxTime;
	FTMsg.iMsgPara4 = taskSta;
	FTMsg.iMsgPara5 = errCode;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFTMsgFtTaskDelete(stSockFdSet *pstFdSet, UINT32 dstCPU, UINT32 reqId, UINT32 taskId)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	FTMsg.iMsgCtrl = 0;

	/*this msg is transferd local CUP, so..., and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = GPN_SOCK_CREAT_DIREC(dstCPU, GPN_SOCK_ROLE_FT);
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_FT_MSG_FILE_TRANS_TASK_DEL;
	
	FTMsg.iMsgPara1 = reqId;
	FTMsg.iMsgPara2 = taskId;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtTaskDeleteRsp(stSockFdSet *pstFdSet, UINT32 taskOwner,
	UINT32 reqId, UINT32 localTaskId, UINT32 sta, UINT32 errCode)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = taskOwner;
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_FT_MSG_FILE_TRANS_TASK_DEL_RSP;
	
	FTMsg.iMsgPara1 = reqId;
	FTMsg.iMsgPara2 = localTaskId;
	FTMsg.iMsgPara2 = sta;
	FTMsg.iMsgPara4 = errCode;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtTaskStaGet(stSockFdSet *pstFdSet, UINT32 dstCPU, UINT32 reqId, UINT32 taskId)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	FTMsg.iMsgCtrl = 0;

	/*this msg is transferd local CUP, so..., and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = GPN_SOCK_CREAT_DIREC(dstCPU, GPN_SOCK_ROLE_FT);
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_FT_MSG_TASK_STA_GET;
	
	FTMsg.iMsgPara1 = reqId;
	FTMsg.iMsgPara2 = taskId;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtTaskStaNotify(stSockFdSet *pstFdSet, UINT32 taskOwner, UINT32 reqId,
	UINT32 localTaskId, UINT32 sta, UINT32 errCode, UINT32 percent, UINT32 sufix)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = taskOwner;
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_FT_MSG_TASK_STA_NOTIFY;
	
	FTMsg.iMsgPara1 = reqId;
	FTMsg.iMsgPara2 = localTaskId;
	FTMsg.iMsgPara3 = sta;
	FTMsg.iMsgPara4 = errCode;
	FTMsg.iMsgPara5 = percent;
	FTMsg.iMsgPara6 = sufix;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}



/****************************************************************************************/
/*                               ACTION  API                                                                                                        */
/****************************************************************************************/
UINT32 gpnSockFTMsgFtTaskStart(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 fileType, UINT32 fileSrc, UINT32 fileDst, UINT32 totBlock)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = GPN_SOCK_CREAT_DIREC(dstCPU, GPN_SOCK_ROLE_FT);
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_CFGMGT_MSG_FILE_TRANS_ACT_START;
	
	FTMsg.iMsgPara1 = srcTaskId;
	FTMsg.iMsgPara2 = fileType;
	FTMsg.iMsgPara3 = fileSrc;
	FTMsg.iMsgPara4 = fileDst;
	FTMsg.iMsgPara5 = totBlock;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFTMsgFtSpTaskStart(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 totBlock, UINT32 fileType, UINT32 src, char *srcPath,
	UINT32 spLen, UINT32 dst, char *dstPath, UINT32 dpLen)
{
	UINT8 payload[GPN_FT_PARA_SPFILE_TT_START_MSG_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pFTMsg;
	char *tmp;
	
	/*assert*/
	if( (pstFdSet == NULL) ||\
		(srcPath == NULL) ||\
		(dstPath == NULL) ||\
		(spLen > GPN_FT_PARA_MAX_PATH_LEN) ||\
		(dpLen > GPN_FT_PARA_MAX_PATH_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pFTMsg = (gpnSockMsg *)payload;
	
	pFTMsg->iMsgCtrl = 0;
	
	/*this msg is transferd local CUP, so..., and GPN_SOCK_ROLE_TRANS is dst modu*/
	pFTMsg->iDstId = GPN_SOCK_CREAT_DIREC(dstCPU, GPN_SOCK_ROLE_FT);
	pFTMsg->iSrcId = pstFdSet->localSrc;
	
	pFTMsg->iMsgType = GPN_CFGMGT_MSG_SPFILE_TRANS_ACT_START;
	
	pFTMsg->iMsgPara1 = srcTaskId;
	pFTMsg->iMsgPara2 = totBlock;
	pFTMsg->iMsgPara3 = fileType;
	pFTMsg->iMsgPara4 = src;
	pFTMsg->iMsgPara5 = dst;
	pFTMsg->iMsgPara6 = spLen;
	pFTMsg->iMsgPara7 = dpLen;
	pFTMsg->msgCellLen = spLen+dpLen;

	tmp = (char *)(&(pFTMsg->msgCellLen)+1);
	memcpy(tmp, srcPath, spLen);

	tmp += spLen;
	memcpy(tmp, dstPath, dpLen);
	
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pFTMsg, GPN_SOCK_MSG_HEAD_BLEN+pFTMsg->msgCellLen);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtTaskStartRsp(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 dstTaskId, UINT32 sta, UINT32 errCode)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = GPN_SOCK_CREAT_DIREC(dstCPU, GPN_SOCK_ROLE_FT);;
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_CFGMGT_MSG_FILE_TRANS_ACT_START_RSP;
	
	FTMsg.iMsgPara1 = srcTaskId;
	FTMsg.iMsgPara2 = dstTaskId;
	FTMsg.iMsgPara3 = sta;
	FTMsg.iMsgPara4 = errCode;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFTMsgFtTaskStop(stSockFdSet *pstFdSet, UINT32 dstCPU, UINT32 srcTaskId, UINT32 dstTaskId)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = GPN_SOCK_CREAT_DIREC(dstCPU, GPN_SOCK_ROLE_FT);;
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP;
	
	FTMsg.iMsgPara1 = srcTaskId;
	FTMsg.iMsgPara2 = dstTaskId;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFTMsgFtTaskStopRsp(stSockFdSet *pstFdSet, UINT32 msgDst,
	UINT32 srcTaskId, UINT32 dstTaskId, UINT32 sta, UINT32 errCode)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = msgDst;
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP_RSP;
	
	FTMsg.iMsgPara1 = srcTaskId;
	FTMsg.iMsgPara2 = dstTaskId;
	FTMsg.iMsgPara3 = sta;
	FTMsg.iMsgPara4 = errCode;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtTaskTxReq(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 dstTaskId, UINT32 bStart, UINT32 bNum, UINT32 bSize)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = GPN_SOCK_CREAT_DIREC(dstCPU, GPN_SOCK_ROLE_FT);;
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_CFGMGT_MSG_FT_ACT_TX_REQ;
	
	FTMsg.iMsgPara1 = srcTaskId;
	FTMsg.iMsgPara2 = dstTaskId;
	FTMsg.iMsgPara3 = bStart;
	FTMsg.iMsgPara4 = bNum;
	FTMsg.iMsgPara5 = bSize;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtTaskTxRsp(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 dstTaskId, UINT32 bLab, UINT32 crc32, UINT8 *text, UINT32 len)
{
	UINT8 payload[GPN_FT_PARA_TASK_TX_MSG_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pFTMsg;

	/*assert*/
	if( (pstFdSet == NULL) ||\
		(text ==NULL) ||\
		(len > GPN_FT_PARA_TASK_TX_MSG_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pFTMsg = (gpnSockMsg *)payload;
	
	pFTMsg->iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	pFTMsg->iDstId = GPN_SOCK_CREAT_DIREC(dstCPU, GPN_SOCK_ROLE_FT);
	pFTMsg->iSrcId = pstFdSet->localSrc;
	
	pFTMsg->iMsgType = GPN_CFGMGT_MSG_FT_ACT_TX_RSP;
	
	pFTMsg->iMsgPara1 = srcTaskId;
	pFTMsg->iMsgPara2 = dstTaskId;
	pFTMsg->iMsgPara3 = bLab;
	pFTMsg->iMsgPara4 = crc32;
	pFTMsg->iMsgPara5 = len;
	
	pFTMsg->msgCellLen = len;
	memcpy((payload+GPN_SOCK_MSG_HEAD_BLEN), text, len);
	
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pFTMsg, GPN_SOCK_MSG_HEAD_BLEN + len);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockFTMsgFtTaskMD5CheckReq(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 dstTaskId, UINT8 *md5, UINT32 len)
{
	UINT8 payload[GPN_FT_PARA_CHECK_MD5_MSG_LEN];
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if( (pstFdSet == NULL) ||\
		(md5 == NULL) ||\
		(len != GPN_FT_PARA_MD5_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = GPN_SOCK_CREAT_DIREC(dstCPU, GPN_SOCK_ROLE_FT);;
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_CFGMGT_MSG_FILE_TRANS_MD5_CHECK;
	
	FTMsg.iMsgPara1 = srcTaskId;
	FTMsg.iMsgPara2 = dstTaskId;
	FTMsg.iMsgPara3 = len;
	
	
	FTMsg.msgCellLen = len;
	memcpy((payload+GPN_SOCK_MSG_HEAD_BLEN), md5, len);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_FT_PARA_CHECK_MD5_MSG_LEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockFTMsgFtTaskMD5CheckRsp(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 dstTaskId, UINT32 sta)
{
	/*msgHead(64)*/
	gpnSockMsg FTMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	FTMsg.iMsgCtrl = 0;
	
	/*this msg is transferd in local CPU, so use local cpuid as dst cup, and GPN_SOCK_ROLE_TRANS is dst modu*/
	FTMsg.iDstId = GPN_SOCK_CREAT_DIREC(dstCPU, GPN_SOCK_ROLE_FT);;
	FTMsg.iSrcId = pstFdSet->localSrc;
	
	FTMsg.iMsgType = GPN_CFGMGT_MSG_FILE_TRANS_MD5_CHECK_RSP;
	
	FTMsg.iMsgPara1 = srcTaskId;
	FTMsg.iMsgPara2 = dstTaskId;
	FTMsg.iMsgPara3 = sta;
	FTMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &FTMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;

}



#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SOCK_FT_MSG_DEF_C_*/

