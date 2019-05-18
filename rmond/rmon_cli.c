/******************************************************************************
 * Filename: rmond_cli.c
 *  Copyright (c) 2016-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: rmond_cli functions for rmond.
 *
 * History:
 *  *2017.10.23  lidingcheng created
 *
******************************************************************************/
#include <lib/vty.h>
#include <lib/module_id.h>
#include <lib/ifm_common.h>
#include <lib/rmon_common.h>

#include "prefix.h"
#include "rmon_config.h"
#include "rmond.h"

#include "rmon_api.h"
#include "rmond_main.h"
#include "rmon_cli.h"
#include "lib/log.h"


#define CLI_SPLITER "--------------------------------------------------------------------------------"

static struct cmd_node rmon_node =
{
    RMON_NODE,
    "%s(config-rmon)# ",
    1
};

DEFUN (rmon_conf_debug_fun,
       rmon_conf_debug_cmd,
       "debug rmon (enable|disable) (all|hal|snmp)",
       "Debub config\n"
       "rmon config\n"
       "rmon debug enable\n"
       "rmon debug disable\n"
       "rmon debug type all\n"
       "rmon debug type hal\n"
       "rmon debug type snmp\n"
)
{
    int enable = 0;
    unsigned int type = 0;

    if(argv[0][0] == 'e') enable = 1;

    if(strcmp(argv[1],"snmp") == 0)
        type = RMON_MTYPE_SNMP;
    else if(strcmp(argv[1],"hal") == 0)
        type = RMON_MTYPE_HAL;
    else if(strcmp(argv[1],"all") == 0)
        type = RMON_MTYPE_MAX;
    else type = RMON_MTYPE_MAX;

    zlog_debug_set(vty, type,  enable);
    return(CMD_SUCCESS);
}



DEFUN(
    rmon_mode_enable,
    rmon_mode_enable_cmd,
    "rmon",
    "rmon\n"
    "rmon command node\n")
{
    vty->node = RMON_NODE;
    return CMD_SUCCESS;
}

DEFUN(rmon_clear_if_counters,
      rmon_clear_if_counters_cmd,
      "rmon clear counters",
      "rmon\n"
      "clear counters\n"
      "counters\n")
{
    rmon_api_clear_all_counters();

    return CMD_SUCCESS;
}

