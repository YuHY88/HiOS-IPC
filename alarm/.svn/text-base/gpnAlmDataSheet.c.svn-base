/**********************************************************
* file name: gpnAlmDataSheet.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-09-23
* function: 
*    define alarm Scan API
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_DATA_SHEET_C_
#define _GPN_ALM_DATA_SHEET_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <string.h>

#include <stdio.h>

#include <lib/vty.h>

#include "gpnAlmShow.h"

#include "gpnAlmDataSheet.h"

#include "gpnAlmScan.h"

stAlmDataSheetSpace gstAlmDataSheetInfo;
extern gstAlmTypeWholwInfo *pgstAlmTypeWholwInfo;
extern stEQUAlmProcSpace *pgstAlmPreScanWholwInfo;


stDataSheetTemp gstDataSheetTemp[] =
{
	{GPN_ALM_CURR_ALM_DSHEET, 	GPN_ALM_CURR_ALM_DB_SIZE,	GPN_ALM_CYC_DSHEET,	sizeof(stCurrAlmDB),	"CURR ALM DB"},
	{GPN_ALM_HIST_ALM_DSHEET, 	GPN_ALM_HIST_ALM_DB_SIZE,	GPN_ALM_CYC_DSHEET,	sizeof(stHistAlmDB),	"HIST ALM DB"},
	{GPN_ALM_EVENT_DSHEET, 	  	GPN_ALM_EVENT_DB_SIZE,		GPN_ALM_CYC_DSHEET,	sizeof(stEventAlmDB),	"EVENT DB"},
};

UINT32 gpnAlmRAMDataSheetInit(void)
{
	UINT32 i;
	UINT32 dbNum;
	stDataSheetTemp *pgstDataSheetTemp;

	/* init global dataSheet space */
	listInit(&(gstAlmDataSheetInfo.almDSList));

	/* add x-type dataSheet in global dataSheet */
	pgstDataSheetTemp = gstDataSheetTemp;
	dbNum = sizeof(gstDataSheetTemp)/sizeof(gstDataSheetTemp[0]);

	pgstAlmPreScanWholwInfo->almGlobalCfg.currDBSize = GPN_ALM_CURR_ALM_DB_SIZE;
	pgstAlmPreScanWholwInfo->almGlobalCfg.histDBSize = GPN_ALM_HIST_ALM_DB_SIZE;
	pgstAlmPreScanWholwInfo->almGlobalCfg.eventDBSize = GPN_ALM_EVENT_DB_SIZE;

	pgstAlmPreScanWholwInfo->almGlobalCfg.almPortBaseMoniCfgNum =
		pgstAlmPreScanWholwInfo->PTpVsSTpRelation.sysSupPortTpNum * EQU_SLOT_MAX_ID * GPN_ALM_PRESCAN_HASH;
	pgstAlmPreScanWholwInfo->almGlobalCfg.almAttribCfgNum = 
		pgstAlmTypeWholwInfo->almSubTypeNum + pgstAlmPreScanWholwInfo->almGlobalCfg.almPortBaseMoniCfgNum;

	for(i=0; i<dbNum; i++)
	{
		gpnAlmXTypeDataSheetInit(pgstDataSheetTemp);
		pgstDataSheetTemp++;
	}
	
	return GPN_ALM_DATA_SHEET_OK;
}
UINT32 gpnAlmXTypeDataSheetInit(stDataSheetTemp *pDsTemp)
{
	UINT32 i;
	stDataSheetNode *pstDSheetNode;

	/* assert */

	/* malloc X-Type dataSheet node */
	pstDSheetNode = (stDataSheetNode *)malloc(sizeof(stDataSheetNode));
	if(pstDSheetNode == NULL)
	{
		perror("malloc");
		return GPN_ALM_DATA_SHEET_ERR;
	}
	memset((UINT8 *)pstDSheetNode, 0, sizeof(stDataSheetNode));

	/* init X-Type dataSheet node */
	pstDSheetNode->dbType = pDsTemp->dbType;
	pstDSheetNode->maxLine = pDsTemp->maxLine;
	pstDSheetNode->cycMode = pDsTemp->cycMode;
	pstDSheetNode->unitSize = pDsTemp->unitSize;
	
	//pstDSheetNode->dbName = strndup(pDsTemp->dbName, 32);
	pstDSheetNode->dbName = strdup(pDsTemp->dbName);
	if(pstDSheetNode->dbName == NULL)
	{
		perror("strndup");
		free(pstDSheetNode);
		return GPN_ALM_DATA_SHEET_ERR;
	}
	
	for(i=0; i<GPN_ALM_DS_HASH_KEY; i++)
	{
		listInit(&(pstDSheetNode->usedhash[i]));
	}
	listInit(&(pstDSheetNode->usedList));
	listInit(&(pstDSheetNode->idleList));
	
	listAdd(&(gstAlmDataSheetInfo.almDSList), (NODE *)pstDSheetNode);

	/* init X-Type dataSheet lineNode */
	gpnAlmXTypeDataSheetLineInit(pstDSheetNode);
	
	return GPN_ALM_DATA_SHEET_OK;
}
UINT32 gpnAlmXTypeDataSheetLineInit(stDataSheetNode *pstDSheetNode)
{
	UINT32 i;
	UINT32 lineNum;
	UINT32 unitSize;
	stDStHashNode *pstDStHashNode;
	stDStLineNode *pstDStLineNode;
	void *pDataBuff;
	void *pUnitQuen;
	
	/* assert */
	
	/* malloc X-Type dataSheet LineNode HashNode DataBuff */
	lineNum = pstDSheetNode->maxLine;
	unitSize = pstDSheetNode->unitSize;
	
	i= lineNum * (sizeof(stDStHashNode) + sizeof(stDStLineNode) + unitSize);
	pUnitQuen = (void *)malloc(i);
	if(pUnitQuen == NULL)
	{
		perror("malloc");
		return GPN_ALM_DATA_SHEET_ERR;
	}
	memset((UINT8 *)pUnitQuen, 0, i);

	/* init X-Type dataSheet unitQuen */
	pstDStHashNode = (stDStHashNode *)pUnitQuen;
	pstDStLineNode = (stDStLineNode *)(pstDStHashNode + lineNum);
	pDataBuff = (void *)(pstDStLineNode + lineNum);
	for(i=0; i<lineNum; i++)
	{
		pstDStLineNode->lineAddr = pDataBuff;
		pstDStHashNode->lineNode = pstDStLineNode;
		
		listAdd(&(pstDSheetNode->idleList), (NODE *)pstDStHashNode);
		
		/* move pointer */
		pstDStHashNode++;
		pstDStLineNode++;
		pDataBuff = (void *)((UINT8 *)pDataBuff + unitSize);
	}

	return GPN_ALM_DATA_SHEET_OK;
}

