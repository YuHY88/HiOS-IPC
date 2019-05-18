/******************************************************************************
 * Filename: gpnStatSnmp.c
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2018.1.19  lipf created
 *
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>

#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/linklist.h>
#include <lib/inet_ip.h>
#include <lib/types.h>
#include <lib/pkt_buffer.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/syslog.h>

#include "lib/msg_ipc.h"
#include "lib/msg_ipc_n.h"
#include "lib/memshare.h"

#include "gpnStatSnmp.h"
#include "gpnStatSnmpApi.h"
#include "gpnStatDataStruct.h"
#include "gpnStatFuncApi.h"
#include "gpnStatDebug.h"


#include "socketComm/gpnSockStatMsgDef.h"
#include "socketComm/gpnGlobalPortIndexDef.h"

#define ALM_SNMP_ROW_ACTIVE 1
#define	IPC_BUF_LEN  16000

/*local temp variable*/
//static uchar 	str_value[STRING_LEN] = {'\0'};
//static uchar 	mac_value[6] = {0};
static int   	int_value = 0;
//static uint32_t ip_value = 0;
static uchar ipc_buf[IPC_BUF_LEN];


#if 0
static void stat_snmp_global_sub_type_attri_get(struct ipc_mesg *pmsg)
{
	struct ipc_msghdr *phdr = &pmsg->msghdr;
	stGloSubTypeAttribTab_index *pIndex = (stGloSubTypeAttribTab_index *)pmsg->msg_data;
	
	gpnSockMsg gSockMsgIn;	
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;	
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));
	
	gpnSockMsg *pgStatMsgSpOut = (gpnSockMsg *)XMALLOC(MTYPE_STAT, sizeof(gpnSockMsg) +
		sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred));
	memset (pgStatMsgSpOut, 0, sizeof (gpnSockMsg) + sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred));
	pgStatMsgSpOut->msgCellLen = sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred);

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg) + sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred);
	
	stGloSubTypeAttribTab_info tableInfo;
	memset (&tableInfo, 0, sizeof (stGloSubTypeAttribTab_info));

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= GPN_STAT_MSG_SUB_STAT_TYPE_GET;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pIndex->subTypeIndex;

	if(GPN_STAT_SNMP_GEN_OK == gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgStatMsgSpOut, outLen))
	{
		if (pgStatMsgSpOut->iMsgType == GPN_STAT_MSG_SUB_STAT_TYPE_GET_RSP)
		{
			tableInfo.index.subTypeIndex			= pgStatMsgSpOut->iMsgPara1;
			tableInfo.data.subTypeBelongScanType 	= pgStatMsgSpOut->iMsgPara2;
			tableInfo.data.subTypeValueClass 		= pgStatMsgSpOut->iMsgPara3;
			tableInfo.data.subTypeDataBitDeep 		= pgStatMsgSpOut->iMsgPara4;
			memcpy (&tableInfo.data.longCycUpThrRelatAlarm, &(pgStatMsgSpOut->msgCellLen) + 1,
				sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred));
			
			ipc_send_reply (&tableInfo, sizeof(stGloSubTypeAttribTab_info), phdr->sender_id, 
				phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
			
			XFREE(MTYPE_STAT, pgStatMsgSpOut);
			return;
		}
	}

	if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
					phdr->msg_subtype, phdr->msg_index) < 0)
	{
		zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
	}
	
	XFREE(MTYPE_STAT, pgStatMsgSpOut);
}

static void stat_snmp_global_sub_type_attri_set(struct ipc_mesg *pmsg)
{
	int mode = 0;
	memcpy(&mode, pmsg->msg_data, sizeof(int));
	stGloSubTypeAttribTab_index *pIndex = (stGloSubTypeAttribTab_index *)(pmsg->msg_data + sizeof(int));
		
	gpnSockMsg *pgSockMsgIn = (gpnSockMsg *)XMALLOC(MTYPE_STAT, sizeof(gpnSockMsg) + sizeof(stSubTypeThred));
	memset (pgSockMsgIn, 0, sizeof(gpnSockMsg) + sizeof(stSubTypeThred));
	pgSockMsgIn->msgCellLen = sizeof(stSubTypeThred);
	
	stGloSubTypeAttribTab_info tableInfo;
	memset (&tableInfo, 0, sizeof (stGloSubTypeAttribTab_info));
	memcpy(&tableInfo, (pmsg->msg_data + sizeof(int)), sizeof(stGloSubTypeAttribTab_info));

	if(mode == GPN_STAT_MSG_SUB_STAT_TYPE_MODIFY)
	{
		pgSockMsgIn->iIndex		= 0;
		pgSockMsgIn->iMsgType	= mode;
		pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
		pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
		pgSockMsgIn->iMsgPara1	= pIndex->subTypeIndex;
		pgSockMsgIn->msgCellLen = sizeof(stSubTypeThred);
		memcpy((&(pgSockMsgIn->msgCellLen)+1), &tableInfo.data.longCycUpThrHigh32, sizeof(stSubTypeThred));

		gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
	}

	XFREE(MTYPE_STAT, pgSockMsgIn);
}
#endif


#if 0
static void stat_snmp_port_moni_ctrl_get(struct ipc_mesg *pmsg)
{
	struct ipc_msghdr *phdr = &pmsg->msghdr;
	stPortMoniCtrlTab_index *pIndex = (stPortMoniCtrlTab_index *)pmsg->msg_data;

	gpnSockMsg gSockMsgIn;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;	
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));
	
	gpnSockMsg *pgStatMsgSpOut = (gpnSockMsg *)XMALLOC(MTYPE_STAT, sizeof(gpnSockMsg) +
		(sizeof(stStatMsgMonObjCfg) + sizeof(stStatObjDesc) + sizeof(UINT32)));
	memset (pgStatMsgSpOut, 0, sizeof (gpnSockMsg) + (sizeof(stStatMsgMonObjCfg) + sizeof(stStatObjDesc) + sizeof(UINT32)));
	pgStatMsgSpOut->msgCellLen = (sizeof(stStatMsgMonObjCfg) + sizeof(stStatObjDesc) + sizeof(UINT32));
	stStatMsgMonObjCfg *pstatMonObjCfg = (stStatMsgMonObjCfg *)(&(pgStatMsgSpOut->msgCellLen) + 1);

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg) + (sizeof(stStatMsgMonObjCfg) + sizeof(stStatObjDesc) + sizeof(UINT32));
	
	stPortMoniCtrlTab_info tableInfo;
	memset (&tableInfo, 0, sizeof (stPortMoniCtrlTab_info));

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType		= GPN_STAT_MSG_MON_GET;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->portMoniDevIndex;
	gSockMsgIn.iMsgPara2	= pIndex->portMoniIndex1;
	gSockMsgIn.iMsgPara3	= pIndex->portMoniIndex2;
	gSockMsgIn.iMsgPara4	= pIndex->portMoniIndex3;
	gSockMsgIn.iMsgPara5	= pIndex->portMoniIndex4;
	gSockMsgIn.iMsgPara6	= pIndex->portMoniIndex5;
	gSockMsgIn.iMsgPara7	= pIndex->portMoniScanType;

	if(GPN_STAT_SNMP_GEN_OK == gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgStatMsgSpOut, outLen))
	{
		if (pgStatMsgSpOut->iMsgType == GPN_STAT_MSG_MON_GET_NEXT_RSP)
		{
			memset (&tableInfo, 0, sizeof (stPortMoniCtrlTab_info));
			tableInfo.index.portMoniDevIndex	= pgStatMsgSpOut->iMsgPara1;
			tableInfo.index.portMoniIndex1		= pgStatMsgSpOut->iMsgPara2;
			tableInfo.index.portMoniIndex2		= pgStatMsgSpOut->iMsgPara3;
			tableInfo.index.portMoniIndex3		= pgStatMsgSpOut->iMsgPara4;
			tableInfo.index.portMoniIndex4		= pgStatMsgSpOut->iMsgPara5;
			tableInfo.index.portMoniIndex5		= pgStatMsgSpOut->iMsgPara6;
			tableInfo.index.portMoniScanType	= pgStatMsgSpOut->iMsgPara7;
			
			tableInfo.data.moniEn 						= pstatMonObjCfg->statMoniEn;
			tableInfo.data.currMoniEn 					= pstatMonObjCfg->currStatMoniEn;
			tableInfo.data.longCycMoniEn 				= pstatMonObjCfg->longCycStatMoniEn;
			tableInfo.data.longCycBelongTask 			= pstatMonObjCfg->longCycBelongTask;
			tableInfo.data.longCycEvtThrLocation 		= pstatMonObjCfg->longCycEvnThredTpId;
			tableInfo.data.longCycSubTypeFiltLocation 	= pstatMonObjCfg->longCycSubFiltTpId;
			tableInfo.data.longCycAlmThrLocation 		= pstatMonObjCfg->longCycAlmThredTpId;
			tableInfo.data.longCycHistAutoReport 		= pstatMonObjCfg->longCycHistReptTpId;
			tableInfo.data.longCycHistLocation 			= pstatMonObjCfg->longCycHistDBId;
			tableInfo.data.shortCycMoniEn 				= pstatMonObjCfg->shortCycStatMoniEn;
			tableInfo.data.shortCycBelongTask 			= pstatMonObjCfg->shortCycBelongTask;
			tableInfo.data.shortCycEvtThrLocation 		= pstatMonObjCfg->shortCycEvnThredTpId;
			tableInfo.data.shortCycSubTypeFiltLocation 	= pstatMonObjCfg->shortCycSubFiltTpId;
			tableInfo.data.shortCycAlmThrLocation 		= pstatMonObjCfg->shortCycAlmThredTpId;
			tableInfo.data.shortCycHistAutoReport 		= pstatMonObjCfg->shortCycHistReptTpId;
			tableInfo.data.shortCycHistLocation 		= pstatMonObjCfg->shortCycHistDBId;
			tableInfo.data.usrDefCycMoniEn 				= pstatMonObjCfg->udCycStatMoniEn;
			tableInfo.data.usrDefCycBelongTask 			= pstatMonObjCfg->udCycBelongTask;
			tableInfo.data.usrDefCycEvtThrLocation 		= pstatMonObjCfg->udCycEvnThredTpId;
			tableInfo.data.usrDefCycSubTypeFiltLocation = pstatMonObjCfg->udCycSubFiltTpId;
			tableInfo.data.usrDefCycAlmThrLocation 		= pstatMonObjCfg->udCycAlmThredTpId;
			tableInfo.data.usrDefCycHistAutoReport 		= pstatMonObjCfg->udCycHistReptTpId;
			tableInfo.data.usrDefCycHistLocation 		= pstatMonObjCfg->udCycHistDBId;
			tableInfo.data.usrDefCycHistSeconds 		= pstatMonObjCfg->udCycSecs;
			tableInfo.data.rowStatus 					= ALM_SNMP_ROW_ACTIVE;

			zlog_debug(0, "%s[%d] : find port monitor\n", __func__, __LINE__);
			ipc_send_reply (&tableInfo, sizeof(stPortMoniCtrlTab_info), phdr->sender_id, 
				phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
			XFREE(MTYPE_STAT, pgStatMsgSpOut);
			return;
		}
	}	

	zlog_debug(0, "%s[%d] : not find port monitor\n", __func__, __LINE__);
	if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
					phdr->msg_subtype, phdr->msg_index) < 0)
	{
		zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
	}
					
	XFREE(MTYPE_STAT, pgStatMsgSpOut);
}
#endif

