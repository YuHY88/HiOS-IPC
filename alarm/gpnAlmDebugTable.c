/**********************************************************
* file name: gpnAlmDebugTable.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-19
* function: 
*    define alarm table get/getNext/set debug
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_DEBUG_TABLE_C_
#define _GPN_ALM_DEBUG_TABLE_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef GPN_ALM_INSERT_DEBUG

#endif /* GPN_ALM_INSERT_DEBUG */
#if 1 /* GPN_ALM_INSERT_DEBUG */

#include "socketComm/gpnGlobalPortIndexDef.h"

#include "gpnAlmDebugTable.h"
#include "gpnAlmPortNotify.h"
#include "gpnAlmAlmNotify.h"
#include "gpnAlmDataStructApi.h"
#include "gpnAlmMsgProc.h"
#include "gpnAlmDataSheet.h"

UINT32 debugGpnAlmDevSelfNotify(void)
{
	UINT32 devIndex;
	UINT32 reVal;

	//devIndex = DeviceIndex_Create( 1, 0 );
	devIndex = DeviceIndex_Create( 1 );
	reVal = gpnAlmPNtDevStaNotify(devIndex, NULL, GPN_SOCK_MSG_DEV_STA_INSERT);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : register dev(%08x) error\n\r",\
			__FUNCTION__, devIndex);
		
		return GPN_ALM_DEBUG_ERR;
	}

	debugGpnAlmPNtDevPrint();

	reVal = gpnAlmPNtDevIsInsert(devIndex);
	if(reVal == GPN_ALM_DEBUG_OK)
	{
		printf("%s : dev(%08x) is insert\n\r",\
			__FUNCTION__, devIndex);
	}
	
	return GPN_ALM_DEBUG_OK;
}

UINT32 debugGpnAlmFixPortSelfNotify(void)
{
	UINT32 portTpNum;
	UINT32 devIndex;
	UINT32 reVal;

	stAlmPortObjCL portObjCL[] =\
	{
		{IFM_SOFT_TYPE,		2},
		{IFM_ENV_TYPE,		2}
	};

	//devIndex = DeviceIndex_Create( 1, 0 );
	devIndex = DeviceIndex_Create( 1 );
	portTpNum = sizeof(portObjCL)/sizeof(stAlmPortObjCL);
	reVal = gpnAlmPNtFixPortUserNotify(devIndex, portTpNum, portObjCL);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : fix port register error\n\r",\
			__FUNCTION__);
		
		return GPN_ALM_DEBUG_ERR;
	}

	debugGpnAlmPNtFixPreScanPrint();

	return GPN_ALM_DEBUG_OK;
}

UINT32 debugGpnAlmVariPorNotify(void)
{
	objLogicDesc portInfo;
	UINT32 reVal;

	/* creat vari port 1 */
	//portInfo.devIndex = DeviceIndex_Create( 1, 0 );
	portInfo.devIndex = DeviceIndex_Create( 1 );
	portInfo.portIndex = IFM_SMP_PID_COMP(IFM_E1_TYPE, 0, 2);
	portInfo.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
	reVal = gpnAlmPNtVariPortReg(&portInfo);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : vari port(%08x-%08x|%08x|%08x|%08x|%08x) register error\n\r",\
			__FUNCTION__,\
			portInfo.devIndex, portInfo.portIndex,\
			portInfo.portIndex3, portInfo.portIndex4,\
			portInfo.portIndex5, portInfo.portIndex6);

		return GPN_ALM_DEBUG_ERR;
	}

	/* creat vari port 2 */
	//portInfo.devIndex = DeviceIndex_Create( 1, 0 );
	portInfo.devIndex = DeviceIndex_Create( 1 );
	portInfo.portIndex = IFM_FUN_ETH_PID_COMP(IFM_FUN_ETH_TYPE, 0, 5001);
	portInfo.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
	reVal = gpnAlmPNtVariPortReg(&portInfo);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : vari port(%08x-%08x|%08x|%08x|%08x|%08x) register error\n\r",\
			__FUNCTION__,\
			portInfo.devIndex, portInfo.portIndex,\
			portInfo.portIndex3, portInfo.portIndex4,\
			portInfo.portIndex5, portInfo.portIndex6);

		return GPN_ALM_DEBUG_ERR;
	}

	/* creat vari port 3 */
	//portInfo.devIndex = DeviceIndex_Create( 1, 0 );
	portInfo.devIndex = DeviceIndex_Create( 1 );
	portInfo.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	portInfo.portIndex3 = 16;
	portInfo.portIndex4 = 32;
	portInfo.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	portInfo.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	reVal = gpnAlmPNtVariPortReg(&portInfo);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : vari port(%08x-%08x|%08x|%08x|%08x|%08x) register error\n\r",\
			__FUNCTION__,\
			portInfo.devIndex, portInfo.portIndex,\
			portInfo.portIndex3, portInfo.portIndex4,\
			portInfo.portIndex5, portInfo.portIndex6);

		return GPN_ALM_DEBUG_ERR;
	}
	
	/* creat vari port 2 */
	//portInfo.devIndex = DeviceIndex_Create( 1, 0 );
	portInfo.devIndex = DeviceIndex_Create( 1 );
	portInfo.portIndex = IFM_FUN_ETH_PID_COMP(IFM_FUN_ETH_TYPE, 0, 5001);
	portInfo.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
	reVal = gpnAlmPNtVariPortUnReg(&portInfo);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : vari port(%08x-%08x|%08x|%08x|%08x|%08x) unReg error\n\r",\
			__FUNCTION__,\
			portInfo.devIndex, portInfo.portIndex,\
			portInfo.portIndex3, portInfo.portIndex4,\
			portInfo.portIndex5, portInfo.portIndex6);

		return GPN_ALM_DEBUG_ERR;
	}

	debugGpnAlmPNtVariPreScanPrint();
	
	return GPN_ALM_DEBUG_OK;
}

