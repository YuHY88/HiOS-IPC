/**********************************************************
* file name: gpnAlmScan.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-09
* function: 
*    define scan and process alarms details
* modify:
*
***********************************************************/

#ifndef _GPN_ALM_SCAN_C_
#define _GPN_ALM_SCAN_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
/*
#include <vxWorks.h>
#include <msgQLib.h>
#include <semLib.h>
#include <taskLib.h>
#include <stdio.h>
#include <string.h>
#include <ioLib.h>
*/
#include <lib/syslog.h>
#include <lib/log.h>
#include <lib/devm_com.h>

#include "socketComm/gpnSockMsgDef.h"
#include "socketComm/gpnSockAlmMsgDef.h"
#include "gpnAlmTrapNotify.h"
#include "socketComm/gpnSockCommRoleMan.h"

#include "gpnAlmScan.h"
#include "gpnAlmTimeProc.h"
#include "gpnAlmAlmNotify.h"

#include "gpnAlmDataSheet.h"
#include "gpnAlmSnmp.h"
#include "gpnAlmCfg.h"

/*log function include*/
#include "gpnLog/gpnLogFuncApi.h"

/* for cmcc, tmp add for restrain */
#include "gpnAlmTmpRestrain.h"
#include "gpnAlmDebug.h"
#include "gpnAlmMasterSlaveSync.h"

/*socket communication global data:define in socket communication module*/
extern sockCommData gSockCommData;

extern gstAlmTypeWholwInfo *pgstAlmTypeWholwInfo;

stEQUAlmProcSpace *pgstAlmPreScanWholwInfo = NULL;

stAlmPTpVsSTpTemp gAlmPTpVsSTpTemp[] =
{
	{IFM_EQU_TYPE,				GPN_ALM_SCAN_TYPE_EQU,			"EQU"},
	{IFM_STMN_TYPE,				GPN_ALM_SCAN_TYPE_SDH_OH,	    "SDH_STM-N"},
	{IFM_SOFT_TYPE,				GPN_ALM_SCAN_TYPE_INB,			"DEV INB"},
	{IFM_ENV_TYPE,				GPN_ALM_SCAN_TYPE_ENV,			"DEV ENV"},
	{IFM_POWN48_TYPE,			GPN_ALM_SCAN_TYPE_POW,			"POW -48V"},
	{IFM_POW220_TYPE,			GPN_ALM_SCAN_TYPE_POW,			"POW 220V"},
	{IFM_FAN_TYPE,				GPN_ALM_SCAN_TYPE_FAN,			"FAN"},
	{IFM_V35_TYPE,				GPN_ALM_SCAN_TYPE_V35,			"V35"},
	{IFM_E1_TYPE,				GPN_ALM_SCAN_TYPE_PDH,			"PDH"},
	{IFM_PWE3E1_TYPE,			GPN_ALM_SCAN_TYPE_PWE,			"PWE3 E1"},
	{IFM_CFM_MEP_TYPE,			GPN_ALM_SCAN_TYPE_FUN_ETH_CFM,	"FUN CFM OAM"},
	{IFM_VLAN_OAM_TYPE,			GPN_ALM_SCAN_TYPE_FUN_ETH_VLAN,	"FUN ETH VLAN"},
	{IFM_ETH_CLC_IN_TYPE,		GPN_ALM_SCAN_TYPE_FUN_ETH_CLC,	"FUN ETH CLC IN"},
	{IFM_ETH_CLC_OUT_TYPE,		GPN_ALM_SCAN_TYPE_FUN_ETH_CLC,	"FUN ETH CLC OUT"},
	{IFM_ETH_CLC_SUB_TYPE,		GPN_ALM_SCAN_TYPE_FUN_ETH_CLC,	"FUN ETH CLC SUB"},
	{IFM_ETH_CLC_LINE_TYPE,		GPN_ALM_SCAN_TYPE_FUN_ETH_CLC,	"FUN ETH CLC LINE"},
	{IFM_FUN_ETH_TYPE,			GPN_ALM_SCAN_TYPE_FUN_ETH,		"FUN ETH"},	
	{IFM_VS_TYPE,				GPN_ALM_SCAN_TYPE_VS_OAM,		"PTN VS"},
	{IFM_LSP_TYPE,				GPN_ALM_SCAN_TYPE_LSP_OAM,		"PTN LSP"},
	{IFM_PW_TYPE,				GPN_ALM_SCAN_TYPE_PW_OAM,		"PTN PW"},	
	{IFM_PWE3STMN_TYPE,			GPN_ALM_SCAN_TYPE_PWE,			"PWE3 STM-N"},
	{IFM_ETH_SFP_TYPE,			GPN_ALM_SCAN_TYPE_ETH_SFP,		"ETH SFP"},
	{IFM_VPLSPW_TYPE,			GPN_ALM_SCAN_TYPE_VPLSPW_OAM,	"PTN VPLS PW"},
	{IFM_VUNI_TYPE,			    GPN_ALM_SCAN_TYPE_PTN_V_UNI,    "PTN V_UNI"},

	{IFM_FUN_ETH_SUB_TYPE,		GPN_ALM_SCAN_TYPE_FUN_ETH,		"FUN ETH SUB"},	//add for ipran
	{IFM_TUNNEL_TYPE,			GPN_ALM_SCAN_TYPE_TUNNEL_OAM,	"PTN TUNNEL"},	//add for ipran
	{IFM_E1_SUB_TYPE,			GPN_ALM_SCAN_TYPE_PDH,			"PDH SUB"}, 	//add for ipran
	{IFM_BFD_TYPE,				GPN_ALM_SCAN_TYPE_BFD_OAM,		"BFD"}, 		//add for ipran
	{IFM_TRUNK_TYPE,			GPN_ALM_SCAN_TYPE_FUN_ETH,		"TRUNK"}, 		//add for ipran
	{IFM_TRUNK_SUBIF_TYPE,		GPN_ALM_SCAN_TYPE_FUN_ETH,		"TRUNK SUB"},	//add for ipran
	{IFM_VS_2_TYPE,				GPN_ALM_SCAN_TYPE_VS_OAM,		"IPRAN VS"}, 	//add for ipran
	{IFM_LSP_2_TYPE,			GPN_ALM_SCAN_TYPE_LSP_OAM,		"IPRAN LSP"}, 	//add for ipran
	{IFM_PW_2_TYPE,				GPN_ALM_SCAN_TYPE_PW_OAM,		"IPRAN PW"}, 	//add for ipran
		
	{IFM_VC4_TYPE,				GPN_ALM_SCAN_TYPE_SDH_HP,		"SDH_PHY_VC4"},
	{IFM_VC12_TYPE,				GPN_ALM_SCAN_TYPE_SDH_LP,		"SDH_PHY_VC12"},
	/*{IFM_ETH_TYPE,				GPN_ALM_SCAN_TYPE_ETH	,	"ETH"},*/
};

UINT32 gpnAlmMakePortIndex(UINT32 devIndex, UINT32 devPortTpNum,
	stAlmPortObjCL *pPortObjCL, objLogicDesc *portIndexBuff, objLogicDesc *upPtBuff)
{
	objLogicDesc defPortInfo;
	
	UINT32 portIndex;
	UINT32 slotId;
	
	UINT32 sdh_stmn_phy_port;
	
	UINT32 sdh_phy_vc4_port;

	UINT32 sdh_phy_vc12_port;

	UINT32 smp_phy_port;

	UINT32 lastVc4FarthPT;
	UINT32 lastVc12FarthPT;
	UINT32 portNumb;
		
	UINT32 i;
	UINT32 j;

	defPortInfo.devIndex = GPN_ALM_SCAN_32_FFFF;
	defPortInfo.portIndex = GPN_ALM_SCAN_32_FFFF;
	defPortInfo.portIndex3 = GPN_ALM_SCAN_32_FFFF;
	defPortInfo.portIndex4 = GPN_ALM_SCAN_32_FFFF;
	defPortInfo.portIndex5 = GPN_ALM_SCAN_32_FFFF;
	defPortInfo.portIndex6 = GPN_ALM_SCAN_32_FFFF;
	
	sdh_stmn_phy_port = 0;
	sdh_phy_vc4_port = 0;
	sdh_phy_vc12_port = 0;
	
	lastVc4FarthPT = IFM_ALL_TYPE;
	lastVc12FarthPT = IFM_ALL_TYPE;
	//retValue = GPN_ALM_GEN_YES;

	portNumb = 0;
	slotId = DeviceIndex_GetSlot(devIndex);
	/*端口能力通告重整理，主要是为了解决PDH端口中低级
	时隙不确定的问题才这样做，如有的PDH中有2个低阶，有
	的却可能有4个低级,此函数随端口能力通告发起模块而变*/
	
	for(i=0;i<devPortTpNum;i++)
	{	
		if(pPortObjCL[i].portNum >= 100)
		{
			GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : illegal port num %d err\n\r",\
				__FUNCTION__, portNumb);
			gpnLog(GPN_LOG_L_ALERT, "%s : illegal port num %d err\n\r",\
				__FUNCTION__, portNumb);
			return GPN_ALM_GEN_NO;
		}
		
		switch(pPortObjCL[i].portType)
		{
			case IFM_STMN_TYPE:
				if(pPortObjCL[i].portNum != 1)
				{
					GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : villed STM10 port num %d\n\r",\
						__FUNCTION__, pPortObjCL[i].portNum);
					gpnLog(GPN_LOG_L_ALERT, "%s : villed STM10 port num %d\n\r",\
						__FUNCTION__, pPortObjCL[i].portNum);
					return GPN_ALM_GEN_NO;
				}

				portIndexBuff[portNumb] = defPortInfo;
				upPtBuff[portNumb] = defPortInfo;
				
				sdh_stmn_phy_port++;
				lastVc4FarthPT = IFM_STMN_TYPE;
				portIndex = SDH_PortIndex_Create(IFM_STMN_TYPE,slotId,
					sdh_stmn_phy_port,0,0);
				portIndexBuff[portNumb].devIndex = devIndex;
				portIndexBuff[portNumb].portIndex = portIndex;
				upPtBuff[portNumb].devIndex = GPN_ALM_SCAN_32_NULL;
				upPtBuff[portNumb].portIndex = GPN_ALM_SCAN_32_NULL;
				portNumb++;

				sdh_phy_vc4_port = 0;
				break;
				
			case IFM_VC4_TYPE:
				if(lastVc4FarthPT != IFM_STMN_TYPE)
				{
					GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : villed PHYVC4 farth port type %8x\n\r",\
						__FUNCTION__, lastVc4FarthPT);
					gpnLog(GPN_LOG_L_ALERT, "%s :villed PHYVC4 farth port type %8x\n\r",\
						__FUNCTION__, lastVc4FarthPT);
					return GPN_ALM_GEN_NO;
				}
				if(pPortObjCL[i].portNum != 1)
				{
					GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : villed PHYVC4 port num %d\n\r",\
						__FUNCTION__, pPortObjCL[i].portNum);
					gpnLog(GPN_LOG_L_ALERT, "%s : villed PHYVC4 port num %d\n\r",\
						__FUNCTION__, pPortObjCL[i].portNum);
					return GPN_ALM_GEN_NO;
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
					GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : villed PHYVC4 farth port type %8x\n\r",\
						__FUNCTION__, lastVc4FarthPT);
					return GPN_ALM_GEN_NO;
				}
				if(lastVc12FarthPT != IFM_VC4_TYPE)
				{
					GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : villed PHYVC12 farth port type %8x\n\r",\
						__FUNCTION__, lastVc12FarthPT);
					return GPN_ALM_GEN_NO;
				}
				if(pPortObjCL[i].portNum != 63)
				{
					GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : villed VC12 port num %d\n\r",\
						__FUNCTION__, pPortObjCL[i].portNum);
					return GPN_ALM_GEN_NO;
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
					upPtBuff[portNumb].devIndex = GPN_ALM_SCAN_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_ALM_SCAN_32_NULL;
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
					upPtBuff[portNumb].devIndex = GPN_ALM_SCAN_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_ALM_SCAN_32_NULL;
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
					upPtBuff[portNumb].devIndex = GPN_ALM_SCAN_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_ALM_SCAN_32_NULL;
					portNumb++;
				}
				break;

			case IFM_FUN_ETH_SUB_TYPE:
				/* 待添加 */
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
					upPtBuff[portNumb].devIndex = GPN_ALM_SCAN_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_ALM_SCAN_32_NULL;
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
					upPtBuff[portNumb].devIndex = GPN_ALM_SCAN_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_ALM_SCAN_32_NULL;
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
					upPtBuff[portNumb].devIndex = GPN_ALM_SCAN_32_NULL;
					upPtBuff[portNumb].portIndex = GPN_ALM_SCAN_32_NULL;
					portNumb++;
				}
				break;

			default:
				GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : villed port type %08x\n\r",\
					__FUNCTION__, pPortObjCL[i].portType);
				return GPN_ALM_GEN_NO;
		}
	}
	return GPN_ALM_GEN_YES;
}

#if 0
UINT32 gpnAlmAutoInversNotify(stAlmLocalNode *pAlmLocalPort, UINT32 almSubType)
{
	stAlmTrapMsgSt trapMsg;
	stAlmProducTrap *pAutoRevs;

	trapMsg.msgType = GPN_ALM_TRAP_THYE_CURR;
	trapMsg.msgCtrl = 0;
	pAutoRevs = (stAlmProducTrap *)(trapMsg.msgInfo);
	pAutoRevs->ifIndex = pAlmLocalPort->localPort.portIndex;
	pAutoRevs->almType = almSubType;
	pAutoRevs->ifIndex2 = pAlmLocalPort->localPort.portIndex3;
	pAutoRevs->ifIndex3 = pAlmLocalPort->localPort.portIndex4;
	pAutoRevs->ifIndex4 = pAlmLocalPort->localPort.portIndex5;
	pAutoRevs->ifIndex5 = pAlmLocalPort->localPort.portIndex6;

	gpnAlmTrapMsgQuenSend(GPN_ALM_TRAP_THYE_AUTO_R, &trapMsg, GPN_ALM_TRAP_STRUCT_BLEN);
	return GPN_ALM_GEN_YES;
}
#endif

UINT32 gpnAlmAutoInversNotify(stAlmLocalNode *pAlmLocalPort, UINT32 almSubType)
{	
	stSnmpAlmAttribCfgTableInfo stInfo;
	memset (&stInfo, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
	stInfo.index.iAlmSubType = almSubType;
	stInfo.index.iAttrDevIndex = pAlmLocalPort->localPort.devIndex;
	stInfo.index.iAttrPortIndex1 = pAlmLocalPort->localPort.portIndex;
	stInfo.index.iAttrPortIndex2 = pAlmLocalPort->localPort.portIndex3;
	stInfo.index.iAttrPortIndex3 = pAlmLocalPort->localPort.portIndex4;
	stInfo.index.iAttrPortIndex4 = pAlmLocalPort->localPort.portIndex5;
	stInfo.index.iAttrPortIndex5 = pAlmLocalPort->localPort.portIndex6;

	alarm_send_trap ((char *)&stInfo, ALM_EVENT_REPORT_TRAP);
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmAutoInversRecoverDSModify(stAlmLocalNode *pAlmLocalPort, UINT32 almSubType)
{
	if(pAlmLocalPort)
	{
		
		return GPN_ALM_GEN_YES;
	}
	else
	{
		return GPN_ALM_GEN_ERR;
	}
}
UINT32 gpnAlmBackPortNodeGet(void)
{
	return GPN_ALM_GEN_YES;
}

UINT32 gpnAlmUpperAlmPassProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort)
{
	UINT32 i;
	UINT32 tempAlmData;
	stAlmScanPortInfo *pAlmScanPort;

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	
	/*抑制类内记录实际使用页数*/
	for(i=0; i<pAlmScanType->almUsePag; i++)
	{
		tempAlmData = pAlmScanPort->iScanAlmDate[i] & (~pAlmScanType->scanTypeAlmMask[i]);
		tempAlmData |= (pAlmScanPort->iScanAlmDate[i] & GPN_ALM_PAGE_MASK);
		tempAlmData = tempAlmData ^ pAlmScanPort->pUppeAlmScanPort->iRestAlmDate[i];
		if(tempAlmData != 0)
		{
			pAlmScanPort->iScanAlmDate[i] &= pAlmScanType->scanTypeAlmMask[i];
			pAlmScanPort->iScanAlmDate[i] |= pAlmScanPort->pUppeAlmScanPort->iRestAlmDate[i];
			pAlmScanPort->iFrehAlmMark[i] |= tempAlmData;
			pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM]++;
		}
	}
	
	return GPN_ALM_GEN_YES;
}

