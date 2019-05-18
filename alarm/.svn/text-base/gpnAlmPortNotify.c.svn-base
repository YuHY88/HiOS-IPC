/**********************************************************
* file name: gpnAlmPortNotify.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-12
* function: 
*    define alarm port about message process details
* modify:
*
***********************************************************/

#ifndef _GPN_ALM_PORT_NOTIFY_C_
#define _GPN_ALM_PORT_NOTIFY_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <lib/syslog.h>
#include <lib/log.h>

#include "gpnAlmPortNotify.h"
#include "gpnAlmAlmNotify.h"
#include "gpnAlmDebug.h"

/*log function include*/
#include "gpnLog/gpnLogFuncApi.h"

extern gstAlmTypeWholwInfo *pgstAlmTypeWholwInfo;
extern stEQUAlmProcSpace *pgstAlmPreScanWholwInfo;


/*==================================================*/
/*name :  gpnAlmPNtPeerPortReplace                                                                    */
/*para :                                                                                                               */
/*retu :  void                                                                                                        */
/*desc :  process request replace peer port                                                              */
/*==================================================*/
UINT32 gpnAlmPNtPeerPortReplace(optObjOrient *pOldPeerP, optObjOrient *pNewPeerP)
{
	stAlmPreScanQuen *pAlmPreScanNode;
	stAlmPreScanQuen *pAlmPreScanNodeTmp;
	stAlmPeerNode *pAlmPeerPort;
	optObjOrient *pstProtInfo;

	UINT32 peerScanNodeNum;
	UINT32 slotId;
	//UINT32 slotIdTmp;
	UINT32 hashKey;
	UINT32 hashKeyTmp;

	/* #### step 1/2 : base old peer port, find peer port, and delete frome old quen */
	pAlmPreScanNode = NULL;
	gpnAlmSeekPortIndexToAlmPreScanNode((objLogicDesc *)pOldPeerP, &pAlmPreScanNode);
	if(pAlmPreScanNode != NULL)
	{
		/* get slot hsah key from first(master)-portIndex */
		slotId = PortIndex_GetSlot(pOldPeerP->portIndex);
		hashKey = (pOldPeerP->portIndex)%(pAlmPreScanNode->hashKey);
		
		/* check if notify port in per scan list, compare with peer port */
		peerScanNodeNum = 0;
		pAlmPeerPort = (stAlmPeerNode *)listFirst(&(pAlmPreScanNode->almPeerPortQuen[hashKey]));
		while((pAlmPeerPort != NULL)&&(peerScanNodeNum < pAlmPreScanNode->peerPortNodeNum[hashKey]))
		{
			pstProtInfo = &(pAlmPeerPort->peerPort);
			/* here devIndex should do what modify */
			if( /*(pstProtInfo->devIndex == pPortInfo->devIndex) &&\*/
				(pstProtInfo->portIndex == pOldPeerP->portIndex) &&\
				(pstProtInfo->portIndex3 == pOldPeerP->portIndex3) &&\
				(pstProtInfo->portIndex4 == pOldPeerP->portIndex4) &&\
				(pstProtInfo->portIndex5 == pOldPeerP->portIndex5) &&\
				(pstProtInfo->portIndex6 == pOldPeerP->portIndex6) )
			{
				pAlmPreScanNodeTmp = NULL;
				gpnAlmSeekPortIndexToAlmPreScanNode((objLogicDesc *)pNewPeerP, &pAlmPreScanNodeTmp);
				if(pAlmPreScanNodeTmp != NULL)
				{
					if(GPN_ALM_GEN_OK != gpnAlmRelevCheckIsPeerInPeerPortQuen(\
						pNewPeerP, pAlmPreScanNodeTmp))
					{
						/* when match, delete from old list and add in new list  */
						listDelete(&(pAlmPreScanNode->almPeerPortQuen[hashKey]), (NODE *)pAlmPeerPort);
						pAlmPreScanNode->peerPortNodeNum[hashKey]--;
						/*gpnLog(GPN_LOG_L_ALERT,\
							"%s : delete port(%08x-%08x|%08x|%08x|%08x|%08x) "
							"out almPeerPortQuen(scanType %08x slot %d hash %d).\n",\
							__FUNCTION__,\
							pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
							pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
							pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6,\
							pAlmPreScanNode->almScanType, slotId, hashKey);*/
					}
					else
					{
						gpnLog(GPN_LOG_L_ALERT,\
							"%s : new port(%08x-%08x|%08x|%08x|%08x|%08x) "
							"already in almPeerPortQuen\n",\
							__FUNCTION__,\
							pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
							pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
							pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6);
						
						return GPN_ALM_GEN_ERR;
					}
				}
				else
				{
					gpnLog(GPN_LOG_L_ALERT,\
						"%s : new port(%08x-%08x|%08x|%08x|%08x|%08x) "
						"not found stAlmPreScanQuen\n",\
						__FUNCTION__,\
						pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
						pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
						pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6);
					return GPN_ALM_GEN_ERR;
				}
				
				break;
			}
			peerScanNodeNum++;
			pAlmPeerPort = (stAlmPeerNode *)listNext((NODE *)(pAlmPeerPort));
		}

		/* when delete ,pAlmPreScanNode->peerPortNodeNum[hashKey]--*/
		if(pAlmPeerPort == NULL)
		{
			/* port type support, but not found port in slot hash list */
			gpnLog(GPN_LOG_L_ALERT,\
				"%s : not found port(%08x-%08x|%08x|%08x|%08x|%08x) "
				"in scanType(%08x)'s slot(%d) hash %d pre list\n",\
				__FUNCTION__,\
				pOldPeerP->devIndex, pOldPeerP->portIndex,\
				pOldPeerP->portIndex3, pOldPeerP->portIndex4,\
				pOldPeerP->portIndex5, pOldPeerP->portIndex6,\
				pAlmPreScanNode->almScanType, slotId, hashKey);
			
			GPN_ALM_PNT_PRINT(GPN_ALM_PNT_CUP,\
				"%s : not found port(%08x-%08x|%08x|%08x|%08x|%08x) "
				"in scanType(%08x)'s slot(%d) hash %d pre list\n",\
				__FUNCTION__,\
				pOldPeerP->devIndex, pOldPeerP->portIndex,\
				pOldPeerP->portIndex3, pOldPeerP->portIndex4,\
				pOldPeerP->portIndex5, pOldPeerP->portIndex6,\
				pAlmPreScanNode->almScanType, slotId, hashKey);
			
			return GPN_ALM_GEN_ERR;
		}

		/* #### step 2/2 : base new peer port, add peer port in quen */
		/* renew peer port */
		memcpy(&(pAlmPeerPort->peerPort), pNewPeerP, sizeof(optObjOrient));
		
		//slotIdTmp = PortIndex_GetSlot(pNewPeerP->portIndex);
		hashKeyTmp = (pNewPeerP->portIndex)%(pAlmPreScanNodeTmp->hashKey);
		
		listAdd(&(pAlmPreScanNodeTmp->almPeerPortQuen[hashKeyTmp]), (NODE *)pAlmPeerPort);
		pAlmPreScanNodeTmp->peerPortNodeNum[hashKeyTmp]++;
		
		/*gpnLog(GPN_LOG_L_ALERT,\
			"%s : add new port(%08x-%08x|%08x|%08x|%08x|%08x) "
			"in almPeerPortQuen(scanType %08x slot %d hash %d).\n",\
			__FUNCTION__,\
			pAlmPeerPort->peerPort.devIndex, pAlmPeerPort->peerPort.portIndex,\
			pAlmPeerPort->peerPort.portIndex3, pAlmPeerPort->peerPort.portIndex4,\
			pAlmPeerPort->peerPort.portIndex5, pAlmPeerPort->peerPort.portIndex6,\
			pAlmPreScanNodeTmp->almScanType, slotIdTmp, hashKeyTmp);*/

		return GPN_ALM_GEN_OK;
	}
	else
	{
		gpnLog(GPN_LOG_L_ALERT,\
			"%s : old port(%08x-%08x|%08x|%08x|%08x|%08x) "
			"not found pAlmPreScanNode\n",\
			__FUNCTION__,\
			pOldPeerP->devIndex, pOldPeerP->portIndex,\
			pOldPeerP->portIndex3, pOldPeerP->portIndex4,\
			pOldPeerP->portIndex5, pOldPeerP->portIndex6);
		
		return GPN_ALM_GEN_ERR;
	}
}

