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
#include <time.h>
#include <stdlib.h>

#include "net-snmp-config.h"
#include "types.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"

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
#include <lib/devm_com.h>

#include <ripd/rip_msg.h>
#include <lib/if.h>
#include <isisd/isis_msg.h>
#include <ospfd/ospf_msg.h>
#include <bgpd/bgp_msg.h>
#include <lib/log.h>

#include "ipran_snmp_data_cache.h"
#include "mib_l3func.h"
#include <lib/msg_ipc_n.h>

#include "snmpd.h"
#include <route/route_if.h>
#include "snmp_config_table.h"


/*local temp variable*/
static uchar        str_value[STRING_LEN] = {'\0'};
static uchar        mac_value[6] = {0};
static int          int_value = 0;
static uint32_t     uint_value = 0;
static uint32_t     ip_value = 0;
#define BGP_INSTANCE_TABLE_EMPTY_TIME                   1

struct static_route_expand
{
    int search_flag;
    struct route_static route_data;
};
/*
 * Object ID definitions
 */
/*route common*/
static oid hhrLayer3Function_oid[] = {HHRLAVER3FUNCTION};
FindVarMethod hhrLayer3Function_get;
struct variable1 hhrLayer3Function_variables[] =
{
    {hhrRouteId, ASN_IPADDRESS, RONLY, hhrLayer3Function_get, 1, {1}}
};

/*static route*/
static struct ipran_snmp_data_cache *hhrStaticRouteTable_cache = NULL;
static oid hhrStaticRouteTable_oid[] = {HHRLAVER3FUNCTION, 2};
FindVarMethod hhrStaticRouteTable_get;
struct variable2 hhrStaticRouteTable_variables[] =
{
    {hhrSRBlackhole,       ASN_INTEGER,   RONLY, hhrStaticRouteTable_get, 2, {1, 5}},
    {hhrSRNextHopPortName, ASN_OCTET_STR, RONLY, hhrStaticRouteTable_get, 2, {1, 6}},
    {hhrSRMetric,          ASN_UNSIGNED,  RONLY, hhrStaticRouteTable_get, 2, {1, 7}}
};

/*rip*/
static struct ipran_snmp_data_cache *hhrRIPInstanceTable_cache = NULL;
static oid hhrRIPInstanceTable_oid[] = {HHRLAVER3FUNCTION, 3, 1};
FindVarMethod hhrRIPInstanceTable_get;
struct variable2 hhrRIPInstanceTable_variables[] =
{
    {hhrRIPVersion,         ASN_INTEGER,  RONLY, hhrRIPInstanceTable_get, 2, {1, 2}},
    {hhrRIPDistance,        ASN_UNSIGNED, RONLY, hhrRIPInstanceTable_get, 2, {1, 3}},
    {hhrRIPUpdate,          ASN_UNSIGNED, RONLY, hhrRIPInstanceTable_get, 2, {1, 4}},
    {hhrRIPAge,             ASN_UNSIGNED, RONLY, hhrRIPInstanceTable_get, 2, {1, 5}},
    {hhrRIPGarbageCollect,  ASN_UNSIGNED, RONLY, hhrRIPInstanceTable_get, 2, {1, 6}},
    {hhrRIPAutoSummary,     ASN_UNSIGNED, RONLY, hhrRIPInstanceTable_get, 2, {1, 7}}
};

#if 0
static oid hhrRIPInstanceNetworkTable_oid[] = {HHRLAVER3FUNCTION, 3, 2};
FindVarMethod hhrRIPInstanceNetworkTable_get;
struct variable2 hhrRIPInstanceNetworkTable_variables[] =
{
    {hhrRIPInstanceNetworkReservedField, ASN_INTEGER, RONLY, hhrRIPInstanceNetworkTable_get, 2, {1, 2}}
};
#endif

static struct ipran_snmp_data_cache *hhrRIPInstanceIfTable_cache = NULL;
static oid hhrRIPInstanceIfTable_oid[] = {HHRLAVER3FUNCTION, 3, 3};
FindVarMethod hhrRIPInstanceIfTable_get;
struct variable2 hhrRIPInstanceIfTable_variables[] =
{
    {hhrRIPInstanceIFDesc,      ASN_OCTET_STR, RONLY, hhrRIPInstanceIfTable_get, 2, {1, 1}},
    {hhrRIPIfRelatedInstanceID, ASN_UNSIGNED,  RONLY, hhrRIPInstanceIfTable_get, 2, {1, 2}},
    {hhrRIPIfAuthPassword,      ASN_OCTET_STR, RONLY, hhrRIPInstanceIfTable_get, 2, {1, 3}},
    {hhrRIPIfAuthMd5Id,         ASN_UNSIGNED,  RONLY, hhrRIPInstanceIfTable_get, 2, {1, 4}},
    {hhrRIPIfSplitHorizon,      ASN_UNSIGNED,  RONLY, hhrRIPInstanceIfTable_get, 2, {1, 5}},
    {hhrRIPIfPoisonReverse,     ASN_UNSIGNED,  RONLY, hhrRIPInstanceIfTable_get, 2, {1, 6}},
    {hhrRIPIfPassive,           ASN_UNSIGNED,  RONLY, hhrRIPInstanceIfTable_get, 2, {1, 7}},
    {hhrRIPIfMetric,            ASN_UNSIGNED,  RONLY, hhrRIPInstanceIfTable_get, 2, {1, 8}},
    {hhrRIPIfVersion,           ASN_UNSIGNED,  RONLY, hhrRIPInstanceIfTable_get, 2, {1, 9}},
    {hhrRIPIfV2VersionSendType, ASN_UNSIGNED,  RONLY, hhrRIPInstanceIfTable_get, 2, {1, 10}}
};

static struct ipran_snmp_data_cache *hhrRIPRedistributeTable_cache = NULL;
static oid hhrRIPRedistributeTable_oid[] = {HHRLAVER3FUNCTION, 3, 6};
FindVarMethod hhrRIPRedistributeTable_get;
struct variable2 hhrRIPRedistributeTable_variables[] =
{
    {hhrRIPRedistributeMetric,  ASN_UNSIGNED, RONLY, hhrRIPRedistributeTable_get, 2, {1, 3}},
};

static struct ipran_snmp_data_cache *hhrRIPIfRouteSummaryTable_cache = NULL;
static oid hhrRIPIfRouteSummaryTable_oid[] = {HHRLAVER3FUNCTION, 3, 8};
FindVarMethod hhrRIPIfRouteSummaryTable_get;
struct variable2 hhrRIPIfRouteSummaryTable_variables[] =
{
    {hhrRIPIfRouteSummaryIFDesc,    ASN_OCTET_STR, RONLY, hhrRIPIfRouteSummaryTable_get, 2, {1, 3}},
};

static struct ipran_snmp_data_cache *hhrRIPIfNeighborTable_cache = NULL;
static oid hhrRIPIfNeighborTable_oid[] = {HHRLAVER3FUNCTION, 3, 10};
FindVarMethod hhrRIPIfNeighborTable_get;
struct variable2 hhrRIPIfNeighborTable_variables[] =
{
    {hhrRIPIfNeighborIFDesc,    ASN_OCTET_STR, RONLY, hhrRIPIfNeighborTable_get, 2, {1, 2}},
};


/*ospf*/
static struct ipran_snmp_data_cache *hhrOSPFInstanceTable_cache = NULL;
static oid hhrOSPFInstanceTable_oid[] = {HHRLAVER3FUNCTION, 4, 10};
FindVarMethod hhrOSPFInstanceTable_get;
struct variable2 hhrOSPFInstanceTable_variables[] =
{
    {hhrOSPFInstanceRouterId,               ASN_IPADDRESS, RONLY, hhrOSPFInstanceTable_get, 2, {1, 2}},
    {hhrOSPFInstanceVPNId,                  ASN_UNSIGNED,  RONLY, hhrOSPFInstanceTable_get, 2, {1, 3}},
    {hhrOSPFInstanceBandwidth,              ASN_UNSIGNED,  RONLY, hhrOSPFInstanceTable_get, 2, {1, 4}},
    {hhrOSPFInstanceOpaqueLsa,              ASN_UNSIGNED,  RONLY, hhrOSPFInstanceTable_get, 2, {1, 5}},
    {hhrOSPFInstanceRfc1583,                ASN_UNSIGNED,  RONLY, hhrOSPFInstanceTable_get, 2, {1, 6}},
    {hhrOSPFInstanceDistance,               ASN_UNSIGNED,  RONLY, hhrOSPFInstanceTable_get, 2, {1, 7}},
    {hhrOSPFInstanceASExDistance,           ASN_UNSIGNED,  RONLY, hhrOSPFInstanceTable_get, 2, {1, 8}},
    {hhrOSPFInstanceLSDBRefreshInterval,    ASN_UNSIGNED,  RONLY, hhrOSPFInstanceTable_get, 2, {1, 9}}
};

static struct ipran_snmp_data_cache *hhrOSPFAreaTable_cache = NULL;
static oid hhrOSPFAreaTable_oid[] = {HHRLAVER3FUNCTION, 4, 11};
FindVarMethod hhrOSPFAreaTable_get;
struct variable2 hhrOSPFAreaTable_variables[] =
{
    {hhrOSPFAreaName,        ASN_OCTET_STR, RONLY, hhrOSPFAreaTable_get, 2, {1, 2}},
    {hhrOSPFAreaType,        ASN_INTEGER,   RONLY, hhrOSPFAreaTable_get, 2, {1, 3}},
    {hhrOSPFAreaNoSummary,   ASN_INTEGER,   RONLY, hhrOSPFAreaTable_get, 2, {1, 4}},
    {hhrOSPFAreaDefaultCost, ASN_UNSIGNED,  RONLY, hhrOSPFAreaTable_get, 2, {1, 5}}
};

static struct ipran_snmp_data_cache *hhrOSPFAreaNetworkTable_cache = NULL;
static oid hhrOSPFAreaNetworkTable_oid[] = {HHRLAVER3FUNCTION, 4, 12};
FindVarMethod hhrOSPFAreaNetworkTable_get;
struct variable2 hhrOSPFAreaNetworkTable_variables[] =
{
    {hhrOSPFAreaNetWorkReservedField, ASN_UNSIGNED, RONLY, hhrOSPFAreaNetworkTable_get, 2, {1, 3}}
};

static struct ipran_snmp_data_cache *hhrOSPFIfTable_cache = NULL;
static oid hhrOSPFIfTable_oid[] = {HHRLAVER3FUNCTION, 4, 13};
FindVarMethod hhrOSPFIfTable_get;
struct variable2 hhrOSPFIfTable_variables[] =
{
    {hhrOSPFIfDesc,                 ASN_OCTET_STR, RONLY, hhrOSPFIfTable_get, 2, {1, 1}},
    {hhrOSPFIfNetType,              ASN_INTEGER,   RONLY, hhrOSPFIfTable_get, 2, {1, 2}},
    {hhrOSPFIfAuthType,             ASN_INTEGER,   RONLY, hhrOSPFIfTable_get, 2, {1, 3}},
    {hhrOSPFIfMd5Id,                ASN_INTEGER,   RONLY, hhrOSPFIfTable_get, 2, {1, 4}},
    {hhrOSPFIfAuthPassword,         ASN_OCTET_STR, RONLY, hhrOSPFIfTable_get, 2, {1, 5}},
    {hhrOSPFIfCostValue,            ASN_INTEGER,   RONLY, hhrOSPFIfTable_get, 2, {1, 6}},
    {hhrOSPFIfDeadTimeInterval,     ASN_INTEGER,   RONLY, hhrOSPFIfTable_get, 2, {1, 7}},
    {hhrOSPFIfHelloTimeInterval,    ASN_INTEGER,   RONLY, hhrOSPFIfTable_get, 2, {1, 8}},
    {hhrOSPFIfMTUCheck,             ASN_INTEGER,   RONLY, hhrOSPFIfTable_get, 2, {1, 9}},
    {hhrOSPFIfDRPriority,           ASN_INTEGER,   RONLY, hhrOSPFIfTable_get, 2, {1, 10}},
    {hhrOSPFIfRetransmitInterval,   ASN_INTEGER,   RONLY, hhrOSPFIfTable_get, 2, {1, 11}}
};

static struct ipran_snmp_data_cache *hhrOSPFAbrSummaryTable_cache = NULL;
static oid hhrOSPFAbrSummaryTable_oid[] = {HHRLAVER3FUNCTION, 4, 16};
FindVarMethod hhrOSPFAbrSummaryTable_get;
struct variable2 hhrOSPFAbrSummaryTable_variables[] =
{
    {hhrOSPFAbrSummaryNotAdvertise,     ASN_INTEGER,   RONLY, hhrOSPFAbrSummaryTable_get, 2, {1, 3}},
    {hhrOSPFAbrSummaryCost,             ASN_INTEGER,   RONLY, hhrOSPFAbrSummaryTable_get, 2, {1, 4}}
};

static struct ipran_snmp_data_cache *hhrOSPFAsbrSummaryTable_cache = NULL;
static oid hhrOSPFAsbrSummaryTable_oid[] = {HHRLAVER3FUNCTION, 4, 18};
FindVarMethod hhrOSPFAsbrSummaryTable_get;
struct variable2 hhrOSPFAsbrSummaryTable_variables[] =
{
    {hhrOSPFAsbrSummaryNotAdvertise,    ASN_INTEGER,    RONLY, hhrOSPFAsbrSummaryTable_get, 2, {1, 3}},
    {hhrOSPFAsbrSummaryCost,            ASN_INTEGER,    RONLY, hhrOSPFAsbrSummaryTable_get, 2, {1, 4}}
};

static struct ipran_snmp_data_cache *hhrOSPFRedistributeTable_cache = NULL;
static oid hhrOSPFRedistributeTable_oid[] = {HHRLAVER3FUNCTION, 4, 20};
FindVarMethod hhrOSPFRedistributeTable_get;
struct variable2 hhrOSPFRedistributeTable_variables[] =
{
    {hhrOSPFRedistributeMetric,         ASN_INTEGER,   RONLY, hhrOSPFRedistributeTable_get, 2, {1, 3}},
    {hhrOSPFRedistributeMetricType,     ASN_INTEGER,   RONLY, hhrOSPFRedistributeTable_get, 2, {1, 4}}
};

static struct ipran_snmp_data_cache *hhrOSPFNeighborTable_cache = NULL;
static oid hhrOSPFNeighborTable_oid[] = {HHRLAVER3FUNCTION, 4, 22};
FindVarMethod hhrOSPFNeighborTable_get;
struct variable2 hhrOSPFNeighborTable_variables[] =
{
    {hhrOSPFNeighborPollInterval,    ASN_INTEGER,   RONLY, hhrOSPFNeighborTable_get, 2, {1, 2}}
};


/*arp*/
static oid hhrArp_oid[] = {HHRLAVER3FUNCTION, 5};
FindVarMethod hhrArp_get;
struct variable1 hhrArp_variables[] =
{
    {hhrArpAgeTime,              ASN_UNSIGNED, RONLY, hhrArp_get, 1, {1}},
    {hhrArpDynamicLearnNumLimit, ASN_UNSIGNED, RONLY, hhrArp_get, 1, {2}}
};

static struct ipran_snmp_data_cache *hhrIpNetToMediaTable_cache = NULL;
static oid hhrIpNetToMediaTable_oid[] = {HHRLAVER3FUNCTION, 5, 10};
FindVarMethod hhrIpNetToMediaTable_get;
struct variable2 hhrIpNetToMediaTable_variables[] =
{
    {hhrIpNetToMediaIfDesc,      ASN_OCTET_STR, RONLY, hhrIpNetToMediaTable_get, 2, {1, 3}},
    {hhrIpNetToMediaPhysAddress, ASN_OCTET_STR, RONLY, hhrIpNetToMediaTable_get, 2, {1, 4}},
    {hhrIpNetToMediaType,        ASN_INTEGER,   RONLY, hhrIpNetToMediaTable_get, 2, {1, 5}},
    {hhrIpNetToMediaVlanIf,      ASN_OCTET_STR, RONLY, hhrIpNetToMediaTable_get, 2, {1, 6}},
};

static struct ipran_snmp_data_cache *hhrIpPortArpConfigTable_cache = NULL;
static oid hhrIpPortArpConfigTable_oid[] = {HHRLAVER3FUNCTION, 5, 13};
FindVarMethod hhrIpPortArpConfigTable_get;
struct variable2 hhrIpPortArpConfigTable_variables[] =
{
    {hhrPortArpConfigIfindex,                   ASN_INTEGER,     NOACCESS, hhrIpPortArpConfigTable_get, 2, {1, 1}},
    {hhrPortArpConfigIfDescr,                   ASN_OCTET_STR, RONLY,        hhrIpPortArpConfigTable_get, 2, {1, 2}},
    {hhrPortArpConfigLearnEnable,       ASN_INTEGER,      RONLY,        hhrIpPortArpConfigTable_get, 2, {1, 3}},
    {hhrPortArpConfigLearnNumLimit, ASN_INTEGER,      RONLY,        hhrIpPortArpConfigTable_get, 2, {1, 4}},
    {hhrPortArpConfigProxyEnable,        ASN_INTEGER,      RONLY,        hhrIpPortArpConfigTable_get, 2, {1, 5}}
};

static oid arpstatistics_oid[] = {HHRLAVER3FUNCTION, 5, 15};
FindVarMethod arpstatistics_get;
struct variable1 arpstatistics_variables[] =
{
    {hhrArpStatisticsTotalNum,                    ASN_UNSIGNED, RONLY, arpstatistics_get, 1, {1}},
    {hhrArpStatisticsStaticArpNum,            ASN_UNSIGNED, RONLY, arpstatistics_get, 1, {2}},
    {hhrArpStatisticsCompleteArpNum,     ASN_UNSIGNED, RONLY, arpstatistics_get, 1, {3}},
    {hhrArpStatisticsIncompleteArpNum, ASN_UNSIGNED, RONLY, arpstatistics_get, 1, {4}},
    {hhrArpStatisticsAgedArpNum,             ASN_UNSIGNED, RONLY, arpstatistics_get, 1, {5}}
};

