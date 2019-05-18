/**********************************************************
* file name: gpnSockFTMsgDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-05-30
* function: 
*    define details about communication between gpn_ft modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_FT_MSG_DEF_H_
#define _GPN_SOCK_FT_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnSockMsgDef.h"
#include "socketComm/gpnSelectOpt.h"
#include "socketComm/gpnGlobalPortIndexDef.h"


/*==========================================================*/
/*==========================================================*/
/*define main type  (from 1 to 0x7E)  : 1                                                                        */
/*************************************************************************/
/*this main type include normal comm bettwen gpn_ft and other module                         */
#define GPN_FT_MT_USER_API							GPN_SOCK_MSG_MT_CREAT(1)

/***************************************************************************** **********/
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  :   1                                                                     */
/****************************************************************************************/
#define GPN_CFGMGT_MST_FILE_TRANS_OPT				GPN_SOCK_MSG_MST_CREAT(1)
/****************************************************************************************/
/*define main type 1's main-sub type 1's sub-sub type  (from 1 to 0xFE)  :                                                  */
/****************************************************************************************/
	#define GPN_CFGMGT_SST_FILE_TRANS_REQ_CREAT			GPN_SOCK_MSG_SST_CREAT(1)
	#define GPN_CFGMGT_SST_SPFILE_TRANS_REQ_CREAT		GPN_SOCK_MSG_SST_CREAT(2)
	#define GPN_CFGMGT_SST_FILE_TRANS_REQ_DEL			GPN_SOCK_MSG_SST_CREAT(3)
	#define GPN_CFGMGT_SST_REQ_STA_GET					GPN_SOCK_MSG_SST_CREAT(4)
	#define GPN_CFGMGT_SST_REQ_STA_NOTIFY				GPN_SOCK_MSG_SST_CREAT(5)

	#define GPN_CFGMGT_SST_FILE_TRANS_TASK_CREAT		GPN_SOCK_MSG_SST_CREAT(6)
	#define GPN_CFGMGT_SST_SPFILE_TRANS_TASK_CREAT		GPN_SOCK_MSG_SST_CREAT(7)
	#define GPN_CFGMGT_SST_FILE_TRANS_TASK_DEL			GPN_SOCK_MSG_SST_CREAT(8)
	#define GPN_CFGMGT_SST_TASK_STA_GET					GPN_SOCK_MSG_SST_CREAT(9)
	#define GPN_CFGMGT_SST_TASK_STA_NOTIFY				GPN_SOCK_MSG_SST_CREAT(10)

	#define GPN_CFGMGT_SST_FILE_TRANS_ACT_START			GPN_SOCK_MSG_SST_CREAT(11)
	#define GPN_CFGMGT_SST_SPFILE_TRANS_ACT_START		GPN_SOCK_MSG_SST_CREAT(12)
	#define GPN_CFGMGT_SST_FILE_TRANS_ACT_STOP			GPN_SOCK_MSG_SST_CREAT(13)
	#define GPN_CFGMGT_SST_FT_ACT_TX_REQ				GPN_SOCK_MSG_SST_CREAT(14)	
	#define GPN_CFGMGT_SST_FILE_TRANS_MD5_CHECK			GPN_SOCK_MSG_SST_CREAT(15)
	
/***************************************************************************** **********/
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 2                                                                        */
/****************************************************************************************/

/****************************************************************************************/
/*define main type 1's main-sub type 2's sub-sub type  (from 1 to 0xFE)  :                                                    */
/****************************************************************************************/


/****************************************************************************************/
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 3                                                                        */
/****************************************************************************************/

/*GPN_FT req sta define */
#define GPN_FT_REQ_STA_ERR							1/*ft err*/
#define GPN_FT_REQ_STA_WAIT							2/*ft req creat(in gpn_transfer), but ft task not creat(in gpn_ftp)*/
#define GPN_FT_REQ_STA_RUN							3/*ft req creat(in gpn_transfer), ft task also creat(in gpn_ftp)*/
#define GPN_FT_REQ_STA_DEL							4/*ft req delete(in gpn_transfer)*/
#define GPN_FT_REQ_STA_DONE							5/*ft task over(in gpn_ftp)*/

