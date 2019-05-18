/**********************************************************
* file name: gpnSockSysMgtMsgDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-05-30
* function: 
*    define details about communication between gpn_sysmgt modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_SYSMGT_MSG_DEF_H_
#define _GPN_SOCK_SYSMGT_MSG_DEF_H_
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
/************************************************************/
/*this main type include normal comm msg used by gpn_sysmgt */
#define GPN_SYSMGT_MT_XCFG						GPN_SOCK_MSG_MT_CREAT(1)

/****************************************************************************************/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  :   1                           */
/****************************************************************************************/
#define GPN_SYSMGT_MST_PHY_CFG					GPN_SOCK_MSG_MST_CREAT(1)
/****************************************************************************************/
/*define main type 1's main-sub type 1's sub-sub type  (from 1 to 0xFE)  :              */
/****************************************************************************************/
	#define GPN_SYSMGT_SST_DEV_STA_SURE			GPN_SOCK_MSG_SST_CREAT(1)
	#define GPN_SYSMGT_SST_SUB_CFG_RECOVER_OPT	GPN_SOCK_MSG_SST_CREAT(2)
	#define GPN_SYSMGT_SST_SUB_REBOOT_OPT			GPN_SOCK_MSG_SST_CREAT(3)
	#define GPN_SYSMGT_SST_SUB_SAVE_OPT			GPN_SOCK_MSG_SST_CREAT(4)
	#define GPN_SYSMGT_SST_EU_TYPE_NOTIFY			GPN_SOCK_MSG_SST_CREAT(5)
	#define GPN_SYSMGT_SST_DEV_STA_NOTIFY			GPN_SOCK_MSG_SST_CREAT(6)
	#define GPN_SYSMGT_SST_DEV_STA_REQ			GPN_SOCK_MSG_SST_CREAT(7)


/****************************************************************************************/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  :   2                                 */
/****************************************************************************************/
#define GPN_SYSMGT_MST_L2_CFG						GPN_SOCK_MSG_MST_CREAT(2)
/****************************************************************************************/
/*define main type 1's main-sub type 2's sub-sub type  (from 1 to 0xFE)  :                       */
/****************************************************************************************/
	#define GPN_SYSMGT_SST_SUB_L2CFG_NOTIFY			GPN_SOCK_MSG_SST_CREAT(1)



/****************************************************************************************/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  :   3                                 */
/****************************************************************************************/
#define GPN_SYSMGT_MST_XPROCESS_CFG				GPN_SOCK_MSG_MST_CREAT(3)
	#define GPN_SYSMGT_SST_CPX_MASTER_SLAVER			GPN_SOCK_MSG_SST_CREAT(1)
	#define GPN_SYSMGT_SST_CPX_CFG_ASYN_OPT			GPN_SOCK_MSG_SST_CREAT(2)

/****************************************************************************************/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)	:   4                               */
/****************************************************************************************/
#define GPN_SYSMGT_MST_L3_CFG				    GPN_SOCK_MSG_MST_CREAT(4)
	#define GPN_SYSMGT_SST_INBAND_IP_NOTIFY			GPN_SOCK_MSG_SST_CREAT(1)


#define GPN_SYSMGT_MSG_DEV_STA_SURE					((GPN_SOCK_OWNER_SYSMGT)|(GPN_SYSMGT_MT_XCFG)|(GPN_SYSMGT_MST_PHY_CFG)|(GPN_SYSMGT_SST_DEV_STA_SURE))	/**/
/* msg parameters direction: 
			iIndex	   	= tx_Index++
			iMsgType    = GPN_SYSMGT_MSG_DEV_STA_SURE
			iSrcId		= SYSMGT(NMx)
			iDstId		= CFGMGT(NMx)
			iMsgPara1  	= devIndex(slot)
			iMsgPara2  	= device type
			iMsgPara3  	= status(1:active  2:delete  3: )
			msgCellLen 	= 0
*/
#define GPN_SYSMGT_MSG_DEV_STA_SURE_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYSMGT_MSG_DEV_STA_SURE))
/* msg parameters direction:
			iIndex	   	= CMD_Index
			iMsgType   	= GPN_SYSMGT_MSG_DEV_STA_CFG_ACK
			iSrcId	   	= CFGMGT(NMx)
			iDstId	   	= SYSMGT(NMx)
			
*/

#define GPN_SYSMGT_MSG_SUB_CFG_RECOVER_OPT			((GPN_SOCK_OWNER_SYSMGT)|(GPN_SYSMGT_MT_XCFG)|(GPN_SYSMGT_MST_PHY_CFG)|(GPN_SYSMGT_SST_SUB_CFG_RECOVER_OPT))
/* msg parameters direction: 
			iIndex	   	= tx_Index++
			iMsgType    = GPN_SYSMGT_MSG_SUB_CFG_RECOVER_OPT
			iSrcId		= SYSMGT(NMx)
			iDstId		= CFGMGT(NMx)
			iMsgPara1  	= devIndex(slot)
			iMsgPara2  	= device type
			iMsgPara3   = file type(reserve)
			iMsgPara4  	= action(1: recover startup, 2: resv, 3: recover factory, 4: recover user)
			msgCellLen 	= 0
*/
#define GPN_SYSMGT_MSG_SUB_CFG_RECOVER_OPT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYSMGT_MSG_SUB_CFG_RECOVER_OPT))
/* msg parameters direction:
			iIndex	   	= CMD_Index
			iMsgType   	= GPN_SYSMGT_MSG_SUB_CFG_RECOVER_OPT_ACK
			iSrcId	   	= CFGMGT(NMx)
			iDstId	   	= SYSMGT(NMx)
*/

