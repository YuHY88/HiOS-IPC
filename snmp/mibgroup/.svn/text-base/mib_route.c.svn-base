/*
 *  ntp snmp browser interface - ntp_snmp.c
 *
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
#include <lib/route_com.h>
#include <lib/linklist.h>
#include <pthread.h>
#include <lib/inet_ip.h>
#include "route/route_main.h"

#include "ipran_snmp_data_cache.h"

#include "mib_route.h"

#define ROUTE_OIDPREFIX         1,3,6,1,2,1,4
#define HHR_ROUTE_OIDPREFIX     1,3,6,1,4,1,9966,5,35,3,9
#define HHR_L3_OIDPREFIX        1,3,6,1,4,1,9966,5,35,3

/*-----------------------------------------------------------------------------------------*/

FindVarMethod ipRouteTable_get;
static struct ipran_snmp_data_cache *ipRouteTable_cache = NULL ;
struct variable2 ipRouteTableVariables[] =
{
    {ipRouteDest,           ASN_IPADDRESS,    NOACCESS,     ipRouteTable_get, 2, {1, 1}},
    {ipRouteIfIndex,        ASN_INTEGER,      RONLY,        ipRouteTable_get, 2, {1, 2}},
    {ipRouteMetric1,        ASN_INTEGER,      RONLY,        ipRouteTable_get, 2, {1, 3}},
    {ipRouteMetric2,        ASN_INTEGER,      RONLY,        ipRouteTable_get, 2, {1, 4}},
    {ipRouteMetric3,        ASN_INTEGER,      RONLY,        ipRouteTable_get, 2, {1, 5}},
    {ipRouteMetric4,        ASN_INTEGER,      RONLY,        ipRouteTable_get, 2, {1, 6}},
    {ipRouteNextHop,        ASN_IPADDRESS,    RONLY,        ipRouteTable_get, 2, {1, 7}},
    {ipRouteType,           ASN_INTEGER,      RONLY,        ipRouteTable_get, 2, {1, 8}},
    {ipRouteProto,          ASN_INTEGER,      RONLY,        ipRouteTable_get, 2, {1, 9}},
    {ipRouteAge,            ASN_INTEGER,      RONLY,        ipRouteTable_get, 2, {1, 10}},
    {ipRouteMask,           ASN_IPADDRESS,    RONLY,        ipRouteTable_get, 2, {1, 11}},
    {ipRouteMetric5,        ASN_INTEGER,      RONLY,        ipRouteTable_get, 2, {1, 12}},
    {ipRouteInfo,           ASN_OBJECT_ID,    RONLY,        ipRouteTable_get, 2, {1, 13}}
} ;

oid  ipRouteTableOid[]   = {ROUTE_OIDPREFIX , 21};
/*-----------------------------------------------------------------------------------------*/
FindVarMethod hhrRouteTable_get;
static struct ipran_snmp_data_cache *hhrRouteTable_cache = NULL ;
struct variable2 hhrRouteTable_Variables[] =
{
    {hhrRouteTableDestIP,           ASN_IPADDRESS,      NOACCESS,   hhrRouteTable_get, 2, {1, 1}},
    {hhrRouteTableDestMask,         ASN_IPADDRESS,      NOACCESS,   hhrRouteTable_get, 2, {1, 2}},
    {hhrRouteTableVPNId,            ASN_INTEGER,        NOACCESS,   hhrRouteTable_get, 2, {1, 3}},
    {hhrRouteTableNextHopAddress,   ASN_IPADDRESS,      NOACCESS,   hhrRouteTable_get, 2, {1, 4}},
    {hhrRouteTableCostValue,        ASN_INTEGER,        RONLY,      hhrRouteTable_get, 2, {1, 5}},
    {hhrRouteTablePreference,       ASN_INTEGER,        RONLY,      hhrRouteTable_get, 2, {1, 6}},
    {hhrRouteTableOutIfDesc,        ASN_OCTET_STR,      RONLY,      hhrRouteTable_get, 2, {1, 7}},
    {hhrRouteTableStatus,           ASN_INTEGER,        RONLY,      hhrRouteTable_get, 2, {1, 8}}
} ;