/*GPN_FT req sta error code define */
#define GPN_FT_REQ_ERR_NONE							0/*no erron happen */
#define GPN_FT_REQ_ERR_UNKNOWE						1/*unknow erron happen */
#define GPN_FT_REQ_ERR_NOIDLE						2/*when creat ft-req, no idle node */
#define GPN_FT_REQ_ERR_CREATED						3/*when creat ft-req, ft-req already creat */
#define GPN_FT_REQ_ERR_PATHERR						4/*when get fileType path, err happen */
#define GPN_FT_REQ_ERR_FILECHECKERR					5/*when do file check, err happen */
#define GPN_FT_REQ_ERR_FILENOTEXIST					6/*file not exist */
#define GPN_FT_REQ_ERR_FILEOPTERR					7/*opt file err */
#define GPN_FT_REQ_ERR_LISTOPTERR					8/*opt list err */
#define GPN_FT_REQ_ERR_NOTHISREQID					9/*not found the reqId in list */
#define GPN_FT_REQ_ERR_RUNNINGTIMEOUT				10/*when req-running state, not rcv gpn_ftp's task state notify */
#define GPN_FT_REQ_ERR_WAITTIMEOUT					11/*can't creat ft-task */
#define GPN_FT_REQ_ERR_TASKREPORTERR				12/*ft-task report err */



#define GPN_FT_MSG_FILE_TRANS_REQ_CREAT				((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_FILE_TRANS_REQ_CREAT))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_FT_MSG_FILE_TRANS_REQ_CREAT
		iSrcId	   = other modu
		iDstId	   = FT
		iMsgPara1  = file type
		iMsgPara2  = file src place(slot) 
		iMsgPara3  = file dst place(slot) 
		msgCellLen = 0
*/
#define GPN_FT_MSG_FILE_TRANS_REQ_CREAT_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_FILE_TRANS_REQ_CREAT))
/* msg parameters direction:
		iIndex	   = CMD_Index
		iMsgType   = GPN_FT_MSG_FILE_TRANS_REQ_CREAT_ACK
		iSrcId	= FT
		iDstId	= other modu
*/
#define GPN_FT_MSG_FILE_TRANS_REQ_CREAT_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_FT_MSG_FILE_TRANS_REQ_CREAT))
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_FT_MSG_FILE_TRANS_REQ_CREAT_RSP
		iSrcId	= FT
		iDstId	= other modu
		iMsgPara1  = file type
		iMsgPara2  = file src place(slot) 
		iMsgPara3  = file dst place(slot) 
		iMsgPara4  = request ID(REQ_ID) 
		iMsgPara5  = max need time(use to judge time out, 's')
		iMsgPara6  = requestion status(REQ_STA 1:ERROR 2: 3: )
		iMsgPara7  = ERR_CODE(if REQ_STA == ERROR),
		msgCellLen = 0
*/
#define GPN_FT_MSG_FILE_TRANS_REQ_CREAT_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_FILE_TRANS_REQ_CREAT_RSP))
/* msg parameters direction:
		iIndex	   = CMD_Index
		iMsgType   = GPN_FT_MSG_FILE_TRANS_REQ_CREAT_RSP_ACK
		iSrcId	   = other modu
		iDstId	   = FT
*/

#define GPN_FT_PARA_MAX_PATH_LEN						256
#define GPN_FT_PARA_SPFILE_TR_CREAT_MSG_LEN				((GPN_SOCK_MSG_HEAD_BLEN) + 2*(GPN_FT_PARA_MAX_PATH_LEN))
#define GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT				((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_SPFILE_TRANS_REQ_CREAT))	/**/
/* msg parameters direction: 
		iIndex	= tx_Index++
		iMsgType  	= GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT
		iSrcId	= other modu
		iDstId	= FT
		iMsgPara1 	= file type(must be 0)
		iMsgPara2 	= file src place(slot) 
		iMsgPara3 	= file dst place(slot) 
		iMsgPara4  = src file full path(include file name) lenth
		iMsgPara5  = dst file full path(include file name) lenth
		msgCellLen = iMsgPara4 +iMsgPara5
		paload =  src file full path +  dst file full path
*/
#define GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT))
/* msg parameters direction:
		iIndex	 = CMD_Index
		iMsgType   = GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT_ACK
		iSrcId	= FT
		iDstId	= other modu
*/
#define GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT))
/* msg parameters direction: 
		iIndex	= tx_Index++
		iMsgType  	= GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT_RSP
		iSrcId	= FT
		iDstId	= other modu
		iMsgPara1  = file type(must be 0)
		iMsgPara2  = file src place(slot) 
		iMsgPara3  = file dst place(slot) 
		iMsgPara4  = src file full path(include file name) lenth
		iMsgPara5  = dst file full path(include file name) lenth
		iMsgPara6  = request ID(REQ_ID) 
		iMsgPara7  = max need time(use to judge time out, 's')
		iMsgPara8  = requestion status(REQ_STA 1:ERROR 2: 3: )
		iMsgPara9  = ERR_CODE(if REQ_STA == ERROR),
		msgCellLen = 0
*/
#define GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT_RSP))
/* msg parameters direction:
		iIndex	= CMD_Index
		iMsgType  	= GPN_FT_MSG_SPFILE_TRANS_REQ_CREAT_RSP_ACK
		iSrcId	= other modu
		iDstId	= FT
*/

