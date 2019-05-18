/**********************************************************
* file name: gpnSockIfmMsgDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-28
* function: 
*    define details about communication between gpn_ifm modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_IFM_MSG_DEF_C_
#define _GPN_SOCK_IFM_MSG_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <sys/types.h>
#include <unistd.h>

#include "socketComm/gpnSockIfmMsgDef.h"
#include "socketComm/gpnSockCommModuDef.h"
#include "socketComm/gpnSockCommRoleMan.h"
#include "socketComm/gpnGlobalPortIndexDef.h"


/*socket communication global data:define in socket communication module*/
extern sockCommData gSockCommData;

UINT32 gpnSockIfmMsgTxDevStateNotify(stSockFdSet *pstFdSet, UINT32 commRole, UINT32 devIndex, UINT32 sta)
{
	/*msgHead(64)*/
	gpnSockMsg ifmStaMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	if( (commRole != GPN_COMMM_ALM) &&\
		(commRole != GPN_COMMM_STAT) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	if( (sta == GPN_SOCK_MSG_DEV_STA_NULL) ||\
		(sta == GPN_SOCK_MSG_DEV_STA_PULL) ||\
		(sta == GPN_SOCK_MSG_DEV_STA_INSERT) ||\
		(sta == GPN_SOCK_MSG_DEV_STA_RUN) ||\
		(sta == GPN_SOCK_MSG_DEV_STA_ERR) )
	{
		ifmStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;

		ifmStaMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, commRole);
		ifmStaMsg.iSrcId = pstFdSet->localSrc;
	
		ifmStaMsg.iMsgType = GPN_IFM_MSG_DEV_STATUS_CHANG;
		ifmStaMsg.iMsgPara1 = devIndex;
		/*god portIndex*/
		ifmStaMsg.iMsgPara8 = sta;
		ifmStaMsg.iMsgPara9 = pstFdSet->pid;
		ifmStaMsg.msgCellLen = 0;

		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pstFdSet, &ifmStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxFixPortCapabilityNotify(stSockFdSet *pstFdSet, UINT32 commRole, UINT32 devIndex, stFixPorcReg *pfixPortSet, UINT32 len)
{
	/*msgHead(64)+sockaddr_un(110)*/
	UINT8 msgBuff[1024];
	gpnSockMsg *pIfmCmdMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	if(len > ((1024-GPN_SOCK_MSG_HEAD_LEN)/4))
	{
		return GPN_SELECT_GEN_ERR;
	}

	if( (commRole != GPN_COMMM_ALM) &&\
		(commRole != GPN_COMMM_STAT) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	pIfmCmdMsg = (gpnSockMsg *)msgBuff;

	pIfmCmdMsg->iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;

	pIfmCmdMsg->iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, commRole);
	pIfmCmdMsg->iSrcId = pstFdSet->localSrc;
		
	pIfmCmdMsg->iMsgType = GPN_IFM_MSG_FIX_PORT_REG;
	pIfmCmdMsg->iMsgPara1 = devIndex;
	pIfmCmdMsg->iMsgPara2 = pstFdSet->pid;
	
	pIfmCmdMsg->msgCellLen = len;
	memcpy((msgBuff+GPN_SOCK_MSG_HEAD_BLEN), pfixPortSet, len);

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, pIfmCmdMsg, (GPN_SOCK_MSG_HEAD_BLEN+len));
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, (gpnSockMsg *)msgBuff, (GPN_SOCK_MSG_HEAD_BLEN+len));*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockIfmMsgTxVariPortCreatNotify(stSockFdSet *pstFdSet, UINT32 commRole, UINT32 vPortIndex, UINT32 opt)
{
	/*msgHead(64)*/
	gpnSockMsg ifmRegMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	if( (commRole != GPN_COMMM_ALM) &&\
		(commRole != GPN_COMMM_STAT) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	if( (opt == GPN_SOCK_MSG_OPT_CREAT) ||\
		(opt == GPN_SOCK_MSG_OPT_DELETE) )
	{
		/*almRegMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
		ifmRegMsg.iMsgCtrl = 0;

		ifmRegMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, commRole);
		ifmRegMsg.iSrcId = pstFdSet->localSrc;
		
		if(opt == GPN_SOCK_MSG_OPT_CREAT)
		{
			ifmRegMsg.iMsgType = GPN_IFM_MSG_VARI_PORT_REG;
		}
		else
		{
			ifmRegMsg.iMsgType = GPN_IFM_MSG_VARI_PORT_UNREG;
		}
		ifmRegMsg.iMsgPara1 = vPortIndex;
		ifmRegMsg.iMsgPara2 = GPN_GEN_MSG_INVALID_PORT;
		ifmRegMsg.iMsgPara3 = GPN_GEN_MSG_INVALID_PORT;
		ifmRegMsg.iMsgPara4 = GPN_GEN_MSG_INVALID_PORT;
		ifmRegMsg.iMsgPara5 = GPN_GEN_MSG_INVALID_PORT;
		ifmRegMsg.iMsgPara6 = GPN_GEN_MSG_INVALID_PORT;
		ifmRegMsg.iMsgPara7 = getpid();
		ifmRegMsg.msgCellLen = 0;

		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pstFdSet, &ifmRegMsg, GPN_SOCK_MSG_HEAD_BLEN);
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almRegMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxVariMultPortCreatNotify(stSockFdSet *pstFdSet, UINT32 commRole, objLogicDesc *pPortIndex, UINT32 opt)
{
	/*msgHead(64)*/
	gpnSockMsg ifmRegMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	if( (commRole != GPN_COMMM_ALM) &&\
		(commRole != GPN_COMMM_STAT) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	if( (opt == GPN_SOCK_MSG_OPT_CREAT) ||\
		(opt == GPN_SOCK_MSG_OPT_DELETE) )
	{
		/*almRegMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
		ifmRegMsg.iMsgCtrl = 0;

		ifmRegMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, commRole);
		ifmRegMsg.iSrcId = pstFdSet->localSrc;
		
		if(opt == GPN_SOCK_MSG_OPT_CREAT)
		{
			ifmRegMsg.iMsgType = GPN_IFM_MSG_VARI_PORT_REG;
		}
		else
		{
			ifmRegMsg.iMsgType = GPN_IFM_MSG_VARI_PORT_UNREG;
		}
		ifmRegMsg.iMsgPara1 = pPortIndex->portIndex;
		ifmRegMsg.iMsgPara2 = pPortIndex->portIndex3;
		ifmRegMsg.iMsgPara3 = pPortIndex->portIndex4;
		ifmRegMsg.iMsgPara4 = pPortIndex->portIndex5;
		ifmRegMsg.iMsgPara5 = pPortIndex->portIndex6;
		ifmRegMsg.iMsgPara6 = pPortIndex->devIndex;
		ifmRegMsg.iMsgPara7 = getpid();
		ifmRegMsg.msgCellLen = 0;

		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pstFdSet, &ifmRegMsg, GPN_SOCK_MSG_HEAD_BLEN);
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almRegMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockIfmMsgTxCfmMEPPortCreatNotify(stSockFdSet *pstFdSet, UINT32 commRole, UINT32 ethPortId,
	UINT32 vlanId, UINT32 MDId, UINT32 MAId, UINT32 MEPId, UINT32 RemMEPId, UINT32 opt)
{
	/*msgHead(64)*/
	gpnSockMsg ifmRegMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	if( (commRole != GPN_COMMM_ALM) &&\
		(commRole != GPN_COMMM_STAT) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	if( (opt == GPN_SOCK_MSG_OPT_CREAT) ||\
		(opt == GPN_SOCK_MSG_OPT_DELETE) )
	{
		/*almRegMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
		ifmRegMsg.iMsgCtrl = 0;

		ifmRegMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, commRole);
		ifmRegMsg.iSrcId = pstFdSet->localSrc;
		
		if(opt == GPN_SOCK_MSG_OPT_CREAT)
		{
			ifmRegMsg.iMsgType = GPN_IFM_MSG_VARI_PORT_REG;
		}
		else
		{
			ifmRegMsg.iMsgType = GPN_IFM_MSG_VARI_PORT_UNREG;
		}	

		ifmRegMsg.iMsgPara1 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, MEPId);
		ifmRegMsg.iMsgPara2 = MAId;
		ifmRegMsg.iMsgPara3 = MDId;
		ifmRegMsg.iMsgPara4 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, ethPortId, vlanId);
		ifmRegMsg.iMsgPara5 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, RemMEPId);
		ifmRegMsg.iMsgPara6 = GPN_GEN_MSG_INVALID_PORT;
		ifmRegMsg.iMsgPara7 = getpid();
		
		ifmRegMsg.msgCellLen = 0;

		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pstFdSet, &ifmRegMsg, GPN_SOCK_MSG_HEAD_BLEN);
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almRegMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockIfmMsgTxPTNPortCreatNotify(stSockFdSet *pstFdSet, UINT32 commRole,
	UINT32 vc/*pw*/, UINT32 vp/*lsp*/, UINT32 vs, UINT32 opt)
{
	/*msgHead(64)*/
	gpnSockMsg ifmRegMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	if( (commRole != GPN_COMMM_ALM) &&\
		(commRole != GPN_COMMM_STAT) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	if( (opt == GPN_SOCK_MSG_OPT_CREAT) ||\
		(opt == GPN_SOCK_MSG_OPT_DELETE) )
	{
		/*almRegMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
		ifmRegMsg.iMsgCtrl = 0;

		ifmRegMsg.iDstId = GPN_SOCK_CREAT_DIREC(pstFdSet->CPUId, commRole);
		ifmRegMsg.iSrcId = pstFdSet->localSrc;
		
		if(opt == GPN_SOCK_MSG_OPT_CREAT)
		{
			ifmRegMsg.iMsgType = GPN_IFM_MSG_VARI_PORT_REG;
		}
		else
		{
			ifmRegMsg.iMsgType = GPN_IFM_MSG_VARI_PORT_UNREG;
		}
		if(vc == GPN_GEN_MSG_INVALID_PORT)
		{
			if(vp == GPN_GEN_MSG_INVALID_PORT)
			{
				ifmRegMsg.iMsgPara1 = LAB_PortIndex_Create(IFM_VS_TYPE, 0, vs);
				ifmRegMsg.iMsgPara2 = GPN_GEN_MSG_INVALID_PORT;
				ifmRegMsg.iMsgPara3 = GPN_GEN_MSG_INVALID_PORT;
			}
			else
			{
				ifmRegMsg.iMsgPara1 = LAB_PortIndex_Create(IFM_LSP_TYPE, 0, vp);
				/*ifmRegMsg.iMsgPara2 = LAB_PortIndex_Create(IFM_VS_TYPE, 0, vs);*/
				ifmRegMsg.iMsgPara2 = GPN_GEN_MSG_INVALID_PORT;
				ifmRegMsg.iMsgPara3 = GPN_GEN_MSG_INVALID_PORT;
			}
		}
		else
		{
			ifmRegMsg.iMsgPara1 = LAB_PortIndex_Create(IFM_PW_TYPE, 0, vc);
			/*ifmRegMsg.iMsgPara2 = LAB_PortIndex_Create(IFM_LSP_TYPE, 0, vp);*/
			ifmRegMsg.iMsgPara2 = GPN_GEN_MSG_INVALID_PORT;
			/*ifmRegMsg.iMsgPara4 = LAB_PortIndex_Create(IFM_VS_TYPE, 0, vs);*/
			ifmRegMsg.iMsgPara3 = GPN_GEN_MSG_INVALID_PORT;
		}

		ifmRegMsg.iMsgPara4 = GPN_GEN_MSG_INVALID_PORT;
		ifmRegMsg.iMsgPara5 = GPN_GEN_MSG_INVALID_PORT;
		ifmRegMsg.iMsgPara6 = GPN_GEN_MSG_INVALID_PORT;
		ifmRegMsg.iMsgPara7 = getpid();
		
		ifmRegMsg.msgCellLen = 0;

		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pstFdSet, &ifmRegMsg, GPN_SOCK_MSG_HEAD_BLEN);
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almRegMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxPeerPortReplaceNotify(optObjOrient *pOldPeer, optObjOrient *pNewPeer, UINT32 MODU_ID)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg ifmPeerMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);

	/*assert*/
	if( (pOldPeer == NULL) ||\
		(pNewPeer == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	ifmPeerMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	ifmPeerMsg.iMsgType = GPN_IFM_MSG_PEER_PORT_REPLACE;

	ifmPeerMsg.iDstId = GPN_SOCK_CREAT_DIREC(pgstFdSet->CPUId, MODU_ID);
	ifmPeerMsg.iSrcId = pgstFdSet->localSrc;

	ifmPeerMsg.iMsgPara1 = pOldPeer->portIndex;
	ifmPeerMsg.iMsgPara2 = pOldPeer->portIndex3;
	ifmPeerMsg.iMsgPara3 = pOldPeer->portIndex4;
	ifmPeerMsg.iMsgPara4 = pOldPeer->portIndex5;
	ifmPeerMsg.iMsgPara5 = pOldPeer->portIndex6;
	ifmPeerMsg.iMsgPara6 = pOldPeer->devIndex;

	ifmPeerMsg.iMsgPara7 = pNewPeer->portIndex;
	ifmPeerMsg.iMsgPara8 = pNewPeer->portIndex3;
	ifmPeerMsg.iMsgPara9 = pNewPeer->portIndex4;
	ifmPeerMsg.iMsgParaA = pNewPeer->portIndex5;
	ifmPeerMsg.iMsgParaB = pNewPeer->portIndex6;
	ifmPeerMsg.iMsgParaC = pNewPeer->devIndex;

	ifmPeerMsg.iMsgParaD = getpid();
	ifmPeerMsg.msgCellLen = 0;
	
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pgstFdSet, &ifmPeerMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/

	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockIfmMsgTxViewPortReplaceNotify(objLogicDesc *pOldView, objLogicDesc *pNewView, UINT32 MODU_ID)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg ifmPeerMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);

	/*assert*/
	if( (pOldView == NULL) ||\
		(pNewView == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	ifmPeerMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	ifmPeerMsg.iMsgType = GPN_IFM_MSG_VIEW_PORT_REPLACE;

	ifmPeerMsg.iDstId = GPN_SOCK_CREAT_DIREC(pgstFdSet->CPUId, MODU_ID);
	ifmPeerMsg.iSrcId = pgstFdSet->localSrc;

	ifmPeerMsg.iMsgPara1 = pOldView->portIndex;
	ifmPeerMsg.iMsgPara2 = pOldView->portIndex3;
	ifmPeerMsg.iMsgPara3 = pOldView->portIndex4;
	ifmPeerMsg.iMsgPara4 = pOldView->portIndex5;
	ifmPeerMsg.iMsgPara5 = pOldView->portIndex6;
	ifmPeerMsg.iMsgPara6 = pOldView->devIndex;

	ifmPeerMsg.iMsgPara7 = pNewView->portIndex;
	ifmPeerMsg.iMsgPara8 = pNewView->portIndex3;
	ifmPeerMsg.iMsgPara9 = pNewView->portIndex4;
	ifmPeerMsg.iMsgParaA = pNewView->portIndex5;
	ifmPeerMsg.iMsgParaB = pNewView->portIndex6;
	ifmPeerMsg.iMsgParaC = pNewView->devIndex;

	ifmPeerMsg.iMsgParaD = getpid();
	ifmPeerMsg.msgCellLen = 0;
	
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pgstFdSet, &ifmPeerMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/

	return GPN_SELECT_GEN_OK;
}

/*define for x-modu notify index chang(creat or deletle) to IFM modu */
UINT32 gpnSockIfmMsgTxDevStateNotify2Ifm(UINT32 devIndex, UINT32 sta)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxDevStateNotify(&(gSockCommData.gstFdSet), /*GPN_COMMM_IFM*/GPN_COMMM_ALM, devIndex, sta))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxFixPortCapabilityNotify2Ifm(UINT32 devIndex, stFixPorcReg *pfixPortSet, UINT32 len)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxFixPortCapabilityNotify(&(gSockCommData.gstFdSet), /*GPN_COMMM_IFM*/GPN_COMMM_ALM, devIndex, pfixPortSet, len))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxVariPortCreatNotify2Ifm(UINT32 vPortIndex, UINT32 opt)
{
	UINT32 reVal;
	UINT32 ret;

	ret = 0;
	/*send to gpn_alarm*/
	reVal = gpnSockIfmMsgTxVariPortCreatNotify(&(gSockCommData.gstFdSet), 
		GPN_COMMM_ALM, vPortIndex, opt);
	if(reVal != GPN_SELECT_GEN_OK)
	{
		ret |= 1;
		/*print or log*/
		
	}

	/*send to gpn_stat*/
	reVal = gpnSockIfmMsgTxVariPortCreatNotify(&(gSockCommData.gstFdSet), 
		GPN_COMMM_STAT, vPortIndex, opt);
	if(reVal != GPN_SELECT_GEN_OK)
	{
		ret |= 1;
		/*print or log*/
		
	}
	
	if(ret != 0)
	{
		return GPN_SELECT_GEN_ERR;
	}
	else
	{
		return GPN_SELECT_GEN_OK;
	}
}
UINT32 gpnSockIfmMsgTxVariMultPortCreatNotify2Ifm(objLogicDesc *pPortIndex, UINT32 opt)
{
	UINT32 reVal;
	UINT32 ret;

	ret = 0;
	/*send to gpn_alarm*/
	reVal = gpnSockIfmMsgTxVariMultPortCreatNotify(&(gSockCommData.gstFdSet),\
				GPN_COMMM_ALM, pPortIndex, opt);
	if(reVal != GPN_SELECT_GEN_OK)
	{
		ret |= 1;
		/*print or log*/
		
	}

	/*send to gpn_stat*/
	reVal = gpnSockIfmMsgTxVariMultPortCreatNotify(&(gSockCommData.gstFdSet),\
				GPN_COMMM_STAT, pPortIndex, opt);
	if(reVal != GPN_SELECT_GEN_OK)
	{
		ret |= 1;
		/*print or log*/
		
	}
	
	if(ret != 0)
	{
		return GPN_SELECT_GEN_ERR;
	}
	else
	{
		return GPN_SELECT_GEN_OK;
	}
}

UINT32 gpnSockIfmMsgTxCfmMEPPortCreatNotify2Ifm(stSockFdSet *pstFdSet, UINT32 ethPortId,
	UINT32 vlanId, UINT32 MDId, UINT32 MAId, UINT32 MEPId, UINT32 RemMEPId, UINT32 opt)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxCfmMEPPortCreatNotify(pstFdSet, /*GPN_COMMM_IFM*/GPN_COMMM_ALM, ethPortId,
			vlanId, MDId, MAId, MEPId, RemMEPId, opt))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxPTNPortCreatNotify2Ifm(stSockFdSet *pstFdSet,
	UINT32 vc/*pw*/, UINT32 vp/*lsp*/, UINT32 vs, UINT32 opt)
{
	UINT32 reVal;
	UINT32 ret;

	ret = 0;
	/*send to gpn_alarm*/
	reVal = gpnSockIfmMsgTxPTNPortCreatNotify(pstFdSet,\
				GPN_COMMM_ALM, vc, vp, vs, opt);
	if(reVal != GPN_SELECT_GEN_OK)
	{
		ret |= 1;
		/*print or log*/
		
	}

	/*send to gpn_stat*/
	reVal = gpnSockIfmMsgTxPTNPortCreatNotify(pstFdSet,\
				GPN_COMMM_STAT, vc, vp, vs, opt);
	if(reVal != GPN_SELECT_GEN_OK)
	{
		ret |= 1;
		/*print or log*/
		
	}
	if(ret != 0)
	{
		return GPN_SELECT_GEN_ERR;
	}
	else
	{
		return GPN_SELECT_GEN_OK;
	}
}
UINT32 gpnSockIfmMsgTxPeerPortReplaceNotify2Ifm(optObjOrient *pOldPeer, optObjOrient *pNewPeer)
{
	UINT32 reVal;
	UINT32 ret;

	ret = 0;
	/*send to gpn_alarm*/
	reVal = gpnSockIfmMsgTxPeerPortReplaceNotify2Alm(pOldPeer, pNewPeer);
	if(reVal != GPN_SELECT_GEN_OK)
	{
		ret |= 1;
		/*print or log*/
		
	}

	/*send to gpn_stat*/
	reVal = gpnSockIfmMsgTxPeerPortReplaceNotify2Stat(pOldPeer, pNewPeer);
	if(reVal != GPN_SELECT_GEN_OK)
	{
		ret |= 1;
		/*print or log*/
		
	}
	if(ret != 0)
	{
		return GPN_SELECT_GEN_ERR;
	}
	else
	{
		return GPN_SELECT_GEN_OK;
	}
}
UINT32 gpnSockIfmMsgTxViewPortReplaceNotify2Ifm(objLogicDesc *pOldView, objLogicDesc *pNewView)
{
	UINT32 reVal;
	UINT32 ret;

	ret = 0;
	/*send to gpn_alarm*/
	reVal = gpnSockIfmMsgTxViewPortReplaceNotify(pOldView, pNewView, GPN_COMMM_ALM);
	if(reVal != GPN_SELECT_GEN_OK)
	{
		ret |= 1;
		/*print or log*/
		
	}

	/*send to gpn_stat*/
	reVal = gpnSockIfmMsgTxViewPortReplaceNotify(pOldView, pNewView, GPN_COMMM_STAT);
	if(reVal != GPN_SELECT_GEN_OK)
	{
		ret |= 1;
		/*print or log*/
		
	}
	
	if(ret != 0)
	{
		return GPN_SELECT_GEN_ERR;
	}
	else
	{
		return GPN_SELECT_GEN_OK;
	}
}

UINT32 gpnSockIfmMsgTxZebosL2NewIfNotify2Ifm(objLogicDesc *pl2If, objLogicDesc *ppeerIf)
{
	/*assert*/
	if( (pl2If == NULL) ||\
		(ppeerIf == NULL) )
	{
		return GPN_SELECT_GEN_ERR;
	}
	
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxVariMultPortCreatNotify2Ifm((objLogicDesc *)pl2If, GPN_SOCK_MSG_OPT_CREAT))
	{
		if(memcmp((const char *)pl2If, (const char *)ppeerIf, sizeof(objLogicDesc)) != 0)
		{
			if(GPN_SELECT_GEN_OK ==\
				gpnSockIfmMsgTxPeerPortReplaceNotify2Ifm((optObjOrient *)pl2If, (optObjOrient *)ppeerIf))
			{
				return GPN_SELECT_GEN_OK;
			}
			else
			{
				printf("gpnSockIfmMsgTxPeerPortReplaceNotify2Ifm err\n\r");
			}
		}
		else
		{
			printf("gpnSockIfmMsgTxZebosL2NewIfNotify2Ifm strncmp equ\n\r");
		}
	}
	else
	{
		printf("gpnSockIfmMsgTxVariMultPortCreatNotify2Ifm err\n\r");
	}

	return GPN_SELECT_GEN_ERR;
}


/*define for notify status to ALM*/
UINT32 gpnSockIfmMsgTxDevStateNotify2Alm(UINT32 devIndex, UINT32 sta)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxDevStateNotify(&(gSockCommData.gstFdSet), GPN_COMMM_ALM, devIndex, sta))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxFixPortCapabilityNotify2Alm(UINT32 devIndex, stFixPorcReg *pfixPortSet, UINT32 len)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxFixPortCapabilityNotify(&(gSockCommData.gstFdSet), GPN_COMMM_ALM, devIndex, pfixPortSet, len))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxVariPortCreatNotify2Alm(stSockFdSet *pstFdSet, UINT32 vPortIndex, UINT32 opt)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxVariPortCreatNotify(pstFdSet, GPN_COMMM_ALM, vPortIndex, opt))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxVariMultPortCreatNotify2Alm(objLogicDesc *pPortIndex, UINT32 opt)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxVariMultPortCreatNotify(&(gSockCommData.gstFdSet), GPN_COMMM_ALM, pPortIndex, opt))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockIfmMsgTxCfmMEPPortCreatNotify2Alm(stSockFdSet *pstFdSet, UINT32 ethPortId,
	UINT32 vlanId, UINT32 MDId, UINT32 MAId, UINT32 MEPId, UINT32 RemMEPId, UINT32 opt)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxCfmMEPPortCreatNotify(pstFdSet, GPN_COMMM_ALM, ethPortId,
			vlanId, MDId, MAId, MEPId, RemMEPId, opt))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxPTNPortCreatNotify2Alm(stSockFdSet *pstFdSet,
	UINT32 vc/*pw*/, UINT32 vp/*lsp*/, UINT32 vs, UINT32 opt)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxPTNPortCreatNotify(pstFdSet, GPN_COMMM_ALM, vc, vp, vs, opt))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxPeerPortReplaceNotify2Alm(optObjOrient *pOldPeer, optObjOrient *pNewPeer)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxPeerPortReplaceNotify(pOldPeer, pNewPeer, GPN_COMMM_ALM))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockIfmMsgTxViewPortReplaceNotify2Alm(objLogicDesc *pOldView, objLogicDesc *pNewView)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxViewPortReplaceNotify(pOldView, pNewView, GPN_COMMM_ALM))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}


