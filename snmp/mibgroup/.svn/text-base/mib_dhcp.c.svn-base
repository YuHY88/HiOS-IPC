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
#include <lib/types.h>

#include "net-snmp-config.h"
#include "types.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"

#include "../../dhcp/pool.h"
#include "../../dhcp/pool_address.h"
#include "../../dhcp/dhcp_if.h"

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
#include <lib/log.h>


#include "mib_dhcp.h"

//#include "lib/snmp_common.h"
#include "snmp_config_table.h"


enum table_lock
{
    UNLOCK,
    LOCK,
};


/*local temp variable*/
static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value = 0;
static uchar mac_value[6] = {0};
static uint32_t           uint_value = 0;

/* table list define */
struct list *hhrDHCPIPPoolTable_list = NULL;
struct list *hhrDHCPIPBindTable_list = NULL;
struct list *hhrDHCPIfConfigTable_list = NULL;

/* variable to store refresh data time of the last time */
static time_t hhrDHCPIPPoolTable_time_old = 0;
static time_t hhrDHCPIPBindTable_time_old = 0;
static time_t hhrDHCPIfConfigTable_time_old = 0;

static enum table_lock hhrDHCPIPPoolTable_lock = UNLOCK;
static enum table_lock hhrDHCPIPBindTable_lock = UNLOCK;
static enum table_lock hhrDHCPIfConfigTable_lock = UNLOCK;

/*snmp get information*/
#define DHCP_SNMP_IPPOOL_GET         0
#define DHCP_SNMP_IPBIND_GET         1
#define DHCP_SNMP_IFCONFIG_GET       2
#define DHCP_SNMP_DYNAMIC_IPBIND_GET 3


/*
 * Object ID definitions
 */
/*hhrDHCPIPPoolTable*/
static oid hhrDHCPIPPoolTable_oid[] = {HHRDHCPOID, 1};
FindVarMethod hhrDHCPIPPoolTable_get;
struct variable2 hhrDHCPIPPoolTable_variables[] =
{
    {hhrIPPollName,  ASN_OCTET_STR, RONLY, hhrDHCPIPPoolTable_get, 2, {1,  2}},
    {hhrGatewayIP,   ASN_IPADDRESS, RONLY, hhrDHCPIPPoolTable_get, 2, {1,  3}},
    {hhrGatewayMask, ASN_IPADDRESS, RONLY, hhrDHCPIPPoolTable_get, 2, {1,  4}},
    {hhrStartIP,     ASN_IPADDRESS, RONLY, hhrDHCPIPPoolTable_get, 2, {1,  5}},
    {hhrIPPoolIPNum, ASN_INTEGER,   RONLY, hhrDHCPIPPoolTable_get, 2, {1,  6}},
    {hhrDNSIP,       ASN_IPADDRESS, RONLY, hhrDHCPIPPoolTable_get, 2, {1,  7}},
    {hhrIPPoolLease, ASN_INTEGER,   RONLY, hhrDHCPIPPoolTable_get, 2, {1,  8}}
};

/*hhrDHCPIPBindTable*/
static oid hhrDHCPIPBindTable_oid[] = {HHRDHCPOID, 2};
FindVarMethod hhrDHCPIPBindTable_get;
struct variable2 hhrDHCPIPBindTable_variables[] =
{
    {hhrIPPoolIP, ASN_IPADDRESS, RONLY, hhrDHCPIPBindTable_get, 2, {1,  2}},
};

/*hhrDHCPIfConfigTable*/
static oid hhrDHCPIfConfigTable_oid[] = {HHRDHCPOID, 3};
FindVarMethod hhrDHCPIfConfigTable_get;
struct variable2 hhrDHCPIfConfigTable_variables[] =
{
    {hhrDHCPIfDesc,       ASN_OCTET_STR, RONLY, hhrDHCPIfConfigTable_get, 2, {1,  1}},
    {hhrDHCPServerEnable, ASN_INTEGER,   RONLY, hhrDHCPIfConfigTable_get, 2, {1,  2}},
    {hhrDHCPClientEnable, ASN_INTEGER,   RONLY, hhrDHCPIfConfigTable_get, 2, {1,  3}},
    {hhrDHCPRelayEnable,  ASN_INTEGER,   RONLY, hhrDHCPIfConfigTable_get, 2, {1,  4}},
    {hhrDHCPRelayNextHot, ASN_IPADDRESS, RONLY, hhrDHCPIfConfigTable_get, 2, {1,  5}},
	{hhrDHCPClientSaveEnable,	ASN_INTEGER,   RONLY, hhrDHCPIfConfigTable_get, 2, {1,	6}},
	{hhrDHCPClientZeroEnable,   ASN_INTEGER,   RONLY, hhrDHCPIfConfigTable_get, 2, {1,  7}},
};

/*hhrDHCPDynamicIPBindTable*/
static oid hhrDHCPDynamicIPBindTable_oid[] = {HHRDHCPOID, 4};
FindVarMethod hhrDHCPDynamicIPBindTable_get;
struct variable2 hhrDHCPDynamicIPBindTable_variables[] =
{
    {hhrDHCPDynamicMac, ASN_OCTET_STR, RONLY, hhrDHCPDynamicIPBindTable_get, 2, {1,  2}}
};

