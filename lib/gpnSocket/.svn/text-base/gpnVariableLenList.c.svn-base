/**********************************************************
* file name: gpnVariableLenList.c
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-09
* function: 
*    define a special list data struct
* modify:
*
***********************************************************/

#ifndef _GPN_VLQ_DATA_STRUCTURE_C_
#define _GPN_VLQ_DATA_STRUCTURE_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include "socketComm/gpnVariableLenList.h"

UINT32 funcGPNVLQPieceInit(stVLQDataSt *pvlqData,UINT32 moduleId)
{
	UINT32 i; 
	stVLQNode *pvlqNode;
	UINT8 *memPoint;
	
	if(pvlqData == NULL)
	{
		GPN_VLQ_ST_PRINT(GPN_VLQ_ST_SVP, "funcGPNVLQPieceInit:parameter err-pointer is NULL\n");
		return GPN_VLQ_ST_NO;
	}
	if(pvlqData->pieceNum > pvlqData->usedPicNum)
	{
		i = pvlqData->stepLenth * pvlqData->nodeSize;
		memPoint = (UINT8*)malloc(i);
		if(memPoint == NULL)
		{
			perror("malloc:funcGPNVLQPieceInit err");
			return GPN_VLQ_ST_NO;
		}
		memset((UINT8 *)memPoint, 0, i);
		pvlqNode = (stVLQNode *)memPoint;	
		*(pvlqData->pPerPieceHead + pvlqData->usedPicNum) = (UINT32*)pvlqNode;

		for(i=0;i<pvlqData->stepLenth;i++)
		{
			pvlqNode->objSt = (void *)((UINT8 *)pvlqNode + sizeof(stVLQNode));
			listAdd(&(pvlqData->VLQIdleList), (NODE *)pvlqNode);
			memPoint += pvlqData->nodeSize;
			pvlqNode = (stVLQNode *)memPoint;
		}
		
		pvlqData->nodeNum += pvlqData->stepLenth;
		pvlqData->idleNodeNum += pvlqData->stepLenth;
		pvlqData->usedPicNum++;
	}
	else
	{
		GPN_VLQ_ST_PRINT(GPN_VLQ_ST_SVP, "funcGPNVLQPieceInit:piece use up\n");
		return GPN_VLQ_ST_NO;
	}
	
	return GPN_VLQ_ST_YES;
}

stVLQDataSt* funcGPNVLQInit(UINT32 pieceNum,UINT32 stepLenth,UINT32 pieceSize, UINT32 moduleId)
{
	UINT32 i;
	stVLQDataSt *pvlqData;

	if((pieceNum == 0)||(stepLenth == 0)||(pieceSize == 0))
	{
		GPN_VLQ_ST_PRINT(GPN_VLQ_ST_SVP, "funcGPNVLQInit:key parameter err,can not be 0\n");
		return NULL;
	}
	i = sizeof(stVLQDataSt)+pieceNum*sizeof(UINT32/*???*/);
	pvlqData = (stVLQDataSt *)malloc(i);
	if(pvlqData == NULL)
	{
		GPN_VLQ_ST_PRINT(GPN_VLQ_ST_SVP, "malloc:funcGPNVLQInit:VOS_Malloc err\n");
		return NULL;
	}
	memset((UINT8 *)pvlqData, 0, i);
	/*这里应该使用指向指针的指针*/
	pvlqData->pPerPieceHead = (UINT32 **)((UINT8*)pvlqData + sizeof(stVLQDataSt));
	pvlqData->pieceNum = pieceNum;
	pvlqData->usedPicNum = 0;
	pvlqData->stepLenth = stepLenth;
	pvlqData->nodeNum = 0;
	pvlqData->idleNodeNum = 0;
	pvlqData->nodeSize = sizeof(stVLQNode) + pieceSize;
	listInit(&(pvlqData->VLQIdleList));
	for(i=0;i<GPN_VLQ_HASH_KEY;i++)
	{
		listInit(&(pvlqData->VLQUseList[i]));
		pvlqData->VLQULNNum[i] = 0;
	}

	for(i=0;i<pvlqData->pieceNum;i++)
	{
		*(pvlqData->pPerPieceHead + i) = 0;
	}

	if(funcGPNVLQPieceInit(pvlqData,moduleId) == GPN_VLQ_ST_YES)
	{
		return pvlqData;
	}
	else
	{
		funcGPNVLQFree(pvlqData);
		return NULL;
	}
	
}

UINT32 funcGPNVLQFree(stVLQDataSt *pvlqData)
{
	UINT32 i;
	for(i=0;i<pvlqData->usedPicNum;i++)
	{
		if((*(pvlqData->pPerPieceHead + i)) != 0)
		{
			free((void *)(*(pvlqData->pPerPieceHead+i)));
		}
	}
	free((void *)pvlqData);

	/*外部置VLQData为NULL，避免野指针*/
	return GPN_VLQ_ST_YES;
}

stVLQNode *funcGPNVLQNodeGet(stVLQDataSt *pvlqData,UINT32 moduleId)
{
	stVLQNode *pvlqNode;
	UINT32 hashKey;

	pvlqNode = (stVLQNode *)listGet(&(pvlqData->VLQIdleList));
	if(pvlqNode == NULL)
	{
		if(pvlqData->pieceNum > pvlqData->usedPicNum)
		{
			funcGPNVLQPieceInit(pvlqData,moduleId);
			pvlqNode = (stVLQNode *)listGet(&(pvlqData->VLQIdleList));
		}
	}

	if(pvlqNode != NULL)
	{
		hashKey = (((UINT32)(pvlqNode->objSt))/4)%GPN_VLQ_HASH_KEY;
		listAdd(&(pvlqData->VLQUseList[hashKey]), (NODE *)pvlqNode);
		pvlqData->VLQULNNum[hashKey]++;
		pvlqData->idleNodeNum--;
	}
	return pvlqNode;
}

UINT32 funcGPNVLQNodeReclaim(stVLQDataSt *pvlqData,void *pVlqNodeObj)
{
	stVLQNode *pVlqNode;
	UINT32 hashKey;
	UINT32 nodeNum;
	
	if( (pvlqData == NULL)&&(pVlqNodeObj == NULL))
	{
		return GPN_VLQ_ST_NO;
	}

	hashKey = (((UINT32)pVlqNodeObj)/4)%GPN_VLQ_HASH_KEY;
	nodeNum = 0;
	pVlqNode = (stVLQNode *)listFirst(&(pvlqData->VLQUseList[hashKey]));
	while((pVlqNode != NULL) && (nodeNum < pvlqData->VLQULNNum[hashKey]))
	{
		if(((UINT32)(pVlqNode->objSt)) == (UINT32)pVlqNodeObj)
		{
			break;
		}

		nodeNum++;
		pVlqNode = (stVLQNode *)listNext((NODE *)(pVlqNode));
	}
	if((pVlqNode == NULL) || (nodeNum >= pvlqData->VLQULNNum[hashKey]))
	{
		GPN_VLQ_ST_PRINT(GPN_VLQ_ST_SVP, "funcGPNVLQNodeReclaim:mem not found\n");
		return GPN_VLQ_ST_NO;
	}
	else
	{
		listDelete(&(pvlqData->VLQUseList[hashKey]),(NODE *)pVlqNode);
		pvlqData->VLQULNNum[hashKey]--;
		listAdd(&(pvlqData->VLQIdleList), (NODE *)pVlqNode);
		pvlqData->idleNodeNum++;
	}
	
	return GPN_VLQ_ST_YES;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif/* _GPN_VLQ_DATA_STRUCTURE_C_ */


