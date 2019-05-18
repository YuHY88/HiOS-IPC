/**********************************************************
* file name: gpnAlmDataStructApi.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-09-08
* function: 
*    define alarm inner data struct API
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_DATA_STRUCT_API_C_
#define _GPN_ALM_DATA_STRUCT_API_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <lib/syslog.h>
#include <lib/log.h>

#include "gpnAlmScan.h"
#include "gpnAlmDataStructApi.h"
#include "gpnAlmTypeStruct.h"
#include "gpnAlmCfg.h"
#include "gpnAlmDataSheet.h"
#include "gpnAlmDebug.h"


extern gstAlmTypeWholwInfo *pgstAlmTypeWholwInfo;
extern stEQUAlmProcSpace *pgstAlmPreScanWholwInfo;

static UINT32 gpnAlmDSApiPortAlmSubTpNodeGet(UINT32 almSubType, stAlmScanTypeDef *pAlmScanType, stAlmSTCharacterDef **ppAlmSubTpStr)
{
	stAlmSTCharacterDef *pAlmSubTpStr;
	UINT32 almSubTpNum;

	almSubTpNum = 0;
	pAlmSubTpStr = (stAlmSTCharacterDef *)listFirst(&(pAlmScanType->almScanSubTCharaQuen));
	while((pAlmSubTpStr != NULL) && (almSubTpNum < pAlmScanType->almSubTpNumInScanType))
	{
		if(pAlmSubTpStr->almSubType == almSubType)
		{
			*ppAlmSubTpStr = pAlmSubTpStr;
			return GPN_ALM_GEN_YES;
		}
		
		almSubTpNum++;
		pAlmSubTpStr = (stAlmSTCharacterDef *)listNext((NODE *)(pAlmSubTpStr));
	}

	*ppAlmSubTpStr = NULL;
	return GPN_ALM_GEN_ERR;
}
static UINT32 gpnAlmDSApiPortAlmSubTpNodeGetFirst(stAlmScanTypeDef *pAlmScanType, stAlmSTCharacterDef **ppAlmSubTpStr)
{
	stAlmSTCharacterDef *pAlmSubTpStr;

	pAlmSubTpStr = (stAlmSTCharacterDef *)listFirst(&(pAlmScanType->almScanSubTCharaQuen));
	if(pAlmSubTpStr != NULL)
	{
		*ppAlmSubTpStr = pAlmSubTpStr;
		return GPN_ALM_GEN_YES;
	}	
	else
	{
		*ppAlmSubTpStr = NULL;
		return GPN_ALM_GEN_ERR;
	}
}
static UINT32 gpnAlmDSApiPortAlmSubTpNodeGetNext(UINT32 almSubType,
	stAlmScanTypeDef *pAlmScanType, stAlmSTCharacterDef **ppNextSubTpStr)
{
	stAlmSTCharacterDef *pNextSubTpStr;
	UINT32 almSubTpNum;

	almSubTpNum = 0;
	pNextSubTpStr = (stAlmSTCharacterDef *)listFirst(&(pAlmScanType->almScanSubTCharaQuen));
	while((pNextSubTpStr != NULL) && (almSubTpNum < pAlmScanType->almSubTpNumInScanType))
	{
		if((pNextSubTpStr->almSubType == almSubType) && (GPN_ALM_TYPE_ETH_SFP_MIS == almSubType))
		{
			break;
		}
		
		almSubTpNum++;
		pNextSubTpStr = (stAlmSTCharacterDef *)listNext((NODE *)(pNextSubTpStr));
	}

	if((pNextSubTpStr == NULL) || (almSubTpNum >= pAlmScanType->almSubTpNumInScanType))
	{
		*ppNextSubTpStr = NULL;
		return GPN_ALM_GEN_ERR;
	}
	
	pNextSubTpStr = (stAlmSTCharacterDef *)listNext((NODE *)(pNextSubTpStr));
	if(pNextSubTpStr != NULL)
	{
		*ppNextSubTpStr = pNextSubTpStr;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		*ppNextSubTpStr = NULL;
		return GPN_ALM_GEN_ERR;
	}
}
static UINT32 gpnAlmDSApiAlmSubTpNodeGet(UINT32 almSubType, stAlmSTCharacterDef **ppAlmSubTpStr)
{
	stAlmSubTpNode *pAlmSubTpNode;
	UINT32 almSubTpNum;

	almSubTpNum = 0;
	pAlmSubTpNode = (stAlmSubTpNode *)listFirst(&(pgstAlmTypeWholwInfo->almSubTypeQuen));
	while((pAlmSubTpNode != NULL) && (almSubTpNum < pgstAlmTypeWholwInfo->almSubTypeNum))
	{
		if(pAlmSubTpNode->pAlmSubTpStr->almSubType == almSubType)
		{
			*ppAlmSubTpStr = pAlmSubTpNode->pAlmSubTpStr;
			return GPN_ALM_GEN_YES;
		}
		
		almSubTpNum++;
		pAlmSubTpNode = (stAlmSubTpNode *)listNext((NODE *)(pAlmSubTpNode));
	}

	*ppAlmSubTpStr = NULL;
	return GPN_ALM_GEN_ERR;
}
static UINT32 gpnAlmDSApiAlmSubTpNodeGetFirst(stAlmSTCharacterDef **ppAlmSubTpStr)
{
	stAlmSubTpNode *pAlmSubTpNode;

	pAlmSubTpNode = (stAlmSubTpNode *)listFirst(&(pgstAlmTypeWholwInfo->almSubTypeQuen));
	if(pAlmSubTpNode != NULL)
	{
		*ppAlmSubTpStr = pAlmSubTpNode->pAlmSubTpStr;
		return GPN_ALM_GEN_YES;
	}	
	else
	{
		*ppAlmSubTpStr = NULL;
		return GPN_ALM_GEN_ERR;
	}
}
static UINT32 gpnAlmDSApiAlmSubTpNodeGetNext(UINT32 almSubType, stAlmSTCharacterDef **ppNextSubTpStr)
{
	stAlmSubTpNode *pAlmSubTpNode;
	UINT32 almSubTpNum;

	almSubTpNum = 0;
	pAlmSubTpNode = (stAlmSubTpNode *)listFirst(&(pgstAlmTypeWholwInfo->almSubTypeQuen));
	while((pAlmSubTpNode != NULL) && (almSubTpNum < pgstAlmTypeWholwInfo->almSubTypeNum))
	{
		if(pAlmSubTpNode->pAlmSubTpStr->almSubType == almSubType)
		{
			break;
		}
		
		almSubTpNum++;
		pAlmSubTpNode = (stAlmSubTpNode *)listNext((NODE *)(pAlmSubTpNode));
	}

	if((pAlmSubTpNode == NULL) || (almSubTpNum >= pgstAlmTypeWholwInfo->almSubTypeNum))
	{
		*ppNextSubTpStr = NULL;
		return GPN_ALM_GEN_ERR;
	}

	pAlmSubTpNode = (stAlmSubTpNode *)listNext((NODE *)(pAlmSubTpNode));	
	if(pAlmSubTpNode != NULL)
	{
		*ppNextSubTpStr = pAlmSubTpNode->pAlmSubTpStr;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		*ppNextSubTpStr = NULL;
		return GPN_ALM_GEN_ERR;
	}
}

static UINT32 gpnAlmDSApiLocalPortNodeGet(objLogicDesc *pPortInfo, stAlmLocalNode **ppAlmLocalPort)
{
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stAlmPreScanQuen *pAlmPreScanNode;
	stAlmPreScanIndex *pAlmPreScanIndex;
	objLogicDesc *pstProtInfo;
	UINT32 preScanNodeNum;
	UINT32 sysPortTypeNum;
	UINT32 portType;
	UINT32 hashKey;
	UINT32 slotId;
	
	/* first portIndex include objLogicDesc's port type */
	portType = PortIndex_GetType(pPortInfo->portIndex);
	
	/* check if port type be support */
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		if(pPortTpToScanTp->portType == portType)
		{
			/* use slot do hash opt, in order to increase check speed */
			slotId = PortIndex_GetSlot(pPortInfo->portIndex);

			pAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
			hashKey = pPortInfo->portIndex%(pAlmPreScanNode->hashKey);
			
			/* compare with local port index */
			preScanNodeNum = 0;
			pAlmPreScanIndex = (stAlmPreScanIndex *)listFirst(&(pAlmPreScanNode->almPreScanQuen[hashKey]));
			while((pAlmPreScanIndex != NULL) && (preScanNodeNum < pAlmPreScanNode->preScanNodeNum[hashKey]))
			{
				pstProtInfo = &(pAlmPreScanIndex->pAlmLocalPort->localPort);
				if( (pstProtInfo->devIndex == pPortInfo->devIndex) &&\
					(pstProtInfo->portIndex == pPortInfo->portIndex) &&\
					(pstProtInfo->portIndex3 == pPortInfo->portIndex3) &&\
					(pstProtInfo->portIndex4 == pPortInfo->portIndex4) &&\
					(pstProtInfo->portIndex5 == pPortInfo->portIndex5) &&\
					(pstProtInfo->portIndex6 == pPortInfo->portIndex6) )
				{
					/* get locaPortNode base portIndex */
					*ppAlmLocalPort = pAlmPreScanIndex->pAlmLocalPort;
					
					return GPN_ALM_DST_OK;
				}
				
				preScanNodeNum++;
				pAlmPreScanIndex = (stAlmPreScanIndex *)listNext((NODE *)(pAlmPreScanIndex));
			}
			
			/* port type support, but not found port in slot hash list */
			*ppAlmLocalPort = NULL;
			GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP,\
				"%s : not found port(%08x|%08x|%08x|%08x|%08x|%08x) "
				"in scanType(%08x)'s slot(%d) pre list\n",\
				__FUNCTION__,\
				pPortInfo->devIndex, pPortInfo->portIndex,\
				pPortInfo->portIndex3, pPortInfo->portIndex4,\
				pPortInfo->portIndex5, pPortInfo->portIndex6,\
				pPortTpToScanTp->ppAlmPreScanNode->pAlmPreScanNode[0].almScanType,\
				slotId);

			return GPN_ALM_DST_ERR;
		}
		
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	
	/* port type not support */
	*ppAlmLocalPort = NULL;
	GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP,\
		"%s : portType(%08x) not support for port(%08x|%08x|%08x|%08x|%08x|%08x)\n",\
		__FUNCTION__,\
		portType,\
		pPortInfo->devIndex, pPortInfo->portIndex,\
		pPortInfo->portIndex3, pPortInfo->portIndex4,\
		pPortInfo->portIndex5, pPortInfo->portIndex6);
		
	return GPN_ALM_DST_ERR;
}
static UINT32 gpnAlmDSApiLocalPortNodeGetFirst(stAlmLocalNode **ppAlmLocalPort)
{
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stAlmPreScanQuen *pAlmPreScanNode;
	stAlmPreScanIndex *pAlmPreScanIndex;
	UINT32 sysPortTypeNum;
	UINT32 hashKey;
	UINT32 slotId;
	
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen)); 
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		for(slotId=0; slotId<=EQU_SLOT_MAX_ID; slotId++)
		{
			pAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
			for(hashKey=0; hashKey<GPN_ALM_PRESCAN_HASH; hashKey++)
			{
				/* compare with local port index */
				pAlmPreScanIndex = (stAlmPreScanIndex *)listFirst(&(pAlmPreScanNode->almPreScanQuen[hashKey]));
				 
				if(pAlmPreScanIndex != NULL)
				{
					/* get first locaPortNode */
					*ppAlmLocalPort = pAlmPreScanIndex->pAlmLocalPort;
					
					return GPN_ALM_DST_OK;
				}
			}
		}
		
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	
	GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : not port found\n\r", __FUNCTION__);
		
	return GPN_ALM_DST_ERR;
}
static UINT32 gpnAlmDSApiLocalPortNodeGetNext(objLogicDesc *pPortInfo, stAlmLocalNode **ppNextLocalPort)
{
	stPortTpToAlmScanTp *pPortTpToScanTp = NULL;
	stAlmPreScanQuen *pAlmPreScanNode = NULL;
	stAlmPreScanIndex *pAlmPreScanIndex = NULL;
	objLogicDesc *pstProtInfo = NULL;
	UINT32 preScanNodeNum = 0;
	UINT32 sysPortTypeNum = 0;
	UINT32 portType = 0;
	UINT32 portType_temp = 0;
	UINT32 hashKey = 0;
	UINT32 slotId = 0;
	
	/* first portIndex include objLogicDesc's port type */
	portType = PortIndex_GetType(pPortInfo->portIndex);
	
	/* check if port type be support */
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		if(pPortTpToScanTp->portType == portType)
		{
			/* use slot do hash opt, in order to increase check speed */
			slotId = PortIndex_GetSlot(pPortInfo->portIndex);

			pAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
			hashKey = pPortInfo->portIndex%(pAlmPreScanNode->hashKey);
			
			/* compare with local port index */
			preScanNodeNum = 0;
			pAlmPreScanIndex = (stAlmPreScanIndex *)listFirst(&(pAlmPreScanNode->almPreScanQuen[hashKey]));
			while((pAlmPreScanIndex != NULL) && (preScanNodeNum < pAlmPreScanNode->preScanNodeNum[hashKey]))
			{
				pstProtInfo = &(pAlmPreScanIndex->pAlmLocalPort->localPort);				
				if( (pstProtInfo->devIndex == pPortInfo->devIndex) &&\
					(pstProtInfo->portIndex == pPortInfo->portIndex) &&\
					(pstProtInfo->portIndex3 == pPortInfo->portIndex3) &&\
					(pstProtInfo->portIndex4 == pPortInfo->portIndex4) &&\
					(pstProtInfo->portIndex5 == pPortInfo->portIndex5) &&\
					(pstProtInfo->portIndex6 == pPortInfo->portIndex6) )
    				{
    					/* get locaPortNode base portIndex */    					
    					break;
    				}
				
				preScanNodeNum++;
				pAlmPreScanIndex = (stAlmPreScanIndex *)listNext((NODE *)(pAlmPreScanIndex));
			}

			if((pAlmPreScanIndex == NULL) || (preScanNodeNum >= pAlmPreScanNode->preScanNodeNum[hashKey]))
			{
				/* port type support, but not found port in slot hash list */
				*ppNextLocalPort = NULL;
				zlog_debug(ALARM_DBG_GET_NEXT, "%s[%d] : not found port(%08x|%08x|%08x|%08x|%08x|%08x) "
					"in scanType(%08x)'s slot(%d) pre list\n",\
					__FUNCTION__, __LINE__,\
					pPortInfo->devIndex, pPortInfo->portIndex,\
					pPortInfo->portIndex3, pPortInfo->portIndex4,\
					pPortInfo->portIndex5, pPortInfo->portIndex6,\
					pPortTpToScanTp->ppAlmPreScanNode->pAlmPreScanNode[0].almScanType,\
					slotId);

				return GPN_ALM_DST_ERR;
			}

			break;
		}
		
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	
	if((pPortTpToScanTp == NULL) || (sysPortTypeNum >= pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		/* port type not support */
		*ppNextLocalPort = NULL;
		zlog_debug(ALARM_DBG_GET_NEXT, "%s[%d] : portType(%08x) not support for port(%08x|%08x|%08x|%08x|%08x|%08x)\n",\
			__FUNCTION__, __LINE__,\
			portType,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);

		return GPN_ALM_DST_ERR;
	}

	/* find next stAlmLocalNode */
	*ppNextLocalPort = NULL;
	pAlmPreScanIndex = (stAlmPreScanIndex *)listNext((NODE *)(pAlmPreScanIndex));

	if(pAlmPreScanIndex != NULL)
	{		
		while(pAlmPreScanIndex != NULL)
		{
			/* get next locaPortNode */
			portType_temp = PortIndex_GetType(pAlmPreScanIndex->pAlmLocalPort->localPort.portIndex);
			if(pPortTpToScanTp->portType == portType_temp)
			{
				*ppNextLocalPort = pAlmPreScanIndex->pAlmLocalPort;
				
				return GPN_ALM_DST_OK;
			}
			pAlmPreScanIndex = (stAlmPreScanIndex *)listNext((NODE *)(pAlmPreScanIndex));
		}
	}	
	
	/*if(pAlmPreScanIndex != NULL)
	{		
		*ppNextLocalPort = pAlmPreScanIndex->pAlmLocalPort;
	}
	else*/
	{
		hashKey++;
		while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
		{
			for( ; slotId<=EQU_SLOT_MAX_ID; slotId++)
			{
				pAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
				for( ; hashKey<GPN_ALM_PRESCAN_HASH; hashKey++)
				{
					pAlmPreScanIndex = (stAlmPreScanIndex *)listFirst(&(pAlmPreScanNode->almPreScanQuen[hashKey]));
					while(pAlmPreScanIndex != NULL)
					{
						/* get next locaPortNode */
						portType_temp = PortIndex_GetType(pAlmPreScanIndex->pAlmLocalPort->localPort.portIndex);
						if(pPortTpToScanTp->portType == portType_temp)
						{
							*ppNextLocalPort = pAlmPreScanIndex->pAlmLocalPort;
							
							return GPN_ALM_DST_OK;
						}
						pAlmPreScanIndex = (stAlmPreScanIndex *)listNext((NODE *)(pAlmPreScanIndex));
					}
				}
				
				hashKey = 0;
			}

			slotId = 0;
			sysPortTypeNum++;
			pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
		}
	}
	return GPN_ALM_DST_ERR;
}


/*********************************************************/
/*                 gpn alarm snmp/cli control api part                               */
/*********************************************************/
UINT32 gpnAlmDSApiRiseDelayCfg(UINT32 opt)
{
	return gpnAlmCfgRiseDelay(opt);
}
UINT32 gpnAlmDSApiRiseDelayGet(UINT32 *opt)
{
	return gpnAlmGetRiseDelay(opt);
}
UINT32 gpnAlmDSApiCleanDelayCfg(UINT32 opt)
{
	return gpnAlmCfgCleanDelay(opt);
}
UINT32 gpnAlmDSApiCleanDelayGet(UINT32 *opt)
{
	return gpnAlmGetCleanDelay(opt);
}
UINT32 gpnAlmDSApiBuzzerEnCfg(UINT32 opt)
{
	return gpnAlmCfgBuzzerEn(opt);
}
UINT32 gpnAlmDSApiBuzzerEnGet(UINT32 *opt)
{
	return gpnAlmGetBuzzerEn(opt);
}
UINT32 gpnAlmDSApiBuzzerClrCfg(UINT32 opt)
{
	return gpnAlmCfgBuzzerClr(opt);
}
UINT32 gpnAlmDSApiBuzzerThreCfg(UINT32 opt)
{
	return gpnAlmCfgBuzzerThre(opt);
}
UINT32 gpnAlmDSApiBuzzerThreGet(UINT32 *opt)
{
	return gpnAlmGetBuzzerThre(opt);
}
UINT32 gpnAlmDSApiRestModeCfg(UINT32 opt)
{
	return gpnAlmCfgRestMode(opt);
}
UINT32 gpnAlmDSApiRestModeGet(UINT32 *opt)
{
	return gpnAlmGetRestMode(opt);
}
UINT32 gpnAlmDSApiReverModeCfg(UINT32 opt)
{
	return gpnAlmCfgReverMode(opt);
}
UINT32 gpnAlmDSApiReverModeGet(UINT32 *opt)
{
	return gpnAlmGetReverMode(opt);
}
UINT32 gpnAlmDSApiCurrAlmDBCycCfg(UINT32 opt)
{
	return gpnAlmCfgCurrAlmDBCyc(opt);
}
UINT32 gpnAlmDSApiCurrAlmDBCycGet(UINT32 *opt)
{
	return gpnAlmGetCurrAlmDBCyc(opt);
}
UINT32 gpnAlmDSApiHistAlmDBCycCfg(UINT32 opt)
{
	return gpnAlmCfgHistAlmDBCyc(opt);
}
UINT32 gpnAlmDSApiHistAlmDBCycGet(UINT32 *opt)
{
	return gpnAlmGetHistAlmDBCyc(opt);
}
UINT32 gpnAlmDSApiEventDBCycCfg(UINT32 opt)
{
	return gpnAlmCfgEventDBCyc(opt);
}
UINT32 gpnAlmDSApiEventDBCycGet(UINT32 *opt)
{
	return gpnAlmGetEventDBCyc(opt);
}
UINT32 gpnAlmDSApiCurrDBSizeGet(UINT32 *opt)
{
	return gpnAlmGetCurrDBSize(opt);
}
UINT32 gpnAlmDSApiHistDBSizeGet(UINT32 *opt)
{
	return gpnAlmGetHistDBSize(opt);
}
UINT32 gpnAlmDSApiEventDBSizeGet(UINT32 *opt)
{
	return gpnAlmGetEventDBSize(opt);
}

UINT32 gpnAlmDSApiPortMonStateGet(objLogicDesc *plocalPIndex, UINT32 *enState)
{
	stAlmLocalNode *pAlmLocalPort;

	/* assert */
	
	/* find almScanPort base localPortIndex */
	pAlmLocalPort = NULL;
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(plocalPIndex, &pAlmLocalPort);
	if(pAlmLocalPort == NULL)
	{
		return GPN_ALM_DST_ERR;
	}

	/* port mon state */
	if(pAlmLocalPort->pAlmScanPort->scanQuenValid == GPN_ALM_DST_ENABLE)
	{
		*enState = GPN_ALM_DST_ENABLE;
	}
	else
	{
		*enState = GPN_ALM_DST_DISABLE;
	}
	
	return GPN_ALM_DST_OK;
}

UINT32 gpnAlmDSApiPortMonStateGetNext(objLogicDesc *plocalPIndex, objLogicDesc *pnextPIndex, UINT32 *enState)
{
	stAlmLocalNode *pNextLocalPort;

	/* assert */

	/* first GET-NEXT check */
	if(plocalPIndex->portIndex == GPN_ILLEGAL_PORT_INDEX)
	{
		/* find first almScanPort */
		pNextLocalPort = NULL;
		gpnAlmDSApiLocalPortNodeGetFirst(&pNextLocalPort);
		if(pNextLocalPort == NULL)
		{
			return GPN_ALM_DST_ERR;
		}

		/* next is first */
		memcpy(pnextPIndex,  &(pNextLocalPort->localPort), sizeof(objLogicDesc));
		*enState = pNextLocalPort->pAlmScanPort->scanQuenValid;
	}
	else
	{
		pNextLocalPort = NULL;
		gpnAlmDSApiLocalPortNodeGetNext(plocalPIndex, &pNextLocalPort);
		if(pNextLocalPort != NULL)
		{
			memcpy(pnextPIndex,  &(pNextLocalPort->localPort), sizeof(objLogicDesc));
			*enState = pNextLocalPort->pAlmScanPort->scanQuenValid;
		}
		else
		{
			return GPN_ALM_DST_ERR;
		}
	}

	return GPN_ALM_DST_OK;
}

UINT32 gpnAlmDSApiAlmAttributeGet(UINT32 almSubType, stDSAlmAttri *almAttri)
{
	stAlmSTCharacterDef *pAlmSubTpStr;

	/* assert */
	
	/* find almScanPort base localPortIndex */
	pAlmSubTpStr = NULL;
	gpnAlmDSApiAlmSubTpNodeGet(almSubType, &pAlmSubTpStr);
	if(pAlmSubTpStr == NULL)
	{
		return GPN_ALM_DST_ERR;
	}

	/* port mon state */
	almAttri->bitMark = 0;
	almAttri->rank = pAlmSubTpStr->almRank;
	almAttri->mask = pAlmSubTpStr->isScreen;
	almAttri->filt = pAlmSubTpStr->isFilted;
	almAttri->rept = pAlmSubTpStr->isReport;
	almAttri->recd = pAlmSubTpStr->isRecord;
	almAttri->revs = GPN_ALM_DST_NO;
	
	return GPN_ALM_DST_OK;
}
UINT32 gpnAlmDSApiAlmAttributeGetNext(UINT32 almSubType, UINT32 *nextSubType, stDSAlmAttri *almAttri)
{
	stAlmSTCharacterDef *pNextSubTpStr;

	/* assert */
	
	/* find almScanPort base localPortIndex */
	pNextSubTpStr = NULL;
	if(almSubType == GPN_ALM_TYPE_INVALID)
	{
		gpnAlmDSApiAlmSubTpNodeGetFirst(&pNextSubTpStr);
	}
	else
	{
		gpnAlmDSApiAlmSubTpNodeGetNext(almSubType, &pNextSubTpStr);
	}
	if(pNextSubTpStr == NULL)
	{
		return GPN_ALM_DST_ERR;
	}

	*nextSubType = pNextSubTpStr->almSubType;
	/* almType attribute */
	almAttri->bitMark = 0;
	almAttri->rank = pNextSubTpStr->almRank;
	almAttri->mask = pNextSubTpStr->isScreen;
	almAttri->filt = pNextSubTpStr->isFilted;
	almAttri->rept = pNextSubTpStr->isReport;
	almAttri->recd = pNextSubTpStr->isRecord;
	almAttri->revs = GPN_ALM_DST_NO;
	
	return GPN_ALM_DST_OK;
}

static UINT32 gpnAlmDSApiPortAlmAttriCheckOut(stAlmLocalNode *pAlmLocalPort, stAlmSTCharacterDef *pAlmSubTpStr, stDSAlmAttri *almAttri)
{
	stAlmScanPortInfo *pAlmScanPort;
	UINT32 page;
	UINT32 bMake;

	/* assert */

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
	bMake = pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP;

	/* init bitMark as 0 */
	almAttri->bitMark = 0;

	if(pAlmScanPort->isRankBasePort[page] & bMake)
	{
		almAttri->bitMark |= ((1U << GPN_ALM_DST_ATTRI_BIT_RANK) << 16);
		almAttri->rank = pAlmScanPort->AlmValueBuff[pAlmSubTpStr->devOrderInScanTp].portAlmRank;
	}
	else
	{
		almAttri->rank = pAlmSubTpStr->almRank;
	}

	if(pAlmScanPort->isScreBasePort[page] & bMake)
	{
		almAttri->bitMark |= ((1U << GPN_ALM_DST_ATTRI_BIT_MASK) << 16);
		/* 0 means screen ; 1 means not screen (default) */
		if(pAlmScanPort->iScreAlmInfo[page] & bMake)
		{
			almAttri->mask = GPN_ALM_DST_DISABLE;
		}
		else
		{
			almAttri->mask = GPN_ALM_DST_ENABLE;
		}
	}
	else
	{
		almAttri->mask = pAlmSubTpStr->isScreen;
	}

	if(pAlmScanPort->isFiltBasePort[page] & bMake)
	{
		almAttri->bitMark |= ((1U << GPN_ALM_DST_ATTRI_BIT_FILT) << 16);
		/* 0 means filt ; 1 means no filt(default) */
		if(pAlmScanPort->iFiltAlmInfo[page] & bMake)
		{
			almAttri->filt = GPN_ALM_DST_DISABLE;
		}
		else
		{
			almAttri->filt = GPN_ALM_DST_ENABLE;
		}
	}
	else
	{
		almAttri->filt = pAlmSubTpStr->isFilted;
	}

	if(pAlmScanPort->isReptBasePort[page] & bMake)
	{
		almAttri->bitMark |= ((1U << GPN_ALM_DST_ATTRI_BIT_REPT) << 16);
		/* 0 means not report ; 1 means report(default) */
		if(pAlmScanPort->iReptAlmInfo[page] & bMake)
		{
			almAttri->rept = GPN_ALM_DST_ENABLE;
		}
		else
		{
			almAttri->rept = GPN_ALM_DST_DISABLE;
		}
	}
	else
	{
		almAttri->rept = pAlmSubTpStr->isReport;
	}

	if(pAlmScanPort->isReckBasePort[page] & bMake)
	{
		almAttri->bitMark |= ((1U << GPN_ALM_DST_ATTRI_BIT_RECD) << 16);
		/* 0 means not record ; 1 means record(default) */
		if(pAlmScanPort->iReckAlmInfo[page] & bMake)
		{
			almAttri->recd= GPN_ALM_DST_ENABLE;
		}
		else
		{
			almAttri->recd = GPN_ALM_DST_DISABLE;
		}
	}
	else
	{
		almAttri->recd = pAlmSubTpStr->isRecord;
	}

	almAttri->bitMark |= ((1U << GPN_ALM_DST_ATTRI_BIT_REVS) << 16);
	/* 0 means not reverse(default) ; 1 means reverse */
	if(pAlmScanPort->iInveAlmInfo[page] & bMake)
	{
		almAttri->revs = GPN_ALM_DST_ENABLE;
	}
	else
	{
		almAttri->revs = GPN_ALM_DST_DISABLE;
	}
	
	return GPN_ALM_DST_OK;
}
UINT32 gpnAlmDSApiPortTypeToAlmScanType(UINT32 portType, stAlmScanTypeDef **ppAlmScanType)
{
	UINT32 sysPortTypeNum;
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stPTpVsAlmSTpRelation *pPTpVsSTpRelation;
	
	sysPortTypeNum = 0;
	pPTpVsSTpRelation = &(pgstAlmPreScanWholwInfo->PTpVsSTpRelation);
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pPTpVsSTpRelation->PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pPTpVsSTpRelation->sysSupPortTpNum))
	{
		if(portType == pPortTpToScanTp->portType)
		{
			*ppAlmScanType = pPortTpToScanTp->ppAlmPreScanNode[0].pAlmPreScanNode->almScanTypeNode;
			break;
		}
		
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	if((pPortTpToScanTp == NULL)||(sysPortTypeNum >= pPTpVsSTpRelation->sysSupPortTpNum))
	{
		*ppAlmScanType = NULL;
		return GPN_ALM_GEN_ERR;
	}
	return GPN_ALM_GEN_YES;
}