/*==================================================*/
/*name :  gpnAlmPNtPeerPortReplace                                                                    */
/*para :                                                                                                               */
/*retu :  void                                                                                                        */
/*desc :  process request replace peer port                                                              */
/*==================================================*/
UINT32 gpnAlmPNtViewPortReplace(objLogicDesc *pLocalPort, objLogicDesc *pNewViewP)
{
	stAlmLocalNode *pAlmLocalPort;
	stAlmValueRecd *palmValueRecd;
	UINT32 i;
	
	/* #### step 1/5 : base local port, find local port node */
	pAlmLocalPort = NULL;
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(pLocalPort, &pAlmLocalPort);
	if(pAlmLocalPort == NULL)
	{
		gpnLog(GPN_LOG_L_ALERT,\
			"%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) "
			"not found pAlmPreScanNode\n",\
			__FUNCTION__,\
			pLocalPort->devIndex,\
			pLocalPort->portIndex,\
			pLocalPort->portIndex3,\
			pLocalPort->portIndex4,\
			pLocalPort->portIndex5,\
			pLocalPort->portIndex6);
		return GPN_ALM_GEN_ERR;
	}

	/* #### step2/5 : check if new view port same with old view port */
	if(memcmp(&(pAlmLocalPort->viewPort), pNewViewP, sizeof(objLogicDesc)) == 0)
	{
		gpnLog(GPN_LOG_L_ALERT,\
			"%s : equ view port(%08x-%08x|%08x|%08x|%08x|%08x)\n",\
			__FUNCTION__,\
			pNewViewP->devIndex,\
			pNewViewP->portIndex,\
			pNewViewP->portIndex3,\
			pNewViewP->portIndex4,\
			pNewViewP->portIndex5,\
			pNewViewP->portIndex6);
		return GPN_ALM_GEN_ERR;
	}
	
	/* #### step3/5 : check new view port not used for same scan type node */
	/* check whole hasl pAlmPreScanNode */
	/* now not do ths */
	
	/* #### step 4/5 : old view port alarm clean, product at new view port */
	for(i=0; i<pAlmLocalPort->pAlmScanPort->almNumInScanPort; i++)
	{
		palmValueRecd = &(pAlmLocalPort->pAlmScanPort->AlmValueBuff[i]);
		if(palmValueRecd->almSubType == GPN_ALM_ARISE)
		{
			/******* add old view port alarm in hist db *******/
			gpnAlmANtHistAlmInsertDS(&(pAlmLocalPort->viewPort), palmValueRecd);

			/******* report old view port alarm clean *******/
			gpnAlmANtDisappReport(&(pAlmLocalPort->viewPort), palmValueRecd);

			/* modify for alarm product */
			palmValueRecd->prodCnt++;
			palmValueRecd->thisTime = (UINT32)pgstAlmPreScanWholwInfo->almTaskTime;
			palmValueRecd->index = pgstAlmPreScanWholwInfo->almGlobalCfg.currAlmIndex++;

			/* modify curr alarm portIndex to new view port */
			memcpy(&(pAlmLocalPort->viewPort), pNewViewP, sizeof(objLogicDesc));
			
			/******* add new view port alarm in db  *******/
			gpnAlmANtCurrAlmInsertDS(&(pAlmLocalPort->viewPort), palmValueRecd);

			/****** report new view port alarm rise ******/
			gpnAlmANtProductReport(pNewViewP, palmValueRecd);
		}
	}
	
	return GPN_ALM_GEN_OK;
}
UINT32 gpnAlmPNtVariPortUserNotify(objLogicDesc *pPortInfo)
{
	UINT32 reVal;
	
	/* call alone reg */
	reVal= gpnAlmPNtFixPortReg(pPortInfo);
	if(reVal != GPN_ALM_GEN_OK)
	{
		return GPN_ALM_GEN_ERR;
	}
	
	/* search for mxml save config */
	/* ??? todo */

	return GPN_ALM_GEN_OK;
}
UINT32 gpnAlmPNtVariPortReg(objLogicDesc *pPortInfo)
{
	return gpnAlmPNtPortReg(pPortInfo, GPN_ALM_VARI_MODE);
}
UINT32 gpnAlmPNtVariPortUnReg(objLogicDesc *pPortInfo)
{
	stAlmPreScanIndex *pAlmPreScanIndex;
	stAlmScanPortInfo *pAlmScanPort;

	/* check if port add in perQuen */
	pAlmPreScanIndex = NULL;
	gpnAlmSeekLocalPortIndexToAlmScanPortIndex(pPortInfo, &pAlmPreScanIndex);
	if(pAlmPreScanIndex == NULL)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP,\
			"%s : when unReg(%08x-%08x|%08x|%08x|%08x|%08x), not found!\n",\
			__FUNCTION__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);
		
		gpnLog(GPN_LOG_L_ERR,\
			"%s : when unReg(%08x-%08x|%08x|%08x|%08x|%08x), not found!\n",\
			__FUNCTION__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);

		return GPN_ALM_GEN_ERR;
	}
	
	pAlmScanPort = pAlmPreScanIndex->pAlmLocalPort->pAlmScanPort;
	
	pAlmScanPort->addQuenMode = GPN_ALM_VARI_MODE;		//add by lipf 2018/12/10
	if(pAlmScanPort->addQuenMode != GPN_ALM_VARI_MODE)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP,\
			"%s : when unReg(%08x-%08x|%08x|%08x|%08x|%08x), addQuenMode(%d) error!\n",\
			__FUNCTION__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6,\
			pAlmScanPort->addQuenMode);
		
		gpnLog(GPN_LOG_L_ERR,\
			"%s : when unReg(%08x-%08x|%08x|%08x|%08x|%08x), addQuenMode(%d) error!\n",\
			__FUNCTION__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6,\
			pAlmScanPort->addQuenMode);

		return GPN_ALM_GEN_ERR;
	}

	if(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_ENABLE)
	{
		/* delete from validScan */
		gpnAlmRelevChgAlmScanPortToValidScanQuen(pAlmPreScanIndex->pAlmLocalPort, GPN_ALM_GEN_DISABLE);
	}
	
	if(pAlmScanPort->preQuenValid == GPN_ALM_GEN_ENABLE)
	{
		/* delete from perScan */
		gpnAlmRelevChgAlmScanPortToPreScanQuen(pAlmPreScanIndex, GPN_ALM_GEN_DISABLE);
	}

	/* free mem */
	free(pAlmPreScanIndex);
	
	return GPN_ALM_GEN_OK;
}
UINT32 gpnAlmPNtAllPortUnReg(UINT32 devIndex)
{
	stAlmPreScanIndex *pAlmPreScanIndex;
	stAlmPreScanQuen *pAlmPreScanTypeNode;
	/*stAlmPreScanQuen *pAlmPreScanNode;*/
	stAlmLocalNode *pAlmLocalPort;
	UINT32 preScanTypeNum;
	UINT32 preScanPortNum;
	UINT32 slotId;
	UINT32 i;
	
	/* first get slotId */
	slotId = DeviceIndex_GetSlot(devIndex);
	
	/* delete valid check, should delete start from remotest */
	/* ??? todo */
	
	/* delete perQuen and validQuen */
	preScanTypeNum = 0;
	pAlmPreScanTypeNode = (stAlmPreScanQuen *)listFirst(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].almPreScanTpQuen));
	while((pAlmPreScanTypeNode != NULL)&&(preScanTypeNum < pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].quenNodeNum))
	{
		for(i=0;i<GPN_ALM_PRESCAN_HASH;i++)
		{
			preScanPortNum = 0;
			pAlmPreScanIndex = (stAlmPreScanIndex *)listFirst(&(pAlmPreScanTypeNode->almPreScanQuen[i]));
			while((pAlmPreScanIndex != NULL)&&(preScanPortNum<pAlmPreScanTypeNode->preScanNodeNum[i]))
			{
				pAlmLocalPort = pAlmPreScanIndex->pAlmLocalPort;
				/* check for devIndex , here should check level */
				/* ??? todo */
				if(pAlmLocalPort->localPort.devIndex == devIndex)
				{
					if(pAlmLocalPort->pAlmScanPort->scanQuenValid == GPN_ALM_GEN_ENABLE)
					{
						/* delete from validScan */
						gpnAlmRelevChgAlmScanPortToValidScanQuen(pAlmLocalPort, GPN_ALM_GEN_DISABLE);
					}
					if(pAlmLocalPort->pAlmScanPort->preQuenValid == GPN_ALM_GEN_ENABLE)
					{
						/* delete from perScan */
						gpnAlmRelevChgAlmScanPortToPreScanQuen(pAlmPreScanIndex, GPN_ALM_GEN_DISABLE);
					}
					
					/* free mem */
					free(pAlmPreScanIndex);
				
				}
				preScanPortNum++;
				pAlmPreScanIndex = (stAlmPreScanIndex *)listNext((NODE *)(pAlmPreScanIndex));
			}
		}
		
		preScanTypeNum++;
		pAlmPreScanTypeNode = (stAlmPreScanQuen *)listNext((NODE *)(pAlmPreScanTypeNode));
	}
	
	return GPN_ALM_GEN_YES;
}

