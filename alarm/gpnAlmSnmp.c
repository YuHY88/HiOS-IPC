/******************************************************************************
 * Filename: gpnAlmSnmp.c
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.11.22  lipf created
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
#include <lib/memshare.h>
#include <lib/msg_ipc_n.h>
#include <lib/syslog.h>
#include <lib/log.h>
#include "lib/snmp_common.h"

#include "gpnAlmSnmp.h"
#include "gpnAlmApi.h"
#include "gpnAlmSnmpApi.h"
#include "gpnAlmDebug.h"

#include "socketComm/gpnSockAlmMsgDef.h"

/*local temp variable*/

#define	IPC_BUF_LEN  16000

static int   	int_value = 0;
static char		ipc_buf[IPC_BUF_LEN];

extern stEQUAlmProcSpace *pgstAlmPreScanWholwInfo;



static void alarm_snmp_port_monitor_cfg_get (struct ipc_msghdr_n *phdr, stSnmpAlmPortMonitorCfgTableIndex *pIndex)
{
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (pgSockMsgOut, 0, sizeof (gpnSockMsg));

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);

	stSnmpAlmPortMonitorCfgTableInfo tableInfo;
	memset (&tableInfo, 0, sizeof(stSnmpAlmPortMonitorCfgTableInfo));

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pIndex->iPortIndex1;
	pgSockMsgIn->iMsgPara2	= pIndex->iPortIndex2;
	pgSockMsgIn->iMsgPara3	= pIndex->iPortIndex3;
	pgSockMsgIn->iMsgPara4	= pIndex->iPortIndex4;
	pgSockMsgIn->iMsgPara5	= pIndex->iPortIndex5;
	pgSockMsgIn->iMsgPara6	= pIndex->iDevIndex;

	if(GPN_ALM_SNMP_OK == gpnAlmAgentxIoCtrl (pgSockMsgIn, inLen, pgSockMsgOut, outLen))
	{
		if (gSockMsgOut.iMsgType == GPN_ALM_MSG_PORT_MONI_DB_GET_RSP)
		{
			memset (&tableInfo, 0, sizeof (stSnmpAlmPortMonitorCfgTableInfo));
			tableInfo.index.iPortIndex1 	= gSockMsgOut.iMsgPara1;
			tableInfo.index.iPortIndex2 	= gSockMsgOut.iMsgPara2;
			tableInfo.index.iPortIndex3 	= gSockMsgOut.iMsgPara3;
			tableInfo.index.iPortIndex4 	= gSockMsgOut.iMsgPara4;
			tableInfo.index.iPortIndex5 	= gSockMsgOut.iMsgPara5;
			tableInfo.index.iDevIndex 		= gSockMsgOut.iMsgPara6;
			tableInfo.data.iAlmMonitorEN 	= gSockMsgOut.iMsgPara7;			
			
			/*ipc_send_reply (&tableInfo, sizeof(stSnmpAlmPortMonitorCfgTableInfo), phdr->sender_id, 
				phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);*/

			ipc_ack_to_snmp(phdr, (void *)&tableInfo, sizeof(stSnmpAlmPortMonitorCfgTableInfo), 1);
		}
	}

	/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
					phdr->msg_subtype, phdr->msg_index) < 0)
	{
		zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
	}*/

	ipc_noack_to_snmp(phdr);
}


static void alarm_snmp_attribute_cfg_get (struct ipc_msghdr_n *phdr, stSnmpAlmAttribCfgTableIndex *pIndex)
{
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (pgSockMsgOut, 0, sizeof (gpnSockMsg));

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);

	stSnmpAlmAttribCfgTableInfo tableInfo;
	memset (&tableInfo, 0, sizeof(stSnmpAlmAttribCfgTableInfo));

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pIndex->iAlmSubType;
	pgSockMsgIn->iMsgPara2	= pIndex->iAttrPortIndex1;
	pgSockMsgIn->iMsgPara3	= pIndex->iAttrPortIndex2;
	pgSockMsgIn->iMsgPara4	= pIndex->iAttrPortIndex3;
	pgSockMsgIn->iMsgPara5	= pIndex->iAttrPortIndex4;
	pgSockMsgIn->iMsgPara6	= pIndex->iAttrPortIndex5;
	pgSockMsgIn->iMsgPara7	= pIndex->iAttrDevIndex;

	
	if(GPN_ALM_SNMP_OK == gpnAlmAgentxIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen))
	{
		if (gSockMsgOut.iMsgType == GPN_ALM_MSG_ATTRIB_DB_GET_RSP)
		{
			tableInfo.index.iAlmSubType		= pgSockMsgOut->iMsgPara1;
			tableInfo.index.iAttrPortIndex1	= pgSockMsgOut->iMsgPara2;
			tableInfo.index.iAttrPortIndex2	= pgSockMsgOut->iMsgPara3;
			tableInfo.index.iAttrPortIndex3	= pgSockMsgOut->iMsgPara4;
			tableInfo.index.iAttrPortIndex4	= pgSockMsgOut->iMsgPara5;
			tableInfo.index.iAttrPortIndex5	= pgSockMsgOut->iMsgPara6;
			tableInfo.index.iAttrDevIndex	= pgSockMsgOut->iMsgPara7;
			tableInfo.data.iBasePortMark	= pgSockMsgOut->iMsgPara8;
			tableInfo.data.iAlmLevel		= pgSockMsgOut->iMsgPara9;
			tableInfo.data.iAlmMask 		= pgSockMsgOut->iMsgParaA;
			tableInfo.data.iAlmFilter		= pgSockMsgOut->iMsgParaB;
			tableInfo.data.iAlmReport		= pgSockMsgOut->iMsgParaC;
			tableInfo.data.iAlmRecord		= pgSockMsgOut->iMsgParaD;
			tableInfo.data.iPortAlmReverse	= pgSockMsgOut->iMsgParaE;
			
			/*ipc_send_reply (&tableInfo, sizeof(stSnmpAlmAttribCfgTableInfo), phdr->sender_id, 
				phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);*/

			ipc_ack_to_snmp(phdr, (void *)&tableInfo, sizeof(stSnmpAlmAttribCfgTableInfo), 1);
		}
	}

	/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
					phdr->msg_subtype, phdr->msg_index) < 0)
	{
		zlog_err("%s[%d] : stat send config info to snmpd failed\n", __FUNCTION__, __LINE__);
	}*/

	ipc_noack_to_snmp(phdr);
}





