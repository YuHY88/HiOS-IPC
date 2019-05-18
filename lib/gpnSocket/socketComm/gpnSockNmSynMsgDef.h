/**********************************************************
* file name: gpnSockNmSynMsgDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-12-31
* function: 
*    define details about communication between gpn_syn modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_NMSYN_MSG_DEF_H_
#define _GPN_SOCK_NMSYN_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnSockMsgDef.h"
#include "lib/gpnSocket/socketComm/gpnSelectOpt.h"

#include "lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h"

/*==========================================================*/
/*==========================================================*/
/*define main type  (from 1 to 0x7E)  : 1                                                                                   */
/*******************************************************************************/
/*this main type include normal comm msg used by gpn_syn                                                       */
#define GPN_NMSYN_MT_CFGSYN						GPN_SOCK_MSG_MT_CREAT(1)

/****************************************************************************************/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)	: 1 									     */
/****************************************************************************************/
#define GPN_NMSYN_MST_PER_INFO_GET				GPN_SOCK_MSG_MST_CREAT(1)
/****************************************************************************************/
/*define main type 1's main-sub type 2's sub-sub type  (from 1 to 0xFE)  :																				*/
/****************************************************************************************/
	#define GPN_NMSYN_SST_NM_STRATUP_DOWN_REQ		GPN_SOCK_MSG_SST_CREAT(1)
	#define GPN_NMSYN_SST_NM_LOCAL_MS_REQ			GPN_SOCK_MSG_SST_CREAT(2)
	#define GPN_NMSYN_SST_NM_PARTNER_MS_REQ			GPN_SOCK_MSG_SST_CREAT(3)


#define GPN_NMSYN_MSG_NM_STRATUP_DOWN_REQ		((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_CFGSYN)|(GPN_NMSYN_MST_PER_INFO_GET)|(GPN_NMSYN_SST_NM_STRATUP_DOWN_REQ))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_NM_STRATUP_DOWN_REQ
			iSrcId        = nmSyn(sub)
			iDstId        = sysmgt(nm)
			iMsgPara1  = devIndex(slot)
			iMsgPara2  = device type
			iMsgPara3  = reserve;
			msgCellLen= 0
*/	

#define GPN_NMSYN_MSG_NM_LOCAL_MS_REQ			((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_CFGSYN)|(GPN_NMSYN_MST_PER_INFO_GET)|(GPN_NMSYN_SST_NM_LOCAL_MS_REQ))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_NM_LOCAL_MS_REQ
			iSrcId        = nmSyn(NMx) or ...
			iDstId        = emd(NMx)
			iMsgPara1  = reserve;
			msgCellLen= 0
*/
#define GPN_NMSYN_MSG_NM_LOCAL_MS_REQ_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_NM_LOCAL_MS_REQ))
/* msg parameters direction:   
			iIndex	= CMD index
			iMsgType	= GPN_NMSYN_MSG_NM_LOCAL_MS_REQ_ACK
			iSrcId        = emd(NMx)
			iDstId        = nmSyn(NMx)
*/
#define GPN_NMSYN_MSG_NM_LOCAL_MS_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_NMSYN_MSG_NM_LOCAL_MS_REQ))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_NM_LOCAL_MS_RSP
			iSrcId        = emd(NMx)
			iDstId        = nmSyn(NMx)
			iMsgPara1  = local mss;
			iMsgPara2  = is both master and slave in;
			msgCellLen= 0
*/
#define GPN_NMSYN_MSG_NM_LOCAL_MS_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_NM_LOCAL_MS_RSP))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_NM_LOCAL_MS_REQ
			iSrcId        = nmSyn(NMx)
			iDstId        = emd(NMx)
			iMsgPara1  = reserve;
			msgCellLen= 0
*/


