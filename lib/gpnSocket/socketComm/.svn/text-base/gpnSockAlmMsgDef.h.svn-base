/**********************************************************
* file name: gpnSockAlmMsgDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-11
* function: 
*    define details about communication between alarm modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_ALM_MSG_DEF_H_
#define _GPN_SOCK_ALM_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"

#include "lib/gpnSocket/socketComm/gpnSockMsgDef.h"

#include "lib/gpnSocket/socketComm/gpnSockIfmMsgDef.h"
#include "lib/gpnSocket/socketComm/gpnAlmTypeDef.h"

/**********************************/
/*define owner : GPN_SOCK_OWNER_ALM*/
/**********************************/

/*================================================================*/
/*define main type  (from 1 to 0x7E)  : 1                                                                                                  */
/***************************************************************************************/
/*this main type include alarm module how to get alarm and event status(include poll and auto report proc)    */
#define GPN_ALM_MT_STATUS_GET				((1 << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 1                                                                        */
/****************************************************************************************/
#define GPN_ALM_MST_ALARM_GET				((1 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
#define GPN_ALM_SST_ALM_REPT_TYPE_BASE		((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_REPT_BITS_BASE		((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_ACTIVE_POLL			((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_REPT_CFM_BASE		((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/****************************************************************************************/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 2                                                                         */
/****************************************************************************************/
#define GPN_ALM_MST_EVENT_GET				((2 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
#define GPN_ALM_SST_EVT_REPT_TYPE_BASE		((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_EVT_REPT_BITS_BASE		((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_EVT_ACTIVE_POLL			((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/****************************************************************************************/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 3                                                                         */
/****************************************************************************************/
#define GPN_ALM_MST_PORC_RESULT_VIEW		((3 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
#define GPN_ALM_SST_PROC_RESULT_FILE_VIEW	((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)


#define GPN_ALM_MSG_ALM_REPT_TYPE_BASE		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_STATUS_GET)|(GPN_ALM_MST_ALARM_GET)|(GPN_ALM_SST_ALM_REPT_TYPE_BASE))   /* alarm status auto report base alarm type */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_ALM_MSG_ALM_REPT_TYPE_BASE
                   	iSrcId     	= other modu
                   	iDstId     	= ALM
                   	iMsgPara1  = portIndex1
                   	iMsgPara2  = portIndex2
                   	iMsgPara3  = portIndex3
                   	iMsgPara4  = portIndex4
                   	iMsgPara5  = portIndex5
                   	iMsgPara6  = portIndex6 (device index ???)
			iMsgPara7  = alm type
			iMsgPara8  = rise/clean
			iMsgPara9  = sender's pid(getpid())
                   	msgCellLen = 0
*/
#define GPN_ALM_MSG_ALM_REPT_TYPE_BASE_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_ALM_REPT_TYPE_BASE))	/* alarm status auto report base alarm type ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_ALM_MSG_ALM_REPT_TYPE_BASE_ACK
			iSrcId	= ALM
			iDstId	= other modu
*/

#define GPN_ALM_MSG_ALM_REPT_BITS_BASE		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_STATUS_GET)|(GPN_ALM_MST_ALARM_GET)|(GPN_ALM_SST_ALM_REPT_BITS_BASE))   /*poll alarm status base alarm bits position  */
/* msg parameters direction:
			iIndex	   = tx_Index++
                   	iMsgType   = GPN_ALM_MSG_ALM_REPT_BITS_BASE
                   	iSrcId     	= other modu
                   	iDstId     	= ALM
                   	iMsgPara1  = portIndex1
                   	iMsgPara2  = portIndex2
                   	iMsgPara3  = portIndex3
                   	iMsgPara4  = portIndex4
                   	iMsgPara5  = portIndex5
                   	iMsgPara6  = devIndex
                   	iMsgPara7  = page Num n
			iMsgPara8  = alm info page n
			iMsgPara9  = alm info page n-1
			iMsgParaA  = alm info page n-2
			iMsgParaB  = alm info page n-3
			iMsgParaC  = alm info page n-4
			iMsgParaD  = alm info page n-5
			iMsgParaE  = alm info page n-6
			iMsgParaF  = alm info page n-7
                   	msgCellLen = 0
*/
#define GPN_ALM_MSG_ALM_REPT_BITS_BASE_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_ALM_REPT_BITS_BASE))	/* alarm status auto report base alarm type ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_ALM_MSG_ALM_REPT_BITS_BASE_ACK
			iSrcId	= ALM
			iDstId	= other modu
*/

#define GPN_ALM_MSG_ALM_ACTIVE_POLL			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_STATUS_GET)|(GPN_ALM_MST_ALARM_GET)|(GPN_ALM_SST_ALM_ACTIVE_POLL))   /*active poll alarm stauts*/
/* msg parameters direction:
			iIndex	= tx_Index++
			iMsgType   = GPN_ALM_MSG_ALM_ACTIVE_POLL
                   	iSrcId     	= ALM
                   	iDstId     	= other modu
                   	iMsgPara2  = portIndex1
                   	iMsgPara3  = portIndex2
                   	iMsgPara4  = portIndex3
                   	iMsgPara5  = portIndex4
                   	iMsgPara6  = portIndex5
                   	iMsgPara7  = portIndex6
                   	msgCellLen = 0
*/
#define GPN_ALM_MSG_ALM_ACTIVE_POLL_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_ALM_ACTIVE_POLL))	/*active poll alarm stauts ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_ALM_MSG_ALM_ACTIVE_POLL_ACK
			iSrcId	= other modu
			iDstId	= ALM
*/

#define GPN_ALM_MSG_ALM_ACTIVE_POLL_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_ALM_ACTIVE_POLL))   /*active poll alarm stauts response */
/* msg parameters direction:
			iIndex	= tx_Index++
			iMsgType  	= GPN_ALM_MSG_ACTIVE_COLL_RSP
                   	iSrcId     	= other modu
                   	iDstId     	= ALM
                   	iMsgPara2  = portIndex1
                   	iMsgPara3  = portIndex2
                   	iMsgPara4  = portIndex3
                   	iMsgPara5  = portIndex4
                   	iMsgPara6  = portIndex5
                   	iMsgPara7  = portIndex6
			iMsgPara8  = alm info page3
			iMsgPara9  = alm info page2
			iMsgParaA  = alm info page1
			iMsgParaB  = alm info page0
                   	msgCellLen = 0
*/
#define GPN_ALM_MSG_ALM_ACTIVE_POLL_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_ALM_ACTIVE_POLL_RSP))	/*active poll alarm stauts response ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_ALM_MSG_ALM_ACTIVE_POLL_RSP_ACK
			iSrcId	= ALM
			iDstId	= other modu
*/

#define GPN_ALM_MSG_ALM_REPT_CFM_BASE		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_STATUS_GET)|(GPN_ALM_MST_ALARM_GET)|(GPN_ALM_SST_ALM_REPT_CFM_BASE))   /* alarm status auto report base CFM alarm type */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_ALM_MSG_ALM_REPT_CFM_BASE
                   	iSrcId     	= other modu
                   	iDstId     	= ALM
                   	iMsgPara1  = portIndex1
                   	iMsgPara2  = portIndex2
                   	iMsgPara3  = portIndex3
                   	iMsgPara4  = portIndex4
                   	iMsgPara5  = portIndex5
                   	iMsgPara6  = portIndex6
			iMsgPara7  = alm type
			iMsgPara8  = rise/clean
			iMsgPara9  = sender's pid(getpid())
                   	msgCellLen = 0
*/
#define GPN_ALM_MSG_ALM_REPT_CFM_BASE_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_ALM_REPT_CFM_BASE))	/* alarm status auto report base CFM alarm type ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_ALM_MSG_ALM_REPT_CFM_BASE_ACK
			iSrcId	= ALM
			iDstId	= other modu
*/


