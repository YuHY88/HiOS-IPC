/*
 * =====================================================================================
 *
 *       Filename:  mib_stat.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/14/2017 02:18:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <lib/types.h>

#include "net-snmp-config.h"
#include "types.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"

#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/memory.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/linklist.h>
#include <lib/log.h>

#include <lib/memtypes.h>
#include <lib/memshare.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>

#include "socketComm/gpnSockMsgDef.h"
#include "socketComm/gpnSockStatMsgDef.h"

#include "statis/gpnStatSnmp.h"

#include "mib_stat.h"
#include "ipran_snmp_data_cache.h"
//#include "lib/snmp_common.h"
#include "snmp_config_table.h"

/*local temp variable*/
static uchar    str_value[STRING_LEN] = {'\0'};
static uchar    *str = NULL;
static uchar    mac_value[6] = {0};
static int      int_value = 0;
static uint32_t uint_value = 0;
static uint32_t ip_value = 0;


/* table list define */
static struct ipran_snmp_data_cache *statGlobalScanTypeAttribTable_cache = NULL ;
static struct ipran_snmp_data_cache *statGlobalSubTypeAttribTable_cache = NULL ;
static struct ipran_snmp_data_cache *statPortTypeToStatScanTypeTable_cache = NULL ;
static struct ipran_snmp_data_cache *statPortMoniCtrlTable_chche = NULL ;
static struct ipran_snmp_data_cache *currStatTable_cache = NULL ;
static struct ipran_snmp_data_cache *statTaskTable_cache = NULL ;
static struct ipran_snmp_data_cache *statEventThresholdTemplatTable_cache = NULL ;
static struct ipran_snmp_data_cache *statSubStatTypeFileTpTable_cache = NULL ;
static struct ipran_snmp_data_cache *statAlarmThresholdTemplatTable_cache = NULL ;
static struct ipran_snmp_data_cache *statSubStatTypeReportTemplatTable_cache = NULL ;
static struct ipran_snmp_data_cache *histStatDataTable_cache = NULL ;
static struct ipran_snmp_data_cache *histDataReportInStandFormatTable_cache = NULL ;


stStatGloScanTypeAttribTable_info   statGlobalScanTypeAtrribTableList, scanTypeData;
stGloSubTypeAttribTab_info          statGlobalSubTypeAtrribTableList, subTypeData;
stPortTypeToScanTypeTab_info        statPortTypeToScanTypeTableList, portTypeToScanTypeData;
stPortMoniCtrlTab_info              statPortMoniCtrlTableList, portMoniCtrlData;
stCurrStatTab_info                  statCurrStatTableList, currStatData;
stStatTaskTab_info                  statStatTaskTableList, statTaskData;
stEvtThrTmpTab_info                 statEvtThrTmpTableList, evtThrTmpData;
stSubStatTypeFileTpTab_info         statSubStatTypeFileTpTableList, subStatTypeFileTpData;
stAlmThrTmpTab_info                 statAlmThrTmpTableList, almThrTmpData;
stSubTypeReportTmpTab_info          statSubTypeReportTmpTableList, subTypeReportTmpData;
stHistDataTab_info                  statHistDataTableList, histData;
stHistDataReportFormatTab_info      statHistDataReportFormatTableList, histDataReportFormatData;


/*status for modify the table*/
subTypeAtrribStatus subTypeStat;
portMoniCtrlStatus  portMoniStat;
currStatus          currStat;
statTaskStatus      taskStat;
eventThTpStatus     eventStat;
filtTpStatus        filtStat;
almThTpStatus       almStat;
reportTpStatus      repStat;


/*
 * Object ID definitions
 */
/*hhrStatGlobalTimeScalar*/
static oid hhrStatGlobalTimeScalar_oid[] = {HHRSTATCONFIG, 1, 1};
FindVarMethod hhrStatGlobalTimeScalar_get;
struct variable1 hhrStatGlobalTimeScalar_variables[] =
{
    {hhrStatGlobalTimeOffsetPolarity, ASN_INTEGER,  RONLY, hhrStatGlobalTimeScalar_get, 1, {1}},
    {hhrStatGlobalTimeOffsetValue,    ASN_UNSIGNED, RONLY, hhrStatGlobalTimeScalar_get, 1, {2}}
};

/*hhrStatMoniSourceScalar*/
static oid hhrStatMoniSourceScalar_oid[] = {HHRSTATCONFIG, 1, 2};
FindVarMethod hhrStatMoniSourceScalar_get;
struct variable1 hhrStatMoniSourceScalar_variables[] =
{
    {hhrStatMonObjNum,                ASN_INTEGER,  RONLY,  hhrStatMoniSourceScalar_get, 1, {1}},
    {hhrStatAlreadyMonObjNum,         ASN_UNSIGNED, RONLY,  hhrStatMoniSourceScalar_get, 1, {2}},
    {hhrStatTaskMaxNum,               ASN_INTEGER,  RONLY,  hhrStatMoniSourceScalar_get, 1, {3}},
    {hhrStatAlreadyRunTaskNum,        ASN_UNSIGNED, RONLY,  hhrStatMoniSourceScalar_get, 1, {4}},
    {hhrStatThdTmpMaxNum,             ASN_INTEGER,  RONLY,  hhrStatMoniSourceScalar_get, 1, {5}},
    {hhrStatAlreadyThdTmpMaxNum,      ASN_UNSIGNED, RONLY,  hhrStatMoniSourceScalar_get, 1, {6}},
    {hhrStatSubFiltTpMaxNum,          ASN_INTEGER,  RONLY,  hhrStatMoniSourceScalar_get, 1, {7}},
    {hhrStatAlreadySubFiltTpMaxNum,   ASN_UNSIGNED, RONLY,  hhrStatMoniSourceScalar_get, 1, {8}},
    {hhrStatSubReportTpMaxNum,        ASN_INTEGER,  RONLY,  hhrStatMoniSourceScalar_get, 1, {9}},
    {hhrStatAlreadySubReportTpMaxNum, ASN_UNSIGNED, RONLY,  hhrStatMoniSourceScalar_get, 1, {10}}
};

/*hhrStatAutoReportScalar*/
static oid hhrStatAutoReportScalar_oid[] = {HHRSTATCONFIG, 1, 3};
FindVarMethod hhrStatAutoReportScalar_get;
struct variable1 hhrStatAutoReportScalar_variables[] =
{
    {hhrStatAutoReportLastHist, ASN_INTEGER, RWRITE, hhrStatAutoReportScalar_get, 1, {1}},
    {hhrStatAutoReportOldHist,  ASN_INTEGER, RWRITE, hhrStatAutoReportScalar_get, 1, {2}},
    {hhrStatAutoNotifyAlarm,    ASN_INTEGER, RWRITE, hhrStatAutoReportScalar_get, 1, {3}},
    {hhrStatAutoNotifyEvent,    ASN_INTEGER, RWRITE, hhrStatAutoReportScalar_get, 1, {4}}
};

/*hhrStatGetIndexScalar*/
static oid hhrStatGetIndexScalar_oid[] = {HHRSTATCONFIG, 1, 4};
FindVarMethod hhrStatGetIndexScalar_get;
struct variable1 hhrStatGetIndexScalar_variables[] =
{
    {hhrStatGetTaskId,            ASN_UNSIGNED, RONLY, hhrStatGetIndexScalar_get, 1, {1}},
    {hhrStatGetEventThTpId,       ASN_UNSIGNED, RONLY, hhrStatGetIndexScalar_get, 1, {2}},
    {hhrStatGetSubTypeTpId,       ASN_UNSIGNED, RONLY, hhrStatGetIndexScalar_get, 1, {3}},
    {hhrStatGetAlarmThTpId,       ASN_UNSIGNED, RONLY, hhrStatGetIndexScalar_get, 1, {4}},
    {hhrStatGetSubTypeReportTpId, ASN_UNSIGNED, RONLY, hhrStatGetIndexScalar_get, 1, {5}}
};

/*hhrStatDataCollectTypeScalar*/
static oid hhrStatDataCollectTypeScalar_oid[] = {HHRSTATCONFIG, 1};
FindVarMethod hhrStatDataCollectTypeScalar_get;
struct variable1 hhrStatDataCollectTypeScalar_variables[] =
{
    {hhrStatDataCollectType, ASN_INTEGER, RONLY, hhrStatDataCollectTypeScalar_get, 1, {5}},
};

/*hhrStatGlobalScanTypeAtribTable*/
static oid hhrStatGlobalScanTypeAtribTable_oid[] = {HHRSTATCONFIG, 2};
FindVarMethod hhrStatGlobalScanTypeAtribTable_get;
struct variable2 hhrStatGlobalScanTypeAtribTable_variables[] =
{
    {hhrStatTypeIndex,            ASN_UNSIGNED, RONLY,  hhrStatGlobalScanTypeAtribTable_get, 2, {1, 1}},
    {hhrStatIncludeTypeNum,       ASN_UNSIGNED, RONLY,  hhrStatGlobalScanTypeAtribTable_get, 2, {1, 2}},
    {hhrStatSubTypeNumInScanType, ASN_UNSIGNED, RONLY,  hhrStatGlobalScanTypeAtribTable_get, 2, {1, 3}},
    {hhrStatLongCycSeconds,       ASN_UNSIGNED, RONLY,  hhrStatGlobalScanTypeAtribTable_get, 2, {1, 4}},
    {hhrStatShortCycSeconds,      ASN_UNSIGNED, RONLY,  hhrStatGlobalScanTypeAtribTable_get, 2, {1, 5}},
    {hhrStatTypeRowStatus,        ASN_INTEGER,  RWRITE, hhrStatGlobalScanTypeAtribTable_get, 2, {1, 26}}
};

/*hhrStatGlobalSubTypeAtribTable*/
static oid hhrStatGlobalSubTypeAtribTable_oid[] = {HHRSTATCONFIG, 3};
FindVarMethod hhrStatGlobalSubTypeAtribTable_get;
struct variable2 hhrStatGlobalSubTypeAtribTable_variables[] =
{
    {hhrStatSubTypeIndex,          ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 1}},
    {hhrStatSubTypeBelongScanType, ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 2}},
    {hhrStatSubTypeValueClass,     ASN_INTEGER,  RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 3}},
    {hhrStatSubTypeDataBitDeep,    ASN_INTEGER,  RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 4}},
    {hhrStatLongCycUpTdAlarm,      ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 5}},
    {hhrStatLongCycUpTdEvent,      ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 6}},
    {hhrStatLongCycLowTdAlarm,     ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 7}},
    {hhrStatLongCycLowTdEvent,     ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 8}},
    {hhrStatShortCycUpTdAlarm,     ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 9}},
    {hhrStatShortCycUpTdEvent,     ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 10}},
    {hhrStatShortCycLowTdAlarm,    ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 11}},
    {hhrStatShortCycLowTdEvent,    ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 12}},
    {hhrStatUsrDefCycUpTdAlarm,    ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 13}},
    {hhrStatUsrDefCycUpTdEvent,    ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 14}},
    {hhrStatUsrDefCycLowTdAlarm,   ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 15}},
    {hhrStatUsrDefCycLowTdEvent,   ASN_UNSIGNED, RONLY,  hhrStatGlobalSubTypeAtribTable_get, 2, {1, 16}},
    {hhrStatLongCycUpTdHigh32,     ASN_UNSIGNED, RWRITE, hhrStatGlobalSubTypeAtribTable_get, 2, {1, 17}},
    {hhrStatLongCycUpTdLow32,      ASN_UNSIGNED, RWRITE, hhrStatGlobalSubTypeAtribTable_get, 2, {1, 18}},
    {hhrStatLongCycLowTdHigh32,    ASN_UNSIGNED, RWRITE, hhrStatGlobalSubTypeAtribTable_get, 2, {1, 19}},
    {hhrStatLongCycLowTdLow32,     ASN_UNSIGNED, RWRITE, hhrStatGlobalSubTypeAtribTable_get, 2, {1, 20}},
    {hhrStatShortCycUpTdHigh32,    ASN_UNSIGNED, RWRITE, hhrStatGlobalSubTypeAtribTable_get, 2, {1, 21}},
    {hhrStatShortCycUpTdLow32,     ASN_UNSIGNED, RWRITE, hhrStatGlobalSubTypeAtribTable_get, 2, {1, 22}},
    {hhrStatShortCycLowTdHigh32,   ASN_UNSIGNED, RWRITE, hhrStatGlobalSubTypeAtribTable_get, 2, {1, 23}},
    {hhrStatShortCycLowTdLow32,    ASN_UNSIGNED, RWRITE, hhrStatGlobalSubTypeAtribTable_get, 2, {1, 24}},
    {hhrStatSubTypeRowStatus,      ASN_INTEGER,  RWRITE, hhrStatGlobalSubTypeAtribTable_get, 2, {1, 25}}
};

/*hhrStatPortTypeToScanTypeTable*/
static oid hhrStatPortTypeToScanTypeTable_oid[] = {HHRSTATCONFIG, 4};
FindVarMethod hhrStatPortTypeToScanTypeTable_get;
struct variable2 hhrStatPortTypeToScanTypeTable_variables[] =
{
    {hhrStatPortTypeIndex,                ASN_UNSIGNED, RONLY,  hhrStatPortTypeToScanTypeTable_get, 2, {1, 1}},
    {hhrStatPortTypeAboutStatScanTypeNum, ASN_UNSIGNED, RONLY,  hhrStatPortTypeToScanTypeTable_get, 2, {1, 2}},
    {hhrStatScanType1IndexForPortType,    ASN_UNSIGNED, RONLY,  hhrStatPortTypeToScanTypeTable_get, 2, {1, 3}},
    {hhrStatScanType2IndexForPortType,    ASN_UNSIGNED, RONLY,  hhrStatPortTypeToScanTypeTable_get, 2, {1, 4}},
    {hhrStatScanType3IndexForPortType,    ASN_UNSIGNED, RONLY,  hhrStatPortTypeToScanTypeTable_get, 2, {1, 5}},
    {hhrStatScanType4IndexForPortType,    ASN_UNSIGNED, RONLY,  hhrStatPortTypeToScanTypeTable_get, 2, {1, 6}},
    {hhrStatScanType5IndexForPortType,    ASN_UNSIGNED, RONLY,  hhrStatPortTypeToScanTypeTable_get, 2, {1, 7}},
    {hhrStatPortTypeToScanTypeRowStatus,  ASN_INTEGER,  RWRITE, hhrStatPortTypeToScanTypeTable_get, 2, {1, 8}}
};

/*hhrStatPortMoniCtrlTable*/
static oid hhrStatPortMoniCtrlTable_oid[] = {HHRSTATCONFIG, 6};
FindVarMethod hhrStatPortMoniCtrlTable_get;
struct variable2 hhrStatPortMoniCtrlTable_variables[] =
{
    /*{hhrStatPortMoniIndex1,                 ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 1}},
    {hhrStatPortMoniIndex2,                 ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 2}},
    {hhrStatPortMoniIndex3,                 ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 3}},
    {hhrStatPortMoniIndex4,                 ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 4}},
    {hhrStatPortMoniIndex5,                 ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 5}},
    {hhrStatPortMoniDevIndex,               ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 6}},
    {hhrStatPortMoniScanType,               ASN_INTEGER,  RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 7}},*/
    {hhrStatMoniEn,                         ASN_INTEGER,  RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 8}},
    {hhrStatCurrMoniEn,                     ASN_INTEGER,  RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 9}},
    {hhrStatLongCycStatMoniEn,              ASN_INTEGER,  RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 10}},
    {hhrStatLongCycBelongTask,              ASN_UNSIGNED, RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 11}},
    {hhrStatLongCycStatEventTdLocat,        ASN_UNSIGNED, RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 12}},
    {hhrStatLongCycSubStatTypeFiltTdLocat,  ASN_UNSIGNED, RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 13}},
    {hhrStatLongCycStatAlarmTdLocat,        ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 14}},
    {hhrStatLongCycHistAutoReport,          ASN_INTEGER,  RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 15}},
    {hhrStatLongCycStatHistLocat,           ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 16}},
    {hhrStatShortCycStatMoniEn,             ASN_INTEGER,  RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 17}},
    {hhrStatShortCycBelongTask,             ASN_UNSIGNED, RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 18}},
    {hhrStatShortCycStatEventTdLocat,       ASN_UNSIGNED, RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 19}},
    {hhrStatShortCycSubStatTypeFiltTdLocat, ASN_UNSIGNED, RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 20}},
    {hhrStatShortCycStatAlarmTdLocat,       ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 21}},
    {hhrStatShortCycHistAutoReport,         ASN_INTEGER,  RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 22}},
    {hhrStatShortCycStatHistLocat,          ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 23}},
    {hhrStatUsrDefCycStatMoniEn,            ASN_INTEGER,  RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 24}},
    {hhrStatUsrDefCycBelongTask,            ASN_UNSIGNED, RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 25}},
    {hhrStatUsrDefCycStatEventTdLocat,      ASN_UNSIGNED, RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 26}},
    {hhrStatUsrDefCycSubStatTypeFiltTdLocat, ASN_UNSIGNED, RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 27}},
    {hhrStatUsrDefCycStatAlarmTdLocat,      ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 28}},
    {hhrStatUsrDefCycHistAutoReport,        ASN_INTEGER,  RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 29}},
    {hhrStatUsrDefCycStatHistLocat,         ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 30}},
    {hhrStatUsrDefCycHistSeconds,           ASN_UNSIGNED, RONLY,  hhrStatPortMoniCtrlTable_get, 2, {1, 31}},
    {hhrStatPortMoniRowStatus,              ASN_INTEGER,  RWRITE, hhrStatPortMoniCtrlTable_get, 2, {1, 32}}
};

/*hhrStatCurrStatTable*/
static oid hhrStatCurrStatTable_oid[] = {HHRSTATCONFIG, 8};
FindVarMethod hhrStatCurrStatTable_get;
struct variable2 hhrStatCurrStatTable_variables[] =
{
    /*{hhrStatCurrStatPortIndex1,      ASN_UNSIGNED, RONLY,  hhrStatCurrStatTable_get, 2, {1, 1}},
    {hhrStatCurrStatPortIndex2,      ASN_UNSIGNED, RONLY,  hhrStatCurrStatTable_get, 2, {1, 2}},
    {hhrStatCurrStatPortIndex3,      ASN_UNSIGNED, RONLY,  hhrStatCurrStatTable_get, 2, {1, 3}},
    {hhrStatCurrStatPortIndex4,      ASN_UNSIGNED, RONLY,  hhrStatCurrStatTable_get, 2, {1, 4}},
    {hhrStatCurrStatPortIndex5,      ASN_UNSIGNED, RONLY,  hhrStatCurrStatTable_get, 2, {1, 5}},
    {hhrStatCurrStatDevIndex,        ASN_UNSIGNED, RONLY,  hhrStatCurrStatTable_get, 2, {1, 6}},
    {hhrStatCurrStatDataType,        ASN_UNSIGNED, RONLY,  hhrStatCurrStatTable_get, 2, {1, 7}},*/
    {hhrStatClearCurrDataClear,      ASN_INTEGER,  RWRITE, hhrStatCurrStatTable_get, 2, {1, 13}},
    {hhrStatCurrDataTypeValueHigh32, ASN_UNSIGNED, RONLY,  hhrStatCurrStatTable_get, 2, {1, 15}},
    {hhrStatCurrDataTypeValueLow32,  ASN_UNSIGNED, RONLY,  hhrStatCurrStatTable_get, 2, {1, 16}},
    {hhrStatCurrRowStatus,           ASN_INTEGER,  RWRITE, hhrStatCurrStatTable_get, 2, {1, 74}}
};

/*hhrStatTaskTable*/
static oid hhrStatTaskTable_oid[] = {HHRSTATCONFIG, 10};
FindVarMethod hhrStatTaskTable_get;
struct variable2 hhrStatTaskTable_variables[] =
{
    //{hhrStatTaskIndex,            ASN_UNSIGNED, RWRITE, hhrStatTaskTable_get, 2, {1, 1}},
    {hhrStatMaxPortInTask,        ASN_UNSIGNED, RWRITE, hhrStatTaskTable_get, 2, {1, 2}},
    {hhrStatTaskScanType,         ASN_UNSIGNED, RWRITE, hhrStatTaskTable_get, 2, {1, 3}},
    {hhrStatIncludeMonObjNum,     ASN_UNSIGNED, RONLY,  hhrStatTaskTable_get, 2, {1, 4}},
    {hhrStatTaskCycMoniType,      ASN_INTEGER,  RWRITE, hhrStatTaskTable_get, 2, {1, 5}},
    {hhrStatTaskMoniCycSeconds,   ASN_UNSIGNED, RWRITE, hhrStatTaskTable_get, 2, {1, 6}},
    {hhrStatTaskStartTime,        ASN_UNSIGNED, RWRITE, hhrStatTaskTable_get, 2, {1, 7}},
    {hhrStatTaskEndTime,          ASN_UNSIGNED, RWRITE, hhrStatTaskTable_get, 2, {1, 8}},
    {hhrStatTaskAlarmTdTpLocat,   ASN_UNSIGNED, RWRITE, hhrStatTaskTable_get, 2, {1, 9}},
    {hhrStatTaskSubReportTpLocat, ASN_UNSIGNED, RWRITE, hhrStatTaskTable_get, 2, {1, 10}},
    {hhrStatTaskRowStatus,        ASN_INTEGER,  RWRITE, hhrStatTaskTable_get, 2, {1, 20}}
};

/*hhrStatEventTdTpTable*/
static oid hhrStatEventTdTpTable_oid[] = {HHRSTATCONFIG, 12};
FindVarMethod hhrStatEventTdTpTable_get;
struct variable2 hhrStatEventTdTpTable_variables[] =
{
    //{hhrStatEventTdTpIndex,            ASN_UNSIGNED, RONLY,  hhrStatEventTdTpTable_get, 2, {1, 1}},
    //{hhrStatEventTdTpSubType,          ASN_UNSIGNED, RONLY,  hhrStatEventTdTpTable_get, 2, {1, 2}},
    {hhrStatEventTdTpScanType,         ASN_UNSIGNED, RWRITE, hhrStatEventTdTpTable_get, 2, {1, 3}},
    {hhrStatEventTdTpSubTypeUpHigh32,  ASN_UNSIGNED, RWRITE, hhrStatEventTdTpTable_get, 2, {1, 4}},
    {hhrStatEventTdTpSubTypeUpLow32,   ASN_UNSIGNED, RWRITE, hhrStatEventTdTpTable_get, 2, {1, 5}},
    {hhrStatEventTdTpSubTypeLowHigh32, ASN_UNSIGNED, RWRITE, hhrStatEventTdTpTable_get, 2, {1, 6}},
    {hhrStatEventTdTpSubTypeLowLow32,  ASN_UNSIGNED, RWRITE, hhrStatEventTdTpTable_get, 2, {1, 7}},
    {hhrStatEventTdTpRowStatus,        ASN_INTEGER,  RWRITE, hhrStatEventTdTpTable_get, 2, {1, 114}}
};

/*hhrStatSubStatTypeFiltTpTable*/
static oid hhrStatSubStatTypeFiltTpTable_oid[] = {HHRSTATCONFIG, 14};
FindVarMethod hhrStatSubStatTypeFiltTpTable_get;
struct variable2 hhrStatSubStatTypeFiltTpTable_variables[] =
{
    //{hhrStatSubStatTypeFiltIndex, ASN_UNSIGNED, RONLY,  hhrStatSubStatTypeFiltTpTable_get, 2, {1, 1}},
    //{hhrStatSelectSubType,        ASN_UNSIGNED, RONLY,  hhrStatSubStatTypeFiltTpTable_get, 2, {1, 2}},
    {hhrStatSelectStatScanType,   ASN_UNSIGNED, RWRITE, hhrStatSubStatTypeFiltTpTable_get, 2, {1, 3}},
    {hhrStatSelectStatus,         ASN_INTEGER,  RWRITE, hhrStatSubStatTypeFiltTpTable_get, 2, {1, 4}},
    {hhrStatSelectSubRowStatus,   ASN_INTEGER,  RWRITE, hhrStatSubStatTypeFiltTpTable_get, 2, {1, 33}}
};

/*hhrStatAlarmTdTpTable*/
static oid hhrStatAlarmTdTpTable_oid[] = {HHRSTATCONFIG, 16};
FindVarMethod hhrStatAlarmTdTpTable_get;
struct variable2 hhrStatAlarmTdTpTable_variables[] =
{
    //{hhrStatAlarmTdTpIndex,              ASN_UNSIGNED, RONLY,  hhrStatAlarmTdTpTable_get, 2, {1, 1}},
    //{hhrStatAlarmTdSubType,              ASN_UNSIGNED, RONLY,  hhrStatAlarmTdTpTable_get, 2, {1, 2}},
    {hhrStatAlarmTdScanType,             ASN_UNSIGNED, RWRITE, hhrStatAlarmTdTpTable_get, 2, {1, 3}},
    {hhrStatAlarmTdSubTypeUpRiseHigh32,  ASN_UNSIGNED, RWRITE, hhrStatAlarmTdTpTable_get, 2, {1, 4}},
    {hhrStatAlarmTdSubTypeUpRiseLow32,   ASN_UNSIGNED, RWRITE, hhrStatAlarmTdTpTable_get, 2, {1, 5}},
    {hhrStatAlarmTdSubTypeUpClearHigh32, ASN_UNSIGNED, RWRITE, hhrStatAlarmTdTpTable_get, 2, {1, 6}},
    {hhrStatAlarmTdSubTypeUpClearLow32,  ASN_UNSIGNED, RWRITE, hhrStatAlarmTdTpTable_get, 2, {1, 7}},
    {hhrStatAlarmTdSubTypeDnRiseHigh32,  ASN_UNSIGNED, RWRITE, hhrStatAlarmTdTpTable_get, 2, {1, 8}},
    {hhrStatAlarmTdSubTypeDnRiseLow32,   ASN_UNSIGNED, RWRITE, hhrStatAlarmTdTpTable_get, 2, {1, 9}},
    {hhrStatAlarmTdSubTypeDnClearHigh32, ASN_UNSIGNED, RWRITE, hhrStatAlarmTdTpTable_get, 2, {1, 10}},
    {hhrStatAlarmTdSubTypeDnClearLow32,  ASN_UNSIGNED, RWRITE, hhrStatAlarmTdTpTable_get, 2, {1, 11}},
    {hhrStatAlarmTdTpRowStatus,          ASN_INTEGER,  RWRITE, hhrStatAlarmTdTpTable_get, 2, {1, 200}}
};

/*hhrStatSubStatTypeReportTpTable*/
static oid hhrStatSubStatTypeReportTpTable_oid[] = {HHRSTATCONFIG, 18};
FindVarMethod hhrStatSubStatTypeReportTpTable_get;
struct variable2 hhrStatSubStatTypeReportTpTable_variables[] =
{
    //{hhrStatSubStatTypeReportTpIndex, ASN_INTEGER,  RONLY,  hhrStatSubStatTypeReportTpTable_get, 2, {1, 1}},
    //{hhrStatSubReportSubType,         ASN_INTEGER,  RONLY,  hhrStatSubStatTypeReportTpTable_get, 2, {1, 2}},
    {hhrStatSubReportScanType,        ASN_UNSIGNED, RWRITE, hhrStatSubStatTypeReportTpTable_get, 2, {1, 3}},
    {hhrStatSubReportStatus,          ASN_UNSIGNED, RWRITE, hhrStatSubStatTypeReportTpTable_get, 2, {1, 4}},
    {hhrStatSubReportRowStatus,       ASN_INTEGER,  RWRITE, hhrStatSubStatTypeReportTpTable_get, 2, {1, 5}}
};