UINT32 gpnAlmScreenProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort)
{	
	UINT32 i;
	UINT32 iBaseAlmDate[GPN_ALM_PAGE_NUM];
	stAlmScanPortInfo *pAlmScanPort;

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	
	/*基于类型做告警屏蔽与基于端口做告警屏蔽的组合功能，基于端口的优先*/
	/*操作为安位与操作，故0为屏蔽，1为不屏蔽，默认为不屏蔽*/
	/*isScreBasePort表示屏蔽是否基于端口配置，0为不基于，1为基于，默认为0*/

	/*基于类型做告警屏蔽,基于端口屏蔽的告警子类不受基于扫描类的屏蔽配置影响*/
	for(i=0; i<pAlmScanType->almUsePag; i++)
	{
		if(pAlmScanPort->iFrehAlmMark[i] != 0)
		{
			/*基于扫描类型屏蔽*/
			iBaseAlmDate[i] = pAlmScanPort->iScanAlmDate[i] & \
				(pAlmScanType->almScreInfo[i] | pAlmScanPort->isScreBasePort[i]);
			/*基于端口屏蔽*/
			iBaseAlmDate[i] = iBaseAlmDate[i] & \
				(pAlmScanPort->iScreAlmInfo[i] | (~(pAlmScanPort->isScreBasePort[i])));
			
			pAlmScanPort->iRestAlmMark |= iBaseAlmDate[i] ^ pAlmScanPort->iBaseAlmDate[i];
			pAlmScanPort->iBaseAlmDate[i] = iBaseAlmDate[i];
		}
	}
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmRestrainProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort)
{
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubTpStr;
	UINT32 iBaseAlmDate[GPN_ALM_PAGE_NUM]={0};
	UINT32 restChg;
	UINT32 bitMask;
	UINT32 bitsInPage;
	UINT32 i;
	UINT32 j;
	UINT32 k;
	
	pAlmScanPort = pAlmLocalPort->pAlmScanPort;

	/*抑制类的处理*/
	if(pgstAlmPreScanWholwInfo->almGlobalCfg.restMode == GPN_ALM_REST_LEVEL_MODE)
	{
		for(i=0; i<pAlmScanType->almUsePag; i++)
		{
			iBaseAlmDate[i] = pAlmScanPort->iBaseAlmDate[i];
			if(iBaseAlmDate[i] == 0)
			{
				continue;
			}
			
			if((i+1) == pAlmScanType->almUsePag)
			{
				if( (pAlmScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE) == 0)
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
			
			for(j=0; j<bitsInPage; j++)
			{
				if(iBaseAlmDate[i] & (((UINT32)(1U))<<j))
				{
					pAlmSubTpStr = NULL;
					gpnAlmSeekScanTypeToSubType(pAlmScanType,(i*GPN_ALM_PAGE_BIT_SIZE+j),&pAlmSubTpStr);
					if(pAlmSubTpStr != NULL)
					{
						/*抑制类内的抑制*/
						for(k=0; k<pAlmScanType->almUsePag; k++)
						{
							iBaseAlmDate[k] &= pAlmSubTpStr->restrainMask[k];
							
						}
					}
				}
			}
		}
	}
	else if(pgstAlmPreScanWholwInfo->almGlobalCfg.restMode == GPN_ALM_REST_TIME_MODE)
	{
		for(i=0; i<pAlmScanType->almUsePag; i++)
		{
			iBaseAlmDate[i] = pAlmScanPort->iBaseAlmDate[i];
			if(iBaseAlmDate[i] == 0)
			{
				continue;
			}
		}
	}
	else
	{
		for(i=0; i<pAlmScanType->almUsePag; i++)
		{
			iBaseAlmDate[i] = pAlmScanPort->iBaseAlmDate[i];
		}
	}

	/*抑制类关系脱离???*/
	for(i=0; i<pAlmScanType->almUsePag; i++)
	{
		restChg = iBaseAlmDate[i] ^ pAlmScanPort->iRestAlmDate[i];
		if(restChg == 0)
		{
			continue;
		}

		if((i+1) == pAlmScanType->almUsePag)
		{
			if( (pAlmScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE) == 0)
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

		for(j=0; j<bitsInPage; j++)
		{
			bitMask = (((UINT32)(1U))<<j);
			if(restChg & bitMask)
			{
				/*扫描类中的子类告警在抑制类中的bit位不连续，需要特殊查找*/
				k = GPN_ALM_SCAN_32_FFFF;
				gpnAlmSeekAlmUseBitPToSubTypeNumInScanType(pAlmScanType,(i*GPN_ALM_PAGE_BIT_SIZE+j),&k);
				if(k != GPN_ALM_SCAN_32_FFFF)
				{
					if(pAlmScanPort->iRestAlmDate[i]/*原来值*/ & bitMask)
					{
						pAlmScanPort->AlmValueBuff[k].iAlmValue = GPN_ALM_CLEAN;
						/*原来有告警,还没产生完,现在无告警*/
						if(pAlmScanPort->AlmValueBuff[k].iDelayCount != 0)
						{					
							pAlmScanPort->AlmValueBuff[k].iDelayCount = 0;
						}
						else
						{
							pAlmScanPort->AlmValueBuff[k].iDelayCount =\
								pgstAlmPreScanWholwInfo->almGlobalCfg.disapDelay;
							if((pAlmScanPort->iDelyAlmMark[i] & bitMask) == 0)
							{
								pAlmScanPort->iDelyAlmMark[i] |= bitMask;
								pAlmScanPort->iDelyAlmMark[GPN_ALM_PAGE_NUM]++;
							}
							GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : clear %d %08x dly %d\n\r",\
								__FUNCTION__, j, pAlmScanPort->iDelyAlmMark[i], pAlmScanPort->AlmValueBuff[k].iDelayCount);
						}
					}
					else
					{
						pAlmScanPort->AlmValueBuff[k].iAlmValue = GPN_ALM_ARISE;
						/*原来无告警,还没消失完,现在有告警*/
						if(pAlmScanPort->AlmValueBuff[k].iDelayCount != 0)
						{
							pAlmScanPort->AlmValueBuff[k].iDelayCount = 0;
						}
						else
						{
							pAlmScanPort->AlmValueBuff[k].iDelayCount = \
								pgstAlmPreScanWholwInfo->almGlobalCfg.prodDelay;
							if((pAlmScanPort->iDelyAlmMark[i] & bitMask) == 0)
							{
								pAlmScanPort->iDelyAlmMark[i] |= bitMask;
								pAlmScanPort->iDelyAlmMark[GPN_ALM_PAGE_NUM]++;
							}
							GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : rise %d %08x dly %d\n\r",\
								__FUNCTION__, j, pAlmScanPort->iDelyAlmMark[i], pAlmScanPort->AlmValueBuff[k].iDelayCount);
						}
					}
				}
				else
				{
					GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : almScanType(%8x), page(%d)bit(%d), err!\n\r",\
						__FUNCTION__, pAlmScanType->almScanType,i, j);
				}
			}
		}
		pAlmScanPort->iRestAlmDate[i] = iBaseAlmDate[i];
	}

	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmInvesalDlyInProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort)
{
	stAlmScanPortInfo *pAlmScanPort;
	UINT32 InveDlyIn;
	UINT32 bitPos;
	UINT32 bitsInPage;
	UINT32 i;
	UINT32 j;
	UINT32 k;

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	
	/*反转配置信息为:0为不反转，1为反转，默认为不反转*/
	/*反转的告警信息来源与pAlmScanPort->iScanAlmDate，即驱动层的扫描信息*/
	/*这里只做反转相关延时的进入，实际的反转在延时结束后进行*/
	if(pgstAlmPreScanWholwInfo->almGlobalCfg.reverMode == GPN_ALM_INVE_MODE_DISABLE)
	{
		for(i=0; i<pAlmScanType->almUsePag; i++)
		{
			pAlmScanPort->iFrehAlmMark[i] = 0;
		}
	}
	else
	{
		for(i=0; i<pAlmScanType->almUsePag; i++)
		{
			InveDlyIn = pAlmScanPort->iInveAlmInfo[i] & pAlmScanPort->iFrehAlmMark[i];
			/*Fresh info clear*/
			pAlmScanPort->iFrehAlmMark[i] = 0;
			
			if(InveDlyIn == 0)
			{
				continue;
			}
			
			if((i+1) == pAlmScanType->almUsePag)
			{
				if( (pAlmScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE) == 0)
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
			
			for(j=0; j<bitsInPage; j++)
			{
				bitPos = (((UINT32)(1U))<<j);
				if(InveDlyIn & bitPos)
				{
					k = GPN_ALM_SCAN_32_FFFF;
					gpnAlmSeekAlmUseBitPToSubTypeNumInScanType(pAlmScanType,(i*GPN_ALM_PAGE_BIT_SIZE+j),&k);
					if(k != GPN_ALM_SCAN_32_FFFF)
					{
						if(pAlmScanPort->iScanAlmDate[i] & bitPos)
						{						
							/*简化处理，直接按产生延时处理*/
							pAlmScanPort->AlmValueBuff[k].iDelayCount =\
								pgstAlmPreScanWholwInfo->almGlobalCfg.prodDelay;
						}
						else /*if((pAlmScanPort->iScanAlmDate & bitPos) == 0)*/
						{
							/*简化处理，直接按消失延时处理*/
							pAlmScanPort->AlmValueBuff[k].iDelayCount =\
								pgstAlmPreScanWholwInfo->almGlobalCfg.disapDelay;
						}
						
						if((pAlmScanPort->iDelyAlmMark[i] & bitPos) == 0)
						{
							pAlmScanPort->iDelyAlmMark[i] |= bitPos;
							pAlmScanPort->iDelyAlmMark[GPN_ALM_PAGE_NUM]++;
						}
					}
				}
			}
		}
	}
	return GPN_ALM_GEN_YES;	
}

UINT32 gpnAlmDlyProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort)
{
	stAlmScanPortInfo *pAlmScanPort;
	UINT32 bitsInPage;
	UINT32 bitMask;
	UINT32 i;
	UINT32 j;
	UINT32 k;

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	
	for(i=0; i<pAlmScanType->almUsePag; i++)
	{
		if(pAlmScanPort->iDelyAlmMark[i] == 0)
		{
			continue;
		}
		
		if((i+1) == pAlmScanType->almUsePag)
		{
			if( (pAlmScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE) == 0)
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

		for(j=0; j<bitsInPage; j++)
		{
			bitMask = (((UINT32)(1U))<<j);
			if(pAlmScanPort->iDelyAlmMark[i] & bitMask)
			{
				k = GPN_ALM_SCAN_32_FFFF;
				gpnAlmSeekAlmUseBitPToSubTypeNumInScanType(pAlmScanType, (i*GPN_ALM_PAGE_BIT_SIZE+j), &k);
				if(k == GPN_ALM_SCAN_32_FFFF)
				{
					return GPN_ALM_GEN_ERR;
				}
				if(pAlmScanPort->AlmValueBuff[k].iDelayCount > 0)
				{
					pAlmScanPort->AlmValueBuff[k].iDelayCount--;
				}
				if(pAlmScanPort->AlmValueBuff[k].iDelayCount == 0)
				{
					/*延时结束*/
					pAlmScanPort->iDelyAlmMark[i] &= (~bitMask);
					if(pAlmScanPort->iDelyAlmMark[GPN_ALM_PAGE_NUM] > 0)
					{
						pAlmScanPort->iDelyAlmMark[GPN_ALM_PAGE_NUM]--;
					}
					/*锁定变化的告警，供后继处理*/
					pAlmScanPort->iSureAlmMark[i] |= bitMask;
					pAlmScanPort->iSureAlmMark[GPN_ALM_PAGE_NUM]++;
					/*供告警反转的激活用
					pAlmScanPort->iFrehAlmMark[i] |= bitMask;
					pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM]++;*/
					/*上报告警的动作*/
					if(pAlmScanPort->AlmValueBuff[k].iAlmValue == GPN_ALM_ARISE)
					{
						pAlmScanPort->iInveAlmDate[i] |= bitMask;
					}
					else
					{
						pAlmScanPort->iInveAlmDate[i] &= (~bitMask);
					}
				}
			}
		}
	}
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmInvesalProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort)
{
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubTpStr;
	UINT64 InveDlyOut;
	UINT32 bitsInPage;
	UINT64 bitPos;
	UINT32 i;
	UINT32 j;

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;

	/*反转配置信息为:0为不反转，1为反转，默认为不反转*/
	/*pAlmScanPort->iSureAlmMark视为告警变化(含反转变化)通过延时确认后的信息*/
	for(i=0; i<pAlmScanType->almUsePag; i++)
	{
		InveDlyOut = pAlmScanPort->iInveAlmInfo[i] & pAlmScanPort->iSureAlmMark[i];
		if(InveDlyOut == 0)
		{
			continue;
		}

		if((i+1) == pAlmScanType->almUsePag)
		{
			if( (pAlmScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE) == 0)
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

		for(j=0; j<bitsInPage; j++)
		{
			bitPos = (((UINT32)(1U))<<j);
			if(InveDlyOut & bitPos)
			{
				if(pgstAlmPreScanWholwInfo->almGlobalCfg.reverMode == GPN_ALM_INVE_MODE_AUTO)
				{
					if(pAlmScanPort->iScanAlmDate[i] & bitPos)
					{
						/*自动恢复模式下，反转延期倒时，驱动有告警，人机界面做无告警处理*/
						pAlmScanPort->iInveAlmDate[i] &= (~bitPos);
						GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : auto inve scanTp %8x's %d-th alm to clean\n\r",\
							__FUNCTION__, pAlmScanType->almScanType,(i*GPN_ALM_PAGE_BIT_SIZE+j));
					}
					else
					{
						/*自动恢复模式下，反转延期倒时，驱动无告警，反转自动结束，人机界面做无告警处理*/
						/*按pAlmScanPort->iInveAlmDate[i]原有值*/
						/*pAlmScanPort->iInveAlmDate[i] &= (~bitPos);*/
						GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : auto inve scanTp %8x's %d-th alm to clean,inve auto end\n\r",\
							__FUNCTION__, pAlmScanType->almScanType,(i*GPN_ALM_PAGE_BIT_SIZE+j));

						/*发送配置改变消息 ???trap & db modify*/
						pAlmScanPort->iInveAlmInfo[i] &= (~bitPos);
						pAlmSubTpStr = NULL;
						gpnAlmSeekScanTypeToSubType(pAlmScanType, (i*GPN_ALM_PAGE_BIT_SIZE+j), &pAlmSubTpStr);
						if(pAlmSubTpStr != NULL)
						{
							gpnAlmAutoInversRecoverDSModify(pAlmLocalPort, pAlmSubTpStr->almSubType);
							gpnAlmAutoInversNotify(pAlmLocalPort, pAlmSubTpStr->almSubType);
						}
					}
				}
				else if(pgstAlmPreScanWholwInfo->almGlobalCfg.reverMode == GPN_ALM_INVE_MODE_MANU)
				{
					if(pAlmScanPort->iScanAlmDate[i] & bitPos)
					{
						/*手动恢复模式下，反转延期倒时，驱动有告警，人机界面做无告警处理*/
						pAlmScanPort->iInveAlmDate[i] &= (~bitPos);
						GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : manu inve scanTp %8x's %d-th alm to clean\n\r",\
							__FUNCTION__, pAlmScanType->almScanType,(i*GPN_ALM_PAGE_BIT_SIZE+j));
					}
					else
					{
						/*手动恢复模式下，反转延期倒时，驱动无告警，人机界面做有告警处理*/
						pAlmScanPort->iInveAlmDate[i] |= bitPos;
						GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : manu inve scanTp %8x's %d-th alm to rise\n\r",\
							__FUNCTION__, pAlmScanType->almScanType,(i*GPN_ALM_PAGE_BIT_SIZE+j));
					}
				}
				else if(pgstAlmPreScanWholwInfo->almGlobalCfg.reverMode == GPN_ALM_INVE_MODE_DISABLE)
				{
					/*按pAlmScanPort->iInveAlmDate[i]原有值*/
				}
				else
				{
					return GPN_ALM_GEN_ERR;
				}
			}
		}
	}
	return GPN_ALM_GEN_YES;
}

UINT32 gpnAlmFilterProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort)
{
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubType;
	UINT32 bitsInPage;
	UINT32 bitPos;
	UINT32 i;
	UINT32 j;
	UINT32 k;

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;

	/*过滤前写数据表*/
	for(i=0; i<pAlmScanType->almUsePag; i++)
	{
		if(pAlmScanPort->iSureAlmMark[i] == 0)
		{
			continue;
		}

		if((i+1) == pAlmScanType->almUsePag)
		{
			if( (pAlmScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE) == 0)
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

		for(j=0; j<bitsInPage; j++)
		{
			bitPos = (((UINT32)(1U))<<j);
			if(pAlmScanPort->iSureAlmMark[i] & bitPos)
			{
				/*找出告警子类*/
				pAlmSubType = NULL;
				gpnAlmSeekScanTypeToSubType(pAlmScanType, (i*GPN_ALM_PAGE_BIT_SIZE+j), &pAlmSubType);
				if(pAlmSubType == NULL)
				{
					return GPN_ALM_GEN_ERR;
				}
				/*修改子类参数:目前为告警级别，还可包括其他信息*/
				k = pAlmSubType->devOrderInScanTp;
				/*考虑系统中告警过滤的功能，写入数据表时，对于过滤与否做差异化处理澹(即记录的信息有过不过滤的标志)*/
				
				if( ( (pAlmScanType->almFiltInfo[i] & bitPos) &&\
					  ((pAlmScanPort->isFiltBasePort[i] & bitPos) == 0) ) ||\
					( (pAlmScanPort->iFiltAlmInfo[i] & bitPos) &&\
					  (pAlmScanPort->isFiltBasePort[i] & bitPos) ) )
				{
					/*不过滤*/
					if( (pAlmScanPort->iInveAlmDate[i] & bitPos)/*产生*/ &&
						((pAlmScanPort->iDbSvAlmDate[i] & bitPos) == 0)/*未写入*/)
					{
						/*嵌入数据表当前告警表写入告警*/
						GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : almSubTp(0x%08x)(%20s) record in CURR_ALM_SYS_DATABASE\n\r",\
							__FUNCTION__, pAlmSubType->almSubType,pAlmSubType->almName);
						/*告警灯/铃处理*/
						/*???*/
						gpnAlmANtCurrAlmInsertDS(&(pAlmLocalPort->viewPort), &(pAlmScanPort->AlmValueBuff[k]));
						pAlmScanPort->iDbSvAlmDate[i] |= bitPos;

						/* 本盘为主盘且备盘存在，将告警放入同步告警表中，2018/8/16 */
						if((DEVM_HA_MASTER == alarm_MS_my_status_get()) &&
							(alarm_MS_slave_board_slot_get()) &&
							(alarm_MS_slave_power_done()))
						{
							alarm_new_curr_alarm_sync_table_add(&(pAlmLocalPort->viewPort), 
								&(pAlmScanPort->AlmValueBuff[k]), GPN_SOCK_MSG_OPT_RISE);
						}
					}
					else if(((pAlmScanPort->iInveAlmDate[i] & bitPos) == 0)/*消失*/ &&
						 	(pAlmScanPort->iDbSvAlmDate[i] & bitPos)/*已写入*/)
					{
						/*嵌入数据表当前告警表清除告警*/
						GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : almSubTp(0x%08x)(%20s) clean up CURR_ALM_SYS_DATABASE\n\r",\
							__FUNCTION__, pAlmSubType->almSubType,pAlmSubType->almName);
						gpnAlmANtCurrAlmOutDS(&(pAlmLocalPort->viewPort), &(pAlmScanPort->AlmValueBuff[k]));

						/* 本盘为主盘且备盘存在，将告警消除放入同步告警表中，2018/8/16 */
						if((DEVM_HA_MASTER == alarm_MS_my_status_get()) &&
							(alarm_MS_slave_board_slot_get()) &&
							(alarm_MS_slave_power_done()))
						{
							alarm_new_curr_alarm_sync_table_add(&(pAlmLocalPort->viewPort), 
								&(pAlmScanPort->AlmValueBuff[k]), GPN_SOCK_MSG_OPT_CLEAN);
						}
						
						/*是否写历史表*/
						if( ( (pAlmScanType->almReckInfo[i] & bitPos) &&\
					 	   	  ((pAlmScanPort->isReckBasePort[i] & bitPos) == 0) ) ||\
						    ( (pAlmScanPort->iFiltAlmInfo[i] & bitPos) &&\
					  	      (pAlmScanPort->isReckBasePort[i] & bitPos) ) )
						{
							GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : almSubTp(0x%08x) hist db in\n\r",\
								__FUNCTION__, pAlmSubType->almSubType);
							/*printf("%s : almSubTp(0x%08x) hist db in dev order %d num %d rank %d cnt %d\n\r",\
								__FUNCTION__, pAlmSubType->almSubType, k, pAlmScanPort->almNumInScanPort,\
								pAlmScanPort->AlmValueBuff[k].portAlmRank, pAlmScanPort->AlmValueBuff[k].prodCnt);*/
							gpnAlmANtHistAlmInsertDS(&(pAlmLocalPort->viewPort), &(pAlmScanPort->AlmValueBuff[k]));

							/* 本盘为主盘且备盘存在，将告警消除放入同步告警表中，2018/8/16 */
							if((DEVM_HA_MASTER == alarm_MS_my_status_get()) &&
								(alarm_MS_slave_board_slot_get()) &&
								(alarm_MS_slave_power_done()))
							{
								alarm_new_hist_alarm_sync_table_add(&(pAlmLocalPort->viewPort), 
									&(pAlmScanPort->AlmValueBuff[k]));
							}
						}
						pAlmScanPort->iDbSvAlmDate[i] &= (~bitPos);
					}
				}
				else if( ( ((pAlmScanType->almFiltInfo[i] & bitPos) == 0) &&\
					 	   ((pAlmScanPort->isFiltBasePort[i] & bitPos) == 0) ) ||\
						 ( ((pAlmScanPort->iFiltAlmInfo[i] & bitPos) == 0) &&\
					  	   (pAlmScanPort->isFiltBasePort[i] & bitPos) ) )
				{
					/*过滤*/
					if(pAlmScanPort->iDbSvAlmDate[i] & bitPos)/*已写入*/
					{
						/*嵌入数据表当前告警表清除告警*/
						GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : almSubTp(0x%08x)(%20s) clean up CURR_ALM_SYS_DATABASE,so filted\n\r",\
							__FUNCTION__, pAlmSubType->almSubType,pAlmSubType->almName);
						/*告警灯/铃处理*/
						/*???*/
						gpnAlmANtCurrAlmOutDS(&(pAlmLocalPort->viewPort), &(pAlmScanPort->AlmValueBuff[k]));
						pAlmScanPort->iDbSvAlmDate[i] &= (~bitPos);

						/* 本盘为主盘且备盘存在，将告警消除放入同步告警表中，2018/8/16 */
						if((DEVM_HA_MASTER == alarm_MS_my_status_get()) &&
							(alarm_MS_slave_board_slot_get()) &&
							(alarm_MS_slave_power_done()))
						{
							alarm_new_curr_alarm_sync_table_add(&(pAlmLocalPort->viewPort), 
								&(pAlmScanPort->AlmValueBuff[k]), GPN_SOCK_MSG_OPT_CLEAN);
						}
					}
				}
				else
				{
					return GPN_ALM_GEN_ERR;
				}
			}
		}
	}

	return GPN_ALM_GEN_YES;
}

UINT32 gpnAlmReportProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort)
{
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubType;
	UINT32 bitsInPage;
	UINT64 bitPos;
	UINT32 i;
	UINT32 j;
	UINT32 k;
	
	pAlmScanPort = pAlmLocalPort->pAlmScanPort;

	/* almReport config means : when alarm status change(arise
	    to dissp or dissp to arise), if or not notify easyView */
	
	/* after filt, alarm who was filted, can not see changes here */
	for(i=0; i<pAlmScanType->almUsePag; i++)
	{
		if(pAlmScanPort->iSureAlmMark[i] == 0)
		{
			continue;
		}

		if((i+1) == pAlmScanType->almUsePag)
		{
			if( (pAlmScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE) == 0)
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

		for(j=0; j<bitsInPage; j++)
		{
			bitPos = (((UINT32)(1U))<<j);
			if(pAlmScanPort->iSureAlmMark[i] & bitPos)
			{
				/* find subType alarm */
				pAlmSubType = NULL;
				gpnAlmSeekScanTypeToSubType(pAlmScanType, (i*GPN_ALM_PAGE_BIT_SIZE+j), &pAlmSubType);
				if(pAlmSubType == NULL)
				{
					return GPN_ALM_GEN_ERR;
				}

				k = pAlmSubType->devOrderInScanTp;
				
				if( ( (pAlmScanType->almReptInfo[i] & bitPos) &&\
					  ((pAlmScanPort->isReptBasePort[i] & bitPos) == 0) ) ||\
					( (pAlmScanPort->iReptAlmInfo[i] & bitPos) &&\
					  (pAlmScanPort->isReptBasePort[i] & bitPos) ) )
				{
					/* almPorc result is alarm disappear, alm report last record is alarm product, then report alarm disppear */
					if( (pAlmScanPort->iReptAlmDate[i] & bitPos) &&
						((pAlmScanPort->iDbSvAlmDate[i] & bitPos) == 0) )
					{
						gpnAlmANtDisappReport(&(pAlmLocalPort->viewPort), &(pAlmScanPort->AlmValueBuff[k]));
						
						pAlmScanPort->iReptAlmDate[i] &= (~bitPos);
					}
					/* almPorc result is alarm product, alm report last record is alarm disppear, then report alarm product */
					else if(((pAlmScanPort->iReptAlmDate[i] & bitPos) == 0) &&
							 (pAlmScanPort->iDbSvAlmDate[i] & bitPos))
					{
						gpnAlmANtProductReport(&(pAlmLocalPort->viewPort), &(pAlmScanPort->AlmValueBuff[k]));

						pAlmScanPort->iReptAlmDate[i] |= bitPos;
					}
					
					pAlmScanPort->iSureAlmMark[i] &= (~bitPos);
				}
				
				/*here we see real alarm chang, so do some opt like bell, led... */
				/*bell, led ...'s status based device, not DataA....                      */
				/*how do we make sure alarm's status ? see iInveAlmDate!      */
				if(pAlmScanPort->iInveAlmDate[i] &= bitPos)
				{
					gpnAlmCfgAlmRankSummary(pAlmScanPort->AlmValueBuff[k].portAlmRank, GPN_ALM_ARISE);
				}
				else
				{
					gpnAlmCfgAlmRankSummary(pAlmScanPort->AlmValueBuff[k].portAlmRank, GPN_ALM_CLEAN);
				}
			}
		}
		pAlmScanPort->iSureAlmMark[GPN_ALM_PAGE_NUM] = 0;
	}
	return GPN_ALM_GEN_YES;
}

UINT32 gpnEventScreenProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort)
{
	stAlmScanPortInfo *pAlmScanPort;
	stEvtSTCharacterDef *pEvtSubTpStr;
	stEvtValueRecd evtValueRecd;
	UINT32 bitsInPage;
	UINT64 bitPos;
	UINT32 i;
	UINT32 j;
	
	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	
	for(i=0; i<pAlmScanType->evtUsePag; i++)
	{
		if(pAlmScanPort->iFrehEvtMark[i] != 0)
		{
			pAlmScanPort->iFrehEvtMark[i] =\
				pAlmScanType->evtScreInfo[i] & pAlmScanPort->iFrehEvtMark[i];
			/*printf("pAlmScanType->evtScreInfo[i] %08x page %d\n\r", pAlmScanType->evtScreInfo[i], i);*/
			if((i+1) == pAlmScanType->evtUsePag)
			{
				if(pAlmScanType->evtSubTpNumInScanType == GPN_EVT_PAGE_BIT_SIZE)
				{
					bitsInPage = GPN_EVT_PAGE_BIT_SIZE;
				}
				else
				{
					bitsInPage = pAlmScanType->evtSubTpNumInScanType % GPN_EVT_PAGE_BIT_SIZE;
				}
			}
			else
			{
				bitsInPage = GPN_EVT_PAGE_BIT_SIZE;
			}

			for(j=0; j<bitsInPage; j++)
			{
				bitPos = (((UINT32)(1U))<<j);
				if(pAlmScanPort->iFrehEvtMark[i] & bitPos)
				{
					/*找出告警子类*/
					pEvtSubTpStr = NULL;
					gpnEvtSeekScanTypeToSubType(pAlmScanType, (i*GPN_EVT_PAGE_BIT_SIZE+j), &pEvtSubTpStr);
					if(pEvtSubTpStr != NULL)
					{
						/*event go to DB*/
						evtValueRecd.evtSubType = pEvtSubTpStr->evtSubType;
						evtValueRecd.detial = pEvtSubTpStr->detial;
						evtValueRecd.evtRank = pEvtSubTpStr->evtRank;
						gpnAlmANtEvtInsertDS(&(pAlmLocalPort->viewPort), &evtValueRecd);

						/* 本盘为主盘且备盘存在，将事件放入同步事件表中，2018/8/16 */
						if((DEVM_HA_MASTER == alarm_MS_my_status_get()) &&
							(alarm_MS_slave_board_slot_get()) &&
							(alarm_MS_slave_power_done()))
						{
							alarm_new_event_sync_table_add(&(pAlmLocalPort->viewPort), &evtValueRecd);
						}
						
						GPN_EVT_SCAN_PRINT(GPN_EVT_SCAN_CUP, "%s:evtSubTp(0x%08x)(%20s) rise, localPort(%08x-%08x|%08x|%08x|%08x|%08x) viewPort(%08x-%08x|%08x|%08x|%08x|%08x) tiem %08x\n\r",\
							__FUNCTION__, pEvtSubTpStr->evtSubType, pEvtSubTpStr->evtName,\
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
							(UINT32)pgstAlmPreScanWholwInfo->almTaskTime);
					}
					else
					{
						return GPN_ALM_GEN_ERR;
					}
				}
			}
		}
	}
	
	return GPN_ALM_GEN_YES;
}

UINT32 gpnEventReportProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort)
{
	stAlmScanPortInfo *pAlmScanPort;
	stEvtSTCharacterDef *pEvtSubTpStr;
	stEvtValueRecd evtValueRecd;
	UINT32 bitsInPage;
	UINT64 bitPos;
	UINT32 i;
	UINT32 j;
	
	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	for(i=0; i<pAlmScanType->evtUsePag; i++)
	{		
		if(pAlmScanPort->iFrehEvtMark[i] != 0)
		{
			pAlmScanPort->iFrehEvtMark[i] =\
				pAlmScanType->evtReptInfo[i] & pAlmScanPort->iFrehEvtMark[i];
		
			if((i+1) == pAlmScanType->evtUsePag)
			{
				if(pAlmScanType->evtSubTpNumInScanType == GPN_EVT_PAGE_BIT_SIZE)
				{
					bitsInPage = GPN_EVT_PAGE_BIT_SIZE;
				}
				else
				{
					bitsInPage = pAlmScanType->evtSubTpNumInScanType % GPN_EVT_PAGE_BIT_SIZE;
				}
			}
			else
			{
				bitsInPage = GPN_EVT_PAGE_BIT_SIZE;
			}

			for(j=0; j<bitsInPage; j++)
			{
				bitPos = (((UINT32)(1U))<<j);
				if(pAlmScanPort->iFrehEvtMark[i] & bitPos)
				{
					/*找出告警子类*/
					pEvtSubTpStr = NULL;
					gpnEvtSeekScanTypeToSubType(pAlmScanType, (i*GPN_EVT_PAGE_BIT_SIZE+j), &pEvtSubTpStr);
					if(pEvtSubTpStr != NULL)
					{
						/*event go to DB*/
						evtValueRecd.evtSubType = pEvtSubTpStr->evtSubType;
						evtValueRecd.detial = pEvtSubTpStr->detial;
						evtValueRecd.evtRank = pEvtSubTpStr->evtRank;
						
						gpnAlmANtEvtProductReport(&(pAlmLocalPort->viewPort), &evtValueRecd);
						GPN_EVT_SCAN_PRINT(GPN_EVT_SCAN_CUP, "%s:evtSubTp(0x%08x)(%20s) report, localPort(%08x-%08x|%08x|%08x|%08x|%08x) viewPort(%08x-%08x|%08x|%08x|%08x|%08x) tiem %08x\n\r",\
							__FUNCTION__, pEvtSubTpStr->evtSubType, pEvtSubTpStr->evtName,\
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
							(UINT32)pgstAlmPreScanWholwInfo->almTaskTime);

					}
					else
					{
						return GPN_ALM_GEN_ERR;
					}
				}
			}
		}
		pAlmScanPort->iFrehEvtMark[i] = 0;
	}
	pAlmScanPort->iFrehEvtMark[GPN_EVT_PAGE_NUM] = 0;
	
	return GPN_ALM_GEN_YES;
}

/*==========================================================*/
/*name :  gpnAlmFreshAlmProc                                                                                                */
/*para :                                                                                                                                   */
/*retu :  void                                                                                                                            */
/*desc :                                                                                            */
/*==========================================================*/
UINT32 gpnAlmFreshAlmProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort)
{
	stAlmScanPortInfo *pAlmScanPort;
	
	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	
	/*告警层次:更新复用关系中的上层告警*/
	if(pAlmScanPort->pUppeAlmScanPort != NULL)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "1");
		gpnAlmUpperAlmPassProc(pAlmScanType, pAlmLocalPort);
	}
	/*告警屏蔽(端口级,告警子类级)*/
	if(pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM] != 0)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "2");
		gpnAlmScreenProc(pAlmScanType, pAlmLocalPort);
	}
	/*告警抑制(只有端口级)*/
	if(pAlmScanPort->iRestAlmMark != 0)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "3");
		gpnAlmRestrainProc(pAlmScanType, pAlmLocalPort);
		pAlmScanPort->iRestAlmMark = 0;
	}
	/*告警反转延时进入(只有端口级),必须是最后处理的"延时进入单元"*/
	if(pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM] != 0)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "4");
		gpnAlmInvesalDlyInProc(pAlmScanType, pAlmLocalPort);
		pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM] = 0;
	}
	/*告警延时(只有端口级)*/
	/*printf("befor dly %08x %08x %08x %08x %08x\n\r",pAlmScanPort->iDelyAlmMark[4],\
		pAlmScanPort->iDelyAlmMark[3],pAlmScanPort->iDelyAlmMark[2],\
		pAlmScanPort->iDelyAlmMark[1],pAlmScanPort->iDelyAlmMark[0]);*/
	if(pAlmScanPort->iDelyAlmMark[GPN_ALM_PAGE_NUM] != 0)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "5");
		gpnAlmDlyProc(pAlmScanType, pAlmLocalPort);
	}
	/*printf("after dly %08x %08x %08x %08x %08x\n\r",pAlmScanPort->iDelyAlmMark[4],
		pAlmScanPort->iDelyAlmMark[3],pAlmScanPort->iDelyAlmMark[2],\
		pAlmScanPort->iDelyAlmMark[1],pAlmScanPort->iDelyAlmMark[0]);*/
	/*告警反转处理(只有端口级)*/
	if(pAlmScanPort->iSureAlmMark[GPN_ALM_PAGE_NUM] != 0)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "6");
		gpnAlmInvesalProc(pAlmScanType, pAlmLocalPort);
	}
	/*告警过滤(端口级,告警子类级)*/
	if(pAlmScanPort->iSureAlmMark[GPN_ALM_PAGE_NUM] != 0)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "7");
		gpnAlmFilterProc(pAlmScanType, pAlmLocalPort);
	}
	/*告警上报(端口级,告警子类级)*/
	if(pAlmScanPort->iSureAlmMark[GPN_ALM_PAGE_NUM] != 0)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "8");
		gpnAlmReportProc(pAlmScanType, pAlmLocalPort);
	}

	//GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "9");

	/*event process*/
	if(pAlmScanPort->iFrehEvtMark[GPN_EVT_PAGE_NUM] != 0)
	{
		gpnEventScreenProc(pAlmScanType, pAlmLocalPort);
		gpnEventReportProc(pAlmScanType, pAlmLocalPort);
	}
	return GPN_ALM_GEN_YES;
}
/*==========================================================*/
/*name :  gpnAlmShakeAlmProc                                                                                                */
/*para :                                                                                                                                   */
/*retu :  void                                                                                                                            */
/*desc :                                                                                            */
/*==========================================================*/
UINT32 gpnAlmShakeAlmProc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort)
{
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubTpStr;
	stAlmShakeRecd *pAlmShakeRecd;
	stAlmShakeUnit *pAlmShakeUnit;
	UINT32 bitsInPage;
	UINT32 bitPos;
	UINT32 i;
	UINT32 j;
	UINT32 k;
	
	pAlmScanPort = pAlmLocalPort->pAlmScanPort;

	/* 1 process old period shake mark */
	if(pAlmScanPort->iShakOldMark[GPN_ALM_PAGE_NUM] > 0)
	{
		for(i=0; i<pAlmScanType->almUsePag; i++)
		{
			if(pAlmScanPort->iShakOldMark[i] == 0)
			{
				continue;
			}
			
			if((i+1) == pAlmScanType->almUsePag)
			{
				if( (pAlmScanType->almSubTpNumInRestType % GPN_ALM_PAGE_BIT_SIZE) == 0)
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
				
			for(j=0; j<bitsInPage; j++)
			{
				/* some bit has shake record */
				bitPos = ((UINT32)(1U))<<j;
				if(pAlmScanPort->iShakOldMark[i] & bitPos)
				{
					pAlmSubTpStr = NULL;
					gpnAlmSeekScanTypeToSubType(pAlmScanType, (i*GPN_ALM_PAGE_BIT_SIZE+j), &pAlmSubTpStr);
					if(pAlmSubTpStr == NULL)
					{
						/* err happen */
						continue;
					}

					/* finde shake unit */
					k = pAlmSubTpStr->devOrderInScanTp;
					pAlmShakeRecd = &(pAlmScanPort->AlmShakeBuff[k]);
					pAlmShakeUnit = &(pAlmShakeRecd->shakeUnit[pAlmScanPort->valShakeUnit%2]);
					if(pAlmShakeUnit->shakeCnt > 0)
					{
						/* do have valid shake, add shake count to alarm product count */
						//pAlmScanPort->AlmValueBuff[k].prodCnt += pAlmShakeUnit->shakeCnt;
						
						/* if not filt/record, in histDB */
						
						/* if repot, trap histAlarm */
						if( ( (pAlmScanType->almReptInfo[i] & bitPos) &&\
					  			((pAlmScanPort->isReptBasePort[i] & bitPos) == 0) ) ||\
							( (pAlmScanPort->iReptAlmInfo[i] & bitPos) &&\
					 		 	(pAlmScanPort->isReptBasePort[i] & bitPos) ) )
						{
							
						}
					}
					/* reset shake unit */
					pAlmShakeUnit->stat = GPN_ALM_UNKOWN;
					
				}
			}
			pAlmScanPort->iShakOldMark[i] = 0;
		}
		pAlmScanPort->iShakOldMark[GPN_ALM_PAGE_NUM] = 0;
	}

	/* 2 save now period mark info to old shake record period */
	if(pAlmScanPort->iShakAlmMark[GPN_ALM_PAGE_NUM] > 0)
	{
		for(i=0; i<pAlmScanType->almUsePag; i++)
		{
			pAlmScanPort->iShakOldMark[i] = pAlmScanPort->iShakAlmMark[i];
		}
		pAlmScanPort->iShakOldMark[GPN_ALM_PAGE_NUM] =\
			pAlmScanPort->iShakAlmMark[GPN_ALM_PAGE_NUM];
	}

	/* 3  turn period, now period shake record turn to old period shake record, overflow ? howerver !*/
	pAlmScanPort->valShakeUnit++;
	
	return GPN_ALM_GEN_YES;
}

/*=====================================*/
/*name :  gpnAlmScanProc                                                     */
/*para :                                                                                */
/*retu :  void                                                                         */
/*desc :  cyc proc alarm value                                                  */
/*=====================================*/
UINT32 gpnAlmScanProc(void)
{
	stAlmScanTypeDef *pAlmScanType;
	stAlmLocalNode *pAlmLocalPort;
	UINT32 portNum;
	UINT32 scanTpNum;

	/* cyc proc base scanType, because scanType vs portType */
	scanTpNum = 0;
	pAlmScanType = (stAlmScanTypeDef *)listFirst(&(pgstAlmTypeWholwInfo->almScanTypeQuen));
	while((pAlmScanType != NULL)&&(scanTpNum < pgstAlmTypeWholwInfo->almScanTypeNum))
	{	
		portNum = 0;
		pAlmLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
		while((pAlmLocalPort != NULL)&&(portNum < pAlmScanType->almScanQuenPortNum))
		{
			/* for cmcc, tmp add for restrain */
			gpnAlmEthSfpPassAlm2EthPhy(pAlmLocalPort);
			
			/*上引用队列有分辨新鲜告警的能力*/
			gpnAlmFreshAlmProc(pAlmScanType, pAlmLocalPort);
		
			portNum++;
			pAlmLocalPort = (stAlmLocalNode *)listNext((NODE *)(pAlmLocalPort));
		}
		scanTpNum++;
		pAlmScanType = (stAlmScanTypeDef *)listNext((NODE *)(pAlmScanType));
	}

	return GPN_ALM_GEN_YES;
}
/*=====================================*/
/*name :  gpnAlmShakeProc                                                  */
/*para :                                                                                */
/*retu :  void                                                                         */
/*desc :  cyc proc alarm value                                                  */
/*=====================================*/
UINT32 gpnAlmShakeProc(void)
{
	stAlmScanTypeDef *pAlmScanType;
	stAlmLocalNode *pAlmLocalPort;
	UINT32 portNum;
	UINT32 scanTpNum;

	/* cyc proc base scanType, because scanType vs portType */
	scanTpNum = 0;
	pAlmScanType = (stAlmScanTypeDef *)listFirst(&(pgstAlmTypeWholwInfo->almScanTypeQuen));
	while((pAlmScanType != NULL)&&(scanTpNum < pgstAlmTypeWholwInfo->almScanTypeNum))
	{	
		portNum = 0;
		pAlmLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
		while((pAlmLocalPort != NULL)&&(portNum < pAlmScanType->almScanQuenPortNum))
		{
			if( (pAlmLocalPort->pAlmScanPort->iShakAlmMark[GPN_ALM_PAGE_NUM] > 0) ||\
				(pAlmLocalPort->pAlmScanPort->iShakOldMark[GPN_ALM_PAGE_NUM] > 0) )
			{
				gpnAlmShakeAlmProc(pAlmScanType, pAlmLocalPort);
			}
			
			portNum++;
			pAlmLocalPort = (stAlmLocalNode *)listNext((NODE *)(pAlmLocalPort));
		}
		scanTpNum++;
		pAlmScanType = (stAlmScanTypeDef *)listNext((NODE *)(pAlmScanType));
	}

	return GPN_ALM_GEN_YES;
}

/*==========================================================*/
/*name :  gpnAlmPoolGetBaseAlmScanTypeProc			   	*/
/*para :  				               						*/
/*retu :  void                         						*/
/*desc :  主动扫描告警:告警获得方法一  						*/
/*==========================================================*/
UINT32 gpnAlmPoolGetBaseAlmScanTypeProc(stAlmScanTypeDef *pAlmScanType, stAlmPeerNode *pAlmPeerPort)
{
	/*废弃*/
	stAlmScanPortInfo *pAlmScanPort;
	UINT32 iAlmData[GPN_ALM_PAGE_NUM]={0};
	UINT32 tmpAlmData;
	UINT32 i;

	pAlmScanPort = pAlmPeerPort->pAlmScanPort;
	if(1/*MstpBdDrvIOCtlBoard(pAlmPeerPort->peerPort.devIndex,pAlmPeerPort->peerPort.portIndex,
						pAlmScanType->scanTpOpeCtl,&iAlmData, sizeof(UINT32)) == OK*/)
	{     
		/*扫描类内告警合法性保证*/
		for(i=0; i<GPN_ALM_PAGE_NUM; i++)
		{
			iAlmData[i] &= pAlmScanType->scanTypeAlmMask[i];	
			/*新旧告警异或，得出变化告警*/
			tmpAlmData = iAlmData[i] ^ pAlmScanPort->iScanAlmDate[i];
			if(tmpAlmData != 0)
			{
				pAlmScanPort->iScanAlmDate[i] = iAlmData[i];
				pAlmScanPort->iFrehAlmMark[i] |= tmpAlmData;
				pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM]++;
			}
		}
	}
	else
	{
		/*板不在位，针对此端口的延时啊什么的仍要继续么*/
	}

	return GPN_ALM_GEN_YES;
}

