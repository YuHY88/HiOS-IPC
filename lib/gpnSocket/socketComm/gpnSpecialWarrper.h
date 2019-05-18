/***********************************************************
* file name: gpnSpecialWarrper.h
* Copyright: 
* Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2014-04-01
* function: 
*    define spcial function's gpn-format warrper
* modify:
*
***********************************************************/
#ifndef _GPN_SPCIAL_WARRPER_H_
#define _GPN_SPCIAL_WARRPER_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"

/*************************************************************************/
/*                                                    comm define                                                          */
/*************************************************************************/
#define GPN_SW_OK                               GPN_SOCK_SYS_OK
#define GPN_SW_ERR                              GPN_SOCK_SYS_ERR

#define GPN_SW_YES                              GPN_SOCK_SYS_OK
#define GPN_SW_NO                               GPN_SOCK_SYS_ERR

/*************************************************************************/
/*                                        fget spceial warrper define                                                  */
/*************************************************************************/
#define GPN_FFETS_MAX_AGAIN						200
char *gpnFgets(char *s, INT32 size, FILE *stream);

/*************************************************************************/
/*                              gpn key file version spceial opration define                                     */
/*************************************************************************/
#define GPN_KEY_FILE_VER_DEV_STR_LEN           64
typedef struct _gpnVerTag_
{
	char devStr[GPN_KEY_FILE_VER_DEV_STR_LEN];
	UINT32 version;
}gpnVerTag;

#define GPN_KEY_FILE_LINE_LEN					4096
#define GPN_KEY_FILE_VERSION_LEN				((GPN_KEY_FILE_VER_DEV_STR_LEN)+30)
#define GPN_KEY_FILE_VERSION_CREAT(pDevTag, D, V, C, R, B)	\
{                                                                         \
	snprintf((pDevTag)->devStr, GPN_KEY_FILE_VER_DEV_STR_LEN, "%s", D);     \
	(pDevTag)->version = (((V & 0x000000FF) << 24) |                         \
						((C & 0x000000FF) << 16) |                         \
						((R & 0x000000FF) <<  8) |                         \
						((B & 0x000000FF) <<  0));                         \
}                                                                         \


#define GPN_KEY_FILE_VERSION_V_GET(version)		(((version) >> 24) & 0x000000FF)
#define GPN_KEY_FILE_VERSION_C_GET(version)		(((version) >> 16) & 0x000000FF)
#define GPN_KEY_FILE_VERSION_R_GET(version)		(((version) >>  8) & 0x000000FF)
#define GPN_KEY_FILE_VERSION_B_GET(version)		(((version) >>  0) & 0x000000FF)
#define GPN_KEY_FILE_VERSION_HEAD				"#Version : "
#define GPN_KEY_FILE_VERSION_STR_CREAT(pVerStr, pVerTag) \
{\
	snprintf(pVerStr, GPN_KEY_FILE_VERSION_LEN, "%sD%sV%dC%dR%dB%d\n",\
		GPN_KEY_FILE_VERSION_HEAD,\
		pVerTag->devStr,\
		GPN_KEY_FILE_VERSION_V_GET(pVerTag->version),\
		GPN_KEY_FILE_VERSION_C_GET(pVerTag->version),\
		GPN_KEY_FILE_VERSION_R_GET(pVerTag->version),\
		GPN_KEY_FILE_VERSION_B_GET(pVerTag->version));\
}\

INT32 gpnKeyFileVersionGet(const char *pathname, gpnVerTag *pVerTag);		
INT32 gpnKeyFileVersionCheck(const char *pathname, gpnVerTag *pExpVerTag);

/*************************************************************************/
/*                                   gpn ps grep awk opration about define                                      */
/*************************************************************************/
#define GPN_PS_KEY_MAX_CMD_LEN				1024 
UINT32 gpnFindPsKeyWorkOrder(char *keyWord, UINT32 *order);

/*************************************************************************/
/*                                 gpn porcess unique run check define                                           */
/*************************************************************************/
#define GPN_UCHECK_MAX_NAME_LEN		        256
#define GPN_UCHECK_MAX_CMD_LEN		        256
UINT32 gpnProcessUniqueRunCheck(INT8 *progname);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_SPCIAL_WARRPER_H_*/

