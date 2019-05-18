/*
*       define rmon common function and structure
*
*/

#ifndef HIOS_RMON_COMMON_H
#define HIOS_RMON_COMMON_H


#include <lib/types.h>
#include <lib/inet_ip.h>
#include <lib/msg_ipc_n.h>
#include "fifo.h"
#include "thread.h"
#include <sys/types.h>

#include <snmp/include/asn1.h>


#define RMON_etherStatsTable                 1
#define RMON_historyControlTable           2
#define RMON_etherHistoryTable              3
#define RMON_alarmTable                           4
#define RMON_eventTable                          5

#define ETHER_STATS_OID_SIZE          10



typedef union {
        u_int8_t  c[8];
        u_int16_t s[4];
        u_int32_t l[2];
        u_int64_t ll;
} ut_int64_t;   

typedef union {
        s_int8_t  c[8];
        s_int16_t s[4];
        s_int32_t l[2];
} st_int64_t;   

#define bool_t char 


#define RMON_NO_ALARM                           2
#define RMON_RISE_ALARM                         1
#define RMON_FALL_ALARM                         0

#define RISING_ALARM                 1
#define FALLING_ALARM                2
#define RISING_OR_FALLING_ALARM      3

#define ETHER_STATS_OID_SIZE          10



#define RMON_MODE_DEFAULT                      0
#define RMON_STATUS_VALID                      1
#define RMON_STATUS_CREATEREQUEST              2
#define RMON_STATUS_UNDERCREATION              3
#define RMON_STATUS_INVALID                    4
#define RMON_STATUS_NONEXISTENT                5

#define RMON_ERR_BASE                      -100
#define RMON_ERR_NOMEM                    (RMON_ERR_BASE + 1)

#define RMON_MODE_DEFAULT                      0

#define VALID_STATUS                    1
#define CREATE_REQ_STATUS               2
#define UNDER_CREATION_STATUS           3
#define INVALID_STATUS                  4

#define RMON_EVENT_NONE                        1 
#define RMON_EVENT_LOG                         2
#define RMON_EVENT_TRAP                        3
#define RMON_EVENT_LOG_TRAP                    4
#define RMON_DATA_SOURCE_MAXSIZE               11
#define RMON_IFINDEX_LOC                       10
/* Minimum and Maximum Allowable values for various paramters */
#define RMON_ETHER_STATS_                      1
#define RMON_HISTORY_CONTROL_                  1
#define RMON_ALARM_VAR_WORD_LENGTH             32
#define RMON_EVENT_                            7
#define RMON_COMM_LENGTH                      127
#define RMON_DESCR_LENGTH                     255
#define RMON_OWNER_NAME_SIZE                  127

/* History control table defaults */
#define RMON_HISTORY_CTRL_BR_DEFAULT                        50 
#define RMON_HISTORY_CTRL_BG_DEFAULT                        50
#define RMON_HISTORY_CTRL_SAMPLE_INTERVAL_DEFAULT           1800  /* In secs */

/* Alarm table defaults */
#define RMON_ALARM_INTERVAL_DEFAULT                         10

