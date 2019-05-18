/**********************************************************
* file name: gpnSockSynceMsgDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan Chengquan 2014-11-07
* function: 
*    define details about communication between synce modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_SYNCE_MSG_DEF_H_
#define _GPN_SOCK_SYNCE_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnSockMsgDef.h"
#include "socketComm/gpnSelectOpt.h"

/*==========================================================*/
/*==========================================================*/
/*define main type  (from 1 to 0x7E)  : 1                   */
/*************************************************************************/
/*this main type include upData msg 					                 */
#define GPN_SYNCE_MT_SSM_FUNCTION				GPN_SOCK_MSG_MT_CREAT(1)
/***************************************************************************************/
/***************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  :   1                          */
/***************************************************************************************/
#define GPN_SYNCE_MST_WITH_SELF					GPN_SOCK_MSG_MST_CREAT(1)
/***************************************************************************************/
/*define main type 1's main-sub type 1's sub-sub type  (from 1 to 0xFE)  :             */
/***************************************************************************************/
	#define GPN_SYNCE_SST_SEND_LOCAL_SSM_2_SYSMGT_REQ	GPN_SOCK_MSG_SST_CREAT(1)
	#define GPN_SYNCE_SST_SEND_CLCSRC_PORT_2_SYSMGT     GPN_SOCK_MSG_SST_CREAT(2)

/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)	:	2	ADD BY CQ		        */
/****************************************************************************************/
#define GPN_SYSMGT_MST_WITH_SELF				GPN_SOCK_MSG_MST_CREAT(2)
/***************************************************************************************/
/*define main type 1's main-sub type 2's sub-sub type  (from 1 to 0xFE)  :			   */
/***************************************************************************************/
	#define GPN_SYSMGT_SST_SEND_SSM_2_SYNCED_REQ		GPN_SOCK_MSG_SST_CREAT(1)


#define GPN_SYNCE_PARA_MAX_PATH_LEN					256
#define GPN_SYNCE_PARA_SPFILE_TR_CREAT_MSG_LEN		((GPN_SOCK_MSG_HEAD_BLEN) + 2*(GPN_SDM_PARA_MAX_PATH_LEN))

/*px synced send ssm to nm*/
#define GPN_SYNCE_MSG_SEND_LOCAL_SSM_2_SYSMGT_CMD	((GPN_SOCK_OWNER_SYNCED)|(GPN_SYNCE_MT_SSM_FUNCTION)|(GPN_SYNCE_MST_WITH_SELF)|(GPN_SYNCE_SST_SEND_LOCAL_SSM_2_SYSMGT_REQ))	/**/
/* msg parameters direction: 
		iIndex     = tx_Index++
		iMsgType   = GPN_SYNCE_MSG_SEND_LOCAL_SSM_2_SYSMGT_CMD
            iSrcId     = SYNCE(SUB)
            iDstId     = SYSMGT(NMx)
            iMsgPara1  = ssm is_enable
            iMsgPara2  = ssm Sa
            iMsgPara3  = slot(SUB)
            msgCellLen = 0
*/
#define GPN_SYNCE_MSG_SEND_LOCAL_SSM_2_SYSMGT_CMD_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYNCE_MSG_SEND_LOCAL_SSM_2_SYSMGT_CMD))	/**/
/* msg parameters direction: 
		iIndex     = tx_Index++
		iMsgType   = GPN_SYNCE_MSG_SEND_LOCAL_SSM_2_SYSMGT_CMD_ACK
            iSrcId     = SYSMGT(NMx)
            iDstId     = SYNCE(SUB)
*/
#define GPN_SYNCE_MSG_SEND_LOCAL_SSM_2_SYSMGT_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_SYNCE_MSG_SEND_LOCAL_SSM_2_SYSMGT_CMD))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_SYNCE_MSG_SEND_LOCAL_SSM_2_SYSMGT_RSP
		iSrcId	   = SYSMGT(NMx)
		iDstId	   = SYNCE(SUB)
*/
#define GPN_SYNCE_MSG_SEND_CLCSRC_PORT_2_SYSMGT_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYNCE_MSG_SEND_LOCAL_SSM_2_SYSMGT_RSP))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_SYNCE_MSG_SEND_CLCSRC_PORT_2_SYSMGT_RSP_ACK
		iSrcId	   = SYNCE(SUB)
		iDstId	   = SYSMGT(NMx)
