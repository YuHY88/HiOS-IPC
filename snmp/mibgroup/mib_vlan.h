/*
 * vlan_snmp.h
 *
 */
#ifndef _VLAN_SNMP_H
#define _VLAN_SNMP_H

#define VLAN_HHRVLANCONFIG                  1,3,6,1,4,1,9966,5,35,2

/*hhrVlanDatabaseTable*/
#define hhrVlanId                           1
#define hhrVlanMacLearn                     2
#define hhrVlanMacLearnLimit                3
#define hhrVlanStormSupressUnicast          4
#define hhrVlanStormSupressBroadcast        5
#define hhrVlanStormSupressMulticast        6
#define hhrVlanMacLearnAction               7

/*hhrIfVlanConfigTable*/
#define hhrVlanConfigIfDescr                1
#define hhrVlanConfigPVID                   2
#define hhrVlanConfigVlanMode               3
#define hhrVlanConfigVlanMember             4
#define hhrVlanConfigunTagVlan              5


/*hhrVlanMappingTable*/
#define hhrVlanMappingIfDesrc               3
#define hhrVlanMappingAction                4
#define hhrVlanMappingSvlan                 5
#define hhrVlanMappingSvlanCos              6
#define hhrVlanMappingCvlan                 7

/*hhrVlanMapping2LayerTable*/
#define HhrVlanMapping2LayerIfDesrc         4
#define HhrVlanMapping2LayerAction          5
#define HhrVlanMapping2LayerSvlan           6
#define HhrVlanMapping2LayerSvlanCos        7
#define HhrVlanMapping2LayerCvlan           8

/*hhrVlanConfigList*/
#define hhrl2VlanListForIpran1              1
#define hhrl2VlanListForIpran2              2
#define hhrl2VlanListForIpran3              3
#define hhrl2VlanListForIpran4              4
#define hhrl2VlanListForIpran5              5
#define hhrl2VlanListForIpran6              6
#define hhrl2VlanListForIpran7              7
#define hhrl2VlanListForIpran8              8

/* table define */
#define HHR_VLAN_DATABASE_TABLE             0
#define HHR_VLAN_CINFIG_TABLE               1
#define HHR_VLANMAPPING_TABLE               2
#define HHR_VLANMAPPING2LAYER_TABLE         3


/* table data refresh time(seconds) */
#define HHR_VLAN_DATABASE_TABLE_REFRESH_TIME            5
#define HHR_VLAN_CINFIG_TABLE_REFRESH_TIME              5
#define HHR_VLAN_MAPPING_TABLE_REFRESH_TIME             5
#define HHR_VLAN_MAPPING2LAYER_TABLE_REFRESH_TIME       5

/* get-next operate interval time(seconds) */
#define HHR_VLAN_DATABASE_TABLE_GET_NEXT_INTERVAL       3
#define HHR_VLAN_CINFIG_TABLE_GET_NEXT_INTERVAL         3
#define HHR_VLAN_MAPPING_TABLE_GET_NEXT_INTERVAL        3
#define HHR_VLAN_MAPPING2LAYER_TABLE_GET_NEXT_INTERVAL  3

void init_mib_vlan(void);
static void *snmp_vlan_node_xcalloc(int table_num);
static void snmp_vlan_node_add(int table_num, void *node);
static void *snmp_vlan_node_lookup(int exact, int table_num, void *index_input);
static void snmp_vlan_node_del_all(int table_num);
static void snmp_vlan_list_init(void);

extern struct ipc_mesg_n *snmp_get_switch_info_bulk(uint32_t ifindex, int module_id, int *pdata_num);


#endif                          /* _VLAN_SNMP_H */
