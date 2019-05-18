/**********************************************************
* file name: gpnVariableLenList.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-09
* function: 
*    define a special list data struct
* modify:
*
***********************************************************/
#ifndef _GPN_VLQ_DATA_STRUCTURE_H_
#define _GPN_VLQ_DATA_STRUCTURE_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <error.h>
#include "socketComm/gpnCommList.h"
#include "socketComm/gpnSockTypeDef.h"

#define GPN_VLQ_ST_YES 						GPN_SOCK_SYS_YES
#define GPN_VLQ_ST_NO 						GPN_SOCK_SYS_NO

#define GPN_VLQ_ST_PRINT(level, info...) 	GEN_SOCK_SYS_PRINT((level), info)
#define GPN_VLQ_ST_AGP 						GEN_SOCK_SYS_AGP
#define GPN_VLQ_ST_SVP 						GEN_SOCK_SYS_SVP
#define GPN_VLQ_ST_CMP 						GEN_SOCK_SYS_CMP
#define GPN_VLQ_ST_CUP 						GEN_SOCK_SYS_CUP

#define GPN_VLQ_HASH_KEY					10

typedef struct _stVLQDataSt_
{
	UINT32 **pPerPieceHead;
	UINT32 pieceNum;
	UINT32 usedPicNum;
	UINT32 stepLenth;	
	UINT32 nodeNum;
	UINT32 idleNodeNum;
	UINT32 nodeSize;
	LIST VLQIdleList;
	LIST VLQUseList[GPN_VLQ_HASH_KEY];
	UINT32 VLQULNNum[GPN_VLQ_HASH_KEY];
}stVLQDataSt;

typedef struct _stVLQNode_
{
	NODE node;
	void *objSt;
}stVLQNode;

UINT32 funcGPNVLQPieceInit(stVLQDataSt *pvlqData,UINT32 moduleId);
stVLQDataSt* funcGPNVLQInit(UINT32 pieceNum,UINT32 stepLenth,UINT32 pieceSize,UINT32 moduleId);
UINT32 funcGPNVLQFree(stVLQDataSt *pvlqData);
stVLQNode *funcGPNVLQNodeGet(stVLQDataSt *pvlqData,UINT32 moduleId);
UINT32 funcGPNVLQNodeReclaim(stVLQDataSt *pvlqData,void *pVlqNodeObj);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif/* _GPN_VLQ_DATA_STRUCTURE_H_ */


