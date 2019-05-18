/******************************************************************************
 * Filename: rmond_api.c
 *  Copyright (c) 2016-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: rmond_api functions for rmond.
 *
 * History:
 *  *2017.10.23  lidingcheng created
 *
******************************************************************************/
#include "lib/thread.h"
#include "lib/log.h"
#include "lib/memory.h"
#include "lib/timer.h"
#include "lib/linklist.h"
#include "lib/if.h"
#include "lib/hash.h"
#include "lib/stream.h"
#include "lib/prefix.h"
#include "lib/table.h"
#include "lib/module_id.h"
#include "lib/msg_ipc.h"
#include "lib/ifm_common.h"
#include "lib/rmon_common.h"
#include "lib/memtypes.h"
#include "lib/ifm_common.h"
#include "ifm/ifm.h"
#include "lib/msg_ipc_n.h"
#include "lib/memshare.h"


#include "rmon_config.h"
#include "rmond.h"
#include "rmond_main.h"
#include "rmon_timer.h"
#include "rmon_api.h"
#include "rmon_cli.h"


oid RMON_IFINDEX_OID[] = {RMON_IFINDEX_OID_VALUE};

static struct rmon_master *g_rm;


int init_rmon_master(void)
{
    g_rm = XCALLOC(MTYPE_RMON, sizeof(struct rmon_master));

    if (g_rm == NULL)
    {
        return RESULT_ERROR;
    }

    g_rm->etherStats_table     = route_table_init();
    g_rm->historyControl_table = route_table_init();
    g_rm->alarm_table          = route_table_init();
    g_rm->event_table          = route_table_init();
    g_rm->interface_table      = route_table_init();

    return RESULT_OK;
}

struct rmon_master *get_rmond_master_value(void)
{
    assert(g_rm);
    return g_rm;
}

void rmon_converter_struct_2_longlong(ut_int64_t in_data, uint64_t *out_data)
{
    if (NULL == out_data)
    {
        return;
    }

    uint64_t data_tmp = in_data.l[0];
    *out_data = (data_tmp << 32) | in_data.l[1];
}

void rmon_converter_longlong_2_struct(uint64_t in_data, ut_int64_t *out_data)
{
    if (NULL == out_data)
    {
        return;
    }

    out_data->l[0] = (in_data >> 32) & 0xFFFFFFFF;
    out_data->l[1] = in_data & 0xFFFFFFFF;
}

ut_int64_t rmon_struct_value_add(ut_int64_t data1, ut_int64_t data2)
{
    uint64_t lldata1 = 0;
    uint64_t lldata2 = 0;
    uint64_t llresult = 0;
    ut_int64_t temp_data;

    temp_data.ll = 0;
    rmon_converter_struct_2_longlong(data1, &lldata1);
    rmon_converter_struct_2_longlong(data2, &lldata2);

    llresult = lldata1 + lldata2;
    rmon_converter_longlong_2_struct(llresult, &temp_data);

    return temp_data;
}

ut_int64_t rmon_struct_value_sub(ut_int64_t data1, ut_int64_t data2)
{
    uint64_t lldata1 = 0;
    uint64_t lldata2 = 0;
    uint64_t llresult = 0;
    ut_int64_t temp_data;

    temp_data.ll = 0;
    rmon_converter_struct_2_longlong(data1, &lldata1);
    rmon_converter_struct_2_longlong(data2, &lldata2);

    llresult = lldata1 - lldata2;
    rmon_converter_longlong_2_struct(llresult, &temp_data);

    return temp_data;
}

int rmon_get_increment(struct rmon_if_stats *new_stats, struct rmon_if_stats *lst_stats, struct rmon_if_stats *pincrement)
{
    if (NULL == new_stats || NULL == lst_stats || NULL == pincrement)
    {
        return RESULT_ERROR;
    }

    pincrement->drop_events            = rmon_struct_value_sub(new_stats->drop_events,             lst_stats->drop_events);
    pincrement->rx_packets             = rmon_struct_value_sub(new_stats->rx_packets,              lst_stats->rx_packets);
    pincrement->tx_packets             = rmon_struct_value_sub(new_stats->tx_packets,              lst_stats->tx_packets);
    pincrement->tx_bytes               = rmon_struct_value_sub(new_stats->tx_bytes,              lst_stats->tx_bytes);
    pincrement->tx_dropped             = rmon_struct_value_sub(new_stats->tx_dropped,              lst_stats->tx_dropped);
    pincrement->good_octets_rcv        = rmon_struct_value_sub(new_stats->good_octets_rcv,         lst_stats->good_octets_rcv);
    pincrement->bad_octets_rcv         = rmon_struct_value_sub(new_stats->bad_octets_rcv,          lst_stats->bad_octets_rcv);
    pincrement->rx_errors         = rmon_struct_value_sub(new_stats->rx_errors,          lst_stats->rx_errors);

//    pincrement->stats_pkts             = rmon_struct_value_sub(new_stats->stats_pkts,              lst_stats->stats_pkts);
//    pincrement->stats_octets           = rmon_struct_value_sub(new_stats->stats_octets,            lst_stats->stats_octets);
    pincrement->rx_bytes             = rmon_struct_value_sub(new_stats->rx_bytes,              lst_stats->rx_bytes);

    pincrement->rx_multicast         = rmon_struct_value_sub(new_stats->rx_multicast,          lst_stats->rx_multicast);
    pincrement->stats_broadcast        = rmon_struct_value_sub(new_stats->stats_broadcast,         lst_stats->stats_broadcast);
    pincrement->mac_transmit_err       = rmon_struct_value_sub(new_stats->mac_transmit_err,        lst_stats->mac_transmit_err);
    pincrement->good_pkts_rcv          = rmon_struct_value_sub(new_stats->good_pkts_rcv,           lst_stats->good_pkts_rcv);
    pincrement->bad_pkts_rcv           = rmon_struct_value_sub(new_stats->bad_pkts_rcv,            lst_stats->bad_pkts_rcv);
//    pincrement->brdc_pkts_rcv          = rmon_struct_value_sub(new_stats->brdc_pkts_rcv,           lst_stats->brdc_pkts_rcv);
//    pincrement->mc_pkts_rcv            = rmon_struct_value_sub(new_stats->mc_pkts_rcv,             lst_stats->mc_pkts_rcv);
    pincrement->rx_broadcast          = rmon_struct_value_sub(new_stats->rx_broadcast,           lst_stats->rx_broadcast);
    pincrement->rx_ucast             = rmon_struct_value_sub(new_stats->rx_ucast,             lst_stats->rx_ucast);

    pincrement->pkts_64_octets         = rmon_struct_value_sub(new_stats->pkts_64_octets,          lst_stats->pkts_64_octets);
    pincrement->pkts_65_127_octets     = rmon_struct_value_sub(new_stats->pkts_65_127_octets,      lst_stats->pkts_65_127_octets);
    pincrement->pkts_128_255_octets    = rmon_struct_value_sub(new_stats->pkts_128_255_octets,     lst_stats->pkts_128_255_octets);
    pincrement->pkts_256_511_octets    = rmon_struct_value_sub(new_stats->pkts_256_511_octets,     lst_stats->pkts_256_511_octets);
    pincrement->pkts_512_1023_octets   = rmon_struct_value_sub(new_stats->pkts_512_1023_octets,    lst_stats->pkts_512_1023_octets);
    pincrement->pkts_1024_1518_octets   = rmon_struct_value_sub(new_stats->pkts_1024_1518_octets,    lst_stats->pkts_1024_1518_octets);
    pincrement->good_octets_sent       = rmon_struct_value_sub(new_stats->good_octets_sent,        lst_stats->good_octets_sent);
    pincrement->good_pkts_sent         = rmon_struct_value_sub(new_stats->good_pkts_sent,          lst_stats->good_pkts_sent);
    pincrement->excessive_collisions   = rmon_struct_value_sub(new_stats->excessive_collisions,    lst_stats->excessive_collisions);
//    pincrement->mc_pkts_sent           = rmon_struct_value_sub(new_stats->mc_pkts_sent,            lst_stats->mc_pkts_sent);
//    pincrement->brdc_pkts_sent         = rmon_struct_value_sub(new_stats->brdc_pkts_sent,          lst_stats->brdc_pkts_sent);
    pincrement->tx_multicast           = rmon_struct_value_sub(new_stats->tx_multicast,            lst_stats->tx_multicast);
    pincrement->tx_broadcast         = rmon_struct_value_sub(new_stats->tx_broadcast,          lst_stats->tx_broadcast);

    pincrement->unrecog_mac_cntr_rcv   = rmon_struct_value_sub(new_stats->unrecog_mac_cntr_rcv,    lst_stats->unrecog_mac_cntr_rcv);
    pincrement->fc_sent                = rmon_struct_value_sub(new_stats->fc_sent,                 lst_stats->fc_sent);
    pincrement->good_fc_rcv            = rmon_struct_value_sub(new_stats->good_fc_rcv,             lst_stats->good_fc_rcv);
    pincrement->rx_dropped             = rmon_struct_value_sub(new_stats->rx_dropped,              lst_stats->rx_dropped);
    pincrement->undersize_pkts         = rmon_struct_value_sub(new_stats->undersize_pkts,          lst_stats->undersize_pkts);
    pincrement->fragments_pkts         = rmon_struct_value_sub(new_stats->fragments_pkts,          lst_stats->fragments_pkts);
    pincrement->oversize_pkts_rcv      = rmon_struct_value_sub(new_stats->oversize_pkts_rcv,       lst_stats->oversize_pkts_rcv);
    pincrement->jabber_pkts            = rmon_struct_value_sub(new_stats->jabber_pkts,             lst_stats->jabber_pkts);
    pincrement->mac_rcv_error          = rmon_struct_value_sub(new_stats->mac_rcv_error,           lst_stats->mac_rcv_error);
    pincrement->bad_crc                = rmon_struct_value_sub(new_stats->bad_crc,                 lst_stats->bad_crc);
    pincrement->collisions             = rmon_struct_value_sub(new_stats->collisions,              lst_stats->collisions);
    pincrement->late_collisions        = rmon_struct_value_sub(new_stats->late_collisions,         lst_stats->late_collisions);
    pincrement->bad_fc_rcv             = rmon_struct_value_sub(new_stats->bad_fc_rcv,              lst_stats->bad_fc_rcv);
//    pincrement->in_discards            = rmon_struct_value_sub(new_stats->in_discards,             lst_stats->in_discards);
//    pincrement->unic_pkts_rcv          = rmon_struct_value_sub(new_stats->unic_pkts_rcv,           lst_stats->unic_pkts_rcv);
//    pincrement->unic_pkts_sent         = rmon_struct_value_sub(new_stats->unic_pkts_sent,          lst_stats->unic_pkts_sent);
    pincrement->rx_dropped            = rmon_struct_value_sub(new_stats->rx_dropped,             lst_stats->rx_dropped);
    pincrement->rx_ucast          = rmon_struct_value_sub(new_stats->rx_ucast,           lst_stats->rx_ucast);
    pincrement->tx_ucast         = rmon_struct_value_sub(new_stats->tx_ucast,          lst_stats->tx_ucast);

    pincrement->bad_pkts_sent          = rmon_struct_value_sub(new_stats->bad_pkts_sent,           lst_stats->bad_pkts_sent);
    pincrement->tx_errors          = rmon_struct_value_sub(new_stats->tx_errors,           lst_stats->tx_errors);

//    pincrement->out_discards           = rmon_struct_value_sub(new_stats->out_discards,            lst_stats->out_discards);
    pincrement->tx_dropped           = rmon_struct_value_sub(new_stats->tx_dropped,            lst_stats->tx_dropped);


    return RESULT_OK;
}
#if 0
struct ifm_counter *rmon_get_counter_from_sdk_msg(uint32_t ifindex, int module_id)
{
    return (struct ifm_counter *)ipc_send_hal_wait_reply(NULL, 0, 1, \
            MODULE_ID_HAL, \
            module_id, \
            IPC_TYPE_IFM, \
            IFNET_INFO_COUNTER, \
            IPC_OPCODE_GET, \
            ifindex);
}
#else
struct ifm_counter *rmon_get_counter_from_sdk_msg(uint32_t ifindex, int module_id)
{
    struct ipc_mesg_n * pmesg = NULL;
    struct ifm_counter iport_counter;

