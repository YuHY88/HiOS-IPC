/**********************************************************
* file name: gpnSockStatMsgDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-25
* function: 
*    define details about communication between gpn_stat modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_STAT_MSG_DEF_H_
#define _GPN_SOCK_STAT_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnSockMsgDef.h"
#include "lib/gpnSocket/socketComm/gpnSelectOpt.h"
#include "lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h"
#include "lib/gpnSocket/socketComm/gpnStatTypeDef.h"


/************************************/
/*define owner : GPN_SOCK_OWNER_STAT */
/************************************/

/* lipf add for cycle */
#define GPN_STAT_CYCLE_MAX	24*3600/20		//seconds
#define GPN_STAT_CYCLE_MIN	1				//seconds



/*================================================================*/
/*define main type  (from 1 to 0x7E)  : 1                                                                                                  */
/***************************************************************************************/
/*this main type include gpn_stat (snmp defined) global scaler opt   */
#define GPN_STAT_MT_GLOBAL_OPT				((1 << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 1                                                                        */
/****************************************************************************************/
#define GPN_STAT_MST_GLOBAL_SCALER_OPT		((1 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
#define GPN_STAT_SST_TIME_POLARIT_GET		((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_TIME_OFFSET_GET		((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_MAX_OBJ_SRC_GET		((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_USE_OBJ_SRC_GET		((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_LAST_DATA_REPT_GET		((5 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_LAST_DATA_REPT_SET		((6 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_OLD_DATA_REPT_GET		((7 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_OLD_DATA_REPT_SET		((8 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_ALARM_REPT_GET			((9 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_ALARM_REPT_SET			((10 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_EVENT_REPT_GET			((11 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_EVENT_REPT_SET			((12 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_TASK_ID_GET			((13 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_MAX_TASK_NUM_GET		((14 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_RUN_TASK_NUM_GET		((15 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_EVN_THRED_TP_ID_GET    ((16 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_MAX_EVN_THRED_TP_NUM_GET ((17 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_RUN_EVN_THRED_TP_NUM_GET ((18 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_ALM_THRED_TP_ID_GET    ((19 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_MAX_ALM_THRED_TP_NUM_GET ((20 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_RUN_ALM_THRED_TP_NUM_GET ((21 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_SUB_FILT_TP_ID_GET		((22 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_MAX_SUBFILTTP_NUM_GET	((23 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_RUN_SUBFILTTP_NUM_GET	((24 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_SUB_REPORT_TP_ID_GET	((25 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_MAX_SUBREPORTTP_NUM_GET ((26 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_RUN_SUBREPORTTP_NUM_GET ((27 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_COUNT_TYPE_GET			((28 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_MSG_TIME_POLARIT_GET		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_TIME_POLARIT_GET))   /* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_TIME_POLARIT_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_TIME_POLARIT_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TIME_POLARIT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TIME_POLARIT_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_TIME_POLARIT_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_TIME_POLARIT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_TIME_POLARIT_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = polarity(1+/2-)
			iMsgPara2  = offset
			msgCellLen = 0
*/
#define GPN_STAT_MSG_TIME_POLARIT_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TIME_POLARIT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TIME_POLARIT_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_TIME_OFFSET_GET		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_TIME_OFFSET_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_TIME_OFFSET_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_TIME_OFFSET_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TIME_OFFSET_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TIME_OFFSET_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_TIME_OFFSET_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_TIME_OFFSET_GET))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_TIME_OFFSET_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = polarity(1+/2-)
			iMsgPara2  = offset
			msgCellLen = 0
*/
#define GPN_STAT_MSG_TIME_OFFSET_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TIME_OFFSET_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TIME_OFFSET_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_MAX_OBJ_SRC_GET		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_MAX_OBJ_SRC_GET))   /* */
/* msg parameters direction:
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MAX_OBJ_SRC_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_MAX_OBJ_SRC_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MAX_OBJ_SRC_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MAX_OBJ_SRC_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_MAX_OBJ_SRC_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_MAX_OBJ_SRC_GET))   /* */
/* msg parameters direction:
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MAX_OBJ_SRC_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = support max obj
			iMsgPara2  = already use obj
			msgCellLen = 0
*/
#define GPN_STAT_MSG_MAX_OBJ_SRC_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MAX_OBJ_SRC_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MAX_OBJ_SRC_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_USE_OBJ_SRC_GET		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_USE_OBJ_SRC_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_USE_OBJ_SRC_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_USE_OBJ_SRC_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_USE_OBJ_SRC_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_USE_OBJ_SRC_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_USE_OBJ_SRC_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_USE_OBJ_SRC_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_USE_OBJ_SRC_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = location index num(1=<i<=6)
			iMsgPara8  = support max obj
			iMsgPara9  = already use obj
			msgCellLen = 0
*/
#define GPN_STAT_MSG_USE_OBJ_SRC_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_USE_OBJ_SRC_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_USE_OBJ_SRC_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_LAST_DATA_REPT_GET		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_LAST_DATA_REPT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_LAST_DATA_REPT_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_LAST_DATA_REPT_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_LAST_DATA_REPT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_LAST_DATA_REPT_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_LAST_DATA_REPT_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_LAST_DATA_REPT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_LAST_DATA_REPT_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = 1 enable;2 disable
			msgCellLen = 0
*/
#define GPN_STAT_MSG_LAST_DATA_REPT_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_LAST_DATA_REPT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_LAST_DATA_REPT_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_LAST_DATA_REPT_SET		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_LAST_DATA_REPT_SET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_LAST_DATA_REPT_SET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = 1 enable;2 disable
			msgCellLen = 0
*/
#define GPN_STAT_MSG_LAST_DATA_REPT_SET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_LAST_DATA_REPT_SET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_LAST_DATA_REPT_SET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_OLD_DATA_REPT_GET		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_OLD_DATA_REPT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_OLD_DATA_REPT_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_OLD_DATA_REPT_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_OLD_DATA_REPT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_OLD_DATA_REPT_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_OLD_DATA_REPT_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_OLD_DATA_REPT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_OLD_DATA_REPT_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = 1 enable;2 disable
			msgCellLen = 0
*/
#define GPN_STAT_MSG_OLD_DATA_REPT_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_OLD_DATA_REPT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_OLD_DATA_REPT_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_OLD_DATA_REPT_SET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_OLD_DATA_REPT_SET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_OLD_DATA_REPT_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = 1 enable;2 disable
			msgCellLen = 0
*/
#define GPN_STAT_MSG_OLD_DATA_REPT_SET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_OLD_DATA_REPT_SET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_OLD_DATA_REPT_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_ALARM_REPT_GET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_ALARM_REPT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ALARM_REPT_GET
			iSrcId		= other modu
			iDstId		= gpn_stat
			msgCellLen = 0
*/
#define GPN_STAT_MSG_ALARM_REPT_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ALARM_REPT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ALARM_REPT_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_ALARM_REPT_GET_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_ALARM_REPT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ALARM_REPT_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = 1 enable;2 disable
			msgCellLen = 0
*/
#define GPN_STAT_MSG_ALARM_REPT_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ALARM_REPT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ALARM_REPT_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_ALARM_REPT_SET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_ALARM_REPT_SET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ALARM_REPT_SET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = 1 enable;2 disable
			msgCellLen = 0
*/
#define GPN_STAT_MSG_ALARM_REPT_SET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ALARM_REPT_SET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ALARM_REPT_SET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_EVENT_REPT_GET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_EVENT_REPT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EVENT_REPT_GET
			iSrcId		= other modu
			iDstId		= gpn_stat
			msgCellLen = 0
*/
#define GPN_STAT_MSG_EVENT_REPT_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EVENT_REPT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_EVENT_REPT_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_EVENT_REPT_GET_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_EVENT_REPT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EVENT_REPT_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = 1 enable;2 disable
			msgCellLen = 0
*/
#define GPN_STAT_MSG_EVENT_REPT_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EVENT_REPT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_EVENT_REPT_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_EVENT_REPT_SET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_EVENT_REPT_SET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EVENT_REPT_SET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = 1 enable;2 disable
			msgCellLen = 0
*/
#define GPN_STAT_MSG_EVENT_REPT_SET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EVENT_REPT_SET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_EVENT_REPT_SET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_TASK_ID_GET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_TASK_ID_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_TASK_ID_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_TASK_ID_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TASK_ID_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TASK_ID_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_TASK_ID_GET_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_TASK_ID_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_TASK_ID_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = stat taskId
			msgCellLen = 0
*/
#define GPN_STAT_MSG_TASK_ID_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TASK_ID_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TASK_ID_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_MAX_TASK_NUM_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_MAX_TASK_NUM_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MAX_TASK_NUM_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_MAX_TASK_NUM_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MAX_TASK_NUM_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MAX_TASK_NUM_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_MAX_TASK_NUM_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_MAX_TASK_NUM_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_MAX_TASK_NUM_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = max task num
			iMsgPara2  = run task num
			msgCellLen = 0
*/
#define GPN_STAT_MSG_MAX_TASK_NUM_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MAX_TASK_NUM_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MAX_TASK_NUM_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_RUN_TASK_NUM_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_RUN_TASK_NUM_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_RUN_TASK_NUM_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_RUN_TASK_NUM_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_RUN_TASK_NUM_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_RUN_TASK_NUM_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_RUN_TASK_NUM_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_RUN_TASK_NUM_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_RUN_TASK_NUM_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = max task num
			iMsgPara2  = run task num
			msgCellLen = 0
*/
#define GPN_STAT_MSG_RUN_TASK_NUM_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_RUN_TASK_NUM_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_RUN_TASK_NUM_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_EVN_THRED_TP_ID_GET		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_EVN_THRED_TP_ID_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TP_ID_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_EVN_THRED_TP_ID_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EVN_THRED_TP_ID_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TP_ID_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_EVN_THRED_TP_ID_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_EVN_THRED_TP_ID_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TP_ID_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = stat threshold templat ID
			msgCellLen = 0