/* EtherStats Variable Strings */
#define ETHERSTATSINDEXSTR                     "etherStatsIndex"
#define ETHERSTATSDSRCSTR                      "etherStatsDataSource"
#define ETHERSTATSDROPEVENTSVSTR               "etherStatsDropEvents"
#define ETHERSTATSOCTETSSTR                    "etherStatsOctets"
#define ETHERSTATSPKTSSTR                      "etherStatsPkts"
#define ETHERSTATSBDCPKTSSTR                   "etherStatsBroadcastPkts"
#define ETHERSTATSMCPKTSSTR                    "etherStatsMulticastPkts"
#define ETHERSTATSCRCALIGNERRSTR               "etherStatsCRCAlignErrors"
#define ETHERSTATSUSZPKTSSTR                   "etherStatsUndersizePkts"
#define ETHERSTATSOVSZPKTSSTR                  "etherStatsOversizePkts"
#define ETHERSTATSFRGSSTR                      "etherStatsFragments"
#define ETHERSTATSJABBERSSTR                   "etherStatsJabbers"
#define ETHERSTATSCOLLISIONSSTR                "etherStatsCollisions"
#define ETHERSTATSPKT64OCTSTR                  "etherStatsPkts64Octets"
#define ETHERSTATSPKT65TO127OCTSTR             "etherStatsPkts65to127Octets"
#define ETHERSTATSPKT128TO255OCTSTR            "etherStatsPkts128to255Octets"
#define ETHERSTATSPKT256TO511OCTSTR            "etherStatsPkts256to511Octets"
#define ETHERSTATSPKT512TO1023OCTSTR           "etherStatsPkts512to1023Octets"
#define ETHERSTATSPKT1024TO15180OCTSTR         "etherStatsPkts1024to1518Octets"
/* liuyf add for extern rmon statistic group */
#define ETHERSTATSUNICASTPKTSSTR               "etherStatsUnicastPkts"
#define ETHERSTATSBADPKTSSTR                   "etherStatsErrorPkts"
#define ETHERSTATSINDISCARDPKTSSTR             "etherStatsLossPkts"
#define ETHERSTATSSENTOCTETSSTR                "etherStatsTxOctets"
#define ETHERSTATSSENTPKTSSTR                  "etherStatsTxPkts"
#define ETHERSTATSSENTUNICASTPKTSSTR           "etherStatsTxUnicastPkts"
#define ETHERSTATSSENTBROADCASTPKTSSTR         "etherStatsTxBroadcastPkts"
#define ETHERSTATSSENTMULTICASTPKTSSTR         "etherStatsTxMulticastPkts"
#define ETHERSTATSSENTBADPKTSSTR               "etherStatsTxErrorPkts"
#define ETHERSTATSSENTDISCARDPKTSSTR           "etherStatsTxLossPkts"
/* liuyf add end */
#define ETHERSTATSOWNERSTR                     "etherStatsOwner"
#define ETHERSTATSSTATUSSTR                    "etherStatsStatus"



#define ETHERSTATSINDEX                         1
#define ETHERSTATSDATASOURCE                    2
#define ETHERSTATSDROPEVENTS                    3
#define ETHERSTATSOCTETS                        4
#define ETHERSTATSPKTS                          5
#define ETHERSTATSBROADCASTPKTS                 6
#define ETHERSTATSMULTICASTPKTS                 7
#define ETHERSTATSCRCALIGNERRORS                8
#define ETHERSTATSUNDERSIZEPKTS                 9
#define ETHERSTATSOVERSIZEPKTS                  10
#define ETHERSTATSFRAGMENTS                     11
#define ETHERSTATSJABBERS                       12
#define ETHERSTATSCOLLISIONS                    13
#define ETHERSTATSPKTS64OCTETS                  14
#define ETHERSTATSPKTS65TO127OCTETS             15
#define ETHERSTATSPKTS128TO255OCTETS            16
#define ETHERSTATSPKTS256TO511OCTETS            17
#define ETHERSTATSPKTS512TO1023OCTETS           18
#define ETHERSTATSPKTS1024TO1518OCTETS          19
#define ETHERSTATSOWNER                         20
//#define ETHERSTATSSTATUS                        21

/* liuyf add for extern rmon statistic group */
#define ETHERSTATSUNICASTPKTS                   22
#define ETHERSTATSBADPKTS                       23
#define ETHERSTATSINDISCARDPKTS                 24
#define ETHERSTATSSENTOCTETS                    25
#define ETHERSTATSSENTPKTS                      26
#define ETHERSTATSSENTUNICASTPKTS               27
#define ETHERSTATSSENTBROADCASTPKTS             28
#define ETHERSTATSSENTMULTICASTPKTS             29
#define ETHERSTATSSENTBADPKTS                   30
#define ETHERSTATSSENTDISCARDPKTS               31

#define ETHERSTATSSTATUS                        32


/* Macros used to find out whether the user has given etherStatsEntry.x.x format or etherStatsPkts.x format */
#define RMON_ETHER_STATS_ENTRY                 1
#define RMON_ETHER_STATS_VARIABLE              2

