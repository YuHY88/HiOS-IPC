/**********************************************************
* file name: gpnAlmTmpRestrain.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-08-24
* function: 
*    define temp restrain
* modify:
*
***********************************************************/

#ifndef _GPN_ALM_TMP_RESTRAIN_C_
#define _GPN_ALM_TMP_RESTRAIN_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "gpnAlmTmpRestrain.h"

extern stEQUAlmProcSpace *pgstAlmPreScanWholwInfo;

/* cyc call for renew FUN_ETH_TYPE port with ETH_SFP_TYPE port alarm */
UINT32 gpnAlmEthSfpPassAlm2EthPhy(stAlmLocalNode *almLocalNode)
{
	stAlmLocalNode *pfunEthNode;
	objLogicDesc *pethSfpPort;
	objLogicDesc funEthPort;
	
	/* assert */
	if(almLocalNode == NULL)
	{
		return GPN_ALM_GEN_ERR;
	}

	/* judge if localPort is ETH_SFP_TYPE */
	pethSfpPort = &(almLocalNode->localPort);
	if(IFM_ETH_SFP_TYPE != IFM_PORT_TYPE_DECOM(pethSfpPort->portIndex))
	{
		return GPN_ALM_GEN_OK;
	}
	
	/* encode relation FUN_ETH_TYPE portIndex */
	funEthPort.devIndex = pethSfpPort->devIndex;
	funEthPort.portIndex = IFM_FUN_ETH_PID_COMP(IFM_FUN_ETH_TYPE, IFM_LAB_SLOT_DECOM(pethSfpPort->portIndex), IFM_LAB_LAB_DECOM(pethSfpPort->portIndex));
	funEthPort.portIndex3 = pethSfpPort->portIndex3;
	funEthPort.portIndex4 = pethSfpPort->portIndex4;
	funEthPort.portIndex5 = pethSfpPort->portIndex5;
	funEthPort.portIndex6 = pethSfpPort->portIndex6;
#if 0
	if(IFM_LAB_LAB_DECOM(pethSfpPort->portIndex) == 5017)
	{
		printf("ETH_SFP(%08x-%08x|%08x|%08x|%08x|%08x) pass to FUN_ETH(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",
			pethSfpPort->devIndex, pethSfpPort->portIndex, pethSfpPort->portIndex3,\
			pethSfpPort->portIndex4, pethSfpPort->portIndex5, pethSfpPort->portIndex6,\
			funEthPort.devIndex, funEthPort.portIndex, funEthPort.portIndex3,\
			funEthPort.portIndex4, funEthPort.portIndex5, funEthPort.portIndex6);
	}
#endif
	
	/* find stAlmLocalNode with FUN_ETH_TYPE portIndex */
	pfunEthNode = NULL;
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(&funEthPort, &pfunEthNode);
	if(pfunEthNode == NULL)
	{
		return GPN_ALM_GEN_OK;
	}

	/* pass ETH_SFP_TYPE alarm to FUN_ETH_TYPE port */
#if 0
	if(IFM_LAB_LAB_DECOM(pethSfpPort->portIndex) == 5017)
	{
		printf("SFP Basic(%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x)\n\r",
			almLocalNode->pAlmScanPort->iBaseAlmDate[7], almLocalNode->pAlmScanPort->iBaseAlmDate[6],\
			almLocalNode->pAlmScanPort->iBaseAlmDate[5], almLocalNode->pAlmScanPort->iBaseAlmDate[4],\
			almLocalNode->pAlmScanPort->iBaseAlmDate[3], almLocalNode->pAlmScanPort->iBaseAlmDate[2],\
			almLocalNode->pAlmScanPort->iBaseAlmDate[1], almLocalNode->pAlmScanPort->iBaseAlmDate[0]);
		printf("SFP Alm  (%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x)\n\r",
			almLocalNode->pAlmScanPort->iRestAlmDate[7], almLocalNode->pAlmScanPort->iRestAlmDate[6],\
			almLocalNode->pAlmScanPort->iRestAlmDate[5], almLocalNode->pAlmScanPort->iRestAlmDate[4],\
			almLocalNode->pAlmScanPort->iRestAlmDate[3], almLocalNode->pAlmScanPort->iRestAlmDate[2],\
			almLocalNode->pAlmScanPort->iRestAlmDate[1], almLocalNode->pAlmScanPort->iRestAlmDate[0]);
		printf("FUN Basic(%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x)\n\r",
			pfunEthNode->pAlmScanPort->iBaseAlmDate[7], pfunEthNode->pAlmScanPort->iBaseAlmDate[6],\
			pfunEthNode->pAlmScanPort->iBaseAlmDate[5], pfunEthNode->pAlmScanPort->iBaseAlmDate[4],\
			pfunEthNode->pAlmScanPort->iBaseAlmDate[3], pfunEthNode->pAlmScanPort->iBaseAlmDate[2],\
			pfunEthNode->pAlmScanPort->iBaseAlmDate[1], pfunEthNode->pAlmScanPort->iBaseAlmDate[0]);
		printf("FUN Alm  (%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x)\n\r",
			pfunEthNode->pAlmScanPort->iRestAlmDate[7], pfunEthNode->pAlmScanPort->iRestAlmDate[6],\
			pfunEthNode->pAlmScanPort->iRestAlmDate[5], pfunEthNode->pAlmScanPort->iRestAlmDate[4],\
			pfunEthNode->pAlmScanPort->iRestAlmDate[3], pfunEthNode->pAlmScanPort->iRestAlmDate[2],\
			pfunEthNode->pAlmScanPort->iRestAlmDate[1], pfunEthNode->pAlmScanPort->iRestAlmDate[0]);
	}
#endif
	gpnAlmDiffScanTypeRestrainProc(almLocalNode, pfunEthNode);
#if 0
	if(IFM_LAB_LAB_DECOM(pethSfpPort->portIndex) == 5017)
	{
		printf("ETH_SFP alm(%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x)\n\r",
			almLocalNode->pAlmScanPort->iRestAlmDate[7], almLocalNode->pAlmScanPort->iRestAlmDate[6],\
			almLocalNode->pAlmScanPort->iRestAlmDate[5], almLocalNode->pAlmScanPort->iRestAlmDate[4],\
			almLocalNode->pAlmScanPort->iRestAlmDate[3], almLocalNode->pAlmScanPort->iRestAlmDate[2],\
			almLocalNode->pAlmScanPort->iRestAlmDate[1], almLocalNode->pAlmScanPort->iRestAlmDate[0]);
		printf("FUN_ETH alm(%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x)\n\r",
			pfunEthNode->pAlmScanPort->iRestAlmDate[7], pfunEthNode->pAlmScanPort->iRestAlmDate[6],\
			pfunEthNode->pAlmScanPort->iRestAlmDate[5], pfunEthNode->pAlmScanPort->iRestAlmDate[4],\
			pfunEthNode->pAlmScanPort->iRestAlmDate[3], pfunEthNode->pAlmScanPort->iRestAlmDate[2],\
			pfunEthNode->pAlmScanPort->iRestAlmDate[1], pfunEthNode->pAlmScanPort->iRestAlmDate[0]);
	}
#endif	
	return GPN_ALM_GEN_OK;
}

