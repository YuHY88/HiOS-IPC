/**********************************************************
* file name: gpnSockStatMsgDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-25
* function: 
*    define details about communication between gpn_stat modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_STAT_MSG_DEF_C_
#define _GPN_SOCK_STAT_MSG_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <sys/types.h>
#include <unistd.h>

#include "lib/msg_ipc_n.h"
#include "lib/memshare.h"

#include "socketComm/gpnSockStatMsgDef.h"
#include "socketComm/gpnSockCommModuDef.h"
#include "socketComm/gpnSockCommRoleMan.h"

/*socket communication global data:define in socket communication module*/
extern sockCommData gSockCommData;



static UINT32 gpnSendIpcMsg(void *pData, int data_len, int data_num, int module_id)
{
	int  iRetVal = 0;
    int  iDatalen = data_len;
    
    struct ipc_mesg_n *pSndMsg = NULL;

    pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + iDatalen, module_id);
	if(pSndMsg == NULL) 
	{
		printf("%s[%d] : mem share malloc error\n", __func__, __LINE__);
		return 0;
    }
	
	memset(pSndMsg, 0, (sizeof(struct ipc_msghdr_n) + iDatalen));

	if(MODULE_ID_STAT != module_id)
	{
		pSndMsg->msghdr.sender_id = MODULE_ID_STAT;
	}
	else
	{
		pSndMsg->msghdr.sender_id = 0;
	}
	
    pSndMsg->msghdr.module_id   = module_id;
    //pSndMsg->msghdr.sender_id   = 0;
    pSndMsg->msghdr.msg_type    = IPC_TYPE_STAT_DB;
    pSndMsg->msghdr.msg_subtype = 0;
    pSndMsg->msghdr.opcode      = 0;
    pSndMsg->msghdr.data_num    = data_num;
    pSndMsg->msghdr.data_len    = (unsigned int)iDatalen;

	memcpy(pSndMsg->msg_data, pData, iDatalen);

    iRetVal = ipc_send_msg_n1(pSndMsg, (sizeof(struct ipc_msghdr_n) + iDatalen));

	/* ¡¤¡é?¨ª¨º¡ì¡ã¨¹¡ê?D¨¨¨°a¡À???¨º¨ª¡¤?12?¨ª?¨²¡ä? */
    if(-1 == iRetVal)	
    {
		mem_share_free(pSndMsg, module_id);
    }

	return 1;
}


static int gpnCommm2IpranModule(UINT32 dstModu)
{
	int module_id = 0;
	switch(dstModu)
	{
		case GPN_COMMM_ALM:
			module_id = MODULE_ID_ALARM;
			break;

		case GPN_COMMM_IFM:
			module_id = MODULE_ID_IFM;
			break;

		case GPN_COMMM_STAT:
			module_id = MODULE_ID_STAT;
			break;

		case GPN_COMMM_MPLS:
			module_id = MODULE_ID_MPLS;
			break;

		case GPN_COMMM_CLOCK:
			module_id = MODULE_ID_CLOCK;
			break;

		case GPN_COMMM_SYSTEM:
			module_id = MODULE_ID_SYSTEM;
			break;

		case GPN_COMMM_DEVM:
			module_id = MODULE_ID_DEVM;
			break;

		case GPN_COMMM_HAL:
			module_id = MODULE_ID_HAL;
			break;

		case GPN_COMMM_L2:
			module_id = MODULE_ID_L2;
			break;

		case GPN_COMMM_CES:
			module_id = MODULE_ID_CES;
			break;

		case GPN_COMMM_FTM:
			module_id = MODULE_ID_FTM;
			break;

		default:
			break;
	}
	return module_id;
}


UINT32 gpnSockStatMsgApiPortStatMonEnable(optObjOrient *pportIndex, UINT32 statType, UINT32 dstModu)
{
	gpnSockMsg statMsg;	

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	int module_id = gpnCommm2IpranModule(dstModu);

	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, dstModu);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;/*GPN_COMMM_STAT*/
	
	statMsg.iMsgType = GPN_STAT_MSG_PORT_STAT_MON_ENABLE;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	statMsg.iMsgPara7 = statType;
	/*cyc, lipf add*/
	statMsg.iMsgPara8 = gpnStatCalcCycle(pportIndex, statType);
	
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, module_id);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}