UINT32 debugGpnAlmPortMonTableGet(void)
{
	objLogicDesc portInfo;
	UINT32 reVal;
	UINT32 sta;
	
	/* creat vari port */
	//portInfo.devIndex = DeviceIndex_Create( 1, 0 );
	portInfo.devIndex = DeviceIndex_Create( 1 );
	portInfo.portIndex = IFM_SMP_PID_COMP(IFM_E1_TYPE, 0, 2);
	portInfo.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex6 = GPN_ILLEGAL_PORT_INDEX;
	
	reVal = gpnAlmDSApiPortMonStateGet(&portInfo, &sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : get port(%08x-%08x|%08x|%08x|%08x|%08x) error\n\r",\
			__FUNCTION__,\
			portInfo.devIndex, portInfo.portIndex,\
			portInfo.portIndex3, portInfo.portIndex4,\
			portInfo.portIndex5, portInfo.portIndex6);

		return GPN_ALM_DEBUG_ERR;
	}

	printf("GET portMonTabe(%08x-%08x|%08x|%08x|%08x|%08x) sta(%d)\n\r",\
		portInfo.devIndex, portInfo.portIndex,\
		portInfo.portIndex3, portInfo.portIndex4,\
		portInfo.portIndex5, portInfo.portIndex6,\
		sta);

	return GPN_ALM_DEBUG_OK;
}

