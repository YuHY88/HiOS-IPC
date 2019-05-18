#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <net-snmp-config.h>
#include <types.h>
#include <net-snmp-includes.h>
#include <net-snmp-agent-includes.h>
#include <snmp_index_operater.h>

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/ifm_common.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/types.h>
#include <lib/route_com.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/inet_ip.h>
#include <lib/linklist.h>
#include <qos/sla/sla_session.h>
#include "mib_ddm.h"
#include <lib/log.h>

#include "ipran_snmp_data_cache.h"

//#include "lib/snmp_common.h"
#include "snmp_config_table.h"


/*local temp variable*/
static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value = 0;
static uchar mac_value[6] = {0};
static uint32_t           uint_value = 0;
static uint32_t           ip_value = 0;

static struct ipran_snmp_data_cache *hhrDDMInfoTable_cache = NULL ;

#define                   SLA_STRING_LEN         256

FindVarMethod hhrDDMInfoTable_get ;
oid  hhrDDMInfoTable_oid[]   = { MIB_HHRDDM_ROOT , 15};
int  hhrDDMInfoTable_oid_len = sizeof(hhrDDMInfoTable_oid) / sizeof(oid);
struct variable2 hhrDDMInfoTable_variables[] =
{
    {hhrDDMIfDescr,             ASN_OCTET_STR, RONLY, hhrDDMInfoTable_get, 2, {1, 2}},
    {hhrDDMVendorPN,            ASN_OCTET_STR,  RONLY, hhrDDMInfoTable_get, 2, {1, 3}},
    {hhrDDMVendorName,              ASN_OCTET_STR,  RONLY, hhrDDMInfoTable_get, 2, {1, 4}},
    {hhrDDMVendorOUI,           ASN_OCTET_STR,  RONLY, hhrDDMInfoTable_get, 2, {1, 5}},
    {hhrDDMVendorRev,           ASN_OCTET_STR,  RONLY, hhrDDMInfoTable_get, 2, {1, 6}},
    {hhrDDMVendorPrivateInformation,      ASN_OCTET_STR,    RONLY, hhrDDMInfoTable_get, 2, {1, 7}},
    {hhrDDMTransceiverIdentifier,           ASN_OCTET_STR,  RONLY, hhrDDMInfoTable_get, 2, {1, 8}},
    {hhrDDMFibreChannelTransmissionMedia,           ASN_OCTET_STR,  RONLY, hhrDDMInfoTable_get, 2, {1, 9}},
    {hhrDDMTransmissionMedia,       ASN_OCTET_STR,  RONLY, hhrDDMInfoTable_get, 2, {1, 10}} ,
    {hhrDDMConnector,               ASN_OCTET_STR, RONLY, hhrDDMInfoTable_get, 2, {1, 11}},
    {hhrDDMNominalBR,           ASN_OCTET_STR,  RONLY, hhrDDMInfoTable_get, 2, {1, 12}},
    {hhrDDMWaveLength,              ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 13}},
    {hhrDDMTransmissionDistance,            ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 14}},
    {hhrDDMTXDisableState,      ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 15}},
    {hhrDDMTXFaultState,            ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 16}},
    {hhrDDMRxLOSState,          ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 17}},

    {hhrDDMCurrentSFPmoduleTemperature,         ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 18}} ,
    {hhrDDMHighTemperatureAlarmThreshold,           ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 19}},
    {hhrDDMLowTemperatureAlarmThreshold,        ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 20}} ,
    {hhrDDMHighTemperatureWarnThreshold,        ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 21}} ,
    {hhrDDMLowTemperatureWarnThreshold,             ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 22}},

    {hhrDDMCurrentSFPModuleVoltage,         ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 23}} ,
    {hhrDDMHighVoltageAlarmThreshold,               ASN_INTEGER, RONLY, hhrDDMInfoTable_get, 2, {1, 24}},
    {hhrDDMLowVoltageAlarmThreshold,            ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 25}},
    {hhrDDMHighVoltageWarnThreshold,            ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 26}},
    {hhrDDMLowVoltageWarnThreshold,             ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 27}},

    {hhrDDMCurrentSFPmoduleTxBias,      ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 28}},
    {hhrDDMHighTxBiasAlarmThreshold,            ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 29}},
    {hhrDDMLowTxBiasAlarmThreshold,         ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 30}} ,
    {hhrDDMHighTxBiasWarnThreshold,             ASN_INTEGER, RONLY, hhrDDMInfoTable_get, 2, {1, 31}},
    {hhrDDMLowTxBiasWarnThreshold,          ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 32}},

    {hhrDDMCurrentSFPmoduleTxpower,             ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 33}},
    {hhrDDMHighTxpowerAlarmThreshold,      ASN_INTEGER,     RONLY, hhrDDMInfoTable_get, 2, {1, 34}},
    {hhrDDMLowTxpowerAlarmThreshold,            ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 35}},
    {hhrDDMHighTxpowerWarnThreshold,            ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 36}},
    {hhrDDMLowTxpowerWarnThreshold,         ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 37}} ,

    {hhrDDMCurrentSFPmoduleRxPower,             ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 38}},
    {hhrDDMHighRxPowerAlarmThreshold,       ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 39}} ,
    {hhrDDMLowRxPowerAlarmThreshold,        ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 40}} ,
    {hhrDDMHighRxPowerWarnThreshold,            ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 41}},
    {hhrDDMLowRxPowerWarnThreshold,         ASN_INTEGER,    RONLY, hhrDDMInfoTable_get, 2, {1, 42}},
    {hhrDDMVendorSN,                        ASN_OCTET_STR,  RONLY, hhrDDMInfoTable_get, 2, {1, 43}}
} ;


