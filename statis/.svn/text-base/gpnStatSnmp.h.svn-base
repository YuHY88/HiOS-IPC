/******************************************************************************
 * Filename: gpnStatSnmp.h
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

#ifndef _STAT_SNMP_H_
#define _STAT_SNMP_H_


#include "lib/pkt_buffer.h"
#include "lib/types.h"

#include "lib/msg_ipc_n.h"

#include "gpnStatDataStruct.h"


/* stat subtype */
enum STAT_SNMP_SUBTYPE
{
	/********** scaler subtype ***********/
	SNMP_STAT_GLOBAL_TIME_OFFSET_POLARITY = 0,
	SNMP_STAT_GLOBAL_TIME_OFFSET_VALUE,
	
	SNMP_STAT_MON_OBJ_MAX_NUM,
	SNMP_STAT_ALREADY_MON_OBJ_NUM,
	SNMP_STAT_TASK_MAX_NUM,
	SNMP_STAT_ALREADY_RUN_TASK_NUM,
	SNMP_STAT_THRESHOLD_TEMPLAT_MAX_NUM,
	SNMP_STAT_ALREADY_THRESHOLD_TEMPLATE_NUM,
	SNMP_STAT_SUB_STAT_FILT_TP_MAX_NUM,
	SNMP_STAT_ALREADY_SUB_STAT_FILT_TP_NUM,
	SNMP_STAT_SUB_REPORT_TP_MAX_NUM,
	SNMP_STAT_ALREADY_SUB_REPORT_IP_NUM,

	SNMP_STAT_AUTO_REPORT_LAST_HIST,
	SNMP_STAT_AUTO_REPORT_OLD_HIST,
	SNMP_STAT_AUTO_NOTIFY_ALARM,
	SNMP_STAT_AUTO_NOTIFY_EVENT,

	SNMP_STAT_GET_STAT_TASK_ID,
	SNMP_STAT_GET_STAT_EVENT_THRESHOLD_TP_ID,
	SNMP_STAT_GET_MARK_SUB_TYPE_TP_ID,
	SNMP_STAT_GET_STAT_ALARM_THRESHOLD_TP_ID,
	SNMP_STAT_GET_SUB_TYPE_REPORT_TP_ID,

	SNMP_STAT_DATA_COLLECT_TYPE,

	/********** table subtype ***********/
	SNMP_STAT_GLOBAL_SCAN_TYPE_ATTRI_TABLE,
	SNMP_STAT_GLOBAL_SUB_TYPE_ATTRI_TABLE,
	SNMP_STAT_PORT_TYPE_TO_STAT_SCAN_TYPE_TABLE,
	SNMP_STAT_PORT_MONI_CTRL_TABLE,
	SNMP_CURR_STAT_TABLE,
	SNMP_STAT_TASK_TABLE,
	SNMP_STAT_EVENT_THRESHOLD_TEMPLAT_TABLE,
	SNMP_STAT_SUB_STAT_TYPE_FILT_TP_TABLE,
	SNMP_STAT_ALARM_THRESHOLD_TEMPLAT_TABLE,
	SNMP_STAT_SUB_STAT_TYPE_REPORT_TEMPLAT_TABLE,
	SNMP_HIST_STAT_DATA_TABLE,
	SNMP_HIST_DATA_REPORT_IN_STAND_FORMAT_TABLE,

	/************** trap ****************/
	SNMP_HIST_DATA_TRAP_IN_STAND_FORMAT_TYPE,	
};


/* StatGloScanTypeAttribTable */
typedef struct
{
	u_int32_t		typeIndex;
}stStatGloScanTypeAttribTable_index;

typedef struct
{
	u_int32_t		includeStatTypeNum;
	u_int32_t		subTypeNumInScanType;
	u_int32_t		longCycSeconds;
	u_int32_t		shortCycSeconds;
	u_int32_t		rowStatus;
}stStatGloScanTypeAttribTable_data;

