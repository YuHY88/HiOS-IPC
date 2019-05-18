/**********************************************************
* file name: gpnStatCollectProc.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-25
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_COLLECT_PROC_C_
#define _GPN_STAT_COLLECT_PROC_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>

#include <lib/syslog.h>
#include <lib/log.h>

#include "gpnStatCollectProc.h"
#include "gpnStatDataStruct.h"
#include "gpnLog/gpnLogFuncApi.h"


static void debug_show_data(UINT32 statType, void *pdata)
{
	gpnStatSdhRsData sdhRs;			memset(&sdhRs, 0, sizeof(gpnStatSdhRsData));
	gpnStatSdhMsData sdhMs;			memset(&sdhMs, 0, sizeof(gpnStatSdhMsData));
	gpnStatSdhHpData sdhHp;			memset(&sdhHp, 0, sizeof(gpnStatSdhHpData));
	gpnStatSdhLpData sdhLp;			memset(&sdhLp, 0, sizeof(gpnStatSdhLpData));
	gpnStatPdhPpiData pdhPpi;		memset(&pdhPpi, 0, sizeof(gpnStatPdhPpiData));
	gpnStatEthMacPData ethMacP;		memset(&ethMacP, 0, sizeof(gpnStatEthMacPData));
	gpnStatPtnVsPData vsP;			memset(&vsP, 0, sizeof(gpnStatPtnVsPData));
	gpnStatPtnLspPData lspP;		memset(&lspP, 0, sizeof(gpnStatPtnLspPData));
	gpnStatPtnPwPData pwP;			memset(&pwP, 0, sizeof(gpnStatPtnPwPData));
	gpnStatFollowPData followP;		memset(&followP, 0, sizeof(gpnStatFollowPData));
	gpnStatPWE3Data pwE3;			memset(&pwE3, 0, sizeof(gpnStatPWE3Data));
	gpnStatPtnVplsPwPData vplsPwP;	memset(&vplsPwP, 0, sizeof(gpnStatPtnVplsPwPData));
	gpnStatPtnVUniPData vUniP;		memset(&vUniP, 0, sizeof(gpnStatPtnVUniPData));
	gpnStatEthSFPData ethSfpP;		memset(&ethSfpP, 0, sizeof(gpnStatEthSFPData));
	gpnStatPOWData pow;				memset(&pow, 0, sizeof(gpnStatPOWData));
	gpnStatEquData equ;				memset(&equ, 0, sizeof(gpnStatEquData));
	gpnStatPtnVsMData vsM;			memset(&vsM, 0, sizeof(gpnStatPtnVsMData));
	gpnStatPtnLspMData lspM;		memset(&lspM, 0, sizeof(gpnStatPtnLspMData));
	gpnStatPtnPwMData pwM;			memset(&pwM, 0, sizeof(gpnStatPtnPwMData));
	gpnStatEthMacMData ethMacM;		memset(&ethMacM, 0, sizeof(gpnStatEthMacMData));
	gpnStatSdhSfpData sdhSfp;		memset(&sdhSfp, 0, sizeof(gpnStatSdhSfpData));
	gpnStatPdhSfpData pdhSfp;		memset(&pdhSfp, 0, sizeof(gpnStatPdhSfpData));
	gpnStatEnvData env;				memset(&env, 0, sizeof(gpnStatEnvData));
	gpnStatSoftData soft;			memset(&soft, 0, sizeof(gpnStatSoftData));
	gpnStatFollowMData followM;		memset(&followM, 0, sizeof(gpnStatFollowMData));
	gpnStatPtnVplsPwMData vplsPwM;	memset(&vplsPwM, 0, sizeof(gpnStatPtnVplsPwMData));
	gpnStatPtnMepMData mepM;		memset(&mepM, 0, sizeof(gpnStatPtnMepMData));
	
	zlog_debug(0, "\r\n\r\n%s[%d] : statType(%x)\n", __func__, __LINE__, statType);
	switch(statType)
	{
		case GPN_STAT_T_SDH_RS_TYPE:
			memcpy(&sdhRs, (char *)pdata, sizeof(gpnStatSdhRsData));
			zlog_debug(0, "%s[%d] : SDH_RS data(%u|%u|%u|%u|%u)\n", __func__, __LINE__,
				sdhRs.statSdhRsBBE, sdhRs.statSdhRsES, sdhRs.statSdhRsSES, sdhRs.statSdhRsUAS, sdhRs.statSdhRsOFS);
			break;

		
		case GPN_STAT_T_SDH_MS_TYPE:
			memcpy(&sdhMs, (char *)pdata, sizeof(gpnStatSdhMsData));
			zlog_debug(0, "%s[%d] : SDH_MS data(%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				sdhMs.statSdhMsBBE, sdhMs.statSdhMsES, sdhMs.statSdhMsSES, sdhMs.statSdhMsUAS,
				sdhMs.statSdhMsReiBBE, sdhMs.statSdhMsReiES, sdhMs.statSdhMsReiSES);
			break;

		
		case GPN_STAT_T_SDH_HP_TYPE:
			memcpy(&sdhHp, (char *)pdata, sizeof(gpnStatSdhHpData));
			zlog_debug(0, "%s[%d] : SDH_HP data(%d|%d|%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				sdhHp.statSdhHpBBE, sdhHp.statSdhHpES, sdhHp.statSdhHpSES, sdhHp.statSdhHpUAS, sdhHp.statSdhHpReiBBE,
				sdhHp.statSdhHpReiES, sdhHp.statSdhHpReiSES, sdhHp.statSdhHpAuPJCP, sdhHp.statSdhHpAuPJCN);
			break;

		
		case GPN_STAT_T_SDH_LP_TYPE:
			memcpy(&sdhLp, (char *)pdata, sizeof(gpnStatSdhLpData));
			zlog_debug(0, "%s[%d] : SDH_LP data(%d|%d|%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				sdhLp.statSdhLpBBE, sdhLp.statSdhLpES, sdhLp.statSdhLpSES, sdhLp.statSdhLpUAS, sdhLp.statSdhLpReiBBE,
				sdhLp.statSdhLpReiES, sdhLp.statSdhLpReiSES, sdhLp.statSdhLpTuPJCP, sdhLp.statSdhLpTuPJCN);
			break;
		
				
		case GPN_STAT_T_PDH_PPI_TYPE:
			memcpy(&pdhPpi, (char *)pdata, sizeof(gpnStatPdhPpiData));
			zlog_debug(0, "%s[%d] : PDH_PPI data(%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				pdhPpi.statPdhES, pdhPpi.statPdhSES, pdhPpi.statPdhBBE, pdhPpi.statPdhUAS, pdhPpi.statPdhCV);
			break;

		
		case GPN_STAT_T_ETH_MAC_P_TYPE:
			memcpy(&ethMacP, (char *)pdata, sizeof(gpnStatEthMacPData));
			zlog_debug(0, "%s[%d] : ETH_MAC_P data(%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				ethMacP.statEthMacRXGBh32, ethMacP.statEthMacRXGBl32, ethMacP.statEthMacRXGPh32, ethMacP.statEthMacRXGPl32,
				ethMacP.statEthMacTXGBh32, ethMacP.statEthMacTXGBl32, ethMacP.statEthMacTXGPh32, ethMacP.statEthMacTXGPl32,
				ethMacP.statEthMacFCSh32, ethMacP.statEthMacFCSl32, ethMacP.statEthMacDroph32, ethMacP.statEthMacDropl32,
				ethMacP.statEthMacRXBh32, ethMacP.statEthMacRXBl32, ethMacP.statEthMacRXPh32, ethMacP.statEthMacRXPl32,
				ethMacP.statEthMacTXBh32, ethMacP.statEthMacTXBl32, ethMacP.statEthMacTXPh32, ethMacP.statEthMacTXPl32,
				ethMacP.statEthMacRERRSh32, ethMacP.statEthMacRERRl32);
			zlog_debug(0, "%s[%d] : ETH_MAC_P data(%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				ethMacP.statEthMacROSPCh32, ethMacP.statEthMacROSPCl32, ethMacP.statEthMacRUSPCh32, ethMacP.statEthMacRUSPCl32,
				ethMacP.statEthMacRUCPCh32, ethMacP.statEthMacRUCPCl32, ethMacP.statEthMacRMCPCh32, ethMacP.statEthMacRMCPCl32,
				ethMacP.statEthMacRBCPCh32, ethMacP.statEthMacRBCPCl32, ethMacP.statEthMacTUCPCh32, ethMacP.statEthMacTUCPCl32,
				ethMacP.statEthMacTMCPCh32, ethMacP.statEthMacTMCPCl32, ethMacP.statEthMacTBCPCh32, ethMacP.statEthMacTBCPCl32);
			break;

		case GPN_STAT_T_PTN_VS_P_TYPE:
			memcpy(&vsP, (char *)pdata, sizeof(gpnStatPtnVsPData));
			zlog_debug(0, "%s[%d] : VS_P data(%d|%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				vsP.statPtnVsTXFh32, vsP.statPtnVsTXFl32, vsP.statPtnVsTXBh32, vsP.statPtnVsTXBl32,
				vsP.statPtnVsRXFh32, vsP.statPtnVsRXFl32, vsP.statPtnVsRXBh32, vsP.statPtnVsRXBl32);
			break;

		
		case GPN_STAT_T_PTN_LSP_P_TYPE:
			memcpy(&lspP, (char *)pdata, sizeof(gpnStatPtnLspPData));
			zlog_debug(0, "%s[%d] : LSP_P data(%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				lspP.statPtnLspTXFh32, lspP.statPtnLspTXFl32, lspP.statPtnLspTXBh32, lspP.statPtnLspTXBl32,
				lspP.statPtnLspRXFh32, lspP.statPtnLspRXFl32, lspP.statPtnLspRXBh32, lspP.statPtnLspRXBl32,
				lspP.statPtnRevLspTXFh32, lspP.statPtnRevLspTXFl32, lspP.statPtnRevLspTXBh32, lspP.statPtnRevLspTXBl32,
				lspP.statPtnRevLspRXFh32, lspP.statPtnRevLspRXFl32, lspP.statPtnRevLspRXBh32, lspP.statPtnRevLspRXBl32);
			break;

		
		case GPN_STAT_T_PTN_PW_P_TYPE:
			memcpy(&pwP, (char *)pdata, sizeof(gpnStatPtnPwPData));
			zlog_debug(0, "%s[%d] : PW_P data(%d|%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				pwP.statPtnPwTXFh32, pwP.statPtnPwTXFl32, pwP.statPtnPwTXBh32, pwP.statPtnPwTXBl32,
				pwP.statPtnPwRXFh32, pwP.statPtnPwRXFl32, pwP.statPtnPwRXBh32, pwP.statPtnPwRXBl32);
			break;

		
		case GPN_STAT_T_FOLLOW_P_TYPE:
			memcpy(&followP, (char *)pdata, sizeof(gpnStatFollowPData));
			zlog_debug(0, "%s[%d] : FOLLOW_P data(%d|%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				followP.statFollowTXFh32, followP.statFollowTXFl32, followP.statFollowTXBh32, followP.statFollowTXBl32,
				followP.statFollowRXFh32, followP.statFollowRXFl32, followP.statFollowRXBh32, followP.statFollowRXBl32);
			break;
		
				
		case GPN_STAT_T_PWE3_TYPE:
			memcpy(&pwE3, (char *)pdata, sizeof(gpnStatPWE3Data));
			zlog_debug(0, "%s[%d] : PWE3 data(%d|%d|%d|%d)\n", __func__, __LINE__,
				pwE3.statPWE31h32, pwE3.statPWE31l32, pwE3.statPWE32h32, pwE3.statPWE32l32);
			break;

		
		case GPN_STAT_T_PTN_VPLSPW_P_TYPE:
			memcpy(&vplsPwP, (char *)pdata, sizeof(gpnStatPtnVplsPwPData));
			zlog_debug(0, "%s[%d] : VPLSPW_P data(%d|%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				vplsPwP.statPtnVplsPwTXFh32, vplsPwP.statPtnVplsPwTXFl32, vplsPwP.statPtnVplsPwTXBh32, vplsPwP.statPtnVplsPwTXBl32,
				vplsPwP.statPtnVplsPwRXFh32, vplsPwP.statPtnVplsPwRXFl32, vplsPwP.statPtnVplsPwRXBh32, vplsPwP.statPtnVplsPwRXBl32);
			break;

		case GPN_STAT_T_PTN_V_UNI_P_TYPE:
			memcpy(&vUniP, (char *)pdata, sizeof(gpnStatPtnVUniPData));
			zlog_debug(0, "%s[%d] : VUNI data(%d|%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				vUniP.statPtnVUniTXFh32, vUniP.statPtnVUniTXFl32, vUniP.statPtnVUniTXBh32, vUniP.statPtnVUniTXBl32,
				vUniP.statPtnVUniRXFh32, vUniP.statPtnVUniRXFl32, vUniP.statPtnVUniRXBh32, vUniP.statPtnVUniRXBl32);
			break;

		
		case GPN_STAT_T_ETH_SFP_TYPE:
			memcpy(&ethSfpP, (char *)pdata, sizeof(gpnStatEthSFPData));
			zlog_debug(0, "%s[%d] : ETH_SFP data(%d|%d|%d|%d)\n", __func__, __LINE__,
				ethSfpP.statSfpTem, ethSfpP.statSfpTxPow, ethSfpP.statSfpRxPow, ethSfpP.statSfpBias);
			break;

		
		case GPN_STAT_T_POW_TYPE:
			memcpy(&pow, (char *)pdata, sizeof(gpnStatPOWData));
			zlog_debug(0, "%s[%d] : POW data(%d|%d)\n", __func__, __LINE__, pow.statPowVol, pow.statPowPow);
			break;

		
		case GPN_STAT_T_EQU_TYPE:
			memcpy(&equ, (char *)pdata, sizeof(gpnStatEquData));
			zlog_debug(0, "%s[%d] : EQU data(%d|%d)\n", __func__, __LINE__, equ.statEquCpuUtileza, equ.statEquTemp);
			break;
		
				
		case GPN_STAT_T_PTN_VS_M_TYPE:
			memcpy(&vsM, (char *)pdata, sizeof(gpnStatPtnVsMData));
			zlog_debug(0, "%s[%d] : VS_M data(%d|%d|%d|%d)\n", __func__, __LINE__,
				vsM.statPtnVsMDROPRATA, vsM.statPtnVsMDELAY, vsM.statPtnVsMDELAYCHG, vsM.statPtnVsMDRROPRATA);
			break;

		
		case GPN_STAT_T_PTN_LSP_M_TYPE:
			memcpy(&lspM, (char *)pdata, sizeof(gpnStatPtnLspMData));
			zlog_debug(0, "%s[%d] : LSP_M data(%d|%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				lspM.statPtnLspdLDROPRATA, lspM.statPtnLspdDELAY, lspM.statPtnLspdDELAYCHG, lspM.statPtnRevLspdLDROPRATA,
				lspM.statPtnRevLspdDELAY, lspM.statPtnRevLspdDELAYCHG, lspM.statPtnLspdRDROPRATA, lspM.statPtnRevLspdRDROPRATA);
			break;

		case GPN_STAT_T_PTN_PW_M_TYPE:
			memcpy(&pwM, (char *)pdata, sizeof(gpnStatPtnPwMData));
			zlog_debug(0, "%s[%d] : PW_M data(%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				pwM.statPtnPwLDROPRATA, pwM.statPtnPwDELAY, pwM.statPtnPwDELAYCHG, pwM.statPtnPwRDROPRATA, pwM.statPtnPwBWRATA);
			break;
		
				
		case GPN_STAT_T_ETH_MAC_M_TYPE:
			memcpy(&ethMacM, (char *)pdata, sizeof(gpnStatEthMacMData));
			zlog_debug(0, "%s[%d] : ETH_MAC_M data(%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				ethMacM.statEthMacLDROPRATA, ethMacM.statEthMacDELAY, ethMacM.statEthMacDELAYCHG, ethMacM.statEthMacRDROPRATA,
				ethMacM.statEthMacRxBWRATA, ethMacM.statEthMacTxBWRATA);
			break;

		
		case GPN_STAT_T_SDH_SFP_TYPE:
			memcpy(&sdhSfp, (char *)pdata, sizeof(gpnStatSdhSfpData));
			zlog_debug(0, "%s[%d] : SDH_SFP data(%d|%d|%d|%d)\n", __func__, __LINE__,
				sdhSfp.statSfpTem, sdhSfp.statSfpTxPow, sdhSfp.statSfpRxPow, sdhSfp.statSfpBias);
			break;

		case GPN_STAT_T_PDH_SFP_TYPE:
			memcpy(&pdhSfp, (char *)pdata, sizeof(gpnStatPdhSfpData));
			zlog_debug(0, "%s[%d] : PDH_SFP data(%d|%d|%d|%d)\n", __func__, __LINE__,
				pdhSfp.statSfpTem, pdhSfp.statSfpTxPow, pdhSfp.statSfpRxPow, pdhSfp.statSfpBias);
			break;
		
				
		case GPN_STAT_T_ENV_TYPE:
			memcpy(&env, (char *)pdata, sizeof(gpnStatEnvData));
			zlog_debug(0, "%s[%d] : ENV data(%d|%d)\n", __func__, __LINE__,
				env.statEnvHumidity, env.statEnvTemp);
			break;

		
		case GPN_STAT_T_SOFT_TYPE:
			memcpy(&soft, (char *)pdata, sizeof(gpnStatSoftData));
			zlog_debug(0, "%s[%d] : SOFT data(%d|%d)\n", __func__, __LINE__,
				soft.statSoftCpuUtileza, soft.statSoftMemUtileza);
			break;

		case GPN_STAT_T_FOLLOW_M_TYPE:
			memcpy(&followM, (char *)pdata, sizeof(gpnStatFollowMData));
			zlog_debug(0, "%s[%d] : FOLLOW_M data(%d|%d|%d)\n", __func__, __LINE__,
				followM.statFollowDROPRATA, followM.statFollowDELAY, followM.statFollowDELAYCHG);
			break;
		
				
		case GPN_STAT_T_PTN_VPLSPW_M_TYPE:
			memcpy(&vplsPwM, (char *)pdata, sizeof(gpnStatPtnVplsPwMData));
			zlog_debug(0, "%s[%d] : VPLSPW_M data(%d|%d|%d|%d)\n", __func__, __LINE__,
				vplsPwM.statPtnVplsPwLDROPRATA, vplsPwM.statPtnVplsPwDELAY, vplsPwM.statPtnVplsPwDELAYCHG,
				vplsPwM.statPtnVplsPwRDROPRATA);
			break;

		
		case GPN_STAT_T_PTN_MEP_M_TYPE:
			memcpy(&mepM, (char *)pdata, sizeof(gpnStatPtnMepMData));
			zlog_debug(0, "%s[%d] : MEP_M data(%d|%d|%d|%d)\n", __func__, __LINE__,
				mepM.statPtnMepLDROPRATA, mepM.statPtnMepRDROPRATA, mepM.statPtnMepDELAY, mepM.statPtnMepDELAYCHG);
			break;

		default:
			break;
	}
	zlog_debug(0, "\r\n\r\n");
}


/* add by lipf, to calc average, 2018/3/27 */
void gpnStatCalcCountAverage_32(UINT32 *pbase, UINT32 *pcurr, UINT32 *pdata, UINT32 dataCnt)
{
	UINT32 average_old 	= 0;
	UINT32 average_new 	= 0;
	UINT32 data_new		= 0;

	average_old = *pcurr;

	if((*pdata) >= (*pbase))
	{
		data_new = (*pdata) - (*pbase);
	}
	else
	{
		data_new = 0;
	}

	average_new = (average_old * (dataCnt - 1) + data_new)/dataCnt;
	*pcurr = average_new;
}