/*isis*/
static struct ipran_snmp_data_cache *hhrISISInstanceTable_cache = NULL;
static oid hhrISISInstanceTable_oid[] = {HHRLAVER3FUNCTION, 6, 1};
FindVarMethod hhrISISInstanceTable_get;
struct variable2 hhrISISInstanceTable_variables[] =
{
    {hhrISISInstanceLevel,      ASN_INTEGER,  RONLY, hhrISISInstanceTable_get, 2, {1,  2}},
    {hhrISISMetricStyle,        ASN_INTEGER,  RONLY, hhrISISInstanceTable_get, 2, {1,  3}},
    {hhrISISLspGenInterval,     ASN_UNSIGNED, RONLY, hhrISISInstanceTable_get, 2, {1,  4}},
    {hhrISISLspRefreshInterval, ASN_UNSIGNED, RONLY, hhrISISInstanceTable_get, 2, {1,  5}},
    {hhrISISLspLifetime,        ASN_UNSIGNED, RONLY, hhrISISInstanceTable_get, 2, {1,  6}},
    {hhrISISLspLength,          ASN_UNSIGNED, RONLY, hhrISISInstanceTable_get, 2, {1,  7}},
    {hhrISISSpfInterval,        ASN_UNSIGNED, RONLY, hhrISISInstanceTable_get, 2, {1,  8}},
    {hhrISISOverloadBit,        ASN_UNSIGNED, RONLY, hhrISISInstanceTable_get, 2, {1,  9}},
    {hhrISISAttachedBit,        ASN_UNSIGNED, RONLY, hhrISISInstanceTable_get, 2, {1, 10}},
    {hhrISISHelloPadding,       ASN_UNSIGNED, RONLY, hhrISISInstanceTable_get, 2, {1, 11}},
    {hhrISISDistanceValue,      ASN_UNSIGNED, RONLY, hhrISISInstanceTable_get, 2, {1, 12}},
    {hhrISISLevel1ToLevel2RoutePenetration,       ASN_UNSIGNED, RONLY, hhrISISInstanceTable_get, 2, {1, 13}},
    {hhrISISLevel2ToLevel1RoutePenetration,       ASN_UNSIGNED, RONLY, hhrISISInstanceTable_get, 2, {1, 14}}
};

static struct ipran_snmp_data_cache *hhrISISInstanceNetEntityTable_cache = NULL;
static oid hhrISISInstanceNetEntityTable_oid[] = {HHRLAVER3FUNCTION, 6, 2};
FindVarMethod hhrISISInstanceNetEntityTable_get;
struct variable2 hhrISISInstanceNetEntityTable_variables[] =
{
    {hhrISISInstanceNetEntityReservedField, ASN_UNSIGNED, RONLY, hhrISISInstanceNetEntityTable_get, 2, {1, 2}}
};

static struct ipran_snmp_data_cache *hhrISISInstanceIfTable_cache = NULL;
static oid hhrISISInstanceIfTable_oid[] = {HHRLAVER3FUNCTION, 6, 3};
FindVarMethod hhrISISInstanceIfTable_get;
struct variable2 hhrISISInstanceIfTable_variables[] =
{
    {hhrISISIfInstanceID,      ASN_UNSIGNED,  RONLY, hhrISISInstanceIfTable_get, 2, {1,  1}},
    {hhrISISIfDesc,            ASN_OCTET_STR, RONLY, hhrISISInstanceIfTable_get, 2, {1,  2}},
    {hhrISISIfPassive,         ASN_INTEGER,   RONLY, hhrISISInstanceIfTable_get, 2, {1,  3}},
    {hhrISISIfNetworkType,     ASN_INTEGER,   RONLY, hhrISISInstanceIfTable_get, 2, {1,  4}},
    {hhrISISIfCircuitType,     ASN_INTEGER,   RONLY, hhrISISInstanceIfTable_get, 2, {1,  5}},
    {hhrISISIfAuthType,        ASN_INTEGER,   RONLY, hhrISISInstanceIfTable_get, 2, {1,  6}},
    {hhrISISIfAuthPwd,         ASN_OCTET_STR, RONLY, hhrISISInstanceIfTable_get, 2, {1,  7}},
    {hhrISISIfPriority,        ASN_UNSIGNED,  RONLY, hhrISISInstanceIfTable_get, 2, {1,  8}},
    {hhrISISIfMetric,          ASN_UNSIGNED,  RONLY, hhrISISInstanceIfTable_get, 2, {1,  9}},
    {hhrISISIfHelloInterval,   ASN_UNSIGNED,  RONLY, hhrISISInstanceIfTable_get, 2, {1, 10}},
    {hhrISISIfHelloMultiplier, ASN_UNSIGNED,  RONLY, hhrISISInstanceIfTable_get, 2, {1, 11}},
    {hhrISISIfCsnpInterval,    ASN_UNSIGNED,  RONLY, hhrISISInstanceIfTable_get, 2, {1, 12}},
    {hhrISISIfPsnpInterval,    ASN_UNSIGNED,  RONLY, hhrISISInstanceIfTable_get, 2, {1, 13}},
};

/*bgp*/
static struct ipran_snmp_data_cache *hhrBGPInstanceIfTable_cache = NULL;
static oid hhrBGPInstanceIfTable_oid[] = {HHRLAVER3FUNCTION, 7, 1};
FindVarMethod hhrBGPInstanceIfTable_get;
struct variable2 hhrBGPInstanceIfTable_variables[] =
{
    {hhrBGPRouterId,                      ASN_IPADDRESS, RONLY, hhrBGPInstanceIfTable_get, 2, {1,  2}},
    {hhrBGPASPATH,                        ASN_INTEGER,   RONLY, hhrBGPInstanceIfTable_get, 2, {1,  3}},
    {hhrBGPKeepalive,                     ASN_UNSIGNED,  RONLY, hhrBGPInstanceIfTable_get, 2, {1,  4}},
    {hhrBGPHoldTime,                      ASN_UNSIGNED,  RONLY, hhrBGPInstanceIfTable_get, 2, {1,  5}},
    {hhrBGPRetryTime,                     ASN_UNSIGNED,  RONLY, hhrBGPInstanceIfTable_get, 2, {1,  6}},
    {hhrBGPLocalPreference,               ASN_UNSIGNED,  RONLY, hhrBGPInstanceIfTable_get, 2, {1,  7}},
    {hhrBGPDistanceEbgpPri ,              ASN_UNSIGNED,  RONLY, hhrBGPInstanceIfTable_get, 2, {1,  8}},
    {hhrBGPDistanceIbgpPri,               ASN_UNSIGNED,  RONLY, hhrBGPInstanceIfTable_get, 2, {1,  9}},
    {hhrBGPDistanceLocalPri,              ASN_UNSIGNED,  RONLY, hhrBGPInstanceIfTable_get, 2, {1, 10}},
    {hhrBGPDampeningEnable,               ASN_INTEGER,   RONLY, hhrBGPInstanceIfTable_get, 2, {1, 11}},
    {hhrBGPDampeningHalfLife,             ASN_UNSIGNED,  RONLY, hhrBGPInstanceIfTable_get, 2, {1, 12}},
    {hhrBGPDampeningReusethreshold,       ASN_UNSIGNED,  RONLY, hhrBGPInstanceIfTable_get, 2, {1, 13}},
    {hhrBGPDampeningSuppressionThreshold, ASN_UNSIGNED,  RONLY, hhrBGPInstanceIfTable_get, 2, {1, 14}},
    {hhrBGPDampeningMaxSuppressionTime,   ASN_UNSIGNED,  RONLY, hhrBGPInstanceIfTable_get, 2, {1, 15}},
    {hhrBGPMEDCompareDifferentAs,         ASN_INTEGER,   RONLY, hhrBGPInstanceIfTable_get, 2, {1, 16}},
    {hhrBGPMEDCompareDeterministic,       ASN_INTEGER,   RONLY, hhrBGPInstanceIfTable_get, 2, {1, 17}},
    {hhrBGPMED,                           ASN_UNSIGNED,  RONLY, hhrBGPInstanceIfTable_get, 2, {1, 18}}
};

static struct ipran_snmp_data_cache *hhrBGPRedistributeTable_cache = NULL;
static oid hhrBGPRedistributeTable_oid[] = {HHRLAVER3FUNCTION, 7, 2};
FindVarMethod hhrBGPRedistributeTable_get;
struct variable2 hhrBGPRedistributeTable_variables[] =
{
    {hhrBGPRedistributeMetric, ASN_UNSIGNED, RONLY, hhrBGPRedistributeTable_get, 2, {1, 4}}
};

static struct ipran_snmp_data_cache *hhrBGPNeighborTable_cache = NULL;
static oid hhrBGPNeighborTable_oid[] = {HHRLAVER3FUNCTION, 7, 3};
FindVarMethod hhrBGPNeighborTable_get;
struct variable2 hhrBGPNeighborTable_variables[] =
{
    {hhrBGPNeighborRemoteAS,                ASN_UNSIGNED,  RONLY, hhrBGPNeighborTable_get, 2, {1,  2}},
    {hhrBGPNeighborRemoteDesc,              ASN_OCTET_STR, RONLY, hhrBGPNeighborTable_get, 2, {1,  3}},
    {hhrBGPNeighborEnable,                  ASN_INTEGER,   RONLY, hhrBGPNeighborTable_get, 2, {1,  4}},
    {hhrBGPNeighborSourceIP,                ASN_IPADDRESS, RONLY, hhrBGPNeighborTable_get, 2, {1,  5}},
    {hhrBGPNeighborEbgpMultihop,            ASN_INTEGER,   RONLY, hhrBGPNeighborTable_get, 2, {1,  6}},
    {hhrBGPNeighborEbgpMultihopTTL,         ASN_UNSIGNED,  RONLY, hhrBGPNeighborTable_get, 2, {1,  7}},
    {hhrBGPNeighborEbgpLocalAS,             ASN_UNSIGNED,  RONLY, hhrBGPNeighborTable_get, 2, {1,  8}},
    {hhrBGPNeighborRouteSendDefault,        ASN_INTEGER,   RONLY, hhrBGPNeighborTable_get, 2, {1,  9}},
    {hhrBGPNeighborRouteSendIBGP,           ASN_INTEGER,   RONLY, hhrBGPNeighborTable_get, 2, {1, 10}},
    {hhrBGPNeighborRouteLimit,              ASN_UNSIGNED,  RONLY, hhrBGPNeighborTable_get, 2, {1, 11}},
    {hhrBGPNeighborRouteWeight,             ASN_UNSIGNED,  RONLY, hhrBGPNeighborTable_get, 2, {1, 12}},
    {hhrBGPNeighborRouteNextHopChangeLocal, ASN_INTEGER,   RONLY, hhrBGPNeighborTable_get, 2, {1, 13}},
    {hhrBGPNeighborAdvertiseInterval,       ASN_UNSIGNED,  RONLY, hhrBGPNeighborTable_get, 2, {1, 14}},
    {hhrBGPNeighborAsPathAllowAsLoop,       ASN_UNSIGNED,  RONLY, hhrBGPNeighborTable_get, 2, {1, 15}},
    {hhrBGPNeighborAsPathExcludePrivateAs,  ASN_INTEGER,   RONLY, hhrBGPNeighborTable_get, 2, {1, 16}},
    {hhrBGPNeighborIpv4FamilyVpnv4,         ASN_INTEGER,   RONLY, hhrBGPNeighborTable_get, 2, {1, 17}},
    {hhrBGPNeighborIpv4FamilyVpn,           ASN_UNSIGNED,  RONLY, hhrBGPNeighborTable_get, 2, {1, 18}},
    {hhrBGPNeighborBindingTunnelIfIndex,    ASN_UNSIGNED,  RONLY, hhrBGPNeighborTable_get, 2, {1, 19}},
    {hhrBGPNeighborBindingTunnelIfDescr,    ASN_OCTET_STR, RONLY, hhrBGPNeighborTable_get, 2, {1, 20}},
    {hhrBGPNeighborStatus,                  ASN_INTEGER,   RONLY, hhrBGPNeighborTable_get, 2, {1, 21}},
    {hhrBGPNeighborMd5Password,             ASN_OCTET_STR, RONLY, hhrBGPNeighborTable_get, 2, {1, 22}}
};

static struct ipran_snmp_data_cache *hhrBGPPublishTable_cache = NULL;
static oid hhrBGPPublishTable_oid[] = {HHRLAVER3FUNCTION, 7, 6};
FindVarMethod hhrBGPPublishTable_get;
struct variable2 hhrBGPPublishTable_variables[] =
{
    {hhrBGPPublishRoutesReservedField, ASN_UNSIGNED, RONLY, hhrBGPPublishTable_get, 2, {1, 3}}
};

static struct ipran_snmp_data_cache *hhrBGPSummaryTable_cache = NULL;
static oid hhrBGPSummaryTable_oid[] = {HHRLAVER3FUNCTION, 7, 8};
FindVarMethod hhrBGPSummaryTable_get;
struct variable2 hhrBGPSummaryTable_variables[] =
{
    {hhrBGPSummaryasSet, ASN_INTEGER, RONLY, hhrBGPSummaryTable_get, 2, {1, 3}},
    {hhrBGPSummarySummaryOnly, ASN_INTEGER, RONLY, hhrBGPSummaryTable_get, 2, {1, 4}}

};

static struct ipran_snmp_data_cache *hhrBGPRouteTable_cache = NULL;
static oid hhrBGPRouteTable_oid[] = {HHRLAVER3FUNCTION, 7, 10};
FindVarMethod hhrBGPRouteTable_get;
struct variable2 hhrBGPRouteTable_variables[] =
{
    {hhrBGPRouteTableMetricValue, ASN_UNSIGNED, RONLY, hhrBGPRouteTable_get, 2, {1, 5}},
    {hhrBGPRouteTablePreference, ASN_INTEGER, RONLY, hhrBGPRouteTable_get, 2, {1, 6}},
    {hhrBGPRouteTableMed, ASN_UNSIGNED, RONLY, hhrBGPRouteTable_get, 2, {1, 7}}
};


/*ospf dcn*/
static struct ipran_snmp_data_cache *hhrOSPFDCNConfigTable_cache = NULL;
static oid hhrOSPFDCNConfigTable_oid[] = {HHRLAVER3FUNCTION, 8, 1};
FindVarMethod hhrOSPFDCNConfigTable_get;
struct variable1 hhrOSPFDCNConfigTable_variables[] =
{
    {hhrOSPFDCNEnable,     ASN_INTEGER,   RONLY, hhrOSPFDCNConfigTable_get, 1, {1}},
    {hhrOSPFDCNTrapReport, ASN_INTEGER,   RONLY, hhrOSPFDCNConfigTable_get, 1, {2}},
    {hhrOSPFDCNGNeId,      ASN_UNSIGNED,  RONLY, hhrOSPFDCNConfigTable_get, 1, {3}},
    {hhrOSPFDCNGNeIP,      ASN_IPADDRESS, RONLY, hhrOSPFDCNConfigTable_get, 1, {4}},
    {hhrOSPFDCNGNeIPMask,  ASN_INTEGER,   RONLY, hhrOSPFDCNConfigTable_get, 1, {5}}

};

static struct ipran_snmp_data_cache *hhrOSPFDCNNeInfoTable_cache = NULL;
static oid hhrOSPFDCNNeInfoTable_oid[] = {HHRLAVER3FUNCTION, 8, 2};
FindVarMethod hhrOSPFDCNNeInfoTable_get;
struct variable2 hhrOSPFDCNNeInfoTable_variables[] =
{
    {hhrOSPFDCNNeIP,       ASN_IPADDRESS, RONLY, hhrOSPFDCNNeInfoTable_get, 2, {1,  2}},
    {hhrOSPFDCNMetric,     ASN_INTEGER,   RONLY, hhrOSPFDCNNeInfoTable_get, 2, {1,  3}},
    {hhrOSPFDCNDeviceType, ASN_OCTET_STR, RONLY, hhrOSPFDCNNeInfoTable_get, 2, {1,  4}},
    {hhrOSPFDCNNeMac,      ASN_OCTET_STR, RONLY, hhrOSPFDCNNeInfoTable_get, 2, {1,  5}},
    {hhrOSPFDCNVendor,     ASN_OCTET_STR, RONLY, hhrOSPFDCNNeInfoTable_get, 2, {1,  6}}
};

/*ospf dcn adapt h3c mib*/
static struct ipran_snmp_data_cache *hhIpRanDcnConfigTable_cache = NULL;
static oid hhIpRanDcnConfigTable_oid[] = {HHRLAVER3FUNCTION_DCN_H3C, 1, 1};
FindVarMethod hhIpRanDcnConfigTable_get;
struct variable1 hhIpRanDcnConfigTable_variables[] =
{
    {hhIpRanDcnNeId,        ASN_OCTET_STR,  RONLY, hhIpRanDcnConfigTable_get, 1, {1}},
    {hhIpRanDcnNeIpType,    ASN_INTEGER,    RONLY, hhIpRanDcnConfigTable_get, 1, {2}},
    {hhIpRanDcnNeIp,        ASN_IPADDRESS,  RONLY, hhIpRanDcnConfigTable_get, 1, {3}},
    {hhIpRanDcnMask,        ASN_IPADDRESS,  RONLY, hhIpRanDcnConfigTable_get, 1, {4}},
    {hhIpRanDcnMAC,         ASN_OCTET_STR,  RONLY, hhIpRanDcnConfigTable_get, 1, {5}},
    {hhIpRanDcnVendor,      ASN_OCTET_STR,  RONLY, hhIpRanDcnConfigTable_get, 1, {6}}
};

static struct ipran_snmp_data_cache *hhIpRanDcnNeInfoTable_cache = NULL;
static oid hhIpRanDcnNeInfoTable_oid[] = {HHRLAVER3FUNCTION_DCN_H3C, 1, 2};
FindVarMethod hhIpRanDcnNeInfoTable_get;
struct variable2 hhIpRanDcnNeInfoTable_variables[] =
{
    {hhIpRanDcnNeInfoNeIpType,      ASN_INTEGER,    RONLY, hhIpRanDcnNeInfoTable_get, 2, {1,  2}},
    {hhIpRanDcnNeInfoNeIp,          ASN_IPADDRESS,  RONLY, hhIpRanDcnNeInfoTable_get, 2, {1,  3}},
    {hhIpRanDcnNeInfoMetric,        ASN_INTEGER,    RONLY, hhIpRanDcnNeInfoTable_get, 2, {1,  4}},
    {hhIpRanDcnNeInfoDeviceType,    ASN_OCTET_STR,  RONLY, hhIpRanDcnNeInfoTable_get, 2, {1,  5}},
    {hhIpRanDcnNeInfoMAC,           ASN_OCTET_STR,  RONLY, hhIpRanDcnNeInfoTable_get, 2, {1,  6}},
    {hhIpRanDcnNeInfoVendor,        ASN_OCTET_STR,  RONLY, hhIpRanDcnNeInfoTable_get, 2, {1,  7}}
};


