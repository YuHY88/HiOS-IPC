#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <net-snmp-config.h>
#include <types.h>
#include <net-snmp-includes.h>
#include <net-snmp-agent-includes.h>
#include <snmp_index_operater.h>

#include <lib/types.h>
#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include <l2/l2_if.h>
#include <l2/l2_snmp.h>
#include <l2/efm/efm_snmp.h>
#include <lib/linklist.h>
#include <ifm/ifm_message.h>

#include "ipran_snmp_data_cache.h"

#include "mib_mstp.h"
#include "l2/mstp/mstp_api.h"

#include "snmp_config_table.h"

static  uint8_t mac_value[MAC_LEN] = {0};
static uint8_t name_value[NAME_LEN] = {0};
static uint8_t vlan_value[ STRING_LEN ] = {'\0'};
static uint8_t bridge_value[BRIDGE_ID] = {0};
static  uint32_t   int_value = 0;
static  uint16_t   short_value = 0;
static 	uint8_t    bits_value = 0;

static struct ipran_snmp_data_cache *mstp_global_config_cache = NULL ;
static struct ipran_snmp_data_cache *mstp_port_info_cache = NULL ;
static struct ipran_snmp_data_cache *mstp_instance_cache = NULL ;
static struct ipran_snmp_data_cache *mstp_instance_port_cache = NULL ;
static struct ipran_snmp_data_cache *mstp_port_statistics_cache = NULL ;



FindVarMethod  hhrMstpGlobalConfig_get;

struct variable1 hhrMstpGlobalConfig[] =
{ 

	{MstpRegionName, ASN_OCTET_STR,       RONLY, hhrMstpGlobalConfig_get, 1, {1}},
	{MstpRevisionLevel,ASN_INTEGER,       RONLY, hhrMstpGlobalConfig_get, 1, {2}},
	{MstpVlanMap,  ASN_OCTET_STR,         RONLY, hhrMstpGlobalConfig_get, 1, {3}},
	{MstpMode,ASN_INTEGER,                RONLY, hhrMstpGlobalConfig_get, 1, {4}},
	{MstpBridgeId,ASN_OCTET_STR,          RONLY, hhrMstpGlobalConfig_get, 1, {5}},
	{MstpRootBridgeId,ASN_OCTET_STR,      RONLY, hhrMstpGlobalConfig_get, 1, {6}},
	{MstpRegionRootBridgeId,ASN_OCTET_STR,RONLY, hhrMstpGlobalConfig_get, 1, {7}},
	{MstpInternRootPathCost,ASN_INTEGER,  RONLY, hhrMstpGlobalConfig_get, 1, {8}},
	{MstpExternRootPathCost,ASN_INTEGER,   RONLY, hhrMstpGlobalConfig_get,1, {9}},
	{MstpBridgeRole,ASN_INTEGER,           RONLY, hhrMstpGlobalConfig_get,1, {10}},
	{MstpBridgePriority,ASN_INTEGER,       RONLY, hhrMstpGlobalConfig_get,1,{11}},  
	{MstpHelloTime,ASN_INTEGER,            RONLY, hhrMstpGlobalConfig_get,1,{12}},
	{MstpFowardDelay,ASN_INTEGER,          RONLY, hhrMstpGlobalConfig_get,1,{13}},
	{MstpMaxAge,ASN_INTEGER,               RONLY, hhrMstpGlobalConfig_get,1,{14}},
	{MstpMaxHopCount,ASN_INTEGER,          RONLY, hhrMstpGlobalConfig_get,1,{15}}


} ;

oid hhrMstpGlobalConfig_oid[] = {MSTP_OID};

FindVarMethod hhrMstpPortInfoTable_get;

