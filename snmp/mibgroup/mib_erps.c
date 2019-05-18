
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

#include "net-snmp-config.h"
#include "types.h"
#include "ipran_snmp_data_cache.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"
#include "snmp_index_operater.h"

#include "../../l2/raps/erps.h"
#include "mib_erps.h"
#include "snmp_config_table.h"


static struct ipran_snmp_data_cache *hhrErpsSessionList_cache = NULL ;

static oid hhrErpsGlobalList_oid[] = {HHRERPSOID};
FindVarMethod hhrErpsGlobalList_get;
struct variable1 hhrErpsGlobalList_variables[] =
{
    {hhrErpsPktRecv1,      ASN_INTEGER, RONLY, hhrErpsGlobalList_get, 1, {1}},
    {hhrErpsPktSend2,      ASN_INTEGER, RONLY, hhrErpsGlobalList_get, 1, {2}},
    {hhrErpsPktError3,     ASN_INTEGER, RONLY, hhrErpsGlobalList_get, 1, {3}},
    {hhrErpsCreatedNums4,  ASN_INTEGER, RONLY, hhrErpsGlobalList_get, 1, {4}},
    {hhrErpsEnabledNums5,  ASN_INTEGER, RONLY, hhrErpsGlobalList_get, 1, {5}},
};

static oid hhrErpsConfigTable_oid[] = {HHRERPSOID, 6};
FindVarMethod hhrErpsConfigTable_get;
struct variable2 hhrErpsConfigTable_variables[] =
{
    {hhrErpsRole,             ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 2}},
    {hhrErpsEastInterface,    ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 3}},
    {hhrErpsWestInterface,    ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 4}},
    {hhrErpsRplInterface,     ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 5}},
	{hhrErpsTrafficVlans,     ASN_OCTET_STR, RONLY, hhrErpsConfigTable_get, 2, {1, 6}},
    {hhrErpsProtocolVlan,     ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 7}},
    {hhrErpsRingId,           ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 8}},
    {hhrErpsMode,             ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 9}},
    {hhrErpsLevel,            ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 10}},
	{hhrErpsGuardTimer,       ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 11}},
    {hhrErpsHoldoffTimer,     ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 12}},
    {hhrErpsWTRTimer,         ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 13}},
    {hhrErpsWTBTimer,         ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 14}},
    {hhrErpsKeepaliveTime,    ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 15}},
	{hhrErpsAttachSessionId,  ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 16}},
    {hhrErpsEastCfmSessionId, ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 17}},
    {hhrErpsWestCfmSessionId, ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 18}},
    {hhrErpsSubRing,          ASN_INTEGER,   RONLY, hhrErpsConfigTable_get, 2, {1, 19}},
};

static oid hhrErpsStatusTable_oid[] = {HHRERPSOID, 7};
FindVarMethod hhrErpsStatusTable_get;
struct variable2 hhrErpsStatusTable_variables[] =
{
    {hhrErpsSessionStatus,       ASN_INTEGER,   RONLY, hhrErpsStatusTable_get, 2, {1, 1}},
    {hhrErpsCurProtectStatus,    ASN_INTEGER,   RONLY, hhrErpsStatusTable_get, 2, {1, 2}},
    {hhrErpsBlockInterface,      ASN_INTEGER,   RONLY, hhrErpsStatusTable_get, 2, {1, 3}},
    {hhrErpsWestInterfaceStatus, ASN_INTEGER,   RONLY, hhrErpsStatusTable_get, 2, {1, 4}},
    {hhrErpsEastInterfaceStatus, ASN_INTEGER,   RONLY, hhrErpsStatusTable_get, 2, {1, 5}},
};


void init_mib_erps(void)
{
	REGISTER_MIB("hhrErpsGlobalList", hhrErpsGlobalList_variables,variable1,
		         hhrErpsGlobalList_oid);
    REGISTER_MIB("hhrErpsConfigTable", hhrErpsConfigTable_variables, variable2,
                 hhrErpsConfigTable_oid);
	REGISTER_MIB("hhrErpsStatusTable", hhrErpsStatusTable_variables, variable2,
                 hhrErpsStatusTable_oid);
}

struct ipc_mesg_n *erps_com_get_session_instance(uint16_t session_id, int module_id, int *pdata_num)
{
	struct ipc_mesg_n *pmesg = NULL ;

	if (session_id == 0)
	{
		pmesg = ipc_sync_send_n2(&session_id, 0, 1, MODULE_ID_L2,
								module_id, IPC_TYPE_ERPS, 0 , IPC_OPCODE_GET_BULK, session_id, 0);
	}
	else
	{
		pmesg = ipc_sync_send_n2(&session_id, 4, 1, MODULE_ID_L2,
								module_id, IPC_TYPE_ERPS, 0 , IPC_OPCODE_GET_BULK, session_id, 0);
	}

	if (NULL != pmesg)
	{
		*pdata_num = pmesg->msghdr.data_num;
		return pmesg;
	}