void init_mib_l3func(void)
{
    /*common route id*/
    REGISTER_MIB("hhrLayer3Function", hhrLayer3Function_variables
                 , variable1, hhrLayer3Function_oid);

    /*static route*/
    REGISTER_MIB("hhrStaticRouteTable", hhrStaticRouteTable_variables
                 , variable2, hhrStaticRouteTable_oid);

    /*ospf*/
    REGISTER_MIB("hhrOSPFInstanceTable", hhrOSPFInstanceTable_variables
                 , variable2, hhrOSPFInstanceTable_oid);
    REGISTER_MIB("hhrOSPFAreaTable", hhrOSPFAreaTable_variables
                 , variable2, hhrOSPFAreaTable_oid);
    REGISTER_MIB("hhrOSPFAreaNetworkTable", hhrOSPFAreaNetworkTable_variables
                 , variable2, hhrOSPFAreaNetworkTable_oid);
    REGISTER_MIB("hhrOSPFIfTable", hhrOSPFIfTable_variables
                 , variable2, hhrOSPFIfTable_oid);
    REGISTER_MIB("hhrOSPFAbrSummaryTable", hhrOSPFAbrSummaryTable_variables
                 , variable2, hhrOSPFAbrSummaryTable_oid);
    REGISTER_MIB("hhrOSPFAsbrSummaryTable", hhrOSPFAsbrSummaryTable_variables
                 , variable2, hhrOSPFAsbrSummaryTable_oid);
    REGISTER_MIB("hhrOSPFRedistributeTable", hhrOSPFRedistributeTable_variables
                 , variable2, hhrOSPFRedistributeTable_oid);
    REGISTER_MIB("hhrOSPFNeighborTable", hhrOSPFNeighborTable_variables
                 , variable2, hhrOSPFNeighborTable_oid);

    /*arp*/
    REGISTER_MIB("hhrArp", hhrArp_variables
                 , variable1, hhrArp_oid);
    REGISTER_MIB("hhrIpNetToMediaTable", hhrIpNetToMediaTable_variables
                 , variable2, hhrIpNetToMediaTable_oid);

    REGISTER_MIB("arpstatistics", arpstatistics_variables
                 , variable1, arpstatistics_oid);
    REGISTER_MIB("hhrIpPortArpConfigTable", hhrIpPortArpConfigTable_variables
                 , variable2, hhrIpPortArpConfigTable_oid);

    /*ospf dcn*/
    REGISTER_MIB("hhrOSPFDCNConfigTable", hhrOSPFDCNConfigTable_variables
                 , variable1, hhrOSPFDCNConfigTable_oid);
    REGISTER_MIB("hhrOSPFDCNNeInfoTable", hhrOSPFDCNNeInfoTable_variables
                 , variable2, hhrOSPFDCNNeInfoTable_oid);

    /*ospf dcn adpate h3c*/
    REGISTER_MIB("hhIpRanDcnConfigTable", hhIpRanDcnConfigTable_variables
                 , variable1, hhIpRanDcnConfigTable_oid);
    REGISTER_MIB("hhIpRanDcnNeInfoTable", hhIpRanDcnNeInfoTable_variables
                 , variable2, hhIpRanDcnNeInfoTable_oid);

    if (gDevTypeID != ID_HT157  && gDevTypeID != ID_HT158 && gDevTypeID != ID_HT153)
    {
        /*rip*/
        REGISTER_MIB("hhrRIPInstanceTable", hhrRIPInstanceTable_variables
                     , variable2, hhrRIPInstanceTable_oid);
        /*
            REGISTER_MIB ("hhrRIPInstanceNetworkTable", hhrRIPInstanceNetworkTable_variables
                                , variable2, hhrRIPInstanceNetworkTable_oid);
        */
        REGISTER_MIB("hhrRIPInstanceIfTable", hhrRIPInstanceIfTable_variables
                     , variable2, hhrRIPInstanceIfTable_oid);

        REGISTER_MIB("hhrRIPRedistributeTable", hhrRIPRedistributeTable_variables
                     , variable2, hhrRIPRedistributeTable_oid);

        REGISTER_MIB("hhrRIPIfRouteSummaryTable", hhrRIPIfRouteSummaryTable_variables
                     , variable2, hhrRIPIfRouteSummaryTable_oid);

        REGISTER_MIB("hhrRIPIfNeighborTable", hhrRIPIfNeighborTable_variables
                     , variable2, hhrRIPIfNeighborTable_oid);

        /*isis*/
        REGISTER_MIB("hhrISISInstanceTable", hhrISISInstanceTable_variables
                     , variable2, hhrISISInstanceTable_oid);
        REGISTER_MIB("hhrISISInstanceNetEntityTable", hhrISISInstanceNetEntityTable_variables
                     , variable2, hhrISISInstanceNetEntityTable_oid);
        REGISTER_MIB("hhrISISInstanceIfTable", hhrISISInstanceIfTable_variables
                     , variable2, hhrISISInstanceIfTable_oid);

        /*bgp*/
        REGISTER_MIB("hhrBGPInstanceIfTable", hhrBGPInstanceIfTable_variables
                     , variable2, hhrBGPInstanceIfTable_oid);
        REGISTER_MIB("hhrBGPRedistributeTable", hhrBGPRedistributeTable_variables
                     , variable2, hhrBGPRedistributeTable_oid);
        REGISTER_MIB("hhrBGPNeighborTable", hhrBGPNeighborTable_variables
                     , variable2, hhrBGPNeighborTable_oid);

        REGISTER_MIB("hhrBGPPublishTable", hhrBGPPublishTable_variables
                     , variable2, hhrBGPPublishTable_oid);
        REGISTER_MIB("hhrBGPSummaryTable", hhrBGPSummaryTable_variables
                     , variable2, hhrBGPSummaryTable_oid);
        REGISTER_MIB("hhrBGPRouteTable", hhrBGPRouteTable_variables
                     , variable2, hhrBGPRouteTable_oid);
    }
}

/*** hhrLayer3Function start **************************************************/
u_char *hhrLayer3Function_get(struct variable *vp,
                              oid *name,
                              size_t *length,
                              int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t  *route_id;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrRouteId:
            *var_len = sizeof(uint32_t);
            route_id  = route_com_get_router_id(MODULE_ID_SNMPD);

            if (NULL != route_id)
            {
                memcpy(&ip_value, route_id, sizeof(uint32_t));
                ip_value = htonl(ip_value);
                mem_share_free_bydata(route_id, MODULE_ID_SNMPD);
            }
            else
            {
                ip_value = 0;
            }

            return (u_char *)(&ip_value);

        default :
            return NULL;
    }

    return NULL;
}

/*** hhrStaticRouteTable start ************************************************/
int hhrStaticRouteTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                          struct static_route_expand *index_input)
{
    struct route_static *pdata = NULL;
    struct route_static *pfree = NULL;
    struct static_route_expand pdata_expand;
    int data_len = sizeof(struct static_route_expand);
    int data_num = 0;
    int i;

    pdata = route_com_get_sroute_bulk(&(index_input->route_data), index_input->search_flag, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num || NULL == pdata)
    {
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            memset(&pdata_expand, 0, sizeof(struct static_route_expand));
            pdata_expand.search_flag = 1;
            memcpy(&(pdata_expand.route_data), pdata, sizeof(struct route_static));
            snmp_cache_add(cache, &pdata_expand, data_len);
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct static_route_expand *hhrStaticRouteTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                            int exact,
                                                            const struct static_route_expand *index_input)
{
    struct listnode            *node;
    struct static_route_expand *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->search_flag)
        {
            return cache->data_list->head->data;
        }

        if ((pdata->route_data.prefix.addr.ipv4 == index_input->route_data.prefix.addr.ipv4) \
                && (pdata->route_data.prefix.prefixlen == index_input->route_data.prefix.prefixlen) \
                && (pdata->route_data.vpn == index_input->route_data.vpn) \
                && (pdata->route_data.nexthop.addr.ipv4 == index_input->route_data.nexthop.addr.ipv4))
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrStaticRouteTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    struct static_route_expand  index;
    struct static_route_expand *pdata;
    int                         data_len = sizeof(struct static_route_expand);
    int                         ret = 0;
    u_int32_t                   sRPrefixOfDestIpAddress = 0;
    u_int32_t                   sRNetworkMask = 0;
    u_int32_t                   sRVpn = 0;
    u_int32_t                   sRNextHopAddress = 0;
    int                         lookup_flag = 1;

    *write_method = NULL;

    /* validate the index */
    ret = ipran_snmp_ip_intx2_ip_index_get(vp, name, length, &sRPrefixOfDestIpAddress, \
                                           &sRNetworkMask, \
                                           &sRVpn, \
                                           &sRNextHopAddress, \
                                           exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrStaticRouteTable_cache)
    {
        hhrStaticRouteTable_cache = snmp_cache_init(data_len,
                                                    hhrStaticRouteTable_get_data_from_ipc,
                                                    hhrStaticRouteTable_node_lookup);

        if (NULL == hhrStaticRouteTable_cache)
        {
            return NULL;
        }
    }

    lookup_flag = (1 == ret) ? 0 : 1;

    memset(&index, 0, data_len);
    index.search_flag = lookup_flag;
    index.route_data.prefix.addr.ipv4  = ntohl(sRPrefixOfDestIpAddress);
    index.route_data.prefix.prefixlen  = sRNetworkMask;
    index.route_data.vpn               = sRVpn;
    index.route_data.nexthop.addr.ipv4 = ntohl(sRNextHopAddress);
    pdata = snmp_cache_get_data_by_index(hhrStaticRouteTable_cache, exact, &index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_ip_intx2_ip_index_set(vp, name, length, htonl(pdata->route_data.prefix.addr.ipv4), \
                                         (u_int32_t)pdata->route_data.prefix.prefixlen, \
                                         (u_int32_t)pdata->route_data.vpn, \
                                         htonl(pdata->route_data.nexthop.addr.ipv4));
    }

    switch (vp->magic)
    {
        case hhrSRBlackhole:
            *var_len = sizeof(int);
            int_value = (1 == pdata->route_data.action) ? 1 : 2 ;
            return (u_char *)&int_value;

        case hhrSRNextHopPortName:
            memset(str_value, '\0', STRING_LEN);

            if (-1 == ifm_get_name_by_ifindex(pdata->route_data.ifindex, str_value))
            {
                *var_len = 0;
                return str_value;
            }

            *var_len = strlen(str_value);
            return str_value;

        case hhrSRMetric:
            uint_value = pdata->route_data.distance;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        default:
            return NULL;
    }

    return NULL;
}

static struct ipc_mesg_n *hhrRIP_ALL_Table_get_bulk(void *pIndex, int subtype)
{
    int index_len = 0;

    switch (subtype)
    {
        case RIP_SNMP_INSTANCE_GET:
            index_len = sizeof(uint32_t);
            break;

        case RIP_SNMP_INTERFACE_GET:
            index_len = sizeof(uint32_t);
            break;

        case RIP_SNMP_REDIS_ROUTE_GET:
            index_len = sizeof(struct rip_redistribute_route_key);
            break;
		
		case RIP_SNMP_ROUTE_SUMMARY_GET:
            index_len = sizeof(struct rip_route_summary_key);
            break;
		
		case RIP_SNMP_IF_NBR_GET:
            index_len = sizeof(struct rip_if_nbr_key);
            break;

        default:
            break;
    }

	return ipc_sync_send_n2((void *)pIndex, index_len, 1, MODULE_ID_RIP, MODULE_ID_SNMPD, \
							IPC_TYPE_RIP, subtype, IPC_OPCODE_GET_BULK, 0, 1000);
}

/*** hhrRIPInstanceTable start ************************************************/
#if 0
struct rip_brief *rip_com_get_instance_bulk(uint32_t id, int module_id, int *pdata_num)
{
    uint32_t id_temp = id;
    /*struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(&id_temp, sizeof(uint32_t), 1 , MODULE_ID_RIP, module_id,
                                                         IPC_TYPE_RIP, 0, IPC_OPCODE_GET_BULK, 0);*/

	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&id_temp, sizeof(uint32_t), 1 , MODULE_ID_RIP, module_id,
                                                         IPC_TYPE_RIP, 0, IPC_OPCODE_GET_BULK, 0, 2000);
	

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct rip_brief *)pmesg->msg_data;
    }

    return NULL;
}
#endif

int hhrRIPInstanceTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                          struct rip_brief *index_input)
{
    struct rip_brief *pdata = NULL;
    int data_len = sizeof(struct rip_brief);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrRIP_ALL_Table_get_bulk(&(index_input->id), RIP_SNMP_INSTANCE_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct rip_brief *)pMsgRcv->msg_data;
    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }

		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct rip_brief *hhrRIPInstanceTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                  int exact,
                                                  const struct rip_brief *index_input)
{
    struct listnode    *node;
    struct rip_brief   *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->id)
        {
            return cache->data_list->head->data;
        }

        if (pdata->id == index_input->id)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrRIPInstanceTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    struct rip_brief    data_index;
    struct rip_brief    *pdata;
    int                 data_len = sizeof(struct rip_brief);
    u_int32_t           index_int = 0;
    int                 ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrRIPInstanceTable_cache)
    {
        hhrRIPInstanceTable_cache = snmp_cache_init(data_len,
                                                    hhrRIPInstanceTable_get_data_from_ipc,
                                                    hhrRIPInstanceTable_node_lookup);

        if (NULL == hhrRIPInstanceTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.id = index_int;
    pdata = snmp_cache_get_data_by_index(hhrRIPInstanceTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->id);
    }

    switch (vp->magic)
    {
        case hhrRIPVersion:
            int_value = pdata->version;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrRIPDistance:
            uint_value = pdata->distance;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrRIPUpdate:
            uint_value = pdata->update_time;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrRIPAge:
            uint_value = pdata->timeout_time;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrRIPGarbageCollect:
            uint_value = pdata->garbage_time;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrRIPAutoSummary:
            uint_value = pdata->auto_summary;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        default:
            return NULL;
    }

    return NULL;
}

#if 0
/*** hhrRIPInstanceNetworkTable start *****************************************/
u_char *hhrRIPInstanceNetworkTable_get(struct variable *vp,
                                       oid *name,
                                       size_t *length,
                                       int exact, size_t *var_len, WriteMethod **write_method)
{
    return NULL;
}
#endif

/*** hhrRIPInstanceIfTable start **********************************************/
#if 0
struct ripinter_brief *rip_com_get_interface_bulk(uint32_t ifindex, int module_id, int *pdata_num)
{
    uint32_t ifindex_temp = ifindex;
    /*struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(&ifindex_temp, sizeof(uint32_t), 1 , MODULE_ID_RIP, module_id,
                                                         IPC_TYPE_RIP, 1, IPC_OPCODE_GET_BULK, 0);*/

	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&ifindex_temp, sizeof(uint32_t), 1 , MODULE_ID_RIP, module_id,
                                                         IPC_TYPE_RIP, 1, IPC_OPCODE_GET_BULK, 0, 2000);
	

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct ripinter_brief *)pmesg->msg_data;
    }

    return NULL;
}
#endif

int hhrRIPInstanceIfTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                            struct ripinter_brief *index_input)
{
    struct ripinter_brief *pdata = NULL;
    int data_len = sizeof(struct ripinter_brief);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrRIP_ALL_Table_get_bulk(&(index_input->ifindex), RIP_SNMP_INTERFACE_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct ripinter_brief *)pMsgRcv->msg_data;

    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }

		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct ripinter_brief *hhrRIPInstanceIfTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                         int exact,
                                                         const struct ripinter_brief *index_input)
{
    struct listnode        *node;
    struct ripinter_brief  *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->ifindex)
        {
            return cache->data_list->head->data;
        }

        if (pdata->ifindex == index_input->ifindex)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrRIPInstanceIfTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ripinter_brief   data_index;
    struct ripinter_brief   *pdata;
    int                     data_len = sizeof(struct ripinter_brief);
    u_int32_t               index_int = 0;
    int                     ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrRIPInstanceIfTable_cache)
    {
        hhrRIPInstanceIfTable_cache = snmp_cache_init(data_len,
                                                      hhrRIPInstanceIfTable_get_data_from_ipc,
                                                      hhrRIPInstanceIfTable_node_lookup);

        if (NULL == hhrRIPInstanceIfTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.ifindex = index_int;
    pdata = snmp_cache_get_data_by_index(hhrRIPInstanceIfTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->ifindex);
    }

    switch (vp->magic)
    {
        case hhrRIPInstanceIFDesc:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->name, NAME_STRING_LEN);
            *var_len = strlen(str_value);
            return str_value;

        case hhrRIPIfRelatedInstanceID:
            uint_value = pdata->id;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrRIPIfAuthPassword:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->auth_password, NAME_STRING_LEN);
            *var_len = strlen(str_value);
            return str_value;

        case hhrRIPIfAuthMd5Id:
            uint_value = pdata->auth_md5_id;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrRIPIfSplitHorizon:
            uint_value = pdata->split_horizon_status;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrRIPIfPoisonReverse:
            uint_value = pdata->poison_reverse_status;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrRIPIfPassive:
            uint_value = pdata->passive_status;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrRIPIfMetric:
            uint_value = pdata->metric;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrRIPIfVersion:
            uint_value = pdata->rip_version;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrRIPIfV2VersionSendType:
            uint_value = pdata->pkt_send_type;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        default:
            return NULL;
    }

    return NULL;
}