UINT32 gpnSockStatMsgApiPortStatMonEnableRsp(optObjOrient *pportIndex, UINT32 scanType, UINT32 enStat)
{
	//int Reval;
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;
	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_PORT_STAT_MON_ENABLE_RSP;	
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	statMsg.iMsgPara7 = scanType;
	statMsg.iMsgPara8 = enStat;
	statMsg.iMsgPara9 = (enStat == 1) ? GPN_SELECT_GEN_OK: GPN_SELECT_GEN_ERR;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_STAT);

	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiPortStatMonDisable(optObjOrient *pportIndex, UINT32 statType, UINT32 dstModu)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	int module_id = gpnCommm2IpranModule(dstModu);

	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, dstModu);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;/*GPN_COMMM_STAT*/
	
	statMsg.iMsgType = GPN_STAT_MSG_PORT_STAT_MON_DISABLE;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, module_id);

	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockStatMsgApiPortStatMonReplace(optObjOrient *pOldPort, optObjOrient *pNewPort, UINT32 dstModu)
{
	gpnSockMsg statMsg;

	/*assert */
	if( (pOldPort == NULL) ||\
		(pNewPort == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	int module_id = gpnCommm2IpranModule(dstModu);

	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, dstModu);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;/*GPN_COMMM_STAT*/
	
	statMsg.iMsgType = GPN_STAT_MSG_STAT_MON_PORT_REPLACE;
	statMsg.iMsgPara1 = pOldPort->devIndex;
	statMsg.iMsgPara2 = pOldPort->portIndex;
	statMsg.iMsgPara3 = pOldPort->portIndex3;
	statMsg.iMsgPara4 = pOldPort->portIndex4;
	statMsg.iMsgPara5 = pOldPort->portIndex5;
	statMsg.iMsgPara6 = pOldPort->portIndex6;
	
	statMsg.iMsgPara7 = pNewPort->devIndex;
	statMsg.iMsgPara8 = pNewPort->portIndex;
	statMsg.iMsgPara9 = pNewPort->portIndex3;
	statMsg.iMsgParaA = pNewPort->portIndex4;
	statMsg.iMsgParaB = pNewPort->portIndex5;
	statMsg.iMsgParaC = pNewPort->portIndex6;
	
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, module_id);

	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiPortStatMonDisableRsp(optObjOrient *pportIndex, UINT32 scanType, UINT32 disStat)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_PORT_STAT_MON_DISABLE_RSP;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	statMsg.iMsgPara7 = scanType;
	statMsg.iMsgPara8 = disStat;
	statMsg.iMsgPara9 = (disStat == 1) ? GPN_SELECT_GEN_OK: GPN_SELECT_GEN_ERR;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_STAT);

	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiNsmPtnVsPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_PTN_VS_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_PTN_VS_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockStatMsgApiPtnVsMStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_PTN_VS_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_PTN_VS_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiNsmPtnLspPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_PTN_LSP_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_PTN_LSP_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockStatMsgApiPtnLspMStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_PTN_LSP_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_PTN_LSP_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiNsmPtnPwPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_PTN_PW_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_PTN_PW_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockStatMsgApiPtnPwMStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_PTN_PW_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_PTN_PW_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiNsmPtnVplsPwPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_PTN_VPLSPW_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_PTN_VPLSPW_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockStatMsgApiNsmPtnVplsPwMStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_PTN_VPLSPW_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_PTN_VPLSPW_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiNsmEthPhyPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_ETH_MAC_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_ETH_MAC_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockStatMsgApiEthMonStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_ETH_MAC_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_ETH_MAC_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockStatMsgApiSecMonStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_ETH_MAC_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_ETH_MAC_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiEthMonStatMonPortReplace(optObjOrient *pOldPort, optObjOrient *pNewPort)
{
	gpnSockStatMsgApiPortStatMonReplace(pOldPort, pNewPort, GPN_COMMM_HAL);
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiSdmSFPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_ETH_SFP_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_ETH_SFP_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiNsmFollowPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_FOLLOW_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_FOLLOW_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiNsmFollowMStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	#if 0
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_FOLLOW_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_FOLLOW_M_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	#else
	return GPN_SELECT_GEN_ERR;
	#endif
}
UINT32 gpnSockStatMsgApiNsmPtnVUNIPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_PTN_V_UNI_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_PTN_V_UNI_P_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockStatMsgApiOamPtnMepMStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_PTN_MEP_M_TYPE, GPN_COMMM_L2);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_PTN_MEP_M_TYPE, GPN_COMMM_L2);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiSdmEquStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		/* get CpuUtileza from system and get EquTemp from hal by lipf , 2018/3/23 */
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_EQU_TYPE, GPN_COMMM_SYSTEM);
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_EQU_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_EQU_TYPE, GPN_COMMM_SYSTEM);
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_EQU_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiSdmSoftStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_SOFT_TYPE, GPN_COMMM_SYSTEM);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_SOFT_TYPE, GPN_COMMM_SYSTEM);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiSdmEnvStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_ENV_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_ENV_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiTxPdhPpiStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiPdhPpiStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_PDH_PPI_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_PDH_PPI_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiNsmSFPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en)
{
	if(en == GPN_SELECT_GEN_ENABLE)
	{
		gpnSockStatMsgApiPortStatMonEnable(pportIndex, GPN_STAT_T_ETH_SFP_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else if(en == GPN_SELECT_GEN_DISABLE)
	{
		gpnSockStatMsgApiPortStatMonDisable(pportIndex, GPN_STAT_T_ETH_SFP_TYPE, GPN_COMMM_HAL);
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiTxPtnVsPStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_VS_P_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_VS_P_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxPtnVsPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_VS_P_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_VS_P_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_VS_P_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_VS_P_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_VS_P_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_VS_P_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_VS_P_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_VS_P_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiNsmPtnVsCounterReset(optObjOrient *pportIndex, UINT32 scanType, UINT32 subType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_VS_STAT_RESET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType meaningless index */
	statMsg.iMsgPara7 = scanType;
	statMsg.iMsgPara7 = subType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockStatMsgApiTxPtnVsMStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_VS_M_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_VS_M_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxPtnVsMStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_VS_M_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_VS_M_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_VS_M_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_VS_M_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_VS_M_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_VS_M_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_VS_M_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_VS_M_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiTxPtnLspPStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_LSP_P_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_LSP_P_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxPtnLspPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_LSP_P_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_LSP_P_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_LSP_P_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_LSP_P_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_LSP_P_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_LSP_P_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_LSP_P_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_LSP_P_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/

		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiNsmPtnLspCounterReset(optObjOrient *pportIndex, UINT32 scanType, UINT32 subType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_LSP_STAT_RESET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType meaningless index */
	statMsg.iMsgPara7 = scanType;
	statMsg.iMsgPara7 = subType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockStatMsgApiTxPtnLspMStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_LSP_M_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_LSP_M_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxPtnLspMStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_LSP_M_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_LSP_M_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_LSP_M_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_LSP_M_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_LSP_M_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_LSP_M_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_LSP_M_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_LSP_M_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/

		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiTxPtnPwPStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_PW_P_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_LSP_P_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxPtnPwPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_PW_P_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_PW_P_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_PW_P_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_LSP_P_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_PW_P_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_PW_P_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_PW_P_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_PW_P_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiTxPtnVplsPwPStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_VPLSPW_P_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_LSP_P_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxPtnVplsPwPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_VPLSPW_P_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_PW_P_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_VPLSPW_P_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_LSP_P_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_VPLSPW_P_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_VPLSPW_P_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_VPLSPW_P_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_VPLSPW_P_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}


UINT32 gpnSockStatMsgApiNsmPtnPwCounterReset(optObjOrient *pportIndex, UINT32 scanType, UINT32 subType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_PW_STAT_RESET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType meaningless index */
	statMsg.iMsgPara7 = scanType;
	statMsg.iMsgPara7 = subType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockStatMsgApiTxPtnPwMStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_PW_M_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_PW_M_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxPtnPwMStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_PW_M_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_PW_M_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_PW_M_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_PW_M_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_PW_M_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_PW_M_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_PW_M_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_PW_M_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiTxPtnVplsPwMStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_VPLSPW_M_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_PW_M_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxPtnVplsPwMStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_VPLSPW_M_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_VPLSPW_M_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_VPLSPW_M_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_PTN_PW_M_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_VPLSPW_M_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_VPLSPW_M_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_VPLSPW_M_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_VPLSPW_M_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}


UINT32 gpnSockStatMsgApiTxNsmEthPhyPStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_ETH_PHY_P_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_MAC_P_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxNsmEthPhyPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_ETHPHY_P_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_ETHPHY_P_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_ETH_PHY_P_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_MAC_P_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_ETHPHY_P_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_ETHPHY_P_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_ETHPHY_P_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_ETHPHY_P_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockStatMsgApiTxSecMonStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_ETH_PHY_M_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_MAC_M_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxSecMonStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_ETHPHY_M_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_ETHPHY_M_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_ETH_PHY_M_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_MAC_M_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_ETHPHY_M_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_ETHPHY_M_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_ETHPHY_M_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_ETHPHY_M_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiTxEthMonStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_ETH_PHY_M_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_MAC_M_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxEthMonStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_ETHPHY_M_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_ETHPHY_M_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_ETH_PHY_M_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_MAC_M_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_ETHPHY_M_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_ETHPHY_M_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_ETHPHY_M_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_ETHPHY_M_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockStatMsgApiNsmEthCounterReset(optObjOrient *pportIndex, UINT32 scanType, UINT32 subType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_ETH_STAT_RESET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType meaningless index */
	statMsg.iMsgPara7 = scanType;
	statMsg.iMsgPara7 = subType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxSdmEthSFPStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_ETH_SFP_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_SFP_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxsdmEthSFPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_ETHSFP_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_ETHSFP_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_ETH_SFP_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_SFP_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_ETHSFP_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_ETHSFP_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_ETHSFP_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_ETHSFP_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxFollowPStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_FLOW_P_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxFollowPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_FLOW_P_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_FLOW_P_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_FLOW_P_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_FLOW_P_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_FLOW_P_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_FLOW_P_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_FLOW_P_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockStatMsgApiTxFollowMStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_FLOW_M_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxFollowMStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_FLOW_M_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_FLOW_M_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_FLOW_M_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_MAC_M_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_FLOW_M_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_FLOW_M_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_FLOW_M_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_FLOW_M_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiNsmFollowCounterReset(optObjOrient *pportIndex, UINT32 scanType, UINT32 subType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_MPLS);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_FLOW_STAT_RESET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType meaningless index */
	statMsg.iMsgPara7 = scanType;
	statMsg.iMsgPara7 = subType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockStatMsgApiTxPtnVUNIPStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxPtnVUNIPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_PTN_V_UNI_P_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_PTN_V_UNI_P_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_PTN_V_UNI_P_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_PTN_V_UNI_P_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_PTN_V_UNI_P_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_PTN_V_UNI_P_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockStatMsgApiTxPtnMepMStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_L2);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_PTN_MEP_M_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_L2);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxPtnMepMStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_PTN_MEP_M_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_PTN_MEP_M_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_PTN_MEP_M_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_MAC_M_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_PTN_MEP_M_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_PTN_MEP_M_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_PTN_MEP_M_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_PTN_MEP_M_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxSdmEquStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_EQU_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/

	/* get EquTemp from hal by lipf */
	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_SYSTEM);
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_SYSTEM);
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxSdmEquStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_EQU_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_EQU_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_EQU_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_EQU_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_EQU_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_EQU_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_EQU_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxSdmSoftStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_SYSTEM);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_SOFT_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_SYSTEM);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxSdmSoftStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_SOFT_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_SOFT_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_SOFT_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_SOFT_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_SOFT_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_SOFT_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_SOFT_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxSdmEnvStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_ENV_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxSdmEnvStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_ENV_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_ENV_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_ENV_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*scanType*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_ENV_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_ENV_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_ENV_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_ENV_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
}