UINT32 debugGpnAlmPortMonTableGetNext(void)
{
	objLogicDesc portInfo;
	objLogicDesc nextPort;
	UINT32 reVal;
	UINT32 num;
	UINT32 sta;

	/* creat NULL port */
	num = 1;
	sta = 0;
	portInfo.devIndex = GPN_ILLEGAL_DEVICE_INDEX;
	portInfo.portIndex = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	do
	{
		reVal = gpnAlmDSApiPortMonStateGetNext(&portInfo, &nextPort, &sta);
		if(reVal != GPN_ALM_DEBUG_OK)
		{
			printf("GET-NEXT(%d) portMonTabe(%08x-%08x|%08x|%08x|%08x|%08x)'s Next error\n\r",\
				num++,\
				portInfo.devIndex, portInfo.portIndex,\
				portInfo.portIndex3, portInfo.portIndex4,\
				portInfo.portIndex5, portInfo.portIndex6);

			return GPN_ALM_DEBUG_ERR;
		}

		printf("GET-NEXT(%d) portMonTabe(%08x-%08x|%08x|%08x|%08x|%08x) sta(%d)\n\r",\
			num++,\
			nextPort.devIndex, nextPort.portIndex,\
			nextPort.portIndex3, nextPort.portIndex4,\
			nextPort.portIndex5, nextPort.portIndex6,\
			sta);

		/* copy nextPort to portInfo */
		memcpy(&portInfo, &nextPort, sizeof(objLogicDesc));
	}
	while(portInfo.portIndex != GPN_ILLEGAL_PORT_INDEX);

	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmAttributeTableGet(void)
{
	objLogicDesc portInfo;
	stDSAlmAttri almAttri;
	UINT32 almType;
	UINT32 reVal;
	
	/* get almType Attribute */
	almType = GPN_ALM_TYPE_LSP_RXB_24HTHR;
	portInfo.devIndex = GPN_ILLEGAL_DEVICE_INDEX;
	portInfo.portIndex = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	reVal = gpnAlmDSApiAttributeTableGet(&portInfo, almType, &almAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : get almType(%08x)'s attribute error\n\r",\
			__FUNCTION__, almType);

		return GPN_ALM_DEBUG_ERR;
	}

	printf("GET portAttriTabe (%08x)(%08x-%08x|%08x|%08x|%08x|%08x) "
		"rank(%d) mask(%d) filt(%d) rept(%d) recd(%d) revs(%d) bit(%08x)\n\r",\
		almType,\
		portInfo.devIndex, portInfo.portIndex,\
		portInfo.portIndex3, portInfo.portIndex4,\
		portInfo.portIndex5, portInfo.portIndex6,\
		almAttri.rank, almAttri.mask, almAttri.filt,\
		almAttri.rept, almAttri.recd, almAttri.revs,\
		almAttri.bitMark);
	
	/* get jportBase almType Attribute */
	almType = GPN_ALM_TYPE_FUN_CFM_LBLOS;
	//portInfo.devIndex = DeviceIndex_Create( 1, 0 );
	portInfo.devIndex = DeviceIndex_Create( 1 );
	portInfo.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	portInfo.portIndex3 = 16;
	portInfo.portIndex4 = 32;
	portInfo.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	portInfo.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	
	reVal = gpnAlmDSApiAttributeTableGet(&portInfo, almType, &almAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : get almType(%08x) "
			"port(%08x-%08x|%08x|%08x|%08x|%08x)'s attribute error\n\r",\
			__FUNCTION__, almType,\
			portInfo.devIndex, portInfo.portIndex,\
			portInfo.portIndex3, portInfo.portIndex4,\
			portInfo.portIndex5, portInfo.portIndex6);

		return GPN_ALM_DEBUG_ERR;
	}

	printf("GET portAttriTabe (%08x)(%08x-%08x|%08x|%08x|%08x|%08x) "
		"rank(%d) mask(%d) filt(%d) rept(%d) recd(%d) revs(%d) bit(%08x)\n\r",\
		almType,\
		portInfo.devIndex, portInfo.portIndex,\
		portInfo.portIndex3, portInfo.portIndex4,\
		portInfo.portIndex5, portInfo.portIndex6,\
		almAttri.rank, almAttri.mask, almAttri.filt,\
		almAttri.rept, almAttri.recd, almAttri.revs,\
		almAttri.bitMark);

	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmAttributeTableGetNext(void)
{
	objLogicDesc portInfo;
	objLogicDesc nextPort;
	stDSAlmAttri almAttri;
	UINT32 almType;
	UINT32 nextAlmType;
	UINT32 reVal;
	UINT32 num;

	/* creat NULL port */
	num = 1;
	almType = GPN_ALM_TYPE_INVALID;
	portInfo.devIndex = GPN_ILLEGAL_DEVICE_INDEX;
	portInfo.portIndex = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portInfo.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	do
	{
		reVal = gpnAlmDSApiAttributeTableGetNext(&portInfo,\
			almType, &nextPort, &nextAlmType, &almAttri);
		if(reVal != GPN_ALM_DEBUG_OK)
		{
			printf("GET-NEXT(%4d) portAttirbuteTabe(%08x-%08x|%08x|%08x|%08x|%08x)'s Next error\n\r",\
				num++,\
				portInfo.devIndex, portInfo.portIndex,\
				portInfo.portIndex3, portInfo.portIndex4,\
				portInfo.portIndex5, portInfo.portIndex6);

			return GPN_ALM_DEBUG_ERR;
		}

		printf("GET-NEXT(%4d) portAttirbuteTabe(%08x)(%08x-%08x|%08x|%08x|%08x|%08x) "
			"rank(%d) mask(%d) filt(%d) rept(%d) recd(%d) revs(%d) bit(%08x)\n\r",\
			num++,nextAlmType,\
			nextPort.devIndex, nextPort.portIndex,\
			nextPort.portIndex3, nextPort.portIndex4,\
			nextPort.portIndex5, nextPort.portIndex6,\
			almAttri.rank, almAttri.mask, almAttri.filt,\
			almAttri.rept, almAttri.recd, almAttri.revs,\
			almAttri.bitMark);

		/* copy nextPort to portInfo */
		almType = nextAlmType;
		memcpy(&portInfo, &nextPort, sizeof(objLogicDesc));
	}
	while(almType != GPN_ALM_TYPE_INVALID);

	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmAlmReport(void)
{
	optObjOrient peerPort;
	UINT32 almType;
	UINT32 reVal;
	UINT32 sta;

	//peerPort.devIndex = DeviceIndex_Create( 1, 0 );
	peerPort.devIndex = DeviceIndex_Create( 1 );
	peerPort.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	peerPort.portIndex3 = 16;
	peerPort.portIndex4 = 32;
	peerPort.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	peerPort.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	almType = GPN_ALM_TYPE_FUN_CFM_LBLOS;
	/* first arise for curr alarm dataSheet */
	sta = GPN_ALM_ARISE;
	
	reVal = gpnAlmANtBaseSubTypeProc(&peerPort, almType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			peerPort.devIndex, peerPort.portIndex,\
			peerPort.portIndex3, peerPort.portIndex4,\
			peerPort.portIndex5, peerPort.portIndex6,\
			almType, sta);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);	
	gpnAlm1SecondTimerPorc(NULL);

	/* first clean for hist alarm dataSheet */
	sta = GPN_ALM_CLEAN;
	
	reVal = gpnAlmANtBaseSubTypeProc(&peerPort, almType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			peerPort.devIndex, peerPort.portIndex,\
			peerPort.portIndex3, peerPort.portIndex4,\
			peerPort.portIndex5, peerPort.portIndex6,\
			almType, sta);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);

	/* second arise for curr alarm dataSheet */
	sta = GPN_ALM_ARISE;
		
	reVal = gpnAlmANtBaseSubTypeProc(&peerPort, almType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			peerPort.devIndex, peerPort.portIndex,\
			peerPort.portIndex3, peerPort.portIndex4,\
			peerPort.portIndex5, peerPort.portIndex6,\
			almType, sta);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);

	/* second clean for curr alarm dataSheet */
	sta = GPN_ALM_CLEAN;
	
	reVal = gpnAlmANtBaseSubTypeProc(&peerPort, almType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			peerPort.devIndex, peerPort.portIndex,\
			peerPort.portIndex3, peerPort.portIndex4,\
			peerPort.portIndex5, peerPort.portIndex6,\
			almType, sta);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);

	/* third arise for curr alarm dataSheet */
	sta = GPN_ALM_ARISE;
		
	reVal = gpnAlmANtBaseSubTypeProc(&peerPort, almType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			peerPort.devIndex, peerPort.portIndex,\
			peerPort.portIndex3, peerPort.portIndex4,\
			peerPort.portIndex5, peerPort.portIndex6,\
			almType, sta);

		return GPN_ALM_DEBUG_ERR;
	}

	//peerPort.devIndex = DeviceIndex_Create( 1, 0 );
	peerPort.devIndex = DeviceIndex_Create( 1 );
	peerPort.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	peerPort.portIndex3 = 16;
	peerPort.portIndex4 = 32;
	peerPort.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	peerPort.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	almType = GPN_ALM_TYPE_FUN_CFM_LCK;
	/* first arise for curr alarm dataSheet */
	sta = GPN_ALM_ARISE;
	
	reVal = gpnAlmANtBaseSubTypeProc(&peerPort, almType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			peerPort.devIndex, peerPort.portIndex,\
			peerPort.portIndex3, peerPort.portIndex4,\
			peerPort.portIndex5, peerPort.portIndex6,\
			almType, sta);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	
	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmCurrAlmTableGet(void)
{
	stDSCurrAlm currAlm;
	UINT32 index;
	UINT32 reVal;

	index = 2;
	reVal = gpnAlmDSApiCurrAlmGet(index, &currAlm);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : index(%3d) get error\n\r",\
			__FUNCTION__, index);
		
		return GPN_ALM_DEBUG_ERR;
	}

	printf("%s : index(%3d) port(%08x-%08x|%08x|%08x|%08x|%08x) "
		"almType(%08x) rank(%d) count(%d) thisTime(%08x)\n\r",\
		__FUNCTION__, currAlm.index,\
		currAlm.devIndex, currAlm.ifIndex,\
		currAlm.ifIndex2, currAlm.ifIndex3,\
		currAlm.ifIndex4, currAlm.ifIndex5,\
		currAlm.almType, currAlm.level,\
		currAlm.count, currAlm.thisTime);
	
	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmCurrAlmTableGetNext(void)
{
	stDSCurrAlm currAlm;
	UINT32 nextIndex;
	UINT32 index;
	UINT32 reVal;

	/* debug start from first */
	index = GPN_ALM_DS_ILLEGAL_INDEX;
	reVal = GPN_ALM_DEBUG_OK;
	while(reVal == GPN_ALM_DEBUG_OK)
	{
		reVal = gpnAlmDSApiCurrAlmGetNext(index, &nextIndex, &currAlm);
		if(reVal != GPN_ALM_DEBUG_OK)
		{
			printf("%s : index(%3d) getNext error\n\r",\
				__FUNCTION__, index);
			
			return GPN_ALM_DEBUG_ERR;
		}

		printf("%s : index(%3d) port(%08x-%08x|%08x|%08x|%08x|%08x) "
			"almType(%08x) rank(%d) count(%d) thisTime(%08x)\n\r",\
			__FUNCTION__, currAlm.index,\
			currAlm.devIndex, currAlm.ifIndex,\
			currAlm.ifIndex2, currAlm.ifIndex3,\
			currAlm.ifIndex4, currAlm.ifIndex5,\
			currAlm.almType, currAlm.level,\
			currAlm.count, currAlm.thisTime);

		/* renew index to nextIndex */
		index = nextIndex;
	}
	
	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmHistAlmTableGet(void)
{
	stDSHistAlm histAlm;
	UINT32 index;
	UINT32 reVal;

	index = 0;
	reVal = gpnAlmDSApiHistAlmGet(index, &histAlm);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : index(%3d) get error\n\r",\
			__FUNCTION__, index);
		
		return GPN_ALM_DEBUG_ERR;
	}

	printf("%s : index(%3d) port(%08x-%08x|%08x|%08x|%08x|%08x) "
		"almType(%08x) count(%d) thisTime(%08x)  dispTime(%08x)\n\r",\
		__FUNCTION__, histAlm.index,\
		histAlm.devIndex, histAlm.ifIndex,\
		histAlm.ifIndex2, histAlm.ifIndex3,\
		histAlm.ifIndex4, histAlm.ifIndex5,\
		histAlm.almType, histAlm.count,\
		histAlm.thisTime, histAlm.disapTime);
	
	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmHistAlmTableGetNext(void)
{
	stDSHistAlm histAlm;
	UINT32 nextIndex;
	UINT32 index;
	UINT32 reVal;

	/* debug start from first */
	index = GPN_ALM_DS_ILLEGAL_INDEX;
	reVal = GPN_ALM_DEBUG_OK;
	while(reVal == GPN_ALM_DEBUG_OK)
	{
		reVal = gpnAlmDSApiHistAlmGetNext(index, &nextIndex, &histAlm);
		if(reVal != GPN_ALM_DEBUG_OK)
		{
			printf("%s : index(%3d) getNext error\n\r",\
				__FUNCTION__, index);
			
			return GPN_ALM_DEBUG_ERR;
		}

		printf("%s : index(%3d) port(%08x-%08x|%08x|%08x|%08x|%08x) "
			"almType(%08x) count(%d) thisTime(%08x) dispTime(%08x)\n\r",\
			__FUNCTION__, histAlm.index,\
			histAlm.devIndex, histAlm.ifIndex,\
			histAlm.ifIndex2, histAlm.ifIndex3,\
			histAlm.ifIndex4, histAlm.ifIndex5,\
			histAlm.almType, histAlm.count,\
			histAlm.thisTime, histAlm.disapTime);

		/* renew index to nextIndex */
		index = nextIndex;
	}
	
	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmEvtReport(void)
{
	optObjOrient peerPort;
	UINT32 evtType;
	UINT32 reVal;
	UINT32 detail;

	//peerPort.devIndex = DeviceIndex_Create( 1, 0 );
	peerPort.devIndex = DeviceIndex_Create( 1 );
	peerPort.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	peerPort.portIndex3 = 16;
	peerPort.portIndex4 = 32;
	peerPort.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	peerPort.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	evtType = GPN_EVT_TYPE_FUN_CFM_REM_LP_TOUT;
	detail = 5;
	
	reVal = gpnAlmANtEvtBaseSubTypeProc(&peerPort, evtType, detail);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) evtType(%08x) detail(%d) error\n\r",\
			__FUNCTION__,\
			peerPort.devIndex, peerPort.portIndex,\
			peerPort.portIndex3, peerPort.portIndex4,\
			peerPort.portIndex5, peerPort.portIndex6,\
			evtType, detail);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);

	//peerPort.devIndex = DeviceIndex_Create( 1, 0 );
	peerPort.devIndex = DeviceIndex_Create( 1 );
	peerPort.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	peerPort.portIndex3 = 16;
	peerPort.portIndex4 = 32;
	peerPort.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	peerPort.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	evtType = GPN_EVT_TYPE_FUN_CFM_REM_LP_TOUT;
	detail = 6;
	
	reVal = gpnAlmANtEvtBaseSubTypeProc(&peerPort, evtType, detail);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) evtType(%08x) detail(%d) error\n\r",\
			__FUNCTION__,\
			peerPort.devIndex, peerPort.portIndex,\
			peerPort.portIndex3, peerPort.portIndex4,\
			peerPort.portIndex5, peerPort.portIndex6,\
			evtType, detail);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	
	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmEventTableGet(void)
{
	stDSEvent event;
	UINT32 index;
	UINT32 reVal;

	index = 1;
	reVal = gpnAlmDSApiEventGet(index, &event);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : index(%3d) get error\n\r",\
			__FUNCTION__, index);
		
		return GPN_ALM_DEBUG_ERR;
	}

	printf("%s : index(%3d) port(%08x-%08x|%08x|%08x|%08x|%08x) "
		"evtType(%08x) level(%d) thisTime(%08x)  detail(%d)\n\r",\
		__FUNCTION__, event.index,\
		event.devIndex, event.ifIndex,\
		event.ifIndex2, event.ifIndex3,\
		event.ifIndex4, event.ifIndex5,\
		event.eventType, event.level,\
		event.thisTime, event.detail);
	
	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmEventTableGetNext(void)
{
	stDSEvent event;
	UINT32 nextIndex;
	UINT32 index;
	UINT32 reVal;

	/* debug start from first */
	index = GPN_ALM_DS_ILLEGAL_INDEX;
	reVal = GPN_ALM_DEBUG_OK;
	while(reVal == GPN_ALM_DEBUG_OK)
	{
		reVal = gpnAlmDSApiEventGetNext(index, &nextIndex, &event);
		if(reVal != GPN_ALM_DEBUG_OK)
		{
			printf("%s : index(%3d) getNext error\n\r",\
				__FUNCTION__, index);
			
			return GPN_ALM_DEBUG_ERR;
		}

		printf("%s : index(%3d) port(%08x-%08x|%08x|%08x|%08x|%08x) "
			"evtType(%08x) level(%d) thisTime(%08x)  detail(%d)\n\r",\
			__FUNCTION__, event.index,\
			event.devIndex, event.ifIndex,\
			event.ifIndex2, event.ifIndex3,\
			event.ifIndex4, event.ifIndex5,\
			event.eventType, event.level,\
			event.thisTime, event.detail);

		/* renew index to nextIndex */
		index = nextIndex;
	}
	
	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmAlmTableCyc(void)
{
	optObjOrient peerPort;
	UINT32 almType;
	UINT32 reVal;
	UINT32 sta;
	UINT32 i;

	//peerPort.devIndex = DeviceIndex_Create( 1, 0 );
	peerPort.devIndex = DeviceIndex_Create( 1 );
	peerPort.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	peerPort.portIndex3 = 16;
	peerPort.portIndex4 = 32;
	peerPort.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	peerPort.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	almType = GPN_ALM_TYPE_FUN_CFM_LBLOS;

	for(i=0; i<12; i++)
	{
		sta = GPN_ALM_ARISE;
		reVal = gpnAlmANtBaseSubTypeProc(&peerPort, almType, sta);
		if(reVal != GPN_ALM_DEBUG_OK)
		{
			printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
				__FUNCTION__,\
				peerPort.devIndex, peerPort.portIndex,\
				peerPort.portIndex3, peerPort.portIndex4,\
				peerPort.portIndex5, peerPort.portIndex6,\
				almType, sta);

			return GPN_ALM_DEBUG_ERR;
		}
		gpnAlm1SecondTimerPorc(NULL);
		gpnAlm1SecondTimerPorc(NULL);
		gpnAlm1SecondTimerPorc(NULL);

		sta = GPN_ALM_CLEAN;
		reVal = gpnAlmANtBaseSubTypeProc(&peerPort, almType, sta);
		if(reVal != GPN_ALM_DEBUG_OK)
		{
			printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
				__FUNCTION__,\
				peerPort.devIndex, peerPort.portIndex,\
				peerPort.portIndex3, peerPort.portIndex4,\
				peerPort.portIndex5, peerPort.portIndex6,\
				almType, sta);

			return GPN_ALM_DEBUG_ERR;
		}
		gpnAlm1SecondTimerPorc(NULL);
		gpnAlm1SecondTimerPorc(NULL);
		gpnAlm1SecondTimerPorc(NULL);
	}

	debugGpnAlmHistAlmTableGetNext();
	gpnAlmDSApiHistAlmDBCycCfg(GPN_ALM_LINE_DSHEET);

	sta = GPN_ALM_ARISE;
	reVal = gpnAlmANtBaseSubTypeProc(&peerPort, almType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			peerPort.devIndex, peerPort.portIndex,\
			peerPort.portIndex3, peerPort.portIndex4,\
			peerPort.portIndex5, peerPort.portIndex6,\
			almType, sta);

		return GPN_ALM_DEBUG_ERR;
	}
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);

	/* last is arist */
	
	debugGpnAlmHistAlmTableGetNext();
	gpnAlmDSApiHistAlmDBCycCfg(GPN_ALM_CYC_DSHEET);
	
	return GPN_ALM_DEBUG_OK;
}