struct rmon_if_stats
{
    u_int32_t ifindex;
    ut_int64_t rx_packets;   /* total packets received       */
    ut_int64_t tx_packets;   /* total packets transmitted    */
    ut_int64_t rx_bytes;     /* total bytes received         */
    ut_int64_t tx_bytes;     /* total bytes transmitted      */
    ut_int64_t rx_errors;    /* error packets received       */
    ut_int64_t tx_errors;    /* error packet transmited      */
    ut_int64_t rx_dropped;   /* drop packets received        */
    ut_int64_t tx_dropped;   /* drop packets transmitted     */
    ut_int64_t rx_multicast; /* multicast packets received   */
    ut_int64_t tx_multicast; /* multicast packets transmitted*/
    ut_int64_t rx_broadcast; /* broadcast packets received   */
    ut_int64_t tx_broadcast; /* broadcast packets transmitted*/
    ut_int64_t rx_ucast; /* ucast packets received   */
    ut_int64_t tx_ucast; /* ucast packets transmitted*/
	ut_int64_t rx_nucast;/*nucast packets received */
	ut_int64_t tx_nucast;/*nucast packets transmitted*/
	ut_int64_t rx_uknownprotos;/*unknown protos received*/
    ut_int64_t undersize_pkts;
    ut_int64_t oversize_pkts;
    ut_int64_t bad_crc;
    ut_int64_t good_octets_rcv;     /*total good octets received*/
    ut_int64_t mac_transmit_err;    /*mac transmit error*/
    ut_int64_t good_pkts_rcv;       /*good packets received*/
	ut_int64_t mc_pkts_rcv;         /*mc packets received*/
	ut_int64_t pkts_64_octets;      /*packets 64 octets*/
	ut_int64_t pkts_65_127_octets;  /*packets 65 to 127 octets*/
	ut_int64_t pkts_128_255_octets; /*packets 128 to 255 octets*/
	ut_int64_t pkts_256_511_octets; /*packets 256 to 511 octets*/
	ut_int64_t pkts_512_1023_octets;/*packets 512 to 1023 octets*/
	ut_int64_t pkts_1024_max_octets;
	ut_int64_t good_octets_sent;    /*good octets sent*/
	ut_int64_t good_pkts_sent;      /*good packets sent*/
    ut_int64_t excessive_collisions;/*collisions*/
	ut_int64_t mc_pkts_sent;        /*mc packets sent*/
	ut_int64_t drop_events;         /*drop events*/
	ut_int64_t late_collisions;     /*late collisions*/
	ut_int64_t in_discards;         /*discard received*/
	ut_int64_t out_discards;        /*discard transmitted*/
	ut_int64_t brdc_pkts_rcv;
	ut_int64_t bad_octets_rcv;      /*bad octets received*/
	ut_int64_t brdc_pkts_sent;
    ut_int64_t fragments_pkts;
    ut_int64_t jabber_pkts;
    ut_int64_t collisions;
	ut_int64_t unic_pkts_rcv;
	ut_int64_t unic_pkts_sent;
    ut_int64_t mac_rcv_error;
	ut_int64_t bad_pkts_sent;
    ut_int64_t stats_pkts;
    ut_int64_t stats_octets;
    ut_int64_t stats_multicast;
	ut_int64_t stats_broadcast;
    ut_int64_t pkts_1024_1518_octets;
    ut_int64_t pkts_1519_1522_octets;
    ut_int64_t pkts_1523_2047_octets;
    ut_int64_t pkts_2048_4095_octets;
	ut_int64_t pkts_4096_9216_octets;
	ut_int64_t pkts_9217_16383_octets;
    ut_int64_t oversize_pkts_rcv;
	ut_int64_t pkts_1519_max_octets;

    ut_int64_t unrecog_mac_cntr_rcv;
    ut_int64_t bad_pkts_rcv;
    ut_int64_t fc_sent;
    ut_int64_t good_fc_rcv;
    ut_int64_t bad_fc_rcv;
   
};


