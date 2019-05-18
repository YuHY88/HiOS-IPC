/*
 * mib_loopdect.h
 *
 */
#ifndef _MIB_LOOPDETECT_H
#define _MIB_LOOPDETECT_H

#define LOOPDECT_HHRLOOPDECT            1,3,6,1,4,1,9966,5,35,1

/*hhrLoopDetectTable*/
#define hhrLoopDetectIfDesrc            2
#define hhrLoopDetectEnable             3
#define hhrLoopDetectVlan               4
#define hhrLoopDetectInterval           5
#define hhrLoopDetectRestoreTime        6
#define hhrLoopDetectAction             7
#define hhrLoopDetectStatus             8
#define hhrLoopDetectPackets            9

/* table define */
#define HHR_LOOPDETECT_TABLE                        0

/* table data refresh time(seconds) */
#define HHR_LOOPDETECT_TABLE_REFRESH_TIME           2

/* get-next operate interval time(seconds) */
#define HHR_LOOPDETECT_TABLE_GET_NEXT_INTERVAL      1

void init_mib_loopdetect(void);
static void loopdetect_list_init(void);
void hhrLoopdetectTable_node_free(struct loop_detect_snmp *node);
#endif                          /* _VLAN_SNMP_H */