/*==============================================*/
/*name :  gpnMsgNotifyAlmBaseAlmScanTypeProc                                    */
/*para :                                                                                                      */
/*retu :  void                                                                                               */
/*desc :                                                                                                      */
/*==============================================*/
UINT32 gpnMsgNotifyAlmBaseAlmScanTypeProc(optObjOrient *pPortInfo, UINT32 *pAlmData, UINT32 len)
{
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stAlmPreScanQuen *pAlmPreScanNode;
	stAlmPeerNode *pAlmPeerPort;
	stAlmScanPortInfo *pAlmScanPort;
	optObjOrient *pstProtInfo;
	UINT32 peerScanNodeNum;
	UINT32 protTypeNum;
	UINT32 portType;
	UINT32 slotId;
	UINT32 hashKey;
	UINT32 tmpAlmData;
	UINT32 i;

	/* get port type from first-portIndex */
	portType = PortIndex_GetType(pPortInfo->portIndex);
	/* is port type supported */
	protTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(protTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		if(pPortTpToScanTp->portType == portType)
		{
			/* get slot hsah key */
			slotId = PortIndex_GetSlot(pPortInfo->portIndex);
			pAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
			hashKey = (pPortInfo->portIndex)%(pAlmPreScanNode->hashKey);

			/* check if notify port in per scan list, compare with peer port */
			peerScanNodeNum = 0;
			pAlmPeerPort = (stAlmPeerNode *)listFirst(&(pAlmPreScanNode->almPeerPortQuen[hashKey]));
			while((pAlmPeerPort != NULL)&&(peerScanNodeNum < pAlmPreScanNode->peerPortNodeNum[hashKey]))
			{
				pstProtInfo = &(pAlmPeerPort->peerPort);
				if( /*(pstProtInfo->devIndex == pPortInfo->devIndex) &&\*/
					(pstProtInfo->portIndex == pPortInfo->portIndex) &&\
					(pstProtInfo->portIndex3 == pPortInfo->portIndex3) &&\
					(pstProtInfo->portIndex4 == pPortInfo->portIndex4) &&\
					(pstProtInfo->portIndex5 == pPortInfo->portIndex5) &&\
					(pstProtInfo->portIndex6 == pPortInfo->portIndex6) )
				{
					pAlmScanPort = pAlmPeerPort->pAlmScanPort;
					/* is port in valid scan list */
					if(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_ENABLE)
					{
						/* renew alm data, chang sys support bit to alarm support bit */
						gpnAlmSysUseBitPToAlmUseBitP(pAlmPreScanNode->almScanTypeNode,pAlmData,len);
						for(i=0; i<pAlmPreScanNode->almScanTypeNode->almUsePag; i++)
						{
							pAlmData[i] &= pAlmPreScanNode->almScanTypeNode->scanTypeAlmMask[i];
							/* find new chang */
							tmpAlmData = pAlmData[i] ^ pAlmScanPort->iScanAlmDate[i];
							if(tmpAlmData != 0)
							{
								pAlmScanPort->iScanAlmDate[i] = pAlmData[i];
								pAlmScanPort->iFrehAlmMark[i] |= tmpAlmData;
								pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM]++;
							}
		
							/*如果有新鲜告警，则调整有效扫描队列的位置*/
							if(pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM] != 0)
							{
								/*具体实现与否，及如何实现有待研究*/
							}
						}
					}
					else
					{
						/* port in perScanQuen, but not in validScanQuen */
						gpnLog(GPN_LOG_L_ALERT, "%s : not in valid list, port(%08x|%08x|%08x|%08x|%08x|%08x)\n\r",\
							__FUNCTION__,\
							pPortInfo->devIndex,\
							pPortInfo->portIndex,\
							pPortInfo->portIndex3,\
							pPortInfo->portIndex4,\
							pPortInfo->portIndex5,\
							pPortInfo->portIndex6);
						GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : not in valid list, port(%08x|%08x|%08x|%08x|%08x|%08x)\n\r",\
							__FUNCTION__,\
							pPortInfo->devIndex,\
							pPortInfo->portIndex,\
							pPortInfo->portIndex3,\
							pPortInfo->portIndex4,\
							pPortInfo->portIndex5,\
							pPortInfo->portIndex6);
					}
					break;
				}
				peerScanNodeNum++;
				pAlmPeerPort = (stAlmPeerNode *)listNext((NODE *)(pAlmPeerPort));
			}
			if((pAlmPeerPort == NULL)||(peerScanNodeNum >= pAlmPreScanNode->peerPortNodeNum[hashKey]))
			{
				/* port type support, but not found port in slot hash list */
				gpnLog(GPN_LOG_L_ALERT, "%s : not found port(%08x|%08x|%08x|%08x|%08x|%08x) in scanType(%08x)'s slot(%d) pre list\n",\
					__FUNCTION__,\
					pPortInfo->devIndex,\
					pPortInfo->portIndex,\
					pPortInfo->portIndex3,\
					pPortInfo->portIndex4,\
					pPortInfo->portIndex5,\
					pPortInfo->portIndex6,\
					pPortTpToScanTp->ppAlmPreScanNode->pAlmPreScanNode[0].almScanType,\
					slotId);
				GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : not found port(%08x|%08x|%08x|%08x|%08x|%08x) in scanType(%08x)'s slot(%d) pre list\n",\
					__FUNCTION__,\
					pPortInfo->devIndex,\
					pPortInfo->portIndex,\
					pPortInfo->portIndex3,\
					pPortInfo->portIndex4,\
					pPortInfo->portIndex5,\
					pPortInfo->portIndex6,\
					pPortTpToScanTp->ppAlmPreScanNode->pAlmPreScanNode[0].almScanType,\
					slotId);
				return GPN_ALM_GEN_NO;
			}

			break;
		}
		protTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	if((pPortTpToScanTp == NULL)||(protTypeNum >= pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		/* port type not support */
		gpnLog(GPN_LOG_L_ALERT, "%s : portType(%08x) not support for port(%08x|%08x|%08x|%08x|%08x|%08x)\n",\
			__FUNCTION__,\
			portType,\
			pPortInfo->devIndex,\
			pPortInfo->portIndex,\
			pPortInfo->portIndex3,\
			pPortInfo->portIndex4,\
			pPortInfo->portIndex5,\
			pPortInfo->portIndex6);
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : portType(%08x) not support for port(%08x|%08x|%08x|%08x|%08x|%08x)\n",\
			__FUNCTION__,\
			portType,\
			pPortInfo->devIndex,\
			pPortInfo->portIndex,\
			pPortInfo->portIndex3,\
			pPortInfo->portIndex4,\
			pPortInfo->portIndex5,\
			pPortInfo->portIndex6);
		return GPN_ALM_GEN_NO;
	}
	return GPN_ALM_GEN_YES;
}



