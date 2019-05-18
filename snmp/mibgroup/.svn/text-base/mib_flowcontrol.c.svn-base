#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <net-snmp-config.h>
#include <types.h>
#include <net-snmp-includes.h>
#include <net-snmp-agent-includes.h>
#include <snmp_index_operater.h>

#include <lib/types.h>
#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include <l2/l2_if.h>
#include <l2/l2_snmp.h>
#include <lib/linklist.h>
#include <ifm/ifm_message.h>
#include "ipran_snmp_data_cache.h"
#include "mib_flowcontrol.h"
#include "snmp_config_table.h"

struct flow_control_info_mib
{
	uint32_t ifindex;
	struct ifm_flow_control fc;
};

static struct ipran_snmp_data_cache *flowcontrol_cache = NULL ;
static int   int_value = 0;
static struct counter64 long_value;
#define SYS_STRING_LEN  256
static uchar str_value[STRING_LEN] = {'\0'};

static oid hhrFlowControlTable_oid[] = {HHR_FLOWCONTROL, 17};
FindVarMethod hhrFlowControlTable_get;
struct variable2 hhrFlowControlTable_variables[] =
{
    {hhrFlowControlIfDesrc,    ASN_OCTET_STR,  RONLY, hhrFlowControlTable_get, 2, {1, 1}},
    {hhrFlowControlSend,       ASN_INTEGER,    RONLY, hhrFlowControlTable_get, 2, {1, 2}},
    {hhrFlowControlRecive,     ASN_INTEGER,    RONLY, hhrFlowControlTable_get, 2, {1, 3}},
    {hhrFlowControlRxPause,    ASN_COUNTER64,  RONLY, hhrFlowControlTable_get, 2, {1, 4}},
    {hhrFlowControlTxPause,    ASN_COUNTER64,  RONLY, hhrFlowControlTable_get, 2, {1, 5}}
};

static oid hhrFlowControlGlobalTable_oid[] = {HHR_FLOWCONTROL};
FindVarMethod hhrFlowControlGlobalTable_get;
struct variable1 hhrFlowControlGlobalTable_variables[] =
{
    {hhrFlowControlSupport,    ASN_INTEGER,    RONLY, hhrFlowControlGlobalTable_get, 1, {16}},
    {hhrIfExtSupport,          ASN_INTEGER,    RONLY, hhrFlowControlGlobalTable_get, 1, {100}}
};


void  init_mib_flowcontrol(void)
{
    REGISTER_MIB("hhrFlowControlTable", hhrFlowControlTable_variables, variable2,
                 hhrFlowControlTable_oid);
    REGISTER_MIB("hhrFlowControlGlobalTable", hhrFlowControlGlobalTable_variables, variable1,
                 hhrFlowControlGlobalTable_oid);
}

struct flow_control_info_mib *snmp_get_FlowControl_info_bulk(uint32_t ifindex, int module_id , int *pdata_num)
{
	static struct flow_control_info_mib fc_info;
#if 0
    struct ipc_mesg *pmesg = ipc_send_hal_wait_reply1(NULL, 0, 1, MODULE_ID_HAL, module_id,
                               IPC_TYPE_IFM, IFNET_INFO_FLOW_CONTROL, IPC_OPCODE_GET_BULK, ifindex);
#endif

	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1 , MODULE_ID_HAL, module_id,
                                  IPC_TYPE_IFM, IFNET_INFO_FLOW_CONTROL, IPC_OPCODE_GET_BULK, 0,2000);
	
    memset(&fc_info, 0, sizeof(struct flow_control_info_mib));
	if (NULL != pmesg)
	{
        if(pmesg->msg_data)
        {
            memcpy(&fc_info, pmesg->msg_data, sizeof(struct flow_control_info_mib));
    		*pdata_num = pmesg->msghdr.data_num;
        }
        else
        {
            *pdata_num = 0;
        }
		mem_share_free(pmesg, MODULE_ID_SNMPD);

		return &fc_info;
	}

    return NULL;
}

int flowControl_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                  					struct flow_control_info_mib *index)
{
    struct flow_control_info_mib *pifm_info = NULL;
    int data_num = 0;
    int ret = 0;
    pifm_info = snmp_get_FlowControl_info_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num);
    //zlog_debug(SNMP_MTYPE_MIB_MODULE,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    if (0 == data_num || NULL == pifm_info)
    {
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pifm_info , sizeof(struct flow_control_info_mib));
            pifm_info++;
        }

        return TRUE;
    }
}

struct flow_control_info_mib *flowControl_if_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                 int exact,
                                                 const struct flow_control_info_mib *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;
    struct flow_control_info_mib *data1_find;

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
hhrFlowControlTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{
    struct flow_control_info_mib index ;
    u_int32_t index_next = 0;
    int ret = 0;
    int temp = 0 ;
    struct flow_control_info_mib *pifm_info = NULL;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
      zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);
        return NULL;
    }

    if (NULL == flowcontrol_cache)
    {
        flowcontrol_cache = snmp_cache_init(sizeof(struct flow_control_info_mib),
                                           flowControl_info_get_data_from_ipc,
                                           flowControl_if_info_node_lookup);

        if (NULL == flowcontrol_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]: \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    pifm_info = snmp_cache_get_data_by_index(flowcontrol_cache , exact, &index);
	
    if (NULL == pifm_info)
    {
    	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]: \n", __FILE__, __LINE__, __func__);
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pifm_info->ifindex);
    }

    switch (vp->magic)
    {
        case hhrFlowControlIfDesrc :
            memset(str_value , 0, SYS_STRING_LEN);
            ifm_get_name_by_ifindex(pifm_info->ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrFlowControlSend :
		if(pifm_info->fc.direction & IFM_FLOW_CONTROL_SEND)
		{
			int_value = 1;//enable
		}
		else
		{
			int_value = 2;
		}
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrFlowControlRecive :
		if(pifm_info->fc.direction & IFM_FLOW_CONTROL_RECEIVE)
		{
			int_value = 1;//enable
		}
		else
		{
			int_value = 2;
		}
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrFlowControlRxPause :
			long_value.high = ( pifm_info->fc.rx_pause >> 32);
            long_value.low  = ( pifm_info->fc.rx_pause);
            *var_len = sizeof(struct counter64);
            return (u_char *)&long_value;

        case hhrFlowControlTxPause :
            long_value.high = ( pifm_info->fc.tx_pause >> 32);
            long_value.low  = ( pifm_info->fc.tx_pause);
            *var_len = sizeof(struct counter64);
            return (u_char *)&long_value;

        default :
            return (NULL);
    }
}

u_char *
hhrFlowControlGlobalTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{
   
   if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
   {
	   return NULL;
   }
   

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    switch (vp->magic)
    {
        case hhrFlowControlSupport :
            int_value = 1;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrIfExtSupport :
            int_value = 1;
            *var_len = sizeof(int);
            return (u_char *)&int_value;
    }
}