void init_mib_dhcp(void)
{

    REGISTER_MIB("hhrDHCPIPPoolTable", hhrDHCPIPPoolTable_variables,
                 variable2, hhrDHCPIPPoolTable_oid);

    REGISTER_MIB("hhrDHCPIPBindTable", hhrDHCPIPBindTable_variables,
                 variable2, hhrDHCPIPBindTable_oid);

    REGISTER_MIB("hhrDHCPIfConfigTable", hhrDHCPIfConfigTable_variables,
                 variable2, hhrDHCPIfConfigTable_oid);

    REGISTER_MIB("hhrDHCPDynamicIPBindTable", hhrDHCPDynamicIPBindTable_variables,
                 variable2, hhrDHCPDynamicIPBindTable_oid);
}

struct ip_pool *dhcp_com_get_ippool_instance(uchar pool_id, int module_id, int *pdata_num)
{
    /*struct ipc_mesg *pmesg = NULL ;

    if (!pdata_num)
    {
        return NULL;
    }

    pmesg = ipc_send_common_wait_reply1(&pool_id, 1, 1, MODULE_ID_DHCP,
                                        module_id, IPC_TYPE_L3IF, DHCP_SNMP_IPPOOL_GET , IPC_OPCODE_GET_BULK , 0);


    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct ip_pool *)pmesg->msg_data;
    }

    return NULL;*/

	struct ipc_mesg_n* pMsgRcv = ipc_sync_send_n2((void *)&pool_id, sizeof(pool_id), 1, MODULE_ID_DHCP, module_id, 
		IPC_TYPE_L3IF, DHCP_SNMP_IPPOOL_GET , IPC_OPCODE_GET_BULK , 0, 1000);

	if(NULL == pMsgRcv)
	{
		return NULL;
	}

	*pdata_num = pMsgRcv->msghdr.data_num;
	if(0 == *pdata_num)
	{
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
		return NULL;
	}
	else
	{
		return (struct ip_pool *)pMsgRcv->msg_data;
	}
}

struct dhcpOfferedAddr *dhcp_com_get_ipbind_instance(struct dhcpOfferedAddr *ipbind_key, int module_id, int *pdata_num)
{
#if 0
    struct ipc_mesg *pmesg = NULL ;

    if (!pdata_num)
    {
        return NULL;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s':  pool_index [%d] mac [%d]\n", __FILE__, __LINE__, __func__  , ipbind_key->pool_index , ipbind_key->chaddr[0]);

    pmesg = ipc_send_common_wait_reply1(ipbind_key, sizeof(struct dhcpOfferedAddr), 1, MODULE_ID_DHCP,
                                        module_id, IPC_TYPE_L3IF, DHCP_SNMP_IPBIND_GET , IPC_OPCODE_GET_BULK , 0);


    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct dhcpOfferedAddr *)pmesg->msg_data;
    }

    return NULL;
#endif

	struct ipc_mesg_n* pMsgRcv = ipc_sync_send_n2((void *)ipbind_key, sizeof(struct dhcpOfferedAddr), 1, MODULE_ID_DHCP, module_id, 
		IPC_TYPE_L3IF, DHCP_SNMP_IPBIND_GET , IPC_OPCODE_GET_BULK , 0, 1000);

	if(NULL == pMsgRcv)
	{
		return NULL;
	}

	*pdata_num = pMsgRcv->msghdr.data_num;
	if(0 == *pdata_num)
	{
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
		return NULL;
	}
	else
	{
		return (struct ip_pool *)pMsgRcv->msg_data;
	}
}

struct if_dhcp *dhcp_com_get_ifconfig_instance(uint32_t ifindex, int module_id, int *pdata_num)
{
#if 0
    struct ipc_mesg *pmesg = NULL ;

    if (!pdata_num)
    {
        return NULL;
    }

    pmesg = ipc_send_common_wait_reply1(&ifindex, sizeof(uint32_t), 1, MODULE_ID_DHCP,
                                        module_id, IPC_TYPE_L3IF, DHCP_SNMP_IFCONFIG_GET, IPC_OPCODE_GET_BULK , 0);


    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct if_dhcp *)pmesg->msg_data;
    }

    return NULL;
#endif

	struct ipc_mesg_n* pMsgRcv = ipc_sync_send_n2((void *)&ifindex, sizeof(uint32_t), 1, MODULE_ID_DHCP, module_id, 
		IPC_TYPE_L3IF, DHCP_SNMP_IFCONFIG_GET, IPC_OPCODE_GET_BULK , 0, 1000);

	if(NULL == pMsgRcv)
	{
		return NULL;
	}

	*pdata_num = pMsgRcv->msghdr.data_num;
	if(0 == *pdata_num)
	{
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
		return NULL;
	}
	else
	{
		return (struct ip_pool *)pMsgRcv->msg_data;
	}
}

struct dhcpOfferedAddr *dhcp_dynamic_com_get_ipbind_instance(struct dhcpOfferedAddr *ipbind_key, int module_id, int *pdata_num)
{
#if 0
    struct ipc_mesg *pmesg = NULL ;

