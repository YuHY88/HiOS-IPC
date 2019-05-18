/******************************************************************************
 * Filename: rmond_timer.c
 *  Copyright (c) 2016-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: rmond_timer functions for rmond.
 *
 * History:
 *  *2017.10.23  lidingcheng created
 *
******************************************************************************/

#include "memory.h"
#include "time.h"
#include "prefix.h"
#include <table.h>
#include "lib/memshare.h"
#include <lib/rmon_common.h>
#include <lib/thread.h>

#include <lib/msg_ipc_n.h>


#include <sys/sysinfo.h>

#include "rmon_config.h"
#include "rmond_main.h"
#include "rmon_api.h"
#include "rmon_timer.h"
#include "lib/log.h"

void rmon_trap(struct rmon_AlarmGroup *alarm, u_int32_t rise_or_fall)
{
    alarm->raise_or_fall = rise_or_fall;
#if 0
    ipc_send_common(alarm, sizeof(struct rmon_AlarmGroup), 1,
                    MODULE_ID_SNMPD, MODULE_ID_RMON,
                    IPC_TYPE_RMOND, 0, IPC_OPCODE_EVENT);
#else if
    int ret = ipc_send_msg_n2(alarm, sizeof (struct rmon_AlarmGroup), 1,
		MODULE_ID_SNMPD_TRAP, MODULE_ID_RMON, IPC_TYPE_RMOND, 0, IPC_OPCODE_EVENT,  0);

    if(ret < 0)
    {
        zlog_err("%s[%d] : rmon send trap to snmpd error\n", __func__, __LINE__);
    }
#endif

}

static s_int32_t
find_prev_sample_no(struct rmon_HistoryControlGroup *rmon_history_ctrl_grp,
                    u_int32_t current_sample_no)
{
    struct prefix prev;


    if (!rmon_history_ctrl_grp)
    {
        return 0;
    }

    /* Samples are circular. If curren_sample_no is 0, prev_sample is the
     * last sample. For first time creation it will be -1
     */

    if (current_sample_no == 1)
    {
        struct route_node *rn_prev;

        PREFIX_RMON_SET(&prev, rmon_history_ctrl_grp->historyControlBucketsGranted);
        rn_prev = route_node_lookup
                  (rmon_history_ctrl_grp->historyStats_table,
                   (struct prefix *) & prev);

        if (!rn_prev)
        {
            return -1;
        }
        else
        {
            return (rmon_history_ctrl_grp->historyControlBucketsGranted);
        }
    }
    else
    {
        return current_sample_no--;
    }

    return 0;
}

u_int32_t rmon_time_since_boot(void)
{
    struct sysinfo info;

    sysinfo(&info);

    return info.uptime;
}

static int
rmon_store_sample_stats(struct rmon_HistoryControlGroup *rmon_history_ctrl_grp,
                        struct rmon_if_stats *if_stats)
{
    struct rmon_EtherHistoryGroup *rmon_curr_sample_stats;
    u_int32_t current_sample_no;
    struct route_node *rn_curr = NULL;

    struct prefix curr;

    if (! rmon_history_ctrl_grp || ! if_stats)
    {
        return RESULT_ERROR;
    }

    current_sample_no = rmon_history_ctrl_grp->current_sample_no;

    PREFIX_RMON_SET(&curr, current_sample_no);

    rn_curr = route_node_lookup
              (rmon_history_ctrl_grp->historyStats_table,
               (struct prefix *) & curr);

    if (rn_curr)
    {
        if (rn_curr->info == NULL)
        {
            rmon_curr_sample_stats = XCALLOC(MTYPE_RMON,
                                             sizeof(struct rmon_EtherHistoryGroup));
        }
        else
        {
            rmon_curr_sample_stats = rn_curr->info;
        }

        route_unlock_node(rn_curr);
    }
    else
    {
        return RESULT_ERROR;
    }


    if (!rmon_curr_sample_stats || !if_stats)
    {
        return RESULT_ERROR;
    }