static void alarm_snmp_port_monitor_cfg_get_bulk (struct ipc_msghdr_n *phdr, stSnmpAlmPortMonitorCfgTableIndex *pIndex)
{
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);
	size_t unitLen = sizeof(stSnmpAlmPortMonitorCfgTableInfo);

	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset (pgSockMsgIn, 0, inLen);
	memset (pgSockMsgOut, 0, outLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stSnmpAlmPortMonitorCfgTableInfo tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pIndex->iPortIndex1;
	pgSockMsgIn->iMsgPara2	= pIndex->iPortIndex2;
	pgSockMsgIn->iMsgPara3	= pIndex->iPortIndex3;
	pgSockMsgIn->iMsgPara4	= pIndex->iPortIndex4;
	pgSockMsgIn->iMsgPara5	= pIndex->iPortIndex5;
	pgSockMsgIn->iMsgPara6	= pIndex->iDevIndex;

	uint32_t ret = gpnAlmAgentxIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while (GPN_ALM_SNMP_OK == ret)
	{
		if (gSockMsgOut.iMsgType == GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT_RSP)
		{
			memset (&tableInfo, 0, sizeof (stSnmpAlmPortMonitorCfgTableInfo));
			tableInfo.index.iPortIndex1  = pgSockMsgOut->iMsgPara1;
			tableInfo.index.iPortIndex2  = pgSockMsgOut->iMsgPara2;
			tableInfo.index.iPortIndex3  = pgSockMsgOut->iMsgPara3;
			tableInfo.index.iPortIndex4  = pgSockMsgOut->iMsgPara4;
			tableInfo.index.iPortIndex5  = pgSockMsgOut->iMsgPara5;
			tableInfo.index.iDevIndex 	 = pgSockMsgOut->iMsgPara6;
			tableInfo.data.iAlmMonitorEN = pgSockMsgOut->iMsgPara7;
			tableInfo.data.rowStatus 	 = 1;
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, sizeof (stSnmpAlmPortMonitorCfgTableInfo));
			buf_cnt += sizeof (stSnmpAlmPortMonitorCfgTableInfo);
			table_cnt++;

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));
			memset (&gSockMsgOut, 0, sizeof (gpnSockMsg));
			pgSockMsgIn->iIndex		= 0;
			pgSockMsgIn->iMsgType	= GPN_ALM_MSG_PORT_MONI_DB_GET_NEXT;
			pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
			pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
			pgSockMsgIn->iMsgPara1 	= tableInfo.index.iPortIndex1;
			pgSockMsgIn->iMsgPara2 	= tableInfo.index.iPortIndex2;
			pgSockMsgIn->iMsgPara3 	= tableInfo.index.iPortIndex3;
			pgSockMsgIn->iMsgPara4 	= tableInfo.index.iPortIndex4;
			pgSockMsgIn->iMsgPara5 	= tableInfo.index.iPortIndex5;
			pgSockMsgIn->iMsgPara6	= tableInfo.index.iDevIndex;
			
			ret = gpnAlmAgentxIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
			break;
	}

	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*sizeof (stSnmpAlmPortMonitorCfgTableInfo),
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : alarm send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : alarm send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}
}


static void alarm_snmp_attribute_cfg_get_bulk (struct ipc_msghdr_n *phdr, stSnmpAlmAttribCfgTableIndex *pIndex)
{
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	gpnSockMsg *pgSockMsgOut = &gSockMsgOut;
	memset(pgSockMsgIn, 0, sizeof (gpnSockMsg));
	memset(pgSockMsgOut, 0, sizeof (gpnSockMsg));

	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);
	size_t unitLen = sizeof(stSnmpAlmAttribCfgTableInfo);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);	

	stSnmpAlmAttribCfgTableInfo tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;	

	pgSockMsgIn->iIndex		= 0;
	pgSockMsgIn->iMsgType	= GPN_ALM_MSG_ATTRIB_DB_GET_NEXT;
	pgSockMsgIn->iSrcId		= GPN_COMMM_ALM;
	pgSockMsgIn->iDstId		= GPN_COMMM_ALM;
	pgSockMsgIn->iMsgPara1	= pIndex->iAlmSubType;
	pgSockMsgIn->iMsgPara2	= pIndex->iAttrPortIndex1;
	pgSockMsgIn->iMsgPara3	= pIndex->iAttrPortIndex2;
	pgSockMsgIn->iMsgPara4	= pIndex->iAttrPortIndex3;
	pgSockMsgIn->iMsgPara5	= pIndex->iAttrPortIndex4;
	pgSockMsgIn->iMsgPara6	= pIndex->iAttrPortIndex5;
	pgSockMsgIn->iMsgPara7	= pIndex->iAttrDevIndex;

	uint32_t ret = gpnAlmAgentxIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
	while (GPN_ALM_SNMP_OK == ret)
	{
		if (gSockMsgOut.iMsgType == GPN_ALM_MSG_ATTRIB_DB_GET_NEXT_RSP)
		{
			memset (&tableInfo, 0, sizeof (stSnmpAlmAttribCfgTableInfo));
			tableInfo.index.iAlmSubType 	= gSockMsgOut.iMsgPara1;
			tableInfo.index.iAttrPortIndex1	= gSockMsgOut.iMsgPara2;
			tableInfo.index.iAttrPortIndex2	= gSockMsgOut.iMsgPara3;
			tableInfo.index.iAttrPortIndex3	= gSockMsgOut.iMsgPara4;
			tableInfo.index.iAttrPortIndex4	= gSockMsgOut.iMsgPara5;
			tableInfo.index.iAttrPortIndex5 = gSockMsgOut.iMsgPara6;
			tableInfo.index.iAttrDevIndex 	= gSockMsgOut.iMsgPara7;
			tableInfo.data.iBasePortMark 	= gSockMsgOut.iMsgPara8;
			tableInfo.data.iAlmLevel	 	= gSockMsgOut.iMsgPara9;
			tableInfo.data.iAlmMask	 		= gSockMsgOut.iMsgParaA;
			tableInfo.data.iAlmFilter	 	= gSockMsgOut.iMsgParaB;
			tableInfo.data.iAlmReport 		= gSockMsgOut.iMsgParaC;
			tableInfo.data.iAlmRecord	 	= gSockMsgOut.iMsgParaD;
			tableInfo.data.iPortAlmReverse 	= gSockMsgOut.iMsgParaE;
			tableInfo.data.rowStatus 		= 1;

			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += sizeof(stSnmpAlmAttribCfgTableInfo);
			table_cnt++;

			if (table_cnt >= max_table_cnt)
			{
				break;
			}

			memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));
			memset (&gSockMsgOut, 0, sizeof (gpnSockMsg));			
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_ALM_MSG_ATTRIB_DB_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= tableInfo.index.iAlmSubType;
			gSockMsgIn.iMsgPara2	= tableInfo.index.iAttrPortIndex1;
			gSockMsgIn.iMsgPara3	= tableInfo.index.iAttrPortIndex2;
			gSockMsgIn.iMsgPara4	= tableInfo.index.iAttrPortIndex3;
			gSockMsgIn.iMsgPara5	= tableInfo.index.iAttrPortIndex4;
			gSockMsgIn.iMsgPara6	= tableInfo.index.iAttrPortIndex5;
			gSockMsgIn.iMsgPara7	= tableInfo.index.iAttrDevIndex;
			
			ret = gpnAlmAgentxIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);	
		}
		else
			break;
	}

	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*sizeof (stSnmpAlmAttribCfgTableInfo),
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : alarm send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : alarm send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}
}


