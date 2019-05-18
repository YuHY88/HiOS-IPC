/*
 *  alarm snmp browser interface - alarm_snmp.h
 *
 */
#ifndef _ALARM_SNMP_H
#define _ALARM_SNMP_H


#define ALARM_OIDPREFIX 1,3,6,1,4,1,9966,5,14

#define alarmGolbalCfg              1
//#define alarmPortMonitorCfg       2
//#define alarmAttributeCfg         3
//#define alarmCurrDataSheet        4
//#define alarmHistDataSheet        5
//#define eventDataSheet            6
#define alarmTrap                   7
#define alarmPortMonitorCfg         8
#define alarmAttributeCfg           9
#define alarmCurrDataSheet          11
#define alarmHistDataSheet          13
#define eventDataSheet              15


#define alarmDelayTime              1
#define alarmBuzzerCtl              2
#define alarmDataSheetCtrl          3
#define alarmRestrain               4
#define alarmReversalMode           5

#define alarmCurrAlmDSCtrl          1
#define alarmHistAlmDSCtrl          2
#define alarmEventDSCtrl            3
#define alarmAttriCtrl              4
#define alarmPoerBaseMonitorCtrl    5


#define alarmProductDelayTime       1
#define alarmDisappearDelayTime     2

#define alarmBuzzerEn               1
#define alarmBuzzerClear            2
#define alarmBuzzerThreshold        3

#define alarmCurrAlmDSSize          1
#define alarmCurrAlmDSCircle        2

#define alarmHistAlmDSSize          1
#define alarmHistAlmDSCircle        2

#define alarmEventDSSize            1
#define alarmEventDSCircle          2

#define alarmAttribCfgNum           1

#define alarmPoerBaseMonitorCfgNum  1


#define alarmPortMonitorCfgTable    1

#define monDevIndex                 1
#define monPortIndex1               2
#define monPortIndex2               3
#define monPortIndex3               4
#define monPortIndex4               5
#define monPortIndex5               6
#define alarmMonitorEn              7
#define alarmRowStatus              8

#define alarmAttributeCfgTable      1

#define alarmSubType                1
#define attribDevIndex              2
#define attribPortIndex1            3
#define attribPortIndex2            4
#define attribPortIndex3            5
#define attribPortIndex4            6
#define attribPortIndex5            7
#define basePortMark                8
#define alarmLevel                  9
#define alarmMask                   10
#define alarmFilter                 11
#define alarmReport                 12
#define alarmRecord                 13
#define portAlmReverse              14
#define attribRowStatus             15

#define alarmCurrDataSheetTable     1

#define currAlarmDSIndex            1
#define currAlarmType               2
#define currDevIndex                3
#define currPortIndex1              4
#define currPortIndex2              5
#define currPortIndex3              6
#define currPortIndex4              7
#define currPortIndex5              8
#define currAlarmLevel              9
#define alarmCurrProductCnt         10
#define alarmCurrFirstProductTime   11
#define alarmCurrThisProductTime    12
#define alarmCurrRowStatus          13

#define alarmHistDataSheetTable     1

#define histAlarmDSIndex            1
#define histAlarmType               2
#define histDevIndex                3
#define histPortIndex1              4
#define histPortIndex2              5
#define histPortIndex3              6
#define histPortIndex4              7
#define histPortIndex5              8
#define histAlarmLevel              9
#define alarmHistProductCnt         10
#define alarmHistFirstProductTime   11
#define alarmHistThisProductTime    12
#define alarmHistThisDisappearTime  13
#define alarmHistRowStatus          14

#define eventDataSheetTable         1

#define eventDSIndex                1
#define eventAlarmType              2
#define eventDevIndex               3
#define eventPortIndex1             4
#define eventPortIndex2             5
#define eventPortIndex3             6
#define eventPortIndex4             7
#define eventPortIndex5             8
#define eventAlarmLevel             9
#define eventDetail                 10
#define eventTime                   11
#define eventRowStatus              12


#define alarmProductTrap            1
#define alarmDisappearTrap          2
#define alarmEventReportTrap        3
#define alarmReverseAutoRecTrap     4

/* statGlobalSubTypeAtrribTable */
#define FLAG_ALARMMONITOREN         (0x1 << 0)
#define FLAG_ALARMMONITORROWSTATUS  (0x1 << 1)

#define FLAG_BASEPORTMARK           (0x1 << 0)
#define FLAG_ALARMLEVEL             (0x1 << 1)
#define FLAG_ALARMMASK              (0x1 << 2)
#define FLAG_ALARMFILTER            (0x1 << 3)
#define FLAG_ALARMREPORT            (0x1 << 4)
#define FLAG_ALARMRECORD            (0x1 << 5)
#define FLAG_PORTALMREVERSE         (0x1 << 6)
#define FLAG_ALARMATTRIROWSTATUS    (0x1 << 7)

/*use for sign the status of the table when modify*/
#define ALARM_MODIFY_TABLE_FLAG     1

typedef struct
{
//  statGlobalSubTypeAtrribTable_index index;
    u_int32_t status;
    u_int32_t flag;
} almPortMonitorCfgStatus;

typedef struct
{
//  statPortMoniCtrlTable_index index;
    u_int32_t status;
    u_int32_t flag;
} almAttriCfgStatus;





WriteMethod write_alarmProductDelayTime;
WriteMethod write_alarmDisappearDelayTime;

WriteMethod write_alarmBuzzerEn;
WriteMethod write_alarmBuzzerClear;
WriteMethod write_alarmBuzzerThreshold;

WriteMethod write_alarmCurrAlmDSCircle;
WriteMethod write_alarmHistAlmDSCircle;
WriteMethod write_alarmEventDSCircle;

WriteMethod write_alarmRestrain;
WriteMethod write_alarmReversalMode;

WriteMethod write_alarmMonitorEn;
WriteMethod write_alarmRowStatus;

WriteMethod write_basePortMark;
WriteMethod write_alarmLevel;
WriteMethod write_alarmMask;
WriteMethod write_alarmFilter;
WriteMethod write_alarmReport;
WriteMethod write_alarmRecord;
WriteMethod write_alarmReverse;
WriteMethod write_attribRowStatus;


void init_alarm_snmp(void);

#endif                          /* _ALARM_SNMP_H */
