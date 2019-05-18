/**********************************************************
* file name: gpnSockCfgMgtMsgDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-05-30
* function: 
*    define details about communication between gpn_cfgmgt modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_CFGMGT_MSG_DEF_H_
#define _GPN_SOCK_CFGMGT_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnSockMsgDef.h"
#include "socketComm/gpnSelectOpt.h"

/*==========================================================*/
/*==========================================================*/
/*define main type  (from 1 to 0x7E)  : 1                                                                        */
/*************************************************************************/
/*this main type include normal comm msg used by gpn_cfgmgt                                            */
#define GPN_CFGMGT_MT_CFGSYN						GPN_SOCK_MSG_MT_CREAT(1)

/***************************************************************************** **********/
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  :   1                                                                     */
/****************************************************************************************/
#define GPN_CFGMGT_MST_WITH_SYSMGT					GPN_SOCK_MSG_MST_CREAT(1)
/****************************************************************************************/
/*define main type 1's main-sub type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
	#define GPN_CFGMGT_SST_NOTIFY_TYPE_2_SYSMGT			GPN_SOCK_MSG_SST_CREAT(1)
	#define GPN_CFGMGT_SST_NOTIFY_STA_2_SYSMGT			GPN_SOCK_MSG_SST_CREAT(2)
	#define GPN_CFGMGT_SST_ALARM_SYN_2_SYSMGT			GPN_SOCK_MSG_SST_CREAT(3)

/***************************************************************************** **********/
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 2                                                                        */
/****************************************************************************************/
#define GPN_CFGMGT_MST_WITH_TRANS					GPN_SOCK_MSG_MST_CREAT(2)
/****************************************************************************************/
/*define main type 1's main-sub type 2's sub-sub type  (from 1 to 0xFE)  :                                                                              */
/****************************************************************************************/


/****************************************************************************************/
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 3                                                                        */
/****************************************************************************************/
#define GPN_CFGMGT_MST_WITH_SELF					GPN_SOCK_MSG_MST_CREAT(3)
/****************************************************************************************/
/*define main type 1's main-sub type 3's sub-sub type  (from 1 to 0xFE)  :                                                                              */
/****************************************************************************************/
	#define GPN_CFGMGT_SST_SUB_NOTIFY_STA               GPN_SOCK_MSG_SST_CREAT(1)
	#define GPN_CFGMGT_SST_SUB_REGISTER                 GPN_SOCK_MSG_SST_CREAT(2)
	#define GPN_CFGMGT_SST_TAG_FILE_REPLACE	            GPN_SOCK_MSG_SST_CREAT(3)
	#define GPN_CFGMGT_SST_SUB_CFG_RECOVER_OPT          GPN_SOCK_MSG_SST_CREAT(4)
	#define GPN_CFGMGT_SST_REBOOT_SUB_DEV               GPN_SOCK_MSG_SST_CREAT(5)
	#define GPN_CFGMGT_SST_SUB_CFG_SAVE_OPT             GPN_SOCK_MSG_SST_CREAT(6)
	#define GPN_CFGMGT_SST_SUB_NTP_TIME_SYN             GPN_SOCK_MSG_SST_CREAT(7)
	#define GPN_CFGMGT_SST_SUB_RECALCULATE_MD5          GPN_SOCK_MSG_SST_CREAT(8)
	#define GPN_CFGMGT_SST_SUB_INFORM_SYNCED			GPN_SOCK_MSG_SST_CREAT(9)
	
/***************************************************************************** **********/
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)	: 2 																	   */
/****************************************************************************************/
#define GPN_CFGMGT_MST_WITH_OTHER					GPN_SOCK_MSG_MST_CREAT(4)
/****************************************************************************************/
/*define main type 1's main-sub type 2's sub-sub type  (from 1 to 0xFE)  :																				*/
/****************************************************************************************/
	#define GPN_CFGMGT_SST_SUB_PROMAC_2_NSM				GPN_SOCK_MSG_SST_CREAT(1)

#define GPN_CFGMGT_MSG_NOTIFY_TYPE_2_SYSMGT			((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_SYSMGT)|(GPN_CFGMGT_SST_NOTIFY_TYPE_2_SYSMGT))	/**/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_NOTIFY_TYPE_2_SYSMGT
                   	iSrcId     = CFGMGT(sub)
                   	iDstId     = NSM(sub)
                   	iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	msgCellLen = 0