#define GPN_ALM_MSG_EVT_REPT_TYPE_BASE		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_STATUS_GET)|(GPN_ALM_MST_EVENT_GET)|(GPN_ALM_SST_EVT_REPT_TYPE_BASE))   /* event status auto report base event type */
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType  	= GPN_ALM_MSG_EVT_REPT_TYPE_BASE
			iSrcId	= other modu
			iDstId	= ALM
			iMsgPara1  = portIndex1
			iMsgPara2  = portIndex2
			iMsgPara3  = portIndex3
			iMsgPara4  = portIndex4
			iMsgPara5  = portIndex5
			iMsgPara6  = portIndex6
			iMsgPara7  = alm type
			iMsgPara8  = detial
			iMsgPara9  = sender's pid(getpid())
			msgCellLen = 0
*/
#define GPN_ALM_MSG_EVT_REPT_TYPE_BASE_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_REPT_TYPE_BASE))	/* event status auto report base event type ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_ALM_MSG_EVT_REPT_TYPE_BASE_ACK
			iSrcId	= ALM
			iDstId	= other modu
*/
	
#define GPN_ALM_MSG_EVT_REPT_BIT_BASE		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_STATUS_GET)|(GPN_ALM_MST_EVENT_GET)|(GPN_ALM_SST_EVT_REPT_BITS_BASE))   /*poll event status base event bits position  */
/* msg parameters direction:
			iIndex	   = tx_Index++
			iMsgType   = GPN_ALM_MSG_EVT_REPT_BIT_BASE
			iSrcId		= other modu
			iDstId		= ALM
			iMsgPara1  = portIndex1
			iMsgPara2  = portIndex2
			iMsgPara3  = portIndex3
			iMsgPara4  = portIndex4
			iMsgPara5  = portIndex5
			iMsgPara6  = portIndex6
			iMsgPara7  = event info (max 32)
			msgCellLen = 0
*/
#define GPN_ALM_MSG_EVT_REPT_BITS_BASE_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_REPT_BIT_BASE))	/* event status auto report base event type ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_ALM_MSG_EVT_REPT_BITS_BASE_ACK
			iSrcId	= ALM
			iDstId	= other modu
*/
	
#define GPN_ALM_MSG_EVT_ACTIVE_POLL			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_STATUS_GET)|(GPN_ALM_MST_EVENT_GET)|(GPN_ALM_SST_EVT_ACTIVE_POLL))   /*active poll event stauts*/
/* msg parameters direction:
			iIndex	= tx_Index++
			iMsgType   = GPN_ALM_MSG_ALM_ACTIVE_POLL
			iSrcId		= ALM
			iDstId		= other modu
			iMsgPara1  = portIndex1
			iMsgPara2  = portIndex2
			iMsgPara3  = portIndex3
			iMsgPara4  = portIndex4
			iMsgPara5  = portIndex5
			iMsgPara6  = portIndex6
			msgCellLen = 0
*/
#define GPN_ALM_MSG_EVT_ACTIVE_POLL_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_ACTIVE_POLL))	/*active poll event stauts ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_ALM_MSG_ALM_ACTIVE_POLL_ACK
			iSrcId	= other modu
			iDstId	= ALM
*/
#define GPN_ALM_MSG_EVT_ACTIVE_POLL_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_EVT_ACTIVE_POLL))   /*active poll event stauts response */
/* msg parameters direction:
			iIndex	= tx_Index++
			iMsgType	= GPN_ALM_MSG_EVT_ACTIVE_COLL_RSP
			iSrcId		= other modu
			iDstId		= ALM
			iMsgPara1  = portIndex1
			iMsgPara2  = portIndex2
			iMsgPara3  = portIndex3
			iMsgPara4  = portIndex4
			iMsgPara5  = portIndex5
			iMsgPara6  = portIndex6
			iMsgPara7  = event info (max 32)
			msgCellLen = 0
*/
#define GPN_ALM_MSG_EVT_ACTIVE_POLL_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_ACTIVE_POLL_RSP))	/*active poll event stauts response ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_ALM_MSG_EVT_ACTIVE_POLL_RSP_ACK
			iSrcId	= ALM
			iDstId	= other modu
*/


#define GPN_ALM_MSG_PARA_RESULT_CURR        1
#define GPN_ALM_MSG_PARA_RESULT_HIST        2
#define GPN_ALM_MSG_PARA_RESULT_EVENT       3
#define GPN_ALM_MSG_PAPA_RESULT_VIEW_PATH	265
#define GPN_ALM_MSG_PAPA_RESULT_VIEW_BUFF	((GPN_SOCK_MSG_HEAD_BLEN)+GPN_ALM_MSG_PAPA_RESULT_VIEW_PATH)
#define GPN_ALM_MSG_PORC_RESULT_FILE_VIEW	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_STATUS_GET)|(GPN_ALM_MST_PORC_RESULT_VIEW)|(GPN_ALM_SST_PROC_RESULT_FILE_VIEW))   /**/
/* msg parameters direction:
			iIndex	= tx_Index++
			iMsgType   = GPN_ALM_MSG_PORC_RESULT_FILE_VIEW
			iMsgPara1  = type(curr, hist, event)
			iSrcId	= IMI
			iDstId	= ALM
			iMsgPara1  = result type
			msgCellLen = sizeof(path)
			payload = path
*/
#define GPN_ALM_MSG_PORC_RESULT_FILE_VIEW_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_PORC_RESULT_FILE_VIEW))	/*active poll event stauts ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_ALM_MSG_PORC_RESULT_FILE_VIEW_ACK
			iSrcId	= ALM
			iDstId	= IMI
*/


/*================================================================*/
/*define main type  (from 1 to 0x7E)  : 2                                                                                                  */
/***************************************************************************************/
/*this main type include alarm module how to get alarm and event status(include poll and auto report proc)    */
#define GPN_ALM_MT_ALM_ATRRIB_CFG			((2 << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 1                                                                        */
/****************************************************************************************/
#define GPN_ALM_MST_ALM_MON_CFG				((1 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
#define GPN_ALM_SST_ALM_MON_CFG				((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/****************************************************************************************/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 2                                                                         */
/****************************************************************************************/
#define GPN_ALM_MST_ALM_CFG_TYPE_BASE		((2 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
#define GPN_ALM_SST_ALM_TYPE_BASE_SCREEN	((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_TYPE_BASE_FILT		((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_TYPE_BASE_REPORT	((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_TYPE_BASE_RECORD	((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_TYPE_BASE_RANK		((5 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/****************************************************************************************/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  :3                                                                         */
/****************************************************************************************/
#define GPN_ALM_MST_ALM_CFG_PORT_BASE		((3 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
#define GPN_ALM_SST_ALM_PORT_BASE_SCREEN	((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_PORT_BASE_FILT		((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_PORT_BASE_REPORT	((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_PORT_BASE_RECORD	((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_PORT_BASE_RANK		((5 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_PORT_BASE_REVER		((6 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/****************************************************************************************/

#define  GPN_ALM_MSG_PORT_ALM_MON_CFG  		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ATRRIB_CFG)|(GPN_ALM_MST_ALM_MON_CFG)|(GPN_ALM_SST_ALM_MON_CFG))
/* msg parameters direction:
			iIndex	   = tx_Index++
			iMsgType   = GPN_ALM_MSG_PORT_ALM_MON_CFG
		   	iSrcId     	= other modu
                   	iDstId     	= ALM
		   	iMsgPara1  = location index num(1=<i<=6)
		   	iMsgPara2  = portIndex1
		   	iMsgPara3  = portIndex2
		   	iMsgPara4  = portIndex3
		   	iMsgPara5  = portIndex4
		   	iMsgPara6  = portIndex5
		   	iMsgPara7  = portIndex6
		   	iMsgPara8  = 1/2(enable/disable)
		   	iMsgPara9  = 0
		   	msgCellLen = 0
*/
#define  GPN_ALM_MSG_PORT_ALM_MON_CFG_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_PORT_ALM_MON_CFG))
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_ALM_MSG_PORT_ALM_MON_CFG_ACK
			iSrcId	= ALM
			iDstId	= other modu
*/