struct variable2 hhrMstpPortInfoTable[] =
{
	{MstpPortProtocalState,  ASN_INTEGER, RONLY, hhrMstpPortInfoTable_get, 2, {1, 1}},
	{MstpPortState, ASN_INTEGER,    RONLY, hhrMstpPortInfoTable_get, 2, {1, 2}},
	{MstpPortMode,  ASN_INTEGER,    RONLY, hhrMstpPortInfoTable_get, 2, {1, 3}},
	{MstpPortDesignatedBridgeId,  ASN_OCTET_STR, RONLY, hhrMstpPortInfoTable_get, 2, {1, 4}},
	{MstpPortRole,  ASN_INTEGER, RONLY, hhrMstpPortInfoTable_get, 2, {1, 5}},
	{MstpPortPriority,  ASN_INTEGER, RONLY, hhrMstpPortInfoTable_get, 2, {1, 6}},
	{MstpPortPathCost,  ASN_INTEGER, RONLY, hhrMstpPortInfoTable_get, 2, {1, 7}},
	{MstpPortMessage,  ASN_INTEGER, RONLY, hhrMstpPortInfoTable_get,  2, {1, 8}},
	{MstpPortEdgeState,  ASN_INTEGER, RONLY, hhrMstpPortInfoTable_get, 2, {1, 9}},
	{MstpPortFilterState, ASN_INTEGER, RONLY, hhrMstpPortInfoTable_get, 2, {1, 10}},
	{MstpPortP2P, ASN_INTEGER, RONLY, hhrMstpPortInfoTable_get, 2, {1, 11}}


} ;
oid   hhrMstpPortInfoTable_oid[]   = { MSTP_OID, 16};


FindVarMethod hhrMstpInstanceTable_get;

struct variable2 hhrMstpInstanceTable[] = 
{
	{MstpInstanceId,  ASN_INTEGER, RONLY,hhrMstpInstanceTable_get, 2, {1, 1}},
	{MstpInstanceVlanMap,  ASN_OCTET_STR, RONLY, hhrMstpInstanceTable_get, 2, {1, 2}},
	{MstpInstanceBridgePriority,  ASN_INTEGER, RONLY,hhrMstpInstanceTable_get, 2, {1, 3}},
	{MstpInstanceBridgeId,  ASN_OCTET_STR, RONLY, hhrMstpInstanceTable_get, 2, {1, 4}},
	{MstpInstanceRegRootBridgeId,  ASN_OCTET_STR, RONLY, hhrMstpInstanceTable_get, 2, {1, 5}},
	{MstpInstanceInternalRootPathCost,  ASN_INTEGER, RONLY,hhrMstpInstanceTable_get, 2, {1, 6}},
};

oid  hhrMstpInstanceTable_oid[]   = { MSTP_OID, 17};

FindVarMethod hhrMstpInstancePortTable_get;

struct variable2 hhrMstpInstancePortTable[] = 
{
	{MstpInstancePortState,  ASN_INTEGER, RONLY,             hhrMstpInstancePortTable_get, 2, {1, 1}},
	{MstpInstancePortRole,  ASN_INTEGER, RONLY,              hhrMstpInstancePortTable_get, 2, {1, 2}},
	{MstpInstancePortPriority,  ASN_INTEGER, RONLY,          hhrMstpInstancePortTable_get, 2, {1, 3}},
	{MstpInstancePortPathCost,  ASN_INTEGER, RONLY,          hhrMstpInstancePortTable_get, 2, {1, 4}},
	{MstpInstancePortMessage,  ASN_INTEGER, RONLY,           hhrMstpInstancePortTable_get, 2, {1, 5}},
	{MstpInstancePortDesignalBridgeId,  ASN_OCTET_STR, RONLY,hhrMstpInstancePortTable_get, 2, {1, 6}},
	{MstpInstancePortMaxHop,  ASN_INTEGER, RONLY,            hhrMstpInstancePortTable_get, 2, {1, 7}},
	{MstpInstancePortEdgeState,  ASN_INTEGER, RONLY,         hhrMstpInstancePortTable_get, 2, {1, 8}},
	{MstpInstancePortFilterState,  ASN_INTEGER, RONLY,       hhrMstpInstancePortTable_get, 2, {1, 9}},
	{MstpInstancePortP2p,  ASN_INTEGER, RONLY,               hhrMstpInstancePortTable_get, 2, {1, 10}},

};

oid  hhrMstpInstancePortTable_oid[]   = { MSTP_OID, 18};

FindVarMethod hhrMstpStatisticsTable_get;