UINT32 gpnAlmDSApiPortAlmAttributeGet(objLogicDesc *plocalPIndex, UINT32 almSubType, stDSAlmAttri *almAttri)
{
	UINT32 portType;
	stAlmLocalNode *pAlmLocalPort;
	stAlmScanTypeDef *pAlmScanType;
	stAlmSTCharacterDef *pAlmSubTpStr;

	/* assert */
	
	/* find stAlmLocalNode base localPortIndex */
	pAlmLocalPort = NULL;
	gpnAlmDSApiLocalPortNodeGet(plocalPIndex, &pAlmLocalPort);
	if(pAlmLocalPort == NULL)
	{
		return GPN_ALM_DST_ERR;
	}

	/* get localPortIndex's almScanTypeNode by portType */
	portType = PortIndex_GetType(plocalPIndex->portIndex);
	pAlmScanType = NULL;
	gpnAlmSeekPortTypeToAlmScanType(portType, &pAlmScanType);
	if(pAlmScanType == NULL)
	{
		return GPN_ALM_DST_ERR;
	}

	/* find stAlmSTCharacterDef base almSubType */
	pAlmSubTpStr = NULL;
	gpnAlmDSApiPortAlmSubTpNodeGet(almSubType, pAlmScanType, &pAlmSubTpStr);
	if(pAlmSubTpStr == NULL)
	{
		return GPN_ALM_DST_ERR;
	}

	gpnAlmDSApiPortAlmAttriCheckOut(pAlmLocalPort, pAlmSubTpStr, almAttri);
	
	return GPN_ALM_DST_OK;
}
UINT32 gpnAlmDSApiPortAlmAttributeGetNext(objLogicDesc *plocalPIndex, 
	UINT32 almSubType, objLogicDesc *pnextPIndex, UINT32 *nextSubType, stDSAlmAttri *almAttri)
{
	UINT32 portType;
	stAlmLocalNode *pNextLocalPort;
	stAlmScanTypeDef *pAlmScanType;
	stAlmSTCharacterDef *pNextSubTpStr;

	/* assert */

	/*
		portIndex and almSubType double Index proc 
		first localtion portIndex, next for almSunType,
		if almSubType next to 0xFFFFFFFF, then next
		for protIndex and first for almSubTyoe
	*/
	/* first find nextPortIndex */
	if(plocalPIndex->portIndex == GPN_ILLEGAL_PORT_INDEX)
	{
		pNextLocalPort = NULL;
		gpnAlmDSApiLocalPortNodeGetFirst(&pNextLocalPort);
		if(pNextLocalPort == NULL)
		{
			return GPN_ALM_DST_ERR;
		}
		memcpy(pnextPIndex, &(pNextLocalPort->localPort), sizeof(objLogicDesc));
	}
	else
	{
		pNextLocalPort = NULL;
		gpnAlmDSApiLocalPortNodeGet(plocalPIndex, &pNextLocalPort);
		if(pNextLocalPort == NULL)
		{
			return GPN_ALM_DST_ERR;
		}
		memcpy(pnextPIndex, plocalPIndex, sizeof(objLogicDesc));
	}

	/* get nextPortIndex's almScanTypeNode by portType */
	pAlmScanType = NULL;	
	portType = PortIndex_GetType(pnextPIndex->portIndex);
	gpnAlmSeekPortTypeToAlmScanType(portType, &pAlmScanType);
	if(pAlmScanType == NULL)
	{
		return GPN_ALM_DST_ERR;
	}
		
	/* find stAlmSTCharacterDef base almSubType */
	pNextSubTpStr = NULL;
	if(almSubType == GPN_ALM_TYPE_INVALID)
	{
		gpnAlmDSApiPortAlmSubTpNodeGetFirst(pAlmScanType, &pNextSubTpStr);
	}
	else
	{
		gpnAlmDSApiPortAlmSubTpNodeGetNext(almSubType, pAlmScanType, &pNextSubTpStr);
	}
	if(pNextSubTpStr == NULL)
	{
		pNextLocalPort = NULL;
		gpnAlmDSApiLocalPortNodeGetNext(pnextPIndex, &pNextLocalPort);
		if(pNextLocalPort == NULL)
		{
			return GPN_ALM_DST_ERR;
		}
		memcpy(pnextPIndex, &(pNextLocalPort->localPort), sizeof(objLogicDesc));

		pAlmScanType = NULL;
		portType = PortIndex_GetType(pnextPIndex->portIndex);
		gpnAlmSeekPortTypeToAlmScanType(portType, &pAlmScanType);
		if(pAlmScanType == NULL)
		{
			return GPN_ALM_DST_ERR;
		}

		pNextSubTpStr = NULL;
		gpnAlmDSApiPortAlmSubTpNodeGetFirst(pAlmScanType, &pNextSubTpStr);		
		if(pNextSubTpStr == NULL)
		{
			return GPN_ALM_DST_ERR;
		}
	}
	
	*nextSubType = pNextSubTpStr->almSubType;
	gpnAlmDSApiPortAlmAttriCheckOut(pNextLocalPort, pNextSubTpStr, almAttri);
	return GPN_ALM_DST_OK;
}
UINT32 gpnAlmDSApiAttributeTableGet(objLogicDesc *plocalPIndex, UINT32 almSubType, stDSAlmAttri *almAttri)
{
	UINT32 reVal;
	
	if(plocalPIndex->portIndex == GPN_ILLEGAL_PORT_INDEX)
	{
		reVal = gpnAlmDSApiAlmAttributeGet(almSubType, almAttri);
	}
	else
	{
		reVal = gpnAlmDSApiPortAlmAttributeGet(plocalPIndex, almSubType, almAttri);
	}

	return reVal;
}
UINT32 gpnAlmDSApiAttributeTableGetNext(objLogicDesc *plocalPIndex, 
	UINT32 almSubType, objLogicDesc *pnextPIndex, UINT32 *nextSubType, stDSAlmAttri *almAttri)
{
	
	UINT32 reVal;
	
	if(plocalPIndex->portIndex == GPN_ILLEGAL_PORT_INDEX)
	{
		reVal = gpnAlmDSApiAlmAttributeGetNext(almSubType, nextSubType, almAttri);
		if(reVal == GPN_ALM_DST_OK)
		{
			pnextPIndex->portIndex	= GPN_ILLEGAL_PORT_INDEX;
			pnextPIndex->portIndex3	= GPN_ILLEGAL_PORT_INDEX;
			pnextPIndex->portIndex4	= GPN_ILLEGAL_PORT_INDEX;
			pnextPIndex->portIndex5	= GPN_ILLEGAL_PORT_INDEX;
			pnextPIndex->portIndex6	= GPN_ILLEGAL_PORT_INDEX;
			pnextPIndex->devIndex	= GPN_ILLEGAL_DEVICE_INDEX;

			return GPN_ALM_DST_OK;
		}
		else
		{
			almSubType = GPN_ALM_TYPE_INVALID;
			plocalPIndex->devIndex = GPN_ILLEGAL_DEVICE_INDEX;
			plocalPIndex->portIndex = GPN_ILLEGAL_PORT_INDEX;
			plocalPIndex->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPIndex->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPIndex->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPIndex->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
		}
	}
	

	reVal = gpnAlmDSApiPortAlmAttributeGetNext(plocalPIndex,\
		almSubType, pnextPIndex, nextSubType, almAttri);

	return reVal;
}
UINT32 gpnAlmDSApiCurrAlmGet(UINT32 index, stDSCurrAlm *currAlm)
{
	stCurrAlmDB currAlmLine;
	UINT32 reVal;

	/* assert */
	
	/* find currAlm dataSheet Line */
	reVal = gpnAlmDataSheetGetLine(GPN_ALM_CURR_ALM_DSHEET, index, &currAlmLine, sizeof(stCurrAlmDB));
	if(reVal != GPN_ALM_DST_OK)
	{
		return GPN_ALM_DST_ERR;
	}

	currAlm->index = currAlmLine.index;
	currAlm->almType = currAlmLine.almType;
	currAlm->devIndex = currAlmLine.devIndex;
	currAlm->ifIndex = currAlmLine.ifIndex;
	currAlm->ifIndex2 = currAlmLine.ifIndex2;
	currAlm->ifIndex3 = currAlmLine.ifIndex3;
	currAlm->ifIndex4 = currAlmLine.ifIndex4;
	currAlm->ifIndex5 = currAlmLine.ifIndex5;
	currAlm->level = currAlmLine.level;
	currAlm->count = currAlmLine.count;
	currAlm->firstTime = currAlmLine.firstTime;
	currAlm->thisTime = currAlmLine.thisTime;
	currAlm->suffixLen = currAlmLine.suffixLen;
	if(currAlmLine.suffixLen > GPN_ALM_DST_SUFFIX_LEN)
	{
		return GPN_ALM_DST_ERR;
	}
	memcpy(currAlm->almSuffix, currAlmLine.almSuffix, currAlmLine.suffixLen);
	return GPN_ALM_DST_OK;
}