DEFUN(rmon_if_collection_stats,
      rmon_if_collection_stats_cmd,
      "rmon collection (ethernet|gigabitethernet|xgigabitethernet) IFNAME statistics <1-100> {owner WORD}",
      "rmon\n"
      "collection\n "
      "ethernet\n"
      "gigabitethernet\n"
      "xgigabitethernet\n"
      "interface name\n"
      "ethernet statistics\n"
      "stat Index\n"
      "rmon owner identity\n"
      "rmon stat owner name max length 126\n")
{
    s_int32_t ret = 0;
    u_int32_t ifindex = 0;
    int stat = 0; 
    struct ifm_port ifm_port;
	memset(&ifm_port, 0, sizeof(struct ifm_port));

    ifindex = ifm_get_ifindex_by_name(argv[0], (char *) argv[1]);
    if (0 == ifindex)
    {
        vty_error_out(vty, "interface name %s not correct %s", (char *)argv[1], VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (ifm_get_port_info(ifindex, MODULE_ID_RMON, &ifm_port) < 0)
    {
        vty_error_out(vty, "interface name %s not exists %s", (char *)argv[1], VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    stat = atoi(argv[2]);
    ret  = rmon_coll_stats_entry_status(stat);

    if (RESULT_OK == ret)
    {
        vty_error_out(vty, "rmon collection statistics already exist %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (NULL != argv[3])
    {
        if (strlen(argv[3]) >= RMON_OWNER_NAME_SIZE)
        {
            vty_error_out(vty, "rmon collection statistics owner size exceeded %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    ret = rmon_coll_stats_entry_add(ifindex, stat, (char *)argv[3]);

    if (ret < 0)
    {
        vty_error_out(vty, "rmon collection statistics add failure %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(no_rmon_if_collection_stats,
      no_rmon_if_collection_stats_cmd,
      "no rmon collection statistics <1-100> ",
      "no\n"
      "rmon\n"
      "collection \n"
      "ethernet statistics\n"
      "statistics index\n")
{
    int ret = 0;
    int stat = 0;

    stat = atoi(argv[0]);

    ret = rmon_coll_stats_entry_remove(stat);

    if (ret < 0)
    {
        vty_error_out(vty, "rmon collection statistics remove failure %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(rmon_if_collection_history,
      rmon_if_collection_history_cmd,
      "rmon collection (ethernet|gigabitethernet|xgigabitethernet) IFNAME history <1-300> buckets <1-50> interval <1-86400> {owner WORD} ",
      "rmon\n"
      "collection\n"
      "ethernet\n"
      "gigabitethernet\n"
      "xgigabitethernet\n"
      "interface name\n"
      "history commands\n"
      "history Index\n"
      "buckets (default 50)\n"
      "buckets number\n"
      "polling Interval (default 1800)\n"
      "seconds\n"
      "owner identity\n"
      "owner name max length 126\n")
{
    u_int32_t bucketNum = 50;
    u_int32_t interval = 1800;
    s_int32_t ret = 0;
    int stat = 0;
    u_int32_t ifindex = 0;
    struct rmon_HistoryControlGroup *prmon_history_ctrl;
    struct ifm_port ifm_port;
	memset(&ifm_port, 0, sizeof(struct ifm_port));

    ifindex = ifm_get_ifindex_by_name(argv[0], (char *)argv[1]);

    if (0 == ifindex)
    {
        vty_error_out(vty, "interface name %s not correct %s", (char *)argv[1], VTY_NEWLINE);
        return CMD_WARNING;
    }
	
    if (ifm_get_port_info(ifindex, MODULE_ID_RMON, &ifm_port) < 0)
    {
        vty_error_out(vty, "interface name %s not exists %s", (char *)argv[1], VTY_NEWLINE);
        return CMD_WARNING;
    }

    stat      = atoi(argv[2]);
    bucketNum = atoi(argv[3]);
    interval  = atoi(argv[4]);
    ret = rmon_coll_history_entry_status(stat);

    if (RESULT_OK == ret)
    {
        vty_error_out(vty, "rmon collection history already exist %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (NULL != argv[5])
    {
        if (strlen(argv[5]) >= RMON_OWNER_NAME_SIZE)
        {
            vty_error_out(vty, "rmon collection history owner size exceeded %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    prmon_history_ctrl = rmon_coll_history_lookup_by_ifindex(ifindex, interval);

    if (prmon_history_ctrl != NULL)
    {
        vty_error_out(vty, "exists same interval for interface %s %s%s", argv[0], (char *)argv[1] , VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = rmon_coll_history_entry_add(ifindex, stat, bucketNum, interval, (char *)argv[5]);

    if (ret < 0)
    {
        vty_error_out(vty, "rmon collection history add failure %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    prmon_history_ctrl = rmon_coll_history_entry_info_lookup(stat);

    if (NULL != prmon_history_ctrl)
    {
        rmon_coll_history_entry_active(prmon_history_ctrl);
    }

    return CMD_SUCCESS;
}

DEFUN(no_rmon_if_collection_history,
      no_rmon_if_collection_history_cmd,
      "no rmon collection history <1-300> ",
      "no\n"
      "rmon\n"
      "collection \n"
      "history commands\n"
      "history Index\n")
{
    s_int32_t ret = 0;
    s_int32_t stat = 0;

    stat = atoi(argv[0]);
    ret = rmon_coll_history_entry_remove(stat);

    if (ret < 0)
    {
        vty_error_out(vty, "rmon collection history remove failure %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(rmon_event_log,
      rmon_event_log_cmd,
      "rmon event <1-1200> (log|trap|log-trap) description WORD {community WORD | owner WORD}",
      "rmon\n"
      "event command\n"
      "event Index\n"
      "log the event\n"
      "trap the event\n"
      "log and trap the event\n"
      "event description\n"
      "description string max length 126\n"
      "event community\n"
      "community string max length 126\n"
      "owner identity\n"
      "owner name max length 126\n")
{
    int ret = 0;
    s_int32_t stat = 0;
    int event_type = RMON_EVENT_NONE;

    stat = atoi(argv[0]);
    ret  = rmon_event_entry_status(stat);

    if (RESULT_OK == ret)
    {
        vty_error_out(vty, "rmon event already exist %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (0 == strcmp("log-trap", argv[1]))
    {
        event_type = RMON_EVENT_LOG_TRAP;
    }
    else if (0 == strcmp("log", argv[1]))
    {
        event_type = RMON_EVENT_LOG;
    }
    else
    {
        event_type = RMON_EVENT_TRAP;
    }

    /* description same with mib */
//    if (strlen(argv[2]) >= RMON_DESCR_LENGTH)
    if (strlen(argv[2]) >= RMON_OWNER_NAME_SIZE)    
    {
        vty_error_out(vty, "rmon event description size exceede %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (NULL != argv[3])
    {
        if (RMON_EVENT_LOG == event_type)
        {
            vty_error_out(vty, "log can not set community %s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        if (strlen(argv[3]) >= RMON_COMM_LENGTH)
        {
            vty_error_out(vty, "rmon event community size exceede %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    if (NULL != argv[4])
    {
#if 0

        if (RMON_EVENT_TRAP == event_type)
        {
            vty_error_out(vty, "rmon event log set owner %s", VTY_NEWLINE);
            return CMD_WARNING;
        }

#endif

        if (strlen(argv[4]) >= RMON_OWNER_NAME_SIZE)
        {
            vty_error_out(vty, "rmon event owner size exceede %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    ret = rmon_event_entry_add(stat, event_type, (char *)argv[2], (char *)argv[3], (char *)argv[4]);

    if (ret < 0)
    {
        vty_error_out(vty, "rmon event add failure %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(no_rmon_event,
      no_rmon_event_cmd,
      "no rmon event <1-1200> ",
      "no\n"
      "rmon\n"
      "event commands\n"
      "event Index\n")
{
    int ret = 0;
    int stat = 0;

    stat = atoi(argv[0]);
    ret = rmon_event_entry_remove(stat);

    if (ret < 0)
    {
        vty_error_out(vty, "rmon event remove failure %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(rmon_alarm,
      rmon_alarm_cmd,
      "rmon alarm <1-600> statistics <1-100> type  (etherStatsDropEvents | etherStatsOctets"
      " | etherStatsPkts | etherStatsBroadcastPkts | etherStatsMulticastPkts | etherStatsCRCAlignErrors"
      " | etherStatsUndersizePkts | etherStatsOversizePkts | etherStatsFragments | etherStatsJabbers"
      " | etherStatsCollisions | etherStatsPkts64Octets | etherStatsPkts65to127Octets | etherStatsPkts128to255Octets"
      " | etherStatsPkts256to511Octets | etherStatsPkts512to1023Octets | etherStatsPkts1024to1518Octets"
      " | etherStatsUnicastPkts | etherStatsErrorPkts | etherStatsLossPkts | etherStatsTxOctets | etherStatsTxPkts"
      " | etherStatsTxUnicastPkts | etherStatsTxBroadcastPkts | etherStatsTxMulticastPkts | etherStatsTxErrorPkts | etherStatsTxLossPkts)"
      " interval <1-86400> (delta | absolute) "
      " rising-threshold <1-2147483647> event <1-1200> falling-threshold <1-2147483647>"
      " event <1-1200> alarmstartup (rising|falling|risingAndFalling) {owner WORD}",
      "rmon\n"
      "alarm command \n"
      "alarm Index\n"
      "ethernet Statistics\n"
      "ethernet Statistics Index\n"
      "type\n"
      "Ethernet Statistics DropEvents\n"
      "Ethernet Statistics Receive Octets\n"
      "Ethernet Statistics Receive Packets\n"
      "Ethernet Statistics Receive Broadcast Packets\n"
      "Ethernet Statistics Receive Multicast Packets\n"
      "Ethernet Statistics CRC Align Errors\n"
      "Ethernet Statistics UnderSize Packets\n"
      "Ethernet Statistics OverSize Packets\n"
      "Ethernet Statistics Fragments\n"
      "Ethernet Statistics Jabbers\n"
      "Ethernet Statistics Collisions\n"
      "Ethernet Statistics Receive 64 Octets Packets\n"
      "Ethernet Statistics Receive 65 ~ 127 Octets Packets\n"
      "Ethernet Statistics Receive 128 ~ 255 Octets Packets\n"
      "Ethernet Statistics Receive 256 ~ 511 Octets Packets\n"
      "Ethernet Statistics Receive 512 ~ 1023 Octets Packets\n"
      "Ethernet Statistics Receive 1204 ~ 1518 Octets Packets\n"
      "Ethernet Statistics Receive Unicast Packets\n"
      "Ethernet Statistics Receive error Packets\n"
      "Ethernet Statistics In Discards Packets\n"
      "Ethernet Statistics Sent Octets\n"
      "Ethernet Statistics Sent Packets\n"
      "Ethernet Statistics Sent Unicast Packets\n"
      "Ethernet Statistics Sent Broadcast Packets\n"
      "Ethernet Statistics Sent Multicast Packets\n"
      "Ethernet Statistics Sent Error Packets\n"
      "Ethernet Statistics Out discards Packets\n"
      "alarm interval\n"
      "seconds\n"
      "delta\n"
      "absolute\n"
      "alarm rising threshold\n"
      "rising threshold value  <1 to 2147483647>\n"
      "event for rising alarm\n"
      "event index\n"
      "alarm falling threshold\n"
      "falling threshold value <1 to 2147483647>\n"
      "event for falling alarm\n"
      "event index\n"
      "alarm startup type\n"
      "rising\n"
      "falling\n"
      "rising and falling\n"
      "owner identity\n"
      "owner name max length 126\n")
{
    s_int32_t ret = 0;
    u_int32_t alarmIndex = 0;
    s_int32_t stats_type = 0;
    u_int32_t statsIndex = 0;
    u_int32_t interval = 0;
    u_int32_t alarmSampleType = 0;
    s_int32_t risingThresValue;
    s_int32_t fallingThresValue;
    u_int32_t risingEvent = 0;
    u_int32_t fallingEvent = 0;
    u_int8_t alarmstartupalarm = 0;
    struct rmon_EventGroup *rmon_event = NULL ;

    alarmIndex = atoi(argv[0]);
    ret  = rmon_alarm_entry_status(alarmIndex);

    if (RESULT_OK == ret)
    {
        vty_error_out(vty, "rmon alarm already exist %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    statsIndex = atoi(argv[1]);
    ret = rmon_coll_stats_entry_status(statsIndex);

    if (RESULT_OK != ret)
    {
        vty_error_out(vty, "rmon collection statistics  %d not exist %s", statsIndex, VTY_NEWLINE);
        return CMD_WARNING;
    }

    stats_type = rmon_alarm_get_stats_type((char *)argv[2]);

    if (stats_type <= 0)
    {
        vty_error_out(vty, "rmon alarm type %s error %s", argv[2], VTY_NEWLINE);
        return CMD_WARNING;
    }

    interval   = atoi(argv[3]);

    if (strncmp(argv[4], "ab", 2) == 0)
    {
        alarmSampleType = RMON_ALARM_ABS;
    }
    else
    {
        alarmSampleType = RMON_ALARM_DELTA;
    }

    risingThresValue  = atoi(argv[5]);
    risingEvent       = atoi(argv[6]);
    fallingThresValue = atoi(argv[7]);
    fallingEvent      = atoi(argv[8]);

    rmon_event = rmon_event_entry_info_lookup(risingEvent);

    if (NULL == rmon_event)
    {
        vty_error_out(vty, "rmon alarm rising event not exist  %s error %s", argv[2], VTY_NEWLINE);
        return CMD_WARNING;
    }

    rmon_event = rmon_event_entry_info_lookup(fallingEvent);

    if (NULL == rmon_event)
    {
        vty_error_out(vty, "rmon alarm falling event not exist  %s error %s", argv[2], VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strncmp(argv[9] , "risingAnd", 9) == 0)
    {
        alarmstartupalarm = 3;
    }
    else if (strncmp(argv[9] , "falling", 6) == 0)
    {
        alarmstartupalarm = 2;
    }
    else
    {
        alarmstartupalarm = 1;
    }

    if (NULL != argv[10])
    {
        if (strlen(argv[10]) >= RMON_OWNER_NAME_SIZE)
        {
            vty_error_out(vty, "rmon alarm owner size exceede %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    ret = rmon_alarm_entry_add(alarmIndex, \
                               stats_type, (char *)argv[2], statsIndex, \
                               interval, \
                               alarmSampleType, \
                               risingThresValue, \
                               fallingThresValue, \
                               risingEvent, \
                               fallingEvent, \
                               (char *)argv[10], \
                               alarmstartupalarm);

    if (ret < 0)
    {
        vty_error_out(vty, "rmon alarm add failure %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(no_rmon_alarm,
      no_rmon_alarm_cmd,
      "no rmon alarm <1-600> ",
      "no\n"
      "rmon\n"
      "alarm commands\n"
      "alarm Index\n")
{
    int ret = 0;
    s_int32_t stat = 0;

    stat = atoi(argv[0]);
    ret = rmon_alarm_index_remove(stat);

    if (ret < 0)
    {
        vty_error_out(vty, "rmon alarm remove failure %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(show_rmon_spec_if_stats,
      show_rmon_spec_if_stats_cmd,
      "show rmon statistics <1-100>",
      "show\n"
      "rmon\n"
      "statistics\n"
      "statistics index\n")
{
    struct route_node *rn;
    struct rmon_etherStatsGroup *rinfo;
    struct rmon_if_stats if_stats;
    u_int32_t i_stat = 0;
    u_int32_t ifindex = 0;
    char ifname[IFNET_NAMESIZE];
    struct rmon_master *rm = (struct rmon_master *)get_rmond_master_value();

    i_stat = atoi(argv[0]);

    if (NULL == rm || NULL == rm->etherStats_table)
    {
        return CMD_WARNING;
    }

    for (rn = route_top(rm->etherStats_table); rn; rn = route_next(rn))
    {
        rinfo = (struct rmon_etherStatsGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue;
        }

        if (i_stat != rinfo->etherStatsIndex)
        {
            continue;
        }

        ifindex = rinfo->etherStatsDataSource[RMON_IFINDEX_LOC];

        if (rinfo->etherStatsStatus == VALID_STATUS)
        {
            memset(&if_stats, 0, sizeof(struct rmon_if_stats));

            rmon_interface_counter_curr_increment(ifindex, &if_stats);
            rmon_if_stats_2_rmon_etherStatsGroup(&if_stats, rinfo);
            ifm_get_name_by_ifindex(ifindex, ifname);
            vty_out(vty,  "    rmon collection index: %d %s"
                    "    dataSouce: %s %s"
                    "    owner: %s %s",
                    rinfo->etherStatsIndex, VTY_NEWLINE, \
                    ifname, VTY_NEWLINE, \
                    (char *)rinfo->etherStatsOwner, VTY_NEWLINE);

            uint64_t etherStatsDropEvents = 0;
            rmon_converter_struct_2_longlong(rinfo->etherStatsDropEvents, &etherStatsDropEvents);
            uint64_t etherStatsOctets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsOctets, &etherStatsOctets);
            uint64_t etherStatsPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts, &etherStatsPkts);
            uint64_t etherStatsBroadcastPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsBroadcastPkts, &etherStatsBroadcastPkts);
            uint64_t etherStatsMulticastPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsMulticastPkts, &etherStatsMulticastPkts);
            uint64_t etherStatsCRCAlignErrors = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsCRCAlignErrors, &etherStatsCRCAlignErrors);
            uint64_t etherStatsPkts64Octets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts64Octets, &etherStatsPkts64Octets);
            uint64_t etherStatsOversizePkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsOversizePkts, &etherStatsOversizePkts);
            uint64_t etherStatsPkts65to127Octets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts65to127Octets, &etherStatsPkts65to127Octets);
            uint64_t etherStatsJabbers = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsJabbers, &etherStatsJabbers);
            uint64_t etherStatsPkts128to255Octets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts128to255Octets, &etherStatsPkts128to255Octets);
            uint64_t etherStatsCollisions = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsCollisions, &etherStatsCollisions);
            uint64_t etherStatsPkts256to511Octets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts256to511Octets, &etherStatsPkts256to511Octets);
            uint64_t etherStatsFragments = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsFragments, &etherStatsFragments);
            uint64_t etherStatsPkts512to1023Octets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts512to1023Octets, &etherStatsPkts512to1023Octets);
            uint64_t etherStatsUndersizePkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsUndersizePkts, &etherStatsUndersizePkts);
            uint64_t etherStatsPkts1024to1518Octets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts1024to1518Octets, &etherStatsPkts1024to1518Octets);
            uint64_t etherStatsUnicastPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsUnicastPkts, &etherStatsUnicastPkts);
            uint64_t etherStatsErrorPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsErrorPkts, &etherStatsErrorPkts);
            uint64_t etherStatsLossPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsLossPkts, &etherStatsLossPkts);
            uint64_t etherStatsTxOctets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxOctets, &etherStatsTxOctets);
            uint64_t etherStatsTxPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxPkts, &etherStatsTxPkts);
            uint64_t etherStatsTxUnicastPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxUnicastPkts, &etherStatsTxUnicastPkts);
            uint64_t etherStatsTxBroadcastPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxBroadcastPkts, &etherStatsTxBroadcastPkts);
            uint64_t etherStatsTxMulticastPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxMulticastPkts, &etherStatsTxMulticastPkts);
            uint64_t etherStatsTxErrorPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxErrorPkts, &etherStatsTxErrorPkts);
            uint64_t etherStatsTxLossPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxLossPkts, &etherStatsTxLossPkts);

            vty_out(vty,   "          drop events %21llu %s"
                    "         stats octets %21llu %s"
                    "         stats packts %21llu %s"
                    "       broadcast pkts %21llu %s"
                    "       multicast pkts %21llu %s"
                    "           crc errors %21llu %s"
                    "       64 octets pkts %21llu %s"
                    "        overSize pkts %21llu %s"
                    "   65-127 octets pkts %21llu %s"
                    "        stats jabbers %21llu %s"
                    "  128-255 octets pkts %21llu %s"
                    "      stats collsions %21llu %s"
                    "  256-511 octets pkts %21llu %s"
                    "      stats fragments %21llu %s"
                    " 512-1023 octets pkts %21llu %s"
                    "       underSize pkts %21llu %s"
                    " 1024-max octets pkts %21llu %s"
                    "     unicast pkts rcv %21llu %s"
                    "         bad pkts rcv %21llu %s"
                    "     in discards pkts %21llu %s"
                    "    stats octets sent %21llu %s"
                    "      stats pkts sent %21llu %s"
                    "    unicast pkts sent %21llu %s"
                    "  broadcast pkts sent %21llu %s"
                    "  multicast pkts sent %21llu %s"
                    "        bad pkts sent %21llu %s"
                    "    out discards sent %21llu %s" ,
                    etherStatsDropEvents, VTY_NEWLINE,           \
                    etherStatsOctets, VTY_NEWLINE,               \
                    etherStatsPkts, VTY_NEWLINE,                 \
                    etherStatsBroadcastPkts, VTY_NEWLINE,        \
                    etherStatsMulticastPkts, VTY_NEWLINE,        \
                    etherStatsCRCAlignErrors, VTY_NEWLINE,       \
                    etherStatsPkts64Octets, VTY_NEWLINE,         \
                    etherStatsOversizePkts, VTY_NEWLINE,         \
                    etherStatsPkts65to127Octets, VTY_NEWLINE,    \
                    etherStatsJabbers, VTY_NEWLINE,              \
                    etherStatsPkts128to255Octets, VTY_NEWLINE,   \
                    etherStatsCollisions, VTY_NEWLINE,           \
                    etherStatsPkts256to511Octets, VTY_NEWLINE,   \
                    etherStatsFragments, VTY_NEWLINE,            \
                    etherStatsPkts512to1023Octets, VTY_NEWLINE,  \
                    etherStatsUndersizePkts, VTY_NEWLINE,        \
                    etherStatsPkts1024to1518Octets, VTY_NEWLINE, \
                    etherStatsUnicastPkts, VTY_NEWLINE,          \
                    etherStatsErrorPkts, VTY_NEWLINE,            \
                    etherStatsLossPkts, VTY_NEWLINE,             \
                    etherStatsTxOctets, VTY_NEWLINE,             \
                    etherStatsTxPkts, VTY_NEWLINE,               \
                    etherStatsTxUnicastPkts, VTY_NEWLINE,        \
                    etherStatsTxBroadcastPkts, VTY_NEWLINE,      \
                    etherStatsTxMulticastPkts, VTY_NEWLINE,      \
                    etherStatsTxErrorPkts, VTY_NEWLINE,          \
                    etherStatsTxLossPkts, VTY_NEWLINE
                   );

            vty_out(vty, "%s %s", CLI_SPLITER, VTY_NEWLINE);
        }
        else
        {
            ifm_get_name_by_ifindex(ifindex, ifname);
            vty_out(vty, "    rmon collection index %d (invalid) %s"
                    "    dataSouce %s %s"
                    "    owner: %s %s",
                    rinfo->etherStatsIndex, VTY_NEWLINE, \
                    ifname, VTY_NEWLINE, \
                    rinfo->etherStatsOwner, VTY_NEWLINE);
        }
    }

    vty_out(vty,   "***  statistics  collection data end  *** %s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


DEFUN(show_rmon_if_stats,
      show_rmon_if_stats_cmd,
      "show rmon statistics",
      "show\n"
      "rmon\n"
      "statistics\n")
{
    struct route_node *rn;
    struct rmon_etherStatsGroup *rinfo;
    struct rmon_if_stats if_stats;
    u_int32_t ifindex = 0;
    char ifname[IFNET_NAMESIZE];
    struct rmon_master *rm = (struct rmon_master *)get_rmond_master_value();

    if (NULL == rm || NULL == rm->etherStats_table)
    {
        return CMD_WARNING;
    }

    for (rn = route_top(rm->etherStats_table); rn; rn = route_next(rn))
    {
        rinfo = (struct rmon_etherStatsGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue;
        }

        ifindex = rinfo->etherStatsDataSource[RMON_IFINDEX_LOC];

        if (rinfo->etherStatsStatus == VALID_STATUS)
        {
            memset(&if_stats, 0, sizeof(struct rmon_if_stats));

            rmon_interface_counter_curr_increment(ifindex, &if_stats);
            rmon_if_stats_2_rmon_etherStatsGroup(&if_stats, rinfo);
            ifm_get_name_by_ifindex(ifindex, ifname);
            vty_out(vty,  "    rmon collection index: %d %s"
                    "    dataSouce: %s %s"
                    "    owner: %s %s",
                    rinfo->etherStatsIndex, VTY_NEWLINE, \
                    ifname, VTY_NEWLINE, \
                    (char *)rinfo->etherStatsOwner, VTY_NEWLINE);

            uint64_t etherStatsDropEvents = 0;
            rmon_converter_struct_2_longlong(rinfo->etherStatsDropEvents, &etherStatsDropEvents);
            uint64_t etherStatsOctets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsOctets, &etherStatsOctets);
            uint64_t etherStatsPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts, &etherStatsPkts);
            uint64_t etherStatsBroadcastPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsBroadcastPkts, &etherStatsBroadcastPkts);
            uint64_t etherStatsMulticastPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsMulticastPkts, &etherStatsMulticastPkts);
            uint64_t etherStatsCRCAlignErrors = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsCRCAlignErrors, &etherStatsCRCAlignErrors);
            uint64_t etherStatsPkts64Octets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts64Octets, &etherStatsPkts64Octets);
            uint64_t etherStatsOversizePkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsOversizePkts, &etherStatsOversizePkts);
            uint64_t etherStatsPkts65to127Octets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts65to127Octets, &etherStatsPkts65to127Octets);
            uint64_t etherStatsJabbers = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsJabbers, &etherStatsJabbers);
            uint64_t etherStatsPkts128to255Octets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts128to255Octets, &etherStatsPkts128to255Octets);
            uint64_t etherStatsCollisions = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsCollisions, &etherStatsCollisions);
            uint64_t etherStatsPkts256to511Octets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts256to511Octets, &etherStatsPkts256to511Octets);
            uint64_t etherStatsFragments = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsFragments, &etherStatsFragments);
            uint64_t etherStatsPkts512to1023Octets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts512to1023Octets, &etherStatsPkts512to1023Octets);
            uint64_t etherStatsUndersizePkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsUndersizePkts, &etherStatsUndersizePkts);
            uint64_t etherStatsPkts1024to1518Octets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsPkts1024to1518Octets, &etherStatsPkts1024to1518Octets);
            uint64_t etherStatsUnicastPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsUnicastPkts, &etherStatsUnicastPkts);
            uint64_t etherStatsErrorPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsErrorPkts, &etherStatsErrorPkts);
            uint64_t etherStatsLossPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsLossPkts, &etherStatsLossPkts);
            uint64_t etherStatsTxOctets = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxOctets, &etherStatsTxOctets);
            uint64_t etherStatsTxPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxPkts, &etherStatsTxPkts);
            uint64_t etherStatsTxUnicastPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxUnicastPkts, &etherStatsTxUnicastPkts);
            uint64_t etherStatsTxBroadcastPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxBroadcastPkts, &etherStatsTxBroadcastPkts);
            uint64_t etherStatsTxMulticastPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxMulticastPkts, &etherStatsTxMulticastPkts);
            uint64_t etherStatsTxErrorPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxErrorPkts, &etherStatsTxErrorPkts);
            uint64_t etherStatsTxLossPkts = 0 ;
            rmon_converter_struct_2_longlong(rinfo->etherStatsTxLossPkts, &etherStatsTxLossPkts);

            vty_out(vty,   "          drop events %21llu %s"
                    "         stats octets %21llu %s"
                    "         stats packts %21llu %s"
                    "       broadcast pkts %21llu %s"
                    "       multicast pkts %21llu %s"
                    "           crc errors %21llu %s"
                    "       64 octets pkts %21llu %s"
                    "        overSize pkts %21llu %s"
                    "   65-127 octets pkts %21llu %s"
                    "        stats jabbers %21llu %s"
                    "  128-255 octets pkts %21llu %s"
                    "      stats collsions %21llu %s"
                    "  256-511 octets pkts %21llu %s"
                    "      stats fragments %21llu %s"
                    " 512-1023 octets pkts %21llu %s"
                    "       underSize pkts %21llu %s"
                    " 1024-max octets pkts %21llu %s"
                    "     unicast pkts rcv %21llu %s"
                    "         bad pkts rcv %21llu %s"
                    "     in discards pkts %21llu %s"
                    "    stats octets sent %21llu %s"
                    "      stats pkts sent %21llu %s"
                    "    unicast pkts sent %21llu %s"
                    "  broadcast pkts sent %21llu %s"
                    "  multicast pkts sent %21llu %s"
                    "        bad pkts sent %21llu %s"
                    "    out discards sent %21llu %s" ,
                    etherStatsDropEvents, VTY_NEWLINE,           \
                    etherStatsOctets, VTY_NEWLINE,               \
                    etherStatsPkts, VTY_NEWLINE,                 \
                    etherStatsBroadcastPkts, VTY_NEWLINE,        \
                    etherStatsMulticastPkts, VTY_NEWLINE,        \
                    etherStatsCRCAlignErrors, VTY_NEWLINE,       \
                    etherStatsPkts64Octets, VTY_NEWLINE,         \
                    etherStatsOversizePkts, VTY_NEWLINE,         \
                    etherStatsPkts65to127Octets, VTY_NEWLINE,    \
                    etherStatsJabbers, VTY_NEWLINE,              \
                    etherStatsPkts128to255Octets, VTY_NEWLINE,   \
                    etherStatsCollisions, VTY_NEWLINE,           \
                    etherStatsPkts256to511Octets, VTY_NEWLINE,   \
                    etherStatsFragments, VTY_NEWLINE,            \
                    etherStatsPkts512to1023Octets, VTY_NEWLINE,  \
                    etherStatsUndersizePkts, VTY_NEWLINE,        \
                    etherStatsPkts1024to1518Octets, VTY_NEWLINE, \
                    etherStatsUnicastPkts, VTY_NEWLINE,          \
                    etherStatsErrorPkts, VTY_NEWLINE,            \
                    etherStatsLossPkts, VTY_NEWLINE,             \
                    etherStatsTxOctets, VTY_NEWLINE,             \
                    etherStatsTxPkts, VTY_NEWLINE,               \
                    etherStatsTxUnicastPkts, VTY_NEWLINE,        \
                    etherStatsTxBroadcastPkts, VTY_NEWLINE,      \
                    etherStatsTxMulticastPkts, VTY_NEWLINE,      \
                    etherStatsTxErrorPkts, VTY_NEWLINE,          \
                    etherStatsTxLossPkts, VTY_NEWLINE
                   );

            vty_out(vty, "%s %s", CLI_SPLITER, VTY_NEWLINE);
        }
        else
        {
            ifm_get_name_by_ifindex(ifindex, ifname);
            vty_out(vty, "    rmon collection index %d (invalid) %s"
                    "    dataSouce %s %s"
                    "    owner: %s %s",
                    rinfo->etherStatsIndex, VTY_NEWLINE, \
                    ifname, VTY_NEWLINE, \
                    rinfo->etherStatsOwner, VTY_NEWLINE);
        }
    }

    vty_out(vty,   "***  statistics  collection data end  *** %s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

/*liuyf debug add */
DEFUN(show_rmon_if_phy_stats,
      show_rmon_if_phy_stats_cmd,
      "show rmon phystats",
      "show\n"
      "rmon\n"
      "phystats\n")
{
    struct rmon_if_stats if_stats;
    struct route_node *rn;
    struct rmon_etherStatsGroup *rinfo;
    char ifname[IFNET_NAMESIZE];
    u_int32_t ifindex = 0;
    struct rmon_master *rm  = (struct rmon_master *)get_rmond_master_value();

    if (!rm)
    {
        return CMD_WARNING;
    }

    for (rn = route_top(rm->etherStats_table); rn; rn = route_next(rn))
    {
        rinfo = (struct rmon_etherStatsGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue;
        }

        ifindex =  rinfo->etherStatsDataSource[RMON_IFINDEX_LOC] ;
        memset(&if_stats, 0, sizeof(struct rmon_if_stats));

        if (rinfo->etherStatsStatus == VALID_STATUS)
        {
            rmon_get_counter_from_sdk(ifindex, &if_stats);
            zlog_debug(ZLOG_LIB_DBG_PKG, " %s[%d]:'%s': ifindex [%d] \r\n", __FILE__, __LINE__, __func__ , ifindex);

            ifm_get_name_by_ifindex(ifindex, ifname);

            vty_out(vty,  "    ifname: %s\r\n"
                    "    rmon collection index: %d \r\n"
                    "    dataSouce: %s\r\n"
                    "    owner: %s\r\n",
                    ifname, rinfo->etherStatsIndex, \
                    ifname , \
                    (char *)rinfo->etherStatsOwner);

            uint64_t etherStatsDropEvents = 0 ;
            rmon_converter_struct_2_longlong(if_stats.drop_events, &etherStatsDropEvents);
            uint64_t etherStatsOctets = 0 ;
            rmon_converter_struct_2_longlong(if_stats.rx_bytes, &etherStatsOctets);
            uint64_t etherStatsPkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.rx_packets, &etherStatsPkts);
            uint64_t etherStatsBroadcastPkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.rx_broadcast, &etherStatsBroadcastPkts);
            uint64_t etherStatsMulticastPkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.rx_multicast, &etherStatsMulticastPkts);
            uint64_t etherStatsCRCAlignErrors = 0 ;
            rmon_converter_struct_2_longlong(if_stats.bad_crc , &etherStatsCRCAlignErrors);
            uint64_t etherStatsPkts64Octets = 0 ;
            rmon_converter_struct_2_longlong(if_stats.pkts_64_octets, &etherStatsPkts64Octets);
            uint64_t etherStatsOversizePkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.oversize_pkts_rcv, &etherStatsOversizePkts);
            uint64_t etherStatsPkts65to127Octets = 0 ;
            rmon_converter_struct_2_longlong(if_stats.pkts_65_127_octets, &etherStatsPkts65to127Octets);
            uint64_t etherStatsJabbers = 0 ;
            rmon_converter_struct_2_longlong(if_stats.jabber_pkts, &etherStatsJabbers);
            uint64_t etherStatsPkts128to255Octets = 0 ;
            rmon_converter_struct_2_longlong(if_stats.pkts_128_255_octets, &etherStatsPkts128to255Octets);
            uint64_t etherStatsCollisions = 0 ;
            rmon_converter_struct_2_longlong(if_stats.collisions, &etherStatsCollisions);
            uint64_t etherStatsPkts256to511Octets = 0 ;
            rmon_converter_struct_2_longlong(if_stats.pkts_256_511_octets, &etherStatsPkts256to511Octets);
            uint64_t etherStatsFragments = 0 ;
            rmon_converter_struct_2_longlong(if_stats.fragments_pkts, &etherStatsFragments);
            uint64_t etherStatsPkts512to1023Octets = 0 ;
            rmon_converter_struct_2_longlong(if_stats.pkts_512_1023_octets, &etherStatsPkts512to1023Octets);
            uint64_t etherStatsUndersizePkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.undersize_pkts, &etherStatsUndersizePkts);           
            uint64_t etherpkts_1519_max_octets = 0 ;
            rmon_converter_struct_2_longlong(if_stats.pkts_1519_max_octets, &etherpkts_1519_max_octets);
            uint64_t etherStatsPkts1024to1518Octets = 0 ;
            rmon_converter_struct_2_longlong(if_stats.pkts_1024_1518_octets, &etherStatsPkts1024to1518Octets);

            uint64_t etherStatsUnicastPkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.rx_ucast, &etherStatsUnicastPkts);
            uint64_t etherStatsErrorPkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.rx_errors, &etherStatsErrorPkts);
            uint64_t etherStatsLossPkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.rx_dropped, &etherStatsLossPkts);
            uint64_t etherStatsTxOctets = 0 ;
            rmon_converter_struct_2_longlong(if_stats.tx_bytes, &etherStatsTxOctets);
            uint64_t etherStatsTxPkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.tx_packets, &etherStatsTxPkts);
            uint64_t etherStatsTxUnicastPkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.tx_ucast, &etherStatsTxUnicastPkts);
            uint64_t etherStatsTxBroadcastPkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.tx_broadcast, &etherStatsTxBroadcastPkts);
            uint64_t etherStatsTxMulticastPkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.tx_multicast, &etherStatsTxMulticastPkts);
            uint64_t etherStatsTxErrorPkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.tx_errors, &etherStatsTxErrorPkts);
            uint64_t etherStatsTxLossPkts = 0 ;
            rmon_converter_struct_2_longlong(if_stats.tx_dropped, &etherStatsTxLossPkts);

            vty_out(vty,   "          drop events %21llu\r\n"
                    "         stats octets %21llu\r\n"
                    "         stats packts %21llu\r\n"
                    "       broadcast pkts %21llu\r\n"
                    "       multicast pkts %21llu\r\n"
                    "           crc errors %21llu\r\n"
                    "       64 octets pkts %21llu\r\n"
                    "        overSize pkts %21llu\r\n"
                    "   65-127 octets pkts %21llu\r\n"
                    "        stats jabbers %21llu\r\n"
                    "  128-255 octets pkts %21llu\r\n"
                    "      stats collsions %21llu\r\n"
                    "  256-511 octets pkts %21llu\r\n"
                    "      stats fragments %21llu\r\n"
                    " 512-1023 octets pkts %21llu\r\n"
                    "       underSize pkts %21llu\r\n"
                    " 1024-max octets pkts %21llu\r\n"
                    "     unicast pkts rcv %21llu\r\n"
                    "         bad pkts rcv %21llu \r\n"
                    "     in discards pkts %21llu\r\n"
                    "    stats octets sent %21llu \r\n"
                    "      stats pkts sent %21llu\r\n"
                    "    unicast pkts sent %21llu \r\n"
                    "  broadcast pkts sent %21llu\r\n"
                    "  multicast pkts sent %21llu \r\n"
                    "        bad pkts sent %21llu\r\n"
                    "    out discards sent %21llu \r\n",
                    etherStatsDropEvents , \
                    etherStatsOctets, \
                    etherStatsPkts, \
                    etherStatsBroadcastPkts, \
                    etherStatsMulticastPkts, \
                    etherStatsCRCAlignErrors, \
                    etherStatsPkts64Octets, \
                    etherStatsOversizePkts, \
                    etherStatsPkts65to127Octets, \
                    etherStatsJabbers, \
                    etherStatsPkts128to255Octets, \
                    etherStatsCollisions, \
                    etherStatsPkts256to511Octets, \
                    etherStatsFragments, \
                    etherStatsPkts512to1023Octets, \
                    etherStatsUndersizePkts, \
                    etherStatsPkts1024to1518Octets, \

                    etherStatsUnicastPkts, \
                    etherStatsErrorPkts, \
                    etherStatsLossPkts, \
                    etherStatsTxOctets, \
                    etherStatsTxPkts, \
                    etherStatsTxUnicastPkts, \
                    etherStatsTxBroadcastPkts, \
                    etherStatsTxMulticastPkts, \
                    etherStatsTxErrorPkts, \
                    etherStatsTxLossPkts
                   );
            vty_out(vty, "%s %s", CLI_SPLITER, VTY_NEWLINE);

        }
        else
        {
            ifm_get_name_by_ifindex(ifindex, ifname);

            vty_out(vty,   "    statsStatus(%d) invalid\n"
                    "    rmon collection index %d"
                    "    dataSouce %s\r\n"
                    "    owner: %s\r\n",
                    rinfo->etherStatsStatus, \
                    rinfo->etherStatsIndex, \
                    ifname, \
                    rinfo->etherStatsOwner);
        }
    }

    vty_out(vty,   "***  statistics  collection data end  ***\r\n");
    return CMD_SUCCESS;
}
/*liuyf debug add end*/

