/**********************************************************
* file name: gpnSockSdmMsgDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan Wangyx 2014-07-29
* function: 
*    define details about communication between sdmd modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_SDM_MSG_DEF_H_
#define _GPN_SOCK_SDM_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnSockMsgDef.h"
#include "lib/gpnSocket/socketComm/gpnSelectOpt.h"

/*==========================================================*/
/*==========================================================*/
/*define main type  (from 1 to 0x7E)  : 1                                                                        */
/*************************************************************************/
/*this main type include upData msg 					                                           */
#define GPN_SDMD_MT_SDM_UPDATA					GPN_SOCK_MSG_MT_CREAT(1)
/***************************************************************************** **********/
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  :   1                                                                     */
/****************************************************************************************/
#define GPN_SDMD_MST_WITH_SELF					GPN_SOCK_MSG_MST_CREAT(1)
/****************************************************************************************/
/*define main type 1's main-sub type 1's sub-sub type  (from 1 to 0xFE)  :                                                                             */
/****************************************************************************************/
	#define GPN_SDMD_SST_START_DOWNLOAD_REQ			GPN_SOCK_MSG_SST_CREAT(1)
	#define GPN_SDMD_SST_START_UPLOAD_REQ			GPN_SOCK_MSG_SST_CREAT(2)
	#define GPN_SDMD_SST_TAG_FLIE_REPLACE			GPN_SOCK_MSG_SST_CREAT(3)
	#define GPN_SDMD_SST_TAG_FLIE_DELETE			GPN_SOCK_MSG_SST_CREAT(4)

#define GPN_SDM_PARA_MAX_PATH_LEN					256
#define GPN_SDM_PARA_SPFILE_TR_CREAT_MSG_LEN		((GPN_SOCK_MSG_HEAD_BLEN) + 2*(GPN_SDM_PARA_MAX_PATH_LEN))
#define GPN_SDMD_MSG_START_DOWNLOAD_REQ				((GPN_SOCK_OWNER_SDM)|(GPN_SDMD_MT_SDM_UPDATA)|(GPN_SDMD_MST_WITH_SELF)|(GPN_SDMD_SST_START_DOWNLOAD_REQ))	/**/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_SDMD_MSG_START_DOWNLOAD_REQ
            iSrcId     = SDMD(NMx)
            iDstId     = SDMD(sub)
            iMsgPara1  = devIndex(dst_slot)
            iMsgPara2  = device type
            iMsgPara3  = file type
            iMsgPara4  = file size
            msgCellLen = file name length
            PAYLOAD:	file name
*/
#define GPN_SDMD_MSG_START_DOWNLOAD_REQ_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_SDMD_MSG_START_DOWNLOAD_REQ))
/* msg parameters direction:
			iIndex     = CMD_Index
			iMsgType   = GPN_SDMD_MSG_START_DOWNLOAD_OPT_ACK
            iSrcId     = SDMD(sub)
            iDstId     = SDMD(NMx)
*/
#define GPN_SDMD_MSG_START_DOWNLOAD_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_SDMD_MSG_START_DOWNLOAD_REQ))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_SDMD_MSG_START_DOWNLOAD_RSP
			iSrcId	   = SDMD(sub)
			iDstId	   = SDMD(NMx)
			iMsgPara1  = devIndex(src_slot)
			iMsgPara2  = device type
			iMsgPara3  = file type
			iMsgPara4  = stat(1-ok, 2-error)
			iMsgPara5  = error code
			iMsgPara6  = file name length
			iMsgPara7  = file path length
			msgCellLen =  file name length + file path length
			PAYLOAD:	file name + file path
*/
#define GPN_SDMD_MSG_START_DOWNLOAD_RSP_ACK		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_SDMD_MSG_START_DOWNLOAD_RSP))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_SDMD_MSG_START_DOWNLOAD_RSP_ACK
            iSrcId     = SDMD(NMx)
            iDstId     = SDMD(sub)
*/

