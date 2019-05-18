#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <net-snmp-config.h>
#include <types.h>
#include <net-snmp-includes.h>
#include <net-snmp-agent-includes.h>
#include <snmp_index_operater.h>

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
#include <lib/linklist.h>
#include <lib/rmon_common.h>
#include <lib/msg_ipc_n.h>
#include <lib/log.h>

#include "mib_rmon.h"
//#include "lib/snmp_common.h"
#include "snmp_config_table.h"

#include "ipran_snmp_data_cache.h"

/*local temp variable*/
static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value = 0;
static uchar mac_value[6] = {0};
static uint32_t           uint_value = 0;
static uint32_t           ip_value = 0;
static struct counter64 uint64_value;


struct ipran_snmp_data_cache *etherStatsTable_cache = NULL;
FindVarMethod etherStatsTable_get ;
oid  etherStatsTable_oid[]   = { RMONMIBOID , 1 , 1};
int  etherStatsTable_oid_len = sizeof(etherStatsTable_oid) / sizeof(oid);
struct variable2 etherStatsTable_variables[] =
{
    {ETHERSTATSINDEX,               ASN_UNSIGNED, RONLY, etherStatsTable_get, 2, {1, 1}},
    {ETHERSTATSDATASOURCE,          OBJECT_ID,  RONLY, etherStatsTable_get, 2, {1, 2}},
    {ETHERSTATSDROPEVENTS,              COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 3}},
    {ETHERSTATSOCTETS,              COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 4}},
    {ETHERSTATSPKTS,      COUNTER64,    RONLY, etherStatsTable_get, 2, {1, 5}},
    {ETHERSTATSBROADCASTPKTS,           COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 6}},
    {ETHERSTATSMULTICASTPKTS,           COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 7}},
    {ETHERSTATSCRCALIGNERRORS,      COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 8}} ,
    {ETHERSTATSUNDERSIZEPKTS,               COUNTER64, RONLY, etherStatsTable_get, 2, {1, 9}},
    {ETHERSTATSOVERSIZEPKTS,            COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 10}},
    {ETHERSTATSFRAGMENTS,           COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 11}},
    {ETHERSTATSJABBERS,             COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 12}},
    {ETHERSTATSCOLLISIONS,      COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 13}},
    {ETHERSTATSPKTS64OCTETS,            COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 14}},
    {ETHERSTATSPKTS65TO127OCTETS,           COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 15}},
    {ETHERSTATSPKTS128TO255OCTETS,      COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 16}} ,
    {ETHERSTATSPKTS256TO511OCTETS,          COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 17}},
    {ETHERSTATSPKTS512TO1023OCTETS,             COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 18}},
    {ETHERSTATSPKTS1024TO1518OCTETS,        COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 19}} ,
    {ETHERSTATSOWNER,       OWNERSTRING,    RONLY, etherStatsTable_get, 2, {1, 20}} ,
    {ETHERSTATSSTATUS,          ENTRYSTATUS,    RONLY, etherStatsTable_get, 2, {1, 21}},
    {ETHERSTATSUNICASTPKTS,         COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 22}} ,
    {ETHERSTATSBADPKTS,         COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 23}},
    {ETHERSTATSINDISCARDPKTS,       COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 24}} ,
    {ETHERSTATSSENTOCTETS,          COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 25}},
    {ETHERSTATSSENTPKTS,            COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 26}},
    {ETHERSTATSSENTUNICASTPKTS,     COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 27}} ,
    {ETHERSTATSSENTBROADCASTPKTS,       COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 28}} ,
    {ETHERSTATSSENTMULTICASTPKTS,           COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 29}},
    {ETHERSTATSSENTBADPKTS,         COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 30}} ,
    {ETHERSTATSSENTDISCARDPKTS,         COUNTER64,  RONLY, etherStatsTable_get, 2, {1, 31}}

} ;

struct ipran_snmp_data_cache *historyControlTable_cache = NULL;
FindVarMethod historyControlTable_get ;
oid  historyControlTable_oid[]   = { RMONMIBOID , 2, 1};
int  historyControlTable_oid_len = sizeof(historyControlTable_oid) / sizeof(oid);
struct variable2 historyControlTable_variables[] =
{
    {HISTORYCONTROLINDEX,               INTEGER32, RONLY, historyControlTable_get, 2, {1, 1}},
    {HISTORYCONTROLDATASOURCE,          OBJECT_ID,  RONLY, historyControlTable_get, 2, {1, 2}},
    {HISTORYCONTROLBUCKETSREQUESTED,            INTEGER32,  RONLY, historyControlTable_get, 2, {1, 3}},
    {HISTORYCONTROLBUCKETSGRANTED,              INTEGER32,  RONLY, historyControlTable_get, 2, {1, 4}},
    {HISTORYCONTROLINTERVAL,      INTEGER32,    RONLY, historyControlTable_get, 2, {1, 5}},
    {HISTORYCONTROLOWNER,           OWNERSTRING,    RONLY, historyControlTable_get, 2, {1, 6}},
    {HISTORYCONTROLSTATUS,          ENTRYSTATUS,    RONLY, historyControlTable_get, 2, {1, 7}}
} ;

