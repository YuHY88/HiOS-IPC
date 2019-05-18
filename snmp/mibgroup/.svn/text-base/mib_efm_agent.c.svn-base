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
#include <lib/msg_ipc.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include <l2/l2_if.h>
#include <l2/l2_snmp.h>
#include <lib/linklist.h>
#include <ifm/ifm_message.h>
#include "ipran_snmp_data_cache.h"
#include "mib_efm_agent.h"
#include <l2/efm/efm_agent_snmp.h>

#include "snmp_config_table.h"

static uchar str_value[STRING_LEN] = {'\0'};
static  uint32_t   int_value = 0;
static  uint8_t   bits_value = 0;
static uchar mac_value[6] = {0};
static uchar bits_num[8] = {0};
static uchar soft_num[25] = {0};

static struct ipran_snmp_data_cache *efm_comm_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_ethpt_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_oltpt_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_oltptalm_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_ethptalm_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_vlan_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_oltvlan_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_ethvlan_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_cpuvlan_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_vlangroup_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_qinq_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_qos_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_perf_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_oltperf_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_ethperf_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_cpuperf_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_oam_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_addr_info_cache = NULL ;
static struct ipran_snmp_data_cache *efm_conver_info_cache = NULL ;

WriteMethod hpmcrtCommInfo_table_set;
WriteMethod hpmcrtOltptInfo_table_set;
WriteMethod hpmcrtEthptInfo_table_set;
WriteMethod hpmcrtVlanMode_table_set;
WriteMethod hpmcrtOltptVlan_table_set;
WriteMethod hpmcrtEthptVlan_table_set;
WriteMethod hpmcrtCpuptVlan_table_set;
WriteMethod hpmcrtVlanGroup_table_set;
WriteMethod hpmcrtQinq_table_set;
WriteMethod hpmcrtQos_table_set;
WriteMethod hpmcrtPerf_table_set ;
WriteMethod hpmcrtOltPerf_table_set;
WriteMethod hpmcrtEthPerf_table_set;
WriteMethod hpmcrtCpuPerf_table_set;
WriteMethod hpmcrtAddrInfo_table_set;



WriteMethod hpmcrtOamMode_table_set;



FindVarMethod hpmcrtCommConfTable_get ;

struct variable2 hpmcrtCommConfTable[] =
{
    {hpmcrtSubType,             ASN_INTEGER, RONLY, hpmcrtCommConfTable_get, 2, {1, 1}},
    {hpmcrtSoftVer,            ASN_OCTET_STR,    RONLY, hpmcrtCommConfTable_get, 2, {1, 3}},
    {hpmcrtEthChNum,           ASN_INTEGER,    RONLY, hpmcrtCommConfTable_get, 2, {1, 4}},
    {hpmcrtEthPtNum,              ASN_INTEGER,    RONLY, hpmcrtCommConfTable_get, 2, {1, 5}},
    {hpmcrtOltPtNum,      ASN_INTEGER,   RONLY, hpmcrtCommConfTable_get, 2, {1, 6}},
    {hpmcrtHwSw,  ASN_INTEGER,    RONLY, hpmcrtCommConfTable_get, 2, {1, 7}},
    {hpmcrtSFI,  ASN_INTEGER,    RONLY, hpmcrtCommConfTable_get, 2, {1, 8}},
    {hpmcrtDataResum, ASN_INTEGER, RWRITE, hpmcrtCommConfTable_get, 2, {1, 9}},
    {hpmcrtLFPCtrl, ASN_INTEGER, RWRITE, hpmcrtCommConfTable_get, 2, {1, 10}},
    {hpmcrtALSCtrl, ASN_INTEGER, RWRITE, hpmcrtCommConfTable_get, 2, {1, 11}},
    {hpmcrtJumbo, ASN_INTEGER, RWRITE, hpmcrtCommConfTable_get, 2, {1, 12}},
    {hpmcrtReboot, ASN_INTEGER, RWRITE, hpmcrtCommConfTable_get, 2, {1, 13}},
    {hpmcrtIpCtrl, ASN_INTEGER, RWRITE, hpmcrtCommConfTable_get, 2, {1, 14}},
    {hpmcrtEthStromfilter, ASN_OCTET_STR, RWRITE, hpmcrtCommConfTable_get, 2, {1, 15}},
    {hpmcrtDHCPclient, ASN_INTEGER, RWRITE, hpmcrtCommConfTable_get, 2, {1, 16}},

    {hpmcrtDipSwitchSta, ASN_OCTET_STR, RONLY, hpmcrtCommConfTable_get, 2, {1, 18}},
    {hpmcrtHostName, ASN_OCTET_STR, RONLY, hpmcrtCommConfTable_get, 2, {1, 19}}
} ;


oid  hpmcrtCommConfTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 1, 1};
int  hpmcrtCommConfTable_oid_len = sizeof(hpmcrtCommConfTable_oid) / sizeof(oid);




FindVarMethod hpmcrtOltPtConfTable_get ;

struct variable2 hpmcrtOltPtConfTable[] =
{
    {hpmcrtOltPtNegCtrl,             ASN_INTEGER, RWRITE, hpmcrtOltPtConfTable_get, 2, {1, 1}},
    {hpmcrtOltPtSpdCtrl ,             ASN_INTEGER,   RWRITE, hpmcrtOltPtConfTable_get, 2, {1, 2}},
    {hpmcrtOltPtDplCtrl ,           ASN_INTEGER,    RWRITE, hpmcrtOltPtConfTable_get, 2, {1, 3}},
    {hpmcrtOltPtDisCtrl,              ASN_INTEGER,    RWRITE, hpmcrtOltPtConfTable_get, 2, {1, 4}},
    {hpmcrtOltPtIngressRateN,      ASN_INTEGER,   RWRITE, hpmcrtOltPtConfTable_get, 2, {1, 6}},
    {hpmcrtOltPtEgressRateN,  ASN_INTEGER,   RWRITE, hpmcrtOltPtConfTable_get, 2, {1, 8}},
    {hpmcrtOltPtFlowCtrl,  ASN_INTEGER,    RWRITE, hpmcrtOltPtConfTable_get, 2, {1, 9}}

} ;


oid hpmcrtOltPtConfTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 1, 2};
int  hpmcrtOltPtConfTable_oid_len = sizeof(hpmcrtOltPtConfTable_oid) / sizeof(oid);


FindVarMethod hpmcrtEthPtConfTable_get ;

struct variable2 hpmcrtEthPtConfTable[] =
{
    { hpmcrtEthPtNegCtrl,             ASN_INTEGER, RWRITE, hpmcrtEthPtConfTable_get, 2, {1, 1}},
    {hpmcrtEthPtSpdCtrl ,             ASN_INTEGER,   RWRITE, hpmcrtEthPtConfTable_get, 2, {1, 2}},
    {hpmcrtEthPtDplCtrl,           ASN_INTEGER,    RWRITE, hpmcrtEthPtConfTable_get, 2, {1, 3}},
    {hpmcrtEthPtDisCtrl ,              ASN_INTEGER,    RWRITE, hpmcrtEthPtConfTable_get, 2, {1, 4}},
    {hpmcrtEthPtIngressRateN,      ASN_INTEGER,   RWRITE, hpmcrtEthPtConfTable_get, 2, {1, 6}},
    { hpmcrtEthPtEgressRateN,  ASN_INTEGER,   RWRITE, hpmcrtEthPtConfTable_get, 2, {1, 8}},
    {hpmcrtEthPtFlowCtrl ,  ASN_INTEGER,    RWRITE, hpmcrtEthPtConfTable_get, 2, {1, 9}}

} ;


oid hpmcrtEthPtConfTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 1, 3};
int  hpmcrtEthPtConfTable_oid_len = sizeof(hpmcrtEthPtConfTable_oid) / sizeof(oid);


FindVarMethod hpmcrtOltPtAlmTable_get ;

struct variable2 hpmcrtOltPtAlmTable[] =
{
    {hpmcrtOltPtNO,             ASN_INTEGER,  RONLY, hpmcrtOltPtAlmTable_get, 2, {1, 1}},
    {hpmcrtOltPtStatInfo,  ASN_OCTET_STR,    RONLY, hpmcrtOltPtAlmTable_get, 2, {1, 2}},
    {hpmcrtOltPtAlmInfo, ASN_OCTET_STR,     RONLY, hpmcrtOltPtAlmTable_get, 2, {1, 3}}

} ;


oid hpmcrtOltPtAlmTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 2, 2};
int  hpmcrtOltPtAlmTable_oid_len = sizeof(hpmcrtOltPtAlmTable_oid) / sizeof(oid);






FindVarMethod  hpmcrtEthPtAlmTable_get ;

struct variable2 hpmcrtEthPtAlmTable[] =
{
    {hpmcrtEthPtNO,   ASN_INTEGER,   RONLY, hpmcrtEthPtAlmTable_get, 2, {1, 1}},
    {hpmcrtEthPtStatInfo, ASN_OCTET_STR,  RONLY, hpmcrtEthPtAlmTable_get, 2, {1, 2}},
    {hpmcrtEthPtAlmInfo,  ASN_OCTET_STR,  RONLY, hpmcrtEthPtAlmTable_get, 2, {1, 3}}

} ;


oid hpmcrtEthPtAlmTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 2, 4};
int  hpmcrtEthPtAlmTable_oid_len = sizeof(hpmcrtEthPtAlmTable_oid) / sizeof(oid);




FindVarMethod  hpmcrtVlanConfTable_get ;

struct variable2 hpmcrtVlanConfTable[] =
{
    { hpmcrtVlanMode,   ASN_INTEGER,   RWRITE, hpmcrtVlanConfTable_get, 2, {1, 1}}


} ;


oid hpmcrtVlanConfTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 3, 1};
int  hpmcrtVlanConfTable_oid_len = sizeof(hpmcrtVlanConfTable_oid) / sizeof(oid);

FindVarMethod  hpmcrtOltPtVlanConfTable_get ;

struct variable2 hpmcrtOltPtVlanConfTable[] =
{
    { hpmcrtOltPtInMode,   ASN_INTEGER,   RWRITE, hpmcrtOltPtVlanConfTable_get, 2, {1, 1}},
    { hpmcrtOltPtPVID,   ASN_INTEGER,   RWRITE, hpmcrtOltPtVlanConfTable_get, 2, {1, 2}},
    { hpmcrtOltPtPrior,   ASN_INTEGER,   RWRITE, hpmcrtOltPtVlanConfTable_get, 2, {1, 3}},
    { hpmcrtOltPtTagMode,   ASN_INTEGER,   RWRITE, hpmcrtOltPtVlanConfTable_get, 2, {1, 4}}

} ;


oid hpmcrtOltPtVlanConfTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 3, 2};
int  hpmcrtOltPtVlanConfTable_oid_len = sizeof(hpmcrtOltPtVlanConfTable_oid) / sizeof(oid);


FindVarMethod  hpmcrtEthPtVlanConfTable_get ;

