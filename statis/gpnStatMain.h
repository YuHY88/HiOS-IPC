/**********************************************************
* file name: gpnStatMain.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-03-11
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_MAIN_H_
#define _GPN_STAT_MAIN_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "type_def.h"
#include "stat_debug.h"

#define GPN_STAT_GEN_OK 			GEN_SYS_OK
#define GPN_STAT_GEN_ERR 			GEN_SYS_ERR

#define GPN_STAT_GEN_YES 			GEN_SYS_YES
#define GPN_STAT_GEN_NO 			GEN_SYS_NO

#define GPN_STAT_GEN_ENABLE 		GEN_SYS_ENABLE
#define GPN_STAT_GEN_DISABLE 		GEN_SYS_DISABLE

UINT32 gpnStatSelfNotifyDevStatus(void);
UINT32 gpnStatSelfNotifyFixPort(void);
UINT32 gpnStatLogInit(void);
INT32 gpnStatStarUp(void);
UINT32 gpnStatHelp(INT32 status, INT8 *progname);
UINT32 gpnStatDaemonize (UINT32 nochdir, UINT32 noclose);
INT32 main(INT32 argc, INT8 **argv);

void statis_die(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_STAT_MAIN_H_*/