UINT32 debugGpnAlmPNtVariPreScanPrint(void)
{
	return debugGpnAlmPNtPreScanPrint(GPN_ALM_VARI_MODE);
}

UINT32 gpnAlmPNtFixPortUserNotify(UINT32 devIndex, UINT32 devPortTpNum, stAlmPortObjCL *pPortObjCL)
{
	objLogicDesc portInfo;
	UINT32 slotId;
	UINT32 reVal;
	UINT32 i;
	UINT32 j;

	/* get slot for make portIndex */
	slotId = DeviceIndex_GetSlot(devIndex);

	/* check if portType was supported */
	for(i=0; i<devPortTpNum; i++)
	{
		for(j=0; j<pPortObjCL[i].portNum; j++)
		{
			/* make SMP portIndex */
			portInfo.devIndex = devIndex;
			portInfo.portIndex = IFM_SMP_PID_COMP(pPortObjCL[i].portType, slotId, j+1);
			portInfo.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			portInfo.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			portInfo.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			portInfo.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

			/* call alone reg */
			reVal = gpnAlmPNtFixPortReg(&portInfo);
			if(reVal != GPN_ALM_GEN_OK)
			{
				return GPN_ALM_GEN_ERR;
			}

			/* search for mxml save config */
			/* ??? todo */
		}
	}

	return GPN_ALM_GEN_OK;
}
UINT32 gpnAlmPNtFixPortReg(objLogicDesc *pPortInfo)
{
	return gpnAlmPNtPortReg(pPortInfo, GPN_ALM_FIX_MODE);
}
UINT32 gpnAlmPNtPortReg(objLogicDesc *pPortInfo, UINT32 mode)
{
	stAlmScanTypeDef *pAlmScanType;
	stAlmPreScanIndex *pAlmPreScanIndex;
	stAlmLocalNode *pAlmLocalPort;
	stAlmPeerNode *pAlmPeerPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmValueRecd *pAlmValueRecd;
	stAlmShakeRecd *pAlmShakeRecd;
	UINT32 portType;
	UINT32 reVal;
	UINT32 i;
	
	/* find portType Vs  scanType */
	portType = PortIndex_GetType(pPortInfo->portIndex);
	if(IFM_VUNI_TYPE == portType)
	{
		zlog_debug(ALARM_DBG_REGISTER, "%s[%d] : not accept vuni register\n", __func__, __LINE__);
		return GPN_ALM_GEN_ERR;
	}
	
	pAlmScanType = NULL;
	gpnAlmSeekPortTypeToAlmScanType(portType, &pAlmScanType);
	if(pAlmScanType == NULL)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP,\
			"%s : portType(%08x) was not supported "
			"when reg (%08x-%08x|%08x|%08x|%08x|%08x)!\n",\
			__FUNCTION__, portType,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);

		zlog_debug(ALARM_DBG_REGISTER, "%s : portType(%08x) was not supported "
			"when reg (%08x-%08x|%08x|%08x|%08x|%08x)!\n",\
			__FUNCTION__, portType,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);
		
		gpnLog(GPN_LOG_L_ERR,\
			"%s : portType(%08x) was not supported "
			"when reg (%08x-%08x|%08x|%08x|%08x|%08x)!\n",\
			__FUNCTION__, portType,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);

		return GPN_ALM_GEN_ERR;
	}

	/* check if dev state is insert */
	reVal = gpnAlmPNtDevIsInsert(pPortInfo->devIndex);
	if(reVal != GPN_ALM_GEN_OK)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP,\
			"%s : devIndex(%08x) was not insert "
			"when reg (%08x-%08x|%08x|%08x|%08x|%08x)!\n",\
			__FUNCTION__, pPortInfo->devIndex,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);

		zlog_err ("%s : devIndex(%08x) was not insert "
			"when reg (%08x-%08x|%08x|%08x|%08x|%08x)!\n",\
			__FUNCTION__, pPortInfo->devIndex,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);
		
		gpnLog(GPN_LOG_L_ERR,\
			"%s : devIndex(%08x) was not insert "
			"when reg (%08x-%08x|%08x|%08x|%08x|%08x)!\n",\
			__FUNCTION__, pPortInfo->devIndex,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);

		return GPN_ALM_GEN_ERR;
	}
	
	/* check if port already in perQuen */
	pAlmLocalPort = NULL;
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(pPortInfo, &pAlmLocalPort);
	if(pAlmLocalPort != NULL)
	{
		gpnLog(GPN_LOG_L_ERR,\
			"%s : port already add "
			"when reg (%08x-%08x|%08x|%08x|%08x|%08x)!\n",\
			__FUNCTION__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);

		zlog_err("%s[%d] : port already add (%08x-%08x|%08x|%08x|%08x|%08x)!\n",\
			__FUNCTION__, __LINE__, \
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);

		return GPN_ALM_GEN_ERR;
	}
	
	/* calculat port need mem space */
	i = sizeof(stAlmPreScanIndex) +\
		sizeof(stAlmLocalNode) +\
		sizeof(stAlmPeerNode) +\
		sizeof(stAlmScanPortInfo) +\
		sizeof(stAlmValueRecd)*(pAlmScanType->almSubTpNumInScanType) +\
		sizeof(stAlmShakeRecd)*(pAlmScanType->almSubTpNumInScanType);

	GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP,\
		"stAlmPreScanIndex %d unit   1\n"
		"stAlmLocalNode    %d unit   1\n"
		"stAlmPeerNode     %d unit   1\n"
		"stAlmScanPortInfo %d unit   1\n"
		"stAlmValueRecd    %d unit %3d\n"
		"stAlmShakeRecd    %d unit %3d\n",\
		sizeof(stAlmPreScanIndex),\
		sizeof(stAlmLocalNode),\
		sizeof(stAlmPeerNode),\
		sizeof(stAlmScanPortInfo),\
		sizeof(stAlmValueRecd), pAlmScanType->almSubTpNumInScanType,\
		sizeof(stAlmShakeRecd), pAlmScanType->almSubTpNumInScanType);

	/* malloc and distribution mem */
	pAlmPreScanIndex = (stAlmPreScanIndex *)malloc(i);
	if(pAlmPreScanIndex == NULL)
	{
		gpnLog(GPN_LOG_L_ERR,\
			"%s : malloc(%d) error when reg (%08x-%08x|%08x|%08x|%08x|%08x)!\n",\
			__FUNCTION__, i,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);

		return GPN_ALM_GEN_ERR;
	}
	memset(pAlmPreScanIndex, 0, i);
	pAlmLocalPort = (stAlmLocalNode *)((UINT8 *)(pAlmPreScanIndex) + sizeof(stAlmPreScanIndex));
	pAlmPeerPort = (stAlmPeerNode *)((UINT8 *)(pAlmLocalPort) + sizeof(stAlmLocalNode));				
	pAlmScanPort = (stAlmScanPortInfo *)((UINT8 *)(pAlmPeerPort) + sizeof(stAlmPeerNode));
	pAlmValueRecd = (stAlmValueRecd *)((UINT8 *)(pAlmScanPort) + sizeof(stAlmScanPortInfo));
 	pAlmShakeRecd = (stAlmShakeRecd *)((UINT8 *)(pAlmValueRecd) + sizeof(stAlmValueRecd)*(pAlmScanType->almSubTpNumInScanType));

	/* port mem init */
	gpnAlmScanPortInit(pPortInfo, pAlmScanType, pAlmPreScanIndex,\
		pAlmLocalPort, pAlmPeerPort, pAlmScanPort,\
		pAlmValueRecd, pAlmShakeRecd);
	
	/* perQuen add mode is fix or vari */
	if( (mode != GPN_ALM_FIX_MODE) &&\
		(mode != GPN_ALM_VARI_MODE) )
	{
		gpnLog(GPN_LOG_L_ERR,\
			"%s : reg mode(%d) error when reg (%08x-%08x|%08x|%08x|%08x|%08x)!\n",\
			__FUNCTION__, mode,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);

		zlog_err("%s[%d] : regist error (%08x-%08x|%08x|%08x|%08x|%08x)!\n",\
			__FUNCTION__, __LINE__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);

		/* not return */
		mode = GPN_ALM_VARI_MODE;
	}
	pAlmScanPort->addQuenMode = mode;
	
	/* add in perQuen */
	gpnAlmRelevChgAlmScanPortToPreScanQuen(pAlmPreScanIndex, GPN_ALM_GEN_ENABLE);
	
	/* add in validQuen */
	gpnAlmRelevChgAlmScanPortToValidScanQuen(pAlmLocalPort, GPN_ALM_GEN_ENABLE);

	return GPN_ALM_GEN_OK;
}
UINT32 debugGpnAlmPNtFixPreScanPrint(void)
{
	return debugGpnAlmPNtPreScanPrint(GPN_ALM_FIX_MODE);
}
UINT32 debugGpnAlmPNtPreScanPrint(UINT32 mode)
{
	stAlmPreScanQuen *pAlmPreScanTypeNode;
	stAlmPreScanIndex *pAlmPreScanIndex;
	stAlmLocalNode *pAlmLocalPort;
	stAlmPeerNode *pAlmPeerPort;
	stAlmScanPortInfo *pAlmScanPort;
	UINT32 preScanTypeNum;
	UINT32 preScanPortNum;
	UINT32 Num;
	UINT32 i;
	UINT32 j;
	
	Num = 0;

	for(i=0; i<(EQU_SLOT_MAX_ID + 1); i++)
	{
		preScanPortNum = 0;
		preScanTypeNum = 0;
		pAlmPreScanTypeNode = (stAlmPreScanQuen *)listFirst(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].almPreScanTpQuen));
		while((pAlmPreScanTypeNode != NULL)&&(preScanTypeNum < pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].quenNodeNum))
		{
			for(j=0; j<GPN_ALM_PRESCAN_HASH; j++)
			{	
				preScanPortNum += pAlmPreScanTypeNode->preScanNodeNum[j];
			}
			
			preScanTypeNum++;
			pAlmPreScanTypeNode = (stAlmPreScanQuen *)listNext((NODE *)(pAlmPreScanTypeNode));
		}
		printf("slot %d has %d monitors, detials as follows\n", i ,preScanPortNum);
		printf("---------------------------------------------------------------------\n");
		
		preScanTypeNum = 0;
		pAlmPreScanTypeNode = (stAlmPreScanQuen *)listFirst(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].almPreScanTpQuen));
		while((pAlmPreScanTypeNode != NULL)&&(preScanTypeNum < pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].quenNodeNum))
		{
			for(j=0;j<GPN_ALM_PRESCAN_HASH;j++)
			{
				preScanPortNum = 0;
				pAlmPreScanIndex = (stAlmPreScanIndex *)listFirst(&(pAlmPreScanTypeNode->almPreScanQuen[j]));
				while((pAlmPreScanIndex != NULL)&&(preScanPortNum<pAlmPreScanTypeNode->preScanNodeNum[j]))
				{
					pAlmLocalPort = pAlmPreScanIndex->pAlmLocalPort;
					pAlmPeerPort = pAlmLocalPort->pPeerNode;
					pAlmScanPort = pAlmLocalPort->pAlmScanPort;

					if(pAlmScanPort->addQuenMode == mode)
					{
						printf("Num(%4d) level(%d) scanType(%08x) V(%d)P(%d) (%s)\n\r"
							"L(%08x-%08x|%08x|%08x|%08x|%08x)\n\r"
							"V(%08x-%08x|%08x|%08x|%08x|%08x)\n\r"
							"P(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
							++Num,\
							DeviceIndex_GetLevel(pAlmLocalPort->localPort.devIndex),\
							pAlmPreScanTypeNode->almScanType,\
							pAlmScanPort->scanQuenValid,\
							pAlmScanPort->preQuenValid,\
							pAlmPreScanTypeNode->almScanTypeNode->almScanTypeName,\
							pAlmLocalPort->localPort.devIndex,\
							pAlmLocalPort->localPort.portIndex,\
							pAlmLocalPort->localPort.portIndex3,\
							pAlmLocalPort->localPort.portIndex4,\
							pAlmLocalPort->localPort.portIndex5,\
							pAlmLocalPort->localPort.portIndex6,\
							pAlmLocalPort->viewPort.devIndex,\
							pAlmLocalPort->viewPort.portIndex,\
							pAlmLocalPort->viewPort.portIndex3,\
							pAlmLocalPort->viewPort.portIndex4,\
							pAlmLocalPort->viewPort.portIndex5,\
							pAlmLocalPort->viewPort.portIndex6,\
							pAlmPeerPort->peerPort.devIndex,\
							pAlmPeerPort->peerPort.portIndex,\
							pAlmPeerPort->peerPort.portIndex3,\
							pAlmPeerPort->peerPort.portIndex4,\
							pAlmPeerPort->peerPort.portIndex5,\
							pAlmPeerPort->peerPort.portIndex6);
					}
					
					preScanPortNum++;
					pAlmPreScanIndex = (stAlmPreScanIndex *)listNext((NODE *)(pAlmPreScanIndex));
				}
			}

			preScanTypeNum++;
			pAlmPreScanTypeNode = (stAlmPreScanQuen *)listNext((NODE *)(pAlmPreScanTypeNode));
		}

	}
	return GPN_ALM_GEN_YES;
}