#define GPN_FT_MSG_FILE_TRANS_REQ_DEL				((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_FILE_TRANS_REQ_DEL))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_FT_MSG_FILE_TRANS_REQ_DEL
		iSrcId	   = other modu
		iDstId	   = FT
		iMsgPara1  = REQ_ID
		msgCellLen = 0
*/
#define GPN_FT_MSG_FILE_TRANS_REQ_DEL_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_FILE_TRANS_REQ_DEL))
/* msg parameters direction:
		iIndex	   	= CMD_Index
		iMsgType   	= GPN_FT_MSG_FILE_TRANS_REQ_DEL_ACK
		iSrcId		= FT
		iDstId		= other modu
*/
#define GPN_FT_MSG_FILE_TRANS_REQ_DEL_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_FT_MSG_FILE_TRANS_REQ_DEL))
/* msg parameters direction: 
		iIndex	   	= tx_Index++
		iMsgType   	= GPN_FT_MSG_FILE_TRANS_REQ_DEL_RSP
		iSrcId		= FT
		iDstId		= other modu
		iMsgPara1  = REQ_ID
		iMsgPara2  = REQ_STA
		iMsgPara3  = ERR_CODE
		msgCellLen = 0
*/
#define GPN_FT_MSG_FILE_TRANS_REQ_DEL_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_FILE_TRANS_REQ_DEL_RSP))
/* msg parameters direction:
		iIndex	   = CMD_Index
		iMsgType   = GPN_FT_MSG_FILE_TRANS_REQ_DEL_RSP_ACK
		iSrcId	   = other modu
		iDstId	   = FT
	
*/

#define GPN_FT_MSG_REQ_STA_GET					((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_REQ_STA_GET))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_FT_MSG_REQ_STA_GET
		iSrcId	   = other modu
		iDstId	   = FT
		iMsgPara1  = REQ_ID
		msgCellLen = 0
*/
#define GPN_FT_MSG_REQ_STA_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_REQ_STA_GET))
/* msg parameters direction:
		iIndex	   = CMD_Index
		iMsgType   = GPN_FT_MSG_REQ_STA_GET_ACK
		iSrcId	= FT
		iDstId	= other modu
*/

#define GPN_FT_MSG_REQ_STA_NOTIFY					((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_REQ_STA_NOTIFY))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_FT_MSG_REQ_STA_NOTIFY
		iSrcId	   = other modu
		iDstId	   = FT
		iMsgPara1  = REQ_ID
		iMsgPara2  = REQ_STA
		iMsgPara3  = ERR_CODE
		iMsgPara4  = percentage
		iMsgPara5  = SUFFIX_ID
		msgCellLen = 0
*/
#define GPN_FT_MSG_REQ_STA_NOTIFY_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_REQ_STA_NOTIFY))
/* msg parameters direction:
		iIndex	   = CMD_Index
		iMsgType   = GPN_FT_MSG_REQ_STA_NOTIFY_ACK
		iSrcId	= FT
		iDstId	= other modu
*/


/*GPN_FT task sta define */
#define GPN_FT_TASK_STA_ERR							1/*ft task err*/
#define GPN_FT_TASK_STA_WAIT						2/*ft req creat(in gpn_transfer), but ft task not creat(in gpn_ftp)*/
#define GPN_FT_TASK_STA_RUN							3/*ft req creat(in gpn_transfer), ft task also creat(in gpn_ftp)*/
#define GPN_FT_TASK_STA_DEL							4/*ft req delete(in gpn_transfer)*/
#define GPN_FT_TASK_STA_DONE						5/*ft task over(in gpn_ftp)*/

