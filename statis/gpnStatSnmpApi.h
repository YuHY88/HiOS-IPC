/**********************************************************
* file name: gpnStatSnmpApi.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-17
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_SNMP_API_H_
#define _GPN_STAT_SNMP_API_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>

#include "type_def.h"
#include "stat_debug.h"

#define GPN_STAT_SNMP_GEN_OK 					GEN_SYS_OK
#define GPN_STAT_SNMP_GEN_ERR 					GEN_SYS_ERR

#define GPN_STAT_SNMP_GEN_YES 					GEN_SYS_YES
#define GPN_STAT_SNMP_GEN_NO					GEN_SYS_NO

#define GPN_STAT_SNMP_GEN_ENABLE 				GEN_SYS_ENABLE
#define GPN_STAT_SNMP_GEN_DISABLE				GEN_SYS_DISABLE

#define GPN_STAT_SNMP_PRINT(level, info...) 	GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_STAT_SNMP_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_STAT_SNMP_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_STAT_SNMP_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_STAT_SNMP_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_STAT_SNMP_UD5						GEN_SYS_DEBUG_UD5
#define GPN_STAT_SNMP_UD4						GEN_SYS_DEBUG_UD4
#define GPN_STAT_SNMP_UD3						GEN_SYS_DEBUG_UD3
#define GPN_STAT_SNMP_UD2						GEN_SYS_DEBUG_UD2
#define GPN_STAT_SNMP_UD1						GEN_SYS_DEBUG_UD1
#define GPN_STAT_SNMP_UD0						GEN_SYS_DEBUG_UD0

UINT32 gpnStatSnmpApiStatScanTypeGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiStatScanTypeGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiSubStatTypeGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiSubStatTypeGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiPortType2ScanTypeGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiPortType2ScanTypeGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatAgentXIoCtrl(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiTaskGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiTaskGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiTaskGetNext_2(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);

UINT32 gpnStatSnmpApiEventThredTpGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiEventThredTpGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiAlarmThredTpGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiAlarmThredTpGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiSubFiltTpGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiSubFiltTpGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiSubReportTpGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiSubReportTpGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiPortStatMonCfgGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiPortStatMonCfgGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiCurrDataBaseSubTypeGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiCurrDataBaseSubTypeGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiHistDataBaseSubTypeGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnStatSnmpApiHistDataBaseSubTypeGetNext(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_STAT_SNMP_API_H_*/


