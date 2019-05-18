/*
 *  ntp snmp browser interface - ntp_snmp.c
 *
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "net-snmp-config.h"
#include <lib/types.h>
//#include "types.h"

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
#include <lib/linklist.h>
#include <lib/log.h>

#include "../../ospfd/ospf_msg.h"
#include "../../ospfd/ospfd.h"

#include "ipran_snmp_data_cache.h"
#include "mib_h3c_ospf.h"



static int int_value;
static int ospf_int_value;
static unsigned char puchr[4] = {0, 0, 0, 0};
static uchar str_value[STRING_LEN] = {'\0'};
static uint32_t           ip_value = 0;

/*---------------------------------------------ospfAreaTable_get--------------------------------------------*/
FindVarMethod ospfAreaTable_get ;
struct variable2 ospfAreaTable_variables[] =
{
    {ospfAreaId,                              ASN_IPADDRESS, NOACCESS,  ospfAreaTable_get, 2, {1, 1}},
    {ospfAuthType,                            ASN_INTEGER,   RONLY,     ospfAreaTable_get, 2, {1, 2}},
    {ospfImportAsExtern,                      ASN_INTEGER,   RONLY,     ospfAreaTable_get, 2, {1, 3}},
    {ospfSpfRuns,                             ASN_COUNTER,   RONLY,     ospfAreaTable_get, 2, {1, 4}},
    //{ospfAreaBdrRtrCount,                   ASN_GAUGE,     RONLY,     ospfAreaTable_get, 2, {1, 5}},
    //{ospfAsBdrRtrCount,                     ASN_GAUGE,     RONLY,     ospfAreaTable_get, 2, {1, 6}},
    {ospfAreaLsaCount,                        ASN_GAUGE,     RONLY,     ospfAreaTable_get, 2, {1, 7}},
    {ospfAreaSummary,                         ASN_INTEGER,   RONLY,     ospfAreaTable_get, 2, {1, 9}},
    {ospfAreaNssaTranslatorRole,              ASN_INTEGER,   RONLY,     ospfAreaTable_get, 2, {1, 11}},
    {ospfAreaNssaTranslatorState,             ASN_INTEGER,   RONLY,     ospfAreaTable_get, 2, {1, 12}},
    {ospfAreaNssaTranslatorStabilityInterval, ASN_INTEGER,   RONLY,     ospfAreaTable_get, 2, {1, 13}},
    {ospfAreaNssaTranslatorEvents,            ASN_COUNTER,   RONLY,     ospfAreaTable_get, 2, {1, 14}}
} ;

oid  ospfAreaTablee_oid[]   = { OSPF_MIB_ROOT_OID , 2};

static struct ipran_snmp_data_cache *h3c_ospf_area_cache = NULL ;

FindVarMethod ospfIfTable_get ;
struct variable2 ospfIfTable_variables[] =
{
    {ospfIfIpAddress,                    ASN_IPADDRESS, NOACCESS,   ospfIfTable_get, 2, {1, 1}},
    {ospfAddressLessIf,                  ASN_INTEGER,   NOACCESS,   ospfIfTable_get, 2, {1, 2}},
    {ospfIfAreaId_mib,                   ASN_IPADDRESS, RONLY,      ospfIfTable_get, 2, {1, 3}},
    {ospfIfType_mib,                     ASN_INTEGER,   RONLY,      ospfIfTable_get, 2, {1, 4}},
    {ospfIfAdminStat_mib,                ASN_INTEGER,   RONLY,      ospfIfTable_get, 2, {1, 5}},
    {ospfIfRtrPriority_mib,              ASN_INTEGER,   RONLY,      ospfIfTable_get, 2, {1, 6}},
    {ospfIfTransitDelay_mib,             ASN_INTEGER,   RONLY,      ospfIfTable_get, 2, {1, 7}},
    {ospfIfRetransInterval_mib,          ASN_INTEGER,   RONLY,      ospfIfTable_get, 2, {1, 8}},
    {ospfIfHelloInterval_mib,            ASN_INTEGER,   RONLY,      ospfIfTable_get, 2, {1, 9}},
    {ospfIfRtrDeadInterval_mib,          ASN_INTEGER,   RONLY,      ospfIfTable_get, 2, {1, 10}},
    //{ospfIfState_mib,                  ASN_INTEGER,   RONLY,      ospfIfTable_get, 2, {1, 12}},
    {ospfIfDesignatedRouter_mib,         ASN_IPADDRESS, RONLY,      ospfIfTable_get, 2, {1, 13}},
    {ospfIfBackupDesignatedRouter_mib,   ASN_IPADDRESS, RONLY,      ospfIfTable_get, 2, {1, 14}},
    {ospfIfEvents_mib,                   ASN_COUNTER,   RONLY,      ospfIfTable_get, 2, {1, 15}},
    {ospfIfAuthKey_mib,                  ASN_OCTET_STR, RONLY,      ospfIfTable_get, 2, {1, 16}},
    {ospfIfAuthType_mib,                 ASN_INTEGER,   RONLY,      ospfIfTable_get, 2, {1, 20}},
    {ospfIfDesignatedRouterId_mib,       ASN_IPADDRESS, RONLY,      ospfIfTable_get, 2, {1, 23}},
    {ospfIfBackupDesignatedRouterId_mib, ASN_IPADDRESS, RONLY,      ospfIfTable_get, 2, {1, 24}}
} ;

