/**********************************************************
* file name: gpnStatDebug.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-10-11
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_DEBUG_C_
#define _GPN_STAT_DEBUG_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <lib/syslog.h>
#include <lib/log.h>

#include "socketComm/gpnSockInclude.h"

/*log function include*/
#include "gpnLog/gpnLogFuncApi.h"

/*SUP SMP debug mode include*/
#include "socketComm/gpnDebugFuncApi.h"
#include "socketComm/gpnSockIfmMsgDef.h"
#include "socketComm/gpnSockStatMsgDef.h"
#include "socketComm/gpnSpecialWarrper.h"
#include "devCoreInfo/gpnDevCoreInfoFunApi.h"

/*stat process include*/
#include "gpnStatProcMain.h"
#include "gpnStatFuncApi.h"
#include "gpnStatSnmpApi.h"
#include "gpnStatDataStruct.h"
#include "gpnStatCollectProc.h"
#include "gpnStatDebug.h"


void gpnStatForNsmFollowTest(UINT32 para1, UINT32 para2)
{
	optObjOrient portIndex;

#if 0	/* ptn 规则 */
	portIndex.devIndex = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex = LAB_PortIndex_Create(IFM_LSP_TYPE, 0, 100);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
#endif

	/* ipran规则 */
	portIndex.devIndex = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex = LAB_PortIndex_Create(IFM_LSP_TYPE, 0, 0);
	portIndex.portIndex3 = para1;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
	
	gpnSockStatMsgApiTxFollowPStatGet(&portIndex, GPN_STAT_SCAN_TYPE_PTN_LSP);

	return;
}

void gpnStatForEvnThredTpTest(UINT32 para1, UINT32 para2)
{
	UINT32 reVal;
	UINT32 reVal1;
	gpnSockMsg gpnStatMsgSpIn;
	gpnSockMsg *pgpnStatMsgSpOut;
	objLogicDesc *plocalPIndex;
	stStatMsgMonObjCfg *pstatMonObjCfg;

	memset(&gpnStatMsgSpIn, 0, sizeof(gpnSockMsg));
	
	pgpnStatMsgSpOut = (gpnSockMsg *)malloc(sizeof(gpnSockMsg)+sizeof(objLogicDesc)+sizeof(stStatMsgMonObjCfg));
	pgpnStatMsgSpOut->msgCellLen = sizeof(objLogicDesc)+sizeof(stStatMsgMonObjCfg);

	gpnStatMsgSpIn.iMsgPara1 = GPN_ILLEGAL_DEVICE_INDEX;
	gpnStatMsgSpIn.iMsgPara2 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara3 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara4 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara5 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara6 = GPN_ILLEGAL_PORT_INDEX;
	
	reVal1 = GPN_STAT_DEBUG_OK;

	while(reVal1 == GPN_STAT_DEBUG_OK)
	{
		gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_MON_GET_NEXT;

		reVal1 = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
		printf("reVal %d\n\r", reVal1);
		
		pstatMonObjCfg = (stStatMsgMonObjCfg *)(&(pgpnStatMsgSpOut->msgCellLen)+1);
		plocalPIndex = (objLogicDesc *)(pstatMonObjCfg+1);
		printf("use this node  dev(%08x) port(%08x|%08x|%08x|%08x|%08x)\n\r",\
			gpnStatMsgSpIn.iMsgPara1, gpnStatMsgSpIn.iMsgPara2, gpnStatMsgSpIn.iMsgPara3,\
			gpnStatMsgSpIn.iMsgPara4, gpnStatMsgSpIn.iMsgPara5, gpnStatMsgSpIn.iMsgPara6);
		printf("find this Node dev(%08x) port(%08x|%08x|%08x|%08x|%08x)\n\r",\
			pstatMonObjCfg->portIndex.devIndex, pstatMonObjCfg->portIndex.portIndex,\
			pstatMonObjCfg->portIndex.portIndex3, pstatMonObjCfg->portIndex.portIndex4,\
			pstatMonObjCfg->portIndex.portIndex5, pstatMonObjCfg->portIndex.portIndex6);
		
		printf("find next Node dev(%08x) port(%08x|%08x|%08x|%08x|%08x)\n\r",\
			plocalPIndex->devIndex, plocalPIndex->portIndex, plocalPIndex->portIndex3,\
			plocalPIndex->portIndex4, plocalPIndex->portIndex5, plocalPIndex->portIndex6);
			
		printf("gEn     %d currEn %d\n\r",\
			pstatMonObjCfg->statMoniEn, pstatMonObjCfg->currStatMoniEn);
		printf("longEn  %d task   %d evnThId %d subFiltThId %d almThId %d hist %d\n\r",\
			pstatMonObjCfg->longCycStatMoniEn, pstatMonObjCfg->longCycBelongTask, pstatMonObjCfg->longCycEvnThredTpId,\
			pstatMonObjCfg->longCycSubFiltTpId, pstatMonObjCfg->longCycAlmThredTpId, pstatMonObjCfg->longCycHistReptTpId);
		printf("shortEn %d task   %d evnThId %d subFiltThId %d almThId %d hist %d\n\r",\
			pstatMonObjCfg->shortCycStatMoniEn, pstatMonObjCfg->shortCycBelongTask, pstatMonObjCfg->shortCycEvnThredTpId,\
			pstatMonObjCfg->shortCycSubFiltTpId, pstatMonObjCfg->shortCycAlmThredTpId, pstatMonObjCfg->shortCycHistReptTpId);
		printf("udEn    %d task   %d evnThId %d subFiltThId %d almThId %d hist %d udSecs %d\n\r",\
			pstatMonObjCfg->udCycStatMoniEn, pstatMonObjCfg->udCycBelongTask, pstatMonObjCfg->udCycEvnThredTpId,\
			pstatMonObjCfg->udCycSubFiltTpId, pstatMonObjCfg->udCycAlmThredTpId, pstatMonObjCfg->udCycHistDBId,\
			pstatMonObjCfg->udCycSecs);

		gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_MON_ADD;	
		gpnStatMsgSpIn.iMsgPara1 = pgpnStatMsgSpOut->iMsgPara1;
		gpnStatMsgSpIn.iMsgPara2 = pgpnStatMsgSpOut->iMsgPara2;
		gpnStatMsgSpIn.iMsgPara3 = pgpnStatMsgSpOut->iMsgPara3;
		gpnStatMsgSpIn.iMsgPara4 = pgpnStatMsgSpOut->iMsgPara4;
		gpnStatMsgSpIn.iMsgPara5 = pgpnStatMsgSpOut->iMsgPara5;
		gpnStatMsgSpIn.iMsgPara6 = pgpnStatMsgSpOut->iMsgPara6;
		reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
		printf("reVal %d\n\r", reVal);

		gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_BASE_MON_OPT;
		gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_DEBUG_ENABLE;
		reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
		printf("msg(%08x) reVal %d\n\r", GPN_STAT_MSG_BASE_MON_OPT, reVal);

		
		gpnStatMsgSpIn.iMsgPara1 = plocalPIndex->devIndex;
		gpnStatMsgSpIn.iMsgPara2 = plocalPIndex->portIndex;
		gpnStatMsgSpIn.iMsgPara3 = plocalPIndex->portIndex3;
		gpnStatMsgSpIn.iMsgPara4 = plocalPIndex->portIndex4;
		gpnStatMsgSpIn.iMsgPara5 = plocalPIndex->portIndex5;
		gpnStatMsgSpIn.iMsgPara6 = plocalPIndex->portIndex6;

		if(plocalPIndex->portIndex == GPN_ILLEGAL_PORT_INDEX)
		{
			break;
		}
	}
	#if 0
	pgpnStatMsgSpOut->msgCellLen = sizeof(stStatMsgEvnThTp);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = 0;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = 1;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = 2;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);

	pgpnStatMsgSpOut->msgCellLen = sizeof(stStatMsgSubFiltTp);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = 0;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = 1;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = 2;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);
	#endif
	free(pgpnStatMsgSpOut);
}

void gpnStatForNsmMsgTest(UINT32 para1, UINT32 para2)
{
	optObjOrient portIndex;

#if 0  /* ptn规则 */
	portIndex.devIndex = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex = LAB_PortIndex_Create(IFM_LSP_TYPE, 0, 100);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
#endif

	/* ipran规则 */
	portIndex.devIndex = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex = LAB_PortIndex_Create(IFM_LSP_TYPE, 0, 0);
	portIndex.portIndex3 = para1;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
	
	gpnSockStatMsgApiNsmFollowPStatMonStaCfg(&portIndex, 1);
	//gpnSockStatMsgApiNsmFollowPStatMonStaCfg(&portIndex, 2);
	gpnSockStatMsgApiTxFollowPStatGet(&portIndex, GPN_STAT_SCAN_TYPE_PTN_LSP);
	//gpnSockStatMsgApiNsmFollowCounterReset(&portIndex, GPN_STAT_SCAN_TYPE_PTN_LSP, 1);
}

void gpnStatForSdmMsgTest(UINT32 para1, UINT32 para2)
{
	optObjOrient portIndex;

	portIndex.devIndex = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex = LAB_PortIndex_Create(IFM_EQU_TYPE, 0, 100);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	gpnSockStatMsgApiSdmEquStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiSdmEquStatMonStaCfg(&portIndex, GPN_SELECT_GEN_DISABLE);
	gpnSockStatMsgApiTxSdmEquStatGet(&portIndex, GPN_STAT_SCAN_TYPE_EQU);
	
	/* maybe soft ???? */
	#if 0
	gpnSockStatMsgApiSdmSoftStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiSdmSoftStatMonStaCfg(&portIndex, GPN_SELECT_GEN_DISABLE);
	gpnSockStatMsgApiTxSdmSoftStatGet(&portIndex, GPN_STAT_SCAN_TYPE_SOFT);
	#endif
}