#define  GPN_ALM_MSG_TYPE_BASE_SCREEN  		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ATRRIB_CFG)|(GPN_ALM_MST_ALM_CFG_TYPE_BASE)|(GPN_ALM_SST_ALM_TYPE_BASE_SCREEN))
#define  GPN_ALM_MSG_TYPE_BASE_FILT   		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ATRRIB_CFG)|(GPN_ALM_MST_ALM_CFG_TYPE_BASE)|(GPN_ALM_SST_ALM_TYPE_BASE_FILT))
#define  GPN_ALM_MSG_TYPE_BASE_REPORT		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ATRRIB_CFG)|(GPN_ALM_MST_ALM_CFG_TYPE_BASE)|(GPN_ALM_SST_ALM_TYPE_BASE_REPORT))
#define  GPN_ALM_MSG_TYPE_BASE_RECORD		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ATRRIB_CFG)|(GPN_ALM_MST_ALM_CFG_TYPE_BASE)|(GPN_ALM_SST_ALM_TYPE_BASE_RECORD))
#define  GPN_ALM_MSG_TYPE_BASE_RANK			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ATRRIB_CFG)|(GPN_ALM_MST_ALM_CFG_TYPE_BASE)|(GPN_ALM_SST_ALM_TYPE_BASE_RANK))
/* msg parameters direction:  
			iIndex	= tx_Index++
			iMsgType  	= above
                   	iSrcId     	= other modu
                   	iDstId     	= ALM
                   	iMsgPara1  = alm type
                   	iMsgPara2  = enable/disbale
                   	msgCellLen = 0
*/
#define  GPN_ALM_MSG_TYPE_BASE_SCREEN_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_TYPE_BASE_SCREEN))
#define  GPN_ALM_MSG_TYPE_BASE_FILT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_TYPE_BASE_FILT))
#define  GPN_ALM_MSG_TYPE_BASE_REPORT_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_TYPE_BASE_REPORT))
#define  GPN_ALM_MSG_TYPE_BASE_RECORD_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_TYPE_BASE_RECORD))
#define  GPN_ALM_MSG_TYPE_BASE_RANK_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_TYPE_BASE_RANK))
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = above
			iSrcId	= ALM
			iDstId	= other modu
*/

#define  GPN_ALM_MSG_PORT_BASE_SCREEN  		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ATRRIB_CFG)|(GPN_ALM_MST_ALM_CFG_PORT_BASE)|(GPN_ALM_SST_ALM_PORT_BASE_SCREEN))
#define  GPN_ALM_MSG_PORT_BASE_FILT   		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ATRRIB_CFG)|(GPN_ALM_MST_ALM_CFG_PORT_BASE)|(GPN_ALM_SST_ALM_PORT_BASE_FILT))
#define  GPN_ALM_MSG_PORT_BASE_REPORT   	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ATRRIB_CFG)|(GPN_ALM_MST_ALM_CFG_PORT_BASE)|(GPN_ALM_SST_ALM_PORT_BASE_REPORT))
#define  GPN_ALM_MSG_PORT_BASE_RECORD   	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ATRRIB_CFG)|(GPN_ALM_MST_ALM_CFG_PORT_BASE)|(GPN_ALM_SST_ALM_PORT_BASE_RECORD))
#define  GPN_ALM_MSG_PORT_BASE_RANK   		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ATRRIB_CFG)|(GPN_ALM_MST_ALM_CFG_PORT_BASE)|(GPN_ALM_SST_ALM_PORT_BASE_RANK))
#define  GPN_ALM_MSG_PORT_BASE_REVER    	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ATRRIB_CFG)|(GPN_ALM_MST_ALM_CFG_PORT_BASE)|(GPN_ALM_SST_ALM_PORT_BASE_REVER))
/* msg parameters direction:  
			iIndex	= tx_Index++
			iMsgType  	= above
		   	iSrcId     	= other modu
                   	iDstId     	= ALM
		   	iMsgPara1  = location index num(1=<i<=6)
                   	iMsgPara2  = portIndex1
                   	iMsgPara3  = portIndex2
                   	iMsgPara4  = portIndex3
                   	iMsgPara5  = portIndex4
                   	iMsgPara6  = portIndex5
                   	iMsgPara7  = portIndex6
		   	iMsgPara8  = alm type
		   	iMsgPara9  = cancel/enable/disbale
		   	其他参数为0
		   	msgCellLen = 0
*/
#define  GPN_ALM_MSG_PORT_BASE_SCREEN_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_PORT_BASE_SCREEN))
#define  GPN_ALM_MSG_PORT_BASE_FILT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_PORT_BASE_FILT))
#define  GPN_ALM_MSG_PORT_BASE_REPORT_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_PORT_BASE_REPORT))
#define  GPN_ALM_MSG_PORT_BASE_RECORD_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_PORT_BASE_RECORD))
#define  GPN_ALM_MSG_PORT_BASE_RANK_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_PORT_BASE_RANK))
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = above
			iSrcId	= ALM
			iDstId	= other modu
*/

/*================================================================*/
/*define main type  (from 1 to 0x7E)  : 3                                                                                                  */
/***************************************************************************************/
/*this main type include alarm module how to syn config and status                                                            */
#define GPN_ALM_MT_DATA_SYN					((3 << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 1                                                                        */
/****************************************************************************************/
#define GPN_ALM_MST_DB_SYN					((1 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                              */
/****************************************************************************************/
#define GPN_ALM_SST_DB_SYN_XXX				((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/****************************************************************************************/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 2                                                                         */
/****************************************************************************************/
#define GPN_ALM_MST_STATUS_NOTIFY			((1 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                              */
/****************************************************************************************/
#define GPN_ALM_SST_ALM_NOT_SUB2NM			((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_NOT_MAS2SLA			((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_EVT_NOT_SUB2NM			((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_EVT_NOT_MAS2SLA			((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/****************************************************************************************/

#define GPN_ALM_MSG_ALM_NOT_SUB2NM			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_DATA_SYN)|(GPN_ALM_MST_STATUS_NOTIFY)|(GPN_ALM_SST_ALM_NOT_SUB2NM)) /*sub solt notify self's alarm status to master NM*/
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType  	= GPN_ALM_MSG_ALM_NOT_SUB2NM
					iSrcId	= SUB-ALM
					iDstId	= MAS-ALM
					iMsgPara1  = location index num(1=<i<=6)
				   	iMsgPara2  = portIndex1
				   	iMsgPara3  = portIndex2
				   	iMsgPara4  = portIndex3
				   	iMsgPara5  = portIndex4
				   	iMsgPara6  = portIndex5
				   	iMsgPara7  = portIndex6
				   	iMsgPara8  = alm type
				   	iMsgPara9  = rise/clean
				   	msgCellLen = 0
*/
#define GPN_ALM_MSG_ALM_NOT_MAS2SLA			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_DATA_SYN)|(GPN_ALM_MST_STATUS_NOTIFY)|(GPN_ALM_SST_ALM_NOT_MAS2SLA))   /*master NM notify self's alarmstatus to slave NM*/
/* msg parameters direction: 
					iIndex	   = tx_Index++
					iMsgType   = GPN_ALM_MSG_ALM_NOT_MAS2SLA
					iSrcId	= SUB-ALM
					iDstId	= MAS-ALM
					iMsgPara1  = location index num(1=<i<=6)
					iMsgPara2  = portIndex1
					iMsgPara3  = portIndex2
					iMsgPara4  = portIndex3
					iMsgPara5  = portIndex4
					iMsgPara6  = portIndex5
					iMsgPara7  = portIndex6
					iMsgPara8  = alm type
					iMsgPara9  = rise/clean
					msgCellLen = 0
*/
#define GPN_ALM_MSG_EVT_NOT_SUB2NM			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_DATA_SYN)|(GPN_ALM_MST_STATUS_NOTIFY)|(GPN_ALM_SST_EVT_NOT_SUB2NM)) /*sub solt notify self's event status to master NM*/
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_NOT_SUB2NM
					iSrcId	= SUB-ALM
					iDstId	= MAS-ALM
					iMsgPara1  = location index num(1=<i<=6)
					iMsgPara2  = portIndex1
					iMsgPara3  = portIndex2
					iMsgPara4  = portIndex3
					iMsgPara5  = portIndex4
					iMsgPara6  = portIndex5
					iMsgPara7  = portIndex6
					iMsgPara8  = alm type
					iMsgPara9  = detail
					msgCellLen = 0