/*GPN_FT task sta error code define */
#define GPN_FT_TASK_ERR_NONE						1/*no erron happen */
#define GPN_FT_TASK_ERR_UNKNOWE						2/*unknow erron happen */
#define GPN_FT_TASK_ERR_NOIDLE						3/*when creat ft-req, no idle node */
#define GPN_FT_TASK_ERR_CREATED						4/*when creat ft-req, ft-req already creat */
#define GPN_FT_TASK_ERR_PATHERR						5/*when get fileType path, err happen */
#define GPN_FT_TASK_ERR_FILENOTEXIST				6/*file not exist */
#define GPN_FT_TASK_ERR_FILEOPTERR					7/*opt file err */
#define GPN_FT_TASK_ERR_LISTOPTERR					8/*opt list err */
#define GPN_FT_TASK_ERR_NOTHISREQID					9/*not found the reqId in list */
#define GPN_FT_TASK_ERR_RUNNINGTIMEOUT				10/*when req-running state, not rcv gpn_ftp's task state notify */
#define GPN_FT_TASK_ERR_WAITTIMEOUT					11/*can't creat ft-task */
#define GPN_FT_TASK_ERR_MD5CHECKERR					12/*can't creat ft-task */
#define GPN_FT_TASK_ERR_FILETYPEERR					13/*can't creat ft-task */
#define GPN_FT_TASK_ERR_POINTERERR					14/*can't creat ft-task */




#define GPN_FT_MSG_FILE_TRANS_TASK_CREAT			((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_FILE_TRANS_TASK_CREAT))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType      = GPN_FT_MSG_FILE_TRANS_TASK_CREAT
		iSrcId	   = gpn_transfer(cpu x)
		iDstId	   = gpn_ftp(src cpu)
		iMsgPara1    = taskOwner
		iMsgPara2    = reqId 
		iMsgPara3    = fileType
		iMsgPara4    = src 
		iMsgPara5    = dst
		msgCellLen  = 0
*/
#define GPN_FT_MSG_FILE_TRANS_TASK_CREAT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_FILE_TRANS_TASK_CREAT))
/* msg parameters direction:
		iIndex	   = CMD_Index
		iMsgType   = GPN_FT_MSG_FILE_TRANS_TASK_CREAT_ACK
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_transfer(cpu x)
*/
#define GPN_FT_MSG_FILE_TRANS_TASK_CREAT_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_FT_MSG_FILE_TRANS_TASK_CREAT))
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_FT_MSG_FILE_TRANS_TASK_CREAT_RSP
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_transfer(cpu x)
		iMsgPara1  = reqId 
		iMsgPara2  = taskId(src ftp)
		iMsgPara3  = maxTime;
		iMsgPara4  = TASK_STA
		iMsgPara5  = ERR_CODE(if TASK_STA == ERROR),
		msgCellLen = 0
*/
#define GPN_FT_MSG_FILE_TRANS_TASK_CREAT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_FILE_TRANS_TASK_CREAT_RSP))
/* msg parameters direction:
		iIndex	   = CMD_Index
		iMsgType   = GPN_FT_MSG_FILE_TRANS_TASK_CREAT_RSP_ACK
		iSrcId	   = gpn_transfer(cpu x)
		iDstId	   = gpn_ftp(src cpu)
*/


#define GPN_FT_PARA_SPFILE_TT_CREAT_MSG_LEN			((GPN_SOCK_MSG_HEAD_BLEN) + 2*(GPN_FT_PARA_MAX_PATH_LEN))
#define GPN_FT_MSG_SPFILE_TRANS_TASK_CREAT			((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_SPFILE_TRANS_TASK_CREAT))	/**/
/* msg parameters direction: 
		iIndex	 = tx_Index++
		iMsgType	 = GPN_FT_MSG_SPFILE_TRANS_TASK_CREAT
		iSrcId	 = gpn_transfer(cpu x)
		iDstId	 = gpn_ftp(src file cpu)
		iMsgPara1	 = taskOwner
		iMsgPara2	 = reqId
		iMsgPara3	 = fileType(must be 0)
		iMsgPara4  = file src place(slot)
		iMsgPara5  = file dst place(slot)
		iMsgPara6  = src file full path(include file name) lenth
		iMsgPara7  = dst file full path(include file name) lenth
		msgCellLen = iMsgPara6 +iMsgPara7
		paload =  src file full path +  dst file full path
*/
#define GPN_FT_MSG_SPFILE_TRANS_TASK_CREAT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_SPFILE_TRANS_TASK_CREAT))
/* msg parameters direction:
		iIndex	= CMD_Index
		iMsgType  	= GPN_FT_MSG_SPFILE_TRANS_TASK_CREAT_ACK
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_transfer(cpu x)
*/
#define GPN_FT_MSG_SPFILE_TRANS_TASK_CREAT_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_FT_MSG_SPFILE_TRANS_TASK_CREAT))
/* msg parameters direction: 
		iIndex	 = tx_Index++
		iMsgType   = GPN_FT_MSG_SPFILE_TRANS_TASK_CREAT_RSP
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_transfer(cpu x)
		iMsgPara1  = reqId 
		iMsgPara2  = taskId(src ftp)
		iMsgPara3  = maxTime;
		iMsgPara4  = TASK_STA
		iMsgPara5  = ERR_CODE(if TASK_STA == ERROR),
		msgCellLen = 0
*/
#define GPN_FT_MSG_SPFILE_TRANS_TASK_CREAT_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_SPFILE_TRANS_TASK_CREAT_RSP))
/* msg parameters direction:
		iIndex	= CMD_Index
		iMsgType   = GPN_FT_MSG_SPFILE_TRANS_TASK_CREAT_RSP_ACK
		iSrcId	= gpn_transfer(cpu x)
		iDstId	= gpn_ftp(src cpu)
*/