void  init_mib_ddm(void)
{
    REGISTER_MIB("hhrDDMInfoTable", hhrDDMInfoTable_variables, variable2,
                 hhrDDMInfoTable_oid) ;
}

int hhrDDMInfoTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                      struct ifm_sfp_info  *index)
{
    struct ifm_sfp_info *phal_ifm_sfp = NULL;
	struct ifm_sfp_info *tphal_ifm_sfp = NULL;
    int data_num = 0;
    int ret = 0;
    phal_ifm_sfp = ifm_get_sfp_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num);
	tphal_ifm_sfp = phal_ifm_sfp;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ifindex [%d] data_num [%d]\n", __FILE__, __LINE__, __func__ , index->ifindex, data_num);

    if (0 == data_num || NULL == phal_ifm_sfp)
    {
    	mem_share_free_bydata(tphal_ifm_sfp, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, phal_ifm_sfp , sizeof(struct ifm_sfp_info));
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ifindex [%d] [%d]\n", __FILE__, __LINE__, __func__ , phal_ifm_sfp->ifindex , ret);
            phal_ifm_sfp++;
        }

        //time(&cache->getNext_time_old);
        mem_share_free_bydata(tphal_ifm_sfp, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct ifm_sfp_info *hhrDDMInfoTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                 int exact,
                                                 const struct ifm_sfp_info  *index_input)
{
    struct listnode  *node, *nnode;
    struct ifm_sfp_info     *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data;
        }

        if (data1_find->ifindex == index_input->ifindex)
        {
            if (1 == exact) //get
            {
                return data1_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return node->next->data;
                }
            }
        }
    }

    return (NULL);
}