struct variable2 hhrMstpStatisticsTable[] = 
{
	{MstpPortTcSend,   ASN_INTEGER, RONLY,    hhrMstpStatisticsTable_get, 2, {1, 1}},
	{MstpPortTcRecv,  ASN_INTEGER, RONLY,     hhrMstpStatisticsTable_get, 2, {1, 2}},
	{MstpPortTcnSend,  ASN_INTEGER, RONLY,    hhrMstpStatisticsTable_get, 2, {1, 3}},
	{MstpPortTcnRecv,  ASN_INTEGER, RONLY,    hhrMstpStatisticsTable_get, 2, {1, 4}},
	{MstpPortBpduSend,  ASN_INTEGER, RONLY,   hhrMstpStatisticsTable_get, 2, {1, 5}},
	{MstpPortBpduRecv,  ASN_INTEGER, RONLY,   hhrMstpStatisticsTable_get, 2, {1, 6}},
	{MstpPortConfigSend,  ASN_INTEGER, RONLY, hhrMstpStatisticsTable_get, 2, {1, 7}},
	{MstpPortConfigRecv,  ASN_INTEGER, RONLY, hhrMstpStatisticsTable_get, 2, {1, 8}},
	{MstpPortRstpSend,   ASN_INTEGER, RONLY,  hhrMstpStatisticsTable_get, 2, {1, 9}},
	{MstpPortRstpRecv,  ASN_INTEGER, RONLY,   hhrMstpStatisticsTable_get, 2, {1, 10}},
	{MstpPortMstpSend,  ASN_INTEGER, RONLY,   hhrMstpStatisticsTable_get, 2, {1, 11}},
	{MstpPortMstpRecv,  ASN_INTEGER, RONLY,   hhrMstpStatisticsTable_get, 2, {1, 12}},

};

oid  hhrMstpStatisticsTable_oid[]   = { MSTP_OID, 19};

void  init_mib_mstp(void)
{
	REGISTER_MIB("hhrMstpGlobalConfigTable", hhrMstpGlobalConfig, variable1,
			hhrMstpGlobalConfig_oid) ;

	REGISTER_MIB("hhrMstpPortInfoTable", hhrMstpPortInfoTable, variable2,
			hhrMstpPortInfoTable_oid) ;

	REGISTER_MIB("hhrMstpInstanceTable", hhrMstpInstanceTable, variable2,
			hhrMstpInstanceTable_oid) ;

	REGISTER_MIB("hhrMstpInstancePortTable", hhrMstpInstancePortTable, variable2,
			hhrMstpInstancePortTable_oid) ;

	REGISTER_MIB("hhrMstpStatisticsTable", hhrMstpStatisticsTable, variable2,
			hhrMstpStatisticsTable_oid) ;
}

	u_char *