/*hhrStatHistDataTable*/
static oid hhrStatHistDataTable_oid[] = {HHRSTATCONFIG, 20};
FindVarMethod hhrStatHistDataTable_get;
struct variable2 hhrStatHistDataTable_variables[] =
{
    //{hhrStatHistDataIndex,     ASN_UNSIGNED, RONLY,  hhrStatHistDataTable_get, 2, {1, 1}},
    //{hhrStatHistDataStopTime,  ASN_UNSIGNED, RONLY,  hhrStatHistDataTable_get, 2, {1, 2}},
    //{hhrStatHistStatType,      ASN_UNSIGNED, RONLY,  hhrStatHistDataTable_get, 2, {1, 3}},
    {hhrStatHistDataScanType,  ASN_UNSIGNED, RONLY,  hhrStatHistDataTable_get, 2, {1, 4}},
    {hhrStatHistDataHigh32,    ASN_UNSIGNED, RONLY,  hhrStatHistDataTable_get, 2, {1, 5}},
    {hhrStatHistDataLow32,     ASN_UNSIGNED, RONLY,  hhrStatHistDataTable_get, 2, {1, 6}},
    {hhrStatHistDataRowStatus, ASN_INTEGER,  RWRITE, hhrStatHistDataTable_get, 2, {1, 69}}
};

/*hhrStatHistDataReportTable*/
static oid hhrStatHistDataReportTable_oid[] = {HHRSTATCONFIG, 22};
FindVarMethod hhrStatHistDataReportTable_get;
struct variable2 hhrStatHistDataReportTable_variables[] =
{
    //{hhrStatHistDataReportBaseIndex,  ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 1}},
    //{hhrStatHistDataReportSubType,    ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 2}},
    {hhrStatHistDataReportStopTime,   ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 3}},
    {hhrStatHistDataReportScanype,    ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 4}},
    {hhrStatHistDataReportCycType,    ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 5}},
    {hhrStatHistDataReportPortIndex1, ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 6}},
    {hhrStatHistDataReportPortIndex2, ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 7}},
    {hhrStatHistDataReportPortIndex3, ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 8}},
    {hhrStatHistDataReportPortIndex4, ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 9}},
    {hhrStatHistDataReportPortIndex5, ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 10}},
    {hhrStatHistDataReportDevIndex,   ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 11}},
    {hhrStatHistDataReportTypeHigh32, ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 12}},
    {hhrStatHistDataReportTypeLow32,  ASN_UNSIGNED, RONLY,  hhrStatHistDataReportTable_get, 2, {1, 13}},
    {hhrStatHistDataReportRowStatus,  ASN_INTEGER,  RWRITE, hhrStatHistDataReportTable_get, 2, {1, 79}}
};


/* add scalars to speed up ack to mib when tables are empty */
static oid hhrStatTableSupport_oid[] = {HHRSTATCONFIG};
FindVarMethod hhrStatTableSupport_get;
struct variable1 hhrStatTableSupport_variables[] =
{
    {hhrStatPortMoniCtrlTableSupport,               ASN_INTEGER, RONLY, hhrStatTableSupport_get, 1, {7}},
    {hhrCurrStatTableSupport,                       ASN_INTEGER, RONLY, hhrStatTableSupport_get, 1, {9}},
    {hhrTaskTableSupport,                           ASN_INTEGER, RONLY, hhrStatTableSupport_get, 1, {11}},
    {hhrStatEventThresholdTemplateTableSupport,     ASN_INTEGER, RONLY, hhrStatTableSupport_get, 1, {13}},
    {hhrStatSubStatTypeFiltTpTableSupport,          ASN_INTEGER, RONLY, hhrStatTableSupport_get, 1, {15}},
    {hhrStatAlarmThresholdTemplateTableSupport,     ASN_INTEGER, RONLY, hhrStatTableSupport_get, 1, {17}},
    {hhrStatSubStatTypeReportTemplatTableSupport,   ASN_INTEGER, RONLY, hhrStatTableSupport_get, 1, {19}},
    {hhrHistStatDataTableSupport,                   ASN_INTEGER, RONLY, hhrStatTableSupport_get, 1, {21}},
};




/*init stat mib*/
void init_mib_stat(void)
{
    REGISTER_MIB("hhrStatGlobalTimeScalar", hhrStatGlobalTimeScalar_variables, variable1,
                 hhrStatGlobalTimeScalar_oid);
    REGISTER_MIB("hhrStatMoniSourceScalar", hhrStatMoniSourceScalar_variables, variable1,
                 hhrStatMoniSourceScalar_oid);
    REGISTER_MIB("hhrStatAutoReportScalar", hhrStatAutoReportScalar_variables, variable1,
                 hhrStatAutoReportScalar_oid);
    REGISTER_MIB("hhrStatGetIndexScalar", hhrStatGetIndexScalar_variables, variable1,
                 hhrStatGetIndexScalar_oid);
    REGISTER_MIB("hhrStatDataCollectTypeScalar", hhrStatDataCollectTypeScalar_variables, variable1,
                 hhrStatDataCollectTypeScalar_oid);
    REGISTER_MIB("hhrStatGlobalScanTypeAtribTable", hhrStatGlobalScanTypeAtribTable_variables, variable2,
                 hhrStatGlobalScanTypeAtribTable_oid);
    REGISTER_MIB("hhrStatGlobalSubTypeAtribTable", hhrStatGlobalSubTypeAtribTable_variables, variable2,
                 hhrStatGlobalSubTypeAtribTable_oid);
    REGISTER_MIB("hhrStatPortTypeToScanTypeTable", hhrStatPortTypeToScanTypeTable_variables, variable2,
                 hhrStatPortTypeToScanTypeTable_oid);
    REGISTER_MIB("hhrStatPortMoniCtrlTable", hhrStatPortMoniCtrlTable_variables, variable2,
                 hhrStatPortMoniCtrlTable_oid);
    REGISTER_MIB("hhrStatCurrStatTable", hhrStatCurrStatTable_variables, variable2,
                 hhrStatCurrStatTable_oid);
    REGISTER_MIB("hhrStatTaskTable", hhrStatTaskTable_variables, variable2,
                 hhrStatTaskTable_oid);
    REGISTER_MIB("hhrStatEventTdTpTable", hhrStatEventTdTpTable_variables, variable2,
                 hhrStatEventTdTpTable_oid);
    REGISTER_MIB("hhrStatSubStatTypeFiltTpTable", hhrStatSubStatTypeFiltTpTable_variables, variable2,
                 hhrStatSubStatTypeFiltTpTable_oid);
    REGISTER_MIB("hhrStatAlarmTdTpTable", hhrStatAlarmTdTpTable_variables, variable2,
                 hhrStatAlarmTdTpTable_oid);
    REGISTER_MIB("hhrStatSubStatTypeReportTpTable", hhrStatSubStatTypeReportTpTable_variables, variable2,
                 hhrStatSubStatTypeReportTpTable_oid);
    REGISTER_MIB("hhrStatHistDataTable", hhrStatHistDataTable_variables, variable2,
                 hhrStatHistDataTable_oid);
    REGISTER_MIB("hhrStatHistDataReportTable", hhrStatHistDataReportTable_variables, variable2,
                 hhrStatHistDataReportTable_oid);

    /* add scalars to speed up ack to mib when tables are empty */
    REGISTER_MIB("hhrStatTableSupportScalar", hhrStatTableSupport_variables, variable1,
                 hhrStatTableSupport_oid);
}




/************************ ipc msg request ************************/

static uint32_t stat_mib_get_scalar(uint8_t *pData, int module_id, uint32_t msg_type, uint16_t subtype, uint8_t opcode)
{
	uint32_t recv_len = 0;

    struct ipc_mesg_n *pMsgRcv = NULL;
	pMsgRcv = ipc_sync_send_n2(NULL, 0, 0, module_id, MODULE_ID_SNMPD, msg_type, subtype, 
							opcode, 0, 1000);

	if(pMsgRcv)
	{
		recv_len = pMsgRcv->msghdr.data_len;
		memcpy(pData, pMsgRcv->msg_data, pMsgRcv->msghdr.data_len);

		mem_share_free(pMsgRcv, module_id);
	}

	return recv_len;
}


static void stat_mib_set(uint8_t *pData, uint32_t data_len, int module_id, uint32_t msg_type, uint16_t subtype, uint8_t opcode)
{
	int    iRetv = 0;

    struct ipc_mesg_n *pMsgSnd = mem_share_malloc((sizeof(struct ipc_msghdr_n) + data_len), module_id);
    if(pMsgSnd == NULL) 
	{
		return;
    }

    memset(pMsgSnd, 0, (sizeof(struct ipc_msghdr_n) + data_len));

	pMsgSnd->msghdr.data_len  = data_len;
    pMsgSnd->msghdr.module_id = module_id;
    pMsgSnd->msghdr.sender_id = MODULE_ID_SNMPD;
    pMsgSnd->msghdr.msg_type  = msg_type;
    pMsgSnd->msghdr.msg_subtype = subtype;
    pMsgSnd->msghdr.opcode      = opcode;
    pMsgSnd->msghdr.msg_index   = 0;
    pMsgSnd->msghdr.data_num    = 1;

	memcpy(pMsgSnd->msg_data, (uint8_t *)pData, data_len);

    iRetv = ipc_send_msg_n1(pMsgSnd, (sizeof(struct ipc_msghdr_n) + data_len));

    if(-1 == iRetv)	
    {
		mem_share_free(pMsgSnd, module_id);
    }
}



static int mib_stat_getInt(uint8_t *pData, int subtype)
{
    /*return ipc_send_common_wait_reply(NULL, 0, 1, module_id, MODULE_ID_SNMPD,
                                      IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET, 0);*/

	uint32_t recv_len = stat_mib_get_scalar(pData, MODULE_ID_STAT, IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET);

	if(sizeof(int) == recv_len)
	{
		return 1;
	}
	else
	{
		memset(pData, 0, sizeof(int));
		return 0;
	}
}

static uint32_t mib_stat_getStr(uchar *pData, int subtype)
{
    /*struct ipc_mesg *pmsg = ipc_send_common_wait_reply1(NULL, 0, 1, module_id, MODULE_ID_SNMPD,
                                                        IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET, 0);

    if (pmsg)
    {
        memset(str_value, 0, STRING_LEN);
        memcpy(str_value, pmsg->msg_data, pmsg->msghdr.data_len);
        return TRUE;
    }
    else
    {
        return FALSE;
    }*/

	return stat_mib_get_scalar((uint8_t *)pData, MODULE_ID_STAT, IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET);
}


/* 配置标量数据 */
static int *mib_stat_setInt(uint32_t val, int subtype)
{
    stat_mib_set((uint8_t *)&val, sizeof(uint32_t), MODULE_ID_STAT, IPC_TYPE_SNMP, subtype, IPC_OPCODE_UPDATE);
	return TRUE;
}


/* 批量返回表信息，返回值是表的指针，*pdata_num 返回实际的表数量 */
static struct ipc_mesg_n *mib_stat_get_bulk(void *pIndex, int subtype)
{
    int index_len = 0;

    switch (subtype)
    {
        case SNMP_STAT_GLOBAL_SCAN_TYPE_ATTRI_TABLE:
            index_len = sizeof(stStatGloScanTypeAttribTable_index);
            break;

        case SNMP_STAT_GLOBAL_SUB_TYPE_ATTRI_TABLE:
            index_len = sizeof(stGloSubTypeAttribTab_index);
            break;

        case SNMP_STAT_PORT_TYPE_TO_STAT_SCAN_TYPE_TABLE:
            index_len = sizeof(stPortTypeToScanTypeTab_index);
            break;

        case SNMP_STAT_PORT_MONI_CTRL_TABLE:
            index_len = sizeof(stPortMoniCtrlTab_index);
            break;

        case SNMP_CURR_STAT_TABLE:
            index_len = sizeof(stCurrStatTab_index);
            break;

        case SNMP_STAT_TASK_TABLE:
            index_len = sizeof(stStatTaskTab_index);
            break;

        case SNMP_STAT_EVENT_THRESHOLD_TEMPLAT_TABLE:
            index_len = sizeof(stEvtThrTmpTab_index);
            break;

        case SNMP_STAT_SUB_STAT_TYPE_FILT_TP_TABLE:
            index_len = sizeof(stSubStatTypeFileTpTab_index);
            break;

        case SNMP_STAT_ALARM_THRESHOLD_TEMPLAT_TABLE:
            index_len = sizeof(stAlmThrTmpTab_index);
            break;

        case SNMP_STAT_SUB_STAT_TYPE_REPORT_TEMPLAT_TABLE:
            index_len = sizeof(stSubTypeReportTmpTab_index);
            break;

        case SNMP_HIST_STAT_DATA_TABLE:
            index_len = sizeof(stHistDataTab_index);
            break;

        case SNMP_HIST_DATA_REPORT_IN_STAND_FORMAT_TABLE:
            index_len = sizeof(stHistDataReportFormatTab_index);
            break;

        default:
            break;
    }

#if 0
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(index, index_len, 1 , MODULE_ID_STAT, MODULE_ID_SNMPD,
                                                         IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET_BULK, 0);

    if (pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return pmesg->msg_data;
    }

    return NULL;
#endif

	return ipc_sync_send_n2(pIndex, index_len, 1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP, subtype, 
							IPC_OPCODE_GET_BULK, 0, 1000);
}




/* 获取 statGloScanTypeAttribTable 数据 */
int statGloScanTypeAttribTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                                 stStatGloScanTypeAttribTable_info *pinfo_input)
{
    stStatGloScanTypeAttribTable_info *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if (0 == pinfo_input->index.typeIndex)
    {
        pinfo_input->index.typeIndex = 0xffffffff;
    }

    struct ipc_mesg_n *pMsgRcv = mib_stat_get_bulk((void *)&pinfo_input->index, SNMP_STAT_GLOBAL_SCAN_TYPE_ATTRI_TABLE);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
        return FALSE;
    }

	pinfo = (stStatGloScanTypeAttribTable_info *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stStatGloScanTypeAttribTable_info));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stStatGloScanTypeAttribTable_info *statGloScanTypeAttribTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                          int exact, const stStatGloScanTypeAttribTable_index *index)
{
    struct listnode  *node, *nnode;

    stStatGloScanTypeAttribTable_info *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || (0 == index->typeIndex))
        {
            return (stStatGloScanTypeAttribTable_info *)cache->data_list->head->data ;
        }

        if (pinfo_find->index.typeIndex == index->typeIndex)
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (stStatGloScanTypeAttribTable_info *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 GloSubTypeAttriTable 数据 */
int statGloSubTypeAttribTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                                stGloSubTypeAttribTab_info *pinfo_input)
{
    stGloSubTypeAttribTab_info *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if (0 == pinfo_input->index.subTypeIndex)
    {
        pinfo_input->index.subTypeIndex = 0xffffffff;
    }

    struct ipc_mesg_n *pMsgRcv = mib_stat_get_bulk((void *)&pinfo_input->index, SNMP_STAT_GLOBAL_SUB_TYPE_ATTRI_TABLE);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
        return FALSE;
    }

	pinfo = (stGloSubTypeAttribTab_info *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stGloSubTypeAttribTab_info));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stGloSubTypeAttribTab_info *statGloSubTypeAttribTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                  int exact, const stGloSubTypeAttribTab_index *index)
{
    struct listnode  *node, *nnode;

    stGloSubTypeAttribTab_info *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || (0 == index->subTypeIndex))
        {
            return (stGloSubTypeAttribTab_info *)cache->data_list->head->data ;
        }

        if (pinfo_find->index.subTypeIndex == index->subTypeIndex)
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (stGloSubTypeAttribTab_info *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 StatPortTypeToStatScanTypeTable 数据 */
int statPortTypeToStatScanTypeTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                                      stPortTypeToScanTypeTab_info *pinfo_input)
{
    stPortTypeToScanTypeTab_info *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if (0 == pinfo_input->index.portTypeIndex)
    {
        pinfo_input->index.portTypeIndex = 0xffffffff;
    }
    
    struct ipc_mesg_n *pMsgRcv = mib_stat_get_bulk((void *)&pinfo_input->index, SNMP_STAT_PORT_TYPE_TO_STAT_SCAN_TYPE_TABLE);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
        return FALSE;
    }

	pinfo = (stPortTypeToScanTypeTab_info *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stPortTypeToScanTypeTab_info));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stPortTypeToScanTypeTab_info *statPortTypeToStatScanTypeTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                          int exact, const stPortTypeToScanTypeTab_index *index)
{
    struct listnode  *node, *nnode;

    stPortTypeToScanTypeTab_info *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if (0 == index->portTypeIndex)
        {
            return (stPortTypeToScanTypeTab_info *)cache->data_list->head->data ;
        }

        if (pinfo_find->index.portTypeIndex == index->portTypeIndex)
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (stPortTypeToScanTypeTab_info *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 StatPortMoniCtrlTable 数据 */
int statPortMoniCtrlTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                            stPortMoniCtrlTab_info *pinfo_input)
{
    stPortMoniCtrlTab_info *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if ((0 == pinfo_input->index.portMoniIndex1) &&
        (0 == pinfo_input->index.portMoniIndex2) &&
        (0 == pinfo_input->index.portMoniIndex3) &&
        (0 == pinfo_input->index.portMoniIndex4) &&
        (0 == pinfo_input->index.portMoniIndex5) &&
        (0 == pinfo_input->index.portMoniDevIndex) &&
        (0 == pinfo_input->index.portMoniScanType))
    {
        pinfo_input->index.portMoniIndex1 = 0xffffffff;
        pinfo_input->index.portMoniIndex2 = 0xffffffff;
        pinfo_input->index.portMoniIndex3 = 0xffffffff;
        pinfo_input->index.portMoniIndex4 = 0xffffffff;
        pinfo_input->index.portMoniIndex5 = 0xffffffff;
        pinfo_input->index.portMoniDevIndex = 0xffffffff;
        pinfo_input->index.portMoniScanType = 0xffffffff;
    }

    struct ipc_mesg_n *pMsgRcv = mib_stat_get_bulk((void *)&pinfo_input->index, SNMP_STAT_PORT_MONI_CTRL_TABLE);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
        return FALSE;
    }

	pinfo = (stPortMoniCtrlTab_info *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stPortMoniCtrlTab_info));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stPortMoniCtrlTab_info *statPortMoniCtrlTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                          int exact, const stPortMoniCtrlTab_index *index)
{
    struct listnode  *node, *nnode;

    stPortMoniCtrlTab_info *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || ((0 == index->portMoniIndex1) &&
                                (0 == index->portMoniIndex2) &&
                                (0 == index->portMoniIndex3) &&
                                (0 == index->portMoniIndex4) &&
                                (0 == index->portMoniIndex5) &&
                                (0 == index->portMoniDevIndex) &&
                                (0 == index->portMoniScanType)))
        {
            return (stPortMoniCtrlTab_info *)cache->data_list->head->data ;
        }

        if ((pinfo_find->index.portMoniIndex1 == index->portMoniIndex1) &&
                (pinfo_find->index.portMoniIndex2 == index->portMoniIndex2) &&
                (pinfo_find->index.portMoniIndex3 == index->portMoniIndex3) &&
                (pinfo_find->index.portMoniIndex4 == index->portMoniIndex4) &&
                (pinfo_find->index.portMoniIndex5 == index->portMoniIndex5) &&
                (pinfo_find->index.portMoniDevIndex == index->portMoniDevIndex) &&
                (pinfo_find->index.portMoniScanType == index->portMoniScanType))
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (stPortMoniCtrlTab_info *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}

/* 获取 currStatTable 数据 */
int currStatTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                    stCurrStatTab_info *pinfo_input)
{
    stCurrStatTab_info *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if ((0 == pinfo_input->index.currPortIndex1) &&
        (0 == pinfo_input->index.currPortIndex2) &&
        (0 == pinfo_input->index.currPortIndex3) &&
        (0 == pinfo_input->index.currPortIndex4) &&
        (0 == pinfo_input->index.currPortIndex5) &&
        (0 == pinfo_input->index.currDevIndex) &&
        (0 == pinfo_input->index.currDataType))
    {
        pinfo_input->index.currPortIndex1 = 0xffffffff;
        pinfo_input->index.currPortIndex2 = 0xffffffff;
        pinfo_input->index.currPortIndex3 = 0xffffffff;
        pinfo_input->index.currPortIndex4 = 0xffffffff;
        pinfo_input->index.currPortIndex5 = 0xffffffff;
        pinfo_input->index.currDevIndex = 0xffffffff;
        pinfo_input->index.currDataType = 0xffffffff;
    }

    struct ipc_mesg_n *pMsgRcv = mib_stat_get_bulk((void *)&pinfo_input->index, SNMP_CURR_STAT_TABLE);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
        return FALSE;
    }

	pinfo = (stCurrStatTab_info *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stCurrStatTab_info));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stCurrStatTab_info *currStatTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                              int exact, const stCurrStatTab_index *index)
{
    struct listnode  *node, *nnode;

    stCurrStatTab_info *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || ((0 == index->currPortIndex1) &&
                                (0 == index->currPortIndex2) &&
                                (0 == index->currPortIndex3) &&
                                (0 == index->currPortIndex4) &&
                                (0 == index->currPortIndex5) &&
                                (0 == index->currDevIndex) &&
                                (0 == index->currDataType)))
        {
            return (stCurrStatTab_info *)cache->data_list->head->data ;
        }

        if ((pinfo_find->index.currPortIndex1 == index->currPortIndex1) &&
                (pinfo_find->index.currPortIndex2 == index->currPortIndex2) &&
                (pinfo_find->index.currPortIndex3 == index->currPortIndex3) &&
                (pinfo_find->index.currPortIndex4 == index->currPortIndex4) &&
                (pinfo_find->index.currPortIndex5 == index->currPortIndex5) &&
                (pinfo_find->index.currDevIndex == index->currDevIndex) &&
                (pinfo_find->index.currDataType == index->currDataType))
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (stCurrStatTab_info *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 statTaskTable 数据 */
int statTaskTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                    stStatTaskTab_info *pinfo_input)
{
    stStatTaskTab_info *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if (0 == pinfo_input->index.taskIndex)
    {
        pinfo_input->index.taskIndex = 0xffffffff;
    }

    struct ipc_mesg_n *pMsgRcv = mib_stat_get_bulk((void *)&pinfo_input->index, SNMP_STAT_TASK_TABLE);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
        return FALSE;
    }

	pinfo = (stStatTaskTab_info *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stStatTaskTab_info));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stStatTaskTab_info *statTaskTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                              int exact, const stStatTaskTab_index *index)
{
    struct listnode  *node, *nnode;

    stStatTaskTab_info *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || (0 == index->taskIndex))
        {
            return (stStatTaskTab_info *)cache->data_list->head->data ;
        }

        if (pinfo_find->index.taskIndex == index->taskIndex)
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (stStatTaskTab_info *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 statEvtThrTemplatTable 数据 */
int statEvtThrTemplatTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                             stEvtThrTmpTab_info *pinfo_input)
{
    stEvtThrTmpTab_info *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if ((0 == pinfo_input->index.evtThrTmpIndex) && (0 == pinfo_input->index.evtThrTmpTpSubType))
    {
        pinfo_input->index.evtThrTmpIndex = 0xffffffff;
        pinfo_input->index.evtThrTmpTpSubType = 0xffffffff;
    }

    struct ipc_mesg_n *pMsgRcv = mib_stat_get_bulk((void *)&pinfo_input->index, SNMP_STAT_EVENT_THRESHOLD_TEMPLAT_TABLE);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
        return FALSE;
    }

	pinfo = (stEvtThrTmpTab_info *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stEvtThrTmpTab_info));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stEvtThrTmpTab_info *statEvtThrTemplatTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                        int exact, const stEvtThrTmpTab_index *index)
{
    struct listnode  *node, *nnode;

    stEvtThrTmpTab_info *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || ((0 == index->evtThrTmpIndex) &&
                                (0 == index->evtThrTmpTpSubType)))
        {
            return (stEvtThrTmpTab_info *)cache->data_list->head->data ;
        }

        if ((pinfo_find->index.evtThrTmpIndex == index->evtThrTmpIndex) &&
                (pinfo_find->index.evtThrTmpTpSubType == index->evtThrTmpTpSubType))
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (stEvtThrTmpTab_info *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 statSubStatTypeFileTpTable 数据 */
int statSubStatTypeFileTpTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                                 stSubStatTypeFileTpTab_info *pinfo_input)
{
    stSubStatTypeFileTpTab_info *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if ((0 == pinfo_input->index.subTypeFiltIndex) &&
        (0 == pinfo_input->index.selectSubType))
    {
        pinfo_input->index.subTypeFiltIndex = 0xffffffff;
        pinfo_input->index.selectSubType = 0xffffffff;
    }

    struct ipc_mesg_n *pMsgRcv = mib_stat_get_bulk((void *)&pinfo_input->index, SNMP_STAT_SUB_STAT_TYPE_FILT_TP_TABLE);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
        return FALSE;
    }

	pinfo = (stSubStatTypeFileTpTab_info *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stSubStatTypeFileTpTab_info));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stSubStatTypeFileTpTab_info *statSubStatTypeFileTpTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                    int exact, const stSubStatTypeFileTpTab_index *index)
{
    struct listnode  *node, *nnode;

    stSubStatTypeFileTpTab_info *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || ((0 == index->subTypeFiltIndex) &&
                                (0 == index->selectSubType)))
        {
            return (stSubStatTypeFileTpTab_info *)cache->data_list->head->data ;
        }

        if ((pinfo_find->index.subTypeFiltIndex == index->subTypeFiltIndex) &&
                (pinfo_find->index.selectSubType == index->selectSubType))
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (stSubStatTypeFileTpTab_info *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 stAlmThrTmpTab 数据 */
int stAlmThrTmpTab_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                     stAlmThrTmpTab_info *pinfo_input)
{
    stAlmThrTmpTab_info *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if ((0 == pinfo_input->index.almThrTmpIndex) &&
        (0 == pinfo_input->index.almThrTmpTpSubType))
    {
        pinfo_input->index.almThrTmpIndex = 0xffffffff;
        pinfo_input->index.almThrTmpTpSubType = 0xffffffff;
    }

    struct ipc_mesg_n *pMsgRcv = mib_stat_get_bulk((void *)&pinfo_input->index, SNMP_STAT_ALARM_THRESHOLD_TEMPLAT_TABLE);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
        return FALSE;
    }

	pinfo = (stAlmThrTmpTab_info *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stAlmThrTmpTab_info));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stAlmThrTmpTab_info *stAlmThrTmpTab_node_lookup(struct ipran_snmp_data_cache *cache,
                                                int exact, const stAlmThrTmpTab_index *index)
{
    struct listnode  *node, *nnode;

    stAlmThrTmpTab_info *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || ((0 == index->almThrTmpIndex) &&
                                (0 == index->almThrTmpTpSubType)))
        {
            return (stAlmThrTmpTab_info *)cache->data_list->head->data ;
        }

        if ((pinfo_find->index.almThrTmpIndex == index->almThrTmpIndex) &&
                (pinfo_find->index.almThrTmpTpSubType == index->almThrTmpTpSubType))
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (stAlmThrTmpTab_info *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 statSubStatTypeReportTemplatTable 数据 */
int statSubStatTypeReportTemplatTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                                        stSubTypeReportTmpTab_info *pinfo_input)
{
    stSubTypeReportTmpTab_info *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if ((0 == pinfo_input->index.subReportTmpIndex) &&
        (0 == pinfo_input->index.subReportTmpIndex))
    {
        pinfo_input->index.subReportTmpIndex = 0xffffffff;
        pinfo_input->index.subReportSubType = 0xffffffff;
    }

    struct ipc_mesg_n *pMsgRcv = mib_stat_get_bulk((void *)&pinfo_input->index, SNMP_STAT_SUB_STAT_TYPE_REPORT_TEMPLAT_TABLE);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
        return FALSE;
    }

	pinfo = (stSubTypeReportTmpTab_info *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stSubTypeReportTmpTab_info));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stSubTypeReportTmpTab_info *statSubStatTypeReportTemplatTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                          int exact, const stSubTypeReportTmpTab_index *index)
{
    struct listnode  *node, *nnode;

    stSubTypeReportTmpTab_info *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || ((0 == index->subReportTmpIndex) &&
                                (0 == index->subReportSubType)))
        {
            return (stSubTypeReportTmpTab_info *)cache->data_list->head->data ;
        }

        if ((pinfo_find->index.subReportTmpIndex == index->subReportTmpIndex) &&
                (pinfo_find->index.subReportSubType == index->subReportSubType))
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (stSubTypeReportTmpTab_info *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 histStatDataTable 数据 */
int histStatDataTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                        stHistDataTab_info *pinfo_input)
{
    stHistDataTab_info *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if ((0 == pinfo_input->index.histDataIndex) &&
        (0 == pinfo_input->index.histDataStopTime) &&
        (0 == pinfo_input->index.histStatType))
    {
        pinfo_input->index.histDataIndex = 0xffffffff;
        pinfo_input->index.histDataStopTime = 0xffffffff;
        pinfo_input->index.histStatType = 0xffffffff;
    }

    struct ipc_mesg_n *pMsgRcv = mib_stat_get_bulk((void *)&pinfo_input->index, SNMP_HIST_STAT_DATA_TABLE);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
        return FALSE;
    }

	pinfo = (stHistDataTab_info *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stHistDataTab_info));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stHistDataTab_info *histStatDataTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                  int exact, const stHistDataTab_index *index)
{
    struct listnode  *node, *nnode;

    stHistDataTab_info *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || ((0 == index->histDataIndex) &&
                                (0 == index->histDataStopTime) &&
                                (0 == index->histStatType)))
        {
            return (stHistDataTab_info *)cache->data_list->head->data ;
        }

        if ((pinfo_find->index.histDataIndex == index->histDataIndex) &&
                (pinfo_find->index.histDataStopTime == index->histDataStopTime) &&
                (pinfo_find->index.histStatType == index->histStatType))
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (stHistDataTab_info *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 histDataReportInStandFormatTable 数据 */
int histDataReportInStandFormatTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                                       stHistDataReportFormatTab_info *pinfo_input)
{
    stHistDataReportFormatTab_info *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if ((0 == pinfo_input->index.histDataReportTaskBaseIndex) &&
        (0 == pinfo_input->index.histDataReportSubType))
    {
        pinfo_input->index.histDataReportTaskBaseIndex = 0xffffffff;
        pinfo_input->index.histDataReportSubType = 0xffffffff;
    }

    struct ipc_mesg_n *pMsgRcv = mib_stat_get_bulk((void *)&pinfo_input->index, SNMP_HIST_DATA_REPORT_IN_STAND_FORMAT_TABLE);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
        return FALSE;
    }

	pinfo = (stHistDataReportFormatTab_info *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stHistDataReportFormatTab_info));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stHistDataReportFormatTab_info *histDataReportInStandFormatTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                             int exact, const stHistDataReportFormatTab_index *index)
{
    struct listnode  *node, *nnode;

    stHistDataReportFormatTab_info *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || ((0 == index->histDataReportTaskBaseIndex) &&
                                (0 == index->histDataReportSubType)))
        {
            return (stHistDataReportFormatTab_info *)cache->data_list->head->data ;
        }

        if ((pinfo_find->index.histDataReportTaskBaseIndex == index->histDataReportTaskBaseIndex) &&
                (pinfo_find->index.histDataReportSubType == index->histDataReportSubType))
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (stHistDataReportFormatTab_info *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}






