/**********************************************************
* file name: gpnSockAlmMsgDef.c
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-26
* function: 
*    define select opt about
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_ALM_MSG_DEF_C_
#define _GPN_SOCK_ALM_MSG_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockAlmMsgDef.h"

#include "socketComm/gpnSockCommModuDef.h"
#include "socketComm/gpnSockCommRoleMan.h"
#include "socketComm/gpnGlobalPortIndexDef.h"
#include "socketComm/gpnSelectOpt.h"

extern sockCommData gSockCommData;

UINT32 gpnSockAlmMsgTxMonStaCfg(UINT32 portIndex, UINT32 sta)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg almMonMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);

	if( (sta == GPN_SOCK_MSG_OPT_ENABLE) ||\
		(sta == GPN_SOCK_MSG_OPT_DISABLE) )
	{
		almMonMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;
		
		almMonMsg.iDstId = GPN_SOCK_CREAT_DIREC(pgstFdSet->CPUId, GPN_SOCK_ROLE_ALM);
		almMonMsg.iSrcId = pgstFdSet->localSrc;
		
		almMonMsg.iMsgType = GPN_ALM_MSG_PORT_ALM_MON_CFG;
		almMonMsg.iMsgPara1 = 1;
		almMonMsg.iMsgPara2 = portIndex;
		almMonMsg.iMsgPara3 = GPN_GEN_MSG_INVALID_PORT;
		almMonMsg.iMsgPara4 = GPN_GEN_MSG_INVALID_PORT;
		almMonMsg.iMsgPara5 = GPN_GEN_MSG_INVALID_PORT;
		almMonMsg.iMsgPara6 = GPN_GEN_MSG_INVALID_PORT;
		almMonMsg.iMsgPara7 = GPN_GEN_MSG_INVALID_PORT;
		almMonMsg.iMsgPara8 = sta;
		almMonMsg.msgCellLen = 0;

		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pgstFdSet, &almMonMsg, GPN_SOCK_MSG_HEAD_BLEN);
		/*gpnSockGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockAlmMsgTxAlmNotifyBaseSubType(UINT32 portIndex, UINT32 subAlmType, UINT32 sta)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg almStaMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);

	if( (sta == GPN_SOCK_MSG_OPT_RISE) ||\
		(sta == GPN_SOCK_MSG_OPT_CLEAN) )
	{
		/*almStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
		almStaMsg.iMsgCtrl = 0;
		
		almStaMsg.iDstId = GPN_SOCK_CREAT_DIREC(pgstFdSet->CPUId, GPN_COMMM_ALM);
		almStaMsg.iSrcId = pgstFdSet->localSrc;

		almStaMsg.iMsgType = GPN_ALM_MSG_ALM_REPT_TYPE_BASE;
		almStaMsg.iMsgPara1 = portIndex;
		almStaMsg.iMsgPara2 = GPN_GEN_MSG_INVALID_PORT;
		almStaMsg.iMsgPara3 = GPN_GEN_MSG_INVALID_PORT;
		almStaMsg.iMsgPara4 = GPN_GEN_MSG_INVALID_PORT;
		almStaMsg.iMsgPara5 = GPN_GEN_MSG_INVALID_PORT;
		almStaMsg.iMsgPara6 = GPN_GEN_MSG_INVALID_PORT;
		almStaMsg.iMsgPara7 = subAlmType;
		almStaMsg.iMsgPara8 = sta;
		almStaMsg.iMsgPara9 = pgstFdSet->pid;
		almStaMsg.msgCellLen = 0;

		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
		/*gpnSockGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType(optObjOrient *pPortInfo, UINT32 subAlmType, UINT32 sta)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg almStaMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);

	if( (sta == GPN_SOCK_MSG_OPT_RISE) ||\
		(sta == GPN_SOCK_MSG_OPT_CLEAN) )
	{
		/*almStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
		almStaMsg.iMsgCtrl = 0;

		almStaMsg.iDstId = GPN_SOCK_CREAT_DIREC(pgstFdSet->CPUId, GPN_COMMM_ALM);
		almStaMsg.iSrcId = pgstFdSet->localSrc;
		
		almStaMsg.iMsgType = GPN_ALM_MSG_ALM_REPT_TYPE_BASE;
		almStaMsg.iMsgPara1 = pPortInfo->portIndex;
		almStaMsg.iMsgPara2 = pPortInfo->portIndex3;
		almStaMsg.iMsgPara3 = pPortInfo->portIndex4;
		almStaMsg.iMsgPara4 = pPortInfo->portIndex5;
		almStaMsg.iMsgPara5 = pPortInfo->portIndex6;
		//almStaMsg.iMsgPara6 = GPN_GEN_MSG_INVALID_PORT;/*dev index*/
		almStaMsg.iMsgPara6 = pPortInfo->devIndex;  //modify for ipran, by lipf 2018/5/2
		almStaMsg.iMsgPara7 = subAlmType;
		almStaMsg.iMsgPara8 = sta;
		almStaMsg.iMsgPara9 = pgstFdSet->pid;
		almStaMsg.msgCellLen = 0;

		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
		/*gpnSockGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}
UINT32 gpnSockAlmMsgTxCFMAlmNotifyBaseSubType(optObjOrient *pPortInfo, UINT32 subAlmType, UINT32 sta)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg almStaMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);


	if( (sta == GPN_SOCK_MSG_OPT_RISE) ||\
		(sta == GPN_SOCK_MSG_OPT_CLEAN) )
	{
		/*almStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
		almStaMsg.iMsgCtrl = 0;

		almStaMsg.iDstId = GPN_SOCK_CREAT_DIREC(pgstFdSet->CPUId, GPN_COMMM_ALM);
		almStaMsg.iSrcId = pgstFdSet->localSrc;
		
		almStaMsg.iMsgType = GPN_ALM_MSG_ALM_REPT_CFM_BASE;
		almStaMsg.iMsgPara1 = pPortInfo->portIndex;
		almStaMsg.iMsgPara2 = pPortInfo->portIndex3;
		almStaMsg.iMsgPara3 = pPortInfo->portIndex4;
		almStaMsg.iMsgPara4 = pPortInfo->portIndex5;
		almStaMsg.iMsgPara5 = pPortInfo->portIndex6;
		//almStaMsg.iMsgPara6 = GPN_GEN_MSG_INVALID_PORT;
		almStaMsg.iMsgPara6 = pPortInfo->devIndex;  //modify for ipran, by lipf 2018/5/2
		almStaMsg.iMsgPara7 = subAlmType;
		almStaMsg.iMsgPara8 = sta;
		almStaMsg.iMsgPara9 = pgstFdSet->pid;
		almStaMsg.msgCellLen = 0;

		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
		/*gpnSockGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
		
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

UINT32 gpnSockAlmMsgTxEventNotifyBaseSubType(optObjOrient *pPortInfo, UINT32 subEvtType, UINT32 detial)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg evtNtyMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);

	/*almStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;*/
	evtNtyMsg.iMsgCtrl = 0;

	evtNtyMsg.iDstId = GPN_SOCK_CREAT_DIREC(pgstFdSet->CPUId, GPN_COMMM_ALM);
	evtNtyMsg.iSrcId = pgstFdSet->localSrc;
		
	evtNtyMsg.iMsgType = GPN_ALM_MSG_EVT_REPT_TYPE_BASE;
	evtNtyMsg.iMsgPara1 = pPortInfo->portIndex;
	evtNtyMsg.iMsgPara2 = pPortInfo->portIndex3;
	evtNtyMsg.iMsgPara3 = pPortInfo->portIndex4;
	evtNtyMsg.iMsgPara4 = pPortInfo->portIndex5;
	evtNtyMsg.iMsgPara5 = pPortInfo->portIndex6;
	//evtNtyMsg.iMsgPara6 = GPN_GEN_MSG_INVALID_PORT;
	evtNtyMsg.iMsgPara6 = pPortInfo->devIndex;  //modify for ipran, by lipf 2018/5/2
	evtNtyMsg.iMsgPara7 = subEvtType;
	evtNtyMsg.iMsgPara8 = detial;
	evtNtyMsg.iMsgPara9 = pgstFdSet->pid;
	evtNtyMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pgstFdSet, &evtNtyMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockAlmMsgTxAlmNotifyBaseScanType(UINT32 portIndex, UINT32 *almVal, UINT32 almPageNum)
{
	UINT32 i;
	UINT32 *ppage;
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg almStaMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);

	almStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;

	almStaMsg.iDstId = GPN_SOCK_CREAT_DIREC(pgstFdSet->CPUId, GPN_COMMM_ALM);
	almStaMsg.iSrcId = pgstFdSet->localSrc;
	
	almStaMsg.iMsgType = GPN_ALM_MSG_ALM_REPT_BITS_BASE;
	almStaMsg.iMsgPara1 = portIndex;
	almStaMsg.iMsgPara2 = GPN_GEN_MSG_INVALID_PORT;
	almStaMsg.iMsgPara3 = GPN_GEN_MSG_INVALID_PORT;
	almStaMsg.iMsgPara4 = GPN_GEN_MSG_INVALID_PORT;
	almStaMsg.iMsgPara5 = GPN_GEN_MSG_INVALID_PORT;
	almStaMsg.iMsgPara6 = GPN_GEN_MSG_INVALID_PORT;
	almStaMsg.iMsgPara7 = GPN_ALM_PAGE_NUM;

	/* 8 9 A B C D E F : TOTAL 8 PARAM */
	if( (GPN_ALM_PAGE_NUM > 8) ||\
		(almPageNum !=  GPN_ALM_PAGE_NUM) )
	{
		return GPN_SELECT_GEN_ERR;
	}

	ppage = &(almStaMsg.iMsgPara8);
	for(i = 0; i < almPageNum; i++)
	{
		*ppage = almVal[i];
		ppage++;
	}
	
	almStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockAlmMsgTxBellSwitchCfg(UINT32 opt)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg almStaMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);
	
	almStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;

	almStaMsg.iDstId = GPN_SOCK_CREAT_DIREC(pgstFdSet->CPUId, GPN_COMMM_SDM);
	almStaMsg.iSrcId = pgstFdSet->localSrc;
	
	almStaMsg.iMsgType = GPN_ALM_MSG_SWITHC_BELL;
	almStaMsg.iMsgPara1 = opt;/*should be 1/2(open/close)*/
	almStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}