UINT32 gpnAlmDSApiCurrAlmGetNext(UINT32 index, UINT32 *nextIndex, stDSCurrAlm *currAlm)
{
	stCurrAlmDB nextCurrLine;
	UINT32 reVal;

	/* assert */

	/* find currAlm dataSheet Line */
	reVal = gpnAlmDataSheetGetNextLine(GPN_ALM_CURR_ALM_DSHEET, index, &nextCurrLine, sizeof(stCurrAlmDB));
	if(reVal != GPN_ALM_DST_OK)
	{
		return GPN_ALM_DST_ERR;
	}

	*nextIndex = nextCurrLine.index;

	currAlm->index = nextCurrLine.index;
	currAlm->almType = nextCurrLine.almType;
	currAlm->devIndex = nextCurrLine.devIndex;
	currAlm->ifIndex = nextCurrLine.ifIndex;
	currAlm->ifIndex2 = nextCurrLine.ifIndex2;
	currAlm->ifIndex3 = nextCurrLine.ifIndex3;
	currAlm->ifIndex4 = nextCurrLine.ifIndex4;
	currAlm->ifIndex5 = nextCurrLine.ifIndex5;
	currAlm->level = nextCurrLine.level;
	currAlm->count = nextCurrLine.count;
	currAlm->firstTime = nextCurrLine.firstTime;
	currAlm->thisTime = nextCurrLine.thisTime;
	currAlm->suffixLen = nextCurrLine.suffixLen;
	
	if(nextCurrLine.suffixLen > GPN_ALM_DST_SUFFIX_LEN)
	{
		return GPN_ALM_DST_ERR;
	}
	memcpy(currAlm->almSuffix, nextCurrLine.almSuffix, nextCurrLine.suffixLen);
	
	return GPN_ALM_DST_OK;
}