*/
#define GPN_ALM_MSG_EVT_NOT_MAS2SLA			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_DATA_SYN)|(GPN_ALM_MST_STATUS_NOTIFY)|(GPN_ALM_SST_EVT_NOT_MAS2SLA))   /*master NM notify self's event status to slave NM*/
/* msg parameters direction: 
					iIndex	   = tx_Index++
					iMsgType   = GPN_ALM_MSG_EVT_NOT_MAS2SLA
					iSrcId	= SUB-ALM
					iDstId	= MAS-ALM
					iMsgPara1  = location index num(1=<i<=6)
					iMsgPara2  = portIndex1
					iMsgPara3  = portIndex2
					iMsgPara4  = portIndex3
					iMsgPara5  = portIndex4
					iMsgPara6  = portIndex5
					iMsgPara7  = portIndex6
					iMsgPara8  = alm type
					iMsgPara9  = detail
					msgCellLen = 0
*/

/*================================================================*/
/*define main type  (from 1 to 0x7E)  : 4                                                                                                  */
/***************************************************************************************/
/*this main type include alarm module how to get alarm and event status(include poll and auto report proc)    */
#define GPN_ALM_MT_ALM_GLO_OPT				((4 << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 1                                                                        */
/****************************************************************************************/
#define GPN_ALM_MST_DB_GLO_OPT				((1 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                              */
/****************************************************************************************/
#define GPN_ALM_SST_CURR_DB_SIZE_GET		((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_CURR_DB_CYC_CFG			((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_CURR_DB_CYC_GET			((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_HIST_DB_SIZE_GET		((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_HIST_DB_CYC_CFG			((5 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_HIST_DB_CYC_GET			((6 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_EVT_DB_SIZE_GET			((7 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_EVT_DB_CYC_CFG			((8 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_EVT_DB_CYC_GET			((9 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
//#define GPN_ALM_SST_ATRRIB_DB_SIZE_GET		((10 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
//#define GPN_ALM_SST_MON_DB_SIZE_GET			((11 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)


//------------------------------leo modify------------------------------//

#define GPN_ALM_SST_PORT_MONI_DB_GET		((12 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_PORT_MONI_DB_GET_NEXT	((13 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_PORT_MONI_DB_MODIFY		((14 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_PORT_MONI_DB_ADD		((15 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_PORT_MONI_DB_DELETE		((16 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)


#define GPN_ALM_SST_ATTRIB_DB_GET			((17 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ATTRIB_DB_GET_NEXT		((18 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ATTRIB_DB_MODIFY		((19 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ATTRIB_DB_ADD			((20 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ATTRIB_DB_DELETE		((21 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)


#define GPN_ALM_SST_CURR_ALM_DB_GET			((22 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_CURR_ALM_DB_GET_NEXT	((23 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_CURR_ALM_DB_MODIFY		((24 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_CURR_ALM_DB_ADD			((25 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_CURR_ALM_DB_DELETE		((26 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)


#define GPN_ALM_SST_HIST_ALM_DB_GET			((27 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_HIST_ALM_DB_GET_NEXT	((28 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_HIST_ALM_DB_MODIFY		((29 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_HIST_ALM_DB_ADD			((30 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_HIST_ALM_DB_DELETE		((31 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)


#define GPN_ALM_SST_EVT_DATA_DB_GET			((32 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_EVT_DATA_DB_GET_NEXT	((33 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_EVT_DATA_DB_ADD			((34 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_EVT_DATA_DB_DELETE		((35 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)


#define GPN_ALM_SST_DEBUG_CURR_ALM_DB_GET			((36 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_DEBUG_CURR_ALM_DB_GET_NEXT		((37 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_DEBUG_CURR_ALM_DB_MODIFY		((38 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_DEBUG_CURR_ALM_DB_ADD			((39 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_DEBUG_CURR_ALM_DB_DELETE		((40 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)


#define GPN_ALM_SST_DEBUG_HIST_ALM_DB_GET			((41 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_DEBUG_HIST_ALM_DB_GET_NEXT		((42 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_DEBUG_HIST_ALM_DB_MODIFY		((43 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_DEBUG_HIST_ALM_DB_ADD			((44 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_DEBUG_HIST_ALM_DB_DELETE		((45 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)


#define GPN_ALM_SST_DEBUG_EVT_DATA_DB_GET			((46 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_DEBUG_EVT_DATA_DB_GET_NEXT		((47 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_DEBUG_EVT_DATA_DB_ADD			((48 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_DEBUG_EVT_DATA_DB_DELETE		((49 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)


//------------------------------  end  ------------------------------//


/****************************************************************************************/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 2                                                                         */
/****************************************************************************************/
#define GPN_ALM_MST_GLO_CTRL_OPT			((2 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
#define GPN_ALM_SST_GLO_RISE_DELAY_CFG		((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_GLO_RISE_DELAY_GET		((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_GLO_CLEAN_DELAY_CFG		((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_GLO_CLEAN_DELAY_GET		((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_GLO_RSET_EN_GET			((5 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_GLO_RSET_EN_CFG			((6 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_GLO_REVER_EN_GET		((7 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_GLO_REVER_EN_CFG		((8 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_GLO_RING_RANK_GET		((9 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_GLO_RING_RANK_CFG		((10 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_GLO_RING_EN_GET			((11 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_GLO_RING_EN_CFG			((12 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_GLO_RING_SURE			((13 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/****************************************************************************************/

#define GPN_ALM_MSG_CURR_DB_SIZE_GET		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_CURR_DB_SIZE_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType  	= GPN_ALM_MSG_CURR_DB_SIZE_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen = 0
*/
#define GPN_ALM_MSG_CURR_DB_SIZE_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_CURR_DB_SIZE_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_DB_SIZE_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/

#define GPN_ALM_MSG_CURR_DB_SIZE_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_CURR_DB_SIZE_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_DB_SIZE_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = DB lines num	
					msgCellLen = 0
*/
#define GPN_ALM_MSG_CURR_DB_SIZE_GET_RSP_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_CURR_DB_SIZE_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_DB_SIZE_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define  GPN_ALM_MSG_CURR_DB_CYC_CFG		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_CURR_DB_CYC_CFG))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType  	= GPN_ALM_MSG_CURR_DB_CYC_CFG
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1  = 1/2(circle/no-circle)	
					msgCellLen= 0
*/
#define GPN_ALM_MSG_CURR_DB_CYC_CFG_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_CURR_DB_CYC_CFG))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_DB_CYC_CFG_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/

#define  GPN_ALM_MSG_CURR_DB_CYC_GET		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_CURR_DB_CYC_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_DB_CYC_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen= 0
*/
#define GPN_ALM_MSG_CURR_DB_CYC_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_CURR_DB_CYC_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_DB_CYC_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/

#define  GPN_ALM_MSG_CURR_DB_CYC_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_CURR_DB_CYC_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_DB_CYC_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = 1/2(circle/no-circle)	
					msgCellLen= 0
*/
#define GPN_ALM_MSG_CURR_DB_CYC_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_CURR_DB_CYC_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_DB_CYC_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_HIST_DB_SIZE_GET		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_HIST_DB_SIZE_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType  	= GPN_ALM_MSG_HIST_DB_SIZE_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen = 0
*/
#define GPN_ALM_MSG_HIST_DB_SIZE_GET_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_HIST_DB_SIZE_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_DB_SIZE_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/

#define GPN_ALM_MSG_HIST_DB_SIZE_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_HIST_DB_SIZE_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_DB_SIZE_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = DB lines num	
					msgCellLen = 0
*/
#define GPN_ALM_MSG_HIST_DB_SIZE_GET_RSP_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_HIST_DB_SIZE_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_DB_SIZE_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define  GPN_ALM_MSG_HIST_DB_CYC_CFG		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_HIST_DB_CYC_CFG))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType  	= GPN_ALM_MSG_HIST_DB_CYC_CFG
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1  = 1/2(circle/no-circle)	
					msgCellLen= 0
*/
#define GPN_ALM_MSG_HIST_DB_CYC_CFG_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_HIST_DB_CYC_CFG))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_DB_CYC_CFG_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/

#define  GPN_ALM_MSG_HIST_DB_CYC_GET		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_HIST_DB_CYC_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_DB_CYC_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen= 0
*/
#define GPN_ALM_MSG_HIST_DB_CYC_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_HIST_DB_CYC_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_DB_CYC_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/