	return NULL;
}


int hhrErpsSessionList_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                               struct erps_info *erpsTable)
{
	struct ipc_mesg_n *pmesg = NULL ;
    struct erps_info *pstErpsSession = NULL;
	int data_len = sizeof(struct erps_info);
	int data_num = 0;
    int ret = 0;

	if(NULL == cache || NULL == erpsTable)
	{
		return FALSE;
	}
	
    pmesg = erps_com_get_session_instance(erpsTable->sess_id, MODULE_ID_SNMPD, &data_num);
	if(NULL == pmesg)
	{
		return FALSE;
	}
	
	pstErpsSession = (struct erps_sess *)pmesg->msg_data;
    if (0 == data_num || NULL == pstErpsSession)
    {
    	mem_share_free(pmesg, MODULE_ID_SNMPD);
        return FALSE;
    }
   
    for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pstErpsSession , data_len);
        pstErpsSession++;
    }
	mem_share_free(pmesg, MODULE_ID_SNMPD);
    return TRUE;
    
}

struct erps_info *hhrErpsSessionList_node_lookup(struct ipran_snmp_data_cache *cache,
														int exact,
														const struct erps_info  *pstErpsTable)
{
   zlog_debug (SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
   struct listnode	*node, *nnode;
   struct erps_info  *pstErpsNode;

   for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pstErpsNode))
   {
	   if (NULL == pstErpsTable || 0 == pstErpsTable->sess_id)
	   {
		   return cache->data_list->head->data ;
	   }

	   if (pstErpsNode->sess_id == pstErpsTable->sess_id)
	   {
		   if (1 == exact) 
		   {
			   return pstErpsNode;
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

int erps_com_get_global_data(int module_id, int *pdata_num, struct erps_global *pstErpsGlobal )
{
    struct ipc_mesg_n *pmesg = NULL;

    zlog_debug (SNMP_DBG_MIB_GET,"%s[%d]: in function '%s' \n", __FILE__, __LINE__, __func__);

	if(NULL == pstErpsGlobal)
	{
		return -1;
	}
	
    pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_L2,
                             module_id, IPC_TYPE_ERPS, 0, IPC_OPCODE_GET, 0, 0);
	if(NULL == pmesg)
	{
        zlog_debug (SNMP_DBG_MIB_GET,"%s[%d]: in function '%s' \n", __FILE__, __LINE__, __func__);
		return -1;
	}
	
    *pdata_num = pmesg->msghdr.data_num;
	if(NULL == pmesg->msg_data || 0 == *pdata_num)
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return -1;
	}
	
	memcpy(pstErpsGlobal, pmesg->msg_data, sizeof(struct erps_global));
	mem_share_free(pmesg, MODULE_ID_SNMPD);
    return 0;

}

u_char *hhrErpsGlobalList_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
  
{
   static uint32_t uint_value = 0;
   int data_num = 0;
   int ret = 0;
   struct erps_global erpsGlobalInfor;

   if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
   {
	   return NULL;
   }

   memset(&erpsGlobalInfor, 0, sizeof(struct erps_global));
   ret = erps_com_get_global_data(MODULE_ID_SNMPD, &data_num, &erpsGlobalInfor);

   if (0 != ret)
   {
	   return NULL;
   }

   uint_value = 0;
   switch (vp->magic)
   {
	   case hhrErpsPktRecv1:
		   uint_value = erpsGlobalInfor.pkt_recv;
           *var_len = sizeof(uint32_t);
           return (u_char *)&uint_value;

	   case hhrErpsPktSend2:
		   uint_value = erpsGlobalInfor.pkt_send;
           *var_len = sizeof(uint32_t);
           return (u_char *)&uint_value;

	   case hhrErpsPktError3:
		   uint_value = erpsGlobalInfor.pkt_err;
           *var_len = sizeof(uint32_t);
           return (u_char *)&uint_value;

	   case hhrErpsCreatedNums4:
		   uint_value = erpsGlobalInfor.sess_total;
           *var_len = sizeof(uint32_t);
           return (u_char *)&uint_value;

	   case hhrErpsEnabledNums5:
		   uint_value = erpsGlobalInfor.sess_enable;
           *var_len = sizeof(uint32_t);
           return (u_char *)&uint_value;

	   default:
		   return (NULL);
   }
}
							   
u_char *hhrErpsConfigTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    static uint32_t uint_value = 0;
	static uint8_t data_vlan_map[VLAN_MEAP_MAX];
    struct erps_info *pErpsTable = NULL;
    struct erps_info erpsTable;
	int firstIndex = 0 ;
    int ret = 0;
 
    ret = ipran_snmp_int_index_get(vp, name, length, &firstIndex, exact);

    zlog_debug (SNMP_DBG_MIB_GET,"%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , firstIndex);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrErpsSessionList_cache)
    {
        hhrErpsSessionList_cache = snmp_cache_init(sizeof(struct erps_info) ,
                                                         hhrErpsSessionList_get_data_from_ipc,
                                                         hhrErpsSessionList_node_lookup);

        if (NULL == hhrErpsSessionList_cache)
        {
            zlog_debug (SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

	memset(&erpsTable, 0, sizeof(struct erps_info));
	erpsTable.sess_id = firstIndex;
    pErpsTable = snmp_cache_get_data_by_index(hhrErpsSessionList_cache , exact, &erpsTable);

    if (NULL == pErpsTable)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pErpsTable->sess_id);
    }

	uint_value = 0;
    switch (vp->magic)
    {
        case hhrErpsRole:
            uint_value = pErpsTable->role;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

		case hhrErpsEastInterface:
            uint_value = pErpsTable->east_interface;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;
			
        case hhrErpsWestInterface:
            uint_value = pErpsTable->west_interface;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;
			
		case hhrErpsRplInterface:
            uint_value = pErpsTable->rpl_interface;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

		case hhrErpsTrafficVlans:
            memset(data_vlan_map, 0, sizeof(uint8_t) * VLAN_MEAP_MAX);
            memcpy(data_vlan_map, pErpsTable->data_vlan_map, sizeof(uint8_t) * VLAN_MEAP_MAX);
            *var_len = sizeof(uint8_t) * VLAN_MEAP_MAX;
            return (data_vlan_map);
			
        case hhrErpsProtocolVlan:
            uint_value = pErpsTable->pvlan;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

		case hhrErpsRingId:
            uint_value = pErpsTable->ring_id;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrErpsMode:
            uint_value = pErpsTable->failback;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrErpsLevel:
            uint_value = pErpsTable->level;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrErpsGuardTimer:
            uint_value = pErpsTable->guardtimer;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrErpsHoldoffTimer:
            uint_value = pErpsTable->holdoff;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;
        
		case hhrErpsWTRTimer:
			uint_value = pErpsTable->wtr;
			*var_len = sizeof(uint32_t);
			return (u_char *)&uint_value;

		case hhrErpsWTBTimer:
			uint_value = pErpsTable->wtb;
			*var_len = sizeof(uint32_t);
			return (u_char *)&uint_value;
			
		case hhrErpsKeepaliveTime:
			uint_value = pErpsTable->keepalive;
			*var_len = sizeof(uint32_t);
			return (u_char *)&uint_value;

		case hhrErpsAttachSessionId:
			uint_value = pErpsTable->attach_sess_id;
			*var_len = sizeof(uint32_t);
			return (u_char *)&uint_value;	

		case hhrErpsEastCfmSessionId:
			uint_value = pErpsTable->east_cfm_session_id;
			*var_len = sizeof(uint32_t);
			return (u_char *)&uint_value;

		case hhrErpsWestCfmSessionId:
			uint_value = pErpsTable->west_cfm_session_id;
			*var_len = sizeof(uint32_t);
			return (u_char *)&uint_value;
			
		case hhrErpsSubRing:
			uint_value = pErpsTable->sub_ring;
			*var_len = sizeof(uint32_t);
			return (u_char *)&uint_value;	
			
        default:
            return NULL;
    }
}

u_char *hhrErpsStatusTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    static uint32_t uint_value = 0;
    struct erps_info *pErpsTable = NULL;
    struct erps_info erpsTable;
	int firstIndex = 0 ;
    int ret = 0;
 
    ret = ipran_snmp_int_index_get(vp, name, length, &firstIndex, exact);

    zlog_debug (SNMP_DBG_MIB_GET,"%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , firstIndex);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrErpsSessionList_cache)
    {
        hhrErpsSessionList_cache = snmp_cache_init(sizeof(struct erps_info) ,
                                                         hhrErpsSessionList_get_data_from_ipc,
                                                         hhrErpsSessionList_node_lookup);

        if (NULL == hhrErpsSessionList_cache)
        {
            zlog_debug (SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

	memset(&erpsTable, 0, sizeof(struct erps_info));
	erpsTable.sess_id = firstIndex;
    pErpsTable = snmp_cache_get_data_by_index(hhrErpsSessionList_cache , exact, &erpsTable);

    if (NULL == pErpsTable)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pErpsTable->sess_id);
    }

	uint_value = 0;
    switch (vp->magic)
    {
        case hhrErpsSessionStatus:
            uint_value = pErpsTable->status;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

		case hhrErpsCurProtectStatus:
            uint_value = pErpsTable->current_status;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;
			
        case hhrErpsBlockInterface:
            uint_value = pErpsTable->block_interface;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;
			
		case hhrErpsWestInterfaceStatus:
            uint_value = pErpsTable->west_interface_flag;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

		case hhrErpsEastInterfaceStatus:
            uint_value = pErpsTable->east_interface_flag;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        default:
            return NULL;
    }
}




