/**********************************************************
* file name: gpnSockCoprocMsgDef.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-01-22
* function: 
*    define details about communication between gpn_coproc modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_COPROC_MSG_DEF_H_
#define _GPN_SOCK_COPROC_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnSockMsgDef.h"
#include "socketComm/gpnSelectOpt.h"


/*==========================================================*/
/*==========================================================*/
/*define main type  (from 1 to 0x7E)  : 1                                                                                   */
/*******************************************************************************/
/*this main type include normal comm msg used by gpn_syn                                                */
#define GPN_COPROC_MT_EMD_OPT					GPN_SOCK_MSG_MT_CREAT(1)

/****************************************************************************************/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)	: 1 									     */
/****************************************************************************************/
#define GPN_COPROC_MST_EMD_GET					GPN_SOCK_MSG_MST_CREAT(1)
/****************************************************************************************/
/*define main type 1's main-sub type 2's sub-sub type  (from 1 to 0xFE)  :                                                   */
/****************************************************************************************/
	#define GPN_COPROC_SST_xxxx_STAT_GET			GPN_SOCK_MSG_SST_CREAT(1)


/****************************************************************************************/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)	: 1 										 */
/****************************************************************************************/
#define GPN_COPROC_MST_EMD_SET					GPN_SOCK_MSG_MST_CREAT(2)
/****************************************************************************************/
/*define main type 1's main-sub type 2's sub-sub type  (from 1 to 0xFE)  :                                                   */
/****************************************************************************************/
	#define GPN_COPROC_SST_SET_TEST					GPN_SOCK_MSG_SST_CREAT(1)





#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_COPROC_MSG_DEF_H_*/