#define GPN_NMSYN_MSG_NM_PARTNER_MS_REQ			((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_CFGSYN)|(GPN_NMSYN_MST_PER_INFO_GET)|(GPN_NMSYN_SST_NM_PARTNER_MS_REQ))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_NM_PARTNER_MS_REQ
			iSrcId        = nmSyn(NMx)
			iDstId        = nmSyn(NMy)
			iMsgPara1  = NMx(Partner) sms;
			iMsgPara2  = NMx(Partner) ha out;
			iMsgPara3  = NMx(Partner) mssm l_in;
			iMsgPara4  = NMx(Partner) mssm p_in;
			iMsgPara5  = NMx(Partner) mssm delay;
			iMsgPara6  = NMx(Partner) mssm in_s;
			msgCellLen= 0
*/
#define GPN_NMSYN_MSG_NM_PARTNER_MS_REQ_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_NM_PARTNER_MS_REQ))
/* msg parameters direction:   
			iIndex	= CMD index
			iMsgType	= GPN_NMSYN_MSG_NM_PARTNER_MS_REQ_ACK
			iSrcId        = nmSyn(NMY)
			iDstId        = nmSyn(NMX)
*/
#define GPN_NMSYN_MSG_NM_PARTNER_MS_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_NMSYN_MSG_NM_PARTNER_MS_REQ))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_NM_PARTNER_MS_RSP
			iSrcId        = nmSyn(NMY)
			iDstId        = nmSyn(NMX)
			iMsgPara1  = NMx(Partner) sms;
			iMsgPara2  = NMx(Partner) ha out;
			iMsgPara3  = NMx(Partner) mssm l_in;
			iMsgPara4  = NMx(Partner) mssm p_in;
			iMsgPara5  = NMx(Partner) mssm delay;
			iMsgPara6  = NMx(Partner) mssm in_s;
			msgCellLen= 0
*/
#define GPN_NMSYN_MSG_NM_PARTNER_MS_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_NM_PARTNER_MS_RSP))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_NM_PARTNER_MS_RSP_ACK
			iSrcId        = nmSyn(NMx)
			iDstId        = nmSyn(NMy)
			iMsgPara1  = reserve;
			msgCellLen= 0
*/

UINT32 gpnSockNmSynMsgTxProtocolMacReqToNmaSysMgt(UINT32 slot);

UINT32 gpnSockNmSynMsgTxLocalMSSGet(UINT32 dstRole);
UINT32 gpnSockNmSynMsgTxLocalMSSSend(UINT32 dstRole, UINT32 l_mss, UINT32 is_both);
UINT32 gpnSockNmSynMsgTxPartnerMSSGet(UINT32 l_sms, UINT32 l_mid_ha, UINT32 l_in, UINT32 p_in, UINT32 delay, UINT32 in_s);
UINT32 gpnSockNmSynMsgTxPartnerMSSSend(UINT32 l_sms, UINT32 l_mid_ha, UINT32 l_in, UINT32 p_in, UINT32 delay, UINT32 in_s);

/*==========================================================*/
/*==========================================================*/
/*define main type  (from 1 to 0x7E)  : 2                                                                                   */
/*******************************************************************************/
/*this main type include normal comm notify msg used by gpn_xxx tranfer by gpn_nmsyn              */
#define GPN_NMSYN_MT_NM_NOTIFY					GPN_SOCK_MSG_MT_CREAT(2)

