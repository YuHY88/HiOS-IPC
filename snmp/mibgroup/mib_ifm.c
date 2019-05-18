/*
 *  cli command MIB group implementation - ifm_snmp.c
 *
 */
/* Portions of this file are subject to the following copyright(s).  See
 * the Net-SNMP's COPYING file for more details and other copyrights
 * that may apply:
 */
/*
 * Portions of this file are copyrighted by:
 * Copyright ?2003 Sun Microsystems, Inc. All rights reserved.
 * Use is subject to license terms specified in the COPYING file
 * distributed with the Net-SNMP package.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "net-snmp-config.h"
#include <lib/types.h>
#include "types.h"

#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>

#include <lib/mpls_common.h>

#include <ifm/ifm_message.h>

#include "ipran_snmp_data_cache.h"

#include "mib_ifm.h"
#include <lib/log.h>
#include <ces/stm_if.h>
#include "snmp_config_table.h"

struct ifm_snmp_counter
{
    uint32_t        ifindex;
    struct ifm_counter   counter;
};


static uchar str_value[STRING_LEN] = {'\0'};
static uchar mac_value[6] = {0};
static int   int_value = 0;
static struct counter64 uint64_value;
static uint32_t     ip_value = 0;



/*
 * define the structure we're going to ask the agent to register our
 * information at
 */

oid  hhrBaseIfTable_oid[]   = {IFM_HHRIFEXT, 1};
static struct ipran_snmp_data_cache *hhrBaseIfTable_cache = NULL;
FindVarMethod hhrBaseIfTable_get;
struct variable2 hhrBaseIfTable_variables[] =
{
    {hhrBaseIfDescr,           ASN_OCTET_STR, RWRITE, hhrBaseIfTable_get, 2, {1, 1}},
    {hhrBaseIfMode,            ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 2}},
    {hhrBaseIfPortType,        ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 3}},
    {hhrBaseIfMtu,             ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 4}} ,
    {hhrBaseIfWorkMode,        ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 5}},
    {hhrBaseIfSpeed,           ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 6}},
    {hhrBaseIfFlappingControl, ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 7}},
    {hhrBaseIfJumboFrame,      ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 8}},
    {hhrBaseIfAdminStatus,     ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 9}},
    {hhrBaseIfTPId,            ASN_OCTET_STR, RWRITE, hhrBaseIfTable_get, 2, {1, 10}},
    {hhrBaseIfStatistics,      ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 11}},
    {hhrBaseIfL3Vpn,           ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 12}},
    {hhrBaseIfPhysAddress,     ASN_OCTET_STR, RWRITE, hhrBaseIfTable_get, 2, {1, 13}},
    {hhrBaseIfOperStatus,      ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 14}},
    {hhrBaseIfLoopbackStatus,  ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 15}},
    {hhrBaseIfLoopbackType,    ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 16}},
    {hhrBaseIfLoopbackvlan,    ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 17}},
    {hhrBaseIfLoopbackIP,      ASN_IPADDRESS, RWRITE, hhrBaseIfTable_get, 2, {1, 18}},
    {hhrBaseIfLoopbackMAC,     ASN_OCTET_STR, RWRITE, hhrBaseIfTable_get, 2, {1, 19}},
    {hhrBaseIfAliase,          ASN_OCTET_STR, RWRITE, hhrBaseIfTable_get, 2, {1, 20}}, 
    {hhrBaseIfMasterMode,      ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 21}},  
    {hhrBaseIfMasterState,     ASN_INTEGER,   RWRITE, hhrBaseIfTable_get, 2, {1, 22}},
};

oid  hhrL3IfIPv4Table_oid[]   = {IFM_HHRIFEXT, 2};
static struct ipran_snmp_data_cache *hhrL3IfIPv4Table_cache = NULL;
FindVarMethod hhrL3IfIPv4Table_get;
struct variable2 hhrL3IfIPv4Table_variables[] =
{
    {hhrL3IpAddress,             ASN_OCTET_STR, RWRITE,   hhrL3IfIPv4Table_get, 2, {1, 1}},
    {hhrL3IfDescr,               ASN_OCTET_STR, RWRITE,   hhrL3IfIPv4Table_get, 2, {1, 2}},
    {hhrL3IfIPv4Type,            ASN_INTEGER,   RWRITE,   hhrL3IfIPv4Table_get, 2, {1, 3}} ,
    {hhrL3IfUnnumberedIfIndex,   ASN_INTEGER,   RWRITE,   hhrL3IfIPv4Table_get, 2, {1, 4}},
    {hhrL3IfUnnumberedIfDesc,    ASN_OCTET_STR, RWRITE,   hhrL3IfIPv4Table_get, 2, {1, 5}}
};

oid  hhrL2SubIfVlanTranslateTable_oid[]   = {IFM_HHRIFEXT, 4};
static struct ipran_snmp_data_cache *hhrL2SubIfVlanTranslateTable_cache = NULL;
FindVarMethod hhrL2SubIfVlanTranslateTable_get ;
struct variable2 hhrL2SubIfVlanTranslateTable_variables[] =
{
    {hhrVTSubIfDescr,           ASN_OCTET_STR, RWRITE, hhrL2SubIfVlanTranslateTable_get, 2, {1, 1}},
    {hhrVTMode,                 ASN_INTEGER,   RWRITE, hhrL2SubIfVlanTranslateTable_get, 2, {1, 2}},
    {hhrVTEncapsulate,          ASN_INTEGER,   RWRITE, hhrL2SubIfVlanTranslateTable_get, 2, {1, 3}},
    {hhrVTcvlan,                ASN_OCTET_STR, RWRITE, hhrL2SubIfVlanTranslateTable_get, 2, {1, 4}},
    {hhrVTsvlan,                ASN_OCTET_STR, RWRITE, hhrL2SubIfVlanTranslateTable_get, 2, {1, 5}},
    {hhrVTTranslateType,        ASN_INTEGER,   RWRITE, hhrL2SubIfVlanTranslateTable_get, 2, {1, 6}},
    {hhrVTTranslatedcvlan,      ASN_OCTET_STR, RWRITE, hhrL2SubIfVlanTranslateTable_get, 2, {1, 7}},
    {hhrVTTranslatedcvlanCos,   ASN_INTEGER,   RWRITE, hhrL2SubIfVlanTranslateTable_get, 2, {1, 8}},
    {hhrVTTranslatedsvlan,      ASN_OCTET_STR, RWRITE, hhrL2SubIfVlanTranslateTable_get, 2, {1, 9}},
    {hhrVTTranslatedsvlanCos,   ASN_INTEGER,   RWRITE, hhrL2SubIfVlanTranslateTable_get, 2, {1, 10}}
};

