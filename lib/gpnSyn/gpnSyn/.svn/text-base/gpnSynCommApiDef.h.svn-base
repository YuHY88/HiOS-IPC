/**********************************************************
* file name: gpnSynCommApiDef.h
* Copyright: 
	 Copyright 2016 huahuan.
* author: 
*    huahuan liuyf 2016-04-27
* function: 
*    define gpn_syn about syn and ha 
		func api use by gpn_syn and other module 
* modify:
*
***********************************************************/
#ifndef _GPN_SYN_COMM_API_DEF_H_
#define _GPN_SYN_COMM_API_DEF_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnSockNmSynMsgDef.h"
#include "socketComm/gpnSockInitdMsgDef.h"
#include "socketComm/gpnSockFTMsgDef.h"

#include "socketComm/gpnSockCommRoleMan.h"

#include "gpnSyn/gpnSynGenTypeDef.h"

/*
 * ==========================================================
 * Send Start Up Step1 Ok to GPN_INIT 
 * ==========================================================
 */
#define GPN_SYN_STEP1_NOTITFY_OK gpnSockInitdMsgSUStepXEndNotify(GPN_INIT_MSG_PARA_SUS1, GEN_SYS_OK)
#define GPN_SYN_IS_CFG_DOWN_ASK gpnSockInitdMsgTxIsCfgDownAsk()

/*
 * ==========================================================
 * Get/Send Local Master & Slave State (MSS default in EMD)
 * ==========================================================
 */
#define GPN_SYN_MSS_L_GET gpnSockNmSynMsgTxLocalMSSGet(GPN_SOCK_ROLE_SDM)
#define GPN_SYN_MSS_L_SEND(l_mss, is_both) gpnSockNmSynMsgTxLocalMSSSend(GPN_SOCK_ROLE_NM_SYN, l_mss, is_both)

/*
 * ==========================================================
 * Get/Send partner Master & Slave State (MSS default in gpn_syn)
 * ==========================================================
 */
#define GPN_SYN_MSS_P_GET(l_sms, l_mid_ha, l_in, p_in, delay, in_s) gpnSockNmSynMsgTxPartnerMSSGet(l_sms, l_mid_ha, l_in, p_in, delay, in_s)
#define GPN_SYN_MSS_P_SEND(l_sms, l_mid_ha, l_in, p_in, delay, in_s) gpnSockNmSynMsgTxPartnerMSSSend(l_sms, l_mid_ha, l_in, p_in, delay, in_s)

/*
 * ==========================================================
 *Send/Rcv Pre/Real/Batch Syn Msg
 * ==========================================================
 */
#define GPN_SYN_RS_SEND(wb_type, obj_type, md5, index, syn_info, len) gpnSockNmSynMsgTxRealSynSend(wb_type, obj_type, md5, (objLogicDesc *)index, syn_info, len)



/*
 * ==========================================================
 * Send IMI File Type Syn Data Change to GPN_SYN
 * ==========================================================
 */
#if defined(GPN_CFLAGS_DEV_H20PN2000)
#define GPN_SYN_ZEBOS_STARTUP_CFG_CHG_SEND gpnSockNmSynMsgTxFileTypeSynDataChange(GPN_SYN_SDATA_WBASE_MASTER, GPN_SYN_SDATA_OBJ_IMI_STARTUP_CFG_FILE)
#define GPN_SYN_ZEBOS_PRODUCT_CFG_CHG_SEND gpnSockNmSynMsgTxFileTypeSynDataChange(GPN_SYN_SDATA_WBASE_MASTER, GPN_SYN_SDATA_OBJ_IMI_PRODUCT_CFG_FILE)
#else
#define GPN_SYN_ZEBOS_STARTUP_CFG_CHG_SEND
#define GPN_SYN_ZEBOS_PRODUCT_CFG_CHG_SEND
#endif
/*
 * ==========================================================
 * Send EMD File Type Syn Data Change to GPN_SYN
 * ==========================================================
 */
#if defined(GPN_CFLAGS_DEV_H20PN2000)
#define GPN_SYN_EMD_STARTUP_CFG_CHG_SEND gpnSockNmSynMsgTxFileTypeSynDataChange(GPN_SYN_SDATA_WBASE_MASTER, GPN_SYN_SDATA_OBJ_EMD_STARTUP_CFG_FILE)
#define GPN_SYN_EMD_PRODUCT_CFG_CHG_SEND gpnSockNmSynMsgTxFileTypeSynDataChange(GPN_SYN_SDATA_WBASE_MASTER, GPN_SYN_SDATA_OBJ_EMD_PRODUCT_CFG_FILE)
#define GPN_SYN_EMD_ADDRESS_CFG_CHG_SEND gpnSockNmSynMsgTxFileTypeSynDataChange(GPN_SYN_SDATA_WBASE_MASTER, GPN_SYN_SDATA_OBJ_EMD_ADDRESS_CFG_FILE)
#else
#define GPN_SYN_EMD_STARTUP_CFG_CHG_SEND
#define GPN_SYN_EMD_PRODUCT_CFG_CHG_SEND
#define GPN_SYN_EMD_ADDRESS_CFG_CHG_SEND
#endif

/*
 * ==========================================================
 * Send File Type Syn Data Register to GPN_SYN
 * ==========================================================
 */