oid  ospfIfTable_oid[]   = { OSPF_MIB_ROOT_OID , 7};

static struct ipran_snmp_data_cache *h3c_ospf_if_cache = NULL ;


FindVarMethod ospfNbrTable_get ;
struct variable2 ospfNbrTable_variables[] =
{
    {ospfNbrIpAddress,      ASN_IPADDRESS,  NOACCESS,   ospfNbrTable_get, 2, {1, 1}},
    {ospfNbrAddressLessIf,  ASN_INTEGER,    NOACCESS,   ospfNbrTable_get, 2, {1, 2}},
    {ospfNbrRtrId,          ASN_IPADDRESS,  RONLY,      ospfNbrTable_get, 2, {1, 3}},
    {ospfNbrOption,         ASN_INTEGER,    RONLY,      ospfNbrTable_get, 2, {1, 4}},
    {ospfNbrPriority,       ASN_INTEGER,    RONLY,      ospfNbrTable_get, 2, {1, 5}},
    {ospfNbrState,          ASN_INTEGER,    RONLY,      ospfNbrTable_get, 2, {1, 6}},
    {ospfNbrEvents,         ASN_COUNTER,    RONLY,      ospfNbrTable_get, 2, {1, 7}},
    {ospfNbrLsRetransQLen,  ASN_GAUGE,      RONLY,      ospfNbrTable_get, 2, {1, 8}}
} ;

oid  ospfNbrTable_oid[]   = { OSPF_MIB_ROOT_OID , 10};

static struct ipran_snmp_data_cache *h3c_ospfNbrTable_cache = NULL ;


FindVarMethod hhospfNetworkTable_get ;
struct variable2 hhospfNetworkTable_variables[] =
{
    {hhospfProcessId_mib,           ASN_INTEGER,    NOACCESS,   hhospfNetworkTable_get, 2, {1, 1}},
    {hhospfAreaId_mib,              ASN_IPADDRESS,  NOACCESS,   hhospfNetworkTable_get, 2, {1, 2}},
    {hhospfNetworkIpAddr_mib,       ASN_IPADDRESS,  NOACCESS,   hhospfNetworkTable_get, 2, {1, 3}},
    {hhospfNetworkIpAddrMask_mib,   ASN_IPADDRESS,  RONLY,      hhospfNetworkTable_get, 2, {1, 4}}
} ;

oid  hhospfNetworkTable_oid[]   = { HH_OSPF_MIB_ROOT_OID , 1};

static struct ipran_snmp_data_cache *h3c_hhospfNetworkTable_cache = NULL ;

void  init_mib_h3c_ospf(void)
{
    REGISTER_MIB("h3c/ospfAreaTable", ospfAreaTable_variables, variable2,
                 ospfAreaTablee_oid) ;
    REGISTER_MIB("h3c/ospfIfTable", ospfIfTable_variables, variable2,
                 ospfIfTable_oid) ;
    REGISTER_MIB("h3c/ospfNbrTable", ospfNbrTable_variables, variable2,
                 ospfNbrTable_oid) ;
    REGISTER_MIB("h3c/hhospfNetworkTable", hhospfNetworkTable_variables, variable2,
                 hhospfNetworkTable_oid) ;
}

