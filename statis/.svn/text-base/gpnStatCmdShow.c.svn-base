/******************************************************************************
 * Filename: gpnStatCmdShow.c
 *	Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.11.18  lipf created
 *
******************************************************************************/

#include <unistd.h>
#include <string.h>


#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/thread.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/ifm_common.h>
#include <lib/alarm.h>


#include "socketComm/gpnSockStatMsgDef.h"
#include "socketComm/gpnGlobalPortIndexDef.h"

#include "gpnStatCmdShow.h"
#include "gpnStatTypeStruct.h"
#include "gpnStatDebug.h"
#include "gpnStatDataStruct.h"
#include "gpnStatFuncApi.h"
#include "gpnStatSnmpApi.h"


extern stEQUStatProcSpace *pgstEQUStatProcSpace;


static void gpnStatTest(void)
{
	optObjOrient portIndex;
	memset(&portIndex, 0, sizeof(optObjOrient));
	portIndex.devIndex = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	//GPN_STAT_MSG_VS_P_STAT_GET
	portIndex.portIndex = LAB_PortIndex_Create(IFM_VS_2_TYPE, 0, 0);
	portIndex.portIndex3 = 100;
	gpnSockStatMsgApiNsmPtnVsPStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxPtnVsPStatGet(&portIndex, GPN_STAT_T_PTN_VS_P_TYPE);


	//GPN_STAT_MSG_VS_M_STAT_GET
	portIndex.portIndex = LAB_PortIndex_Create(IFM_VS_2_TYPE, 0, 0);
	portIndex.portIndex3 = 200;
	gpnSockStatMsgApiPtnVsMStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxPtnVsMStatGet(&portIndex, GPN_STAT_T_PTN_VS_M_TYPE);
	
	
	//GPN_STAT_MSG_LSP_P_STAT_GET
	portIndex.portIndex = LAB_PortIndex_Create(IFM_LSP_2_TYPE, 0, 0);
	portIndex.portIndex3 = 100;
	gpnSockStatMsgApiNsmPtnLspPStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxPtnLspPStatGet(&portIndex, GPN_STAT_T_PTN_LSP_P_TYPE);

	//GPN_STAT_MSG_LSP_M_STAT_GET
	portIndex.portIndex = LAB_PortIndex_Create(IFM_LSP_2_TYPE, 0, 0);
	portIndex.portIndex3 = 200;
	gpnSockStatMsgApiPtnLspMStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxPtnLspMStatGet(&portIndex, GPN_STAT_T_PTN_LSP_M_TYPE);

	//GPN_STAT_MSG_FLOW_P_STAT_GET
	portIndex.portIndex = LAB_PortIndex_Create(IFM_LSP_2_TYPE, 0, 0);
	portIndex.portIndex3 = 100;
	gpnSockStatMsgApiNsmFollowPStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxFollowPStatGet(&portIndex, GPN_STAT_SCAN_TYPE_PTN_LSP);
	
	//GPN_STAT_MSG_FLOW_M_STAT_GET
	portIndex.portIndex = LAB_PortIndex_Create(IFM_LSP_2_TYPE, 0, 0);
	portIndex.portIndex3 = 200;
	gpnSockStatMsgApiNsmFollowMStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxFollowMStatGet(&portIndex, GPN_STAT_SCAN_TYPE_PTN_LSP);
	
	//GPN_STAT_MSG_PW_P_STAT_GET
	portIndex.portIndex = LAB_PortIndex_Create(IFM_PW_2_TYPE, 0, 0);
	portIndex.portIndex3 = 100;
	gpnSockStatMsgApiNsmPtnPwPStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxPtnPwPStatGet(&portIndex, GPN_STAT_T_PTN_PW_P_TYPE);
	
	//GPN_STAT_MSG_PW_M_STAT_GET
	portIndex.portIndex = LAB_PortIndex_Create(IFM_PW_2_TYPE, 0, 0);
	portIndex.portIndex3 = 200;
	gpnSockStatMsgApiPtnPwMStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxPtnPwMStatGet(&portIndex, GPN_STAT_T_PTN_PW_M_TYPE);
	
	//GPN_STAT_MSG_VPLSPW_P_STAT_GET
	portIndex.portIndex = LAB_PortIndex_Create(IFM_VPLSPW_TYPE, 0, 0);
	portIndex.portIndex3 = 200;
	gpnSockStatMsgApiNsmPtnVplsPwPStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxPtnVplsPwPStatGet(&portIndex, GPN_STAT_T_PTN_VPLSPW_P_TYPE);
	
	//GPN_STAT_MSG_VPLSPW_M_STAT_GET
	portIndex.portIndex = SMP_PortIndex_Create(IFM_VPLSPW_TYPE, 0, 0);
	portIndex.portIndex3 = 200;
	gpnSockStatMsgApiNsmPtnVplsPwMStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxPtnVplsPwMStatGet(&portIndex, GPN_STAT_T_PTN_VPLSPW_M_TYPE);
	
	//GPN_STAT_PARA_PDHPPI_P_STAT_GET
	//portIndex.portIndex = PDH_PortIndex_Create(GPN_STAT_T_PDH_PPI_TYPE, 1, 1, 0)
	gpnSockStatMsgApiPdhPpiStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiPdhPpiStatGet(&portIndex, GPN_STAT_T_PDH_PPI_TYPE);

	//GPN_STAT_MSG_ETH_PHY_P_STAT_GET
	portIndex.portIndex = FUN_ETH_PortIndex_Create(IFM_ETH_TYPE, 1, 1);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	gpnSockStatMsgApiNsmEthPhyPStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxNsmEthPhyPStatGet(&portIndex, GPN_STAT_T_ETH_MAC_P_TYPE);
	
	//GPN_STAT_MSG_ETH_PHY_M_STAT_GET
	portIndex.portIndex = FUN_ETH_PortIndex_Create(IFM_ETH_TYPE, 1, 2);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	gpnSockStatMsgApiEthMonStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxEthMonStatGet(&portIndex, GPN_STAT_T_ETH_MAC_M_TYPE);
	
	//GPN_STAT_MSG_ETH_SFP_STAT_GET
	portIndex.portIndex = SMP_PortIndex_Create(IFM_ETH_SFP_TYPE, 1, 3);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	gpnSockStatMsgApiSdmSFPStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);	
	gpnSockStatMsgApiTxSdmEthSFPStatGet(&portIndex, GPN_STAT_T_ETH_SFP_TYPE);	
	
	//GPN_STAT_MSG_PTN_V_UNI_P_STAT_GET
	//gpnSockStatMsgApiNsmPtnVUNIPStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	//gpnSockStatMsgApiTxPtnVUNIPStatGet(&portIndex, GPN_STAT_T_PTN_V_UNI_P_TYPE);
	
	//GPN_STAT_MSG_PTN_MEP_M_STAT_GET
	portIndex.portIndex = CFM_MEP_PortIndex_Create(IFM_CFM_MEP_TYPE, 0, 300);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	gpnSockStatMsgApiOamPtnMepMStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxPtnMepMStatGet(&portIndex, GPN_STAT_T_PTN_MEP_M_TYPE);
	
	//GPN_STAT_MSG_EQU_STAT_GET
	portIndex.portIndex = SMP_PortIndex_Create(IFM_EQU_TYPE, 1, 1);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	gpnSockStatMsgApiSdmEquStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxSdmEquStatGet(&portIndex, GPN_STAT_T_EQU_TYPE);
	
	//GPN_STAT_MSG_SOFT_STAT_GET
	portIndex.portIndex = SMP_PortIndex_Create(IFM_SOFT_TYPE, 1, 1);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	gpnSockStatMsgApiSdmSoftStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxSdmSoftStatGet(&portIndex, GPN_STAT_T_SOFT_TYPE);
	
	//GPN_STAT_MSG_ENV_STAT_GET
	portIndex.portIndex = SMP_PortIndex_Create(IFM_ENV_TYPE, 1, 1);
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	gpnSockStatMsgApiSdmEnvStatMonStaCfg(&portIndex, GPN_SELECT_GEN_ENABLE);
	gpnSockStatMsgApiTxSdmEnvStatGet(&portIndex, GPN_STAT_T_ENV_TYPE);
}