oid  hhrRouteTable_Oid[]   = {HHR_ROUTE_OIDPREFIX , 1};
/*-----------------------------------------------------------------------------------------*/
FindVarMethod hhrRouteStatisticsTable_get;
static struct ipran_snmp_data_cache *hhrRouteStatisticsTable_cache = NULL ;
struct variable2 hhrRouteStatisticsTable_Variables[] =
{
    {hhrRouteStatisticsVpnInstance,     ASN_INTEGER,       NOACCESS,    hhrRouteStatisticsTable_get, 2, {1, 1}},
    {hhrRouteStatisticsTotalDirect,     ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 2}},
    {hhrRouteStatisticsActiveDirect,    ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 3}},
    {hhrRouteStatisticsTotalStatic,     ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 4}},
    {hhrRouteStatisticsActiveStatic,    ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 5}},
    {hhrRouteStatisticsTotalOSPF,       ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 6}},
    {hhrRouteStatisticsActiveOSPF,      ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 7}},
    {hhrRouteStatisticsTotalISIS,       ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 8}},
    {hhrRouteStatisticsActiveISIS,      ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 9}},
    {hhrRouteStatisticsTotalRip,        ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 10}},
    {hhrRouteStatisticsActiveRip,       ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 11}},
    {hhrRouteStatisticsTotalIbgp,       ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 12}},
    {hhrRouteStatisticsActiveIbgp,      ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 13}},
    {hhrRouteStatisticsTotalEbgp,       ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 14}},
    {hhrRouteStatisticsActiveEbgp,      ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 15}},
    {hhrRouteStatisticsTotal,           ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 16}},
    {hhrRouteStatisticsActiveTotal,     ASN_UNSIGNED,      RONLY,       hhrRouteStatisticsTable_get, 2, {1, 17}}
} ;

oid  hhrRouteStatisticsTable_Oid[]   = {HHR_ROUTE_OIDPREFIX , 3};
/*-----------------------------------------------------------------------------------------*/
FindVarMethod hhrRouteActiveTable_get;
static struct ipran_snmp_data_cache *hhrRouteActiveTable_cache = NULL ;
struct variable2 hhrRouteActiveTable_Variables[] =
{
    {hhrRouteActiveTableDestIP,         ASN_IPADDRESS,      NOACCESS,   hhrRouteActiveTable_get, 2, {1, 1}},
    {hhrRouteActiveTableDestMask,       ASN_IPADDRESS,      NOACCESS,   hhrRouteActiveTable_get, 2, {1, 2}},
    {hhrRouteActiveTableVPNId,          ASN_INTEGER,        NOACCESS,   hhrRouteActiveTable_get, 2, {1, 3}},
    {hhrRouteActiveTableNextHopAddress, ASN_IPADDRESS,      NOACCESS,   hhrRouteActiveTable_get, 2, {1, 4}},
    {hhrRouteActiveTableProtocolType,   ASN_IPADDRESS,      NOACCESS,   hhrRouteActiveTable_get, 2, {1, 5}},
    {hhrRouteActiveTableCostValue,      ASN_INTEGER,        RONLY,      hhrRouteActiveTable_get, 2, {1, 6}},
    {hhrRouteActiveTablePreference,     ASN_INTEGER,        RONLY,      hhrRouteActiveTable_get, 2, {1, 7}},
    {hhrRouteActiveTableOutIfDesc,      ASN_OCTET_STR,      RONLY,      hhrRouteActiveTable_get, 2, {1, 8}},
} ;
oid  hhrRouteActiveTable_Oid[]   = {HHR_ROUTE_OIDPREFIX , 5};
/*-----------------------------------------------------------------------------------------*/
FindVarMethod hhrRouteGlobal_get;
struct variable1 hhrRouteGlobal_Variables[] =
{
    {hhrRouteEcmpEnable,                ASN_INTEGER,      RONLY,    hhrRouteGlobal_get, 1, {11}},
    {hhrRouteFrrEnable,                 ASN_INTEGER,      RONLY,    hhrRouteGlobal_get, 1, {13}},
} ;
oid  hhrRouteGlobal_Oid[]   = {HHR_L3_OIDPREFIX};
/*-----------------------------------------------------------------------------------------*/