oid  hhrIFStatisticsTable_oid[]   = {IFM_HHRIFEXT, 5};
FindVarMethod hhrIFStatisticsTable_get ;
struct variable2 hhrIFStatisticsTable_variables[] =
{
    {hhrISInputTotalBytes,        ASN_COUNTER64, RONLY, hhrIFStatisticsTable_get, 2, {1, 2}},
    {hhrISInputTotalPackets,      ASN_COUNTER64, RONLY, hhrIFStatisticsTable_get, 2, {1, 3}},
    {hhrISInputDropPackets,       ASN_COUNTER64, RONLY, hhrIFStatisticsTable_get, 2, {1, 4}},
    {hhrISOutputTotalBytes,       ASN_COUNTER64, RONLY, hhrIFStatisticsTable_get, 2, {1, 5}},
    {hhrISOutputTotalPackets,     ASN_COUNTER64, RONLY, hhrIFStatisticsTable_get, 2, {1, 6}},
    {hhrISOutputDropPackets,      ASN_COUNTER64, RONLY, hhrIFStatisticsTable_get, 2, {1, 7}},
    {hhrISInputMulticastPackets,  ASN_COUNTER64, RONLY, hhrIFStatisticsTable_get, 2, {1, 8}},
    {hhrISInputBroadcastPackets,  ASN_COUNTER64, RONLY, hhrIFStatisticsTable_get, 2, {1, 9}},
    {hhrISInputUnicastPackets,    ASN_COUNTER64, RONLY, hhrIFStatisticsTable_get, 2, {1, 10}},
    {hhrISOutputMulticastPackets, ASN_COUNTER64, RONLY, hhrIFStatisticsTable_get, 2, {1, 11}},
    {hhrISOutputBroadcastPackets, ASN_COUNTER64, RONLY, hhrIFStatisticsTable_get, 2, {1, 12}},
    {hhrISOutputUnicastPackets,   ASN_COUNTER64, RONLY, hhrIFStatisticsTable_get, 2, {1, 13}},
    {hhrISBandwidthUtlization,    ASN_INTEGER, RONLY, hhrIFStatisticsTable_get, 2, {1, 14}}
};

static oid hhrSFPConfigTable_oid[] = {IFM_HHRIFEXT, 10};
struct ipran_snmp_data_cache *hhrSFPConfigTable_cache = NULL;
FindVarMethod hhrSFPConfigTable_get;
struct variable2 hhrSFPConfigTable_variables[] =
{
    {hhrSFPAlsIfDescr, ASN_OCTET_STR, RONLY, hhrSFPConfigTable_get, 2, {1, 1}},
    {hhSFPAlsEnable,   ASN_INTEGER,   RONLY, hhrSFPConfigTable_get, 2, {1, 2}},
    {hhSFPTsEnable,    ASN_INTEGER,   RONLY, hhrSFPConfigTable_get, 2, {1, 3}},
    {hhrSFPAlsOffTime, ASN_INTEGER,   RONLY, hhrSFPConfigTable_get, 2, {1, 4}},
    {hhrSFPAlsOnTime,  ASN_INTEGER,   RONLY, hhrSFPConfigTable_get, 2, {1, 5}}
};

void  init_mib_ifm(void)
{

    REGISTER_MIB("ipran/hhrBaseIfTable", hhrBaseIfTable_variables, variable2,
                 hhrBaseIfTable_oid) ;
    REGISTER_MIB("ipran/hhrL3IfIPv4Table", hhrL3IfIPv4Table_variables, variable2,
                 hhrL3IfIPv4Table_oid) ;
    REGISTER_MIB("ipran/hhrL2SubIfVlanTranslateTable", hhrL2SubIfVlanTranslateTable_variables, variable2,
                 hhrL2SubIfVlanTranslateTable_oid) ;
    REGISTER_MIB("ipran/hhrIFStatisticsTable", hhrIFStatisticsTable_variables, variable2,
                 hhrIFStatisticsTable_oid) ;
    REGISTER_MIB("hhrSFPConfigTable", hhrSFPConfigTable_variables, variable2,
                 hhrSFPConfigTable_oid);
}


int ifm_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                               struct ifm_snmp_info  *index)
{
    struct ifm_snmp_info *pifm_info = NULL;	
    struct ifm_snmp_info *tpifm_info = NULL;
    int data_num = 0;
    int ret = 0;

    pifm_info = ifm_get_bulk(index->ifm.ifindex, MODULE_ID_SNMPD, &data_num);
	tpifm_info = pifm_info;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    if (0 == data_num || NULL == pifm_info)
    {
    	mem_share_free_bydata(tpifm_info, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pifm_info , sizeof(struct ifm_snmp_info));
            pifm_info++;
        }
		mem_share_free_bydata(tpifm_info, MODULE_ID_SNMPD);
        return TRUE;
    }
}

int ifm_info_get_data_from_ipc1(struct ipran_snmp_data_cache *cache ,
                                struct ifm_snmp_info  *index)
{
    struct ifm_snmp_info *pifm_info = NULL;
	 struct ifm_snmp_info *tpifm_info = NULL;
    int data_num = 0;
    int ret = 0;
    int flag = 0;
    uint32_t ifindex;

    ifindex = index->ifm.ifindex;

    while (1)
    {
        pifm_info = ifm_get_bulk(ifindex, MODULE_ID_SNMPD, &data_num);
		tpifm_info = pifm_info;

        if (0 == data_num || NULL == pifm_info)
        {
        	mem_share_free_bydata(tpifm_info, MODULE_ID_SNMPD);
            return FALSE;
        }
        else
        {
            for (ret = 0; ret < data_num; ret++)
            {
                ifindex = pifm_info->ifm.ifindex;

                if (hhrSubIfEncapsulateTable_isValid(pifm_info) == 0)
                {
                    pifm_info++;
                    continue;
                }

                snmp_cache_add(cache, pifm_info , sizeof(struct ifm_snmp_info));

                pifm_info++;
                flag = 1;
            }

            if (1 == flag)
            {
            	mem_share_free_bydata(tpifm_info, MODULE_ID_SNMPD);
                return TRUE;
            }
        }
    }
	mem_share_free_bydata(tpifm_info, MODULE_ID_SNMPD);
    return FALSE;
}