*/
#define GPN_STAT_MSG_EVN_THRED_TP_ID_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EVN_THRED_TP_ID_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TP_ID_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET	((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_MAX_EVN_THRED_TP_NUM_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = max task num
			iMsgPara2  = run task num
			msgCellLen = 0
*/
#define GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET_RSP_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_RUN_EVN_THRED_TP_NUM_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = max task num
			iMsgPara2  = run task num
			msgCellLen = 0
*/
#define GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_ALM_THRED_TP_ID_GET		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_ALM_THRED_TP_ID_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TP_ID_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_ALM_THRED_TP_ID_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ALM_THRED_TP_ID_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TP_ID_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_ALM_THRED_TP_ID_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_ALM_THRED_TP_ID_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TP_ID_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = stat threshold templat ID
			msgCellLen = 0
*/
#define GPN_STAT_MSG_ALM_THRED_TP_ID_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ALM_THRED_TP_ID_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TP_ID_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET	((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_MAX_ALM_THRED_TP_NUM_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = max task num
			iMsgPara2  = run task num
			msgCellLen = 0
*/
#define GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET_RSP_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MAX_ALM_THRED_TP_NUM_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_RUN_ALM_THRED_TP_NUM_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = max task num
			iMsgPara2  = run task num
			msgCellLen = 0
*/
#define GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_RUN_ALM_THRED_TP_NUM_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_SUB_FILT_TP_ID_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_SUB_FILT_TP_ID_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUB_FILT_TP_ID_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_SUB_FILT_TP_ID_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUB_FILT_TP_ID_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUB_FILT_TP_ID_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_SUB_FILT_TP_ID_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_SUB_FILT_TP_ID_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUB_FILT_TP_ID_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = stat sub type filt templat ID
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SUB_FILT_TP_ID_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUB_FILT_TP_ID_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUB_FILT_TP_ID_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_MAX_SUBFILTTP_NUM_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = max task num
			iMsgPara2  = run task num
			msgCellLen = 0
*/
#define GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_RUN_SUBFILTTP_NUM_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = max task num
			iMsgPara2  = run task num
			msgCellLen = 0
*/
#define GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_SUB_REPORT_TP_ID_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_SUB_REPORT_TP_ID_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUB_REPORT_TP_ID_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_SUB_REPORT_TP_ID_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUB_REPORT_TP_ID_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUB_REPORT_TP_ID_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_SUB_REPORT_TP_ID_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_SUB_REPORT_TP_ID_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUB_REPORT_TP_ID_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = stat sub type report templat ID
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SUB_REPORT_TP_ID_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUB_REPORT_TP_ID_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUB_REPORT_TP_ID_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_MAX_SUBREPORTTP_NUM_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = max templat num
			iMsgPara2  = run templat num
			msgCellLen = 0
*/
#define GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_RUN_SUBREPORTTP_NUM_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET
                   	iSrcId     	= other modu
                   	iDstId     	= gpn_stat
                   	msgCellLen = 0
*/
#define GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = max templat num
			iMsgPara2  = run templat num
			msgCellLen = 0
*/
#define GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_COUNT_TYPE_GET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_GLOBAL_SCALER_OPT)|(GPN_STAT_SST_COUNT_TYPE_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_COUNT_TYPE_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			msgCellLen = 0
*/
#define GPN_STAT_MSG_COUNT_TYPE_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_COUNT_TYPE_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_COUNT_TYPE_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_COUNT_TYPE_GET_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_COUNT_TYPE_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_COUNT_TYPE_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = stat data counter type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_COUNT_TYPE_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_COUNT_TYPE_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_COUNT_TYPE_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

/****************************************************************************************/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)	: 2 																		*/
/****************************************************************************************/
#define GPN_STAT_MST_STAT_TYPE_OPT				((2 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :																			 */
/****************************************************************************************/
#define GPN_STAT_SST_STAT_TYPE_GET				((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_STAT_TYPE_GET_NEXT			((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_SUB_STAT_TYPE_GET			((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_SUB_STAT_TYPE_GET_NEXT		((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_SUB_STAT_TYPE_MODIFY		((5 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_PORT_TP_2_SCAN_TP_GET		((6 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_PORT_TP_2_SCAN_TP_GET_NEXT	((7 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_SUB_STAT_TRED_SET			((8 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_MSG_STAT_TYPE_GET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_TYPE_OPT)|(GPN_STAT_SST_STAT_TYPE_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_STAT_TYPE_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = stat scan type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_STAT_TYPE_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_STAT_TYPE_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_STAT_TYPE_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_STAT_TYPE_GET_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_STAT_TYPE_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_STAT_TYPE_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = stat scan type(index)
			iMsgPara2  = long cyc seconds
			iMsgPara3  = short cyc seconds
			iMsgPara4  = sub stat type num in scan type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_STAT_TYPE_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_STAT_TYPE_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_STAT_TYPE_GET_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_STAT_TYPE_GET_NEXT			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_TYPE_OPT)|(GPN_STAT_SST_STAT_TYPE_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_STAT_TYPE_GET_NEXT
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = stat scan type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_STAT_TYPE_GET_NEXT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_STAT_TYPE_GET_NEXT))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_STAT_TYPE_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_STAT_TYPE_GET_NEXT_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_STAT_TYPE_GET_NEXT))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_STAT_TYPE_GET_NEXT_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = stat scan type(index)
			iMsgPara2  = long cyc seconds
			iMsgPara3  = short cyc seconds
			iMsgPara4  = sub stat type num in scan type
			iMsgPara5  = next stat scan type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_STAT_TYPE_GET_NEXT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_STAT_TYPE_GET_NEXT_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_STAT_TYPE_GET_NEXT_RSP_ACK
			iDstId	= gpn_stat
*/

typedef struct _stSubTypeBoundAlm_
{
	UINT32 longCycUpAlm;
	UINT32 longCycLowAlm;
	UINT32 longCycUpEvt;
	UINT32 longCycLowEvt;
	UINT32 shortCycUpAlm;
	UINT32 shortCycLowAlm;
	UINT32 shortCycUpEvt;
	UINT32 shortCycLowEvt;
	UINT32 userDefCycUpAlm;
	UINT32 userDefCycLowAlm;
	UINT32 userDefCycUpEvt;
	UINT32 userDefCycLowEvt;
}stSubTypeBoundAlm;
typedef struct _stSubTypeThred_
{
	UINT32 longCycUpThredHigh32;
	UINT32 longCycUpThredLow32;
	UINT32 longCycDnThredHigh32;
	UINT32 longCycDnThredLow32;
	UINT32 shortCycUpThredHigh32;
	UINT32 shortCycUpThredLow32;
	UINT32 shortCycDnThredHigh32;
	UINT32 shortCycDnThredLow32;
}stSubTypeThred;
#define GPN_STAT_MSG_SUB_STAT_TYPE_GET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_TYPE_OPT)|(GPN_STAT_SST_SUB_STAT_TYPE_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUB_STAT_TYPE_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SUB_STAT_TYPE_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUB_STAT_TYPE_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUB_STAT_TYPE_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_SUB_STAT_TYPE_GET_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_SUB_STAT_TYPE_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUB_STAT_TYPE_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = sub stat type
			iMsgPara2  = sub type belong type
			iMsgPara3  = sub type value class
			iMsgPara4  = sub type data bit deep
			msgCellLen = sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred)
			payload = stSubTypeBoundAlm + stSubTypeThred
*/
#define GPN_STAT_MSG_SUB_STAT_TYPE_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUB_STAT_TYPE_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_SUB_STAT_TYPE_GET_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_TYPE_OPT)|(GPN_STAT_SST_SUB_STAT_TYPE_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = sub stat type
			iMsgPara2  = sub type belong type
			iMsgPara3  = sub type value class
			iMsgPara4  = sub type data bit deep
			iMsgPara5  = next sub stat type
			msgCellLen = sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred)
			payload = stSubTypeBoundAlm + stSubTypeThred.
*/
#define GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_SUB_STAT_TYPE_MODIFY			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_TYPE_OPT)|(GPN_STAT_SST_SUB_STAT_TYPE_MODIFY))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUB_STAT_TYPE_MODIFY
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat type
			msgCellLen = sizeof(stSubTypeThred)
*/
#define GPN_STAT_MSG_SUB_STAT_TYPE_MODIFY_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUB_STAT_TYPE_MODIFY))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUB_STAT_TYPE_MODIFY_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu	
*/

#define GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_TYPE_OPT)|(GPN_STAT_SST_PORT_TP_2_SCAN_TP_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = port type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType	= GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = port type
			iMsgPara2  = port type about stat scan type num
			iMsgPara3  = stat scan type (1)
			iMsgPara4  = stat scan type (2)
			iMsgPara5  = stat scan type (3)
			iMsgPara6  = stat scan type (4)
			iMsgPara7  = stat scan type (5)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_RSP_ACK
			iDstId	= gpn_stat
*/
	

#define GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_TYPE_OPT)|(GPN_STAT_SST_PORT_TP_2_SCAN_TP_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = port type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = port type
			iMsgPara2  = port type about stat scan type num
			iMsgPara3  = stat scan type (1)
			iMsgPara4  = stat scan type (2)
			iMsgPara5  = stat scan type (3)
			iMsgPara6  = stat scan type (4)
			iMsgPara7  = stat scan type (5)
			iMsgPara8  = next port type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT_RSP_ACK
			iDstId	= gpn_stat
*/