*/

#define GPN_CFGMGT_MSG_NOTIFY_TYPE_2_SYSMGT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_NOTIFY_TYPE_2_SYSMGT))
/* msg parameters direction:
			iIndex     = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_NOTIFY_TYPE_2_SYSMGT_ACK
                   	iSrcId     	= CFGMGT(NMx)
                   	iDstId     	= SYSMGT(NMx)
*/

#define GPN_CFGMGT_MSG_NOTIFY_STAT_2_SYSMGT			((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_SYSMGT)|(GPN_CFGMGT_SST_NOTIFY_STA_2_SYSMGT))	/**/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_NOTIFY_STAT_2_SYSMGT
                   	iSrcId     = CFGMGT(NMx)
                   	iDstId     = SYSMGT(NMx)
                   	iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	iMsgPara3  = status(1: 2: 3:)
                   	msgCellLen = 0
*/

#define GPN_CFGMGT_MSG_NOTIFY_STAT_2_SYSMGT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_SST_NOTIFY_STA_2_SYSMGT))
/* msg parameters direction:
			iIndex     = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_NOTIFY_STAT_2_SYSMGT_ACK
                   	iSrcId     	= CFGMGT(NMx)
                   	iDstId     	= SYSMGT(NMx)
*/

#define GPN_CFGMGT_MSG_ALARM_SYN_2_SYSMGT			((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_SYSMGT)|(GPN_CFGMGT_SST_ALARM_SYN_2_SYSMGT))	/**/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_NOTIFY_TYPE_2_SYSMGT
                   	iSrcId     = CFGMGT(NMx)
                   	iDstId     = SYSMGT(NMx)
                   	iMsgPara1  = 1-cfg not syn, 0-cfg syn
                   	msgCellLen = 0
*/

#define GPN_CFGMGT_MSG_ALARM_SYN_2_SYSMGT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_ALARM_SYN_2_SYSMGT))
/* msg parameters direction:
			iIndex     = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_NOTIFY_TYPE_2_SYSMGT_ACK
                   	iSrcId     	= CFGMGT(NMx)
                   	iDstId     	= SYSMGT(NMx)
*/

#define GPN_CFGMGT_MSG_SUB_NOTIFY_STA				((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_SELF)|(GPN_CFGMGT_SST_SUB_NOTIFY_STA))	/*cfgmgt on sub slot notify cftmgt on NMx slot self's status*/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_SUB_NOTIFY_STA
                   	iSrcId     = CFGMGT(sub)
                   	iDstId     = CFGMGT(NMx)
                   	iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	iMsgPara3  = file type
                   	iMsgPara4  = synced
                   	msgCellLen = 32(MD5 info lenth)
                   	palyload = MD5
*/

#define GPN_CFGMGT_MSG_SUB_NOTIFY_STA_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_SUB_NOTIFY_STA))
/* msg parameters direction:
			iIndex     = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_SUB_NOTIFY_STA_ACK
                   	iSrcId     = CFGMGT(NMx)
                   	iDstId     = CFGMGT(sub)
*/

#define GPN_CFGMGT_MSG_CARD_REGISTER				((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_SELF)|(GPN_CFGMGT_SST_SUB_REGISTER))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_TAG_FILE_REPLACE
					iSrcId	   = CFGMGT(sub)
					iDstId	   = CFGMGT(NMX)
					iMsgPara1  = devIndex(slot)
					msgCellLen = 0

*/
	
#define GPN_CFGMGT_MSG_CARD_REGISTER_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_CARD_REGISTER))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_TAG_FILE_REPLACE_ACK
					iSrcId	   = CFGMGT(NMX)
					iDstId	   = CFGMGT(sub)
*/
#define GPN_CFGMGT_MSG_CARD_REGISTER_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_CFGMGT_MSG_CARD_REGISTER))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_CARD_REGISTER_RSP
			iSrcId	   = CFGMGT(NMX)
			iDstId	   = CFGMGT(sub)
			iMsgPara1  = devIndex(slot)
			msgCellLen = 0

*/