/****************************************************************************************/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)	: 1 									     */
/****************************************************************************************/
#define GPN_NMSYN_MST_PROCESS_COMM				GPN_SOCK_MSG_MST_CREAT(1)
/****************************************************************************************/
/*define main type 1's main-sub type 2's sub-sub type  (from 1 to 0xFE)  :						       */
/****************************************************************************************/
	#define GPN_NMSYN_SST_NOTIFY_BETWEEN_NM				GPN_SOCK_MSG_SST_CREAT(1)	
	#define GPN_NMSYN_SST_LOCAL_NOTIFY_FILE_SD_REG		GPN_SOCK_MSG_SST_CREAT(2)
	#define GPN_NMSYN_SST_LOCAL_NOTIFY_FILE_SD_CHANG	GPN_SOCK_MSG_SST_CREAT(3)
	#define GPN_NMSYN_SST_LOCAL_NOTIFY_DATA_SD_REG		GPN_SOCK_MSG_SST_CREAT(4)
	#define GPN_NMSYN_SST_LOCAL_NOTIFY_DATA_SD_CHANG	GPN_SOCK_MSG_SST_CREAT(5)
	#define GPN_NMSYN_SST_LOCAL_NOTIFY_DATA_SD_DEL  	GPN_SOCK_MSG_SST_CREAT(6)
	#define GPN_NMSYN_SST_LOCAL_NOTIFY_DATA_SD_GET		GPN_SOCK_MSG_SST_CREAT(7)

	#define GPN_NMSYN_SST_PRE_SYN_LEAF_COMP_REQ         GPN_SOCK_MSG_SST_CREAT(8)
	#define GPN_NMSYN_SST_REAL_SYN_SEND                 GPN_SOCK_MSG_SST_CREAT(9)
	#define GPN_NMSYN_SST_PATCH_SYN_START_REQ           GPN_SOCK_MSG_SST_CREAT(10)
	#define GPN_NMSYN_SST_PATCH_SYN_XNODE_COMP_REQ      GPN_SOCK_MSG_SST_CREAT(11)
	#define GPN_NMSYN_SST_PATCH_SYN_LEAF_COMP_REQ       GPN_SOCK_MSG_SST_CREAT(12)

	#define GPN_NMSYN_SST_SYN_NOTIFY_DATA_SD_CHANG		GPN_SOCK_MSG_SST_CREAT(13)
	#define GPN_NMSYN_SST_SYN_NOTIFY_DATA_SD_DEL  		GPN_SOCK_MSG_SST_CREAT(14)

	#define GPN_NMSYN_SST_SYN_AUTO_PATCH_SYN_REQ		GPN_SOCK_MSG_SST_CREAT(15)
	#define GPN_NMSYN_SST_SYN_AUTO_PATCH_SYN_RSP  		GPN_SOCK_MSG_SST_CREAT(16)

#define GPN_NMSYN_PARA_MAX_NOTIFY_PAY			1024
#define GPN_NMSYN_PARA_MAX_NOTIFY_LEN			((GPN_SOCK_MSG_HEAD_BLEN) + (GPN_NMSYN_PARA_MAX_NOTIFY_PAY))

#define GPN_NMSYN_MSG_NOTIFY_BETWEEN_NM			((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_NOTIFY_BETWEEN_NM))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_NOTIFY_BETWEEN_NM
			iSrcId        = xxx(nm A x module)
			iDstId        = xxx(nm B x module)
			msgCellLen= sizeof(payload)
		        payload     = notify messeg
*/	
#define GPN_NMSYN_MSG_NOTIFY_BETWEEN_NM_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_NOTIFY_BETWEEN_NM))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_NOTIFY_BETWEEN_NM_ACK
			iSrcId	= xxx(nm B x module)
			iDstId	= xxx(nm A x module)
*/

#define GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_REG		((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_LOCAL_NOTIFY_FILE_SD_REG))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_REG
			iSrcId        = xxx(nm A x module)
			iDstId        = xxx(nm B x module)
			iMsgPara1  = who base syn;
			iMsgPara2  = syn obj type;
			iMsgPara3  = file_type;
			msgCellLen= 0
*/	
#define GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_REG_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_REG))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_REG_ACK
			iSrcId	= xxx(nm B x module)
			iDstId	= xxx(nm A x module)
*/

#define GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_CHANG		((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_LOCAL_NOTIFY_FILE_SD_CHANG))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_CHANG
			iSrcId        = xxx(nm A x module)
			iDstId        = xxx(nm B x module)
			iMsgPara1  = who base syn;
			iMsgPara2  = syn obj type;
			iMsgPara3  = reserve;
			msgCellLen= 0
*/	
#define GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_CHANG_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_CHANG))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_CHANG_ACK
			iSrcId	= xxx(nm B x module)
			iDstId	= xxx(nm A x module)
*/