static struct ipc_mesg_n *hhrOSPF_H3C_ALL_Table_get_bulk(void *pIndex, int subtype)
{
    int index_len = 0;

    switch (subtype)
    {
        case OSPF_SNMP_AREA_GET_H3C:
            index_len = sizeof(uint32_t);
            break;

        case OSPF_SNMP_INTERFACE_GET_H3C:
            index_len = sizeof(struct ospf_key_prefix_h3c);
            break;

        case OSPF_SNMP_NEIGHBOR_GET_H3C:
            index_len = sizeof(struct ospf_key_prefix_h3c);
            break;
		
		case OSPF_SNMP_NETWORK_GET_H3C:
            index_len = sizeof(struct ospf_key_network_h3c);
            break;

        default:
            break;
    }

	return ipc_sync_send_n2((void *)pIndex, index_len, 1, MODULE_ID_OSPF, MODULE_ID_SNMPD, \
							IPC_TYPE_OSPF, subtype, IPC_OPCODE_GET_BULK, 0, 1000);
}


#if 0
struct statics_ospf_area_h3c *snmp_get_ospf_area_h3c_bulk(uint32_t ifindex, int module_id ,
                                                          int *pdata_num)
{
    /*struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(&ifindex, sizeof(uint32_t), 1, MODULE_ID_OSPF, module_id,
                                                         IPC_TYPE_OSPF, OSPF_SNMP_AREA_GET_H3C, IPC_OPCODE_GET_BULK,
                                                         h3c_ospf_area_cache->nullkey);*/
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&ifindex, sizeof(uint32_t), 1, MODULE_ID_OSPF, module_id,
                                                         IPC_TYPE_OSPF, OSPF_SNMP_AREA_GET_H3C, IPC_OPCODE_GET_BULK,
                                                         h3c_ospf_area_cache->nullkey, 2000);

    if (pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (void *) pmesg->msg_data;
    }

    return NULL;
}
#endif


int ospfAreaTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                    struct statics_ospf_area_h3c  *index)
{
    struct statics_ospf_area_h3c *area_info = NULL;
    int data_num = 0;
    int ret = 0;

	struct ipc_mesg_n *pMsgRcv = hhrOSPF_H3C_ALL_Table_get_bulk(index->area_id, OSPF_SNMP_AREA_GET_H3C);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	area_info = (struct statics_ospf_area_h3c *)pMsgRcv->msg_data;
	if (0 == data_num || NULL == area_info)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }

	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, area_info , sizeof(struct statics_ospf_area_h3c)) ;
        area_info++;
    }
	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
	
	return TRUE;
}