#if 0
UINT32 gpnAlmFixMonitorsSpacePerdistribution(UINT32 devIndex,UINT32 devPortTpNum,stAlmPortObjCL *pPortObjCL)
{
	/*stAlmPreScanQuen *pPfmPreScanNode;*/
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stAlmPreScanIndex *pAlmPreScanIndex;
	stRemDevAlmProcNode *pRemDevAlmProcNode;
	stAlmLocalNode *pAlmLocalPort;
	stAlmPeerNode *pAlmPeerPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmValueRecd *pAlmValueRecd;
	stAlmShakeRecd *pAlmShakeRecd;
	objLogicDesc *portIndexBuff;
	objLogicDesc *upperPortIndexBuff;

	UINT32 portIndex;
	UINT32 slotId;
	UINT32 level;
	UINT32 portType;
	
	UINT32 almScanPortNum;
	UINT32 almValueNum;

	UINT32 sysPortTypeNum;
	UINT32 remEevNum;
	UINT32 i;
	UINT32 j;

	/* get slot from devIndex */
	slotId = DeviceIndex_GetSlot(devIndex);
	level = DeviceIndex_GetLevel(devIndex);
	/*校验slotId的合法性???*/
	
	almScanPortNum = 0;
	almValueNum = 0;

	pAlmPreScanIndex = NULL;
	pAlmLocalPort = NULL;
	pAlmPeerPort = NULL;
	pAlmScanPort = NULL;
	pAlmValueRecd = NULL;
	pAlmShakeRecd = NULL;
	
	for(i=0;i<devPortTpNum;i++)
	{		
		sysPortTypeNum = 0;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));
		while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
		{
			if(pPortObjCL[i].portType == pPortTpToScanTp->portType)
			{
				/*端口数量和告警数量累计*/
				almScanPortNum += pPortObjCL[i].portNum;
				/*告警数量的空间是倍增与端口数量的*/
				almValueNum += (pPortTpToScanTp->almSubTpNumInPortType * (pPortObjCL[i].portNum));
				break;
			}
			sysPortTypeNum++;
			pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
		}
		if((pPortTpToScanTp == NULL)||(sysPortTypeNum >= pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
		{
			GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : villed port type %08x\n\r",\
				__FUNCTION__, pPortObjCL[i].portType);
			return GPN_ALM_GEN_ERR;
		}
	}

	i = almScanPortNum*sizeof(stAlmPreScanIndex) +\
		almScanPortNum*sizeof(stAlmLocalNode) +\
		almScanPortNum*sizeof(stAlmPeerNode) +\
		almScanPortNum*sizeof(stAlmScanPortInfo) +\
		almValueNum*sizeof(stAlmValueRecd) +\
		almValueNum*sizeof(stAlmShakeRecd);
	
	GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : malloc %d byte,almScanPortNum %d\n",\
		__FUNCTION__, i,almScanPortNum);
	GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, 	"stAlmPreScanIndex %d unit %d\n"
											"stAlmLocalNode    %d unit %d\n"
											"stAlmPeerNode     %d unit %d\n"
											"stAlmScanPortInfo %d unit %d\n"
											"stAlmValueRecd    %d unit %d\n"
											"stAlmShakeRecd    %d unit %d\n",\
									sizeof(stAlmPreScanIndex), almScanPortNum,\
									sizeof(stAlmLocalNode), almScanPortNum,\
									sizeof(stAlmPeerNode), almScanPortNum,\
									sizeof(stAlmScanPortInfo), almScanPortNum,\
									sizeof(stAlmValueRecd), almValueNum,\
									sizeof(stAlmShakeRecd), almValueNum);

	/*局远端差异化处理过程*/
	if(level == 1)
	{
		if( (pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].pCenterPreScanPortNodeMemSpace == NULL) &&
			(pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].slotDevSta == GPN_ALM_DEV_STA_INSERT))
		{
			/*局端有效槽位*/
			pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].pCenterPreScanPortNodeMemSpace = (stAlmPreScanIndex *)malloc(i);
			memset((UINT8 *)pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].pCenterPreScanPortNodeMemSpace,0,i);
			
			/*内存空间分配指示信息*/
			pAlmPreScanIndex = pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].pCenterPreScanPortNodeMemSpace;
			pAlmLocalPort = (stAlmLocalNode *)(pAlmPreScanIndex + almScanPortNum);
			pAlmPeerPort = (stAlmPeerNode *)(pAlmLocalPort + almScanPortNum);
			pAlmScanPort = (stAlmScanPortInfo *)(pAlmPeerPort + almScanPortNum);
			pAlmValueRecd = (stAlmValueRecd *)(pAlmScanPort + almScanPortNum);
			pAlmShakeRecd = (stAlmShakeRecd *)(pAlmValueRecd + almValueNum);
		}
		else
		{
			GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : slot %d double init OR not release\n\r",\
				__FUNCTION__, slotId);
			return GPN_ALM_GEN_ERR;
		}
	}
	else if((level == 2) || (level == 3))
	{
		remEevNum = 0;
		pRemDevAlmProcNode = (stRemDevAlmProcNode *)listFirst(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].RemDevAlmPorcNodeQuen));
		while((pRemDevAlmProcNode != NULL)&&(remEevNum < pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].RemDevAlmPorcNodeNum))
		{
			if(pRemDevAlmProcNode->devIndex == devIndex)
			{
				if( (pRemDevAlmProcNode->pRemPreScanPortNodeMemSpace == NULL) &&
					(pRemDevAlmProcNode->remotDevSta == GPN_ALM_DEV_STA_INSERT))
				{
					pRemDevAlmProcNode->pRemPreScanPortNodeMemSpace = (void *)malloc(i);
					memset((UINT8 *)pRemDevAlmProcNode->pRemPreScanPortNodeMemSpace,0,i);

					/*内存空间分配指示信息*/
					pAlmPreScanIndex = (stAlmPreScanIndex *)(pRemDevAlmProcNode->pRemPreScanPortNodeMemSpace);
					pAlmLocalPort = (stAlmLocalNode *)(pAlmPreScanIndex + almScanPortNum);
					pAlmPeerPort = (stAlmPeerNode *)(pAlmLocalPort + almScanPortNum);
					pAlmScanPort = (stAlmScanPortInfo *)(pAlmPeerPort + almScanPortNum);
					pAlmValueRecd = (stAlmValueRecd *)(pAlmScanPort + almScanPortNum);
					pAlmShakeRecd = (stAlmShakeRecd *)(pAlmValueRecd + almValueNum);
				}
				else
				{
					GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : remot dev %08x double init OR not release\n\r",\
						__FUNCTION__, devIndex);
					return GPN_ALM_GEN_ERR;
				}
				break;
			}
			
			remEevNum++;
			pRemDevAlmProcNode = (stRemDevAlmProcNode *)listNext((NODE *)(pRemDevAlmProcNode));
		}
		if((pRemDevAlmProcNode == NULL)||(remEevNum >= pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].RemDevAlmPorcNodeNum))
		{
			GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : remot dev %08x do not be created\n\r",\
				__FUNCTION__, devIndex);
			return GPN_ALM_GEN_ERR;
		}
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
	
	/*组织数据结构*/
	i = 2*almScanPortNum*sizeof(objLogicDesc);
	portIndexBuff = (objLogicDesc *)malloc(i);
	upperPortIndexBuff = (objLogicDesc *)((UINT8 *)portIndexBuff + i/2);
	gpnAlmMakePortIndex(devIndex,devPortTpNum,pPortObjCL,portIndexBuff,upperPortIndexBuff);

	/*重新找到类型*/
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		for(i=0,j=0;((i<almScanPortNum)&&(j<almScanPortNum));i++)
		{
			portIndex = portIndexBuff[i].portIndex;
			portType = PortIndex_GetType(portIndex);
			if(portType == pPortTpToScanTp->portType)
			{
				/*只起测试的作用*/
				j++;
				/*扫描端口初始化*/
				gpnAlmScanPortInit(&(portIndexBuff[i]),\
					pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode->almScanTypeNode, pAlmPreScanIndex,\
					pAlmLocalPort, pAlmPeerPort, pAlmScanPort,\
					pAlmValueRecd, pAlmShakeRecd);
				
				/*设置加入扫描队列的方式:FIX模式加入*/
				pAlmScanPort->addQuenMode = GPN_ALM_FIX_MODE;
				/*加入预扫描*/
				gpnAlmRelevChgAlmScanPortToPreScanQuen(pAlmPreScanIndex, GPN_ALM_GEN_ENABLE);
				/*加入有效扫描*/
				
				gpnAlmRelevChgAlmScanPortToValidScanQuen(pAlmLocalPort, GPN_ALM_GEN_ENABLE);

				pAlmPreScanIndex++;
				pAlmLocalPort++;
				pAlmPeerPort++;
				pAlmScanPort++;
				pAlmValueRecd += pPortTpToScanTp->almSubTpNumInPortType;
				pAlmShakeRecd += pPortTpToScanTp->almSubTpNumInPortType;
				
			}
		}
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	

	for(i=0;i<almScanPortNum;i++)
	{
		/*复用及联带关系生成:复用关系生成(集中加入时首先按类型，因此导致不安端口BUFF中的顺序加入)*/
		gpnAlmUppePortNodeSet(portIndexBuff[i],upperPortIndexBuff[i]);
	}

	free((void *)portIndexBuff);
	return GPN_ALM_GEN_OK;
}
UINT32 gpnAlmVariMonitorSpacePerdistribution(objLogicDesc *pPortInfo)
{
	stAlmPreScanQuen *pAlmPreScanNode;
	stVLQNode *pVLQNode;
	stAlmPreScanIndex *pAlmPreScanIndex;
	stAlmLocalNode *pAlmLocalPort;
	stAlmPeerNode *pAlmPeerPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmValueRecd *pAlmValueRecd;
	stAlmShakeRecd *pAlmShakeRecd;

	UINT32 variNodeSize;
	UINT32 retVal;
	
	GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : portIndex(%08x-%08x|%08x|%08x|%08x|%08x)\n",\
		__FUNCTION__,\
		pPortInfo->devIndex,\
		pPortInfo->portIndex,\
		pPortInfo->portIndex3,\
		pPortInfo->portIndex4,\
		pPortInfo->portIndex5,\
		pPortInfo->portIndex6);

	retVal = GPN_ALM_GEN_YES;

	/* check if local port already in mon list */
	pAlmLocalPort = NULL;
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(pPortInfo, &pAlmLocalPort);
	if(pAlmLocalPort == NULL)
	{
		pAlmPreScanNode = NULL;
		gpnAlmSeekPortIndexToAlmPreScanNode(pPortInfo, &pAlmPreScanNode);
		if(pAlmPreScanNode != NULL)
		{
			if(pAlmPreScanNode->pVariPTList == NULL)
			{
				/* init VLQ memrey space for perScanQuen */
				variNodeSize = 	sizeof(stAlmPreScanIndex) +\
								sizeof(stAlmLocalNode) +\
								sizeof(stAlmPeerNode) +\
								sizeof(stAlmScanPortInfo) +\
								sizeof(stAlmValueRecd)*(pAlmPreScanNode->almScanTypeNode->almSubTpNumInScanType) +\
								sizeof(stAlmShakeRecd)*(pAlmPreScanNode->almScanTypeNode->almSubTpNumInScanType);
				pAlmPreScanNode->pVariPTList =\
				funcGPNVLQInit(10,50,variNodeSize,0);
			}
			
			pVLQNode = funcGPNVLQNodeGet(pAlmPreScanNode->pVariPTList,0);
			if(pVLQNode != NULL)
			{
				/* get node memrey for this use */
				pAlmPreScanIndex = (stAlmPreScanIndex *)(pVLQNode->objSt);
				pAlmLocalPort = (stAlmLocalNode *)((UINT8 *)(pAlmPreScanIndex) + sizeof(stAlmPreScanIndex));
				pAlmPeerPort = (stAlmPeerNode *)((UINT8 *)(pAlmLocalPort) + sizeof(stAlmLocalNode));				
				pAlmScanPort = (stAlmScanPortInfo *)((UINT8 *)(pAlmPeerPort) + sizeof(stAlmPeerNode));
				pAlmValueRecd = (stAlmValueRecd *)((UINT8 *)(pAlmScanPort) + sizeof(stAlmScanPortInfo));
				pAlmShakeRecd = (stAlmShakeRecd *)((UINT8 *)(pAlmValueRecd) +\
									sizeof(stAlmValueRecd)*(pAlmPreScanNode->almScanTypeNode->almSubTpNumInScanType));
				/* init memrey spcae?/
				gpnAlmScanPortInit(pPortInfo,\
					pAlmPreScanNode->almScanTypeNode, pAlmPreScanIndex,\
					pAlmLocalPort, pAlmPeerPort, pAlmScanPort,\
					pAlmValueRecd, pAlmShakeRecd);
				
				/* fix this port add in perScanQuen as  VARI */
				pAlmScanPort->addQuenMode = GPN_ALM_VARI_MODE;
				/* add localport in perScanQuen */
				gpnAlmRelevChgAlmScanPortToPreScanQuen(pAlmPreScanIndex, GPN_ALM_GEN_ENABLE);
				/* add localport in valid quen */
				gpnAlmRelevChgAlmScanPortToValidScanQuen(pAlmLocalPort, GPN_ALM_GEN_ENABLE);
			}
			else
			{
				retVal = GPN_ALM_GEN_NO;
			}
		}
		else
		{
			retVal = GPN_ALM_GEN_NO;
		}
	}
	else
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) already in mon list\n",\
			__FUNCTION__,\
			pPortInfo->devIndex,\
			pPortInfo->portIndex,\
			pPortInfo->portIndex3,\
			pPortInfo->portIndex4,\
			pPortInfo->portIndex5,\
			pPortInfo->portIndex6);
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) already in mon list\n",\
			__FUNCTION__,\
			pPortInfo->devIndex,\
			pPortInfo->portIndex,\
			pPortInfo->portIndex3,\
			pPortInfo->portIndex4,\
			pPortInfo->portIndex5,\
			pPortInfo->portIndex6);
		retVal = GPN_ALM_GEN_NO;
	}
	return retVal;

}
UINT32 gpnAlmVariMonitorSpaceFree(objLogicDesc *pPortInfo)
{
	stAlmPreScanIndex *pAlmPreScanIndex;
	stAlmPreScanQuen *pAlmPreScanNode;
	stAlmScanPortInfo *pAlmScanPort;
	UINT32 retVal;

	GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : portIndex(%08x-%08x|%08x|%08x|%08x|%08x)\n",\
		__FUNCTION__,\
		pPortInfo->devIndex,\
		pPortInfo->portIndex,\
		pPortInfo->portIndex3,\
		pPortInfo->portIndex4,\
		pPortInfo->portIndex5,\
		pPortInfo->portIndex6);

	retVal = GPN_ALM_GEN_YES;

	pAlmPreScanIndex = NULL;
	gpnAlmSeekLocalPortIndexToAlmScanPortIndex(pPortInfo, &pAlmPreScanIndex);
	if(pAlmPreScanIndex != NULL)
	{
		pAlmScanPort = pAlmPreScanIndex->pAlmLocalPort->pAlmScanPort;
		/*节点以VARI的方式加入预/有效扫描队列*/
		if(pAlmScanPort->addQuenMode == GPN_ALM_VARI_MODE)
		{
			pAlmPreScanNode = NULL;
			gpnAlmSeekPortIndexToAlmPreScanNode(pPortInfo, &pAlmPreScanNode);
			if(pAlmPreScanNode != NULL)
			{
				if(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_ENABLE)
				{
					/*退出有效扫描*/
					gpnAlmRelevChgAlmScanPortToValidScanQuen(pAlmPreScanIndex->pAlmLocalPort, GPN_ALM_GEN_DISABLE);
				}
				if(pAlmScanPort->preQuenValid == GPN_ALM_GEN_ENABLE)
				{
					/*退出预扫描*/
					gpnAlmRelevChgAlmScanPortToPreScanQuen(pAlmPreScanIndex, GPN_ALM_GEN_DISABLE);
				}
				
				funcGPNVLQNodeReclaim(pAlmPreScanNode->pVariPTList, (void *)pAlmPreScanIndex);
			}
			else
			{
				retVal = GPN_ALM_GEN_NO;
			}
		}
		else
		{
			GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : portIndex(%08x-%08x|%08x|%08x|%08x|%08x) mode is fix\n",\
				__FUNCTION__,\
				pPortInfo->devIndex,\
				pPortInfo->portIndex,\
				pPortInfo->portIndex3,\
				pPortInfo->portIndex4,\
				pPortInfo->portIndex5,\
				pPortInfo->portIndex6);
			retVal = GPN_ALM_GEN_NO;
		}
	}
	else
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : portIndex(%08x-%08x|%08x|%08x|%08x|%08x) not found pAlmPreScanIndex\n",\
			__FUNCTION__,\
			pPortInfo->devIndex,\
			pPortInfo->portIndex,\
			pPortInfo->portIndex3,\
			pPortInfo->portIndex4,\
			pPortInfo->portIndex5,\
			pPortInfo->portIndex6);
		retVal = GPN_ALM_GEN_NO;
	}
	return retVal;
}

