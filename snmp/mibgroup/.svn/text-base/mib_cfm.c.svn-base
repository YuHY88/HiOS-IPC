/*
 *  cli command MIB group implementation - ifm_snmp.c
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
#include <lib/types.h>

#include "net-snmp-config.h"
#include "types.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/ifm_common.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/types.h>
#include <lib/route_com.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/inet_ip.h>
#include <lib/oam_common.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>

#include <lib/log.h>


#include "../../l2/cfm/cfm_session.h"
#include "../../l2/cfm/cfm.h"
#include "../../l2/aps/elps.h"
#include "ipran_snmp_data_cache.h"

#include "mib_cfm.h"

#include "snmp_config_table.h"


enum table_lock
{
    UNLOCK,
    LOCK,
};

#define CFM_SNMP_MD_GET      0
#define CFM_SNMP_MA_GET      1
#define CFM_SNMP_SESSION_GET 2
#define CFM_SNMP_SLA_GET     3
#define CFM_SNMP_SESSION_GET_NEW 4
#define CFM_SNMP_SLA_GET_NEW     5

/*local temp variable*/
static uchar long_str_value[512] = {'\0'};
static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value = 0;
static uchar mac_value[6] = {0};
static uint16_t int16_value = 0;
static uint32_t uint_value = 0;

/* table list define */
struct list *hhrCFMMdTable_list  = NULL;
struct list *hhrCFMMaTable_list = NULL;
struct list *hhrCFMMepTable_list  = NULL;
struct list *hhrCFMSLAResultTable_list = NULL;

/* variable to store refresh data time of the last time */
static time_t hhrCFMMdTable_time_old = 0;
static time_t hhrCFMMaTable_time_old = 0;
static time_t hhrCFMMepTable_time_old  = 0;
static time_t hhrCFMSLAResultTable_time_old = 0;

static enum table_lock hhrCFMMdTable_lock = UNLOCK;
static enum table_lock hhrCFMMaTable_lock = UNLOCK;
static enum table_lock hhrCFMMepTable_lock = UNLOCK;
static enum table_lock hhrCFMSLAResultTable_lock = UNLOCK;


/*
 * Object ID definitions
 */
/*hhrCFMMdTable*/
static oid hhrCFMMdTable_oid[] = {HHRCFMOID, 1};
FindVarMethod hhrCFMMdTable_get;
struct variable2 hhrCFMMdTable_variables[] =
{
    {hhrCFMMdLevel, ASN_INTEGER,   RONLY, hhrCFMMdTable_get, 2, {1, 2}},
    {hhrCFMMdName,  ASN_OCTET_STR, RONLY, hhrCFMMdTable_get, 2, {1, 3}}
};

/*hhrCFMMaTable*/
static oid hhrCFMMaTable_oid[] = {HHRCFMOID, 2};
FindVarMethod hhrCFMMaTable_get;
struct variable2 hhrCFMMaTable_variables[] =
{
    {hhrCFMMaRelatedVlan,  ASN_INTEGER,   RONLY, hhrCFMMaTable_get, 2, {1, 2}},
    {hhrCFMMaLevel,        ASN_INTEGER,   RONLY, hhrCFMMaTable_get, 2, {1, 3}},
    {hhrCFMMaName,         ASN_OCTET_STR, RONLY, hhrCFMMaTable_get, 2, {1, 4}},
    {hhrCFMMaProtocolType, ASN_INTEGER,   RONLY, hhrCFMMaTable_get, 2, {1, 5}},
    {hhrCFMMaMipEnable,    ASN_INTEGER,   RONLY, hhrCFMMaTable_get, 2, {1, 6}},
    {hhrCFMMaY1731,        ASN_INTEGER,   RONLY, hhrCFMMaTable_get, 2, {1, 7}}
};

/*hhrCFMMepTable*/
static oid hhrCFMMepTable_oid[] = {HHRCFMOID, 3};
FindVarMethod hhrCFMMepTable_get;
struct variable2 hhrCFMMepTable_variables[] =
{
    {hhrCFMMepDir,         ASN_INTEGER,   RONLY, hhrCFMMepTable_get, 2, {1, 2}},
    {hhrCFMMepCCMEnable,   ASN_INTEGER,   RONLY, hhrCFMMepTable_get, 2, {1, 3}},
    {hhrCFMMepCCMInterval, ASN_INTEGER,   RONLY, hhrCFMMepTable_get, 2, {1, 4}},
    {hhrCFMMepIfDescr,     ASN_OCTET_STR, RONLY, hhrCFMMepTable_get, 2, {1, 5}},
    {hhrCFMRMepIndex,      ASN_INTEGER,   RONLY, hhrCFMMepTable_get, 2, {1, 6}},
    {hhrCFMRMepMac,        ASN_OCTET_STR, RONLY, hhrCFMMepTable_get, 2, {1, 7}},
    {hhrCFMMepStatus,      ASN_INTEGER,   RONLY, hhrCFMMepTable_get, 2, {1, 8}},
    {hhrCFMSession,        ASN_INTEGER,   RONLY, hhrCFMMepTable_get, 2, {1, 9}},
    {hhrCFMMepOverPW,      ASN_INTEGER,   RONLY, hhrCFMMepTable_get, 2, {1, 10}},
    {hhrCFMAisEnable,      ASN_INTEGER,   RONLY, hhrCFMMepTable_get, 2, {1, 11}},
    {hhrCFMAisInterval,    ASN_INTEGER,   RONLY, hhrCFMMepTable_get, 2, {1, 12}},
    {hhrCFMAisLevel,       ASN_OCTET_STR, RONLY, hhrCFMMepTable_get, 2, {1, 13}}
};

/*hhrCFMSLAConfigTable*/
static oid hhrCFMSLAConfigTable_oid[] = {HHRCFMOID, 4};
FindVarMethod hhrCFMSLAConfigTable_get;
struct variable2 hhrCFMSLAConfigTable_variables[] =
{
    {hhrCFMSLAMdIndex,    ASN_INTEGER,   RONLY, hhrCFMSLAConfigTable_get, 2, {1, 3}},
    {hhrCFMSLALMStatus,   ASN_INTEGER,   RONLY, hhrCFMSLAConfigTable_get, 2, {1, 4}},
    {hhrCFMSLALMInterval, ASN_INTEGER,   RONLY, hhrCFMSLAConfigTable_get, 2, {1, 5}},
    {hhrCFMSLADMStatus,   ASN_INTEGER,   RONLY, hhrCFMSLAConfigTable_get, 2, {1, 6}},
    {hhrCFMSLADMInterval, ASN_INTEGER,   RONLY, hhrCFMSLAConfigTable_get, 2, {1, 7}}
};

static oid hhrCFMSLAResultTable_oid[] = {HHRCFMOID, 5};
FindVarMethod hhrCFMSLAResultTable_get;
struct variable2 hhrCFMSLAResultTable_variables[] =
{
    {hhrCFMSLARMdIndex,               ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 3}},
    {hhrCFMSLAMaxFramelossRatioNear,  ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 4}},
    {hhrCFMSLAMinFramelossRatioNear,  ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 5}},
    {hhrCFMSLAMeanFrameLossRatioNear, ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 6}},
    {hhrCFMSLAMaxFrameLossRatioFar,   ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 7}},
    {hhrCFMSLAMinFramelossRatioFar,   ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 8}},
    {hhrCFMSLAMeanFrameLossRatioFar,  ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 9}},
    {hhrCFMSLAMaxDelay,               ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 10}},
    {hhrCFMSLAMaxDelayJitter,         ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 11}},
    {hhrCFMSLAMinDelay,               ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 12}},
    {hhrCFMSLAMinDelayJitter,         ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 13}},
    {hhrCFMSLAMeanDelay,              ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 14}},
    {hhrCFMSLAMeanDelayJitter,        ASN_INTEGER,   RONLY, hhrCFMSLAResultTable_get, 2, {1, 15}}
};

static oid hhrELPSTable_oid[] = {HHRETHPROTECTIONOID, 1};
FindVarMethod hhrELPSTable_get;
struct ipran_snmp_data_cache *hhrELPSTable_cache = NULL;
struct variable2 hhrELPSTable_variables[] =
{
    {hhrElpsProtectPortMasterIfIndex, ASN_UNSIGNED,  RONLY, hhrELPSTable_get, 2, {1, 2}},
    {hhrElpsProtectPortMasterIfDescr, ASN_OCTET_STR, RONLY, hhrELPSTable_get, 2, {1, 3}},
    {hhrElpsProtectPortbackupIfIndex, ASN_UNSIGNED,  RONLY, hhrELPSTable_get, 2, {1, 4}},
    {hhrElpsProtectPortbackupIfDescr, ASN_OCTET_STR, RONLY, hhrELPSTable_get, 2, {1, 5}},
    {hhrElpsVlan,                     ASN_OCTET_STR, RONLY, hhrELPSTable_get, 2, {1, 6}},
    {hhrElpsPrimaryVlan,              ASN_INTEGER,   RONLY, hhrELPSTable_get, 2, {1, 7}},
    {hhrElpsKeepAlive,                ASN_UNSIGNED,  RONLY, hhrELPSTable_get, 2, {1, 8}},
    {hhrElpsBackupMode,               ASN_INTEGER,   RONLY, hhrELPSTable_get, 2, {1, 9}},
    {hhrElpsBackupWtr,                ASN_UNSIGNED,  RONLY, hhrELPSTable_get, 2, {1, 10}},
    {hhrElpsPriority,                 ASN_UNSIGNED,  RONLY, hhrELPSTable_get, 2, {1, 11}},
    {hhrElpsHoldOff,                  ASN_UNSIGNED,  RONLY, hhrELPSTable_get, 2, {1, 12}},
    {hhrElpsEnable,                   ASN_INTEGER,   RONLY, hhrELPSTable_get, 2, {1, 13}},
    {hhrElpsCurrentState,             ASN_INTEGER,   RONLY, hhrELPSTable_get, 2, {1, 14}},
	{hhrElpsBindCfmSessionMaster,     ASN_INTEGER,   RONLY, hhrELPSTable_get, 2, {1, 15}},
    {hhrElpsBindCfmSessionBackup,     ASN_INTEGER,   RONLY, hhrELPSTable_get, 2, {1, 16}},
	{hhrElpsProtectCurrentWorkPort,   ASN_OCTET_STR, RONLY, hhrELPSTable_get, 2, {1, 17}}
};