#define GPN_SDMD_MSG_START_UPLOAD_REQ			((GPN_SOCK_OWNER_SDM)|(GPN_SDMD_MT_SDM_UPDATA)|(GPN_SDMD_MST_WITH_SELF)|(GPN_SDMD_SST_START_UPLOAD_REQ))	/**/
/* msg parameters direction: 
			iIndex     	= tx_Index++
			iMsgType   = GPN_SDMD_MSG_START_DOWNLOAD_REQ
            iSrcId     	= SDMD(NMx)
            iDstId     	= SDMD(sub)
            iMsgPara1  = devIndex(dst_slot)
            iMsgPara2  = device type
            iMsgPara3  = file type
			iMsgPara4  = file name length                   	
            msgCellLen = file name length
            PAYLOAD = file name
*/	
#define GPN_SDMD_MSG_START_UPLOAD_REQ_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SDMD_MSG_START_UPLOAD_REQ))
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType  	= GPN_SDMD_MSG_START_UPLOAD_REQ_ACK
			iSrcId	= SDMD(sub)
			iDstId	= SDMD(NMx)
*/	
#define GPN_SDMD_MSG_START_UPLOAD_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_SDMD_MSG_START_UPLOAD_REQ))	/**/
/* msg parameters direction: 
			iIndex        = tx_Index++
			iMsgType   = GPN_SDMD_MSG_START_UPLOAD_RSP
			iSrcId	= SDMD(sub)
			iDstId	= SDMD(NMx)
			iMsgPara1  = devIndex(src_slot)
			iMsgPara2  = device type
			iMsgPara3  = file type
			iMsgPara4  = stat(1-ok, 2-error)
			iMsgPara5  = error code
			iMsgPara6  = file size
			iMsgPara7  = file name len
			iMsgPara8  = file path len
			
			msgCellLen = file name len + file path len
			PAYLOAD:	full file name (name + path)
*/
#define GPN_SDMD_MSG_START_UPLOAD_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SDMD_MSG_START_UPLOAD_REQ))
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_SDMD_MSG_START_UPLOAD_RSP_ACK
			iSrcId	= SDMD(NMx)
			iDstId	= SDMD(sub)
*/

#define GPN_SDMD_MSG_TAG_FILE_REPLACE_OPT			((GPN_SOCK_OWNER_SDM)|(GPN_SDMD_MT_SDM_UPDATA)|(GPN_SDMD_MST_WITH_SELF)|(GPN_SDMD_SST_TAG_FLIE_REPLACE))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType      = GPN_SDMD_MSG_TAG_FILE_REPLACE_OPT
			iSrcId	   = SDMD(NMx)
			iDstId	   = SDMD(sub)
			iMsgPara1  = devIndex(dst_slot)
			iMsgPara2  = device type
			iMsgPara3  = file type
			iMsgPara4  = suffixId
			iMsgPara5  = file name len
			iMsgPara6  = file path len
			msgCellLen = (file name len + file path len) (not include suffixId)
			PAYLOAD:	full file name (name + path)
*/
#define GPN_SDMD_MSG_TAG_FILE_REPLACE_OPT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SDMD_MSG_TAG_FILE_REPLACE_OPT))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_SDMD_MSG_TAG_FILE_REPLACE_OPT_ACK
			iSrcId	   = SDMD(sub)
			iDstId	   = SDMD(NMx)
*/		
#define GPN_SDMD_MSG_TAG_FILE_REPLACE_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_SDMD_MSG_TAG_FILE_REPLACE_OPT))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_SDMD_MSG_TAG_FILE_REPLACE_RSP
			iSrcId	   = SDMD(sub)
			iDstId	   = SDMD(NMx)
			iMsgPara1  = devIndex(src_slot)
			iMsgPara2  = device type
			iMsgPara3  = file type
			iMsgPara4  = suffixId
			iMsgPara5  = sta(1-ok, 2-error)
			iMsgPara6  = error code
			iMsgPara7  = file name len
			iMsgPara8  = file path len
			
			msgCellLen = (file name len + file path len) (not include suffixId)
			PAYLOAD:	full file name (name + path)
*/
#define GPN_SDMD_MSG_TAG_FILE_REPLACE_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SDMD_MSG_TAG_FILE_REPLACE_RSP))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_SDMD_MSG_TAG_FILE_REPLACE_RSP_ACK
			iSrcId	   = SDMD(NMx)
			iDstId	   = SDMD(sub)

*/