#endif
UINT32 gpnAlmPNtDevStaNotify(UINT32 devIndex, objLogicDesc *pgodPortIndex, UINT32 sta)
{
	stRemDevAlmProcNode *pRemDevAlmProcNode;
	UINT32 remDevNum;
	UINT32 slotId;
	UINT32 level;
	UINT32 i;
	
	slotId = (UINT32)DeviceIndex_GetSlot(devIndex);
	level = (UINT32)DeviceIndex_GetLevel(devIndex);

	if( (sta != GPN_ALM_DEV_STA_INSERT) &&
		(sta != GPN_ALM_DEV_STA_PULL) )
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : dev(%08x) lev(%d) chang error state(%d)\n\r",\
			__FUNCTION__, devIndex, level, sta);
		
		return GPN_ALM_GEN_ERR;
	}
	
	if(level == 1)
	{
		/* level 1 means local position, init NULL,  then fix, then NULL, must be like this */
		if(pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].slotDevSta != sta)
		{
			GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP,\
				"%s : slot %d chang sta from %d to %d\n\r",\
				__FUNCTION__, slotId,\
				pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].slotDevSta, sta);

			pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].devIndex = devIndex;
			pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].slotDevSta = sta;
			
			if(sta == GPN_ALM_DEV_STA_PULL)
			{
				/* delete local dev influence on remote devices ??? */
				gpnAlmPNtAllPortUnReg(devIndex);
			}

			return GPN_ALM_GEN_OK;
		}
		else
		{
			return GPN_ALM_GEN_ERR;
		}
	}
	else if((level == 2) || (level == 3))
	{
		/* 
			remote valid check, should not have same devIndex, check like this :
			check if RemDevAlmPorcNodeQuen's devIndex already be used
		*/
		remDevNum = 0;
		pRemDevAlmProcNode = (stRemDevAlmProcNode *)listFirst(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].RemDevAlmPorcNodeQuen));
		while((pRemDevAlmProcNode != NULL)&&(remDevNum < pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].RemDevAlmPorcNodeNum))
		{
			if(pRemDevAlmProcNode->devIndex == devIndex)
			{
				/* find remote dev, opt should be "PULL" */
				
				GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP,\
					"%s : remot dev %08x slot %d lev %d chang sta from %d to %d\n\r",\
					__FUNCTION__, devIndex, slotId,\
					level, pRemDevAlmProcNode->remotDevSta, sta);
				
				pRemDevAlmProcNode->remotDevSta = sta;
				if(pRemDevAlmProcNode->remotDevSta == GPN_ALM_DEV_STA_PULL)
				{
					/* delete remote dev influence on remote's remote devices ??? */
					gpnAlmPNtAllPortUnReg(devIndex);
					
					/* delete frome RemDevAlmPorcNodeQuen */
					listDelete(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].RemDevAlmPorcNodeQuen),(NODE*)(pRemDevAlmProcNode));
					pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].RemDevAlmPorcNodeNum--;
					
					free((void *)(pRemDevAlmProcNode->pRemDevSelfSpace));

					return GPN_ALM_GEN_OK;
				}
				else
				{
					return GPN_ALM_GEN_ERR;
				}
			}

			remDevNum++;
			pRemDevAlmProcNode = (stRemDevAlmProcNode *)listNext((NODE *)pRemDevAlmProcNode);
		}

		/* remote dev not found in RemDevAlmPorcNodeQuen */
		if(sta == GPN_ALM_DEV_STA_INSERT)
		{
			i = sizeof(stRemDevAlmProcNode);
			pRemDevAlmProcNode = (stRemDevAlmProcNode *)malloc(i);
			memset((UINT8 *)pRemDevAlmProcNode,0,i);
			
			pRemDevAlmProcNode->devIndex = devIndex;

			if(pgodPortIndex != NULL)
			{
				pRemDevAlmProcNode->godDevIndex = pgodPortIndex->devIndex;
				pRemDevAlmProcNode->godPortIndex.devIndex= pgodPortIndex->devIndex;
				pRemDevAlmProcNode->godPortIndex.portIndex = pgodPortIndex->portIndex;
				pRemDevAlmProcNode->godPortIndex.portIndex3 = pgodPortIndex->portIndex3;
				pRemDevAlmProcNode->godPortIndex.portIndex4 = pgodPortIndex->portIndex4;
				pRemDevAlmProcNode->godPortIndex.portIndex5 = pgodPortIndex->portIndex5;
			}
			
			pRemDevAlmProcNode->pRemDevSelfSpace = pRemDevAlmProcNode;
			pRemDevAlmProcNode->remotDevSta = GPN_ALM_DEV_STA_INSERT;

			/* remote dev add in MGMT, level save in devIndex */
			listAdd(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].RemDevAlmPorcNodeQuen),(NODE *)(pRemDevAlmProcNode));
			pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].RemDevAlmPorcNodeNum++;

			return GPN_ALM_GEN_OK;
		}
		else
		{
			return GPN_ALM_GEN_ERR;
		}
		
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}