    if (!pdata_num)
    {
        return NULL;
    }

    pmesg = ipc_send_common_wait_reply1(ipbind_key, sizeof(struct dhcpOfferedAddr), 1, MODULE_ID_DHCP,
                                        module_id, IPC_TYPE_L3IF, DHCP_SNMP_DYNAMIC_IPBIND_GET , IPC_OPCODE_GET_BULK , 0);


    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct dhcpOfferedAddr *)pmesg->msg_data;
    }

    return NULL;
#endif

	struct ipc_mesg_n* pMsgRcv = ipc_sync_send_n2((void *)ipbind_key, sizeof(struct dhcpOfferedAddr), 1, MODULE_ID_DHCP, module_id, 
		IPC_TYPE_L3IF, DHCP_SNMP_DYNAMIC_IPBIND_GET , IPC_OPCODE_GET_BULK , 0, 1000);

	if(NULL == pMsgRcv)
	{
		return NULL;
	}

	*pdata_num = pMsgRcv->msghdr.data_num;
	if(0 == *pdata_num)
	{
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
		return NULL;
	}
	else
	{
		return (struct ip_pool *)pMsgRcv->msg_data;
	}
}

void hhrDHCPIPPoolTable_node_free(struct ip_pool *node)
{
    XFREE(0, node);
}

void hhrDHCPIPBindTable_node_free(struct dhcpOfferedAddr *node)
{
    XFREE(0, node);
}

void hhrDHCPIfConfigTable_node_free(struct if_dhcp *node)
{
    XFREE(0, node);
}

static void *cfm_node_xcalloc(int table_num)
{
    switch (table_num)
    {
        case hhrDHCPIPPoolTable:
            return XCALLOC(0, sizeof(struct ip_pool));

        case hhrDHCPIPBindTable:
            return XCALLOC(0, sizeof(struct dhcpOfferedAddr));

        case hhrDHCPIfConfigTable:
            return XCALLOC(0, sizeof(struct if_dhcp));

        default:
            break;
    }

    return NULL;
}

static void dhcp_node_del_all(int table_num)
{
    switch (table_num)
    {
        case hhrDHCPIPPoolTable:
            list_delete_all_node(hhrDHCPIPPoolTable_list);
            break;

        case hhrDHCPIPBindTable:
            list_delete_all_node(hhrDHCPIPBindTable_list);
            break;

        case hhrDHCPIfConfigTable:
            list_delete_all_node(hhrDHCPIfConfigTable_list);
            break;

        default:
            break;
    }
}

void dhcp_list_init(void)
{
    hhrDHCPIPPoolTable_list = list_new();
    hhrDHCPIPPoolTable_list->del = (void (*)(void *))hhrDHCPIPPoolTable_node_free;

    hhrDHCPIPBindTable_list = list_new();
    hhrDHCPIPBindTable_list->del = (void (*)(void *))hhrDHCPIPBindTable_node_free;

    hhrDHCPIfConfigTable_list = list_new();
    hhrDHCPIfConfigTable_list->del = (void (*)(void *))hhrDHCPIfConfigTable_node_free;
}

static void *dhcp_node_xcalloc(int table_num)
{
    switch (table_num)
    {
        case hhrDHCPIPPoolTable:
            return XCALLOC(0, sizeof(struct ip_pool));

        case hhrDHCPIPBindTable:
            return XCALLOC(0, sizeof(struct dhcpOfferedAddr));

        case hhrDHCPIfConfigTable:
            return XCALLOC(0, sizeof(struct if_dhcp));

        case hhrDHCPDynamicIPBindTable:
            return XCALLOC(0, sizeof(struct dhcpOfferedAddr));

        default:
            break;
    }

    return NULL;
}

static void dhcp_node_add(int table_num, void *node)
{
    void *node_add = NULL;

    switch (table_num)
    {
        case hhrDHCPIPPoolTable:
            node_add = dhcp_node_xcalloc(hhrDHCPIPPoolTable);
            memcpy(node_add, node, sizeof(struct ip_pool));
            listnode_add(hhrDHCPIPPoolTable_list, node_add);
            break;

        case hhrDHCPIPBindTable:
            node_add = dhcp_node_xcalloc(hhrDHCPIPBindTable);
            memcpy(node_add, node, sizeof(struct dhcpOfferedAddr));
            listnode_add(hhrDHCPIPBindTable_list, node_add);
            break;

        case hhrDHCPIfConfigTable:
            node_add = dhcp_node_xcalloc(hhrDHCPIfConfigTable);
            memcpy(node_add, node, sizeof(struct if_dhcp));
            listnode_add(hhrDHCPIfConfigTable_list, node_add);
            break;

        default:
            break;
    }
}