struct ifm_snmp_info *ifm_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                           int exact,
                                           const struct ifm_snmp_info  *index_input)
{
    struct listnode  *node, *nnode;
    struct ifm_snmp_info *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifm.ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifm.ifindex == index_input->ifm.ifindex)
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
                    return node->next->data ;
                }
            }
        }
    }

    return (NULL);
}


int hhrL3IfIPv4Table_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                       struct ifm_l3  *index)
{
    struct ifm_l3 *pifm_info = NULL;
	struct ifm_l3 *tpifm_info = NULL;
    int data_num = 0;
    int ret = 0;

    pifm_info = ifm_get_l3if_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num);
	tpifm_info = pifm_info;

    if (0 == data_num || NULL == pifm_info)
    {
    	mem_share_free_bydata(tpifm_info, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pifm_info, sizeof(struct ifm_l3));
            pifm_info++;
        }
		mem_share_free_bydata(tpifm_info, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct ifm_l3 *hhrL3IfIPv4Table_node_lookup(struct ipran_snmp_data_cache *cache ,
                                            int exact,
                                            const struct ifm_l3  *index_input)
{
    struct listnode  *node, *nnode;
    struct ifm_l3 *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
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
                    return node->next->data ;
                }
            }
        }
    }

    return (NULL);
}

