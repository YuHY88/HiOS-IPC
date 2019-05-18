/**********************************************************
* file name: gpnDevCoreInfoFunApi.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-09-02
* function: 
*    define GPN_DEV_INFO comm API
* modify:
*
***********************************************************/
#ifndef _GPN_DEV_CORE_INFO_FUN_API_H_
#define _GPN_DEV_CORE_INFO_FUN_API_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnDebugFuncApi.h"

#include "devCoreInfo/gpnDevCoreInfo.h"

#define GPN_DEV_CI_API_GEN_OK 					GPN_SOCK_SYS_OK
#define GPN_DEV_CI_API_GEN_ERR 					GPN_SOCK_SYS_ERR

#define GPN_DEV_CI_API_GEN_YES					GPN_SOCK_SYS_YES
#define GPN_DEV_CI_API_GEN_NO 					GPN_SOCK_SYS_NO

#define GPN_DEV_CI_API_PRINT(level, info...) 	GPN_DEBUG_FUNC_API_PRINT((level), info)
#define GPN_DEV_CI_API_EGP                      GPN_DEBUG_FUNC_API_EGP
#define GPN_DEV_CI_API_SVP                      GPN_DEBUG_FUNC_API_SVP
#define GPN_DEV_CI_API_CMP                      GPN_DEBUG_FUNC_API_CMP
#define GPN_DEV_CI_API_CUP                      GPN_DEBUG_FUNC_API_CUP
#define GPN_DEV_CI_API_UD5                      GPN_DEBUG_FUNC_API_UD5
#define GPN_DEV_CI_API_UD4                      GPN_DEBUG_FUNC_API_UD4
#define GPN_DEV_CI_API_UD3                      GPN_DEBUG_FUNC_API_UD3
#define GPN_DEV_CI_API_UD2                      GPN_DEBUG_FUNC_API_UD2
#define GPN_DEV_CI_API_UD1                      GPN_DEBUG_FUNC_API_UD1
#define GPN_DEV_CI_API_UD0                      GPN_DEBUG_FUNC_API_UD0

UINT32 gpnDevApiGetDevIndex(UINT32 *pdevIndex);
UINT32 gpnDevApiGetDevType(UINT32 *devType, char *devStrType, UINT32 strLen);
UINT32 gpnDevApiGetSelfSoltNum(void);
UINT32 gpnDevApiGetNMxSoltNum(UINT32 *pNMaSlot, UINT32 *pNMbSlot);
UINT32 gpnDevApiGetProtocolMac(char *mac, UINT32 len);
UINT32 gpnDevApiSaveProtocolMac(char *mac);
UINT32 gpnDevApiGetProtocolMacFromFile(char *mac, UINT32 len);
UINT32 gpnDevApiIsNMaSlot(void);
UINT32 gpnDevApiIsNMbSlot(void);
UINT32 gpnDevApiIsNMxSlot(void);
UINT32 gpnDevApiIsSubSlot(void);
UINT32 gpnDevApiGetSlotIp(UINT32 slot, UINT32 *ip, UINT16 *pUPort, UINT32 protNum);
UINT32 gpnDevApiGetDevFixPortInfo(UINT32 *pportTpNum, stIfInfo *pstIfInfo, UINT32 pstIfInfoSpSize);
UINT32 gpnDevApiGetMgtInbandIp(UINT32 *inbandIp);
UINT32 gpnDevApiSaveMgtInbandIp(UINT32 inbandIp);
UINT32 gpnDevApiSaveMgtL2Info(UINT32 inbandVlan, UINT32 ib1QPri, UINT8 *ibMac);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_DEV_CORE_INFO_FUN_API_H_ */


