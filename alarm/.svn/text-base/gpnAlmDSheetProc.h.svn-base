/**********************************************************
* file name: gpnAlmDSPorc.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-13
* function: 
*    define alarm data sheet struct and operation
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_DS_PROC_H_
#define _GPN_ALM_DS_PROC_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "socketComm/gpnCommList.h"
#include "gpnAlmDataSheet.h"

#include "type_def.h"
#include "alarm_debug.h"

#define GPN_ALM_DSP_OK 						GEN_SYS_OK
#define GPN_ALM_DSP_ERR 					GEN_SYS_ERR

#define GPN_ALM_DSP_YES 					GEN_SYS_OK
#define GPN_ALM_DSP_NO 						GEN_SYS_ERR


UINT32 gpnAlmDspXDbCycModeChg(UINT32 dbType, UINT32 cycMode);
UINT32 gpnAlmDspSubAlmRankChg(UINT32 index, UINT32 almRank);
UINT32 gpnAlmDspSubAlmAriseCountClear(UINT32 index);
UINT32 gpnAlmDspTimeChangeProc(INT32 offset);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_DS_PROC_H_*/