void gpnStatForOamMsgTest(UINT32 para1, UINT32 para2)
{
	optObjOrient portIndex;

	portIndex.devIndex = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex = LAB_PortIndex_Create(IFM_EQU_TYPE, 0, 100);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	gpnSockStatMsgApiPtnVsMStatMonStaCfg(&portIndex, 1);
	gpnSockStatMsgApiPtnVsMStatMonStaCfg(&portIndex, 2);
	gpnSockStatMsgApiTxPtnVsMStatGet(&portIndex, GPN_STAT_T_PTN_VS_M_TYPE);

	gpnSockStatMsgApiPtnLspMStatMonStaCfg(&portIndex, 1);
	gpnSockStatMsgApiPtnLspMStatMonStaCfg(&portIndex, 2);
	gpnSockStatMsgApiTxPtnLspMStatGet(&portIndex, GPN_STAT_T_PTN_LSP_M_TYPE);

	gpnSockStatMsgApiPtnPwMStatMonStaCfg(&portIndex, 1);
	gpnSockStatMsgApiPtnPwMStatMonStaCfg(&portIndex, 2);
	gpnSockStatMsgApiTxPtnPwMStatGet(&portIndex, GPN_STAT_T_PTN_PW_M_TYPE);
	
}

void gpnStatForOnmdMsgTest(UINT32 para1, UINT32 para2)
{
	optObjOrient portIndex;

	portIndex.devIndex = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex = LAB_PortIndex_Create(IFM_EQU_TYPE, 0, 100);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	gpnSockStatMsgApiEthMonStatMonStaCfg(&portIndex, 1);
	gpnSockStatMsgApiEthMonStatMonStaCfg(&portIndex, 2);
	gpnSockStatMsgApiTxEthMonStatGet(&portIndex, GPN_STAT_T_ETH_MAC_M_TYPE);
	
}