#define  GPN_ALM_MSG_HIST_DB_CYC_GET_RSP	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_HIST_DB_CYC_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_DB_CYC_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = 1/2(circle/no-circle)	
					msgCellLen= 0
*/
#define GPN_ALM_MSG_HIST_DB_CYC_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_HIST_DB_CYC_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_DB_CYC_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_EVT_DB_SIZE_GET			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_EVT_DB_SIZE_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DB_SIZE_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen = 0
*/
#define GPN_ALM_MSG_EVT_DB_SIZE_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_DB_SIZE_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DB_SIZE_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
	
#define GPN_ALM_MSG_EVT_DB_SIZE_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_EVT_DB_SIZE_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DB_SIZE_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = DB lines num	
					msgCellLen = 0
*/
#define GPN_ALM_MSG_EVT_DB_SIZE_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_DB_SIZE_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DB_SIZE_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/
	
#define GPN_ALM_MSG_EVT_DB_CYC_CFG			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_EVT_DB_CYC_CFG))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DB_CYC_CFG
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1  = 1/2(circle/no-circle)	
					msgCellLen= 0
*/
#define GPN_ALM_MSG_EVT_DB_CYC_CFG_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_DB_CYC_CFG))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DB_CYC_CFG_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
	
#define GPN_ALM_MSG_EVT_DB_CYC_GET			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_EVT_DB_CYC_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DB_CYC_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen= 0
*/
#define GPN_ALM_MSG_EVT_DB_CYC_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_DB_CYC_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DB_CYC_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
	
#define GPN_ALM_MSG_EVT_DB_CYC_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_EVT_DB_CYC_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DB_CYC_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = 1/2(circle/no-circle)	
					msgCellLen= 0
*/
#define GPN_ALM_MSG_EVT_DB_CYC_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_DB_CYC_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DB_CYC_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

//------------------------------leo modify------------------------------//

//Table alarmPortMonitorCfg

#define GPN_ALM_MSG_PORT_MONI_DB_GET			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_PORT_MONI_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = monPortIndex1
					iMsgPara2 = monPortIndex2
					iMsgPara3 = monPortIndex3
					iMsgPara4 = monPortIndex4
					iMsgPara5 = monPortIndex5
					iMsgPara6 = monDevIndex
					msgCellLen= 0
*/
#define GPN_ALM_MSG_PORT_MONI_DB_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_PORT_MONI_DB_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_PORT_MONI_DB_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_PORT_MONI_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = monPortIndex1
					iMsgPara2  = monPortIndex2
					iMsgPara3  = monPortIndex3
					iMsgPara4  = monPortIndex4
					iMsgPara5  = monPortIndex5
					iMsgPara6  = monDevIndex
					iMsgPara7  = alarmMonitorEn
					msgCellLen= 0
*/
#define GPN_ALM_MSG_PORT_MONI_DB_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_PORT_MONI_DB_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_PORT_MONI_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1  = monPortIndex1	
					iMsgPara2  = monPortIndex2
					iMsgPara3  = monPortIndex3
					iMsgPara4  = monPortIndex4
					iMsgPara5  = monPortIndex5
					iMsgPara6  = monDevIndex
					msgCellLen= 0
*/
#define GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = next_monPortIndex1
					iMsgPara2  = next_monPortIndex2
					iMsgPara3  = next_monPortIndex3
					iMsgPara4  = next_monPortIndex4
					iMsgPara5  = next_monPortIndex5
					iMsgPara6  = next_monDevIndex
					iMsgPara7  = alarmMonitorEn
					msgCellLen= 0
*/
#define GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_PORT_MONI_DB_MODIFY			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_PORT_MONI_DB_MODIFY))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_MODIFY
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1  = monPortIndex1	
					iMsgPara2  = monPortIndex2
					iMsgPara3  = monPortIndex3
					iMsgPara4  = monPortIndex4
					iMsgPara5  = monPortIndex5
					iMsgPara6  = monDevIndex
					iMsgPara7  = alarmMonitorEn
					msgCellLen= 0
*/
#define GPN_ALM_MSG_PORT_MONI_DB_MODIFY_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_PORT_MONI_DB_MODIFY))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_MODIFY_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/

#define GPN_ALM_MSG_PORT_MONI_DB_ADD			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_PORT_MONI_DB_ADD))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_ADD
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = monPortIndex1	
					iMsgPara2  = monPortIndex2
					iMsgPara3  = monPortIndex3
					iMsgPara4  = monPortIndex4
					iMsgPara5  = monPortIndex5
					iMsgPara6  = monDevIndex
					iMsgPara7  = alarmMonitorEn
					msgCellLen= 0
*/
#define GPN_ALM_MSG_PORT_MONI_DB_ADD_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_PORT_MONI_DB_ADD))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_ADD_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/

#define GPN_ALM_MSG_PORT_MONI_DB_DELETE			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_PORT_MONI_DB_DELETE))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = monPortIndex1
					msgCellLen= 0
*/
#define GPN_ALM_MSG_PORT_MONI_DB_DELETE_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_PORT_MONI_DB_DELETE))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/



//Table alarmAttributeCfg

#define GPN_ALM_MSG_ATTRIB_DB_GET			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_ATTRIB_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1  = alarmType	
					iMsgPara2  = attribPortIndex1
					iMsgPara3  = attribPortIndex2
					iMsgPara4  = attribPortIndex3
					iMsgPara5  = attribPortIndex4
					iMsgPara6  = attribPortIndex5
					iMsgPara7  = attribDevIndex
					msgCellLen= 0
*/
#define GPN_ALM_MSG_ATTRIB_DB_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_ATTRIB_DB_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_ATTRIB_DB_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_ATTRIB_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = alarmType	
					iMsgPara2  = attribPortIndex1
					iMsgPara3  = attribPortIndex2
					iMsgPara4  = attribPortIndex3
					iMsgPara5  = attribPortIndex4
					iMsgPara6  = attribPortIndex5
					iMsgPara7  = attribDevIndex
					iMsgPara8  = basePortMark
					iMsgPara9  = alarmLevel
					iMsgParaA  = alarmMask
					iMsgParaB  = alarmFilter
					iMsgParaC  = alarmReport
					iMsgParaD  = alarmRecord
					iMsgParaE  = portAlmReverse
					msgCellLen= 0
*/
#define GPN_ALM_MSG_ATTRIB_DB_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_ATTRIB_DB_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_ATTRIB_DB_GET_NEXT	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_ATTRIB_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1  = alarmType	
					iMsgPara2  = attribPortIndex1
					iMsgPara3  = attribPortIndex2
					iMsgPara4  = attribPortIndex3
					iMsgPara5  = attribPortIndex4
					iMsgPara6  = attribPortIndex5
					iMsgPara7  = attribDevIndex
					msgCellLen= 0
*/
#define GPN_ALM_MSG_ATTRIB_DB_GET_NEXT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_ATTRIB_DB_GET_NEXT))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET_NEXT_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_ATTRIB_DB_GET_NEXT_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_ATTRIB_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = nextAlarmType	
					iMsgPara2  = nextAttribPortIndex1
					iMsgPara3  = nextAttribPortIndex2
					iMsgPara4  = nextAttribPortIndex3
					iMsgPara5  = nextAttribPortIndex4
					iMsgPara6  = nextAttribPortIndex5
					iMsgPara7  = nextAttribDevIndex
					iMsgPara8  = basePortMark
					iMsgPara9  = alarmLevel
					iMsgParaA  = alarmMask
					iMsgParaB  = alarmFilter
					iMsgParaC  = alarmReport
					iMsgParaD  = alarmRecord
					iMsgParaE  = portAlmReverse
					msgCellLen = 0
*/
#define GPN_ALM_MSG_ATTRIB_DB_GET_NEXT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_ATTRIB_DB_GET_NEXT_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET_NEXT_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_ATTRIB_DB_MODIFY			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_ATTRIB_DB_MODIFY))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_MODIFY
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = ifIndex
					iMsgPara2 = alarmType
					iMsgPara3 = data
					msgCellLen= 0
*/

#define GPN_ALM_MSG_ATTRIB_DB_MODIFY_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_ATTRIB_DB_MODIFY))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_ATTRIB_DB_MODIFY_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/