#define GPN_FT_MSG_FILE_TRANS_TASK_DEL				((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_FILE_TRANS_TASK_DEL))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType      = GPN_FT_MSG_FILE_TRANS_TASK_DEL
		iSrcId	   = gpn_transfer(cpu x)
		iDstId	   = gpn_ftp(src cpu)
		iMsgPara1     = reqId
		iMsgPara1     = TASK_ID(in MASTE, it is local task Id)
		msgCellLen   = 0
*/
#define GPN_FT_MSG_FILE_TRANS_TASK_DEL_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_FILE_TRANS_TASK_DEL))
/* msg parameters direction:
		iIndex	   = CMD_Index
		iMsgType      = GPN_FT_MSG_FILE_TRANS_TASK_DEL_ACK
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_transfer(cpu x)

*/
#define GPN_FT_MSG_FILE_TRANS_TASK_DEL_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_FT_MSG_FILE_TRANS_TASK_DEL))
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType      = GPN_FT_MSG_FILE_TRANS_TASK_DEL_RSP
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_transfer(cpu x)
		iMsgPara1  = REQ_ID
		iMsgPara2  = TASK_ID(src ftp)
		iMsgPara3  = TASK_STA
		iMsgPara4  = ERROR_CODE
		msgCellLen = 0
*/
#define GPN_FT_MSG_FILE_TRANS_TASK_DEL_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_FILE_TRANS_TASK_DEL_RSP))
/* msg parameters direction:
		iIndex	   = CMD_Index
		iMsgType      = GPN_FT_MSG_FILE_TRANS_TASK_DEL_RSP_ACK
		iSrcId	   = gpn_transfer(cpu x)
		iDstId	   = gpn_ftp(src cpu)
*/

#define GPN_FT_MSG_TASK_STA_GET						((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_TASK_STA_GET))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType      = GPN_FT_MSG_TASK_STA_GET
		iSrcId	   = gpn_transfer(cpu x)
		iDstId	   = gpn_ftp(src cpu)
		iMsgPara1     = reqId
		iMsgPara2     = TASK_ID(in MASTE, it is local task Id)
		msgCellLen   = 0
*/
#define GPN_FT_MSG_TASK_STA_GET_ACK					((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_TASK_STA_GET))
/* msg parameters direction:
		iIndex	 = CMD_Index
		iMsgType   = GPN_FT_MSG_TASK_STA_GET_ACK
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_transfer(cpu x)

*/

#define GPN_FT_MSG_TASK_STA_NOTIFY					((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_TASK_STA_NOTIFY))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_FT_MSG_TASK_STA_NOTIFY
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_transfer(cpu x)
		iMsgPara1  = REQ_ID
		iMsgPara2  = TASK_ID(src ftp)
		iMsgPara3  = TASK_STA
		iMsgPara4  = ERR_CODE
		iMsgPara5  = percentage
		iMsgPara6  = SUFFIX_ID
		msgCellLen = 0
*/
#define GPN_FT_MSG_TASK_STA_NOTIFY_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_FT_MSG_TASK_STA_NOTIFY))
/* msg parameters direction:
		iIndex	= CMD_Index
		iMsgType  	= GPN_FT_MSG_TASK_STA_NOTIFY_ACK
		iSrcId	= gpn_transfer(cpu x)
		iDstId	= gpn_ftp(src cpu)
*/


#define GPN_CFGMGT_MSG_FILE_TRANS_ACT_START			((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_FILE_TRANS_ACT_START))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FILE_TRANS_ACT_START
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_ftp(dst cpu)
		iMsgPara1  = TASK_ID(src ftp)
		iMsgPara2  = fileType
		iMsgPara3  = fileSrc
		iMsgPara4  = fileDst
		iMsgPara5  = totBlock
		msgCellLen = 0