UINT32 gpnAlmDSApiHistAlmGet(UINT32 index, stDSHistAlm *histAlm)
{
	stHistAlmDB histAlmLine;
	UINT32 reVal;

	/* assert */
	
	/* find currAlm dataSheet Line */
	reVal = gpnAlmDataSheetGetLine(GPN_ALM_HIST_ALM_DSHEET, index, &histAlmLine, sizeof(stHistAlmDB));
	if(reVal != GPN_ALM_DST_OK)
	{
		return GPN_ALM_DST_ERR;
	}

	histAlm->index = histAlmLine.index;
	histAlm->almType = histAlmLine.almType;
	histAlm->devIndex = histAlmLine.devIndex;
	histAlm->ifIndex = histAlmLine.ifIndex;
	histAlm->ifIndex2 = histAlmLine.ifIndex2;
	histAlm->ifIndex3 = histAlmLine.ifIndex3;
	histAlm->ifIndex4 = histAlmLine.ifIndex4;
	histAlm->ifIndex5 = histAlmLine.ifIndex5;
	histAlm->level = histAlmLine.level;
	histAlm->count = histAlmLine.count;
	histAlm->firstTime = histAlmLine.firstTime;
	histAlm->thisTime = histAlmLine.thisTime;
	histAlm->disapTime = histAlmLine.disapTime;
	histAlm->suffixLen = histAlmLine.suffixLen;
	if(histAlmLine.suffixLen > GPN_ALM_DST_SUFFIX_LEN)
	{
		return GPN_ALM_DST_ERR;
	}
	memcpy(histAlm->almSuffix, histAlmLine.almSuffix, histAlmLine.suffixLen);
	
	return GPN_ALM_DST_OK;
}