static void *dhcp_node_lookup(int exact, int table_num, void *index_input)
{
    struct listnode  *node, *nnode;

    struct ip_pool  *data1_find;
    u_int32_t       index1;

    struct dhcpOfferedAddr  *data2_find;
    struct dhcpOfferedAddr  index2;

    struct if_dhcp  *data3_find;
    u_int32_t       index3;

    int i = 0 ;

    switch (table_num)
    {
        case hhrDHCPIPPoolTable:
            index1 = *((u_int32_t *)index_input);

            for (ALL_LIST_ELEMENTS(hhrDHCPIPPoolTable_list, node, nnode, data1_find))
            {
                if (0 == index1)
                {
                    return hhrDHCPIPPoolTable_list->head->data;
                }

                if (data1_find->pool_index == index1)
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

            break;

        case hhrDHCPIPBindTable:
            index2 = *((struct dhcpOfferedAddr *)index_input);

            for (ALL_LIST_ELEMENTS(hhrDHCPIPBindTable_list, node, nnode, data2_find))
            {
                if ((0 == index2.pool_index) && (0 == index2.chaddr[0]  &&
                                                 0 == index2.chaddr[1] &&
                                                 0 == index2.chaddr[2] &&
                                                 0 == index2.chaddr[3] &&
                                                 0 == index2.chaddr[4] &&
                                                 0 == index2.chaddr[5]))
                {
                    return hhrDHCPIPBindTable_list->head->data;
                }

                if ((data2_find->pool_index == index2.pool_index) &&
                        ((data2_find->chaddr[0] == index2.chaddr[0]) &&
                         (data2_find->chaddr[1] == index2.chaddr[1]) &&
                         (data2_find->chaddr[2] == index2.chaddr[2]) &&
                         (data2_find->chaddr[3] == index2.chaddr[3]) &&
                         (data2_find->chaddr[4] == index2.chaddr[4]) &&
                         (data2_find->chaddr[5] == index2.chaddr[5])))
                {
                    if (1 == exact) //get
                    {
                        return data2_find;
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

            break;

        case hhrDHCPIfConfigTable:
            index3 = *((u_int32_t *)index_input);

            for (ALL_LIST_ELEMENTS(hhrDHCPIfConfigTable_list, node, nnode, data3_find))
            {
                if (0 ==  index3)
                {

                    return hhrDHCPIfConfigTable_list->head->data;
                }

                if (data3_find->ifindex  == index3)
                {
                    if (1 == exact) //get
                    {
                        return data3_find;
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

            break;

        case hhrDHCPDynamicIPBindTable:
            index2 = *((struct dhcpOfferedAddr *)index_input);

            for (ALL_LIST_ELEMENTS(hhrDHCPIPBindTable_list, node, nnode, data2_find))
            {
                if ((0 == index2.pool_index) && (0 == index2.yiaddr))
                {
                    return hhrDHCPIPBindTable_list->head->data;
                }

                if ((data2_find->pool_index == index2.pool_index) &&
                        (data2_find->yiaddr == index2.yiaddr))
                {
                    if (1 == exact) //get
                    {
                        return data2_find;
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

            break;
    }

    return (NULL);
}


int hhrDHCPIPPoolTable_get_data_from_ipc(u_int32_t index)
{
    struct ip_pool *pip_pool = NULL;
	struct ip_pool *pip_pool_temp = NULL;
    int data_num = 0;
    int ret = 0;

    pip_pool_temp = dhcp_com_get_ippool_instance(index, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d] data_num [%d]\n", __FILE__, __LINE__, __func__ , index , data_num);

    if (0 == data_num)
    {
        return FALSE;
    }
    else
    {
		pip_pool = pip_pool_temp;
        for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
        {
            dhcp_node_add(hhrDHCPIPPoolTable, pip_pool);
            pip_pool++;
        }

		mem_share_free_bydata((void *)pip_pool_temp, MODULE_ID_SNMPD);

        time(&hhrDHCPIPPoolTable_time_old);
        return TRUE;
    }
}

struct ip_pool *hhrDHCPIPPoolTable_get_data_by_index(int exact, u_int32_t *index, u_int32_t *index_next)
{
    struct ip_pool *pip_pool = NULL;
    struct ip_pool *entry_temp;
    u_int32_t index_temp;
    static time_t getNext_time_old; //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > hhrDHCPIPPoolTable_GET_NEXT_INTERVAL))
    {
        hhrDHCPIPPoolTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrDHCPIPPoolTable_time_old) > hhrDHCPIPPoolTable_REFRESH_TIME) && \
            (UNLOCK == hhrDHCPIPPoolTable_lock))
    {
        if (NULL == hhrDHCPIPPoolTable_list)
        {
            hhrDHCPIPPoolTable_list = list_new();
            hhrDHCPIPPoolTable_list->del = (void (*)(void *))hhrDHCPIPPoolTable_node_free;
        }

        if (0 != hhrDHCPIPPoolTable_list->count)
        {
            dhcp_node_del_all(hhrDHCPIPPoolTable);
        }
    }

    /* then lock */
    hhrDHCPIPPoolTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrDHCPIPPoolTable_list)
    {
        hhrDHCPIPPoolTable_list = list_new();
        hhrDHCPIPPoolTable_list->del = (void (*)(void *))hhrDHCPIPPoolTable_node_free;
    }

    if (0 == hhrDHCPIPPoolTable_list->count)
    {
        index_temp = 0;
        ret = hhrDHCPIPPoolTable_get_data_from_ipc(index_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    pip_pool = (struct ip_pool *)dhcp_node_lookup(exact, hhrDHCPIPPoolTable, index);

    if (NULL == pip_pool)   //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrDHCPIPPoolTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct ip_pool *)hhrDHCPIPPoolTable_list->tail->data;
            ret = hhrDHCPIPPoolTable_get_data_from_ipc(entry_temp->pool_index);

            if (FALSE == ret)
            {
                hhrDHCPIPPoolTable_lock = UNLOCK;   //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            pip_pool = (struct ip_pool *)dhcp_node_lookup(exact, hhrDHCPIPPoolTable, index);

            if (NULL != pip_pool)
            {
                *index_next = pip_pool->pool_index;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrDHCPIPPoolTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pip_pool;
            }
        }
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    *index_next = pip_pool->pool_index;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrDHCPIPPoolTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pip_pool;
}


u_char *hhrDHCPIPPoolTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ip_pool *phhrDHCPIPPoolTable = NULL;
    int index = 0 ;
    int index_next = 0 ;

    int ret = 0;
    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index);

    if (ret < 0)
    {
        return NULL;
    }


    /* get data by index and renew index_next */
    phhrDHCPIPPoolTable = hhrDHCPIPPoolTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == phhrDHCPIPPoolTable)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index);
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, index_next);
    }

    switch (vp->magic)
    {
        case hhrIPPollName:
            memset(str_value, 0, STRING_LEN);
            sprintf(str_value, "%d", phhrDHCPIPPoolTable->pool_index);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrGatewayIP:
            *var_len = sizeof(uint32_t);
            uint_value = phhrDHCPIPPoolTable->dhcp_pool.gateway ;
            return (u_char *)(&uint_value);

        case hhrGatewayMask:
            *var_len = sizeof(uint32_t);
            uint_value = phhrDHCPIPPoolTable->dhcp_pool.mask ;
            return (u_char *)(&uint_value);

        case hhrStartIP:
            *var_len = sizeof(uint32_t);
            uint_value = phhrDHCPIPPoolTable->dhcp_pool.start ;
            return (u_char *)(&uint_value);

        case hhrIPPoolIPNum:
            *var_len = sizeof(uint32_t);
            uint_value = phhrDHCPIPPoolTable->dhcp_pool.total ;
            return (u_char *)(&uint_value);

        case hhrDNSIP:
            *var_len = sizeof(uint32_t);
            uint_value = phhrDHCPIPPoolTable->dhcp_pool.dns_server ;
            return (u_char *)(&uint_value);

        case hhrIPPoolLease:
            *var_len = sizeof(uint32_t);
            uint_value = phhrDHCPIPPoolTable->dhcp_pool.leasetime ;
            return (u_char *)(&uint_value);

        default :
            return (NULL);
    }
}



int hhrDHCPIPBindTable_get_data_from_ipc(struct dhcpOfferedAddr *index)
{
    struct dhcpOfferedAddr *pdhcpOfferedAddr = NULL;
	struct dhcpOfferedAddr *pdhcpOfferedAddr_temp = NULL;
    int data_num = 0;
    int ret = 0;
    pdhcpOfferedAddr_temp = dhcp_com_get_ipbind_instance(index, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    if (0 == data_num)
    {
        return FALSE;
    }
    else
    {
		pdhcpOfferedAddr = pdhcpOfferedAddr_temp;
        for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
        {
            dhcp_node_add(hhrDHCPIPBindTable, pdhcpOfferedAddr);
            pdhcpOfferedAddr++;
        }

		mem_share_free_bydata((void *)pdhcpOfferedAddr_temp, MODULE_ID_SNMPD);

        time(&hhrDHCPIPBindTable_time_old);
        return TRUE;
    }
}

struct dhcpOfferedAddr *hhrDHCPIPBindTable_get_data_by_index(int exact,
                                                             struct dhcpOfferedAddr *index , struct dhcpOfferedAddr *indexNext)
{
    struct dhcpOfferedAddr *pdhcpOfferedAddrl = NULL;
    struct dhcpOfferedAddr *entry_temp;
    struct dhcpOfferedAddr index_temp;
    static time_t getNext_time_old; //to store the get-next operate time of the last time
    int ret = 0;

    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > hhrDHCPIPBindTable_GET_NEXT_INTERVAL))
    {
        hhrDHCPIPBindTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrDHCPIPBindTable_time_old) > hhrDHCPIPBindTable_REFRESH_TIME) && \
            (UNLOCK == hhrDHCPIPBindTable_lock))
    {
        if (NULL == hhrDHCPIPBindTable_list)
        {
            hhrDHCPIPBindTable_list = list_new();
            hhrDHCPIPBindTable_list->del = (void (*)(void *))hhrDHCPIPBindTable_node_free;
        }

        if (0 != hhrDHCPIPBindTable_list->count)
        {
            dhcp_node_del_all(hhrDHCPIPBindTable);
        }
    }

    /* then lock */
    hhrDHCPIPBindTable_lock = LOCK ;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrDHCPIPBindTable_list)
    {
        hhrDHCPIPBindTable_list = list_new();
        hhrDHCPIPBindTable_list->del = (void (*)(void *))hhrDHCPIPBindTable_node_free;
    }

    if (0 == hhrDHCPIPBindTable_list->count)
    {
        memset(&index_temp.chaddr, 0, ETHER_ADDR_LEN * sizeof(char));
        index_temp.pool_index = 0 ;
        ret = hhrDHCPIPBindTable_get_data_from_ipc(&index_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    memcpy(&index_temp.chaddr, index->chaddr, ETHER_ADDR_LEN * sizeof(char));
    index_temp.pool_index = index->pool_index ;

    pdhcpOfferedAddrl = (struct dhcpOfferedAddr *)dhcp_node_lookup(exact, hhrDHCPIPBindTable, &index_temp);

    if (NULL == pdhcpOfferedAddrl)  //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrDHCPIPBindTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct dhcpOfferedAddr *)hhrDHCPIPBindTable_list->tail->data;
            ret = hhrDHCPIPBindTable_get_data_from_ipc(entry_temp);

            if (FALSE == ret)
            {
                hhrDHCPIPBindTable_lock = UNLOCK;   //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            pdhcpOfferedAddrl = (struct dhcpOfferedAddr *)dhcp_node_lookup(exact, hhrDHCPIPBindTable, &index_temp);

            if (NULL != pdhcpOfferedAddrl)
            {
                indexNext->pool_index = pdhcpOfferedAddrl->pool_index;
                memcpy(&indexNext->chaddr , pdhcpOfferedAddrl->chaddr , sizeof(char)*ETHER_ADDR_LEN);

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrDHCPIPBindTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pdhcpOfferedAddrl;
            }
        }
    }

    indexNext->pool_index = pdhcpOfferedAddrl->pool_index;
    memcpy(&indexNext->chaddr , pdhcpOfferedAddrl->chaddr , sizeof(char)*ETHER_ADDR_LEN);

    if (1 == exact) //get operate && find node, unlock
    {
        hhrDHCPIPBindTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pdhcpOfferedAddrl;
}


u_char *hhrDHCPIPBindTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    struct dhcpOfferedAddr *pdhcpOfferedAddr;
    struct dhcpOfferedAddr  index;
    struct dhcpOfferedAddr  indexNext;
    int ret = 0;
    int poolIndex = 0;

    memset(&index, 0, sizeof(struct dhcpOfferedAddr));
    memset(&indexNext, 0, sizeof(struct dhcpOfferedAddr));

    /* validate the index */
    ret = ipran_snmp_int_mac_index_get(vp, name, length, &poolIndex, index.chaddr, exact);
    index.pool_index = poolIndex;

    if (ret < 0)
    {
        return NULL;
    }

    /* get data by index and renew index_next */
    pdhcpOfferedAddr = hhrDHCPIPBindTable_get_data_by_index(exact, &index, &indexNext);

    if (NULL == pdhcpOfferedAddr)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_mac_index_set(vp, name, length, indexNext.pool_index, indexNext.chaddr);
    }

    switch (vp->magic)
    {
        case hhrIPPoolIP:
            *var_len = sizeof(uint32_t);
            uint_value = pdhcpOfferedAddr->yiaddr ;
            return (u_char *)(&uint_value);

        default :
            return (NULL);
    }

}