DEFUN(show_rmon_history_ctrl,
      show_rmon_history_ctrl_cmd,
      "show rmon history control",
      "show\n"
      "rmon\n"
      "history\n"
      "history control\n")
{
    struct route_node *rn;
    struct rmon_master *rm  = (struct rmon_master *)get_rmond_master_value();
    char ifname[IFNET_NAMESIZE];

    for (rn = route_top(rm->historyControl_table); rn; rn = route_next(rn))
    {
        struct rmon_HistoryControlGroup *rinfo;
        rinfo = (struct rmon_HistoryControlGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue;
        }

        if (rinfo->historyControlStatus != VALID_STATUS)
        {
            continue;
        }

        ifm_get_name_by_ifindex(
            (unsigned int)rinfo->historyControlDataSource[RMON_IFINDEX_LOC] , ifname);
        vty_out(vty,   "        history index: %d \r\n"
                "        ifname:%s \r\n"
                "        buckets requested: %d \r\n"
                "        buckets granted: %d \r\n"
                "        interval: %d \r\n"
                "        Owner: %s \r\n\n",
                rinfo->historyControlIndex,
                ifname ,
                rinfo->historyControlBucketsRequested,
                rinfo->historyControlBucketsGranted,
                rinfo->historyControlInterval,
                rinfo->historyControlOwner);
        vty_out(vty, "%s %s", CLI_SPLITER, VTY_NEWLINE);
    }

    //vty_out (vty, "        ***  history control end  ***\n");

    return CMD_SUCCESS;
}

