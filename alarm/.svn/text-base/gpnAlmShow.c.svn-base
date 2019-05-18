/**********************************************************
* file name: gpnAlmShow.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-06-30
* function: 
*    define alarm show proc Api
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_SHOW_C_
#define _GPN_ALM_SHOW_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "gpnAlmShow.h"

#include "gpnAlmScan.h"

#include "gpnAlmUnifyPath.h"
#include "gpnAlmDataSheet.h"


UINT32 ALM_SUB_TYPE_2_STR(UINT32 almSubType, char *alm_string, UINT32 len)
{
	stAlmSTCharacterDef *pAlmSubTpStr;
	
	/* assert */
	if( (alm_string == NULL) ||\
		(len < GPN_ALM_SUB_TYPE_2_STR_STD_BUFF) )
	{
		return GPN_ALM_SHOW_ERR;
	}

	pAlmSubTpStr = NULL;
	gpnAlmSeekAlmSubTpToAlmSubTpNode(almSubType, &pAlmSubTpStr);
	if(pAlmSubTpStr == NULL)
	{
 		snprintf(alm_string, len, "#(0x00000000)[null] ");
	}
	else
	{
		snprintf(alm_string, len, "#(0x%08X)[%s] ", almSubType, pAlmSubTpStr->almName);
	}

	return GPN_ALM_SHOW_OK;
}
UINT32 EVT_SUB_TYPE_2_STR(UINT32 evtSubType, char *evt_string, UINT32 len)
{
	stEvtSTCharacterDef *pEvtSubTpStr;
	
	/* assert */
	if( (evt_string == NULL) ||\
		(len < GPN_ALM_SUB_TYPE_2_STR_STD_BUFF) )
	{
		return GPN_ALM_SHOW_ERR;
	}

	pEvtSubTpStr = NULL;
	gpnAlmSeekEvtSubTpToEvtSubTpNode(evtSubType, &pEvtSubTpStr);
	if(pEvtSubTpStr == NULL)
	{
 		snprintf(evt_string, len, "#(0x00000000)[null] ");
	}
	else
	{
		snprintf(evt_string, len, "#(0x%08X)[%s] ", evtSubType, pEvtSubTpStr->evtName);
	}

	return GPN_ALM_SHOW_OK;
}

