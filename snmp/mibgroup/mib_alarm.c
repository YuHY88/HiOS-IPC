/*
 *  alarm MIB group implementation - mib_alarm.c
 *
 */
/* Portions of this file are subject to the following copyright(s).  See
 * the Net-SNMP's COPYING file for more details and other copyrights
 * that may apply:
 */
/*
 * Portions of this file are copyrighted by:
 * Copyright ?2003 Sun Microsystems, Inc. All rights reserved.
 * Use is subject to license terms specified in the COPYING file
 * distributed with the Net-SNMP package.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "net-snmp-config.h"
#include <lib/types.h>
#include "types.h"

#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"

#include <lib/pkt_type.h>
#include <lib/linklist.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/ifm_common.h>
#include <lib/alarm.h>
#include <lib/log.h>

#include <lib/memtypes.h>
#include <lib/memshare.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>

#include "mib_alarm.h"
#include "alarm/gpnAlmSnmp.h"
#include "ipran_snmp_data_cache.h"
//#include "lib/snmp_common.h"
#include "snmp_config_table.h"

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

/*local temp variable*/
static uchar    str_value[STRING_LEN] = {'\0'};
static uchar    *str = NULL;
static uchar    mac_value[6] = {0};
static int      int_value = 0;
static uint32_t uint_value = 0;
static uint32_t ip_value = 0;


/* table list define */
static struct ipran_snmp_data_cache *alarmPortMonitorCfgTable_cache = NULL ;
static struct ipran_snmp_data_cache *alarmAttributeCfgTable_cache = NULL ;
static struct ipran_snmp_data_cache *alarmCurrDataSheetTable_cache = NULL ;
static struct ipran_snmp_data_cache *alarmHistDataSheetTable_cache = NULL ;
static struct ipran_snmp_data_cache *eventDataSheetTable_cache = NULL ;


stSnmpAlmPortMonitorCfgTableInfo almPortMonitorCfgTabList, almPortMonitorData;
stSnmpAlmAttribCfgTableInfo attribCfgTabList, attribCfgTabData;

/*status for modify the table*/
almPortMonitorCfgStatus almPortMoniCfgTabStat;
almAttriCfgStatus attribCfgTabStat;


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

static oid alarmDelayTime_oid[] = {ALARM_OIDPREFIX, alarmGolbalCfg, alarmDelayTime};
FindVarMethod alarmDelayTime_get;
struct variable1 alarmDelayTime_variables[] =
{
    {alarmProductDelayTime,     ASN_INTEGER,    RWRITE, alarmDelayTime_get, 1, {1}},
    {alarmDisappearDelayTime,   ASN_INTEGER,    RWRITE, alarmDelayTime_get, 1, {2}},
};


static oid alarmBuzzerCtrl_oid[] = {ALARM_OIDPREFIX, alarmGolbalCfg, alarmBuzzerCtl};
FindVarMethod alarmBuzzerCtrl_get;
struct variable1 alarmBuzzerCtrl_variables[] =
{
    {alarmBuzzerEn,             ASN_INTEGER,    RWRITE, alarmBuzzerCtrl_get, 1, {1}},
    {alarmBuzzerClear,          ASN_INTEGER,    RWRITE, alarmBuzzerCtrl_get, 1, {2}},
    {alarmBuzzerThreshold,      ASN_INTEGER,    RWRITE, alarmBuzzerCtrl_get, 1, {3}},
};


static oid alarmCurrAlmDSCtrl_oid[] = {ALARM_OIDPREFIX, alarmGolbalCfg, alarmDataSheetCtrl, alarmCurrAlmDSCtrl};
FindVarMethod alarmCurrAlmDSCtrl_get;
struct variable1 alarmCurrAlmDSCtrl_variables[] =
{
    {alarmCurrAlmDSSize,        ASN_INTEGER,    RONLY,  alarmCurrAlmDSCtrl_get, 1, {1}},
    {alarmCurrAlmDSCircle,      ASN_INTEGER,    RWRITE, alarmCurrAlmDSCtrl_get, 1, {2}},
};

static oid alarmHistAlmDSCtrl_oid[] = {ALARM_OIDPREFIX, alarmGolbalCfg, alarmDataSheetCtrl, alarmHistAlmDSCtrl};
FindVarMethod alarmHistAlmDSCtrl_get;
struct variable1 alarmHistAlmDSCtrl_variables[] =
{
    {alarmHistAlmDSSize,        ASN_INTEGER,    RONLY,  alarmHistAlmDSCtrl_get, 1, {1}},
    {alarmHistAlmDSCircle,      ASN_INTEGER,    RWRITE, alarmHistAlmDSCtrl_get, 1, {2}},
};


static oid alarmEventDSCtrl_oid[] = {ALARM_OIDPREFIX, alarmGolbalCfg, alarmDataSheetCtrl, alarmEventDSCtrl};
FindVarMethod alarmEventDSCtrl_get;
struct variable1 alarmEventDSCtrl_variables[] =
{
    {alarmEventDSSize,          ASN_INTEGER,    RONLY,  alarmEventDSCtrl_get, 1, {1}},
    {alarmEventDSCircle,        ASN_INTEGER,    RWRITE, alarmEventDSCtrl_get, 1, {2}},
};

static oid alarmAttribCtrl_oid[] = {ALARM_OIDPREFIX, alarmGolbalCfg, alarmDataSheetCtrl, alarmAttriCtrl};
FindVarMethod alarmAttribCtrl_get;
struct variable1 alarmAttribCtrl_variables[] =
{
    {alarmAttribCfgNum,         ASN_INTEGER,    RONLY,  alarmAttribCtrl_get, 1, {1}},
};

static oid alarmPoerBaseMonitorCtrl_oid[] = {ALARM_OIDPREFIX, alarmGolbalCfg, alarmDataSheetCtrl, alarmPoerBaseMonitorCtrl};
FindVarMethod alarmPoerBaseMonitorCtrl_get;
struct variable1 alarmPoerBaseMonitorCtrl_variables[] =
{
    {alarmPoerBaseMonitorCfgNum, ASN_INTEGER,   RONLY,  alarmPoerBaseMonitorCtrl_get, 1, {1}},
};

static oid alarmGlobalCfg_oid[] = {ALARM_OIDPREFIX, alarmGolbalCfg};
FindVarMethod alarmGlobalCfg_get;
struct variable1 alarmGlobalCfg_variables[] =
{
    {alarmRestrain,             ASN_INTEGER,    RWRITE, alarmGlobalCfg_get, 1, {4}},
    {alarmReversalMode,         ASN_INTEGER,    RWRITE, alarmGlobalCfg_get, 1, {5}},
};


static oid alarmPortMonitorCfgTable_oid[] = {ALARM_OIDPREFIX, alarmPortMonitorCfg, alarmPortMonitorCfgTable};
FindVarMethod alarmPortMonitorCfgTable_get;
struct variable2 alarmPortMonitorCfgTable_variables[] =
{
    /*{monDevIndex,               ASN_UNSIGNED,    RONLY,  alarmPortMonitorCfgTable_get, 2, {1, 1}},
    {monPortIndex1,             ASN_UNSIGNED,    RONLY,  alarmPortMonitorCfgTable_get, 2, {1, 2}},
    {monPortIndex2,             ASN_UNSIGNED,    RONLY,  alarmPortMonitorCfgTable_get, 2, {1, 3}},
    {monPortIndex3,             ASN_UNSIGNED,    RONLY,  alarmPortMonitorCfgTable_get, 2, {1, 4}},
    {monPortIndex4,             ASN_UNSIGNED,    RONLY,  alarmPortMonitorCfgTable_get, 2, {1, 5}},
    {monPortIndex5,             ASN_UNSIGNED,    RONLY,  alarmPortMonitorCfgTable_get, 2, {1, 6}},*/
    {alarmMonitorEn,            ASN_INTEGER,    RWRITE, alarmPortMonitorCfgTable_get, 2, {1, 7}},
    {alarmRowStatus,            ASN_INTEGER,    RWRITE, alarmPortMonitorCfgTable_get, 2, {1, 8}}
};


static oid alarmAttributeCfgTable_oid[] = {ALARM_OIDPREFIX, alarmAttributeCfg, alarmAttributeCfgTable};
FindVarMethod alarmAttributeCfgTable_get;
struct variable2 alarmAttributeCfgTable_variables[] =
{
    /*{alarmSubType,              ASN_UNSIGNED,    RONLY,  alarmAttributeCfgTable_get, 2, {1, 1}},
    {attribDevIndex,            ASN_UNSIGNED,    RONLY,  alarmAttributeCfgTable_get, 2, {1, 2}},
    {attribPortIndex1,          ASN_UNSIGNED,    RONLY,  alarmAttributeCfgTable_get, 2, {1, 3}},
    {attribPortIndex2,          ASN_UNSIGNED,    RONLY,  alarmAttributeCfgTable_get, 2, {1, 4}},
    {attribPortIndex3,          ASN_UNSIGNED,    RONLY,  alarmAttributeCfgTable_get, 2, {1, 5}},
    {attribPortIndex4,          ASN_UNSIGNED,    RONLY,  alarmAttributeCfgTable_get, 2, {1, 6}},
    {attribPortIndex5,          ASN_UNSIGNED,    RONLY,  alarmAttributeCfgTable_get, 2, {1, 7}},*/
    {basePortMark,              ASN_INTEGER,    RWRITE, alarmAttributeCfgTable_get, 2, {1, 8}},
    {alarmLevel,                ASN_INTEGER,    RWRITE, alarmAttributeCfgTable_get, 2, {1, 9}},
    {alarmMask,                 ASN_INTEGER,    RWRITE, alarmAttributeCfgTable_get, 2, {1, 10}},
    {alarmFilter,               ASN_INTEGER,    RWRITE, alarmAttributeCfgTable_get, 2, {1, 11}},
    {alarmReport,               ASN_INTEGER,    RWRITE, alarmAttributeCfgTable_get, 2, {1, 12}},
    {alarmRecord,               ASN_INTEGER,    RWRITE, alarmAttributeCfgTable_get, 2, {1, 13}},
    {portAlmReverse,            ASN_INTEGER,    RWRITE, alarmAttributeCfgTable_get, 2, {1, 14}},
    {attribRowStatus,           ASN_INTEGER,    RWRITE, alarmAttributeCfgTable_get, 2, {1, 15}},
};