/****************************************************************************************/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)	: 3 																		*/
/****************************************************************************************/
#define GPN_STAT_MST_STAT_MON_OPT				((3 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :																			 */
/****************************************************************************************/
#define GPN_STAT_SST_TASK_ADD					((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_TASK_DELETE				((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_TASK_MODIFY				((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_TASK_GET					((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_TASK_GET_NEXT				((5 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_EVN_THRED_TEMP_ADD			((6 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_EVN_THRED_TEMP_DELETE		((7 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_EVN_THRED_TEMP_MODIFY		((8 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_EVN_THRED_TEMP_GET			((9 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_EVN_THRED_TEMP_GET_NEXT	((10 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_ALM_THRED_TEMP_ADD			((11 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_ALM_THRED_TEMP_DELETE		((12 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_ALM_THRED_TEMP_MODIFY		((13 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_ALM_THRED_TEMP_GET			((14 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_ALM_THRED_TEMP_GET_NEXT	((15 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_SUBFILT_TEMP_ADD			((16 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_SUBFILT_TEMP_DELETE		((17 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_SUBFILT_TEMP_MODIFY		((18 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_SUBFILT_TEMP_GET			((19 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_SUBFILT_TEMP_GET_NEXT		((20 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_SUBREPORT_TEMP_ADD			((21 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_SUBREPORT_TEMP_DELETE		((22 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_SUBREPORT_TEMP_MODIFY		((23 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_SUBREPORT_TEMP_GET			((24 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_SUBREPORT_TEMP_GET_NEXT	((25 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_MON_ADD					((26 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_MON_DELETE					((27 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_MON_GET					((28 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_MON_GET_NEXT				((29 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_BASE_MON_OPT				((30 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_PORT_CYC_MON_OPT			((31 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_TASK_ALM_THREDTP_CFG		((32 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_MSG_TASK_ADD					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_TASK_ADD))   /*  */
/* msg parameters direction : 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_TASK_ADD
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = taskId
			iMsgPara2  = statScanType
			iMsgPara3  = subTaskNum( include port Num)
			iMsgPara4  = cycClass
			iMsgPara5  = cyc seconds
			iMsgPara6  = task start time
			iMsgPara7  = task end time
			iMsgPara8  = alarm threshold templat index
			iMsgPara9  = sub type report templat index
			msgCellLen = 0
*/
#define GPN_STAT_MSG_TASK_ADD_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TASK_ADD))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TASK_ADD_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_TASK_DELETE				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_TASK_DELETE))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_TASK_DELETE
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = taskId
			msgCellLen = 0
*/
#define GPN_STAT_MSG_TASK_DELETE_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TASK_DELETE))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TASK_DELETE_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_TASK_MODIFY				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_TASK_MODIFY))   /*  */
/* msg parameters direction : 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_TASK_MODIFY
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = taskId(can't modify)
			iMsgPara2  = statScanType(meaningless, can't modify)
			iMsgPara3  = subTaskNum( include port Num)(meaningless, can't modify)
			iMsgPara4  = cycClass(meaningless, can't modify)
			iMsgPara5  = cyc seconds
			iMsgPara6  = task start time
			iMsgPara7  = task end time
			iMsgPara8  = alarm threshold templat index
			iMsgPara9  = sub type report templat index
			msgCellLen = 0
*/
#define GPN_STAT_MSG_TASK_MODIFY_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TASK_MODIFY))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TASK_MODIFY_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_TASK_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_TASK_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_TASK_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = taskId
			msgCellLen = 0
*/
#define GPN_STAT_MSG_TASK_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TASK_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TASK_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = taskId
*/
#define GPN_STAT_MSG_TASK_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_TASK_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_TASK_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = taskId
			iMsgPara2  = statScanType
			iMsgPara3  = subTaskNum( include port Num)
			iMsgPara4  = cycClass
			iMsgPara5  = cyc seconds
			iMsgPara6  = task start time
			iMsgPara7  = task end time
			iMsgPara8  = alarm threshold templat index
			iMsgPara9  = sub type report templat index
			msgCellLen = 0
*/
#define GPN_STAT_MSG_TASK_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TASK_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_TASK_GET_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_TASK_GET_NEXT				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_TASK_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_TASK_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = taskId
			msgCellLen = 0
*/
#define GPN_STAT_MSG_TASK_GET_NEXT_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TASK_GET_NEXT))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TASK_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_TASK_GET_NEXT_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_TASK_GET_NEXT))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_TASK_GET_NEXT_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = taskId
			iMsgPara2  = statScanType
			iMsgPara3  = subTaskNum( include port Num)
			iMsgPara4  = cycClass
			iMsgPara5  = cyc seconds
			iMsgPara6  = task start time
			iMsgPara7  = task end time
			iMsgPara8  = alarm threshold templat index
			iMsgPara9  = sub type report templat index
			iMsgParaA  = NEXT taskId
			msgCellLen = 0
*/
#define GPN_STAT_MSG_TASK_GET_NEXT_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TASK_GET_NEXT_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_TASK_GET_NEXT_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_EVN_THRED_TEMP_ADD				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_EVN_THRED_TEMP_ADD))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TEMP_ADD
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = threshold templat id(index)
			iMsgPara2  = sub type(index)
			iMsgPara3  = stat scan type
			iMsgPara4  = upThredH32
			iMsgPara5  = upThredL32
			iMsgPara6  = dnThredH32
			iMsgPara7  = dnThredL32
			msgCellLen = 0
*/
#define GPN_STAT_MSG_EVN_THRED_TEMP_ADD_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EVN_THRED_TEMP_ADD))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TEMP_ADD_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_EVN_THRED_TEMP_DELETE			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_EVN_THRED_TEMP_DELETE))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TEMP_DELETE
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = threshold templat id
			msgCellLen = 0
*/
#define GPN_STAT_MSG_EVN_THRED_TEMP_DELETE_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EVN_THRED_TEMP_DELETE))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TEMP_DELETE_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_EVN_THRED_TEMP_MODIFY			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_EVN_THRED_TEMP_MODIFY))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TEMP_MODIFY
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = threshold templat id(index)
			iMsgPara2  = sub type(index)
			iMsgPara3  = stat scan type
			iMsgPara4  = upThredH32
			iMsgPara5  = upThredL32
			iMsgPara6  = dnThredH32
			iMsgPara7  = dnThredL32
			msgCellLen = 0
*/
#define GPN_STAT_MSG_EVN_THRED_TEMP_MODIFY_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EVN_THRED_TEMP_MODIFY))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TEMP_MODIFY_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_EVN_THRED_TEMP_GET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_EVN_THRED_TEMP_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TEMP_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = threshold templat id(index)
			iMsgPara2  = sub type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_EVN_THRED_TEMP_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EVN_THRED_TEMP_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TEMP_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_EVN_THRED_TEMP_GET_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_EVN_THRED_TEMP_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TEMP_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = threshold templat id(index)
			iMsgPara2  = sub type(index)
			iMsgPara3  = stat scan type
			iMsgPara4  = upThredH32
			iMsgPara5  = upThredL32
			iMsgPara6  = dnThredH32
			iMsgPara7  = dnThredL32
			msgCellLen =0
*/
#define GPN_STAT_MSG_EVN_THRED_TEMP_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EVN_THRED_TEMP_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_EVN_THRED_TEMP_GET_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_EVN_THRED_TEMP_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TEMP_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = threshold templat id(index)
			iMsgPara2  = sub type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = threshold templat id(index)
			iMsgPara2  = sub type(index)
			iMsgPara3  = stat scan type
			iMsgPara4  = upThredH32
			iMsgPara5  = upThredL32
			iMsgPara6  = dnThredH32
			iMsgPara7  = dnThredL32
			iMsgPara8  = next threshold templat id(index)
			iMsgPara9  = next sub type(index)
			msgCellLen = 0

*/
#define GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT_RSP_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_ALM_THRED_TEMP_ADD				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_ALM_THRED_TEMP_ADD))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TEMP_ADD
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = threshold templat id
			iMsgPara2  = sub type
			iMsgPara3  = stat scan type
			iMsgPara4  = upRiseThredH32
			iMsgPara5  = upRiseThredL32
			iMsgPara6  = upDispThredH32
			iMsgPara7  = upDispThredL32
			iMsgPara8  = dnRiseThredH32
			iMsgPara9  = dnRiseThredL32
			iMsgParaA  = dnDispThredH32
			iMsgParaB  = dnDispThredL32
			msgCellLen =0
*/
#define GPN_STAT_MSG_ALM_THRED_TEMP_ADD_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ALM_THRED_TEMP_ADD))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TEMP_ADD_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_ALM_THRED_TEMP_DELETE			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_ALM_THRED_TEMP_DELETE))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TEMP_DELETE
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = threshold templat id
			msgCellLen = 0
*/
#define GPN_STAT_MSG_ALM_THRED_TEMP_DELETE_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ALM_THRED_TEMP_DELETE))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TEMP_DELETE_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_ALM_THRED_TEMP_MODIFY			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_ALM_THRED_TEMP_MODIFY))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TEMP_MODIFY
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = threshold templat id
			iMsgPara2  = sub type
			iMsgPara3  = stat scan type(meaningless)
			iMsgPara4  = upRiseThredH32
			iMsgPara5  = upRiseThredL32
			iMsgPara6  = upDispThredH32
			iMsgPara7  = upDispThredL32
			iMsgPara8  = dnRiseThredH32
			iMsgPara9  = dnRiseThredL32
			iMsgParaA  = dnDispThredH32
			iMsgParaB  = dnDispThredL32
			msgCellLen =0