#define GPN_SYSMGT_MSG_SUB_REBOOT_OPT				((GPN_SOCK_OWNER_SYSMGT)|(GPN_SYSMGT_MT_XCFG)|(GPN_SYSMGT_MST_PHY_CFG)|(GPN_SYSMGT_SST_SUB_REBOOT_OPT))
/* msg parameters direction: 
			iIndex	   	= tx_Index++
			iMsgType    = GPN_SYSMGT_MSG_SUB_REBOOT_OPT
			iSrcId		= SYSMGT(NMx)
			iDstId		= CFGMGT(NMx)
			iMsgPara1  	= devIndex(slot)
			iMsgPara2  	= device type
			msgCellLen 	= 0
*/
#define GPN_SYSMGT_MSG_SUB_REBOOT_OPT_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYSMGT_MSG_SUB_REBOOT_OPT))
/* msg parameters direction:
			iIndex	   	= CMD_Index
			iMsgType   	= GPN_SYSMGT_MSG_SUB_REBOOT_OPT_ACK
			iSrcId	   	= CFGMGT(NMx)
			iDstId	   	= SYSMGT(NMx)
*/

#define GPN_SYSMGT_MSG_SUB_SAVE_OPT					((GPN_SOCK_OWNER_SYSMGT)|(GPN_SYSMGT_MT_XCFG)|(GPN_SYSMGT_MST_PHY_CFG)|(GPN_SYSMGT_SST_SUB_SAVE_OPT))
/* msg parameters direction: 
			iIndex     	= tx_Index++
			iMsgType    = GPN_SYSMGT_MSG_SUB_SAVE_OPT
			iSrcId      = SYSMGT(NMx)
			iDstId      = CFGMGT(NMx)
			iMsgPara1  	= devIndex(slot)
			iMsgPara2  	= device type
			iMsgPara3   = file type(reserve)
			iMsgPara4   = action(1: save as startup, 4: save as user)
			msgCellLen 	= 0
*/
#define GPN_SYSMGT_MSG_SUB_SAVE_OPT_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYSMGT_MSG_SUB_SAVE_OPT))
/* msg parameters direction:
			iIndex	   	= CMD_Index
			iMsgType   	= GPN_SYSMGT_MSG_SUB_SAVE_OPT_ACK
			iSrcId	   	= CFGMGT(NMx)
			iDstId	   	= SYSMGT(NMx)
*/

#define GPN_SYSMGT_MSG_DEV_STA_REQ					((GPN_SOCK_OWNER_SYSMGT)|(GPN_SYSMGT_MT_XCFG)|(GPN_SYSMGT_MST_PHY_CFG)|(GPN_SYSMGT_SST_DEV_STA_REQ))
/* msg parameters direction: 
			iIndex     	= tx_Index++
			iMsgType    = GPN_SYSMGT_MSG_DEV_STA_REQ
			iSrcId      = SYSMGT(NMx)
			iDstId      = CFGMGT(NMx)
			iMsgPara1  	= devIndex(slot)
			iMsgPara2  	= device type
			msgCellLen 	= 0
*/


#define GPN_SYSMGT_MSG_SUB_L2CFG_NOTIFY				((GPN_SOCK_OWNER_SYSMGT)|(GPN_SYSMGT_MT_XCFG)|(GPN_SYSMGT_MST_L2_CFG)|(GPN_SYSMGT_SST_SUB_L2CFG_NOTIFY))
/* msg parameters direction: 
			iIndex     	= tx_Index++
			iMsgType    = GPN_SYSMGT_MSG_SUB_L2CFG_NOTIFY
			iSrcId      = SYSMGT(NMx)
			iDstId      = CFGMGT(slot)
			iMsgPara1  	= devIndex(slot)
			iMsgPara2  	= device type
			iMsgPara3   = mac[2]
			iMsgPara4   = mac[4]
			iMsgPara5   = nm81_i_mac[2]
			iMsgPara6   = nm81_i_mac[4]
			iMsgPara7   = vlan_en
			iMsgPara8   = vlan_id
			msgCellLen 	= 0
*/
#define GPN_SYSMGT_MSG_SUB_L2CFG_NOTIFY_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYSMGT_MSG_SUB_L2CFG_NOTIFY))
/* msg parameters direction:
			iIndex	   	= CMD_Index
			iMsgType   	= GPN_SYSMGT_MSG_SUB_L2CFG_NOTIFY_ACK
			iSrcId	   	= CFGMGT(slot)
			iDstId	   	= SYSMGT(NMx)
*/