/*-----------------------------------------------------------------------------------------*/
FindVarMethod hhrCFMETHTestResultTable_get;
static struct ipran_snmp_data_cache *hhrCFMETHTestResultTable_cache = NULL ;
struct variable2 hhrCFMETHTestResultTable_Variables[] =
{
    {hhrCFMETHTestSessionId,          ASN_INTEGER,   NOACCESS, hhrCFMETHTestResultTable_get, 2, {1, 1}},
    {hhrCFMETHTestTxNum,              ASN_UNSIGNED,  RONLY,    hhrCFMETHTestResultTable_get, 2, {1, 2}},
    {hhrCFMETHTestRxNum,              ASN_UNSIGNED,  RONLY,    hhrCFMETHTestResultTable_get, 2, {1, 3}},
    {hhrCFMETHTestErrorNum,           ASN_UNSIGNED,  RONLY,    hhrCFMETHTestResultTable_get, 2, {1, 4}}
} ;

oid  hhrCFMETHTestResultTable_Oid[]   = {HHRCFMOID , 8};
/*-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------*/
FindVarMethod hhrCFMExMepTable_get;
static struct ipran_snmp_data_cache *hhrCFMExMepTable_cache = NULL ;
struct variable2 hhrCFMExMepTable_Variables[] =
{
    {hhrCFMExSession,               ASN_INTEGER,      NOACCESS, hhrCFMExMepTable_get, 2, {1, 1}},
    {hhrCFMExMaIndex,               ASN_INTEGER,      RONLY,    hhrCFMExMepTable_get, 2, {1, 2}},
    {hhrCFMExMepIndex,              ASN_INTEGER,      RONLY,    hhrCFMExMepTable_get, 2, {1, 3}},
    {hhrCFMExMepDir,                ASN_INTEGER,      RONLY,    hhrCFMExMepTable_get, 2, {1, 4}},
    {hhrCFMExMepCCMEnable,          ASN_INTEGER,      RONLY,    hhrCFMExMepTable_get, 2, {1, 5}},
    {hhrCFMExMepCCMInterval,        ASN_INTEGER,      RONLY,    hhrCFMExMepTable_get, 2, {1, 6}},
    {hhrCFMExMepIfDescr,            ASN_OCTET_STR,    RONLY,    hhrCFMExMepTable_get, 2, {1, 7}},
    {hhrCFMExRMepIndex,             ASN_INTEGER,      RONLY,    hhrCFMExMepTable_get, 2, {1, 8}},
    {hhrCFMExRMepMac,               ASN_OCTET_STR,    RONLY,    hhrCFMExMepTable_get, 2, {1, 9}},
    {hhrCFMExMepStatus,             ASN_INTEGER,      RONLY,    hhrCFMExMepTable_get, 2, {1, 10}},
    {hhrCFMExMepOverPw,             ASN_INTEGER,      RONLY,    hhrCFMExMepTable_get, 2, {1, 11}},
    {hhrCFMExMepAisEnable,          ASN_INTEGER,      RONLY,    hhrCFMExMepTable_get, 2, {1, 12}},
    {hhrCFMExMepAisInterval,        ASN_INTEGER,      RONLY,    hhrCFMExMepTable_get, 2, {1, 13}},
    {hhrCFMExMepAisLevel,           ASN_OCTET_STR,    RONLY,    hhrCFMExMepTable_get, 2, {1, 14}},
    {hhrCFMExMepEthTestMode,        ASN_INTEGER,      RONLY,    hhrCFMExMepTable_get, 2, {1, 15}},
    {hhrCFMExMepLCKInterval,        ASN_INTEGER,      RONLY,    hhrCFMExMepTable_get, 2, {1, 16}},
    {hhrCFMExMepLCKLevel,           ASN_OCTET_STR,    RONLY,    hhrCFMExMepTable_get, 2, {1, 17}},
} ;

oid  hhrCFMExMepTable_Oid[]   = {HHRCFMOID , 11};
/*-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------*/
FindVarMethod hhrCFMExSLAConfigTable_get;
struct variable2 hhrCFMExSLAConfigTable_Variables[] =
{
    {hhrCFMExSLAMaIndex,        ASN_INTEGER,      NOACCESS,     hhrCFMExSLAConfigTable_get, 2, {1, 1}},
    {hhrCFMExSLAMepIndex,       ASN_INTEGER,      RONLY,        hhrCFMExSLAConfigTable_get, 2, {1, 2}},
    {hhrCFMExSLAMdIndex,        ASN_INTEGER,      RONLY,        hhrCFMExSLAConfigTable_get, 2, {1, 3}},
    {hhrCFMExSLALMStatus,       ASN_INTEGER,      RONLY,        hhrCFMExSLAConfigTable_get, 2, {1, 4}},
    {hhrCFMExSLALMInterval,     ASN_INTEGER,      RONLY,        hhrCFMExSLAConfigTable_get, 2, {1, 5}},
    {hhrCFMExSLADMStatus,       ASN_INTEGER,      RONLY,        hhrCFMExSLAConfigTable_get, 2, {1, 6}},
    {hhrCFMExSLADMInterval,     ASN_INTEGER,      RONLY,        hhrCFMExSLAConfigTable_get, 2, {1, 7}}
} ;

oid  hhrCFMExSLAConfigTable_Oid[]   = {HHRCFMOID , 13};
/*-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------*/
FindVarMethod hhrCFMExSLAResultTable_get;
static struct ipran_snmp_data_cache *hhrCFMExSLAResultTable_cache = NULL ;
struct variable2 hhrCFMExSLAResultTable_Variables[] =
{
    {hhrCFMExSLARMaIndex,                   ASN_INTEGER,      NOACCESS,     hhrCFMExSLAResultTable_get, 2, {1, 1}},
    {hhrCFMExSLARMepIndex,                  ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 2}},
    {hhrCFMExSLARMdIndex,                   ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 3}},
    {hhrCFMExSLAMaxFramelossRatioNear,      ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 4}},
    {hhrCFMExSLAMinFramelossRatioNear,      ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 5}},
    {hhrCFMExSLAMeanFrameLossRatioNear,     ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 6}},
    {hhrCFMExSLAMaxFrameLossRatioFar,       ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 7}},
    {hhrCFMExSLAMinFramelossRatioFar,       ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 8}},
    {hhrCFMExSLAMeanFrameLossRatioFar,      ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 9}},
    {hhrCFMExSLAMaxDelay,                   ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 10}},
    {hhrCFMExSLAMaxDelayJitter,             ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 11}},
    {hhrCFMExSLAMinDelay,                   ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 12}},
    {hhrCFMExSLAMinDelayJitter,             ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 13}},
    {hhrCFMExSLAMeanDelay,                  ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 14}},
    {hhrCFMExSLAMeanDelayJitter,            ASN_INTEGER,      RONLY,        hhrCFMExSLAResultTable_get, 2, {1, 15}}
};

oid  hhrCFMExSLAResultTable_Oid[]   = {HHRCFMOID , 15};
/*-----------------------------------------------------------------------------------------*/



void init_mib_cfm(void)
{
    REGISTER_MIB("hhrCFMMdTable", hhrCFMMdTable_variables,
                 variable2, hhrCFMMdTable_oid);

    REGISTER_MIB("hhrCFMMaTable", hhrCFMMaTable_variables,
                 variable2, hhrCFMMaTable_oid);

    REGISTER_MIB("hhrCFMMepTable", hhrCFMMepTable_variables,
                 variable2, hhrCFMMepTable_oid);

    REGISTER_MIB("hhrCFMSLAConfigTable", hhrCFMSLAConfigTable_variables,
                 variable2, hhrCFMSLAConfigTable_oid);

    REGISTER_MIB("hhrCFMSLAResultTable", hhrCFMSLAResultTable_variables,
                 variable2, hhrCFMSLAResultTable_oid);

    REGISTER_MIB("hhrELPSTable", hhrELPSTable_variables,
                 variable2, hhrELPSTable_oid);

///*---------------------------------for seesion id-------------------------------*/
    REGISTER_MIB("hhrCFMETHTestResultTable", hhrCFMETHTestResultTable_Variables,
                 variable2, hhrCFMETHTestResultTable_Oid);

    REGISTER_MIB("hhrCFMExMepTable", hhrCFMExMepTable_Variables,
                 variable2, hhrCFMExMepTable_Oid);

    REGISTER_MIB("hhrCFMSLAResultTable", hhrCFMExSLAConfigTable_Variables,
                 variable2, hhrCFMExSLAConfigTable_Oid);

    REGISTER_MIB("hhrCFMExSLAResultTable", hhrCFMExSLAResultTable_Variables,
                 variable2, hhrCFMExSLAResultTable_Oid);
}


void hhrCFMMdTable_node_free(struct cfm_md *node)
{
    XFREE(0, node);
}

void hhrCFMMaTable_node_free(struct cfm_ma *node)
{
    XFREE(0, node);
}

void hhrCFMMepTable_node_free(struct cfm_sess *node)
{
    XFREE(0, node);
}

void hhrCFMSLAResultTable_node_free(struct sla_measure *node)
{
    XFREE(0, node);
}



static void *cfm_node_xcalloc(int table_num)
{
    switch (table_num)
    {
        case hhrCFMMdTable:
            return XCALLOC(0, sizeof(struct cfm_md));
            break;

        case hhrCFMMaTable:
            return XCALLOC(0, sizeof(struct cfm_ma));
            break;

        case hhrCFMMepTable:
            return XCALLOC(0, sizeof(struct cfm_sess));
            break;

        case hhrCFMSLAResultTable:
            return XCALLOC(0, sizeof(struct sla_measure));
            break;

        default:
            break;
    }

    return NULL;
}

static void cfm_node_add(int table_num, void *node)
{
    void *node_add = NULL;

    switch (table_num)
    {
        case hhrCFMMdTable:
            node_add = cfm_node_xcalloc(hhrCFMMdTable);
            memcpy(node_add, node, sizeof(struct cfm_md));
            listnode_add(hhrCFMMdTable_list, node_add);
            break;

        case hhrCFMMaTable:
            node_add = cfm_node_xcalloc(hhrCFMMaTable);
            memcpy(node_add, node, sizeof(struct cfm_ma));
            listnode_add(hhrCFMMaTable_list, node_add);
            break;

        case hhrCFMMepTable:
            node_add = cfm_node_xcalloc(hhrCFMMepTable);
            memcpy(node_add, node, sizeof(struct cfm_sess));
            listnode_add(hhrCFMMepTable_list, node_add);
            break;

        case hhrCFMSLAResultTable:
            node_add = cfm_node_xcalloc(hhrCFMSLAResultTable);
            memcpy(node_add, node, sizeof(struct sla_measure));
            listnode_add(hhrCFMSLAResultTable_list, node_add);
            break;

        default:
            break;
    }
}