/*** hhrRIPRedistributeTable start *************************************************************/
#if 0
struct rip_redistribute_route_info *rip_com_get_redistribute_route(struct rip_redistribute_route_key *key,
                                                                   int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg;

    pmesg = ipc_send_common_wait_reply1(key, sizeof(struct rip_redistribute_route_key), 1, MODULE_ID_RIP,
                                        module_id, IPC_TYPE_RIP, RIP_SNMP_REDIS_ROUTE_GET, IPC_OPCODE_GET_BULK, 0);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct rip_redistribute_route_info *)pmesg->msg_data;
    }

    return NULL;
}
#endif
int hhrRIPRedistributeTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                              struct rip_redistribute_route_key *index_input)
{
    struct rip_redistribute_route_info *pdata = NULL;
    int data_len = sizeof(struct rip_redistribute_route_info);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrRIP_ALL_Table_get_bulk(index_input, RIP_SNMP_REDIS_ROUTE_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct rip_redistribute_route_info *)pMsgRcv->msg_data;
    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }

		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct rip_redistribute_route_info *hhrRIPRedistributeTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                        int exact,
                                                                        const struct rip_redistribute_route_key *index_input)
{
    struct listnode        *node;
    struct rip_redistribute_route_info *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if ((0 == index_input->rip_id_key)
                && (0 == index_input->redis_route_type_key)
                && (0 == index_input->redis_target_instance_id_key))
        {
            return cache->data_list->head->data;
        }

        if ((pdata->rip_id == index_input->rip_id_key)
                && (pdata->redis_route_type == index_input->redis_route_type_key)
                && (pdata->redis_target_instance_id == index_input->redis_target_instance_id_key))
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrRIPRedistributeTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct rip_redistribute_route_key  data_index;
    struct rip_redistribute_route_info  *pdata = NULL;
    int                     data_len = sizeof(struct rip_redistribute_route_info);
    u_int32_t               index_rip_id = 0;
    u_int32_t               index_route_type = 0;
    u_int32_t               index_instance = 0;
    int                     ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_intx3_index_get(vp, name, length, &index_rip_id, \
                                     &index_route_type, \
                                     &index_instance, \
                                     exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrRIPRedistributeTable_cache)
    {
        hhrRIPRedistributeTable_cache = snmp_cache_init(data_len,
                                                        hhrRIPRedistributeTable_get_data_from_ipc,
                                                        hhrRIPRedistributeTable_node_lookup);

        if (NULL == hhrRIPRedistributeTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, sizeof(struct rip_redistribute_route_key));
    data_index.rip_id_key = index_rip_id;
    data_index.redis_route_type_key = index_route_type;
    data_index.redis_target_instance_id_key = index_instance;
    pdata = snmp_cache_get_data_by_index(hhrRIPRedistributeTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->rip_id)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_intx3_index_set(vp, name, length, (u_int32_t)pdata->rip_id, \
                                   (u_int32_t)pdata->redis_route_type, \
                                   (u_int32_t)pdata->redis_target_instance_id);
    }

    switch (vp->magic)
    {
        case hhrRIPRedistributeMetric:
            int_value = pdata->redis_route_metric;
            *var_len = sizeof(int_value);
            return (u_char *)(&int_value);

        default :
            return NULL;
    }

    return NULL;
}


/*** hhrRIPIfRouteSummaryTable start *************************************************************/
#if 0
struct rip_route_summary_info *rip_com_get_route_summary(struct rip_route_summary_key *key, int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg;

    pmesg = ipc_send_common_wait_reply1(key, sizeof(struct rip_route_summary_key), 1, MODULE_ID_RIP,
                                        module_id, IPC_TYPE_RIP, RIP_SNMP_ROUTE_SUMMARY_GET, IPC_OPCODE_GET_BULK, 0);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct rip_route_summary_info *)pmesg->msg_data;
    }

    return NULL;
}
#endif
int hhrRIPIfRouteSummaryTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                                struct rip_route_summary_key *index_input)
{
    struct rip_route_summary_info *pdata = NULL;
    int data_len = sizeof(struct rip_route_summary_info);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrRIP_ALL_Table_get_bulk(index_input, RIP_SNMP_ROUTE_SUMMARY_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct rip_route_summary_info *)pMsgRcv->msg_data;

    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct rip_route_summary_info *hhrRIPIfRouteSummaryTable_lookup(struct ipran_snmp_data_cache *cache,
                                                                int exact,
                                                                const struct rip_route_summary_key *index_input)
{
    struct listnode        *node;
    struct rip_route_summary_info *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if ((0 == index_input->ifindex_key)
                && (0 == index_input->sum_net_ip_key.s_addr)
                && (0 == index_input->sum_net_mask_key.s_addr))
        {
            return cache->data_list->head->data;
        }

        if ((pdata->ifindex == index_input->ifindex_key)
                && (pdata->sum_net_ip.s_addr == index_input->sum_net_ip_key.s_addr)
                && (pdata->sum_net_mask.s_addr == index_input->sum_net_mask_key.s_addr))
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrRIPIfRouteSummaryTable_get(struct variable *vp,
                                      oid *name,
                                      size_t *length,
                                      int exact, size_t *var_len, WriteMethod **write_method)
{
    struct rip_route_summary_key data_index;
    struct rip_route_summary_info *pdata = NULL;
    int                        data_len = sizeof(struct rip_route_summary_info);
    u_int32_t                  index_if = 0;
    u_int32_t                  index_addr = 0;
    u_int32_t                  index_mask = 0;
    int                        ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_ipx2_index_get(vp, name, length, &index_if, \
                                        &index_addr, \
                                        &index_mask , \
                                        exact);


    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrRIPIfRouteSummaryTable_cache)
    {
        hhrRIPIfRouteSummaryTable_cache = snmp_cache_init(data_len,
                                                          hhrRIPIfRouteSummaryTable_get_data_from_ipc,
                                                          hhrRIPIfRouteSummaryTable_lookup);

        if (NULL == hhrRIPIfRouteSummaryTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, sizeof(struct rip_route_summary_key));
    data_index.ifindex_key = index_if;
    data_index.sum_net_ip_key.s_addr = index_addr;
    data_index.sum_net_mask_key.s_addr = index_mask;
    pdata = snmp_cache_get_data_by_index(hhrRIPIfRouteSummaryTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->ifindex)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_ipx2_index_set(vp, name, length, (u_int32_t)pdata->ifindex,
                                      (u_int32_t)pdata->sum_net_ip.s_addr,
                                      (u_int32_t)pdata->sum_net_mask.s_addr);
    }

    switch (vp->magic)
    {
        case hhrRIPIfRouteSummaryIFDesc:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->sum_if_desc, strlen(pdata->sum_if_desc));
            *var_len = strlen(str_value);
            return str_value;

        default :
            return NULL;
    }

    return NULL;
}


/*** hhrRIPIfNeighborTable start *************************************************************/
#if 0
struct rip_if_nbr_info *rip_com_get_if_neighbor(struct rip_if_nbr_key *key,
                                                int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg;

    pmesg = ipc_send_common_wait_reply1(key, sizeof(struct rip_if_nbr_key), 1, MODULE_ID_RIP,
                                        module_id, IPC_TYPE_RIP, RIP_SNMP_IF_NBR_GET, IPC_OPCODE_GET_BULK, 0);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct rip_if_nbr_info *)pmesg->msg_data;
    }

    return NULL;
}
#endif

int hhrRIPIfNeighborTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                            struct rip_if_nbr_key *index_input)
{
    struct rip_if_nbr_info *pdata = NULL;
    int data_len = sizeof(struct rip_if_nbr_info);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrRIP_ALL_Table_get_bulk(index_input, RIP_SNMP_IF_NBR_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct rip_if_nbr_info *)pMsgRcv->msg_data;

    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct rip_if_nbr_info *hhrRIPIfNeighborTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                          int exact,
                                                          const struct rip_if_nbr_key *index_input)
{
    struct listnode        *node;
    struct rip_if_nbr_info *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if ((0 == index_input->ifindex_key)
                && (0 == index_input->remote_nbr_ip_key.s_addr))
        {
            return cache->data_list->head->data;
        }

        if ((pdata->ifindex == index_input->ifindex_key)
                && (pdata->remote_nbe_ip.s_addr == index_input->remote_nbr_ip_key.s_addr))
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrRIPIfNeighborTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    struct rip_if_nbr_key  data_index;
    struct rip_if_nbr_info  *pdata = NULL;
    int                     data_len = sizeof(struct rip_if_nbr_info);
    u_int32_t               index_if = 0;
    u_int32_t               index_addr = 0;
    int                     ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_ip_index_get(vp, name, length, &index_if, &index_addr, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrRIPIfNeighborTable_cache)
    {
        hhrRIPIfNeighborTable_cache = snmp_cache_init(data_len,
                                                      hhrRIPIfNeighborTable_get_data_from_ipc,
                                                      hhrRIPIfNeighborTable_node_lookup);

        if (NULL == hhrRIPIfNeighborTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, sizeof(struct rip_if_nbr_key));
    data_index.ifindex_key = index_if;
    data_index.remote_nbr_ip_key.s_addr = index_addr;
    pdata = snmp_cache_get_data_by_index(hhrRIPIfNeighborTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->ifindex)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_ip_index_set(vp, name, length, (u_int32_t)pdata->ifindex, \
                                    (u_int32_t)pdata->remote_nbe_ip.s_addr);
    }

    switch (vp->magic)
    {
        case hhrRIPIfNeighborIFDesc:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->nbr_if_desc, strlen(pdata->nbr_if_desc));
            *var_len = strlen(str_value);
            return str_value;

        default :
            return NULL;
    }

    return NULL;
}


static struct ipc_mesg_n *hhrOSPF_ALL_Table_get_bulk(void *pIndex, int subtype)
{
    int index_len = 0;

    switch (subtype)
    {
        case OSPF_SNMP_INSTANCE_GET:
            index_len = sizeof(uint32_t);
            break;

        case OSPF_SNMP_AREA_GET:
            index_len = sizeof(struct ospf_key);
            break;

        case OSPF_SNMP_NETWORK_GET:
            index_len = sizeof(struct ospf_key_network);
            break;
		
		case OSPF_SNMP_INTERFACE_GET:
            index_len = sizeof(uint32_t);
            break;
		
		case OSPF_SNMP_ABR_SUMMARY_GET:
            index_len = sizeof(struct ospf_abr_sum_info_key);
            break;
		
		case OSPF_SNMP_ASBR_SUMMARY_GET:
            index_len = sizeof(struct ospf_asbr_sum_info_key);
            break;

		case OSPF_SNMP_REDIS_ROUTE_GET:
            index_len = sizeof(struct ospf_redistribute_route_info_key);
            break;

        case OSPF_SNMP_NBMA_NEIGHBOR_GET:
            index_len = sizeof(struct ospf_nbma_nbr_info_key);
            break;
		
		case OSPF_SNMP_DCN_CONF_GET:
            index_len = sizeof(uint32_t);
            break;
		
		case OSPF_SNMP_DCN_NE_INFO_GET:
            index_len = sizeof(uint32_t);
            break;
		
        default:
            break;
    }

	return ipc_sync_send_n2((void *)pIndex, index_len, 1, MODULE_ID_OSPF, MODULE_ID_SNMPD, \
							IPC_TYPE_OSPF, subtype, IPC_OPCODE_GET_BULK, 0, 1000);
}

/*** hhrOSPFInstanceTable start ***********************************************/
#if 0
struct statics_ospf *ospf_com_get_ospf_instance(int ospf_id, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = NULL ;

    /*pmesg = ipc_send_common_wait_reply1(&ospf_id, 4, 1, MODULE_ID_OSPF,
                              module_id, IPC_TYPE_OSPF, OSPF_SNMP_INSTANCE_GET, IPC_OPCODE_GET_BULK, 0);*/
	
	pmesg = ipc_sync_send_n2(&ospf_id, 4, 1, MODULE_ID_OSPF,
					module_id, IPC_TYPE_OSPF, OSPF_SNMP_INSTANCE_GET, IPC_OPCODE_GET_BULK, 0,2000);
	
	if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct statics_ospf *)pmesg->msg_data;
    }

    return NULL;
}
#endif
int hhrOSPFInstanceTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                           struct statics_ospf *index_input)
{
    struct statics_ospf *pdata = NULL;
    int data_len = sizeof(struct statics_ospf);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrOSPF_ALL_Table_get_bulk(&(index_input->ospf_id), OSPF_SNMP_INSTANCE_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct statics_ospf *)pMsgRcv->msg_data;

    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct statics_ospf *hhrOSPFInstanceTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                      int exact,
                                                      const struct statics_ospf *index_input)
{
    struct listnode        *node;
    struct statics_ospf    *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->ospf_id)
        {
            return cache->data_list->head->data;
        }

        if (pdata->ospf_id == index_input->ospf_id)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrOSPFInstanceTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct statics_ospf data_index;
    struct statics_ospf *pdata;
    int                 data_len = sizeof(struct statics_ospf);
    u_int32_t           index_int = 0;
    int                 ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrOSPFInstanceTable_cache)
    {
        hhrOSPFInstanceTable_cache = snmp_cache_init(data_len,
                                                     hhrOSPFInstanceTable_get_data_from_ipc,
                                                     hhrOSPFInstanceTable_node_lookup);

        if (NULL == hhrOSPFInstanceTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.ospf_id = index_int;
    pdata = snmp_cache_get_data_by_index(hhrOSPFInstanceTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->ospf_id)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->ospf_id);
    }

    switch (vp->magic)
    {
        case hhrOSPFInstanceRouterId:
            *var_len = sizeof(uint32_t);
            memcpy(&ip_value, &pdata->router_id, sizeof(uint32_t));
            return (u_char *)(&ip_value);

        case hhrOSPFInstanceVPNId:
            uint_value = pdata->vpn;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrOSPFInstanceBandwidth:
            uint_value = pdata->ref_bandwidth;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrOSPFInstanceOpaqueLsa:
            uint_value = pdata->opaque_lsa_conf;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrOSPFInstanceRfc1583:
            uint_value = pdata->rfc1583_conf;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrOSPFInstanceDistance:
            uint_value = pdata->manger_distance;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrOSPFInstanceASExDistance:
            uint_value = pdata->ase_distance;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrOSPFInstanceLSDBRefreshInterval:
            uint_value = pdata->lsdb_refresh_interval;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        default :
            return (NULL);
    }
}

/*** hhrOSPFAreaTable start ***********************************************/
#if 0
struct statics_ospf_area *ospf_com_get_ospf_area(struct ospf_key *key, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg;

    /*pmesg = ipc_send_common_wait_reply1(key, sizeof(struct ospf_key), 1, MODULE_ID_OSPF,
                          module_id, IPC_TYPE_OSPF, OSPF_SNMP_AREA_GET, IPC_OPCODE_GET_BULK, 0);*/
    pmesg = ipc_sync_send_n2(key, sizeof(struct ospf_key), 1, MODULE_ID_OSPF,
                          module_id, IPC_TYPE_OSPF, OSPF_SNMP_AREA_GET, IPC_OPCODE_GET_BULK, 0, 2000);

	
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct statics_ospf_area *)pmesg->msg_data;
    }

    return NULL;
}
#endif
int hhrOSPFAreaTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                       struct statics_ospf_area *index_input)
{
    struct statics_ospf_area *pdata = NULL;
    struct ospf_key key;
    int data_len = sizeof(struct statics_ospf_area);
    int data_num = 0;
    int i;

    key.ospf_id = index_input->ospf_id;
    key.area_id = index_input->area_id;
	
    struct ipc_mesg_n *pMsgRcv = hhrOSPF_ALL_Table_get_bulk(&key, OSPF_SNMP_AREA_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct statics_ospf_area *)pMsgRcv->msg_data;
	
    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct statics_ospf_area *hhrOSPFAreaTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                       int exact,
                                                       const struct statics_ospf_area *index_input)
{
    struct listnode            *node;
    struct statics_ospf_area   *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->ospf_id
                && 0 == index_input->area_id)
        {
            return cache->data_list->head->data;
        }

        if (pdata->ospf_id == index_input->ospf_id
                && pdata->area_id  == index_input->area_id)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrOSPFAreaTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct statics_ospf_area    data_index;
    struct statics_ospf_area    *pdata;
    int                         data_len = sizeof(struct statics_ospf_area);
    u_int32_t                   index_ospf_id = 0;
    u_int32_t                   index_area_id = 0;
    int                         ret = 0;

    *write_method = NULL;

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index_ospf_id, &index_area_id, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrOSPFAreaTable_cache)
    {
        hhrOSPFAreaTable_cache = snmp_cache_init(data_len,
                                                 hhrOSPFAreaTable_get_data_from_ipc,
                                                 hhrOSPFAreaTable_node_lookup);

        if (NULL == hhrOSPFAreaTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.ospf_id = index_ospf_id;
    data_index.area_id = index_area_id;
    pdata = snmp_cache_get_data_by_index(hhrOSPFAreaTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_intx2_index_set(vp, name, length, pdata->ospf_id, pdata->area_id);
    }

    switch (vp->magic)
    {
        case hhrOSPFAreaName:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pdata->name, strlen(pdata->name));
            *var_len = strlen(pdata->name);
            return str_value;

        case hhrOSPFAreaType:
            *var_len = sizeof(int);
            int_value = 0;

            if (0 == pdata->area_type)
            {
                int_value = 3;
            }
            else if (1 == pdata->area_type)
            {
                int_value = 1;
            }
            else if (2 == pdata->area_type)
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrOSPFAreaNoSummary:
            *var_len = sizeof(int);
            int_value = (0 == pdata->summary) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrOSPFAreaDefaultCost:
            *var_len = sizeof(u_int32);
            uint_value = pdata->cost;
            return (u_char *)(&uint_value);

        default :
            return NULL;
    }

    return NULL;
}

/*** hhrOSPFAreaNetworkTable start ********************************************/
#if 0
struct statics_ospf_network *ospf_com_get_ospf_network(struct statics_ospf_network *key, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = NULL;

    /*pmesg = ipc_send_common_wait_reply1(key, sizeof(struct ospf_key_network), 1, MODULE_ID_OSPF,
                              module_id, IPC_TYPE_OSPF, OSPF_SNMP_NETWORK_GET, IPC_OPCODE_GET_BULK, 0);*/
	pmesg = ipc_sync_send_n2(key, sizeof(struct ospf_key_network), 1, MODULE_ID_OSPF,
							module_id, IPC_TYPE_OSPF, OSPF_SNMP_NETWORK_GET, IPC_OPCODE_GET_BULK, 0, 2000);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct statics_ospf_network *)pmesg->msg_data;
    }

    return NULL;
}
#endif

int hhrOSPFAreaNetworkTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                              struct statics_ospf_network *index_input)