u_char *
hhrStatGlobalTimeScalar_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    if (MATCH_FAILED == header_generic(vp, name, length, exact, var_len, write_method))
    {
        return NULL;
    }

    uint8_t *pdata = NULL;
    int_value = 0;

    switch (vp->magic)
    {
        case hhrStatGlobalTimeOffsetPolarity:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_GLOBAL_TIME_OFFSET_POLARITY);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);


        case hhrStatGlobalTimeOffsetValue:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_GLOBAL_TIME_OFFSET_VALUE);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }

    return NULL;
}


u_char *
hhrStatMoniSourceScalar_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{

    if (MATCH_FAILED == header_generic(vp, name, length, exact, var_len, write_method))
    {
        return NULL;
    }

    uint8_t *pdata = NULL;

    switch (vp->magic)
    {
        case hhrStatMonObjNum:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_MON_OBJ_MAX_NUM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatAlreadyMonObjNum:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_ALREADY_MON_OBJ_NUM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatTaskMaxNum:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_TASK_MAX_NUM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatAlreadyRunTaskNum:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_ALREADY_RUN_TASK_NUM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatThdTmpMaxNum:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_THRESHOLD_TEMPLAT_MAX_NUM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatAlreadyThdTmpMaxNum:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_ALREADY_THRESHOLD_TEMPLATE_NUM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatSubFiltTpMaxNum:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_SUB_STAT_FILT_TP_MAX_NUM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatAlreadySubFiltTpMaxNum:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_ALREADY_SUB_STAT_FILT_TP_NUM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatSubReportTpMaxNum:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_SUB_REPORT_TP_MAX_NUM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatAlreadySubReportTpMaxNum:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_ALREADY_SUB_REPORT_IP_NUM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }

    return NULL;
}



u_char *
hhrStatAutoReportScalar_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    if (MATCH_FAILED == header_generic(vp, name, length, exact, var_len, write_method))
    {
        return NULL;
    }

    int_value = 0;

    switch (vp->magic)
    {
        case hhrStatAutoReportLastHist:
            *write_method = write_autoReportLastHist;
            mib_stat_getInt((uint8_t *)(uint8_t *)&int_value, SNMP_STAT_AUTO_REPORT_LAST_HIST);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatAutoReportOldHist:
            *write_method = write_autoReportOldHist;
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_AUTO_REPORT_OLD_HIST);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatAutoNotifyAlarm:
            *write_method = write_autoNotifyAlarm;
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_AUTO_NOTIFY_ALARM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatAutoNotifyEvent:
            *write_method = write_autoNotifyEvent;
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_AUTO_NOTIFY_EVENT);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }

    return NULL;
}



u_char *
hhrStatGetIndexScalar_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    if (MATCH_FAILED == header_generic(vp, name, length, exact, var_len, write_method))
    {
        return NULL;
    }

    int_value = 0;

    switch (vp->magic)
    {
        case hhrStatGetTaskId:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_GET_STAT_TASK_ID);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatGetEventThTpId:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_GET_STAT_EVENT_THRESHOLD_TP_ID);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatGetSubTypeTpId:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_GET_MARK_SUB_TYPE_TP_ID);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatGetAlarmThTpId:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_GET_STAT_ALARM_THRESHOLD_TP_ID);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatGetSubTypeReportTpId:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_GET_SUB_TYPE_REPORT_TP_ID);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }

    return NULL;
}



u_char *
hhrStatDataCollectTypeScalar_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    if (MATCH_FAILED == header_generic(vp, name, length, exact, var_len, write_method))
    {
        return NULL;
    }

    int_value = 0;

    switch (vp->magic)
    {
        case hhrStatDataCollectType:
            mib_stat_getInt((uint8_t *)&int_value, SNMP_STAT_DATA_COLLECT_TYPE);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }

    return NULL;
}



u_char *
hhrStatTableSupport_get(struct variable *vp,
                        oid *name,
                        size_t *length,
                        int exact, size_t *var_len, WriteMethod **write_method)
{
    if (MATCH_FAILED == header_generic(vp, name, length, exact, var_len, write_method))
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrStatPortMoniCtrlTableSupport:
            int_value = 1;
            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrCurrStatTableSupport:
            int_value = 1;
            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrTaskTableSupport:
            int_value = 1;
            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatEventThresholdTemplateTableSupport:
            int_value = 1;
            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatSubStatTypeFiltTpTableSupport:
            int_value = 1;
            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatAlarmThresholdTemplateTableSupport:
            int_value = 1;
            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrStatSubStatTypeReportTemplatTableSupport:
            int_value = 1;
            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrHistStatDataTableSupport:
            int_value = 1;
            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }

    return NULL;
}