static uchar        str_value[STRING_LEN] = {'\0'};
static int          int_value = 0;
static uint32_t     ip_value = 0;
static uint32_t     uint_value = 0;

void  init_mib_route(void)
{
    REGISTER_MIB("ipran/route", ipRouteTableVariables, variable2, ipRouteTableOid);
    REGISTER_MIB("ipran/hhrRouteTable", hhrRouteTable_Variables, variable2, hhrRouteTable_Oid);
    REGISTER_MIB("ipran/hhrRouteStatisticsTable", hhrRouteStatisticsTable_Variables,
                 variable2, hhrRouteStatisticsTable_Oid);
    REGISTER_MIB("ipran/hhrRouteActiveTable_Variables", hhrRouteActiveTable_Variables,
                 variable2, hhrRouteActiveTable_Oid);
    REGISTER_MIB("ipran/hhrRouteGlobal_Variables", hhrRouteGlobal_Variables,
                 variable1, hhrRouteGlobal_Oid);
}

/*-----------------------------------------------------------------------------------------*/
int ipran_route_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                  struct route_entry  *index)
{
    struct route_entry *pifm_info = NULL;
    int data_num = 0;
    int ret = 0;
    pifm_info = route_com_get_route_active_brief_bulk(index, cache->nullkey, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num || NULL == pifm_info)
    {
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pifm_info , sizeof(struct route_entry));
            pifm_info++;
        }

        return TRUE;
    }
}

struct route_entry *ipran_route_node_lookup(struct ipran_snmp_data_cache *cache ,
                                            int exact,
                                            const struct route_entry  *index_input)
{
    struct listnode  *node, *nnode;
    struct route_entry *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 1 == cache->nullkey)
        {
            return cache->data_list->head->data;
        }

        if (data1_find->prefix.addr.ipv4 == index_input->prefix.addr.ipv4)
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



