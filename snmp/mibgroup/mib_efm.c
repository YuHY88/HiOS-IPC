#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <net-snmp-config.h>
#include <types.h>
#include <net-snmp-includes.h>
#include <net-snmp-agent-includes.h>
#include <snmp_index_operater.h>

#include <lib/types.h>
#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include <l2/l2_if.h>
#include <l2/l2_snmp.h>
#include <l2/efm/efm_snmp.h>
#include <lib/linklist.h>
#include <lib/msg_ipc_n.h>
#include <ifm/ifm_message.h>
#include <lib/log.h>

#include "ipran_snmp_data_cache.h"

#include "snmp_config_table.h"

#include "mib_efm.h"

static struct ipran_snmp_data_cache *efm_local_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_peer_cache = NULL;
static struct  ipran_snmp_data_cache *efm_loopback_cache = NULL;
static struct  ipran_snmp_data_cache *efm_stats_cache = NULL;
static struct  ipran_snmp_data_cache *efm_event_config_cache = NULL;
static struct  ipran_snmp_data_cache *efm_eventlog_cache = NULL;
static struct  ipran_snmp_data_cache *efm_cache = NULL;
static struct  ipran_snmp_data_cache *efm_remloopback_cache = NULL;
static struct  ipran_snmp_data_cache *efm_event_cache = NULL;


#define SYS_STRING_LEN  256
static uchar str_value[STRING_LEN] = {'\0'};
static  uint32_t   int_value = 0;
static  uint8_t   bits_value = 0;
static uchar mac_value[6] = {0};

FindVarMethod dot3OamTable_get ;

struct variable2 dot3OamTable[] =
{
    {dot3OamAdminState,             ASN_INTEGER, RONLY, dot3OamTable_get, 2, {1, 1}},
    {dot3OamOperStatus,             ASN_INTEGER,    RONLY, dot3OamTable_get, 2, {1, 2}},
    {dot3OamMode,           ASN_INTEGER,    RONLY, dot3OamTable_get, 2, {1, 3}},
    {dot3OamMaxOamPduSize,              ASN_INTEGER,    RONLY, dot3OamTable_get, 2, {1, 4}},
    {dot3OamConfigRevision,      ASN_INTEGER,   RONLY, dot3OamTable_get, 2, {1, 5}},
    {dot3OamFunctionsSupported,  ASN_OCTET_STR,    RONLY, dot3OamTable_get, 2, {1, 6}}
} ;


oid  dot3OamTable_oid[]   = { EFM_HHREFM, 1};
int  dot3OamTable_oid_len = sizeof(dot3OamTable_oid) / sizeof(oid);

FindVarMethod dot3OamPeerTable_get ;

struct variable2 dot3OamPeerTable[] =
{
    {dot3OamPeerMacAddress,       ASN_OCTET_STR, RONLY, dot3OamPeerTable_get , 2, {1, 1}},
    {dot3OamPeerVendorOui,     ASN_OCTET_STR,   RONLY, dot3OamPeerTable_get , 2, {1, 2}},
    {dot3OamPeerVendorInfo,     ASN_INTEGER,  RONLY, dot3OamPeerTable_get , 2, {1, 3}},
    {dot3OamPeerMode,     ASN_INTEGER,  RONLY, dot3OamPeerTable_get , 2, {1, 4}},
    {dot3OamPeerMaxOamPduSize,   ASN_INTEGER,   RONLY, dot3OamPeerTable_get , 2, {1, 5}},
    {dot3OamPeerConfigRevision,      ASN_INTEGER,   RONLY, dot3OamPeerTable_get , 2, {1, 6}},
    {dot3OamPeerFunctionsSupported ,    ASN_OCTET_STR,    RONLY, dot3OamPeerTable_get , 2, {1, 7}}

};
oid  dot3OamPeerTable_oid[]   = { EFM_HHREFM, 2};
int  dot3OamPeerTable_oid_len = sizeof(dot3OamPeerTable_oid) / sizeof(oid);

FindVarMethod dot3OamLoopbackTable_get;
struct variable2 dot3OamLoopbackTable[] =
{
    { dot3OamLoopbackStatus,   ASN_INTEGER, RONLY, dot3OamLoopbackTable_get , 2, {1, 1}},

    {dot3OamLoopbackIgnoreRx,   ASN_INTEGER,  RONLY, dot3OamLoopbackTable_get , 2, {1, 2}}

};
oid  dot3OamLoopbackTable_oid[]   = { EFM_HHREFM, 3};
int  dot3OamLoopbackTable_oid_len = sizeof(dot3OamLoopbackTable_oid) / sizeof(oid);