    rmon_curr_sample_stats->etherHistoryIndex = rmon_history_ctrl_grp->historyControlIndex;
    rmon_history_ctrl_grp->linearity_sample_no++ ;
    rmon_curr_sample_stats->etherHistorySampleIndex = rmon_history_ctrl_grp->current_sample_no ;
    rmon_curr_sample_stats->etherHistoryCRCAlignErrors = if_stats->bad_crc;
    rmon_curr_sample_stats->etherHistoryUndersizePkts = if_stats->undersize_pkts;
    rmon_curr_sample_stats->etherHistoryOversizePkts = if_stats->oversize_pkts_rcv;
    rmon_curr_sample_stats->etherHistoryFragments = if_stats->fragments_pkts;
    rmon_curr_sample_stats->etherHistoryJabbers = if_stats->jabber_pkts;
    rmon_curr_sample_stats->etherHistoryCollisions = if_stats->collisions;
    rmon_curr_sample_stats->etherHistoryOctets = if_stats->rx_bytes;
    rmon_curr_sample_stats->etherHistoryPkts = if_stats->rx_packets;
    rmon_curr_sample_stats->etherHistoryBroadcastPkts = if_stats->rx_broadcast;
    rmon_curr_sample_stats->etherHistoryMulticastPkts = if_stats->rx_multicast;
    rmon_curr_sample_stats->etherHistoryDropEvents = if_stats->drop_events;
    rmon_curr_sample_stats->etherHistoryUtilization = 0;
    rmon_curr_sample_stats->etherHistoryIntervalStart = rmon_time_since_boot();

    rn_curr->info = rmon_curr_sample_stats;

    return RESULT_OK;
}

//int rmon_coll_history_timer_handler(struct thread *t)
int rmon_coll_history_timer_handler(void *t)
{
    struct rmon_HistoryControlGroup *rmon_history_ctrl_grp = NULL;
    struct rmon_if_stats if_stats;
    struct prefix p;
    struct rmon_EtherHistoryGroup *rmon_ether_history_stats = NULL ;
    struct route_node *rn;
    int ret = RESULT_OK;

    memset(&if_stats , 0 , sizeof(struct rmon_if_stats));

    struct thread_master *rmond_master  = (struct thread_master *)get_rmond_thread_master();

    //rmon_history_ctrl_grp = THREAD_ARG(t);
    rmon_history_ctrl_grp = (struct rmon_HistoryControlGroup *)t;

    if (NULL == rmon_history_ctrl_grp)
    {
        return (RESULT_ERROR);
    }
    //else if (NULL == rmon_history_ctrl_grp->rmon_coll_history_timer)
    //{
    //    rmon_history_ctrl_grp->rmon_coll_history_timer = thread_add_timer(rmond_master ,
    //            rmon_coll_history_timer_handler,
    //            rmon_history_ctrl_grp,
    //            rmon_history_ctrl_grp->historyControlInterval);
    //    return RESULT_ERROR;
    //}

    else if (NULL == rmon_history_ctrl_grp->historyStats_table)
    {
        rmon_history_ctrl_grp->historyStats_table = route_table_init();

        if (NULL == rmon_history_ctrl_grp->historyStats_table)
        {
            //RMON_TIMER_OFF(rmon_history_ctrl_grp->rmon_coll_history_timer);
			high_pre_timer_delete(rmon_history_ctrl_grp->rmon_coll_history_timer);
			rmon_history_ctrl_grp->rmon_coll_history_timer = NULL;
            return RESULT_ERROR;
        }
    }

    rmon_interface_counter_hist_increment(
        rmon_history_ctrl_grp->historyControlDataSource[RMON_IFINDEX_LOC] ,
        &if_stats);

    //if (ret < 0)
    //{
    //    zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d]:'%s': \r\n", __FILE__, __LINE__, __func__);
	//
    //    rmon_history_ctrl_grp->rmon_coll_history_timer = thread_add_timer(
    //                rmond_master, rmon_coll_history_timer_handler,
    //                rmon_history_ctrl_grp, rmon_history_ctrl_grp->historyControlInterval);
    //    return RMON_API_SET_FAILURE;
    //}

    /* Create an entry for the sample, copy it and do calculation */
    PREFIX_RMON_SET(&p, rmon_history_ctrl_grp->current_sample_no);

    rn = route_node_lookup(rmon_history_ctrl_grp->historyStats_table,
                           (struct prefix *) & p);

    if (!rn)
    {
        rn = route_node_get(rmon_history_ctrl_grp->historyStats_table,
                            (struct prefix *) & p);

        if (rn->info == NULL)
        {
            rmon_ether_history_stats = XCALLOC(MTYPE_RMON,
                                               sizeof(struct rmon_EtherHistoryGroup));

            if (!rmon_ether_history_stats)
            {
                route_unlock_node(rn);
                //RMON_TIMER_OFF(rmon_history_ctrl_grp->rmon_coll_history_timer);
				high_pre_timer_delete(rmon_history_ctrl_grp->rmon_coll_history_timer);
				rmon_history_ctrl_grp->rmon_coll_history_timer = NULL;
                return RESULT_ERROR;
            }

            RMON_INFO_SET(rn, rmon_ether_history_stats);
            rmon_ether_history_stats->etherHistorySampleIndex
                = rmon_history_ctrl_grp->current_sample_no;
            ret = rmon_store_sample_stats(rmon_history_ctrl_grp, &if_stats);
            zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d]:'%s': \r\n", __FILE__, __LINE__, __func__);
        }
    }
    else
    {
        zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d]:'%s': \r\n", __FILE__, __LINE__, __func__);
        ret = rmon_store_sample_stats(rmon_history_ctrl_grp, &if_stats);
        route_unlock_node(rn);
    }

    if (ret == RESULT_ERROR)
    {
        //RMON_TIMER_OFF(rmon_history_ctrl_grp->rmon_coll_history_timer);
		high_pre_timer_delete(rmon_history_ctrl_grp->rmon_coll_history_timer);
		rmon_history_ctrl_grp->rmon_coll_history_timer = NULL;
        return RESULT_ERROR;
    }

    rmon_history_ctrl_grp->current_sample_no++;

    /* Reset the sample to start */
    if (rmon_history_ctrl_grp->current_sample_no
            > rmon_history_ctrl_grp->historyControlBucketsGranted)
    {
        rmon_history_ctrl_grp->current_sample_no = 1;
    }

    //rmon_history_ctrl_grp->rmon_coll_history_timer = NULL;
    //rmon_history_ctrl_grp->rmon_coll_history_timer = thread_add_timer(rmond_master,
    //        rmon_coll_history_timer_handler,
    //        rmon_history_ctrl_grp, rmon_history_ctrl_grp->historyControlInterval);
    return RESULT_OK;
}