DEFUN(show_rmon_history_data,
      show_rmon_history_data_cmd,
      "show rmon history data  hist-control-table-index <1-300>",
      "show\n"
      "rmon\n"
      "history\n"
      "history data\n"
      "history collection control index\n"
      "history collection control index value\n")
{
    struct route_node *rn;
    struct route_node *rn_s;
    struct rmon_master *rm  = (struct rmon_master *)get_rmond_master_value();
    char ifname[IFNET_NAMESIZE];

    struct rmon_EtherHistoryGroup *rinfo_s;
    u_int32_t index = 0;
    u_int32_t sample;

    index = atoi(argv[0]) ;

    for (rn = route_top(rm->historyControl_table); rn; rn = route_next(rn))
    {
        struct rmon_HistoryControlGroup *rinfo;
        rinfo = (struct rmon_HistoryControlGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue;
        }

        if (rinfo->historyControlIndex == index)
        {
            if (rinfo->historyControlStatus != VALID_STATUS)
            {
                vty_out(vty, "        history control status not valid(%d)\n",
                        rinfo->historyControlStatus);
            }
            else
            {
                ifm_get_name_by_ifindex(
                    (unsigned int)rinfo->historyControlDataSource[RMON_IFINDEX_LOC] ,
                    ifname);
                vty_out(vty,   "        history index: %d \r\n"
                        "        ifname:%s \r\n"
                        "        buckets requested: %d \r\n"
                        "        buckets granted: %d \r\n"
                        "        interval: %d \r\n"
                        "        Owner: %s \r\n",
                        rinfo->historyControlIndex,
                        ifname ,
                        rinfo->historyControlBucketsRequested,
                        rinfo->historyControlBucketsGranted,
                        rinfo->historyControlInterval,
                        rinfo->historyControlOwner);

                sample = 0xFFFFFFFF;

                for (rn_s = route_top(rinfo->historyStats_table); rn_s; rn_s = route_next(rn_s))
                {
                    rinfo_s = (struct rmon_EtherHistoryGroup *)rn_s->info;

                    if (rinfo_s == NULL)
                    {
                        continue;
                    }

                    if (rinfo_s->etherHistoryIndex == index)
                    {
                        if (rinfo_s->etherHistorySampleIndex <= sample)
                        {
                            sample = rinfo_s->etherHistorySampleIndex;
                            continue;
                        }
                    }
                }

                for (rn_s = route_top(rinfo->historyStats_table); rn_s; rn_s = route_next(rn_s))
                {
                    rinfo_s = (struct rmon_EtherHistoryGroup *)rn_s->info;

                    if (rinfo_s == NULL)
                    {
                        continue;
                    }

                    if (rinfo_s->etherHistoryIndex == index)
                    {
                        if (rinfo_s->etherHistorySampleIndex == sample)
                        {
                            break;
                        }
                    }
                }

                for (; rn_s; rn_s = route_next(rn_s))
                {
                    rinfo_s = (struct rmon_EtherHistoryGroup *)rn_s->info;

                    if (rinfo_s == NULL)
                    {
                        continue;
                    }

                    if (rinfo_s->etherHistoryIndex == index)
                    {
                        uint64_t etherStatsDropEvents = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryDropEvents, &etherStatsDropEvents);
                        uint64_t etherHistoryOctets = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryOctets, &etherHistoryOctets);
                        uint64_t etherHistoryPkts = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryPkts, &etherHistoryPkts);
                        uint64_t etherHistoryBroadcastPkts = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryBroadcastPkts, &etherHistoryBroadcastPkts);
                        uint64_t etherHistoryMulticastPkts = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryMulticastPkts, &etherHistoryMulticastPkts);
                        uint64_t etherHistoryCRCAlignErrors = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryCRCAlignErrors, &etherHistoryCRCAlignErrors);
                        uint64_t etherHistoryUndersizePkts = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryUndersizePkts, &etherHistoryUndersizePkts);
                        uint64_t etherHistoryOversizePkts = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryOversizePkts, &etherHistoryOversizePkts);
                        uint64_t etherHistoryFragments = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryFragments, &etherHistoryFragments);
                        uint64_t etherHistoryJabbers = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryJabbers, &etherHistoryJabbers);
                        uint64_t etherHistoryCollisions = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryCollisions, &etherHistoryCollisions);

                        ifm_get_name_by_ifindex(
                            (unsigned int)rinfo->historyControlDataSource[RMON_IFINDEX_LOC] ,
                            ifname);

                        vty_out(vty,   "    *** HistortIndex    %8d    dataSource:%s ***\r\n"
                                "    *** SampleIndex     %8d    Interval     %8d ***\r\n"
                                "    *** SampleStatT   %10d                   ***\r\n"
                                "    drop events %21llu      stats octets %21llu\r\n"
                                "   stats packts %21llu    broadcast pkts %21llu\r\n"
                                " multicast pkts %21llu        crc errors %21llu\r\n"
                                " underSize pkts %21llu     overSize pkts %21llu\r\n"
                                "stats fragments %21llu     stats jabbers %21llu\r\n"
                                //"stats collsions %21llu stats utilization %21d\r\n",
                                "stats collsions %21llu \r\n",
                                rinfo_s->etherHistoryIndex, ifname, \
                                rinfo_s->etherHistorySampleIndex, rinfo->historyControlInterval, \
                                rinfo_s->etherHistoryIntervalStart, \
                                etherStatsDropEvents,
                                etherHistoryOctets,
                                etherHistoryPkts,
                                etherHistoryBroadcastPkts,
                                etherHistoryMulticastPkts,
                                etherHistoryCRCAlignErrors,
                                etherHistoryUndersizePkts,
                                etherHistoryOversizePkts,
                                etherHistoryFragments,
                                etherHistoryJabbers,
                                etherHistoryCollisions
                                //0);
                                );
                        vty_out(vty, "%s %s", CLI_SPLITER, VTY_NEWLINE);
                    }
                }

                for (rn_s = route_top(rinfo->historyStats_table); rn_s; rn_s = route_next(rn_s))
                {
                    rinfo_s = (struct rmon_EtherHistoryGroup *)rn_s->info;

                    if (rinfo_s == NULL)
                    {
                        continue;
                    }

                    if (rinfo_s->etherHistoryIndex == index)
                    {
                        if (rinfo_s->etherHistorySampleIndex == sample)
                        {
                            break;
                        }

                        uint64_t etherStatsDropEvents = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryDropEvents, &etherStatsDropEvents);
                        uint64_t etherHistoryOctets = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryOctets, &etherHistoryOctets);
                        uint64_t etherHistoryPkts = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryPkts, &etherHistoryPkts);
                        uint64_t etherHistoryBroadcastPkts = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryBroadcastPkts, &etherHistoryBroadcastPkts);
                        uint64_t etherHistoryMulticastPkts = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryMulticastPkts, &etherHistoryMulticastPkts);
                        uint64_t etherHistoryCRCAlignErrors = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryCRCAlignErrors, &etherHistoryCRCAlignErrors);
                        uint64_t etherHistoryUndersizePkts = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryUndersizePkts, &etherHistoryUndersizePkts);
                        uint64_t etherHistoryOversizePkts = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryOversizePkts, &etherHistoryOversizePkts);
                        uint64_t etherHistoryFragments = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryFragments, &etherHistoryFragments);
                        uint64_t etherHistoryJabbers = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryJabbers, &etherHistoryJabbers);
                        uint64_t etherHistoryCollisions = 0 ;
                        rmon_converter_struct_2_longlong(rinfo_s->etherHistoryCollisions, &etherHistoryCollisions);

                        vty_out(vty,   "    *** HistortIndex:    %8d    dataSource:%s***\r\n"
                                "    *** SampleIndex:     %8d    Interval:     %8d ***\r\n"
                                "    *** SampleStatT:   %10d                   ***\r\n"
                                "    drop events %21llu      stats octets %21llu\r\n"
                                "   stats packts %21llu    broadcast pkts %21llu\r\n"
                                " multicast pkts %21llu        crc errors %21llu\r\n"
                                " underSize pkts %21llu     overSize pkts %21llu\r\n"
                                " tats fragments %21llu     stats jabbers %21llu\r\n"
                                //"stats collsions %21llu stats utilization %21d\r\n",
                                "stats collsions %21llu \r\n",
                                rinfo_s->etherHistoryIndex, ifname , \
                                rinfo_s->etherHistorySampleIndex, rinfo->historyControlInterval, \
                                rinfo_s->etherHistoryIntervalStart, \
                                etherStatsDropEvents,
                                etherHistoryOctets,
                                etherHistoryPkts,
                                etherHistoryBroadcastPkts,
                                etherHistoryMulticastPkts,
                                etherHistoryCRCAlignErrors,
                                etherHistoryUndersizePkts,
                                etherHistoryOversizePkts,
                                etherHistoryFragments,
                                etherHistoryJabbers,
                                etherHistoryCollisions
                                //0);
                                );
                        vty_out(vty, "%s %s", CLI_SPLITER, VTY_NEWLINE);
                    }
                }
            }

            /*over*/
            break;
        }
        else
        {
            continue;
        }
    }

    vty_out(vty, "        ***  history collection data end  ***\r\n");

    return CMD_SUCCESS;
}