struct ipran_snmp_data_cache *etherHistoryTable_cache = NULL;
FindVarMethod etherHistoryTable_get ;
oid  etherHistoryTable_oid[]   = { RMONMIBOID , 2, 2};
int  etherHistoryTable_oid_len = sizeof(etherHistoryTable_oid) / sizeof(oid);
struct variable2 etherHistoryTable_variables[] =
{
    {ETHERHISTORYINDEX,             INTEGER32, RONLY, etherHistoryTable_get, 2, {1, 1}},
    {ETHERHISTORYSAMPLEINDEX,           INTEGER32,  RONLY, etherHistoryTable_get, 2, {1, 2}},
    {ETHERHISTORYINTERVALSTART,             TIMETICKS,  RONLY, etherHistoryTable_get, 2, {1, 3}},
    {ETHERHISTORYDROPEVENTS,            COUNTER64,  RONLY, etherHistoryTable_get, 2, {1, 4}},
    {ETHERHISTORYOCTETS,      COUNTER64,    RONLY, etherHistoryTable_get, 2, {1, 5}},
    {ETHERHISTORYPKTS,          COUNTER64,  RONLY, etherHistoryTable_get, 2, {1, 6}},
    {ETHERHISTORYBROADCASTPKTS,             COUNTER64,  RONLY, etherHistoryTable_get, 2, {1, 7}} ,
    {ETHERHISTORYMULTICASTPKTS,             COUNTER64, RONLY, etherHistoryTable_get, 2, {1, 8}},
    {ETHERHISTORYCRCALIGNERRORS,            COUNTER64,  RONLY, etherHistoryTable_get, 2, {1, 9}},
    {ETHERHISTORYUNDERSIZEPKTS,             COUNTER64,  RONLY, etherHistoryTable_get, 2, {1, 10}},
    {ETHERHISTORYOVERSIZEPKTS,              COUNTER64,  RONLY, etherHistoryTable_get, 2, {1, 11}},
    {ETHERHISTORYFRAGMENTS,      COUNTER64,     RONLY, etherHistoryTable_get, 2, {1, 12}},
    {ETHERHISTORYJABBERS,           COUNTER64,  RONLY, etherHistoryTable_get, 2, {1, 13}} ,
    {ETHERHISTORYCOLLISIONS,     COUNTER64, RONLY, etherHistoryTable_get, 2, {1, 14}},
    {ETHERHISTORYUTILIZATION,           INTEGER32,  RONLY, etherHistoryTable_get, 2, {1, 15}}
};

struct ipran_snmp_data_cache *alarmTable_cache = NULL;
FindVarMethod alarmTable_get ;
oid  alarmTable_oid[]   = { RMONMIBOID , 3 , 1};
int  alarmTable_oid_len = sizeof(alarmTable_oid) / sizeof(oid);
struct variable2 alarmTable_variables[] =
{
    {ALARMINDEX,            INTEGER32, RONLY, alarmTable_get, 2, {1, 1}},
    {ALARMINTERVAL,             INTEGER32,  RONLY, alarmTable_get, 2, {1, 2}},
    {ALARMVARIABLE,             OBJECT_ID,  RONLY, alarmTable_get, 2, {1, 3}},
    {ALARMSAMPLETYPE,           INTEGER,    RONLY, alarmTable_get, 2, {1, 4}},
    {ALARMVALUE,      INTEGER32,    RONLY, alarmTable_get, 2, {1, 5}},
    {ALARMSTARTUPALARM,             INTEGER,    RONLY, alarmTable_get, 2, {1, 6}},
    {ALARMRISINGTHRESHOLD,          INTEGER32,  RONLY, alarmTable_get, 2, {1, 7}} ,
    {ALARMFALLINGTHRESHOLD,             INTEGER32, RONLY, alarmTable_get, 2, {1, 8}},
    {ALARMRISINGEVENTINDEX,             INTEGER32,  RONLY, alarmTable_get, 2, {1, 9}},
    {ALARMFALLINGEVENTINDEX,            INTEGER32,  RONLY, alarmTable_get, 2, {1, 10}},
    {ALARMOWNER,            OWNERSTRING,    RONLY, alarmTable_get, 2, {1, 11}},
    {ALARMSTATUS,      ENTRYSTATUS,     RONLY, alarmTable_get, 2, {1, 12}}
};