static void stat_snmp_port_moni_ctrl_get (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stPortMoniCtrlTab_index *pIndex = (stPortMoniCtrlTab_index *)pmsg->msg_data;

	int inLen = sizeof(gpnSockMsg);
	int outLen = sizeof(gpnSockMsg) + (sizeof(stStatMsgMonObjCfg) + sizeof(stStatObjDesc) + sizeof(UINT32));

	gpnSockMsg gSockMsgIn;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	memset (pgSockMsgIn, 0, inLen);
	
	gpnSockMsg *pgSockMsgOut = (gpnSockMsg *)XMALLOC(MTYPE_STAT, outLen);
	memset (pgSockMsgOut, 0, outLen);
	
	pgSockMsgOut->msgCellLen = (outLen - sizeof(gpnSockMsg));
	stStatMsgMonObjCfg *pstatMonObjCfg = (stStatMsgMonObjCfg *)(&(pgSockMsgOut->msgCellLen) + 1);
	stStatObjDesc *pstatObjDesc = (stStatObjDesc *)(pstatMonObjCfg + 1);
	uint32_t *pscanType = (uint32_t *)(pstatObjDesc + 1);

	stPortMoniCtrlTab_info tableInfo;
	memset (&tableInfo, 0, sizeof (stPortMoniCtrlTab_info));

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= GPN_STAT_MSG_MON_GET_NEXT;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= GPN_ILLEGAL_DEVICE_INDEX;
	pgSockMsgIn->iMsgPara2	= GPN_ILLEGAL_PORT_INDEX;
	pgSockMsgIn->iMsgPara3	= GPN_ILLEGAL_PORT_INDEX;
	pgSockMsgIn->iMsgPara4	= GPN_ILLEGAL_PORT_INDEX;
	pgSockMsgIn->iMsgPara5	= GPN_ILLEGAL_PORT_INDEX;
	pgSockMsgIn->iMsgPara6	= GPN_ILLEGAL_PORT_INDEX;
	pgSockMsgIn->iMsgPara7	= GPN_ILLEGAL_PORT_INDEX;

	uint32_t ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while (GPN_STAT_SNMP_GEN_OK == ret)
	{
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_MON_GET_NEXT_RSP)
		{
			if((pIndex->portMoniDevIndex != pstatObjDesc->devIndex) \
				|| (pIndex->portMoniIndex1 != pstatObjDesc->portIndex) \
				|| (pIndex->portMoniIndex2 != pstatObjDesc->portIndex3) \
				|| (pIndex->portMoniIndex3 != pstatObjDesc->portIndex4) \
				|| (pIndex->portMoniIndex4 != pstatObjDesc->portIndex5) \
				|| (pIndex->portMoniIndex5 != pstatObjDesc->portIndex6) \
				|| (pIndex->portMoniScanType != *pscanType))
			{
				/* search done */
				if((IFM_PORT_TYPE_INVALID == pstatObjDesc->devIndex) \
					&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex) \
					&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex3) \
					&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex4) \
					&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex5) \
					&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex6) \
					&&(GPN_STAT_SCAN_TYPE_ALL == *pscanType))		
				{
					break;
				}
					
				memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));			
			
				pgSockMsgIn->iIndex		= 0;
				pgSockMsgIn->iMsgType	= GPN_STAT_MSG_MON_GET_NEXT;
				pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
				pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
				pgSockMsgIn->iMsgPara1	= pstatObjDesc->devIndex;
				pgSockMsgIn->iMsgPara2	= pstatObjDesc->portIndex;
				pgSockMsgIn->iMsgPara3	= pstatObjDesc->portIndex3;
				pgSockMsgIn->iMsgPara4	= pstatObjDesc->portIndex4;
				pgSockMsgIn->iMsgPara5	= pstatObjDesc->portIndex5;
				pgSockMsgIn->iMsgPara6	= pstatObjDesc->portIndex6;
				pgSockMsgIn->iMsgPara7	= *pscanType;

				memset (pgSockMsgOut, 0, outLen);
				pgSockMsgOut->msgCellLen = (outLen - sizeof(gpnSockMsg));
				
				ret = gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			}
			else
			{
				memset (&tableInfo, 0, sizeof (stPortMoniCtrlTab_info));
				tableInfo.index.portMoniDevIndex	= pgSockMsgOut->iMsgPara1;
				tableInfo.index.portMoniIndex1		= pgSockMsgOut->iMsgPara2;
				tableInfo.index.portMoniIndex2		= pgSockMsgOut->iMsgPara3;
				tableInfo.index.portMoniIndex3		= pgSockMsgOut->iMsgPara4;
				tableInfo.index.portMoniIndex4		= pgSockMsgOut->iMsgPara5;
				tableInfo.index.portMoniIndex5		= pgSockMsgOut->iMsgPara6;
				tableInfo.index.portMoniScanType	= pgSockMsgOut->iMsgPara7;
				
				tableInfo.data.moniEn 						= pstatMonObjCfg->statMoniEn;
				tableInfo.data.currMoniEn 					= pstatMonObjCfg->currStatMoniEn;
				tableInfo.data.longCycMoniEn 				= pstatMonObjCfg->longCycStatMoniEn;
				tableInfo.data.longCycBelongTask 			= pstatMonObjCfg->longCycBelongTask;
				tableInfo.data.longCycEvtThrLocation 		= pstatMonObjCfg->longCycEvnThredTpId;
				tableInfo.data.longCycSubTypeFiltLocation 	= pstatMonObjCfg->longCycSubFiltTpId;
				tableInfo.data.longCycAlmThrLocation 		= pstatMonObjCfg->longCycAlmThredTpId;
				tableInfo.data.longCycHistAutoReport 		= pstatMonObjCfg->longCycHistReptTpId;
				tableInfo.data.longCycHistLocation 			= pstatMonObjCfg->longCycHistDBId;
				tableInfo.data.shortCycMoniEn 				= pstatMonObjCfg->shortCycStatMoniEn;
				tableInfo.data.shortCycBelongTask 			= pstatMonObjCfg->shortCycBelongTask;
				tableInfo.data.shortCycEvtThrLocation 		= pstatMonObjCfg->shortCycEvnThredTpId;
				tableInfo.data.shortCycSubTypeFiltLocation 	= pstatMonObjCfg->shortCycSubFiltTpId;
				tableInfo.data.shortCycAlmThrLocation 		= pstatMonObjCfg->shortCycAlmThredTpId;
				tableInfo.data.shortCycHistAutoReport 		= pstatMonObjCfg->shortCycHistReptTpId;
				tableInfo.data.shortCycHistLocation 		= pstatMonObjCfg->shortCycHistDBId;
				tableInfo.data.usrDefCycMoniEn 				= pstatMonObjCfg->udCycStatMoniEn;
				tableInfo.data.usrDefCycBelongTask 			= pstatMonObjCfg->udCycBelongTask;
				tableInfo.data.usrDefCycEvtThrLocation 		= pstatMonObjCfg->udCycEvnThredTpId;
				tableInfo.data.usrDefCycSubTypeFiltLocation = pstatMonObjCfg->udCycSubFiltTpId;
				tableInfo.data.usrDefCycAlmThrLocation 		= pstatMonObjCfg->udCycAlmThredTpId;
				tableInfo.data.usrDefCycHistAutoReport 		= pstatMonObjCfg->udCycHistReptTpId;
				tableInfo.data.usrDefCycHistLocation 		= pstatMonObjCfg->udCycHistDBId;
				tableInfo.data.usrDefCycHistSeconds 		= pstatMonObjCfg->udCycSecs;
				tableInfo.data.rowStatus 					= ALM_SNMP_ROW_ACTIVE;

				/*ipc_send_reply (&tableInfo, sizeof(stPortMoniCtrlTab_info), phdr->sender_id, 
					phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);*/

				ipc_ack_to_snmp(phdr, (void *)&tableInfo, sizeof(stPortMoniCtrlTab_info), 1);
				
				XFREE(MTYPE_STAT, pgSockMsgOut);
				return;
			}
		}
		else
			break;
	}

	/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
					phdr->msg_subtype, phdr->msg_index) < 0)
	{
		zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
	}*/

	ipc_noack_to_snmp(phdr);
					
	XFREE(MTYPE_STAT, pgSockMsgOut);
}


static void stat_snmp_port_moni_ctrl_set(struct ipc_mesg_n *pmsg)
{
	int mode = 0;
	memcpy(&mode, pmsg->msg_data, sizeof(int));

	int flag_set = 0;
	memcpy(&flag_set, (pmsg->msg_data + sizeof(int)), sizeof(int));
	
	stPortMoniCtrlTab_info *pInfo = (stPortMoniCtrlTab_info *)(pmsg->msg_data + sizeof(int) + sizeof(int));
	
	gpnSockMsg gSockMsgIn;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= mode;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pInfo->index.portMoniDevIndex;
	pgSockMsgIn->iMsgPara2	= pInfo->index.portMoniIndex1;
	pgSockMsgIn->iMsgPara3	= pInfo->index.portMoniIndex2;
	pgSockMsgIn->iMsgPara4	= pInfo->index.portMoniIndex3;
	pgSockMsgIn->iMsgPara5	= pInfo->index.portMoniIndex4;
	pgSockMsgIn->iMsgPara6	= pInfo->index.portMoniIndex5;
	pgSockMsgIn->iMsgPara7	= pInfo->index.portMoniScanType;
	pgSockMsgIn->msgCellLen = 0;

	switch(pmsg->msghdr.opcode)
	{
		case IPC_OPCODE_ADD:
			zlog_debug(STATIS_DBG_SET, "%s[%d] : add statPortMoniCtrlTable : (%x:%x:%x:%x:%x:%x:%x)\n\r", __FUNCTION__, __LINE__,\
				pInfo->index.portMoniDevIndex, pInfo->index.portMoniIndex1, pInfo->index.portMoniIndex2,
				pInfo->index.portMoniIndex3, pInfo->index.portMoniIndex4, pInfo->index.portMoniIndex5,
				pInfo->index.portMoniScanType);
			pgSockMsgIn->iMsgType  = GPN_STAT_MSG_MON_ADD;			
			gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));

			pgSockMsgIn->iMsgType  = GPN_STAT_MSG_BASE_MON_OPT;
			pgSockMsgIn->iMsgPara8 = pInfo->data.moniEn;
			pgSockMsgIn->iMsgPara9 = pInfo->data.currMoniEn;			
			gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));

			pgSockMsgIn->iMsgType  = GPN_STAT_MSG_PORT_CYC_MON_OPT;
			if(flag_set & PORT_MONI_PORT_LCYC_MON_OPT)		//set long
			{
				pgSockMsgIn->iMsgPara8 = pInfo->data.longCycBelongTask;
				pgSockMsgIn->iMsgPara9 = pInfo->data.longCycMoniEn;
				pgSockMsgIn->iMsgParaA = pInfo->data.longCycEvtThrLocation;
				pgSockMsgIn->iMsgParaB = pInfo->data.longCycSubTypeFiltLocation;
				pgSockMsgIn->iMsgParaC = pInfo->data.longCycHistAutoReport;

				gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
			}
			else if(flag_set & PORT_MONI_PORT_SCYC_MON_OPT)		//set short
			{
				pgSockMsgIn->iMsgPara8 = pInfo->data.shortCycBelongTask;
				pgSockMsgIn->iMsgPara9 = pInfo->data.shortCycMoniEn;
				pgSockMsgIn->iMsgParaA = pInfo->data.shortCycEvtThrLocation;
				pgSockMsgIn->iMsgParaB = pInfo->data.shortCycSubTypeFiltLocation;
				pgSockMsgIn->iMsgParaC = pInfo->data.shortCycHistAutoReport;

				gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
			}
			else if(flag_set & PORT_MONI_PORT_UCYC_MON_OPT)		//set userDef
			{
				pgSockMsgIn->iMsgPara8 = pInfo->data.usrDefCycBelongTask;
				pgSockMsgIn->iMsgPara9 = pInfo->data.usrDefCycMoniEn;
				pgSockMsgIn->iMsgParaA = pInfo->data.usrDefCycEvtThrLocation;
				pgSockMsgIn->iMsgParaB = pInfo->data.usrDefCycSubTypeFiltLocation;
				pgSockMsgIn->iMsgParaC = pInfo->data.usrDefCycHistAutoReport;

				gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
			}
			break;

		case IPC_OPCODE_UPDATE:
			pgSockMsgIn->iMsgType  = GPN_STAT_MSG_BASE_MON_OPT;
			pgSockMsgIn->iMsgPara8 = pInfo->data.moniEn;
			pgSockMsgIn->iMsgPara9 = pInfo->data.currMoniEn;			
			gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));

			pgSockMsgIn->iMsgType  = GPN_STAT_MSG_PORT_CYC_MON_OPT;
			if(flag_set & PORT_MONI_PORT_LCYC_MON_OPT)		//set long
			{
				pgSockMsgIn->iMsgPara8 = pInfo->data.longCycBelongTask;
				pgSockMsgIn->iMsgPara9 = pInfo->data.longCycMoniEn;
				pgSockMsgIn->iMsgParaA = pInfo->data.longCycEvtThrLocation;
				pgSockMsgIn->iMsgParaB = pInfo->data.longCycSubTypeFiltLocation;
				pgSockMsgIn->iMsgParaC = pInfo->data.longCycHistAutoReport;

				gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
			}
			if(flag_set & PORT_MONI_PORT_SCYC_MON_OPT)		//set short
			{
				pgSockMsgIn->iMsgPara8 = pInfo->data.shortCycBelongTask;
				pgSockMsgIn->iMsgPara9 = pInfo->data.shortCycMoniEn;
				pgSockMsgIn->iMsgParaA = pInfo->data.shortCycEvtThrLocation;
				pgSockMsgIn->iMsgParaB = pInfo->data.shortCycSubTypeFiltLocation;
				pgSockMsgIn->iMsgParaC = pInfo->data.shortCycHistAutoReport;

				gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
			}
			if(flag_set & PORT_MONI_PORT_UCYC_MON_OPT)		//set userDef
			{
				pgSockMsgIn->iMsgPara8 = pInfo->data.usrDefCycBelongTask;
				pgSockMsgIn->iMsgPara9 = pInfo->data.usrDefCycMoniEn;
				pgSockMsgIn->iMsgParaA = pInfo->data.usrDefCycEvtThrLocation;
				pgSockMsgIn->iMsgParaB = pInfo->data.usrDefCycSubTypeFiltLocation;
				pgSockMsgIn->iMsgParaC = pInfo->data.usrDefCycHistAutoReport;

				gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
			}
			break;

		case IPC_OPCODE_DELETE:
			pgSockMsgIn->iMsgType  = GPN_STAT_MSG_MON_DELETE;
			gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
			break;

		default:
			break;
	}

	zlog_debug(STATIS_DBG_SET, "%s[%d] : mode(%d), flag_set(%d)\n", __func__, __LINE__,
		mode, flag_set);
	if(mode == GPN_STAT_MSG_PORT_CYC_MON_OPT)
	{
		if(flag_set & PORT_MONI_PORT_LCYC_MON_OPT)		//set long
		{
			pgSockMsgIn->iMsgPara8 = pInfo->data.longCycBelongTask;
			pgSockMsgIn->iMsgPara9 = pInfo->data.longCycMoniEn;
			pgSockMsgIn->iMsgParaA = pInfo->data.longCycEvtThrLocation;
			pgSockMsgIn->iMsgParaB = pInfo->data.longCycSubTypeFiltLocation;
			pgSockMsgIn->iMsgParaC = pInfo->data.longCycHistAutoReport;

			gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
		}

		if(flag_set & PORT_MONI_PORT_SCYC_MON_OPT)		//set short
		{
			pgSockMsgIn->iMsgPara8 = pInfo->data.shortCycBelongTask;
			pgSockMsgIn->iMsgPara9 = pInfo->data.shortCycMoniEn;
			pgSockMsgIn->iMsgParaA = pInfo->data.shortCycEvtThrLocation;
			pgSockMsgIn->iMsgParaB = pInfo->data.shortCycSubTypeFiltLocation;
			pgSockMsgIn->iMsgParaC = pInfo->data.shortCycHistAutoReport;

			gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
		}

		if(flag_set & PORT_MONI_PORT_UCYC_MON_OPT)		//set userDef
		{
			pgSockMsgIn->iMsgPara8 = pInfo->data.usrDefCycBelongTask;
			pgSockMsgIn->iMsgPara9 = pInfo->data.usrDefCycMoniEn;
			pgSockMsgIn->iMsgParaA = pInfo->data.usrDefCycEvtThrLocation;
			pgSockMsgIn->iMsgParaB = pInfo->data.usrDefCycSubTypeFiltLocation;
			pgSockMsgIn->iMsgParaC = pInfo->data.usrDefCycHistAutoReport;

			gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
		}
	}
	else if(mode == GPN_STAT_MSG_BASE_MON_OPT)
	{
		pgSockMsgIn->iMsgPara8 = pInfo->data.moniEn;
		pgSockMsgIn->iMsgPara9 = pInfo->data.currMoniEn;
		
		gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
	}
	else if((mode == GPN_STAT_MSG_MON_ADD) || (mode == GPN_STAT_MSG_MON_DELETE))
	{
		gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
	}
}