UINT32 debugGpnAlmProductCountClear(void)
{
	debugGpnAlmCurrAlmTableGetNext();
	gpnAlmDSApiCurrAlmProductCountClear(15);
	debugGpnAlmCurrAlmTableGetNext();

	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmAttributeRankCfg(void)
{
	objLogicDesc localPIndex;
	stDSAlmAttri dsAttri;
	UINT32 almSubType;
	UINT32 reVal;

	debugGpnAlmAttributeTableGetNext();

	/* config base almType + port */
	//localPIndex.devIndex = DeviceIndex_Create( 1, 0 );
	localPIndex.devIndex = DeviceIndex_Create( 1 );
	localPIndex.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	localPIndex.portIndex3 = 16;
	localPIndex.portIndex4 = 32;
	localPIndex.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	localPIndex.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	almSubType = GPN_ALM_TYPE_FUN_CFM_LBLOS;
	dsAttri.rank = 4;
	dsAttri.bitMark = ((1U << GPN_ALM_DST_ATTRI_BIT_RANK) << 16) | (1U << GPN_ALM_DST_ATTRI_BIT_RANK);
	reVal = gpnAlmDSApiAlmAttribModify(&localPIndex, almSubType, &dsAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) rank change error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType);

		return GPN_ALM_DEBUG_ERR;
	}

	/* config base almType */
	localPIndex.devIndex = GPN_GEN_MSG_INVALID_DEV;
	localPIndex.portIndex = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex3 = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex4 = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex5 = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex6 = GPN_GEN_MSG_INVALID_PORT;
	almSubType = GPN_ALM_TYPE_FUN_CFM_LBLOS;
	dsAttri.rank = 2;
	dsAttri.bitMark = (1U << GPN_ALM_DST_ATTRI_BIT_RANK);
	reVal = gpnAlmDSApiAlmAttribModify(&localPIndex, almSubType, &dsAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) rank change error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType);

		return GPN_ALM_DEBUG_ERR;
	}

	/* cancel config base almType + port 
	localPIndex.devIndex = DeviceIndex_Create( 1, 0 );
	localPIndex.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	localPIndex.portIndex3 = 16;
	localPIndex.portIndex4 = 32;
	localPIndex.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	localPIndex.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	almSubType = GPN_ALM_TYPE_FUN_CFM_LBLOS;
	dsAttri.rank = 1;
	dsAttri.bitMark = (1U << GPN_ALM_DST_ATTRI_BIT_RANK);
	reVal = gpnAlmDSApiAlmAttribModify(&localPIndex, almSubType, &dsAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) rank change error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType);

		return GPN_ALM_DEBUG_ERR;
	}*/
	debugGpnAlmAttributeTableGetNext();
	debugGpnAlmCurrAlmTableGetNext();

	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmAttributeScreenCfg(void)
{
	objLogicDesc localPIndex;
	stDSAlmAttri dsAttri;
	UINT32 almSubType;
	UINT32 reVal;

	debugGpnAlmAttributeTableGetNext();
	debugGpnAlmCurrAlmTableGetNext();

	/* config base almType */
	localPIndex.devIndex = GPN_GEN_MSG_INVALID_DEV;
	localPIndex.portIndex = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex3 = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex4 = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex5 = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex6 = GPN_GEN_MSG_INVALID_PORT;
	almSubType = GPN_ALM_TYPE_FUN_CFM_LBLOS;
	dsAttri.mask = GPN_ALM_DEBUG_ENABLE;
	dsAttri.bitMark = (1U << GPN_ALM_DST_ATTRI_BIT_MASK);
	reVal = gpnAlmDSApiAlmAttribModify(&localPIndex, almSubType, &dsAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) screen change error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);

	debugGpnAlmCurrAlmTableGetNext();

	/* config base almType + port */
	//localPIndex.devIndex = DeviceIndex_Create( 1, 0 );
	localPIndex.devIndex = DeviceIndex_Create( 1 );
	localPIndex.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	localPIndex.portIndex3 = 16;
	localPIndex.portIndex4 = 32;
	localPIndex.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	localPIndex.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	almSubType = GPN_ALM_TYPE_FUN_CFM_LBLOS;
	dsAttri.mask = GPN_ALM_DEBUG_DISABLE;
	dsAttri.bitMark = ((1U << GPN_ALM_DST_ATTRI_BIT_MASK) << 16) | (1U << GPN_ALM_DST_ATTRI_BIT_MASK);
	reVal = gpnAlmDSApiAlmAttribModify(&localPIndex, almSubType, &dsAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) screen change error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);

	debugGpnAlmAttributeTableGetNext();
	debugGpnAlmCurrAlmTableGetNext();

	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmAttributeFiltCfg(void)
{
	objLogicDesc localPIndex;
	stDSAlmAttri dsAttri;
	UINT32 almSubType;
	UINT32 reVal;

	debugGpnAlmAttributeTableGetNext();
	debugGpnAlmCurrAlmTableGetNext();

	/* config base almType */
	localPIndex.devIndex = GPN_GEN_MSG_INVALID_DEV;
	localPIndex.portIndex = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex3 = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex4 = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex5 = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex6 = GPN_GEN_MSG_INVALID_PORT;
	almSubType = GPN_ALM_TYPE_FUN_CFM_LBLOS;
	dsAttri.filt = GPN_ALM_DEBUG_ENABLE;
	dsAttri.bitMark = (1U << GPN_ALM_DST_ATTRI_BIT_FILT);
	reVal = gpnAlmDSApiAlmAttribModify(&localPIndex, almSubType, &dsAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) filt change error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);

	debugGpnAlmCurrAlmTableGetNext();

	/* config base almType + port */
	//localPIndex.devIndex = DeviceIndex_Create( 1, 0 );
	localPIndex.devIndex = DeviceIndex_Create( 1 );
	localPIndex.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	localPIndex.portIndex3 = 16;
	localPIndex.portIndex4 = 32;
	localPIndex.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	localPIndex.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	almSubType = GPN_ALM_TYPE_FUN_CFM_LBLOS;
	dsAttri.filt = GPN_ALM_DEBUG_DISABLE;
	dsAttri.bitMark = ((1U << GPN_ALM_DST_ATTRI_BIT_FILT) << 16) | (1U << GPN_ALM_DST_ATTRI_BIT_FILT);
	reVal = gpnAlmDSApiAlmAttribModify(&localPIndex, almSubType, &dsAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) filt change error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);

	debugGpnAlmAttributeTableGetNext();
	debugGpnAlmCurrAlmTableGetNext();

	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmAttributeReportCfg(void)
{
	objLogicDesc localPIndex;
	optObjOrient peerPIndex;
	stDSAlmAttri dsAttri;
	UINT32 almSubType;
	UINT32 reVal;
	UINT32 sta;
	
	gpnAlmDSApiRestModeCfg(GPN_ALM_REST_LEVEL_MODE);

	/* config base almType */
	localPIndex.devIndex = GPN_GEN_MSG_INVALID_DEV;
	localPIndex.portIndex = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex3 = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex4 = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex5 = GPN_GEN_MSG_INVALID_PORT;
	localPIndex.portIndex6 = GPN_GEN_MSG_INVALID_PORT;
	almSubType = GPN_ALM_TYPE_FUN_CFM_LOC;
	dsAttri.rept = GPN_ALM_DEBUG_DISABLE;
	dsAttri.bitMark = (1U << GPN_ALM_DST_ATTRI_BIT_REPT);
	reVal = gpnAlmDSApiAlmAttribModify(&localPIndex, almSubType, &dsAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) filt change error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType);

		return GPN_ALM_DEBUG_ERR;
	}

	//peerPIndex.devIndex = DeviceIndex_Create( 1, 0 );
	peerPIndex.devIndex = DeviceIndex_Create( 1 );
	peerPIndex.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	peerPIndex.portIndex3 = 16;
	peerPIndex.portIndex4 = 32;
	peerPIndex.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	peerPIndex.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	sta = GPN_ALM_ARISE;
	almSubType = GPN_ALM_TYPE_FUN_CFM_LOC;
	reVal = gpnAlmANtBaseSubTypeProc(&peerPIndex, almSubType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType, sta);

		return GPN_ALM_DEBUG_ERR;
	}
	sta = GPN_ALM_ARISE;
	almSubType = GPN_ALM_TYPE_FUN_CFM_AIS;
	reVal = gpnAlmANtBaseSubTypeProc(&peerPIndex, almSubType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType, sta);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);

	debugGpnAlmCurrAlmTableGetNext();

	/* config base almType + port */
	//localPIndex.devIndex = DeviceIndex_Create( 1, 0 );
	localPIndex.devIndex = DeviceIndex_Create( 1 );
	localPIndex.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	localPIndex.portIndex3 = 16;
	localPIndex.portIndex4 = 32;
	localPIndex.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	localPIndex.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	almSubType = GPN_ALM_TYPE_FUN_CFM_LOC;
	dsAttri.rept = GPN_ALM_DEBUG_ENABLE;
	dsAttri.bitMark = ((1U << GPN_ALM_DST_ATTRI_BIT_REPT) << 16) | (1U << GPN_ALM_DST_ATTRI_BIT_REPT);
	reVal = gpnAlmDSApiAlmAttribModify(&localPIndex, almSubType, &dsAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) filt change error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType);

		return GPN_ALM_DEBUG_ERR;
	}

	sta = GPN_ALM_CLEAN;
	almSubType = GPN_ALM_TYPE_FUN_CFM_LOC;
	reVal = gpnAlmANtBaseSubTypeProc(&peerPIndex, almSubType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType, sta);

		return GPN_ALM_DEBUG_ERR;
	}
	
	sta = GPN_ALM_ARISE;
	almSubType = GPN_ALM_TYPE_FUN_CFM_AIS;
	reVal = gpnAlmANtBaseSubTypeProc(&peerPIndex, almSubType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType, sta);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	
	debugGpnAlmCurrAlmTableGetNext();
	debugGpnAlmAttributeTableGetNext();
	
	sta = GPN_ALM_ARISE;
	almSubType = GPN_ALM_TYPE_FUN_CFM_LOC;
	reVal = gpnAlmANtBaseSubTypeProc(&peerPIndex, almSubType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType, sta);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);

	debugGpnAlmCurrAlmTableGetNext();

	return GPN_ALM_DEBUG_OK;
}
UINT32 debugGpnAlmAttributeReverseCfg(void)
{
	objLogicDesc localPIndex;
	optObjOrient peerPIndex;
	stDSAlmAttri dsAttri;
	UINT32 almSubType;
	UINT32 reVal;
	UINT32 sta;

	/* config global reverse mode : manu */
	gpnAlmDSApiReverModeCfg(GPN_ALM_INVE_MODE_MANU);
	printf("%s : start\n\r", __FUNCTION__);

	/* config base almType + port */
	//peerPIndex.devIndex = DeviceIndex_Create( 1, 0 );
	peerPIndex.devIndex = DeviceIndex_Create( 1 );
	peerPIndex.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	peerPIndex.portIndex3 = 16;
	peerPIndex.portIndex4 = 32;
	peerPIndex.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	peerPIndex.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	almSubType = GPN_ALM_TYPE_FUN_CFM_LOC;
	sta = GPN_ALM_ARISE;
	reVal = gpnAlmANtBaseSubTypeProc(&peerPIndex, almSubType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType, sta);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	
	debugGpnAlmCurrAlmTableGetNext();
	printf("%s : set reverse\n\r", __FUNCTION__);

	//localPIndex.devIndex = DeviceIndex_Create( 1, 0 );
	localPIndex.devIndex = DeviceIndex_Create( 1 );
	localPIndex.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 100);
	localPIndex.portIndex3 = 16;
	localPIndex.portIndex4 = 32;
	localPIndex.portIndex5 = VLAN_PortIndex_Create(IFM_VLAN_OAM_TYPE, 0, 5001, 100);
	localPIndex.portIndex6 = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, GPN_GEN_MSG_INVALID_PORT);
	dsAttri.revs = GPN_ALM_DEBUG_ENABLE;
	dsAttri.bitMark = ((1U << GPN_ALM_DST_ATTRI_BIT_REVS) << 16) | (1U << GPN_ALM_DST_ATTRI_BIT_REVS);
	reVal = gpnAlmDSApiAlmAttribModify(&localPIndex, almSubType, &dsAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) reverse change error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType);

		return GPN_ALM_DEBUG_ERR;
	}
	
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	
	debugGpnAlmCurrAlmTableGetNext();
	printf("%s : clear alarm\n\r", __FUNCTION__);

	sta = GPN_ALM_CLEAN;
	almSubType = GPN_ALM_TYPE_FUN_CFM_LOC;
	reVal = gpnAlmANtBaseSubTypeProc(&peerPIndex, almSubType, sta);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) sta(%d) error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType, sta);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	
	debugGpnAlmCurrAlmTableGetNext();
	printf("%s : clear reverse\n\r", __FUNCTION__);

	dsAttri.revs = GPN_ALM_DEBUG_DISABLE;
	dsAttri.bitMark = ((1U << GPN_ALM_DST_ATTRI_BIT_REVS) << 16) | (1U << GPN_ALM_DST_ATTRI_BIT_REVS);
	reVal = gpnAlmDSApiAlmAttribModify(&localPIndex, almSubType, &dsAttri);
	if(reVal != GPN_ALM_DEBUG_OK)
	{
		printf("%s : port(%08x-%08x|%08x|%08x|%08x|%08x) almType(%08x) reverse change error\n\r",\
			__FUNCTION__,\
			localPIndex.devIndex, localPIndex.portIndex,\
			localPIndex.portIndex3, localPIndex.portIndex4,\
			localPIndex.portIndex5, localPIndex.portIndex6,\
			almSubType);

		return GPN_ALM_DEBUG_ERR;
	}

	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);
	gpnAlm1SecondTimerPorc(NULL);

	debugGpnAlmCurrAlmTableGetNext();

	return GPN_ALM_DEBUG_OK;
}