UINT32 gpnAlmDSApiHistAlmGetNext(UINT32 index, UINT32 *nextIndex, stDSHistAlm *histAlm)
{
	stHistAlmDB NextHistLine;
	UINT32 reVal;

	/* assert */

	/* find currAlm dataSheet Line */
	reVal = gpnAlmDataSheetGetNextLine(GPN_ALM_HIST_ALM_DSHEET, index, &NextHistLine, sizeof(stHistAlmDB));
	if(reVal != GPN_ALM_DST_OK)
	{
		return GPN_ALM_DST_ERR;
	}
	
	*nextIndex = NextHistLine.index;
	
	histAlm->index = NextHistLine.index;
	histAlm->almType = NextHistLine.almType;
	histAlm->devIndex = NextHistLine.devIndex;
	histAlm->ifIndex = NextHistLine.ifIndex;
	histAlm->ifIndex2 = NextHistLine.ifIndex2;
	histAlm->ifIndex3 = NextHistLine.ifIndex3;
	histAlm->ifIndex4 = NextHistLine.ifIndex4;
	histAlm->ifIndex5 = NextHistLine.ifIndex5;
	histAlm->level = NextHistLine.level;
	histAlm->count = NextHistLine.count;
	histAlm->firstTime = NextHistLine.firstTime;
	histAlm->thisTime = NextHistLine.thisTime;
	histAlm->disapTime = NextHistLine.disapTime;
	histAlm->suffixLen = NextHistLine.suffixLen;
	
	if(NextHistLine.suffixLen > GPN_ALM_DST_SUFFIX_LEN)
	{
		return GPN_ALM_DST_ERR;
	}
	memcpy(histAlm->almSuffix, NextHistLine.almSuffix, NextHistLine.suffixLen);
	
	return GPN_ALM_DST_OK;
}
UINT32 gpnAlmDSApiEventGet(UINT32 index, stDSEvent *event)
{
	stEventAlmDB eventLine;
	UINT32 reVal;

	/* assert */
	
	/* find currAlm dataSheet Line */
	reVal = gpnAlmDataSheetGetLine(GPN_ALM_EVENT_DSHEET, index, &eventLine, sizeof(stEventAlmDB));
	if(reVal != GPN_ALM_DST_OK)
	{
		return GPN_ALM_DST_ERR;
	}

	event->index = eventLine.index;
	event->eventType = eventLine.eventType;
	event->devIndex = eventLine.devIndex;
	event->ifIndex = eventLine.ifIndex;
	event->ifIndex2 = eventLine.ifIndex2;
	event->ifIndex3 = eventLine.ifIndex3;
	event->ifIndex4 = eventLine.ifIndex4;
	event->ifIndex5 = eventLine.ifIndex5;
	event->level = eventLine.level;
	event->detail = eventLine.detial;
	event->thisTime = eventLine.thisTime;
	event->suffixLen = eventLine.suffixLen;
	if(eventLine.suffixLen > GPN_EVT_DST_SUFFIX_LEN)
	{
		return GPN_ALM_DST_ERR;
	}
	memcpy(event->evtSuffix, eventLine.evtSuffix, eventLine.suffixLen);
	
	return GPN_ALM_DST_OK;
}