u_char *
hhrBaseIfTable_get(struct variable *vp,
                   oid *name,
                   size_t *length,
                   int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ifm_snmp_info index ;
    u_int32_t index_next = 0;
    int ret = 0;
    struct ifm_snmp_info *pifm_info = NULL;
    struct ifm_l3 *pifm_l3 ;
    struct ifm_l3 pifm_index ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifm.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrBaseIfTable_cache)
    {
        hhrBaseIfTable_cache = snmp_cache_init(sizeof(struct ifm_snmp_info) ,
                                               ifm_info_get_data_from_ipc ,
                                               ifm_info_node_lookup);

        if (NULL == hhrBaseIfTable_cache)
        {
            zlog_info("%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    if (NULL == hhrL3IfIPv4Table_cache)
    {
        hhrL3IfIPv4Table_cache = snmp_cache_init(sizeof(struct ifm_l3) ,
                                                 hhrL3IfIPv4Table_get_data_from_ipc ,
                                                 hhrL3IfIPv4Table_node_lookup);

        if (NULL == hhrL3IfIPv4Table_cache)
        {
            zlog_info("%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    pifm_info = snmp_cache_get_data_by_index(hhrBaseIfTable_cache , exact, &index);

    if (NULL == pifm_info)
    {
        return NULL;
    }

    index_next =    pifm_info->ifm.ifindex ;

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pifm_info->ifm.ifindex);
    }

    /*
    * this is where we do the value assignments for the mib results.
    */

    switch (vp->magic)
    {
        case hhrBaseIfDescr :
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);
            ret = ifm_get_name_by_ifindex(index_next, (char *) str_value);

            if (ret < 0)
            {
                zlog_err("%s[%d] ifm_get_name_by_ifindex 0x%0x erro\n", __FUNCTION__, __LINE__, index_next);
                return NULL;
            }

            *var_len = strlen((char *) str_value);
            return str_value;

        case hhrBaseIfMode :
            *var_len = sizeof(int);

            if (pifm_info->ifm.mode == IFNET_MODE_L3)
            {
                int_value = MODE_L3;
            }
            else if (pifm_info->ifm.mode == IFNET_MODE_L2)
            {
                int_value = MODE_L2;
            }
            else if (pifm_info->ifm.mode == IFNET_MODE_SWITCH)
            {
                int_value = MODE_SWITCH;
            }
            else if (pifm_info->ifm.mode == IFNET_MODE_PHYSICAL)
            {
                int_value = MODE_PHY;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *) &int_value;

        case hhrBaseIfOperStatus :
            *var_len = sizeof(int);

            if (pifm_info->ifm.status == IFNET_LINKUP)
            {
                int_value = OPERSTA_UP;
            }
            else
            {
                int_value = OPERSTA_DOWN;
            }

            return (u_char *) &int_value;

        case hhrBaseIfPhysAddress :
            *var_len = sizeof(uchar) * 6;
            memcpy(mac_value, pifm_info->ifm.mac, sizeof(uchar) * 6);
            return mac_value;

        case hhrBaseIfPortType :
            *var_len = sizeof(int);

            if (IFM_TYPE_IS_METHERNET(index_next) && (!IFM_IS_SUBPORT(index_next)))
            {

                if (pifm_info->port_info.fiber == IFNET_FIBERTYPE_FIBER)
                {
                    int_value = FIBERTYPE_FIBER;
                }
                else if (pifm_info->port_info.fiber == IFNET_FIBERTYPE_COPPER)
                {
                    int_value = FIBERTYPE_COPPER;
                }
                else
                {
                    if (pifm_info->port_info.fiber_sub == IFNET_FIBERTYPE_COPPER)
                    {
                        int_value = FIBERTYPE_COPPER;
                    }
                    else if (pifm_info->port_info.fiber_sub == IFNET_FIBERTYPE_FIBER)
                    {
                        int_value = FIBERTYPE_FIBER;
                    }
                    else
                    {
                        int_value = FIBERTYPE_OTHER;
                    }
                }
            }
            else
            {
                int_value = FIBERTYPE_OTHER;
            }

            return (u_char *) &int_value;

        case hhrBaseIfMtu :
            *var_len = sizeof(int);
            int_value = pifm_info->ifm.mtu ;
            return (u_char *) &int_value;

        case hhrBaseIfJumboFrame :
            *var_len = sizeof(int);
            int_value = pifm_info->ifm.jumbo;
            return (u_char *) &int_value;

        case hhrBaseIfAdminStatus :
            *var_len = sizeof(int);

            if (pifm_info->ifm.shutdown == IFNET_SHUTDOWN)
            {
                int_value = ADMINSTA_DOWN;
            }
            else
            {
                int_value = ADMINSTA_UP;
            }

            return (u_char *) &int_value;

        case hhrBaseIfTPId :
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);
            sprintf((char *) str_value, "0x%0x%", pifm_info->ifm.tpid);
            *var_len = strlen((char *) str_value);
            return str_value;

        case hhrBaseIfStatistics :
            *var_len = sizeof(int);

            if (IFNET_TYPE_TUNNEL == pifm_info->ifm.type)
            {
                struct tunnel_if *tnl = mpls_com_get_tunnel(index_next, MODULE_ID_SNMPD);
                struct tunnel_if *pfree = NULL;

                if (tnl != NULL)
                {
                    pfree = tnl;
                    if (tnl->tunnel.statis_enable == IFNET_STAT_ENABLE)
                    {
                        int_value = STAT_ENABLE;
                    }
                    else
                    {
                        int_value = STAT_DISABLE;
                    }
                    mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
                }
                else
                {
                    int_value = STAT_DISABLE;
                }

                return (u_char *) &int_value;
            }

            if (pifm_info->ifm.statistics == IFNET_STAT_ENABLE)
            {
                int_value = STAT_ENABLE;
            }
            else
            {
                int_value = STAT_DISABLE;
            }

            return (u_char *) &int_value;

        case hhrBaseIfWorkMode :
            *var_len = sizeof(int);
            int_value = pifm_info->port_info.autoneg;
            return (u_char *) &int_value;

        case hhrBaseIfSpeed :
            *var_len = sizeof(int);

            if (pifm_info->ifm.status == IFNET_LINKUP)
            {
                if (pifm_info->port_info.duplex == IFNET_DUPLEX_FULL)
                {
                    if (pifm_info->port_info.speed == IFNET_SPEED_GE)
                    {
                        int_value = SPEED_1000MF;
                    }
                    else if (pifm_info->port_info.speed == IFNET_SPEED_FE)
                    {
                        int_value = SPEED_100MF;
                    }
                    else if (pifm_info->port_info.speed == IFNET_SPEED_10M)
                    {
                        int_value = SPEED_10MF;
                    }
                    else if (pifm_info->port_info.speed == IFNET_SPEED_10GE)
                    {
                        int_value =  SPEED_10GF;
                    }
                    else
                    {
                        int_value = SPEED_INVALID;
                    }
                }
                else if (pifm_info->port_info.duplex == IFNET_DUPLEX_HALF)
                {
                    if (pifm_info->port_info.speed == IFNET_SPEED_GE)
                    {
                        int_value = SPEED_1000MH;
                    }
                    else if (pifm_info->port_info.speed == IFNET_SPEED_FE)
                    {
                        int_value = SPEED_100MH;
                    }
                    else if (pifm_info->port_info.speed == IFNET_SPEED_10M)
                    {
                        int_value = SPEED_10MH;
                    }
                    else if (pifm_info->port_info.speed == IFNET_SPEED_10GE)
                    {
                        int_value =  SPEED_10GH;
                    }
                    else
                    {
                        int_value = SPEED_INVALID;
                    }
                }
                else
                {
                    int_value = SPEED_INVALID;
                }
            }
            else if (pifm_info->ifm.status == IFNET_LINKDOWN)
            {
                if (pifm_info->port_info.autoneg == IFNET_SPEED_MODE_AUTO)
                {
                    int_value = SPEED_INVALID;
                }
                else if (pifm_info->port_info.autoneg == IFNET_SPEED_MODE_FORCE)
                {
                    if (pifm_info->port_info.duplex == IFNET_DUPLEX_FULL)
                    {
                        if (pifm_info->port_info.speed == IFNET_SPEED_GE)
                        {
                            int_value = SPEED_1000MF;
                        }
                        else if (pifm_info->port_info.speed == IFNET_SPEED_FE)
                        {
                            int_value = SPEED_100MF;
                        }
                        else if (pifm_info->port_info.speed == IFNET_SPEED_10M)
                        {
                            int_value = SPEED_10MF;
                        }
                        else if (pifm_info->port_info.speed == IFNET_SPEED_10GE)
                        {
                            int_value =  SPEED_10GF;
                        }
                        else
                        {
                            int_value = SPEED_INVALID;
                        }
                    }
                    else if (pifm_info->port_info.duplex == IFNET_DUPLEX_HALF)
                    {
                        if (pifm_info->port_info.speed == IFNET_SPEED_GE)
                        {
                            int_value = SPEED_1000MH;
                        }
                        else if (pifm_info->port_info.speed == IFNET_SPEED_FE)
                        {
                            int_value = SPEED_100MH;
                        }
                        else if (pifm_info->port_info.speed == IFNET_SPEED_10M)
                        {
                            int_value = SPEED_10MH;
                        }
                        else if (pifm_info->port_info.speed == IFNET_SPEED_10GE)
                        {
                            int_value =  SPEED_10GH;
                        }
                        else
                        {
                            int_value = SPEED_INVALID;
                        }
                    }
                    else
                    {
                        int_value = SPEED_INVALID;
                    }
                }
            }

            return (u_char *) &int_value;

        case hhrBaseIfFlappingControl :
            *var_len = sizeof(int);
            int_value = pifm_info->port_info.flap_period;
            return (u_char *) &int_value;

        case hhrBaseIfL3Vpn :
            *var_len = sizeof(int);

            pifm_index.ifindex = pifm_info->ifm.ifindex ;
            pifm_l3 = snmp_cache_get_data_by_index(hhrL3IfIPv4Table_cache, 1, &pifm_index);

            if (pifm_l3)
            {
                int_value = pifm_l3->vpn;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *) &int_value;

        case hhrBaseIfLoopbackStatus:
            *var_len = sizeof(int);

            //added by liub 2018-9-10, for stm loopback status
            if ((pifm_info->ifm.type == IFNET_TYPE_STM) &&
                    (pifm_info->ifm.sub_type == IFNET_SUBTYPE_STM1))
            {
                if (pifm_info->ifm.lb_flags == IFM_LOOPBACK_INTERNAL)
                {
                    int_value = INTERNAL_LOOPBACK;
                }
                else if (pifm_info->ifm.lb_flags == IFM_LOOPBACK_EXTERNEL)
                {
                    int_value = EXTERNAL_LOOPBACK;
                }
                else
                {
                    int_value = NO_LOOPBACK;
                }
            }
            else
            {
                if (64 == pifm_info->ifm.reflector.set_flag)
                {
                    if (pifm_info->ifm.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL)
                    {
                        int_value = EXTERNAL_LOOPBACK;
                    }
                    else if (pifm_info->ifm.reflector.reflector_flag == IFM_LOOPBACK_INTERNAL)
                    {
                        int_value = INTERNAL_LOOPBACK;
                    }
                    else
                    {
                        int_value = NO_LOOPBACK;
                    }

                }
                else
                {
                    if (pifm_info->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_EXTERNEL)
                    {
                        int_value = EXTERNAL_LOOPBACK;
                    }
                    else if (pifm_info->ifm.loop_info.reflector.reflector_flag == IFM_LOOPBACK_INTERNAL)
                    {
                        int_value = INTERNAL_LOOPBACK;
                    }
                    else
                    {
                        int_value = NO_LOOPBACK;
                    }

                }
            }

            return (u_char *) &int_value;

        case hhrBaseIfLoopbackType:
            *var_len = sizeof(int);

            if (64 == pifm_info->ifm.reflector.set_flag)
            {
                int_value = 0;
            }
            else if (1 == pifm_info->ifm.loop_info.set_flag && (0 != pifm_info->ifm.loop_info.reflector.reflector_flag))
            {
                int_value = 1;
            }
            else if (4 == pifm_info->ifm.loop_info.set_flag  && (0 != pifm_info->ifm.loop_info.reflector.reflector_flag))
            {
                int_value = 4;
            }

            else if (8 == pifm_info->ifm.loop_info.set_flag  && (0 != pifm_info->ifm.loop_info.reflector.reflector_flag))
            {
                int_value = 5;
            }

            else if (16 == pifm_info->ifm.loop_info.set_flag  && (0 != pifm_info->ifm.loop_info.reflector.reflector_flag))
            {
                int_value = 2;
            }

            else if (32 == pifm_info->ifm.loop_info.set_flag  && (0 != pifm_info->ifm.loop_info.reflector.reflector_flag))
            {
                int_value = 3;
            }
            else
            {
                int_value = 6;
            }

            return (u_char *)&int_value;

        case hhrBaseIfLoopbackvlan:
            *var_len = sizeof(int);

            if (IFM_LB_SVLAN_SET == pifm_info->ifm.loop_info.set_flag)
            {
                int_value = pifm_info->ifm.loop_info.svlan;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)&int_value;

        case hhrBaseIfLoopbackIP:
            *var_len = sizeof(uint32_t);

            if (IFM_LB_SIP_SET == pifm_info->ifm.loop_info.set_flag)
            {
                ip_value = htonl(pifm_info->ifm.loop_info.sip.addr);
            }
            else if (IFM_LB_DIP_SET ==  pifm_info->ifm.loop_info.set_flag)
            {
                ip_value = htonl(pifm_info->ifm.loop_info.dip.addr);
            }
            else
            {
                ip_value = htonl(0);
            }

            return (u_char *)&ip_value;

        case hhrBaseIfLoopbackMAC:
            *var_len = 6;

            if (IFM_LB_SMAC_SET == pifm_info->ifm.loop_info.set_flag)
            {
                memcpy(mac_value, pifm_info->ifm.loop_info.smac, 6);
            }
            else if (IFM_LB_DMAC_SET ==  pifm_info->ifm.loop_info.set_flag)
            {
                memcpy(mac_value, pifm_info->ifm.loop_info.dmac, 6);
            }
            else
            {
                memset(mac_value, 0x00, 6);
            }

            return mac_value;
        case hhrBaseIfAliase:
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);
            *var_len = strlen((char *) pifm_info->alias);
            memcpy(str_value,pifm_info->alias,NAME_STRING_LEN);
            return str_value;
        case hhrBaseIfMasterMode:
            *var_len = sizeof(int);
            int_value = pifm_info->ifm.phymaster;

            return (u_char *)&int_value;
        case hhrBaseIfMasterState:
            *var_len = sizeof(int);
            int_value = pifm_info->ifm.phymaster_status;

            return (u_char *)&int_value;

        default :
            return (NULL);
    }
}