struct statics_ospf_area_h3c *ospfAreaTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                        int exact,
                                                        const struct statics_ospf_area_h3c  *index_input)
{
    struct listnode  *node, *nnode;
    struct statics_ospf_area_h3c *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 1 == cache->nullkey)
        {
            return cache->data_list->head->data;
        }

        if (data1_find->area_id == index_input->area_id)
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
ospfAreaTable_get(struct variable *vp,
                  oid *name,
                  size_t *length,
                  int exact, size_t *var_len, WriteMethod **write_method)
{
    struct statics_ospf_area_h3c index ;
    int ret = 0;
    struct statics_ospf_area_h3c *ospf_area = NULL;

    memset(&index, 0 , sizeof(struct statics_ospf_area_h3c));

    if (NULL == h3c_ospf_area_cache)
    {
        h3c_ospf_area_cache = snmp_cache_init(sizeof(struct statics_ospf_area_h3c) ,
                                              ospfAreaTable_get_data_from_ipc ,
                                              ospfAreaTable_node_lookup) ;

        if (NULL == h3c_ospf_area_cache)
        {
            return (NULL);
        }
    }

    ret = ipran_snmp_ip_index_get(vp, name, length, &index.area_id, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (ret == 1)
    {
        index.is_first = 1;
        snmp_cache_nullkey_set(h3c_ospf_area_cache, 1);
    }
    else
    {
        index.is_first = 0;
        snmp_cache_nullkey_set(h3c_ospf_area_cache, 0);
    }

    ospf_area = snmp_cache_get_data_by_index(h3c_ospf_area_cache, exact, &index);

    if (NULL == ospf_area)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_ip_index_set(vp, name, length, ospf_area->area_id);
    }

    switch (vp->magic)
    {
        case ospfAuthType :
            int_value = ospf_area->auth_type;

            switch (int_value)
            {
                case OSPF_AUTH_NULL :
                    int_value = 0 ;
                    break ;

                case OSPF_AUTH_SIMPLE :
                    int_value = 1 ;
                    break ;

                case OSPF_AUTH_CRYPTOGRAPHIC:
                    int_value = 2 ;
                    break ;

                default :
                    int_value = 0 ;
                    break ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  ospfImportAsExtern:
            int_value = ospf_area->external_routing;

            switch (int_value)
            {
                case OSPF_AREA_DEFAULT:
                    int_value = 1 ;
                    break ;

                case OSPF_AREA_STUB:
                    int_value = 2 ;
                    break ;

                case OSPF_AREA_NSSA:
                    int_value = 3 ;
                    break ;

                default :
                    int_value = 0 ;
                    break ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  ospfSpfRuns:
            int_value = ospf_area->spf_calculation;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  ospfAreaBdrRtrCount:
            int_value = ospf_area->abr_count;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  ospfAsBdrRtrCount:
            int_value = ospf_area->asbr_count;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  ospfAreaLsaCount:
            int_value = ospf_area->lsa_count;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  ospfAreaSummary:
            int_value = ospf_area->summary;

            switch (int_value)
            {
                case  0 :
                    int_value = 2;
                    break ;

                case 1 :
                    int_value = 1 ;
                    break ;

                default :
                    int_value = 2 ;
                    break ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  ospfAreaNssaTranslatorRole:
            int_value = ospf_area->NSSATranslatorRole;

            switch (int_value)
            {
                case OSPF_NSSA_ROLE_CANDIDATE:
                    int_value = 2 ;
                    break ;

                case OSPF_NSSA_ROLE_ALWAYS:
                    int_value = 1 ;
                    break ;

                default :
                    int_value = 1 ;
                    break ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  ospfAreaNssaTranslatorState:
            int_value = ospf_area->NSSATranslatorState;

            switch (int_value)
            {
                case OSPF_NSSA_TRANSLATE_DISABLED :
                    int_value = 3 ;
                    break ;

                case OSPF_NSSA_TRANSLATE_ENABLED :
                    int_value = 1 ;
                    break ;

                default :
                    int_value = 2 ;
                    break ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  ospfAreaNssaTranslatorStabilityInterval:
            int_value = ospf_area->NSSATranslatorStabilityInterval;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return  NULL ;

    }

}

/*-----------------------------------------------------------------------------------------*/
#if 0
struct statics_ospf_if_h3c *snmp_get_ospf_if_h3c_bulk(struct ospf_key_prefix_h3c *index, int module_id,
                                                      int *pdata_num)
{
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(index, sizeof(struct ospf_key_prefix_h3c), 1, MODULE_ID_OSPF, module_id,
                                                         IPC_TYPE_OSPF, OSPF_SNMP_INTERFACE_GET_H3C, IPC_OPCODE_GET_BULK, h3c_ospf_if_cache->nullkey);

    if (pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (void *) pmesg->msg_data;
    }

    return NULL;
}
#endif

int ospfIfTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                  struct statics_ospf_if_h3c  *index)
{
    struct statics_ospf_if_h3c *if_info = NULL;
    int data_num = 0;
    int ret = 0;
    struct   ospf_key_prefix_h3c key ;

    key.prefix4 = index->prefix4;
    key.ifindex = index->ifindex;

	struct ipc_mesg_n *pMsgRcv = hhrOSPF_H3C_ALL_Table_get_bulk(&key, OSPF_SNMP_INTERFACE_GET_H3C);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	if_info = (struct statics_ospf_if_h3c *)pMsgRcv->msg_data;
	if (0 == data_num || NULL == if_info)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }

	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, if_info , sizeof(struct statics_ospf_if_h3c)) ;
        if_info++;
    }
	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
	
	return TRUE;
}

struct statics_ospf_if_h3c *ospfIfTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                    int exact,
                                                    const struct statics_ospf_if_h3c  *index_input)
{
    struct listnode  *node, *nnode;
    struct statics_ospf_if_h3c *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 1 == cache->nullkey)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->prefix4 == index_input->prefix4 && data1_find->ifindex == index_input->ifindex)
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
ospfIfTable_get(struct variable *vp,
                oid *name,
                size_t *length,
                int exact, size_t *var_len, WriteMethod **write_method)
{
    struct statics_ospf_if_h3c index ;
    int ret = 0;
    struct statics_ospf_if_h3c *ospf_if = NULL;

    memset(&index, 0 , sizeof(struct statics_ospf_if_h3c));

    if (NULL == h3c_ospf_if_cache)
    {
        h3c_ospf_if_cache = snmp_cache_init(sizeof(struct statics_ospf_if_h3c) ,
                                            ospfIfTable_get_data_from_ipc ,
                                            ospfIfTable_node_lookup) ;

        if (NULL == h3c_ospf_if_cache)
        {
            return (NULL);
        }
    }

    ret = ipran_snmp_ip_int_index_get(vp, name, length, &index.prefix4, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (ret == 1)
    {
        snmp_cache_nullkey_set(h3c_ospf_if_cache, 1);
    }
    else
    {
        snmp_cache_nullkey_set(h3c_ospf_if_cache, 0);
    }

    ospf_if = snmp_cache_get_data_by_index(h3c_ospf_if_cache, exact, &index);

    if (NULL == ospf_if)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_ip_int_index_set(vp, name, length, ospf_if->prefix4, ospf_if->ifindex);
    }

    switch (vp->magic)
    {
        case ospfIfAreaId_mib :
            ip_value = ospf_if->area_id;
            *var_len = sizeof(uint32_t);
            return (u_char *)&ip_value ;

        case ospfIfType_mib :
            int_value = ospf_if->type;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ospfIfAdminStat_mib :
            int_value = ospf_if->IfAdminStat;
            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case ospfIfDesignatedRouter_mib :
            ip_value = ospf_if->ospfIfDesignatedRouter;
            *var_len = sizeof(uint32_t);
            return (u_char *)&ip_value;

        case ospfIfBackupDesignatedRouter_mib :
            ip_value = ospf_if->ospfIfBackupDesignatedRouter;
            *var_len = sizeof(uint32_t);
            return (u_char *)&ip_value;

        case ospfIfRtrPriority_mib :
            int_value = ospf_if->ospfIfRtrPriority;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ospfIfTransitDelay_mib :
            int_value = ospf_if->ospfIfTransitDelay;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ospfIfRetransInterval_mib :
            int_value = ospf_if->ospfIfRetransInterval;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ospfIfRtrDeadInterval_mib :
            int_value = ospf_if->ospfIfRtrDeadInterval;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ospfIfHelloInterval_mib :
            int_value = ospf_if->ospfIfHelloInterval;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ospfIfState_mib :
            int_value = ospf_if->ospfIfState;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ospfIfEvents_mib :
            int_value = ospf_if->ospfIfEvents;
            *var_len = sizeof(uint32_t);
            return (u_char *)&int_value ;

        case ospfIfAuthKey_mib :
            memset(str_value, 0, STRING_LEN);
            strcpy(str_value, ospf_if->ospfIfAuthKey);
            // sprintf(str_value , "%s",ospf_if->ospfIfAuthKey);
            *var_len = strlen(str_value);
            return (u_char *)(str_value);

        case ospfIfAuthType_mib :
            int_value = ospf_if->ospfIfAuthType;

            switch (int_value)
            {
                case OSPF_AUTH_NULL  :
                    int_value = 0 ;
                    break ;

                case  OSPF_AUTH_SIMPLE :
                    int_value = 1 ;
                    break ;

                case  OSPF_AUTH_CRYPTOGRAPHIC :
                    int_value = 2 ;
                    break ;

                default :
                    int_value = 0 ;
                    break ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case ospfIfDesignatedRouterId_mib :
            ip_value = ospf_if->IfDesignatedRouterid;
            *var_len = sizeof(uint32_t);
            return (u_char *)&ip_value ;

        case ospfIfBackupDesignatedRouterId_mib :
            ip_value = ospf_if->IfBackupDesignatedRouterid;
            *var_len = sizeof(uint32_t);
            return (u_char *)&ip_value ;

        default:
            return NULL;
    }
}


/*-----------------------------------------------------------------------------------------*/
#if 0
struct statics_ospf_neighbor_h3c *snmp_get_ospf_nbr_h3c_bulk(struct statics_ospf_neighbor_h3c  *index, int module_id ,
                                                             int *pdata_num)
{
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(index, sizeof(struct statics_ospf_neighbor_h3c), 1, MODULE_ID_OSPF, module_id,
                                                         IPC_TYPE_OSPF, OSPF_SNMP_NEIGHBOR_GET_H3C, IPC_OPCODE_GET_BULK, h3c_ospfNbrTable_cache->nullkey);

    if (pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (void *) pmesg->msg_data;
    }

    return NULL;
}
#endif

int ospfNbrTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                   struct statics_ospf_neighbor_h3c  *index)
{
    struct statics_ospf_neighbor_h3c *nbr_info = NULL;
    int data_num = 0;
    int ret = 0;
    struct   ospf_key_prefix_h3c key ;

    key.prefix4 = index->prefix4;
    key.ifindex = index->ifindex;

	struct ipc_mesg_n *pMsgRcv = hhrOSPF_H3C_ALL_Table_get_bulk(&key, OSPF_SNMP_NEIGHBOR_GET_H3C);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	nbr_info = (struct statics_ospf_neighbor_h3c *)pMsgRcv->msg_data;
	if (0 == data_num || NULL == nbr_info)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }

	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, nbr_info , sizeof(struct statics_ospf_neighbor_h3c)) ;
        nbr_info++;
    }
	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
	
	return TRUE;
}