static oid alarmCurrDataSheetTable_oid[] = {ALARM_OIDPREFIX, alarmCurrDataSheet, alarmCurrDataSheetTable};
FindVarMethod alarmCurrDataSheetTable_get;
struct variable2 alarmCurrDataSheetTable_variables[] =
{
    //{currAlarmDSIndex,          ASN_UNSIGNED,    RONLY, alarmCurrDataSheetTable_get, 2, {1, 1}},
    {currAlarmType,             ASN_UNSIGNED,    RONLY, alarmCurrDataSheetTable_get, 2, {1, 2}},
    {currDevIndex,              ASN_UNSIGNED,    RONLY, alarmCurrDataSheetTable_get, 2, {1, 3}},
    {currPortIndex1,            ASN_UNSIGNED,    RONLY, alarmCurrDataSheetTable_get, 2, {1, 4}},
    {currPortIndex2,            ASN_UNSIGNED,    RONLY, alarmCurrDataSheetTable_get, 2, {1, 5}},
    {currPortIndex3,            ASN_UNSIGNED,    RONLY, alarmCurrDataSheetTable_get, 2, {1, 6}},
    {currPortIndex4,            ASN_UNSIGNED,    RONLY, alarmCurrDataSheetTable_get, 2, {1, 7}},
    {currPortIndex5,            ASN_UNSIGNED,    RONLY, alarmCurrDataSheetTable_get, 2, {1, 8}},
    {currAlarmLevel,            ASN_INTEGER,     RONLY, alarmCurrDataSheetTable_get, 2, {1, 9}},
    {alarmCurrProductCnt,       ASN_UNSIGNED,    RONLY, alarmCurrDataSheetTable_get, 2, {1, 10}},
    {alarmCurrFirstProductTime, ASN_UNSIGNED,    RONLY, alarmCurrDataSheetTable_get, 2, {1, 11}},
    {alarmCurrThisProductTime,  ASN_UNSIGNED,    RONLY, alarmCurrDataSheetTable_get, 2, {1, 12}},
    {alarmCurrRowStatus,        ASN_INTEGER,     RONLY, alarmCurrDataSheetTable_get, 2, {1, 13}},
};


static oid alarmHistDataSheetTable_oid[] = {ALARM_OIDPREFIX, alarmHistDataSheet, alarmHistDataSheetTable};
FindVarMethod alarmHistDataSheetTable_get;
struct variable2 alarmHistDataSheetTable_variables[] =
{
    //{histAlarmDSIndex,          ASN_UNSIGNED,    RONLY, alarmHistDataSheetTable_get, 2, {1, 1}},
    {histAlarmType,             ASN_UNSIGNED,    RONLY, alarmHistDataSheetTable_get, 2, {1, 2}},
    {histDevIndex,              ASN_UNSIGNED,    RONLY, alarmHistDataSheetTable_get, 2, {1, 3}},
    {histPortIndex1,            ASN_UNSIGNED,    RONLY, alarmHistDataSheetTable_get, 2, {1, 4}},
    {histPortIndex2,            ASN_UNSIGNED,    RONLY, alarmHistDataSheetTable_get, 2, {1, 5}},
    {histPortIndex3,            ASN_UNSIGNED,    RONLY, alarmHistDataSheetTable_get, 2, {1, 6}},
    {histPortIndex4,            ASN_UNSIGNED,    RONLY, alarmHistDataSheetTable_get, 2, {1, 7}},
    {histPortIndex5,            ASN_UNSIGNED,    RONLY, alarmHistDataSheetTable_get, 2, {1, 8}},
    {histAlarmLevel,            ASN_INTEGER,     RONLY, alarmHistDataSheetTable_get, 2, {1, 9}},
    {alarmHistProductCnt,       ASN_UNSIGNED,    RONLY, alarmHistDataSheetTable_get, 2, {1, 10}},
    {alarmHistFirstProductTime, ASN_UNSIGNED,    RONLY, alarmHistDataSheetTable_get, 2, {1, 11}},
    {alarmHistThisProductTime,  ASN_UNSIGNED,    RONLY, alarmHistDataSheetTable_get, 2, {1, 12}},
    {alarmHistThisDisappearTime, ASN_UNSIGNED,   RONLY, alarmHistDataSheetTable_get, 2, {1, 13}},
    {alarmHistRowStatus,        ASN_INTEGER,     RONLY, alarmHistDataSheetTable_get, 2, {1, 14}},
};


static oid eventDataSheetTable_oid[] = {ALARM_OIDPREFIX, eventDataSheet, eventDataSheetTable};
FindVarMethod eventDataSheetTable_get;
struct variable2 eventDataSheetTable_variables[] =
{
    //{eventDSIndex,              ASN_UNSIGNED,    RONLY, eventDataSheetTable_get, 2, {1, 1}},
    {eventAlarmType,            ASN_UNSIGNED,    RONLY, eventDataSheetTable_get, 2, {1, 2}},
    {eventDevIndex,             ASN_UNSIGNED,    RONLY, eventDataSheetTable_get, 2, {1, 3}},
    {eventPortIndex1,           ASN_UNSIGNED,    RONLY, eventDataSheetTable_get, 2, {1, 4}},
    {eventPortIndex2,           ASN_UNSIGNED,    RONLY, eventDataSheetTable_get, 2, {1, 5}},
    {eventPortIndex3,           ASN_UNSIGNED,    RONLY, eventDataSheetTable_get, 2, {1, 6}},
    {eventPortIndex4,           ASN_UNSIGNED,    RONLY, eventDataSheetTable_get, 2, {1, 7}},
    {eventPortIndex5,           ASN_UNSIGNED,    RONLY, eventDataSheetTable_get, 2, {1, 8}},
    {eventAlarmLevel,           ASN_INTEGER,     RONLY, eventDataSheetTable_get, 2, {1, 9}},
    {eventDetail,               ASN_INTEGER,     RONLY, eventDataSheetTable_get, 2, {1, 10}},
    {eventTime,                 ASN_UNSIGNED,    RONLY, eventDataSheetTable_get, 2, {1, 11}},
    {eventRowStatus,            ASN_INTEGER,     RONLY, eventDataSheetTable_get, 2, {1, 12}},
};



/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
void  init_mib_alarm(void)
{
    REGISTER_MIB("alarmDelayTime", alarmDelayTime_variables, variable1, alarmDelayTime_oid);

    REGISTER_MIB("alarmBuzzerCtrl", alarmBuzzerCtrl_variables, variable1, alarmBuzzerCtrl_oid);

    REGISTER_MIB("alarmCurrAlmDSCtrl", alarmCurrAlmDSCtrl_variables, variable1, alarmCurrAlmDSCtrl_oid);

    REGISTER_MIB("alarmHistAlmDSCtrl", alarmHistAlmDSCtrl_variables, variable1, alarmHistAlmDSCtrl_oid);

    REGISTER_MIB("alarmEventDSCtrl", alarmEventDSCtrl_variables, variable1, alarmEventDSCtrl_oid);

    REGISTER_MIB("alarmAttribCtrl", alarmAttribCtrl_variables, variable1, alarmAttribCtrl_oid);

    REGISTER_MIB("alarmPoerBaseMonitorCtrl", alarmPoerBaseMonitorCtrl_variables, variable1, alarmPoerBaseMonitorCtrl_oid);

    REGISTER_MIB("alarmGlobalCfg", alarmGlobalCfg_variables, variable1, alarmGlobalCfg_oid);

    REGISTER_MIB("alarmPortMonitorCfgTable", alarmPortMonitorCfgTable_variables, variable2, alarmPortMonitorCfgTable_oid);

    REGISTER_MIB("alarmAttributeCfgTable", alarmAttributeCfgTable_variables, variable2, alarmAttributeCfgTable_oid);

    REGISTER_MIB("alarmCurrDataSheetTable", alarmCurrDataSheetTable_variables, variable2, alarmCurrDataSheetTable_oid);

    REGISTER_MIB("alarmHistDataSheetTable", alarmHistDataSheetTable_variables, variable2, alarmHistDataSheetTable_oid);

    REGISTER_MIB("eventDataSheetTable", eventDataSheetTable_variables, variable2, eventDataSheetTable_oid);
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

/************************ ipc msg request ************************/

static uint32_t alarm_mib_get_scalar(uint8_t *pData, int module_id, uint32_t msg_type, uint16_t subtype, uint8_t opcode)
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


static void alarm_mib_set_scalar(uint8_t *pData, uint32_t data_len, int module_id, uint32_t msg_type, uint16_t subtype, uint8_t opcode)
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

	memcpy(pMsgSnd->msg_data, (uint8_t *)pData, sizeof(uint32_t));

    iRetv = ipc_send_msg_n1(pMsgSnd, (sizeof(struct ipc_msghdr_n) + data_len));

    if(-1 == iRetv)	
    {
		mem_share_free(pMsgSnd, module_id);
    }
}