#define GPN_CFGMGT_MSG_TAG_FILE_REPLACE				((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_SELF)|(GPN_CFGMGT_SST_TAG_FILE_REPLACE))	/**/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_TAG_FILE_REPLACE
                   	iSrcId     = CFGMGT(NMx)
                   	iDstId     = CFGMGT(sub)
                   	iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	iMsgPara3  = file type
                   	iMsgPara3  = SUFFIX_ID
                   	msgCellLen = 0

*/

#define GPN_CFGMGT_MSG_TAG_FILE_REPLACE_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_TAG_FILE_REPLACE))
/* msg parameters direction:
			iIndex     = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_TAG_FILE_REPLACE_ACK
                   	iSrcId     = CFGMGT(sub)
                   	iDstId     = CFGMGT(NMx)
*/
#define GPN_CFGMGT_MSG_TAG_FILE_REPLACE_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_CFGMGT_MSG_TAG_FILE_REPLACE))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_TAG_FILE_REPLACE_RSP
			iSrcId	   = CFGMGT(sub)
			iDstId	   = CFGMGT(NMx)
			iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	iMsgPara3  = file type
                   	iMsgPara4  = SUFFIX_ID
                   	iMsgPara5  = opt result(1:OK 2:ERROR)
			msgCellLen = 0

*/
#define GPN_CFGMGT_MSG_TAG_FILE_REPLACE_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_TAG_FILE_REPLACE_RSP))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_TAG_FILE_REPLACE_RSP_ACK
			iSrcId	   = CFGMGT(NMx)
			iDstId	   = CFGMGT(sub)
*/

#define GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT				((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_SELF)|(GPN_CFGMGT_SST_SUB_CFG_RECOVER_OPT))	/**/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT
                   	iSrcId     = CFGMGT(NMx)
                   	iDstId     = CFGMGT(sub)
                   	iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	iMsgPara3  = file type
                   	iMsgPara4  = (CFGMGT_FILE_PRODUCT_TYPE_INVALID: recover product;	CFGMGT_FILE_USER_PRODUCT: recover user ...)
                   	msgCellLen = 0

*/
#define GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT))
/* msg parameters direction:
			iIndex     = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT_ACK
                   	iSrcId     = CFGMGT(sub)
                   	iDstId     = CFGMGT(NMx)
*/

#define GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT_RSP
			iSrcId	   = CFGMGT(sub)
			iDstId	   = CFGMGT(NMx)
			iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	iMsgPara3  = file type
                   	iMsgPara4  = (CFGMGT_FILE_PRODUCT_TYPE_INVALID: recover product;	CFGMGT_FILE_USER_PRODUCT: recover user ...)
                   	iMsgPara5  = OPT_STA(1:OK 2:ERROR)
			msgCellLen = 0

*/
#define GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT_RSP))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_SUB_CFG_RECOVER_OPT_RSP_ACK
			iSrcId	   = CFGMGT(NMx)
			iDstId	   = CFGMGT(sub)
*/


#define GPN_CFGMGT_MSG_SUB_CFG_SAVE_OPT				((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_SELF)|(GPN_CFGMGT_SST_SUB_CFG_SAVE_OPT))	/**/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_SUB_CFG_SAVE_OPT
                   	iSrcId     = CFGMGT(NMx)
                   	iDstId     = CFGMGT(sub)
                   	iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	iMsgPara3  = file type
                   	iMsgPara4  =  (CFGMGT_FILE_TYPE_CONF: save as start;  CFGMGT_FILE_USER_TYPE_CONF: save as user ...)
                   	msgCellLen = 0

*/
#define GPN_CFGMGT_MSG_SUB_CFG_SAVE_OPT_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_SUB_CFG_SAVE_OPT))
/* msg parameters direction:
			iIndex     = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_SUB_CFG_SAVE_OPT_ACK
                   	iSrcId     = CFGMGT(sub)
                   	iDstId     = CFGMGT(NMx)
*/
#define GPN_CFGMGT_MSG_SUB_CFG_SAVE_OPT_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_CFGMGT_MSG_SUB_CFG_SAVE_OPT))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_SUB_CFG_FILE_OPT_RSP
			iSrcId	   = CFGMGT(sub)
			iDstId	   = CFGMGT(NMx)
			iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	iMsgPara3  = file type
                   	iMsgPara4  = (CFGMGT_FILE_TYPE_CONF: save as start;  CFGMGT_FILE_USER_TYPE_CONF: save as user ...)
                   	iMsgPara5  = OPT_STA(1:OK 2:ERROR)
			msgCellLen = 0