static void stat_snmp_curr_stat_get(struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stCurrStatTab_index *pIndex = (stCurrStatTab_index *)pmsg->msg_data;
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (pgSockMsgOut, 0, sizeof (gpnSockMsg));

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);

	stCurrStatTab_info tableInfo;
	memset (&tableInfo, 0, sizeof (stCurrStatTab_info));

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType 	= GPN_STAT_MSG_CURR_DATA_GET;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->currDevIndex;
	gSockMsgIn.iMsgPara2	= pIndex->currPortIndex1;
	gSockMsgIn.iMsgPara3	= pIndex->currPortIndex2;
	gSockMsgIn.iMsgPara4	= pIndex->currPortIndex3;
	gSockMsgIn.iMsgPara5	= pIndex->currPortIndex4;
	gSockMsgIn.iMsgPara6	= pIndex->currPortIndex5;
	gSockMsgIn.iMsgPara7	= pIndex->currDataType;

	if(GPN_STAT_SNMP_GEN_OK == gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen))
	{
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_CURR_DATA_GET_RSP)
		{
			memset (&tableInfo, 0, sizeof (stCurrStatTab_info));
			tableInfo.index.currDevIndex	= pgSockMsgOut->iMsgPara1;
			tableInfo.index.currPortIndex1	= pgSockMsgOut->iMsgPara2;
			tableInfo.index.currPortIndex2	= pgSockMsgOut->iMsgPara3;
			tableInfo.index.currPortIndex3	= pgSockMsgOut->iMsgPara4;
			tableInfo.index.currPortIndex4	= pgSockMsgOut->iMsgPara5;
			tableInfo.index.currPortIndex5	= pgSockMsgOut->iMsgPara6;
			tableInfo.index.currDataType	= pgSockMsgOut->iMsgPara7;
			
			tableInfo.data.clearCurrData			= pgSockMsgOut->iMsgPara8;
			tableInfo.data.currDataTypeValHigh32	= pgSockMsgOut->iMsgPara9;
			tableInfo.data.currDataTypeValLow32 	= pgSockMsgOut->iMsgParaA;
			tableInfo.data.rowStatus				= ALM_SNMP_ROW_ACTIVE;

			/*ipc_send_reply (&tableInfo, sizeof(stCurrStatTab_info), phdr->sender_id, 
				phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);*/

			ipc_ack_to_snmp(phdr, (void *)&tableInfo, sizeof(stCurrStatTab_info), 1);
			
			return;
		}
	}
	
	/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
					phdr->msg_subtype, phdr->msg_index) < 0)
	{
		zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
	}*/

	ipc_noack_to_snmp(phdr);
}


static void stat_snmp_curr_stat_set(struct ipc_mesg_n *pmsg)
{
	int mode = 0;
	memcpy(&mode, pmsg->msg_data, sizeof(int));
	stCurrStatTab_info *pInfo = (stCurrStatTab_info *)(pmsg->msg_data + sizeof(int));
	
	gpnSockMsg gSockMsgIn;	
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= mode;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pInfo->index.currDevIndex;
	pgSockMsgIn->iMsgPara2	= pInfo->index.currPortIndex1;
	pgSockMsgIn->iMsgPara3	= pInfo->index.currPortIndex2;
	pgSockMsgIn->iMsgPara4	= pInfo->index.currPortIndex3;
	pgSockMsgIn->iMsgPara5	= pInfo->index.currPortIndex4;
	pgSockMsgIn->iMsgPara6	= pInfo->index.currPortIndex5;
	pgSockMsgIn->iMsgPara7	= pInfo->index.currDataType;
	pgSockMsgIn->iMsgPara8	= pInfo->data.clearCurrData;
	pgSockMsgIn->msgCellLen = 0;	

	gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
}

static void stat_snmp_task_get(struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stStatTaskTab_index *pIndex = (stStatTaskTab_index *)pmsg->msg_data;
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (pgSockMsgOut, 0, sizeof (gpnSockMsg));

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);
	
	stStatTaskTab_info tableInfo;
	memset (&tableInfo, 0, sizeof (stStatTaskTab_info));

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType 	= GPN_STAT_MSG_TASK_GET;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->taskIndex;
	
	if(GPN_STAT_SNMP_GEN_OK == gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen))
	{
		if (gSockMsgOut.iMsgType == GPN_STAT_MSG_TASK_GET_RSP)
		{
			memset (&tableInfo, 0, sizeof (stStatTaskTab_info));
			tableInfo.index.taskIndex			= gSockMsgOut.iMsgPara1;
			tableInfo.data.maxPortInTask		= gSockMsgOut.iMsgParaA;
			tableInfo.data.taskScanType 		= gSockMsgOut.iMsgPara2;
			tableInfo.data.includeMonObjNum 	= gSockMsgOut.iMsgPara3;
			tableInfo.data.taskCycMoniType		= gSockMsgOut.iMsgPara4;
			tableInfo.data.taskMoniCycSec		= gSockMsgOut.iMsgPara5;
			tableInfo.data.taskStartTime		= gSockMsgOut.iMsgPara6;
			tableInfo.data.taskEndTime			= gSockMsgOut.iMsgPara7;
			tableInfo.data.taskAlmThrTpLocation = gSockMsgOut.iMsgPara8;
			tableInfo.data.taskSubReportLocation= gSockMsgOut.iMsgPara9;
			tableInfo.data.rowStatus			= ALM_SNMP_ROW_ACTIVE;

			/*ipc_send_reply (&tableInfo, sizeof(stStatTaskTab_info), phdr->sender_id, 
				phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);*/

			ipc_ack_to_snmp(phdr, (void *)&tableInfo, sizeof(stStatTaskTab_info), 1);
			return;
		}
	}
	
	/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
					phdr->msg_subtype, phdr->msg_index) < 0)
	{
		zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
	}*/
	
	ipc_noack_to_snmp(phdr);
}


static void stat_snmp_task_set(struct ipc_mesg_n *pmsg)
{
	int mode = 0;
	memcpy(&mode, pmsg->msg_data, sizeof(int));
	stStatTaskTab_info *pInfo = (stStatTaskTab_info *)(pmsg->msg_data + sizeof(int));
	
	gpnSockMsg gSockMsgIn;	
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));

	zlog_debug(STATIS_DBG_SET, "%s[%d] : mode[%x:ADM(%x|%x|%x)], index(%d)\n", __func__, __LINE__,
		mode,
		GPN_STAT_MSG_TASK_ADD,
		GPN_STAT_MSG_TASK_DELETE,
		GPN_STAT_MSG_TASK_MODIFY,
		pInfo->index.taskIndex);

#if 0
	/*增加行时，使用分配的id，否则配置不下去*/
	uint32_t taskIndex = 0;	
	if(IPC_OPCODE_ADD == pmsg->msghdr.opcode)
	{
		taskIndex = gpnStatTaskIdGet();
	}
	else
	{
		taskIndex = pInfo->index.taskIndex;
	}
#endif

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= mode;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pInfo->index.taskIndex;
	//pgSockMsgIn->iMsgPara1	= taskIndex;
	pgSockMsgIn->iMsgPara2	= pInfo->data.taskScanType;
	pgSockMsgIn->iMsgPara3	= pInfo->data.maxPortInTask;
	pgSockMsgIn->iMsgPara4	= pInfo->data.taskCycMoniType;
	pgSockMsgIn->iMsgPara5	= pInfo->data.taskMoniCycSec;
	pgSockMsgIn->iMsgPara6	= pInfo->data.taskStartTime;
	pgSockMsgIn->iMsgPara7	= pInfo->data.taskEndTime;
	pgSockMsgIn->iMsgPara8	= pInfo->data.taskAlmThrTpLocation;
	pgSockMsgIn->iMsgPara9	= pInfo->data.taskSubReportLocation;
	pgSockMsgIn->msgCellLen = 0;	

	gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
}

static void stat_snmp_evt_td_tp_get(struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stEvtThrTmpTab_index *pIndex = (stEvtThrTmpTab_index *)pmsg->msg_data;
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (pgSockMsgOut, 0, sizeof (gpnSockMsg));

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);

	stEvtThrTmpTab_info tableInfo;
	memset (&tableInfo, 0, sizeof (stEvtThrTmpTab_info));

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType 	= GPN_STAT_MSG_EVN_THRED_TEMP_GET;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->evtThrTmpIndex;
	gSockMsgIn.iMsgPara2	= pIndex->evtThrTmpTpSubType;

	if(GPN_STAT_SNMP_GEN_OK == gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen))
	{
		if (gSockMsgOut.iMsgType == GPN_STAT_MSG_EVN_THRED_TEMP_GET_RSP)
		{
			memset (&tableInfo, 0, sizeof (stEvtThrTmpTab_info));
			tableInfo.index.evtThrTmpIndex				= gSockMsgOut.iMsgPara1;
			tableInfo.index.evtThrTmpTpSubType			= gSockMsgOut.iMsgPara2;
			tableInfo.data.evtThrScanType				= gSockMsgOut.iMsgPara3;
			tableInfo.data.evtThrTpSubTypeUpThrHigh32	= gSockMsgOut.iMsgPara4;
			tableInfo.data.evtThrTpSubTypeUpThrLow32	= gSockMsgOut.iMsgPara5;
			tableInfo.data.evtThrTpSubTypeLowThrHigh32	= gSockMsgOut.iMsgPara6;
			tableInfo.data.evtThrTpSubTypeLowThrLow32	= gSockMsgOut.iMsgPara7;
			tableInfo.data.rowStatus					= ALM_SNMP_ROW_ACTIVE;

			zlog_debug(STATIS_DBG_GET, "%s[%d] : get event table success\n", __func__, __LINE__);
			/*ipc_send_reply (&tableInfo, sizeof(stEvtThrTmpTab_info), phdr->sender_id, 
				phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);*/

			ipc_ack_to_snmp(phdr, (void *)&tableInfo, sizeof(stEvtThrTmpTab_info), 1);

			return;
		}
	}

	zlog_debug(STATIS_DBG_GET, "%s[%d] : get event table failed\n", __func__, __LINE__);
	/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
					phdr->msg_subtype, phdr->msg_index) < 0)
	{
		zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
	}*/

	ipc_noack_to_snmp(phdr);
}


static void stat_snmp_evt_td_tp_set(struct ipc_mesg_n *pmsg)
{
	int mode = 0;
	memcpy(&mode, pmsg->msg_data, sizeof(int));
	stEvtThrTmpTab_info *pInfo = (stEvtThrTmpTab_info *)(pmsg->msg_data + sizeof(int));
	
	gpnSockMsg gSockMsgIn;	
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));

	zlog_debug(STATIS_DBG_SET, "%s[%d] : mode[%x:ADM(%x|%x|%x)], index(%d|%x)\n", __func__, __LINE__,
		mode,
		GPN_STAT_MSG_EVN_THRED_TEMP_ADD,
		GPN_STAT_MSG_EVN_THRED_TEMP_DELETE,
		GPN_STAT_MSG_EVN_THRED_TEMP_MODIFY,
		pInfo->index.evtThrTmpIndex, pInfo->index.evtThrTmpTpSubType);

#if 0
	/*增加行时，使用分配的id，否则配置不下去*/
	uint32_t evtThrTmpIndex = 0;	
	if(IPC_OPCODE_ADD == pmsg->msghdr.opcode)
	{
		evtThrTmpIndex = gpnStatEventThredTemplatIdGet();
	}
	else
	{
		evtThrTmpIndex = pInfo->index.evtThrTmpIndex;
	}
#endif
	
	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= mode;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pInfo->index.evtThrTmpIndex;
	//pgSockMsgIn->iMsgPara1	= evtThrTmpIndex;
	pgSockMsgIn->iMsgPara2	= pInfo->index.evtThrTmpTpSubType;
	pgSockMsgIn->iMsgPara3	= pInfo->data.evtThrScanType;
	pgSockMsgIn->iMsgPara4	= pInfo->data.evtThrTpSubTypeUpThrHigh32;
	pgSockMsgIn->iMsgPara5	= pInfo->data.evtThrTpSubTypeUpThrLow32;
	pgSockMsgIn->iMsgPara6	= pInfo->data.evtThrTpSubTypeLowThrHigh32;
	pgSockMsgIn->iMsgPara7	= pInfo->data.evtThrTpSubTypeLowThrLow32;
	pgSockMsgIn->msgCellLen = 0;	

	gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
}

static void stat_snmp_sub_type_filt_tp_get(struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stSubStatTypeFileTpTab_index *pIndex = (stSubStatTypeFileTpTab_index *)pmsg->msg_data;
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (pgSockMsgOut, 0, sizeof (gpnSockMsg));

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);

	stSubStatTypeFileTpTab_info tableInfo;
	memset (&tableInfo, 0, sizeof (stSubStatTypeFileTpTab_info));

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType 	= GPN_STAT_MSG_SUBFILT_TEMP_GET;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->subTypeFiltIndex;
	gSockMsgIn.iMsgPara2	= pIndex->selectSubType;

	if(GPN_STAT_SNMP_GEN_OK == gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen))
	{
		if (gSockMsgOut.iMsgType == GPN_STAT_MSG_SUBFILT_TEMP_GET_RSP)
		{
			memset (&tableInfo, 0, sizeof (stSubStatTypeFileTpTab_info));
			tableInfo.index.subTypeFiltIndex = gSockMsgOut.iMsgPara1;
			tableInfo.index.selectSubType	 = gSockMsgOut.iMsgPara2;
			tableInfo.data.selectScanType	 = gSockMsgOut.iMsgPara3;
			tableInfo.data.selectStatus 	 = gSockMsgOut.iMsgPara4;
			tableInfo.data.rowStatus		 = ALM_SNMP_ROW_ACTIVE;

			/*ipc_send_reply (&tableInfo, sizeof(stSubStatTypeFileTpTab_info), phdr->sender_id, 
				phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);*/

			ipc_ack_to_snmp(phdr, (void *)&tableInfo, sizeof(stSubStatTypeFileTpTab_info), 1);
			
			return;
		}
	}

	/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
					phdr->msg_subtype, phdr->msg_index) < 0)
	{
		zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
	}*/

	ipc_noack_to_snmp(phdr);
}