    memset(&iport_counter,0,sizeof(struct ifm_counter));

    //return ipc_send_hal_wait_reply(pdata, data_len, 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_IFM, subtype, IPC_OPCODE_GET, msg_index);
    pmesg =  ipc_sync_send_n2(NULL, 0, 1, \
            MODULE_ID_HAL, \
            module_id, \
            IPC_TYPE_IFM, \
            IFNET_INFO_COUNTER, \
            IPC_OPCODE_GET, \
            ifindex,
            2000);
    if(pmesg)
    {
        memcpy(&iport_counter,pmesg->msg_data,sizeof(iport_counter));
        mem_share_free(pmesg, MODULE_ID_L2);
        return &iport_counter;
    }
    return NULL;
}
#endif


int rmon_get_counter_from_sdk(uint32_t ifindex, struct rmon_if_stats *if_stats)
{
    struct ifm_counter *counter = NULL;

    if (NULL == if_stats)
    {
        return RESULT_ERROR;
    }

    memset(if_stats, 0, sizeof(struct rmon_if_stats));

    counter = rmon_get_counter_from_sdk_msg(ifindex, MODULE_ID_RMON);

    if (NULL == counter)
    {
        zlog_debug(RMON_MTYPE_HAL, " %s[%d] recv msg \n", __FUNCTION__, __LINE__);
        return RESULT_ERROR;
    }

    if_stats->ifindex = ifindex;

    rmon_converter_longlong_2_struct(counter->rx_packets,           &(if_stats->rx_packets));
    rmon_converter_longlong_2_struct(counter->drop_events,          &(if_stats->drop_events));
    rmon_converter_longlong_2_struct(counter->tx_packets,           &(if_stats->tx_packets));
    rmon_converter_longlong_2_struct(counter->tx_bytes,           &(if_stats->tx_bytes));
    rmon_converter_longlong_2_struct(counter->tx_dropped,           &(if_stats->tx_dropped));
    rmon_converter_longlong_2_struct(counter->good_octets_rcv,      &(if_stats->good_octets_rcv));
    rmon_converter_longlong_2_struct(counter->bad_octets_rcv,       &(if_stats->bad_octets_rcv));
//    rmon_converter_longlong_2_struct(counter->stats_octets,         &(if_stats->stats_octets));
    rmon_converter_longlong_2_struct(counter->rx_bytes,           &(if_stats->rx_bytes));

    rmon_converter_longlong_2_struct(counter->rx_multicast,      &(if_stats->rx_multicast));
    rmon_converter_longlong_2_struct(counter->stats_broadcast,      &(if_stats->stats_broadcast));
    rmon_converter_longlong_2_struct(counter->mac_transmit_err,     &(if_stats->mac_transmit_err));
    rmon_converter_longlong_2_struct(counter->good_pkts_rcv,        &(if_stats->good_pkts_rcv));
    rmon_converter_longlong_2_struct(counter->rx_errors,         &(if_stats->rx_errors));
//    rmon_converter_longlong_2_struct(counter->brdc_pkts_rcv,        &(if_stats->brdc_pkts_rcv));
//    rmon_converter_longlong_2_struct(counter->mc_pkts_rcv,          &(if_stats->mc_pkts_rcv));
    rmon_converter_longlong_2_struct(counter->rx_broadcast,        &(if_stats->rx_broadcast));
    rmon_converter_longlong_2_struct(counter->rx_ucast,          &(if_stats->rx_ucast));

    rmon_converter_longlong_2_struct(counter->pkts_64_octets,       &(if_stats->pkts_64_octets));
    rmon_converter_longlong_2_struct(counter->pkts_65_127_octets,   &(if_stats->pkts_65_127_octets));
    rmon_converter_longlong_2_struct(counter->pkts_128_255_octets,  &(if_stats->pkts_128_255_octets));
    rmon_converter_longlong_2_struct(counter->pkts_256_511_octets,  &(if_stats->pkts_256_511_octets));
    rmon_converter_longlong_2_struct(counter->pkts_512_1023_octets, &(if_stats->pkts_512_1023_octets));
    rmon_converter_longlong_2_struct(counter->oversize_pkts_rcv, &(if_stats->oversize_pkts_rcv));
    rmon_converter_longlong_2_struct(counter->pkts_1024_1518_octets, &(if_stats->pkts_1024_1518_octets));

    rmon_converter_longlong_2_struct(counter->pkts_1519_max_octets, &(if_stats->pkts_1519_max_octets));

    rmon_converter_longlong_2_struct(counter->good_octets_sent,     &(if_stats->good_octets_sent));
    rmon_converter_longlong_2_struct(counter->good_pkts_sent,       &(if_stats->good_pkts_sent));
    rmon_converter_longlong_2_struct(counter->excessive_collisions, &(if_stats->excessive_collisions));
//    rmon_converter_longlong_2_struct(counter->mc_pkts_sent,         &(if_stats->mc_pkts_sent));
//    rmon_converter_longlong_2_struct(counter->brdc_pkts_sent,       &(if_stats->brdc_pkts_sent));
    rmon_converter_longlong_2_struct(counter->tx_multicast,         &(if_stats->tx_multicast));
    rmon_converter_longlong_2_struct(counter->tx_broadcast,       &(if_stats->tx_broadcast));

    rmon_converter_longlong_2_struct(counter->unrecog_mac_cntr_rcv, &(if_stats->unrecog_mac_cntr_rcv));
    rmon_converter_longlong_2_struct(counter->fc_sent,              &(if_stats->fc_sent));
    rmon_converter_longlong_2_struct(counter->good_fc_rcv,          &(if_stats->good_fc_rcv));
    rmon_converter_longlong_2_struct(counter->rx_dropped,           &(if_stats->rx_dropped));
    rmon_converter_longlong_2_struct(counter->undersize_pkts,       &(if_stats->undersize_pkts));
    rmon_converter_longlong_2_struct(counter->fragments_pkts,       &(if_stats->fragments_pkts));
    rmon_converter_longlong_2_struct(counter->oversize_pkts,        &(if_stats->oversize_pkts));
    rmon_converter_longlong_2_struct(counter->jabber_pkts,          &(if_stats->jabber_pkts));
    rmon_converter_longlong_2_struct(counter->mac_rcv_error,        &(if_stats->mac_rcv_error));
    rmon_converter_longlong_2_struct(counter->bad_crc,              &(if_stats->bad_crc));
    rmon_converter_longlong_2_struct(counter->collisions,           &(if_stats->collisions));
    rmon_converter_longlong_2_struct(counter->late_collisions,      &(if_stats->late_collisions));
    rmon_converter_longlong_2_struct(counter->bad_fc_rcv,           &(if_stats->bad_fc_rcv));
//    rmon_converter_longlong_2_struct(counter->in_discards,          &(if_stats->in_discards));
//    rmon_converter_longlong_2_struct(counter->unic_pkts_rcv,        &(if_stats->unic_pkts_rcv));
//    rmon_converter_longlong_2_struct(counter->unic_pkts_sent,       &(if_stats->unic_pkts_sent));
    rmon_converter_longlong_2_struct(counter->rx_dropped,          &(if_stats->rx_dropped));
    rmon_converter_longlong_2_struct(counter->rx_ucast,        &(if_stats->rx_ucast));
    rmon_converter_longlong_2_struct(counter->tx_ucast,       &(if_stats->tx_ucast));

    rmon_converter_longlong_2_struct(counter->tx_errors,        &(if_stats->tx_errors));
//    rmon_converter_longlong_2_struct(counter->out_discards,         &(if_stats->out_discards));
    rmon_converter_longlong_2_struct(counter->tx_dropped,         &(if_stats->tx_dropped));

    return RESULT_OK;
}

struct rmon_interface *rmon_interface_base_counter(uint32_t ifindex)
{
    struct route_node *rn = NULL;
    struct prefix p;
    struct rmon_interface *pnew = NULL;

    if (NULL == g_rm || NULL == g_rm->interface_table)
    {
        return NULL;
    }

    memset(&p, 0, sizeof(struct prefix));
    PREFIX_RMON_SET(&p, ifindex);

    rn = route_node_get(g_rm->interface_table, &p);

    if (NULL != rn)
    {
        if (NULL == rn->info)
        {
            pnew = XCALLOC(MTYPE_RMON, sizeof(struct rmon_interface));

            if (NULL == pnew)
            {
                return NULL;
            }

            memset(&pnew->if_init_cntrs, 0, sizeof(struct rmon_if_stats));
            memset(&pnew->if_hist_cntrs, 0, sizeof(struct rmon_if_stats));
            pnew->ifindex = ifindex;

            RMON_INFO_SET(rn, pnew);
        }
        else
        {
            pnew = (struct rmon_interface *)rn->info;
        }

        return pnew;
    }

