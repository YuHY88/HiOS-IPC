/******************************************************************************
 * Filename: rmond_api.h
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

#ifndef __ZEBOS_RMON_INSTR_H__
#define __ZEBOS_RMON_INSTR_H__

#include "table.h"
#include "rmon_config.h"

/*
  RMON GET, SET CODE

  This module declares the instrumentation code to the RMON
*/

#define RESULT_OK                  0
#define RESULT_ERROR              -1

#define RMON_API_SET_SUCCESS       0
#define RMON_API_SET_FAILURE      -1

#define RMON_NONE                  0
#define RMON_NATIVE                1
#define RMON_PROMISCUOUS           2

#define RMON_INDEX_MIN             1
#define RMON_INDEX_MAX             65535

#define RMON_STATS_INDEX_MIN       1
#define RMON_STATS_INDEX_MAX       100

#define RMON_HIST_INDEX_MIN        1
#define RMON_HIST_INDEX_MAX        300

#define RMON_ALARM_INDEX_MIN       1
#define RMON_ALARM_INDEX_MAX       600

#define RMON_EVENT_INDEX_MIN       1
#define RMON_EVENT_INDEX_MAX       1200

#define RMON_INTERVAL_MIN          1
#define RMON_INTERVAL_MAX          86400

#define RMON_MIN_THRESHOLD         1
#define RMON_MAX_THRESHOLD         2147483647

#define RMON_ALARM_ABS             1
#define RMON_ALARM_DELTA           2

#define ETHER_STATS_INDEX_POS      11

#define RMOB_ALARM_OID_INDEX_TYPE   10
#define RMOB_ALARM_OID_INDEX_STATS  11

#define ETHER_STATS_BUF_MAX        100

#define RMON_ALARM_STARTUP_RISING             1
#define RMON_ALARM_STARTUP_FALLING            2
#define RMON_ALARM_STARTUP_RISINGANDFALLING   3

#define RMON_ALARM_STARTUP_NOT_MET            0
#define RMON_ALARM_STARTUP_MET                1

#define RMON_ALARM_NOT_MET                    0
#define RMON_ALARM_MET                        1

#define RMON_IFINDEX_OID_VALUE                1,3,6,1,2,1,2,2,1,1

#define PREFIX_RMON_SET(P,I)                                                 \
   do {                                                                      \
       memset ((P), 0, sizeof (struct prefix));                              \
       (P)->family = AF_INET;                                                \
       (P)->prefixlen = 32;                                                  \
       (P)->u.prefix4.s_addr = htonl (I);                                    \
   } while (0)

#define RMON_INFO_SET(R,V)                                                   \
   do {                                                                      \
      (R)->info = (V);                                                       \
   } while (0)

#define RMON_INFO_UNSET(R)                                                   \
   do {                                                                      \
     (R)->info = NULL;                                                       \
   } while (0)


int rmon_handle_snmp_msg(struct ipc_mesg_n *pmsg) ;

// ------------------------------------------
int                                 init_rmon_master(void);
struct rmon_master                 *get_rmond_master_value(void);

int   rmon_api_clear_all_counters();

int register_if_msg() ;

int rmon_if_up(uint32_t ifindex) ;

int rmon_if_delete(uint32_t ifindex) ;

void rmon_converter_struct_2_longlong(ut_int64_t in_data, uint64_t *out_data);
void rmon_converter_longlong_2_struct(uint64_t in_data, ut_int64_t *out_data);
ut_int64_t rmon_struct_value_add(ut_int64_t data1, ut_int64_t data2);
ut_int64_t rmon_struct_value_sub(ut_int64_t data1, ut_int64_t data2);

int rmon_get_increment(struct rmon_if_stats *new_stats, \
                       struct rmon_if_stats *lst_stats, \
                       struct rmon_if_stats *pincrement);

int rmon_get_counter_from_sdk(uint32_t ifindex,
                              struct rmon_if_stats *);

int rmon_if_stats_2_rmon_etherStatsGroup(struct rmon_if_stats *if_stats ,
        struct rmon_etherStatsGroup *rinfo) ;

s_int32_t rmon_event_entry_add(u_int32_t index, int type,
                               char *description, char *community,
                               char *ownername) ;

s_int32_t rmon_event_entry_remove(u_int32_t index) ;