static void alarm_snmp_curr_data_sheet_get_bulk (struct ipc_msghdr_n *phdr, stSnmpAlmCurrDSTableIndex *pIndex)
{
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg) + GPN_ALM_DST_SUFFIX_LEN;
	size_t unitLen = sizeof(stSnmpAlmCurrDSTableInfo);

	gpnSockMsg gSockMsgIn;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	memset (pgSockMsgIn, 0, inLen);
	
	gpnSockMsg *pgSockMsgOut = (gpnSockMsg *)XMALLOC(MTYPE_ALARM_ENTRY, outLen);
	memset (pgSockMsgOut, 0, outLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stSnmpAlmCurrDSTableInfo tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType		= GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->iAlmCurrDSIndex;
	gSockMsgIn.iMsgPara2	= GPN_ILLEGAL_PORT_INDEX;
	gSockMsgIn.iMsgPara3	= GPN_ILLEGAL_PORT_INDEX;
	gSockMsgIn.iMsgPara4	= GPN_ILLEGAL_PORT_INDEX;
	gSockMsgIn.iMsgPara5	= GPN_ILLEGAL_PORT_INDEX;
	gSockMsgIn.iMsgPara6	= GPN_ILLEGAL_PORT_INDEX;
	gSockMsgIn.iMsgPara7	= GPN_ILLEGAL_PORT_INDEX;

	uint32_t ret = gpnAlmAgentxIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);

	while (GPN_ALM_SNMP_OK == ret)
	{
		if (pgSockMsgOut->iMsgType == GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT_RSP)
		{
			memset (&tableInfo, 0, sizeof (stSnmpAlmCurrDSTableInfo));
			tableInfo.index.iAlmCurrDSIndex 		= pgSockMsgOut->iMsgPara1;
			tableInfo.data.iCurrAlmType 			= pgSockMsgOut->iMsgPara2;
			tableInfo.data.iCurrDevIndex 			= pgSockMsgOut->iMsgPara3;
			tableInfo.data.iCurrPortIndex1 			= pgSockMsgOut->iMsgPara4;
			tableInfo.data.iCurrPortIndex2 			= pgSockMsgOut->iMsgPara5;
			tableInfo.data.iCurrPortIndex3 			= pgSockMsgOut->iMsgPara6;
			tableInfo.data.iCurrPortIndex4 			= pgSockMsgOut->iMsgPara7;
			tableInfo.data.iCurrPortIndex5 			= pgSockMsgOut->iMsgPara8;
			tableInfo.data.iCurrAlmLevel			= pgSockMsgOut->iMsgPara9;
			tableInfo.data.iAlmCurrProductCnt 		= pgSockMsgOut->iMsgParaA;
			tableInfo.data.iAlmCurrFirstProductTime = pgSockMsgOut->iMsgParaB;
			tableInfo.data.iAlmCurrThisProductTime 	= pgSockMsgOut->iMsgParaC;
			tableInfo.data.suffixLen				= pgSockMsgOut->msgCellLen;
			memcpy(tableInfo.data.almSuffix, (uint8_t *)(&pgSockMsgOut->msgCellLen + 1),
				pgSockMsgOut->msgCellLen);
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += sizeof (stSnmpAlmCurrDSTableInfo);
			table_cnt++;

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (pgSockMsgIn, 0, inLen);
			memset (pgSockMsgOut, 0, outLen);
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_ALM_MSG_CURR_ALM_DB_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= tableInfo.index.iAlmCurrDSIndex;
			
			ret = gpnAlmAgentxIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
			break;
	}

	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : alarm send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : alarm send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	XFREE(MTYPE_ALARM_ENTRY, pgSockMsgOut);
}


static void alarm_snmp_hist_data_sheet_get_bulk (struct ipc_msghdr_n *phdr, stSnmpAlmHistDSTableIndex *pIndex)
{
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg) + GPN_ALM_DST_SUFFIX_LEN;
	size_t unitLen = sizeof(stSnmpAlmHistDSTableInfo);

	gpnSockMsg gSockMsgIn;
	gpnSockMsg *pgSockMsgIn = &gSockMsgIn;
	memset (pgSockMsgIn, 0, inLen);
	
	gpnSockMsg *pgSockMsgOut = (gpnSockMsg *)XMALLOC(MTYPE_ALARM_ENTRY, outLen);
	memset (pgSockMsgOut, 0, outLen);

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stSnmpAlmHistDSTableInfo tableInfo;
	memset (&tableInfo, 0, sizeof (stSnmpAlmHistDSTableInfo));

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/sizeof(stSnmpAlmHistDSTableInfo);

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType		= GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->iAlmHistDSIndex;

	uint32_t ret = gpnAlmAgentxIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);

	while (GPN_ALM_SNMP_OK == ret)
	{
		if (pgSockMsgOut->iMsgType == GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT_RSP)
		{
			memset (&tableInfo, 0, sizeof (stSnmpAlmHistDSTableInfo));
			tableInfo.index.iAlmHistDSIndex 		 = pgSockMsgOut->iMsgPara1;
			tableInfo.data.iHistAlmType 			 = pgSockMsgOut->iMsgPara2;
			tableInfo.data.iHistDevIndex 			 = pgSockMsgOut->iMsgPara3;
			tableInfo.data.iHistPortIndex1 			 = pgSockMsgOut->iMsgPara4;
			tableInfo.data.iHistPortIndex2 			 = pgSockMsgOut->iMsgPara5;
			tableInfo.data.iHistPortIndex3 			 = pgSockMsgOut->iMsgPara6;
			tableInfo.data.iHistPortIndex4 			 = pgSockMsgOut->iMsgPara7;
			tableInfo.data.iHistPortIndex5 			 = pgSockMsgOut->iMsgPara8;
			tableInfo.data.iHistAlmLevel			 = pgSockMsgOut->iMsgPara9;
			tableInfo.data.iAlmHistProductCnt 		 = pgSockMsgOut->iMsgParaA;
			tableInfo.data.iAlmHistFirstProductTime  = pgSockMsgOut->iMsgParaB;
			tableInfo.data.iAlmHistThisProductTime   = pgSockMsgOut->iMsgParaC;
			tableInfo.data.iAlmHistThisDisappearTime = pgSockMsgOut->iMsgParaD;
			tableInfo.data.suffixLen				 = pgSockMsgOut->msgCellLen;
			memcpy(tableInfo.data.almSuffix, (uint8_t *)(&pgSockMsgOut->msgCellLen + 1),
				pgSockMsgOut->msgCellLen);
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, unitLen);
			buf_cnt += unitLen;
			table_cnt++;

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset(pgSockMsgIn, 0, inLen);
			memset(pgSockMsgOut, 0, outLen);
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_ALM_MSG_HIST_ALM_DB_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= tableInfo.index.iAlmHistDSIndex;
			
			ret = gpnAlmAgentxIoCtrl(pgSockMsgIn, inLen, pgSockMsgOut, outLen);
		}
		else
			break;
	}

	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*unitLen,
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : alarm send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : alarm send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}

	XFREE(MTYPE_ALARM_ENTRY, pgSockMsgOut);
}