u_char *ipRouteTable_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    int    iRetVal = 0, i;
    struct route_entry *ipRouteData = NULL ;
    struct route_entry index  ;
    struct in_addr temp_addr;

    memset(&index, 0 , sizeof(struct route_entry));

    if (NULL == ipRouteTable_cache)
    {
        ipRouteTable_cache = snmp_cache_init(sizeof(struct route_entry) ,
                                             ipran_route_get_data_from_ipc ,
                                             ipran_route_node_lookup) ;

        if (NULL == ipRouteTable_cache)
        {
            return (NULL);
        }
    }

    iRetVal = ipran_snmp_ip_index_get(vp, name, length, &index.prefix.addr.ipv4, exact);
    index.prefix.addr.ipv4 = ntohl(index.prefix.addr.ipv4) ;

    if (iRetVal < 0)
    {
        return NULL;
    }

    if (iRetVal == 1)
    {
        snmp_cache_nullkey_set(ipRouteTable_cache, 1);
    }
    else
    {
        snmp_cache_nullkey_set(ipRouteTable_cache, 0);
    }

    ipRouteData = snmp_cache_get_data_by_index(ipRouteTable_cache, exact, &index);

    if (NULL == ipRouteData)
    {
        return NULL ;
    }

    if (!exact)
    {
        ip_value = htonl(ipRouteData->prefix.addr.ipv4);
        ipran_snmp_ip_index_set(vp, name, length, ip_value);
    }

    switch (vp->magic)
    {
        case ipRouteIfIndex :
            uint_value = ipRouteData->nhp[0].nhp_index;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case ipRouteMetric1 :
            int_value = ipRouteData->nhp[0].distance;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ipRouteMetric2 :
            int_value = 0 ;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ipRouteMetric3 :
            int_value = 0;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ipRouteMetric4 :
            int_value = 0;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ipRouteNextHop :
            ip_value = ipRouteData->nhp[0].nexthop.addr.ipv4 ;
            ip_value = htonl(ip_value);
            *var_len = sizeof(uint32_t);
            return (u_char *)&ip_value;

        case ipRouteType :
            int_value = ipRouteData->nhp[0].nhp_type ;
            *var_len = sizeof(int);

            switch (int_value)
            {
                case NHP_TYPE_INVALID :
                    int_value = 2 ;
                    break ;

                case NHP_TYPE_HOST :
                    int_value = 3 ;
                    break ;

                case NHP_TYPE_IP :
                    int_value = 4;
                    break ;

                case NHP_TYPE_CONNECT :
                    int_value = 3;
                    break ;

                case NHP_TYPE_LSP :
                    int_value = 3;
                    break ;

                case NHP_TYPE_FRR :
                    int_value = 4;
                    break ;

                case NHP_TYPE_ECMP :
                    int_value = 4;
                    break ;

                case NHP_TYPE_TUNNEL :
                    int_value = 4;
                    break ;

                default :
                    int_value = 1;
                    break ;

            }

            return (u_char *)&int_value;

        case ipRouteProto :
            int_value = ipRouteData->nhp[0].protocol ;
            *var_len = sizeof(int);

            switch (int_value)
            {
                case ROUTE_PROTO_INVALID :
                    int_value = 1;
                    break ;

                case ROUTE_PROTO_STATIC :
                    int_value = 2;
                    break ;

                case ROUTE_PROTO_CONNECT :
                    int_value = 2;
                    break ;

                case ROUTE_PROTO_ARP :
                    int_value = 2;
                    break ;

                case ROUTE_PROTO_OSPF :
                    int_value = 13;
                    break ;

                case ROUTE_PROTO_OSPF6 :
                    int_value = 13;
                    break ;

                case ROUTE_PROTO_ISIS :
                    int_value = 9;
                    break ;

                case ROUTE_PROTO_ISIS6 :
                    int_value = 9;
                    break ;

                case ROUTE_PROTO_RIP :
                    int_value = 8;
                    break ;

                case ROUTE_PROTO_RIPNG :
                    int_value = 8;
                    break ;

                case ROUTE_PROTO_IBGP :
                    int_value = 14;
                    break ;

                case ROUTE_PROTO_EBGP :
                    int_value = 5;
                    break ;

                case ROUTE_PROTO_IBGP6 :
                    int_value = 14;
                    break ;

                case ROUTE_PROTO_EBGP6 :
                    int_value = 5;
                    break ;

                case ROUTE_PROTO_MPLSTP :
                    int_value = 1;
                    break ;

                case ROUTE_PROTO_RSVPTE :
                    int_value = 1;
                    break ;

                case ROUTE_PROTO_LDP :
                    int_value = 1;
                    break ;

                default :
                    return 1 ;

            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ipRouteAge :
            int_value = 0 ;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ipRouteMask :
            ip_value = ipRouteData->prefix.prefixlen;
            masklen_to_netip(ip_value, &temp_addr);
            ip_value = temp_addr.s_addr;
            *var_len = sizeof(u_int32_t);
            return (u_char *)&ip_value;

        case ipRouteMetric5 :
            int_value = 0 ;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case ipRouteInfo :
            int_value = 0 ;
            *var_len = 0;
            return (u_char *)&int_value;

        default :
            return NULL ;

    }
}

/*-----------------------------------------------------------------------------------------*/

int ipran_hhr_route_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                      struct route_rib_mib  *index)
{
    struct route_rib_mib *proute_rib_mib = NULL;
    int data_num = 0;
    int ret = 0;
    proute_rib_mib = route_com_get_route_rib_bulk(index, cache->nullkey, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': cache->nullkey [%d]\n", __FILE__, __LINE__, __func__ , cache->nullkey);

    if (0 == data_num || NULL == proute_rib_mib)
    {
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, proute_rib_mib , sizeof(struct route_rib_mib));
            proute_rib_mib++;
        }

        return TRUE;
    }
}

struct route_entry *ipran_hhr_route_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                int exact,
                                                const struct route_rib_mib  *index_input)
{
    struct listnode  *node, *nnode;
    struct route_rib_mib *data1_find;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 1 == cache->nullkey)
        {
            return cache->data_list->head->data;
        }

        if (data1_find->prefix.addr.ipv4 == index_input->prefix.addr.ipv4 && \
                data1_find->prefix.prefixlen == index_input->prefix.prefixlen && \
                data1_find->nhp.nexthop.addr.ipv4 == index_input->nhp.nexthop.addr.ipv4 && \
                data1_find->vpn == index_input->vpn && \
                data1_find->nhp.protocol == index_input->nhp.protocol
           )
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