static void stat_snmp_sub_type_filt_tp_set(struct ipc_mesg_n *pmsg)
{
	int mode = 0;
	memcpy(&mode, pmsg->msg_data, sizeof(int));
	stSubStatTypeFileTpTab_info *pInfo = (stSubStatTypeFileTpTab_info *)(pmsg->msg_data + sizeof(int));
	
	gpnSockMsg gSockMsgIn;	
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));

#if 0
	/*增加行时，使用分配的id，否则配置不下去*/
	uint32_t subTypeFiltIndex = 0;
	if(IPC_OPCODE_ADD == pmsg->msghdr.opcode)
	{
		subTypeFiltIndex = gpnStatSubTypeFiltIdGet();
	}
	else
	{
		subTypeFiltIndex = pInfo->index.subTypeFiltIndex;
	}
#endif
	zlog_debug(STATIS_DBG_SET, "%s[%d] : opcode(%d:%d|%d|%d)\n", __func__, __LINE__,
		pmsg->msghdr.opcode, IPC_OPCODE_ADD, IPC_OPCODE_UPDATE, IPC_OPCODE_DELETE);

	zlog_debug(STATIS_DBG_SET, "%s[%d] : mode[%x:ADM(%x|%x|%x)], index(%d|%x)\n", __func__, __LINE__,
		mode,
		GPN_STAT_MSG_SUBFILT_TEMP_ADD,
		GPN_STAT_MSG_SUBFILT_TEMP_DELETE,
		GPN_STAT_MSG_SUBFILT_TEMP_MODIFY,
		pInfo->index.subTypeFiltIndex, pInfo->index.selectSubType);

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= mode;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pInfo->index.subTypeFiltIndex;
	//pgSockMsgIn->iMsgPara1	= subTypeFiltIndex;
	pgSockMsgIn->iMsgPara2	= pInfo->index.selectSubType;
	pgSockMsgIn->iMsgPara3	= pInfo->data.selectScanType;
	pgSockMsgIn->iMsgPara4	= pInfo->data.selectStatus;
	pgSockMsgIn->msgCellLen = 0;	

	gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
}

static void stat_snmp_alm_td_tp_get(struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stAlmThrTmpTab_index *pIndex = (stAlmThrTmpTab_index *)pmsg->msg_data;
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (pgSockMsgOut, 0, sizeof (gpnSockMsg));

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);

	stAlmThrTmpTab_info tableInfo;
	memset (&tableInfo, 0, sizeof(stAlmThrTmpTab_info));

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType 	= GPN_STAT_MSG_ALM_THRED_TEMP_GET;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->almThrTmpIndex;
	gSockMsgIn.iMsgPara2	= pIndex->almThrTmpTpSubType;

	if(GPN_STAT_SNMP_GEN_OK == gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen))
	{
		if (gSockMsgOut.iMsgType == GPN_STAT_MSG_ALM_THRED_TEMP_GET_RSP)
		{
			memset (&tableInfo, 0, sizeof(stSubStatTypeFileTpTab_info));
			tableInfo.index.almThrTmpIndex		= gSockMsgOut.iMsgPara1;
			tableInfo.index.almThrTmpTpSubType	= gSockMsgOut.iMsgPara2;
			tableInfo.data.almThrScanType		= gSockMsgOut.iMsgPara3;
			tableInfo.data.almThrTpSubTypeUpRiseHigh32	= gSockMsgOut.iMsgPara4;
			tableInfo.data.almThrTpSubTypeUpRiseLow32	= gSockMsgOut.iMsgPara5;
			tableInfo.data.almThrTpSubTypeUpClrHigh32	= gSockMsgOut.iMsgPara6;
			tableInfo.data.almThrTpSubTypeUpClrLow32	= gSockMsgOut.iMsgPara7;
			tableInfo.data.almThrTpSubTypeDnRiseHigh32	= gSockMsgOut.iMsgPara8;
			tableInfo.data.almThrTpSubTypeDnRiseLow32	= gSockMsgOut.iMsgPara9;
			tableInfo.data.almThrTpSubTypeDnClrHigh32	= gSockMsgOut.iMsgParaA;
			tableInfo.data.almThrTpSubTypeDnClrLow32	= gSockMsgOut.iMsgParaB;
			tableInfo.data.rowStatus					= ALM_SNMP_ROW_ACTIVE;

			/*ipc_send_reply (&tableInfo, sizeof(stAlmThrTmpTab_info), phdr->sender_id, 
				phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);*/

			ipc_ack_to_snmp(phdr, (void *)&tableInfo, sizeof(stAlmThrTmpTab_info), 1);
			
			return;
		}
	}

	/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
					phdr->msg_subtype, phdr->msg_index) < 0)
	{
		zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
	}*/

	ipc_noack_to_snmp(phdr);
}


static void stat_snmp_alm_td_tp_set(struct ipc_mesg_n *pmsg)
{
	int mode = 0;
	memcpy(&mode, pmsg->msg_data, sizeof(int));
	stAlmThrTmpTab_info *pInfo = (stAlmThrTmpTab_info *)(pmsg->msg_data + sizeof(int));
	
	gpnSockMsg gSockMsgIn;	
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));

#if 0
	/*增加行时，使用分配的id，否则配置不下去*/
	uint32_t almThrTmpIndex = 0;	
	if(IPC_OPCODE_ADD == pmsg->msghdr.opcode)
	{
		almThrTmpIndex = gpnStatAlarmThredTemplatIdGet();
	}
	else
	{
		almThrTmpIndex = pInfo->index.almThrTmpIndex;
	}
#endif

	zlog_debug(STATIS_DBG_SET, "%s[%d] : mode[%x:ADM(%x|%x|%x)], index(%d|%x)\n", __func__, __LINE__,
		mode,
		GPN_STAT_MSG_ALM_THRED_TEMP_ADD,
		GPN_STAT_MSG_ALM_THRED_TEMP_DELETE,
		GPN_STAT_MSG_ALM_THRED_TEMP_MODIFY,
		pInfo->index.almThrTmpIndex, pInfo->index.almThrTmpTpSubType);

	pgSockMsgIn->iIndex 	= 0;
	pgSockMsgIn->iMsgType	= mode;
	pgSockMsgIn->iSrcId 	= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId 	= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pInfo->index.almThrTmpIndex;
	//pgSockMsgIn->iMsgPara1	= almThrTmpIndex;
	pgSockMsgIn->iMsgPara2	= pInfo->index.almThrTmpTpSubType;
	pgSockMsgIn->iMsgPara3	= pInfo->data.almThrScanType;
	pgSockMsgIn->iMsgPara4	= pInfo->data.almThrTpSubTypeUpRiseHigh32;
	pgSockMsgIn->iMsgPara5	= pInfo->data.almThrTpSubTypeUpRiseLow32;
	pgSockMsgIn->iMsgPara6	= pInfo->data.almThrTpSubTypeUpClrHigh32;
	pgSockMsgIn->iMsgPara7	= pInfo->data.almThrTpSubTypeUpClrLow32;
	pgSockMsgIn->iMsgPara8	= pInfo->data.almThrTpSubTypeDnRiseHigh32;
	pgSockMsgIn->iMsgPara9	= pInfo->data.almThrTpSubTypeDnRiseLow32;
	pgSockMsgIn->iMsgParaA	= pInfo->data.almThrTpSubTypeDnClrHigh32;
	pgSockMsgIn->iMsgParaB	= pInfo->data.almThrTpSubTypeDnClrLow32;
	pgSockMsgIn->msgCellLen = 0;	

	gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
}


static void stat_snmp_sub_type_report_tp_get(struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stSubTypeReportTmpTab_index *pIndex = (stSubTypeReportTmpTab_index *)pmsg->msg_data;
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (pgSockMsgOut, 0, sizeof (gpnSockMsg));

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);

	stSubTypeReportTmpTab_info tableInfo;
	memset (&tableInfo, 0, sizeof(stSubTypeReportTmpTab_info));

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType 	= GPN_STAT_MSG_SUBREPORT_TEMP_GET;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->subReportTmpIndex;
	gSockMsgIn.iMsgPara2	= pIndex->subReportSubType;

	if(GPN_STAT_SNMP_GEN_OK == gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen))
	{
		if (gSockMsgOut.iMsgType == GPN_STAT_MSG_SUBREPORT_TEMP_GET_RSP)
		{
			memset (&tableInfo, 0, sizeof (stSubStatTypeFileTpTab_info));
			tableInfo.index.subReportTmpIndex	= gSockMsgOut.iMsgPara1;
			tableInfo.index.subReportSubType	= gSockMsgOut.iMsgPara2;
			tableInfo.data.subReportScanType	= gSockMsgOut.iMsgPara3;
			tableInfo.data.subReportStatus		= gSockMsgOut.iMsgPara4;
			tableInfo.data.rowStatus			= ALM_SNMP_ROW_ACTIVE;

			/*ipc_send_reply (&tableInfo, sizeof(stSubTypeReportTmpTab_info), phdr->sender_id, 
				hdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);*/

			ipc_ack_to_snmp(phdr, (void *)&tableInfo, sizeof(stSubTypeReportTmpTab_info), 1);
			
			return;
		}
	}

	/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
					phdr->msg_subtype, phdr->msg_index) < 0)
	{
		zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
	}*/

	ipc_noack_to_snmp(phdr);
}


static void stat_snmp_sub_type_report_tp_set(struct ipc_mesg_n *pmsg)
{
	int mode = 0;
	memcpy(&mode, pmsg->msg_data, sizeof(int));
	stSubTypeReportTmpTab_info *pInfo = (stSubTypeReportTmpTab_info *)(pmsg->msg_data + sizeof(int));
	
	gpnSockMsg gSockMsgIn;	
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));

#if 0
	/*增加行时，使用分配的id，否则配置不下去*/
	uint32_t subReportTmpIndex = 0;	
	if(IPC_OPCODE_ADD == pmsg->msghdr.opcode)
	{
		subReportTmpIndex = gpnStatSubTypeReportIdGet();
	}
	else
	{
		subReportTmpIndex = pInfo->index.subReportTmpIndex;
	}
#endif

	zlog_debug(STATIS_DBG_SET, "%s[%d] : mode[%x:ADM(%x|%x|%x)], index(%d|%x)\n", __func__, __LINE__,
		mode,
		GPN_STAT_MSG_SUBREPORT_TEMP_ADD,
		GPN_STAT_MSG_SUBREPORT_TEMP_DELETE,
		GPN_STAT_MSG_SUBREPORT_TEMP_MODIFY,
		pInfo->index.subReportTmpIndex, pInfo->index.subReportSubType);
	
	pgSockMsgIn->iIndex 	= 0;
	pgSockMsgIn->iMsgType	= mode;
	pgSockMsgIn->iSrcId 	= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId 	= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pInfo->index.subReportTmpIndex;
	//pgSockMsgIn->iMsgPara1	= subReportTmpIndex;
	pgSockMsgIn->iMsgPara2	= pInfo->index.subReportSubType;
	pgSockMsgIn->iMsgPara3	= pInfo->data.subReportScanType;
	pgSockMsgIn->iMsgPara4	= pInfo->data.subReportStatus;
	pgSockMsgIn->msgCellLen = 0;	

	gpnStatGlobalPorc(pgSockMsgIn, sizeof(gpnSockMsg));
}