DEFUN(show_rmon_event,
      show_rmon_event_cmd,
      "show rmon event",
      "show\n"
      "rmon\n"
      "event\n")
{
    struct route_node *rn;
    int h, m, s;
    struct rmon_master *rm  = (struct rmon_master *)get_rmond_master_value();

    for (rn = route_top(rm->event_table); rn; rn = route_next(rn))
    {
        struct rmon_EventGroup *rinfo;


        rinfo = (struct rmon_EventGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue;
        }

        if (rinfo->eventStatus == VALID_STATUS)
        {
            h = rinfo->eventLastTimeSent / 3600;
            m = (rinfo->eventLastTimeSent % 3600) / 60;
            s = rinfo->eventLastTimeSent % 60;

            vty_out(vty,   "        Event Index = %d \r\n"
                    "        Description: %s \r\n"
                    "        Last Time Sent: %02d:%02d:%02d \r\n"
                    "        Event Type: %d (2-LOG,3-TRAP,4-LOG & TRAP)\r\n"
                    "        Owner: %s \r\n",
                    rinfo->eventIndex, \
                    rinfo->eventDescription, \
                    h, m, s, \
                    rinfo->eventType, \
                    rinfo->eventOwner);

            if ((rinfo->eventType == RMON_EVENT_TRAP) ||
                    (rinfo->eventType == RMON_EVENT_LOG_TRAP))
            {
                vty_out(vty, "        Event community name: %s\r\n",
                        rinfo->eventCommunity);
            }

            vty_out(vty, "%s %s", CLI_SPLITER, VTY_NEWLINE);
        }
    }

    vty_out(vty, "        *** event table end ***\r\n");

    return CMD_SUCCESS;

}