#define GPN_SDMD_MSG_TAG_FILE_DELETE_OPT			((GPN_SOCK_OWNER_SDM)|(GPN_SDMD_MT_SDM_UPDATA)|(GPN_SDMD_MST_WITH_SELF)|(GPN_SDMD_SST_TAG_FLIE_DELETE))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType      = GPN_SDMD_MSG_TAG_FILE_DELETE_OPT
			iSrcId	   = SDMD(NMx)
			iDstId	   = SDMD(sub)
			iMsgPara1  = devIndex(dst_slot)
			iMsgPara2  = device type
			iMsgPara3  = file type
			iMsgPara4  = file name len
			iMsgPara5  = file path len
			msgCellLen = (file name len + file path len) (not include suffixId)
			PAYLOAD:	full file name (name + path)
*/
#define GPN_SDMD_MSG_TAG_FILE_DELETE_OPT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SDMD_MSG_TAG_FILE_DELETE_OPT))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_SDMD_MSG_TAG_FILE_DELETE_OPT_ACK
			iSrcId	   = SDMD(sub)
			iDstId	   = SDMD(NMx)
*/

UINT32 gpnSockSDMDMsgTxStartDownloadReq(stSockFdSet *pstFdSet, 
	UINT32 dstSlot, UINT32 devType, UINT32 fileType, UINT32 fileSize, UINT32 fileNameLen, char *fileName);
UINT32 gpnSockSDMDMsgTxStartDownloadRsp(stSockFdSet *pstFdSet, 	UINT32 dstSlot, UINT32 srcSlot, 
	UINT32 devType,  UINT32 fileType, UINT32 stat, UINT32 errCode, UINT32 fileNameLen, UINT32 filePathLen, 
	INT8 *fileName, INT8 *filePath);
UINT32 gpnSockSDMDMsgTxStartUploadReq(stSockFdSet *pstFdSet, 
	UINT32 dstSlot, UINT32 devType, UINT32 fileType, UINT32 fileNameLen, INT8 *fileName);
UINT32 gpnSockSDMDMsgTxStartUploadRsp(stSockFdSet *pstFdSet, 	UINT32 dstSlot, UINT32 srcSlot, 
	UINT32 devType,  UINT32 fileType, UINT32 stat, UINT32 errCode, UINT32 fileSize, UINT32 fileNameLen, 
	UINT32 filePathLen, INT8 *fileName, INT8 *filePath);

UINT32 gpnSockSDMDMsgTxTagFileRepalceOpt(stSockFdSet *pstFdSet, UINT32 dstSlot, UINT32 devType,
	UINT32 fileType, UINT32 suffixID, UINT32 fileNameLen, UINT32 filePathLen, INT8 *fileName, INT8 *filePath);
UINT32 gpnSockSDMDMsgTxTagFileRepalceRsp(stSockFdSet *pstFdSet, 
	UINT32 dstSlot, UINT32 srcSlot, UINT32 devType,  UINT32 fileType, UINT32 suffixID, 
	UINT32 stat, UINT32 errCode, UINT32 fileNameLen, UINT32 filePathLen, INT8 *fileName, INT8 *filePath);
UINT32 gpnSockSDMDMsgTxTagFileDeleteOpt(stSockFdSet *pstFdSet, 
	UINT32 dstSlot, UINT32 devType, UINT32 fileType, UINT32 fileNameLen, UINT32 filePathLen, 
	INT8 *fileName, INT8 *filePath);

