/**********************************************************
* file name: gpnAlmDebugScalar.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-19
* function: 
*    define alarm scalar get/set debug
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_DEBUG_SCALAR_H_
#define _GPN_ALM_DEBUG_SCALAR_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef GPN_ALM_INSERT_DEBUG

#endif /* GPN_ALM_INSERT_DEBUG */
#if 1 /* GPN_ALM_INSERT_DEBUG */

#include <stdio.h>

#include "gpnAlmDebug.h"

UINT32 debugGpnAlmAllScalarGet(void);
UINT32 debugGpnAlmSmpScalarCfg(void);
UINT32 debugGpnAlmScalar(void);


#endif /* GPN_ALM_INSERT_DEBUG */

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_ALM_DEBUG_SCALAR_H_ */