struct variable2 hpmcrtEthPtVlanConfTable[] =
{
    { hpmcrtEthPtInMode,   ASN_INTEGER,   RWRITE, hpmcrtEthPtVlanConfTable_get , 2, {1, 1}},
    {  hpmcrtEthPtPVID,   ASN_INTEGER,   RWRITE, hpmcrtEthPtVlanConfTable_get , 2, {1, 2}},
    {  hpmcrtEthPtPrior,   ASN_INTEGER,   RWRITE, hpmcrtEthPtVlanConfTable_get , 2, {1, 3}},
    { hpmcrtEthPtTagMode,   ASN_INTEGER,   RWRITE, hpmcrtEthPtVlanConfTable_get , 2, {1, 4}}

} ;


oid hpmcrtEthPtVlanConfTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 3, 3};
int  hpmcrtEthPtVlanConfTable_oid_len = sizeof(hpmcrtEthPtVlanConfTable_oid) / sizeof(oid);



FindVarMethod  hpmcrtCpuPtVlanConfTable_get ;

struct variable2 hpmcrtCpuPtVlanConfTable[] =
{
    { hpmcrtCpuPtInMode,   ASN_INTEGER,   RONLY, hpmcrtCpuPtVlanConfTable_get  , 2, {1, 1}},
    { hpmcrtCpuPtPVID,   ASN_INTEGER,   RONLY, hpmcrtCpuPtVlanConfTable_get  , 2, {1, 2}},
    { hpmcrtCpuPtPrior,   ASN_INTEGER,   RWRITE, hpmcrtCpuPtVlanConfTable_get  , 2, {1, 3}},
    { hpmcrtCpuPtTagMode,   ASN_INTEGER,  RONLY, hpmcrtCpuPtVlanConfTable_get  , 2, {1, 4}}

} ;


oid hpmcrtCpuPtVlanConfTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 3, 4};
int  hpmcrtCpuPtVlanConfTable_oid_len = sizeof(hpmcrtCpuPtVlanConfTable_oid) / sizeof(oid);

FindVarMethod  hpmcrtVlanGroupConfTable_get ;

struct variable2 hpmcrtVlanGroupConfTable[] =
{
    { hpmcrtVlanGroupNO,   ASN_INTEGER,   RWRITE, hpmcrtVlanGroupConfTable_get  , 2, {1, 1}},
    { hpmcrtVlanId,   ASN_INTEGER,   RWRITE, hpmcrtVlanGroupConfTable_get  , 2, {1, 2}},
    { hpmcrtVlanMember, ASN_OCTET_STR,   RWRITE, hpmcrtVlanGroupConfTable_get  , 2, {1, 3}}


} ;


oid hpmcrtVlanGroupConfTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 3, 5};
int  hpmcrtVlanGroupConfTable_oid_len = sizeof(hpmcrtVlanGroupConfTable_oid) / sizeof(oid);
#if 0
FindVarMethod  hpmcrtInvalidTable_get ;

struct variable2 hpmcrtInvalidTable[] =
{
    { hpmcrtInvildvalue,   ASN_INTEGER,   RWRITE, hpmcrtInvalidTable_get  , 0}

} ;


oid hpmcrtInvalidTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 3, 6};
int  hpmcrtInvalidTable_oid_len = sizeof(hpmcrtInvalidTable_oid) / sizeof(oid);
#endif


FindVarMethod  hpmcrtQinQTable_get ;

struct variable2 hpmcrtQinQTable[] =
{
    { hpmcrtQinQDirection,   ASN_INTEGER,   RWRITE, hpmcrtQinQTable_get  , 2, {1, 1}},
    { hpmcrtQinQTPID,   ASN_UNSIGNED,   RWRITE, hpmcrtQinQTable_get , 2, {1, 2}},
    { hpmcrtQinQVid, ASN_INTEGER,   RWRITE, hpmcrtQinQTable_get, 2, {1, 3}},
    { hpmcrtQinQPrior, ASN_INTEGER,   RWRITE, hpmcrtQinQTable_get, 2, {1, 4}}

} ;


oid hpmcrtQinQTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 4, 1};
int  hpmcrtQinQTable_oid_len = sizeof(hpmcrtQinQTable_oid) / sizeof(oid);


FindVarMethod  hpmcrtQosTable_get ;

struct variable2 hpmcrtQosTable[] =
{
    {  hpmcrt1pQos,   ASN_INTEGER,   RWRITE, hpmcrtQosTable_get , 2, {1, 1}}

} ;


oid hpmcrtQosTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 5, 1};
int  hpmcrtQosTable_oid_len = sizeof(hpmcrtQosTable_oid) / sizeof(oid);


FindVarMethod  hpmcrtPerfTable_get ;

struct variable2 hpmcrtPerfTable[] =
{
    { hpmcrtCleanCounter,   ASN_INTEGER,   RWRITE, hpmcrtPerfTable_get  , 2, {1, 1}}

} ;


oid  hpmcrtPerfTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 6, 1};
int   hpmcrtPerfTable_oid_len = sizeof(hpmcrtPerfTable_oid) / sizeof(oid);


FindVarMethod  hpmcrtOltPerfTable_get ;

struct variable2 hpmcrtOltPerfTable[] =
{
    { hpmcrtOltCntType,   ASN_INTEGER,   RWRITE, hpmcrtOltPerfTable_get  , 2, {1, 1}},
    { hpmcrtOltRxPCnt ,  ASN_COUNTER,   RONLY, hpmcrtOltPerfTable_get , 2, {1, 2}},
    { hpmcrtOltTxPCnt, ASN_COUNTER,   RONLY, hpmcrtOltPerfTable_get, 2, {1, 3}},
    {hpmcrtOltRxBytes, ASN_OCTET_STR,   RONLY, hpmcrtOltPerfTable_get, 2, {1, 4}}

} ;


oid  hpmcrtOltPerfTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 6, 2};
int   hpmcrtOltPerfTable_oid_len = sizeof(hpmcrtOltPerfTable_oid) / sizeof(oid);

FindVarMethod  hpmcrtEthPerfTable_get ;

struct variable2 hpmcrtEthPerfTable[] =
{
    { hpmcrtEthCntType,   ASN_INTEGER, RWRITE, hpmcrtEthPerfTable_get  , 2, {1, 1}},
    { hpmcrtEthRxPCnt,   ASN_COUNTER,  RONLY, hpmcrtEthPerfTable_get , 2, {1, 2}},
    { hpmcrtEthTxPCnt, ASN_COUNTER, RONLY, hpmcrtEthPerfTable_get, 2, {1, 3}},
    { hpmcrtEthRxBytes, ASN_OCTET_STR,  RONLY, hpmcrtEthPerfTable_get, 2, {1, 4}}

} ;


oid hpmcrtEthPerfTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 6, 3};
int  hpmcrtEthPerfTable_oid_len = sizeof(hpmcrtEthPerfTable_oid) / sizeof(oid);


FindVarMethod  hpmcrtCpuPerfTable_get ;

struct variable2 hpmcrtCpuPerfTable[] =
{
    { hpmcrtCpuCntType ,   ASN_INTEGER,   RWRITE, hpmcrtCpuPerfTable_get  , 2, {1, 1}},
    { hpmcrtCpuRxPCnt,   ASN_COUNTER,   RONLY, hpmcrtCpuPerfTable_get , 2, {1, 2}},
    {  hpmcrtCpuTxPCnt, ASN_COUNTER,   RONLY, hpmcrtCpuPerfTable_get, 2, {1, 3}},
    { hpmcrtCpuRxBytes, ASN_OCTET_STR,   RONLY, hpmcrtCpuPerfTable_get, 2, {1, 4}}

} ;

oid hpmcrtCpuPerfTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 6, 4};
int  hpmcrtCpuPerfTable_oid_len = sizeof(hpmcrtCpuPerfTable_oid) / sizeof(oid);



FindVarMethod  hpmcrtOamTable_get ;

struct variable2 hpmcrtOamTable[] =
{
    { hpmcrtOamMode ,   ASN_INTEGER,   RWRITE, hpmcrtOamTable_get  , 2, {1, 1}},
    { hpmcrtOamLpbkSta, ASN_INTEGER,   RONLY, hpmcrtOamTable_get, 2, {1, 3}}

} ;

oid hpmcrtOamTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 7, 1};
int  hpmcrtOamTable_oid_len = sizeof(hpmcrtOamTable_oid) / sizeof(oid);



FindVarMethod  hpmcrtAddrTable_get ;

struct variable2 hpmcrtAddrTable[] =
{
    { hpmcrtIpAddr , ASN_IPADDRESS,   RWRITE, hpmcrtAddrTable_get  , 2, {1, 1}},
    { hpmcrtIpMask , ASN_IPADDRESS,   RWRITE, hpmcrtAddrTable_get  , 2, {1, 2}},
    { hpmcrtGatewayAddr ,  ASN_IPADDRESS,   RWRITE, hpmcrtAddrTable_get  , 2, {1, 3}},
    { hpmcrtMacAddr, ASN_OCTET_STR,   RONLY, hpmcrtAddrTable_get, 2, {1, 4}}

} ;

oid hpmcrtAddrTable_oid[]   = { EFM_AGENT_HPMCREMOTE, 8, 1};
int  hpmcrtAddrTable_oid_len = sizeof(hpmcrtAddrTable_oid) / sizeof(oid);

FindVarMethod  hpmcrtConverTable_get ;

struct variable2 hpmcrtConverTable[] =
{
    { hpmcrtIpConverterMode ,  ASN_INTEGER,  RONLY, hpmcrtConverTable_get  , 2, {1, 1}}


} ;

oid hpmcrtConverTable_oid[]   = { EFM_AGENT_CONVERTER, 1};
int  hpmcrtConverTable_oid_len = sizeof(hpmcrtConverTable_oid) / sizeof(oid);


void  init_mib_efm_agent(void)
{
    REGISTER_MIB("hpmcrtCommConfTable", hpmcrtCommConfTable, variable2,
                 hpmcrtCommConfTable_oid) ;

    REGISTER_MIB("hpmcrtOltPtConfTable", hpmcrtOltPtConfTable, variable2,
                 hpmcrtOltPtConfTable_oid) ;

    REGISTER_MIB("hpmcrtEthPtConfTable", hpmcrtEthPtConfTable, variable2,
                 hpmcrtEthPtConfTable_oid) ;


    REGISTER_MIB("hpmcrtOltPtAlmTable", hpmcrtOltPtAlmTable, variable2,
                 hpmcrtOltPtAlmTable_oid) ;
    REGISTER_MIB("hpmcrtEthPtAlmTable", hpmcrtEthPtAlmTable, variable2,
                 hpmcrtEthPtAlmTable_oid) ;
    REGISTER_MIB("hpmcrtVlanConfTable", hpmcrtVlanConfTable, variable2,
                 hpmcrtVlanConfTable_oid) ;
    REGISTER_MIB("hpmcrtOltPtVlanConfTable", hpmcrtOltPtVlanConfTable, variable2,
                 hpmcrtOltPtVlanConfTable_oid) ;
    REGISTER_MIB("hpmcrtEthPtVlanConfTable", hpmcrtEthPtVlanConfTable, variable2,
                 hpmcrtEthPtVlanConfTable_oid) ;
    REGISTER_MIB("hpmcrtCpuPtVlanConfTable", hpmcrtCpuPtVlanConfTable, variable2,
                 hpmcrtCpuPtVlanConfTable_oid) ;

    REGISTER_MIB("hpmcrtVlanGroupConfTable", hpmcrtVlanGroupConfTable, variable2,
                 hpmcrtVlanGroupConfTable_oid) ;

    REGISTER_MIB("hpmcrtQinQTable", hpmcrtQinQTable, variable2,
                 hpmcrtQinQTable_oid) ;
    REGISTER_MIB("hpmcrtQosTable", hpmcrtQosTable, variable2,
                 hpmcrtQosTable_oid) ;
    REGISTER_MIB("hpmcrtPerfTable", hpmcrtPerfTable, variable2,
                 hpmcrtPerfTable_oid) ;
    REGISTER_MIB("hpmcrtOltPerfTable", hpmcrtOltPerfTable, variable2,
                 hpmcrtOltPerfTable_oid) ;
    REGISTER_MIB("hpmcrtEthPerfTable", hpmcrtEthPerfTable, variable2,
                 hpmcrtEthPerfTable_oid) ;
    REGISTER_MIB("hpmcrtCpuPerfTable", hpmcrtCpuPerfTable, variable2,
                 hpmcrtCpuPerfTable_oid) ;
    REGISTER_MIB("hpmcrtOamTable", hpmcrtOamTable, variable2,
                 hpmcrtOamTable_oid) ;
    REGISTER_MIB("hpmcrtAddrTable", hpmcrtAddrTable, variable2,
                 hpmcrtAddrTable_oid) ;
    REGISTER_MIB("hpmcrtConverTable", hpmcrtConverTable, variable2,
                 hpmcrtConverTable_oid) ;

}