hhrMstpGlobalConfig_get(struct variable *vp,
		oid *name,
		size_t *length,
		int exact, size_t *var_len, WriteMethod **write_method)
{
	struct ipc_mesg_n  *pmesg = NULL;
	struct mstp_global_config_snmp * mstp_info_temp = NULL;
	struct mstp_global_config_snmp mstp_info;
	int data_num = 0;

	
	if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }
	
	pmesg = snmp_get_mstp_info_bulk(0, MODULE_ID_SNMPD, IPC_TYPE_SNMP_MSTP_INFO, MSTP_GLOBAL_CONFIG_TYPE);
	if (NULL == pmesg)
	{
		return NULL;
	}

	
	mstp_info_temp = (struct mstp_global_config_snmp *)pmesg->msg_data;
	data_num = pmesg->msghdr.data_num;
	
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);
	if(!data_num || (NULL == mstp_info_temp))
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return NULL;
	}
	else
	{
		memset(&mstp_info,0,sizeof(mstp_info));
		memcpy(&mstp_info,mstp_info_temp,sizeof(mstp_info));
		
		mem_share_free(pmesg, MODULE_ID_SNMPD);
	}

	switch (vp->magic)
	{
		case MstpRegionName:

			memcpy(name_value,mstp_info.global_region_name,strlen(mstp_info.global_region_name));
			*var_len = strlen(mstp_info.global_region_name);
			return (u_char *)name_value;

		case MstpRevisionLevel:
			int_value = mstp_info.global_revision_level;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case MstpVlanMap:
			memcpy(vlan_value,mstp_info.global_vlan_Map,strlen(mstp_info.global_vlan_Map));
			*var_len = strlen(mstp_info.global_vlan_Map);
			return (u_char *)vlan_value;

		case MstpMode:
			bits_value = mstp_info.global_mode;
			*var_len = sizeof(uint8_t);
			return (u_char *)&bits_value;

		case MstpBridgeId:
			memcpy(bridge_value,mstp_info.global_bridge_id,BRIDGE_ID);
			*var_len = BRIDGE_ID;
			return (u_char *)bridge_value;

		case MstpRootBridgeId:
			memcpy(bridge_value,mstp_info.global_root_bridge_id,BRIDGE_ID);
			*var_len = BRIDGE_ID;
			return (u_char *)bridge_value;


		case MstpRegionRootBridgeId:
			memcpy(bridge_value,mstp_info.global_region_root_bridge_id,BRIDGE_ID);
			*var_len = BRIDGE_ID;
			return (u_char *)bridge_value;


		case MstpInternRootPathCost:
			int_value = mstp_info.global_intern_root_path_cost;
			*var_len = sizeof(mstp_info.global_intern_root_path_cost);
			return (u_char *)&int_value;

		case MstpExternRootPathCost:
			int_value = mstp_info.global_extern_root_path_cost;
			*var_len = sizeof(mstp_info.global_extern_root_path_cost);
			return (u_char *)&int_value;

		case MstpBridgeRole:
			bits_value = mstp_info.global_bridge_role;
			*var_len = sizeof(mstp_info.global_bridge_role);
			return (u_char *)&bits_value;

		case MstpBridgePriority:
			int_value = mstp_info.global_bridge_priority;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case  MstpHelloTime:
			bits_value = mstp_info.global_hello_time;
			*var_len = sizeof(mstp_info.global_hello_time);
			return (u_char *)&bits_value;

		case MstpFowardDelay:
			bits_value = mstp_info.global_foward_delay;
			*var_len = sizeof(mstp_info.global_foward_delay);
			return (u_char *)&bits_value;

		case  MstpMaxAge:
			bits_value = mstp_info.global_max_age;
			*var_len = sizeof(mstp_info.global_max_age);
			return (u_char *)&bits_value;

		case MstpMaxHopCount:
			bits_value = mstp_info.global_max_hop_count;
			*var_len = sizeof(mstp_info.global_max_hop_count);
			return (u_char *)&bits_value;

		default :
			return (NULL);
	}

	
}


int mstp_if_port_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
		struct mstp_port_info_snmp  *index)
{
	struct mstp_port_info_snmp *mstp_info = NULL;
	struct ipc_mesg_n *pmesg = NULL;
	int data_num = 0;
	int ret = 0;

	pmesg = snmp_get_mstp_info_bulk(index->ifindex, MODULE_ID_SNMPD, IPC_TYPE_SNMP_MSTP_INFO, MSTP_PORT_INFO_TYPE);
	mstp_info = pmesg->msg_data;
	data_num = pmesg->msghdr.data_num;

	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);
	if ((0 == data_num) || (NULL == mstp_info))
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return FALSE;
	}
	else
	{
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, mstp_info , sizeof(struct mstp_port_info_snmp));
			mstp_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);

		return TRUE;
	}
}



struct mstp_port_info_snmp *mstp_if_port_info_node_lookup(struct ipran_snmp_data_cache *cache ,
		int exact,
		struct  mstp_port_info_snmp  *index_input)
{
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
	struct listnode  *node, *nnode;
	struct  mstp_port_info_snmp *data1_find;

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
hhrMstpPortInfoTable_get(struct variable *vp,
		oid *name,
		size_t *length,
		int exact, size_t *var_len, WriteMethod **write_method)
{


	int ret = 0;
	struct mstp_port_info_snmp *mstp_info = NULL;
	struct mstp_port_info_snmp index;
	int data_num = 0;

	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);


	/* validate the index */
	ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

	if (ret < 0)
	{
		return NULL;
	}