/* stat scalar get */
static void stat_snmp_get (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (pgSockMsgOut, 0, sizeof (gpnSockMsg));	

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);

	stGloSubTypeAttribTab_info tableInfo;
	memset (&tableInfo, 0, sizeof (stGloSubTypeAttribTab_info));

	gSockMsgIn.iIndex = 0;
	gSockMsgIn.iSrcId = GPN_COMMM_STAT;
	gSockMsgIn.iDstId = GPN_COMMM_STAT;

	switch(phdr->msg_subtype)
	{
		case SNMP_STAT_GLOBAL_TIME_OFFSET_POLARITY:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_TIME_POLARIT_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_TIME_POLARIT_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_GLOBAL_TIME_OFFSET_VALUE:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_TIME_OFFSET_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_TIME_OFFSET_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_MON_OBJ_MAX_NUM:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_MAX_OBJ_SRC_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_MAX_OBJ_SRC_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_ALREADY_MON_OBJ_NUM:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_USE_OBJ_SRC_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_USE_OBJ_SRC_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_TASK_MAX_NUM:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_MAX_TASK_NUM_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_MAX_TASK_NUM_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_ALREADY_RUN_TASK_NUM:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_RUN_TASK_NUM_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_RUN_TASK_NUM_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_THRESHOLD_TEMPLAT_MAX_NUM:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_MAX_EVN_THRED_TP_NUM_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_ALREADY_THRESHOLD_TEMPLATE_NUM:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_RUN_EVN_THRED_TP_NUM_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_SUB_STAT_FILT_TP_MAX_NUM:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_MAX_SUBFILTTP_NUM_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_ALREADY_SUB_STAT_FILT_TP_NUM:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_RUN_SUBFILTTP_NUM_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_SUB_REPORT_TP_MAX_NUM:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_MAX_SUBREPORTTP_NUM_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_ALREADY_SUB_REPORT_IP_NUM:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_RUN_SUBREPORTTP_NUM_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_AUTO_REPORT_LAST_HIST:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_LAST_DATA_REPT_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_LAST_DATA_REPT_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_AUTO_REPORT_OLD_HIST:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_OLD_DATA_REPT_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_OLD_DATA_REPT_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_AUTO_NOTIFY_ALARM:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_ALARM_REPT_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_ALARM_REPT_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_AUTO_NOTIFY_EVENT:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_EVENT_REPT_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_EVENT_REPT_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_GET_STAT_TASK_ID:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_TASK_ID_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_TASK_ID_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_GET_STAT_EVENT_THRESHOLD_TP_ID:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TP_ID_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_EVN_THRED_TP_ID_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_GET_MARK_SUB_TYPE_TP_ID:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_SUB_FILT_TP_ID_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_SUB_FILT_TP_ID_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_GET_STAT_ALARM_THRESHOLD_TP_ID:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_ALM_THRED_TP_ID_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_ALM_THRED_TP_ID_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_GET_SUB_TYPE_REPORT_TP_ID:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_SUB_REPORT_TP_ID_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_SUB_REPORT_TP_ID_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_STAT_DATA_COLLECT_TYPE:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_COUNT_TYPE_GET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			if (GPN_STAT_MSG_COUNT_TYPE_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;
		
		default:
			goto table_get;
			break;
	}

	/*if (ipc_send_reply(&int_value, sizeof(int_value), phdr->sender_id,
					phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
	{
		zlog_err ("%s[%d] : stat send scalar to snmp failed\n", __FUNCTION__, __LINE__);
	}*/

	ipc_ack_to_snmp(phdr, (void *)&int_value, sizeof(int_value), 1);
	
	return;

table_get:
	switch(phdr->msg_subtype)
	{
#if 0
		case SNMP_STAT_GLOBAL_SUB_TYPE_ATTRI_TABLE:
			stat_snmp_global_sub_type_attri_get(pmsg);
			break;
#endif

		case SNMP_STAT_PORT_MONI_CTRL_TABLE:
			stat_snmp_port_moni_ctrl_get(pmsg);
			break;

		case SNMP_CURR_STAT_TABLE:
			stat_snmp_curr_stat_get(pmsg);
			break;

		case SNMP_STAT_TASK_TABLE:
			stat_snmp_task_get(pmsg);
			break;

		case SNMP_STAT_EVENT_THRESHOLD_TEMPLAT_TABLE:
			stat_snmp_evt_td_tp_get(pmsg);
			break;

		case SNMP_STAT_SUB_STAT_TYPE_FILT_TP_TABLE:
			stat_snmp_sub_type_filt_tp_get(pmsg);
			break;

		case SNMP_STAT_ALARM_THRESHOLD_TEMPLAT_TABLE:
			stat_snmp_alm_td_tp_get(pmsg);
			break;

		case SNMP_STAT_SUB_STAT_TYPE_REPORT_TEMPLAT_TABLE:
			stat_snmp_sub_type_report_tp_get(pmsg);
		
		default:
			/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
							phdr->msg_subtype, phdr->msg_index) < 0)
			{
				zlog_err("%s[%d] : stat send data to snmpd failed\n", __func__, __LINE__);
			}*/

			ipc_noack_to_snmp(phdr);
			break;
	}
}


/* stat scalar set */
static void stat_snmp_set (struct ipc_mesg_n *pmsg)
{
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (pgSockMsgOut, 0, sizeof (gpnSockMsg));

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);

	uint32_t iReVal = 0;
	memcpy (&iReVal, pmsg->msg_data, sizeof(uint32_t));

	gSockMsgIn.iIndex = 0;
	gSockMsgIn.iSrcId = GPN_COMMM_STAT;
	gSockMsgIn.iDstId = GPN_COMMM_STAT;
	gSockMsgIn.iMsgPara1 = iReVal;

	switch(pmsg->msghdr.msg_subtype)
	{
		case SNMP_STAT_AUTO_REPORT_LAST_HIST:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_LAST_DATA_REPT_SET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			break;

		case SNMP_STAT_AUTO_REPORT_OLD_HIST:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_OLD_DATA_REPT_SET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			break;

		case SNMP_STAT_AUTO_NOTIFY_ALARM:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_ALARM_REPT_SET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			break;

		case SNMP_STAT_AUTO_NOTIFY_EVENT:
			gSockMsgIn.iMsgType = GPN_STAT_MSG_EVENT_REPT_SET;
			gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
			break;
			
		case SNMP_STAT_PORT_MONI_CTRL_TABLE:
			stat_snmp_port_moni_ctrl_set(pmsg);
			break;

		case SNMP_CURR_STAT_TABLE:
			stat_snmp_curr_stat_set(pmsg);
			break;

		case SNMP_STAT_TASK_TABLE:
			stat_snmp_task_set(pmsg);
			break;

		case SNMP_STAT_EVENT_THRESHOLD_TEMPLAT_TABLE:
			stat_snmp_evt_td_tp_set(pmsg);
			break;

		case SNMP_STAT_SUB_STAT_TYPE_FILT_TP_TABLE:
			stat_snmp_sub_type_filt_tp_set(pmsg);
			break;

		case SNMP_STAT_ALARM_THRESHOLD_TEMPLAT_TABLE:
			stat_snmp_alm_td_tp_set(pmsg);
			break;

		case SNMP_STAT_SUB_STAT_TYPE_REPORT_TEMPLAT_TABLE:
			stat_snmp_sub_type_report_tp_set(pmsg);
			
		default:
			break;
	}
}


static void stat_snmp_global_scan_type_attri_get_bulk (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stStatGloScanTypeAttribTable_index *pIndex = (stStatGloScanTypeAttribTable_index *)pmsg->msg_data;
		
	static int iCnt_read = 0;	//0:first time to read; 1:other time to read
	
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);
	size_t unitLen = sizeof(stStatGloScanTypeAttribTable_info);
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	
	memset (pgSockMsgIn, 0, inLen);
	memset (pgSockMsgOut, 0, outLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stStatGloScanTypeAttribTable_info tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType		= GPN_STAT_MSG_STAT_TYPE_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->typeIndex;

	uint32_t ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while(GPN_STAT_SNMP_GEN_OK == ret)
	{
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_STAT_TYPE_GET_NEXT_RSP)
		{
			if(0 == iCnt_read)
			{
				iCnt_read = 1;
				
				pgSockMsgIn->iIndex		= 0;
				pgSockMsgIn->iMsgType	= GPN_STAT_MSG_STAT_TYPE_GET_NEXT;
				pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
				pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
				pgSockMsgIn->iMsgPara1	= pgSockMsgOut->iMsgPara6;

				if(GPN_STAT_SCAN_TYPE_ALL == pgSockMsgOut->iMsgPara6)
				{
					break;
				}
				
				ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
				continue;
			}
			
			memset (&tableInfo, 0, unitLen);
			tableInfo.index.typeIndex 			= gSockMsgOut.iMsgPara1;
			tableInfo.data.includeStatTypeNum 	= gSockMsgOut.iMsgPara5;
			tableInfo.data.subTypeNumInScanType = gSockMsgOut.iMsgPara4;
			tableInfo.data.longCycSeconds 		= gSockMsgOut.iMsgPara2;
			tableInfo.data.shortCycSeconds 		= gSockMsgOut.iMsgPara3;
			tableInfo.data.rowStatus 			= ALM_SNMP_ROW_ACTIVE;
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += unitLen;
			table_cnt++;

			if(GPN_STAT_SCAN_TYPE_ALL == pgSockMsgOut->iMsgPara6)	//search done
			{
				break;
			}

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (pgSockMsgIn, 0, inLen);
			
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_STAT_MSG_STAT_TYPE_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= pgSockMsgOut->iMsgPara6;

			memset (pgSockMsgOut, 0, outLen);
			
			ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
		{
			break;
		}
	}

	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	iCnt_read = 0;
}

static void stat_snmp_global_sub_type_attri_get_bulk (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stGloSubTypeAttribTab_index *pIndex = (stGloSubTypeAttribTab_index *)pmsg->msg_data;

	static int iCnt_read = 0;	//0:first time to read; 1:other time to read
	
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof (gpnSockMsg) + sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred);
	size_t unitLen = sizeof(stGloSubTypeAttribTab_info);
	
	gpnSockMsg gSockMsgIn;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = (gpnSockMsg *)XMALLOC(MTYPE_STAT, outLen);
	
	memset (pgSockMsgIn, 0, inLen);
	memset (pgSockMsgOut, 0, outLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stGloSubTypeAttribTab_info tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType		= GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->subTypeIndex;
	pgSockMsgOut->msgCellLen = (outLen - sizeof(gpnSockMsg));

	uint32_t ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while(GPN_STAT_SNMP_GEN_OK == ret)
	{
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT_RSP)
		{
			if(0 == iCnt_read)
			{
				iCnt_read = 1;
				
				pgSockMsgIn->iIndex		= 0;
				pgSockMsgIn->iMsgType	= GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT;
				pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
				pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
				pgSockMsgIn->iMsgPara1	= pgSockMsgOut->iMsgPara5;
				pgSockMsgOut->msgCellLen = (outLen - sizeof(gpnSockMsg));

				if(GPN_STAT_TYPE_INVALID == pgSockMsgOut->iMsgPara5)
				{
					break;
				}
				
				ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
				continue;
			}
			
			memset (&tableInfo, 0, unitLen);
			tableInfo.index.subTypeIndex			= pgSockMsgOut->iMsgPara1;
			tableInfo.data.subTypeBelongScanType 	= pgSockMsgOut->iMsgPara2;
			tableInfo.data.subTypeValueClass 		= pgSockMsgOut->iMsgPara3;
			tableInfo.data.subTypeDataBitDeep 		= pgSockMsgOut->iMsgPara4;
			memcpy (&tableInfo.data.longCycUpThrRelatAlarm, &(pgSockMsgOut->msgCellLen) + 1,
				sizeof(stSubTypeBoundAlm) + sizeof(stSubTypeThred));
			tableInfo.data.rowStatus 			= ALM_SNMP_ROW_ACTIVE;
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += unitLen;
			table_cnt++;

			if(GPN_STAT_TYPE_INVALID == pgSockMsgOut->iMsgPara5)	//search done
			{
				break;
			}

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (pgSockMsgIn, 0, inLen);
			
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_STAT_MSG_SUB_STAT_TYPE_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= pgSockMsgOut->iMsgPara5;
			pgSockMsgOut->msgCellLen = (outLen - sizeof(gpnSockMsg));

			memset (pgSockMsgOut, 0, outLen);
			
			ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
		{
			break;
		}
	}

	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	iCnt_read = 0;
	XFREE(MTYPE_STAT, pgSockMsgOut);
}



static void stat_snmp_port_type_to_scan_type_get_bulk (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stPortTypeToScanTypeTab_index *pIndex = (stPortTypeToScanTypeTab_index *)pmsg->msg_data;

	static int iCnt_read = 0;	//0:first time to read; 1:other time to read

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof (gpnSockMsg);
	size_t unitLen = sizeof(stPortTypeToScanTypeTab_info);
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	memset (&gSockMsgIn, 0, inLen);
	memset (&gSockMsgOut, 0, unitLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stPortTypeToScanTypeTab_info tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	gSockMsgIn.iIndex		= pIndex->portTypeIndex;
	gSockMsgIn.iMsgType		= GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->portTypeIndex;

	uint32_t ret = gpnStatAgentXIoCtrl(&gSockMsgIn, inLen, &gSockMsgOut, outLen);
	while (GPN_STAT_SNMP_GEN_OK == ret)
	{
		if (gSockMsgOut.iMsgType == GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT_RSP)
		{
			if(0 == iCnt_read)
			{
				iCnt_read = 1;
				
				gSockMsgIn.iIndex		= 0;
				gSockMsgIn.iMsgType		= GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT;
				gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
				gSockMsgIn.iDstId		= GPN_COMMM_ALM;
				gSockMsgIn.iMsgPara1	= gSockMsgOut.iMsgPara8;

				if(IFM_PORT_TYPE_INVALID == gSockMsgOut.iMsgPara8)	//search done
				{
					break;
				}
				
				ret = gpnStatAgentXIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
				continue;
			}
			
			memset (&tableInfo, 0, sizeof(stPortTypeToScanTypeTab_info));
			tableInfo.index.portTypeIndex 	= gSockMsgOut.iMsgPara1;
			tableInfo.data.portTypeAboutScanTypeNum 	= gSockMsgOut.iMsgPara2;
			tableInfo.data.type1Index 		= gSockMsgOut.iMsgPara3;
			tableInfo.data.type2Index 		= gSockMsgOut.iMsgPara4;
			tableInfo.data.type3Index 		= gSockMsgOut.iMsgPara5;
			tableInfo.data.type4Index 		= gSockMsgOut.iMsgPara6;
			tableInfo.data.type5Index 		= gSockMsgOut.iMsgPara7;
			tableInfo.data.rowStatus 		= ALM_SNMP_ROW_ACTIVE;			
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += unitLen;
			table_cnt++;

			if(IFM_PORT_TYPE_INVALID == gSockMsgOut.iMsgPara8)	//search done
			{
				break;
			}

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));			
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_STAT_MSG_PORT_TP_2_SCAN_TP_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= gSockMsgOut.iMsgPara8;
			memset (&gSockMsgOut, 0, sizeof(gpnSockMsg));
			
			ret = gpnStatAgentXIoCtrl (&gSockMsgIn, inLen, &gSockMsgOut, outLen);
		}
		else
			break;
	}

	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}
	
	iCnt_read = 0;
}