struct ipran_snmp_data_cache *eventTable_cache = NULL;
FindVarMethod eventTable_get ;
oid  eventTable_oid[]   = { RMONMIBOID , 9, 1};
int  eventTable_oid_len = sizeof(eventTable_oid) / sizeof(oid);
struct variable2 eventTable_variables[] =
{
    {EVENTINDEX,            INTEGER32, RONLY, eventTable_get, 2, {1, 1}},
    {EVENTDESCRIPTION,          DISPLAYSTRING,  RONLY, eventTable_get, 2, {1, 2}},
    {EVENTTYPE,             INTEGER,    RONLY, eventTable_get, 2, {1, 3}},
    {EVENTCOMMUNITY,            OCTET_STRING,   RONLY, eventTable_get, 2, {1, 4}},
    {EVENTLASTTIMESENT,      TIMETICKS,     RONLY, eventTable_get, 2, {1, 5}},
    {EVENTOWNER,            OWNERSTRING,    RONLY, eventTable_get, 2, {1, 6}},
    {EVENTSTATUS,           ENTRYSTATUS,    RONLY, eventTable_get, 2, {1, 7}}
};


void  init_mib_rmon(void)
{
    REGISTER_MIB("etherStatsTable", etherStatsTable_variables, variable2,
                 etherStatsTable_oid) ;
    REGISTER_MIB("historyControlTable", historyControlTable_variables, variable2,
                 historyControlTable_oid) ;
    REGISTER_MIB("etherHistoryTable", etherHistoryTable_variables, variable2,
                 etherHistoryTable_oid) ;
    REGISTER_MIB("alarmTable", alarmTable_variables, variable2,
                 alarmTable_oid) ;
    REGISTER_MIB("eventTable", eventTable_variables, variable2,
                 eventTable_oid) ;
}

int etherStatsTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct rmon_etherStatsGroup *index)
{
    struct rmon_etherStatsGroup *prmon = NULL;
    struct ipc_mesg_n *pmsg = NULL;
    int ret = 0;
    int data_num = 0;
    pmsg = etherStatsTable_bulk(index->etherStatsIndex, MODULE_ID_SNMPD, &data_num);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d index->etherStatsIndex [%d]\n", __FILE__, __LINE__, __func__, data_num , index->etherStatsIndex);

    if( NULL == pmsg )
    {
        return FALSE ;
    }


    if (0 == data_num)
    {
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }

    else
    {
        prmon =  (struct rmon_etherStatsGroup *)pmsg->msg_data;
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, prmon , sizeof(struct rmon_etherStatsGroup));
            prmon++;
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);

        return TRUE;
    }

}


struct rmon_etherStatsGroup *etherStatsTable_data_lookup(struct ipran_snmp_data_cache *cache ,
                                                         int exact,  const struct rmon_etherStatsGroup  *index_input)
{

    struct listnode  *node, *nnode;

    struct rmon_etherStatsGroup    *data1_find ;
    struct rmon_etherStatsGroup        index1 ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] \n", __FUNCTION__, __LINE__);

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] \n", __FUNCTION__, __LINE__);
            return cache->data_list->head->data ;
        }

        if (0 == index_input->etherStatsIndex)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] \n", __FUNCTION__, __LINE__);
            return cache->data_list->head->data ;
        }

        if (index_input->etherStatsIndex == data1_find->etherStatsIndex)

        {

            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] \n", __FUNCTION__, __LINE__);

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