#if defined(GPN_CFLAGS_DEV_H20PN2000)
#define GPN_SYN_ZEBOS_STARTUP_CFG_REGISTER gpnSockNmSynMsgTxFileTypeSynDataRegister(GPN_SYN_SDATA_WBASE_MASTER, GPN_SYN_SDATA_OBJ_IMI_STARTUP_CFG_FILE, 1)
#define GPN_SYN_ZEBOS_PRODUCT_CFG_REGISTER gpnSockNmSynMsgTxFileTypeSynDataRegister(GPN_SYN_SDATA_WBASE_MASTER, GPN_SYN_SDATA_OBJ_IMI_PRODUCT_CFG_FILE, 1)
#define GPN_SYN_EMD_STARTUP_CFG_REGISTER gpnSockNmSynMsgTxFileTypeSynDataRegister(GPN_SYN_SDATA_WBASE_MASTER, GPN_SYN_SDATA_OBJ_EMD_STARTUP_CFG_FILE, 1)
#define GPN_SYN_EMD_PRODUCT_CFG_REGISTER gpnSockNmSynMsgTxFileTypeSynDataRegister(GPN_SYN_SDATA_WBASE_MASTER, GPN_SYN_SDATA_OBJ_EMD_PRODUCT_CFG_FILE, 1)
#define GPN_SYN_EMD_ADDRESS_CFG_REGISTER gpnSockNmSynMsgTxFileTypeSynDataRegister(GPN_SYN_SDATA_WBASE_MASTER, GPN_SYN_SDATA_OBJ_EMD_ADDRESS_CFG_FILE, 1)
#else
#define GPN_SYN_ZEBOS_STARTUP_CFG_REGISTER
#define GPN_SYN_ZEBOS_PRODUCT_CFG_REGISTER
#define GPN_SYN_EMD_STARTUP_CFG_REGISTER
#define GPN_SYN_EMD_PRODUCT_CFG_REGISTER
#define GPN_SYN_EMD_ADDRESS_CFG_REGISTER
#endif

/*
 * ==========================================================
 * Send Local Master & Slave State to GPN_SYN
 * ==========================================================
 */
#if defined(GPN_CFLAGS_DEV_H20PN2000)
#define GPN_SYN_L_MSS_SEND(l_mss, is_both) GPN_SYN_MSS_L_SEND(l_mss, is_both)
#else
#define GPN_SYN_L_MSS_SEND(l_mss, is_both)
#endif

/*
 * ==========================================================
 * Ask Local Master PRE-Syn
 * ==========================================================
 */
#define GPN_SYN_PRE_SYN_LEAF_COMP_REQ(wb_type, obj_type, p_index)  gpnSockNmSynMsgTxPreSynLeafCompReq(wb_type, obj_type, p_index);
#define GPN_SYN_PRE_SYN_LEAF_COMP_RSP(wb_type, obj_type, p_index, md5, syn_info, len)  gpnSockNmSynMsgTxPreSynLeafCompRsp(wb_type, obj_type, p_index, md5, syn_info, len);


/*
 * ==========================================================
 * Ask Local Master Patch-Syn
 * ==========================================================
 */
#if defined(GPN_CFLAGS_DEV_H20PN2000)
#define GPN_SYN_L_MA_PATCH_SYN_REQ(ask_role) gpnSockNmSynMsgTxPatchSynStartReq(ask_role)

#else
#define GPN_SYN_L_MA_PATCH_SYN_REQ(ask_role)

#endif

#define GPN_SYN_L_MA_PATCH_SYN_RSP(ask_src, ps_stat) gpnSockNmSynMsgTxPatchSynStartRsp(ask_src, ps_stat)

#define GPN_SYN_PATCH_SYN_XNODE_COMP_REQ(xl_type, wb_type, obj_type, hash_key) gpnSockNmSynMsgTxPatSynXNodeCompReq(xl_type, wb_type, obj_type, hash_key)
#define GPN_SYN_PATCH_SYN_XNODE_COMP_RSP(xl_type, wb_type, obj_type, hash_key, md5) gpnSockNmSynMsgTxPatSynXNodeCompRsp(xl_type, wb_type, obj_type, hash_key, md5)

#define GPN_SYN_PATCH_SYN_LEAF_COMP_REQ(wb_type, obj_type, hash_key, leaf_order)  gpnSockNmSynMsgTxPatSynLeafCompReq(wb_type, obj_type, hash_key, leaf_order);
#define GPN_SYN_PATCH_SYN_LEAF_COMP_RSP(wb_type, obj_type, hash_key, p_index, md5, syn_info, len, next_leaf_order)  gpnSockNmSynMsgTxPatSynLeafCompRsp(wb_type, obj_type, hash_key, p_index, md5, syn_info, len, next_leaf_order);

/*
 * ==========================================================
 * GPN SYN FILE TRANSFER ABOUT 
 * ==========================================================
 */
#define GPN_SYN_FILE_RENEW_REQ(file_type) gpnSockFTMsgFileSynReqCreat(file_type)
#define GPN_SYN_FILE_RENEW_DEL(req_id) gpnSockFTMsgFtReqDeleteNew(req_id)

/*
 * ==========================================================
 * GPN SYN AUOT PATCH SYN ABOUT
 * ==========================================================
 */
#define GPN_SYN_AUTO_PATCH_SYN_REQ(top_md5) gpnSockNmSynMsgTxAutoPatSynReq(top_md5)
#define GPN_SYN_AUTO_PATCH_SYN_RSP gpnSockNmSynMsgTxAutoPatSynRsp()

/*
 * ==========================================================
 * GPN SYN EMD About Syn Obj Opt
 * ==========================================================
 */


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_SYN_COMM_API_DEF_H_*/