#define GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_REG			((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_LOCAL_NOTIFY_DATA_SD_REG))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_REG
			iSrcId        = x module
			iDstId        = gpn_syn module
			iMsgPara1  = who base syn;
			iMsgPara2  = obj_type;
			iMsgPara3  = is index valid
			iMsgPara4  = dev index;
			iMsgPara5  = protIndex;
			iMsgPara6  = protIndex2;
			iMsgPara7  = protIndex3;
			iMsgPara8  = protIndex4;
			iMsgPara9  = protIndex5;
			msgCellLen= sizeof(syn_info)
			payload = syn_info
*/	
#define GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_REG_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_REG))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_REG_ACK
			iSrcId        = gpn_syn module
			iDstId        = x module

*/

#define GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_CHANG		((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_LOCAL_NOTIFY_DATA_SD_CHANG))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_CHANG
			iSrcId        = x module
			iDstId        = gpn_syn module
			iMsgPara1  = who base syn;
			iMsgPara2  = obj_type;
			iMsgPara3  = is index valid
			iMsgPara4  = dev index;
			iMsgPara5  = protIndex;
			iMsgPara6  = protIndex2;
			iMsgPara7  = protIndex3;
			iMsgPara8  = protIndex4;
			iMsgPara9  = protIndex5;
			msgCellLen= sizeof(syn_info)
			payload = syn_info
*/	
#define GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_CHANG_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_CHANG))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_CHANG_ACK
			iSrcId        = gpn_syn module
			iDstId        = x module
*/

#define GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_DEL			((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_LOCAL_NOTIFY_DATA_SD_DEL))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_DEL
			iSrcId        = x module
			iDstId        = gpn_syn module
			iMsgPara1  = who base syn;
			iMsgPara2  = obj_type;
			iMsgPara3  = is index valid
			iMsgPara4  = dev index;
			iMsgPara5  = protIndex;
			iMsgPara6  = protIndex2;
			iMsgPara7  = protIndex3;
			iMsgPara8  = protIndex4;
			iMsgPara9  = protIndex5;
			msgCellLen= 0
*/	
#define GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_DEL_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_DEL))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_DEL_ACK
			iSrcId        = gpn_syn module
			iDstId        = x module
*/

#define GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_GET			((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_LOCAL_NOTIFY_DATA_SD_GET))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_GET
			iSrcId        = x module
			iDstId        = gpn_syn module
			iMsgPara1  = who base syn;
			iMsgPara2  = obj_type;
			iMsgPara3  = is index valid
			iMsgPara4  = dev index;
			iMsgPara5  = protIndex;
			iMsgPara6  = protIndex2;
			iMsgPara7  = protIndex3;
			iMsgPara8  = protIndex4;
			iMsgPara9  = protIndex5;
			msgCellLen= 0
*/
#define GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_GET))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_GET_ACK
			iSrcId	= gpn_syn module
			iDstId	= x module
*/
#define GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_GET))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_GET_RSP
			iSrcId        = gpn_syn module
			iDstId        = x module
			iMsgPara1  = who base syn;
			iMsgPara2  = obj_type;
			iMsgPara3  = is index valid
			iMsgPara4  = dev index;
			iMsgPara5  = protIndex;
			iMsgPara6  = protIndex2;
			iMsgPara7  = protIndex3;
			iMsgPara8  = protIndex4;
			iMsgPara9  = protIndex5;
			msgCellLen= sizeof(syn_info)
			payload = syn_info
*/
#define GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_GET_RSP))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_DATA_SD_GET_RSP_ACK
			iSrcId        = x module
			iDstId        = gpn_syn module
*/

#define GPN_NMSYN_MSG_SYN_NT_DATA_SD_CHANG			((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_SYN_NOTIFY_DATA_SD_CHANG))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_SYN_NT_DATA_SD_CHANG
			iSrcId        = x module
			iDstId        = gpn_syn module
			iMsgPara1  = who base syn;
			iMsgPara2  = obj_type;
			iMsgPara3  = is index valid
			iMsgPara4  = dev index;
			iMsgPara5  = protIndex;
			iMsgPara6  = protIndex2;
			iMsgPara7  = protIndex3;
			iMsgPara8  = protIndex4;
			iMsgPara9  = protIndex5;
			msgCellLen= sizeof(syn_info)
			payload = syn_info