u_char *hhrRouteTable_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    int    iRetVal = 0, i;
    struct route_rib_mib *ipRouteData = NULL ;
    struct route_rib_mib index  ;
    struct in_addr temp_addr;
    int ret = 0 ;

    memset(&index, 0 , sizeof(struct route_rib_mib));

    if (NULL == hhrRouteTable_cache)
    {
        hhrRouteTable_cache = snmp_cache_init(sizeof(struct route_rib_mib) ,
                                              ipran_hhr_route_get_data_from_ipc ,
                                              ipran_hhr_route_node_lookup) ;

        if (NULL == hhrRouteTable_cache)
        {
            return (NULL);
        }
    }

    iRetVal = ipran_snmp_ipx2_int_ip_int_index_get(vp, name, length,
                                                   &index.prefix.addr.ipv4, &index.prefix.prefixlen,
                                                   (int *)&index.vpn, &index.nhp.nexthop.addr.ipv4,
                                                   (int *)&index.nhp.protocol,
                                                   exact);

    index.prefix.addr.ipv4 = ntohl(index.prefix.addr.ipv4) ;
    index.prefix.prefixlen = ntohl(index.prefix.prefixlen) ;
    index.nhp.nexthop.addr.ipv4 = ntohl(index.nhp.nexthop.addr.ipv4) ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index.prefix.addr.ipv4 [%8x] index.prefix.prefixlen[%d] index.vpn[%d] index.nhp.nexthop.addr.ipv4 [%8x] protocol [%d]\n", __FILE__, __LINE__, __func__  ,
               index.prefix.addr.ipv4, index.prefix.prefixlen ,
               index.vpn , index.nhp.nexthop.addr.ipv4 , index.nhp.protocol);

    if (iRetVal < 0)
    {
        return NULL;
    }

    if (iRetVal == 1)
    {
        snmp_cache_nullkey_set(hhrRouteTable_cache, 1);
    }
    else if (iRetVal == 0)
    {
        snmp_cache_nullkey_set(hhrRouteTable_cache, 0);
    }

    ipRouteData = snmp_cache_get_data_by_index(hhrRouteTable_cache , exact, &index);

    if (NULL == ipRouteData)
    {
        return NULL ;
    }

    if (!exact)
    {
        ipran_snmp_ipx2_int_ip_int_index_set(vp, name, length, htonl(ipRouteData->prefix.addr.ipv4) , \
                                             htonl(ipRouteData->prefix.prefixlen) , ipRouteData->vpn , \
                                             htonl(ipRouteData->nhp.nexthop.addr.ipv4) , \
                                             ipRouteData->nhp.protocol);
    }

    switch (vp->magic)
    {
        case hhrRouteTableCostValue :
            uint_value = ipRouteData->nhp.cost ;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteTablePreference :
            int_value = ipRouteData->nhp.distance;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrRouteTableOutIfDesc :
            uint_value = ipRouteData->nhp.ifindex;
            memset(str_value, 0, STRING_LEN);
            ret = ifm_get_name_by_ifindex(uint_value, (char *) str_value);

            if (ret < 0)
            {
                *var_len = 0;
            }
            else
            {
                *var_len = strlen((char *) str_value);
            }

            return str_value;

        case hhrRouteTableStatus :
            int_value = ipRouteData->nhp.active;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return NULL ;

    }
}

/*-----------------------------------------------------------------------------------------*/

int ipran_hhrRouteStatisticsTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                                    struct route_count_mib  *index)
{
    struct route_count_mib *pifm_info = NULL;
    int data_num = 0;
    int ret = 0;

    pifm_info = route_com_get_route_count_bulk(index, cache->nullkey, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d] true_index [%d] cache->nullkey [%d]\n",
               __FILE__, __LINE__, __func__ , data_num , index->vpn_instance , cache->nullkey);

    if (0 == data_num || NULL == pifm_info)
    {
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pifm_info , sizeof(struct route_count_mib));
            pifm_info++;
        }

        return TRUE;
    }
}

