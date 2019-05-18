/**********************************************************
* file name: gpnMemoryMgtApi.c
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2015-05-27
* function: 
*    define gpn support function about memory management
* modify:
*
***********************************************************/
#ifndef _GPN_MEMORY_MGT_API_C_
#define _GPN_MEMORY_MGT_API_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "gpnSupport/gpnMemoryApi.h"
#include "gpnSupport/gpnMemoryUP.h"

extern stGpnMemSmpMgt *pgMemSmpMgt;

UINT32 gpnMemMgtObjShow2File(void)
{
	UINT32 i;
	FILE *fp;
	stGpnMemTpMgt *pgpnMemTpMgt;
	stGpnMemTpObjMgt *pMgtObj;
	char mTypeShow[GPN_MEM_MGT_SHOW_FNAME_LEN];
	char mTypeLine[GPN_MEM_MGT_SHOW_LINE_LEN];
	UINT32 mCounter;
	UINT32 mByte;
	UINT32 fCounter;
	UINT32 fByte;
	UINT32 uCounter;
	UINT32 uByte;
	UINT32 errMallCnt;

	if(pgMemSmpMgt == NULL)
	{
		gpnMemSmpMgtInit();
	}

	snprintf(mTypeShow, GPN_MEM_MGT_SHOW_FNAME_LEN, "%s/memType-%d",\
		GPN_MEM_UP_RAM_NODE, getpid());
	
	fp = fopen(mTypeShow, "w+");
	if(fp == NULL)
	{
		GPN_MEM_MGT_API_PRINT(GPN_MEM_MGT_API_AGP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), mTypeShow);
		return GPN_MEM_MGT_API_ERR;
	}

	/**/
#if defined(GPN_FUNC_SUP_MEM_SMP_MGT_EN)
	pgpnMemTpMgt = gpnMemSmpMgtMemTpGet();
#elif defined(GPN_FUNC_SUP_MEM_XXX_MGT_EN)
	pgpnMemTpMgt = NULL;
#else
	pgpnMemTpMgt = NULL;
#endif

	if(pgpnMemTpMgt == NULL)
	{
		GPN_MEM_MGT_API_PRINT(GPN_MEM_MGT_API_AGP, "%s(%d) : can not find memTpMgt data st!\n\r",
			__FUNCTION__, getpid());

		fclose(fp);
		return GPN_MEM_MGT_API_ERR;
	}

	mCounter = 0;
	mByte = 0;
	fCounter = 0;
	fByte = 0;
	uCounter = 0;
	uByte = 0;
	errMallCnt = 0;
	snprintf(mTypeLine, GPN_MEM_MGT_SHOW_LINE_LEN,\
		"mType mallocCount mallocByte  freeCount   freeByte usingCount  usingByte errMallCnt mTypeName\n");
	fwrite(mTypeLine, strlen(mTypeLine), 1, fp);
	for(i=0;i<pgpnMemTpMgt->mTypeNum;i++)
	{
		pMgtObj = pgpnMemTpMgt->pMgtObj + i;
		snprintf(mTypeLine, GPN_MEM_MGT_SHOW_LINE_LEN,\
			"%5d %11d %10d %10d %10d %10d %10d %10d %s\n",\
			pMgtObj->memType, pMgtObj->mCounter, pMgtObj->mByte,\
			pMgtObj->fCounter, pMgtObj->fByte,\
			pMgtObj->mCounter - pMgtObj->fCounter,\
			pMgtObj->mByte - pMgtObj->fByte,\
			pMgtObj->errMallCnt, pMgtObj->mMName);
		fwrite(mTypeLine, strlen(mTypeLine), 1, fp);

		mCounter += pMgtObj->mCounter;
		mByte += pMgtObj->mByte;
		fCounter += pMgtObj->fCounter;
		fByte += pMgtObj->fByte;
		errMallCnt += pMgtObj->errMallCnt;
		
	}

	uCounter = mCounter - fCounter;
	uByte = mByte - fByte;
	snprintf(mTypeLine, GPN_MEM_MGT_SHOW_LINE_LEN,\
		"\ntotal %11d %10d %10d %10d %10d %10d %10d\n",\
		mCounter, mByte, fCounter, fByte, uCounter, uByte, errMallCnt);
	fwrite(mTypeLine, strlen(mTypeLine), 1, fp);

	fflush(fp);
	fclose(fp);
	
	return GPN_MEM_MGT_API_OK;
	
}