struct rmon_etherStatsGroup 
{
  u_int32_t etherStatsIndex;
#define RMON_ETHER_STATS_DATASOURCE      (1 << 0)
#define RMON_ETHER_STATS_OWNER           (1 << 1)
#define RMON_ETHER_STATS_STATUS          (1 << 2)

  oid       etherStatsDataSource[RMON_DATA_SOURCE_MAXSIZE];
  ut_int64_t etherStatsDropEvents;
  ut_int64_t etherStatsOctets;
  ut_int64_t etherStatsPkts;
  ut_int64_t etherStatsBroadcastPkts;
  ut_int64_t etherStatsMulticastPkts;
  ut_int64_t etherStatsCRCAlignErrors;
  ut_int64_t etherStatsUndersizePkts;
  ut_int64_t etherStatsOversizePkts;
  ut_int64_t etherStatsFragments;
  ut_int64_t etherStatsJabbers;
  ut_int64_t etherStatsCollisions;
  ut_int64_t etherStatsPkts64Octets;
  ut_int64_t etherStatsPkts65to127Octets;
  ut_int64_t etherStatsPkts128to255Octets;
  ut_int64_t etherStatsPkts256to511Octets;
  ut_int64_t etherStatsPkts512to1023Octets;
  ut_int64_t etherStatsPkts1024to1518Octets;
  /* liuyf add for extern rmon statistic group */
  ut_int64_t etherStatsUnicastPkts;
  ut_int64_t etherStatsErrorPkts;
  ut_int64_t etherStatsLossPkts;
  ut_int64_t etherStatsTxOctets;
  ut_int64_t etherStatsTxPkts;
  ut_int64_t etherStatsTxUnicastPkts;
  ut_int64_t etherStatsTxBroadcastPkts;
  ut_int64_t etherStatsTxMulticastPkts;
  ut_int64_t etherStatsTxErrorPkts;
  ut_int64_t etherStatsTxLossPkts;
  /* liuyf add end */
  char      etherStatsOwner[RMON_OWNER_NAME_SIZE+1];
  u_int32_t etherStatsStatus; /*1-Valid, 2-createRequest,
                               *3-UnderCreation, 4- invalid,
                               *5-nonExistent */
};

struct rmon_HistoryControlGroup 
{
  u_int32_t historyControlIndex;
#define RMON_HISTORY_CONTROL_DATASOURCE  (1 << 0)
#define RMON_HISTORY_CONTROL_BUCKETS_REQ (1 << 1)
#define RMON_HISTORY_CONTROL_INTERVAL    (1 << 2)
#define RMON_HISTORY_CONTROL_OWNER       (1 << 3)
#define RMON_HISTORY_CONTROL_STATUS      (1 << 4)

  oid       historyControlDataSource[RMON_DATA_SOURCE_MAXSIZE];
  u_int32_t historyControlBucketsRequested;
  u_int32_t historyControlBucketsGranted;
  u_int32_t historyControlInterval;
  char      historyControlOwner[RMON_OWNER_NAME_SIZE+1];
  u_int32_t historyControlStatus; /*1-Valid, 2-createRequest,
                                   *3-UnderCreation, 4- invalid,
                                   *5-nonExistent */
  /*cycle, count of samples */
  u_int32_t current_sample_no;

  /*linearity, count of samples */
  u_int32_t linearity_sample_no;
  
  /* Timer thread */
  struct thread *rmon_coll_history_timer;
  struct route_table *historyStats_table;
};

struct rmon_EtherHistoryGroup 
{
  u_int32_t etherHistoryIndex;
  u_int32_t etherHistorySampleIndex;
  s_int32_t etherHistoryIntervalStart;
  ut_int64_t etherHistoryDropEvents;
  ut_int64_t etherHistoryOctets;
  ut_int64_t etherHistoryPkts;
  ut_int64_t etherHistoryBroadcastPkts;
  ut_int64_t etherHistoryMulticastPkts;
  ut_int64_t etherHistoryCRCAlignErrors;
  ut_int64_t etherHistoryUndersizePkts;
  ut_int64_t etherHistoryOversizePkts;
  ut_int64_t etherHistoryFragments;
  ut_int64_t etherHistoryJabbers;
  ut_int64_t etherHistoryCollisions;
  u_int32_t etherHistoryUtilization;
};

