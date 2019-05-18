/**********************************************************
* file name: gpnStatFuncApi.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-25
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_FUNC_API_C_
#define _GPN_STAT_FUNC_API_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdlib.h>

#include <lib/syslog.h>
#include <lib/log.h>

#include "socketComm/gpnSockStatMsgDef.h"

#include "gpnStatFuncApi.h"
#include "gpnStatCollectProc.h"
#include "devCoreInfo/gpnDevCoreInfoFunApi.h"

#include "gpnLog/gpnLogFuncApi.h"
#include "gpnStatDebug.h"


/* to put equ data recv from hal and system together */
static UINT32 gpnStatTogetherEquData(gpnSockMsg *pgpnStatMsgSp)
{
	static UINT32 statEquCpuUtileza = 0;
	static UINT32 statEquTemp = 276;

	gpnStatEquData *pData = NULL;
	gpnStatEquData dataTemp;
	memset(&dataTemp, 0, sizeof(gpnStatEquData));

	
	if((GPN_STAT_MSG_EQU_STAT_GET_RSP == pgpnStatMsgSp->iMsgType) \
		&& (sizeof(gpnStatEquData) == pgpnStatMsgSp->msgCellLen))
	{
		pData = (gpnStatEquData *)(&(pgpnStatMsgSp->msgCellLen) + 1);
		if(0xffffffff == pData->statEquCpuUtileza)
		{
			dataTemp.statEquCpuUtileza = statEquCpuUtileza;
			dataTemp.statEquTemp = pData->statEquTemp;
			statEquTemp = pData->statEquTemp;
		}
		else if(0xffffffff == pData->statEquTemp)
		{
			dataTemp.statEquCpuUtileza = pData->statEquCpuUtileza;
			statEquCpuUtileza = pData->statEquCpuUtileza;
			dataTemp.statEquTemp = statEquTemp;
		}
		else
		{
			return GPN_STAT_API_GEN_ERR;
		}

		/* renew data of pgpnStatMsgSp */
		memcpy((&(pgpnStatMsgSp->msgCellLen) + 1), &dataTemp, sizeof(gpnStatEquData));
	}	
		
	return GPN_STAT_API_GEN_OK;
}