UINT32 gpnAlmDSApiEventGetNext(UINT32 index, UINT32 *nextIndex, stDSEvent *event)
{
	stEventAlmDB nextEventLine;
	UINT32 reVal;

	/* assert */

	/* find currAlm dataSheet Line */
	reVal = gpnAlmDataSheetGetNextLine(GPN_ALM_EVENT_DSHEET, index, &nextEventLine, sizeof(stEventAlmDB));
	if(reVal != GPN_ALM_DST_OK)
	{
		return GPN_ALM_DST_ERR;
	}

	*nextIndex = nextEventLine.index;

	event->index = nextEventLine.index;
	event->eventType = nextEventLine.eventType;
	event->devIndex = nextEventLine.devIndex;
	event->ifIndex = nextEventLine.ifIndex;
	event->ifIndex2 = nextEventLine.ifIndex2;
	event->ifIndex3 = nextEventLine.ifIndex3;
	event->ifIndex4 = nextEventLine.ifIndex4;
	event->ifIndex5 = nextEventLine.ifIndex5;
	event->level = nextEventLine.level;
	event->detail = nextEventLine.detial;
	event->thisTime = nextEventLine.thisTime;
	event->suffixLen = nextEventLine.suffixLen;
	
	if(nextEventLine.suffixLen > GPN_EVT_DST_SUFFIX_LEN)
	{
		return GPN_ALM_DST_ERR;
	}
	memcpy(event->evtSuffix, nextEventLine.evtSuffix, nextEventLine.suffixLen);
	
	return GPN_ALM_DST_OK;
}
UINT32 gpnAlmDSApiCurrAlmPCountClear(UINT32 index)
{
	objLogicDesc localPIndex;
	stCurrAlmDB currAlmLine;
	UINT32 almSubType;
	UINT32 reVal;

	/* assert */
	
	/* find currAlm dataSheet Line */
	reVal = gpnAlmDataSheetGetLine(GPN_ALM_CURR_ALM_DSHEET, index, &currAlmLine, sizeof(stCurrAlmDB));
	if(reVal != GPN_ALM_DST_OK)
	{
		return GPN_ALM_DST_ERR;
	}

	/* clear dataSheet alarm counter */
	currAlmLine.count = 1;
	currAlmLine.firstTime = currAlmLine.thisTime;
	reVal = gpnAlmDataSheetModify(GPN_ALM_CURR_ALM_DSHEET, index, &currAlmLine, sizeof(stCurrAlmDB));
	if(reVal != GPN_ALM_DST_OK)
	{
		return GPN_ALM_DST_ERR;
	}

	almSubType					= currAlmLine.almType;
	localPIndex.portIndex		= currAlmLine.ifIndex;
	localPIndex.portIndex3		= currAlmLine.ifIndex2;
	localPIndex.portIndex4		= currAlmLine.ifIndex3;
	localPIndex.portIndex5		= currAlmLine.ifIndex4;
	localPIndex.portIndex6		= currAlmLine.ifIndex5;
	localPIndex.devIndex		= currAlmLine.devIndex;

	reVal = gpnAlmCfgPortOfSubAlmPCountClear(&localPIndex, almSubType);
	if(reVal != GPN_ALM_DST_OK)
	{
		return GPN_ALM_DST_ERR;
	}
	
	return GPN_ALM_DST_OK;
}