int efm_if_comm_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                       struct efm_comm_info_snmp  *index)
{

	struct ipc_mesg_n *pmesg = NULL;
    struct efm_comm_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
	
	pmesg = snmp_get_efm_agent_info_bulk(index->ifindex, &data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_COMM_INFO_TYPE);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_comm_info_snmp *)(pmesg->msg_data);
	
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_comm_info_snmp));
			pefm_info++;
		}
			mem_share_free(pmesg, MODULE_ID_SNMPD);
			
		return TRUE;
	
	}
	
	return FALSE;
	
}
struct efm_comm_info_snmp *efm_if_comm_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                        int exact,
                                                        const struct efm_comm_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_comm_info_snmp   *data1_find = NULL;

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
hpmcrtCommConfTable_get(struct variable *vp,
                        oid *name,
                        size_t *length,
                        int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_comm_info_snmp index ;
    int ret = 0;
    struct efm_comm_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0; 

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);
	
    if (ret < 0)
    {
        return NULL;
    }
	zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  ifindex =  %x\n", __FUNCTION__, __LINE__, index.ifindex);

    if (NULL == efm_comm_info_cache)
    {
        efm_comm_info_cache = snmp_cache_init(sizeof(struct efm_comm_info_snmp) ,
                                              efm_if_comm_info_get_data_from_ipc ,
                                              efm_if_comm_info_node_lookup);

        if (NULL == efm_comm_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }


	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		efm_info = snmp_cache_get_data_by_index(efm_comm_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_comm_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info )
    {
    	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] \n", __FUNCTION__, __LINE__);
        return NULL;
    }

	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x\n",__FUNCTION__,__LINE__,efm_info->ifindex);
	if(!exact)
	{
		ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
	}



    *write_method = hpmcrtCommInfo_table_set;

    zlog_debug(SNMP_DBG_MIB_GET,"%s %s [%d] vp->magic = %d \n", __FILE__, __FUNCTION__, __LINE__, vp->magic);

    switch (vp->magic)
    {

        case  hpmcrtSubType:
            int_value = efm_info->efm_comm_info.efm_SubType;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtSoftVer:
            sprintf(soft_num, "%x", efm_info->efm_comm_info.efm_SoftVer);
            soft_num[0] = atoi(soft_num);
            sprintf(soft_num, "%d%c%d", soft_num[0] / 10, 46, soft_num[0] % 10);
            *var_len = strlen(soft_num);
            return (u_char *)soft_num;

        case  hpmcrtEthChNum:
            int_value = efm_info->efm_comm_info.efm_EthChNum;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtEthPtNum:
            int_value = efm_info->efm_comm_info.efm_EthPtNum;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtOltPtNum:
            int_value = efm_info->efm_comm_info.efm_OltPtNum;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtHwSw:
            int_value = efm_info->efm_comm_info.efm_HwSw;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtSFI:
            int_value = efm_info->efm_comm_info.efm_SFI;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtDataResum:
            //*write_method = hpmcrtCommInfo_set;
            int_value = efm_info->efm_comm_info.efm_DataResum;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtLFPCtrl:

            int_value = efm_info->efm_comm_info.efm_LFPCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtALSCtrl:
            int_value = efm_info->efm_comm_info.efm_ALSCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtJumbo:
            int_value = efm_info->efm_comm_info.efm_Jumbo;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtReboot:
            int_value = efm_info->efm_comm_info.efm_Reboot;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtIpCtrl:
            int_value = efm_info->efm_comm_info.efm_IpCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtEthStromfilter:
            bits_value = efm_info->efm_comm_info.efm_EthStromfilter;
            *var_len = sizeof(uint8_t);
            return (u_char *)&bits_value;

        case  hpmcrtDHCPclient:
            int_value = efm_info->efm_comm_info.efm_DHCPclient;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtDipSwitchSta:
            bits_value = efm_info->efm_comm_info.efm_DipSwitchSta;
            *var_len = sizeof(uint8_t);
            return (u_char *)&bits_value;

		case hpmcrtHostName:
			//bits_value = efm_info->efm_comm_info.efm_HostName;
            memcpy(str_value, efm_info->efm_comm_info.efm_HostName, VTYSH_HOST_NAME_LEN + 1);
            *var_len = VTYSH_HOST_NAME_LEN + 1;
            return (u_char *)str_value;
			break;
        default :
            return (NULL);
    }

}
int efm_if_oltpt_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                        struct efm_oltpt_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL; 
    struct efm_oltpt_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
	pmesg = snmp_get_efm_agent_SecIfindex_info_bulk(&index->efm_oltpt_info.efm_Oltifindex,sizeof(uint32_t),index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_OLTPT_INFO_TYPE);
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);


	if(pmesg)
	{
		pefm_info  = (struct efm_oltpt_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_oltpt_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_oltpt_info_snmp *efm_if_oltpt_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                          int exact,
                                                          const struct efm_oltpt_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_oltpt_info_snmp *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {


        if (NULL == index_input)
        {
            return cache->data_list->head->data ;
        }

        if (0 == index_input->ifindex && 0 == index_input->efm_oltpt_info.efm_Oltifindex)
        {
            return cache->data_list->head->data ;

        }

        if ((data1_find->ifindex == index_input->ifindex) && (data1_find->efm_oltpt_info.efm_Oltifindex == index_input->efm_oltpt_info.efm_Oltifindex))
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

/*two ifindex*/
u_char *
hpmcrtOltPtConfTable_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_oltpt_info_snmp index ;
    int ret = 0;
    uint32_t sec_ifindex = 0;
    struct efm_oltpt_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */

    ret = ipran_snmp_intx2_index_get(vp, name, length, &index.ifindex, &sec_ifindex , exact);

    if (ret < 0)
    {
        return NULL;
    }
	
    index.efm_oltpt_info.efm_Oltifindex = sec_ifindex;


    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]: '%s:ifindex = %d --sec_nidex =  %d\n", __FILE__, __LINE__, __func__, index.ifindex, sec_ifindex);

    if (NULL == efm_oltpt_info_cache)
    {
        efm_oltpt_info_cache = snmp_cache_init(sizeof(struct efm_oltpt_info_snmp) ,
                                               efm_if_oltpt_info_get_data_from_ipc ,
                                               efm_if_oltpt_info_node_lookup);

        if (NULL == efm_oltpt_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }


    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		index.efm_oltpt_info.efm_Oltifindex = 1; 
		efm_info = snmp_cache_get_data_by_index(efm_oltpt_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_oltpt_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }



    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, efm_info->ifindex, efm_info->efm_oltpt_info.efm_Oltifindex);
    }

    *write_method = hpmcrtOltptInfo_table_set;

    switch (vp->magic)
    {

        case hpmcrtOltPtNO :
            int_value = efm_info->efm_oltpt_info.efm_Oltifindex;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtOltPtNegCtrl:
            int_value = efm_info->efm_oltpt_info.efm_OltPtNegCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtOltPtSpdCtrl :
            int_value = efm_info->efm_oltpt_info.efm_OltPtSpdCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtOltPtDplCtrl:
            int_value = efm_info->efm_oltpt_info.efm_OltPtDplCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtOltPtDisCtrl :
            int_value = efm_info->efm_oltpt_info.efm_OltPtDisCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtOltPtIngressRateN :
            int_value = efm_info->efm_oltpt_info.efm_OltPtIngressRateN;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtOltPtEgressRateN:
            int_value = efm_info->efm_oltpt_info.efm_OltPtEgressRateN;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtOltPtFlowCtrl:
            int_value = efm_info->efm_oltpt_info.efm_OltPtFlowCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return (NULL);
    }

}