u_char *
hhrL3IfIPv4Table_get(struct variable *vp,
                     oid *name,
                     size_t *length,
                     int exact, size_t *var_len, WriteMethod **write_method)
{
    struct  ifm_l3 index ;
    u_int32_t index_next = 0;
    int ret = 0;
    char ip[INET_ADDRSTRLEN] = "";
    char ip_slave[INET_ADDRSTRLEN] = "";
    struct ifm_l3 *pifm_l3;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrL3IfIPv4Table_cache)
    {
        hhrL3IfIPv4Table_cache = snmp_cache_init(sizeof(struct ifm_l3) ,
                                                 hhrL3IfIPv4Table_get_data_from_ipc ,
                                                 hhrL3IfIPv4Table_node_lookup);

        if (NULL == hhrL3IfIPv4Table_cache)
        {
            zlog_info("%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    /* lookup */
    pifm_l3 = snmp_cache_get_data_by_index(hhrL3IfIPv4Table_cache , exact, &index);

    if (NULL == pifm_l3)
    {
        return NULL;
    }

    index_next = pifm_l3->ifindex ;

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, index_next);
    }

    /*
    * this is where we do the value assignments for the mib results.
    */

    switch (vp->magic)
    {

        case hhrL3IpAddress:
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);

            inet_ipv4tostr(pifm_l3->ipv4[0].addr, ip);
            inet_ipv4tostr(pifm_l3->ipv4[1].addr, ip_slave);

            sprintf((char *) str_value, "%s/%d(master):%s/%d(slave)",
                    ip, pifm_l3->ipv4[0].prefixlen,
                    ip_slave, pifm_l3->ipv4[1].prefixlen);

            *var_len = strlen((char *) str_value);
            return str_value;

        case hhrL3IfDescr :
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);
            ret = ifm_get_name_by_ifindex(index_next, (char *) str_value);

            if (ret < 0)
            {
                zlog_err("%s[%d] ifm_get_name_by_ifindex 0x%0x erro\n", __FUNCTION__, __LINE__, index_next);
                return NULL;
            }

            *var_len = strlen((char *) str_value);
            return str_value;

        case hhrL3IfIPv4Type :
            *var_len = sizeof(pifm_l3->ipv4_flag);
            int_value = pifm_l3->ipv4_flag;
            return (u_char *) &int_value;

        case hhrL3IfUnnumberedIfIndex :
            *var_len = sizeof(pifm_l3->unnumbered_if);
            int_value = pifm_l3->unnumbered_if;
            return (u_char *) &int_value;

        case hhrL3IfUnnumberedIfDesc :
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);
            ret = ifm_get_name_by_ifindex(pifm_l3->unnumbered_if, (char *) str_value);

            if (ret < 0)
            {
                zlog_err("%s[%d] ifm_get_name_by_ifindex 0x%0x erro\n", __FUNCTION__, __LINE__,
                         pifm_l3->unnumbered_if);
                return NULL;
            }

            *var_len = strlen((char *) str_value);
            return str_value;

        default :
            return (NULL);
    }
}

