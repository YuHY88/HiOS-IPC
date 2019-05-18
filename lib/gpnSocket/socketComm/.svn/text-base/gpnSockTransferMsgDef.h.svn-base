/**********************************************************
* file name: gpnSockTransferMsgDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-25
* function: 
*    define details about communication between gpn_transfer modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_TRANSFER_MSG_DEF_H_
#define _GPN_SOCK_TRANSFER_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnSockMsgDef.h"
#include "lib/gpnSocket/socketComm/gpnSelectOpt.h"

/*====================================================================================*/
/*GPN_TRANSFER MSG DEFINE                                                                                                                                                              */
/*====================================================================================*/

/*****************************************/
/*define owner : GPN_SOCK_OWNER_TRANSFER  */
/*****************************************/

/*================================================================*/
/*define main type  (from 1 to 0x7E)  : 1                                                                                                  */
/***************************************************************************************/
/*this main type include GPN_TRANSFER function test about msg                                                                */
#define GPN_TRANS_MT_COMM_TEST				((1 << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
/***************************************************************************** **********/
/*define main type 1's sub type  (from 1 to 0xFE)  :                                                                                    */
/****************************************************************************************/
#define GPN_TRANS_ST_CPU_COMM_TEST			((1 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)

#define GPN_TRANS_MSG_CPU_COMM_TEST			((GPN_SOCK_OWNER_TRANSFER)|GPN_TRANS_MT_COMM_TEST|(GPN_TRANS_ST_CPU_COMM_TEST))	/**/
/* msg parameters direction: 
			iIndex         = tx_Index++
			iMsgType    = GPN_TRANS_MSG_CPU_COMM_TEST
			iSrcId         = gpn_transfer(slot)
			iDstId         = gpn_transfer(slot)
			iMsgPara1   = src slot
			iMsgPara2   = dest slot
			msgCellLen = 0

*/
#define GPN_TRANS_MSG_CPU_COMM_TEST_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_TRANS_MSG_CPU_COMM_TEST))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_TRANS_MSG_CPU_COMM_TEST_ACK
			iSrcId	   = other modu
			iDstId	   = CFGMGT
*/

void gpnSockSpMsgTxCPUCommTest(UINT32 dstRole, UINT32 dstSlot);
void gpnSockSpMsgTxNMCommTest(UINT32 dstRole, UINT32 rsv1);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_TRANSFER_MSG_DEF_H_*/