UINT32 gpnAlmDiffScanTypeRestrainProc(stAlmLocalNode *mastLocalNode, stAlmLocalNode *slavLocalNode)
{
	stAlmScanPortInfo *pMastScanPort;
	stAlmScanPortInfo *pSlavScanPort;
	stAlmScanTypeDef *pMastScanType;
	stAlmScanTypeDef *pSlavScanType;
	stAlmSTCharacterDef *pMastSubTpStr;
	UINT32 iMastAlmDate[GPN_ALM_PAGE_NUM]={0};
	UINT32 iSlavAlmDate[GPN_ALM_PAGE_NUM]={0};
	UINT32 restChg;
	UINT32 bitMask;
	UINT32 bitsInPage;
	UINT32 i;
	UINT32 j;
	UINT32 k;
#if 0
	static UINT32 test = 0;
#endif

	pMastScanPort = mastLocalNode->pAlmScanPort;
	pSlavScanPort = slavLocalNode->pAlmScanPort;

	pMastScanType = NULL;
	gpnAlmSeekPortTypeToAlmScanType(IFM_PORT_TYPE_DECOM(mastLocalNode->localPort.portIndex), &pMastScanType);
	if(pMastScanType == NULL)
	{
		return GPN_ALM_GEN_ERR;
	}
	pSlavScanType = NULL;
	gpnAlmSeekPortTypeToAlmScanType(IFM_PORT_TYPE_DECOM(slavLocalNode->localPort.portIndex), &pSlavScanType);
	if(pSlavScanType == NULL)
	{
		return GPN_ALM_GEN_ERR;
	}

#if 0
	if(IFM_LAB_LAB_DECOM(mastLocalNode->localPort.portIndex) == 5017)
	{
		printf("Master Scan Type(%s) usePag(%d) subInRest(%d)\n\r",
			pMastScanType->almScanTypeName, pMastScanType->almUsePag,\
			pMastScanType->almSubTpNumInRestType);
		printf("Slave  Scan Type(%s) usePag(%d) subInRest(%d)\n\r",
			pSlavScanType->almScanTypeName, pMastScanType->almUsePag,\
			pSlavScanType->almSubTpNumInRestType);
	}
#endif

	/*抑制类的处理*/
	if(pgstAlmPreScanWholwInfo->almGlobalCfg.restMode == GPN_ALM_REST_LEVEL_MODE)
	{
		for(k=0; k<pSlavScanType->almUsePag; k++)
		{
			iSlavAlmDate[k] = pSlavScanPort->iBaseAlmDate[k];
		}
		
		for(i=0; i<pMastScanType->almUsePag; i++)
		{
			/* user master's after-rest alarm date */
			iMastAlmDate[i] = pMastScanPort->iRestAlmDate[i];
			if(iMastAlmDate[i] == 0)
			{
				continue;
			}
			
			if((i+1) == pMastScanType->almUsePag)
			{
				if( (pMastScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE) == 0)
				{
					bitsInPage = GPN_ALM_PAGE_BIT_SIZE;
				}
				else
				{
					bitsInPage = pMastScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE;
				}
			}
			else
			{
				bitsInPage = GPN_ALM_PAGE_BIT_SIZE;
			}
			
			for(j=0; j<bitsInPage; j++)
			{
				if(iMastAlmDate[i] & (((UINT32)(1U))<<j))
				{
					pMastSubTpStr = NULL;
					gpnAlmSeekScanTypeToSubType(pMastScanType,(i*GPN_ALM_PAGE_BIT_SIZE+j),&pMastSubTpStr);
					if(pMastSubTpStr != NULL)
					{
#if 0
						if(IFM_LAB_LAB_DECOM(mastLocalNode->localPort.portIndex) == 5017)
						{
							if(test == 5)
							{
								gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType(&(slavLocalNode->pPeerNode->peerPort),\
									GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_RISE);
								gpnSockAlmMsgTxAlmNotifyBaseSubType(slavLocalNode->pPeerNode->peerPort.portIndex,\
									GPN_ALM_TYPE_FUN_ETH_LINK_FAIL, GPN_SOCK_MSG_OPT_RISE);
								gpnSockAlmMsgTxAlmNotifyBaseSubType(slavLocalNode->pPeerNode->peerPort.portIndex,\
									GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_RISE);
							}
							test++;
						}
#endif
						/*抑制类内的抑制*/
						for(k=0; k<pSlavScanType->almUsePag; k++)
						{
							iSlavAlmDate[k] &= pMastSubTpStr->restrainMask[k];
							
						}
					}
				}
			}
		}
	}
	else if(pgstAlmPreScanWholwInfo->almGlobalCfg.restMode == GPN_ALM_REST_TIME_MODE)
	{
		/* do not support */
		for(i=0; i<pSlavScanType->almUsePag; i++)
		{
			iSlavAlmDate[i] = pSlavScanPort->iBaseAlmDate[i];
		}
	}
	else
	{
		for(i=0; i<pSlavScanType->almUsePag; i++)
		{
			iSlavAlmDate[i] = pSlavScanPort->iBaseAlmDate[i];
		}
	}

	/*抑制类关系脱离???*/
	for(i=0; i<pSlavScanType->almUsePag; i++)
	{
		restChg = iSlavAlmDate[i] ^ pSlavScanPort->iRestAlmDate[i];
		if(restChg == 0)
		{
			continue;
		}

		if((i+1) == pSlavScanType->almUsePag)
		{
			if( (pSlavScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE) == 0)
			{
				bitsInPage = GPN_ALM_PAGE_BIT_SIZE;
			}
			else
			{
				bitsInPage = pSlavScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE;
			}
		}
		else
		{
			bitsInPage = GPN_ALM_PAGE_BIT_SIZE;
		}

		for(j=0; j<bitsInPage; j++)
		{
			bitMask = (((UINT32)(1U))<<j);
			if(restChg & bitMask)
			{
				/*扫描类中的子类告警在抑制类中的bit位不连续，需要特殊查找*/
				k = GPN_ALM_SCAN_32_FFFF;
				gpnAlmSeekAlmUseBitPToSubTypeNumInScanType(pSlavScanType, (i*GPN_ALM_PAGE_BIT_SIZE+j), &k);
				if(k != GPN_ALM_SCAN_32_FFFF)
				{
					if(pSlavScanPort->iRestAlmDate[i]/*原来值*/ & bitMask)
					{
						pSlavScanPort->AlmValueBuff[k].iAlmValue = GPN_ALM_CLEAN;
						/*原来有告警,还没产生完,现在无告警*/
						if(pSlavScanPort->AlmValueBuff[k].iDelayCount != 0)
						{					
							pSlavScanPort->AlmValueBuff[k].iDelayCount = 0;
						}
						else
						{
							pSlavScanPort->AlmValueBuff[k].iDelayCount =\
								pgstAlmPreScanWholwInfo->almGlobalCfg.disapDelay;
							if((pSlavScanPort->iDelyAlmMark[i] & bitMask) == 0)
							{
								pSlavScanPort->iDelyAlmMark[i] |= bitMask;
								pSlavScanPort->iDelyAlmMark[GPN_ALM_PAGE_NUM]++;
							}
							GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : clear %d %08x dly %d\n\r",\
								__FUNCTION__, j, pSlavScanPort->iDelyAlmMark[i], pSlavScanPort->AlmValueBuff[k].iDelayCount);
						}
					}
					else
					{
						pSlavScanPort->AlmValueBuff[k].iAlmValue = GPN_ALM_ARISE;
						/*原来无告警,还没消失完,现在有告警*/
						if(pSlavScanPort->AlmValueBuff[k].iDelayCount != 0)
						{
							pSlavScanPort->AlmValueBuff[k].iDelayCount = 0;
						}
						else
						{
							pSlavScanPort->AlmValueBuff[k].iDelayCount = \
								pgstAlmPreScanWholwInfo->almGlobalCfg.prodDelay;
							if((pSlavScanPort->iDelyAlmMark[i] & bitMask) == 0)
							{
								pSlavScanPort->iDelyAlmMark[i] |= bitMask;
								pSlavScanPort->iDelyAlmMark[GPN_ALM_PAGE_NUM]++;
							}
							GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : rise %d %08x dly %d\n\r",\
								__FUNCTION__, j, pSlavScanPort->iDelyAlmMark[i], pSlavScanPort->AlmValueBuff[k].iDelayCount);
						}
					}
				}
				else
				{
					GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : almScanType(%8x), page(%d)bit(%d), err!\n\r",\
						__FUNCTION__, pSlavScanType->almScanType,i, j);
				}
			}
		}
		pSlavScanPort->iRestAlmDate[i] = iSlavAlmDate[i];
	}

	return GPN_ALM_GEN_YES;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif/* _GPN_ALM_TMP_RESTRAIN_C_ */