void statTest(struct vty *vty)
{
	vty_out(vty, "Statis testing start!%s", VTY_NEWLINE);
	gpnStatTest();
	vty_out(vty, "Statis testing stop!%s", VTY_NEWLINE);
}


static uint32_t debug_statis_add_event_tp(int scanType)
{
	uint32_t eventTpId = gpnStatEventThredTemplatIdGet();
	zlog_debug(0, "%s[%d] : event id(%d)\n", __func__, __LINE__, eventTpId);
	
	gpnSockMsg gpnStatMsgSpIn, gpnStatMsgSpOut;
	gpnSockMsg *pgpnStatMsgSpIn = &gpnStatMsgSpIn;
	gpnSockMsg *pgpnStatMsgSpOut = &gpnStatMsgSpOut;
	memset(pgpnStatMsgSpIn, 0, sizeof(gpnSockMsg));
	memset(pgpnStatMsgSpOut, 0, sizeof(gpnSockMsg));
	
	pgpnStatMsgSpIn->iIndex   = 1;
	pgpnStatMsgSpIn->iMsgCtrl = 0;
	pgpnStatMsgSpIn->iDstId   = GPN_COMMM_STAT;
	pgpnStatMsgSpIn->iSrcId	  = GPN_COMMM_STAT;
	pgpnStatMsgSpIn->iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_ADD;
	pgpnStatMsgSpIn->iMsgPara1 = eventTpId;
	//pgpnStatMsgSpIn->iMsgPara2 = scanSubType;	//扫描子类型
	pgpnStatMsgSpIn->iMsgPara3 = scanType;
	pgpnStatMsgSpIn->iMsgPara4 = 0x00000011;
	pgpnStatMsgSpIn->iMsgPara5 = 0x00000022;
	pgpnStatMsgSpIn->iMsgPara6 = 0x00000077;
	pgpnStatMsgSpIn->iMsgPara7 = 0x00000088;
	pgpnStatMsgSpIn->msgCellLen = 0;
	
	switch(scanType)
	{
		case GPN_STAT_SCAN_TYPE_SDH_OH:
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_SDH_RS_BBE_TYPE;	//SDH再生段背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_SDH_HP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_SDH_HP_BBE_TYPE;	//高阶通道背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_SDH_LP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_SDH_LP_BBE_TYPE;	//低阶通道背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_PDH:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PDH_BBE_TYPE;	//PDH再生段背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_ETH:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_ETH_MAC_RXGB_TYPE;	//ETH接收好字节数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_VS:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_VS_TXF_TYPE;	//出方向VS包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_LSP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_LSP_TXF_TYPE;	//前向出方向LSP包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_PW:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_PW_TXF_TYPE;	//出方向PW包数
			break;

		case GPN_STAT_SCAN_TYPE_FOLLOW:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_FOLLOW_TXF_TYPE;	//特征流发送包数
			break;

		case GPN_STAT_SCAN_TYPE_POW:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_POW_VOL_TYPE;	//电源电压
			break;

		case GPN_STAT_SCAN_TYPE_EQU:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_EQU_CPU_UTILIZA_TYPE;	//设备CPU利用率
			break;

		case GPN_STAT_SCAN_TYPE_SOFT:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_CPU_UTILIZA_TYPE;	//CPU利用率
			break;

		case GPN_STAT_SCAN_TYPE_ENV:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_ENV_HUM_TYPE;	//环境湿度
			break;

		case GPN_STAT_SCAN_TYPE_ETH_SFP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_ETHSFP_TEM_TYPE;	//以太网光模块温度
			break;

		case GPN_STAT_SCAN_TYPE_PTN_VPLSPW:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_VPLSPW_TXF_TYPE;	//出方向VPLS PW包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_V_UNI:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_V_UNI_TXF_TYPE;	//出方向V_UNI包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_MEP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_MEP_DROP_RATA_TYPE;	//CFM本端丢包率
			break;

		default:
			break;
	}
	
	uint32_t reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		zlog_debug(0, "%s : add event eventTpId(%d) err\n\r", __FUNCTION__, eventTpId);
		return 0;
	}
	
	return eventTpId;
}

