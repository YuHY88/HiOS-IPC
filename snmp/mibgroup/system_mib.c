/*
 *  System MIB group implementation - system.c
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

#include "net-snmp-config.h"

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <sys/types.h>
#if HAVE_WINSOCK_H
#include <winsock.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <ctype.h>
#if HAVE_UTSNAME_H
#include <utsname.h>
#else
#if HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "util_funcs.h"
#include "system_mib.h"
#include "struct.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <net-snmp-config.h>
#include <types.h>
#include <sys/sysinfo.h>

#include <net-snmp-includes.h>
#include <net-snmp-agent-includes.h>
#include <snmp_index_operater.h>

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include "lib/ifm_common.h"
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/types.h>
#include <lib/route_com.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/inet_ip.h>
#include <lib/linklist.h>

#include "ipran_snmp_data_cache.h"
#include "ifm/ifm_message.h"
#include "system_group.h"


/*********************
*
*  Kernel & interface information,
*   and internal forward declarations
*
*********************/

static uint32_t         int_value;
static uchar            str_value[STRING_LEN] = {'\0'};
static uint32_t         ip_value = 0;
static struct counter64 uint64_value;
static uchar            mac_value[6] = {0};
static uint32_t         uint_value = 0;
static long             long_value = 0;

static struct ipran_snmp_data_cache *ifTable_cache = NULL ;

/*
 * define the structure we're going to ask the agent to register our
 * information at
 */
struct variable1 system_variables[] =
{
    {VERSION_DESCR, ASN_OCTET_STR, RONLY, var_system, 1, {1}},
    {VERSIONID, ASN_OBJECT_ID, RONLY, var_system, 1, {2}},
    {UPTIME, ASN_TIMETICKS, RONLY, var_system, 1, {3}},
    {SYSCONTACT, ASN_OCTET_STR, RWRITE, var_system, 1, {4}},
    {SYSTEMNAME, ASN_OCTET_STR, RWRITE, var_system, 1, {5}},
    {SYSLOCATION, ASN_OCTET_STR, RWRITE, var_system, 1, {6}},
    {SYSSERVICES, ASN_INTEGER, RONLY, var_system, 1, {7}},
    {SYSORLASTCHANGE, ASN_TIMETICKS, RONLY, var_system, 1, {8}}
};

static oid ifCount_oid[] = {SNMP_OID_MIB2, 2 };
FindVarMethod ifCount_get;
struct variable1 ifcount_variables[] =
{
    {IfNumber, ASN_INTEGER, RONLY, ifCount_get, 1, {1}},
};

static oid ifTable_oid[] = {SNMP_OID_MIB2, 2 , 2};
FindVarMethod ifTable_get;
struct variable2 ifTable_variables[] =
{
    {Ifindex,           ASN_INTEGER,   RONLY, ifTable_get, 2, {1,  1}},
    {IfDescr,           ASN_OCTET_STR, RONLY, ifTable_get, 2, {1,  2}},
    {IfType,            ASN_INTEGER,   RONLY, ifTable_get, 2, {1,  3}},
    {IfMtu,             ASN_INTEGER,   RONLY, ifTable_get, 2, {1,  4}},
    {IfSpeed,           ASN_GAUGE,     RONLY, ifTable_get, 2, {1,  5}},
    {IfPhysAddress,     ASN_OCTET_STR, RONLY, ifTable_get, 2, {1,  6}},
    {IfAdminStatus,     ASN_INTEGER,   RONLY, ifTable_get, 2, {1,  7}},
    {IfOperStatus,      ASN_INTEGER,   RONLY, ifTable_get, 2, {1,  8}},
    {IfLastChange,      ASN_INTEGER,   RONLY, ifTable_get, 2, {1,  9}},
    {IfInOctets,        ASN_COUNTER,   RONLY, ifTable_get, 2, {1, 10}},
    {IfInUcastPkts,     ASN_COUNTER,   RONLY, ifTable_get, 2, {1, 11}},
    {IfInNUcastPkts,    ASN_COUNTER,   RONLY, ifTable_get, 2, {1, 12}},
    {IfInDiscards,      ASN_COUNTER,   RONLY, ifTable_get, 2, {1, 13}},
    {IfInErrors,        ASN_COUNTER,   RONLY, ifTable_get, 2, {1, 14}},
    {IfInUnknownProtos, ASN_COUNTER,   RONLY, ifTable_get, 2, {1, 15}},
    {IfOutOctets,       ASN_COUNTER,   RONLY, ifTable_get, 2, {1, 16}},
    {IfOutUcastPkts,    ASN_COUNTER,   RONLY, ifTable_get, 2, {1, 17}},
    {IfOutNUcastPkts,   ASN_COUNTER,   RONLY, ifTable_get, 2, {1, 18}},
    {IfOutDiscards,     ASN_COUNTER,   RONLY, ifTable_get, 2, {1, 19}},
    {IfOutErrors,       ASN_COUNTER,   RONLY, ifTable_get, 2, {1, 20}},
    {IfOutQLen,         ASN_GAUGE,     RONLY, ifTable_get, 2, {1, 21}},
    {IfSpecific,        ASN_OBJECT_ID, RONLY, ifTable_get, 2, {1, 22}}
};