/*==================================================*/
/*name :  gpnAlmPeerPortReplaceNotify                                                                 */
/*para :                                                                                                               */
/*retu :  void                                                                                                        */
/*desc :  process request replace peer port                                                              */
/*==================================================*/
UINT32 gpnAlmPeerPortReplaceNotify(optObjOrient *pOldPeerP, optObjOrient *pNewPeerP)
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
		slotId = PortIndex_GetSlot(pOldPeerP->portIndex);
		hashKey = (pOldPeerP->portIndex)%(pAlmPreScanNode->hashKey);
		
		/* check if notify port in per scan list, compare with peer port */
		peerScanNodeNum = 0;
		pAlmPeerPort = (stAlmPeerNode *)listFirst(&(pAlmPreScanNode->almPeerPortQuen[hashKey]));
		while((pAlmPeerPort != NULL)&&(peerScanNodeNum < pAlmPreScanNode->peerPortNodeNum[hashKey]))
		{
			pstProtInfo = &(pAlmPeerPort->peerPort);
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
					if(GPN_ALM_GEN_YES != gpnAlmRelevCheckIsPeerInPeerPortQuen(\
						pNewPeerP, pAlmPreScanNodeTmp))
					{
						/* when match, delete from old list and add in new list  */
						listDelete(&(pAlmPreScanNode->almPeerPortQuen[hashKey]), (NODE *)pAlmPeerPort);
						pAlmPreScanNode->peerPortNodeNum[hashKey]--;
						/*gpnLog(GPN_LOG_L_ALERT, "%s : delete port(%08x-%08x|%08x|%08x|%08x|%08x) out almPeerPortQuen(scanType %08x slot %d hash %d).\n",\
							__FUNCTION__,\
							pAlmPeerPort->peerPort.devIndex,\
							pAlmPeerPort->peerPort.portIndex,\
							pAlmPeerPort->peerPort.portIndex3,\
							pAlmPeerPort->peerPort.portIndex4,\
							pAlmPeerPort->peerPort.portIndex5,\
							pAlmPeerPort->peerPort.portIndex6,\
							pAlmPreScanNode->almScanType,\
							slotId, hashKey);*/
					}
					else
					{
						gpnLog(GPN_LOG_L_ALERT, "%s : new port(%08x-%08x|%08x|%08x|%08x|%08x) already in almPeerPortQuen\n",\
							__FUNCTION__,\
							pNewPeerP->devIndex,\
							pNewPeerP->portIndex,\
							pNewPeerP->portIndex3,\
							pNewPeerP->portIndex4,\
							pNewPeerP->portIndex5,\
							pNewPeerP->portIndex6);
						return GPN_ALM_GEN_NO;
					}
				}
				else
				{
					gpnLog(GPN_LOG_L_ALERT, "%s : new port(%08x-%08x|%08x|%08x|%08x|%08x) not found almPeerPortQuen\n",\
						__FUNCTION__,\
						pNewPeerP->devIndex,\
						pNewPeerP->portIndex,\
						pNewPeerP->portIndex3,\
						pNewPeerP->portIndex4,\
						pNewPeerP->portIndex5,\
						pNewPeerP->portIndex6);
					return GPN_ALM_GEN_NO;
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
			gpnLog(GPN_LOG_L_ALERT, "%s : not found port(%08x-%08x|%08x|%08x|%08x|%08x) in scanType(%08x)'s slot(%d) hash %d pre list\n",\
				__FUNCTION__,\
				pOldPeerP->devIndex,\
				pOldPeerP->portIndex,\
				pOldPeerP->portIndex3,\
				pOldPeerP->portIndex4,\
				pOldPeerP->portIndex5,\
				pOldPeerP->portIndex6,\
				pAlmPreScanNode->almScanType,\
				slotId, hashKey);
			GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : not found port(%08x-%08x|%08x|%08x|%08x|%08x) in scanType(%08x)'s slot(%d) hash %d pre list\n",\
				__FUNCTION__,\
				pOldPeerP->devIndex,\
				pOldPeerP->portIndex,\
				pOldPeerP->portIndex3,\
				pOldPeerP->portIndex4,\
				pOldPeerP->portIndex5,\
				pOldPeerP->portIndex6,\
				pAlmPreScanNode->almScanType,\
				slotId, hashKey);
			return GPN_ALM_GEN_NO;
		}

		/* #### step 2/2 : base new peer port, add peer port in quen */
		/* renew peer port */
		memcpy(&(pAlmPeerPort->peerPort), pNewPeerP, sizeof(optObjOrient));
		
		//slotIdTmp = PortIndex_GetSlot(pNewPeerP->portIndex);
		hashKeyTmp = (pNewPeerP->portIndex)%(pAlmPreScanNodeTmp->hashKey);
		
		listAdd(&(pAlmPreScanNodeTmp->almPeerPortQuen[hashKeyTmp]), (NODE *)pAlmPeerPort);
		pAlmPreScanNodeTmp->peerPortNodeNum[hashKeyTmp]++;
		
		/*gpnLog(GPN_LOG_L_ALERT, "%s : add new port(%08x-%08x|%08x|%08x|%08x|%08x) in almPeerPortQuen(scanType %08x slot %d hash %d).\n",\
				__FUNCTION__,\
				pAlmPeerPort->peerPort.devIndex,\
				pAlmPeerPort->peerPort.portIndex,\
				pAlmPeerPort->peerPort.portIndex3,\
				pAlmPeerPort->peerPort.portIndex4,\
				pAlmPeerPort->peerPort.portIndex5,\
				pAlmPeerPort->peerPort.portIndex6,\
				pAlmPreScanNodeTmp->almScanType,\
				slotIdTmp, hashKeyTmp);*/

		return GPN_ALM_GEN_YES;
	}
	else
	{
		gpnLog(GPN_LOG_L_ALERT, "%s : old port(%08x-%08x|%08x|%08x|%08x|%08x) not found pAlmPreScanNode\n",\
			__FUNCTION__,\
			pOldPeerP->devIndex,\
			pOldPeerP->portIndex,\
			pOldPeerP->portIndex3,\
			pOldPeerP->portIndex4,\
			pOldPeerP->portIndex5,\
			pOldPeerP->portIndex6);
		return GPN_ALM_GEN_NO;
	}
}

/*==================================================*/
/*name :  gpnAlmViewPortReplaceNotify                                                                 */
/*para :                                                                                                               */
/*retu :  void                                                                                                        */
/*desc :  process request replace peer port                                                              */
/*==================================================*/


/*==================================================*/
/*name :  gpnMasterToSlaveAlmInfoSynchProc                                                  */
/*para :                                                                                                               */
/*retu :  void                                                                                                        */
/*desc :  use peer port index to synch alarm                                                            */
/*==================================================*/
UINT32 gpnMasterToSlaveAlmInfoSynchProc(optObjOrient *pPortInfo, UINT32 almSubType, UINT32 almSta)
{
	gpnMsgNotifyAlmBaseSubTypeProc(pPortInfo, almSubType, almSta);
	return GPN_ALM_GEN_YES;
}
UINT32 gpnMsgNotifyAlmBaseSubTypeProc(optObjOrient* pPortInfo, UINT32 almSubType, UINT32 almSta)
{
	return GPN_ALM_GEN_YES;
}
/*==================================================*/
/*name :  gpnAlmDevStaNotifyPorc                                                                   */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
/*==================================================*/
/*name :  gpnAlmMomAlmChgPorc                                                                   */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmMomAlmChgPorc(stAlmScanTypeDef *pAlmScanType, stAlmLocalNode *pAlmLocalPort, UINT32 opt)
{
	stAlmScanPortInfo *pAlmScanPort;
	stAlmSTCharacterDef *pAlmSubTpStr;
	UINT32 i;

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	if(opt == GPN_ALM_GEN_DISABLE)
	{
		/* clear current alarm record and report */
		for(i=0; i<GPN_ALM_PAGE_NUM; i++)
		{
			pAlmScanPort->iSureAlmMark[i] = pAlmScanPort->iDbSvAlmDate[i];
			pAlmScanPort->iSureAlmMark[GPN_ALM_PAGE_NUM]++;

			pAlmScanPort->iInveAlmDate[i] = 0;/*告警消失*/
		}
		gpnAlmFilterProc(pAlmScanType, pAlmLocalPort);
		gpnAlmReportProc(pAlmScanType, pAlmLocalPort);
		
	}
	else if(opt == GPN_ALM_GEN_ENABLE)
	{
		/* init port alarm value */
		i = 0;
		pAlmSubTpStr = (stAlmSTCharacterDef *)listFirst(&(pAlmScanType->almScanSubTCharaQuen));
		while((pAlmSubTpStr != NULL)&&(i < pAlmScanType->almSubTpNumInScanType))
		{
			pAlmScanPort->AlmValueBuff[i].almSubType = pAlmSubTpStr->almSubType;
			
			pAlmScanPort->AlmValueBuff[i].portAlmRank = pAlmSubTpStr->almRank;
			pAlmScanPort->AlmValueBuff[i].iAlmValue = GPN_ALM_CLEAN;
			pAlmScanPort->AlmValueBuff[i].iDelayCount = 0;

			pAlmScanPort->AlmValueBuff[i].prodCnt = 0;
			pAlmScanPort->AlmValueBuff[i].firstTime = 0;

			i++;
			pAlmSubTpStr = (stAlmSTCharacterDef *)listNext((NODE *)(pAlmSubTpStr));
			
	   	}
	}
	else
	{
		return GPN_ALM_GEN_NO;
	}
	
	gpnAlmScanPortInfoClean(pAlmScanPort);
	
	return GPN_ALM_GEN_YES;
}