static uint32_t debug_statis_add_alarm_tp(int scanType)
{
	uint32_t alarmTpId = gpnStatAlarmThredTemplatIdGet();
	zlog_debug(0, "%s[%d] : alarm id(%d)\n", __func__, __LINE__, alarmTpId);
	
	gpnSockMsg gpnStatMsgSpIn, gpnStatMsgSpOut;
	gpnSockMsg *pgpnStatMsgSpIn = &gpnStatMsgSpIn;
	gpnSockMsg *pgpnStatMsgSpOut = &gpnStatMsgSpOut;
	memset(pgpnStatMsgSpIn, 0, sizeof(gpnSockMsg));
	memset(pgpnStatMsgSpOut, 0, sizeof(gpnSockMsg));

	pgpnStatMsgSpIn->iIndex   = 1;
	pgpnStatMsgSpIn->iMsgCtrl = 0;
	pgpnStatMsgSpIn->iDstId   = GPN_COMMM_STAT;
	pgpnStatMsgSpIn->iSrcId	  = GPN_COMMM_STAT;
	pgpnStatMsgSpIn->iMsgType = GPN_STAT_MSG_ALM_THRED_TEMP_ADD;
	pgpnStatMsgSpIn->iMsgPara1 = alarmTpId;
	//pgpnStatMsgSpIn->iMsgPara2 = scanSubType;	//扫描子类型
	pgpnStatMsgSpIn->iMsgPara3 = scanType;
	pgpnStatMsgSpIn->iMsgPara4 = 0x00000011;
	pgpnStatMsgSpIn->iMsgPara5 = 0x00000022;
	pgpnStatMsgSpIn->iMsgPara6 = 0x00000033;
	pgpnStatMsgSpIn->iMsgPara7 = 0x00000044;
	pgpnStatMsgSpIn->iMsgPara8 = 0x00000055;
	pgpnStatMsgSpIn->iMsgPara9 = 0x00000066;
	pgpnStatMsgSpIn->iMsgParaA = 0x00000077;
	pgpnStatMsgSpIn->iMsgParaB = 0x00000088;
	pgpnStatMsgSpIn->msgCellLen = 0;
	
	switch(scanType)
	{
		case GPN_STAT_SCAN_TYPE_SDH_OH:
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_SDH_RS_BBE_TYPE;	//SDH再生段背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_SDH_HP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_SDH_HP_BBE_TYPE;	//高阶通道背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_SDH_LP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_SDH_LP_BBE_TYPE;	//低阶通道背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_PDH:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PDH_BBE_TYPE;	//PDH再生段背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_ETH:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_ETH_MAC_RXGB_TYPE;	//ETH接收好字节数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_VS:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_VS_TXF_TYPE;	//出方向VS包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_LSP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_LSP_TXF_TYPE;	//前向出方向LSP包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_PW:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_PW_TXF_TYPE;	//出方向PW包数
			break;

		case GPN_STAT_SCAN_TYPE_FOLLOW:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_FOLLOW_TXF_TYPE;	//特征流发送包数
			break;

		case GPN_STAT_SCAN_TYPE_POW:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_POW_VOL_TYPE;	//电源电压
			break;

		case GPN_STAT_SCAN_TYPE_EQU:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_EQU_CPU_UTILIZA_TYPE;	//设备CPU利用率
			break;

		case GPN_STAT_SCAN_TYPE_SOFT:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_CPU_UTILIZA_TYPE;	//CPU利用率
			break;

		case GPN_STAT_SCAN_TYPE_ENV:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_ENV_HUM_TYPE;	//环境湿度
			break;

		case GPN_STAT_SCAN_TYPE_ETH_SFP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_ETHSFP_TEM_TYPE;	//以太网光模块温度
			break;

		case GPN_STAT_SCAN_TYPE_PTN_VPLSPW:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_VPLSPW_TXF_TYPE;	//出方向VPLS PW包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_V_UNI:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_V_UNI_TXF_TYPE;	//出方向V_UNI包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_MEP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_MEP_DROP_RATA_TYPE;	//CFM本端丢包率
			break;

		default:
			break;
	}
	
	uint32_t reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		zlog_debug(0, "%s : add event alarmTpId(%d) err\n\r", __FUNCTION__, alarmTpId);
		return 0;
	}

	return alarmTpId;
}

