/**********************************************************
* file name: gpnSockProtocolMsgDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-06-11
* function: 
*   	define details about gpn_socket protocol msg
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_PROTOCOL_MSG_DEF_H_
#define _GPN_SOCK_PROTOCOL_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnSockMsgDef.h"
#include "socketComm/gpnSelectOpt.h"

/*====================================================================================*/
/*GPN_SOCKET PROTOCOL MSG DEFINE                                                                                                                                                 */
/*====================================================================================*/

/***********************************/
/*define owner : GPN_SOCK_OWNER_SELF*/
/***********************************/

/*================================================================*/
/*define main type  (from 1 to 0x7E)  : 1                                                                                                  */
/***************************************************************************************/
/*this main type include GPN_TIMER timer service about msg                                                                     */
#define GPN_SELF_MT_TIMER_DEF				((1 << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
/***************************************************************************** **********/
/*define main type 1's sub type  (from 1 to 0xFE)  :                                                                                    */
/****************************************************************************************/
#define GPN_SELF_ST_TIMER_BASE				((1 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_10MS				((2 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_20MS				((3 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_30MS				((4 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_40MS				((5 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_50MS				((6 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_100MS				((7 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_200MS				((8 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_300MS				((9 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_400MS				((10 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_500MS				((11 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_1S				((12 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_1500MS			((13 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_2S				((14 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_5S				((15 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_TIMER_10S				((16 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
/****************************************************************************************/

#define GPN_GEN_MSG_TIMER_BASE      		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_BASE))
#define GPN_GEN_MSG_TIMER_10MS      		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_10MS))
#define GPN_GEN_MSG_TIMER_20MS      		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_20MS))
#define GPN_GEN_MSG_TIMER_30MS      		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_30MS))
#define GPN_GEN_MSG_TIMER_40MS      		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_40MS))
#define GPN_GEN_MSG_TIMER_50MS      		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_50MS))
#define GPN_GEN_MSG_TIMER_100MS     		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_100MS))
#define GPN_GEN_MSG_TIMER_200MS     		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_200MS))
#define GPN_GEN_MSG_TIMER_300MS     		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_300MS))
#define GPN_GEN_MSG_TIMER_400MS     		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_400MS))
#define GPN_GEN_MSG_TIMER_500MS     		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_500MS))
#define GPN_GEN_MSG_TIMER_1S    			((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_1S))
#define GPN_GEN_MSG_TIMER_1500MS    		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_1500MS))
#define GPN_GEN_MSG_TIMER_2S        		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_2S))
#define GPN_GEN_MSG_TIMER_5S        		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_5S))
#define GPN_GEN_MSG_TIMER_10S       		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_TIMER_DEF)|(GPN_SELF_ST_TIMER_10S))
/* msg parameters direction:   
			iIndeX	= tx_Index++
			iMsgType   = above msg
                   	iSrcId     	= modu self
                   	iDstId     	= modu self
                   	iMsgPara1  = time bingo num
                   	msgCellLen = 0
*/

/*================================================================*/
/*define main type  (from 1 to 0x7E)  : 2                                                                                                  */
/***************************************************************************************/
/*this main type include GPN_SOCKET protocol detail                                                                                */
#define GPN_SELF_MT_PROTOCOL_PROC			((2 << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
#define GPN_SELF_ST_CMD_RESEND				((1 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_ROLE_INFO_GET			((2 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_COMM_CHECK				((3 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_NEW_CONNCET_NOTIFY		((4 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SELF_ST_BASE_TIMER_SYN		    ((5 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)


/****************************************************************************************/

#define GPN_SELF_MSG_CMD_RESEND  			((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_PROTOCOL_PROC)|(GPN_SELF_ST_CMD_RESEND))
/* msg parameters direction:   
					iIndex	= tx_Index++
					iMsgType  	= GPN_ALM_MSG_CMD_RESEND
					iSrcId	= A modu
					iDstId	= B modu
					iMsgPara1  = cmd index
					msgCellLen= 0
*/
#define GPN_SELF_MSG_CMD_RESEND_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_SELF_MSG_CMD_RESEND))
/* msg parameters direction:   
					iIndex	= CMD index
					iMsgType	= GPN_SELF_MSG_CMD_RESEND_ACK
					iSrcId	= B modu
					iDstId	= A modu

*/

#define GPN_SELF_MSG_ROLE_INFO_GET			((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_PROTOCOL_PROC)|(GPN_SELF_ST_ROLE_INFO_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType  	= GPN_SELF_MSG_ROLE_INFO_GET
					iSrcId	= A modu
					iDstId	= B modu
					msgCellLen = 0
