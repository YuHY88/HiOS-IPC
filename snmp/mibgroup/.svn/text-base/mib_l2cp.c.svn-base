
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

#include "../../l2/l2cp/l2cp.h"
#include "mib_l2cp.h"
#include "snmp_config_table.h"


static struct ipran_snmp_data_cache *hhrL2cpSessionList_cache = NULL ;

static oid hhrL2cpConfigList_oid[] = {HHRL2CPOID,1};
FindVarMethod hhrL2cpConfigList_get;
struct variable2 hhrL2cpConfigList_variables[] =
{
    {hhrL2cpProtocolMac,            ASN_OCTET_STR, RONLY, hhrL2cpConfigList_get, 2, {1, 3}},
    {hhrL2cpProtocolEtherType,      ASN_INTEGER,   RONLY, hhrL2cpConfigList_get, 2, {1, 4}},
    {hhrL2cpAction,                 ASN_INTEGER,   RONLY, hhrL2cpConfigList_get, 2, {1, 5}},
    {hhrL2cpMode,                   ASN_INTEGER,   RONLY, hhrL2cpConfigList_get, 2, {1, 6}},
    {hhrL2cpProtoGroupMac,          ASN_OCTET_STR, RONLY, hhrL2cpConfigList_get, 2, {1, 7}},
	{hhrL2cpProtoGroupVlan,         ASN_INTEGER,   RONLY, hhrL2cpConfigList_get, 2, {1, 8}},
    {hhrL2cpProtoGroupVlanCos,      ASN_INTEGER,   RONLY, hhrL2cpConfigList_get, 2, {1, 9}},
    {hhrL2cpMplsSessionId,          ASN_INTEGER,   RONLY, hhrL2cpConfigList_get, 2, {1, 10}},
    {hhrL2cpMplsStatus,             ASN_INTEGER,   RONLY, hhrL2cpConfigList_get, 2, {1, 11}},
 
};

void init_mib_l2cp(void)
{
	REGISTER_MIB("hhrL2cpConfigList", hhrL2cpConfigList_variables,variable2,
		         hhrL2cpConfigList_oid);
}

struct ipc_mesg_n *l2cp_com_get_session_instance(struct l2cp_snmp_info *l2cpTable, int module_id, int *pdata_num)
{
	struct ipc_mesg_n * pRevmsg = NULL;
	int data_len = sizeof(struct l2cp_snmp_info);	
        
    pRevmsg = ipc_sync_send_n2(l2cpTable, data_len, 1, MODULE_ID_HAL, module_id, 
            IPC_TYPE_L2CP, 0, IPC_OPCODE_GET_BULK, 0, 0);

	if (NULL != pRevmsg)
    {
        zlog_debug (SNMP_DBG_MIB_GET,"%s[%d]: in function '%s' \n", __FILE__, __LINE__, __func__);
        *pdata_num = pRevmsg->msghdr.data_num;
        return pRevmsg;
    }
	
    return NULL;
}


int hhrL2cpSessionList_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                               struct l2cp_snmp_info *l2cpTable)
{
	struct ipc_mesg_n * pRevmsg = NULL;
    struct l2cp_snmp_info *pstL2cpSession = NULL;
	int data_len = sizeof(struct l2cp_snmp_info);
	int data_num = 0;
    int ret = 0;
	
    pRevmsg = l2cp_com_get_session_instance(l2cpTable, MODULE_ID_SNMPD, &data_num);
	if(NULL == pRevmsg)
	{
		return FALSE;
	}
	
	pstL2cpSession = (struct erps_global *)pRevmsg->msg_data;
	if(NULL == pstL2cpSession || 0 == data_num)
	{
		mem_share_free(pRevmsg, MODULE_ID_SNMPD);
		return FALSE;
	}
	
    for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, pstL2cpSession , data_len);
        pstL2cpSession++;
    }
	mem_share_free(pRevmsg, MODULE_ID_SNMPD);
    return TRUE;

}

