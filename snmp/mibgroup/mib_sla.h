/*
 * mib_loopdect.h
 *
 */
#ifndef _MIB_SLA_H
#define _MIB_SLA_H

#define MIB_HHRLSA_ROOT          1,3,6,1,4,1,9966,5,35,13

#define hhrslaMeasureConfigTable                       0
#define hhrrfc2544MeasureResultTable                   1
#define hhry1564MeasureResultTable                     2

#define hhrslaMeasureConfigTable_REFRESH_TIME          5
#define hhrrfc2544MeasureResultTable_REFRESH_TIME      5
#define hhry1564MeasureResultTable_REFRESH_TIME        5

#define hhrslaMeasureConfigTable_GET_NEXT_INTERVAL     3
#define hhrrfc2544MeasureResultTable_GET_NEXT_INTERVAL 3
#define hhry1564MeasureResultTable_GET_NEXT_INTERVAL   3

/*hhrslaMeasureConfigTable*/
#define hhrslaMeasureProtocol            2
#define hhrslaMeasureL2dmac              3
#define hhrslaMeasureL2cvlan             4
#define hhrslaMeasureL2svlan             5
#define hhrslaMeasureL2cos               6
#define hhrslaMeasureL3sip               7
#define hhrslaMeasureL3dip               8
#define hhrslaMeasureL3sport             9
#define hhrslaMeasureL3dport            10
#define hhrslaMeasureL3ttl              11
#define hhrslaMeasureL3dscp             12
#define hhrslaMeasurePacketSize         13
#define hhrslaMeasurePacketRate         14
#define hhrslaMeasureMeasurePacketLoss  15
#define hhrslaMeasureMeasurePacketDelay 16
#define hhrslaMeasureMeasureThroughput  17
#define hhrslaMeasureScheduleInterval   18
#define hhrslaMeasureScheduleFrequency  19
#define hhrslaMeasureMeasureIfDescr     20
#define hhrslaMeasureL2smac             21
#define hhrslaMeasureSACLos             22
#define hhrslaMeasureSACDelay           23
#define hhrslaMeasureSACJitter          24
#define hhrslaMeasureL2ctpid            25
#define hhrslaMeasureL2stpid            26
#define hhrslaMeasureL2scos             27
#define hhrslaMeasureCir                28
#define hhrslaMeasureEir                29
#define hhrslaMeasureRFC2544PacketSize  30
#define hhrslaMeasureY1564PacketSize    31
#define hhrslaMeasureDuration           32


//hhrrfc2544MeasureResultTable
#define hhrrfc2544MeasureSessionStatus      2
#define hhrrfc2544MeasureFLRMin             3
#define hhrrfc2544MeasureFLRMax             4
#define hhrrfc2544MeasureFLRMean            5
#define hhrrfc2544MeasureFTDMin             6
#define hhrrfc2544MeasureFTDMax             7
#define hhrrfc2544MeasureFTDMean            8
#define hhrrfc2544MeasureThroughput         9

//hhry1564MeasureResultTable
#define hhry1564Measurecir                  2
#define hhry1564MeasureSessionStatus        3
#define hhry1564MeasureFLRMin               4
#define hhry1564MeasureFLRMax               5
#define hhry1564MeasureFLRMean              6
#define hhry1564MeasureFTDMin               7
#define hhry1564MeasureFTDMax               8
#define hhry1564MeasureFTDMean              9
#define hhry1564MeasureFDVMin              10
#define hhry1564MeasureFDVMax              11
#define hhry1564MeasureFDVMean             12

struct ipran_snmp_sla_session
{
    int y1564_cir ;
    struct sla_session old_data ;
};

struct ipran_snmp_sla_rfc2544_table
{
    int index_size ;
    struct sla_session old_data ;
};

void init_mib_lsa(void);

#endif                          /* _VLAN_SNMP_H */