{
    struct statics_ospf_network *pdata = NULL;
    int data_len = sizeof(struct statics_ospf_network);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrOSPF_ALL_Table_get_bulk(index_input, OSPF_SNMP_NETWORK_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct statics_ospf_network *)pMsgRcv->msg_data;
	
    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct statics_ospf_network *hhrOSPFAreaNetworkTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                 int exact,
                                                                 const struct statics_ospf_network *index_input)
{
    struct listnode                *node;
    struct statics_ospf_network    *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->ospf_id
                && 0 == index_input->area_id
                && 0 == index_input->p.u.prefix4.s_addr
                && 0 == index_input->p.prefixlen)
        {
            return cache->data_list->head->data;
        }

        if (pdata->ospf_id == index_input->ospf_id
                && pdata->area_id == index_input->area_id
                && pdata->p.u.prefix4.s_addr == index_input->p.u.prefix4.s_addr
                && pdata->p.prefixlen == index_input->p.prefixlen)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrOSPFAreaNetworkTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct statics_ospf_network data_index;
    struct statics_ospf_network *pdata;
    int                         data_len = sizeof(struct statics_ospf_network);
    u_int32_t                   index_ospf_id = 0;
    u_int32_t                   index_area_id = 0;
    u_int32_t                   index_addr = 0;
    u_int32_t                   index_prefixlen = 0;
    int                         ret = 0;

    *write_method = NULL;

    ret  = ipran_snmp_intx2_ip_int_index_get(vp, name, length, &index_ospf_id, \
                                             &index_area_id, \
                                             &index_addr, \
                                             &index_prefixlen , \
                                             exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrOSPFAreaNetworkTable_cache)
    {
        hhrOSPFAreaNetworkTable_cache = snmp_cache_init(data_len,
                                                        hhrOSPFAreaNetworkTable_get_data_from_ipc,
                                                        hhrOSPFAreaNetworkTable_node_lookup);

        if (NULL == hhrOSPFAreaNetworkTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.ospf_id = index_ospf_id;
    data_index.area_id = index_area_id;
    data_index.p.u.prefix4.s_addr = index_addr;
    data_index.p.prefixlen = index_prefixlen;
    pdata = snmp_cache_get_data_by_index(hhrOSPFAreaNetworkTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->ospf_id)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_intx2_ip_int_index_set(vp, name, length, (u_int32_t)pdata->ospf_id, \
                                          (u_int32_t)pdata->area_id, \
                                          (u_int32_t)pdata->p.u.prefix4.s_addr, \
                                          (u_int32_t)pdata->p.prefixlen);
    }

    switch (vp->magic)
    {
        case hhrOSPFAreaNetWorkReservedField:
            *var_len = sizeof(u_int32);
            uint_value = 0;
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }

    return NULL;
}

/*** hhrOSPFIfTable start *****************************************************/
#if 0
struct statics_ospf_if *ospf_com_get_interface(uint32_t ifindex, int module_id, int *pdata_num)
{

    struct ipc_mesg_n *pmesg = NULL;

    /*pmesg = ipc_send_common_wait_reply1(&ifindex, sizeof(uint32_t), 1, MODULE_ID_OSPF,
                            module_id, IPC_TYPE_OSPF, OSPF_SNMP_INTERFACE_GET, IPC_OPCODE_GET_BULK, 0);*/
	pmesg = ipc_sync_send_n2(&ifindex, sizeof(uint32_t), 1, MODULE_ID_OSPF,
						module_id, IPC_TYPE_OSPF, OSPF_SNMP_INTERFACE_GET, IPC_OPCODE_GET_BULK, 0, 2000);

							

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct statics_ospf_if *)pmesg->msg_data;
    }

    return NULL;
}
#endif
int hhrOSPFIfTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                     struct statics_ospf_if *index_input)
{
    struct statics_ospf_if *pdata = NULL;
    int data_len = sizeof(struct statics_ospf_if);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrOSPF_ALL_Table_get_bulk(&(index_input->ifindex), OSPF_SNMP_INTERFACE_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct statics_ospf_if *)pMsgRcv->msg_data;
	
    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct statics_ospf_if *hhrOSPFIfTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                   int exact,
                                                   const struct statics_ospf_if *index_input)
{
    struct listnode        *node;
    struct statics_ospf_if *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->ifindex)
        {
            return cache->data_list->head->data;
        }

        if (pdata->ifindex == index_input->ifindex)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrOSPFIfTable_get(struct variable *vp,
                           oid *name,
                           size_t *length,
                           int exact, size_t *var_len, WriteMethod **write_method)
{
    struct statics_ospf_if  data_index;
    struct statics_ospf_if  *pdata;
    int                     data_len = sizeof(struct statics_ospf_if);
    u_int32_t               index_int = 0;
    int                     ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrOSPFIfTable_cache)
    {
        hhrOSPFIfTable_cache = snmp_cache_init(data_len,
                                               hhrOSPFIfTable_get_data_from_ipc,
                                               hhrOSPFIfTable_node_lookup);

        if (NULL == hhrOSPFIfTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.ifindex = index_int;
    pdata = snmp_cache_get_data_by_index(hhrOSPFIfTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->ifindex);
    }

    switch (vp->magic)
    {
        case hhrOSPFIfDesc:
            memset(str_value, 0, STRING_LEN);
            ifm_get_name_by_ifindex(pdata->ifindex, str_value);
            *var_len = strlen(str_value);
            return str_value;

        case hhrOSPFIfNetType:
            *var_len = sizeof(int);
            int_value = 0;

            if (pdata->if_type == 1)
            {
                int_value = 3;
            }
            else if (pdata->if_type == 2)
            {
                int_value = 1;
            }
            else if (pdata->if_type == 3)
            {
                int_value = 2;
            }
            else if (pdata->if_type == 4)
            {
                int_value = 5;
            }

            return (u_char *)(&int_value);

        case hhrOSPFIfAuthType:
            int_value = pdata->auth_type;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrOSPFIfMd5Id:
            int_value = pdata->md5_id;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrOSPFIfAuthPassword:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->auth_password, strlen(pdata->auth_password));
            *var_len = strlen(str_value);
            return str_value;

        case hhrOSPFIfCostValue:
            int_value = pdata->cost_value;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrOSPFIfDeadTimeInterval:
            int_value = pdata->dead_interval;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrOSPFIfHelloTimeInterval:
            int_value = pdata->hello_interval;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrOSPFIfMTUCheck:
            int_value = pdata->mtu_check;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrOSPFIfDRPriority:
            int_value = pdata->dr_priority;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrOSPFIfRetransmitInterval:
            int_value = pdata->retransmit_interval;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default :
            return NULL;
    }

    return NULL;
}

/*** hhrOSPFAbrSummaryTable start *************************************************************/
#if 0
struct statistics_ospf_abr_summary_info *ospf_com_get_abr_summary(struct ospf_abr_sum_info_key *key, int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg;

    pmesg = ipc_send_common_wait_reply1(key, sizeof(struct ospf_abr_sum_info_key), 1, MODULE_ID_OSPF,
                                        module_id, IPC_TYPE_OSPF, OSPF_SNMP_ABR_SUMMARY_GET, IPC_OPCODE_GET_BULK, 0);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct statistics_ospf_abr_summary_info *)pmesg->msg_data;
    }

    return NULL;
}
#endif

int hhrOSPFAbrSummaryTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                             struct ospf_abr_sum_info_key *index_input)
{
    struct statistics_ospf_abr_summary_info *pdata = NULL;
    int data_len = sizeof(struct statistics_ospf_abr_summary_info);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrOSPF_ALL_Table_get_bulk(index_input, OSPF_SNMP_ABR_SUMMARY_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct statistics_ospf_abr_summary_info *)pMsgRcv->msg_data;
	
    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct statistics_ospf_abr_summary_info *hhrOSPFAbrSummaryTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                            int exact,
                                                                            const struct ospf_abr_sum_info_key *index_input
                                                                           )
{
    struct listnode        *node;
    struct statistics_ospf_abr_summary_info *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if ((0 == index_input->ospf_id_key)
                && (0 == index_input->area_id_key)
                && (0 == index_input->abr_sum_ip_addr.s_addr)
                && (0 == index_input->abr_sum_ip_mask.s_addr))
        {
            return cache->data_list->head->data;
        }

        if ((pdata->ospf_id == index_input->ospf_id_key)
                && (pdata->area_id == index_input->area_id_key)
                && (pdata->abr_sum_ip_addr.s_addr == index_input->abr_sum_ip_addr.s_addr)
                && (pdata->abr_sum_ip_mask.s_addr == index_input->abr_sum_ip_mask.s_addr))
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrOSPFAbrSummaryTable_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ospf_abr_sum_info_key data_index;
    struct statistics_ospf_abr_summary_info *pdata = NULL;
    int                       data_len = sizeof(struct statistics_ospf_abr_summary_info);
    u_int32_t                 index_ospf_id = 0;
    u_int32_t                 index_area_id = 0;
    u_int32_t                 index_addr = 0;
    u_int32_t                 index_mask = 0;
    int                       ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_intx2_ipx2_index_get(vp, name, length, &index_ospf_id, \
                                          &index_area_id, \
                                          &index_addr, \
                                          &index_mask , \
                                          exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrOSPFAbrSummaryTable_cache)
    {
        hhrOSPFAbrSummaryTable_cache = snmp_cache_init(data_len,
                                                       hhrOSPFAbrSummaryTable_get_data_from_ipc,
                                                       hhrOSPFAbrSummaryTable_node_lookup);

        if (NULL == hhrOSPFAbrSummaryTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, sizeof(struct ospf_abr_sum_info_key));
    data_index.ospf_id_key = index_ospf_id;
    data_index.area_id_key = index_area_id;
    data_index.abr_sum_ip_addr.s_addr = index_addr;
    data_index.abr_sum_ip_mask.s_addr = index_mask;
    pdata = snmp_cache_get_data_by_index(hhrOSPFAbrSummaryTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->ospf_id)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_intx2_ipx2_index_set(vp, name, length, (u_int32_t)pdata->ospf_id,
                                        (u_int32_t)pdata->area_id,
                                        (u_int32_t)pdata->abr_sum_ip_addr.s_addr,
                                        (u_int32_t)pdata->abr_sum_ip_mask.s_addr);
    }

    switch (vp->magic)
    {
        case hhrOSPFAbrSummaryNotAdvertise:
            int_value = pdata->abr_summary_notAdvertise;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrOSPFAbrSummaryCost:
            int_value = pdata->abr_summary_cost;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);


        default :
            return NULL;
    }

    return NULL;
}

/*** hhrOSPFAsbrSummaryTable start *************************************************************/
#if 0
struct statistics_ospf_asbr_summary_info *ospf_com_get_asbr_summary(struct ospf_asbr_sum_info_key *key, int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg;

    pmesg = ipc_send_common_wait_reply1(key, sizeof(struct ospf_asbr_sum_info_key), 1, MODULE_ID_OSPF,
                                        module_id, IPC_TYPE_OSPF, OSPF_SNMP_ASBR_SUMMARY_GET, IPC_OPCODE_GET_BULK, 0);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct statistics_ospf_asbr_summary_info *)pmesg->msg_data;
    }

    return NULL;
}
#endif

int hhrOSPFAsbrSummaryTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                              struct ospf_asbr_sum_info_key *index_input)
{
    struct statistics_ospf_asbr_summary_info *pdata = NULL;
    int data_len = sizeof(struct statistics_ospf_asbr_summary_info);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrOSPF_ALL_Table_get_bulk(index_input, OSPF_SNMP_ASBR_SUMMARY_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct statistics_ospf_asbr_summary_info *)pMsgRcv->msg_data;
	
    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct statistics_ospf_asbr_summary_info *hhrOSPFAsbrSummaryTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                              int exact,
                                                                              const struct ospf_asbr_sum_info_key *index_input)
{
    struct listnode        *node;
    struct statistics_ospf_asbr_summary_info *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if ((0 == index_input->ospf_id_key)
                && (0 == index_input->asbr_sum_ip_addr.s_addr)
                && (0 == index_input->asbr_sum_ip_mask.s_addr))
        {
            return cache->data_list->head->data;
        }

        if ((pdata->ospf_id == index_input->ospf_id_key)
                && (pdata->asbr_sum_ip_addr.s_addr == index_input->asbr_sum_ip_addr.s_addr)
                && (pdata->asbr_sum_ip_mask.s_addr == index_input->asbr_sum_ip_mask.s_addr))
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrOSPFAsbrSummaryTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ospf_asbr_sum_info_key data_index;
    struct statistics_ospf_asbr_summary_info *pdata = NULL;
    int                        data_len = sizeof(struct statistics_ospf_asbr_summary_info);
    u_int32_t                  index_ospf_id = 0;
    u_int32_t                  index_addr = 0;
    u_int32_t                  index_mask = 0;
    int                        ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_ipx2_index_get(vp, name, length, &index_ospf_id, \
                                        &index_addr, \
                                        &index_mask , \
                                        exact);


    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrOSPFAsbrSummaryTable_cache)
    {
        hhrOSPFAsbrSummaryTable_cache = snmp_cache_init(data_len,
                                                        hhrOSPFAsbrSummaryTable_get_data_from_ipc,
                                                        hhrOSPFAsbrSummaryTable_node_lookup);

        if (NULL == hhrOSPFAsbrSummaryTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, sizeof(struct ospf_asbr_sum_info_key));
    data_index.ospf_id_key = index_ospf_id;
    data_index.asbr_sum_ip_addr.s_addr = index_addr;
    data_index.asbr_sum_ip_mask.s_addr = index_mask;
    pdata = snmp_cache_get_data_by_index(hhrOSPFAsbrSummaryTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->ospf_id)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_ipx2_index_set(vp, name, length, (u_int32_t)pdata->ospf_id,
                                      (u_int32_t)pdata->asbr_sum_ip_addr.s_addr,
                                      (u_int32_t)pdata->asbr_sum_ip_mask.s_addr);
    }

    switch (vp->magic)
    {
        case hhrOSPFAsbrSummaryNotAdvertise:
            int_value = pdata->asbr_summary_notAdvertise;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrOSPFAsbrSummaryCost:
            int_value = pdata->asbr_summary_cost;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default :
            return NULL;
    }

    return NULL;
}

/*** hhrOSPFRedistributeTable start *************************************************************/
#if 0
struct statistics_ospf_redistribute_route_info *ospf_com_get_redistribute_route(struct ospf_redistribute_route_info_key *key,
                                                                                int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg;

    pmesg = ipc_send_common_wait_reply1(key, sizeof(struct ospf_redistribute_route_info_key), 1, MODULE_ID_OSPF,
                                        module_id, IPC_TYPE_OSPF, OSPF_SNMP_REDIS_ROUTE_GET, IPC_OPCODE_GET_BULK, 0);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct statistics_ospf_redistribute_route_info *)pmesg->msg_data;
    }

    return NULL;
}
#endif

int hhrOSPFRedistributeTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                               struct ospf_redistribute_route_info_key *index_input)
{
    struct statistics_ospf_redistribute_route_info *pdata = NULL;
    int data_len = sizeof(struct statistics_ospf_redistribute_route_info);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrOSPF_ALL_Table_get_bulk(index_input, OSPF_SNMP_REDIS_ROUTE_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct statistics_ospf_redistribute_route_info *)pMsgRcv->msg_data;
	
    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct statistics_ospf_redistribute_route_info *hhrOSPFRedistributeTable_node_lookup(struct ipran_snmp_data_cache *cache,
        int exact,
        const struct ospf_redistribute_route_info_key *index_input)
{
    struct listnode        *node;
    struct statistics_ospf_redistribute_route_info *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if ((0 == index_input->ospf_id_key)
                && (0 == index_input->redis_route_type_key)
                && (0 == index_input->redis_target_instance_id_key))
        {
            return cache->data_list->head->data;
        }

        if ((pdata->ospf_id == index_input->ospf_id_key)
                && (pdata->redis_route_type == index_input->redis_route_type_key)
                && (pdata->redis_target_instance_id == index_input->redis_target_instance_id_key))
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrOSPFRedistributeTable_get(struct variable *vp,
                                     oid *name,
                                     size_t *length,
                                     int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ospf_redistribute_route_info_key  data_index;
    struct statistics_ospf_redistribute_route_info  *pdata = NULL;
    int                     data_len = sizeof(struct statistics_ospf_redistribute_route_info);
    u_int32_t               index_ospf_id = 0;
    u_int32_t               index_route_type = 0;
    u_int32_t               index_instance = 0;
    int                     ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_intx3_index_get(vp, name, length, &index_ospf_id, \
                                     &index_route_type, \
                                     &index_instance, \
                                     exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrOSPFRedistributeTable_cache)
    {
        hhrOSPFRedistributeTable_cache = snmp_cache_init(data_len,
                                                         hhrOSPFRedistributeTable_get_data_from_ipc,
                                                         hhrOSPFRedistributeTable_node_lookup);

        if (NULL == hhrOSPFRedistributeTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, sizeof(struct ospf_redistribute_route_info_key));
    data_index.ospf_id_key = index_ospf_id;
    data_index.redis_route_type_key = index_route_type;
    data_index.redis_target_instance_id_key = index_instance;
    pdata = snmp_cache_get_data_by_index(hhrOSPFRedistributeTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->ospf_id)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_intx3_index_set(vp, name, length, (u_int32_t)pdata->ospf_id, \
                                   (u_int32_t)pdata->redis_route_type, \
                                   (u_int32_t)pdata->redis_target_instance_id);
    }

    switch (vp->magic)
    {
        case hhrOSPFRedistributeMetric:
            int_value = pdata->redis_route_metric;
            *var_len = sizeof(int_value);
            return (u_char *)(&int_value);

        case hhrOSPFRedistributeMetricType:
            int_value = pdata->route_turn2_ospf_tpye;
            *var_len = sizeof(int_value);
            return (u_char *)(&int_value);

        default :
            return NULL;
    }

    return NULL;
}


/*** hhrOSPFNeighborTable start *************************************************************/
#if 0
struct statistics_ospf_nbma_neighbor_info *ospf_com_get_nbma_neighbor(struct ospf_nbma_nbr_info_key *key,
                                                                      int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg;

    pmesg = ipc_send_common_wait_reply1(key, sizeof(struct ospf_nbma_nbr_info_key), 1, MODULE_ID_OSPF,
                                        module_id, IPC_TYPE_OSPF, OSPF_SNMP_NBMA_NEIGHBOR_GET, IPC_OPCODE_GET_BULK, 0);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct statistics_ospf_nbma_neighbor_info *)pmesg->msg_data;
    }

    return NULL;
}
#endif

int hhrOSPFNeighborTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                           struct ospf_nbma_nbr_info_key *index_input)
{
    struct statistics_ospf_nbma_neighbor_info *pdata = NULL;
    int data_len = sizeof(struct statistics_ospf_nbma_neighbor_info);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrOSPF_ALL_Table_get_bulk(index_input, OSPF_SNMP_NBMA_NEIGHBOR_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct statistics_ospf_nbma_neighbor_info *)pMsgRcv->msg_data;
	
    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct statistics_ospf_nbma_neighbor_info *hhrOSPFNeighborTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                            int exact,
                                                                            const struct ospf_nbma_nbr_info_key *index_input)
{
    struct listnode        *node;
    struct statistics_ospf_nbma_neighbor_info *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if ((0 == index_input->ospf_id_key)
                && (0 == index_input->nbr_addr_key.s_addr))
        {
            return cache->data_list->head->data;
        }

        if ((pdata->ospf_id == index_input->ospf_id_key)
                && (pdata->nbr_addr.s_addr == index_input->nbr_addr_key.s_addr))
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrOSPFNeighborTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ospf_nbma_nbr_info_key  data_index;
    struct statistics_ospf_nbma_neighbor_info  *pdata = NULL;
    int                     data_len = sizeof(struct statistics_ospf_nbma_neighbor_info);
    u_int32_t               index_ospf_id = 0;
    u_int32_t               index_addr = 0;
    int                     ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_ip_index_get(vp, name, length, &index_ospf_id, &index_addr, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrOSPFNeighborTable_cache)
    {
        hhrOSPFNeighborTable_cache = snmp_cache_init(data_len,
                                                     hhrOSPFNeighborTable_get_data_from_ipc,
                                                     hhrOSPFNeighborTable_node_lookup);

        if (NULL == hhrOSPFNeighborTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, sizeof(struct ospf_nbma_nbr_info_key));
    data_index.ospf_id_key = index_ospf_id;
    data_index.nbr_addr_key.s_addr = index_addr;
    pdata = snmp_cache_get_data_by_index(hhrOSPFNeighborTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->ospf_id)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_ip_index_set(vp, name, length, (u_int32_t)pdata->ospf_id, \
                                    (u_int32_t)pdata->nbr_addr.s_addr);
    }

    switch (vp->magic)
    {
        case hhrOSPFNeighborPollInterval:
            int_value = pdata->nbr_poll_interval;
            *var_len = sizeof(int_value);
            return (u_char *)(&int_value);

        default :
            return NULL;
    }

    return NULL;
}


/*** hhrArp start *************************************************************/
u_char *hhrArp_get(struct variable *vp,
                   oid *name,
                   size_t *length,
                   int exact, size_t *var_len, WriteMethod **write_method)
{
    struct arp_global *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    pdata = route_com_get_garp_conf(MODULE_ID_SNMPD);

    if (NULL == pdata)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrArpAgeTime:
            uint_value = pdata->age_time;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrArpDynamicLearnNumLimit:
            uint_value = pdata->num_limit;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;
    }
	mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
    return NULL;
}

/*** hhrIpNetToMediaTable start ***********************************************/
int hhrIpNetToMediaTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                           struct arp_entry *index_input)
{
    struct arp_entry *pdata = NULL;
    int data_len = sizeof(struct arp_entry);
    int data_num = 0;
    int i;

    pdata = route_com_get_sarp_bulk(index_input->key.ipaddr, index_input->key.vpnid, \
                                    MODULE_ID_SNMPD, &data_num);

	if(NULL == pdata)
	{
		 return FALSE;
	}
    if (0 == data_num)
    {
    	mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct arp_entry *hhrIpNetToMediaTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                   int exact,
                                                   const struct arp_entry *index_input)
{
    struct listnode    *node;
    struct arp_entry   *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->key.ipaddr
                && 0 == index_input->key.vpnid)
        {
            return cache->data_list->head->data;
        }

        if (pdata->key.ipaddr == index_input->key.ipaddr
                && pdata->key.vpnid  == index_input->key.vpnid)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrIpNetToMediaTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct arp_entry    data_index;
    struct arp_entry    *pdata;
    int                 data_len = sizeof(struct arp_entry);
    u_int32_t           index_vpnid = 0;
    u_int32_t           index_ip = 0;
    int                 ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_ip_int_index_get(vp, name, length, &index_ip, \
                                      &index_vpnid, \
                                      exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrIpNetToMediaTable_cache)
    {
        hhrIpNetToMediaTable_cache = snmp_cache_init(data_len,
                                                     hhrIpNetToMediaTable_get_data_from_ipc,
                                                     hhrIpNetToMediaTable_node_lookup);

        if (NULL == hhrIpNetToMediaTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.key.ipaddr = ntohl(index_ip);
    data_index.key.vpnid  = index_vpnid;
    pdata = snmp_cache_get_data_by_index(hhrIpNetToMediaTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_ip_int_index_set(vp, name, length, htonl((u_int32_t)pdata->key.ipaddr), \
                                    (u_int32_t)pdata->key.vpnid);
    }

    switch (vp->magic)
    {
        case hhrIpNetToMediaIfDesc:
            memset(str_value, '\0', STRING_LEN);
			if(IFM_TYPE_IS_VLANIF(pdata->ifindex))
			{
				if (-1 == ifm_get_name_by_ifindex(pdata->port, str_value))
	            {
	                return NULL;
	            }
			}
			else
			{
            	if (-1 == ifm_get_name_by_ifindex(pdata->ifindex, str_value))
            	{
                	return NULL;
            	}
			}
            *var_len = strlen(str_value);
            return str_value;

        case hhrIpNetToMediaPhysAddress:
            memcpy(mac_value, pdata->mac, 6);
            *var_len = 6;
            return mac_value;

        case hhrIpNetToMediaType:

            switch (pdata->status)
            {
                case ARP_STATUS_INCOMPLETE:
                    int_value = 2;
                    break;

                case ARP_STATUS_COMPLETE:
                    int_value = 3;
                    break;

                case ARP_STATUS_STATIC:
                    int_value = 4;
                    break;

                case ARP_STATUS_AGED:
                    int_value = 2;
                    break;

                default:
                    int_value = 1;
                    break;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;
            break;
		case hhrIpNetToMediaVlanIf:	
			memset(str_value, '\0', STRING_LEN);
			if(IFM_TYPE_IS_VLANIF(pdata->ifindex))
			{
				if (-1 == ifm_get_name_by_ifindex(pdata->ifindex, str_value))
	            {
	                return NULL;
	            }	
				*var_len = strlen(str_value);
				return str_value;
			}	
			break;
        default:
            return NULL;
    }

    return NULL;
}

/*** hhrIpPortArpConfigTable start ***********************************************/
int hhrIpPortArpConfigTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                              struct ifm_arp *ifm_arp_index)
{
    struct ifm_arp *pdata = NULL;
    int data_len = sizeof(struct ifm_arp);
    int data_num = 0;
    int i;

    pdata = route_com_get_intf_arp_conf_bulk(MODULE_ID_SNMPD, ifm_arp_index->ifindex , &data_num);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ifm_arp_index->ifindex [%8x] data_num [%d]\n",
               __FILE__, __LINE__, __func__ , ifm_arp_index->ifindex , data_num);

	if (NULL == pdata)
	{
        return FALSE;
    }	
    if (0 == data_num )
    {
    	mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct arp_entry *hhrIpPortArpConfigTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                      int exact,
                                                      const struct ifm_arp *index_input)
{
    struct listnode    *node;
    struct ifm_arp   *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->ifindex)
        {
            return cache->data_list->head->data;
        }

        if (index_input->ifindex == pdata->ifindex)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrIpPortArpConfigTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ifm_arp    data_index;
    struct ifm_arp    *pdata;
    int                 data_len = sizeof(struct ifm_arp);
    u_int32_t           ifindex_in = 0;
    int                 ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &ifindex_in, exact);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': ifindex_in [%8x] \n",
               __FILE__, __LINE__, __func__ , ifindex_in);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrIpPortArpConfigTable_cache)
    {
        hhrIpPortArpConfigTable_cache = snmp_cache_init(data_len,
                                                        hhrIpPortArpConfigTable_get_data_from_ipc,
                                                        hhrIpPortArpConfigTable_node_lookup);

        if (NULL == hhrIpPortArpConfigTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.ifindex =  ifindex_in ;
    pdata = snmp_cache_get_data_by_index(hhrIpPortArpConfigTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_index_set(vp, name, length, (u_int32_t)pdata->ifindex);
    }

    switch (vp->magic)
    {
        case hhrPortArpConfigIfDescr:
            memset(str_value, '\0', STRING_LEN);

            if (-1 == ifm_get_name_by_ifindex(pdata->ifindex, str_value))
            {
                return NULL;
            }

            *var_len = strlen(str_value);
            return str_value;

        case hhrPortArpConfigLearnEnable:
            switch (pdata->arp_disable)
            {
                case ARP_DISABLE_DEF: //enable
                    int_value = 1;
                    break;

                default:
                    int_value = 2;
                    break;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrPortArpConfigLearnNumLimit:
            int_value = pdata->arp_num_max ;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrPortArpConfigProxyEnable:
            switch (pdata->arp_proxy)
            {
                case ARP_PROXY_DEF:
                    int_value = 2;
                    break;

                default:
                    int_value = 1;
                    break;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

/*** arpstatistics start *************************************************************/
u_char *arpstatistics_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ftm_arp_count *pdata;
    int datanum = 0 ;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    pdata = route_com_get_arp_statis(MODULE_ID_SNMPD , 0 , &datanum);

    if (NULL == pdata)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrArpStatisticsTotalNum:
            uint_value = pdata->arp_total_num;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrArpStatisticsStaticArpNum:
            uint_value = pdata->arp_static_num;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrArpStatisticsCompleteArpNum:
            uint_value = pdata->arp_complete_num;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrArpStatisticsIncompleteArpNum:
            uint_value = pdata->arp_incomplete_num;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrArpStatisticsAgedArpNum:
            uint_value = pdata->arp_aged_num;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        default :
            return NULL ;
    }
	mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
	
    return NULL;
}



/*** hhrISISInstanceTable start ***********************************************/
struct ipc_mesg_n *isis_com_get_instance_bulk(uint32_t instance, int module_id, int *pdata_num)
{
    uint32_t instance_temp = instance;
    //struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(&instance_temp, sizeof(uint32_t), 1 , MODULE_ID_ISIS, module_id,
      //                                                  IPC_TYPE_ISIS, 0, IPC_OPCODE_GET_BULK, 0);

	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&instance_temp, sizeof(uint32_t), 1 ,
				 				MODULE_ID_ISIS, module_id,IPC_TYPE_ISIS, 0, IPC_OPCODE_GET_BULK, 0,5000);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return pmesg;
    }

    return NULL;
}

int hhrISISInstanceTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                           struct isis_instance_entry *index_input)
{
    struct isis_instance_entry *pdata = NULL;
	struct ipc_mesg_n *pmesg = NULL;
    int data_len = sizeof(struct isis_instance_entry);
    int data_num = 0;
    int i;

    pmesg = isis_com_get_instance_bulk(index_input->instance, MODULE_ID_SNMPD, &data_num);
	pdata = (struct isis_instance_entry*)pmesg->msg_data;
	
    if (0 == data_num || NULL == pdata)
    {

    	mem_share_free(pmesg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
    }
	mem_share_free(pmesg, MODULE_ID_SNMPD);
    return TRUE;
}

struct isis_instance_entry *hhrISISInstanceTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                             int exact,
                                                             const struct isis_instance_entry *index_input)
{
    struct listnode            *node;
    struct isis_instance_entry *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->instance)
        {
            return cache->data_list->head->data;
        }

        if (pdata->instance == index_input->instance)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrISISInstanceTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct isis_instance_entry  data_index;
    struct isis_instance_entry  *pdata;
    int                         data_len = sizeof(struct isis_instance_entry);
    u_int32_t                   index_int = 0;
    int                         ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrISISInstanceTable_cache)
    {
        hhrISISInstanceTable_cache = snmp_cache_init(data_len,
                                                     hhrISISInstanceTable_get_data_from_ipc,
                                                     hhrISISInstanceTable_node_lookup);

        if (NULL == hhrISISInstanceTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.instance = index_int;
    pdata = snmp_cache_get_data_by_index(hhrISISInstanceTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->instance);
    }

    switch (vp->magic)
    {
        case hhrISISInstanceLevel:
            int_value = pdata->level;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrISISMetricStyle:
            int_value = pdata->metric_type;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrISISLspGenInterval:
            uint_value = pdata->lsp_gen_interval;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISLspRefreshInterval:
            uint_value = pdata->lsp_refresh;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISLspLifetime:
            uint_value = pdata->max_lsp_lifetime;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISLspLength:
            uint_value = pdata->lsp_mtu;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISSpfInterval:
            uint_value = pdata->spf_interval;
            *var_len = sizeof(u_int32_t);
            return (u_char *)&uint_value;

        case hhrISISOverloadBit:

            if (4 == pdata->overloadbit)
            {
                uint_value = 1;
            }
            else
            {
                uint_value = 2;
            }

            *var_len = sizeof(u_int32_t);
            return (u_char *)&uint_value;

        case hhrISISAttachedBit:
            uint_value = pdata->attachedbit;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISHelloPadding:
            uint_value = pdata->hellopadding;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISDistanceValue:
            uint_value = pdata->distance;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISLevel1ToLevel2RoutePenetration:
            uint_value = pdata->level1_to_level2;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISLevel2ToLevel1RoutePenetration:
            uint_value = pdata->level2_to_level1;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        default:
            return NULL;
    }

    return NULL;
}

/*** hhrISISInstanceNetEntityTable start **************************************/
struct ipc_mesg_n *isis_com_get_netentity_bulk(struct isis_netentity_entry *isis_net, int module_id, int *pdata_num)
{
   // struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(isis_net, sizeof(struct isis_netentity_entry), 1 , MODULE_ID_ISIS, module_id,
     //                                                    IPC_TYPE_ISIS, 2, IPC_OPCODE_GET_BULK, 0);
	
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(isis_net, sizeof(struct isis_netentity_entry), 1 , MODULE_ID_ISIS, module_id,
                                                         IPC_TYPE_ISIS, 2, IPC_OPCODE_GET_BULK, 0,5000);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return pmesg;
    }

    return NULL;
}

int hhrISISInstanceNetEntityTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                                    struct isis_netentity_entry *index_input)
{
    struct isis_netentity_entry *pdata = NULL;
	struct ipc_mesg_n *pmesg = NULL;
    int data_len = sizeof(struct isis_netentity_entry);
    int data_num = 0;
    int i;

    pmesg = isis_com_get_netentity_bulk(index_input, MODULE_ID_SNMPD, &data_num);
	pdata = (struct isis_netentity_entry *)pmesg->msg_data;

    if (0 == data_num || NULL == pdata)
    {
		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
    }
	mem_share_free(pmesg, MODULE_ID_SNMPD);
    return TRUE;
}

struct isis_netentity_entry *hhrISISInstanceNetEntityTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                       int exact,
                                                                       const struct isis_netentity_entry *index_input)
{
    struct listnode                *node;
    struct isis_netentity_entry    *pdata = NULL;
    int                             len;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->instance)
        {
            return cache->data_list->head->data;
        }

        // FIXME
        len = (index_input->netentity_len > pdata->netentity_len) ? pdata->netentity_len : index_input->netentity_len;

        if ((pdata->instance == index_input->instance) \
                && (0 == memcmp(pdata->netentity, index_input->netentity, len)))
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrISISInstanceNetEntityTable_get(struct variable *vp,
                                          oid *name,
                                          size_t *length,
                                          int exact, size_t *var_len, WriteMethod **write_method)
{
    struct isis_netentity_entry     data_index;
    struct isis_netentity_entry     *pdata;
    int                             data_len = sizeof(struct isis_netentity_entry);
    u_int32_t                       index_int = 0;
    int                             ret = 0;
    int                             i, j, len;
    u_char                          buff[20];

    *write_method = NULL;

    memset(buff, 0, sizeof(buff));
    memset(&data_index, 0, data_len);
    ret = ipran_snmp_int_implicitstr_index_get(vp, name, length, &data_index.instance, buff, 20, exact);

    if (ret < 0)
    {
        return NULL;
    }

    len     = 0;

    for (i = 0; i < 20; ++i)
    {
        if (0 != buff[i])
        {
            break;
        }
    }

    for (j = i; j < 20; ++j)
    {
        data_index.netentity[len++] = buff[j];
    }

    data_index.netentity_len = len;

    if (len < 8 && len != 0)
    {
        return NULL;
    }

    if (NULL == hhrISISInstanceNetEntityTable_cache)
    {
        hhrISISInstanceNetEntityTable_cache = snmp_cache_init(data_len,
                                                              hhrISISInstanceNetEntityTable_get_data_from_ipc,
                                                              hhrISISInstanceNetEntityTable_node_lookup);

        if (NULL == hhrISISInstanceNetEntityTable_cache)
        {
            return NULL;
        }
    }

    pdata = snmp_cache_get_data_by_index(hhrISISInstanceNetEntityTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        memset(buff, 0, 20);
        len = pdata->netentity_len;

        if (len < 8 || len > 20)
        {
            return NULL;
        }

        for (i = 0; i < len; ++i)
        {
            buff[20 - len + i] = pdata->netentity[i];
        }

        ipran_snmp_int_implicitstr_index_set(vp, name, length, pdata->instance, buff, 20);
    }

    switch (vp->magic)
    {
        case hhrISISInstanceNetEntityReservedField:
            uint_value = 0;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        default:
            return NULL;
    }

    return NULL;
}

/*** hhrISISInstanceIfTable start *********************************************/
struct ipc_mesg_n *isis_com_get_interface_bulk(uint32_t ifindex, int module_id, int *pdata_num)
{
    uint32_t ifindex_temp = ifindex;
    //struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(&ifindex_temp, sizeof(uint32_t), 1 , MODULE_ID_ISIS, module_id,
      //                                                   IPC_TYPE_ISIS, 1, IPC_OPCODE_GET_BULK, 0);
	
	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&ifindex_temp, sizeof(uint32_t), 1 , MODULE_ID_ISIS, module_id,
															 IPC_TYPE_ISIS, 1, IPC_OPCODE_GET_BULK, 0,5000);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return pmesg;
    }

    return NULL;
}

int hhrISISInstanceIfTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                             struct isis_circuit_entry *index_input)
{
    struct isis_circuit_entry *pdata = NULL;
	struct ipc_mesg_n *pmesg = NULL;
    int data_len = sizeof(struct isis_circuit_entry);
    int data_num = 0;
    int i;

    pmesg = isis_com_get_interface_bulk(index_input->ifindex, MODULE_ID_SNMPD, &data_num);
	pdata = (struct isis_circuit_entry *)pmesg->msg_data;

    if (0 == data_num || NULL == pdata)
    {

		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
    }
	mem_share_free(pmesg, MODULE_ID_SNMPD);
    return TRUE;
}

struct isis_circuit_entry *hhrISISInstanceIfTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                              int exact,
                                                              const struct isis_circuit_entry *index_input)
{
    struct listnode            *node;
    struct isis_circuit_entry  *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->ifindex)
        {
            return cache->data_list->head->data;
        }

        if (pdata->ifindex == index_input->ifindex)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrISISInstanceIfTable_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    struct isis_circuit_entry   data_index;
    struct isis_circuit_entry   *pdata;
    int                         data_len = sizeof(struct isis_circuit_entry);
    u_int32_t                   index_int = 0;
    int                         ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrISISInstanceIfTable_cache)
    {
        hhrISISInstanceIfTable_cache = snmp_cache_init(data_len,
                                                       hhrISISInstanceIfTable_get_data_from_ipc,
                                                       hhrISISInstanceIfTable_node_lookup);

        if (NULL == hhrISISInstanceIfTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.ifindex = index_int;
    pdata = snmp_cache_get_data_by_index(hhrISISInstanceIfTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->ifindex);
    }

    switch (vp->magic)
    {
        case hhrISISIfInstanceID:
            uint_value = pdata->instance;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISIfDesc:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->name, sizeof(pdata->name));
            *var_len = strlen(str_value);
            return str_value;

        case hhrISISIfPassive:
            int_value = pdata->passive + 1;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrISISIfNetworkType:
            int_value = pdata->circuit_type;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrISISIfCircuitType:
            int_value = pdata->circuit_level;   //level
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrISISIfAuthType:
            int_value = pdata->password_type;

            if (54 == int_value)
            {
                int_value = 2;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrISISIfAuthPwd:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->passwd, 255);
            *var_len = strlen(str_value);
            return str_value;

        case hhrISISIfPriority:
            uint_value = pdata->priority;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISIfMetric:
            uint_value = pdata->metric;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISIfHelloInterval:
            uint_value = pdata->hello_interval;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISIfHelloMultiplier:
            uint_value = pdata->hello_multiplier;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISIfCsnpInterval:
            uint_value = pdata->csnp_interval;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        case hhrISISIfPsnpInterval:
            uint_value = pdata->psnp_interval;
            *var_len = sizeof(u_int32);
            return (u_char *)&uint_value;

        default:
            return NULL;
    }

    return NULL;
}

/*** hhrBGPInstanceIfTable start **********************************************/
struct bgp_as_key *bgp_com_get_as_info(int module_id)
{
    struct ipc_mesg_n* pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_BGP, module_id,
                                       IPC_TYPE_BGP, BGP_SNMP_GET_AS, IPC_OPCODE_GET, 0, 2000);
	 if (NULL != pmesg)
    {
        return (struct bgp_as_key *)pmesg->msg_data;
    }
   
    return NULL;
}

/* hhrBGPInstanceIfTable */
u_char *hhrBGPInstanceIfTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    struct bgp_as_key   *pBGPInstanceIfEntry = NULL;
    char ip_addr[INET_ADDRSTRLEN] = "";
    u_int32_t index = 0;
    u_int32_t index_next = 0;
    int ret = 0;
    time_t time_now = 0;
    static time_t getempty_time;    //Save the query to empty table time
    time(&time_now);

    zlog_debug(SNMP_DBG_MIB_GET,  "%s[%d] enter hhrBGPInstanceIfTable_get exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    pBGPInstanceIfEntry = bgp_com_get_as_info(MODULE_ID_SNMPD);

    if (NULL == pBGPInstanceIfEntry)
    {
        time(&getempty_time);
        zlog_debug(SNMP_DBG_MIB_GET,  "%s[%d]: in function '%s' -> no bgp data return, read over\n", \
                   __FILE__, __LINE__, __func__);
        return NULL;
    }

    if (1 == exact)
    {
        if (index != pBGPInstanceIfEntry->as)
        {
            return NULL;
        }
    }
    else
    {
        if (index >= pBGPInstanceIfEntry->as)
        {
            return NULL;
        }

        index = pBGPInstanceIfEntry->as;
        ipran_snmp_int_index_set(vp, name, length, index);
    }

    switch (vp->magic)
    {
        case hhrBGPRouterId:
            ip_value = pBGPInstanceIfEntry->router_id.s_addr;
            *var_len = sizeof(int);
            return (u_char *)&ip_value;

        case hhrBGPASPATH:
            int_value = pBGPInstanceIfEntry->as_path;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPKeepalive:
            int_value = pBGPInstanceIfEntry->keepalive;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPHoldTime:
            int_value = pBGPInstanceIfEntry->holdtime;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPRetryTime:
            int_value = pBGPInstanceIfEntry->retrytime;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPLocalPreference:
            uint_value = pBGPInstanceIfEntry->local_pref;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        case hhrBGPDistanceEbgpPri:
            int_value = pBGPInstanceIfEntry->distance_ebgp;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPDistanceIbgpPri:
            int_value = pBGPInstanceIfEntry->distance_ibgp;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPDampeningEnable:
            int_value = pBGPInstanceIfEntry->damped;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPDistanceLocalPri:
            int_value = pBGPInstanceIfEntry->distance_local;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPDampeningHalfLife:
            int_value = (int)pBGPInstanceIfEntry->half_life;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPDampeningReusethreshold:
            int_value = pBGPInstanceIfEntry->reuse_limit;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPDampeningSuppressionThreshold:
            int_value = pBGPInstanceIfEntry->suppress_value;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPDampeningMaxSuppressionTime:
            int_value = pBGPInstanceIfEntry->max_suppress_time;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPMEDCompareDifferentAs:
            int_value = pBGPInstanceIfEntry->med_diff;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPMEDCompareDeterministic:
            int_value = pBGPInstanceIfEntry->med_deter;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPMED:
            uint_value = pBGPInstanceIfEntry->med;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        default:
            return NULL;
    }

	mem_share_free_bydata(pBGPInstanceIfEntry, MODULE_ID_SNMPD);
    return NULL;
}

/*** hhrBGPRedistributeTable start ********************************************/
struct ipc_mesg_n *bgp_com_get_redistribute_info(struct bgp_redistribute_key *bgp_redistribute, int module_id)
{
    struct ipc_mesg_n *pmesg = NULL;

    if (bgp_redistribute == NULL)
    {
        pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_BGP,
                                            module_id, IPC_TYPE_BGP, BGP_SNMP_GET_REDISTRIBUTE , IPC_OPCODE_GET, 0, 2000);
    }
    else
    {
        pmesg = ipc_sync_send_n2(bgp_redistribute, sizeof(struct bgp_peer_key), 1, MODULE_ID_BGP,
                                            module_id, IPC_TYPE_BGP, BGP_SNMP_GET_REDISTRIBUTE , IPC_OPCODE_GET, 0, 2000);
    }

    if (NULL != pmesg)
    {
        return  pmesg;
    }

    return NULL;
}

int hhrBGPRedistributeTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                              struct bgp_redistribute_key *index_input)
{
    struct bgp_redistribute_key *pdata = NULL;
	struct ipc_mesg_n *pmsg = NULL;
	
    int data_len = sizeof(struct bgp_redistribute_key);
    int data_num = 0;
    int i;

    pmsg = bgp_com_get_redistribute_info(index_input, MODULE_ID_SNMPD);

	if (NULL == pmsg)
    {
        return FALSE;
    }
	
    data_num = pmsg->msghdr.data_num;
	if(0 == data_num)
	{   
	    mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE; 
	}
     
    pdata = (struct bgp_redistribute_key *)pmsg->msg_data;
    for (i = 0; i < data_num; i++)
    {
        snmp_cache_add(cache, pdata, data_len);
        pdata++;
    }
   
	mem_share_free(pmsg, MODULE_ID_SNMPD);
    return TRUE;    
}

struct bgp_redistribute_key *hhrBGPRedistributeTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                                 int exact,
                                                                 const struct bgp_redistribute_key *index_input)
{
    struct listnode                *node;
    struct bgp_redistribute_key    *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->as
                && 0 == index_input->vrf_id
                && 0 == index_input->redist_type
                && 0 == index_input->redist_instance)
        {
            return cache->data_list->head->data;
        }

        if (pdata->as == index_input->as
                && pdata->vrf_id == index_input->vrf_id
                && pdata->redist_type == index_input->redist_type
                && pdata->redist_instance == index_input->redist_instance)

        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrBGPRedistributeTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct bgp_redistribute_key data_index;
    struct bgp_redistribute_key *pdata;
    int                         data_len = sizeof(struct bgp_redistribute_key);
    u_int32_t                   as = 0;
    u_int32_t                   vrf_id = 0;
    u_int32_t                   redist_type = 0;
    u_int32_t                   redist_instance = 0;
    int                         ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_intx4_index_get(vp, name, length, &as, &vrf_id, &redist_type, &redist_instance, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrBGPRedistributeTable_cache)
    {
        hhrBGPRedistributeTable_cache = snmp_cache_init(data_len,
                                                        hhrBGPRedistributeTable_get_data_from_ipc,
                                                        hhrBGPRedistributeTable_node_lookup);

        if (NULL == hhrBGPRedistributeTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.as = as;
    data_index.vrf_id = vrf_id;
    data_index.redist_type = redist_type;
    data_index.redist_instance = redist_instance;

    pdata = snmp_cache_get_data_by_index(hhrBGPRedistributeTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_intx4_index_set(vp, name, length, pdata->as, pdata->vrf_id,
                                   pdata->redist_type, pdata->redist_instance);
    }

    switch (vp->magic)
    {
        case hhrBGPRedistributeMetric:
            uint_value = pdata->redist_metric;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        default:
            return NULL;
    }

    return NULL;
}

/*** hhrBGPNeighborTable start ************************************************/
struct ipc_mesg_n *bgp_com_get_peer_info(struct bgp_peer_key *bgp_peer, int module_id)
{
    struct ipc_mesg_n *pmesg ;

    if (bgp_peer == NULL)
    {
        pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_BGP,
                                            module_id, IPC_TYPE_BGP, BGP_SNMP_GET_PEER, IPC_OPCODE_GET, 0, 2000);
    }
    else
    {
        pmesg = ipc_sync_send_n2(bgp_peer, sizeof(struct bgp_peer_key), 1, MODULE_ID_BGP,
                                            module_id, IPC_TYPE_BGP, BGP_SNMP_GET_PEER, IPC_OPCODE_GET, 0, 2000);
    }

    if (NULL != pmesg)
    {
        return  pmesg;
    }

    return NULL;
}

int hhrBGPNeighborTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                          struct bgp_peer_key *index_input)
{
    struct bgp_peer_key *pdata = NULL;
	struct ipc_mesg_n *pmsg = NULL;
    int data_len = sizeof(struct bgp_peer_key);
    int data_num = 0;
    int i;

    pmsg = bgp_com_get_peer_info(index_input, MODULE_ID_SNMPD);

    if ( NULL == pmsg)
    {
        return FALSE;
    }

	data_num = pmsg->msghdr.data_num;
    if(0 == data_num)
    {  
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
	}

	pdata = (struct bgp_peer_key *)pmsg->msg_data;
    for (i = 0; i < data_num; i++)
    {
        snmp_cache_add(cache, pdata, data_len);
        pdata++;
    }

    mem_share_free(pmsg, MODULE_ID_SNMPD);
    return TRUE;
}

struct bgp_peer_key *hhrBGPNeighborTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                     int exact,
                                                     const struct bgp_peer_key *index_input)
{
    struct listnode        *node;
    struct bgp_peer_key    *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->local_as
                && 0 == index_input->remote_ip.s_addr)
        {
            return cache->data_list->head->data;
        }

        if (pdata->local_as == index_input->local_as
                && pdata->remote_ip.s_addr == index_input->remote_ip.s_addr)

        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrBGPNeighborTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    struct bgp_peer_key data_index;
    struct bgp_peer_key *pdata;
    int                 data_len = sizeof(struct bgp_peer_key);
    int                 ret = 0;

    *write_method = NULL;

    memset(&data_index, 0, data_len);

    ret = ipran_snmp_int_ip_index_get(vp, name, length, &data_index.local_as, (u_int32_t *) & data_index.remote_ip.s_addr, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrBGPNeighborTable_cache)
    {
        hhrBGPNeighborTable_cache = snmp_cache_init(data_len,
                                                    hhrBGPNeighborTable_get_data_from_ipc,
                                                    hhrBGPNeighborTable_node_lookup);

        if (NULL == hhrBGPNeighborTable_cache)
        {
            return NULL;
        }
    }

    pdata = snmp_cache_get_data_by_index(hhrBGPNeighborTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_ip_index_set(vp, name, length, pdata->local_as, (u_int32_t)pdata->remote_ip.s_addr);
    }

    switch (vp->magic)
    {
        case hhrBGPNeighborRemoteAS:
            uint_value = (u_int32_t)pdata->remote_as;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        case hhrBGPNeighborRemoteDesc:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->desc, sizeof(pdata->desc));
            *var_len = strlen(str_value);
            return str_value;

        case hhrBGPNeighborEnable:
            int_value = pdata->enable;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPNeighborSourceIP:
            ip_value = (u_int32_t)pdata->source_ip.s_addr;
            *var_len = sizeof(int);
            return (u_char *)&ip_value;

        case hhrBGPNeighborEbgpMultihop:
            int_value = pdata->ttl_enable;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPNeighborEbgpMultihopTTL:
            uint_value = (u_int32_t)pdata->ttl;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        case hhrBGPNeighborEbgpLocalAS:
            uint_value = (u_int32_t)pdata->change_local_as;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        case hhrBGPNeighborRouteSendDefault:
            int_value = pdata->send_default;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPNeighborRouteSendIBGP:
            int_value = pdata->send_ibgp;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPNeighborRouteLimit:
            uint_value = (u_int32_t)pdata->route_max;   // FIXME
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        case hhrBGPNeighborRouteWeight:
            uint_value = pdata->weight;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        case hhrBGPNeighborRouteNextHopChangeLocal:
            int_value = pdata->nexthop_change_local;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPNeighborAdvertiseInterval:
            uint_value = pdata->routeadv;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        case hhrBGPNeighborAsPathAllowAsLoop:
            uint_value = pdata->allow_in;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        case hhrBGPNeighborAsPathExcludePrivateAs:
            int_value = pdata->exclude_private_as;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPNeighborIpv4FamilyVpnv4:
            int_value = pdata->vpnv4_enable;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPNeighborIpv4FamilyVpn:
            uint_value = pdata->vrf_id;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        case hhrBGPNeighborBindingTunnelIfIndex:
            uint_value = pdata->ifindex_tnl;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        case hhrBGPNeighborBindingTunnelIfDescr:
            memset(str_value, 0, sizeof(str_value));
            ifm_get_name_by_ifindex(pdata->ifindex_tnl, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrBGPNeighborStatus:
            int_value = pdata->negr_status;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPNeighborMd5Password:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->md5_password, sizeof(pdata->md5_password));
            *var_len = strlen(str_value);
            return str_value;

        default:
            return NULL;
    }

    return NULL;
}

struct ipc_mesg_n *bgp_com_get_publish_info(struct bgp_publish_key *bgp_publish, int module_id)
{
    struct ipc_mesg_n *pmesg ;

    if (bgp_publish == NULL)
    {
        pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_BGP,
                                            module_id, IPC_TYPE_BGP, BGP_SNMP_GET_PUBLISH, IPC_OPCODE_GET, 0, 2000);
    }
    else
    {
        pmesg = ipc_sync_send_n2(bgp_publish, sizeof(struct bgp_publish_key), 1, MODULE_ID_BGP,
                                            module_id, IPC_TYPE_BGP, BGP_SNMP_GET_PUBLISH, IPC_OPCODE_GET, 0, 2000);
    }

    if (NULL != pmesg)
    {
        return  pmesg;
    }

    return NULL;
}

int hhrBGPPublishTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                         struct bgp_publish_key *index_input)
{
    struct bgp_publish_key *pdata = NULL;
	struct ipc_mesg_n *pmsg = NULL;
    int data_len = sizeof(struct bgp_publish_key);
    int data_num = 0;
    int i;

    pmsg = bgp_com_get_publish_info(index_input, MODULE_ID_SNMPD);

    if ( NULL == pmsg)
    {
        return FALSE;
    }

    data_num = pmsg->msghdr.data_num;
	if(0 == data_num)
	{   
	    mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
	}

	pdata = (struct bgp_publish_key *)pmsg->msg_data;
    for (i = 0; i < data_num; i++)
    {
        snmp_cache_add(cache, pdata, data_len);
        pdata++;
    }

	mem_share_free(pmsg, MODULE_ID_SNMPD);
    return TRUE;
}

struct bgp_publish_key *hhrBGPPublishTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                       int exact,
                                                       const struct bgp_publish_key *index_input)
{
    struct listnode        *node;
    struct bgp_publish_key    *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->local_as
                && 0 == index_input->p.u.prefix4.s_addr && 0 == index_input->router_mask.s_addr)
        {
            return cache->data_list->head->data;
        }

        if (pdata->local_as == index_input->local_as
                && pdata->p.u.prefix4.s_addr == index_input->p.u.prefix4.s_addr && pdata->router_mask.s_addr == index_input->router_mask.s_addr)

        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrBGPPublishTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    struct bgp_publish_key data_index;
    struct bgp_publish_key *pdata;
    int                    data_len = sizeof(struct bgp_publish_key);
    int                    ret = 0;

    *write_method = NULL;
    memset(&data_index, 0, data_len);
    ret = ipran_snmp_int_ipx2_index_get(vp, name, length, &data_index.local_as, (u_int32_t *)&data_index.p.u.prefix4.s_addr, (u_int32_t *) &data_index.router_mask.s_addr, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrBGPPublishTable_cache)
    {
        hhrBGPPublishTable_cache = snmp_cache_init(data_len,
                                                   hhrBGPPublishTable_get_data_from_ipc,
                                                   hhrBGPPublishTable_node_lookup);

        if (NULL == hhrBGPPublishTable_cache)
        {
            return NULL;
        }
    }

    pdata = snmp_cache_get_data_by_index(hhrBGPPublishTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_ipx2_index_set(vp, name, length, pdata->local_as, (u_int32_t)pdata->p.u.prefix4.s_addr, (u_int32_t)pdata->router_mask.s_addr);
    }

    switch (vp->magic)
    {
        case hhrBGPPublishRoutesReservedField:
            uint_value = pdata->reserved;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        default:
            return NULL;
    }

    return NULL;
}


