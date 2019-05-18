/**********************************************************
* file name: gpnAlmDSPorc.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-13
* function: 
*    define alarm Scan API
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_DS_PROC_C_
#define _GPN_ALM_DS_PROC_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <string.h>

#include <stdio.h>

#include "gpnAlmDataSheet.h"
#include "gpnAlmDSheetProc.h"
#include "gpnAlmMasterSlaveSync.h"

extern stAlmDataSheetSpace gstAlmDataSheetInfo;

UINT32 gpnAlmDspXDbCycModeChg(UINT32 dbType, UINT32 cycMode)
{
	stDataSheetNode *pstDataSheetNode;
	
	/*assert*/

	pstDataSheetNode = NULL;
	gpnAlmDataSheetGetXDB(dbType, &pstDataSheetNode);
	if(pstDataSheetNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	if( (GPN_ALM_LINE_DSHEET != cycMode) &&\
		(GPN_ALM_CYC_DSHEET != cycMode) )
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	/* x dbType who can be modified cyc mode */
	if( (GPN_ALM_CURR_ALM_DSHEET == dbType) ||\
		(GPN_ALM_HIST_ALM_DSHEET == dbType) ||\
		(GPN_ALM_EVENT_DSHEET == dbType) )
	{
		pstDataSheetNode->cycMode = cycMode;
		return GPN_ALM_DATA_SHEET_OK;
	}
	else
	{
		return GPN_ALM_DSP_OK;
	}
}

/*=================================================*/
/*name :  gpnAlmDspSubAlmRankChg                                                                  */
/*para :                                                                                                               */
/*retu :  void                                                                                                        */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmDspSubAlmRankChg(UINT32 index, UINT32 almRank)
{
	stCurrAlmDB currAlm;
	UINT32 reVal;
	
	reVal = gpnAlmDataSheetGetLine(GPN_ALM_CURR_ALM_DSHEET, index, &currAlm, sizeof(stCurrAlmDB));
	if(reVal != GPN_ALM_DSP_OK)
	{
		return GPN_ALM_DSP_ERR;
	}

	/* real have this alarm in dataSheet */
	currAlm.level = almRank;
	reVal = gpnAlmDataSheetModify(GPN_ALM_CURR_ALM_DSHEET, index, &currAlm, sizeof(stCurrAlmDB));
	if(reVal != GPN_ALM_DSP_OK)
	{
		return GPN_ALM_DSP_ERR;
	}
	
	return GPN_ALM_DSP_OK;

}
/*=================================================*/
/*name :  gpnAlmDspSubAlmAriseCountClear                                                       */
/*para :                                                                                                               */
/*retu :  void                                                                                                        */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmDspSubAlmAriseCountClear(UINT32 index)
{
	stCurrAlmDB currAlm;
	UINT32 reVal;
	
	reVal = gpnAlmDataSheetGetLine(GPN_ALM_CURR_ALM_DSHEET, index, &currAlm, sizeof(stCurrAlmDB));
	if(reVal != GPN_ALM_DSP_OK)
	{
		return GPN_ALM_DSP_ERR;
	}

	/* real have this alarm in dataSheet */
	currAlm.count = 1;
	currAlm.firstTime = currAlm.thisTime;
	reVal = gpnAlmDataSheetModify(GPN_ALM_CURR_ALM_DSHEET, index, &currAlm, sizeof(stCurrAlmDB));
	if(reVal != GPN_ALM_DSP_OK)
	{
		return GPN_ALM_DSP_ERR;
	}
	
	return GPN_ALM_DSP_OK;

}
/*=================================================*/
/*name :  gpnAlmDspTimeChangeProc                                                                  */
/*para :                                                                                                               */
/*retu :  void                                                                                                        */
/*desc :                                                                                                                */
/*==================================================*/
UINT32 gpnAlmDspTimeChangeProc(INT32 offset)
{
	stDataSheetNode *pstDSheetNode;
	stDStLineNode *pstLineNode;
	stCurrAlmDB *pcurrAlm;
	stEventAlmDB *peventAlm;

	/* 本盘为备盘时，不进行偏移处理 */
	if(DEVM_HA_SLAVE == alarm_MS_my_status_get())
	{
		return GPN_ALM_DSP_OK;
	}

	/* CURR alarm dateSheet */
	pstDSheetNode = NULL;
	gpnAlmDataSheetGetXDB(GPN_ALM_CURR_ALM_DSHEET, &pstDSheetNode);
	if(pstDSheetNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}
	
	/* get First Line */
	pstLineNode = (stDStLineNode *)listFirst(&(pstDSheetNode->usedList));
	while(pstLineNode != NULL)
	{
		pcurrAlm = (stCurrAlmDB *)(pstLineNode->lineAddr);
		/* if curr alarm arise first, change first time */
		if(pcurrAlm->count == 1)
		{
			pcurrAlm->firstTime = (UINT32)((INT32)(pcurrAlm->firstTime) + offset);
		}
		
		pcurrAlm->thisTime = (UINT32)((INT32)(pcurrAlm->thisTime) + offset);

		/* get next node */
		pstLineNode = (stDStLineNode *)listNext((NODE *)pstLineNode);
	}

	/* HIST alarm dateSheet */
	pstDSheetNode = NULL;
	gpnAlmDataSheetGetXDB(GPN_ALM_EVENT_DSHEET, &pstDSheetNode);
	if(pstDSheetNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}
	
	/* get First Line */
	pstLineNode = (stDStLineNode *)listFirst(&(pstDSheetNode->usedList));
	while(pstLineNode != NULL)
	{
		peventAlm = (stEventAlmDB *)(pstLineNode->lineAddr);
		peventAlm->thisTime = (UINT32)((INT32)(peventAlm->thisTime) + offset);

		/* get next node */
		pstLineNode = (stDStLineNode *)listNext((NODE *)pstLineNode);
	}

	return GPN_ALM_DSP_OK;

}



#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_ALM_DS_PROC_C_ */

