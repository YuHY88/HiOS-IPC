/**********************************************************
* file name: gpnMemorySmpMgt.c
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2015-05-27
* function: 
*    define gpn support function about memory management
* modify:
*
***********************************************************/
#ifndef _GPN_MEMORY_SMP_MGT_C_
#define _GPN_MEMORY_SMP_MGT_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdlib.h>
#include <string.h>

#include "gpnSupport/gpnMemorySmpMgt.h"
#include "gpnSupport/gpnMemoryUP.h"

#ifdef GPN_MEM_MGT_USER_SUP
#include "gpnFuncSup/gpnMemUserSup.h"
#endif

stGpnMemSmpMgt *pgMemSmpMgt = NULL;

/* the order of each mtype in the following table must match the order
     followed in mtype enum declaration in gpnMemory.h */
stGpnMemObjTemp gSysMemType[] =
{
	{GPN_MTYPE_LOG,		"gpn_log"},
	{GPN_MTYPE_DEBUG,	"gpn_debug"},
	{GPN_MTYPE_MEMMGT,	"gpn_mem_mgt"},
		
	{GPN_MTYPE_SOCK,	"gpn_socke"},
	{GPN_MTYPE_DEVMGT,	"gpn_dev_info"},
	{GPN_MTYPE_TIMER,	"gpn_timer"},
	
};

#ifdef GPN_MEM_MGT_USER_SUP
extern stGpnMemObjTemp gUserMemType[GPN_MTYPE_USER_MAX-GPN_MEM_TYPE_USER_SUP_MIN];
#endif

static stGpnMemTpObjMgt *gpnMemSmpMgtObjLookUp(UINT32 mType);
static UINT32 gpnMemSmpMgtMallocMark(stGpnMemSMNode *smpNode, UINT32 mType, UINT32 mSize);
static void *gpnMemSmpMgtFreeMark(void *ptr);


UINT32 gpnMemSmpMgtInit(void)
{
	UINT32 i;
#ifdef GPN_MEM_MGT_USER_SUP
	UINT32 j;
#endif
	UINT32 memMgtSize;
	UINT32 sysMtObjNum;
	UINT32 userMtObjNum;

	stGpnMemSMNode *pgpnMemSmNode;
	stGpnMemTpObjMgt *memMgtObj;

	gpnMemoryUPInit();
	
	/* memory type valid check */
	if( (GPN_MTYPE_SYS_MAX > GPN_MEM_TYPE_SYS_SUP_MAX)
#ifdef GPN_MEM_MGT_USER_SUP
		|| (GPN_MTYPE_USER_MAX > GPN_MEM_TYPE_USER_SUP_MAX)
#endif
		)
	{
		GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) : memory type err, sys-max(%d)!\n\r",\
			__FUNCTION__, getpid(), GPN_MTYPE_SYS_MAX);
#ifdef GPN_MEM_MGT_USER_SUP
		GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) : memory type err, user-max(%d)!\n\r",\
			__FUNCTION__, getpid(), GPN_MTYPE_USER_MAX);
#endif		
		return GPN_MEM_SMP_MGT_ERR;
	}

	/* calculate gpn_mem simple management memory size */
	sysMtObjNum = sizeof(gSysMemType)/sizeof(gSysMemType[0]);
#ifdef GPN_MEM_MGT_USER_SUP
	userMtObjNum = sizeof(gUserMemType)/sizeof(gUserMemType[0]);
#else
	userMtObjNum = 0;
#endif

	memMgtSize = sizeof(stGpnMemSMNode) +\
				sizeof(stGpnMemSmpMgt) +\
				sizeof(stGpnMemTpObjMgt)*(sysMtObjNum + userMtObjNum);
		
	pgpnMemSmNode = (stGpnMemSMNode *)malloc(memMgtSize);
	if(pgpnMemSmNode == NULL)
	{
		GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) : malloc err!\n\r",\
			__FUNCTION__, getpid());
		return GPN_MEM_SMP_MGT_ERR;
	}

	memset(pgpnMemSmNode, 0, memMgtSize);

	/* init pgMemSmpMgt */
	pgMemSmpMgt = (stGpnMemSmpMgt *)(pgpnMemSmNode + 1);
	pgMemSmpMgt->memTpMgt.pMgtObj = (stGpnMemTpObjMgt *)(pgMemSmpMgt + 1);
	pgMemSmpMgt->memTpMgt.mTypeNum = sysMtObjNum + userMtObjNum;

	/* memory type init */
	for(i=0;i<GPN_MTYPE_SYS_MAX;i++)
	{
		memMgtObj = pgMemSmpMgt->memTpMgt.pMgtObj + i;
		memMgtObj->memType = gSysMemType[i].memType;
		memMgtObj->mMName = gSysMemType[i].mMName;

	}