void gpnStatForFixTableTest(UINT32 para1, UINT32 para2)
{
	UINT32 reVal;
	gpnSockMsg gpnStatMsgSpIn;
	gpnSockMsg *pgpnStatMsgSpOut;
	stSubTypeBoundAlm *psubBoundAlm;
	stSubTypeThred *psubTypethred;

	
	/*scanType get */
	gpnStatMsgSpIn.iIndex = 1;
	gpnStatMsgSpIn.iMsgCtrl = 0;
	gpnStatMsgSpIn.iDstId = GPN_COMMM_STAT;
	gpnStatMsgSpIn.iSrcId= GPN_COMMM_STAT;
	
	pgpnStatMsgSpOut = (gpnSockMsg *)malloc(sizeof(gpnSockMsg) +
		sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred));
	#if 1
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_STAT_TYPE_GET;
	gpnStatMsgSpIn.iMsgPara1 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.msgCellLen = 0;
	
	reVal = gpnStatSnmpApiStatScanTypeGet(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("%s : scanType(%08x) longSecs(%d) shortSecs(%d) subNum(%d)\n\r",\
			__FUNCTION__, pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2,\
			pgpnStatMsgSpOut->iMsgPara3, pgpnStatMsgSpOut->iMsgPara4);
	}
	else
	{
		printf("%s : err!\n\r", __FUNCTION__);
	}

	reVal = GPN_STAT_DEBUG_OK;
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_STAT_TYPE_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = GPN_STAT_SCAN_TYPE_ALL;
	while(reVal == GPN_STAT_DEBUG_OK)
	{
		reVal = gpnStatSnmpApiStatScanTypeGetNext(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
		if(reVal == GPN_STAT_DEBUG_OK)
		{
			printf("%s : scanType(%08x) longSecs(%d) shortSecs(%d) subNum(%d) nextScanType(%08x)\n\r",\
				__FUNCTION__, pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2,\
				pgpnStatMsgSpOut->iMsgPara3, pgpnStatMsgSpOut->iMsgPara4,
				pgpnStatMsgSpOut->iMsgPara5);
			if(pgpnStatMsgSpOut->iMsgPara5 != GPN_STAT_SCAN_TYPE_ALL)
			{
				gpnStatMsgSpIn.iMsgPara1 = pgpnStatMsgSpOut->iMsgPara5;
			}
			else
			{
				break;
			}
		}
		else
		{
			printf("%s : err!\n\r", __FUNCTION__);
		}
	}
	#endif
	#if 1
	/*subType get */
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUB_STAT_TYPE_GET;
	gpnStatMsgSpIn.iMsgPara1 = GPN_STAT_D_PTN_LSP_TXB_TYPE;
	gpnStatMsgSpIn.msgCellLen = 0;
	reVal = gpnStatSnmpApiSubStatTypeGet(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("%s : subType(%08x) b-scan(%08x) valClass(%2d) bitDeep(%2d)\n\r",\
			__FUNCTION__, pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2,\
			pgpnStatMsgSpOut->iMsgPara3, pgpnStatMsgSpOut->iMsgPara4);
		
		psubBoundAlm = (stSubTypeBoundAlm *)(&(pgpnStatMsgSpOut->msgCellLen) + 1);
		psubTypethred = (stSubTypeThred *)(psubBoundAlm + 1);
		
		printf("longUpAlm(%08x) longUpEvn(%08x) longDnAlm(%08x) longDnEvn(%08x)\n\r",\
			psubBoundAlm->longCycUpAlm, psubBoundAlm->longCycUpEvt,\
			psubBoundAlm->longCycLowAlm, psubBoundAlm->longCycLowEvt);

		printf("shortUpAlm(%08x) shortUpEvn(%08x) shortDnAlm(%08x) shortDnEvn(%08x)\n\r",\
			psubBoundAlm->shortCycUpAlm, psubBoundAlm->shortCycUpEvt,\
			psubBoundAlm->shortCycLowAlm, psubBoundAlm->shortCycLowEvt);

		printf("userDefUpAlm(%08x) userDefUpEvn(%08x) userDefDnAlm(%08x) userDefDnEvn(%08x)\n\r",\
			psubBoundAlm->userDefCycUpAlm, psubBoundAlm->userDefCycUpEvt,\
			psubBoundAlm->userDefCycLowAlm, psubBoundAlm->userDefCycLowEvt);

		printf("longUpH(%08x) longUpL(%08x) longDnH(%08x) longDnL(%08x)\n\r",\
			psubTypethred->longCycUpThredHigh32, psubTypethred->longCycUpThredLow32,\
			psubTypethred->longCycDnThredHigh32, psubTypethred->longCycDnThredLow32);

		printf("shortUpH(%08x) shortUpL(%08x) shortDnH(%08x) shortDnL(%08x)\n\r",\
			psubTypethred->shortCycUpThredHigh32, psubTypethred->shortCycUpThredLow32,\
			psubTypethred->shortCycDnThredHigh32, psubTypethred->shortCycDnThredLow32);
	}
	else
	{
		printf("%s : err!\n\r", __FUNCTION__);
	}

	reVal = GPN_STAT_DEBUG_OK;
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = GPN_STAT_SUB_TYPE_INVALID;
	while(reVal == GPN_STAT_DEBUG_OK)
	{
		reVal = gpnStatSnmpApiSubStatTypeGetNext(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
		if(reVal == GPN_STAT_DEBUG_OK)
		{
			printf("%s : subType(%08x) b-scan(%08x) valClass(%2d) bitDeep(%2d) nextSubType(%08x)\n\r",\
				__FUNCTION__, pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2,\
				pgpnStatMsgSpOut->iMsgPara3, pgpnStatMsgSpOut->iMsgPara4,\
				pgpnStatMsgSpOut->iMsgPara5);

			psubBoundAlm = (stSubTypeBoundAlm *)(&(pgpnStatMsgSpOut->msgCellLen) + 1);
			psubTypethred = (stSubTypeThred *)(psubBoundAlm + 1);
			
			printf("longUpAlm(%08x) longUpEvn(%08x) longDnAlm(%08x) longDnEvn(%08x)\n\r",\
			psubBoundAlm->longCycUpAlm, psubBoundAlm->longCycUpEvt,\
			psubBoundAlm->longCycLowAlm, psubBoundAlm->longCycLowEvt);

			printf("shortUpAlm(%08x) shortUpEvn(%08x) shortDnAlm(%08x) shortDnEvn(%08x)\n\r",\
				psubBoundAlm->shortCycUpAlm, psubBoundAlm->shortCycUpEvt,\
				psubBoundAlm->shortCycLowAlm, psubBoundAlm->shortCycLowEvt);

			printf("userDefUpAlm(%08x) userDefUpEvn(%08x) userDefDnAlm(%08x) userDefDnEvn(%08x)\n\r",\
				psubBoundAlm->userDefCycUpAlm, psubBoundAlm->userDefCycUpEvt,\
				psubBoundAlm->userDefCycLowAlm, psubBoundAlm->userDefCycLowEvt);

			printf("longUpH(%08x) longUpL(%08x) longDnH(%08x) longDnL(%08x)\n\r",\
				psubTypethred->longCycUpThredHigh32, psubTypethred->longCycUpThredLow32,\
				psubTypethred->longCycDnThredHigh32, psubTypethred->longCycDnThredLow32);

			printf("shortUpH(%08x) shortUpL(%08x) shortDnH(%08x) shortDnL(%08x)\n\r",\
				psubTypethred->shortCycUpThredHigh32, psubTypethred->shortCycUpThredLow32,\
				psubTypethred->shortCycDnThredHigh32, psubTypethred->shortCycDnThredLow32);
			if(pgpnStatMsgSpOut->iMsgPara5 != GPN_STAT_SUB_TYPE_INVALID)
			{
				gpnStatMsgSpIn.iMsgPara1 = pgpnStatMsgSpOut->iMsgPara5;
			}
			else
			{
				break;
			}
		}
		else
		{
			printf("%s : err!\n\r", __FUNCTION__);
		}
	}
	#endif
	#if 1
	/*scanType & portType relation */
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET;
	gpnStatMsgSpIn.iMsgPara1 = IFM_LSP_TYPE;
	gpnStatMsgSpIn.msgCellLen = 0;
	reVal = gpnStatSnmpApiPortType2ScanTypeGet(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("%s : portType(%08x) include(%02d) f1(%08x) f2(%08x) f3(%08x) f4(%08x) f5(%08x)\n\r",\
			__FUNCTION__, pgpnStatMsgSpOut->iMsgPara1,pgpnStatMsgSpOut->iMsgPara2,\
			pgpnStatMsgSpOut->iMsgPara3, pgpnStatMsgSpOut->iMsgPara4,\
			pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6,\
			pgpnStatMsgSpOut->iMsgPara7);
	}

	reVal = GPN_STAT_DEBUG_OK;
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = GPN_ILLEGAL_PORT_INDEX;
	while(reVal == GPN_STAT_DEBUG_OK)
	{
		reVal = gpnStatSnmpApiPortType2ScanTypeGetNext(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
		if(reVal == GPN_STAT_DEBUG_OK)
		{
			printf("%s : portType(%08x) include(%02d) f1(%08x) f2(%08x) f3(%08x) f4(%08x) f5(%08x) nextType(%08x)\n\r",\
				__FUNCTION__, pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2,\
				pgpnStatMsgSpOut->iMsgPara3, pgpnStatMsgSpOut->iMsgPara4,\
				pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6,\
				pgpnStatMsgSpOut->iMsgPara7, pgpnStatMsgSpOut->iMsgPara8);
			if(pgpnStatMsgSpOut->iMsgPara8 != GPN_ILLEGAL_PORT_INDEX)
			{
				gpnStatMsgSpIn.iMsgPara1 = pgpnStatMsgSpOut->iMsgPara8;
			}
			else
			{
				break;
			}
		}
		else
		{
			printf("%s : err!\n\r", __FUNCTION__);
		}
	}
	#endif
	free(pgpnStatMsgSpOut);
}
void gpnStatForAlarmThredTpTest(UINT32 para1, UINT32 para2)
{
	UINT32 reVal;
	gpnSockMsg gpnStatMsgSpIn;
	gpnSockMsg gpnStatMsgSpOut;
	gpnSockMsg *pgpnStatMsgSpOut;
	UINT32 almThredId1;
	UINT32 almThredId2;

	/*scanType get */
	gpnStatMsgSpIn.iIndex = 1;
	gpnStatMsgSpIn.iMsgCtrl = 0;
	gpnStatMsgSpIn.iDstId = GPN_COMMM_STAT;
	gpnStatMsgSpIn.iSrcId= GPN_COMMM_STAT;
	
	pgpnStatMsgSpOut = &gpnStatMsgSpOut;

	/*get alarm threshold template Id */
	almThredId1 = gpnStatAlarmThredTemplatIdGet();
	printf("%s : get almThredId1(%d)\n\r",\
		__FUNCTION__, almThredId1);
	
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_ALM_THRED_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = almThredId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_TXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = 0x00000001;
	gpnStatMsgSpIn.iMsgPara5 = 0x00000002;
	gpnStatMsgSpIn.iMsgPara6 = 0x00000003;
	gpnStatMsgSpIn.iMsgPara7 = 0x00000004;
	gpnStatMsgSpIn.iMsgPara8 = 0x00000005;
	gpnStatMsgSpIn.iMsgPara9 = 0x00000006;
	gpnStatMsgSpIn.iMsgParaA = 0x00000007;
	gpnStatMsgSpIn.iMsgParaB = 0x00000008;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add alarm threshold(%d) err\n\r", __FUNCTION__, almThredId1);
	}

	/*get alarm threshold template Id */
	almThredId2 = gpnStatAlarmThredTemplatIdGet();
	printf("%s : get almThredId2(%d)\n\r",\
		__FUNCTION__, almThredId2);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_ALM_THRED_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = almThredId2;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = 0x00000011;
	gpnStatMsgSpIn.iMsgPara5 = 0x00000022;
	gpnStatMsgSpIn.iMsgPara6 = 0x00000033;
	gpnStatMsgSpIn.iMsgPara7 = 0x00000044;
	gpnStatMsgSpIn.iMsgPara8 = 0x00000055;
	gpnStatMsgSpIn.iMsgPara9 = 0x00000066;
	gpnStatMsgSpIn.iMsgParaA = 0x00000077;
	gpnStatMsgSpIn.iMsgParaB = 0x00000088;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add alarm threshold(%d) err\n\r", __FUNCTION__, almThredId2);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_ALM_THRED_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = almThredId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = 0x00000011;
	gpnStatMsgSpIn.iMsgPara5 = 0x00000022;
	gpnStatMsgSpIn.iMsgPara6 = 0x00000033;
	gpnStatMsgSpIn.iMsgPara7 = 0x00000044;
	gpnStatMsgSpIn.iMsgPara8 = 0x00000055;
	gpnStatMsgSpIn.iMsgPara9 = 0x00000066;
	gpnStatMsgSpIn.iMsgParaA = 0x00000077;
	gpnStatMsgSpIn.iMsgParaB = 0x00000088;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add alarm threshold(%d) err\n\r", __FUNCTION__, almThredId2);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_ALM_THRED_TEMP_MODIFY;
	gpnStatMsgSpIn.iMsgPara1 = almThredId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = 0x00000111;
	gpnStatMsgSpIn.iMsgPara5 = 0x00000222;
	gpnStatMsgSpIn.iMsgPara6 = 0x00000333;
	gpnStatMsgSpIn.iMsgPara7 = 0x00000444;
	gpnStatMsgSpIn.iMsgPara8 = 0x00000555;
	gpnStatMsgSpIn.iMsgPara9 = 0x00000666;
	gpnStatMsgSpIn.iMsgParaA = 0x00000777;
	gpnStatMsgSpIn.iMsgParaB = 0x00000888;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : modify alarm threshold(%d) err\n\r", __FUNCTION__, almThredId2);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_ALM_THRED_TEMP_GET;
	gpnStatMsgSpIn.iMsgPara1 = almThredId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatSnmpApiAlarmThredTpGet(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("threshold(%d) subType(%08x scanType(%08x)\n\r",\
			pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2, pgpnStatMsgSpOut->iMsgPara3);
		printf("upRH(%08x) upRL(%08x upDH(%08x) upDL(%08x dnRH(%08x) dnRL(%08x dnDH(%08x) dnDL(%08x)\n\r",\
			pgpnStatMsgSpOut->iMsgPara4, pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6,\
			pgpnStatMsgSpOut->iMsgPara7, pgpnStatMsgSpOut->iMsgPara8, pgpnStatMsgSpOut->iMsgPara9,\
			pgpnStatMsgSpOut->iMsgParaA, pgpnStatMsgSpOut->iMsgParaB);
	}
	else
	{
		printf("%s : get alarm threshold(%d) subType(%08x) err\n\r",\
			__FUNCTION__, almThredId2, GPN_STAT_D_PTN_LSP_RXB_TYPE);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = GPN_STAT_INVALID_TEMPLAT_ID;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_SCAN_TYPE_ALL;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = GPN_STAT_DEBUG_OK;
	while(reVal == GPN_STAT_DEBUG_OK)
	{
		reVal = gpnStatSnmpApiAlarmThredTpGetNext(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
		if(reVal == GPN_STAT_DEBUG_OK)
		{
			printf("threshold(%d) subType(%08x) scanType(%08x) nextTdId(%d) nextSubType(%08x)\n\r",\
				pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2, pgpnStatMsgSpOut->iMsgPara3,\
				pgpnStatMsgSpOut->iMsgParaC, pgpnStatMsgSpOut->iMsgParaD);
			printf("upRH(%08x) upRL(%08x upDH(%08x) upDL(%08x dnRH(%08x) dnRL(%08x dnDH(%08x) dnDL(%08x)\n\r",\
				pgpnStatMsgSpOut->iMsgPara4, pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6,\
				pgpnStatMsgSpOut->iMsgPara7, pgpnStatMsgSpOut->iMsgPara8, pgpnStatMsgSpOut->iMsgPara9,\
				pgpnStatMsgSpOut->iMsgParaA, pgpnStatMsgSpOut->iMsgParaB);

			if(pgpnStatMsgSpOut->iMsgParaC == GPN_STAT_INVALID_TEMPLAT_ID)
			{
				printf("get end\n\r");
				break;
			}
			else
			{
				gpnStatMsgSpIn.iMsgPara1 = pgpnStatMsgSpOut->iMsgParaC;
				gpnStatMsgSpIn.iMsgPara2 = pgpnStatMsgSpOut->iMsgParaD;
			}
		}
		else
		{
			printf("%s : get alarm threshold(%d) subType(%08x) err\n\r",\
				__FUNCTION__, almThredId2, GPN_STAT_D_PTN_LSP_RXB_TYPE);
		}
	}
}
void gpnStatForEventThredTpTest(UINT32 para1, UINT32 para2)
{
	UINT32 reVal;
	gpnSockMsg gpnStatMsgSpIn;
	gpnSockMsg gpnStatMsgSpOut;
	gpnSockMsg *pgpnStatMsgSpOut;
	UINT32 evnThredId1;
	UINT32 evnThredId2;

	/*scanType get */
	gpnStatMsgSpIn.iIndex = 1;
	gpnStatMsgSpIn.iMsgCtrl = 0;
	gpnStatMsgSpIn.iDstId = GPN_COMMM_STAT;
	gpnStatMsgSpIn.iSrcId= GPN_COMMM_STAT;
	
	pgpnStatMsgSpOut = &gpnStatMsgSpOut;

	/*get alarm threshold template Id */
	evnThredId1 = gpnStatEventThredTemplatIdGet();
	printf("%s : get evnThredId1(%d)\n\r",\
		__FUNCTION__, evnThredId1);
	
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = evnThredId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_TXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = 0x00000001;
	gpnStatMsgSpIn.iMsgPara5 = 0x00000002;
	gpnStatMsgSpIn.iMsgPara6 = 0x00000003;
	gpnStatMsgSpIn.iMsgPara7 = 0x00000004;
	gpnStatMsgSpIn.iMsgPara8 = 0x00000005;
	gpnStatMsgSpIn.iMsgPara9 = 0x00000006;
	gpnStatMsgSpIn.iMsgParaA = 0x00000007;
	gpnStatMsgSpIn.iMsgParaB = 0x00000008;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add event evnThredId1(%d) err\n\r", __FUNCTION__, evnThredId1);
	}

	/*get event threshold template Id */
	evnThredId2 = gpnStatEventThredTemplatIdGet();
	printf("%s : get evnThredId2(%d)\n\r",\
		__FUNCTION__, evnThredId2);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = evnThredId2;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = 0x00000011;
	gpnStatMsgSpIn.iMsgPara5 = 0x00000022;
	gpnStatMsgSpIn.iMsgPara6 = 0x00000033;
	gpnStatMsgSpIn.iMsgPara7 = 0x00000044;
	gpnStatMsgSpIn.iMsgPara8 = 0x00000055;
	gpnStatMsgSpIn.iMsgPara9 = 0x00000066;
	gpnStatMsgSpIn.iMsgParaA = 0x00000077;
	gpnStatMsgSpIn.iMsgParaB = 0x00000088;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add event evnThredId2(%d) err\n\r", __FUNCTION__, evnThredId2);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = evnThredId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = 0x00000011;
	gpnStatMsgSpIn.iMsgPara5 = 0x00000022;
	gpnStatMsgSpIn.iMsgPara6 = 0x00000033;
	gpnStatMsgSpIn.iMsgPara7 = 0x00000044;
	gpnStatMsgSpIn.iMsgPara8 = 0x00000055;
	gpnStatMsgSpIn.iMsgPara9 = 0x00000066;
	gpnStatMsgSpIn.iMsgParaA = 0x00000077;
	gpnStatMsgSpIn.iMsgParaB = 0x00000088;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add event threshold(%d) err\n\r", __FUNCTION__, evnThredId1);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_MODIFY;
	gpnStatMsgSpIn.iMsgPara1 = evnThredId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = 0x00000111;
	gpnStatMsgSpIn.iMsgPara5 = 0x00000222;
	gpnStatMsgSpIn.iMsgPara6 = 0x00000333;
	gpnStatMsgSpIn.iMsgPara7 = 0x00000444;
	gpnStatMsgSpIn.iMsgPara8 = 0x00000555;
	gpnStatMsgSpIn.iMsgPara9 = 0x00000666;
	gpnStatMsgSpIn.iMsgParaA = 0x00000777;
	gpnStatMsgSpIn.iMsgParaB = 0x00000888;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : modify event threshold(%d) err\n\r", __FUNCTION__, evnThredId1);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_GET;
	gpnStatMsgSpIn.iMsgPara1 = evnThredId2;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatSnmpApiEventThredTpGet(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("threshold(%d) subType(%08x scanType(%08x)\n\r",\
			pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2, pgpnStatMsgSpOut->iMsgPara3);
		printf("upH(%08x) upL(%08x) dnH(%08x) dnL(%08x)\n\r",\
				pgpnStatMsgSpOut->iMsgPara4, pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6,\
				pgpnStatMsgSpOut->iMsgPara7);
	}
	else
	{
		printf("%s : get alarm threshold(%d) subType(%08x) err\n\r",\
			__FUNCTION__, evnThredId2, GPN_STAT_D_PTN_LSP_RXB_TYPE);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = GPN_STAT_INVALID_TEMPLAT_ID;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_SCAN_TYPE_ALL;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = GPN_STAT_DEBUG_OK;
	while(reVal == GPN_STAT_DEBUG_OK)
	{
		reVal = gpnStatSnmpApiEventThredTpGetNext(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
		if(reVal == GPN_STAT_DEBUG_OK)
		{
			printf("threshold(%d) subType(%08x) scanType(%08x) nextTdId(%d) nextSubType(%08x)\n\r",\
				pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2, pgpnStatMsgSpOut->iMsgPara3,\
				pgpnStatMsgSpOut->iMsgPara8, pgpnStatMsgSpOut->iMsgPara9);
			printf("upH(%08x) upL(%08x) dnH(%08x) dnL(%08x)\n\r",\
				pgpnStatMsgSpOut->iMsgPara4, pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6,\
				pgpnStatMsgSpOut->iMsgPara7);

			if(pgpnStatMsgSpOut->iMsgPara8 == GPN_STAT_INVALID_TEMPLAT_ID)
			{
				printf("get end\n\r");
				break;
			}
			else
			{
				gpnStatMsgSpIn.iMsgPara1 = pgpnStatMsgSpOut->iMsgPara8;
				gpnStatMsgSpIn.iMsgPara2 = pgpnStatMsgSpOut->iMsgPara9;
			}
		}
		else
		{
			printf("%s : get alarm threshold(%d) subType(%08x) err\n\r",\
				__FUNCTION__, gpnStatMsgSpIn.iMsgPara1, gpnStatMsgSpIn.iMsgPara2);
		}
	}
}
void gpnStatForSubFiltTpTest(UINT32 para1, UINT32 para2)
{
	UINT32 reVal;
	gpnSockMsg gpnStatMsgSpIn;
	gpnSockMsg gpnStatMsgSpOut;
	gpnSockMsg *pgpnStatMsgSpOut;
	UINT32 subFiltTpId1;
	UINT32 subFiltTpId2;

	/*scanType get */
	gpnStatMsgSpIn.iIndex = 1;
	gpnStatMsgSpIn.iMsgCtrl = 0;
	gpnStatMsgSpIn.iDstId = GPN_COMMM_STAT;
	gpnStatMsgSpIn.iSrcId= GPN_COMMM_STAT;
	
	pgpnStatMsgSpOut = &gpnStatMsgSpOut;

	/*get sub filt template Id */
	subFiltTpId1 = gpnStatSubTypeFiltIdGet();
	printf("%s : get subFiltTpId1(%d)\n\r",\
		__FUNCTION__, subFiltTpId1);
	
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = subFiltTpId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_TXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_DEBUG_DISABLE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add subFiltTpId1(%d) err\n\r", __FUNCTION__, subFiltTpId1);
	}
	
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = subFiltTpId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_DROP_RATA_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_DEBUG_DISABLE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add subFiltTpId1(%d) err\n\r", __FUNCTION__, subFiltTpId1);
	}

	/*get sub filt template Id */
	subFiltTpId2 = gpnStatSubTypeFiltIdGet();
	printf("%s : get subFiltTpId2(%d)\n\r",\
		__FUNCTION__, subFiltTpId2);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = subFiltTpId2;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_DEBUG_DISABLE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add event subFiltTpId2(%d) err\n\r", __FUNCTION__, subFiltTpId2);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = subFiltTpId2;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_DELAY_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_DEBUG_DISABLE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add event subFiltTpId2(%d) err\n\r", __FUNCTION__, subFiltTpId2);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = subFiltTpId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_DEBUG_DISABLE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add subFiltTpId1(%d) err\n\r", __FUNCTION__, subFiltTpId1);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_MODIFY;
	gpnStatMsgSpIn.iMsgPara1 = subFiltTpId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_DEBUG_ENABLE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : modify subFiltTpId1(%d) err\n\r", __FUNCTION__, subFiltTpId1);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_GET;
	gpnStatMsgSpIn.iMsgPara1 = subFiltTpId2;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatSnmpApiSubFiltTpGet(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("threshold(%d) subType(%08x) scanType(%08x) status(%d)\n\r",\
			pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2, pgpnStatMsgSpOut->iMsgPara3,\
			pgpnStatMsgSpOut->iMsgPara4);
	}
	else
	{
		printf("%s : get subFiltTpId(%d) subType(%08x) err\n\r",\
			__FUNCTION__, subFiltTpId1, GPN_STAT_D_PTN_LSP_RXB_TYPE);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = GPN_STAT_INVALID_TEMPLAT_ID;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_SCAN_TYPE_ALL;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = GPN_STAT_DEBUG_OK;
	while(reVal == GPN_STAT_DEBUG_OK)
	{
		reVal = gpnStatSnmpApiSubFiltTpGetNext(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
		if(reVal == GPN_STAT_DEBUG_OK)
		{
			printf("threshold(%d) subType(%08x) scanType(%08x) status(%d) nextTdId(%d) nextSub(%08x)\n\r",\
				pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2, pgpnStatMsgSpOut->iMsgPara3,\
				pgpnStatMsgSpOut->iMsgPara4, pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6);

			if(pgpnStatMsgSpOut->iMsgPara5 == GPN_STAT_INVALID_TEMPLAT_ID)
			{
				printf("get end\n\r");
				break;
			}
			else
			{
				gpnStatMsgSpIn.iMsgPara1 = pgpnStatMsgSpOut->iMsgPara5;
				gpnStatMsgSpIn.iMsgPara2 = pgpnStatMsgSpOut->iMsgPara6;
			}
		}
		else
		{
			printf("%s : get subFiltTpId(%d) subType(%08x) err\n\r",\
				__FUNCTION__, gpnStatMsgSpIn.iMsgPara1, gpnStatMsgSpIn.iMsgPara2);
		}
	}
}
void gpnStatForSubReportTpTest(UINT32 para1, UINT32 para2)
{
	UINT32 reVal;
	gpnSockMsg gpnStatMsgSpIn;
	gpnSockMsg gpnStatMsgSpOut;
	gpnSockMsg *pgpnStatMsgSpOut;
	UINT32 subReportTpId1;
	UINT32 subReportTpId2;

	/*scanType get */
	gpnStatMsgSpIn.iIndex = 1;
	gpnStatMsgSpIn.iMsgCtrl = 0;
	gpnStatMsgSpIn.iDstId = GPN_COMMM_STAT;
	gpnStatMsgSpIn.iSrcId= GPN_COMMM_STAT;
	
	pgpnStatMsgSpOut = &gpnStatMsgSpOut;

	/*get sub report template Id */
	subReportTpId1 = gpnStatSubTypeReportIdGet();
	printf("%s : get subReportTpId1(%d)\n\r",\
		__FUNCTION__, subReportTpId1);
	
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBREPORT_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = subReportTpId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_TXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_DEBUG_ENABLE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add subReportTpId1(%d) err\n\r", __FUNCTION__, subReportTpId1);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBREPORT_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = subReportTpId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_DROP_RATA_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_DEBUG_ENABLE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add subReportTpId1(%d) err\n\r", __FUNCTION__, subReportTpId1);
	}

	/*get sub report template Id */
	subReportTpId2 = gpnStatSubTypeReportIdGet();
	printf("%s : get subReportTpId2(%d)\n\r",\
		__FUNCTION__, subReportTpId2);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBREPORT_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = subReportTpId2;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_DEBUG_ENABLE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add event subReportTpId2(%d) err\n\r", __FUNCTION__, subReportTpId2);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBREPORT_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = subReportTpId2;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_DELAY_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_DEBUG_ENABLE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add event subReportTpId2(%d) err\n\r", __FUNCTION__, subReportTpId2);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBREPORT_TEMP_ADD;
	gpnStatMsgSpIn.iMsgPara1 = subReportTpId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_DEBUG_ENABLE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add subReportTpId1(%d) err\n\r", __FUNCTION__, subReportTpId1);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBREPORT_TEMP_MODIFY;
	gpnStatMsgSpIn.iMsgPara1 = subReportTpId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_DEBUG_ENABLE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : modify subReportTpId1(%d) err\n\r", __FUNCTION__, subReportTpId1);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBREPORT_TEMP_GET;
	gpnStatMsgSpIn.iMsgPara1 = subReportTpId2;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_D_PTN_LSP_RXB_TYPE;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = gpnStatSnmpApiSubReportTpGet(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("threshold(%d) subType(%08x scanType(%08x) status(%d)\n\r",\
			pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2, pgpnStatMsgSpOut->iMsgPara3,\
			pgpnStatMsgSpOut->iMsgPara4);
	}
	else
	{
		printf("%s : get subReportTpId(%d) subType(%08x) err\n\r",\
			__FUNCTION__, subReportTpId1, GPN_STAT_D_PTN_LSP_RXB_TYPE);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = GPN_STAT_INVALID_TEMPLAT_ID;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_SCAN_TYPE_ALL;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = GPN_STAT_DEBUG_OK;
	while(reVal == GPN_STAT_DEBUG_OK)
	{
		reVal = gpnStatSnmpApiSubReportTpGetNext(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
		if(reVal == GPN_STAT_DEBUG_OK)
		{
			printf("threshold(%d) subType(%08x) scanType(%08x) status(%d) nextTdId(%d) nextSub(%08x)\n\r",\
				pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2, pgpnStatMsgSpOut->iMsgPara3,\
				pgpnStatMsgSpOut->iMsgPara4, pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6);

			if(pgpnStatMsgSpOut->iMsgPara5 == GPN_STAT_INVALID_TEMPLAT_ID)
			{
				printf("get end\n\r");
				break;
			}
			else
			{
				gpnStatMsgSpIn.iMsgPara1 = pgpnStatMsgSpOut->iMsgPara5;
				gpnStatMsgSpIn.iMsgPara2 = pgpnStatMsgSpOut->iMsgPara6;
			}
		}
		else
		{
			printf("%s : get subReportTpId(%d) subType(%08x) err\n\r",\
				__FUNCTION__, gpnStatMsgSpIn.iMsgPara1, gpnStatMsgSpIn.iMsgPara2);
		}
	}
}
void gpnStatForTaskTest(UINT32 para1, UINT32 para2)
{
	UINT32 reVal;
	UINT32 taskId1;
	UINT32 taskId2;
	gpnSockMsg gpnStatMsgSpIn;
	gpnSockMsg gpnStatMsgSpOut;
	gpnSockMsg *pgpnStatMsgSpOut;
	
	/*add alarm threshold template */
	gpnStatForAlarmThredTpTest(0, 0);

	/*add alarm threshold template */
	gpnStatForSubReportTpTest(0, 0);

	/*scanType get */
	gpnStatMsgSpIn.iIndex = 1;
	gpnStatMsgSpIn.iMsgCtrl = 0;
	gpnStatMsgSpIn.iDstId = GPN_COMMM_STAT;;
	gpnStatMsgSpIn.iSrcId= GPN_COMMM_STAT;
		
	pgpnStatMsgSpOut = &gpnStatMsgSpOut;

	/*get sub report template Id */
	taskId1 = gpnStatTaskIdGet();
	printf("%s: taskId1 %d\n\r", __FUNCTION__, taskId1);
		
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_TASK_ADD;
	gpnStatMsgSpIn.iMsgPara1 = taskId1;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara3 = 3/*taskMonObjNum*/;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_CYC_CLASS_LONG;
	gpnStatMsgSpIn.iMsgPara5 = 15;
	gpnStatMsgSpIn.iMsgPara6 = 0x44444444;
	gpnStatMsgSpIn.iMsgPara7 = 0x55555555;
	gpnStatMsgSpIn.iMsgPara8 = 1;
	gpnStatMsgSpIn.iMsgPara9 = 2;
	gpnStatMsgSpIn.msgCellLen = 0;
	
	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add taskId(%d) err\n\r", __FUNCTION__, taskId1);
	}

	/*get sub report template Id */
	taskId2 = gpnStatTaskIdGet();
	printf("%s: taskId2 %d\n\r", __FUNCTION__, taskId2);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_TASK_ADD;
	gpnStatMsgSpIn.iMsgPara1 = taskId2;
	gpnStatMsgSpIn.iMsgPara2 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara3 = 4/*taskMonObjNum*/;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_CYC_CLASS_SHORT;
	gpnStatMsgSpIn.iMsgPara5 = 15;
	gpnStatMsgSpIn.iMsgPara6 = 0x44444444;
	gpnStatMsgSpIn.iMsgPara7 = 0x55555555;
	gpnStatMsgSpIn.iMsgPara8 = 2;
	gpnStatMsgSpIn.iMsgPara9 = 1;
	gpnStatMsgSpIn.msgCellLen = 0;
	
	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : add taskId(%d) err\n\r", __FUNCTION__, taskId2);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_TASK_GET;
	gpnStatMsgSpIn.iMsgPara1 = taskId1;
	gpnStatMsgSpIn.msgCellLen = 0;
	reVal = gpnStatSnmpApiTaskGet(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : get taskId(%d) err\n\r", __FUNCTION__, taskId2);
	}
	else
	{
		printf("taskId(%d) scanType(%08x) num(%d) cyc(%d) cycSecs(%d) s(%08x) e(%08x) alm(%d) subReport(%d)\n\r",\
			gpnStatMsgSpOut.iMsgPara1,\
			gpnStatMsgSpOut.iMsgPara2,\
			gpnStatMsgSpOut.iMsgPara3,\
			gpnStatMsgSpOut.iMsgPara4,\
			gpnStatMsgSpOut.iMsgPara5,\
			gpnStatMsgSpOut.iMsgPara6,\
			gpnStatMsgSpOut.iMsgPara7,\
			gpnStatMsgSpOut.iMsgPara8,\
			gpnStatMsgSpOut.iMsgPara9);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_TASK_MODIFY;
	gpnStatMsgSpIn.iMsgPara1 = taskId2;
	gpnStatMsgSpIn.iMsgPara2 = 0;
	gpnStatMsgSpIn.iMsgPara3 = 0/*taskMonObjNum*/;
	gpnStatMsgSpIn.iMsgPara4 = GPN_STAT_CYC_CLASS_LONG;
	gpnStatMsgSpIn.iMsgPara5 = 20;
	gpnStatMsgSpIn.iMsgPara6 = 0x66666666;
	gpnStatMsgSpIn.iMsgPara7 = 0x77777777;
	gpnStatMsgSpIn.iMsgPara8 = 1;
	gpnStatMsgSpIn.iMsgPara9 = 2;
	gpnStatMsgSpIn.msgCellLen = 0;
	
	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : modify taskId(%d) err\n\r", __FUNCTION__, taskId2);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_TASK_GET;
	gpnStatMsgSpIn.iMsgPara1 = taskId2;
	gpnStatMsgSpIn.msgCellLen = 0;
	reVal = gpnStatSnmpApiTaskGet(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		printf("%s : get taskId(%d) err\n\r", __FUNCTION__, taskId2);
	}
	else
	{
		printf("taskId(%d) scanType(%08x) num(%d) cyc(%d) cycSecs(%d) s(%08x) e(%08x) alm(%d) subReport(%d)\n\r",\
			gpnStatMsgSpOut.iMsgPara1,\
			gpnStatMsgSpOut.iMsgPara2,\
			gpnStatMsgSpOut.iMsgPara3,\
			gpnStatMsgSpOut.iMsgPara4,\
			gpnStatMsgSpOut.iMsgPara5,\
			gpnStatMsgSpOut.iMsgPara6,\
			gpnStatMsgSpOut.iMsgPara7,\
			gpnStatMsgSpOut.iMsgPara8,\
			gpnStatMsgSpOut.iMsgPara9);
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_TASK_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = GPN_STAT_INVALID_TEMPLAT_ID;
	gpnStatMsgSpIn.msgCellLen = 0;

	reVal = GPN_STAT_DEBUG_OK;
	while(reVal == GPN_STAT_DEBUG_OK)
	{
		reVal = gpnStatSnmpApiTaskGetNext(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, sizeof(gpnSockMsg));
		if(reVal == GPN_STAT_DEBUG_OK)
		{
			printf("taskId(%d) scanType(%08x) num(%d) cyc(%d) cycSecs(%d) s(%08x) e(%08x) alm(%d) subReport(%d) nextTask(%d)\n\r",\
				gpnStatMsgSpOut.iMsgPara1,\
				gpnStatMsgSpOut.iMsgPara2,\
				gpnStatMsgSpOut.iMsgPara3,\
				gpnStatMsgSpOut.iMsgPara4,\
				gpnStatMsgSpOut.iMsgPara5,\
				gpnStatMsgSpOut.iMsgPara6,\
				gpnStatMsgSpOut.iMsgPara7,\
				gpnStatMsgSpOut.iMsgPara8,\
				gpnStatMsgSpOut.iMsgPara9,\
				gpnStatMsgSpOut.iMsgParaA);

			if(gpnStatMsgSpOut.iMsgParaA == GPN_STAT_INVALID_TEMPLAT_ID)
			{
				printf("get end\n\r");
				break;
			}
			else
			{
				gpnStatMsgSpIn.iMsgPara1 = gpnStatMsgSpOut.iMsgParaA;
			}
		}
		else
		{
			printf("get task(%d) err\n\r", gpnStatMsgSpIn.iMsgPara1);
		}
	}
}

void gpnStatForPortMonTest(UINT32 para1, UINT32 para2)
{
	UINT32 reVal;
	objLogicDesc portIndex1;
	objLogicDesc portIndex2;
	gpnSockMsg gpnStatMsgSpIn;
	gpnSockMsg *pgpnStatMsgSpOut;
	stStatMsgMonObjCfg *pmonCfg;
	objLogicDesc *pnextPortIndex;
	UINT32 *pscanType;

	pgpnStatMsgSpOut = (gpnSockMsg *)malloc(sizeof(gpnSockMsg) + sizeof(stStatMsgMonObjCfg) +\
		sizeof(objLogicDesc) + sizeof(UINT32));
	/*first : creat port for test */
	
	portIndex1.devIndex = 1;
	//portIndex1.devIndex = DeviceIndex_Create(1, 0);
	portIndex1.portIndex = LAB_PortIndex_Create(IFM_LSP_TYPE, 0, 1234);
	portIndex1.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex1.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex1.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex1.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_MON_ADD;	
	gpnStatMsgSpIn.iMsgPara1 = portIndex1.devIndex;
	gpnStatMsgSpIn.iMsgPara2 = portIndex1.portIndex;
	gpnStatMsgSpIn.iMsgPara3 = portIndex1.portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = portIndex1.portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = portIndex1.portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = portIndex1.portIndex6;
	gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("%s : add dev(%08x) port(%08x|%08x|%08x|%08x|%08x)!\n\r",\
			__FUNCTION__, portIndex1.devIndex, portIndex1.portIndex, portIndex1.portIndex3,\
			portIndex1.portIndex4, portIndex1.portIndex5, portIndex1.portIndex6);
	}
	else
	{
		printf("%s : add portMon err!\n\r", __FUNCTION__);
		return;
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_BASE_MON_OPT;
	gpnStatMsgSpIn.iMsgPara1 = portIndex1.devIndex;
	gpnStatMsgSpIn.iMsgPara2 = portIndex1.portIndex;
	gpnStatMsgSpIn.iMsgPara3 = portIndex1.portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = portIndex1.portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = portIndex1.portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = portIndex1.portIndex6;
	gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara8 = GPN_STAT_DEBUG_ENABLE;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("%s : enable base dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x)!\n\r",\
			__FUNCTION__, portIndex1.devIndex, portIndex1.portIndex, portIndex1.portIndex3,\
			portIndex1.portIndex4, portIndex1.portIndex5, portIndex1.portIndex6, GPN_STAT_SCAN_TYPE_PTN_LSP);
	}
	else
	{
		printf("%s : enable porBasetMon err!\n\r", __FUNCTION__);
		return;
	}

	pgpnStatMsgSpOut->msgCellLen = sizeof(stStatMsgMonObjCfg);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_PORT_CYC_MON_OPT;
	gpnStatMsgSpIn.iMsgPara1 = portIndex1.devIndex;
	gpnStatMsgSpIn.iMsgPara2 = portIndex1.portIndex;
	gpnStatMsgSpIn.iMsgPara3 = portIndex1.portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = portIndex1.portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = portIndex1.portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = portIndex1.portIndex6;
	gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara8 = 2/*taskId*/;
	gpnStatMsgSpIn.iMsgPara9 = GPN_STAT_DEBUG_ENABLE;
	gpnStatMsgSpIn.iMsgParaA = 2/*event tp id*/;
	gpnStatMsgSpIn.iMsgParaB = 1/*sub filt tp id*/;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("%s : enable cyc dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x)!\n\r",\
			__FUNCTION__, portIndex1.devIndex, portIndex1.portIndex, portIndex1.portIndex3,\
			portIndex1.portIndex4, portIndex1.portIndex5, portIndex1.portIndex6, GPN_STAT_SCAN_TYPE_PTN_LSP);
	}
	else
	{
		printf("%s : enable porCyctMon err!\n\r", __FUNCTION__);
		return;
	}

	/*add port other */
	/*first : creat port for test */
	portIndex2.devIndex = 1;
	portIndex2.portIndex = LAB_PortIndex_Create(IFM_LSP_TYPE, 0, 101);
	portIndex2.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex2.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex2.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex2.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_MON_ADD;	
	gpnStatMsgSpIn.iMsgPara1 = portIndex2.devIndex;
	gpnStatMsgSpIn.iMsgPara2 = portIndex2.portIndex;
	gpnStatMsgSpIn.iMsgPara3 = portIndex2.portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = portIndex2.portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = portIndex2.portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = portIndex2.portIndex6;
	gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("%s : add dev(%08x) port(%08x|%08x|%08x|%08x|%08x)!\n\r",\
			__FUNCTION__, portIndex2.devIndex, portIndex2.portIndex, portIndex2.portIndex3,\
			portIndex2.portIndex4, portIndex2.portIndex5, portIndex2.portIndex6);
	}
	else
	{
		printf("%s : add portMon err!\n\r", __FUNCTION__);
		return;
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_BASE_MON_OPT;
	gpnStatMsgSpIn.iMsgPara1 = portIndex2.devIndex;
	gpnStatMsgSpIn.iMsgPara2 = portIndex2.portIndex;
	gpnStatMsgSpIn.iMsgPara3 = portIndex2.portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = portIndex2.portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = portIndex2.portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = portIndex2.portIndex6;
	gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara8 = GPN_STAT_DEBUG_ENABLE;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("%s : enable base dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x)!\n\r",\
			__FUNCTION__, portIndex2.devIndex, portIndex2.portIndex, portIndex2.portIndex3,\
			portIndex2.portIndex4, portIndex2.portIndex5, portIndex2.portIndex6, GPN_STAT_SCAN_TYPE_PTN_LSP);
	}
	else
	{
		printf("%s : enable porBasetMon err!\n\r", __FUNCTION__);
		return;
	}

	pgpnStatMsgSpOut->msgCellLen = sizeof(stStatMsgMonObjCfg);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_PORT_CYC_MON_OPT;
	gpnStatMsgSpIn.iMsgPara1 = portIndex2.devIndex;
	gpnStatMsgSpIn.iMsgPara2 = portIndex2.portIndex;
	gpnStatMsgSpIn.iMsgPara3 = portIndex2.portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = portIndex2.portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = portIndex2.portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = portIndex2.portIndex6;
	gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	gpnStatMsgSpIn.iMsgPara8 = 1/*taskId*/;
	gpnStatMsgSpIn.iMsgPara9 = GPN_STAT_DEBUG_ENABLE;
	gpnStatMsgSpIn.iMsgParaA = 1/*event tp id*/;
	gpnStatMsgSpIn.iMsgParaB = 2/*sub filt tp id*/;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("%s : enable cyc dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x)!\n\r",\
			__FUNCTION__, portIndex2.devIndex, portIndex2.portIndex, portIndex2.portIndex3,\
			portIndex2.portIndex4, portIndex2.portIndex5, portIndex2.portIndex6, GPN_STAT_SCAN_TYPE_PTN_LSP);
	}
	else
	{
		printf("%s : enable porCyctMon err!\n\r", __FUNCTION__);
		return;
	}
	
	pgpnStatMsgSpOut->msgCellLen = sizeof(stStatMsgMonObjCfg);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_MON_GET;
	gpnStatMsgSpIn.iMsgPara1 = portIndex1.devIndex;
	gpnStatMsgSpIn.iMsgPara2 = portIndex1.portIndex;
	gpnStatMsgSpIn.iMsgPara3 = portIndex1.portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = portIndex1.portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = portIndex1.portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = portIndex1.portIndex6;
	gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("%s : get dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x)!\n\r",\
			__FUNCTION__, pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2,\
			pgpnStatMsgSpOut->iMsgPara3, pgpnStatMsgSpOut->iMsgPara4,\
			pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6,\
			pgpnStatMsgSpOut->iMsgPara7);


		pmonCfg = (stStatMsgMonObjCfg *)(&pgpnStatMsgSpOut->msgCellLen + 1);
		printf("gEn     %d currEn %d\n\r",\
			pmonCfg->statMoniEn, pmonCfg->currStatMoniEn);
		printf("longEn  %d task %08x evnThId %08x subFiltThId %08x almThId %08x rept %08x histData %08x\n\r",\
			pmonCfg->longCycStatMoniEn, pmonCfg->longCycBelongTask, pmonCfg->longCycEvnThredTpId,\
			pmonCfg->longCycSubFiltTpId, pmonCfg->longCycAlmThredTpId, pmonCfg->longCycHistReptTpId,\
			pmonCfg->longCycHistDBId);
		printf("shortEn %d task %08x evnThId %08x subFiltThId %08x almThId %08x rept %08x histData %08x\n\r",\
			pmonCfg->shortCycStatMoniEn, pmonCfg->shortCycBelongTask, pmonCfg->shortCycEvnThredTpId,\
			pmonCfg->shortCycSubFiltTpId, pmonCfg->shortCycAlmThredTpId, pmonCfg->shortCycHistReptTpId,\
			pmonCfg->shortCycHistDBId);
		printf("udEn    %d task %08x evnThId %08x subFiltThId %08x almThId %08x rept %08x histData %08x secs %08x\n\r",\
			pmonCfg->udCycStatMoniEn, pmonCfg->udCycBelongTask, pmonCfg->udCycEvnThredTpId,\
			pmonCfg->udCycSubFiltTpId, pmonCfg->udCycAlmThredTpId, pmonCfg->udCycHistReptTpId,\
			pmonCfg->udCycHistDBId, pmonCfg->udCycSecs);
	}
	else
	{
		printf("%s : get portMon err!\n\r", __FUNCTION__);
		return;
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_MON_GET;
	gpnStatMsgSpIn.iMsgPara1 = portIndex2.devIndex;
	gpnStatMsgSpIn.iMsgPara2 = portIndex2.portIndex;
	gpnStatMsgSpIn.iMsgPara3 = portIndex2.portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = portIndex2.portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = portIndex2.portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = portIndex2.portIndex6;
	gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_SCAN_TYPE_PTN_LSP;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		printf("%s : get dev(%08x) port(%08x|%08x|%08x|%08x|%08x)! scanType(%08x)\n\r",\
			__FUNCTION__, pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2,\
			pgpnStatMsgSpOut->iMsgPara3, pgpnStatMsgSpOut->iMsgPara4,\
			pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6,\
			pgpnStatMsgSpOut->iMsgPara7);


		pmonCfg = (stStatMsgMonObjCfg *)(&pgpnStatMsgSpOut->msgCellLen + 1);
		printf("gEn     %d currEn %d\n\r",\
			pmonCfg->statMoniEn, pmonCfg->currStatMoniEn);
		printf("longEn  %d task %08x evnThId %08x subFiltThId %08x almThId %08x rept %08x histData %08x\n\r",\
			pmonCfg->longCycStatMoniEn, pmonCfg->longCycBelongTask, pmonCfg->longCycEvnThredTpId,\
			pmonCfg->longCycSubFiltTpId, pmonCfg->longCycAlmThredTpId, pmonCfg->longCycHistReptTpId,\
			pmonCfg->longCycHistDBId);
		printf("shortEn %d task %08x evnThId %08x subFiltThId %08x almThId %08x rept %08x histData %08x\n\r",\
			pmonCfg->shortCycStatMoniEn, pmonCfg->shortCycBelongTask, pmonCfg->shortCycEvnThredTpId,\
			pmonCfg->shortCycSubFiltTpId, pmonCfg->shortCycAlmThredTpId, pmonCfg->shortCycHistReptTpId,\
			pmonCfg->shortCycHistDBId);
		printf("udEn    %d task %08x evnThId %08x subFiltThId %08x almThId %08x rept %08x histData %08x secs %08x\n\r",\
			pmonCfg->udCycStatMoniEn, pmonCfg->udCycBelongTask, pmonCfg->udCycEvnThredTpId,\
			pmonCfg->udCycSubFiltTpId, pmonCfg->udCycAlmThredTpId, pmonCfg->udCycHistReptTpId,\
			pmonCfg->udCycHistDBId, pmonCfg->udCycSecs);
	}
	else
	{
		printf("%s : get portMon err!\n\r", __FUNCTION__);
		return;
	}

	pgpnStatMsgSpOut->msgCellLen = sizeof(stStatMsgMonObjCfg) + sizeof(objLogicDesc) + sizeof(UINT32);
	
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_MON_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = GPN_ILLEGAL_DEVICE_INDEX;
	gpnStatMsgSpIn.iMsgPara2 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara3 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara4 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara5 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara6 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_SCAN_TYPE_ALL;
	reVal = GPN_STAT_DEBUG_OK;
	while(reVal == GPN_STAT_DEBUG_OK)
	{
		reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
		if(reVal == GPN_STAT_DEBUG_OK)
		{
			printf("get      dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x)!\n\r",\
				pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2,\
				pgpnStatMsgSpOut->iMsgPara3, pgpnStatMsgSpOut->iMsgPara4,\
				pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6,\
				pgpnStatMsgSpOut->iMsgPara7);

			pmonCfg = (stStatMsgMonObjCfg *)(&pgpnStatMsgSpOut->msgCellLen + 1);
			pnextPortIndex = (objLogicDesc *)(pmonCfg + 1);
			pscanType = (UINT32 *)(pnextPortIndex + 1);

			printf("get next dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x)!\n\r",\
				pnextPortIndex->devIndex, pnextPortIndex->portIndex,\
				pnextPortIndex->portIndex3, pnextPortIndex->portIndex4,\
				pnextPortIndex->portIndex5, pnextPortIndex->portIndex6,\
				*pscanType);

			printf("gEn     %d currEn %d\n\r",\
				pmonCfg->statMoniEn, pmonCfg->currStatMoniEn);
			printf("longEn  %d task %08x evnThId %08x subFiltThId %08x almThId %08x rept %08x histData %08x\n\r",\
				pmonCfg->longCycStatMoniEn, pmonCfg->longCycBelongTask, pmonCfg->longCycEvnThredTpId,\
				pmonCfg->longCycSubFiltTpId, pmonCfg->longCycAlmThredTpId, pmonCfg->longCycHistReptTpId,\
				pmonCfg->longCycHistDBId);
			printf("shortEn %d task %08x evnThId %08x subFiltThId %08x almThId %08x rept %08x histData %08x\n\r",\
				pmonCfg->shortCycStatMoniEn, pmonCfg->shortCycBelongTask, pmonCfg->shortCycEvnThredTpId,\
				pmonCfg->shortCycSubFiltTpId, pmonCfg->shortCycAlmThredTpId, pmonCfg->shortCycHistReptTpId,\
				pmonCfg->shortCycHistDBId);
			printf("udEn    %d task %08x evnThId %08x subFiltThId %08x almThId %08x rept %08x histData %08x secs %08x\n\r",\
				pmonCfg->udCycStatMoniEn, pmonCfg->udCycBelongTask, pmonCfg->udCycEvnThredTpId,\
				pmonCfg->udCycSubFiltTpId, pmonCfg->udCycAlmThredTpId, pmonCfg->udCycHistReptTpId,\
				pmonCfg->udCycHistDBId, pmonCfg->udCycSecs);

			if(pnextPortIndex->portIndex != GPN_ILLEGAL_PORT_INDEX)
			{
				gpnStatMsgSpIn.iMsgPara1 = pnextPortIndex->devIndex;
				gpnStatMsgSpIn.iMsgPara2 = pnextPortIndex->portIndex;
				gpnStatMsgSpIn.iMsgPara3 = pnextPortIndex->portIndex3;
				gpnStatMsgSpIn.iMsgPara4 = pnextPortIndex->portIndex4;
				gpnStatMsgSpIn.iMsgPara5 = pnextPortIndex->portIndex5;
				gpnStatMsgSpIn.iMsgPara6 = pnextPortIndex->portIndex6;
				gpnStatMsgSpIn.iMsgPara7 = *pscanType;
			}
			else
			{
				printf("get end!\n\r");
				break;
			}
		}
		else
		{
			printf("%s : get portMon err!\n\r", __FUNCTION__);
			return;
		}
	}
	free(pgpnStatMsgSpOut);
}
void gpnStatForCurrDataGet(UINT32 para1, UINT32 para2)
{
	UINT32 reVal;
	objLogicDesc portIndex;
	gpnSockMsg gpnStatMsgSpIn;
	gpnSockMsg *pgpnStatMsgSpOut;
	gpnStatMsgCurrDataIndex *nextIndex;

	pgpnStatMsgSpOut = (gpnSockMsg *)malloc(sizeof(gpnSockMsg) + sizeof(gpnStatMsgCurrDataIndex));

	/*first : creat port for test */
	portIndex.devIndex = 1;
	portIndex.portIndex = LAB_PortIndex_Create(IFM_LSP_TYPE, 0, 100);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_CURR_DATA_GET;	
	gpnStatMsgSpIn.iMsgPara1 = portIndex.devIndex;
	gpnStatMsgSpIn.iMsgPara2 = portIndex.portIndex;
	gpnStatMsgSpIn.iMsgPara3 = portIndex.portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = portIndex.portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = portIndex.portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = portIndex.portIndex6;
	gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_D_PTN_LSP_RXF_TYPE;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		zlog_debug(STATIS_DBG_GET, "%s : dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x) subType(%08x) (%08x %08x)\n\r",\
			__FUNCTION__, pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2,\
			pgpnStatMsgSpOut->iMsgPara3, pgpnStatMsgSpOut->iMsgPara4,\
			pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6,\
			pgpnStatMsgSpOut->iMsgPara8, pgpnStatMsgSpOut->iMsgPara7,\
			pgpnStatMsgSpOut->iMsgPara9, pgpnStatMsgSpOut->iMsgParaA);
	}
	else
	{
		zlog_debug(STATIS_DBG_GET, "%s : get curr data none!\n\r", __FUNCTION__);
		return;
	}

	pgpnStatMsgSpOut->msgCellLen = sizeof(gpnSockMsg) + sizeof(gpnStatMsgCurrDataIndex);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_CURR_DATA_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = GPN_ILLEGAL_DEVICE_INDEX;
	gpnStatMsgSpIn.iMsgPara2 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara3 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara4 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara5 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara6 = GPN_ILLEGAL_PORT_INDEX;
	gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_SUB_TYPE_INVALID;
	
	reVal = GPN_STAT_DEBUG_OK;
	while(reVal == GPN_STAT_DEBUG_OK)
	{
		reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
		if(reVal == GPN_STAT_DEBUG_OK)
		{
			zlog_debug(STATIS_DBG_GET, "get  dev(%08x) port(%08x|%08x|%08x|%08x|%08x) subType(%08x) scanType(%08x) (%08x %08x)\n\r",\
				portIndex.devIndex, portIndex.portIndex, portIndex.portIndex3,\
				portIndex.portIndex4, portIndex.portIndex5, portIndex.portIndex6,\
				pgpnStatMsgSpOut->iMsgPara7, pgpnStatMsgSpOut->iMsgPara8,\
				pgpnStatMsgSpOut->iMsgPara9, pgpnStatMsgSpOut->iMsgParaA);

			nextIndex = (gpnStatMsgCurrDataIndex *)(&pgpnStatMsgSpOut->msgCellLen + 1);
			zlog_debug(STATIS_DBG_GET, "next dev(%08x) port(%08x|%08x|%08x|%08x|%08x) subType(%08x)\n\r",\
				nextIndex->portIndex.devIndex, nextIndex->portIndex.portIndex,\
				nextIndex->portIndex.portIndex3, nextIndex->portIndex.portIndex4,\
				nextIndex->portIndex.portIndex5, nextIndex->portIndex.portIndex6,\
				nextIndex->statSubType);
			if(nextIndex->portIndex.portIndex != GPN_ILLEGAL_PORT_INDEX)
			{
				gpnStatMsgSpIn.iMsgPara1 = nextIndex->portIndex.devIndex;
				gpnStatMsgSpIn.iMsgPara2 = nextIndex->portIndex.portIndex;
				gpnStatMsgSpIn.iMsgPara3 = nextIndex->portIndex.portIndex3;
				gpnStatMsgSpIn.iMsgPara4 = nextIndex->portIndex.portIndex4;
				gpnStatMsgSpIn.iMsgPara5 = nextIndex->portIndex.portIndex5;
				gpnStatMsgSpIn.iMsgPara6 = nextIndex->portIndex.portIndex6;
				gpnStatMsgSpIn.iMsgPara7 = nextIndex->statSubType;
			}
			else
			{
				//printf("get end!\n\r");
				break;
			}
		}
		else
		{
			printf("%s : get curr data err!\n\r", __FUNCTION__);
			return;
		}
	}
	free(pgpnStatMsgSpOut);
}