    return NULL;
}

int rmon_interface_base_counter_curr_update(struct rmon_interface *prmon_interface)
{
    if (NULL == prmon_interface)
    {
        return RESULT_ERROR;
    }

    rmon_get_counter_from_sdk(prmon_interface->ifindex, &prmon_interface->if_init_cntrs);

    return RESULT_OK;
}

int rmon_interface_base_counter_hist_update(struct rmon_interface *prmon_interface)
{
    if (NULL == prmon_interface)
    {
        return RESULT_ERROR;
    }

    rmon_get_counter_from_sdk(prmon_interface->ifindex, &prmon_interface->if_hist_cntrs);

    return RESULT_OK ;
}

int rmon_interface_base_counter_get(uint32_t ifindex, struct rmon_interface *data_out)
{
    struct route_node *rn = NULL;
    struct prefix p;
    struct rmon_interface *prmon_interface = NULL;

    if (NULL == g_rm || NULL == g_rm->interface_table || NULL == data_out)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, ifindex);
    rn = route_node_lookup(g_rm->interface_table, &p);

    if (NULL == rn || NULL == rn->info)
    {
        return RESULT_ERROR;
    }

    prmon_interface = (struct rmon_interface *)rn->info;
    memcpy(data_out, prmon_interface, sizeof(struct rmon_interface));

    route_unlock_node(rn);

    return RESULT_OK;
}

int rmon_interface_counter_curr_increment(uint32_t ifindex,           struct rmon_if_stats *pincrement)
{
    struct rmon_if_stats  curr_data;
    struct rmon_interface data_ifinterface;
    int ret = 0;

    if (NULL == pincrement)
    {
        return RESULT_ERROR;
    }

    ret = rmon_get_counter_from_sdk(ifindex, &curr_data);

    if (RESULT_OK != ret)
    {
        zlog_debug(RMON_MTYPE_HAL, " %s[%d] recv msg \n", __FUNCTION__, __LINE__);
        return RESULT_ERROR;
    }

    ret = rmon_interface_base_counter_get(ifindex, &data_ifinterface);

    if (RESULT_OK != ret)
    {
        zlog_debug(RMON_MTYPE_HAL, " %s[%d] recv msg \n", __FUNCTION__, __LINE__);
        return RESULT_ERROR;
    }

    rmon_get_increment(&curr_data, &data_ifinterface.if_init_cntrs, pincrement);

    return RESULT_OK;
}

int rmon_interface_counter_hist_increment(uint32_t ifindex,           struct rmon_if_stats *pincrement)
{
    struct rmon_if_stats  curr_data;
    struct rmon_interface data_ifinterface;
    int ret = 0;

    if (NULL == pincrement)
    {
        return RESULT_ERROR;
    }

    ret = rmon_get_counter_from_sdk(ifindex, &curr_data);

    if (RESULT_OK != ret)
    {
        return RESULT_ERROR;
    }

    ret = rmon_interface_base_counter_get(ifindex, &data_ifinterface);

    if (RESULT_OK != ret)
    {
        return RESULT_ERROR;
    }

    rmon_get_increment(&curr_data, &data_ifinterface.if_hist_cntrs, pincrement);

    return RESULT_OK;
}

int rmon_if_stats_2_rmon_etherStatsGroup(struct rmon_if_stats *if_stats ,
        struct rmon_etherStatsGroup *rinfo)
{
    rinfo->etherStatsDropEvents = if_stats->drop_events;
    rinfo->etherStatsOctets = if_stats->rx_bytes;
    rinfo->etherStatsPkts = if_stats->rx_packets;
    rinfo->etherStatsBroadcastPkts = if_stats->rx_broadcast;
    rinfo->etherStatsMulticastPkts = if_stats->rx_multicast;
    rinfo->etherStatsCRCAlignErrors = if_stats->bad_crc;
    rinfo->etherStatsUndersizePkts = if_stats->undersize_pkts;
    rinfo->etherStatsOversizePkts = if_stats->oversize_pkts_rcv;
    rinfo->etherStatsFragments = if_stats->fragments_pkts;
    rinfo->etherStatsJabbers = if_stats->jabber_pkts;
    rinfo->etherStatsCollisions = if_stats->collisions;
    rinfo->etherStatsPkts64Octets = if_stats->pkts_64_octets;
    rinfo->etherStatsPkts65to127Octets = if_stats->pkts_65_127_octets;
    rinfo->etherStatsPkts128to255Octets = if_stats->pkts_128_255_octets;
    rinfo->etherStatsPkts256to511Octets = if_stats->pkts_256_511_octets;
    rinfo->etherStatsPkts512to1023Octets = if_stats->pkts_512_1023_octets;
    rinfo->etherStatsPkts1024to1518Octets = if_stats->pkts_1024_1518_octets;
    rinfo->etherStatsLossPkts = if_stats->rx_dropped;
    rinfo->etherStatsErrorPkts = if_stats->rx_errors;
    rinfo->etherStatsUnicastPkts = if_stats->rx_ucast;
    rinfo->etherStatsTxOctets = if_stats->tx_bytes;
    rinfo->etherStatsTxPkts = if_stats->tx_packets;
    rinfo->etherStatsTxMulticastPkts = if_stats->tx_multicast;
    rinfo->etherStatsTxBroadcastPkts = if_stats->tx_broadcast;
    rinfo->etherStatsTxUnicastPkts = if_stats->tx_ucast;
    rinfo->etherStatsTxErrorPkts = if_stats->tx_errors;
    rinfo->etherStatsTxLossPkts = if_stats->tx_dropped;


    return RESULT_OK ;

}

s_int32_t rmon_coll_stats_entry_status(u_int32_t index)
{
    struct prefix p;
    struct route_node *rn;

    if (NULL == g_rm || NULL == g_rm->etherStats_table)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->etherStats_table, &p);

    if (NULL != rn)
    {
        route_unlock_node(rn);

        return RESULT_OK;
    }

    return RESULT_ERROR;
}

struct rmon_etherStatsGroup *rmon_coll_stats_entry_info_lookup(u_int32_t index)
{
    struct route_node *rn;
    struct prefix p;
    struct rmon_etherStatsGroup *rmon_ether_stats;

    if (NULL == g_rm || g_rm->etherStats_table)
    {
        return NULL;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->etherStats_table, &p);

    if (NULL == rn)
    {
        return NULL;
    }

    route_unlock_node(rn);
    rmon_ether_stats = (struct rmon_etherStatsGroup *)rn->info;

    return rmon_ether_stats;
}

s_int32_t rmon_coll_stats_entry_add(u_int32_t ifindex, u_int32_t index, char *ownername)
{
    struct route_node *rn;
    struct prefix p;
    struct rmon_etherStatsGroup *rmon_ether_stats = NULL;
    struct rmon_interface *prmon_interface = NULL;

    if (NULL == g_rm || NULL == g_rm->etherStats_table)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_get(g_rm->etherStats_table, &p);

    if (NULL == rn)
    {
        return RESULT_ERROR;
    }

    rmon_ether_stats = rn->info;

    if (NULL == rmon_ether_stats)
    {
        rmon_ether_stats = XCALLOC(MTYPE_RMON, sizeof(struct rmon_etherStatsGroup));

        if (NULL == rmon_ether_stats)
        {
            return RMON_ERR_NOMEM;
        }

        memset(rmon_ether_stats, 0, sizeof(struct rmon_etherStatsGroup));
        rmon_ether_stats->etherStatsIndex = index;

        RMON_INFO_SET(rn, rmon_ether_stats);
    }
    else
    {
        route_unlock_node(rn);
    }

    if (ownername)
    {
        snprintf(rmon_ether_stats->etherStatsOwner, RMON_OWNER_NAME_SIZE, "%s", ownername);
    }
    else
    {
        snprintf(rmon_ether_stats->etherStatsOwner, RMON_OWNER_NAME_SIZE, "%s", "RMON_SNMP");
    }

    memset(rmon_ether_stats->etherStatsDataSource, 0, sizeof(rmon_ether_stats->etherStatsDataSource));
    memcpy(rmon_ether_stats->etherStatsDataSource, RMON_IFINDEX_OID, sizeof(rmon_ether_stats->etherStatsDataSource)); // FIXME
    rmon_ether_stats->etherStatsDataSource[RMON_IFINDEX_LOC] = ifindex;

    rmon_ether_stats->etherStatsStatus = VALID_STATUS;

    prmon_interface = rmon_interface_base_counter(ifindex);

    if (NULL != prmon_interface)
    {
        rmon_interface_base_counter_curr_update(prmon_interface);
    }

    return RESULT_OK;
}

s_int32_t rmon_coll_stats_entry_remove(u_int32_t index)
{
    struct route_node *rn;
    struct prefix p;
    struct rmon_etherStatsGroup *rmon_ether_stats;

    if (NULL == g_rm || NULL == g_rm->etherStats_table)
    {
        return RESULT_ERROR;
    }

    /*liuyf add for check if stats clould be deleted 2015-03-31*/
    if (rmon_check_stats_group_in_alarm_group(index) == RMON_API_SET_SUCCESS)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->etherStats_table, &p);

    if (NULL == rn)
    {
        return RESULT_ERROR;
    }

    rmon_ether_stats = rn->info;

    RMON_INFO_UNSET(rn);

    /* To unlock the read lock set in route_node_lookup */
    route_unlock_node(rn);
    /* To unlock the read lock set in route_node_get  */
    route_unlock_node(rn);

    XFREE(MTYPE_RMON, rmon_ether_stats);

    return RESULT_OK;
}

s_int32_t rmon_coll_stats_entry_remove_force(u_int32_t index)
{
    struct route_node *rn;
    struct prefix p;
    struct rmon_etherStatsGroup *rmon_ether_stats;

    if (NULL == g_rm)
    {
        return RESULT_ERROR;
    }

    /*liuyf add for check if stats clould be deleted 2015-03-31*/
    if (rmon_check_stats_group_in_alarm_group(index) == RMON_API_SET_SUCCESS)
    {
        for (rn = route_top(g_rm->alarm_table); rn; rn = route_next(rn))
        {
            struct rmon_AlarmGroup *alarm_stats;

            alarm_stats = rn->info;

            if (alarm_stats
                    && (alarm_stats->alarmRisingEventIndex == index
                        || alarm_stats->alarmFallingEventIndex == index))
            {
                zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d]:'%s': alarm_stats->alarmIndex [%d] \r\n", __FILE__, __LINE__, __func__, alarm_stats->alarmIndex);
                rmon_alarm_index_remove(alarm_stats->alarmIndex);
            }
        }
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->etherStats_table, &p);

    if (NULL == rn)
    {
        return RESULT_ERROR;
    }

    rmon_ether_stats = rn->info;

    RMON_INFO_UNSET(rn);
    /* To unlock the read lock set in route_node_lookup */
    route_unlock_node(rn);
    route_unlock_node(rn);
    XFREE(MTYPE_RMON, rmon_ether_stats);
    return RESULT_OK;
}