	if (NULL == mstp_port_info_cache)
	{
		mstp_port_info_cache = snmp_cache_init(sizeof(struct mstp_port_info_snmp) ,
				mstp_if_port_info_get_data_from_ipc ,
				mstp_if_port_info_node_lookup);

		if (NULL == mstp_port_info_cache)
		{	
			zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
			return (NULL);
		}
	}

	mstp_info = snmp_cache_get_data_by_index(mstp_port_info_cache , exact, &index);

	if (NULL == mstp_info)
	{	
		return NULL;
	}

	/* get ready the next index */
	if (!exact)
	{
		ipran_snmp_int_index_set(vp, name, length, mstp_info->ifindex);
	}

	switch (vp->magic)
	{
		case MstpPortProtocalState:

			bits_value = mstp_info->port_protocol_state;
			*var_len = sizeof(bits_value );
			return (u_char *)&bits_value;

		case MstpPortState :
			bits_value = mstp_info->port_state;
			*var_len = sizeof(bits_value );
			return (u_char *)&bits_value;

		case MstpPortMode:
			bits_value = mstp_info->port_mode;
			*var_len = sizeof(bits_value );
			return (u_char *)&bits_value;

		case MstpPortDesignatedBridgeId:
			memcpy(bridge_value,mstp_info->port_designated_bridge_id,BRIDGE_ID);
			*var_len = BRIDGE_ID;
			return (u_char *)bridge_value;

		case MstpPortRole:
			bits_value = mstp_info->port_role;
			*var_len = sizeof(bits_value );
			return (u_char *)&bits_value;

		case MstpPortPriority:
			short_value = mstp_info->port_priority;
			*var_len = sizeof(short_value );
			return (u_char *)&short_value;

		case MstpPortPathCost:
			int_value = mstp_info->port_path_cost;
			*var_len = sizeof(int_value );
			return (u_char *)&int_value;

		case MstpPortMessage:
			int_value = mstp_info->port_message;
			*var_len = sizeof(int_value );
			return (u_char *)&int_value;

		case MstpPortEdgeState:
			bits_value = mstp_info->port_edge_state;
			*var_len = sizeof(bits_value );
			return (u_char *)&bits_value;

		case MstpPortFilterState:
			bits_value = mstp_info->port_filter_state;
			*var_len = sizeof(bits_value );
			return (u_char *)&bits_value;

		case MstpPortP2P:
			bits_value = mstp_info->port_p2p;
			*var_len = sizeof(bits_value );
			return (u_char *)&bits_value;

		default :
			return (NULL);
	}

}



int mstp_if_instance_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
		struct mstp_instance_snmp  *index)
{
	struct mstp_instance_snmp *mstp_info = NULL;
	struct ipc_mesg_n *pmesg = NULL;
	int data_num = 0;
	int ret = 0;

	pmesg = snmp_get_mstp_info_bulk(index->instance_id, MODULE_ID_SNMPD, IPC_TYPE_SNMP_MSTP_INFO, MSTP_INSTANCE_TYPE);
	if (NULL == pmesg)
	{
		return FALSE;
	}

	mstp_info = pmesg->msg_data;
	data_num = pmesg->msghdr.data_num;
	
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if ((0 == data_num) || (NULL == mstp_info))
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		
		return FALSE;
	}
	else
	{
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, mstp_info , sizeof(struct mstp_instance_snmp));
			mstp_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		
		return TRUE;
	}
}



struct mstp_instance_snmp *mstp_if_instance_node_lookup(struct ipran_snmp_data_cache *cache ,
		int exact,
		struct  mstp_instance_snmp  *index_input)
{
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
	struct listnode  *node, *nnode;
	struct  mstp_instance_snmp *data1_find;

