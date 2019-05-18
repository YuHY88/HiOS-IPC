/**********************************************************
* file name: gpnAlmPortNotify.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-12
* function: 
*    define alarm port about message process details
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_PORT_NOTIFY_H_
#define _GPN_ALM_PORT_NOTIFY_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "socketComm/gpnGlobalPortIndexDef.h"

#include "gpnAlmScan.h"

#include "type_def.h"


#define GPN_ALM_PNT_PRINT(level, info...) 		GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_ALM_PNT_EGP 						GEN_SYS_DEBUG_EGP
#define GPN_ALM_PNT_SVP 						GEN_SYS_DEBUG_SVP
#define GPN_ALM_PNT_CMP 						GEN_SYS_DEBUG_CMP
#define GPN_ALM_PNT_CUP 						GEN_SYS_DEBUG_CUP
#define GPN_ALM_PNT_UD5						 	GEN_SYS_DEBUG_UD5
#define GPN_ALM_PNT_UD4							GEN_SYS_DEBUG_UD4
#define GPN_ALM_PNT_UD3							GEN_SYS_DEBUG_UD3
#define GPN_ALM_PNT_UD2							GEN_SYS_DEBUG_UD2
#define GPN_ALM_PNT_UD1							GEN_SYS_DEBUG_UD1
#define GPN_ALM_PNT_UD0							GEN_SYS_DEBUG_UD0


#define GPN_ALM_PNT_08_NULL						GEN_SYS_08_NULL
#define GPN_ALM_PNT_16_NULL						GEN_SYS_16_NULL
#define GPN_ALM_PNT_32_NULL						GEN_SYS_32_NULL
/*#define GPN_ALM_PNT_64_NULL						GEN_SYS_64_NULL*/

#define GPN_ALM_PNT_08_FFFF						GEN_SYS_08_FFFF
#define GPN_ALM_PNT_16_FFFF						GEN_SYS_16_FFFF
#define GPN_ALM_PNT_32_FFFF						GEN_SYS_32_FFFF
/*#define GPN_ALM_PNT_64_FFFF						GEN_SYS_64_FFFF*/

UINT32 gpnAlmPNtPeerPortReplace(optObjOrient *pOldPeerP, optObjOrient *pNewPeerP);
UINT32 gpnAlmPNtViewPortReplace(objLogicDesc *pLocalPort, objLogicDesc *pNewViewP);

UINT32 gpnAlmPNtVariPortUserNotify(objLogicDesc *pPortInfo);
UINT32 gpnAlmPNtVariPortReg(objLogicDesc *pPortInfo);
UINT32 gpnAlmPNtVariPortUnReg(objLogicDesc *pPortInfo);
UINT32 gpnAlmPNtAllPortUnReg(UINT32 devIndex);
UINT32 debugGpnAlmPNtVariPreScanPrint(void);

UINT32 gpnAlmPNtFixPortUserNotify(UINT32 devIndex, UINT32 devPortTpNum, stAlmPortObjCL *pPortObjCL);
UINT32 gpnAlmPNtFixPortReg(objLogicDesc *pPortInfo);
UINT32 gpnAlmPNtPortReg(objLogicDesc *pPortInfo, UINT32 mode);
UINT32 debugGpnAlmPNtFixPreScanPrint(void);
UINT32 debugGpnAlmPNtPreScanPrint(UINT32 mode);


UINT32 gpnAlmPNtDevStaNotify(UINT32 devIndex, objLogicDesc *pgodPortIndex, UINT32 sta);
UINT32 gpnAlmPNtDevStaGet(UINT32 devIndex, objLogicDesc *pgodPortIndex, UINT32 *devState);
UINT32 gpnAlmPNtDevIsInsert(UINT32 devIndex);
UINT32 debugGpnAlmPNtDevPrint(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif/* _GPN_ALM_PORT_NOTIFY_H_ */