*/
#define GPN_CFGMGT_MSG_SUB_CFG_SAVE_OPT_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_SUB_CFG_SAVE_OPT_RSP))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_SUB_CFG_SAVE_OPT_RSP_ACK
			iSrcId	   = CFGMGT(NMx)
			iDstId	   = CFGMGT(sub)
*/

#define GPN_CFGMGT_MSG_REBOOT_SUB_DEV				((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_SELF)|(GPN_CFGMGT_SST_REBOOT_SUB_DEV))	/**/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_REBOOT_SUB_DEV
                   	iSrcId     = CFGMGT(NMx)
                   	iDstId     = CFGMGT(sub)
                   	iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	msgCellLen = 0

*/
#define GPN_CFGMGT_MSG_REBOOT_SUB_DEV_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_REBOOT_SUB_DEV))
/* msg parameters direction:
			iIndex     = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_REBOOT_SUB_DEV_ACK
                   	iSrcId     = CFGMGT(sub)
                   	iDstId     = CFGMGT(NMx)
*/

#define GPN_CFGMGT_MSG_SUB_NTP_TIME_SYN_OPT			((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_SELF)|(GPN_CFGMGT_SST_SUB_NTP_TIME_SYN))	/**/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_SUB_NTP_TIME_SYN_OPT
                   	iSrcId     = CFGMGT(NMx)
                   	iDstId     = CFGMGT(sub)
                   	iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	iMsgPara3  = tv_sec
                   	iMsgPara4  = tv_usec
                   	iMsgPara5  = tz_minuteswest
                   	msgCellLen = 0

*/
#define GPN_CFGMGT_MSG_SUB_NTP_TIME_SYN_OPT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_SUB_NTP_TIME_SYN_OPT))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_SUB_NTP_TIME_SYN_OPT_ACK
			iSrcId	   = CFGMGTsub)
			iDstId	   = CFGMGT(NMx)
*/


#define GPN_CFGMGT_MSG_SUB_RECALCULATE_MD5_OPT			((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_SELF)|(GPN_CFGMGT_SST_SUB_RECALCULATE_MD5))	/**/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_CFGMGT_MSG_SUB_RECALCULATE_MD5_OPT
                   	iSrcId     = CFGMGT(NMx)
                   	iDstId     = CFGMGT(sub)
                   	iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	iMsgPara3  = filetype

                   	msgCellLen = 0

*/
#define GPN_CFGMGT_MSG_SUB_RECALCULATE_MD5_OPT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_SUB_RECALCULATE_MD5_OPT))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_SUB_RECALCULATE_MD5_OPT_ACK
			iSrcId	   = CFGMGTsub)
			iDstId	   = CFGMGT(NMx)
*/


#define GPN_CFGMGT_SST_SUB_INFORM_SYNCED_OPT			((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_SELF)|(GPN_CFGMGT_SST_SUB_INFORM_SYNCED))	/**/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_CFGMGT_SST_SUB_INFORM_SYNCED_OPT
                   	iSrcId     = CFGMGT(NMx)
                   	iDstId     = CFGMGT(sub)
                   	iMsgPara1  = devIndex(slot)
                   	iMsgPara2  = device type
                   	iMsgPara3  = synced

                   	msgCellLen = 0

*/
#define GPN_CFGMGT_SST_SUB_INFORM_SYNCED_OPT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_SST_SUB_INFORM_SYNCED_OPT))
	/* msg parameters direction:
				iIndex	   = CMD_Index
				iMsgType   = GPN_CFGMGT_SST_SUB_INFORM_SYNCED_OPT_ACK
				iSrcId	   = CFGMGTsub)
				iDstId	   = CFGMGT(NMx)
	*/

#define GPN_CFGMGT_MSG_SUB_PROMAC_2_NSM				((GPN_SOCK_OWNER_CFGMGT)|(GPN_CFGMGT_MT_CFGSYN)|(GPN_CFGMGT_MST_WITH_OTHER)|(GPN_CFGMGT_SST_SUB_PROMAC_2_NSM))
/* msg parameters direction: 
			iIndex		= tx_Index++
			iMsgType	= GPN_SYSMGT_MSG_SUB_L2CFG_NOTIFY
			iSrcId		= CFGMGT(slot)
			iDstId		= NSM(slot)
			iMsgPara1	= devIndex(slot)
			iMsgPara2	= device type
			iMsgPara3	= reserve;
			iMsgPara4	= mac[2]
			iMsgPara5	= mac[4]
			msgCellLen	= 0
*/

