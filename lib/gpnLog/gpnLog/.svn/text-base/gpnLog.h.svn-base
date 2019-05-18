/**********************************************************
* file name: gpnLog.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-10-30
* function: 
*    define GPN_LOG method
* modify:
*
***********************************************************/
#ifndef _GPN_LOG_H_
#define _GPN_LOG_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/time.h>
#include "lib/gpnSocket/socketComm/gpnCommList.h"
#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnDebugFuncApi.h"


#define GPN_LOG_OK 								GPN_SOCK_SYS_OK
#define GPN_LOG_ERR 							GPN_SOCK_SYS_ERR

#define GPN_LOG_YES 							GPN_SOCK_SYS_YES
#define GPN_LOG_NO 								GPN_SOCK_SYS_NO

#define GPN_LOG_ENABLE 							GPN_SOCK_SYS_ENABLE
#define GPN_LOG_DISABLE 						GPN_SOCK_SYS_DISABLE

#define GPN_LOG_PRINT(level, info...)    		GPN_DEBUG_FUNC_API_PRINT((level), info)
#define GPN_LOG_EGP                      		GPN_DEBUG_FUNC_API_EGP
#define GPN_LOG_SVP                      		GPN_DEBUG_FUNC_API_SVP
#define GPN_LOG_CMP                      		GPN_DEBUG_FUNC_API_CMP
#define GPN_LOG_CUP                      		GPN_DEBUG_FUNC_API_CUP
#define GPN_LOG_UD5                      		GPN_DEBUG_FUNC_API_UD5
#define GPN_LOG_UD4                      		GPN_DEBUG_FUNC_API_UD4
#define GPN_LOG_UD3                      		GPN_DEBUG_FUNC_API_UD3
#define GPN_LOG_UD2                      		GPN_DEBUG_FUNC_API_UD2
#define GPN_LOG_UD1                      		GPN_DEBUG_FUNC_API_UD1
#define GPN_LOG_UD0                      		GPN_DEBUG_FUNC_API_UD0

#ifndef GPN_LOG_ERR_LEVEL
#define GPN_LOG_L_DEBUG       					0 /* debug-level messages */
#define GPN_LOG_L_INFO        					1 /* informational */
#define GPN_LOG_L_NOTICE      					2 /* normal but significant condition */
#define GPN_LOG_L_WARNING     					3 /* warning conditions */
#define GPN_LOG_L_ERR         					4 /* error conditions */
#define GPN_LOG_L_CRIT        					5 /* critical conditions */
#define GPN_LOG_L_ALERT       					6 /* action must be taken immediately */
#define GPN_LOG_L_EMERG       					7 /* system is unusable */
#define GPN_LOG_L_ALL       					8 /* for process ctrl */
#endif

#ifndef GPN_LOG_METHOD_DEF
#define GPN_LOGHANDLER_STDOUT					0
#define GPN_LOGHANDLER_STDERR					1
#define GPN_LOGHANDLER_BFILE					2
#define GPN_LOGHANDLER_LFILE					3
#define GPN_LOGHANDLER_SYSLOG					4
#define GPN_LOGHANDLER_APPLOG					5
#define GPN_LOGHANDLER_CALLBACK					6
#define GPN_LOGHANDLER_NONE						7
#endif

#ifndef GPN_DEFAULTB_LOG_ID
/*#define GPN_DEFAULTB_LOG_ID						"/var/log/gpnDefBlog"*/
#define GPN_DEFAULTB_LOG_ID						"/home/gpnDefBlog"
#endif

