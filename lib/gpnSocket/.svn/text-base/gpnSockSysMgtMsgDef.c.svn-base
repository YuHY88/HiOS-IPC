/**********************************************************
* file name: gpnSockSysMgtMsgDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-05-30
* function: 
*    define details about communication between gpn_sysmgt modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_SYSMGT_MSG_DEF_C_
#define _GPN_SOCK_SYSMGT_MSG_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <sys/types.h>
#include <unistd.h>

#include "socketComm/gpnDebugFuncApi.h"

#include "socketComm/gpnSockSysMgtMsgDef.h"
#include "socketComm/gpnSockCommRoleMan.h"

#define GPN_SOCK_SYSMGT_PRINT(level, info...)	GEN_SYS_DEBUG_PRINT((level), info)


UINT32 gpnSockSysMgtMsgTxCardSure2CfgMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT32 sta)
{
	/*msgHead(64)*/
	gpnSockMsg sysMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	sysMgtMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	sysMgtMsg.iMsgType = GPN_SYSMGT_MSG_DEV_STA_SURE;

	sysMgtMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_CFGMGT);
	sysMgtMsg.iSrcId = pstFdSet->localSrc;

	sysMgtMsg.iMsgPara1 = slot;
	sysMgtMsg.iMsgPara2 = dev_type;
	sysMgtMsg.iMsgPara3 = sta;
	sysMgtMsg.msgCellLen = 0;

	/**/
	GPN_SOCK_SYSMGT_PRINT(GEN_SYS_DEBUG_UD0, "sysmgt : tx gpnSockSysMgtMsgTxCardSure2CfgMgt slot=%d\n", slot);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &sysMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);

	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSysMgtMsgTxCardCfgRecover2CfgMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT32 file_type, UINT32 act)
{
	/*msgHead(64)*/
	gpnSockMsg sysMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	sysMgtMsg.iMsgCtrl		= GPN_SOCK_MSG_ACK_NEED_BIT;
	sysMgtMsg.iMsgType		= GPN_SYSMGT_MSG_SUB_CFG_RECOVER_OPT;
	sysMgtMsg.iDstId		= GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_CFGMGT);
	sysMgtMsg.iSrcId		= pstFdSet->localSrc;
	sysMgtMsg.iMsgPara1		= slot;
	sysMgtMsg.iMsgPara2		= dev_type;
	sysMgtMsg.iMsgPara3		= file_type;
	sysMgtMsg.iMsgPara4		= act;
	sysMgtMsg.msgCellLen	= 0;

	/**/
	GPN_SOCK_SYSMGT_PRINT(GEN_SYS_DEBUG_UD0, "sysmgt : tx gpnSockSysMgtMsgTxCardCfgRecover2CfgMgt slot=%d, act=%d\n", slot, act);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &sysMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);

	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSysMgtMsgTxCardReboot2CfgMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type)
{
	/*msgHead(64)*/
	gpnSockMsg sysMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	sysMgtMsg.iMsgCtrl		= GPN_SOCK_MSG_ACK_NEED_BIT;
	sysMgtMsg.iMsgType		= GPN_SYSMGT_MSG_SUB_REBOOT_OPT;
	sysMgtMsg.iDstId		= GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_CFGMGT);
	sysMgtMsg.iSrcId		= pstFdSet->localSrc;
	sysMgtMsg.iMsgPara1		= slot;
	sysMgtMsg.iMsgPara2		= dev_type;
	sysMgtMsg.msgCellLen	= 0;

	/**/
	GPN_SOCK_SYSMGT_PRINT(GEN_SYS_DEBUG_UD0, "sysmgt : tx gpnSockSysMgtMsgTxCardReboot2CfgMgt slot=%d\n", slot);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &sysMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);

	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSysMgtMsgTxCardSave2CfgMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT32 file_type, UINT32 act)
{
	/*msgHead(64)*/
	gpnSockMsg sysMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	sysMgtMsg.iMsgCtrl		= GPN_SOCK_MSG_ACK_NEED_BIT;
	sysMgtMsg.iMsgType		= GPN_SYSMGT_MSG_SUB_SAVE_OPT;
	sysMgtMsg.iDstId		= GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_CFGMGT);
	sysMgtMsg.iSrcId		= pstFdSet->localSrc;
	sysMgtMsg.iMsgPara1		= slot;
	sysMgtMsg.iMsgPara2		= dev_type;
	sysMgtMsg.iMsgPara3		= file_type;
	sysMgtMsg.iMsgPara4		= act;
	sysMgtMsg.msgCellLen	= 0;

	/**/
	GPN_SOCK_SYSMGT_PRINT(GEN_SYS_DEBUG_UD0, "sysmgt : tx gpnSockSysMgtMsgTxCardSave2CfgMgt slot=%d act=%d\n", slot, act);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &sysMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);

	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSysMgtMsgTxCardStatReqMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type)
{
	/*msgHead(64)*/
	gpnSockMsg sysMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	sysMgtMsg.iMsgCtrl		= 0;
	sysMgtMsg.iMsgType		= GPN_SYSMGT_MSG_DEV_STA_REQ;
	sysMgtMsg.iDstId		= GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_CFGMGT);
	sysMgtMsg.iSrcId		= pstFdSet->localSrc;
	sysMgtMsg.iMsgPara1		= slot;
	sysMgtMsg.iMsgPara2		= dev_type;
	sysMgtMsg.msgCellLen	= 0;

	/**/
	GPN_SOCK_SYSMGT_PRINT(GEN_SYS_DEBUG_UD0, "sysmgt : tx gpnSockSysMgtMsgTxCardStatReqMgt slot=%d\n", slot);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &sysMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);

	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSysMgtMsgTxCardL2CfgMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT8 * mac, UINT8 * nm_mac, UINT32 vlan_en, UINT16 vlan_id)
{
	/*msgHead(64)*/
	gpnSockMsg sysMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	sysMgtMsg.iMsgCtrl		= GPN_SOCK_MSG_ACK_NEED_BIT;
	sysMgtMsg.iMsgType		= GPN_SYSMGT_MSG_SUB_L2CFG_NOTIFY;
	sysMgtMsg.iDstId		= GPN_SOCK_CREAT_DIREC(slot, GPN_SOCK_ROLE_CFGMGT);
	sysMgtMsg.iSrcId		= pstFdSet->localSrc;
	sysMgtMsg.iMsgPara1		= slot;
	sysMgtMsg.iMsgPara2		= dev_type;
	sysMgtMsg.iMsgPara3		= ((UINT32)mac[0] <<  8) | mac[1];
	sysMgtMsg.iMsgPara4		= ((UINT32)mac[2] << 24) | ((UINT32)mac[3] << 16) | ((UINT32)mac[4] << 8) | mac[5];
	sysMgtMsg.iMsgPara5		= ((UINT32)nm_mac[0] <<  8) | nm_mac[1];
	sysMgtMsg.iMsgPara6		= ((UINT32)nm_mac[2] << 24) | ((UINT32)nm_mac[3] << 16) | ((UINT32)nm_mac[4] << 8) | nm_mac[5];
	sysMgtMsg.iMsgPara7		= vlan_en;
	sysMgtMsg.iMsgPara8		= (UINT32)vlan_id;
	sysMgtMsg.msgCellLen	= 0;

	/**/
	GPN_SOCK_SYSMGT_PRINT(GEN_SYS_DEBUG_UD0, \
	"sysmgt : tx gpnSockSysMgtMsgTxCardL2CfgMgt slot=%d mac=%02x%02x:%02x%02x:%02x%02x nm_mac=%02x%02x:%02x%02x:%02x%02x vlan_en=%d vlan_id=%04x\n", \
					slot, \
					mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], \
					nm_mac[0], nm_mac[1], nm_mac[2], nm_mac[3], nm_mac[4], nm_mac[5], \
					vlan_en, vlan_id );

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &sysMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);

	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSysMgtMsgTxCPXMasterSlaverMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT32 master_slot)
{
	/*msgHead(64)*/
	gpnSockMsg sysMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	sysMgtMsg.iMsgCtrl		= GPN_SOCK_MSG_ACK_NEED_BIT;
	sysMgtMsg.iMsgType		= GPN_SYSMGT_MSG_SUB_CPX_MASTER_SLAVER;
	sysMgtMsg.iDstId		= GPN_SOCK_CREAT_DIREC(slot, GPN_SOCK_ROLE_CFGMGT);
	sysMgtMsg.iSrcId		= pstFdSet->localSrc;
	sysMgtMsg.iMsgPara1		= slot;
	sysMgtMsg.iMsgPara2		= dev_type;
	sysMgtMsg.iMsgPara3		= master_slot;
	sysMgtMsg.msgCellLen	= 0;

	/**/
	GPN_SOCK_SYSMGT_PRINT(GEN_SYS_DEBUG_UD0, "sysmgt : tx gpnSockSysMgtMsgTxCPXMasterSlaverMgt slot=%d master_slot=%d\n", slot, master_slot );

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &sysMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);

	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockSysMgtMsgTxCPXCfgSynMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 action)
{
	/*msgHead(64)*/
	gpnSockMsg sysMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	sysMgtMsg.iMsgCtrl		= GPN_SOCK_MSG_ACK_NEED_BIT;
	sysMgtMsg.iMsgType		= GPN_SYSMGT_MSG_SUB_CPX_CFG_ASYN_OPT;
	sysMgtMsg.iDstId		= GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, GPN_SOCK_ROLE_CFGMGT);
	sysMgtMsg.iSrcId		= pstFdSet->localSrc;

	sysMgtMsg.iMsgPara1		= slot;
	sysMgtMsg.iMsgPara2		= action;

	sysMgtMsg.msgCellLen	= 0;

	/**/
	GPN_SOCK_SYSMGT_PRINT(GEN_SYS_DEBUG_UD0, "sysmgt : tx gpnSockSysMgtMsgTxCPXCfgSynMgt slot(%d) action(%d)\n", slot, action );

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &sysMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);

	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockSysMgtMsgTxCardL3InbandIpCfgMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT32 inbandIp)
{
	/*msgHead(64)*/
	gpnSockMsg sysMgtMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	sysMgtMsg.iMsgCtrl		= GPN_SOCK_MSG_ACK_NEED_BIT;
	sysMgtMsg.iMsgType		= GPN_SYSMGT_MSG_INBAND_IP_NOTIFY;
	sysMgtMsg.iDstId		= GPN_SOCK_CREAT_DIREC(slot, GPN_SOCK_ROLE_CFGMGT);
	sysMgtMsg.iSrcId		= pstFdSet->localSrc;
	sysMgtMsg.iMsgPara1		= slot;
	sysMgtMsg.iMsgPara2		= dev_type;
	sysMgtMsg.iMsgPara3		= inbandIp;
	sysMgtMsg.msgCellLen	= 0;

	/**/
	GPN_SOCK_SYSMGT_PRINT(GEN_SYS_DEBUG_UD0, \
	"sysmgt : tx gpnSockSysMgtMsgTxCardL3InbandIpCfgMgt slot=%d inbandIp %d.%d.%d.%d\n", \
					slot, \
					(UINT8)((inbandIp >> 24) & 0x000000FF),\
					(UINT8)((inbandIp >> 16) & 0x000000FF),\
					(UINT8)((inbandIp >>  8) & 0x000000FF),\
					(UINT8)((inbandIp >>  0) & 0x000000FF));

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &sysMgtMsg, GPN_SOCK_MSG_HEAD_BLEN);

	return GPN_SELECT_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SOCK_SYSMGT_MSG_DEF_C_*/