FindVarMethod dot3OamStatsTable_get;
struct variable2 dot3OamStatsTable[] =
{
    {dot3OamInformationTx , ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 1}},
    {dot3OamInformationRx , ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 2}},
    {dot3OamUniqueEventNotificationTx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 3}},
    {dot3OamUniqueEventNotificationRx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 4}},
    {dot3OamDuplicateEventNotificationTx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 5}},
    {dot3OamDuplicateEventNotificationRx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 6}},
    {dot3OamLoopbackControlTx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 7}},
    {dot3OamLoopbackControlRx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 8}},
    {dot3OamVariableRequestTx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 9}},
    {dot3OamVariableRequestRx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 10}},
    {dot3OamVariableResponseTx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 11}},
    {dot3OamVariableResponseRx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 12}},
    {dot3OamOrgSpecificTx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 13}},
    {dot3OamOrgSpecificRx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 14}},
    {dot3OamUnsupportedCodesTx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 15}},
    {dot3OamUnsupportedCodesRx, ASN_UNSIGNED,   RONLY, dot3OamStatsTable_get, 2, {1, 16}},
    {dot3OamFramesLostDueToOam, ASN_INTEGER,   RONLY, dot3OamStatsTable_get, 2, {1, 17}},




};
oid  dot3OamStatsTable_oid[]   = { EFM_HHREFM, 4};
int  dot3OamStatsTable_oid_len = sizeof(dot3OamStatsTable_oid) / sizeof(oid);

FindVarMethod dot3OamEventConfigTable_get;
struct variable2 dot3OamEventConfigTable[] =
{
    {dot3OamErrSymPeriodWindowHi , ASN_UNSIGNED,   RONLY, dot3OamEventConfigTable_get, 2, {1, 1}},
    {dot3OamErrSymPeriodWindowLo, ASN_UNSIGNED,   RONLY, dot3OamEventConfigTable_get, 2, {1, 2}},
    {dot3OamErrSymPeriodThresholdHi , ASN_UNSIGNED,   RONLY, dot3OamEventConfigTable_get, 2, {1, 3}},
    {dot3OamErrSymPeriodThresholdLo, ASN_UNSIGNED,   RONLY, dot3OamEventConfigTable_get, 2, {1, 4}},
    { dot3OamErrSymPeriodEvNotifEnable, ASN_INTEGER,   RONLY, dot3OamEventConfigTable_get, 2, {1, 5}},
    { dot3OamErrFramePeriodWindow , ASN_UNSIGNED,   RONLY, dot3OamEventConfigTable_get, 2, {1, 6}},
    {dot3OamErrFramePeriodThreshold , ASN_UNSIGNED,   RONLY, dot3OamEventConfigTable_get, 2, {1, 7}},
    {dot3OamErrFramePeriodEvNotifEnable, ASN_INTEGER,   RONLY, dot3OamEventConfigTable_get, 2, {1, 8}},
    {dot3OamErrFrameWindow, ASN_UNSIGNED,   RONLY, dot3OamEventConfigTable_get, 2, {1, 9}},
    {dot3OamErrFrameThreshold, ASN_UNSIGNED,   RONLY, dot3OamEventConfigTable_get, 2, {1, 10}},
    {dot3OamErrFrameEvNotifEnable, ASN_INTEGER,   RONLY, dot3OamEventConfigTable_get, 2, {1, 11}},
    {dot3OamErrFrameSecsSummaryWindow , ASN_UNSIGNED,   RONLY, dot3OamEventConfigTable_get, 2, {1, 12}},
    {dot3OamErrFrameSecsSummaryThreshold, ASN_UNSIGNED,   RONLY, dot3OamEventConfigTable_get, 2, {1, 13}},
    {dot3OamErrFrameSecsEvNotifEnable , ASN_INTEGER,   RONLY, dot3OamEventConfigTable_get, 2, {1, 14}},
    {dot3OamDyingGaspEnable , ASN_INTEGER,   RONLY, dot3OamEventConfigTable_get, 2, {1, 15}},
    {dot3OamCriticalEventEnable, ASN_INTEGER,   RONLY, dot3OamEventConfigTable_get, 2, {1, 16}},

};
oid  dot3OamEventConfigTable_oid[]   = { EFM_HHREFM, 5};
int  dot3OamEventConfigTable_oid_len = sizeof(dot3OamEventConfigTable_oid) / sizeof(oid);



FindVarMethod dot3OamEventLogTable_get;

struct variable2 dot3OamEventLogTable[] =
{

    {dot3OamEventLogIndex , ASN_UNSIGNED,     RONLY, dot3OamEventLogTable_get, 2, {1, 1}},
    {dot3OamEventLogTimestamp , ASN_UNSIGNED,    RONLY, dot3OamEventLogTable_get, 2, {1, 2}},
    {dot3OamEventLogOui ,  ASN_OCTET_STR,    RONLY, dot3OamEventLogTable_get, 2, {1, 3}},
    { dot3OamEventLogType , ASN_INTEGER,     RONLY, dot3OamEventLogTable_get, 2, {1, 4}},
    {dot3OamEventLogLocation , ASN_INTEGER,  RONLY, dot3OamEventLogTable_get, 2, {1, 5}},
    {dot3OamEventLogWindowHi , ASN_UNSIGNED,     RONLY, dot3OamEventLogTable_get, 2, {1, 6}},
    { dot3OamEventLogWindowLo     , ASN_UNSIGNED,    RONLY, dot3OamEventLogTable_get, 2, {1, 7}},
    {dot3OamEventLogThresholdHi  , ASN_UNSIGNED,     RONLY, dot3OamEventLogTable_get, 2, {1, 8}},
    { dot3OamEventLogThresholdLo , ASN_UNSIGNED,    RONLY, dot3OamEventLogTable_get, 2, {1, 9}},
    {dot3OamEventLogValue , ASN_UNSIGNED,    RONLY, dot3OamEventLogTable_get, 2, {1, 10}},
    {dot3OamEventLogRunningTotal  , ASN_UNSIGNED,    RONLY, dot3OamEventLogTable_get, 2, {1, 11}},
    {dot3OamEventLogEventTotal, ASN_UNSIGNED,    RONLY, dot3OamEventLogTable_get, 2, {1, 12}}

};
oid  dot3OamEventLogTable_oid[]   = { EFM_HHREFM, 6};
int  dot3OamEventLogTable_oid_len = sizeof(dot3OamEventLogTable_oid) / sizeof(oid);


