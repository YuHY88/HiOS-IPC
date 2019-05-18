/**********************************************************
* file name: gpnMemoryUP.h
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2015-05-29
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_MEMORY_UP_H_
#define _GPN_MEMORY_UP_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"


#include "socketComm/gpnUnifyPathDef.h"
#include "gpnLog/gpnLogFuncApi.h"


#define GPN_MEM_UP_OK 						GPN_SOCK_SYS_OK
#define GPN_MEM_UP_ERR 						GPN_SOCK_SYS_ERR

/*gpn_mem_mgt unify path define*/
/*ROM*/
#define GPN_MEM_UP_ROM_NODE					GPN_UNIFY_PATH_ROM_NODE(config/gpn/gpn_mem_mgt/)
#define GPN_MEM_UP_ROM_DIR(fName)			GPN_SC_UP_ROM_NODE#fName


/*RAM*/
#define GPN_MEM_UP_RAM_NODE					GPN_UNIFY_PATH_RAM_NODE(gpn/gpn_mem_mgt/)
#define GPN_MEM_UP_RAM_DIR(fName)			GPN_SC_UP_RAM_NODE#fName

UINT32 gpnMemoryUPInit(void);


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_MEMORY_UP_H_*/