UINT32 gpnStatCorePorc(gpnSockMsg *pgpnStatMsgSp, UINT32 len)
{
	UINT32 reval;
	UINT32 msgMainType;
	
	GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : recive msgType(%08x), len(%d)\n\r",
		__FUNCTION__, pgpnStatMsgSp->iMsgType, len);

	zlog_debug(STATIS_DBG_DATA_COLLECT, "%s[%d] : statis recive gpnSocket data\n\r",
		__FUNCTION__, __LINE__);

	reval = GPN_STAT_API_GEN_ERR;
	if((pgpnStatMsgSp->iMsgType & GPN_SOCK_MSG_OWNER_BIT) == GPN_SOCK_OWNER_STAT)
	{
		/* sata msg type */
		msgMainType = pgpnStatMsgSp->iMsgType & GPN_SOCK_MSG_MAIN_TYPE_BIT;
		zlog_debug(STATIS_DBG_DATA_COLLECT, "%s[%d] : statis recive gpnSocket collect data\n\r", __FUNCTION__, __LINE__);
		switch(msgMainType)
		{
			case GPN_STAT_MT_GLOBAL_OPT:
				reval = gpnStatGlobalPorc(pgpnStatMsgSp, len);
				break;

			case GPN_STAT_MT_DATA_COLLECT_OPT:
				reval = gpnStatCollectPorc(pgpnStatMsgSp, len);
				break;
				
			default:
				break;
		}
	}
	else
	{
		/*other module msg type */
		switch(pgpnStatMsgSp->iMsgType)
		{
			case GPN_IFM_MSG_DEV_STATUS_CHANG:
				GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : device(%08x), devSta(%08x)\n\r",\
					__FUNCTION__, pgpnStatMsgSp->iMsgPara1, pgpnStatMsgSp->iMsgPara8);
				gpnLog(GPN_LOG_L_INFO, "%s : device(%08x), devSta(%08x)\n\r",\
					__FUNCTION__, pgpnStatMsgSp->iMsgPara1, pgpnStatMsgSp->iMsgPara8);
				reval = gpnStatFuncApiDevStausNotify(pgpnStatMsgSp->iMsgPara1, pgpnStatMsgSp->iMsgPara8);
				break;
				
			case GPN_IFM_MSG_FIX_PORT_REG:
				GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : device(%08x), fixInfoLen(%d), fixUnitSize(%d)\n\r",\
					__FUNCTION__, pgpnStatMsgSp->iMsgPara1, pgpnStatMsgSp->msgCellLen, sizeof(stStatPortObjCL));

				/* modify for ipran */
				//reval =  gpnStatFuncApiFixPortRegist(pgpnStatMsgSp, len); 
				
				gpnLog(GPN_LOG_L_INFO, "%s : device(%08x), fixInfoLen(%d), fixUnitSize(%d) reval(%d)\n\r",\
					__FUNCTION__, pgpnStatMsgSp->iMsgPara1, pgpnStatMsgSp->msgCellLen,\
					sizeof(stStatPortObjCL), reval);
				break;
				
			case GPN_IFM_MSG_VARI_PORT_REG:
				GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : pid(%d) reg local port(%08x-%08x|%08x|%08x|%08x|%08x)!\n\r",\
					__FUNCTION__, pgpnStatMsgSp->iMsgPara8, pgpnStatMsgSp->iMsgPara7,\
					pgpnStatMsgSp->iMsgPara2, pgpnStatMsgSp->iMsgPara3,\
					pgpnStatMsgSp->iMsgPara4, pgpnStatMsgSp->iMsgPara5,\
					pgpnStatMsgSp->iMsgPara6);
				gpnLog(GPN_LOG_L_INFO, "%s : pid(%d) reg local port(%08x-%08x|%08x|%08x|%08x|%08x)!\n\r",\
					__FUNCTION__, pgpnStatMsgSp->iMsgPara8, pgpnStatMsgSp->iMsgPara7,\
					pgpnStatMsgSp->iMsgPara2, pgpnStatMsgSp->iMsgPara3,\
					pgpnStatMsgSp->iMsgPara4, pgpnStatMsgSp->iMsgPara5,\
					pgpnStatMsgSp->iMsgPara6);
				zlog_debug(STATIS_DBG_REGISTER, "%s[%d] : recv port reg msg, local port(%08x-%08x|%08x|%08x|%08x|%08x)!\n\r",\
					__FUNCTION__, __LINE__, pgpnStatMsgSp->iMsgPara1,\
					pgpnStatMsgSp->iMsgPara2, pgpnStatMsgSp->iMsgPara3,\
					pgpnStatMsgSp->iMsgPara4, pgpnStatMsgSp->iMsgPara5,\
					pgpnStatMsgSp->iMsgPara6);
				reval = gpnStatFuncApiVariPortRegist(pgpnStatMsgSp, len);
				break;

			case GPN_IFM_MSG_VARI_PORT_UNREG:
				GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : pid(%d) unReg local port(%08x-%08x|%08x|%08x|%08x|%08x)!\n\r",\
					__FUNCTION__, pgpnStatMsgSp->iMsgPara8, pgpnStatMsgSp->iMsgPara7,\
					pgpnStatMsgSp->iMsgPara2, pgpnStatMsgSp->iMsgPara3,\
					pgpnStatMsgSp->iMsgPara4, pgpnStatMsgSp->iMsgPara5,\
					pgpnStatMsgSp->iMsgPara6);
				gpnLog(GPN_LOG_L_INFO, "%s : pid(%d) unReg local port(%08x-%08x|%08x|%08x|%08x|%08x)!\n\r",\
					__FUNCTION__, pgpnStatMsgSp->iMsgPara8, pgpnStatMsgSp->iMsgPara7,\
					pgpnStatMsgSp->iMsgPara2, pgpnStatMsgSp->iMsgPara3,\
					pgpnStatMsgSp->iMsgPara4, pgpnStatMsgSp->iMsgPara5,\
					pgpnStatMsgSp->iMsgPara6);
				reval = gpnStatFuncApiVariPortUnRegist(pgpnStatMsgSp, len);
				break;

			case GPN_IFM_MSG_PEER_PORT_REPLACE:
				/* peer port replace use peer port */
				gpnStatFuncApiPeerPortReplaceNotify(pgpnStatMsgSp, len);
				break;
			
			case GPN_IFM_MSG_VIEW_PORT_REPLACE:
				/* old view port replace use view port */
				gpnStatFuncApiViewPortReplaceNotify(pgpnStatMsgSp, len);
				break;
			
				
			default:
				break;
		}
	}

	return reval;
}
UINT32 gpnStatGlobalPorc(gpnSockMsg *pgpnStatMsgSp, UINT32 len)
{
	UINT32 reval;
	objLogicDesc localPIndex;

	GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : recive msgType(%08x), len(%d)\n\r",
		__FUNCTION__, pgpnStatMsgSp->iMsgType, len);
	
	/*in GPN_STAT, req DA creat devIndex */
	//devIndex = DeviceIndex_Create(1, 0);
	
	/*assert*/
	/*assert do not do */
	reval = GPN_STAT_API_GEN_ERR;
	switch(pgpnStatMsgSp->iMsgType)
	{
		/*************************** Scalar **************************/
		case GPN_STAT_MSG_TIME_POLARIT_GET:
			break;	
		case GPN_STAT_MSG_TIME_OFFSET_GET:
			break;
		case GPN_STAT_MSG_MAX_OBJ_SRC_GET:
			break;
		case GPN_STAT_MSG_USE_OBJ_SRC_GET:
			break;
		case GPN_STAT_MSG_LAST_DATA_REPT_GET:
			break;
			
		case GPN_STAT_MSG_LAST_DATA_REPT_SET:
			reval = gpnStatLastHistReportCfgSet(pgpnStatMsgSp->iMsgPara1);
			break;
			
		case GPN_STAT_MSG_OLD_DATA_REPT_GET:
			break;
			
		case GPN_STAT_MSG_OLD_DATA_REPT_SET:
			reval = gpnStatOldHistReportCfgSet(pgpnStatMsgSp->iMsgPara1);
			break;
			
		case GPN_STAT_MSG_ALARM_REPT_GET:
			break;
			
		case GPN_STAT_MSG_ALARM_REPT_SET:
			reval = gpnStatAlarmReportCfgSet(pgpnStatMsgSp->iMsgPara1);
			break;
			
		case GPN_STAT_MSG_EVENT_REPT_GET:
			break;
			
		case GPN_STAT_MSG_EVENT_REPT_SET:
			reval = gpnStatEvnetReportCfgSet(pgpnStatMsgSp->iMsgPara1);
			break;
			
		case GPN_STAT_MSG_TASK_ID_GET:
			break;
		case GPN_STAT_MSG_EVN_THRED_TP_ID_GET:
			break;
		case GPN_STAT_MSG_ALM_THRED_TP_ID_GET:
			break;
		case GPN_STAT_MSG_SUB_FILT_TP_ID_GET:
			break;
		case GPN_STAT_MSG_SUB_REPORT_TP_ID_GET:
			break;
		case GPN_STAT_MSG_COUNT_TYPE_GET:
			break;
			
		/********************* Table ****************************/
		case GPN_STAT_MSG_STAT_TYPE_GET:
			break;
			
		case GPN_STAT_MSG_STAT_TYPE_GET_NEXT:
			break;
			
		case GPN_STAT_MSG_SUB_STAT_TYPE_GET:
			break;
			
		case GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT:
			break;

		case GPN_STAT_MSG_SUB_STAT_TYPE_MODIFY:
			reval = gpnStatFuncApiSubTypeThredModify(pgpnStatMsgSp->iMsgPara1,\
				(stSubTypeThred *)(&(pgpnStatMsgSp->msgCellLen)+1), pgpnStatMsgSp->msgCellLen);
			break;

		case GPN_STAT_MSG_CURR_DATA_MODIFY:
			reval = gpnStatFuncApiCurrStatModify((objLogicDesc *)(&(pgpnStatMsgSp->iMsgPara1)), \
				pgpnStatMsgSp->iMsgPara7, pgpnStatMsgSp->iMsgParaB);
			break;
			
		case GPN_STAT_MSG_TASK_ADD:
			reval = gpnStatFuncApiTaskAdd(pgpnStatMsgSp->iMsgPara1, pgpnStatMsgSp->iMsgPara2,\
				pgpnStatMsgSp->iMsgPara3, pgpnStatMsgSp->iMsgPara4, pgpnStatMsgSp->iMsgPara5,\
				pgpnStatMsgSp->iMsgPara6, pgpnStatMsgSp->iMsgPara7, pgpnStatMsgSp->iMsgPara8,\
				pgpnStatMsgSp->iMsgPara9);
			break;
			
		case GPN_STAT_MSG_TASK_DELETE:
			reval = gpnStatFuncApiTaskDelete(pgpnStatMsgSp->iMsgPara1);
			break;

		case GPN_STAT_MSG_TASK_MODIFY:
			reval = gpnStatFuncApiTaskModify(pgpnStatMsgSp->iMsgPara1,\
				pgpnStatMsgSp->iMsgPara5, pgpnStatMsgSp->iMsgPara6, pgpnStatMsgSp->iMsgPara7,\
				pgpnStatMsgSp->iMsgPara8, pgpnStatMsgSp->iMsgPara9);
			break;
			
		case GPN_STAT_MSG_TASK_GET:
			break;
		case GPN_STAT_MSG_TASK_GET_NEXT:
			break;
			
		case GPN_STAT_MSG_EVN_THRED_TEMP_ADD:
			reval = gpnStatFuncApiEvnThredTpAdd(pgpnStatMsgSp->iMsgPara1,\
				pgpnStatMsgSp->iMsgPara2, pgpnStatMsgSp->iMsgPara3, pgpnStatMsgSp->iMsgPara4,\
				pgpnStatMsgSp->iMsgPara5, pgpnStatMsgSp->iMsgPara6, pgpnStatMsgSp->iMsgPara7);
			break;
			
		case GPN_STAT_MSG_EVN_THRED_TEMP_DELETE:
			reval = gpnStatFuncApiEvnThredTpDelete(pgpnStatMsgSp->iMsgPara1);
			break;
			
		case GPN_STAT_MSG_EVN_THRED_TEMP_MODIFY:
			reval = gpnStatFuncApiEvnThredTpModify(pgpnStatMsgSp->iMsgPara1,\
				pgpnStatMsgSp->iMsgPara2, pgpnStatMsgSp->iMsgPara4, pgpnStatMsgSp->iMsgPara5,\
				pgpnStatMsgSp->iMsgPara6, pgpnStatMsgSp->iMsgPara7);
			break;

		case GPN_STAT_MSG_EVN_THRED_TEMP_GET:
			break;
		case GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT:
			break;
			
		case GPN_STAT_MSG_ALM_THRED_TEMP_ADD:
			reval = gpnStatFuncApiAlmThredTpAdd(pgpnStatMsgSp->iMsgPara1,\
				pgpnStatMsgSp->iMsgPara2, pgpnStatMsgSp->iMsgPara3, pgpnStatMsgSp->iMsgPara4,\
				pgpnStatMsgSp->iMsgPara5, pgpnStatMsgSp->iMsgPara6, pgpnStatMsgSp->iMsgPara7,\
				pgpnStatMsgSp->iMsgPara8, pgpnStatMsgSp->iMsgPara9, pgpnStatMsgSp->iMsgParaA,\
				pgpnStatMsgSp->iMsgParaB);
			break;
			
		case GPN_STAT_MSG_ALM_THRED_TEMP_DELETE:
			reval = gpnStatFuncApiAlmThredTpDelete(pgpnStatMsgSp->iMsgPara1);
			break;
			
		case GPN_STAT_MSG_ALM_THRED_TEMP_MODIFY:
			reval = gpnStatFuncApiAlmThredTpModify(pgpnStatMsgSp->iMsgPara1,\
				pgpnStatMsgSp->iMsgPara2, pgpnStatMsgSp->iMsgPara4, pgpnStatMsgSp->iMsgPara5,\
				pgpnStatMsgSp->iMsgPara6, pgpnStatMsgSp->iMsgPara7, pgpnStatMsgSp->iMsgPara8,\
				pgpnStatMsgSp->iMsgPara9, pgpnStatMsgSp->iMsgParaA, pgpnStatMsgSp->iMsgParaB);
			break;
			
		case GPN_STAT_MSG_ALM_THRED_TEMP_GET:
			break;
		case GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT:
			break;
			
		case GPN_STAT_MSG_SUBFILT_TEMP_ADD:
			reval = gpnStatFuncApiSubFiltTpAdd(pgpnStatMsgSp->iMsgPara1, pgpnStatMsgSp->iMsgPara2,\
				pgpnStatMsgSp->iMsgPara3, pgpnStatMsgSp->iMsgPara4);
			break;
			
		case GPN_STAT_MSG_SUBFILT_TEMP_DELETE:
			reval = gpnStatFuncApiSubFiltTpDelete(pgpnStatMsgSp->iMsgPara1);
			break;

		case GPN_STAT_MSG_SUBFILT_TEMP_MODIFY:
			reval = gpnStatFuncApiSubFiltTpModify(pgpnStatMsgSp->iMsgPara1, pgpnStatMsgSp->iMsgPara2,\
				pgpnStatMsgSp->iMsgPara4);
			break;
			
		case GPN_STAT_MSG_SUBFILT_TEMP_GET:
			break;
		case GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT:
			break;

		case GPN_STAT_MSG_SUBREPORT_TEMP_ADD:
			reval = gpnStatFuncApiSubReportTpAdd(pgpnStatMsgSp->iMsgPara1, pgpnStatMsgSp->iMsgPara2,\
				pgpnStatMsgSp->iMsgPara3, pgpnStatMsgSp->iMsgPara4);
			break;
			
		case GPN_STAT_MSG_SUBREPORT_TEMP_DELETE:
			reval = gpnStatFuncApiSubReportTpDelete(pgpnStatMsgSp->iMsgPara1);
			break;

		case GPN_STAT_MSG_SUBREPORT_TEMP_MODIFY:
			reval = gpnStatFuncApiSubReportTpModify(pgpnStatMsgSp->iMsgPara1, pgpnStatMsgSp->iMsgPara2,\
				pgpnStatMsgSp->iMsgPara4);
			break;
			
		case GPN_STAT_MSG_SUBREPORT_TEMP_GET:
			break;
		case GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT:
			break;

		case GPN_STAT_MSG_MON_ADD:
			localPIndex.devIndex = pgpnStatMsgSp->iMsgPara1;
			localPIndex.portIndex = pgpnStatMsgSp->iMsgPara2;
			localPIndex.portIndex3 = pgpnStatMsgSp->iMsgPara3;
			localPIndex.portIndex4 = pgpnStatMsgSp->iMsgPara4;
			localPIndex.portIndex5 = pgpnStatMsgSp->iMsgPara5;
			localPIndex.portIndex6 = pgpnStatMsgSp->iMsgPara6;
			reval = gpnStatFuncApiMonAdd(&localPIndex, pgpnStatMsgSp->iMsgPara7);
			break;
			
		case GPN_STAT_MSG_MON_DELETE:
			localPIndex.devIndex = pgpnStatMsgSp->iMsgPara1;
			localPIndex.portIndex = pgpnStatMsgSp->iMsgPara2;
			localPIndex.portIndex3 = pgpnStatMsgSp->iMsgPara3;
			localPIndex.portIndex4 = pgpnStatMsgSp->iMsgPara4;
			localPIndex.portIndex5 = pgpnStatMsgSp->iMsgPara5;
			localPIndex.portIndex6 = pgpnStatMsgSp->iMsgPara6;
			reval = gpnStatFuncApiMonDelete(&localPIndex, pgpnStatMsgSp->iMsgPara7);
			break;

		case GPN_STAT_MSG_MON_GET:
			break;
		case GPN_STAT_MSG_MON_GET_NEXT:
			break;
			
		/*port moni modify start*/
		case GPN_STAT_MSG_BASE_MON_OPT:
			localPIndex.devIndex = pgpnStatMsgSp->iMsgPara1;
			localPIndex.portIndex = pgpnStatMsgSp->iMsgPara2;
			localPIndex.portIndex3 = pgpnStatMsgSp->iMsgPara3;
			localPIndex.portIndex4 = pgpnStatMsgSp->iMsgPara4;
			localPIndex.portIndex5 = pgpnStatMsgSp->iMsgPara5;
			localPIndex.portIndex6 = pgpnStatMsgSp->iMsgPara6;
			reval = gpnStatFuncApiBaseMonOpt(&localPIndex,\
				pgpnStatMsgSp->iMsgPara7, pgpnStatMsgSp->iMsgPara8, pgpnStatMsgSp->iMsgPara9);
			break;
			
		case GPN_STAT_MSG_PORT_CYC_MON_OPT:
			localPIndex.devIndex = pgpnStatMsgSp->iMsgPara1;
			localPIndex.portIndex = pgpnStatMsgSp->iMsgPara2;
			localPIndex.portIndex3 = pgpnStatMsgSp->iMsgPara3;
			localPIndex.portIndex4 = pgpnStatMsgSp->iMsgPara4;
			localPIndex.portIndex5 = pgpnStatMsgSp->iMsgPara5;
			localPIndex.portIndex6 = pgpnStatMsgSp->iMsgPara6;
			/*add pgpnStatMsgSp->iMsgParaC  geqian */
			reval = gpnStatFuncApiXCycMonOpt(&localPIndex, pgpnStatMsgSp->iMsgPara7,\
				pgpnStatMsgSp->iMsgPara8, pgpnStatMsgSp->iMsgPara9,\
				pgpnStatMsgSp->iMsgParaA, pgpnStatMsgSp->iMsgParaB, pgpnStatMsgSp->iMsgParaC);
			break;
			
		case GPN_STAT_MSG_TASK_ALM_THREDTP_CFG:
			break;
			
		/*port moni modify end*/

		
		default:
			GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : msgType(%08x),proc err!\n\r",
				__FUNCTION__, pgpnStatMsgSp->iMsgType);
			break;
	}

	return reval;
}

