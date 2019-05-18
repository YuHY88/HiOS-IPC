/**********************************************************
* file name: gpnSockSdmMsgDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan Wangyx 2014-07-29
* function: 
*   define details about communication between sdmd modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_SDM_MSG_DEF_C_
#define _GPN_SOCK_SDM_MSG_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <sys/types.h>
#include <unistd.h>

#include "socketComm/gpnSockCommRoleMan.h"
#include "socketComm/gpnSockSdmMsgDef.h"
#include "devCoreInfo/gpnDevCoreInfoFunApi.h"


/*socket communication global data:define in socket communication module*/
extern sockCommData gSockCommData;

UINT32 gpnSockSDMDMsgTxStartDownloadReq(stSockFdSet *pstFdSet, 
	UINT32 dstSlot, UINT32 devType, UINT32 fileType, UINT32 fileSize, UINT32 fileNameLen, char *fileName)
{
	UINT8 payload[GPN_SDM_PARA_SPFILE_TR_CREAT_MSG_LEN];
	INT8 *tmp;

	gpnSockMsg *sdmMsg;
	
	/*assert*/
	if( (pstFdSet == NULL) ||\
		(fileNameLen > GPN_SDM_PARA_MAX_PATH_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	sdmMsg = (gpnSockMsg *)payload;
	
	sdmMsg->iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	
	sdmMsg->iDstId = GPN_SOCK_CREAT_DIREC(dstSlot, GPN_SOCK_ROLE_SDM);
	sdmMsg->iSrcId = pstFdSet->localSrc;
	
	sdmMsg->iMsgType = GPN_SDMD_MSG_START_DOWNLOAD_REQ;
	
	sdmMsg->iMsgPara1 = dstSlot;
	sdmMsg->iMsgPara2 = devType;
	sdmMsg->iMsgPara3 = fileType;
	sdmMsg->iMsgPara4 = fileSize;
	sdmMsg->msgCellLen = fileNameLen;

	tmp = (char *)(&(sdmMsg->msgCellLen)+1);
	memcpy(tmp, fileName, fileNameLen);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, sdmMsg, GPN_SOCK_MSG_HEAD_BLEN+sdmMsg->msgCellLen);
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSDMDMsgTxStartDownloadRsp(stSockFdSet *pstFdSet, 
	UINT32 dstSlot, UINT32 srcSlot, UINT32 devType,  UINT32 fileType, UINT32 stat, UINT32 errCode, 
	UINT32 fileNameLen, UINT32 filePathLen, INT8 *fileName, INT8 *filePath)
{
	UINT8 payload[GPN_SDM_PARA_SPFILE_TR_CREAT_MSG_LEN];
	INT8 *tmp;

	gpnSockMsg *sdmMsg;
	
	/*assert*/
	if( (pstFdSet == NULL) ||\
		(fileNameLen > GPN_SDM_PARA_MAX_PATH_LEN) ||\
		(filePathLen > GPN_SDM_PARA_MAX_PATH_LEN))
	{
		return GPN_SELECT_GEN_ERR;
	}

	sdmMsg = (gpnSockMsg *)payload;
	
	sdmMsg->iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	
	sdmMsg->iDstId = GPN_SOCK_CREAT_DIREC(dstSlot, GPN_SOCK_ROLE_SDM);
	sdmMsg->iSrcId = pstFdSet->localSrc;
	
	sdmMsg->iMsgType = GPN_SDMD_MSG_START_DOWNLOAD_RSP;
	
	sdmMsg->iMsgPara1 = srcSlot;
	sdmMsg->iMsgPara2 = devType;
	sdmMsg->iMsgPara3 = fileType;
	sdmMsg->iMsgPara4 = stat;
	sdmMsg->iMsgPara5 = errCode;
	sdmMsg->iMsgPara6 = fileNameLen;
	sdmMsg->iMsgPara7 = filePathLen;

	sdmMsg->msgCellLen = fileNameLen + filePathLen;

	tmp = (char *)(&(sdmMsg->msgCellLen)+1);
	memcpy(tmp, fileName, fileNameLen);

	tmp += fileNameLen;
	memcpy(tmp, filePath, filePathLen);
	
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, sdmMsg, GPN_SOCK_MSG_HEAD_BLEN+sdmMsg->msgCellLen);

	return GPN_SELECT_GEN_OK;
}

/*src slot is sub, upload to NMx*/
UINT32 gpnSockSDMDMsgTxStartUploadReq(stSockFdSet *pstFdSet, 
	UINT32 dstSlot, UINT32 devType, UINT32 fileType, UINT32 fileNameLen, INT8 *fileName)
{
	UINT8 payload[GPN_SDM_PARA_SPFILE_TR_CREAT_MSG_LEN];
	INT8 *tmp;

	gpnSockMsg *sdmMsg;

	/*assert*/
	if( (pstFdSet == NULL) ||\
		(fileNameLen > GPN_SDM_PARA_MAX_PATH_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	sdmMsg = (gpnSockMsg *)payload;
	
	sdmMsg->iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	
	sdmMsg->iDstId = GPN_SOCK_CREAT_DIREC(dstSlot, GPN_SOCK_ROLE_SDM);
	sdmMsg->iSrcId = pstFdSet->localSrc;
	
	sdmMsg->iMsgType = GPN_SDMD_MSG_START_UPLOAD_REQ;
	
	sdmMsg->iMsgPara1 = dstSlot;
	sdmMsg->iMsgPara2 = devType;
	sdmMsg->iMsgPara3 = fileType;
	sdmMsg->iMsgPara4 = fileNameLen;

	sdmMsg->msgCellLen = fileNameLen;

	tmp = (char *)(&(sdmMsg->msgCellLen)+1);
	memcpy(tmp, fileName, fileNameLen);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, sdmMsg, GPN_SOCK_MSG_HEAD_BLEN + sdmMsg->msgCellLen);
	
	return GPN_SELECT_GEN_OK;
}

/*dstSlot is NMx*/
UINT32 gpnSockSDMDMsgTxStartUploadRsp(stSockFdSet *pstFdSet, 
	UINT32 dstSlot, UINT32 srcSlot, UINT32 devType,  UINT32 fileType, UINT32 stat, UINT32 errCode, 
	UINT32 fileSize, UINT32 fileNameLen, UINT32 filePathLen, INT8 *fileName, INT8 *filePath)
{
	UINT8 payload[GPN_SDM_PARA_SPFILE_TR_CREAT_MSG_LEN];
	INT8 *tmp;

	gpnSockMsg *sdmMsg;

	/*assert*/
	if( (pstFdSet == NULL) ||\
		(fileNameLen > GPN_SDM_PARA_MAX_PATH_LEN) ||\
		(filePathLen > GPN_SDM_PARA_MAX_PATH_LEN))
	{
		return GPN_SELECT_GEN_ERR;
	}

	sdmMsg = (gpnSockMsg *)payload;
	
	sdmMsg->iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	
	sdmMsg->iDstId = GPN_SOCK_CREAT_DIREC(dstSlot, GPN_SOCK_ROLE_SDM);
	sdmMsg->iSrcId = pstFdSet->localSrc;
	
	sdmMsg->iMsgType = GPN_SDMD_MSG_START_UPLOAD_RSP;
	
	sdmMsg->iMsgPara1 = srcSlot;
	sdmMsg->iMsgPara2 = devType;
	sdmMsg->iMsgPara3 = fileType;
	sdmMsg->iMsgPara4 = stat;
	sdmMsg->iMsgPara5 = errCode;
	sdmMsg->iMsgPara6 = fileSize;
	sdmMsg->iMsgPara7 = fileNameLen;
	sdmMsg->iMsgPara8 = filePathLen;
	sdmMsg->msgCellLen = fileNameLen + filePathLen;

	tmp = (char *)(&(sdmMsg->msgCellLen)+1);
	memcpy(tmp, fileName, fileNameLen);

	tmp += fileNameLen;
	memcpy(tmp, filePath, filePathLen);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, sdmMsg, GPN_SOCK_MSG_HEAD_BLEN+sdmMsg->msgCellLen);
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockSDMDMsgTxTagFileRepalceOpt(stSockFdSet *pstFdSet, 
	UINT32 dstSlot, UINT32 devType, UINT32 fileType, UINT32 suffixID, 
	UINT32 fileNameLen, UINT32 filePathLen, INT8 *fileName, INT8 *filePath)
{
	UINT8 payload[GPN_SDM_PARA_SPFILE_TR_CREAT_MSG_LEN];
	INT8 *tmp;

	gpnSockMsg *sdmMsg;
	
	/*assert*/
	if( (pstFdSet == NULL) ||\
		(fileNameLen > GPN_SDM_PARA_MAX_PATH_LEN)||\
		(filePathLen > GPN_SDM_PARA_MAX_PATH_LEN))
	{
		return GPN_SELECT_GEN_ERR;
	}

	sdmMsg = (gpnSockMsg *)payload;
	
	sdmMsg->iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	
	sdmMsg->iDstId = GPN_SOCK_CREAT_DIREC(dstSlot, GPN_SOCK_ROLE_SDM);
	sdmMsg->iSrcId = pstFdSet->localSrc;
	
	sdmMsg->iMsgType = GPN_SDMD_MSG_TAG_FILE_REPLACE_OPT;
	
	sdmMsg->iMsgPara1 = dstSlot;
	sdmMsg->iMsgPara2 = devType;
	sdmMsg->iMsgPara3 = fileType;
	sdmMsg->iMsgPara4 = suffixID;
	sdmMsg->iMsgPara5 = fileNameLen;
	sdmMsg->iMsgPara6 = filePathLen;
	sdmMsg->msgCellLen = fileNameLen + filePathLen;

	tmp = (char *)(&(sdmMsg->msgCellLen)+1);
	memcpy(tmp, fileName, fileNameLen);

	tmp += fileNameLen;
	memcpy(tmp, filePath, filePathLen);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, sdmMsg, GPN_SOCK_MSG_HEAD_BLEN+sdmMsg->msgCellLen);
	
	return GPN_SELECT_GEN_OK;
}

/*dstSlot is NMx*/
UINT32 gpnSockSDMDMsgTxTagFileRepalceRsp(stSockFdSet *pstFdSet, 
	UINT32 dstSlot, UINT32 srcSlot, UINT32 devType,  UINT32 fileType, UINT32 suffixID, 
	UINT32 stat, UINT32 errCode, UINT32 fileNameLen, UINT32 filePathLen, INT8 *fileName, INT8 *filePath)
{
	UINT8 payload[GPN_SDM_PARA_SPFILE_TR_CREAT_MSG_LEN];
	INT8 *tmp;

	gpnSockMsg *sdmMsg;

	/*assert*/
	if( (pstFdSet == NULL) ||\
		(fileNameLen > GPN_SDM_PARA_MAX_PATH_LEN) ||\
		(filePathLen > GPN_SDM_PARA_MAX_PATH_LEN))
	{
		return GPN_SELECT_GEN_ERR;
	}

	sdmMsg = (gpnSockMsg *)payload;
	
	sdmMsg->iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	
	sdmMsg->iDstId = GPN_SOCK_CREAT_DIREC(dstSlot, GPN_SOCK_ROLE_SDM);
	sdmMsg->iSrcId = pstFdSet->localSrc;
	
	sdmMsg->iMsgType = GPN_SDMD_MSG_TAG_FILE_REPLACE_RSP;
	
	sdmMsg->iMsgPara1 = srcSlot;
	sdmMsg->iMsgPara2 = devType;
	sdmMsg->iMsgPara3 = fileType;
	sdmMsg->iMsgPara4 = suffixID;
	sdmMsg->iMsgPara5 = stat;
	sdmMsg->iMsgPara6 = errCode;
	sdmMsg->iMsgPara7 = fileNameLen;
	sdmMsg->iMsgPara8 = filePathLen;
	sdmMsg->msgCellLen = fileNameLen + filePathLen;

	tmp = (char *)(&(sdmMsg->msgCellLen)+1);
	memcpy(tmp, fileName, fileNameLen);

	tmp += fileNameLen;
	memcpy(tmp, filePath, filePathLen);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, sdmMsg, GPN_SOCK_MSG_HEAD_BLEN+sdmMsg->msgCellLen);

	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSDMDMsgTxTagFileDeleteOpt(stSockFdSet *pstFdSet, 
	UINT32 dstSlot, UINT32 devType, UINT32 fileType, UINT32 fileNameLen, UINT32 filePathLen, 
	INT8 *fileName, INT8 *filePath)
{
	UINT8 payload[GPN_SDM_PARA_SPFILE_TR_CREAT_MSG_LEN];
	INT8 *tmp;

	gpnSockMsg *sdmMsg;
	
	/*assert*/
	if( (pstFdSet == NULL) ||\
		(fileNameLen > GPN_SDM_PARA_MAX_PATH_LEN)||\
		(filePathLen > GPN_SDM_PARA_MAX_PATH_LEN))
	{
		return GPN_SELECT_GEN_ERR;
	}

	sdmMsg = (gpnSockMsg *)payload;
	
	sdmMsg->iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	
	sdmMsg->iDstId = GPN_SOCK_CREAT_DIREC(dstSlot, GPN_SOCK_ROLE_SDM);
	sdmMsg->iSrcId = pstFdSet->localSrc;
	
	sdmMsg->iMsgType = GPN_SDMD_MSG_TAG_FILE_DELETE_OPT;
	
	sdmMsg->iMsgPara1 = dstSlot;
	sdmMsg->iMsgPara2 = devType;
	sdmMsg->iMsgPara3 = fileType;
	sdmMsg->iMsgPara4 = fileNameLen;
	sdmMsg->iMsgPara5 = filePathLen;
	sdmMsg->msgCellLen = fileNameLen + filePathLen;

	tmp = (char *)(&(sdmMsg->msgCellLen)+1);
	memcpy(tmp, fileName, fileNameLen);

	tmp += fileNameLen;
	memcpy(tmp, filePath, filePathLen);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, sdmMsg, GPN_SOCK_MSG_HEAD_BLEN+sdmMsg->msgCellLen);
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSDMDMsgSoltStatGet(UINT32 slot)
{
	gpnSockMsg sdmMsg;

	/*assert */

	sdmMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	sdmMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_SDM);
	sdmMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	sdmMsg.iMsgType = GPN_SDMD_MSG_EMD_SLOT_STAT_GET;
	sdmMsg.iMsgPara1 = slot;
	sdmMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &sdmMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockSDMDMsgSoltOpenNotify(UINT32 slot, UINT32 devType)
{
	gpnSockMsg sdmMsg;

	/*assert */

	sdmMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	sdmMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_NSM);
	sdmMsg.iSrcId = gSockCommData.gstFdSet.localSrc;/*GPN_COMMM_STAT*/
	
	sdmMsg.iMsgType = GPN_SDMD_MSG_EMD_SLOT_OPEN_NOTIFY;
	sdmMsg.iMsgPara1 = slot;
	sdmMsg.iMsgPara2 = devType;
	sdmMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &sdmMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockSDMDMsgSoltCloseNotify(UINT32 slot, UINT32 devType)
{
	gpnSockMsg sdmMsg;

	/*assert */

	sdmMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	sdmMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_NSM);
	sdmMsg.iSrcId = gSockCommData.gstFdSet.localSrc;/*GPN_COMMM_STAT*/
	
	sdmMsg.iMsgType = GPN_SDMD_MSG_EMD_SLOT_CLOSE_NOTIFY;
	sdmMsg.iMsgPara1 = slot;
	sdmMsg.iMsgPara2 = devType;
	sdmMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &sdmMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}