UINT32 gpnAlmPNtDevStaGet(UINT32 devIndex, objLogicDesc *pgodPortIndex, UINT32 *devState)
{
	stRemDevAlmProcNode *pRemDevAlmProcNode;
	UINT32 remDevNum;
	UINT32 slotId;
	UINT32 level;
	
	slotId = (UINT32)DeviceIndex_GetSlot(devIndex);
	level = (UINT32)DeviceIndex_GetLevel(devIndex);
	
	if(level == 1)
	{
		*devState = pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].slotDevSta;

		pgodPortIndex->devIndex = GPN_ILLEGAL_DEVICE_INDEX;
		pgodPortIndex->portIndex = GPN_ILLEGAL_PORT_INDEX;
		pgodPortIndex->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
		pgodPortIndex->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
		pgodPortIndex->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
		pgodPortIndex->portIndex6 = GPN_ILLEGAL_PORT_INDEX;

		return GPN_ALM_GEN_OK;
	}
	else if((level == 2) || (level == 3))
	{
		remDevNum = 0;
		pRemDevAlmProcNode = (stRemDevAlmProcNode *)listFirst(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].RemDevAlmPorcNodeQuen));
		while((pRemDevAlmProcNode != NULL)&&(remDevNum < pgstAlmPreScanWholwInfo->unitAlmProcSpace[slotId].RemDevAlmPorcNodeNum))
		{
			if(pRemDevAlmProcNode->devIndex == devIndex)
			{				
				*devState = pRemDevAlmProcNode->remotDevSta;

				pgodPortIndex->devIndex = pRemDevAlmProcNode->godPortIndex.devIndex;
				pgodPortIndex->portIndex = pRemDevAlmProcNode->godPortIndex.portIndex;
				pgodPortIndex->portIndex3 = pRemDevAlmProcNode->godPortIndex.portIndex3;
				pgodPortIndex->portIndex4 = pRemDevAlmProcNode->godPortIndex.portIndex4;
				pgodPortIndex->portIndex5 = pRemDevAlmProcNode->godPortIndex.portIndex5;
				pgodPortIndex->portIndex6 = pRemDevAlmProcNode->godPortIndex.portIndex6;

				return GPN_ALM_GEN_OK;
			}

			remDevNum++;
			pRemDevAlmProcNode = (stRemDevAlmProcNode *)listNext((NODE *)pRemDevAlmProcNode);
		}
	}

	return GPN_ALM_GEN_ERR;
}
UINT32 gpnAlmPNtDevIsInsert(UINT32 devIndex)
{
#if 0	
	objLogicDesc godPortIndex;
	UINT32 devState;
	UINT32 reVal;

	reVal = gpnAlmPNtDevStaGet(devIndex, &godPortIndex, &devState);
	if( (reVal == GPN_ALM_GEN_OK) &&\
		(devState == GPN_ALM_DEV_STA_INSERT) )
	{
		return GPN_ALM_GEN_OK;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
#endif

	return GPN_ALM_GEN_OK;	//modify for ipran by lipf, 2018/4/23
}
UINT32 debugGpnAlmPNtDevPrint()
{
	stRemDevAlmProcNode *pRemDevAlmProcNode;
	objLogicDesc *pgodPortIndex;
	UINT32 remDevNum;
	UINT32 devIndex;
	UINT32 slotId;
	UINT32 level;
	UINT32 i;
	
	
	for(i=0; i<(EQU_SLOT_MAX_ID + 1); i++)
	{
		/* local dev state */
		printf("slot(%2d) level(1) dev(%08x) sta(%d)\n\r",\
			i,\
			pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].devIndex,\
			pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].slotDevSta);

		/* remote dev state */
		remDevNum = 0;
		pRemDevAlmProcNode = (stRemDevAlmProcNode *)listFirst(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].RemDevAlmPorcNodeQuen));
		while((pRemDevAlmProcNode != NULL)&&(remDevNum < pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].RemDevAlmPorcNodeNum))
		{
			devIndex = pRemDevAlmProcNode->devIndex;
			pgodPortIndex = &(pRemDevAlmProcNode->godPortIndex);
			slotId = (UINT32)DeviceIndex_GetSlot(devIndex);
			level = (UINT32)DeviceIndex_GetLevel(devIndex);

			printf("slot(%2d) level(%d) dev(%08x) sta(%d) gadPort(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
				slotId, level, devIndex,\
				pRemDevAlmProcNode->remotDevSta,\
				pgodPortIndex->devIndex, pgodPortIndex->portIndex,\
				pgodPortIndex->portIndex3, pgodPortIndex->portIndex4,\
				pgodPortIndex->portIndex5, pgodPortIndex->portIndex6);

			remDevNum++;
			pRemDevAlmProcNode = (stRemDevAlmProcNode *)listNext((NODE *)pRemDevAlmProcNode);
		}
	}
	printf("****************************************************\n\r");

	return GPN_ALM_GEN_OK;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif/* _GPN_ALM_PORT_NOTIFY_C_ */