FindVarMethod dot3OamPktStaticClearAll_get;
struct variable1 dot3OamPktStaticClearAll[] =
{

    {dot3OamPacketStaticClearAll , ASN_UNSIGNED,     RONLY, dot3OamPktStaticClearAll_get, 1, {7}},

};
oid  dot3OamPktStaticClearAll_oid[]   = { EFM_HHREFM};


FindVarMethod hhrEFMTable_get;
struct variable2 hhrEFMTable[] =
{
    { hhrEFMTimeout,   ASN_INTEGER, RONLY, hhrEFMTable_get , 2, {1, 1}},
    {hhrEFMUnidirectionalLinkSupport ,   ASN_INTEGER,     RONLY, hhrEFMTable_get , 2, {1, 2}},
    { hhrEFMRate,   ASN_INTEGER, RONLY, hhrEFMTable_get , 2, {1, 3}}

};
oid  hhrEFMTable_oid[]   = { EFM_PRI_HHREFM, 1};
int  hhrEFMTable_oid_len = sizeof(hhrEFMTable_oid) / sizeof(oid);

FindVarMethod hhrEFMRemLoopbackTable_get;
struct variable2 hhrEFMRemLoopbackTable[] =
{
    { hhrEFMRemLoopbackAction,   ASN_INTEGER, RONLY, hhrEFMRemLoopbackTable_get , 2, {1, 1}},
    { hhrEFMRemLoopbackTimeout,   ASN_INTEGER, RONLY, hhrEFMRemLoopbackTable_get , 2, {1, 2}},
    { hhrEFMRemLoopbackSupport,   ASN_INTEGER,    RONLY, hhrEFMRemLoopbackTable_get , 2, {1, 3}},
    { hhrEFMRemLoopbackMacSwap,   ASN_INTEGER,  RONLY, hhrEFMRemLoopbackTable_get , 2, {1, 4}}

};
oid  hhrEFMRemLoopbackTable_oid[]   = { EFM_PRI_HHREFM, 2};
int  hhrEFMRemLoopbackTable_oid_len = sizeof(hhrEFMRemLoopbackTable_oid) / sizeof(oid);

FindVarMethod hhrEFMEventTable_get;
struct variable2 hhrEFMEventTable[] =
{
    { hhrEFMEventLinkMonitorStatus,   ASN_INTEGER, RONLY, hhrEFMEventTable_get , 2, {1, 1}},
    { hhrEFMEventLinkMonitorSupport,   ASN_INTEGER,   RONLY, hhrEFMEventTable_get , 2, {1, 2}},
    { hhrEFMEventLogNumber,   ASN_INTEGER,  RONLY, hhrEFMEventTable_get , 2, {1, 3}}

};
oid  hhrEFMEventTable_oid[]   = { EFM_PRI_HHREFM, 3};
int  hhrEFMEventTable_oid_len = sizeof(hhrEFMEventTable_oid) / sizeof(oid);

void  init_mib_efm(void)
{
    REGISTER_MIB("dot3OamTable", dot3OamTable, variable2,
                 dot3OamTable_oid) ;

    REGISTER_MIB("dot3OamPeerTable", dot3OamPeerTable, variable2,
                 dot3OamPeerTable_oid) ;
    REGISTER_MIB("dot3OamLoopbackTable", dot3OamLoopbackTable, variable2,
                 dot3OamLoopbackTable_oid) ;
    REGISTER_MIB("dot3OamStatsTable", dot3OamStatsTable, variable2,
                 dot3OamStatsTable_oid) ;
    REGISTER_MIB("dot3OamEventConfigTable", dot3OamEventConfigTable, variable2,
                 dot3OamEventConfigTable_oid) ;
    REGISTER_MIB("dot3OamEventLogTable", dot3OamEventLogTable, variable2,
                 dot3OamEventLogTable_oid) ;
    REGISTER_MIB("dot3OamPktStaticClearAll", dot3OamPktStaticClearAll, variable1,
                 dot3OamPktStaticClearAll_oid) ;
    REGISTER_MIB("hhrEFMTable", hhrEFMTable, variable2,
                 hhrEFMTable_oid) ;
    REGISTER_MIB("hhrEFMRemLoopbackTable", hhrEFMRemLoopbackTable, variable2,
                 hhrEFMRemLoopbackTable_oid) ;
    REGISTER_MIB("hhrEFMEventTable", hhrEFMEventTable, variable2,
                 hhrEFMEventTable_oid) ;

}

#if 0
void  *snmp_get_efm_info_bulk(uint32_t ifindex, int *pdata_num, int msg_subtype, int data_num)
{
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(NULL, 0, data_num, MODULE_ID_L2, module_id,
                                                         IPC_TYPE_SNMP, msg_subtype, IPC_OPCODE_GET, ifindex);

    if (pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (void *) pmesg->msg_data;
    }

    return NULL;
}
#endif