void gpnStatForHistDataGet(UINT32 para1, UINT32 para2)
{
	UINT32 reVal;
	gpnSockMsg gpnStatMsgSpIn;
	gpnSockMsg *pgpnStatMsgSpOut;

	pgpnStatMsgSpOut = (gpnSockMsg *)malloc(sizeof(gpnSockMsg));

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_HIST_DATA_GET;	
	gpnStatMsgSpIn.iMsgPara1 = 1;
	gpnStatMsgSpIn.iMsgPara2 = 0;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_D_PTN_LSP_TXB_TYPE;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		zlog_debug(STATIS_DBG_GET, "%s : histId(%d) stopTime(%08x) subType(%08x) scanType(%08x) (%08x %08x)\n\r",\
			__FUNCTION__, pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2,\
			pgpnStatMsgSpOut->iMsgPara3, pgpnStatMsgSpOut->iMsgPara4,\
			pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6);
	}
	else
	{
		zlog_debug(STATIS_DBG_GET, "%s : get hist data none!\n\r", __FUNCTION__);
		return;
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_HIST_DATA_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = GPN_STAT_INVALID_TEMPLAT_ID;
	gpnStatMsgSpIn.iMsgPara2 = 0;
	gpnStatMsgSpIn.iMsgPara3 = GPN_STAT_SUB_TYPE_INVALID;
	
	reVal = GPN_STAT_DEBUG_OK;
	while(reVal == GPN_STAT_DEBUG_OK)
	{
		reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
		if(reVal == GPN_STAT_DEBUG_OK)
		{
			zlog_debug(STATIS_DBG_GET, "this histId(%d) stopTime(%08x) subType(%08x) scanType(%08x) (%08x %08x)\n\r",\
				pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara2,\
				pgpnStatMsgSpOut->iMsgPara3, pgpnStatMsgSpOut->iMsgPara4,\
				pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6);

			zlog_debug(STATIS_DBG_GET, "next histId(%d) stopTime(%08x) subType(%08x)\n\r",\
				pgpnStatMsgSpOut->iMsgPara7, pgpnStatMsgSpOut->iMsgPara8,\
				pgpnStatMsgSpOut->iMsgPara9);
			
			if(pgpnStatMsgSpOut->iMsgPara7 != GPN_STAT_INVALID_TEMPLAT_ID)
			{
				gpnStatMsgSpIn.iMsgPara1 = pgpnStatMsgSpOut->iMsgPara7;
				gpnStatMsgSpIn.iMsgPara2 = pgpnStatMsgSpOut->iMsgPara8;
				gpnStatMsgSpIn.iMsgPara3 = pgpnStatMsgSpOut->iMsgPara9;
			}
			else
			{
				printf("get end!\n\r");
				break;
			}
		}
		else
		{
			printf("%s : get hist data err!\n\r", __FUNCTION__);
			return;
		}
	}
	free(pgpnStatMsgSpOut);
}

