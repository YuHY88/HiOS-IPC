/**********************************************************
* file name: gpnMemoryUP.c
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2015-05-29
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_MEMORY_UP_C_
#define _GPN_MEMORY_UP_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "gpnSupport/gpnMemoryUP.h"
    
UINT32 gpnMemoryUPInit(void)
{
	/*used for gpn_mem_mgt function*/
	gpnUnifyPathCreat((char *)GPN_MEM_UP_ROM_NODE);
	gpnUnifyPathCreat((char *)GPN_MEM_UP_RAM_NODE);
	
	return GPN_MEM_UP_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_MEMORY_UP_C_*/