/*==================================================*/
/*name :  gpnAlmSysUseBitPToAlmUseBitP                                                        */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmSysUseBitPToAlmUseBitP(stAlmScanTypeDef *pAlmScanType,UINT32 *pAlmDate,UINT32 len)
{
	UINT32 almSubTpNumInSc;
	UINT32 newAlmDate[GPN_ALM_PAGE_NUM]={0};
	UINT32 page;
	stAlmSTCharacterDef *pAlmSubType;
	
	almSubTpNumInSc = 0;
	pAlmSubType = (stAlmSTCharacterDef *)listFirst(&(pAlmScanType->almScanSubTCharaQuen));
	while((pAlmSubType != NULL)&&(almSubTpNumInSc < pAlmScanType->almSubTpNumInScanType))
	{	
		page = pAlmSubType->sysUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
		if(pAlmSubType->sysUseIdentify & pAlmDate[page])
		{
			page = pAlmSubType->almUseIdentify >> GPN_ALM_PAGE_BIT_SIZE;
			newAlmDate[page] |= pAlmSubType->almUseIdentify;
		}

		almSubTpNumInSc++;
		pAlmSubType = (stAlmSTCharacterDef *)listNext((NODE *)(pAlmSubType));
	}
	for(page=0; page<len; page++)
	{
		pAlmDate[page] = newAlmDate[page];
	}

	return GPN_ALM_GEN_YES;
}
/*==================================================*/
/*name :  gpnAlmSeekAlmUseBitPToSubTypeNumInScanType                             */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmSeekAlmUseBitPToSubTypeNumInScanType(stAlmScanTypeDef *pAlmScanType,UINT32 almUseBitp,UINT32 *pSubTpIndexInST)
{
	stAlmSTCharacterDef *pAlmSubType;
	UINT32 almSubTpNumInSc;
	
	almSubTpNumInSc = 0;
	pAlmSubType = (stAlmSTCharacterDef *)listFirst(&(pAlmScanType->almScanSubTCharaQuen));
	while((pAlmSubType != NULL)&&(almSubTpNumInSc < pAlmScanType->almSubTpNumInScanType))
	{
		if(pAlmSubType->almUseBitPosition == almUseBitp)
		{
			/*顺序从0开始*/
			*pSubTpIndexInST = almSubTpNumInSc;
			break;
		}

		almSubTpNumInSc++;
		pAlmSubType = (stAlmSTCharacterDef *)listNext((NODE *)(pAlmSubType));
	}
	if((pAlmSubType == NULL)&&(almSubTpNumInSc >= pAlmScanType->almSubTpNumInScanType))
	{
		*pSubTpIndexInST = GPN_ALM_SCAN_32_FFFF;
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : scanType(%08x), almUseBit(%d), err!\n\r",\
			__FUNCTION__, pAlmScanType->almScanType, almUseBitp);
		return GPN_ALM_GEN_NO;
	}
	return GPN_ALM_GEN_YES;
}
/*==================================================*/
/*name :  gpnAlmSeekAlmSubTpToOrderInScanTp                                                 */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmSeekAlmSubTpToOrderInScanTp(UINT32 almSubType,UINT32 *porderInScanTp)
{
	stAlmSTCharacterDef *pAlmSubTpStr;
	stAlmScanTypeDef *pAlmScanType;
	UINT32 almSubTpNumInSc;
	UINT32 scanTpNum;

	/*状态变化的扫描项处理*/
	scanTpNum = 0;
	pAlmScanType = (stAlmScanTypeDef *)listFirst(&(pgstAlmTypeWholwInfo->almScanTypeQuen));
	while((pAlmScanType != NULL)&&(scanTpNum < pgstAlmTypeWholwInfo->almScanTypeNum))
	{	
		almSubTpNumInSc = 0;
		pAlmSubTpStr = (stAlmSTCharacterDef *)listFirst(&(pAlmScanType->almScanSubTCharaQuen));
		while((pAlmSubTpStr != NULL)&&(almSubTpNumInSc < pAlmScanType->almSubTpNumInScanType))
		{
			if(pAlmSubTpStr->almSubType == almSubType)
			{
				*porderInScanTp = pAlmSubTpStr->devOrderInScanTp;
				return GPN_ALM_GEN_YES;
			}

			almSubTpNumInSc++;
			pAlmSubTpStr = (stAlmSTCharacterDef *)listNext((NODE *)(pAlmSubTpStr));
		}
		
		scanTpNum++;
		pAlmScanType = (stAlmScanTypeDef *)listNext((NODE *)(pAlmScanType));
	}
	
	*porderInScanTp = GPN_ALM_SCAN_32_NULL;
	return GPN_ALM_GEN_NO;
}

/*==================================================*/
/*name :  gpnAlmSeekScanTypeToSubType                                                        */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmSeekScanTypeToSubType(stAlmScanTypeDef *pAlmScanType,UINT32 almUseBitp,stAlmSTCharacterDef **ppAlmSubTpStr)
{
	UINT32 almSubTpNumInSc;
	stAlmSTCharacterDef *pAlmSubType;
	
	almSubTpNumInSc = 0;
	pAlmSubType = (stAlmSTCharacterDef *)listFirst(&(pAlmScanType->almScanSubTCharaQuen));
	while((pAlmSubType != NULL)&&(almSubTpNumInSc < pAlmScanType->almSubTpNumInScanType))
	{
		if(pAlmSubType->almUseBitPosition == almUseBitp)
		{
			*ppAlmSubTpStr = pAlmSubType;
			break;
		}

		almSubTpNumInSc++;
		pAlmSubType = (stAlmSTCharacterDef *)listNext((NODE *)(pAlmSubType));
	}
	if((pAlmSubType == NULL)&&(almSubTpNumInSc >= pAlmScanType->almSubTpNumInScanType))
	{
		*ppAlmSubTpStr = NULL;
		return GPN_ALM_GEN_NO;
	}
	return GPN_ALM_GEN_YES;
}
/*==================================================*/
/*name :  gpnEvtSeekScanTypeToSubType                                                             */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnEvtSeekScanTypeToSubType(stAlmScanTypeDef *pAlmScanType, UINT32 evtSysIndentify, stEvtSTCharacterDef **ppEvtSubTpStr)
{
	UINT32 evtSubTpNumInSc;
	stEvtSTCharacterDef *pEvtSubType;
	
	evtSubTpNumInSc = 0;
	pEvtSubType = (stEvtSTCharacterDef *)listFirst(&(pAlmScanType->evtScanSubTCharaQuen));
	while((pEvtSubType != NULL)&&(evtSubTpNumInSc < pAlmScanType->evtSubTpNumInScanType))
	{
		if(pEvtSubType->evtUseBitPosition == evtSysIndentify)
		{
			*ppEvtSubTpStr = pEvtSubType;
			break;
		}

		evtSubTpNumInSc++;
		pEvtSubType = (stEvtSTCharacterDef *)listNext((NODE *)(pEvtSubType));
	}
	if((pEvtSubType == NULL)&&(evtSubTpNumInSc >= pAlmScanType->evtSubTpNumInScanType))
	{
		*ppEvtSubTpStr = NULL;
		return GPN_EVT_GEN_ERR;
	}
	return GPN_EVT_GEN_OK;
}

/*==================================================*/
/*name :  gpnAlmSeekPortTypeToAlmScanType                                                 */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmSeekPortTypeToAlmScanType(UINT32 portType, stAlmScanTypeDef **ppAlmScanType)
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
/*==================================================*/
/*name :  gpnAlmSeekLocalPortIndexToAlmLocalPortNode                                  */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc : use local port index find almScanPort node                                                  */
/*==================================================*/
UINT32 gpnAlmSeekLocalPortIndexToAlmLocalPortNode(objLogicDesc *pPortInfo, stAlmLocalNode **ppAlmLocalPort)
{
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stAlmPreScanQuen *pAlmPreScanNode;
	stAlmPreScanIndex *pAlmPreScanIndex;
	objLogicDesc *pstProtInfo;
	UINT32 preScanNodeNum;
	UINT32 sysPortTypeNum;
	UINT32 portIndex;
	UINT32 portType;
	UINT32 slotId;
	UINT32 hashKey;

	/* first portIndex include objLogicDesc's port type */
	portIndex = pPortInfo->portIndex;
	portType = PortIndex_GetType(portIndex);
	
	/* check if port type be support */
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		if(pPortTpToScanTp->portType == portType)
		{
			/* use slot do hash opt, in order to increase check speed */
			slotId = PortIndex_GetSlot(portIndex);

			pAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
			hashKey = pPortInfo->portIndex%(pAlmPreScanNode->hashKey);
			
			/* compare with local port index */
			preScanNodeNum = 0;
			pAlmPreScanIndex = (stAlmPreScanIndex *)listFirst(&(pAlmPreScanNode->almPreScanQuen[hashKey]));
			while((pAlmPreScanIndex != NULL)&&(preScanNodeNum < pAlmPreScanNode->preScanNodeNum[hashKey]))
			{
				pstProtInfo = &(pAlmPreScanIndex->pAlmLocalPort->localPort);
				if( (pstProtInfo->devIndex == pPortInfo->devIndex) &&\
					(pstProtInfo->portIndex == pPortInfo->portIndex) &&\
					(pstProtInfo->portIndex3 == pPortInfo->portIndex3) &&\
					(pstProtInfo->portIndex4 == pPortInfo->portIndex4) &&\
					(pstProtInfo->portIndex5 == pPortInfo->portIndex5) &&\
					(pstProtInfo->portIndex6 == pPortInfo->portIndex6) )
				{
					*ppAlmLocalPort = pAlmPreScanIndex->pAlmLocalPort;
					
					break;
				}
				preScanNodeNum++;
				pAlmPreScanIndex = (stAlmPreScanIndex *)listNext((NODE *)(pAlmPreScanIndex));
			}
			
			if((pAlmPreScanIndex == NULL)||(preScanNodeNum >= pAlmPreScanNode->preScanNodeNum[hashKey]))
			{
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
			}

			break;
		}
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	if((pPortTpToScanTp == NULL)||(sysPortTypeNum >= pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		/* port type not support */
		*ppAlmLocalPort = NULL;
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP,\
			"%s : portType(%08x) not support for port(%08x|%08x|%08x|%08x|%08x|%08x)\n",\
			__FUNCTION__,\
			portType,\
			pPortInfo->devIndex, pPortInfo->portIndex,\
			pPortInfo->portIndex3, pPortInfo->portIndex4,\
			pPortInfo->portIndex5, pPortInfo->portIndex6);
		return GPN_ALM_GEN_NO;
	}
	
	return GPN_ALM_GEN_YES;
}
/*==================================================*/
/*name :  gpnAlmSeekLocalPortIndexToAlmScanPortIndex                                  */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc : use local port index find port's per scan index                                               */
/*==================================================*/
UINT32 gpnAlmSeekLocalPortIndexToAlmScanPortIndex(objLogicDesc *pPortInfo, stAlmPreScanIndex **ppAlmPreScanIndex)
{
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stAlmPreScanQuen *pAlmPreScanNode;
	stAlmPreScanIndex *pAlmPreScanIndex;
	objLogicDesc *pstProtInfo;
	UINT32 preScanNodeNum;
	UINT32 sysPortTypeNum;
	//UINT32 devIndex;
	UINT32 portIndex;
	UINT32 portType;
	UINT32 slotId;
	UINT32 hashKey;

	//devIndex = pPortInfo->devIndex;
	portIndex = pPortInfo->portIndex;
	/* first portIndex include objLogicDesc's port type */
	portType = PortIndex_GetType(portIndex);
	
	/* check if port type be support */
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		if(pPortTpToScanTp->portType == portType)
		{
			/* use slot do hash opt, in order to increase check speed */
			/*slotId = DeviceIndex_GetSlot(devIndex);*/
			slotId = PortIndex_GetSlot(portIndex);

			pAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
			hashKey = pPortInfo->portIndex%(pAlmPreScanNode->hashKey);
			
			/* compare with local port index */
			preScanNodeNum = 0;
			pAlmPreScanIndex = (stAlmPreScanIndex *)listFirst(&(pAlmPreScanNode->almPreScanQuen[hashKey]));
			while((pAlmPreScanIndex != NULL)&&(preScanNodeNum < pAlmPreScanNode->preScanNodeNum[hashKey]))
			{
				pstProtInfo = &(pAlmPreScanIndex->pAlmLocalPort->localPort);
				if( /*(pstProtInfo->devIndex == pPortInfo->devIndex) &&\*/
					(pstProtInfo->portIndex == pPortInfo->portIndex) &&\
					(pstProtInfo->portIndex3 == pPortInfo->portIndex3) &&\
					(pstProtInfo->portIndex4 == pPortInfo->portIndex4) &&\
					(pstProtInfo->portIndex5 == pPortInfo->portIndex5) &&\
					(pstProtInfo->portIndex6 == pPortInfo->portIndex6) )
				{
					*ppAlmPreScanIndex = pAlmPreScanIndex;
					
					break;
				}
				preScanNodeNum++;
				pAlmPreScanIndex = (stAlmPreScanIndex *)listNext((NODE *)(pAlmPreScanIndex));
			}
			if((pAlmPreScanIndex == NULL)||(preScanNodeNum >= pAlmPreScanNode->preScanNodeNum[hashKey]))
			{
				/* port type support, but not found port in slot hash list */
				*ppAlmPreScanIndex = NULL;
				GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : not found port(%08x|%08x|%08x|%08x|%08x|%08x) in scanType(%08x)'s slot(%d) pre list\n",\
					__FUNCTION__,\
					pPortInfo->devIndex,\
					pPortInfo->portIndex,\
					pPortInfo->portIndex3,\
					pPortInfo->portIndex4,\
					pPortInfo->portIndex5,\
					pPortInfo->portIndex6,\
					pPortTpToScanTp->ppAlmPreScanNode->pAlmPreScanNode[0].almScanType,\
					slotId);
				return GPN_ALM_GEN_NO;
			}

			break;
		}
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	if((pPortTpToScanTp == NULL)||(sysPortTypeNum >= pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		/* port type not support */
		*ppAlmPreScanIndex = NULL;
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : portType(%08x) not support for port(%08x|%08x|%08x|%08x|%08x|%08x)\n",\
			__FUNCTION__,\
			portType,\
			pPortInfo->devIndex,\
			pPortInfo->portIndex,\
			pPortInfo->portIndex3,\
			pPortInfo->portIndex4,\
			pPortInfo->portIndex5,\
			pPortInfo->portIndex6);
		return GPN_ALM_GEN_NO;
	}
	return GPN_ALM_GEN_YES;
}
/*==================================================*/
/*name :  gpnAlmSeekPeerPortIndexToAlmScanPort                                           */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc : use peer port index find almScanPort node                                                  */
/*==================================================*/
UINT32 gpnAlmSeekPeerPortIndexToAlmPeerNode(optObjOrient *pPortInfo, stAlmPeerNode **ppAlmPeerPort)
{
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stAlmPreScanQuen *pAlmPreScanNode;
	stAlmPeerNode *pAlmPeerPort;
	optObjOrient *pstProtInfo;
	UINT32 preScanNodeNum;
	UINT32 sysPortTypeNum;
	//UINT32 devIndex;
	UINT32 portIndex;
	UINT32 portType;
	UINT32 slotId;
	UINT32 hashKey;

	//devIndex = pPortInfo->devIndex;
	portIndex = pPortInfo->portIndex;
	/* first portIndex include optObjOrient's port type */
	portType = PortIndex_GetType(portIndex);
	
	/* check if port type be support */
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		if(pPortTpToScanTp->portType == portType)
		{
			/* use slot do hash opt, in order to increase check speed */
			slotId = PortIndex_GetSlot(portIndex);

			pAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
			hashKey = pPortInfo->portIndex%(pAlmPreScanNode->hashKey);
			
			/* compare with peer port index */
			preScanNodeNum = 0;
			pAlmPeerPort = (stAlmPeerNode *)listFirst(&(pAlmPreScanNode->almPeerPortQuen[hashKey]));
			while((pAlmPeerPort != NULL)&&(preScanNodeNum < pAlmPreScanNode->peerPortNodeNum[hashKey]))
			{
				pstProtInfo = &(pAlmPeerPort->peerPort);
				if( /*(pstProtInfo->devIndex == pPortInfo->devIndex) &&\*/
					(pstProtInfo->portIndex == pPortInfo->portIndex) &&\
					(pstProtInfo->portIndex3 == pPortInfo->portIndex3) &&\
					(pstProtInfo->portIndex4 == pPortInfo->portIndex4) &&\
					(pstProtInfo->portIndex5 == pPortInfo->portIndex5) &&\
					(pstProtInfo->portIndex6 == pPortInfo->portIndex6) )
				{
					*ppAlmPeerPort = pAlmPeerPort;
					
					break;
				}
				preScanNodeNum++;
				pAlmPeerPort = (stAlmPeerNode *)listNext((NODE *)(pAlmPeerPort));
			}
			if((pAlmPeerPort == NULL)||(preScanNodeNum >= pAlmPreScanNode->peerPortNodeNum[hashKey]))
			{
				/* port type support, but not found port in slot hash list */
				*ppAlmPeerPort = NULL;
				GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : not found port(%08x|%08x|%08x|%08x|%08x|%08x) in scanType(%08x)'s slot(%d) pre list\n",\
					__FUNCTION__,\
					pPortInfo->devIndex,\
					pPortInfo->portIndex,\
					pPortInfo->portIndex3,\
					pPortInfo->portIndex4,\
					pPortInfo->portIndex5,\
					pPortInfo->portIndex6,\
					pPortTpToScanTp->ppAlmPreScanNode->pAlmPreScanNode[0].almScanType,\
					slotId);
				return GPN_ALM_GEN_NO;
			}

			break;
		}
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	if((pPortTpToScanTp == NULL)||(sysPortTypeNum >= pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		/* port type not support */
		*ppAlmPeerPort = NULL;
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : portType(%08x) not support for port(%08x|%08x|%08x|%08x|%08x|%08x)\n",\
			__FUNCTION__,\
			portType,\
			pPortInfo->devIndex,\
			pPortInfo->portIndex,\
			pPortInfo->portIndex3,\
			pPortInfo->portIndex4,\
			pPortInfo->portIndex5,\
			pPortInfo->portIndex6);
		return GPN_ALM_GEN_NO;
	}
	return GPN_ALM_GEN_YES;
}

/*==================================================*/
/*name :  gpnAlmSeekPortIndexToAlmPreScanNode                                          */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc : use peer port index find port type's per scan node                                         */
/*==================================================*/
UINT32 gpnAlmSeekPortIndexToAlmPreScanNode(objLogicDesc *pPortInfo, stAlmPreScanQuen **ppAlmPreScanNode)
{
	stPortTpToAlmScanTp *pPortTpToScanTp;
	UINT32 sysPortTypeNum;
	UINT32 portType;
	UINT32 slotId;
	//UINT32 devIndex;
	UINT32 portIndex;

	//devIndex = pPortInfo->devIndex;
	portIndex = pPortInfo->portIndex;
	/* first portIndex include objLogicDesc's port type */
	portType = PortIndex_GetType(portIndex);
	
	/* check if port type be support */
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		if(pPortTpToScanTp->portType == portType)
		{
			/* use slot do hash opt, in order to increase check speed */
			/*slotId = DeviceIndex_GetSlot(devIndex);*/
			slotId = PortIndex_GetSlot(portIndex);
			*ppAlmPreScanNode = pPortTpToScanTp->ppAlmPreScanNode[slotId].pAlmPreScanNode;
			break;
		}
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	
	if((pPortTpToScanTp == NULL)||(sysPortTypeNum >= pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum))
	{
		/* port type not support */
		*ppAlmPreScanNode = NULL;
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : portType(%08x) not support for port(%08x|%08x|%08x|%08x|%08x|%08x)\n",\
			__FUNCTION__,\
			portType,\
			pPortInfo->devIndex,\
			pPortInfo->portIndex,\
			pPortInfo->portIndex3,\
			pPortInfo->portIndex4,\
			pPortInfo->portIndex5,\
			pPortInfo->portIndex6);
		return GPN_ALM_GEN_NO;
	}
	
	return GPN_ALM_GEN_YES;
}

/*==================================================*/
/*name :  gpnAlmSeekAlmSubTpToAlmSubTpNode                                           */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmSeekAlmSubTpToAlmSubTpNode(UINT32 almSubType, stAlmSTCharacterDef **ppAlmSubTpStr)
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
			break;
		}
		
		almSubTpNum++;
		pAlmSubTpNode = (stAlmSubTpNode *)listNext((NODE *)(pAlmSubTpNode));
	}
	if((pAlmSubTpNode == NULL)||(almSubTpNum >= pgstAlmTypeWholwInfo->almSubTypeNum))
	{
		/*驱动误报，无此端口类型*/
		*ppAlmSubTpStr = NULL;
		return GPN_ALM_GEN_NO;
	}

	return GPN_ALM_GEN_YES;
}
/*==================================================*/
/*name :  gpnAlmSeekAlmSubTpToAlmScanTp                                                  */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmSeekAlmSubTpToAlmScanTp(UINT32 almSubType, stAlmScanTypeDef **ppAlmScanType)
{
	stAlmSTCharacterDef *pAlmSubTpStr;
	stAlmScanTypeDef *pAlmScanType;
	UINT32 subTpInScanTpNum;
	UINT32 scanTpNum;

	/*状态变化的扫描项处理*/
	scanTpNum = 0;
	pAlmScanType = (stAlmScanTypeDef *)listFirst(&(pgstAlmTypeWholwInfo->almScanTypeQuen));
	while((pAlmScanType != NULL)&&(scanTpNum < pgstAlmTypeWholwInfo->almScanTypeNum))
	{	
		subTpInScanTpNum = 0;
		pAlmSubTpStr = (stAlmSTCharacterDef *)listFirst(&(pAlmScanType->almScanSubTCharaQuen));
		while((pAlmSubTpStr != NULL)&&(subTpInScanTpNum < pAlmScanType->almSubTpNumInScanType))
		{
			if(pAlmSubTpStr->almSubType == almSubType)
			{
				*ppAlmScanType = pAlmScanType;
				return GPN_ALM_GEN_YES;
			}
			
			subTpInScanTpNum++;
			pAlmSubTpStr = (stAlmSTCharacterDef *)listNext((NODE *)(pAlmSubTpStr));
		}
		
		scanTpNum++;
		pAlmScanType = (stAlmScanTypeDef *)listNext((NODE *)(pAlmScanType));
	}
	if((pAlmScanType == NULL)||(scanTpNum >= pgstAlmTypeWholwInfo->almScanTypeNum))
	{
		/*驱动误报，无此端口类型*/
		*ppAlmScanType = NULL;
		return GPN_ALM_GEN_NO;
	}
	return GPN_ALM_GEN_YES;
}
/*==================================================*/
/*name :  gpnAlmSeekAlmSubTpToAlmSubTpBitValue                                        */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmSeekAlmSubTpToAlmSubTpBitValue(UINT32 almSubType, stAlmScanTypeDef *pAlmScanType, stAlmSTCharacterDef **ppAlmSubTpStr)
{
	stAlmSTCharacterDef *pAlmSubTpStr;
	UINT32 almSubTpNum;

	/*状态变化的扫描项处理*/
	almSubTpNum = 0;
	pAlmSubTpStr = (stAlmSTCharacterDef *)listFirst(&(pAlmScanType->almScanSubTCharaQuen));
	
	while((pAlmScanType != NULL)&&(almSubTpNum < pAlmScanType->almSubTpNumInScanType))
	{
		if(pAlmSubTpStr->almSubType == almSubType)
		{
			*ppAlmSubTpStr = pAlmSubTpStr;
			break;
		}
		almSubTpNum++;
		pAlmSubTpStr = (stAlmSTCharacterDef *)listNext((NODE *)(pAlmSubTpStr));
	}
	
	if((pAlmSubTpStr == NULL)||(almSubTpNum >= pAlmScanType->almSubTpNumInScanType))
	{
		/*驱动误报，无此子告警口类型*/
		*ppAlmSubTpStr = NULL;

		return GPN_ALM_GEN_NO;
	}
	
	return GPN_ALM_GEN_YES;
}
/*==================================================*/
/*name :  gpnAlmSeekEvtSubTpToEvtSubTpBitValue                                          */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmSeekEvtSubTpToEvtSubTpNode(UINT32 evtSubType, stEvtSTCharacterDef **ppEvtSubTpStr)
{
	stEvtSubTpNode *pEvtSubTpNode;
	UINT32 evtSubTpNum;

	evtSubTpNum = 0;
	pEvtSubTpNode = (stEvtSubTpNode *)listFirst(&(pgstAlmTypeWholwInfo->evtSubTypeQuen));
	while((pEvtSubTpNode != NULL) && (evtSubTpNum < pgstAlmTypeWholwInfo->evtSubTypeNum))
	{
		if(pEvtSubTpNode->pEvtSubTpStr->evtSubType == evtSubType)
		{
			*ppEvtSubTpStr = pEvtSubTpNode->pEvtSubTpStr;
			break;
		}
		
		evtSubTpNum++;
		pEvtSubTpNode = (stEvtSubTpNode *)listNext((NODE *)(pEvtSubTpNode));
	}
	if((pEvtSubTpNode == NULL)||(evtSubTpNum >= pgstAlmTypeWholwInfo->evtSubTypeNum))
	{
		*ppEvtSubTpStr = NULL;
		return GPN_ALM_GEN_NO;
	}

	return GPN_ALM_GEN_YES;
}