static void cfm_node_del_all(int table_num)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' \n", __FILE__, __LINE__, __func__);

    switch (table_num)
    {
        case hhrCFMMdTable:
            list_delete_all_node(hhrCFMMdTable_list);
            break;

        case hhrCFMMaTable:
            list_delete_all_node(hhrCFMMaTable_list);
            break;

        case hhrCFMMepTable:
            list_delete_all_node(hhrCFMMepTable_list);
            break;

        case hhrCFMSLAResultTable:
            list_delete_all_node(hhrCFMSLAResultTable_list);
            break;

        default:
            break;
    }
}


void cfm_list_init(void)
{
    hhrCFMMdTable_list = list_new();
    hhrCFMMdTable_list->del = (void (*)(void *))hhrCFMMdTable_node_free;

    hhrCFMMaTable_list = list_new();
    hhrCFMMaTable_list->del = (void (*)(void *))hhrCFMMaTable_node_free;

    hhrCFMMepTable_list = list_new();
    hhrCFMMepTable_list->del = (void (*)(void *))hhrCFMMepTable_node_free;

    hhrCFMSLAResultTable_list = list_new();
    hhrCFMSLAResultTable_list->del = (void (*)(void *))hhrCFMSLAResultTable_node_free;
}

/* md */
#if 0
struct cfm_md *cfm_com_get_md_instance(uchar md_id, int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg = NULL ;

    if (!pdata_num)
    {
        return NULL;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s' md_id [%d] \n", __FILE__, __LINE__, __func__ , md_id);

    pmesg = ipc_send_common_wait_reply1(&md_id, 1, 1, MODULE_ID_L2,
                                        module_id, IPC_TYPE_CFM, CFM_SNMP_MD_GET , IPC_OPCODE_GET_BULK , 0);


    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct cfm_md *)pmesg->msg_data;
    }

    return NULL;
}
#endif

struct ipc_mesg_n *cfm_com_get_md_instance(uchar md_id, int module_id)
{
	struct ipc_mesg_n *pRsvMsg = NULL ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s' md_id [%d] \n", __FILE__, __LINE__, __func__ , md_id);

    pRsvMsg = ipc_sync_send_n2(&md_id, 1, 1, MODULE_ID_L2,
                          module_id, IPC_TYPE_CFM, CFM_SNMP_MD_GET , IPC_OPCODE_GET_BULK , 0, 0);

    return pRsvMsg;
}
/* ma */
#if 0
struct cfm_ma *cfm_com_get_ma_instance(struct cfm_ma_key *ma_key, int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg = NULL ;

    if (!ma_key || !pdata_num)
    {
        return NULL;
    }


    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s' md_id [%d] , ma_id [%d] \n", __FILE__, __LINE__, __func__ , ma_key->md_id, ma_key->ma_id);

    pmesg = ipc_send_common_wait_reply1(ma_key, sizeof(struct cfm_ma_key), 1, MODULE_ID_L2,
                                        module_id, IPC_TYPE_CFM, CFM_SNMP_MA_GET, IPC_OPCODE_GET_BULK , 0);


    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct cfm_ma *)pmesg->msg_data;
    }

    return NULL;
}
#endif

struct ipc_mesg_n *cfm_com_get_ma_instance(struct cfm_ma_key *ma_key, int module_id)
{
    struct ipc_mesg_n *pRsvMsg = NULL;

    if (!ma_key)
    {
        return NULL;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s' md_id [%d] , ma_id [%d] \n", __FILE__, __LINE__, __func__ , ma_key->md_id, ma_key->ma_id);

    pRsvMsg = ipc_sync_send_n2(ma_key, sizeof(struct cfm_ma_key), 1, MODULE_ID_L2,
                                        module_id, IPC_TYPE_CFM, CFM_SNMP_MA_GET, IPC_OPCODE_GET_BULK , 0, 0);

    return pRsvMsg;
}
/* cfm get session instance,ma+localmep is used for key*/
#if 0
struct cfm_sess *cfm_com_get_session_instance(struct cfm_session_key *sess_key, int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg = NULL ;

    if (!sess_key || !pdata_num)
    {
        return NULL;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s' ma_id [%d] , sess_id [%d] \n", __FILE__, __LINE__, __func__ , sess_key->ma_id, sess_key->sess_id);

    pmesg = ipc_send_common_wait_reply1(sess_key, sizeof(struct cfm_session_key), 1, MODULE_ID_L2,
                                        module_id, IPC_TYPE_CFM, CFM_SNMP_SESSION_GET, IPC_OPCODE_GET_BULK , 0);


    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct cfm_sess *)pmesg->msg_data;
    }

    return NULL;
}
#endif

struct ipc_mesg_n *cfm_com_get_session_instance(struct cfm_session_key *sess_key, int module_id)
{
    struct ipc_mesg_n *pRsvMsg = NULL;

    if (!sess_key)
    {
        return NULL;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s' ma_id [%d] , sess_id [%d] \n", __FILE__, __LINE__, __func__ , sess_key->ma_id, sess_key->sess_id);

    pRsvMsg = ipc_sync_send_n2(sess_key, sizeof(struct cfm_session_key), 1, MODULE_ID_L2,
                                        module_id, IPC_TYPE_CFM, CFM_SNMP_SESSION_GET, IPC_OPCODE_GET_BULK , 0, 0);

    return pRsvMsg;
}
/* cfm get session instance new, session id is used for key*/
#if 0
struct cfm_sess *cfm_com_get_session_instance_new(struct cfm_session_key *sess_key, int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg = NULL ;

    if (!sess_key || !pdata_num)
    {
        return NULL;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s' , sess_id [%d] \n", __FILE__, __LINE__, __func__ ,  sess_key->sess_id);

    pmesg = ipc_send_common_wait_reply1(sess_key, sizeof(struct cfm_session_key), 1, MODULE_ID_L2,
                                        module_id, IPC_TYPE_CFM, CFM_SNMP_SESSION_GET_NEW, IPC_OPCODE_GET_BULK , 0);


    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct cfm_sess *)pmesg->msg_data;
    }

    return NULL;
}
#endif

struct ipc_mesg_n *cfm_com_get_session_instance_new(struct cfm_session_key *sess_key, int module_id)
{
    struct ipc_mesg_n *pRsvMsg = NULL;

    if (!sess_key)
    {
        return NULL;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s' , sess_id [%d] \n", __FILE__, __LINE__, __func__ ,  sess_key->sess_id);

    pRsvMsg = ipc_sync_send_n2(sess_key, sizeof(struct cfm_session_key), 1, MODULE_ID_L2,
                                        module_id, IPC_TYPE_CFM, CFM_SNMP_SESSION_GET_NEW, IPC_OPCODE_GET_BULK , 0, 0);

    return pRsvMsg;
}
/* cfm get session sla instance,ma+localmep is used for key*/
#if 0
struct sla_measure *cfm_com_get_sla_instance(struct cfm_session_key *sess_key, int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg = NULL ;

    if (!sess_key || !pdata_num)
    {
        return NULL;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s' ma_id [%d] , sess_id [%d] \n", __FILE__, __LINE__, __func__ , sess_key->ma_id, sess_key->sess_id);

    pmesg = ipc_send_common_wait_reply1(sess_key, sizeof(struct cfm_session_key), 1, MODULE_ID_L2,
                                        module_id, IPC_TYPE_CFM, CFM_SNMP_SLA_GET, IPC_OPCODE_GET_BULK , 0);


    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct sla_measure *)pmesg->msg_data;
    }

    return NULL;
}
#endif

struct ipc_mesg_n *cfm_com_get_sla_instance(struct cfm_session_key *sess_key, int module_id)
{
	struct ipc_mesg_n *pRsvMsg = NULL;

    if (!sess_key)
    {
        return NULL;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s' ma_id [%d] , sess_id [%d] \n", __FILE__, __LINE__, __func__ , sess_key->ma_id, sess_key->sess_id);

    pRsvMsg = ipc_sync_send_n2(sess_key, sizeof(struct cfm_session_key), 1, MODULE_ID_L2,
                                        module_id, IPC_TYPE_CFM, CFM_SNMP_SLA_GET, IPC_OPCODE_GET_BULK , 0, 0);

    return pRsvMsg;
}
/* cfm get session sla instance new, session id is used for key*/
#if 0
struct sla_measure *cfm_com_get_sla_instance_new(struct cfm_session_key *sess_key, int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg = NULL ;

    if (!sess_key || !pdata_num)
    {
        return NULL;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s' , sess_id [%d] \n", __FILE__, __LINE__, __func__ , sess_key->sess_id);

    pmesg = ipc_send_common_wait_reply1(sess_key, sizeof(struct cfm_session_key), 1, MODULE_ID_L2,
                                        module_id, IPC_TYPE_CFM, CFM_SNMP_SLA_GET_NEW, IPC_OPCODE_GET_BULK , 0);


    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct sla_measure *)pmesg->msg_data;
    }

    return NULL;
}
#endif

struct ipc_mesg_n *cfm_com_get_sla_instance_new(struct cfm_session_key *sess_key, int module_id)
{
    struct ipc_mesg_n *pRsvMsg = NULL ;