UINT32 gpnStatCollectPorc(gpnSockMsg *pgpnStatMsgSp, UINT32 len)
{
	UINT32 reval;
	optObjOrient peerPIndex;

	GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : recive msgType(%08x), len(%d)\n\r",
		__FUNCTION__, pgpnStatMsgSp->iMsgType, len);
	
	/*in GPN_STAT, req DA creat devIndex */	
	peerPIndex.devIndex = pgpnStatMsgSp->iMsgPara1;
	peerPIndex.portIndex = pgpnStatMsgSp->iMsgPara2;
	peerPIndex.portIndex3 = pgpnStatMsgSp->iMsgPara3;
	peerPIndex.portIndex4 = pgpnStatMsgSp->iMsgPara4;
	peerPIndex.portIndex5 = pgpnStatMsgSp->iMsgPara5;
	peerPIndex.portIndex6 = pgpnStatMsgSp->iMsgPara6;
	
	/*assert*/
	/*assert : do not do */
	reval = GPN_STAT_API_GEN_ERR;
#if 0
	zlog_debug(0, "%s[%d] : recv iMsgType(%x)\n", __func__, __LINE__, pgpnStatMsgSp->iMsgType);
	zlog_debug(0, "                    EN(%x|%x)\n", GPN_STAT_MSG_PORT_STAT_MON_ENABLE_RSP, GPN_STAT_MSG_PORT_STAT_MON_DISABLE_RSP);
	zlog_debug(0, "                 Vs_PM(%x|%x)\n", GPN_STAT_MSG_VS_P_STAT_GET_RSP, GPN_STAT_MSG_VS_M_STAT_GET_RSP);
	zlog_debug(0, "                LSP_PM(%x|%x)\n", GPN_STAT_MSG_LSP_P_STAT_GET_RSP, GPN_STAT_MSG_LSP_M_STAT_GET_RSP);
	zlog_debug(0, "                 PW_PM(%x|%x)\n", GPN_STAT_MSG_PW_P_STAT_GET_RSP, GPN_STAT_MSG_PW_M_STAT_GET_RSP);
	zlog_debug(0, "                ETH_PM(%x|%x)\n", GPN_STAT_MSG_ETH_PHY_P_STAT_GET_RSP, GPN_STAT_MSG_ETH_PHY_M_STAT_GET_RSP);
	zlog_debug(0, "               ETH_SFP(%x)\n", GPN_STAT_MSG_ETH_SFP_STAT_GET_RSP);
	zlog_debug(0, "             FOLLOW_PM(%x|%x)\n", GPN_STAT_MSG_FLOW_P_STAT_GET_RSP, GPN_STAT_MSG_FLOW_M_STAT_GET_RSP);
	zlog_debug(0, "                   EQU(%x)\n", GPN_STAT_MSG_EQU_STAT_GET_RSP);
	zlog_debug(0, "                  SOFT(%x)\n", GPN_STAT_MSG_SOFT_STAT_GET_RSP);
	zlog_debug(0, "                   ENV(%x)\n", GPN_STAT_MSG_ENV_STAT_GET_RSP);
	zlog_debug(0, "                PDHPPI(%x)\n", GPN_STAT_PARA_PDHPPI_P_STAT_GET_RSP);
	zlog_debug(0, "              VPLSPW_P(%x|%x)\n", GPN_STAT_MSG_VPLSPW_P_STAT_GET_RSP, GPN_STAT_MSG_VPLSPW_M_STAT_GET_RSP);
	zlog_debug(0, "               V_UNI_P(%x)\n", GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET_RSP);
	zlog_debug(0, "                   CFM(%x)\n", GPN_STAT_MSG_PTN_MEP_M_STAT_GET_RSP);
