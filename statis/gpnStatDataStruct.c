/**********************************************************
* file name: gpnStatDataStruct.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-22
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_DATA_STRUCT_C_
#define _GPN_STAT_DATA_STRUCT_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdlib.h>
#include <string.h>

#include <lib/syslog.h>
#include <lib/log.h>

#include "gpnLog/gpnLogFuncApi.h"
#include "socketComm/gpnStatTypeDef.h"
#include "devCoreInfo/gpnDevCoreInfoFunApi.h"

#include "gpnStatTypeStruct.h"

#include "gpnStatDataStruct.h"

#include "gpnStatTimeProc.h"
#include "gpnStatProcMain.h"
#include "gpnStatDebug.h"


stEQUStatProcSpace *pgstEQUStatProcSpace = NULL;

stStatPTpVsSTpTemp gStatPTpVsSTpTemp[] =
{
	{IFM_FUN_ETH_TYPE,	GPN_STAT_SCAN_TYPE_ETH,			"FUN ETH"},
	{IFM_STMN_TYPE,		GPN_STAT_SCAN_TYPE_SDH_OH,		"SDH_STM1-N"},
	{IFM_VC4_TYPE,		GPN_STAT_SCAN_TYPE_SDH_HP,		"SDH_VC4"},
	{IFM_VC12_TYPE,		GPN_STAT_SCAN_TYPE_SDH_LP,		"SDH_VC12"},
	{IFM_E1_TYPE,		GPN_STAT_SCAN_TYPE_PDH,			"PDH_PPI"},
	{IFM_LSP_TYPE,		GPN_STAT_SCAN_TYPE_PTN_LSP,		"PTN LSP"},
	{IFM_PW_TYPE,		GPN_STAT_SCAN_TYPE_PTN_PW,		"PTN PW"},
	{IFM_PWE3E1_TYPE,	GPN_STAT_SCAN_TYPE_PWE3,		"PWE3_E1"},
	{IFM_EQU_TYPE,      GPN_STAT_SCAN_TYPE_EQU,			"EQU"},
	{IFM_SOFT_TYPE,     GPN_STAT_SCAN_TYPE_SOFT,		"SOFT"},
	{IFM_ENV_TYPE,      GPN_STAT_SCAN_TYPE_ENV,	    	"ENV"},
	{IFM_ETH_SFP_TYPE,	GPN_STAT_SCAN_TYPE_ETH_SFP,		"ETH SFP"},
	{IFM_VPLSPW_TYPE,	GPN_STAT_SCAN_TYPE_PTN_VPLSPW,	"PTN VPLS PW"},
	{IFM_VS_TYPE,		GPN_STAT_SCAN_TYPE_PTN_VS,		"PTN VS"},
	{IFM_VUNI_TYPE,	    GPN_STAT_SCAN_TYPE_PTN_V_UNI,	"PTN V_UNI"},
	{IFM_CFM_MEP_TYPE,	GPN_STAT_SCAN_TYPE_PTN_MEP,  	"PTN MEP"},

	/* lipf add for ipran, 2018/3/5 */
	{IFM_VS_2_TYPE,		GPN_STAT_SCAN_TYPE_PTN_VS,		"IPRAN VS"},
	{IFM_LSP_2_TYPE,	GPN_STAT_SCAN_TYPE_PTN_LSP,		"IPRAN LSP"},
	{IFM_TUNNEL_TYPE,	GPN_STAT_SCAN_TYPE_PTN_LSP,		"IPRAN TUNNEL"},
	{IFM_PW_2_TYPE,		GPN_STAT_SCAN_TYPE_PTN_PW,		"IPRAN PW"},
};

extern gstStatTypeWholwInfo *pgstStatTypeWholwInfo;

UINT32 gpnStatMakePortIndex(UINT32 devIndex, UINT32 devPortTpNum,
	stStatPortObjCL *pPortObjCL, objLogicDesc *portIndexBuff, objLogicDesc *upPtBuff)
{
	objLogicDesc defPortInfo;
	
	UINT32 portIndex;
	UINT32 slotId;
	UINT32 portIndex3;
	
	UINT32 sdh_stmn_phy_port;
	
	UINT32 sdh_phy_vc4_port;

	UINT32 sdh_phy_vc12_port;

	UINT32 smp_phy_port;

	UINT32 lastVc4FarthPT;
	UINT32 lastVc12FarthPT;
	UINT32 portNumb;
	
	//UINT32 retValue;
	UINT32 i;
	UINT32 j;
	
	sdh_stmn_phy_port = 0;
	sdh_phy_vc4_port = 0;
	sdh_phy_vc12_port = 0;
	
	lastVc4FarthPT = IFM_ALL_TYPE;
	lastVc12FarthPT = IFM_ALL_TYPE;
	//retValue = GPN_STAT_DBS_GEN_OK;

	portNumb = 0;
	slotId = DeviceIndex_GetSlot(devIndex);

	for(i=0;i<devPortTpNum;i++)
	{	
		defPortInfo.devIndex = GPN_STAT_DBS_32_FFFF;
		defPortInfo.portIndex = GPN_STAT_DBS_32_FFFF;
		defPortInfo.portIndex3 = GPN_STAT_DBS_32_FFFF;
		defPortInfo.portIndex4 = GPN_STAT_DBS_32_FFFF;
		defPortInfo.portIndex5 = GPN_STAT_DBS_32_FFFF;
		defPortInfo.portIndex6 = GPN_STAT_DBS_32_FFFF;
		
		if(pPortObjCL[i].portNum >= 100)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_EGP, "%s : illegal port num %d err\n\r",\
				__FUNCTION__, portNumb);
			gpnLog(GPN_LOG_L_ALERT, "%s : illegal port num %d err\n\r",\
				__FUNCTION__, portNumb);
			return GPN_STAT_DBS_GEN_ERR;
		}
		
		switch(pPortObjCL[i].portType)
		{
			case IFM_STMN_TYPE:
				if(pPortObjCL[i].portNum != 1)
				{
					GPN_STAT_DBS_PRINT(GPN_STAT_DBS_EGP, "%s : villed STM10 port num %d\n\r",\
						__FUNCTION__, pPortObjCL[i].portNum);
					gpnLog(GPN_LOG_L_ALERT, "%s : villed STM10 port num %d\n\r",\
						__FUNCTION__, pPortObjCL[i].portNum);
					return GPN_STAT_DBS_GEN_ERR;
				}

				portIndexBuff[portNumb] = defPortInfo;
				upPtBuff[portNumb] = defPortInfo;
				
				sdh_stmn_phy_port++;
				lastVc4FarthPT = IFM_STMN_TYPE;
				portIndex = SDH_PortIndex_Create(IFM_STMN_TYPE,slotId,
					sdh_stmn_phy_port,0,0);
				portIndexBuff[portNumb].devIndex = devIndex;
				portIndexBuff[portNumb].portIndex = portIndex;
				upPtBuff[portNumb].devIndex = GPN_STAT_DBS_32_NULL;
				upPtBuff[portNumb].portIndex = GPN_STAT_DBS_32_NULL;
				portNumb++;

				sdh_phy_vc4_port = 0;
				break;
				
			case IFM_VC4_TYPE:
				if(lastVc4FarthPT != IFM_STMN_TYPE)
				{
					GPN_STAT_DBS_PRINT(GPN_STAT_DBS_EGP, "%s : villed PHYVC4 farth port type %8x\n\r",\
						__FUNCTION__, lastVc4FarthPT);
					gpnLog(GPN_LOG_L_ALERT, "%s : villed PHYVC4 farth port type %8x\n\r",\
						__FUNCTION__, lastVc4FarthPT);
					return GPN_STAT_DBS_GEN_ERR;
				}
				if(pPortObjCL[i].portNum != 1)
				{
					GPN_STAT_DBS_PRINT(GPN_STAT_DBS_EGP, "%s : villed PHYVC4 port num %d\n\r",\
						__FUNCTION__, pPortObjCL[i].portNum);
					gpnLog(GPN_LOG_L_ALERT, "%s : villed PHYVC4 port num %d\n\r",\
						__FUNCTION__, pPortObjCL[i].portNum);
					return GPN_STAT_DBS_GEN_ERR;
				}

				portIndexBuff[portNumb] = defPortInfo;
				upPtBuff[portNumb] = defPortInfo;
				
				sdh_phy_vc4_port++;
				lastVc12FarthPT = IFM_VC4_TYPE;

				portIndex = SDH_PortIndex_Create(IFM_VC4_TYPE,slotId,
					sdh_stmn_phy_port,sdh_phy_vc4_port,0);
				portIndexBuff[portNumb].devIndex = devIndex;
				portIndexBuff[portNumb].portIndex = portIndex;
				portIndex = SDH_PortIndex_Create(lastVc4FarthPT,slotId,
					sdh_stmn_phy_port,0,0);				
				upPtBuff[portNumb].devIndex = devIndex;
				upPtBuff[portNumb].portIndex = portIndex;
				portNumb++;

				sdh_phy_vc12_port = 0;
				break;
				
			case IFM_VC12_TYPE:
				if(lastVc4FarthPT != IFM_STMN_TYPE)
				{
					GPN_STAT_DBS_PRINT(GPN_STAT_DBS_EGP, "%s : villed PHYVC4 farth port type %8x\n\r",\
						__FUNCTION__, lastVc4FarthPT);
					return GPN_STAT_DBS_GEN_ERR;
				}
				if(lastVc12FarthPT != IFM_VC4_TYPE)
				{
					GPN_STAT_DBS_PRINT(GPN_STAT_DBS_EGP, "%s : villed PHYVC12 farth port type %8x\n\r",\
						__FUNCTION__, lastVc12FarthPT);
					return GPN_STAT_DBS_GEN_ERR;
				}
				if(pPortObjCL[i].portNum != 63)
				{
					GPN_STAT_DBS_PRINT(GPN_STAT_DBS_EGP, "%s : villed VC12 port num %d\n\r",\
						__FUNCTION__, pPortObjCL[i].portNum);
					return GPN_STAT_DBS_GEN_ERR;
				}
				for(j=0;j<pPortObjCL[i].portNum;j++)
				{
					portIndexBuff[portNumb] = defPortInfo;
					upPtBuff[portNumb] = defPortInfo;
					
					sdh_phy_vc12_port++;

					portIndex = SDH_PortIndex_Create(IFM_VC12_TYPE,slotId,
						sdh_stmn_phy_port,sdh_phy_vc4_port,sdh_phy_vc12_port);
					portIndexBuff[portNumb].devIndex = devIndex;
					portIndexBuff[portNumb].portIndex = portIndex;
					portIndex = SDH_PortIndex_Create(lastVc12FarthPT,slotId,
						sdh_stmn_phy_port,sdh_phy_vc4_port,0);
					upPtBuff[portNumb].devIndex = devIndex;
					upPtBuff[portNumb].portIndex = portIndex;
					portNumb++;
				}
				break;

			case IFM_E1_TYPE:
				smp_phy_port = 0;
				for(j=0;j<pPortObjCL[i].portNum;j++)
				{
					portIndexBuff[portNumb] = defPortInfo;
					upPtBuff[portNumb] = defPortInfo;
					
					smp_phy_port++;
					portIndex = PDH_PortIndex_Create(IFM_E1_TYPE,slotId,smp_phy_port,1);
					portIndexBuff[portNumb].devIndex = devIndex;
					portIndexBuff[portNumb].portIndex = portIndex;
					upPtBuff[portNumb].devIndex = GPN_STAT_DBS_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_STAT_DBS_32_NULL;
					portNumb++;
				}
				break;
				
			case IFM_ETH_TYPE:
				smp_phy_port = 0;
				for(j=0;j<pPortObjCL[i].portNum;j++)
				{
					portIndexBuff[portNumb] = defPortInfo;
					upPtBuff[portNumb] = defPortInfo;
					
					smp_phy_port++;
					portIndex = SMP_PortIndex_Create(IFM_ETH_TYPE,slotId,smp_phy_port);
					portIndexBuff[portNumb].devIndex = devIndex;
					portIndexBuff[portNumb].portIndex = portIndex;
					upPtBuff[portNumb].devIndex = GPN_STAT_DBS_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_STAT_DBS_32_NULL;
					portNumb++;
				}
				break;
				
			case IFM_FUN_ETH_TYPE:
				smp_phy_port = 5000;
				for(j=0;j<pPortObjCL[i].portNum;j++)
				{
					portIndexBuff[portNumb] = defPortInfo;
					upPtBuff[portNumb] = defPortInfo;
					
					smp_phy_port++;
					portIndex = FUN_ETH_PortIndex_Create(IFM_FUN_ETH_TYPE,slotId,smp_phy_port);
					portIndexBuff[portNumb].devIndex = devIndex;
					portIndexBuff[portNumb].portIndex = portIndex;
					upPtBuff[portNumb].devIndex = GPN_STAT_DBS_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_STAT_DBS_32_NULL;
					portNumb++;
				}
				break;
				
			case IFM_V35_TYPE:
				smp_phy_port = 0;
				for(j=0;j<pPortObjCL[i].portNum;j++)
				{
					portIndexBuff[portNumb] = defPortInfo;
					upPtBuff[portNumb] = defPortInfo;
					
					smp_phy_port++;
					portIndex = SMP_PortIndex_Create(IFM_V35_TYPE,slotId,smp_phy_port);
					portIndexBuff[portNumb].devIndex = devIndex;
					portIndexBuff[portNumb].portIndex = portIndex;
					upPtBuff[portNumb].devIndex = GPN_STAT_DBS_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_STAT_DBS_32_NULL;
					portNumb++;
				}
				break;

			case IFM_PWE3E1_TYPE:
				smp_phy_port = 0;
				for(j=0;j<pPortObjCL[i].portNum;j++)
				{
					portIndexBuff[portNumb] = defPortInfo;
					upPtBuff[portNumb] = defPortInfo;
					
					smp_phy_port++;
					portIndex = SMP_PortIndex_Create(IFM_PWE3E1_TYPE,slotId,smp_phy_port);
					portIndexBuff[portNumb].devIndex = devIndex;
					portIndexBuff[portNumb].portIndex = portIndex;
					upPtBuff[portNumb].devIndex = GPN_STAT_DBS_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_STAT_DBS_32_NULL;
					portNumb++;
				}
				break;
				
			case IFM_SOFT_TYPE:
			case IFM_POWN48_TYPE:
			case IFM_POW220_TYPE:
			case IFM_ENV_TYPE:
			case IFM_FAN_TYPE:
			case IFM_EQU_TYPE:
			case IFM_ETH_CLC_IN_TYPE:
			case IFM_ETH_CLC_OUT_TYPE:
			case IFM_ETH_CLC_SUB_TYPE:
			case IFM_ETH_CLC_LINE_TYPE:
			case IFM_ETH_SFP_TYPE:
				smp_phy_port = 0;
				for(j=0;j<pPortObjCL[i].portNum;j++)
				{
					portIndexBuff[portNumb] = defPortInfo;
					upPtBuff[portNumb] = defPortInfo;
					
					smp_phy_port++;
					portIndex = SMP_PortIndex_Create(pPortObjCL[i].portType,slotId,smp_phy_port);
					portIndexBuff[portNumb].devIndex = devIndex;
					portIndexBuff[portNumb].portIndex = portIndex;
					upPtBuff[portNumb].devIndex = GPN_STAT_DBS_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_STAT_DBS_32_NULL;
					portNumb++;
				}
				break;

			/*just for test, not use in real device */
			case IFM_VS_TYPE:
			case IFM_LSP_TYPE:
			case IFM_PW_TYPE:
				smp_phy_port = 0;
				for(j=0;j<pPortObjCL[i].portNum;j++)
				{
					portIndexBuff[portNumb] = defPortInfo;
					upPtBuff[portNumb] = defPortInfo;
					
					smp_phy_port++;
					portIndex = LAB_PortIndex_Create(pPortObjCL[i].portType, slotId, smp_phy_port/*labal*/);
					portIndexBuff[portNumb].devIndex = devIndex;
					portIndexBuff[portNumb].portIndex = portIndex;
					upPtBuff[portNumb].devIndex = GPN_STAT_DBS_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_STAT_DBS_32_NULL;
					portNumb++;
				}
				break;

			/* lipf add for ipran, 2018/3/5 */
			case IFM_VS_2_TYPE:
			case IFM_LSP_2_TYPE:
			case IFM_PW_2_TYPE:
				smp_phy_port = 0;
				for(j=0;j<pPortObjCL[i].portNum;j++)
				{
					portIndexBuff[portNumb] = defPortInfo;
					upPtBuff[portNumb] = defPortInfo;
					
					smp_phy_port++;
					portIndex = LAB_PortIndex_Create(pPortObjCL[i].portType, slotId, 0/*labal*/);
					portIndex3 = smp_phy_port;
					portIndexBuff[portNumb].devIndex = devIndex;
					portIndexBuff[portNumb].portIndex = portIndex;
					upPtBuff[portNumb].devIndex = GPN_STAT_DBS_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_STAT_DBS_32_NULL;
					portNumb++;
				}
				break;
				
			case IFM_VPLSPW_TYPE:
				smp_phy_port = 0;
				for(j=0;j<pPortObjCL[i].portNum;j++)
				{
					portIndexBuff[portNumb] = defPortInfo;
					upPtBuff[portNumb] = defPortInfo;
					
					smp_phy_port++;
					portIndex = LAB_PortIndex_Create(pPortObjCL[i].portType, slotId, smp_phy_port/*vsi -d*/);
					portIndex3 = LAB_PortIndex_Create(pPortObjCL[i].portType, slotId, smp_phy_port/*vc-id*/);
					portIndexBuff[portNumb].devIndex = devIndex;
					portIndexBuff[portNumb].portIndex = portIndex;
					portIndexBuff[portNumb].portIndex3 = portIndex3;
					upPtBuff[portNumb].devIndex = GPN_STAT_DBS_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_STAT_DBS_32_NULL;
					portIndexBuff[portNumb].portIndex3 = GPN_STAT_DBS_32_NULL;
					portNumb++;
				}
				break;
			default:
				gpnLog(GPN_LOG_L_ERR, "%s : villed port type %08x\n\r",\
					__FUNCTION__, pPortObjCL[i].portType);
				return GPN_STAT_DBS_GEN_ERR;
		}
	}
	
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatTimePolarityGet(void)
{
	return pgstEQUStatProcSpace->statGlobalInfo.polarity;
}

UINT32 gpnStatTimeOffsetGet(void)
{
	return pgstEQUStatProcSpace->statGlobalInfo.offSet;
}

UINT32 gpnStatMaxMonObjGet(void)
{
	return pgstEQUStatProcSpace->statTaskInfo.maxMonObjNum;
}

UINT32 gpnStatRunMonObjGet(void)
{
	return pgstEQUStatProcSpace->statTaskInfo.runMonObjNum;
}

UINT32 gpnStatLastHistReportCfgGet(void)
{
	return pgstEQUStatProcSpace->statGlobalInfo.lastHistRept;
}

UINT32 gpnStatLastHistReportCfgSet(UINT32 opt)
{
	if( (opt == GPN_STAT_DBS_GEN_ENABLE) ||\
		(opt == GPN_STAT_DBS_GEN_DISABLE) )
	{
		pgstEQUStatProcSpace->statGlobalInfo.lastHistRept = opt;
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
}

UINT32 gpnStatOldHistReportCfgGet(void)
{
	return pgstEQUStatProcSpace->statGlobalInfo.oldHistRept;
}

UINT32 gpnStatOldHistReportCfgSet(UINT32 opt)
{
	if( (opt == GPN_STAT_DBS_GEN_ENABLE) ||\
		(opt == GPN_STAT_DBS_GEN_DISABLE) )
	{
		pgstEQUStatProcSpace->statGlobalInfo.oldHistRept = opt;
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
}

UINT32 gpnStatAlarmReportCfgGet(void)
{
	return pgstEQUStatProcSpace->statGlobalInfo.alarmRept;
}

UINT32 gpnStatAlarmReportCfgSet(UINT32 opt)
{
	if( (opt == GPN_STAT_DBS_GEN_ENABLE) ||\
		(opt == GPN_STAT_DBS_GEN_DISABLE) )
	{
		pgstEQUStatProcSpace->statGlobalInfo.alarmRept = opt;
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
}

UINT32 gpnStatEventReportCfgGet(void)
{
	return pgstEQUStatProcSpace->statGlobalInfo.eventRept;
}

UINT32 gpnStatEvnetReportCfgSet(UINT32 opt)
{
	if( (opt == GPN_STAT_DBS_GEN_ENABLE) ||\
		(opt == GPN_STAT_DBS_GEN_DISABLE) )
	{
		pgstEQUStatProcSpace->statGlobalInfo.eventRept = opt;
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
}

UINT32 gpnStatTaskIdGet(void)
{
	stStatTaskNode *ptaskNode;
	
	if(pgstEQUStatProcSpace->statTaskInfo.idleTaskNum > 0)
	{
		ptaskNode = (stStatTaskNode *)listFirst(&(pgstEQUStatProcSpace->statTaskInfo.idleTaskQuen));
		if(ptaskNode != NULL)
		{
			return ptaskNode->taskId;
		}
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : no idleNode\n\r",\
		__FUNCTION__);
	
	return 0;
}
UINT32 gpnStatTaskIdCheck(UINT32 taskId)
{
	stStatTaskNode *ptaskNode;
	
	if(pgstEQUStatProcSpace->statTaskInfo.idleTaskNum > 0)
	{
		ptaskNode = (stStatTaskNode *)listFirst(&(pgstEQUStatProcSpace->statTaskInfo.idleTaskQuen));
		if(ptaskNode != NULL)
		{
			if(ptaskNode->taskId == taskId)
			{
				return GPN_STAT_DBS_GEN_OK;
			}
		}
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : taskId(%d) not vilad\n\r",\
		__FUNCTION__, taskId);
	
	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatMaxTaskNumGet(void)
{
	return pgstEQUStatProcSpace->statTaskInfo.maxTaskNum;
}

UINT32 gpnStatRunTaskNumGet(void)
{
	return pgstEQUStatProcSpace->statTaskInfo.runTaskNum;
}

UINT32 gpnStatEventThredTemplatIdGet(void)
{
	stStatEvnThredTpNode *pthredTpNode;

	if(pgstEQUStatProcSpace->statEventThredTemp.idleThredTpNum> 0)
	{
		pthredTpNode = (stStatEvnThredTpNode *)listFirst(&(pgstEQUStatProcSpace->statEventThredTemp.idleThredTpQuen));
		if(pthredTpNode != NULL)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get thredTpId(%d)\n\r",\
				__FUNCTION__, pthredTpNode->thredTpId);
			return pthredTpNode->thredTpId;
		}
	}
	
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : no idleNode\n\r",\
		__FUNCTION__);
	
	return 0;
}

UINT32 gpnStatEventThredTemplatIdCheck(UINT32 evnTherdId)
{
	stStatEvnThredTpNode *pthredTpNode;

	if(pgstEQUStatProcSpace->statEventThredTemp.idleThredTpNum> 0)
	{
		pthredTpNode = (stStatEvnThredTpNode *)listFirst(&(pgstEQUStatProcSpace->statEventThredTemp.idleThredTpQuen));
		if(pthredTpNode != NULL)
		{
			if(pthredTpNode->thredTpId == evnTherdId)
			{
				return GPN_STAT_DBS_GEN_OK;
			}
		}
	}
	
	gpnLog(GPN_LOG_L_ERR, "%s : evnTherdId(%d) not vilad\n\r",\
		__FUNCTION__, evnTherdId);
	
	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatMaxEventThredTpNumGet(void)
{
	return pgstEQUStatProcSpace->statEventThredTemp.maxThredTpNum;
}

UINT32 gpnStatRunEventThredTpNumGet(void)
{
	return pgstEQUStatProcSpace->statEventThredTemp.runThredTpNum;
}
UINT32 gpnStatAlarmThredTemplatIdGet(void)
{
	stStatAlmThredTpNode *pthredTpNode;

	if(pgstEQUStatProcSpace->statAlarmThredTemp.idleThredTpNum> 0)
	{
		pthredTpNode = (stStatAlmThredTpNode *)listFirst(&(pgstEQUStatProcSpace->statAlarmThredTemp.idleThredTpQuen));
		if(pthredTpNode != NULL)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get thredTpId(%d)\n\r",\
				__FUNCTION__, pthredTpNode->thredTpId);
			return pthredTpNode->thredTpId;
		}
	}
	
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : no idleNode\n\r",\
		__FUNCTION__);
	
	return 0;
}

UINT32 gpnStatAlarmThredTemplatIdCheck(UINT32 almThredId)
{
	stStatAlmThredTpNode *pthredTpNode;

	if(pgstEQUStatProcSpace->statAlarmThredTemp.idleThredTpNum> 0)
	{
		pthredTpNode = (stStatAlmThredTpNode *)listFirst(&(pgstEQUStatProcSpace->statAlarmThredTemp.idleThredTpQuen));
		if(pthredTpNode != NULL)
		{
			if(pthredTpNode->thredTpId == almThredId)
			{
				return GPN_STAT_DBS_GEN_OK;
			}
		}
	}
	
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : almThredId(%d) not vilad\n\r",\
		__FUNCTION__, almThredId);
	
	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatMaxAlarmThredTpNumGet(void)
{
	return pgstEQUStatProcSpace->statAlarmThredTemp.maxThredTpNum;
}

UINT32 gpnStatRunAlarmThredTpNumGet(void)
{
	return pgstEQUStatProcSpace->statAlarmThredTemp.runThredTpNum;
}

UINT32 gpnStatSubTypeFiltIdGet(void)
{
	stStatSubFiltTpNode *psubFiltTpsNode;

	if(pgstEQUStatProcSpace->statSubFiltTemp.idleSubFiltTpNum> 0)
	{
		psubFiltTpsNode = (stStatSubFiltTpNode *)listFirst(&(pgstEQUStatProcSpace->statSubFiltTemp.idleSubFiltTpQuen));
		if(psubFiltTpsNode != NULL)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get SubTypeFiltId(%d)\n\r",\
				__FUNCTION__, psubFiltTpsNode->subFiltTpId);
			return psubFiltTpsNode->subFiltTpId;
		}
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : no idleNode\n\r",\
		__FUNCTION__);
	
	return 0;
}

UINT32 gpnStatSubTypeFiltIdCheck(UINT32 subFiltId)
{
	stStatSubFiltTpNode *psubFiltTpsNode;

	if(pgstEQUStatProcSpace->statSubFiltTemp.idleSubFiltTpNum> 0)
	{
		psubFiltTpsNode = (stStatSubFiltTpNode *)listFirst(&(pgstEQUStatProcSpace->statSubFiltTemp.idleSubFiltTpQuen));
		if(psubFiltTpsNode != NULL)
		{
			if(psubFiltTpsNode->subFiltTpId == subFiltId)
			{
				return GPN_STAT_DBS_GEN_OK;
			}
		}
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : subFiltId(%d) not vilad!\n\r",\
		__FUNCTION__, subFiltId);
	
	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatMaxSubFiltTpNumGet(void)
{
	return pgstEQUStatProcSpace->statSubFiltTemp.maxSubFiltTpNum;
}

UINT32 gpnStatRunSubFiltTpNumGet(void)
{
	return pgstEQUStatProcSpace->statSubFiltTemp.runSubFiltTpNum;
}

UINT32 gpnStatSubTypeReportIdGet(void)
{
	stStatSubReportTpNode *psubReportTpsNode;

	if(pgstEQUStatProcSpace->statSubReportTemp.idleSubReportTpNum> 0)
	{
		psubReportTpsNode = (stStatSubReportTpNode *)listFirst(&(pgstEQUStatProcSpace->statSubReportTemp.idleSubReportTpQuen));
		if(psubReportTpsNode != NULL)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get SubTypeReportId(%d)\n\r",\
				__FUNCTION__, psubReportTpsNode->subReportTpId);
			return psubReportTpsNode->subReportTpId;
		}
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : no idleNode\n\r",\
		__FUNCTION__);
	
	return 0;
}

UINT32 gpnStatSubTypeReportIdCheck(UINT32 subThredId)
{
	stStatSubReportTpNode *psubReportTpsNode;

	if(pgstEQUStatProcSpace->statSubReportTemp.idleSubReportTpNum> 0)
	{
		psubReportTpsNode = (stStatSubReportTpNode *)listFirst(&(pgstEQUStatProcSpace->statSubReportTemp.idleSubReportTpQuen));
		if(psubReportTpsNode != NULL)
		{
			if(psubReportTpsNode->subReportTpId == subThredId)
			{
				return GPN_STAT_DBS_GEN_OK;
			}
		}
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : subReportId(%d) not vilad!\n\r",\
		__FUNCTION__, subThredId);
	
	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatMaxSubReportTpNumGet(void)
{
	return pgstEQUStatProcSpace->statSubReportTemp.maxSubReportTpNum;
}

UINT32 gpnStatRunSubReportTpNumGet(void)
{
	return pgstEQUStatProcSpace->statSubReportTemp.runSubReportTpNum;
}

UINT32 gpnStatCountTypeGet(void)
{
	return pgstEQUStatProcSpace->statGlobalInfo.conutType;
}

UINT32 gpnStatDeviceRegist(UINT32 devIndex, UINT32 sta)
{
	if(sta == GPN_STAT_DEV_STA_NULL)
	{
		gpnStatDeviceDestroy(devIndex);
	}
	else if(sta == GPN_STAT_DEV_STA_PULL)
	{
		
	}
	else if(sta == GPN_STAT_DEV_STA_INSERT)
	{
		//gpnStatDeviceDistribution(devIndex);
	}
	else if(sta == GPN_STAT_DEV_STA_RUN)
	{
		
	}
	else if(sta == GPN_STAT_DEV_STA_ERR)
	{
		
	}
	else
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatDeviceDistribution(UINT32 devIndex)
{
	/*stRemDevStatProcNode *pRemDevStatProcNode;
	UINT32 remDevNum;
	UINT32 remDevOn;*/

#if 0
	UINT32 slotId;
	UINT32 level;
	
	slotId = (UINT32)DeviceIndex_GetSlot(devIndex);
	level = (UINT32)DeviceIndex_GetLevel(devIndex);
	
	if(level == IFM_DEV_LEVEL_CENTER)
	{
		/*center device should be in suitable sta for distribution */
		if(pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta == GPN_STAT_DEV_STA_NULL)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : level(%d) slot(%d) chang sta from %d to %d\n\r",\
				__FUNCTION__, level, slotId,\
				pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta, GPN_STAT_DEV_STA_INSERT);
			
			pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta = GPN_STAT_DEV_STA_INSERT;	

			/*for memrey malloc */
			/*???*/
		}
		else
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : level(%d) slot(%d) just has sta %d\n\r",\
				__FUNCTION__, level, slotId,\
				pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta);
		}
	}
	else if((level == IFM_DEV_LEVEL_NERREM) || (level == IFM_DEV_LEVEL_FAREDN))
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : level(%d) slot(%d) just has sta %d\n\r",\
			__FUNCTION__, level, slotId,\
			pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta);
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : index(%08x) level(%d) err\n\r",\
			__FUNCTION__, devIndex, level);
		return GPN_STAT_DBS_GEN_OK;
	}
#endif	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatDeviceDestroy(UINT32 devIndex)
{
	/*stRemDevStatProcNode *pRemDevStatProcNode;
	UINT32 remDevNum;
	UINT32 remDevOn;*/
#if 0
	UINT32 slotId;
	UINT32 level;
	
	slotId = (UINT32)DeviceIndex_GetSlot(devIndex);
	level = (UINT32)DeviceIndex_GetLevel(devIndex);
	
	if(level == IFM_DEV_LEVEL_CENTER)
	{
		/*center device should be in suitable sta */
		if( (pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta == GPN_STAT_DEV_STA_INSERT) ||\
			(pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta == GPN_STAT_DEV_STA_RUN) ||\
			(pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta == GPN_STAT_DEV_STA_ERR) )
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : level(%d) slot(%d) chang sta from %d to %d\n\r",\
				__FUNCTION__, level, slotId,\
				pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta, GPN_STAT_DEV_STA_NULL);
			
			pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta = GPN_STAT_DEV_STA_NULL;	

			/*clear memrey */
			/*???*/
		}
		else
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : level(%d) slot(%d) just has sta %d\n\r",\
				__FUNCTION__, level, slotId,\
				pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta);
		}
	}
	else if((level == IFM_DEV_LEVEL_NERREM) || (level == IFM_DEV_LEVEL_FAREDN))
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : level(%d) slot(%d) just has sta %d\n\r",\
			__FUNCTION__, level, slotId,\
			pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta);
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : index(%08x) level(%d) err\n\r",\
			__FUNCTION__, devIndex, level);
		return GPN_STAT_DBS_GEN_OK;
	}
#endif	
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatPeerPortReplaceNotify(optObjOrient *pOldPeerP, optObjOrient *pNewPeerP)
{
	UINT32 slotId;
	UINT32 portType;
	UINT32 hashKey;
	stPortTpToStatScanTp *pPortTpToScanTp;
	stStatPreScanQuen *pStatPreScanNode;
	stStatPeerPortNode *pStatOldPeer;
	stStatPeerPortNode *pStatNewPeer;

	/*assert */
	if( (pOldPeerP == NULL) ||\
		(pNewPeerP == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*
		step 1 : if old peer is not exit,  err
	*/
	pStatOldPeer = NULL;
	gpnStatSeekPeerPort2PeerPortNode(pOldPeerP, &pStatOldPeer);
	if(pStatOldPeer == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : old peer port(%08x-%08x|%08x|%08x|%08x|%08x) not exit, err!\n\r",\
			__FUNCTION__, pOldPeerP->devIndex, pOldPeerP->portIndex, pOldPeerP->portIndex3,\
			pOldPeerP->portIndex4, pOldPeerP->portIndex5, pOldPeerP->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*
		step 2 : if new peer is exit, if exit, err
	*/
	pStatNewPeer = NULL;
	gpnStatSeekPeerPort2PeerPortNode(pNewPeerP, &pStatNewPeer);
	if(pStatNewPeer != NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : new peer port(%08x-%08x|%08x|%08x|%08x|%08x) alreadly exit, err!\n\r",\
			__FUNCTION__, pNewPeerP->devIndex, pNewPeerP->portIndex, pNewPeerP->portIndex3,\
			pNewPeerP->portIndex4, pNewPeerP->portIndex5, pNewPeerP->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*
		step 3 : delete old peer node out of perScanQuen
	*/
	slotId = PortIndex_GetSlot(pOldPeerP->portIndex);
	portType = PortIndex_GetType(pOldPeerP->portIndex);
	
	pPortTpToScanTp = NULL;
	gpnStatSeekPortType2PTVsST(portType, &pPortTpToScanTp);
	if(pPortTpToScanTp == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : peer port(%08x-%08x|%08x|%08x|%08x|%08x) can't cross scanType, err!\n\r",\
			__FUNCTION__, pOldPeerP->devIndex, pOldPeerP->portIndex, pOldPeerP->portIndex3,\
			pOldPeerP->portIndex4, pOldPeerP->portIndex5, pOldPeerP->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatPreScanNode = pPortTpToScanTp->pStatPreScanNode[slotId];
	hashKey = (pOldPeerP->portIndex)%(pStatPreScanNode->hashKey);
	
	listDelete(&(pStatPreScanNode->statPeerPortQuen[hashKey]), (NODE *)pStatOldPeer);
	pStatPreScanNode->peerPortNodeNum[hashKey]--;

	/*
		step 4 : add new peer node into perScanQuen
	*/
	slotId = PortIndex_GetSlot(pNewPeerP->portIndex);
	portType = PortIndex_GetType(pNewPeerP->portIndex);
	
	pPortTpToScanTp = NULL;
	gpnStatSeekPortType2PTVsST(portType, &pPortTpToScanTp);
	if(pPortTpToScanTp == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : peer port(%08x-%08x|%08x|%08x|%08x|%08x) can't cross scanType,err!\n\r",\
			__FUNCTION__, pNewPeerP->devIndex, pNewPeerP->portIndex, pNewPeerP->portIndex3,\
			pNewPeerP->portIndex4, pNewPeerP->portIndex5, pNewPeerP->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatPreScanNode = pPortTpToScanTp->pStatPreScanNode[slotId];
	hashKey = (pNewPeerP->portIndex)%(pStatPreScanNode->hashKey);
	
	/* peer port replace */
	memcpy(&(pStatOldPeer->peerPort), pNewPeerP, sizeof(optObjOrient));
	
	listAdd(&(pStatPreScanNode->statPeerPortQuen[hashKey]), (NODE *)pStatOldPeer);
	pStatPreScanNode->peerPortNodeNum[hashKey]++;

	gpnLog(GPN_LOG_L_INFO, "%s : old port(%08x-%08x|%08x|%08x|%08x|%08x) replaced by new port(%08x-%08x|%08x|%08x|%08x|%08x\n\r",\
		__FUNCTION__, pOldPeerP->devIndex, pOldPeerP->portIndex, pOldPeerP->portIndex3,\
		pOldPeerP->portIndex4, pOldPeerP->portIndex5, pOldPeerP->portIndex6,\
		pNewPeerP->devIndex, pNewPeerP->portIndex, pNewPeerP->portIndex3,\
		pNewPeerP->portIndex4, pNewPeerP->portIndex5, pNewPeerP->portIndex6);

	/*
		step 5 : notify process modu this replace if port add in validQuen
		here SMP for fix notify NSM
	*/
	if(pStatOldPeer->pStatScanPort->scanQuenValid == GPN_STAT_DBS_GEN_ENABLE)
	{
		gpnSockStatMsgApiEthMonStatMonPortReplace(pOldPeerP, pNewPeerP);
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatViewPortReplaceNotify(objLogicDesc *pLocalPort, objLogicDesc *pNewViewP)
{
	stStatLocalPortNode *pStatLocalNode;

	/*assert */
	if( (pLocalPort == NULL) ||\
		(pNewViewP == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*
		step 1 : if local port is exit, if not, err
	*/
	pStatLocalNode = NULL;
	gpnStatSeekLocalPort2LocalPortNode(pLocalPort, &pStatLocalNode);
	if(pStatLocalNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) not exit, err!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*
		step 2 : replace new view port
	*/
	memcpy(&(pStatLocalNode->viewPort), pNewViewP, sizeof(objLogicDesc));

	return GPN_STAT_DBS_GEN_YES;
}

UINT32 gpnStatVariPortRegist(objLogicDesc *pLocalPort, UINT32 opt)
{
	UINT32 reVal;
	stStatLocalPortNode *pStatLocalNode;

	/*assert */
	if(pLocalPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	if( (opt != GPN_STAT_DBS_GEN_ENABLE) &&\
		(opt != GPN_STAT_DBS_GEN_DISABLE) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pLocalPort->devIndex = pLocalPort->devIndex;

	gpnLog(GPN_LOG_L_INFO, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) opt %d!\n\r",\
		__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
		pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6, opt);
	/* search pStatLocalNode in statPerScanQuen, if exist, port already reg, if not, could reg port */
	pStatLocalNode = NULL;
	gpnStatSeekLocalPort2LocalPortNode(pLocalPort, &pStatLocalNode);

	if(pStatLocalNode == NULL)
	{
		gpnLog(GPN_LOG_L_INFO, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) not exist in perScan quen!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
		
		if(opt == GPN_STAT_DBS_GEN_ENABLE)
		{
			reVal = gpnStatVariPortDistribution(pLocalPort);
			if (reVal == GPN_STAT_DBS_GEN_ERR)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : vari port distribution failed!\n\r",\
					__FUNCTION__);
				return GPN_STAT_DBS_GEN_ERR;
			}
			gpnLog(GPN_LOG_L_INFO, "%s : vari port distribution ok!\n\r",\
				__FUNCTION__);
		}
		else
		{
			gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) opt(%d) err!\n\r",\
				__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
				pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6, opt);
			return GPN_STAT_DBS_GEN_ERR;
		}
	}
	else
	{
		gpnLog(GPN_LOG_L_INFO, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) opt(%d) exist!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6, opt);
		
		if(opt == GPN_STAT_DBS_GEN_DISABLE)
		{
			reVal = gpnStatVariPortDestroy(pLocalPort);
			if (reVal == GPN_STAT_DBS_GEN_ERR)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : destroy vari port failed!\n\r",\
					__FUNCTION__);
				return GPN_STAT_DBS_GEN_ERR;
			}
			gpnLog(GPN_LOG_L_INFO, "%s : destroy vari port ok!\n\r",\
				__FUNCTION__);
		}
		else
		{
			gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) opt(%d) err!\n\r",\
				__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
				pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6, opt);
			return GPN_STAT_DBS_GEN_ERR;
		}
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatVariPortDistribution(objLogicDesc *pLocalPort)
{
	UINT32 i;
	UINT32 reVal;
	UINT8 *nodeSpace;
	
	stStatPreScanIndex *pStatPreScanIndex;
	stStatLocalPortNode *pStatLocalNode;
	stStatPeerPortNode *pStatPeerNode;
	stStatScanPortInfo *pStatScanPort;
	
	/*assert */
	if(pLocalPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* calculat memrey space : total ports node & middle link node */
	i = sizeof(stStatPreScanIndex) +\
		sizeof(stStatLocalPortNode) +\
		sizeof(stStatPeerPortNode) +\
		sizeof(stStatScanPortInfo);

	gpnLog(GPN_LOG_L_INFO, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) need malloc %d byte space!\n\r",\
		__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
		pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6, i);
	
	nodeSpace = (UINT8 *)malloc(i);
	if(nodeSpace == NULL)
	{
		perror("malloc");
		gpnLog(GPN_LOG_L_ERR, "%s : malloc err!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	memset(nodeSpace, 0, i);

	/*cut memrey to three part : 1 middle link node; 2 node; */
	pStatPreScanIndex = (stStatPreScanIndex *)nodeSpace;
	pStatLocalNode = (stStatLocalPortNode *)(pStatPreScanIndex + 1);
	pStatPeerNode = (stStatPeerPortNode *)(pStatLocalNode + 1);
	pStatScanPort = (stStatScanPortInfo *)(pStatPeerNode + 1);

	/* stat port node init, use middle link node */
	pStatPreScanIndex->pStatLocalPort = pStatLocalNode;
	gpnStatScanPortNodeInit(pLocalPort,\
		pStatLocalNode, pStatPeerNode, pStatScanPort);

	/* mark port add in node-quen, used when delete node */
	pStatScanPort->addQuenMode = GPN_STAT_VARI_MODE;

	/* add port to pre scan quen */
	reVal = gpnStatNodeRelevChg2PreScanQuen(pStatPreScanIndex, GPN_STAT_DBS_GEN_ENABLE);
	if (reVal == GPN_STAT_DBS_GEN_ERR)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : add port to pre scan quen failed!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*add port to valid scan quen */
	/*reVal = gpnStatNodeRelevChg2ValidScanQuen(pStatLocalNode, GPN_STAT_DBS_GEN_ENABLE);*/

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatVariPortDestroy(objLogicDesc *pLocalPort)
{
	stStatPreScanIndex *pStatPreScanIndex;
	UINT32 reVal;

	/*assert */
	if(pLocalPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatPreScanIndex = NULL;
	gpnStatSeekLocalPort2PreScanNodeIndex(pLocalPort, &pStatPreScanIndex);
	if(pStatPreScanIndex == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP,"%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) not exist, err!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* first delete from validQuen */
	reVal = gpnStatNodeRelevChg2ValidScanQuen(pStatPreScanIndex->pStatLocalPort, GPN_STAT_DBS_GEN_DISABLE);
	if (reVal == GPN_STAT_DBS_GEN_ERR)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/* second delete from perQuen */
	reVal = gpnStatNodeRelevChg2PreScanQuen(pStatPreScanIndex, GPN_STAT_DBS_GEN_DISABLE);
	if (reVal == GPN_STAT_DBS_GEN_ERR)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	/* releas port scan about space */
	reVal = gpnStatScanPortNodeDestroy(pStatPreScanIndex);
	if (reVal == GPN_STAT_DBS_GEN_ERR)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : gpnStatScanPortNodeDestroy return err!\n\r", __FUNCTION__);
		
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* release vari port node space */
	if(pStatPreScanIndex->pStatLocalPort->pStatScanPort->addQuenMode == GPN_STAT_VARI_MODE)
	{
		free(pStatPreScanIndex);
	}
	return GPN_STAT_DBS_GEN_OK;
}

/*once fix interface regist, will not unregist */
UINT32 gpnStatFixPortRegist(UINT32 devIndex,
	UINT32 devPortTpNum, stStatPortObjCL *pPortObjCL)
{	
	stPortTpToStatScanTp *pPortTpToScanTp = NULL;
	//stRemDevStatProcNode *pRemDevStatProcNode;

	stStatPreScanIndex *pStatPreScanIndex = NULL;
	stStatLocalPortNode *pStatLocalNode = NULL;
	stStatPeerPortNode *pStatPeerNode = NULL;
	stStatScanPortInfo *pStatScanPort = NULL;

	stStatPortObjCL *pStatPortObjCL = NULL;
		
	objLogicDesc *portIndexBuff = NULL;
	objLogicDesc *upperPortIndexBuff = NULL;

	//UINT32 slotId;
	//UINT32 level;
	UINT32 portType;

	UINT32 statScanPortNum;
	UINT32 statDevPortNum;

	UINT32 sysPortTypeNum;
	//UINT32 remEevNum;
	UINT32 i;

	/*get slot and devic level from devIndex */
#if 0
	slotId = DeviceIndex_GetSlot(devIndex);
	level = DeviceIndex_GetLevel(devIndex);
	gpnLog(GPN_LOG_L_INFO, "%s : device(%08x) slot(%d) level(%d)\n\r",\
		__FUNCTION__, devIndex, slotId, level);
#endif

	/* calculat memrey space stat */
	statScanPortNum = 0;
	statDevPortNum = 0;
	pStatPortObjCL = (stStatPortObjCL *) malloc(devPortTpNum * sizeof(stStatPortObjCL));
	if(pStatPortObjCL == NULL)
	{
		perror("pStatPortObjCL malloc err");
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	for(i=0;i<devPortTpNum;i++)
	{	
		sysPortTypeNum = 0;
		pPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(
			&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));
		while( (pPortTpToScanTp != NULL) &&\
			   (sysPortTypeNum < pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum) )
		{
			if(pPortTpToScanTp->portType == pPortObjCL[i].portType)
			{
				/* calculate for scan port num : used for malloc port node space */
				statScanPortNum += pPortObjCL[i].portNum;

				memcpy(&(pStatPortObjCL[statDevPortNum].portType), &(pPortObjCL[i].portType), sizeof(stStatPortObjCL));
				statDevPortNum++;
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : statDevPortNum:%d portType(%08x)(%16s) num(%3d) perNode(%d) localNode(%d) peerNode(%d) portInfo(%d) dataSize(%d)\n\r",\
					__FUNCTION__, statDevPortNum, pPortObjCL[i].portType, pPortTpToScanTp->portTpName, pPortObjCL[i].portNum,\
					sizeof(stStatPreScanIndex), sizeof(stStatLocalPortNode),\
					sizeof(stStatPeerPortNode), sizeof(stStatScanPortInfo),\
					pPortTpToScanTp->statDataSizeForPort);
				break;
			}
			
			sysPortTypeNum++;
			pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
		}

		if( (pPortTpToScanTp == NULL) ||\
			(sysPortTypeNum >= pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum) )
		{
			gpnLog(GPN_LOG_L_ERR, "%s : stStatPortObjCL->portType(%08x) not found, err!\n\r",\
				__FUNCTION__, pPortObjCL[i].portType);
			/* gpn_stat may not use this port type for statistic, so move next port type */
			//return GPN_STAT_DBS_GEN_ERR;
		}
	}
	
	/* calculat memrey space : total ports nodes & middle link node */
	i = sizeof(stStatPreScanIndex) +\
		sizeof(stStatLocalPortNode) +\
		sizeof(stStatPeerPortNode) +\
		sizeof(stStatScanPortInfo);
	i *= statScanPortNum;
	gpnLog(GPN_LOG_L_INFO, "%s : portTpNum(%d) protNum(%d) need malloc %d byte space!\n\r",\
		__FUNCTION__, statDevPortNum, statScanPortNum, i);
	/* calculat memrey space end */

#if 0	
	if(level == IFM_DEV_LEVEL_CENTER)
	{
		/* is center device open : 1 no malloc memrey; 2 device status is INSERT */
		if( ((pgstEQUStatProcSpace->unitStatProcSpace[slotId]->pCenterPreScanPortNodeMemSpace) == NULL) &&
			(pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta == GPN_STAT_DEV_STA_INSERT) )
		{
			/**/
			pgstEQUStatProcSpace->unitStatProcSpace[slotId]->pCenterPreScanPortNodeMemSpace =
				(stStatPreScanIndex *)malloc(i);
			if((pgstEQUStatProcSpace->unitStatProcSpace[slotId]->pCenterPreScanPortNodeMemSpace) == NULL)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : slot(%d) level(%d) malloc PortNodeMemSpace err\n\r",
					__FUNCTION__, slotId, level);

				return GPN_STAT_DBS_GEN_ERR;
			}
			memset((UINT8 *)pgstEQUStatProcSpace->unitStatProcSpace[slotId]->pCenterPreScanPortNodeMemSpace, 0, i);

			/*cut memrey to 4 part : 1 middle link node; 2 local 3 peer 4 portInfo */
			pStatPreScanIndex = pgstEQUStatProcSpace->unitStatProcSpace[slotId]->pCenterPreScanPortNodeMemSpace;
			pStatLocalNode = (stStatLocalPortNode *)(pStatPreScanIndex + statScanPortNum);
			pStatPeerNode = (stStatPeerPortNode *)(pStatLocalNode + statScanPortNum);
			pStatScanPort = (stStatScanPortInfo *)(pStatPeerNode + statScanPortNum);

			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : pStatPreScanIndex(%08x) pStatLocalNode(%08x) pStatPeerNode(%08x) pStatScanPort(%08x)\n\r",\
				__FUNCTION__, (UINT32)pStatPreScanIndex, (UINT32)pStatScanPort,\
				(UINT32)pStatPeerNode, (UINT32)pStatScanPort);
		}
		else
		{
			/* ERR */
			gpnLog(GPN_LOG_L_ERR, "%s : slot(%d) ptr(%08x) status(%d) double init OR not release OR not insert, err!\n\r",\
				__FUNCTION__, slotId,\
				pgstEQUStatProcSpace->unitStatProcSpace[slotId]->pCenterPreScanPortNodeMemSpace,\
				pgstEQUStatProcSpace->unitStatProcSpace[slotId]->slotDevSta);

			return GPN_STAT_DBS_GEN_ERR;
		}
		
	}
	else if( (level == IFM_DEV_LEVEL_NERREM) || (level == IFM_DEV_LEVEL_FAREDN) )
	{
		/* is center device open : first remot device should in list, second rem dev status is INSERT, thrid no malloc memrey */
		remEevNum = 0;
		pRemDevStatProcNode = (stRemDevStatProcNode *)listFirst(
			&(pgstEQUStatProcSpace->unitStatProcSpace[slotId]->RemDevStatPorcNodeQuen));
		while( (pRemDevStatProcNode != NULL) &&\
			   (remEevNum < pgstEQUStatProcSpace->unitStatProcSpace[slotId]->RemDevStatPorcNodeNum) )
		{
			if(pRemDevStatProcNode->devIndex == devIndex)
			{
				if( (pRemDevStatProcNode->pRemPreScanPortNodeMemSpace == NULL) &&\
					(pRemDevStatProcNode->remotDevSta == GPN_STAT_DEV_STA_INSERT) )
				{
					pRemDevStatProcNode->pRemPreScanPortNodeMemSpace = (void *)malloc(i);
					if(pRemDevStatProcNode->pRemPreScanPortNodeMemSpace == NULL)
					{
						GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : malloc PortNodeMemSpace err\n\r",
							__FUNCTION__);

						return GPN_STAT_DBS_GEN_ERR;
					}
					memset((UINT8 *)pRemDevStatProcNode->pRemPreScanPortNodeMemSpace, 0, i);

					/*cut memrey to 4 part : 1 middle link node; 2 local 3 peer 4 portInfo */
					pStatPreScanIndex = pgstEQUStatProcSpace->unitStatProcSpace[slotId]->pCenterPreScanPortNodeMemSpace;
					pStatLocalNode = (stStatLocalPortNode *)(pStatPreScanIndex + statScanPortNum);
					pStatPeerNode = (stStatPeerPortNode *)(pStatLocalNode + statScanPortNum);
					pStatScanPort = (stStatScanPortInfo *)(pStatPeerNode + statScanPortNum);

					GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : pStatPreScanIndex(%08x) pStatLocalNode(%08x) pStatPeerNode(%08x) pStatScanPort(%08x)\n\r",\
						__FUNCTION__, (UINT32)pStatPreScanIndex, (UINT32)pStatScanPort,\
						(UINT32)pStatPeerNode, (UINT32)pStatScanPort);

					break;
				}
				else
				{
					/* ERR */
					gpnLog(GPN_LOG_L_ERR, "%s : remot dev(%08x) can not be found!\n\r",\
						__FUNCTION__, devIndex);

					return GPN_STAT_DBS_GEN_ERR;
				}
			}
		}
		if( (pRemDevStatProcNode == NULL) ||\
			(remEevNum >= pgstEQUStatProcSpace->unitStatProcSpace[slotId]->RemDevStatPorcNodeNum) )
		{
			gpnLog(GPN_LOG_L_ERR, "%s : remot dev(%08x) status(%d) double init OR not release OR not insert, err!\n\r",\
				__FUNCTION__, devIndex, pRemDevStatProcNode->remotDevSta);

			return GPN_STAT_DBS_GEN_ERR;
		}
	}
	else
	{
		/* ERR */
		gpnLog(GPN_LOG_L_ERR, "%s : dev(%08x) has wrong level(%d), err!\n\r",\
			__FUNCTION__, devIndex, level);
		return GPN_STAT_DBS_GEN_ERR;
	}
#endif

	/* creat port index : local port index & peer port index */	
	i = 2*statScanPortNum*sizeof(optObjOrient);
	portIndexBuff = (objLogicDesc *)malloc(i);
	if(portIndexBuff == NULL)
	{
		perror("malloc err");
		return GPN_STAT_DBS_GEN_ERR;
	}
	upperPortIndexBuff = (objLogicDesc *)((UINT8 *)portIndexBuff + i/2);
	if(gpnStatMakePortIndex(devIndex,statDevPortNum,pStatPortObjCL,portIndexBuff, upperPortIndexBuff) != GPN_STAT_DBS_GEN_OK)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : dev(%08x) make portIndex err!\n\r",\
			__FUNCTION__, devIndex);
		goto __fixPortBuffFree;
	}

	/*fix port debug */
	/*for(i=0;i<statScanPortNum;i++)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : debug:dev(%08x) portIndex(%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__, portIndexBuff[i].devIndex, portIndexBuff[i].portIndex, portIndexBuff[i].portIndex3,\
			portIndexBuff[i].portIndex4, portIndexBuff[i].portIndex5, portIndexBuff[i].portIndex6);
	}*/
	
	/* init stat per scan node */
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(
		&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL) &&\
		  (sysPortTypeNum < pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum))
	{
		for(i=0;i<statScanPortNum;i++)
		{
			/*first portIndex's portType is main portType */
			//slotId = PortIndex_GetSlot(portIndexBuff[i].portIndex);
			portType = PortIndex_GetType(portIndexBuff[i].portIndex);
			if(portType == pPortTpToScanTp->portType)
			{
				/* stat port node init, use middle link node */
				pStatPreScanIndex->pStatLocalPort = pStatLocalNode;
				gpnStatScanPortNodeInit(&(portIndexBuff[i]),\
					pStatLocalNode, pStatPeerNode, pStatScanPort);
				
				/* mark port add in node-quen, used when delete node */
				pStatScanPort->addQuenMode = GPN_STAT_FIX_MODE;
				
				/* add port to pre scan quen */
				gpnStatNodeRelevChg2PreScanQuen(pStatPreScanIndex, GPN_STAT_DBS_GEN_ENABLE);
				
				/*add port to valid scan quen */
				/*gpnStatNodeRelevChg2ValidScanQuen(pStatLocalNode, GPN_STAT_DBS_GEN_ENABLE);*/

				/* re-local next port's memrey place */
				pStatPreScanIndex++;
				pStatLocalNode++;
				pStatPeerNode++;
				pStatScanPort++;
			}
		}
		
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	
	free(portIndexBuff);
	free(pStatPortObjCL);
	return GPN_STAT_DBS_GEN_OK;

	__fixPortBuffFree:

	free(portIndexBuff);
	free(pStatPortObjCL);
	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatScanPortNodeInit(objLogicDesc *pLocalPort, stStatLocalPortNode *pStatLocalNode, 
	stStatPeerPortNode *pStatPeerNode, stStatScanPortInfo *pStatScanPort)
{
	/*assert*/
	if( (pLocalPort == NULL) ||\
		(pStatLocalNode == NULL) ||\
		(pStatPeerNode == NULL) ||\
		(pStatScanPort == NULL) )
	{
		gpnLog(GPN_LOG_L_ERR, "%s : para err!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	gpnStatLocalPortNodeInit(pLocalPort, pStatLocalNode, pStatPeerNode, pStatScanPort);
	gpnStatPeerPortNodeInit((optObjOrient *)pLocalPort, pStatPeerNode, pStatScanPort);
	gpnStatScanPortInfoInit(pStatScanPort, &(pStatPeerNode->peerPort));
	
	gpnLog(GPN_LOG_L_INFO, "%s : local portIndex(%08x-%08x|%08x|%08x|%08x|%08x) regist!\n\r",\
		__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
		pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);

	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatLocalPortNodeInit(objLogicDesc *pLocalPort, stStatLocalPortNode *pStatLocalNode, 
	stStatPeerPortNode *pStatPeerNode, stStatScanPortInfo *pStatScanPort)
{
	/*assert*/
	if( (pLocalPort == NULL) ||\
		(pStatLocalNode == NULL) ||\
		(pStatPeerNode == NULL) ||\
		(pStatScanPort == NULL) )
	{
		gpnLog(GPN_LOG_L_ERR, "%s : para err!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* localport come from DA : DA start localprot's stat mon */
	memcpy(&(pStatLocalNode->localPort), pLocalPort, sizeof(objLogicDesc));
	/* temp modify for slot info about */
	gpnStatLocalPortIndexInit(&(pStatLocalNode->localPort));
	
	memcpy(&(pStatLocalNode->viewPort), pLocalPort, sizeof(objLogicDesc));
	/* modify view port's slot info */
	gpnStatViewPortIndexInit(&(pStatLocalNode->viewPort));

	pStatLocalNode->pPeerNode = pStatPeerNode;
	pStatLocalNode->pStatScanPort = pStatScanPort;

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatPeerPortNodeInit(optObjOrient *pPeerPort,
	stStatPeerPortNode *pStatPeerNode, stStatScanPortInfo *pStatScanPort)
{
	/*assert*/
	if( (pPeerPort == NULL) ||\
		(pStatPeerNode == NULL) ||\
		(pStatScanPort == NULL) )
	{
		gpnLog(GPN_LOG_L_ERR, "%s : para err!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* peerport init same with localport */
	memcpy(&(pStatPeerNode->peerPort), pPeerPort, sizeof(optObjOrient));
	
	pStatPeerNode->pStatScanPort = pStatScanPort;

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatScanPortInfoInit(stStatScanPortInfo *pStatScanPort, optObjOrient *pPeerPort)
{	
	//UINT32 slotId;
	UINT32 portType;
	stPortTpToStatScanTp *pPortTpToScanTp;
	
	/*assert */
	if( (pStatScanPort == NULL) ||\
		(pPeerPort == NULL) )
	{
		gpnLog(GPN_LOG_L_ERR, "%s : para err!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* perQuen used for fast serach */
	pStatScanPort->scanQuenValid = GPN_STAT_DBS_GEN_DISABLE;
	/*ValidQuen used for per scan opt */
	pStatScanPort->preQuenValid = GPN_STAT_DBS_GEN_DISABLE;
	pStatScanPort->addQuenMode = GPN_STAT_NULL_MODE;
	pStatScanPort->rsd = 0;

	/*==========================*/
	/*	follow 's base scanType, portType maybe
		vs n scanType, in this statuation, here wo-
		uld be modify
	 */
	/*==========================*/
	//slotId = PortIndex_GetSlot(pPeerPort->portIndex);
	portType = PortIndex_GetType(pPeerPort->portIndex);
	pPortTpToScanTp = NULL;
	gpnStatSeekPortType2PTVsST(portType, &pPortTpToScanTp);
	if(pPortTpToScanTp == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : peer port(%08x-%08x|%08x|%08x|%08x|%08x) can't cross scanType!\n\r",\
			__FUNCTION__, pPeerPort->devIndex, pPeerPort->portIndex, pPeerPort->portIndex3,\
			pPeerPort->portIndex4, pPeerPort->portIndex5, pPeerPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}
	pStatScanPort->statScanType = pPortTpToScanTp->statScanType;
	pStatScanPort->pscanTypeNode = pPortTpToScanTp->statScanTypeNode;

	pStatScanPort->dataSize = pPortTpToScanTp->statDataSizeForPort;
	pStatScanPort->dataBitDeep = pPortTpToScanTp->statScanTypeNode->subBitDeep;
	pStatScanPort->valueType = pPortTpToScanTp->statScanTypeNode->subValueType;
	pStatScanPort->numType = pPortTpToScanTp->statScanTypeNode->subNumType;
	
	pStatScanPort->miniScanCyc = pPortTpToScanTp->statScanTypeNode->statScanTpCycle;

	pStatScanPort->isBaseInit = GPN_STAT_DBS_32_FFFF;
	/* these space mollac when stat mon enable */
	pStatScanPort->statBaseDate = NULL;

	/*curr mon default disable */
	pStatScanPort->currMonEn = GPN_STAT_DBS_GEN_DISABLE;
	/* these space mollac when current mon enable */
	pStatScanPort->statCurrDate = NULL;
	
	/* these space mollac when spicel stat mon enable */
	pStatScanPort->statLCycRec = NULL;
	pStatScanPort->statSCycRec = NULL;
	pStatScanPort->statUCycRec = NULL;

	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatLocalPortIndexInit(objLogicDesc *pLocalPort)
{	
	//UINT32 slotId;

	/*assert*/
	if(pLocalPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	gpnLog(GPN_LOG_L_INFO, "%s : old local port(%08x-%08x|%08x|%08x|%08x|%08x)!\n\r",\
		__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
		pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
	
	/*get self slot*/
	//slotId = gpnDevApiGetSelfSoltNum();

#ifdef GPN_CFLAGS_DEV_V8ECPX10
	if(pLocalPort->devIndex != GPN_ILLEGAL_DEVICE_INDEX)
	{
		//IFM_DEV_CHANG_SLOT(pLocalPort->devIndex, slotId);
	}

	if(pLocalPort->portIndex != GPN_ILLEGAL_PORT_INDEX)
	{
		IFM_PORT_CHANG_SLOT(pLocalPort->portIndex, 0);
	}
	if(pLocalPort->portIndex3 != GPN_ILLEGAL_PORT_INDEX)
	{
		IFM_PORT_CHANG_SLOT(pLocalPort->portIndex3, 0);
	}
	if(pLocalPort->portIndex4 != GPN_ILLEGAL_PORT_INDEX)
	{
		IFM_PORT_CHANG_SLOT(pLocalPort->portIndex4, 0);
	}
	if(pLocalPort->portIndex5 != GPN_ILLEGAL_PORT_INDEX)
	{
		IFM_PORT_CHANG_SLOT(pLocalPort->portIndex5, 0);
	}
	if(pLocalPort->portIndex6 != GPN_ILLEGAL_PORT_INDEX)
	{
		IFM_PORT_CHANG_SLOT(pLocalPort->portIndex6, 0);
	}
#endif
	gpnLog(GPN_LOG_L_INFO, "%s : new local port(%08x-%08x|%08x|%08x|%08x|%08x)!\n\r",\
		__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
		pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
	
	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatViewPortIndexInit(objLogicDesc *pViewPort)
{	
	//UINT32 slotId;

	/*assert*/
	if(pViewPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	gpnLog(GPN_LOG_L_INFO, "%s : old view port(%08x-%08x|%08x|%08x|%08x|%08x)!\n\r",\
		__FUNCTION__, pViewPort->devIndex, pViewPort->portIndex, pViewPort->portIndex3,\
		pViewPort->portIndex4, pViewPort->portIndex5, pViewPort->portIndex6);
	
	/*get self slot*/
	//slotId = gpnDevApiGetSelfSoltNum();

#if GPN_CFLAGS_DEV_V8ECPX10
	if(pViewPort->devIndex != GPN_ILLEGAL_DEVICE_INDEX)
	{
		//IFM_DEV_CHANG_SLOT(pViewPort->devIndex, slotId);
	}

	if(pViewPort->portIndex != GPN_ILLEGAL_PORT_INDEX)
	{
		IFM_PORT_CHANG_SLOT(pViewPort->portIndex, 0);
	}
	if(pViewPort->portIndex3 != GPN_ILLEGAL_PORT_INDEX)
	{
		IFM_PORT_CHANG_SLOT(pViewPort->portIndex3, 0);
	}
	if(pViewPort->portIndex4 != GPN_ILLEGAL_PORT_INDEX)
	{
		IFM_PORT_CHANG_SLOT(pViewPort->portIndex4, 0);
	}
	if(pViewPort->portIndex5 != GPN_ILLEGAL_PORT_INDEX)
	{
		IFM_PORT_CHANG_SLOT(pViewPort->portIndex5, 0);
	}
	if(pViewPort->portIndex6 != GPN_ILLEGAL_PORT_INDEX)
	{
		IFM_PORT_CHANG_SLOT(pViewPort->portIndex6, 0);
	}
#endif
	gpnLog(GPN_LOG_L_INFO, "%s : new view port(%08x-%08x|%08x|%08x|%08x|%08x)!\n\r",\
		__FUNCTION__, pViewPort->devIndex, pViewPort->portIndex, pViewPort->portIndex3,\
		pViewPort->portIndex4, pViewPort->portIndex5, pViewPort->portIndex6);
	
	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatScanPortNodeDestroy(stStatPreScanIndex *pStatPreScanIndex)
{
	stStatScanPortInfo *pStatScanPort;
	
	/*assert ??? */
	if(pStatPreScanIndex == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para err!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	if(pStatPreScanIndex->pStatLocalPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanPort = pStatPreScanIndex->pStatLocalPort->pStatScanPort;
	if(pStatScanPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/* these space mollac when spicel stat mon enable */
	if(pStatScanPort->statLCycRec != NULL)
	{
		free(pStatScanPort->statLCycRec);
		pStatScanPort->statLCycRec = NULL;
	}
	if(pStatScanPort->statSCycRec != NULL)
	{
		free(pStatScanPort->statSCycRec);
		pStatScanPort->statSCycRec = NULL;
	}
	if(pStatScanPort->statUCycRec != NULL)
	{
		free(pStatScanPort->statUCycRec);
		pStatScanPort->statUCycRec = NULL;
	}

	/*this space mollac when portNode init */
	if(pStatScanPort->statBaseDate != NULL)
	{
		free(pStatScanPort->statBaseDate);
		pStatScanPort->statBaseDate = NULL;
		pStatScanPort->statCurrDate = NULL;
	}

	/*statBaseDate and statCurrDate in one big memery space */
	/*if(pStatScanPort->statCurrDate != NULL)
	{
		free(pStatScanPort->statCurrDate);
		pStatScanPort->statCurrDate = NULL;
	}*/

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatNodeRelevChg2PreScanQuen(stStatPreScanIndex *pStatPreScanIndex, UINT32 opt)
{
	UINT32 reVal;
	UINT32 localHash;
	UINT32 peerHash;
	UINT32 slotId;
	UINT32 portType;
	objLogicDesc *pLocalPort;
	optObjOrient *pPeerPort;
	stStatLocalPortNode *pStatLocalNode;
	stStatPeerPortNode *pStatPeerNode;
	stStatScanPortInfo *pStatScanPort;
	stPortTpToStatScanTp *pPortTpToScanTp;
	stStatPreScanQuen *pLocalPreScanNode;
	stStatPreScanQuen *pPeerPreScanNode;
	
	/*assert */
	if( (pStatPreScanIndex == NULL) ||\
		((opt != GPN_STAT_DBS_GEN_ENABLE) &&\
		 (opt != GPN_STAT_DBS_GEN_DISABLE)) )
 	{
		gpnLog(GPN_LOG_L_ERR, "%s : para err!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatLocalNode = pStatPreScanIndex->pStatLocalPort;
	pStatPeerNode = pStatLocalNode->pPeerNode;
	pStatScanPort = pStatLocalNode->pStatScanPort;

	/*
		step 1 : find local port's pStatPreScanNode
	*/
	pLocalPort = &(pStatLocalNode->localPort);
	slotId = PortIndex_GetSlot(pLocalPort->portIndex);
	portType = PortIndex_GetType(pLocalPort->portIndex);
	pPortTpToScanTp = NULL;
	gpnStatSeekPortType2PTVsST(portType, &pPortTpToScanTp);
	if(pPortTpToScanTp == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) con't cross scanType!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}
	pLocalPreScanNode = pPortTpToScanTp->pStatPreScanNode[slotId];
	
	/*
		step 2 : find peer port's pStatPreScanNode
	*/
	pPeerPort = &(pStatPeerNode->peerPort);
	slotId = PortIndex_GetSlot(pPeerPort->portIndex);
	portType = PortIndex_GetType(pPeerPort->portIndex);
	pPortTpToScanTp = NULL;
	gpnStatSeekPortType2PTVsST(portType, &pPortTpToScanTp);
	if(pPortTpToScanTp == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : peer port(%08x-%08x|%08x|%08x|%08x|%08x) con't cross scanType!\n\r",\
			__FUNCTION__, pPeerPort->devIndex, pPeerPort->portIndex,\
			pPeerPort->portIndex3, pPeerPort->portIndex4,\
			pPeerPort->portIndex5, pPeerPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}
	pPeerPreScanNode = pPortTpToScanTp->pStatPreScanNode[slotId];

	/*
		step 3 : if local port have same port type whit peer port ???
	*/
	/*
		if() ...
	*/

	if( (opt == GPN_STAT_DBS_GEN_ENABLE) &&
		(pStatScanPort->preQuenValid == GPN_STAT_DBS_GEN_DISABLE) )
	{
		/*
			step 4 : local port old disable pre-scan, now enable pre-scan 
		*/
		localHash = ((pLocalPort->portIndex) % (pLocalPreScanNode->hashKey));
		reVal = gpnStatRelevCheckIsLocalInPerScanQuen(pLocalPort, pLocalPreScanNode);
		if(reVal != GPN_STAT_DBS_GEN_OK)
		{		
			listAdd(&(pLocalPreScanNode->statPreScanQuen[localHash]), (NODE*)pStatPreScanIndex);
			pLocalPreScanNode->preScanNodeNum[localHash]++;

			gpnLog(GPN_LOG_L_INFO, "%s : local prot(%08x-%08x|%08x|%08x|%08x|%08x) add in perQuen!\n\r",\
				__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex,\
				pLocalPort->portIndex3, pLocalPort->portIndex4,\
				pLocalPort->portIndex5, pLocalPort->portIndex6);
		}
		else
		{
			gpnLog(GPN_LOG_L_ERR, "%s : local prot(%08x-%08x|%08x|%08x|%08x|%08x) already in perQuen, err!\n\r",\
				__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex,\
				pLocalPort->portIndex3, pLocalPort->portIndex4,\
				pLocalPort->portIndex5, pLocalPort->portIndex6);
		}

		/*
			step 5 : peer port old disable pre-scan, now enable pre-scan 
		*/
		peerHash = ((pPeerPort->portIndex) % (pPeerPreScanNode->hashKey));
		reVal = gpnStatRelevCheckIsPeerInPeerPortQuen(pPeerPort, pPeerPreScanNode);
		if(reVal != GPN_STAT_DBS_GEN_OK)
		{
			listAdd(&(pPeerPreScanNode->statPeerPortQuen[peerHash]), (NODE*)pStatPeerNode);
			pPeerPreScanNode->peerPortNodeNum[peerHash]++;

			gpnLog(GPN_LOG_L_INFO, "%s : peer prot(%08x-%08x|%08x|%08x|%08x|%08x) add in perQuen!\n\r",\
				__FUNCTION__, pPeerPort->devIndex, pPeerPort->portIndex,\
				pPeerPort->portIndex3, pPeerPort->portIndex4,\
				pPeerPort->portIndex5, pPeerPort->portIndex6);
		}
		else
		{
			gpnLog(GPN_LOG_L_ERR, "%s : peer prot(%08x-%08x|%08x|%08x|%08x|%08x) already in perQuen, err!\n\r",\
				__FUNCTION__, pPeerPort->devIndex, pPeerPort->portIndex,\
				pPeerPort->portIndex3, pPeerPort->portIndex4,\
				pPeerPort->portIndex5, pPeerPort->portIndex6);
		}

		/*
			step 6 : change perQuenValid stat  
		*/
		pStatScanPort->preQuenValid = GPN_STAT_DBS_GEN_ENABLE;
	}
	else if((opt == GPN_STAT_DBS_GEN_DISABLE) &&
			(pStatScanPort->preQuenValid == GPN_STAT_DBS_GEN_ENABLE))
	{
		/*
			step 4 : local port old enable pre-scan, now disable pre-scan;
			see node add-mode, if mode is GPN_STAT_FIX_MODE, 
			should not delete from perQuen
		*/
		if(pStatScanPort->addQuenMode == GPN_STAT_FIX_MODE)
		{
			gpnLog(GPN_LOG_L_ERR, "%s : local prot(%08x-%08x|%08x|%08x|%08x|%08x) add perQuen mode is FIX, can't delete out of perQuen!\n\r",\
				__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex,\
				pLocalPort->portIndex3, pLocalPort->portIndex4,\
				pLocalPort->portIndex5, pLocalPort->portIndex6);

			return GPN_STAT_DBS_GEN_ERR;
		}

		/*
			step 5  : local port node should be delete from ValidQuen
		*/
		if(pStatScanPort->scanQuenValid == GPN_STAT_DBS_GEN_ENABLE)
		{
			reVal = gpnStatNodeRelevChg2ValidScanQuen(pStatLocalNode, GPN_STAT_DBS_GEN_DISABLE);
			if (reVal == GPN_STAT_DBS_GEN_ERR)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : gpnStatNodeRelevChg2ValidScanQuen return err!\n\r",\
					__FUNCTION__);

				return GPN_STAT_DBS_GEN_ERR;
			}
		}
		
		/*
			step 6  : local port node delete out of perQuen
		*/
		localHash = ((pLocalPort->portIndex) % (pLocalPreScanNode->hashKey));
		
		listDelete(&(pLocalPreScanNode->statPreScanQuen[localHash]), (NODE*)pStatPreScanIndex);
		if(pLocalPreScanNode->preScanNodeNum[localHash] > 0)
		{
			pLocalPreScanNode->preScanNodeNum[localHash]--;
		}
		gpnLog(GPN_LOG_L_INFO, "%s : local prot(%08x-%08x|%08x|%08x|%08x|%08x) delete out of perQuen!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6);

		/*
			step 7 : local port node delete out of perQuen
		*/
		peerHash = ((pPeerPort->portIndex) % (pPeerPreScanNode->hashKey));
		
		listDelete(&(pPeerPreScanNode->statPeerPortQuen[peerHash]), (NODE*)pStatPeerNode);
		if(pPeerPreScanNode->peerPortNodeNum[peerHash] > 0)
		{
			pPeerPreScanNode->peerPortNodeNum[peerHash]--;
		}
		gpnLog(GPN_LOG_L_INFO, "%s : peer prot(%08x-%08x|%08x|%08x|%08x|%08x) delete out of perQuen!\n\r",\
			__FUNCTION__, pPeerPort->devIndex, pPeerPort->portIndex,\
			pPeerPort->portIndex3, pPeerPort->portIndex4,\
			pPeerPort->portIndex5, pPeerPort->portIndex6);

		/*
			step 8 : change perQuenValid stat  
		*/
		pStatScanPort->preQuenValid = GPN_STAT_DBS_GEN_DISABLE;
	}
	else
	{
		/*
			status same with opt, do nothing, but should not come here
		*/
		gpnLog(GPN_LOG_L_ERR, "%s : status(%d) same with opt(%d)!\n\r",\
			__FUNCTION__, pStatScanPort->preQuenValid, opt);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatNodeRelevChg2ValidScanQuen(stStatLocalPortNode *pStatLocalNode, UINT32 opt)
{
	UINT32 slotId;
	UINT32 portType;
	objLogicDesc *pLocalPort;
	stPortTpToStatScanTp *pPortTpToScanTp;
	stStatPreScanQuen *pLocalPreScanNode;
	stStatScanPortInfo *pStatScanPort;
	
	/*assert */
	if( (pStatLocalNode == NULL) ||\
		((opt != GPN_STAT_DBS_GEN_ENABLE) &&\
		 (opt != GPN_STAT_DBS_GEN_DISABLE)) )
 	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para err!\n\r",\
			__FUNCTION__);
		printf("%s[%d] : para err!\n\r", __FUNCTION__, __LINE__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*
		step 1 : find local port's pStatPreScanNode
	*/
	pLocalPort = &(pStatLocalNode->localPort);
	slotId = PortIndex_GetSlot(pLocalPort->portIndex);
	portType = PortIndex_GetType(pLocalPort->portIndex);
	pPortTpToScanTp = NULL;
	gpnStatSeekPortType2PTVsST(portType, &pPortTpToScanTp);
	if(pPortTpToScanTp == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) con't cross scanType!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6);
		printf("%s[%d] : local port(%08x-%08x|%08x|%08x|%08x|%08x) con't cross scanType!\n\r",\
			__FUNCTION__, __LINE__, pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}
	pLocalPreScanNode = pPortTpToScanTp->pStatPreScanNode[slotId];

	pStatScanPort = pStatLocalNode->pStatScanPort;
	if( (opt == GPN_STAT_DBS_GEN_ENABLE) &&
		(pStatScanPort->scanQuenValid == GPN_STAT_DBS_GEN_DISABLE) )
	{
		/*
			step 2 : old disable vaild-scan, now enable vaild-scan
		*/
		listAdd(&(pLocalPreScanNode->statScanTypeNode->statScanPortObjQuen), (NODE*)pStatLocalNode);
		pLocalPreScanNode->statScanTypeNode->statScanQuenPortNum++;

		/*
			step 3 : change scanQuenValid stat
		*/
		pStatScanPort->scanQuenValid = GPN_STAT_DBS_GEN_ENABLE;

		gpnLog(GPN_LOG_L_INFO, "%s : local prot(%08x-%08x|%08x|%08x|%08x|%08x) add in viladScanQuen!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6);
		zlog_debug(STATIS_DBG_REGISTER, "%s[%d] : local prot(%08x-%08x|%08x|%08x|%08x|%08x) add in viladScanQuen!\n\r",\
			__FUNCTION__, __LINE__, pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6);
	}
	else if((opt == GPN_STAT_DBS_GEN_DISABLE) &&
			(pStatScanPort->scanQuenValid == GPN_STAT_DBS_GEN_ENABLE))
	{
		/*
			step 2 : old enable vaild-scan, now disable vaild-scan
		*/
		listDelete(&(pLocalPreScanNode->statScanTypeNode->statScanPortObjQuen), (NODE*)pStatLocalNode);
		if(pLocalPreScanNode->statScanTypeNode->statScanQuenPortNum > 0)
		{
			pLocalPreScanNode->statScanTypeNode->statScanQuenPortNum--;
		}

		/*
			step 3 : change scanQuenValid stat
		*/
		pStatScanPort->scanQuenValid = GPN_STAT_DBS_GEN_DISABLE;
		pStatScanPort->insAddFlag = 0;

		gpnLog(GPN_LOG_L_INFO, "%s : local prot(%08x-%08x|%08x|%08x|%08x|%08x) delete in viladScanQuen!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6);
		zlog_debug(STATIS_DBG_REGISTER, "%s[%d] : local prot(%08x-%08x|%08x|%08x|%08x|%08x) delete in viladScanQuen!\n\r",\
			__FUNCTION__, __LINE__, pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6);
	}
	else
	{
		/*
			status same with opt, do nothing, but should not come here 
		*/
		gpnLog(GPN_LOG_L_ERR, "%s : status(%d) same with opt(%d)!\n\r",\
			__FUNCTION__, pStatScanPort->scanQuenValid, opt);
		zlog_debug(STATIS_DBG_REGISTER, "%s[%d] : status(%d) same with opt(%d)!\n\r",\
			__FUNCTION__, __LINE__, pStatScanPort->scanQuenValid, opt);
		return GPN_STAT_DBS_GEN_ERR;
	}

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatCycClassAlmTpSet(stStatXCycDataRecd *statXCycRec, 
				UINT32 almThredTpId, UINT32 evnThredTpId)
{
	stStatAlmThredTpNode *palmThredTpNode;
	stStatEvnThredTpNode *pevnThredTpNode;
	
	/*assert */
	if(statXCycRec == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	palmThredTpNode = NULL;
	gpnStatAlarmThredTpGet(almThredTpId, &palmThredTpNode);
	if(palmThredTpNode != NULL)
	{
		statXCycRec->xCycAlmThId = almThredTpId;
		statXCycRec->xCycAlmThred = (void *)palmThredTpNode->palmThredTp->palmThred;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : almThredTpId(%d) err!\n\r",\
			__FUNCTION__, almThredTpId);
		statXCycRec->xCycAlmThId = GPN_STAT_INVALID_TEMPLAT_ID;
		statXCycRec->xCycAlmThred = NULL;
	}
	
	pevnThredTpNode = NULL;
	gpnStatEventThredTpGet(evnThredTpId, &pevnThredTpNode);
	if(pevnThredTpNode != NULL)
	{
		statXCycRec->xCycEvnThId = evnThredTpId;
		statXCycRec->xCycEvnThred = (void *)pevnThredTpNode->pevnThredTp->pevnThred;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : evnThredTpId(%d) err!\n\r",\
			__FUNCTION__, evnThredTpId);
		statXCycRec->xCycEvnThId = GPN_STAT_INVALID_TEMPLAT_ID;
		statXCycRec->xCycEvnThred = NULL;
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatCycClassChgAlmTp(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc,
	     UINT32 cycClass, UINT32 almThredTpId, UINT32 evnThredTpId)
{
	stStatScanPortInfo *pStatScanPort;
	
	/* assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanPort = pStatLocalNode->pStatScanPort;
	if(cycClass == GPN_STAT_CYC_CLASS_LONG)
	{
		if(pStatScanPort->statLCycRec != NULL)
		{
			gpnStatCycClassAlmTpSet(pStatScanPort->statLCycRec, almThredTpId, evnThredTpId);
		}
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_SHORT)
	{
		if(pStatScanPort->statSCycRec != NULL)
		{
			gpnStatCycClassAlmTpSet(pStatScanPort->statSCycRec, almThredTpId, evnThredTpId);
		}
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_USR)
	{
		if(pStatScanPort->statUCycRec != NULL)
		{
			gpnStatCycClassAlmTpSet(pStatScanPort->statUCycRec,almThredTpId, evnThredTpId);
		}
	}
	else
	{
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) cycClass(%d) err!\n\r",\
			__FUNCTION__,\
			pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
			pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
			pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
			cycClass);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*renew cyc poll stat data cycSecs */
	if(cycClass != GPN_STAT_CYC_CLASS_BASE)
	{
		gpnStatRenewPortPollCyc(pStatLocalNode, scanTpPollCyc);
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatCycClassDataTpSet(stStatXCycDataRecd *statXCycRec,
					UINT32 subReptTpId, UINT32 subFiltTpId)
{
	stStatSubReportTpNode *psubReportTpNode;
	stStatSubFiltTpNode *psubFiltTpNode;
	
	/*assert */
	if(statXCycRec == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	psubReportTpNode = NULL;
	gpnStatSubReportTpGet(subReptTpId, &psubReportTpNode);
	if(psubReportTpNode != NULL)
	{
		statXCycRec->xCycsubReptId= subReptTpId;
		statXCycRec->subReptInfo= (void *)psubReportTpNode->psubReportTp->psubReport;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : subReptTpId(%d) err!\n\r",\
			__FUNCTION__, subReptTpId);
		statXCycRec->xCycsubReptId = GPN_STAT_INVALID_TEMPLAT_ID;
		statXCycRec->subReptInfo = NULL;
	}
	
	psubFiltTpNode = NULL;
	gpnStatSubFiltTpGet(subFiltTpId, &psubFiltTpNode);
	if(psubFiltTpNode != NULL)
	{
		statXCycRec->xCycsubFiltId = subFiltTpId;
		statXCycRec->subFiltInfo = (void *)psubFiltTpNode->psubFiltTp->psubFilt;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : subFiltTpId(%d) err!\n\r",\
			__FUNCTION__, subFiltTpId);
		statXCycRec->xCycsubFiltId = GPN_STAT_INVALID_TEMPLAT_ID;
		statXCycRec->subFiltInfo = NULL;
	}
    
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatCycClassChgDataTp(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc,
	UINT32 cycClass, UINT32 subReptTpId, UINT32 subFiltTpId)
{
	stStatScanPortInfo *pStatScanPort;
	
	/* assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanPort = pStatLocalNode->pStatScanPort;
	if(cycClass == GPN_STAT_CYC_CLASS_LONG)
	{
		if(pStatScanPort->statLCycRec != NULL)
		{
			gpnStatCycClassDataTpSet(pStatScanPort->statLCycRec, subReptTpId, subFiltTpId);
		}
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_SHORT)
	{
		if(pStatScanPort->statSCycRec != NULL)
		{
			gpnStatCycClassDataTpSet(pStatScanPort->statSCycRec, subReptTpId, subFiltTpId);
		}
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_USR)
	{
		if(pStatScanPort->statUCycRec != NULL)
		{
			gpnStatCycClassDataTpSet(pStatScanPort->statUCycRec,subReptTpId, subFiltTpId);
		}
	}
	else
	{
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) cycClass(%d) err!\n\r",\
			__FUNCTION__,\
			pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
			pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
			pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
			cycClass);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*renew cyc poll stat data cycSecs */
	if(cycClass != GPN_STAT_CYC_CLASS_BASE)
	{
		gpnStatRenewPortPollCyc(pStatLocalNode, scanTpPollCyc);
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatCycClassHistAutoReport(stStatLocalPortNode *pStatLocalNode, 
	UINT32 scanTpPollCyc, UINT32 cycClass, INT32 histReport)
{
	stStatScanPortInfo *pStatScanPort;
	
	/* assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanPort = pStatLocalNode->pStatScanPort;
	if(cycClass == GPN_STAT_CYC_CLASS_LONG)
	{
		if(pStatScanPort->statLCycRec != NULL)
		{
			pStatScanPort->statLCycRec->trapEn = histReport;
		}
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_SHORT)
	{
		if(pStatScanPort->statSCycRec != NULL)
		{
			pStatScanPort->statSCycRec->trapEn = histReport;
		}
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_USR)
	{
		if(pStatScanPort->statUCycRec != NULL)
		{
			pStatScanPort->statUCycRec->trapEn = histReport;
		}
	}
	else
	{
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) cycClass(%d) err!\n\r",\
			__FUNCTION__,\
			pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
			pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
			pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
			cycClass);
		return GPN_STAT_DBS_GEN_ERR;
	}

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatMonOpt(objLogicDesc *pLocalPort, UINT32 scanType,
	UINT32 cycClass, UINT32 cycSec, UINT32 taskId, UINT32 almThredTpId, UINT32 subReptTpId, 
	UINT32 evnThredTpId, UINT32 subFiltTpId, INT32 histReport, UINT32 opt, UINT32 currOpt)
{
	/* DA get perPortInfo from GPN_IFM */
	UINT32 reVal;
	//UINT32 slotId;
	UINT32 portType;
	UINT32 taskTmpId;
	UINT32 subReportLTmpId;
	UINT32 subReportSTmpId;
	UINT32 subReportUTmpId;
	UINT32 subFiltLTmpId;
	UINT32 subFiltSTmpId;
	UINT32 subFiltUTmpId;
	UINT32 almThredLTpId;
	UINT32 almThredSTpId;
	UINT32 almThredUTpId;
	//UINT32 evnThredLTpId;
	//UINT32 evnThredSTpId;
	//UINT32 evnThredUTpId;
	UINT32 scanTpPollCyc;
	stPortTpToStatScanTp *pPortTpToScanTp;
	stStatPreScanIndex *pStatPreScanIndex;
	stStatLocalPortNode *pStatLocalPort;
	stStatScanPortInfo *pStatScanPort;
	optObjOrient *peerPortIndex;
	stStatTaskNode *ptaskNode;

	/* assert */
	if(pLocalPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if( (cycClass != GPN_STAT_CYC_CLASS_BASE) &&\
		(cycClass != GPN_STAT_CYC_CLASS_LONG) &&\
		(cycClass != GPN_STAT_CYC_CLASS_SHORT) &&\
		(cycClass != GPN_STAT_CYC_CLASS_USR) )
	{
		zlog_err("%s[%d] : cycClass(%d) err!\n\r", __FUNCTION__, __LINE__, cycClass);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if( (opt != GPN_STAT_MON_OPT_ENABLE) &&\
		(opt != GPN_STAT_MON_OPT_DISABLE) &&\
		(opt != GPN_STAT_MON_OPT_DELETE) )
	{
		zlog_err("%s[%d] : opt(%d) err!\n\r", __FUNCTION__, __LINE__, opt);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*for bug 35286*/
//	if ((currOpt != GPN_STAT_MON_OPT_ENABLE) &&
//		(currOpt != GPN_STAT_MON_OPT_DISABLE))
//	{
//		return GPN_STAT_DBS_GEN_ERR;
//	}
	
	/*geqian 2015.10.8 histReport assert */
	if(cycClass != GPN_STAT_CYC_CLASS_BASE) 
	{		
		if( (histReport != GPN_STAT_DBS_GEN_ENABLE) &&
			(histReport != GPN_STAT_DBS_GEN_DISABLE) )
		{
			zlog_err("%s[%d] : cyyClass, histReport(%d|%d) err!\n\r", __FUNCTION__, __LINE__, cycClass, histReport);
			return GPN_STAT_DBS_GEN_ERR;
		}
	}

	/*
		first : search port in perQuen (when enable 
		xCyc mon, port should be in perScan quen,
		and also port should be in valiScan quen,
		that means port should be add in table  
	*/
	taskTmpId = 0;
	subReportLTmpId = 0;
	subReportSTmpId = 0;
	subReportUTmpId = 0;
	subFiltLTmpId = 0;
	subFiltSTmpId = 0;
	subFiltUTmpId = 0;
	almThredLTpId = 0;
	almThredSTpId = 0;
	almThredUTpId = 0;
	//evnThredLTpId = 0;
	//evnThredSTpId = 0;
	//evnThredUTpId = 0;
	
	pStatPreScanIndex = NULL;
	gpnStatSeekLocalPort2PreScanNodeIndex(pLocalPort, &pStatPreScanIndex);
	
	if(pStatPreScanIndex == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) not in preScan quen!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);

		zlog_err("%s[%d] : local port(%08x-%08x|%08x|%08x|%08x|%08x) not in preScan quen!\n\r",\
			__FUNCTION__, __LINE__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanPort = pStatPreScanIndex->pStatLocalPort->pStatScanPort;
	if(pStatScanPort->scanQuenValid != GPN_STAT_DBS_GEN_ENABLE)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) not in valiScan quen!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);

		zlog_err("%s[%d] : local port(%08x-%08x|%08x|%08x|%08x|%08x) not in valiScan quen!\n\r",\
			__FUNCTION__, __LINE__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*
		second : find para scanTpPollCyc
	*/
	//slotId = PortIndex_GetSlot(pLocalPort->portIndex);
	portType = PortIndex_GetType(pLocalPort->portIndex);

	pPortTpToScanTp = NULL;
	gpnStatSeekPortType2PTVsST(portType, &pPortTpToScanTp);
	if(pPortTpToScanTp == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : dev(%08x) port(%08x|%08x|%08x|%08x|%08x) portType vs statType err!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);

		zlog_err("%s[%d] : dev(%08x) port(%08x|%08x|%08x|%08x|%08x) portType vs statType err!\n\r",\
			__FUNCTION__, __LINE__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}
	scanTpPollCyc = pPortTpToScanTp->statScanTypeNode->statScanTpCycle;

	/*
		thrid : do enbale or disable
	*/
	pStatLocalPort = pStatPreScanIndex->pStatLocalPort;
	if(opt == GPN_STAT_DBS_GEN_ENABLE)
	{
		/*enable port stat mon thrid : malloc memrey space */
		reVal = gpnStatCycClassMemoryMalloc(pStatLocalPort, cycClass);
		if(reVal != GPN_STAT_DBS_GEN_OK)
		{
			gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) memory malloc, return(%d)\n\r",\
				__FUNCTION__, cycClass, reVal);
		}

		/*when repalce taskTemplate 	out portMon out the old task, make sure the port mon in only one cys class*/
		if(cycClass == GPN_STAT_CYC_CLASS_LONG)
		{
			if(pStatScanPort->statLCycRec)
			{
				subReportLTmpId = pStatScanPort->statLCycRec->xCycsubReptId;
				subFiltLTmpId = pStatScanPort->statLCycRec->xCycsubFiltId;
				almThredLTpId = pStatScanPort->statLCycRec->xCycAlmThId;
				//evnThredLTpId = pStatScanPort->statLCycRec->xCycEvnThId;
					
				if(pStatScanPort->statLCycRec->ptaskNode)
				{
					taskTmpId = pStatScanPort->statLCycRec->ptaskNode->taskId;
					if(taskId != taskTmpId)
					{
						/*check whether need clear alarm*/
						peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
						gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
							pStatScanPort->statLCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_LONG);
						//printf("clear long class alarm when change task template\n\r");
					
						gpnStatCycClassDelete(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_LONG);
						//printf("%s : delete cycClass(%d)\n\r", __FUNCTION__, GPN_STAT_CYC_CLASS_LONG);

						gpnStatPortMonOutTask(pStatLocalPort, taskTmpId);
						//printf("long cyc port mon out old task:%d\n\r", taskTmpId);

						ptaskNode = NULL;
						gpnStatTaskGet(taskId, &ptaskNode);
						if(ptaskNode == NULL)
						{
							zlog_err("%s[%d] : gpnStatTaskGet err!\n\r", __func__, __LINE__);
							return GPN_STAT_DBS_GEN_ERR;
						}

						gpnStatCycClassChgAlmTp(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_LONG, \
								ptaskNode->almThredTpId, evnThredTpId);
						//printf("%s:%d  cycClass(%d) almThredTpId:%d evnThredTpId:%d\n\r",
							//__FUNCTION__, __LINE__, ptaskNode->cycClass, ptaskNode->almThredTpId, evnThredTpId);
					}
					else
					{
						if (almThredTpId != almThredLTpId)
						{
							/*check whether need clear alarm*/
							peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
							gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
								pStatScanPort->statLCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_LONG);
							//printf("clear long class alarm when change task template\n\r");
						
							gpnStatCycClassChgAlmTp(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_LONG, \
								pStatScanPort->statLCycRec->ptaskNode->almThredTpId, evnThredTpId);

							//printf("%s:%d  longClass almThredTpId:%d:%d:%d evnThredTpId:%d\n\r",
							//	__FUNCTION__, __LINE__, almThredLTpId, almThredTpId, pStatScanPort->statLCycRec->ptaskNode->almThredTpId, evnThredTpId);
						}
					}
				}	
			}
		}
		if(cycClass == GPN_STAT_CYC_CLASS_SHORT)
		{
			if(pStatScanPort->statSCycRec)
			{
				subReportSTmpId = pStatScanPort->statSCycRec->xCycsubReptId;
				subFiltSTmpId = pStatScanPort->statSCycRec->xCycsubFiltId;
				almThredSTpId = pStatScanPort->statSCycRec->xCycAlmThId;
				//evnThredSTpId = pStatScanPort->statSCycRec->xCycEvnThId;
				
				if(pStatScanPort->statSCycRec->ptaskNode)
				{	
					taskTmpId = pStatScanPort->statSCycRec->ptaskNode->taskId;
					if(taskId != taskTmpId)
					{
						/*check whether need clear alarm*/
						peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
						gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
							pStatScanPort->statSCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_SHORT);
						//printf("clear short class alarm when change task template\n\r");
					
						gpnStatCycClassDelete(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_SHORT);
						//printf("%s : delete cycClass(%d)\n\r", __FUNCTION__, GPN_STAT_CYC_CLASS_SHORT);
						
						gpnStatPortMonOutTask(pStatLocalPort, taskTmpId);
						//printf("short cyc port mon out old task:%d\n\r", taskTmpId);

						ptaskNode = NULL;
						gpnStatTaskGet(taskId, &ptaskNode);
						if(ptaskNode == NULL)
						{
							zlog_err("%s[%d] : gpnStatTaskGet err!\n\r", __func__, __LINE__);
							return GPN_STAT_DBS_GEN_ERR;
						}

						gpnStatCycClassChgAlmTp(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_SHORT, \
								ptaskNode->almThredTpId, evnThredTpId);
						
						//printf("%s:%d cycClass(%d) almThredTpId:%d evnThredTpId:%d\n\r",
							//__FUNCTION__, __LINE__, ptaskNode->cycClass, ptaskNode->almThredTpId, evnThredTpId);
					}
					else
					{
						if (almThredTpId != almThredSTpId)
						{
							/*check whether need clear alarm*/
							peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
							gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
								pStatScanPort->statSCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_SHORT);
							//printf("clear short class alarm when change task template\n\r");
						
							gpnStatCycClassChgAlmTp(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_SHORT, \
								pStatScanPort->statSCycRec->ptaskNode->almThredTpId, evnThredTpId);
						}
					}
				}	
			}
		}	
		if(cycClass == GPN_STAT_CYC_CLASS_USR)
		{
			if(pStatScanPort->statUCycRec)
			{
				subReportUTmpId = pStatScanPort->statUCycRec->xCycsubReptId;
				subFiltUTmpId = pStatScanPort->statUCycRec->xCycsubFiltId;
				almThredUTpId = pStatScanPort->statUCycRec->xCycAlmThId;
				//evnThredUTpId = pStatScanPort->statUCycRec->xCycEvnThId;
				
				if(pStatScanPort->statUCycRec->ptaskNode)
				{
					/*check whether need clear alarm*/
					taskTmpId = pStatScanPort->statUCycRec->ptaskNode->taskId;
					if(taskId != taskTmpId)
					{
						peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
						gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
							pStatScanPort->statUCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_USR);
						//printf("clear user define class alarm when change task template\n\r");
					
						gpnStatCycClassDelete(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_USR);
						//printf("%s : delete cycClass(%d)\n\r", __FUNCTION__, GPN_STAT_CYC_CLASS_USR);
						
						gpnStatPortMonOutTask(pStatLocalPort, taskTmpId);
						//printf("user syc port mon out old task:%d\n\r", taskTmpId);

						ptaskNode = NULL;
						gpnStatTaskGet(taskId, &ptaskNode);
						if(ptaskNode == NULL)
						{
							zlog_err("%s[%d] : gpnStatTaskGet err!\n\r", __func__, __LINE__);
							return GPN_STAT_DBS_GEN_ERR;
						}
						
						gpnStatCycClassChgAlmTp(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_USR, \
								ptaskNode->almThredTpId, evnThredTpId);						
					}
					else
					{
						if (almThredTpId != almThredUTpId)
						{
							peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
							gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
								pStatScanPort->statUCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_USR);
							//printf("clear user define class alarm when change task template\n\r");
							
							/*need to check whether template is different, if different change   ??????*/
							gpnStatCycClassChgAlmTp(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_USR, \
								pStatScanPort->statUCycRec->ptaskNode->almThredTpId, evnThredTpId);							
						}
					}
				}	
			}
		}	
		
		/*check port stast mon whether in task*/
		reVal = gpnStatCheckPortMonInTask(pStatLocalPort, taskId);
		if (reVal != GPN_STAT_DBS_GEN_OK)
		{
			/*enable port stat mon thrid : init memrey space */
			reVal = gpnStatCycClassMemoryInit(pStatLocalPort, cycClass);
			if(reVal != GPN_STAT_DBS_GEN_OK)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) memory init, return(%d)\n\r",\
					__FUNCTION__, cycClass, reVal);
			}

			/*enable port stat mon forth : reset cycClass memory space */
			reVal = gpnStatCycClassMemoryReset(pStatLocalPort, cycClass);
			if(reVal != GPN_STAT_DBS_GEN_OK)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) memory reset, return(%d)\n\r",\
					__FUNCTION__, cycClass, reVal);
			}

			reVal = gpnStatPortMon2Task(pStatLocalPort, taskId);
			if(reVal != GPN_STAT_DBS_GEN_OK)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) port int task, return(%d)\n\r",\
					__FUNCTION__, cycClass, reVal);
			}

			/*enable port stat mon fifth : enable cycClass stat mon */
			reVal = gpnStatCycClassEnable(pStatLocalPort, scanTpPollCyc,\
				cycClass, almThredTpId, subReptTpId, evnThredTpId, subFiltTpId, histReport);
			if(reVal != GPN_STAT_DBS_GEN_OK)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) enable, return(%d)\n\r",\
					__FUNCTION__, cycClass, reVal);
			}
			else 
			{
				if(pStatLocalPort && pStatLocalPort->pStatScanPort)
				{
					pStatLocalPort->pStatScanPort->insAddFlag = 1;
				}
			}
		}
		else
		{
			if((cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
				(pStatScanPort->statLCycRec != NULL))
			{
				pStatScanPort->statLCycRec->en = GPN_STAT_DBS_GEN_ENABLE;
			}
			else if((cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
					(pStatScanPort->statSCycRec != NULL))
			{
				pStatScanPort->statSCycRec->en = GPN_STAT_DBS_GEN_ENABLE;
			}
			else if((cycClass == GPN_STAT_CYC_CLASS_USR) &&\
					(pStatScanPort->statUCycRec != NULL))
			{
				pStatScanPort->statUCycRec->en = GPN_STAT_DBS_GEN_ENABLE;
			}
	
			if (cycClass == GPN_STAT_CYC_CLASS_LONG)
			{
				if (subReptTpId && subReptTpId != subReportLTmpId 
					&& subFiltTpId && subFiltTpId != subFiltLTmpId)
				{
					/*check whether need clear alarm*/
					peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
					gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
						pStatScanPort->statLCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_LONG);
					//printf("clear long class alarm when change data template\n\r");
					
					/*change data template */
					reVal = gpnStatCycClassChgDataTp(pStatLocalPort, scanTpPollCyc,\
						GPN_STAT_CYC_CLASS_LONG, subReptTpId, subFiltTpId);
					
					if(reVal != GPN_STAT_DBS_GEN_OK)
					{
						gpnLog(GPN_LOG_L_ERR, "%s(%d) : cycClass(%d) chg subReptTpId(%dTo%d) subFiltTpId (%dTo%d)err\n\r",\
							__FUNCTION__, __LINE__, cycClass, subReportLTmpId, subReptTpId, subFiltLTmpId, subFiltTpId);
					}
				}

				gpnStatCycClassHistAutoReport(pStatLocalPort, scanTpPollCyc,\
					GPN_STAT_CYC_CLASS_LONG, histReport);
			}

			if (cycClass == GPN_STAT_CYC_CLASS_SHORT) 
			{
				if (subReptTpId && subReptTpId != subReportSTmpId 
					&& subFiltTpId && subFiltTpId != subFiltSTmpId)
				{
					/*check whether need clear alarm*/
					peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
					gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
						pStatScanPort->statSCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_SHORT);
					//printf("clear short class alarm when change data template\n\r");
					
					/*change data template */
					reVal = gpnStatCycClassChgDataTp(pStatLocalPort, scanTpPollCyc,\
						GPN_STAT_CYC_CLASS_SHORT, subReptTpId, subFiltTpId);
					
					if(reVal != GPN_STAT_DBS_GEN_OK)
					{
						gpnLog(GPN_LOG_L_ERR, "%s(%d) : cycClass(%d) chg subReptTpId(%dTo%d) subFiltTpId (%dTo%d) err\n\r",\
							__FUNCTION__, __LINE__, cycClass, subReportSTmpId, subReptTpId, subFiltSTmpId, subFiltTpId);
					}
				}

				gpnStatCycClassHistAutoReport(pStatLocalPort, scanTpPollCyc,\
						GPN_STAT_CYC_CLASS_SHORT, histReport);
			}

			if (cycClass == GPN_STAT_CYC_CLASS_USR)
			{
				if (subReptTpId && subReptTpId != subReportUTmpId 
					&& subFiltTpId && subFiltTpId != subFiltUTmpId)
				{	
					/*check whether need clear alarm*/
					peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
					gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
						pStatScanPort->statUCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_USR);
					//printf("clear user define class alarm when change data template\n\r");
			
					/*change data template */
					reVal = gpnStatCycClassChgDataTp(pStatLocalPort, scanTpPollCyc,\
						GPN_STAT_CYC_CLASS_USR, subReptTpId, subFiltTpId);
					
					if(reVal != GPN_STAT_DBS_GEN_OK)
					{
						gpnLog(GPN_LOG_L_ERR, "%s(%d) : cycClass(%d) chg subReptTpId(%dTo%d) subFiltTpId (%dTo%d) err\n\r",\
							__FUNCTION__, __LINE__, cycClass, subReportUTmpId, subReptTpId, subFiltUTmpId, subFiltTpId);
					}
				}

				gpnStatCycClassHistAutoReport(pStatLocalPort, scanTpPollCyc,\
						GPN_STAT_CYC_CLASS_USR, histReport);
			}
		}

		/*currEn*/
		if (cycClass == GPN_STAT_CYC_CLASS_BASE)
		{
			pStatScanPort->currMonEn = currOpt;
		}
	}
	else if(opt == GPN_STAT_DBS_GEN_DISABLE)
	{
		/*disable cycClass stat mon */
		reVal = gpnStatCycClassDisable(pStatLocalPort, scanTpPollCyc, cycClass);
		//printf("%s : disable cycClass(%d) \n\r", __FUNCTION__, cycClass);
		if(reVal != GPN_STAT_DBS_GEN_OK)
		{
			gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) disable, return(%d)\n\r",\
				__FUNCTION__, cycClass, reVal);
		}
#if 0
		if (cycClass == GPN_STAT_CYC_CLASS_LONG)
		{
			peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
			gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
				pStatScanPort->statLCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_LONG);
			printf("clear long class alarm when disable port moni\n\r");
		}

		if (cycClass == GPN_STAT_CYC_CLASS_SHORT)
		{
			peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
			gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
				pStatScanPort->statSCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_SHORT);
			printf("clear short class alarm when disable port moni\n\r");
		}

		if (cycClass == GPN_STAT_CYC_CLASS_USR)
		{
			peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
			gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
				pStatScanPort->statUCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_USR);
			printf("clear user class alarm when disable port moni\n\r");
		}
#endif
	}
	else if(opt == GPN_STAT_DBS_GEN_DELETE) 
	{
		/* if delete long or base, delete long cyc */
		if( (cycClass == GPN_STAT_CYC_CLASS_LONG) ||\
			(cycClass == GPN_STAT_CYC_CLASS_BASE) )
		{
			/*clear alarm*/
			peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
			gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
				pStatScanPort->statLCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_LONG);
			//printf("clear long class alarm when delete port moni\n\r");

			/*clear Event*/
			gpnStatClearStatEventMark(pStatScanPort->statLCycRec);
			
			reVal = gpnStatCycClassDelete(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_LONG);
			if(reVal == GPN_STAT_DBS_GEN_ERR)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : delete cycClass(%d) return(%d)\n\r",\
					__FUNCTION__, GPN_STAT_CYC_CLASS_LONG, reVal);
			}

			/*lookup for taskId   geqian 2016.5.19*/
			if (taskId == 0)
			{
				if(pStatScanPort)
				{
					if (pStatScanPort->statLCycRec) 
					{
						if (pStatScanPort->statLCycRec->ptaskNode)
						{
							taskTmpId = pStatScanPort->statLCycRec->ptaskNode->taskId;
							reVal = gpnStatPortMonOutTask(pStatLocalPort, taskTmpId);
						}
					}
				}
			}
			else
			{
				reVal = gpnStatPortMonOutTask(pStatLocalPort, taskId);
			}	
			if(reVal != GPN_STAT_DBS_GEN_OK)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) port int task, return(%d)\n\r",\
					__FUNCTION__, cycClass, reVal);
			}

			reVal = gpnStatCycClassMemoryFree(pStatLocalPort, GPN_STAT_CYC_CLASS_LONG);
			if(reVal != GPN_STAT_DBS_GEN_OK)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) mem free, return(%d)\n\r",\
					__FUNCTION__, cycClass, reVal);
			}
		}
		if( (cycClass == GPN_STAT_CYC_CLASS_SHORT) ||\
			(cycClass == GPN_STAT_CYC_CLASS_BASE) )
		{
			/*clear alarm*/
			peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
			gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
				pStatScanPort->statSCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_SHORT);
			//printf("clear short class alarm when delete port moni\n\r");
			
			/*clear Event*/
			gpnStatClearStatEventMark(pStatScanPort->statSCycRec);
			
			reVal = gpnStatCycClassDelete(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_SHORT);
			if(reVal == GPN_STAT_DBS_GEN_ERR)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : delete cycClass(%d) return(%d)\n\r",\
					__FUNCTION__, GPN_STAT_CYC_CLASS_SHORT, reVal);
			}
			
			if (taskId == 0)
			{
				if(pStatScanPort)
				{
					if (pStatScanPort->statSCycRec) 
					{
						if (pStatScanPort->statSCycRec->ptaskNode)
						{
							taskTmpId = pStatScanPort->statSCycRec->ptaskNode->taskId;
							reVal = gpnStatPortMonOutTask(pStatLocalPort, taskTmpId);
						}
					}
				}
			}
			else
			{
				reVal = gpnStatPortMonOutTask(pStatLocalPort, taskId);
			}	
			if(reVal != GPN_STAT_DBS_GEN_OK)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) port int task, return(%d)\n\r",\
					__FUNCTION__, cycClass, reVal);
			}

			reVal = gpnStatCycClassMemoryFree(pStatLocalPort, GPN_STAT_CYC_CLASS_SHORT);
			if(reVal != GPN_STAT_DBS_GEN_OK)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) mem free, return(%d)\n\r",\
					__FUNCTION__, cycClass, reVal);
			}
		}
		
		if( (cycClass == GPN_STAT_CYC_CLASS_USR) ||\
			(cycClass == GPN_STAT_CYC_CLASS_BASE) )
		{
			/*clear alarm*/
			peerPortIndex = &(pStatLocalPort->pPeerNode->peerPort);
			gpnStatDeleteStatAlarm(pStatScanPort->pscanTypeNode, \
				pStatScanPort->statUCycRec, peerPortIndex, GPN_STAT_CYC_CLASS_USR);
			//printf("clear user class alarm when delete port moni\n\r");
			
			/*clear Event*/
			gpnStatClearStatEventMark(pStatScanPort->statSCycRec);
			
			reVal = gpnStatCycClassDelete(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_USR);
			if(reVal == GPN_STAT_DBS_GEN_ERR)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : delete cycClass(%d) return(%d)\n\r",\
					__FUNCTION__, GPN_STAT_CYC_CLASS_USR, reVal);
			}

			if (taskId == 0)
			{
				if(pStatScanPort)
				{
					if (pStatScanPort->statUCycRec) 
					{
						if (pStatScanPort->statUCycRec->ptaskNode)
						{
							taskTmpId = pStatScanPort->statUCycRec->ptaskNode->taskId;
							reVal = gpnStatPortMonOutTask(pStatLocalPort, taskTmpId);
						}
					}
				}
			}
			else
			{
				reVal = gpnStatPortMonOutTask(pStatLocalPort, taskId);
			}	
		
			if(reVal != GPN_STAT_DBS_GEN_OK)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) port int task, return(%d)\n\r",\
					__FUNCTION__, cycClass, reVal);
			}

			reVal = gpnStatCycClassMemoryFree(pStatLocalPort, GPN_STAT_CYC_CLASS_USR);
			if(reVal != GPN_STAT_DBS_GEN_OK)
			{
				gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) mem free, return(%d)\n\r",\
					__FUNCTION__, cycClass, reVal);
			}
		}
	#if 0	
		if(cycClass == GPN_STAT_CYC_CLASS_BASE)
		{
	#endif
			if ((cycClass == GPN_STAT_CYC_CLASS_BASE) ||
				((pStatScanPort->statLCycRec == NULL) &&
				(pStatScanPort->statSCycRec == NULL) &&
				(pStatScanPort->statUCycRec == NULL)))
			{
				reVal = gpnStatCycClassDelete(pStatLocalPort, scanTpPollCyc, GPN_STAT_CYC_CLASS_BASE);
				
				if(reVal == GPN_STAT_DBS_GEN_ERR)
				{
					gpnLog(GPN_LOG_L_ERR, "%s : delete cycClass(%d) return(%d)\n\r",\
						__FUNCTION__, GPN_STAT_CYC_CLASS_BASE, reVal);
				}

				reVal = gpnStatCycClassMemoryFree(pStatLocalPort, GPN_STAT_CYC_CLASS_BASE);
				
				if(reVal != GPN_STAT_DBS_GEN_OK)
				{
					gpnLog(GPN_LOG_L_ERR, "%s : cycClass(%d) mem free, return(%d)\n\r",\
						__FUNCTION__, cycClass, reVal);
				}

				if(pStatLocalPort && pStatLocalPort->pStatScanPort)
				{
					pStatLocalPort->pStatScanPort->insAddFlag = 0;
				}
			}
	#if 0
		}
	#endif

	}

	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatCycClassMemoryMalloc(stStatLocalPortNode *pStatLocalNode, UINT32 cycClass)
{
	UINT32 i;
	UINT32 j;
	UINT8 *p;
	void *pstatData;
	UINT32 portType;
	UINT32 histBfSize;
	UINT32 histBuffNum;
	UINT32 subInScanNum;
	objLogicDesc *pLocalPort;
	stStatHistRecdUnit *phistRecdUnit;
	stStatScanPortInfo *pStatScanPort;
	stPortTpToStatScanTp *pPortTpToScanTp;
	
	/* assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	if( (cycClass != GPN_STAT_CYC_CLASS_BASE) &&\
		(cycClass != GPN_STAT_CYC_CLASS_LONG) &&\
		(cycClass != GPN_STAT_CYC_CLASS_SHORT) &&\
		(cycClass != GPN_STAT_CYC_CLASS_USR) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) cycClass(%d) err!\n\r",\
			__FUNCTION__,\
			pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
			pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
			pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
			cycClass);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanPort = NULL;
	pLocalPort = &(pStatLocalNode->localPort);
	pStatScanPort = pStatLocalNode->pStatScanPort;
	if(pStatScanPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	/*
		check if mem already malloc,
		exp : enable -> disable -> enable
	*/
	if(((cycClass == GPN_STAT_CYC_CLASS_BASE) &&\
		(pStatScanPort->statBaseDate != NULL)) ||\
	   ((cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
	    (pStatScanPort->statLCycRec != NULL)) ||\
	   ((cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
	    (pStatScanPort->statSCycRec != NULL)) ||\
	   ((cycClass == GPN_STAT_CYC_CLASS_USR) &&\
	    (pStatScanPort->statUCycRec != NULL)))
	{
		gpnLog(GPN_LOG_L_INFO, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) cyc(%d) already malloc!\n\r",\
			__FUNCTION__,\
			pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6,\
			cycClass);

		return GPN_STAT_DBS_GEN_OK;
	}

	/*
		check port xCyc memrey about info
	*/
	pPortTpToScanTp = NULL;
	portType = PortIndex_GetType(pLocalPort->portIndex);
	gpnStatSeekPortType2PTVsST(portType, &pPortTpToScanTp);
	if(pPortTpToScanTp == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) portType(%08x) vs statType err!\n\r",\
			__FUNCTION__,\
			pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6,\
			portType);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*
		calculate port xCyc memrey
	*/
	histBfSize = 0;
	histBuffNum = 0;
	subInScanNum = pPortTpToScanTp->statScanTypeNode->statSubTpNumInScanType;
	if(cycClass == GPN_STAT_CYC_CLASS_BASE)
	{
		/*
			base data space include : 
			1 baseData space;
			2 currData space;
		*/
		i = 2*pPortTpToScanTp->statDataSizeForPort;
	}
	else
	{
		if(cycClass == GPN_STAT_CYC_CLASS_LONG)
		{
			histBuffNum = GPN_STAT_HIST_LCYC_REC_NUM;
		}
		else if(cycClass == GPN_STAT_CYC_CLASS_SHORT)
		{
			histBuffNum = GPN_STAT_HIST_SCYC_REC_NUM;
		}
		else /*if(cycClass == GPN_STAT_CYC_CLASS_USR)*/
		{
			histBuffNum = GPN_STAT_HIST_UCYC_REC_NUM;
		}

		histBfSize = sizeof(stStatDataElement)*subInScanNum;
		/*
			xCyc data space include :
			1 cycClass struct size, here is sizeof(stStatXCycDataRecd);
			2 hist's curr data(used for calculat hist buff date);
			3 hist buff struct size;
			4 hist data in hist buff struct;
		*/
		i = sizeof(stStatXCycDataRecd);
		i += pPortTpToScanTp->statDataSizeForPort;
		i += sizeof(stStatHistRecdUnit)*histBuffNum;
		i += histBfSize*histBuffNum;

		/*xCyc malloc debug*/
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "xCycRecd %d recdUnit %d histBfSize %d histBuffNum %d currSize %d\n\r",\
			sizeof(stStatXCycDataRecd), sizeof(stStatHistRecdUnit),\
			histBfSize, histBuffNum, pPortTpToScanTp->statDataSizeForPort);
	}

	/*
		malloc and memset port xCyc memrey
	*/
	p = malloc(i);
	if(p == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) mollac err!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}
	gpnLog(GPN_LOG_L_INFO, "%s : mollac(%d) byte for dev(%08x) port(%08x|%08x|%08x|%08x|%08x) cycClass(%d)!\n\r",\
		__FUNCTION__, i,\
		pLocalPort->devIndex, pLocalPort->portIndex,\
		pLocalPort->portIndex3, pLocalPort->portIndex4,\
		pLocalPort->portIndex5, pLocalPort->portIndex6,\
		cycClass);
	memset(p, 0, i);

	/*
		init port xCyc memrey
	*/
	if(cycClass == GPN_STAT_CYC_CLASS_BASE)
	{
		/*base and curr data space disp */
		pStatScanPort->statBaseDate = (void *)p;
		pStatScanPort->statCurrDate = (void *)((UINT8 *)p + pPortTpToScanTp->statDataSizeForPort);
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_LONG)
	{
		/*porintor to LCycRec start : mem part 1*/
		pStatScanPort->statLCycRec = (stStatXCycDataRecd *)p;

		/*porintor to Long hist curr data : mem part 2*/
		pStatScanPort->statLCycRec->xCycCurrDate =\
			(void *)((pStatScanPort->statLCycRec) + 1);

		/*porintor to Long hist data node : mem part 3*/
		phistRecdUnit = (stStatHistRecdUnit *)((UINT8 *)(pStatScanPort->statLCycRec->xCycCurrDate) +\
			pPortTpToScanTp->statDataSizeForPort);

		/*porintor to Long hist data payload : mem part 4*/
		pstatData = (void *)(phistRecdUnit + histBuffNum);
		
		for(j=0;j<GPN_STAT_HIST_LCYC_REC_NUM;j++)
		{
			//printf("%08x %08x\n\r", (UINT32)phistRecdUnit, (UINT32)pstatData);
			pStatScanPort->statLCycRec->pstatXCycHistRecd[j] = phistRecdUnit;
			phistRecdUnit->statDate = pstatData;

			phistRecdUnit++;
			pstatData = (void *)((UINT8 *)pstatData + histBfSize);
		}
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_SHORT)
	{
		/*porintor to LCycRec start : mem part 1*/
		pStatScanPort->statSCycRec = (stStatXCycDataRecd *)p;

		/*porintor to Long hist curr data : mem part 2*/
		pStatScanPort->statSCycRec->xCycCurrDate =\
			(void *)((pStatScanPort->statSCycRec) + 1);

		/*porintor to Long hist data node : mem part 3*/
		phistRecdUnit = (stStatHistRecdUnit *)((UINT8 *)(pStatScanPort->statSCycRec->xCycCurrDate) +\
			pPortTpToScanTp->statDataSizeForPort);

		/*porintor to Long hist data payload : mem part 4*/
		pstatData = (void *)(phistRecdUnit + histBuffNum);
		
		for(j=0;j<GPN_STAT_HIST_SCYC_REC_NUM;j++)
		{
			//printf("%08x %08x\n\r", (UINT32)phistRecdUnit, (UINT32)pstatData);
			pStatScanPort->statSCycRec->pstatXCycHistRecd[j] = phistRecdUnit;
			phistRecdUnit->statDate = pstatData;

			phistRecdUnit++;
			pstatData = (void *)((UINT8 *)pstatData + histBfSize);
		}
	}
	else /* if(cycClass == GPN_STAT_CYC_CLASS_USR) */
	{
		/*porintor to LCycRec start : mem part 1*/
		pStatScanPort->statUCycRec = (stStatXCycDataRecd *)p;

		/*porintor to Long hist curr data : mem part 2*/
		pStatScanPort->statUCycRec->xCycCurrDate =\
			(void *)((pStatScanPort->statUCycRec) + 1);

		/*porintor to Long hist data node : mem part 3*/
		phistRecdUnit = (stStatHistRecdUnit *)((UINT8 *)(pStatScanPort->statUCycRec->xCycCurrDate) +\
			pPortTpToScanTp->statDataSizeForPort);

		/*porintor to Long hist data payload : mem part 4*/
		pstatData = (void *)(phistRecdUnit + histBuffNum);
		
		for(j=0;j<GPN_STAT_HIST_UCYC_REC_NUM;j++)
		{
			//printf("%08x %08x\n\r", (UINT32)phistRecdUnit, (UINT32)pstatData);
			pStatScanPort->statUCycRec->pstatXCycHistRecd[j] = phistRecdUnit;
			phistRecdUnit->statDate = pstatData;

			phistRecdUnit++;
			pstatData = (void *)((UINT8 *)pstatData + histBfSize);
		}
	}
	
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatCycClassMemoryInit(stStatLocalPortNode *pStatLocalNode, UINT32 cycClass)
{
	UINT32 i;
	UINT32 j;
	UINT32 portType;
	UINT32 subInScanNum;
	objLogicDesc *pLocalPort;
	stStatHistRecdUnit *phistRecdUnit;
	stPortTpToStatScanTp *pPortTpToScanTp;
	stStatScanPortInfo *pStatScanPort;
	
	/* assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	if( (cycClass != GPN_STAT_CYC_CLASS_BASE) &&\
		(cycClass != GPN_STAT_CYC_CLASS_LONG) &&\
		(cycClass != GPN_STAT_CYC_CLASS_SHORT) &&\
		(cycClass != GPN_STAT_CYC_CLASS_USR) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) cycClass(%d) err!\n\r",\
			__FUNCTION__,\
			pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
			pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
			pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
			cycClass);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanPort = NULL;
	pLocalPort = &(pStatLocalNode->localPort);
	pStatScanPort = pStatLocalNode->pStatScanPort;
	if(pStatScanPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	/*
		check if mem already malloc,
		exp : enable -> disable -> enable
	*/
	if(((cycClass == GPN_STAT_CYC_CLASS_BASE) &&\
		(pStatScanPort->statBaseDate == NULL)) ||\
	   ((cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
	    (pStatScanPort->statLCycRec == NULL)) ||\
	   ((cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
	    (pStatScanPort->statSCycRec == NULL)) ||\
	   ((cycClass == GPN_STAT_CYC_CLASS_USR) &&\
	    (pStatScanPort->statUCycRec == NULL)))
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) cyc(%d) not malloc!\n\r",\
			__FUNCTION__,\
			pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6,\
			cycClass);
		return GPN_STAT_DBS_GEN_OK;
	}

	/*
		check port xCyc memrey about info
	*/
	pPortTpToScanTp = NULL;
	portType = PortIndex_GetType(pLocalPort->portIndex);
	gpnStatSeekPortType2PTVsST(portType, &pPortTpToScanTp);
	if(pPortTpToScanTp == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) portType(%08x) vs statType err!\n\r",\
			__FUNCTION__,\
			pLocalPort->devIndex, pLocalPort->portIndex,\
			pLocalPort->portIndex3, pLocalPort->portIndex4,\
			pLocalPort->portIndex5, pLocalPort->portIndex6,\
			portType);
		return GPN_STAT_DBS_GEN_ERR;
	}
	subInScanNum = pPortTpToScanTp->statScanTypeNode->statSubTpNumInScanType;

	/*
		init port xCyc memrey
	*/
	if(cycClass == GPN_STAT_CYC_CLASS_BASE)
	{
		pStatScanPort->isBaseInit = GPN_STAT_DBS_32_FFFF;
		pStatScanPort->currMonEn  = GPN_STAT_DBS_GEN_DISABLE;
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_LONG)
	{
		pStatScanPort->statLCycRec->en = GPN_STAT_DBS_GEN_DISABLE;
		pStatScanPort->statLCycRec->recStart = 0;
		/*init recEnd, when first rec in buff recEnd will be '0' */
		pStatScanPort->statLCycRec->recEnd = GPN_STAT_DBS_08_FFFF;
		pStatScanPort->statLCycRec->trapEn = GPN_STAT_DBS_GEN_DISABLE;
		/*get xCyc hist data localtion */
		pStatScanPort->statLCycRec->xhistDataLoc = gpnStatHistDataTpIdGet();
		/*???*/
		pStatScanPort->statLCycRec->histRecNum = 0;
		pStatScanPort->statLCycRec->histRecBufoNum = GPN_STAT_HIST_LCYC_REC_NUM;
		pStatScanPort->statLCycRec->subStatNum = subInScanNum;
		pStatScanPort->statLCycRec->dataBitDeep = pStatScanPort->dataBitDeep;
		pStatScanPort->statLCycRec->valueType = pStatScanPort->valueType;
		pStatScanPort->statLCycRec->numType = pStatScanPort->numType;
		pStatScanPort->statLCycRec->xCycEvnThId = GPN_STAT_INVALID_TEMPLAT_ID;
		pStatScanPort->statLCycRec->xCycEvnThred = NULL;
		pStatScanPort->statLCycRec->xCycAlmThId = GPN_STAT_INVALID_TEMPLAT_ID;
		pStatScanPort->statLCycRec->xCycAlmThred = NULL;
		pStatScanPort->statLCycRec->xCycsubFiltId = GPN_STAT_INVALID_TEMPLAT_ID;
		pStatScanPort->statLCycRec->subFiltInfo = NULL;
		pStatScanPort->statLCycRec->xCycsubReptId = GPN_STAT_INVALID_TEMPLAT_ID;
		pStatScanPort->statLCycRec->subReptInfo = NULL;
		pStatScanPort->statLCycRec->trapTSlot = 0;
		pStatScanPort->statLCycRec->statXCycUpAlm = 0;
		pStatScanPort->statLCycRec->statXCycDownAlm = 0;
		pStatScanPort->statLCycRec->statXCycUpEvent = 0;
		pStatScanPort->statLCycRec->statXCycDownEvent = 0;
		pStatScanPort->statLCycRec->ptaskNode = NULL;
		
		for(i=0;i<GPN_STAT_HIST_LCYC_REC_NUM;i++)
		{
			phistRecdUnit = pStatScanPort->statLCycRec->pstatXCycHistRecd[i];

			/*init phistRecdUnit */
			phistRecdUnit->histDataLoc = pStatScanPort->statLCycRec->xhistDataLoc;
			phistRecdUnit->stopTime = 0;
			phistRecdUnit->en = GPN_STAT_DBS_GEN_DISABLE;
			phistRecdUnit->subWild = subInScanNum;
			phistRecdUnit->validSub = 0;
		}
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_SHORT)
	{
		pStatScanPort->statSCycRec->en = GPN_STAT_DBS_GEN_DISABLE;
		pStatScanPort->statSCycRec->recStart = 0;
		/*init recEnd, when first rec in buff recEnd will be '0' */
		pStatScanPort->statSCycRec->recEnd = GPN_STAT_DBS_08_FFFF;
		pStatScanPort->statSCycRec->trapEn = GPN_STAT_DBS_GEN_DISABLE;
		/*get xCyc hist data localtion */
		pStatScanPort->statSCycRec->xhistDataLoc = gpnStatHistDataTpIdGet();
		/*???*/
		pStatScanPort->statSCycRec->histRecNum = 0;
		pStatScanPort->statSCycRec->histRecBufoNum = GPN_STAT_HIST_SCYC_REC_NUM;
		pStatScanPort->statSCycRec->subStatNum = subInScanNum;
		pStatScanPort->statSCycRec->dataBitDeep = pStatScanPort->dataBitDeep;
		pStatScanPort->statSCycRec->valueType = pStatScanPort->valueType;
		pStatScanPort->statSCycRec->numType = pStatScanPort->numType;
		pStatScanPort->statSCycRec->xCycEvnThId = GPN_STAT_INVALID_TEMPLAT_ID;
		pStatScanPort->statSCycRec->xCycEvnThred = NULL;
		pStatScanPort->statSCycRec->xCycAlmThId = GPN_STAT_INVALID_TEMPLAT_ID;
		pStatScanPort->statSCycRec->xCycAlmThred = NULL;
		pStatScanPort->statSCycRec->xCycsubFiltId = GPN_STAT_INVALID_TEMPLAT_ID;
		pStatScanPort->statSCycRec->subFiltInfo = NULL;
		pStatScanPort->statSCycRec->xCycsubReptId = GPN_STAT_INVALID_TEMPLAT_ID;
		pStatScanPort->statSCycRec->subReptInfo = NULL;
		pStatScanPort->statSCycRec->trapTSlot = 0;
		pStatScanPort->statSCycRec->statXCycUpAlm = 0;
		pStatScanPort->statSCycRec->statXCycDownAlm = 0;
		pStatScanPort->statSCycRec->statXCycUpEvent = 0;
		pStatScanPort->statSCycRec->statXCycDownEvent = 0;
		pStatScanPort->statSCycRec->ptaskNode = NULL;
		
		for(j=0;j<GPN_STAT_HIST_SCYC_REC_NUM;j++)
		{
			phistRecdUnit = pStatScanPort->statSCycRec->pstatXCycHistRecd[j];
			
			/*init phistRecdUnit */
			phistRecdUnit->histDataLoc = pStatScanPort->statSCycRec->xhistDataLoc;
			phistRecdUnit->stopTime = 0;
			phistRecdUnit->en = GPN_STAT_DBS_GEN_DISABLE;
			phistRecdUnit->subWild = subInScanNum;
			phistRecdUnit->validSub = 0;
		}
	}
	else /* if(cycClass == GPN_STAT_CYC_CLASS_USR) */
	{
		pStatScanPort->statUCycRec->en = GPN_STAT_DBS_GEN_DISABLE;
		pStatScanPort->statUCycRec->recStart = 0;
		/*init recEnd, when first rec in buff recEnd will be '0' */
		pStatScanPort->statUCycRec->recEnd = GPN_STAT_DBS_08_FFFF;
		pStatScanPort->statUCycRec->trapEn = GPN_STAT_DBS_GEN_DISABLE;
		/*get xCyc hist data localtion */
		pStatScanPort->statUCycRec->xhistDataLoc = gpnStatHistDataTpIdGet();
		/*???*/
		pStatScanPort->statUCycRec->histRecNum = 0;
		pStatScanPort->statUCycRec->histRecBufoNum = GPN_STAT_HIST_UCYC_REC_NUM;
		pStatScanPort->statUCycRec->subStatNum = subInScanNum;
		pStatScanPort->statUCycRec->dataBitDeep = pStatScanPort->dataBitDeep;
		pStatScanPort->statUCycRec->valueType = pStatScanPort->valueType;
		pStatScanPort->statUCycRec->numType = pStatScanPort->numType;
		pStatScanPort->statUCycRec->xCycEvnThId = GPN_STAT_INVALID_TEMPLAT_ID;
		pStatScanPort->statUCycRec->xCycEvnThred = NULL;
		pStatScanPort->statUCycRec->xCycAlmThId = GPN_STAT_INVALID_TEMPLAT_ID;
		pStatScanPort->statUCycRec->xCycAlmThred = NULL;
		pStatScanPort->statUCycRec->xCycsubFiltId = GPN_STAT_INVALID_TEMPLAT_ID;
		pStatScanPort->statUCycRec->subFiltInfo = NULL;
		pStatScanPort->statUCycRec->xCycsubReptId = GPN_STAT_INVALID_TEMPLAT_ID;
		pStatScanPort->statUCycRec->subReptInfo = NULL;
		pStatScanPort->statUCycRec->trapTSlot = 0;
		pStatScanPort->statUCycRec->statXCycUpAlm = 0;
		pStatScanPort->statUCycRec->statXCycDownAlm = 0;
		pStatScanPort->statUCycRec->statXCycUpEvent = 0;
		pStatScanPort->statUCycRec->statXCycDownEvent = 0;
		pStatScanPort->statUCycRec->ptaskNode = NULL;
		
		for(j=0;j<GPN_STAT_HIST_UCYC_REC_NUM;j++)
		{
			phistRecdUnit = pStatScanPort->statUCycRec->pstatXCycHistRecd[j];
			
			/*init phistRecdUnit */
			phistRecdUnit->histDataLoc = pStatScanPort->statUCycRec->xhistDataLoc;
			phistRecdUnit->stopTime = 0;
			phistRecdUnit->en = GPN_STAT_DBS_GEN_DISABLE;
			phistRecdUnit->subWild = subInScanNum;
			phistRecdUnit->validSub = 0;
		}
	}

	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatCycClassMemoryFree(stStatLocalPortNode *pStatLocalNode, UINT32 cycClass)
{
	stStatScanPortInfo *pStatScanPort;
	
	/* assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* 
		free base cyc data space : statBaseDate;
		free hist xCyc data space : statXCycRec;
		set idle porinter to NULL
	*/
	pStatScanPort = pStatLocalNode->pStatScanPort;
	if (pStatScanPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if( (cycClass == GPN_STAT_CYC_CLASS_BASE) &&\
		(pStatScanPort->statBaseDate != NULL) )
	{
		free(pStatScanPort->statBaseDate);
		pStatScanPort->statBaseDate = NULL;
		pStatScanPort->statCurrDate = NULL;
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
			(pStatScanPort->statLCycRec != NULL))
	{
		free(pStatScanPort->statLCycRec);
		pStatScanPort->statLCycRec = NULL;
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
			(pStatScanPort->statSCycRec != NULL))
	{
		free(pStatScanPort->statSCycRec);
		pStatScanPort->statSCycRec = NULL;
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_USR) &&\
			(pStatScanPort->statUCycRec != NULL))
	{
		free(pStatScanPort->statUCycRec);
		pStatScanPort->statUCycRec = NULL;
	}
	else
	{
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) cycClass(%d) err!\n\r",\
			__FUNCTION__,\
			pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
			pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
			pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
			cycClass);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatCycClassMemoryReset(stStatLocalPortNode *pStatLocalNode, UINT32 cycClass)
{
	stStatScanPortInfo *pStatScanPort;
	
	/* assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pStatScanPort = pStatLocalNode->pStatScanPort;
	if( (cycClass == GPN_STAT_CYC_CLASS_BASE) &&\
		(pStatScanPort->statBaseDate != NULL) &&\
		(pStatScanPort->statCurrDate != NULL) )
	{
		memset(pStatScanPort->statBaseDate, 0, pStatScanPort->dataSize);
		pStatScanPort->isBaseInit = GPN_STAT_DBS_32_FFFF;

		memset(pStatScanPort->statCurrDate, 0, pStatScanPort->dataSize);
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
			(pStatScanPort->statLCycRec != NULL))
	{
		pStatScanPort->statLCycRec->recStart = 0;
		pStatScanPort->statLCycRec->recEnd = GPN_STAT_DBS_08_FFFF;
		pStatScanPort->statLCycRec->histRecNum = 0;
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
			(pStatScanPort->statSCycRec != NULL))
	{
		pStatScanPort->statSCycRec->recStart = 0;
		pStatScanPort->statSCycRec->recEnd = GPN_STAT_DBS_08_FFFF;
		pStatScanPort->statSCycRec->histRecNum = 0;
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_USR) &&\
			(pStatScanPort->statUCycRec != NULL))
	{
		pStatScanPort->statUCycRec->recStart = 0;
		pStatScanPort->statUCycRec->recEnd = GPN_STAT_DBS_08_FFFF;
		pStatScanPort->statUCycRec->histRecNum = 0;
	}
	else
	{
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) cycClass(%d) err!\n\r",\
			__FUNCTION__,\
			pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
			pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
			pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
			cycClass);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatCheckPortMonInTask(stStatLocalPortNode *pStatLocalNode, UINT32 taskId)
{
	UINT32 portNum;
	stStatTaskNode *ptaskNode;
	stStatMonObjNode *pMonObjNode;

	/* assert */
	if( (pStatLocalNode == NULL) ||\
		(taskId == GPN_STAT_INVALID_TASK_ID) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	ptaskNode = NULL;
	gpnStatTaskGet(taskId, &ptaskNode);
	if(ptaskNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : task(%d) not found node, err!\n\r",\
			__FUNCTION__, taskId);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*
		localPortNode's xCyc could add in only one task
	*/
	portNum = 0;
	pMonObjNode = (stStatMonObjNode *)listFirst(&(ptaskNode->actMOnOBjQuen));
	while((pMonObjNode != NULL) && (portNum < ptaskNode->actMOnOBjNum))
	{
		//printf("portNum:%d, actMOnOBjNum:%d\n\r", portNum, ptaskNode->actMOnOBjNum);
		if(pMonObjNode->pStatLocalPort == pStatLocalNode)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) alreadly add in task(%d) cyc(%d), err!\n\r",\
				__FUNCTION__,\
				pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
				pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
				pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
				taskId, ptaskNode->cycClass);
			return GPN_STAT_DBS_GEN_OK;
		}
		portNum++;
		pMonObjNode = (stStatMonObjNode *)listNext((NODE *)pMonObjNode);
	}

	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatPortMon2Task(stStatLocalPortNode *pStatLocalNode, UINT32 taskId)
{
	UINT32 portNum;
	stStatTaskNode *ptaskNode;
	stStatTaskInfo *pstatTaskInfo;
	stStatMonObjNode *pMonObjNode;
	stStatMonObjNode *pIdleObjNode;
	stStatScanPortInfo *pStatScanPort;

	/* assert */
	if( (pStatLocalNode == NULL) ||\
		(taskId == GPN_STAT_INVALID_TASK_ID) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	ptaskNode = NULL;
	gpnStatTaskGet(taskId, &ptaskNode);
	if(ptaskNode == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : task(%d) not found node, err!\n\r",\
			__FUNCTION__, taskId);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*
		localPortNode's xCyc could add in only one task
	*/
	portNum = 0;
	pMonObjNode = (stStatMonObjNode *)listFirst(&(ptaskNode->actMOnOBjQuen));
	while((pMonObjNode != NULL) && (portNum < ptaskNode->actMOnOBjNum))
	{
		if(pMonObjNode->pStatLocalPort == pStatLocalNode)
		{
			gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) alreadly add in task(%d) cyc(%d), err!\n\r",\
				__FUNCTION__,\
				pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
				pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
				pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
				taskId, ptaskNode->cycClass);
			return GPN_STAT_DBS_GEN_ERR;
		}
		portNum++;
		pMonObjNode = (stStatMonObjNode *)listNext((NODE *)pMonObjNode);
	}

	/*
		get ilde mon port node from idleQuen
	*/
	pstatTaskInfo = &(pgstEQUStatProcSpace->statTaskInfo);
	if(pstatTaskInfo->idleMOnOBjNum == 0)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : do not have idleMOnOBjNdoe, err!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	pIdleObjNode = (stStatMonObjNode *)listGet(&(pstatTaskInfo->idleMOnOBjQuen));
	if(pIdleObjNode == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : get idleMOnOBjNdoe err!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	if(pstatTaskInfo->idleMOnOBjNum > 0)
	{
		pstatTaskInfo->idleMOnOBjNum--;
	}

	/*
		add idle mon port node in actQuen
	*/
	pIdleObjNode->taskId = ptaskNode->taskId;
	pIdleObjNode->pStatLocalPort = pStatLocalNode;
	listAdd(&(ptaskNode->actMOnOBjQuen), (NODE *)pIdleObjNode);
	ptaskNode->actMOnOBjNum++;

	/*
		set relationShip wiht localPortNode xCyc and taskNode
	*/
	pStatScanPort = pStatLocalNode->pStatScanPort;
	if( (ptaskNode->cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
		(pStatScanPort->statLCycRec != NULL) )
	{
		pStatScanPort->statLCycRec->ptaskNode = ptaskNode;
	}
	else if((ptaskNode->cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
			(pStatScanPort->statSCycRec != NULL))
	{
		pStatScanPort->statSCycRec->ptaskNode = ptaskNode;
	}
	else if((ptaskNode->cycClass == GPN_STAT_CYC_CLASS_USR) &&\
			(pStatScanPort->statUCycRec != NULL))
	{
		pStatScanPort->statUCycRec->ptaskNode = ptaskNode;
	}
	else
	{
		gpnLog(GPN_LOG_L_ERR, "%s : get idleMOnOBjNdoe err!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatPortMonOutTask(stStatLocalPortNode *pStatLocalNode, UINT32 taskId)
{
	UINT32 monPortNum;
	stStatTaskNode *ptaskNode;
	stStatTaskInfo *pstatTaskInfo;
	stStatMonObjNode *pmonObjNode;
	objLogicDesc *pLocalPort;
	stStatScanPortInfo *pStatScanPort;

	/* assert */
	if (pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	ptaskNode = NULL;
	gpnStatTaskGet(taskId, &ptaskNode);
	if(ptaskNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : task(%d) not found node, err!\n\r",\
			__FUNCTION__, taskId);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pstatTaskInfo = &(pgstEQUStatProcSpace->statTaskInfo);

	monPortNum = 0;
	pmonObjNode = (stStatMonObjNode *)listFirst(&(ptaskNode->actMOnOBjQuen));
	while((pmonObjNode!=NULL)&&(monPortNum<ptaskNode->actMOnOBjNum))
	{
		if (pmonObjNode->pStatLocalPort == NULL)
		{
			continue;
		}
		
		pLocalPort = &(pmonObjNode->pStatLocalPort->localPort);
		if( (pLocalPort->devIndex == pStatLocalNode->localPort.devIndex) &&\
			(pLocalPort->portIndex == pStatLocalNode->localPort.portIndex) &&\
			(pLocalPort->portIndex3 == pStatLocalNode->localPort.portIndex3) &&\
			(pLocalPort->portIndex4 == pStatLocalNode->localPort.portIndex4) &&\
			(pLocalPort->portIndex5 == pStatLocalNode->localPort.portIndex5) &&\
			(pLocalPort->portIndex6 == pStatLocalNode->localPort.portIndex6) )
		{
			/*
				delete mon port node from actQuen, add mon port node in idleQuen
			*/
			listDelete(&(ptaskNode->actMOnOBjQuen), (NODE *)pmonObjNode);
			if(ptaskNode->actMOnOBjNum > 0)
			{
				ptaskNode->actMOnOBjNum--;
			}
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : actMOnOBjNum(%d)  idleMOnOBjNum(%d)\n\r",\
				__FUNCTION__, ptaskNode->actMOnOBjNum, pstatTaskInfo->idleMOnOBjNum);

			listAdd(&(pstatTaskInfo->idleMOnOBjQuen), (NODE *)pmonObjNode);
			pstatTaskInfo->idleMOnOBjNum++;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : after delete task actMOnOBjNum(%d)  idleMOnOBjNum(%d)\n\r",\
				__FUNCTION__, ptaskNode->actMOnOBjNum, pstatTaskInfo->idleMOnOBjNum);

			/*
				release relationShip between LocalPortNode xCyc and taskNode
			*/
			pStatScanPort = pStatLocalNode->pStatScanPort;
			if (pStatScanPort)
			{
				if(ptaskNode->cycClass == GPN_STAT_CYC_CLASS_LONG)
				{
					pStatScanPort->statLCycRec->ptaskNode = NULL;
				}
				else if(ptaskNode->cycClass == GPN_STAT_CYC_CLASS_SHORT)
				{
					pStatScanPort->statSCycRec->ptaskNode = NULL;
				}
				else if(ptaskNode->cycClass == GPN_STAT_CYC_CLASS_USR)
				{
					pStatScanPort->statUCycRec->ptaskNode = NULL;
				}
			}
			
			break;
		}
		monPortNum++;
		pmonObjNode = (stStatMonObjNode *)listNext((NODE *)pmonObjNode);
	}

	return GPN_STAT_DBS_GEN_OK;
}

/*
	long short & user define cyc hist data all come 
	from base data, so how long the poll cyc should be set 
*/
UINT32 gpnStatRenewPortPollCyc(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc)
{
	UINT32 miniCyc;
	stStatScanPortInfo *pStatScanPort;
	
	/* assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	miniCyc = 0xFFFFFFFF;
	pStatScanPort = pStatLocalNode->pStatScanPort;
	
	if( (pStatScanPort->statLCycRec != NULL) &&\
		(pStatScanPort->statLCycRec->ptaskNode != NULL) &&\
		(pStatScanPort->statLCycRec->en == GPN_STAT_DBS_GEN_ENABLE) )
	{
		if(pStatScanPort->statLCycRec->ptaskNode->cycSeconds < miniCyc)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : longCyc(%d)!\n\r",\
				__FUNCTION__, pStatScanPort->statLCycRec->ptaskNode->cycSeconds);
			miniCyc = pStatScanPort->statLCycRec->ptaskNode->cycSeconds;
		}
	}
	if( (pStatScanPort->statSCycRec != NULL) &&\
		(pStatScanPort->statSCycRec->ptaskNode != NULL) &&\
		(pStatScanPort->statSCycRec->en == GPN_STAT_DBS_GEN_ENABLE) )
	{
		if(pStatScanPort->statSCycRec->ptaskNode->cycSeconds < miniCyc)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : shortCyc(%d)!\n\r",\
				__FUNCTION__, pStatScanPort->statSCycRec->ptaskNode->cycSeconds);
			miniCyc = pStatScanPort->statSCycRec->ptaskNode->cycSeconds;
		}
	}
	if( (pStatScanPort->statUCycRec != NULL) &&\
		(pStatScanPort->statUCycRec->ptaskNode != NULL) &&\
		(pStatScanPort->statUCycRec->en == GPN_STAT_DBS_GEN_ENABLE) )
	{
		if(pStatScanPort->statUCycRec->ptaskNode->cycSeconds < miniCyc)
		{
			gpnLog(GPN_LOG_L_INFO,  "%s : usrCyc(%d)!\n\r",\
				__FUNCTION__, pStatScanPort->statUCycRec->ptaskNode->cycSeconds);
			miniCyc = pStatScanPort->statUCycRec->ptaskNode->cycSeconds;
		}
	}

	if(miniCyc != 0xFFFFFFFF)
	{
		/*divistion mini cycSecs : 1/GPN_STAT_DBS_MINI_POLL_CYC_DIVI */
		miniCyc = miniCyc/GPN_STAT_DBS_MINI_POLL_CYC_DIVI + 1;

		if(scanTpPollCyc < miniCyc)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : portPollCyc(%d)!\n\r",\
				__FUNCTION__, scanTpPollCyc);
			miniCyc = scanTpPollCyc;
		}

		pStatScanPort->miniScanCyc = miniCyc;
	}
	else
	{
		pStatScanPort->miniScanCyc = scanTpPollCyc;
	}
	
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : new portPollCyc(%d)!\n\r",\
		__FUNCTION__, pStatScanPort->miniScanCyc);
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatPortMonEnNotifyFuncModu(stStatLocalPortNode *pStatLocalNode, UINT32 en)
{
	UINT32 statTypeNum;
	UINT32 portType;
	stStatTpNode *pstatTpNode;
	stStatTypeDef *pstatType;
	stStatScanTypeDef *pStatScanTypeNode;
	stStatPeerPortNode *pStatPeerNode;

	/*assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pStatScanTypeNode = NULL;
	portType = PortIndex_GetType(pStatLocalNode->localPort.portIndex);
	gpnStatSeekPortType2ScanTypeNode(portType, &pStatScanTypeNode);
	if(pStatScanTypeNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : peer port(%08x-%08x|%08x|%08x|%08x|%08x) portType(%08x) vs statType err!\n\r",\
			__FUNCTION__,\
			pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
			pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
			pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
			portType);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatPeerNode = pStatLocalNode->pPeerNode;
	
	/*stat scan type include stat type, so notify function module base each stat type */
	statTypeNum = 0;
	pstatTpNode = (stStatTpNode *)listFirst(&(pStatScanTypeNode->statTpsOfScanTpQuen));
	while((pstatTpNode != NULL)&&(statTypeNum < pStatScanTypeNode->statTpNumInScanType))
	{
		pstatType = pstatTpNode->pStatTpStr;

		if(pstatType->pStatuNotfyFunc != NULL)
		{
			pstatType->pStatuNotfyFunc(&(pStatPeerNode->peerPort), en);
		}
		
		statTypeNum++;
		pstatTpNode = (stStatTpNode *)listNext((NODE *)(pstatTpNode));
	}

	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatCycClassTemplatSet(stStatXCycDataRecd *statXCycRec,
	UINT32 almThredTpId, UINT32 subReptTpId, UINT32 evnThredTpId, UINT32 subFiltTpId)
{
	stStatAlmThredTpNode *palmThredTpNode;
	stStatSubReportTpNode *psubReportTpNode;
	stStatEvnThredTpNode *pevnThredTpNode;
	stStatSubFiltTpNode *psubFiltTpNode;
	
	/*assert */
	if(statXCycRec == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	palmThredTpNode = NULL;
	gpnStatAlarmThredTpGet(almThredTpId, &palmThredTpNode);
	if(palmThredTpNode != NULL)
	{
		statXCycRec->xCycAlmThId = almThredTpId;
		statXCycRec->xCycAlmThred = (void *)palmThredTpNode->palmThredTp->palmThred;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : almThredTpId(%d) err!\n\r",\
			__FUNCTION__, almThredTpId);
		statXCycRec->xCycAlmThId = GPN_STAT_INVALID_TEMPLAT_ID;
		statXCycRec->xCycAlmThred = NULL;
	}
	
	psubReportTpNode = NULL;
	gpnStatSubReportTpGet(subReptTpId, &psubReportTpNode);
	if(psubReportTpNode != NULL)
	{
		statXCycRec->xCycsubReptId= subReptTpId;
		statXCycRec->subReptInfo= (void *)psubReportTpNode->psubReportTp->psubReport;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : subReptTpId(%d) err!\n\r",\
			__FUNCTION__, subReptTpId);
		statXCycRec->xCycsubReptId = GPN_STAT_INVALID_TEMPLAT_ID;
		statXCycRec->subReptInfo = NULL;
	}
	
	pevnThredTpNode = NULL;
	gpnStatEventThredTpGet(evnThredTpId, &pevnThredTpNode);
	if(pevnThredTpNode != NULL)
	{
		statXCycRec->xCycEvnThId = evnThredTpId;
		statXCycRec->xCycEvnThred = (void *)pevnThredTpNode->pevnThredTp->pevnThred;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : evnThredTpId(%d) err!\n\r",\
			__FUNCTION__, evnThredTpId);
		statXCycRec->xCycEvnThId = GPN_STAT_INVALID_TEMPLAT_ID;
		statXCycRec->xCycEvnThred = NULL;
	}
	
	psubFiltTpNode = NULL;
	gpnStatSubFiltTpGet(subFiltTpId, &psubFiltTpNode);
	if(psubFiltTpNode != NULL)
	{
		statXCycRec->xCycsubFiltId = subFiltTpId;
		statXCycRec->subFiltInfo = (void *)psubFiltTpNode->psubFiltTp->psubFilt;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : subFiltTpId(%d) err!\n\r",\
			__FUNCTION__, subFiltTpId);
		statXCycRec->xCycsubFiltId = GPN_STAT_INVALID_TEMPLAT_ID;
		statXCycRec->subFiltInfo = NULL;
	}
    
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatCycClassEnable(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc,
	UINT32 cycClass, UINT32 almThredTpId, UINT32 subReptTpId, UINT32 evnThredTpId, UINT32 subFiltTpId, INT32 histReport)
{
	UINT32 i;
	stStatScanPortInfo *pStatScanPort;
	
	/* assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanPort = pStatLocalNode->pStatScanPort;
	if(cycClass == GPN_STAT_CYC_CLASS_BASE)
	{
		pStatScanPort->currMonEn = GPN_STAT_DBS_GEN_ENABLE;

		/*notify function module enable port stat mon */
		gpnStatPortMonEnNotifyFuncModu(pStatLocalNode, GPN_STAT_DBS_GEN_ENABLE);
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_LONG)
	{
		if(pStatScanPort->statLCycRec != NULL)
		{
			pStatScanPort->statLCycRec->en = GPN_STAT_DBS_GEN_ENABLE;

			pStatScanPort->statLCycRec->trapEn = histReport;

			gpnStatCycClassTemplatSet(pStatScanPort->statLCycRec,
				almThredTpId, subReptTpId, evnThredTpId, subFiltTpId);
			
			/*add to histQuen*/
			for(i=0;i<GPN_STAT_HIST_LCYC_REC_NUM;i++)
			{
				//printf("cycClass:%d hist data add in mgt:%d\n\r", cycClass, i);
				gpnStatHistDataAddInMgt(pStatScanPort->statLCycRec->pstatXCycHistRecd[i]);
			}
		}
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_SHORT)
	{
		if(pStatScanPort->statSCycRec != NULL)
		{
			pStatScanPort->statSCycRec->en = GPN_STAT_DBS_GEN_ENABLE;

			pStatScanPort->statSCycRec->trapEn = histReport;

			gpnStatCycClassTemplatSet(pStatScanPort->statSCycRec,
				almThredTpId, subReptTpId, evnThredTpId, subFiltTpId);

			/*add to histQuen*/
			for(i=0;i<GPN_STAT_HIST_SCYC_REC_NUM;i++)
			{
				//printf("cycClass:%d hist data add in mgt:%d\n\r", cycClass, i);
				gpnStatHistDataAddInMgt(pStatScanPort->statSCycRec->pstatXCycHistRecd[i]);
			}
		}
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_USR)
	{
		if(pStatScanPort->statUCycRec != NULL)
		{
			pStatScanPort->statUCycRec->en = GPN_STAT_DBS_GEN_ENABLE;

			pStatScanPort->statUCycRec->trapEn = histReport;

			/*printf("\n\r almThredTpId:%d, subReptTpId:%d, evnThredTpId:%d, subFiltTpId:%d \n\r", \
				almThredTpId, subReptTpId, evnThredTpId, subFiltTpId);*/
			
			gpnStatCycClassTemplatSet(pStatScanPort->statUCycRec,
				almThredTpId, subReptTpId, evnThredTpId, subFiltTpId);
		
			/*add to histQuen*/
			for(i=0;i<GPN_STAT_HIST_UCYC_REC_NUM;i++)
			{
				//printf("cycClass:%d hist data add in mgt:%d\n\r", cycClass, i);
				gpnStatHistDataAddInMgt(pStatScanPort->statUCycRec->pstatXCycHistRecd[i]);
			}
		}
	}
	else
	{
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) cycClass(%d) err!\n\r",\
			__FUNCTION__,\
			pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
			pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
			pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
			cycClass);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*renew cyc poll stat data cycSecs */
	if(cycClass != GPN_STAT_CYC_CLASS_BASE)
	{
		gpnStatRenewPortPollCyc(pStatLocalNode, scanTpPollCyc);
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatCycClassEnableRestore(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc, UINT32 cycClass)
{	
	UINT32 reVal;
	stStatScanPortInfo *pStatScanPort;
	
	/* assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pStatScanPort = pStatLocalNode->pStatScanPort;

	if((cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
			(pStatScanPort->statLCycRec != NULL))
	{
		pStatScanPort->statLCycRec->en = GEN_SYS_ENABLE;
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
			(pStatScanPort->statSCycRec != NULL))
	{
		pStatScanPort->statSCycRec->en = GEN_SYS_ENABLE;
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_USR) &&\
			(pStatScanPort->statUCycRec != NULL))
	{
		pStatScanPort->statUCycRec->en = GEN_SYS_ENABLE;
	}
	else
	{
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) cycClass(%d) disable err!\n\r",\
			__FUNCTION__,\
			pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
			pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
			pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
			cycClass);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*calculate new poll cycSecs */
	reVal = gpnStatRenewPortPollCyc(pStatLocalNode, scanTpPollCyc);
	if (reVal == GPN_STAT_DBS_GEN_ERR)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatCycClassDisable(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc, UINT32 cycClass)
{	
	UINT32 reVal;
	stStatScanPortInfo *pStatScanPort;
	
	/* assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pStatScanPort = pStatLocalNode->pStatScanPort;

	if(cycClass == GPN_STAT_CYC_CLASS_BASE)
	{
		pStatScanPort->currMonEn = GPN_STAT_DBS_GEN_DISABLE;
		
		/*notify function module disable port stat mon */
		reVal = gpnStatPortMonEnNotifyFuncModu(pStatLocalNode, GPN_STAT_DBS_GEN_DISABLE);
		if (reVal == GPN_STAT_DBS_GEN_ERR)
		{
			gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) disable modu mon err!\n\r",\
				__FUNCTION__,\
				pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
				pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
				pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6);
			return GPN_STAT_DBS_GEN_ERR;
		}
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
			(pStatScanPort->statLCycRec != NULL))
	{
		pStatScanPort->statLCycRec->en = GPN_STAT_DBS_GEN_DISABLE;
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
			(pStatScanPort->statSCycRec != NULL))
	{
		pStatScanPort->statSCycRec->en = GPN_STAT_DBS_GEN_DISABLE;
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_USR) &&\
			(pStatScanPort->statUCycRec != NULL))
	{
		pStatScanPort->statUCycRec->en = GPN_STAT_DBS_GEN_DISABLE;
	}
	else
	{
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) cycClass(%d) disable err!\n\r",\
			__FUNCTION__,\
			pStatLocalNode->localPort.devIndex, pStatLocalNode->localPort.portIndex,\
			pStatLocalNode->localPort.portIndex3, pStatLocalNode->localPort.portIndex4,\
			pStatLocalNode->localPort.portIndex5, pStatLocalNode->localPort.portIndex6,\
			cycClass);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*calculate new poll cycSecs */
	reVal = gpnStatRenewPortPollCyc(pStatLocalNode, scanTpPollCyc);
	if (reVal == GPN_STAT_DBS_GEN_ERR)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatCycClassDelete(stStatLocalPortNode *pStatLocalNode, UINT32 scanTpPollCyc, UINT32 cycClass)
{
	UINT32 i;
	UINT32 reVal;
	stStatScanPortInfo *pStatScanPort;
	
	/* assert */
	if(pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	reVal = gpnStatCycClassDisable(pStatLocalNode, scanTpPollCyc, cycClass);
	if(reVal == GPN_STAT_DBS_GEN_ERR)
	{
		//return GPN_STAT_DBS_GEN_ERR;
		gpnLog(GPN_LOG_L_INFO, "%s : cycClass(%d) disable return(%d)\n\r",\
			__FUNCTION__, cycClass, reVal);
	}

	pStatScanPort = pStatLocalNode->pStatScanPort;
	if (pStatScanPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if( (cycClass == GPN_STAT_CYC_CLASS_LONG) &&\
		(pStatScanPort->statLCycRec != NULL) )
	{
		/*delete from histQuen*/
		for(i=0;i<GPN_STAT_HIST_LCYC_REC_NUM;i++)
		{
			//printf("cycClass:%d hist data del out mgt:%d\n\r", cycClass, i);
			gpnStatHistDataDelOutMgt(pStatScanPort->statLCycRec->pstatXCycHistRecd[i]->histDataLoc);
		}
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_SHORT) &&\
			(pStatScanPort->statSCycRec != NULL))
	{
		/*delete from histQuen*/
		for(i=0;i<GPN_STAT_HIST_SCYC_REC_NUM;i++)
		{
			//printf("cycClass:%d hist data del out mgt:%d\n\r", cycClass, i);
			gpnStatHistDataDelOutMgt(pStatScanPort->statSCycRec->pstatXCycHistRecd[i]->histDataLoc);
		}
	}
	else if((cycClass == GPN_STAT_CYC_CLASS_USR) &&\
			(pStatScanPort->statUCycRec != NULL))
	{
		/*delete from histQuen*/
		for(i=0;i<GPN_STAT_HIST_UCYC_REC_NUM;i++)
		{
			//printf("cycClass:%d hist data del out mgt:%d\n\r", cycClass, i);
			gpnStatHistDataDelOutMgt(pStatScanPort->statUCycRec->pstatXCycHistRecd[i]->histDataLoc);
		}
	}
	else
	{
		//printf("sycClass:%d hist data del out mgt err\n\r", cycClass);
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

/* how to seek node */
UINT32 gpnStatRelevCheckIsLocalInPerScanQuen(objLogicDesc *pLocalPort, stStatPreScanQuen *pStatPreScanNode)
{
	UINT32 hashValue;
	UINT32 perNodeNum;
	objLogicDesc *pLocalIndex;
	stStatPreScanIndex *pStatPreScanIndex;

	/*assert */
	
	hashValue = ((pLocalPort->portIndex) % (pStatPreScanNode->hashKey));
	
	perNodeNum = 0;
	pStatPreScanIndex = (stStatPreScanIndex *)listFirst(&(pStatPreScanNode->statPreScanQuen[hashValue]));
	while((pStatPreScanIndex != NULL)&&(perNodeNum < pStatPreScanNode->preScanNodeNum[hashValue]))
	{
		pLocalIndex = &(pStatPreScanIndex->pStatLocalPort->localPort);
		if( (pLocalIndex->devIndex == pLocalPort->devIndex) &&\
			(pLocalIndex->portIndex == pLocalPort->portIndex) &&\
			(pLocalIndex->portIndex3 == pLocalPort->portIndex3) &&\
			(pLocalIndex->portIndex4 == pLocalPort->portIndex4) &&\
			(pLocalIndex->portIndex5 == pLocalPort->portIndex5) &&\
			(pLocalIndex->portIndex6 == pLocalPort->portIndex6) )
		{
			return GPN_STAT_DBS_GEN_YES;
		}
		perNodeNum++;
		pStatPreScanIndex = (stStatPreScanIndex *)listNext((NODE *)(pStatPreScanIndex));
	}
	return GPN_STAT_DBS_GEN_NO;
}

UINT32 gpnStatRelevCheckIsPeerInPeerPortQuen(optObjOrient *pPeerPort, stStatPreScanQuen *pStatPreScanNode)
{
	UINT32 hashValue;
	UINT32 peerNodeNum;
	stStatPeerPortNode *pStatPeerNode;
	optObjOrient *pPeerIndex;

	/*assert */
	
	hashValue = ((pPeerPort->portIndex) % (pStatPreScanNode->hashKey));
	
	peerNodeNum = 0;
	pStatPeerNode = (stStatPeerPortNode *)listFirst(&(pStatPreScanNode->statPeerPortQuen[hashValue]));
	while((pStatPeerNode != NULL)&&(peerNodeNum < pStatPreScanNode->peerPortNodeNum[hashValue]))
	{
		pPeerIndex = &(pStatPeerNode->peerPort);
		if( (pPeerIndex->devIndex == pPeerPort->devIndex) &&\
			(pPeerIndex->portIndex == pPeerPort->portIndex) &&\
			(pPeerIndex->portIndex3 == pPeerPort->portIndex3) &&\
			(pPeerIndex->portIndex4 == pPeerPort->portIndex4) &&\
			(pPeerIndex->portIndex5 == pPeerPort->portIndex5) &&\
			(pPeerIndex->portIndex6 == pPeerPort->portIndex6) )
		{
			return GPN_STAT_DBS_GEN_YES;
		}
		peerNodeNum++;
		pStatPeerNode = (stStatPeerPortNode *)listNext((NODE *)(pStatPeerNode));
	}
	return GPN_STAT_DBS_GEN_NO;
}

UINT32 gpnStatSeekLocalPort2PreScanNodeIndex(objLogicDesc *pLocalPort, stStatPreScanIndex **ppStatPreScanIndex)
{
	UINT32 slotId;
	UINT32 portType;
	UINT32 hashValue;
	UINT32 sysPortTypeNum;
	objLogicDesc *pPortIndex;
	stStatPreScanQuen *pslotPerScanQuen;
	stPortTpToStatScanTp *pPortTpToScanTp;
	UINT32 perProtNum;
	stStatPreScanIndex *pStatPreScanIndex;

	/*assert */
	if( (pLocalPort == NULL) ||\
		(ppStatPreScanIndex == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* find statScanType node */
	portType = PortIndex_GetType(pLocalPort->portIndex);
	
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(
		&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));
	while(  (pPortTpToScanTp != NULL) &&\
			(sysPortTypeNum < pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum))
	{
		if(portType == pPortTpToScanTp->portType)
		{
			slotId = PortIndex_GetSlot(pLocalPort->portIndex);
			pslotPerScanQuen = pPortTpToScanTp->pStatPreScanNode[slotId];
			hashValue = pLocalPort->portIndex % pslotPerScanQuen->hashKey;
			
			perProtNum = 0;
			pStatPreScanIndex = (stStatPreScanIndex *)listFirst(&(pslotPerScanQuen->statPreScanQuen[hashValue]));
			while((pStatPreScanIndex != NULL) && (perProtNum < pslotPerScanQuen->preScanNodeNum[hashValue]))
			{
				pPortIndex = &(pStatPreScanIndex->pStatLocalPort->localPort);
				if( (pLocalPort->devIndex == pPortIndex->devIndex) &&\
					(pLocalPort->portIndex == pPortIndex->portIndex) &&\
					(pLocalPort->portIndex3 == pPortIndex->portIndex3) &&\
					(pLocalPort->portIndex4 == pPortIndex->portIndex4) &&\
					(pLocalPort->portIndex5 == pPortIndex->portIndex5) &&\
					(pLocalPort->portIndex6 == pPortIndex->portIndex6) )
				{
					*ppStatPreScanIndex = pStatPreScanIndex;
					return GPN_STAT_DBS_GEN_OK;
				}
				perProtNum++;
				pStatPreScanIndex = (stStatPreScanIndex *)listNext((NODE *)(pStatPreScanIndex));
			}
		}
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : perScanQuen don't havelocal port(%08x-%08x|%08x|%08x|%08x|%08x)!\n\r",\
		__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
		pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
	*ppStatPreScanIndex = NULL;
	
	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatSeekLocalPort2LocalPortNode(objLogicDesc *pLocalPort, stStatLocalPortNode **ppStatLocalNode)
{
	UINT32 slotId;
	UINT32 portType;
	UINT32 hashValue;
	UINT32 sysPortTypeNum;
	objLogicDesc *pPortIndex;
	stStatPreScanQuen *pslotPerScanQuen;
	stPortTpToStatScanTp *pPortTpToScanTp;
	UINT32 perProtNum;
	stStatPreScanIndex *pStatPreScanIndex;

	/*assert */
	if( (pLocalPort == NULL) ||\
		(ppStatLocalNode == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* find statScanType node */
	portType = PortIndex_GetType(pLocalPort->portIndex);
	
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(
		&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));
	while(  (pPortTpToScanTp != NULL) &&\
			(sysPortTypeNum < pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum))
	{
		if(portType == pPortTpToScanTp->portType)
		{
			slotId = PortIndex_GetSlot(pLocalPort->portIndex);
			pslotPerScanQuen = pPortTpToScanTp->pStatPreScanNode[slotId];
			hashValue = pLocalPort->portIndex % pslotPerScanQuen->hashKey;
			
			perProtNum = 0;
			pStatPreScanIndex = (stStatPreScanIndex *)listFirst(&(pslotPerScanQuen->statPreScanQuen[hashValue]));
			while((pStatPreScanIndex != NULL) && (perProtNum < pslotPerScanQuen->preScanNodeNum[hashValue]))
			{
				pPortIndex = &(pStatPreScanIndex->pStatLocalPort->localPort);
				if( (pLocalPort->devIndex == pPortIndex->devIndex) &&\
					(pLocalPort->portIndex == pPortIndex->portIndex) &&\
					(pLocalPort->portIndex3 == pPortIndex->portIndex3) &&\
					(pLocalPort->portIndex4 == pPortIndex->portIndex4) &&\
					(pLocalPort->portIndex5 == pPortIndex->portIndex5) &&\
					(pLocalPort->portIndex6 == pPortIndex->portIndex6) )
				{
					*ppStatLocalNode = pStatPreScanIndex->pStatLocalPort;
					return GPN_STAT_DBS_GEN_OK;
				}
				perProtNum++;
				pStatPreScanIndex = (stStatPreScanIndex *)listNext((NODE *)(pStatPreScanIndex));
			}
		}
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : perScanQuen don't havelocal port(%08x-%08x|%08x|%08x|%08x|%08x)!\n\r",\
		__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
		pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
	*ppStatLocalNode = NULL;
	
	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatSeekPeerPort2PeerPortNode(optObjOrient *pPeerPort, stStatPeerPortNode **ppStatPeerNode)
{
	UINT32 slotId;
	UINT32 portType;
	UINT32 hashValue;
	UINT32 sysPortTypeNum;
	optObjOrient *pPortIndex;
	stStatPreScanQuen *pslotPerScanQuen;
	stPortTpToStatScanTp *pPortTpToScanTp;
	UINT32 peerProtNum;
	stStatPeerPortNode *pStatPeerNode;

	/*assert */
	if( (pPeerPort == NULL) ||\
		(ppStatPeerNode == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* find statScanType node */
	portType = PortIndex_GetType(pPeerPort->portIndex);
	
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(
		&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));
	while(  (pPortTpToScanTp != NULL) &&\
			(sysPortTypeNum < pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum))
	{
		if(portType == pPortTpToScanTp->portType)
		{
			slotId = PortIndex_GetSlot(pPeerPort->portIndex);
			pslotPerScanQuen = pPortTpToScanTp->pStatPreScanNode[slotId];
			hashValue = pPeerPort->portIndex % pslotPerScanQuen->hashKey;
			
			peerProtNum = 0;
			pStatPeerNode = (stStatPeerPortNode *)listFirst(&(pslotPerScanQuen->statPeerPortQuen[hashValue]));
			while((pStatPeerNode != NULL) && (peerProtNum < pslotPerScanQuen->peerPortNodeNum[hashValue]))
			{
				pPortIndex = &(pStatPeerNode->peerPort);
				if( (pPeerPort->devIndex == pPortIndex->devIndex) &&\
					(pPeerPort->portIndex == pPortIndex->portIndex) &&\
					(pPeerPort->portIndex3 == pPortIndex->portIndex3) &&\
					(pPeerPort->portIndex4 == pPortIndex->portIndex4) &&\
					(pPeerPort->portIndex5 == pPortIndex->portIndex5) &&\
					(pPeerPort->portIndex6 == pPortIndex->portIndex6) )
				{
					*ppStatPeerNode = pStatPeerNode;
					return GPN_STAT_DBS_GEN_OK;
				}
				peerProtNum++;
				pStatPeerNode = (stStatPeerPortNode *)listNext((NODE *)(pStatPeerNode));
			}
		}
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : perScanQuen don't havelocal peer port(%08x-%08x|%08x|%08x|%08x|%08x)!\n\r",\
		__FUNCTION__, pPeerPort->devIndex, pPeerPort->portIndex, pPeerPort->portIndex3,\
		pPeerPort->portIndex4, pPeerPort->portIndex5, pPeerPort->portIndex6);
	*ppStatPeerNode = NULL;
	
	return GPN_STAT_DBS_GEN_ERR;
}


UINT32 gpnStatSeekFirstPreScanNodeIndex(stStatPreScanIndex **ppStatPreScanIndex)
{
	UINT32 i;
	UINT32 j;
	stStatPreScanQuen *pslotPerScanQuen;
	stPortTpToStatScanTp *pPortTpToScanTp;
	stStatPreScanIndex *pStatPreScanIndex;

	/*assert */
	if(ppStatPreScanIndex == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* find statScanType node */
	pPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(
		&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));

	if(pPortTpToScanTp == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : find pPortTpToScanTp err!\n\r",\
			__FUNCTION__);
		*ppStatPreScanIndex = NULL;
		return GPN_STAT_DBS_GEN_ERR;
	}
	for(i=0;i<(EQU_SLOT_MAX_NUM+1);i++)
	{
		pslotPerScanQuen = pPortTpToScanTp->pStatPreScanNode[i];

		for(j=0;j<GPN_STAT_PRESCAN_HASH;j++)
		{
			pStatPreScanIndex = (stStatPreScanIndex *)listFirst(
				&(pslotPerScanQuen->statPreScanQuen[j]));
			if(pStatPreScanIndex != NULL)
			{
				*ppStatPreScanIndex = pStatPreScanIndex;
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : find first scanPortNode(%08x)!\n\r",\
					__FUNCTION__, (UINT32)pStatPreScanIndex);
				return GPN_STAT_DBS_GEN_OK;
			}
		}
	}

	*ppStatPreScanIndex = NULL;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : find first scanPortNode err!\n\r",\
		__FUNCTION__);
	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatSeekNextPreScanNodeIndex(stStatPreScanIndex *pStatPreScanIndex, stStatPreScanIndex **ppStatPreScanIndex)
{
	stPortTpToStatScanTp *pPortTpToScanTp;
	stStatPreScanQuen *pslotPerScanQuen;
	objLogicDesc *pPortInfo;
	UINT32 sysPortTypeNum;
	UINT32 portType;
	UINT32 hashValue;
	UINT32 slotId;
	UINT32 i;
	UINT32 j;

	/*assert */
	if( (pStatPreScanIndex == NULL) ||\
		(ppStatPreScanIndex == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*init out para */
	*ppStatPreScanIndex = NULL;

	pPortInfo = &(pStatPreScanIndex->pStatLocalPort->localPort);
	/* find statScanType node */
	portType = PortIndex_GetType(pPortInfo->portIndex);
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(
		&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));
	while(	(pPortTpToScanTp != NULL) &&\
			(sysPortTypeNum < pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum))
	{
		if(portType == pPortTpToScanTp->portType)
		{
			slotId = PortIndex_GetSlot(pPortInfo->portIndex);
			pslotPerScanQuen = pPortTpToScanTp->pStatPreScanNode[slotId];
			hashValue = pPortInfo->portIndex % pslotPerScanQuen->hashKey;

			/*find next node */
			pStatPreScanIndex = (stStatPreScanIndex *)listNext((NODE *)(pStatPreScanIndex));
			if(pStatPreScanIndex != NULL)
			{
				*ppStatPreScanIndex = pStatPreScanIndex;
				return GPN_STAT_DBS_GEN_OK;
			}
			else
			{
				for(i=slotId;i<(EQU_SLOT_MAX_NUM+1);i++)
				{
					pslotPerScanQuen = pPortTpToScanTp->pStatPreScanNode[i];
					for(j=(hashValue+1);j<GPN_STAT_PRESCAN_HASH;j++)
					{
						pStatPreScanIndex = (stStatPreScanIndex *)listFirst(&(pslotPerScanQuen->statPreScanQuen[j]));
						if(pStatPreScanIndex != NULL)
						{
							*ppStatPreScanIndex = pStatPreScanIndex;
							return GPN_STAT_DBS_GEN_OK;
						}
					}
				}
			}

			/*find suit portType, and do proc, then break */
			break;
		}
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}

	/*find next in other portType */
	sysPortTypeNum++;
	pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
	while( 	(pPortTpToScanTp != NULL) &&\
			(sysPortTypeNum < pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum))
	{
		for(i=0;i<(EQU_SLOT_MAX_NUM+1);i++)
		{
			pslotPerScanQuen = pPortTpToScanTp->pStatPreScanNode[i];
			for(j=0;j<GPN_STAT_PRESCAN_HASH;j++)
			{
				pStatPreScanIndex = (stStatPreScanIndex *)listFirst(&(pslotPerScanQuen->statPreScanQuen[j]));
				if(pStatPreScanIndex != NULL)
				{
					*ppStatPreScanIndex = pStatPreScanIndex;
					return GPN_STAT_DBS_GEN_OK;
				}
			}
		}
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}

	*ppStatPreScanIndex = NULL;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : find next scanPortNode err!\n\r",\
		__FUNCTION__);
	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatSeekFirstValidScanNodeIndex(stStatLocalPortNode **ppStatLocalNode)
{
	stStatScanTypeDef *pStatScanTpNode;
	stStatLocalPortNode *pStatLocalNode;

	/*assert */
	if(ppStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanTpNode = NULL;
	gpnStatTypeGetFirstScanTypeNode(&pStatScanTpNode);
	while(pStatScanTpNode != NULL)
	{
		pStatLocalNode = (stStatLocalPortNode *)listFirst(&(pStatScanTpNode->statScanPortObjQuen));
		if(pStatLocalNode != NULL)
		{
			*ppStatLocalNode = pStatLocalNode;
			return GPN_STAT_DBS_GEN_OK;
		}
		
		gpnStatTypeGetNextScanTypeNode(pStatScanTpNode, &pStatScanTpNode);
	}

	*ppStatLocalNode = NULL;
	return GPN_STAT_TYPE_ERR;
}
UINT32 gpnStatSeekNextValidScanNodeIndex(stStatLocalPortNode *pStatLocalNode, stStatLocalPortNode **ppStatLocalNode)
{
	stStatScanTypeDef *pStatScanTpNode;
	stStatLocalPortNode *pStatLocalNodeSave;

	/*assert */
	if( (pStatLocalNode == NULL) ||\
		(ppStatLocalNode == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*if next is NULL, we can get last */
	pStatLocalNodeSave = pStatLocalNode;
	pStatLocalNode = (stStatLocalPortNode *)listNext((NODE *)pStatLocalNode);
	if(pStatLocalNode != NULL)
	{
		*ppStatLocalNode = pStatLocalNode;
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{		
		pStatScanTpNode = pStatLocalNodeSave->pStatScanPort->pscanTypeNode;
		//pStatScanTpNode = NULL;
		gpnStatTypeGetNextScanTypeNode(pStatScanTpNode, &pStatScanTpNode);
		while(pStatScanTpNode != NULL)
		{
			pStatLocalNode = (stStatLocalPortNode *)listFirst(&(pStatScanTpNode->statScanPortObjQuen));
			if(pStatLocalNode != NULL)
			{
				*ppStatLocalNode = pStatLocalNode;
				return GPN_STAT_DBS_GEN_OK;
			}
		
			gpnStatTypeGetNextScanTypeNode(pStatScanTpNode, &pStatScanTpNode);
		}
	}

	*ppStatLocalNode = NULL;
	return GPN_STAT_TYPE_ERR;
}


UINT32 gpnStatSeekFirstSubTypeInPortScanNode(stStatLocalPortNode *pStatLocalNode, UINT32 *psubType)
{
	stStatScanPortInfo *pStatScanPort;
	stStatSTCharacterDef *subTpNode;

	/*assert */
	if( (pStatLocalNode == NULL) ||\
		(psubType == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanPort = pStatLocalNode->pStatScanPort;
	subTpNode = (stStatSTCharacterDef *)listFirst(&(pStatScanPort->pscanTypeNode->statScanSubTCharaQuen));
	if(subTpNode != NULL)
	{
		*psubType = subTpNode->statSubType;
		return GPN_STAT_TYPE_OK;
	}
	else
	{
		*psubType = GPN_STAT_SUB_TYPE_INVALID;
		return GPN_STAT_TYPE_ERR;
	}
	
	
}
UINT32 gpnStatSeekNextSubTypeInPortScanNode(stStatLocalPortNode *pStatLocalNode, UINT32 subType, UINT32  *psubType)
{
	stStatScanPortInfo *pStatScanPort;
	stStatScanTypeDef *pscanTypeNode;
	stStatSTCharacterDef *subTpNode;
	UINT32 subNum;

	/*assert */
	if( (pStatLocalNode == NULL) ||\
		(psubType == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanPort = pStatLocalNode->pStatScanPort;
	pscanTypeNode = pStatScanPort->pscanTypeNode;

	subNum = 0;
	subTpNode = (stStatSTCharacterDef *)listFirst(&(pscanTypeNode->statScanSubTCharaQuen));
	while((subTpNode!=NULL)&&(subNum<pscanTypeNode->statSubTpNumInScanType))
	{
		if(subTpNode->statSubType == subType)
		{
			break;
		}

		subNum++;
		subTpNode = (stStatSTCharacterDef *)listNext((NODE *)subTpNode);
	}
	if((subTpNode==NULL)||(subNum>=pscanTypeNode->statSubTpNumInScanType))
	{
		*psubType = GPN_STAT_SUB_TYPE_INVALID;
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	subTpNode = (stStatSTCharacterDef *)listNext((NODE *)subTpNode);
	if(subTpNode != NULL)
	{
		*psubType = subTpNode->statSubType;
		return GPN_STAT_TYPE_OK;
	}
	else
	{
		*psubType = GPN_STAT_SUB_TYPE_INVALID;
		return GPN_STAT_TYPE_ERR;
	}
}

UINT32 gpnStatSeekPortType2ScanTypeNode(UINT32 portType, stStatScanTypeDef **ppStatScanTypeNode)
{
	UINT32 sysPortTypeNum;
	stPortTpToStatScanTp *pPortTpToScanTp;

	/*assert */
	if(ppStatScanTypeNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(
		&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));
	while(  (pPortTpToScanTp != NULL) &&\
			(sysPortTypeNum < pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum))
	{
		if(portType == pPortTpToScanTp->portType)
		{
			*ppStatScanTypeNode = pPortTpToScanTp->pStatPreScanNode[0]->statScanTypeNode;
			return GPN_STAT_DBS_GEN_ERR;
		}
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : portType(%0x8) can't find scanTypeNode!\n\r",\
		__FUNCTION__, portType);
	*ppStatScanTypeNode = NULL;
	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatSeekFirstPortType2PTVsST(stPortTpToStatScanTp **ppPortTpToScanTp)
{
	/*assert */
	if(ppPortTpToScanTp == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	*ppPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(
		&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : portType(%08x) to scanType(%08x)!\n\r",\
		__FUNCTION__, (*ppPortTpToScanTp)->portType, (*ppPortTpToScanTp)->statScanType);

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatSeekPortType2PTVsST(UINT32 portType, stPortTpToStatScanTp **ppPortTpToScanTp)
{
	UINT32 sysPortTypeNum;
	stPortTpToStatScanTp *pPortTpToScanTp;

	/*assert */
	if(ppPortTpToScanTp == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(
		&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));
	while(  (pPortTpToScanTp != NULL) &&\
			(sysPortTypeNum < pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum))
	{
		if(portType == pPortTpToScanTp->portType)
		{
			*ppPortTpToScanTp = pPortTpToScanTp;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : portType(%08x) to scanType(%08x)!\n\r",\
				__FUNCTION__, (*ppPortTpToScanTp)->portType, (*ppPortTpToScanTp)->statScanType);
			return GPN_STAT_DBS_GEN_OK;
		}
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : portType(%0x8) can't find scanTypeNode!\n\r",\
		__FUNCTION__, portType);
	*ppPortTpToScanTp = NULL;
	return GPN_STAT_DBS_GEN_ERR;
}


UINT32 gpnStatDataStructMonAdd(objLogicDesc *pLocalPort)
{
	UINT32 reVal;
	stStatLocalPortNode *pStatLocalNode;

	/*assert */
	if(pLocalPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/* 
		search port in perQuen, if not exist, 
		means IFM not have this portIndex,
		could not add in stat mon
	*/
	pStatLocalNode = NULL;
	gpnStatSeekLocalPort2LocalPortNode(pLocalPort, &pStatLocalNode);
	if(pStatLocalNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) not in preScan quen, err!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* 
		add localPort to validScanQuen 
	*/
	reVal = gpnStatNodeRelevChg2ValidScanQuen(pStatLocalNode, GPN_STAT_DBS_GEN_ENABLE);
	if (reVal == GPN_STAT_DBS_GEN_ERR)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) monitor regist err!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}

	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatDataStructMonDelete(objLogicDesc *pLocalPort)
{
	UINT32 reVal;
	stStatLocalPortNode *pStatLocalNode;

	/*assert */
	if(pLocalPort == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/* 
		search port in perQuen, if not exist, 
		means IFM not have this portIndex,
		should not delete in stat mon
	*/
	pStatLocalNode = NULL;
	gpnStatSeekLocalPort2LocalPortNode(pLocalPort, &pStatLocalNode);
	if(pStatLocalNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) not in preScan quen, err!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/* 
		delete localPort to validScanQuen 
	*/
	reVal = gpnStatNodeRelevChg2ValidScanQuen(pStatLocalNode, GPN_STAT_DBS_GEN_DISABLE);
	if (reVal == GPN_STAT_DBS_GEN_ERR)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : local port(%08x-%08x|%08x|%08x|%08x|%08x) monitor unregist err!\n\r",\
			__FUNCTION__, pLocalPort->devIndex, pLocalPort->portIndex, pLocalPort->portIndex3,\
			pLocalPort->portIndex4, pLocalPort->portIndex5, pLocalPort->portIndex6);
		return GPN_STAT_DBS_GEN_ERR;
	}

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatTaskAdd(stStatTaskNode *ptaskNode)
{
	UINT32 hash;
	UINT32 taskNum;
	stStatTaskNode *pTempTaskNode;
	stStatTaskInfo *pstatTaskInfo;
	
	/*assert */
	if(ptaskNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	if( (ptaskNode->taskId == 0) ||\
		(ptaskNode->taskId == GPN_STAT_INVALID_TASK_ID) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : taskId(%d) err!\n\r",\
			__FUNCTION__, ptaskNode->taskId);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pstatTaskInfo = &(pgstEQUStatProcSpace->statTaskInfo);
	
	hash = (ptaskNode->taskId)%(pstatTaskInfo->hash);
	
	taskNum = 0;
	pTempTaskNode = (stStatTaskNode *)listFirst(&(pstatTaskInfo->actTaskQuen[hash]));
	while((pTempTaskNode!=NULL)&&(taskNum<pstatTaskInfo->actTaskNum[hash]))
	{
		if(pTempTaskNode->taskId == ptaskNode->taskId)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : taskId(%d) already creat!\n\r",\
				__FUNCTION__, ptaskNode->taskId);
			return GPN_STAT_DBS_GEN_ERR;
		}

		taskNum++;
		pTempTaskNode = (stStatTaskNode *)listNext((NODE *)pTempTaskNode);
	}

	if(pstatTaskInfo->idleTaskNum == 0)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : do not have idleNode(%d)!\n\r",\
			__FUNCTION__, pstatTaskInfo->idleTaskNum);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*get idle task node base taskId (taskId init when node add in idleTaskQuen) */
	taskNum = 0;
	pTempTaskNode = (stStatTaskNode *)listFirst(&(pstatTaskInfo->idleTaskQuen));
	while((pTempTaskNode!=NULL)&&(taskNum<pstatTaskInfo->idleTaskNum))
	{
		if(pTempTaskNode->taskId == ptaskNode->taskId)
		{
			/*copy task info to new node */
			/*pTempTaskNode->taskId = ptaskNode->taskId;*/

			pTempTaskNode->statScanTp = ptaskNode->statScanTp;
			
			pTempTaskNode->cycClass = ptaskNode->cycClass;
			pTempTaskNode->cycSeconds = ptaskNode->cycSeconds;
			
			pTempTaskNode->monStartTime = ptaskNode->monStartTime;
			pTempTaskNode->monEndTime = ptaskNode->monEndTime;
			
			pTempTaskNode->almThredTpId = ptaskNode->almThredTpId;
			pTempTaskNode->subReportTpId = ptaskNode->subReportTpId;
			/*pTempTaskNode->evnThredTpId = ptaskNode->evnThredTpId;*/
			/*pTempTaskNode->subFiltTpId = ptaskNode->subFiltTpId;*/
			
			pTempTaskNode->cycStart = 0;
			pTempTaskNode->cycBingo = 0;
			pTempTaskNode->thisPush = 0;
			/*init task cycBingo and thisPush */
			gpnStatHistPushBingoTimeInit(ptaskNode->monStartTime, ptaskNode->cycSeconds,\
				&(pTempTaskNode->cycStart), &(pTempTaskNode->thisPush));
			pTempTaskNode->cycBingo = pTempTaskNode->cycStart + ptaskNode->cycSeconds;
			
			listInit(&(pTempTaskNode->actMOnOBjQuen));
			pTempTaskNode->actMOnOBjNum = 0;
			pTempTaskNode->maxPortInTask = ptaskNode->maxPortInTask;
			
			/*DA get taskId when creat needed, so delelte valid idle node from idlelist */
			listDelete(&(pstatTaskInfo->idleTaskQuen), (NODE *)pTempTaskNode);
			pstatTaskInfo->idleTaskNum--;
			/*add new task in list */
			listAdd(&(pstatTaskInfo->actTaskQuen[hash]), (NODE *)pTempTaskNode);
			pstatTaskInfo->actTaskNum[hash]++;
			
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : add task(%d)!\n\r",\
				__FUNCTION__, pTempTaskNode->taskId);
			
			return GPN_STAT_DBS_GEN_OK;
		}

		taskNum++;
		pTempTaskNode = (stStatTaskNode *)listNext((NODE *)pTempTaskNode);
	}
	
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : don't find task(%d) in idle list!\n\r",\
		__FUNCTION__, ptaskNode->taskId);
	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatTaskDelete(UINT32 taskId)
{
	UINT32 hash;
	UINT32 taskNum;
	stStatTaskNode *ptaskNode;
	stStatTaskInfo *pstatTaskInfo;

	pstatTaskInfo = &(pgstEQUStatProcSpace->statTaskInfo);
	
	hash = taskId%(pstatTaskInfo->hash);

	taskNum = 0;
	ptaskNode = (stStatTaskNode *)listFirst(&(pstatTaskInfo->actTaskQuen[hash]));
	while((ptaskNode!=NULL)&&(taskNum<pstatTaskInfo->actTaskNum[hash]))
	{
		if(taskId == ptaskNode->taskId)
		{
			if(ptaskNode->actMOnOBjNum != 0)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not delete taskId(%d), still have monObj(%d)!\n\r",\
					__FUNCTION__, ptaskNode->taskId, ptaskNode->actMOnOBjNum);

				return GPN_STAT_DBS_GEN_ERR;
			}
			else
			{
				/*delete task, first delete port */
				listDelete(&(pstatTaskInfo->actTaskQuen[hash]), (NODE *)ptaskNode);
				pstatTaskInfo->actTaskNum[hash]--;

				listAdd(&(pstatTaskInfo->idleTaskQuen), (NODE *)ptaskNode);
				pstatTaskInfo->idleTaskNum++;
				
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : delete taskId(%d)!\n\r",\
					__FUNCTION__, ptaskNode->taskId);

				return GPN_STAT_DBS_GEN_OK;
			}
		}

		taskNum++;
		ptaskNode = (stStatTaskNode *)listNext((NODE *)ptaskNode);
	}

	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatTaskModify(stStatTaskNode *pnewTaskNode)
{
	stStatTaskNode *poldTaskNode;
	
	/*assert */
	if(pnewTaskNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	poldTaskNode = NULL;
	gpnStatTaskGet(pnewTaskNode->taskId, &poldTaskNode);
	if(poldTaskNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	if(poldTaskNode->cycSeconds != pnewTaskNode->cycSeconds)
	{
		poldTaskNode->cycSeconds = pnewTaskNode->cycSeconds;

		/*proc*/
	}

	if(poldTaskNode->monStartTime != pnewTaskNode->monStartTime)
	{
		poldTaskNode->monStartTime = pnewTaskNode->monStartTime;

		/*proc*/
	}

	if(poldTaskNode->monEndTime != pnewTaskNode->monEndTime)
	{
		poldTaskNode->monEndTime = pnewTaskNode->monEndTime;

		/*proc*/
	}

	if(poldTaskNode->almThredTpId != pnewTaskNode->almThredTpId)
	{
		poldTaskNode->almThredTpId = pnewTaskNode->almThredTpId;

		/*proc*/
	}

	if(poldTaskNode->subReportTpId != pnewTaskNode->subReportTpId)
	{
		poldTaskNode->subReportTpId = pnewTaskNode->subReportTpId;

		/*proc*/
	}
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatTaskGet(UINT32 taskId, stStatTaskNode **pptaskNode)
{
	UINT32 hash;
	UINT32 taskNum;
	stStatTaskNode *ptempTaskNode;
	stStatTaskInfo *pstatTaskInfo;

	if(pptaskNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pstatTaskInfo = &(pgstEQUStatProcSpace->statTaskInfo);
	
	hash = taskId%(pstatTaskInfo->hash);

	taskNum = 0;
	ptempTaskNode = (stStatTaskNode *)listFirst(&(pstatTaskInfo->actTaskQuen[hash]));
	while((ptempTaskNode!=NULL)&&(taskNum<pstatTaskInfo->actTaskNum[hash]))
	{
		if(taskId == ptempTaskNode->taskId)
		{
			/*bingo */
			*pptaskNode = ptempTaskNode;
			
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get taskId(%d)!\n\r",\
				__FUNCTION__, taskId);

			return GPN_STAT_DBS_GEN_OK;
		}

		taskNum++;
		ptempTaskNode = (stStatTaskNode *)listNext((NODE *)ptempTaskNode);
	}

	*pptaskNode = NULL;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find taskId(%d)!\n\r",\
		__FUNCTION__, taskId);
	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatTaskGetNext(UINT32 taskId, stStatTaskNode **pptaskNode, UINT32 *pnextTask)
{
	UINT32 hash;
	UINT32 taskNum;
	UINT32 tempTaskId;
	stStatTaskNode *ptempTaskNode;
	stStatTaskInfo *pstatTaskInfo;

	/*assert */
	if( (pptaskNode == NULL) ||\
		(pnextTask == NULL) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pstatTaskInfo = &(pgstEQUStatProcSpace->statTaskInfo);

	tempTaskId = taskId;
	if(tempTaskId == GPN_STAT_INVALID_TASK_ID)
	{
		/*0xFFFFFFFF(-1) means task search start, so re-give a search start : find first task node */
		hash = 0;
		while(hash < GPN_STAT_SEARCH_HASH)
		{
			taskNum = 0;
			ptempTaskNode = NULL;
			ptempTaskNode = (stStatTaskNode *)listFirst(&(pstatTaskInfo->actTaskQuen[hash]));
			if(ptempTaskNode != NULL)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get first taskId(%d)!\n\r",\
					__FUNCTION__, ptempTaskNode->taskId);
				*pptaskNode = ptempTaskNode;
				tempTaskId = ptempTaskNode->taskId;
				
				break;
			}

			/*to next quen */
			hash++;
		}
		if(hash >= GPN_STAT_SEARCH_HASH)
		{
			*pptaskNode = NULL;
			*pnextTask = GPN_STAT_INVALID_TASK_ID;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find first taskId!\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*first is invalid, so next is 'valid first' */
		*pnextTask = tempTaskId;
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next taskId(%d)!\n\r",\
			__FUNCTION__, *pnextTask);
		
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
		hash = taskId%(pstatTaskInfo->hash);
		
		taskNum = 0;
		ptempTaskNode = (stStatTaskNode *)listFirst(&(pstatTaskInfo->actTaskQuen[hash]));
		while((ptempTaskNode!=NULL)&&(taskNum<pstatTaskInfo->actTaskNum[hash]))
		{
			if(taskId == ptempTaskNode->taskId)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get now taskId(%d)!\n\r",\
					__FUNCTION__, taskId);
				*pptaskNode = ptempTaskNode;
			
				break;
			}

			taskNum++;
			ptempTaskNode = (stStatTaskNode *)listNext((NODE *)ptempTaskNode);
		}
		if((ptempTaskNode==NULL)||(taskNum>=pstatTaskInfo->actTaskNum[hash]))
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find taskId(%d)!\n\r",\
			__FUNCTION__, taskId);
			*pptaskNode = NULL;

			return GPN_STAT_DBS_GEN_OK;
		}

		/*fast go to find next node, search in old hash task quen*/
		if(ptempTaskNode != NULL)
		{
			taskNum++;
			ptempTaskNode = (stStatTaskNode *)listNext((NODE *)ptempTaskNode);
			if((ptempTaskNode!=NULL)&&(taskNum<pstatTaskInfo->actTaskNum[hash]))
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d) in old quen!\n\r",\
					__FUNCTION__, ptempTaskNode->taskId);
				*pnextTask = ptempTaskNode->taskId;
					
				return GPN_STAT_DBS_GEN_OK;
			}
		}
			
		/*fast go to find next node, search in all-other hash thredTp quen*/
		hash++;
		while(hash < GPN_STAT_SEARCH_HASH)
		{
			ptempTaskNode = (stStatTaskNode *)listFirst(&(pstatTaskInfo->actTaskQuen[hash]));
			if(ptempTaskNode != NULL)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d) in new quen!\n\r",\
					__FUNCTION__, ptempTaskNode->taskId);
				*pnextTask = ptempTaskNode->taskId;
				
				return GPN_STAT_DBS_GEN_OK;
			}
				
			/*start in new quen */
			hash++;
		}
	}
		
	*pnextTask = GPN_STAT_INVALID_TEMPLAT_ID;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not get next taskId!\n\r",\
		__FUNCTION__);
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatTaskGetNext_2(UINT32 taskId, stStatTaskNode **pptaskNode)
{
	UINT32 hash;
	UINT32 taskNum;
	stStatTaskNode *ptempTaskNode;
	stStatTaskInfo *pstatTaskInfo;

	/*assert */
	if(pptaskNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pstatTaskInfo = &(pgstEQUStatProcSpace->statTaskInfo);

	if(taskId == GPN_STAT_INVALID_TASK_ID)
	{
		/*0xFFFFFFFF(-1) means task search start, so re-give a search start : find first task node */
		hash = 0;
		while(hash < GPN_STAT_SEARCH_HASH)
		{
			taskNum = 0;
			ptempTaskNode = NULL;
			ptempTaskNode = (stStatTaskNode *)listFirst(&(pstatTaskInfo->actTaskQuen[hash]));
			if(ptempTaskNode != NULL)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get first taskId(%d)!\n\r",\
					__FUNCTION__, ptempTaskNode->taskId);
				*pptaskNode = ptempTaskNode;
				
				break;
			}

			/*to next quen */
			hash++;
		}
		if(hash >= GPN_STAT_SEARCH_HASH)
		{
			*pptaskNode = NULL;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find first taskId!\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*first is invalid, so next is 'valid first' */
		
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
		hash = taskId%(pstatTaskInfo->hash);
		
		taskNum = 0;
		ptempTaskNode = (stStatTaskNode *)listFirst(&(pstatTaskInfo->actTaskQuen[hash]));
		while((ptempTaskNode!=NULL)&&(taskNum<pstatTaskInfo->actTaskNum[hash]))
		{
			if(taskId == ptempTaskNode->taskId)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get now taskId(%d)!\n\r",\
					__FUNCTION__, taskId);
				*pptaskNode = (stStatTaskNode *)listNext((NODE *)ptempTaskNode);
			
				break;
			}

			taskNum++;
			ptempTaskNode = (stStatTaskNode *)listNext((NODE *)ptempTaskNode);
		}
	}
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not get next taskId!\n\r",\
		__FUNCTION__);
	return GPN_STAT_DBS_GEN_OK;
}


UINT32 gpnStatEventThredTpAdd(stStatEvnThredTpTable *pthredTpLine)
{
	UINT32 i;
	UINT32 hash;
	UINT32 order;
	UINT32 addMode;
	UINT32 subTpNum;
	stStatEvnThredTpNode *pevnThredTpNode;
	stStatThredTemp *pstatThredTpInfo;
	stStatScanTypeDef *pStatScanTpNode;
	evnThredTpUnit *pevnThredTpUnit;
	
	/*assert */
	if(pthredTpLine == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	/*assert subReportId */
	if( (pthredTpLine->thredTpId == 0)  ||\
		(pthredTpLine->thredTpId == GPN_STAT_INVALID_TEMPLAT_ID) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : thredTpId(%d) err!\n\r",\
			__FUNCTION__, pthredTpLine->thredTpId);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pstatThredTpInfo = &(pgstEQUStatProcSpace->statEventThredTemp);
	hash = (pthredTpLine->thredTpId)%(pstatThredTpInfo->hash);

	/*fix add mode */
	addMode = GPN_STAT_DBS_ADD_NEW_LINE;
	subTpNum = 0;
	pevnThredTpNode = (stStatEvnThredTpNode *)listFirst(&(pstatThredTpInfo->actThredTpQuen[hash]));
	while((pevnThredTpNode!=NULL)&&(subTpNum<pstatThredTpInfo->actThredTpNum[hash]))
	{
		if(pevnThredTpNode->thredTpId == pthredTpLine->thredTpId)
		{	
			/*already add lin, find subType */
			if(gpnStatEventThredTpSubTypeQuery(pevnThredTpNode,\
				pthredTpLine->subType, NULL) == GPN_STAT_DBS_GEN_OK)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : thredTpId(%d) subType(%08x) already creat!\n\r",\
					__FUNCTION__, pthredTpLine->thredTpId, pthredTpLine->subType);
			
				return GPN_STAT_DBS_GEN_ERR;
			}
			else
			{
				addMode = GPN_STAT_DBS_ADD_NEW_UNIT;
			}

			break;
		}

		subTpNum++;
		pevnThredTpNode = (stStatEvnThredTpNode *)listNext((NODE *)pevnThredTpNode);
	}
	if((pevnThredTpNode==NULL)||(subTpNum>=pstatThredTpInfo->actThredTpNum[hash]))
	{
		addMode = GPN_STAT_DBS_ADD_NEW_LINE;
	}
	
	/*add new node */
	if(addMode == GPN_STAT_DBS_ADD_NEW_LINE)
	{
		/*check resouce ok */
		if(pstatThredTpInfo->idleThredTpNum== 0)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : do not have idleNode(%d)!\n\r",\
				__FUNCTION__, pstatThredTpInfo->idleThredTpNum);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*find right idle node */
		subTpNum = 0;
		pevnThredTpNode = (stStatEvnThredTpNode *)listFirst(&(pstatThredTpInfo->idleThredTpQuen));
		while((pevnThredTpNode!=NULL)&&(subTpNum<pstatThredTpInfo->idleThredTpNum))
		{
			if(pevnThredTpNode->thredTpId == pthredTpLine->thredTpId)
			{
				break;
			}
			
			subTpNum++;
			pevnThredTpNode = (stStatEvnThredTpNode *)listNext((NODE *)pevnThredTpNode);
		}
		if((pevnThredTpNode==NULL)||(subTpNum>=pstatThredTpInfo->idleThredTpNum))
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : don't find thredTpId(%d) in idle list!\n\r",\
				__FUNCTION__, pthredTpLine->thredTpId);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*init memrey : calculate space size */
		pStatScanTpNode = NULL;
		gpnStatTypeGetScanTypeNode(pthredTpLine->scanType, &pStatScanTpNode);
		if(pStatScanTpNode == NULL)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : get scanType(%08x) node err!\n\r",\
				__FUNCTION__, pthredTpLine->scanType);
			return GPN_STAT_DBS_GEN_ERR;
		}

		subTpNum = pStatScanTpNode->statSubTpNumInScanType;
		i = sizeof(evnThredTp) + sizeof(evnThredTpUnit)*subTpNum;
		pevnThredTpNode->pevnThredTp = (evnThredTp *)malloc(i);
		if(pevnThredTpNode->pevnThredTp == NULL)
		{
			gpnLog(GPN_LOG_L_ERR, "%s : malloc err\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}
		
		/*init memrey : init memrey 'pevnThredTpNode' */
		pevnThredTpNode->scanType = pthredTpLine->scanType;
		/*init memrey : init memrey 'pevnThredTp' */
		pevnThredTpNode->pevnThredTp->subNum = subTpNum;
		pevnThredTpNode->pevnThredTp->pevnThred =\
			(evnThredTpUnit *)(pevnThredTpNode->pevnThredTp + 1);
		pevnThredTpUnit = pevnThredTpNode->pevnThredTp->pevnThred;
		for(i=0;i<subTpNum;i++)
		{
			/*just need set sub type to NULL */ 
			pevnThredTpUnit[i].subType = GPN_STAT_SUB_TYPE_INVALID;
			pevnThredTpUnit[i].upThredH32 = GPN_STAT_DBS_32_FFFF;
			pevnThredTpUnit[i].upThredL32 = GPN_STAT_DBS_32_FFFF;
			pevnThredTpUnit[i].dnThredH32 = GPN_STAT_DBS_32_NULL;
			pevnThredTpUnit[i].dnThredL32 = GPN_STAT_DBS_32_NULL;
		}

		listDelete(&(pstatThredTpInfo->idleThredTpQuen), (NODE *)pevnThredTpNode);
		pstatThredTpInfo->idleThredTpNum--;
			
		listAdd(&(pstatThredTpInfo->actThredTpQuen[hash]), (NODE *)pevnThredTpNode);
		pstatThredTpInfo->actThredTpNum[hash]++;
	}
	else
	{
		/*just already find  'pevnThredTpNode' */
	}

	/*add new unit : find subType's order & add new unit in line */
	pevnThredTpUnit = pevnThredTpNode->pevnThredTp->pevnThred;
	if(gpnStatTypeGetSubTpOrderBaseScanType(pthredTpLine->subType,\
					pthredTpLine->scanType, &order) == GPN_STAT_DBS_GEN_OK)
	{
		pevnThredTpUnit[order].subType = pthredTpLine->subType;
		pevnThredTpUnit[order].upThredH32 = pthredTpLine->upThredH32;
		pevnThredTpUnit[order].upThredL32 = pthredTpLine->upThredL32;
		pevnThredTpUnit[order].dnThredH32 = pthredTpLine->dnThredH32;
		pevnThredTpUnit[order].dnThredL32 = pthredTpLine->dnThredL32;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : scanType(%08x)'s subType(%08x) envThredTp(%d), err order!\n\r",\
			__FUNCTION__, pthredTpLine->scanType, pthredTpLine->subType,\
			pevnThredTpNode->thredTpId);

		return GPN_STAT_DBS_GEN_ERR;
	}

	/*debug print */
	#if  0
	printf("thredTpId %d scanType %08x subTypeNum %d\n\r",\
		pevnThredTpNode->thredTpId, pevnThredTpNode->scanType,\
		pevnThredTpNode->pevnThredTp->subNum);
	for(i=0;i<pevnThredTpNode->pevnThredTp->subNum;i++)
	{
		printf("subType %08x upH %08x upL %08x dnH %08x dnL %08x\n",\
			pevnThredTpNode->pevnThredTp->pevnThred[i].subType,\
			pevnThredTpNode->pevnThredTp->pevnThred[i].upThredH32,\
			pevnThredTpNode->pevnThredTp->pevnThred[i].upThredL32,\
			pevnThredTpNode->pevnThredTp->pevnThred[i].dnThredH32,\
			pevnThredTpNode->pevnThredTp->pevnThred[i].dnThredL32);
	}
	#endif
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : add scanType(%08x)'s subType(%08x) in envThredTp(%d)!\n\r",\
		__FUNCTION__, pthredTpLine->scanType, pthredTpLine->subType,\
		pevnThredTpNode->thredTpId);
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatEventTpDelCheckPortDel(stStatEvnThredTpNode *pthredTpNode)
{
	stStatLocalPortNode *pStatLocalNode, *sStatLocalNode;
	stStatScanPortInfo *pStatScanPort;
	stStatPortMonMgt portMonMgt;

	/*assert */
	if( pthredTpNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatLocalNode = NULL;
	gpnStatSeekFirstValidScanNodeIndex(&pStatLocalNode);
	if (pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	do 
	{
		pStatScanPort = pStatLocalNode->pStatScanPort;
		if (pthredTpNode->scanType == pStatScanPort->statScanType)
		{
			if(gpnStatPortMonGet(pStatLocalNode, pthredTpNode->scanType, &portMonMgt) == GPN_STAT_DBS_GEN_OK)
			{
				if ((pStatScanPort->statLCycRec && pthredTpNode->thredTpId == portMonMgt.longCycEvnThredTpId)
					|| (pStatScanPort->statSCycRec && pthredTpNode->thredTpId == portMonMgt.shortCycEvnThredTpId)
					|| (pStatScanPort->statUCycRec && pthredTpNode->thredTpId == portMonMgt.udCycEvnThredTpId))
				{
					gpnLog(GPN_LOG_L_INFO, "%s : eventThredTpId(%d) is in use in port moni task!\n\r",\
						__FUNCTION__, pthredTpNode->thredTpId);
					return GPN_STAT_DBS_GEN_OK;
				}
			}
		}

		gpnStatSeekNextValidScanNodeIndex(pStatLocalNode, &sStatLocalNode);
		pStatLocalNode = sStatLocalNode;
		
	} while (pStatLocalNode);

	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatEventThredTpDelete(UINT32 thredTpId)
{
	UINT32 hash;
	UINT32 thredTpNum;
	stStatEvnThredTpNode *pthredTpNode;
	stStatThredTemp *pthredTpInfo;

	pthredTpInfo = &(pgstEQUStatProcSpace->statEventThredTemp);
	
	hash = thredTpId%(pthredTpInfo->hash);

	thredTpNum = 0;
	pthredTpNode = (stStatEvnThredTpNode *)listFirst(&(pthredTpInfo->actThredTpQuen[hash]));
	while((pthredTpNode!=NULL)&&(thredTpNum<pthredTpInfo->actThredTpNum[hash]))
	{
		if(thredTpId == pthredTpNode->thredTpId)
		{
			/*delete task, first delete port */
//			if (gpnStatEventTpDelCheckPortDel(pthredTpNode) != GPN_STAT_DBS_GEN_OK)
//			{
				listDelete(&(pthredTpInfo->actThredTpQuen[hash]), (NODE *)pthredTpNode);
				pthredTpInfo->actThredTpNum[hash]--;

				listAdd(&(pthredTpInfo->idleThredTpQuen), (NODE *)pthredTpNode);
				pthredTpInfo->idleThredTpNum++;

				/*memrey free */
				if(pthredTpNode->pevnThredTp != NULL)
				{
					free(pthredTpNode->pevnThredTp);
					pthredTpNode->pevnThredTp = NULL;
				}
				
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : delete eventThredTpId(%d)!\n\r",\
					__FUNCTION__, pthredTpNode->thredTpId);

				return GPN_STAT_DBS_GEN_OK;
//			}
		}

		thredTpNum++;
		pthredTpNode = (stStatEvnThredTpNode *)listNext((NODE *)pthredTpNode);
	}

	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatEventThredTpModify(stStatEvnThredTpTable *pthredTpLine)
{
	UINT32 hash;
	UINT32 order;
	UINT32 modify;
	UINT32 subTpNum;
	stStatEvnThredTpNode *pevnThredTpNode;
	stStatThredTemp *pevnThredTpInfo;
	evnThredTpUnit *pevnThredUnit;
	
	/*assert */
	if(pthredTpLine == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pevnThredTpInfo = &(pgstEQUStatProcSpace->statEventThredTemp);
	hash = (pthredTpLine->thredTpId)%(pevnThredTpInfo->hash);

	/*fix add mode */
	modify = GPN_STAT_DBS_GEN_NO;
	subTpNum = 0;
	pevnThredTpNode = (stStatEvnThredTpNode *)listFirst(&(pevnThredTpInfo->actThredTpQuen[hash]));
	while((pevnThredTpNode!=NULL)&&(subTpNum<pevnThredTpInfo->actThredTpNum[hash]))
	{
		if(pevnThredTpNode->thredTpId == pthredTpLine->thredTpId)
		{	
			/*check modify-able */
			if(gpnStatEventThredTpSubTypeQuery(pevnThredTpNode,
				pthredTpLine->subType, NULL) == GPN_STAT_DBS_GEN_OK)
			{
				modify = GPN_STAT_DBS_GEN_YES;
			}
			
			break;
		}

		subTpNum++;
		pevnThredTpNode = (stStatEvnThredTpNode *)listNext((NODE *)pevnThredTpNode);
	}
	
	/*add new node */
	if(modify == GPN_STAT_DBS_GEN_YES)
	{
		/*find valid sub type, then modify */
		pevnThredUnit = pevnThredTpNode->pevnThredTp->pevnThred;
		if(gpnStatTypeGetSubTpOrderBaseScanType(pthredTpLine->subType,\
			pevnThredTpNode->scanType, &order) == GPN_STAT_DBS_GEN_OK)
		{
			/*pevnThredUnit[order].subType = pthredTpLine->subType;*/
			pevnThredUnit[order].upThredH32 = pthredTpLine->upThredH32;
			pevnThredUnit[order].upThredL32 = pthredTpLine->upThredL32;
			pevnThredUnit[order].dnThredH32 = pthredTpLine->dnThredH32;
			pevnThredUnit[order].dnThredL32 = pthredTpLine->dnThredL32;
			return GPN_STAT_DBS_GEN_OK;
		}
	}

	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatEventThredTpGet(UINT32 thredTpId, stStatEvnThredTpNode **ppthredTpNode)
{
	UINT32 hash;
	UINT32 thredTpNum;
	stStatEvnThredTpNode *ptempThredTpNode;
	stStatThredTemp *pthredTpInfo;

	if(ppthredTpNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pthredTpInfo = &(pgstEQUStatProcSpace->statEventThredTemp);
	
	hash = thredTpId%(pthredTpInfo->hash);

	thredTpNum = 0;
	ptempThredTpNode = (stStatEvnThredTpNode *)listFirst(&(pthredTpInfo->actThredTpQuen[hash]));
	while((ptempThredTpNode!=NULL)&&(thredTpNum<pthredTpInfo->actThredTpNum[hash]))
	{
		if(thredTpId == ptempThredTpNode->thredTpId)
		{
			/*bingo */
			*ppthredTpNode = ptempThredTpNode;
			
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get thredTpId(%d)!\n\r",\
				__FUNCTION__, thredTpId);

			return GPN_STAT_DBS_GEN_OK;
		}

		thredTpNum++;
		ptempThredTpNode = (stStatEvnThredTpNode *)listNext((NODE *)ptempThredTpNode);
	}

	*ppthredTpNode = NULL;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find thredTpId(%d)!\n\r",\
		__FUNCTION__, thredTpId);
	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatEventThredTpSubTypeQuery(stStatEvnThredTpNode *pevnThredTpNode/*in*/,
	UINT32 subType, stStatEvnThredTpTable *pevnThredTpLine/*out*/)
{
	UINT32 i;
	UINT32 seat;

	/*assert */
	if(pevnThredTpNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(subType == GPN_STAT_SUB_TYPE_INVALID)
	{
		seat = 0;
	}
	else
	{
		/*give a invalid seat before for(...) start */
		seat = pevnThredTpNode->pevnThredTp->subNum;
		for(i=0;i<pevnThredTpNode->pevnThredTp->subNum;i++)
		{
			if(subType == pevnThredTpNode->pevnThredTp->pevnThred[i].subType)
			{
				seat = i;
				break;
			}
		}
	}
	
	if(seat >= pevnThredTpNode->pevnThredTp->subNum)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can't find subType(%08x) in evnThredTp(%d)\n\r",\
			__FUNCTION__, subType, pevnThredTpNode->thredTpId);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*find valid subType */
	if(pevnThredTpLine != NULL)
	{
		pevnThredTpLine->thredTpId = pevnThredTpNode->thredTpId;
		pevnThredTpLine->subType= pevnThredTpNode->pevnThredTp->pevnThred[seat].subType;
		pevnThredTpLine->scanType = pevnThredTpNode->scanType;
		pevnThredTpLine->upThredH32 = pevnThredTpNode->pevnThredTp->pevnThred[seat].upThredH32;
		pevnThredTpLine->upThredL32 = pevnThredTpNode->pevnThredTp->pevnThred[seat].upThredL32;
		pevnThredTpLine->dnThredH32 = pevnThredTpNode->pevnThredTp->pevnThred[seat].dnThredH32;
		pevnThredTpLine->dnThredL32 = pevnThredTpNode->pevnThredTp->pevnThred[seat].dnThredL32;
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatEventThredTpGetNext(UINT32 thredTpId, stStatEvnThredTpNode **ppthredTpNode, UINT32 *pnextThredTp)
{
	UINT32 hash;
	UINT32 thredTpNum;
	UINT32 tmpThredTpId;
	stStatEvnThredTpNode *ptempThredTpNode;
	stStatThredTemp *pthredTpInfo;

	/*assert */
	if( (ppthredTpNode == NULL) ||\
		(pnextThredTp == NULL) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pthredTpInfo = &(pgstEQUStatProcSpace->statEventThredTemp);

	tmpThredTpId = thredTpId;
	if(tmpThredTpId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		/*0xFFFFFFFF means template search start, so re-give a search start : must find first tp */
		hash = 0;
		while(hash < pthredTpInfo->hash)
		{
			thredTpNum = 0;
			ptempThredTpNode = (stStatEvnThredTpNode *)listFirst(&(pthredTpInfo->actThredTpQuen[hash]));
			if(ptempThredTpNode != NULL)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get first thredId(%d)!\n\r",\
					__FUNCTION__, ptempThredTpNode->thredTpId);
				*ppthredTpNode = ptempThredTpNode;
				tmpThredTpId = ptempThredTpNode->thredTpId;
				
				break;
			}

			/*to next quen */
			hash++;
		}
		if(hash >= pthredTpInfo->hash)
		{
			*ppthredTpNode = NULL;
			*pnextThredTp = GPN_STAT_INVALID_TEMPLAT_ID;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find first thredId!\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*first is invalid, so next is 'valid first' */
		*pnextThredTp = tmpThredTpId;
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d)!\n\r",\
			__FUNCTION__, *pnextThredTp);
		
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
		hash = tmpThredTpId%(pthredTpInfo->hash);

		thredTpNum = 0;
		ptempThredTpNode = (stStatEvnThredTpNode *)listFirst(&(pthredTpInfo->actThredTpQuen[hash]));
		while((ptempThredTpNode!=NULL)&&(thredTpNum<pthredTpInfo->actThredTpNum[hash]))
		{
			if(tmpThredTpId == ptempThredTpNode->thredTpId)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get this thredId(%d)!\n\r",\
					__FUNCTION__, tmpThredTpId);
				*ppthredTpNode = ptempThredTpNode;
				
				break;
			}

			thredTpNum++;
			ptempThredTpNode = (stStatEvnThredTpNode *)listNext((NODE *)ptempThredTpNode);
		}
		if((ptempThredTpNode==NULL)||(thredTpNum>=pthredTpInfo->actThredTpNum[hash]))
		{
			*ppthredTpNode = NULL;
			*pnextThredTp = GPN_STAT_INVALID_TEMPLAT_ID;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find thredId(%d)!\n\r",\
				__FUNCTION__, tmpThredTpId);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*fast go to find next node, search in old hash thredTp quen*/
		if(ptempThredTpNode != NULL)
		{
			thredTpNum++;
			ptempThredTpNode = (stStatEvnThredTpNode *)listNext((NODE *)ptempThredTpNode);
			if((ptempThredTpNode!=NULL)&&(thredTpNum<pthredTpInfo->actThredTpNum[hash]))
			{
				*pnextThredTp = ptempThredTpNode->thredTpId;
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d) in old quen!\n\r",\
					__FUNCTION__, ptempThredTpNode->thredTpId);
				
				return GPN_STAT_DBS_GEN_OK;
			}
		}
		
		/*fast go to find next node, search in all-other hash thredTp quen*/
		hash++;
		while(hash < pthredTpInfo->hash)
		{
			ptempThredTpNode = (stStatEvnThredTpNode *)listFirst(&(pthredTpInfo->actThredTpQuen[hash]));
			if(ptempThredTpNode != NULL)
			{
				*pnextThredTp = ptempThredTpNode->thredTpId;
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d) in new quen!\n\r",\
					__FUNCTION__, ptempThredTpNode->thredTpId);
				
				return GPN_STAT_DBS_GEN_OK;
			}
			
			/*start in new quen */
			hash++;
		}
	}
	
	*pnextThredTp = GPN_STAT_INVALID_TEMPLAT_ID;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not get next thredId!\n\r",\
		__FUNCTION__);
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatEventThredTpSubTypeQueryNext(stStatEvnThredTpNode *pevnThredTpNode/*in*/,
	UINT32 thredTpId, UINT32 subType, stStatEvnThredTpTable *pevnThredTpLine/*out*/,
	INT32 sNextEvnThredTpId, UINT32 *pnextEvnThredTpId, UINT32 *pnextSubType)
{
	UINT32 i;
	UINT32 seat;

	/*assert */
	if( (pevnThredTpNode == NULL) ||\
		(pnextEvnThredTpId == NULL) ||\
		(pnextSubType == NULL) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(subType == GPN_STAT_SUB_TYPE_INVALID)
	{
		seat = 0;
	}
	else
	{
		/*give a invalid seat before for(...) start */
		seat = pevnThredTpNode->pevnThredTp->subNum;
		for(i=0;i<pevnThredTpNode->pevnThredTp->subNum;i++)
		{
			if(subType == pevnThredTpNode->pevnThredTp->pevnThred[i].subType)
			{
				seat = i;
				break;
			}
		}
	}
	
	if(seat >= pevnThredTpNode->pevnThredTp->subNum)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can't find subType(%08x) in envThredTp(%d)\n\r",\
			__FUNCTION__, subType, pevnThredTpNode->thredTpId);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*find valid subType */
	if(pevnThredTpLine != NULL)
	{
		pevnThredTpLine->thredTpId = pevnThredTpNode->thredTpId;
		pevnThredTpLine->subType = pevnThredTpNode->pevnThredTp->pevnThred[seat].subType;
		pevnThredTpLine->scanType = pevnThredTpNode->scanType;
		pevnThredTpLine->upThredH32 = pevnThredTpNode->pevnThredTp->pevnThred[seat].upThredH32;
		pevnThredTpLine->upThredL32 = pevnThredTpNode->pevnThredTp->pevnThred[seat].upThredL32;
		pevnThredTpLine->dnThredH32 = pevnThredTpNode->pevnThredTp->pevnThred[seat].dnThredH32;
		pevnThredTpLine->dnThredL32 = pevnThredTpNode->pevnThredTp->pevnThred[seat].dnThredL32;
	}
	
	/*find next valid subType */
	if(thredTpId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		/*means first get next , use this index for next index */
		*pnextSubType = pevnThredTpNode->pevnThredTp->pevnThred[seat].subType;;
		*pnextEvnThredTpId = pevnThredTpNode->thredTpId;
	}
	else
	{
		for(seat += 1;seat<pevnThredTpNode->pevnThredTp->subNum;seat++)
		{
			if(pevnThredTpNode->pevnThredTp->pevnThred[seat].subType != GPN_STAT_SUB_TYPE_INVALID) 
			{
				/*find valid next subType in same line*/
				*pnextSubType = pevnThredTpNode->pevnThredTp->pevnThred[seat].subType;
				/*next valid still in same line */
				*pnextEvnThredTpId = pevnThredTpNode->thredTpId;

				break;
			}
		}
		
		if(seat >= pevnThredTpNode->pevnThredTp->subNum)
		{
			/*not find valid next subType in same line */
			*pnextSubType = GPN_STAT_SUB_TYPE_INVALID;
			/*next valid in next line */
			*pnextEvnThredTpId = sNextEvnThredTpId;
		}
	}
	
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s :statiThredTp(%d) thredTp(%d) subType(%08x) goto next thredTp(%d) subType(%08x)\n\r",\
		__FUNCTION__, sNextEvnThredTpId, pevnThredTpNode->thredTpId, subType,\
		*pnextEvnThredTpId, *pnextSubType);
	
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatEventThredTpSubTypeQueryNextAdjust(UINT32 *pnextEvnThredTpId, UINT32 *pnextSubType)
{
	stStatEvnThredTpNode *pthredTpNode;
	//evnThredTpUnit *pevnThred;
	UINT32 i;
	
	/*assert */
	if( (pnextEvnThredTpId == NULL) ||\
		(pnextSubType == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(*pnextEvnThredTpId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		*pnextSubType = GPN_STAT_SUB_TYPE_INVALID;
		
		return GPN_STAT_DBS_GEN_OK;
	}

	if(*pnextSubType == GPN_STAT_SUB_TYPE_INVALID)
	{
		pthredTpNode = NULL;
		gpnStatEventThredTpGet(*pnextEvnThredTpId, &pthredTpNode);
		if(pthredTpNode == NULL)
		{
			return GPN_STAT_DBS_GEN_ERR;
		}

		//pevnThred = pthredTpNode->pevnThredTp->pevnThred;
		for(i=0;i<pthredTpNode->pevnThredTp->subNum;i++)
		{
			if(pthredTpNode->pevnThredTp->pevnThred[i].subType != GPN_STAT_SUB_TYPE_INVALID)
			{
				*pnextSubType = pthredTpNode->pevnThredTp->pevnThred[i].subType;

				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : adjust to nextThredTp(%d) nextSubType(%08x)\n\r",\
					__FUNCTION__, *pnextEvnThredTpId, *pnextSubType);
				break;
			}
		}

		if(i >= pthredTpNode->pevnThredTp->subNum)
		{
			return GPN_STAT_DBS_GEN_ERR;
		}
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatAlarmThredTpAdd(stStatAlmThredTpTable *pthredTpLine)
{
	UINT32 i;
	UINT32 hash;
	UINT32 order;
	UINT32 addMode;
	UINT32 subTpNum;
	stStatAlmThredTpNode *palmThredTpNode;
	stStatThredTemp *pstatThredTpInfo;
	stStatScanTypeDef *pStatScanTpNode;
	almThredTpUnit *palmThredTpUnit;

	/*assert */
	if(pthredTpLine == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	/*assert subReportId */
	if( (pthredTpLine->thredTpId == 0)  ||\
		(pthredTpLine->thredTpId == GPN_STAT_INVALID_TEMPLAT_ID) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : thredTpId(%d) err!\n\r",\
			__FUNCTION__, pthredTpLine->thredTpId);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pstatThredTpInfo = &(pgstEQUStatProcSpace->statAlarmThredTemp);
	hash = (pthredTpLine->thredTpId)%(pstatThredTpInfo->hash);

	/*fix add mode */
	addMode = GPN_STAT_DBS_ADD_NEW_LINE;
	subTpNum = 0;
	palmThredTpNode = (stStatAlmThredTpNode *)listFirst(&(pstatThredTpInfo->actThredTpQuen[hash]));
	while((palmThredTpNode!=NULL)&&(subTpNum<pstatThredTpInfo->actThredTpNum[hash]))
	{
		if(palmThredTpNode->thredTpId == pthredTpLine->thredTpId)
		{	
			/*already add lin, find subType */
			if(gpnStatAlarmThredTpSubTypeQuery(palmThredTpNode,\
				pthredTpLine->subType, NULL) == GPN_STAT_DBS_GEN_OK)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : thredTpId(%d) subType(%08x) already creat!\n\r",\
					__FUNCTION__, pthredTpLine->thredTpId, pthredTpLine->subType);
			
				return GPN_STAT_DBS_GEN_ERR;
			}
			else
			{
				addMode = GPN_STAT_DBS_ADD_NEW_UNIT;
			}

			break;
		}

		subTpNum++;
		palmThredTpNode = (stStatAlmThredTpNode *)listNext((NODE *)palmThredTpNode);
	}
	if((palmThredTpNode==NULL)||(subTpNum>=pstatThredTpInfo->actThredTpNum[hash]))
	{
		addMode = GPN_STAT_DBS_ADD_NEW_LINE;
	}
	
	/*add new node */
	if(addMode == GPN_STAT_DBS_ADD_NEW_LINE)
	{
		/*check resouce ok */
		if(pstatThredTpInfo->idleThredTpNum== 0)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : do not have idleNode(%d)!\n\r",\
				__FUNCTION__, pstatThredTpInfo->idleThredTpNum);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*find right idle node */
		subTpNum = 0;
		palmThredTpNode = (stStatAlmThredTpNode *)listFirst(&(pstatThredTpInfo->idleThredTpQuen));
		while((palmThredTpNode!=NULL)&&(subTpNum<pstatThredTpInfo->idleThredTpNum))
		{
			if(palmThredTpNode->thredTpId == pthredTpLine->thredTpId)
			{
				break;
			}
			
			subTpNum++;
			palmThredTpNode = (stStatAlmThredTpNode *)listNext((NODE *)palmThredTpNode);
		}
		if((palmThredTpNode==NULL)||(subTpNum>=pstatThredTpInfo->idleThredTpNum))
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : don't find thredTpId(%d) in idle list!\n\r",\
				__FUNCTION__, pthredTpLine->thredTpId);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*init memrey : calculate space size */
		pStatScanTpNode = NULL;
		gpnStatTypeGetScanTypeNode(pthredTpLine->scanType, &pStatScanTpNode);
		if(pStatScanTpNode == NULL)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : get scanType(%08x) node err!\n\r",\
				__FUNCTION__, pthredTpLine->scanType);
			return GPN_STAT_DBS_GEN_ERR;
		}

		subTpNum = pStatScanTpNode->statSubTpNumInScanType;
		i = sizeof(almThredTp) + sizeof(almThredTpUnit)*subTpNum;
		palmThredTpNode->palmThredTp = (almThredTp *)malloc(i);
		if(palmThredTpNode->palmThredTp == NULL)
		{
			gpnLog(GPN_LOG_L_ERR, "%s : malloc err\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}
		
		/*init memrey : init memrey 'palmThredTpNode' */
		palmThredTpNode->scanType = pthredTpLine->scanType;
		/*init memrey : init memrey 'palmThredTp' */
		palmThredTpNode->palmThredTp->subNum = subTpNum;
		palmThredTpNode->palmThredTp->palmThred =\
			(almThredTpUnit *)(palmThredTpNode->palmThredTp + 1);

		palmThredTpUnit = palmThredTpNode->palmThredTp->palmThred;
		for(i=0;i<subTpNum;i++)
		{
			/*just need set sub type to NULL */
			palmThredTpUnit[i].subType = GPN_STAT_SUB_TYPE_INVALID;
			palmThredTpUnit[i].upRiseThredH32 = GPN_STAT_DBS_32_FFFF;
			palmThredTpUnit[i].upRiseThredL32 = GPN_STAT_DBS_32_FFFF;
			palmThredTpUnit[i].upDispThredH32 = GPN_STAT_DBS_32_FFFF;
			palmThredTpUnit[i].upDispThredL32 = GPN_STAT_DBS_32_FFFF;
			palmThredTpUnit[i].dnRiseThredH32 = GPN_STAT_DBS_32_NULL;
			palmThredTpUnit[i].dnRiseThredL32 = GPN_STAT_DBS_32_NULL;
			palmThredTpUnit[i].dnDispThredH32 = GPN_STAT_DBS_32_NULL;
			palmThredTpUnit[i].dnDispThredL32 = GPN_STAT_DBS_32_NULL;
		}

		listDelete(&(pstatThredTpInfo->idleThredTpQuen), (NODE *)palmThredTpNode);
		pstatThredTpInfo->idleThredTpNum--;
			
		listAdd(&(pstatThredTpInfo->actThredTpQuen[hash]), (NODE *)palmThredTpNode);
		pstatThredTpInfo->actThredTpNum[hash]++;
	}
	else
	{
		/*just already find  'pevnThredTpNode' */
	}

	/*add new unit : find subType's order & add new unit in line */
	palmThredTpUnit = palmThredTpNode->palmThredTp->palmThred;
	if(gpnStatTypeGetSubTpOrderBaseScanType(pthredTpLine->subType,\
					pthredTpLine->scanType, &order) == GPN_STAT_DBS_GEN_OK)
	{
		palmThredTpUnit[order].subType = pthredTpLine->subType;
		palmThredTpUnit[order].upRiseThredH32 = pthredTpLine->upRiseThredH32;
		palmThredTpUnit[order].upRiseThredL32 = pthredTpLine->upRiseThredL32;
		palmThredTpUnit[order].upDispThredH32 = pthredTpLine->upDispThredH32;
		palmThredTpUnit[order].upDispThredL32 = pthredTpLine->upDispThredL32;
		palmThredTpUnit[order].dnRiseThredH32 = pthredTpLine->dnRiseThredH32;
		palmThredTpUnit[order].dnRiseThredL32 = pthredTpLine->dnRiseThredL32;
		palmThredTpUnit[order].dnDispThredH32 = pthredTpLine->dnDispThredH32;
		palmThredTpUnit[order].dnDispThredL32 = pthredTpLine->dnDispThredL32;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : scanType(%08x)'s subType(%08x) almThredTp(%d), err order!\n\r",\
			__FUNCTION__, pthredTpLine->scanType, pthredTpLine->subType,\
			palmThredTpNode->thredTpId);

		return GPN_STAT_DBS_GEN_ERR;
	}

	/*debug print */
	#if 0
	printf("thredTpId %d scanType %08x subTypeNum %d\n\r",\
		palmThredTpNode->thredTpId, palmThredTpNode->scanType,\
		palmThredTpNode->palmThredTp->subNum);
	for(i=0;i<palmThredTpNode->palmThredTp->subNum;i++)
	{
		printf("subType %08x upHr %08x upLr %08x upHd %08x upLd %08x dnHr %08x dnLr %08x dnHd %08x dnLd %08x\n",\
			palmThredTpNode->palmThredTp->palmThred[i].subType,\
			palmThredTpNode->palmThredTp->palmThred[i].upRiseThredH32,\
			palmThredTpNode->palmThredTp->palmThred[i].upRiseThredL32,\
			palmThredTpNode->palmThredTp->palmThred[i].upDispThredH32,\
			palmThredTpNode->palmThredTp->palmThred[i].upDispThredL32,\
			palmThredTpNode->palmThredTp->palmThred[i].dnRiseThredH32,\
			palmThredTpNode->palmThredTp->palmThred[i].dnRiseThredL32,\
			palmThredTpNode->palmThredTp->palmThred[i].dnDispThredH32,\
			palmThredTpNode->palmThredTp->palmThred[i].dnDispThredL32);
	}
	#endif
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : add scanType(%08x)'s subType(%08x) in almThredTp(%d)!\n\r",\
		__FUNCTION__, pthredTpLine->scanType, pthredTpLine->subType,\
		palmThredTpNode->thredTpId);
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatAlarmTpDelCheckPortDel(stStatAlmThredTpNode *pthredTpNode)
{
	stStatLocalPortNode *pStatLocalNode, *sStatLocalNode;
	stStatScanPortInfo *pStatScanPort;
	stStatPortMonMgt portMonMgt;
	stStatTaskNode *pTaskNode;

	/*assert */
	if( pthredTpNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatLocalNode = NULL;
	gpnStatSeekFirstValidScanNodeIndex(&pStatLocalNode);
	if (pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	do 
	{
		pStatScanPort = pStatLocalNode->pStatScanPort;
		if (pthredTpNode->scanType == pStatScanPort->statScanType)
		{
			if(gpnStatPortMonGet(pStatLocalNode, pthredTpNode->scanType, &portMonMgt) == GPN_STAT_DBS_GEN_OK)
			{
				if (pStatScanPort->statLCycRec)
				{
					pTaskNode = pStatScanPort->statLCycRec->ptaskNode;
					if (pTaskNode)
					{
						if(pTaskNode->actMOnOBjNum != 0 && pthredTpNode->thredTpId == pTaskNode->almThredTpId)
						{
							gpnLog(GPN_LOG_L_INFO, "%s : alarmThredTpId(%d) is in use in port moni task!\n\r",\
									__FUNCTION__, pthredTpNode->thredTpId);
							return GPN_STAT_DBS_GEN_OK;
						}
					}
				}

				if (pStatScanPort->statSCycRec)
				{
					pTaskNode = pStatScanPort->statSCycRec->ptaskNode;
					if (pTaskNode)
					{
						if(pTaskNode->actMOnOBjNum != 0 && pthredTpNode->thredTpId == pTaskNode->almThredTpId)
						{
							gpnLog(GPN_LOG_L_INFO, "%s : alarmThredTpId(%d) is in use in port moni task!\n\r",\
									__FUNCTION__, pthredTpNode->thredTpId);
							return GPN_STAT_DBS_GEN_OK;
						}
					}
				}

				if (pStatScanPort->statUCycRec)
				{
					pTaskNode = pStatScanPort->statUCycRec->ptaskNode;
					if (pTaskNode)
					{
						if(pTaskNode->actMOnOBjNum != 0 && pthredTpNode->thredTpId == pTaskNode->almThredTpId)
						{
							gpnLog(GPN_LOG_L_INFO, "%s : alarmThredTpId(%d) is in use in port moni task!\n\r",\
									__FUNCTION__, pthredTpNode->thredTpId);
							return GPN_STAT_DBS_GEN_OK;
						}
					}
				}
			}
		}

		gpnStatSeekNextValidScanNodeIndex(pStatLocalNode, &sStatLocalNode);
		pStatLocalNode = sStatLocalNode;
		
	} while (pStatLocalNode);

	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatAlarmThredTpDelete(UINT32 thredTpId)
{
	UINT32 hash;
	UINT32 thredTpNum;
	stStatAlmThredTpNode *pthredTpNode;
	stStatThredTemp *pthredTpInfo;

	pthredTpInfo = &(pgstEQUStatProcSpace->statAlarmThredTemp);
	
	hash = thredTpId%(pthredTpInfo->hash);

	thredTpNum = 0;
	pthredTpNode = (stStatAlmThredTpNode *)listFirst(&(pthredTpInfo->actThredTpQuen[hash]));
	while((pthredTpNode!=NULL)&&(thredTpNum<pthredTpInfo->actThredTpNum[hash]))
	{
		if(thredTpId == pthredTpNode->thredTpId)
		{
			/*delete task, first delete port */
//			if (gpnStatAlarmTpDelCheckPortDel(pthredTpNode) != GPN_STAT_DBS_GEN_OK)
//			{
				listDelete(&(pthredTpInfo->actThredTpQuen[hash]), (NODE *)pthredTpNode);
				pthredTpInfo->actThredTpNum[hash]--;

				listAdd(&(pthredTpInfo->idleThredTpQuen), (NODE *)pthredTpNode);
				pthredTpInfo->idleThredTpNum++;

				/*memrey free */
				if(pthredTpNode->palmThredTp != NULL)
				{
					free(pthredTpNode->palmThredTp);
					pthredTpNode->palmThredTp = NULL;
				}
				
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : delete alarm thredTp(%d)!\n\r",\
					__FUNCTION__, pthredTpNode->thredTpId);

				return GPN_STAT_DBS_GEN_OK;
			}
//		}

		thredTpNum++;
		pthredTpNode = (stStatAlmThredTpNode *)listNext((NODE *)pthredTpNode);
	}

	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatAlarmThredTpModify(stStatAlmThredTpTable *pthredTpLine)
{
	UINT32 hash;
	UINT32 order;
	UINT32 modify;
	UINT32 subTpNum;
	stStatAlmThredTpNode *palmThredTpNode;
	stStatThredTemp *palmThredTpInfo;
	almThredTpUnit *palmThredUnit;
	
	/*assert */
	if(pthredTpLine == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	palmThredTpInfo = &(pgstEQUStatProcSpace->statAlarmThredTemp);
	hash = (pthredTpLine->thredTpId)%(palmThredTpInfo->hash);

	/*fix add mode */
	modify = GPN_STAT_DBS_GEN_NO;
	subTpNum = 0;
	palmThredTpNode = (stStatAlmThredTpNode *)listFirst(&(palmThredTpInfo->actThredTpQuen[hash]));
	while((palmThredTpNode!=NULL)&&(subTpNum<palmThredTpInfo->actThredTpNum[hash]))
	{
		if(palmThredTpNode->thredTpId == pthredTpLine->thredTpId)
		{	
			/*check modify-able */
			if(gpnStatAlarmThredTpSubTypeQuery(palmThredTpNode,
				pthredTpLine->subType, NULL) == GPN_STAT_DBS_GEN_OK)
			{
				modify = GPN_STAT_DBS_GEN_YES;
			}
			
			break;
		}

		subTpNum++;
		palmThredTpNode = (stStatAlmThredTpNode *)listNext((NODE *)palmThredTpNode);
	}

	/*add new node */
	if(modify == GPN_STAT_DBS_GEN_YES)
	{
		/*find valid sub type, then modify */
		palmThredUnit = palmThredTpNode->palmThredTp->palmThred;
		if(gpnStatTypeGetSubTpOrderBaseScanType(pthredTpLine->subType,\
			palmThredTpNode->scanType, &order) == GPN_STAT_DBS_GEN_OK)
		{
			/*palmThredUnit[order].subType = pthredTpLine->subType;*/
			palmThredUnit[order].upRiseThredH32 = pthredTpLine->upRiseThredH32;
			palmThredUnit[order].upRiseThredL32 = pthredTpLine->upRiseThredL32;
			palmThredUnit[order].upDispThredH32 = pthredTpLine->upDispThredH32;
			palmThredUnit[order].upDispThredL32 = pthredTpLine->upDispThredL32;
			palmThredUnit[order].dnRiseThredH32 = pthredTpLine->dnRiseThredH32;
			palmThredUnit[order].dnRiseThredL32 = pthredTpLine->dnRiseThredL32;
			palmThredUnit[order].dnDispThredH32 = pthredTpLine->dnDispThredH32;
			palmThredUnit[order].dnDispThredL32 = pthredTpLine->dnDispThredL32;
			return GPN_STAT_DBS_GEN_OK;
		}
	}

	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatAlarmThredTpGet(UINT32 thredTpId, stStatAlmThredTpNode **ppthredTpNode)
{
	UINT32 hash;
	UINT32 thredTpNum;
	stStatAlmThredTpNode *ptempThredTpNode;
	stStatThredTemp *pthredTpInfo;

	if(ppthredTpNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pthredTpInfo = &(pgstEQUStatProcSpace->statAlarmThredTemp);
	
	hash = thredTpId%(pthredTpInfo->hash);

	thredTpNum = 0;
	ptempThredTpNode = (stStatAlmThredTpNode *)listFirst(&(pthredTpInfo->actThredTpQuen[hash]));
	while((ptempThredTpNode!=NULL)&&(thredTpNum<pthredTpInfo->actThredTpNum[hash]))
	{
		if(thredTpId == ptempThredTpNode->thredTpId)
		{
			/*bingo */
			*ppthredTpNode = ptempThredTpNode;
			
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get AlarmThredId(%d)!\n\r",\
				__FUNCTION__, thredTpId);

			return GPN_STAT_DBS_GEN_OK;
		}

		thredTpNum++;
		ptempThredTpNode = (stStatAlmThredTpNode *)listNext((NODE *)ptempThredTpNode);
	}

	*ppthredTpNode = NULL;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find AlarmThredId(%d)!\n\r",\
		__FUNCTION__, thredTpId);
	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatAlarmThredTpSubTypeQuery(stStatAlmThredTpNode *palmThredTpNode/*in*/,
	UINT32 subType, stStatAlmThredTpTable *palmThredTpLine/*out*/)
{
	UINT32 i;
	UINT32 seat;

	/*assert */
	if(palmThredTpNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(subType == GPN_STAT_SUB_TYPE_INVALID)
	{
		seat = 0;
	}
	else
	{
		/*give a invalid seat before for(...) start */
		seat = palmThredTpNode->palmThredTp->subNum;
		for(i=0;i<palmThredTpNode->palmThredTp->subNum;i++)
		{
			if(subType == palmThredTpNode->palmThredTp->palmThred[i].subType)
			{
				seat = i;
				break;
			}
		}
	}
	
	if(seat >= palmThredTpNode->palmThredTp->subNum)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can't find subType(%08x) in almThredTp(%d)\n\r",\
			__FUNCTION__, subType, palmThredTpNode->thredTpId);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*find valid subType */
	if(palmThredTpLine != NULL)
	{
		palmThredTpLine->thredTpId = palmThredTpNode->thredTpId;
		palmThredTpLine->subType= palmThredTpNode->palmThredTp->palmThred[seat].subType;
		palmThredTpLine->scanType = palmThredTpNode->scanType;
		palmThredTpLine->upRiseThredH32 = palmThredTpNode->palmThredTp->palmThred[seat].upRiseThredH32;
		palmThredTpLine->upRiseThredL32 = palmThredTpNode->palmThredTp->palmThred[seat].upRiseThredL32;
		palmThredTpLine->upDispThredH32 = palmThredTpNode->palmThredTp->palmThred[seat].upDispThredH32;
		palmThredTpLine->upDispThredL32 = palmThredTpNode->palmThredTp->palmThred[seat].upDispThredL32;
		palmThredTpLine->dnRiseThredH32 = palmThredTpNode->palmThredTp->palmThred[seat].dnRiseThredH32;
		palmThredTpLine->dnRiseThredL32 = palmThredTpNode->palmThredTp->palmThred[seat].dnRiseThredL32;
		palmThredTpLine->dnDispThredH32 = palmThredTpNode->palmThredTp->palmThred[seat].dnDispThredH32;
		palmThredTpLine->dnDispThredL32 = palmThredTpNode->palmThredTp->palmThred[seat].dnDispThredL32;
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatAlarmThredTpGetNext(UINT32 thredTpId, stStatAlmThredTpNode **ppthredTpNode, UINT32 *pnextThredTp)
{
	UINT32 hash;
	UINT32 thredTpNum;
	UINT32 tmpThredTpId;
	stStatAlmThredTpNode *ptempThredTpNode;
	stStatThredTemp *pthredTpInfo;

	/*assert */
	if( (ppthredTpNode == NULL) ||\
		(pnextThredTp == NULL) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pthredTpInfo = &(pgstEQUStatProcSpace->statAlarmThredTemp);

	tmpThredTpId = thredTpId;
	if(tmpThredTpId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		/*0xFFFFFFFF means template search start, so re-give a search start : must find first tp */
		hash = 0;
		while(hash < pthredTpInfo->hash)
		{
			thredTpNum = 0;
			ptempThredTpNode = (stStatAlmThredTpNode *)listFirst(&(pthredTpInfo->actThredTpQuen[hash]));
			if(ptempThredTpNode != NULL)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get first thredId(%d)!\n\r",\
					__FUNCTION__, ptempThredTpNode->thredTpId);
				*ppthredTpNode = ptempThredTpNode;
				tmpThredTpId = ptempThredTpNode->thredTpId;
				
				break;
			}

			/*to next quen */
			hash++;
		}
		if(hash >= pthredTpInfo->hash)
		{
			*ppthredTpNode = NULL;
			*pnextThredTp = GPN_STAT_INVALID_TEMPLAT_ID;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find first thredId!\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*first is invalid, so next is 'valid first' */
		*pnextThredTp = tmpThredTpId;
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d)!\n\r",\
			__FUNCTION__, *pnextThredTp);
		
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
		hash = tmpThredTpId%(pthredTpInfo->hash);

		thredTpNum = 0;
		ptempThredTpNode = (stStatAlmThredTpNode *)listFirst(&(pthredTpInfo->actThredTpQuen[hash]));
		while((ptempThredTpNode!=NULL)&&(thredTpNum<pthredTpInfo->actThredTpNum[hash]))
		{
			if(tmpThredTpId == ptempThredTpNode->thredTpId)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get this thredId(%d)!\n\r",\
					__FUNCTION__, tmpThredTpId);
				*ppthredTpNode = ptempThredTpNode;
				
				break;
			}

			thredTpNum++;
			ptempThredTpNode = (stStatAlmThredTpNode *)listNext((NODE *)ptempThredTpNode);
		}
		if((ptempThredTpNode==NULL)||(thredTpNum>=pthredTpInfo->actThredTpNum[hash]))
		{
			*ppthredTpNode = NULL;
			
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find thredId(%d)!\n\r",\
				__FUNCTION__, tmpThredTpId);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*fast go to find next node, search in old hash thredTp quen*/
		if(ptempThredTpNode != NULL)
		{
			thredTpNum++;
			ptempThredTpNode = (stStatAlmThredTpNode *)listNext((NODE *)ptempThredTpNode);
			if((ptempThredTpNode!=NULL)&&(thredTpNum<pthredTpInfo->actThredTpNum[hash]))
			{
				*pnextThredTp = ptempThredTpNode->thredTpId;
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d) in old quen!\n\r",\
					__FUNCTION__, *pnextThredTp);
				return GPN_STAT_DBS_GEN_OK;
			}
		}
		
		/*fast go to find next node, search in all-other hash thredTp quen*/
		hash++;
		while(hash < pthredTpInfo->hash)
		{
			ptempThredTpNode = (stStatAlmThredTpNode *)listFirst(&(pthredTpInfo->actThredTpQuen[hash]));
			if(ptempThredTpNode != NULL)
			{
				*pnextThredTp = ptempThredTpNode->thredTpId;
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d) in new quen!\n\r",\
					__FUNCTION__, *pnextThredTp);
				return GPN_STAT_DBS_GEN_OK;
			}
			
			/*start in new quen */
			hash++;
		}
	}
	
	*pnextThredTp = GPN_STAT_INVALID_TEMPLAT_ID;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not get next thredId!\n\r",\
		__FUNCTION__);
	return GPN_STAT_DBS_GEN_OK;
}


UINT32 gpnStatAlarmThredTpSubTypeQueryNext(stStatAlmThredTpNode *palmThredTpNode/*in*/,
	UINT32 thredTpId, UINT32 subType, stStatAlmThredTpTable *palmThredTpLine/*out*/,
	UINT32 sNextAlmThredTpId, UINT32 *pnextAlmThredTpId, UINT32 *pnextSubType)
{
	UINT32 i;
	UINT32 seat;

	/*assert */
	if( (palmThredTpNode == NULL) ||\
		(pnextAlmThredTpId == NULL) ||\
		(pnextSubType == NULL) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(subType == GPN_STAT_SUB_TYPE_INVALID)
	{
		/*in getNext, here is subType find first, so qurey valid 'first', not 'order first' */
		seat = palmThredTpNode->palmThredTp->subNum;
		for(i=0;i<palmThredTpNode->palmThredTp->subNum;i++)
		{
			if(palmThredTpNode->palmThredTp->palmThred[i].subType !=\
				GPN_STAT_SUB_TYPE_INVALID)
			{
				seat = i;
				break;
			}
		}
	}
	else
	{
		/*give a invalid seat before for(...) start */
		seat = palmThredTpNode->palmThredTp->subNum;
		for(i=0;i<palmThredTpNode->palmThredTp->subNum;i++)
		{
			if(subType == palmThredTpNode->palmThredTp->palmThred[i].subType)
			{
				seat = i;
				break;
			}
		}
	}
	
	if(seat >= palmThredTpNode->palmThredTp->subNum)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can't find subType(%08x) in almThredTp(%d)\n\r",\
			__FUNCTION__, subType, palmThredTpNode->thredTpId);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*find valid subType */
	if(palmThredTpLine != NULL)
	{
		palmThredTpLine->thredTpId = palmThredTpNode->thredTpId;
		palmThredTpLine->subType = palmThredTpNode->palmThredTp->palmThred[seat].subType;
		palmThredTpLine->scanType = palmThredTpNode->scanType;
		palmThredTpLine->upRiseThredH32 = palmThredTpNode->palmThredTp->palmThred[seat].upRiseThredH32;
		palmThredTpLine->upRiseThredL32 = palmThredTpNode->palmThredTp->palmThred[seat].upRiseThredL32;
		palmThredTpLine->upDispThredH32 = palmThredTpNode->palmThredTp->palmThred[seat].upDispThredH32;
		palmThredTpLine->upDispThredL32 = palmThredTpNode->palmThredTp->palmThred[seat].upDispThredL32;
		palmThredTpLine->dnRiseThredH32 = palmThredTpNode->palmThredTp->palmThred[seat].dnRiseThredH32;
		palmThredTpLine->dnRiseThredL32 = palmThredTpNode->palmThredTp->palmThred[seat].dnRiseThredL32;
		palmThredTpLine->dnDispThredH32 = palmThredTpNode->palmThredTp->palmThred[seat].dnDispThredH32;
		palmThredTpLine->dnDispThredL32 = palmThredTpNode->palmThredTp->palmThred[seat].dnDispThredL32;
	}
	
	/*find next valid subType */
	if(thredTpId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		/*means first get next , use this index for next index */
		*pnextSubType = palmThredTpNode->palmThredTp->palmThred[seat].subType;;
		*pnextAlmThredTpId = palmThredTpNode->thredTpId;
	}
	else
	{
		for(seat += 1;seat<palmThredTpNode->palmThredTp->subNum;seat++)
		{
			if(palmThredTpNode->palmThredTp->palmThred[seat].subType != GPN_STAT_SUB_TYPE_INVALID) 
			{
				/*find valid next subType in same line*/
				*pnextSubType = palmThredTpNode->palmThredTp->palmThred[seat].subType;
				/*next valid still in same line */
				*pnextAlmThredTpId = palmThredTpNode->thredTpId;

				break;
			}
		}
		
		if(seat >= palmThredTpNode->palmThredTp->subNum)
		{
			/*not find valid next subType in same line */
			*pnextSubType = GPN_STAT_SUB_TYPE_INVALID;
			/*next valid in next line */
			*pnextAlmThredTpId = sNextAlmThredTpId;
		}
	}
	
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s :statiThredTp(%d) thredTp(%d) subType(%08x) goto next thredTp(%d) subType(%08x)\n\r",\
		__FUNCTION__, sNextAlmThredTpId, palmThredTpNode->thredTpId, subType,\
		*pnextAlmThredTpId, *pnextSubType);
	
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatAlarmThredTpSubTypeQueryNextAdjust(UINT32 *pnextAlmThredTpId, UINT32 *pnextSubType)
{
	stStatAlmThredTpNode *pthredTpNode;
	//almThredTpUnit *palmThred;
	UINT32 i;
	
	/*assert */
	if( (pnextAlmThredTpId == NULL) ||\
		(pnextSubType == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(*pnextAlmThredTpId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		*pnextSubType = GPN_STAT_SUB_TYPE_INVALID;
		
		return GPN_STAT_DBS_GEN_OK;
	}

	if(*pnextSubType == GPN_STAT_SUB_TYPE_INVALID)
	{
		pthredTpNode = NULL;
		gpnStatAlarmThredTpGet(*pnextAlmThredTpId, &pthredTpNode);
		if(pthredTpNode == NULL)
		{
			return GPN_STAT_DBS_GEN_ERR;
		}

		//palmThred = pthredTpNode->palmThredTp->palmThred;
		for(i=0;i<pthredTpNode->palmThredTp->subNum;i++)
		{
			if(pthredTpNode->palmThredTp->palmThred[i].subType != GPN_STAT_SUB_TYPE_INVALID)
			{
				*pnextSubType = pthredTpNode->palmThredTp->palmThred[i].subType;

				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : adjust to nextThredTp(%d) nextSubType(%08x)\n\r",\
					__FUNCTION__, *pnextAlmThredTpId, *pnextSubType);
				break;
			}
		}

		if(i >= pthredTpNode->palmThredTp->subNum)
		{
			return GPN_STAT_DBS_GEN_ERR;
		}
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatSubFiltTpAdd(stStatSubFiltTpTable *psubFiltTpLine)
{
	UINT32 i;
	UINT32 hash;
	UINT32 order;
	UINT32 addMode;
	UINT32 subTpNum;
	stStatSubFiltTpNode *psubFiltTpNode;
	stStatSubFiltTemp *psubFiltTpInfo;
	stStatScanTypeDef *pStatScanTpNode;
	subFiltTpUnit *psubFiltUnit;
	
	/*assert */
	if(psubFiltTpLine == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	/*assert subReportId */
	if( (psubFiltTpLine->subFiltTpId == 0)  ||\
		(psubFiltTpLine->subFiltTpId == GPN_STAT_INVALID_TEMPLAT_ID) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : subFiltTpId(%d) err!\n\r",\
			__FUNCTION__, psubFiltTpLine->subFiltTpId);
		return GPN_STAT_DBS_GEN_ERR;
	}

	psubFiltTpInfo = &(pgstEQUStatProcSpace->statSubFiltTemp);
	hash = (psubFiltTpLine->subFiltTpId)%(psubFiltTpInfo->hash);

	/*fix add mode */
	subTpNum = 0;
	psubFiltTpNode = (stStatSubFiltTpNode *)listFirst(&(psubFiltTpInfo->actSubFiltTpQuen[hash]));
	while((psubFiltTpNode!=NULL)&&(subTpNum<psubFiltTpInfo->actSubFiltTpNum[hash]))
	{
		if(psubFiltTpNode->subFiltTpId == psubFiltTpLine->subFiltTpId)
		{	
			/*already add lin, find subType */
			if(gpnStatSubFiltTpSubTypeQuery(psubFiltTpNode,
				psubFiltTpLine->subType, NULL) == GPN_STAT_DBS_GEN_OK)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : subFiltId(%d) subType(%08x) already creat!\n\r",\
					__FUNCTION__, psubFiltTpLine->subFiltTpId, psubFiltTpLine->subType);
			
				return GPN_STAT_DBS_GEN_ERR;
			}
			else
			{
				addMode = GPN_STAT_DBS_ADD_NEW_UNIT;
			}

			break;
		}

		subTpNum++;
		psubFiltTpNode = (stStatSubFiltTpNode *)listNext((NODE *)psubFiltTpNode);
	}
	if((psubFiltTpNode==NULL)||(subTpNum>=psubFiltTpInfo->actSubFiltTpNum[hash]))
	{
		addMode = GPN_STAT_DBS_ADD_NEW_LINE;
	}
	
	/*add new node */
	if(addMode == GPN_STAT_DBS_ADD_NEW_LINE)
	{
		/*check resouce ok */
		if(psubFiltTpInfo->idleSubFiltTpNum == 0)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : do not have idleNode(%d)!\n\r",\
				__FUNCTION__, psubFiltTpInfo->idleSubFiltTpNum);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*find right idle node */
		subTpNum = 0;
		psubFiltTpNode = (stStatSubFiltTpNode *)listFirst(&(psubFiltTpInfo->idleSubFiltTpQuen));
		while((psubFiltTpNode!=NULL)&&(subTpNum<psubFiltTpInfo->idleSubFiltTpNum))
		{
			if(psubFiltTpNode->subFiltTpId == psubFiltTpLine->subFiltTpId)
			{
				break;
			}
			
			subTpNum++;
			psubFiltTpNode = (stStatSubFiltTpNode *)listNext((NODE *)psubFiltTpNode);
		}
		if((psubFiltTpNode==NULL)||(subTpNum>=psubFiltTpInfo->idleSubFiltTpNum))
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : don't find subFiltId(%d) in idle list!\n\r",\
				__FUNCTION__, psubFiltTpLine->subFiltTpId);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*init memrey : calculate space size */
		pStatScanTpNode = NULL;
		gpnStatTypeGetScanTypeNode(psubFiltTpLine->scanType, &pStatScanTpNode);
		if(pStatScanTpNode == NULL)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : get scanType(%08x) node err!\n\r",\
				__FUNCTION__, psubFiltTpLine->scanType);
			return GPN_STAT_DBS_GEN_ERR;
		}

		subTpNum = pStatScanTpNode->statSubTpNumInScanType;
		i = sizeof(subFiltTp) + sizeof(subFiltTpUnit)*subTpNum;
		psubFiltTpNode->psubFiltTp = (subFiltTp *)malloc(i);
		if(psubFiltTpNode->psubFiltTp == NULL)
		{
			gpnLog(GPN_LOG_L_ERR, "%s : malloc err\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}
		
		/*init memrey : init memrey 'psubReportNode' */
		psubFiltTpNode->scanType = psubFiltTpLine->scanType;
		/*init memrey : init memrey 'psubReportTp' */
		psubFiltTpNode->psubFiltTp->subNum = subTpNum;
		psubFiltTpNode->psubFiltTp->psubFilt =\
			(subFiltTpUnit *)(psubFiltTpNode->psubFiltTp + 1);
		psubFiltUnit = psubFiltTpNode->psubFiltTp->psubFilt;
		for(i=0;i<subTpNum;i++)
		{
			/*just need set sub type to NULL */ 
			psubFiltUnit[i].subType = GPN_STAT_SUB_TYPE_INVALID;
			psubFiltUnit[i].status = GPN_STAT_DBS_GEN_ENABLE;
		}

		listDelete(&(psubFiltTpInfo->idleSubFiltTpQuen), (NODE *)psubFiltTpNode);
		psubFiltTpInfo->idleSubFiltTpNum--;
			
		listAdd(&(psubFiltTpInfo->actSubFiltTpQuen[hash]), (NODE *)psubFiltTpNode);
		psubFiltTpInfo->actSubFiltTpNum[hash]++;
	}
	else
	{
		/*just already find  'psubReportTpNode' */
	}

	/*add new unit : find subType's order & add new unit in line */
	psubFiltUnit = psubFiltTpNode->psubFiltTp->psubFilt;
	if(gpnStatTypeGetSubTpOrderBaseScanType(psubFiltTpLine->subType,\
					psubFiltTpLine->scanType, &order) == GPN_STAT_DBS_GEN_OK)
	{
		psubFiltUnit[order].subType = psubFiltTpLine->subType;
		psubFiltUnit[order].status = psubFiltTpLine->status;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s :scanType(%08x)'s subType(%08x) subFiltTp(%d), err order!\n\r",\
			__FUNCTION__, psubFiltTpLine->scanType, psubFiltTpLine->subType,\
			psubFiltTpNode->subFiltTpId);

		return GPN_STAT_DBS_GEN_ERR;
	}

	/*debug print */
	#if 0
	printf("filtTpId %d scanType %08x subTypeNum %d\n\r",\
		psubFiltTpNode->subFiltTpId, psubFiltTpNode->scanType,\
		psubFiltTpNode->psubFiltTp->subNum);
	for(i=0;i<psubFiltTpNode->psubFiltTp->subNum;i++)
	{
		printf("subType %08x status %d\n",\
			psubFiltTpNode->psubFiltTp->psubFilt[i].subType,\
			psubFiltTpNode->psubFiltTp->psubFilt[i].status);
	}
	#endif
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : add scanType(%08x)'s subType(%08x) in subFiltTp(%d)!\n\r",\
		__FUNCTION__, psubFiltTpLine->scanType, psubFiltTpLine->subType,\
		psubFiltTpLine->subFiltTpId);
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatFiltTpDelCheckPortDel(stStatSubFiltTpNode *pFiltTpNode)
{
	stStatLocalPortNode *pStatLocalNode, *sStatLocalNode;
	stStatScanPortInfo *pStatScanPort;
	stStatPortMonMgt portMonMgt;

	/*assert */
	if( pFiltTpNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatLocalNode = NULL;
	gpnStatSeekFirstValidScanNodeIndex(&pStatLocalNode);
	if (pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	do 
	{
		pStatScanPort = pStatLocalNode->pStatScanPort;
		if (pFiltTpNode->scanType == pStatScanPort->statScanType)
		{
			if(gpnStatPortMonGet(pStatLocalNode, pFiltTpNode->scanType, &portMonMgt) == GPN_STAT_DBS_GEN_OK)
			{
				if ((pStatScanPort->statLCycRec && pFiltTpNode->subFiltTpId == portMonMgt.longCycSubFiltTpId)
					|| (pStatScanPort->statSCycRec && pFiltTpNode->subFiltTpId == portMonMgt.shortCycSubFiltTpId)
					|| (pStatScanPort->statUCycRec && pFiltTpNode->subFiltTpId == portMonMgt.udCycSubFiltTpId))
				{
					gpnLog(GPN_LOG_L_INFO, "%s : subFiltThredTpId(%d) is in use in port moni task!\n\r",\
						__FUNCTION__, pFiltTpNode->subFiltTpId);
					return GPN_STAT_DBS_GEN_OK;
				}
			}
		}

		gpnStatSeekNextValidScanNodeIndex(pStatLocalNode, &sStatLocalNode);
		pStatLocalNode = sStatLocalNode;
		
	} while (pStatLocalNode);

	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatSubFiltTpDelete(UINT32 subFiltTpId)
{
	UINT32 hash;
	UINT32 subFiltTpNum;
	stStatSubFiltTpNode *psubFiltTpNode;
	stStatSubFiltTemp *psubFiltTpInfo;

	psubFiltTpInfo = &(pgstEQUStatProcSpace->statSubFiltTemp);
	
	hash = subFiltTpId%(psubFiltTpInfo->hash);

	subFiltTpNum = 0;
	psubFiltTpNode = (stStatSubFiltTpNode *)listFirst(&(psubFiltTpInfo->actSubFiltTpQuen[hash]));
	while((psubFiltTpNode!=NULL)&&(subFiltTpNum<psubFiltTpInfo->actSubFiltTpNum[hash]))
	{
		if(subFiltTpId == psubFiltTpNode->subFiltTpId)
		{
			/*delete task, first delete port */
//			if (gpnStatFiltTpDelCheckPortDel(psubFiltTpNode) != GPN_STAT_DBS_GEN_OK)
//			{
				listDelete(&(psubFiltTpInfo->actSubFiltTpQuen[hash]), (NODE *)psubFiltTpNode);
				psubFiltTpInfo->actSubFiltTpNum[hash]--;

				listAdd(&(psubFiltTpInfo->idleSubFiltTpQuen), (NODE *)psubFiltTpNode);
				psubFiltTpInfo->idleSubFiltTpNum++;

				/*memrey free */
				if(psubFiltTpNode->psubFiltTp != NULL)
				{
					free(psubFiltTpNode->psubFiltTp);
					psubFiltTpNode->psubFiltTp = NULL;
				}
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : delete subFiltTpId(%d)!\n\r",\
					__FUNCTION__, psubFiltTpNode->subFiltTpId);

				return GPN_STAT_DBS_GEN_OK;
//			}
		}

		subFiltTpNum++;
		psubFiltTpNode = (stStatSubFiltTpNode *)listNext((NODE *)psubFiltTpNode);
	}

	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatSubFiltTpModify(stStatSubFiltTpTable *psubFiltTpLine)
{
	UINT32 hash;
	UINT32 order;
	UINT32 modify;
	UINT32 subTpNum;
	stStatSubFiltTpNode *psubFiltTpNode;
	stStatSubFiltTemp *psubFiltTpInfo;
	subFiltTpUnit *psubFiltUnit;
	
	/*assert */
	if(psubFiltTpLine == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	psubFiltTpInfo = &(pgstEQUStatProcSpace->statSubFiltTemp);
	hash = (psubFiltTpLine->subFiltTpId)%(psubFiltTpInfo->hash);

	/*fix add mode */
	modify = GPN_STAT_DBS_GEN_NO;
	subTpNum = 0;
	psubFiltTpNode = (stStatSubFiltTpNode *)listFirst(&(psubFiltTpInfo->actSubFiltTpQuen[hash]));
	while((psubFiltTpNode!=NULL)&&(subTpNum<psubFiltTpInfo->actSubFiltTpNum[hash]))
	{
		if(psubFiltTpNode->subFiltTpId == psubFiltTpLine->subFiltTpId)
		{	
			/*check modify-able */
			if(gpnStatSubFiltTpSubTypeQuery(psubFiltTpNode,
				psubFiltTpLine->subType, NULL) == GPN_STAT_DBS_GEN_OK)
			{
				modify = GPN_STAT_DBS_GEN_YES;
			}
			
			break;
		}

		subTpNum++;
		psubFiltTpNode = (stStatSubFiltTpNode *)listNext((NODE *)psubFiltTpNode);
	}
	
	/*add new node */
	if(modify == GPN_STAT_DBS_GEN_YES)
	{
		/*find valid sub type, then modify */
		psubFiltUnit = psubFiltTpNode->psubFiltTp->psubFilt;
		if(gpnStatTypeGetSubTpOrderBaseScanType(psubFiltTpLine->subType,\
			psubFiltTpNode->scanType, &order) == GPN_STAT_DBS_GEN_OK)
		{
			/*psubFiltUnit[order].subType = psubFiltTpLine->subType;*/
			psubFiltUnit[order].status = psubFiltTpLine->status;
			
			return GPN_STAT_DBS_GEN_OK;
		}
	}

	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatSubFiltTpGet(UINT32 subFiltTpId, stStatSubFiltTpNode **ppsubFiltTpNode)
{
	UINT32 hash;
	UINT32 subFiltTpNum;
	stStatSubFiltTpNode *ptempSubFiltTpNode;
	stStatSubFiltTemp *psubFiltTpInfo;

	if(ppsubFiltTpNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	psubFiltTpInfo = &(pgstEQUStatProcSpace->statSubFiltTemp);
	
	hash = subFiltTpId%(psubFiltTpInfo->hash);

	subFiltTpNum = 0;
	ptempSubFiltTpNode = (stStatSubFiltTpNode *)listFirst(&(psubFiltTpInfo->actSubFiltTpQuen[hash]));
	while((ptempSubFiltTpNode!=NULL)&&(subFiltTpNum<psubFiltTpInfo->actSubFiltTpNum[hash]))
	{
		if(subFiltTpId == ptempSubFiltTpNode->subFiltTpId)
		{
			/*bingo */
			*ppsubFiltTpNode = ptempSubFiltTpNode;
			
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get subFiltTpId(%d)!\n\r",\
				__FUNCTION__, subFiltTpId);

			return GPN_STAT_DBS_GEN_OK;
		}

		subFiltTpNum++;
		ptempSubFiltTpNode = (stStatSubFiltTpNode *)listNext((NODE *)ptempSubFiltTpNode);
	}

	*ppsubFiltTpNode = NULL;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find subFiltTpId(%d)!\n\r",\
		__FUNCTION__, subFiltTpId);
	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatSubFiltTpSubTypeQuery(stStatSubFiltTpNode *psubFiltTpNode/*in*/,
	UINT32 subType, stStatSubFiltTpTable *psubFiltTpLine/*out*/)
{
	UINT32 i;
	UINT32 seat;

	/*assert */
	if(psubFiltTpNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(subType == GPN_STAT_SUB_TYPE_INVALID)
	{
		seat = 0;
	}
	else
	{
		/*give a invalid seat before for(...) start */
		seat = psubFiltTpNode->psubFiltTp->subNum;
		for(i=0;i<psubFiltTpNode->psubFiltTp->subNum;i++)
		{
			if(subType == psubFiltTpNode->psubFiltTp->psubFilt[i].subType)
			{
				seat = i;
				break;
			}
		}
	}
	
	if(seat >= psubFiltTpNode->psubFiltTp->subNum)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can't find subType(%08x) in subFiltTp(%d)\n\r",\
			__FUNCTION__, subType, psubFiltTpNode->subFiltTpId);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*find valid subType */
	if(psubFiltTpLine != NULL)
	{
		psubFiltTpLine->subFiltTpId = psubFiltTpNode->subFiltTpId;
		psubFiltTpLine->subType = psubFiltTpNode->psubFiltTp->psubFilt[seat].subType;
		psubFiltTpLine->scanType = psubFiltTpNode->scanType;
		psubFiltTpLine->status = psubFiltTpNode->psubFiltTp->psubFilt[seat].status;
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatSubFiltTpGetNext(UINT32 subFiltTpId, stStatSubFiltTpNode **ppsubFiltTpNode, UINT32 *pnextSubFiltTp)
{
	UINT32 hash;
	UINT32 subFiltTpNum;
	UINT32 tmpSubFiltTpId;
	stStatSubFiltTpNode *ptempSubFiltTpNode;
	stStatSubFiltTemp *psubFiltTpInfo;

	/*assert */
	if( (ppsubFiltTpNode == NULL) ||\
		(pnextSubFiltTp == NULL) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	psubFiltTpInfo = &(pgstEQUStatProcSpace->statSubFiltTemp);

	tmpSubFiltTpId = subFiltTpId;
	if(tmpSubFiltTpId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		/*0xFFFFFFFF means template search start, so re-give a search start : must find first tp */
		hash = 0;
		while(hash < psubFiltTpInfo->hash)
		{
			subFiltTpNum = 0;
			ptempSubFiltTpNode = (stStatSubFiltTpNode *)listFirst(&(psubFiltTpInfo->actSubFiltTpQuen[hash]));
			if(ptempSubFiltTpNode != NULL)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get first thredId(%d)!\n\r",\
					__FUNCTION__, ptempSubFiltTpNode->subFiltTpId);
				*ppsubFiltTpNode = ptempSubFiltTpNode;
				tmpSubFiltTpId = ptempSubFiltTpNode->subFiltTpId;
				
				break;
			}

			/*to next quen */
			hash++;
		}
		if(hash >= psubFiltTpInfo->hash)
		{
			*ppsubFiltTpNode = NULL;
			*pnextSubFiltTp = GPN_STAT_INVALID_TEMPLAT_ID;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find first thredId!\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*first is invalid, so next is 'valid first' */
		*pnextSubFiltTp = tmpSubFiltTpId;
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d)!\n\r",\
			__FUNCTION__, *pnextSubFiltTp);
		
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
		hash = tmpSubFiltTpId%(psubFiltTpInfo->hash);

		subFiltTpNum = 0;
		ptempSubFiltTpNode = (stStatSubFiltTpNode *)listFirst(&(psubFiltTpInfo->actSubFiltTpQuen[hash]));
		while((ptempSubFiltTpNode!=NULL)&&(subFiltTpNum<psubFiltTpInfo->actSubFiltTpNum[hash]))
		{
			if(tmpSubFiltTpId == ptempSubFiltTpNode->subFiltTpId)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get this thredId(%d)!\n\r",\
					__FUNCTION__, ptempSubFiltTpNode->subFiltTpId);
				*ppsubFiltTpNode = ptempSubFiltTpNode;
				
				break;
			}

			subFiltTpNum++;
			ptempSubFiltTpNode = (stStatSubFiltTpNode *)listNext((NODE *)ptempSubFiltTpNode);
		}
		if((ptempSubFiltTpNode==NULL)||(subFiltTpNum>=psubFiltTpInfo->actSubFiltTpNum[hash]))
		{
			*ppsubFiltTpNode = NULL;
			*pnextSubFiltTp = GPN_STAT_INVALID_TEMPLAT_ID;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find thredId(%d)!\n\r",\
				__FUNCTION__, tmpSubFiltTpId);
			return GPN_STAT_DBS_GEN_ERR;
		}
	
		/*fast go to find next node, search in old hash thredTp quen*/
		if(ptempSubFiltTpNode != NULL)
		{
			subFiltTpNum++;
			ptempSubFiltTpNode = (stStatSubFiltTpNode *)listNext((NODE *)ptempSubFiltTpNode);
			if((ptempSubFiltTpNode!=NULL)&&(subFiltTpNum<psubFiltTpInfo->actSubFiltTpNum[hash]))
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d) in old quen!\n\r",\
					__FUNCTION__, ptempSubFiltTpNode->subFiltTpId);
				*pnextSubFiltTp = ptempSubFiltTpNode->subFiltTpId;
				
				return GPN_STAT_DBS_GEN_OK;
			}
		}
		
		/*fast go to find next node, search in all-other hash thredTp quen*/
		hash++;
		while(hash < psubFiltTpInfo->hash)
		{
			ptempSubFiltTpNode = (stStatSubFiltTpNode *)listFirst(&(psubFiltTpInfo->actSubFiltTpQuen[hash]));
			if(ptempSubFiltTpNode != NULL)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d) in new quen!\n\r",\
					__FUNCTION__, ptempSubFiltTpNode->subFiltTpId);
				*pnextSubFiltTp = ptempSubFiltTpNode->subFiltTpId;
			
				return GPN_STAT_DBS_GEN_OK;
			}
			
			/*start in new quen */
			hash++;
		}
	}
	
	*pnextSubFiltTp = GPN_STAT_INVALID_TEMPLAT_ID;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not get next thredId!\n\r",\
		__FUNCTION__);
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatSubFiltTpSubTypeQueryNext(stStatSubFiltTpNode *psubFiltTpNode/*in*/,
	UINT32 subFiltTpId, UINT32 subType, stStatSubFiltTpTable *psubFiltTpLine/*out*/,
	UINT32 sNextSubFiltTpId, UINT32 *pnextSubFiltTpId, UINT32 *pnextSubType)
{
	UINT32 i;
	UINT32 seat;

	/*assert */
	if( (psubFiltTpNode == NULL) ||\
		(pnextSubFiltTpId == NULL) ||\
		(pnextSubType == NULL) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(subType == GPN_STAT_SUB_TYPE_INVALID)
	{
		/*in getNext, here is subType find first, so qurey valid 'first', not 'order first' */
		seat = psubFiltTpNode->psubFiltTp->subNum;
		for(i=0;i<psubFiltTpNode->psubFiltTp->subNum;i++)
		{
			if(psubFiltTpNode->psubFiltTp->psubFilt[i].subType !=\
				GPN_STAT_SUB_TYPE_INVALID)
			{
				seat = i;
				break;
			}
		}
	}
	else
	{
		/*give a invalid seat before for(...) start */
		seat = psubFiltTpNode->psubFiltTp->subNum;
		for(i=0;i<psubFiltTpNode->psubFiltTp->subNum;i++)
		{
			if(subType == psubFiltTpNode->psubFiltTp->psubFilt[i].subType)
			{
				seat = i;
				break;
			}
		}
	}
	
	if(seat >= psubFiltTpNode->psubFiltTp->subNum)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can't find subType(%08x) in subFiltTp(%d)\n\r",\
			__FUNCTION__, subType, psubFiltTpNode->subFiltTpId);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*find valid subType */
	if(psubFiltTpLine != NULL)
	{
		psubFiltTpLine->subFiltTpId = psubFiltTpNode->subFiltTpId;
		psubFiltTpLine->subType = psubFiltTpNode->psubFiltTp->psubFilt[seat].subType;
		psubFiltTpLine->scanType = psubFiltTpNode->scanType;
		psubFiltTpLine->status = psubFiltTpNode->psubFiltTp->psubFilt[seat].status;
	}

	/*find next valid subType */
	if(subFiltTpId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		/*means first get next , use this index for next index */
		*pnextSubType = psubFiltTpNode->psubFiltTp->psubFilt[seat].subType;;
		*pnextSubFiltTpId = psubFiltTpNode->subFiltTpId;
	}
	else
	{
		for(seat += 1;seat<psubFiltTpNode->psubFiltTp->subNum;seat++)
		{
			if(psubFiltTpNode->psubFiltTp->psubFilt[seat].subType != GPN_STAT_SUB_TYPE_INVALID) 
			{
				/*find valid next subType in same line*/
				*pnextSubType = psubFiltTpNode->psubFiltTp->psubFilt[seat].subType;
				/*next valid still in same line */
				*pnextSubFiltTpId = psubFiltTpNode->subFiltTpId;

				break;
			}
		}
		
		if(seat >= psubFiltTpNode->psubFiltTp->subNum)
		{
			/*next valid in next line */
			*pnextSubFiltTpId = sNextSubFiltTpId;
			/*not find valid next subType in same line */
			*pnextSubType = GPN_STAT_SUB_TYPE_INVALID;
		}
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s :statiThredTp(%d) thredTp(%d) subType(%08x) goto next thredTp(%d) subType(%08x)\n\r",\
		__FUNCTION__, sNextSubFiltTpId, psubFiltTpNode->subFiltTpId, subType,\
		*pnextSubFiltTpId, *pnextSubType);
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatSubFiltTpSubTypeQueryNextAdjust(UINT32 *pnextSubFiltTpId, UINT32 *pnextSubType)
{
	stStatSubFiltTpNode *pthredTpNode;
	//subFiltTpUnit *psubFiltTp;
	UINT32 i;
	
	/*assert */
	if( (pnextSubFiltTpId == NULL) ||\
		(pnextSubType == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(*pnextSubFiltTpId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		*pnextSubType = GPN_STAT_SUB_TYPE_INVALID;
		
		return GPN_STAT_DBS_GEN_OK;
	}

	if(*pnextSubType == GPN_STAT_SUB_TYPE_INVALID)
	{
		pthredTpNode = NULL;
		gpnStatSubFiltTpGet(*pnextSubFiltTpId, &pthredTpNode);
		if(pthredTpNode == NULL)
		{
			return GPN_STAT_DBS_GEN_ERR;
		}

		//psubFiltTp = pthredTpNode->psubFiltTp->psubFilt;
		for(i=0;i<pthredTpNode->psubFiltTp->subNum;i++)
		{
			if(pthredTpNode->psubFiltTp->psubFilt[i].subType != GPN_STAT_SUB_TYPE_INVALID)
			{
				*pnextSubType = pthredTpNode->psubFiltTp->psubFilt[i].subType;

				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : adjust to nextThredTp(%d) nextSubType(%08x)\n\r",\
					__FUNCTION__, *pnextSubFiltTpId, *pnextSubType);
				break;
			}
		}

		if(i >= pthredTpNode->psubFiltTp->subNum)
		{
			return GPN_STAT_DBS_GEN_ERR;
		}
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatSubReportTpAdd(stStatSubReportTpTable *psubReportTpLine)
{
	UINT32 i;
	UINT32 hash;
	UINT32 order;
	UINT32 addMode;
	UINT32 subTpNum;
	stStatSubReportTpNode *psubReportTpNode;
	stStatSubReportTemp *psubReportTpInfo;
	stStatScanTypeDef *pStatScanTpNode;
	subReportTpUnit *psubReportUnit;
	
	/*assert */
	if(psubReportTpLine == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	/*assert subReportId */
	if( (psubReportTpLine->subReportTpId == 0)  ||\
		(psubReportTpLine->subReportTpId == GPN_STAT_INVALID_TEMPLAT_ID) )
	{
		gpnLog(GPN_LOG_L_ERR, "%s : subReportTpId(%d) err!\n\r",\
			__FUNCTION__, psubReportTpLine->subReportTpId);
		return GPN_STAT_DBS_GEN_ERR;
	}

	psubReportTpInfo = &(pgstEQUStatProcSpace->statSubReportTemp);
	hash = (psubReportTpLine->subReportTpId)%(psubReportTpInfo->hash);

	/*fix add mode */
	subTpNum = 0;
	psubReportTpNode = (stStatSubReportTpNode *)listFirst(&(psubReportTpInfo->actSubReportTpQuen[hash]));
	while((psubReportTpNode!=NULL)&&(subTpNum<psubReportTpInfo->actSubReportTpNum[hash]))
	{
		if(psubReportTpNode->subReportTpId == psubReportTpLine->subReportTpId)
		{	
			/*already add lin, find subType */
			if(gpnStatSubReportTpSubTypeQuery(psubReportTpNode,
				psubReportTpLine->subType, NULL) == GPN_STAT_DBS_GEN_OK)
			{
				addMode = GPN_STAT_DBS_ADD_NEW_UNIT;
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : subReportId(%d) subType(%08x) already creat!\n\r",\
					__FUNCTION__, psubReportTpLine->subReportTpId, psubReportTpLine->subType);
			
				return GPN_STAT_DBS_GEN_ERR;
			}
			else
			{
				addMode = GPN_STAT_DBS_ADD_NEW_UNIT;
			}

			break;
		}

		subTpNum++;
		psubReportTpNode = (stStatSubReportTpNode *)listNext((NODE *)psubReportTpNode);
	}
	if((psubReportTpNode==NULL)||(subTpNum>=psubReportTpInfo->actSubReportTpNum[hash]))
	{
		addMode = GPN_STAT_DBS_ADD_NEW_LINE;
	}
	
	/*add new node */
	if(addMode == GPN_STAT_DBS_ADD_NEW_LINE)
	{
		/*check resouce ok */
		if(psubReportTpInfo->idleSubReportTpNum == 0)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : do not have idleNode(%d)!\n\r",\
				__FUNCTION__, psubReportTpInfo->idleSubReportTpNum);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*find right idle node */
		subTpNum = 0;
		psubReportTpNode = (stStatSubReportTpNode *)listFirst(&(psubReportTpInfo->idleSubReportTpQuen));
		while((psubReportTpNode!=NULL)&&(subTpNum<psubReportTpInfo->idleSubReportTpNum))
		{
			if(psubReportTpNode->subReportTpId == psubReportTpLine->subReportTpId)
			{
				break;
			}
			
			subTpNum++;
			psubReportTpNode = (stStatSubReportTpNode *)listNext((NODE *)psubReportTpNode);
		}
		if((psubReportTpNode==NULL)||(subTpNum>=psubReportTpInfo->idleSubReportTpNum))
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : don't find subReportId(%d) in idle list!\n\r",\
				__FUNCTION__, psubReportTpLine->subReportTpId);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*init memrey : calculate space size */
		pStatScanTpNode = NULL;
		gpnStatTypeGetScanTypeNode(psubReportTpLine->scanType, &pStatScanTpNode);
		if(pStatScanTpNode == NULL)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : get scanType(%08x) node err!\n\r",\
				__FUNCTION__, psubReportTpLine->scanType);
			return GPN_STAT_DBS_GEN_ERR;
		}

		subTpNum = pStatScanTpNode->statSubTpNumInScanType;
		i = sizeof(subReportTp) + sizeof(subReportTpUnit)*subTpNum;
		psubReportTpNode->psubReportTp = (subReportTp *)malloc(i);
		if(psubReportTpNode->psubReportTp == NULL)
		{
			gpnLog(GPN_LOG_L_ERR, "%s : malloc err\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}
		
		/*init memrey : init memrey 'psubReportNode' */
		psubReportTpNode->scanType = psubReportTpLine->scanType;
		/*init memrey : init memrey 'psubReportTp' */
		psubReportTpNode->psubReportTp->subNum = subTpNum;
		psubReportTpNode->psubReportTp->psubReport =\
			(subReportTpUnit *)(psubReportTpNode->psubReportTp + 1);
		psubReportUnit = psubReportTpNode->psubReportTp->psubReport;
		for(i=0;i<subTpNum;i++)
		{
			/*just need set sub type to NULL */ 
			psubReportUnit[i].subType = GPN_STAT_SUB_TYPE_INVALID;
			psubReportUnit[i].status = GPN_STAT_DBS_GEN_DISABLE;
		}

		listDelete(&(psubReportTpInfo->idleSubReportTpQuen), (NODE *)psubReportTpNode);
		psubReportTpInfo->idleSubReportTpNum--;
			
		listAdd(&(psubReportTpInfo->actSubReportTpQuen[hash]), (NODE *)psubReportTpNode);
		psubReportTpInfo->actSubReportTpNum[hash]++;
	}
	else
	{
		/*just already find  'psubReportTpNode' */
	}

	/*add new unit : find subType's order & add new unit in line */
	psubReportUnit = psubReportTpNode->psubReportTp->psubReport;
	if(gpnStatTypeGetSubTpOrderBaseScanType(psubReportTpLine->subType,\
					psubReportTpLine->scanType, &order) == GPN_STAT_DBS_GEN_OK)
	{
		psubReportUnit[order].subType = psubReportTpLine->subType;
		psubReportUnit[order].status = psubReportTpLine->status;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : scanType(%08x)'s subType(%08x) subReportTp(%d), err order!\n\r",\
			__FUNCTION__, psubReportTpLine->scanType, psubReportTpLine->subType,\
			psubReportTpNode->subReportTpId);

		return GPN_STAT_DBS_GEN_ERR;
	}

	/*debug print */
	#if 0
	printf("reportTpId %d scanType %08x subTypeNum %d\n\r",\
		psubReportTpNode->subReportTpId, psubReportTpNode->scanType,\
		psubReportTpNode->psubReportTp->subNum);
	for(i=0;i<psubReportTpNode->psubReportTp->subNum;i++)
	{
		printf("subType %08x status %d\n",\
			psubReportTpNode->psubReportTp->psubReport[i].subType,\
			psubReportTpNode->psubReportTp->psubReport[i].status);
	}
	#endif
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : add scanType(%08x)'s subType(%08x) in subReportTp(%d)!\n\r",\
		__FUNCTION__, psubReportTpLine->scanType, psubReportTpLine->subType,\
		psubReportTpNode->subReportTpId);
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatReportTpDelCheckPortDel(stStatSubReportTpNode *psubReportTpNode)
{
	stStatLocalPortNode *pStatLocalNode, *sStatLocalNode;
	stStatScanPortInfo *pStatScanPort;
	stStatPortMonMgt portMonMgt;
	stStatTaskNode *pTaskNode;

	/*assert */
	if( psubReportTpNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatLocalNode = NULL;
	gpnStatSeekFirstValidScanNodeIndex(&pStatLocalNode);
	if (pStatLocalNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	do 
	{
		pStatScanPort = pStatLocalNode->pStatScanPort;
		if (psubReportTpNode->scanType == pStatScanPort->statScanType)
		{
			if(gpnStatPortMonGet(pStatLocalNode, psubReportTpNode->scanType, &portMonMgt) == GPN_STAT_DBS_GEN_OK)
			{
				if (pStatScanPort->statLCycRec)
				{
					pTaskNode = pStatScanPort->statLCycRec->ptaskNode;
					if (pTaskNode)
					{
						if(pTaskNode->actMOnOBjNum != 0 && psubReportTpNode->subReportTpId == pTaskNode->subReportTpId)
						{
							gpnLog(GPN_LOG_L_INFO, "%s : subReportThredTpId(%d) is in use in port moni task!\n\r",\
									__FUNCTION__, psubReportTpNode->subReportTpId);
							return GPN_STAT_DBS_GEN_OK;
						}
					}
				}

				if (pStatScanPort->statSCycRec)
				{
					pTaskNode = pStatScanPort->statSCycRec->ptaskNode;
					if (pTaskNode)
					{
						if(pTaskNode->actMOnOBjNum != 0 && psubReportTpNode->subReportTpId == pTaskNode->subReportTpId)
						{
							gpnLog(GPN_LOG_L_INFO, "%s : subReporthredTpId(%d) is in use in port moni task!\n\r",\
									__FUNCTION__, psubReportTpNode->subReportTpId);
							return GPN_STAT_DBS_GEN_OK;
						}
					}
				}

				if (pStatScanPort->statUCycRec)
				{
					pTaskNode = pStatScanPort->statUCycRec->ptaskNode;
					if (pTaskNode)
					{
						if(pTaskNode->actMOnOBjNum != 0 && psubReportTpNode->subReportTpId == pTaskNode->subReportTpId)
						{
							gpnLog(GPN_LOG_L_INFO, "%s : subReportThredTpId(%d) is in use in port moni task!\n\r",\
									__FUNCTION__, psubReportTpNode->subReportTpId);
							return GPN_STAT_DBS_GEN_OK;
						}
					}
				}
			}
		}

		gpnStatSeekNextValidScanNodeIndex(pStatLocalNode, &sStatLocalNode);
		pStatLocalNode = sStatLocalNode;
		
	} while (pStatLocalNode);

	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatSubReportTpDelete(UINT32 subReportTpId)
{
	UINT32 hash;
	UINT32 subReportTpNum;
	stStatSubReportTpNode *psubReportTpNode;
	stStatSubReportTemp *psubReportTpInfo;

	psubReportTpInfo = &(pgstEQUStatProcSpace->statSubReportTemp);
	
	hash = subReportTpId%(psubReportTpInfo->hash);

	subReportTpNum = 0;
	psubReportTpNode = (stStatSubReportTpNode *)listFirst(&(psubReportTpInfo->actSubReportTpQuen[hash]));
	while((psubReportTpNode!=NULL)&&(subReportTpNum<psubReportTpInfo->actSubReportTpNum[hash]))
	{
		if(subReportTpId == psubReportTpNode->subReportTpId)
		{
			/*delete task, first delete port */
//			if (gpnStatReportTpDelCheckPortDel(psubReportTpNode) != GPN_STAT_DBS_GEN_OK)
//			{
				listDelete(&(psubReportTpInfo->actSubReportTpQuen[hash]), (NODE *)psubReportTpNode);
				psubReportTpInfo->actSubReportTpNum[hash]--;

				listAdd(&(psubReportTpInfo->idleSubReportTpQuen), (NODE *)psubReportTpNode);
				psubReportTpInfo->idleSubReportTpNum++;

				/*memrey free */
				if(psubReportTpNode->psubReportTp != NULL)
				{
					free(psubReportTpNode->psubReportTp);
					psubReportTpNode->psubReportTp = NULL;
				}
				gpnLog(GPN_LOG_L_INFO, "%s : delete subReport(%d) sucess!\n\r",\
							__FUNCTION__, psubReportTpNode->subReportTpId);

				return GPN_STAT_DBS_GEN_OK;
//			}
		}

		subReportTpNum++;
		psubReportTpNode = (stStatSubReportTpNode *)listNext((NODE *)psubReportTpNode);
	}

	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatSubReportTpModify(stStatSubReportTpTable *psubReportTpLine)
{
	UINT32 hash;
	UINT32 order;
	UINT32 modify;
	UINT32 subTpNum;
	stStatSubReportTpNode *psubReportTpNode;
	stStatSubReportTemp *psubReportTpInfo;
	subReportTpUnit *psubReportUnit;
	
	/*assert */
	if(psubReportTpLine == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	psubReportTpInfo = &(pgstEQUStatProcSpace->statSubReportTemp);
	hash = (psubReportTpLine->subReportTpId)%(psubReportTpInfo->hash);

	/*fix add mode */
	modify = GPN_STAT_DBS_GEN_NO;
	subTpNum = 0;
	psubReportTpNode = (stStatSubReportTpNode *)listFirst(&(psubReportTpInfo->actSubReportTpQuen[hash]));
	while((psubReportTpNode!=NULL)&&(subTpNum<psubReportTpInfo->actSubReportTpNum[hash]))
	{
		if(psubReportTpNode->subReportTpId == psubReportTpLine->subReportTpId)
		{	
			/*check modify-able */
			if(gpnStatSubReportTpSubTypeQuery(psubReportTpNode,
				psubReportTpLine->subType, NULL) == GPN_STAT_DBS_GEN_OK)
			{
				modify = GPN_STAT_DBS_GEN_YES;
			}
			
			break;
		}

		subTpNum++;
		psubReportTpNode = (stStatSubReportTpNode *)listNext((NODE *)psubReportTpNode);
	}
	
	/*add new node */
	if(modify == GPN_STAT_DBS_GEN_YES)
	{
		/*find valid sub type, then modify */
		psubReportUnit = psubReportTpNode->psubReportTp->psubReport;
		if(gpnStatTypeGetSubTpOrderBaseScanType(psubReportTpLine->subType,\
					psubReportTpNode->scanType, &order) == GPN_STAT_DBS_GEN_OK)
		{
			/*psubReportUnit[order].subType = psubReportTpLine->subType;*/
			psubReportUnit[order].status = psubReportTpLine->status;
			
			return GPN_STAT_DBS_GEN_OK;
		}
	}

	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatSubReportTpGet(UINT32 subReportTpId, stStatSubReportTpNode **ppsubReportTpNode)
{
	UINT32 hash;
	UINT32 subReportTpNum;
	stStatSubReportTpNode *ptempSubReportTpNode;
	stStatSubReportTemp *psubReportTpInfo;

	if(ppsubReportTpNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	psubReportTpInfo = &(pgstEQUStatProcSpace->statSubReportTemp);
	
	hash = subReportTpId%(psubReportTpInfo->hash);

	subReportTpNum = 0;
	ptempSubReportTpNode = (stStatSubReportTpNode *)listFirst(&(psubReportTpInfo->actSubReportTpQuen[hash]));
	while((ptempSubReportTpNode!=NULL)&&(subReportTpNum<psubReportTpInfo->actSubReportTpNum[hash]))
	{
		if(subReportTpId == ptempSubReportTpNode->subReportTpId)
		{
			/*bingo */
			*ppsubReportTpNode = ptempSubReportTpNode;
			
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get subReportTpId(%d)!\n\r",\
				__FUNCTION__, subReportTpId);

			return GPN_STAT_DBS_GEN_OK;
		}

		subReportTpNum++;
		ptempSubReportTpNode = (stStatSubReportTpNode *)listNext((NODE *)ptempSubReportTpNode);
	}

	*ppsubReportTpNode = NULL;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find subReportTpId(%d)!\n\r",\
		__FUNCTION__, subReportTpId);
	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatSubReportTpSubTypeQuery(stStatSubReportTpNode *psubReportTpNode/*in*/,
	UINT32 subType, stStatSubReportTpTable *psubReportTpLine/*out*/)
{
	UINT32 i;
	UINT32 seat;

	/*assert */
	if(psubReportTpNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(subType == GPN_STAT_SUB_TYPE_INVALID)
	{
		seat = 0;
	}
	else
	{
		/*give a invalid seat before for(...) start */
		seat = psubReportTpNode->psubReportTp->subNum;
		for(i=0;i<psubReportTpNode->psubReportTp->subNum;i++)
		{
			if(subType == psubReportTpNode->psubReportTp->psubReport[i].subType)
			{
				seat = i;
				break;
			}
		}
	}
	
	if(seat >= psubReportTpNode->psubReportTp->subNum)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can't find subType(%08x) in subReportTp(%d)\n\r",\
			__FUNCTION__, subType, psubReportTpNode->subReportTpId);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*find valid subType */
	if(psubReportTpLine != NULL)
	{
		psubReportTpLine->subReportTpId = psubReportTpNode->subReportTpId;
		psubReportTpLine->subType = psubReportTpNode->psubReportTp->psubReport[seat].subType;
		psubReportTpLine->scanType = psubReportTpNode->scanType;
		psubReportTpLine->status = psubReportTpNode->psubReportTp->psubReport[seat].status;
	}
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatSubReportTpGetNext(UINT32 subReportTpId, stStatSubReportTpNode **ppsubReportTpNode, UINT32 *pnextSubReportTp)
{
	UINT32 hash;
	UINT32 subReportTpNum;
	UINT32 tmpSubReportTpId;
	stStatSubReportTpNode *ptempSubReportTpNode;
	stStatSubReportTemp *psubReportTpInfo;

	/*assert */
	if( (ppsubReportTpNode == NULL) ||\
		(pnextSubReportTp == NULL) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	psubReportTpInfo = &(pgstEQUStatProcSpace->statSubReportTemp);

	tmpSubReportTpId = subReportTpId;
	if(tmpSubReportTpId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		/*0xFFFFFFFF means template search start, so re-give a search start : must find first tp */
		hash = 0;
		while(hash < psubReportTpInfo->hash)
		{
			subReportTpNum = 0;
			ptempSubReportTpNode = (stStatSubReportTpNode *)listFirst(&(psubReportTpInfo->actSubReportTpQuen[hash]));
			if(ptempSubReportTpNode != NULL)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get first thredId(%d)!\n\r",\
					__FUNCTION__, ptempSubReportTpNode->subReportTpId);
				*ppsubReportTpNode = ptempSubReportTpNode;
				tmpSubReportTpId = ptempSubReportTpNode->subReportTpId;
				
				break;
			}

			/*to next quen */
			hash++;
		}
		if(hash >= psubReportTpInfo->hash)
		{
			*ppsubReportTpNode = NULL;
			*pnextSubReportTp = GPN_STAT_INVALID_TEMPLAT_ID;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find first thredId!\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*first is invalid, so next is 'valid first' */
		*pnextSubReportTp = tmpSubReportTpId;
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d)!\n\r",\
			__FUNCTION__, *pnextSubReportTp);
		
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
		hash = tmpSubReportTpId%(psubReportTpInfo->hash);

		subReportTpNum = 0;
		ptempSubReportTpNode = (stStatSubReportTpNode *)listFirst(&(psubReportTpInfo->actSubReportTpQuen[hash]));
		while((ptempSubReportTpNode!=NULL)&&(subReportTpNum<psubReportTpInfo->actSubReportTpNum[hash]))
		{
			if(tmpSubReportTpId == ptempSubReportTpNode->subReportTpId)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get this thredId(%d)!\n\r",\
					__FUNCTION__, ptempSubReportTpNode->subReportTpId);
				*ppsubReportTpNode = ptempSubReportTpNode;
				
				break;
			}

			subReportTpNum++;
			ptempSubReportTpNode = (stStatSubReportTpNode *)listNext((NODE *)ptempSubReportTpNode);
		}
		if((ptempSubReportTpNode==NULL)||(subReportTpNum>=psubReportTpInfo->actSubReportTpNum[hash]))
		{
			*ppsubReportTpNode = NULL;
			*pnextSubReportTp = GPN_STAT_INVALID_TEMPLAT_ID;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find thredId(%d)!\n\r",\
				__FUNCTION__, tmpSubReportTpId);
			return GPN_STAT_DBS_GEN_ERR;
		}
	
		/*fast go to find next node, search in old hash thredTp quen*/
		if(ptempSubReportTpNode != NULL)
		{
			subReportTpNum++;
			ptempSubReportTpNode = (stStatSubReportTpNode *)listNext((NODE *)ptempSubReportTpNode);
			if((ptempSubReportTpNode!=NULL)&&(subReportTpNum<psubReportTpInfo->actSubReportTpNum[hash]))
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d) in old quen!\n\r",\
					__FUNCTION__, ptempSubReportTpNode->subReportTpId);
				*pnextSubReportTp = ptempSubReportTpNode->subReportTpId;
				
				return GPN_STAT_DBS_GEN_OK;
			}
		}
		
		/*fast go to find next node, search in all-other hash thredTp quen*/
		hash++;
		while(hash < psubReportTpInfo->hash)
		{
			ptempSubReportTpNode = (stStatSubReportTpNode *)listFirst(&(psubReportTpInfo->actSubReportTpQuen[hash]));
			if(ptempSubReportTpNode != NULL)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next thredId(%d) in new quen!\n\r",\
					__FUNCTION__, ptempSubReportTpNode->subReportTpId);
				*pnextSubReportTp = ptempSubReportTpNode->subReportTpId;
			
				return GPN_STAT_DBS_GEN_OK;
			}
			
			/*start in new quen */
			hash++;
		}
	}
	
	*pnextSubReportTp = GPN_STAT_INVALID_TEMPLAT_ID;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not get next thredId!\n\r",\
		__FUNCTION__);
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatSubReportTpSubTypeQueryNext(stStatSubReportTpNode *psubReportTpNode/*in*/,
	UINT32 subReportTpId, UINT32 subType, stStatSubReportTpTable *psubReportTpLine/*out*/,
	UINT32 sNextSubReportTpId, UINT32 *pnextSubReportTpId, UINT32 *pnextSubType)
{
	UINT32 i;
	UINT32 seat;

	/*assert */
	if( (psubReportTpNode == NULL) ||\
		(pnextSubReportTpId == NULL) ||\
		(pnextSubType == NULL) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(subType == GPN_STAT_SUB_TYPE_INVALID)
	{
		/*in getNext, here is subType find first, so qurey valid 'first', not 'order first' */
		seat = psubReportTpNode->psubReportTp->subNum;
		for(i=0;i<psubReportTpNode->psubReportTp->subNum;i++)
		{
			if(psubReportTpNode->psubReportTp->psubReport[i].subType !=\
				GPN_STAT_SUB_TYPE_INVALID)
			{
				seat = i;
				break;
			}
		}
	}
	else
	{
		/*give a invalid seat before for(...) start */
		seat = psubReportTpNode->psubReportTp->subNum;
		for(i=0;i<psubReportTpNode->psubReportTp->subNum;i++)
		{
			if(subType == psubReportTpNode->psubReportTp->psubReport[i].subType)
			{
				seat = i;
				break;
			}
		}
	}
	
	if(seat >= psubReportTpNode->psubReportTp->subNum)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can't find subType(%08x) in subReportTp(%d)\n\r",\
			__FUNCTION__, subType, psubReportTpNode->subReportTpId);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*find valid subType */
	if(psubReportTpLine != NULL)
	{
		psubReportTpLine->subReportTpId = psubReportTpNode->subReportTpId;
		psubReportTpLine->subType = psubReportTpNode->psubReportTp->psubReport[seat].subType;
		psubReportTpLine->scanType = psubReportTpNode->scanType;
		psubReportTpLine->status = psubReportTpNode->psubReportTp->psubReport[seat].status;
	}

	/*find next valid subType */
	if(subReportTpId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		/*means first get next , use this index for next index */
		*pnextSubType = psubReportTpNode->psubReportTp->psubReport[seat].subType;;
		*pnextSubReportTpId = psubReportTpNode->subReportTpId;
	}
	else
	{
		for(seat += 1;seat<psubReportTpNode->psubReportTp->subNum;seat++)
		{
			if(psubReportTpNode->psubReportTp->psubReport[seat].subType != GPN_STAT_SUB_TYPE_INVALID) 
			{
				/*find valid next subType in same line*/
				*pnextSubType = psubReportTpNode->psubReportTp->psubReport[seat].subType;
				/*next valid still in same line */
				*pnextSubReportTpId = psubReportTpNode->subReportTpId;

				break;
			}
		}
		
		if(seat >= psubReportTpNode->psubReportTp->subNum)
		{
			/*next valid in next line */
			*pnextSubReportTpId = sNextSubReportTpId;
			/*not find valid next subType in same line */
			*pnextSubType = GPN_STAT_SUB_TYPE_INVALID;
		}
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s :statiThredTp(%d) thredTp(%d) subType(%08x) goto next thredTp(%d) subType(%08x)\n\r",\
		__FUNCTION__, sNextSubReportTpId, psubReportTpNode->subReportTpId, subType,\
		*pnextSubReportTpId, *pnextSubType);
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatSubReportTpSubTypeQueryNextAdjust(UINT32 *pnextSubReportTpId, UINT32 *pnextSubType)
{
	stStatSubReportTpNode *pthredTpNode;
	//subReportTpUnit *psubReportTp;
	UINT32 i;
	
	/*assert */
	if( (pnextSubReportTpId == NULL) ||\
		(pnextSubType == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(*pnextSubReportTpId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		*pnextSubType = GPN_STAT_SUB_TYPE_INVALID;
		
		return GPN_STAT_DBS_GEN_OK;
	}

	if(*pnextSubType == GPN_STAT_SUB_TYPE_INVALID)
	{
		pthredTpNode = NULL;
		gpnStatSubReportTpGet(*pnextSubReportTpId, &pthredTpNode);
		if(pthredTpNode == NULL)
		{
			return GPN_STAT_DBS_GEN_ERR;
		}

		//psubReportTp = pthredTpNode->psubReportTp->psubReport;
		for(i=0;i<pthredTpNode->psubReportTp->subNum;i++)
		{
			if(pthredTpNode->psubReportTp->psubReport[i].subType != GPN_STAT_SUB_TYPE_INVALID)
			{
				*pnextSubType = pthredTpNode->psubReportTp->psubReport[i].subType;

				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : adjust to nextThredTp(%d) nextSubType(%08x)\n\r",\
					__FUNCTION__, *pnextSubReportTpId, *pnextSubType);
				break;
			}
		}

		if(i >= pthredTpNode->psubReportTp->subNum)
		{
			return GPN_STAT_DBS_GEN_ERR;
		}
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatMonObjSubFiltTpCfg(objLogicDesc *pmonObjInfo, UINT32 cycClass, void *subFiltTp)
{
	stStatScanPortInfo *pStatScanPort;
	stStatLocalPortNode *pStatLocalNode;

	/*assert */
	if( (pmonObjInfo == NULL) ||\
		(subFiltTp == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pStatLocalNode = NULL;
	gpnStatSeekLocalPort2LocalPortNode(pmonObjInfo, &pStatLocalNode);
	if(pStatLocalNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : dev(%08x) port(%08x) can't find perScanNode!\n\r",\
			__FUNCTION__, pmonObjInfo->devIndex, pmonObjInfo->portIndex);
		return GPN_STAT_DBS_GEN_OK;
	}

	pStatScanPort = pStatLocalNode->pStatScanPort;

	if(cycClass == GPN_STAT_CYC_CLASS_LONG)
	{
		pStatScanPort->statLCycRec->subFiltInfo = subFiltTp;
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_SHORT)
	{
		pStatScanPort->statLCycRec->subFiltInfo = subFiltTp;
	}
	else if(cycClass == GPN_STAT_CYC_CLASS_USR)
	{
		pStatScanPort->statLCycRec->subFiltInfo = subFiltTp;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : dev(%08x) port(%08x) cycClass(%d) err!\n\r",\
			__FUNCTION__, pmonObjInfo->devIndex, pmonObjInfo->portIndex, cycClass);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatPortMonGet(stStatLocalPortNode *pStatLocalNode,
	UINT32 scanType, stStatPortMonMgt *pportMonMgt)
{
	stStatScanPortInfo *pStatScanPort;
	stStatXCycDataRecd *statXCycRec;

	/*assert */
	if( (pStatLocalNode == NULL) ||\
		(pportMonMgt == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	memcpy(&(pportMonMgt->portIndex), &(pStatLocalNode->localPort), sizeof(objLogicDesc));

	pStatScanPort = pStatLocalNode->pStatScanPort;
	/*scanType proc */
	if(scanType == GPN_STAT_SCAN_TYPE_ALL)
	{
		/*give first scanType*/
		pportMonMgt->scanType = pStatScanPort->statScanType;
	}
	else
	{
		/*check and find valid scanType */
		/*if(scanType != pStatScanPort->statScanType)*/
		pportMonMgt->scanType = pStatScanPort->statScanType;
	}
	
	/*when one portType vs m-scanType, here will be modify */
	pportMonMgt->statMoniEn = pStatScanPort->scanQuenValid;
	pportMonMgt->currStatMoniEn = pStatScanPort->currMonEn;
	pportMonMgt->insAddFlag = pStatScanPort->insAddFlag;
	if(pStatScanPort->statLCycRec != NULL)
	{
		statXCycRec = pStatScanPort->statLCycRec;
		pportMonMgt->longCycStatMoniEn = statXCycRec->en;
		if(statXCycRec->ptaskNode != NULL)
		{
			pportMonMgt->longCycBelongTask = statXCycRec->ptaskNode->taskId;
			pportMonMgt->longCycAlmThredTpId = statXCycRec->ptaskNode->almThredTpId;
		}
		else
		{
			pportMonMgt->longCycBelongTask = 0;
			pportMonMgt->longCycAlmThredTpId = 0;
		}
		pportMonMgt->longCycEvnThredTpId = statXCycRec->xCycEvnThId;
		pportMonMgt->longCycSubFiltTpId = statXCycRec->xCycsubFiltId;
		pportMonMgt->longCycHistReptTpId = statXCycRec->trapEn;
		pportMonMgt->longCycHistDBId = statXCycRec->xhistDataLoc;
	}
	else
	{
		pportMonMgt->longCycStatMoniEn = GPN_STAT_DBS_GEN_DISABLE;
	}
	if(pStatScanPort->statSCycRec != NULL)
	{
		statXCycRec = pStatScanPort->statSCycRec;
		pportMonMgt->shortCycStatMoniEn = statXCycRec->en;
		if(statXCycRec->ptaskNode != NULL)
		{
			pportMonMgt->shortCycBelongTask = statXCycRec->ptaskNode->taskId;
			pportMonMgt->shortCycAlmThredTpId = statXCycRec->ptaskNode->almThredTpId;
		}
		else
		{
			pportMonMgt->shortCycBelongTask = 0;
			pportMonMgt->shortCycAlmThredTpId = 0;
		}
		pportMonMgt->shortCycEvnThredTpId = statXCycRec->xCycEvnThId;
		pportMonMgt->shortCycSubFiltTpId = statXCycRec->xCycsubFiltId;
		pportMonMgt->shortCycHistReptTpId = statXCycRec->trapEn;
		pportMonMgt->shortCycHistDBId = statXCycRec->xhistDataLoc;
	}
	else
	{
		pportMonMgt->shortCycStatMoniEn = GPN_STAT_DBS_GEN_DISABLE;
	}
	if(pStatScanPort->statUCycRec != NULL)
	{
		statXCycRec = pStatScanPort->statUCycRec;
		pportMonMgt->udCycStatMoniEn = statXCycRec->en;
		if(statXCycRec->ptaskNode != NULL)
		{
			pportMonMgt->udCycBelongTask = statXCycRec->ptaskNode->taskId;
			pportMonMgt->udCycAlmThredTpId = statXCycRec->ptaskNode->almThredTpId;
		}
		else
		{
			pportMonMgt->udCycBelongTask = 0;
			pportMonMgt->udCycAlmThredTpId = 0;
		}
		pportMonMgt->udCycEvnThredTpId = statXCycRec->xCycEvnThId;
		pportMonMgt->udCycSubFiltTpId = statXCycRec->xCycsubFiltId;
		pportMonMgt->udCycHistReptTpId = statXCycRec->trapEn;
		pportMonMgt->udCycHistDBId = statXCycRec->xhistDataLoc;
		pportMonMgt->udCycSecs = statXCycRec->ptaskNode->cycSeconds;
	}
	else
	{
		pportMonMgt->udCycStatMoniEn = GPN_STAT_DBS_GEN_DISABLE;
	}

	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatCurrDataClear(objLogicDesc *pmonObjInfo)
{
	stStatLocalPortNode *pStatLocalNode;

	/*assert */
	if(pmonObjInfo == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	pStatLocalNode = NULL;
	gpnStatSeekLocalPort2LocalPortNode(pmonObjInfo, &pStatLocalNode);
	if(pStatLocalNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : dev(%08x) port(%08x) can't find perScanNode!\n\r",\
			__FUNCTION__, pmonObjInfo->devIndex, pmonObjInfo->portIndex);
		return GPN_STAT_DBS_GEN_ERR;
	}

	if (pStatLocalNode->pStatScanPort->statCurrDate != NULL)
	{
		memset(pStatLocalNode->pStatScanPort->statCurrDate, 0,\
			pStatLocalNode->pStatScanPort->dataSize);
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatCurrDataSubTypeQurey(stStatLocalPortNode *pStatLocalNode, UINT32 subType, stStatCurrDataTable *pcurrDataLine)
{
	stStatScanPortInfo *pStatScanPort;
	stStatScanTypeDef *pscanTypeNode;
	UINT32 *psubCurr;
	UINT32 scanType;
	UINT32 order;
	UINT32 off;
	UINT32 i;

	/*assert */
	if( (pStatLocalNode == NULL) ||\
		(pcurrDataLine == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	if(gpnStatTypeSubStatType2ScanType(subType, &scanType) == GPN_STAT_DBS_GEN_ERR)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : subType(%08x) to scanType err!\n\r",\
			__FUNCTION__, subType);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanPort = pStatLocalNode->pStatScanPort;
	pscanTypeNode = pStatScanPort->pscanTypeNode;

	/*use scanType find right curr data */
	/*xxxScanTypeXCurrData();*/
	/*scanType*/
	if(scanType != pscanTypeNode->statScanType)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : scanType(%08x) err!\n\r",\
			__FUNCTION__, scanType);
		return GPN_STAT_DBS_GEN_ERR;
	}
	if(pStatScanPort->statCurrDate == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : curr mon disable, err!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	if(gpnStatTypeGetSubTpOrderBaseScanType(subType,\
			scanType, &order) != GPN_STAT_DBS_GEN_OK)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : subType(%08x) get order err!\n\r",\
			__FUNCTION__, subType);
		return GPN_STAT_DBS_GEN_ERR;
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : subType(%08x) order(%d) total(%d)!\n\r",\
		__FUNCTION__, subType, order, pscanTypeNode->statSubTpNumInScanType);
	/*find right subType data place */
	off = 0;
    for(i=0;i<pscanTypeNode->statSubTpNumInScanType;i++)
    {
		if(i == order)
		{
			memcpy(&(pcurrDataLine->viewPort), &(pStatLocalNode->viewPort), sizeof(optObjOrient));
			pcurrDataLine->subType = subType;
			pcurrDataLine->scanType = scanType;
			
			psubCurr = (UINT32 *)((UINT8 *)(pStatScanPort->statCurrDate) + off*sizeof(UINT32));
			if(pscanTypeNode->subBitDeep & (1U<<i))
			{
				/*64 bit */
				pcurrDataLine->currDataH32 = *psubCurr;
				pcurrDataLine->currDataL32 = *(psubCurr + 1);
			}
			else
			{
				pcurrDataLine->currDataH32 = 0;
				pcurrDataLine->currDataL32 = *psubCurr;
			}

			return GPN_STAT_DBS_GEN_OK;
		}
		else
		{
			if(pscanTypeNode->subBitDeep & (1U<<i))
			{
				/*64 bit */
				off +=2;
			}
			else
			{
				/*32 bit */
				off +=1;
			}
		}
    }

	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatCurrDataSubTypeQureyNext(stStatLocalPortNode *pStatLocalNode, UINT32 subType,
 	stStatCurrDataTable *pcurrDataLine, objLogicDesc *psNextIndex,
 	objLogicDesc *pnextPIndex, UINT32 *psNextSubType)
{
	stStatScanPortInfo *pStatScanPort;
	stStatScanTypeDef *pscanTypeNode;
	UINT32 *psubCurr;
	UINT32 scanType;
	UINT32 order;
	UINT32 off;
	UINT32 i;
	UINT32 subTypeRecord;
	
	/*assert */
	if( (pStatLocalNode == NULL) ||\
		(pcurrDataLine == NULL) ||\
		(psNextIndex == NULL) ||\
		(pnextPIndex == NULL) ||\
		(psNextSubType == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*subType valid check */
	subTypeRecord = subType;
	if(subType == GPN_STAT_SUB_TYPE_INVALID)
	{
		if(gpnStatSeekFirstSubTypeInPortScanNode(pStatLocalNode, &subType) == GPN_STAT_DBS_GEN_ERR)
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : get first subType err!\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}
	}

	/*use scanType find right curr data */
	/*xxxScanTypeXCurrData();*/
	/*scanType*/
	if(gpnStatTypeSubStatType2ScanType(subType, &scanType) == GPN_STAT_DBS_GEN_ERR)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : subType(%08x) to scanType err!\n\r",\
			__FUNCTION__, subType);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pStatScanPort = pStatLocalNode->pStatScanPort;
	pscanTypeNode = pStatScanPort->pscanTypeNode;
	
	if(scanType != pStatScanPort->statScanType)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : scanType(%08x) err!\n\r",\
			__FUNCTION__, scanType);

		/*re-set scanType */
	}
	
	if(pStatScanPort->statCurrDate == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : curr mon disable, err!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	if(gpnStatTypeGetSubTpOrderBaseScanType(subType,\
			scanType, &order) != GPN_STAT_DBS_GEN_OK)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : subType(%08x) get order err!\n\r",\
			__FUNCTION__, subType);
		return GPN_STAT_DBS_GEN_ERR;
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : subType(%08x) order(%d) total(%d)!\n\r",\
		__FUNCTION__, subType, order, pscanTypeNode->statSubTpNumInScanType);
	/*find right subType data place */
	off = 0;
	for(i=0;i<pscanTypeNode->statSubTpNumInScanType;i++)
	{
		if(i == order)
		{
			memcpy(&(pcurrDataLine->viewPort), &(pStatLocalNode->viewPort), sizeof(optObjOrient));
			pcurrDataLine->subType = subType;
			pcurrDataLine->scanType = scanType;
			
			psubCurr = (UINT32 *)((UINT8 *)(pStatScanPort->statCurrDate) + off*sizeof(UINT32));
			if(pscanTypeNode->subBitDeep & (1U<<i))
			{
				/*64 bit */
				pcurrDataLine->currDataH32 = *psubCurr;
				pcurrDataLine->currDataL32 = *(psubCurr + 1);
			}
			else
			{
				pcurrDataLine->currDataH32 = 0;
				pcurrDataLine->currDataL32 = *psubCurr;
			}
		}
		else
		{
			if(pscanTypeNode->subBitDeep & (1U<<i))
			{
				/*64 bit */
				off +=2;
			}
			else
			{
				/*32 bit */
				off +=1;
			}
		}
	}

	/*next index get */
	/*modify by geqian 2015.12.9*/
	if (subTypeRecord == GPN_STAT_SUB_TYPE_INVALID)
	{
		*psNextSubType = subType;
		memcpy(pnextPIndex, &(pStatLocalNode->localPort), sizeof(objLogicDesc));
	}
	else
	{
		gpnStatSeekNextSubTypeInPortScanNode(pStatLocalNode, subType, psNextSubType);
		if(*psNextSubType == GPN_STAT_SUB_TYPE_INVALID)
		{
			memcpy(pnextPIndex, psNextIndex, sizeof(objLogicDesc));
		}
		else
		{
			memcpy(pnextPIndex, &(pStatLocalNode->localPort), sizeof(objLogicDesc));
		}
	}
	
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : next dev(%08x) port(%08x|%08x|%08x|%08x|%08x) subTyep(%08x)!\n\r",\
	__FUNCTION__, pnextPIndex->devIndex, pnextPIndex->portIndex,\
	pnextPIndex->portIndex3, pnextPIndex->portIndex4,\
	pnextPIndex->portIndex5, pnextPIndex->portIndex6	,\
	*psNextSubType);
	
	
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatCurrDataSubTypeQureyNextAdjust(objLogicDesc *pnextPIndex, UINT32 *pnextSubType)
{
	stStatLocalPortNode *pStatLocalNode;
	
	/*assert */
	if( (pnextPIndex == NULL) ||\
		(pnextSubType == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if(pnextPIndex->portIndex == GPN_ILLEGAL_PORT_INDEX)
	{
		*pnextSubType = GPN_STAT_SUB_TYPE_INVALID;
		return GPN_STAT_DBS_GEN_OK;
	}

	if(*pnextSubType == GPN_STAT_SUB_TYPE_INVALID)
	{
		pStatLocalNode = NULL;
		gpnStatSeekLocalPort2LocalPortNode(pnextPIndex, &pStatLocalNode);
		if(pStatLocalNode != NULL)
		{
			if(gpnStatSeekFirstSubTypeInPortScanNode(pStatLocalNode, pnextSubType) == GPN_STAT_DBS_GEN_OK)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : adjust to nextSubType(%08x)\n\r",\
					__FUNCTION__, *pnextSubType);
				return GPN_STAT_DBS_GEN_OK;
			}
		}
	
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatHistDataTpIdGet(void)
{
	/*maybe porblerm ???*/
	static UINT32 histLocal = 0;
	
	++histLocal;
	if(histLocal == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		++histLocal;
	}
	if(histLocal == 0)
	{
		++histLocal;
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get histLocal(%08x)\n\r",\
		__FUNCTION__, histLocal);
	return histLocal;
}

UINT32 gpnStatHistDataAddInMgt(stStatHistRecdUnit *phistRecdUnit)
{
	UINT32 hash;
	
	/*assert */
	if(phistRecdUnit == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*printf("\n\rhistDataLoc:%d stoptime:%d en:%d subWild:%d valideSub:%d\n\r", \
		phistRecdUnit->histDataLoc, phistRecdUnit->stopTime, phistRecdUnit->en, phistRecdUnit->subWild, phistRecdUnit->validSub);*/
	hash = phistRecdUnit->histDataLoc % pgstEQUStatProcSpace->statHistDataMgt.hash;

	listAdd(&(pgstEQUStatProcSpace->statHistDataMgt.histDataQuen[hash]), (NODE *)phistRecdUnit);
	pgstEQUStatProcSpace->statHistDataMgt.histDataNum[hash]++;

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatHistDataDelOutMgt(UINT32 histLocal)
{
	UINT32 hash;
	UINT32 histDRNum;
	stStatHistRecdUnit *ptempRecdUnit;
	
	/*assert */
	
	hash = histLocal % pgstEQUStatProcSpace->statHistDataMgt.hash;

	/*first : found; then : delelte */
	histDRNum = 0;
	ptempRecdUnit = (stStatHistRecdUnit *)listFirst(&(pgstEQUStatProcSpace->statHistDataMgt.histDataQuen[hash]));
	while((ptempRecdUnit!=NULL)&&(histDRNum<pgstEQUStatProcSpace->statHistDataMgt.histDataNum[hash]))
	{
		if(histLocal == ptempRecdUnit->histDataLoc)
		{
			listDelete(&(pgstEQUStatProcSpace->statHistDataMgt.histDataQuen[hash]), (NODE *)ptempRecdUnit);
			pgstEQUStatProcSpace->statHistDataMgt.histDataNum[hash]--;

			/*node should be free in memrey proc.
			   we should not use this method, should
			   be malloc, then add in idleList. */
		}

		histDRNum++;
		ptempRecdUnit = (stStatHistRecdUnit *)listNext((NODE *)ptempRecdUnit);
	}

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatHistDataGet(UINT32 histDataLoc, UINT32 stopTime, stStatHistRecdUnit **pphistRecdUnit)
{
	UINT32 hash;
	UINT32 histDRNum;
	stStatHistDataMgt *phistDataMgt;
	stStatHistRecdUnit *ptempRecdUnit;
	
	/*assert */
	if(pphistRecdUnit == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	phistDataMgt = &(pgstEQUStatProcSpace->statHistDataMgt);
	hash = histDataLoc % phistDataMgt->hash;

	/*first : found; then : delelte */
	histDRNum = 0;
	ptempRecdUnit = (stStatHistRecdUnit *)listFirst(&(phistDataMgt->histDataQuen[hash]));
	while((ptempRecdUnit!=NULL)&&(histDRNum<phistDataMgt->histDataNum[hash]))
	{
		if( (histDataLoc == ptempRecdUnit->histDataLoc) &&\
			(stopTime == ptempRecdUnit->stopTime) &&\
			(GPN_STAT_DBS_GEN_ENABLE == ptempRecdUnit->en) )
		{
			*pphistRecdUnit = ptempRecdUnit;
			
			return GPN_STAT_DBS_GEN_OK;
		}

		histDRNum++;
		ptempRecdUnit = (stStatHistRecdUnit *)listNext((NODE *)ptempRecdUnit);
	}

	*pphistRecdUnit = NULL;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : node(Loc %d stopTime %08x) not found!\n\r",\
		__FUNCTION__, histDataLoc, stopTime);

	return GPN_STAT_DBS_GEN_ERR;
}
UINT32 gpnStatHistDataSubTypeQurey(stStatHistRecdUnit *phistRecdUnit,
	UINT32 subType, stStatHistDataTable *phistDataLine)
{
	UINT32 i;
	stStatDataElement *psubElement;
	UINT32 statScanType;
	
	/*assert */
	if( (phistRecdUnit == NULL) ||\
		(phistDataLine == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*find subType hist data */
	psubElement = (stStatDataElement *)phistRecdUnit->statDate;
	for(i=0;i<phistRecdUnit->subWild;i++)
	{
		if(subType == psubElement[i].subType)
		{
			phistDataLine->index = phistRecdUnit->histDataLoc;
			phistDataLine->stopTime = phistRecdUnit->stopTime;
			phistDataLine->subType = psubElement[i].subType;

			/*should not include this */
			/*modify by geqian 2015.12.8*/
			//phistDataLine->scanType = GPN_STAT_SCAN_TYPE_ALL;
			if(gpnStatTypeSubStatType2ScanType(subType, &statScanType) == GPN_STAT_TYPE_OK)
			{
				phistDataLine->scanType = statScanType;
			}

			phistDataLine->histDataH32 = psubElement[i].dataHigh32;
			phistDataLine->histDataL32 = psubElement[i].dataLow32;

			return GPN_STAT_DBS_GEN_OK;
		}
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : histIndex(%08x) stopTime(%08x) subTyp(%08x) hist find err!\n\r",\
		__FUNCTION__, phistRecdUnit->histDataLoc, phistRecdUnit->stopTime, subType);
	return GPN_STAT_DBS_GEN_ERR;
}

UINT32 gpnStatHistDataGetNext(UINT32 histDataLoc, UINT32 stopTime,
	stStatHistRecdUnit **pphistRecdUnit, UINT32 *pnextHistDataLoc, UINT32 *pnextStopTime)
{
	UINT32 hash;
	UINT32 histDRNum;
	UINT32 tmpHistLocal;
	stStatHistDataMgt *phistDataMgt = NULL;
	stStatHistRecdUnit *ptempRecdUnit = NULL;
	
	/*assert */
	if( (pphistRecdUnit == NULL) ||\
		(pnextHistDataLoc == NULL) ||\
		(pnextStopTime == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	phistDataMgt = &(pgstEQUStatProcSpace->statHistDataMgt);
	
	tmpHistLocal = histDataLoc;
	if(tmpHistLocal == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		/*0xFFFFFFFF means template search start, so re-give a search start : must find first tp */
		*pphistRecdUnit = NULL;
		hash = 0;
		while(hash < phistDataMgt->hash)
		{
			histDRNum = 0;
			ptempRecdUnit = (stStatHistRecdUnit *)listFirst(&(phistDataMgt->histDataQuen[hash]));
			while((ptempRecdUnit!=NULL)&&(histDRNum<phistDataMgt->histDataNum[hash]))
			{
				/*histDataNode must be enable */
				//printf("ptempRecdUnit->en:%d\n\r", ptempRecdUnit->en);
				if(GPN_STAT_DBS_GEN_ENABLE == ptempRecdUnit->en)
				{
					*pphistRecdUnit = ptempRecdUnit;
					tmpHistLocal = ptempRecdUnit->histDataLoc;
					break;
				}

				histDRNum++;
				ptempRecdUnit = (stStatHistRecdUnit *)listNext((NODE *)ptempRecdUnit);
			}
			if(*pphistRecdUnit != NULL)
			{
				break;
			}

			/*this quen is empyt, so goto next quen */
			hash++;
		}
		if(hash >= phistDataMgt->hash)
		{
			*pphistRecdUnit = NULL;
			*pnextHistDataLoc = GPN_STAT_INVALID_TEMPLAT_ID;
			*pnextStopTime = GPN_STAT_DBS_32_FFFF;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find first histLocal!\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}

		*pnextHistDataLoc = ptempRecdUnit->histDataLoc;
		*pnextStopTime = ptempRecdUnit->stopTime;
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next histDataLocal(%d) stopTime(%08x)!\n\r",\
			__FUNCTION__, *pnextHistDataLoc, *pnextStopTime);
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
		hash = tmpHistLocal % phistDataMgt->hash;

		histDRNum = 0;
		ptempRecdUnit = (stStatHistRecdUnit *)listFirst(&(phistDataMgt->histDataQuen[hash]));
		while((ptempRecdUnit!=NULL)&&(histDRNum<phistDataMgt->histDataNum[hash]))
		{
			if( (tmpHistLocal == ptempRecdUnit->histDataLoc) &&\
				(stopTime == ptempRecdUnit->stopTime) &&\
				(GPN_STAT_DBS_GEN_ENABLE == ptempRecdUnit->en) ) 
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get this histLocal(%d) stopTime(%08x)!\n\r",\
					__FUNCTION__, tmpHistLocal, stopTime);
				*pphistRecdUnit = ptempRecdUnit;
				
				break;
			}

			histDRNum++;
			ptempRecdUnit = (stStatHistRecdUnit *)listNext((NODE *)ptempRecdUnit);
		}
		if((ptempRecdUnit==NULL)||(histDRNum>=phistDataMgt->histDataNum[hash]))
		{
			*pphistRecdUnit = NULL;
			*pnextHistDataLoc = GPN_STAT_INVALID_TEMPLAT_ID;
			*pnextStopTime = GPN_STAT_DBS_32_FFFF;
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not find histLocal(%d) stopTime(%08x)!\n\r",\
				__FUNCTION__, tmpHistLocal, stopTime);
			return GPN_STAT_DBS_GEN_ERR;
		}
	
		/*fast go to find next node, search in old hash thredTp quen*/
		if(ptempRecdUnit != NULL)
		{
			histDRNum++;
			ptempRecdUnit = (stStatHistRecdUnit *)listNext((NODE *)ptempRecdUnit);
			while((ptempRecdUnit!=NULL)&&(histDRNum<phistDataMgt->histDataNum[hash]))
			{
				if(GPN_STAT_DBS_GEN_ENABLE == ptempRecdUnit->en)
				{
					GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next histLocal(%d) stopTime(%08x) in old quen!\n\r",\
						__FUNCTION__, ptempRecdUnit->histDataLoc, ptempRecdUnit->stopTime);
					*pnextHistDataLoc = ptempRecdUnit->histDataLoc;
					*pnextStopTime = ptempRecdUnit->stopTime;
					
					return GPN_STAT_DBS_GEN_OK;
				}

				histDRNum++;
				ptempRecdUnit = (stStatHistRecdUnit *)listNext((NODE *)ptempRecdUnit);
			}
		}
		
		/*fast go to find next node, search in all-other hash thredTp quen*/
		hash++;
		while(hash < phistDataMgt->hash)
		{
			histDRNum = 0;
			ptempRecdUnit = (stStatHistRecdUnit *)listFirst(&(phistDataMgt->histDataQuen[hash]));
			while((ptempRecdUnit!=NULL)&&(histDRNum<phistDataMgt->histDataNum[hash]))
			{
				if(GPN_STAT_DBS_GEN_ENABLE == ptempRecdUnit->en)
				{
					GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : get next histLocal(%d) stopTime(%08x) in new quen!\n\r",\
						__FUNCTION__, ptempRecdUnit->histDataLoc, ptempRecdUnit->stopTime);
					*pnextHistDataLoc = ptempRecdUnit->histDataLoc;
					*pnextStopTime = ptempRecdUnit->stopTime;
				
					return GPN_STAT_DBS_GEN_OK;
				}

				histDRNum++;
				ptempRecdUnit = (stStatHistRecdUnit *)listNext((NODE *)ptempRecdUnit);
			}
			
			/*start in new quen */
			hash++;
		}
	}
	
	*pnextHistDataLoc = GPN_STAT_INVALID_TEMPLAT_ID;
	*pnextStopTime = GPN_STAT_DBS_32_FFFF;
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can not get next histLocal!\n\r",\
		__FUNCTION__);
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatHistDataSubTypeQureyNext(stStatHistRecdUnit *phistRecdUnit/*in*/,
	UINT32 histId, UINT32 stopTime, UINT32 subType, stStatHistDataTable *phistDataLine/*out*/,
	UINT32 sNextHistId, UINT32 sNextStopTime, UINT32 *pnextHistId, UINT32 *pnextStopTime,
	UINT32 *pnextSubType)
{
	UINT32 i;
	UINT32 seat;
	stStatDataElement *psubElement;

	/*assert */
	if( (phistRecdUnit == NULL) ||\
		(pnextHistId == NULL) ||\
		(pnextStopTime == NULL) ||\
		(pnextSubType == NULL) )
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	psubElement = (stStatDataElement *)phistRecdUnit->statDate;
	if(subType == GPN_STAT_SUB_TYPE_INVALID)
	{
		/*in getNext, here is subType find first, so qurey valid 'first', not 'order first' */
		seat = phistRecdUnit->subWild;
		for(i=0;i<phistRecdUnit->subWild;i++)
		{
			//printf("psubElement[%d].subType:%d  seat:%d\n\r", i, psubElement[i].subType, seat);
			if(psubElement[i].subType != GPN_STAT_SUB_TYPE_INVALID)
			{
				seat = i;
				break;
			}
		}
	}
	else
	{
		/*give a invalid seat before for(...) start */
		seat = phistRecdUnit->subWild;
		for(i=0;i<phistRecdUnit->subWild;i++)
		{
			if(subType == psubElement[i].subType)
			{
				seat = i;
				break;
			}
		}
	}
	
	if(seat >= phistRecdUnit->subWild)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : can't find subType(%08x) in histLocal(%d) stopTime(%08x)\n\r",\
			__FUNCTION__, subType, phistRecdUnit->histDataLoc, phistRecdUnit->stopTime);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*find valid subType */
	if(phistDataLine != NULL)
	{
		phistDataLine->index = phistRecdUnit->histDataLoc;
		phistDataLine->stopTime = phistRecdUnit->stopTime;
		phistDataLine->subType = psubElement[seat].subType;
		phistDataLine->scanType = GPN_STAT_SCAN_TYPE_ALL;

		phistDataLine->histDataH32 = psubElement[seat].dataHigh32;
		phistDataLine->histDataL32 = psubElement[seat].dataLow32;
	}

	/*find next valid subType */
	if(histId == GPN_STAT_INVALID_TEMPLAT_ID)
	{
		/*means first get next , use this index for next index */
		*pnextSubType = psubElement[seat].subType;
		*pnextStopTime = phistRecdUnit->stopTime;
		*pnextHistId = phistRecdUnit->histDataLoc;
	}
	else
	{
		for(seat += 1;seat<phistRecdUnit->subWild;seat++)
		{
			if(psubElement[seat].subType != GPN_STAT_SUB_TYPE_INVALID) 
			{
				/*find valid next subType in same line*/
				*pnextSubType = psubElement[seat].subType;
				/*next valid still in same line */
				*pnextStopTime = phistRecdUnit->stopTime;
				*pnextHistId = phistRecdUnit->histDataLoc;

				break;
			}
		}
		
		if(seat >= phistRecdUnit->subWild)
		{
			/*next valid in next line */
			*pnextHistId = sNextHistId;
			*pnextStopTime = sNextStopTime;
			/*not find valid next subType in same line */
			*pnextSubType = GPN_STAT_SUB_TYPE_INVALID;
		}
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s :sNextHistId(%d) histId(%d) stopTime(%08x) subType(%08x) goto next histId(%d) stopTime(%08x) subType(%08x)\n\r",\
		__FUNCTION__, sNextHistId, histId, stopTime, subType,\
		*pnextHistId, *pnextStopTime, *pnextSubType);
	return GPN_STAT_DBS_GEN_OK;

}
UINT32 gpnStatHistDataSubTypeQureyNextAdjust(UINT32 *pnextHistId, UINT32 *pnextStopTim, UINT32 *pnextSubType)
{
	stStatHistRecdUnit *phistRecdUnit;
	stStatDataElement *psubElement;
	UINT32 i;
	
	/*assert */
	if( (pnextHistId == NULL) ||\
		(pnextStopTim == NULL) ||\
		(pnextSubType == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	if( (*pnextHistId == GPN_STAT_INVALID_TEMPLAT_ID) ||\
		(*pnextStopTim == 0) )
	{
		*pnextHistId = GPN_STAT_INVALID_TEMPLAT_ID;
		*pnextStopTim = GPN_STAT_DBS_32_FFFF;
		*pnextSubType = GPN_STAT_SUB_TYPE_INVALID;
		
		return GPN_STAT_DBS_GEN_OK;
	}

	if(*pnextSubType == GPN_STAT_SUB_TYPE_INVALID)
	{
		phistRecdUnit = NULL;
		gpnStatHistDataGet(*pnextHistId, *pnextStopTim, &phistRecdUnit);
		if(phistRecdUnit == NULL)
		{
			return GPN_STAT_DBS_GEN_ERR;
		}

		psubElement = phistRecdUnit->statDate;
		for(i=0;i<phistRecdUnit->subWild;i++)
		{
			if(psubElement[i].subType != GPN_STAT_SUB_TYPE_INVALID)
			{
				*pnextSubType = psubElement[i].subType;

				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : adjust to nextHistId(%d) nextStop(%08x) nextSubType(%08x)\n\r",\
					__FUNCTION__, *pnextHistId, *pnextStopTim, *pnextSubType);
				break;
			}
		}

		if(i >= phistRecdUnit->subWild)
		{
			return GPN_STAT_DBS_GEN_ERR;
		}
	}
	
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatTrapDataAddInMgt(stStatXCycDataRecd *pxCycDataRecd,
	stStatTaskNode *ptaskNode, objLogicDesc *pMonObjIndex)
{
	/*on auto report, this used for delete old node, then add new node */
	static UINT32 nextDel = 1;
	
	UINT32 hash;
	UINT32 trapNodeNum;
	stStatHistDataMgt *phistDataMgt;
	stStatHistTrapDataNode trapNode;
	stStatHistTrapDataNode *ptrapNode;
	stStatHistTrapDataNode *ptmpTrapNode;

	/*assert */
	if( (pxCycDataRecd == NULL) ||\
		(ptaskNode == NULL) ||\
		(pMonObjIndex == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	phistDataMgt = &(pgstEQUStatProcSpace->statHistDataMgt);
	
	/*auto trap hist data */
	if(pgstEQUStatProcSpace->statGlobalInfo.lastHistRept == GPN_STAT_DBS_GEN_ENABLE)
	{	
		if (pxCycDataRecd->trapEn == GPN_STAT_DBS_GEN_ENABLE)
		{
			if(phistDataMgt->idleTrapDataNum > 0)
			{
				/*get idle trap node */
				ptrapNode = (stStatHistTrapDataNode *)listGet(&(phistDataMgt->idleTrapDataQuen));
				phistDataMgt->idleTrapDataNum--;
			
				/*calculate trap timeslot */
				gpnStatCreatTrapTimeSlot(pxCycDataRecd);
		
				/*set trap node info */
				gpnStatTrapNodeSetInfo(ptrapNode, pxCycDataRecd, ptaskNode, pMonObjIndex);

				/*add in trap wait list :
				  1 first search list from tail to head; 
				  2 if trapTSlot > tmpTrapTSlot, insert */
				hash = ptrapNode->dataIndex % phistDataMgt->hash;
				trapNodeNum = 0;
				ptmpTrapNode = (stStatHistTrapDataNode *)listLast(&(phistDataMgt->actTrapDataQuen[hash]));
				while((ptmpTrapNode!=NULL)&&(trapNodeNum<phistDataMgt->actTrapDataNum[hash]))
				{
					if(ptrapNode->stopTime >= ptmpTrapNode->stopTime)
					{
						break;
					}
					trapNodeNum++;
					ptmpTrapNode = (stStatHistTrapDataNode *)listPrevious((NODE *)ptmpTrapNode);
				}
				
				listInsert(&(phistDataMgt->actTrapDataQuen[hash]), (NODE *)ptmpTrapNode, (NODE *)ptrapNode);
				phistDataMgt->actTrapDataNum[hash]++;

				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : insert trapNode(%d) nowTime(%08x) trapSlot(%08x) stopTime(%08x) cycClass(%d) vilSub(%d)!\n\r",\
					__FUNCTION__, ptrapNode->dataIndex, pgstEQUStatProcSpace->statTaskInfo.statSysTime,\
					ptrapNode->trapTSlot, ptrapNode->stopTime,\
					ptrapNode->cycClass, ptrapNode->validSubNum);

				//printf("%s : hash(%d) meStop:%d\n\r",  __FUNCTION__, hash, ptrapNode->stopTime);
			}
			else /*if(pgstEQUStatProcSpace->statGlobalInfo.lastHistRept == GPN_STAT_DBS_GEN_DISABLE)*/
			{
				/*trap right now */
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : trap right now!\n\r",\
					__FUNCTION__);

				gpnStatTrapNodeSetInfo(&trapNode, pxCycDataRecd, ptaskNode, pMonObjIndex);

				gpnStatTrapDataOpt(&trapNode);
			}
		}	
	}
	/*do not trap hist data */
	else
	{
		if(phistDataMgt->idleTrapDataNum > 0)
		{
			/*get idle trap node */
			ptrapNode = (stStatHistTrapDataNode *)listGet(&(phistDataMgt->idleTrapDataQuen));
			phistDataMgt->idleTrapDataNum--;

			/*init trap node */
			gpnStatTrapNodeSetInfo(ptrapNode, pxCycDataRecd, ptaskNode, pMonObjIndex);
			
			/*add in list */
			hash = ptrapNode->dataIndex % phistDataMgt->hash;
			listAdd(&(phistDataMgt->actTrapDataQuen[hash]), (NODE *)ptrapNode);
			phistDataMgt->actTrapDataNum[hash]++;
		}
		else
		{
			/*delete oldest node */
			hash = nextDel % phistDataMgt->hash;
			trapNodeNum = 0;
			ptrapNode = (stStatHistTrapDataNode *)listFirst(&(phistDataMgt->actTrapDataQuen[hash]));
			while((ptrapNode!=NULL)&&(trapNodeNum<phistDataMgt->actTrapDataNum[hash]))
			{
				/*find delete node */
				if(ptrapNode->dataIndex == nextDel)
				{
					/*reset trap node use new hist data */
					gpnStatTrapNodeSetInfo(ptrapNode, pxCycDataRecd, ptaskNode, pMonObjIndex);
					break;
				}
				trapNodeNum++;
				ptrapNode = (stStatHistTrapDataNode *)listPrevious((NODE *)ptrapNode);
			}
			if((ptrapNode==NULL)||(trapNodeNum>=phistDataMgt->actTrapDataNum[hash]))
			{
				/*server error, log */
				return GPN_STAT_DBS_GEN_ERR;
			}
			
			/*increase for next use */
			nextDel++;
			if(nextDel > phistDataMgt->trapNodeNum)
			{
				nextDel = 1;
			}
		}

		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : insert newHist(%d) nowTime(%08x) trapSlot(%d) stopTime(%08x) cycClass(%d) vilSub(%d)!\n\r",\
			__FUNCTION__, ptrapNode->dataIndex, pgstEQUStatProcSpace->statTaskInfo.statSysTime,\
			ptrapNode->trapTSlot, ptrapNode->stopTime,\
			ptrapNode->cycClass, ptrapNode->validSubNum);
	}

	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatCreatTrapTimeSlot(stStatXCycDataRecd *pxCycDataRecd)
{
	static UINT32 fakeRandom;
	UINT32 suffix;
	
	/*assert */
	if(pxCycDataRecd == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*modify by geqian */
	/*when modify subfiltTp, ptaskNode maybe null*/
	if (pxCycDataRecd->ptaskNode == NULL)   
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*first 	: find A, which is smallest among (xCycSec/2) , GPN_STAT_MAX_TRAP_DELAY_SEC; 	*/
	/*second 	: get B, which is fakeRandom % A;										*/
	/*thrid	: get C, which is statSysTime + B											*/
	
	fakeRandom++;
	
	if((pxCycDataRecd->ptaskNode->cycSeconds/2) < GPN_STAT_MAX_TRAP_DELAY_SEC)
	{
		suffix = (pxCycDataRecd->ptaskNode->cycSeconds/2);
	}
	else
	{
		suffix = GPN_STAT_MAX_TRAP_DELAY_SEC;
	}

	suffix = fakeRandom%suffix;
	
	pxCycDataRecd->trapTSlot = pgstEQUStatProcSpace->statTaskInfo.statSysTime + suffix;
	
//	printf("fakeRandom:%d, suffix:%d, pxCycDataRecd->trapTSlot:%d nowTime:%d\n\r", 
//		fakeRandom, suffix, pxCycDataRecd->trapTSlot, time(NULL));
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatTrapNodeSetInfo(stStatHistTrapDataNode *ptrapNode,
	stStatXCycDataRecd *pxCycDataRecd, stStatTaskNode *ptaskNode,
	objLogicDesc *pMonObjIndex)
{
	static UINT32 trapIndex;
	subFiltTpUnit * psubFileInfo;
	subReportTpUnit *psubReptInfo;
	stStatHistRecdUnit *phistRecUnit;
	stStatDataElement *phistTrapElement;
	UINT32 subNum;
	UINT32 i;
	UINT32 j;
	
	/*assert */
	if( (ptrapNode == NULL) ||\
		(pxCycDataRecd == NULL) ||\
		(ptaskNode == NULL) ||\
		(pMonObjIndex == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	if(pxCycDataRecd->histRecNum > 0)
	{
		phistRecUnit = pxCycDataRecd->pstatXCycHistRecd[pxCycDataRecd->recEnd];
		phistTrapElement = (stStatDataElement *)phistRecUnit->statDate;
		psubFileInfo = (subFiltTpUnit * )pxCycDataRecd->subFiltInfo;
		psubReptInfo = (subReportTpUnit * )pxCycDataRecd->subReptInfo;
		subNum = phistRecUnit->subWild;

		if (psubFileInfo == NULL || psubReptInfo == NULL)
		{
			//printf("make trouble maybe because delete subFile Template\n\r");
			return GPN_STAT_DBS_GEN_ERR;
		}

		/*do this when phistDataMgt init (ptrapNode add in idleTrapDataQuen) */
		ptrapNode->trapTSlot = pxCycDataRecd->trapTSlot;
		/*init trap data*/
		ptrapNode->scanType = ptaskNode->statScanTp;
		ptrapNode->cycClass = ptaskNode->cycClass;
		ptrapNode->stopTime = phistRecUnit->stopTime;
		memcpy(&(ptrapNode->viewPort), pMonObjIndex, sizeof(objLogicDesc));
		ptrapNode->validSubNum = phistRecUnit->validSub;
		/*copy palyload data */
		for(i=0,j=0;i<subNum;i++)
		{
//			printf("\n\r subFileInfo status:%d, subReptInfo status:%d \n\r",
//				psubFileInfo[i].status, psubReptInfo[i].status);
			if( (psubFileInfo[i].status == GPN_STAT_DBS_GEN_DISABLE) &&\
				(psubReptInfo[i].status == GPN_STAT_DBS_GEN_ENABLE) )
			{
				if(j < GPN_STAT_MAX_SUB_INCLUDE)
				{
					ptrapNode->trapIndex = trapIndex++;
					ptrapNode->data[j].subType = phistTrapElement[i].subType;
					ptrapNode->data[j].dataHigh32 = phistTrapElement[i].dataHigh32;
					ptrapNode->data[j].dataLow32 = phistTrapElement[i].dataLow32;
					j++;
				}
				else
				{
					printf("%s[%d] : stStatHistTrapDataNode data memory is not enough, please modify GPN_STAT_MAX_SUB_INCLUDE!!!\n", __func__, __LINE__);					
				}
			}
		}
		
//		printf("%s : trapIndex:%d trapSlot:%d scanType:%08x stopTime:%d\n\r",
//			__FUNCTION__, ptrapNode->trapIndex, ptrapNode->trapTSlot, 
//			ptrapNode->scanType, ptrapNode->stopTime);

		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
//		printf("%s : faital error happen, system !!!!\n\r", __FUNCTION__);
		/*no hist rec data , no trap data element */
		ptrapNode->validSubNum = 0;
		return GPN_STAT_DBS_GEN_ERR;
	}
}
UINT32 gpnStatTrapDataClear(void)
{
	UINT32 hash;
	stStatHistTrapDataNode *ptempTrapData;

	for(hash=0;hash<pgstEQUStatProcSpace->statHistDataMgt.hash;hash++)
	{
		while(pgstEQUStatProcSpace->statHistDataMgt.actTrapDataNum[hash] > 0)
		{
			ptempTrapData = (stStatHistTrapDataNode *)listGet(&(pgstEQUStatProcSpace->statHistDataMgt.actTrapDataQuen[hash]));
			pgstEQUStatProcSpace->statHistDataMgt.actTrapDataNum[hash]--;
			if(ptempTrapData == NULL)
			{
				GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : (%d)th list err!\n\r",\
					__FUNCTION__, hash);
				break;
			}
			
			listAdd(&(pgstEQUStatProcSpace->statHistDataMgt.idleTrapDataQuen), (NODE *)ptempTrapData);
			pgstEQUStatProcSpace->statHistDataMgt.idleTrapDataNum++;
		}
	}
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatTrapDataOpt(stStatHistTrapDataNode *ptrapNode)
{
	stStatTrapDataTable trapDataLine;
	UINT32 i;
	
	/*assert */
	if(ptrapNode == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	trapDataLine.trapIndex = ptrapNode->trapIndex;
	trapDataLine.scanType = ptrapNode->scanType;
	trapDataLine.cycClass = ptrapNode->cycClass;
	trapDataLine.stopTime = ptrapNode->stopTime;
	memcpy(&(trapDataLine.viewPort), &(ptrapNode->viewPort), sizeof(objLogicDesc));
	for(i=0;i<ptrapNode->validSubNum;i++)
	{
		trapDataLine.subType = ptrapNode->data[i].subType;
		trapDataLine.dataHigh32 = ptrapNode->data[i].dataHigh32;
		trapDataLine.dataLow32 = ptrapNode->data[i].dataLow32;

		/*trap method call back*/
		if(pgstEQUStatProcSpace->trapMethodFunc != NULL)
		{
			pgstEQUStatProcSpace->trapMethodFunc(&trapDataLine);
		}

		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : i(%d) type(%08x)(%08x) cyc(%d) time(%08x) h(%08x)l(%08x)!\n\r",\
			__FUNCTION__, trapDataLine.trapIndex, trapDataLine.scanType,\
			trapDataLine.subType, trapDataLine.cycClass, trapDataLine.stopTime,\
			trapDataLine.dataHigh32, trapDataLine.dataLow32);
	}

	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatTrapDataMethodRegist(STATTRAPFUN trapMethodFunc)
{
	/*assert */
	if(trapMethodFunc == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	pgstEQUStatProcSpace->trapMethodFunc = trapMethodFunc;
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatTrapDataGet(UINT32 histDataIndex, stStatHistTrapDataNode **pptrapData)
{
	UINT32 hash;
	UINT32 histDataNum;
	stStatHistTrapDataNode *ptempTrapData;
	
	/*assert */
	if(pptrapData == NULL)
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	hash = histDataIndex % pgstEQUStatProcSpace->statHistDataMgt.hash;

	/*first : found; then : delelte */
	histDataNum = 0;
	ptempTrapData = (stStatHistTrapDataNode *)listFirst(&(pgstEQUStatProcSpace->statHistDataMgt.actTrapDataQuen[hash]));
	while((ptempTrapData!=NULL)&&(histDataNum<pgstEQUStatProcSpace->statHistDataMgt.actTrapDataNum[hash]))
	{
		if(histDataIndex == ptempTrapData->dataIndex)
		{
			*pptrapData = ptempTrapData;
			
			return GPN_STAT_DBS_GEN_OK;
		}

		histDataNum++;
		ptempTrapData = (stStatHistTrapDataNode *)listNext((NODE *)ptempTrapData);
	}
	
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : index(%d) not found in list!\n\r",\
		__FUNCTION__, histDataIndex);
	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatTrapDataGetNext(UINT32 histDataIndex, stStatHistTrapDataNode **pptrapData, UINT32 *pnextIndex)
{
	UINT32 hash;
	UINT32 histDataNum;
	stStatHistTrapDataNode *ptempTrapData;
	
	/*assert */
	if( (pptrapData == NULL) &&\
		(pnextIndex == NULL) )
	{
		return GPN_STAT_DBS_GEN_ERR;
	}

	hash = histDataIndex % pgstEQUStatProcSpace->statHistDataMgt.hash;

	/*first : found; then : delelte */
	histDataNum = 0;
	ptempTrapData = (stStatHistTrapDataNode *)listFirst(&(pgstEQUStatProcSpace->statHistDataMgt.actTrapDataQuen[hash]));
	while((ptempTrapData!=NULL)&&(histDataNum<pgstEQUStatProcSpace->statHistDataMgt.actTrapDataNum[hash]))
	{
		if(histDataIndex == ptempTrapData->dataIndex)
		{
			*pptrapData = ptempTrapData;
			
			break;
		}

		histDataNum++;
		ptempTrapData = (stStatHistTrapDataNode *)listNext((NODE *)ptempTrapData);
	}
	
	if(ptempTrapData != NULL)
	{
		ptempTrapData = (stStatHistTrapDataNode *)listNext((NODE *)ptempTrapData);
		if(ptempTrapData != NULL)
		{
			*pnextIndex = ptempTrapData->dataIndex;
		}
		else
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : index(%d) not have next!\n\r",\
				__FUNCTION__, histDataIndex);
		}
		return GPN_STAT_DBS_GEN_OK;
	}
	else
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : index(%d) not found in list!\n\r",\
			__FUNCTION__, histDataIndex);
		return GPN_STAT_DBS_GEN_ERR;
	}
}

UINT32 gpnStatEQUStatProcSpaceMollac(stEQUStatProcSpace **ppEQUStatProcSpace)
{
	UINT32 i;
	UINT32 j;
	
	/*assert */
	if(*ppEQUStatProcSpace != NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para is not NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	i = listCount(&(pgstStatTypeWholwInfo->statScanTypeQuen));
	if(i != pgstStatTypeWholwInfo->statScanTypeNum)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : statScanTypeQuen Node Num err(i:%d,list %d)\n\r",\
			__FUNCTION__, i, pgstStatTypeWholwInfo->statScanTypeNum);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*three part memrey : 
		1 global struct part;
		2 per device(slot) struct part;
		3 per device(slot)'s per scan type perScan Node;*/
	j = sizeof(stEQUStatProcSpace) +
		(EQU_SLOT_MAX_NUM + 1) * sizeof(stUnitStatProcSpace) +
		(EQU_SLOT_MAX_NUM + 1) * i * sizeof(stStatPreScanQuen);
		
	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CUP, "%s : stEQUStatProcSpace %d stUnitStatProcSpace %d*%d stStatPreScanQuen %d*%d\n",\
		__FUNCTION__, sizeof(stEQUStatProcSpace),\
		sizeof(stUnitStatProcSpace), (EQU_SLOT_MAX_NUM + 1),\
		sizeof(stStatPreScanQuen), (EQU_SLOT_MAX_NUM + 1)*i);

	*ppEQUStatProcSpace = (stEQUStatProcSpace *)malloc(j);
	if(*ppEQUStatProcSpace == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : malloc %d size for stEQUStatProcSpace err!\n\r",\
			__FUNCTION__, j);
		return GPN_STAT_DBS_GEN_ERR;
	}

	gpnLog(GPN_LOG_L_INFO, "%s : malloc %d size for stEQUStatProcSpace\n\r",\
		__FUNCTION__, j);

	memset((UINT8 *)(*ppEQUStatProcSpace), 0, j);
	
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatGlobalInfoInit(stEQUStatProcSpace *pEQUStatProcSpace)
{
	stStatGlobalInfo *pstatGlobalInfo;
	
	/*assert */
	if(pEQUStatProcSpace == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pstatGlobalInfo = &(pEQUStatProcSpace->statGlobalInfo);
	
	pstatGlobalInfo->polarity = GPN_STAT_TIME_POSITIVE;
	pstatGlobalInfo->offSet = 0;
	pstatGlobalInfo->lastHistRept = GPN_STAT_DBS_GEN_ENABLE;
	pstatGlobalInfo->oldHistRept = GPN_STAT_DBS_GEN_ENABLE;
	pstatGlobalInfo->alarmRept = GPN_STAT_DBS_GEN_ENABLE;
	pstatGlobalInfo->eventRept = GPN_STAT_DBS_GEN_ENABLE;
	pstatGlobalInfo->conutType = GPN_STAT_COUNT_TYPE_DELTA;

	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatTaskInfoInit(stEQUStatProcSpace *pEQUStatProcSpace)
{
	UINT32 i;
	stStatTaskNode *ptaskNode;
	stStatMonObjNode *pmonObjNode;
	stStatTaskInfo *pstatTaskInfo;
	
	/*assert */
	if(pEQUStatProcSpace == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pstatTaskInfo = &(pEQUStatProcSpace->statTaskInfo);
	
	pstatTaskInfo->hash = GPN_STAT_SEARCH_HASH;

	/*init task node */
	pstatTaskInfo->maxTaskNum = GPN_STAT_MAX_TASK_NUM;
	pstatTaskInfo->runTaskNum = 0;
	
	for(i=0;i<GPN_STAT_SEARCH_HASH;i++)
	{
		listInit(&(pstatTaskInfo->actTaskQuen[i]));
		pstatTaskInfo->actTaskNum[i] = 0;
	}
	listInit(&(pstatTaskInfo->idleTaskQuen));
	pstatTaskInfo->idleTaskNum = 0;

	i = (sizeof(stStatTaskNode))*(GPN_STAT_MAX_TASK_NUM);
	ptaskNode = (stStatTaskNode *)malloc(i);
	if(ptaskNode == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : malloc size(%d) err!\n\r",\
			__FUNCTION__, i);
		return GPN_STAT_DBS_GEN_ERR;
	}
	memset((UINT8 *)ptaskNode, 0, i);

	for(i=0;i<GPN_STAT_MAX_TASK_NUM;i++)
	{
		/*init taskId */
		ptaskNode->taskId = i+1;
		
		listAdd(&(pstatTaskInfo->idleTaskQuen), (NODE *)ptaskNode);
		pstatTaskInfo->idleTaskNum++;

		ptaskNode++;
	}

	/*init task include port node*/
	pstatTaskInfo->maxMonObjNum = GPN_STAT_MAX_MON_OBJ_NUM;
	pstatTaskInfo->runMonObjNum = 0;
	
	listInit(&(pstatTaskInfo->idleMOnOBjQuen));
	pstatTaskInfo->idleMOnOBjNum= 0;

	i = (sizeof(stStatMonObjNode *))*(GPN_STAT_MAX_MON_OBJ_NUM);
	pmonObjNode = (stStatMonObjNode *)malloc(i);
	if(pmonObjNode == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : malloc size(%d) err!\n\r",\
			__FUNCTION__, i);
		return GPN_STAT_DBS_GEN_ERR;
	}
	memset((UINT8 *)pmonObjNode, 0, i);

	for(i=0;i<GPN_STAT_MAX_TASK_NUM;i++)
	{
		listAdd(&(pstatTaskInfo->idleMOnOBjQuen), (NODE *)pmonObjNode);
		pstatTaskInfo->idleMOnOBjNum++;

		pmonObjNode++;
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : idleTaskNode(%d) idleMOnOBjNode(%d)!\n\r",\
		__FUNCTION__, pstatTaskInfo->idleTaskNum, pstatTaskInfo->idleMOnOBjNum);
	
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatEventThredTempInit(stEQUStatProcSpace *pEQUStatProcSpace)
{
	UINT32 i;
	stStatEvnThredTpNode *pthredTpNode;
	stStatThredTemp *pstatThredTemp;
	
	/*assert */
	if(pEQUStatProcSpace == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pstatThredTemp = &(pEQUStatProcSpace->statEventThredTemp);

	pstatThredTemp->thredTpIdDsp = 0;
	pstatThredTemp->maxThredTpNum = GPN_STAT_MAX_EVENT_THRED_TP_NUM;
	pstatThredTemp->runThredTpNum = 0;
	pstatThredTemp->hash = GPN_STAT_SEARCH_HASH;

	for(i=0;i<GPN_STAT_SEARCH_HASH;i++)
	{
		listInit(&(pstatThredTemp->actThredTpQuen[i]));
		pstatThredTemp->actThredTpNum[i] = 0;
	}
	listInit(&(pstatThredTemp->idleThredTpQuen));
	pstatThredTemp->idleThredTpNum = 0;

	i = (sizeof(stStatEvnThredTpNode))*(GPN_STAT_MAX_EVENT_THRED_TP_NUM);
	pthredTpNode = (stStatEvnThredTpNode *)malloc(i);
	if(pthredTpNode == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : malloc size(%d) err!\n\r",\
			__FUNCTION__, i);
		return GPN_STAT_DBS_GEN_ERR;
	}
	memset(pthredTpNode, 0, i);

	for(i=0;i<GPN_STAT_MAX_EVENT_THRED_TP_NUM;i++)
	{
		/*init thredTpId */
		pthredTpNode->thredTpId = i+1;
		
		listAdd(&(pstatThredTemp->idleThredTpQuen), (NODE *)pthredTpNode);
		pstatThredTemp->idleThredTpNum++;

		pthredTpNode++;
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : idleNode(%d)!\n\r",\
		__FUNCTION__, pstatThredTemp->idleThredTpNum);

	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatAlarmThredTempInit(stEQUStatProcSpace *pEQUStatProcSpace)
{
	UINT32 i;
	stStatAlmThredTpNode *pthredTpNode;
	stStatThredTemp *pstatThredTemp;
	
	/*assert */
	if(pEQUStatProcSpace == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	pstatThredTemp = &(pEQUStatProcSpace->statAlarmThredTemp);

	pstatThredTemp->thredTpIdDsp = 0;
	pstatThredTemp->maxThredTpNum = GPN_STAT_MAX_ALARM_THRED_TP_NUM;
	pstatThredTemp->runThredTpNum = 0;
	pstatThredTemp->hash = GPN_STAT_SEARCH_HASH;

	for(i=0;i<GPN_STAT_SEARCH_HASH;i++)
	{
		listInit(&(pstatThredTemp->actThredTpQuen[i]));
		pstatThredTemp->actThredTpNum[i] = 0;
	}
	listInit(&(pstatThredTemp->idleThredTpQuen));
	pstatThredTemp->idleThredTpNum = 0;

	i = (sizeof(stStatAlmThredTpNode))*(GPN_STAT_MAX_ALARM_THRED_TP_NUM);
	pthredTpNode = (stStatAlmThredTpNode *)malloc(i);
	if(pthredTpNode == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : malloc size(%d) err!\n\r",\
			__FUNCTION__, i);
		return GPN_STAT_DBS_GEN_ERR;
	}
	memset(pthredTpNode, 0, i);

	for(i=0;i<GPN_STAT_MAX_ALARM_THRED_TP_NUM;i++)
	{
		/*init thredTpId */
		pthredTpNode->thredTpId = i+1;
		
		listAdd(&(pstatThredTemp->idleThredTpQuen), (NODE *)pthredTpNode);
		pstatThredTemp->idleThredTpNum++;

		pthredTpNode++;
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : idleNode(%d)!\n\r",\
		__FUNCTION__, pstatThredTemp->idleThredTpNum);

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatSubFiltTempInit(stEQUStatProcSpace *pEQUStatProcSpace)
{
	UINT32 i;
	stStatSubFiltTpNode *psubFiltTpNode;
	stStatSubFiltTemp *psubFiltTemp;
	
	/*assert */
	if(pEQUStatProcSpace == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	psubFiltTemp = &(pEQUStatProcSpace->statSubFiltTemp);

	psubFiltTemp->maxSubFiltTpNum = GPN_STAT_MAX_SUB_FILT_TP_NUM;
	psubFiltTemp->runSubFiltTpNum = 0;
	psubFiltTemp->hash = GPN_STAT_SEARCH_HASH;

	for(i=0;i<GPN_STAT_SEARCH_HASH;i++)
	{
		listInit(&(psubFiltTemp->actSubFiltTpQuen[i]));
		psubFiltTemp->actSubFiltTpNum[i] = 0;
	}
	listInit(&(psubFiltTemp->idleSubFiltTpQuen));
	psubFiltTemp->idleSubFiltTpNum = 0;

	i = (sizeof(stStatSubFiltTpNode))*(GPN_STAT_MAX_SUB_FILT_TP_NUM);
	psubFiltTpNode = (stStatSubFiltTpNode *)malloc(i);
	if(psubFiltTpNode == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : malloc size(%d) err!\n\r",\
			__FUNCTION__, i);
		return GPN_STAT_DBS_GEN_ERR;
	}
	memset(psubFiltTpNode, 0, i);

	for(i=0;i<GPN_STAT_MAX_SUB_FILT_TP_NUM;i++)
	{
		/*init subFiltTpId */
		psubFiltTpNode->subFiltTpId = i+1;
		
		listAdd(&(psubFiltTemp->idleSubFiltTpQuen), (NODE *)psubFiltTpNode);
		psubFiltTemp->idleSubFiltTpNum++;

		psubFiltTpNode++;
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : idleNode(%d)!\n\r",\
		__FUNCTION__, psubFiltTemp->idleSubFiltTpNum);

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatSubReportTempInit(stEQUStatProcSpace *pEQUStatProcSpace)
{
	UINT32 i;
	stStatSubReportTpNode *psubReportTpNode;
	stStatSubReportTemp *psubReportTemp;
	
	/*assert */
	if(pEQUStatProcSpace == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	psubReportTemp = &(pEQUStatProcSpace->statSubReportTemp);

	psubReportTemp->maxSubReportTpNum = GPN_STAT_MAX_SUB_REPORT_TP_NUM;
	psubReportTemp->runSubReportTpNum = 0;
	psubReportTemp->hash = GPN_STAT_SEARCH_HASH;

	for(i=0;i<GPN_STAT_SEARCH_HASH;i++)
	{
		listInit(&(psubReportTemp->actSubReportTpQuen[i]));
		psubReportTemp->actSubReportTpNum[i] = 0;
	}
	listInit(&(psubReportTemp->idleSubReportTpQuen));
	psubReportTemp->idleSubReportTpNum = 0;

	i = (sizeof(stStatSubReportTpNode))*(GPN_STAT_MAX_SUB_REPORT_TP_NUM);
	psubReportTpNode = (stStatSubReportTpNode *)malloc(i);
	if(psubReportTpNode == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : malloc size(%d) err!\n\r",\
			__FUNCTION__, i);
		return GPN_STAT_DBS_GEN_ERR;
	}
	memset(psubReportTpNode, 0, i);

	for(i=0;i<GPN_STAT_MAX_SUB_REPORT_TP_NUM;i++)
	{
		/*init subReportTpId */
		psubReportTpNode->subReportTpId = i+1;
		
		listAdd(&(psubReportTemp->idleSubReportTpQuen), (NODE *)psubReportTpNode);
		psubReportTemp->idleSubReportTpNum++;

		psubReportTpNode++;
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : idleNode(%d)!\n\r",\
		__FUNCTION__, psubReportTemp->idleSubReportTpNum);

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatHistDataMgtInit(stEQUStatProcSpace *pEQUStatProcSpace)
{
	UINT32 i;
	stStatHistTrapDataNode *ptrapDataNode;
	stStatHistDataMgt *pstatHistDataMgt;
	
	/*assert */
	if(pEQUStatProcSpace == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*trap call bace */
	pEQUStatProcSpace->trapMethodFunc = NULL;

	pstatHistDataMgt = &(pEQUStatProcSpace->statHistDataMgt);

	pstatHistDataMgt->hash = GPN_STAT_SEARCH_HASH;

	for(i=0;i<GPN_STAT_SEARCH_HASH;i++)
	{
		listInit(&(pstatHistDataMgt->histDataQuen[i]));
		pstatHistDataMgt->histDataNum[i] = 0;
		
		listInit(&(pstatHistDataMgt->actTrapDataQuen[i]));
		pstatHistDataMgt->actTrapDataNum[i] = 0;
	}
	listInit(&(pstatHistDataMgt->idleTrapDataQuen));
	pstatHistDataMgt->idleTrapDataNum= 0;

	i = (sizeof(stStatHistTrapDataNode))*(GPN_STAT_MAX_TRAP_DATA_NUM);
	ptrapDataNode = (stStatHistTrapDataNode *)malloc(i);
	if(ptrapDataNode == NULL)
	{
		gpnLog(GPN_LOG_L_ERR, "%s : malloc size(%d) err!\n\r",\
			__FUNCTION__, i);
		return GPN_STAT_DBS_GEN_ERR;
	}
	memset(ptrapDataNode, 0, i);

	pstatHistDataMgt->trapNodeNum = GPN_STAT_MAX_TRAP_DATA_NUM;
	for(i=0;i<GPN_STAT_MAX_TRAP_DATA_NUM;i++)
	{
		/*ptrapDataNode init, code from '1' */
		ptrapDataNode->dataIndex = i+1;
		
		listAdd(&(pstatHistDataMgt->idleTrapDataQuen), (NODE *)ptrapDataNode);
		pstatHistDataMgt->idleTrapDataNum++;

		ptrapDataNode++;
	}

	GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : idleNode(%d)!\n\r",\
		__FUNCTION__, pstatHistDataMgt->idleTrapDataNum);

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 gpnStatPerScanQuenInit(stEQUStatProcSpace *pEQUStatProcSpace)
{
	UINT32 i;
	UINT32 k;
	UINT32 scanTpNum;	
	stUnitStatProcSpace *pslotStatProcSpace;
	stStatPreScanQuen *pStatPreScanQuenNode;
	stStatScanTypeDef *pStatScanTypeNode;

	/*assert */
	if(pEQUStatProcSpace == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}

	/*global memrey part 2 : base part 1 jump sizeof(pgstEQUStatProcSpace) */
	pgstEQUStatProcSpace->unitStatProcSpace[0] = (stUnitStatProcSpace *)(pgstEQUStatProcSpace + 1);
	/*global memrey part 3 : base part 2 jump (EQU_SLOT_MAX_NUM + 1)*sizeof(stUnitStatProcSpace) */
	/* Creat pStatPreScanQuenNode To unitStatProcSpace's Relation, Init pStatPreScanQuenNode*/
	pStatPreScanQuenNode = (stStatPreScanQuen *)(pgstEQUStatProcSpace->unitStatProcSpace[0] + (EQU_SLOT_MAX_NUM + 1));
	for(i=0;i<(EQU_SLOT_MAX_NUM + 1);i++)
	{
		/*init slot base data hungup point*/
		pgstEQUStatProcSpace->unitStatProcSpace[i] = pgstEQUStatProcSpace->unitStatProcSpace[0] + i;
		
		pslotStatProcSpace = pgstEQUStatProcSpace->unitStatProcSpace[i];

		/*Init every Slot's pslotStatProcSpace*/
		listInit(&(pslotStatProcSpace->statPreScanTpQuen));
		pslotStatProcSpace->quenNodeNum = 0;
		
		/*pStatPreScanQuenNode's memrey start in this slot, ( here we have statScanTypeNum pStatPreScanQuenNodes in one slot)*/
		pslotStatProcSpace->pCenterPreScanPortNodeMemSpace = NULL;
		/*slot's remot dev */
		
		listInit(&(pslotStatProcSpace->RemDevStatPorcNodeQuen));
		pslotStatProcSpace->RemDevStatPorcNodeNum = 0;
		
		scanTpNum = 0;
		pStatScanTypeNode = (stStatScanTypeDef *)listFirst(&(pgstStatTypeWholwInfo->statScanTypeQuen));
		while((pStatScanTypeNode != NULL) && (scanTpNum < pgstStatTypeWholwInfo->statScanTypeNum))
		{	
			/*in each slot, Init pPfmPreScanQuenNode Base statScanType, And PfmPreScanQuenNode's
			    portList use HASH, So when wo search for a port_stat_mon_node, we get a high speed because
			    slot, stat_scan_type, HASH */
			listAdd(&(pslotStatProcSpace->statPreScanTpQuen),(NODE*)pStatPreScanQuenNode);	
			pslotStatProcSpace->quenNodeNum++;
			
			for(k=0;k<GPN_STAT_PRESCAN_HASH;k++)
			{
				listInit(&(pStatPreScanQuenNode->statPreScanQuen[k]));
				pStatPreScanQuenNode->preScanNodeNum[k] = 0;

				listInit(&(pStatPreScanQuenNode->statPeerPortQuen[k]));
				pStatPreScanQuenNode->peerPortNodeNum[k] = 0;
			}
			
			/*hash key */
			pStatPreScanQuenNode->hashKey = GPN_STAT_PRESCAN_HASH;
			pStatPreScanQuenNode->statScanType = pStatScanTypeNode->statScanType;
			/*statPreScanQuenNode */
			pStatPreScanQuenNode->statScanTypeNode = pStatScanTypeNode;
			/*is use VariPTList*/
			pStatPreScanQuenNode->pVariPTList = NULL;

			scanTpNum++;
			pStatScanTypeNode = (stStatScanTypeDef *)listNext((NODE*)(pStatScanTypeNode));

			/*for each scan type*/
			pStatPreScanQuenNode++;
		}
		if((pStatScanTypeNode != NULL) || (scanTpNum < pgstStatTypeWholwInfo->statScanTypeNum))
		{
			gpnLog(GPN_LOG_L_ERR, "%s : pStatPreScanQuenNode init err\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}
	}
	
	return GPN_STAT_DBS_GEN_OK;
	
}
UINT32 gpnStatPortTypeVsStatScanTypeInit(stEQUStatProcSpace *pEQUStatProcSpace)
{
	UINT32 i;
	UINT32 k;
	UINT32 scanTpNum;
	UINT32 tmpSupPortTp;
	stUnitStatProcSpace *pslotStatProcSpace;
	stPortTpToStatScanTp *pPortTpToScanTp;
	stStatPTpVsSTpTemp *pStatPTpVsSTpTemp;
	stStatPreScanQuen *pStatPreScanQuenNode;
	stStatScanTypeDef *pStatScanTypeNode;

	/*assert */
	if(pEQUStatProcSpace == NULL)
	{
		GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : para NULL!\n\r",\
			__FUNCTION__);
		return GPN_STAT_DBS_GEN_ERR;
	}
	
	/*port type(i) to stat scan type(j) -- i : j = 1 : n*/
	tmpSupPortTp = sizeof(gStatPTpVsSTpTemp)/sizeof(gStatPTpVsSTpTemp[0]);

	/*port type to each slot's scan type -- stStatPreScanQuen *pStatPreScanNode[EQU_SLOT_MAX_NUM+1]*/
	/*port type base */
	k = tmpSupPortTp * sizeof(stPortTpToStatScanTp);
	pPortTpToScanTp = (stPortTpToStatScanTp*)malloc(k);
	memset((UINT8 *)(pPortTpToScanTp), 0, k);
	gpnLog(GPN_LOG_L_INFO, "%s : malloc %d size for stPortTpToStatScanTp, global base port type\n\r",\
		__FUNCTION__, k);

	/*Init PTpVsSTpRelation */
	listInit(&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));
	pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum = 0;
	pgstEQUStatProcSpace->PTpVsStatSTpRelation.pPortTpToScanTpMemspace = pPortTpToScanTp;

	pStatPTpVsSTpTemp = gStatPTpVsSTpTemp;
	pPortTpToScanTp = pgstEQUStatProcSpace->PTpVsStatSTpRelation.pPortTpToScanTpMemspace;	
	for(i=0;i<tmpSupPortTp;i++)
	{
		scanTpNum = 0;
		pStatScanTypeNode = (stStatScanTypeDef *)listFirst(&(pgstStatTypeWholwInfo->statScanTypeQuen));
		while((pStatScanTypeNode != NULL)&&(scanTpNum < pgstStatTypeWholwInfo->statScanTypeNum))
		{
			if(pStatPTpVsSTpTemp->statScanType == pStatScanTypeNode->statScanType)
			{
				pPortTpToScanTp->statScanType = pStatPTpVsSTpTemp->statScanType;
				pPortTpToScanTp->portType = pStatPTpVsSTpTemp->portType;
				pPortTpToScanTp->portTpName = pStatPTpVsSTpTemp->portTpName;
				
				pPortTpToScanTp->statDataSizeForPort = pStatScanTypeNode->statSTDTotalSize;
				pPortTpToScanTp->statScanTypeNode = pStatScanTypeNode;

				/*pStatPreScanNode init in next while */
				/*pPortTpToScanTp->pStatPreScanNode[slot] = */
				
				break;
			}
			
			scanTpNum++;
			pStatScanTypeNode = (stStatScanTypeDef *)listNext((NODE *)(pStatScanTypeNode));
		}
		if((pStatScanTypeNode == NULL) || (scanTpNum >= pgstStatTypeWholwInfo->statScanTypeNum))
		{
			GPN_STAT_DBS_PRINT(GPN_STAT_DBS_CMP, "%s : statScanTypeQuen & gStatPTpVsSTpTemp ScanType inconformity\n\r",\
				__FUNCTION__);
			return GPN_STAT_DBS_GEN_ERR;
		}
		listAdd(&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen),(NODE*)pPortTpToScanTp);
		pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum++;

		pPortTpToScanTp++;
		pStatPTpVsSTpTemp++;
	}
	
	/*pStatPreScanNode init*/
	for(i=0;i<(EQU_SLOT_MAX_NUM + 1);i++)
	{
		pslotStatProcSpace = pgstEQUStatProcSpace->unitStatProcSpace[i];
		/*1 : n, so put port type in outer while*/
		tmpSupPortTp = 0;
		pPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));
		while((pPortTpToScanTp != NULL)&&(tmpSupPortTp < pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum))
		{
			scanTpNum = 0;
			pStatPreScanQuenNode = (stStatPreScanQuen *)listFirst(&(pslotStatProcSpace->statPreScanTpQuen));
			while((pStatPreScanQuenNode != NULL)&&(scanTpNum < pslotStatProcSpace->quenNodeNum))
			{
				if(pStatPreScanQuenNode->statScanType == pPortTpToScanTp->statScanType)
				{
					pPortTpToScanTp->pStatPreScanNode[i] = pStatPreScanQuenNode;
					break;
				}
				
				scanTpNum++;
				pStatPreScanQuenNode = (stStatPreScanQuen *)listNext((NODE *)(pStatPreScanQuenNode));
			}
			
			tmpSupPortTp++;
			pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
		}
	
	}
	return GPN_STAT_DBS_GEN_OK;
}
UINT32 gpnStatDataBaseStructInit(void)
{	
	/*init pgstEQUStatProcSpace: 0 malloc space */
	gpnStatEQUStatProcSpaceMollac(&pgstEQUStatProcSpace);
	/*init pgstEQUStatProcSpace: 1 stat global info init */
	gpnStatGlobalInfoInit(pgstEQUStatProcSpace);
	/*init pgstEQUStatProcSpace: 2 stat task info init */
	gpnStatTaskInfoInit(pgstEQUStatProcSpace);
	/*init pgstEQUStatProcSpace: 3 stat event threshold templat init */
	gpnStatEventThredTempInit(pgstEQUStatProcSpace);
	/*init pgstEQUStatProcSpace: 4 stat alarm threshold templat init */
	gpnStatAlarmThredTempInit(pgstEQUStatProcSpace);
	/*init pgstEQUStatProcSpace: 5 stat sub type filt templat init */
	gpnStatSubFiltTempInit(pgstEQUStatProcSpace);
	/*init pgstEQUStatProcSpace: 5 stat sub type report templat init */
	gpnStatSubReportTempInit(pgstEQUStatProcSpace);
	/*init pgstEQUStatProcSpace: 6 stat hist data mgt init */
	gpnStatHistDataMgtInit(pgstEQUStatProcSpace);
	
	/*init pgstEQUStatProcSpace: 7 stat per-scan-per-slot init */
	gpnStatPerScanQuenInit(pgstEQUStatProcSpace);
	/*init pgstEQUStatProcSpace: 8 stat portType Vs scanType init */
	gpnStatPortTypeVsStatScanTypeInit(pgstEQUStatProcSpace);

	return GPN_STAT_DBS_GEN_OK;
}

UINT32 debugGpnStatDataBaseStruct(void)
{
	UINT32 hash;
	UINT32 slot;
	UINT32 tmpSupPortTp;
	UINT32 scanTpNum;
	//UINT32 scanPtNum;
	stPortTpToStatScanTp *pPortTpToScanTp;
	stStatPreScanQuen *pStatPreScanQuenNode;
	//stStatPreScanQuen *pTmpStatPreScanNode;
	
	tmpSupPortTp = 0;
	pPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(tmpSupPortTp < pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum))
	{
		scanTpNum = 0;
		pStatPreScanQuenNode = (stStatPreScanQuen *)listFirst(&(pgstEQUStatProcSpace->unitStatProcSpace[0]->statPreScanTpQuen));
		while((pStatPreScanQuenNode != NULL)&&(scanTpNum < pgstEQUStatProcSpace->unitStatProcSpace[0]->quenNodeNum))
		{
			if(pStatPreScanQuenNode->statScanType == pPortTpToScanTp->statScanType)
			{
				printf("portType(              name) scanType (              name)\n\r");
				printf("%08x(%18s) %08x (%18s)\n\r",\
					pPortTpToScanTp->portType, pPortTpToScanTp->portTpName,\
					pStatPreScanQuenNode->statScanType, pStatPreScanQuenNode->statScanTypeNode->statScanTypeName);

				printf("slot scanType preNodeP\n\r");
				for(slot=0;slot<(EQU_SLOT_MAX_NUM + 1);slot++)
				{
					printf("%04d %08x %08x\n\r",\
						slot, pPortTpToScanTp->pStatPreScanNode[slot]->statScanType,\
						(UINT32)pPortTpToScanTp->pStatPreScanNode[slot]);

					/*add after*/
					for(hash=0;hash<(GPN_STAT_PRESCAN_HASH);hash++)
					{
						//pTmpStatPreScanNode = pPortTpToScanTp->pStatPreScanNode[slot];
						
						//scanPtNum = 0;
					}
				}
				printf("\n\r");
				break;
			}
			scanTpNum++;
			pStatPreScanQuenNode = (stStatPreScanQuen *)listNext((NODE *)(pStatPreScanQuenNode));
			
		}
		
		tmpSupPortTp++;
		pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	return GPN_STAT_DBS_GEN_OK;
}


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_STAT_DATA_STRUCT_C_*/