int efm_if_ethpt_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                        struct efm_ethpt_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL; 
	struct efm_ethpt_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;

	pmesg = snmp_get_efm_agent_SecIfindex_info_bulk(&index->efm_ethpt_info.efm_Ethifindex,sizeof(uint32_t),index->ifindex, &data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_ETHPT_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_ethpt_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_ethpt_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_ethpt_info_snmp *efm_if_ethpt_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                          int exact,
                                                          const struct efm_ethpt_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_ethpt_info_snmp *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {

        if (NULL == index_input)
        {
            return cache->data_list->head->data ;
        }

        if (0 == index_input->ifindex && 0 == index_input->efm_ethpt_info.efm_Ethifindex)
        {

            return cache->data_list->head->data;

        }

        if (data1_find->ifindex == index_input->ifindex && data1_find->efm_ethpt_info.efm_Ethifindex  == index_input->efm_ethpt_info.efm_Ethifindex)
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
hpmcrtEthPtConfTable_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{

    int ret = 0;
    uint32_t sec_ifindex = 0;
    struct efm_ethpt_info_snmp index ;
    struct efm_ethpt_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index.ifindex , &sec_ifindex , exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.efm_ethpt_info.efm_Ethifindex = sec_ifindex;


    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]: '%s:ifindex = %d --sec_nidex =  %d\n", __FILE__, __LINE__, __func__, index.ifindex, sec_ifindex);

    if (NULL == efm_ethpt_info_cache)
    {
        efm_ethpt_info_cache = snmp_cache_init(sizeof(struct efm_ethpt_info_snmp) ,
                                               efm_if_ethpt_info_get_data_from_ipc ,
                                               efm_if_ethpt_info_node_lookup);

        if (NULL == efm_ethpt_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		index.efm_ethpt_info.efm_Ethifindex = 1; 
		efm_info = snmp_cache_get_data_by_index(efm_ethpt_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_ethpt_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}


    if (NULL == efm_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, efm_info->ifindex, efm_info->efm_ethpt_info.efm_Ethifindex);
    }

    *write_method = hpmcrtEthptInfo_table_set;

    switch (vp->magic)
    {

        case hpmcrtEthPtNO :
            int_value = efm_info->efm_ethpt_info.efm_Ethifindex;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtEthPtNegCtrl :
            int_value = efm_info->efm_ethpt_info.efm_EthPtNegCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtEthPtSpdCtrl  :
            int_value = efm_info->efm_ethpt_info.efm_EthPtSpdCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtEthPtDplCtrl :
            int_value = efm_info->efm_ethpt_info.efm_EthPtDplCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtEthPtDisCtrl :
            int_value = efm_info->efm_ethpt_info.efm_EthPtDisCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtEthPtIngressRateN:
            int_value = efm_info->efm_ethpt_info.efm_EthPtIngressRateN;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtEthPtEgressRateN:
            int_value = efm_info->efm_ethpt_info.efm_EthPtEgressRateN;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtEthPtFlowCtrl :
            int_value = efm_info->efm_ethpt_info.efm_EthPtFlowCtrl;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return (NULL);
    }

}

int efm_if_oltptalm_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                           struct efm_oltptalm_info_snmp   *index)
{
		struct ipc_mesg_n *pmesg = NULL;
    struct efm_oltptalm_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;

	pmesg = snmp_get_efm_agent_SecIfindex_info_bulk(&index->efm_oltptalm_info.efm_OltPtalmNO,sizeof(uint32_t),index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_OLTPTALM_INFO_TYPE);
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{	
		pefm_info = (struct efm_oltptalm_info_snmp *)(pmesg->msg_data);
			
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_oltptalm_info_snmp));
			pefm_info++;
		}
	
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}
		
	return FALSE;
}
struct efm_oltptalm_info_snmp *efm_if_oltptalm_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                                int exact,
                                                                const struct efm_oltptalm_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_oltptalm_info_snmp *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {

        if (NULL == index_input)
        {
            return cache->data_list->head->data ;
        }

        if (0 == index_input->ifindex && 0 == index_input->efm_oltptalm_info.efm_OltPtalmNO)
        {
            return cache->data_list->head->data;

        }

        if (data1_find->ifindex == index_input->ifindex && data1_find->efm_oltptalm_info.efm_OltPtalmNO == index_input->efm_oltptalm_info.efm_OltPtalmNO)
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
hpmcrtOltPtAlmTable_get(struct variable *vp,
                        oid *name,
                        size_t *length,
                        int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_oltptalm_info_snmp index ;
    int ret = 0;
    uint32_t sec_ifindex = 0;
    struct efm_oltptalm_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;
	
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index.ifindex,  &sec_ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.efm_oltptalm_info.efm_OltPtalmNO = sec_ifindex;


    if (NULL == efm_oltptalm_info_cache)
    {
        efm_oltptalm_info_cache = snmp_cache_init(sizeof(struct efm_oltptalm_info_snmp) ,
                                                  efm_if_oltptalm_info_get_data_from_ipc ,
                                                  efm_if_oltptalm_info_node_lookup);

        if (NULL == efm_oltptalm_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		index.efm_oltptalm_info.efm_OltPtalmNO = 1; 
		efm_info = snmp_cache_get_data_by_index(efm_oltptalm_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_oltptalm_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, efm_info->ifindex, efm_info->efm_oltptalm_info.efm_OltPtalmNO);
    }



    switch (vp->magic)
    {

        case  hpmcrtOltPtNO :

            int_value = efm_info->efm_oltptalm_info.efm_OltPtalmNO;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtOltPtStatInfo:
            bits_value = efm_info->efm_oltptalm_info.efm_OltPtalmStatInfo;
            *var_len = sizeof(uint8_t);
            return (u_char *)&bits_value;

        case hpmcrtOltPtAlmInfo:
            bits_value = efm_info->efm_oltptalm_info.efm_OltPtalmAlmInfo;
            *var_len = sizeof(uint8_t);
            return (u_char *)&bits_value;


        default :
            return (NULL);
    }

}



int efm_if_ethptalm_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                           struct efm_ethptalm_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct efm_ethptalm_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;
	
	pmesg = snmp_get_efm_agent_SecIfindex_info_bulk(&index->efm_ethptalm_info.efm_EthPtNO,sizeof(uint32_t),index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_ETHPTALM_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_ethptalm_info_snmp *)(pmesg->msg_data);
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_ethptalm_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_ethptalm_info_snmp *efm_if_ethptalm_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                                int exact,
                                                                const struct efm_ethptalm_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_ethptalm_info_snmp *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data ;
        }

        if (0 == index_input->ifindex && 0 ==  index_input->efm_ethptalm_info.efm_EthPtNO)
        {
            return cache->data_list->head->data ;

        }

        if (data1_find->ifindex == index_input->ifindex && \
                data1_find->efm_ethptalm_info.efm_EthPtNO == index_input->efm_ethptalm_info.efm_EthPtNO)
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
hpmcrtEthPtAlmTable_get(struct variable *vp,
                        oid *name,
                        size_t *length,
                        int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_ethptalm_info_snmp index ;
    int ret = 0;
    uint32_t sec_ifindex = 0;
    struct efm_ethptalm_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index.ifindex, &sec_ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.efm_ethptalm_info.efm_EthPtNO = sec_ifindex;

    if (NULL == efm_ethptalm_info_cache)
    {
        efm_ethptalm_info_cache = snmp_cache_init(sizeof(struct efm_ethptalm_info_snmp) ,
                                                  efm_if_ethptalm_info_get_data_from_ipc ,
                                                  efm_if_ethptalm_info_node_lookup);

        if (NULL == efm_ethptalm_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    
    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		index.efm_ethptalm_info.efm_EthPtNO = 1; 
		efm_info = snmp_cache_get_data_by_index(efm_ethptalm_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_ethptalm_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, efm_info->ifindex, efm_info->efm_ethptalm_info.efm_EthPtNO);
    }



    switch (vp->magic)
    {

        case hpmcrtEthPtNO :

            int_value = efm_info->efm_ethptalm_info.efm_EthPtNO;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtEthPtStatInfo:
            bits_value = efm_info->efm_ethptalm_info.efm_EthPtStatInfo;
            *var_len = sizeof(uint8_t);
            return (u_char *)&bits_value;

        case  hpmcrtEthPtAlmInfo :
            bits_value = efm_info->efm_ethptalm_info.efm_EthPtAlmInfo;
            *var_len = sizeof(uint8_t);
            return (u_char *)&bits_value;



        default :
            return (NULL);
    }

}



int efm_if_vlan_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                       struct efm_vlan_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct efm_vlan_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;
	
	pmesg = snmp_get_efm_agent_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_VLAN_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_vlan_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_vlan_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_vlan_info_snmp *efm_if_vlan_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                        int exact,
                                                        const struct efm_vlan_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_vlan_info_snmp *data1_find  = NULL;

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
hpmcrtVlanConfTable_get(struct variable *vp,
                        oid *name,
                        size_t *length,
                        int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_vlan_info_snmp index ;
    int ret = 0;
    struct efm_vlan_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }


    if (NULL == efm_vlan_info_cache)
    {
        efm_vlan_info_cache = snmp_cache_init(sizeof(struct efm_vlan_info_snmp) ,
                                              efm_if_vlan_info_get_data_from_ipc ,
                                              efm_if_vlan_info_node_lookup);

        if (NULL == efm_vlan_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }


    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		efm_info = snmp_cache_get_data_by_index(efm_vlan_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_vlan_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}


    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }

    *write_method = hpmcrtVlanMode_table_set;

    switch (vp->magic)
    {

        case hpmcrtVlanMode:
            int_value = efm_info->efm_VlanMode;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        default :
            return (NULL);
    }

}



int efm_if_oltvlan_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                          struct efm_oltvlan_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct efm_oltvlan_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;
	
	pmesg = snmp_get_efm_agent_info_bulk(index->ifindex, &data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_OLTVLAN_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_oltvlan_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_oltvlan_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_oltvlan_info_snmp *efm_if_oltvlan_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                              int exact,
                                                              const struct efm_oltvlan_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_oltvlan_info_snmp *data1_find = NULL;

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
hpmcrtOltPtVlanConfTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_oltvlan_info_snmp index ;
    int ret = 0;
    struct efm_oltvlan_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;
	
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }
	
	
    if (NULL == efm_oltvlan_info_cache)
    {
        efm_oltvlan_info_cache = snmp_cache_init(sizeof(struct efm_oltvlan_info_snmp) ,
                                                 efm_if_oltvlan_info_get_data_from_ipc ,
                                                 efm_if_oltvlan_info_node_lookup);

        if (NULL == efm_oltvlan_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		efm_info = snmp_cache_get_data_by_index(efm_oltvlan_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_oltvlan_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }

    *write_method = hpmcrtOltptVlan_table_set;

    switch (vp->magic)
    {

        case hpmcrtOltPtInMode :

            int_value = efm_info->efm_oltvlan_info.efm_OltPtInMode;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtOltPtPVID :
            int_value = efm_info->efm_oltvlan_info.efm_OltPtPVID;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtOltPtPrior :
            int_value = efm_info->efm_oltvlan_info.efm_OltPtPrior;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtOltPtTagMode :
            int_value = efm_info->efm_oltvlan_info.efm_OltPtTagMode;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return (NULL);
    }

}



int efm_if_ethvlan_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                          struct efm_ethvlan_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct efm_ethvlan_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;
	
	pmesg = snmp_get_efm_agent_SecIfindex_info_bulk(&index->efm_ethvlan_info.efm_EthPtNo,sizeof(uint32_t),index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_ETHVLAN_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_ethvlan_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_ethvlan_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_ethvlan_info_snmp *efm_if_ethvlan_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                              int exact,
                                                              const struct efm_ethvlan_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_ethvlan_info_snmp *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data ;
        }

        if (0 == index_input->ifindex && 0 == index_input->efm_ethvlan_info.efm_EthPtNo)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifindex == index_input->ifindex && \
                data1_find->efm_ethvlan_info.efm_EthPtNo == index_input->efm_ethvlan_info.efm_EthPtNo)
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
hpmcrtEthPtVlanConfTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_ethvlan_info_snmp index ;
    int ret = 0;
    uint32_t sec_ifindex = 0;
    struct efm_ethvlan_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index.ifindex, &sec_ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.efm_ethvlan_info.efm_EthPtNo =    sec_ifindex;
	

    if (NULL == efm_ethvlan_info_cache)
    {
        efm_ethvlan_info_cache = snmp_cache_init(sizeof(struct efm_ethvlan_info_snmp) ,
                                                 efm_if_ethvlan_info_get_data_from_ipc ,
                                                 efm_if_ethvlan_info_node_lookup);

        if (NULL == efm_ethvlan_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		index.efm_ethvlan_info.efm_EthPtNo = 1;
		efm_info = snmp_cache_get_data_by_index(efm_ethvlan_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_ethvlan_info_cache , exact, &index);
		
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, efm_info->ifindex, efm_info->efm_ethvlan_info.efm_EthPtNo);
    }

    *write_method = hpmcrtEthptVlan_table_set;

    switch (vp->magic)
    {
        case hpmcrtEthPtNO:
            int_value = efm_info->efm_ethvlan_info.efm_EthPtNo;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtEthPtInMode :
            int_value = efm_info->efm_ethvlan_info.efm_EthPtInMode;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtEthPtPVID :
            int_value = efm_info->efm_ethvlan_info.efm_EthPtPVID;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtEthPtPrior :
            int_value = efm_info->efm_ethvlan_info.efm_EthPtPrior;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtEthPtTagMode :
            int_value = efm_info->efm_ethvlan_info.efm_EthPtTagMode;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        default :
            return (NULL);
    }

}