static void alarm_snmp_event_data_sheet_get_bulk (struct ipc_msghdr_n *phdr, stSnmpEventDSTableIndex *pIndex)
{
	size_t inLen = sizeof(gpnSockMsg);
	size_t outLen = sizeof(gpnSockMsg);
	size_t unitLen = sizeof(stSnmpEventDSTableInfo);
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (&gSockMsgOut, 0, sizeof (gpnSockMsg));

	//uchar ipc_buf[IPC_BUF_LEN];
	int buf_cnt = 0;
	memset (ipc_buf, 0, IPC_BUF_LEN);

	stSnmpEventDSTableInfo tableInfo;
	memset (&tableInfo, 0, unitLen);

	size_t table_cnt = 0;
	size_t max_table_cnt = IPC_BUF_LEN/unitLen;

	gSockMsgIn.iIndex		= 0;
	gSockMsgIn.iMsgType		= GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT;
	gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
	gSockMsgIn.iDstId		= GPN_COMMM_ALM;
	gSockMsgIn.iMsgPara1	= pIndex->iEventDSIndex;
	uint32_t ret = gpnAlmAgentxIoCtrl (&gSockMsgIn, inLen, &gSockMsgOut, outLen);

	while (GPN_ALM_SNMP_OK == ret)
	{
		if (gSockMsgOut.iMsgType == GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT_RSP)
		{
			memset (&tableInfo, 0, sizeof (stSnmpEventDSTableInfo));
			tableInfo.index.iEventDSIndex 	= gSockMsgOut.iMsgPara1;
			tableInfo.data.iEventAlmType 	= gSockMsgOut.iMsgPara2;
			tableInfo.data.iEventDevIndex   = gSockMsgOut.iMsgPara3;
			tableInfo.data.iEventPortIndex1 = gSockMsgOut.iMsgPara4;
			tableInfo.data.iEventPortIndex2 = gSockMsgOut.iMsgPara5;
			tableInfo.data.iEventPortIndex3 = gSockMsgOut.iMsgPara6;
			tableInfo.data.iEventPortIndex4 = gSockMsgOut.iMsgPara7;
			tableInfo.data.iEventPortIndex5 = gSockMsgOut.iMsgPara8;
			tableInfo.data.iEventAlmLevel	= gSockMsgOut.iMsgPara9;
			tableInfo.data.iEventDetail 	= gSockMsgOut.iMsgParaA;
			tableInfo.data.iEventTime 		= gSockMsgOut.iMsgParaB;
			
			memcpy (ipc_buf + buf_cnt, &tableInfo, sizeof (stSnmpEventDSTableInfo));
			buf_cnt += sizeof (stSnmpEventDSTableInfo);
			table_cnt++;

			if (table_cnt >= max_table_cnt)
			{
				break;
			}
			
			memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));
			memset (&gSockMsgOut, 0, sizeof (gpnSockMsg));
			gSockMsgIn.iIndex		= 0;
			gSockMsgIn.iMsgType		= GPN_ALM_MSG_EVT_DATA_DB_GET_NEXT;
			gSockMsgIn.iSrcId		= GPN_COMMM_ALM;
			gSockMsgIn.iDstId		= GPN_COMMM_ALM;
			gSockMsgIn.iMsgPara1	= tableInfo.index.iEventDSIndex;
			ret = gpnAlmAgentxIoCtrl (&gSockMsgIn, inLen, &gSockMsgOut, outLen);
		}
		else
			break;
	}

	if (table_cnt)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, table_cnt*sizeof (stSnmpEventDSTableInfo),
			table_cnt, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : alarm send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, table_cnt*unitLen, table_cnt);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : alarm send config info to snmpd failed\n", __FUNCTION__, __LINE__);
		}*/

		ipc_noack_to_snmp(phdr);
	}
}