struct statics_ospf_neighbor_h3c *ospfNbrTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                           int exact,
                                                           const struct statics_ospf_neighbor_h3c  *index_input)
{
    struct listnode  *node, *nnode;
    struct statics_ospf_if_h3c *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 1 == cache->nullkey)
        {
            return cache->data_list->head->data;
        }

        if (data1_find->prefix4 == index_input->prefix4 && data1_find->ifindex == index_input->ifindex)
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

u_char *
ospfNbrTable_get(struct variable *vp,
                 oid *name,
                 size_t *length,
                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct statics_ospf_neighbor_h3c index ;
    int ret = 0;
    struct statics_ospf_neighbor_h3c *ospf_nbr = NULL;

    memset(&index, 0, sizeof(struct statics_ospf_neighbor_h3c));

    if (NULL == h3c_ospfNbrTable_cache)
    {
        h3c_ospfNbrTable_cache = snmp_cache_init(sizeof(struct statics_ospf_neighbor_h3c) ,
                                                 ospfNbrTable_get_data_from_ipc ,
                                                 ospfNbrTable_node_lookup) ;

        if (NULL == h3c_ospfNbrTable_cache)
        {
            return (NULL);
        }
    }

    ret = ipran_snmp_ip_int_index_get(vp, name, length, &index.prefix4, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (ret == 1)
    {
        snmp_cache_nullkey_set(h3c_ospfNbrTable_cache, 1);
    }
    else
    {
        snmp_cache_nullkey_set(h3c_ospfNbrTable_cache, 0);
    }

    ospf_nbr = snmp_cache_get_data_by_index(h3c_ospfNbrTable_cache , exact, &index);

    if (NULL == ospf_nbr)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_ip_int_index_set(vp, name, length, ospf_nbr->prefix4, ospf_nbr->ifindex);
    }

    switch (vp->magic)
    {
        case ospfNbrRtrId :
            ip_value = ospf_nbr->router_id;
            *var_len = sizeof(uint32_t);
            return (u_char *)&ip_value;

        case ospfNbrOption :
            int_value = ospf_nbr->options;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ospfNbrPriority :
            int_value = ospf_nbr->priority;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ospfNbrState :
            int_value = ospf_nbr->state;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ospfNbrEvents :
            int_value = ospf_nbr->state_change;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return  NULL ;

    }
}