    if (!sess_key)
    {
        return NULL;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s' , sess_id [%d] \n", __FILE__, __LINE__, __func__ , sess_key->sess_id);

    pRsvMsg = ipc_sync_send_n2(sess_key, sizeof(struct cfm_session_key), 1, MODULE_ID_L2,
                                        module_id, IPC_TYPE_CFM, CFM_SNMP_SLA_GET_NEW, IPC_OPCODE_GET_BULK , 0, 0);

    return pRsvMsg;
}


static void *cfm_node_lookup(int exact, int table_num, void *index_input)
{
    struct listnode  *node, *nnode;

    struct cfm_md   *data1_find;
    u_int32_t       index1;

    struct cfm_ma   *data2_find;
    struct cfm_ma_key       *index2;

    struct cfm_sess     *data3_find;
    struct cfm_session_key      *index3;

    struct sla_measure  *data4_find;
    struct cfm_session_key      *index4;

    switch (table_num)
    {
        case hhrCFMMdTable:
            index1 = *((u_int32_t *)index_input);

            for (ALL_LIST_ELEMENTS(hhrCFMMdTable_list, node, nnode, data1_find))
            {
                if (0 == index1)
                {
                    return hhrCFMMdTable_list->head->data;
                }

                if (data1_find->md_index == index1)
                {
                    if (1 == exact) //get
                    {
                        return data1_find;
                    }
                    else
                    {
                        if (NULL == node->next)
                        {
                            return NULL;
                        }
                        else
                        {
                            return node->next->data;
                        }
                    }
                }
            }

            return NULL;

        case hhrCFMMaTable:
            index2 = ((struct cfm_ma_key *)index_input);

            for (ALL_LIST_ELEMENTS(hhrCFMMaTable_list, node, nnode, data2_find))
            {
                if (0 == index2->md_id && 0 == index2->ma_id)
                {
                    return hhrCFMMaTable_list->head->data;
                }

                if (data2_find->md_index == index2->md_id &&
                        data2_find->ma_index == index2->ma_id)
                {
                    if (1 == exact) //get
                    {
                        return data2_find;
                    }
                    else
                    {
                        if (NULL == node->next)
                        {
                            return NULL;
                        }
                        else
                        {
                            return node->next->data;
                        }
                    }
                }
            }

            return NULL;

        case hhrCFMMepTable:
            index3 = ((struct cfm_session_key *)index_input);

            for (ALL_LIST_ELEMENTS(hhrCFMMepTable_list, node, nnode, data3_find))
            {
                if (0 ==  index3->ma_id && 0 == index3->sess_id)
                {

                    return hhrCFMMepTable_list->head->data;
                }

                if (data3_find->ma_index  == index3->ma_id &&
                        data3_find->local_mep == index3->sess_id)
                {
                    if (1 == exact) //get
                    {
                        return data3_find;
                    }
                    else
                    {
                        if (NULL == node->next)
                        {
                            return NULL;
                        }
                        else
                        {
                            return node->next->data;
                        }
                    }
                }
            }

            return NULL;

        case hhrCFMSLAResultTable:
            index4 = ((struct cfm_session_key *)index_input);

            for (ALL_LIST_ELEMENTS(hhrCFMSLAResultTable_list, node, nnode, data4_find))
            {
                if (0 ==  index4->ma_id && 0 == index4->sess_id)
                {

                    return hhrCFMSLAResultTable_list->head->data;
                }

                if (data4_find->ma_index  == index4->ma_id &&
                        data4_find->local_mep == index4->sess_id)
                {
                    if (1 == exact) //get
                    {
                        return data4_find;
                    }
                    else
                    {
                        if (NULL == node->next)
                        {
                            return NULL;
                        }
                        else
                        {
                            return node->next->data;
                        }
                    }
                }
            }

            return NULL;

        default:
            return NULL;
    }
}

#if 0
struct hhrelps_snmp *hhrethprotection_bulk(uint16_t hhrElpsSessionId, int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg = NULL ;


    if (hhrElpsSessionId == 0)
    {
        pmesg = ipc_send_common_wait_reply1(&hhrElpsSessionId, 0, 1, MODULE_ID_L2,
                                            module_id, IPC_TYPE_ELPS, 0 , IPC_OPCODE_GET_BULK, hhrElpsSessionId);
    }
    else
    {
        pmesg = ipc_send_common_wait_reply1(&hhrElpsSessionId, 4, 1, MODULE_ID_L2,
                                            module_id, IPC_TYPE_ELPS, 0 , IPC_OPCODE_GET_BULK, hhrElpsSessionId);
    }

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct hhrelps_snmp *)pmesg->msg_data;
    }

    return NULL;
}
#endif

struct ipc_mesg_n *hhrethprotection_bulk(uint16_t hhrElpsSessionId, int module_id)
{
    struct ipc_mesg_n *pRsvMsg = NULL;


    if (hhrElpsSessionId == 0)
    {
        pRsvMsg = ipc_sync_send_n2(&hhrElpsSessionId, 0, 1, MODULE_ID_L2,
                                            module_id, IPC_TYPE_ELPS, 0 , IPC_OPCODE_GET_BULK, hhrElpsSessionId, 0);
    }
    else
    {
        pRsvMsg = ipc_sync_send_n2(&hhrElpsSessionId, 4, 1, MODULE_ID_L2,
                                            module_id, IPC_TYPE_ELPS, 0 , IPC_OPCODE_GET_BULK, hhrElpsSessionId, 0);
    }

    return pRsvMsg;
}

int hhrCFMMdTable_get_data_from_ipc(u_int32_t index)
{
	struct ipc_mesg_n *pRsvMsg = NULL;
    struct cfm_md *pcfm_md = NULL;
    int data_num = 0;
    int ret = 0;

    pRsvMsg = cfm_com_get_md_instance(index, MODULE_ID_SNMPD);
	if (NULL == pRsvMsg)
	{
		return FALSE;
	}

	pcfm_md = (struct cfm_md *)pRsvMsg->msg_data;
	if (NULL == pcfm_md)
	{
		mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
		
		return FALSE;
	}

	data_num = pRsvMsg->msghdr.data_num;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);
	
    for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
    {
        cfm_node_add(hhrCFMMdTable, pcfm_md);
        pcfm_md++;
    }

    time(&hhrCFMMdTable_time_old);  //refresh time_old after refresh cache data

	mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
    
    return TRUE; 
}

struct cfm_md *hhrCFMMdTable_get_data_by_index(int exact, u_int32_t *index, u_int32_t *index_next)
{
    struct cfm_md *pfm_md = NULL;
    struct cfm_md *entry_temp;
    u_int32_t index_temp;
    static time_t getNext_time_old;    //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > hhrCFMMdTable_GET_NEXT_INTERVAL))
    {
        hhrCFMMdTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrCFMMdTable_time_old) > hhrCFMMdTable_REFRESH_TIME) && \
            (UNLOCK == hhrCFMMdTable_lock))
    {
        if (NULL == hhrCFMMdTable_list)
        {
            hhrCFMMdTable_list = list_new();
            hhrCFMMdTable_list->del = (void (*)(void *))hhrCFMMdTable_node_free;
        }

        if (0 != hhrCFMMdTable_list->count)
        {
            cfm_node_del_all(hhrCFMMdTable);
        }
    }

    /* then lock */
    hhrCFMMdTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrCFMMdTable_list)
    {
        hhrCFMMdTable_list = list_new();
        hhrCFMMdTable_list->del = (void (*)(void *))hhrCFMMdTable_node_free;
    }

    if (0 == hhrCFMMdTable_list->count)
    {
        index_temp = 0;
        ret = hhrCFMMdTable_get_data_from_ipc(index_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    pfm_md = (struct cfm_md *)cfm_node_lookup(exact, hhrCFMMdTable, index);

    if (NULL == pfm_md) //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrCFMMdTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct cfm_md *)hhrCFMMdTable_list->tail->data;
            ret = hhrCFMMdTable_get_data_from_ipc(entry_temp->md_index);

            if (FALSE == ret)
            {
                hhrCFMMdTable_lock = UNLOCK;    //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            pfm_md = (struct cfm_md *)cfm_node_lookup(exact, hhrCFMMdTable, index);

            if (NULL != pfm_md)
            {
                *index_next = pfm_md->md_index;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrCFMMdTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pfm_md;
            }
        }
    }

    *index_next = pfm_md->md_index;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrCFMMdTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pfm_md;
}


u_char *hhrCFMMdTable_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    struct cfm_md *phhrCFMMdTable = NULL;
    int index = 0 ;
    int index_next = 0 ;
    int ret = 0;
    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index , exact);

    if (ret < 0)
    {
        return NULL;
    }


    /* get data by index and renew index_next */
    phhrCFMMdTable = hhrCFMMdTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == phhrCFMMdTable)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index_next [%d]\n", __FILE__, __LINE__, __func__ , index_next);
        ipran_snmp_int_index_set(vp, name, length, index_next);
    }

    switch (vp->magic)
    {
        case hhrCFMMdLevel:
            *var_len = sizeof(int);
            int_value = phhrCFMMdTable->level ;
            return (u_char *)(&int_value);

        case hhrCFMMdName:
            memset(str_value, 0x00, STRING_LEN);
            snprintf(str_value, STRING_LEN, "%s", phhrCFMMdTable->name);
            *var_len = strlen(str_value);
            return (str_value);

        default:
            return (NULL);
    }
}

int hhrCFMMaTable_get_data_from_ipc(struct cfm_ma_key *ma_key)
{
	struct ipc_mesg_n *pRsvMsg = NULL;
    struct cfm_ma *pcfm_ma = NULL;
    int data_num = 0;
    int ret = 0;

    pRsvMsg = cfm_com_get_ma_instance(ma_key, MODULE_ID_SNMPD);
	if (NULL == pRsvMsg)
	{
		return FALSE;
	}

	
	pcfm_ma = (struct cfm_ma *)pRsvMsg->msg_data;
    if (NULL == pcfm_ma)
    {
    	mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
        return FALSE;
    }

	data_num = pRsvMsg->msghdr.data_num;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);
	
    for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
    {
        cfm_node_add(hhrCFMMaTable, pcfm_ma);
        pcfm_ma++;
    }

    time(&hhrCFMMaTable_time_old);  //refresh time_old after refresh cache data
    
	mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
	
    return TRUE;
}

struct cfm_ma *hhrCFMMaTable_get_data_by_index(int exact, u_int32_t *md_index, u_int32_t *md_index_next ,
                                               u_int32_t *ma_index, u_int32_t *ma_index_next)
{
    struct cfm_ma *pfm_ma = NULL;
    struct cfm_ma *entry_temp;
    u_int32_t index_temp;
    static time_t getNext_time_old;
    struct cfm_ma_key index_key ;

    index_key.md_id = *md_index ;
    index_key.ma_id = *ma_index ;

    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > hhrCFMMaTable_GET_NEXT_INTERVAL))
    {
        hhrCFMMaTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrCFMMaTable_time_old) > hhrCFMMaTable_REFRESH_TIME) && \
            (UNLOCK == hhrCFMMaTable_lock))
    {
        if (NULL == hhrCFMMaTable_list)
        {
            hhrCFMMaTable_list = list_new();
            hhrCFMMaTable_list->del = (void (*)(void *))hhrCFMMaTable_node_free;
        }

        if (0 != hhrCFMMaTable_list->count)
        {
            cfm_node_del_all(hhrCFMMaTable);
        }
    }

    /* then lock */
    hhrCFMMaTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrCFMMaTable_list)
    {
        hhrCFMMaTable_list = list_new();
        hhrCFMMaTable_list->del = (void (*)(void *))hhrCFMMaTable_node_free;
    }

    if (0 == hhrCFMMaTable_list->count)
    {
        index_key.ma_id = 0 ;
        index_key.md_id = 0 ;
        ret = hhrCFMMaTable_get_data_from_ipc(&index_key);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    //index_key.md_id = *md_index ;
    //index_key.ma_id= *ma_index ;
    pfm_ma = (struct cfm_ma *)cfm_node_lookup(exact, hhrCFMMaTable, &index_key);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s':\n", __FILE__, __LINE__, __func__);

    if (NULL == pfm_ma) //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrCFMMaTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct cfm_ma *)hhrCFMMaTable_list->tail->data;

            index_key.ma_id = entry_temp->ma_index ;
            index_key.md_id = entry_temp->md_index ;

            ret = hhrCFMMaTable_get_data_from_ipc(&index_key);

            if (FALSE == ret)
            {
                hhrCFMMaTable_lock = UNLOCK;    //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            pfm_ma = (struct cfm_ma *)cfm_node_lookup(exact, hhrCFMMaTable, &index_key);

            if (NULL != pfm_ma)
            {
                *md_index_next = pfm_ma->md_index;
                *ma_index_next = pfm_ma->ma_index;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrCFMMaTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pfm_ma;
            }
        }
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': md_index_next [%d] ma_index_next [%d]\n",
               __FILE__, __LINE__, __func__ , pfm_ma->md_index , pfm_ma->ma_index);
    *md_index_next = pfm_ma->md_index;
    *ma_index_next = pfm_ma->ma_index;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrCFMMaTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pfm_ma;
}


