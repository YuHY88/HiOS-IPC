/**********************************************************
* file name: gpnSockInclude.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-26
* function: 
*    define this file for other file #inlclude
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_INCLUDE_H_
#define _GPN_SOCK_INCLUDE_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "socketComm/gpnAlmTypeDef.h"
#include "socketComm/gpnGlobalPortIndexDef.h"
#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnSockMsgDef.h"
#include "socketComm/gpnSockAlmMsgDef.h"
#include "socketComm/gpnSockIfmMsgDef.h"
#include "socketComm/gpnSockInitdMsgDef.h"
#include "socketComm/gpnSockCommModuDef.h"
#include "socketComm/gpnSockAliveMan.h"
#include "socketComm/gpnSocketMan.h"
#include "socketComm/gpnTimerService.h"
#include "socketComm/gpnSelectOpt.h"
#include "socketComm/gpnSockCommRoleMan.h"

#include "socketComm/gpnSockCommFuncApi.h"
#include "gpnLog/gpnLogFuncApi.h"
#include "socketComm/gpnDebugFuncApi.h"

#define GPN_SOCKET_COMM_GEN_OK 		GPN_SOCK_SYS_OK
#define GPN_SOCKET_COMM_GEN_ERR 	GPN_SOCK_SYS_ERR
/*------------------------------------------------------------------*/
/*Socket communication head file level                          	*/
/*top		gpnSockInclude.h										*/
/*001		gpnCommRoleMan.h										*/
/*002		gpnSelectOpt.h											*/
/*003											gpnTimerService.h	*/
/*004							gpnSocketMan.h						*/
/*005		gpnSockAliveMan.h										*/
/*006				 gpnAlmGlobalPortIndexDef.h/gpnSockCommModuDef.h*/
/*bottom	gpnSockTypeDef.h/gpnSockMsgDef.h/gpnAlmTypeDef.h		*/
/*------------------------------------------------------------------*/

/*===============================================================================================*/
/*消息处理函数定义--ALM  MOUDLE*/
/*===============================================================================================*/


/*===============================================================================================*/
/*消息处理函数定义--XXX  MOUDLE*/
/*===============================================================================================*/

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_MSG_DEF_H_*/