/*define for notify status to STAT*/
UINT32 gpnSockIfmMsgTxDevStateNotify2Stat(UINT32 devIndex, UINT32 sta)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxDevStateNotify(&(gSockCommData.gstFdSet), GPN_COMMM_STAT, devIndex, sta))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxFixPortCapabilityNotify2Stat(UINT32 devIndex, stFixPorcReg *pfixPortSet, UINT32 len)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxFixPortCapabilityNotify(&(gSockCommData.gstFdSet), GPN_COMMM_STAT, devIndex, pfixPortSet, len))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxVariPortCreatNotify2Stat(stSockFdSet *pstFdSet, UINT32 vPortIndex, UINT32 opt)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxVariPortCreatNotify(pstFdSet, GPN_COMMM_STAT, vPortIndex, opt))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxVariMultPortCreatNotify2Stat(objLogicDesc *pPortIndex, UINT32 opt)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxVariMultPortCreatNotify(&(gSockCommData.gstFdSet), GPN_COMMM_STAT, pPortIndex, opt))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockIfmMsgTxCfmMEPPortCreatNotify2Stat(stSockFdSet *pstFdSet, UINT32 ethPortId,
	UINT32 vlanId, UINT32 MDId, UINT32 MAId, UINT32 MEPId, UINT32 RemMEPId, UINT32 opt)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxCfmMEPPortCreatNotify(pstFdSet, GPN_COMMM_STAT, ethPortId,
			vlanId, MDId, MAId, MEPId, RemMEPId, opt))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxPTNPortCreatNotify2Stat(UINT32 vc/*pw*/, UINT32 vp/*lsp*/, UINT32 vs, UINT32 opt)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxPTNPortCreatNotify(&(gSockCommData.gstFdSet), GPN_COMMM_STAT, vc, vp, vs, opt))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxPeerPortReplaceNotify2Stat(optObjOrient *pOldPeer, optObjOrient *pNewPeer)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxPeerPortReplaceNotify(pOldPeer, pNewPeer, GPN_COMMM_STAT))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockIfmMsgTxViewPortReplaceNotify2Stat(objLogicDesc *pOldView, objLogicDesc *pNewView)
{
	if(GPN_SELECT_GEN_OK ==\
		gpnSockIfmMsgTxViewPortReplaceNotify(pOldView, pNewView, GPN_COMMM_STAT))
	{
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockIfmMsgTxRoleDebugCtrl2OtherModule(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 MODU_ID, UINT32 debugCtrl)
{
	/*msgHead(64)*/
	gpnSockMsg ifmRegMsg;

	/*assert*/
	if(pstFdSet == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	ifmRegMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
	ifmRegMsg.iMsgType = GPN_IFM_MSG_ROLE_DEBUG_CTRL;

	ifmRegMsg.iDstId = GPN_SOCK_CREAT_DIREC(CPUId, MODU_ID);
	ifmRegMsg.iSrcId = pstFdSet->localSrc;

	ifmRegMsg.iMsgPara1 = debugCtrl;

	ifmRegMsg.msgCellLen = 0;
	
	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pstFdSet, &ifmRegMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SOCK_IFM_MSG_DEF_C_*/