*/
#define GPN_STAT_MSG_ALM_THRED_TEMP_MODIFY_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ALM_THRED_TEMP_MODIFY))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TEMP_MODIFY_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_ALM_THRED_TEMP_GET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_ALM_THRED_TEMP_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TEMP_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = threshold templat id(index)
			iMsgPara2  = sub stat type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_ALM_THRED_TEMP_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ALM_THRED_TEMP_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TEMP_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_ALM_THRED_TEMP_GET_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_ALM_THRED_TEMP_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TEMP_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = threshold templat id
			iMsgPara2  = sub type
			iMsgPara3  = stat scan type
			iMsgPara4  = upRiseThredH32
			iMsgPara5  = upRiseThredL32
			iMsgPara6  = upDispThredH32
			iMsgPara7  = upDispThredL32
			iMsgPara8  = dnRiseThredH32
			iMsgPara9  = dnRiseThredL32
			iMsgParaA  = dnDispThredH32
			iMsgParaB  = dnDispThredL32
			msgCellLen = 0
*/
#define GPN_STAT_MSG_ALM_THRED_TEMP_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ALM_THRED_TEMP_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_ALM_THRED_TEMP_GET_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_ALM_THRED_TEMP_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TEMP_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = threshold templat id(index)
			iMsgPara2  = sub stat type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = threshold templat id
			iMsgPara2  = sub type
			iMsgPara3  = stat scan type
			iMsgPara4  = upRiseThredH32
			iMsgPara5  = upRiseThredL32
			iMsgPara6  = upDispThredH32
			iMsgPara7  = upDispThredL32
			iMsgPara8  = dnRiseThredH32
			iMsgPara9  = dnRiseThredL32
			iMsgParaA  = dnDispThredH32
			iMsgParaB  = dnDispThredL32
			iMsgParaC  = next threshold templat id
			iMsgParaD  = next subType
			msgCellLen = 0

*/
#define GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_SUBFILT_TEMP_ADD					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_SUBFILT_TEMP_ADD))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBFILT_TEMP_ADD
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat filt templat id
			iMsgPara2  = sub type
			iMsgPara3  = stat scan type
			iMsgPara4  = status
			msgCellLen =0
*/
#define GPN_STAT_MSG_SUBFILT_TEMP_ADD_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBFILT_TEMP_ADD))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUBFILT_TEMP_ADD_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_SUBFILT_TEMP_DELETE				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_SUBFILT_TEMP_DELETE))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBFILT_TEMP_DELETE
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat filt templat id
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SUBFILT_TEMP_DELETE_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBFILT_TEMP_ADD))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TASK_DELETE_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_SUBFILT_TEMP_MODIFY				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_SUBFILT_TEMP_MODIFY))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBFILT_TEMP_MODIFY
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat filt templat id(index)
			iMsgPara2  = sub type(index)
			iMsgPara3  = stat scan type(meaningless)
			iMsgPara4  = status
			msgCellLen =0
*/
#define GPN_STAT_MSG_SUBFILT_TEMP_MODIFY_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBFILT_TEMP_MODIFY))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUBFILT_TEMP_MODIFY_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_SUBFILT_TEMP_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_SUBFILT_TEMP_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBFILT_TEMP_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat filt templat id(index)
			iMsgPara2  = sub stat type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SUBFILT_TEMP_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBFILT_TEMP_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUBFILT_TEMP_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_SUBFILT_TEMP_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_SUBFILT_TEMP_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBFILT_TEMP_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = sub stat filt templat id(index)
			iMsgPara2  = sub type(index)
			iMsgPara3  = stat scan type
			iMsgPara4  = status
			msgCellLen =0
*/
#define GPN_STAT_MSG_SUBFILT_TEMP_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBFILT_TEMP_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_SUBFILT_TEMP_GET_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_SUBFILT_TEMP_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBFILT_TEMP_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat filt templat id(index)
			iMsgPara2  = sub stat type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = sub stat filt templat id(index)
			iMsgPara2  = sub type(index)
			iMsgPara3  = stat scan type
			iMsgPara4  = status
			iMsgPara5  = next sub stat filt templat id(index)
			iMsgPara6  = next sub type(index)
			msgCellLen =0
*/
#define GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_SUBREPORT_TEMP_ADD					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_SUBREPORT_TEMP_ADD))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBREPORT_TEMP_ADD
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat report templat id(index)
			iMsgPara2  = sub stat type(index)
			iMsgPara3  = stat scan type
			iMsgPara4  = sub stat type report status
			msgCellLen =0
*/
#define GPN_STAT_MSG_SUBREPORT_TEMP_ADD_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBREPORT_TEMP_ADD))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUBREPORT_TEMP_ADD_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_SUBREPORT_TEMP_DELETE				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_SUBREPORT_TEMP_DELETE))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBREPORT_TEMP_DELETE
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat filt templat id(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SUBREPORT_TEMP_DELETE_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBREPORT_TEMP_ADD))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TASK_DELETE_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_SUBREPORT_TEMP_MODIFY				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_SUBREPORT_TEMP_MODIFY))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBREPORT_TEMP_MODIFY
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat report templat id(index)
			iMsgPara2  = sub stat type(index)
			iMsgPara3  = stat scan type(meaningless)
			iMsgPara4  = sub stat type report status
			msgCellLen =0
*/
#define GPN_STAT_MSG_SUBREPORT_TEMP_MODIFY_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBREPORT_TEMP_MODIFY))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUBREPORT_TEMP_MODIFY_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_SUBREPORT_TEMP_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_SUBREPORT_TEMP_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBREPORT_TEMP_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat report templat id(index)
			iMsgPara2  = sub stat type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SUBREPORT_TEMP_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBREPORT_TEMP_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUBREPORT_TEMP_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_SUBREPORT_TEMP_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_SUBREPORT_TEMP_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBREPORT_TEMP_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = sub stat report templat id(index)
			iMsgPara2  = sub stat type(index)
			iMsgPara3  = stat scan type
			iMsgPara4  = sub stat type report status
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SUBREPORT_TEMP_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBREPORT_TEMP_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_SUBREPORT_TEMP_GET_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_SUBREPORT_TEMP_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = sub stat report templat id(index)
			iMsgPara2  = sub stat type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = sub stat report templat id(index)
			iMsgPara2  = sub stat type(index)
			iMsgPara3  = stat scan type
			iMsgPara4  = sub stat type report status
			iMsgPara5  = next sub stat report templat id(index)
			iMsgPara6  = next sub stat type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT_RSP_ACK
			iDstId	= gpn_stat
*/
typedef objLogicDesc stStatObjDesc;

typedef struct _stStatMsgMonObjCfg_
{
	/*table index*/
	stStatObjDesc portIndex;
	UINT32 scanType;
	UINT32 insAddFlag;       //modify by geqian   2016.5.19        use for statPortMoniCtrlTable  operation
	
	UINT32 statMoniEn;
	UINT32 currStatMoniEn;
	UINT32 longCycStatMoniEn;
	UINT32 longCycBelongTask;
	UINT32 longCycEvnThredTpId;
	UINT32 longCycSubFiltTpId;
	UINT32 longCycAlmThredTpId;
	UINT32 longCycHistReptTpId;
	UINT32 longCycHistDBId;
	UINT32 shortCycStatMoniEn;
	UINT32 shortCycBelongTask;
	UINT32 shortCycEvnThredTpId;
	UINT32 shortCycSubFiltTpId;
	UINT32 shortCycAlmThredTpId;
	UINT32 shortCycHistReptTpId;
	UINT32 shortCycHistDBId;
	UINT32 udCycStatMoniEn;
	UINT32 udCycBelongTask;
	UINT32 udCycEvnThredTpId;
	UINT32 udCycSubFiltTpId;
	UINT32 udCycAlmThredTpId;
	UINT32 udCycHistReptTpId;
	UINT32 udCycHistDBId;
	UINT32 udCycSecs;
}stStatMsgMonObjCfg;

