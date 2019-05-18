/**********************************************************
* file name: gpnAlmPowDown.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-17
* function: 
*    define alarm power down porc about
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_POW_DOWN_C_
#define _GPN_ALM_POW_DOWN_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>

#include "gpnAlmPowDown.h"
#include "gpnAlmAlmNotify.h"

#include "gpnAlmScan.h"


/*log function include*/
#include "gpnLog/gpnLogFuncApi.h"

void gpnAlmPDClearSIGHandler(INT32 sig)
{
	gpnAlmPDClearDevPowerFail();
	gpnAlmPDClearDevPowerDown();

	return;
}

/*==================================================*/
/*name :  gpnAlmPDClearDevPowerFail                                                                   */
/*para :                                                                                                                */
/*retu :  void                                                                                                         */
/*desc :  no not user, clear by easy view                                                                  */
/*==================================================*/
UINT32 gpnAlmPDClearDevPowerFail(void)
{
	stAlmValueRecd almValueRecd;
	objLogicDesc viewPort;		
				
	viewPort.portIndex = SMP_PortIndex_Create(IFM_EQU_TYPE, GPN_ALM_PD_EQU_SLOT, 1);
	viewPort.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	viewPort.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	viewPort.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	viewPort.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	almValueRecd.almSubType = GPN_ALM_TYPE_DEV_POW_FAIL;
	
	almValueRecd.index = 0;
	almValueRecd.portAlmRank = GPN_ALM_RANK_EMERG;
	almValueRecd.prodCnt = 0;
	almValueRecd.firstTime = 0;
	almValueRecd.thisTime = 0;
	almValueRecd.disapTime = 0;

	/* alarm in hist db */
	gpnAlmANtHistAlmInsertDS(&viewPort, &almValueRecd);

	/* hist alarm trap */
	gpnAlmANtDisappReport(&viewPort, &almValueRecd);
	
	return GPN_ALM_PD_OK;
}

UINT32 gpnAlmPDClearDevPowerDown(void)
{
	UINT32 portNum;
	objLogicDesc viewPort;
	stAlmValueRecd almValueRecd;
	stAlmLocalNode *pAlmLocalPort;
	stAlmScanTypeDef *pAlmScanType;
	
	/*base portType to find scanType: -48V and 220V*/
	gpnAlmSeekPortTypeToAlmScanType(IFM_POWN48_TYPE, &pAlmScanType);
	if(pAlmScanType != NULL)
	{
		portNum = 0;
		pAlmLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
		while((pAlmLocalPort != NULL)&&(portNum < pAlmScanType->almScanQuenPortNum))
		{
			viewPort.portIndex = pAlmLocalPort->viewPort.portIndex;
			viewPort.portIndex3 = pAlmLocalPort->viewPort.portIndex3;
			viewPort.portIndex4 = pAlmLocalPort->viewPort.portIndex4;
			viewPort.portIndex5 = pAlmLocalPort->viewPort.portIndex5;
			viewPort.portIndex6 = pAlmLocalPort->viewPort.portIndex6;

			almValueRecd.almSubType = GPN_ALM_TYPE_POW_DOWN;

			almValueRecd.index = 0;
			almValueRecd.portAlmRank = GPN_ALM_RANK_EMERG;
			almValueRecd.prodCnt = 0;
			almValueRecd.firstTime = 0;
			almValueRecd.thisTime = 0;
			almValueRecd.disapTime = 0;

			/* alarm in hist db */
			gpnAlmANtHistAlmInsertDS(&viewPort, &almValueRecd);

			/* hist alarm trap */
			gpnAlmANtDisappReport(&viewPort, &almValueRecd);
			
			portNum++;
			pAlmLocalPort = (stAlmLocalNode *)listNext((NODE *)(pAlmLocalPort));
		}
	}
	
	gpnAlmSeekPortTypeToAlmScanType(IFM_POW220_TYPE, &pAlmScanType);
	if(pAlmScanType != NULL)
	{
		portNum = 0;
		pAlmLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
		while((pAlmLocalPort != NULL)&&(portNum < pAlmScanType->almScanQuenPortNum))
		{
			viewPort.portIndex = pAlmLocalPort->viewPort.portIndex;
			viewPort.portIndex3 = pAlmLocalPort->viewPort.portIndex3;
			viewPort.portIndex4 = pAlmLocalPort->viewPort.portIndex4;
			viewPort.portIndex5 = pAlmLocalPort->viewPort.portIndex5;
			viewPort.portIndex6 = pAlmLocalPort->viewPort.portIndex6;

			almValueRecd.almSubType = GPN_ALM_TYPE_POW_DOWN;

			almValueRecd.index = 0;
			almValueRecd.portAlmRank = GPN_ALM_RANK_EMERG;
			almValueRecd.prodCnt = 0;
			almValueRecd.firstTime = 0;
			almValueRecd.thisTime = 0;
			almValueRecd.disapTime = 0;

			/* alarm in hist db */
			gpnAlmANtHistAlmInsertDS(&viewPort, &almValueRecd);
			
			/* hist alarm trap */
			gpnAlmANtDisappReport(&viewPort, &almValueRecd);
			
			portNum++;
			pAlmLocalPort = (stAlmLocalNode *)listNext((NODE *)(pAlmLocalPort));
		}
	}

	return GPN_ALM_PD_OK;
}