#ifndef GPN_DEFAULTL_LOG_ID
#define GPN_DEFAULTL_LOG_ID						"/home/gpnDefLlog"
#endif
#define GPN_LOG_FILE_BLOCK_NUM					64
#define GPN_LOG_FILE_BLOCK_SIZE					4096
#define GPN_LOG_FILE_BLCK_MARK					"!@#$%^&*\n\r"
#define GPN_LOG_FILE_BLCK_FREE_MARK				"12345678\n\r"
#define GPN_LOG_FILE_BLCK_MARK_SIZE				10
#define GPN_LOG_FILE_BLOCK_BUFF_SIZE			((GPN_LOG_FILE_BLOCK_SIZE)+(GPN_LOG_FILE_BLCK_MARK_SIZE))
#define GPN_LOG_FILE_BLOCK_TEXT_SIZE			((GPN_LOG_FILE_BLOCK_SIZE)-(GPN_LOG_FILE_BLCK_MARK_SIZE))

#define GPN_LOG_FILE_VLINE_NUM					((GPN_LOG_FILE_BLOCK_SIZE)/(GPN_LOG_FILE_VLINE_SIZE))
#define GPN_LOG_FILE_VLINE_SIZE					128
#define GPN_LOG_FILE_VLINE_MARK					"*&^%$#@!\n\r"
#define GPN_LOG_FILE_VLINE_MARK_SIZE			10

#define GPN_LOG_FILE_TIME_STAMP_LEN				30
#define GPN_LOG_TEXT_MAX_LINE					256
#define GPN_LOG_FILE_MAX_LINE					((GPN_LOG_FILE_TIME_STAMP_LEN)+(64/*token-lenth*/)+(10/*mark-lenth*/)+(GPN_LOG_TEXT_MAX_LINE))
#define GPN_LOG_FILE_SIZE						((GPN_LOG_FILE_BLOCK_SIZE)*(GPN_LOG_FILE_BLOCK_NUM))

#define GPN_LOG_FILE_LINE_SIZE					GPN_LOG_FILE_VLINE_SIZE
#define GPN_LOG_FILE_LINE_TOTOL					((GPN_LOG_FILE_SIZE)/(GPN_LOG_FILE_LINE_SIZE))
#define GPN_LOG_FILE_LINE_MARK					GPN_LOG_FILE_VLINE_MARK
#define GPN_LOG_FILE_LINE_MARK_SIZE				GPN_LOG_FILE_VLINE_MARK_SIZE

#define GPN_LOG_BLOCK_FIFO						1
#define GPN_LOG_LINE_FIFO						2
#define GPN_LOG_NONE_FIFO						3

#define GPN_LOG_MAX_TOKEN						128
#define GPN_LOG_METHOD_MAX_NODE					10

struct _gpnLogMethod_;
typedef UINT32 (*gpnLogHandler)(struct _gpnLogMethod_*, UINT8, const INT8*);

typedef struct _stRecordInfo_
{
	UINT32 blockTh;
	UINT32 buffUse;
	UINT32 buffWtStar;
	INT8 bkBuff[GPN_LOG_FILE_BLOCK_BUFF_SIZE];
}stRecordInfo;

typedef struct _gpnLogMethod_
{
	NODE node;

	UINT8 type;
	UINT8 enable;

	UINT8 min_pri;
	UINT8 max_pri;

	UINT32 magic;
	UINT32 imagic;
	
	gpnLogHandler handler;

	/* each log method correspond an exclusive log file */
	FILE *fd;
	INT8 token[GPN_LOG_MAX_TOKEN];
	stRecordInfo reckInfo;

	INT8 pcName[GPN_LOG_MAX_TOKEN];
}gpnLogMethod;

typedef struct _stgpnLogInfo_
{
	LIST gpnMUseList;
	UINT32 useNum;
	
	LIST gpnMIdleList;
	UINT32 idleNum;

	INT8 pcName[GPN_LOG_MAX_TOKEN];
	gpnLogMethod gLogM[GPN_LOG_METHOD_MAX_NODE];

}stGpnLogInfo;

UINT32 gpnLogInit(void);
UINT32 gpnLogFuncEnable(char *token, UINT32 logType, UINT8 high_pri, UINT8 low_pri);
UINT32 gpnLogFuncDisable(char *token, UINT32 logType);
UINT32 gpnVLog(UINT8 priority, const INT8 *format, va_list ap);
UINT32 debugLogGlobalInfo(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_LOG_H_*/