UINT32 gpnAlmCurrAlmShowLineCreat(UINT32 order, UINT32 almSubType,
	objLogicDesc *portIndex, time_t time, char *show_line, UINT32 len)
{
	char *show_p;
	INT32 use_ct;
	INT32 left_ct;
	UINT32 re_val;
	struct tm *tm_info;
	
	/* assert */
	if( (portIndex == NULL) ||\
		(show_line == NULL) ||\
		(len < GPN_ALM_SHOW_INFO_2_STR_STD_BUFF) )
	{
		return GPN_ALM_SHOW_ERR;
	}

	show_p = show_line;
	left_ct = len;
	use_ct = 0;

	/* setp 1 : format order and time */
	tm_info = localtime(&time);
	snprintf(show_p, left_ct, "(%3d) %04d-%02d-%02d %02d:%02d:%02d ",\
		order, tm_info->tm_year+1900, tm_info->tm_mon, tm_info->tm_mday,\
		tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
	
	use_ct = strlen(show_p);
	show_p += use_ct;
	left_ct -= use_ct;

	/* setp 2 : format alm sub type */
	re_val = ALM_SUB_TYPE_2_STR(almSubType, show_p, left_ct);
	if(re_val != GPN_ALM_SHOW_OK)
	{
		return GPN_ALM_SHOW_ERR;
	}

	use_ct = strlen(show_p);
	show_p += use_ct;
	left_ct -= use_ct;

	/* step 3 : format lcation information */
	//re_val = INDEX_2_STR(portIndex, show_p, left_ct);
	re_val = PORT_INDEX_2_STR((optObjOrient *)portIndex, show_p, left_ct);
	if(re_val != GPN_ALM_SHOW_OK)
	{
		return GPN_ALM_SHOW_ERR;
	}

	
	return GPN_ALM_SHOW_OK;
}

UINT32 gpnAlmCurrAlmShowLineWrite(UINT32 order, UINT32 almSubType,
	objLogicDesc *portIndex, time_t time, char *show_line, UINT32 len, struct vty *vty)
{
	UINT32 re_val;
	
	/* assert */
	if( (portIndex == NULL) ||\
		(show_line == NULL) )
	{
		return GPN_ALM_SHOW_ERR;
	}

	re_val = gpnAlmCurrAlmShowLineCreat(order, almSubType, portIndex,
				time, show_line, GPN_ALM_SHOW_INFO_2_STR_STD_BUFF);
	if(re_val != GPN_ALM_SHOW_OK)
	{
		return GPN_ALM_SHOW_ERR;
	}
	vty_out (vty, "%s %s", show_line, VTY_NEWLINE);
	//fwrite(show_line, strlen(show_line), 1, fp);

	return GPN_ALM_SHOW_OK;
}
UINT32 gpnAlmHistAlmShowLineCreat(UINT32 order, UINT32 almSubType,
	objLogicDesc *portIndex, time_t r_time, time_t c_time, char *show_line, UINT32 len)
{
	char *show_p;
	INT32 use_ct;
	INT32 left_ct;
	UINT32 re_val;
	struct tm *tm_info;
	
	/* assert */
	if( (portIndex == NULL) ||\
		(show_line == NULL) ||\
		(len < GPN_ALM_SHOW_INFO_2_STR_STD_BUFF) )
	{
		return GPN_ALM_SHOW_ERR;
	}

	show_p = show_line;
	left_ct = len;
	use_ct = 0;

	/* setp 1 : format order and time */
	tm_info = localtime(&r_time);
	snprintf(show_p, left_ct, "(%3d) (%04d-%02d-%02d %02d:%02d:%02d ",\
		order, tm_info->tm_year+1990, tm_info->tm_mon, tm_info->tm_mday,\
		tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
	
	use_ct = strlen(show_p);
	show_p += use_ct;
	left_ct -= use_ct;

	tm_info = localtime(&c_time);
	snprintf(show_p, left_ct, "-> %04d-%02d-%02d %02d:%02d:%02d) ",\
		tm_info->tm_year+1990, tm_info->tm_mon, tm_info->tm_mday,\
		tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
	
	use_ct = strlen(show_p);
	show_p += use_ct;
	left_ct -= use_ct;

	/* setp 2 : format alm sub type */
	re_val = ALM_SUB_TYPE_2_STR(almSubType, show_p, left_ct);
	if(re_val != GPN_ALM_SHOW_OK)
	{
		return GPN_ALM_SHOW_ERR;
	}

	use_ct = strlen(show_p);
	show_p += use_ct;
	left_ct -= use_ct;

	/* step 3 : format lcation information */
	//re_val = INDEX_2_STR(portIndex, show_p, left_ct);
	re_val = PORT_INDEX_2_STR((optObjOrient *)portIndex, show_p, left_ct);
	if(re_val != GPN_ALM_SHOW_OK)
	{
		return GPN_ALM_SHOW_ERR;
	}

	
	return GPN_ALM_SHOW_OK;
}

UINT32 gpnAlmHistAlmShowLineWrite(UINT32 order, UINT32 almSubType,
	objLogicDesc *portIndex, time_t r_time, time_t c_time, char *show_line, UINT32 len, struct vty *vty)
{
	UINT32 re_val;
	
	/* assert */
	if( (portIndex == NULL) ||\
		(show_line == NULL) )
	{
		return GPN_ALM_SHOW_ERR;
	}

	re_val = gpnAlmHistAlmShowLineCreat(order, almSubType, portIndex,
				r_time, c_time, show_line, GPN_ALM_SHOW_INFO_2_STR_STD_BUFF);
	if(re_val != GPN_ALM_SHOW_OK)
	{
		return GPN_ALM_SHOW_ERR;
	}

	vty_out (vty, "%s %s", show_line, VTY_NEWLINE);
	//fwrite(show_line, strlen(show_line), 1, fp);

	return GPN_ALM_SHOW_OK;
}
UINT32 gpnAlmEventShowLineCreat(UINT32 order, UINT32 eventType,
	objLogicDesc *portIndex, time_t time, UINT32 detail, char *show_line, UINT32 len)
{
	char *show_p;
	INT32 use_ct;
	INT32 left_ct;
	UINT32 re_val;
	struct tm *tm_info;
	
	/* assert */
	if( (portIndex == NULL) ||\
		(show_line == NULL) ||\
		(len < GPN_ALM_SHOW_INFO_2_STR_STD_BUFF) )
	{
		return GPN_ALM_SHOW_ERR;
	}

	show_p = show_line;
	left_ct = len;
	use_ct = 0;

	/* setp 1 : format order and time */
	tm_info = localtime(&time);
	snprintf(show_p, left_ct, "(%3d) %04d-%02d-%02d %02d:%02d:%02d ",\
		order, tm_info->tm_year+1990, tm_info->tm_mon, tm_info->tm_mday,\
		tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
	
	use_ct = strlen(show_p);
	show_p += use_ct;
	left_ct -= use_ct;

	/* setp 2 : format event sub type */
	re_val = EVT_SUB_TYPE_2_STR(eventType, show_p, left_ct);
	if(re_val != GPN_ALM_SHOW_OK)
	{
		return GPN_ALM_SHOW_ERR;
	}

	use_ct = strlen(show_p);
	show_p += use_ct;
	left_ct -= use_ct;

	/* step 3 : format lcation information */
	//re_val = INDEX_2_STR(portIndex, show_p, left_ct);
	re_val = PORT_INDEX_2_STR((optObjOrient *)portIndex, show_p, left_ct);
	if(re_val != GPN_ALM_SHOW_OK)
	{
		return GPN_ALM_SHOW_ERR;
	}

	
	return GPN_ALM_SHOW_OK;
}

UINT32 gpnAlmEventShowLineWrite(UINT32 order, UINT32 eventType,
	objLogicDesc *portIndex, time_t time, UINT32 detail, char *show_line, UINT32 len, struct vty *vty)
{
	UINT32 re_val;
	
	/* assert */
	if( (portIndex == NULL) ||\
		(show_line == NULL) )
	{
		return GPN_ALM_SHOW_ERR;
	}

	re_val = gpnAlmEventShowLineCreat(order, eventType, portIndex,
				time, detail, show_line, GPN_ALM_SHOW_INFO_2_STR_STD_BUFF);
	if(re_val != GPN_ALM_SHOW_OK)
	{
		return GPN_ALM_SHOW_ERR;
	}

	vty_out (vty, "%s %s", show_line, VTY_NEWLINE);
	//fwrite(show_line, strlen(show_line), 1, fp);

	return GPN_ALM_SHOW_OK;
}


UINT32 gpnAlmShowFileCreatTmp(char *path, FILE **p_fp)
{
	char suffix_path[256];

	/* assert */
	if( (path == NULL) ||\
		(p_fp == NULL) )
	{
		return GPN_ALM_SHOW_ERR;
	}

	/* path re-direction : add suffix */
	snprintf(suffix_path, 256, "%s.tmp", path);
	
	/* file creat */
	*p_fp = fopen(suffix_path, "wb");
	if(*p_fp == NULL)
	{
		perror("file open failure!");
		return GPN_ALM_SHOW_ERR;
	}

	return GPN_ALM_SHOW_OK;
}
UINT32 gpnAlmShowFileVisible(char *path, FILE *fp)
{
	char suffix_path[256];
	char mv_cmd[512];

	/* assert */
	if( (path == NULL) ||\
		(fp == NULL) )
	{
		return GPN_ALM_SHOW_ERR;
	}

	/* path re-direction : add suffix */
	snprintf(suffix_path, 256, "%s.tmp", path);

	/* close tmp show file */
	fclose(fp);

	/* mv tmp */
	snprintf(mv_cmd, 512, "mv %s %s", suffix_path, path);
	//snprintf(mv_cmd, 512, "cp %s %s", suffix_path, path);

	system(mv_cmd);

	return GPN_ALM_SHOW_OK;
}

UINT32 gpnAlmShowFileCreat(UINT32 type, char *path)
{
	FILE *fp;
	UINT32 re_val;
	
	/* assert */
	if(path == NULL)
	{
		return GPN_ALM_SHOW_ERR;
	}

	re_val = gpnAlmShowFileCreatTmp(path, &fp);
	if(re_val != GPN_ALM_SHOW_OK)
	{
		return GPN_ALM_SHOW_ERR;
	}

	if(type == GPN_ALM_SHOW_TYOE_CURR)
	{
		//gpnAlmDataSheetCurrAlmViewWrite(fp);
	}
	else if(type == GPN_ALM_SHOW_TYOE_HIST)
	{
		//gpnAlmDataSheetHistAlmViewWrite(fp);
	}
	else if(type == GPN_ALM_SHOW_TYOE_EVENT)
	{
		//gpnAlmDataSheetEventViewWrite(fp);
	}
	else
	{
		return GPN_ALM_SHOW_ERR;
	}

	gpnAlmShowFileVisible(path, fp);

	return GPN_ALM_SHOW_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_SHOW_C_*/