*/
#define GPN_CFGMGT_MSG_FILE_TRANS_ACT_START_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_FILE_TRANS_ACT_START))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FILE_TRANS_ACT_START
		iSrcId	= gpn_ftp(dst cpu)
		iDstId	= gpn_ftp(src cpu)
*/
#define GPN_CFGMGT_MSG_FILE_TRANS_ACT_START_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_CFGMGT_MSG_FILE_TRANS_ACT_START))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FILE_TRANS_ACT_START_ACK
		iSrcId	= gpn_ftp(dst cpu)
		iDstId	= gpn_ftp(src cpu)
		iMsgPara1  = TASK_ID(src ftp)
		iMsgPara2  = TASK_ID(dst ftp)
		iMsgPara3  = TASK_STA
		iMsgPara4  = ERR_CODE
		msgCellLen = 0
*/
#define GPN_CFGMGT_MSG_FILE_TRANS_ACT_START_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_FILE_TRANS_ACT_START_RSP))
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FILE_TRANS_ACT_START
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_ftp(dst cpu)

*/

#define GPN_FT_PARA_SPFILE_TT_START_MSG_LEN			((GPN_SOCK_MSG_HEAD_BLEN) + 2*(GPN_FT_PARA_MAX_PATH_LEN))
#define GPN_CFGMGT_MSG_SPFILE_TRANS_ACT_START			((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_SPFILE_TRANS_ACT_START))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_SPFILE_TRANS_ACT_START
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_ftp(dst cpu)
		iMsgPara1  = TASK_ID(src ftp)
		iMsgPara2  = totBlock
		iMsgPara3	= fileType(must be 0)
		iMsgPara4  = file src place(slot)
		iMsgPara5  = file dst place(slot)
		iMsgPara6  = src file full path(include file name) lenth
		iMsgPara7  = dst file full path(include file name) lenth
		msgCellLen = iMsgPara6 +iMsgPara7
		paload =  src file full path +  dst file full path
*/
#define GPN_CFGMGT_MSG_SPFILE_TRANS_ACT_START_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_SPFILE_TRANS_ACT_START))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_SPFILE_TRANS_ACT_START
		iSrcId	= gpn_ftp(dst cpu)
		iDstId	= gpn_ftp(src cpu)
*/
#define GPN_CFGMGT_MSG_SPFILE_TRANS_ACT_START_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_CFGMGT_MSG_SPFILE_TRANS_ACT_START))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_SPFILE_TRANS_ACT_START_ACK
		iSrcId	= gpn_ftp(dst cpu)
		iDstId	= gpn_ftp(src cpu)
		iMsgPara1  = TASK_ID(src ftp)
		iMsgPara2  = TASK_ID(dst ftp)
		iMsgPara3  = TASK_STA
		iMsgPara4  = ERR_CODE
		msgCellLen = 0
*/
#define GPN_CFGMGT_MSG_SPFILE_TRANS_ACT_START_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_SPFILE_TRANS_ACT_START_RSP))
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_SPFILE_TRANS_ACT_START
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_ftp(dst cpu)

*/

#define GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP			((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_FILE_TRANS_ACT_STOP))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_ftp(dst cpu)
		iMsgPara1  = TASK_ID(src ftp)
		iMsgPara2  = TASK_ID(dst ftp)
		msgCellLen = 0
*/
#define GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP
		iSrcId	= gpn_ftp(dst cpu)
		iDstId	= gpn_ftp(src cpu)
*/
#define GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP))	/**/
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP_ACK
		iSrcId	= gpn_ftp(dst cpu)
		iDstId	= gpn_ftp(src cpu)
		iMsgPara1  = TASK_ID(src ftp)
		iMsgPara2  = TASK_ID(dst ftp)
		iMsgPara3  = TASK_STA
		iMsgPara4  = ERR_CODE
		msgCellLen = 0
*/
#define GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP_RSP))
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FILE_TRANS_ACT_STOP
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_ftp(dst cpu)

*/
#define GPN_FT_PARA_MAX_TXB_PER_REQ					4
#define GPN_FT_PARA_BLOCK_MAX_SIZE					1024
#define GPN_FT_PARA_TASK_TX_MSG_LEN					((GPN_FT_PARA_BLOCK_MAX_SIZE)+(GPN_SOCK_MSG_HEAD_BLEN))
#define GPN_CFGMGT_MSG_FT_ACT_TX_REQ				((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_FT_ACT_TX_REQ))	/**/					
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FT_ACT_TX_REQ
		iSrcId	= gpn_ftp(dst cpu)
		iDstId	= gpn_ftp(src cpu)
		iMsgPara1  = TASK_ID(src ftp)
		iMsgPara2  = TASK_ID(dst ftp)
		iMsgPara3  = blockStart
		iMsgPara4  = blockNum
		iMsgPara5  = pBSize
		msgCellLen = 0