struct ipc_mesg_n *bgp_com_get_summary_info(struct bgp_summary_key *bgp_summary, int module_id)
{
    struct ipc_mesg_n *pmesg = NULL ;

    if (bgp_summary == NULL)
    {
        pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_BGP,
                                            module_id, IPC_TYPE_BGP, BGP_SNMP_GET_SUMMARY, IPC_OPCODE_GET, 0, 2000);
    }
    else
    {
        pmesg = ipc_sync_send_n2(bgp_summary, sizeof(struct bgp_summary_key), 1, MODULE_ID_BGP,
                                            module_id, IPC_TYPE_BGP, BGP_SNMP_GET_SUMMARY, IPC_OPCODE_GET, 0, 2000);
    }

    if (NULL != pmesg)
    {
        return  pmesg;
    }

    return NULL;
}

int hhrBGPSummaryTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                         struct bgp_summary_key *index_input)
{
    struct bgp_summary_key *pdata = NULL;
	struct ipc_mesg_n *pmesg = NULL ;
    int data_len = sizeof(struct bgp_summary_key);
    int data_num = 0;
    int i;

    pmesg = bgp_com_get_summary_info(index_input, MODULE_ID_SNMPD);

    if ( NULL == pmesg)
    {
        return FALSE;
    }

	data_num = pmesg->msghdr.data_num;   
	if ( 0 == data_num)
	{   
	    mem_share_free(pmesg, MODULE_ID_SNMPD);
		return FALSE;
	}

	pdata = ( struct bgp_summary_key *)pmesg->msg_data;
    for (i = 0; i < data_num; i++)
    {
        snmp_cache_add(cache, pdata, data_len);
        pdata++;
    }

	mem_share_free(pmesg, MODULE_ID_SNMPD);
    return TRUE;
}

struct bgp_summary_key *hhrBGPSummaryTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                       int exact,
                                                       const struct bgp_summary_key *index_input)
{
    struct listnode        *node;
    struct bgp_summary_key    *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->local_as
                && 0 == index_input->p.u.prefix4.s_addr && 0 == index_input->router_mask.s_addr)
        {
            return cache->data_list->head->data;
        }

        if (pdata->local_as == index_input->local_as
                && pdata->p.u.prefix4.s_addr == index_input->p.u.prefix4.s_addr && pdata->router_mask.s_addr == index_input->router_mask.s_addr)

        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrBGPSummaryTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    struct bgp_summary_key data_index;
    struct bgp_summary_key *pdata;
    int                    data_len = sizeof(struct bgp_summary_key);
    int                    ret = 0;

    memset(&data_index, 0, data_len);
    ret = ipran_snmp_int_ipx2_index_get(vp, name, length, &data_index.local_as, (u_int32_t *)&data_index.p.u.prefix4.s_addr, (u_int32_t *) &data_index.router_mask.s_addr, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrBGPSummaryTable_cache)
    {
        hhrBGPSummaryTable_cache = snmp_cache_init(data_len,
                                                   hhrBGPSummaryTable_get_data_from_ipc,
                                                   hhrBGPSummaryTable_node_lookup);

        if (NULL == hhrBGPSummaryTable_cache)
        {
            return NULL;
        }
    }

    pdata = snmp_cache_get_data_by_index(hhrBGPSummaryTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_ipx2_index_set(vp, name, length, pdata->local_as, (u_int32_t)pdata->p.u.prefix4.s_addr, (u_int32_t)pdata->router_mask.s_addr);
    }

    switch (vp->magic)
    {
        case hhrBGPSummaryasSet:
            int_value = pdata->as_set;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPSummarySummaryOnly:
            int_value = pdata->summary_only;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}


struct ipc_mesg_n *bgp_com_get_route_info(struct bgp_route_key *bgp_route, int module_id)
{
    struct ipc_mesg_n *pmesg = NULL;

    if (bgp_route == NULL)
    {
        pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_BGP,
                                            module_id, IPC_TYPE_BGP, BGP_SNMP_GET_ROUTES, IPC_OPCODE_GET, 0, 2000);
    }
    else
    {
        pmesg = ipc_sync_send_n2(bgp_route, sizeof(struct bgp_route_key), 1, MODULE_ID_BGP,
                                            module_id, IPC_TYPE_BGP, BGP_SNMP_GET_ROUTES, IPC_OPCODE_GET, 0, 2000);
    }

    if (NULL != pmesg)
    {
        return  pmesg;
    }

    return NULL;
}

int hhrBGPRouteTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                       struct bgp_route_key *index_input)
{
    struct bgp_route_key *pdata = NULL;
	struct ipc_mesg_n *pmesg = NULL;
    int data_len = sizeof(struct bgp_route_key);
    int data_num = 0;
    int i;

    pmesg = bgp_com_get_route_info(index_input, MODULE_ID_SNMPD);

    if ( NULL == pmesg)
    {
        return FALSE;
    }

	data_num = pmesg->msghdr.data_num;
	if(0 == data_num)
	{   
	    mem_share_free(pmesg, MODULE_ID_SNMPD);
        return FALSE;
	}

    pdata = (struct bgp_route_key *)pmesg->msg_data;
    for (i = 0; i < data_num; i++)
    {
        snmp_cache_add(cache, pdata, data_len);
        pdata++;
    }

	mem_share_free(pmesg, MODULE_ID_SNMPD);
    return TRUE;
}

struct bgp_route_key *hhrBGPRouteTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                   int exact,
                                                   const struct bgp_route_key *index_input)
{
    struct listnode        *node;
    struct bgp_route_key      *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->p.u.prefix4.s_addr && 0 == index_input->router_mask.s_addr
                && 0 == index_input->vpn_id && 0 == index_input->next_hop.s_addr)
        {
            return cache->data_list->head->data;
        }

        if (pdata->p.u.prefix4.s_addr == index_input->p.u.prefix4.s_addr && pdata->router_mask.s_addr == index_input->router_mask.s_addr
                && pdata->vpn_id == index_input->vpn_id && pdata->next_hop.s_addr == index_input->next_hop.s_addr)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrBGPRouteTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct bgp_route_key data_index;
    struct bgp_route_key *pdata;
    int                  data_len = sizeof(struct bgp_route_key);
    int                  ret = 0;
    *write_method = NULL;
    memset(&data_index, 0, data_len);
    ret = ipran_snmp_ipx2_int_ip_index_get(vp, name, length, (u_int32_t *)&data_index.p.u.prefix4.s_addr, (u_int32_t *) &data_index.router_mask.s_addr,
                                           &data_index.vpn_id, (u_int32_t *)&data_index.next_hop.s_addr, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrBGPRouteTable_cache)
    {
        hhrBGPRouteTable_cache = snmp_cache_init(data_len,
                                                 hhrBGPRouteTable_get_data_from_ipc,
                                                 hhrBGPRouteTable_node_lookup);

        if (NULL == hhrBGPRouteTable_cache)
        {
            return NULL;
        }
    }

    pdata = snmp_cache_get_data_by_index(hhrBGPRouteTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_ipx2_int_ip_index_set(vp, name, length, (u_int32_t)pdata->p.u.prefix4.s_addr, (u_int32_t)pdata->router_mask.s_addr, pdata->vpn_id, (u_int32_t)pdata->next_hop.s_addr);
    }

    switch (vp->magic)
    {
        case hhrBGPRouteTableMetricValue:
            uint_value = pdata->metric;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        case hhrBGPRouteTablePreference:
            int_value = pdata->preference;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrBGPRouteTableMed:
            uint_value = pdata->med;
            *var_len = sizeof(int);
            return (u_char *)&uint_value;

        default:
            return NULL;
    }

    return NULL;
}


/*** hhrOSPFDCNConfigTable start **********************************************/
int hhrOSPFDCNConfigTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                            struct ospf_dcn_config *index_input)
{
    struct ipc_mesg_n            *pmesg = NULL;
    unsigned int                ospf_id = 0;
    struct ospf_dcn_config     *pdata = NULL;
    int data_len = sizeof(struct ospf_dcn_config);
    int data_num = 0;

    /*pmesg = ipc_send_common_wait_reply1(&ospf_id, sizeof(uint32_t), 1, MODULE_ID_OSPF,
                        MODULE_ID_SNMPD, IPC_TYPE_OSPF, OSPF_SNMP_DCN_CONF_GET, IPC_OPCODE_GET_BULK, 0);*/
	struct ipc_mesg_n *pMsgRcv = hhrOSPF_ALL_Table_get_bulk(&ospf_id, OSPF_SNMP_DCN_CONF_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct ospf_dcn_config *)pMsgRcv->msg_data;

    if (data_num != 0 && NULL != pdata)
    {
        snmp_cache_add(cache, pdata, data_len);
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
    return FALSE;
}

struct ospf_dcn_config *hhrOSPFDCNConfigTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                          int exact,
                                                          const struct ospf_dcn_config *index_input)
{
    struct listnode        *node;
    struct ospf_dcn_config *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->ospf_id)
        {
            return cache->data_list->head->data;
        }

        if (1 == exact)
        {
            return pdata;
        }

        return NULL;
    }

    return NULL;
}

u_char *hhrOSPFDCNConfigTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ospf_dcn_config data_index;
    struct ospf_dcn_config *pdata;
    int                    data_len = sizeof(struct ospf_dcn_config);
    int                    ret = 0;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    if (NULL == hhrOSPFDCNConfigTable_cache)
    {
        hhrOSPFDCNConfigTable_cache = snmp_cache_init(data_len,
                                                      hhrOSPFDCNConfigTable_get_data_from_ipc,
                                                      hhrOSPFDCNConfigTable_node_lookup);

        if (NULL == hhrOSPFDCNConfigTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    pdata = snmp_cache_get_data_by_index(hhrOSPFDCNConfigTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        memset(&data_index, 0, data_len);
        data_index.ospf_dcn_enable = DISABLE;
        pdata = &data_index;
    }

    switch (vp->magic)
    {
        case hhrOSPFDCNEnable:
            int_value = (DISABLE == pdata->ospf_dcn_enable) ? 1 : 0;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrOSPFDCNTrapReport:
            int_value = (int)pdata->ospf_dcn_trap_enable;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrOSPFDCNGNeId:
            uint_value = pdata->ospf_dcn_ne_id;
            *var_len = sizeof(u_int32_t);
            return (u_char *)&uint_value;

        case hhrOSPFDCNGNeIP:
            *var_len = sizeof(u_int32_t);
            ip_value = (u_int32_t)pdata->ospf_dcn_ne_ip;
            //ip_value = htonl(ip_value);
            return (u_char *)(&ip_value);

        case hhrOSPFDCNGNeIPMask:
            int_value = (int)pdata->ospf_dcn_ne_ip_prefixlen;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

/*adapt h3c dcn config info lookup*/
u_char *hhIpRanDcnConfigTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ospf_dcn_config data_index;
    struct ospf_dcn_config *pdata;
    struct ipc_mesg            *pmesg = NULL;
    struct in_addr temp_addr;
    int                  data_len = sizeof(struct ospf_dcn_config);
    int                  ret = 0;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    if (NULL == hhIpRanDcnConfigTable_cache)
    {
        hhIpRanDcnConfigTable_cache = snmp_cache_init(data_len,
                                                      hhrOSPFDCNConfigTable_get_data_from_ipc,
                                                      hhrOSPFDCNConfigTable_node_lookup);

        if (NULL == hhIpRanDcnConfigTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    pdata = snmp_cache_get_data_by_index(hhIpRanDcnConfigTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        memset(&data_index, 0, data_len);
        data_index.ospf_dcn_enable = DISABLE;
        pdata = &data_index;
    }

    switch (vp->magic)
    {
        case hhIpRanDcnNeId:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, (char *)(&pdata->ospf_dcn_ne_id), 4);
            *var_len = 4;
            return str_value;

        case hhIpRanDcnNeIpType:
            int_value = 1;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhIpRanDcnNeIp:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, (char *)(&pdata->ospf_dcn_ne_ip), 4);
            *var_len = 4;
            return str_value;

        case hhIpRanDcnMask:
            masklen_to_netip((int)pdata->ospf_dcn_ne_ip_prefixlen, &temp_addr);
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, (char *)(&temp_addr.s_addr), 4);
            *var_len = 4;
            return str_value;

        case hhIpRanDcnMAC:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->mac, 6);
            *var_len = 6;
            return str_value;

        case hhIpRanDcnVendor:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->dcn_ne_vendor, 64);
            *var_len = strlen(str_value);
            return str_value;

        default:
            return NULL;
    }

    return NULL;
}


/*** hhrOSPFDCNNeInfoTable start **********************************************/
#if 0
struct ospf_dcn_ne_info *ospf_dcn_get_ne_info_bulk(uint32_t dcn_ne_id, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = NULL;

    /*pmesg = ipc_send_common_wait_reply1(&dcn_ne_id, sizeof(uint32_t), 1, MODULE_ID_OSPF,
                     module_id, IPC_TYPE_OSPF, OSPF_SNMP_DCN_NE_INFO_GET, IPC_OPCODE_GET_BULK, 0);*/
	pmesg = ipc_sync_send_n2(&dcn_ne_id, sizeof(uint32_t), 1, MODULE_ID_OSPF,
                     module_id, IPC_TYPE_OSPF, OSPF_SNMP_DCN_NE_INFO_GET, IPC_OPCODE_GET_BULK, 0, 2000);

										

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct ospf_dcn_ne_info *)pmesg->msg_data;
    }

    return NULL;
}
#endif

int hhrOSPFDCNNeInfoTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                            struct ospf_dcn_ne_info *index_input)
{

    struct ospf_dcn_ne_info *pdata = NULL;
    int data_len = sizeof(struct ospf_dcn_ne_info);
    int data_num = 0;
    int i;

    struct ipc_mesg_n *pMsgRcv = hhrOSPF_ALL_Table_get_bulk(&(index_input->dcn_ne_id), OSPF_SNMP_DCN_NE_INFO_GET);
	if(NULL == pMsgRcv)
	{
		return FALSE;
	}

	data_num = pMsgRcv->msghdr.data_num;
	pdata = (struct ospf_dcn_ne_info *)pMsgRcv->msg_data;
	
    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct ospf_dcn_ne_info *hhrOSPFDCNNeInfoTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                           int exact,
                                                           const struct ospf_dcn_ne_info *index_input)
{
    struct listnode            *node;
    struct ospf_dcn_ne_info    *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->dcn_ne_id)
        {
            return cache->data_list->head->data;
        }

        if (pdata->dcn_ne_id == index_input->dcn_ne_id)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrOSPFDCNNeInfoTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ospf_dcn_ne_info data_index;
    struct ospf_dcn_ne_info *pdata;
    int                     data_len = sizeof(struct ospf_dcn_ne_info);
    u_int32_t               index_int = 0;
    int                     ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrOSPFDCNNeInfoTable_cache)
    {
        hhrOSPFDCNNeInfoTable_cache = snmp_cache_init(data_len,
                                                      hhrOSPFDCNNeInfoTable_get_data_from_ipc,
                                                      hhrOSPFDCNNeInfoTable_node_lookup);

        if (NULL == hhrOSPFDCNNeInfoTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.dcn_ne_id = index_int;
    pdata = snmp_cache_get_data_by_index(hhrOSPFDCNNeInfoTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->dcn_ne_id);
    }

    switch (vp->magic)
    {
        case hhrOSPFDCNNeIP:
            ip_value = pdata->dcn_ne_ip;
            *var_len = sizeof(int);
            return (u_char *)&ip_value;

        case hhrOSPFDCNMetric:
            int_value = pdata->dcn_ne_metric;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrOSPFDCNDeviceType:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->dcn_ne_device_type, 64);
            *var_len = strlen(str_value);
            return str_value;

        case hhrOSPFDCNNeMac:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->mac, 6);
            *var_len = 6;
            return str_value;

        case hhrOSPFDCNVendor:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->dcn_ne_vendor, 64);
            *var_len = strlen(str_value);
            return str_value;

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhIpRanDcnNeInfoTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ospf_dcn_ne_info data_index;
    struct ospf_dcn_ne_info *pdata;
    int                   data_len = sizeof(struct ospf_dcn_ne_info);
    u_char      index_int[5] = {0};;
    char ipv4_addr[20];
    int                   ret = 0;

    *write_method = NULL;

    //print_oid(name, *length);
    ret = ipran_snmp_octstring_index_get(vp, name, length, index_int, 5, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhIpRanDcnNeInfoTable_cache)
    {
        hhIpRanDcnNeInfoTable_cache = snmp_cache_init(data_len,
                                                      hhrOSPFDCNNeInfoTable_get_data_from_ipc,
                                                      hhrOSPFDCNNeInfoTable_node_lookup);

        if (NULL == hhIpRanDcnNeInfoTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.dcn_ne_id = *(int *)index_int;
    pdata = snmp_cache_get_data_by_index(hhIpRanDcnNeInfoTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)   /* get ready the next index */
    {
        memset(index_int, 0, sizeof(index_int));
        memcpy(index_int, (char *)(&pdata->dcn_ne_id), 4);
        ipran_snmp_octstring_index_set(vp, name, length, index_int, 4);
    }

    switch (vp->magic)
    {
        case hhIpRanDcnNeInfoNeIpType:
            int_value = (int)1;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhIpRanDcnNeInfoNeIp:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, (char *)(&pdata->dcn_ne_ip), 4);
            *var_len = 4;
            return str_value;

        case hhIpRanDcnNeInfoMetric:
            int_value = pdata->dcn_ne_metric;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhIpRanDcnNeInfoDeviceType:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->dcn_ne_device_type, 64);
            *var_len = strlen(str_value);
            return str_value;

        case hhIpRanDcnNeInfoMAC:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->mac, 6);
            *var_len = 6;
            return str_value;

        case hhIpRanDcnNeInfoVendor:
            memset(str_value, 0, sizeof(str_value));
            memcpy(str_value, pdata->dcn_ne_vendor, 64);
            *var_len = strlen(str_value);
            return str_value;

        default:
            return NULL;
    }

    return NULL;
}