#define GPN_ALM_MSG_ATTRIB_DB_ADD			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_ATTRIB_DB_ADD))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_ADD
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = monPortIndex1	
					iMsgPara2  = monPortIndex2
					iMsgPara3  = monPortIndex3
					iMsgPara4  = monPortIndex4
					iMsgPara5  = monPortIndex5
					iMsgPara6  = monDevIndex
					iMsgPara7  = alarmMonitorEn
					msgCellLen= 0
*/
#define GPN_ALM_MSG_ATTRIB_DB_ADD_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_ATTRIB_DB_ADD))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_ADD_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/

#define GPN_ALM_MSG_ATTRIB_DB_DELETE			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_ATTRIB_DB_DELETE))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = monPortIndex1
					msgCellLen= 0
*/
#define GPN_ALM_MSG_ATTRIB_DB_DELETE_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_ATTRIB_DB_DELETE))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/



//Table alarmCurrDataSheet

#define GPN_ALM_MSG_CURR_ALM_DB_GET			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_CURR_ALM_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_GET
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = alarmCurrDSIndex
					msgCellLen= 0
*/
#define GPN_ALM_MSG_CURR_ALM_DB_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_CURR_ALM_DB_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_CURR_ALM_DB_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_CURR_ALM_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = alarmCurrDSIndex	
					iMsgPara2  = currAlarmType
					iMsgPara3  = currPortIndex1
					iMsgPara4  = currPortIndex2
					iMsgPara5  = currPortIndex3
					iMsgPara6  = currPortIndex4
					iMsgPara7  = currPortIndex5
					iMsgPara8  = currAlarmLevel
					iMsgPara9  = alarmCurrProductCnt
					iMsgParaA  = alarmCurrFirstProductTime
					iMsgParaB  = alarmCurrThisProductTime
					iMsgParaC  = len_alarmCurrSuffix
					msgCellLen = len_alarmCurrSuffix
					
					alarmCurrSuffix[0]
					...
					alarmCurrSuffix[len_alarmCurrSuffix-1]
*/
#define GPN_ALM_MSG_CURR_ALM_DB_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_CURR_ALM_DB_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_CURR_ALM_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_GET
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = alarmCurrDSIndex
					msgCellLen= 0
*/
#define GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = nextAlarmCurrDSIndex	
					iMsgPara2  = nextCurrAlarmType
					iMsgPara3  = nextCurrPortIndex1
					iMsgPara4  = nextCurrPortIndex2
					iMsgPara5  = nextCurrPortIndex3
					iMsgPara6  = nextCurrPortIndex4
					iMsgPara7  = nextCurrPortIndex5
					iMsgPara8  = nextCurrAlarmLevel
					iMsgPara9  = nextAlarmCurrProductCnt
					iMsgParaA  = nextAlarmCurrFirstProductTime
					iMsgParaB  = nextAlarmCurrThisProductTime
					iMsgParaC  = nextLen_alarmCurrSuffix
					msgCellLen =nextLen_alarmCurrSuffix
					
					nextAlarmCurrSuffix[0]
					...
					nextAlarmCurrSuffix[len_alarmCurrSuffix-1]
*/
#define GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_CURR_ALM_DB_MODIFY			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_CURR_ALM_DB_MODIFY))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_MODIFY
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = alarmCurrDSIndex 
					msgCellLen= 0
*/

#define GPN_ALM_MSG_CURR_ALM_DB_MODIFY_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_CURR_ALM_DB_MODIFY))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_CURR_ALM_DB_MODIFY_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/


#define GPN_ALM_MSG_CURR_ALM_DB_ADD			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_CURR_ALM_DB_ADD))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_ADD
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = monPortIndex1	
					iMsgPara2  = monPortIndex2
					iMsgPara3  = monPortIndex3
					iMsgPara4  = monPortIndex4
					iMsgPara5  = monPortIndex5
					iMsgPara6  = monDevIndex
					iMsgPara7  = alarmMonitorEn
					msgCellLen= 0
*/
#define GPN_ALM_MSG_CURR_ALM_DB_ADD_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_CURR_ALM_DB_ADD))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_ADD_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/

#define GPN_ALM_MSG_CURR_ALM_DB_DELETE			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_CURR_ALM_DB_DELETE))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = monPortIndex1
					msgCellLen= 0
*/
#define GPN_ALM_MSG_CURR_ALM_DB_DELETE_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_CURR_ALM_DB_DELETE))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/



//AlarmHistDataSheet

#define GPN_ALM_MSG_HIST_ALM_DB_GET			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_HIST_ALM_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_GET
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = alarmHistDSIndex
					msgCellLen= 0
*/
#define GPN_ALM_MSG_HIST_ALM_DB_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_HIST_ALM_DB_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_HIST_ALM_DB_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_HIST_ALM_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = alarmHistDSIndex	
					iMsgPara2  = histAlarmType
					iMsgPara3  = histPortIndex1
					iMsgPara4  = histPortIndex2
					iMsgPara5  = histPortIndex3
					iMsgPara6  = histPortIndex4
					iMsgPara7  = histPortIndex5
					iMsgPara8  = histAlarmLevel
					iMsgPara9  = alarmHistProductCnt
					iMsgParaA  = alarmHistFirstProductTime
					iMsgParaB  = alarmHistThisProductTime
					iMsgParaC  = head_alarmHistSuffix
					iMsgParaD  = len_alarmHistSuffix
					msgCellLen= 0
*/
#define GPN_ALM_MSG_HIST_ALM_DB_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_HIST_ALM_DB_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_HIST_ALM_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_GET
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = alarmHistDSIndex
					msgCellLen= 0
*/
#define GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = nextAlarmHistDSIndex	
					iMsgPara2  = nextHistAlarmType
					iMsgPara3  = nextHistPortIndex1
					iMsgPara4  = nextHistPortIndex2
					iMsgPara5  = nextHistPortIndex3
					iMsgPara6  = nextHistPortIndex4
					iMsgPara7  = nextHistPortIndex5
					iMsgPara8  = nextHistAlarmLevel
					iMsgPara9  = nextAlarmHistProductCnt
					iMsgParaA  = nextAlarmHistFirstProductTime
					iMsgParaB  = nextAlarmHistThisProductTime
					iMsgParaC  = nextHead_alarmHistSuffix
					iMsgParaD  = nextLen_alarmHistSuffix
					msgCellLen= 0
*/
#define GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_HIST_ALM_DB_MODIFY			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_HIST_ALM_DB_MODIFY))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_MODIFY
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = alarmHistDSIndex 
					iMsgPara2 = data
					msgCellLen= 0
*/

#define GPN_ALM_MSG_HIST_ALM_DB_MODIFY_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_HIST_ALM_DB_MODIFY))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_HIST_ALM_DB_MODIFY_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/


#define GPN_ALM_MSG_HIST_ALM_DB_ADD			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_HIST_ALM_DB_ADD))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_ADD
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = monPortIndex1	
					iMsgPara2  = monPortIndex2
					iMsgPara3  = monPortIndex3
					iMsgPara4  = monPortIndex4
					iMsgPara5  = monPortIndex5
					iMsgPara6  = monDevIndex
					iMsgPara7  = alarmMonitorEn
					msgCellLen= 0
*/
#define GPN_ALM_MSG_HIST_ALM_DB_ADD_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_HIST_ALM_DB_ADD))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_ADD_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/

#define GPN_ALM_MSG_HIST_ALM_DB_DELETE			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_HIST_ALM_DB_DELETE))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = monPortIndex1
					msgCellLen= 0
*/
#define GPN_ALM_MSG_HIST_ALM_DB_DELETE_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_HIST_ALM_DB_DELETE))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/



//eventDataSheet

#define GPN_ALM_MSG_EVT_DATA_DB_GET			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_EVT_DATA_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DATA_DB_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen= 0
*/
#define GPN_ALM_MSG_EVT_DATA_DB_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_DATA_DB_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DATA_DB_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_EVT_DATA_DB_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_EVT_DATA_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DATA_DB_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = eventIndex	
					iMsgPara2  = eventType
					iMsgPara3  = eventPortIndex1
					iMsgPara4  = eventPortIndex2
					iMsgPara5  = eventPortIndex3
					iMsgPara6  = eventPortIndex4
					iMsgPara7  = eventPortIndex5
					iMsgPara8  = eventLevel
					iMsgPara9  = eventDetail
					iMsgParaA  = eventProductTime
					iMsgParaB  = len_eventSuffix
					msgCellLen= len_eventSuffix
