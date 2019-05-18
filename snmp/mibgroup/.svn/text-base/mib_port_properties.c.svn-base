
#include <stdio.h>
#include <string.h>
#include <assert.h>
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
#include <lib/oam_common.h>
#include <l2/l2_if.h>


#include "ipran_snmp_data_cache.h"
#include "net-snmp-config.h"
#include "types.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"
#include "snmp_index_operater.h"

#include "mib_port_properties.h"

static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value = 0;


static struct ipran_snmp_data_cache *hhrPortStormSuppressConfigTable_cache = NULL ;
static oid hhrPortStormSuppressConfigTable_oid[] = {HHR_IF_EXT_OID, 15};
FindVarMethod hhrPortStormSuppressConfigTable_get;
struct variable2 hhrPortStormSuppressConfigTable_variables[] =
{
    {hhrPortStormSuppressIfDescr,             ASN_OCTET_STR,   RONLY, hhrPortStormSuppressConfigTable_get, 2, {1, 1}},
    {hhrPortStormSuppressUnicast,    ASN_INTEGER,   RONLY, hhrPortStormSuppressConfigTable_get, 2, {1, 2}},
    {hhrPortStormSuppressBroadcast,    ASN_INTEGER,   RONLY, hhrPortStormSuppressConfigTable_get, 2, {1, 3}},
    {hhrPortStormSuppressMulticast,     ASN_INTEGER,   RONLY, hhrPortStormSuppressConfigTable_get, 2, {1, 4}},
    {hhrPortStormSuppressUnicastRate,     ASN_INTEGER, RONLY, hhrPortStormSuppressConfigTable_get, 2, {1, 5}},
    {hhrPortStormSuppressBroadcastRate,     ASN_INTEGER,   RONLY, hhrPortStormSuppressConfigTable_get, 2, {1, 6}},
    {hhrPortStormSuppressMulticastRate,           ASN_INTEGER,   RONLY, hhrPortStormSuppressConfigTable_get, 2, {1, 7}},
};

static struct ipran_snmp_data_cache *hhrPortDot1qTunnelConfigTable_cache = NULL ;
static oid hhrPortDot1qTunnelConfigTable_oid[] = {HHR_IF_EXT_OID, 17};
FindVarMethod hhrPortDot1qTunnelConfigTable_get;
struct variable2 hhrPortDot1qTunnelConfigTable_variables[] =
{
    {hhrPortDot1qTunnelIfDescr,       ASN_OCTET_STR,   RONLY, hhrPortDot1qTunnelConfigTable_get, 2, {1, 1}},
    {hhrPortDot1qTunnelSVlan,    ASN_INTEGER,   RONLY, hhrPortDot1qTunnelConfigTable_get, 2, {1, 2}},
    {hhrPortDot1qTunnelCos,      ASN_INTEGER,   RONLY, hhrPortDot1qTunnelConfigTable_get, 2, {1, 3}},
};

/*
static struct ipran_snmp_data_cache *hhrSubIfManageIPTable_cache = NULL ;
static oid hhrSubIfManageIPTable_oid[] = {HHR_IF_EXT_OID, 22};

FindVarMethod hhrSubIfManageIPTable_get;
struct variable2 hhrSubIfManageIPTable_variables[] =
{
    {hhrSubIfManageIPIfDescr,       ASN_OCTET_STR,   RONLY, hhrSubIfManageIPTable_get, 2, {1, 1}},
    {hhrSubIfManageIPEnable,    ASN_INTEGER,   RONLY, hhrSubIfManageIPTable_get, 2, {1, 2}},
};
*/

void init_mib_port_properties(void)
{
    REGISTER_MIB("hhrPortStormSuppressConfigTable", hhrPortStormSuppressConfigTable_variables, variable2,
                 hhrPortStormSuppressConfigTable_oid);
    REGISTER_MIB("hhrPortDot1qTunnelConfigTable", hhrPortDot1qTunnelConfigTable_variables, variable2,
                 hhrPortDot1qTunnelConfigTable_oid);
   /* REGISTER_MIB("hhrSubIfManageIPTable", hhrSubIfManageIPTable_variables, variable2,
                 hhrSubIfManageIPTable_oid);    */
}

int hhrPortStormSuppressConfigTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                    struct mib_l2if_storm_control_info  *index)
{
    struct mib_l2if_storm_control_info *storm_info = NULL;
	struct mib_l2if_storm_control_info *tstorm_info = NULL;
    int data_num = 0;
    int ret = 0;
		
    storm_info = l2if_get_storm_suppress_bulk(index->ifindex.ifindex, MODULE_ID_SNMPD, &data_num);
	tstorm_info = storm_info;
    if (0 == data_num || NULL == storm_info)
    {
    	mem_share_free_bydata(tstorm_info, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, storm_info , sizeof(struct mib_l2if_storm_control_info)) ;
            storm_info++;
        }
		mem_share_free_bydata(tstorm_info, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct mib_l2if_storm_control_info *hhrPortStormSuppressConfigTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                        int exact,
                                                        const struct mib_l2if_storm_control_info  *index_input)
{
    struct listnode  *node, *nnode;
    struct mib_l2if_storm_control_info *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex.ifindex )
        {
            return cache->data_list->head->data;
        }

        if (data1_find->ifindex.ifindex == index_input->ifindex.ifindex)
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


uchar *hhrPortStormSuppressConfigTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct  mib_l2if_storm_control_info *pdata = NULL;
    struct mib_l2if_storm_control_info index;
    int ret = 0;
    int ifindex = 0;

    memset(&index , 0 , sizeof( struct mib_l2if_storm_control_info));
    
    ret = ipran_snmp_int_index_get(vp, name, length, &ifindex , exact);

    index.ifindex.ifindex = ifindex ;

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrPortStormSuppressConfigTable_cache)
    {
        hhrPortStormSuppressConfigTable_cache = snmp_cache_init(sizeof(struct mib_l2if_storm_control_info),
                                                  hhrPortStormSuppressConfigTable_get_data_from_ipc,
                                                  hhrPortStormSuppressConfigTable_node_lookup);

        if (NULL == hhrPortStormSuppressConfigTable_cache)
        {
            return (NULL);
        }
    }

    pdata = snmp_cache_get_data_by_index(hhrPortStormSuppressConfigTable_cache, exact, &index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->ifindex.ifindex);
    }

    switch (vp->magic)
    {
        case hhrPortStormSuppressIfDescr:
            ifm_get_name_by_ifindex(pdata->ifindex.ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrPortStormSuppressUnicast:
            *var_len = sizeof(int);
            int_value = pdata->data.storm_unicast ;
            if (1 == int_value)
            {
                int_value = 1 ;
            }
            else
            {
                int_value = 2 ;
            }
            return (u_char *)&int_value;
            
        case hhrPortStormSuppressBroadcast:
            *var_len = sizeof(int);
            int_value = pdata->data.storm_broadcast ;
            if (1 == int_value)
            {
                int_value = 1 ;
            }
            else
            {
                int_value = 2 ;
            }
            return (u_char *)&int_value;

        case hhrPortStormSuppressMulticast:
            *var_len = sizeof(int);
            int_value = pdata->data.storm_multicast ;
            if (1 == int_value)
            {
                int_value = 1 ;
            }
            else
            {
                int_value = 2 ;
            }
            return (u_char *)&int_value;

        case hhrPortStormSuppressUnicastRate:
            *var_len = sizeof(int);
            int_value = pdata->data.unicast_rate ;            
            return (u_char *)&int_value;

        case hhrPortStormSuppressBroadcastRate:
            *var_len = sizeof(int);
            int_value = pdata->data.broadcast_rate ;            
            return (u_char *)&int_value;

        case hhrPortStormSuppressMulticastRate:
            *var_len = sizeof(int);
            int_value = pdata->data.multicast_rate ;            
            return (u_char *)&int_value;

        default:
            return NULL;
    }
}

int hhrPortDot1qTunnelConfigTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                    struct mib_l2if_dot1q_tunnel_info  *index)
{
    struct mib_l2if_dot1q_tunnel_info *pdot1qTunnel = NULL;
    int data_num = 0;
    int ret = 0;
	
    pdot1qTunnel = l2if_get_dot1q_tunnel_bulk(index->ifindex.ifindex, MODULE_ID_SNMPD, &data_num);
    if (0 == data_num || NULL == pdot1qTunnel)
    {
        mem_share_free_bydata(pdot1qTunnel, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
        	snmp_cache_add(cache, pdot1qTunnel , sizeof(struct mib_l2if_dot1q_tunnel_info)) ;
            pdot1qTunnel++;
        }

        mem_share_free_bydata(pdot1qTunnel, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct mib_l2if_dot1q_tunnel_info *hhrPortDot1qTunnelConfigTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                        int exact,
                                                        const struct mib_l2if_dot1q_tunnel_info  *index_input)
{
    struct listnode  *node, *nnode;
    struct mib_l2if_dot1q_tunnel_info *data1_find;
	
    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 
			0 == index_input->ifindex.ifindex)
        {
            return cache->data_list->head->data;
        }

        if (data1_find->ifindex.ifindex == index_input->ifindex.ifindex)
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


uchar *hhrPortDot1qTunnelConfigTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct  mib_l2if_dot1q_tunnel_info *pdata = NULL;
    struct mib_l2if_dot1q_tunnel_info index;
    int ret = 0;
    int ifindex = 0;

    memset(&index , 0 , sizeof( struct mib_l2if_dot1q_tunnel_info));
    
    ret = ipran_snmp_int_index_get(vp, name, length, &ifindex , exact);

    index.ifindex.ifindex = ifindex ;

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrPortDot1qTunnelConfigTable_cache)
    {
        hhrPortDot1qTunnelConfigTable_cache = snmp_cache_init(sizeof(struct mib_l2if_dot1q_tunnel_info),
                                                  hhrPortDot1qTunnelConfigTable_get_data_from_ipc,
                                                  hhrPortDot1qTunnelConfigTable_node_lookup);

        if (NULL == hhrPortDot1qTunnelConfigTable_cache)
        {
            return (NULL);
        }
    }

    pdata = snmp_cache_get_data_by_index(hhrPortDot1qTunnelConfigTable_cache, exact, &index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->ifindex.ifindex);	
    }

    switch (vp->magic)
    {
        case hhrPortDot1qTunnelIfDescr:	
            ifm_get_name_by_ifindex(pdata->ifindex.ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);	

        case hhrPortDot1qTunnelSVlan:
            *var_len = sizeof(int);
            int_value = pdata->data.dot1q_tunnel ;            
            return (u_char *)&int_value;

        case hhrPortDot1qTunnelCos:
            *var_len = sizeof(int);
            int_value = pdata->data.dot1q_cos ;            
            return (u_char *)&int_value;

        default:
            return NULL;
    }
}