struct route_count_mib *ipran_hhrRouteStatisticsTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                                  int exact,
                                                                  const struct route_count_mib  *index_input)
{
    struct listnode  *node, *nnode;
    struct route_count_mib *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || (0 == index_input->vpn_instance && cache->nullkey == 1))
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->vpn_instance == index_input->vpn_instance)
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

u_char *hhrRouteStatisticsTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    int    iRetVal = 0, i;
    struct route_count_mib *ipRouteData = NULL ;
    struct route_count_mib index  ;
    struct in_addr temp_addr;
    int protocol = 8 ;

    memset(&index, 0 , sizeof(struct route_count_mib));

    if (NULL == hhrRouteStatisticsTable_cache)
    {
        hhrRouteStatisticsTable_cache = snmp_cache_init(sizeof(struct route_count_mib) ,
                                                        ipran_hhrRouteStatisticsTable_get_data_from_ipc ,
                                                        ipran_hhrRouteStatisticsTable_node_lookup) ;

        if (NULL == hhrRouteStatisticsTable_cache)
        {
            return (NULL);
        }
    }

    iRetVal = ipran_snmp_int_index_get(vp, name, length, &index.vpn_instance , exact);

    if (iRetVal < 0)
    {
        return NULL;
    }

    if (iRetVal == 1)
    {
        snmp_cache_nullkey_set(hhrRouteStatisticsTable_cache , 1);
    }
    else if (iRetVal == 0)
    {
        snmp_cache_nullkey_set(hhrRouteStatisticsTable_cache , 0);
    }

    ipRouteData = snmp_cache_get_data_by_index(hhrRouteStatisticsTable_cache ,
                                               exact, &index);

    if (NULL == ipRouteData)
    {
        return NULL ;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, ipRouteData->vpn_instance);
    }

    switch (vp->magic)
    {
        case hhrRouteStatisticsTotalDirect :
            uint_value = ipRouteData->num_direct;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsActiveDirect :
            uint_value = ipRouteData->num_direct_active;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsTotalStatic :
            uint_value = ipRouteData->num_static;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsActiveStatic  :
            uint_value = ipRouteData->num_static_active;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsTotalOSPF :
            uint_value = ipRouteData->num_ospf;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsActiveOSPF  :
            uint_value = ipRouteData->num_ospf_active;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsTotalISIS :
            uint_value = ipRouteData->num_isis;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsActiveISIS  :
            uint_value = ipRouteData->num_isis_active;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsTotalRip :
            uint_value = ipRouteData->num_rip;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsActiveRip  :
            uint_value = ipRouteData->num_rip_active;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsTotalIbgp :
            uint_value = ipRouteData->num_ibgp;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsActiveIbgp  :
            uint_value = ipRouteData->num_ibgp_active;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsTotalEbgp :
            uint_value = ipRouteData->num_ebgp;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsActiveEbgp  :
            uint_value = ipRouteData->num_ebgp_active;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsTotal :
            uint_value = ipRouteData->total_num;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteStatisticsActiveTotal  :
            uint_value = ipRouteData->active_num;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        default :
            return NULL ;
    }
}

/*-----------------------------------------------------------------------------------------*/

int ipran_hhrRouteActiveTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                                struct route_entry  *index)
{
    struct route_entry *pifm_info = NULL;
    int data_num = 0;
    int ret = 0;
    pifm_info = route_com_get_route_active_brief_bulk(index, cache->nullkey, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    if (0 == data_num || NULL == pifm_info)
    {
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pifm_info , sizeof(struct route_entry));
            pifm_info++;
        }

        return TRUE;
    }
}

struct route_entry *ipran_hhrRouteActiveTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                          int exact,
                                                          const struct route_entry  *index_input)
{
    struct listnode  *node, *nnode;
    struct route_entry *data1_find;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 1 == cache->nullkey)
        {
            return cache->data_list->head->data;
        }

        if (data1_find->prefix.addr.ipv4 == index_input->prefix.addr.ipv4 && \
                data1_find->prefix.prefixlen == index_input->prefix.prefixlen && \
                data1_find->nhp[0].nexthop.addr.ipv4 == index_input->nhp[0].nexthop.addr.ipv4 && \
                data1_find->vpn == index_input->vpn && \
                data1_find->nhp[0].protocol == index_input->nhp[0].protocol
           )
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