UINT32 debugGpnAlmTable(void)
{
	debugGpnAlmDevSelfNotify();
	debugGpnAlmFixPortSelfNotify();
	debugGpnAlmVariPorNotify();
	
	//debugGpnAlmPortMonTableGet();
	//debugGpnAlmPortMonTableGetNext();
	//debugGpnAlmAttributeTableGet();
	//debugGpnAlmAttributeTableGetNext();
	debugGpnAlmAlmReport();
	//debugGpnAlmCurrAlmTableGet();
	debugGpnAlmCurrAlmTableGetNext();
	//debugGpnAlmHistAlmTableGet();
	//debugGpnAlmHistAlmTableGetNext();
	debugGpnAlmEvtReport();
	//debugGpnAlmEventTableGet();
	debugGpnAlmEventTableGetNext();
	debugGpnAlmAlmTableCyc();
	
	debugGpnAlmProductCountClear();
	//debugGpnAlmAttributeRankCfg();
	//debugGpnAlmAttributeScreenCfg();
	//debugGpnAlmAttributeFiltCfg();
	debugGpnAlmAttributeReportCfg();
	//debugGpnAlmAttributeReverseCfg();

	return GPN_ALM_DEBUG_OK;
}

#endif /* GPN_ALM_INSERT_DEBUG */

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_DEBUG_TABLE_C_*/