/*port global stat moni enable/disable */
#define GPN_STAT_MSG_MON_ADD							((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_MON_ADD))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MON_ADD
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(device index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan type(index)
			msgCellLen = sizeof(stStatMsgMonObjCfg)
*/
#define GPN_STAT_MSG_MON_ADD_ACK						((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MON_ADD))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MON_ADD_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
	
#define GPN_STAT_MSG_MON_DELETE							((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_MON_DELETE))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MON_DELETE
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(device index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_MON_DELETE_ACK						((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MON_DELETE))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MON_ADD_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
	
#define GPN_STAT_MSG_MON_GET							((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_MON_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MON_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(device index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_MON_GET_ACK						((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MON_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MON_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_MON_GET_RSP						((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_MON_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MON_GET_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(device index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan type(index)
			msgCellLen = sizeof(stStatMsgMonObjCfg)
*/
#define GPN_STAT_MSG_MON_GET_RSP_ACK					((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MON_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_MON_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
	
#define GPN_STAT_MSG_MON_GET_NEXT						((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_MON_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MON_GET_NEXT
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(device index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_MON_GET_NEXT_ACK					((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MON_GET_NEXT))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_MON_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_MON_GET_NEXT_RSP					((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_MON_GET_NEXT))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_MON_GET_NEXT_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(device index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan type(index)
			msgCellLen = sizeof(stStatMsgMonObjCfg)+sizeof(statObjDesc)+sizeof(UINT32)
*/
#define GPN_STAT_MSG_MON_GET_NEXT_RSP_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_MON_GET_NEXT_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   =GPN_STAT_MSG_MON_GET_NEXT_RSP_ACK
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_BASE_MON_OPT						((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_BASE_MON_OPT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_BASE_MON_OPT
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(device index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan type(index)
			iMsgPara8  = enable/disable
			msgCellLen = 0
*/
#define GPN_STAT_MSG_BASE_MON_OPT_ACK					((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_BASE_MON_OPT))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_BASE_MON_ENABLE_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_PORT_CYC_MON_OPT					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_PORT_CYC_MON_OPT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PORT_CYC_MON_OPT
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = device index
			iMsgPara2  = portIndex1
			iMsgPara3  = portIndex2
			iMsgPara4  = portIndex3
			iMsgPara5  = portIndex4
			iMsgPara6  = portIndex5
			iMsgPara7  = scan type(index)
			iMsgPara8  = taskId(means cycClass: long,short,user-def)
			iMsgPara9  = enable/disable/delete
			iMsgParaA  = evnThTpId
			iMsgParaB  = subFiltThTpId
			msgCellLen = 0
*/
#define GPN_STAT_MSG_PORT_CYC_MON_OPT_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PORT_CYC_MON_OPT))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PORT_CYC_MON_OPT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_TASK_ALM_THREDTP_CFG				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_MON_OPT)|(GPN_STAT_SST_TASK_ALM_THREDTP_CFG))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_TASK_ALM_THREDTP_CFG
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = taskId
			iMsgPara2  = THRESHOLD templat ID
			msgCellLen = 0
*/
#define GPN_STAT_MSG_TASK_ALM_THREDTP_CFG_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_TASK_ALM_THREDTP_CFG))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_TASK_ALM_THREDTP_CFG_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

/*================================================================*/
/*define main type  (from 1 to 0x7E)  :2                                                                                                  */
/***************************************************************************************/
/*this main type include gpn_stat (snmp defined) data search and notify opt   */
#define GPN_STAT_MT_DATA_COLLECT_OPT				((2 << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 1                                                                        */
/****************************************************************************************/
#define GPN_STAT_MST_STAT_CTL_OTHER_MODU           	((1 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
#define GPN_STAT_SST_PORT_STAT_MON_ENABLE			((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_PORT_STAT_MON_DISABLE			((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_STAT_MON_PORT_REPLACE			((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_MSG_PORT_STAT_MON_ENABLE			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_STAT_CTL_OTHER_MODU)|(GPN_STAT_SST_PORT_STAT_MON_ENABLE))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_SST_PORT_STAT_MON_ENABLE
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan Type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_PORT_STAT_MON_ENABLE_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PORT_STAT_MON_ENABLE))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PORT_STAT_MON_ENABLE_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_PORT_STAT_MON_ENABLE_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_PORT_STAT_MON_ENABLE))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PORT_STAT_MON_ENABLE_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan Type
			iMsgPara8  = enStat
			iMsgPara9  = error code
*/
#define GPN_STAT_MSG_PORT_STAT_MON_ENABLE_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PORT_STAT_MON_ENABLE_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PORT_STAT_MON_ENABLE_RSP
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_PORT_STAT_MON_DISABLE			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_STAT_CTL_OTHER_MODU)|(GPN_STAT_SST_PORT_STAT_MON_DISABLE))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PORT_STAT_MON_DISABLE
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan Type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_PORT_STAT_MON_DISABLE_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PORT_STAT_MON_DISABLE))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PORT_STAT_MON_DISABLE_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_PORT_STAT_MON_DISABLE_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_PORT_STAT_MON_DISABLE))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PORT_STAT_MON_DISABLE_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan Type
			iMsgPara8  = disStat
			iMsgPara9  = error code
*/
#define GPN_STAT_MSG_PORT_STAT_MON_DISABLE_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PORT_STAT_MON_DISABLE_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PORT_STAT_MON_DISABLE_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_STAT_MON_PORT_REPLACE			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_STAT_CTL_OTHER_MODU)|(GPN_STAT_SST_STAT_MON_PORT_REPLACE))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_STAT_MON_PORT_REPLACE
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = old mon devIndex(index)
			iMsgPara2  = old mon portIndex1(index)
			iMsgPara3  = old mon portIndex2(index)
			iMsgPara4  = old mon portIndex3(index)
			iMsgPara5  = old mon portIndex4(index)
			iMsgPara6  = old mon portIndex5(index)
			iMsgPara7  = new mon devIndex(index)
			iMsgPara8  = new mon portIndex1(index)
			iMsgPara9  = new mon portIndex2(index)
			iMsgParaA  = new mon portIndex3(index)
			iMsgParaB  = new mon portIndex4(index)
			iMsgParaC  = new mon portIndex5(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_STAT_MON_PORT_REPLACE_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_STAT_MON_PORT_REPLACE))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_STAT_MON_PORT_REPLACE_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

/***************************************************************************** **********/
/*define main type 4's main-sub type  (from 1 to 0xFE)  : 2                                                                        */
/****************************************************************************************/
#define GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE		((2 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
#define GPN_STAT_SST_VS_P_STAT_GET					((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_VS_M_STAT_GET					((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_VS_STAT_RESET					((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_LSP_P_STAT_GET					((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_LSP_M_STAT_GET					((5 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_LSP_STAT_RESET					((6 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_PW_P_STAT_GET					((7 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_PW_M_STAT_GET					((8 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_PW_STAT_RESET					((9 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_ETH_PHY_P_STAT_GET				((10 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_ETH_PHY_M_STAT_GET				((11 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_ETH_SFP_STAT_GET				((12 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_ETH_STAT_RESET					((13 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_FLOW_P_STAT_GET				((14 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_FLOW_M_STAT_GET				((15 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_FLOW_STAT_RESET				((16 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_EQU_STAT_GET					((17 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_SOFT_STAT_GET					((18 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_ENV_STAT_GET					((19 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_PDH_STAT_GET					((20 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_VPLSPW_P_STAT_GET				((21 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_VPLSPW_M_STAT_GET				((22 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_VPLSPW_STAT_RESET				((23 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_ETH_OAM_M_STAT_GET				((24 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_SST_PTN_V_UNI_P_STAT_GET			((25 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_PTN_MEP_M_STAT_GET				((26 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

#define GPN_STAT_PARA_VS_P_STAT_DATA_SIZE			sizeof(gpnStatPtnVsPData)
#define GPN_STAT_PARA_VS_P_STAT_GET_MSG_SIZE		((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_VS_P_STAT_DATA_SIZE))
#define GPN_STAT_MSG_VS_P_STAT_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_VS_P_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_VS_P_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_VS_P_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_VS_P_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_VS_P_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_VS_P_STAT_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_VS_P_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_VS_P_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatPtnVsPDate)
			payload     = gpnStatPtnVsPDate
*/
#define GPN_STAT_MSG_VS_P_STAT_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_VS_P_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_VS_P_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_PARA_VS_M_STAT_DATA_SIZE			sizeof(gpnStatPtnVsMData)
#define GPN_STAT_PARA_VS_M_STAT_GET_MSG_SIZE		((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_VS_M_STAT_DATA_SIZE))
#define GPN_STAT_MSG_VS_M_STAT_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_VS_M_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_VS_M_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_VS_M_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_VS_M_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_VS_M_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_VS_M_STAT_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_VS_M_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_VS_M_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatPtnVsMData)
			payload     = gpnStatPtnVsMData
*/
#define GPN_STAT_MSG_VS_M_STAT_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_VS_M_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_VS_M_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_VS_STAT_RESET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_VS_STAT_RESET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_VS_STAT_RESET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan Type(meaningless index)
			iMsgPara8  = stat sub Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_VS_STAT_RESET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_VS_STAT_RESET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_VS_STAT_RESET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_PARA_LSP_P_STAT_DATA_SIZE			sizeof(gpnStatPtnLspPData)
#define GPN_STAT_PARA_LSP_P_STAT_GET_MSG_SIZE		((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_LSP_P_STAT_DATA_SIZE))
#define GPN_STAT_MSG_LSP_P_STAT_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_LSP_P_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_LSP_P_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_LSP_P_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_LSP_P_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_LSP_P_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_LSP_P_STAT_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_LSP_P_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_LSP_P_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatPtnLspDate)
			payload     = gpnStatPtnLspDate
*/
#define GPN_STAT_MSG_LSP_P_STAT_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_LSP_P_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_LSP_P_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_PARA_LSP_M_STAT_DATA_SIZE			sizeof(gpnStatPtnLspMData)
#define GPN_STAT_PARA_LSP_M_STAT_GET_MSG_SIZE		((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_LSP_M_STAT_DATA_SIZE))
#define GPN_STAT_MSG_LSP_M_STAT_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_LSP_M_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_LSP_M_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_LSP_M_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_LSP_M_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_LSP_M_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_LSP_M_STAT_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_LSP_M_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_LSP_M_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatPtnLspMDate)
			payload     = gpnStatPtnLspMDate
*/
#define GPN_STAT_MSG_LSP_M_STAT_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_LSP_M_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_LSP_M_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_LSP_STAT_RESET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_LSP_STAT_RESET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_LSP_STAT_RESET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan Type(meaningless index)
			iMsgPara8  = stat sub Type(index)
			msgCellLen = 0
*/

#define GPN_STAT_MSG_LSP_STAT_RESET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_LSP_STAT_RESET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_LSP_STAT_RESET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_PARA_PW_P_STAT_DATA_SIZE			sizeof(gpnStatPtnPwPData)
#define GPN_STAT_PARA_PW_P_STAT_GET_MSG_SIZE		((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_PW_P_STAT_DATA_SIZE))
#define GPN_STAT_MSG_PW_P_STAT_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_PW_P_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PW_P_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_PW_P_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PW_P_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PW_P_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_PW_P_STAT_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_PW_P_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PW_P_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatPtnPwPDate)
			payload     = gpnStatPtnPwPDate