/* dataSheet opt : add, delete, modify, get */
UINT32 gpnAlmDataSheetGetNode(UINT32 dbType, UINT32 index, stDStHashNode **ppstDStHashNode)
{
	stDataSheetNode *pstDataSheetNode;
	stDStHashNode *pstDStHashNode;
	UINT32 hash;
	
	/*assert*/

	pstDataSheetNode = NULL;
	gpnAlmDataSheetGetXDB(dbType, &pstDataSheetNode);
	if(pstDataSheetNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}
	
	/* get spLine from HASH list */
	hash = index % GPN_ALM_DS_HASH_KEY;
	
	pstDStHashNode = (stDStHashNode *)listFirst(&(pstDataSheetNode->usedhash[hash]));
	while(pstDStHashNode != NULL)
	{
		if(pstDStHashNode->index == index)
		{
			*ppstDStHashNode = pstDStHashNode;
			return GPN_ALM_DATA_SHEET_OK;
		}
		
		pstDStHashNode = (stDStHashNode *)listNext((NODE *)pstDStHashNode);
	}

	*ppstDStHashNode = NULL;
	return GPN_ALM_DATA_SHEET_ERR;
}
UINT32 gpnAlmDataSheetGetFirstNode(UINT32 dbType, stDStLineNode **ppstDStLineNode)
{
	stDataSheetNode *pstDataSheetNode;
	stDStLineNode *pstDStLineNode;
	
	/*assert*/

	pstDataSheetNode = NULL;
	gpnAlmDataSheetGetXDB(dbType, &pstDataSheetNode);
	if(pstDataSheetNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}
	
	/* get First Line */
	pstDStLineNode = (stDStLineNode *)listFirst(&(pstDataSheetNode->usedList));
	if(pstDStLineNode != NULL)
	{
		*ppstDStLineNode = pstDStLineNode;
		
		return GPN_ALM_DATA_SHEET_OK;
	}
	else
	{
		*ppstDStLineNode = NULL;
		
		return GPN_ALM_DATA_SHEET_ERR;
	}
}
UINT32 gpnAlmDataSheetGetNextNode(UINT32 dbType, UINT32 index, stDStLineNode **ppstDStNextNode)
{
	stDStHashNode *pstDStHashNode;
	stDStLineNode *pstDStLineNode;
	
	/*assert*/
	pstDStLineNode = NULL;
	if(index == GPN_ALM_DS_ILLEGAL_INDEX)
	{
		/* get first Line, as next line */
		gpnAlmDataSheetGetFirstNode(dbType, &pstDStLineNode);
	}
	else
	{
		/* get this Line */
		pstDStHashNode = NULL;
		gpnAlmDataSheetGetNode(dbType, index, &pstDStHashNode);
		if(pstDStHashNode != NULL)
		{
			pstDStLineNode = pstDStHashNode->lineNode;
		}

		/* get next Line */
		if(NULL == pstDStLineNode)
		{
			*ppstDStNextNode = NULL;
			return GPN_ALM_DATA_SHEET_ERR;
		}
		
		pstDStLineNode = (stDStLineNode *)listNext((NODE *)pstDStLineNode);
	}
	if(pstDStLineNode == NULL)
	{
		*ppstDStNextNode = NULL;
		return GPN_ALM_DATA_SHEET_ERR;
	}	

	*ppstDStNextNode = pstDStLineNode;

	return GPN_ALM_DATA_SHEET_OK;
}
UINT32 gpnAlmDataSheetGetXDB(UINT32 dbType, stDataSheetNode **ppstDSheetNode)
{
	stDataSheetNode *pstDataSheetTmp;

	pstDataSheetTmp = (stDataSheetNode *)listFirst(&(gstAlmDataSheetInfo.almDSList));
	while(pstDataSheetTmp != NULL)
	{
		if(pstDataSheetTmp->dbType == dbType)
		{
			*ppstDSheetNode = pstDataSheetTmp;
			
			return GPN_ALM_DATA_SHEET_OK;
		}

		pstDataSheetTmp = (stDataSheetNode *)listNext((NODE *)(pstDataSheetTmp));
	}

	*ppstDSheetNode = NULL;
	return GPN_ALM_DATA_SHEET_ERR;
}