*/
#define GPN_CFGMGT_MSG_FT_ACT_TX_REQ_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_FT_ACT_TX_REQ))	/**/					
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FT_ACT_TX_REQ_ACK
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_ftp(dst cpu)
*/
#define GPN_CFGMGT_MSG_FT_ACT_TX_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_CFGMGT_MSG_FT_ACT_TX_REQ))	/**/					
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FT_ACT_TX_RSP
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_ftp(dst cpu)
		iMsgPara1  = TASK_ID(src ftp)
		iMsgPara2  = TASK_ID(dst ftp)
		iMsgPara3  = blockLab
		iMsgPara4  = crc32
		iMsgPara5  = realBlockSzie
		msgCellLen = blockSzie+msgHead
*/
#define GPN_CFGMGT_MSG_FT_ACT_TX_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_FT_ACT_TX_RSP))	/**/					
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FT_ACT_TX_RSP_ACK
		iSrcId	= gpn_ftp(dst cpu)
		iDstId	= gpn_ftp(src cpu)
*/


#define GPN_FT_PARA_MD5_SIZE							32
#define GPN_FT_PARA_CHECK_MD5_MSG_LEN				((GPN_FT_PARA_MD5_SIZE)+(GPN_SOCK_MSG_HEAD_BLEN))
#define GPN_CFGMGT_MSG_FILE_TRANS_MD5_CHECK 		((GPN_SOCK_OWNER_FT)|(GPN_FT_MT_USER_API)|(GPN_CFGMGT_MST_FILE_TRANS_OPT)|(GPN_CFGMGT_SST_FILE_TRANS_MD5_CHECK))	/**/	
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FILE_TRANS_MD5_CHECK
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_ftp(dst cpu)
		iMsgPara1  = TASK_ID(dst ftp)
		iMsgPara2  = TASK_ID(src ftp)
		iMsgPara3  = sizeof(MD5)
		msgCellLen = sizeof(MD5)+msgHead
		
*/
#define GPN_CFGMGT_MSG_FILE_TRANS_MD5_CHECK_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_FILE_TRANS_MD5_CHECK))
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FT_ACT_TX_RSP_ACK
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_ftp(dst cpu)
*/
	
#define GPN_CFGMGT_MSG_FILE_TRANS_MD5_CHECK_RSP 	((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_CFGMGT_MSG_FILE_TRANS_MD5_CHECK))	/**/	
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FILE_TRANS_MD5_CHECK_RSP
		iSrcId	= gpn_ftp(src cpu)
		iDstId	= gpn_ftp(dst cpu)
		iMsgPara1  = TASK_ID(src ftp)
		iMsgPara2  = TASK_ID(dst ftp)
		iMsgPara3  = STA(1 : ok, 2 err)
		
*/
#define GPN_CFGMGT_MSG_FILE_TRANS_MD5_CHECK_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_CFGMGT_MSG_FILE_TRANS_MD5_CHECK_RSP))
/* msg parameters direction: 
		iIndex	   = tx_Index++
		iMsgType   = GPN_CFGMGT_MSG_FILE_TRANS_MD5_CHECK_RSP_ACK
		iSrcId	= gpn_ftp(dst cpu)
		iDstId	= gpn_ftp(src cpu)
*/

UINT32 gpnSockFTMsgFileSynReqCreat(UINT32 fileType);
UINT32 gpnSockFTMsgFtReqCreatNew(UINT32 fileType, UINT32 src, UINT32 dst);
UINT32 gpnSockFTMsgFtReqCreat(stSockFdSet *pstFdSet, UINT32 fileType, UINT32 src, UINT32 dst);
UINT32 gpnSockFTMsgFtSpReqCreatNew(UINT32 fileType,
	UINT32 src, char *srcPath, UINT32 spLen, UINT32 dst, char *dstPath, UINT32 dpLen);
UINT32 gpnSockFTMsgFtSpReqCreat(stSockFdSet *pstFdSet,  UINT32 fileType,
	UINT32 src, char *srcPath, UINT32 spLen, UINT32 dst, char *dstPath, UINT32 dpLen);
UINT32 gpnSockFTMsgFtReqCreatRsp(stSockFdSet *pstFdSet, UINT32 reqOwner, UINT32 fileType,
	UINT32 src, UINT32 dst, UINT32 reqId, UINT32 maxTimer, UINT32 sta, UINT32 errCode);