u_char *hhrCFMMaTable_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    struct cfm_ma *phhrCFMMaTable = NULL;
    struct cfm_ma_key index;
    struct cfm_ma_key index_next;
    int index_md = 0;
    int index_ma = 0;
    int next_md = 0;
    int next_ma = 0;
    int ret = 0;

    memset(&index,      0, sizeof(struct cfm_ma_key));
    memset(&index_next, 0, sizeof(struct cfm_ma_key));

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index_md , &index_ma , exact);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index.md [%d] index.ma [%d] \n", __FILE__, __LINE__, __func__ , index_md, index_ma);

    if (ret < 0)
    {
        return NULL;
    }

    /* get data by index and renew index_next */
    phhrCFMMaTable = hhrCFMMaTable_get_data_by_index(exact, &index_md, &next_md,
                                                     &index_ma, &next_ma);

    if (NULL == phhrCFMMaTable)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': md_index_next [%d] ma_index_next [%d]\n",
                   __FILE__, __LINE__, __func__ , next_md , next_ma);
        ipran_snmp_intx2_index_set(vp, name, length, next_md, next_ma);
    }

    switch (vp->magic)
    {
        case hhrCFMMaRelatedVlan:
            *var_len = sizeof(int);
            int_value = phhrCFMMaTable->vlan;
            return (u_char *)(&int_value);

        case hhrCFMMaLevel:
            *var_len = sizeof(int);
            int_value = phhrCFMMaTable->priority;
            return (u_char *)(&int_value);

        case hhrCFMMaName:
            memset(str_value, 0x00, STRING_LEN);
            snprintf(str_value, STRING_LEN, "%s", phhrCFMMaTable->name);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrCFMMaProtocolType:
            *var_len = sizeof(int);
            int_value = (1 == phhrCFMMaTable->y1731_en) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrCFMMaMipEnable:
            *var_len = sizeof(int);
            int_value = (1 == phhrCFMMaTable->mip_enable) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrCFMMaY1731:
            *var_len = sizeof(int);
            int_value = (1 == phhrCFMMaTable->y1731_en) ? 1 : 2;
            return (u_char *)(&int_value);

        default:
            return (NULL);
    }
}

int hhrCFMMepTable_get_data_from_ipc(struct cfm_session_key *sess_key)
{
	struct ipc_mesg_n *pRsvMsg = NULL;
    struct cfm_sess *pcfm_sess = NULL;
    int data_num = 0;
    int ret = 0;

    pRsvMsg = cfm_com_get_session_instance(sess_key, MODULE_ID_SNMPD);
	if (NULL == pRsvMsg)
	{
		return FALSE;
	}
	
	pcfm_sess = (struct cfm_sess *)pRsvMsg->msg_data;
	if (NULL == pcfm_sess)
	{
		mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
		return FALSE;
	}

	data_num = pRsvMsg->msghdr.data_num;
	
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
    {
        cfm_node_add(hhrCFMMepTable, pcfm_sess);
        pcfm_sess++;
    }

    time(&hhrCFMMepTable_time_old);

	mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
	
    return TRUE;
}

struct cfm_sess *hhrCFMMepTable_get_data_by_index(int exact,
                                                  u_int32_t *ma_index, u_int32_t *ma_index_next,
                                                  u_int32_t *mep_index, u_int32_t *mep_index_next)
{
    struct cfm_sess *pcfm_sess = NULL;
    struct cfm_sess *entry_temp;
    static time_t getNext_time_old;    //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    struct cfm_session_key index_sess ;
    u_int32_t *aaaa = 0 ;

    index_sess.ma_id = *ma_index ;
    index_sess.sess_id = *mep_index ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ma_index [%d] mep_index[%d]\n",
               __FILE__, __LINE__, __func__ , *ma_index , *mep_index);
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > hhrCFMMepTable_GET_NEXT_INTERVAL))
    {
        hhrCFMMepTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrCFMMepTable_time_old) > hhrCFMMepTable_REFRESH_TIME) && \
            (UNLOCK == hhrCFMMepTable_lock))
    {
        if (NULL == hhrCFMMepTable_list)
        {
            hhrCFMMepTable_list = list_new();
            hhrCFMMepTable_list->del = (void (*)(void *))hhrCFMMepTable_node_free;
        }

        if (0 != hhrCFMMepTable_list->count)
        {
            cfm_node_del_all(hhrCFMMepTable);
        }
    }

    /* then lock */
    hhrCFMMepTable_lock = LOCK;

    if (0 == hhrCFMMepTable_list->count)
    {
        memset(&index_sess, 0, sizeof(struct cfm_session_key));
        index_sess.ma_id = *ma_index ;
        index_sess.sess_id = *mep_index ;
        ret = hhrCFMMepTable_get_data_from_ipc(&index_sess);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    index_sess.ma_id = *ma_index ;
    index_sess.sess_id = *mep_index ;
    pcfm_sess = (struct cfm_sess *)cfm_node_lookup(exact, hhrCFMMepTable, &index_sess);

    if (NULL == pcfm_sess)  //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            if (NULL == hhrCFMMepTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct cfm_sess *)hhrCFMMepTable_list->tail->data;
            memset(&index_sess,     0, sizeof(struct cfm_session_key));
            index_sess.ma_id = entry_temp->ma_index ;
            index_sess.sess_id = entry_temp->local_mep ;
            ret = hhrCFMMepTable_get_data_from_ipc(&index_sess);

            if (FALSE == ret)
            {
                hhrCFMMepTable_lock = UNLOCK;   //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            index_sess.ma_id = *ma_index ;
            index_sess.sess_id = *mep_index ;
            pcfm_sess = (struct cfm_sess *)cfm_node_lookup(exact, hhrCFMMepTable, &index_sess);

            if (NULL != pcfm_sess)
            {
                *ma_index_next = pcfm_sess->ma_index;
                *mep_index_next = pcfm_sess->local_mep;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrCFMMepTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pcfm_sess;
            }
        }
    }

    *ma_index_next = pcfm_sess->ma_index;
    *mep_index_next = pcfm_sess->local_mep;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrCFMMepTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pcfm_sess;
}


u_char *hhrCFMMepTable_get(struct variable *vp,
                           oid *name,
                           size_t *length,
                           int exact, size_t *var_len, WriteMethod **write_method)
{
    struct cfm_sess *phhrCFMMepTable = NULL;
    struct cfm_session_key index;
    struct cfm_session_key index_next;
    memset(&index,      0, sizeof(struct cfm_session_key));
    memset(&index_next, 0, sizeof(struct cfm_session_key));
    int ma_id = 0 ;
    int ma_next = 0 ;
    int i;
    int sess_id = 0 ;
    int sess_next = 0 ;

    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &ma_id, &sess_id, exact);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ma_id [%d] sess_id [%d] \n", __FILE__, __LINE__, __func__ , ma_id, sess_id);

    if (ret < 0)
    {
        return NULL;
    }

    /* get data by index and renew index_next */
    phhrCFMMepTable = hhrCFMMepTable_get_data_by_index(exact, &ma_id, &ma_next,
                                                       &sess_id, &sess_next);

    if (NULL == phhrCFMMepTable)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ma_next [%d] sess_next [%d]\n", __FILE__, __LINE__, __func__ ,
                   ma_next , sess_next);
        ipran_snmp_intx2_index_set(vp, name, length, ma_next, sess_next);
    }

    switch (vp->magic)
    {
        case hhrCFMMepDir:
            *var_len = sizeof(int);
            int_value = phhrCFMMepTable->direct ;

            if (MEP_DIRECT_UP == int_value)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrCFMMepCCMEnable:
            *var_len = sizeof(int);
            int_value = phhrCFMMepTable->cc_enable ;

            if (1 == int_value)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrCFMMepCCMInterval:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->cc_interval;

            if (3 == uint_value)
            {
                uint_value = 1;
            }
            else if (10 == uint_value)
            {
                uint_value = 2;
            }
            else if (30 == int_value)
            {
                int_value = 3;
            }
            else if (100 == uint_value)
            {
                uint_value = 4;
            }
            else if (1000 == uint_value)
            {
                uint_value = 5;
            }
            else if (10000 == uint_value)
            {
                uint_value = 6;
            }

            return (u_char *)(&uint_value);

        case hhrCFMMepIfDescr:
            memset(str_value, 0, STRING_LEN);
            ifm_get_name_by_ifindex(phhrCFMMepTable->ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrCFMRMepIndex:
            *var_len = sizeof(int);
            int_value = phhrCFMMepTable->remote_mep ;
            return (u_char *)(&int_value);

        case hhrCFMRMepMac:
            *var_len = 6;

            if (phhrCFMMepTable->dmac_valid)
            {
                memcpy(mac_value, phhrCFMMepTable->dmac , 6);
            }
            else
            {
                memset(mac_value, 0 , 6);
            }

            return (mac_value);

        case hhrCFMMepStatus:
            *var_len = sizeof(int);
            int_value = phhrCFMMepTable->state ;

            if (OAM_STATUS_DISABLE == int_value)
            {
                int_value = 1;
            }
            else if (OAM_STATUS_ENABLE == int_value)
            {
                int_value = 2;
            }
            else if (OAM_STATUS_DOWN == int_value)
            {
                int_value = 3;
            }
            else if (OAM_STATUS_UP == int_value)
            {
                int_value = 4;
            }
            else if (OAM_STATUS_STOP == int_value)
            {
                int_value = 5;
            }
            else
            {
                int_value = 6;
            }

            return (u_char *)(&int_value);

        case hhrCFMSession:
            *var_len = sizeof(int);
            int_value = phhrCFMMepTable->sess_id ;
            return (u_char *)(&int_value);

        case hhrCFMMepOverPW:
            *var_len = sizeof(int);
            int_value = phhrCFMMepTable->over_pw ;

            if (int_value == 1)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrCFMAisEnable:
            *var_len = sizeof(int);
            int_value = phhrCFMMepTable->ais_enable ;

            if (int_value == 1)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrCFMAisInterval:
            *var_len = sizeof(int);

            if (1 == phhrCFMMepTable->ais_send_interval)
            {
                int_value = 1;
            }
            else if (60 == phhrCFMMepTable->ais_send_interval)
            {
                int_value = 2;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)(&int_value);

        case hhrCFMAisLevel:
            memset(str_value, 0, STRING_LEN);

            if (phhrCFMMepTable->ais_enable)
            {
                for (i = 0; i < 8; i++)
                {
                    if (phhrCFMMepTable->ais_send_level & (0x01 << i))
                    {
                        sprintf(str_value + strlen(str_value), "%d,", i);
                    }
                }

                if (strlen(str_value) > 1)
                {
                    str_value[strlen(str_value) - 1] = 0;
                }
            }
            else
            {
                sprintf(str_value, "%s", "--");
            }

            *var_len = strlen(str_value);
            return str_value;

        case hhrCFMMepEthTestMode:
            *var_len = sizeof(int);
            int_value = phhrCFMMepTable->tst_mode ;

            if (int_value == 0)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrCFMMepLCKInterval:
            *var_len = sizeof(int);
            int_value = phhrCFMMepTable->lck_send_interval ;

            if (int_value == 0)
            {
                int_value = 0;
            }
            else if (int_value == 1)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrCFMMepLCKLevel:
            memset(str_value, 0, STRING_LEN);

            if (phhrCFMMepTable->lck_send_level)
            {
                for (i = 0; i < 8; i++)
                {
                    if (phhrCFMMepTable->ais_send_level & (0x01 << i))
                    {
                        sprintf(str_value + strlen(str_value), "%d,", i);
                    }
                }

                if (strlen(str_value) > 1)
                {
                    str_value[strlen(str_value) - 1] = 0;
                }
            }
            else
            {
                sprintf(str_value, "%s", "--");
            }

            *var_len = strlen(str_value);
            return str_value;

        default :
            return (NULL);
    }
}

u_char *hhrCFMSLAConfigTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct cfm_sess *phhrCFMMepTable = NULL;
    struct cfm_session_key index;
    struct cfm_session_key index_next;
    memset(&index,      0, sizeof(struct cfm_session_key));
    memset(&index_next, 0, sizeof(struct cfm_session_key));
    int ma_id = 0 ;
    int ma_next = 0 ;

    int sess_id = 0 ;
    int sess_next = 0 ;


    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &ma_id, &sess_id, exact);

    if (ret < 0)
    {
        return NULL;
    }

    /* get data by index and renew index_next */
    phhrCFMMepTable = hhrCFMMepTable_get_data_by_index(exact, &ma_id, &ma_next,
                                                       &sess_id, &sess_next);

    if (NULL == phhrCFMMepTable)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ma_next [%d] sess_next [%d]\n", __FILE__, __LINE__, __func__ ,
                   ma_next , sess_next);
        ipran_snmp_intx2_index_set(vp, name, length, ma_next, sess_next);
    }

    switch (vp->magic)
    {
        case hhrCFMSLAMdIndex:
            int_value = phhrCFMMepTable->md_index ;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrCFMSLALMStatus:
            *var_len = sizeof(int);
            int_value = phhrCFMMepTable->lm_enable ;

            if (OAM_STATUS_UP == int_value)
            {
                int_value = 1;
            }
            else if (OAM_STATUS_DISABLE == int_value)
            {
                int_value = 2;
            }
            else
            {
                int_value = 3;
            }

            return (u_char *)(&int_value);

        case hhrCFMSLALMInterval:
            *var_len = sizeof(int);

            int_value = phhrCFMMepTable->lm_interval;

            return (u_char *)(&int_value);

        case hhrCFMSLADMStatus:
            *var_len = sizeof(int);
            int_value = phhrCFMMepTable->dm_enable ;

            if (OAM_STATUS_UP == int_value)
            {
                int_value = 1;
            }
            else if (OAM_STATUS_DISABLE == int_value)
            {
                int_value = 2;
            }
            else
            {
                int_value = 3;
            }

            return (u_char *)(&int_value);

        case hhrCFMSLADMInterval:
            *var_len = sizeof(int);
            int_value = phhrCFMMepTable->dm_interval ;
            return (u_char *)(&int_value);

        default :
            return (NULL);
    }

}

int hhrCFMSLAResultTable_get_data_from_ipc(struct cfm_session_key *sess_key)
{
    struct sla_measure *pcfm_sess = NULL;
	struct ipc_mesg_n *pRsvMsg = NULL;
    int data_num = 0;
    int ret = 0;

    pRsvMsg = cfm_com_get_sla_instance(sess_key, MODULE_ID_SNMPD);
	if (NULL == pRsvMsg)
	{
		return FALSE;
	}

	pcfm_sess = (struct sla_measure *)pRsvMsg->msg_data;
	if(NULL == pcfm_sess)
	{
		mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
		return FALSE;
	}

	data_num = pRsvMsg->msghdr.data_num;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);
	
    for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
    {
        cfm_node_add(hhrCFMSLAResultTable, pcfm_sess);
        pcfm_sess++;
    }

    time(&hhrCFMSLAResultTable_time_old);   //refresh time_old after refresh cache data
    
	mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
	
    return TRUE;
}

struct sla_measure *hhrCFMSLAResultTable_get_data_by_index(int exact,
                                                           u_int32_t *ma_index, u_int32_t *ma_index_next,
                                                           u_int32_t *mep_index, u_int32_t *mep_index_next)
{
    struct sla_measure *pcfm_sess = NULL;
    struct sla_measure *entry_temp;
    static time_t getNext_time_old;    //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    struct cfm_session_key index_sess ;
    u_int32_t *aaaa = 0 ;

    index_sess.ma_id = *ma_index ;
    index_sess.sess_id = *mep_index ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ma_index [%d] mep_index[%d]\n",
               __FILE__, __LINE__, __func__ , *ma_index , *mep_index);
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > hhrCFMSLAResultTable_GET_NEXT_INTERVAL))
    {
        hhrCFMSLAResultTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrCFMSLAResultTable_time_old) > hhrCFMSLAResultTable_REFRESH_TIME) && \
            (UNLOCK == hhrCFMSLAResultTable_lock))
    {
        if (NULL == hhrCFMSLAResultTable_list)
        {
            hhrCFMSLAResultTable_list = list_new();
            hhrCFMSLAResultTable_list->del = (void (*)(void *))hhrCFMSLAResultTable_node_free;
        }

        if (0 != hhrCFMSLAResultTable_list->count)
        {
            cfm_node_del_all(hhrCFMSLAResultTable);
        }
    }

    /* then lock */
    hhrCFMSLAResultTable_lock = LOCK;

    if (0 == hhrCFMSLAResultTable_list->count)
    {
        memset(&index_sess, 0, sizeof(struct cfm_session_key));
        index_sess.ma_id = *ma_index ;
        index_sess.sess_id = *mep_index ;
        ret = hhrCFMSLAResultTable_get_data_from_ipc(&index_sess);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    index_sess.ma_id = *ma_index ;
    index_sess.sess_id = *mep_index ;
    pcfm_sess = (struct sla_measure *)cfm_node_lookup(exact, hhrCFMSLAResultTable, &index_sess);

    if (NULL == pcfm_sess)  //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            if (NULL == hhrCFMSLAResultTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct sla_measure *)hhrCFMSLAResultTable_list->tail->data;
            memset(&index_sess,     0, sizeof(struct cfm_session_key));
            index_sess.ma_id = entry_temp->ma_index ;
            index_sess.sess_id = entry_temp->local_mep ;
            ret = hhrCFMSLAResultTable_get_data_from_ipc(&index_sess);

            if (FALSE == ret)
            {
                hhrCFMSLAResultTable_lock = UNLOCK; //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            index_sess.ma_id = *ma_index ;
            index_sess.sess_id = *mep_index ;
            pcfm_sess = (struct sla_measure *)cfm_node_lookup(exact, hhrCFMSLAResultTable, &index_sess);

            if (NULL != pcfm_sess)
            {
                *ma_index_next = pcfm_sess->ma_index;
                *mep_index_next = pcfm_sess->local_mep;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrCFMSLAResultTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pcfm_sess;
            }
        }
    }

    *ma_index_next = pcfm_sess->ma_index;
    *mep_index_next = pcfm_sess->local_mep;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrCFMSLAResultTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pcfm_sess;
}

u_char *hhrCFMSLAResultTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct sla_measure *phhrCFMMepTable = NULL;
    struct cfm_sess *pcfm_sess = NULL;

    struct cfm_session_key index;
    struct cfm_session_key index_next;
    memset(&index,      0, sizeof(struct cfm_session_key));
    memset(&index_next, 0, sizeof(struct cfm_session_key));
    int ma_id = 0 ;
    int ma_next = 0 ;

    int sess_id = 0 ;
    int sess_next = 0 ;


    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &ma_id, &sess_id, exact);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ma_id [%d] sess_id [%d] \n", __FILE__, __LINE__, __func__ , ma_id, sess_id);

    if (ret < 0)
    {
        return NULL;
    }

    pcfm_sess = hhrCFMMepTable_get_data_by_index(exact, &ma_id, &ma_next,
                                                 &sess_id, &sess_next);

    if (NULL == pcfm_sess)
    {
        return NULL;
    }

    /* get data by index and renew index_next */
    phhrCFMMepTable = hhrCFMSLAResultTable_get_data_by_index(exact, &ma_id, &ma_next,
                                                             &sess_id, &sess_next);

    if (NULL == phhrCFMMepTable)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ma_next [%d] sess_next [%d]\n", __FILE__, __LINE__, __func__ ,
                   ma_next , sess_next);
        ipran_snmp_intx2_index_set(vp, name, length, ma_next, sess_next);
    }

    switch (vp->magic)
    {
        case hhrCFMSLARMdIndex:
            *var_len = sizeof(int);
            int_value = pcfm_sess->md_index;
            return (u_char *)(&int_value);

        case hhrCFMSLAMaxFramelossRatioNear:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->lm_max;
            return (u_char *)(&uint_value);

        case hhrCFMSLAMinFramelossRatioNear:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->lm_min;
            return (u_char *)(&uint_value);


        case hhrCFMSLAMeanFrameLossRatioNear:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->lm_mean;
            return (u_char *)(&uint_value);

        case hhrCFMSLAMaxFrameLossRatioFar:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->lm_max_peer;
            return (u_char *)(&uint_value);

        case hhrCFMSLAMinFramelossRatioFar:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->lm_min_peer;
            return (u_char *)(&uint_value);

        case hhrCFMSLAMeanFrameLossRatioFar:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->lm_mean_peer;
            return (u_char *)(&uint_value);

        case hhrCFMSLAMaxDelay:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->dm_max;
            return (u_char *)(&uint_value);

        case hhrCFMSLAMaxDelayJitter:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->jitter_max;
            return (u_char *)(&uint_value);

        case hhrCFMSLAMinDelay:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->dm_min;
            return (u_char *)(&uint_value);

        case hhrCFMSLAMinDelayJitter:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->jitter_min;
            return (u_char *)(&uint_value);

        case hhrCFMSLAMeanDelay:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->dm_mean;
            return (u_char *)(&uint_value);

        case hhrCFMSLAMeanDelayJitter:
            *var_len = sizeof(uint32_t);
            uint_value = phhrCFMMepTable->jitter_mean;
            return (u_char *)(&uint_value);

        default :
            return (NULL);
    }

}