s_int32_t rmon_coll_stats_snmp_update_status(u_int32_t index, u_int32_t status)
{
    struct prefix p;
    struct route_node *rn = NULL;
    struct rmon_etherStatsGroup *rmon_ether_stats = NULL;

    if (NULL == g_rm || NULL == g_rm->etherStats_table)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);

    switch (status)
    {
        case VALID_STATUS:
            rmon_ether_stats = rmon_coll_stats_entry_info_lookup(index);

            if (NULL != rmon_ether_stats)
            {
                rmon_ether_stats->etherStatsStatus = VALID_STATUS;
                return RESULT_OK;
            }
            else
            {
                return RESULT_ERROR;
            }

        case CREATE_REQ_STATUS:
            rn = route_node_get(g_rm->etherStats_table, &p);

            if (rn == NULL)
            {
                return RESULT_ERROR;
            }

            if (rn->info == NULL)
            {
                rmon_ether_stats = XCALLOC(MTYPE_RMON, sizeof(struct rmon_etherStatsGroup));

                if (NULL == rmon_ether_stats)
                {
                    return RESULT_ERROR;
                }

                memset(rmon_ether_stats, 0, sizeof(struct rmon_etherStatsGroup));
                rmon_ether_stats->etherStatsIndex = index;
                rmon_ether_stats->etherStatsStatus = UNDER_CREATION_STATUS;

                RMON_INFO_SET(rn, rmon_ether_stats);
            }
            else
            {
                rmon_ether_stats->etherStatsStatus = CREATE_REQ_STATUS;
                route_unlock_node(rn);
            }

            return RESULT_OK;

        case UNDER_CREATION_STATUS:
            return RESULT_OK;

        case INVALID_STATUS:
            return rmon_coll_stats_entry_remove(index);

        default :
            return RESULT_ERROR;
    }

    return RESULT_ERROR;
}

s_int32_t rmon_coll_history_entry_status(u_int32_t index)
{
    struct prefix p;
    struct route_node *rn;

    if (NULL == g_rm || NULL == g_rm->historyControl_table)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->historyControl_table, &p);

    if (NULL != rn)
    {
        route_unlock_node(rn);
        return RESULT_OK;
    }

    return RESULT_ERROR;
}

struct rmon_HistoryControlGroup *rmon_coll_history_lookup_by_ifindex(u_int32_t ifindex, u_int32_t interval)
{
    struct rmon_HistoryControlGroup *rmon_history_ctrl_grp = NULL;
    struct route_node *rn;
    for (rn = route_top(g_rm->historyControl_table); rn; rn = route_next(rn))
    {
        struct rmon_HistoryControlGroup *rinfo = NULL;

        rinfo = (struct rmon_HistoryControlGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue;
        }

        if (ifindex == rinfo->historyControlDataSource[RMON_IFINDEX_LOC])
        {
            rmon_history_ctrl_grp = (struct rmon_HistoryControlGroup *)rn->info;

            if (interval == rmon_history_ctrl_grp->historyControlInterval)
            {
                return rmon_history_ctrl_grp;
            }
        }
    }

    return NULL;
}


struct rmon_HistoryControlGroup *rmon_coll_history_entry_info_lookup(u_int32_t index)
{
    struct rmon_HistoryControlGroup *rmon_history_ctrl_grp = NULL;
    struct route_node *rn;
    struct prefix p;

    if (NULL == g_rm || NULL == g_rm->historyControl_table)
    {
        return NULL;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->historyControl_table, &p);

    if (NULL == rn)
    {
        return NULL;
    }

    route_unlock_node(rn);
    rmon_history_ctrl_grp = (struct rmon_HistoryControlGroup *)rn->info;

    return rmon_history_ctrl_grp;
}

s_int32_t rmon_coll_history_entry_active(struct rmon_HistoryControlGroup *prmon_history_ctrl)
{
    struct thread_master   *thread_rmon = (struct thread_master *)get_rmond_thread_master();

    if (NULL == prmon_history_ctrl || NULL == thread_rmon)
    {
        return RESULT_ERROR;
    }

    if (VALID_STATUS != prmon_history_ctrl->historyControlStatus)
    {
        prmon_history_ctrl->current_sample_no   = 1;
        prmon_history_ctrl->linearity_sample_no = 1;
    }

    if (NULL == prmon_history_ctrl->historyStats_table)
    {
        prmon_history_ctrl->historyStats_table = route_table_init();
    }

    prmon_history_ctrl->historyControlStatus    = VALID_STATUS;

    //RMON_TIMER_OFF(prmon_history_ctrl->rmon_coll_history_timer);
    //prmon_history_ctrl->rmon_coll_history_timer = thread_add_timer(thread_rmon, \
    //        rmon_coll_history_timer_handler, \
    //        prmon_history_ctrl, \
    //        prmon_history_ctrl->historyControlInterval);

	high_pre_timer_delete(prmon_history_ctrl->rmon_coll_history_timer);
    //printf("%s[%d]\n\r", __FUNCTION__, __LINE__);
	prmon_history_ctrl->rmon_coll_history_timer = high_pre_timer_add("rmonHistCollTimer", \
			LIB_TIMER_TYPE_LOOP, \
			rmon_coll_history_timer_handler, \
			prmon_history_ctrl, \
            prmon_history_ctrl->historyControlInterval*1000);
    //printf("%s[%d]\n\r", __FUNCTION__, __LINE__);

    return RESULT_OK;
}

s_int32_t rmon_coll_history_entry_inactive(struct rmon_HistoryControlGroup *prmon_history_ctrl)
{
    struct route_node *rn = NULL;

    if (NULL == prmon_history_ctrl)
    {
        return RESULT_ERROR;
    }

    //RMON_TIMER_OFF(prmon_history_ctrl->rmon_coll_history_timer);
	high_pre_timer_delete(prmon_history_ctrl->rmon_coll_history_timer);
	prmon_history_ctrl->rmon_coll_history_timer = NULL;

    if (prmon_history_ctrl->historyStats_table)
    {
        for (rn = route_top(prmon_history_ctrl->historyStats_table); rn; rn = route_next(rn))
        {
            if (rn->info)
            {
                XFREE(MTYPE_RMON, rn->info);
                rn->info = NULL;
            }
        }

        route_table_finish(prmon_history_ctrl->historyStats_table);
        prmon_history_ctrl->historyStats_table = NULL;
    }

    prmon_history_ctrl->historyControlStatus = INVALID_STATUS;

    return RESULT_OK;
}

s_int32_t rmon_coll_history_entry_add(u_int32_t ifindex, u_int32_t index, u_int32_t bucketno, u_int32_t interval, char *ownername)
{
    struct route_node *rn;
    struct prefix p;
    struct rmon_HistoryControlGroup *rmon_history_ctrl_grp = NULL;
    struct rmon_interface *prmon_interface = NULL;

    if (NULL == g_rm || NULL == g_rm->historyControl_table)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_get(g_rm->historyControl_table, &p);

    if (NULL == rn)
    {
        return RESULT_ERROR;
    }

    rmon_history_ctrl_grp = rn->info;

    if (NULL == rmon_history_ctrl_grp)
    {
        rmon_history_ctrl_grp = XCALLOC(MTYPE_RMON, sizeof(struct rmon_HistoryControlGroup));

        if (NULL == rmon_history_ctrl_grp)
        {
            return RMON_ERR_NOMEM;
        }

        memset(rmon_history_ctrl_grp, 0, sizeof(struct rmon_HistoryControlGroup));
        rmon_history_ctrl_grp->historyControlIndex = index;

        RMON_INFO_SET(rn, rmon_history_ctrl_grp);
    }
    else
    {
        route_unlock_node(rn);
    }

    if (ownername)
    {
        snprintf(rmon_history_ctrl_grp->historyControlOwner, RMON_OWNER_NAME_SIZE, "%s", ownername);
    }
    else
    {
        snprintf(rmon_history_ctrl_grp->historyControlOwner, RMON_OWNER_NAME_SIZE, "%s", "RMON_SNMP");
    }

    memset(rmon_history_ctrl_grp->historyControlDataSource, 0, sizeof(rmon_history_ctrl_grp->historyControlDataSource));
    memcpy(rmon_history_ctrl_grp->historyControlDataSource, RMON_IFINDEX_OID, sizeof(rmon_history_ctrl_grp->historyControlDataSource)); // FIXME
    rmon_history_ctrl_grp->historyControlDataSource[RMON_IFINDEX_LOC] = ifindex;

    rmon_history_ctrl_grp->historyControlBucketsRequested = bucketno;
    rmon_history_ctrl_grp->historyControlBucketsGranted   = bucketno;
    rmon_history_ctrl_grp->historyControlInterval         = interval;

    prmon_interface = rmon_interface_base_counter(ifindex);

    if (NULL != prmon_interface)
    {
        rmon_interface_base_counter_hist_update(prmon_interface);
    }

    return RESULT_OK;
}

s_int32_t rmon_coll_history_entry_remove(u_int32_t index)
{
    struct rmon_HistoryControlGroup *rmon_history_ctrl_grp;
    struct prefix p;
    struct route_node *rn;

    if (NULL == g_rm || NULL == g_rm->historyControl_table)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->historyControl_table, &p);

    if (rn == NULL)
    {
        return RESULT_ERROR;
    }

    rmon_history_ctrl_grp = rn->info;
    /* Stop the running timer */
    rmon_coll_history_entry_inactive(rmon_history_ctrl_grp);
    RMON_INFO_UNSET(rn);
    /* To unlock the read lock set in route_node_lookup */
    route_unlock_node(rn);
    /* To unlock the read lock set in route_node_get  */
    route_unlock_node(rn);
    XFREE(MTYPE_RMON, rmon_history_ctrl_grp);
    return RESULT_OK;
}