#endif

	/* to deal with equ data from system and hal, add by lipf, 2018/3/28 */
	gpnStatTogetherEquData(pgpnStatMsgSp);
	
	switch(pgpnStatMsgSp->iMsgType)
	{
		case GPN_STAT_MSG_VS_P_STAT_GET_RSP:
		case GPN_STAT_MSG_VS_M_STAT_GET_RSP:
		case GPN_STAT_MSG_LSP_P_STAT_GET_RSP:
		case GPN_STAT_MSG_LSP_M_STAT_GET_RSP:
		case GPN_STAT_MSG_PW_P_STAT_GET_RSP:
		case GPN_STAT_MSG_PW_M_STAT_GET_RSP:
		case GPN_STAT_MSG_ETH_PHY_P_STAT_GET_RSP:
		case GPN_STAT_MSG_ETH_PHY_M_STAT_GET_RSP:
		case GPN_STAT_MSG_ETH_SFP_STAT_GET_RSP:
		case GPN_STAT_MSG_FLOW_P_STAT_GET_RSP:
		case GPN_STAT_MSG_FLOW_M_STAT_GET_RSP:
		case GPN_STAT_MSG_EQU_STAT_GET_RSP:
		case GPN_STAT_MSG_SOFT_STAT_GET_RSP:
		case GPN_STAT_MSG_ENV_STAT_GET_RSP:
		case GPN_STAT_PARA_PDHPPI_P_STAT_GET_RSP:
		case GPN_STAT_MSG_VPLSPW_P_STAT_GET_RSP:
		case GPN_STAT_MSG_VPLSPW_M_STAT_GET_RSP:
		case GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET_RSP:
		case GPN_STAT_MSG_PTN_MEP_M_STAT_GET_RSP:
			GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : rec statType(%08x), dev(%08x) port(%08x|%08x|%08x|%08x|%08x), len(%d)\n\r",\
				__FUNCTION__, pgpnStatMsgSp->iMsgPara7, pgpnStatMsgSp->iMsgPara1,
				pgpnStatMsgSp->iMsgPara2, pgpnStatMsgSp->iMsgPara3,\
				pgpnStatMsgSp->iMsgPara4, pgpnStatMsgSp->iMsgPara5,\
				pgpnStatMsgSp->iMsgPara6, len);

			zlog_debug(STATIS_DBG_DATA_COLLECT, "%s[%d] : rec statType(%08x), dev(%08x) port(%08x|%08x|%08x|%08x|%08x), len(%d)\n\r",\
				__FUNCTION__, __LINE__, pgpnStatMsgSp->iMsgPara7, pgpnStatMsgSp->iMsgPara1,
				pgpnStatMsgSp->iMsgPara2, pgpnStatMsgSp->iMsgPara3,\
				pgpnStatMsgSp->iMsgPara4, pgpnStatMsgSp->iMsgPara5,\
				pgpnStatMsgSp->iMsgPara6, len);
			reval = gpnStatDataCollectProc(&peerPIndex, pgpnStatMsgSp->iMsgPara7,\
				(void *)(&(pgpnStatMsgSp->msgCellLen) + 1),\
				pgpnStatMsgSp->msgCellLen);
			if (reval == GPN_STAT_COLL_GEN_ERR)
			{
				return GPN_STAT_COLL_GEN_ERR;
			}
			
			/*add this for data test */
			#if 0
			reval = gpnStatDataCollectDebug(&peerPIndex, pgpnStatMsgSp->iMsgPara7);
			if (reval == GPN_STAT_COLL_GEN_ERR)
			{
				return GPN_STAT_COLL_GEN_ERR;
			}
			#endif
			break;

		case GPN_STAT_MSG_PORT_STAT_MON_ENABLE_RSP:
		case GPN_STAT_MSG_PORT_STAT_MON_DISABLE_RSP:
			GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : statType(%08x) monStaCfg rsp, dev(%08x) port(%08x|%08x|%08x|%08x|%08x), en(%d) errCode(%d)\n\r",\
				__FUNCTION__, pgpnStatMsgSp->iMsgPara7, pgpnStatMsgSp->iMsgPara1,
				pgpnStatMsgSp->iMsgPara2, pgpnStatMsgSp->iMsgPara3,\
				pgpnStatMsgSp->iMsgPara4, pgpnStatMsgSp->iMsgPara5,\
				pgpnStatMsgSp->iMsgPara6, pgpnStatMsgSp->iMsgPara8,\
				pgpnStatMsgSp->iMsgPara9);
			if (pgpnStatMsgSp->iMsgPara9 == GPN_STAT_COLL_GEN_ERR)
			{
				return GPN_STAT_COLL_GEN_ERR;
			}
			break;
			
		default:
			break;
	}

	return reval;
}

/*FUNCTION SUPPORT FUNCTION */
UINT32 gpnStatFuncApiSubTypeThredModify(UINT32 subType, stSubTypeThred *psubTypeThred, UINT32 len)
{
	/*assert */
	if( (psubTypeThred == NULL) ||\
		(len != sizeof(stSubTypeThred)) )
	{
		return GPN_STAT_API_GEN_ERR;
	}

	/*dataBase proc */
	/*???*/
	
	gpnStatTypeSubThredModify(subType, psubTypeThred);

	return GPN_STAT_API_GEN_OK;
}

UINT32 gpnStatFuncApiCurrStatModify(objLogicDesc *pmonObjInfo, UINT32 subType, UINT32 dataClear)
{
	/*assert */
	if (!dataClear || (pmonObjInfo == NULL))
	{
		return GPN_STAT_API_GEN_ERR;
	}

	/*dataBase proc */
	
	gpnStatCurrDataClear(pmonObjInfo);
	
	return GPN_STAT_API_GEN_OK;
}