void gpnStatCalcMeasureAverage_32(UINT32 *pcurr, UINT32 *pdata, UINT32 dataCnt)
{
	UINT32 average_old 	= *pcurr;
	UINT32 average_new 	= 0;
	UINT32 data_new		= *pdata;

	average_new = (average_old * (dataCnt - 1) + data_new)/dataCnt;
	*pdata = average_new;
}



/* add by lipf, to calc average, 2018/3/27 */
void gpnStatCalcCountAverage_64(stGpnStatGen64bit *pbase, stGpnStatGen64bit *pcurr, stGpnStatGen64bit *pdata, UINT32 dataCnt)
{
	stGpnStatGen64bit average_old;
	stGpnStatGen64bit average_new;
	stGpnStatGen64bit data_new;

	memset(&average_old, 0, sizeof(stGpnStatGen64bit));
	memset(&average_new, 0, sizeof(stGpnStatGen64bit));
	memset(&data_new, 0, sizeof(stGpnStatGen64bit));

	if((pdata->h > pbase->h) || ((pdata->h == pbase->h) && (pdata->l >= pbase->l)))
	{
		if(pdata->l >= pbase->l)
		{
			data_new.h = (pdata->h - pbase->h);
			data_new.l = (pdata->l - pbase->l);			
		}
		else
		{
			data_new.h = (pdata->h - pbase->h -1);
			data_new.l = (0xffffffff - pbase->l + pdata->h);
		}
	}
	else
	{
		data_new.h = 0;
		data_new.l = 0;
	}
}

