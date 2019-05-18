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
#ifndef _GPN_MEMORY_MGT_API_H_
#define _GPN_MEMORY_MGT_API_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "gpnSupport/gpnMemorySmpMgt.h"

#define GPN_MEM_MGT_API_OK 						GPN_SOCK_SYS_OK
#define GPN_MEM_MGT_API_ERR 					GPN_SOCK_SYS_ERR

#define GPN_MEM_MGT_API_PRINT(level, info...) 	GEN_SOCK_SYS_PRINT((level), info)
#define GPN_MEM_MGT_API_AGP 					GEN_SOCK_SYS_AGP
#define GPN_MEM_MGT_API_SVP 					GEN_SOCK_SYS_SVP
#define GPN_MEM_MGT_API_CMP 					GEN_SOCK_SYS_CMP
#define GPN_MEM_MGT_API_CUP 					GEN_SOCK_SYS_CUP

#define GPN_FUNC_SUP_MEM_SMP_MGT_EN
#undef GPN_FUNC_SUP_MEM_XXX_MGT_EN

/*
#undef GPN_FUNC_SUP_MEM_SMP_MGT_EN
#define GPN_FUNC_SUP_MEM_XXX_MGT_EN
*/

#if defined(GPN_FUNC_SUP_MEM_SMP_MGT_EN)
#define GPN_MALLOC(mType, size) gpnMemSmpMgtMalloc(mType, size);
#define GPN_FREE(ptr) gpnMemSmpMgtFree(ptr);

#elif defined(GPN_FUNC_SUP_MEM_XXX_MGT_EN)

#else
#define GPN_MALLOC(mType, size) malloc(size);
#define GPN_FREE(ptr) free(ptr);

#endif

#define GPN_MEM_MGT_SHOW_FNAME_LEN		256
#define GPN_MEM_MGT_SHOW_LINE_LEN		256

UINT32 gpnMemMgtObjShow2File(void);
UINT32 gpnMemSmpMgtSlice2File(void);
UINT32 debugGpnMemSmpMgtCheck(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_MEMORY_MGT_API_H_*/