*/
#define GPN_SELF_MSG_ROLE_INFO_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SELF_MSG_ROLE_INFO_GET))
/* msg parameters direction:
					iIndex	   = tx_Index++
					iMsgType   = GPN_SELF_MSG_ROLE_INFO_GET_ACK
					iSrcId	= B modu
					iDstId	= A modu
					msgCellLen = 0
*/
#define  GPN_SELF_MSG_ROLE_INFO_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_SELF_MSG_ROLE_INFO_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType  	= GPN_SELF_MSG_ROLE_INFO_GET_RSP
					iSrcId	= B modu
					iDstId	= A modu
					iMsgPara1  = role
					msgCellLen = 0
*/
#define  GPN_SELF_MSG_ROLE_INFO_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_SELF_MSG_ROLE_INFO_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_SELF_MSG_ROLE_INFO_GET_RSP_ACK
					iSrcId	= A modu
					iDstId	= B modu
					iMsgPara1  = role
					msgCellLen = 0
*/

#define GPN_SELF_MSG_COMM_CHECK  			((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_PROTOCOL_PROC)|(GPN_SELF_ST_COMM_CHECK))
/* msg parameters direction:   
					iIndex	= tx_Index++
					iMsgType  	= GPN_SELF_MSG_COMM_CHECK
					iSrcId	= A modu
					iDstId	= B modu
					iMsgPara1  = cmd index
					msgCellLen= 0
*/
#define GPN_SELF_MSG_COMM_CHECK_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_SELF_MSG_COMM_CHECK))
/* msg parameters direction:   
					iIndex	= CMD index
					iMsgType	= GPN_SELF_MSG_COMM_CHECK_ACK
					iSrcId	= B modu
					iDstId	= A modu

*/
#define GPN_SELF_MSG_COMM_CHECK_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_SELF_MSG_COMM_CHECK))
/* msg parameters direction:   
					iIndex	= tx_Index++
					iMsgType	= GPN_SELF_MSG_COMM_CHECK_RSP
					iSrcId	= B modu
					iDstId	= A modu
					msgCellLen= 0
*/
#define GPN_SELF_MSG_COMM_CHECK_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SELF_MSG_COMM_CHECK_RSP))
/* msg parameters direction:   
					iIndex	= CMD index
					iMsgType	= GPN_SELF_MSG_COMM_CHECK_RSP_ACK
					iSrcId	= A modu
					iDstId	= B modu
*/

#define GPN_SELF_MSG_NEW_CONNCET_NOTIFY		((GPN_SOCK_OWNER_SELF)|(GPN_SELF_MT_PROTOCOL_PROC)|(GPN_SELF_ST_NEW_CONNCET_NOTIFY))
/* msg parameters direction:   
					iIndex	= tx_Index++
					iMsgType  	= GPN_SELF_MSG_NEW_CONNCET_NOTIFY
					iSrcId	= A modu
					iDstId	= B modu
					msgCellLen = sizeof(struct sockaddr_un) or sizeof(struct sockaddr_in)
					payload    	= sockaddr_un / sockaddr_in
*/
#define GPN_SELF_MSG_NEW_CONNCET_NOTIFY_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_SELF_MSG_NEW_CONNCET_NOTIFY))
/* msg parameters direction:   
					iIndex	= CMD index
					iMsgType	= GPN_SELF_MSG_NEW_CONNCET_NOTIFY_ACK
					iSrcId	= B modu
					iDstId	= A modu
*/


/*===============================================================================================*/

#define GPN_SELF_MSG_BASE_TIMER_SYN_OPT		((GPN_SOCK_OWNER_SELF)|GPN_SELF_MT_PROTOCOL_PROC|(GPN_SELF_ST_BASE_TIMER_SYN))	/**/
/* msg parameters direction: 
			iIndex         = tx_Index++
			iMsgType    = GPN_SELF_MSG_BASE_TIMER_SYN_OPT
			iSrcId         = CFGMGT
			iDstId         = other modu
			iMsgPara1   = old time (time_t)
			iMsgPara2   = new time (time_t)
			msgCellLen = 0

*/
#define GPN_SELF_MSG_BASE_TIMER_SYN_OPT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SELF_MSG_BASE_TIMER_SYN_OPT))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_SELF_MSG_BASE_TIMER_SYN_OPT_ACK
			iSrcId	   = other modu
			iDstId	   = CFGMGT
*/


UINT32 gpnSockSpMsgTxNewConnNotifyPartner(stSockFdSet *pstFdSet, UINT32 dstId, void *newConnAddr);
UINT32 gpnSockSpMsgTxBaseTimerSyn2ValidModule(stSockFdSet *pstFdSet, INT32 oldTime, INT32 newTime);
UINT32 gpnSockSpMsgTxBaseTimerSyn2OtherModule(stSockFdSet *pstFdSet, UINT32 MODU_ID, INT32 oldTime, INT32 newTime);
UINT32 gpnSockSpMsgTxProMac2NSMModule(stSockFdSet *pstFdSet, UINT32 subSlot, UINT32 dev_type, UINT8 * proMac);



#ifdef    __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_SOCK_PROTOCOL_MSG_DEF_H_ */