int hhrDHCPIfConfigTable_get_data_from_ipc(u_int32_t index)
{
    struct if_dhcp *pif_dhcp = NULL;
	struct if_dhcp *pif_dhcp_temp = NULL;
    int data_num = 0;
    int ret = 0;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index);
    pif_dhcp_temp = dhcp_com_get_ifconfig_instance(index, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d] data_num [%d]\n", __FILE__, __LINE__, __func__ , index , data_num);

    if (0 == data_num)
    {
        return FALSE;
    }
    else
    {
		pif_dhcp = pif_dhcp_temp;
        for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
        {
            dhcp_node_add(hhrDHCPIfConfigTable, pif_dhcp);
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d] \n", __FILE__, __LINE__, __func__ , pif_dhcp->ifindex);
            pif_dhcp++;
        }

		mem_share_free_bydata((void *)pif_dhcp_temp, MODULE_ID_SNMPD);

        time(&hhrDHCPIfConfigTable_time_old);   //refresh time_old after refresh cache data
        return TRUE;
    }
}

struct if_dhcp *hhrDHCPIfConfigTable_get_data_by_index(int exact, u_int32_t *index, u_int32_t *index_next)
{
    struct if_dhcp *pif_dhcp = NULL;
    struct if_dhcp *entry_temp;
    u_int32_t index_temp;
    static time_t getNext_time_old; //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > hhrDHCPIfConfigTable_GET_NEXT_INTERVAL))
    {
        hhrDHCPIfConfigTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrDHCPIfConfigTable_time_old) > hhrDHCPIfConfigTable_REFRESH_TIME) && \
            (UNLOCK == hhrDHCPIfConfigTable_lock))
    {
        if (NULL == hhrDHCPIfConfigTable_list)
        {
            hhrDHCPIfConfigTable_list = list_new();
            hhrDHCPIfConfigTable_list->del = (void (*)(void *))hhrDHCPIfConfigTable_node_free;
        }

        if (0 != hhrDHCPIfConfigTable_list->count)
        {
            dhcp_node_del_all(hhrDHCPIfConfigTable);
        }
    }

    /* then lock */
    hhrDHCPIfConfigTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrDHCPIfConfigTable_list)
    {
        hhrDHCPIfConfigTable_list = list_new();
        hhrDHCPIfConfigTable_list->del = (void (*)(void *))hhrDHCPIfConfigTable_node_free;
    }

    if (0 == hhrDHCPIfConfigTable_list->count)
    {
        index_temp = 0;
        ret = hhrDHCPIfConfigTable_get_data_from_ipc(index_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    pif_dhcp = (struct if_dhcp *)dhcp_node_lookup(exact, hhrDHCPIfConfigTable, index);

    if (NULL == pif_dhcp)   //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrDHCPIfConfigTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct if_dhcp *)hhrDHCPIfConfigTable_list->tail->data;
            ret = hhrDHCPIfConfigTable_get_data_from_ipc(entry_temp->ifindex);

            if (FALSE == ret)
            {
                hhrDHCPIfConfigTable_lock = UNLOCK; //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            pif_dhcp = (struct if_dhcp *)dhcp_node_lookup(exact, hhrDHCPIfConfigTable, index);

            if (NULL != pif_dhcp)
            {
                *index_next = pif_dhcp->ifindex;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrDHCPIfConfigTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pif_dhcp;
            }
        }
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    *index_next = pif_dhcp->ifindex;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrDHCPIfConfigTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pif_dhcp;
}


u_char *hhrDHCPIfConfigTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct if_dhcp *pif_dhcp = NULL;
    int index = 0 ;
    int index_next = 0 ;

    int ret = 0;
    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index);

    if (ret < 0)
    {
        return NULL;
    }


    /* get data by index and renew index_next */
    pif_dhcp = hhrDHCPIfConfigTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == pif_dhcp)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index);
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, index_next);
    }

    switch (vp->magic)
    {
        case hhrDHCPIfDesc:
            memset(str_value, 0, STRING_LEN) ;
            ifm_get_name_by_ifindex(pif_dhcp->ifindex, str_value) ;
            *var_len = strlen(str_value) ;
            return (str_value) ;

        case hhrDHCPServerEnable:
            *var_len = sizeof(int);

            if (E_DHCP_SERVER == pif_dhcp->dhcp_role)
            {
                int_value = 1 ;
            }
            else
            {
                int_value = 2 ;
            }

            return (u_char *)(&int_value) ;

        case hhrDHCPClientEnable:
            *var_len = sizeof(int);

            if (E_DHCP_CLIENT == pif_dhcp->dhcp_role)
            {
                int_value = 1 ;
            }
            else
            {
                int_value = 2 ;
            }

            return (u_char *)(&int_value) ;

        case hhrDHCPRelayEnable:
            *var_len = sizeof(int);

            if (E_DHCP_RELAY == pif_dhcp->dhcp_role)
            {
                int_value = 1 ;
            }
            else
            {
                int_value = 2 ;
            }

            return (u_char *)(&int_value) ;

        case hhrDHCPRelayNextHot:
            *var_len = sizeof(uint32_t) ;

            if (E_DHCP_RELAY == pif_dhcp->dhcp_role)
                //if (NULL !=pif_dhcp.r_ctrl && 0!=pif_dhcp->r_ctrl )
            {
                uint_value = pif_dhcp->nexthop ;
            }
            else
            {
                uint_value = 0;
            }

            return (u_char *)(&uint_value) ;
			
		case hhrDHCPClientSaveEnable:
			*var_len = sizeof(int);
					
			if (1 == pif_dhcp->save)
			{
				int_value = 1 ;
			}
			else
			{
				int_value = 2 ;
			}
			
			
			return (u_char *)(&int_value) ;
		
		case hhrDHCPClientZeroEnable:
			*var_len = sizeof(int);
		
			if (E_DHCP_ZERO == pif_dhcp->dhcp_role)
			{
				int_value = 1 ;
			}
			else
			{
				int_value = 2 ;
			}
		
			return (u_char *)(&int_value) ;
		
        default :
            return (NULL) ;
    }
}