typedef struct
{
	stStatGloScanTypeAttribTable_index	index;
	stStatGloScanTypeAttribTable_data	data;

	u_int32_t		flagsOfColumnSet;
}stStatGloScanTypeAttribTable_info;


/* GloSubTypeAttriTable */
typedef struct
{
	u_int32_t		subTypeIndex;
}stGloSubTypeAttribTab_index;

typedef struct
{
	u_int32_t		subTypeBelongScanType;
	u_int32_t		subTypeValueClass;
	u_int32_t		subTypeDataBitDeep;
	u_int32_t		longCycUpThrRelatAlarm;
	u_int32_t		longCycUpThrRelatEvent;
	u_int32_t		longCycLowThrRelatAlarm;
	u_int32_t		longCycLowThrRelatEvent;
	u_int32_t		shortCycUpThrRelatAlarm;
	u_int32_t		shortCycUpThrRelatEvent;
	u_int32_t		shortCycLowThrRelatAlarm;
	u_int32_t		shortCycLowThrRelatEvent;
	u_int32_t		usrDefCycUpThrRelatAlarm;
	u_int32_t		usrDefCycUpThrRelatEvent;
	u_int32_t		usrDefCycLowThrRelatAlarm;
	u_int32_t		usrDefCycLowThrRelatEvent;
	u_int32_t		longCycUpThrHigh32;
	u_int32_t		longCycUpThrLow32;
	u_int32_t		longCycLowThrHigh32;
	u_int32_t		longCycLowThrLow32;
	u_int32_t		shortCycUpThrHigh32;
	u_int32_t		shortCycUpThrLow32;
	u_int32_t		shortCycLowThrHigh32;
	u_int32_t		shortCycLowThrLow32;
	u_int32_t		rowStatus;
}stGloSubTypeAttribTab_data;

typedef struct
{
	stGloSubTypeAttribTab_index	index;
	stGloSubTypeAttribTab_data	data;

	u_int32_t		flagsOfColumnSet;
}stGloSubTypeAttribTab_info;


/* StatPortTypeToStatScanTypeTable */
typedef struct
{
	u_int32_t		portTypeIndex;
}stPortTypeToScanTypeTab_index;

typedef struct
{
	u_int32_t		portTypeAboutScanTypeNum;
	u_int32_t		type1Index;
	u_int32_t		type2Index;
	u_int32_t		type3Index;
	u_int32_t		type4Index;
	u_int32_t		type5Index;
	u_int32_t		rowStatus;
}stPortTypeToScanTypeTab_data;

typedef struct
{
	stPortTypeToScanTypeTab_index	index;
	stPortTypeToScanTypeTab_data	data;

	u_int32_t		flagsOfColumnSet;
}stPortTypeToScanTypeTab_info;


/* StatPortMoniCtrlTable */
#define PORT_MONI_INVALID				0
#define PORT_MONI_BASE_MON_OPT			(0x01 << 0)
#define PORT_MONI_PORT_LCYC_MON_OPT		(0x01 << 1)	//long cyc cfg
#define PORT_MONI_PORT_SCYC_MON_OPT		(0x01 << 2)	//short cyc cfg
#define PORT_MONI_PORT_UCYC_MON_OPT		(0x01 << 3)	//user-def cyc cfg


typedef struct
{
	u_int32_t		portMoniIndex1;
	u_int32_t		portMoniIndex2;
	u_int32_t		portMoniIndex3;
	u_int32_t		portMoniIndex4;
	u_int32_t		portMoniIndex5;
	u_int32_t		portMoniDevIndex;
	u_int32_t		portMoniScanType;
}stPortMoniCtrlTab_index;