DEFUN(show_rmon_alarm,
      show_rmon_alarm_cmd,
      "show rmon alarm",
      "show\n"
      "rmon\n"
      "alarm\n")
{
    struct route_node *rn;
    struct rmon_master *rm  = (struct rmon_master *)get_rmond_master_value();

    for (rn = route_top(rm->alarm_table); rn; rn = route_next(rn))
    {
        struct rmon_AlarmGroup *rinfo;
        char st[36] = {0};

        rinfo = (struct rmon_AlarmGroup *)rn->info;

        if (rinfo == NULL)
        {
            continue;
        }

        switch (rinfo->alarmStatus)
        {
            case VALID_STATUS :
                sprintf(st , "%s", "VALID");
                break;

            case CREATE_REQ_STATUS :
                sprintf(st , "%s", "CREATE_REQ_STATUS");
                break;

            case UNDER_CREATION_STATUS :
                sprintf(st , "%s", "UNDER_CREATION_STATUS");
                break;

            case INVALID_STATUS :
                sprintf(st , "%s", "INVALID_STATUS");
                break;

            default:
                sprintf(st , "%s", "INVALID ALARM STATUS");
                break;
        }

        uint64_t alarmValue = 0 ;
        rmon_converter_struct_2_longlong(rinfo->alarmValue, &alarmValue);
        uint64_t alarmRisingThreshold = 0 ;
        rmon_converter_struct_2_longlong(rinfo->alarmRisingThreshold, &alarmRisingThreshold);
        uint64_t alarmFallingThreshold = 0 ;
        rmon_converter_struct_2_longlong(rinfo->alarmFallingThreshold, &alarmFallingThreshold);

        vty_out(vty,   "                    alarm Index = %d \r\n"
                "                 alarm Interval = %d \r\n"
                "                alarm Variable  = %d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d\r\n"
                "              alarm Sample Type = %d (1-Absolute, 2-Delta)\r\n"
                "             alarm startup Type = %d (1-Rising, 2-Falling, 3 R and F)\r\n"
                "                    alarm Value = %21llu\r\n"
                "         alarm Rising Threshold = %21llu\r\n"
                "        alarm Falling Threshold = %21llu\r\n"
                "             alarm Rising Event = %d \r\n"
                "            alarm Falling Event = %d \r\n"
                "                    alarm Owner : %s \r\n"
                "                   alarm status = %d (%s)\r\n",
                (int)rinfo->alarmIndex, \
                (int)rinfo->alarmInterval, \
                (int)rinfo->alarmVariable[0], (int)rinfo->alarmVariable[1], (int)rinfo->alarmVariable[2], \
                (int)rinfo->alarmVariable[3], (int)rinfo->alarmVariable[4], (int)rinfo->alarmVariable[5], \
                (int)rinfo->alarmVariable[6], (int)rinfo->alarmVariable[7], (int)rinfo->alarmVariable[8], \
                (int)rinfo->alarmVariable[9], (int)rinfo->alarmVariable[10], (int)rinfo->alarmVariable[11], \
                rinfo->alarmSampleType, \
                rinfo->alarmStartupAlarm, \
                alarmValue,
                alarmRisingThreshold,
                alarmFallingThreshold,
                rinfo->alarmRisingEventIndex, \
                rinfo->alarmFallingEventIndex, \
                rinfo->alarmOwner, \
                rinfo->alarmStatus, st);

        vty_out(vty, "%s %s", CLI_SPLITER, VTY_NEWLINE);

    }

    vty_out(vty, "        *** alarm table end ***\r\n");

    return CMD_SUCCESS;
}


