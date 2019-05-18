/**********************************************************
* file name: gpnMemorySmpMgt.h
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2015-05-27
* function: 
*    define gpn support function about memory management
* modify:
*
***********************************************************/
#ifndef _GPN_MEMORY_SMP_MGT_H_
#define _GPN_MEMORY_SMP_MGT_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnCommList.h"


#define GPN_MEM_SMP_MGT_OK 						GPN_SOCK_SYS_OK
#define GPN_MEM_SMP_MGT_ERR 					GPN_SOCK_SYS_ERR

#define GPN_MEM_SMP_MGT_PRINT(level, info...) 	GEN_SOCK_SYS_PRINT((level), info)
#define GPN_MEM_SMP_MGT_AGP 					GEN_SOCK_SYS_AGP
#define GPN_MEM_SMP_MGT_SVP 					GEN_SOCK_SYS_SVP
#define GPN_MEM_SMP_MGT_CMP 					GEN_SOCK_SYS_CMP
#define GPN_MEM_SMP_MGT_CUP 					GEN_SOCK_SYS_CUP

#define GPN_MEM_TYPE_SYS_SUP_MIN				0
#define GPN_MEM_TYPE_SYS_SUP_MAX				1024
#define GPN_MEM_TYPE_USER_SUP_MIN				1025
#define GPN_MEM_TYPE_USER_SUP_MAX				2048

typedef enum _emGpnSupMemType_
{
	/* used for gpn_log , must always be first and 
	    should be GPN_MEM_TYPE_SYS_SUP_MIN */
	GPN_MTYPE_LOG = GPN_MEM_TYPE_SYS_SUP_MIN,
	/* used for gpn_debug */
	GPN_MTYPE_DEBUG,
	/* used for gpn_dev_info */
	GPN_MTYPE_MEMMGT,

	
	/* used for gpn_socket*/
	GPN_MTYPE_SOCK,
	/* used for gpn_dev_info */
	GPN_MTYPE_DEVMGT,
	/* used for gpn_timer */
	GPN_MTYPE_TIMER,

	/* add for other type  ... ... ...*/

	/* must be GPN_MEM_MGT_SYS_SUP memory type last and
	    should be largest & less than GPN_MEM_TYPE_SYS_SUP_MAX(2048)*/
	GPN_MTYPE_SYS_MAX
}emGpnSupMemType;


typedef struct _stGpnMemObjTemp_
{
	UINT32 memType;
	const char *mMName;
}stGpnMemObjTemp;

typedef struct _stGpnMemTpObjMgt_
{
	UINT32 memType;
	
	UINT32 mCounter;
	UINT32 mByte;
	
	UINT32 fCounter;
	UINT32 fByte;

	UINT32 errMallCnt;
	/*UINT32 errFreeCnt;*/
	
	const char *mMName;
}stGpnMemTpObjMgt;

typedef struct _stGpnMemTpMgt_
{
	UINT32 mTypeNum;
	stGpnMemTpObjMgt *pMgtObj;

}stGpnMemTpMgt;

typedef struct _stGpnMemSMNode_
{
	NODE node;
	
	UINT32 mType;
	UINT32 mSize;
	struct _stGpnMemSMNode_ *memPtr;
	/*void *userPtr = (UINT8 *)memPtr + sizeof(stGpnMemSMNode)  */
	/*void *userPtr;*/

}stGpnMemSMNode;

/* (ptr >> 2) % hsah, memory address should be DDWORD align */
#define GPN_MEM_SMP_MARK_HASH	20

typedef struct _stGpnMemSmpMark_
{
	LIST smpMarkList[GPN_MEM_SMP_MARK_HASH];
	UINT32 smpMarkNum[GPN_MEM_SMP_MARK_HASH];

}stGpnMemSmpMark;

typedef struct _stGpnMemSmpMgt_
{
	stGpnMemTpMgt memTpMgt;
	stGpnMemSmpMark memSmpMark;
}stGpnMemSmpMgt;

UINT32 gpnMemSmpMgtInit(void);
stGpnMemTpMgt *gpnMemSmpMgtMemTpGet(void);
stGpnMemSmpMark *gpnMemSmpMgtMarkerGet(void);
void *gpnMemSmpMgtMalloc(UINT32 mType, UINT32 size);
void gpnMemSmpMgtFree(void *ptr);
UINT32 gpnMemMgtObjMallocRenew(stGpnMemTpObjMgt *memMgtObj, UINT32 mSize);
UINT32 gpnMemMgtMallocErrRenew(stGpnMemTpObjMgt *memMgtObj);
UINT32 gpnMemMgtObjFreeRenew(stGpnMemTpObjMgt *memMgtObj, UINT32 mSize);


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_MEMORY_SMP_MGT_H_*/