	for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
	{
		if (NULL == index_input || 0 == index_input->instance_id)
		{
			return cache->data_list->head->data ;
		}

		if (data1_find->instance_id== index_input->instance_id)
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
hhrMstpInstanceTable_get(struct variable *vp,
		oid *name,
		size_t *length,
		int exact, size_t *var_len, WriteMethod **write_method)
{


	int ret = 0;
	struct mstp_instance_snmp *mstp_info = NULL;
	struct mstp_instance_snmp index;
	int data_num = 0;

	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);


	/* validate the index */
	ret = ipran_snmp_int_index_get(vp, name, length, &index.instance_id, exact);

	if (ret < 0)
	{
		return NULL;
	}


	if (NULL == mstp_instance_cache)
	{
		mstp_instance_cache = snmp_cache_init(sizeof(struct mstp_instance_snmp) ,
				mstp_if_instance_get_data_from_ipc ,
				mstp_if_instance_node_lookup);

		if (NULL == mstp_instance_cache)
		{	
			zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
			return (NULL);
		}
	}

	mstp_info = snmp_cache_get_data_by_index(mstp_instance_cache , exact, &index);

	if (NULL == mstp_info)
	{	
		return NULL;
	}

	/* get ready the next index */
	if (!exact)
	{
		ipran_snmp_int_index_set(vp, name, length, mstp_info->instance_id);
	}
	int i = 0;
	switch (vp->magic)
	{
		case MstpInstanceId:
			bits_value = mstp_info->instance_id;
			*var_len = sizeof(bits_value );
			return (u_char *)&bits_value;

		case MstpInstanceVlanMap:
			memcpy(vlan_value,mstp_info->instance_vlan_map,strlen(mstp_info->instance_vlan_map));
			*var_len = strlen(mstp_info->instance_vlan_map);
			return (u_char *)vlan_value;

		case MstpInstanceBridgePriority :
			int_value = mstp_info->instance_bridge_priority;
			*var_len = sizeof(int_value );
			return (u_char *)&int_value;

		case MstpInstanceBridgeId:
			memcpy(bridge_value,mstp_info->instance_bridge_id,BRIDGE_ID);
			*var_len = BRIDGE_ID;
			return (u_char *)bridge_value;

		case MstpInstanceRegRootBridgeId:
			memcpy(bridge_value,mstp_info->instance_region_root_bridge_id,BRIDGE_ID);
			*var_len = BRIDGE_ID;
			return (u_char *)bridge_value;

		case MstpInstanceInternalRootPathCost:
			int_value = mstp_info->instance_internal_root_path_cost;
			*var_len = sizeof(int_value );
			return (u_char *)&int_value;

		default :
			return (NULL);
	}

}

int mstp_if_instance_port_get_data_from_ipc(struct ipran_snmp_data_cache *cache , struct mstp_instance_port_snmp *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct mstp_instance_port_snmp *mstp_info = NULL;
	int data_num = 0;
	int ret = 0;

	pmesg = snmp_get_mstp_instance_port_info_bulk(&index->instance_port_ifindex, sizeof(uint32_t), \
			index->instance_id, MODULE_ID_SNMPD, IPC_TYPE_SNMP_MSTP_INFO,MSTP_INSTANCE_PORT_TYPE);
	if (NULL == pmesg)
	{
		return FALSE;
	}

	mstp_info = pmesg->msg_data;
	data_num = pmesg->msghdr.data_num;
	
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

	if ((0 == data_num) || (NULL == pmesg))
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
	
		return FALSE;
	}
	else
	{
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, mstp_info , sizeof(struct mstp_instance_port_snmp));
			mstp_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);

		return TRUE;
	}
}