u_char *etherStatsTable_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    struct rmon_etherStatsGroup *pstat = NULL;
    struct rmon_etherStatsGroup index  ;
    int ret = 0;
    int index_int = 0 ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index_int , exact);

    index.etherStatsIndex = index_int ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index.etherStatsIndex);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == etherStatsTable_cache)
    {
        etherStatsTable_cache = snmp_cache_init(sizeof(struct rmon_etherStatsGroup) ,
                                                etherStatsTable_data_form_ipc ,
                                                etherStatsTable_data_lookup);

        if (NULL == etherStatsTable_cache)
        {
            return (NULL);
        }
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    pstat = snmp_cache_get_data_by_index(etherStatsTable_cache , exact, &index);

    if (NULL == pstat)
    {
        return NULL;
    }

    if (!exact)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsIndex [%d] ifindex [%d]\n", __FILE__, __LINE__, __func__ ,
                   pstat->etherStatsIndex ,
                   pstat->etherStatsDataSource[RMON_IFINDEX_LOC]);
        ipran_snmp_int_index_set(vp, name, length, pstat->etherStatsIndex);
    }

    switch (vp->magic)
    {
        case ETHERSTATSINDEX:
            *var_len = sizeof(int);
            int_value = pstat->etherStatsIndex ;
            return (u_char *)(&int_value);

        case ETHERSTATSDATASOURCE:
            //*var_len= sizeof(pstat->etherStatsDataSource) /sizeof(oid) ;
            *var_len = 11 * sizeof(oid) ;
            return (u_char *)pstat->etherStatsDataSource;

        case ETHERSTATSDROPEVENTS:
            uint64_value.high = (pstat->etherStatsDropEvents.l[0]);
            uint64_value.low  = (pstat->etherStatsDropEvents.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSOCTETS:
            uint64_value.high = (pstat->etherStatsOctets.l[0]);
            uint64_value.low  = (pstat->etherStatsOctets.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSPKTS:
            uint64_value.high = (pstat->etherStatsPkts.l[0]);
            uint64_value.low  = (pstat->etherStatsPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSBROADCASTPKTS:
            uint64_value.high = (pstat->etherStatsBroadcastPkts.l[0]);
            uint64_value.low  = (pstat->etherStatsBroadcastPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSMULTICASTPKTS:
            uint64_value.high = (pstat->etherStatsMulticastPkts.l[0]);
            uint64_value.low  = (pstat->etherStatsMulticastPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;


        case ETHERSTATSCRCALIGNERRORS:
            uint64_value.high = (pstat->etherStatsCRCAlignErrors.l[0]);
            uint64_value.low  = (pstat->etherStatsCRCAlignErrors.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSUNDERSIZEPKTS:
            uint64_value.high = (pstat->etherStatsCRCAlignErrors.l[0]);
            uint64_value.low  = (pstat->etherStatsCRCAlignErrors.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSOVERSIZEPKTS:
            uint64_value.high = (pstat->etherStatsUndersizePkts.l[0]);
            uint64_value.low  = (pstat->etherStatsUndersizePkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSFRAGMENTS:
            uint64_value.high = (pstat->etherStatsFragments.l[0]);
            uint64_value.low  = (pstat->etherStatsFragments.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSJABBERS:
            uint64_value.high = (pstat->etherStatsJabbers.l[0]);
            uint64_value.low  = (pstat->etherStatsJabbers.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSCOLLISIONS:
            uint64_value.high = (pstat->etherStatsCollisions.l[0]);
            uint64_value.low  = (pstat->etherStatsCollisions.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSPKTS64OCTETS:
            uint64_value.high = (pstat->etherStatsPkts64Octets.l[0]);
            uint64_value.low  = (pstat->etherStatsPkts64Octets.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSPKTS65TO127OCTETS:
            uint64_value.high = (pstat->etherStatsPkts65to127Octets.l[0]);
            uint64_value.low  = (pstat->etherStatsPkts65to127Octets.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSPKTS128TO255OCTETS:
            uint64_value.high = (pstat->etherStatsPkts128to255Octets.l[0]);
            uint64_value.low  = (pstat->etherStatsPkts128to255Octets.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSPKTS256TO511OCTETS:
            uint64_value.high = (pstat->etherStatsPkts256to511Octets.l[0]);
            uint64_value.low  = (pstat->etherStatsPkts256to511Octets.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;


        case ETHERSTATSPKTS512TO1023OCTETS:
            uint64_value.high = (pstat->etherStatsPkts512to1023Octets.l[0]);
            uint64_value.low  = (pstat->etherStatsPkts512to1023Octets.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSPKTS1024TO1518OCTETS:
            uint64_value.high = (pstat->etherStatsPkts1024to1518Octets.l[0]);
            uint64_value.low  = (pstat->etherStatsPkts1024to1518Octets.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSOWNER:
            memset(str_value, 0, RMON_OWNER_NAME_SIZE);

            if (NULL != pstat->etherStatsOwner)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [%s]\n", __FILE__, __LINE__, __func__ , pstat->etherStatsOwner);
                *var_len = strlen(pstat->etherStatsOwner);
                snprintf(str_value,  RMON_OWNER_NAME_SIZE, "%s", pstat->etherStatsOwner);
            }
            else
            {
                *var_len = 0;
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [-----]\n", __FILE__, __LINE__, __func__);
            }

            return str_value;


        case ETHERSTATSUNICASTPKTS:
            uint64_value.high = (pstat->etherStatsUnicastPkts.l[0]);
            uint64_value.low  = (pstat->etherStatsUnicastPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSBADPKTS:
            uint64_value.high = (pstat->etherStatsErrorPkts.l[0]);
            uint64_value.low  = (pstat->etherStatsErrorPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSINDISCARDPKTS:
            uint64_value.high = (pstat->etherStatsLossPkts.l[0]);
            uint64_value.low  = (pstat->etherStatsLossPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSSENTOCTETS:
            uint64_value.high = (pstat->etherStatsTxOctets.l[0]);
            uint64_value.low  = (pstat->etherStatsTxOctets.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSSENTPKTS:
            uint64_value.high = (pstat->etherStatsTxPkts.l[0]);
            uint64_value.low  = (pstat->etherStatsTxPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSSENTUNICASTPKTS:
            uint64_value.high = (pstat->etherStatsTxUnicastPkts.l[0]);
            uint64_value.low  = (pstat->etherStatsTxUnicastPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSSENTBROADCASTPKTS:
            uint64_value.high = (pstat->etherStatsTxBroadcastPkts.l[0]);
            uint64_value.low  = (pstat->etherStatsTxBroadcastPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSSENTMULTICASTPKTS:
            uint64_value.high = (pstat->etherStatsTxMulticastPkts.l[0]);
            uint64_value.low  = (pstat->etherStatsTxMulticastPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSSENTBADPKTS:
            uint64_value.high = (pstat->etherStatsTxErrorPkts.l[0]);
            uint64_value.low  = (pstat->etherStatsTxErrorPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSSENTDISCARDPKTS:
            uint64_value.high = (pstat->etherStatsTxLossPkts.l[0]);
            uint64_value.low  = (pstat->etherStatsTxLossPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERSTATSSTATUS:
            *var_len = sizeof(int);
            uint_value = pstat->etherStatsStatus ;
            return (u_char *)(&uint_value);

        default :
            return (NULL);
    }

}

int historyControlTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct rmon_HistoryControlGroup *index)
{
    struct rmon_HistoryControlGroup *prmon = NULL;
    struct ipc_mesg_n *pmsg = NULL;    
    int ret = 0;
    int data_num = 0;
    pmsg = historyControlTable_bulk(index->historyControlIndex, MODULE_ID_SNMPD, &data_num);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if( NULL == pmsg )
    {
        return FALSE ;
    }

    if (0 == data_num)
    {
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        prmon =  (struct rmon_HistoryControlGroup *)pmsg->msg_data;
        for (ret = 0; ret < data_num; ret++)
        {            
            snmp_cache_add(cache, prmon , sizeof(struct rmon_HistoryControlGroup));
            prmon++;
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct rmon_HistoryControlGroup *historyControlTable_data_lookup(struct ipran_snmp_data_cache *cache ,
                                                                 int exact,  const struct rmon_HistoryControlGroup  *index_input)
{
    struct listnode  *node, *nnode;

    struct rmon_HistoryControlGroup    *data1_find ;
    struct rmon_HistoryControlGroup        index1 ;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {

            return cache->data_list->head->data ;
        }

        if (0 == index_input->historyControlIndex)
        {

            return cache->data_list->head->data ;
        }

        if (index_input->historyControlIndex == data1_find->historyControlIndex)

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


u_char *historyControlTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    struct rmon_HistoryControlGroup *pstat = NULL;
    struct rmon_HistoryControlGroup index  ;
    int ret = 0;
    int index_int = 0 ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index_int , exact);

    index.historyControlIndex = index_int ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index.historyControlIndex);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == historyControlTable_cache)
    {
        historyControlTable_cache = snmp_cache_init(sizeof(struct rmon_HistoryControlGroup) ,
                                                    historyControlTable_data_form_ipc ,
                                                    historyControlTable_data_lookup);

        if (NULL == historyControlTable_cache)
        {
            return (NULL);
        }
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    pstat = snmp_cache_get_data_by_index(historyControlTable_cache , exact, &index);

    if (NULL == pstat)
    {
        return NULL;
    }

    if (!exact)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsIndex [%d] ifindex [%d]\n", __FILE__, __LINE__, __func__ ,
                   pstat->historyControlIndex ,
                   pstat->historyControlDataSource[RMON_IFINDEX_LOC]);
        ipran_snmp_int_index_set(vp, name, length, pstat->historyControlIndex);
    }

    switch (vp->magic)
    {
        case HISTORYCONTROLINDEX :
            *var_len = sizeof(int);
            int_value =  pstat->historyControlIndex ;
            return (u_char *)(&int_value);

        case HISTORYCONTROLDATASOURCE :
            *var_len = 11 * sizeof(oid) ;
            return (u_char *)pstat->historyControlDataSource;

        case HISTORYCONTROLBUCKETSREQUESTED :
            *var_len = sizeof(int);
            int_value =  pstat->historyControlBucketsRequested ;
            return (u_char *)(&int_value);

        case HISTORYCONTROLBUCKETSGRANTED :
            *var_len = sizeof(int);
            int_value =  pstat->historyControlBucketsGranted ;
            return (u_char *)(&int_value);

        case HISTORYCONTROLINTERVAL :
            *var_len = sizeof(int);
            int_value =  pstat->historyControlInterval ;
            return (u_char *)(&int_value);

        case HISTORYCONTROLOWNER :
            memset(str_value, 0, RMON_OWNER_NAME_SIZE);

            if (NULL != pstat->historyControlOwner)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [%s]\n", __FILE__, __LINE__, __func__ , pstat->historyControlOwner);
                *var_len = strlen(pstat->historyControlOwner);
                snprintf(str_value,  RMON_OWNER_NAME_SIZE, "%s", pstat->historyControlOwner);
            }
            else
            {
                *var_len = 0;
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [-----]\n", __FILE__, __LINE__, __func__);
            }

            return str_value;

        case HISTORYCONTROLSTATUS :
            *var_len = sizeof(int);
            uint_value = pstat->historyControlStatus ;
            return (u_char *)(&uint_value);

    }
}

int etherHistoryTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct rmon_EtherHistoryGroup *index)
{
    struct rmon_EtherHistoryGroup *prmon = NULL;
    struct ipc_mesg_n *pmsg = NULL;    
    int ret = 0;
    int data_num = 0;

    pmsg = etherHistoryTable_bulk(index->etherHistoryIndex , MODULE_ID_SNMPD, &data_num , index->etherHistorySampleIndex);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if( NULL == pmsg )
    {
        return FALSE ;
    }

    if (0 == data_num)
    {
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        prmon =  (struct rmon_EtherHistoryGroup *)pmsg->msg_data;    
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, prmon , sizeof(struct rmon_EtherHistoryGroup));
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d] etherHistorySampleIndex[%d]\n", __FILE__, __LINE__, __func__ ,
                       prmon->etherHistoryIndex , prmon->etherHistorySampleIndex);
            prmon++;
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct rmon_EtherHistoryGroup *etherHistoryTable_data_lookup(struct ipran_snmp_data_cache *cache ,
                                                             int exact,  const struct rmon_EtherHistoryGroup  *index_input)
{
    struct listnode  *node, *nnode;

    struct rmon_EtherHistoryGroup    *data1_find ;
    struct rmon_EtherHistoryGroup        index1 ;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {

            return cache->data_list->head->data ;
        }

        if (0 == index_input->etherHistoryIndex)
        {

            return cache->data_list->head->data ;
        }

        if (0 == index_input->etherHistorySampleIndex)
        {

            return cache->data_list->head->data ;
        }

        if (index_input->etherHistoryIndex == data1_find->etherHistoryIndex
                && index_input->etherHistorySampleIndex == data1_find->etherHistorySampleIndex)

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


u_char *etherHistoryTable_get(struct variable *vp,
                              oid *name,
                              size_t *length,
                              int exact, size_t *var_len, WriteMethod **write_method)
{
    struct rmon_EtherHistoryGroup  *pstat = NULL;
    struct rmon_EtherHistoryGroup  index  ;
    int ret = 0;
    int index_int = 0 ;
    int index_int2 = 0 ;

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index_int , &index_int2 , exact);

    index.etherHistoryIndex = index_int ;
    index.etherHistorySampleIndex = index_int2 ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d] etherHistorySampleIndex[%d]\n", __FILE__, __LINE__, __func__ ,
               index.etherHistoryIndex , index.etherHistorySampleIndex);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == etherHistoryTable_cache)
    {
        etherHistoryTable_cache = snmp_cache_init(sizeof(struct rmon_EtherHistoryGroup) ,
                                                  etherHistoryTable_data_form_ipc ,
                                                  etherHistoryTable_data_lookup);

        if (NULL == etherHistoryTable_cache)
        {
            return (NULL);
        }
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    pstat = snmp_cache_get_data_by_index(etherHistoryTable_cache , exact, &index);

    if (NULL == pstat)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, pstat->etherHistoryIndex , pstat->etherHistorySampleIndex);
    }

    switch (vp->magic)
    {
        case ETHERHISTORYINDEX :
            *var_len = sizeof(int);
            int_value =  pstat->etherHistoryIndex ;
            return (u_char *)(&int_value);

        case ETHERHISTORYSAMPLEINDEX :
            *var_len = sizeof(int);
            int_value =  pstat->etherHistorySampleIndex ;
            return (u_char *)(&int_value);

        case ETHERHISTORYINTERVALSTART :
            *var_len = sizeof(int);
            int_value =  pstat->etherHistoryIntervalStart * 100; //convertor to timeticks
            return (u_char *)(&int_value);

        case ETHERHISTORYDROPEVENTS :
            uint64_value.high = (pstat->etherHistoryDropEvents.l[0]);
            uint64_value.low  = (pstat->etherHistoryDropEvents.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERHISTORYOCTETS :
            uint64_value.high = (pstat->etherHistoryOctets.l[0]);
            uint64_value.low  = (pstat->etherHistoryOctets.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERHISTORYPKTS :
            uint64_value.high = (pstat->etherHistoryPkts.l[0]);
            uint64_value.low  = (pstat->etherHistoryPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;


        case ETHERHISTORYBROADCASTPKTS :
            uint64_value.high = (pstat->etherHistoryBroadcastPkts.l[0]);
            uint64_value.low  = (pstat->etherHistoryBroadcastPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;


        case ETHERHISTORYMULTICASTPKTS :
            uint64_value.high = (pstat->etherHistoryMulticastPkts.l[0]);
            uint64_value.low  = (pstat->etherHistoryMulticastPkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERHISTORYCRCALIGNERRORS :
            uint64_value.high = (pstat->etherHistoryCRCAlignErrors.l[0]);
            uint64_value.low  = (pstat->etherHistoryCRCAlignErrors.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;


        case ETHERHISTORYUNDERSIZEPKTS :
            uint64_value.high = (pstat->etherHistoryUndersizePkts.l[0]);
            uint64_value.low  = (pstat->etherHistoryUndersizePkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;


        case ETHERHISTORYOVERSIZEPKTS :
            uint64_value.high = (pstat->etherHistoryOversizePkts.l[0]);
            uint64_value.low  = (pstat->etherHistoryOversizePkts.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;


        case ETHERHISTORYFRAGMENTS :
            uint64_value.high = (pstat->etherHistoryFragments.l[0]);
            uint64_value.low  = (pstat->etherHistoryFragments.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERHISTORYJABBERS :
            uint64_value.high = (pstat->etherHistoryJabbers.l[0]);
            uint64_value.low  = (pstat->etherHistoryJabbers.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERHISTORYCOLLISIONS :
            uint64_value.high = (pstat->etherHistoryCollisions.l[0]);
            uint64_value.low  = (pstat->etherHistoryCollisions.l[1]);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case ETHERHISTORYUTILIZATION :
            *var_len = sizeof(int);
            int_value =  pstat->etherHistoryUtilization ;
            return (u_char *)(&int_value);
    }

}

int alarmTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct rmon_AlarmGroup *index)
{
    struct rmon_AlarmGroup *prmon = NULL;
    struct ipc_mesg_n *pmsg = NULL;        
    
    int ret = 0;
    int data_num = 0;
    pmsg = alarmTable_bulk(index->alarmIndex, MODULE_ID_SNMPD, &data_num);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if( NULL == pmsg )
    {
        return FALSE ;
    }

    if (0 == data_num)
    {
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        prmon =  (struct rmon_AlarmGroup *)pmsg->msg_data; 
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, prmon , sizeof(struct rmon_AlarmGroup));
            prmon++;
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct rmon_AlarmGroup *alarmTable_data_lookup(struct ipran_snmp_data_cache *cache,
                                               int exact, const struct rmon_AlarmGroup *index_input)
{
    struct listnode  *node, *nnode;
    struct rmon_AlarmGroup    *data1_find ;
    struct rmon_AlarmGroup        index1 ;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {

            return cache->data_list->head->data ;
        }

        if (0 == index_input->alarmIndex)
        {

            return cache->data_list->head->data ;
        }

        if (index_input->alarmIndex == data1_find->alarmIndex)

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

u_char *alarmTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{
    struct rmon_AlarmGroup *pstat = NULL;
    struct rmon_AlarmGroup index  ;
    int ret = 0;
    int index_int = 0 ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index_int , exact);

    index.alarmIndex = index_int ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index.alarmIndex);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == alarmTable_cache)
    {
        alarmTable_cache = snmp_cache_init(sizeof(struct rmon_AlarmGroup) ,
                                           alarmTable_data_form_ipc ,
                                           alarmTable_data_lookup);

        if (NULL == alarmTable_cache)
        {
            return (NULL);
        }
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);

    pstat = snmp_cache_get_data_by_index(alarmTable_cache , exact, &index);

    if (NULL == pstat)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pstat->alarmIndex);
    }

    switch (vp->magic)
    {
        case ALARMINDEX :
            *var_len = sizeof(int);
            int_value =  pstat->alarmIndex ;
            return (u_char *)(&int_value);

        case ALARMINTERVAL :
            *var_len = sizeof(int);
            int_value =  pstat->alarmInterval ;
            return (u_char *)(&int_value);

        case ALARMVARIABLE :
            *var_len = 12 * sizeof(oid) ;
            return (u_char *)pstat->alarmVariable;

        case ALARMSAMPLETYPE :
            *var_len = sizeof(int);
            int_value =  pstat->alarmSampleType ;
            return (u_char *)(&int_value);

        case ALARMVALUE :
            uint64_value.high = (pstat->alarmValue.l[0]);
            uint64_value.low  = (pstat->alarmValue.l[1]);
            *var_len = sizeof(int);
            return (u_char *)&uint64_value.low;

        case ALARMSTARTUPALARM :
            *var_len = sizeof(int);
            int_value =  pstat->alarmStartupAlarm;
            return (u_char *)(&int_value);

        case ALARMRISINGTHRESHOLD :
            uint64_value.high = (pstat->alarmRisingThreshold.l[0]);
            uint64_value.low  = (pstat->alarmRisingThreshold.l[1]);
            *var_len = sizeof(int);
            return (u_char *)&uint64_value.low;

        case ALARMFALLINGTHRESHOLD :
            uint64_value.high = (pstat->alarmFallingThreshold.l[0]);
            uint64_value.low  = (pstat->alarmFallingThreshold.l[1]);
            *var_len = sizeof(int);
            return (u_char *)&uint64_value.low;

        case ALARMRISINGEVENTINDEX :
            *var_len = sizeof(int);
            uint_value = pstat->alarmRisingEventIndex ;
            return (u_char *)(&uint_value);

        case ALARMFALLINGEVENTINDEX :
            *var_len = sizeof(int);
            uint_value = pstat->alarmFallingEventIndex ;
            return (u_char *)(&uint_value);

        case ALARMOWNER :
            memset(str_value, 0, RMON_OWNER_NAME_SIZE);

            if (NULL != pstat->alarmOwner)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [%s]\n", __FILE__, __LINE__, __func__ , pstat->alarmOwner);
                *var_len = strlen(pstat->alarmOwner);
                snprintf(str_value,  RMON_OWNER_NAME_SIZE, "%s", pstat->alarmOwner);
            }
            else
            {
                *var_len = 0;
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [-----]\n", __FILE__, __LINE__, __func__);
            }

            return str_value;

        case ALARMSTATUS :
            *var_len = sizeof(int);
            uint_value = pstat->alarmStatus ;
            return (u_char *)(&uint_value);

        default :
            return NULL;
    }
}

int eventTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct rmon_EventGroup *index)
{
    struct rmon_EventGroup *prmon = NULL;
    struct ipc_mesg_n *pmsg = NULL;      
    int ret = 0;
    int data_num = 0;
    prmon = eventTable_bulk(index->eventIndex, MODULE_ID_SNMPD, &data_num);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if( NULL == pmsg )
    {
        return FALSE ;
    }

    if (0 == data_num)
    {
        mem_share_free(pmsg, MODULE_ID_SNMPD);    
        return FALSE;
    }
    else
    {
        prmon =  (struct rmon_AlarmGroup *)pmsg->msg_data; 
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, prmon , sizeof(struct rmon_EventGroup));
            prmon++;
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct rmon_EventGroup *eventTable_data_lookup(struct ipran_snmp_data_cache *cache,
                                               int exact, const struct rmon_EventGroup *index_input)
{
    struct listnode  *node, *nnode;
    struct rmon_EventGroup    *data1_find ;
    struct rmon_EventGroup     index1 ;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {

            return cache->data_list->head->data ;
        }

        if (0 == index_input->eventIndex)
        {

            return cache->data_list->head->data ;
        }

        if (index_input->eventIndex == data1_find->eventIndex)

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

u_char *eventTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{
    struct rmon_EventGroup *pstat = NULL;
    struct rmon_EventGroup index  ;
    int ret = 0;
    int index_int = 0 ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index_int , exact);

    index.eventIndex = index_int ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index.eventIndex);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == eventTable_cache)
    {
        eventTable_cache = snmp_cache_init(sizeof(struct rmon_EventGroup) ,
                                           eventTable_data_form_ipc ,
                                           eventTable_data_lookup);

        if (NULL == eventTable_cache)
        {
            return (NULL);
        }
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    pstat = snmp_cache_get_data_by_index(eventTable_cache , exact, &index);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);

    if (NULL == pstat)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pstat->eventIndex);
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);

    switch (vp->magic)
    {
        case EVENTINDEX :
            *var_len = sizeof(int);
            int_value =  pstat->eventIndex ;
            return (u_char *)(&int_value);

        case EVENTDESCRIPTION :
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [-----]\n", __FILE__, __LINE__, __func__);
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [%s]\n", __FILE__, __LINE__, __func__ , pstat->eventDescription);
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [-----]\n", __FILE__, __LINE__, __func__);

            memset(str_value, 0, RMON_OWNER_NAME_SIZE);

            if (NULL != pstat->eventDescription)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [%s]\n", __FILE__, __LINE__, __func__ , pstat->eventDescription);
                *var_len = strlen(pstat->eventDescription);
                snprintf(str_value,  RMON_OWNER_NAME_SIZE, "%s", pstat->eventDescription);
            }
            else
            {
                *var_len = 0;
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [-----]\n", __FILE__, __LINE__, __func__);
            }

            return str_value;


        case EVENTTYPE :
            *var_len = sizeof(int);
            int_value =  pstat->eventType ;
            return (u_char *)(&int_value);


        case EVENTCOMMUNITY :
            memset(str_value, 0, RMON_OWNER_NAME_SIZE);

            if (NULL != pstat->eventCommunity)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [%s]\n", __FILE__, __LINE__, __func__ , pstat->eventCommunity);
                *var_len = strlen(pstat->eventCommunity);
                snprintf(str_value,  RMON_OWNER_NAME_SIZE, "%s", pstat->eventCommunity);
            }
            else
            {
                *var_len = 0;
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [-----]\n", __FILE__, __LINE__, __func__);
            }

            return str_value;

        case EVENTLASTTIMESENT :
            *var_len = sizeof(int);
            int_value =  pstat->eventLastTimeSent * 100;
            return (u_char *)(&int_value);

        case EVENTOWNER :
            memset(str_value, 0, RMON_OWNER_NAME_SIZE);

            if (NULL != pstat->eventOwner)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [%s]\n", __FILE__, __LINE__, __func__ , pstat->eventOwner);
                *var_len = strlen(pstat->eventOwner);
                snprintf(str_value,  RMON_OWNER_NAME_SIZE, "%s", pstat->eventOwner);
            }
            else
            {
                *var_len = 0;
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': pstat->etherStatsOwner [-----]\n", __FILE__, __LINE__, __func__);
            }

            return str_value;


        case EVENTSTATUS :
            *var_len = sizeof(int);
            int_value =  pstat->eventStatus ;
            return (u_char *)(&int_value);

        default :
            return NULL ;

    }

}