static void stat_snmp_port_moni_ctrl_get_bulk (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stPortMoniCtrlTab_index *pIndex = (stPortMoniCtrlTab_index *)pmsg->msg_data;

	static int iCnt_read = 0;	//0:first time to read; 1:other time to read

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg) + (sizeof(stStatMsgMonObjCfg) + sizeof(stStatObjDesc) + sizeof(UINT32));
	size_t unitLen = sizeof(stPortMoniCtrlTab_info);
	
	gpnSockMsg gSockMsgIn;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	memset (pgSockMsgIn, 0, inLen);
	
	gpnSockMsg *pgSockMsgOut = (gpnSockMsg *)XMALLOC(MTYPE_STAT, outLen);
	memset (pgSockMsgOut, 0, outLen);
	
	pgSockMsgOut->msgCellLen = (outLen - sizeof(gpnSockMsg));
	stStatMsgMonObjCfg *pstatMonObjCfg = (stStatMsgMonObjCfg *)(&(pgSockMsgOut->msgCellLen) + 1);
	stStatObjDesc *pstatObjDesc = (stStatObjDesc *)(pstatMonObjCfg + 1);
	uint32_t *pscanType = (uint32_t *)(pstatObjDesc + 1);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	static stPortMoniCtrlTab_info tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= GPN_STAT_MSG_MON_GET_NEXT;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pIndex->portMoniDevIndex;
	pgSockMsgIn->iMsgPara2	= pIndex->portMoniIndex1;
	pgSockMsgIn->iMsgPara3	= pIndex->portMoniIndex2;
	pgSockMsgIn->iMsgPara4	= pIndex->portMoniIndex3;
	pgSockMsgIn->iMsgPara5	= pIndex->portMoniIndex4;
	pgSockMsgIn->iMsgPara6	= pIndex->portMoniIndex5;
	pgSockMsgIn->iMsgPara7	= pIndex->portMoniScanType;

	uint32_t ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while (GPN_STAT_SNMP_GEN_OK == ret)
	{
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_MON_GET_NEXT_RSP)
		{
			if(0 == iCnt_read)
			{
				iCnt_read = 1;
				
				pgSockMsgIn->iIndex		= 0;
				pgSockMsgIn->iMsgType	= GPN_STAT_MSG_MON_GET_NEXT;
				pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
				pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
				pgSockMsgIn->iMsgPara1	= pstatObjDesc->devIndex;
				pgSockMsgIn->iMsgPara2	= pstatObjDesc->portIndex;
				pgSockMsgIn->iMsgPara3	= pstatObjDesc->portIndex3;
				pgSockMsgIn->iMsgPara4	= pstatObjDesc->portIndex4;
				pgSockMsgIn->iMsgPara5	= pstatObjDesc->portIndex5;
				pgSockMsgIn->iMsgPara6	= pstatObjDesc->portIndex6;
				pgSockMsgIn->iMsgPara7	= *pscanType;

				/* search done */
				if((IFM_PORT_TYPE_INVALID == pstatObjDesc->devIndex) \
					&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex) \
					&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex3) \
					&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex4) \
					&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex5) \
					&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex6) \
					&&(GPN_STAT_SCAN_TYPE_ALL == *pscanType))
				{
					break;
				}
				
				ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
				continue;
			}
			
			memset (&tableInfo, 0, sizeof(stPortMoniCtrlTab_info));
			tableInfo.index.portMoniIndex1 		= pgSockMsgOut->iMsgPara2;
			tableInfo.index.portMoniIndex2 		= pgSockMsgOut->iMsgPara3;
			tableInfo.index.portMoniIndex3 		= pgSockMsgOut->iMsgPara4;
			tableInfo.index.portMoniIndex4 		= pgSockMsgOut->iMsgPara5;
			tableInfo.index.portMoniIndex5 		= pgSockMsgOut->iMsgPara6;
			tableInfo.index.portMoniDevIndex 	= pgSockMsgOut->iMsgPara1;
			tableInfo.index.portMoniScanType 	= pgSockMsgOut->iMsgPara7;
			
			tableInfo.data.moniEn 						= pstatMonObjCfg->statMoniEn;
			tableInfo.data.currMoniEn 					= pstatMonObjCfg->currStatMoniEn;
			tableInfo.data.longCycMoniEn 				= pstatMonObjCfg->longCycStatMoniEn;
			tableInfo.data.longCycBelongTask 			= pstatMonObjCfg->longCycBelongTask;
			tableInfo.data.longCycEvtThrLocation 		= pstatMonObjCfg->longCycEvnThredTpId;
			tableInfo.data.longCycSubTypeFiltLocation 	= pstatMonObjCfg->longCycSubFiltTpId;
			tableInfo.data.longCycAlmThrLocation 		= pstatMonObjCfg->longCycAlmThredTpId;
			tableInfo.data.longCycHistAutoReport 		= pstatMonObjCfg->longCycHistReptTpId;
			tableInfo.data.longCycHistLocation 			= pstatMonObjCfg->longCycHistDBId;
			tableInfo.data.shortCycMoniEn 				= pstatMonObjCfg->shortCycStatMoniEn;
			tableInfo.data.shortCycBelongTask 			= pstatMonObjCfg->shortCycBelongTask;
			tableInfo.data.shortCycEvtThrLocation 		= pstatMonObjCfg->shortCycEvnThredTpId;
			tableInfo.data.shortCycSubTypeFiltLocation 	= pstatMonObjCfg->shortCycSubFiltTpId;
			tableInfo.data.shortCycAlmThrLocation 		= pstatMonObjCfg->shortCycAlmThredTpId;
			tableInfo.data.shortCycHistAutoReport 		= pstatMonObjCfg->shortCycHistReptTpId;
			tableInfo.data.shortCycHistLocation 		= pstatMonObjCfg->shortCycHistDBId;
			tableInfo.data.usrDefCycMoniEn 				= pstatMonObjCfg->udCycStatMoniEn;
			tableInfo.data.usrDefCycBelongTask 			= pstatMonObjCfg->udCycBelongTask;
			tableInfo.data.usrDefCycEvtThrLocation 		= pstatMonObjCfg->udCycEvnThredTpId;
			tableInfo.data.usrDefCycSubTypeFiltLocation = pstatMonObjCfg->udCycSubFiltTpId;
			tableInfo.data.usrDefCycAlmThrLocation 		= pstatMonObjCfg->udCycAlmThredTpId;
			tableInfo.data.usrDefCycHistAutoReport 		= pstatMonObjCfg->udCycHistReptTpId;
			tableInfo.data.usrDefCycHistLocation 		= pstatMonObjCfg->udCycHistDBId;
			tableInfo.data.usrDefCycHistSeconds 		= pstatMonObjCfg->udCycSecs;
			tableInfo.data.rowStatus 					= ALM_SNMP_ROW_ACTIVE;			
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += unitLen;
			table_cnt++;

			/* search done */
			if((IFM_PORT_TYPE_INVALID == pstatObjDesc->devIndex) \
				&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex) \
				&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex3) \
				&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex4) \
				&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex5) \
				&&(IFM_PORT_TYPE_INVALID == pstatObjDesc->portIndex6) \
				&&(GPN_STAT_SCAN_TYPE_ALL == *pscanType))		
			{
				break;
			}

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));			
			
			pgSockMsgIn->iIndex		= 0;
			pgSockMsgIn->iMsgType	= GPN_STAT_MSG_MON_GET_NEXT;
			pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
			pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
			pgSockMsgIn->iMsgPara1	= pstatObjDesc->devIndex;
			pgSockMsgIn->iMsgPara2	= pstatObjDesc->portIndex;
			pgSockMsgIn->iMsgPara3	= pstatObjDesc->portIndex3;
			pgSockMsgIn->iMsgPara4	= pstatObjDesc->portIndex4;
			pgSockMsgIn->iMsgPara5	= pstatObjDesc->portIndex5;
			pgSockMsgIn->iMsgPara6	= pstatObjDesc->portIndex6;
			pgSockMsgIn->iMsgPara7	= *pscanType;

			memset (pgSockMsgOut, 0, outLen);
			pgSockMsgOut->msgCellLen = (outLen - sizeof(gpnSockMsg));
			
			ret = gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
			break;
	}

	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	XFREE(MTYPE_STAT, pgSockMsgOut);

	iCnt_read = 0;
}


static void stat_snmp_curr_stat_get_bulk (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stCurrStatTab_index *pIndex = (stCurrStatTab_index *)pmsg->msg_data;

	static int iCnt_read = 0;	//0:first time to read; 1:other time to read
	
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg) + sizeof(gpnStatMsgCurrDataIndex);
	size_t unitLen = sizeof(stCurrStatTab_info);
	
	gpnSockMsg gSockMsgIn;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	memset (pgSockMsgIn, 0, inLen);
	
	gpnSockMsg *pgSockMsgOut = (gpnSockMsg *)XMALLOC(MTYPE_STAT, outLen);
	gpnStatMsgCurrDataIndex *pCurrIndex = (gpnStatMsgCurrDataIndex *)(&pgSockMsgOut->msgCellLen + 1);
	memset (pgSockMsgOut, 0, outLen);
	pgSockMsgOut->msgCellLen = (outLen - sizeof(gpnSockMsg));

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stCurrStatTab_info tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType 	= GPN_STAT_MSG_CURR_DATA_GET_NEXT;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pIndex->currDevIndex;
	pgSockMsgIn->iMsgPara2	= pIndex->currPortIndex1;
	pgSockMsgIn->iMsgPara3	= pIndex->currPortIndex2;
	pgSockMsgIn->iMsgPara4	= pIndex->currPortIndex3;
	pgSockMsgIn->iMsgPara5	= pIndex->currPortIndex4;
	pgSockMsgIn->iMsgPara6	= pIndex->currPortIndex5;
	pgSockMsgIn->iMsgPara7	= pIndex->currDataType;
	pgSockMsgOut->msgCellLen = sizeof(gpnStatMsgCurrDataIndex);

	uint32_t ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while (GPN_STAT_SNMP_GEN_OK == ret)
	{
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_CURR_DATA_GET_NEXT_RSP)
		{
			if(0 == iCnt_read)
			{
				iCnt_read = 1;
				
				pgSockMsgIn->iIndex		= 0;
				pgSockMsgIn->iMsgType 	= GPN_STAT_MSG_CURR_DATA_GET_NEXT;
				pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
				pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
				pgSockMsgIn->iMsgPara1	= pCurrIndex->portIndex.devIndex;
				pgSockMsgIn->iMsgPara2	= pCurrIndex->portIndex.portIndex;
				pgSockMsgIn->iMsgPara3	= pCurrIndex->portIndex.portIndex3;
				pgSockMsgIn->iMsgPara4	= pCurrIndex->portIndex.portIndex4;
				pgSockMsgIn->iMsgPara5	= pCurrIndex->portIndex.portIndex5;
				pgSockMsgIn->iMsgPara6	= pCurrIndex->portIndex.portIndex6;
				pgSockMsgIn->iMsgPara7	= pCurrIndex->statSubType;

				

				/* search done */
				if((GPN_ILLEGAL_DEVICE_INDEX == pCurrIndex->portIndex.devIndex) \
					&& (GPN_ILLEGAL_PORT_INDEX == pCurrIndex->portIndex.portIndex) \
					&& (GPN_ILLEGAL_PORT_INDEX == pCurrIndex->portIndex.portIndex3) \
					&& (GPN_ILLEGAL_PORT_INDEX == pCurrIndex->portIndex.portIndex4) \
					&& (GPN_ILLEGAL_PORT_INDEX == pCurrIndex->portIndex.portIndex5) \
					&& (GPN_ILLEGAL_PORT_INDEX == pCurrIndex->portIndex.portIndex6))					
				{
					break;
				}
					
				memset(pgSockMsgOut, 0, outLen);
				pgSockMsgOut->msgCellLen = (outLen - sizeof(gpnSockMsg));
				
				ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
				continue;
			}
			
			memset (&tableInfo, 0, unitLen);
			tableInfo.index.currPortIndex1 	= pgSockMsgOut->iMsgPara2;
			tableInfo.index.currPortIndex2 	= pgSockMsgOut->iMsgPara3;
			tableInfo.index.currPortIndex3 	= pgSockMsgOut->iMsgPara4;
			tableInfo.index.currPortIndex4 	= pgSockMsgOut->iMsgPara5;
			tableInfo.index.currPortIndex5 	= pgSockMsgOut->iMsgPara6;
			tableInfo.index.currDevIndex 	= pgSockMsgOut->iMsgPara1;
			tableInfo.index.currDataType 	= pgSockMsgOut->iMsgPara7;
			
			tableInfo.data.clearCurrData 		 = pgSockMsgOut->iMsgPara8;
			tableInfo.data.currDataTypeValHigh32 = pgSockMsgOut->iMsgPara9;
			tableInfo.data.currDataTypeValLow32  = pgSockMsgOut->iMsgParaA;
			tableInfo.data.rowStatus 			 = ALM_SNMP_ROW_ACTIVE;			
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += unitLen;
			table_cnt++;

			if((GPN_ILLEGAL_DEVICE_INDEX == pCurrIndex->portIndex.devIndex) \
				&& (GPN_ILLEGAL_PORT_INDEX == pCurrIndex->portIndex.portIndex) \
				&& (GPN_ILLEGAL_PORT_INDEX == pCurrIndex->portIndex.portIndex3) \
				&& (GPN_ILLEGAL_PORT_INDEX == pCurrIndex->portIndex.portIndex4) \
				&& (GPN_ILLEGAL_PORT_INDEX == pCurrIndex->portIndex.portIndex5) \
				&& (GPN_ILLEGAL_PORT_INDEX == pCurrIndex->portIndex.portIndex6) \
				&& (GPN_ILLEGAL_PORT_INDEX == pCurrIndex->portIndex.portIndex))		//search done
			{
				break;
			}

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));			
			
			pgSockMsgIn->iIndex		= 0;
			pgSockMsgIn->iMsgType	= GPN_STAT_MSG_CURR_DATA_GET_NEXT;
			pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
			pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
			pgSockMsgIn->iMsgPara1	= pCurrIndex->portIndex.devIndex;
			pgSockMsgIn->iMsgPara2	= pCurrIndex->portIndex.portIndex;
			pgSockMsgIn->iMsgPara3	= pCurrIndex->portIndex.portIndex3;
			pgSockMsgIn->iMsgPara4	= pCurrIndex->portIndex.portIndex4;
			pgSockMsgIn->iMsgPara5	= pCurrIndex->portIndex.portIndex5;
			pgSockMsgIn->iMsgPara6	= pCurrIndex->portIndex.portIndex6;
			pgSockMsgIn->iMsgPara7	= pCurrIndex->statSubType;			

			memset (pgSockMsgOut, 0, outLen);
			pgSockMsgOut->msgCellLen = (outLen - sizeof(gpnSockMsg));
			
			ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
			break;
	}

	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	iCnt_read = 0;
	XFREE(MTYPE_STAT, pgSockMsgOut);
}