UINT32 gpnMemSmpMgtSlice2File(void)
{
	FILE *fp;
	UINT32 i;
	UINT32 slice;
	UINT32 mSize;
	UINT32 nodeNum;
	stGpnMemSMNode *pMarkNode;
	stGpnMemSmpMark *pMemSmpMark;
	char markShow[GPN_MEM_MGT_SHOW_FNAME_LEN];
	char markLine[GPN_MEM_MGT_SHOW_LINE_LEN];

	if(pgMemSmpMgt == NULL)
	{
		gpnMemSmpMgtInit();
	}
	
	snprintf(markShow, GPN_MEM_MGT_SHOW_FNAME_LEN, "%s/memSlice-%d",\
		GPN_MEM_UP_RAM_NODE, getpid());
	
	fp = fopen(markShow, "w+");
	if(fp == NULL)
	{
		GPN_MEM_MGT_API_PRINT(GPN_MEM_MGT_API_AGP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), markShow);
		return GPN_MEM_MGT_API_ERR;
	}

	snprintf(markLine, GPN_MEM_MGT_SHOW_LINE_LEN,\
		"mType    memSize        ptr\n");
	fwrite(markLine, strlen(markLine), 1, fp);

	slice = 0;
	mSize = 0;
	pMemSmpMark = gpnMemSmpMgtMarkerGet();
	for(i=0;i<GPN_MEM_SMP_MARK_HASH;i++)
	{
		nodeNum = 0;
		pMarkNode = (stGpnMemSMNode *)listFirst(&(pMemSmpMark->smpMarkList[i]));
		while((pMarkNode != NULL)&&(nodeNum < pMemSmpMark->smpMarkNum[i]))
		{
			snprintf(markLine, GPN_MEM_MGT_SHOW_LINE_LEN,\
				"%5d %10d 0x%08x\n",\
				pMarkNode->mType, pMarkNode->mSize, (UINT32)pMarkNode->memPtr);
			fwrite(markLine, strlen(markLine), 1, fp);

			slice++;
			mSize += pMarkNode->mSize;
			
			nodeNum++;
			pMarkNode = (stGpnMemSMNode *)listNext((NODE *)pMarkNode);
		}
	}
	snprintf(markLine, GPN_MEM_MGT_SHOW_LINE_LEN,\
		"total slice %d memSize %d\n", slice, mSize);
	fwrite(markLine, strlen(markLine), 1, fp);

	fflush(fp);
	fclose(fp);

	return GPN_MEM_MGT_API_OK;
}

UINT32 debugGpnMemSmpMgtCheck(void)
{
	void * ptr1;
	void * ptr2;
	void * ptr3;
	
	GPN_MALLOC(GPN_MTYPE_LOG, 80);
	GPN_MALLOC(GPN_MTYPE_SOCK, 80);
	GPN_MALLOC(GPN_MTYPE_TIMER, 80);

	ptr1 = GPN_MALLOC(GPN_MTYPE_LOG, 80);
	ptr2 = GPN_MALLOC(GPN_MTYPE_SOCK, 80);
	ptr3 = GPN_MALLOC(GPN_MTYPE_TIMER, 80);
	gpnMemSmpMgtSlice2File();

	GPN_FREE(ptr1);
	GPN_FREE(ptr2);
	GPN_FREE(ptr3);

	GPN_MALLOC(1025, 80);
	GPN_MALLOC(1026, 80);

	ptr1 = GPN_MALLOC(1025, 80);
	ptr2 = GPN_MALLOC(1026, 80);

	GPN_FREE(ptr1);
	GPN_FREE(ptr2);

	gpnMemMgtObjShow2File();
	gpnMemSmpMgtSlice2File();

	return GPN_MEM_MGT_API_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_MEMORY_MGT_API_C_*/

