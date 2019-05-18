/*
 *  cli command MIB group implementation - port_isolate_snmp.c
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
#include <time.h>
#include <stdlib.h>

#include "net-snmp-config.h"
#include "types.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"
#include "ipran_snmp_data_cache.h"

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/ifm_common.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/types.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/inet_ip.h>
#include <l2/l2_if.h>

#include "snmp_config_table.h"
#include "l2/port_isolate.h"
#include "mib_isolate.h"

/*local temp variable*/
static int                int_value = 0;

/*
 * Object ID definitions
 */
/*EP-TREE*/
static oid hhrEPTree_variables_oid[] = {HHETREEOID, 23};
FindVarMethod hhrEPTree_get;
struct ipran_snmp_data_cache *hhrEPTreeTable_cache = NULL;
struct variable2 hhrEPTree_variables[] =
{
    {hhrPortIsolateEPtreeRole,  ASN_INTEGER,  RONLY, hhrEPTree_get, 2, {1, 3}},
    {hhrPortIsolateEPtreeState,  ASN_INTEGER, RONLY, hhrEPTree_get, 2, {1, 4}}
};

/*EVP-TREE*/
static oid hhrEvPTree_variables_oid[] = {HHETREEOID, 24};
FindVarMethod hhrEVPTree_get;
struct ipran_snmp_data_cache *hhrEVPTreeTable_cache = NULL;
struct variable2 hhrEVPTree_variables[] =
{
    {hhrPortIsolateEVPtreeRole,  ASN_INTEGER,  RONLY, hhrEVPTree_get, 2, {1, 3}},
    {hhrPortIsolateEVPtreeState,  ASN_INTEGER, RONLY, hhrEVPTree_get, 2, {1, 4}}
};

void init_mib_portisltfunc(void)
{
    REGISTER_MIB("hhrEPTreeTable", hhrEPTree_variables, variable2, hhrEPTree_variables_oid);
    REGISTER_MIB("hhrEVPTreeTable", hhrEVPTree_variables, variable2, hhrEvPTree_variables_oid);
}

/************************ get functions *****************************/
struct port_islt_snmp *port_islt_get_group_bulk(struct port_islt_snmp *index, int module_id, int *pdata_num )
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 (index, sizeof(struct port_islt_snmp), 1 , MODULE_ID_L2, 
									module_id, IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_GET_BULK, index->ifindex);
#endif

	static struct port_islt_snmp islt_info;
	struct ipc_mesg_n *pmesg = NULL;

	pmesg= ipc_sync_send_n2(index, sizeof(struct port_islt_snmp), 1 , MODULE_ID_L2, module_id,
                        IPC_TYPE_L2IF, L2IF_INFO_ISOLATE, IPC_OPCODE_GET_BULK, 0,2000);
	if (NULL != pmesg)
	{
		if (NULL == pmesg->msg_data)
		{
			mem_share_free(pmesg, MODULE_ID_SNMPD);
		}
		else
		{
			memset(&islt_info, 0, sizeof(struct port_islt_snmp));
			memcpy(&islt_info, pmesg->msg_data, sizeof(struct port_islt_snmp));
			*pdata_num = pmesg->msghdr.data_num;
			mem_share_free(pmesg, MODULE_ID_SNMPD);

			return (struct port_islt_snmp *)&islt_info;
		}
	}

    return NULL;
}

int hhrEPTreeTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct port_islt_snmp *index)
{
    int data_num = 0;
    int ret = 0;
    int i = 0;
    struct port_islt_snmp *pdata;
	
    pdata = port_islt_get_group_bulk(index, MODULE_ID_SNMPD, &data_num);

    printf("%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (0 == data_num || NULL == pdata)
    {
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(hhrEPTreeTable_cache, pdata, sizeof(struct port_islt_snmp));
            pdata ++;
        }

        return TRUE;
    }
}