/*-----------------------------------------------------------------------------------------*/
#if 0
struct statics_ospf_network *snmp_get_hhospfNetworkTable_h3c_bulk(struct statics_ospf_neighbor_h3c  *index, int module_id ,
                                                                  int *pdata_num)
{
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(index, sizeof(struct statics_ospf_neighbor_h3c), 1, MODULE_ID_OSPF, module_id,
                                                         IPC_TYPE_OSPF, OSPF_SNMP_NETWORK_GET_H3C, IPC_OPCODE_GET_BULK, h3c_hhospfNetworkTable_cache->nullkey);

    if (pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (void *) pmesg->msg_data;
    }

    return NULL;
}
#endif

int hhospfNetworkTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                         struct statics_ospf_network  *index)
{
    struct statics_ospf_network *net_info = NULL;
    int data_num = 0;
    int ret = 0;
    struct   ospf_key_network_h3c key ;

    key.prefix4  = index->p.u.prefix4.s_addr;
    key.area_id = index->area_id;
    key.ospf_id = index->ospf_id;

	struct ipc_mesg_n *pMsgRcv = hhrOSPF_H3C_ALL_Table_get_bulk(&key, OSPF_SNMP_NETWORK_GET_H3C);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	net_info = (struct statics_ospf_network *)pMsgRcv->msg_data;
	if (0 == data_num || NULL == net_info)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }

	for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(cache, net_info , sizeof(struct statics_ospf_network)) ;
        net_info++;
    }
	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
	
	return TRUE;
}