int hhrELPSTable_data_form_ipc(struct ipran_snmp_data_cache *cache,
                               struct hhrelps_snmp *index_input)
{
	struct ipc_mesg_n *pRsvMsg = NULL;
    struct hhrelps_snmp *pdata = NULL;
    int data_len = sizeof(struct hhrelps_snmp);
    int data_num = 0;
    int i;

    pRsvMsg = hhrethprotection_bulk(index_input->info.sess_id, MODULE_ID_SNMPD);
	if (NULL == pRsvMsg)
	{
		return FALSE;
	}

	pdata = (struct hhrelps_snmp *)pRsvMsg->msg_data;
	if (NULL == pdata)
	{
		mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
		return FALSE;
	}

	data_num = pRsvMsg->msghdr.data_num;
    for (i = 0; i < data_num; i++)
    {
        snmp_cache_add(cache, pdata, data_len);
        pdata++;
    }

	mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
	
    return TRUE; 
}

struct hhrelps_snmp *hhrELPSTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                              int exact,
                                              const struct hhrelps_snmp *index_input)
{
    struct hhrelps_snmp *pdata;
    struct listnode        *node;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {

        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->info.sess_id)
        {
            return cache->data_list->head->data;
        }


        if (pdata->info.sess_id == index_input->info.sess_id)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrELPSTable_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    struct hhrelps_snmp *pdata = NULL;
    struct hhrelps_snmp data_index ;
    uint32_t id_index = 0;
    int ret = 0;
    ret = ipran_snmp_int_index_get(vp, name, length, &id_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrELPSTable_cache)
    {
        hhrELPSTable_cache = snmp_cache_init(sizeof(struct hhrelps_snmp),
                                             hhrELPSTable_data_form_ipc,
                                             hhrELPSTable_node_lookup);

        if (NULL == hhrELPSTable_cache)
        {
            return (NULL);
        }
    }

    memset(&data_index, 0, sizeof(struct hhrelps_snmp));
    data_index.info.sess_id = id_index;
    pdata = snmp_cache_get_data_by_index(hhrELPSTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->info.sess_id);
    }

    switch (vp->magic)
    {
        case hhrElpsProtectPortMasterIfIndex:
            uint_value = pdata->info.master_port;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrElpsProtectPortMasterIfDescr:
            memset(str_value, 0, sizeof(str_value));
            ifm_get_name_by_ifindex(pdata->info.master_port, str_value);
            *var_len = strlen((char *) str_value);
            return (str_value);

        case hhrElpsProtectPortbackupIfIndex:
            uint_value = pdata->info.backup_port;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrElpsProtectPortbackupIfDescr:
            memset(str_value, 0, sizeof(str_value));
            ifm_get_name_by_ifindex(pdata->info.backup_port, str_value);
            *var_len = strlen((char *) str_value);
            return (str_value);

        case hhrElpsVlan:
            memset(long_str_value, 0, sizeof(uchar) * 512);
            memcpy(long_str_value, pdata->info.data_vlan_map, sizeof(pdata->info.data_vlan_map));
            *var_len = 512;
            return (long_str_value);

        case hhrElpsPrimaryVlan:
            uint_value = pdata->info.pvlan;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrElpsKeepAlive:
            uint_value = pdata->info.keepalive;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrElpsBackupMode:

            if (0 == pdata->info.failback)
            {
                uint_value = 2;
            }
            else if (1 == pdata->info.failback)
            {
                uint_value = 1;
            }
            else
            {
                uint_value = 0;
            }

            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;;

        case hhrElpsBackupWtr:
            uint_value = pdata->info.wtr;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrElpsPriority:
            uint_value = pdata->info.priority;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrElpsHoldOff:
            uint_value = pdata->info.holdoff;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrElpsEnable:
            int_value = (pdata->info.status ? 1 : 2);
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrElpsCurrentState:
            int_value = pdata->info.current_status;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

		case hhrElpsBindCfmSessionMaster:
			int_value = pdata->info.master_cfm_session;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

		case hhrElpsBindCfmSessionBackup:
			int_value = pdata->info.backup_cfm_session;
            *var_len = sizeof(int);
            return (u_char *)&int_value;
			
		case hhrElpsProtectCurrentWorkPort:
			memset(str_value, 0, sizeof(str_value));
            ifm_get_name_by_ifindex(pdata->info.active_port, str_value);
            *var_len = strlen((char *) str_value);
            return (str_value);
        default:
            return NULL;
    }

    return NULL;
}


int hhrCFMETHTestResultTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                               struct cfm_sess  *index)
{
	struct ipc_mesg_n *pRsvMsg = NULL;
    struct cfm_sess *seesions = NULL;
    int data_num = 0;
    int ret = 0;
    struct   cfm_session_key key ;
    key.sess_id = index->sess_id ;
	
    pRsvMsg = cfm_com_get_session_instance_new(&key, MODULE_ID_SNMPD);
	if (NULL == pRsvMsg)
	{
		return FALSE;
	}

	seesions = (struct cfm_sess *)pRsvMsg->msg_data;
    if (NULL == seesions)
    {
    	mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
	
        return FALSE;
    }

	data_num = pRsvMsg->msghdr.data_num;
    for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, seesions , sizeof(struct cfm_sess)) ;
        seesions++;
    }
	
	mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
	
    return TRUE;

}

struct cfm_sess *hhrCFMETHTestResultTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                      int exact,
                                                      const struct cfm_sess  *index_input)
{
    struct listnode  *node, *nnode;
    struct cfm_sess *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || index_input->sess_id == 0)
        {
            return cache->data_list->head->data;
        }

        if (data1_find->sess_id == index_input->sess_id)
        {
            if (1 == exact) //get
            {
                return data1_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return node->next->data;
                }
            }
        }
    }

    return (NULL);
}


u_char *hhrCFMETHTestResultTable_get(struct variable *vp,
                                     oid *name,
                                     size_t *length,
                                     int exact, size_t *var_len, WriteMethod **write_method)
{
    struct cfm_sess index ;
    u_int32_t index_next = 0;
    int ret = 0;
    int temp = 0 ;
    struct cfm_sess *pifm_info = NULL;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &temp, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrCFMETHTestResultTable_cache)
    {
        hhrCFMETHTestResultTable_cache = snmp_cache_init(sizeof(struct cfm_sess) ,
                                                         hhrCFMETHTestResultTable_get_data_from_ipc ,
                                                         hhrCFMETHTestResultTable_node_lookup);

        if (NULL == hhrCFMETHTestResultTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    index.sess_id = temp ;
    pifm_info = snmp_cache_get_data_by_index(hhrCFMETHTestResultTable_cache , exact, &index);

    if (NULL == pifm_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pifm_info->sess_id);
    }

    switch (vp->magic)
    {
        case hhrCFMETHTestTxNum:
            *var_len = sizeof(int);
            int_value =  pifm_info->tst_next_trans_id ;
            return (u_char *)(&int_value);

        case hhrCFMETHTestRxNum:
            *var_len = sizeof(int);
            int_value =  pifm_info->tst_rxnum;
            return (u_char *)(&int_value);

        case hhrCFMETHTestErrorNum:
            *var_len = sizeof(int);
            int_value =  pifm_info->tst_errnum;
            return (u_char *)(&int_value);

        default :
            return NULL ;
    }

}