#ifdef GPN_MEM_MGT_USER_SUP
	
	for(j=0;i<(GPN_MTYPE_SYS_MAX + GPN_MTYPE_USER_MAX - GPN_MEM_TYPE_USER_SUP_MIN);i++,j++)
	{
		memMgtObj = pgMemSmpMgt->memTpMgt.pMgtObj + i;
		memMgtObj->memType = gUserMemType[j].memType;
		memMgtObj->mMName = gUserMemType[j].mMName;
	}
#endif

	/* slice mark init */
	for(i=0;i<GPN_MEM_SMP_MARK_HASH;i++)
	{
		listInit(&(pgMemSmpMgt->memSmpMark.smpMarkList[i]));
		pgMemSmpMgt->memSmpMark.smpMarkNum[i] = 0;
	}

	memMgtObj = gpnMemSmpMgtObjLookUp(GPN_MTYPE_MEMMGT);
	if(memMgtObj == NULL)
	{
		GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) : mType(%d) can not find mgt obj!\n\r",\
			__FUNCTION__, getpid(), GPN_MTYPE_MEMMGT);
		return GPN_MEM_SMP_MGT_ERR;
	}
	gpnMemMgtObjMallocRenew(memMgtObj, memMgtSize);	
	/* add this malloc to simple memory management */
	gpnMemSmpMgtMallocMark(pgpnMemSmNode, GPN_MTYPE_MEMMGT, memMgtSize);

	/* pgMemSmpMgt debug */
	/*printf("mSize %08x pgpnMemSmNode %08x stGpnMemSMNode %08x pgMemSmpMgt %08x stGpnMemSmpMgt %08x ObjMgtptr %08x stGpnMemTpObjMgt %08x objNum %08x\n\r",\
		memMgtSize,\
		pgpnMemSmNode, sizeof(stGpnMemSMNode),\
		pgMemSmpMgt, sizeof(stGpnMemSmpMgt),\
		pgMemSmpMgt->memTpMgt.pMgtObj, sizeof(stGpnMemTpObjMgt), pgMemSmpMgt->memTpMgt.mTypeNum);*/
	return GPN_MEM_SMP_MGT_OK;
}

void *gpnMemSmpMgtMalloc(UINT32 mType, UINT32 size)
{
	UINT32 mSize;
	stGpnMemSMNode *pgpnMemSmNode;
	stGpnMemTpObjMgt *memMgtObj;
	
	if(pgMemSmpMgt == NULL)
	{
		gpnMemSmpMgtInit();
	}

	/*find memory mgt obj */
	memMgtObj = gpnMemSmpMgtObjLookUp(mType);
	if(memMgtObj == NULL)
	{
		GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) : mType(%d) can not find mgt obj!\n\r",\
			__FUNCTION__, getpid(), GPN_MTYPE_MEMMGT);
		return NULL;
	}
	
	/*malloc */
	mSize = size + sizeof(stGpnMemSMNode);
	pgpnMemSmNode = (stGpnMemSMNode *)malloc(mSize);
	if(pgpnMemSmNode != NULL)
	{
		gpnMemMgtObjMallocRenew(memMgtObj, mSize);
		/*normal mark */
		gpnMemSmpMgtMallocMark(pgpnMemSmNode, mType, mSize);
		
		return (void *)(pgpnMemSmNode + 1);
	}
	else
	{
		/*err mark */
		gpnMemMgtMallocErrRenew(memMgtObj);

		return NULL;
	}
}

void gpnMemSmpMgtFree(void *ptr)
{
	void *freePtr;
	
	if( (pgMemSmpMgt == NULL) ||\
		(ptr == NULL) )
	{
		/* error, first should be malloc */
		GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) :  pgMemSmpMgt not init or ptr is NULL, err!\n\r",\
			__FUNCTION__, getpid());
		return;
	}

	freePtr = gpnMemSmpMgtFreeMark(ptr);
	if(freePtr != NULL)
	{
		free(freePtr);
	}
	return;
}

stGpnMemTpMgt *gpnMemSmpMgtMemTpGet(void)
{
	if(pgMemSmpMgt == NULL)
	{
		gpnMemSmpMgtInit();
	}
	
	return &(pgMemSmpMgt->memTpMgt);
}

stGpnMemSmpMark *gpnMemSmpMgtMarkerGet(void)
{
	if(pgMemSmpMgt == NULL)
	{
		gpnMemSmpMgtInit();
	}
	
	return &(pgMemSmpMgt->memSmpMark);
}

