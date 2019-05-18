/**********************************************************
* file name: gpnAlmDebug.c
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2015-05-22
* function: 
*    define alarm Time proc Api
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_DEBUG_C_
#define _GPN_ALM_DEBUG_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <time.h>
#include <sys/time.h>

#include "gpnAlmScan.h"
#include "gpnAlmTimeProc.h"
#include "gpnAlmDebug.h"
#include "gpnAlmDataSheet.h"

#include "socketComm/gpnSockAlmMsgDef.h"
#include "socketComm/gpnSockCommFuncApi.h"

#include "socketComm/gpnTimerService.h"

#include "socketComm/gpnSockProtocolMsgDef.h"
#include "socketComm/gpnSockCommRoleMan.h"


extern sockCommData gSockCommData;
extern gstAlmTypeWholwInfo *pgstAlmTypeWholwInfo;
extern stEQUAlmProcSpace *pgstAlmPreScanWholwInfo;

/* gpn_alarm timer test alarm report */
#ifdef GPN_ALM_INSERT_DEBUG
void gpnAlmNotifyStateDebug(void)
{
	/* event rise debug */
	gpnAlmEventRiseDebug();
	debugGPNAlmRAMDBListPrint(GPN_ALM_EVENT_DSHEET);
	
	/* alarm product to clear debug */
	gpnAlmProduct2ClearDebug();
	debugGPNAlmRAMDBListPrint(GPN_ALM_HIST_ALM_DSHEET);

	return;
}

void gpnAlmEventRiseDebug(void)
{
	UINT32 sysPortTypeNum;
	UINT32 genEvtSubTypeNum;
	UINT32 devIndex;
	objLogicDesc localPort;
	optObjOrient portInfo;
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stPTpVsAlmSTpRelation *pPTpVsSTpRelation;
	stEvtSubTpNode *pEvtSTNode;
	stAlmScanTypeDef *pAlmScanType;

	/* make a fake device index */
	devIndex = DeviceIndex_Create(1);

	/* add this fake device index */
	gpnSockIfmMsgTxDevStateNotify2Alm(\
		devIndex, GPN_SOCK_MSG_DEV_STA_INSERT);
	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block), process test device insert */
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	/* sys support port type relation to scan type */
	sysPortTypeNum = 0;
	pPTpVsSTpRelation = &(pgstAlmPreScanWholwInfo->PTpVsSTpRelation);
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pPTpVsSTpRelation->PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pPTpVsSTpRelation->sysSupPortTpNum))
	{
		/* find scan type */
		pAlmScanType = pPortTpToScanTp->ppAlmPreScanNode[0].pAlmPreScanNode->almScanTypeNode;

		GPN_ALM_DEBUG_PRINT(GPN_ALM_DEBUG_UD0, "%s : test portType(%d) vs scanType(%08x %s)\n\r",\
			__FUNCTION__, pPortTpToScanTp->portType,\
			pAlmScanType->almScanType, pAlmScanType->almScanTypeName);
		

		/* make a fake port index */
		if(gpnAlmMakeFakePortIndex(pPortTpToScanTp->portType, devIndex, &localPort) != GPN_ALM_DEBUG_OK)
		{
			GPN_ALM_DEBUG_PRINT(GPN_ALM_DEBUG_UD0, "%s : portType(%d %s) can't make fake portIndex\n\r",\
				__FUNCTION__, pPortTpToScanTp->portType, pPortTpToScanTp->portTpName);
			continue;
		}
		GPN_ALM_DEBUG_PRINT(GPN_ALM_DEBUG_UD0, "%s : creat fake port(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
			__FUNCTION__, localPort.devIndex,\
			localPort.portIndex, localPort.portIndex3,\
			localPort.portIndex4, localPort.portIndex5,\
			localPort.portIndex6);
		
		/* add this fake port to valid scan list use mode vari */
		gpnSockIfmMsgTxVariMultPortCreatNotify2Alm(\
			&localPort, GPN_SOCK_MSG_OPT_CREAT);
		sleep(1);
		sleep(1);
		/*gpn_sock process, select opt (no block), process vari port creat */
		gpnSockCommApiNoBlockProc();
		//snmp_subagent_tick(0);

		/* use peer port replace local port */
		memcpy(&portInfo, &localPort, sizeof(optObjOrient));

		/* find sub type of scan type , then notify rise and clean */
		genEvtSubTypeNum = 0;
		pEvtSTNode = (stEvtSubTpNode *)listFirst(&(pAlmScanType->genEvtScanSubTCharaQuen));
		while((pEvtSTNode != NULL)&&(genEvtSubTypeNum < pAlmScanType->genEvtSubTpNumInScanType))
		{
			while(1)
			{
				/* report event */
				gpnSockAlmMsgTxEventNotifyBaseSubType(&portInfo,\
					pEvtSTNode->pEvtSubTpStr->evtSubType, 0);
				GPN_ALM_DEBUG_PRINT(GPN_ALM_DEBUG_UD0, "%s : notify event %08x to port(%08x-%08x|%08x|%08x|%08x|%08x)\n\r",\
					__FUNCTION__,pEvtSTNode->pEvtSubTpStr->evtSubType,\
					portInfo.devIndex, portInfo.portIndex,\
  					portInfo.portIndex3, portInfo.portIndex4,\
  					portInfo.portIndex5, portInfo.portIndex6);

				sleep(1);
				/*gpn_sock process, select opt (no block)*/
				gpnSockCommApiNoBlockProc();
				//snmp_subagent_tick(0);

				sleep(1);
				/*gpn_sock process, select opt (no block)*/
				gpnSockCommApiNoBlockProc();
				//snmp_subagent_tick(0);

				sleep(1);
				/*gpn_sock process, select opt (no block)*/
				gpnSockCommApiNoBlockProc();
				//snmp_subagent_tick(0);

				sleep(1);
				/*gpn_sock process, select opt (no block)*/
				gpnSockCommApiNoBlockProc();
				///snmp_subagent_tick(0);
					
				break;
			}

			sleep(1);	
			/*gpn_sock process, select opt (no block)*/
			gpnSockCommApiNoBlockProc();
			//snmp_subagent_tick(0);
			
			/* go for next sub type */
			genEvtSubTypeNum++;
			pEvtSTNode = (stEvtSubTpNode *)listNext((NODE *)(pEvtSTNode));
		}

		/* del this fake port */
		gpnSockIfmMsgTxVariMultPortCreatNotify2Alm(\
			&localPort, GPN_SOCK_MSG_OPT_DELETE);
		sleep(1);
		sleep(1);
		/*gpn_sock process, select opt (no block), process vari port delete */
		gpnSockCommApiNoBlockProc();
		//snmp_subagent_tick(0);

		/* go for next port type */
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}

	/* del this fake device index */
	gpnSockIfmMsgTxDevStateNotify2Alm(\
		devIndex, GPN_SOCK_MSG_DEV_STA_PULL);
	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block), process test device delete */
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	return;
}

