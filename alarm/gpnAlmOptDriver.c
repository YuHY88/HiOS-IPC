/**********************************************************
* file name: gpnAlmOptDriver.c
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-10-25
* function: 
*    define alarm driver process
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_OPT_DRVER_C_
#define _GPN_ALM_OPT_DRVER_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <lib/syslog.h>
#include <lib/log.h>

#include "stdio.h"
#include "gpnAlmOptDriver.h"
#include "socketComm/gpnSockAlmMsgDef.h"

UINT32 gpnAlmOptLedEn(UINT32 emergNum, UINT32 severeNum, UINT32 commNum, UINT32 curNum)
{
	zlog_debug(0, "%s : alarm num base rank emerg(%d) severe(%d) severe(%d) cur(%d)\n\r",
		__FUNCTION__, emergNum, severeNum, commNum, curNum);
	
	gpnSockAlmMsgTxAlmRankNumNotify(emergNum, severeNum, commNum, curNum);

	return GPN_ALM_OPTD_OK;
}

UINT32 gpnAlmOptBuzzerEn(UINT32 opt)
{
	zlog_debug(0, "%s : config buzzer %d\n\r",
		__FUNCTION__, opt);
	
	gpnSockAlmMsgTxBellSwitchCfg(opt);
	
	return GPN_ALM_OPTD_OK;
}

UINT32 gpnAlmOptBuzzerClr(UINT32 opt)
{
	zlog_debug(0, "%s : clear buzzer %d\n\r",
		__FUNCTION__, opt);
	
	return GPN_ALM_OPTD_OK;
}

UINT32 gpnAlmOptBuzzerThre(UINT32 opt)
{
	zlog_debug(0, "%s : config buzzer threshold %d\n\r",
		__FUNCTION__, opt);
	
	return GPN_ALM_OPTD_OK;
}


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_ALM_OPT_DRVER_C_ */