static uint32_t debug_statis_add_subType_filt_tp(int scanType)
{
	uint32_t subFiltTpId = gpnStatSubTypeFiltIdGet();
	zlog_debug(0, "%s[%d] : subFiltTpId(%d)\n", __func__, __LINE__, subFiltTpId);
	
	gpnSockMsg gpnStatMsgSpIn, gpnStatMsgSpOut;
	gpnSockMsg *pgpnStatMsgSpIn = &gpnStatMsgSpIn;
	gpnSockMsg *pgpnStatMsgSpOut = &gpnStatMsgSpOut;
	memset(pgpnStatMsgSpIn, 0, sizeof(gpnSockMsg));
	memset(pgpnStatMsgSpOut, 0, sizeof(gpnSockMsg));

	pgpnStatMsgSpIn->iIndex   = 1;
	pgpnStatMsgSpIn->iMsgCtrl = 0;
	pgpnStatMsgSpIn->iDstId   = GPN_COMMM_STAT;
	pgpnStatMsgSpIn->iSrcId   = GPN_COMMM_STAT;
	pgpnStatMsgSpIn->iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_ADD;
	pgpnStatMsgSpIn->iMsgPara1 = subFiltTpId;
	//pgpnStatMsgSpIn->iMsgPara2 = scanSubType; //扫描子类型
	pgpnStatMsgSpIn->iMsgPara3 = scanType;
	pgpnStatMsgSpIn->iMsgPara4 = GPN_STAT_DEBUG_DISABLE;
	pgpnStatMsgSpIn->msgCellLen = 0;
	
	switch(scanType)
	{
		case GPN_STAT_SCAN_TYPE_SDH_OH:
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_SDH_RS_BBE_TYPE;	//SDH再生段背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_SDH_HP: 		 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_SDH_HP_BBE_TYPE;	//高阶通道背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_SDH_LP: 		 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_SDH_LP_BBE_TYPE;	//低阶通道背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_PDH:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PDH_BBE_TYPE;	//PDH再生段背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_ETH:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_ETH_MAC_RXGB_TYPE;	//ETH接收好字节数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_VS: 		 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_VS_TXF_TYPE;	//出方向VS包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_LSP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_LSP_TXF_TYPE;	//前向出方向LSP包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_PW: 		 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_PW_TXF_TYPE;	//出方向PW包数
			break;

		case GPN_STAT_SCAN_TYPE_FOLLOW: 		 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_FOLLOW_TXF_TYPE;	//特征流发送包数
			break;

		case GPN_STAT_SCAN_TYPE_POW:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_POW_VOL_TYPE;	//电源电压
			break;

		case GPN_STAT_SCAN_TYPE_EQU:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_EQU_CPU_UTILIZA_TYPE;	//设备CPU利用率
			break;

		case GPN_STAT_SCAN_TYPE_SOFT:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_CPU_UTILIZA_TYPE;	//CPU利用率
			break;

		case GPN_STAT_SCAN_TYPE_ENV:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_ENV_HUM_TYPE;	//环境湿度
			break;

		case GPN_STAT_SCAN_TYPE_ETH_SFP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_ETHSFP_TEM_TYPE;	//以太网光模块温度
			break;

		case GPN_STAT_SCAN_TYPE_PTN_VPLSPW: 		 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_VPLSPW_TXF_TYPE;	//出方向VPLS PW包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_V_UNI:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_V_UNI_TXF_TYPE; //出方向V_UNI包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_MEP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_MEP_DROP_RATA_TYPE; //CFM本端丢包率
			break;

		default:
			break;
	}
	
	uint32_t reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		zlog_debug(0, "%s : add event subFiltTpId(%d) err\n\r", __FUNCTION__, subFiltTpId);
		return 0;
	}

	return subFiltTpId;
}


static uint32_t debug_statis_add_subType_report_tp(int scanType)
{
	uint32_t subReportTpId = gpnStatSubTypeReportIdGet();
	zlog_debug(0, "%s[%d] : subReportTpId(%d)\n", __func__, __LINE__, subReportTpId);
	
	gpnSockMsg gpnStatMsgSpIn, gpnStatMsgSpOut;
	gpnSockMsg *pgpnStatMsgSpIn = &gpnStatMsgSpIn;
	gpnSockMsg *pgpnStatMsgSpOut = &gpnStatMsgSpOut;
	memset(pgpnStatMsgSpIn, 0, sizeof(gpnSockMsg));
	memset(pgpnStatMsgSpOut, 0, sizeof(gpnSockMsg));

	pgpnStatMsgSpIn->iIndex   = 1;
	pgpnStatMsgSpIn->iMsgCtrl = 0;
	pgpnStatMsgSpIn->iDstId   = GPN_COMMM_STAT;
	pgpnStatMsgSpIn->iSrcId   = GPN_COMMM_STAT;
	pgpnStatMsgSpIn->iMsgType = GPN_STAT_MSG_SUBREPORT_TEMP_ADD;
	pgpnStatMsgSpIn->iMsgPara1 = subReportTpId;
	//pgpnStatMsgSpIn->iMsgPara2 = scanSubType; //扫描子类型
	pgpnStatMsgSpIn->iMsgPara3 = scanType;
	pgpnStatMsgSpIn->iMsgPara4 = GPN_STAT_DEBUG_ENABLE;
	pgpnStatMsgSpIn->msgCellLen = 0;
	
	switch(scanType)
	{
		case GPN_STAT_SCAN_TYPE_SDH_OH:
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_SDH_RS_BBE_TYPE;	//SDH再生段背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_SDH_HP: 		 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_SDH_HP_BBE_TYPE;	//高阶通道背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_SDH_LP: 		 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_SDH_LP_BBE_TYPE;	//低阶通道背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_PDH:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PDH_BBE_TYPE;	//PDH再生段背景误码块
			break;

		case GPN_STAT_SCAN_TYPE_ETH:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_ETH_MAC_RXGB_TYPE;	//ETH接收好字节数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_VS: 		 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_VS_TXF_TYPE;	//出方向VS包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_LSP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_LSP_TXF_TYPE;	//前向出方向LSP包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_PW: 		 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_PW_TXF_TYPE;	//出方向PW包数
			break;

		case GPN_STAT_SCAN_TYPE_FOLLOW: 		 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_FOLLOW_TXF_TYPE;	//特征流发送包数
			break;

		case GPN_STAT_SCAN_TYPE_POW:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_POW_VOL_TYPE;	//电源电压
			break;

		case GPN_STAT_SCAN_TYPE_EQU:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_EQU_CPU_UTILIZA_TYPE;	//设备CPU利用率
			break;

		case GPN_STAT_SCAN_TYPE_SOFT:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_CPU_UTILIZA_TYPE;	//CPU利用率
			break;

		case GPN_STAT_SCAN_TYPE_ENV:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_ENV_HUM_TYPE;	//环境湿度
			break;

		case GPN_STAT_SCAN_TYPE_ETH_SFP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_ETHSFP_TEM_TYPE;	//以太网光模块温度
			break;

		case GPN_STAT_SCAN_TYPE_PTN_VPLSPW: 		 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_VPLSPW_TXF_TYPE;	//出方向VPLS PW包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_V_UNI:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_V_UNI_TXF_TYPE; //出方向V_UNI包数
			break;

		case GPN_STAT_SCAN_TYPE_PTN_MEP:			 
			pgpnStatMsgSpIn->iMsgPara2 = GPN_STAT_D_PTN_MEP_DROP_RATA_TYPE; //CFM本端丢包率
			break;

		default:
			break;
	}
	
	uint32_t reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		zlog_debug(0, "%s : add event subReportTpId(%d) err\n\r", __FUNCTION__, subReportTpId);
	}

	return subReportTpId;
}