u_char *hhrRouteActiveTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    int    iRetVal = 0, i;
    struct route_entry *ipRouteData = NULL ;
    struct route_entry index  ;
    struct in_addr temp_addr;
    int ret = 0 ;

    memset(&index, 0 , sizeof(struct route_rib_mib));

    if (NULL == hhrRouteActiveTable_cache)
    {
        hhrRouteActiveTable_cache = snmp_cache_init(sizeof(struct route_entry) ,
                                                    ipran_hhrRouteActiveTable_get_data_from_ipc ,
                                                    ipran_hhrRouteActiveTable_node_lookup) ;

        if (NULL == hhrRouteActiveTable_cache)
        {
            return (NULL);
        }
    }

    iRetVal = ipran_snmp_ipx2_int_ip_int_index_get(vp, name, length,
                                                   &index.prefix.addr.ipv4, &index.prefix.prefixlen,
                                                   (int *)&index.vpn, &index.nhp[0].nexthop.addr.ipv4,
                                                   (int *)&index.nhp[0].protocol,
                                                   exact);

    index.prefix.addr.ipv4 = ntohl(index.prefix.addr.ipv4) ;
    index.prefix.prefixlen = ntohl(index.prefix.prefixlen) ;
    index.nhp[0].nexthop.addr.ipv4 = ntohl(index.nhp[0].nexthop.addr.ipv4) ;

    if (iRetVal < 0)
    {
        return NULL;
    }

    if (iRetVal == 1)
    {
        snmp_cache_nullkey_set(hhrRouteActiveTable_cache , 1);
    }
    else if (iRetVal == 0)
    {
        snmp_cache_nullkey_set(hhrRouteActiveTable_cache , 0);
    }

    ipRouteData = snmp_cache_get_data_by_index(hhrRouteActiveTable_cache , exact, &index);

    if (NULL == ipRouteData)
    {
        return NULL ;
    }

    if (!exact)
    {
        ipran_snmp_ipx2_int_ip_int_index_set(vp, name, length, htonl(ipRouteData->prefix.addr.ipv4) , \
                                             htonl(ipRouteData->prefix.prefixlen) , ipRouteData->vpn , \
                                             htonl(ipRouteData->nhp[0].nexthop.addr.ipv4) , \
                                             ipRouteData->nhp[0].protocol);
    }

    switch (vp->magic)
    {
        case hhrRouteActiveTableCostValue :
            uint_value = ipRouteData->nhp[0].cost ;
            *var_len = sizeof(uint32_t);
            return (u_char *)&uint_value;

        case hhrRouteActiveTablePreference :
            int_value = ipRouteData->nhp[0].distance;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrRouteActiveTableOutIfDesc :
            uint_value = ipRouteData->nhp[0].ifindex;
            memset(str_value, 0, STRING_LEN);
            ret = ifm_get_name_by_ifindex(uint_value, (char *) str_value);

            if (ret < 0)
            {
                *var_len = 0;
            }
            else
            {
                *var_len = strlen((char *) str_value);
            }

            return str_value;

        default :
            return NULL ;

    }
}

u_char *hhrRouteGlobal_get(struct variable *vp,
                           oid *name,
                           size_t *length,
                           int exact, size_t *var_len, WriteMethod **write_method)
{
    struct route_global *route_global_data = NULL;
    struct route_global  data;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    route_global_data = route_com_get_route_gconf(MODULE_ID_SNMPD);

    if (NULL == route_global_data)
    {
        memset(&data, 0, sizeof(struct route_global));
    }
    else
    {
        memcpy(&data, route_global_data, sizeof(struct route_global));
    }

    switch (vp->magic)
    {
        case hhrRouteEcmpEnable:
            int_value = data.route_ecmp_flag;

            if (DISABLE == int_value)
            {
                int_value = 2;
            }
            else
            {
                int_value = 1;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrRouteFrrEnable:
            int_value = data.route_frr_flag;
            *var_len = sizeof(int);

            if (DISABLE == int_value)
            {
                int_value = 2;
            }
            else
            {
                int_value = 1;
            }

            return (u_char *)&int_value;

        default :
            return NULL ;
    }

}

