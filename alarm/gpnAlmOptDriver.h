/**********************************************************
* file name: gpnAlmOptDriver.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-25
* function: 
*    define alarm driver process
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_OPT_DRVER_H_
#define _GPN_ALM_OPT_DRVER_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "type_def.h"

#define GPN_ALM_OPTD_OK 							GEN_SYS_OK
#define GPN_ALM_OPTD_ERR 							GEN_SYS_ERR

#define GPN_ALM_OPTD_ENABLE 						GEN_SYS_ENABLE
#define GPN_ALM_OPTD_DISABLE 						GEN_SYS_DISABLE

#define GPN_ALM_OPTD_YES 							GEN_SYS_YES
#define GPN_ALM_OPTD_NO 							GEN_SYS_NO

#define GPN_ALM_OPTD_08_FFFF						GEN_SYS_08_FFFF
#define GPN_ALM_OPTD_16_FFFF						GEN_SYS_16_FFFF
#define GPN_ALM_OPTDT_32_FFFF						GEN_SYS_32_FFFF
/*#define GPN_ALM_OPTD_64_FFFF						GEN_SYS_64_FFFF*/

#define GPN_ALM_OPTD_PRINT(level, info...) 			GEN_SYS_DEBUG_PRINT((level), info)
#define GPN_ALM_OPTD_EGP 							GEN_SYS_DEBUG_EGP
#define GPN_ALM_OPTD_SVP 							GEN_SYS_DEBUG_SVP
#define GPN_ALM_OPTD_CMP 							GEN_SYS_DEBUG_CMP
#define GPN_ALM_OPTD_CUP 							GEN_SYS_DEBUG_CUP
#define GPN_ALM_OPTD_UD5							GEN_SYS_DEBUG_UD5
#define GPN_ALM_OPTD_UD4							GEN_SYS_DEBUG_UD4
#define GPN_ALM_OPTD_UD3							GEN_SYS_DEBUG_UD3
#define GPN_ALM_OPTD_UD2							GEN_SYS_DEBUG_UD2
#define GPN_ALM_OPTD_UD1							GEN_SYS_DEBUG_UD1
#define GPN_ALM_OPTD_UD0							GEN_SYS_DEBUG_UD0


UINT32 gpnAlmOptBuzzerEn(UINT32 opt);
UINT32 gpnAlmOptBuzzerClr(UINT32 opt);
UINT32 gpnAlmOptBuzzerThre(UINT32 opt);

UINT32 gpnAlmOptLedEn(UINT32 emergNum, UINT32 severeNum, UINT32 commNum, UINT32 curNum);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_ALM_OPT_DRVER_H_ */