u_char *
hhrStatGlobalScanTypeAtribTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    stStatGloScanTypeAttribTable_info *pinfo = NULL;
    stStatGloScanTypeAttribTable_index index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.typeIndex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == statGlobalScanTypeAttribTable_cache)
    {
        statGlobalScanTypeAttribTable_cache = snmp_cache_init(sizeof(stStatGloScanTypeAttribTable_info),
                                                              statGloScanTypeAttribTable_get_data_from_ipc,
                                                              statGloScanTypeAttribTable_node_lookup);

        if (NULL == statGlobalScanTypeAttribTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stStatGloScanTypeAttribTable_info *)snmp_cache_get_data_by_index(statGlobalScanTypeAttribTable_cache, exact, &index);

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_int_index_set(vp, name, length, pinfo->index.typeIndex);
        }
    }

    switch (vp->magic)
    {
        case hhrStatTypeIndex:
            if (pinfo)
            {
                uint_value = pinfo->index.typeIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatIncludeTypeNum:
            if (pinfo)
            {
                uint_value = pinfo->data.includeStatTypeNum;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatSubTypeNumInScanType:
            if (pinfo)
            {
                uint_value = pinfo->data.subTypeNumInScanType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycSeconds:
            if (pinfo)
            {
                uint_value = pinfo->data.longCycSeconds;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycSeconds:
            if (pinfo)
            {
                uint_value = pinfo->data.shortCycSeconds;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatTypeRowStatus:
            *write_method = write_statTypeRowStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}

u_char *
hhrStatGlobalSubTypeAtribTable_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    stGloSubTypeAttribTab_info *pinfo = NULL;
    stGloSubTypeAttribTab_index index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.subTypeIndex , exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == statGlobalSubTypeAttribTable_cache)
    {
        statGlobalSubTypeAttribTable_cache = snmp_cache_init(sizeof(stGloSubTypeAttribTab_info),
                                                             statGloSubTypeAttribTable_get_data_from_ipc,
                                                             statGloSubTypeAttribTable_node_lookup);

        if (NULL == statGlobalSubTypeAttribTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stGloSubTypeAttribTab_info *)snmp_cache_get_data_by_index(statGlobalSubTypeAttribTable_cache, exact, &index);

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_int_index_set(vp, name, length, pinfo->index.subTypeIndex);
        }
    }

    switch (vp->magic)
    {
        case hhrStatSubTypeIndex:
            if (pinfo)
            {
                uint_value = pinfo->index.subTypeIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatSubTypeBelongScanType:
            if (pinfo)
            {
                uint_value = pinfo->data.subTypeBelongScanType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatSubTypeValueClass:
            if (pinfo)
            {
                uint_value = pinfo->data.subTypeValueClass;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatSubTypeDataBitDeep:
            if (pinfo)
            {
                uint_value = pinfo->data.subTypeDataBitDeep;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycUpTdAlarm:
            if (pinfo)
            {
                uint_value = pinfo->data.longCycUpThrRelatAlarm;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycUpTdEvent:
            if (pinfo)
            {
                uint_value = pinfo->data.longCycUpThrRelatEvent;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycLowTdAlarm:
            if (pinfo)
            {
                uint_value = pinfo->data.longCycLowThrRelatAlarm;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycLowTdEvent:
            if (pinfo)
            {
                uint_value = pinfo->data.longCycLowThrRelatEvent;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycUpTdAlarm:
            if (pinfo)
            {
                uint_value = pinfo->data.shortCycUpThrRelatAlarm;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycUpTdEvent:
            if (pinfo)
            {
                uint_value = pinfo->data.shortCycUpThrRelatEvent;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycLowTdAlarm:
            if (pinfo)
            {
                uint_value = pinfo->data.shortCycLowThrRelatAlarm;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycLowTdEvent:
            if (pinfo)
            {
                uint_value = pinfo->data.shortCycLowThrRelatEvent;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatUsrDefCycUpTdAlarm:
            if (pinfo)
            {
                uint_value = pinfo->data.usrDefCycUpThrRelatAlarm;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatUsrDefCycUpTdEvent:
            if (pinfo)
            {
                uint_value = pinfo->data.usrDefCycUpThrRelatEvent;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatUsrDefCycLowTdAlarm:
            if (pinfo)
            {
                uint_value = pinfo->data.usrDefCycLowThrRelatAlarm;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatUsrDefCycLowTdEvent:
            if (pinfo)
            {
                uint_value = pinfo->data.usrDefCycLowThrRelatEvent;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycUpTdHigh32:
            *write_method = write_longCycUpTdHigh32;

            if (pinfo)
            {
                uint_value = pinfo->data.longCycUpThrHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycUpTdLow32:
            *write_method = write_longCycUpTdLow32;

            if (pinfo)
            {
                uint_value = pinfo->data.longCycUpThrLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycLowTdHigh32:
            *write_method = write_longCycLowTdHigh32;

            if (pinfo)
            {
                uint_value = pinfo->data.longCycLowThrHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycLowTdLow32:
            *write_method = write_longCycLowTdLow32;

            if (pinfo)
            {
                uint_value = pinfo->data.longCycLowThrLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycUpTdHigh32:
            *write_method = write_shortCycUpTdHigh32;

            if (pinfo)
            {
                uint_value = pinfo->data.shortCycUpThrHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycUpTdLow32:
            *write_method = write_shortCycUpTdLow32;

            if (pinfo)
            {
                uint_value = pinfo->data.shortCycUpThrLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycLowTdHigh32:
            *write_method = write_shortCycLowTdHigh32;

            if (pinfo)
            {
                uint_value = pinfo->data.shortCycLowThrHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycLowTdLow32:
            *write_method = write_shortCycLowTdLow32;

            if (pinfo)
            {
                uint_value = pinfo->data.shortCycLowThrLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatSubTypeRowStatus:
            *write_method = write_statSubTypeRowStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}

u_char *
hhrStatPortTypeToScanTypeTable_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    stPortTypeToScanTypeTab_info *pinfo = NULL;
    stPortTypeToScanTypeTab_index index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.portTypeIndex , exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == statPortTypeToStatScanTypeTable_cache)
    {
        statPortTypeToStatScanTypeTable_cache = snmp_cache_init(sizeof(stPortTypeToScanTypeTab_info),
                                                                statPortTypeToStatScanTypeTable_get_data_from_ipc,
                                                                statPortTypeToStatScanTypeTable_node_lookup);

        if (NULL == statPortTypeToStatScanTypeTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stPortTypeToScanTypeTab_info *)snmp_cache_get_data_by_index(statPortTypeToStatScanTypeTable_cache, exact, &index);

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_int_index_set(vp, name, length, pinfo->index.portTypeIndex);
        }
    }

    switch (vp->magic)
    {
        case hhrStatPortTypeIndex:
            if (pinfo)
            {
                uint_value = pinfo->index.portTypeIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatPortTypeAboutStatScanTypeNum:
            if (pinfo)
            {
                uint_value = pinfo->data.portTypeAboutScanTypeNum;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatScanType1IndexForPortType:
            if (pinfo)
            {
                uint_value = pinfo->data.type1Index;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatScanType2IndexForPortType:
            if (pinfo)
            {
                uint_value = pinfo->data.type2Index;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatScanType3IndexForPortType:
            if (pinfo)
            {
                uint_value = pinfo->data.type3Index;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatScanType4IndexForPortType:
            if (pinfo)
            {
                uint_value = pinfo->data.type4Index;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatScanType5IndexForPortType:
            if (pinfo)
            {
                uint_value = pinfo->data.type5Index;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatPortTypeToScanTypeRowStatus:
            *write_method = write_statPortTypeToScanTypeRowStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}

u_char *
hhrStatPortMoniCtrlTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    stPortMoniCtrlTab_info *pinfo = NULL;
    stPortMoniCtrlTab_index index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx7_index_get(vp, name, length, &index.portMoniIndex1,
                                     &index.portMoniIndex2,
                                     &index.portMoniIndex3,
                                     &index.portMoniIndex4,
                                     &index.portMoniIndex5,
                                     &index.portMoniDevIndex,
                                     &index.portMoniScanType, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == statPortMoniCtrlTable_chche)
    {
        statPortMoniCtrlTable_chche = snmp_cache_init(sizeof(stPortMoniCtrlTab_info),
                                                      statPortMoniCtrlTable_get_data_from_ipc,
                                                      statPortMoniCtrlTable_node_lookup);

        if (NULL == statPortMoniCtrlTable_chche)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stPortMoniCtrlTab_info *)snmp_cache_get_data_by_index(statPortMoniCtrlTable_chche, exact, &index);

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_intx7_index_set(vp, name, length, pinfo->index.portMoniIndex1,
                                       pinfo->index.portMoniIndex2,
                                       pinfo->index.portMoniIndex3,
                                       pinfo->index.portMoniIndex4,
                                       pinfo->index.portMoniIndex5,
                                       pinfo->index.portMoniDevIndex,
                                       pinfo->index.portMoniScanType);
        }
    }

    switch (vp->magic)
    {
#if 0

        case hhrStatPortMoniIndex1:
            if (pinfo)
            {
                uint_value = pinfo->index.portMoniIndex1;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatPortMoniIndex2:
            if (pinfo)
            {
                uint_value = pinfo->index.portMoniIndex2;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatPortMoniIndex3:
            if (pinfo)
            {
                uint_value = pinfo->index.portMoniIndex3;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatPortMoniIndex4:
            if (pinfo)
            {
                uint_value = pinfo->index.portMoniIndex4;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatPortMoniIndex5:
            if (pinfo)
            {
                uint_value = pinfo->index.portMoniIndex5;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatPortMoniDevIndex:
            if (pinfo)
            {
                uint_value = pinfo->index.portMoniDevIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatPortMoniScanType:
            if (pinfo)
            {
                uint_value = pinfo->index.portMoniScanType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

#endif

        case hhrStatMoniEn:
            *write_method = write_moniEn;

            if (pinfo)
            {
                uint_value = pinfo->data.moniEn;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatCurrMoniEn:
            *write_method = write_currMoniEn;

            if (pinfo)
            {
                uint_value = pinfo->data.currMoniEn;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycStatMoniEn:
            *write_method = write_longCycMoniEn;

            if (pinfo)
            {
                uint_value = pinfo->data.longCycMoniEn;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycBelongTask:
            *write_method = write_longCycBelongTask;

            if (pinfo)
            {
                uint_value = pinfo->data.longCycBelongTask;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycStatEventTdLocat:
            *write_method = write_longCycEvtThrLocation;

            if (pinfo)
            {
                uint_value = pinfo->data.longCycEvtThrLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycSubStatTypeFiltTdLocat:
            *write_method = write_longCycSubTypeFiltLocation;

            if (pinfo)
            {
                uint_value = pinfo->data.longCycSubTypeFiltLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycStatAlarmTdLocat:
            if (pinfo)
            {
                uint_value = pinfo->data.longCycAlmThrLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycHistAutoReport:
            *write_method = write_longCycHistAutoReport;

            if (pinfo)
            {
                uint_value = pinfo->data.longCycHistAutoReport;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatLongCycStatHistLocat:
            if (pinfo)
            {
                uint_value = pinfo->data.longCycHistLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycStatMoniEn:
            *write_method = write_shortCycMoniEn;

            if (pinfo)
            {
                uint_value = pinfo->data.shortCycMoniEn;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycBelongTask:
            *write_method = write_shortCycBelongTask;

            if (pinfo)
            {
                uint_value = pinfo->data.shortCycBelongTask;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycStatEventTdLocat:
            *write_method = write_shortCycEvtThrLocation;

            if (pinfo)
            {
                uint_value = pinfo->data.shortCycEvtThrLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycSubStatTypeFiltTdLocat:
            *write_method = write_shortCycSubTypeFiltLocation;

            if (pinfo)
            {
                uint_value = pinfo->data.shortCycSubTypeFiltLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycStatAlarmTdLocat:
            if (pinfo)
            {
                uint_value = pinfo->data.shortCycAlmThrLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycHistAutoReport:
            *write_method = write_shortCycHistAutoReport;

            if (pinfo)
            {
                uint_value = pinfo->data.shortCycHistAutoReport;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatShortCycStatHistLocat:
            if (pinfo)
            {
                uint_value = pinfo->data.shortCycHistLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatUsrDefCycStatMoniEn:
            *write_method = write_usrDefCycMoniEn;

            if (pinfo)
            {
                uint_value = pinfo->data.usrDefCycMoniEn;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatUsrDefCycBelongTask:
            *write_method = write_usrDefCycBelongTask;

            if (pinfo)
            {
                uint_value = pinfo->data.usrDefCycBelongTask;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatUsrDefCycStatEventTdLocat:
            *write_method = write_usrDefCycEvtThrLocation;

            if (pinfo)
            {
                uint_value = pinfo->data.usrDefCycEvtThrLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatUsrDefCycSubStatTypeFiltTdLocat:
            *write_method = write_usrDefCycSubTypeFiltLocation;

            if (pinfo)
            {
                uint_value = pinfo->data.usrDefCycSubTypeFiltLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatUsrDefCycStatAlarmTdLocat:
            if (pinfo)
            {
                uint_value = pinfo->data.usrDefCycAlmThrLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatUsrDefCycHistAutoReport:
            *write_method = write_usrDefCycHistAutoReport;

            if (pinfo)
            {
                uint_value = pinfo->data.usrDefCycHistAutoReport;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatUsrDefCycStatHistLocat:
            if (pinfo)
            {
                uint_value = pinfo->data.usrDefCycHistLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatUsrDefCycHistSeconds:
            if (pinfo)
            {
                uint_value = pinfo->data.usrDefCycHistSeconds;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatPortMoniRowStatus:
            *write_method = write_statPortMoniRowStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}

u_char *
hhrStatCurrStatTable_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    stCurrStatTab_info *pinfo = NULL;
    stCurrStatTab_index index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx7_index_get(vp, name, length, &index.currPortIndex1,
                                     &index.currPortIndex2,
                                     &index.currPortIndex3,
                                     &index.currPortIndex4,
                                     &index.currPortIndex5,
                                     &index.currDevIndex,
                                     &index.currDataType, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == currStatTable_cache)
    {
        currStatTable_cache = snmp_cache_init(sizeof(stCurrStatTab_info),
                                              currStatTable_get_data_from_ipc,
                                              currStatTable_node_lookup);

        if (NULL == currStatTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stCurrStatTab_info *)snmp_cache_get_data_by_index(currStatTable_cache, exact, &index);

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_intx7_index_set(vp, name, length, pinfo->index.currPortIndex1,
                                       pinfo->index.currPortIndex2,
                                       pinfo->index.currPortIndex3,
                                       pinfo->index.currPortIndex4,
                                       pinfo->index.currPortIndex5,
                                       pinfo->index.currDevIndex,
                                       pinfo->index.currDataType);
        }
    }

    switch (vp->magic)
    {
#if 0

        case hhrStatCurrStatPortIndex1:
            if (pinfo)
            {
                uint_value = pinfo->index.currPortIndex1;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatCurrStatPortIndex2:
            if (pinfo)
            {
                uint_value = pinfo->index.currPortIndex2;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatCurrStatPortIndex3:
            if (pinfo)
            {
                uint_value = pinfo->index.currPortIndex3;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatCurrStatPortIndex4:
            if (pinfo)
            {
                uint_value = pinfo->index.currPortIndex4;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatCurrStatPortIndex5:
            if (pinfo)
            {
                uint_value = pinfo->index.currPortIndex5;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatCurrStatDevIndex:
            if (pinfo)
            {
                uint_value = pinfo->index.currDevIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatCurrStatDataType:
            if (pinfo)
            {
                uint_value = pinfo->index.currDataType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

#endif

        case hhrStatClearCurrDataClear:
            *write_method = write_clearCurrData;

            if (pinfo)
            {
                uint_value = pinfo->data.clearCurrData;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatCurrDataTypeValueHigh32:
            if (pinfo)
            {
                uint_value = pinfo->data.currDataTypeValHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatCurrDataTypeValueLow32:
            if (pinfo)
            {
                uint_value = pinfo->data.currDataTypeValLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatCurrRowStatus:
            *write_method = write_currStatRowStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}


u_char *
hhrStatTaskTable_get(struct variable *vp,
                     oid *name,
                     size_t *length,
                     int exact, size_t *var_len, WriteMethod **write_method)
{
    stStatTaskTab_info *pinfo = NULL;
    stStatTaskTab_index index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.taskIndex , exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == statTaskTable_cache)
    {
        statTaskTable_cache = snmp_cache_init(sizeof(stStatTaskTab_info),
                                              statTaskTable_get_data_from_ipc,
                                              statTaskTable_node_lookup);

        if (NULL == statTaskTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stStatTaskTab_info *)snmp_cache_get_data_by_index(statTaskTable_cache, exact, &index);

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_int_index_set(vp, name, length, pinfo->index.taskIndex);
        }
    }

    switch (vp->magic)
    {
#if 0

        case hhrStatTaskIndex:
            if (pinfo)
            {
                uint_value = pinfo->index.taskIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

#endif

        case hhrStatMaxPortInTask:
            *write_method = write_maxPortInTask;

            if (pinfo)
            {
                uint_value = pinfo->data.maxPortInTask;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatTaskScanType:
            *write_method = write_taskScanType;

            if (pinfo)
            {
                uint_value = pinfo->data.taskScanType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatIncludeMonObjNum:
            if (pinfo)
            {
                uint_value = pinfo->data.includeMonObjNum;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatTaskCycMoniType:
            *write_method = write_statTaskCycMoniType;

            if (pinfo)
            {
                uint_value = pinfo->data.taskCycMoniType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatTaskMoniCycSeconds:
            *write_method = write_taskMoniCycSec;

            if (pinfo)
            {
                uint_value = pinfo->data.taskMoniCycSec;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatTaskStartTime:
            *write_method = write_taskStartTime;

            if (pinfo)
            {
                uint_value = pinfo->data.taskStartTime;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatTaskEndTime:
            *write_method = write_taskEndTime;

            if (pinfo)
            {
                uint_value = pinfo->data.taskEndTime;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatTaskAlarmTdTpLocat:
            *write_method = write_taskAlmThrTpLocation;

            if (pinfo)
            {
                uint_value = pinfo->data.taskAlmThrTpLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatTaskSubReportTpLocat:
            *write_method = write_taskSubReportLocation;

            if (pinfo)
            {
                uint_value = pinfo->data.taskSubReportLocation;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatTaskRowStatus:
            *write_method = write_statTaskRowStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}


u_char *
hhrStatEventTdTpTable_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    stEvtThrTmpTab_info *pinfo = NULL;
    stEvtThrTmpTab_index index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index.evtThrTmpIndex,
                                     &index.evtThrTmpTpSubType, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == statEventThresholdTemplatTable_cache)
    {
        statEventThresholdTemplatTable_cache = snmp_cache_init(sizeof(stEvtThrTmpTab_info),
                                                               statEvtThrTemplatTable_get_data_from_ipc,
                                                               statEvtThrTemplatTable_node_lookup);

        if (NULL == statEventThresholdTemplatTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stEvtThrTmpTab_info *)snmp_cache_get_data_by_index(statEventThresholdTemplatTable_cache, exact, &index);

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_intx2_index_set(vp, name, length, pinfo->index.evtThrTmpIndex,
                                       pinfo->index.evtThrTmpTpSubType);
        }
    }

    switch (vp->magic)
    {
#if 0

        case hhrStatEventTdTpIndex:
            if (pinfo)
            {
                uint_value = pinfo->index.evtThrTmpIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatEventTdTpSubType:
            if (pinfo)
            {
                uint_value = pinfo->index.evtThrTmpTpSubType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

#endif

        case hhrStatEventTdTpScanType:
            *write_method = write_evtThrScanType;

            if (pinfo)
            {
                uint_value = pinfo->data.evtThrScanType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatEventTdTpSubTypeUpHigh32:
            *write_method = write_evtThrTpSubTypeUpThrHigh32;

            if (pinfo)
            {
                uint_value = pinfo->data.evtThrTpSubTypeUpThrHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatEventTdTpSubTypeUpLow32:
            *write_method = write_evtThrTpSubTypeUpThrLow32;

            if (pinfo)
            {
                uint_value = pinfo->data.evtThrTpSubTypeUpThrLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatEventTdTpSubTypeLowHigh32:
            *write_method = write_evtThrTpSubTypeLowThrHigh32;

            if (pinfo)
            {
                uint_value = pinfo->data.evtThrTpSubTypeLowThrHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatEventTdTpSubTypeLowLow32:
            *write_method = write_evtThrTpSubTypeLowThrLow32;

            if (pinfo)
            {
                uint_value = pinfo->data.evtThrTpSubTypeLowThrLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatEventTdTpRowStatus:
            *write_method = write_statEventTdTpRowStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}

u_char *
hhrStatSubStatTypeFiltTpTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    stSubStatTypeFileTpTab_info *pinfo = NULL;
    stSubStatTypeFileTpTab_index index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index.subTypeFiltIndex,
                                     &index.selectSubType, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == statSubStatTypeFileTpTable_cache)
    {
        statSubStatTypeFileTpTable_cache = snmp_cache_init(sizeof(stSubStatTypeFileTpTab_info),
                                                           statSubStatTypeFileTpTable_get_data_from_ipc,
                                                           statSubStatTypeFileTpTable_node_lookup);

        if (NULL == statSubStatTypeFileTpTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stSubStatTypeFileTpTab_info *)snmp_cache_get_data_by_index(statSubStatTypeFileTpTable_cache, exact, &index);

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_intx2_index_set(vp, name, length, pinfo->index.subTypeFiltIndex,
                                       pinfo->index.selectSubType);
        }
    }

    switch (vp->magic)
    {
#if 0

        case hhrStatSubStatTypeFiltIndex:
            if (pinfo)
            {
                uint_value = pinfo->index.subTypeFiltIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatSelectSubType:
            if (pinfo)
            {
                uint_value = pinfo->index.selectSubType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

#endif

        case hhrStatSelectStatScanType:
            *write_method = write_selectScanType;

            if (pinfo)
            {
                uint_value = pinfo->data.selectScanType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatSelectStatus:
            *write_method = write_selectStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.selectStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatSelectSubRowStatus:
            *write_method = write_statSelectSubRowStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}

u_char *
hhrStatAlarmTdTpTable_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    stAlmThrTmpTab_info *pinfo = NULL;
    stAlmThrTmpTab_index index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index.almThrTmpIndex,
                                     &index.almThrTmpTpSubType, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == statAlarmThresholdTemplatTable_cache)
    {
        statAlarmThresholdTemplatTable_cache = snmp_cache_init(sizeof(stAlmThrTmpTab_info),
                                                               stAlmThrTmpTab_get_data_from_ipc,
                                                               stAlmThrTmpTab_node_lookup);

        if (NULL == statAlarmThresholdTemplatTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stAlmThrTmpTab_info *)snmp_cache_get_data_by_index(statAlarmThresholdTemplatTable_cache, exact, &index);

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_intx2_index_set(vp, name, length, pinfo->index.almThrTmpIndex,
                                       pinfo->index.almThrTmpTpSubType);
        }
    }

    switch (vp->magic)
    {
#if 0

        case hhrStatAlarmTdTpIndex:
            if (pinfo)
            {
                uint_value = pinfo->index.almThrTmpIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatAlarmTdSubType:
            if (pinfo)
            {
                uint_value = pinfo->index.almThrTmpTpSubType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

#endif

        case hhrStatAlarmTdScanType:
            *write_method = write_almThrScanType;

            if (pinfo)
            {
                uint_value = pinfo->data.almThrScanType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatAlarmTdSubTypeUpRiseHigh32:
            *write_method = write_almThrTpSubTypeUpRiseHigh32;

            if (pinfo)
            {
                uint_value = pinfo->data.almThrTpSubTypeUpRiseHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatAlarmTdSubTypeUpRiseLow32:
            *write_method = write_almThrTpSubTypeUpRiseLow32;

            if (pinfo)
            {
                uint_value = pinfo->data.almThrTpSubTypeUpRiseLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatAlarmTdSubTypeUpClearHigh32:
            *write_method = write_almThrTpSubTypeUpClrHigh32;

            if (pinfo)
            {
                uint_value = pinfo->data.almThrTpSubTypeUpClrHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatAlarmTdSubTypeUpClearLow32:
            *write_method = write_almThrTpSubTypeUpClrLow32;

            if (pinfo)
            {
                uint_value = pinfo->data.almThrTpSubTypeUpClrLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatAlarmTdSubTypeDnRiseHigh32:
            *write_method = write_almThrTpSubTypeDnRiseHigh32;

            if (pinfo)
            {
                uint_value = pinfo->data.almThrTpSubTypeDnRiseHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatAlarmTdSubTypeDnRiseLow32:
            *write_method = write_almThrTpSubTypeDnRiseLow32;

            if (pinfo)
            {
                uint_value = pinfo->data.almThrTpSubTypeDnRiseLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatAlarmTdSubTypeDnClearHigh32:
            *write_method = write_almThrTpSubTypeDnClrHigh32;

            if (pinfo)
            {
                uint_value = pinfo->data.almThrTpSubTypeDnClrHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatAlarmTdSubTypeDnClearLow32:
            *write_method = write_almThrTpSubTypeDnClrLow32;

            if (pinfo)
            {
                uint_value = pinfo->data.almThrTpSubTypeDnClrLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatAlarmTdTpRowStatus:
            *write_method = write_statAlarmTdTpRowStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}

u_char *
hhrStatSubStatTypeReportTpTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    stSubTypeReportTmpTab_info *pinfo = NULL;
    stSubTypeReportTmpTab_index index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index.subReportTmpIndex,
                                     &index.subReportSubType, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == statSubStatTypeReportTemplatTable_cache)
    {
        statSubStatTypeReportTemplatTable_cache = snmp_cache_init(sizeof(stSubTypeReportTmpTab_info),
                                                                  statSubStatTypeReportTemplatTable_get_data_from_ipc,
                                                                  statSubStatTypeReportTemplatTable_node_lookup);

        if (NULL == statSubStatTypeReportTemplatTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stSubTypeReportTmpTab_info *)snmp_cache_get_data_by_index(statSubStatTypeReportTemplatTable_cache, exact, &index);

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_intx2_index_set(vp, name, length, pinfo->index.subReportTmpIndex,
                                       pinfo->index.subReportSubType);
        }
    }

    switch (vp->magic)
    {
#if 0

        case hhrStatSubStatTypeReportTpIndex:
            if (pinfo)
            {
                uint_value = pinfo->index.subReportTmpIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatSubReportSubType:
            if (pinfo)
            {
                uint_value = pinfo->index.subReportSubType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

#endif

        case hhrStatSubReportScanType:
            *write_method = write_subReportScanType;

            if (pinfo)
            {
                uint_value = pinfo->data.subReportScanType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatSubReportStatus:
            *write_method = write_subReportStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.subReportStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatSubReportRowStatus:
            *write_method = write_statSubReportRowStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}

u_char *
hhrStatHistDataTable_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    stHistDataTab_info *pinfo = NULL;
    stHistDataTab_index index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx3_index_get(vp, name, length, &index.histDataIndex,
                                     &index.histDataStopTime,
                                     &index.histStatType, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == histStatDataTable_cache)
    {
        histStatDataTable_cache = snmp_cache_init(sizeof(stHistDataTab_info),
                                                  histStatDataTable_get_data_from_ipc,
                                                  histStatDataTable_node_lookup);

        if (NULL == histStatDataTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stHistDataTab_info *)snmp_cache_get_data_by_index(histStatDataTable_cache, exact, &index);

    /* get ready the next index */

    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_intx3_index_set(vp, name, length, pinfo->index.histDataIndex,
                                       pinfo->index.histDataStopTime,
                                       pinfo->index.histStatType);
        }
    }

    switch (vp->magic)
    {
#if 0

        case hhrStatHistDataIndex:
            if (pinfo)
            {
                uint_value = pinfo->index.histDataIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataStopTime:
            if (pinfo)
            {
                uint_value = pinfo->index.histDataStopTime;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistStatType:
            if (pinfo)
            {
                uint_value = pinfo->index.histStatType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

#endif

        case hhrStatHistDataScanType:
            if (pinfo)
            {
                uint_value = pinfo->data.histDSScanType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataHigh32:
            if (pinfo)
            {
                uint_value = pinfo->data.histTypeDataHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataLow32:
            if (pinfo)
            {
                uint_value = pinfo->data.histTypeDataLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataRowStatus:
            *write_method = write_statHistDataRowStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}

u_char *
hhrStatHistDataReportTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    stHistDataReportFormatTab_info *pinfo = NULL;
    stHistDataReportFormatTab_index index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index.histDataReportTaskBaseIndex,
                                     &index.histDataReportSubType, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == histDataReportInStandFormatTable_cache)
    {
        histDataReportInStandFormatTable_cache = snmp_cache_init(sizeof(stHistDataReportFormatTab_info),
                                                                 histDataReportInStandFormatTable_get_data_from_ipc,
                                                                 histDataReportInStandFormatTable_node_lookup);

        if (NULL == histDataReportInStandFormatTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stHistDataReportFormatTab_info *)snmp_cache_get_data_by_index(histDataReportInStandFormatTable_cache, exact, &index);

    if (NULL == pinfo)
    {
        return NULL;
    }

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_intx2_index_set(vp, name, length, pinfo->index.histDataReportTaskBaseIndex,
                                       pinfo->index.histDataReportSubType);
        }
    }

    switch (vp->magic)
    {
#if 0

        case hhrStatHistDataReportBaseIndex:
            if (pinfo)
            {
                uint_value = pinfo->index.histDataReportTaskBaseIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataReportSubType:
            if (pinfo)
            {
                uint_value = pinfo->index.histDataReportSubType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

#endif

        case hhrStatHistDataReportStopTime:
            if (pinfo)
            {
                uint_value = pinfo->data.histDRStopTime;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataReportScanype:
            if (pinfo)
            {
                uint_value = pinfo->data.histDRScanType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataReportCycType:
            if (pinfo)
            {
                uint_value = pinfo->data.histDRCycType;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataReportPortIndex1:
            if (pinfo)
            {
                uint_value = pinfo->data.histDRPortIndex1;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataReportPortIndex2:
            if (pinfo)
            {
                uint_value = pinfo->data.histDRPortIndex2;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataReportPortIndex3:
            if (pinfo)
            {
                uint_value = pinfo->data.histDRPortIndex3;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataReportPortIndex4:
            if (pinfo)
            {
                uint_value = pinfo->data.histDRPortIndex4;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataReportPortIndex5:
            if (pinfo)
            {
                uint_value = pinfo->data.histDRPortIndex5;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataReportDevIndex:
            if (pinfo)
            {
                uint_value = pinfo->data.histDRDevIndex;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataReportTypeHigh32:
            if (pinfo)
            {
                uint_value = pinfo->data.histDRTypeHigh32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataReportTypeLow32:
            if (pinfo)
            {
                uint_value = pinfo->data.histDRTypeLow32;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        case hhrStatHistDataReportRowStatus:
            *write_method = write_statHistDataReportRowStatus;

            if (pinfo)
            {
                uint_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&uint_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }

}


/**************************** write interface ********************************/
int write_autoReportLastHist(int action, u_char *var_val,
                             u_char var_val_type, size_t var_val_len,
                             u_char *statP, oid *name, size_t length)
{
    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    memcpy(&i_data, var_val, var_val_len);  //将设置的值存储到i_data中，给下一步使用

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    if (mib_stat_setInt(i_data, SNMP_STAT_AUTO_REPORT_LAST_HIST) == FALSE)    //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int write_autoReportOldHist(int action, u_char *var_val,
                            u_char var_val_type, size_t var_val_len,
                            u_char *statP, oid *name, size_t length)
{
    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    memcpy(&i_data, var_val, var_val_len);  //将设置的值存储到i_data中，给下一步使用

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    if (mib_stat_setInt(i_data, SNMP_STAT_AUTO_REPORT_OLD_HIST) == FALSE)     //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}


int write_autoNotifyAlarm(int action, u_char *var_val,
                          u_char var_val_type, size_t var_val_len,
                          u_char *statP, oid *name, size_t length)
{
    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    memcpy(&i_data, var_val, var_val_len);  //将设置的值存储到i_data中，给下一步使用

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    if (mib_stat_setInt(i_data, SNMP_STAT_AUTO_NOTIFY_ALARM) == FALSE)    //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int write_autoNotifyEvent(int action, u_char *var_val,
                          u_char var_val_type, size_t var_val_len,
                          u_char *statP, oid *name, size_t length)
{
    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    memcpy(&i_data, var_val, var_val_len);  //将设置的值存储到i_data中，给下一步使用

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    if (mib_stat_setInt(i_data, SNMP_STAT_AUTO_NOTIFY_EVENT) == FALSE)    //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}



/********************************************** write table ************************************************************/

/**************************************** write statGlobalScanType table *********************************************/


int write_statTypeRowStatus(int action, u_char *var_val,
                            u_char var_val_type, size_t var_val_len,
                            u_char *statP, oid *name, size_t length)
{
    stStatGloScanTypeAttribTable_info info;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    info.index.typeIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
#if 0

    switch (action)
    {
        case RESERVE1:
            if (statScanTypeRowStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

#endif

    return SNMP_ERR_NOERROR;
}


/**************************************** Read & Operate Functions *********************************************/


static int statGlobalSubTypeReadMessage(stGloSubTypeAttribTab_index index_input)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
	
    stGloSubTypeAttribTab_index index;
    index.subTypeIndex = index_input.subTypeIndex;

    if (subTypeData.index.subTypeIndex != index.subTypeIndex)
    {
        memset(&subTypeData, 0, sizeof(stGloSubTypeAttribTab_info));
    }

    if (!subTypeData.data.rowStatus)
    {
        /*pinfo = (stGloSubTypeAttribTab_info *)ipc_send_common_wait_reply(&index, sizeof(stGloSubTypeAttribTab_index),
                                                                         1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                                                                         SNMP_STAT_GLOBAL_SUB_TYPE_ATTRI_TABLE, IPC_OPCODE_GET, 0);*/
		
		pMsgRcv = ipc_sync_send_n2(&index, sizeof(stGloSubTypeAttribTab_index), 1, MODULE_ID_STAT,
			MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_STAT_GLOBAL_SUB_TYPE_ATTRI_TABLE, IPC_OPCODE_GET, 0, 1000);

		if (NULL == pMsgRcv)
		{
			return FALSE;
		}

        if (0 != pMsgRcv->msghdr.data_num)
        {
            memcpy(&subTypeData, pMsgRcv->msg_data, sizeof(stGloSubTypeAttribTab_info));
            subTypeData.data.rowStatus  = SNMP_ROW_ACTIVE;

			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return TRUE;
        }
        else
        {
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return FALSE;
        }
    }

    return TRUE;
}

static int statGlobalSubTypeOperate(int mode, stGloSubTypeAttribTab_index index)
{
    stGloSubTypeAttribTab_info info;
    memset(&info, 0, sizeof(stGloSubTypeAttribTab_info));
    info.index.subTypeIndex = index.subTypeIndex;

    info.data.longCycUpThrHigh32 = statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_LONGCYCUPTHRESHOLDHIGH32
                                   ? statGlobalSubTypeAtrribTableList.data.longCycUpThrHigh32 : subTypeData.data.longCycUpThrHigh32;
    info.data.longCycUpThrLow32 = statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_LONGCYCUPTHRESHOLDLOW32
                                  ? statGlobalSubTypeAtrribTableList.data.longCycUpThrLow32 : subTypeData.data.longCycUpThrLow32;
    info.data.longCycLowThrHigh32 = statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_LONGCYCLOWTHRESHOLDHIGH32
                                    ? statGlobalSubTypeAtrribTableList.data.longCycLowThrHigh32 : subTypeData.data.longCycLowThrHigh32;
    info.data.longCycLowThrLow32 = statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_LONGCYCLOWTHRESHOLDLOW32
                                   ? statGlobalSubTypeAtrribTableList.data.longCycLowThrLow32 : subTypeData.data.longCycLowThrLow32;
    info.data.shortCycUpThrHigh32 = statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_SHORTCYCUPTHRESHOLDHIHG32
                                    ? statGlobalSubTypeAtrribTableList.data.shortCycUpThrHigh32 : subTypeData.data.shortCycUpThrHigh32;
    info.data.shortCycUpThrLow32 = statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_SHORTCYCUPTHRESHOLDLOW32
                                   ? statGlobalSubTypeAtrribTableList.data.shortCycUpThrLow32 : subTypeData.data.shortCycUpThrLow32;
    info.data.shortCycLowThrHigh32 = statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_SHORTCYCLOWTHRESHOLDHIGH32
                                     ? statGlobalSubTypeAtrribTableList.data.shortCycLowThrHigh32 : subTypeData.data.shortCycLowThrHigh32;
    info.data.shortCycLowThrLow32 = statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_SHORTCYCLOWTHRESHOLDLOW32
                                    ? statGlobalSubTypeAtrribTableList.data.shortCycLowThrLow32 : subTypeData.data.shortCycLowThrLow32;

    uchar buf[IPC_MSG_LEN];
    int buf_len = 0;
    memset(buf, 0, IPC_MSG_LEN);

    memcpy(buf, &mode, sizeof(int));
    memcpy(buf + sizeof(int), &info, sizeof(stGloSubTypeAttribTab_info));
    buf_len = sizeof(int) + sizeof(stGloSubTypeAttribTab_info);

    /*ipc_send_common(buf, buf_len, 1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                    SNMP_STAT_GLOBAL_SUB_TYPE_ATTRI_TABLE, IPC_OPCODE_UPDATE);*/

	stat_mib_set(buf, buf_len, MODULE_ID_STAT, IPC_TYPE_SNMP, SNMP_STAT_GLOBAL_SUB_TYPE_ATTRI_TABLE,
		IPC_OPCODE_UPDATE);

    return TRUE;
}


/*statGlobalSubType*/
static int statPortMoniCtrlReadMessage(stPortMoniCtrlTab_index index_input)
{
    struct ipc_mesg_n *pMsgRcv = NULL;

	stPortMoniCtrlTab_index index;
    index.portMoniIndex1 = index_input.portMoniIndex1;
    index.portMoniIndex2 = index_input.portMoniIndex2;
    index.portMoniIndex3 = index_input.portMoniIndex3;
    index.portMoniIndex4 = index_input.portMoniIndex4;
    index.portMoniIndex5 = index_input.portMoniIndex5;
    index.portMoniDevIndex = index_input.portMoniDevIndex;
    index.portMoniScanType = index_input.portMoniScanType;

    if ((portMoniCtrlData.index.portMoniIndex1 != index.portMoniIndex1) ||
            (portMoniCtrlData.index.portMoniIndex2 != index.portMoniIndex2) ||
            (portMoniCtrlData.index.portMoniIndex3 != index.portMoniIndex3) ||
            (portMoniCtrlData.index.portMoniIndex4 != index.portMoniIndex4) ||
            (portMoniCtrlData.index.portMoniIndex5 != index.portMoniIndex5) ||
            (portMoniCtrlData.index.portMoniDevIndex != index.portMoniDevIndex) ||
            (portMoniCtrlData.index.portMoniScanType != index.portMoniScanType))
    {
        memset(&portMoniCtrlData, 0, sizeof(stPortMoniCtrlTab_info));
    }

    if (!portMoniCtrlData.data.rowStatus)
    {
        /*pinfo = (stPortMoniCtrlTab_info *)ipc_send_common_wait_reply(&index, sizeof(stPortMoniCtrlTab_index),
                                                                     1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                                                                     SNMP_STAT_PORT_MONI_CTRL_TABLE, IPC_OPCODE_GET, 0);*/

		pMsgRcv = ipc_sync_send_n2(&index, sizeof(stPortMoniCtrlTab_index), 1, MODULE_ID_STAT,
			MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_STAT_PORT_MONI_CTRL_TABLE, IPC_OPCODE_GET, 0, 1000);

		if (NULL == pMsgRcv)
		{
			return FALSE;
		}

        if (0 != pMsgRcv->msghdr.data_num)
        {
            memcpy(&portMoniCtrlData, pMsgRcv->msg_data, sizeof(stPortMoniCtrlTab_info));
            portMoniCtrlData.data.rowStatus = SNMP_ROW_ACTIVE;

			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return TRUE;
        }
        else
        {
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return FALSE;
        }
    }

    return TRUE;
}

static int statPortMoniCtrlOperate(int mode, stPortMoniCtrlTab_index index)
{
    int opcode = 0;
    int flag_set = PORT_MONI_INVALID;

    stPortMoniCtrlTab_info info;
    memset(&info, 0, sizeof(stPortMoniCtrlTab_info));
    info.index.portMoniIndex1 = index.portMoniIndex1;
    info.index.portMoniIndex2 = index.portMoniIndex2;
    info.index.portMoniIndex3 = index.portMoniIndex3;
    info.index.portMoniIndex4 = index.portMoniIndex4;
    info.index.portMoniIndex5 = index.portMoniIndex5;
    info.index.portMoniDevIndex = index.portMoniDevIndex;
    info.index.portMoniScanType = index.portMoniScanType;

    info.data.moniEn = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_STATMONIEN
                       ? statPortMoniCtrlTableList.data.moniEn : portMoniCtrlData.data.moniEn;
    info.data.currMoniEn = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_CURRSTATMONIEN
                           ? statPortMoniCtrlTableList.data.currMoniEn : portMoniCtrlData.data.currMoniEn;

    info.data.longCycMoniEn = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCSTATMONIEN
                              ? statPortMoniCtrlTableList.data.longCycMoniEn : portMoniCtrlData.data.longCycMoniEn;
    info.data.longCycBelongTask = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCBELONGTASK
                                  ? statPortMoniCtrlTableList.data.longCycBelongTask : portMoniCtrlData.data.longCycBelongTask;
    info.data.longCycEvtThrLocation = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCSTATEVENTTHRESHOLDLOCATION
                                      ? statPortMoniCtrlTableList.data.longCycEvtThrLocation : portMoniCtrlData.data.longCycEvtThrLocation;
    info.data.longCycSubTypeFiltLocation = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCSUBSTATTYPEFILTLOCATION
                                           ? statPortMoniCtrlTableList.data.longCycSubTypeFiltLocation : portMoniCtrlData.data.longCycSubTypeFiltLocation;
    info.data.longCycHistAutoReport = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCHISTAUTOREPORT
                                      ? statPortMoniCtrlTableList.data.longCycHistAutoReport : portMoniCtrlData.data.longCycHistAutoReport;

    info.data.shortCycMoniEn = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCSTATMONIEN
                               ? statPortMoniCtrlTableList.data.shortCycMoniEn : portMoniCtrlData.data.shortCycMoniEn;
    info.data.shortCycBelongTask = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCBELONGTASK
                                   ? statPortMoniCtrlTableList.data.shortCycBelongTask : portMoniCtrlData.data.shortCycBelongTask;
    info.data.shortCycEvtThrLocation = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCSTATEVENTTHRESHOLDLOCATION
                                       ? statPortMoniCtrlTableList.data.shortCycEvtThrLocation : portMoniCtrlData.data.shortCycEvtThrLocation;
    info.data.shortCycSubTypeFiltLocation = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCSUBSTATTYPEFILTLOCATION
                                            ? statPortMoniCtrlTableList.data.shortCycSubTypeFiltLocation : portMoniCtrlData.data.shortCycSubTypeFiltLocation;
    info.data.shortCycHistAutoReport = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCHISTAUTOREPORT
                                       ? statPortMoniCtrlTableList.data.shortCycHistAutoReport : portMoniCtrlData.data.shortCycHistAutoReport;

    info.data.usrDefCycMoniEn = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCSTATMONIEN
                                ? statPortMoniCtrlTableList.data.usrDefCycMoniEn : portMoniCtrlData.data.usrDefCycMoniEn;
    info.data.usrDefCycBelongTask = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCBELONGTASK
                                    ? statPortMoniCtrlTableList.data.usrDefCycBelongTask : portMoniCtrlData.data.usrDefCycBelongTask;
    info.data.usrDefCycEvtThrLocation = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCSTATEVENTTHRESHOLDLOCATION
                                        ? statPortMoniCtrlTableList.data.usrDefCycEvtThrLocation : portMoniCtrlData.data.usrDefCycEvtThrLocation;
    info.data.usrDefCycSubTypeFiltLocation = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCSUBSTATTYPEFILTLOCATION
                                             ? statPortMoniCtrlTableList.data.usrDefCycSubTypeFiltLocation : portMoniCtrlData.data.usrDefCycSubTypeFiltLocation;
    info.data.usrDefCycHistAutoReport = statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCHISTAUTOREPORT
                                        ? statPortMoniCtrlTableList.data.usrDefCycHistAutoReport : portMoniCtrlData.data.usrDefCycHistAutoReport;

    if (STATIS_DEL_TABLE == mode)
    {
        opcode = IPC_OPCODE_DELETE;
    }
    else if ((STATIS_MODIFY_TABLE == mode) || (STATIS_ADD_TABLE == mode))
    {
        if (STATIS_ADD_TABLE == mode)
        {
            opcode = IPC_OPCODE_ADD;
        }
        else if (STATIS_MODIFY_TABLE == mode)
        {
            opcode = IPC_OPCODE_UPDATE;
        }

        if ((statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCSTATMONIEN
                || statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCBELONGTASK
                || statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCSTATEVENTTHRESHOLDLOCATION
                || statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCSUBSTATTYPEFILTLOCATION
                || statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCHISTAUTOREPORT))
        {
            flag_set |= PORT_MONI_PORT_LCYC_MON_OPT;
        }

        if ((statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCSTATMONIEN
                || statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCBELONGTASK
                || statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCSTATEVENTTHRESHOLDLOCATION
                || statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCSUBSTATTYPEFILTLOCATION
                || statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCHISTAUTOREPORT))
        {
            flag_set |= PORT_MONI_PORT_SCYC_MON_OPT;
        }

        if ((statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCSTATMONIEN
                || statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCBELONGTASK
                || statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCSTATEVENTTHRESHOLDLOCATION
                || statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCSUBSTATTYPEFILTLOCATION
                || statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCHISTAUTOREPORT))
        {
            flag_set |= PORT_MONI_PORT_UCYC_MON_OPT;
        }
    }
    else
    {
        return FALSE;
    }


    uchar buf[IPC_MSG_LEN];
    int buf_len = 0;
    memset(buf, 0, IPC_MSG_LEN);

    memcpy(buf, &mode, sizeof(int));
    buf_len += sizeof(int);

    memcpy(buf + buf_len, &flag_set, sizeof(int));
    buf_len += sizeof(int);

    memcpy(buf + buf_len, &info, sizeof(stPortMoniCtrlTab_info));
    buf_len += sizeof(stPortMoniCtrlTab_info);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : mode(%d|%d|%d|%d|%d), flag_set(%d)\n", __func__, __LINE__,
               mode, GPN_STAT_MSG_BASE_MON_OPT, GPN_STAT_MSG_PORT_CYC_MON_OPT,
               GPN_STAT_MSG_MON_ADD, GPN_STAT_MSG_MON_DELETE,
               flag_set);

    /*ipc_send_common(buf, buf_len, 1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                    SNMP_STAT_PORT_MONI_CTRL_TABLE, opcode);*/

	stat_mib_set(buf, buf_len, MODULE_ID_STAT, IPC_TYPE_SNMP, SNMP_STAT_PORT_MONI_CTRL_TABLE,
		opcode);

    return TRUE;
}


static int currStatReadMessage(stCurrStatTab_index index_input)
{
    struct ipc_mesg_n *pMsgRcv = NULL;

	stCurrStatTab_index index;
    index.currPortIndex1 = index_input.currPortIndex1;
    index.currPortIndex2 = index_input.currPortIndex2;
    index.currPortIndex3 = index_input.currPortIndex3;
    index.currPortIndex4 = index_input.currPortIndex4;
    index.currPortIndex5 = index_input.currPortIndex5;
    index.currDevIndex = index_input.currDevIndex;
    index.currDataType = index_input.currDataType;

    if ((currStatData.index.currPortIndex1 != index.currPortIndex1) ||
            (currStatData.index.currPortIndex2 != index.currPortIndex2) ||
            (currStatData.index.currPortIndex3 != index.currPortIndex3) ||
            (currStatData.index.currPortIndex4 != index.currPortIndex4) ||
            (currStatData.index.currPortIndex5 != index.currPortIndex5) ||
            (currStatData.index.currDevIndex != index.currDevIndex) ||
            (currStatData.index.currDataType != index.currDataType))
    {
        memset(&currStatData, 0, sizeof(stCurrStatTab_info));
    }

    if (!currStatData.data.rowStatus)
    {
        /*pinfo = (stCurrStatTab_info *)ipc_send_common_wait_reply(&index, sizeof(stCurrStatTab_index),
                                                                 1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                                                                 SNMP_CURR_STAT_TABLE, IPC_OPCODE_GET, 0);*/

		pMsgRcv = ipc_sync_send_n2(&index, sizeof(stCurrStatTab_index), 1, MODULE_ID_STAT,
			MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_CURR_STAT_TABLE, IPC_OPCODE_GET, 0, 1000);

		if (NULL == pMsgRcv)
		{
			return FALSE;
		}

        if (0 != pMsgRcv->msghdr.data_num)
        {
            memcpy(&currStatData, pMsgRcv->msg_data, sizeof(stCurrStatTab_info));
            currStatData.data.rowStatus = SNMP_ROW_ACTIVE;

			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return TRUE;
        }
        else
        {
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return FALSE;
        }
    }

    return TRUE;
}

static int currStatOperate(int mode, stCurrStatTab_index index)
{
    stCurrStatTab_info info;
    memset(&info, 0, sizeof(stCurrStatTab_info));
    info.index.currPortIndex1 = index.currPortIndex1;
    info.index.currPortIndex2 = index.currPortIndex2;
    info.index.currPortIndex3 = index.currPortIndex3;
    info.index.currPortIndex4 = index.currPortIndex4;
    info.index.currPortIndex5 = index.currPortIndex5;
    info.index.currDevIndex = index.currDevIndex;
    info.index.currDataType = index.currDataType;

    info.data.clearCurrData = statCurrStatTableList.flagsOfColumnSet & FLAG_CLEARCURRDATACLEAR
                              ? statCurrStatTableList.data.clearCurrData : currStatData.data.clearCurrData;


    uchar buf[IPC_MSG_LEN];
    int buf_len = 0;
    memset(buf, 0, IPC_MSG_LEN);

    memcpy(buf, &mode, sizeof(int));
    memcpy(buf + sizeof(int), &info, sizeof(stCurrStatTab_info));
    buf_len = sizeof(int) + sizeof(stCurrStatTab_info);

    /*ipc_send_common(buf, buf_len, 1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                    SNMP_CURR_STAT_TABLE, IPC_OPCODE_UPDATE);*/

	stat_mib_set(buf, buf_len, MODULE_ID_STAT, IPC_TYPE_SNMP, SNMP_CURR_STAT_TABLE,
		IPC_OPCODE_UPDATE);

    return TRUE;
}


static int statTaskReadMessage(stStatTaskTab_index index_input)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
	
    stStatTaskTab_index index;
    index.taskIndex = index_input.taskIndex;

    if (statTaskData.index.taskIndex != index.taskIndex)
    {
        memset(&statTaskData, 0, sizeof(stStatTaskTab_info));
    }

    if (!statTaskData.data.rowStatus)
    {
        /*pinfo = (stStatTaskTab_info *)ipc_send_common_wait_reply(&index, sizeof(stStatTaskTab_index),
                                                                 1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                                                                 SNMP_STAT_TASK_TABLE, IPC_OPCODE_GET, 0);*/

		pMsgRcv = ipc_sync_send_n2(&index, sizeof(stStatTaskTab_index), 1, MODULE_ID_STAT,
			MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_STAT_TASK_TABLE, IPC_OPCODE_GET, 0, 1000);

		if (NULL == pMsgRcv)
		{
			return FALSE;
		}

        if (0 != pMsgRcv->msghdr.data_num)
        {
            memcpy(&statTaskData, pMsgRcv->msg_data, sizeof(stStatTaskTab_info));
            statTaskData.data.rowStatus = SNMP_ROW_ACTIVE;

			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return TRUE;
        }
        else
        {
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return FALSE;
        }
    }

    return TRUE;
}

static int statTaskOperate(int mode, stStatTaskTab_index index)
{
    int opcode;
    stStatTaskTab_info info;
    memset(&info, 0, sizeof(stStatTaskTab_info));
    info.index.taskIndex = index.taskIndex;

    info.data.maxPortInTask = statStatTaskTableList.flagsOfColumnSet & FLAG_MAXPORTINTASK
                              ? statStatTaskTableList.data.maxPortInTask : statTaskData.data.maxPortInTask;
    info.data.taskScanType = statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKSCANTYPE
                             ? statStatTaskTableList.data.taskScanType : statTaskData.data.taskScanType;
    info.data.taskCycMoniType = statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKCYCMONITYPE
                                ? statStatTaskTableList.data.taskCycMoniType : statTaskData.data.taskCycMoniType;
    info.data.taskMoniCycSec = statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKMONICYCSECONDS
                               ? statStatTaskTableList.data.taskMoniCycSec : statTaskData.data.taskMoniCycSec;
    info.data.taskStartTime = statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKSTARTTIME
                              ? statStatTaskTableList.data.taskStartTime : statTaskData.data.taskStartTime;
    info.data.taskEndTime = statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKENDTIME
                            ? statStatTaskTableList.data.taskEndTime : statTaskData.data.taskEndTime;
    info.data.taskAlmThrTpLocation = statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKALMTHRESHOLDTPLOCATION
                                     ? statStatTaskTableList.data.taskAlmThrTpLocation : statTaskData.data.taskAlmThrTpLocation;
    info.data.taskSubReportLocation = statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKSUBREPORTLOCATION
                                      ? statStatTaskTableList.data.taskSubReportLocation : statTaskData.data.taskSubReportLocation;


    uchar buf[IPC_MSG_LEN];
    int buf_len = 0;
    memset(buf, 0, IPC_MSG_LEN);

    memcpy(buf, &mode, sizeof(int));
    memcpy(buf + sizeof(int), &info, sizeof(stStatTaskTab_info));
    buf_len = sizeof(int) + sizeof(stStatTaskTab_info);

    if (GPN_STAT_MSG_TASK_ADD == mode)
    {
        opcode = IPC_OPCODE_ADD;
    }
    else if (GPN_STAT_MSG_TASK_MODIFY == mode)
    {
        opcode = IPC_OPCODE_UPDATE;
    }
    else if (GPN_STAT_MSG_TASK_DELETE == mode)
    {
        opcode = IPC_OPCODE_DELETE;
    }
    else
    {
        return FALSE;
    }

    /*ipc_send_common(buf, buf_len, 1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                    SNMP_STAT_TASK_TABLE, opcode);*/

	stat_mib_set(buf, buf_len, MODULE_ID_STAT, IPC_TYPE_SNMP, SNMP_STAT_TASK_TABLE, opcode);

    return TRUE;
}


static int statEvtThrTmpReadMessage(stEvtThrTmpTab_index index_input)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
	
    stEvtThrTmpTab_index index;
    index.evtThrTmpIndex = index_input.evtThrTmpIndex;
    index.evtThrTmpTpSubType = index_input.evtThrTmpTpSubType;

    if ((evtThrTmpData.index.evtThrTmpIndex != index.evtThrTmpIndex) ||
            (evtThrTmpData.index.evtThrTmpTpSubType != index.evtThrTmpTpSubType))
    {
        memset(&evtThrTmpData, 0, sizeof(stEvtThrTmpTab_info));
    }

    if (!evtThrTmpData.data.rowStatus)
    {
        /*pinfo = (stEvtThrTmpTab_info *)ipc_send_common_wait_reply(&index, sizeof(stEvtThrTmpTab_index),
                                                                  1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                                                                  SNMP_STAT_EVENT_THRESHOLD_TEMPLAT_TABLE, IPC_OPCODE_GET, 0);*/

		pMsgRcv = ipc_sync_send_n2(&index, sizeof(stEvtThrTmpTab_index), 1, MODULE_ID_STAT,
			MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_STAT_EVENT_THRESHOLD_TEMPLAT_TABLE, IPC_OPCODE_GET, 0, 1000);

		if (NULL == pMsgRcv)
		{
			return FALSE;
		}

        if (0 != pMsgRcv->msghdr.data_num)
        {
            memcpy(&evtThrTmpData, pMsgRcv->msg_data, sizeof(stEvtThrTmpTab_info));
            evtThrTmpData.data.rowStatus    = SNMP_ROW_ACTIVE;

			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return TRUE;
        }
        else
        {
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return FALSE;
        }
    }

    return TRUE;
}

static int statEvtThrTmpOperate(int mode, stEvtThrTmpTab_index index)
{
    int opcode = 0;
    stEvtThrTmpTab_info info;
    memset(&info, 0, sizeof(stEvtThrTmpTab_info));
    info.index.evtThrTmpIndex = index.evtThrTmpIndex;
    info.index.evtThrTmpTpSubType = index.evtThrTmpTpSubType;

    info.data.evtThrScanType = statEvtThrTmpTableList.flagsOfColumnSet & FLAG_STATEVENTTHRESHOLDSCANTYPE
                               ? statEvtThrTmpTableList.data.evtThrScanType : evtThrTmpData.data.evtThrScanType;
    info.data.evtThrTpSubTypeUpThrHigh32 = statEvtThrTmpTableList.flagsOfColumnSet & FLAG_STATEVENTTHRESHOLDTPSUBTYPEUPTHRESHHIGH32
                                           ? statEvtThrTmpTableList.data.evtThrTpSubTypeUpThrHigh32 : evtThrTmpData.data.evtThrTpSubTypeUpThrHigh32;
    info.data.evtThrTpSubTypeUpThrLow32 = statEvtThrTmpTableList.flagsOfColumnSet & FLAG_STATEVENTTHRESHOLDTPSUBTYPEUPTHRESHLOW32
                                          ? statEvtThrTmpTableList.data.evtThrTpSubTypeUpThrLow32 : evtThrTmpData.data.evtThrTpSubTypeUpThrLow32;
    info.data.evtThrTpSubTypeLowThrHigh32 = statEvtThrTmpTableList.flagsOfColumnSet & FLAG_STATEVENTTHRESHOLDTPSUBTYPELOWTHRESHHIGH32
                                            ? statEvtThrTmpTableList.data.evtThrTpSubTypeLowThrHigh32 : evtThrTmpData.data.evtThrTpSubTypeLowThrHigh32;
    info.data.evtThrTpSubTypeLowThrLow32 = statEvtThrTmpTableList.flagsOfColumnSet & FLAG_STATEVENTTHRESHOLDTPSUBTYPELOWTHRESHLOW32
                                           ? statEvtThrTmpTableList.data.evtThrTpSubTypeLowThrLow32 : evtThrTmpData.data.evtThrTpSubTypeLowThrLow32;


    uchar buf[IPC_MSG_LEN];
    int buf_len = 0;
    memset(buf, 0, IPC_MSG_LEN);

    memcpy(buf, &mode, sizeof(int));
    memcpy(buf + sizeof(int), &info, sizeof(stEvtThrTmpTab_info));
    buf_len = sizeof(int) + sizeof(stEvtThrTmpTab_info);

    if (GPN_STAT_MSG_EVN_THRED_TEMP_ADD == mode)
    {
        opcode = IPC_OPCODE_ADD;
    }
    else if (GPN_STAT_MSG_EVN_THRED_TEMP_MODIFY == mode)
    {
        opcode = IPC_OPCODE_UPDATE;
    }
    else if (GPN_STAT_MSG_EVN_THRED_TEMP_DELETE == mode)
    {
        opcode = IPC_OPCODE_DELETE;
    }
    else
    {
        return FALSE;
    }

    /*ipc_send_common(buf, buf_len, 1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                    SNMP_STAT_EVENT_THRESHOLD_TEMPLAT_TABLE, opcode);*/

	stat_mib_set(buf, buf_len, MODULE_ID_STAT, IPC_TYPE_SNMP, SNMP_STAT_EVENT_THRESHOLD_TEMPLAT_TABLE,
		opcode);

    return TRUE;
}

static int statSubStatTypeFileTpTabReadMessage(stSubStatTypeFileTpTab_index index_input)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
	
    stSubStatTypeFileTpTab_index index;
    index.subTypeFiltIndex = index_input.subTypeFiltIndex;
    index.selectSubType = index_input.selectSubType;

    if ((subStatTypeFileTpData.index.subTypeFiltIndex != index.subTypeFiltIndex) ||
            (subStatTypeFileTpData.index.selectSubType != index.selectSubType))
    {
        memset(&subStatTypeFileTpData, 0, sizeof(stSubStatTypeFileTpTab_info));
    }

    if (!subStatTypeFileTpData.data.rowStatus)
    {
        /*pinfo = (stSubStatTypeFileTpTab_info *)ipc_send_common_wait_reply(&index, sizeof(stSubStatTypeFileTpTab_index),
                                                                          1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                                                                          SNMP_STAT_SUB_STAT_TYPE_FILT_TP_TABLE, IPC_OPCODE_GET, 0);*/

		pMsgRcv = ipc_sync_send_n2(&index, sizeof(stSubStatTypeFileTpTab_index), 1, MODULE_ID_STAT,
			MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_STAT_SUB_STAT_TYPE_FILT_TP_TABLE, IPC_OPCODE_GET, 0, 1000);

		if (NULL == pMsgRcv)
		{
			return FALSE;
		}

        if (0 != pMsgRcv->msghdr.data_num)
        {
            memcpy(&subStatTypeFileTpData, pMsgRcv->msg_data, sizeof(stSubStatTypeFileTpTab_info));
            subStatTypeFileTpData.data.rowStatus    = SNMP_ROW_ACTIVE;

			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return TRUE;
        }
        else
        {
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return FALSE;
        }
    }

    return TRUE;
}

static int statSubStatTypeFileTpTabOperate(int mode, stSubStatTypeFileTpTab_index index)
{
    int opcode = 0;
    stSubStatTypeFileTpTab_info info;
    memset(&info, 0, sizeof(stSubStatTypeFileTpTab_info));
    info.index.subTypeFiltIndex = index.subTypeFiltIndex;
    info.index.selectSubType = index.selectSubType;

    info.data.selectScanType = statSubStatTypeFileTpTableList.flagsOfColumnSet & FLAG_STATSELECTSTATSCANTYPE
                               ? statSubStatTypeFileTpTableList.data.selectScanType : subStatTypeFileTpData.data.selectScanType;
    info.data.selectStatus = statSubStatTypeFileTpTableList.flagsOfColumnSet & FLAG_STATSELECTSTATUS
                             ? statSubStatTypeFileTpTableList.data.selectStatus : subStatTypeFileTpData.data.selectStatus;


    uchar buf[IPC_MSG_LEN];
    int buf_len = 0;
    memset(buf, 0, IPC_MSG_LEN);

    memcpy(buf, &mode, sizeof(int));
    memcpy(buf + sizeof(int), &info, sizeof(stSubStatTypeFileTpTab_info));
    buf_len = sizeof(int) + sizeof(stSubStatTypeFileTpTab_info);

    if (GPN_STAT_MSG_SUBFILT_TEMP_ADD == mode)
    {
        opcode = IPC_OPCODE_ADD;
    }
    else if (GPN_STAT_MSG_SUBFILT_TEMP_MODIFY == mode)
    {
        opcode = IPC_OPCODE_UPDATE;
    }
    else if (GPN_STAT_MSG_SUBFILT_TEMP_DELETE == mode)
    {
        opcode = IPC_OPCODE_DELETE;
    }
    else
    {
        return FALSE;
    }

    /*ipc_send_common(buf, buf_len, 1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                    SNMP_STAT_SUB_STAT_TYPE_FILT_TP_TABLE, opcode);*/

	stat_mib_set(buf, buf_len, MODULE_ID_STAT, IPC_TYPE_SNMP, SNMP_STAT_SUB_STAT_TYPE_FILT_TP_TABLE,
		opcode);

    return TRUE;
}


static int statAlmThrTmpTabReadMessage(stAlmThrTmpTab_index index_input)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
	
    stAlmThrTmpTab_index index;
    index.almThrTmpIndex = index_input.almThrTmpIndex;
    index.almThrTmpTpSubType = index_input.almThrTmpTpSubType;

    if ((almThrTmpData.index.almThrTmpIndex != index.almThrTmpIndex) ||
            (almThrTmpData.index.almThrTmpTpSubType != index.almThrTmpTpSubType))
    {
        memset(&almThrTmpData, 0, sizeof(stAlmThrTmpTab_info));
    }

    if (!almThrTmpData.data.rowStatus)
    {
        /*pinfo = (stAlmThrTmpTab_info *)ipc_send_common_wait_reply(&index, sizeof(stAlmThrTmpTab_index),
                                                                  1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                                                                  SNMP_STAT_ALARM_THRESHOLD_TEMPLAT_TABLE, IPC_OPCODE_GET, 0);*/

		pMsgRcv = ipc_sync_send_n2(&index, sizeof(stAlmThrTmpTab_index), 1, MODULE_ID_STAT,
			MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_STAT_ALARM_THRESHOLD_TEMPLAT_TABLE, IPC_OPCODE_GET, 0, 1000);

		if (NULL == pMsgRcv)
		{
			return FALSE;
		}

        if (0 != pMsgRcv->msghdr.data_num)
        {
            memcpy(&almThrTmpData, pMsgRcv->msg_data, sizeof(stAlmThrTmpTab_info));
            almThrTmpData.data.rowStatus    = SNMP_ROW_ACTIVE;

			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return TRUE;
        }
        else
        {
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return FALSE;
        }
    }

    return TRUE;
}

static int statAlmThrTmpTabOperate(int mode, stAlmThrTmpTab_index index)
{
    int opcode = 0;
    stAlmThrTmpTab_info info;
    memset(&info, 0, sizeof(stAlmThrTmpTab_info));
    info.index.almThrTmpIndex = index.almThrTmpIndex;
    info.index.almThrTmpTpSubType = index.almThrTmpTpSubType;

    info.data.almThrScanType = statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSCANTYPE
                               ? statAlmThrTmpTableList.data.almThrScanType : almThrTmpData.data.almThrScanType;
    info.data.almThrTpSubTypeUpRiseHigh32 = statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEUPRISEHIGH32
                                            ? statAlmThrTmpTableList.data.almThrTpSubTypeUpRiseHigh32 : almThrTmpData.data.almThrTpSubTypeUpRiseHigh32;
    info.data.almThrTpSubTypeUpRiseLow32 = statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEUPRISELOW32
                                           ? statAlmThrTmpTableList.data.almThrTpSubTypeUpRiseLow32 : almThrTmpData.data.almThrTpSubTypeUpRiseLow32;
    info.data.almThrTpSubTypeUpClrHigh32 = statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEUPCLEARHIGH32
                                           ? statAlmThrTmpTableList.data.almThrTpSubTypeUpClrHigh32 : almThrTmpData.data.almThrTpSubTypeUpClrHigh32;
    info.data.almThrTpSubTypeUpClrLow32 = statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEUPCLEARLOW32
                                          ? statAlmThrTmpTableList.data.almThrTpSubTypeUpClrLow32 : almThrTmpData.data.almThrTpSubTypeUpClrLow32;
    info.data.almThrTpSubTypeDnRiseHigh32 = statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEDNRISEHIGH32
                                            ? statAlmThrTmpTableList.data.almThrTpSubTypeDnRiseHigh32 : almThrTmpData.data.almThrTpSubTypeDnRiseHigh32;
    info.data.almThrTpSubTypeDnRiseLow32 = statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEDNRISELOW32
                                           ? statAlmThrTmpTableList.data.almThrTpSubTypeDnRiseLow32 : almThrTmpData.data.almThrTpSubTypeDnRiseLow32;
    info.data.almThrTpSubTypeDnClrHigh32 = statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEDNCLEARHIGH32
                                           ? statAlmThrTmpTableList.data.almThrTpSubTypeDnClrHigh32 : almThrTmpData.data.almThrTpSubTypeDnClrHigh32;
    info.data.almThrTpSubTypeDnClrLow32 = statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEDNCLEARLOW32
                                          ? statAlmThrTmpTableList.data.almThrTpSubTypeDnClrLow32 : almThrTmpData.data.almThrTpSubTypeDnClrLow32;


    uchar buf[IPC_MSG_LEN];
    int buf_len = 0;
    memset(buf, 0, IPC_MSG_LEN);

    memcpy(buf, &mode, sizeof(int));
    memcpy(buf + sizeof(int), &info, sizeof(stAlmThrTmpTab_info));
    buf_len = sizeof(int) + sizeof(stAlmThrTmpTab_info);

    if (GPN_STAT_MSG_ALM_THRED_TEMP_ADD == mode)
    {
        opcode = IPC_OPCODE_ADD;
    }
    else if (GPN_STAT_MSG_ALM_THRED_TEMP_MODIFY == mode)
    {
        opcode = IPC_OPCODE_UPDATE;
    }
    else if (GPN_STAT_MSG_ALM_THRED_TEMP_DELETE == mode)
    {
        opcode = IPC_OPCODE_DELETE;
    }
    else
    {
        return FALSE;
    }

    /*ipc_send_common(buf, buf_len, 1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                    SNMP_STAT_ALARM_THRESHOLD_TEMPLAT_TABLE, opcode);*/

	stat_mib_set(buf, buf_len, MODULE_ID_STAT, IPC_TYPE_SNMP, SNMP_STAT_ALARM_THRESHOLD_TEMPLAT_TABLE,
		opcode);

    return TRUE;
}


static int statSubTypeReportTmpTabReadMessage(stSubTypeReportTmpTab_index index_input)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
	
    stSubTypeReportTmpTab_index index;
    index.subReportTmpIndex = index_input.subReportTmpIndex;
    index.subReportSubType = index_input.subReportSubType;

    if ((subTypeReportTmpData.index.subReportTmpIndex != index.subReportTmpIndex) ||
            (subTypeReportTmpData.index.subReportSubType != index.subReportSubType))
    {
        memset(&subTypeReportTmpData, 0, sizeof(stSubTypeReportTmpTab_info));
    }

    if (!subTypeReportTmpData.data.rowStatus)
    {
        /*pinfo = (stSubTypeReportTmpTab_info *)ipc_send_common_wait_reply(&index, sizeof(stSubTypeReportTmpTab_index),
                                                                         1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                                                                         SNMP_STAT_SUB_STAT_TYPE_REPORT_TEMPLAT_TABLE, IPC_OPCODE_GET, 0);*/

		pMsgRcv = ipc_sync_send_n2(&index, sizeof(stSubTypeReportTmpTab_index), 1, MODULE_ID_STAT,
			MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_STAT_SUB_STAT_TYPE_REPORT_TEMPLAT_TABLE, IPC_OPCODE_GET, 0, 1000);

		if (NULL == pMsgRcv)
		{
			return FALSE;
		}

        if (0 != pMsgRcv->msghdr.data_num)
        {
            memcpy(&subTypeReportTmpData, pMsgRcv->msg_data, sizeof(stSubTypeReportTmpTab_info));
            subTypeReportTmpData.data.rowStatus = SNMP_ROW_ACTIVE;

			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return TRUE;
        }
        else
        {
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return FALSE;
        }
    }

    return TRUE;
}

static int statSubTypeReportTmpTabOperate(int mode, stSubTypeReportTmpTab_index index)
{
    int opcode = 0;
    stSubTypeReportTmpTab_info info;
    memset(&info, 0, sizeof(stSubTypeReportTmpTab_info));
    info.index.subReportTmpIndex = index.subReportTmpIndex;
    info.index.subReportSubType = index.subReportSubType;

    info.data.subReportScanType = statSubTypeReportTmpTableList.flagsOfColumnSet & FLAG_STATSUBREPORTSCANTYPE
                                  ? statSubTypeReportTmpTableList.data.subReportScanType : subTypeReportTmpData.data.subReportScanType;
    info.data.subReportStatus = statSubTypeReportTmpTableList.flagsOfColumnSet & FLAG_STATSUBREPORTSTATUS
                                ? statSubTypeReportTmpTableList.data.subReportStatus : subTypeReportTmpData.data.subReportStatus;


    uchar buf[IPC_MSG_LEN];
    int buf_len = 0;
    memset(buf, 0, IPC_MSG_LEN);

    memcpy(buf, &mode, sizeof(int));
    memcpy(buf + sizeof(int), &info, sizeof(stSubTypeReportTmpTab_info));
    buf_len = sizeof(int) + sizeof(stSubTypeReportTmpTab_info);

    if (GPN_STAT_MSG_SUBREPORT_TEMP_ADD == mode)
    {
        opcode = IPC_OPCODE_ADD;
    }
    else if (GPN_STAT_MSG_SUBREPORT_TEMP_MODIFY == mode)
    {
        opcode = IPC_OPCODE_UPDATE;
    }
    else if (GPN_STAT_MSG_SUBREPORT_TEMP_DELETE == mode)
    {
        opcode = IPC_OPCODE_DELETE;
    }
    else
    {
        return FALSE;
    }

    /*ipc_send_common(buf, buf_len, 1, MODULE_ID_STAT, MODULE_ID_SNMPD, IPC_TYPE_SNMP,
                    SNMP_STAT_SUB_STAT_TYPE_REPORT_TEMPLAT_TABLE, opcode);*/

	stat_mib_set(buf, buf_len, MODULE_ID_STAT, IPC_TYPE_SNMP, SNMP_STAT_SUB_STAT_TYPE_REPORT_TEMPLAT_TABLE,
		opcode);

    return TRUE;
}



/**************************************** write statGlobalSubType table *********************************************/

static int statSubTypeRowStatus_get(stGloSubTypeAttribTab_index index, int *data)
{
    u_int32_t ret;

    ret = statGlobalSubTypeReadMessage(index);

    if (ret == FALSE)
    {
        if ((statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_STATSUBTYPEROWSTATUS)
                && (statGlobalSubTypeAtrribTableList.index.subTypeIndex == index.subTypeIndex))
        {
            if ((statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_LONGCYCUPTHRESHOLDHIGH32) &&
                    (statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_LONGCYCUPTHRESHOLDLOW32) &&
                    (statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_LONGCYCLOWTHRESHOLDHIGH32) &&
                    (statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_LONGCYCLOWTHRESHOLDLOW32) &&
                    (statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_SHORTCYCUPTHRESHOLDHIHG32) &&
                    (statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_SHORTCYCUPTHRESHOLDLOW32) &&
                    (statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_SHORTCYCLOWTHRESHOLDHIGH32) &&
                    (statGlobalSubTypeAtrribTableList.flagsOfColumnSet & FLAG_SHORTCYCLOWTHRESHOLDLOW32))
            {
                *data =  SNMP_ROW_NOTINSERVICE;
            }
            else
            {
                *data =  SNMP_ROW_NOTREADY;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        *data = SNMP_ROW_ACTIVE;
    }

    return TRUE;
}

static int statSubTypeRowStatus_set(stGloSubTypeAttribTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSubTypeRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        if (SNMP_ROW_CREATEANDWAIT == data)
        {
            statGlobalSubTypeAtrribTableList.flagsOfColumnSet = FLAG_STATSUBTYPEROWSTATUS;
            //assign the index here
            statGlobalSubTypeAtrribTableList.index.subTypeIndex = index.subTypeIndex;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if (SNMP_ROW_ACTIVE == data)
        {
            //if (statGlobalSubTypeAtrribTableList.data.statSubTypeRowStatus == SNMP_ROW_NOTINSERVICE)
            if (subTypeStat.flag == MODIFY_TABLE_FLAG)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : modify subTypeReport table\n\r", __func__, __LINE__);
                ret = statGlobalSubTypeOperate(GPN_STAT_MSG_SUB_STAT_TYPE_MODIFY, index);

                //zlog_debug ("%s[%d] : modify the table\n\r", __func__, __LINE__);
                if (ret == FALSE)
                {
                    return FALSE;
                }

                memset(&subTypeStat, 0, sizeof(subTypeAtrribStatus));
                memset(&subTypeData, 0, sizeof(stGloSubTypeAttribTab_info));
                memset(&statGlobalSubTypeAtrribTableList, 0, sizeof(stGloSubTypeAttribTab_info));
            }
            else if (SNMP_ROW_NOTINSERVICE == row_status)
            {
                //fill your code here
                /*ret = statGlobalSubTypeOperate(GPN_STAT_MSG_SUB_STAT_TYPE_ADD, index);
                if (ret == FALSE)
                {
                    return FALSE;
                }

                memset(&subTypeStat, 0, sizeof(subTypeAtrribStatus));
                memset(&statGlobalSubTypeAtrribTableList, 0, sizeof(stGloSubTypeAttribTab_info));*/
            }
            else
            {
                return FALSE;
            }
        }
        else if (SNMP_ROW_DESTROY == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //fill your code here
                /*ret = statGlobalSubTypeOperate(GPN_STAT_MSG_SUB_STAT_TYPE_DELETE, index);
                if (ret == FALSE)
                {
                    return FALSE;
                }
                subType_index = 0;
                memset(&subTypeStat, 0, sizeof(subTypeAtrribStatus));*/
                memset(&subTypeStat, 0, sizeof(subTypeAtrribStatus));
                memset(&subTypeData, 0, sizeof(stGloSubTypeAttribTab_info));
                memset(&statGlobalSubTypeAtrribTableList, 0, sizeof(stGloSubTypeAttribTab_info));
                return FALSE;
            }
            else
            {
                memset(&subTypeStat, 0, sizeof(subTypeAtrribStatus));
                memset(&subTypeData, 0, sizeof(stGloSubTypeAttribTab_info));
                memset(&statGlobalSubTypeAtrribTableList, 0, sizeof(stGloSubTypeAttribTab_info));
            }
        }
        else if (SNMP_ROW_NOTINSERVICE == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //statGlobalSubTypeAtrribTableList.data.statSubTypeRowStatus = SNMP_ROW_NOTINSERVICE;
                subTypeStat.flag = MODIFY_TABLE_FLAG;
                statGlobalSubTypeAtrribTableList.flagsOfColumnSet = FLAG_STATSUBTYPEROWSTATUS;
            }
            else
            {
                memset(&subTypeStat, 0, sizeof(subTypeAtrribStatus));
                memset(&subTypeData, 0, sizeof(stGloSubTypeAttribTab_info));
                memset(&statGlobalSubTypeAtrribTableList, 0, sizeof(stGloSubTypeAttribTab_info));
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }

    }

    return TRUE;
}


static int longCycUpTdHigh32_set(stGloSubTypeAttribTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSubTypeRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (subTypeStat.flag == MODIFY_TABLE_FLAG)
            {
                statGlobalSubTypeAtrribTableList.data.longCycUpThrHigh32 = data;
                statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_LONGCYCUPTHRESHOLDHIGH32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statGlobalSubTypeAtrribTableList.data.longCycUpThrHigh32 = data;
            statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_LONGCYCUPTHRESHOLDHIGH32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int longCycUpTdLow32_set(stGloSubTypeAttribTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSubTypeRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (subTypeStat.flag == MODIFY_TABLE_FLAG)
            {
                statGlobalSubTypeAtrribTableList.data.longCycUpThrLow32 = data;
                statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_LONGCYCUPTHRESHOLDLOW32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statGlobalSubTypeAtrribTableList.data.longCycUpThrLow32 = data;
            statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_LONGCYCUPTHRESHOLDLOW32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

static int longCycLowTdHigh32_set(stGloSubTypeAttribTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSubTypeRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (subTypeStat.flag == MODIFY_TABLE_FLAG)
            {
                statGlobalSubTypeAtrribTableList.data.longCycLowThrHigh32 = data;
                statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_LONGCYCLOWTHRESHOLDHIGH32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statGlobalSubTypeAtrribTableList.data.longCycLowThrHigh32 = data;
            statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_LONGCYCLOWTHRESHOLDHIGH32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

static int longCycLowTdLow32_set(stGloSubTypeAttribTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSubTypeRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (subTypeStat.flag == MODIFY_TABLE_FLAG)
            {
                statGlobalSubTypeAtrribTableList.data.longCycLowThrLow32 = data;
                statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_LONGCYCLOWTHRESHOLDLOW32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statGlobalSubTypeAtrribTableList.data.longCycLowThrLow32 = data;
            statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_LONGCYCLOWTHRESHOLDLOW32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

static int shortCycUpTdHigh32_set(stGloSubTypeAttribTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSubTypeRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (subTypeStat.flag == MODIFY_TABLE_FLAG)
            {
                statGlobalSubTypeAtrribTableList.data.shortCycUpThrHigh32 = data;
                statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_SHORTCYCUPTHRESHOLDHIHG32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statGlobalSubTypeAtrribTableList.data.shortCycUpThrHigh32 = data;
            statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_SHORTCYCUPTHRESHOLDHIHG32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

static int shortCycUpTdLow32_set(stGloSubTypeAttribTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSubTypeRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (subTypeStat.flag == MODIFY_TABLE_FLAG)
            {
                statGlobalSubTypeAtrribTableList.data.shortCycUpThrLow32 = data;
                statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_SHORTCYCUPTHRESHOLDLOW32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statGlobalSubTypeAtrribTableList.data.shortCycUpThrLow32 = data;
            statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_SHORTCYCUPTHRESHOLDLOW32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

static int shortCycLowTdHigh32_set(stGloSubTypeAttribTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSubTypeRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (subTypeStat.flag == MODIFY_TABLE_FLAG)
            {
                statGlobalSubTypeAtrribTableList.data.shortCycLowThrHigh32 = data;
                statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_SHORTCYCLOWTHRESHOLDHIGH32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statGlobalSubTypeAtrribTableList.data.shortCycLowThrHigh32 = data;
            statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_SHORTCYCLOWTHRESHOLDHIGH32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

static int shortCycLowTdLow32_set(stGloSubTypeAttribTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSubTypeRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (subTypeStat.flag == MODIFY_TABLE_FLAG)
            {
                statGlobalSubTypeAtrribTableList.data.shortCycLowThrLow32 = data;
                statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_SHORTCYCLOWTHRESHOLDLOW32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            statGlobalSubTypeAtrribTableList.data.shortCycLowThrLow32 = data;
            statGlobalSubTypeAtrribTableList.flagsOfColumnSet |= FLAG_SHORTCYCLOWTHRESHOLDLOW32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}







int write_longCycUpTdHigh32(int action, u_char *var_val,
                            u_char var_val_type, size_t var_val_len,
                            u_char *statP, oid *name, size_t length)
{
    stGloSubTypeAttribTab_index index;
    //int ret = 0;
    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subTypeIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (longCycUpTdHigh32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_longCycUpTdLow32(int action, u_char *var_val,
                           u_char var_val_type, size_t var_val_len,
                           u_char *statP, oid *name, size_t length)
{
    stGloSubTypeAttribTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subTypeIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (longCycUpTdLow32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_longCycLowTdHigh32(int action, u_char *var_val,
                             u_char var_val_type, size_t var_val_len,
                             u_char *statP, oid *name, size_t length)
{
    stGloSubTypeAttribTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subTypeIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (longCycLowTdHigh32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_longCycLowTdLow32(int action, u_char *var_val,
                            u_char var_val_type, size_t var_val_len,
                            u_char *statP, oid *name, size_t length)
{
    stGloSubTypeAttribTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subTypeIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (longCycLowTdLow32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}


int write_shortCycUpTdHigh32(int action, u_char *var_val,
                             u_char var_val_type, size_t var_val_len,
                             u_char *statP, oid *name, size_t length)
{
    stGloSubTypeAttribTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subTypeIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (shortCycUpTdHigh32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_shortCycUpTdLow32(int action, u_char *var_val,
                            u_char var_val_type, size_t var_val_len,
                            u_char *statP, oid *name, size_t length)
{
    stGloSubTypeAttribTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subTypeIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (shortCycUpTdLow32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_shortCycLowTdHigh32(int action, u_char *var_val,
                              u_char var_val_type, size_t var_val_len,
                              u_char *statP, oid *name, size_t length)
{
    stGloSubTypeAttribTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subTypeIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (shortCycLowTdHigh32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_shortCycLowTdLow32(int action, u_char *var_val,
                             u_char var_val_type, size_t var_val_len,
                             u_char *statP, oid *name, size_t length)
{
    stGloSubTypeAttribTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subTypeIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (shortCycLowTdLow32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}


int write_statSubTypeRowStatus(int action, u_char *var_val,
                               u_char var_val_type, size_t var_val_len,
                               u_char *statP, oid *name, size_t length)
{
    stGloSubTypeAttribTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subTypeIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (statSubTypeRowStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}


/**************************************** write statGlobalSubType table end *********************************************/

/**************************************** write statGlobalScanType table *********************************************/

int write_statPortTypeToScanTypeRowStatus(int action, u_char *var_val,
                                          u_char var_val_type, size_t var_val_len,
                                          u_char *statP, oid *name, size_t length)
{
    stPortTypeToScanTypeTab_info info;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    info.index.portTypeIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
#if 0

    switch (action)
    {
        case RESERVE1:
            if (statPortTypeToScanTypeRowStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

#endif

    return SNMP_ERR_NOERROR;
}


/**************************************** write statPortMoniCtrl table *********************************************/

static int statPortMoniCtrlRowStatus_get(stPortMoniCtrlTab_index index, int *data)
{
    u_int32_t ret;

    ret = statPortMoniCtrlReadMessage(index);

    if (ret == FALSE)
    {
        if ((statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_STATPORTMONIROWSTATUS) &&
                ((statPortMoniCtrlTableList.index.portMoniIndex1 == index.portMoniIndex1) &&
                 (statPortMoniCtrlTableList.index.portMoniIndex2 == index.portMoniIndex2) &&
                 (statPortMoniCtrlTableList.index.portMoniIndex3 == index.portMoniIndex3) &&
                 (statPortMoniCtrlTableList.index.portMoniIndex4 == index.portMoniIndex4) &&
                 (statPortMoniCtrlTableList.index.portMoniIndex5 == index.portMoniIndex5) &&
                 (statPortMoniCtrlTableList.index.portMoniDevIndex == index.portMoniDevIndex) &&
                 (statPortMoniCtrlTableList.index.portMoniScanType == index.portMoniScanType)))
        {
            if (((statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_STATMONIEN) &&
                    (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_CURRSTATMONIEN)) &&
                    (((statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCSTATMONIEN) &&
                      (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCBELONGTASK) &&
                      (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCSTATEVENTTHRESHOLDLOCATION) &&
                      (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCSUBSTATTYPEFILTLOCATION) &&
                      (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_LONGCYCHISTAUTOREPORT)) ||
                     ((statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCSTATMONIEN) &&
                      (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCBELONGTASK) &&
                      (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCSTATEVENTTHRESHOLDLOCATION) &&
                      (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCSUBSTATTYPEFILTLOCATION) &&
                      (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_SHORTCYCHISTAUTOREPORT)) ||
                     ((statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCSTATMONIEN) &&
                      (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCBELONGTASK) &&
                      (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCSTATEVENTTHRESHOLDLOCATION) &&
                      (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCSUBSTATTYPEFILTLOCATION) &&
                      (statPortMoniCtrlTableList.flagsOfColumnSet & FLAG_USERDEFCYCHISTAUTOREPORT))))
            {
                *data =  SNMP_ROW_NOTINSERVICE;
            }
            else
            {
                *data =  SNMP_ROW_NOTREADY;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        *data = SNMP_ROW_ACTIVE;
    }

    return TRUE;
}

static int statPortMoniCtrlRowStatus_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);
    zlog_debug(SNMP_DBG_MIB_GET, "%s : stat alarm status(%d) return reslut(%d), data(%d)\n\r", \
               __FUNCTION__, row_status, ret, data);

    if (ret == FALSE)
    {
        if (SNMP_ROW_CREATEANDWAIT == data)
        {
            statPortMoniCtrlTableList.flagsOfColumnSet = FLAG_STATPORTMONIROWSTATUS;
            //assign the index here
            statPortMoniCtrlTableList.index.portMoniIndex1 = index.portMoniIndex1;
            statPortMoniCtrlTableList.index.portMoniIndex2 = index.portMoniIndex2;
            statPortMoniCtrlTableList.index.portMoniIndex3 = index.portMoniIndex3;
            statPortMoniCtrlTableList.index.portMoniIndex4 = index.portMoniIndex4;
            statPortMoniCtrlTableList.index.portMoniIndex5 = index.portMoniIndex5;
            statPortMoniCtrlTableList.index.portMoniDevIndex = index.portMoniDevIndex;
            statPortMoniCtrlTableList.index.portMoniScanType = index.portMoniScanType;
        }
        else if (SNMP_ROW_NOTINSERVICE == data)
        {
            portMoniStat.flag = MODIFY_TABLE_FLAG;
            statPortMoniCtrlTableList.flagsOfColumnSet = FLAG_STATPORTMONIROWSTATUS;
            statPortMoniCtrlTableList.index.portMoniIndex1   = index.portMoniIndex1;
            statPortMoniCtrlTableList.index.portMoniIndex2   = index.portMoniIndex2;
            statPortMoniCtrlTableList.index.portMoniIndex3   = index.portMoniIndex3;
            statPortMoniCtrlTableList.index.portMoniIndex4   = index.portMoniIndex4;
            statPortMoniCtrlTableList.index.portMoniIndex5   = index.portMoniIndex5;
            statPortMoniCtrlTableList.index.portMoniDevIndex = index.portMoniDevIndex;
            statPortMoniCtrlTableList.index.portMoniScanType = index.portMoniScanType;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if (SNMP_ROW_ACTIVE == data)
        {
            //if (statGlobalSubTypeAtrribTableList.data.statSubTypeRowStatus == SNMP_ROW_NOTINSERVICE)
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : modify portMonitor table\n", __func__, __LINE__);
                ret = statPortMoniCtrlOperate(STATIS_MODIFY_TABLE, index);

                if (ret == FALSE)
                {
                    memset(&portMoniStat, 0, sizeof(portMoniCtrlStatus));
                    memset(&portMoniCtrlData, 0, sizeof(stPortMoniCtrlTab_info));
                    memset(&statPortMoniCtrlTableList, 0, sizeof(stPortMoniCtrlTab_info));
                    return FALSE;
                }

                memset(&portMoniStat, 0, sizeof(portMoniCtrlStatus));
                memset(&portMoniCtrlData, 0, sizeof(stPortMoniCtrlTab_info));
                memset(&statPortMoniCtrlTableList, 0, sizeof(stPortMoniCtrlTab_info));

                if (ret == FALSE)
                {
                    return FALSE;
                }
            }
            else if (SNMP_ROW_NOTINSERVICE == row_status)
            {
                //fill your code here
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : add portMonitor table\n", __func__, __LINE__);
                ret = statPortMoniCtrlOperate(STATIS_ADD_TABLE, index);
                zlog_debug(SNMP_DBG_MIB_GET, "%s : add statPortMoniCtrlTable=>%d:%d:%d:%d:%d:%d result(%d)\n\r", __FUNCTION__, \
                           index.portMoniIndex1, index.portMoniIndex2, index.portMoniIndex3, index.portMoniIndex4, \
                           index.portMoniIndex5, index.portMoniDevIndex, ret);

                memset(&portMoniStat, 0, sizeof(portMoniCtrlStatus));
                memset(&portMoniCtrlData, 0, sizeof(stPortMoniCtrlTab_info));
                memset(&statPortMoniCtrlTableList, 0, sizeof(stPortMoniCtrlTab_info));

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : add statPortMoniCtrlTable failure\n\r", __FUNCTION__);
                    return FALSE;
                }
            }
            else
            {
                return FALSE;
            }
        }
        else if (SNMP_ROW_DESTROY == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //fill your code here
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : delete portMonitor table\n", __func__, __LINE__);
                ret = statPortMoniCtrlOperate(STATIS_DEL_TABLE, index);

                memset(&portMoniStat, 0, sizeof(portMoniCtrlStatus));
                memset(&portMoniCtrlData, 0, sizeof(stPortMoniCtrlTab_info));
                memset(&statPortMoniCtrlTableList, 0, sizeof(stPortMoniCtrlTab_info));

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "\n\r%s : delete statPortMoniCtrlTable failure\n\r", __FUNCTION__);
                    return FALSE;
                }
            }
            else
            {
                memset(&portMoniStat, 0, sizeof(portMoniCtrlStatus));
                memset(&portMoniCtrlData, 0, sizeof(stPortMoniCtrlTab_info));
                memset(&statPortMoniCtrlTableList, 0, sizeof(stPortMoniCtrlTab_info));
            }
        }
        else if (SNMP_ROW_NOTINSERVICE == data)
        {
            portMoniStat.flag = MODIFY_TABLE_FLAG;

            statPortMoniCtrlTableList.flagsOfColumnSet = FLAG_STATPORTMONIROWSTATUS;
            statPortMoniCtrlTableList.index.portMoniIndex1   = index.portMoniIndex1;
            statPortMoniCtrlTableList.index.portMoniIndex2   = index.portMoniIndex2;
            statPortMoniCtrlTableList.index.portMoniIndex3   = index.portMoniIndex3;
            statPortMoniCtrlTableList.index.portMoniIndex4   = index.portMoniIndex4;
            statPortMoniCtrlTableList.index.portMoniIndex5   = index.portMoniIndex5;
            statPortMoniCtrlTableList.index.portMoniDevIndex = index.portMoniDevIndex;
            statPortMoniCtrlTableList.index.portMoniScanType = index.portMoniScanType;
        }
        else
        {
            return FALSE;
        }

    }

    return TRUE;
}


static int moniEn_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.moniEn = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_STATMONIEN;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.moniEn = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_STATMONIEN;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}


static int currMoniEn_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.currMoniEn = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_CURRSTATMONIEN;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.currMoniEn = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_CURRSTATMONIEN;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int longCycMoniEn_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.longCycMoniEn = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_LONGCYCSTATMONIEN;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.longCycMoniEn = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_LONGCYCSTATMONIEN;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int longCycBelongTask_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.longCycBelongTask = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_LONGCYCBELONGTASK;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.longCycBelongTask = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_LONGCYCBELONGTASK;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int longCycEvtThrLocation_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.longCycEvtThrLocation = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_LONGCYCSTATEVENTTHRESHOLDLOCATION;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.longCycEvtThrLocation = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_LONGCYCSTATEVENTTHRESHOLDLOCATION;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int longCycSubTypeFiltLocation_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.longCycSubTypeFiltLocation = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_LONGCYCSUBSTATTYPEFILTLOCATION;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.longCycSubTypeFiltLocation = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_LONGCYCSUBSTATTYPEFILTLOCATION;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int longCycHistAutoReport_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.longCycHistAutoReport = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_LONGCYCHISTAUTOREPORT;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.longCycHistAutoReport = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_LONGCYCHISTAUTOREPORT;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int shortCycMoniEn_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.shortCycMoniEn = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_SHORTCYCSTATMONIEN;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.shortCycMoniEn = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_SHORTCYCSTATMONIEN;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int shortCycBelongTask_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.shortCycBelongTask = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_SHORTCYCBELONGTASK;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.shortCycBelongTask = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_SHORTCYCBELONGTASK;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int shortCycEvtThrLocation_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.shortCycEvtThrLocation = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_SHORTCYCSTATEVENTTHRESHOLDLOCATION;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.shortCycEvtThrLocation = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_SHORTCYCSTATEVENTTHRESHOLDLOCATION;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int shortCycSubTypeFiltLocation_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.shortCycSubTypeFiltLocation = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_SHORTCYCSUBSTATTYPEFILTLOCATION;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.shortCycSubTypeFiltLocation = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_SHORTCYCSUBSTATTYPEFILTLOCATION;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int shortCycHistAutoReport_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.shortCycHistAutoReport = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_SHORTCYCHISTAUTOREPORT;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.shortCycHistAutoReport = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_SHORTCYCHISTAUTOREPORT;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int usrDefCycMoniEn_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.usrDefCycMoniEn = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_USERDEFCYCSTATMONIEN;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.usrDefCycMoniEn = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_USERDEFCYCSTATMONIEN;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int usrDefCycBelongTask_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.usrDefCycBelongTask = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_USERDEFCYCBELONGTASK;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.usrDefCycBelongTask = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_USERDEFCYCBELONGTASK;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int usrDefCycEvtThrLocation_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.usrDefCycEvtThrLocation = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_USERDEFCYCSTATEVENTTHRESHOLDLOCATION;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.usrDefCycEvtThrLocation = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_USERDEFCYCSTATEVENTTHRESHOLDLOCATION;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int usrDefCycSubTypeFiltLocation_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.usrDefCycSubTypeFiltLocation = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_USERDEFCYCSUBSTATTYPEFILTLOCATION;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.usrDefCycSubTypeFiltLocation = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_USERDEFCYCSUBSTATTYPEFILTLOCATION;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int usrDefCycHistAutoReport_set(stPortMoniCtrlTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statPortMoniCtrlRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (portMoniStat.flag == MODIFY_TABLE_FLAG)
            {
                statPortMoniCtrlTableList.data.usrDefCycHistAutoReport = data;
                statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_USERDEFCYCHISTAUTOREPORT;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statPortMoniCtrlTableList.data.usrDefCycHistAutoReport = data;
            statPortMoniCtrlTableList.flagsOfColumnSet |= FLAG_USERDEFCYCHISTAUTOREPORT;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}



int write_moniEn(int action, u_char *var_val,
                 u_char var_val_type, size_t var_val_len,
                 u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (moniEn_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_currMoniEn(int action, u_char *var_val,
                     u_char var_val_type, size_t var_val_len,
                     u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (currMoniEn_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_longCycMoniEn(int action, u_char *var_val,
                        u_char var_val_type, size_t var_val_len,
                        u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (longCycMoniEn_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_longCycBelongTask(int action, u_char *var_val,
                            u_char var_val_type, size_t var_val_len,
                            u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (longCycBelongTask_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_longCycEvtThrLocation(int action, u_char *var_val,
                                u_char var_val_type, size_t var_val_len,
                                u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (longCycEvtThrLocation_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_longCycSubTypeFiltLocation(int action, u_char *var_val,
                                     u_char var_val_type, size_t var_val_len,
                                     u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (longCycSubTypeFiltLocation_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_longCycHistAutoReport(int action, u_char *var_val,
                                u_char var_val_type, size_t var_val_len,
                                u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (longCycHistAutoReport_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_shortCycMoniEn(int action, u_char *var_val,
                         u_char var_val_type, size_t var_val_len,
                         u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (shortCycMoniEn_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_shortCycBelongTask(int action, u_char *var_val,
                             u_char var_val_type, size_t var_val_len,
                             u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (shortCycBelongTask_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_shortCycEvtThrLocation(int action, u_char *var_val,
                                 u_char var_val_type, size_t var_val_len,
                                 u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (shortCycEvtThrLocation_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_shortCycSubTypeFiltLocation(int action, u_char *var_val,
                                      u_char var_val_type, size_t var_val_len,
                                      u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (shortCycSubTypeFiltLocation_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_shortCycHistAutoReport(int action, u_char *var_val,
                                 u_char var_val_type, size_t var_val_len,
                                 u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (shortCycHistAutoReport_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_usrDefCycMoniEn(int action, u_char *var_val,
                          u_char var_val_type, size_t var_val_len,
                          u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (usrDefCycMoniEn_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_usrDefCycBelongTask(int action, u_char *var_val,
                              u_char var_val_type, size_t var_val_len,
                              u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (usrDefCycBelongTask_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_usrDefCycEvtThrLocation(int action, u_char *var_val,
                                  u_char var_val_type, size_t var_val_len,
                                  u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (usrDefCycEvtThrLocation_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_usrDefCycSubTypeFiltLocation(int action, u_char *var_val,
                                       u_char var_val_type, size_t var_val_len,
                                       u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (usrDefCycSubTypeFiltLocation_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_usrDefCycHistAutoReport(int action, u_char *var_val,
                                  u_char var_val_type, size_t var_val_len,
                                  u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (usrDefCycHistAutoReport_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_statPortMoniRowStatus(int action, u_char *var_val,
                                u_char var_val_type, size_t var_val_len,
                                u_char *statP, oid *name, size_t length)
{
    stPortMoniCtrlTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.portMoniIndex1 = name[length - 7];
    index.portMoniIndex2 = name[length - 6];
    index.portMoniIndex3 = name[length - 5];
    index.portMoniIndex4 = name[length - 4];
    index.portMoniIndex5 = name[length - 3];
    index.portMoniDevIndex = name[length - 2];
    index.portMoniScanType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (statPortMoniCtrlRowStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}


/**************************************** write currStat table *********************************************/

static int currStatRowStatus_get(stCurrStatTab_index index, int *data)
{
    u_int32_t ret;

    ret = currStatReadMessage(index);

    if (ret == FALSE)
    {
        if ((statCurrStatTableList.flagsOfColumnSet & FLAG_CURRSTATROWSTATUS)
                && ((statCurrStatTableList.index.currPortIndex1 == index.currPortIndex1) &&
                    (statCurrStatTableList.index.currPortIndex2 == index.currPortIndex2) &&
                    (statCurrStatTableList.index.currPortIndex3 == index.currPortIndex3) &&
                    (statCurrStatTableList.index.currPortIndex4 == index.currPortIndex4) &&
                    (statCurrStatTableList.index.currPortIndex5 == index.currPortIndex5) &&
                    (statCurrStatTableList.index.currDevIndex == index.currDevIndex) &&
                    (statCurrStatTableList.index.currDataType == index.currDataType)))
        {
            if (statCurrStatTableList.flagsOfColumnSet & FLAG_CLEARCURRDATACLEAR)
            {
                *data =  SNMP_ROW_NOTINSERVICE;
            }
            else
            {
                *data =  SNMP_ROW_NOTREADY;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        *data = SNMP_ROW_ACTIVE;
    }

    return TRUE;
}

static int currStatRowStatus_set(stCurrStatTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = currStatRowStatus_get(index, &row_status);
    zlog_debug(SNMP_DBG_MIB_GET, "%s : ret(%d) : row_status(%d)\n\r", __FUNCTION__, ret, row_status);

    if (ret == FALSE)
    {
        if (SNMP_ROW_CREATEANDWAIT == data)
        {
            statCurrStatTableList.flagsOfColumnSet = FLAG_CURRSTATROWSTATUS;
            //assign the index here
            statCurrStatTableList.index.currPortIndex1 = index.currPortIndex1;
            statCurrStatTableList.index.currPortIndex2 = index.currPortIndex2;
            statCurrStatTableList.index.currPortIndex3 = index.currPortIndex3;
            statCurrStatTableList.index.currPortIndex4 = index.currPortIndex4;
            statCurrStatTableList.index.currPortIndex5 = index.currPortIndex5;
            statCurrStatTableList.index.currDevIndex = index.currDevIndex;
            statCurrStatTableList.index.currDataType = index.currDataType;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if (SNMP_ROW_ACTIVE == data)
        {
            //if (statCurrStatTableList.column.currStatRowStatus == SNMP_ROW_NOTINSERVICE)
            if (currStat.flag == MODIFY_TABLE_FLAG)
            {
                ret = currStatOperate(GPN_STAT_MSG_CURR_DATA_MODIFY, index);
                zlog_debug(SNMP_DBG_MIB_GET, "%s : modify currStatTable return(%d) \n\r", \
                           __FUNCTION__, ret);

                if (ret == FALSE)
                {
                    return FALSE;
                }

                memset(&currStat, 0, sizeof(currStatus));
                memset(&currStatData, 0, sizeof(stCurrStatTab_info));
                memset(&statCurrStatTableList, 0, sizeof(stCurrStatTab_info));
            }
            else if (SNMP_ROW_NOTINSERVICE == row_status)
            {
                //fill your code here

                memset(&currStat, 0, sizeof(currStatus));
                memset(&currStatData, 0, sizeof(stCurrStatTab_info));
                memset(&statCurrStatTableList, 0, sizeof(stCurrStatTab_info));
                return FALSE;
            }
            else
            {
                return FALSE;
            }
        }
        else if (SNMP_ROW_DESTROY == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //fill your code here
                memset(&repStat, 0, sizeof(reportTpStatus));
                memset(&subTypeReportTmpData, 0 , sizeof(stSubTypeReportTmpTab_info));
                memset(&statSubTypeReportTmpTableList, 0, sizeof(stSubTypeReportTmpTab_info));
                return FALSE;
            }
            else
            {
                memset(&currStat, 0, sizeof(currStatus));
                memset(&currStatData, 0, sizeof(stCurrStatTab_info));
                memset(&statCurrStatTableList, 0, sizeof(stCurrStatTab_info));
                return FALSE;
            }
        }
        else if (SNMP_ROW_NOTINSERVICE == data)
        {
            //statCurrStatTableList.column.currStatRowStatus = SNMP_ROW_NOTINSERVICE;
            currStat.flag = MODIFY_TABLE_FLAG;
            statCurrStatTableList.flagsOfColumnSet = FLAG_CURRSTATROWSTATUS;
        }
        else
        {
            memset(&currStat, 0, sizeof(currStatus));
            memset(&currStatData, 0, sizeof(stCurrStatTab_info));
            memset(&statCurrStatTableList, 0, sizeof(stCurrStatTab_info));
            return FALSE;
        }
    }

    return TRUE;
}



static int clearCurrStat_set(stCurrStatTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = currStatRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (currStat.flag == MODIFY_TABLE_FLAG)
            {
                statCurrStatTableList.data.clearCurrData = data;
                statCurrStatTableList.flagsOfColumnSet |= FLAG_CLEARCURRDATACLEAR;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statCurrStatTableList.data.clearCurrData = data;
            statCurrStatTableList.flagsOfColumnSet |= FLAG_CLEARCURRDATACLEAR;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


int write_clearCurrData(int action, u_char *var_val,
                        u_char var_val_type, size_t var_val_len,
                        u_char *statP, oid *name, size_t length)
{
    stCurrStatTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.currPortIndex1 = name[length - 7];
    index.currPortIndex2 = name[length - 6];
    index.currPortIndex3 = name[length - 5];
    index.currPortIndex4 = name[length - 4];
    index.currPortIndex5 = name[length - 3];
    index.currDevIndex = name[length - 2];
    index.currDataType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (clearCurrStat_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_currStatRowStatus(int action, u_char *var_val,
                            u_char var_val_type, size_t var_val_len,
                            u_char *statP, oid *name, size_t length)
{
    stCurrStatTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.currPortIndex1 = name[length - 7];
    index.currPortIndex2 = name[length - 6];
    index.currPortIndex3 = name[length - 5];
    index.currPortIndex4 = name[length - 4];
    index.currPortIndex5 = name[length - 3];
    index.currDevIndex = name[length - 2];
    index.currDataType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (currStatRowStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;

}


/**************************************** write statTask table *********************************************/


static int statTaskRowStatus_get(stStatTaskTab_index index, int *data)
{
    u_int32_t ret;

    ret = statTaskReadMessage(index);

    if (ret == FALSE)
    {
        if ((statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKTABLEROWSTATUS)
                && (statStatTaskTableList.index.taskIndex == index.taskIndex))
        {
            if ((statStatTaskTableList.flagsOfColumnSet & FLAG_MAXPORTINTASK)
                    && (statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKSCANTYPE)
                    && (statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKCYCMONITYPE)
                    && (statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKMONICYCSECONDS)
                    && (statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKSTARTTIME)
                    && (statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKENDTIME)
                    && (statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKALMTHRESHOLDTPLOCATION)
                    && (statStatTaskTableList.flagsOfColumnSet & FLAG_STATTASKSUBREPORTLOCATION))
            {
                *data =  SNMP_ROW_NOTINSERVICE;
            }
            else
            {
                *data =  SNMP_ROW_NOTREADY;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        *data = SNMP_ROW_ACTIVE;
    }

    return TRUE;
}

static int statTaskRowStatus_set(stStatTaskTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statTaskRowStatus_get(index, &row_status);
    zlog_debug(SNMP_DBG_MIB_GET, "%s : stat alarm status(%d) return reslut(%d), data(%d)\n\r", \
               __FUNCTION__, row_status, ret, data);

    if (ret == FALSE)
    {
        if (SNMP_ROW_CREATEANDWAIT == data)
        {
            statStatTaskTableList.flagsOfColumnSet = FLAG_STATTASKTABLEROWSTATUS;
            //assign the index here
            statStatTaskTableList.index.taskIndex = index.taskIndex;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if (SNMP_ROW_ACTIVE == data)
        {
            //if (statStatTaskTableList.column.statTaskTableRowStatus == SNMP_ROW_NOTINSERVICE)
            if (taskStat.flag == MODIFY_TABLE_FLAG)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s : modify task table\n\r", __FUNCTION__);
                ret = statTaskOperate(GPN_STAT_MSG_TASK_MODIFY, index);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : modify table failure\n\r", __FUNCTION__);
                    return FALSE;
                }

                memset(&taskStat, 0, sizeof(statTaskStatus));
                memset(&statTaskData, 0 , sizeof(stStatTaskTab_info));
                memset(&statStatTaskTableList, 0, sizeof(stStatTaskTab_info));
            }
            else if (SNMP_ROW_NOTINSERVICE == row_status)
            {
                //fill your code here
                zlog_debug(SNMP_DBG_MIB_GET, "%s : add task table\n\r", __FUNCTION__);
                ret = statTaskOperate(GPN_STAT_MSG_TASK_ADD, index);
                zlog_debug(SNMP_DBG_MIB_GET, "%s : add statTaskTable return(%d)\n\r", \
                           __FUNCTION__, ret);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : add table failure\n\r", __FUNCTION__);
                    return FALSE;
                }

                memset(&taskStat, 0, sizeof(statTaskStatus));
                memset(&statTaskData, 0 , sizeof(stStatTaskTab_info));
                memset(&statStatTaskTableList, 0, sizeof(stStatTaskTab_info));
            }
            else
            {
                return FALSE;
            }
        }
        else if (SNMP_ROW_DESTROY == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //fill your code here
                zlog_debug(SNMP_DBG_MIB_GET, "%s : delete task table\n\r", __FUNCTION__);
                ret = statTaskOperate(GPN_STAT_MSG_TASK_DELETE, index);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : delete table failure\n\r", __FUNCTION__);
                    return FALSE;
                }

                memset(&taskStat, 0, sizeof(statTaskStatus));
                memset(&statTaskData, 0 , sizeof(stStatTaskTab_info));
                memset(&statStatTaskTableList, 0, sizeof(stStatTaskTab_info));
            }
            else
            {
                memset(&taskStat, 0, sizeof(statTaskStatus));
                //memset(&statTaskData, 0 , sizeof(stStatTaskTab_info));
                memset(&statStatTaskTableList, 0, sizeof(stStatTaskTab_info));
            }
        }
        else if (SNMP_ROW_NOTINSERVICE == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //statStatTaskTableList.column.statTaskTableRowStatus = SNMP_ROW_NOTINSERVICE;
                taskStat.flag = MODIFY_TABLE_FLAG;
                statStatTaskTableList.flagsOfColumnSet = FLAG_STATTASKTABLEROWSTATUS;
            }
            else
            {
                memset(&taskStat, 0, sizeof(statTaskStatus));
                memset(&statTaskData, 0 , sizeof(stStatTaskTab_info));
                memset(&statStatTaskTableList, 0, sizeof(stStatTaskTab_info));
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}



static int maxPortInTask_set(stStatTaskTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statTaskRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (taskStat.flag == MODIFY_TABLE_FLAG)
            {
                statStatTaskTableList.data.maxPortInTask = data;
                statStatTaskTableList.flagsOfColumnSet |= FLAG_MAXPORTINTASK;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statStatTaskTableList.data.maxPortInTask = data;
            statStatTaskTableList.flagsOfColumnSet |= FLAG_MAXPORTINTASK;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


static int taskScanType_set(stStatTaskTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statTaskRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (taskStat.flag == MODIFY_TABLE_FLAG)
            {
                statStatTaskTableList.data.taskScanType = data;
                statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKSCANTYPE;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statStatTaskTableList.data.taskScanType = data;
            statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKSCANTYPE;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int taskCycMoniType_set(stStatTaskTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statTaskRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (taskStat.flag == MODIFY_TABLE_FLAG)
            {
                statStatTaskTableList.data.taskCycMoniType = data;
                statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKCYCMONITYPE;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statStatTaskTableList.data.taskCycMoniType = data;
            statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKCYCMONITYPE;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int taskMoniCycSec_set(stStatTaskTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statTaskRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (taskStat.flag == MODIFY_TABLE_FLAG)
            {
                statStatTaskTableList.data.taskMoniCycSec = data;
                statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKMONICYCSECONDS;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statStatTaskTableList.data.taskMoniCycSec = data;
            statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKMONICYCSECONDS;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int taskStartTime_set(stStatTaskTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statTaskRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (taskStat.flag == MODIFY_TABLE_FLAG)
            {
                statStatTaskTableList.data.taskStartTime = data;
                statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKSTARTTIME;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statStatTaskTableList.data.taskStartTime = data;
            statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKSTARTTIME;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int taskEndTime_set(stStatTaskTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statTaskRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (taskStat.flag == MODIFY_TABLE_FLAG)
            {
                statStatTaskTableList.data.taskEndTime = data;
                statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKENDTIME;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statStatTaskTableList.data.taskEndTime = data;
            statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKENDTIME;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int taskAlmThrTpLocation_set(stStatTaskTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statTaskRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (taskStat.flag == MODIFY_TABLE_FLAG)
            {
                statStatTaskTableList.data.taskAlmThrTpLocation = data;
                statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKALMTHRESHOLDTPLOCATION;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statStatTaskTableList.data.taskAlmThrTpLocation = data;
            statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKALMTHRESHOLDTPLOCATION;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int taskSubReportLocation_set(stStatTaskTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statTaskRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (taskStat.flag == MODIFY_TABLE_FLAG)
            {
                statStatTaskTableList.data.taskSubReportLocation = data;
                statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKSUBREPORTLOCATION;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statStatTaskTableList.data.taskSubReportLocation = data;
            statStatTaskTableList.flagsOfColumnSet |= FLAG_STATTASKSUBREPORTLOCATION;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


int write_maxPortInTask(int action, u_char *var_val,
                        u_char var_val_type, size_t var_val_len,
                        u_char *statP, oid *name, size_t length)
{
    stStatTaskTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.taskIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (maxPortInTask_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_taskScanType(int action, u_char *var_val,
                       u_char var_val_type, size_t var_val_len,
                       u_char *statP, oid *name, size_t length)
{
    stStatTaskTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.taskIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (taskScanType_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_statTaskCycMoniType(int action, u_char *var_val,
                              u_char var_val_type, size_t var_val_len,
                              u_char *statP, oid *name, size_t length)
{
    stStatTaskTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.taskIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (taskCycMoniType_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_taskMoniCycSec(int action, u_char *var_val,
                         u_char var_val_type, size_t var_val_len,
                         u_char *statP, oid *name, size_t length)
{
    stStatTaskTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.taskIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (taskMoniCycSec_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_taskStartTime(int action, u_char *var_val,
                        u_char var_val_type, size_t var_val_len,
                        u_char *statP, oid *name, size_t length)
{
    stStatTaskTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.taskIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (taskStartTime_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_taskEndTime(int action, u_char *var_val,
                      u_char var_val_type, size_t var_val_len,
                      u_char *statP, oid *name, size_t length)
{
    stStatTaskTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.taskIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (taskEndTime_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_taskAlmThrTpLocation(int action, u_char *var_val,
                               u_char var_val_type, size_t var_val_len,
                               u_char *statP, oid *name, size_t length)
{
    stStatTaskTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.taskIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (taskAlmThrTpLocation_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_taskSubReportLocation(int action, u_char *var_val,
                                u_char var_val_type, size_t var_val_len,
                                u_char *statP, oid *name, size_t length)
{
    stStatTaskTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.taskIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (taskSubReportLocation_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_statTaskRowStatus(int action, u_char *var_val,
                            u_char var_val_type, size_t var_val_len,
                            u_char *statP, oid *name, size_t length)
{
    stStatTaskTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.taskIndex = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (statTaskRowStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}



/**************************************** write statEvtThrTmp table *********************************************/


static int statEventTdTpRowStatus_get(stEvtThrTmpTab_index index, int *data)
{
    u_int32_t ret;

    ret = statEvtThrTmpReadMessage(index);

    if (ret == FALSE)
    {
        if ((statEvtThrTmpTableList.flagsOfColumnSet & FLAG_STATEVENTTHRESHOLDTPSTATROWSTATUS)
                && ((statEvtThrTmpTableList.index.evtThrTmpIndex == index.evtThrTmpIndex) &&
                    (statEvtThrTmpTableList.index.evtThrTmpTpSubType == index.evtThrTmpTpSubType)))
        {
            if ((statEvtThrTmpTableList.flagsOfColumnSet & FLAG_STATEVENTTHRESHOLDSCANTYPE) &&
                    (statEvtThrTmpTableList.flagsOfColumnSet & FLAG_STATEVENTTHRESHOLDTPSUBTYPEUPTHRESHHIGH32) &&
                    (statEvtThrTmpTableList.flagsOfColumnSet & FLAG_STATEVENTTHRESHOLDTPSUBTYPEUPTHRESHLOW32) &&
                    (statEvtThrTmpTableList.flagsOfColumnSet & FLAG_STATEVENTTHRESHOLDTPSUBTYPELOWTHRESHHIGH32) &&
                    (statEvtThrTmpTableList.flagsOfColumnSet & FLAG_STATEVENTTHRESHOLDTPSUBTYPELOWTHRESHLOW32))
            {
                *data =  SNMP_ROW_NOTINSERVICE;
            }
            else
            {
                *data =  SNMP_ROW_NOTREADY;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        *data = SNMP_ROW_ACTIVE;
    }

    return TRUE;
}

static int statEventTdTpRowStatus_set(stEvtThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statEventTdTpRowStatus_get(index, &row_status);
    zlog_debug(SNMP_DBG_MIB_GET, "%s : statEventThresholdTpStatRowStatus_get return(%d) row_status(%d)\n\r", \
               __FUNCTION__, ret, row_status);

    if (ret == FALSE)
    {
        if (SNMP_ROW_CREATEANDWAIT == data)
        {
            statEvtThrTmpTableList.flagsOfColumnSet = FLAG_STATEVENTTHRESHOLDTPSTATROWSTATUS;
            //assign the index here
            statEvtThrTmpTableList.index.evtThrTmpIndex     = index.evtThrTmpIndex;
            statEvtThrTmpTableList.index.evtThrTmpTpSubType = index.evtThrTmpTpSubType;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if (SNMP_ROW_ACTIVE == data)
        {
            if (eventStat.flag == MODIFY_TABLE_FLAG)
            {
                statEvtThrTmpTableList.index.evtThrTmpIndex     = index.evtThrTmpIndex;
                statEvtThrTmpTableList.index.evtThrTmpTpSubType = index.evtThrTmpTpSubType;
                ret = statEvtThrTmpOperate(GPN_STAT_MSG_EVN_THRED_TEMP_MODIFY, index);
                zlog_debug(SNMP_DBG_MIB_GET, "%s : modify eventThrTmp table\n\r", __FUNCTION__);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : modify eventThrTmp failure\n\r", __FUNCTION__);
                    return FALSE;
                }

                memset(&eventStat, 0, sizeof(eventThTpStatus));
                memset(&evtThrTmpData, 0 , sizeof(stEvtThrTmpTab_info));
                memset(&statEvtThrTmpTableList, 0, sizeof(stEvtThrTmpTab_info));
            }
            else if (SNMP_ROW_NOTINSERVICE == row_status)
            {
                //fill your code here
                zlog_debug(SNMP_DBG_MIB_GET, "%s : add eventThrTmp table\n\r", __FUNCTION__);
                ret = statEvtThrTmpOperate(GPN_STAT_MSG_EVN_THRED_TEMP_ADD, statEvtThrTmpTableList.index);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : add table eventThrTmp failure\n\r", __FUNCTION__);
                    return FALSE;
                }

                memset(&eventStat, 0, sizeof(eventThTpStatus));
                memset(&evtThrTmpData, 0 , sizeof(stEvtThrTmpTab_info));
                memset(&statEvtThrTmpTableList, 0, sizeof(stEvtThrTmpTab_info));
            }
            else
            {
                memset(&eventStat, 0, sizeof(eventThTpStatus));
                memset(&evtThrTmpData, 0 , sizeof(stEvtThrTmpTab_info));
                memset(&statEvtThrTmpTableList, 0, sizeof(stEvtThrTmpTab_info));
                return FALSE;
            }
        }
        else if (SNMP_ROW_DESTROY == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //fill your code here
                zlog_debug(SNMP_DBG_MIB_GET, "%s : delete eventThrTmp table\n\r", __FUNCTION__);
                //statEvtThrTmpTableList.index.evtThrTmpIndex   = index.evtThrTmpIndex;
                //statEvtThrTmpTableList.index.evtThrTmpTpSubType   = index.evtThrTmpTpSubType;
                ret = statEvtThrTmpOperate(GPN_STAT_MSG_EVN_THRED_TEMP_DELETE, index);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : delete table eventThrTmp failure\n\r", __FUNCTION__);
                    return FALSE;
                }

                memset(&eventStat, 0, sizeof(eventThTpStatus));
                memset(&evtThrTmpData, 0 , sizeof(stEvtThrTmpTab_info));
                memset(&statEvtThrTmpTableList, 0, sizeof(stEvtThrTmpTab_info));
            }
            else
            {
                memset(&eventStat, 0, sizeof(eventThTpStatus));
                memset(&evtThrTmpData, 0 , sizeof(stEvtThrTmpTab_info));
                memset(&statEvtThrTmpTableList, 0, sizeof(stEvtThrTmpTab_info));
            }
        }
        else if (SNMP_ROW_NOTINSERVICE == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //statEvtThrTmpTableList.column.statEventThresholdTpStatRowStatus = SNMP_ROW_NOTINSERVICE;
                eventStat.flag = MODIFY_TABLE_FLAG;
                statEvtThrTmpTableList.flagsOfColumnSet = FLAG_STATEVENTTHRESHOLDTPSTATROWSTATUS;
            }
            else
            {
                memset(&eventStat, 0, sizeof(eventThTpStatus));
                memset(&evtThrTmpData, 0 , sizeof(stEvtThrTmpTab_info));
                memset(&statEvtThrTmpTableList, 0, sizeof(stEvtThrTmpTab_info));
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}



static int evtThrScanType_set(stEvtThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statEventTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (eventStat.flag == MODIFY_TABLE_FLAG)
            {
                statEvtThrTmpTableList.data.evtThrScanType = data;
                statEvtThrTmpTableList.flagsOfColumnSet |= FLAG_STATEVENTTHRESHOLDSCANTYPE;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statEvtThrTmpTableList.data.evtThrScanType = data;
            statEvtThrTmpTableList.flagsOfColumnSet |= FLAG_STATEVENTTHRESHOLDSCANTYPE;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int evtThrTpSubTypeUpThrHigh32_set(stEvtThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statEventTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (eventStat.flag == MODIFY_TABLE_FLAG)
            {
                statEvtThrTmpTableList.data.evtThrTpSubTypeUpThrHigh32 = data;
                statEvtThrTmpTableList.flagsOfColumnSet |= FLAG_STATEVENTTHRESHOLDTPSUBTYPEUPTHRESHHIGH32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statEvtThrTmpTableList.data.evtThrTpSubTypeUpThrHigh32 = data;
            statEvtThrTmpTableList.flagsOfColumnSet |= FLAG_STATEVENTTHRESHOLDTPSUBTYPEUPTHRESHHIGH32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int evtThrTpSubTypeUpThrLow32_set(stEvtThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statEventTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (eventStat.flag == MODIFY_TABLE_FLAG)
            {
                statEvtThrTmpTableList.data.evtThrTpSubTypeUpThrLow32 = data;
                statEvtThrTmpTableList.flagsOfColumnSet |= FLAG_STATEVENTTHRESHOLDTPSUBTYPEUPTHRESHLOW32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statEvtThrTmpTableList.data.evtThrTpSubTypeUpThrLow32 = data;
            statEvtThrTmpTableList.flagsOfColumnSet |= FLAG_STATEVENTTHRESHOLDTPSUBTYPEUPTHRESHLOW32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int evtThrTpSubTypeLowThrHigh32_set(stEvtThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statEventTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (eventStat.flag == MODIFY_TABLE_FLAG)
            {
                statEvtThrTmpTableList.data.evtThrTpSubTypeLowThrHigh32 = data;
                statEvtThrTmpTableList.flagsOfColumnSet |= FLAG_STATEVENTTHRESHOLDTPSUBTYPELOWTHRESHHIGH32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statEvtThrTmpTableList.data.evtThrTpSubTypeLowThrHigh32 = data;
            statEvtThrTmpTableList.flagsOfColumnSet |= FLAG_STATEVENTTHRESHOLDTPSUBTYPELOWTHRESHHIGH32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int evtThrTpSubTypeLowThrLow32_set(stEvtThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statEventTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (eventStat.flag == MODIFY_TABLE_FLAG)
            {
                statEvtThrTmpTableList.data.evtThrTpSubTypeLowThrLow32 = data;
                statEvtThrTmpTableList.flagsOfColumnSet |= FLAG_STATEVENTTHRESHOLDTPSUBTYPELOWTHRESHLOW32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statEvtThrTmpTableList.data.evtThrTpSubTypeLowThrLow32 = data;
            statEvtThrTmpTableList.flagsOfColumnSet |= FLAG_STATEVENTTHRESHOLDTPSUBTYPELOWTHRESHLOW32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}



int write_evtThrScanType(int action, u_char *var_val,
                         u_char var_val_type, size_t var_val_len,
                         u_char *statP, oid *name, size_t length)
{
    stEvtThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.evtThrTmpIndex = name[length - 2];
    index.evtThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (evtThrScanType_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_evtThrTpSubTypeUpThrHigh32(int action, u_char *var_val,
                                     u_char var_val_type, size_t var_val_len,
                                     u_char *statP, oid *name, size_t length)
{
    stEvtThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.evtThrTmpIndex = name[length - 2];
    index.evtThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (evtThrTpSubTypeUpThrHigh32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_evtThrTpSubTypeUpThrLow32(int action, u_char *var_val,
                                    u_char var_val_type, size_t var_val_len,
                                    u_char *statP, oid *name, size_t length)
{
    stEvtThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.evtThrTmpIndex = name[length - 2];
    index.evtThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (evtThrTpSubTypeUpThrLow32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_evtThrTpSubTypeLowThrHigh32(int action, u_char *var_val,
                                      u_char var_val_type, size_t var_val_len,
                                      u_char *statP, oid *name, size_t length)
{
    stEvtThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.evtThrTmpIndex = name[length - 2];
    index.evtThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (evtThrTpSubTypeLowThrHigh32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_evtThrTpSubTypeLowThrLow32(int action, u_char *var_val,
                                     u_char var_val_type, size_t var_val_len,
                                     u_char *statP, oid *name, size_t length)
{
    stEvtThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.evtThrTmpIndex = name[length - 2];
    index.evtThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (evtThrTpSubTypeLowThrLow32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_statEventTdTpRowStatus(int action, u_char *var_val,
                                 u_char var_val_type, size_t var_val_len,
                                 u_char *statP, oid *name, size_t length)
{
    stEvtThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.evtThrTmpIndex = name[length - 2];
    index.evtThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (statEventTdTpRowStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}



/**************************************** write stSubStatTypeFileTp table *********************************************/


static int statSelectSubRowStatus_get(stSubStatTypeFileTpTab_index index, int *data)
{
    u_int32_t ret;

    ret = statSubStatTypeFileTpTabReadMessage(index);

    if (ret == FALSE)
    {
        if ((statSubStatTypeFileTpTableList.flagsOfColumnSet & FLAG_STATSELECTSUBROWSTATUS)
                && ((statSubStatTypeFileTpTableList.index.subTypeFiltIndex == index.subTypeFiltIndex) &&
                    (statSubStatTypeFileTpTableList.index.selectSubType == index.selectSubType)))
        {
            if ((statSubStatTypeFileTpTableList.flagsOfColumnSet & FLAG_STATSELECTSTATSCANTYPE) &&
                    (statSubStatTypeFileTpTableList.flagsOfColumnSet & FLAG_STATSELECTSTATUS))
            {
                *data =  SNMP_ROW_NOTINSERVICE;
            }
            else
            {
                *data =  SNMP_ROW_NOTREADY;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        *data = SNMP_ROW_ACTIVE;
    }

    return TRUE;
}

static int statSelectSubRowStatus_set(stSubStatTypeFileTpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSelectSubRowStatus_get(index, &row_status);
    zlog_debug(SNMP_DBG_MIB_GET, "%s : stat alarm status(%d) return reslut(%d), data(%d)\n\r", \
               __FUNCTION__, row_status, ret, data);

    if (ret == FALSE)
    {
        if (SNMP_ROW_CREATEANDWAIT == data)
        {
            statSubStatTypeFileTpTableList.flagsOfColumnSet = FLAG_STATSELECTSUBROWSTATUS;
            //assign the index here
            statSubStatTypeFileTpTableList.index.subTypeFiltIndex = index.subTypeFiltIndex;
            statSubStatTypeFileTpTableList.index.selectSubType    = index.selectSubType;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if (SNMP_ROW_ACTIVE == data)
        {
            if (filtStat.flag == MODIFY_TABLE_FLAG)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s : modify subFiltTmp table\n\r", __FUNCTION__);
                ret = statSubStatTypeFileTpTabOperate(GPN_STAT_MSG_SUBFILT_TEMP_MODIFY, index);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : modify subFiltTmp failure\n\r", \
                               __FUNCTION__);
                    return FALSE;
                }

                memset(&filtStat, 0, sizeof(filtTpStatus));
                memset(&subStatTypeFileTpData, 0 , sizeof(stSubStatTypeFileTpTab_info));
                memset(&statSubStatTypeFileTpTableList, 0, sizeof(stSubStatTypeFileTpTab_info));
            }
            else if (SNMP_ROW_NOTINSERVICE == row_status)
            {
                //fill your code here
                zlog_debug(SNMP_DBG_MIB_GET, "%s : add subFiltTmp table\n\r", __FUNCTION__);
                ret = statSubStatTypeFileTpTabOperate(GPN_STAT_MSG_SUBFILT_TEMP_ADD, index);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : add subFiltTmp table failure\n\r", \
                               __FUNCTION__);
                    return FALSE;
                }

                memset(&filtStat, 0, sizeof(filtTpStatus));
                memset(&subStatTypeFileTpData, 0 , sizeof(stSubStatTypeFileTpTab_info));
                memset(&statSubStatTypeFileTpTableList, 0, sizeof(stSubStatTypeFileTpTab_info));
            }
            else
            {
                return FALSE;
            }
        }
        else if (SNMP_ROW_DESTROY == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //fill your code here
                zlog_debug(SNMP_DBG_MIB_GET, "%s : delete subFiltTmp table\n\r", __FUNCTION__);
                ret = statSubStatTypeFileTpTabOperate(GPN_STAT_MSG_SUBFILT_TEMP_DELETE, index);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : delete subFiltTmp table failure\n\r", \
                               __FUNCTION__);
                    return FALSE;
                }

                memset(&filtStat, 0, sizeof(filtTpStatus));
                memset(&subStatTypeFileTpData, 0 , sizeof(stSubStatTypeFileTpTab_info));
                memset(&statSubStatTypeFileTpTableList, 0, sizeof(stSubStatTypeFileTpTab_info));
            }
            else
            {
                memset(&filtStat, 0, sizeof(filtTpStatus));
                //memset(&subStatTypeFileTpData, 0 , sizeof(stSubStatTypeFileTpTab_info));
                memset(&statSubStatTypeFileTpTableList, 0, sizeof(stSubStatTypeFileTpTab_info));
            }
        }
        else if (SNMP_ROW_NOTINSERVICE == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //statSubStatTypeFileTpTableList.column.statSelectSubRowStatus = SNMP_ROW_NOTINSERVICE;
                filtStat.flag = MODIFY_TABLE_FLAG;
                statSubStatTypeFileTpTableList.flagsOfColumnSet = FLAG_STATSELECTSUBROWSTATUS;
            }
            else
            {
                memset(&subStatTypeFileTpData, 0 , sizeof(stSubStatTypeFileTpTab_info));
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}



static int selectScanType_set(stSubStatTypeFileTpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSelectSubRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (filtStat.flag == MODIFY_TABLE_FLAG)
            {
                statSubStatTypeFileTpTableList.data.selectScanType = data;
                statSubStatTypeFileTpTableList.flagsOfColumnSet |= FLAG_STATSELECTSTATSCANTYPE;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statSubStatTypeFileTpTableList.data.selectScanType = data;
            statSubStatTypeFileTpTableList.flagsOfColumnSet |= FLAG_STATSELECTSTATSCANTYPE;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int selectStatus_set(stSubStatTypeFileTpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSelectSubRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (filtStat.flag == MODIFY_TABLE_FLAG)
            {
                statSubStatTypeFileTpTableList.data.selectStatus = data;
                statSubStatTypeFileTpTableList.flagsOfColumnSet |= FLAG_STATSELECTSTATUS;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statSubStatTypeFileTpTableList.data.selectStatus = data;
            statSubStatTypeFileTpTableList.flagsOfColumnSet |= FLAG_STATSELECTSTATUS;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}


int write_selectScanType(int action, u_char *var_val,
                         u_char var_val_type, size_t var_val_len,
                         u_char *statP, oid *name, size_t length)
{
    stSubStatTypeFileTpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subTypeFiltIndex = name[length - 2];
    index.selectSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (selectScanType_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_selectStatus(int action, u_char *var_val,
                       u_char var_val_type, size_t var_val_len,
                       u_char *statP, oid *name, size_t length)
{
    stSubStatTypeFileTpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subTypeFiltIndex = name[length - 2];
    index.selectSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (selectStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_statSelectSubRowStatus(int action, u_char *var_val,
                                 u_char var_val_type, size_t var_val_len,
                                 u_char *statP, oid *name, size_t length)
{
    stSubStatTypeFileTpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subTypeFiltIndex = name[length - 2];
    index.selectSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (statSelectSubRowStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}




/**************************************** write statAlmThrTmpTab table *********************************************/


static int statAlarmTdTpRowStatus_get(stAlmThrTmpTab_index index, int *data)
{
    u_int32_t ret;

    ret = statAlmThrTmpTabReadMessage(index);

    if (ret == FALSE)
    {
        if ((statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDTPROWSTATUS)
                && ((statAlmThrTmpTableList.index.almThrTmpIndex == index.almThrTmpIndex) &&
                    (statAlmThrTmpTableList.index.almThrTmpTpSubType == index.almThrTmpTpSubType)))
        {
            if ((statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSCANTYPE) &&
                (statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEUPRISEHIGH32) &&
                (statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEUPRISELOW32) &&
                (statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEUPCLEARHIGH32) &&
                (statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEUPCLEARLOW32) &&
                (statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEDNRISEHIGH32) &&
                (statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEDNRISELOW32) &&
                (statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEDNCLEARHIGH32) &&
                (statAlmThrTmpTableList.flagsOfColumnSet & FLAG_STATALARMTHRESHOLDSUBTYPEDNCLEARLOW32))
            {
                *data =  SNMP_ROW_NOTINSERVICE;
            }
            else
            {
                *data =  SNMP_ROW_NOTREADY;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        *data = SNMP_ROW_ACTIVE;
    }

    return TRUE;
}

static int statAlarmTdTpRowStatus_set(stAlmThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statAlarmTdTpRowStatus_get(index, &row_status);
    zlog_debug(SNMP_DBG_MIB_GET, "%s : stat alarm status(%d) return reslut(%d), data(%d)\n\r", \
               __FUNCTION__, row_status, ret, data);

    if (ret == FALSE)
    {
        if (SNMP_ROW_CREATEANDWAIT == data)
        {
            statAlmThrTmpTableList.flagsOfColumnSet = FLAG_STATALARMTHRESHOLDTPROWSTATUS;
            //assign the index here
            statAlmThrTmpTableList.index.almThrTmpIndex     = index.almThrTmpIndex;
            statAlmThrTmpTableList.index.almThrTmpTpSubType = index.almThrTmpTpSubType;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if (SNMP_ROW_ACTIVE == data)
        {
            if (almStat.flag == MODIFY_TABLE_FLAG)
            {
                ret = statAlmThrTmpTabOperate(GPN_STAT_MSG_ALM_THRED_TEMP_MODIFY, index);
                zlog_debug(SNMP_DBG_MIB_GET, "%s : modify alarmThrTmp table\n\r", __FUNCTION__);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : modify alarmThrTmp failure\n\r", \
                               __FUNCTION__);
                    return FALSE;
                }

                memset(&almStat, 0, sizeof(almThTpStatus));
                memset(&almThrTmpData, 0 , sizeof(stAlmThrTmpTab_info));
                memset(&statAlmThrTmpTableList, 0, sizeof(stAlmThrTmpTab_info));
            }
            else if (SNMP_ROW_NOTINSERVICE == row_status)
            {
                //fill your code here
                zlog_debug(SNMP_DBG_MIB_GET, "%s : add alarmThrTmp table\n\r", __FUNCTION__);
                ret = statAlmThrTmpTabOperate(GPN_STAT_MSG_ALM_THRED_TEMP_ADD, index);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : add alarmThrTmp failure\n\r", \
                               __FUNCTION__);
                    return FALSE;
                }

                memset(&almStat, 0, sizeof(almThTpStatus));
                memset(&almThrTmpData, 0 , sizeof(stAlmThrTmpTab_info));
                memset(&statAlmThrTmpTableList, 0, sizeof(stAlmThrTmpTab_info));
            }
            else
            {
                return FALSE;
            }
        }
        else if (SNMP_ROW_DESTROY == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //fill your code here
                zlog_debug(SNMP_DBG_MIB_GET, "%s : delete alarmThrTmp table\n\r", __FUNCTION__);
                ret = statAlmThrTmpTabOperate(GPN_STAT_MSG_ALM_THRED_TEMP_DELETE, index);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : delete alarmThrTmp failure\n\r", \
                               __FUNCTION__);
                    return FALSE;
                }

                memset(&almStat, 0, sizeof(almThTpStatus));
                memset(&almThrTmpData, 0 , sizeof(stAlmThrTmpTab_info));
                memset(&statAlmThrTmpTableList, 0, sizeof(stAlmThrTmpTab_info));
            }
            else
            {
                memset(&almStat, 0, sizeof(almThTpStatus));
                memset(&almThrTmpData, 0 , sizeof(stAlmThrTmpTab_info));
                memset(&statAlmThrTmpTableList, 0, sizeof(stAlmThrTmpTab_info));
            }
        }
        else if (SNMP_ROW_NOTINSERVICE == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //statAlmThrTmpTableList.column.statAlarmThresholdTpRowStatus = SNMP_ROW_NOTINSERVICE;
                almStat.flag = MODIFY_TABLE_FLAG;
                statAlmThrTmpTableList.flagsOfColumnSet = FLAG_STATALARMTHRESHOLDTPROWSTATUS;
            }
            else
            {
                memset(&almThrTmpData, 0 , sizeof(stAlmThrTmpTab_info));
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}



static int almThrScanType_set(stAlmThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statAlarmTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (almStat.flag == MODIFY_TABLE_FLAG)
            {
                statAlmThrTmpTableList.data.almThrScanType = data;
                statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSCANTYPE;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statAlmThrTmpTableList.data.almThrScanType = data;
            statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSCANTYPE;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int almThrTpSubTypeUpRiseHigh32_set(stAlmThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statAlarmTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (almStat.flag == MODIFY_TABLE_FLAG)
            {
                statAlmThrTmpTableList.data.almThrTpSubTypeUpRiseHigh32 = data;
                statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEUPRISEHIGH32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statAlmThrTmpTableList.data.almThrTpSubTypeUpRiseHigh32 = data;
            statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEUPRISEHIGH32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int almThrTpSubTypeUpRiseLow32_set(stAlmThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statAlarmTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (almStat.flag == MODIFY_TABLE_FLAG)
            {
                statAlmThrTmpTableList.data.almThrTpSubTypeUpRiseLow32 = data;
                statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEUPRISELOW32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statAlmThrTmpTableList.data.almThrTpSubTypeUpRiseLow32 = data;
            statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEUPRISELOW32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int almThrTpSubTypeUpClrHigh32_set(stAlmThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statAlarmTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (almStat.flag == MODIFY_TABLE_FLAG)
            {
                statAlmThrTmpTableList.data.almThrTpSubTypeUpClrHigh32 = data;
                statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEUPCLEARHIGH32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statAlmThrTmpTableList.data.almThrTpSubTypeUpClrHigh32 = data;
            statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEUPCLEARHIGH32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int almThrTpSubTypeUpClrLow32_set(stAlmThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statAlarmTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (almStat.flag == MODIFY_TABLE_FLAG)
            {
                statAlmThrTmpTableList.data.almThrTpSubTypeUpClrLow32 = data;
                statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEUPCLEARLOW32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statAlmThrTmpTableList.data.almThrTpSubTypeUpClrLow32 = data;
            statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEUPCLEARLOW32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int almThrTpSubTypeDnRiseHigh32_set(stAlmThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statAlarmTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (almStat.flag == MODIFY_TABLE_FLAG)
            {
                statAlmThrTmpTableList.data.almThrTpSubTypeDnRiseHigh32 = data;
                statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEDNRISEHIGH32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statAlmThrTmpTableList.data.almThrTpSubTypeDnRiseHigh32 = data;
            statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEDNRISEHIGH32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int almThrTpSubTypeDnRiseLow32_set(stAlmThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statAlarmTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (almStat.flag == MODIFY_TABLE_FLAG)
            {
                statAlmThrTmpTableList.data.almThrTpSubTypeDnRiseLow32 = data;
                statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEDNRISELOW32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statAlmThrTmpTableList.data.almThrTpSubTypeDnRiseLow32 = data;
            statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEDNRISELOW32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int almThrTpSubTypeDnClrHigh32_set(stAlmThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statAlarmTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (almStat.flag == MODIFY_TABLE_FLAG)
            {
                statAlmThrTmpTableList.data.almThrTpSubTypeDnClrHigh32 = data;
                statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEDNCLEARHIGH32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statAlmThrTmpTableList.data.almThrTpSubTypeDnClrHigh32 = data;
            statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEDNCLEARHIGH32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int almThrTpSubTypeDnClrLow32_set(stAlmThrTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statAlarmTdTpRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (almStat.flag == MODIFY_TABLE_FLAG)
            {
                statAlmThrTmpTableList.data.almThrTpSubTypeDnClrLow32 = data;
                statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEDNCLEARLOW32;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statAlmThrTmpTableList.data.almThrTpSubTypeDnClrLow32 = data;
            statAlmThrTmpTableList.flagsOfColumnSet |= FLAG_STATALARMTHRESHOLDSUBTYPEDNCLEARLOW32;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}


int write_almThrScanType(int action, u_char *var_val,
                         u_char var_val_type, size_t var_val_len,
                         u_char *statP, oid *name, size_t length)
{
    stAlmThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.almThrTmpIndex = name[length - 2];
    index.almThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (almThrScanType_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_almThrTpSubTypeUpRiseHigh32(int action, u_char *var_val,
                                      u_char var_val_type, size_t var_val_len,
                                      u_char *statP, oid *name, size_t length)
{
    stAlmThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.almThrTmpIndex = name[length - 2];
    index.almThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (almThrTpSubTypeUpRiseHigh32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_almThrTpSubTypeUpRiseLow32(int action, u_char *var_val,
                                     u_char var_val_type, size_t var_val_len,
                                     u_char *statP, oid *name, size_t length)
{
    stAlmThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.almThrTmpIndex = name[length - 2];
    index.almThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (almThrTpSubTypeUpRiseLow32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_almThrTpSubTypeUpClrHigh32(int action, u_char *var_val,
                                     u_char var_val_type, size_t var_val_len,
                                     u_char *statP, oid *name, size_t length)
{
    stAlmThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.almThrTmpIndex = name[length - 2];
    index.almThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (almThrTpSubTypeUpClrHigh32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_almThrTpSubTypeUpClrLow32(int action, u_char *var_val,
                                    u_char var_val_type, size_t var_val_len,
                                    u_char *statP, oid *name, size_t length)
{
    stAlmThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.almThrTmpIndex = name[length - 2];
    index.almThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (almThrTpSubTypeUpClrLow32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_almThrTpSubTypeDnRiseHigh32(int action, u_char *var_val,
                                      u_char var_val_type, size_t var_val_len,
                                      u_char *statP, oid *name, size_t length)
{
    stAlmThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.almThrTmpIndex = name[length - 2];
    index.almThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (almThrTpSubTypeDnRiseHigh32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_almThrTpSubTypeDnRiseLow32(int action, u_char *var_val,
                                     u_char var_val_type, size_t var_val_len,
                                     u_char *statP, oid *name, size_t length)
{
    stAlmThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.almThrTmpIndex = name[length - 2];
    index.almThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (almThrTpSubTypeDnRiseLow32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_almThrTpSubTypeDnClrHigh32(int action, u_char *var_val,
                                     u_char var_val_type, size_t var_val_len,
                                     u_char *statP, oid *name, size_t length)
{
    stAlmThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.almThrTmpIndex = name[length - 2];
    index.almThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (almThrTpSubTypeDnClrHigh32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_almThrTpSubTypeDnClrLow32(int action, u_char *var_val,
                                    u_char var_val_type, size_t var_val_len,
                                    u_char *statP, oid *name, size_t length)
{
    stAlmThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.almThrTmpIndex = name[length - 2];
    index.almThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (almThrTpSubTypeDnClrLow32_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_statAlarmTdTpRowStatus(int action, u_char *var_val,
                                 u_char var_val_type, size_t var_val_len,
                                 u_char *statP, oid *name, size_t length)
{
    stAlmThrTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.almThrTmpIndex = name[length - 2];
    index.almThrTmpTpSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (statAlarmTdTpRowStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}




/**************************************** write SubTypeReportTmpTab table *********************************************/


static int statSubReportRowStatus_get(stSubTypeReportTmpTab_index index, int *data)
{
    u_int32_t ret;

    ret = statSubTypeReportTmpTabReadMessage(index);

    if (ret == FALSE)
    {
        if ((statSubTypeReportTmpTableList.flagsOfColumnSet & FLAG_STATSUBREPORTROWSTATUS)
                && ((statSubTypeReportTmpTableList.index.subReportTmpIndex == index.subReportTmpIndex) &&
                    (statSubTypeReportTmpTableList.index.subReportSubType == index.subReportSubType)))
        {
            if ((statSubTypeReportTmpTableList.flagsOfColumnSet & FLAG_STATSUBREPORTSCANTYPE) &&
                    (statSubTypeReportTmpTableList.flagsOfColumnSet & FLAG_STATSUBREPORTSTATUS))
            {
                *data =  SNMP_ROW_NOTINSERVICE;
            }
            else
            {
                *data =  SNMP_ROW_NOTREADY;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        *data = SNMP_ROW_ACTIVE;
    }

    return TRUE;
}

static int statSubReportRowStatus_set(stSubTypeReportTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSubReportRowStatus_get(index, &row_status);
    zlog_debug(SNMP_DBG_MIB_GET, "%s : ret(%d) row_status(%d) data(%d)\n\r", \
               __FUNCTION__, ret, row_status, data);

    if (ret == FALSE)
    {
        if (SNMP_ROW_CREATEANDWAIT == data)
        {
            statSubTypeReportTmpTableList.flagsOfColumnSet = FLAG_STATSUBREPORTROWSTATUS;
            //assign the index here
            statSubTypeReportTmpTableList.index.subReportTmpIndex = index.subReportTmpIndex;
            statSubTypeReportTmpTableList.index.subReportSubType  = index.subReportSubType;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if (SNMP_ROW_ACTIVE == data)
        {
            if (repStat.flag == MODIFY_TABLE_FLAG)
            {
                //fill your code here
                ret = statSubTypeReportTmpTabOperate(GPN_STAT_MSG_SUBREPORT_TEMP_MODIFY, index);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : modify statSubStatTypeReportTemplatTable failure\n\r", \
                               __FUNCTION__);
                    return FALSE;
                }

                memset(&repStat, 0, sizeof(reportTpStatus));
                memset(&subTypeReportTmpData, 0 , sizeof(stSubTypeReportTmpTab_info));
                memset(&statSubTypeReportTmpTableList, 0, sizeof(stSubTypeReportTmpTab_info));
            }
            else if (SNMP_ROW_NOTINSERVICE == row_status)
            {
                //fill your code here
                ret = statSubTypeReportTmpTabOperate(GPN_STAT_MSG_SUBREPORT_TEMP_ADD, index);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : add statSubTypeReportTmpTabOperate failure\n\r", \
                               __FUNCTION__);
                    return FALSE;
                }

                memset(&repStat, 0, sizeof(reportTpStatus));
                memset(&subTypeReportTmpData, 0 , sizeof(stSubTypeReportTmpTab_info));
                memset(&statSubTypeReportTmpTableList, 0, sizeof(stSubTypeReportTmpTab_info));
            }
            else
            {
                return FALSE;
            }
        }
        else if (SNMP_ROW_DESTROY == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //fill your code here
                ret = statSubTypeReportTmpTabOperate(GPN_STAT_MSG_SUBREPORT_TEMP_DELETE, index);

                if (ret == FALSE)
                {
                    zlog_debug(SNMP_DBG_MIB_GET, "%s : delete statAlarmThresholdTemplatTable failure\n\r", \
                               __FUNCTION__);
                    return FALSE;
                }

                memset(&repStat, 0, sizeof(reportTpStatus));
                memset(&subTypeReportTmpData, 0 , sizeof(stSubTypeReportTmpTab_info));
                memset(&statSubTypeReportTmpTableList, 0, sizeof(stSubTypeReportTmpTab_info));

            }
            else
            {
                memset(&repStat, 0, sizeof(reportTpStatus));
                memset(&subTypeReportTmpData, 0 , sizeof(stSubTypeReportTmpTab_info));
                memset(&statSubTypeReportTmpTableList, 0, sizeof(stSubTypeReportTmpTab_info));
            }
        }
        else if (SNMP_ROW_NOTINSERVICE == data)
        {
            //statSubTypeReportTmpTableList.column.statSubReportRowStatus = SNMP_ROW_NOTINSERVICE;
            repStat.flag = MODIFY_TABLE_FLAG;
            statSubTypeReportTmpTableList.flagsOfColumnSet = FLAG_STATSUBREPORTROWSTATUS;
        }
        else
        {
            memset(&subTypeReportTmpData, 0 , sizeof(stSubTypeReportTmpTab_info));
            return FALSE;
        }
    }

    return TRUE;
}



static int subReportScanType_set(stSubTypeReportTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSubReportRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (repStat.flag == MODIFY_TABLE_FLAG)
            {
                statSubTypeReportTmpTableList.data.subReportScanType = data;
                statSubTypeReportTmpTableList.flagsOfColumnSet |= FLAG_STATSUBREPORTSCANTYPE;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statSubTypeReportTmpTableList.data.subReportScanType = data;
            statSubTypeReportTmpTableList.flagsOfColumnSet |= FLAG_STATSUBREPORTSCANTYPE;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}

static int subReportStatus_set(stSubTypeReportTmpTab_index index, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = statSubReportRowStatus_get(index, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (repStat.flag == MODIFY_TABLE_FLAG)
            {
                statSubTypeReportTmpTableList.data.subReportStatus = data;
                statSubTypeReportTmpTableList.flagsOfColumnSet |= FLAG_STATSUBREPORTSTATUS;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            statSubTypeReportTmpTableList.data.subReportStatus = data;
            statSubTypeReportTmpTableList.flagsOfColumnSet |= FLAG_STATSUBREPORTSTATUS;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;


    //statTable_set(&info, SNMP_LONG_CYC_UP_THRESHOLD_HIGH_32) == FALSE
}



int write_subReportScanType(int action, u_char *var_val,
                            u_char var_val_type, size_t var_val_len,
                            u_char *statP, oid *name, size_t length)
{
    stSubTypeReportTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subReportTmpIndex = name[length - 2];
    index.subReportSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (subReportScanType_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_subReportStatus(int action, u_char *var_val,
                          u_char var_val_type, size_t var_val_len,
                          u_char *statP, oid *name, size_t length)
{
    stSubTypeReportTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subReportTmpIndex = name[length - 2];
    index.subReportSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (subReportStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int write_statSubReportRowStatus(int action, u_char *var_val,
                                 u_char var_val_type, size_t var_val_len,
                                 u_char *statP, oid *name, size_t length)
{
    stSubTypeReportTmpTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.subReportTmpIndex = name[length - 2];
    index.subReportSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : action(%d) (%d|%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__, action,
               RESERVE1, RESERVE2, ACTION, COMMIT, FREE, UNDO, FINISHED_SUCCESS, FINISHED_FAILURE);

    switch (action)
    {
        case RESERVE1:
            if (statSubReportRowStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}



int write_statHistDataRowStatus(int action, u_char *var_val,
                                u_char var_val_type, size_t var_val_len,
                                u_char *statP, oid *name, size_t length)
{
    stHistDataTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.histDataIndex = name[length - 3];
    index.histDataStopTime = name[length - 2];
    index.histStatType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
#if 0

    switch (action)
    {
        case RESERVE1:
            if (statHistDataRowStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

#endif

    return SNMP_ERR_NOERROR;
}



int write_statHistDataReportRowStatus(int action, u_char *var_val,
                                      u_char var_val_type, size_t var_val_len,
                                      u_char *statP, oid *name, size_t length)
{
    stHistDataReportFormatTab_index index;
    //int ret = 0;

    long i_data = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(i_data))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    /* validate the index */
    index.histDataReportTaskBaseIndex = name[length - 2];
    index.histDataReportSubType = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
#if 0

    switch (action)
    {
        case RESERVE1:
            if (statHistDataReportRowStatus_set(index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

#endif

    return SNMP_ERR_NOERROR;
}