struct statics_ospf_network *hhospfNetworkTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                            int exact,
                                                            const struct statics_ospf_network  *index_input)
{
    struct listnode  *node, *nnode;
    struct statics_ospf_network *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input  || 1 == cache->nullkey)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->p.u.prefix4.s_addr == index_input->p.u.prefix4.s_addr &&
                data1_find->area_id == index_input->area_id &&
                data1_find->ospf_id == index_input->ospf_id)
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


u_char *
hhospfNetworkTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{
    struct statics_ospf_network index;
    int ret = 0;
    u_int32_t ospf_id_tmp = 0;
    u_int32_t area_id_tmp = 0;
    u_int32_t ip_tmp = 0;
    struct in_addr temp_addr;
    struct statics_ospf_network *ospf_network = NULL;

    memset(&index, 0, sizeof(struct statics_ospf_network));

    ret = ipran_snmp_int_ipx2_index_get(vp, name, length, &ospf_id_tmp, &area_id_tmp,
                                        &ip_tmp, exact);

    index.ospf_id = ospf_id_tmp;
    index.area_id = area_id_tmp;
    index.p.u.prefix4.s_addr = ip_tmp;

    if (NULL == h3c_hhospfNetworkTable_cache)
    {
        h3c_hhospfNetworkTable_cache = snmp_cache_init(sizeof(struct statics_ospf_network) ,
                                                       hhospfNetworkTable_get_data_from_ipc ,
                                                       hhospfNetworkTable_node_lookup) ;

        if (NULL == h3c_hhospfNetworkTable_cache)
        {
            return (NULL);
        }
    }

    if (ret < 0)
    {
        return NULL;
    }

    if (ret == 1)
    {
        snmp_cache_nullkey_set(h3c_hhospfNetworkTable_cache, 1);
    }
    else
    {
        snmp_cache_nullkey_set(h3c_hhospfNetworkTable_cache, 0);
    }

    ospf_network = snmp_cache_get_data_by_index(h3c_hhospfNetworkTable_cache , exact, &index);

    if (NULL == ospf_network)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_ipx2_index_set(vp, name, length, ospf_network->ospf_id, ospf_network->area_id ,
                                      ospf_network->p.u.prefix4.s_addr);
    }

    switch (vp->magic)
    {
        case hhospfNetworkIpAddrMask_mib :
            ip_value = ospf_network->p.prefixlen;
            masklen_to_netip(ip_value, &temp_addr);
            ip_value = temp_addr.s_addr;
            *var_len = sizeof(u_int32_t);
            return (u_char *)&ip_value;

        default:
            return NULL;
    }
}


