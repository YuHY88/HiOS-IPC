/**********************************************************
* file name: gpnSockCfgMgtMsgDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-05-30
* function: 
*    define details about communication between gpn_cfgmgt modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_CFGMGT_MSG_DEF_C_
#define _GPN_SOCK_CFGMGT_MSG_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <sys/types.h>
#include <unistd.h>

#include "socketComm/gpnSockCfgMgtMsgDef.h"
#include "socketComm/gpnSockCommRoleMan.h"

UINT32 gpnSockCfgMgtMsgTxNotifyTypeToSysMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_NOTIFY_TYPE_2_SYSMGT;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_SYSMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = slot;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxNotifyStatToSysMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT32 sta)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_NOTIFY_STAT_2_SYSMGT;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_SYSMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = slot;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.iMsgPara3 = sta;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}


UINT32 gpnSockCfgMgtMsgTxAlarmSynToSysMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 syn_stat)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_ALARM_SYN_2_SYSMGT;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_SYSMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;

	/*slot = 17, cpx1 and cpx2 's conf file in the nm not synced*/
	/*slot = 1~16, sub slot and nm 's conf file not synced*/
	cfgMgtMsg.iMsgPara1 = slot;
	cfgMgtMsg.iMsgPara2 = syn_stat;
	
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxTagFileReplace2CFGClient(stSockFdSet *pstFdSet,
	UINT32 CPUId, UINT32 dev_type, UINT32 file_type, UINT32 suffix_id)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_TAG_FILE_REPLACE;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(CPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = CPUId/*CPUId just same as slot*/;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.iMsgPara3 = file_type;
	cfgMgtMsg.iMsgPara4 = suffix_id;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxCfgRecoverOpt2CFGClient(stSockFdSet *pstFdSet,
	UINT32 CPUId, UINT32 dev_type, UINT32 file_type, UINT32 cmd)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(CPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = CPUId/*CPUId just same as slot*/;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.iMsgPara3 = file_type;
	cfgMgtMsg.iMsgPara4 = cmd;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxCfgSaveOpt2CFGClient(stSockFdSet *pstFdSet,
	UINT32 CPUId, UINT32 dev_type, UINT32 file_type, UINT32 cmd)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_SUB_CFG_SAVE_OPT;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(CPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = CPUId/*CPUId just same as slot*/;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.iMsgPara3 = file_type;
	cfgMgtMsg.iMsgPara4 = cmd;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;

}

UINT32 gpnSockCfgMgtMsgTxRebootSubDev2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 dev_type)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_REBOOT_SUB_DEV;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(CPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = CPUId/*CPUId just same as slot*/;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxNTPTimeSynOpt2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 dev_type, UINT32 tv_sec, UINT32 tv_usec, UINT32 zone)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_SUB_NTP_TIME_SYN_OPT;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(CPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = CPUId/*CPUId just same as slot*/;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.iMsgPara3 = tv_sec;
	cfgMgtMsg.iMsgPara4 = tv_usec;
	cfgMgtMsg.iMsgPara5 = zone;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxRecalculateMd5Opt2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 dev_type, UINT32 file_type)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_SUB_RECALCULATE_MD5_OPT;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(CPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = CPUId/*CPUId just same as slot*/;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.iMsgPara3 = file_type;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxInformSyncedOpt2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 dev_type, UINT32 synced)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_SST_SUB_INFORM_SYNCED_OPT;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(CPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = CPUId/*CPUId just same as slot*/;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.iMsgPara3 = synced;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxCardRegisterRsp2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_CARD_REGISTER_RSP;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(CPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = CPUId/*CPUId just same as slot*/;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxTagFileReplaceRsp2CFGServer(stSockFdSet *pstFdSet,
	UINT32 NMxCPUId, UINT32 subSlot, UINT32 dev_type, UINT32 file_type, UINT32 suffix_id, UINT32 result)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_TAG_FILE_REPLACE_RSP;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(NMxCPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = subSlot;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.iMsgPara3 = file_type;
	cfgMgtMsg.iMsgPara4 = suffix_id;
	cfgMgtMsg.iMsgPara5 = result;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxCfgRecoverOptRsp2CFGServer(stSockFdSet *pstFdSet,
	UINT32 NMxCPUId, UINT32 subSlot, UINT32 dev_type, UINT32 file_type, UINT32 cmd, UINT32 result)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT_RSP;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(NMxCPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = subSlot;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.iMsgPara3 = file_type;
	cfgMgtMsg.iMsgPara4 = cmd;
	cfgMgtMsg.iMsgPara5 = result;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxCfgSaveOptRsp2CFGServer(stSockFdSet *pstFdSet,
	UINT32 NMxCPUId, UINT32 subSlot, UINT32 dev_type, UINT32 file_type, UINT32 cmd, UINT32 result)
{
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_SUB_CFG_SAVE_OPT_RSP;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(NMxCPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = subSlot;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.iMsgPara3 = file_type;
	cfgMgtMsg.iMsgPara4 = cmd;
	cfgMgtMsg.iMsgPara5 = result;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxNotifySta2CFGServer(stSockFdSet *pstFdSet,
	UINT32 NMxCPUId, UINT32 subSlot, UINT32 dev_type, UINT32 file_type,  UINT32 synced, UINT32 config_done,INT8 *md5)
{
	UINT8 msgBuff[GPN_CFGMGT_MD5_STA_NOTIFY_MSG_LEN];
	gpnSockMsg *cfgMgtMsg;

	/*assert*/
	if( (pstFdSet == NULL) ||\
		(md5 == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg = (gpnSockMsg *)msgBuff;
	
	cfgMgtMsg->iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg->iMsgType = GPN_CFGMGT_MSG_SUB_NOTIFY_STA;
	
	cfgMgtMsg->iDstId = GPN_SOCK_CREAT_DIREC(NMxCPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg->iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg->iMsgPara1 = subSlot;
	cfgMgtMsg->iMsgPara2 = dev_type;
	cfgMgtMsg->iMsgPara3 = file_type;
	cfgMgtMsg->iMsgPara4 = synced;
	cfgMgtMsg->iMsgPara5 = config_done;

	memcpy((msgBuff + GPN_SOCK_MSG_HEAD_BLEN), md5, GPN_CFGMGT_MD5_INFO_BLEN);
	cfgMgtMsg->msgCellLen = GPN_CFGMGT_MD5_INFO_BLEN;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, cfgMgtMsg, GPN_CFGMGT_MD5_STA_NOTIFY_MSG_LEN);
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxCardRegister2CFGServer(stSockFdSet *pstFdSet,	UINT32 NMxCPUId, UINT32 subSlot)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_CARD_REGISTER;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(NMxCPUId, GPN_SOCK_ROLE_CFGMGT);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;
	
	cfgMgtMsg.iMsgPara1 = subSlot;
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockCfgMgtMsgTxProMac2NSMModule(stSockFdSet *pstFdSet, UINT32 subSlot, UINT32 dev_type, UINT8 * proMac)
{
	/*msgHead(64)*/
	gpnSockMsg cfgMgtMsg = {0};

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	cfgMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	cfgMgtMsg.iMsgType = GPN_CFGMGT_MSG_SUB_PROMAC_2_NSM;
	
	cfgMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_NSM);
	cfgMgtMsg.iSrcId = pstFdSet->localSrc;


	cfgMgtMsg.iMsgPara1 = subSlot;
	cfgMgtMsg.iMsgPara2 = dev_type;
	cfgMgtMsg.iMsgPara3 = 0;
	cfgMgtMsg.iMsgPara4 = ((proMac[0] << 1) | proMac[1]);
	cfgMgtMsg.iMsgPara5 = (((UINT32)proMac[2] << 24) | ((UINT32)proMac[3] << 16) | ((UINT32)proMac[4] << 8) | (UINT32)proMac[5]);
	cfgMgtMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &cfgMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);
	
	return GPN_SELECT_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SOCK_CFGMGT_MSG_DEF_C_*/