static void stat_snmp_task_get_bulk (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stStatTaskTab_index *pIndex = (stStatTaskTab_index *)pmsg->msg_data;

	static int iCnt_read = 0;	//0:first time to read; 1:other time to read
	
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);
	size_t unitLen = sizeof(stStatTaskTab_info);
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	
	memset (pgSockMsgIn, 0, inLen);
	memset (pgSockMsgOut, 0, outLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stStatTaskTab_info tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= GPN_STAT_MSG_TASK_GET_NEXT;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pIndex->taskIndex;

	uint32_t ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while (GPN_STAT_SNMP_GEN_OK == ret)
	{
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_TASK_GET_NEXT_RSP)
		{
			if(0 == iCnt_read)
			{
				iCnt_read = 1;
				
				pgSockMsgIn->iIndex		= 0;
				pgSockMsgIn->iMsgType	= GPN_STAT_MSG_TASK_GET_NEXT;
				pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
				pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
				pgSockMsgIn->iMsgPara1	= pgSockMsgOut->iMsgParaA;

				if(GPN_STAT_INVALID_TASK_ID == pgSockMsgOut->iMsgParaA)	//search done
				{
					break;
				}
				
				ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
				continue;
			}
			
			memset (&tableInfo, 0, sizeof(stStatTaskTab_info));
			tableInfo.index.taskIndex 		= pgSockMsgOut->iMsgPara1;
			tableInfo.data.maxPortInTask 	= pgSockMsgOut->iMsgParaB;
			tableInfo.data.taskScanType 	= pgSockMsgOut->iMsgPara2;
			tableInfo.data.includeMonObjNum = pgSockMsgOut->iMsgPara3;
			tableInfo.data.taskCycMoniType 	= pgSockMsgOut->iMsgPara4;
			tableInfo.data.taskMoniCycSec 	= pgSockMsgOut->iMsgPara5;
			tableInfo.data.taskStartTime 	= pgSockMsgOut->iMsgPara6;
			tableInfo.data.taskEndTime 		= pgSockMsgOut->iMsgPara7;
			tableInfo.data.taskAlmThrTpLocation  = pgSockMsgOut->iMsgPara8;
			tableInfo.data.taskSubReportLocation = pgSockMsgOut->iMsgPara9;
			tableInfo.data.rowStatus 			 = ALM_SNMP_ROW_ACTIVE;
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += unitLen;
			table_cnt++;

			if(GPN_STAT_INVALID_TASK_ID == pgSockMsgOut->iMsgParaA)	//search done
			{
				break;
			}

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (pgSockMsgIn, 0, inLen);
			
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_STAT_MSG_TASK_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= pgSockMsgOut->iMsgParaA;

			memset (pgSockMsgOut, 0, outLen);
			
			ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
		{
			break;
		}
	}

	//zlog_debug(0, "%s[%d] : table_cnt[%d], max[%d]\n", __func__, __LINE__, table_cnt, max_table_cnt);	
	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	iCnt_read = 0;
}


static void stat_snmp_evt_thr_tmp_get_bulk (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stEvtThrTmpTab_index *pIndex = (stEvtThrTmpTab_index *)pmsg->msg_data;
	
	static int iCnt_read = 0;	//0:first time to read; 1:other time to read
	
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);
	size_t unitLen = sizeof(stEvtThrTmpTab_info);
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	
	memset (pgSockMsgIn, 0, inLen);
	memset (pgSockMsgOut, 0, outLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stEvtThrTmpTab_info tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType		= GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->evtThrTmpIndex;
	gSockMsgIn.iMsgPara2	= pIndex->evtThrTmpTpSubType;

	uint32_t ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while (GPN_STAT_SNMP_GEN_OK == ret)
	{
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT_RSP)
		{
			if(0 == iCnt_read)
			{
				iCnt_read = 1;
				
				pgSockMsgIn->iIndex		= 0;
				pgSockMsgIn->iMsgType	= GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT;
				pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
				pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
				pgSockMsgIn->iMsgPara1	= pgSockMsgOut->iMsgPara8;
				pgSockMsgIn->iMsgPara2	= pgSockMsgOut->iMsgPara9;

				if((GPN_STAT_INVALID_TEMPLAT_ID == pgSockMsgOut->iMsgPara8) \
					&& (GPN_STAT_SUB_TYPE_INVALID == pgSockMsgOut->iMsgPara9))//search done
				{
					break;
				}
				
				ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
				continue;
			}
			
			memset (&tableInfo, 0, sizeof(stStatTaskTab_info));
			tableInfo.index.evtThrTmpIndex 		= pgSockMsgOut->iMsgPara1;
			tableInfo.index.evtThrTmpTpSubType 	= pgSockMsgOut->iMsgPara2;
			
			tableInfo.data.evtThrScanType 				= pgSockMsgOut->iMsgPara3;
			tableInfo.data.evtThrTpSubTypeUpThrHigh32 	= pgSockMsgOut->iMsgPara4;
			tableInfo.data.evtThrTpSubTypeUpThrLow32 	= pgSockMsgOut->iMsgPara5;
			tableInfo.data.evtThrTpSubTypeLowThrHigh32 	= pgSockMsgOut->iMsgPara6;
			tableInfo.data.evtThrTpSubTypeLowThrLow32 	= pgSockMsgOut->iMsgPara7;
			tableInfo.data.rowStatus 			 		= ALM_SNMP_ROW_ACTIVE;
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += unitLen;
			table_cnt++;

			if((GPN_STAT_INVALID_TEMPLAT_ID == pgSockMsgOut->iMsgPara8) \
				&& (GPN_STAT_SUB_TYPE_INVALID == pgSockMsgOut->iMsgPara9))	//search done
			{
				break;
			}

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (pgSockMsgIn, 0, inLen);
			
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= pgSockMsgOut->iMsgPara8;
			gSockMsgIn.iMsgPara2	= pgSockMsgOut->iMsgPara9;

			memset (pgSockMsgOut, 0, outLen);
			
			ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
		{
			break;
		}
	}

	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	iCnt_read = 0;
}

static void stat_snmp_sub_stat_type_filt_tp_get_bulk (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stSubStatTypeFileTpTab_index *pIndex = (stSubStatTypeFileTpTab_index *)pmsg->msg_data;

	static int iCnt_read = 0;	//0:first time to read; 1:other time to read
	
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);
	size_t unitLen = sizeof(stSubStatTypeFileTpTab_info);
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, inLen);
	memset (pgSockMsgOut, 0, outLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stSubStatTypeFileTpTab_info tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType		= GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->subTypeFiltIndex;
	gSockMsgIn.iMsgPara2	= pIndex->selectSubType;

	uint32_t ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while (GPN_STAT_SNMP_GEN_OK == ret)
	{
		//zlog_debug(0, "%s[%d] : iCnt_read = %d\n", __func__, __LINE__, iCnt_read);
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT_RSP)
		{
			if(0 == iCnt_read)
			{
				iCnt_read = 1;
				
				pgSockMsgIn->iIndex		= 0;
				pgSockMsgIn->iMsgType	= GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT;
				pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
				pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
				pgSockMsgIn->iMsgPara1	= pgSockMsgOut->iMsgPara5;
				pgSockMsgIn->iMsgPara2	= pgSockMsgOut->iMsgPara6;

				if((GPN_STAT_INVALID_TEMPLAT_ID == pgSockMsgOut->iMsgPara5) \
					&& (GPN_STAT_SUB_TYPE_INVALID == pgSockMsgOut->iMsgPara6))//search done
				{
					break;
				}
				
				ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
				continue;
			}
			
			memset (&tableInfo, 0, unitLen);
			tableInfo.index.subTypeFiltIndex = pgSockMsgOut->iMsgPara1;
			tableInfo.index.selectSubType 	 = pgSockMsgOut->iMsgPara2;			
			tableInfo.data.selectScanType 	 = pgSockMsgOut->iMsgPara3;
			tableInfo.data.selectStatus 	 = pgSockMsgOut->iMsgPara4;
			tableInfo.data.rowStatus 		 = ALM_SNMP_ROW_ACTIVE;
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += unitLen;
			table_cnt++;

			if((GPN_STAT_INVALID_TEMPLAT_ID == pgSockMsgOut->iMsgPara5) \
				&& (GPN_STAT_SUB_TYPE_INVALID == pgSockMsgOut->iMsgPara6))	//search done
			{
				break;
			}

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (pgSockMsgIn, 0, inLen);
			
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= pgSockMsgOut->iMsgPara5;
			gSockMsgIn.iMsgPara2	= pgSockMsgOut->iMsgPara6;

			memset (pgSockMsgOut, 0, outLen);
			
			ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
		{
			break;
		}
	}

	//zlog_debug(0, "%s[%d] : table_cnt[%d], max[%d]\n", __func__, __LINE__, table_cnt, max_table_cnt);	
	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	iCnt_read = 0;
}

static void stat_snmp_alm_td_tp_get_bulk (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stAlmThrTmpTab_index *pIndex = (stAlmThrTmpTab_index *)pmsg->msg_data;
	
	static int iCnt_read = 0;	//0:first time to read; 1:other time to read
	
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);
	size_t unitLen = sizeof(stAlmThrTmpTab_info);
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, inLen);
	memset (pgSockMsgOut, 0, outLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stAlmThrTmpTab_info tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType 	= GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pIndex->almThrTmpIndex;
	pgSockMsgIn->iMsgPara2	= pIndex->almThrTmpTpSubType;

	uint32_t ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while (GPN_STAT_SNMP_GEN_OK == ret)
	{
		//zlog_debug(0, "%s[%d] : iCnt_read = %d\n", __func__, __LINE__, iCnt_read);
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT_RSP)
		{
			if(0 == iCnt_read)
			{
				iCnt_read = 1;
				
				pgSockMsgIn->iIndex		= 0;
				pgSockMsgIn->iMsgType	= GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT;
				pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
				pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
				pgSockMsgIn->iMsgPara1	= pgSockMsgOut->iMsgParaC;
				pgSockMsgIn->iMsgPara2	= pgSockMsgOut->iMsgParaD;

				if((GPN_STAT_INVALID_TEMPLAT_ID == pgSockMsgOut->iMsgParaC) \
					&& (GPN_STAT_SUB_TYPE_INVALID == pgSockMsgOut->iMsgParaD))//search done
				{
					break;
				}
				
				ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
				continue;
			}
			
			memset (&tableInfo, 0, unitLen);
			tableInfo.index.almThrTmpIndex 				= pgSockMsgOut->iMsgPara1;
			tableInfo.index.almThrTmpTpSubType 			= pgSockMsgOut->iMsgPara2;			
			tableInfo.data.almThrScanType 	 			= pgSockMsgOut->iMsgPara3;
			tableInfo.data.almThrTpSubTypeUpRiseHigh32 	= pgSockMsgOut->iMsgPara4;
			tableInfo.data.almThrTpSubTypeUpRiseLow32 	= pgSockMsgOut->iMsgPara5;
			tableInfo.data.almThrTpSubTypeUpClrHigh32 	= pgSockMsgOut->iMsgPara6;
			tableInfo.data.almThrTpSubTypeUpClrLow32 	= pgSockMsgOut->iMsgPara7;
			tableInfo.data.almThrTpSubTypeDnRiseHigh32 	= pgSockMsgOut->iMsgPara8;
			tableInfo.data.almThrTpSubTypeDnRiseLow32 	= pgSockMsgOut->iMsgPara9;
			tableInfo.data.almThrTpSubTypeDnClrHigh32 	= pgSockMsgOut->iMsgParaA;
			tableInfo.data.almThrTpSubTypeDnClrLow32 	= pgSockMsgOut->iMsgParaB;
			tableInfo.data.rowStatus 		 			= ALM_SNMP_ROW_ACTIVE;
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += unitLen;
			table_cnt++;

			if((GPN_STAT_INVALID_TEMPLAT_ID == pgSockMsgOut->iMsgParaC) \
				&& (GPN_STAT_SUB_TYPE_INVALID == pgSockMsgOut->iMsgParaD))	//search done
			{
				break;
			}

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (pgSockMsgIn, 0, inLen);
			
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_STAT_MSG_ALM_THRED_TEMP_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= pgSockMsgOut->iMsgParaC;
			gSockMsgIn.iMsgPara2	= pgSockMsgOut->iMsgParaD;

			memset (pgSockMsgOut, 0, outLen);
			
			ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
		{
			break;
		}
	}

	//zlog_debug(0, "%s[%d] : table_cnt[%d], max[%d]\n", __func__, __LINE__, table_cnt, max_table_cnt);	
	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	iCnt_read = 0;
}


static void stat_snmp_sub_type_report_tp_get_bulk (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stSubTypeReportTmpTab_index *pIndex = (stSubTypeReportTmpTab_index *)pmsg->msg_data;
	
	static int iCnt_read = 0;	//0:first time to read; 1:other time to read
	
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);
	size_t unitLen = sizeof(stSubTypeReportTmpTab_info);
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, inLen);
	memset (pgSockMsgOut, 0, outLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stSubTypeReportTmpTab_info tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType 	= GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->subReportTmpIndex;
	gSockMsgIn.iMsgPara2	= pIndex->subReportSubType;

	uint32_t ret = gpnStatAgentXIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while (GPN_STAT_SNMP_GEN_OK == ret)
	{
		//zlog_debug(0, "%s[%d] : iCnt_read = %d\n", __func__, __LINE__, iCnt_read);
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT_RSP)
		{
			if(0 == iCnt_read)
			{
				iCnt_read = 1;
				
				pgSockMsgIn->iIndex		= 0;
				pgSockMsgIn->iMsgType	= GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT;
				pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
				pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
				pgSockMsgIn->iMsgPara1	= pgSockMsgOut->iMsgPara5;
				pgSockMsgIn->iMsgPara2	= pgSockMsgOut->iMsgPara6;

				if((GPN_STAT_INVALID_TEMPLAT_ID == pgSockMsgOut->iMsgPara5) \
					&& (GPN_STAT_SUB_TYPE_INVALID == pgSockMsgOut->iMsgPara6))//search done
				{
					break;
				}
				
				ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
				continue;
			}
			
			memset (&tableInfo, 0, unitLen);
			tableInfo.index.subReportTmpIndex = pgSockMsgOut->iMsgPara1;
			tableInfo.index.subReportSubType  = pgSockMsgOut->iMsgPara2;			
			tableInfo.data.subReportScanType  = pgSockMsgOut->iMsgPara3;
			tableInfo.data.subReportStatus 	  = pgSockMsgOut->iMsgPara4;
			tableInfo.data.rowStatus 		  = ALM_SNMP_ROW_ACTIVE;
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += unitLen;
			table_cnt++;

			if((GPN_STAT_INVALID_TEMPLAT_ID == pgSockMsgOut->iMsgPara5) \
				&& (GPN_STAT_SUB_TYPE_INVALID == pgSockMsgOut->iMsgPara6))	//search done
			{
				break;
			}

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (pgSockMsgIn, 0, inLen);
			
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_STAT_MSG_SUBREPORT_TEMP_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= pgSockMsgOut->iMsgPara5;
			gSockMsgIn.iMsgPara2	= pgSockMsgOut->iMsgPara6;

			memset (pgSockMsgOut, 0, outLen);
			
			ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
		{
			break;
		}
	}

	//zlog_debug(0, "%s[%d] : table_cnt[%d], max[%d]\n", __func__, __LINE__, table_cnt, max_table_cnt);	
	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	iCnt_read = 0;
}