int
rmon_config_write(struct vty *vty)
{
    struct rmon_master *rm = NULL;
    u_int32_t write = 0;
    u_int32_t statsIndex;
    struct route_node *rn;
    char ifname[255] = {0} ;

    rm  = (struct rmon_master *)get_rmond_master_value();

    vty_out(vty, "rmon%s", VTY_NEWLINE);

    if (NULL != rm->etherStats_table)
    {
        for (rn = route_top(rm->etherStats_table); rn; rn = route_next(rn))
        {
            struct rmon_etherStatsGroup *stats;
            stats = rn->info;

            if (!stats)
            {
                continue;
            }

            ifm_get_name_by_ifindex(stats->etherStatsDataSource[RMON_IFINDEX_LOC], ifname);
            /*use index fine ifp*/
            vty_out(vty, "  rmon collection %s statistics %d owner %s \n",
                    ifname, stats->etherStatsIndex, stats->etherStatsOwner);
        }
    }

    if (NULL != rm->historyControl_table)
    {
        for (rn = route_top(rm->historyControl_table); rn; rn = route_next(rn))
        {
            struct rmon_HistoryControlGroup *hist_ctrl;

            hist_ctrl = rn->info;

            if (!hist_ctrl)
            {
                continue;
            }

            ifm_get_name_by_ifindex(hist_ctrl->historyControlDataSource[RMON_IFINDEX_LOC], ifname);

            vty_out(vty,  "  rmon collection  %s history %d buckets %d interval %d owner %s \n",
                    ifname, hist_ctrl->historyControlIndex, hist_ctrl->historyControlBucketsGranted,
                    hist_ctrl->historyControlInterval, hist_ctrl->historyControlOwner);

        }
    }

    /*liuyf add for wait IMI->NSM->RMON end*/

    if (NULL != rm->event_table)
    {

        for (rn = route_top(rm->event_table); rn; rn = route_next(rn))
        {
            struct rmon_EventGroup *event;
            event = rn->info;

            if (NULL == event)
            {
                continue;
            }

            switch (event->eventType)
            {
                case RMON_EVENT_LOG_TRAP:
                    vty_out(vty,  "  rmon event %d log-trap description %s community %s owner %s \n",
                            event->eventIndex, event->eventDescription,
                            event->eventCommunity, event->eventOwner);
                    write++;
                    break;

                case RMON_EVENT_TRAP:
                    vty_out(vty,  "  rmon event %d trap description %s community %s owner %s \n",
                            event->eventIndex, event->eventDescription,
                            event->eventCommunity, event->eventOwner);

                    write++;
                    break;

                case RMON_EVENT_LOG:
                    vty_out(vty,  "  rmon event %d log description %s owner %s \n",
                            event->eventIndex, event->eventDescription,
                            event->eventOwner);
                    write++;
                    break;

                default:
                    if (CHECK_FLAG(event->eventConf, RMON_EVENT_CONF_DESCRIPTION)
                            && CHECK_FLAG(event->eventConf, RMON_EVENT_CONF_OWNER))
                        vty_out(vty,  "  rmon event %d description %s owner %s \n",
                                event->eventIndex, event->eventDescription,
                                event->eventOwner);
                    else if (CHECK_FLAG(event->eventConf, RMON_EVENT_CONF_DESCRIPTION))
                        vty_out(vty,  "  rmon event %d description %s \n",
                                event->eventIndex, event->eventDescription);
                    else if (CHECK_FLAG(event->eventConf, RMON_EVENT_CONF_OWNER))
                        vty_out(vty,  "  rmon event %d owner %s \n",
                                event->eventIndex, event->eventOwner);
                    else
                    {
                        vty_out(vty,  "  rmon event %d \n", event->eventIndex);
                    }

                    write++;
                    break;
            }
        }
    }

    /* Alarm show */
    if (NULL != rm->alarm_table)
    {
        char *alarm_type_str;
        char alarmVariableWord[RMON_ALARM_VAR_WORD_LENGTH + 1];

        for (rn = route_top(rm->alarm_table); rn; rn = route_next(rn))
        {
            struct rmon_AlarmGroup *alarm;
            char startupStr[64] = {0} ;
            alarm = rn->info;

            if (!alarm)
            {
                continue;
            }


            /*trans alarm monitor object*/
            alarm_type_str = strchr(alarm->alarmVariableWord, '.');

            if (alarm_type_str == NULL)
            {
                return RESULT_ERROR;
            }

            memset(alarmVariableWord, '\0', (RMON_ALARM_VAR_WORD_LENGTH + 1));
            memcpy(alarmVariableWord, alarm->alarmVariableWord, ((int)alarm_type_str - (int)(alarm->alarmVariableWord)));

            statsIndex = atoi(++alarm_type_str);

            if (1 == alarm->alarmStartupAlarm)
            {
                snprintf(startupStr, 64 , "%s" , "rising");
            }
            else if (2 == alarm->alarmStartupAlarm)
            {
                snprintf(startupStr, 64 , "%s" , "falling");
            }
            else
            {
                snprintf(startupStr, 64 , "%s" , "risingAndFalling");
            }

            if (CHECK_FLAG(alarm->alarmConf, RMON_ALARM_OWNER_CONF))
                vty_out(vty,  "  rmon alarm %d statistics %d type %s interval %d %s rising-threshold %d "
                        "event %d falling-threshold %d event %d alarmstartup %s "
                        "owner %s \n",
                        alarm->alarmIndex, statsIndex, alarmVariableWord,
                        alarm->alarmInterval,
                        (alarm->alarmSampleType == 1 ? "absolute" : "delta"),
                        alarm->alarmRisingThreshold.l[1],
                        alarm->alarmRisingEventIndex,
                        alarm->alarmFallingThreshold.l[1],
                        alarm->alarmFallingEventIndex,
                        startupStr,
                        alarm->alarmOwner);
            else
                vty_out(vty, "  rmon alarm %d statistics %d type %s interval %d %s rising-threshold %d "
                        "event %d falling-threshold %d event %d alarmstartup %s \n" ,
                        alarm->alarmIndex, statsIndex, alarmVariableWord,
                        alarm->alarmInterval,
                        (alarm->alarmSampleType == 1 ? "absolute" : "delta"),
                        alarm->alarmRisingThreshold.l[1],
                        alarm->alarmRisingEventIndex,
                        alarm->alarmFallingThreshold.l[1],
                        alarm->alarmFallingEventIndex,
                        startupStr);

            write++;
        }

    }

    //return write;
    return 0;
}