int hhrDHCPDynamicIPBindTable_get_data_from_ipc(struct dhcpOfferedAddr *index)
{
    struct dhcpOfferedAddr *pdhcpOfferedAddr = NULL;
	struct dhcpOfferedAddr *pdhcpOfferedAddr_temp = NULL;
    int data_num = 0;
    int ret = 0;
    pdhcpOfferedAddr_temp = dhcp_dynamic_com_get_ipbind_instance(index, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    if (0 == data_num)
    {
        return FALSE;
    }
    else
    {
		pdhcpOfferedAddr = pdhcpOfferedAddr_temp;
        for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': yiaddr [%8x] index.pool_index [%d]\n", __FILE__, __LINE__, __func__ , pdhcpOfferedAddr->yiaddr, pdhcpOfferedAddr->pool_index);
            dhcp_node_add(hhrDHCPIPBindTable, pdhcpOfferedAddr);
            pdhcpOfferedAddr++;
        }

		mem_share_free_bydata((void *)pdhcpOfferedAddr_temp, MODULE_ID_SNMPD);

        time(&hhrDHCPIPBindTable_time_old);
        return TRUE;
    }
}


struct dhcpOfferedAddr *hhrDHCPDynamicIPBindTable_get_data_by_index(int exact,
                                                                    struct dhcpOfferedAddr *index , struct dhcpOfferedAddr *indexNext)
{
    struct dhcpOfferedAddr *pdhcpOfferedAddrl = NULL;
    struct dhcpOfferedAddr *entry_temp;
    struct dhcpOfferedAddr index_temp;
    static time_t getNext_time_old; //to store the get-next operate time of the last time
    int ret = 0;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__);
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > hhrDHCPIPBindTable_GET_NEXT_INTERVAL))
    {
        hhrDHCPIPBindTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrDHCPIPBindTable_time_old) > hhrDHCPIPBindTable_REFRESH_TIME) && \
            (UNLOCK == hhrDHCPIPBindTable_lock))
    {
        if (NULL == hhrDHCPIPBindTable_list)
        {
            hhrDHCPIPBindTable_list = list_new();
            hhrDHCPIPBindTable_list->del = (void (*)(void *))hhrDHCPIPBindTable_node_free;
        }

        if (0 != hhrDHCPIPBindTable_list->count)
        {
            dhcp_node_del_all(hhrDHCPIPBindTable);
        }
    }

    /* then lock */
    hhrDHCPIPBindTable_lock = LOCK ;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrDHCPIPBindTable_list)
    {
        hhrDHCPIPBindTable_list = list_new();
        hhrDHCPIPBindTable_list->del = (void (*)(void *))hhrDHCPIPBindTable_node_free;
    }

    if (0 == hhrDHCPIPBindTable_list->count)
    {
        //memset(&index_temp.chaddr, 0, ETHER_ADDR_LEN*sizeof(char));
        index_temp.pool_index = 0 ;
        index_temp.yiaddr = 0;
        ret = hhrDHCPDynamicIPBindTable_get_data_from_ipc(&index_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    //memcpy(&index_temp.chaddr, index->chaddr, ETHER_ADDR_LEN*sizeof(char));
    index_temp.yiaddr = index->yiaddr;
    index_temp.pool_index = index->pool_index ;

    pdhcpOfferedAddrl = (struct dhcpOfferedAddr *)dhcp_node_lookup(exact, hhrDHCPDynamicIPBindTable, &index_temp);

    if (NULL == pdhcpOfferedAddrl)  //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrDHCPIPBindTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct dhcpOfferedAddr *)hhrDHCPIPBindTable_list->tail->data;
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__);
            ret = hhrDHCPDynamicIPBindTable_get_data_from_ipc(entry_temp);

            if (FALSE == ret)
            {
                hhrDHCPIPBindTable_lock = UNLOCK;   //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__);
            pdhcpOfferedAddrl = (struct dhcpOfferedAddr *)dhcp_node_lookup(exact, hhrDHCPDynamicIPBindTable, &index_temp);

            if (NULL != pdhcpOfferedAddrl)
            {
                indexNext->pool_index = pdhcpOfferedAddrl->pool_index;
                //memcpy(&indexNext->chaddr , pdhcpOfferedAddrl->chaddr , sizeof(char)*ETHER_ADDR_LEN);
                indexNext->yiaddr = pdhcpOfferedAddrl->yiaddr;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrDHCPIPBindTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pdhcpOfferedAddrl;
            }
        }
    }

    indexNext->pool_index = pdhcpOfferedAddrl->pool_index;
    //memcpy(&indexNext->chaddr , pdhcpOfferedAddrl->chaddr , sizeof(char)*ETHER_ADDR_LEN);
    indexNext->yiaddr = pdhcpOfferedAddrl->yiaddr;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , indexNext->pool_index);

    if (1 == exact) //get operate && find node, unlock
    {
        hhrDHCPIPBindTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pdhcpOfferedAddrl;
}