static void
rmon_copy_alarm_value(struct rmon_if_stats *if_stats,
                      u_int32_t ether_stats_var, ut_int64_t *value)
{
    if (!if_stats)
    {
        return ;
    }

    if (ether_stats_var == ETHERSTATSDROPEVENTS)
    {
        memcpy(value, &if_stats->drop_events, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSOCTETS)
    {
        memcpy(value, &if_stats->rx_bytes , sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSPKTS)
    {
        memcpy(value, &if_stats->rx_packets , sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSBROADCASTPKTS)
    {
        memcpy(value, &if_stats->rx_broadcast, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSMULTICASTPKTS)
    {
        memcpy(value, &if_stats->rx_multicast, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSCRCALIGNERRORS)
    {
        memcpy(value, &if_stats->bad_crc, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSUNDERSIZEPKTS)
    {
        memcpy(value, &if_stats->undersize_pkts, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSOVERSIZEPKTS)
    {
        memcpy(value, &if_stats->oversize_pkts_rcv, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSFRAGMENTS)
    {
        memcpy(value, &if_stats->fragments_pkts, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSJABBERS)
    {
        memcpy(value, &if_stats->jabber_pkts, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSCOLLISIONS)
    {
        memcpy(value, &if_stats->collisions, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSPKTS64OCTETS)
    {
        memcpy(value, &if_stats->pkts_64_octets, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSPKTS65TO127OCTETS)
    {
        memcpy(value, &if_stats->pkts_65_127_octets, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSPKTS128TO255OCTETS)
    {
        memcpy(value, &if_stats->pkts_128_255_octets, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSPKTS256TO511OCTETS)
    {
        memcpy(value, &if_stats->pkts_256_511_octets, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSPKTS512TO1023OCTETS)
    {
        memcpy(value, &if_stats->pkts_512_1023_octets, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSPKTS1024TO1518OCTETS)
    {
        memcpy(value, &if_stats->pkts_1024_1518_octets, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSUNICASTPKTS)
    {
        memcpy(value, &if_stats->rx_ucast, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSBADPKTS)
    {
        memcpy(value, &if_stats->rx_errors, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSINDISCARDPKTS)
    {
        memcpy(value, &if_stats->rx_dropped, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSSENTOCTETS)
    {
        memcpy(value, &if_stats->tx_bytes, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSSENTPKTS)
    {
        memcpy(value, &if_stats->tx_packets, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSSENTUNICASTPKTS)
    {
        memcpy(value, &if_stats->tx_ucast, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSSENTBROADCASTPKTS)
    {
        memcpy(value, &if_stats->tx_broadcast, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSSENTMULTICASTPKTS)
    {
        memcpy(value, &if_stats->tx_multicast, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSSENTBADPKTS)
    {
        memcpy(value, &if_stats->tx_errors, sizeof(ut_int64_t));
    }

    if (ether_stats_var == ETHERSTATSSENTDISCARDPKTS)
    {
        memcpy(value, &if_stats->tx_dropped, sizeof(ut_int64_t));
    }


    return ;
}


//int rmon_alarm_timer_handler(struct thread *t)
int rmon_alarm_timer_handler(void *t)
{
    struct rmon_AlarmGroup *alarm = NULL;
    //oid ether_stats_oid[] = {1, 3, 6, 1, 2, 1, 16, 1, 1, 1};
    u_int32_t ether_stats_index = 0;
    u_int32_t ether_stats_var = 0;
    struct rmon_etherStatsGroup *rmon_ether_stats = NULL;
    struct rmon_if_stats if_stats;
    memset(&if_stats , 0 , sizeof(struct rmon_if_stats));
    ut_int64_t zero_threshold;

    s_int32_t  ret = 0;
    s_int32_t result = 0;

    struct thread_master *rmond_master  = (struct thread_master *)get_rmond_thread_master();

    //alarm =  THREAD_ARG(t);
    alarm =  (struct rmon_AlarmGroup *)(t);

    if (NULL == alarm)
    {
        return RESULT_ERROR;
    }
    //else if (NULL == alarm->alarm_timer)
    //{
    //    alarm->alarm_timer = thread_add_timer(rmond_master, rmon_alarm_timer_handler,
    //                                          alarm, alarm->alarmInterval);
    //    return RESULT_ERROR;
    //}

    //alarm->alarm_timer = NULL;

    //if (memcmp(alarm->alarmVariable, ether_stats_oid, ETHER_STATS_OID_SIZE))
    //{
    //    alarm->alarm_timer = thread_add_timer(rmond_master, rmon_alarm_timer_handler,
    //                                          alarm, alarm->alarmInterval);
	//
    //    return RESULT_ERROR;
    //}

    ether_stats_index = alarm->alarmVariable [RMOB_ALARM_OID_INDEX_STATS];
    ether_stats_var = alarm->alarmVariable [RMOB_ALARM_OID_INDEX_TYPE];

    rmon_ether_stats = rmon_ether_stats_lookup_by_id(&ether_stats_index, 1);

    if (!rmon_ether_stats)
    {
        //alarm->alarm_timer = thread_add_timer(rmond_master, rmon_alarm_timer_handler,
        //                                      alarm, alarm->alarmInterval);

        return RESULT_ERROR;
    }

    if (rmon_ether_stats->etherStatsStatus == VALID_STATUS)
    {
        rmon_interface_counter_curr_increment(
            rmon_ether_stats->etherStatsDataSource[RMON_IFINDEX_LOC] , &if_stats);

        rmon_copy_alarm_value(&if_stats, ether_stats_var, &alarm->alarm_curr_value);

        if (alarm->alarmSampleType == RMON_ALARM_DELTA)
        {
            alarm->alarmValue = rmon_struct_value_sub(alarm->alarm_curr_value , alarm->alarm_prev_value) ;
            memcpy(&alarm->alarm_prev_value, &alarm->alarm_curr_value,
                   sizeof(ut_int64_t));
            zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d]:'%s':  alarm->alarmValue.l[0] [%8x] alarm->alarmValue.l[1] [%8x] \r\n",
                       __FILE__, __LINE__, __func__ , alarm->alarm_curr_value.l[0] , alarm->alarm_curr_value.l[1]);
        }
        else if ((alarm->alarmSampleType == RMON_ALARM_ABS))
        {
            rmon_copy_alarm_value((struct rmon_if_stats *)&alarm->alarm_prev_value,
                                  (u_int32_t)ether_stats_var,
                                  (ut_int64_t *)&alarm->alarm_last_value);

            memcpy(&alarm->alarmValue, &alarm->alarm_curr_value,
                   sizeof(ut_int64_t));

            zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d]:'%s':  alarm->alarmValue.l[0] [%8x] alarm->alarmValue.l[1] [%8x] \r\n",
                       __FILE__, __LINE__, __func__ , alarm->alarm_curr_value.l[0] , alarm->alarm_curr_value.l[1]);
        }

        memset(&zero_threshold, 0, sizeof(ut_int64_t));

        if (alarm->alarm_startup_alarm_status != RMON_ALARM_STARTUP_MET)
        {
            if (alarm->alarmStartupAlarm == RMON_ALARM_STARTUP_RISING)
            {
                ret = rmon_alarm_rising_threshold_handler(alarm, zero_threshold, if_stats);

                if (ret == RMON_ALARM_MET)
                {
                    alarm->alarm_startup_alarm_status = RMON_ALARM_STARTUP_MET;
                    result = RESULT_OK;
                }
                else
                {
                    result = RESULT_ERROR;
                }
            }
            else if (alarm->alarmStartupAlarm == RMON_ALARM_STARTUP_RISINGANDFALLING)
            {
                ret = rmon_alarm_rising_threshold_handler(alarm, zero_threshold, if_stats);

                if (ret == RMON_ALARM_MET)
                {
                    alarm->alarm_startup_alarm_status = RMON_ALARM_STARTUP_MET;
                    result = RESULT_OK;
                }
                else  /*StartUp Falling alarm*/
                {
                    ret = rmon_alarm_falling_threshold_handler(alarm, zero_threshold, if_stats);

                    if (ret == RMON_ALARM_MET)
                    {
                        alarm->alarm_startup_alarm_status = RMON_ALARM_STARTUP_MET;
                        result = RESULT_OK;
                    }
                    else
                    {
                        result = RESULT_ERROR;
                    }
                }
            }
            else if (alarm->alarmStartupAlarm == RMON_ALARM_STARTUP_FALLING)
            {
                ret = rmon_alarm_falling_threshold_handler(alarm, zero_threshold, if_stats);

                if (ret == RMON_ALARM_MET)
                {
                    alarm->alarm_startup_alarm_status = RMON_ALARM_STARTUP_MET;
                    result = RESULT_OK;
                }
                else
                {
                    result = RESULT_ERROR;
                }
            }
        }
        else
        {
            ret = rmon_alarm_rising_threshold_handler(alarm, zero_threshold, if_stats);

            if (ret == RMON_ALARM_MET)
            {
                result = RESULT_OK;
            }
            else
            {
                ret = rmon_alarm_falling_threshold_handler(alarm, zero_threshold, if_stats);

                if (ret == RMON_ALARM_MET)
                {
                    result = RESULT_OK;
                }
                else
                {
                    result = RESULT_ERROR;
                }
            }
        }

        //alarm->alarm_timer =  NULL;

        //alarm->alarm_timer = thread_add_timer(rmond_master, rmon_alarm_timer_handler,
        //                                      alarm, alarm->alarmInterval);

        return result;
    }

    //alarm->alarm_timer =  NULL;
    //alarm->alarm_timer = thread_add_timer(rmond_master, rmon_alarm_timer_handler,
    //                                      alarm, alarm->alarmInterval);

    return RMON_ALARM_NOT_MET;
}

int
rmon_alarm_rising_threshold_handler(struct rmon_AlarmGroup *alarm,
                                    ut_int64_t zero_threshold,
                                    struct rmon_if_stats if_stats)
{
    unsigned long long ull_thr = 0;
    unsigned long long ull_alarm_value = 0;
    unsigned long long ull_alarmRisingThreshold_value = 0;


    if (alarm == NULL)
    {
        return RESULT_ERROR;
    }

    rmon_converter_struct_2_longlong(alarm->alarmRisingThreshold , &ull_thr);
    rmon_converter_struct_2_longlong(alarm->alarmValue , &ull_alarm_value);
    rmon_converter_struct_2_longlong(alarm->alarmRisingThreshold , &ull_alarmRisingThreshold_value);

    if ((alarm->alarm_last_event != RMON_RISE_ALARM) &&
        ull_alarm_value > ull_alarmRisingThreshold_value)
    {

        struct rmon_EventGroup *rising_event = NULL;

        rising_event = rmon_event_entry_info_lookup(alarm->alarmRisingEventIndex);

        if ((rising_event) && (&alarm->alarmRisingEventIndex != 0)
                && (rising_event->eventStatus == VALID_STATUS))
        {
            if (rising_event->eventType == RMON_EVENT_LOG ||
                    rising_event->eventType ==  RMON_EVENT_LOG_TRAP)
            {
                zlog_err(" Alarm Index %d "
                           " alarm Rising Threshold %llu "
                           " alarm Value %llu "
                           " alarm Rising event Index %d \n",
                           alarm->alarmIndex,
                           ull_thr ,
                           ull_alarm_value ,
                           alarm->alarmRisingEventIndex);
            }

            if ((rising_event->eventType == RMON_EVENT_TRAP ||
                    rising_event->eventType ==  RMON_EVENT_LOG_TRAP))
            {
                /* Trigger the trap */
                rmon_trap(alarm, RMON_RISE_ALARM);
            }

            alarm->alarmStatus = VALID_STATUS;
            alarm->alarm_last_event = RMON_RISE_ALARM;
            rising_event->eventLastTimeSent = rmon_time_since_boot();
            return RMON_ALARM_MET;
        }
    }

    return RMON_ALARM_NOT_MET;
}

int
rmon_alarm_falling_threshold_handler(struct rmon_AlarmGroup *alarm,
                                     ut_int64_t zero_threshold,
                                     struct rmon_if_stats if_stats)
{
    unsigned long long ull_thr = 0;
    unsigned long long ull_alarm_value = 0;
    unsigned long long ull_alarmRisingThreshold_value = 0;

    if (!alarm)
    {
        return RESULT_ERROR;
    }

    rmon_converter_struct_2_longlong(alarm->alarmFallingThreshold , &ull_thr);
    rmon_converter_struct_2_longlong(alarm->alarmValue , &ull_alarm_value);
    rmon_converter_struct_2_longlong(alarm->alarmFallingThreshold , &ull_alarmRisingThreshold_value);

     if ((alarm->alarm_last_event != RMON_FALL_ALARM) &&
         ull_alarm_value < ull_alarmRisingThreshold_value)
    {
        struct rmon_EventGroup *falling_event = NULL;

        falling_event = rmon_event_entry_info_lookup(alarm->alarmFallingEventIndex);



        if ((falling_event) && (&alarm->alarmFallingEventIndex != 0)
                && (falling_event->eventStatus == VALID_STATUS))
        {
            if (falling_event->eventType == RMON_EVENT_LOG ||
                    falling_event->eventType ==  RMON_EVENT_LOG_TRAP)
            {
                zlog_err(" Alarm Index %d "
                           " alarm Falling Threshold %llu "
                           " alarm Value %llu "
                           " alarm Falling event Index %d \n",
                           alarm->alarmIndex,
                           ull_thr ,
                           ull_alarm_value ,
                           alarm->alarmFallingEventIndex);
            }

            if ((falling_event->eventType == RMON_EVENT_TRAP ||
                    falling_event->eventType == RMON_EVENT_LOG_TRAP))
            {
                /* Trigger the trap */
                rmon_trap(alarm, RMON_FALL_ALARM);
            }

            alarm->alarmStatus = VALID_STATUS;
            alarm->alarm_last_event = RMON_FALL_ALARM;
            falling_event->eventLastTimeSent = rmon_time_since_boot();
            return RMON_ALARM_MET;
        }
    } /* End of falling threshold */

    return RMON_ALARM_NOT_MET;
}
