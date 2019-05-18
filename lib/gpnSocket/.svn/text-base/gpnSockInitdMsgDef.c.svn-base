/**********************************************************
* file name: gpnSockInitdMsgDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-11
* function: 
*    define details about communication between gpnInitd modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_INITD_MSG_DEF_C_
#define _GPN_SOCK_INITD_MSG_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <sys/types.h>
#include <unistd.h>

#include "socketComm/gpnSockInitdMsgDef.h"
#include "socketComm/gpnSockCommModuDef.h"
#include "socketComm/gpnSockCommRoleMan.h"
/*msg direction creat*/

extern sockCommData gSockCommData;

UINT32 gpnSockInitdMsgNDRequest(UINT32 MODU_ID)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg initStaMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	initStaMsg.iMsgCtrl = 0;
	initStaMsg.iMsgType = GPN_INITD_PROCESS_ND_REQ;
	initStaMsg.iDstId  = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, MODU_ID);
	initStaMsg.iSrcId = pstFdSet->localSrc;
	initStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &initStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockInitdMsgNDResponse(pid_t pid, UINT32 runSta)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg initStaMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	initStaMsg.iMsgCtrl = 0;
	initStaMsg.iMsgType = GPN_INITD_PROCESS_ND_RSP;
	initStaMsg.iDstId  = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	initStaMsg.iSrcId = pstFdSet->localSrc;


	
	initStaMsg.iMsgPara1 = pid;
	initStaMsg.iMsgPara2 = runSta;
	initStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &initStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockInitdMsgSUStepXEndNotify(UINT32 stepIndex, UINT32 sta)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg initStaMsg;

	pstFdSet = &(gSockCommData.gstFdSet);
	
	initStaMsg.iMsgCtrl = 0;
	initStaMsg.iMsgType = GPN_INITD_MSG_STEP_OK_REPT;
	initStaMsg.iDstId  = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	initStaMsg.iSrcId = pstFdSet->localSrc;

	initStaMsg.iMsgPara1 = stepIndex;
	initStaMsg.iMsgPara2 = sta;
	initStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &initStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockInitdMsgTxIsCfgDownAsk(void)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg initStaMsg;

	pstFdSet = &(gSockCommData.gstFdSet);

	initStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	initStaMsg.iMsgType = GPN_INITD_MSG_CFG_DOWN_REQ;
	initStaMsg.iDstId =  GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId,  GPN_SOCK_ROLE_INIT);
	initStaMsg.iSrcId = pstFdSet->localSrc;
	
	initStaMsg.iMsgPara1 = GPN_SOCK_DECOM_MODUID(pstFdSet->localSrc);
	initStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &initStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockInitdMsgTxCfgDown2ValidModule(void)
{
	#if 0
	UINT32 i;
	
	for(i=GPN_SOCK_ROLE_TRANS;i<(GPN_SOCK_INS_ROLE_ID_MAX+1);i++)
	{
		if(gpnSockRoleModuValidCheck(i) == GPN_SELECT_GEN_OK)
		{
			gpnSockInitdMsgTxCfgDown2OtherModule(i, GPN_SELECT_GEN_YES);
		}
	}
	#endif
	#if defined(HAVE_GPN_CFGMGT)
	gpnSockInitdMsgTxCfgDown2OtherModule(GPN_SOCK_ROLE_CFGMGT, GPN_SELECT_GEN_YES);
	#endif
	#if defined(HAVE_GPN_SDM)
	gpnSockInitdMsgTxCfgDown2OtherModule(GPN_SOCK_ROLE_SDM, GPN_SELECT_GEN_YES);
	#endif
	#if defined(HAVE_GPN_NMSYN)
	gpnSockInitdMsgTxCfgDown2OtherModule(GPN_SOCK_ROLE_NM_SYN, GPN_SELECT_GEN_YES);
	#endif

	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockInitdMsgTxCfgDown2OtherModule(UINT32 MODU_ID, UINT32 cfg_down)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg initStaMsg;

	pstFdSet = &(gSockCommData.gstFdSet);

	initStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	initStaMsg.iMsgType = GPN_INITD_MSG_CFG_DOWN_NOTIFY;
	initStaMsg.iDstId =  GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId,  MODU_ID);
	initStaMsg.iSrcId = pstFdSet->localSrc;
	
	initStaMsg.iMsgPara1 = pstFdSet->localSrc;
	initStaMsg.iMsgPara2 = cfg_down;
	initStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &initStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockInitdMsgReRunMeRequest(pid_t pid, UINT32 killDelay, UINT32 upDelay)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg initStaMsg;

	pstFdSet = &(gSockCommData.gstFdSet);

	initStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	initStaMsg.iMsgType = GPN_INITD_RERUN_ME_REQ;
	initStaMsg.iDstId =  GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId,  GPN_SOCK_ROLE_INIT);
	initStaMsg.iSrcId = pstFdSet->localSrc;
	initStaMsg.iMsgPara1 = pid;
	initStaMsg.iMsgPara2 = killDelay;
	initStaMsg.iMsgPara2 = upDelay;
	initStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &initStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockInitdMsgReRunHeRequest(pid_t pid, char *procName, UINT32 killDelay, UINT32 upDelay)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)+sockaddr_un(110)*/
	UINT8 msgBuff[GPN_INITD_CMD_STR_MAX_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pinitStaMsg;

	pstFdSet = &(gSockCommData.gstFdSet);

	pinitStaMsg = (gpnSockMsg *)msgBuff;
	pinitStaMsg->iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	pinitStaMsg->iMsgType = GPN_INITD_RERUN_HE_REQ;
	pinitStaMsg->iDstId =  GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId,  GPN_SOCK_ROLE_INIT);
	pinitStaMsg->iSrcId = pstFdSet->localSrc;
	pinitStaMsg->iMsgPara1 = pid;
	pinitStaMsg->iMsgPara2 = killDelay;
	pinitStaMsg->iMsgPara2 = upDelay;

	pinitStaMsg->msgCellLen = strlen(procName)+1;
	if( (pinitStaMsg->msgCellLen > GPN_SOCK_PAYLOAD_FIFO_BYTE_SIZE) ||
		(pinitStaMsg->msgCellLen > GPN_INITD_CMD_STR_MAX_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	memcpy((msgBuff+GPN_SOCK_MSG_HEAD_BLEN), procName, pinitStaMsg->msgCellLen);
	/*sure cmdStr's last char is '\0'*/
	msgBuff[GPN_SOCK_MSG_HEAD_BLEN+strlen(procName)] = '\0';

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pinitStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockInitdMsgRebootRequest(pid_t pid, UINT32 delay)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg initStaMsg;

	pstFdSet = &(gSockCommData.gstFdSet);

	initStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	initStaMsg.iMsgType = GPN_INITD_REBOOT_SYS_REQ;
	initStaMsg.iDstId =  GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	initStaMsg.iSrcId = pstFdSet->localSrc;
	initStaMsg.iMsgPara1 = pid;
	initStaMsg.iMsgPara2 = delay;
	initStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &initStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockInitdMsgKillRequest(pid_t pid, UINT32 delay)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg initStaMsg;

	pstFdSet = &(gSockCommData.gstFdSet);

	initStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	initStaMsg.iMsgType = GPN_INITD_KILL_ME_REQ;
	initStaMsg.iDstId =  GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	initStaMsg.iSrcId = pstFdSet->localSrc;
	initStaMsg.iMsgPara1 = pid;
	initStaMsg.iMsgPara2 = delay;
	initStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &initStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockInitdMsgFatalFail(pid_t pid, UINT32 delay, UINT32 method)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg initStaMsg;

	pstFdSet = &(gSockCommData.gstFdSet);

	initStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	initStaMsg.iMsgType = GPN_INITD_PROCESS_FAIL_NOT;
	initStaMsg.iDstId =  GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	initStaMsg.iSrcId = pstFdSet->localSrc;
	initStaMsg.iMsgPara1 = pid;
	initStaMsg.iMsgPara2 = delay;
	initStaMsg.iMsgPara3 = method;
	initStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &initStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockInitdMsgHwcMonAdd(pid_t pid, char *hwcStr)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)+sockaddr_un(110)*/
	UINT8 msgBuff[GPN_INITD_CMD_STR_MAX_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pinitStaMsg;

	/*assert*/
	if(hwcStr == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstFdSet = &(gSockCommData.gstFdSet);
	
	pinitStaMsg = (gpnSockMsg *)msgBuff;
	pinitStaMsg->iMsgCtrl = 0;
	pinitStaMsg->iMsgType = GPN_INITD_MON_ADD_HW_CHECK;
	pinitStaMsg->iDstId =  GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	pinitStaMsg->iSrcId = pstFdSet->localSrc;
	pinitStaMsg->iMsgPara1 = pid;
	
	pinitStaMsg->msgCellLen = strlen(hwcStr)+1;
	if( (pinitStaMsg->msgCellLen > GPN_SOCK_PAYLOAD_FIFO_BYTE_SIZE) ||
		(pinitStaMsg->msgCellLen > GPN_INITD_CMD_STR_MAX_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	memcpy((msgBuff+GPN_SOCK_MSG_HEAD_BLEN), hwcStr, pinitStaMsg->msgCellLen);
	/*sure cmdStr's last char is '\0'*/
	msgBuff[GPN_SOCK_MSG_HEAD_BLEN+strlen(hwcStr)] = '\0';

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen)));
	/*gpnSockGuaranteeMsgTxProc(&(pstFdSet->fdOptInfo[pinitStaMsg->iDstId]), (void *)pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen));*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockInitdMsgPorcessMonAdd(pid_t pid, char *processStr)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)+sockaddr_un(110)*/
	UINT8 msgBuff[GPN_INITD_CMD_STR_MAX_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pinitStaMsg;

	/*assert*/
	if(processStr == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstFdSet = &(gSockCommData.gstFdSet);
	
	pinitStaMsg = (gpnSockMsg *)msgBuff;
	pinitStaMsg->iMsgCtrl = 0;
	pinitStaMsg->iMsgType = GPN_INITD_MON_ADD_PROCESS;
	pinitStaMsg->iDstId =  GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	pinitStaMsg->iSrcId = pstFdSet->localSrc;
	pinitStaMsg->iMsgPara1 = pid;
	
	pinitStaMsg->msgCellLen = strlen(processStr)+1;
	if( (pinitStaMsg->msgCellLen > GPN_SOCK_PAYLOAD_FIFO_BYTE_SIZE) ||
		(pinitStaMsg->msgCellLen > GPN_INITD_CMD_STR_MAX_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	memcpy((msgBuff+GPN_SOCK_MSG_HEAD_BLEN), processStr, pinitStaMsg->msgCellLen);
	/*sure cmdStr's last char is '\0'*/
	msgBuff[GPN_SOCK_MSG_HEAD_BLEN+strlen(processStr)] = '\0';

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen)));
	/*gpnSockGuaranteeMsgTxProc(&(pstFdSet->fdOptInfo[pinitStaMsg->iDstId]), (void *)pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen));*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockInitdMsgPorcessMonDel(pid_t pid, char *processStr)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)+sockaddr_un(110)*/
	UINT8 msgBuff[GPN_INITD_CMD_STR_MAX_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pinitStaMsg;

	/*assert*/
	if(processStr == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstFdSet = &(gSockCommData.gstFdSet);
	
	pinitStaMsg = (gpnSockMsg *)msgBuff;
	pinitStaMsg->iMsgCtrl = 0;
	pinitStaMsg->iMsgType = GPN_INITD_MON_DEL_PROCESS;
	pinitStaMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	pinitStaMsg->iSrcId = pstFdSet->localSrc;
	pinitStaMsg->iMsgPara1 = pid;
	
	pinitStaMsg->msgCellLen = strlen(processStr)+1;
	if( (pinitStaMsg->msgCellLen > GPN_SOCK_PAYLOAD_FIFO_BYTE_SIZE) ||
		(pinitStaMsg->msgCellLen > GPN_INITD_CMD_STR_MAX_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	memcpy((msgBuff+GPN_SOCK_MSG_HEAD_BLEN), processStr, pinitStaMsg->msgCellLen);
	/*sure cmdStr's last char is '\0'*/
	msgBuff[GPN_SOCK_MSG_HEAD_BLEN+strlen(processStr)] = '\0';

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen)));
	/*gpnSockGuaranteeMsgTxProc(&(pstFdSet->fdOptInfo[pinitStaMsg->iDstId]), (void *)pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen));*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockInitdMsgAdMonAdd(pid_t pid, char *starupDlyStr)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)+sockaddr_un(110)*/
	UINT8 msgBuff[GPN_INITD_CMD_STR_MAX_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pinitStaMsg;

	/*assert*/
	if(starupDlyStr == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstFdSet = &(gSockCommData.gstFdSet);

	pinitStaMsg = (gpnSockMsg *)msgBuff;
	pinitStaMsg->iMsgCtrl = 0;
	pinitStaMsg->iMsgType = GPN_INITD_MON_ADD_STARUP_DELAY;
	pinitStaMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	pinitStaMsg->iSrcId = pstFdSet->localSrc;
	pinitStaMsg->iMsgPara1 = pid;
	
	pinitStaMsg->msgCellLen = strlen(starupDlyStr)+1;
	if( (pinitStaMsg->msgCellLen > GPN_SOCK_PAYLOAD_FIFO_BYTE_SIZE) ||
		(pinitStaMsg->msgCellLen > GPN_INITD_CMD_STR_MAX_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	memcpy((msgBuff+GPN_SOCK_MSG_HEAD_BLEN), starupDlyStr, pinitStaMsg->msgCellLen);
	/*sure cmdStr's last char is '\0'*/
	msgBuff[GPN_SOCK_MSG_HEAD_BLEN+strlen(starupDlyStr)] = '\0';

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen)));
	/*gpnSockGuaranteeMsgTxProc(&(pstFdSet->fdOptInfo[pinitStaMsg->iDstId]), (void *)pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen));*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockInitdMsgMethodMonAdd(pid_t pid, char *methodStr)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)+sockaddr_un(110)*/
	UINT8 msgBuff[GPN_INITD_CMD_STR_MAX_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pinitStaMsg;

	/*assert*/
	if(methodStr == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstFdSet = &(gSockCommData.gstFdSet);

	pinitStaMsg = (gpnSockMsg *)msgBuff;
	pinitStaMsg->iMsgCtrl = 0;
	pinitStaMsg->iMsgType = GPN_INITD_MON_ADD_METHOD;
	pinitStaMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	pinitStaMsg->iSrcId = pstFdSet->localSrc;
	pinitStaMsg->iMsgPara1 = pid;
	
	pinitStaMsg->msgCellLen = strlen(methodStr)+1;
	if( (pinitStaMsg->msgCellLen > GPN_SOCK_PAYLOAD_FIFO_BYTE_SIZE) ||
		(pinitStaMsg->msgCellLen > GPN_INITD_CMD_STR_MAX_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	memcpy((msgBuff+GPN_SOCK_MSG_HEAD_BLEN), methodStr, pinitStaMsg->msgCellLen);
	/*sure cmdStr's last char is '\0'*/
	msgBuff[GPN_SOCK_MSG_HEAD_BLEN+strlen(methodStr)] = '\0';
	
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen)));
	/*gpnSockGuaranteeMsgTxProc(&(pstFdSet->fdOptInfo[pinitStaMsg->iDstId]), (void *)pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen));*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockInitdMsgNdMonAdd(pid_t pid, char *ndOptStr)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)+sockaddr_un(110)*/
	UINT8 msgBuff[GPN_INITD_CMD_STR_MAX_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pinitStaMsg;

	/*assert*/
	if(ndOptStr == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstFdSet = &(gSockCommData.gstFdSet);

	pinitStaMsg = (gpnSockMsg *)msgBuff;
	pinitStaMsg->iMsgCtrl = 0;
	pinitStaMsg->iMsgType = GPN_INITD_MON_ADD_ND_OPT;
	pinitStaMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	pinitStaMsg->iSrcId = pstFdSet->localSrc;
	pinitStaMsg->iMsgPara1 = pid;
	
	pinitStaMsg->msgCellLen = strlen(ndOptStr)+1;
	if( (pinitStaMsg->msgCellLen > GPN_SOCK_PAYLOAD_FIFO_BYTE_SIZE) ||
		(pinitStaMsg->msgCellLen > GPN_INITD_CMD_STR_MAX_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	memcpy((msgBuff+GPN_SOCK_MSG_HEAD_BLEN), ndOptStr, pinitStaMsg->msgCellLen);
	/*sure cmdStr's last char is '\0'*/
	msgBuff[GPN_SOCK_MSG_HEAD_BLEN+strlen(ndOptStr)] = '\0';

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen)));
	/*gpnSockGuaranteeMsgTxProc(&(pstFdSet->fdOptInfo[pinitStaMsg->iDstId]), (void *)pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen));*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockInitdMsgMNAAdd(pid_t pid, char *MNAOptStr)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)+sockaddr_un(110)*/
	UINT8 msgBuff[GPN_INITD_CMD_STR_MAX_LEN];
	/*msgHead(64)*/
	gpnSockMsg *pinitStaMsg;

	/*assert*/
	if(MNAOptStr == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstFdSet = &(gSockCommData.gstFdSet);

	pinitStaMsg = (gpnSockMsg *)msgBuff;
	pinitStaMsg->iMsgCtrl = 0;
	pinitStaMsg->iMsgType = GPN_INITD_MON_ADD_MNA;
	pinitStaMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	pinitStaMsg->iSrcId = pstFdSet->localSrc;
	pinitStaMsg->iMsgPara1 = pid;
	
	pinitStaMsg->msgCellLen = strlen(MNAOptStr)+1;
	if( (pinitStaMsg->msgCellLen > GPN_SOCK_PAYLOAD_FIFO_BYTE_SIZE) ||
		(pinitStaMsg->msgCellLen > GPN_INITD_CMD_STR_MAX_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	memcpy((msgBuff+GPN_SOCK_MSG_HEAD_BLEN), MNAOptStr, pinitStaMsg->msgCellLen);
	/*sure cmdStr's last char is '\0'*/
	msgBuff[GPN_SOCK_MSG_HEAD_BLEN+strlen(MNAOptStr)] = '\0';

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen)));
	/*gpnSockGuaranteeMsgTxProc(&(pstFdSet->fdOptInfo[pinitStaMsg->iDstId]), (void *)pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen));*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockInitdMsgMonRegisterNotify(pid_t pid,
										char *processStr,
										char *starupDlyStr,
										char *methodStr,
										char *ndOptStr,
										char *MNAOptStr)
{
	UINT32 reVal;

	reVal = GPN_SELECT_GEN_OK;
	if(GPN_SELECT_GEN_OK != gpnSockInitdMsgPorcessMonAdd(pid, processStr))
	{
		reVal = GPN_SELECT_GEN_ERR;
	}
	if(GPN_SELECT_GEN_OK != gpnSockInitdMsgAdMonAdd(pid, starupDlyStr))
	{
		reVal = GPN_SELECT_GEN_ERR;
	}
	if(GPN_SELECT_GEN_OK != gpnSockInitdMsgMethodMonAdd(pid, methodStr))
	{
		reVal = GPN_SELECT_GEN_ERR;
	}
	if(GPN_SELECT_GEN_OK != gpnSockInitdMsgNdMonAdd(pid, ndOptStr))
	{
		reVal = GPN_SELECT_GEN_ERR;
	}
	if(GPN_SELECT_GEN_OK != gpnSockInitdMsgMNAAdd(pid, MNAOptStr))
	{
		reVal = GPN_SELECT_GEN_ERR;
	}

	return reVal;
}
UINT32 gpnSockInitdMsgNeedMonAsk(UINT32 MODU_ID)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg initStaMsg;
	
	pstFdSet = &(gSockCommData.gstFdSet);

	initStaMsg.iMsgCtrl = 0;
	initStaMsg.iMsgType = GPN_INITD_MON_NEED_ASK;
	initStaMsg.iDstId  = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, MODU_ID);
	initStaMsg.iSrcId = pstFdSet->localSrc;	
	initStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &initStaMsg,\
		GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(&(pstFdSet->fdOptInfo[initStaMsg.]), (void *)(&initStaMsg),\
		GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockInitdMsgSnmpdCommityGet(UINT32 MODU_ID)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)*/
	gpnSockMsg initStaMsg;
	
	pstFdSet = &(gSockCommData.gstFdSet);

	initStaMsg.iMsgCtrl = 0;
	initStaMsg.iMsgType = GPN_INITD_SNMP_R_COMMITY_GET;
	initStaMsg.iDstId  = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, MODU_ID);

	initStaMsg.iSrcId = pstFdSet->localSrc;	
	initStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &initStaMsg,\
		GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(&(pstFdSet->fdOptInfo[initStaMsg.]), (void *)(&initStaMsg),\
		GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockInitdMsgNotifySnmpdCommity(char *commity, UINT32 len)
{
	stSockFdSet *pstFdSet;
	/*msgHead(64)+sockaddr_un(110)*/
	//UINT8 msgBuff[GPN_INITD_COMMITY_MAX_LEN];
	UINT8 msgBuff[192];
	/*msgHead(64)*/
	gpnSockMsg *pinitStaMsg;

	/*assert*/
	if( (commity == NULL) ||\
		(len > GPN_INITD_COMMITY_MAX_LEN) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstFdSet = &(gSockCommData.gstFdSet);

	pinitStaMsg = (gpnSockMsg *)msgBuff;
	pinitStaMsg->iMsgCtrl = 0;
	pinitStaMsg->iMsgType = GPN_INITD_NOTIFY_SNMPD_R_COMMITY;
	pinitStaMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_INIT);
	pinitStaMsg->iSrcId = pstFdSet->localSrc;
	pinitStaMsg->iMsgPara1 = len;
	
	pinitStaMsg->msgCellLen = strlen(commity)+1;
	if( (pinitStaMsg->msgCellLen > GPN_SOCK_PAYLOAD_FIFO_BYTE_SIZE) ||
		(pinitStaMsg->msgCellLen > (GPN_INITD_COMMITY_MAX_LEN+1)) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	memcpy((msgBuff+GPN_SOCK_MSG_HEAD_BLEN), commity, pinitStaMsg->msgCellLen);
	/*sure cmdStr's last char is '\0'*/
	msgBuff[GPN_SOCK_MSG_HEAD_BLEN+strlen(commity)] = '\0';

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen)));
	/*gpnSockGuaranteeMsgTxProc(&(pstFdSet->fdOptInfo[pinitStaMsg->iDstId]), (void *)pinitStaMsg,\
		((GPN_SOCK_MSG_HEAD_BLEN)+(pinitStaMsg->msgCellLen));*/
		
	return GPN_SELECT_GEN_OK;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_INITD_MSG_DEF_C_*/