static uint32_t debug_statis_add_task(int scanType)
{
	uint32_t reVal = 0;
	uint32_t taskId = 0;
	uint32_t almThTpId = 0;
	uint32_t subReportTpId = 0;
	
	gpnSockMsg gpnStatMsgSpIn, gpnStatMsgSpOut;
	gpnSockMsg *pgpnStatMsgSpIn = &gpnStatMsgSpIn;
	gpnSockMsg *pgpnStatMsgSpOut = &gpnStatMsgSpOut;
	memset(pgpnStatMsgSpIn, 0, sizeof(gpnSockMsg));
	memset(pgpnStatMsgSpOut, 0, sizeof(gpnSockMsg));
	
	/*add alarm threshold template */
	almThTpId = debug_statis_add_alarm_tp(scanType);

	/*add alarm threshold template */
	subReportTpId = debug_statis_add_subType_report_tp(scanType);

	/*scanType get */
	pgpnStatMsgSpIn->iIndex   = 1;
	pgpnStatMsgSpIn->iMsgCtrl = 0;
	pgpnStatMsgSpIn->iDstId   = GPN_COMMM_STAT;;
	pgpnStatMsgSpIn->iSrcId   = GPN_COMMM_STAT;
		
	pgpnStatMsgSpOut = &gpnStatMsgSpOut;

	/*get sub report template Id */
	taskId = gpnStatTaskIdGet();
	zlog_debug(0, "%s[%d] : taskId(%d)\n", __func__, __LINE__, taskId);
		
	pgpnStatMsgSpIn->iMsgType  = GPN_STAT_MSG_TASK_ADD;
	pgpnStatMsgSpIn->iMsgPara1 = taskId;
	pgpnStatMsgSpIn->iMsgPara2 = scanType;
	pgpnStatMsgSpIn->iMsgPara3 = 3/*taskMonObjNum*/;
	pgpnStatMsgSpIn->iMsgPara4 = GPN_STAT_CYC_CLASS_USR;
	pgpnStatMsgSpIn->iMsgPara5 = 15;
	pgpnStatMsgSpIn->iMsgPara6 = 0;
	pgpnStatMsgSpIn->iMsgPara7 = 0xffffffff;
	pgpnStatMsgSpIn->iMsgPara8 = almThTpId;
	pgpnStatMsgSpIn->iMsgPara9 = subReportTpId;
	pgpnStatMsgSpIn->msgCellLen = 0;
	
	reVal = gpnStatGlobalPorc(&gpnStatMsgSpIn, sizeof(gpnSockMsg));
	if(reVal != GPN_STAT_DEBUG_OK)
	{
		zlog_debug(0, "%s : add taskId(%d) err\n\r", __FUNCTION__, taskId);
		return 0;
	}

	return taskId;
}

#if 0
static int debug_statis_creat_portIndex (struct gpnPortInfo *pgPortInfo, objLogicDesc *pPortIndex)
{	
	uint32_t unit = IFM_UNIT_ID_GET(pgPortInfo->iIfindex);
	uint32_t portType = IFM_TYPE_ID_GET(pgPortInfo->iIfindex);
	uint32_t slotNo = IFM_SLOT_ID_GET(pgPortInfo->iIfindex);
	uint32_t portNo = IFM_PORT_ID_GET(pgPortInfo->iIfindex);
	uint32_t subPortNo = IFM_SUBPORT_ID_GET(pgPortInfo->iIfindex);

	zlog_debug(0, "%s[%d] : alarmType[%x] slot[%d] port[%d] para1[%d] para2[%d]\n",
		__func__, __LINE__, pgPortInfo->iAlarmPort, slotNo, portNo, pgPortInfo->iMsgPara1, pgPortInfo->iMsgPara2);

	pPortIndex->devIndex = DeviceIndex_Create(unit);
	
	switch (pgPortInfo->iAlarmPort)
	{
		case IFM_FUN_ETH_TYPE:		
		case IFM_TUNNEL_TYPE:
			pPortIndex->portIndex = FUN_ETH_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, portNo);
			break;

		case IFM_TRUNK_TYPE:
		case IFM_SOFT_TYPE:
		case IFM_PWE3E1_TYPE:
		case IFM_POWN48_TYPE:		
		case IFM_PWE3STMN_TYPE:
		case IFM_EQU_TYPE:
		case IFM_ENV_TYPE:
		case IFM_FAN_TYPE:
		case IFM_POW220_TYPE:
		case IFM_V35_TYPE:
		case IFM_DSL_TYPE:
		case IFM_ETH_CLC_IN_TYPE:
		case IFM_ETH_CLC_OUT_TYPE:
		case IFM_ETH_CLC_SUB_TYPE:
		case IFM_ETH_CLC_LINE_TYPE:
		case IFM_ETH_SFP_TYPE:		
			pPortIndex->portIndex = SMP_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, portNo);
			break;

		case IFM_VC3_TYPE:
		case IFM_VC12_TYPE:
		case IFM_VC4_TYPE:
		case IFM_STMN_TYPE:
			pPortIndex->portIndex = SDH_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, portNo, 
													pgPortInfo->iMsgPara1, pgPortInfo->iMsgPara2);
			break;

		case IFM_VCG_TYPE:
		case IFM_VCG_LP_TYPE:
			pPortIndex->portIndex = VCG_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, portNo, pgPortInfo->iMsgPara1);
			break;

		case IFM_ETH_TYPE:
			break;

		case IFM_VS_TYPE:
		case IFM_LSP_TYPE:
		case IFM_PW_TYPE:
		case IFM_MPLS_TYPE:		
		case IFM_BFD_TYPE:
			pPortIndex->portIndex = LAB_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, pgPortInfo->iMsgPara1);
			break;

		case IFM_VS_2_TYPE:
		case IFM_LSP_2_TYPE:
		case IFM_PW_2_TYPE:
		case IFM_VPLSPW_TYPE:
			pPortIndex->portIndex = LAB_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, portNo);
			pPortIndex->portIndex3 = pgPortInfo->iMsgPara1;
			break;

		case IFM_VLAN_OAM_TYPE:
			pPortIndex->portIndex = VLAN_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, portNo, pgPortInfo->iMsgPara1);
			break;

		case IFM_CFM_MEP_TYPE:
			pPortIndex->portIndex = CFM_MEP_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, pgPortInfo->iMsgPara1);
			break;

		case IFM_FUN_ETH_SUB_TYPE:
			pPortIndex->portIndex = FUN_ETH_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, portNo);
			pPortIndex->portIndex3 = subPortNo;
			break;

		case IFM_TRUNK_SUBIF_TYPE:
			pPortIndex->portIndex = SMP_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, portNo);
			pPortIndex->portIndex3 = subPortNo;
			break;

		case IFM_E1_SUB_TYPE:
			pPortIndex->portIndex = PDH_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, portNo, pgPortInfo->iMsgPara1);
			pPortIndex->portIndex3 = subPortNo;
			break;
		
		case IFM_E1_TYPE:
		case IFM_E2_TYPE:
		case IFM_E3_TYPE:
			pPortIndex->portIndex = PDH_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, portNo, pgPortInfo->iMsgPara1);
			break;

		case IFM_VUNI_TYPE:
			pPortIndex->portIndex = FUN_ETH_PortIndex_Create(pgPortInfo->iAlarmPort, slotNo, portNo);
			pPortIndex->portIndex3 = pgPortInfo->iMsgPara1;
			break;
			
		default:
			break;
	}

	return TRUE;
}
#endif