void gpnStatMeasureCountAverage_64(stGpnStatGen64bit *pbase, stGpnStatGen64bit *pcurr, stGpnStatGen64bit *pdata, UINT32 dataCnt)
{
}




UINT32 gpnStatDataCollectProc(optObjOrient *pPeerIndex, UINT32 statType, void *pdata, UINT32 len)
{
	/*peerPortNode */
	stStatPeerPortNode *pStatPeerNode;
	stStatScanPortInfo *pStatScanPort;
	/*scanTypetNode */
	stStatScanTypeDef *pscanTypeNode;
	/*statTypeNode */
	stStatTpNode *pstatTpNode;
	UINT32 statTpNum;
	
	UINT32 subTpStNum;
	UINT32 dataOff;
	
	UINT32 dataSize;
	UINT32 subTpNum;
	
	/*assert */
	if( (pPeerIndex == NULL) ||\
		(pdata == NULL) )
	{
		return GPN_STAT_COLL_GEN_ERR;
	}
	
	/*fine pStatPeerNode*/
	pStatPeerNode = NULL;
	gpnStatSeekPeerPort2PeerPortNode(pPeerIndex, &pStatPeerNode);
	if(pStatPeerNode == NULL)
	{
		GPN_STAT_COLL_PRINT(GPN_STAT_COLL_CMP, "%s : peer port(%08x-%08x|%08x|%08x|%08x|%08x) find err!\n\r",\
			__FUNCTION__, pPeerIndex->devIndex, pPeerIndex->portIndex,\
			pPeerIndex->portIndex3, pPeerIndex->portIndex4,\
			pPeerIndex->portIndex5, pPeerIndex->portIndex6);
		return GPN_STAT_COLL_GEN_ERR;
	}
	pStatScanPort = pStatPeerNode->pStatScanPort;
	if(pStatScanPort->preQuenValid != GPN_STAT_COLL_GEN_ENABLE)
	{
		return GPN_STAT_COLL_GEN_ERR;
	}
	pscanTypeNode = pStatScanPort->pscanTypeNode;
	
	/*find statTypeNode */
	subTpNum = 0;
	dataSize = 0;
	
	dataOff = 0;
	subTpStNum = 0;
	
	statTpNum = 0;
	pstatTpNode = (stStatTpNode *)listFirst(&(pscanTypeNode->statTpsOfScanTpQuen));
	while((pstatTpNode!=NULL)&&(statTpNum<pscanTypeNode->statTpNumInScanType))
	{
		if(pstatTpNode->pStatTpStr->statType == statType)
		{
			debug_show_data(statType, pdata);
			dataSize = pstatTpNode->pStatTpStr->statTpSpSize;
			subTpNum = pstatTpNode->pStatTpStr->subStatNumInType;
			break;
		}
			
		dataOff += pstatTpNode->pStatTpStr->statTpSpSize;
		subTpStNum += pstatTpNode->pStatTpStr->subStatNumInType;
		
		statTpNum++;
		pstatTpNode = (stStatTpNode *)listNext((NODE *)pstatTpNode);
	}
	if((pstatTpNode==NULL)||(statTpNum>=pscanTypeNode->statTpNumInScanType))
	{
		GPN_STAT_COLL_PRINT(GPN_STAT_COLL_CMP, "%s : statType(%08x) find err!\n\r",\
			__FUNCTION__, statType);
		return GPN_STAT_COLL_GEN_ERR;
	}

	GPN_STAT_COLL_PRINT(GPN_STAT_COLL_CMP, "%s : dataOff(%d) dataBitDeep(%d) subTpS(%d) subNum(%d) dataSize(%d)\n\r",\
		__FUNCTION__, dataOff, pStatScanPort->dataBitDeep, subTpStNum, subTpNum, dataSize);
	if(pStatScanPort->scanQuenValid == GPN_STAT_COLL_GEN_ENABLE)
	{
		if(pStatScanPort->isBaseInit & (1U<<statTpNum))
		{
			pStatScanPort->isBaseInit &= (~(1U<<statTpNum));

			/*add by lipf, to record count of getting data, then to calc average*/
			pStatScanPort->currDataCnt = 0;
			
			if(pStatScanPort->statBaseDate != NULL)
			{
				memcpy(((UINT8 *)(pStatScanPort->statBaseDate) + dataOff), pdata, dataSize);
			}
			if(pStatScanPort->statCurrDate != NULL)
			{
				memset(((UINT8 *)(pStatScanPort->statCurrDate) + dataOff), 0, dataSize);
			}
			if( (pStatScanPort->statLCycRec != NULL) &&\
				(pStatScanPort->statLCycRec->en == GPN_STAT_COLL_GEN_ENABLE) )
			{
				memset(((UINT8 *)(pStatScanPort->statLCycRec->xCycCurrDate) + dataOff), 0, dataSize);
				pStatScanPort->statLCycRec->dataValid = GPN_STAT_COLL_GEN_DISABLE;
			}
			if( (pStatScanPort->statSCycRec != NULL) &&\
				(pStatScanPort->statSCycRec->en == GPN_STAT_COLL_GEN_ENABLE) )
			{
				memset(((UINT8 *)(pStatScanPort->statSCycRec->xCycCurrDate) + dataOff), 0, dataSize);
				pStatScanPort->statSCycRec->dataValid = GPN_STAT_COLL_GEN_DISABLE;
			}
			if( (pStatScanPort->statUCycRec != NULL) &&\
				(pStatScanPort->statUCycRec->en == GPN_STAT_COLL_GEN_ENABLE) )
			{
				memset(((UINT8 *)(pStatScanPort->statUCycRec->xCycCurrDate) + dataOff), 0, dataSize);
				pStatScanPort->statUCycRec->dataValid = GPN_STAT_COLL_GEN_DISABLE;
			}
		}
		else
		{
			/*add by lipf, to record count of getting data, then to calc average*/
			pStatScanPort->currDataCnt++;
			
			/*stat data to curr data */
			if(pStatScanPort->currMonEn == GPN_STAT_COLL_GEN_ENABLE)
			{
				if(pStatScanPort->statBaseDate != NULL)
				{
					gpnStatDataCollectBase2CurrProc(((UINT8 *)(pStatScanPort->statBaseDate)+dataOff),\
						((UINT8 *)(pStatScanPort->statCurrDate)+dataOff), pdata,\
						pStatScanPort->dataBitDeep>>subTpStNum,\
						pStatScanPort->valueType>>subTpStNum,\
						subTpNum,
						pStatScanPort->currDataCnt);
				}
			}
				
			/*stat data to longCyc base data */
			if( (pStatScanPort->statLCycRec != NULL) &&\
				(pStatScanPort->statLCycRec->en == GPN_STAT_COLL_GEN_ENABLE) )
			{
				if(pStatScanPort->statBaseDate != NULL)
				{
					gpnStatDataCollectBase2CurrProc(((UINT8 *)(pStatScanPort->statBaseDate)+dataOff),\
						((UINT8 *)(pStatScanPort->statLCycRec->xCycCurrDate)+dataOff), pdata,\
						pStatScanPort->statLCycRec->dataBitDeep>>subTpStNum,\
						pStatScanPort->statLCycRec->valueType>>subTpStNum,\
						subTpNum,
						pStatScanPort->currDataCnt);
					
					pStatScanPort->statLCycRec->dataValid = GPN_STAT_COLL_GEN_ENABLE;
				}
			}

			/*stat data to shortCyc base data */
			if( (pStatScanPort->statSCycRec != NULL) &&\
				(pStatScanPort->statSCycRec->en == GPN_STAT_COLL_GEN_ENABLE) )
			{
				if(pStatScanPort->statBaseDate != NULL)
				{
					gpnStatDataCollectBase2CurrProc(((UINT8 *)(pStatScanPort->statBaseDate)+dataOff),\
						((UINT8 *)(pStatScanPort->statSCycRec->xCycCurrDate)+dataOff), pdata,\
						pStatScanPort->statSCycRec->dataBitDeep>>subTpStNum,\
						pStatScanPort->statSCycRec->valueType>>subTpStNum,\
						subTpNum,
						pStatScanPort->currDataCnt);

					pStatScanPort->statSCycRec->dataValid = GPN_STAT_COLL_GEN_ENABLE;
				}
			}
		
			/*stat data to userDefCyc base data */
			if( (pStatScanPort->statUCycRec != NULL) &&\
				(pStatScanPort->statUCycRec->en == GPN_STAT_COLL_GEN_ENABLE) )
			{
				if(pStatScanPort->statBaseDate != NULL)
				{
					gpnStatDataCollectBase2CurrProc(((UINT8 *)(pStatScanPort->statBaseDate)+dataOff),\
						((UINT8 *)(pStatScanPort->statUCycRec->xCycCurrDate)+dataOff), pdata,\
						pStatScanPort->statUCycRec->dataBitDeep>>subTpStNum,\
						pStatScanPort->statUCycRec->valueType>>subTpStNum,\
						subTpNum,
						pStatScanPort->currDataCnt);

					pStatScanPort->statUCycRec->dataValid = GPN_STAT_COLL_GEN_ENABLE;
				}
			}
			
			/*re-new base data */
			if(pStatScanPort->statBaseDate != NULL)
			{
				memcpy(((UINT8 *)(pStatScanPort->statBaseDate) + dataOff), pdata, dataSize);
			}
		}
	}
			
	return GPN_STAT_COLL_GEN_OK;
}