int efm_if_cpuvlan_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                          struct efm_cpuvlan_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct efm_cpuvlan_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;
	
	pmesg = snmp_get_efm_agent_info_bulk(index->ifindex, &data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_CPUVLAN_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_cpuvlan_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_cpuvlan_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_cpuvlan_info_snmp *efm_if_cpuvlan_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                              int exact,
                                                              const struct efm_cpuvlan_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_cpuvlan_info_snmp *data1_find = NULL;

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
hpmcrtCpuPtVlanConfTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_cpuvlan_info_snmp index ;
    int ret = 0;
    struct efm_cpuvlan_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

	
    if (NULL == efm_cpuvlan_info_cache)
    {
        efm_cpuvlan_info_cache = snmp_cache_init(sizeof(struct efm_cpuvlan_info_snmp) ,
                                                 efm_if_cpuvlan_info_get_data_from_ipc ,
                                                 efm_if_cpuvlan_info_node_lookup);

        if (NULL == efm_cpuvlan_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

  	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	 zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		efm_info = snmp_cache_get_data_by_index(efm_cpuvlan_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_cpuvlan_info_cache, exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }


    *write_method = hpmcrtCpuptVlan_table_set;

    switch (vp->magic)
    {

        case hpmcrtCpuPtInMode:

            int_value = efm_info->efm_cpuvlan_info.efm_CpuPtInMode;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtCpuPtPVID :
            int_value = efm_info->efm_cpuvlan_info.efm_CpuPtPVID;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtCpuPtPrior :
            int_value = efm_info->efm_cpuvlan_info.efm_CpuPtPrior;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtCpuPtTagMode:
            int_value = efm_info->efm_cpuvlan_info.efm_CpuPtTagMode;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return (NULL);
    }

}


int efm_if_vlangroup_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                            struct efm_vlangroup_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct efm_vlangroup_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;
	
	pmesg = snmp_get_efm_agent_SecIfindex_info_bulk(&index->efm_vlangroup_info.efm_VlanGroupNO,sizeof(uint32_t),index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_VLANGROUP_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_vlangroup_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_vlangroup_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_vlangroup_info_snmp *efm_if_vlangroup_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                                  int exact,
                                                                  const struct efm_vlangroup_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_vlangroup_info_snmp *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data ;
        }

        if (0 == index_input->ifindex && 0 == index_input->efm_vlangroup_info.efm_VlanGroupNO)
        {
            return cache->data_list->head->data;
        }

        if (data1_find->ifindex == index_input->ifindex && data1_find->efm_vlangroup_info.efm_VlanGroupNO == \
                index_input->efm_vlangroup_info.efm_VlanGroupNO)
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
hpmcrtVlanGroupConfTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{

    int ret = 0;
    uint32_t  sec_ifindex = 0;
    struct efm_vlangroup_info_snmp index ;
    struct efm_vlangroup_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

	
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index.ifindex, &sec_ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.efm_vlangroup_info.efm_VlanGroupNO = sec_ifindex;


    if (NULL == efm_vlangroup_info_cache)
    {
        efm_vlangroup_info_cache = snmp_cache_init(sizeof(struct efm_vlangroup_info_snmp) ,
                                                   efm_if_vlangroup_info_get_data_from_ipc ,
                                                   efm_if_vlangroup_info_node_lookup);

        if (NULL == efm_vlangroup_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		index.efm_vlangroup_info.efm_VlanGroupNO = 1;
		efm_info = snmp_cache_get_data_by_index(efm_vlangroup_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_vlangroup_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, efm_info->ifindex, efm_info->efm_vlangroup_info.efm_VlanGroupNO);
    }


    *write_method = hpmcrtVlanGroup_table_set;

    switch (vp->magic)
    {

        case hpmcrtVlanGroupNO:

            int_value = efm_info->efm_vlangroup_info.efm_VlanGroupNO;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtVlanId :
            int_value = efm_info->efm_vlangroup_info.efm_VlanId;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtVlanMember :
            bits_value = efm_info->efm_vlangroup_info.efm_VlanMember;
            *var_len = sizeof(uint8_t);
            return (u_char *)&bits_value;

        default :
            return (NULL);
    }

}

/*this part maybe use */
#if 0
int efm_if_invalid_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                          struct efm_invalid_info_snmp  *index)
{
    struct efm_invalid_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
    pefm_info = (struct efm_invalid_info_snmp *)snmp_get_efm_agent_info_bulk(index->ifindex, &data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_INVALID_INFO_TYPE);
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    if (0 == data_num  || NULL == pefm_info)
    {
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pefm_info , sizeof(struct efm_invalid_info_snmp));
            pefm_info++;
        }

        //time(&cache->getNext_time_old);
        return TRUE;
    }
}
struct efm_invalid_info_snmp *efm_if_invalid_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                              int exact,
                                                              const struct efm_invalid_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_invalid_info_snmp *data1_find = NULL;

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
hpmcrtInvalidTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{


    struct efm_invalid_info_snmp index ;
    int ret = 0;
    struct efm_invalid_info_snmp *efm_info = NULL;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]	exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == efm_invalid_info_cache)
    {
        efm_invalid_info_cache = snmp_cache_init(sizeof(struct efm_invalid_info_snmp) ,
                                                 efm_if_invalid_info_get_data_from_ipc ,
                                                 efm_if_invalid_info_node_lookup);

        if (NULL == efm_invalid_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    efm_info = snmp_cache_get_data_by_index(efm_invalid_info_cache , exact, &index);

    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }


    //*write_method = hpmcrtCpuptVlan_table_set;

    switch (vp->magic)
    {

        case  hpmcrtInvildvalue:

            int_value = efm_info->invalid_data;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return (NULL);
    }

    return (NULL);
}
#endif
int efm_if_qinq_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                       struct efm_qinq_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct efm_qinq_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;
	
	pmesg = snmp_get_efm_agent_info_bulk(index->ifindex, &data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_QINQ_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_qinq_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_qinq_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_qinq_info_snmp *efm_if_qinq_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                        int exact,
                                                        const struct efm_qinq_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_qinq_info_snmp *data1_find = NULL;

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
hpmcrtQinQTable_get(struct variable *vp,
                    oid *name,
                    size_t *length,
                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_qinq_info_snmp index ;
    int ret = 0;
    struct efm_qinq_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == efm_qinq_info_cache)
    {
        efm_qinq_info_cache = snmp_cache_init(sizeof(struct efm_qinq_info_snmp) ,
                                              efm_if_qinq_info_get_data_from_ipc ,
                                              efm_if_qinq_info_node_lookup);

        if (NULL == efm_qinq_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		efm_info = snmp_cache_get_data_by_index(efm_qinq_info_cache, 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_qinq_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }

    *write_method =     hpmcrtQinq_table_set;

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }



    switch (vp->magic)
    {

        case hpmcrtQinQDirection :

            int_value = efm_info->efm_qinq_info.efm_QinQDirection;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtQinQTPID:
            int_value = efm_info->efm_qinq_info.efm_QinQTPID;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtQinQVid :
            int_value = efm_info->efm_qinq_info.efm_QinQVid;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtQinQPrior :
            int_value = efm_info->efm_qinq_info.efm_QinQPrior;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        default :
            return (NULL);
    }

}



int efm_if_qos_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                      struct efm_qos_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct efm_qos_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;
	
	pmesg = snmp_get_efm_agent_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_QOS_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_qos_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_qos_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_qos_info_snmp *efm_if_qos_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                      int exact,
                                                      const struct efm_qos_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_qos_info_snmp *data1_find = NULL;

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
hpmcrtQosTable_get(struct variable *vp,
                   oid *name,
                   size_t *length,
                   int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_qos_info_snmp index ;
    int ret = 0;
    struct efm_qos_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }
	
    if (NULL == efm_qos_info_cache)
    {
        efm_qos_info_cache = snmp_cache_init(sizeof(struct efm_qos_info_snmp) ,
                                             efm_if_qos_info_get_data_from_ipc ,
                                             efm_if_qos_info_node_lookup);

        if (NULL == efm_qos_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		efm_info = snmp_cache_get_data_by_index(efm_qos_info_cache, 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_qos_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }


    *write_method = hpmcrtQos_table_set ;

    switch (vp->magic)
    {

        case hpmcrt1pQos:

            int_value = efm_info->efm_1pQos;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        default :
            return (NULL);
    }

}



int efm_if_perf_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                       struct efm_perf_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
    struct efm_perf_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
	pmesg = snmp_get_efm_agent_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_PERF_INFO_TYPE);
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_perf_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_perf_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_perf_info_snmp *efm_if_perf_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                        int exact,
                                                        const struct efm_perf_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_perf_info_snmp *data1_find = NULL;

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
hpmcrtPerfTable_get(struct variable *vp,
                    oid *name,
                    size_t *length,
                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_perf_info_snmp index ;
    int ret = 0;
    struct efm_perf_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }
	
    if (NULL == efm_perf_info_cache)
    {
        efm_perf_info_cache = snmp_cache_init(sizeof(struct efm_perf_info_snmp) ,
                                              efm_if_perf_info_get_data_from_ipc ,
                                              efm_if_perf_info_node_lookup);

        if (NULL == efm_perf_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		efm_info = snmp_cache_get_data_by_index(efm_perf_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_perf_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }

    *write_method = hpmcrtPerf_table_set;

    switch (vp->magic)
    {
        case hpmcrtCleanCounter:
            int_value = efm_info->efm_CleanCounter;
            *var_len = sizeof(int);
            return (u_char *)&int_value;


        default :
            return (NULL);
    }

}


int efm_if_oltperf_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                          struct efm_oltperf_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct efm_oltperf_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;
	
	pmesg = snmp_get_efm_agent_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_OLTPERF_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_oltperf_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_oltperf_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_oltperf_info_snmp *efm_if_oltperf_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                              int exact,
                                                              const struct efm_oltperf_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_oltperf_info_snmp *data1_find = NULL;

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
hpmcrtOltPerfTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_oltperf_info_snmp index ;
    int ret = 0;
    struct efm_oltperf_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

	
    if (NULL == efm_oltperf_info_cache)
    {
        efm_oltperf_info_cache = snmp_cache_init(sizeof(struct efm_oltperf_info_snmp) ,
                                                 efm_if_oltperf_info_get_data_from_ipc ,
                                                 efm_if_oltperf_info_node_lookup);

        if (NULL == efm_oltperf_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

  	 ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	 zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		efm_info = snmp_cache_get_data_by_index(efm_oltperf_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_oltperf_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }

    *write_method = hpmcrtOltPerf_table_set;

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }



    switch (vp->magic)
    {

        case hpmcrtOltCntType:

            int_value = efm_info->efm_oltperf_info.efm_OltCntType;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtOltRxPCnt :
            int_value = efm_info->efm_oltperf_info.efm_OltRxPCnt;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtOltTxPCnt :
            int_value = efm_info->efm_oltperf_info.efm_OltTxPCnt;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case   hpmcrtOltRxBytes:

            memcpy(bits_num, efm_info->efm_oltperf_info.efm_OltRxBytes, BITS_NUM);
            *var_len = sizeof(bits_num) / sizeof(u_char);
            return (u_char *)bits_num;


        default :
            return (NULL);
    }

}


int efm_if_ethperf_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                          struct efm_ethperf_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct efm_ethperf_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;
	
	pmesg = snmp_get_efm_agent_SecIfindex_info_bulk(&index->efm_ethperf_info.efm_EthPerfNo,sizeof(uint32_t),index->ifindex, &data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_ETHPERF_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_ethperf_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_ethperf_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_ethperf_info_snmp *efm_if_ethperf_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                              int exact,
                                                              const struct efm_ethperf_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_ethperf_info_snmp *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data ;
        }

        if (0 == index_input->ifindex  &&  0 == index_input->efm_ethperf_info.efm_EthPerfNo)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifindex == index_input->ifindex \
                && data1_find->efm_ethperf_info.efm_EthPerfNo == index_input->efm_ethperf_info.efm_EthPerfNo)
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
hpmcrtEthPerfTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_ethperf_info_snmp index ;
    int ret = 0;
    uint32_t sec_ifindex = 0;
    struct efm_ethperf_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index.ifindex, &sec_ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.efm_ethperf_info.efm_EthPerfNo = sec_ifindex ;

	
    if (NULL == efm_ethperf_info_cache)
    {
        efm_ethperf_info_cache = snmp_cache_init(sizeof(struct efm_ethperf_info_snmp) ,
                                                 efm_if_ethperf_info_get_data_from_ipc ,
                                                 efm_if_ethperf_info_node_lookup);

        if (NULL == efm_ethperf_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		index.efm_ethperf_info.efm_EthPerfNo = 1;
		efm_info = snmp_cache_get_data_by_index(efm_ethperf_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_ethperf_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, efm_info->ifindex, efm_info->efm_ethperf_info.efm_EthPerfNo);
    }

    *write_method = hpmcrtEthPerf_table_set;

    switch (vp->magic)
    {

        case hpmcrtEthPtNo:

            int_value = efm_info->efm_ethperf_info.efm_EthPerfNo;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtEthCntType:

            int_value = efm_info->efm_ethperf_info.efm_EthCntType;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtEthRxPCnt :
            int_value = efm_info->efm_ethperf_info.efm_EthRxPCnt;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtEthTxPCnt :
            int_value = efm_info->efm_ethperf_info.efm_EthTxPCnt;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case   hpmcrtEthRxBytes:
            memcpy(bits_num, efm_info->efm_ethperf_info.efm_EthRxBytes, BITS_NUM);
            *var_len = sizeof(bits_num) / sizeof(u_char);
            return (u_char *)bits_num;

        default :
            return (NULL);
    }

}


int efm_if_cpuperf_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                          struct efm_cpuperf_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct efm_cpuperf_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;
	
	pmesg = snmp_get_efm_agent_info_bulk(index->ifindex, &data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_CPUPERF_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_cpuperf_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_cpuperf_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_cpuperf_info_snmp *efm_if_cpuperf_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                              int exact,
                                                              const struct efm_cpuperf_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_cpuperf_info_snmp *data1_find = NULL;

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
hpmcrtCpuPerfTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_cpuperf_info_snmp index ;
    int ret = 0;
    struct efm_cpuperf_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == efm_cpuperf_info_cache)
    {
        efm_cpuperf_info_cache = snmp_cache_init(sizeof(struct efm_cpuperf_info_snmp) ,
                                                 efm_if_cpuperf_info_get_data_from_ipc ,
                                                 efm_if_cpuperf_info_node_lookup);

        if (NULL == efm_cpuperf_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		efm_info = snmp_cache_get_data_by_index(efm_cpuperf_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_cpuperf_info_cache, exact, &index);
		
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }


    *write_method = hpmcrtCpuPerf_table_set;

    switch (vp->magic)
    {

        case hpmcrtCpuCntType:
            int_value = efm_info->efm_cpuperf_info.efm_CpuCntType;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtCpuRxPCnt :
            int_value = efm_info->efm_cpuperf_info.efm_CpuRxPCnt;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case  hpmcrtCpuTxPCnt :
            int_value = efm_info->efm_cpuperf_info.efm_CpuTxPCnt;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case   hpmcrtCpuRxBytes:

            memcpy(bits_num, efm_info->efm_cpuperf_info.efm_CpuRxBytes, BITS_NUM);
            *var_len = sizeof(bits_num) / sizeof(u_char);
            return (u_char *)bits_num;


        default :
            return (NULL);
    }

}


int efm_if_oam_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                      struct efm_oam_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
	struct efm_oam_info_snmp *pefm_info = NULL;
	int data_num = 0;
	int ret = 0;
	
	pmesg = snmp_get_efm_agent_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_OAM_INFO_TYPE);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{
		pefm_info = (struct efm_oam_info_snmp *)(pmesg->msg_data);
		
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_oam_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_oam_info_snmp *efm_if_oam_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                      int exact,
                                                      const struct efm_oam_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_oam_info_snmp *data1_find;

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
hpmcrtOamTable_get(struct variable *vp,
                   oid *name,
                   size_t *length,
                   int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_oam_info_snmp index ;
    int ret = 0;
    struct efm_oam_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

	
    if (NULL == efm_oam_info_cache)
    {
        efm_oam_info_cache = snmp_cache_init(sizeof(struct efm_oam_info_snmp) ,
                                             efm_if_oam_info_get_data_from_ipc ,
                                             efm_if_oam_info_node_lookup);

        if (NULL == efm_oam_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]############ifindex = %x  if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex,ifindex_n);
	if((!index.ifindex) && ifindex_n)
	{
		index.ifindex = ifindex_n;
		efm_info = snmp_cache_get_data_by_index(efm_oam_info_cache , 1, &index);
	}
	else 
	{
    	efm_info = snmp_cache_get_data_by_index(efm_oam_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }


    *write_method = hpmcrtOamMode_table_set;

    switch (vp->magic)
    {

        case hpmcrtOamMode :
            int_value = efm_info->efm_oam_info.efm_OamMode;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hpmcrtOamLpbkSta :
            int_value = efm_info->efm_oam_info.efm_OamLpbkSta;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default :
            return (NULL);
    }

}



int efm_if_addr_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                       struct efm_addr_info_snmp  *index)
{
	struct ipc_mesg_n *pmesg = NULL;
    struct efm_addr_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
	pmesg = snmp_get_efm_agent_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_ADDR_INFO_TYPE);
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	if(pmesg)
	{	pefm_info = (struct efm_addr_info_snmp *)(pmesg->msg_data);
	
		for (ret = 0; ret < data_num; ret++)
		{
			snmp_cache_add(cache, pefm_info , sizeof(struct efm_addr_info_snmp));
			pefm_info++;
		}

		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return TRUE;
	}

	return FALSE;
}
struct efm_addr_info_snmp *efm_if_addr_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                        int exact,
                                                        const struct efm_addr_info_snmp  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node = NULL, *nnode = NULL;
    struct efm_addr_info_snmp *data1_find = NULL;

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
hpmcrtAddrTable_get(struct variable *vp,
                    oid *name,
                    size_t *length,
                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct efm_addr_info_snmp index ;
    int ret = 0;
    struct efm_addr_info_snmp *efm_info = NULL;
	uint32_t ifindex_n = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }
	
    if (NULL == efm_addr_info_cache)
    {
        efm_addr_info_cache = snmp_cache_init(sizeof(struct efm_addr_info_snmp) ,
                                              efm_if_addr_info_get_data_from_ipc ,
                                              efm_if_addr_info_node_lookup);

        if (NULL == efm_addr_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]###############ifindex = %x if_n = %x\n",__FUNCTION__,__LINE__,index.ifindex, ifindex_n);
	if(!index.ifindex && ifindex_n)
	{
		index.ifindex = ifindex_n;
		efm_info = snmp_cache_get_data_by_index(efm_addr_info_cache , 1, &index);
	}
	else
	{
    	efm_info = snmp_cache_get_data_by_index(efm_addr_info_cache , exact, &index);
		if(ifindex_n && efm_info  && efm_info->ifindex != ifindex_n)
		{
			return NULL;

		}
	}

    if (NULL == efm_info)
    {
        return NULL;
    }


    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
    }


    *write_method = hpmcrtAddrInfo_table_set;

    switch (vp->magic)
    {

        case hpmcrtIpAddr:
            memcpy(str_value, efm_info->efm_addr_info.efm_IpAddr, IP_NUM);
			zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]  IP = %d %d %d %d\n", __FUNCTION__, __LINE__,\
				str_value[0],str_value[1],str_value[2],str_value[3]);
            *var_len = IP_NUM;
            return (u_char *)str_value;

        case hpmcrtIpMask :
            memcpy(str_value, efm_info->efm_addr_info.efm_IpMask, IP_NUM);
            *var_len = IP_NUM;
            return (u_char *)str_value;

        case hpmcrtGatewayAddr:
            memcpy(str_value, efm_info->efm_addr_info.efm_GatewayAddr, IP_NUM);
            *var_len = IP_NUM;
            return (u_char *)str_value;

        case  hpmcrtMacAddr:
            memcpy(mac_value, efm_info->efm_addr_info.efm_MacAddr, MAC_NUM);
            *var_len = sizeof(mac_value) / sizeof(uchar);
            return (u_char *)mac_value;


        default :
            return (NULL);
    }

}