void debug_statis_add_port_monitor(struct gpnPortInfo *pgPortInfo)
{	
	uint32_t reVal = 0;
	GPN_STAT_TYPE scanType;	
	optObjOrient portIndex;
	gpnSockMsg gpnStatMsgSpIn;
	gpnSockMsg *pgpnStatMsgSpOut = NULL;
	
	stPortTpToStatScanTp *pPortTpToScanTp;
	gpnStatSeekPortType2PTVsST(pgPortInfo->iAlarmPort, &pPortTpToScanTp);
	if(NULL == pPortTpToScanTp)
	{
		zlog_debug(0, "%s[%d] : add port monitor error\n", __func__, __LINE__);
		XFREE(MTYPE_STAT, pgpnStatMsgSpOut);
		return;
	}
	scanType = pPortTpToScanTp->statScanType;
	zlog_debug(0, "%s[%d] : scanType(%d)\n", __func__, __LINE__, scanType);
	uint32_t taskId = debug_statis_add_task(scanType);
	uint32_t evtTpId = debug_statis_add_event_tp(scanType);
	uint32_t filtTpId = debug_statis_add_subType_filt_tp(scanType);

	pgpnStatMsgSpOut = (gpnSockMsg *)XMALLOC(MTYPE_STAT, sizeof(gpnSockMsg) + sizeof(stStatMsgMonObjCfg) +\
		sizeof(objLogicDesc) + sizeof(uint32_t));
		
	portIndex.portIndex = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex3 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex4 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex5 = GPN_ILLEGAL_PORT_INDEX;
	portIndex.portIndex6 = GPN_ILLEGAL_PORT_INDEX;

	//debug_statis_creat_portIndex(pgPortInfo, &portIndex);
	ipranIndexToGpnIndex(pgPortInfo, &portIndex);
	zlog_debug(0, "%s[%d] : index(%x|%x|%x|%x|%x|%x)\n", __func__, __LINE__,
		portIndex.devIndex, portIndex.portIndex, portIndex.portIndex3,
		portIndex.portIndex4, portIndex.portIndex5, portIndex.portIndex6);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_MON_ADD;	
	gpnStatMsgSpIn.iMsgPara1 = portIndex.devIndex;
	gpnStatMsgSpIn.iMsgPara2 = portIndex.portIndex;
	gpnStatMsgSpIn.iMsgPara3 = portIndex.portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = portIndex.portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = portIndex.portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = portIndex.portIndex6;
	gpnStatMsgSpIn.iMsgPara7 = scanType;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		zlog_debug(0, "%s : add dev(%08x) port(%08x|%08x|%08x|%08x|%08x)!\n\r",\
			__FUNCTION__, portIndex.devIndex, portIndex.portIndex, portIndex.portIndex3,\
			portIndex.portIndex4, portIndex.portIndex5, portIndex.portIndex6);
	}
	else
	{
		zlog_debug(0, "%s[%d] : add portMon err!\n\r", __FUNCTION__, __LINE__);
		return;
	}

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_BASE_MON_OPT;
	gpnStatMsgSpIn.iMsgPara1 = portIndex.devIndex;
	gpnStatMsgSpIn.iMsgPara2 = portIndex.portIndex;
	gpnStatMsgSpIn.iMsgPara3 = portIndex.portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = portIndex.portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = portIndex.portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = portIndex.portIndex6;
	gpnStatMsgSpIn.iMsgPara7 = scanType;
	gpnStatMsgSpIn.iMsgPara8 = GPN_STAT_DEBUG_ENABLE;
	gpnStatMsgSpIn.iMsgPara9 = GPN_STAT_DEBUG_ENABLE;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		zlog_debug(0, "%s[%d] : enable base dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x)!\n\r",\
			__FUNCTION__, __LINE__, portIndex.devIndex, portIndex.portIndex, portIndex.portIndex3,\
			portIndex.portIndex4, portIndex.portIndex5, portIndex.portIndex6, scanType);
	}
	else
	{
		zlog_debug(0, "%s[%d] : enable porBasetMon err!\n\r", __FUNCTION__, __LINE__);
		return;
	}

	pgpnStatMsgSpOut->msgCellLen = sizeof(stStatMsgMonObjCfg);

	zlog_debug(0, "%s[%d] : taskId(%d), evtTpId(%d), filtTpId(%d)\n", __func__, __LINE__, taskId, evtTpId, filtTpId);
	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_PORT_CYC_MON_OPT;
	gpnStatMsgSpIn.iMsgPara1 = portIndex.devIndex;
	gpnStatMsgSpIn.iMsgPara2 = portIndex.portIndex;
	gpnStatMsgSpIn.iMsgPara3 = portIndex.portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = portIndex.portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = portIndex.portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = portIndex.portIndex6;
	gpnStatMsgSpIn.iMsgPara7 = scanType;
	gpnStatMsgSpIn.iMsgPara8 = taskId;
	gpnStatMsgSpIn.iMsgPara9 = GPN_STAT_DEBUG_ENABLE;
	gpnStatMsgSpIn.iMsgParaA = evtTpId/*event tp id*/;
	gpnStatMsgSpIn.iMsgParaB = filtTpId/*sub filt tp id*/;
	gpnStatMsgSpIn.iMsgParaC = GPN_STAT_DBS_GEN_ENABLE;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	if(reVal == GPN_STAT_DEBUG_OK)
	{
		zlog_debug(0, "%s : enable cyc dev(%08x) port(%08x|%08x|%08x|%08x|%08x) scanType(%08x)!\n\r",\
			__FUNCTION__, portIndex.devIndex, portIndex.portIndex, portIndex.portIndex3,\
			portIndex.portIndex4, portIndex.portIndex5, portIndex.portIndex6, GPN_STAT_SCAN_TYPE_PTN_LSP);
	}
	else
	{
		zlog_debug(0, "%s[%d] : enable porCyctMon err!\n\r", __FUNCTION__, __LINE__);
		return;
	}

		

	XFREE(MTYPE_STAT, pgpnStatMsgSpOut);
}