static oid ifTablex_oid[] = {SNMP_OID_IFXTABLE , 1};
FindVarMethod ifTablex_get;
struct variable2 ifTablex_variables[] =
{
    {IfName,                 ASN_OCTET_STR, RONLY, ifTablex_get, 2, {1,  1}},
    {IfInMulticastPkts,      ASN_COUNTER,   RONLY, ifTablex_get, 2, {1,  2}},
    {IfInBroadcastPkts,      ASN_COUNTER,   RONLY, ifTablex_get, 2, {1,  3}},
    {IfOutMulticastPkts,     ASN_COUNTER,   RONLY, ifTablex_get, 2, {1,  4}},
    {IfOutBroadcastPkts,     ASN_COUNTER,   RONLY, ifTablex_get, 2, {1,  5}},
    {IfHCInOctets,           ASN_COUNTER64, RONLY, ifTablex_get, 2, {1,  6}},
    {IfHCInUcastPkts,        ASN_COUNTER64, RONLY, ifTablex_get, 2, {1,  7}},
    {IfHCInMulticastPkts,    ASN_COUNTER64, RONLY, ifTablex_get, 2, {1,  8}},
    {IfHCInBroadcastPkts,    ASN_COUNTER64, RONLY, ifTablex_get, 2, {1,  9}},
    {IfHCOutOctets,          ASN_COUNTER64, RONLY, ifTablex_get, 2, {1, 10}},
    {IfHCOutUcastPkts,       ASN_COUNTER64, RONLY, ifTablex_get, 2, {1, 11}},
    {IfHCOutMulticastPkts,   ASN_COUNTER64, RONLY, ifTablex_get, 2, {1, 12}},
    {IfHCOutBroadcastPkts,   ASN_COUNTER64, RONLY, ifTablex_get, 2, {1, 13}},
    {IfLinkUpDownTrapEnable, ASN_INTEGER,   RONLY, ifTablex_get, 2, {1, 14}},
    {IfHighSpeed,            ASN_INTEGER,   RONLY, ifTablex_get, 2, {1, 15}},
    {IfPromiscuousMode,      ASN_INTEGER,   RONLY, ifTablex_get, 2, {1, 16}},
    {IfConnectorPresent,     ASN_INTEGER,   RONLY, ifTablex_get, 2, {1, 17}},
    {IfAlias,                ASN_OCTET_STR, RONLY, ifTablex_get, 2, {1, 18}}
};

/*
 * Define the OID pointer to the top of the mib tree that we're
 * registering underneath
 */
oid             system_variables_oid[] = { SNMP_OID_MIB2, 1 };

void
init_system_mib(void)
{
    REGISTER_MIB("mibII/system", system_variables, variable1,
                 system_variables_oid);

    REGISTER_MIB("mibII/ifCount", ifcount_variables, variable1,
                 ifCount_oid);

    REGISTER_MIB("mibII/ifTable", ifTable_variables, variable2,
                 ifTable_oid);

    REGISTER_MIB("mibII/ifxTable", ifTablex_variables, variable2,
                 ifTablex_oid);
}

u_char *var_system(struct variable *vp,
                   oid *name,
                   size_t *length,
                   int exact, size_t *var_len, WriteMethod **write_method)
{
    struct system_group *psystem_group;
    struct sysinfo info;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    *write_method = NULL;

    psystem_group = sys_grp_value_get();

    if (NULL == psystem_group)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case VERSION_DESCR:
            *var_len = strlen(psystem_group->sysDesc);
            return (u_char *)(psystem_group->sysDesc);

        case VERSIONID:
            *var_len = sizeof(oid) * psystem_group->sysoid_len;
            return (u_char *)(psystem_group->sysObjectID);

        case UPTIME:
            sysinfo(&info);
            long_value = info.uptime * 100;
            *var_len = 4;
            return ((u_char *)&long_value);

        case SYSCONTACT:
            *var_len = strlen(psystem_group->sysContact);
            return (u_char *)(psystem_group->sysContact);

        case SYSTEMNAME:
            *var_len = strlen(psystem_group->sysName);
            return (u_char *)(psystem_group->sysName);

        case SYSLOCATION:
            *var_len = strlen(psystem_group->sysLocation);
            return (u_char *)(psystem_group->sysLocation);

        case SYSSERVICES:
            int_value = psystem_group->sysServices;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}