int efm_if_conver_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                         struct efm_conver_info_snmp  *index)
{
		struct ipc_mesg_n *pmesg = NULL;
    struct efm_conver_info_snmp *pefm_info = NULL;
    int data_num = 0;
    int ret = 0;
		pmesg = snmp_get_efm_agent_info_bulk(index->ifindex,&data_num, IPC_TYPE_SNMP_EFM_AGENT_INFO, EFM_CONVER_INFO_TYPE);
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

		if(pmesg)
		{
			pefm_info = (struct efm_conver_info_snmp *)(pmesg->msg_data);
			
			for (ret = 0; ret < data_num; ret++)
			{
				snmp_cache_add(cache, pefm_info , sizeof(struct efm_conver_info_snmp));
				pefm_info++;
			}
	
			mem_share_free(pmesg, MODULE_ID_SNMPD);
			return TRUE;
		}

			return FALSE;
	}
	
	struct efm_conver_info_snmp *efm_if_conver_info_node_lookup(struct ipran_snmp_data_cache *cache ,
			int exact,
			const struct efm_conver_info_snmp  *index_input)
	{
		zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
		struct listnode  *node, *nnode;
		struct efm_conver_info_snmp *data1_find;
	
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
	hpmcrtConverTable_get(struct variable *vp,
			oid *name,
			size_t *length,
			int exact, size_t *var_len, WriteMethod **write_method)
	{
		struct efm_conver_info_snmp index ;
		int ret = 0;
		uint8_t temp = 0;
		struct efm_conver_info_snmp *efm_info = NULL;
	
		zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]	exact %d\n", __FUNCTION__, __LINE__, exact);
	
		/* validate the index */
		ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);
	
		if (ret < 0)
		{
			return NULL;
		}
	
		if (NULL == efm_conver_info_cache)
		{
			efm_conver_info_cache = snmp_cache_init(sizeof(struct efm_conver_info_snmp) ,
					efm_if_conver_info_get_data_from_ipc ,
					efm_if_conver_info_node_lookup);
	
			if (NULL == efm_conver_info_cache)
			{
				zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
				return (NULL);
			}
		}
	
		efm_info = snmp_cache_get_data_by_index(efm_conver_info_cache , exact, &index);
	
		if (NULL == efm_info)
		{
			return NULL;
		}
	
	
		/* get ready the next index */
		if (!exact)
		{
			ipran_snmp_int_index_set(vp, name, length, efm_info->ifindex);
		}
			
		switch (vp->magic)
		{
	
			case  hpmcrtIpConverterMode:
				int_value = efm_info->efm_conver;
				*var_len = sizeof(int_value);
				return (u_char *)&int_value;
		
			default :
				return (NULL);
		}
	
	}