*/

typedef struct __synceClcSrcPortList__
{
	UINT32 srcPortNum;
	UINT32 srcPort1;
	UINT32 srcPort2;
	UINT32 srcPort3;
	UINT32 srcPort4;
	UINT32 srcPort5;
	UINT32 srcPort6;
	UINT32 srcPort7;
	UINT32 srcPort8;
	UINT32 srcPort9;
	UINT32 srcPort10;
	UINT32 srcPort11;
	UINT32 srcPort12;
	UINT32 srcPort13;
}synceClcSrcPortList;
#define GPN_SYNCE_SST_SEND_CLCSRC_PORT_2_SYSMGT_CMD			((GPN_SOCK_OWNER_SYNCED)|(GPN_SYNCE_MT_SSM_FUNCTION)|(GPN_SYNCE_MST_WITH_SELF)|(GPN_SYNCE_SST_SEND_CLCSRC_PORT_2_SYSMGT))	/**/
/* msg parameters direction: 
		iIndex     = tx_Index++
		iMsgType   = GPN_SYNCE_SST_SEND_CLCSRC_PORT_2_SYSMGT_CMD
        iSrcId     = SYNCE(SUB)
        iDstId     = SYSMGT(NMx)
        iMsgPara1  = send slot
        iMsgPara2  = clc src port num(1 <= n <= 13)
        iMsgPara3  = clc src port 1
        iMsgPara4  = clc src port 2
        iMsgPara5  = clc src port 3
        .....................................
        msgCellLen = 0
*/
#define GPN_SYNCE_SST_SEND_CLCSRC_PORT_2_SYSMGT_CMD_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYNCE_SST_SEND_CLCSRC_PORT_2_SYSMGT_CMD))	/**/
/* msg parameters direction: 
		iIndex     = tx_Index++
		iMsgType   = GPN_SYNCE_SST_SEND_CLCSRC_PORT_2_SYSMGT_CMD_ACK
        iSrcId     = SYSMGT(NMx)
        iDstId     = SYNCE(SUB)
*/


#define GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_CMD				((GPN_SOCK_OWNER_SYNCED)|(GPN_SYNCE_MT_SSM_FUNCTION)|(GPN_SYSMGT_MST_WITH_SELF)|(GPN_SYSMGT_SST_SEND_SSM_2_SYNCED_REQ))
/* msg parameters direction: 
			iIndex     	= tx_Index++
			iMsgType    = GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_CMD
			iSrcId      = SYSMGT(NMx)
			iDstId      = SYNCED(slot)
			iMsgPara1  	= loadSsmInfo
			iMsgPara2  	= ssmConfig
			msgCellLen 	= 0
*/
#define GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_CMD_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_CMD))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_CMD_ACK
			iSrcId	   = SYNCE(SUB)
			iDstId	   = SYSMGT(NMx)
*/
#define GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_CMD))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_RSP
			iSrcId	   = SYNCE(SUB)
			iDstId	   = SYSMGT(NMx)
			iMsgPara1  	= IS_OK
			iMsgPara2  	= SLOT
*/
#define GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_RSP))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_SYSMGT_MSG_SEND_SSM_2_SYNCED_RSP_ACK
			iSrcId	   = SYSMGT(NMx)
			iDstId	   = SYNCE(SUB)
*/


UINT32 gpnSockSYNCEMsgTxS1OfSSMReq(UINT32 isSsmEnable, UINT32 ssmSa,  UINT32 slot);
UINT32 gpnSockSYNCEMsgTxS1OfSSMRsp(UINT32 slot, UINT32 isOK);

UINT32 gpnSockSYNCEMsgTxClcSrcPortList(synceClcSrcPortList *portList);

UINT32 gpnSockSYSMGTMsgTxS1OfSsmReq(UINT32 slot, UINT32 loadSsmSa, UINT32 ssmSaConfig);
UINT32 gpnSockSYSMGTMsgTxS1OfSsmRsp(UINT32 slot, UINT32 isOK);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_SYNCE_MSG_DEF_H_*/


