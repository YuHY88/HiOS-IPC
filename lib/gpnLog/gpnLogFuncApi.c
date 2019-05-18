/**********************************************************
* file name: gpnLogFuncApi.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-29
* function: 
*    define gpn_log apply in xxx module
* modify:
*
***********************************************************/
#ifndef _GPN_LOG_FUNC_API_C_
#define _GPN_LOG_FUNC_API_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdarg.h>

/*log function include*/
#include "gpnLog/gpnLogFuncApi.h"
#include "gpnLog/gpnLogUnifyPath.h"

extern stGpnLogInfo *pgGpnLogInfo;

UINT32 gpnLogFuncApiLogEnable(char *token, UINT32 logType, UINT8 high_pri, UINT8 low_pri)
{
	UINT32 reVal;

	if(pgGpnLogInfo == NULL)
	{
		gpnLogInit();
		gpnLogUPPathInit();
	}

	reVal = gpnLogFuncEnable(token, logType, high_pri, low_pri);
	return reVal;
}

UINT32 gpnLogFuncApiLogDisable(char *token, UINT32 logType)
{
	UINT32 reVal;

	if(pgGpnLogInfo == NULL)
	{
		gpnLogInit();
		gpnLogUPPathInit();
	}

	reVal = gpnLogFuncDisable(token, logType);
	return reVal;
}

UINT32 gpnLog(UINT8 priority, const INT8 *format, ...)
{
	UINT32 reVal;
    va_list ap;

	if(pgGpnLogInfo == NULL)
	{
		return GPN_LOG_FUNC_API_GEN_ERR;
	}
	
    va_start(ap, format);

    reVal = gpnVLog(priority, format, ap);
    va_end(ap);
	
    return reVal;
}
UINT32 gpnPerrorLog(const INT8 *errFun)
{
    INT8 *error = strerror(errno);

	if(pgGpnLogInfo == NULL)
	{
		return GPN_LOG_FUNC_API_GEN_ERR;
	}
	
    if(errFun)
	{
        if(error)
        {
            gpnLog(GPN_LOG_L_ERR, "%s: %s\n", errFun, error);
        }
        else
        {
            gpnLog(GPN_LOG_L_ERR, "%s: Error %d out-of-range\n", errFun, errno);
        }
    }
	else
	{
        if(error)
        {
            gpnLog(GPN_LOG_L_ERR, "%s\n", error);
        }
        else
        {
            gpnLog(GPN_LOG_L_ERR, "Error %d out-of-range\n", errno);
        }
    }

	return GPN_LOG_FUNC_API_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_LOG_FUNC_API_C_*/