/* 获取整型标量数据 */
static uint32_t alarmScalarInt_get(uint8_t *pData, int subtype)
{
    /*return ipc_send_common_wait_reply(NULL, 0, 1, MODULE_ID_ALARM, MODULE_ID_SNMPD,
                                      IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET, 0);*/

	uint32_t recv_len = alarm_mib_get_scalar(pData, MODULE_ID_ALARM, IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET);

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


/* 获取整型标量数据 */
static uint32_t alarmScalarStr_get(uint8_t *pData, int subtype)
{
    /*return ipc_send_common_wait_reply(NULL, 0, 1, MODULE_ID_ALARM, MODULE_ID_SNMPD,
                                      IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET, 0);*/

	return alarm_mib_get_scalar(pData, MODULE_ID_ALARM, IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET);
}





/* 配置标量数据 */
static void alarmScalarInt_set(int subtype, uint32_t val)
{
    alarm_mib_set_scalar((uint8_t *)&val, sizeof(uint32_t), MODULE_ID_ALARM, IPC_TYPE_SNMP, subtype, IPC_OPCODE_UPDATE);
}


#if 0

static uint8_t *alarmPortMoniCfgTab_get(stSnmpAlmPortMonitorCfgTableIndex *pIndex)
{
    return ipc_send_common_wait_reply(pIndex, sizeof(stSnmpAlmPortMonitorCfgTableIndex), 1,
				MODULE_ID_ALARM, MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_ALM_PORT_MONITOR_CFG_TABLE,
				IPC_OPCODE_GET, 0);
}



static int alarmPortMoniCfgTab_set(stSnmpAlmPortMonitorCfgTableIndex *pIndex)
{
    stSnmpAlmPortMonitorCfgTableInfo *pInfo_get = NULL;
    stSnmpAlmPortMonitorCfgTableInfo info_send;

    if (almPortMonitorCfgTabList.flagsOfColumnSet & FLAG_ALARMMONITOREN)
    {
        pInfo_get = (stSnmpAlmPortMonitorCfgTableInfo *)alarmPortMoniCfgTab_get(pIndex);

        if (pInfo_get)
        {
            memset(&info_send, 0, sizeof(stSnmpAlmPortMonitorCfgTableInfo));
            memcpy(&info_send, pInfo_get, sizeof(stSnmpAlmPortMonitorCfgTableInfo));

            info_send.data.iAlmMonitorEN = almPortMonitorCfgTabList.data.iAlmMonitorEN;

            ipc_send_common(&info_send, sizeof(stSnmpAlmPortMonitorCfgTableInfo), 1, MODULE_ID_ALARM, MODULE_ID_SNMPD,
                            IPC_TYPE_SNMP, SNMP_ALM_PORT_MONITOR_CFG_TABLE, IPC_OPCODE_UPDATE);

            attribCfgTabList.flagsOfColumnSet = 0;
            return TRUE;
        }
        else
        {
            attribCfgTabList.flagsOfColumnSet = 0;
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

static int alarmMonitorEn_set(stSnmpAlmPortMonitorCfgTableIndex *pIndex, int val)
{
    almPortMonitorCfgTabList.flagsOfColumnSet |= FLAG_ALARMMONITOREN;
    almPortMonitorCfgTabList.data.iAlmMonitorEN = val;

    alarmPortMoniCfgTab_set(pIndex);
    return TRUE;
}



static uint8_t *alarmAttribCfgTab_get(stSnmpAlmAttribCfgTableIndex *pIndex)
{
    return ipc_send_common_wait_reply(pIndex, sizeof(stSnmpAlmAttribCfgTableIndex), 1,
                                      MODULE_ID_ALARM, MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_ALM_ATTRIBUTE_CFG_TABLE,
                                      IPC_OPCODE_GET, 0);
}


static int alarmAttribCfgTab_set(stSnmpAlmAttribCfgTableIndex *pIndex)
{
    stSnmpAlmAttribCfgTableInfo *pInfo_get = NULL;
    stSnmpAlmAttribCfgTableInfo info_send;

    if (attribCfgTabList.flagsOfColumnSet & FLAG_BASEPORTMARK)
    {
        pInfo_get = (stSnmpAlmAttribCfgTableInfo *)alarmAttribCfgTab_get(pIndex);

        if (pInfo_get)
        {
            memset(&info_send, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
            memcpy(&info_send, pInfo_get, sizeof(stSnmpAlmAttribCfgTableInfo));

            info_send.data.iBasePortMark = attribCfgTabList.data.iBasePortMark;

            if (attribCfgTabList.flagsOfColumnSet & FLAG_ALARMLEVEL)
            {
                info_send.data.iAlmLevel = attribCfgTabList.data.iAlmLevel;
            }

            if (attribCfgTabList.flagsOfColumnSet & FLAG_ALARMMASK)
            {
                info_send.data.iAlmLevel = attribCfgTabList.data.iAlmMask;
            }

            if (attribCfgTabList.flagsOfColumnSet & FLAG_ALARMFILTER)
            {
                info_send.data.iAlmLevel = attribCfgTabList.data.iAlmFilter;
            }

            if (attribCfgTabList.flagsOfColumnSet & FLAG_ALARMREPORT)
            {
                info_send.data.iAlmLevel = attribCfgTabList.data.iAlmReport;
            }

            if (attribCfgTabList.flagsOfColumnSet & FLAG_ALARMRECORD)
            {
                info_send.data.iAlmLevel = attribCfgTabList.data.iAlmRecord;
            }

            if (attribCfgTabList.flagsOfColumnSet & FLAG_PORTALMREVERSE)
            {
                info_send.data.iAlmLevel = attribCfgTabList.data.iPortAlmReverse;
            }

            ipc_send_common(&info_send, sizeof(stSnmpAlmAttribCfgTableInfo), 1, MODULE_ID_ALARM, MODULE_ID_SNMPD,
                            IPC_TYPE_SNMP, SNMP_ALM_ATTRIBUTE_CFG_TABLE, IPC_OPCODE_UPDATE);

            attribCfgTabList.flagsOfColumnSet = 0;
            return TRUE;
        }
        else
        {
            attribCfgTabList.flagsOfColumnSet = 0;
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

static int alarmBasePortMark_set(stSnmpAlmAttribCfgTableIndex *pIndex, int val)
{
    attribCfgTabList.flagsOfColumnSet |= FLAG_BASEPORTMARK;
    attribCfgTabList.data.iBasePortMark = val;

    alarmAttribCfgTab_set(pIndex);
    return TRUE;
}


static int alarmLevel_set(stSnmpAlmAttribCfgTableIndex *pIndex, int val)
{
    attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMLEVEL;
    attribCfgTabList.data.iAlmLevel = val;

    alarmAttribCfgTab_set(pIndex);
    return TRUE;
}


static int alarmMask_set(stSnmpAlmAttribCfgTableIndex *pIndex, int val)
{
    attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMMASK;
    attribCfgTabList.data.iAlmMask = val;

    alarmAttribCfgTab_set(pIndex);
    return TRUE;
}


static int alarmFilter_set(stSnmpAlmAttribCfgTableIndex *pIndex, int val)
{
    attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMFILTER;
    attribCfgTabList.data.iAlmFilter = val;

    alarmAttribCfgTab_set(pIndex);
    return TRUE;
}

static int alarmReport_set(stSnmpAlmAttribCfgTableIndex *pIndex, int val)
{
    attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMREPORT;
    attribCfgTabList.data.iAlmReport = val;

    alarmAttribCfgTab_set(pIndex);
    return TRUE;
}


static int alarmRecord_set(stSnmpAlmAttribCfgTableIndex *pIndex, int val)
{
    attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMRECORD;
    attribCfgTabList.data.iAlmRecord = val;

    alarmAttribCfgTab_set(pIndex);
    return TRUE;
}

static int alarmReverse_set(stSnmpAlmAttribCfgTableIndex *pIndex, int val)
{
    attribCfgTabList.flagsOfColumnSet |= FLAG_PORTALMREVERSE;
    attribCfgTabList.data.iPortAlmReverse = val;

    alarmAttribCfgTab_set(pIndex);
    return TRUE;
}

#endif



/* 批量返回表信息，返回值是表的指针，*pdata_num 返回实际的表数量 */
static struct ipc_mesg_n*alarmTable_get_bulk(void *pIndex, int module_id, int subtype)
{
    int index_len = 0;

    switch (subtype)
    {
        case SNMP_ALM_PORT_MONITOR_CFG_TABLE:
            index_len = sizeof(stSnmpAlmPortMonitorCfgTableIndex);
            break;

        case SNMP_ALM_ATTRIBUTE_CFG_TABLE:
            index_len = sizeof(stSnmpAlmAttribCfgTableIndex);
            break;

        case SNMP_ALM_CURR_DATA_SHEET_TABLE:
            index_len = sizeof(stSnmpAlmCurrDSTableIndex);
            break;

        case SNMP_ALM_HIST_DATA_SHEET_TABLE:
            index_len = sizeof(stSnmpAlmHistDSTableIndex);
            break;

        case SNMP_ALM_EVENT_DATA_SHEET_TABLE:
            index_len = sizeof(stSnmpEventDSTableIndex);
            break;

        default:
            break;
    }

    /*struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(pIndex, index_len, 1 , module_id, MODULE_ID_SNMPD,
                                                         IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET_BULK, 0);*/

    return ipc_sync_send_n2((void *)pIndex, index_len, 1, module_id, MODULE_ID_SNMPD, IPC_TYPE_SNMP, subtype, 
							IPC_OPCODE_GET_BULK, 0, 1000);
}


/* 获取 alarmPortMonitorCfgTable 数据 */
int alarmPortMonitorCfgTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                               stSnmpAlmPortMonitorCfgTableInfo *pinfo_input)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
	
    stSnmpAlmPortMonitorCfgTableInfo *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if ((0 == pinfo_input->index.iDevIndex) &&\
		(0 == pinfo_input->index.iPortIndex1) &&\
		(0 == pinfo_input->index.iPortIndex2) &&\
		(0 == pinfo_input->index.iPortIndex3) &&\
		(0 == pinfo_input->index.iPortIndex4) &&\
		(0 == pinfo_input->index.iPortIndex5))
    {
        pinfo_input->index.iDevIndex = 0xffffffff;
        pinfo_input->index.iPortIndex1 = 0xffffffff;
        pinfo_input->index.iPortIndex2 = 0xffffffff;
        pinfo_input->index.iPortIndex3 = 0xffffffff;
        pinfo_input->index.iPortIndex4 = 0xffffffff;
        pinfo_input->index.iPortIndex5 = 0xffffffff;
    }

    pMsgRcv = alarmTable_get_bulk(&pinfo_input->index, MODULE_ID_ALARM, SNMP_ALM_PORT_MONITOR_CFG_TABLE);

	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index(%x|%x|%x|%x|%x|%x), data_num(%d)\n", __FILE__, __LINE__, __func__,
       pinfo_input->index.iDevIndex,
       pinfo_input->index.iPortIndex1,
       pinfo_input->index.iPortIndex2,
       pinfo_input->index.iPortIndex3,
       pinfo_input->index.iPortIndex4,
       pinfo_input->index.iPortIndex5, data_num);


	pinfo = (stSnmpAlmPortMonitorCfgTableInfo *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stSnmpAlmPortMonitorCfgTableInfo));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

	return TRUE;
}

stSnmpAlmPortMonitorCfgTableInfo *alarmPortMonitorCfgTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                       int exact, const stSnmpAlmPortMonitorCfgTableIndex *pIndex)
{
    struct listnode  *node, *nnode;

    stSnmpAlmPortMonitorCfgTableInfo *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((0xffffffff == pIndex->iDevIndex)\
                && (0xffffffff == pIndex->iPortIndex1)\
                && (0xffffffff == pIndex->iPortIndex2)\
                && (0xffffffff == pIndex->iPortIndex3)\
                && (0xffffffff == pIndex->iPortIndex4)\
                && (0xffffffff == pIndex->iPortIndex5))
        {
            return (stSnmpAlmPortMonitorCfgTableInfo *)cache->data_list->head->data ;
        }

        if ((pinfo_find->index.iDevIndex == pIndex->iDevIndex)\
                && (pinfo_find->index.iPortIndex1 == pIndex->iPortIndex1)\
                && (pinfo_find->index.iPortIndex2 == pIndex->iPortIndex2)\
                && (pinfo_find->index.iPortIndex3 == pIndex->iPortIndex3)\
                && (pinfo_find->index.iPortIndex4 == pIndex->iPortIndex4)\
                && (pinfo_find->index.iPortIndex5 == pIndex->iPortIndex5))
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
                    return (stSnmpAlmPortMonitorCfgTableInfo *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}



/* 获取 alarmAttributeCfgTable 数据 */
int alarmAttributeCfgTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                             stSnmpAlmAttribCfgTableInfo *pinfo_input)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
	
    stSnmpAlmAttribCfgTableInfo *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if ((0 == pinfo_input->index.iAlmSubType) &&\
        (0 == pinfo_input->index.iAttrDevIndex) &&
        (0 == pinfo_input->index.iAttrPortIndex1) &&\
        (0 == pinfo_input->index.iAttrPortIndex2) &&\
        (0 == pinfo_input->index.iAttrPortIndex3) &&\
        (0 == pinfo_input->index.iAttrPortIndex4) &&\
        (0 == pinfo_input->index.iAttrPortIndex5))
    {
        pinfo_input->index.iAlmSubType     = 0xffffffff;
        pinfo_input->index.iAttrDevIndex   = 0xffffffff;
        pinfo_input->index.iAttrPortIndex1 = 0xffffffff;
        pinfo_input->index.iAttrPortIndex2 = 0xffffffff;
        pinfo_input->index.iAttrPortIndex3 = 0xffffffff;
        pinfo_input->index.iAttrPortIndex4 = 0xffffffff;
        pinfo_input->index.iAttrPortIndex5 = 0xffffffff;
    }

    pMsgRcv = alarmTable_get_bulk(&pinfo_input->index, MODULE_ID_ALARM, SNMP_ALM_ATTRIBUTE_CFG_TABLE);

	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }

	zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index(%x|%x|%x|%x|%x|%x|%x), data_num(%d)\n", __FILE__, __LINE__, __func__,
       pinfo_input->index.iAlmSubType,
       pinfo_input->index.iAttrDevIndex,
       pinfo_input->index.iAttrPortIndex1,
       pinfo_input->index.iAttrPortIndex2,
       pinfo_input->index.iAttrPortIndex3,
       pinfo_input->index.iAttrPortIndex4,
       pinfo_input->index.iAttrPortIndex5, data_num);

	pinfo = (stSnmpAlmAttribCfgTableInfo *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stSnmpAlmAttribCfgTableInfo));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

    return TRUE;
}