#define GPN_CFGMGT_MSG_SUB_PROMAC_2_NSM_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_SUB_PROMAC_2_NSM))
/* msg parameters direction:
			iIndex     = CMD_Index
			iMsgType   = GPN_CFGMGT_MSG_NOTIFY_TYPE_2_SYSMGT_ACK
                   	iSrcId     	= NSM(sub)
                   	iDstId     	= CFGMGT(sub)
*/

/*==========================================================*/
/*==========================================================*/
/*define main type  (from 1 to 0x7E)  : 2                                                                        */
/*************************************************************************/
/*this main type include normal comm msg used by gpn_cfgmgt                                            */

#define GPN_CFGMGT_MD5_INFO_BLEN					(32)
#define GPN_CFGMGT_MD5_STA_NOTIFY_MSG_LEN			((GPN_SOCK_MSG_HEAD_BLEN)+(GPN_CFGMGT_MD5_INFO_BLEN))

UINT32 gpnSockCfgMgtMsgTxNotifyTypeToSysMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type);
UINT32 gpnSockCfgMgtMsgTxNotifyStatToSysMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT32 sta);
UINT32 gpnSockCfgMgtMsgTxAlarmSynToSysMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 syn_stat);

UINT32 gpnSockCfgMgtMsgTxTagFileReplace2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 dev_type, UINT32 file_type, UINT32 suffix_id);
UINT32 gpnSockCfgMgtMsgTxCfgRecoverOpt2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 dev_type, UINT32 file_type, UINT32 cmd);
UINT32 gpnSockCfgMgtMsgTxCfgSaveOpt2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 dev_type, UINT32 file_type, UINT32 cmd);
UINT32 gpnSockCfgMgtMsgTxRebootSubDev2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 dev_type);
UINT32 gpnSockCfgMgtMsgTxTagFileReplaceRsp2CFGServer(stSockFdSet *pstFdSet, UINT32 NMxCPUId, UINT32 subSlot, UINT32 dev_type, UINT32 file_type, UINT32 suffix_id, UINT32 result);
UINT32 gpnSockCfgMgtMsgTxCfgRecoverOptRsp2CFGServer(stSockFdSet *pstFdSet, UINT32 NMxCPUId, UINT32 subSlot, UINT32 dev_type, UINT32 file_type, UINT32 cmd, UINT32 result);
UINT32 gpnSockCfgMgtMsgTxCfgSaveOptRsp2CFGServer(stSockFdSet *pstFdSet, UINT32 NMxCPUId, UINT32 subSlot, UINT32 dev_type, UINT32 file_type, UINT32 cmd, UINT32 result);
UINT32 gpnSockCfgMgtMsgTxNotifySta2CFGServer(stSockFdSet *pstFdSet, UINT32 NMxCPUId, UINT32 subSlot, UINT32 dev_type, UINT32 file_type,  UINT32 synced, UINT32 config_done,INT8 *md5);
UINT32 gpnSockCfgMgtMsgTxCardRegisterRsp2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId);
UINT32 gpnSockCfgMgtMsgTxCardRegister2CFGServer(stSockFdSet *pstFdSet,	UINT32 NMxCPUId, UINT32 subSlot);
UINT32 gpnSockCfgMgtMsgTxNTPTimeSynOpt2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 dev_type, UINT32 tv_sec, UINT32 tv_usec, UINT32 zone);
UINT32 gpnSockCfgMgtMsgTxRecalculateMd5Opt2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 dev_type, UINT32 file_type);
UINT32 gpnSockCfgMgtMsgTxInformSyncedOpt2CFGClient(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 dev_type, UINT32 synced);
UINT32 gpnSockCfgMgtMsgTxProMac2NSMModule(stSockFdSet *pstFdSet, UINT32 subSlot, UINT32 dev_type, UINT8 * proMac);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_CFGMGT_MSG_DEF_H_*/