UINT32 gpnSockStatMsgApiNsmFollowStatMonEnableRsp(optObjOrient *pportIndex, UINT32 scanType, UINT32 enStat)
{
	gpnSockStatMsgApiPortStatMonEnableRsp(pportIndex, scanType, enStat);
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiNsmFollowStatMonDisableRsp(optObjOrient *pportIndex, UINT32 scanType, UINT32 disStat)
{
	gpnSockStatMsgApiPortStatMonDisableRsp(pportIndex, scanType, disStat);
	return GPN_SELECT_GEN_OK;
}


UINT32 gpnSockStatMsgApiNsmFollowCounterResetRsp(optObjOrient *pportIndex, UINT32 scanType, UINT32 resetStat)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_FLOW_STAT_RESET;
	statMsg.iMsgPara1 = pportIndex->portIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex3;
	statMsg.iMsgPara3 = pportIndex->portIndex4;
	statMsg.iMsgPara4 = pportIndex->portIndex5;
	statMsg.iMsgPara5 = pportIndex->portIndex6;
	statMsg.iMsgPara6 = pportIndex->devIndex;
	statMsg.iMsgPara7 = scanType;
	statMsg.iMsgPara8 = resetStat;
	statMsg.iMsgPara9 = (resetStat == 1) ? GPN_SELECT_GEN_OK: GPN_SELECT_GEN_ERR;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_STAT);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiPdhPpiStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_PARA_PDHPPI_P_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_MAC_M_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiPdhPpiStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_PDHPPI_P_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_PDHPPI_P_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_PARA_PDHPPI_P_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_MAC_M_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_PDHPPI_P_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_PDHPPI_P_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_PDHPPI_P_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_PDHPPI_P_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxNsmEthSFPStatGet(optObjOrient *pportIndex, UINT32 statType)
{
	gpnSockMsg statMsg;

	/*assert */
	if(pportIndex == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}
	statMsg.iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//statMsg.iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_HAL);
	//statMsg.iSrcId = gSockCommData.gstFdSet.localSrc;
	
	statMsg.iMsgType = GPN_STAT_MSG_ETH_SFP_STAT_GET;
	statMsg.iMsgPara1 = pportIndex->devIndex;
	statMsg.iMsgPara2 = pportIndex->portIndex;
	statMsg.iMsgPara3 = pportIndex->portIndex3;
	statMsg.iMsgPara4 = pportIndex->portIndex4;
	statMsg.iMsgPara5 = pportIndex->portIndex5;
	statMsg.iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_SFP_TYPE;*/
	statMsg.iMsgPara7 = statType;
	statMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), &statMsg, GPN_SOCK_MSG_HEAD_BLEN);
	gpnSendIpcMsg(&statMsg, sizeof(gpnSockMsg), 1, MODULE_ID_HAL);
	
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockStatMsgApiTxNsmEthSFPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len)
{
	char msgBull[GPN_STAT_PARA_ETHSFP_STAT_GET_MSG_SIZE];
	gpnSockMsg *pstatMsg;

	/*assert */
	if( (pportIndex == NULL) ||\
		(pdata == NULL) ||\
		(len != GPN_STAT_PARA_ETHSFP_STAT_DATA_SIZE) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	pstatMsg = (gpnSockMsg *)msgBull;
	pstatMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	//pstatMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_STAT);
	//pstatMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	pstatMsg->iMsgType = GPN_STAT_MSG_ETH_SFP_STAT_GET_RSP;
	pstatMsg->iMsgPara1 = pportIndex->devIndex;
	pstatMsg->iMsgPara2 = pportIndex->portIndex;
	pstatMsg->iMsgPara3 = pportIndex->portIndex3;
	pstatMsg->iMsgPara4 = pportIndex->portIndex4;
	pstatMsg->iMsgPara5 = pportIndex->portIndex5;
	pstatMsg->iMsgPara6 = pportIndex->portIndex6;
	/*statType*/
	/*statMsg.iMsgPara7 = GPN_STAT_T_ETH_SFP_TYPE;*/
	pstatMsg->iMsgPara7 = statType;
	pstatMsg->msgCellLen = len;

	if(len == GPN_STAT_PARA_ETHSFP_STAT_DATA_SIZE)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, pdata, GPN_STAT_PARA_ETHSFP_STAT_DATA_SIZE);

		/*guarantee or not:not guarantee*/
		//gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), pstatMsg, GPN_STAT_PARA_ETHSFP_STAT_GET_MSG_SIZE);
		gpnSendIpcMsg(pstatMsg, GPN_STAT_PARA_ETHSFP_STAT_GET_MSG_SIZE, 1, MODULE_ID_STAT);
		
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	return GPN_SELECT_GEN_OK;
}


/* lipf add for calc cycle */
UINT32 gpnStatCalcCycle(optObjOrient *pportIndex, UINT32 statType)
{
	UINT32 cyc = 3;

	if(cyc > GPN_STAT_CYCLE_MAX)
		cyc = GPN_STAT_CYCLE_MAX;
	if(cyc < GPN_STAT_CYCLE_MIN)
		cyc = GPN_STAT_CYCLE_MIN;
	
	return cyc;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SOCK_STAT_MSG_DEF_C_*/