*/
#define GPN_STAT_MSG_PW_P_STAT_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PW_P_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PW_P_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_PARA_PW_M_STAT_DATA_SIZE			sizeof(gpnStatPtnPwMData)
#define GPN_STAT_PARA_PW_M_STAT_GET_MSG_SIZE		((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_PW_M_STAT_DATA_SIZE))
#define GPN_STAT_MSG_PW_M_STAT_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_PW_M_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PW_M_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_PW_M_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PW_M_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PW_M_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_PW_M_STAT_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_PW_M_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PW_M_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatPtnPwMDate)
			payload     = gpnStatPtnPwMDate
*/
#define GPN_STAT_MSG_PW_M_STAT_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PW_M_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PW_M_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_PW_STAT_RESET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_PW_STAT_RESET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PW_STAT_RESET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan Type(meaningless index)
			iMsgPara8  = stat sub Type(index)
			msgCellLen = 0
*/

#define GPN_STAT_MSG_PW_STAT_RESET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PW_STAT_RESET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PW_STAT_RESET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_PARA_VPLSPW_P_STAT_DATA_SIZE			sizeof(gpnStatPtnVplsPwPData)
#define GPN_STAT_PARA_VPLSPW_P_STAT_GET_MSG_SIZE		((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_VPLSPW_P_STAT_DATA_SIZE))
#define GPN_STAT_MSG_VPLSPW_P_STAT_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_VPLSPW_P_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PW_P_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_VPLSPW_P_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_VPLSPW_P_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PW_P_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_VPLSPW_P_STAT_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_VPLSPW_P_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PW_P_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatPtnPwPDate)
			payload     = gpnStatPtnPwPDate
*/
#define GPN_STAT_MSG_VPLSPW_P_STAT_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_VPLSPW_P_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PW_P_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_PARA_VPLSPW_M_STAT_DATA_SIZE			sizeof(gpnStatPtnVplsPwMData)
#define GPN_STAT_PARA_VPLSPW_M_STAT_GET_MSG_SIZE		((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_VPLSPW_M_STAT_DATA_SIZE))
#define GPN_STAT_MSG_VPLSPW_M_STAT_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_VPLSPW_M_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PW_M_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_VPLSPW_M_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_VPLSPW_M_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PW_M_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_VPLSPW_M_STAT_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_VPLSPW_M_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PW_M_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatPtnPwMDate)
			payload     = gpnStatPtnPwMDate
*/
#define GPN_STAT_MSG_VPLSPW_M_STAT_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_VPLSPW_M_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PW_M_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_VPLSPW_STAT_RESET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_VPLSPW_STAT_RESET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PW_STAT_RESET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan Type(meaningless index)
			iMsgPara8  = stat sub Type(index)
			msgCellLen = 0
*/

#define GPN_STAT_MSG_VPLSPW_STAT_RESET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_VPLSPW_STAT_RESET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PW_STAT_RESET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_PARA_PDHPPI_P_STAT_DATA_SIZE			sizeof(gpnStatPdhPpiData)
#define GPN_STAT_PARA_PDHPPI_P_STAT_GET_MSG_SIZE		((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_PDHPPI_P_STAT_DATA_SIZE))
#define GPN_STAT_PARA_PDHPPI_P_STAT_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_PDH_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_PARA_PDHPPI_P_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/

#define GPN_STAT_PARA_PDHPPI_P_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_PARA_PDHPPI_P_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ETH_PHY_P_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_PARA_PDHPPI_P_STAT_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_PARA_PDHPPI_P_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_PARA_PDHPPI_P_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatPdhPpiData)
			payload     = gpnStatPdhPpiData
*/

#define GPN_STAT_PARA_PDHPPI_P_STAT_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_PARA_PDHPPI_P_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ETH_PHY_P_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/


#define GPN_STAT_PARA_ETHPHY_P_STAT_DATA_SIZE		sizeof(gpnStatEthMacPData)
#define GPN_STAT_PARA_ETHPHY_P_STAT_GET_MSG_SIZE	((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_ETHPHY_P_STAT_DATA_SIZE))
#define GPN_STAT_MSG_ETH_PHY_P_STAT_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_ETH_PHY_P_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ETH_PHY_P_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_ETH_PHY_P_STAT_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ETH_PHY_P_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ETH_PHY_P_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_ETH_PHY_P_STAT_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_ETH_PHY_P_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ETH_PHY_P_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatEthMacPDate)
			payload     = gpnStatEthMacPDate
*/
#define GPN_STAT_MSG_ETH_PHY_P_STAT_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ETH_PHY_P_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ETH_PHY_P_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_PARA_ETHPHY_M_STAT_DATA_SIZE		sizeof(gpnStatEthMacMData)
#define GPN_STAT_PARA_ETHPHY_M_STAT_GET_MSG_SIZE	((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_ETHPHY_M_STAT_DATA_SIZE))
#define GPN_STAT_MSG_ETH_PHY_M_STAT_GET			    ((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_ETH_PHY_M_STAT_GET))   /*  */
	/* msg parameters direction: 
				iIndex	= tx_Index++
				iMsgType   = GPN_STAT_MSG_ETH_PHY_M_STAT_GET
				iSrcId	= gpn_stat
				iDstId	= other modu
				iMsgPara1  = devIndex(index)
				iMsgPara2  = portIndex1(index)
				iMsgPara3  = portIndex2(index)
				iMsgPara4  = portIndex3(index)
				iMsgPara5  = portIndex4(index)
				iMsgPara6  = portIndex5(index)
				iMsgPara7  = stat Type(index)
				msgCellLen = 0
	*/
#define GPN_STAT_MSG_ETH_PHY_M_STAT_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ETH_PHY_M_STAT_GET))	/* */
	/* msg parameters direction:
				iIndex	= CMD_Index
				iMsgType   = GPN_STAT_MSG_ETH_PHY_M_STAT_GET_ACK
				iSrcId	= other modu
				iDstId	= gpn_stat
	*/
#define GPN_STAT_MSG_ETH_PHY_M_STAT_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_ETH_PHY_M_STAT_GET))	/* */
	/* msg parameters direction: 
				iIndex	= tx_Index++
				iMsgType   = GPN_STAT_MSG_ETH_PHY_M_STAT_GET_RSP
				iSrcId	= other modu
				iDstId	= gpn_stat
				iMsgPara1  = devIndex(index)
				iMsgPara2  = portIndex1(index)
				iMsgPara3  = portIndex2(index)
				iMsgPara4  = portIndex3(index)
				iMsgPara5  = portIndex4(index)
				iMsgPara6  = portIndex5(index)
				iMsgPara7  = stat Type(index)
				msgCellLen= sizeof(gpnStatEthMacMDate)
				payload 	= gpnStatEthMacMDate
	*/
#define GPN_STAT_MSG_ETH_PHY_M_STAT_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ETH_PHY_M_STAT_GET_RSP))	/* */
	/* msg parameters direction:
				iIndex	= CMD_Index
				iMsgType   = GPN_STAT_MSG_ETH_PHY_M_STAT_GET_RSP_ACK
				iSrcId	= gpn_stat
				iDstId	= other modu
	*/
#if 0 
#define GPN_STAT_PARA_ETHOAM_M_STAT_DATA_SIZE		sizeof(gpnStatEthOamMData)
#define GPN_STAT_PARA_ETHOAM_M_STAT_GET_MSG_SIZE	((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_ETHOAM_M_STAT_DATA_SIZE))
#define GPN_STAT_MSG_ETH_OAM_M_STAT_GET			    ((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_ETH_OAM_M_STAT_GET))   /*  */
	/* msg parameters direction: 
				iIndex	= tx_Index++
				iMsgType   = GPN_STAT_MSG_ETH_PHY_M_STAT_GET
				iSrcId	= gpn_stat
				iDstId	= other modu
				iMsgPara1  = devIndex(index)
				iMsgPara2  = portIndex1(index)
				iMsgPara3  = portIndex2(index)
				iMsgPara4  = portIndex3(index)
				iMsgPara5  = portIndex4(index)
				iMsgPara6  = portIndex5(index)
				iMsgPara7  = stat Type(index)
				msgCellLen = 0
	*/
#define GPN_STAT_MSG_ETH_OAM_M_STAT_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ETH_OAM_M_STAT_GET))	/* */
	/* msg parameters direction:
				iIndex	= CMD_Index
				iMsgType   = GPN_STAT_MSG_ETH_PHY_M_STAT_GET_ACK
				iSrcId	= other modu
				iDstId	= gpn_stat
	*/
#define GPN_STAT_MSG_ETH_OAM_M_STAT_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_ETH_OAM_M_STAT_GET))	/* */
	/* msg parameters direction: 
				iIndex	= tx_Index++
				iMsgType   = GPN_STAT_MSG_ETH_PHY_M_STAT_GET_RSP
				iSrcId	= other modu
				iDstId	= gpn_stat
				iMsgPara1  = devIndex(index)
				iMsgPara2  = portIndex1(index)
				iMsgPara3  = portIndex2(index)
				iMsgPara4  = portIndex3(index)
				iMsgPara5  = portIndex4(index)
				iMsgPara6  = portIndex5(index)
				iMsgPara7  = stat Type(index)
				msgCellLen= sizeof(gpnStatEthMacMDate)
				payload 	= gpnStatEthMacMDate
	*/
#define GPN_STAT_MSG_ETH_OAM_M_STAT_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ETH_OAM_M_STAT_GET_RSP))	/* */
	/* msg parameters direction:
				iIndex	= CMD_Index
				iMsgType   = GPN_STAT_MSG_ETH_PHY_M_STAT_GET_RSP_ACK
				iSrcId	= gpn_stat
				iDstId	= other modu
	*/
#endif