void gpnStatForCalculateTest(UINT32 para1, UINT32 para2)
{
	stgpnStatTestData tData;
	stgpnStatTestData tCurr;
	stgpnStatTestData tBase;

	tData.h1 = 2;
	tData.l1 = 2;
	tData.h2 = 2;
	tData.l2 = 2;
	tData.x3 = 5;
	tData.h4 = 1;
	tData.l4 = 1;
	tData.x5 = 1;
	printf("tData 1(%08x %08x) 2(%08x %08x) 3(%08x) 4(%08x %08x) 5(%08x)\n\r",\
		tData.h1, tData.l1, tData.h2, tData.l2, tData.x3,\
		tData.h4, tData.l4, tData.x5);

	tBase.h1 = 1;
	tBase.l1 = 1;
	tBase.h2 = 1;
	tBase.l2 = 3;
	tBase.x3 = 1;
	tBase.h4 = 2;
	tBase.l4 = 2;
	tBase.x5 = 0;
	printf("tBase 1(%08x %08x) 2(%08x %08x) 3(%08x) 4(%08x %08x) 5(%08x)\n\r",\
		tBase.h1, tBase.l1, tBase.h2, tBase.l2, tBase.x3,\
		tBase.h4, tBase.l4, tBase.x5);

	tCurr.h1 = 0;
	tCurr.l1 = 0xFFFFFFFF;
	tCurr.h2 = 0;
	tCurr.l2 = 0;
	tCurr.x3 = 0;
	tCurr.h4 = 0;
	tCurr.l4 = 0;
	tCurr.x5 = 0;
	printf("tCurr 1(%08x %08x) 2(%08x %08x) 3(%08x) 4(%08x %08x) 5(%08x)\n\r",\
		tCurr.h1, tCurr.l1, tCurr.h2, tCurr.l2, tCurr.x3,\
		tCurr.h4, tCurr.l4, tCurr.x5);

	gpnStatDataCollectBase2CurrProc(&tBase, &tCurr, &tData,\
		0xFFFFFFEB, 0xFFFFFFFF, 5, 1);

	printf("tData 1(%08x %08x) 2(%08x %08x) 3(%08x) 4(%08x %08x) 5(%08x)\n\r",\
		tData.h1, tData.l1, tData.h2, tData.l2, tData.x3,\
		tData.h4, tData.l4, tData.x5);
	printf("tBase 1(%08x %08x) 2(%08x %08x) 3(%08x) 4(%08x %08x) 5(%08x)\n\r",\
		tBase.h1, tBase.l1, tBase.h2, tBase.l2, tBase.x3,\
		tBase.h4, tBase.l4, tBase.x5);
	printf("tCurr 1(%08x %08x) 2(%08x %08x) 3(%08x) 4(%08x %08x) 5(%08x)\n\r",\
		tCurr.h1, tCurr.l1, tCurr.h2, tCurr.l2, tCurr.x3,\
		tCurr.h4, tCurr.l4, tCurr.x5);
}