static void stat_snmp_hist_data_get_bulk (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stHistDataTab_index *pIndex = (stHistDataTab_index *)pmsg->msg_data;
	
	static int iCnt_read = 0;	//0:first time to read; 1:other time to read
	
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);
	size_t unitLen = sizeof(stHistDataTab_info);
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, inLen);
	memset (pgSockMsgOut, 0, outLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stHistDataTab_info tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType 	= GPN_STAT_MSG_HIST_DATA_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->histDataIndex;
	gSockMsgIn.iMsgPara2	= pIndex->histDataStopTime;
	gSockMsgIn.iMsgPara3	= pIndex->histStatType;

	uint32_t ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while (GPN_STAT_SNMP_GEN_OK == ret)
	{
		//zlog_debug(0, "%s[%d] : iCnt_read = %d\n", __func__, __LINE__, iCnt_read);
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_HIST_DATA_GET_NEXT_RSP)
		{		
			if((0 == iCnt_read) && ((0xffffffff != pgSockMsgIn->iMsgPara1) ||
									(0xffffffff != pgSockMsgIn->iMsgPara2) ||
									(0xffffffff != pgSockMsgIn->iMsgPara3)))
			{
				iCnt_read = 1;
				
				pgSockMsgIn->iIndex		= 0;
				pgSockMsgIn->iMsgType	= GPN_STAT_MSG_HIST_DATA_GET_NEXT;
				pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
				pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
				pgSockMsgIn->iMsgPara1	= pgSockMsgOut->iMsgPara7;
				pgSockMsgIn->iMsgPara2	= pgSockMsgOut->iMsgPara8;
				pgSockMsgIn->iMsgPara3	= pgSockMsgOut->iMsgPara9;

				if((GPN_STAT_INVALID_TEMPLAT_ID == pgSockMsgOut->iMsgPara7) \
					&& (GPN_STAT_DBS_32_FFFF == pgSockMsgOut->iMsgPara8) \
					&& (GPN_STAT_SUB_TYPE_INVALID == pgSockMsgOut->iMsgPara9))//search done
				{
					break;
				}
				
				ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
				continue;
			}
			
			memset (&tableInfo, 0, unitLen);
			tableInfo.index.histDataIndex 	 = pgSockMsgOut->iMsgPara1;
			tableInfo.index.histDataStopTime = pgSockMsgOut->iMsgPara2;
			tableInfo.index.histStatType 	 = pgSockMsgOut->iMsgPara3;
			
			tableInfo.data.histDSScanType 	  = pgSockMsgOut->iMsgPara4;
			tableInfo.data.histTypeDataHigh32 = pgSockMsgOut->iMsgPara5;
			tableInfo.data.histTypeDataLow32  = pgSockMsgOut->iMsgPara6;
			tableInfo.data.rowStatus 		  = ALM_SNMP_ROW_ACTIVE;
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += unitLen;
			table_cnt++;

			if((GPN_STAT_INVALID_TEMPLAT_ID == pgSockMsgOut->iMsgPara7) \
				&& (GPN_STAT_DBS_32_FFFF == pgSockMsgOut->iMsgPara8) \
				&& (GPN_STAT_SUB_TYPE_INVALID == pgSockMsgOut->iMsgPara9))	//search done
			{
				break;
			}

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (pgSockMsgIn, 0, inLen);
			
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_STAT_MSG_HIST_DATA_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= pgSockMsgOut->iMsgPara7;
			gSockMsgIn.iMsgPara2	= pgSockMsgOut->iMsgPara8;
			gSockMsgIn.iMsgPara3	= pgSockMsgOut->iMsgPara9;

			memset (pgSockMsgOut, 0, outLen);
			
			ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
		{
			break;
		}
	}

	//zlog_debug(0, "%s[%d] : table_cnt[%d], max[%d]\n", __func__, __LINE__, table_cnt, max_table_cnt);	
	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);		
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	iCnt_read = 0;
}


static void stat_snmp_hist_data_report_in_stand_format_get_bulk (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	stHistDataReportFormatTab_index *pIndex = (stHistDataReportFormatTab_index *)pmsg->msg_data;
	
	static int iCnt_read = 0;	//0:first time to read; 1:other time to read
	
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);
	size_t unitLen = sizeof(stHistDataReportFormatTab_info);
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, inLen);
	memset (pgSockMsgOut, 0, outLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stHistDataReportFormatTab_info tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType 	= GPN_STAT_MSG_HIST_TRAP_DATA_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->histDataReportTaskBaseIndex;
	gSockMsgIn.iMsgPara2	= pIndex->histDataReportSubType;

	uint32_t ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while (GPN_STAT_SNMP_GEN_OK == ret)
	{
		//zlog_debug(0, "%s[%d] : iCnt_read = %d\n", __func__, __LINE__, iCnt_read);
		if (pgSockMsgOut->iMsgType == GPN_STAT_MSG_HIST_TRAP_DATA_GET_NEXT_RSP)
		{
			if(0 == iCnt_read)
			{
				iCnt_read = 1;
				
				pgSockMsgIn->iIndex		= 0;
				pgSockMsgIn->iMsgType	= GPN_STAT_MSG_HIST_TRAP_DATA_GET_NEXT;
				pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
				pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
				pgSockMsgIn->iMsgPara1	= pgSockMsgOut->iMsgParaE;
				pgSockMsgIn->iMsgPara2	= pgSockMsgOut->iMsgParaF;

				if((GPN_STAT_INVALID_TEMPLAT_ID == pgSockMsgOut->iMsgParaE) \
					&& (GPN_STAT_SUB_TYPE_INVALID == pgSockMsgOut->iMsgParaF))//search done
				{
					break;
				}
				
				ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
				continue;
			}
			
			memset (&tableInfo, 0, sizeof(stStatTaskTab_info));
			tableInfo.index.histDataReportTaskBaseIndex	= pgSockMsgOut->iMsgPara1;
			tableInfo.index.histDataReportSubType		= pgSockMsgOut->iMsgPara2;
			tableInfo.data.histDRStopTime		= pgSockMsgOut->iMsgPara3;
			tableInfo.data.histDRScanType		= pgSockMsgOut->iMsgPara4;
			tableInfo.data.histDRCycType		= pgSockMsgOut->iMsgPara5;
			tableInfo.data.histDRPortIndex1		= pgSockMsgOut->iMsgPara6;
			tableInfo.data.histDRPortIndex2		= pgSockMsgOut->iMsgPara7;
			tableInfo.data.histDRPortIndex3		= pgSockMsgOut->iMsgPara8;
			tableInfo.data.histDRPortIndex4		= pgSockMsgOut->iMsgPara9;
			tableInfo.data.histDRPortIndex5		= pgSockMsgOut->iMsgParaA;
			tableInfo.data.histDRDevIndex		= pgSockMsgOut->iMsgParaB;
			tableInfo.data.histDRTypeHigh32		= pgSockMsgOut->iMsgParaC;
			tableInfo.data.histDRTypeLow32		= pgSockMsgOut->iMsgParaD;
			tableInfo.data.rowStatus			= ALM_SNMP_ROW_ACTIVE;
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += sizeof (stStatTaskTab_info);
			table_cnt++;

			if((GPN_STAT_INVALID_TEMPLAT_ID == pgSockMsgOut->iMsgParaE) \
				&& (GPN_STAT_SUB_TYPE_INVALID == pgSockMsgOut->iMsgParaF))	//search done
			{
				break;
			}

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (pgSockMsgIn, 0, inLen);
			
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_STAT_MSG_HIST_TRAP_DATA_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= pgSockMsgOut->iMsgParaE;
			gSockMsgIn.iMsgPara2	= pgSockMsgOut->iMsgParaF;

			memset (pgSockMsgOut, 0, outLen);
			
			ret = gpnStatAgentXIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
		{
			break;
		}
	}

	//zlog_debug(0, "%s[%d] : table_cnt[%d], max[%d]\n", __func__, __LINE__, table_cnt, max_table_cnt);	
	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	iCnt_read = 0;
}




/* ��ȡ table ���� */
static void stat_snmp_get_bulk (struct ipc_mesg_n *pmsg)
{
	zlog_debug(STATIS_DBG_GET_NEXT, "%s[%d] : get-bulk stat table %d\n", __func__, __LINE__,
		pmsg->msghdr.msg_subtype - SNMP_STAT_GLOBAL_SCAN_TYPE_ATTRI_TABLE + 1);
	
	switch (pmsg->msghdr.msg_subtype)
	{
		case SNMP_STAT_GLOBAL_SCAN_TYPE_ATTRI_TABLE:
			stat_snmp_global_scan_type_attri_get_bulk (pmsg);
			break;

		case SNMP_STAT_GLOBAL_SUB_TYPE_ATTRI_TABLE:
			stat_snmp_global_sub_type_attri_get_bulk (pmsg);
			break;

		case SNMP_STAT_PORT_TYPE_TO_STAT_SCAN_TYPE_TABLE:
			stat_snmp_port_type_to_scan_type_get_bulk (pmsg);
			break;

		case SNMP_STAT_PORT_MONI_CTRL_TABLE:
			stat_snmp_port_moni_ctrl_get_bulk (pmsg);
			break;

		case SNMP_CURR_STAT_TABLE:
			stat_snmp_curr_stat_get_bulk (pmsg);
			break;

		case SNMP_STAT_TASK_TABLE:
			stat_snmp_task_get_bulk (pmsg);
			break;

		case SNMP_STAT_EVENT_THRESHOLD_TEMPLAT_TABLE:
			stat_snmp_evt_thr_tmp_get_bulk (pmsg);
			break;

		case SNMP_STAT_SUB_STAT_TYPE_FILT_TP_TABLE:
			stat_snmp_sub_stat_type_filt_tp_get_bulk (pmsg);
			break;

		case SNMP_STAT_ALARM_THRESHOLD_TEMPLAT_TABLE:
			stat_snmp_alm_td_tp_get_bulk (pmsg);
			break;

		case SNMP_STAT_SUB_STAT_TYPE_REPORT_TEMPLAT_TABLE:
			stat_snmp_sub_type_report_tp_get_bulk (pmsg);
			break;

		case SNMP_HIST_STAT_DATA_TABLE:
			stat_snmp_hist_data_get_bulk (pmsg);
			break;

		case SNMP_HIST_DATA_REPORT_IN_STAND_FORMAT_TABLE:
			stat_snmp_hist_data_report_in_stand_format_get_bulk (pmsg);
			break;

		default:
			/*if(ipc_send_noack (0, pmsg->msghdr.sender_id, pmsg->msghdr.module_id, pmsg->msghdr.msg_type,
						pmsg->msghdr.msg_subtype, pmsg->msghdr.msg_index) < 0)
			{
				zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
			}*/

			ipc_noack_to_snmp(&pmsg->msghdr);
			break;
	}
}



void stat_handle_snmp_msg (struct ipc_mesg_n *pmsg)
{
	switch (pmsg->msghdr.opcode)
	{
		case IPC_OPCODE_GET:
			stat_snmp_get (pmsg);
			break;

		case IPC_OPCODE_GET_BULK:
			stat_snmp_get_bulk (pmsg);
			break;

		case IPC_OPCODE_UPDATE:
		case IPC_OPCODE_ADD:
		case IPC_OPCODE_DELETE:
			stat_snmp_set (pmsg);
			break;

		default:
			break;
	}
}


void stat_send_trap (char *buf, int buf_len, int subtype)
{
	//ipc_send_common ((void *)buf, buf_len, 1, MODULE_ID_SNMPD, MODULE_ID_STAT, IPC_TYPE_SNMP, subtype, 0);

	int  iRetVal = 0;
    int  iDatalen = buf_len;
    
    struct ipc_mesg_n *pSndMsg = NULL;

    pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + iDatalen, MODULE_ID_SNMPD_TRAP);
	if(pSndMsg == NULL) 
	{
		printf("%s[%d] : mem share malloc error\n", __func__, __LINE__);
		return;
    }
	
	memset(pSndMsg, 0, (sizeof(struct ipc_msghdr_n) + iDatalen));

    pSndMsg->msghdr.module_id   = MODULE_ID_SNMPD_TRAP;
    pSndMsg->msghdr.sender_id   = MODULE_ID_STAT;
    pSndMsg->msghdr.msg_type    = IPC_TYPE_SNMP;
    pSndMsg->msghdr.msg_subtype = subtype;
    pSndMsg->msghdr.opcode      = 0;
    pSndMsg->msghdr.data_num    = 1;
    pSndMsg->msghdr.data_len    = (unsigned int)iDatalen;

	memcpy(pSndMsg->msg_data, buf, iDatalen);

    iRetVal = ipc_send_msg_n1(pSndMsg, (sizeof(struct ipc_msghdr_n) + iDatalen));

	/* ����ʧ�ܣ���Ҫ�����ͷŹ����ڴ� */
    if(-1 == iRetVal)	
    {
		mem_share_free(pSndMsg, MODULE_ID_STAT);
    }
}


uint32_t send_hist_data_trap(stStatTrapDataTable *pTable)
{	
	stat_send_trap((char *)pTable, sizeof(stStatTrapDataTable), 0);
	return 0;
}

//------------------------- end -----------------------------//