stSnmpAlmAttribCfgTableInfo *alarmAttributeCfgTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                int exact, const stSnmpAlmAttribCfgTableIndex *pIndex)
{
    struct listnode  *node, *nnode;

    stSnmpAlmAttribCfgTableInfo *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((0xffffffff == pIndex->iAlmSubType)\
                && (0xffffffff == pIndex->iAttrDevIndex)\
                && (0xffffffff == pIndex->iAttrPortIndex1)\
                && (0xffffffff == pIndex->iAttrPortIndex2)\
                && (0xffffffff == pIndex->iAttrPortIndex3)\
                && (0xffffffff == pIndex->iAttrPortIndex4)\
                && (0xffffffff == pIndex->iAttrPortIndex5))
        {
            return (stSnmpAlmAttribCfgTableInfo *)cache->data_list->head->data ;
        }

        if ((pinfo_find->index.iAlmSubType == pIndex->iAlmSubType)\
                && (pinfo_find->index.iAttrDevIndex == pIndex->iAttrDevIndex)\
                && (pinfo_find->index.iAttrPortIndex1 == pIndex->iAttrPortIndex1)\
                && (pinfo_find->index.iAttrPortIndex2 == pIndex->iAttrPortIndex2)\
                && (pinfo_find->index.iAttrPortIndex3 == pIndex->iAttrPortIndex3)\
                && (pinfo_find->index.iAttrPortIndex4 == pIndex->iAttrPortIndex4)\
                && (pinfo_find->index.iAttrPortIndex5 == pIndex->iAttrPortIndex5))
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
                    return (stSnmpAlmAttribCfgTableInfo *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 alarmCurrDataSheetTable 数据 */
int alarmCurrDataSheetTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                              stSnmpAlmCurrDSTableInfo *pinfo_input)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
	
    stSnmpAlmCurrDSTableInfo *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if (0 == pinfo_input->index.iAlmCurrDSIndex)
    {
        pinfo_input->index.iAlmCurrDSIndex = 0xffffffff;
    }

    pMsgRcv = alarmTable_get_bulk(&pinfo_input->index, MODULE_ID_ALARM, SNMP_ALM_CURR_DATA_SHEET_TABLE);

	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }

	zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ifindex [%x] data_num [%d]\n", __FILE__, __LINE__, __func__,
		pinfo_input->index.iAlmCurrDSIndex, data_num);

	pinfo = (stSnmpAlmCurrDSTableInfo *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stSnmpAlmCurrDSTableInfo));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

	return TRUE;
}

stSnmpAlmCurrDSTableInfo *alarmCurrDataSheetTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                              int exact, const stSnmpAlmCurrDSTableIndex *index)
{
    struct listnode  *node, *nnode;

    stSnmpAlmCurrDSTableInfo *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || (0 == index->iAlmCurrDSIndex))
        {
            return (stSnmpAlmCurrDSTableInfo *)cache->data_list->head->data ;
        }

        if (pinfo_find->index.iAlmCurrDSIndex == index->iAlmCurrDSIndex)
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
                    return (stSnmpAlmCurrDSTableInfo *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 alarmHistDataSheetTable 数据 */
int alarmHistDataSheetTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                              stSnmpAlmHistDSTableInfo *pinfo_input)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
	
    stSnmpAlmHistDSTableInfo *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if (0 == pinfo_input->index.iAlmHistDSIndex)
    {
        pinfo_input->index.iAlmHistDSIndex = 0xffffffff;
    }

    pMsgRcv = alarmTable_get_bulk(&pinfo_input->index, MODULE_ID_ALARM, SNMP_ALM_HIST_DATA_SHEET_TABLE);

	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }

	zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ifindex [%x] data_num [%d]\n", __FILE__, __LINE__, __func__,
		pinfo_input->index.iAlmHistDSIndex, data_num);

	pinfo = (stSnmpAlmHistDSTableInfo *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stSnmpAlmHistDSTableInfo));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

	return TRUE;
}

stSnmpAlmHistDSTableInfo *alarmHistDataSheetTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                              int exact, const stSnmpAlmHistDSTableIndex *index)
{
    struct listnode  *node, *nnode;

    stSnmpAlmHistDSTableInfo *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || (0 == index->iAlmHistDSIndex))
        {
            return (stSnmpAlmHistDSTableInfo *)cache->data_list->head->data ;
        }

        if (pinfo_find->index.iAlmHistDSIndex == index->iAlmHistDSIndex)
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
                    return (stSnmpAlmHistDSTableInfo *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


/* 获取 eventDataSheetTable 数据 */
int eventDataSheetTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                          stSnmpEventDSTableInfo *pinfo_input)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
	
    stSnmpEventDSTableInfo *pinfo = NULL;
    int data_num = 0;
    int ret = 0;

    if (0 == pinfo_input->index.iEventDSIndex)
    {
        pinfo_input->index.iEventDSIndex = 0xffffffff;
    }

    pMsgRcv = alarmTable_get_bulk(&pinfo_input->index, MODULE_ID_ALARM, SNMP_ALM_EVENT_DATA_SHEET_TABLE);

	if(NULL == pMsgRcv)
	{		
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if (0 == data_num)
    {
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }

	zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ifindex [%x] data_num [%d]\n", __FILE__, __LINE__, __func__,
		pinfo_input->index.iEventDSIndex, data_num);

	pinfo = (stSnmpEventDSTableInfo *)pMsgRcv->msg_data;
	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pinfo , sizeof(stSnmpEventDSTableInfo));
        pinfo++;
    }

	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

	return TRUE;
}

stSnmpEventDSTableInfo *eventDataSheetTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                        int exact, const stSnmpEventDSTableIndex *index)
{
    struct listnode  *node, *nnode;

    stSnmpEventDSTableInfo *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || (0 == index->iEventDSIndex))
        {
            return (stSnmpEventDSTableInfo *)cache->data_list->head->data ;
        }

        if (pinfo_find->index.iEventDSIndex == index->iEventDSIndex)
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
                    return (stSnmpEventDSTableInfo *)node->next->data ;
                }
            }
        }
    }

    return NULL;
}


u_char *alarmDelayTime_get(struct variable *vp,
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
        case alarmProductDelayTime:
            *write_method = write_alarmProductDelayTime;
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_PRODUCT_DELAY_TIME);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case alarmDisappearDelayTime:
            *write_method = write_alarmDisappearDelayTime;
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_DISAPPEAR_DELAY_TIME);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }
}

u_char *alarmBuzzerCtrl_get(struct variable *vp,
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
        case alarmBuzzerEn:
            *write_method = write_alarmBuzzerEn;
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_BUZZER_EN);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case alarmBuzzerClear:
            *write_method = write_alarmBuzzerClear;
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_BUZZER_CLEAR);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case alarmBuzzerThreshold:
            *write_method = write_alarmBuzzerThreshold;
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_BUZZER_THRESHOLD);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }
}


u_char *alarmCurrAlmDSCtrl_get(struct variable *vp,
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
        case alarmCurrAlmDSSize:
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_CURR_ALM_DS_SIZE);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case alarmCurrAlmDSCircle:
            *write_method = write_alarmCurrAlmDSCircle;
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_CURR_ALM_DS_CIRCLE);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }
}

u_char *alarmHistAlmDSCtrl_get(struct variable *vp,
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
        case alarmHistAlmDSSize:
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_HIST_ALM_DS_SIZE);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case alarmHistAlmDSCircle:
            *write_method = write_alarmHistAlmDSCircle;
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_HIST_ALM_DS_CIRCLE);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }
}

u_char *alarmEventDSCtrl_get(struct variable *vp,
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
        case alarmEventDSSize:
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_EVENT_DS_SIZE);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case alarmEventDSCircle:
            *write_method = write_alarmEventDSCircle;
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_EVENT_DS_CIRCLE);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }
}

u_char *alarmAttribCtrl_get(struct variable *vp,
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
        case alarmAttribCfgNum:
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_ATTRIB_CFG_NUM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }
}

u_char *alarmPoerBaseMonitorCtrl_get(struct variable *vp,
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
        case alarmPoerBaseMonitorCfgNum:
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_PORT_BASE_MONITOR_CFG_NUM);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }
}

u_char *alarmGlobalCfg_get(struct variable *vp,
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
        case alarmRestrain:
            *write_method = write_alarmRestrain;
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_RESTRAIN);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case alarmReversalMode:
            *write_method = write_alarmReversalMode;
            alarmScalarInt_get((uint8_t *)&int_value, SNMP_ALM_REVERSESAL_MODE);

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }
}



