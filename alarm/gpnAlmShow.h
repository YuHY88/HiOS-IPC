/**********************************************************
* file name: gpnAlmShow.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-06-30
* function: 
*    define alarm show proc Api
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_SHOW_H_
#define _GPN_ALM_SHOW_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>
#include <lib/vty.h>

#include "type_def.h"
#include "socketComm/gpnGlobalPortIndexDef.h"
#include "socketComm/gpnSockAlmMsgDef.h"

#define GPN_ALM_SHOW_OK							GEN_SYS_OK
#define GPN_ALM_SHOW_ERR 						GEN_SYS_ERR

#define GPN_ALM_SHOW_PRINT(level, info...) 		GEN_SYS_PRINT((level), info)
#define GPN_ALM_SHOW_AGP 						GEN_SYS_EGP
#define GPN_ALM_SHOW_SVP 						GEN_SYS_SVP
#define GPN_ALM_SHOW_CMP 						GEN_SYS_CMP
#define GPN_ALM_SHOW_CUP 						GEN_SYS_CUP

#define GPN_ALM_SHOW_TYOE_CURR                  GPN_ALM_MSG_PARA_RESULT_CURR
#define GPN_ALM_SHOW_TYOE_HIST                  GPN_ALM_MSG_PARA_RESULT_HIST
#define GPN_ALM_SHOW_TYOE_EVENT                 GPN_ALM_MSG_PARA_RESULT_EVENT

#define GPN_ALM_SUB_TYPE_2_STR_STD_BUFF         64
#define GPN_ALM_SHOW_INFO_2_STR_STD_BUFF        512

UINT32 ALM_SUB_TYPE_2_STR(UINT32 almSubType, char *alm_string, UINT32 len);
UINT32 EVT_SUB_TYPE_2_STR(UINT32 evtSubType, char *evt_string, UINT32 len);
UINT32 gpnAlmCurrAlmShowLineCreat(UINT32 order, UINT32 almSubType,
	objLogicDesc *portIndex, time_t time, char *show_string, UINT32 len);
UINT32 gpnAlmCurrAlmShowLineWrite(UINT32 order, UINT32 almSubType,
	objLogicDesc *portIndex, time_t time, char *show_line, UINT32 len, struct vty *vty);
UINT32 gpnAlmHistAlmShowLineCreat(UINT32 order, UINT32 almSubType,
	objLogicDesc *portIndex, time_t time, time_t c_time, char *show_string, UINT32 len);
UINT32 gpnAlmHistAlmShowLineWrite(UINT32 order, UINT32 almSubType,
	objLogicDesc *portIndex, time_t s_time, time_t c_time, char *show_line, UINT32 len, struct vty *vty);
UINT32 gpnAlmEventShowLineCreat(UINT32 order, UINT32 eventType,
	objLogicDesc *portIndex, time_t time, UINT32 detail, char *show_line, UINT32 len);
UINT32 gpnAlmEventShowLineWrite(UINT32 order, UINT32 eventType,
	objLogicDesc *portIndex, time_t time, UINT32 detail, char *show_line, UINT32 len, struct vty *vty);
UINT32 gpnAlmShowFileCreatTmp(char *path, FILE **p_fp);
UINT32 gpnAlmShowFileVisible(char *path, FILE *fp);
UINT32 gpnAlmShowFileCreat(UINT32 type, char *path);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_show_H_*/