s_int32_t rmon_alarm_entry_add(u_int32_t index, int type, char *type_name, int stats_index,
                               u_int32_t interval, u_int32_t alarmSampleType,
                               s_int32_t RisingValue,
                               s_int32_t FallingValue, u_int32_t rising_event,
                               u_int32_t falling_event, char *ownername,
                               u_int8_t AlarmStartUpAlarm) ;

struct ifm_counter *rmon_get_counter_from_sdk_msg(uint32_t ifindex, int module_id) ;

s_int32_t rmon_coll_stats_entry_remove_force(u_int32_t index) ;

s_int32_t rmon_event_snmp_update_status(u_int32_t index, u_int32_t status) ;

s_int32_t rmon_set_alarm_variableword(struct rmon_AlarmGroup *rmon_alarm ,
                                      int ether_stats_var) ;


/* if counter */
struct rmon_interface              *rmon_interface_base_counter(uint32_t ifindex);
int                                 rmon_interface_base_counter_curr_update(struct rmon_interface *);
int                                 rmon_interface_base_counter_hist_update(struct rmon_interface *);
int                                 rmon_interface_base_counter_get(uint32_t ifindex, struct rmon_interface *);

int                                 rmon_interface_counter_curr_increment(uint32_t ifindex, struct rmon_if_stats *);
int                                 rmon_interface_counter_hist_increment(uint32_t ifindex, struct rmon_if_stats *);

/* statis */
s_int32_t                           rmon_coll_stats_entry_status(u_int32_t index);
struct rmon_etherStatsGroup        *rmon_coll_stats_entry_info_lookup(u_int32_t index);
s_int32_t                           rmon_coll_stats_entry_add(u_int32_t ifindex, u_int32_t index, char *ownername);
s_int32_t                           rmon_coll_stats_entry_remove(u_int32_t index);
s_int32_t                           rmon_coll_stats_snmp_update_status(u_int32_t index, u_int32_t status);

/* history */
s_int32_t                           rmon_coll_history_entry_status(u_int32_t index);
struct rmon_HistoryControlGroup    *rmon_coll_history_entry_info_lookup(u_int32_t index);
s_int32_t                           rmon_coll_history_entry_active(struct rmon_HistoryControlGroup *);
s_int32_t                           rmon_coll_history_entry_inactive(struct rmon_HistoryControlGroup *);
s_int32_t                           rmon_coll_history_entry_add(u_int32_t ifindex, u_int32_t index, \
        u_int32_t bucketno, u_int32_t interval, char *ownername);
s_int32_t                           rmon_coll_history_entry_remove(u_int32_t index);
s_int32_t                           rmon_coll_history_snmp_update_status(u_int32_t index, u_int32_t status);

/* event */
s_int32_t                           rmon_event_entry_status(u_int32_t index);
struct rmon_EventGroup             *rmon_event_entry_info_lookup(u_int32_t index);

/* alarm */
s_int32_t                           rmon_alarm_entry_status(u_int32_t index);
struct rmon_AlarmGroup             *rmon_alarm_entry_info_lookup(u_int32_t index);
s_int32_t                           rmon_alarm_entry_active(struct rmon_AlarmGroup *);
s_int32_t                           rmon_alarm_entry_inactive(struct rmon_AlarmGroup *);

s_int32_t                           rmon_alarm_index_remove(u_int32_t index);

int                                 rmon_alarm_get_stats_type(char *type_name);
// ------------------------------------------

int rmon_snmp_etherStatsTable_get_bulk(struct ipc_msghdr_n *phdr, u_int32_t *stat_Index) ;

int rmon_snmp_historyControlTable_get_bulk(struct ipc_msghdr_n *phdr, u_int32_t *ctr_Index) ;

int rmon_snmp_etherHistoryTable_get_bulk(struct ipc_msghdr_n *phdr, u_int32_t *sample_Index) ;

int rmon_snmp_alarmTable_get_bulk(struct ipc_msghdr_n *phdr, u_int32_t *alarm_Index) ;

int rmon_snmp_eventTable_get_bulk(struct ipc_msghdr_n *phdr, u_int32_t *event_Index) ;

void rmon_snmp_table_info_get(struct ipc_mesg_n *pmsg) ;

s_int32_t
rmon_check_stats_group_in_alarm_group(u_int32_t etherStatsIndex);

s_int32_t
rmon_check_event_group_in_alarm_group(u_int32_t eventIndex);


struct rmon_etherStatsGroup *
rmon_ether_stats_lookup_by_id(u_int32_t *, int);

#endif