int hhrSubIfEncapsulateTable_isValid(struct ifm_snmp_info *index)
{
    int ret = 0;

    if (!((IFM_TYPE_IS_METHERNET(index->ifm.ifindex) && IFM_IS_SUBPORT(index->ifm.ifindex))
            || (IFM_TYPE_IS_TRUNK(index->ifm.ifindex) && IFM_IS_SUBPORT(index->ifm.ifindex))))
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s' ifindex 0x%0x parent 0x%0x:\n", __FILE__, __LINE__,
                   __func__, index->ifm.ifindex, index->ifm.parent);
        return 0;
    }

    if (index->ifm.encap.type <= IFNET_ENCAP_INVALID
            || index->ifm.encap.type >= IFNET_ENCAP_MAX)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] ifindex 0x%0x encap.type 0x%0x ifindex 0x%0x parent 0x%0x ret %d\n",
                   __FUNCTION__, __LINE__, index->ifm.ifindex, index->ifm.encap.type, index->ifm.ifindex, index->ifm.parent);
        return 0;
    }

    return 1;
}

/*
int hhrL2SubIfVlanTranslateTable_isValid(struct ifm_snmp_info *index)
{
    int ret = 0;

    if (!((IFM_TYPE_IS_METHERNET(index->ifm.ifindex) && IFM_IS_SUBPORT(index->ifm.ifindex))
            || (IFM_TYPE_IS_TRUNK(index->ifm.ifindex) && IFM_IS_SUBPORT(index->ifm.ifindex))))
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s' ifindex 0x%0x parent 0x%0x:\n", __FILE__, __LINE__,
                   __func__, index->ifm.ifindex, index->ifm.parent);

        return 1;
    }

    if (index->ifm.encap.type <= IFNET_ENCAP_INVALID
            || index->ifm.encap.type >= IFNET_ENCAP_MAX)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] ifindex  0x%0x encap.type  0x%0x ifindex 0x%0x parent 0x%0x\n ret %d",
                   __FUNCTION__, __LINE__, index->ifm.ifindex, index->ifm.encap.type, index->ifm.ifindex, index->ifm.parent);

        return 1;
    }

    if (index->ifm.encap.cvlan_act == VLAN_ACTION_NO
            && index->ifm.encap.svlan_act == VLAN_ACTION_NO)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] ifindex  0x%0x parent 0x%0x cvlan_act  0x%0x svlan_act 0x%0x encap.type 0x%0x \n", __FUNCTION__, __LINE__,
                   index->ifm.ifindex, index->ifm.parent, index->ifm.encap.cvlan_act,
                   index->ifm.encap.svlan_act, index->ifm.encap.type);

        return 1;
    }

    return 0;
}
*/