void gpnAlmPDRiseSIGHandler(INT32 sig)
{
	/*printf("alarm got sig %d\n\r", sig);*/
	gpnAlmPDRiseDevPowerFail();
	gpnAlmPDRiseDevPowerDown();

	return;
}
UINT32 gpnAlmPDRiseDevPowerFail()
{	
	stAlmValueRecd almValueRecd;
	objLogicDesc viewPort;	
				
	viewPort.portIndex = SMP_PortIndex_Create(IFM_EQU_TYPE, GPN_ALM_PD_EQU_SLOT, 1);
	viewPort.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	viewPort.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	viewPort.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	viewPort.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	almValueRecd.almSubType = GPN_ALM_TYPE_DEV_POW_FAIL;
	
	almValueRecd.index = 0;
	almValueRecd.portAlmRank = GPN_ALM_RANK_EMERG;
	almValueRecd.prodCnt = 1;
	almValueRecd.firstTime = 0;
	almValueRecd.thisTime = 0;

	/* alarm in curr db */
	/* not time to do dataSheet opt */

	/* curr alarm trap */
	gpnAlmANtProductReport(&viewPort, &almValueRecd);

	return GPN_ALM_PD_OK;
}

UINT32 gpnAlmPDRiseDevPowerDown(void)
{
	UINT32 portNum;
	objLogicDesc viewPort;
	stAlmValueRecd almValueRecd;
	stAlmLocalNode *pAlmLocalPort;
	stAlmScanTypeDef *pAlmScanType;
	
	/*base portType to find scanType: -48V and 220V*/
	gpnAlmSeekPortTypeToAlmScanType(IFM_POWN48_TYPE, &pAlmScanType);
	if(pAlmScanType != NULL)
	{
		portNum = 0;
		pAlmLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
		while((pAlmLocalPort != NULL)&&(portNum < pAlmScanType->almScanQuenPortNum))
		{
			viewPort.portIndex = pAlmLocalPort->viewPort.portIndex;
			viewPort.portIndex3 = pAlmLocalPort->viewPort.portIndex3;
			viewPort.portIndex4 = pAlmLocalPort->viewPort.portIndex4;
			viewPort.portIndex5 = pAlmLocalPort->viewPort.portIndex5;
			viewPort.portIndex6 = pAlmLocalPort->viewPort.portIndex6;

			almValueRecd.almSubType = GPN_ALM_TYPE_POW_DOWN;

			almValueRecd.index = 0;
			almValueRecd.portAlmRank = GPN_ALM_RANK_EMERG;
			almValueRecd.prodCnt = 0;
			almValueRecd.firstTime = 0;
			almValueRecd.thisTime = 0;
			almValueRecd.disapTime = 0;

			/* alarm in curr db */
			/* not time to do dataSheet opt */

			/* curr alarm trap */
			gpnAlmANtDisappReport(&viewPort, &almValueRecd);
			
			portNum++;
			pAlmLocalPort = (stAlmLocalNode *)listNext((NODE *)(pAlmLocalPort));
		}
	}
	
	gpnAlmSeekPortTypeToAlmScanType(IFM_POW220_TYPE, &pAlmScanType);
	if(pAlmScanType != NULL)
	{
		portNum = 0;
		pAlmLocalPort = (stAlmLocalNode *)listFirst(&(pAlmScanType->almScanPortObjQuen));
		while((pAlmLocalPort != NULL)&&(portNum < pAlmScanType->almScanQuenPortNum))
		{
			viewPort.portIndex = pAlmLocalPort->viewPort.portIndex;
			viewPort.portIndex3 = pAlmLocalPort->viewPort.portIndex3;
			viewPort.portIndex4 = pAlmLocalPort->viewPort.portIndex4;
			viewPort.portIndex5 = pAlmLocalPort->viewPort.portIndex5;
			viewPort.portIndex6 = pAlmLocalPort->viewPort.portIndex6;

			almValueRecd.almSubType = GPN_ALM_TYPE_POW_DOWN;

			almValueRecd.index = 0;
			almValueRecd.portAlmRank = GPN_ALM_RANK_EMERG;
			almValueRecd.prodCnt = 0;
			almValueRecd.firstTime = 0;
			almValueRecd.thisTime = 0;
			almValueRecd.disapTime = 0;

			/* alarm in curr db */
			/* not time to do dataSheet opt */
			
			/* curr alarm trap */
			gpnAlmANtDisappReport(&viewPort, &almValueRecd);
			
			portNum++;
			pAlmLocalPort = (stAlmLocalNode *)listNext((NODE *)(pAlmLocalPort));
		}
	}

	return GPN_ALM_PD_OK;
}


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_ALM_POW_DOWN_C_ */