u_char *hhrDDMInfoTable_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ifm_sfp_info *phal_ifm_sfp = NULL;
    struct ifm_sfp_info index  ;
    int ret = 0;
    int index_int = 0 ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index_int , exact);

    index.ifindex = index_int ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index.ifindex);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrDDMInfoTable_cache)
    {
        hhrDDMInfoTable_cache = snmp_cache_init(sizeof(struct ifm_sfp_info) ,
                                                hhrDDMInfoTable_get_data_from_ipc ,
                                                hhrDDMInfoTable_node_lookup);

        if (NULL == hhrDDMInfoTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    phal_ifm_sfp = snmp_cache_get_data_by_index(hhrDDMInfoTable_cache , exact, &index);

    if (NULL == phal_ifm_sfp)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, phal_ifm_sfp->ifindex);
    }

    switch (vp->magic)
    {
        case hhrDDMIfDescr:
            memset(str_value, 0, STRING_LEN);
            ifm_get_name_by_ifindex(phal_ifm_sfp->ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrDDMVendorPN:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value , phal_ifm_sfp->info.vendor_pn , strlen(phal_ifm_sfp->info.vendor_pn));
            *var_len = strlen(str_value);
            return (str_value);

        case hhrDDMVendorName:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value , phal_ifm_sfp->info.vendor_name , strlen(phal_ifm_sfp->info.vendor_pn));
            *var_len = strlen(str_value);
            return (str_value);

        case hhrDDMVendorOUI:
            memset(str_value, 0, STRING_LEN);
            snprintf(str_value, SFP_BASE_VENDOR_OUI_SIZE, "%s", phal_ifm_sfp->info.vendor_oui);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrDDMVendorRev:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value , phal_ifm_sfp->info.vendor_rev , strlen(phal_ifm_sfp->info.vendor_pn));
            *var_len = strlen(str_value);
            return (str_value);

        case hhrDDMVendorPrivateInformation:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value , phal_ifm_sfp->info.vendor_specific , strlen(phal_ifm_sfp->info.vendor_pn));
            *var_len = strlen(str_value);
            return (str_value);

        case hhrDDMTransceiverIdentifier:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value , phal_ifm_sfp->info.identifier , strlen(phal_ifm_sfp->info.vendor_pn));
            *var_len = strlen(str_value);
            return (str_value);

        case hhrDDMFibreChannelTransmissionMedia:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value , phal_ifm_sfp->info.fc_transmission_media , strlen(phal_ifm_sfp->info.vendor_pn));
            *var_len = strlen(str_value);
            return (str_value);

        case hhrDDMTransmissionMedia:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, phal_ifm_sfp->info.transmission_media , strlen(phal_ifm_sfp->info.transmission_media));
            *var_len = strlen(str_value);
            return (str_value);

        case hhrDDMConnector:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value , phal_ifm_sfp->info.connector, strlen(phal_ifm_sfp->info.connector));
            *var_len = strlen(str_value);
            return (str_value);

        case hhrDDMNominalBR:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, phal_ifm_sfp->info.nominal_br, strlen(phal_ifm_sfp->info.nominal_br));
            *var_len = strlen(str_value);
            return (str_value);

        case hhrDDMWaveLength:
            int_value = phal_ifm_sfp->info.wave_length ;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMTransmissionDistance:
            int_value = phal_ifm_sfp->info.transmission_distance ;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMTXDisableState:
            int_value = phal_ifm_sfp->info.tx_disable;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMTXFaultState:
            int_value = (phal_ifm_sfp->info.tx_fault);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMRxLOSState:
            int_value = (phal_ifm_sfp->info.rx_loss);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMCurrentSFPmoduleTemperature:
            int_value = (phal_ifm_sfp->info.temperature.current);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMHighTemperatureAlarmThreshold:
            int_value = (phal_ifm_sfp->info.temperature.high_alarm);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMLowTemperatureAlarmThreshold:
            int_value = (phal_ifm_sfp->info.temperature.low_alarm);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMHighTemperatureWarnThreshold:
            int_value = (phal_ifm_sfp->info.temperature.high_warn);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMLowTemperatureWarnThreshold:
            int_value = (phal_ifm_sfp->info.temperature.low_warn);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMCurrentSFPModuleVoltage:
            int_value = (phal_ifm_sfp->info.voltage.current);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMHighVoltageAlarmThreshold:
            int_value = (phal_ifm_sfp->info.voltage.high_alarm);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

		case hhrDDMLowVoltageAlarmThreshold:
            int_value = (phal_ifm_sfp->info.voltage.low_alarm);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);
		
        case hhrDDMHighVoltageWarnThreshold:
            int_value = (phal_ifm_sfp->info.voltage.high_warn);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMLowVoltageWarnThreshold:
            int_value = (phal_ifm_sfp->info.voltage.low_warn);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMCurrentSFPmoduleTxBias:
            int_value = (phal_ifm_sfp->info.bias.current);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMHighTxBiasAlarmThreshold:
            int_value = (phal_ifm_sfp->info.bias.high_alarm);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMLowTxBiasAlarmThreshold:
            int_value = (phal_ifm_sfp->info.bias.low_alarm);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMHighTxBiasWarnThreshold:
            int_value = (phal_ifm_sfp->info.bias.high_warn);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMLowTxBiasWarnThreshold:
            int_value = (phal_ifm_sfp->info.bias.low_warn);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMCurrentSFPmoduleTxpower:
            int_value = (phal_ifm_sfp->info.tx_power.current);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMHighTxpowerAlarmThreshold:
            int_value = (phal_ifm_sfp->info.tx_power.high_alarm);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMLowTxpowerAlarmThreshold:
            int_value = (phal_ifm_sfp->info.tx_power.low_alarm);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMHighTxpowerWarnThreshold:
            int_value = (phal_ifm_sfp->info.tx_power.high_warn);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMLowTxpowerWarnThreshold:
            int_value = (phal_ifm_sfp->info.tx_power.low_warn);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMCurrentSFPmoduleRxPower:
            int_value = (phal_ifm_sfp->info.rx_power.current);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMHighRxPowerAlarmThreshold:
            int_value = (phal_ifm_sfp->info.rx_power.high_alarm);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMLowRxPowerAlarmThreshold:
            int_value = (phal_ifm_sfp->info.rx_power.low_alarm);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMHighRxPowerWarnThreshold:
            int_value = (phal_ifm_sfp->info.rx_power.high_warn);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrDDMLowRxPowerWarnThreshold:
            int_value = (phal_ifm_sfp->info.rx_power.low_warn);
            *var_len = sizeof(int);
            return (u_char *)(&int_value);
 
        case hhrDDMVendorSN:
			memset(str_value, 0, STRING_LEN);
            memcpy(str_value , phal_ifm_sfp->info.vendor_sn , strlen(phal_ifm_sfp->info.vendor_sn));
            *var_len = strlen(str_value);
            return (str_value);
			
        default :
            return (NULL);
    }
}