struct port_islt_snmp *hhrEPTreeTable_lookup(struct ipran_snmp_data_cache *cache ,
                                                   int exact,
                                                   const struct port_islt_snmp *index_input)
{
    struct port_islt_snmp *data = NULL;
	struct listnode       *node = NULL;

	if (NULL == index_input)
    {
        return NULL;
    }

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, data))
    {
        if ((0 == index_input->ifindex) && (0 == index_input->id))
        {
            return cache->data_list->head->data;
        }

        if ((data->id == index_input->id) && (data->ifindex == index_input->ifindex))
        {
            if (1 == exact) //get
            {
                return data;
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


u_char *hhrEPTree_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
	struct port_islt_snmp *pdata = NULL;
	struct port_islt_snmp data_index;
	uint32_t grpid = 0;
    uint32_t ifindex = 0;
    int ret = 0;

    /* validate the index */
	ret = ipran_snmp_intx2_index_get(vp, name, length, &ifindex, &grpid, exact);
	if (0 > ret)
	{
		return NULL;
	}

	if (NULL == hhrEPTreeTable_cache)
    {
        hhrEPTreeTable_cache = snmp_cache_init(sizeof(struct port_islt_snmp),
                                                    hhrEPTreeTable_data_form_ipc,
                                                    hhrEPTreeTable_lookup);

        if (NULL == hhrEPTreeTable_cache)
        {
            return (NULL);
        }
    }

    data_index.ifindex = ifindex;
    data_index.id 	   = grpid;
	data_index.type    = EP_TREE;
    pdata = snmp_cache_get_data_by_index(hhrEPTreeTable_cache, exact, &data_index);
    if (NULL == pdata)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, data_index.ifindex, data_index.id);
    }

    switch (vp->magic)
    {
        case hhrPortIsolateEPtreeRole:
			int_value = pdata->role;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);
		case hhrPortIsolateEPtreeState:
			int_value = pdata->state;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);
    }

    return NULL;
}

int hhrEVPTreeTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct port_islt_snmp *index)
{
    int data_num = 0;
    int ret = 0;
    int i = 0;
    struct port_islt_snmp *pdata;
	
    pdata = port_islt_get_group_bulk(index, MODULE_ID_SNMPD, &data_num);

    printf("%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (0 == data_num || NULL == pdata)
    {
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(hhrEVPTreeTable_cache, pdata, sizeof(struct port_islt_snmp));
            pdata ++;
        }

        return TRUE;
    }
}

struct port_islt_snmp *hhrEVPTreeTable_lookup(struct ipran_snmp_data_cache *cache ,
                                                   int exact,
                                                   const struct port_islt_snmp *index_input)
{
    struct port_islt_snmp *data = NULL;
	struct listnode       *node = NULL;

	if (NULL == index_input)
    {
        return NULL;
    }

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, data))
    {
        if ((0 == index_input->ifindex) && (0 == index_input->id))
        {
            return cache->data_list->head->data;
        }

        if ((data->id == index_input->id) && (data->ifindex == index_input->ifindex))
        {
            if (1 == exact) //get
            {
                return data;
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


u_char *hhrEVPTree_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
	struct port_islt_snmp *pdata = NULL;
	struct port_islt_snmp data_index;
	uint32_t grpid = 0;
    uint32_t ifindex = 0;
    int ret = 0;

    /* validate the index */
	ret = ipran_snmp_intx2_index_get(vp, name, length, &ifindex, &grpid, exact);
	if (0 > ret)
	{
		return NULL;
	}

	if (NULL == hhrEVPTreeTable_cache)
    {
        hhrEVPTreeTable_cache = snmp_cache_init(sizeof(struct port_islt_snmp),
                                                    hhrEVPTreeTable_data_form_ipc,
                                                    hhrEVPTreeTable_lookup);

        if (NULL == hhrEVPTreeTable_cache)
        {
            return (NULL);
        }
    }

    data_index.ifindex = ifindex;
    data_index.id 	   = grpid;
	data_index.type    = EVP_TREE;
    pdata = snmp_cache_get_data_by_index(hhrEVPTreeTable_cache, exact, &data_index);
    if (NULL == pdata)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, data_index.ifindex, data_index.id);
    }

    switch (vp->magic)
    {
        case hhrPortIsolateEVPtreeRole:
			int_value = pdata->role;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);
		case hhrPortIsolateEVPtreeState:
			int_value = pdata->state;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);
    }

    return NULL;
}