void gpnStatForXPtnLspDataGetRsp(UINT32 para1, UINT32 para2)
{
	static gpnStatPtnLspPData Pdata;
	static gpnStatPtnLspMData Mdata;
	static UINT32 i;
	optObjOrient portIndex;
	UINT32 statType;

	portIndex.devIndex = 1;
	portIndex.portIndex = LAB_PortIndex_Create(IFM_LSP_TYPE, 0, 100);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	statType = GPN_STAT_T_PTN_LSP_P_TYPE;
	i++;
	Pdata.statPtnLspTXFh32 += i;
	Pdata.statPtnLspTXFl32 += (i+1);
	Pdata.statPtnLspTXBh32 += i;
	Pdata.statPtnLspTXBl32 += (i+2);
	Pdata.statPtnLspRXFh32 += i;
	Pdata.statPtnLspRXFl32 += (i+3);
	Pdata.statPtnLspRXBh32 += i;
	Pdata.statPtnLspRXBl32 += (i+4);
	
	gpnSockStatMsgApiTxPtnLspPStatGetRsp(&portIndex,\
		statType, (void *)(&Pdata), sizeof(gpnStatPtnLspPData));

	statType = GPN_STAT_T_PTN_LSP_M_TYPE;
	Mdata.statPtnLspdLDROPRATA = 0x45;
	Mdata.statPtnLspdDELAY = 0x46;
	Mdata.statPtnLspdDELAYCHG = 0x47;
	
	gpnSockStatMsgApiTxPtnLspMStatGetRsp(&portIndex,\
		statType, (void *)(&Mdata), sizeof(gpnStatPtnLspMData));

	portIndex.devIndex = 1;
	portIndex.portIndex = LAB_PortIndex_Create(IFM_LSP_TYPE, 0, 101);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	statType = GPN_STAT_T_PTN_LSP_P_TYPE;

	gpnSockStatMsgApiTxPtnLspPStatGetRsp(&portIndex,\
		statType, (void *)(&Pdata), sizeof(gpnStatPtnLspPData));

	statType = GPN_STAT_T_PTN_LSP_M_TYPE;
	Mdata.statPtnLspdLDROPRATA = 0x48;
	Mdata.statPtnLspdDELAY = 0x49;
	Mdata.statPtnLspdDELAYCHG = 0x4A;
	
	gpnSockStatMsgApiTxPtnLspMStatGetRsp(&portIndex,\
		statType, (void *)(&Mdata), sizeof(gpnStatPtnLspMData));
}

void gpnStatMsgForPtnLspPStatGet(UINT32 para1, UINT32 para2)
{
	UINT32 ReVal;
	optObjOrient portIndex;

	portIndex.devIndex   = 0x00000020;
	portIndex.portIndex  = 0x01000065;
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	ReVal = gpnSockStatMsgApiTxPtnLspPStatGet(&portIndex, GPN_STAT_T_PTN_LSP_P_TYPE);

	printf("%s: get lspStat Data return(%d)\n", __FUNCTION__, ReVal);
}

void gpnStatMsgForPtnEthPStatGet(UINT32 para1, UINT32 para2)
{
	UINT32 ReVal;
	optObjOrient portIndex;

	portIndex.devIndex   = 0x00000020;
	portIndex.portIndex  = 0x01000005;
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	ReVal = gpnSockStatMsgApiTxNsmEthPhyPStatGet(&portIndex, GPN_STAT_T_ETH_MAC_P_TYPE);

	printf("%s: get ethStat Data return(%d)\n", __FUNCTION__, ReVal);
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_STAT_DEBUG_C_ */