*/
#define GPN_ALM_MSG_EVT_DATA_DB_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_DATA_DB_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DATA_DB_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_EVT_DATA_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen= 0
*/
#define GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = nextEventIndex	
					iMsgPara2  = nextEventType
					iMsgPara3  = nextEventPortIndex1
					iMsgPara4  = nextEeventPortIndex2
					iMsgPara5  = nextEventPortIndex3
					iMsgPara6  = nextEventPortIndex4
					iMsgPara7  = nextEventPortIndex5
					iMsgPara8  = nextEventLevel
					iMsgPara9  = nextEventDetail
					iMsgParaA  = nextEventProductTime
					iMsgParaB  = nextLen_eventSuffix
					msgCellLen= len_eventSuffix
*/
#define GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_EVT_DATA_DB_ADD			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_EVT_DATA_DB_ADD))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_ADD
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = monPortIndex1	
					iMsgPara2  = monPortIndex2
					iMsgPara3  = monPortIndex3
					iMsgPara4  = monPortIndex4
					iMsgPara5  = monPortIndex5
					iMsgPara6  = monDevIndex
					iMsgPara7  = alarmMonitorEn
					msgCellLen= 0
*/
#define GPN_ALM_MSG_EVT_DATA_DB_ADD_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_EVT_DATA_DB_ADD))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_ADD_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/

#define GPN_ALM_MSG_HIST_ALM_DB_DELETE			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_HIST_ALM_DB_DELETE))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = monPortIndex1
					msgCellLen= 0
*/
#define GPN_ALM_MSG_HIST_ALM_DB_DELETE_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_HIST_ALM_DB_DELETE))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/


//debugAlarmCurrDataSheet

#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_CURR_ALM_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = database start addr	
					iMsgPara2  = database real lines	
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_CURR_ALM_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = database start addr	
					iMsgPara2  = database real lines	
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_GET_NEXT_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_MODIFY			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_CURR_ALM_DB_MODIFY))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_MODIFY
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = debugAlarmCurrDSIndex 
					iMsgPara2 = data
					msgCellLen= 0
*/

#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_MODIFY_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_CURR_ALM_DB_MODIFY))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_MODIFY_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/


#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_ADD			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_CURR_ALM_DB_ADD))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_ADD
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = monPortIndex1	
					iMsgPara2  = monPortIndex2
					iMsgPara3  = monPortIndex3
					iMsgPara4  = monPortIndex4
					iMsgPara5  = monPortIndex5
					iMsgPara6  = monDevIndex
					iMsgPara7  = alarmMonitorEn
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_ADD_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_CURR_ALM_DB_ADD))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_ADD_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/

#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_DELETE			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_CURR_ALM_DB_DELETE))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = monPortIndex1
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_CURR_ALM_DB_DELETE_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_CURR_ALM_DB_DELETE))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/


//debugAlarmHistDataSheet

#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_HIST_ALM_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = database start addr	
					iMsgPara2  = database real lines	
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_HIST_ALM_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = database start addr	
					iMsgPara2  = database real lines	
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_HIST_ALM_DB_GET_NEXT_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_MODIFY			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_HIST_ALM_DB_MODIFY))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_HIST_ALM_DB_MODIFY
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = debugAlarmHistDSIndex 
					iMsgPara2 = data
					msgCellLen= 0
*/

#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_MODIFY_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_HIST_ALM_DB_MODIFY))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_DEBUG_CURR_ALM_DB_MODIFY_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/


#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_ADD			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_HIST_ALM_DB_ADD))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_ADD
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = monPortIndex1	
					iMsgPara2  = monPortIndex2
					iMsgPara3  = monPortIndex3
					iMsgPara4  = monPortIndex4
					iMsgPara5  = monPortIndex5
					iMsgPara6  = monDevIndex
					iMsgPara7  = alarmMonitorEn
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_ADD_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_HIST_ALM_DB_ADD))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_ADD_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/

#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_DELETE			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_HIST_ALM_DB_DELETE))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = monPortIndex1
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_HIST_ALM_DB_DELETE_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_HIST_ALM_DB_DELETE))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/




//debugEventDataSheet

#define GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_EVT_DATA_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = database start addr	
					iMsgPara2  = database real lines	
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_EVT_DATA_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
		
#define GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = database start addr	
					iMsgPara2  = database real lines	
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT_RSP))
/* msg parameters direction: 
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_DEBUG_EVT_DATA_DB_GET_NEXT_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_DEBUG_EVT_DATA_DB_ADD			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_EVT_DATA_DB_ADD))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ATTRIB_DB_ADD
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = monPortIndex1	
					iMsgPara2  = monPortIndex2
					iMsgPara3  = monPortIndex3
					iMsgPara4  = monPortIndex4
					iMsgPara5  = monPortIndex5
					iMsgPara6  = monDevIndex
					iMsgPara7  = alarmMonitorEn
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_EVT_DATA_DB_ADD_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_EVT_DATA_DB_ADD))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_ADD_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/

#define GPN_ALM_MSG_DEBUG_EVT_DATA_DB_DELETE			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_DB_GLO_OPT)|(GPN_ALM_SST_DEBUG_EVT_DATA_DB_DELETE))
/* msg parameters direction:  
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1 = monPortIndex1
					msgCellLen= 0
*/
#define GPN_ALM_MSG_DEBUG_EVT_DATA_DB_DELETE_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_ALM_MSG_DEBUG_EVT_DATA_DB_DELETE))
/* msg parameters direction:

						iIndex	= tx_Index++
						iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_DELETE_RSP
						iSrcId	= other modu
						iDstId	= ALM
*/


//----------------------------------end----------------------------------//