UINT32 gpnAlmDataSheetGetLine(UINT32 dbType, UINT32 index, void *lineBuff, UINT32 len)
{
	stDStHashNode *pstDStHashNode;
	stDStLineNode *pstDStLineNode;
	
	/*assert*/
	if(lineBuff == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	pstDStLineNode = NULL;
	if(index == GPN_ALM_DS_ILLEGAL_INDEX)
	{
		gpnAlmDataSheetGetFirstNode(dbType, &pstDStLineNode);
	}
	else
	{
		pstDStHashNode = NULL;
		gpnAlmDataSheetGetNode(dbType, index, &pstDStHashNode);
		if(pstDStHashNode != NULL)
		{
			pstDStLineNode = pstDStHashNode->lineNode;
		}
	}
	if(pstDStLineNode != NULL)
	{
		memcpy(lineBuff, pstDStLineNode->lineAddr, len);
		
		return GPN_ALM_DATA_SHEET_OK;
	}
	else
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}	
}

UINT32 gpnAlmDataSheetGetNextLine(UINT32 dbType, UINT32 index, void *nextLineBuff, UINT32 len)
{
	stDStLineNode *pstDStNextNode;

	/* assert */
	if(nextLineBuff == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	/* get spLine from HASH list */
	pstDStNextNode = NULL;
	gpnAlmDataSheetGetNextNode(dbType, index, &pstDStNextNode);
	if(pstDStNextNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}
	
	memcpy(nextLineBuff, pstDStNextNode->lineAddr, len);

	return GPN_ALM_DATA_SHEET_OK;
}
UINT32 gpnAlmDataSheetAdd(UINT32 dbType, UINT32 index, void *lineBuff, UINT32 len)
{
	stDataSheetNode *pstDataSheetNode;
	stDStHashNode *pstDStHashNode;
	stDStLineNode *pstDStLineNode;
	UINT32 hash;
	
	/* assert */

	/* check if exit */
	pstDStHashNode = NULL;
	gpnAlmDataSheetGetNode(dbType, index, &pstDStHashNode);
	if(pstDStHashNode != NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	pstDataSheetNode = NULL;
	gpnAlmDataSheetGetXDB(dbType, &pstDataSheetNode);
	if(pstDataSheetNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	/* parameters illegal check */
	if( (lineBuff == NULL) ||\
		(len != pstDataSheetNode->unitSize) )
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	/* get idle node */
	pstDStHashNode = (stDStHashNode *)listGet(&(pstDataSheetNode->idleList));
	if(pstDStHashNode == NULL)
	{
		/* check for dbCyc Mode */
		if(pstDataSheetNode->cycMode != GPN_ALM_CYC_DSHEET)
		{
			return GPN_ALM_DATA_SHEET_ERR;
		}

		/* delete oldest node for new, 1 get line first index*/
		pstDStLineNode = (stDStLineNode *)listFirst(&(pstDataSheetNode->usedList));
		if(pstDStLineNode == NULL)
		{
			return GPN_ALM_DATA_SHEET_ERR;
		}

		/* delete oldest node for new, 2 user this index find hash node */
		hash = pstDStLineNode->index % GPN_ALM_DS_HASH_KEY;
		pstDStHashNode = (stDStHashNode *)listFirst(&(pstDataSheetNode->usedhash[hash]));
		while(pstDStHashNode != NULL)
		{
			if(pstDStHashNode->index == pstDStLineNode->index)
			{
				break;
			}
			
			pstDStHashNode = (stDStHashNode *)listNext((NODE *)pstDStHashNode);
		}

		if(pstDStHashNode == NULL)
		{
			return GPN_ALM_DATA_SHEET_ERR;
		}

		/* delete oldest node for new, 2 delete first node out quen base hash node */
		listDelete(&(pstDataSheetNode->usedhash[hash]), (NODE *)pstDStHashNode);
		listDelete(&(pstDataSheetNode->usedList), (NODE *)(pstDStHashNode->lineNode));
	}

	/* hash node init */
	pstDStHashNode->index = index;
	hash = index % GPN_ALM_DS_HASH_KEY;
	listAdd(&(pstDataSheetNode->usedhash[hash]), (NODE *)pstDStHashNode);

	/* line node init */
	pstDStLineNode = pstDStHashNode->lineNode;
	pstDStLineNode->index = index;
	memcpy(pstDStLineNode->lineAddr, lineBuff, len);
	listAdd(&(pstDataSheetNode->usedList), (NODE *)pstDStLineNode);

	return GPN_ALM_DATA_SHEET_OK;
}
UINT32 gpnAlmDataSheetDelete(UINT32 dbType, UINT32 index)
{
	stDataSheetNode *pstDSheetNode;
	stDStHashNode *pstDStHashNode;
	stDStLineNode *pstDStLineNode;
	UINT32 hash;
	
	/* assert */

	/* check if exit */
	pstDStHashNode = NULL;
	gpnAlmDataSheetGetNode(dbType, index, &pstDStHashNode);
	if(pstDStHashNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	/* get dataSheet node */
	pstDSheetNode = NULL;
	gpnAlmDataSheetGetXDB(dbType, &pstDSheetNode);
	if(pstDSheetNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	/* delete node */
	hash = pstDStHashNode->index % GPN_ALM_DS_HASH_KEY;
	listDelete(&(pstDSheetNode->usedhash[hash]), (NODE *)pstDStHashNode);

	pstDStLineNode = pstDStHashNode->lineNode;
	listDelete(&(pstDSheetNode->usedList), (NODE *)pstDStLineNode);

	listAdd(&(pstDSheetNode->idleList), (NODE *)pstDStHashNode);
	
	return GPN_ALM_DATA_SHEET_OK;
}
UINT32 gpnAlmDataSheetModify(UINT32 dbType, UINT32 index, void *lineBuff, UINT32 len)
{
	stDataSheetNode *pstDSheetNode;
	stDStHashNode *pstDStHashNode;
	stDStLineNode *pstDStLineNode;
	
	/* check if exit */
	pstDStHashNode = NULL;
	gpnAlmDataSheetGetNode(dbType, index, &pstDStHashNode);
	if(pstDStHashNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	pstDSheetNode = NULL;
	gpnAlmDataSheetGetXDB(dbType, &pstDSheetNode);
	if(pstDSheetNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	/* parameters illegal check */
	if( (lineBuff == NULL) ||\
		(len != pstDSheetNode->unitSize) )
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	pstDStLineNode = pstDStHashNode->lineNode;
	memcpy(pstDStLineNode->lineAddr, lineBuff, len);

	return GPN_ALM_DATA_SHEET_OK;
}

UINT32 debugGPNAlmDataBaseTypeShow(void)
{
	stDataSheetNode *pstDSheetNode;

	printf("type maxLine useLineNum cyc unitSize name\n\r");
	printf("-----------------------------------------------------------------------------\n\r");
	
	pstDSheetNode = (stDataSheetNode *)listFirst(&(gstAlmDataSheetInfo.almDSList));
	while(pstDSheetNode != NULL)
	{
		printf("%4d %7d %10d %3d %8d %s\n\r",
			pstDSheetNode->dbType,\
			pstDSheetNode->maxLine,\
			listCount(&(pstDSheetNode->usedList)),\
			pstDSheetNode->cycMode,\
			pstDSheetNode->unitSize,\
			pstDSheetNode->dbName);
		
		pstDSheetNode = (stDataSheetNode *)listNext((NODE *)(pstDSheetNode));
	}

	return GPN_ALM_DATA_SHEET_OK;
}

UINT32 debugGPNAlmRAMDBListPrint(UINT32 dbType)
{
	stDataSheetNode *pstDSheetNode;
	stDStLineNode *pstDStLineNode;

	stCurrAlmDB *pstCurrAlmDB;
	stHistAlmDB *pstHistAlmDB;
	stEventAlmDB *pstEventAlmDB;
	
	printf("type maxLine cyc unitSize name\n\r");
	printf("--------------------------------------------------\n\r");
	
	pstDSheetNode = NULL;
	gpnAlmDataSheetGetXDB(dbType, &pstDSheetNode);
	if(pstDSheetNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}
	
	printf("%04d %07d %03d %08d %s\n\r",
		pstDSheetNode->dbType,\
		pstDSheetNode->maxLine,\
		pstDSheetNode->cycMode,\
		pstDSheetNode->unitSize,\
		pstDSheetNode->dbName);
		
	switch(dbType)
	{
		case GPN_ALM_CURR_ALM_DSHEET:
			printf("GPN_ALM_CURR_ALM_DB(RAM)\n\r");
			printf("index almSubTp ifIndex  ifIndex2 ifIndex3 ifIndex4 ifIndex5 rank count fTime    tTime   \n\r");
			printf("----------------------------------------------------------------------------------------------\n\r");

			pstDStLineNode = (stDStLineNode *)listFirst(&(pstDSheetNode->usedList));
			while(pstDStLineNode != NULL)
			{
				pstCurrAlmDB = (stCurrAlmDB *)pstDStLineNode->lineAddr;
				printf("%05d %08x %08x %08x %08x %08x %08x %04d %05d %08x %08x\n\r",\
					pstCurrAlmDB->index,\
					pstCurrAlmDB->almType,\
					pstCurrAlmDB->ifIndex,\
					pstCurrAlmDB->ifIndex2,\
					pstCurrAlmDB->ifIndex3,\
					pstCurrAlmDB->ifIndex4,\
					pstCurrAlmDB->ifIndex5,\
					pstCurrAlmDB->level,\
					pstCurrAlmDB->count,\
					pstCurrAlmDB->firstTime,\
					pstCurrAlmDB->thisTime);
				
				pstDStLineNode = (stDStLineNode *)listNext((NODE *)(pstDStLineNode));
			}
			break;
			
		case GPN_ALM_HIST_ALM_DSHEET:
			printf("GPN_ALM_HIST_ALM_DB(RAM)\n\r");
			printf("index almSubTp ifIndex  ifIndex2 ifIndex3 ifIndex4 ifIndex5 rank count fTime    tTime   endTime\n\r");
			printf("------------------------------------------------------------------------------------------------------\n\r");

			pstDStLineNode = (stDStLineNode *)listFirst(&(pstDSheetNode->usedList));
			while(pstDStLineNode != NULL)
			{
				pstHistAlmDB = (stHistAlmDB *)pstDStLineNode->lineAddr;
				printf("%05d %08x %08x %08x %08x %08x %08x %04d %05d %08x %08x %08x\n\r",\
					pstHistAlmDB->index,\
					pstHistAlmDB->almType,\
					pstHistAlmDB->ifIndex,\
					pstHistAlmDB->ifIndex2,\
					pstHistAlmDB->ifIndex3,\
					pstHistAlmDB->ifIndex4,\
					pstHistAlmDB->ifIndex5,\
					pstHistAlmDB->level,\
					pstHistAlmDB->count,\
					pstHistAlmDB->firstTime,\
					pstHistAlmDB->thisTime,\
					pstHistAlmDB->disapTime);
				
				pstDStLineNode = (stDStLineNode *)listNext((NODE *)(pstDStLineNode));
			}
			break;

		case GPN_ALM_EVENT_DSHEET:
			printf("GPN_ALM_EVENT_DB(RAM)\n\r");
			printf("index eventTp  ifIndex  ifIndex2 ifIndex3 ifIndex4 ifIndex5 rank detial thisTime\n\r");
			printf("-------------------------------------------------------------------------------\n\r");

			pstDStLineNode = (stDStLineNode *)listFirst(&(pstDSheetNode->usedList));
			while(pstDStLineNode != NULL)
			{
				pstEventAlmDB = (stEventAlmDB *)pstDStLineNode->lineAddr;
				printf("%05d %08x %08x %08x %08x %08x %08x %04d %08x %08x\n\r",\
					pstEventAlmDB->index,\
					pstEventAlmDB->eventType,\
					pstEventAlmDB->ifIndex,\
					pstEventAlmDB->ifIndex2,\
					pstEventAlmDB->ifIndex3,\
					pstEventAlmDB->ifIndex4,\
					pstEventAlmDB->ifIndex5,\
					pstEventAlmDB->level,\
					pstEventAlmDB->detial,\
					pstEventAlmDB->thisTime);
				
				pstDStLineNode = (stDStLineNode *)listNext((NODE *)(pstDStLineNode));
			}
			break;
			
		default:
			break;
	}
	return GPN_ALM_DATA_SHEET_OK;
}

UINT32 gpnAlmDataSheetCurrAlmViewWrite(struct vty *vty)
{
	stCurrAlmDB *pCurrDB;
	stDataSheetNode *pstDSheetNode;
	stDStLineNode *pstDStLineNode;
	objLogicDesc portIndex;
	char show_line[GPN_ALM_SHOW_INFO_2_STR_STD_BUFF];

	/* find curr alm db node */
	pstDSheetNode = NULL;
	gpnAlmDataSheetGetXDB(GPN_ALM_CURR_ALM_DSHEET, &pstDSheetNode);
	if(pstDSheetNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}
	
	pstDStLineNode = (stDStLineNode *)listLast(&(pstDSheetNode->usedList));
	while(pstDStLineNode != NULL)
	{
		pCurrDB = (stCurrAlmDB *)pstDStLineNode->lineAddr;

		portIndex.devIndex = GPN_ILLEGAL_DEVICE_INDEX;
		portIndex.portIndex = pCurrDB->ifIndex;
		portIndex.portIndex3 = pCurrDB->ifIndex2;
		portIndex.portIndex4 = pCurrDB->ifIndex3;
		portIndex.portIndex5 = pCurrDB->ifIndex4;
		portIndex.portIndex6 = pCurrDB->ifIndex5;

		gpnAlmCurrAlmShowLineWrite(pCurrDB->index, pCurrDB->almType,
			&portIndex, pCurrDB->thisTime, show_line, GPN_ALM_SHOW_INFO_2_STR_STD_BUFF, vty);

		pstDStLineNode = (stDStLineNode *)listPrevious((NODE *)(pstDStLineNode));
	}

	return GPN_ALM_DATA_SHEET_OK;
}
UINT32 gpnAlmDataSheetHistAlmViewWrite(struct vty *vty)
{
	stHistAlmDB *pHistDB;
	stDataSheetNode *pstDSheetNode;
	stDStLineNode *pstDStLineNode;
	objLogicDesc portIndex;
	char show_line[GPN_ALM_SHOW_INFO_2_STR_STD_BUFF];

	/* find curr alm db node */
	pstDSheetNode = NULL;
	gpnAlmDataSheetGetXDB(GPN_ALM_HIST_ALM_DSHEET, &pstDSheetNode);
	if(pstDSheetNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}
	
	pstDStLineNode = (stDStLineNode *)listLast(&(pstDSheetNode->usedList));
	while(pstDStLineNode != NULL)
	{
		pHistDB = (stHistAlmDB *)pstDStLineNode->lineAddr;

		portIndex.devIndex = GPN_ILLEGAL_DEVICE_INDEX;
		portIndex.portIndex = pHistDB->ifIndex;
		portIndex.portIndex3 = pHistDB->ifIndex2;
		portIndex.portIndex4 = pHistDB->ifIndex3;
		portIndex.portIndex5 = pHistDB->ifIndex4;
		portIndex.portIndex6 = pHistDB->ifIndex5;

		gpnAlmHistAlmShowLineWrite(pHistDB->index, pHistDB->almType,
			&portIndex, pHistDB->thisTime, pHistDB->disapTime, show_line, GPN_ALM_SHOW_INFO_2_STR_STD_BUFF, vty);

		pstDStLineNode = (stDStLineNode *)listPrevious((NODE *)(pstDStLineNode));
	}

	return GPN_ALM_DATA_SHEET_OK;
}
UINT32 gpnAlmDataSheetEventViewWrite(struct vty *vty)
{
	stEventAlmDB *pEventDB;
	stDataSheetNode *pstDSheetNode;
	stDStLineNode *pstDStLineNode;
	objLogicDesc portIndex;
	char show_line[GPN_ALM_SHOW_INFO_2_STR_STD_BUFF];

	/* find curr alm db node */
	pstDSheetNode = NULL;
	gpnAlmDataSheetGetXDB(GPN_ALM_EVENT_DSHEET, &pstDSheetNode);
	if(pstDSheetNode == NULL)
	{
		return GPN_ALM_DATA_SHEET_ERR;
	}

	pstDStLineNode = (stDStLineNode *)listLast(&(pstDSheetNode->usedList));
	while(pstDStLineNode != NULL)
	{
		pEventDB = (stEventAlmDB *)pstDStLineNode->lineAddr;

		portIndex.devIndex = GPN_ILLEGAL_DEVICE_INDEX;
		portIndex.portIndex = pEventDB->ifIndex;
		portIndex.portIndex3 = pEventDB->ifIndex2;
		portIndex.portIndex4 = pEventDB->ifIndex3;
		portIndex.portIndex5 = pEventDB->ifIndex4;
		portIndex.portIndex6 = pEventDB->ifIndex5;

		gpnAlmEventShowLineWrite(pEventDB->index, pEventDB->eventType,
			&portIndex, pEventDB->thisTime, pEventDB->detial, show_line, GPN_ALM_SHOW_INFO_2_STR_STD_BUFF, vty);

		pstDStLineNode = (stDStLineNode *)listPrevious((NODE *)(pstDStLineNode));
	}

	return GPN_ALM_DATA_SHEET_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_DATA_SHEET_C_*/

