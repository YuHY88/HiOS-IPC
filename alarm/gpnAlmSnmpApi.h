/**********************************************************
* file name: gpnAlmSnmpApi.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-09-18
* function: 
*    define alarm snmp API
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_SNMP_API_H_
#define _GPN_ALM_SNMP_API_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockMsgDef.h"

#include "type_def.h"

#define GPN_ALM_SNMP_OK 							GEN_SYS_OK
#define GPN_ALM_SNMP_ERR 							GEN_SYS_ERR

#define GPN_ALM_SNMP_CANCEL 						0
#define GPN_ALM_SNMP_ENABLE 						GEN_SYS_ENABLE
#define GPN_ALM_SNMP_DISABLE 						GEN_SYS_DISABLE

#define GPN_ALM_SNMP_YES 							GEN_SYS_YES
#define GPN_ALM_SNMP_NO 							GEN_SYS_NO

#define GPN_ALM_SNMP_08_FFFF						GEN_SYS_08_FFFF
#define GPN_ALM_SNMP_16_FFFF						GEN_SYS_16_FFFF
#define GPN_ALM_SNMP_32_FFFF						GEN_SYS_32_FFFF
/*#define GPN_ALM_SNMP_64_FFFF						GEN_SYS_64_FFFF*/

#define GPN_ALM_SNMP_PRINT(level, info...) 			GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_ALM_SNMP_EGP 							GEN_SYS_DEBUG_EGP
#define GPN_ALM_SNMP_SVP 							GEN_SYS_DEBUG_SVP
#define GPN_ALM_SNMP_CMP 							GEN_SYS_DEBUG_CMP
#define GPN_ALM_SNMP_CUP 							GEN_SYS_DEBUG_CUP
#define GPN_ALM_SNMP_UD5							GEN_SYS_DEBUG_UD5
#define GPN_ALM_SNMP_UD4							GEN_SYS_DEBUG_UD4
#define GPN_ALM_SNMP_UD3							GEN_SYS_DEBUG_UD3
#define GPN_ALM_SNMP_UD2							GEN_SYS_DEBUG_UD2
#define GPN_ALM_SNMP_UD1							GEN_SYS_DEBUG_UD1
#define GPN_ALM_SNMP_UD0							GEN_SYS_DEBUG_UD0


UINT32 gpnAlmAgentxIoCtrl(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_ALM_SNMP_API_H__*/