UINT32 gpnStatDataCollectBase2CurrProc(void *pbase,
	void *pcurr, void *pdata, UINT32 bitDeep, UINT32 vType, UINT32 subNum, UINT32 dataCnt)
{
	UINT32 i;
	stGpnStatGen64bit *pbase64bit;
	/*stGpnStatGen32bit *pbase32bit;*/
	stGpnStatGen64bit *pcurr64bit;
	/*stGpnStatGen32bit *pcurr32bit;*/
	stGpnStatGen64bit *pdata64bit;
	/*stGpnStatGen32bit *pdata32bit;*/
	
	/*assert */
	if( (pbase == NULL) ||\
		(pcurr == NULL) ||\
		(pdata == NULL) )
	{
		return GPN_STAT_COLL_GEN_ERR;
	}


	for(i=0;i<subNum;i++)
	{
		if( (bitDeep & (1U<<i)) &&\
			(vType & (1U<<i)) )
		{
			/*64bit counter type */
			pbase64bit = (stGpnStatGen64bit *)pbase;
			pcurr64bit = (stGpnStatGen64bit *)pcurr;
			pdata64bit = (stGpnStatGen64bit *)pdata;

			GPN_STAT_XADD_64_UINT(pdata64bit, pbase64bit, pcurr64bit);

			pbase = (void *)(pbase64bit + 1);
			pcurr = (void *)(pcurr64bit + 1);
			pdata = (void *)(pdata64bit + 1);
		}
		else if((!(bitDeep & (1U<<i))) &&\
				(vType & (1U<<i)))
		{
			/*32bit counter type */
			GPN_STAT_XADD_32_UNIT(pdata, pbase, pcurr);

			pbase = (void *)((UINT32 *)pbase + 1);
			pcurr = (void *)((UINT32 *)pcurr + 1);
			pdata = (void *)((UINT32 *)pdata + 1);
		}
		else if((bitDeep & (1U<<i)) &&\
				(!(vType & (1U<<i))))
		{
			/*64bit scale type */
			pbase64bit = (stGpnStatGen64bit *)pbase;
			pcurr64bit = (stGpnStatGen64bit *)pcurr;
			pdata64bit = (stGpnStatGen64bit *)pdata;

			pcurr64bit->h = pdata64bit->h;
			pcurr64bit->l = pdata64bit->l;
			
			pbase = (void *)(pbase64bit + 1);
			pcurr = (void *)(pcurr64bit + 1);
			pdata = (void *)(pdata64bit + 1);
		}
		else
		{
			/*32bit scale type */
			//gpnStatCalcMeasureAverage_32((UINT32 *)pcurr, (UINT32 *)pdata, dataCnt);
			*(UINT32 *)pcurr = *(UINT32 *)pdata;			

			pbase = (void *)((UINT32 *)pbase + 1);
			pcurr = (void *)((UINT32 *)pcurr + 1);
			pdata = (void *)((UINT32 *)pdata + 1);
		}
	}
	
	return GPN_STAT_COLL_GEN_OK;
}