/*==========================================================*/
/*==========================================================*/
/*define main type  (from 1 to 0x7E)  : 1                                                                        */
/*************************************************************************/
/*this main type include upData msg 					                                           */
#define GPN_SDMD_MT_EMD_OPT						GPN_SOCK_MSG_MT_CREAT(2)
/***************************************************************************** **********/
/***************************************************************************** **********/
/*define main type 1's main-sub type  (from 1 to 0xFE)  :   1                                                                     */
/****************************************************************************************/
#define GPN_SDMD_MST_EMD_NOTIFY					GPN_SOCK_MSG_MST_CREAT(1)
/****************************************************************************************/
/*define main type 1's main-sub type 1's sub-sub type  (from 1 to 0xFE)  :                                                    */
/****************************************************************************************/
	#define GPN_SDMD_SST_EMD_SLOT_STAT_GET			GPN_SOCK_MSG_SST_CREAT(1)
	#define GPN_SDMD_SST_EMD_SLOT_OPEN_NOTIFY		GPN_SOCK_MSG_SST_CREAT(2)
	#define GPN_SDMD_SST_EMD_SLOT_CLOSE_NOTIFY		GPN_SOCK_MSG_SST_CREAT(3)
	#define GPN_SDMD_SST_EMD_LED_CONTROL			GPN_SOCK_MSG_SST_CREAT(4)
	#define GPN_SDMD_SST_EMD_TEM_STAT_NOTIFY		GPN_SOCK_MSG_SST_CREAT(5)
	#define GPN_SDMD_SST_EMD_NOTIFYED_CFG_SAVE		GPN_SOCK_MSG_SST_CREAT(6)

#define GPN_SDMD_MSG_EMD_SLOT_STAT_GET  				((GPN_SOCK_OWNER_SDM)|(GPN_SDMD_MT_EMD_OPT)|(GPN_SDMD_MST_EMD_NOTIFY)|(GPN_SDMD_SST_EMD_SLOT_STAT_GET))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_SDMD_MSG_EMD_SLOT_STAT_GET
			iSrcId	   = EMD
			iDstId	   = NSM
			iMsgPara1  = slot( IFM_SLOT_MAX_ID means all slot )
			msgCellLen = 0
*/
#define GPN_SDMD_MSG_EMD_SLOT_STAT_GET_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_SDMD_MSG_EMD_SLOT_STAT_GET))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_SDMD_MSG_EMD_SLOT_OPEN_NOTIFY_ACK
			iSrcId	   = NSM
			iDstId	   = EMD
*/

#define GPN_SDMD_MSG_EMD_SLOT_OPEN_NOTIFY				((GPN_SOCK_OWNER_SDM)|(GPN_SDMD_MT_EMD_OPT)|(GPN_SDMD_MST_EMD_NOTIFY)|(GPN_SDMD_SST_EMD_SLOT_OPEN_NOTIFY))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_SDMD_MSG_EMD_SLOT_OPEN_NOTIFY
			iSrcId	   = EMD
			iDstId	   = NSM
			iMsgPara1  = slot
			iMsgPara2  = device type
			msgCellLen = 0
*/
#define GPN_SDMD_MSG_EMD_SLOT_OPEN_NOTIFY_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_SDMD_MSG_EMD_SLOT_OPEN_NOTIFY))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_SDMD_MSG_EMD_SLOT_OPEN_NOTIFY_ACK
			iSrcId	   = NSM
			iDstId	   = EMD
*/

#define GPN_SDMD_MSG_EMD_SLOT_CLOSE_NOTIFY				((GPN_SOCK_OWNER_SDM)|(GPN_SDMD_MT_EMD_OPT)|(GPN_SDMD_MST_EMD_NOTIFY)|(GPN_SDMD_SST_EMD_SLOT_CLOSE_NOTIFY))	/**/
/* msg parameters direction: 
			iIndex	   = tx_Index++
			iMsgType   = GPN_SDMD_MSG_EMD_SLOT_CLOSE_NOTIFY
			iSrcId	   = EMD
			iDstId	   = NSM
			iMsgPara1  = slot
			iMsgPara2  = device type
			msgCellLen = 0
*/
#define GPN_SDMD_MSG_EMD_SLOT_CLOSE_NOTIFY_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_SDMD_MSG_EMD_SLOT_CLOSE_NOTIFY))
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType   = GPN_SDMD_MSG_EMD_SLOT_CLOSE_NOTIFY_ACK
			iSrcId	   = NSM
			iDstId	   = EMD
*/

#define GPN_LED_TYPE_RUN           1
#define GPN_LED_TYPE_ALM           2
#define GPN_LED_TYPE_BUSY          3
#define GPN_LED_TYPE_XXX           4