s_int32_t rmon_coll_history_snmp_update_status(u_int32_t index, u_int32_t status)
{
    struct prefix p;
    struct route_node *rn = NULL;
    struct rmon_HistoryControlGroup *rmon_history_ctrl_grp = NULL;

    if (NULL == g_rm || NULL == g_rm->historyControl_table)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);

    switch (status)
    {
        case VALID_STATUS:
            rmon_history_ctrl_grp = rmon_coll_history_entry_info_lookup(index);

            if (NULL != rmon_history_ctrl_grp)
            {
                rmon_history_ctrl_grp->historyControlStatus = VALID_STATUS;
                return RESULT_OK;
            }
            else
            {
                return RESULT_ERROR;
            }

        case CREATE_REQ_STATUS:
            rn = route_node_get(g_rm->historyControl_table, &p);

            if (rn == NULL)
            {
                return RESULT_ERROR;
            }

            if (rn->info == NULL)
            {
                rmon_history_ctrl_grp = XCALLOC(MTYPE_RMON, sizeof(struct rmon_HistoryControlGroup));

                if (NULL == rmon_history_ctrl_grp)
                {
                    return RMON_ERR_NOMEM;
                }

                memset(rmon_history_ctrl_grp, 0, sizeof(struct rmon_HistoryControlGroup));
                rmon_history_ctrl_grp->historyControlIndex = index;
                rmon_history_ctrl_grp->historyControlStatus = UNDER_CREATION_STATUS;

                RMON_INFO_SET(rn, rmon_history_ctrl_grp);
            }
            else
            {
                rmon_history_ctrl_grp->historyControlStatus = CREATE_REQ_STATUS;
                route_unlock_node(rn);
            }

            return RESULT_OK;

        case UNDER_CREATION_STATUS:
            return RESULT_OK;

        case INVALID_STATUS:
            return rmon_coll_history_entry_remove(index);

        default :
            return RESULT_ERROR;
    }

    return RESULT_ERROR;
}

s_int32_t rmon_event_entry_status(u_int32_t index)
{
    struct prefix p;
    struct route_node *rn;

    if (NULL == g_rm || NULL == g_rm->event_table)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->event_table, &p);

    if (NULL != rn)
    {
        route_unlock_node(rn);
        return RESULT_OK;
    }

    return RESULT_ERROR;
}

struct rmon_EventGroup *rmon_event_entry_info_lookup(u_int32_t index)
{
    struct route_node *rn = NULL;
    struct prefix p;
    struct rmon_EventGroup *rmon_event = NULL;

    if (NULL == g_rm || NULL == g_rm->event_table)
    {
        return NULL;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->event_table, &p);

    if (NULL == rn)
    {
        return NULL;
    }

    route_unlock_node(rn);
    rmon_event = (struct rmon_EventGroup *)rn->info;

    return rmon_event;
}

s_int32_t rmon_event_entry_add(u_int32_t index, int type, char *description, char *community, char *ownername)
{
    struct route_node *rn;
    struct prefix p;
    struct rmon_EventGroup *rmon_event = NULL;

    if (NULL == g_rm || NULL == g_rm->event_table)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_get(g_rm->event_table, &p);

    if (NULL == rn)
    {
        return RESULT_ERROR;
    }

    rmon_event = rn->info;

    if (NULL == rmon_event)
    {
        rmon_event = XCALLOC(MTYPE_RMON, sizeof(struct rmon_EventGroup));

        if (NULL == rmon_event)
        {
            return RESULT_ERROR;
        }

        memset(rmon_event, 0, sizeof(struct rmon_EventGroup));
        rmon_event->eventIndex = index;
        RMON_INFO_SET(rn, rmon_event);
    }
    else
    {
        route_unlock_node(rn);
    }

    rmon_event->eventType = type;

    if (description)
    {
        snprintf(rmon_event->eventDescription, RMON_DESCR_LENGTH, "%s", description);
        SET_FLAG(rmon_event->eventConf, RMON_EVENT_CONF_DESCRIPTION);
    }
    else
    {
        snprintf(rmon_event->eventDescription, RMON_DESCR_LENGTH, "%s", "RMON_SNMP");
    }

    if (community)
    {
        snprintf(rmon_event->eventCommunity, RMON_COMM_LENGTH, "%s", community);
        SET_FLAG(rmon_event->eventConf, RMON_EVENT_CONF_OWNER);
    }
    else
    {
        snprintf(rmon_event->eventCommunity, RMON_COMM_LENGTH, "%s", "RMON_SNMP");
    }

    if (ownername)
    {
        snprintf(rmon_event->eventOwner, RMON_OWNER_NAME_SIZE, "%s", ownername);
    }
    else
    {
        snprintf(rmon_event->eventOwner, RMON_OWNER_NAME_SIZE, "%s", "RMON_SNMP");
    }

    rmon_event->eventStatus = VALID_STATUS;

    return RESULT_OK;
}

s_int32_t rmon_event_entry_remove(u_int32_t index)
{
    struct rmon_EventGroup *rmon_event = NULL;
    struct prefix p;
    struct route_node *rn;

    if (NULL == g_rm || NULL == g_rm->event_table)
    {
        return RESULT_ERROR;
    }

    /*if alarmGroup use this eventGroup */
    if (rmon_check_event_group_in_alarm_group(index) == RMON_API_SET_SUCCESS)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->event_table, &p);

    if (rn == NULL)
    {
        return RESULT_ERROR;
    }

    rmon_event = rn->info;

    RMON_INFO_UNSET(rn);

    /* To unlock the read lock set in route_node_lookup */
    route_unlock_node(rn);

    /* To unlock the read lock set in route_node_get  */
    route_unlock_node(rn);

    XFREE(MTYPE_RMON, rmon_event);

    return RESULT_OK;
}

s_int32_t rmon_event_snmp_update_status(u_int32_t index, u_int32_t status)
{
    struct prefix p;
    struct route_node *rn = NULL;
    struct rmon_EventGroup *rmon_event = NULL;

    if (NULL == g_rm || NULL == g_rm->event_table)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);

    switch (status)
    {
        case VALID_STATUS:
            rmon_event = rmon_event_entry_info_lookup(index);

            if (NULL != rmon_event)
            {
                rmon_event->eventStatus = VALID_STATUS;
                return RESULT_OK;
            }
            else
            {
                return RESULT_ERROR;
            }

        case CREATE_REQ_STATUS:
            rn = route_node_get(g_rm->event_table, &p);

            if (rn == NULL)
            {
                return RESULT_ERROR;
            }

            if (rn->info == NULL)
            {
                rmon_event = XCALLOC(MTYPE_RMON, sizeof(struct rmon_EventGroup));

                if (NULL == rmon_event)
                {
                    return RESULT_ERROR;
                }

                memset(rmon_event, 0, sizeof(struct rmon_EventGroup));
                rmon_event->eventIndex = index;
                rmon_event->eventStatus = UNDER_CREATION_STATUS;

                RMON_INFO_SET(rn, rmon_event);
            }
            else
            {
                route_unlock_node(rn);
                rmon_event->eventStatus = CREATE_REQ_STATUS;
            }

            return RESULT_OK;

        case UNDER_CREATION_STATUS:
            return RESULT_OK;

        case INVALID_STATUS:
            return rmon_event_entry_remove(index);

        default :
            return RESULT_ERROR;
    }

    return RESULT_ERROR;
}

s_int32_t rmon_alarm_entry_status(u_int32_t index)
{
    struct prefix p;
    struct route_node *rn;

    if (NULL == g_rm || NULL == g_rm->alarm_table)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->alarm_table, &p);

    if (NULL != rn)
    {
        route_unlock_node(rn);
        return RESULT_OK;
    }

    return RESULT_ERROR;
}

struct rmon_AlarmGroup *rmon_alarm_entry_info_lookup(u_int32_t index)
{
    struct route_node *rn = NULL;
    struct prefix p;
    struct rmon_AlarmGroup *rmon_alarm = NULL;

    if (NULL == g_rm || NULL == g_rm->alarm_table)
    {
        return NULL;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->alarm_table, &p);

    if (NULL == rn)
    {
        return NULL;
    }

    route_unlock_node(rn);
    rmon_alarm = (struct rmon_AlarmGroup *)rn->info;

    return rmon_alarm;
}