UINT32 gpnSockSDMDMsgLedControl(UINT32 slot, UINT32 ledType, UINT32 ledId, UINT32 opt)
{
	gpnSockMsg sdmMsg;

	/*assert */

	sdmMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	sdmMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_SDM);
	sdmMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	sdmMsg.iMsgType = GPN_SDMD_MSG_EMD_LED_CONTROL;
	if(slot == 0)
	{
		sdmMsg.iMsgPara1 = gpnDevApiGetSelfSoltNum();
	}
	else
	{
		sdmMsg.iMsgPara1 = slot;
	}
	sdmMsg.iMsgPara2 = ledType;
	sdmMsg.iMsgPara3 = ledId;
	sdmMsg.iMsgPara4 = opt;
	sdmMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &sdmMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockSDMDMsgTxTemStatNotif(UINT32 stat, UINT32 detail)
{
	gpnSockMsg sdmMsg;

	/*assert */

	sdmMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	sdmMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_ONMD);
	sdmMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	sdmMsg.iMsgType = GPN_SDMD_MSG_EMD_TEM_STAT_NOTIFY;
	sdmMsg.iMsgPara1 = stat;
	sdmMsg.iMsgPara2 = detail;
	sdmMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &sdmMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockSDMDMsgTxNotifyEmdCfgSave(UINT32 mode)
{
	gpnSockMsg sdmMsg;

	/*assert */

	sdmMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	sdmMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_SDM);
	sdmMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	sdmMsg.iMsgType = GPN_SDMD_MSG_EMD_NOTIFYED_CFG_SAVE;
	sdmMsg.iMsgPara1 = mode;
	sdmMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &sdmMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SOCK_CFGMGT_MSG_DEF_C_*/

