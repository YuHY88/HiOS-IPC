/**********************************************************
* file name: gpnCommList.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-05
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_COMM_LIST_H_
#define _GPN_COMM_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

/* type definitions */
#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"

/*
typedef UINT32 unsigned int;
typedef INT32 int;
*/

/* data struct definitions */
typedef struct _list_node_
{   
	struct _list_node_ *next;  
	struct _list_node_ *previous;    
}NODE;

typedef struct _list_
{   
	NODE node;
	UINT32 count;   
}LIST;

#define  HEAD     node.next  
#define  TAIL     node.previous

/* function declarations */
void listInit (LIST *pList);
NODE *listFirst (LIST *pList);
NODE *listGet (LIST *pList);
NODE *listNext (NODE *pNode);
void listAdd (LIST *pList, NODE *pNode);
void listDelete (LIST *pList, NODE *pNode);
NODE *listLast (LIST *pList);
NODE *listNStep (NODE *pNode, INT32 nStep);
NODE *listNth (LIST *pList, INT32 nodenum);
NODE *listPrevious (NODE *pNode);
UINT32 listCount (LIST *pList);
UINT32 listFind (LIST *pList, NODE *pNode);
void listConcat (LIST *pDstList, LIST *pAddList);
void listExtract (LIST *pSrcList, NODE *pStartNode, NODE *pEndNode,	LIST *pDstList);
void listInsert (LIST *pList, NODE *pPrev, NODE *pNode);
void listFree (LIST *pList);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_COMM_LIST_H_ */