UINT32 gpnAlmDSApiSubAlmRankCfg(UINT32 almSubType, UINT32 almRank)
{
	UINT32 reVal;
	
	/*
		first modify alarm subType node info, 
		then modify scanPort node info, 
		last modify currAlm dataSheet info 
	*/
	reVal = gpnAlmCfgSubAlmRank(almSubType, almRank);
	if(reVal != GPN_ALM_DST_OK)
	{
		return GPN_ALM_DST_ERR;
	}

	return GPN_ALM_DST_OK;
}
UINT32 gpnAlmDSApiPortOfSubAlmRankCfg(objLogicDesc *plocalPIndex, UINT32 almSubType, UINT32 almRank)
{
	return gpnAlmCfgPortOfSubAlmRank(plocalPIndex, almSubType, almRank);
}
UINT32 gpnAlmDSApiSubAlmScreenCfg(UINT32 almSubType, UINT32 opt)
{
	return gpnAlmCfgSubAlmScreen(almSubType, opt);
}
UINT32 gpnAlmDSApiPortOfSubAlmScreenCfg(objLogicDesc *plocalPIndex, UINT32 almSubType, UINT32 opt)
{
	return gpnAlmCfgPortOfSubAlmScreen(plocalPIndex, almSubType, opt);
}
UINT32 gpnAlmDSApiSubAlmFiltCfg(UINT32 almSubType, UINT32 opt)
{
	return gpnAlmCfgSubAlmFilt(almSubType, opt);
}
UINT32 gpnAlmDSApiPortOfSubAlmFiltCfg(objLogicDesc *plocalPIndex, UINT32 almSubType, UINT32 opt)
{
	return gpnAlmCfgPortOfSubAlmFilt(plocalPIndex, almSubType, opt);
}
UINT32 gpnAlmDSApiSubAlmReportCfg(UINT32 almSubType, UINT32 opt)
{
	return gpnAlmCfgSubAlmReport(almSubType, opt);
}
UINT32 gpnAlmDSApiPortOfSubAlmReportCfg(objLogicDesc *plocalPIndex, UINT32 almSubType, UINT32 opt)
{
	return gpnAlmCfgPortOfSubAlmReport(plocalPIndex, almSubType, opt);
}
UINT32 gpnAlmDSApiSubAlmRecordCfg(UINT32 almSubType, UINT32 opt)
{
	return gpnAlmCfgSubAlmRecord(almSubType, opt);
}
UINT32 gpnAlmDSApiPortOfSubAlmRecordCfg(objLogicDesc *plocalPIndex, UINT32 almSubType, UINT32 opt)
{
	return gpnAlmCfgPortOfSubAlmRecord(plocalPIndex, almSubType, opt);
}
UINT32 gpnAlmDSApiPortOfSubAlmReverseCfg(objLogicDesc *plocalPIndex, UINT32 almSubType, UINT32 opt)
{
	return gpnAlmCfgPortOfSubAlmReverse(plocalPIndex, almSubType, opt);
}
UINT32 gpnAlmDSApiPortMonStateCfg(objLogicDesc *plocalPIndex, UINT32 enStat)
{
	return gpnAlmCfgPortAlmMon(plocalPIndex, enStat);
}
UINT32 gpnAlmDSApiCurrAlmProductCountClear(UINT32 currIndex)
{
	stCurrAlmDB currAlm;
	objLogicDesc viewPort;
	UINT32 reVal;

	/* get portIndex from curr dataSheet */
	reVal = gpnAlmDataSheetGetLine(GPN_ALM_CURR_ALM_DSHEET, currIndex, &currAlm, sizeof(stCurrAlmDB));
	if(reVal != GPN_ALM_DST_OK)
	{
		return GPN_ALM_DST_ERR;
	}

	viewPort.devIndex = currAlm.devIndex;
	viewPort.portIndex = currAlm.ifIndex;
	viewPort.portIndex3 = currAlm.ifIndex2;
	viewPort.portIndex4 = currAlm.ifIndex3;
	viewPort.portIndex5 = currAlm.ifIndex4;
	viewPort.portIndex6 = currAlm.ifIndex5;

	reVal = gpnAlmCfgPortOfSubAlmPCountClear(&viewPort, currAlm.almType);
	if(reVal == GPN_ALM_DST_OK)
	{
		return GPN_ALM_DST_OK;
	}
	else
	{
		return GPN_ALM_DST_ERR;
	}
}