#define GPN_LED_CONTROL_OFF        1
#define GPN_LED_CONTROL_ON         2
#define GPN_LED_CONTROL_FLASH      3
#define GPN_LED_CONTROL_BLINK      4
#define GPN_SDMD_MSG_EMD_LED_CONTROL					((GPN_SOCK_OWNER_SDM)|(GPN_SDMD_MT_EMD_OPT)|(GPN_SDMD_MST_EMD_NOTIFY)|(GPN_SDMD_SST_EMD_LED_CONTROL))	/**/
/* msg parameters direction: 
			iIndex	 = tx_Index++
			iMsgType   = GPN_SDMD_MSG_EMD_LED_CONTROL
			iSrcId	 = X-module
			iDstId	 = EMD
			iMsgPara1  = slot
			iMsgPara2  = led type
			iMsgPara3  = led num
			iMsgPara4  = led opt(OFF, ON, FLASH, BLINK)
			msgCellLen = 0
*/
#define GPN_SDMD_MSG_EMD_LED_CONTROL_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_SDMD_MSG_EMD_LED_CONTROL))
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_SDMD_MSG_EMD_LED_CONTROL_ACK
			iSrcId	= EMD
			iDstId	= X-module
*/

#define GPN_TEM_STAT_NORMAL            1
#define GPN_TEM_STAT_ABNORMAL          2

#define GPN_TEM_DETAIL_HIGH_RISE       1
#define GPN_TEM_DETAIL_HIGH_CLEAN      2
#define GPN_TEM_DETAIL_LOW_RISE        3
#define GPN_TEM_DETAIL_LOW_CLEAN       4
#define GPN_SDMD_MSG_EMD_TEM_STAT_NOTIFY				((GPN_SOCK_OWNER_SDM)|(GPN_SDMD_MT_EMD_OPT)|(GPN_SDMD_MST_EMD_NOTIFY)|(GPN_SDMD_SST_EMD_TEM_STAT_NOTIFY))	/**/
/* msg parameters direction: 
			iIndex	 = tx_Index++
			iMsgType   = GPN_SDMD_MSG_EMD_LED_CONTROL
			iSrcId	 = emd
			iDstId	 = onmd
			iMsgPara1  = stat
			iMsgPara2  = detail
			msgCellLen = 0
*/
#define GPN_SDMD_MSG_EMD_TEM_STAT_NOTIFY_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_SDMD_MSG_EMD_TEM_STAT_NOTIFY))
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_SDMD_MSG_EMD_TEM_STAT_NOTIFY_ACK
			iSrcId	= onmd
			iDstId	= emd
*/

#define GPN_SDMD_MSG_EMD_NOTIFYED_CFG_SAVE				((GPN_SOCK_OWNER_SDM)|(GPN_SDMD_MT_EMD_OPT)|(GPN_SDMD_MST_EMD_NOTIFY)|(GPN_SDMD_SST_EMD_NOTIFYED_CFG_SAVE))	/**/
/* msg parameters direction: 
			iIndex	 = tx_Index++
			iMsgType   = GPN_SDMD_MSG_EMD_NOTIFYED_CFG_SAVE
			iSrcId	 = imi
			iDstId	 = emd
			iMsgPara1  = save mode
			msgCellLen = 0
*/
#define GPN_SDMD_MSG_EMD_NOTIFYED_CFG_SAVE_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_SDMD_MSG_EMD_TEM_STAT_NOTIFY))
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType   = GPN_SDMD_MSG_EMD_NOTIFYED_CFG_SAVE_ACK
			iSrcId	= emd
			iDstId	= imi
*/

UINT32 gpnSockSDMDMsgSoltStatGet(UINT32 slot);
UINT32 gpnSockSDMDMsgSoltOpenNotify(UINT32 slot, UINT32 devType);
UINT32 gpnSockSDMDMsgSoltCloseNotify(UINT32 slot, UINT32 devType);
UINT32 gpnSockSDMDMsgLedControl(UINT32 slot, UINT32 ledType, UINT32 ledId, UINT32 opt);
UINT32 gpnSockSDMDMsgTxTemStatNotif(UINT32 stat, UINT32 detail);
UINT32 gpnSockSDMDMsgTxNotifyEmdCfgSave(UINT32 mode);

#define GPN_NOTIFY_EMD_CFG_SAVE(mode) gpnSockSDMDMsgTxNotifyEmdCfgSave(mode)
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_CFGMGT_MSG_DEF_H_*/