u_char *alarmPortMonitorCfgTable_get(struct variable *vp,
                                     oid *name,
                                     size_t *length,
                                     int exact, size_t *var_len, WriteMethod **write_method)
{
    stSnmpAlmPortMonitorCfgTableInfo *pinfo = NULL;
    stSnmpAlmPortMonitorCfgTableIndex index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx6_index_get(vp, name, length, &index.iDevIndex,
                                     &index.iPortIndex1,
                                     &index.iPortIndex2,
                                     &index.iPortIndex3,
                                     &index.iPortIndex4,
                                     &index.iPortIndex5, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if ((0 == index.iDevIndex) &&
            (0 == index.iPortIndex1) &&
            (0 == index.iPortIndex2) &&
            (0 == index.iPortIndex3) &&
            (0 == index.iPortIndex4) &&
            (0 == index.iPortIndex5))
    {
        index.iDevIndex = 0xffffffff;
        index.iPortIndex1 = 0xffffffff;
        index.iPortIndex2 = 0xffffffff;
        index.iPortIndex3 = 0xffffffff;
        index.iPortIndex4 = 0xffffffff;
        index.iPortIndex5 = 0xffffffff;
    }

    if (NULL == alarmPortMonitorCfgTable_cache)
    {
        alarmPortMonitorCfgTable_cache = snmp_cache_init(sizeof(stSnmpAlmPortMonitorCfgTableInfo),
                                                         alarmPortMonitorCfgTable_get_data_from_ipc,
                                                         alarmPortMonitorCfgTable_node_lookup);

        if (NULL == alarmPortMonitorCfgTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stSnmpAlmPortMonitorCfgTableInfo *)snmp_cache_get_data_by_index(alarmPortMonitorCfgTable_cache, exact, &index);

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_intx6_index_set(vp, name, length, pinfo->index.iDevIndex,
                                       pinfo->index.iPortIndex1,
                                       pinfo->index.iPortIndex2,
                                       pinfo->index.iPortIndex3,
                                       pinfo->index.iPortIndex4,
                                       pinfo->index.iPortIndex5);
        }
    }

    switch (vp->magic)
    {
#if 0

        case monDevIndex:
            if (pinfo)
            {
                int_value = pinfo->index.iDevIndex;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case monPortIndex1:
            if (pinfo)
            {
                int_value = pinfo->index.iPortIndex1;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case monPortIndex2:
            if (pinfo)
            {
                int_value = pinfo->index.iPortIndex2;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }


        case monPortIndex3:
            if (pinfo)
            {
                int_value = pinfo->index.iPortIndex3;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case monPortIndex4:
            if (pinfo)
            {
                int_value = pinfo->index.iPortIndex4;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case monPortIndex5:
            if (pinfo)
            {
                int_value = pinfo->index.iPortIndex5;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

#endif

        case alarmMonitorEn:
            *write_method = write_alarmMonitorEn;

            if (pinfo)
            {
                int_value = pinfo->data.iAlmMonitorEN;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case alarmRowStatus:
            *write_method = write_alarmRowStatus;

            if (pinfo)
            {
                int_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}



u_char *alarmAttributeCfgTable_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    stSnmpAlmAttribCfgTableInfo *pinfo = NULL;
    stSnmpAlmAttribCfgTableIndex index;
    memset(&index, 0, sizeof(stSnmpAlmAttribCfgTableIndex));
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx7_index_get(vp, name, length, &index.iAlmSubType,
                                     &index.iAttrDevIndex,
                                     &index.iAttrPortIndex1,
                                     &index.iAttrPortIndex2,
                                     &index.iAttrPortIndex3,
                                     &index.iAttrPortIndex4,
                                     &index.iAttrPortIndex5, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if ((0 == index.iAlmSubType) &&
            (0 == index.iAttrDevIndex) &&
            (0 == index.iAttrPortIndex1) &&
            (0 == index.iAttrPortIndex2) &&
            (0 == index.iAttrPortIndex3) &&
            (0 == index.iAttrPortIndex4) &&
            (0 == index.iAttrPortIndex5))
    {
        index.iAlmSubType = 0xffffffff;
        index.iAttrDevIndex = 0xffffffff;
        index.iAttrPortIndex1 = 0xffffffff;
        index.iAttrPortIndex2 = 0xffffffff;
        index.iAttrPortIndex3 = 0xffffffff;
        index.iAttrPortIndex4 = 0xffffffff;
        index.iAttrPortIndex5 = 0xffffffff;
    }

    if (NULL == alarmAttributeCfgTable_cache)
    {
        alarmAttributeCfgTable_cache = snmp_cache_init(sizeof(stSnmpAlmAttribCfgTableInfo),
                                                       alarmAttributeCfgTable_get_data_from_ipc,
                                                       alarmAttributeCfgTable_node_lookup);

        if (NULL == alarmAttributeCfgTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stSnmpAlmAttribCfgTableInfo *)snmp_cache_get_data_by_index(alarmAttributeCfgTable_cache, exact, &index);

    /* get ready the next index */
    if (pinfo)
    {
        if (!exact)
        {
            ipran_snmp_intx7_index_set(vp, name, length, pinfo->index.iAlmSubType,
                                       pinfo->index.iAttrDevIndex,
                                       pinfo->index.iAttrPortIndex1,
                                       pinfo->index.iAttrPortIndex2,
                                       pinfo->index.iAttrPortIndex3,
                                       pinfo->index.iAttrPortIndex4,
                                       pinfo->index.iAttrPortIndex5);
        }
    }

    switch (vp->magic)
    {
#if 0

        case alarmSubType:
            if (pinfo)
            {
                int_value = pinfo->index.iAlmSubType;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case attribDevIndex:
            if (pinfo)
            {
                int_value = pinfo->index.iAttrDevIndex;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case attribPortIndex1:
            if (pinfo)
            {
                int_value = pinfo->index.iAttrPortIndex1;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case attribPortIndex2:
            if (pinfo)
            {
                int_value = pinfo->index.iAttrPortIndex2;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case attribPortIndex3:
            if (pinfo)
            {
                int_value = pinfo->index.iAttrPortIndex3;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case attribPortIndex4:
            if (pinfo)
            {
                int_value = pinfo->index.iAttrPortIndex4;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case attribPortIndex5:
            if (pinfo)
            {
                int_value = pinfo->index.iAttrPortIndex5;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

#endif

        case basePortMark:
            *write_method = write_basePortMark;

            if (pinfo)
            {
                int_value = pinfo->data.iBasePortMark;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case alarmLevel:
            *write_method = write_alarmLevel;

            if (pinfo)
            {
                int_value = pinfo->data.iAlmLevel;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case alarmMask:
            *write_method = write_alarmMask;

            if (pinfo)
            {
                int_value = pinfo->data.iAlmMask;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case alarmFilter:
            *write_method = write_alarmFilter;

            if (pinfo)
            {
                int_value = pinfo->data.iAlmFilter;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case alarmReport:
            *write_method = write_alarmReport;

            if (pinfo)
            {
                int_value = pinfo->data.iAlmReport;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case alarmRecord:
            *write_method = write_alarmRecord;

            if (pinfo)
            {
                int_value = pinfo->data.iAlmRecord;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case portAlmReverse:
            *write_method = write_alarmReverse;

            if (pinfo)
            {
                int_value = pinfo->data.iPortAlmReverse;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case attribRowStatus:
            *write_method = write_attribRowStatus;

            if (pinfo)
            {
                int_value = pinfo->data.rowStatus;
                *var_len = sizeof(int);
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        default:
            break;
    }
}


u_char *alarmCurrDataSheetTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    stSnmpAlmCurrDSTableInfo *pinfo = NULL;
    stSnmpAlmCurrDSTableIndex index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.iAlmCurrDSIndex , exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == alarmCurrDataSheetTable_cache)
    {
        alarmCurrDataSheetTable_cache = snmp_cache_init(sizeof(stSnmpAlmCurrDSTableInfo),
                                                        alarmCurrDataSheetTable_get_data_from_ipc,
                                                        alarmCurrDataSheetTable_node_lookup);

        if (NULL == alarmCurrDataSheetTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stSnmpAlmCurrDSTableInfo *)snmp_cache_get_data_by_index(alarmCurrDataSheetTable_cache, exact, &index);

    if (NULL == pinfo)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pinfo->index.iAlmCurrDSIndex);
    }

    switch (vp->magic)
    {
#if 0

        case currAlarmDSIndex:
            int_value = pinfo->index.iAlmCurrDSIndex;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);
#endif

        case currAlarmType:
            uint_value = pinfo->data.iCurrAlmType;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case currDevIndex:
            uint_value = pinfo->data.iCurrDevIndex;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case currPortIndex1:
            uint_value = pinfo->data.iCurrPortIndex1;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case currPortIndex2:
            uint_value = pinfo->data.iCurrPortIndex2;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case currPortIndex3:
            uint_value = pinfo->data.iCurrPortIndex3;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case currPortIndex4:
            uint_value = pinfo->data.iCurrPortIndex4;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case currPortIndex5:
            uint_value = pinfo->data.iCurrPortIndex5;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case currAlarmLevel:
            int_value = pinfo->data.iCurrAlmLevel;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case alarmCurrProductCnt:
            uint_value = pinfo->data.iAlmCurrProductCnt;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case alarmCurrFirstProductTime:
            uint_value = pinfo->data.iAlmCurrFirstProductTime;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case alarmCurrThisProductTime:
            uint_value = pinfo->data.iAlmCurrThisProductTime;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case alarmCurrRowStatus:
            int_value = SNMP_ROW_ACTIVE;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default:
            break;
    }
}



u_char *alarmHistDataSheetTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    stSnmpAlmHistDSTableInfo *pinfo = NULL;
    stSnmpAlmHistDSTableIndex index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.iAlmHistDSIndex , exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == alarmHistDataSheetTable_cache)
    {
        alarmHistDataSheetTable_cache = snmp_cache_init(sizeof(stSnmpAlmHistDSTableInfo),
                                                        alarmHistDataSheetTable_get_data_from_ipc,
                                                        alarmHistDataSheetTable_node_lookup);

        if (NULL == alarmHistDataSheetTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stSnmpAlmHistDSTableInfo *)snmp_cache_get_data_by_index(alarmHistDataSheetTable_cache, exact, &index);

    if (NULL == pinfo)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pinfo->index.iAlmHistDSIndex);
    }

    switch (vp->magic)
    {
#if 0

        case histAlarmDSIndex:
            int_value = pinfo->index.iAlmHistDSIndex;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);
#endif

        case histAlarmType:
            uint_value = pinfo->data.iHistAlmType;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case histDevIndex:
            uint_value = pinfo->data.iHistDevIndex;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case histPortIndex1:
            uint_value = pinfo->data.iHistPortIndex1;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case histPortIndex2:
            uint_value = pinfo->data.iHistPortIndex2;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case histPortIndex3:
            uint_value = pinfo->data.iHistPortIndex3;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case histPortIndex4:
            uint_value = pinfo->data.iHistPortIndex4;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case histPortIndex5:
            uint_value = pinfo->data.iHistPortIndex5;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case histAlarmLevel:
            int_value = pinfo->data.iHistAlmLevel;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case alarmHistProductCnt:
            uint_value = pinfo->data.iAlmHistProductCnt;
            *var_len = sizeof(int);
            return (u_char *)(&uint_value);

        case alarmHistFirstProductTime:
            uint_value = pinfo->data.iAlmHistFirstProductTime;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case alarmHistThisProductTime:
            uint_value = pinfo->data.iAlmHistThisProductTime;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case alarmHistThisDisappearTime:
            uint_value = pinfo->data.iAlmHistThisDisappearTime;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case alarmHistRowStatus:
            int_value = SNMP_ROW_ACTIVE;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default:
            break;
    }
}


u_char *eventDataSheetTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    stSnmpEventDSTableInfo *pinfo = NULL;
    stSnmpEventDSTableIndex index;
    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.iEventDSIndex , exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == eventDataSheetTable_cache)
    {
        eventDataSheetTable_cache = snmp_cache_init(sizeof(stSnmpEventDSTableInfo),
                                                    eventDataSheetTable_get_data_from_ipc,
                                                    eventDataSheetTable_node_lookup);

        if (NULL == eventDataSheetTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (stSnmpEventDSTableInfo *)snmp_cache_get_data_by_index(eventDataSheetTable_cache, exact, &index);

    if (NULL == pinfo)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pinfo->index.iEventDSIndex);
    }

    switch (vp->magic)
    {
#if 0

        case eventDSIndex:
            int_value = pinfo->index.iEventDSIndex;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);
#endif

        case eventAlarmType:
            uint_value = pinfo->data.iEventAlmType;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case eventDevIndex:
            uint_value = pinfo->data.iEventDevIndex;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case eventPortIndex1:
            uint_value = pinfo->data.iEventPortIndex1;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case eventPortIndex2:
            uint_value = pinfo->data.iEventPortIndex2;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case eventPortIndex3:
            uint_value = pinfo->data.iEventPortIndex3;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case eventPortIndex4:
            uint_value = pinfo->data.iEventPortIndex4;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case eventPortIndex5:
            uint_value = pinfo->data.iEventPortIndex5;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case eventAlarmLevel:
            int_value = pinfo->data.iEventAlmLevel;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case eventDetail:
            int_value = pinfo->data.iEventDetail;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case eventTime:
            uint_value = pinfo->data.iEventTime;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case eventRowStatus:
            int_value = SNMP_ROW_ACTIVE;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default:
            break;
    }
}






/*************************** write api *********************************/

int alarmParameter_set(int data, int subtype)
{
    unsigned int iReval;

    if ((data < 0) || (data > 20))        //控制数据范围
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : Bad value to set!\n", __func__, __LINE__);
        return FALSE;
    }

    alarmScalarInt_get((uint8_t *)&iReval, subtype);

    if (iReval == data)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : There is no need to set, same value\n", __func__, __LINE__);
        return TRUE;
    }

    alarmScalarInt_set(subtype, data);
	return TRUE;
}

/**************************** write interface ********************************/
int
write_alarmProductDelayTime(int action, u_char *var_val,
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
    if (alarmParameter_set(i_data, SNMP_ALM_PRODUCT_DELAY_TIME) == FALSE)       //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
write_alarmDisappearDelayTime(int action, u_char *var_val,
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
    if (alarmParameter_set(i_data, SNMP_ALM_DISAPPEAR_DELAY_TIME) == FALSE)         //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}


int
write_alarmBuzzerEn(int action, u_char *var_val,
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
    if (alarmParameter_set(i_data, SNMP_ALM_BUZZER_EN) == FALSE)        //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
write_alarmBuzzerClear(int action, u_char *var_val,
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
    if (alarmParameter_set(i_data, SNMP_ALM_BUZZER_CLEAR) == FALSE)         //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
write_alarmBuzzerThreshold(int action, u_char *var_val,
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
    if (alarmParameter_set(i_data, SNMP_ALM_BUZZER_THRESHOLD) == FALSE)         //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
write_alarmCurrAlmDSCircle(int action, u_char *var_val,
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
    if (alarmParameter_set(i_data, SNMP_ALM_CURR_ALM_DS_CIRCLE) == FALSE)       //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
write_alarmHistAlmDSCircle(int action, u_char *var_val,
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
    if (alarmParameter_set(i_data, SNMP_ALM_HIST_ALM_DS_CIRCLE) == FALSE)       //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
write_alarmEventDSCircle(int action, u_char *var_val,
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
    if (alarmParameter_set(i_data, SNMP_ALM_EVENT_DS_CIRCLE) == FALSE)      //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
write_alarmRestrain(int action, u_char *var_val,
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
    if (alarmParameter_set(i_data, SNMP_ALM_RESTRAIN) == FALSE)         //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
write_alarmReversalMode(int action, u_char *var_val,
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
    if (alarmParameter_set(i_data, SNMP_ALM_REVERSESAL_MODE) == FALSE)      //调用设置函数，完成设置流程
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}



/**************************************** write alarmPortMonitorCfg table *********************************************/

/*almPortMonitorCfgTab*/
static int almPortMonitorCfgTabReadMessage(stSnmpAlmPortMonitorCfgTableIndex *pIndex_input)
{
    struct ipc_mesg_n *pMsgRcv = NULL;

    if ((almPortMonitorData.index.iDevIndex != pIndex_input->iDevIndex)\
            || (almPortMonitorData.index.iPortIndex1 != pIndex_input->iPortIndex1)\
            || (almPortMonitorData.index.iPortIndex1 != pIndex_input->iPortIndex2)\
            || (almPortMonitorData.index.iPortIndex1 != pIndex_input->iPortIndex3)\
            || (almPortMonitorData.index.iPortIndex1 != pIndex_input->iPortIndex4)\
            || (almPortMonitorData.index.iPortIndex1 != pIndex_input->iPortIndex5))
    {
        memset(&almPortMonitorData, 0, sizeof(almPortMonitorData));
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : index(%d|%d|%d|%d|%d|%d) (%x|%x|%x|%x|%x|%x)\n", __func__, __LINE__,
               pIndex_input->iDevIndex, pIndex_input->iPortIndex1, \
               pIndex_input->iPortIndex2, pIndex_input->iPortIndex3, pIndex_input->iPortIndex4, pIndex_input->iPortIndex5,
               pIndex_input->iDevIndex, pIndex_input->iPortIndex1, \
               pIndex_input->iPortIndex2, pIndex_input->iPortIndex3, pIndex_input->iPortIndex4, pIndex_input->iPortIndex5);

    if (!almPortMonitorData.data.rowStatus)
    {
        /*pinfo = (stSnmpAlmPortMonitorCfgTableInfo *)ipc_send_common_wait_reply(pIndex_input, sizeof(stSnmpAlmPortMonitorCfgTableIndex),
                                                                               1, MODULE_ID_ALARM, MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_ALM_PORT_MONITOR_CFG_TABLE, IPC_OPCODE_GET, 0);*/

		pMsgRcv = ipc_sync_send_n2(pIndex_input, sizeof(stSnmpAlmPortMonitorCfgTableIndex), 1, MODULE_ID_ALARM,
			MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_ALM_PORT_MONITOR_CFG_TABLE, IPC_OPCODE_GET, 0, 1000);

		if (NULL == pMsgRcv)
		{
			return FALSE;
		}

        if (0 != pMsgRcv->msghdr.data_num)
        {
            memcpy(&almPortMonitorData, pMsgRcv->msg_data, sizeof(stSnmpAlmPortMonitorCfgTableInfo));
            almPortMonitorData.data.rowStatus  = SNMP_ROW_ACTIVE;

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


static int almPortMonitorCfgTabOperate(stSnmpAlmPortMonitorCfgTableIndex *pIndex)
{
    stSnmpAlmPortMonitorCfgTableInfo info;
    memset(&info, 0, sizeof(stSnmpAlmPortMonitorCfgTableInfo));
    memcpy(&info.index, pIndex, sizeof(stSnmpAlmPortMonitorCfgTableIndex));

    info.data.iAlmMonitorEN = almPortMonitorCfgTabList.flagsOfColumnSet & FLAG_ALARMMONITOREN
                              ? almPortMonitorCfgTabList.data.iAlmMonitorEN : almPortMonitorData.data.iAlmMonitorEN;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : index(%d|%d|%d|%d|%d|%d), data(%d)\n", __func__, __LINE__,
               info.index.iDevIndex, info.index.iPortIndex1, info.index.iPortIndex2,
               info.index.iPortIndex3, info.index.iPortIndex4, info.index.iPortIndex5,
               info.data.iAlmMonitorEN);

    /*ipc_send_common(&info, sizeof(stSnmpAlmPortMonitorCfgTableInfo), 1, MODULE_ID_ALARM, MODULE_ID_SNMPD,
                    IPC_TYPE_SNMP, SNMP_ALM_PORT_MONITOR_CFG_TABLE, IPC_OPCODE_UPDATE);*/

	ipc_send_msg_n2((void *)&info, sizeof(stSnmpAlmPortMonitorCfgTableInfo), 1, MODULE_ID_ALARM, MODULE_ID_SNMPD,
		IPC_TYPE_SNMP, SNMP_ALM_PORT_MONITOR_CFG_TABLE, IPC_OPCODE_UPDATE, 0);

    return TRUE;
}


static int alarmRowStatus_get(stSnmpAlmPortMonitorCfgTableIndex *pIndex, int *data)
{
    u_int32_t ret;

    ret = almPortMonitorCfgTabReadMessage(pIndex);

    if (ret == FALSE)
    {
        if ((almPortMonitorCfgTabList.flagsOfColumnSet & FLAG_ALARMMONITORROWSTATUS)
                && ((almPortMonitorCfgTabList.index.iDevIndex == pIndex->iDevIndex)\
                    && (almPortMonitorCfgTabList.index.iPortIndex1 == pIndex->iPortIndex1)\
                    && (almPortMonitorCfgTabList.index.iPortIndex2 == pIndex->iPortIndex2)\
                    && (almPortMonitorCfgTabList.index.iPortIndex3 == pIndex->iPortIndex3)\
                    && (almPortMonitorCfgTabList.index.iPortIndex4 == pIndex->iPortIndex4)\
                    && (almPortMonitorCfgTabList.index.iPortIndex5 == pIndex->iPortIndex5)))
        {
            if (almPortMonitorCfgTabList.flagsOfColumnSet & FLAG_ALARMMONITOREN)
            {
                *data = SNMP_ROW_NOTINSERVICE;
            }
            else
            {
                *data = SNMP_ROW_NOTREADY;
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


static int alarmRowStatus_set(stSnmpAlmPortMonitorCfgTableIndex *pIndex, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = alarmRowStatus_get(pIndex, &row_status);
    zlog_debug(SNMP_DBG_MIB_GET, "%s : stat alarm status(%d) return reslut(%d), data(%d)\n\r", \
               __FUNCTION__, row_status, ret, data);

    if (ret == FALSE)
    {
        if (SNMP_ROW_NOTINSERVICE == data)
        {
            almPortMoniCfgTabStat.flag = ALARM_MODIFY_TABLE_FLAG;
            almPortMonitorCfgTabList.flagsOfColumnSet = FLAG_ALARMMONITORROWSTATUS;
            almPortMonitorCfgTabList.index.iDevIndex   = pIndex->iDevIndex;
            almPortMonitorCfgTabList.index.iPortIndex1 = pIndex->iPortIndex1;
            almPortMonitorCfgTabList.index.iPortIndex2 = pIndex->iPortIndex2;
            almPortMonitorCfgTabList.index.iPortIndex3 = pIndex->iPortIndex3;
            almPortMonitorCfgTabList.index.iPortIndex4 = pIndex->iPortIndex4;
            almPortMonitorCfgTabList.index.iPortIndex5 = pIndex->iPortIndex5;
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
            if (almPortMoniCfgTabStat.flag == ALARM_MODIFY_TABLE_FLAG)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : modify the table\n\r", __func__, __LINE__);
                ret = almPortMonitorCfgTabOperate(pIndex);

                if (ret == FALSE)
                {
                    return FALSE;
                }

                memset(&almPortMoniCfgTabStat, 0, sizeof(almPortMonitorCfgStatus));
                memset(&almPortMonitorData, 0, sizeof(stSnmpAlmPortMonitorCfgTableInfo));
                memset(&almPortMonitorCfgTabList, 0, sizeof(stSnmpAlmPortMonitorCfgTableInfo));
            }
            else if (SNMP_ROW_NOTINSERVICE == row_status)
            {
                //fill your code here
                memset(&almPortMoniCfgTabStat, 0, sizeof(almPortMonitorCfgStatus));
                memset(&almPortMonitorData, 0, sizeof(stSnmpAlmPortMonitorCfgTableInfo));
                memset(&almPortMonitorCfgTabList, 0, sizeof(stSnmpAlmPortMonitorCfgTableInfo));
            }
            else
            {
                return FALSE;
            }
        }
        else if (SNMP_ROW_DESTROY == data)
        {
            memset(&almPortMoniCfgTabStat, 0, sizeof(almPortMonitorCfgStatus));
            memset(&almPortMonitorData, 0, sizeof(stSnmpAlmPortMonitorCfgTableInfo));
            memset(&almPortMonitorCfgTabList, 0, sizeof(stSnmpAlmPortMonitorCfgTableInfo));
            return FALSE;
        }
        else if (SNMP_ROW_NOTINSERVICE == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //almPortMonitorCfgTabList.data.statSubTypeRowStatus = SNMP_ROW_NOTINSERVICE;
                almPortMoniCfgTabStat.flag = ALARM_MODIFY_TABLE_FLAG;
                almPortMonitorCfgTabList.flagsOfColumnSet |= FLAG_ALARMMONITORROWSTATUS;
            }
            else
            {
                memset(&almPortMoniCfgTabStat, 0, sizeof(almPortMonitorCfgStatus));
                memset(&almPortMonitorData, 0, sizeof(stSnmpAlmPortMonitorCfgTableInfo));
                memset(&almPortMonitorCfgTabList, 0, sizeof(stSnmpAlmPortMonitorCfgTableInfo));
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


static int alarmMonitorEn_set(stSnmpAlmPortMonitorCfgTableIndex *pIndex, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = alarmRowStatus_get(pIndex, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            almPortMonitorCfgTabList.data.iAlmMonitorEN = data;
            almPortMonitorCfgTabList.flagsOfColumnSet |= FLAG_ALARMMONITOREN;
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            almPortMonitorCfgTabList.data.iAlmMonitorEN = data;
            almPortMonitorCfgTabList.flagsOfColumnSet |= FLAG_ALARMMONITOREN;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}



int write_alarmMonitorEn(int action, u_char *var_val,
                         u_char var_val_type, size_t var_val_len,
                         u_char *statP, oid *name, size_t length)
{
    stSnmpAlmPortMonitorCfgTableIndex index;
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
    index.iDevIndex   = name[length - 6];
    index.iPortIndex1 = name[length - 5];
    index.iPortIndex2 = name[length - 4];
    index.iPortIndex3 = name[length - 3];
    index.iPortIndex4 = name[length - 2];
    index.iPortIndex5 = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (alarmMonitorEn_set(&index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}


int write_alarmRowStatus(int action, u_char *var_val,
                         u_char var_val_type, size_t var_val_len,
                         u_char *statP, oid *name, size_t length)
{
    stSnmpAlmPortMonitorCfgTableIndex index;
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
    index.iDevIndex   = name[length - 6];
    index.iPortIndex1 = name[length - 5];
    index.iPortIndex2 = name[length - 4];
    index.iPortIndex3 = name[length - 3];
    index.iPortIndex4 = name[length - 2];
    index.iPortIndex5 = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (alarmRowStatus_set(&index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}


/**************************************** write alarmPortMonitorCfg table end *********************************************/



/**************************************** write attributeCfg table *********************************************/

/*almAttribCfgTab*/
static int almAttribCfgTabReadMessage(stSnmpAlmAttribCfgTableIndex *pIndex_input)
{
    struct ipc_mesg_n *pMsgRcv = NULL;

    if ((attribCfgTabData.index.iAlmSubType != pIndex_input->iAlmSubType)\
            || (attribCfgTabData.index.iAttrDevIndex != pIndex_input->iAttrDevIndex)\
            || (attribCfgTabData.index.iAttrPortIndex1 != pIndex_input->iAttrPortIndex1)\
            || (attribCfgTabData.index.iAttrPortIndex2 != pIndex_input->iAttrPortIndex2)\
            || (attribCfgTabData.index.iAttrPortIndex3 != pIndex_input->iAttrPortIndex3)\
            || (attribCfgTabData.index.iAttrPortIndex4 != pIndex_input->iAttrPortIndex4)\
            || (attribCfgTabData.index.iAttrPortIndex5 != pIndex_input->iAttrPortIndex5))
    {
        memset(&attribCfgTabData, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
    }

    if (!attribCfgTabData.data.rowStatus)
    {
        /*pinfo = (stSnmpAlmAttribCfgTableInfo *)ipc_send_common_wait_reply(pIndex_input, sizeof(stSnmpAlmAttribCfgTableIndex),
                                                                          1, MODULE_ID_ALARM, MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_ALM_ATTRIBUTE_CFG_TABLE, IPC_OPCODE_GET, 0);*/

		pMsgRcv = ipc_sync_send_n2(pIndex_input, sizeof(stSnmpAlmAttribCfgTableIndex), 1, MODULE_ID_ALARM,
			MODULE_ID_SNMPD, IPC_TYPE_SNMP, SNMP_ALM_ATTRIBUTE_CFG_TABLE, IPC_OPCODE_GET, 0, 1000);

		if (NULL == pMsgRcv)
		{
			return FALSE;
		}
		
        if (0 != pMsgRcv->msghdr.data_num)
        {
            memcpy(&attribCfgTabData, pMsgRcv->msg_data, sizeof(stSnmpAlmAttribCfgTableInfo));
            attribCfgTabData.data.rowStatus  = SNMP_ROW_ACTIVE;

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


static int almAttribCfgTabOperate(stSnmpAlmAttribCfgTableIndex *pIndex)
{
    stSnmpAlmAttribCfgTableInfo info;
    memset(&info, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
    memcpy(&info.index, pIndex, sizeof(stSnmpAlmAttribCfgTableIndex));

    info.data.iBasePortMark = attribCfgTabList.flagsOfColumnSet & FLAG_BASEPORTMARK
                              ? attribCfgTabList.data.iBasePortMark : attribCfgTabData.data.iBasePortMark;
    info.data.iAlmLevel = attribCfgTabList.flagsOfColumnSet & FLAG_ALARMLEVEL
                          ? attribCfgTabList.data.iAlmLevel : attribCfgTabData.data.iAlmLevel;
    info.data.iAlmMask = attribCfgTabList.flagsOfColumnSet & FLAG_ALARMMASK
                         ? attribCfgTabList.data.iAlmMask : attribCfgTabData.data.iAlmMask;
    info.data.iAlmFilter = attribCfgTabList.flagsOfColumnSet & FLAG_ALARMFILTER
                           ? attribCfgTabList.data.iAlmFilter : attribCfgTabData.data.iAlmFilter;
    info.data.iAlmReport = attribCfgTabList.flagsOfColumnSet & FLAG_ALARMREPORT
                           ? attribCfgTabList.data.iAlmReport : attribCfgTabData.data.iAlmReport;
    info.data.iAlmRecord = attribCfgTabList.flagsOfColumnSet & FLAG_ALARMRECORD
                           ? attribCfgTabList.data.iAlmRecord : attribCfgTabData.data.iAlmRecord;
    info.data.iPortAlmReverse = attribCfgTabList.flagsOfColumnSet & FLAG_PORTALMREVERSE
                                ? attribCfgTabList.data.iPortAlmReverse : attribCfgTabData.data.iPortAlmReverse;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : index(%d|%d|%d|%d|%d|%d|%d), data(%d|%d|%d|%d|%d|%d|%d)\n", __func__, __LINE__,
               info.index.iAlmSubType, info.index.iAttrDevIndex, info.index.iAttrPortIndex1,
               info.index.iAttrPortIndex2, info.index.iAttrPortIndex3, info.index.iAttrPortIndex4,
               info.index.iAttrPortIndex5,
               info.data.iBasePortMark, info.data.iAlmLevel, info.data.iAlmMask,
               info.data.iAlmFilter, info.data.iAlmReport, info.data.iAlmRecord,
               info.data.iPortAlmReverse);

    /*ipc_send_common(&info, sizeof(stSnmpAlmAttribCfgTableInfo), 1, MODULE_ID_ALARM, MODULE_ID_SNMPD,
                    IPC_TYPE_SNMP, SNMP_ALM_ATTRIBUTE_CFG_TABLE, IPC_OPCODE_UPDATE);*/

	ipc_send_msg_n2((void *)&info, sizeof(stSnmpAlmAttribCfgTableInfo), 1, MODULE_ID_ALARM, MODULE_ID_SNMPD,
		IPC_TYPE_SNMP, SNMP_ALM_ATTRIBUTE_CFG_TABLE, IPC_OPCODE_UPDATE, 0);

    return TRUE;
}


static int attribRowStatus_get(stSnmpAlmAttribCfgTableIndex *pIndex, int *data)
{
    u_int32_t ret;

    ret = almAttribCfgTabReadMessage(pIndex);

    if (ret == FALSE)
    {
        if ((attribCfgTabList.flagsOfColumnSet & FLAG_ALARMATTRIROWSTATUS)
                && ((attribCfgTabList.index.iAlmSubType == pIndex->iAlmSubType)\
                    && (attribCfgTabList.index.iAttrDevIndex == pIndex->iAttrDevIndex)\
                    && (attribCfgTabList.index.iAttrPortIndex1 == pIndex->iAttrPortIndex1)\
                    && (attribCfgTabList.index.iAttrPortIndex2 == pIndex->iAttrPortIndex2)\
                    && (attribCfgTabList.index.iAttrPortIndex3 == pIndex->iAttrPortIndex3)\
                    && (attribCfgTabList.index.iAttrPortIndex4 == pIndex->iAttrPortIndex4)\
                    && (attribCfgTabList.index.iAttrPortIndex5 == pIndex->iAttrPortIndex5)))
        {
            if ((attribCfgTabList.flagsOfColumnSet & FLAG_BASEPORTMARK)\
                    && (attribCfgTabList.flagsOfColumnSet & FLAG_ALARMLEVEL)\
                    && (attribCfgTabList.flagsOfColumnSet & FLAG_ALARMMASK)\
                    && (attribCfgTabList.flagsOfColumnSet & FLAG_ALARMFILTER)\
                    && (attribCfgTabList.flagsOfColumnSet & FLAG_ALARMREPORT)\
                    && (attribCfgTabList.flagsOfColumnSet & FLAG_ALARMRECORD)\
                    && (attribCfgTabList.flagsOfColumnSet & FLAG_PORTALMREVERSE))
            {
                *data = SNMP_ROW_NOTINSERVICE;
            }
            else
            {
                *data = SNMP_ROW_NOTREADY;
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


static int attribRowStatus_set(stSnmpAlmAttribCfgTableIndex *pIndex, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = attribRowStatus_get(pIndex, &row_status);
    zlog_debug(SNMP_DBG_MIB_GET, "%s : alarm status(%d) return reslut(%d), data(%d)\n\r", \
               __FUNCTION__, row_status, ret, data);

    if (ret == FALSE)
    {
        if (SNMP_ROW_NOTINSERVICE == data)
        {
            attribCfgTabStat.flag = ALARM_MODIFY_TABLE_FLAG;
            attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMATTRIROWSTATUS;
            attribCfgTabList.index.iAlmSubType   = pIndex->iAlmSubType;
            attribCfgTabList.index.iAttrDevIndex = pIndex->iAttrDevIndex;
            attribCfgTabList.index.iAttrPortIndex1 = pIndex->iAttrPortIndex1;
            attribCfgTabList.index.iAttrPortIndex2 = pIndex->iAttrPortIndex2;
            attribCfgTabList.index.iAttrPortIndex3 = pIndex->iAttrPortIndex3;
            attribCfgTabList.index.iAttrPortIndex4 = pIndex->iAttrPortIndex4;
            attribCfgTabList.index.iAttrPortIndex5 = pIndex->iAttrPortIndex5;
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
            //if (almPortMonitorCfgTabList.data.statSubTypeRowStatus == SNMP_ROW_NOTINSERVICE)
            if (attribCfgTabStat.flag == ALARM_MODIFY_TABLE_FLAG)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : modify the table\n\r", __func__, __LINE__);
                ret = almAttribCfgTabOperate(pIndex);

                if (ret == FALSE)
                {
                    return FALSE;
                }

                memset(&attribCfgTabStat, 0, sizeof(almAttriCfgStatus));
                memset(&attribCfgTabData, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
                memset(&attribCfgTabList, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
            }
            else if (SNMP_ROW_NOTINSERVICE == row_status)
            {
                //fill your code here
                memset(&attribCfgTabStat, 0, sizeof(almAttriCfgStatus));
                memset(&attribCfgTabData, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
                memset(&attribCfgTabList, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
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
                memset(&attribCfgTabStat, 0, sizeof(almAttriCfgStatus));
                memset(&attribCfgTabData, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
                memset(&attribCfgTabList, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
                return FALSE;
            }
            else
            {
                memset(&attribCfgTabStat, 0, sizeof(almAttriCfgStatus));
                memset(&attribCfgTabData, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
                memset(&attribCfgTabList, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
            }
        }
        else if (SNMP_ROW_NOTINSERVICE == data)
        {
            if (SNMP_ROW_ACTIVE == row_status)
            {
                //almPortMonitorCfgTabList.data.statSubTypeRowStatus = SNMP_ROW_NOTINSERVICE;
                attribCfgTabStat.flag = ALARM_MODIFY_TABLE_FLAG;
                attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMATTRIROWSTATUS;
            }
            else
            {
                memset(&attribCfgTabStat, 0, sizeof(almAttriCfgStatus));
                memset(&attribCfgTabData, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
                memset(&attribCfgTabList, 0, sizeof(stSnmpAlmAttribCfgTableInfo));
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


static int alarmBasePortMark_set(stSnmpAlmAttribCfgTableIndex *pIndex, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = attribRowStatus_get(pIndex, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (attribCfgTabStat.flag == ALARM_MODIFY_TABLE_FLAG)
            {
                attribCfgTabList.data.iBasePortMark = data;
                attribCfgTabList.flagsOfColumnSet |= FLAG_BASEPORTMARK;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            attribCfgTabList.data.iBasePortMark = data;
            attribCfgTabList.flagsOfColumnSet |= FLAG_BASEPORTMARK;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

static int alarmLevel_set(stSnmpAlmAttribCfgTableIndex *pIndex, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = attribRowStatus_get(pIndex, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (attribCfgTabStat.flag == ALARM_MODIFY_TABLE_FLAG)
            {
                attribCfgTabList.data.iAlmLevel = data;
                attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMLEVEL;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            attribCfgTabList.data.iAlmLevel = data;
            attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMLEVEL;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}


static int alarmMask_set(stSnmpAlmAttribCfgTableIndex *pIndex, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = attribRowStatus_get(pIndex, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (attribCfgTabStat.flag == ALARM_MODIFY_TABLE_FLAG)
            {
                attribCfgTabList.data.iAlmMask = data;
                attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMMASK;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            attribCfgTabList.data.iAlmMask = data;
            attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMMASK;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}


static int alarmFilter_set(stSnmpAlmAttribCfgTableIndex *pIndex, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = attribRowStatus_get(pIndex, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (attribCfgTabStat.flag == ALARM_MODIFY_TABLE_FLAG)
            {
                attribCfgTabList.data.iAlmFilter = data;
                attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMFILTER;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            attribCfgTabList.data.iAlmFilter = data;
            attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMFILTER;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}


static int alarmReport_set(stSnmpAlmAttribCfgTableIndex *pIndex, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = attribRowStatus_get(pIndex, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (attribCfgTabStat.flag == ALARM_MODIFY_TABLE_FLAG)
            {
                attribCfgTabList.data.iAlmReport = data;
                attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMREPORT;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            attribCfgTabList.data.iAlmReport = data;
            attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMREPORT;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}


static int alarmRecord_set(stSnmpAlmAttribCfgTableIndex *pIndex, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = attribRowStatus_get(pIndex, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (attribCfgTabStat.flag == ALARM_MODIFY_TABLE_FLAG)
            {
                attribCfgTabList.data.iAlmRecord = data;
                attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMRECORD;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            attribCfgTabList.data.iAlmRecord = data;
            attribCfgTabList.flagsOfColumnSet |= FLAG_ALARMRECORD;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}


static int alarmReverse_set(stSnmpAlmAttribCfgTableIndex *pIndex, int data)
{
    int ret = 0;
    int row_status = 0;

    ret = attribRowStatus_get(pIndex, &row_status);

    if (ret == FALSE)
    {
        return FALSE;
    }
    else
    {
        if (SNMP_ROW_ACTIVE == row_status)
        {
            if (attribCfgTabStat.flag == ALARM_MODIFY_TABLE_FLAG)
            {
                attribCfgTabList.data.iPortAlmReverse = data;
                attribCfgTabList.flagsOfColumnSet |= FLAG_PORTALMREVERSE;
            }
            else
            {
                return FALSE;
            }
        }
        else if ((SNMP_ROW_NOTINSERVICE == row_status) || (SNMP_ROW_NOTREADY == row_status))
        {
            //fill your code here
            attribCfgTabList.data.iPortAlmReverse = data;
            attribCfgTabList.flagsOfColumnSet |= FLAG_PORTALMREVERSE;
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}



int
write_basePortMark(int action, u_char *var_val,
                   u_char var_val_type, size_t var_val_len,
                   u_char *statP, oid *name, size_t length)
{
    stSnmpAlmAttribCfgTableIndex index;
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
    index.iAlmSubType       = name[length - 7];
    index.iAttrDevIndex     = name[length - 6];
    index.iAttrPortIndex1   = name[length - 5];
    index.iAttrPortIndex2   = name[length - 4];
    index.iAttrPortIndex3   = name[length - 3];
    index.iAttrPortIndex4   = name[length - 2];
    index.iAttrPortIndex5   = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (alarmBasePortMark_set(&index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int
write_alarmLevel(int action, u_char *var_val,
                 u_char var_val_type, size_t var_val_len,
                 u_char *statP, oid *name, size_t length)
{
    stSnmpAlmAttribCfgTableIndex index;
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
    index.iAlmSubType       = name[length - 7];
    index.iAttrDevIndex     = name[length - 6];
    index.iAttrPortIndex1   = name[length - 5];
    index.iAttrPortIndex2   = name[length - 4];
    index.iAttrPortIndex3   = name[length - 3];
    index.iAttrPortIndex4   = name[length - 2];
    index.iAttrPortIndex5   = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);
    
    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (alarmLevel_set(&index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}

int
write_alarmMask(int action, u_char *var_val,
                u_char var_val_type, size_t var_val_len,
                u_char *statP, oid *name, size_t length)
{
    stSnmpAlmAttribCfgTableIndex index;
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
    index.iAlmSubType       = name[length - 7];
    index.iAttrDevIndex     = name[length - 6];
    index.iAttrPortIndex1   = name[length - 5];
    index.iAttrPortIndex2   = name[length - 4];
    index.iAttrPortIndex3   = name[length - 3];
    index.iAttrPortIndex4   = name[length - 2];
    index.iAttrPortIndex5   = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (alarmMask_set(&index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}


int
write_alarmFilter(int action, u_char *var_val,
                  u_char var_val_type, size_t var_val_len,
                  u_char *statP, oid *name, size_t length)
{
    stSnmpAlmAttribCfgTableIndex index;
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
    index.iAlmSubType       = name[length - 7];
    index.iAttrDevIndex     = name[length - 6];
    index.iAttrPortIndex1   = name[length - 5];
    index.iAttrPortIndex2   = name[length - 4];
    index.iAttrPortIndex3   = name[length - 3];
    index.iAttrPortIndex4   = name[length - 2];
    index.iAttrPortIndex5   = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (alarmFilter_set(&index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}


int
write_alarmReport(int action, u_char *var_val,
                  u_char var_val_type, size_t var_val_len,
                  u_char *statP, oid *name, size_t length)
{
    stSnmpAlmAttribCfgTableIndex index;
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
    index.iAlmSubType       = name[length - 7];
    index.iAttrDevIndex     = name[length - 6];
    index.iAttrPortIndex1   = name[length - 5];
    index.iAttrPortIndex2   = name[length - 4];
    index.iAttrPortIndex3   = name[length - 3];
    index.iAttrPortIndex4   = name[length - 2];
    index.iAttrPortIndex5   = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (alarmReport_set(&index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}


int
write_alarmRecord(int action, u_char *var_val,
                  u_char var_val_type, size_t var_val_len,
                  u_char *statP, oid *name, size_t length)
{
    stSnmpAlmAttribCfgTableIndex index;
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
    index.iAlmSubType       = name[length - 7];
    index.iAttrDevIndex     = name[length - 6];
    index.iAttrPortIndex1   = name[length - 5];
    index.iAttrPortIndex2   = name[length - 4];
    index.iAttrPortIndex3   = name[length - 3];
    index.iAttrPortIndex4   = name[length - 2];
    index.iAttrPortIndex5   = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (alarmRecord_set(&index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}


int
write_alarmReverse(int action, u_char *var_val,
                   u_char var_val_type, size_t var_val_len,
                   u_char *statP, oid *name, size_t length)
{
    stSnmpAlmAttribCfgTableIndex index;
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
    index.iAlmSubType       = name[length - 7];
    index.iAttrDevIndex     = name[length - 6];
    index.iAttrPortIndex1   = name[length - 5];
    index.iAttrPortIndex2   = name[length - 4];
    index.iAttrPortIndex3   = name[length - 3];
    index.iAttrPortIndex4   = name[length - 2];
    index.iAttrPortIndex5   = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (alarmReverse_set(&index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}



int
write_attribRowStatus(int action, u_char *var_val,
                      u_char var_val_type, size_t var_val_len,
                      u_char *statP, oid *name, size_t length)
{
    stSnmpAlmAttribCfgTableIndex index;
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
    index.iAlmSubType       = name[length - 7];
    index.iAttrDevIndex     = name[length - 6];
    index.iAttrPortIndex1   = name[length - 5];
    index.iAttrPortIndex2   = name[length - 4];
    index.iAttrPortIndex3   = name[length - 3];
    index.iAttrPortIndex4   = name[length - 2];
    index.iAttrPortIndex5   = name[length - 1];
    memcpy(&i_data, var_val, var_val_len);

    /*
    * The variable has been stored in 'value' for you to use,
    * and you have just been asked to do something with it.
    * Note that anything done here must be reversable in the UNDO case
    */
    switch (action)
    {
        case RESERVE1:
            if (attribRowStatus_set(&index, i_data) == FALSE)
            {
                return SNMP_ERR_GENERR;
            }

            break;

        case FREE:
            break;
    }

    return SNMP_ERR_NOERROR;
}