static int gpnStatShowPortInfo(struct vty *vty, objLogicDesc *pLocalPort)
{
	if(NULL == pLocalPort)
	{
		return -1;
	}

	struct ifm_usp usp;
	memset(&usp, 0, sizeof(struct ifm_usp));

	char show_buf[82];
	memset(show_buf, 0, sizeof(show_buf));
	
	char *pAlarmPort  = &show_buf[0];
	char *pAlarmIndex = &show_buf[16];	

	uint32_t portType = PortIndex_GetType(pLocalPort->portIndex);
	switch(portType)
	{
		case IFM_FUN_ETH_TYPE:
			sprintf (pAlarmPort, "%s                 ", "ETH");
			break;
		
		case IFM_TUNNEL_TYPE:
			sprintf (pAlarmPort, "%s                 ", "TUNNEL");
			break;

		case IFM_FUN_ETH_SUB_TYPE:
			sprintf (pAlarmPort, "%s                 ", "ETH-SUB");
			break;
		
		case IFM_VUNI_TYPE:
			sprintf (pAlarmPort, "%s                 ", "VUNI");
			break;

		case IFM_SOFT_TYPE:
			sprintf (pAlarmPort, "%s                 ", "SOFT");
			break;
		
		case IFM_PWE3E1_TYPE:
			sprintf (pAlarmPort, "%s                 ", "P3E1");
			break;
		
		case IFM_POWN48_TYPE:
			sprintf (pAlarmPort, "%s                 ", "POWN48");
			break;
		
		case IFM_STMN_TYPE:
			sprintf (pAlarmPort, "%s                 ", "STMN");
			break;
		
		case IFM_PWE3STMN_TYPE:
			sprintf (pAlarmPort, "%s                 ", "PWE3STMN");
			break;
		
		case IFM_EQU_TYPE:
			sprintf (pAlarmPort, "%s                 ", "EQU");
			break;
		
		case IFM_ENV_TYPE:
			sprintf (pAlarmPort, "%s                 ", "ENV");
			break;
		
		case IFM_FAN_TYPE:
			sprintf (pAlarmPort, "%s                 ", "FAN");
			break;
		
		case IFM_POW220_TYPE:
			sprintf (pAlarmPort, "%s                 ", "POW220");
			break;
		
		case IFM_ETH_CLC_IN_TYPE:
			sprintf (pAlarmPort, "%s                 ", "ETH-CLC-IN");
			break;
		
		case IFM_ETH_CLC_OUT_TYPE:
			sprintf (pAlarmPort, "%s                 ", "ETH-CLC-OUT");
			break;
		
		case IFM_ETH_CLC_SUB_TYPE:
			sprintf (pAlarmPort, "%s                 ", "ETH-CLC-SUB");
			break;
		
		case IFM_ETH_CLC_LINE_TYPE:
			sprintf (pAlarmPort, "%s                 ", "ETH-CLC-LINE");
			break;
		
		case IFM_ETH_SFP_TYPE:
			sprintf (pAlarmPort, "%s                 ", "ETH-SFP");
			break;

		case IFM_TRUNK_TYPE:
			sprintf (pAlarmPort, "%s                 ", "TRUNK");
			break;

		case IFM_TRUNK_SUBIF_TYPE:
			sprintf (pAlarmPort, "%s                 ", "TRUNK-SUB");
			break;

		case IFM_E1_TYPE:
			sprintf (pAlarmPort, "%s                 ", "E1");
			break;

		case IFM_E1_SUB_TYPE:
			sprintf (pAlarmPort, "%s                 ", "E1-SUB");
			break;

		case IFM_BFD_TYPE:
			sprintf (pAlarmPort, "%s                 ", "BFD");
			break;

		case IFM_VS_2_TYPE:
			sprintf (pAlarmPort, "%s                 ", "VS");
			break;
		
		case IFM_LSP_2_TYPE:
			sprintf (pAlarmPort, "%s                 ", "LSP");
			break;
		
		case IFM_PW_2_TYPE:
			sprintf (pAlarmPort, "%s                 ", "PW");
			break;
		
		case IFM_VPLSPW_TYPE:
			sprintf (pAlarmPort, "%s                 ", "VPLSPW");
			break;

		case IFM_VLAN_OAM_TYPE:
			sprintf (pAlarmPort, "%s                 ", "VLAN-OAM");
			break;

		case IFM_CFM_MEP_TYPE:
			sprintf (pAlarmPort, "%s                 ", "CFM-MEP");
			break;
			
		default:
			return -1;
	}

	usp.unit = IFM_DEV_UNIT_GET(pLocalPort->devIndex);
	switch(portType)
	{
		case IFM_FUN_ETH_TYPE:
		case IFM_TUNNEL_TYPE:
			usp.slot = IFM_FUN_ETH_SLOT_DECOM(pLocalPort->portIndex);
			usp.port = IFM_FUN_ETH_PORTID_DECOM(pLocalPort->portIndex);
			sprintf (pAlarmIndex, "%d/%d/%d               ", usp.unit, usp.slot, usp.port);
			break;

		case IFM_FUN_ETH_SUB_TYPE:
		case IFM_VUNI_TYPE:
			usp.slot = IFM_FUN_ETH_SLOT_DECOM(pLocalPort->portIndex);
			usp.port = IFM_FUN_ETH_PORTID_DECOM(pLocalPort->portIndex);
			usp.sub_port = pLocalPort->portIndex3;
			sprintf (pAlarmIndex, "%d/%d/%d.%d             ", usp.unit, usp.slot, usp.port, usp.sub_port);
			break;

		case IFM_SOFT_TYPE:
		case IFM_PWE3E1_TYPE:
		case IFM_POWN48_TYPE:
		case IFM_STMN_TYPE:
		case IFM_PWE3STMN_TYPE:
		case IFM_EQU_TYPE:
		case IFM_ENV_TYPE:
		case IFM_FAN_TYPE:
		case IFM_POW220_TYPE:
		case IFM_ETH_CLC_IN_TYPE:
		case IFM_ETH_CLC_OUT_TYPE:
		case IFM_ETH_CLC_SUB_TYPE:
		case IFM_ETH_CLC_LINE_TYPE:
		case IFM_ETH_SFP_TYPE:
			usp.slot = IFM_SMP_SLOT_DECOM(pLocalPort->portIndex);
			usp.port = IFM_SMP_PORTID_DECOM(pLocalPort->portIndex);
			sprintf (pAlarmIndex, "%d/%d/%d             ", usp.unit, usp.slot, usp.port);
			break;

		case IFM_TRUNK_TYPE:
			usp.port = IFM_SMP_PORTID_DECOM(pLocalPort->portIndex);
			sprintf (pAlarmIndex, "%d/%d                ", usp.unit, usp.port);
			break;

		case IFM_TRUNK_SUBIF_TYPE:
			usp.port = IFM_SMP_PORTID_DECOM(pLocalPort->portIndex);
			usp.sub_port = pLocalPort->portIndex3;
			sprintf (pAlarmIndex, "%d/%d.%d              ", usp.unit, usp.port, usp.sub_port);
			break;

		case IFM_E1_TYPE:
			usp.slot = IFM_PDH_SLOT_DECOM(pLocalPort->portIndex);
			usp.port = IFM_PDH_PORTID_DECOM(pLocalPort->portIndex);
			sprintf (pAlarmIndex, "%d/%d/%d             ", usp.unit, usp.slot, usp.port);
			break;

		case IFM_E1_SUB_TYPE:
			usp.slot = IFM_PDH_SLOT_DECOM(pLocalPort->portIndex);
			usp.port = IFM_PDH_PORTID_DECOM(pLocalPort->portIndex);
			usp.sub_port = pLocalPort->portIndex3;
			sprintf (pAlarmIndex, "%d/%d/%d.%d           ", usp.unit, usp.slot, usp.port, usp.sub_port);
			break;

		case IFM_BFD_TYPE:
			sprintf (pAlarmIndex, "%d/%d                 ", usp.unit, IFM_LAB_LAB_DECOM(pLocalPort->portIndex));
			break;

		case IFM_VS_2_TYPE:
		case IFM_LSP_2_TYPE:
		case IFM_PW_2_TYPE:
		case IFM_VPLSPW_TYPE:
			sprintf (pAlarmIndex, "%d/%d                 ", usp.unit, pLocalPort->portIndex3);
			break;

		case IFM_VLAN_OAM_TYPE:
			sprintf (pAlarmIndex, "%d/%d                 ", usp.unit, VLAN_PortIndex_GetVlan(pLocalPort->portIndex));
			break;

		case IFM_CFM_MEP_TYPE:
			sprintf (pAlarmIndex, "%d/%d                 ", usp.unit, IFM_CFM_MEP_MEPID_DECOM(pLocalPort->portIndex));
			break;
			
		default:
			return -1;
	}

	vty_out (vty, "%s %s", show_buf, VTY_NEWLINE);
	
	return 0;
}



