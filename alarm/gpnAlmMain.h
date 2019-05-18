/**********************************************************
* file name: gpnAlmMan.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-08-13
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_MAN_H_
#define _GPN_ALM_MAN_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "type_def.h"
#define GPN_ALM_GEN_OK 		GEN_SYS_OK
#define GPN_ALM_GEN_ERR 	GEN_SYS_ERR


UINT32 gpnAlmLogInit(void);
INT32 gpnAlmStarUp(void);
UINT32 gpnAlmHelp(INT32 status, INT8 *progname);
UINT32 gpnAlmDaemonize (UINT32 nochdir, UINT32 noclose);
INT32 main(INT32 argc, INT8 **argv);

UINT32 gpnAlmSelfNotifyDevStatus(void);
UINT32 gpnAlarmSelfNotifyFixPort(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_ALM_MAN_H_*/