void
rmon_cli_init()
{
    install_element(CONFIG_NODE, &rmon_conf_debug_cmd, CMD_LOCAL);
    install_node(&rmon_node, rmon_config_write);
    install_default(RMON_NODE);

    install_element(CONFIG_NODE, &rmon_mode_enable_cmd, CMD_SYNC);
    install_element(RMON_NODE, &rmon_clear_if_counters_cmd, CMD_SYNC);
    install_element(RMON_NODE, &rmon_if_collection_stats_cmd, CMD_SYNC);
    install_element(RMON_NODE, &show_rmon_if_stats_cmd, CMD_LOCAL);
    install_element(RMON_NODE, &show_rmon_spec_if_stats_cmd, CMD_LOCAL);
//    install_element(RMON_NODE, &show_rmon_if_base_stats_cmd);
    install_element(RMON_NODE, &show_rmon_if_phy_stats_cmd, CMD_LOCAL);

    install_element(RMON_NODE, &no_rmon_if_collection_stats_cmd, CMD_SYNC);
    install_element(RMON_NODE, &rmon_if_collection_history_cmd, CMD_SYNC);
    install_element(RMON_NODE, &no_rmon_if_collection_history_cmd, CMD_SYNC);
    install_element(RMON_NODE, &rmon_event_log_cmd, CMD_SYNC);

    install_element(RMON_NODE, &no_rmon_event_cmd, CMD_SYNC);
    install_element(RMON_NODE, &rmon_alarm_cmd, CMD_SYNC);
    install_element(RMON_NODE, &no_rmon_alarm_cmd, CMD_SYNC);

    install_element(RMON_NODE, &show_rmon_history_ctrl_cmd, CMD_LOCAL);
    install_element(RMON_NODE, &show_rmon_history_data_cmd, CMD_LOCAL);
    install_element(RMON_NODE, &show_rmon_event_cmd, CMD_LOCAL);
    install_element(RMON_NODE, &show_rmon_alarm_cmd, CMD_LOCAL);
}