int snmp_efm_iftable_isvalid(uint32_t ifindex)
{
    if (IFM_TYPE_IS_ETHERNET(ifindex) && (!IFM_IS_SUBPORT(ifindex)))
    {
        return 0;
    }

    return -1;
}

int efm_if_local_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                        struct efm_local_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
    struct efm_local_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
    pmesg = snmp_get_efm_info_bulk(index->ifindex, &data_num, IPC_TYPE_SNMP_EFM_INFO, EFM_LOCAL_INFO_LEN);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

  
   	if(pmesg )
    {
    	pefm_info = (struct efm_local_info_snmp *)(pmesg->msg_data);
		
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pefm_info , sizeof(struct efm_local_info_snmp));
            pefm_info++;
        }

        mem_share_free(pmesg, MODULE_ID_SNMPD);
        return TRUE;
    }

	return FALSE;
}
int efm_if_peer_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                       struct efm_peer_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
    struct efm_peer_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
    pmesg = snmp_get_efm_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_INFO, EFM_PEER_INFO_LEN);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    if(pmesg)
    {
    	pefm_info = (struct efm_peer_info_snmp *)(pmesg->msg_data);
		
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pefm_info , sizeof(struct efm_peer_info_snmp));
            pefm_info++;
        }
		mem_share_free(pmesg, MODULE_ID_SNMPD);

        //time(&cache->getNext_time_old);
        return TRUE;
    }

	return FALSE;
	
}
int efm_if_loopback_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                           struct efm_loopback_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
    struct efm_loopback_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
	
    pmesg = snmp_get_efm_info_bulk(index->ifindex, &data_num, IPC_TYPE_SNMP_EFM_INFO, EFM_LOOPBACK_INFO_LEN);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    if(pmesg)
    {
    	pefm_info = (struct efm_loopback_info_snmp *)(pmesg->msg_data);
		
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pefm_info , sizeof(struct efm_loopback_info_snmp));
            pefm_info++;
        }

		mem_share_free(pmesg, MODULE_ID_SNMPD);

        return TRUE;
    }

	return FALSE;

}

int efm_if_stats_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                        struct efm_stats_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
    struct efm_stats_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
	
    pmesg = snmp_get_efm_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_INFO, EFM_STATS_INFO_LEN);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    if(pmesg)
    {
    	pefm_info = (struct efm_stats_info_snmp *)(pmesg->msg_data);
		
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pefm_info , sizeof(struct efm_stats_info_snmp));
            pefm_info++;
        }

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		
        return TRUE;
    }

	return FALSE;
}


int efm_if_event_config_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                               struct efm_event_config_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
    struct efm_event_config_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
	
    pmesg = snmp_get_efm_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_INFO, EFM_EVENT_CONFIG_INFO_LEN);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

   	if(pmesg)
    {
    	pefm_info = (struct efm_event_config_info_snmp *)(pmesg->msg_data);
		
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pefm_info , sizeof(struct efm_event_config_info_snmp));
            pefm_info++;
        }
		mem_share_free(pmesg, MODULE_ID_SNMPD);

        //time(&cache->getNext_time_old);
        return TRUE;
    }

	return FALSE;
}
int efm_if_eventlog_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache , struct efm_eventlog_info_snmp *index)
{

	struct ipc_mesg_n *pmesg = NULL;
    struct efm_eventlog_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;

    pmesg = snmp_get_efm_eventlog_info_bulk(&index->efm_eventlog_info.efm_EventLogIndex, sizeof(uint32_t), \
                                                index->ifindex, &data_num, IPC_TYPE_SNMP_EFM_INFO, EFM_EVENTLOG_INFO_LEN);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

   	if(pmesg)
    {
    	pefm_info = (struct efm_eventlog_info_snmp *)(pmesg->msg_data);
		
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pefm_info , sizeof(struct efm_eventlog_info_snmp));
            pefm_info++;
        }
		mem_share_free(pmesg, MODULE_ID_SNMPD);
     
        return TRUE;
    }

	return FALSE;
}
int efm_if_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                  struct efm_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
    struct efm_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
	
   pmesg = snmp_get_efm_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_INFO, EFM_INFO_LEN);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    if(pmesg)
    {
    	pefm_info = (struct efm_info_snmp *)(pmesg->msg_data);
		 	
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pefm_info , sizeof(struct efm_info_snmp));
            pefm_info++;
        }

		mem_share_free(pmesg, MODULE_ID_SNMPD);

        return TRUE;
    }

	return FALSE;
}

int efm_if_remloopback_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                              struct efm_remloopback_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
    struct efm_remloopback_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
	
    pmesg = snmp_get_efm_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_INFO, EFM_REMLOOPBACK_INFO_LEN);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    if(pmesg)
    {
    	pefm_info = (struct efm_remloopback_info_snmp *)(pmesg->msg_data);
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pefm_info , sizeof(struct efm_remloopback_info_snmp));
            pefm_info++;
        }
		mem_share_free(pmesg, MODULE_ID_SNMPD);
        
        return TRUE;
    }

	return FALSE;
}