#define GPN_ALM_MSG_GLO_RISE_DELAY_CFG  	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RISE_DELAY_CFG))
/* msg parameters direction:    
					iIndex	= tx_Index++
					iMsgType  	= GPN_ALM_MSG_GLO_RISE_DELAY_CFG
				   	iSrcId	= other modu
					iDstId	= ALM
				   	iMsgPara1 	= rise delay time(s, 0<=N<=20)
				   	msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_RISE_DELAY_CFG_ACK  ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RISE_DELAY_CFG))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RISE_DELAY_CFG_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_RISE_DELAY_GET  	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RISE_DELAY_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RISE_DELAY_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_RISE_DELAY_GET_ACK  ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RISE_DELAY_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RISE_DELAY_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_RISE_DELAY_GET_RSP  ((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RISE_DELAY_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RISE_DELAY_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1	= rise delay time(s, 0<=N<=20)
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_RISE_DELAY_GET_RSP_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RISE_DELAY_GET_RSP))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RISE_DELAY_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_GLO_CLEAN_DELAY_CFG  	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_CLEAN_DELAY_CFG))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_CLEAN_DELAY_CFG
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1	= clean delay time(s, 0<=N<=20)
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_CLEAN_DELAY_CFG_ACK  ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_CLEAN_DELAY_CFG))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_CLEAN_DELAY_CFG_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_CLEAN_DELAY_GET  	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_CLEAN_DELAY_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_CLEAN_DELAY_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_CLEAN_DELAY_GET_ACK  ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_CLEAN_DELAY_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_CLEAN_DELAY_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_CLEAN_DELAY_GET_RSP  ((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_CLEAN_DELAY_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_CLEAN_DELAY_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1	= clean delay time(s, 0<=N<=20)
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_CLEAN_DELAY_GET_RSP_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_CLEAN_DELAY_GET_RSP))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_CLEAN_DELAY_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_GLO_RSET_EN_CFG  		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RSET_EN_CFG))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RSET_EN_CFG
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1	= 1/2/3(base-level/base-time/disable)
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_RSET_EN_CFG_ACK  	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RSET_EN_CFG))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RSET_EN_CFG_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_RSET_EN_GET  		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RSET_EN_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RSET_EN_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_RSET_EN_GET_ACK  	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RSET_EN_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RSET_EN_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_RSET_EN_GET_RSP  	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RSET_EN_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RSET_EN_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1	= 1/2/3(base-level/base-time/disable)
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_RSET_EN_GET_RSP_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RSET_EN_GET_RSP))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RSET_EN_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_GLO_REVER_EN_CFG  		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_REVER_EN_CFG))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_REVER_EN_CFG
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1  = 1/2/3(auto/manual/disable)	
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_REVER_EN_CFG_ACK  	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_REVER_EN_CFG))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_REVER_EN_CFG_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_REVER_EN_GET  		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_REVER_EN_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_REVER_EN_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_REVER_EN_GET_ACK  	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_REVER_EN_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_REVER_EN_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_REVER_EN_GET_RSP  	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_REVER_EN_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_REVER_EN_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = 1/2/3(auto/manual/disable)	
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_REVER_EN_GET_RSP_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_REVER_EN_GET_RSP))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_REVER_EN_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_GLO_RING_RANK_CFG  		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RING_RANK_CFG))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_REVER_EN_CFG
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1  = alm rank	
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_RING_RANK_CFG_ACK  	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RING_RANK_CFG))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RING_RANK_CFG_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_RING_RANK_GET  		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RING_RANK_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RING_RANK_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_RING_RANK_GET_ACK  	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RING_RANK_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RING_RANK_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_RING_RANK_GET_RSP  	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RING_RANK_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RING_RANK_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = alm rank	
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_RING_RANK_GET_RSP_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RING_RANK_GET_RSP))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RING_RANK_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_GLO_RING_EN_CFG  		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RING_EN_CFG))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RING_EN_CFG
					iSrcId	= other modu
					iDstId	= ALM
					iMsgPara1  = enable/disable	
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_RING_EN_CFG_ACK  	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RING_EN_CFG))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RING_EN_CFG_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_RING_EN_GET  		((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RING_EN_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RING_EN_GET
					iSrcId	= other modu
					iDstId	= ALM
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_RING_EN_GET_ACK  	((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RING_EN_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RING_EN_GET_ACK
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_RING_EN_GET_RSP  	((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RING_EN_GET))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RING_EN_GET_RSP
					iSrcId	= ALM
					iDstId	= other modu
					iMsgPara1  = enable/disable	
					msgCellLen = 0
*/
#define GPN_ALM_MSG_GLO_RING_EN_GET_RSP_ACK ((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RING_EN_GET_RSP))
/* msg parameters direction:	
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RING_EN_GET_RSP_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/

#define GPN_ALM_MSG_GLO_RING_SURE			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_GLO_OPT)|(GPN_ALM_MST_GLO_CTRL_OPT)|(GPN_ALM_SST_GLO_RING_SURE))
/* msg parameters direction:
					iIndex	= tx_Index++
					iMsgType  	= GPN_ALM_MSG_GLO_RING_SURE
					iSrcId	= ALM
					iDstId	= other modu
*/
#define GPN_ALM_MSG_GLO_RING_SURE_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_GLO_RING_SURE))
/* msg parameters direction:
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_GLO_RING_SURE_ACK
					iSrcId	= other modu
					iDstId	= ALM
*/


/*================================================================*/
/*define main type  (from 1 to 0x7E)  : 5                                                                                                  */
/***************************************************************************************/
/*this main type include gpn_alarm active opt(like switch led/bell, auto poll alarm status ,etc )                       */
#define GPN_ALM_MT_ALM_ACTIVE_OPT			((5 << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 1                                                                        */
/****************************************************************************************/
#define GPN_ALM_MST_ALM_ACTIVE_CFG			((1 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                              */
/****************************************************************************************/
#define GPN_ALM_SST_SWITHC_BELL				((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_ALM_RANK_NOT			((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 2                                                                         */
/****************************************************************************************/
#define GPN_ALM_MST_ALM_ACTIVE_POLL			((2 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
#define GPN_ALM_SST_POLL_ALM_BASE_MODU		((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_ALM_SST_POLL_ALM_BASE_PORT		((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/****************************************************************************************/


#define GPN_ALM_MSG_SWITHC_BELL				((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ACTIVE_OPT)|(GPN_ALM_MST_ALM_ACTIVE_CFG)|(GPN_ALM_SST_SWITHC_BELL))
/* msg parameters direction:
					iIndex	= tx_Index++
					iMsgType  	= GPN_ALM_MSG_SWITHC_BELL
					iSrcId	= ALM
					iDstId	= SDMD
					iMsgPara1  = opt(1: open; 2: close)
					msgCellLen = 0
*/
#define GPN_ALM_MSG_SWITHC_BELL_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_SWITHC_BELL))
/* msg parameters direction:
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_SWITHC_BELL_ACK
					iSrcId	=  SDMD
					iDstId	=  ALM
*/
#define GPN_ALM_MSG_ALM_RANK_NOT			((GPN_SOCK_OWNER_ALM)|(GPN_ALM_MT_ALM_ACTIVE_OPT)|(GPN_ALM_MST_ALM_ACTIVE_CFG)|(GPN_ALM_SST_ALM_RANK_NOT))
/* msg parameters direction:
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ALM_RANK_NOT
					iSrcId	= ALM
					iDstId	= SDMD
					iMsgPara1  = emerg rank(GPN_ALM_RANK_EMERG:4) alarms num(0: no this rank alarm)
					iMsgPara2  = severe rank(GPN_ALM_RANK_SEVERE:3) alarms num(0: no this rank alarm)
					iMsgPara3  = common rank(GPN_ALM_RANK_COMMON:2) alarms num(0: no this rank alarm)
					iMsgPara4  = cur rank(GPN_ALM_RANK_CUR:1) alarms num(0: no this rank alarm)
					msgCellLen = 0
*/
#define GPN_ALM_MSG_ALM_RANK_NOT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_ALM_MSG_ALM_RANK_NOT))
/* msg parameters direction:
					iIndex	= tx_Index++
					iMsgType	= GPN_ALM_MSG_ALM_RANK_NOT_ACK
					iSrcId	=  SDMD
					iDstId	=  ALM
*/


#define GPN_ALM_PROC_RESULT_CREAT_CURR(path) gpnSockAlmMsgTxAlmProcResultViewCreat(path, GPN_ALM_MSG_PARA_RESULT_CURR)
#define GPN_ALM_PROC_RESULT_CREAT_HIST(path) gpnSockAlmMsgTxAlmProcResultViewCreat(path, GPN_ALM_MSG_PARA_RESULT_HIST)
#define GPN_ALM_PROC_RESULT_CREAT_EVENT(path) gpnSockAlmMsgTxAlmProcResultViewCreat(path, GPN_ALM_MSG_PARA_RESULT_EVENT)

UINT32 gpnSockAlmMsgTxMonStaCfg(UINT32 portIndex, UINT32 sta);
UINT32 gpnSockAlmMsgTxAlmNotifyBaseSubType(UINT32 portIndex, UINT32 subAlmType, UINT32 sta);
UINT32 gpnSockAlmMsgTxAlmNotifyByMulIndexBaseSubType(optObjOrient *pPortInfo, UINT32 subAlmType, UINT32 sta);
UINT32 gpnSockAlmMsgTxAlmNotifyBaseScanType(UINT32 portIndex, UINT32 *almVal, UINT32 almPageNum);
UINT32 gpnSockAlmMsgTxCFMAlmNotifyBaseSubType(optObjOrient *pPortInfo, UINT32 subAlmType, UINT32 sta);
UINT32 gpnSockAlmMsgTxEventNotifyBaseSubType(optObjOrient *pPortInfo, UINT32 subEvtType, UINT32 detial);

UINT32 gpnSockAlmMsgTxBellSwitchCfg(UINT32 opt);
UINT32 gpnSockAlmMsgTxAlmRankNumNotify(UINT32 emergNum, UINT32 severeNum, UINT32 commNum, UINT32 curNum);

UINT32 gpnSockAlmMsgTxAlmProcResultViewCreat(char *path, UINT32 type);

/* temp define */
UINT32 gpnSockAlmMsgTxEmbStorOutEvent(UINT32 detial);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_ALM_MSG_DEF_H_*/