u_char *
hhrL2SubIfVlanTranslateTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ifm_snmp_info index ;
    u_int32_t index_next = 0;
    int ret = 0;
    struct ifm_snmp_info *pifm_info = NULL;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifm.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrL2SubIfVlanTranslateTable_cache)
    {
        hhrL2SubIfVlanTranslateTable_cache = snmp_cache_init(sizeof(struct ifm_snmp_info) ,
                                                             ifm_info_get_data_from_ipc1,
                                                             ifm_info_node_lookup);

        if (NULL == hhrL2SubIfVlanTranslateTable_cache)
        {
            zlog_info("%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    pifm_info = snmp_cache_get_data_by_index(hhrL2SubIfVlanTranslateTable_cache , exact, &index);

    if (NULL == pifm_info)
    {
        return NULL;
    }

    index_next = pifm_info->ifm.ifindex;

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pifm_info->ifm.ifindex);
    }

    switch (vp->magic)
    {
        case hhrVTSubIfDescr:
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);
            ret = ifm_get_name_by_ifindex(index_next, (char *) str_value);

            if (ret < 0)
            {
                return NULL;
            }

            *var_len = strlen((char *) str_value);
            return str_value;

        case hhrVTMode :
            *var_len = sizeof(int);

            if (pifm_info->ifm.mode == IFNET_MODE_L3)
            {
                int_value = MODE_L3;
            }
            else if (pifm_info->ifm.mode == IFNET_MODE_L2)
            {
                int_value = MODE_L2;
            }
            else if (pifm_info->ifm.mode == IFNET_MODE_SWITCH)
            {
                int_value = MODE_SWITCH;
            }
            else if (pifm_info->ifm.mode == IFNET_MODE_PHYSICAL)
            {
                int_value = MODE_PHY;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *) &int_value;

        case hhrVTEncapsulate :
            *var_len = sizeof(int);
            int_value = pifm_info->ifm.encap.type;
            return (u_char *) &int_value;

        case hhrVTcvlan :
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);

            if (pifm_info->ifm.encap.cvlan.vlan_start != pifm_info->ifm.encap.cvlan.vlan_end)
            {
                sprintf((char *) str_value, "%d-%d", pifm_info->ifm.encap.cvlan.vlan_start, pifm_info->ifm.encap.cvlan.vlan_end);
            }
            else
            {
                sprintf((char *) str_value, "%d", pifm_info->ifm.encap.cvlan.vlan_start);
            }

            *var_len = strlen((char *) str_value);
            return str_value;

        case hhrVTsvlan :
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);

            if (pifm_info->ifm.encap.svlan.vlan_start != pifm_info->ifm.encap.svlan.vlan_end)
            {
                sprintf((char *) str_value, "%d-%d", pifm_info->ifm.encap.svlan.vlan_start, pifm_info->ifm.encap.svlan.vlan_end);
            }
            else
            {
                sprintf((char *) str_value, "%d", pifm_info->ifm.encap.svlan.vlan_start);
            }

            *var_len = strlen((char *) str_value);
            return str_value;

        case hhrVTTranslateType :
            *var_len = sizeof(int);
            int_value = NO_TRANS;

            if (pifm_info->ifm.encap.type == IFNET_ENCAP_UNTAG)
            {
                if (pifm_info->ifm.encap.svlan_act == VLAN_ACTION_ADD
                        && pifm_info->ifm.encap.cvlan_act == VLAN_ACTION_NO)
                {
                    int_value = TRANS_TO_ONE_LAYER;
                }
            }
            else if (pifm_info->ifm.encap.type == IFNET_ENCAP_DOT1Q)
            {
                if (pifm_info->ifm.encap.svlan_act == VLAN_ACTION_ADD
                        && pifm_info->ifm.encap.cvlan_act == VLAN_ACTION_NO)
                {
                    int_value = TRANS_TO_TOW_LAYER;
                }
                else if (pifm_info->ifm.encap.svlan_act == VLAN_ACTION_TRANSLATE
                         && pifm_info->ifm.encap.cvlan_act == VLAN_ACTION_NO)
                {
                    int_value = TRANS_TO_ONE_LAYER;
                }
                else if (pifm_info->ifm.encap.svlan_act == VLAN_ACTION_ADD
                         && pifm_info->ifm.encap.cvlan_act == VLAN_ACTION_TRANSLATE)
                {
                    int_value = TRANS_TO_TOW_LAYER;
                }

            }
            else if (pifm_info->ifm.encap.type == IFNET_ENCAP_QINQ)
            {
                if (pifm_info->ifm.encap.svlan_act == VLAN_ACTION_TRANSLATE
                        && pifm_info->ifm.encap.cvlan_act == VLAN_ACTION_DELETE)
                {
                    int_value = TRANS_TO_ONE_LAYER;
                }
                else if (pifm_info->ifm.encap.svlan_act == VLAN_ACTION_TRANSLATE
                         && pifm_info->ifm.encap.cvlan_act == VLAN_ACTION_TRANSLATE)
                {
                    int_value = TRANS_TO_TOW_LAYER;
                }
                else if (pifm_info->ifm.encap.svlan_act == VLAN_ACTION_TRANSLATE
                         && pifm_info->ifm.encap.cvlan_act == VLAN_ACTION_NO)
                {
                    int_value = TRANS_OUT_KEEP_INNER;
                }
                else if (pifm_info->ifm.encap.svlan_act == VLAN_ACTION_DELETE
                         && pifm_info->ifm.encap.cvlan_act == VLAN_ACTION_NO)
                {
                    int_value = DELETE_OUT_KEEP_INNER;
                }

            }

            return (u_char *) &int_value;

        case hhrVTTranslatedcvlan :
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);

            if (pifm_info->ifm.encap.cvlan_new.vlan_start != pifm_info->ifm.encap.cvlan_new.vlan_end)
            {
                sprintf((char *) str_value, "%d-%d", pifm_info->ifm.encap.cvlan_new.vlan_start, pifm_info->ifm.encap.cvlan_new.vlan_end);
            }
            else
            {
                sprintf((char *) str_value, "%d", pifm_info->ifm.encap.cvlan_new.vlan_start);
            }

            *var_len = strlen((char *) str_value);
            return str_value;

        case hhrVTTranslatedcvlanCos :
            *var_len = sizeof(int);
            int_value = pifm_info->ifm.encap.cvlan_cos;
            return (u_char *) &int_value;

        case hhrVTTranslatedsvlan :
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);

            if (pifm_info->ifm.encap.svlan_new.vlan_start != pifm_info->ifm.encap.svlan_new.vlan_end)
            {
                sprintf((char *) str_value, "%d-%d", pifm_info->ifm.encap.svlan_new.vlan_start, pifm_info->ifm.encap.cvlan_new.vlan_end);
            }
            else
            {
                sprintf((char *) str_value, "%d", pifm_info->ifm.encap.svlan_new.vlan_start);
            }

            *var_len = strlen((char *) str_value);
            return str_value;

        case hhrVTTranslatedsvlanCos :
            *var_len = sizeof(int);
            int_value = pifm_info->ifm.encap.svlan_cos;
            return (u_char *) &int_value;

        default :
            return (NULL);
    }
}