int efm_if_event_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                        struct efm_event_info_snmp    *index)
{
	struct ipc_mesg_n *pmesg = NULL;
    struct efm_event_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
	
   	pmesg = snmp_get_efm_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_INFO, EFM_EVENT_INFO_LEN);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

  	if(pmesg)
    {
    	pefm_info = (struct efm_event_info_snmp *)(pmesg->msg_data);
		 
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pefm_info , sizeof(struct efm_event_info_snmp));
            pefm_info++;
        }
		mem_share_free(pmesg, MODULE_ID_SNMPD);
     
        return TRUE;
    }

	return FALSE;
}

struct efm_local_info_snmp *efm_if_local_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                          int exact,
                                                          const struct efm_local_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;
    struct efm_local_info_snmp *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifindex == index_input->ifindex)
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

struct efm_peer_info_snmp *efm_if_peer_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                        int exact,
                                                        const struct efm_peer_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;
    struct efm_peer_info_snmp *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifindex == index_input->ifindex)
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

struct efm_loopback_info_snmp *efm_if_loopback_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                                int exact,
                                                                const struct efm_loopback_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;
    struct efm_loopback_info_snmp *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifindex == index_input->ifindex)
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
struct efm_stats_info_snmp *efm_if_stats_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                          int exact,
                                                          const struct efm_stats_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;
    struct efm_stats_info_snmp *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifindex == index_input->ifindex)
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
struct efm_event_config_info_snmp *efm_if_event_config_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                                        int exact,
                                                                        const struct efm_event_config_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;
    struct efm_event_config_info_snmp *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifindex == index_input->ifindex)
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

struct efm_eventlog_info_snmp *efm_if_eventlog_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                                int exact,
                                                                const struct efm_eventlog_info_snmp  *index_input)
{
    struct listnode  *node, *nnode;

    struct efm_eventlog_info_snmp    *data1_find ;
    struct efm_eventlog_info_snmp        index1 ;


    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data ;
        }

        if (0 == index_input->ifindex && 0 == index_input->efm_eventlog_info.efm_EventLogIndex)
        {

            return cache->data_list->head->data ;
        }

        if ((data1_find->ifindex == index_input->ifindex) && (data1_find->efm_eventlog_info.efm_EventLogIndex == index_input->efm_eventlog_info.efm_EventLogIndex))

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

    return NULL;
}
struct efm_info_snmp *efm_if_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                              int exact,
                                              const struct efm_info_snmp    *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;
    struct efm_info_snmp *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifindex == index_input->ifindex)
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
struct efm_remloopback_info_snmp *efm_if_remloopback_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                                      int exact,
                                                                      const struct efm_remloopback_info_snmp    *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;
    struct efm_remloopback_info_snmp *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifindex == index_input->ifindex)
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
struct efm_event_info_snmp *efm_if_event_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                          int exact,
                                                          const struct efm_event_info_snmp    *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;
    struct efm_event_info_snmp *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifindex == index_input->ifindex)
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

u_char *
dot3OamTable_get(struct variable *vp,
                 oid *name,
                 size_t *length,
                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_local_info_snmp index ;
    int ret = 0;
    uint8_t temp = 0;
    struct efm_local_info_snmp *efm_info = NULL;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == efm_local_info_cache)
    {
        efm_local_info_cache = snmp_cache_init(sizeof(struct efm_local_info_snmp) ,
                                               efm_if_local_info_get_data_from_ipc ,
                                               efm_if_local_info_node_lookup);

        if (NULL == efm_local_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    efm_info = snmp_cache_get_data_by_index(efm_local_info_cache , exact, &index);

    if (NULL == efm_info)
    {
        return NULL;
    }

#if 0

    if (snmp_efm_iftable_isvalid(index.ifindex))
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': hhrEfmLocalInfoTable_isValid = -1\n", __FILE__, __LINE__, __func__);
        return NULL;
    }

#endif

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }


    switch (vp->magic)
    {
        case dot3OamAdminState :

            if (efm_info->efm_local_info.efm_local_enable)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamOperStatus :
            int_value = efm_info->efm_local_info.efm_local_discovery_status;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamMode :
            int_value = efm_info->efm_local_info.efm_local_mode;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamMaxOamPduSize :
            int_value = efm_info->efm_local_info.efm_local_pdu_size;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  dot3OamConfigRevision:
            int_value = efm_info->efm_local_info.efm_local_config_revision;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamFunctionsSupported :
            EIGHT_BITS_EXCHANGE(temp, efm_info->efm_local_info.efm_local_supports);
            bits_value = temp;
            *var_len = sizeof(uint8_t);
            return (u_char *)&bits_value;

        default :
            return (NULL);
    }
}