struct rmon_AlarmGroup
{
  u_int32_t alarmIndex;
#define RMON_ALARM_INTERVAL              (1 << 0)
#define RMON_ALARM_VARIABLE              (1 << 1)
#define RMON_ALARM_SAMPLETYPE            (1 << 2)
#define RMON_ALARM_STARTUP_ALARM         (1 << 3)
#define RMON_ALARM_RISING_THRESHOLD      (1 << 4)
#define RMON_ALARM_FALLING_THRESHOLD     (1 << 5)
#define RMON_ALARM_RISING_EVENT_IDX      (1 << 6)
#define RMON_ALARM_FALLING_EVENT_IDX     (1 << 7)
#define RMON_ALARM_OWNER                 (1 << 8)
#define RMON_ALARM_STATUS                (1 << 9)
#define RMON_ALARM_VARIABLE_MAXSIZE      12
#define ETHERSTATSENTRY                  "etherStatsEntry."
#define ETHERSTATSNUM                    "1.3.6.1.2.1.16.1.1.1"
s_int32_t raise_or_fall ; //only for trap

  s_int32_t alarmInterval;
//#ifdef HAVE_SNMP
  oid       alarmVariable[RMON_ALARM_VARIABLE_MAXSIZE];
//#endif /* HAVE_SNMP */

#define RMON_ALARM_OWNER_CONF            (1 << 0)
  u_int8_t alarmConf;

  char alarmVariableWord[RMON_ALARM_VAR_WORD_LENGTH+1];
  u_int32_t alarmSampleType; /*1- absoluteValue, 2- deltaValue */
  ut_int64_t alarmValue;
  u_int32_t alarmStartupAlarm;
  ut_int64_t alarmRisingThreshold; /* For ether stats type is ut_int64_t */
  ut_int64_t alarmFallingThreshold;
  u_int32_t alarmRisingEventIndex;
  u_int32_t alarmFallingEventIndex;
  char      alarmOwner[RMON_OWNER_NAME_SIZE + 1];
  u_int32_t alarmStatus;      /*1-Valid, 2-createRequest,
                               *3-UnderCreation, 4- invalid,
                               *5-nonExistent */
  struct thread *alarm_timer;
  ut_int64_t alarm_prev_value;
  ut_int64_t alarm_curr_value;
  ut_int64_t alarm_last_value;
  u_int16_t alarm_last_event;
  u_int16_t alarm_startup_alarm_status;
};

struct rmon_EventGroup
{
  u_int32_t eventIndex;

#define RMON_EVENT_CONF_DESCRIPTION     (1 << 0)
#define RMON_EVENT_CONF_OWNER           (1 << 1)
  u_int8_t eventConf;

  char     eventDescription[RMON_DESCR_LENGTH + 1];
  u_int32_t eventType;        /*1- none, 2- log, 3-snmpTrap, 4-logandTrap */

  char     eventCommunity[RMON_COMM_LENGTH + 1];
  u_int32_t eventLastTimeSent;

  char     eventOwner [RMON_OWNER_NAME_SIZE + 1];
  u_int32_t eventStatus;      /*1-Valid, 2-createRequest,
                               *3-UnderCreation, 4- invalid,
                               *5-nonExistent */
};


struct ipc_mesg_n *etherStatsTable_bulk(uint32_t stat_id, int module_id, int*pdata_num) ;
struct ipc_mesg_n *historyControlTable_bulk(uint32_t his_ctr_id, int module_id, int*pdata_num) ;
struct ipc_mesg_n *alarmTable_bulk(uint32_t alarm_id, int module_id, int*pdata_num) ;
struct ipc_mesg_n *eventTable_bulk(uint32_t event_id, int module_id, int*pdata_num) ;
struct ipc_mesg_n *etherHistoryTable_bulk(uint32_t his_ctr_id , int module_id, int*pdata_num , uint32_t his_sample_id) ;

#endif