typedef struct
{
	u_int32_t		moniEn;
	u_int32_t		currMoniEn;
	u_int32_t		longCycMoniEn;
	u_int32_t		longCycBelongTask;
	u_int32_t		longCycEvtThrLocation;
	u_int32_t		longCycSubTypeFiltLocation;
	u_int32_t		longCycAlmThrLocation;
	u_int32_t		longCycHistAutoReport;
	u_int32_t		longCycHistLocation;
	u_int32_t		shortCycMoniEn;
	u_int32_t		shortCycBelongTask;
	u_int32_t		shortCycEvtThrLocation;
	u_int32_t		shortCycSubTypeFiltLocation;
	u_int32_t		shortCycAlmThrLocation;
	u_int32_t		shortCycHistAutoReport;
	u_int32_t		shortCycHistLocation;
	u_int32_t		usrDefCycMoniEn;
	u_int32_t		usrDefCycBelongTask;
	u_int32_t		usrDefCycEvtThrLocation;
	u_int32_t		usrDefCycSubTypeFiltLocation;
	u_int32_t		usrDefCycAlmThrLocation;
	u_int32_t		usrDefCycHistAutoReport;
	u_int32_t		usrDefCycHistLocation;
	u_int32_t		usrDefCycHistSeconds;
	u_int32_t		rowStatus;
}stPortMoniCtrlTab_data;

typedef struct
{
	stPortMoniCtrlTab_index	index;
	stPortMoniCtrlTab_data	data;

	u_int32_t		flagsOfColumnSet;
}stPortMoniCtrlTab_info;


/* currStatTable */
typedef struct
{
	u_int32_t		currPortIndex1;
	u_int32_t		currPortIndex2;
	u_int32_t		currPortIndex3;
	u_int32_t		currPortIndex4;
	u_int32_t		currPortIndex5;
	u_int32_t		currDevIndex;
	u_int32_t		currDataType;
}stCurrStatTab_index;

typedef struct
{
	u_int32_t		clearCurrData;
	u_int32_t		currDataTypeValHigh32;
	u_int32_t		currDataTypeValLow32;
	u_int32_t		rowStatus;
}stCurrStatTab_data;

typedef struct
{
	stCurrStatTab_index	index;
	stCurrStatTab_data	data;

	u_int32_t		flagsOfColumnSet;
}stCurrStatTab_info;


/* statTaskTable */
typedef struct
{
	u_int32_t		taskIndex;
}stStatTaskTab_index;

typedef struct
{
	u_int32_t		maxPortInTask;
	u_int32_t		taskScanType;
	u_int32_t		includeMonObjNum;
	u_int32_t		taskCycMoniType;
	u_int32_t		taskMoniCycSec;
	u_int32_t		taskStartTime;
	u_int32_t		taskEndTime;
	u_int32_t		taskAlmThrTpLocation;
	u_int32_t		taskSubReportLocation;
	u_int32_t		rowStatus;
}stStatTaskTab_data;

typedef struct
{
	stStatTaskTab_index	index;
	stStatTaskTab_data	data;

	u_int32_t		flagsOfColumnSet;
}stStatTaskTab_info;


/* statEvtThrTemplatTable */
typedef struct
{
	u_int32_t		evtThrTmpIndex;
	u_int32_t		evtThrTmpTpSubType;
}stEvtThrTmpTab_index;

typedef struct
{
	u_int32_t		evtThrScanType;
	u_int32_t		evtThrTpSubTypeUpThrHigh32;
	u_int32_t		evtThrTpSubTypeUpThrLow32;
	u_int32_t		evtThrTpSubTypeLowThrHigh32;
	u_int32_t		evtThrTpSubTypeLowThrLow32;
	u_int32_t		rowStatus;
}stEvtThrTmpTab_data;

typedef struct
{
	stEvtThrTmpTab_index	index;
	stEvtThrTmpTab_data		data;

	u_int32_t		flagsOfColumnSet;
}stEvtThrTmpTab_info;


/* statSubStatTypeFileTpTable */
typedef struct
{
	u_int32_t		subTypeFiltIndex;
	u_int32_t		selectSubType;
}stSubStatTypeFileTpTab_index;

