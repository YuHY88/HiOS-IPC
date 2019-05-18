/**********************************************************
* file name: gpnMemUserSup.c
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2015-05-27
* function: 
*    define gpn_alarm about memory management
* modify:
*
***********************************************************/
#ifndef _GPN_MEM_USER_SUP_C_
#define _GPN_MEM_USER_SUP_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "gpnMemUserSup.h"

stGpnMemObjTemp gUserMemType[GPN_MTYPE_USER_MAX-GPN_MEM_TYPE_USER_SUP_MIN] =
{
	{GPN_ALM_MEM_TP_ALMST,		"gpn_alm_alm"},
	{GPN_ALM_MEM_TP_EVTST,		"gpn_alm_evt"},
	
};

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_MEM_USER_SUP_C_*/

