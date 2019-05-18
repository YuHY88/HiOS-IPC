/**********************************************************
* file name: gpnAlmCfg.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-13
* function: 
*    define alarm Config detail process
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_CFG_C_
#define _GPN_ALM_CFG_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>

#include "gpnAlmCfg.h"
#include "gpnAlmTypeStruct.h"
#include "gpnAlmScan.h"
#include "gpnAlmDSheetProc.h"
#include "gpnAlmAlmNotify.h"
#include "gpnAlmOptDriver.h"

/*log function include*/
#include "gpnLog/gpnLogFuncApi.h"

extern gstAlmTypeWholwInfo *pgstAlmTypeWholwInfo;
extern stEQUAlmProcSpace *pgstAlmPreScanWholwInfo;

UINT32 gpnAlmCfgInitCfg(void)
{
	gpnAlmCfgScalarInitCfg();
	gpnAlmCfgTableInitCfg();
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgScalarInitCfg(void)
{
	/* MIB Scalar about */
	gpnAlmCfgRiseDelay(GPN_ALM_DEF_ARISE_DELAY);
	gpnAlmCfgCleanDelay(GPN_ALM_DEF_CLEAN_DELAY);
	gpnAlmCfgBuzzerEn(GPN_ALM_GEN_ENABLE);
	gpnAlmCfgBuzzerClr(GPN_ALM_RANK_CUR);
	gpnAlmCfgBuzzerThre(GPN_ALM_RANK_CUR);
	gpnAlmCfgRestMode(GPN_ALM_REST_LEVEL_MODE);
	gpnAlmCfgReverMode(GPN_ALM_INVE_MODE_DISABLE);
	gpnAlmCfgCurrAlmDBCyc(GPN_ALM_CYC_DSHEET);
	gpnAlmCfgHistAlmDBCyc(GPN_ALM_CYC_DSHEET);
	gpnAlmCfgEventDBCyc(GPN_ALM_CYC_DSHEET);

	/* other scalar */
	gpnAlmCfgCurrAlmIndexNewStart(1);
	gpnAlmCfgHistAlmIndexNewStart(1);
	gpnAlmCfgEventIndexNewStart(1);
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgTableInitCfg(void)
{
	return GPN_ALM_GEN_YES;
}

UINT32 gpnAlmCfgCurrAlmIndexNewStart(UINT32 sIndex)
{
	pgstAlmPreScanWholwInfo->almGlobalCfg.currAlmIndex = sIndex;
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgHistAlmIndexNewStart(UINT32 sIndex)
{
	pgstAlmPreScanWholwInfo->almGlobalCfg.histAlmIndex = sIndex;
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgEventIndexNewStart(UINT32 sIndex)
{
	pgstAlmPreScanWholwInfo->almGlobalCfg.eventIndex = sIndex;
	
	return GPN_ALM_GEN_YES;
}

/*==================================================*/
/*name :  gpnAlmCfgRiseDelay                                                                              */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmCfgRiseDelay(UINT32 opt)
{
	if(opt <= GPN_ALM_DEF_ARISE_DELAY_MAX)
	{
		pgstAlmPreScanWholwInfo->almGlobalCfg.prodDelay = opt;
	}
	else
	{
		pgstAlmPreScanWholwInfo->almGlobalCfg.prodDelay = GPN_ALM_DEF_ARISE_DELAY_MAX;
	}
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmGetRiseDelay(UINT32 *opt)
{
	if(opt != NULL)
	{
		*opt = pgstAlmPreScanWholwInfo->almGlobalCfg.prodDelay;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}	
}
UINT32 gpnAlmCfgCleanDelay(UINT32 opt)
{
	if(opt <= GPN_ALM_DEF_CLEAN_DELAY_MAX)
	{
		pgstAlmPreScanWholwInfo->almGlobalCfg.disapDelay = opt;
	}
	else
	{
		pgstAlmPreScanWholwInfo->almGlobalCfg.disapDelay = GPN_ALM_DEF_CLEAN_DELAY_MAX;
	}
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmGetCleanDelay(UINT32 *opt)
{
	if(opt != NULL)
	{
		*opt = pgstAlmPreScanWholwInfo->almGlobalCfg.disapDelay;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
	
}
UINT32 gpnAlmCfgBuzzerEn(UINT32 opt)
{
	if( (opt == GPN_ALM_GEN_ENABLE) ||\
		(opt == GPN_ALM_GEN_DISABLE) )
	{
		pgstAlmPreScanWholwInfo->almGlobalCfg.buzzerEn = opt;
		/* call ring config function */
		gpnAlmOptBuzzerEn(opt);
		
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmGetBuzzerEn(UINT32 *opt)
{
	if(opt != NULL)
	{
		*opt = pgstAlmPreScanWholwInfo->almGlobalCfg.buzzerEn;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmCfgBuzzerClr(UINT32 opt)
{
	if(opt == GPN_ALM_GEN_YES)
	{
		pgstAlmPreScanWholwInfo->almGlobalCfg.buzzerClr = opt;
		/* call ring config function */
		gpnAlmOptBuzzerClr(opt);
		
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmCfgBuzzerThre(UINT32 opt)
{
	if( (opt > GPN_ALM_RANK_NULL) &&\
		(opt < GPN_ALM_RANK_ALL) )
	{
		pgstAlmPreScanWholwInfo->almGlobalCfg.buzzerThre = opt;
		/* call ring config function */
		gpnAlmOptBuzzerThre(opt);
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmGetBuzzerThre(UINT32 *opt)
{
	if(opt != NULL)
	{
		*opt = pgstAlmPreScanWholwInfo->almGlobalCfg.buzzerThre;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmCfgRestMode(UINT32 opt)
{
	if( (opt == GPN_ALM_REST_LEVEL_MODE) ||\
		(opt == GPN_ALM_REST_TIME_MODE) ||\
		(opt == GPN_ALM_REST_MODE_DISABLE) )
	{
		if(pgstAlmPreScanWholwInfo->almGlobalCfg.restMode != opt)
		{
			pgstAlmPreScanWholwInfo->almGlobalCfg.restMode = opt;

			/* fresh data struct alarm info for new rever mode */
			gpnAlmCfgRestModeChgProc(opt);
		}
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmCfgRestModeChgProc(UINT32 opt)
{
	UINT32 scanTpNum;
	UINT32 validScanPNum;
	stAlmScanTypeDef *pAlmScanType;
	stAlmLocalNode *pValidLocalPort;

	scanTpNum = 0;
	pAlmScanType = (stAlmScanTypeDef *)listFirst(&(pgstAlmTypeWholwInfo->almScanTypeQuen));
	while((pAlmScanType != NULL) && (scanTpNum < pAlmScanType->almScanQuenPortNum))
	{
		validScanPNum = 0;
		pValidLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
		while((pValidLocalPort != NULL) && (validScanPNum < pAlmScanType->almScanQuenPortNum))
		{
			pValidLocalPort->pAlmScanPort->iRestAlmMark++;

			validScanPNum++;
			pValidLocalPort = (stAlmLocalNode *)listNext((NODE *)(pValidLocalPort));
		}
		
		scanTpNum++;
		pAlmScanType = (stAlmScanTypeDef *)listNext((NODE *)(pAlmScanType));
	}
	
	return GPN_ALM_GEN_YES;
}

UINT32 gpnAlmGetRestMode(UINT32 *opt)
{
	if(opt != NULL)
	{
		*opt = pgstAlmPreScanWholwInfo->almGlobalCfg.restMode;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmCfgReverMode(UINT32 opt)
{
	if( (opt == GPN_ALM_INVE_MODE_AUTO) ||\
		(opt == GPN_ALM_INVE_MODE_MANU) ||\
		(opt == GPN_ALM_INVE_MODE_DISABLE) )
	{
		if(pgstAlmPreScanWholwInfo->almGlobalCfg.reverMode != opt)
		{
			pgstAlmPreScanWholwInfo->almGlobalCfg.reverMode = opt;

			/* fresh data struct alarm info for new rever mode */
			gpnAlmCfgReverModeChgProc(opt);
		}
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmCfgReverModeChgProc(UINT32 opt)
{	
	UINT32 i;
	UINT32 j;
	UINT32 bitMask;
	UINT32 bitsInPage;
	UINT32 scanTpNum;
	UINT32 validScanPNum;
	stAlmScanTypeDef *pAlmScanType;
	stAlmLocalNode *pValidLocalPort;
	stAlmScanPortInfo *pAlmScanPort;

	scanTpNum = 0;
	pAlmScanType = (stAlmScanTypeDef *)listFirst(&(pgstAlmTypeWholwInfo->almScanTypeQuen));
	while((pAlmScanType != NULL) && (scanTpNum < pgstAlmTypeWholwInfo->almScanTypeNum))
	{
		validScanPNum = 0;
		pValidLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
		while((pValidLocalPort != NULL) && (validScanPNum < pAlmScanType->almScanQuenPortNum))
		{
			pAlmScanPort = pValidLocalPort->pAlmScanPort;
			for(i=0;i<pAlmScanType->almUsePag;i++)
			{
				if(pAlmScanPort->iInveAlmInfo[i] == 0)
				{
					continue;
				}
				
				if((i+1) == pAlmScanType->almUsePag)
				{
					if(pAlmScanType->almSubTpNumInRestType == GPN_ALM_PAGE_BIT_SIZE)
					{
						bitsInPage = GPN_ALM_PAGE_BIT_SIZE;
					}
					else
					{
						bitsInPage = pAlmScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE;
					}
				}
				else
				{
					bitsInPage = GPN_ALM_PAGE_BIT_SIZE;
				}
				
				for(j=0;j<bitsInPage;j++)
				{
					bitMask = (((UINT32)(1U))<<j);
					if(pAlmScanPort->iInveAlmInfo[i] & bitMask)
					{
						if((pAlmScanPort->iDelyAlmMark[i] & bitMask) == 0)
						{
							pAlmScanPort->iDelyAlmMark[i] |= bitMask;
							pAlmScanPort->iDelyAlmMark[GPN_ALM_PAGE_NUM]++;
						}
					}
				}
			}
			
			validScanPNum++;
			pValidLocalPort = (stAlmLocalNode *)listNext((NODE *)(pValidLocalPort));
		}
		
		scanTpNum++;
		pAlmScanType = (stAlmScanTypeDef *)listNext((NODE *)(pAlmScanType));
	}
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmGetReverMode(UINT32 *opt)
{
	if(opt != NULL)
	{
		*opt = pgstAlmPreScanWholwInfo->almGlobalCfg.reverMode;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmCfgCurrAlmDBCyc(UINT32 opt)
{
	if( (opt == GPN_ALM_CYC_DSHEET) ||\
		(opt == GPN_ALM_LINE_DSHEET) )
	{
		pgstAlmPreScanWholwInfo->almGlobalCfg.currDBCyc = opt;
		gpnAlmDspXDbCycModeChg(GPN_ALM_CURR_ALM_DSHEET, opt);
		
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmGetCurrAlmDBCyc(UINT32 *opt)
{
	if(opt != NULL)
	{
		*opt = pgstAlmPreScanWholwInfo->almGlobalCfg.currDBCyc;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmCfgHistAlmDBCyc(UINT32 opt)
{
	if( (opt == GPN_ALM_CYC_DSHEET) ||\
		(opt == GPN_ALM_LINE_DSHEET) )
	{
		pgstAlmPreScanWholwInfo->almGlobalCfg.histDBCyc = opt;
		gpnAlmDspXDbCycModeChg(GPN_ALM_HIST_ALM_DSHEET, opt);
		
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmGetHistAlmDBCyc(UINT32 *opt)
{
	if(opt != NULL)
	{
		*opt = pgstAlmPreScanWholwInfo->almGlobalCfg.histDBCyc;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmCfgEventDBCyc(UINT32 opt)
{
	if( (opt == GPN_ALM_CYC_DSHEET) ||\
		(opt == GPN_ALM_LINE_DSHEET) )
	{
		pgstAlmPreScanWholwInfo->almGlobalCfg.eventDBCyc = opt;
		gpnAlmDspXDbCycModeChg(GPN_ALM_EVENT_DSHEET, opt);
		
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmGetEventDBCyc(UINT32 *opt)
{
	if(opt != NULL)
	{
		*opt = pgstAlmPreScanWholwInfo->almGlobalCfg.eventDBCyc;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmGetCurrDBSize(UINT32 *opt)
{
	if(opt != NULL)
	{
		*opt = pgstAlmPreScanWholwInfo->almGlobalCfg.currDBSize;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmGetHistDBSize(UINT32 *opt)
{
	if(opt != NULL)
	{
		*opt = pgstAlmPreScanWholwInfo->almGlobalCfg.histDBSize;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmGetEventDBSize(UINT32 *opt)
{
	if(opt != NULL)
	{
		*opt = pgstAlmPreScanWholwInfo->almGlobalCfg.eventDBSize;
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}

/*==================================================*/
/*name :  gpnAlmSubAlmRankCfg                                                                         */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmCfgSubAlmRank(UINT32 almSubType, UINT32 almRank)
{
	stAlmSTCharacterDef *pAlmSubTpStr;
	stAlmScanTypeDef *pAlmScanType;
	stAlmLocalNode *pAlmLocalPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmValueRecd *almValueRecd;
	UINT32 validScanPNum;
	UINT32 reVal;
	UINT32 page;
	
	/*
		first modify alarm subType node info, 
		then modify scanPort node info, 
		last modify currAlm dataSheet info 
	*/
	pAlmSubTpStr = NULL;
	pAlmScanType = NULL;
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType, &pAlmSubTpStr);
	gpnAlmSeekAlmSubTpToAlmScanTp(almSubType, &pAlmScanType);
	if((pAlmSubTpStr == NULL) || (pAlmScanType == NULL))
	{
		gpnLog(GPN_LOG_L_ALERT,\
			"%s : rela seek err! almTp %08x "
			"pAlmSubTpStr %08x pAlmScanType %08x\n\r",\
			__FUNCTION__, almSubType,\
			(UINT32)pAlmSubTpStr, (UINT32)pAlmScanType);
		
		return GPN_ALM_GEN_NO;
	}
	
	if(almRank != pAlmSubTpStr->almRank)
	{
		/* almRank valid check */
		if( (almRank == GPN_ALM_RANK_CUR) ||\
		 	(almRank == GPN_ALM_RANK_COMMON) ||\
		 	(almRank == GPN_ALM_RANK_SEVERE) ||\
		 	(almRank == GPN_ALM_RANK_EMERG) )
		{
			/* first modify alarm subType node info */
			pAlmSubTpStr->almRank = almRank;

			/*
				then modify scanPort node info,
				should modify all preScanQuen's scanPort,
				but they hash by slot, modify not easy, so,
				just modify validQuen's scanPort, when perScanQuen's
				scanPort add in validQuen, do this modify at that time
			*/
			page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;

			validScanPNum = 0;
			pAlmLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
			while((pAlmLocalPort != NULL) && (validScanPNum < pAlmScanType->almScanQuenPortNum))
			{
				pAlmScanPort = pAlmLocalPort->pAlmScanPort;
				if(pAlmScanPort->isRankBasePort[page] & \
					(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP))
				{
					/*rank base port, do nothing*/
				}
				else
				{
					almValueRecd = &(pAlmScanPort->AlmValueBuff[pAlmSubTpStr->devOrderInScanTp]);
					almValueRecd->portAlmRank = almRank;

					/* last modify currAlm dataSheet info, iAlmValue not means realy have arise alarm */
					if(almValueRecd->iAlmValue == GPN_ALM_ARISE)
					{
						reVal = gpnAlmDspSubAlmRankChg(almValueRecd->index, almRank);
						if(reVal == GPN_ALM_GEN_OK)
						{
							/* dataSheet opt ok, alarm exist, trap new formart almar to easyVeiw */
							gpnAlmANtProductReport(&(pAlmLocalPort->viewPort), almValueRecd);

							/* alm led/ring proc */
							/* ??? */
						}
					}
				}
				
				validScanPNum++;
				pAlmLocalPort = (stAlmLocalNode *)listNext((NODE *)(pAlmLocalPort));
			}
		}
		else
		{
			return GPN_ALM_GEN_NO;
		}
	}
	
	return GPN_ALM_GEN_YES;
}

UINT32 gpnAlmCfgSubAlmScreen(UINT32 almSubType, UINT32 opt)
{
	stAlmSTCharacterDef *pAlmSubTpStr;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmScanTypeDef *pAlmScanType;
	stAlmLocalNode *pAlmLocalPort;
	UINT32 validScanPNum;
	UINT32 tmpAlmData;
	UINT32 page;
	
	/*
		first modify alarm subType node info, 
		second modify scanTypenode info, 
		then modify scanPort node info, 
		last modify currAlm dataSheet info and trap new stat 
	*/
	pAlmSubTpStr = NULL;
	pAlmScanType = NULL;
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType, &pAlmSubTpStr);
	gpnAlmSeekAlmSubTpToAlmScanTp(almSubType, &pAlmScanType);
	if((pAlmSubTpStr == NULL) || (pAlmScanType == NULL))
	{
		gpnLog(GPN_LOG_L_ALERT,\
			"%s : rela seek err! almTp %08x "
			"pAlmSubTpStr %08x pAlmScanType %08x\n\r",\
			__FUNCTION__, almSubType,\
			(UINT32)pAlmSubTpStr, (UINT32)pAlmScanType);
		
		return GPN_ALM_GEN_NO;
	}

	/* first modify alarm subType node info */
	/* second modify scanTypenode info */
	page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
	if(opt == GPN_ALM_GEN_ENABLE)
	{
		pAlmSubTpStr->isScreen = GPN_ALM_GEN_ENABLE;
		pAlmScanType->almScreInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
	}
	else if(opt == GPN_ALM_GEN_DISABLE)
	{
		pAlmSubTpStr->isScreen = GPN_ALM_GEN_DISABLE;
		pAlmScanType->almScreInfo[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
	}
	else
	{
		return GPN_ALM_GEN_NO;
	}

	/* then modify scanPort node info */
	validScanPNum = 0;
	pAlmLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
	while((pAlmLocalPort != NULL) && (validScanPNum < pAlmScanType->almScanQuenPortNum))
	{
		pAlmScanPort = pAlmLocalPort->pAlmScanPort;
		if(pAlmScanPort->isScreBasePort[page] & (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP))
		{
			/*cfg base port has a high priority than cfg base subType*/
			/*do nothing*/
		}
		else
		{
			/* then modify scanPort node info */
			tmpAlmData = (pAlmScanPort->iScanAlmDate[page] & (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
			pAlmScanPort->iFrehAlmMark[page] |= tmpAlmData;
			pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM]++;

			/* last modify currAlm dataSheet info and trap new stat */
			/* timer tick will proc this auto base fresh mark */
		}
				
		validScanPNum++;
		pAlmLocalPort = (stAlmLocalNode *)listNext((NODE *)(pAlmLocalPort));
	}

	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgSubAlmFilt(UINT32 almSubType, UINT32 opt)
{
	stAlmSTCharacterDef *pAlmSubTpStr;
	stAlmScanTypeDef *pAlmScanType;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmLocalNode *pAlmLocalPort;
	UINT32 validScanPNum;
	UINT32 page;
	
	/*
		first modify alarm subType node info, 
		second modify scanTypenode info, 
		then modify scanPort node info, 
		last modify currAlm dataSheet info and trap new stat 
	*/
	pAlmSubTpStr = NULL;
	pAlmScanType = NULL;
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType, &pAlmSubTpStr);
	gpnAlmSeekAlmSubTpToAlmScanTp(almSubType, &pAlmScanType);
	if((pAlmSubTpStr == NULL) || (pAlmScanType == NULL))
	{
		gpnLog(GPN_LOG_L_ALERT, \
			"%s : rela seek err! almTp %08x "
			"pAlmSubTpStr %08x pAlmScanType %08x\n\r",\
			__FUNCTION__, almSubType,\
			(UINT32)pAlmSubTpStr, (UINT32)pAlmScanType);
		
		return GPN_ALM_GEN_NO;
	}

	page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
	if(opt == GPN_ALM_GEN_ENABLE)
	{
		pAlmSubTpStr->isFilted = GPN_ALM_GEN_ENABLE;
		pAlmScanType->almFiltInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
	}
	else if(opt == GPN_ALM_GEN_DISABLE)
	{
		pAlmSubTpStr->isFilted = GPN_ALM_GEN_DISABLE;
		pAlmScanType->almFiltInfo[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
	}
	else
	{
		return GPN_ALM_GEN_NO;
	}

	validScanPNum = 0;
	pAlmLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
	while((pAlmLocalPort != NULL) && (validScanPNum < pAlmScanType->almScanQuenPortNum))
	{
		pAlmScanPort = pAlmLocalPort->pAlmScanPort;
		if(pAlmScanPort->isFiltBasePort[page] & (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP))
		{
			/*cfg base port has a high priority than cfg base subType*/
			/*do nothing*/
		}
		else
		{
			/* then modify scanPort node info */
			pAlmScanPort->iSureAlmMark[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
			pAlmScanPort->iSureAlmMark[GPN_ALM_PAGE_NUM]++;

			/* last modify currAlm dataSheet info and trap new stat */
			/* timer tick will proc this auto base fresh mark */
		}
		
		validScanPNum++;
		pAlmLocalPort = (stAlmLocalNode *)listNext((NODE *)(pAlmLocalPort));
	}
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgSubAlmReport(UINT32 almSubType, UINT32 opt)
{
	stAlmSTCharacterDef *pAlmSubTpStr;
	stAlmScanTypeDef *pAlmScanType;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmLocalNode *pAlmLocalPort;
	UINT32 validScanPNum;
	UINT32 page;
	
	/*
		first modify alarm subType node info, 
		second modify scanTypenode info, 
		then modify scanPort node info, 
		last modify currAlm dataSheet info and trap new stat 
	*/
	pAlmSubTpStr = NULL;
	pAlmScanType = NULL;
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType, &pAlmSubTpStr);
	gpnAlmSeekAlmSubTpToAlmScanTp(almSubType, &pAlmScanType);
	if((pAlmSubTpStr == NULL) || (pAlmScanType == NULL))
	{
		gpnLog(GPN_LOG_L_ALERT,\
			"%s : rela seek err! almTp %08x "
			"pAlmSubTpStr %08x pAlmScanType %08x\n\r",\
			__FUNCTION__, almSubType,\
			(UINT32)pAlmSubTpStr, (UINT32)pAlmScanType);
		
		return GPN_ALM_GEN_NO;
	}

	page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
	if(opt == GPN_ALM_GEN_ENABLE)
	{
		pAlmSubTpStr->isReport = GPN_ALM_GEN_ENABLE;
		pAlmScanType->almReptInfo[page] |= pAlmSubTpStr->almUseIdentify;
	}
	else if(opt == GPN_ALM_GEN_DISABLE)
	{
		pAlmSubTpStr->isReport = GPN_ALM_GEN_DISABLE;
		pAlmScanType->almReptInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
	}
	else
	{
		return GPN_ALM_GEN_NO;
	}

	validScanPNum = 0;
	pAlmLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
	while((pAlmLocalPort != NULL) && (validScanPNum < pAlmScanType->almScanQuenPortNum))
	{
		pAlmScanPort = pAlmLocalPort->pAlmScanPort;
		if(pAlmScanPort->isReptBasePort[page] & (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP))
		{
			/* cfg base port has a high priority than cfg base subType */
			/* do nothing*/
		}
		else
		{
			/* then modify scanPort node info */
			pAlmScanPort->iSureAlmMark[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
			pAlmScanPort->iSureAlmMark[GPN_ALM_PAGE_NUM]++;

			/* last modify currAlm dataSheet info and trap new stat */
			/* timer tick will proc this auto base fresh mark */
		}
		
		validScanPNum++;
		pAlmLocalPort = (stAlmLocalNode *)listNext((NODE *)(pAlmLocalPort));
	}
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgSubAlmRecord(UINT32 almSubType, UINT32 opt)
{
	stAlmSTCharacterDef *pAlmSubTpStr;
	stAlmScanTypeDef *pAlmScanType;
	UINT32 page;
	
	/*
		first modify alarm subType node info, 
		second modify scanTypenode info, 
		then modify scanPort node info, 
		last modify currAlm dataSheet info and trap new stat 
	*/
	pAlmSubTpStr = NULL;
	pAlmScanType = NULL;
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType, &pAlmSubTpStr);
	gpnAlmSeekAlmSubTpToAlmScanTp(almSubType, &pAlmScanType);
	if((pAlmSubTpStr == NULL) || (pAlmScanType == NULL))
	{
		gpnLog(GPN_LOG_L_ALERT,\
			"gpnAlmSubAlmRecordCfg:rela seek err! "
			"almTp %08x pAlmSubTpStr %08x pAlmScanType %08x\n\r",\
			almSubType, (UINT32)pAlmSubTpStr, (UINT32)pAlmScanType);
		
		return GPN_ALM_GEN_NO;
	}

	page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
	if(opt == GPN_ALM_GEN_ENABLE)
	{
		pAlmSubTpStr->isRecord = GPN_ALM_GEN_ENABLE;
		pAlmScanType->almReckInfo[page] |= pAlmSubTpStr->almUseIdentify;
	}
	else if(opt == GPN_ALM_GEN_DISABLE)
	{
		pAlmSubTpStr->isRecord = GPN_ALM_GEN_DISABLE;
		pAlmScanType->almReckInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
	}
	else
	{
		return GPN_ALM_GEN_NO;
	}

	/* no config in scanPort node */
	
	return GPN_ALM_GEN_YES;
}
/*==================================================*/
/*name :  gpnAlmCfgPortOfSubAlmRank                                                                 */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmCfgPortOfSubAlmRank(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 almRank)
{
	UINT32 page;
	UINT32 reVal;
	stAlmScanTypeDef *pAlmScanType;
	stAlmLocalNode *pAlmLocalPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubTpStr;
	stAlmValueRecd *almValueRecd;
	
	pAlmLocalPort = NULL;
	pAlmSubTpStr = NULL;
	pAlmScanType = NULL;
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(pPortInfo, &pAlmLocalPort);
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType, &pAlmSubTpStr);
	gpnAlmSeekAlmSubTpToAlmScanTp(almSubType, &pAlmScanType);
	if(	(pAlmLocalPort == NULL) ||\
		(pAlmSubTpStr == NULL) ||\
		(pAlmScanType == NULL) )
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP,\
			"%s : seek err! port(%08x%08x|%08x|%08x|%08x|%08x) almType %08x almRank %d\n",\
			__FUNCTION__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6,\
			almSubType, almRank);
		
		return GPN_ALM_GEN_NO;
	}

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
	
	/* almRank valid check */
	if( (almRank == GPN_ALM_RANK_CUR) ||\
		(almRank == GPN_ALM_RANK_COMMON) ||\
		(almRank == GPN_ALM_RANK_SEVERE) ||\
		(almRank == GPN_ALM_RANK_EMERG) )
	{
		pAlmScanPort->isRankBasePort[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
	}
	else if(almRank == GPN_ALM_GEN_CANCEL)
	{
		pAlmScanPort->isRankBasePort[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
	}
	else
	{
		return GPN_ALM_GEN_NO;
	}

	almValueRecd = &(pAlmScanPort->AlmValueBuff[pAlmSubTpStr->devOrderInScanTp]);
	if(almValueRecd->portAlmRank != almRank)
	{
		almValueRecd->portAlmRank = almRank;

		/* last modify currAlm dataSheet info, iAlmValue not means realy have arise alarm */
		if(almValueRecd->iAlmValue == GPN_ALM_ARISE)
		{
			reVal = gpnAlmDspSubAlmRankChg(almValueRecd->index, almRank);
			if(reVal == GPN_ALM_GEN_OK)
			{
				/* dataSheet opt ok, alarm exist, trap new formart almar to easyVeiw */
				gpnAlmANtProductReport(&(pAlmLocalPort->viewPort), almValueRecd);

				/* alm led/ring proc */
				/* ??? */
			}
		}
	}
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgPortOfSubAlmScreen(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt)
{
	UINT32 page;
	UINT32 tmpAlmData;
	stAlmScanTypeDef *pAlmScanType;
	stAlmLocalNode *pAlmLocalPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubTpStr;
	
	pAlmLocalPort = NULL;
	pAlmSubTpStr = NULL;
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(pPortInfo, &pAlmLocalPort);
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType, &pAlmSubTpStr);
	gpnAlmSeekAlmSubTpToAlmScanTp(almSubType, &pAlmScanType);
	if(	(pAlmLocalPort == NULL) ||\
		(pAlmSubTpStr == NULL) ||\
		(pAlmScanType == NULL) )
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP,\
			"%s : seek err! port(%08x%08x|%08x|%08x|%08x|%08x) almType %08x opt %d\n",\
			__FUNCTION__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6,\
			almSubType, opt);
		
		return GPN_ALM_GEN_NO;
	}

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
	/* 0 means screen ; 1 means not screen(default) */
	if(opt == GPN_ALM_GEN_ENABLE)
	{
		pAlmScanPort->iScreAlmInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
		pAlmScanPort->isScreBasePort[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
	}
	else if(opt == GPN_ALM_GEN_DISABLE)
	{
		pAlmScanPort->iScreAlmInfo[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
		pAlmScanPort->isScreBasePort[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
	}
	else if(opt == GPN_ALM_GEN_CANCEL)
	{
		pAlmScanPort->isScreBasePort[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
	}
	else
	{
		return GPN_ALM_GEN_NO;
	}

	/* check if in valid scanQuen */
	if(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_YES)
	{
		/* triger timer tick alarm screen process */
		tmpAlmData = (pAlmScanPort->iScanAlmDate[page] & (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
		pAlmScanPort->iFrehAlmMark[page] |= tmpAlmData;
		pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM]++;
	}

	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgPortOfSubAlmFilt(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt)
{
	UINT32 page;
	stAlmScanTypeDef *pAlmScanType;
	stAlmLocalNode *pAlmLocalPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubTpStr;
	
	pAlmLocalPort = NULL;
	pAlmSubTpStr = NULL;
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(pPortInfo, &pAlmLocalPort);
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType, &pAlmSubTpStr);
	gpnAlmSeekAlmSubTpToAlmScanTp(almSubType, &pAlmScanType);
	if(	(pAlmLocalPort == NULL) ||\
		(pAlmSubTpStr == NULL) ||\
		(pAlmScanType == NULL) )
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP,\
			"%s : seek err! port(%08x%08x|%08x|%08x|%08x|%08x) almType %08x opt %d\n",\
			__FUNCTION__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6,\
			almSubType, opt);
		
		return GPN_ALM_GEN_NO;
	}

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
	/* 0 means filt ; 1 means not filt */
	if(opt == GPN_ALM_GEN_ENABLE)
	{
		pAlmScanPort->iFiltAlmInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
		pAlmScanPort->isFiltBasePort[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
	}
	else if(opt == GPN_ALM_GEN_DISABLE)
	{
		pAlmScanPort->iFiltAlmInfo[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
		pAlmScanPort->isFiltBasePort[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
	}
	else if(opt == GPN_ALM_GEN_CANCEL)
	{
		pAlmScanPort->iFiltAlmInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
		pAlmScanPort->isFiltBasePort[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
	}
	else
	{
		return GPN_ALM_GEN_NO;
	}
	
	/* check if in valid scanQuen */
	if(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_YES)
	{
		/* triger timer tick alarm screen process */
		pAlmScanPort->iSureAlmMark[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
		pAlmScanPort->iSureAlmMark[GPN_ALM_PAGE_NUM]++;
	}
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgPortOfSubAlmReport(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt)
{
	UINT32 page;
	stAlmScanTypeDef *pAlmScanType;
	stAlmLocalNode *pAlmLocalPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubTpStr;
	
	pAlmLocalPort = NULL;
	pAlmSubTpStr = NULL;
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(pPortInfo, &pAlmLocalPort);
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType, &pAlmSubTpStr);
	gpnAlmSeekAlmSubTpToAlmScanTp(almSubType, &pAlmScanType);
	if(	(pAlmLocalPort == NULL) ||\
		(pAlmSubTpStr == NULL) ||\
		(pAlmScanType == NULL) )
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP,\
			"%s : seek err! port(%08x%08x|%08x|%08x|%08x|%08x) almType %08x opt %d\n",\
			__FUNCTION__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6,\
			almSubType, opt);
		
		return GPN_ALM_GEN_NO;
	}

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
	/* 0 means not report ; 1 means report */
	if(opt == GPN_ALM_GEN_ENABLE)
	{
		pAlmScanPort->iReptAlmInfo[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
		pAlmScanPort->isReptBasePort[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
	}
	else if(opt == GPN_ALM_GEN_DISABLE)
	{
		pAlmScanPort->iReptAlmInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
		pAlmScanPort->isReptBasePort[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
	}
	else if(opt == GPN_ALM_GEN_CANCEL)
	{
		pAlmScanPort->iReptAlmInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
		pAlmScanPort->isReptBasePort[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
	}
	else
	{
		return GPN_ALM_GEN_NO;
	}
	
	/* check if in valid scanQuen */
	if(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_YES)
	{
		/* triger timer tick alarm screen process */
		pAlmScanPort->iSureAlmMark[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
		pAlmScanPort->iSureAlmMark[GPN_ALM_PAGE_NUM]++;
	}
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgPortOfSubAlmRecord(objLogicDesc *pPortInfo, UINT32 almSubType, UINT32 opt)
{
	UINT32 page;
	stAlmLocalNode *pAlmLocalPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubTpStr;

	pAlmLocalPort = NULL;
	pAlmSubTpStr = NULL;
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(pPortInfo, &pAlmLocalPort);
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType, &pAlmSubTpStr);
	if(	(pAlmLocalPort == NULL) ||\
		(pAlmSubTpStr == NULL) )
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP,\
			"%s : seek err! port(%08x%08x|%08x|%08x|%08x|%08x) almType %08x opt %d\n",\
			__FUNCTION__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6,\
			almSubType, opt);
		
		return GPN_ALM_GEN_NO;
	}

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
	/* 0 means not record ; 1 means record */
	if(opt == GPN_ALM_GEN_ENABLE)
	{
		pAlmScanPort->iReckAlmInfo[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
		pAlmScanPort->isReckBasePort[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
	}
	else if(opt == GPN_ALM_GEN_DISABLE)
	{
		pAlmScanPort->iReckAlmInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
		pAlmScanPort->isReckBasePort[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
	}
	else if(opt == GPN_ALM_GEN_CANCEL)
	{
		pAlmScanPort->iReckAlmInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
		pAlmScanPort->isReckBasePort[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
	}
	else
	{
		return GPN_ALM_GEN_NO;
	}
	
	/* no config any more */
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgPortOfSubAlmReverse(objLogicDesc *pPortInfo, UINT32 almSubType,UINT32 opt)
{
	UINT32 page;
	stAlmScanTypeDef *pAlmScanType;
	stAlmLocalNode *pAlmLocalPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubTpStr;

	pAlmLocalPort = NULL;
	pAlmSubTpStr = NULL;
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(pPortInfo, &pAlmLocalPort);
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType,&pAlmSubTpStr);
	gpnAlmSeekAlmSubTpToAlmScanTp(almSubType,&pAlmScanType);
	if(	(pAlmLocalPort == NULL) ||\
		(pAlmSubTpStr == NULL) ||\
		(pAlmScanType == NULL) )
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP,\
			"%s : seek err! port(%08x%08x|%08x|%08x|%08x|%08x) almType %08x opt %d\n",\
			__FUNCTION__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6,\
			almSubType, opt);
		
		return GPN_ALM_GEN_NO;
	}

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	page = pAlmSubTpStr->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
	/* 0 means not reverse(default) ; 1 means reverse */
	if(opt == GPN_ALM_GEN_ENABLE)
	{
		pAlmScanPort->iInveAlmInfo[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
	}
	else if(opt == GPN_ALM_GEN_DISABLE)
	{
		pAlmScanPort->iInveAlmInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
	}
	else
	{
		pAlmScanPort->iInveAlmInfo[page] &= (~(pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP));
		//return GPN_ALM_GEN_NO;
	}
	
	/* check if in valid scanQuen */
	if(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_YES)
	{
		/* triger timer tick alarm screen process */
		pAlmScanPort->iDelyAlmMark[page] |= (pAlmSubTpStr->almUseIdentify & GPN_ALM_IN_PAGE_BITP);
		pAlmScanPort->iDelyAlmMark[GPN_ALM_PAGE_NUM]++;
	}
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgPortOfSubAlmPCountClear(objLogicDesc *pPortInfo, UINT32 almSubType)
{
	UINT32 reVal;
	stAlmScanTypeDef *pAlmScanType;
	stAlmLocalNode *pAlmLocalPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubTpStr;
	stAlmValueRecd *almValueRecd;
	
	pAlmLocalPort = NULL;
	pAlmSubTpStr = NULL;
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(pPortInfo, &pAlmLocalPort);
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType, &pAlmSubTpStr);
	gpnAlmSeekAlmSubTpToAlmScanTp(almSubType, &pAlmScanType);
	if(	(pAlmLocalPort == NULL) ||\
		(pAlmSubTpStr == NULL) ||\
		(pAlmScanType == NULL) )
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP,\
			"%s : seek err! port(%08x%08x|%08x|%08x|%08x|%08x) almType %08x\n",\
			__FUNCTION__,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6,\
			almSubType);
		
		return GPN_ALM_GEN_NO;
	}

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	almValueRecd = &(pAlmScanPort->AlmValueBuff[pAlmSubTpStr->devOrderInScanTp]);
	
	if(almValueRecd->iAlmValue == GPN_ALM_ARISE)
	{
		/* modify alarm datastruct record */
		almValueRecd->prodCnt = 1;
		almValueRecd->firstTime = almValueRecd->thisTime;

		/* last modify currAlm dataSheet info, iAlmValue not means realy have arise alarm */
		reVal = gpnAlmDspSubAlmAriseCountClear(almValueRecd->index);
		if(reVal != GPN_ALM_GEN_OK)
		{
			return GPN_ALM_GEN_NO;
		}
		
		/* dataSheet opt ok, alarm exist, trap new formart almar to easyVeiw */
		gpnAlmANtProductReport(&(pAlmLocalPort->viewPort), almValueRecd);
	}
	else
	{
		almValueRecd->prodCnt = 0;
		almValueRecd->firstTime = 0;
		almValueRecd->thisTime = 0;
	}

	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmCfgPortAlmMon(objLogicDesc *plocalPIndex, UINT32 opt)
{
	stAlmLocalNode *pAlmLocalPort;
	
	pAlmLocalPort = NULL;
	/* find almScanPort base localPortIndex */
	gpnAlmSeekLocalPortIndexToAlmLocalPortNode(plocalPIndex, &pAlmLocalPort);
	if(pAlmLocalPort == NULL)
	{
		return GPN_ALM_GEN_NO;
	}
	else
	{
		gpnAlmRelevChgAlmScanPortToValidScanQuen(pAlmLocalPort, opt);
		
		/* send msg to IFM, notify alm mon sta change, then process should send or not alarm to gpn_alarm */
		/* ??? */

		return GPN_ALM_GEN_YES;
	}
}

UINT32 gpnAlmCfgAlmRankSummary(UINT32 almRank,UINT32 opt)
{
	stAlmGlobalCfg *palmGCfg;
	UINT32 newStat;

	/* assert */
	if( (almRank < GPN_ALM_RANK_CUR) &&\
		(almRank > GPN_ALM_RANK_EMERG) )
	{
		return GPN_ALM_GEN_ERR;
	}
	
	palmGCfg = &(pgstAlmPreScanWholwInfo->almGlobalCfg);
	newStat = GPN_ALM_GEN_NO;

	/*printf("%s : newRank(%d), newOpt(%d), oldRank(high~%d %d %d %d~low)\n\r",\
		__FUNCTION__, almRank, opt,\
		palmGCfg->rankAlmCnt[GPN_ALM_RANK_EMERG-1],\
		palmGCfg->rankAlmCnt[GPN_ALM_RANK_SEVERE-1],\
		palmGCfg->rankAlmCnt[GPN_ALM_RANK_COMMON-1],\
		palmGCfg->rankAlmCnt[GPN_ALM_RANK_CUR-1]);*/

	if(opt == GPN_ALM_ARISE)
	{
		/*befor counter add*/
		if(palmGCfg->rankAlmCnt[almRank-1] == 0)
		{
			newStat = GPN_ALM_GEN_YES;
		}
		palmGCfg->rankAlmCnt[almRank-1]++;
	}
	else /*if(opt == GPN_ALM_CLEAN)*/
	{
		if(palmGCfg->rankAlmCnt[almRank-1] > 0)
		{
			palmGCfg->rankAlmCnt[almRank-1]--;
			/*after counter increase*/
			if(palmGCfg->rankAlmCnt[almRank-1] == 0)
			{
				newStat = GPN_ALM_GEN_YES;
			}
		}
		
	}

	if(newStat== GPN_ALM_GEN_YES)
	{
		/*alarm led opt : */
		gpnAlmOptLedEn(
			palmGCfg->rankAlmCnt[GPN_ALM_RANK_EMERG-1],\
			palmGCfg->rankAlmCnt[GPN_ALM_RANK_SEVERE-1],\
			palmGCfg->rankAlmCnt[GPN_ALM_RANK_COMMON-1],\
			palmGCfg->rankAlmCnt[GPN_ALM_RANK_CUR-1]);
		
		/*highest alm rank*/
		if(palmGCfg->rankAlmCnt[3/*GPN_ALM_RANK_EMERG-1*/] != 0)
		{
			palmGCfg->highRank = GPN_ALM_RANK_EMERG;
		}
		else if(palmGCfg->rankAlmCnt[2/*GPN_ALM_RANK_SEVERE-1*/] != 0)
		{
			palmGCfg->highRank = GPN_ALM_RANK_SEVERE;
		}
		else if(palmGCfg->rankAlmCnt[1/*GPN_ALM_RANK_COMMON-1*/] != 0)
		{
			palmGCfg->highRank = GPN_ALM_RANK_COMMON;
		}
		else if(palmGCfg->rankAlmCnt[0/*GPN_ALM_RANK_CUR-1*/] != 0)
		{
			palmGCfg->highRank = GPN_ALM_RANK_CUR;
		}
		else
		{
			palmGCfg->highRank = GPN_ALM_RANK_NULL;
		}
		
		/*alarm bell opt*/
		if( (palmGCfg->highRank >= palmGCfg->buzzerThre) &&
			(palmGCfg->buzzerEn == GPN_ALM_GEN_ENABLE) )
		{
			gpnAlmOptBuzzerEn(GPN_ALM_GEN_ENABLE);
		}
		else
		{
			gpnAlmOptBuzzerEn(GPN_ALM_GEN_ENABLE);
		}
	}
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmUppePortNodeSet(objLogicDesc downPort, objLogicDesc uppePort)
{
	stAlmLocalNode *pDownAlmLocalPort;
	stAlmLocalNode *pUppeAlmLocalPort;

	if( (uppePort.devIndex != GPN_ALM_SCAN_32_NULL) &&\
		(uppePort.portIndex != GPN_ALM_SCAN_32_NULL) )
	{
		gpnAlmSeekLocalPortIndexToAlmLocalPortNode(&downPort, &pDownAlmLocalPort);
		if(pDownAlmLocalPort != NULL)
		{
			gpnAlmSeekLocalPortIndexToAlmLocalPortNode(&uppePort, &pUppeAlmLocalPort);
			if(pUppeAlmLocalPort != NULL)
			{
				pDownAlmLocalPort->pAlmScanPort->pUppeAlmScanPort =\
					pUppeAlmLocalPort->pAlmScanPort;
				return GPN_ALM_GEN_YES;
			}
		}
	}

	return GPN_ALM_GEN_NO;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_ALM_CFG	_C_ */