typedef struct
{
	u_int32_t		selectScanType;
	u_int32_t		selectStatus;
	u_int32_t		rowStatus;
}stSubStatTypeFileTpTab_data;

typedef struct
{
	stSubStatTypeFileTpTab_index	index;
	stSubStatTypeFileTpTab_data		data;

	u_int32_t		flagsOfColumnSet;
}stSubStatTypeFileTpTab_info;


/* statAlmThrTemplatTable */
typedef struct
{
	u_int32_t		almThrTmpIndex;
	u_int32_t		almThrTmpTpSubType;
}stAlmThrTmpTab_index;

typedef struct
{
	u_int32_t		almThrScanType;
	u_int32_t		almThrTpSubTypeUpRiseHigh32;
	u_int32_t		almThrTpSubTypeUpRiseLow32;
	u_int32_t		almThrTpSubTypeUpClrHigh32;
	u_int32_t		almThrTpSubTypeUpClrLow32;
	u_int32_t		almThrTpSubTypeDnRiseHigh32;
	u_int32_t		almThrTpSubTypeDnRiseLow32;
	u_int32_t		almThrTpSubTypeDnClrHigh32;
	u_int32_t		almThrTpSubTypeDnClrLow32;
	u_int32_t		rowStatus;
}stAlmThrTmpTab_data;

typedef struct
{
	stAlmThrTmpTab_index	index;
	stAlmThrTmpTab_data		data;

	u_int32_t		flagsOfColumnSet;
}stAlmThrTmpTab_info;


/* statSubStatTypeReportTemplatTable */
typedef struct
{
	u_int32_t		subReportTmpIndex;
	u_int32_t		subReportSubType;
}stSubTypeReportTmpTab_index;

typedef struct
{
	u_int32_t		subReportScanType;
	u_int32_t		subReportStatus;
	u_int32_t		rowStatus;
}stSubTypeReportTmpTab_data;

typedef struct
{
	stSubTypeReportTmpTab_index	index;
	stSubTypeReportTmpTab_data	data;

	u_int32_t		flagsOfColumnSet;
}stSubTypeReportTmpTab_info;


/* histStatDataTable */
typedef struct
{
	u_int32_t		histDataIndex;
	u_int32_t		histDataStopTime;
	u_int32_t		histStatType;
}stHistDataTab_index;

typedef struct
{
	u_int32_t		histDSScanType;
	u_int32_t		histTypeDataHigh32;
	u_int32_t		histTypeDataLow32;
	u_int32_t		rowStatus;
}stHistDataTab_data;

typedef struct
{
	stHistDataTab_index	index;
	stHistDataTab_data	data;

	u_int32_t		flagsOfColumnSet;
}stHistDataTab_info;


/* histDataReportInStandFormatTable */
typedef struct
{
	u_int32_t		histDataReportTaskBaseIndex;
	u_int32_t		histDataReportSubType;
}stHistDataReportFormatTab_index;

typedef struct
{
	u_int32_t		histDRStopTime;
	u_int32_t		histDRScanType;
	u_int32_t		histDRCycType;
	u_int32_t		histDRPortIndex1;
	u_int32_t		histDRPortIndex2;
	u_int32_t		histDRPortIndex3;
	u_int32_t		histDRPortIndex4;
	u_int32_t		histDRPortIndex5;
	u_int32_t		histDRDevIndex;
	u_int32_t		histDRTypeHigh32;
	u_int32_t		histDRTypeLow32;
	u_int32_t		rowStatus;
}stHistDataReportFormatTab_data;

typedef struct
{
	stHistDataReportFormatTab_index	index;
	stHistDataReportFormatTab_data	data;

	u_int32_t		flagsOfColumnSet;
}stHistDataReportFormatTab_info;



void stat_handle_snmp_msg (struct ipc_mesg_n *pmsg);
void stat_send_trap (char *buf, int buf_len, int subtype);
uint32_t send_hist_data_trap(stStatTrapDataTable *pTable);


#endif  /* _STAT_SNMP_H_ */

