/**********************************************************
* file name: gpnAlmAssist.c
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-08-15
* function: 
*    define socket comm details
* modify:
*
***********************************************************/
#ifndef _GPN_COMM_LIST_C_
#define _GPN_COMM_LIST_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdlib.h>
#include "socketComm/gpnCommList.h"

void listInit( LIST *pList )
{
    pList->HEAD  = NULL;
    pList->TAIL  = NULL;
    pList->count = 0;
}

NODE *listFirst( LIST *pList )
{
    return ( pList->HEAD );
}

NODE *listGet( LIST *pList )  
{
    NODE *pNode = pList->HEAD;

    if ( pNode != NULL )
    {
    	pList->HEAD = pNode->next;

    	if ( pNode->next == NULL )
    	{
        	pList->TAIL = NULL;
    	}
    	else
    	{
        	pNode->next->previous = NULL;
    	}

    	pList->count--;
    }  

    return ( pNode );  
}

NODE *listNext( NODE *pNode )  
{
    return ( pNode->next );  
}

void listAdd( LIST *pList, NODE *pNode )  
{
    listInsert ( pList, pList->TAIL, pNode );
}

void listDelete( LIST *pList, NODE *pNode )  
{
    if( pNode->previous == NULL )
    {
    	pList->HEAD = pNode->next;
    }
    else
    {
    	pNode->previous->next = pNode->next;
    }

    if( pNode->next == NULL )
    {
    	pList->TAIL = pNode->previous;
    }
    else
    {
    	pNode->next->previous = pNode->previous;
    }

    pList->count--;  
}

NODE *listLast( LIST *pList )
{
    return ( pList->TAIL );  
}

NODE *listNStep( NODE *pNode, INT32 nStep )  
{  
    INT32 i;

    for( i = 0; i < abs(nStep); i++ )  
    {  
    	if( nStep < 0 )
    	{
        	pNode = pNode->previous;
    	}
    	else if( nStep > 0 )
    	{
        	pNode = pNode->next;
    		if( pNode == NULL )
    		{
        		break;
    		}
    	}
    }
    return ( pNode );
}

NODE *listNth( LIST *pList, INT32 nodenum )
{  
    NODE *pNode;

    if( (nodenum < 1) || (nodenum > (INT32)pList->count ) )
    {
    	return ( NULL );
    }

    if( nodenum < (INT32)(pList->count >> 1) )
    {
    	pNode = pList->HEAD;

 	 	while ( --nodenum > 0 )
 	 	{
        	pNode = pNode->next;
 	 	}
    }
    else
    {
    	nodenum -= pList->count;
    	pNode = pList->TAIL;
		
    	while ( nodenum++ < 0 )
    	{
        	pNode = pNode->previous;
    	}
    }

    return ( pNode );  
}

NODE *listPrevious( NODE *pNode )  
{
    return ( pNode->previous );
}

UINT32 listCount( LIST *pList ) 
{
	return ( pList->count );
}

UINT32 listFind( LIST *pList, NODE *pNode )  
{
    NODE *pNextNode;
    UINT32 index = 1;

    pNextNode = listFirst ( pList );

    while( (pNextNode != NULL) && (pNextNode != pNode) )
    {
     	index++;
    	pNextNode = listNext ( pNextNode );
    }

    if( pNextNode == NULL )
    {
    	return ( 0 );
    }
    else
    {
    	return ( index );
    }
}
  
void listConcat( LIST *pDstList, LIST *pAddList )
{
    if( pAddList->count == 0 )
    {
    	return;
    }

    if( pDstList->count == 0 )
    {
    	*pDstList = *pAddList;
    }
    else
    {
    	pDstList->TAIL->next     = pAddList->HEAD;
    	pAddList->HEAD->previous = pDstList->TAIL;
    	pDstList->TAIL           = pAddList->TAIL;

    	pDstList->count += pAddList->count;
    }

    listInit ( pAddList );
}

void listExtract( LIST *pSrcList, NODE *pStartNode, NODE *pEndNode, LIST *pDstList )
{
    UINT32 i;
    NODE *pNode;
  
    if( pStartNode->previous == NULL )
    {
    	pSrcList->HEAD = pEndNode->next;
    }
    else
    {
    	pStartNode->previous->next = pEndNode->next;
    }

    if ( pEndNode->next == NULL )
    {
    	pSrcList->TAIL = pStartNode->previous;
    }
    else
    {
    	pEndNode->next->previous = pStartNode->previous;
    }

    pDstList->HEAD = pStartNode;
    pDstList->TAIL = pEndNode;
  
    pStartNode->previous = NULL;
    pEndNode->next       = NULL;
  
    i = 0;  
    for ( pNode = pStartNode; pNode != NULL; pNode = pNode->next )
    {
    	i++;
    }
    pSrcList->count -= i;
    pDstList->count = i;
}

void listInsert( LIST *pList, NODE *pPrev, NODE *pNode )  
{
    NODE *pNext;

    if( pPrev == NULL )
    {
    	pNext = pList->HEAD;
    	pList->HEAD = pNode;
    }
    else
    {
    	pNext = pPrev->next;
    	pPrev->next = pNode;
    }

    if( pNext == NULL )
    {
    	pList->TAIL = pNode;
    }
    else
    {
    	pNext->previous = pNode;
    }

    pNode->next      = pNext;
    pNode->previous  = pPrev;
	
    pList->count++;
}

void listFree( LIST *pList )  
{
    NODE *p1;
	NODE *p2;
  
    if (pList->count > 0) 
    {
    	p1 = pList->HEAD;
    	while (p1 != NULL)
        {
        	p2 = p1->next;
        	free ((char *)p1);
        	p1 = p2;
        }
    	pList->count = 0;
    	pList->HEAD = pList->TAIL = NULL;
    }
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_COMM_LIST_C_ */