s_int32_t
rmon_set_alarm_variableword(struct rmon_AlarmGroup *rmon_alarm , int ether_stats_var)
{
    if (!rmon_alarm)
    {
        return RMON_API_SET_FAILURE;
    }

    memset(rmon_alarm->alarmVariableWord, 0x00 , (RMON_ALARM_VAR_WORD_LENGTH + 1));

    switch (ether_stats_var)
    {
        case ETHERSTATSINDEX:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSINDEXSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSDATASOURCE:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSDSRCSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSDROPEVENTS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSDROPEVENTSVSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSOCTETS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSOCTETSSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSPKTS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSPKTSSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSBROADCASTPKTS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSBDCPKTSSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSMULTICASTPKTS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSMCPKTSSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSCRCALIGNERRORS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSCRCALIGNERRSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSUNDERSIZEPKTS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSUSZPKTSSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSOVERSIZEPKTS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSOVSZPKTSSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSFRAGMENTS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSFRGSSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSJABBERS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSJABBERSSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSCOLLISIONS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSCOLLISIONSSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSPKTS64OCTETS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSPKT64OCTSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSPKTS65TO127OCTETS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSPKT65TO127OCTSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSPKTS128TO255OCTETS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSPKT128TO255OCTSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSPKTS256TO511OCTETS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSPKT256TO511OCTSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSPKTS512TO1023OCTETS:
            memcpy(rmon_alarm->alarmVariableWord ,
                   ETHERSTATSPKT512TO1023OCTSTR , RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSPKTS1024TO1518OCTETS:
            memcpy(rmon_alarm->alarmVariableWord ,
                   ETHERSTATSPKT1024TO15180OCTSTR, RMON_ALARM_VAR_WORD_LENGTH);
            break;

        /* liuyf add for extern rmon statistic group */
        case ETHERSTATSUNICASTPKTS:
            memcpy(rmon_alarm->alarmVariableWord ,
                   ETHERSTATSUNICASTPKTSSTR, RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSBADPKTS:
            memcpy(rmon_alarm->alarmVariableWord ,
                   ETHERSTATSBADPKTSSTR, RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSINDISCARDPKTS:
            memcpy(rmon_alarm->alarmVariableWord ,
                   ETHERSTATSINDISCARDPKTSSTR, RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSSENTOCTETS:
            memcpy(rmon_alarm->alarmVariableWord ,
                   ETHERSTATSSENTOCTETSSTR, RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSSENTPKTS:
            memcpy(rmon_alarm->alarmVariableWord ,
                   ETHERSTATSSENTPKTSSTR, RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSSENTUNICASTPKTS:
            memcpy(rmon_alarm->alarmVariableWord ,
                   ETHERSTATSSENTUNICASTPKTSSTR, RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSSENTBROADCASTPKTS:
            memcpy(rmon_alarm->alarmVariableWord ,
                   ETHERSTATSSENTBROADCASTPKTSSTR, RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSSENTMULTICASTPKTS:
            memcpy(rmon_alarm->alarmVariableWord ,
                   ETHERSTATSSENTMULTICASTPKTSSTR, RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSSENTBADPKTS:
            memcpy(rmon_alarm->alarmVariableWord ,
                   ETHERSTATSSENTBADPKTSSTR, RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSSENTDISCARDPKTS:
            memcpy(rmon_alarm->alarmVariableWord ,
                   ETHERSTATSSENTDISCARDPKTSSTR, RMON_ALARM_VAR_WORD_LENGTH);
            break;

        /* liuyf add end */
        case ETHERSTATSOWNER:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSOWNERSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        case ETHERSTATSSTATUS:
            memcpy(rmon_alarm->alarmVariableWord , ETHERSTATSSTATUSSTR,
                   RMON_ALARM_VAR_WORD_LENGTH);
            break;

        default :
            return RMON_API_SET_FAILURE;

    }

    return RMON_API_SET_SUCCESS;
}


s_int32_t rmon_alarm_entry_add(u_int32_t index, int type, char *type_name, int stats_index,
                               u_int32_t interval, u_int32_t alarmSampleType,
                               s_int32_t RisingValue,
                               s_int32_t FallingValue, u_int32_t rising_event,
                               u_int32_t falling_event, char *ownername,
                               u_int8_t AlarmStartUpAlarm)
{
    struct rmon_AlarmGroup *rmon_alarm;
    struct prefix p;
    struct route_node *rn;
    oid oidval[RMON_ALARM_VARIABLE_MAXSIZE] = {1, 3, 6, 1, 2, 1, 16, 1, 1, 1, 0, 0};

    char alarmVariableWordbuf[ETHER_STATS_BUF_MAX];

    if (NULL == g_rm || NULL == g_rm->alarm_table || NULL == type_name || 0 == type || 0 == stats_index)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_get(g_rm->alarm_table, &p);

    if (NULL == rn)
    {
        return RESULT_ERROR;
    }

    rmon_alarm = rn->info;

    if (NULL == rmon_alarm)
    {
        rmon_alarm = XCALLOC(MTYPE_RMON, sizeof(struct rmon_AlarmGroup));

        if (NULL == rmon_alarm)
        {
            return RESULT_ERROR;
        }

        memset(rmon_alarm, 0, sizeof(struct rmon_AlarmGroup));

        RMON_INFO_SET(rn, rmon_alarm);
    }
    else
    {
        route_unlock_node(rn);
    }

    rmon_alarm->alarmIndex = index;
    rmon_alarm->alarmStartupAlarm = AlarmStartUpAlarm;
    rmon_alarm->alarmInterval = interval;
    rmon_alarm->alarmSampleType = alarmSampleType;
    rmon_alarm->alarmRisingThreshold.l[0] = 0;
    rmon_alarm->alarmRisingThreshold.l[1] = RisingValue;
    rmon_alarm->alarmRisingEventIndex = rising_event;
    rmon_alarm->alarmFallingThreshold.l[0] = 0;
    rmon_alarm->alarmFallingThreshold.l[1] = FallingValue;
    rmon_alarm->alarmFallingEventIndex = falling_event;
    rmon_alarm->alarm_last_event = RMON_NO_ALARM;
    rmon_alarm->alarmStatus = CREATE_REQ_STATUS;

    oidval[RMOB_ALARM_OID_INDEX_TYPE]  = type;
    oidval[RMOB_ALARM_OID_INDEX_STATS] = stats_index;
    memcpy(rmon_alarm->alarmVariable, oidval, RMON_ALARM_VARIABLE_MAXSIZE * sizeof(oid));
    rmon_set_alarm_variableword(rmon_alarm , type);
    sprintf(alarmVariableWordbuf, "%s.%d", rmon_alarm->alarmVariableWord, stats_index);
    memcpy(rmon_alarm->alarmVariableWord , alarmVariableWordbuf,
           RMON_ALARM_VAR_WORD_LENGTH);

    if (ownername)
    {
        snprintf(rmon_alarm->alarmOwner, RMON_OWNER_NAME_SIZE, "%s", ownername);
        SET_FLAG(rmon_alarm->alarmConf, RMON_ALARM_OWNER_CONF);
    }
    else
    {
        snprintf(rmon_alarm->alarmOwner, RMON_OWNER_NAME_SIZE, "%s", "RMON_SNMP");
    }

    rmon_alarm_entry_active(rmon_alarm);

    return RESULT_OK;
}

s_int32_t rmon_alarm_entry_active(struct rmon_AlarmGroup *rmon_alarm)
{
    oid                     alarmVariable[RMON_ALARM_VARIABLE_MAXSIZE];
    struct thread_master   *thread_rmon = (struct thread_master *)get_rmond_thread_master();

    if (NULL == rmon_alarm || NULL == thread_rmon)
    {
        return RESULT_ERROR;
    }

    if (0 == rmon_alarm->alarmSampleType)
    {
        return RESULT_ERROR;
    }

    if (0 == rmon_alarm->alarmInterval)
    {
        return RESULT_ERROR;
    }

    memset(alarmVariable, 0, sizeof(RMON_ALARM_VARIABLE_MAXSIZE));

#if 0

    if (memcmp(alarm_counter, rmon_alarm->alarmVariable, RMON_ALARM_VARIABLE_MAXSIZE) == 0)
    {
        return RESULT_ERROR;
    }

#endif
    rmon_alarm->alarmStatus = VALID_STATUS;

    //RMON_TIMER_OFF(rmon_alarm->alarm_timer);
    //rmon_alarm->alarm_timer = thread_add_timer(thread_rmon, \
    //                          rmon_alarm_timer_handler, \
    //                          rmon_alarm, \
    //                          rmon_alarm->alarmInterval);

	high_pre_timer_delete(rmon_alarm->alarm_timer);
	rmon_alarm->alarm_timer = high_pre_timer_add("rmonAlarmTimer", \
								LIB_TIMER_TYPE_LOOP, \
								rmon_alarm_timer_handler, \
								rmon_alarm, \
                              	rmon_alarm->alarmInterval*1000);

    return RESULT_OK;
}

s_int32_t rmon_alarm_entry_inactive(struct rmon_AlarmGroup *rmon_alarm)
{
    if (NULL == rmon_alarm)
    {
        return RESULT_ERROR;
    }

    /* stop the running timer */
    rmon_alarm->alarmStatus = INVALID_STATUS;
    rmon_alarm->alarm_startup_alarm_status = RMON_ALARM_STARTUP_NOT_MET;
    //RMON_TIMER_OFF(rmon_alarm->alarm_timer);
	high_pre_timer_delete(rmon_alarm->alarm_timer);
	rmon_alarm->alarm_timer = NULL;

    return RESULT_OK;
}

s_int32_t rmon_alarm_index_remove(u_int32_t index)
{
    struct rmon_AlarmGroup *rmon_alarm;
    struct prefix p;
    struct route_node *rn;

    if (NULL == g_rm || NULL == g_rm->alarm_table)
    {
        return RESULT_ERROR;
    }

    PREFIX_RMON_SET(&p, index);
    rn = route_node_lookup(g_rm->alarm_table, &p);

    if (rn == NULL)
    {
        return RESULT_ERROR;
    }

    rmon_alarm = rn->info;
    /* stop the running timer */
    rmon_alarm_entry_inactive(rmon_alarm);
    RMON_INFO_UNSET(rn);
    /* To unlock the read lock set in route_node_lookup */
    route_unlock_node(rn);
    /* To unlock the read lock set in route_node_get  */
    route_unlock_node(rn);
    XFREE(MTYPE_RMON, rmon_alarm);
    return RESULT_OK;
}

int rmon_alarm_get_stats_type(char *type_name)
{
    if (NULL == type_name)
    {
        return 0;
    }

    if ((strlen(type_name) == strlen(ETHERSTATSINDEXSTR)) && \
		 (0 == strcmp(type_name, ETHERSTATSINDEXSTR)))
    {
        return ETHERSTATSINDEX;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSDSRCSTR)) && \
			(0 == strcmp(type_name, ETHERSTATSDSRCSTR)))
    {
        return ETHERSTATSDATASOURCE;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSDROPEVENTSVSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSDROPEVENTSVSTR)))
    {
        return ETHERSTATSDROPEVENTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSOCTETSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSOCTETSSTR)))
    {
        return ETHERSTATSOCTETS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSPKTSSTR)))
    {
        return ETHERSTATSPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSBDCPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSBDCPKTSSTR)))
    {
        return ETHERSTATSBROADCASTPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSMCPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSMCPKTSSTR)))
    {
        return ETHERSTATSMULTICASTPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSCRCALIGNERRSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSCRCALIGNERRSTR)))
    {
        return ETHERSTATSCRCALIGNERRORS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSUSZPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSUSZPKTSSTR)))
    {
        return ETHERSTATSUNDERSIZEPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSOVSZPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSOVSZPKTSSTR)))
    {
        return ETHERSTATSOVERSIZEPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSFRGSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSFRGSSTR)))
    {
        return ETHERSTATSFRAGMENTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSJABBERSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSJABBERSSTR)))
    {
        return ETHERSTATSJABBERS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSCOLLISIONSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSCOLLISIONSSTR)))
    {
        return ETHERSTATSCOLLISIONS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSPKT64OCTSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSPKT64OCTSTR)))
    {
        return ETHERSTATSPKTS64OCTETS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSPKT65TO127OCTSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSPKT65TO127OCTSTR)))
    {
        return ETHERSTATSPKTS65TO127OCTETS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSPKT128TO255OCTSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSPKT128TO255OCTSTR)))
    {
        return ETHERSTATSPKTS128TO255OCTETS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSPKT256TO511OCTSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSPKT256TO511OCTSTR)))
    {
        return ETHERSTATSPKTS256TO511OCTETS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSPKT512TO1023OCTSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSPKT512TO1023OCTSTR)))
    {
        return ETHERSTATSPKTS512TO1023OCTETS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSPKT1024TO15180OCTSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSPKT1024TO15180OCTSTR)))
    {
        return ETHERSTATSPKTS1024TO1518OCTETS;
    }
    /* liuyf add for extern rmon statistic group */
    else if ((strlen(type_name) == strlen(ETHERSTATSUNICASTPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSUNICASTPKTSSTR)))
    {
        return ETHERSTATSUNICASTPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSBADPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSBADPKTSSTR)))
    {
        return ETHERSTATSBADPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSINDISCARDPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSINDISCARDPKTSSTR)))
    {
        return ETHERSTATSINDISCARDPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSSENTOCTETSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSSENTOCTETSSTR)))
    {
        return ETHERSTATSSENTOCTETS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSSENTPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSSENTPKTSSTR)))
    {
        return ETHERSTATSSENTPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSSENTUNICASTPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSSENTUNICASTPKTSSTR)))
    {
        return ETHERSTATSSENTUNICASTPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSSENTBROADCASTPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSSENTBROADCASTPKTSSTR)))
    {
        return ETHERSTATSSENTBROADCASTPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSSENTMULTICASTPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSSENTMULTICASTPKTSSTR)))
    {
        return ETHERSTATSSENTMULTICASTPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSSENTBADPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSSENTBADPKTSSTR)))
    {
        return ETHERSTATSSENTBADPKTS;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSSENTDISCARDPKTSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSSENTDISCARDPKTSSTR)))
    {
        return ETHERSTATSSENTDISCARDPKTS;
    }
    /* liuyf add end */
    else if ((strlen(type_name) == strlen(ETHERSTATSOWNERSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSOWNERSTR)))
    {
        return ETHERSTATSOWNER;
    }
    else if ((strlen(type_name) == strlen(ETHERSTATSSTATUSSTR)) && \
			 (0 == strcmp(type_name, ETHERSTATSSTATUSSTR)))
    {
        return ETHERSTATSSTATUS;
    }

    return 0;
}