u_char *hhrDHCPDynamicIPBindTable_get(struct variable *vp,
                                      oid *name,
                                      size_t *length,
                                      int exact, size_t *var_len, WriteMethod **write_method)
{
    struct dhcpOfferedAddr *pdhcpOfferedAddr;
    struct dhcpOfferedAddr  index;
    struct dhcpOfferedAddr  indexNext;
    u_int32_t               index_int;
    int                     ret = 0;

    index_int = 0;
    memset(&index, 0, sizeof(struct dhcpOfferedAddr));
    memset(&indexNext, 0, sizeof(struct dhcpOfferedAddr));

    /* validate the index */
    ret = ipran_snmp_int_ip_index_get(vp, name, length, &index_int, &index.yiaddr, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.pool_index = index_int;
    /* get data by index and renew index_next */
    pdhcpOfferedAddr = hhrDHCPDynamicIPBindTable_get_data_by_index(exact, &index, &indexNext);

    if (NULL == pdhcpOfferedAddr)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_ip_index_set(vp, name, length, indexNext.pool_index, indexNext.yiaddr);
    }

    switch (vp->magic)
    {
        case hhrDHCPDynamicMac:
            *var_len = 6;
            memcpy(&mac_value, pdhcpOfferedAddr->chaddr, 6);
            return (mac_value);

        default :
            return (NULL);
    }
}