UINT32 gpnSockFTMsgFtSpReqCreatRsp(stSockFdSet *pstFdSet, UINT32 reqOwner, UINT32 fileType,
	UINT32 src, char *srcPath, UINT32 spLen, UINT32 dst, char *dstPath, UINT32 dpLen,
	UINT32 reqId, UINT32 maxTimer, UINT32 sta, UINT32 errCode);
UINT32 gpnSockFTMsgFtReqDeleteNew(UINT32 reqId);
UINT32 gpnSockFTMsgFtReqDelete(stSockFdSet *pstFdSet, UINT32 reqId);
UINT32 gpnSockFTMsgFtReqDeleteRsp(stSockFdSet *pstFdSet, UINT32 reqOwner,
	UINT32 reqId, UINT32 sta, UINT32 errCode);
UINT32 gpnSockFTMsgFtReqStaGetNew(UINT32 reqId);
UINT32 gpnSockFTMsgFtReqStaGet(stSockFdSet *pstFdSet, UINT32 reqId);
UINT32 gpnSockFTMsgFtReqStaNotify(stSockFdSet *pstFdSet, UINT32 reqOwner, UINT32 reqId,
	UINT32 sta, UINT32 errCode, UINT32 percent, UINT32 sufix);

/****************************************************************************************/
/*                               TASK  API                                                                                                            */
/****************************************************************************************/
UINT32 gpnSockFTMsgFtTaskCreat(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 reqId, UINT32 fileType, UINT32 src, UINT32 dst);
UINT32 gpnSockFTMsgFtSpTaskCreat(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 reqId, UINT32 fileType, UINT32 src, char *srcPath, UINT32 spLen,
	UINT32 dst, char *dstPath, UINT32 dpLen);
UINT32 gpnSockFTMsgFtTaskCreatRsp(stSockFdSet *pstFdSet, UINT32 taskOwner, UINT32 reqId,
	UINT32 localTaskId, UINT32 maxTime, UINT32 taskSta, UINT32 errCode);
UINT32 gpnSockFTMsgFtTaskDelete(stSockFdSet *pstFdSet, UINT32 dstCPU, UINT32 reqId, UINT32 taskId);
UINT32 gpnSockFTMsgFtTaskDeleteRsp(stSockFdSet *pstFdSet, UINT32 taskOwner,
	UINT32 reqId, UINT32 localTaskId, UINT32 sta, UINT32 errCode);
UINT32 gpnSockFTMsgFtTaskStaGet(stSockFdSet *pstFdSet, UINT32 dstCPU, UINT32 reqId, UINT32 taskId);
UINT32 gpnSockFTMsgFtTaskStaNotify(stSockFdSet *pstFdSet, UINT32 taskOwner, UINT32 reqId,
	UINT32 localTaskId, UINT32 sta, UINT32 errCode, UINT32 percent, UINT32 sufix);

/****************************************************************************************/
/*                               ACTION  API                                                                                                        */
/****************************************************************************************/
UINT32 gpnSockFTMsgFtTaskStart(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 fileType, UINT32 fileSrc, UINT32 fileDst, UINT32 totBlock);
UINT32 gpnSockFTMsgFtSpTaskStart(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 totBlock, UINT32 fileType, UINT32 src, char *srcPath,
	UINT32 spLen, UINT32 dst, char *dstPath, UINT32 dpLen);
UINT32 gpnSockFTMsgFtTaskStartRsp(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 dstTaskId, UINT32 sta, UINT32 errCode);
UINT32 gpnSockFTMsgFtTaskStop(stSockFdSet *pstFdSet, UINT32 dstCPU, UINT32 srcTaskId, UINT32 dstTaskId);
UINT32 gpnSockFTMsgFtTaskStopRsp(stSockFdSet *pstFdSet, UINT32 msgDst,
	UINT32 srcTaskId, UINT32 dstTaskId, UINT32 sta, UINT32 errCode);
UINT32 gpnSockFTMsgFtTaskTxReq(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 dstTaskId, UINT32 bStart, UINT32 bNum, UINT32 bSize);
UINT32 gpnSockFTMsgFtTaskTxRsp(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 dstTaskId, UINT32 bLab, UINT32 crc32, UINT8 *text, UINT32 len);
UINT32 gpnSockFTMsgFtTaskMD5CheckReq(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 dstTaskId, UINT8 *md5, UINT32 len);
UINT32 gpnSockFTMsgFtTaskMD5CheckRsp(stSockFdSet *pstFdSet, UINT32 dstCPU,
	UINT32 srcTaskId, UINT32 dstTaskId, UINT32 sta);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_FT_MSG_DEF_H_*/


