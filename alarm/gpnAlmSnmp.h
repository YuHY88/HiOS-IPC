/******************************************************************************
 * Filename: gpnAlmSnmp.h
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

#ifndef _ALARM_SNMP_H_
#define _ALARM_SNMP_H_


#include "lib/msg_ipc_n.h"
#include "lib/pkt_buffer.h"
#include "lib/types.h"

#include "gpnAlmDataStructApi.h"



/* alarm subtype */
enum ALARM_SNMP_SUBTYPE
{
	/********** scaler subtype ***********/
	SNMP_ALM_PRODUCT_DELAY_TIME,
	SNMP_ALM_DISAPPEAR_DELAY_TIME,
	
	SNMP_ALM_BUZZER_EN,
	SNMP_ALM_BUZZER_CLEAR,
	SNMP_ALM_BUZZER_THRESHOLD,
	
	SNMP_ALM_CURR_ALM_DS_SIZE,
	SNMP_ALM_CURR_ALM_DS_CIRCLE,

	SNMP_ALM_HIST_ALM_DS_SIZE,
	SNMP_ALM_HIST_ALM_DS_CIRCLE,

	SNMP_ALM_EVENT_DS_SIZE,
	SNMP_ALM_EVENT_DS_CIRCLE,

	SNMP_ALM_ATTRIB_CFG_NUM,

	SNMP_ALM_PORT_BASE_MONITOR_CFG_NUM,

	SNMP_ALM_RESTRAIN,
	SNMP_ALM_REVERSESAL_MODE,

	/********** table subtype ***********/
	SNMP_ALM_PORT_MONITOR_CFG_TABLE,
	SNMP_ALM_ATTRIBUTE_CFG_TABLE,
	SNMP_ALM_CURR_DATA_SHEET_TABLE,
	SNMP_ALM_HIST_DATA_SHEET_TABLE,
	SNMP_ALM_EVENT_DATA_SHEET_TABLE,

	/************** trap ****************/
	SNMP_ALM_PRODUCT_TRAP,
	SNMP_ALM_DISAPPEAR_TRAP,
	SNMP_ALM_EVENT_REPORT_TRAP,
	SNMP_ALM_REVERSE_AUTO_REC_TRAP,
};


/* trap subtype */
#define ALM_PRODUCT_TRAP				1
#define ALM_DISAPPEAR_TRAP				2
#define ALM_EVENT_REPORT_TRAP			3
#define ALM_REVERSE_AUTO_REC_TRAP		4


/**************************** alarmPortMonitorCfg Table **********************************/

typedef struct _snmp_alm_port_monitor_cfg_table_index
{
	uint32_t iDevIndex;
	uint32_t iPortIndex1;
	uint32_t iPortIndex2;
	uint32_t iPortIndex3;
	uint32_t iPortIndex4;
	uint32_t iPortIndex5;
	
}stSnmpAlmPortMonitorCfgTableIndex;

typedef struct _snmp_alm_port_monitor_cfg_table_data
{
	uint32_t iAlmMonitorEN;
	uint32_t rowStatus;
	
}stSnmpAlmPortMonitorCfgTableData;

typedef struct _snmp_alm_port_monitor_cfg_table_info
{
	stSnmpAlmPortMonitorCfgTableIndex 	index;
	stSnmpAlmPortMonitorCfgTableData	data;

	u_int32_t		flagsOfColumnSet;
}stSnmpAlmPortMonitorCfgTableInfo;



/**************************** alarmAttributeCfg Table **********************************/

typedef struct _snmp_alm_attribute_cfg_table_index
{
	uint32_t	iAlmSubType;
	uint32_t 	iAttrDevIndex;
	uint32_t 	iAttrPortIndex1;
	uint32_t 	iAttrPortIndex2;
	uint32_t 	iAttrPortIndex3;
	uint32_t 	iAttrPortIndex4;
	uint32_t 	iAttrPortIndex5;	
	
}stSnmpAlmAttribCfgTableIndex;

typedef struct _snmp_alm_attribute_cfg_table_data
{
	int			iBasePortMark;
	int			iAlmLevel;
	int			iAlmMask;
	int			iAlmFilter;
	int			iAlmReport;
	int			iAlmRecord;
	int			iPortAlmReverse;
	uint32_t 	rowStatus;
}stSnmpAlmAttribCfgTableData;