void gpnAlmProduct2ClearDebug(void)
{
	UINT32 prodDelay;
	UINT32 disapDelay;
	UINT32 sysPortTypeNum;
	UINT32 genAlmSubTypeNum;
	UINT32 devIndex;
	objLogicDesc localPort;
	optObjOrient portInfo;
	stPortTpToAlmScanTp *pPortTpToScanTp;
	stPTpVsAlmSTpRelation *pPTpVsSTpRelation;
	stAlmSubTpNode *pAlmSTNode;
	stAlmScanTypeDef *pAlmScanType;
	
	/* save prodDelay and disapDelay , temp set to 0 */
	prodDelay = pgstAlmPreScanWholwInfo->almGlobalCfg.prodDelay;
	disapDelay = pgstAlmPreScanWholwInfo->almGlobalCfg.disapDelay;
	pgstAlmPreScanWholwInfo->almGlobalCfg.prodDelay = 0;
	pgstAlmPreScanWholwInfo->almGlobalCfg.disapDelay = 0;

	/* make a fake device index */
	devIndex = DeviceIndex_Create(1);

	/* add this fake device index */
	gpnSockIfmMsgTxDevStateNotify2Alm(\
		devIndex, GPN_SOCK_MSG_DEV_STA_INSERT);
	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block), process test device insert */
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	/* sys support port type relation to scan type */
	sysPortTypeNum = 0;
	pPTpVsSTpRelation = &(pgstAlmPreScanWholwInfo->PTpVsSTpRelation);
	pPortTpToScanTp = (stPortTpToAlmScanTp *)listFirst(&(pPTpVsSTpRelation->PTpVsSTpQuen));
	while((pPortTpToScanTp != NULL)&&(sysPortTypeNum < pPTpVsSTpRelation->sysSupPortTpNum))
	{
		/* find scan type */
		pAlmScanType = pPortTpToScanTp->ppAlmPreScanNode[0].pAlmPreScanNode->almScanTypeNode;

		GPN_ALM_DEBUG_PRINT(GPN_ALM_DEBUG_UD0, "%s : test portType(%d) vs scanType(%08x %s)\n\r",\
			__FUNCTION__, pPortTpToScanTp->portType,\
			pAlmScanType->almScanType, pAlmScanType->almScanTypeName);

		/* make a fake port index */
		if(gpnAlmMakeFakePortIndex(pPortTpToScanTp->portType, devIndex, &localPort) != GPN_ALM_DEBUG_OK)
		{
			GPN_ALM_DEBUG_PRINT(GPN_ALM_DEBUG_UD0, "%s : portType(%0d %s) can't make fake portIndex\n\r",\
				__FUNCTION__, pPortTpToScanTp->portType, pPortTpToScanTp->portTpName);
			continue;
		}
		
		/* add this fake port to valid scan list use mode vari */
		gpnSockIfmMsgTxVariMultPortCreatNotify2Alm(\
			&localPort, GPN_SOCK_MSG_OPT_CREAT);
		sleep(1);
		sleep(1);
		/*gpn_sock process, select opt (no block), process test device insert */
		gpnSockCommApiNoBlockProc();
		//snmp_subagent_tick(0);

		/* use peer port replace local port */
		memcpy(&portInfo, &localPort, sizeof(optObjOrient));
		
		/* find sub type of scan type , then notify rise and clean */
		genAlmSubTypeNum = 0;
		pAlmSTNode = (stAlmSubTpNode *)listFirst(&(pAlmScanType->genAlmScanSubTCharaQuen));
		while((pAlmSTNode != NULL)&&(genAlmSubTypeNum < pAlmScanType->genAlmSubTpNumInScanType))
		{
			while(1)
			{
				/* rise alarm */
				gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType(&portInfo,\
					pAlmSTNode->pAlmSubTpStr->almSubType, GPN_SOCK_MSG_OPT_RISE);

				sleep(1);
				sleep(1);
				/*gpn_sock process, select opt (no block)*/
				gpnSockCommApiNoBlockProc();
				//snmp_subagent_tick(0);

				sleep(1);
				sleep(1);
				/*gpn_sock process, select opt (no block)*/
				gpnSockCommApiNoBlockProc();
				//snmp_subagent_tick(0);

				sleep(1);
				sleep(1);
				/*gpn_sock process, select opt (no block)*/
				gpnSockCommApiNoBlockProc();
				//snmp_subagent_tick(0);

				sleep(1);
				sleep(1);
				/*gpn_sock process, select opt (no block)*/
				gpnSockCommApiNoBlockProc();
				//snmp_subagent_tick(0);
					
				break;
			}

			sleep(1);
			sleep(1);	
			/*gpn_sock process, select opt (no block)*/
			gpnSockCommApiNoBlockProc();
			//snmp_subagent_tick(0);
			
			while(1)
			{
				/* clear alarm */
				gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType(&portInfo,\
					pAlmSTNode->pAlmSubTpStr->almSubType, GPN_SOCK_MSG_OPT_CLEAN);
				
				sleep(1);
				/*gpn_sock process, select opt (no block)*/
				gpnSockCommApiNoBlockProc();
				//snmp_subagent_tick(0);

				sleep(1);
				/*gpn_sock process, select opt (no block)*/
				gpnSockCommApiNoBlockProc();
				//snmp_subagent_tick(0);

				sleep(1);
				/*gpn_sock process, select opt (no block)*/
				gpnSockCommApiNoBlockProc();
				//snmp_subagent_tick(0);

				sleep(1);
				/*gpn_sock process, select opt (no block)*/
				gpnSockCommApiNoBlockProc();
				//snmp_subagent_tick(0);
					
				break;
			}

			sleep(1);
			sleep(1);
			/*gpn_sock process, select opt (no block)*/
			gpnSockCommApiNoBlockProc();
			//snmp_subagent_tick(0);

			/* go for next sub type */
			genAlmSubTypeNum++;
			pAlmSTNode = (stAlmSubTpNode *)listNext((NODE *)(pAlmSTNode));
		}

		/* del this fake port */
		gpnSockIfmMsgTxVariMultPortCreatNotify2Alm(\
			&localPort, GPN_SOCK_MSG_OPT_DELETE);
		sleep(1);
		sleep(1);
		/*gpn_sock process, select opt (no block), process test device insert */
		gpnSockCommApiNoBlockProc();
		//snmp_subagent_tick(0);

		/* go for next port type */
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToAlmScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}

	/* del this fake device index */
	gpnSockIfmMsgTxDevStateNotify2Alm(\
		devIndex, GPN_SOCK_MSG_DEV_STA_PULL);
	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block), process test device insert */
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);
	
	/* recover prodDelay and disapDelay */
	pgstAlmPreScanWholwInfo->almGlobalCfg.prodDelay = prodDelay;
	pgstAlmPreScanWholwInfo->almGlobalCfg.disapDelay = disapDelay;
	
	return;
}
UINT32 gpnAlmMakeFakePortIndex(UINT32 portType, UINT32 devIndex, objLogicDesc *plocalPort)
{
	UINT32 slot;
	
	/* assert */
	if(plocalPort == NULL)
	{
		return GPN_ALM_DEBUG_ERR;
	}

	slot = DeviceIndex_GetSlot(devIndex);
	switch(portType)
	{
		case IFM_FUN_ETH_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = FUN_ETH_PortIndex_Create(IFM_FUN_ETH_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_ALL_TYPE:
			return GPN_ALM_DEBUG_ERR;

		case IFM_SOFT_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_SOFT_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_STMN_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SDH_PortIndex_Create(IFM_STMN_TYPE, slot, 1, 0, 0);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_VC4_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SDH_PortIndex_Create(IFM_VC4_TYPE, slot, 1, 1, 0);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_VC3_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SDH_PortIndex_Create(IFM_VC3_TYPE, slot, 1, 1, 0);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_VC12_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SDH_PortIndex_Create(IFM_VC12_TYPE, slot, 1, 1, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_VCG_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = VCG_PortIndex_Create(IFM_VCG_TYPE, slot, 1, 0);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_VCG_LP_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = VCG_PortIndex_Create(IFM_VCG_TYPE, slot, 1, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_E1_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = PDH_PortIndex_Create(IFM_E1_TYPE, slot, 1, 0);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_E2_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = PDH_PortIndex_Create(IFM_E2_TYPE, slot, 1, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_E3_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = PDH_PortIndex_Create(IFM_E2_TYPE, slot, 1, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_V35_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_V35_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_DSL_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_DSL_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_ETH_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_ETH_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_VS_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = LAB_PortIndex_Create(IFM_VS_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_LSP_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = LAB_PortIndex_Create(IFM_LSP_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_PW_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = LAB_PortIndex_Create(IFM_PW_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_MPLS_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = LAB_PortIndex_Create(IFM_MPLS_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_VLAN_OAM_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, slot, 1, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_POWN48_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_POWN48_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_EQU_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_EQU_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_ENV_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_ENV_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_FAN_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_FAN_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_PWE3E1_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_PWE3E1_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_CFM_MEP_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_POW220_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_POW220_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_PWE3STMN_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_PWE3STMN_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_ETH_CLC_IN_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_ETH_CLC_IN_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_ETH_CLC_OUT_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_ETH_CLC_OUT_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_ETH_CLC_SUB_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_ETH_CLC_SUB_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_ETH_CLC_LINE_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_ETH_CLC_LINE_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;

		case IFM_ETH_SFP_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_ETH_SFP_TYPE, slot, 1);
			plocalPort->portIndex3 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;

		case IFM_VPLSPW_TYPE:
			plocalPort->devIndex = devIndex;
			plocalPort->portIndex = SMP_PortIndex_Create(IFM_VPLSPW_TYPE, slot, 1);
			plocalPort->portIndex3 = SMP_PortIndex_Create(IFM_VPLSPW_TYPE, slot, 1);
			plocalPort->portIndex4 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex5 = GPN_ILLEGAL_PORT_INDEX;
			plocalPort->portIndex6 = GPN_ILLEGAL_PORT_INDEX;
			break;
			
		case IFM_NULL_TYPE:
			return GPN_ALM_DEBUG_ERR;
			
		default:
			return GPN_ALM_DEBUG_ERR;
	}
	return GPN_ALM_DEBUG_OK;
}

void gpnAlmDebugTickProduct(UINT32 para1, UINT32 para2)
{
	UINT32 *pTick;

	pTick = (UINT32 *)para1;
	if(pTick != NULL)
	{
		*pTick = 1;
	}
	return;
}

void gpnAlmPeerPortReplaceNotifyDebug(void)
{
	UINT32 devIndex;
	objLogicDesc localPort;
	optObjOrient portInfo;
	optObjOrient replPeer;
	
	/* make a fake device index */
	devIndex = DeviceIndex_Create(1);

	/* add this fake device index */
	gpnSockIfmMsgTxDevStateNotify2Alm(\
		devIndex, GPN_SOCK_MSG_DEV_STA_INSERT);
	
	localPort.devIndex = devIndex;
	localPort.portIndex = FUN_ETH_PortIndex_Create(IFM_FUN_ETH_TYPE, DeviceIndex_GetSlot(devIndex), 1);
	localPort.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	localPort.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	localPort.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	localPort.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
	
	/* add this fake port to valid scan list use mode vari */
	gpnSockIfmMsgTxVariMultPortCreatNotify2Alm(\
		&localPort, GPN_SOCK_MSG_OPT_CREAT);

	/* use peer port replace local port */
	memcpy(&portInfo, &localPort, sizeof(optObjOrient));

	/* rise alarm */
	gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType(&portInfo,\
		GPN_ALM_TYPE_FUN_ETH_LOS, GPN_SOCK_MSG_OPT_RISE);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);
	
	/*peer port replace */
	replPeer.devIndex = devIndex;
	replPeer.portIndex = FUN_ETH_PortIndex_Create(IFM_FUN_ETH_TYPE, DeviceIndex_GetSlot(devIndex), 2);
	replPeer.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	replPeer.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	replPeer.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	replPeer.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
	gpnSockIfmMsgTxPeerPortReplaceNotify2Alm(&portInfo, &replPeer);
	
	sleep(1);
	sleep(1);
	sleep(1);
	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);
	
	/* clear alarm */
	gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType(&replPeer,\
		GPN_ALM_TYPE_FUN_ETH_LOS, GPN_SOCK_MSG_OPT_CLEAN);
	
	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	/* show result */
	debugGPNAlmRAMDBListPrint(GPN_ALM_HIST_ALM_DSHEET);
	return;
}

void gpnAlmSFPViewPortReplaceNotifyDebug(void)
{
	UINT32 devIndex;
	objLogicDesc localPort;
	objLogicDesc viewPort;
	optObjOrient portInfo;
	
	/* make a fake device index */
	devIndex = DeviceIndex_Create(1);

	/* add this fake device index */
	gpnSockIfmMsgTxDevStateNotify2Alm(\
		devIndex, GPN_SOCK_MSG_DEV_STA_INSERT);
	
	localPort.devIndex = devIndex;
	localPort.portIndex = SMP_PortIndex_Create(IFM_ETH_SFP_TYPE, DeviceIndex_GetSlot(devIndex), 3);
	localPort.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	localPort.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	localPort.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	localPort.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
	
	/* add this fake port to valid scan list use mode vari */
	gpnSockIfmMsgTxVariMultPortCreatNotify2Alm(\
		&localPort, GPN_SOCK_MSG_OPT_CREAT);

	/*view port replace */
	viewPort.devIndex = devIndex;
	viewPort.portIndex = FUN_ETH_PortIndex_Create(IFM_FUN_ETH_TYPE, DeviceIndex_GetSlot(devIndex), 100);
	viewPort.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	viewPort.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	viewPort.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	viewPort.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
	gpnSockIfmMsgTxViewPortReplaceNotify2Alm(&localPort, &viewPort);

	/* use peer port replace local port */
	memcpy(&portInfo, &localPort, sizeof(optObjOrient));

	/* rise alarm : ETH SFP MIS */
	gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType(&portInfo,\
		GPN_ALM_TYPE_ETH_SFP_MIS, GPN_SOCK_MSG_OPT_RISE);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);
	
	/*view port replace */
	viewPort.devIndex = devIndex;
	viewPort.portIndex = FUN_ETH_PortIndex_Create(IFM_FUN_ETH_TYPE, DeviceIndex_GetSlot(devIndex), 100);
	viewPort.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	viewPort.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	viewPort.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	viewPort.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
	gpnSockIfmMsgTxViewPortReplaceNotify2Alm(&localPort, &viewPort);
	
	sleep(1);
	sleep(1);
	sleep(1);
	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	debugGPNAlmRAMDBListPrint(GPN_ALM_CURR_ALM_DSHEET);
	
	/* clear alarm : ETH SFP MIS */
	gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType(&portInfo,\
		GPN_ALM_TYPE_ETH_SFP_MIS, GPN_SOCK_MSG_OPT_CLEAN);
	
	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	sleep(1);
	sleep(1);
	/*gpn_sock process, select opt (no block)*/
	gpnSockCommApiNoBlockProc();
	//snmp_subagent_tick(0);

	/* show result */
	debugGPNAlmRAMDBListPrint(GPN_ALM_CURR_ALM_DSHEET);
	debugGPNAlmRAMDBListPrint(GPN_ALM_HIST_ALM_DSHEET);
	return;
}

#endif

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_DEBUG_C_*/
