/**********************************************************
* file name: gpnMemoryUserSup.c
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2016-08-08
* function: 
*    define gpn_stat about memory management
* modify:
*
***********************************************************/
#ifndef _GPN_MEM_USER_SUP_H_
#define _GPN_MEM_USER_SUP_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSupport/gpnSupport/gpnMemorySmpMgt.h"

#ifndef GPN_MEM_MGT_USER_SUP
//error;
#endif

typedef enum _emGpnAlmSupMt_
{
	/* used for gpn_alarm alarm type struct , must always be first 
	    and should be GPN_MEM_TYPE_USER_SUP_MIN(1025)    */
	GPN_ALM_MEM_TP_ALMST = GPN_MEM_TYPE_USER_SUP_MIN,
	/* used for gpn_alarm event type struct */
	GPN_ALM_MEM_TP_EVTST,

	/* add for other type  ... ... ...*/

	/* must be GPN_MEM_MGT_USER_SUP memory type last &
	    should be largest & less than GPN_MEM_TYPE_USER_SUP_MAX(2048)
	    & must named GPN_MTYPE_USER_MAX */
	GPN_MTYPE_USER_MAX
}emGpnAlmSupMt;

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_MEM_USER_SUP_H_*/

