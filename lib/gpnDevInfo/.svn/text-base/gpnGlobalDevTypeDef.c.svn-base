/**********************************************************
* file name: gpnGlobalDevTypeDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-09
* function: 
*    define GPN_DEV_INFO Global type
* modify:
*
***********************************************************/
#ifndef _GPN_GLOBAL_DEV_TYPE_DEF_C_
#define _GPN_GLOBAL_DEV_TYPE_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "devCoreInfo/gpnGlobalDevTypeDef.h"


/* modify for ipran , by lipf, 2018/4/9 */
stGpnDevTypeTp gGpnDevTypeTemplat[] =
{
	{GPN_DEV_TYPE_NULL,                  "NULL"},
	{GPN_DEV_TYPE_HT201,             	 "HT201"}, 		//add for ipran
	{GPN_DEV_TYPE_HT201E,             	 "HT201E"}, 	//add for ipran
	{GPN_DEV_TYPE_H20RN2000,             "H20RN2000"}, 	//add for ipran
	{GPN_DEV_TYPE_HT157,             	 "HT157"}, 		//add for ipran
	{GPN_DEV_TYPE_HT158,             	 "HT158"}, 		//add for ipran
	{GPN_DEV_TYPE_VX,             	 	 "VX"}, 		//add for ipran
	{GPN_DEV_TYPE_ALL,                   "GPN_DEV_ALL"},
};    

UINT32 gpnGlobalDevTpNum(void)
{
	return sizeof(gGpnDevTypeTemplat)/sizeof(gGpnDevTypeTemplat[0]);
}

UINT32 gpnGlobalDevStrType2DevType(char *devStrType, UINT32 *devType)
{
	UINT32 i;
	UINT32 devTypeNum;
	
	/*assert*/
	if( (devStrType == NULL) ||\
		(devType == NULL) )
	{
		GPN_DEV_TYPE_PRINT(GPN_DEV_TYPE_AGP, "%s(%d) : para NULL\n\r", __FUNCTION__, getpid());
		return GPN_DEV_TYPE_GEN_ERR;
	}

	devTypeNum = sizeof(gGpnDevTypeTemplat)/sizeof(gGpnDevTypeTemplat[0]);

	for(i=0;i<devTypeNum;i++)
	{
		if( (strncmp(gGpnDevTypeTemplat[i].devStrType, devStrType,\
				strlen(devStrType)) == 0) &&\
			(strncmp(gGpnDevTypeTemplat[i].devStrType, devStrType,\
				strlen(gGpnDevTypeTemplat[i].devStrType)) == 0) )
		{
			*devType = gGpnDevTypeTemplat[i].devType;
			return GPN_DEV_TYPE_GEN_OK;
		}
	}

	GPN_DEV_TYPE_PRINT(GPN_DEV_TYPE_AGP, "%s(%d) : can not find %s, err!\n\r",\
		__FUNCTION__, getpid(), devStrType);
	return GPN_DEV_TYPE_GEN_ERR;
}

UINT32 gpnGlobalDevType2DevStrType(UINT32 devType, char *devStrType, UINT32 lenth)
{
	UINT32 i;
	UINT32 devTypeNum;
	
	/*assert*/
	if( (devStrType == NULL) ||\
		(lenth < GPN_DEV_TYPE_MAX_TYPE_STR_LEN) )
	{
		GPN_DEV_TYPE_PRINT(GPN_DEV_TYPE_AGP, "%s(%d) : para NULL\n\r", __FUNCTION__, getpid());
		return GPN_DEV_TYPE_GEN_ERR;
	}

	devTypeNum = sizeof(gGpnDevTypeTemplat)/sizeof(gGpnDevTypeTemplat[0]);

	for(i=0;i<devTypeNum;i++)
	{
		if(devType == gGpnDevTypeTemplat[i].devType)
		{
			/*clear buffer first */
			memset(devStrType, 0, lenth);
			snprintf(devStrType, GPN_DEV_TYPE_MAX_TYPE_STR_LEN, "%s",\
				gGpnDevTypeTemplat[i].devStrType);
			return GPN_DEV_TYPE_GEN_OK;
		}
	}

	GPN_DEV_TYPE_PRINT(GPN_DEV_TYPE_AGP, "%s(%d) : can not find devType(%d), err!\n\r",\
		__FUNCTION__, getpid(), devType);
	return GPN_DEV_TYPE_GEN_ERR;
}

char *gpnGlobalDevStrGet(UINT32 devType)
{
	UINT32 i;
	UINT32 devTypeNum;

	devTypeNum = sizeof(gGpnDevTypeTemplat)/sizeof(gGpnDevTypeTemplat[0]);

	for(i=0;i<devTypeNum;i++)
	{
		if(devType == gGpnDevTypeTemplat[i].devType)
		{
			return (char *)(gGpnDevTypeTemplat[i].devStrType);
		}
	}

	/* error type */
	return (char *)(gGpnDevTypeTemplat[GPN_DEV_TYPE_NULL].devStrType);
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_GLOBAL_DEV_TYPE_DEF_C_ */

