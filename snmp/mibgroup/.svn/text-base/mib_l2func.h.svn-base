/*
 *  cli_set group interface - ifm_snmp.h
 *
 */
#ifndef _HHR_L2FUNC_H
#define _HHR_L2FUNC_H

#define HHRLAVER2FUNCTION                       1,3,6,1,4,1,9966,5,35,16,1

/*hhrMacConfig*/
#define hhrMacAgeingTime                        1
#define hhrMacMoveEnable                        2
#define hhrMacLearnLimit                        3
#define hhrMacLearnAction                       4

/*hhrMacAddressTable*/
#define hhrMacVSI                               3
#define hhrMaclfDescr                           4
#define hhrMacType                              5

/*hhrMacIfLimitTable*/
#define hhrIfMacLimitifDescr                    2
#define hhrIfMacLearnLimit                      3
#define hhrIfMacLearnAction                     4

/*hhrDynamicMacAddressTable*/
#define hhrDynamicMaclfDescr                    3

/* table define */
#define HHR_MAC_ADDRESS_TABLE                   0

/* table data refresh time(seconds) */
#define HHR_MAC_ADDRESS_TABLE_REFRESH_TIME      5

/* get-next operate interval time(seconds) */
#define HHR_MAC_ADDRESS_TABLE_GET_NEXT_INTERVAL 3

/*
 * init_xxx(void)
 */
void init_hhr_l2func(void);
static void *snmp_mac_node_xcalloc(int table_num);
static void snmp_mac_node_add(int table_num, void *node);
static void *snmp_mac_node_lookup(int exact, int table_num, void *index_input);
static void snmp_mac_node_del_all(int table_num);
static void snmp_mac_list_init(void);
void hhrMacAddressTable_node_free(struct mac_entry *node);

#endif                          /* _HHR_L2FUNC_H */