int
hpmcrtCommInfo_table_set(int action, u_char *var_val,
                         u_char var_val_type, size_t var_val_len,
                         u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER &&  var_val_type != ASN_OCTET_STR)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

	/*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		 index.ifindex = ifindex_n;
	}

	else
	{
    	/* validate the index */
    	index.ifindex = name[name_len - 1];
		
	}
	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]###############ifindex = %x\n",__FUNCTION__,__LINE__,index.ifindex);
    //index1 = name[name_len-2];
    //index2 = name[name_len-1];
    pdata[data_num++] = index.ifindex;         //one ifindex
    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value



    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d   var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/
    switch (name[name_len - 2])
    {
        case hpmcrtDataResum:
            pdata[data_num++] = hpmcrtDataResum;
            break;

        case hpmcrtLFPCtrl:
            pdata[data_num++] = hpmcrtLFPCtrl;
            break;

        case hpmcrtALSCtrl:
            pdata[data_num++] = hpmcrtALSCtrl;
            break;

        case  hpmcrtJumbo:
            pdata[data_num++] = hpmcrtJumbo;
            break;

        case hpmcrtReboot:
            pdata[data_num++] = hpmcrtReboot;
            break;

        case hpmcrtIpCtrl:
            pdata[data_num++] =  hpmcrtIpCtrl;
            break;

        case hpmcrtEthStromfilter:
            if (var_val_len > 1 || *var_val > 7 || *var_val < 0)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[1] = *var_val;
            pdata[data_num++] =  hpmcrtEthStromfilter;
            break;

        case hpmcrtDHCPclient:
            pdata[data_num++] =  hpmcrtDHCPclient;
            break;

        default:
            break;
    }

    pdata[data_num++] = hpmcrtCommConf_set;                    //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    memset(pdata, 0, sizeof(pdata));
    return SNMP_ERR_NOERROR;
}
int
hpmcrtVlanMode_table_set(int action, u_char *var_val,
                         u_char var_val_type, size_t var_val_len,
                         u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

	/*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		 index.ifindex = ifindex_n;
	}
	else
	{
    	/* validate the index */
    	index.ifindex = name[name_len - 1];
	}
    //index1 = name[name_len-2];
    //index2 = name[name_len-1];
    pdata[data_num++] = index.ifindex;         //one ifindex
    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value



    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/

    pdata[data_num++] = hpmcrtVlanMode;
    pdata[data_num++] = hpmcrtVlanConf_set;                    //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtOltptVlan_table_set(int action, u_char *var_val,
                          u_char var_val_type, size_t var_val_len,
                          u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

	/*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		 index.ifindex = ifindex_n;
	}
	else
	{
		  /* validate the index */
   		 index.ifindex = name[name_len - 1];

	}
 
    //index1 = name[name_len-2];
    //index2 = name[name_len-1];
    pdata[data_num++] = index.ifindex;         //one ifindex
    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/
    switch (name[name_len - 2])
    {
        case hpmcrtOltPtInMode:
            pdata[data_num++] = hpmcrtOltPtInMode;
            break;

        case hpmcrtOltPtPVID:
            if (pdata[1] > 4094 || pdata[1] < 1)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[data_num++] = hpmcrtOltPtPVID;
            break;

        case hpmcrtOltPtPrior:
            if (pdata[1] > 7 || pdata[1] < 0)
            {
                return SNMP_ERR_GENERR;
            }

            pdata[data_num++] = hpmcrtOltPtPrior;
            break;

        case hpmcrtOltPtTagMode:
            pdata[data_num++] = hpmcrtOltPtTagMode;
            break;

        default:
            break;
    }


    pdata[data_num++] = hpmcrtOltPtVlanConf_set;                   //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtCpuptVlan_table_set(int action, u_char *var_val,
                          u_char var_val_type, size_t var_val_len,
                          u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

	/*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		 index.ifindex = ifindex_n;
	}
	else 
	{
		  /* validate the index */
    	index.ifindex = name[name_len - 1];

	}
  
    //index1 = name[name_len-2];
    //index2 = name[name_len-1];
    pdata[data_num++] = index.ifindex;         //one ifindex

    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value



    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/

    if (hpmcrtCpuPtPVID != name[name_len - 2] || pdata[1] > 4094 || pdata[1] < 1)
    {

        return SNMP_ERR_WRONGTYPE;

    }

    pdata[data_num++] = hpmcrtCpuPtPVID;
    pdata[data_num++] = hpmcrtCpuPtVlanConf_set;               //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtQinq_table_set(int action, u_char *var_val,
                     u_char var_val_type, size_t var_val_len,
                     u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER  && var_val_type != ASN_UNSIGNED)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

   /*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		 index.ifindex = ifindex_n;
	}
	else 
	{
		  /* validate the index */
    	index.ifindex = name[name_len - 1];

	}

    //index1 = name[name_len-2];
    //index2 = name[name_len-1];
    pdata[data_num++] = index.ifindex;         //one ifindex

    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/

    switch (name[name_len - 2])
    {
        case hpmcrtQinQDirection:
            pdata[data_num++] = hpmcrtQinQDirection;
            break;

        case hpmcrtQinQTPID:
            if (pdata[1] > 65535 || pdata[1] < 0)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[data_num++] = hpmcrtQinQTPID;
            break;

        case hpmcrtQinQVid:
            if (pdata[1] > 4094 || pdata[1] < 1)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[data_num++] = hpmcrtQinQVid;
            break;

        case hpmcrtQinQPrior:
            if (pdata[1] > 7 || pdata[1] < 0)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[data_num++] = hpmcrtQinQPrior;
            break;

    }

    pdata[data_num++] = hpmcrtQinQ_set;                //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtQos_table_set(int action, u_char *var_val,
                    u_char var_val_type, size_t var_val_len,
                    u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

   /*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		 index.ifindex = ifindex_n;
	}
	else 
	{
		  /* validate the index */
    	index.ifindex = name[name_len - 1];

	}

    //index1 = name[name_len-2];
    //index2 = name[name_len-1];
    pdata[data_num++] = index.ifindex;         //one ifindex

    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value


    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/

    if (hpmcrt1pQos != name[name_len - 2])
    {
        return SNMP_ERR_GENERR;

    }

    pdata[data_num++]  = hpmcrt1pQos;
    pdata[data_num++] = hpmcrtQoS_set;             //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtPerf_table_set(int action, u_char *var_val,
                     u_char var_val_type, size_t var_val_len,
                     u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

   /*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		 index.ifindex = ifindex_n;
	}
	else 
	{
		  /* validate the index */
    	index.ifindex = name[name_len - 1];

	}

    //index1 = name[name_len-2];
    //index2 = name[name_len-1];
    pdata[data_num++] = index.ifindex;         //one ifindex

    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value


    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/

    if (hpmcrtCleanCounter != name[name_len - 2])
    {
        return SNMP_ERR_GENERR;

    }

    pdata[data_num++]  = hpmcrtCleanCounter;
    pdata[data_num++] = hpmcrtPerf_set;            //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtOltPerf_table_set(int action, u_char *var_val,
                        u_char var_val_type, size_t var_val_len,
                        u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

   /*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		 index.ifindex = ifindex_n;
	}
	else 
	{
		  /* validate the index */
    	index.ifindex = name[name_len - 1];

	}

    //index1 = name[name_len-2];
    //index2 = name[name_len-1];
    pdata[data_num++] = index.ifindex;         //one ifindex

    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value


    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/

    if (hpmcrtOltCntType != name[name_len - 2])
    {
        return SNMP_ERR_GENERR;

    }

    pdata[data_num++]  = hpmcrtOltCntType;
    pdata[data_num++] = hpmcrtOltPerf_set;             //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtEthPerf_table_set(int action, u_char *var_val,
                        u_char var_val_type, size_t var_val_len,
                        u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

	/*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		 index.ifindex = ifindex_n;
		  pdata[data_num++] = 1;
	}

	else
	{
		 /* validate the index */
   		 index.ifindex = name[name_len - 2];

    	//index1 = name[name_len-2];
   		 //index2 = name[name_len-1];
    	pdata[data_num++] = name[name_len - 1];        //one ifindex

	}
   

    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value


    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/

    if (hpmcrtEthCntType != name[name_len - 3])
    {
        return SNMP_ERR_GENERR;

    }

    pdata[data_num++]  = hpmcrtEthCntType;
    pdata[data_num++] = hpmcrtEthPtPerf_set;           //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtCpuPerf_table_set(int action, u_char *var_val,
                        u_char var_val_type, size_t var_val_len,
                        u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

	/*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		index.ifindex = ifindex_n;
		  
	}

	else
	{    /* validate the index */
    	index.ifindex = name[name_len - 1];
		
	}
	


    //index1 = name[name_len-2];
    //index2 = name[name_len-1];
    pdata[data_num++] = index.ifindex;         //one ifindex

    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value


    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/

    if (hpmcrtCpuCntType != name[name_len - 2])
    {
        return SNMP_ERR_GENERR;

    }

    pdata[data_num++]  = hpmcrtCpuCntType;
    pdata[data_num++] = hpmcrtCpuPerf_set;         //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtOamMode_table_set(int action, u_char *var_val,
                        u_char var_val_type, size_t var_val_len,
                        u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    
	/*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		index.ifindex = ifindex_n;
		  
	}

	else
	{    /* validate the index */
    	index.ifindex = name[name_len - 1];
		
	}

    //index1 = name[name_len-2];
    //index2 = name[name_len-1];
    pdata[data_num++] = index.ifindex;         //one ifindex
    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value



    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/

    pdata[data_num++] = hpmcrtOamMode;
    pdata[data_num++] = hpmcrtOam_set;                 //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtAddrInfo_table_set(int action, u_char *var_val,
                         u_char var_val_type, size_t var_val_len,
                         u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type !=  ASN_IPADDRESS)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    
	/*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		index.ifindex = ifindex_n;
		  
	}

	else
	{    /* validate the index */
    	index.ifindex = name[name_len - 1];
		
	}

    //index1 = name[name_len-2];
    //index2 = name[name_len-1];
    pdata[data_num++] = index.ifindex;         //one ifindex
    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value


    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/
    switch (name[name_len - 2])
    {
        case hpmcrtIpAddr:
            pdata[data_num++] = hpmcrtOamMode;
            break;

        case hpmcrtIpMask:
            pdata[data_num++] = hpmcrtIpMask;
            break;

        case hpmcrtGatewayAddr:
            pdata[data_num++] = hpmcrtGatewayAddr;
            break;

        default:
            return SNMP_ERR_GENERR;

    }

    pdata[data_num++] = hpmcrtAddr_set;                //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtOltptInfo_table_set(int action, u_char *var_val,
                          u_char var_val_type, size_t var_val_len,
                          u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

	
	/*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		index.ifindex = ifindex_n;
		pdata[data_num++] = 1; 
	}

	else
	{     /* validate the index */
    	index.ifindex = name[name_len - 2];

    	//index1 = name[name_len-2];
    	//index2 = name[name_len-1];
   		 pdata[data_num++] = name[name_len - 1];       //sec ifindex
		
	}
	
   
    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value



    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d   var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/
    switch (name[name_len - 3])
    {
        case hpmcrtOltPtNegCtrl:
            pdata[data_num++] = hpmcrtOltPtNegCtrl;
            break;

        case hpmcrtOltPtSpdCtrl:
            pdata[data_num++] = hpmcrtOltPtSpdCtrl;
            break;

        case hpmcrtOltPtDplCtrl:
            pdata[data_num++] = hpmcrtOltPtDplCtrl;
            break;

        case  hpmcrtOltPtDisCtrl:
            pdata[data_num++] = hpmcrtOltPtDisCtrl;
            break;

        case hpmcrtOltPtIngressRateN:
            if (pdata[1] > 16000 || pdata[1] < 0)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[data_num++] = hpmcrtOltPtIngressRateN;
            break;

        case hpmcrtOltPtEgressRateN:
            if (pdata[1] > 16000 || pdata[1] < 0)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[data_num++] =  hpmcrtOltPtEgressRateN;
            break;

        case hpmcrtOltPtFlowCtrl:
            pdata[data_num++] =  hpmcrtOltPtFlowCtrl;
            break;

        default:
            break;
    }

    pdata[data_num++] = hpmcrtOltPtConf_set;                    //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtEthptInfo_table_set(int action, u_char *var_val,
                          u_char var_val_type, size_t var_val_len,
                          u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

	
    	/*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		index.ifindex = ifindex_n;
		pdata[data_num++] = 1; 
	}

	else
	{     /* validate the index */
    	index.ifindex = name[name_len - 2];

    	//index1 = name[name_len-2];
    	//index2 = name[name_len-1];
   		 pdata[data_num++] = name[name_len - 1];       //sec ifindex
		
	}
    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value



    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/
    switch (name[name_len - 3])
    {
        case hpmcrtEthPtNegCtrl:
            pdata[data_num++] = hpmcrtEthPtNegCtrl;
            break;

        case hpmcrtEthPtSpdCtrl:
            pdata[data_num++] = hpmcrtEthPtSpdCtrl;
            break;

        case hpmcrtEthPtDplCtrl:
            pdata[data_num++] = hpmcrtEthPtDplCtrl;
            break;

        case  hpmcrtEthPtDisCtrl:
            pdata[data_num++] = hpmcrtEthPtDisCtrl;
            break;

        case hpmcrtEthPtIngressRateN:
            if (pdata[1] > 16000 || pdata[1] < 0)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[data_num++] = hpmcrtEthPtIngressRateN;
            break;

        case hpmcrtEthPtEgressRateN:
            if (pdata[1] > 16000 || pdata[1] < 0)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[data_num++] =  hpmcrtEthPtEgressRateN;
            break;

        case hpmcrtEthPtFlowCtrl:
            pdata[data_num++] =  hpmcrtEthPtFlowCtrl;
            break;

        default:
            break;
    }

    pdata[data_num++] = hpmcrtEthPtConf_set;                   //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}