int register_if_msg()
{
//    ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_RMON, IFNET_IFTYPE_PORT);
//    ifm_event_register(IFNET_EVENT_UP, MODULE_ID_RMON, IFNET_IFTYPE_PORT);
    return RESULT_OK;
}

int rmon_if_up(uint32_t ifindex)
{
    return RESULT_OK;
}

int rmon_if_delete(uint32_t ifindex)
{
    struct route_node *rn = NULL;
    struct prefix p;
    struct rmon_interface *pRmon_interface = NULL;

    if (NULL == g_rm)
    {
        return RESULT_ERROR;
    }

    memset(&p, 0, sizeof(struct prefix));
    PREFIX_RMON_SET(&p, ifindex);

    if (g_rm->interface_table == NULL)
    {
        return RESULT_ERROR;
    }

    rn = route_node_lookup(g_rm->interface_table, &p);

    if (NULL != rn)
    {
        pRmon_interface = rn->info;
        RMON_INFO_UNSET(rn);

        /* To unlock the read lock set in route_node_lookup */
        route_unlock_node(rn);

        zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d]:'%s': ifindex [%8x] \r\n", __FILE__, __LINE__, __func__, ifindex);

        if (NULL != pRmon_interface)
        {
            XFREE(MTYPE_RMON, pRmon_interface);
        }
    }
    else
    {
        return RESULT_ERROR;
    }

    for (rn = route_top(g_rm->etherStats_table); rn; rn = route_next(rn))
    {
        struct rmon_etherStatsGroup *ether_stats ;
        ether_stats = rn->info;

        if (ether_stats &&
                ether_stats->etherStatsDataSource[RMON_IFINDEX_LOC] == ifindex)
        {
            zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d]:'%s': ether_stats->etherStatsIndex [%8x] \r\n", __FILE__, __LINE__, __func__, ether_stats->etherStatsIndex);
            rmon_coll_stats_entry_remove_force(ether_stats->etherStatsIndex);
        }
    }

    for (rn = route_top(g_rm->historyControl_table); rn; rn = route_next(rn))
    {
        struct rmon_HistoryControlGroup *rinfo ;
        rinfo = (struct rmon_HistoryControlGroup *)rn->info;

        if (NULL == rinfo)
        {
            continue ;
        }

        if (ifindex != rinfo->historyControlDataSource[RMON_IFINDEX_LOC])
        {
            continue ;
        }

        zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d]:'%s': rinfo->historyControlIndex [%8x] \r\n", __FILE__, __LINE__, __func__, rinfo->historyControlIndex);
        rmon_coll_history_entry_remove(rinfo->historyControlIndex);
    }

    return RESULT_OK;
}

int rmon_snmp_etherStatsTable_get_bulk(struct ipc_msghdr_n *phdr, u_int32_t *stat_Index)
{
    int data_num = 0;
    int matched = -1 ;
    u_int32_t ifindex = 0;
    struct rmon_if_stats if_stats;
    int max_msg_num  = IPC_MSG_LEN / sizeof(struct rmon_etherStatsGroup);

    struct route_node *rn;
    struct rmon_etherStatsGroup rinfo;

    struct rmon_etherStatsGroup hist_ctr_buff[max_msg_num];
    memset(hist_ctr_buff, 0, max_msg_num * sizeof(struct rmon_etherStatsGroup));
    memset(&if_stats, 0, sizeof(struct rmon_if_stats));

    for (rn = route_top(g_rm->etherStats_table); rn; rn = route_next(rn))
    {
        if (NULL == rn->info)
        {
            continue ;
        }

        memcpy(&rinfo , rn->info , sizeof(struct rmon_etherStatsGroup));

        if (*stat_Index == 0)
        {
            matched = 0;
        }

        if (*stat_Index == rinfo.etherStatsIndex)
        {
            matched = 0;
            continue;
        }

        if (matched == 0)
        {
            ifindex =  rinfo.etherStatsDataSource[RMON_IFINDEX_LOC];

            if (VALID_STATUS == rinfo.etherStatsStatus)
            {
                if (data_num >= max_msg_num)
                {
                    break ;
                }

                rmon_interface_counter_curr_increment(ifindex, &if_stats);
                rmon_if_stats_2_rmon_etherStatsGroup(&if_stats, &rinfo);
                memcpy(&hist_ctr_buff[data_num], &rinfo, sizeof(struct rmon_etherStatsGroup));
                data_num++ ;
            }
        }
    }


    if (data_num > 0)
    {
        ipc_send_reply_n2((uchar *)hist_ctr_buff, data_num * sizeof(struct rmon_etherStatsGroup),
                            data_num, phdr->sender_id,
                            phdr->module_id,
                            phdr->msg_type, phdr->msg_subtype, 0,phdr->msg_index, IPC_OPCODE_REPLY) ;
    }
    else
    {
        ipc_send_reply_n2(NULL,0 ,0, phdr->sender_id, phdr->module_id, phdr->msg_type,
                       phdr->msg_subtype, 0, phdr->msg_index, IPC_OPCODE_NACK);
    }


    return (RESULT_OK);
}

int rmon_snmp_historyControlTable_get_bulk(struct ipc_msghdr_n *phdr, u_int32_t *ctr_Index)
{
    int data_num = 0;
    int matched = -1 ;
    int max_msg_num  = IPC_MSG_LEN / sizeof(struct rmon_HistoryControlGroup);

    struct route_node *rn;


    struct rmon_HistoryControlGroup hist_ctr_buff[max_msg_num];
    memset(hist_ctr_buff, 0, max_msg_num * sizeof(struct rmon_HistoryControlGroup));

    for (rn = route_top(g_rm->historyControl_table); rn; rn = route_next(rn))
    {
        struct rmon_HistoryControlGroup *rinfo;
        rinfo = (struct rmon_HistoryControlGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue ;
        }

        if (*ctr_Index == 0)
        {
            matched = 0 ;
        }

        if (*ctr_Index  == rinfo->historyControlIndex)
        {
            matched = 0 ;
            continue ;
        }

        if (matched == 0)
        {
            if (data_num >= max_msg_num)
            {
                break ;
            }

            memcpy(&hist_ctr_buff[data_num++], rinfo, sizeof(struct rmon_HistoryControlGroup));
        }
    }
    if (data_num > 0)
    {
        ipc_send_reply_n2((uchar *)hist_ctr_buff, data_num * sizeof(struct rmon_HistoryControlGroup),
                            data_num, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype,  0,phdr->msg_index, IPC_OPCODE_REPLY) ;
    }
    else
    {
        ipc_send_reply_n2(NULL,0 ,0, phdr->sender_id, phdr->module_id, phdr->msg_type,
                       phdr->msg_subtype,  0,phdr->msg_index, IPC_OPCODE_NACK);
    }

    return (RESULT_OK);
}

int history_sample_cmp(struct rmon_EtherHistoryGroup *current_sample_no1 ,  struct rmon_EtherHistoryGroup *current_sample_no2)
{
    if(NULL == current_sample_no1)
       {
           if(NULL == current_sample_no2)
               return 0;
           else
               return -1;
       }
       else
       {
           if(NULL == current_sample_no2)
               return 1;
           else
           {
               if(current_sample_no1->etherHistorySampleIndex > current_sample_no2->etherHistorySampleIndex)
                   return 1;
               else if(current_sample_no1->etherHistorySampleIndex == current_sample_no2->etherHistorySampleIndex)
                   return 0;
               else
                   return -1;
           }
       }

}