#define GPN_STAT_PARA_ETHSFP_STAT_DATA_SIZE		sizeof(gpnStatEthSFPData)
#define GPN_STAT_PARA_ETHSFP_STAT_GET_MSG_SIZE	((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_ETHSFP_STAT_DATA_SIZE))
#define GPN_STAT_MSG_ETH_SFP_STAT_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_ETH_SFP_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ETH_SFP_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_ETH_SFP_STAT_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ETH_SFP_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ETH_SFP_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_ETH_SFP_STAT_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_ETH_SFP_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ETH_SFP_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatEthSFPDate)
			payload 	= gpnStatEthSFPDate
*/
#define GPN_STAT_MSG_ETH_SFP_STAT_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ETH_SFP_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ETH_SFP_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_ETH_STAT_RESET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_ETH_STAT_RESET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ETH_STAT_RESET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			iMsgPara8  = stat sub Type(index)
			msgCellLen = 0
*/

#define GPN_STAT_MSG_ETH_STAT_RESET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ETH_STAT_RESET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ETH_STAT_RESET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_PARA_FLOW_P_STAT_DATA_SIZE			sizeof(gpnStatFollowPData)
#define GPN_STAT_PARA_FLOW_P_STAT_GET_MSG_SIZE		((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_FLOW_P_STAT_DATA_SIZE))
#define GPN_STAT_MSG_FLOW_P_STAT_GET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_FLOW_P_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_FLOW_P_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_FLOW_P_STAT_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_FLOW_P_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_FLOW_P_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_FLOW_P_STAT_GET_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_FLOW_P_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_FLOW_P_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatFollowData)
			payload     = gpnStatFollowData
*/
#define GPN_STAT_MSG_FLOW_P_STAT_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_FLOW_P_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_FLOW_P_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_PARA_FLOW_M_STAT_DATA_SIZE			sizeof(gpnStatFollowMData)
#define GPN_STAT_PARA_FLOW_M_STAT_GET_MSG_SIZE		((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_FLOW_M_STAT_DATA_SIZE))
#define GPN_STAT_MSG_FLOW_M_STAT_GET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_FLOW_M_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_FLOW_M_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_FLOW_M_STAT_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_FLOW_M_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_FLOW_M_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_FLOW_M_STAT_GET_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_FLOW_M_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_FLOW_M_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatFollowData)
			payload     = gpnStatFollowData
*/
#define GPN_STAT_MSG_FLOW_M_STAT_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_FLOW_M_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_FLOW_M_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_MSG_FLOW_STAT_RESET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_FLOW_STAT_RESET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_FLOW_STAT_RESET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = scan Type(meaningless index)
			iMsgPara8  = stat sub Type(index)
			msgCellLen = 0
*/

#define GPN_STAT_MSG_FLOW_STAT_RESET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_FLOW_STAT_RESET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_FLOW_STAT_RESET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_PARA_PTN_V_UNI_P_STAT_DATA_SIZE	sizeof(gpnStatPtnVUniPData)
#define GPN_STAT_PARA_PTN_V_UNI_P_STAT_GET_MSG_SIZE	((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_PTN_V_UNI_P_STAT_DATA_SIZE))
#define GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_PTN_V_UNI_P_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PTN_VLAN_P_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatPtnVlanPData)
			payload 	= gpnStatPtnVlanPData
*/
#define GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
	
#define GPN_STAT_PARA_PTN_MEP_M_STAT_DATA_SIZE		sizeof(gpnStatPtnMepMData)
#define GPN_STAT_PARA_PTN_MEP_M_STAT_GET_MSG_SIZE	((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_PTN_MEP_M_STAT_DATA_SIZE))
#define GPN_STAT_MSG_PTN_MEP_M_STAT_GET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_PTN_MEP_M_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PTN_MEP_M_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_PTN_MEP_M_STAT_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PTN_MEP_M_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PTN_MEP_M_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_PTN_MEP_M_STAT_GET_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_PTN_MEP_M_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_PTN_MEP_M_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStatPtnMepMData)
			payload 	= gpnStatPtnMepMData
*/
#define GPN_STAT_MSG_PTN_MEP_M_STAT_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_PTN_MEP_M_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_PTN_MEP_M_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_PARA_EQU_STAT_DATA_SIZE			sizeof(gpnStatEquData)
#define GPN_STAT_PARA_EQU_STAT_GET_MSG_SIZE			((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_EQU_STAT_DATA_SIZE))
#define GPN_STAT_MSG_EQU_STAT_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_EQU_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EQU_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_EQU_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EQU_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_EQU_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_EQU_STAT_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_EQU_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_EQU_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStaDevData)
			payload     = gpnStaDevData
*/
#define GPN_STAT_MSG_EQU_STAT_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_EQU_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_EQU_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_PARA_SOFT_STAT_DATA_SIZE			sizeof(gpnStatSoftData)
#define GPN_STAT_PARA_SOFT_STAT_GET_MSG_SIZE		((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_SOFT_STAT_DATA_SIZE))
#define GPN_STAT_MSG_SOFT_STAT_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_SOFT_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SOFT_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_SOFT_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SOFT_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SOFT_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_SOFT_STAT_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_SOFT_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_SOFT_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStaDevData)
			payload     = gpnStaDevData
*/
#define GPN_STAT_MSG_SOFT_STAT_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_SOFT_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_SOFT_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/

#define GPN_STAT_PARA_ENV_STAT_DATA_SIZE			sizeof(gpnStatEnvData)
#define GPN_STAT_PARA_ENV_STAT_GET_MSG_SIZE			((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_STAT_PARA_ENV_STAT_DATA_SIZE))
#define GPN_STAT_MSG_ENV_STAT_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_DATA_SEARCH_BASE_SCANTYPE)|(GPN_STAT_SST_ENV_STAT_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ENV_STAT_GET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_ENV_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ENV_STAT_GET))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ENV_STAT_GET_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_ENV_STAT_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_ENV_STAT_GET))	/* */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_ENV_STAT_GET_RSP
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat Type(index)
			msgCellLen= sizeof(gpnStaDevData)
			payload     = gpnStaDevData
*/
#define GPN_STAT_MSG_ENV_STAT_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_ENV_STAT_GET_RSP))	/* */
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_STAT_MSG_ENV_STAT_GET_RSP_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/


/***************************************************************************** **********/
/*define main type 4's main-sub type  (from 1 to 0xFE)  : 3                                                                        */
/****************************************************************************************/
#define GPN_STAT_MST_STAT_DATA_QUREY				((3 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
#define GPN_STAT_SST_CURR_DATA_GET					((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_CURR_DATA_GET_NEXT				((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_HIST_DATA_GET					((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_HIST_DATA_GET_NEXT				((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_HIST_TRAP_DATA_GET				((5 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_HIST_TRAP_DATA_GET_NEXT		((6 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_STAT_SST_CURR_DATA_MODIFY				((9 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)


typedef struct _gpnStatMsgCurrDataIndex_
{
	stStatObjDesc portIndex;
	UINT32 statSubType;
}gpnStatMsgCurrDataIndex;

#define GPN_STAT_MSG_CURR_DATA_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_STAT_DATA_QUREY)|(GPN_STAT_SST_CURR_DATA_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_CURR_DATA_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat sub Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_CURR_DATA_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_CURR_DATA_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_CURR_DATA_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_CURR_DATA_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_CURR_DATA_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_CURR_DATA_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat sub Type(index)
			iMsgPara8  = stat scan Type
			iMsgPara9  = sub type data high 32bit
			iMsgParaA  = sub type data low 32bit
*/
#define GPN_STAT_MSG_CURR_DATA_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_CURR_DATA_GET_RSP))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_CURR_DATA_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_CURR_DATA_GET_NEXT				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_STAT_DATA_QUREY)|(GPN_STAT_SST_CURR_DATA_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_FLOW_STAT_RESET
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat sub Type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_CURR_DATA_GET_NEXT_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_CURR_DATA_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_CURR_DATA_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_CURR_DATA_GET_NEXT_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_CURR_DATA_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_CURR_DATA_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat sub Type(index)
			iMsgPara8  = stat scan Type
			iMsgPara9  = sub type data high 32bit
			iMsgParaA  = sub type data low 32bit
			msgCellLen = sizeof(gpnStatMsgCurrDataIndex)
			payload = next curr data index
*/
#define GPN_STAT_MSG_CURR_DATA_GET_NEXT_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_CURR_DATA_GET_NEXT_RSP))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_CURR_DATA_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
#define GPN_STAT_MSG_CURR_DATA_MODIFY			((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_GLOBAL_OPT)|(GPN_STAT_MST_STAT_TYPE_OPT)|(GPN_STAT_SST_CURR_DATA_MODIFY))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_CURR_DATA_MODIFY
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat sub Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_CURR_DATA_MODIFY_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_SST_CURR_DATA_MODIFY))	/* */
/* msg parameters direction: 
			iIndex	   = CMD_Index
			iMsgType   = GPN_STAT_MSG_CURR_DATA_MODIFY_ACK
			iSrcId	   = gpn_stat
			iDstId	   = other modu
			iMsgPara1  = devIndex(index)
			iMsgPara2  = portIndex1(index)
			iMsgPara3  = portIndex2(index)
			iMsgPara4  = portIndex3(index)
			iMsgPara5  = portIndex4(index)
			iMsgPara6  = portIndex5(index)
			iMsgPara7  = stat sub Type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_HIST_DATA_GET					((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_STAT_DATA_QUREY)|(GPN_STAT_SST_HIST_DATA_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = hist data location(index)
			iMsgPara2  = hist cyc stop time(index)
			iMsgPara3  = hist stat sub type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_HIST_DATA_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_HIST_DATA_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_HIST_DATA_GET_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_HIST_DATA_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = hist data location(index)
			iMsgPara2  = hist cyc stop time(index)
			iMsgPara3  = hist stat sub type(index)
			iMsgPara4  = stat scan Type
			iMsgPara5  = sub type hist data histhigh 32bit
			iMsgPara6  = sub type hist data low 32bit
*/
#define GPN_STAT_MSG_HIST_DATA_GET_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_HIST_DATA_GET_RSP))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_HIST_DATA_GET_NEXT				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_STAT_DATA_QUREY)|(GPN_STAT_SST_HIST_DATA_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_SST_HIST_DATA_GET_NEXT
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = hist data location(index)
			iMsgPara2  = hist cyc stop time(index)
			iMsgPara3  = hist stat sub type(index)
			msgCellLen = 0