u_char *hhrCFMExMepTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct cfm_sess index ;
    u_int32_t index_next = 0;
    int ret = 0;
    int temp = 0 ;
    struct cfm_sess *pifm_info = NULL;
    int i;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &temp, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrCFMETHTestResultTable_cache)
    {
        hhrCFMETHTestResultTable_cache = snmp_cache_init(sizeof(struct cfm_sess) ,
                                                         hhrCFMETHTestResultTable_get_data_from_ipc ,
                                                         hhrCFMETHTestResultTable_node_lookup);

        if (NULL == hhrCFMETHTestResultTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    index.sess_id = temp ;
    pifm_info = snmp_cache_get_data_by_index(hhrCFMETHTestResultTable_cache , exact, &index);

    if (NULL == pifm_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pifm_info->sess_id);
    }

    switch (vp->magic)
    {
        case hhrCFMExMaIndex:
            *var_len = sizeof(int);
            int_value =  pifm_info->ma_index ;
            return (u_char *)(&int_value);

        case hhrCFMExMepIndex:
            *var_len = sizeof(int);
            int_value =  pifm_info->local_mep ;
            return (u_char *)(&int_value);

        case hhrCFMExMepDir:
            *var_len = sizeof(int);
            int_value =  pifm_info->direct ;

            if (MEP_DIRECT_UP == int_value)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrCFMExMepCCMEnable:
            *var_len = sizeof(int);
            int_value = pifm_info->cc_enable ;

            if (1 == int_value)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrCFMExMepCCMInterval:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->cc_interval;

            if (3 == uint_value)
            {
                uint_value = 1;
            }
            else if (10 == uint_value)
            {
                uint_value = 2;
            }
            else if (30 == int_value)
            {
                int_value = 3;
            }
            else if (100 == uint_value)
            {
                uint_value = 4;
            }
            else if (1000 == uint_value)
            {
                uint_value = 5;
            }
            else if (10000 == uint_value)
            {
                uint_value = 6;
            }

            return (u_char *)(&uint_value);

        case hhrCFMExMepIfDescr:
            memset(str_value, 0, STRING_LEN);
            ifm_get_name_by_ifindex(pifm_info->ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrCFMExRMepIndex:
            *var_len = sizeof(int);
            int_value = pifm_info->remote_mep ;
            return (u_char *)(&int_value);

        case hhrCFMExRMepMac:
            *var_len = 6;

            if (pifm_info->dmac_valid)
            {
                memcpy(mac_value, pifm_info->dmac , 6);
            }
            else
            {
                memset(mac_value, 0 , 6);
            }

            return (mac_value);

        case hhrCFMExMepStatus:
            *var_len = sizeof(int);
            int_value = pifm_info->state ;

            if (OAM_STATUS_DISABLE == int_value)
            {
                int_value = 1;
            }
            else if (OAM_STATUS_ENABLE == int_value)
            {
                int_value = 2;
            }
            else if (OAM_STATUS_DOWN == int_value)
            {
                int_value = 3;
            }
            else if (OAM_STATUS_UP == int_value)
            {
                int_value = 4;
            }
            else if (OAM_STATUS_STOP == int_value)
            {
                int_value = 5;
            }
            else
            {
                int_value = 6;
            }

            return (u_char *)(&int_value);

        case hhrCFMExMepOverPw:
            *var_len = sizeof(int);
            int_value = pifm_info->over_pw ;

            if (int_value == 1)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrCFMExMepAisEnable:
            *var_len = sizeof(int);
            int_value = pifm_info->ais_enable ;

            if (int_value == 1)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrCFMExMepAisInterval:
            *var_len = sizeof(int);

            if (1 == pifm_info->ais_send_interval)
            {
                int_value = 1;
            }
            else if (60 == pifm_info->ais_send_interval)
            {
                int_value = 2;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)(&int_value);

        case hhrCFMExMepAisLevel:
            memset(str_value, 0, STRING_LEN);

            if (pifm_info->ais_enable)
            {
                for (i = 0; i < 8; i++)
                {
                    if (pifm_info->ais_send_level & (0x01 << i))
                    {
                        sprintf(str_value + strlen(str_value), "%d,", i);
                    }
                }

                if (strlen(str_value) > 1)
                {
                    str_value[strlen(str_value) - 1] = 0;
                }
            }
            else
            {
                sprintf(str_value, "%s", "--");
            }

            *var_len = strlen(str_value);
            return str_value;

        case hhrCFMExMepEthTestMode:
            *var_len = sizeof(int);
            int_value = pifm_info->tst_mode ;

            if (int_value == 0)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrCFMExMepLCKInterval:
            *var_len = sizeof(int);
            int_value = pifm_info->lck_send_interval ;

            if (int_value == 0)
            {
                int_value = 0;
            }
            else if (int_value == 1)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrCFMExMepLCKLevel:
            memset(str_value, 0, STRING_LEN);

            if (pifm_info->lck_send_level)
            {
                for (i = 0; i < 8; i++)
                {
                    if (pifm_info->ais_send_level & (0x01 << i))
                    {
                        sprintf(str_value + strlen(str_value), "%d,", i);
                    }
                }

                if (strlen(str_value) > 1)
                {
                    str_value[strlen(str_value) - 1] = 0;
                }
            }
            else
            {
                sprintf(str_value, "%s", "--");
            }

            *var_len = strlen(str_value);
            return str_value;

        default :
            return (NULL);
    }
}

int hhrCFMExSLAResultTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                             struct sla_measure  *index)
{
	struct ipc_mesg_n *pRsvMsg = NULL;
    struct sla_measure *seesions = NULL;
    int data_num = 0;
    int ret = 0;
    struct cfm_session_key key;

    key.sess_id = index->sess_id;
	
    pRsvMsg = cfm_com_get_sla_instance_new(&key, MODULE_ID_SNMPD);
    if (NULL == pRsvMsg)
	{
		return FALSE;
	}

	seesions = (struct sla_measure *)pRsvMsg->msg_data;
    if (NULL == seesions)
    {
    	mem_share_free(pRsvMsg, MODULE_ID_SNMPD);
        return FALSE;
    }
	
    for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, seesions , sizeof(struct sla_measure)) ;
        seesions++;
    }

	mem_share_free(pRsvMsg, MODULE_ID_SNMPD);

    return TRUE;
}

struct sla_measure *hhrCFMExSLAResultTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                       int exact,
                                                       const struct sla_measure  *index_input)
{
    struct listnode  *node, *nnode;
    struct sla_measure *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->sess_id)
        {
            return cache->data_list->head->data;
        }

        if (data1_find->sess_id == index_input->sess_id)
        {
            if (1 == exact) //get
            {
                return data1_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return node->next->data ;
                }
            }
        }
    }

    return (NULL);
}

u_char *hhrCFMExSLAConfigTable_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    struct cfm_sess index ;
    u_int32_t index_next = 0;
    int ret = 0;
    int temp = 0 ;
    struct cfm_sess *pifm_info = NULL;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &temp, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrCFMETHTestResultTable_cache)
    {
        hhrCFMETHTestResultTable_cache = snmp_cache_init(sizeof(struct cfm_sess) ,
                                                         hhrCFMETHTestResultTable_get_data_from_ipc ,
                                                         hhrCFMETHTestResultTable_node_lookup);

        if (NULL == hhrCFMETHTestResultTable_cache)
        {
            return (NULL);
        }
    }

    index.sess_id = temp ;
    pifm_info = snmp_cache_get_data_by_index(hhrCFMETHTestResultTable_cache , exact, &index);

    if (NULL == pifm_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pifm_info->sess_id);
    }

    switch (vp->magic)
    {
        case hhrCFMExSLAMaIndex:
            int_value = pifm_info->ma_index ;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrCFMExSLALMStatus:
            *var_len = sizeof(int);
            int_value = pifm_info->lm_enable ;

            if (OAM_STATUS_UP == int_value)
            {
                int_value = 1;
            }
            else if (OAM_STATUS_DISABLE == int_value)
            {
                int_value = 2;
            }
            else
            {
                int_value = 3;
            }

            return (u_char *)(&int_value);

        case hhrCFMExSLALMInterval:
            *var_len = sizeof(int);

            int_value = pifm_info->lm_interval;

            return (u_char *)(&int_value);

        case hhrCFMExSLADMStatus:
            *var_len = sizeof(int);
            int_value = pifm_info->dm_enable ;

            if (OAM_STATUS_UP == int_value)
            {
                int_value = 1;
            }
            else if (OAM_STATUS_DISABLE == int_value)
            {
                int_value = 2;
            }
            else
            {
                int_value = 3;
            }

            return (u_char *)(&int_value);

        case hhrCFMExSLADMInterval:
            *var_len = sizeof(int);
            int_value = pifm_info->dm_interval ;
            return (u_char *)(&int_value);

        default :
            return (NULL);

    }
}

u_char *hhrCFMExSLAResultTable_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    struct sla_measure index ;
    u_int32_t index_next = 0;
    int ret = 0;
    int temp = 0 ;
    struct sla_measure *pifm_info = NULL;
    struct cfm_sess *pcfm_sess = NULL;
    struct cfm_sess index_session ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &temp, exact);
    index.sess_id = temp ;
    index_session.sess_id = index.sess_id ;

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrCFMExSLAResultTable_cache)
    {
        hhrCFMExSLAResultTable_cache = snmp_cache_init(sizeof(struct sla_measure) ,
                                                       hhrCFMExSLAResultTable_get_data_from_ipc ,
                                                       hhrCFMExSLAResultTable_node_lookup);

        if (NULL == hhrCFMExSLAResultTable_cache)
        {
            return (NULL);
        }
    }

    pifm_info = snmp_cache_get_data_by_index(hhrCFMExSLAResultTable_cache, exact, &index);

    if (NULL == pifm_info)
    {
        return NULL;
    }

    if (NULL == hhrCFMETHTestResultTable_cache)
    {
        hhrCFMETHTestResultTable_cache = snmp_cache_init(sizeof(struct cfm_sess) ,
                                                         hhrCFMETHTestResultTable_get_data_from_ipc ,
                                                         hhrCFMETHTestResultTable_node_lookup);

        if (NULL == hhrCFMETHTestResultTable_cache)
        {
            return (NULL);
        }
    }

    pcfm_sess = snmp_cache_get_data_by_index(hhrCFMETHTestResultTable_cache, exact, &index_session);

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pifm_info->sess_id);
    }

    switch (vp->magic)
    {
        case hhrCFMExSLARMaIndex:
            *var_len = sizeof(int);
            int_value = pifm_info->ma_index;
            return (u_char *)(&int_value);

        case hhrCFMExSLARMepIndex:
            *var_len = sizeof(int);
            int_value = pifm_info->local_mep;
            return (u_char *)(&int_value);

        case hhrCFMExSLARMdIndex:
            *var_len = sizeof(int);
            int_value = pcfm_sess->md_index;
            return (u_char *)(&int_value);

        case hhrCFMExSLAMaxFramelossRatioNear:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->lm_max;
            return (u_char *)(&uint_value);

        case hhrCFMExSLAMinFramelossRatioNear:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->lm_min;
            return (u_char *)(&uint_value);

        case hhrCFMExSLAMeanFrameLossRatioNear:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->lm_mean;
            return (u_char *)(&uint_value);

        case hhrCFMExSLAMaxFrameLossRatioFar:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->lm_max_peer;
            return (u_char *)(&uint_value);

        case hhrCFMExSLAMinFramelossRatioFar:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->lm_min_peer;
            return (u_char *)(&uint_value);

        case hhrCFMExSLAMeanFrameLossRatioFar:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->lm_mean_peer;
            return (u_char *)(&uint_value);

        case hhrCFMExSLAMaxDelay:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->dm_max;
            return (u_char *)(&uint_value);

        case hhrCFMExSLAMaxDelayJitter:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->jitter_max;
            return (u_char *)(&uint_value);

        case hhrCFMExSLAMinDelay:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->dm_min;
            return (u_char *)(&uint_value);

        case hhrCFMExSLAMinDelayJitter:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->jitter_min;
            return (u_char *)(&uint_value);

        case hhrCFMExSLAMeanDelay:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->dm_mean;
            return (u_char *)(&uint_value);

        case hhrCFMExSLAMeanDelayJitter:
            *var_len = sizeof(uint32_t);
            uint_value = pifm_info->jitter_mean;
            return (u_char *)(&uint_value);

        default :
            return (NULL);
    }
}