/* alarm scalar get */
static void alarm_snmp_scalar_get (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (&gSockMsgOut, 0, sizeof (gpnSockMsg));

	gSockMsgIn.iIndex = 0;
	gSockMsgIn.iSrcId = GPN_COMMM_STAT;
	gSockMsgIn.iDstId = GPN_COMMM_STAT;

	switch(phdr->msg_subtype)
	{
		case SNMP_ALM_PRODUCT_DELAY_TIME:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_RISE_DELAY_GET;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			if (GPN_ALM_MSG_GLO_RISE_DELAY_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_ALM_DISAPPEAR_DELAY_TIME:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_CLEAN_DELAY_GET;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			if (GPN_ALM_MSG_GLO_CLEAN_DELAY_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_ALM_BUZZER_EN:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_RING_EN_GET;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			if (GPN_ALM_MSG_GLO_RING_EN_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_ALM_BUZZER_CLEAR:
			int_value = 0;
			break;

		case SNMP_ALM_BUZZER_THRESHOLD:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_RING_RANK_GET;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			if (GPN_ALM_MSG_GLO_RING_RANK_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;
			break;

		case SNMP_ALM_CURR_ALM_DS_SIZE:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_CURR_DB_SIZE_GET;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			if (GPN_ALM_MSG_CURR_DB_SIZE_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_ALM_CURR_ALM_DS_CIRCLE:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_CURR_DB_CYC_GET;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));

			if (GPN_ALM_MSG_CURR_DB_CYC_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;
			
			break;

		case SNMP_ALM_HIST_ALM_DS_SIZE:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_HIST_DB_SIZE_GET;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			if (GPN_ALM_MSG_HIST_DB_SIZE_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_ALM_HIST_ALM_DS_CIRCLE:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_HIST_DB_CYC_GET;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			if (GPN_ALM_MSG_HIST_DB_CYC_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_ALM_EVENT_DS_SIZE:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_EVT_DB_SIZE_GET;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			if (GPN_ALM_MSG_EVT_DB_SIZE_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_ALM_EVENT_DS_CIRCLE:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_EVT_DB_CYC_GET;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			if (GPN_ALM_MSG_EVT_DB_CYC_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_ALM_ATTRIB_CFG_NUM:
			int_value = pgstAlmPreScanWholwInfo->almGlobalCfg.almAttribCfgNum;
			break;

		case SNMP_ALM_PORT_BASE_MONITOR_CFG_NUM:
			int_value = pgstAlmPreScanWholwInfo->almGlobalCfg.almPortBaseMoniCfgNum;
			break;

		case SNMP_ALM_RESTRAIN:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_RSET_EN_GET;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			if (GPN_ALM_MSG_GLO_RSET_EN_GET_RSP == gSockMsgOut.iMsgType)
				int_value = gSockMsgOut.iMsgPara1;
			else
				int_value = 0;			
			break;

		case SNMP_ALM_REVERSESAL_MODE:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_REVER_EN_GET;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			if (GPN_ALM_MSG_GLO_REVER_EN_GET_RSP == gSockMsgOut.iMsgType)
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
		zlog_err ("%s[%d] : alarm send scalar to snmp failed\n", __FUNCTION__, __LINE__);
	}*/

	ipc_ack_to_snmp(phdr, (void *)&int_value, sizeof(int_value), 1);
	
	return;

table_get:
	switch(phdr->msg_subtype)
	{
		case SNMP_ALM_PORT_MONITOR_CFG_TABLE:
			alarm_snmp_port_monitor_cfg_get(phdr, (stSnmpAlmPortMonitorCfgTableIndex *)pmsg->msg_data);
			break;

		case SNMP_ALM_ATTRIBUTE_CFG_TABLE:
			alarm_snmp_attribute_cfg_get(phdr, (stSnmpAlmAttribCfgTableIndex *)pmsg->msg_data);
			break;

		default:
			break;
	}
}


/* alarm scalar set */
static void alarm_snmp_set (struct ipc_mesg_n *pmsg)
{
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	
	gpnSockMsg gSockMsgIn, gSockMsgOut;
	memset (&gSockMsgIn, 0, sizeof (gpnSockMsg));
	memset (&gSockMsgOut, 0, sizeof (gpnSockMsg));

	stSnmpAlmPortMonitorCfgTableInfo *pAlmPortMonitorCfgTableInfo = NULL;
	stSnmpAlmAttribCfgTableInfo *pAlmAttribCfgTableInfo = NULL;

	uint32_t iReVal = 0;
	memcpy (&iReVal, pmsg->msg_data, sizeof(uint32_t));

	gSockMsgIn.iIndex = 0;
	gSockMsgIn.iSrcId = GPN_COMMM_STAT;
	gSockMsgIn.iDstId = GPN_COMMM_STAT;
	gSockMsgIn.iMsgPara1 = iReVal;

	switch(phdr->msg_subtype)
	{
		case SNMP_ALM_PRODUCT_DELAY_TIME:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_RISE_DELAY_CFG;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			break;

		case SNMP_ALM_DISAPPEAR_DELAY_TIME:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_CLEAN_DELAY_CFG;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));		
			break;

		case SNMP_ALM_BUZZER_EN:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_RING_EN_CFG;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			break;

		case SNMP_ALM_BUZZER_CLEAR:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_RING_SURE;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			break;

		case SNMP_ALM_BUZZER_THRESHOLD:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_RING_RANK_CFG;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			break;

		case SNMP_ALM_CURR_ALM_DS_CIRCLE:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_CURR_DB_CYC_CFG;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			break;

		case SNMP_ALM_HIST_ALM_DS_CIRCLE:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_HIST_DB_CYC_CFG;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			break;

		case SNMP_ALM_EVENT_DS_CIRCLE:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_EVT_DB_CYC_CFG;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			break;

		case SNMP_ALM_RESTRAIN:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_RSET_EN_CFG;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			break;

		case SNMP_ALM_REVERSESAL_MODE:
			gSockMsgIn.iMsgType = GPN_ALM_MSG_GLO_REVER_EN_CFG;
			gpnAlmAgentxIoCtrl (&gSockMsgIn, sizeof (gpnSockMsg), &gSockMsgOut, sizeof (gpnSockMsg));
			break;
		
		default:
			goto table_set;
			break;
	}

	//int_value = TRUE;
	/*if (ipc_send_reply(&int_value, sizeof(int_value), pmsg->msghdr.sender_id,
					pmsg->msghdr.module_id, pmsg->msghdr.msg_type, 
					pmsg->msghdr.msg_subtype, pmsg->msghdr.msg_index) < 0)
	{
		zlog_err ("%s[%d] : alarm send scalar to snmp failed\n", __FUNCTION__, __LINE__);
	}*/

	//不做应答
	//ipc_ack_to_snmp(phdr, (void *)&int_value, sizeof(int_value), 1);
	
	return;

table_set:
	switch(pmsg->msghdr.msg_subtype)
	{
		case SNMP_ALM_PORT_MONITOR_CFG_TABLE:
			pAlmPortMonitorCfgTableInfo = (stSnmpAlmPortMonitorCfgTableInfo *)pmsg->msg_data;
			gSockMsgIn.iMsgType	 = GPN_ALM_MSG_PORT_ALM_MON_CFG;			
			gSockMsgIn.iMsgPara1 = pAlmPortMonitorCfgTableInfo->index.iPortIndex1;
			gSockMsgIn.iMsgPara2 = pAlmPortMonitorCfgTableInfo->index.iPortIndex2;
			gSockMsgIn.iMsgPara3 = pAlmPortMonitorCfgTableInfo->index.iPortIndex3;
			gSockMsgIn.iMsgPara4 = pAlmPortMonitorCfgTableInfo->index.iPortIndex4;
			gSockMsgIn.iMsgPara5 = pAlmPortMonitorCfgTableInfo->index.iPortIndex5;
			gSockMsgIn.iMsgPara6 = pAlmPortMonitorCfgTableInfo->index.iDevIndex;
			gSockMsgIn.iMsgPara7 = pAlmPortMonitorCfgTableInfo->data.iAlmMonitorEN;
			gpnAlmAgentxIoCtrl(&gSockMsgIn, sizeof(gpnSockMsg), &gSockMsgOut, sizeof(gpnSockMsg));

			zlog_debug(ALARM_DBG_SET, "%s[%d] : set table 1--->index(%d|%d|%d|%d|%d|%d), data(%d)\n", __func__, __LINE__,
				gSockMsgIn.iMsgPara6, gSockMsgIn.iMsgPara1, gSockMsgIn.iMsgPara2,
				gSockMsgIn.iMsgPara3, gSockMsgIn.iMsgPara4, gSockMsgIn.iMsgPara5,
				gSockMsgIn.iMsgPara7);
			gpnAlmAgentxIoCtrl(&gSockMsgIn, sizeof(gpnSockMsg), &gSockMsgOut, sizeof(gpnSockMsg));
			break;

		case SNMP_ALM_ATTRIBUTE_CFG_TABLE:
			pAlmAttribCfgTableInfo = (stSnmpAlmAttribCfgTableInfo *)pmsg->msg_data;
			gSockMsgIn.iMsgType	 = GPN_ALM_MSG_ATTRIB_DB_MODIFY;
			gSockMsgIn.iMsgPara1 = pAlmAttribCfgTableInfo->index.iAlmSubType;			
			gSockMsgIn.iMsgPara2 = pAlmAttribCfgTableInfo->index.iAttrPortIndex1;
			gSockMsgIn.iMsgPara3 = pAlmAttribCfgTableInfo->index.iAttrPortIndex2;
			gSockMsgIn.iMsgPara4 = pAlmAttribCfgTableInfo->index.iAttrPortIndex3;
			gSockMsgIn.iMsgPara5 = pAlmAttribCfgTableInfo->index.iAttrPortIndex4;
			gSockMsgIn.iMsgPara6 = pAlmAttribCfgTableInfo->index.iAttrPortIndex5;
			gSockMsgIn.iMsgPara7 = pAlmAttribCfgTableInfo->index.iAttrDevIndex;
			gSockMsgIn.iMsgPara8 = pAlmAttribCfgTableInfo->data.iBasePortMark;
			gSockMsgIn.iMsgPara9 = pAlmAttribCfgTableInfo->data.iAlmLevel;
			gSockMsgIn.iMsgParaA = pAlmAttribCfgTableInfo->data.iAlmMask;
			gSockMsgIn.iMsgParaB = pAlmAttribCfgTableInfo->data.iAlmFilter;
			gSockMsgIn.iMsgParaC = pAlmAttribCfgTableInfo->data.iAlmReport;
			gSockMsgIn.iMsgParaD = pAlmAttribCfgTableInfo->data.iAlmRecord;
			gSockMsgIn.iMsgParaE = pAlmAttribCfgTableInfo->data.iPortAlmReverse;

			zlog_debug(ALARM_DBG_SET, "%s[%d] : set table 2--->data(%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
				gSockMsgIn.iMsgPara8, gSockMsgIn.iMsgPara9, gSockMsgIn.iMsgParaA,
				gSockMsgIn.iMsgParaB, gSockMsgIn.iMsgParaC, gSockMsgIn.iMsgParaD,
				gSockMsgIn.iMsgParaE);
			gpnAlmAgentxIoCtrl(&gSockMsgIn, sizeof(gpnSockMsg), &gSockMsgOut, sizeof(gpnSockMsg));
			break;

		default:
			break;
	}
}



/* 获取 table 数据 */
static void alarm_snmp_table_info_get (struct ipc_mesg_n *pmsg)
{
	switch (pmsg->msghdr.msg_subtype)
	{
		case SNMP_ALM_PORT_MONITOR_CFG_TABLE:
			alarm_snmp_port_monitor_cfg_get_bulk (&pmsg->msghdr, (stSnmpAlmPortMonitorCfgTableIndex *)pmsg->msg_data);
			break;

		case SNMP_ALM_ATTRIBUTE_CFG_TABLE:
			alarm_snmp_attribute_cfg_get_bulk (&pmsg->msghdr, (stSnmpAlmAttribCfgTableIndex *)pmsg->msg_data);
			break;

		case SNMP_ALM_CURR_DATA_SHEET_TABLE:
			alarm_snmp_curr_data_sheet_get_bulk (&pmsg->msghdr, (stSnmpAlmCurrDSTableIndex *)pmsg->msg_data);
			break;

		case SNMP_ALM_HIST_DATA_SHEET_TABLE:
			alarm_snmp_hist_data_sheet_get_bulk (&pmsg->msghdr, (stSnmpAlmHistDSTableIndex *)pmsg->msg_data);
			break;

		case SNMP_ALM_EVENT_DATA_SHEET_TABLE:
			alarm_snmp_event_data_sheet_get_bulk (&pmsg->msghdr, (stSnmpEventDSTableIndex *)pmsg->msg_data);
			break;

		default:
			break;
	}
}



void alarm_handle_snmp_msg (struct ipc_mesg_n *pmsg)
{
	switch (pmsg->msghdr.opcode)
	{
		case IPC_OPCODE_GET:
			alarm_snmp_scalar_get (pmsg);
			break;

		case IPC_OPCODE_GET_BULK:
			alarm_snmp_table_info_get (pmsg);
			break;

		case IPC_OPCODE_UPDATE:
			alarm_snmp_set (pmsg);
			break;

		default:
			break;
	}
}


void alarm_send_trap(void *pdata, int trapType)
{
	//ipc_send_common ((void *)buf, buf_len, 1, MODULE_ID_SNMPD, MODULE_ID_ALARM, IPC_TYPE_SNMP, subtype, 0);
	
	struct snmp_trap_hdr trap_hdr;
	struct snmp_trap_data trap_data;

	stSnmpAlmCurrDSTableInfo 	stCurrInfo;
	stSnmpAlmHistDSTableInfo 	stHistInfo;
	stSnmpEventDSTableInfo   	stEvtInfo;
	stSnmpAlmAttribCfgTableInfo stReverseInfo;
	
	static oid ALM_TRAP_PRODUCT_OID[]           = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 14, 7, 1};
	static oid ALM_TRAP_DISAPPEAR_OID[]         = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 14, 7, 2};
	static oid ALM_TRAP_EVENT_REPORT_OID[]      = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 14, 7, 3};
	static oid ALM_TRAP_REVERSE_AUTO_REC_OID[]  = {1, 3, 6, 1, 4, 1, 9966, 5, 35, 14, 7, 4};

	static oid alarmProductTrapRow_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 14, 11, 1, 1, 0, 0};
	static oid alarmDisappearTrapRow_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 14, 13, 1, 1, 0, 0};
	static oid alarmEventReportTrapRow_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 14, 15, 1, 1, 0, 0};
	static oid alarmReverseAutoRecTrapRow_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 14, 9, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};

	const struct trap_info currTrapInfo[] = {
		{ 1 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stCurrInfo.index.iAlmCurrDSIndex)), 			sizeof(stCurrInfo.index.iAlmCurrDSIndex) },
		{ 2 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stCurrInfo.data.iCurrAlmType)), 				sizeof(stCurrInfo.data.iCurrAlmType) },
		{ 3 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stCurrInfo.data.iCurrDevIndex)),				sizeof(stCurrInfo.data.iCurrDevIndex) },
		{ 4 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stCurrInfo.data.iCurrPortIndex1)),			sizeof(stCurrInfo.data.iCurrPortIndex1) },
		{ 5 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stCurrInfo.data.iCurrPortIndex2)),			sizeof(stCurrInfo.data.iCurrPortIndex2) },
		{ 6 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stCurrInfo.data.iCurrPortIndex3)),			sizeof(stCurrInfo.data.iCurrPortIndex3) },
		{ 7 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stCurrInfo.data.iCurrPortIndex4)),			sizeof(stCurrInfo.data.iCurrPortIndex4) },
		{ 8 , SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stCurrInfo.data.iCurrPortIndex5)),			sizeof(stCurrInfo.data.iCurrPortIndex5) },
		{ 9 , SNMP_TYPE_INTEGER,  (unsigned char *)(&(stCurrInfo.data.iCurrAlmLevel)),				sizeof(stCurrInfo.data.iCurrAlmLevel) },
		{ 10, SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stCurrInfo.data.iAlmCurrProductCnt)),			sizeof(stCurrInfo.data.iAlmCurrProductCnt) },
		{ 11, SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stCurrInfo.data.iAlmCurrFirstProductTime)),	sizeof(stCurrInfo.data.iAlmCurrFirstProductTime) },
		{ 12, SNMP_TYPE_UNSIGNED, (unsigned char *)(&(stCurrInfo.data.iAlmCurrThisProductTime)),	sizeof(stCurrInfo.data.iAlmCurrThisProductTime) },
	};

	const struct trap_info histTrapInfo[] =
    {
        { 1 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stHistInfo.index.iAlmHistDSIndex)),          sizeof(stHistInfo.index.iAlmHistDSIndex) },
        { 2 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stHistInfo.data.iHistAlmType)),              sizeof(stHistInfo.data.iHistAlmType) },
        { 3 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stHistInfo.data.iHistDevIndex)),             sizeof(stHistInfo.data.iHistDevIndex) },
        { 4 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stHistInfo.data.iHistPortIndex1)),           sizeof(stHistInfo.data.iHistPortIndex1) },
        { 5 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stHistInfo.data.iHistPortIndex2)),           sizeof(stHistInfo.data.iHistPortIndex2) },
        { 6 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stHistInfo.data.iHistPortIndex3)),           sizeof(stHistInfo.data.iHistPortIndex3) },
        { 7 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stHistInfo.data.iHistPortIndex4)),           sizeof(stHistInfo.data.iHistPortIndex4) },
        { 8 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stHistInfo.data.iHistPortIndex5)),           sizeof(stHistInfo.data.iHistPortIndex5) },
        { 9 , SNMP_TYPE_INTEGER    , (unsigned char *)(&(stHistInfo.data.iHistAlmLevel)),             sizeof(stHistInfo.data.iHistAlmLevel) },
        { 10, SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stHistInfo.data.iAlmHistProductCnt)),        sizeof(stHistInfo.data.iAlmHistProductCnt) },
        { 11, SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stHistInfo.data.iAlmHistFirstProductTime)),  sizeof(stHistInfo.data.iAlmHistFirstProductTime) },
        { 12, SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stHistInfo.data.iAlmHistThisProductTime)),   sizeof(stHistInfo.data.iAlmHistThisProductTime) },
        { 13, SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stHistInfo.data.iAlmHistThisDisappearTime)), sizeof(stHistInfo.data.iAlmHistThisDisappearTime) },
    };
	
	const struct trap_info evtTrapInfo[] =
    {
        { 1 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stEvtInfo.index.iEventDSIndex)),      sizeof(stEvtInfo.index.iEventDSIndex) },
        { 2 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stEvtInfo.data.iEventAlmType)),       sizeof(stEvtInfo.data.iEventAlmType) },
        { 3 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stEvtInfo.data.iEventDevIndex)),      sizeof(stEvtInfo.data.iEventDevIndex) },
        { 4 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stEvtInfo.data.iEventPortIndex1)),    sizeof(stEvtInfo.data.iEventPortIndex1) },
        { 5 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stEvtInfo.data.iEventPortIndex2)),    sizeof(stEvtInfo.data.iEventPortIndex2) },
        { 6 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stEvtInfo.data.iEventPortIndex3)),    sizeof(stEvtInfo.data.iEventPortIndex3) },
        { 7 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stEvtInfo.data.iEventPortIndex4)),    sizeof(stEvtInfo.data.iEventPortIndex4) },
        { 8 , SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stEvtInfo.data.iEventPortIndex5)),    sizeof(stEvtInfo.data.iEventPortIndex5) },
        { 9 , SNMP_TYPE_INTEGER    , (unsigned char *)(&(stEvtInfo.data.iEventAlmLevel)),      sizeof(stEvtInfo.data.iEventAlmLevel) },
        { 10, SNMP_TYPE_INTEGER    , (unsigned char *)(&(stEvtInfo.data.iEventDetail)),        sizeof(stEvtInfo.data.iEventDetail) },
        { 11, SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stEvtInfo.data.iEventTime)),          sizeof(stEvtInfo.data.iEventTime) },
    };

	const struct trap_info reverseTrapInfo[]      =
    {
        { 1, SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stReverseInfo.index.iAlmSubType)),        sizeof(stReverseInfo.index.iAlmSubType) },
        { 2, SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stReverseInfo.index.iAttrDevIndex)),      sizeof(stReverseInfo.index.iAttrDevIndex) },
        { 3, SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stReverseInfo.index.iAttrPortIndex1)),    sizeof(stReverseInfo.index.iAttrPortIndex1) },
        { 4, SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stReverseInfo.index.iAttrPortIndex2)),    sizeof(stReverseInfo.index.iAttrPortIndex2) },
        { 5, SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stReverseInfo.index.iAttrPortIndex3)),    sizeof(stReverseInfo.index.iAttrPortIndex3) },
        { 6, SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stReverseInfo.index.iAttrPortIndex4)),    sizeof(stReverseInfo.index.iAttrPortIndex4) },
        { 7, SNMP_TYPE_UNSIGNED   , (unsigned char *)(&(stReverseInfo.index.iAttrPortIndex5)),    sizeof(stReverseInfo.index.iAttrPortIndex5) },
    };

	int nodeNum = 0;
	int i = 0;
	size_t len = 0;

	uchar buf[2000];
	memset(buf, 0, 2000);

	switch(trapType)
	{
		case ALM_PRODUCT_TRAP:
			memcpy(&stCurrInfo, pdata, sizeof(stSnmpAlmCurrDSTableInfo));

			/* First : add trap msg header */
			trap_hdr.trap_oid = ALM_TRAP_PRODUCT_OID;
			trap_hdr.len_trap_oid = OID_LENGTH(ALM_TRAP_PRODUCT_OID);	
			if(0 == snmp_add_trap_msg_hdr(buf, &trap_hdr))
			{
				return;
			}

			/* Second : add trap msg data */
			nodeNum = sizeof(currTrapInfo)/sizeof(currTrapInfo[0]);
			
			for(i = 0; i < nodeNum; i++)
			{
				// index
				len = sizeof(alarmProductTrapRow_oid)/sizeof(alarmProductTrapRow_oid[0]);
				alarmProductTrapRow_oid[len - 2] = currTrapInfo[i].iOid;
				alarmProductTrapRow_oid[len - 1] = stCurrInfo.index.iAlmCurrDSIndex;

				trap_data.type 			= currTrapInfo[i].type;
				trap_data.len_data_oid 	= len;
				trap_data.len_data 		= currTrapInfo[i].len;
				trap_data.data_oid 		= alarmProductTrapRow_oid;
				trap_data.data 			= currTrapInfo[i].buf;

				if(0 == snmp_add_trap_msg_data(buf, &trap_data))
				{
					return;
				}
			}
		
			break;

		case ALM_DISAPPEAR_TRAP:
			memcpy(&stHistInfo, pdata, sizeof(stSnmpAlmHistDSTableInfo));
			
			trap_hdr.trap_oid = ALM_TRAP_DISAPPEAR_OID;
			trap_hdr.len_trap_oid = OID_LENGTH(ALM_TRAP_DISAPPEAR_OID);	
			if(0 == snmp_add_trap_msg_hdr(buf, &trap_hdr))
			{
				return;
			}

			nodeNum = sizeof(histTrapInfo)/sizeof(histTrapInfo[0]);
			
			for(i = 0; i < nodeNum; i++)
			{
				// index
				len = sizeof(alarmDisappearTrapRow_oid)/sizeof(alarmDisappearTrapRow_oid[0]);
				alarmDisappearTrapRow_oid[len - 2] = histTrapInfo[i].iOid;
				alarmDisappearTrapRow_oid[len - 1] = stHistInfo.index.iAlmHistDSIndex;

				trap_data.type 			= histTrapInfo[i].type;
				trap_data.len_data_oid 	= len;
				trap_data.len_data 		= histTrapInfo[i].len;
				trap_data.data_oid 		= alarmDisappearTrapRow_oid;
				trap_data.data 			= histTrapInfo[i].buf;

				if(0 == snmp_add_trap_msg_data(buf, &trap_data))
				{
					return;
				}
			}
			break;

		case ALM_EVENT_REPORT_TRAP:
			memcpy(&stEvtInfo, pdata, sizeof(stSnmpEventDSTableInfo));
			
			trap_hdr.trap_oid = ALM_TRAP_EVENT_REPORT_OID;
			trap_hdr.len_trap_oid = OID_LENGTH(ALM_TRAP_EVENT_REPORT_OID);	
			if(0 == snmp_add_trap_msg_hdr(buf, &trap_hdr))
			{
				return;
			}

			nodeNum = sizeof(evtTrapInfo)/sizeof(evtTrapInfo[0]);
			
			for(i = 0; i < nodeNum; i++)
			{
				// index
				len = sizeof(alarmEventReportTrapRow_oid)/sizeof(alarmEventReportTrapRow_oid[0]);
				alarmEventReportTrapRow_oid[len - 2] = evtTrapInfo[i].iOid;
				alarmEventReportTrapRow_oid[len - 1] = stEvtInfo.index.iEventDSIndex;

				trap_data.type 			= evtTrapInfo[i].type;
				trap_data.len_data_oid 	= len;
				trap_data.len_data 		= evtTrapInfo[i].len;
				trap_data.data_oid 		= alarmEventReportTrapRow_oid;
				trap_data.data 			= evtTrapInfo[i].buf;

				if(0 == snmp_add_trap_msg_data(buf, &trap_data))
				{
					return;
				}
			}
			break;

		case ALM_REVERSE_AUTO_REC_TRAP:
			memcpy(&stReverseInfo, pdata, sizeof(stSnmpAlmAttribCfgTableInfo));
			
			trap_hdr.trap_oid = ALM_TRAP_REVERSE_AUTO_REC_OID;
			trap_hdr.len_trap_oid = OID_LENGTH(ALM_TRAP_REVERSE_AUTO_REC_OID);	
			if(0 == snmp_add_trap_msg_hdr(buf, &trap_hdr))
			{
				return;
			}

			nodeNum = sizeof(reverseTrapInfo)/sizeof(reverseTrapInfo[0]);
			
			for(i = 0; i < nodeNum; i++)
			{
				// index
				len = sizeof(alarmReverseAutoRecTrapRow_oid)/sizeof(alarmReverseAutoRecTrapRow_oid[0]);
				alarmReverseAutoRecTrapRow_oid[len - 8] = reverseTrapInfo[i].iOid;
				alarmReverseAutoRecTrapRow_oid[len - 7] = stReverseInfo.index.iAlmSubType;
				alarmReverseAutoRecTrapRow_oid[len - 6] = stReverseInfo.index.iAttrDevIndex;
				alarmReverseAutoRecTrapRow_oid[len - 5] = stReverseInfo.index.iAttrPortIndex1;
				alarmReverseAutoRecTrapRow_oid[len - 4] = stReverseInfo.index.iAttrPortIndex2;
				alarmReverseAutoRecTrapRow_oid[len - 3] = stReverseInfo.index.iAttrPortIndex3;
				alarmReverseAutoRecTrapRow_oid[len - 2] = stReverseInfo.index.iAttrPortIndex4;
				alarmReverseAutoRecTrapRow_oid[len - 1] = stReverseInfo.index.iAttrPortIndex5;

				trap_data.type 			= reverseTrapInfo[i].type;
				trap_data.len_data_oid 	= len;
				trap_data.len_data 		= reverseTrapInfo[i].len;
				trap_data.data_oid 		= alarmReverseAutoRecTrapRow_oid;
				trap_data.data 			= reverseTrapInfo[i].buf;

				if(0 == snmp_add_trap_msg_data(buf, &trap_data))
				{
					return;
				}
			}
			break;

		default:
			break;
	}

	snmp_send_trap_msg(buf, MODULE_ID_ALARM, 0, 0);
}

//------------------------- end -----------------------------//