UINT32 gpnAlmSeekEvtSubTpToEvtSubTpBitValue(UINT32 evtSubType, stAlmScanTypeDef *pAlmScanType, stEvtSTCharacterDef **ppEvtSubTpStr)
{
	stEvtSTCharacterDef *pEvtSubTpStr;
	UINT32 evtSubTpNum;

	/*状态变化的扫描项处理*/
	evtSubTpNum = 0;
	pEvtSubTpStr = (stEvtSTCharacterDef *)listFirst(&(pAlmScanType->evtScanSubTCharaQuen));
	while((pEvtSubTpStr != NULL)&&(evtSubTpNum < pAlmScanType->evtSubTpNumInScanType))
	{
		if(pEvtSubTpStr->evtSubType == evtSubType)
		{
			*ppEvtSubTpStr = pEvtSubTpStr;
			break;
		}
		
		evtSubTpNum++;
		pEvtSubTpStr = (stEvtSTCharacterDef *)listNext((NODE *)(pEvtSubTpStr));
	}
	
	if((pEvtSubTpStr == NULL)||(evtSubTpNum >= pAlmScanType->evtSubTpNumInScanType))
	{
		/*驱动误报，无此子告警口类型*/
		*ppEvtSubTpStr = NULL;

		return GPN_EVT_GEN_ERR;
	}
	
	return GPN_EVT_GEN_OK;
}

UINT32 gpnAlmRelevCheckIsLocalInPerScanQuen(objLogicDesc *pPortInfo, stAlmPreScanQuen *pAlmPreScanNode)
{
	UINT32 hashValue;
	UINT32 perNodeNum;
	objLogicDesc *pLocalIndex;
	stAlmPreScanIndex *pAlmPreScanIndex;
	
	hashValue = ((pPortInfo->portIndex) % (pAlmPreScanNode->hashKey));
	
	perNodeNum = 0;
	pAlmPreScanIndex = (stAlmPreScanIndex *)listFirst(&(pAlmPreScanNode->almPreScanQuen[hashValue]));
	while((pAlmPreScanIndex != NULL)&&(perNodeNum < pAlmPreScanNode->preScanNodeNum[hashValue]))
	{
		pLocalIndex = &(pAlmPreScanIndex->pAlmLocalPort->localPort);
		if( /*(pLocalIndex->devIndex == pPortInfo->devIndex) &&\*/
			(pLocalIndex->portIndex == pPortInfo->portIndex) &&\
			(pLocalIndex->portIndex3 == pPortInfo->portIndex3) &&\
			(pLocalIndex->portIndex4 == pPortInfo->portIndex4) &&\
			(pLocalIndex->portIndex5 == pPortInfo->portIndex5) &&\
			(pLocalIndex->portIndex6 == pPortInfo->portIndex6) )
		{
			return GPN_ALM_GEN_YES;
		}
		perNodeNum++;
		pAlmPreScanIndex = (stAlmPreScanIndex *)listNext((NODE *)(pAlmPreScanIndex));
	}
	return GPN_ALM_GEN_NO;
}

UINT32 gpnAlmRelevCheckIsPeerInPeerPortQuen(optObjOrient *pPortInfo, stAlmPreScanQuen *pAlmPreScanNode)
{
	UINT32 hashValue;
	UINT32 peerNodeNum;
	stAlmPeerNode *pAlmPeerPort;
	optObjOrient *pPeerIndex;
	
	hashValue = ((pPortInfo->portIndex) % (pAlmPreScanNode->hashKey));
	
	peerNodeNum = 0;
	pAlmPeerPort = (stAlmPeerNode *)listFirst(&(pAlmPreScanNode->almPeerPortQuen[hashValue]));
	while((pAlmPeerPort != NULL)&&(peerNodeNum < pAlmPreScanNode->peerPortNodeNum[hashValue]))
	{
		pPeerIndex = &(pAlmPeerPort->peerPort);
		if( /*(pPeerIndex->devIndex == pPortInfo->devIndex) &&\*/
			(pPeerIndex->portIndex == pPortInfo->portIndex) &&\
			(pPeerIndex->portIndex3 == pPortInfo->portIndex3) &&\
			(pPeerIndex->portIndex4 == pPortInfo->portIndex4) &&\
			(pPeerIndex->portIndex5 == pPortInfo->portIndex5) &&\
			(pPeerIndex->portIndex6 == pPortInfo->portIndex6) )
		{
			return GPN_ALM_GEN_YES;
		}
		peerNodeNum++;
		pAlmPeerPort = (stAlmPeerNode *)listNext((NODE *)(pAlmPeerPort));
	}
	return GPN_ALM_GEN_NO;
}

UINT32 gpnAlmScanPortInit(objLogicDesc *pPortInfo,
		stAlmScanTypeDef *pAlmScanType, stAlmPreScanIndex * pAlmPreScanIndex,
		stAlmLocalNode *pAlmLocalPort, stAlmPeerNode *pAlmPeerPort,
		stAlmScanPortInfo *pAlmScanPort, stAlmValueRecd *pAlmValueRecd,
		stAlmShakeRecd *pAlmShakeRecd)
{	
	/* local port node save in  pAlmPreScanIndex */
	pAlmPreScanIndex->pAlmLocalPort = pAlmLocalPort;

	/*init local port node */
	pAlmLocalPort->localPort = *pPortInfo;
	pAlmLocalPort->viewPort = *pPortInfo;
	pAlmLocalPort->pPeerNode = pAlmPeerPort;
	pAlmLocalPort->pAlmScanPort = pAlmScanPort;
	
	/*init peer port node */
	pAlmPeerPort->peerPort = *((optObjOrient *)pPortInfo);
	pAlmPeerPort->pAlmScanPort = pAlmScanPort;

	/*init scan port node */
	pAlmScanPort->pUppeAlmScanPort = NULL;
	pAlmScanPort->pBackAlmScanPort = NULL;

	pAlmScanPort->preQuenValid = GPN_ALM_GEN_DISABLE;
	pAlmScanPort->scanQuenValid = GPN_ALM_GEN_DISABLE;

	gpnAlmScanPortInfoClean(pAlmScanPort);
	
	pAlmScanPort->almNumInScanPort = pAlmScanType->almSubTpNumInScanType;
	pAlmScanPort->AlmValueBuff = pAlmValueRecd;	
	pAlmScanPort->AlmShakeBuff = pAlmShakeRecd; 

	/*pAlmScanPort->AlmValueBuff init when port add in validScanQuen */
	
	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmScanPortInfoClean(stAlmScanPortInfo *pAlmScanPort)
{
	UINT32 i;
	
	for(i=0;i<GPN_ALM_PAGE_NUM;i++)
   	{
		pAlmScanPort->iScanAlmDate[i] = 0;
		pAlmScanPort->iBaseAlmDate[i] = 0;
		pAlmScanPort->iRestAlmDate[i] = 0;
		pAlmScanPort->iInveAlmDate[i] = 0;
		pAlmScanPort->iReptAlmDate[i] = 0;
		pAlmScanPort->iDbSvAlmDate[i] = 0;
		
		pAlmScanPort->isScreBasePort[i] = 0;
		pAlmScanPort->isFiltBasePort[i] = 0;
		pAlmScanPort->isReptBasePort[i] = 0;
		pAlmScanPort->isRankBasePort[i] = 0;
		pAlmScanPort->isReckBasePort[i] = 0;

		pAlmScanPort->iScreAlmInfo[i] = 0;
		pAlmScanPort->iInveAlmInfo[i] = 0;
		pAlmScanPort->iFiltAlmInfo[i] = 0;
		pAlmScanPort->iReptAlmInfo[i] = 0;
		pAlmScanPort->iReckAlmInfo[i] = 0;

		pAlmScanPort->iFrehAlmMark[i] = 0;
		pAlmScanPort->iDelyAlmMark[i] = 0;
		pAlmScanPort->iSureAlmMark[i] = 0;
		
		/*add for alm shake*/
		pAlmScanPort->iShakAlmMark[i] = 0;
		pAlmScanPort->iShakOldMark[i] = 0;
   	}
	pAlmScanPort->iFrehAlmMark[GPN_ALM_PAGE_NUM] = 0;
	pAlmScanPort->iDelyAlmMark[GPN_ALM_PAGE_NUM] = 0;
	pAlmScanPort->iSureAlmMark[GPN_ALM_PAGE_NUM] = 0;
	
	/*add for alm shake*/
	pAlmScanPort->iShakAlmMark[GPN_ALM_PAGE_NUM] = 0;
	pAlmScanPort->iShakOldMark[GPN_ALM_PAGE_NUM] = 0;
	pAlmScanPort->iRestAlmMark = 0;
	
	for(i=0;i<GPN_EVT_PAGE_NUM;i++)
	{
		/*pAlmScanPort->iScanEvtDate[i] = 0;
		pAlmScanPort->iScreEvtDate[i] = 0;
		pAlmScanPort->iReptEvtDate[i] = 0;*/
		pAlmScanPort->iFrehEvtMark[i] = 0;
	}
	pAlmScanPort->iFrehEvtMark[GPN_EVT_PAGE_NUM] = 0;
	/*pAlmScanPort->iEventScreInfo = 0;
	pAlmScanPort->iEventFiltInfo = 0;
	pAlmScanPort->iEventReptInfo = 0;*/

	return GPN_ALM_GEN_YES;
}

UINT32 gpnAlmRelevChgAlmScanPortToPreScanQuen(stAlmPreScanIndex *pAlmPreScanIndex, UINT32 opt)
{
	stAlmLocalNode *pAlmLocalPort;
	stAlmPeerNode *pAlmPeerPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmPreScanQuen *pAlmPreScanNode;
	UINT32 hashValue;

	pAlmLocalPort = pAlmPreScanIndex->pAlmLocalPort;
	pAlmPeerPort = pAlmLocalPort->pPeerNode;
	pAlmScanPort = pAlmLocalPort->pAlmScanPort;

	/* only ETH/E1/ETH-SFP portType can be add
	 * add by lipf, 2018/4/13
	 */
	/*if(GPN_PINDEX_GEN_NO == IS_PHY_PORT(pAlmLocalPort->localPort.portIndex))
	{
		return GPN_ALM_GEN_YES;
	}*/
	
	if( (opt == GPN_ALM_GEN_ENABLE) &&
		(pAlmScanPort->preQuenValid == GPN_ALM_GEN_DISABLE) )
	{
		/* check if local port in perScanQuen */
		pAlmPreScanNode = NULL;
		gpnAlmSeekPortIndexToAlmPreScanNode(&(pAlmLocalPort->localPort), &pAlmPreScanNode);
		if(pAlmPreScanNode != NULL)
		{
			if(GPN_ALM_GEN_OK != gpnAlmRelevCheckIsLocalInPerScanQuen(\
				&(pAlmLocalPort->localPort), pAlmPreScanNode))
			{
				/* add pAlmPreScanIndex(local port) in perScanQuen */
				hashValue = ((pAlmLocalPort->localPort.portIndex) % (pAlmPreScanNode->hashKey));
				listAdd(&(pAlmPreScanNode->almPreScanQuen[hashValue]),(NODE*)pAlmPreScanIndex);
				pAlmPreScanNode->preScanNodeNum[hashValue]++;

				GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : add local port(%08x|%08x|%08x|%08x|%08x|%08x) to almPreScanQuen.\n\r",\
					__FUNCTION__,\
					pAlmLocalPort->localPort.devIndex,\
					pAlmLocalPort->localPort.portIndex,\
					pAlmLocalPort->localPort.portIndex3,\
					pAlmLocalPort->localPort.portIndex4,\
					pAlmLocalPort->localPort.portIndex5,\
					pAlmLocalPort->localPort.portIndex6);
				zlog_debug(ALARM_DBG_REGISTER, "%s[%d] : add local port(%08x|%08x|%08x|%08x|%08x|%08x) to almPreScanQuen.\n\r",\
					__FUNCTION__, __LINE__,\
					pAlmLocalPort->localPort.devIndex,\
					pAlmLocalPort->localPort.portIndex,\
					pAlmLocalPort->localPort.portIndex3,\
					pAlmLocalPort->localPort.portIndex4,\
					pAlmLocalPort->localPort.portIndex5,\
					pAlmLocalPort->localPort.portIndex6);
			}
		}

		/* check if peer port in peerPortQuen */
		pAlmPreScanNode = NULL;
		gpnAlmSeekPortIndexToAlmPreScanNode((objLogicDesc *)(&(pAlmPeerPort->peerPort)), &pAlmPreScanNode);
		if(pAlmPreScanNode != NULL)
		{
			if(GPN_ALM_GEN_OK != gpnAlmRelevCheckIsPeerInPeerPortQuen(\
				&(pAlmPeerPort->peerPort), pAlmPreScanNode))
			{
				/* add peer port in peerPortQuen */
				hashValue = ((pAlmPeerPort->peerPort.portIndex) % (pAlmPreScanNode->hashKey));
				listAdd(&(pAlmPreScanNode->almPeerPortQuen[hashValue]),(NODE*)pAlmPeerPort);
				pAlmPreScanNode->peerPortNodeNum[hashValue]++;

				GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : add peer port(%08x|%08x|%08x|%08x|%08x|%08x) to almPeerPortQuen.\n\r",\
					__FUNCTION__,\
					pAlmPeerPort->peerPort.devIndex,\
					pAlmPeerPort->peerPort.portIndex,\
					pAlmPeerPort->peerPort.portIndex3,\
					pAlmPeerPort->peerPort.portIndex4,\
					pAlmPeerPort->peerPort.portIndex5,\
					pAlmPeerPort->peerPort.portIndex6);
			}
		}
		
		pAlmScanPort->preQuenValid = GPN_ALM_GEN_ENABLE;
	}
	else if((opt == GPN_ALM_GEN_DISABLE) &&
			(pAlmScanPort->preQuenValid == GPN_ALM_GEN_ENABLE))
	{
		/* if scan port in valid scan quen, delete from valid quen first */
		if(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_ENABLE)
		{
			/* check if ...... ??????????????? */
			gpnAlmRelevChgAlmScanPortToValidScanQuen(pAlmLocalPort, GPN_ALM_GEN_DISABLE);
		}

		/* check if local port in perScanQuen */
		pAlmPreScanNode = NULL;
		gpnAlmSeekPortIndexToAlmPreScanNode(&(pAlmLocalPort->localPort), &pAlmPreScanNode);
		if(pAlmPreScanNode != NULL)
		{
			if(GPN_ALM_GEN_OK == gpnAlmRelevCheckIsLocalInPerScanQuen(\
				&(pAlmLocalPort->localPort), pAlmPreScanNode))
			{
				/* local port out of perScanQuen */
				hashValue = ((pAlmLocalPort->localPort.portIndex) % (pAlmPreScanNode->hashKey));
				listDelete(&(pAlmPreScanNode->almPreScanQuen[hashValue]),(NODE*)pAlmPreScanIndex);
				pAlmPreScanNode->preScanNodeNum[hashValue]--;

				GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : delete local port(%08x|%08x|%08x|%08x|%08x|%08x) out almPreScanQuen.\n\r",\
					__FUNCTION__,\
					pAlmLocalPort->localPort.devIndex,\
					pAlmLocalPort->localPort.portIndex,\
					pAlmLocalPort->localPort.portIndex3,\
					pAlmLocalPort->localPort.portIndex4,\
					pAlmLocalPort->localPort.portIndex5,\
					pAlmLocalPort->localPort.portIndex6);
			}
			else
			{
				GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : local port(%08x|%08x|%08x|%08x|%08x|%08x) not in almPreScanQuen.\n\r",\
					__FUNCTION__,\
					pAlmLocalPort->localPort.devIndex,\
					pAlmLocalPort->localPort.portIndex,\
					pAlmLocalPort->localPort.portIndex3,\
					pAlmLocalPort->localPort.portIndex4,\
					pAlmLocalPort->localPort.portIndex5,\
					pAlmLocalPort->localPort.portIndex6);
			}
		}
		else
		{
			GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : local port(%08x|%08x|%08x|%08x|%08x|%08x) not found pAlmPreScanNode, err!\n\r",\
				__FUNCTION__,\
				pAlmLocalPort->localPort.devIndex,\
				pAlmLocalPort->localPort.portIndex,\
				pAlmLocalPort->localPort.portIndex3,\
				pAlmLocalPort->localPort.portIndex4,\
				pAlmLocalPort->localPort.portIndex5,\
				pAlmLocalPort->localPort.portIndex6);
		}

		/* check if peer port in perScanQuen */
		pAlmPreScanNode = NULL;
		gpnAlmSeekPortIndexToAlmPreScanNode((objLogicDesc *)(&(pAlmPeerPort->peerPort)), &pAlmPreScanNode);
		if(pAlmPreScanNode != NULL)
		{
			if(GPN_ALM_GEN_OK == gpnAlmRelevCheckIsPeerInPeerPortQuen(\
				&(pAlmPeerPort->peerPort), pAlmPreScanNode))
			{
				/* add peer port in peerPortQuen */
				hashValue = ((pAlmPeerPort->peerPort.portIndex) % (pAlmPreScanNode->hashKey));
				listDelete(&(pAlmPreScanNode->almPeerPortQuen[hashValue]),(NODE*)pAlmPeerPort);
				pAlmPreScanNode->peerPortNodeNum[hashValue]--;

				GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : delete peer port(%08x|%08x|%08x|%08x|%08x|%08x) out almPeerPortQuen.\n\r",\
					__FUNCTION__,\
					pAlmPeerPort->peerPort.devIndex,\
					pAlmPeerPort->peerPort.portIndex,\
					pAlmPeerPort->peerPort.portIndex3,\
					pAlmPeerPort->peerPort.portIndex4,\
					pAlmPeerPort->peerPort.portIndex5,\
					pAlmPeerPort->peerPort.portIndex6);
			}
			else
			{
				GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : perr port(%08x|%08x|%08x|%08x|%08x|%08x) not in almPeerPortQuen.\n\r",\
					__FUNCTION__,\
					pAlmPeerPort->peerPort.devIndex,\
					pAlmPeerPort->peerPort.portIndex,\
					pAlmPeerPort->peerPort.portIndex3,\
					pAlmPeerPort->peerPort.portIndex4,\
					pAlmPeerPort->peerPort.portIndex5,\
					pAlmPeerPort->peerPort.portIndex6);
			}
		}
		else
		{
			GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : peer port(%08x|%08x|%08x|%08x|%08x|%08x) not found pAlmPreScanNode, err!\n\r",\
				__FUNCTION__,\
				pAlmPeerPort->peerPort.devIndex,\
				pAlmPeerPort->peerPort.portIndex,\
				pAlmPeerPort->peerPort.portIndex3,\
				pAlmPeerPort->peerPort.portIndex4,\
				pAlmPeerPort->peerPort.portIndex5,\
				pAlmPeerPort->peerPort.portIndex6);
		}

		pAlmScanPort->preQuenValid = GPN_ALM_GEN_DISABLE;
	}
	else
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : opt %d err with preQuenValid %d, port(%08x|%08x|%08x|%08x|%08x|%08x) not in almPreScanQuen.\n\r",\
			__FUNCTION__, opt, pAlmScanPort->preQuenValid,\
			pAlmLocalPort->localPort.devIndex,\
			pAlmLocalPort->localPort.portIndex,\
			pAlmLocalPort->localPort.portIndex3,\
			pAlmLocalPort->localPort.portIndex4,\
			pAlmLocalPort->localPort.portIndex5,\
			pAlmLocalPort->localPort.portIndex6);
		return GPN_ALM_GEN_NO;
	}

	return GPN_ALM_GEN_YES;
}