*/	
#define GPN_NMSYN_MSG_SYN_NT_DATA_SD_CHANG_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_SYN_NT_DATA_SD_CHANG))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_SYN_NT_DATA_SD_CHANG_ACK
			iSrcId        = gpn_syn module
			iDstId        = x module

*/

#define GPN_NMSYN_MSG_SYN_NT_DATA_SD_DEL			((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_SYN_NOTIFY_DATA_SD_DEL))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_SYN_NT_DATA_SD_DEL
			iSrcId        = x module
			iDstId        = gpn_syn module
			iMsgPara1  = who base syn;
			iMsgPara2  = obj_type;
			iMsgPara3  = is index valid
			iMsgPara4  = dev index;
			iMsgPara5  = protIndex;
			iMsgPara6  = protIndex2;
			iMsgPara7  = protIndex3;
			iMsgPara8  = protIndex4;
			iMsgPara9  = protIndex5;
			msgCellLen= sizeof(syn_info)
			payload = syn_info
*/	
#define GPN_NMSYN_MSG_SYN_NT_DATA_SD_CHANG_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_SYN_NT_DATA_SD_CHANG))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_SYN_NT_DATA_SD_CHANG_ACK
			iSrcId        = gpn_syn module
			iDstId        = x module

*/

/* same with GPN_MD5_LENTH */
#define GPN_NMSYN_PARA_M5D_LENTH      32
#define GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_REQ			((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_PRE_SYN_LEAF_COMP_REQ))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_REQ
			iSrcId	= xxx(nm A x module)
			iDstId	= xxx(nm B x module)
			iMsgPara1  = wb_type;
			iMsgPara2  = obj_type;
			iMsgPara3  = is index valid
			iMsgPara4  = dev index;
			iMsgPara5  = protIndex;
			iMsgPara6  = protIndex2;
			iMsgPara7  = protIndex3;
			iMsgPara8  = protIndex4;
			iMsgPara9  = protIndex5;
			msgCellLen= 0
*/	
#define GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_REQ_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_REQ))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_REQ_ACK
			iSrcId	= xxx(nm B x module)
			iDstId	= xxx(nm A x module)
*/
#define GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_REQ))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_RSP
			iSrcId	= xxx(nm B x module)
			iDstId	= xxx(nm A x module)
			iMsgPara1  = wb_type;
			iMsgPara2  = obj_type;
			iMsgPara3  = is index valid
			iMsgPara4  = dev index;
			iMsgPara5  = protIndex;
			iMsgPara6  = protIndex2;
			iMsgPara7  = protIndex3;
			iMsgPara8  = protIndex4;
			iMsgPara9  = protIndex5;
			msgCellLen= sizeof(md5) + sizeof(syn_info)
			payload = md5 + syn_info
*/	
#define GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_RSP))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PRE_SYN_LEAF_COMP_RSP_ACK
			iSrcId	= xxx(nm A x module)
			iDstId	= xxx(nm B x module)
*/

#define GPN_NMSYN_MSG_REAL_SYN_SEND					((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_REAL_SYN_SEND))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_REAL_SYN_SEND
			iSrcId        = xxx(nm A x module)
			iDstId        = xxx(nm B x module)
			iMsgPara1  = who base syn;
			iMsgPara2  = syn obj type;
			iMsgPara3  = is index valid;
			iMsgPara4  = dev index;
			iMsgPara5  = port index;
			iMsgPara6  = port index2;
			iMsgPara7  = port index3;
			iMsgPara8  = port index4;
			iMsgPara9  = port index5;
			msgCellLen= sizeof(md5) + sizeof(syn_info)
			payload = md5 + syn_info
*/	
#define GPN_NMSYN_MSG_REAL_SYN_SEND_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_REAL_SYN_SEND))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_LOCAL_NT_FILE_SD_CHANG_ACK
			iSrcId	= xxx(nm B x module)
			iDstId	= xxx(nm A x module)
*/

#define GPN_NMSYN_MSG_PATCH_SYN_START_REQ			((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_PATCH_SYN_START_REQ))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PATCH_SYN_START_REQ
			iSrcId        = xxx
			iDstId        = nm module
			iMsgPara1  = ask role;
			msgCellLen= 0