typedef struct _snmp_alm_attribute_cfg_table_info
{
	stSnmpAlmAttribCfgTableIndex 	index;
	stSnmpAlmAttribCfgTableData		data;

	u_int32_t		flagsOfColumnSet;
}stSnmpAlmAttribCfgTableInfo;


/**************************** alarmCurrDataSheet Table **********************************/

typedef struct _snmp_alm_curr_alm_data_sheet_index
{
	uint32_t 	iAlmCurrDSIndex;
	
}stSnmpAlmCurrDSTableIndex;

typedef struct _snmp_alm_curr_alm_data_sheet_data
{
	uint32_t 	iCurrAlmType;
	uint32_t 	iCurrDevIndex;
	uint32_t 	iCurrPortIndex1;
	uint32_t 	iCurrPortIndex2;
	uint32_t 	iCurrPortIndex3;
	uint32_t 	iCurrPortIndex4;
	uint32_t 	iCurrPortIndex5;
	uint32_t 	iCurrAlmLevel;
	uint32_t 	iAlmCurrProductCnt;
	uint32_t 	iAlmCurrFirstProductTime;
	uint32_t 	iAlmCurrThisProductTime;

	/* add for VPLS */
	uint32_t 	suffixLen;
	uint8_t 	almSuffix[GPN_ALM_DST_SUFFIX_LEN];
	
}stSnmpAlmCurrDSTableData;

typedef struct _snmp_alm_curr_alm_data_sheet_info
{
	stSnmpAlmCurrDSTableIndex 	index;
	stSnmpAlmCurrDSTableData	data;
	
}stSnmpAlmCurrDSTableInfo;


/**************************** alarmHistDataSheet Table **********************************/

typedef struct _snmp_alm_hist_alm_data_sheet_index
{
	uint32_t 	iAlmHistDSIndex;
	
}stSnmpAlmHistDSTableIndex;

typedef struct _snmp_alm_hist_alm_data_sheet_data
{
	uint32_t 	iHistAlmType;
	uint32_t 	iHistDevIndex;
	uint32_t 	iHistPortIndex1;
	uint32_t 	iHistPortIndex2;
	uint32_t 	iHistPortIndex3;
	uint32_t 	iHistPortIndex4;
	uint32_t 	iHistPortIndex5;
	uint32_t 	iHistAlmLevel;
	uint32_t 	iAlmHistProductCnt;
	uint32_t 	iAlmHistFirstProductTime;
	uint32_t 	iAlmHistThisProductTime;
	uint32_t 	iAlmHistThisDisappearTime;

	/* add for VPLS */
	uint32_t 	suffixLen;
	uint8_t 	almSuffix[GPN_ALM_DST_SUFFIX_LEN];
	
}stSnmpAlmHistDSTableData;

typedef struct _snmp_alm_hist_alm_data_sheet_info
{
	stSnmpAlmHistDSTableIndex 	index;
	stSnmpAlmHistDSTableData	data;
	
}stSnmpAlmHistDSTableInfo;


/**************************** eventDataSheet Table **********************************/

typedef struct _snmp_event_data_sheet_index
{
	uint32_t 	iEventDSIndex;
	
}stSnmpEventDSTableIndex;

typedef struct _snmp_event_data_sheet_data
{
	uint32_t 	iEventAlmType;
	uint32_t 	iEventDevIndex;
	uint32_t 	iEventPortIndex1;
	uint32_t 	iEventPortIndex2;
	uint32_t 	iEventPortIndex3;
	uint32_t 	iEventPortIndex4;
	uint32_t 	iEventPortIndex5;
	uint32_t 	iEventAlmLevel;
	uint32_t 	iEventDetail;
	uint32_t	iEventTime;
	
}stSnmpEventDSTableData;

typedef struct _snmp_event_data_sheet_info
{
	stSnmpEventDSTableIndex index;
	stSnmpEventDSTableData 	data;
	
}stSnmpEventDSTableInfo;


void alarm_handle_snmp_msg (struct ipc_mesg_n *pmsg);
void alarm_send_trap(void *pdata, int trapType);


#endif  /* _ALARM_SNMP_H_ */