UINT32 gpnAlmRelevChgAlmScanPortToValidScanQuen(stAlmLocalNode *pAlmLocalPort, UINT32 opt)
{	
	stAlmScanPortInfo *pAlmScanPort;
	stAlmPreScanQuen *pAlmPreScanNode;

	pAlmScanPort = pAlmLocalPort->pAlmScanPort;
	
	pAlmPreScanNode = NULL;
	gpnAlmSeekPortIndexToAlmPreScanNode(&(pAlmLocalPort->localPort), &pAlmPreScanNode);
	if(pAlmPreScanNode == NULL)
	{
		return GPN_ALM_GEN_NO;
	}

	if( (opt == GPN_ALM_GEN_DISABLE) &&
		(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_ENABLE))
	{
		/*首先pAlmScanPort必在almPreScanQuen里面，这是传入参数的要求*/
		/*其次也不检查pAlmScanPort是否在almScanPortObjQuen里面，这由其他执行模块保证*/
		listDelete(&(pAlmPreScanNode->almScanTypeNode->almScanPortObjQuen),(NODE*)(pAlmLocalPort));
		pAlmPreScanNode->almScanTypeNode->almScanQuenPortNum--;
		pAlmScanPort->scanQuenValid = GPN_ALM_GEN_DISABLE;

		/*初始化有效扫描队列对扫描端口及数据表甚至网管数据的影响*/
		gpnAlmMomAlmChgPorc(pAlmPreScanNode->almScanTypeNode, pAlmLocalPort, opt);

		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : delete port(%08x|%08x|%08x|%08x|%08x|%08x) out validScanQuen.\n\r",\
			__FUNCTION__,\
			pAlmLocalPort->localPort.devIndex,\
			pAlmLocalPort->localPort.portIndex,\
			pAlmLocalPort->localPort.portIndex3,\
			pAlmLocalPort->localPort.portIndex4,\
			pAlmLocalPort->localPort.portIndex5,\
			pAlmLocalPort->localPort.portIndex6);
	}
	else if((opt == GPN_ALM_GEN_ENABLE) &&
			(pAlmScanPort->scanQuenValid == GPN_ALM_GEN_DISABLE))
	{
		listAdd(&(pAlmPreScanNode->almScanTypeNode->almScanPortObjQuen),(NODE*)(pAlmLocalPort));
		pAlmPreScanNode->almScanTypeNode->almScanQuenPortNum++;
		pAlmScanPort->scanQuenValid = GPN_ALM_GEN_ENABLE;

		/*初始化有效扫描队列对扫描端口及数据表甚至网管数据的影响*/
		gpnAlmMomAlmChgPorc(pAlmPreScanNode->almScanTypeNode, pAlmLocalPort, opt);

		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_SVP, "%s : add port(%08x|%08x|%08x|%08x|%08x|%08x) in validScanQuen.\n\r",\
			__FUNCTION__,\
			pAlmLocalPort->localPort.devIndex,\
			pAlmLocalPort->localPort.portIndex,\
			pAlmLocalPort->localPort.portIndex3,\
			pAlmLocalPort->localPort.portIndex4,\
			pAlmLocalPort->localPort.portIndex5,\
			pAlmLocalPort->localPort.portIndex6);

	}
	else
	{
		return GPN_ALM_GEN_NO;	
	}
	
	return GPN_ALM_GEN_YES;
}

UINT32 gpnEQUAlmPreScanProc(void)
{
	/* slot 0 */
	/* gpnAlmFixMonitorsSpacePerdistribution(EQU_MAIN_BOD_NUMEQU);*/
	return GPN_ALM_GEN_YES;
}

UINT32 gpnAlmScanDateStInit(void)
{	
	stUnitAlmProcSpace *pslotAlmProcSpace;
	stAlmPreScanQuen *pAlmPreScanQuenNode;
	stAlmScanTypeDef *pAlmScanTypeNode;
	stAlmPTpVsSTpTemp *pAlmPTpVsSTpTemp;
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stppAlmPreScanNode *ppAlmPreScanNode;

	UINT32 scanTpNum;
	UINT32 preScanTpNum;

	UINT32 i;
	UINT32 j;
	UINT32 k;

	i = listCount(&(pgstAlmTypeWholwInfo->almScanTypeQuen));
	if(i != pgstAlmTypeWholwInfo->almScanTypeNum)
	{
		GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : almScanType Num err list %d %d\n\r",\
			__FUNCTION__, i,pgstAlmTypeWholwInfo->almScanTypeNum);
		return GPN_ALM_GEN_NO;
	}
	j = sizeof(stEQUAlmProcSpace) +
		(EQU_SLOT_MAX_ID + 1) * sizeof(stUnitAlmProcSpace) +
		(EQU_SLOT_MAX_ID + 1) * i * sizeof(stAlmPreScanQuen);
	
	pgstAlmPreScanWholwInfo = (stEQUAlmProcSpace *)malloc(j);
	/* memset first, then set init value */
	memset((UINT8 *)pgstAlmPreScanWholwInfo,0,j);
	pgstAlmPreScanWholwInfo->unitAlmProcSpace = (stUnitAlmProcSpace*)(pgstAlmPreScanWholwInfo + 1);
		
	/* build pAlmPreScanQuenNode to unitAlmProcSpace relationship, init pAlmPreScanQuenNode */
	pAlmPreScanQuenNode = (stAlmPreScanQuen *)(pgstAlmPreScanWholwInfo->unitAlmProcSpace + (EQU_SLOT_MAX_ID + 1));
	for(i=0;i<(EQU_SLOT_MAX_ID + 1);i++)
	{
		/* unitAlmProcSpace is what ???  */
		pslotAlmProcSpace = pgstAlmPreScanWholwInfo->unitAlmProcSpace + i;

		/* init each slot's pslotAlmProcSpace */
		listInit(&(pslotAlmProcSpace->almPreScanTpQuen));
		pslotAlmProcSpace->quenNodeNum = 0;
		pslotAlmProcSpace->slotDevSta = GPN_ALM_DEV_STA_NULL;
		/* record mem start ( total have almScanTypeNum node in same slot) */
		pslotAlmProcSpace->pAlmPreScanQuenMemSpace = pAlmPreScanQuenNode;
		listInit(&(pslotAlmProcSpace->RemDevAlmPorcNodeQuen));
		pslotAlmProcSpace->RemDevAlmPorcNodeNum = 0;
		
		scanTpNum = 0;
		pAlmScanTypeNode = (stAlmScanTypeDef *)listFirst(&(pgstAlmTypeWholwInfo->almScanTypeQuen));
		while((pAlmScanTypeNode != NULL) && (scanTpNum < pgstAlmTypeWholwInfo->almScanTypeNum))
		{	
			/* add pAlmPreScanQuenNode base scanType by each slot, and use hash in pAlmPreScanQuenNode,
			    so when search port, here have slot , scanType , hash total 3 level hash */
			listAdd(&(pslotAlmProcSpace->almPreScanTpQuen),(NODE*)pAlmPreScanQuenNode);	
			pslotAlmProcSpace->quenNodeNum++;
			
			for(k=0;k<GPN_ALM_PRESCAN_HASH;k++)
			{
				listInit(&(pAlmPreScanQuenNode->almPeerPortQuen[k]));
				pAlmPreScanQuenNode->peerPortNodeNum[k] = 0;
				
				listInit(&(pAlmPreScanQuenNode->almPreScanQuen[k]));
				pAlmPreScanQuenNode->preScanNodeNum[k] = 0;
			}
			/* here how to set hash key ??? todo */
			pAlmPreScanQuenNode->hashKey = GPN_ALM_PRESCAN_HASH;
			pAlmPreScanQuenNode->almScanType = pAlmScanTypeNode->almScanType;
			/* give how to find pAlmScanTypeNode base pAlmPreScanQuenNode */
			pAlmPreScanQuenNode->almScanTypeNode = pAlmScanTypeNode;

			scanTpNum++;
			pAlmScanTypeNode = (stAlmScanTypeDef *)listNext((NODE*)(pAlmScanTypeNode));
			pAlmPreScanQuenNode++;
		}
		if((pAlmScanTypeNode != NULL) || (scanTpNum < pgstAlmTypeWholwInfo->almScanTypeNum))
		{
			GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : pAlmPreScanQuenNode init err\n\r", __FUNCTION__);
			return GPN_ALM_GEN_NO;
		}
	}

	/* portType vs scanType : must be n vs 1 , shold not be m vs n */
	i = sizeof(gAlmPTpVsSTpTemp)/sizeof(gAlmPTpVsSTpTemp[0]);
	j = pgstAlmTypeWholwInfo->almScanTypeNum;

	/* build portType to each slot's scanType's stppAlmPreScanNode's relationship */
	k = i * sizeof(stPortTpToAlmScanTp) + (EQU_SLOT_MAX_ID + 1) * j * sizeof(stppAlmPreScanNode);
	pPortTpToScanTp = (stPortTpToAlmScanTp*)malloc(k);
	memset((UINT8 *)(pPortTpToScanTp),0,k);

	/* init pointer portType to stppAlmPreScanNode */
	pgstAlmPreScanWholwInfo->PTpVsSTpRelation.pPortTpToScanTp = pPortTpToScanTp;
	listInit(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen));
	pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum = i;
	pgstAlmPreScanWholwInfo->PTpVsSTpRelation.pAlmPreScanNode =\
		(stppAlmPreScanNode *)(pPortTpToScanTp + pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum);

	/* init pPortTpToScanTp */
	pAlmPTpVsSTpTemp = gAlmPTpVsSTpTemp;
	pPortTpToScanTp = pgstAlmPreScanWholwInfo->PTpVsSTpRelation.pPortTpToScanTp;	
	ppAlmPreScanNode = pgstAlmPreScanWholwInfo->PTpVsSTpRelation.pAlmPreScanNode;
	for(i=0;i<(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum);i++)
	{
		scanTpNum = 0;
		pAlmScanTypeNode = (stAlmScanTypeDef *)listFirst(&(pgstAlmTypeWholwInfo->almScanTypeQuen));
		while((pAlmScanTypeNode != NULL)&&(scanTpNum < pgstAlmTypeWholwInfo->almScanTypeNum))
		{
			if(pAlmPTpVsSTpTemp->almScanType == pAlmScanTypeNode->almScanType)
			{
				pPortTpToScanTp->portType = pAlmPTpVsSTpTemp->portType;
				pPortTpToScanTp->portTpName = pAlmPTpVsSTpTemp->portTpName;
				/* here is good brilliant, build portType to scanType's n vs 1 relation through scanTypnNum */
				pPortTpToScanTp->ppAlmPreScanNode = ppAlmPreScanNode + ((EQU_SLOT_MAX_ID + 1) * scanTpNum);
				pPortTpToScanTp->almSubTpNumInPortType = pAlmScanTypeNode->almSubTpNumInScanType;
				
				break;
			}
			scanTpNum++;
			pAlmScanTypeNode = (stAlmScanTypeDef *)listNext((NODE *)(pAlmScanTypeNode));
		}
		if((pAlmScanTypeNode == NULL) || (scanTpNum >= pgstAlmTypeWholwInfo->almScanTypeNum))
		{
			GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_EGP, "%s : almScanTypeQuen & gAlmPTpVsSTpTemp ScanType inconformity\n\r", __FUNCTION__);
			return GPN_ALM_GEN_NO;
		}
		listAdd(&(pgstAlmPreScanWholwInfo->PTpVsSTpRelation.PTpVsSTpQuen),(NODE*)pPortTpToScanTp);
		/*GPN_ALM_SCAN_PRINT(GPN_ALM_SCAN_CUP, "%s : pPortTpToScanTp:addr %08x,portTp %08x ppAlmPreScanNode-addr %08x\n\r",\
			__FUNCTION__, (UINT32)pPortTpToScanTp,pAlmScanTypeNode->almScanType,(UINT32)pPortTpToScanTp->ppAlmPreScanNode);*/
		pPortTpToScanTp++;
		pAlmPTpVsSTpTemp++;
	}

	/* build pAlmPreScanQuenNode to ppAlmPreScanNode's relation, and init ppAlmPreScanNode */
	scanTpNum = 0;
	pAlmScanTypeNode = (stAlmScanTypeDef *)listFirst(&(pgstAlmTypeWholwInfo->almScanTypeQuen));
	while((pAlmScanTypeNode != NULL)&&(scanTpNum < pgstAlmTypeWholwInfo->almScanTypeNum))
	{	
		for(i=0;i<(EQU_SLOT_MAX_ID + 1);i++)
		{
			/*pgstAlmPreScanWholwInfo->PTpVsSTpRelation.pAlmPreScanNode and scanTpNum have special relation */
			ppAlmPreScanNode = pgstAlmPreScanWholwInfo->PTpVsSTpRelation.pAlmPreScanNode + (((EQU_SLOT_MAX_ID + 1) * scanTpNum) + i);
			
			preScanTpNum = 0;
			pAlmPreScanQuenNode = (stAlmPreScanQuen *)listFirst(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].almPreScanTpQuen));
			while((pAlmScanTypeNode != NULL)&&(preScanTpNum < pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].quenNodeNum))
			{
				if(pAlmScanTypeNode->almScanType == pAlmPreScanQuenNode->almScanType)
				{
					ppAlmPreScanNode->pAlmPreScanNode = pAlmPreScanQuenNode;
					ppAlmPreScanNode->slot = i;
					break;

				}
				preScanTpNum++;
				pAlmPreScanQuenNode = (stAlmPreScanQuen *)listNext((NODE *)(pAlmPreScanQuenNode));
			}
		}
		scanTpNum++;
		pAlmScanTypeNode = (stAlmScanTypeDef *)listNext((NODE *)(pAlmScanTypeNode));
	}

	/* dev perScan build */
	gpnEQUAlmPreScanProc();

	return GPN_ALM_GEN_YES;
}
UINT32 gpnAlmPreScanInitClean(void)
{
	return GPN_ALM_GEN_YES;	
}