u_char *
hhrIFStatisticsTable_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ifm_snmp_info index ;
    u_int32_t index_next = 0;
    int ret = 0;
    struct ifm_snmp_info *pifm_info = NULL;
    u_char      sIndex[IFNET_NAMESIZE]      = {0};
    u_char      sNext_index[IFNET_NAMESIZE] = {0};
    struct ifm_counter   *pcounter = NULL;
	struct ipc_mesg_n *pMsgRcv = NULL;

    /* validate the index */
    ret = ipran_snmp_octstring_index_get(vp, name, length, sIndex, IFNET_NAMESIZE, exact);

    if (0 > ret)
    {
        return NULL;
    }

    if (0x00 == sIndex[0] && 1 == exact)
    {
        zlog_err("%s[%d]:'%s': if (0x00 ==sIndex[0] && 1 == exact )\n", __FILE__, __LINE__, __func__);
        return (NULL);
    }

    index.ifm.ifindex = ifm_get_ifindex_by_name2((char *) sIndex);

    if (NULL == hhrBaseIfTable_cache)
    {
        hhrBaseIfTable_cache = snmp_cache_init(sizeof(struct ifm_snmp_info) ,
                                               ifm_info_get_data_from_ipc ,
                                               ifm_info_node_lookup);

        if (NULL == hhrBaseIfTable_cache)
        {
            zlog_info("%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    while (1)
    {
        pifm_info = snmp_cache_get_data_by_index(hhrBaseIfTable_cache, exact, &index);

        if (NULL == pifm_info)
        {
            return NULL;
        }

        index_next = pifm_info->ifm.ifindex;
		pMsgRcv = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_SNMPD,
            IPC_TYPE_IFM, IFNET_INFO_COUNTER, IPC_OPCODE_GET, index_next, 2000);

		if(NULL == pMsgRcv)
		{
			continue;
		}
		
		pcounter = (struct ifm_counter *)pMsgRcv->msg_data;
        if (pcounter == NULL)
        {
            index.ifm.ifindex = index_next;
            continue;
        }
        else
        {
            /* get ready the next index */
            if (!exact)
            {
                ret = ifm_get_name_by_ifindex(index_next, sNext_index);

                if (ret < 0)
                {
                    index.ifm.ifindex = index_next;
                    continue ;

                }

                ipran_snmp_octstring_index_set(vp, name, length, sNext_index, strlen(sNext_index));
            }

            break;
        }
    }

    if (NULL == pcounter)
    {
		if(pMsgRcv)
		{
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
		}
        return (NULL);
    }

		
		#if 0
        pcounter = ipc_send_hal_wait_reply(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_SNMPD,
                                           IPC_TYPE_IFM, IFNET_INFO_COUNTER, IPC_OPCODE_GET, index_next);

        if (pcounter == NULL)
        {
            if (0 == exact)  //getNext
            {
                index.ifm.ifindex = index_next;
                continue;
            }
            else  //get
            {
                return NULL  ;
            }
        }

        break;
    }

    /* get ready the next index */
    if (!exact)
    {
        ret = ifm_get_name_by_ifindex(index_next, sNext_index);

        if (ret < 0)
        {
            return NULL;
        }

        ipran_snmp_octstring_index_set(vp, name, length, sNext_index, strlen(sNext_index));
    }

    #endif
	switch (vp->magic)
    {
        case hhrISInputTotalBytes :
            uint64_value.high = (pcounter->rx_bytes >> 32);
            uint64_value.low  = (pcounter->rx_bytes);
            *var_len = sizeof(struct counter64);
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrISInputTotalPackets :
            uint64_value.high = (pcounter->rx_packets >> 32);
            uint64_value.low  = (pcounter->rx_packets);
            *var_len = sizeof(struct counter64);
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrISInputDropPackets :
            uint64_value.high = (pcounter->rx_dropped >> 32);
            uint64_value.low  = (pcounter->rx_dropped);
            *var_len = sizeof(struct counter64);
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrISOutputTotalBytes :
            uint64_value.high = (pcounter->tx_bytes >> 32);
            uint64_value.low  = (pcounter->tx_bytes);
            *var_len = sizeof(struct counter64);
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrISOutputTotalPackets :
            uint64_value.high = (pcounter->tx_packets >> 32);
            uint64_value.low  = (pcounter->tx_packets);
            *var_len = sizeof(struct counter64);
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrISOutputDropPackets :
            uint64_value.high = (pcounter->tx_dropped >> 32);
            uint64_value.low  = (pcounter->tx_dropped);
            *var_len = sizeof(struct counter64);
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrISInputMulticastPackets :
            uint64_value.high = (pcounter->rx_multicast >> 32);
            uint64_value.low  = (pcounter->rx_multicast);
            *var_len = sizeof(struct counter64);
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrISInputBroadcastPackets :
            uint64_value.high = (pcounter->rx_broadcast >> 32);
            uint64_value.low  = (pcounter->rx_broadcast);
            *var_len = sizeof(struct counter64);
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrISInputUnicastPackets :
            uint64_value.high = (pcounter->rx_ucast >> 32);
            uint64_value.low  = (pcounter->rx_ucast);
            *var_len = sizeof(struct counter64);
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrISOutputMulticastPackets :
            uint64_value.high = (pcounter->tx_multicast >> 32);
            uint64_value.low  = (pcounter->tx_multicast);
            *var_len = sizeof(struct counter64);
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrISOutputBroadcastPackets :
            uint64_value.high = (pcounter->tx_broadcast >> 32);
            uint64_value.low  = (pcounter->tx_broadcast);
            *var_len = sizeof(struct counter64);
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrISOutputUnicastPackets :
            uint64_value.high = (pcounter->tx_ucast >> 32);
            uint64_value.low  = (pcounter->tx_ucast);
            *var_len = sizeof(struct counter64);
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;
		case hhrISBandwidthUtlization :
            *var_len = sizeof(int);
            int_value = pcounter->rx_width ;
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *) &int_value;

			
        default :
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (NULL);
    }
}


int hhrSFPConfigTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct ifm_snmp_sfp *index)
{
    int data_num = 0;
    int ret = 0;
    int i = 0;
    struct ifm_snmp_sfp *pdata = NULL;
	struct ifm_snmp_sfp *tpdata = NULL;

    pdata = (struct ifm_snmp_sfp *)ifm_get_sfp_if_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num);
	tpdata = pdata;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (0 == data_num)
    {
    	mem_share_free_bydata(tpdata, MODULE_ID_SNMPD);
        return FALSE;
    }

    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(hhrSFPConfigTable_cache, pdata, sizeof(struct ifm_snmp_sfp));
            pdata ++;
        }
		mem_share_free_bydata(tpdata, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct ifm_snmp_sfp *hhrSFPConfigTable_lookup(struct ipran_snmp_data_cache *cache,
                                              int exact,
                                              const struct ifm_snmp_sfp *index_input)
{
    struct listnode  *node;
    struct ifm_snmp_sfp  *data1_find = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, data1_find))
    {
        if (NULL == data1_find || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return NULL;
        }

        if (0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
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

    return NULL;
}


uchar *hhrSFPConfigTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ifm_snmp_sfp *pdata;
    struct ifm_snmp_sfp data_index;
    int if_index;
    int ret = 0;
    ret = ipran_snmp_int_index_get(vp, name, length, &if_index, exact);

    if (0 > ret)
    {
        return NULL;
    }

    if (NULL == hhrSFPConfigTable_cache)
    {
        hhrSFPConfigTable_cache = snmp_cache_init(sizeof(struct ifm_snmp_sfp),
                                                  hhrSFPConfigTable_data_form_ipc,
                                                  hhrSFPConfigTable_lookup);

        if (NULL == hhrSFPConfigTable_cache)
        {
            return (NULL);
        }
    }

    data_index.ifindex = if_index;

    pdata = snmp_cache_get_data_by_index(hhrSFPConfigTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->ifindex);
    }

    switch (vp->magic)
    {
        case hhrSFPAlsIfDescr:
            memset(str_value, 0, sizeof(str_value));
            ifm_get_name_by_ifindex(pdata->ifindex, str_value);
            *var_len = strlen((char *) str_value);
            return (str_value);

        case hhSFPAlsEnable:
            int_value = pdata->sfp_als;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhSFPTsEnable:
            int_value = pdata->sfp_tx;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrSFPAlsOffTime:
            int_value = pdata->sfp_off;
            *var_len = sizeof(int);;
            return (u_char *)(&int_value);

        case hhrSFPAlsOnTime:
            int_value = pdata->sfp_on;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default:
            return NULL;
    }
}