int
hpmcrtEthptVlan_table_set(int action, u_char *var_val,
                          u_char var_val_type, size_t var_val_len,
                          u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    	/*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		index.ifindex = ifindex_n;
		pdata[data_num++] = 1; 
	}

	else
	{     /* validate the index */
    	index.ifindex = name[name_len - 2];

    	//index1 = name[name_len-2];
    	//index2 = name[name_len-1];
   		 pdata[data_num++] = name[name_len - 1];       //sec ifindex
		
	}
	
    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value



    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d	 var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/
    switch (name[name_len - 3])
    {
        case hpmcrtEthPtInMode:
            pdata[data_num++] = hpmcrtEthPtInMode;
            break;

        case hpmcrtEthPtPVID:
            if (pdata[1] > 4094 || pdata[1] < 1)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[data_num++] = hpmcrtEthPtPVID;
            break;

        case hpmcrtEthPtPrior:
            if (pdata[1] > 7 || pdata[1] < 0)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[data_num++] = hpmcrtEthPtPrior;
            break;

        case hpmcrtEthPtTagMode:
            pdata[data_num++] = hpmcrtEthPtTagMode;
            break;

        default:
            break;
    }

    pdata[data_num++] = hpmcrtEthPtVlanConf_set;                   //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
hpmcrtVlanGroup_table_set(int action, u_char *var_val,
                          u_char var_val_type, size_t var_val_len,
                          u_char *statP, oid *name, size_t name_len)
{
    struct efm_comm_info_snmp index ;
    uint32_t intval = 0;
    uint32_t errcode = 0;
    uint32_t pdata[4] = {0};
    uint8_t data_num = 0;
    uint8_t data_len = 0;
	uint32_t ifindex_n = 0;

    if (var_val == NULL)
    {
        return SNMP_ERR_WRONGVALUE;
    }

    if (var_val_type != ASN_INTEGER && var_val_type != ASN_OCTET_STR)
    {
        return SNMP_ERR_WRONGTYPE;
    }

    if (var_val_len > sizeof(uint32_t))
    {
        return SNMP_ERR_WRONGLENGTH;
    }

    	/*only config IP---port info*/
	ifindex_n = snmp_U0_hash_get_ifindex_by_ip(g_snmp_U0_ipv4);
	if (ifindex_n)
	{
		index.ifindex = ifindex_n;
	}

	else
	{     /* validate the index */
    	index.ifindex = name[name_len - 2];
		
	}
	//index1 = name[name_len-2];
    //index2 = name[name_len-1];
   	pdata[data_num++] = name[name_len - 1];       //sec ifindex

    memcpy(&pdata[data_num++], var_val, var_val_len);                  //get set value
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]####### ifindex =%02x   var_val = %d   var_val_len = %d", __FUNCTION__, __LINE__, \
               index.ifindex, *var_val, var_val_len);

    /*need to konw config table who's item*/
    switch (name[name_len - 3])
    {
        case hpmcrtVlanId:
            if (pdata[1] > 4094  || pdata[1] < 1)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[data_num++] = hpmcrtVlanId;
            break;

        case hpmcrtVlanMember:

            if (var_val_len > 1 || *var_val  > 7 || *var_val < 0)
            {
                return SNMP_ERR_WRONGTYPE;
            }

            pdata[1] = *var_val;
            pdata[data_num++] = hpmcrtVlanMember;
            break;

        default:
            break;
    }

    pdata[data_num++] = hpmcrtVlanGroupConf_set;                    //need to know config who's table
    data_len = data_num *  sizeof(uint32_t);

    //send ipc to l2 (index and retval), if set success, return 0, else return 1 or -1;
    errcode = snmp_send_SetInfo_to_efmagent(pdata, data_len, data_num, index.ifindex);

    if (errcode < 0)
    {
        return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int snmp_send_SetInfo_to_efmagent(void *pdata, int data_len, int data_num, uint32_t msg_ifindex)
{
    int errcode = 0;
	#if 0
	
	errcode = ipc_send_common_wait_ack(pdata, data_len, data_num, MODULE_ID_L2, MODULE_ID_SNMPD, IPC_TYPE_SNMP, \
	                  IPC_TYPE_SNMP_EFM_AGENT_INFO, IPC_OPCODE_UPDATE, msg_ifindex);	    
	#endif
	errcode = snmp_config_msg_send_l2_wait_ack(pdata, data_len,data_num,
		IPC_TYPE_SNMP_EFM_AGENT_INFO, IPC_OPCODE_UPDATE, msg_ifindex);
	if(errcode < 0)
	{
		zlog_err("%s[%d]####snmp set efmagent info FAIL!",__FUNCTION__,__LINE__);		
	}
	return errcode;
}

struct ipc_mesg_n   *snmp_get_efm_agent_info_bulk(uint32_t ifindex,int *pdata_num, int msg_subtype, int data_num)
{
	
	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, data_num, MODULE_ID_L2, MODULE_ID_SNMPD,
			IPC_TYPE_SNMP, msg_subtype, IPC_OPCODE_GET, ifindex,5000);
	if (pmesg)
	{
		*pdata_num = pmesg->msghdr.data_num;
		return pmesg;
	}

    return NULL;
}


struct ipc_mesg_n  *snmp_get_efm_agent_SecIfindex_info_bulk(void *pdata, int data_len, uint32_t ifindex,int *pdata_num, int msg_subtype, int data_num)
{
	
	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pdata, data_len, data_num, MODULE_ID_L2, MODULE_ID_SNMPD,
			IPC_TYPE_SNMP, msg_subtype, IPC_OPCODE_GET, ifindex,5000);
	
	if (pmesg)
	{
		*pdata_num = pmesg->msghdr.data_num;
		return pmesg;
	}



    return NULL;
}

/* **************************************************************************
 *  Function : snmp_config_msg_send_l2_wait_ack
 *  return   : 0 ok, 1 error
 *  description: snmp send config info to l2 and confirm config success or fail 
 * **************************************************************************/
/*send info confirm ack or noack*/
int snmp_config_msg_send_l2_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num,
		 uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
		int ret = 0;
		int rcvlen = 0;
		struct ipc_mesg_n *pSndMsg = NULL;
		struct ipc_mesg_n * pRcvMsg = NULL;
		
		pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_SNMPD);
		if(pSndMsg != NULL)
		{
			pSndMsg->msghdr.data_len    = data_len;
			pSndMsg->msghdr.module_id   = MODULE_ID_L2;
			pSndMsg->msghdr.sender_id   = MODULE_ID_SNMPD;
			pSndMsg->msghdr.msg_type    = IPC_TYPE_SNMP;
			pSndMsg->msghdr.msg_subtype = msg_subtype;
			pSndMsg->msghdr.msg_index   = msg_index;
			pSndMsg->msghdr.data_num    = data_num;
			pSndMsg->msghdr.opcode      = opcode;
				
			if(pdata)
			{
				memcpy(pSndMsg->msg_data, pdata, data_len);
			}
			
			/*send info*/
			ret = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n) + data_len, &pRcvMsg, &rcvlen, 5000);
				
			switch(ret)
			{
				/*send fail*/
				case -1:
				{
					ipc_msg_free(pSndMsg,MODULE_ID_SNMPD);
					return -1;
				}
				case -2:
				{
					/*recv fail*/
					return -1;
								
				}
				case 0:
				{	/*recv success*/
					if(pRcvMsg != NULL)
					{	
						/*if noack return errcode*/
						if(IPC_OPCODE_NACK == pRcvMsg->msghdr.opcode)
						{
							ret = -1;      
						}
						/*if ack return 0*/
						else if(IPC_OPCODE_ACK == pRcvMsg->msghdr.opcode)
						{
							ret = 0;
						}
						else
						{
							ret = -1;
						}
						//ret = (int)pRcvMsg->msg_data;
						ipc_msg_free(pRcvMsg,MODULE_ID_SNMPD);
					}
					else
					{
						return -1;
					}
				}
				default:
					return -1;
							
			}
			
		}
		else
		{
			return -1;
		}
	return(ret);
}      