#define GPN_SYSMGT_MSG_SUB_CPX_MASTER_SLAVER		((GPN_SOCK_OWNER_SYSMGT)|(GPN_SYSMGT_MT_XCFG)|(GPN_SYSMGT_MST_XPROCESS_CFG)|(GPN_SYSMGT_SST_CPX_MASTER_SLAVER))
/* msg parameters direction: 
			iIndex     	= tx_Index++
			iMsgType    = GPN_SYSMGT_MSG_SUB_CPX_MASTER_SLAVER
			iSrcId      = SYSMGT(NMx)
			iDstId      = CFGMGT(slot)
			iMsgPara1  	= devIndex(slot)
			iMsgPara2  	= device type
			iMsgPara3   = master devIndex(slot)
			msgCellLen 	= 0
*/
#define GPN_SYSMGT_MSG_SUB_CPX_MASTER_SLAVER_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYSMGT_MSG_SUB_CPX_MASTER_SLAVER))
/* msg parameters direction:
			iIndex	   	= CMD_Index
			iMsgType   	= GPN_SYSMGT_MSG_SUB_CPX_MASTER_SLAVER_ACK
			iSrcId	   	= CFGMGT(slot)
			iDstId	   	= SYSMGT(NMx)
*/

#define GPN_SYSMGT_MSG_SUB_CPX_CFG_ASYN_OPT			((GPN_SOCK_OWNER_SYSMGT)|(GPN_SYSMGT_MT_XCFG)|(GPN_SYSMGT_MST_XPROCESS_CFG)|(GPN_SYSMGT_SST_CPX_CFG_ASYN_OPT))
/* msg parameters direction: 
			iIndex     	= tx_Index++
			iMsgType    = GPN_SYSMGT_MSG_SUB_CPX_CFG_ASYN_OPT
			iSrcId      = SYSMGT(NMx)
			iDstId      = CFGMGT(NMx)
			iMsgPara1   = master devIndex(slot)
			msgCellLen 	= 0
*/
#define GPN_SYSMGT_MSG_SUB_CPX_CFG_ASYN_OPT_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYSMGT_MSG_SUB_CPX_CFG_ASYN_OPT))
/* msg parameters direction:
			iIndex	   	= CMD_Index
			iMsgType   	= GPN_SYSMGT_MSG_SUB_CPX_CFG_ASYN_OPT_ACK
			iSrcId	   	= CFGMGT(NMx)
			iDstId	   	= SYSMGT(NMx)
*/

#define GPN_SYSMGT_MSG_INBAND_IP_NOTIFY				((GPN_SOCK_OWNER_SYSMGT)|(GPN_SYSMGT_MT_XCFG)|(GPN_SYSMGT_MST_L3_CFG)|(GPN_SYSMGT_SST_INBAND_IP_NOTIFY))
/* msg parameters direction: 
			iIndex         = tx_Index++
			iMsgType    = GPN_SYSMGT_MSG_INBAND_IP_NOTIFY
			iSrcId         = SYSMGT(NMx)
			iDstId         = CFGMGT(slot)
			iMsgPara1   = devIndex(slot)
			iMsgPara2   = device type
			iMsgPara3   = inbandIp
			msgCellLen = 0
*/
#define GPN_SYSMGT_MSG_INBAND_IP_NOTIFY_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_SYSMGT_MSG_INBAND_IP_NOTIFY))
/* msg parameters direction:
			iIndex	   	= CMD_Index
			iMsgType   	= GPN_SYSMGT_MSG_INBAND_IP_NOTIFY_ACK
			iSrcId	   	= CFGMGT(slot)
			iDstId	   	= SYSMGT(NMx)
*/

extern UINT32 gpnSockSysMgtMsgTxCardSure2CfgMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT32 sta);
extern UINT32 gpnSockSysMgtMsgTxCardCfgRecover2CfgMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT32 file_type, UINT32 act);
extern UINT32 gpnSockSysMgtMsgTxCardReboot2CfgMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type);
extern UINT32 gpnSockSysMgtMsgTxCardSave2CfgMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT32 file_type, UINT32 act);
extern UINT32 gpnSockSysMgtMsgTxCardStatReqMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type);

extern UINT32 gpnSockSysMgtMsgTxCardL2CfgMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT8 * mac, UINT8 * nm_mac, UINT32 vlan_en, UINT16 vlan_id);

extern UINT32 gpnSockSysMgtMsgTxCPXMasterSlaverMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT32 master_slot);
extern UINT32 gpnSockSysMgtMsgTxCPXCfgSynMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 action);
extern UINT32 gpnSockSysMgtMsgTxCardL3InbandIpCfgMgt(stSockFdSet *pstFdSet, UINT32 slot, UINT32 dev_type, UINT32 inbandIp);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_SYSMGT_MSG_DEF_H_*/


