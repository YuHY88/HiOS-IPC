/**********************************************************
* file name: gpnAlmPowDown.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-17
* function: 
*    define alarm power down porc about
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_POW_DOWN_H_
#define _GPN_ALM_POW_DOWN_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include "type_def.h"


#define GPN_ALM_PD_OK 								GEN_SYS_OK
#define GPN_ALM_PD_ERR 								GEN_SYS_ERR

#define GPN_ALM_PD_CANCEL 							0
#define GPN_ALM_PD_ENABLE 							GEN_SYS_ENABLE
#define GPN_ALM_PD_DISABLE 							GEN_SYS_DISABLE

#define GPN_ALM_PD_YES 								GEN_SYS_YES
#define GPN_ALM_PD_NO 								GEN_SYS_NO

#define GPN_ALM_PD_08_FFFF							GEN_SYS_08_FFFF
#define GPN_ALM_PD_16_FFFF							GEN_SYS_16_FFFF
#define GPN_ALM_PD_32_FFFF							GEN_SYS_32_FFFF
/*#define GPN_ALM_PD_64_FFFF						GEN_SYS_64_FFFF*/


#define GPN_ALM_PD_PRINT(level, info...) 			GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_ALM_PD_EGP 								GEN_SYS_DEBUG_EGP
#define GPN_ALM_PD_SVP 								GEN_SYS_DEBUG_SVP
#define GPN_ALM_PD_CMP 								GEN_SYS_DEBUG_CMP
#define GPN_ALM_PD_CUP 								GEN_SYS_DEBUG_CUP
#define GPN_ALM_PD_UD5								GEN_SYS_DEBUG_UD5
#define GPN_ALM_PD_UD4								GEN_SYS_DEBUG_UD4
#define GPN_ALM_PD_UD3								GEN_SYS_DEBUG_UD3
#define GPN_ALM_PD_UD2								GEN_SYS_DEBUG_UD2
#define GPN_ALM_PD_UD1								GEN_SYS_DEBUG_UD1
#define GPN_ALM_PD_UD0								GEN_SYS_DEBUG_UD0

#define GPN_ALM_PD_EQU_SLOT                         IFM_SLOT_MAX_ID


void gpnAlmPDClearSIGHandler(INT32 sig);
UINT32 gpnAlmPDClearDevPowerFail(void);
UINT32 gpnAlmPDClearDevPowerDown(void);
void gpnAlmPDRiseSIGHandler(INT32 sig);
UINT32 gpnAlmPDRiseDevPowerFail(void);
UINT32 gpnAlmPDRiseDevPowerDown(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_ALM_POW_DOWN_H_ */