u_char         *
ifCount_get(struct variable *vp,
            oid *name,
            size_t *length,
            int exact, size_t *var_len, WriteMethod **write_method)
{
    int ret = 0;

    if (header_generic(vp, name, length, exact, var_len, write_method) ==
            MATCH_FAILED)
    {
        return NULL;
    }

    *write_method = NULL;

    switch (vp->magic)
    {
        case IfNumber:
            if(ifm_get_port_count(MODULE_ID_SNMPD, &int_value) < 0)
            {
            	int_value = 0;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return (NULL);
    }
}

int ifTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                              struct ifm_snmp *pInfo_input)
{
    struct ifm_snmp *pifm_info = NULL;
	struct ifm_snmp *tpifm_info = NULL;
    int data_num = 0;
    int ret = 0;	

    pifm_info = (struct ifm_snmp *)ifm_get_statistics_bulk(pInfo_input->ifm.ifindex, MODULE_ID_SNMPD, &data_num);
	tpifm_info = pifm_info;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ifindex [%d] data_num [%d]\n", __FILE__, __LINE__, __func__ , pInfo_input->ifm.ifindex, data_num);

    if (0 == data_num || NULL == pifm_info)
    {
		zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : get if table over\n", __func__, __LINE__);
		mem_share_free_bydata(tpifm_info, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
			zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] : add ifindex --- 0x%x\n", __func__, __LINE__, pifm_info->ifm.ifindex);
            snmp_cache_add(cache, pifm_info , sizeof(struct ifm_snmp));
            pifm_info++;
        }
		mem_share_free_bydata(tpifm_info, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct ifm_snmp *ifTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                          int exact,
                                          uint32_t  *pIndex_input)
{
    struct listnode  *node , *nnode;
    struct ifm_snmp *data_find;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, data_find))
    {
        //zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_find->ifm.ifindex[%d]\n", __FILE__, __LINE__, __func__ , data_find->ifm.ifindex);

        if (NULL == pIndex_input || 0 == *pIndex_input)
        {
            return cache->data_list->head->data ;
        }

        if (NULL == data_find)
        {
            return (NULL);
        }

        if (data_find->ifm.ifindex == *pIndex_input)
        {
            if (1 == exact) //get
            {
                return data_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    data_find = (struct ifm_snmp *)node->next->data ;
                    return node->next->data ;
                }
            }
        }
    }

    return (NULL);
}