u_char *
dot3OamPeerTable_get(struct variable *vp,
                     oid *name,
                     size_t *length,
                     int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_peer_info_snmp index ;
    int ret = 0;
    int i = 0;
    uint8_t temp = 0;
    struct efm_peer_info_snmp *efm_info = NULL;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == efm_peer_cache)
    {
        efm_peer_cache = snmp_cache_init(sizeof(struct efm_peer_info_snmp) ,
                                         efm_if_peer_info_get_data_from_ipc ,
                                         efm_if_peer_info_node_lookup);

        if (NULL == efm_peer_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    efm_info = snmp_cache_get_data_by_index(efm_peer_cache , exact, &index);

    if (NULL == efm_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }


    switch (vp->magic)
    {
        case dot3OamPeerMacAddress:
            memcpy(mac_value, efm_info->efm_peer_info.efm_peer_mac, 6 * sizeof(uchar));
            *var_len = sizeof(mac_value) / sizeof(uchar);
            return (u_char *)mac_value;

        case dot3OamPeerVendorOui:
            memcpy(str_value, efm_info->efm_peer_info.efm_peer_oui, EFM_OUI_LENGTH);
            *var_len = EFM_OUI_LENGTH;
            return (u_char *)str_value;

        case dot3OamPeerVendorInfo :
            int_value = efm_info->efm_peer_info.efm_peer_vendor_info;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  dot3OamPeerMode:
            int_value = efm_info->efm_peer_info.efm_peer_mode;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamPeerMaxOamPduSize:
            int_value = efm_info->efm_peer_info.efm_peer_pdu_size;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamPeerConfigRevision:
            int_value = efm_info->efm_peer_info.efm_peer_config_revision;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamPeerFunctionsSupported:
            EIGHT_BITS_EXCHANGE(temp, efm_info->efm_peer_info.efm_peer_supports);
            bits_value = temp;
            *var_len = sizeof(uint8_t);
            return (u_char *)&bits_value;

        default :
            return (NULL);
    }
}


u_char *
dot3OamLoopbackTable_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_loopback_info_snmp index ;
    int ret = 0;
    int temp = 0 ;
    struct efm_loopback_info_snmp *efm_info = NULL;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == efm_loopback_cache)
    {
        efm_loopback_cache = snmp_cache_init(sizeof(struct efm_loopback_info_snmp) ,
                                             efm_if_loopback_info_get_data_from_ipc ,
                                             efm_if_loopback_info_node_lookup);

        if (NULL == efm_loopback_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    efm_info = snmp_cache_get_data_by_index(efm_loopback_cache , exact, &index);

    if (NULL == efm_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }


    switch (vp->magic)
    {

        case dot3OamLoopbackStatus:
            int_value = efm_info->efm_loopback_info.efm_loopback_state;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  dot3OamLoopbackIgnoreRx:
            int_value = efm_info->efm_loopback_info.efm_loopback_ignore_rx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return (NULL);
    }
}

u_char *dot3OamStatsTable_get(struct variable *vp,
                              oid *name,
                              size_t *length,
                              int exact, size_t *var_len, WriteMethod **write_method)
{


    struct efm_stats_info_snmp index ;
    int ret = 0;
    int temp = 0 ;
    struct efm_stats_info_snmp *efm_info = NULL;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == efm_stats_cache)
    {
        efm_stats_cache = snmp_cache_init(sizeof(struct efm_stats_info_snmp) ,
                                          efm_if_stats_info_get_data_from_ipc ,
                                          efm_if_stats_info_node_lookup);

        if (NULL == efm_stats_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    efm_info = snmp_cache_get_data_by_index(efm_stats_cache , exact, &index);

    if (NULL == efm_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }

    switch (vp->magic)
    {

        case dot3OamInformationTx:
            int_value = efm_info->efm_stats_info.efm_InformationTx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case    dot3OamInformationRx:
            int_value = efm_info->efm_stats_info.efm_InformationRx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamUniqueEventNotificationTx:
            int_value = efm_info->efm_stats_info.efm_UniqueEventNotificationTx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case    dot3OamUniqueEventNotificationRx:
            int_value = efm_info->efm_stats_info.efm_UniqueEventNotificationRx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamDuplicateEventNotificationTx:
            int_value = efm_info->efm_stats_info.efm_DuplicateEventNotificationTx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamDuplicateEventNotificationRx:
            int_value = efm_info->efm_stats_info.efm_DuplicateEventNotificationRx;;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamLoopbackControlTx:
            int_value = efm_info->efm_stats_info.efm_LoopbackControlTx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamLoopbackControlRx:
            int_value = efm_info->efm_stats_info.efm_LoopbackControlRx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamVariableRequestTx:
            int_value = efm_info->efm_stats_info.efm_VariableRequestTx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        case dot3OamVariableRequestRx:
            int_value = efm_info->efm_stats_info.efm_VariableRequestRx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;



        case dot3OamVariableResponseTx:
            int_value = efm_info->efm_stats_info.efm_VariableResponseTx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        case dot3OamVariableResponseRx:
            int_value = efm_info->efm_stats_info.efm_VariableResponseRx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamOrgSpecificTx:
            int_value = efm_info->efm_stats_info.efm_OrgSpecificTx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamOrgSpecificRx:
            int_value = efm_info->efm_stats_info.efm_OrgSpecificRx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamUnsupportedCodesTx:
            int_value = efm_info->efm_stats_info.efm_UnsupportedCodesTx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        case dot3OamUnsupportedCodesRx:
            int_value = efm_info->efm_stats_info.efm_UnsupportedCodesRx;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamFramesLostDueToOam:
            int_value = efm_info->efm_stats_info.efm_FramesLostDueToOam;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return (NULL);
    }
}

u_char *dot3OamEventConfigTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{


    struct efm_event_config_info_snmp index ;
    int ret = 0;
    int temp = 0 ;
    struct efm_event_config_info_snmp *efm_info = NULL;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == efm_event_config_cache)
    {
        efm_event_config_cache = snmp_cache_init(sizeof(struct efm_event_config_info_snmp) ,
                                                 efm_if_event_config_info_get_data_from_ipc ,
                                                 efm_if_event_config_info_node_lookup);

        if (NULL == efm_event_config_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    efm_info = snmp_cache_get_data_by_index(efm_event_config_cache , exact, &index);

    if (NULL == efm_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }

    switch (vp->magic)
    {

        case dot3OamErrSymPeriodWindowHi :
            int_value = efm_info->efm_event_config_info.efm_ErrSymPeriodWindowHi;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case    dot3OamErrSymPeriodWindowLo:
            int_value = efm_info->efm_event_config_info.efm_ErrSymPeriodWindowLo ;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamErrSymPeriodThresholdHi:
            int_value = efm_info->efm_event_config_info.efm_ErrSymPeriodThresholdHi ;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case    dot3OamErrSymPeriodThresholdLo:
            int_value = efm_info->efm_event_config_info.efm_ErrSymPeriodThresholdLo ;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamErrSymPeriodEvNotifEnable:
            int_value = efm_info->efm_event_config_info.efm_ErrSymPeriodEvNotifEnable;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamErrFramePeriodWindow :
            int_value = efm_info->efm_event_config_info.efm_ErrFramePeriodWindow ;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamErrFramePeriodThreshold:
            int_value = efm_info->efm_event_config_info.efm_ErrFramePeriodThreshold ;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamErrFramePeriodEvNotifEnable:
            int_value = efm_info->efm_event_config_info.efm_ErrFramePeriodEvNotifEnable ;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamErrFrameWindow:
            int_value = efm_info->efm_event_config_info.efm_ErrFrameWindow;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        case dot3OamErrFrameThreshold:
            int_value = efm_info->efm_event_config_info.efm_ErrFrameThreshold;
            *var_len = sizeof(uint32_t);
            return (u_char *)&int_value;



        case dot3OamErrFrameEvNotifEnable :
            int_value = efm_info->efm_event_config_info.efm_ErrFrameEvNotifEnable;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        case dot3OamErrFrameSecsSummaryWindow:
            int_value = efm_info->efm_event_config_info.efm_ErrFrameSecsSummaryWindow;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  dot3OamErrFrameSecsSummaryThreshold:
            int_value = efm_info->efm_event_config_info.efm_ErrFrameSecsSummaryThreshold;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamErrFrameSecsEvNotifEnable:
            int_value = efm_info->efm_event_config_info.efm_ErrFrameSecsEvNotifEnable;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case dot3OamDyingGaspEnable :
            int_value = efm_info->efm_event_config_info.efm_DyingGaspEnable ;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        case dot3OamCriticalEventEnable:
            int_value = efm_info->efm_event_config_info.efm_CriticalEventEnable ;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return (NULL);
    }


}


uchar *dot3OamEventLogTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{

    struct efm_eventlog_info_snmp *efm_info = NULL;
    struct efm_eventlog_info_snmp index;
    int ret = 0;
    int flag = 0, val = 0;
    int p_index = 0;
    int log_index = 0;
    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &p_index, &log_index , exact);

    index.ifindex = p_index;
    index.efm_eventlog_info.efm_EventLogIndex = log_index;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s:p_index = %d --log_nidex =  %d\n", __FILE__, __LINE__, __func__, p_index, log_index);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == efm_eventlog_cache)
    {
        efm_eventlog_cache = snmp_cache_init(sizeof(struct efm_eventlog_info_snmp),
                                             efm_if_eventlog_info_get_data_from_ipc,
                                             efm_if_eventlog_info_node_lookup);

        if (NULL ==  efm_eventlog_cache)
        {
            return (NULL);
        }
    }

    efm_info = snmp_cache_get_data_by_index(efm_eventlog_cache, exact, &index);

    if (NULL == efm_info)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, efm_info->ifindex, efm_info->efm_eventlog_info.efm_EventLogIndex);
    }



    switch (vp->magic)

    {


        case dot3OamEventLogIndex:
            int_value = efm_info->efm_eventlog_info.efm_EventLogIndex;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case dot3OamEventLogTimestamp:
            int_value = efm_info->efm_eventlog_info.efm_EventLogTimestamp;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case dot3OamEventLogOui :
            memcpy(str_value, efm_info->efm_eventlog_info.efm_EventLogOui, EFM_OUI_LENGTH);
            *var_len = EFM_OUI_LENGTH;
            return (u_char *)str_value;

        case  dot3OamEventLogType :
            int_value = efm_info->efm_eventlog_info.efm_EventLogType;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case dot3OamEventLogLocation:
            int_value = efm_info->efm_eventlog_info.efm_EventLogLocation;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case      dot3OamEventLogWindowHi:
            int_value = efm_info->efm_eventlog_info.efm_EventLogWindowHi;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case dot3OamEventLogWindowLo:
            int_value = efm_info->efm_eventlog_info.efm_EventLogWindowLo;
            *var_len = sizeof(int);
            return (uchar *)(&int_value);

        case dot3OamEventLogThresholdHi:
            int_value = efm_info->efm_eventlog_info.efm_EventLogThresholdHi;
            *var_len = sizeof(int);
            return (uchar *)(&int_value);

        case dot3OamEventLogThresholdLo:
            int_value = efm_info->efm_eventlog_info.efm_EventLogThresholdLo;
            *var_len = sizeof(int);
            return (uchar *)(&int_value);

        case dot3OamEventLogValue:
            int_value = efm_info->efm_eventlog_info.efm_EventLogValue;
            *var_len = sizeof(int);
            return (uchar *)(&int_value);

        case dot3OamEventLogRunningTotal:
            int_value = efm_info->efm_eventlog_info.efm_EventLogRunningTotal;
            *var_len = sizeof(int);
            return (uchar *)(&int_value);

        case dot3OamEventLogEventTotal:
            int_value = efm_info->efm_eventlog_info.efm_EventLogEventTotal;
            *var_len = sizeof(int);
            return (uchar *)(&int_value);



        default:
            return NULL;
    }


}