UINT32 gpnAlmDSApiAlmAttribModify(objLogicDesc *localPIndex, UINT32 almSubType, stDSAlmAttri *dsAttri)
{
	UINT32 almAttri;
	UINT32 portAttri;
	UINT32 reVal;

	/* attribute column control */
	almAttri = (dsAttri->bitMark & 0x0000FFFF);
	portAttri = ((dsAttri->bitMark & 0xFFFF0000) >> 16);
	reVal = GPN_ALM_DST_ERR;

	if(localPIndex->portIndex == GPN_ILLEGAL_PORT_INDEX)
	{
		/* attribute config base alarm sub type */
		if(almAttri & (1U << GPN_ALM_DST_ATTRI_BIT_RANK))
		{
			reVal = gpnAlmDSApiSubAlmRankCfg(almSubType, dsAttri->rank);
		}
		if(almAttri & (1U << GPN_ALM_DST_ATTRI_BIT_MASK))
		{
			reVal = gpnAlmDSApiSubAlmScreenCfg(almSubType, dsAttri->mask);
		}
		if(almAttri & (1U << GPN_ALM_DST_ATTRI_BIT_FILT))
		{
			reVal = gpnAlmDSApiSubAlmFiltCfg(almSubType, dsAttri->filt);
		}
		if(almAttri & (1U << GPN_ALM_DST_ATTRI_BIT_REPT))
		{
			reVal = gpnAlmDSApiSubAlmReportCfg(almSubType, dsAttri->rept);
		}
		if(almAttri & (1U << GPN_ALM_DST_ATTRI_BIT_RECD))
		{
			reVal = gpnAlmDSApiSubAlmRecordCfg(almSubType, dsAttri->recd);
		}
		/* reverse not config base almType */
	}
	else
	{
		/* attribute config base port + alarm sub type */
		if(almAttri & (1U << GPN_ALM_DST_ATTRI_BIT_RANK))
		{
			if(portAttri & (1U << GPN_ALM_DST_ATTRI_BIT_RANK))
			{
				reVal = gpnAlmDSApiPortOfSubAlmRankCfg(localPIndex, almSubType, dsAttri->rank);
			}
			else
			{
				reVal = gpnAlmDSApiPortOfSubAlmRankCfg(localPIndex, almSubType, GPN_ALM_GEN_CANCEL);
			}
		}
		/* attribute config base port + alarm sub type */
		if(almAttri & (1U << GPN_ALM_DST_ATTRI_BIT_MASK))
		{
			if(portAttri & (1U << GPN_ALM_DST_ATTRI_BIT_MASK))
			{
				reVal = gpnAlmDSApiPortOfSubAlmScreenCfg(localPIndex, almSubType, dsAttri->mask);
			}
			else
			{
				reVal = gpnAlmDSApiPortOfSubAlmScreenCfg(localPIndex, almSubType, GPN_ALM_GEN_CANCEL);
			}
		}
		/* attribute config base port + alarm sub type */
		if(almAttri & (1U << GPN_ALM_DST_ATTRI_BIT_FILT))
		{
			if(portAttri & (1U << GPN_ALM_DST_ATTRI_BIT_FILT))
			{
				reVal = gpnAlmDSApiPortOfSubAlmFiltCfg(localPIndex, almSubType, dsAttri->filt);
			}
			else
			{
				reVal = gpnAlmDSApiPortOfSubAlmFiltCfg(localPIndex, almSubType, GPN_ALM_GEN_CANCEL);
			}
		}
		/* attribute config base port + alarm sub type */
		if(almAttri & (1U << GPN_ALM_DST_ATTRI_BIT_REPT))
		{
			if(portAttri & (1U << GPN_ALM_DST_ATTRI_BIT_REPT))
			{
				reVal = gpnAlmDSApiPortOfSubAlmReportCfg(localPIndex, almSubType, dsAttri->rept);
			}
			else
			{
				reVal = gpnAlmDSApiPortOfSubAlmReportCfg(localPIndex, almSubType, GPN_ALM_GEN_CANCEL);
			}
		}
		/* attribute config base port + alarm sub type */
		if(almAttri & (1U << GPN_ALM_DST_ATTRI_BIT_RECD))
		{
			if(portAttri & (1U << GPN_ALM_DST_ATTRI_BIT_RECD))
			{
				reVal = gpnAlmDSApiPortOfSubAlmRecordCfg(localPIndex, almSubType, dsAttri->recd);
			}
			else
			{
				reVal = gpnAlmDSApiPortOfSubAlmRecordCfg(localPIndex, almSubType, GPN_ALM_GEN_CANCEL);
			}
		}
		/* attribute config base port + alarm sub type */
		if(almAttri & (1U << GPN_ALM_DST_ATTRI_BIT_REVS))
		{
			if(portAttri & (1U << GPN_ALM_DST_ATTRI_BIT_REVS))
			{
				reVal = gpnAlmDSApiPortOfSubAlmReverseCfg(localPIndex, almSubType, dsAttri->revs);
			}
			else
			{
				reVal = gpnAlmDSApiPortOfSubAlmReverseCfg(localPIndex, almSubType, GPN_ALM_GEN_CANCEL);
			}
		}
	}
	
	return reVal;
}


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_DATA_STRUCT_API_C_*/