UINT32 gpnSockAlmMsgTxAlmRankNumNotify(UINT32 emergNum, UINT32 severeNum, UINT32 commNum, UINT32 curNum)
{
	stSockFdSet *pgstFdSet;
	/*msgHead(64)*/
	gpnSockMsg almStaMsg;

	pgstFdSet = &(gSockCommData.gstFdSet);

	almStaMsg.iMsgCtrl = GPN_SOCK_MSG_ACK_NEED_BIT;

	almStaMsg.iDstId = GPN_SOCK_CREAT_DIREC(pgstFdSet->CPUId, GPN_COMMM_SDM);
	almStaMsg.iSrcId = pgstFdSet->localSrc;
	
	almStaMsg.iMsgType = GPN_ALM_MSG_ALM_RANK_NOT;
	almStaMsg.iMsgPara1 = emergNum;
	almStaMsg.iMsgPara2 = severeNum;
	almStaMsg.iMsgPara3 = commNum;
	almStaMsg.iMsgPara4 = curNum;
	almStaMsg.msgCellLen = 0;

	/*guarantee or not:not guarantee*/
	gpnSockNoGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);
	/*gpnSockGuaranteeMsgTxProc(pgstFdSet, &almStaMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
	return GPN_SELECT_GEN_OK;
}

UINT32 gpnSockAlmMsgTxAlmProcResultViewCreat(char *path, UINT32 type)
{
	char msgBull[GPN_ALM_MSG_PAPA_RESULT_VIEW_BUFF];
	gpnSockMsg *palmStaMsg;
	UINT32 len;

	/*assert */
	if(path == NULL)
	{
		return GPN_SELECT_GEN_ERR;
	}

	palmStaMsg = (gpnSockMsg *)msgBull;
	palmStaMsg->iMsgCtrl = 0/*GPN_SOCK_MSG_ACK_NEED_BIT*/;

	palmStaMsg->iDstId = GPN_SOCK_CREAT_DIREC(gSockCommData.gstFdSet.CPUId, GPN_COMMM_ALM);
	palmStaMsg->iSrcId = gSockCommData.gstFdSet.localSrc;
	
	palmStaMsg->iMsgType = GPN_ALM_MSG_PORC_RESULT_FILE_VIEW;
	palmStaMsg->iMsgPara1 = type;

	/* strlen end when 0xxx*/
	len = strlen(path) + 1;
	palmStaMsg->msgCellLen = len;

	if(len <= GPN_ALM_MSG_PAPA_RESULT_VIEW_PATH)
	{
		memcpy(msgBull+GPN_SOCK_MSG_HEAD_BLEN, path, len);

		/*guarantee or not:not guarantee*/
		gpnSockNoGuaranteeMsgTxProc(&(gSockCommData.gstFdSet), palmStaMsg, GPN_SOCK_MSG_HEAD_BLEN+len);
		/*gpnSockGuaranteeMsgTxProc(pstFdSet, &statMsg, GPN_SOCK_MSG_HEAD_BLEN);*/
	
		return GPN_SELECT_GEN_OK;
	}
	else
	{
		return GPN_SELECT_GEN_ERR;
	}
}

/* temp define */
#include "socketComm/gpnAlmTypeDef.h"
#include "socketComm/gpnGlobalPortIndexDef.h"
#include "devCoreInfo/gpnDevCoreInfoFunApi.h"

UINT32 gpnSockAlmMsgTxEmbStorOutEvent(UINT32 detial)
{
	optObjOrient portInfo;

	portInfo.devIndex = GPN_GEN_MSG_INVALID_PORT;
	portInfo.portIndex = SMP_PortIndex_Create(IFM_SOFT_TYPE,\
							0/*gpnDevApiGetSelfSoltNum()*/, 1);
	
	portInfo.portIndex3 = GPN_GEN_MSG_INVALID_PORT;
	portInfo.portIndex4 = GPN_GEN_MSG_INVALID_PORT;
	portInfo.portIndex5 = GPN_GEN_MSG_INVALID_PORT;
	portInfo.portIndex6 = GPN_GEN_MSG_INVALID_PORT;
	gpnSockAlmMsgTxEventNotifyBaseSubType(&portInfo, GPN_EVT_TYPE_EMB_STOR_OUT, 0);

	return GPN_SELECT_GEN_OK;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_ALM_MSG_DEF_C_*/