UINT32 gpnStatDataCollectDebug(optObjOrient *pPeerIndex, UINT32 statType)
{
	/*peerPortNode */
	stStatPeerPortNode *pStatPeerNode;
	stStatScanPortInfo *pStatScanPort;
	/*scanTypetNode */
	stStatScanTypeDef *pscanTypeNode;
	/*statTypeNode */
	stStatTpNode *pstatTpNode;
	UINT32 statTpNum;
	
	UINT32 subTpStNum;
	UINT32 dataOff;
	
	UINT32 dataSize;
	UINT32 subTpNum;
	
	/*assert */
	if(pPeerIndex == NULL)
	{
		return GPN_STAT_COLL_GEN_ERR;
	}
	
	/*fine pStatPeerNode*/
	pStatPeerNode = NULL;
	gpnStatSeekPeerPort2PeerPortNode(pPeerIndex, &pStatPeerNode);
	if(pStatPeerNode == NULL)
	{
		GPN_STAT_COLL_PRINT(GPN_STAT_COLL_CMP, "%s : peer port(%08x-%08x|%08x|%08x|%08x|%08x) find err!\n\r",\
			__FUNCTION__, pPeerIndex->devIndex, pPeerIndex->portIndex,\
			pPeerIndex->portIndex3, pPeerIndex->portIndex4,\
			pPeerIndex->portIndex5, pPeerIndex->portIndex6);
		return GPN_STAT_COLL_GEN_ERR;
	}
	pStatScanPort = pStatPeerNode->pStatScanPort;
	if(pStatScanPort->preQuenValid != GPN_STAT_COLL_GEN_ENABLE)
	{
		return GPN_STAT_COLL_GEN_ERR;
	}
	pscanTypeNode = pStatScanPort->pscanTypeNode;
	
	/*find statTypeNode */
	subTpNum = 0;
	dataSize = 0;
	
	dataOff = 0;
	subTpStNum = 0;
	
	statTpNum = 0;
	pstatTpNode = (stStatTpNode *)listFirst(&(pscanTypeNode->statTpsOfScanTpQuen));
	while((pstatTpNode!=NULL)&&(statTpNum<pscanTypeNode->statTpNumInScanType))
	{
		if(pstatTpNode->pStatTpStr->statType == statType)
		{
			dataSize = pstatTpNode->pStatTpStr->statTpSpSize;
			subTpNum = pstatTpNode->pStatTpStr->subStatNumInType;
			break;
		}
			
		dataOff += pstatTpNode->pStatTpStr->statTpSpSize;
		subTpStNum += pstatTpNode->pStatTpStr->subStatNumInType;
		
		statTpNum++;
		pstatTpNode = (stStatTpNode *)listNext((NODE *)pstatTpNode);
	}
	if((pstatTpNode==NULL)||(statTpNum>=pscanTypeNode->statTpNumInScanType))
	{
		GPN_STAT_COLL_PRINT(GPN_STAT_COLL_CMP, "%s : statType(%08x) find err!\n\r",\
			__FUNCTION__, statType);
		return GPN_STAT_COLL_GEN_ERR;
	}

	printf("%s : dataOff(%d) bitDeep(%08x) subTpS(%d) subNum(%d) dataSize(%d)\n\r",\
		__FUNCTION__, dataOff, pStatScanPort->dataBitDeep, subTpStNum, subTpNum, dataSize);
	if(pStatScanPort->scanQuenValid == GPN_STAT_COLL_GEN_ENABLE)
	{
		printf("peer port(%08x-%08x|%08x|%08x|%08x|%08x) statType(%08x)\n\r",\
				pPeerIndex->devIndex, pPeerIndex->portIndex,\
				pPeerIndex->portIndex3, pPeerIndex->portIndex4,\
				pPeerIndex->portIndex5, pPeerIndex->portIndex6,\
				statType);
		/*stat data to curr data */
		if(pStatScanPort->currMonEn == GPN_STAT_COLL_GEN_ENABLE)
		{
			printf("port base curr data:\n\r");
			gpnStatDataCollectBaseAndCurrPrintDebug(((UINT8 *)(pStatScanPort->statBaseDate)+dataOff),\
				((UINT8 *)(pStatScanPort->statCurrDate)+dataOff),\
				pStatScanPort->dataBitDeep>>subTpStNum,\
				subTpNum);
		}

		/*stat data to longCyc base data */
		if( (pStatScanPort->statLCycRec != NULL) &&\
			(pStatScanPort->statLCycRec->en == GPN_STAT_COLL_GEN_ENABLE) )
		{
			printf("port base longCyc data:\n\r");
			gpnStatDataCollectBaseAndCurrPrintDebug(((UINT8 *)(pStatScanPort->statBaseDate)+dataOff),\
				((UINT8 *)(pStatScanPort->statLCycRec->xCycCurrDate)+dataOff),\
				pStatScanPort->statLCycRec->dataBitDeep>>subTpStNum,\
				subTpNum);
		}

		/*stat data to shortCyc base data */
		if( (pStatScanPort->statSCycRec != NULL) &&\
			(pStatScanPort->statSCycRec->en == GPN_STAT_COLL_GEN_ENABLE) )
		{
			printf("port base shortCyc data:\n\r");
			gpnStatDataCollectBaseAndCurrPrintDebug(((UINT8 *)(pStatScanPort->statBaseDate)+dataOff),\
				((UINT8 *)(pStatScanPort->statSCycRec->xCycCurrDate)+dataOff),\
				pStatScanPort->statSCycRec->dataBitDeep>>subTpStNum,\
				subTpNum);
		}

		/*stat data to userDefCyc base data */
		if( (pStatScanPort->statUCycRec != NULL) &&\
			(pStatScanPort->statUCycRec->en == GPN_STAT_COLL_GEN_ENABLE) )
		{
			printf("port base userDefCyc data:\n\r");
			gpnStatDataCollectBaseAndCurrPrintDebug(((UINT8 *)(pStatScanPort->statBaseDate)+dataOff),\
				((UINT8 *)(pStatScanPort->statUCycRec->xCycCurrDate)+dataOff),\
				pStatScanPort->statUCycRec->dataBitDeep>>subTpStNum,\
				subTpNum);
		}
	}
			
	return GPN_STAT_COLL_GEN_OK;
}
UINT32 gpnStatDataCollectBaseAndCurrPrintDebug(void *pbase,
	void *pcurr, UINT32 bitDeep, UINT32 subNum)
{
	UINT32 i;
	stGpnStatGen64bit *pcurr64bit;
	/*stGpnStatGen32bit *pcurr32bit;*/
	stGpnStatGen64bit *pbase64bit;
	/*stGpnStatGen32bit *pbase32bit;*/
	
	/*assert */
	if( (pbase == NULL) ||\
		(pcurr == NULL) )
	{
		return GPN_STAT_COLL_GEN_ERR;
	}


	for(i=0;i<subNum;i++)
	{
		if(bitDeep & (1U<<i))
		{
			pbase64bit = (stGpnStatGen64bit *)pbase;
			pcurr64bit = (stGpnStatGen64bit *)pcurr;

			printf("base(%08x %08x) curr(%08x %08x)\n\r",\
				pbase64bit->h, pbase64bit->l,\
				pcurr64bit->h, pcurr64bit->l);

			pbase = (void *)(pbase64bit + 1);
			pcurr = (void *)(pcurr64bit + 1);
		}
		else
		{
			printf("base(        %08x ) curr(         %08x)\n\r",\
				*(UINT32 *)pbase, *(UINT32 *)pcurr);
			
			pbase = (void *)((UINT32 *)pbase + 1);
			pcurr = (void *)((UINT32 *)pcurr + 1);
		}
	}
	
	return GPN_STAT_COLL_GEN_OK;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_STAT_COLLECT_PROC_C_*/