void gpnStatShowRegisterInfo(struct vty *vty)
{
	stStatPreScanQuen *pslotPerScanQuen;
	stPortTpToStatScanTp *pPortTpToScanTp;
	UINT32 sysPortTypeNum = 0;

	UINT32 perProtNum = 0;
	stStatPreScanIndex *pStatPreScanIndex;

	UINT32 slotId = 0;
	UINT32 hashValue = 0;

	vty_out (vty, "----------------------Register info-------------------%s", VTY_NEWLINE);
	vty_out (vty, "AlarmPort       Index                                 %s", VTY_NEWLINE);
	vty_out (vty, "--------------- --------------------------------------%s", VTY_NEWLINE);
	
	pPortTpToScanTp = (stPortTpToStatScanTp *)listFirst(
		&(pgstEQUStatProcSpace->PTpVsStatSTpRelation.PTpVsSTpQuen));
	while(  (pPortTpToScanTp != NULL) &&\
			(sysPortTypeNum < pgstEQUStatProcSpace->PTpVsStatSTpRelation.sysSupPortTpNum))
	{
		for(slotId=0;slotId<(EQU_SLOT_MAX_NUM + 1);slotId++)
		{
			pslotPerScanQuen = pPortTpToScanTp->pStatPreScanNode[slotId];
			/*add after*/
			for(hashValue=0;hashValue<(GPN_STAT_PRESCAN_HASH);hashValue++)
			{
				pStatPreScanIndex = (stStatPreScanIndex *)listFirst(&(pslotPerScanQuen->statPreScanQuen[hashValue]));
				while(pStatPreScanIndex != NULL)
				{
					gpnStatShowPortInfo(vty, &pStatPreScanIndex->pStatLocalPort->localPort);
					perProtNum++;
					pStatPreScanIndex = (stStatPreScanIndex *)listNext((NODE *)(pStatPreScanIndex));
				}
			}
		}
		
		sysPortTypeNum++;
		pPortTpToScanTp = (stPortTpToStatScanTp *)listNext((NODE *)(pPortTpToScanTp));
	}

	vty_out (vty, "--------------- --------------------------------------%s", VTY_NEWLINE);
	vty_out (vty, "Total number  : %d %s", perProtNum, VTY_NEWLINE);
	vty_out (vty, "------------------------------------------------------%s", VTY_NEWLINE);
}