UINT32 gpnAlmDataStInit(void)
{	
	/* alm type data struct init */
	gpnAlmStructInit();
	/* event type data struct init: must after alm init */
	gpnEvtStructInit();
	/* alm scan data struct init */
	gpnAlmScanDateStInit();
	/* alm RAM dataSheet init */
	gpnAlmRAMDataSheetInit();

	return GPN_ALM_GEN_YES;
}
UINT32 debugGPNAlmPreScanNode(void)
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

	for(i=0;i<(EQU_SLOT_MAX_ID + 1);i++)
	{
		preScanPortNum = 0;
		preScanTypeNum = 0;
		pAlmPreScanTypeNode = (stAlmPreScanQuen *)listFirst(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].almPreScanTpQuen));
		while((pAlmPreScanTypeNode != NULL)&&(preScanTypeNum < pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].quenNodeNum))
		{
			for(j=0;j<GPN_ALM_PRESCAN_HASH;j++)
			{	
				preScanPortNum += pAlmPreScanTypeNode->preScanNodeNum[j];
			}
			preScanTypeNum++;
			pAlmPreScanTypeNode = (stAlmPreScanQuen *)listNext((NODE *)(pAlmPreScanTypeNode));
		}
		printf("slot %d has %d monitors,detials as follows\n",i,preScanPortNum);
		printf(" Num. devLevel devIndex portType        localIndex         viewPort         peerIndex isInScan(per) ScanType(----name--)\n");
		printf("--------------------------------------------------------------------------------------------------------------------\n");
		
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

					printf("%5d %8d %08x %8x  %08x%08x  %08x%08x %08x%08x %8d(%3d) %8d",\
						++Num,\
						DeviceIndex_GetLevel(pAlmLocalPort->localPort.devIndex),\
						pAlmLocalPort->localPort.devIndex,\
						PortIndex_GetType(pAlmLocalPort->localPort.portIndex),\
						pAlmLocalPort->localPort.devIndex,\
						pAlmLocalPort->localPort.portIndex,\
						pAlmLocalPort->viewPort.devIndex,\
						pAlmLocalPort->viewPort.portIndex,\
						pAlmPeerPort->peerPort.devIndex,\
						pAlmPeerPort->peerPort.portIndex,\
						pAlmScanPort->scanQuenValid,\
						pAlmScanPort->preQuenValid,\
						pAlmPreScanTypeNode->almScanType);
						
					printf("(%12s)\n",pAlmPreScanTypeNode->almScanTypeNode->almScanTypeName);
					preScanPortNum++;
					pAlmPreScanIndex = (stAlmPreScanIndex *)listNext((NODE *)(pAlmPreScanIndex));
				}
			}

			preScanTypeNum++;
			pAlmPreScanTypeNode = (stAlmPreScanQuen *)listNext((NODE *)(pAlmPreScanTypeNode));
		}

		printf("========================================================================================\n");
		printf(" Num. devLevel devIndex portType        peerIndex isInScan(per) ScanType(----name--)\n");
		printf("--------------------------------------------------------------------------------------------------------------------\n");
		
		preScanTypeNum = 0;
		pAlmPreScanTypeNode = (stAlmPreScanQuen *)listFirst(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].almPreScanTpQuen));
		while((pAlmPreScanTypeNode != NULL)&&(preScanTypeNum < pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].quenNodeNum))
		{
			for(j=0;j<GPN_ALM_PRESCAN_HASH;j++)
			{
				preScanPortNum = 0;
				pAlmPeerPort = (stAlmPeerNode *)listFirst(&(pAlmPreScanTypeNode->almPeerPortQuen[j]));
				while((pAlmPeerPort != NULL)&&(preScanPortNum<pAlmPreScanTypeNode->peerPortNodeNum[j]))
				{
					pAlmScanPort = pAlmPeerPort->pAlmScanPort;

					printf("%5d %8d %08x %8x  %08x%08x %8d(%3d) %8d",\
						++Num,\
						DeviceIndex_GetLevel(pAlmPeerPort->peerPort.devIndex),\
						pAlmPeerPort->peerPort.devIndex,\
						PortIndex_GetType(pAlmPeerPort->peerPort.portIndex),\
						pAlmPeerPort->peerPort.devIndex,\
						pAlmPeerPort->peerPort.portIndex,\
						pAlmScanPort->scanQuenValid,\
						pAlmScanPort->preQuenValid,\
						pAlmPreScanTypeNode->almScanType);
						
					printf("(%12s)\n",pAlmPreScanTypeNode->almScanTypeNode->almScanTypeName);
					preScanPortNum++;
					pAlmPeerPort = (stAlmPeerNode *)listNext((NODE *)(pAlmPeerPort));
				}
			}

			preScanTypeNum++;
			pAlmPreScanTypeNode = (stAlmPreScanQuen *)listNext((NODE *)(pAlmPreScanTypeNode));
		}
	}
	return GPN_ALM_GEN_YES;
}
UINT32 debugGpnAlmNowScanNode(void)
{
	stAlmLocalNode *pAlmLocalPort;
	stAlmPeerNode *pAlmPeerPort;
	stAlmScanPortInfo *pAlmScanPort;
	stAlmScanTypeDef *pAlmScanType;
	UINT32 scanTpNum;
	UINT32 portNum;
	UINT32 Num;
	
	printf(" Num. Lev slot devIndex portType  ScanTyp localIndex       viewIndex       peerIndex\n");
	printf("    screBasePort                         screInfo                             ScanAlmDate\n");
	printf("    filtBasePort                         filtInfo                             BaseAlmDate\n");
	printf("    reptBasePort                         reptInfo                             RestAlmDate\n");
	printf("    rankBasePort                         iInvInfo                             InveAlmDate\n");
	printf("    reckBasePort                         reckInfo                             ReptAlmDate\n");
	printf("                                                                              DbSvAlmDate\n");
	printf("    FreshMask\n\rDelyMask\n\rSureMask\n\rRestMask\n");
	printf("------------------------------------------------------------------------------------------------------------------------------------------------------\n");

	Num = 0;
	scanTpNum = 0;
	pAlmScanType = (stAlmScanTypeDef *)listFirst(&(pgstAlmTypeWholwInfo->almScanTypeQuen));
	while((pAlmScanType != NULL)&&(scanTpNum < pgstAlmTypeWholwInfo->almScanTypeNum))
	{	
		portNum = 0;
		pAlmLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
		while((pAlmLocalPort != NULL)&&(portNum < pAlmScanType->almScanQuenPortNum))
		{
			pAlmPeerPort = pAlmLocalPort->pPeerNode;
			pAlmScanPort = pAlmLocalPort->pAlmScanPort;
			printf("%5d %3d %4d %08x %08x %08x %08x%08x %08x%08x  %08x|%08x|%08x|%08x|%08x|%08x\n",\
				++Num,\
				DeviceIndex_GetLevel(pAlmLocalPort->localPort.devIndex),\
				PortIndex_GetSlot(pAlmLocalPort->localPort.portIndex),\
				pAlmLocalPort->localPort.devIndex,\
				PortIndex_GetType(pAlmLocalPort->localPort.portIndex),\
				pAlmScanType->almScanType,\
				pAlmLocalPort->localPort.devIndex,pAlmLocalPort->localPort.portIndex,\
				pAlmLocalPort->viewPort.devIndex,pAlmLocalPort->viewPort.portIndex,\
				pAlmPeerPort->peerPort.devIndex,pAlmPeerPort->peerPort.portIndex,\
				pAlmPeerPort->peerPort.portIndex3,pAlmPeerPort->peerPort.portIndex4,\
				pAlmPeerPort->peerPort.portIndex5,pAlmPeerPort->peerPort.portIndex6);
			 printf("    %08x %08x %08x %08x  %08x %08x %08x %08x  %08x %08x %08x %08x scan\n",\
			 	pAlmScanPort->isScreBasePort[3],pAlmScanPort->isScreBasePort[2],\
				pAlmScanPort->isScreBasePort[1],pAlmScanPort->isScreBasePort[0],\
				pAlmScanPort->iScreAlmInfo[3],pAlmScanPort->iScreAlmInfo[2],\
				pAlmScanPort->iScreAlmInfo[1],pAlmScanPort->iScreAlmInfo[0],\
			 	pAlmScanPort->iScanAlmDate[3],pAlmScanPort->iScanAlmDate[2],\
				pAlmScanPort->iScanAlmDate[1],pAlmScanPort->iScanAlmDate[0]);
             printf("    %08x %08x %08x %08x  %08x %08x %08x %08x  %08x %08x %08x %08x base\n",\
			 	pAlmScanPort->isFiltBasePort[3],pAlmScanPort->isFiltBasePort[2],\
				pAlmScanPort->isFiltBasePort[1],pAlmScanPort->isFiltBasePort[0],\
				pAlmScanPort->iFiltAlmInfo[3],pAlmScanPort->iFiltAlmInfo[2],\
				pAlmScanPort->iFiltAlmInfo[1],pAlmScanPort->iFiltAlmInfo[0],\
			 	pAlmScanPort->iBaseAlmDate[3],pAlmScanPort->iBaseAlmDate[2],\
				pAlmScanPort->iBaseAlmDate[1],pAlmScanPort->iBaseAlmDate[0]);
             printf("    %08x %08x %08x %08x  %08x %08x %08x %08x  %08x %08x %08x %08x rest\n",\
			 	pAlmScanPort->isReptBasePort[3],pAlmScanPort->isReptBasePort[2],\
				pAlmScanPort->isReptBasePort[1],pAlmScanPort->isReptBasePort[0],\
				pAlmScanPort->iReptAlmInfo[3],pAlmScanPort->iReptAlmInfo[2],\
				pAlmScanPort->iReptAlmInfo[1],pAlmScanPort->iReptAlmInfo[0],\
			 	pAlmScanPort->iRestAlmDate[3],pAlmScanPort->iRestAlmDate[2],\
				pAlmScanPort->iRestAlmDate[1],pAlmScanPort->iRestAlmDate[0]);
             printf("    %08x %08x %08x %08x  %08x %08x %08x %08x  %08x %08x %08x %08x inve\n",\
			 	pAlmScanPort->isRankBasePort[3],pAlmScanPort->isRankBasePort[2],\
				pAlmScanPort->isRankBasePort[1],pAlmScanPort->isRankBasePort[0],\
				pAlmScanPort->iInveAlmInfo[3],pAlmScanPort->iInveAlmInfo[2],\
				pAlmScanPort->iInveAlmInfo[1],pAlmScanPort->iInveAlmInfo[0],\
			 	pAlmScanPort->iInveAlmDate[3],pAlmScanPort->iInveAlmDate[2],\
				pAlmScanPort->iInveAlmDate[1],pAlmScanPort->iInveAlmDate[0]);
             printf("    %08x %08x %08x %08x  %08x %08x %08x %08x  %08x %08x %08x %08x rept\n",\
			 	pAlmScanPort->isReckBasePort[3],pAlmScanPort->isReckBasePort[2],\
				pAlmScanPort->isReckBasePort[1],pAlmScanPort->isReckBasePort[0],\
				pAlmScanPort->iReckAlmInfo[3],pAlmScanPort->iReckAlmInfo[2],\
				pAlmScanPort->iReckAlmInfo[1],pAlmScanPort->iReckAlmInfo[0],\
				pAlmScanPort->iReptAlmDate[3],pAlmScanPort->iReptAlmDate[2],\
				pAlmScanPort->iReptAlmDate[1],pAlmScanPort->iReptAlmDate[0]);
			 printf("                                                                              %08x %08x %08x %08x db\n\r",\
			 	pAlmScanPort->iDbSvAlmDate[3],pAlmScanPort->iDbSvAlmDate[2],\
				pAlmScanPort->iDbSvAlmDate[1],pAlmScanPort->iDbSvAlmDate[0]);
			 printf("    %08x %08x %08x %08x %08x\n\r",\
			 	pAlmScanPort->iFrehAlmMark[4],pAlmScanPort->iFrehAlmMark[3],\
				pAlmScanPort->iFrehAlmMark[2],pAlmScanPort->iFrehAlmMark[1],\
				pAlmScanPort->iFrehAlmMark[0]);
			 printf("    %08x %08x %08x %08x %08x\n\r",\
				pAlmScanPort->iDelyAlmMark[4],pAlmScanPort->iDelyAlmMark[3],\
				pAlmScanPort->iDelyAlmMark[2],pAlmScanPort->iDelyAlmMark[1],\
				pAlmScanPort->iDelyAlmMark[0]);
			 printf("    %08x %08x %08x %08x %08x\n\r    %08x\n\r",\
				pAlmScanPort->iSureAlmMark[4],pAlmScanPort->iSureAlmMark[3],\
				pAlmScanPort->iSureAlmMark[2],pAlmScanPort->iSureAlmMark[1],\
				pAlmScanPort->iSureAlmMark[0],\
				pAlmScanPort->iRestAlmMark);
			portNum++;
			pAlmLocalPort = (stAlmLocalNode *)listNext((NODE *)(pAlmLocalPort));
		}
		scanTpNum++;
		pAlmScanType = (stAlmScanTypeDef *)listNext((NODE *)(pAlmScanType));
	}
	return GPN_ALM_GEN_YES;
}

UINT32 debugGPNAlmScanStruct(void)
{
	stPTpVsAlmSTpRelation *pPTpVsSTpRelation;
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stUnitAlmProcSpace *pslotAlmProcSpace;
	stRemDevAlmProcNode *pRemDevAlmProcNode;
	stAlmPreScanQuen *pAlmPreScanTypeNode;
	UINT32 sysPortTypeNum;
	UINT32 sysScanTypeNUm;
	UINT32 almScanPortNum;
	UINT32 remEevNum;
	UINT32 portType;
	UINT32 i;
	UINT32 j;

	pPTpVsSTpRelation = &(pgstAlmPreScanWholwInfo->PTpVsSTpRelation);
	printf("time:%08x\n\r",(UINT32)pgstAlmPreScanWholwInfo->almTaskTime);
	printf("sys support port type num:%d\n\r",pPTpVsSTpRelation->sysSupPortTpNum);
	printf("portType scanType     portTypeName     scanTypeName\n");
	printf("-----------------------------------------------------\n");
		
	sysPortTypeNum = 0;
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pPTpVsSTpRelation->PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pPTpVsSTpRelation->sysSupPortTpNum))
	{
		printf("%08x %08x %16.16s %16.16s\n",\
			pPortTpToScanTp->portType,\
			pPortTpToScanTp->ppAlmPreScanNode[0].pAlmPreScanNode->almScanType,\
			pPortTpToScanTp->portTpName,\
			pPortTpToScanTp->ppAlmPreScanNode[0].pAlmPreScanNode->almScanTypeNode->almScanTypeName);	

		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}
	
	printf("slotId sta preScanTypeNum remortNum  scanType     scanTypeName protType(?) preScanNodeNum\n");
	printf("-------------------------------------------------------------------------------------------\n");
	for(i=0;i<(EQU_SLOT_MAX_ID + 1);i++)
	{
		
		pslotAlmProcSpace = pgstAlmPreScanWholwInfo->unitAlmProcSpace + i;
		
		printf("%6d %3d %14d %8d\n",\
		i,\
		pslotAlmProcSpace->slotDevSta,\
		pslotAlmProcSpace->quenNodeNum,\
		pslotAlmProcSpace->RemDevAlmPorcNodeNum);
		
		
		sysScanTypeNUm = 0;
		pAlmPreScanTypeNode = (stAlmPreScanQuen *)listFirst(&(pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].almPreScanTpQuen));
		while((pAlmPreScanTypeNode != NULL)&&(sysScanTypeNUm < pgstAlmPreScanWholwInfo->unitAlmProcSpace[i].quenNodeNum))
		{
			portType = 0xeeeeeeed;
			sysPortTypeNum = 0;
			pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pPTpVsSTpRelation->PTpVsSTpQuen));
			while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pPTpVsSTpRelation->sysSupPortTpNum))
			{
				if(pAlmPreScanTypeNode->almScanType == \
					pPortTpToScanTp->ppAlmPreScanNode[0].pAlmPreScanNode->almScanType)
				{
					portType = pPortTpToScanTp->portType;
					break;
				}
				sysPortTypeNum++;
				pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
			}
		
			almScanPortNum = 0;
			for(j=0;j<GPN_ALM_PRESCAN_HASH;j++)
			{
				almScanPortNum += pAlmPreScanTypeNode->preScanNodeNum[j];
			}
			printf("-----------------------------------  %08x %16.16s %08x    %16d\n",\
				pAlmPreScanTypeNode->almScanType,\
				pAlmPreScanTypeNode->almScanTypeNode->almScanTypeName,\
				portType,almScanPortNum);

			sysScanTypeNUm++;
			pAlmPreScanTypeNode = (stAlmPreScanQuen *)listNext((NODE *)(pAlmPreScanTypeNode));
		}
	}
	
	
	printf("slotId remotId devIndex godDevIndex godPortIndex\n");
	printf("------------------------------------------------\n");
	for(i=0;i<(EQU_SLOT_MAX_ID + 1);i++)
	{
		pslotAlmProcSpace = pgstAlmPreScanWholwInfo->unitAlmProcSpace + i;
		
		remEevNum = 0;
		pRemDevAlmProcNode = (stRemDevAlmProcNode *)listFirst(&(pslotAlmProcSpace->RemDevAlmPorcNodeQuen));
		while((pRemDevAlmProcNode != NULL)&&(remEevNum < pslotAlmProcSpace->RemDevAlmPorcNodeNum))
		{
			printf("%-6d %-7d %08x    %08x     %08x\n",
				i,\
				remEevNum+1,\
				pRemDevAlmProcNode->devIndex,\
				pRemDevAlmProcNode->godDevIndex,\
				pRemDevAlmProcNode->godPortIndex.portIndex);

			remEevNum++;
			pRemDevAlmProcNode = (stRemDevAlmProcNode *)listNext((NODE *)(pRemDevAlmProcNode));
		}
	}
	return GPN_ALM_GEN_YES;
}
UINT32 debugGPNAlmScanTypeInfo(void)
{
	stAlmScanTypeDef *pAlmScanType;
	UINT32 scanTpNum;

	printf(".Num ScanType usePag   subTpInS tpInScan portInSc subInRes scanName\n\r");
	printf("scanTpMask                           scanTpScreen\n\r");
	printf("filtInfo                             reportInfo\n\r");
	printf("recordInfo\n\r");
	scanTpNum = 0;
	pAlmScanType = (stAlmScanTypeDef *)listFirst(&(pgstAlmTypeWholwInfo->almScanTypeQuen));
	while((pAlmScanType != NULL)&&(scanTpNum < pgstAlmTypeWholwInfo->almScanTypeNum))
	{
		printf("%04d %08x %08d %08d %08d %08d %08d %s\n\r",\
			scanTpNum,pAlmScanType->almScanType,pAlmScanType->almUsePag,\
			pAlmScanType->almSubTpNumInScanType,pAlmScanType->almTpNumInScanType,\
			pAlmScanType->almScanQuenPortNum,pAlmScanType->almSubTpNumInRestType,\
			pAlmScanType->almScanTypeName);
		printf("%08x %08x %08x %08x  %08x %08x %08x %08x\n\r",\
			pAlmScanType->scanTypeAlmMask[3],pAlmScanType->scanTypeAlmMask[2],\
			pAlmScanType->scanTypeAlmMask[1],pAlmScanType->scanTypeAlmMask[0],\
			pAlmScanType->almScreInfo[3],pAlmScanType->almScreInfo[2],\
			pAlmScanType->almScreInfo[1],pAlmScanType->almScreInfo[0]);
		printf("%08x %08x %08x %08x  %08x %08x %08x %08x\n\r",\
			pAlmScanType->almFiltInfo[3],pAlmScanType->almFiltInfo[2],\
			pAlmScanType->almFiltInfo[1],pAlmScanType->almFiltInfo[0],\
			pAlmScanType->almReptInfo[3],pAlmScanType->almReptInfo[2],\
			pAlmScanType->almReptInfo[1],pAlmScanType->almReptInfo[0]);
		printf("%08x %08x %08x %08x\n\r",\
			pAlmScanType->almReckInfo[3],pAlmScanType->almReckInfo[2],\
			pAlmScanType->almReckInfo[1],pAlmScanType->almReckInfo[0]);
		scanTpNum++;
		pAlmScanType = (stAlmScanTypeDef *)listNext((NODE *)(pAlmScanType));
	}
	return GPN_ALM_GEN_YES;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif/* _GPN_ALM_SCAN_C_ */