struct mstp_instance_port_snmp *mstp_if_instance_port_node_lookup(struct ipran_snmp_data_cache *cache ,
		int exact,
		const struct mstp_instance_port_snmp  *index_input)
{
	struct listnode  *node, *nnode;

	struct mstp_instance_port_snmp   *data1_find ;
	struct mstp_instance_port_snmp       index1 ;


	for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
	{
		if (NULL == index_input)
		{
			return cache->data_list->head->data ;
		}

		if (0 == index_input->instance_id && 0 == index_input->instance_port_ifindex)
		{

			return cache->data_list->head->data ;
		}

		if ((data1_find->instance_id == index_input->instance_id) && (data1_find->instance_port_ifindex == index_input->instance_port_ifindex))

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

	return NULL;
}


uchar *hhrMstpInstancePortTable_get(struct variable *vp,
		oid *name,
		size_t *length,
		int exact, size_t *var_len, WriteMethod **write_method)
{

	struct mstp_instance_port_snmp *mstp_info = NULL;
	struct mstp_instance_port_snmp index;
	int ret = 0;
	int flag = 0, val = 0;
	int p_index = 0;
	int instance_port_index = 0;
	
	/* validate the index */
	ret = ipran_snmp_intx2_index_get(vp, name, length, &p_index, &instance_port_index , exact);

	index.instance_id = p_index;
	index.instance_port_ifindex = instance_port_index;
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]: '%s:p_index = %d --log_nidex =  %d\n", __FILE__, __LINE__, __func__, p_index, instance_port_index);

	if (ret < 0)
	{
		return NULL;
	}

	if (NULL == mstp_instance_port_cache )
	{
		mstp_instance_port_cache  = snmp_cache_init(sizeof(struct mstp_instance_port_snmp),
				mstp_if_instance_port_get_data_from_ipc,
				mstp_if_instance_port_node_lookup);

		if (NULL ==  mstp_instance_port_cache )
		{
			return (NULL);
		}
	}

	mstp_info = snmp_cache_get_data_by_index(mstp_instance_port_cache , exact, &index);

	if (NULL == mstp_info)
	{
		return NULL;
	}

	if (!exact)
	{
		ipran_snmp_intx2_index_set(vp, name, length, mstp_info->instance_id, mstp_info->instance_port_ifindex);
	}

	switch (vp->magic)
	{
		case MstpInstancePortIfindex:
			int_value = mstp_info->instance_port_ifindex;
			*var_len = sizeof(int_value);
			return (u_char *)(&int_value);

		case MstpInstancePortState:
			bits_value = mstp_info->instance_port_state;
			*var_len = sizeof(bits_value);
			return (u_char *)(&bits_value);

		case MstpInstancePortRole:
			bits_value = mstp_info->instance_port_role;
			*var_len = sizeof(bits_value);
			return (u_char *)(&bits_value);

		case  MstpInstancePortPriority:
			short_value = mstp_info->instance_port_priority;
			*var_len = sizeof(short_value);
			return (u_char *)(&short_value);

		case MstpInstancePortPathCost:
			int_value = mstp_info->instance_port_path_cost;
			*var_len = sizeof(int_value);
			return (u_char *)(&int_value);

		case  MstpInstancePortMessage:
			short_value = mstp_info->instance_port_message;
			*var_len = sizeof(short_value);
			return (u_char *)(&short_value);

		case MstpInstancePortDesignalBridgeId:
			memcpy(bridge_value,mstp_info->instance_port_designal_bridge_id,BRIDGE_ID);
			*var_len = BRIDGE_ID;
			return (uchar *)bridge_value;

		case MstpInstancePortMaxHop:
			bits_value = mstp_info->instance_port_max_hop;
			*var_len = sizeof(bits_value);
			return (u_char *)(&bits_value);

		case MstpInstancePortEdgeState:
			bits_value = mstp_info->instance_port_edge_state;
			*var_len = sizeof(bits_value);
			return (u_char *)(&bits_value);

		case MstpInstancePortFilterState:
			bits_value = mstp_info->instance_port_filter_state;
			*var_len = sizeof(bits_value);
			return (u_char *)(&bits_value);

		case MstpInstancePortP2p:
			bits_value = mstp_info->instance_port_p2p;
			*var_len = sizeof(bits_value);
			return (u_char *)(&bits_value);

		default:
			return NULL;
	}


}




int mstp_if_port_statistics_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
		struct mstp_port_statistics_snmp  *index)
{
	struct mstp_port_statistics_snmp *mstp_info = NULL;
	struct ipc_mesg_n *pmesg = NULL;
	int data_num = 0;
	int ret = 0;

	pmesg = snmp_get_mstp_info_bulk(index->ifindex, MODULE_ID_SNMPD, IPC_TYPE_SNMP_MSTP_INFO, MSTP_PORT_STATISTICS_TYPE);
	if (NULL == pmesg)
	{
		return FALSE;
	}

	mstp_info = pmesg->msg_data;
	data_num = pmesg->msghdr.data_num;
	
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if ((0 == data_num) || (NULL == mstp_info))
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return FALSE;
	}
	else
	{
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, mstp_info , sizeof(struct mstp_port_statistics_snmp));
			mstp_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		
		return TRUE;
	}
}