u_char *
dot3OamPktStaticClearAll_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case dot3OamPacketStaticClearAll:
            bits_value = 0;
            *var_len = sizeof(bits_value);
            return (u_char *)&bits_value;

        default :
            return (NULL);
    }
}
u_char *hhrEFMTable_get(struct variable *vp,
                        oid *name,
                        size_t *length,
                        int exact, size_t *var_len, WriteMethod **write_method)
{


    struct efm_info_snmp index ;
    int ret = 0;
    int temp = 0 ;
    struct efm_info_snmp *efm_info_base = NULL;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == efm_cache)
    {
        efm_cache = snmp_cache_init(sizeof(struct efm_info_snmp) ,
                                    efm_if_info_get_data_from_ipc ,
                                    efm_if_info_node_lookup);

        if (NULL == efm_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    efm_info_base = snmp_cache_get_data_by_index(efm_cache , exact, &index);

    if (NULL == efm_info_base)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info_base->ifindex);
    }

    switch (vp->magic)
    {

        case hhrEFMTimeout:
            int_value = efm_info_base->efm_info.efm_timeout;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case    hhrEFMUnidirectionalLinkSupport:
            int_value = efm_info_base->efm_info.efm_unidirectionalLinkSupport;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrEFMRate:
            int_value = efm_info_base->efm_info.efm_rate;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        default :
            return (NULL);
    }


}