struct l2cp_snmp_info *hhrL2cpSessionList_node_lookup(struct ipran_snmp_data_cache *cache,
														int exact,
														const struct l2cp_snmp_info  *pstL2cpTable)
{
   struct listnode	*node, *nnode;
   struct l2cp_snmp_info  *pstL2cpNode = NULL;

   for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pstL2cpNode))
   {
	   if (NULL == pstL2cpTable || 0 == pstL2cpTable->ifindex)
	   {
		   return cache->data_list->head->data ;
	   }

	   if (pstL2cpNode->ifindex == pstL2cpTable->ifindex && \
	   	   pstL2cpNode->protoTypeIndex == pstL2cpTable->protoTypeIndex)
	   {
		   if (1 == exact) 
		   {
			   return pstL2cpNode;
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

							   
u_char *hhrL2cpConfigList_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    static uint32_t uint_value = 0;
	static uint8_t macAddress[DMAC_LEN];
    struct l2cp_snmp_info *pL2cpTable = NULL;
    struct l2cp_snmp_info l2cpTable;
	uint32_t firstIndex = 0;
	uint32_t secondIndex = 0;
    int ret = 0;
  
    ret = ipran_snmp_intx2_index_get(vp, name, length, &firstIndex, &secondIndex, exact);
    zlog_debug (SNMP_DBG_MIB_GET, "%s[%d]:'%s':first_index [%d] second_index [%d]\n", __FILE__, __LINE__, __func__ , firstIndex, secondIndex);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrL2cpSessionList_cache)
    {
        hhrL2cpSessionList_cache = snmp_cache_init(sizeof(struct l2cp_snmp_info) ,
                                                         hhrL2cpSessionList_get_data_from_ipc,
                                                         hhrL2cpSessionList_node_lookup);

        if (NULL == hhrL2cpSessionList_cache)
        {
            zlog_debug (SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

	memset(&l2cpTable, 0, sizeof(struct l2cp_snmp_info));
	l2cpTable.ifindex = firstIndex;
	l2cpTable.protoTypeIndex = secondIndex;
    pL2cpTable = snmp_cache_get_data_by_index(hhrL2cpSessionList_cache, exact, &l2cpTable);

    if (NULL == pL2cpTable)
    {
        return NULL;
    }

    if (0 == exact)
    {
		ipran_snmp_intx2_index_set(vp, name, length, pL2cpTable->ifindex,  pL2cpTable->protoTypeIndex);
    }
	
	uint_value = 0;
    switch (vp->magic)
    {
        case hhrL2cpProtocolMac:
			memset(macAddress, 0, sizeof(uint8_t) * DMAC_LEN);
            memcpy(macAddress, pL2cpTable->dmac , sizeof(uint8_t) * DMAC_LEN);
            *var_len = sizeof(uint8_t) * DMAC_LEN;
            return (u_char *)macAddress;

		case hhrL2cpProtocolEtherType:
            uint_value = pL2cpTable->etherType;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;
			
        case hhrL2cpAction:
            uint_value = pL2cpTable->l2cpAction;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;
			
		case hhrL2cpMode:
            uint_value = pL2cpTable->l2cpMode;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

		case hhrL2cpProtoGroupMac:
			memset(macAddress, 0, sizeof(uint8_t) * DMAC_LEN);
            memcpy(macAddress, pL2cpTable->gmac, sizeof(uint8_t) * DMAC_LEN);
            *var_len = sizeof(uint8_t) * DMAC_LEN;
            return (u_char *)macAddress;

		case hhrL2cpProtoGroupVlan:
            uint_value = pL2cpTable->gvlan;
			if(uint_value == 4095)
			{
				uint_value = 0;
			}
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;
			
        case hhrL2cpProtoGroupVlanCos:
            uint_value = pL2cpTable->gcos;
			if(pL2cpTable->gvlan == 4095)
			{
				uint_value = 0;
			}
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

		case hhrL2cpMplsSessionId:
            uint_value = pL2cpTable->vc_id;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrL2cpMplsStatus:
            uint_value = pL2cpTable->status;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;
			
        default:
            return NULL;
    }
}