UINT32 gpnStatFuncApiTaskAdd(UINT32 taskId,
	UINT32 statScanType, UINT32 subTaskNum, UINT32 cycClass, UINT32 cycSeconds,
	UINT32 monStartTime, UINT32 monEndTime, UINT32 almThredTpId, UINT32 subReportTpId)
{
	UINT32 reVal;
	stStatTaskNode taskNode;
	stStatAlmThredTpNode *palmThredTpNode;
	stStatSubReportTpNode *psubReportTpNode;

	/*assert */
	
	/*task element vilad check:1 is taskId vilad ? */
	reVal = gpnStatTaskIdCheck(taskId);
	if(reVal == GPN_STAT_API_GEN_ERR)
	{
		zlog_err("%s[%d] : task id invalid!\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}

	/*task element vilad check:2 is statScanType vilad ? */
	/*???*//*refer to check:8 and check:9*/

	/*task element vilad check:3 is subTaskNum vilad ? */
	/*???*/
	/*geqian 2015.10.12*/
	if (subTaskNum < 1 || subTaskNum > 500)
	{
		zlog_err("%s[%d] : task num invalid!\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}

	/*task element vilad check:4 is cycClass vilad ? */
	if( (cycClass != GPN_STAT_CYC_CLASS_LONG) &&\
		(cycClass != GPN_STAT_CYC_CLASS_SHORT) &&\
		(cycClass != GPN_STAT_CYC_CLASS_USR) )
	{
		zlog_debug(STATIS_DBG_SET, "%s[%d] : cycClass invalid!\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}

	/*task element vilad check:6~7 is mon Time vilad ? */
	/*???*/
	
	/*task element vilad check:8 is almThredTpId exsit and vilad ? */
	reVal = gpnStatAlarmThredTpGet(almThredTpId, &palmThredTpNode);
	if( (reVal == GPN_STAT_API_GEN_ERR) ||\
		(palmThredTpNode == NULL) )
	{
		zlog_err("%s[%d] : Must config statAlarmThresholdTemplatTable first!\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}
		
	if(palmThredTpNode->scanType != statScanType)
	{
		zlog_err("%s[%d] : scanType invalid! (%d:%d)\n", __func__, __LINE__, 
			palmThredTpNode->scanType, statScanType);
		return GPN_STAT_API_GEN_ERR;
	}

	/*task element vilad check:9 is subReportTpId exsit and vilad ? */
	reVal = gpnStatSubReportTpGet(subReportTpId, &psubReportTpNode);
	if( (reVal == GPN_STAT_API_GEN_ERR) ||\
		(psubReportTpNode == NULL) )
	{
		zlog_err("%s[%d] : Must config statSubStatTypeReportTemplatTable first!\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}
	if(psubReportTpNode->scanType != statScanType)
	{
		zlog_err("%s[%d] : scanType invalid! (%d:%d)\n", __func__, __LINE__, 
			psubReportTpNode->scanType, statScanType);
		return GPN_STAT_API_GEN_ERR;
	}

	/*filt task node */
	taskNode.taskId = taskId;
	taskNode.statScanTp = statScanType;
	taskNode.cycClass = cycClass;
	taskNode.cycSeconds = cycSeconds;
	taskNode.monStartTime = monStartTime;
	taskNode.monEndTime = monEndTime;
	taskNode.almThredTpId = almThredTpId;
	taskNode.subReportTpId = subReportTpId;
	taskNode.maxPortInTask = subTaskNum;
	
	/*dataBase proc */
	/*???*/
	
	if(gpnStatTaskAdd(&taskNode) == GPN_STAT_API_GEN_OK)
	{
		zlog_debug(STATIS_DBG_SET, "%s[%d] : set task table success\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		zlog_err("%s[%d] : set task table error\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}
}

UINT32 gpnStatFuncApiTaskDelete(UINT32 taskId)
{
	/*dataBase proc */
	/*???*/
	
	if(gpnStatTaskDelete(taskId) == GPN_STAT_API_GEN_OK)
	{
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}
}

UINT32 gpnStatFuncApiTaskModify(UINT32 taskId, UINT32 cycSeconds,
	UINT32 monStartTime, UINT32 monEndTime, UINT32 almThredTpId, UINT32 subReportTpId)
{
	UINT32 reVal;
	stStatTaskNode taskNode;
	stStatTaskNode *ptaskNode;
	stStatAlmThredTpNode *palmThredTpNode;
	stStatSubReportTpNode *psubReportTpNode;

	/*assert */

	/*task element vilad check:1 is taskId vilad ? */
	ptaskNode = NULL;
	gpnStatTaskGet(taskId, &ptaskNode);
	if(ptaskNode == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}

	/*task element vilad check:2 is statScanType vilad ? */

	/*task element vilad check:3 is subTaskNum vilad ? */

	/*task element vilad check:4 is cycClass vilad ? */

	/*task element vilad check:6~7 is mon Time vilad ? */
	/*???*/
	
	/*task element vilad check:8 is almThredTpId exsit and vilad ? */
	reVal = gpnStatAlarmThredTpGet(almThredTpId, &palmThredTpNode);
	if( (reVal == GPN_STAT_API_GEN_ERR) ||\
		(palmThredTpNode == NULL) )
	{
		return GPN_STAT_API_GEN_ERR;
	}
	if(palmThredTpNode->scanType != ptaskNode->statScanTp)
	{
		return GPN_STAT_API_GEN_ERR;
	}

	/*task element vilad check:9 is subReportTpId exsit and vilad ? */
	reVal = gpnStatSubReportTpGet(subReportTpId, &psubReportTpNode);
	if( (reVal == GPN_STAT_API_GEN_ERR) ||\
		(psubReportTpNode == NULL) )
	{
		return GPN_STAT_API_GEN_ERR;
	}
	if(psubReportTpNode->scanType != ptaskNode->statScanTp)
	{
		return GPN_STAT_API_GEN_ERR;
	}

	taskNode.taskId = ptaskNode->taskId;
	taskNode.statScanTp = ptaskNode->statScanTp;
	taskNode.cycClass = ptaskNode->cycClass;
	taskNode.maxPortInTask = ptaskNode->maxPortInTask;
	
	taskNode.cycSeconds = cycSeconds;
	taskNode.monStartTime = monStartTime;
	taskNode.monEndTime = monEndTime;
	taskNode.almThredTpId = almThredTpId;
	taskNode.subReportTpId = subReportTpId;
	
	/*dataBase proc */
	/*???*/

	if(gpnStatTaskModify(&taskNode) == GPN_STAT_API_GEN_OK)
	{
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}

}

UINT32 gpnStatFuncApiEvnThredTpAdd(UINT32 thredTpId,
	UINT32 subType, UINT32 scanType, UINT32 upThredH32,
	UINT32 upThredL32, UINT32 downThredH32, UINT32 downThredL32)
{
	stStatEvnThredTpNode *pthredTpNode;
	stStatEvnThredTpTable thredTpLine;
	stStatSubTpNode *pstatSubTpNode;
	UINT32 statScanType;

	/*assert */

	/*event threshold template element vilad check:1 is thredTpId vilad ? */
	pthredTpNode = NULL;
	gpnStatEventThredTpGet(thredTpId, &pthredTpNode);
	if(pthredTpNode == NULL)
	{
		/*thredTpId no exit, is new creat ?*/
		if(gpnStatEventThredTemplatIdCheck(thredTpId) == GPN_STAT_API_GEN_ERR)
		{
			zlog_err("%s[%d] : EventThredTemplat id invalid!\n", __func__, __LINE__);
			return GPN_STAT_API_GEN_ERR;
		}
	}

	/*event threshold template element vilad check:2 is sub type vilad ? */
	/*???*//*geqian 2015.10.12*/
	pstatSubTpNode = NULL;
	if(gpnStatTypeGetSubStatTypeNode(subType, &pstatSubTpNode) != GPN_STAT_API_GEN_OK)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : subStatTp(%08x) get subStatNode error\n\r",\
			__FUNCTION__, subType);
		zlog_err("%s[%d] : subStatTp(%08x) get subStatNode error\n\r",\
			__FUNCTION__, __LINE__, subType);
		return GPN_STAT_API_GEN_ERR;
	}

	/*event threshold template element vilad check:3 is scanType vilad ? */
	/*???*//*geqian 2015.10.12*/
	if(gpnStatTypeSubStatType2ScanType(subType, &statScanType) == GPN_STAT_API_GEN_OK)
	{
		if (scanType != statScanType)
		{
			gpnLog(GPN_LOG_L_ERR, "%s : subStatTp(%08x) vs scanType(%08x) error\n\r",\
				__FUNCTION__, subType, statScanType);
			return GPN_STAT_API_GEN_ERR;
		}
	}
	else
	{
		gpnLog(GPN_LOG_L_ERR, "%s : subStatTp(%08x) get scanType error\n\r",\
			__FUNCTION__, subType);
		return GPN_STAT_API_GEN_ERR;
	}
	
	/*sub report node, make a temp datastruct to format gpn_socket_msg paras */
	thredTpLine.thredTpId = thredTpId;
	thredTpLine.subType = subType;
	thredTpLine.scanType = scanType;
	thredTpLine.upThredH32 = upThredH32;
	thredTpLine.upThredL32 = upThredL32;
	thredTpLine.dnThredH32 = downThredH32;
	thredTpLine.dnThredL32 = downThredL32;

	if(gpnStatEventThredTpAdd(&thredTpLine) == GPN_STAT_API_GEN_OK)
	{
		zlog_debug(STATIS_DBG_SET, "%s[%d] : set event thred template table success\n", __func__, __LINE__);
		gpnLog(GPN_LOG_L_ERR, "%s : subStatTp(%08x) thredId(%d) ok!\n\r",\
			__FUNCTION__, subType, thredTpId);
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		zlog_err("%s[%d] : set event thred template error\n", __func__, __LINE__);
		gpnLog(GPN_LOG_L_ERR, "%s : subStatTp(%08x) thredId(%d) error!\n\r",\
			__FUNCTION__, subType, thredTpId);
		return GPN_STAT_API_GEN_ERR;
	}
}
UINT32 gpnStatFuncApiEvnThredTpDelete(UINT32 thredTpId)
{
	/*dataBase proc */
	/*???*/
	
	if(gpnStatEventThredTpDelete(thredTpId) == GPN_STAT_API_GEN_OK)
	{
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}
}
UINT32 gpnStatFuncApiEvnThredTpModify(UINT32 thredTpId,
	UINT32 subType, UINT32 upThredH,  UINT32 upThredL,
	UINT32 dnThredH, UINT32 dnThredL)
{
	stStatEvnThredTpNode *pthredTpNode;
	stStatEvnThredTpTable evnThredTpLine;
	UINT32 statScanType;
	
	/*assert */

	/*event threshold template element vilad check:1 is thredTpId vilad ? */
	pthredTpNode = NULL;
	gpnStatEventThredTpGet(thredTpId, &pthredTpNode);
	if(pthredTpNode == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}

	/*event threshold template element vilad check:1 is scanType vilad ? */
	/*???*/
	if(gpnStatTypeSubStatType2ScanType(subType, &statScanType) == GPN_STAT_API_GEN_OK)
	{
		if(pthredTpNode->scanType != statScanType)
		{
			return GPN_STAT_API_GEN_ERR;
		}
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}
	
	/*event threshold node, first : make a temp datastruct to format gpn_socket_msg paras */
	evnThredTpLine.thredTpId = thredTpId;
	evnThredTpLine.subType = subType;
	/*subReportTpLine.scanType = scanType;*/
	evnThredTpLine.upThredH32 = upThredH;
	evnThredTpLine.upThredL32 = upThredL;
	evnThredTpLine.dnThredH32 = dnThredH;
	evnThredTpLine.dnThredL32 = dnThredL;
	
	if(gpnStatEventThredTpModify(&evnThredTpLine) == GPN_STAT_API_GEN_OK)
	{
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}	

	return GPN_STAT_API_GEN_OK;
}
UINT32 gpnStatFuncApiAlmThredTpAdd(UINT32 thredTpId,
	UINT32 subType, UINT32 scanType, UINT32 upRiseThreshH, UINT32 upRiseThreshL,
	UINT32 upDispThreshH, UINT32 upDispThreshL, UINT32 dnRiseThreshH,
	UINT32 dnRiseThreshL, UINT32 dnDispThreshH, UINT32 dnDispThreshL)
{
	stStatAlmThredTpNode *pthredTpNode;
	stStatAlmThredTpTable thredTpLine;
	stStatSubTpNode *pstatSubTpNode;
	UINT32 statScanType;

	/*assert */

	/*alarm threshold template element vilad check:1 is thredTpId vilad ? */
	pthredTpNode = NULL;
	gpnStatAlarmThredTpGet(thredTpId, &pthredTpNode);
	if(pthredTpNode == NULL)
	{
		/*thredTpId no exit, is new creat ?*/
		if(gpnStatAlarmThredTemplatIdCheck(thredTpId) == GPN_STAT_API_GEN_ERR)
		{
			zlog_err("%s[%d] : alarmThredTemplat id invalid!\n", __func__, __LINE__);
			return GPN_STAT_API_GEN_ERR;
		}
	}

	/*alarm threshold template element vilad check:2 is sub type vilad ? */
	/*???*//*geqian 2015.10.12*/
	pstatSubTpNode = NULL;
	if(gpnStatTypeGetSubStatTypeNode(subType, &pstatSubTpNode) != GPN_STAT_API_GEN_OK)
	{
		zlog_err("%s[%d] : alarmThredTemplat subType invalid!\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}
	
	/*alarm threshold template element vilad check:3 is scanType vilad ? */
	/*???*//*geqian 2015.10.12*/
	if(gpnStatTypeSubStatType2ScanType(subType, &statScanType) == GPN_STAT_API_GEN_OK)
	{
		if(scanType != statScanType)
		{
			return GPN_STAT_API_GEN_ERR;
		}
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}

	/*alarm threshold node, make a temp datastruct to format gpn_socket_msg paras */
	thredTpLine.thredTpId = thredTpId;
	thredTpLine.subType = subType;
	thredTpLine.scanType = scanType;
	thredTpLine.upRiseThredH32 = upRiseThreshH;
	thredTpLine.upRiseThredL32 = upRiseThreshL;
	thredTpLine.upDispThredH32 = upDispThreshH;
	thredTpLine.upDispThredL32 = upDispThreshL;
	thredTpLine.dnRiseThredH32 = dnRiseThreshH;
	thredTpLine.dnRiseThredL32 = dnRiseThreshL;
	thredTpLine.dnDispThredH32 = dnDispThreshH;
	thredTpLine.dnDispThredL32 = dnDispThreshL;

	GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : add thredId(%d) subType(%08x)!\n\r",\
		__FUNCTION__, thredTpId, subType);

	if(gpnStatAlarmThredTpAdd(&thredTpLine) == GPN_STAT_API_GEN_OK)
	{
		zlog_debug(STATIS_DBG_SET, "%s[%d] : set AlarmThredTp success\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		zlog_err("%s[%d] : set AlarmThredTp error\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}
}
UINT32 gpnStatFuncApiAlmThredTpDelete(UINT32 thredTpId)
{
	if(gpnStatAlarmThredTpDelete(thredTpId) == GPN_STAT_API_GEN_OK)
	{
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}
}
UINT32 gpnStatFuncApiAlmThredTpModify(UINT32 thredTpId,
	UINT32 subType, UINT32 upRiseThredH, UINT32 upRiseThredL,
	UINT32 upDispThredH, UINT32 upDispThredL, UINT32 dnRiseThredH,
	UINT32 dnRiseThredL, UINT32 dnDispThredH, UINT32 dnDispThredL)
{
	stStatAlmThredTpNode *pthredTpNode;
	stStatAlmThredTpTable almThredTpLine;
	UINT32 statScanType;
	
	/*assert */

	/*alarm threshold template element vilad check:1 is thredTpId vilad ? */
	pthredTpNode = NULL;
	gpnStatAlarmThredTpGet(thredTpId, &pthredTpNode);
	if(pthredTpNode == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}

	/*alarm threshold template element vilad check:1 is scanType vilad ? */
	/*???*/
	if(gpnStatTypeSubStatType2ScanType(subType, &statScanType) == GPN_STAT_API_GEN_OK)
	{
		if(pthredTpNode->scanType != statScanType)
		{
			return GPN_STAT_API_GEN_ERR;
		}
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}
	
	/*alarm threshold node, first : make a temp datastruct to format gpn_socket_msg paras */
	almThredTpLine.thredTpId = thredTpId;
	almThredTpLine.subType = subType;
	/*almThredTpLine.scanType = scanType;*/
	almThredTpLine.upRiseThredH32 = upRiseThredH;
	almThredTpLine.upRiseThredL32 = upRiseThredL;
	almThredTpLine.upDispThredH32 = upDispThredH;
	almThredTpLine.upDispThredL32 = upDispThredL;
	almThredTpLine.dnRiseThredH32 = dnRiseThredH;
	almThredTpLine.dnRiseThredL32 = dnRiseThredL;
	almThredTpLine.dnDispThredH32 = dnDispThredH;
	almThredTpLine.dnDispThredL32 = dnDispThredL;
	
	if(gpnStatAlarmThredTpModify(&almThredTpLine) == GPN_STAT_API_GEN_OK)
	{
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}	

	return GPN_STAT_API_GEN_OK;
}

UINT32 gpnStatFuncApiSubFiltTpAdd(UINT32 subFiltTpId,
	UINT32 subType, UINT32 scanType, UINT32 status)
{
	stStatSubFiltTpNode *pthredTpNode;
	stStatSubFiltTpTable subFiltTpLine;
	stStatSubTpNode *pstatSubTpNode;
	UINT32 statScanType;

	/*assert */

	/*sub filt threshold template element vilad check:1 is thredTpId vilad ? */
	pthredTpNode = NULL;
	gpnStatSubFiltTpGet(subFiltTpId, &pthredTpNode);
	if(pthredTpNode == NULL)
	{
		/*thredTpId no exit, is new creat ?*/
		if(gpnStatSubTypeFiltIdCheck(subFiltTpId) == GPN_STAT_API_GEN_ERR)
		{
			zlog_err("%s[%d] : alarmThredTemplat id invalid!\n", __func__, __LINE__);
			return GPN_STAT_API_GEN_ERR;
		}
	}

	/*sub filt threshold template element vilad check:2 is sub type vilad ? */
	/*???*//*geqian 2015.10.12*/
	pstatSubTpNode = NULL;
	if(gpnStatTypeGetSubStatTypeNode(subType, &pstatSubTpNode) != GPN_STAT_API_GEN_OK)
	{
		zlog_err("%s[%d] : alarmThredTemplat subType invalid!\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}

	/*sub filt threshold template element vilad check:3 is scanType vilad ? */
	/*???*//*geqian 2015.10.12*/
	if(gpnStatTypeSubStatType2ScanType(subType, &statScanType) == GPN_STAT_API_GEN_OK)
	{
		if(scanType != statScanType)
		{
			return GPN_STAT_API_GEN_ERR;
		}
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}
	
	
	/*filt subFilt node, first : make a temp datastruct to format gpn_socket_msg paras */
	subFiltTpLine.subFiltTpId = subFiltTpId;
	subFiltTpLine.subType = subType;
	subFiltTpLine.scanType = scanType;
	subFiltTpLine.status = status;
	
	if(gpnStatSubFiltTpAdd(&subFiltTpLine) == GPN_STAT_API_GEN_OK)
	{
		zlog_debug(STATIS_DBG_SET, "%s[%d] : set SubFiltTp success\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		zlog_err("%s[%d] : set SubFiltTp error\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}
}
UINT32 gpnStatFuncApiSubFiltTpDelete(UINT32 subFiltTpId)
{
	if(gpnStatSubFiltTpDelete(subFiltTpId) == GPN_STAT_API_GEN_OK)
	{
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}
}
UINT32 gpnStatFuncApiSubFiltTpModify(UINT32 subFiltTpId, UINT32 subType, UINT32 status)
{
	stStatSubFiltTpNode *pthredTpNode;
	stStatSubFiltTpTable subFiltTpLine;
	UINT32 statScanType;
	
	/*assert */

	/*sub filt threshold template element vilad check:1 is subFiltTpId vilad ? */
	pthredTpNode = NULL;
	gpnStatSubFiltTpGet(subFiltTpId, &pthredTpNode);
	if(pthredTpNode == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}

	/*sub filt threshold template element vilad check:1 is scanType vilad ? */
	/*???*/
	if(gpnStatTypeSubStatType2ScanType(subType, &statScanType) == GPN_STAT_API_GEN_OK)
	{
		if(pthredTpNode->scanType != statScanType)
		{
			return GPN_STAT_API_GEN_ERR;
		}
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}
	
	/*filt subFilt node, first : make a temp datastruct to format gpn_socket_msg paras */
	subFiltTpLine.subFiltTpId = subFiltTpId;
	subFiltTpLine.subType = subType;
	/*subFiltTpLine.scanType = scanType;*/
	subFiltTpLine.status = status;
	
	if(gpnStatSubFiltTpModify(&subFiltTpLine) == GPN_STAT_API_GEN_OK)
	{
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}	

	return GPN_STAT_API_GEN_OK;
}
UINT32 gpnStatFuncApiSubReportTpAdd(UINT32 subReportTpId,
	UINT32 subType, UINT32 scanType, UINT32 status)
{
	stStatSubReportTpNode *pthredTpNode;
	stStatSubReportTpTable subReportTpLine;
	stStatSubTpNode *pstatSubTpNode;
	UINT32 statScanType;

	/*assert */

	/*sub report threshold template element vilad check:1 is subReportTpId vilad ? */
	pthredTpNode = NULL;
	gpnStatSubReportTpGet(subReportTpId, &pthredTpNode);
	if(pthredTpNode == NULL)
	{
		/*thredTpId no exit, is new creat ?*/
		if(gpnStatSubTypeReportIdCheck(subReportTpId) == GPN_STAT_API_GEN_ERR)
		{
			zlog_err("%s[%d] : SubTypeReport id invalid!\n", __func__, __LINE__);
			return GPN_STAT_API_GEN_ERR;
		}
	}

	/*sub report threshold template element vilad check:2is subType vilad ? */
	/*???*//*geqian 2015.10.12*/
	pstatSubTpNode = NULL;
	if(gpnStatTypeGetSubStatTypeNode(subType, &pstatSubTpNode) != GPN_STAT_API_GEN_OK)
	{
		zlog_err("%s[%d] : SubTypeReport subType invalid!\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}

	/*sub report threshold template element vilad check:3 is scanType vilad ? */
	/*???*//*geqian 2015.10.12*/
	if(gpnStatTypeSubStatType2ScanType(subType, &statScanType) == GPN_STAT_API_GEN_OK)
	{
		if(scanType != statScanType)
		{
			return GPN_STAT_API_GEN_ERR;
		}
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}
	
	
	/*filt subReport node, first : make a temp datastruct to format gpn_socket_msg paras */
	subReportTpLine.subReportTpId = subReportTpId;
	subReportTpLine.subType = subType;
	subReportTpLine.scanType = scanType;
	subReportTpLine.status = status;
	
	if(gpnStatSubReportTpAdd(&subReportTpLine) == GPN_STAT_API_GEN_OK)
	{
		zlog_debug(STATIS_DBG_SET, "%s[%d] : set SubReportTp success\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		zlog_err("%s[%d] : set SubReportTp success\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}
}
UINT32 gpnStatFuncApiSubReportTpDelete(UINT32 subReportTpId)
{
	if(gpnStatSubReportTpDelete(subReportTpId) == GPN_STAT_API_GEN_OK)
	{
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}
}
UINT32 gpnStatFuncApiSubReportTpModify(UINT32 subReportTpId, UINT32 subType, UINT32 status)
{
	stStatSubReportTpNode *pthredTpNode;
	stStatSubReportTpTable subReportTpLine;
	UINT32 statScanType;
	
	/*assert */

	/*sub report threshold template element vilad check:1 is thredTpId vilad ? */
	pthredTpNode = NULL;
	gpnStatSubReportTpGet(subReportTpId, &pthredTpNode);
	if(pthredTpNode == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}

	/*sub report threshold template element vilad check:1 is scanType vilad ? */
	/*???*/
	if(gpnStatTypeSubStatType2ScanType(subType, &statScanType) == GPN_STAT_API_GEN_OK)
	{
		if(pthredTpNode->scanType != statScanType)
		{
			return GPN_STAT_API_GEN_ERR;
		}
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}
	
	/*filt subReport node, first : make a temp datastruct to format gpn_socket_msg paras */
	subReportTpLine.subReportTpId = subReportTpId;
	subReportTpLine.subType = subType;
	/*subReportTpLine.scanType = scanType;*/
	subReportTpLine.status = status;
	
	if(gpnStatSubReportTpModify(&subReportTpLine) == GPN_STAT_API_GEN_OK)
	{
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}	

	return GPN_STAT_API_GEN_OK;
}

UINT32 gpnStatFuncApiMonAdd(objLogicDesc *plocalPIndex, UINT32 scanType)
{
	UINT32 reVal;
	
	/*assert */
	if(plocalPIndex == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}

	/*is portIndex villed ??? */
	if( (plocalPIndex->portIndex == GPN_ILLEGAL_PORT_INDEX) ||\
		(plocalPIndex->portIndex == GPN_PORT_INDEX_NULL) )
	{
		zlog_err("%s[%d] : invalid portIndex\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}

	/*is scanType villed ??? now no used */
	
	/*data base opt (renew mxml ram data ???) */

	/*data struct opt */
	reVal = gpnStatDataStructMonAdd(plocalPIndex);

	return reVal;
}

UINT32 gpnStatFuncApiMonDelete(objLogicDesc *plocalPIndex, UINT32 scanType)
{
	UINT32 reVal;
	
	/*assert */
	if(plocalPIndex == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}

	/*is portIndex villed ??? */
	if( (plocalPIndex->portIndex == GPN_ILLEGAL_PORT_INDEX) ||\
		(plocalPIndex->portIndex == GPN_PORT_INDEX_NULL) )
	{
		return GPN_STAT_API_GEN_ERR;
	}

	/*is scanType villed ??? now no used */
	
	/*data base opt (renew mxml ram data ???) */
	reVal = gpnStatMonOpt(plocalPIndex, scanType, GPN_STAT_CYC_CLASS_BASE, 0,\
				0, 0, 0, 0, 0, 0, GPN_STAT_DBS_GEN_DELETE, GPN_STAT_DBS_GEN_DISABLE);

	/*data struct opt */
	reVal = gpnStatDataStructMonDelete(plocalPIndex);
	
	return reVal;
}

UINT32 gpnStatFuncApiBaseMonOpt(objLogicDesc *plocalPIndex, UINT32 scanType, UINT32 opt, UINT32 currOpt)
{
	UINT32 reVal;

	/*mon opt element vilad check:1 isplocalPIndex vilad ? */
	/*???*/
	
	/*use for port's stat mon global control, default enable, once disable and
	   at same time all other cycClass are disbale, then delete that obj's mon */
	reVal = gpnStatMonOpt(plocalPIndex, scanType, GPN_STAT_CYC_CLASS_BASE, 0,\
				0, 0, 0, 0, 0, 0, opt, currOpt);
	if(reVal == GPN_STAT_API_GEN_OK)
	{
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		return GPN_STAT_API_GEN_ERR;
	}
}

UINT32 gpnStatFuncApiXCycMonOpt(objLogicDesc *plocalPIndex, UINT32 scanType,
	UINT32 taskId, UINT32 opt, UINT32 evnThTpId, UINT32 subFiltTpId, INT32 histReport)
{
	UINT32 reVal;
	stStatTaskNode *ptaskNode;
	stStatEvnThredTpNode *pthredTpNode;
	stStatSubFiltTpNode *psubFiltTpNode;

	/*assert */
	if(plocalPIndex == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}
	
	/*mon opt element vilad check:1 isplocalPIndex vilad ? */
	/*???*/

	/*mon opt element vilad check:1 taskId vilad ? */
	ptaskNode = NULL;
	reVal = gpnStatTaskGet(taskId, &ptaskNode);
	if(ptaskNode == NULL)
	{
		GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : get taskNode(Id:%d), err!\n\r",\
			__FUNCTION__, taskId);
		
		zlog_err("%s[%d] : get taskNode(Id:%d), err!\n\r", __FUNCTION__, __LINE__, taskId);
		return GPN_STAT_API_GEN_ERR;
	}
	/*check statScanType */
	/*???*//*geqian 2015.10.12*/
	if(scanType != ptaskNode->statScanTp)
	{
		zlog_err("%s[%d] : scanType(%d) err!\n\r", __FUNCTION__, __LINE__, scanType);
		return GPN_STAT_API_GEN_ERR;
	}

	/**/
	/*mon opt element vilad check:1 evnThTpId vilad ? */
	pthredTpNode = NULL;
	reVal = gpnStatEventThredTpGet(evnThTpId, &pthredTpNode);
	if(pthredTpNode == NULL)
	{
		GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : get event threshold(Id:%d), err!\n\r",\
			__FUNCTION__, evnThTpId);

		zlog_err("%s[%d] : get event threshold(Id:%d), err!\n\r", __FUNCTION__, __LINE__, evnThTpId);
		return GPN_STAT_API_GEN_ERR;
	}
	/*check statScanType */
	/*???*/
	if(scanType != pthredTpNode->scanType)
	{
		zlog_err("%s[%d] : scanType(%d) err!\n\r", __FUNCTION__, __LINE__, scanType);
		return GPN_STAT_API_GEN_ERR;
	}
	
	/*mon opt element vilad check:1 subFiltTpId vilad ? */
	psubFiltTpNode = NULL;
	reVal = gpnStatSubFiltTpGet(subFiltTpId, &psubFiltTpNode);
	if(psubFiltTpNode == NULL)
	{
		GPN_STAT_API_PRINT(GPN_STAT_API_CUP, "%s : get sub filt template(Id:%d), err!\n\r",\
			__FUNCTION__, subFiltTpId);

		zlog_err("%s[%d] : get sub filt template(Id:%d), err!\n\r", __FUNCTION__, __LINE__, subFiltTpId);
		return GPN_STAT_API_GEN_ERR;
	}
	/*check statScanType */
	/*???*/
	if(scanType != psubFiltTpNode->scanType)
	{
		zlog_err("%s[%d] : scanType(%d) err!\n\r", __FUNCTION__, __LINE__, scanType);
		return GPN_STAT_API_GEN_ERR;
	}
	
	reVal = gpnStatMonOpt(plocalPIndex, scanType,\
				ptaskNode->cycClass, ptaskNode->cycSeconds, ptaskNode->taskId,\
				ptaskNode->almThredTpId, ptaskNode->subReportTpId, evnThTpId, subFiltTpId, histReport, opt, opt);
	if(reVal == GPN_STAT_API_GEN_OK)
	{
		zlog_err("%s[%d] : set success\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_OK;
	}
	else
	{
		zlog_err("%s[%d] : set error\n", __func__, __LINE__);
		return GPN_STAT_API_GEN_ERR;
	}
}

#if 0
UINT32 gpnStatFuncApiTaskSubFiltTpCfg(UINT32 taskId, UINT32 subFiltTpId)
{
	UINT32 reVal;
	UINT32 objNum;
	stStatMonObjNode *pobjNode;
	stStatSubFiltTpNode *psubFiltTpNode;
	stStatTaskNode *ptaskNode;

	/*first : thredTp should be exist */
	psubFiltTpNode = NULL;
	reVal = gpnStatSubFiltTpGet(subFiltTpId, &psubFiltTpNode);
	if(psubFiltTpNode == NULL)
	{
		GPN_STAT_API_PRINT(GPN_STAT_API_CMP, "%s : subFiltTp(%d) not exist!",\
			__FUNCTION__, subFiltTpId);
		return GPN_STAT_API_GEN_ERR;
	}
	/*second : task should be exist,  get task, obtain mon obj list */
	ptaskNode = NULL;
	reVal = gpnStatTaskGet(taskId, &ptaskNode);
	if(ptaskNode == NULL)
	{
		GPN_STAT_API_PRINT(GPN_STAT_API_CMP, "%s : task(%d) not exist!",\
			__FUNCTION__, taskId);
		return GPN_STAT_API_GEN_ERR;
	}

	if(ptaskNode->subFiltTpId != subFiltTpId)
	{
		/*chang task's thredTpId */
		ptaskNode->subFiltTpId = subFiltTpId;
		/*chang task's mon obj list's obj threshold templat */
		objNum = 0;
		pobjNode = (stStatMonObjNode *)listFirst(&(ptaskNode->actMOnOBjQuen));
		while((pobjNode!=NULL)&&(objNum<ptaskNode->actMOnOBjNum))
		{
			gpnStatMonObjSubFiltTpCfg((optObjOrient *)(&(pobjNode->pstatScanPort->localPort)),
				ptaskNode->cycClass, (void *)(psubFiltTpNode->psubFiltTp));
			
			objNum++;
			pobjNode = (stStatMonObjNode *)listNext((NODE *)pobjNode);
		}
	}

	return GPN_STAT_API_GEN_OK;
}
#endif
UINT32 gpnStatFuncApiDevStausNotify(UINT32 devIndex, UINT32 sta)
{
	/*cfg save about */

	/*data struct about */
	gpnStatDeviceRegist(devIndex, sta);
	return GPN_STAT_API_GEN_OK;
}

UINT32 gpnStatFuncApiFixPortRegist(gpnSockMsg *pgpnStatMsgSp, UINT32 len)
{
	UINT32 devIndex;
	UINT32 devPortTpNum;
	stStatPortObjCL *pPortObjCL;
	
	/*assert */
	if(pgpnStatMsgSp == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}
	
	devIndex = pgpnStatMsgSp->iMsgPara1;
	devPortTpNum = pgpnStatMsgSp->msgCellLen/sizeof(stStatPortObjCL);
	pPortObjCL = (stStatPortObjCL *)(pgpnStatMsgSp+1);
	
	/*cfg save about */

	/*data struct about */
	gpnStatFixPortRegist(devIndex, devPortTpNum, pPortObjCL);
	return GPN_STAT_API_GEN_OK;
}
UINT32 gpnStatFuncApiVariPortRegist(gpnSockMsg *pgpnStatMsgSp, UINT32 len)
{
	objLogicDesc localPIndex;

	/*assert */
	if(pgpnStatMsgSp == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}
	
	/*cfg save about */
	
	localPIndex.devIndex = pgpnStatMsgSp->iMsgPara6;
	localPIndex.portIndex = pgpnStatMsgSp->iMsgPara1;
	localPIndex.portIndex3 = pgpnStatMsgSp->iMsgPara2;
	localPIndex.portIndex4 = pgpnStatMsgSp->iMsgPara3;
	localPIndex.portIndex5 = pgpnStatMsgSp->iMsgPara4;
	localPIndex.portIndex6 = pgpnStatMsgSp->iMsgPara5;

	/*data struct about */
	gpnStatVariPortRegist(&localPIndex, GPN_STAT_API_GEN_ENABLE);
	return GPN_STAT_API_GEN_OK;
}

UINT32 gpnStatFuncApiVariPortUnRegist(gpnSockMsg *pgpnStatMsgSp, UINT32 len)
{
	objLogicDesc localPIndex;

	/*assert */
	if(pgpnStatMsgSp == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}
	
	/*cfg save about */
	
	localPIndex.devIndex = pgpnStatMsgSp->iMsgPara7;
	localPIndex.portIndex = pgpnStatMsgSp->iMsgPara2;
	localPIndex.portIndex3 = pgpnStatMsgSp->iMsgPara3;
	localPIndex.portIndex4 = pgpnStatMsgSp->iMsgPara4;
	localPIndex.portIndex5 = pgpnStatMsgSp->iMsgPara5;
	localPIndex.portIndex6 = pgpnStatMsgSp->iMsgPara6;

	/*data struct about */
	gpnStatVariPortRegist(&localPIndex, GPN_STAT_API_GEN_DISABLE);
	return GPN_STAT_API_GEN_OK;
}

UINT32 gpnStatFuncApiPeerPortReplaceNotify(gpnSockMsg *pgpnStatMsgSp, UINT32 len)
{
	optObjOrient oldPeer;
	optObjOrient newPeer;

	/*assert */
	if(pgpnStatMsgSp == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}
	
	/*cfg save about */
	
	oldPeer.devIndex = pgpnStatMsgSp->iMsgPara7;
	oldPeer.portIndex = pgpnStatMsgSp->iMsgPara2;
	oldPeer.portIndex3 = pgpnStatMsgSp->iMsgPara3;
	oldPeer.portIndex4 = pgpnStatMsgSp->iMsgPara4;
	oldPeer.portIndex5 = pgpnStatMsgSp->iMsgPara5;
	oldPeer.portIndex6 = pgpnStatMsgSp->iMsgPara6;
	
	newPeer.devIndex = pgpnStatMsgSp->iMsgParaE;
	newPeer.portIndex = pgpnStatMsgSp->iMsgPara9;
	newPeer.portIndex3 = pgpnStatMsgSp->iMsgParaA;
	newPeer.portIndex4 = pgpnStatMsgSp->iMsgParaB;
	newPeer.portIndex5 = pgpnStatMsgSp->iMsgParaC;
	newPeer.portIndex6 = pgpnStatMsgSp->iMsgParaD;
	
	gpnStatPeerPortReplaceNotify(&oldPeer, &newPeer);
	return GPN_STAT_API_GEN_OK;
}

UINT32 gpnStatFuncApiViewPortReplaceNotify(gpnSockMsg *pgpnStatMsgSp, UINT32 len)
{
	objLogicDesc localPort;
	objLogicDesc newView;

	/*assert */
	if(pgpnStatMsgSp == NULL)
	{
		return GPN_STAT_API_GEN_ERR;
	}
	
	/*cfg save about */
	
	localPort.devIndex = pgpnStatMsgSp->iMsgPara7;
	localPort.portIndex = pgpnStatMsgSp->iMsgPara2;
	localPort.portIndex3 = pgpnStatMsgSp->iMsgPara3;
	localPort.portIndex4 = pgpnStatMsgSp->iMsgPara4;
	localPort.portIndex5 = pgpnStatMsgSp->iMsgPara5;
	localPort.portIndex6 = pgpnStatMsgSp->iMsgPara6;
	newView.devIndex = pgpnStatMsgSp->iMsgParaE;
	newView.portIndex = pgpnStatMsgSp->iMsgPara9;
	newView.portIndex3 = pgpnStatMsgSp->iMsgParaA;
	newView.portIndex4 = pgpnStatMsgSp->iMsgParaB;
	newView.portIndex5 = pgpnStatMsgSp->iMsgParaC;
	newView.portIndex6 = pgpnStatMsgSp->iMsgParaD;
	
	gpnStatViewPortReplaceNotify(&localPort, &newView);
	
	return GPN_STAT_API_GEN_OK;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_STAT_FUNC_API_C_*/