u_char *ifTable_get(struct variable *vp,
                    oid *name,
                    size_t *length,
                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ifm_snmp *pIfm_info;
    uint32_t ifindex = 0 ;
    static u_long   ulret = 0;
    int ret = 0 ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &ifindex, exact);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ifindex [%x] \n", __FILE__, __LINE__, __func__ , ifindex);

    if (ret < 0)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
        return NULL;
    }

    if (NULL == ifTable_cache)
    {
        ifTable_cache = snmp_cache_init(sizeof(struct ifm_snmp) ,
                                        ifTable_get_data_from_ipc ,
                                        ifTable_node_lookup);

        if (NULL == ifTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    pIfm_info = (struct ifm_snmp *)snmp_cache_get_data_by_index(ifTable_cache , exact, &ifindex);
	if(NULL == pIfm_info)
	{
		return NULL;
	}
	
    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pIfm_info->ifm.ifindex);
    }

    switch (vp->magic)
    {
        case Ifindex:
            *var_len = sizeof(int);
            int_value = pIfm_info->ifm.ifindex;
            return (u_char *)(&int_value);

        case IfDescr:
            memset(str_value, 0, STRING_LEN);
            ifm_get_name_by_ifindex(pIfm_info->ifm.ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case IfType:
            *var_len = sizeof(int);

            if (IFM_TYPE_IS_METHERNET(pIfm_info->ifm.ifindex))
            {
                int_value = pIfm_info->ifm.type ;
                int_value = 6 ;
            }
            else if (IFM_TYPE_IS_TDM(pIfm_info->ifm.ifindex))
            {
                int_value = 19;
            }
            else
            {
                int_value = 1;
            }

            return (u_char *)(&int_value);

        case IfMtu:
            *var_len = sizeof(int);
            int_value = pIfm_info->ifm.mtu;
            return (u_char *)(&int_value);

        case IfSpeed:
            *var_len = sizeof(int);
            int_value = pIfm_info->port_info.speed ;

            if (IFNET_SPEED_GE == int_value)
            {
                int_value = 1000 ;
            }
            else if (IFNET_SPEED_FE == int_value)
            {
                int_value = 100 ;
            }
            else if (IFNET_SPEED_10GE == int_value)
            {
                int_value = 10000 ;
            }
            else if (IFM_TYPE_IS_TDM(pIfm_info->ifm.ifindex))
            {
                int_value = 2 ;
            }
            else
            {
                int_value = 0 ;
            }

            return (u_char *)(&int_value);

        case IfPhysAddress:
            *var_len = 6;

            if (NULL != pIfm_info->ifm.mac)
            {
                memcpy(&mac_value, pIfm_info->ifm.mac, 6);
            }

            return (mac_value);

        case IfAdminStatus:
            *var_len = sizeof(int);
            int_value = pIfm_info->ifm.shutdown;

            if (int_value == IFNET_NO_SHUTDOWN)
            {
                int_value = 1 ;
            }
            else
            {
                int_value = 2 ;
            }

            return (u_char *)(&int_value);

        case IfOperStatus:
            *var_len = sizeof(int);
            int_value = pIfm_info->ifm.status;

            if (int_value == IFNET_LINKUP)
            {
                int_value = 1 ;
            }
            else
            {
                int_value = 2 ;
            }

            return (u_char *)(&int_value);

        case IfLastChange:
            ulret = pIfm_info->ifm.last_change ;
            ulret *= 100;
            *var_len = sizeof(u_long);
            return ((u_char *)&ulret);

        case IfInOctets:

            if (NULL != pIfm_info)
            {
                int_value = pIfm_info->pcounter.rx_bytes;
            }
            else
            {
                int_value = 0;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfInUcastPkts:
            if (NULL != pIfm_info)
            {
                int_value = pIfm_info->pcounter.rx_ucast;
            }
            else
            {
                int_value = 0;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfInNUcastPkts:
            int_value = 0 ;

            if (NULL != pIfm_info)
            {
                int_value = pIfm_info->pcounter.rx_nucast;
            }
            else
            {
                int_value = 0;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfInDiscards:
            int_value = 0 ;

            if (NULL != pIfm_info)
            {
                int_value = pIfm_info->pcounter.rx_dropped;
            }
            else
            {
                int_value = 0 ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfInErrors:

            if (NULL != pIfm_info)
            {
                int_value = pIfm_info->pcounter.rx_errors;
            }
            else
            {
                int_value = 0 ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfInUnknownProtos:

            if (NULL != pIfm_info)
            {
                int_value = pIfm_info->pcounter.rx_uknownprotos;
            }
            else
            {
                int_value = 0;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfOutOctets:

            if (NULL != pIfm_info)
            {
                int_value = pIfm_info->pcounter.tx_bytes;
            }
            else
            {
                int_value = 0 ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfOutUcastPkts:

            if (NULL != pIfm_info)
            {
                int_value = pIfm_info->pcounter.tx_ucast;
            }
            else
            {
                int_value = 0 ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfOutNUcastPkts:

            if (NULL != pIfm_info)
            {
                int_value = pIfm_info->pcounter.tx_nucast;
            }
            else
            {
                int_value = 0 ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfOutDiscards:

            if (NULL != pIfm_info)
            {
                int_value = pIfm_info->pcounter.tx_dropped;
            }
            else
            {
                int_value = 0 ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfOutErrors:

            if (NULL != pIfm_info)
            {
                int_value = pIfm_info->pcounter.tx_errors;
            }
            else
            {
                int_value = 0 ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfOutQLen:
            if (NULL == pIfm_info)
            {
                int_value = 0 ;
            }
            else
            {
                int_value = pIfm_info->pcounter.out_qlen ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfSpecific:
            int_value = 0;
            *var_len = 1;
            return (u_char *)&int_value;

        default :
            return (NULL);
    }
}

u_char *ifTablex_get(struct variable *vp,
                     oid *name,
                     size_t *length,
                     int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ifm_snmp *pIfm_info  ;
    int ifindex = 0 ;
    int trap_linkup_down_enable = 0 ;

    static u_long   ulret;
    int ret = 0 ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &ifindex, exact);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ifindex [%d] \n", __FILE__, __LINE__, __func__ , ifindex);

    if (ret < 0)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
        return NULL;
    }

    if (NULL == ifTable_cache)
    {
        ifTable_cache = snmp_cache_init(sizeof(struct ifm_snmp) ,
                                        ifTable_get_data_from_ipc ,
                                        ifTable_node_lookup);

        if (NULL == ifTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    pIfm_info = snmp_cache_get_data_by_index(ifTable_cache , exact, &ifindex);

    if (NULL == pIfm_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pIfm_info->ifm.ifindex);
    }

    switch (vp->magic)
    {
        case IfName:
            memset(str_value, 0, STRING_LEN);
            ifm_get_name_by_ifindex(pIfm_info->ifm.ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case IfInMulticastPkts:
			int_value = (uint32_t)pIfm_info->pcounter.rx_multicast;
            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfInBroadcastPkts:
			int_value = (uint32_t)pIfm_info->pcounter.rx_broadcast;
            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfOutMulticastPkts:
			int_value = (uint32_t)pIfm_info->pcounter.tx_multicast;
            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfOutBroadcastPkts:
			int_value = (uint32_t)pIfm_info->pcounter.tx_broadcast;
            *var_len = sizeof(int);
            return (u_char *)&int_value ;

        case IfHCInOctets:
			uint64_value.high = (uint32_t)(pIfm_info->pcounter.rx_bytes >> 32);
            uint64_value.low  = (uint32_t)(pIfm_info->pcounter.rx_bytes);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case IfHCInUcastPkts:
			uint64_value.high = (uint32_t)(pIfm_info->pcounter.rx_ucast >> 32);
            uint64_value.low  = (uint32_t)(pIfm_info->pcounter.rx_ucast);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case IfHCInMulticastPkts:
			uint64_value.high = (uint32_t)(pIfm_info->pcounter.rx_multicast >> 32);
            uint64_value.low  = (uint32_t)(pIfm_info->pcounter.rx_multicast);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case IfHCInBroadcastPkts:
			uint64_value.high = (uint32_t)(pIfm_info->pcounter.rx_broadcast >> 32);
            uint64_value.low  = (uint32_t)(pIfm_info->pcounter.rx_broadcast);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case IfHCOutOctets:
			uint64_value.high = (uint32_t)(pIfm_info->pcounter.rx_bytes >> 32);
            uint64_value.low  = (uint32_t)(pIfm_info->pcounter.rx_bytes);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case IfHCOutUcastPkts:
			uint64_value.high = (uint32_t)(pIfm_info->pcounter.tx_bytes >> 32);
            uint64_value.low  = (uint32_t)(pIfm_info->pcounter.tx_bytes);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case IfHCOutMulticastPkts:
			uint64_value.high = (uint32_t)(pIfm_info->pcounter.tx_multicast >> 32);
            uint64_value.low  = (uint32_t)(pIfm_info->pcounter.tx_multicast);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case IfHCOutBroadcastPkts:
			uint64_value.high = (uint32_t)(pIfm_info->pcounter.tx_broadcast >> 32);
            uint64_value.low  = (uint32_t)(pIfm_info->pcounter.tx_broadcast);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case IfLinkUpDownTrapEnable:
            trap_linkup_down_enable = snmp_trap_link_enable_get();

            if (trap_linkup_down_enable)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case IfHighSpeed:
            *var_len = sizeof(int);
            int_value = pIfm_info->port_info.speed ;

            if (IFNET_SPEED_GE == int_value)
            {
                int_value = 1000 ;
            }
            else if (IFNET_SPEED_FE == int_value)
            {
                int_value = 100 ;
            }
            else if (IFNET_SPEED_10GE == int_value)
            {
                int_value = 10000 ;
            }
            else if (IFM_TYPE_IS_TDM(pIfm_info->ifm.ifindex))
            {
                int_value = 2 ;
            }
            else
            {
                int_value = 0 ;
            }

            int_value = (int_value * 1024 * 1024 * 8) / 1000000 ;  //unit:1,000,000 per second
            return (u_char *)(&int_value);

        case IfPromiscuousMode:
//            int_value = pIfm_info->ifm.status;
            *var_len = sizeof(int);
            int_value = 2;
            return (u_char *)&int_value;

        case IfConnectorPresent:
            int_value = pIfm_info->ifm.status;

            if (1 == int_value)
            {
                int_value = 2;
            }
            else
            {
                int_value = 1;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case IfAlias:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pIfm_info->alias, sizeof(pIfm_info->alias));
            *var_len = strlen(str_value);
            return (str_value);

        default :
            return (NULL);
    }
}