*/
#define GPN_STAT_MSG_HIST_DATA_GET_NEXT_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_HIST_DATA_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_HIST_DATA_GET_NEXT_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_HIST_DATA_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = hist data location(index)
			iMsgPara2  = hist cyc stop time(index)
			iMsgPara3  = hist stat sub type(index)
			iMsgPara4  = stat scan Type
			iMsgPara5  = sub type hist data histhigh 32bit
			iMsgPara6  = sub type hist data low 32bit
			iMsgPara7  = hist data location(next index)
			iMsgPara8  = hist cyc stop time(next index)
			iMsgPara9  = hist stat sub type(next index)
*/
#define GPN_STAT_MSG_HIST_DATA_GET_NEXT_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_HIST_DATA_GET_NEXT_RSP))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET_NEXT_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/

#define GPN_STAT_MSG_HIST_TRAP_DATA_GET				((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_STAT_DATA_QUREY)|(GPN_STAT_SST_HIST_TRAP_DATA_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_TRAP_DATA_GET
			iSrcId	= other modu
			iDstId	= gpn_stat
			iMsgPara1  = hist data trap index
			iMsgPara2  = hist stat sub type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_HIST_TRAP_DATA_GET_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_HIST_TRAP_DATA_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_HIST_TRAP_DATA_GET_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_HIST_DATA_GET))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = hist data trap index(index)
			iMsgPara2  = hist stat sub type(index)
			iMsgPara3  = portIndex1
			iMsgPara4  = portIndex2
			iMsgPara5  = portIndex3
			iMsgPara6  = portIndex4
			iMsgPara7  = portIndex5
			iMsgPara8  = devIndex
			iMsgPara9  = hist stat stop time
			iMsgParaA  = stat scan Type
			iMsgParaB  = stat cyc class
			iMsgParaC  = sub type hist trap data histhigh 32bit
			iMsgParaD  = sub type hist trap data low 32bit
*/
#define GPN_STAT_MSG_HIST_TRAP_DATA_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_HIST_TRAP_DATA_GET_RSP))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/


#define GPN_STAT_MSG_HIST_TRAP_DATA_GET_NEXT		((GPN_SOCK_OWNER_STAT)|(GPN_STAT_MT_DATA_COLLECT_OPT)|(GPN_STAT_MST_STAT_DATA_QUREY)|(GPN_STAT_SST_HIST_TRAP_DATA_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_TRAP_DATA_GET_NEXT
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = hist data trap index
			iMsgPara2  = hist stat sub type
			msgCellLen = 0
*/
#define GPN_STAT_MSG_HIST_TRAP_DATA_GET_NEXT_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_HIST_TRAP_DATA_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
*/
#define GPN_STAT_MSG_HIST_TRAP_DATA_GET_NEXT_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_STAT_MSG_HIST_DATA_GET_NEXT))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET_NEXT_ACK
			iSrcId	= gpn_stat
			iDstId	= other modu
			iMsgPara1  = hist data trap index(index)
			iMsgPara2  = hist stat sub type(index)
			iMsgPara3  = portIndex1
			iMsgPara4  = portIndex2
			iMsgPara5  = portIndex3
			iMsgPara6  = portIndex4
			iMsgPara7  = portIndex5
			iMsgPara8  = devIndex
			iMsgPara9  = hist stat stop time
			iMsgParaA  = stat scan Type
			iMsgParaB  = stat cyc class
			iMsgParaC  = sub type hist trap data histhigh 32bit
			iMsgParaD  = sub type hist trap data low 32bit
			iMsgParaE  = hist data trap index(next index)
			iMsgParaF  = hist stat sub type(next index)
*/
#define GPN_STAT_MSG_HIST_TRAP_DATA_GET_NEXT_RSP_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_STAT_MSG_HIST_TRAP_DATA_GET_NEXT_RSP))   /*  */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_STAT_MSG_HIST_DATA_GET_NEXT_RSP_ACK
			iSrcId	= other modu
			iDstId	= gpn_stat
*/
UINT32 gpnSockStatMsgApiNsmPtnVsMStatMonStaCfg(void);
UINT32 gpnSockStatMsgApiPortStatMonEnable(optObjOrient *pportIndex, UINT32 statType, UINT32 dstModu);
UINT32 gpnSockStatMsgApiPortStatMonDisable(optObjOrient *pportIndex, UINT32 statType, UINT32 dstModu);
UINT32 gpnSockStatMsgApiPortStatMonReplace(optObjOrient *pOldPort, optObjOrient *pNewPort, UINT32 dstModu);
UINT32 gpnSockStatMsgApiNsmPtnVsPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiPtnVsMStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiNsmPtnLspPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiPtnLspMStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiNsmPtnPwPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiPtnPwMStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiNsmEthPhyPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiEthMonStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiSecMonStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiEthMonStatMonPortReplace(optObjOrient *pOldPort, optObjOrient *pNewPort);
UINT32 gpnSockStatMsgApiSdmSFPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiNsmFollowPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiNsmFollowMStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiNsmPtnVUNIPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiOamPtnMepMStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiSdmEquStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiSdmSoftStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiSdmEnvStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiTxPdhPpiStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiPdhPpiStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiNsmSFPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiTxPtnVsPStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxPtnVsPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiNsmPtnVsCounterReset(optObjOrient *pportIndex, UINT32 scanType, UINT32 subType);
UINT32 gpnSockStatMsgApiTxPtnVsMStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxPtnVsMStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxPtnLspPStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxPtnLspPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiNsmPtnLspCounterReset(optObjOrient *pportIndex, UINT32 scanType, UINT32 subType);
UINT32 gpnSockStatMsgApiTxPtnLspMStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxPtnLspMStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxPtnPwPStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxPtnPwPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiNsmPtnPwCounterReset(optObjOrient *pportIndex, UINT32 scanType, UINT32 subType);
UINT32 gpnSockStatMsgApiTxPtnPwMStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxPtnPwMStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiNsmPtnVplsPwPStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiNsmPtnVplsPwMStatMonStaCfg(optObjOrient *pportIndex, UINT32 en);
UINT32 gpnSockStatMsgApiTxPtnVplsPwPStatGet(optObjOrient *pportIndex, UINT32 statType);
UINT32 gpnSockStatMsgApiTxPtnVplsPwPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxPtnVplsPwMStatGet(optObjOrient *pportIndex, UINT32 statType);
UINT32 gpnSockStatMsgApiTxPtnVplsPwMStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxNsmEthPhyPStatGet(optObjOrient *pportIndex, UINT32 statType);
UINT32 gpnSockStatMsgApiTxNsmEthPhyPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxSecMonStatGet(optObjOrient *pportIndex, UINT32 statType);
UINT32 gpnSockStatMsgApiTxSecMonStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxEthMonStatGet(optObjOrient *pportIndex, UINT32 statType);
UINT32 gpnSockStatMsgApiTxEthMonStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiNsmEthCounterReset(optObjOrient *pportIndex, UINT32 scanType, UINT32 subType);
UINT32 gpnSockStatMsgApiTxSdmEthSFPStatGet(optObjOrient *pportIndex, UINT32 statType);
UINT32 gpnSockStatMsgApiTxsdmEthSFPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxFollowPStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxFollowPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxFollowMStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxFollowMStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiNsmFollowCounterReset(optObjOrient *pportIndex, UINT32 scanType, UINT32 subType);
UINT32 gpnSockStatMsgApiTxPtnVUNIPStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxPtnVUNIPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxPtnMepMStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxPtnMepMStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxSdmEquStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxSdmEquStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxSdmSoftStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxSdmSoftStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxSdmEnvStatGet(optObjOrient *pportIndex, UINT32 statType/*no meaningless ???*/);
UINT32 gpnSockStatMsgApiTxSdmEnvStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiPortStatMonEnableRsp(optObjOrient *pportIndex, UINT32 scanType, UINT32 enStat);
UINT32 gpnSockStatMsgApiPortStatMonDisableRsp(optObjOrient *pportIndex, UINT32 scanType, UINT32 disStat);
UINT32 gpnSockStatMsgApiNsmFollowCounterResetRsp(optObjOrient *pportIndex, UINT32 scanType, UINT32 resetStat);
UINT32 gpnSockStatMsgApiPdhPpiStatGet(optObjOrient *pportIndex, UINT32 statType);
UINT32 gpnSockStatMsgApiPdhPpiStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);
UINT32 gpnSockStatMsgApiTxNsmEthSFPStatGet(optObjOrient *pportIndex, UINT32 statType);
UINT32 gpnSockStatMsgApiTxNsmEthSFPStatGetRsp(optObjOrient *pportIndex, UINT32 statType, void *pdata, UINT32 len);

UINT32 gpnSockStatMsgApiNsmFollowStatMonEnableRsp(optObjOrient *pportIndex, UINT32 scanType, UINT32 enStat);
UINT32 gpnSockStatMsgApiNsmFollowStatMonDisableRsp(optObjOrient *pportIndex, UINT32 scanType, UINT32 disStat);

UINT32 gpnStatCalcCycle(optObjOrient *pportIndex, UINT32 statType);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_STAT_MSG_DEF_H_*/


