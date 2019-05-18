/**********************************************************
* file name: gpnLogUnifyPath.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-30
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_LOG_UNIFY_PATH_C_
#define _GPN_LOG_UNIFY_PATH_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <sys/types.h>
#include <sys/stat.h>

#include "gpnLog/gpnLogUnifyPath.h"
    
UINT32 gpnLogUPPathInit(void)
{
	/*just put log function about file in follow path */
	gpnUnifyPathCreat((char *)(GPN_LOG_UP_ROM_NODE));
	gpnUnifyPathCreat((char *)(GPN_LOG_UP_RAM_NODE));

	/* put modules's log file  in follow path */
	gpnUnifyPathCreat((char *)(GPN_LOG_UP_LOG_NODE));
	
	return GPN_LOG_UP_OK;
}


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_LOG_UNIFY_PATH_C_*/