u_char *hhrEFMRemLoopbackTable_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{


    struct efm_remloopback_info_snmp index ;
    int ret = 0;
    int temp = 0 ;
    struct efm_remloopback_info_snmp *efm_remloopback_info = NULL;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == efm_remloopback_cache)
    {
        efm_remloopback_cache = snmp_cache_init(sizeof(struct efm_remloopback_info_snmp) ,
                                                efm_if_remloopback_info_get_data_from_ipc ,
                                                efm_if_remloopback_info_node_lookup);

        if (NULL == efm_remloopback_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    efm_remloopback_info = snmp_cache_get_data_by_index(efm_remloopback_cache , exact, &index);

    if (NULL == efm_remloopback_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_remloopback_info->ifindex);
    }

    switch (vp->magic)
    {

        case hhrEFMRemLoopbackAction:
            int_value = efm_remloopback_info->efm_remloopback_info.efm_remloopback_action;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrEFMRemLoopbackTimeout :
            int_value = efm_remloopback_info->efm_remloopback_info.efm_remloopback_timeout;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrEFMRemLoopbackSupport:
            int_value = efm_remloopback_info->efm_remloopback_info.efm_remloopback_support;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrEFMRemLoopbackMacSwap:
            int_value = efm_remloopback_info->efm_remloopback_info.efm_remloopback_macswap;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        default :
            return (NULL);
    }


}


u_char *hhrEFMEventTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{


    struct efm_event_info_snmp index ;
    int ret = 0;
    int temp = 0 ;
    struct efm_event_info_snmp *efm_event_info = NULL;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == efm_event_cache)
    {
        efm_event_cache = snmp_cache_init(sizeof(struct efm_event_info_snmp) ,
                                          efm_if_event_info_get_data_from_ipc ,
                                          efm_if_event_info_node_lookup);

        if (NULL == efm_event_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    efm_event_info = snmp_cache_get_data_by_index(efm_event_cache , exact, &index);

    if (NULL == efm_event_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_event_info->ifindex);
    }

    switch (vp->magic)
    {

        case hhrEFMEventLinkMonitorStatus :
            int_value = efm_event_info->efm_event_info.efm_event_linkmonitor_status;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrEFMEventLinkMonitorSupport:
            int_value = efm_event_info->efm_event_info.efm_event_linkmonitor_support;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrEFMEventLogNumber:
            int_value = efm_event_info->efm_event_info.efm_eventlog_number;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        default :
            return (NULL);
    }


}
/*deal with one ifindex */
struct ipc_mesg_n   *snmp_get_efm_info_bulk(uint32_t ifindex,int *pdata_num, int msg_subtype, int data_num)
{
	
	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, data_num, MODULE_ID_L2, MODULE_ID_SNMPD,
                                                         IPC_TYPE_SNMP, msg_subtype, IPC_OPCODE_GET, ifindex,5000);
    if (pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return pmesg;
    }

    return NULL;
}
/*deal waith double ifindex*/
struct ipc_mesg_n   *snmp_get_efm_eventlog_info_bulk(void *pdata, int data_len, uint32_t ifindex, int *pdata_num, int msg_subtype, int data_num)
{
	
	 struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pdata, data_len, data_num, MODULE_ID_L2, MODULE_ID_SNMPD,
                                                         IPC_TYPE_SNMP, msg_subtype, IPC_OPCODE_GET, ifindex,5000);
	
	if (pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return pmesg;
    }

    return NULL;
}





