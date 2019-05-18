/*
 * mib_loopdect.h
 *
 */
#ifndef _MIB_RMON_H
#define _MIB_RMON_H

#define RMONMIBOID 1,3,6,1,2,1,16

/*
 * ZebOS enterprise RMON-MIB equivalent root OID.  This variable is used to
 * register the RMON-MIB to SNMP agent under SMUX protocol.
 */
#define RMONDMIBOID 1,3,6,1,4,1,3317,1,2,25

#define INTEGER         ASN_INTEGER
#define INTEGER32       ASN_INTEGER
#define ENTRYSTATUS     ASN_INTEGER
#define INTERFACEINDEX  ASN_INTEGER
#define TRUTHVALUE      ASN_INTEGER
#define COUNTER32       ASN_COUNTER
#define COUNTER64       ASN_COUNTER64
#define GAUGE32         ASN_UNSIGNED
#define IPADDRESS       ASN_IPADDRESS
#define OCTET_STRING    ASN_OCTET_STR
#define SNMPADMINSTRING ASN_OCTET_STR
#define OWNERSTRING     ASN_OCTET_STR
#define DISPLAYSTRING     ASN_OCTET_STR
#define OBJECT_ID       ASN_OBJECT_ID
#define BITS            ASN_BIT_STR
#define RCREATE  3
#define TIMETICKS ASN_TIMETICKS
#define NOTACCESSIBLE NOACCESS

#define RMON_SNMP_RETURN_OID(V,L) \
  do { \
    *var_len = (L)*sizeof(oid); \
    return (u_char *)(V); \
  } while (0)


#if 0
/*
 * RMON MIB etherStatsTable
 */
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
#define ETHERSTATSSTATUS                        21
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
/* liuyf add end */
#endif

/*
 * RMON MIB historyControlTable
 */
#define HISTORYCONTROLINDEX                     1
#define HISTORYCONTROLDATASOURCE                2
#define HISTORYCONTROLBUCKETSREQUESTED          3
#define HISTORYCONTROLBUCKETSGRANTED            4
#define HISTORYCONTROLINTERVAL                  5
#define HISTORYCONTROLOWNER                     6
#define HISTORYCONTROLSTATUS                    7


/*
 * RMON MIB etherHistoryTable
 */
#define ETHERHISTORYINDEX                       1
#define ETHERHISTORYSAMPLEINDEX                 2
#define ETHERHISTORYINTERVALSTART               3
#define ETHERHISTORYDROPEVENTS                  4
#define ETHERHISTORYOCTETS                      5
#define ETHERHISTORYPKTS                        6
#define ETHERHISTORYBROADCASTPKTS               7
#define ETHERHISTORYMULTICASTPKTS               8
#define ETHERHISTORYCRCALIGNERRORS              9
#define ETHERHISTORYUNDERSIZEPKTS               10
#define ETHERHISTORYOVERSIZEPKTS                11
#define ETHERHISTORYFRAGMENTS                   12
#define ETHERHISTORYJABBERS                     13
#define ETHERHISTORYCOLLISIONS                  14
#define ETHERHISTORYUTILIZATION                 15
#define RISING_ALARM                 1
#define FALLING_ALARM                2
#define RISING_OR_FALLING_ALARM      3

/*
 * RMON MIB alarmTable
 */
#define ALARMINDEX                              1
#define ALARMINTERVAL                           2
#define ALARMVARIABLE                           3
#define ALARMSAMPLETYPE                         4
#define ALARMVALUE                              5
#define ALARMSTARTUPALARM                       6
#define ALARMRISINGTHRESHOLD                    7
#define ALARMFALLINGTHRESHOLD                   8
#define ALARMRISINGEVENTINDEX                   9
#define ALARMFALLINGEVENTINDEX                  10
#define ALARMOWNER                              11
#define ALARMSTATUS                             12

/* RMON RANGES */
#define RMON_MAX_EVENT_INDEX                    1200
#define RMON_MAX_BUCKETS                        50
#define RMON_MAX_HISTORY_INTERVAL               86400

/*
 * RMON MIB eventTable
 */
#define EVENTINDEX                              1
#define EVENTDESCRIPTION                        2
#define EVENTTYPE                               3
#define EVENTCOMMUNITY                          4
#define EVENTLASTTIMESENT                       5
#define EVENTOWNER                              6
#define EVENTSTATUS                             7

void init_mib_rmon(void);

#endif                          /* _VLAN_SNMP_H */