static stGpnMemTpObjMgt *gpnMemSmpMgtObjLookUp(UINT32 mType)
{
	UINT32 iType;
		
	if(mType < GPN_MTYPE_SYS_MAX)
	{
		iType = mType;
	}
#ifdef GPN_MEM_MGT_USER_SUP
	else if((mType >= GPN_MEM_TYPE_USER_SUP_MIN) &&
			(mType < GPN_MEM_TYPE_USER_SUP_MAX))
	{
		iType = (mType - GPN_MEM_TYPE_USER_SUP_MIN + GPN_MTYPE_SYS_MAX);
	}
#endif
	else
	{
		/* invalid type */
		GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) : mType(%d) invalid !\n\r",\
			__FUNCTION__, getpid(), mType);
		return NULL;
	}

	return pgMemSmpMgt->memTpMgt.pMgtObj + iType;
}
UINT32 gpnMemMgtObjMallocRenew(stGpnMemTpObjMgt *memMgtObj, UINT32 mSize)
{
	/*assert */
	if(memMgtObj == NULL)
	{
		GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) :  para NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_MEM_SMP_MGT_ERR;
	}

	/*obj modify */
	memMgtObj->mCounter += 1;
	memMgtObj->mByte += mSize;

	return GPN_MEM_SMP_MGT_OK;
}

static UINT32 gpnMemSmpMgtMallocMark(stGpnMemSMNode *smpNode, UINT32 mType, UINT32 mSize)
{
	UINT32 hash;
	
	/*assert */
	if(smpNode == NULL)
	{
		GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) :  para NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_MEM_SMP_MGT_ERR;
	}

	/* modify smiple memory management mark */
	smpNode->mType = mType;
	smpNode->mSize = mSize;
	smpNode->memPtr = smpNode;
	
	hash = ((UINT32)smpNode >> 2)%GPN_MEM_SMP_MARK_HASH;
	listAdd(&(pgMemSmpMgt->memSmpMark.smpMarkList[hash]), (NODE *)smpNode);
	pgMemSmpMgt->memSmpMark.smpMarkNum[hash]++;

	return GPN_MEM_SMP_MGT_OK;
}

UINT32 gpnMemMgtMallocErrRenew(stGpnMemTpObjMgt *memMgtObj)
{
	/*assert */
	if(memMgtObj == NULL)
	{
		GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) :  para NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_MEM_SMP_MGT_ERR;
	}

	/* modify memory management obj */
	memMgtObj->errMallCnt += 1;

	return GPN_MEM_SMP_MGT_OK;
}

UINT32 gpnMemMgtObjFreeRenew(stGpnMemTpObjMgt *memMgtObj, UINT32 mSize)
{
	/*assert */
	if(memMgtObj == NULL)
	{
		GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) :  para NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_MEM_SMP_MGT_ERR;
	}

	/* modify memory management obj */
	memMgtObj->fCounter += 1;
	memMgtObj->fByte += mSize;

	return GPN_MEM_SMP_MGT_OK;
}

static void *gpnMemSmpMgtFreeMark(void *ptr)
{
	UINT32 hash;
	UINT32 smpNodeNum;
	LIST *list;
	UINT32 smpNodeMax;
	stGpnMemSMNode *smpNode;
	stGpnMemSMNode *pSmpNode;
	stGpnMemTpObjMgt *memMgtObj;
		
	/*assert */
	if((UINT32)ptr < sizeof(stGpnMemSMNode))
	{
		GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) :  user ptr(%08x) err!\n\r",\
			__FUNCTION__, getpid(), (UINT32)ptr);
		return NULL;
	}

	pSmpNode = (stGpnMemSMNode *)ptr - 1;
	hash = ((UINT32)pSmpNode >> 2)%GPN_MEM_SMP_MARK_HASH;
	list = &(pgMemSmpMgt->memSmpMark.smpMarkList[hash]);
	smpNodeMax = pgMemSmpMgt->memSmpMark.smpMarkNum[hash];
	
	smpNodeNum = 0;
	smpNode = (stGpnMemSMNode *)listFirst(list);
	while((smpNode != NULL)&&(smpNodeNum < smpNodeMax))
	{
		if(pSmpNode == smpNode->memPtr)
		{
			/* modify obj*/
			memMgtObj = gpnMemSmpMgtObjLookUp(smpNode->mType);
			if(memMgtObj == NULL)
			{
				GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) : mType(%d) can not find mgt obj!\n\r",\
					__FUNCTION__, getpid(), smpNode->mType);
				return NULL;
			}
			gpnMemMgtObjFreeRenew(memMgtObj, smpNode->mSize);

			/* free mark */
			listDelete(list, (NODE *)smpNode);

			return (void *)smpNode->memPtr;
		}
		
		smpNodeNum++;
		smpNode = (stGpnMemSMNode *)listNext((NODE *)smpNode);
	}
	
	GPN_MEM_SMP_MGT_PRINT(GPN_MEM_SMP_MGT_AGP, "%s(%d) :  user ptr(%08x) can not find smpNode !\n\r",\
		__FUNCTION__, getpid(), (UINT32)ptr);

	return NULL;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_MEMORY_SMP_MGT_C_*/