int rmon_snmp_etherHistoryTable_get_bulk(struct ipc_msghdr_n *phdr, u_int32_t *sample_Index)
{
    int data_num = 0;
    int max_msg_num  = IPC_MSG_LEN / sizeof(struct rmon_EtherHistoryGroup);
    struct listnode  *node, *nnode;
    struct rmon_EtherHistoryGroup *data1_find;

    int matched = -1 ;
    u_int32_t his_ctr_Index = phdr->msg_index;
    struct route_node *rn;

    u_int32_t start_sampe = *sample_Index ;

    struct route_node *rn_data;
    struct rmon_EtherHistoryGroup *rinfo_data;

    struct rmon_EtherHistoryGroup hist_ctr_buff[max_msg_num];
    memset(hist_ctr_buff, 0, max_msg_num * sizeof(struct rmon_EtherHistoryGroup));

    struct list *plist = list_new();
    plist->cmp = (int (*) (void *, void *))history_sample_cmp;


    zlog_debug(ZLOG_LIB_DBG_PKG, "%s[%d]:'%s': rinfo->historyControlIndex [%8x] sampleNo [%d] \r\n",
                            __FILE__, __LINE__, __func__, his_ctr_Index , start_sampe);

    for (rn = route_top(g_rm->historyControl_table); rn; rn = route_next(rn))
    {
        struct rmon_HistoryControlGroup *rinfo;
        rinfo = (struct rmon_HistoryControlGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue ;
        }

        if (his_ctr_Index == 0)
        {
            matched = 0 ;
            start_sampe = 0 ;
        }

        if (his_ctr_Index <= rinfo->historyControlIndex)
        {
            matched = 0 ;
            //continue ;
        }

        if (his_ctr_Index < rinfo->historyControlIndex)
        {
            matched = 0 ;
            start_sampe = 0 ;
        }

        if (matched == 0)
        {
// 1:copy historyStats_table to a list
            list_delete_all_node(plist);
            for (rn_data = route_top(rinfo->historyStats_table); rn_data; rn_data = route_next(rn_data))
            {
                rinfo_data = (struct rmon_EtherHistoryGroup *)rn_data->info;

                if (rinfo_data == NULL)
                {
                    continue ;
                }

                listnode_add_sort(plist , rinfo_data);
            }

//3:find start sample
             for (ALL_LIST_ELEMENTS(plist, node, nnode, data1_find))
             {
                    if (start_sampe == 0 || start_sampe < data1_find->etherHistorySampleIndex)
                    {
 //4:copy match data to return array
                         if (data_num >= max_msg_num)
                         {
                             break ;
                         }

                        memcpy(&hist_ctr_buff[data_num++], data1_find, sizeof(struct rmon_EtherHistoryGroup));
                    }
             }
        }
    }
 //5:free list
    list_delete_all_node(plist);
    list_free(plist);

    if (data_num > 0)
    {
        ipc_send_reply_n2((uchar *)hist_ctr_buff, data_num * sizeof(struct rmon_EtherHistoryGroup),
                            data_num, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0,phdr->msg_index, IPC_OPCODE_REPLY) ;
    }
    else
    {
        ipc_send_reply_n2(NULL,0 ,0, phdr->sender_id, phdr->module_id, phdr->msg_type,
                       phdr->msg_subtype, 0,phdr->msg_index, IPC_OPCODE_NACK);
    }

    return (RESULT_OK);
}

int rmon_snmp_alarmTable_get_bulk(struct ipc_msghdr_n *phdr, u_int32_t *alarm_Index)
{
    int data_num = 0;
    int matched = -1 ;

    int max_msg_num  = IPC_MSG_LEN / sizeof(struct rmon_AlarmGroup);

    struct route_node *rn;

    struct rmon_AlarmGroup hist_ctr_buff[max_msg_num];
    memset(hist_ctr_buff, 0, max_msg_num * sizeof(struct rmon_AlarmGroup));

    for (rn = route_top(g_rm->alarm_table); rn; rn = route_next(rn))
    {
        struct rmon_AlarmGroup *rinfo;
        rinfo = (struct rmon_AlarmGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue ;
        }

        if (*alarm_Index == 0)
        {
            matched = 0 ;
        }

        if (*alarm_Index == rinfo->alarmIndex)
        {
            matched = 0 ;
            continue ;
        }

        if (matched == 0)
        {
            if (data_num >= max_msg_num)
            {
                break ;
            }

            memcpy(&hist_ctr_buff[data_num++], rinfo, sizeof(struct rmon_AlarmGroup)) ;
        }
    }

    if (data_num > 0)
    {
        ipc_send_reply_n2((uchar *)hist_ctr_buff, data_num * sizeof(struct rmon_AlarmGroup),
                            data_num, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0,phdr->msg_index, IPC_OPCODE_REPLY) ;
    }
    else
    {
        ipc_send_reply_n2(NULL,0 ,0,phdr->sender_id, phdr->module_id, phdr->msg_type,
                       phdr->msg_subtype, 0,phdr->msg_index, IPC_OPCODE_NACK);
    }

    return (RESULT_OK);
}

int rmon_snmp_eventTable_get_bulk(struct ipc_msghdr_n *phdr, u_int32_t *event_Index)
{
    int data_num = 0;
    int matched = -1 ;

    int max_msg_num  = IPC_MSG_LEN / sizeof(struct rmon_EventGroup);

    struct route_node *rn;
    struct rmon_EventGroup hist_ctr_buff[max_msg_num];

    memset(hist_ctr_buff, 0, max_msg_num * sizeof(struct rmon_EventGroup));

    for (rn = route_top(g_rm->event_table); rn; rn = route_next(rn))
    {
        struct rmon_EventGroup *rinfo;
        rinfo = (struct rmon_EventGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue ;
        }

        if (*event_Index == 0)
        {
            matched = 0 ;
        }

        if (*event_Index == rinfo->eventIndex)
        {
            matched = 0 ;
            continue ;
        }

        if (matched == 0)
        {
            if (data_num >= max_msg_num)
            {
                break ;
            }

            memcpy(&hist_ctr_buff[data_num++], rinfo, sizeof(struct rmon_EventGroup));
        }
    }

    if (data_num > 0)
    {
        ipc_send_reply_n2((uchar *)hist_ctr_buff, data_num * sizeof(struct rmon_EventGroup),
                            data_num, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype,0, phdr->msg_index, IPC_OPCODE_REPLY) ;
    }
    else
    {
        ipc_send_reply_n2(NULL,0 ,0, phdr->sender_id, phdr->module_id, phdr->msg_type,
                       phdr->msg_subtype,0, phdr->msg_index, IPC_OPCODE_NACK);
    }

    return (RESULT_OK);
}

void rmon_snmp_table_info_get(struct ipc_mesg_n *pmsg)
{
    u_int32_t *index_num = ((u_int32_t *)pmsg->msg_data) ;

    switch (pmsg->msghdr.msg_subtype)
    {
        case RMON_etherStatsTable:
            rmon_snmp_etherStatsTable_get_bulk(&pmsg->msghdr, index_num);
            break;

        case RMON_historyControlTable:
            rmon_snmp_historyControlTable_get_bulk(&pmsg->msghdr, index_num);
            break;

        case RMON_etherHistoryTable:
            rmon_snmp_etherHistoryTable_get_bulk(&pmsg->msghdr, index_num);
            break;

        case RMON_alarmTable:
            rmon_snmp_alarmTable_get_bulk(&pmsg->msghdr, index_num);
            break;

        case RMON_eventTable:
            rmon_snmp_eventTable_get_bulk(&pmsg->msghdr, index_num);
            break;

        default:
            break;
    }
}

int rmon_handle_snmp_msg(struct ipc_mesg_n *pmsg)
{
    switch (pmsg->msghdr.opcode)
    {
        case IPC_OPCODE_GET_BULK:
            rmon_snmp_table_info_get(pmsg);
            break;

        default:
            break;
    }

    return (RESULT_OK);
}

/*liuyf add for check statsGroup and alarmGroup relationship  2015-03-31*/
s_int32_t
rmon_check_stats_group_in_alarm_group(u_int32_t etherStatsIndex)
{
    struct route_node *rn = NULL;

    if (g_rm == NULL)
    {
        return RESULT_ERROR;
    }

    if (g_rm->alarm_table == NULL)
    {
        return RESULT_ERROR;
    }

    for (rn = route_top(g_rm->alarm_table); rn; rn = route_next(rn))
    {
        struct rmon_AlarmGroup *rinfo = NULL;

        rinfo = (struct rmon_AlarmGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue;
        }

        if (etherStatsIndex == rinfo->alarmVariable[11])
        {
            return RESULT_OK;
        }
    }

    return RESULT_ERROR;
}

s_int32_t
rmon_check_event_group_in_alarm_group(u_int32_t eventIndex)
{
    struct route_node *rn = NULL;

    if (g_rm == NULL)
    {
        return RESULT_ERROR;
    }

    if (g_rm->alarm_table == NULL)
    {
        return RESULT_ERROR;
    }

    for (rn = route_top(g_rm->alarm_table); rn; rn = route_next(rn))
    {
        struct rmon_AlarmGroup *rinfo = NULL;
        rinfo = (struct rmon_AlarmGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue;
        }


        if ((eventIndex == rinfo->alarmRisingEventIndex) || \
                (eventIndex == rinfo->alarmFallingEventIndex))
        {
            return RESULT_OK;
        }
    }

    return RESULT_ERROR;
}
/*liuyf add end for check eventGroup and alarmGroup relationship 2015-03-31*/

struct rmon_etherStatsGroup *
rmon_ether_stats_lookup_by_id(u_int32_t *index, int exact)
{
    struct route_node *rn = NULL;
    struct rmon_etherStatsGroup *rmon_ether_stats = NULL;
    struct rmon_etherStatsGroup *rinfo = NULL;
    struct prefix p;

    if (!g_rm)
    {
        return NULL;
    }

    if (g_rm->etherStats_table == NULL)
    {
        return NULL;
    }

    PREFIX_RMON_SET(&p, *index);

    if (exact)
    {
        rn = route_node_lookup(g_rm->etherStats_table, &p);

        if (rn)
        {
            rmon_ether_stats = (struct rmon_etherStatsGroup *)rn->info;
            route_unlock_node(rn);

            return rmon_ether_stats;
        }
    }
    else
    {
        for (rn = route_top(g_rm->etherStats_table); rn; rn = route_next(rn))
        {
            rinfo = (struct rmon_etherStatsGroup *)rn->info;

            if (rinfo == NULL)
            {
                continue;
            }

            if (*index < rinfo->etherStatsIndex)
            {
                if (rmon_ether_stats == NULL)
                {
                    rmon_ether_stats = rinfo;
                }
                else if (rinfo->etherStatsIndex < rmon_ether_stats->etherStatsIndex)
                {
                    rmon_ether_stats = rinfo;
                }
            }
        }

        if (rmon_ether_stats)
        {
            *index = rmon_ether_stats->etherStatsIndex;
        }
        else
        {
            *index = 0;
        }

        return rmon_ether_stats;
    }

    return NULL;
}

int rmon_api_clear_all_counters()
{
    struct route_node *rn;
    struct rmon_interface *rinfo;

    for (rn = route_top(g_rm->interface_table); rn; rn = route_next(rn))
    {
        struct rmon_if_stats curr_data ;
        rinfo = (struct rmon_interface *)rn->info;

        if (rinfo == NULL)
        {
            continue;
        }

        memset(&rinfo->if_init_cntrs, 0 , sizeof(struct rmon_if_stats));
        rmon_get_counter_from_sdk(rinfo->ifindex, &curr_data);
        memcpy(&rinfo->if_init_cntrs, &curr_data, sizeof(struct rmon_if_stats));
        memcpy(&rinfo->if_hist_cntrs, &curr_data, sizeof(struct rmon_if_stats));
    }

    return RESULT_OK;
}