*/	
#define GPN_NMSYN_MSG_PATCH_SYN_START_REQ_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_PATCH_SYN_START_REQ))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PATCH_SYN_START_REQ_ACK
			iSrcId	= nm module
			iDstId	= xxx
*/
#define GPN_NMSYN_MSG_PATCH_SYN_START_RSP			((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_NMSYN_MSG_PATCH_SYN_START_REQ))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PATCH_SYN_START_RSP
			iSrcId	= xxx
			iDstId	= nm module
			iMsgPara1  = 1 : ok ; 2 not ok
			msgCellLen= 0
*/	
#define GPN_NMSYN_MSG_PATCH_SYN_START_RSP_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_PATCH_SYN_START_RSP))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PATCH_SYN_START_RSP_ACK
			iSrcId	= nm module
			iDstId	= xxx
*/


#define GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_REQ		((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_PATCH_SYN_XNODE_COMP_REQ))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_REQ
			iSrcId	= xxx(nm A x module)
			iDstId	= xxx(nm B x module)
			iMsgPara1  = xl_type;
			iMsgPara2  = wb_type;
			iMsgPara3  = obj_type;
			iMsgPara4  = hash_key;
			msgCellLen= 0
*/	
#define GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_REQ_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_REQ))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_REQ_ACK
			iSrcId	= xxx(nm B x module)
			iDstId	= xxx(nm A x module)
*/
#define GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_REQ))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_RSP
			iSrcId	= xxx(nm B x module)
			iDstId	= xxx(nm A x module)
			iMsgPara1  = xl_type;
			iMsgPara2  = wb_type;
			iMsgPara3  = obj_type;
			iMsgPara4  = hash_key;
			msgCellLen= sizeof(md5)
			payload = md5
*/	
#define GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_RSP))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PATCH_SYN_XNODE_COMP_RSP_ACK
			iSrcId	= xxx(nm A x module)
			iDstId	= xxx(nm B x module)
*/

#define GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_REQ		((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_PATCH_SYN_LEAF_COMP_REQ))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_REQ
			iSrcId	= xxx(nm A x module)
			iDstId	= xxx(nm B x module)
			iMsgPara1  = wb_type;
			iMsgPara2  = obj_type;
			iMsgPara3  = hash_key;
			iMsgPara4  = leaf_order(1.2.3...) or 0 means not have more leaf;
			msgCellLen= 0
*/	
#define GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_REQ_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_REQ))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_REQ_ACK
			iSrcId	= xxx(nm B x module)
			iDstId	= xxx(nm A x module)
*/
#define GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_REQ))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_RSP
			iSrcId	= xxx(nm B x module)
			iDstId	= xxx(nm A x module)
			iMsgPara1  = who base syn;
			iMsgPara2  = syn obj type;
			iMsgPara3  = hash key;
			iMsgPara4  = next_leaf_order(1.2.3...) or 0 means not have more leaf;
			iMsgPara5  = is index valid;
			iMsgPara6  = dev index;
			iMsgPara7  = port index;
			iMsgPara8  = port index2;
			iMsgPara9  = port index3;
			iMsgParaA  = port index4;
			iMsgParaB  = port index5;
			msgCellLen= sizeof(md5) + sizeof(syn_info)
			payload = md5 + syn_info
*/	
#define GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_RSP))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_PATCH_SYN_LEAF_COMP_RSP_ACK
			iSrcId	= xxx(nm A x module)
			iDstId	= xxx(nm B x module)
*/

#define GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_REQ		((GPN_SOCK_OWNER_NMSYN)|(GPN_NMSYN_MT_NM_NOTIFY)|(GPN_NMSYN_MST_PROCESS_COMM)|(GPN_NMSYN_SST_SYN_AUTO_PATCH_SYN_REQ))
/* msg parameters direction: 
			iIndex       = tx_Index++
			iMsgType   = GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_REQ
			iSrcId	= xxx(nm A x module)
			iDstId	= xxx(nm B x module)
			iMsgPara1  = top md5
			...
			iMsgPara8  = top md5
			msgCellLen= 0
*/	
#define GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_REQ_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_REQ))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_REQ_ACK
			iSrcId	= xxx(nm B x module)
			iDstId	= xxx(nm A x module)