struct mstp_port_statistics_snmp *mstp_if_port_statistics_node_lookup(struct ipran_snmp_data_cache *cache ,
		int exact,
		struct mstp_port_statistics_snmp *index_input)
{
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
	struct listnode  *node, *nnode;
	struct  mstp_port_statistics_snmp *data1_find;

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
hhrMstpStatisticsTable_get(struct variable *vp,
		oid *name,
		size_t *length,
		int exact, size_t *var_len, WriteMethod **write_method)
{


	int ret = 0;
	struct mstp_port_statistics_snmp *mstp_info = NULL;
	struct mstp_port_statistics_snmp index;
	int data_num = 0;

	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);


	/* validate the index */
	ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

	if (ret < 0)
	{
		return NULL;
	}


	if (NULL == mstp_port_statistics_cache)
	{
		mstp_port_statistics_cache = snmp_cache_init(sizeof(struct mstp_port_statistics_snmp),
				mstp_if_port_statistics_get_data_from_ipc ,
				mstp_if_port_statistics_node_lookup);

		if (NULL == mstp_port_statistics_cache)
		{	
			zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
			return (NULL);
		}
	}

	mstp_info = snmp_cache_get_data_by_index(mstp_port_statistics_cache , exact, &index);

	if (NULL == mstp_info)
	{	
		return NULL;
	}

	/* get ready the next index */
	if (!exact)
	{
		ipran_snmp_int_index_set(vp, name, length, mstp_info->ifindex);
	}

	switch (vp->magic)
	{
		case MstpPortTcSend:
			int_value = mstp_info->port_tc_send;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case MstpPortTcRecv:
			int_value = mstp_info->port_tc_recv;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case MstpPortTcnSend:
			int_value = mstp_info->port_tcn_send;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case MstpPortTcnRecv:
			int_value = mstp_info->port_tcn_recv;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case MstpPortBpduSend:
			int_value = mstp_info->port_bpdu_send;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case MstpPortBpduRecv:
			int_value = mstp_info->port_bpdu_recv;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case MstpPortConfigSend:
			int_value = mstp_info->port_config_send;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case MstpPortConfigRecv:
			int_value = mstp_info->port_config_recv;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case MstpPortRstpSend:
			int_value = mstp_info->port_rstp_send;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case MstpPortRstpRecv:
			int_value = mstp_info->port_rstp_recv;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case MstpPortMstpSend:
			int_value = mstp_info->port_mstp_send;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		case MstpPortMstpRecv:
			int_value = mstp_info->port_mstp_recv;
			*var_len = sizeof(int_value);
			return (u_char *)&int_value;

		default :
			return (NULL);
	}

}


struct ipc_mesg_n  *snmp_get_mstp_info_bulk(uint32_t ifindex, int module_id , int msg_subtype, int data_num)
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(NULL, 0, data_num, MODULE_ID_L2, module_id,
			IPC_TYPE_SNMP, msg_subtype, IPC_OPCODE_GET, ifindex);

	if (pmesg)
	{
		*pdata_num = pmesg->msghdr.data_num;
		return (void *) pmesg->msg_data;
	}
#endif

	struct ipc_mesg_n *pmesg = NULL;

	pmesg= ipc_sync_send_n2(NULL, 0, data_num , MODULE_ID_L2, module_id,
                       IPC_TYPE_SNMP, msg_subtype, IPC_OPCODE_GET, 0,2000);

	return pmesg;
}

struct ipc_mesg_n *snmp_get_mstp_instance_port_info_bulk(void * pdata,int data_len,uint32_t ifindex, int module_id , int msg_subtype, int data_num)
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(pdata, data_len, data_num, MODULE_ID_L2, module_id,
			IPC_TYPE_SNMP, msg_subtype, IPC_OPCODE_GET, ifindex);

	if (pmesg)
	{
		*pdata_num = pmesg->msghdr.data_num;
		return (void *) pmesg->msg_data;
	}

	return NULL;
#endif

	struct ipc_mesg_n *pmesg = NULL;

	pmesg= ipc_sync_send_n2(pdata, data_len, data_num , MODULE_ID_L2, module_id,
                          IPC_TYPE_SNMP, msg_subtype, IPC_OPCODE_GET, 0,2000);

	return pmesg;
}