*/
#define GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_REQ))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_RSP
			iSrcId	= xxx(nm B x module)
			iDstId	= xxx(nm A x module)
			msgCellLen= 0
*/	
#define GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_RSP))
/* msg parameters direction: 
			iIndex	= tx_Index++
			iMsgType   = GPN_NMSYN_MSG_SYN_AUTO_PATCH_SYN_RSP_ACK
			iSrcId	= xxx(nm A x module)
			iDstId	= xxx(nm B x module)
*/

UINT32 gpnSockNmSynMsgTxNotifyBetweenNm(void *notify, UINT32 len, UINT32 sendRole);
UINT32 gpnSockNmSynMsgTxCoproNotifyBetweenNm(void *notify, UINT32 len);
UINT32 gpnSockNmSynMsgTxEmdNotifyBetweenNm(void *notify, UINT32 len);
UINT32 gpnSockNmSynMsgTxXloadNotifyBetweenNm(void *notify, UINT32 len);
UINT32 gpnSockNmSynMsgTxImiNotifyBetweenNm(void *notify, UINT32 len);
UINT32 gpnSockNmSynMsgTxFileTypeSynDataChange(UINT32 wb_type, UINT32 syn_obj_type);
UINT32 gpnSockNmSynMsgTxFileTypeSynDataRegister(UINT32 wb_type, UINT32 syn_obj_type, UINT32 file_type);
UINT32 gpnSockNmSynMsgTxDTSynDataRegister(UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index, void *syn_info, UINT32 len);
UINT32 gpnSockNmSynMsgTxDTSynDataChange(UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index, void *syn_info, UINT32 len);
UINT32 gpnSockNmSynMsgTxDTSynDataDelete(UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index);
UINT32 gpnSockNmSynMsgTxDTSynDataInitGet(UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index);
UINT32 gpnSockNmSynMsgTxSynNTDataChange(UINT32 dst_modu, UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index, void *syn_info, UINT32 len);
UINT32 gpnSockNmSynMsgTxSynNTDataDelete(UINT32 dst_modu, UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index);
UINT32 gpnSockNmSynMsgTxPreSynLeafCompReq(UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index);
UINT32 gpnSockNmSynMsgTxPreSynLeafCompRsp(UINT32 wb_type, UINT32 obj_type, objLogicDesc *p_index, char *md5, void *syn_info, UINT32 len);
UINT32 gpnSockNmSynMsgTxRealSynSend(UINT32 wb_type, UINT32 obj_type, char *md5, objLogicDesc *p_index, void *syn_info, UINT32 len);
UINT32 gpnSockNmSynMsgTxPatchSynStartReq(UINT32 ask_role);
UINT32 gpnSockNmSynMsgTxPatchSynStartRsp(UINT32 ask_src, UINT32 ps_stat);
UINT32 gpnSockNmSynMsgTxPatSynXNodeCompReq(UINT32 xl_type,UINT32 wb_type, UINT32 obj_type, UINT32 hash_key);
UINT32 gpnSockNmSynMsgTxPatSynXNodeCompRsp(UINT32 xl_type,UINT32 wb_type, UINT32 obj_type, UINT32 hash_key, char *md5);
UINT32 gpnSockNmSynMsgTxPatSynLeafCompReq(UINT32 wb_type, UINT32 obj_type, UINT32 hash_key, UINT32 leaf_order);
UINT32 gpnSockNmSynMsgTxPatSynLeafCompRsp(UINT32 wb_type, UINT32 obj_type, UINT32 hash_key, objLogicDesc *p_index, char *md5, void *syn_info, UINT32 len, UINT32 leaf_order);
UINT32 gpnSockNmSynMsgTxAutoPatSynReq(char *top_md5);
UINT32 gpnSockNmSynMsgTxAutoPatSynRsp(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_NMSYN_MSG_DEF_H_*/


