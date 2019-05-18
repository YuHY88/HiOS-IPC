#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <net-snmp-config.h>
#include <types.h>
#include <net-snmp-includes.h>
#include <net-snmp-agent-includes.h>
#include <snmp_index_operater.h>

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/ifm_common.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc_n.h>
#include <lib/types.h>
#include <lib/route_com.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/inet_ip.h>
#include <lib/linklist.h>
#include <qos/sla/sla_session.h>
#include "mib_sla.h"
#include <lib/log.h>
#include "snmp_config_table.h"

#include "ipran_snmp_data_cache.h"

/*local temp variable*/
static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value = 0;
static uchar mac_value[6] = {0};
static uint32_t           uint_value = 0;
static uint32_t           ip_value = 0;

static struct ipran_snmp_data_cache *hhrslaMeasureConfigTable_cache = NULL ;
static struct ipran_snmp_data_cache *hhrrfc2544MeasureResultTable_cache = NULL ;
static struct ipran_snmp_data_cache *y1564Table_cache = NULL ;

#define                   SLA_STRING_LEN         256
static int  hhrslaMeasureConfigTable_time_old  = 0;

FindVarMethod hhrslaMeasureConfigTable_get ;
oid  hhrslaMeasureConfigTable_oid[]   = { MIB_HHRLSA_ROOT , 10};
int  hhrslaMeasureConfigTable_oid_len = sizeof(hhrslaMeasureConfigTable_oid) / sizeof(oid);
struct variable2 hhrslaMeasureConfigTable_variables[] =
{
    {hhrslaMeasureProtocol,           ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1,  2}},
    {hhrslaMeasureL2dmac,             ASN_OCTET_STR,  RONLY, hhrslaMeasureConfigTable_get, 2, {1,  3}},
    {hhrslaMeasureL2cvlan,            ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1,  4}},
    {hhrslaMeasureL2svlan,            ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1,  5}},
    {hhrslaMeasureL2cos,              ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1,  6}},
    {hhrslaMeasureL3sip,              ASN_IPADDRESS,  RONLY, hhrslaMeasureConfigTable_get, 2, {1,  7}},
    {hhrslaMeasureL3dip,              ASN_IPADDRESS,  RONLY, hhrslaMeasureConfigTable_get, 2, {1,  8}},
    {hhrslaMeasureL3sport,            ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1,  9}},
    {hhrslaMeasureL3dport,            ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1, 10}},
    {hhrslaMeasureL3ttl,              ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1, 11}},
    {hhrslaMeasureL3dscp,             ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1, 12}},
    {hhrslaMeasurePacketSize,         ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1, 13}},
    {hhrslaMeasurePacketRate,         ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1, 14}},
    {hhrslaMeasureMeasurePacketLoss,  ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1, 15}},
    {hhrslaMeasureMeasurePacketDelay, ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1, 16}},
    {hhrslaMeasureMeasureThroughput,  ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1, 17}},
    {hhrslaMeasureScheduleInterval,   ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1, 18}},
    {hhrslaMeasureScheduleFrequency,  ASN_UNSIGNED,   RONLY, hhrslaMeasureConfigTable_get, 2, {1, 19}},
    {hhrslaMeasureMeasureIfDescr,     ASN_OCTET_STR,  RONLY, hhrslaMeasureConfigTable_get, 2, {1, 20}},
    {hhrslaMeasureL2smac,             ASN_OCTET_STR,  RONLY, hhrslaMeasureConfigTable_get, 2, {1, 21}},
    {hhrslaMeasureSACLos,             ASN_UNSIGNED,   RONLY, hhrslaMeasureConfigTable_get, 2, {1, 22}},
    {hhrslaMeasureSACDelay,           ASN_UNSIGNED,   RONLY, hhrslaMeasureConfigTable_get, 2, {1, 23}},
    {hhrslaMeasureSACJitter,          ASN_UNSIGNED,   RONLY, hhrslaMeasureConfigTable_get, 2, {1, 24}},
    {hhrslaMeasureL2ctpid,            ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1, 25}},
    {hhrslaMeasureL2stpid,            ASN_INTEGER,    RONLY, hhrslaMeasureConfigTable_get, 2, {1, 26}},
    {hhrslaMeasureL2scos,             ASN_UNSIGNED,   RONLY, hhrslaMeasureConfigTable_get, 2, {1, 27}},
    {hhrslaMeasureCir,                ASN_UNSIGNED,   RONLY, hhrslaMeasureConfigTable_get, 2, {1, 28}},
    {hhrslaMeasureEir,                ASN_UNSIGNED,   RONLY, hhrslaMeasureConfigTable_get, 2, {1, 29}},
    {hhrslaMeasureRFC2544PacketSize,  ASN_OCTET_STR,  RONLY, hhrslaMeasureConfigTable_get, 2, {1, 30}},
    {hhrslaMeasureY1564PacketSize,    ASN_UNSIGNED,   RONLY, hhrslaMeasureConfigTable_get, 2, {1, 31}},
    {hhrslaMeasureDuration,           ASN_UNSIGNED,   RONLY, hhrslaMeasureConfigTable_get, 2, {1, 32}}
} ;

FindVarMethod hhrrfc2544MeasureResultTable_get ;
oid  hhrrfc2544MeasureResultTable_oid[]   = { MIB_HHRLSA_ROOT , 11};
int  hhrrfc2544MeasureResultTable_oid_len = sizeof(hhrrfc2544MeasureResultTable_oid) / sizeof(oid);
struct variable2 hhrrfc2544MeasureResultTable_variables[] =
{
    {hhrrfc2544MeasureSessionStatus,  ASN_INTEGER,    RONLY, hhrrfc2544MeasureResultTable_get, 2, {1, 2}},
    {hhrrfc2544MeasureFLRMin,         ASN_UNSIGNED,   RONLY, hhrrfc2544MeasureResultTable_get, 2, {1, 3}},
    {hhrrfc2544MeasureFLRMax,         ASN_UNSIGNED,   RONLY, hhrrfc2544MeasureResultTable_get, 2, {1, 4}},
    {hhrrfc2544MeasureFLRMean,        ASN_UNSIGNED,   RONLY, hhrrfc2544MeasureResultTable_get, 2, {1, 5}},
    {hhrrfc2544MeasureFTDMin,         ASN_UNSIGNED,   RONLY, hhrrfc2544MeasureResultTable_get, 2, {1, 6}},
    {hhrrfc2544MeasureFTDMax,         ASN_UNSIGNED,   RONLY, hhrrfc2544MeasureResultTable_get, 2, {1, 7}},
    {hhrrfc2544MeasureFTDMean,        ASN_UNSIGNED,   RONLY, hhrrfc2544MeasureResultTable_get, 2, {1, 8}},
    {hhrrfc2544MeasureThroughput,     ASN_UNSIGNED,   RONLY, hhrrfc2544MeasureResultTable_get, 2, {1, 9}}
} ;


FindVarMethod hhry1564MeasureResultTable_get ;
oid  hhry1564MeasureResultTable_oid[]   = { MIB_HHRLSA_ROOT , 12};
int  hhry1564MeasureResultTable_oid_len = sizeof(hhry1564MeasureResultTable_oid) / sizeof(oid);
struct variable2 hhry1564MeasureResultTable_variables[] =
{
    {hhry1564Measurecir,           ASN_INTEGER,    RONLY, hhry1564MeasureResultTable_get, 2, {1,  2}},
    {hhry1564MeasureSessionStatus, ASN_INTEGER,    RONLY, hhry1564MeasureResultTable_get, 2, {1,  3}},
    {hhry1564MeasureFLRMin,        ASN_UNSIGNED,   RONLY, hhry1564MeasureResultTable_get, 2, {1,  4}},
    {hhry1564MeasureFLRMax,        ASN_UNSIGNED,   RONLY, hhry1564MeasureResultTable_get, 2, {1,  5}},
    {hhry1564MeasureFLRMean,       ASN_UNSIGNED,   RONLY, hhry1564MeasureResultTable_get, 2, {1,  6}},
    {hhry1564MeasureFTDMin,        ASN_UNSIGNED,   RONLY, hhry1564MeasureResultTable_get, 2, {1,  7}},
    {hhry1564MeasureFTDMax,        ASN_UNSIGNED,   RONLY, hhry1564MeasureResultTable_get, 2, {1,  8}},
    {hhry1564MeasureFTDMean,       ASN_UNSIGNED,   RONLY, hhry1564MeasureResultTable_get, 2, {1,  9}},
    {hhry1564MeasureFDVMin,        ASN_UNSIGNED,   RONLY, hhry1564MeasureResultTable_get, 2, {1, 10}},
    {hhry1564MeasureFDVMax,        ASN_UNSIGNED,   RONLY, hhry1564MeasureResultTable_get, 2, {1, 11}},
    {hhry1564MeasureFDVMean,       ASN_UNSIGNED,   RONLY, hhry1564MeasureResultTable_get, 2, {1, 12}}
};


void  init_mib_sla(void)
{
    REGISTER_MIB("hhrslaMeasureConfigTable", hhrslaMeasureConfigTable_variables, variable2,
                 hhrslaMeasureConfigTable_oid) ;
    REGISTER_MIB("hhrrfc2544MeasureResultTable", hhrrfc2544MeasureResultTable_variables, variable2,
                 hhrrfc2544MeasureResultTable_oid) ;
    REGISTER_MIB("hhry1564MeasureResultTable", hhry1564MeasureResultTable_variables, variable2,
                 hhry1564MeasureResultTable_oid) ;
}

struct ipc_mesg_n *sla_com_get_session_instance(uint16_t session_id, int module_id, int *pdata_num)
{
#if 0
    struct ipc_mesg *pmesg = NULL ;

    if (session_id == 0)
    {
        pmesg = ipc_send_common_wait_reply1(&session_id, 0, 1, MODULE_ID_QOS,
                                            module_id, IPC_TYPE_SLA, 0 , IPC_OPCODE_GET_BULK , session_id);
    }
    else
    {
        pmesg = ipc_send_common_wait_reply1(&session_id, 4, 1, MODULE_ID_QOS,
                                            module_id, IPC_TYPE_SLA, 0 , IPC_OPCODE_GET_BULK , session_id);
    }

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct sla_session *)pmesg->msg_data;
    }

    return NULL;
#endif

	struct ipc_mesg_n *pmsg = NULL;

	if(0 == session_id)
	{
		pmsg = ipc_sync_send_n2(&session_id, 0, 1, MODULE_ID_QOS, module_id,
			IPC_TYPE_SLA, 0 , IPC_OPCODE_GET_BULK , session_id ,2000);
	}
	else
	{
		pmsg = ipc_sync_send_n2(&session_id, 4, 1, MODULE_ID_QOS, module_id,
			IPC_TYPE_SLA, 0, IPC_OPCODE_GET_BULK, session_id ,2000);
	}

    if(pmsg)
    {
        *pdata_num = pmsg->msghdr.data_num;
        return pmsg;
    }

    return NULL;
}

struct ipc_mesg_n *sla_com_get_if_info(int module_id)
{
#if 0
    return  ipc_send_common_wait_reply(NULL, 0, 1, MODULE_ID_QOS, module_id,
                                       IPC_TYPE_SLA, SLA_SUBTYPE_SESS_IF_INFO, IPC_OPCODE_GET, 0);
#endif

	return ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_QOS, module_id,
		IPC_TYPE_SLA, SLA_SUBTYPE_SESS_IF_INFO, IPC_OPCODE_GET, 0,2000);	
}

int hhrslaMeasureConfigTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache , struct sla_session  *index)
{    
	struct sla_session *psla_session = NULL;    
    struct ipc_mesg_n *pmesg = NULL ;
	int data_num = 0;    
	int ret = 0;    
	
	pmesg = sla_com_get_session_instance(index->info.session_id, MODULE_ID_SNMPD, &data_num);   
	zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index->info.session_id [%d]data_num [%d]\n", __FILE__, __LINE__, __func__ , index->info.session_id, data_num);  

	if (0 == data_num || NULL == pmesg)    
	{        
		return FALSE;    
	}    
	else    
	{ 
		psla_session = (struct sla_session*)pmesg->msg_data;

		for (ret = 0; ret < data_num; ret++)        
		{            
			zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s':  psla_session->info.proto [%d]\n", __FILE__, __LINE__, __func__ , psla_session->info.proto);            	
			snmp_cache_add(cache, psla_session , sizeof(struct sla_session));           
			psla_session++;        
		}        
		
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		//time(&cache->getNext_time_old);        
		return TRUE;    
	}
}

int hhrrfc2544MeasureResultTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                               struct ipran_snmp_sla_rfc2544_table  *index)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
    struct sla_session *psla_session = NULL;
	struct ipran_snmp_sla_rfc2544_table *tmp_data = NULL;
	int count = 0;
    int data_num = 0;
	int loaded_data = 0 ;
    int ret = 0;
	
    pMsgRcv = sla_com_get_session_instance(index->old_data.info.session_id, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index->info.session_id [%d]data_num [%d]\n", __FILE__, __LINE__, __func__ ,
									index->old_data.info.session_id, data_num);

    if (0 == data_num || NULL == pMsgRcv)
    {
        return FALSE;
    }
    else
    {
		psla_session = (struct sla_session *)pMsgRcv->msg_data;
		
    	tmp_data = (struct ipran_snmp_sla_rfc2544_table *)malloc(sizeof(struct ipran_snmp_sla_rfc2544_table));
        if (NULL == tmp_data)
        {
            return FALSE;
        }
		
        for (ret = 0; ret < data_num; ret++)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s':  psla_session->info.proto [%d]\n", __FILE__, __LINE__, __func__ , psla_session->info.proto);

            if (psla_session->info.proto != SLA_PROTO_RFC2544)
            {
                psla_session++;
                continue ;
            }

            for (count = 0 ;  count < 7 ; count++)
            {
            	if(psla_session->info.sla_2544_size[count])
            	{
	                memset(tmp_data , 0 , sizeof(struct ipran_snmp_sla_rfc2544_table));
	                memcpy(&tmp_data->old_data , psla_session , sizeof(struct sla_session));
					tmp_data->old_data.result.sla_2544_result.dm_max[0] = psla_session->result.sla_2544_result.dm_max[count];
					tmp_data->old_data.result.sla_2544_result.dm_mean[0] = psla_session->result.sla_2544_result.dm_mean[count];
					tmp_data->old_data.result.sla_2544_result.dm_min[0] = psla_session->result.sla_2544_result.dm_min[count];
					tmp_data->old_data.result.sla_2544_result.lm[0] = psla_session->result.sla_2544_result.lm[count];
					tmp_data->old_data.result.sla_2544_result.throughput[0] = psla_session->result.sla_2544_result.throughput[count];
	                tmp_data->index_size = psla_session->info.sla_2544_size[count];
	                snmp_cache_add(cache, tmp_data, sizeof(struct ipran_snmp_sla_rfc2544_table));
	                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s':  session_id [%d] index_size[%d]\n", __FILE__, __LINE__, __func__ , \
												tmp_data->old_data.info.session_id, tmp_data->index_size);
	                loaded_data++ ;
				}
            }

            psla_session++;
        }
		
		mem_share_free((void *)pMsgRcv, MODULE_ID_SNMPD);

        free(tmp_data);
        if (0 != loaded_data)
        {
            return TRUE;
        }
        else
        {
            return FALSE ;
        }
    }
}

struct sla_session *hhrslaMeasureConfigTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                         int exact,
                                                         const struct sla_session  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;

    struct sla_session  *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->info.session_id)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->info.session_id == index_input->info.session_id)
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

int y1564Table_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                 struct ipran_snmp_sla_session  *index)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
    struct sla_session *psla_session = NULL;
    int data_num = 0;
    int ret = 0;
    int count ;
    int loaded_data = 0 ;
    struct ipran_snmp_sla_session  *tmp_data = NULL;

    pMsgRcv = sla_com_get_session_instance(index->old_data.info.session_id, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index->info.session_id [%d]data_num [%d]\n", __FILE__, __LINE__, __func__ , index->old_data.info.session_id, data_num);

    if (0 == data_num || NULL == pMsgRcv)
    {
        return FALSE;
    }
    else
    {
		psla_session = (struct sla_session *)pMsgRcv->msg_data;
        tmp_data = (struct ipran_snmp_sla_session *)malloc(sizeof(struct ipran_snmp_sla_session));

        if (NULL == tmp_data)
        {
			mem_share_free((void *)pMsgRcv, MODULE_ID_SNMPD);
            return (-1);
        }

        for (ret = 0; ret < data_num; ret++)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s':  psla_session->info.proto [%d]\n", __FILE__, __LINE__, __func__ , psla_session->info.proto);

            if (psla_session->info.proto != SLA_PROTO_Y1564)
            {
                psla_session++;
                continue ;
            }

            count = 0 ;

            for (count = 0 ;  count < 7 ; count++)
            {
            	/*这里鉴于显示的数据类型一致，所以在这里将一个数据块中的不同测试项根据第二索引拆分成一个个cache
				避免在hhry1564MeasureResultTable_get（）函数中做拆分，这里拆分代码量少一些*/
                memset(tmp_data , 0 , sizeof(struct ipran_snmp_sla_session));
                memcpy(&tmp_data->old_data , psla_session , sizeof(struct sla_session));
				if(count < 4)
				{
					tmp_data->old_data.result.sla_1564_result.c_cir_ir[0] = psla_session->result.sla_1564_result.c_cir_ir[count];
					tmp_data->old_data.result.sla_1564_result.c_cir_lm[0] = psla_session->result.sla_1564_result.c_cir_lm[count];
					tmp_data->old_data.result.sla_1564_result.c_cir_dm_max[0] = psla_session->result.sla_1564_result.c_cir_dm_max[count];
					tmp_data->old_data.result.sla_1564_result.c_cir_dm_mean[0] = psla_session->result.sla_1564_result.c_cir_dm_mean[count];
					tmp_data->old_data.result.sla_1564_result.c_cir_dm_min[0] = psla_session->result.sla_1564_result.c_cir_dm_min[count];
					tmp_data->old_data.result.sla_1564_result.c_cir_jm_max[0] = psla_session->result.sla_1564_result.c_cir_jm_max[count];
					tmp_data->old_data.result.sla_1564_result.c_cir_jm_mean[0] = psla_session->result.sla_1564_result.c_cir_jm_mean[count];
					tmp_data->old_data.result.sla_1564_result.c_cir_jm_min[0] = psla_session->result.sla_1564_result.c_cir_jm_min[count];
					tmp_data->y1564_cir = count + 1 ;
                }
				else if(4 == count)
				{
					
					tmp_data->old_data.result.sla_1564_result.c_cir_ir[0] = psla_session->result.sla_1564_result.c_eir_ir;
					tmp_data->old_data.result.sla_1564_result.c_cir_lm[0] = psla_session->result.sla_1564_result.c_eir_lm;
					tmp_data->old_data.result.sla_1564_result.c_cir_dm_max[0] = psla_session->result.sla_1564_result.c_eir_dm_max;
					tmp_data->old_data.result.sla_1564_result.c_cir_dm_mean[0] = psla_session->result.sla_1564_result.c_eir_dm_mean;
					tmp_data->old_data.result.sla_1564_result.c_cir_dm_min[0] = psla_session->result.sla_1564_result.c_eir_dm_min;
					tmp_data->old_data.result.sla_1564_result.c_cir_jm_max[0] = psla_session->result.sla_1564_result.c_eir_jm_max;
					tmp_data->old_data.result.sla_1564_result.c_cir_jm_mean[0] = psla_session->result.sla_1564_result.c_eir_jm_mean;
					tmp_data->old_data.result.sla_1564_result.c_cir_jm_min[0] = psla_session->result.sla_1564_result.c_eir_jm_min;
					tmp_data->y1564_cir = count + 1 ;
				}
				else if(5 == count)
				{
					tmp_data->old_data.result.sla_1564_result.c_cir_ir[0] = psla_session->result.sla_1564_result.c_traf_ir;
					tmp_data->old_data.result.sla_1564_result.c_cir_lm[0] = psla_session->result.sla_1564_result.c_traf_lm;
					tmp_data->old_data.result.sla_1564_result.c_cir_dm_max[0] = psla_session->result.sla_1564_result.c_traf_dm_max;
					tmp_data->old_data.result.sla_1564_result.c_cir_dm_mean[0] = psla_session->result.sla_1564_result.c_traf_dm_mean;
					tmp_data->old_data.result.sla_1564_result.c_cir_dm_min[0] = psla_session->result.sla_1564_result.c_traf_dm_min;
					tmp_data->old_data.result.sla_1564_result.c_cir_jm_max[0] = psla_session->result.sla_1564_result.c_traf_jm_max;
					tmp_data->old_data.result.sla_1564_result.c_cir_jm_mean[0] = psla_session->result.sla_1564_result.c_traf_jm_mean;
					tmp_data->old_data.result.sla_1564_result.c_cir_jm_min[0] = psla_session->result.sla_1564_result.c_traf_jm_min;
					tmp_data->y1564_cir = count + 1 ;
				}
				else if(6 == count)
				{
					tmp_data->old_data.result.sla_1564_result.c_cir_ir[0] = psla_session->result.sla_1564_result.p_ir;
					tmp_data->old_data.result.sla_1564_result.c_cir_lm[0] = psla_session->result.sla_1564_result.p_lm;
					tmp_data->old_data.result.sla_1564_result.c_cir_dm_max[0] = psla_session->result.sla_1564_result.p_dm_max;
					tmp_data->old_data.result.sla_1564_result.c_cir_dm_mean[0] = psla_session->result.sla_1564_result.p_dm_mean;
					tmp_data->old_data.result.sla_1564_result.c_cir_dm_min[0] = psla_session->result.sla_1564_result.p_dm_min;
					tmp_data->old_data.result.sla_1564_result.c_cir_jm_max[0] = psla_session->result.sla_1564_result.p_jm_max;
					tmp_data->old_data.result.sla_1564_result.c_cir_jm_mean[0] = psla_session->result.sla_1564_result.p_jm_mean;
					tmp_data->old_data.result.sla_1564_result.c_cir_jm_min[0] = psla_session->result.sla_1564_result.p_jm_min;
					tmp_data->y1564_cir = count + 1 ;
				}
				//tmp_data->y1564_cir = count + 1 ;
                snmp_cache_add(cache, tmp_data, sizeof(struct ipran_snmp_sla_session));
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s':  session_id [%d] index_cir[%d]\n", __FILE__, __LINE__, __func__ ,\
										tmp_data->old_data.info.session_id, tmp_data->y1564_cir);
                loaded_data++ ;
            }

            psla_session++;
        }

        free(tmp_data);
		mem_share_free((void *)pMsgRcv, MODULE_ID_SNMPD);

        if (0 != loaded_data)
        {
            return TRUE;
        }
        else
        {
            return FALSE ;
        }
    }
}


struct ipran_snmp_sla_session *y1564Table_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                      int exact,
                                                      const struct ipran_snmp_sla_session  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;
    struct ipran_snmp_sla_session   *data1_find;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, data1_find))
    {
        if (NULL == index_input || 0 == index_input->old_data.info.session_id)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->old_data.info.session_id == index_input->old_data.info.session_id &&
                index_input->y1564_cir == data1_find->y1564_cir)
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


u_char *hhry1564MeasureResultTable_get(struct variable *vp,
                                       oid *name,
                                       size_t *length,
                                       int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ipran_snmp_sla_session *psla_session = NULL;
    struct ipran_snmp_sla_session index  ;
    int ret = 0;
    int index_int = 0 ;
    int index_cir = 0 ;
    int index_cir_next = 0 ;

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index_int , &index_cir , exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.old_data.info.session_id = index_int ;
    index.y1564_cir = index_cir ;

    if (NULL == y1564Table_cache)
    {
        y1564Table_cache = snmp_cache_init(sizeof(struct ipran_snmp_sla_session) ,
                                           y1564Table_get_data_from_ipc ,
                                           y1564Table_node_lookup);

        if (NULL == y1564Table_cache)
        {
            return (NULL);
        }
    }

    /* get data by index and renew index_next */
    psla_session = snmp_cache_get_data_by_index(y1564Table_cache , exact, &index);

    if (NULL == psla_session)
    {
        return NULL;
    }

    if (psla_session->old_data.info.proto != SLA_PROTO_Y1564)
    {
        return (NULL);
    }
		
	index_cir_next = psla_session->y1564_cir;
	zlog_debug(SNMP_DBG_MIB_GET, "fun:%s-line:%d --> index_cir_next = %d\n", __FUNCTION__, __LINE__, index_cir_next);
	
    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, psla_session->old_data.info.session_id , psla_session->y1564_cir);
    }

    switch (vp->magic)
    {
        case hhry1564MeasureSessionStatus:
            *var_len = sizeof(int);
            int_value = psla_session->old_data.info.state ;

            if (SLA_STATUS_DISABLE == int_value)
            {
                int_value = 2 ;
            }
            else if (SLA_STATUS_ENABLE == int_value)
            {
                int_value = 1 ;
            }
            else if (SLA_STATUS_UNFINISH == int_value)
            {
                int_value = 3 ;
            }
            else
            {
                int_value = 4 ;
            }

            return (u_char *)(&int_value);

        case hhry1564MeasureFLRMin:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_1564_result.c_cir_lm[0] ;//[index_cir_next - 1]
            return (u_char *)(&uint_value);

        case hhry1564MeasureFLRMax:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_1564_result.c_cir_lm[0] ;
            return (u_char *)(&uint_value);

        case hhry1564MeasureFLRMean:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_1564_result.c_cir_lm[0] ;
            return (u_char *)(&uint_value);

        case hhry1564MeasureFTDMin:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_1564_result.c_cir_dm_min[0] ;
            return (u_char *)(&uint_value);

        case hhry1564MeasureFTDMax:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_1564_result.c_cir_dm_max[0] ;
            return (u_char *)(&uint_value);

        case hhry1564MeasureFTDMean:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_1564_result.c_cir_dm_mean[0] ;
            return (u_char *)(&uint_value);

        case hhry1564MeasureFDVMin:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_1564_result.c_cir_jm_min[0] ;
            return (u_char *)(&uint_value);

        case hhry1564MeasureFDVMax:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_1564_result.c_cir_jm_max[0] ;
            return (u_char *)(&uint_value);

        case hhry1564MeasureFDVMean:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_1564_result.c_cir_jm_mean[0] ;
            return (u_char *)(&uint_value);

        default :
            return (NULL);
    }
}


struct ipran_snmp_sla_rfc2544_table *rfc2544Table_node_lookup(struct ipran_snmp_data_cache *cache ,
													 int exact,
													 const struct ipran_snmp_sla_rfc2544_table  *index_input)
{
	zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index_session[%d] index_size[%d]\n", __FILE__, __LINE__, __func__,\
									index_input->old_data.info.session_id, index_input->index_size);
	struct listnode	*node, *nnode;
	struct ipran_snmp_sla_rfc2544_table   *data1_find;

	for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, data1_find))
	{
	   if (NULL == index_input || 0 == index_input->old_data.info.session_id)
	   {
			zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': index_session[%d] index_size[%d]\n", __FILE__, __LINE__, __func__,\
								index_input->old_data.info.session_id, index_input->index_size);
			return cache->data_list->head->data ;
	   }

	   if (data1_find->old_data.info.session_id == index_input->old_data.info.session_id &&
			   index_input->index_size == data1_find->index_size)
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

u_char *hhrrfc2544MeasureResultTable_get(struct variable *vp,
                                         oid *name,
                                         size_t *length,
                                         int exact, size_t *var_len, WriteMethod **write_method)
{
	struct ipran_snmp_sla_rfc2544_table *psla_session = NULL;
	struct ipran_snmp_sla_rfc2544_table index;
    int ret = 0;
    int index_int = 0;
	int index_size = 0;

    /* validate the index */
	ret = ipran_snmp_intx2_index_get(vp, name, length, &index_int , &index_size , exact);

	memset(&index, 0, sizeof(struct ipran_snmp_sla_rfc2544_table));
    index.old_data.info.session_id = index_int;
	index.index_size = index_size;
	
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index_session [%d] index_size [%d]\n", __FILE__, __LINE__, __func__ , \
			index.old_data.info.session_id, index.index_size);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrrfc2544MeasureResultTable_cache)
    {
        hhrrfc2544MeasureResultTable_cache = snmp_cache_init(sizeof(struct ipran_snmp_sla_rfc2544_table) ,
                                                         hhrrfc2544MeasureResultTable_get_data_from_ipc ,
                                                         rfc2544Table_node_lookup);

        if (NULL == hhrrfc2544MeasureResultTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    psla_session = snmp_cache_get_data_by_index(hhrrfc2544MeasureResultTable_cache , exact, &index);

    if (NULL == psla_session)
    {
        return NULL;
    }

    if (psla_session->old_data.info.proto != SLA_PROTO_RFC2544)
    {
        return (NULL);
    }

    /* get ready the next index */
    if (!exact)
    {
		ipran_snmp_intx2_index_set(vp, name, length, psla_session->old_data.info.session_id , psla_session->index_size);
    }

    switch (vp->magic)
    {
        case hhrrfc2544MeasureSessionStatus:
            *var_len = sizeof(int);
            int_value = psla_session->old_data.info.state ;

            if (SLA_STATUS_DISABLE == int_value)
            {
                int_value = 2 ;
            }
            else if (SLA_STATUS_ENABLE == int_value)
            {
                int_value = 1 ;
            }
            else if (SLA_STATUS_UNFINISH == int_value)
            {
                int_value = 3 ;
            }
            else
            {
                int_value = 4 ;
            }

            return (u_char *)(&int_value);

        case hhrrfc2544MeasureFLRMin:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_2544_result.lm[0] ;
            return (u_char *)(&uint_value);

        case hhrrfc2544MeasureFLRMax:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_2544_result.lm[0] ;
            return (u_char *)(&uint_value);

        case hhrrfc2544MeasureFLRMean:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_2544_result.lm[0] ;
            return (u_char *)(&uint_value);

        case hhrrfc2544MeasureFTDMin:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_2544_result.dm_min[0] ;
            return (u_char *)(&uint_value);

        case hhrrfc2544MeasureFTDMax:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_2544_result.dm_max[0] ;
            return (u_char *)(&uint_value);


        case hhrrfc2544MeasureFTDMean:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_2544_result.dm_mean[0] ;
            return (u_char *)(&uint_value);


        case hhrrfc2544MeasureThroughput:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->old_data.result.sla_2544_result.throughput[0] ;
            return (u_char *)(&uint_value);

        default :
            return (NULL);
    }
}


u_char *hhrslaMeasureConfigTable_get(struct variable *vp,
                                     oid *name,
                                     size_t *length,
                                     int exact, size_t *var_len, WriteMethod **write_method)
{
    struct sla_session *psla_session = NULL;
    struct sla_session index;
    int ret = 0;
    int index_int = 0 ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index_int , exact);

    index.info.session_id = index_int ;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index.info.session_id);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrslaMeasureConfigTable_cache)
    {
        hhrslaMeasureConfigTable_cache = snmp_cache_init(sizeof(struct sla_session) ,
                                                         hhrslaMeasureConfigTable_get_data_from_ipc ,
                                                         hhrslaMeasureConfigTable_node_lookup);

        if (NULL == hhrslaMeasureConfigTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    psla_session = snmp_cache_get_data_by_index(hhrslaMeasureConfigTable_cache , exact, &index);

    if (NULL == psla_session)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, psla_session->info.session_id);
    }

    switch (vp->magic)
    {
        case hhrslaMeasureProtocol:
            *var_len = sizeof(int);
            int_value = (SLA_PROTO_RFC2544 == psla_session->info.proto) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrslaMeasureL2dmac:
            *var_len = 6;
            memcpy(&mac_value, psla_session->pkt_eth.dmac, 6);
            return (mac_value);

        case hhrslaMeasureL2cvlan:
            *var_len = sizeof(int);
            int_value = psla_session->pkt_eth.cvlan;
            return (u_char *)(&int_value);

        case hhrslaMeasureL2svlan:
            *var_len = sizeof(int);
            int_value = psla_session->pkt_eth.svlan;
            return (u_char *)(&int_value);

        case hhrslaMeasureL2cos:
            *var_len = sizeof(int);
            int_value = psla_session->pkt_eth.c_cos;
            return (u_char *)(&int_value);

        case hhrslaMeasureL3sip:
            *var_len = sizeof(uint32_t);
            ip_value = psla_session->pkt_ip.sip;
            ip_value = htonl(ip_value);
            return (u_char *)(&ip_value);

        case hhrslaMeasureL3dip:
            *var_len = sizeof(uint32_t);
            ip_value = psla_session->pkt_ip.dip;
            ip_value = htonl(ip_value);
            return (u_char *)(&ip_value);

        case hhrslaMeasureL3sport:
            *var_len = sizeof(int);
            int_value = psla_session->pkt_ip.sport;
            return (u_char *)(&int_value);

        case hhrslaMeasureL3dport:
            *var_len = sizeof(int);
            int_value = psla_session->pkt_ip.dport;
            return (u_char *)(&int_value);

        case hhrslaMeasureL3ttl:
            *var_len = sizeof(int);
            int_value = psla_session->pkt_ip.ttl;
            return (u_char *)(&int_value);

        case hhrslaMeasureL3dscp:
            *var_len = sizeof(int);
            int_value = psla_session->pkt_ip.dscp;
            return (u_char *)(&int_value);

        case hhrslaMeasurePacketSize:
            *var_len = sizeof(int);
            int_value = psla_session->info.pkt_size;
            return (u_char *)(&int_value);

        case hhrslaMeasurePacketRate:
            *var_len = sizeof(int);
            int_value = psla_session->info.pkt_rate_upper;
            return (u_char *)(&int_value);

        case hhrslaMeasureMeasurePacketLoss:
            *var_len = sizeof(int);
            int_value = (1 == psla_session->info.lm_enable) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrslaMeasureMeasurePacketDelay:
            *var_len = sizeof(int);
            int_value = (1 == psla_session->info.dm_enable) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrslaMeasureMeasureThroughput:
            *var_len = sizeof(int);
            int_value = (SLA_MEASURE_ENABLE == psla_session->info.throughput_enable) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrslaMeasureScheduleInterval:
            *var_len = sizeof(int);
            int_value = psla_session->info.duration;
            return (u_char *)(&int_value);
#if 0

        case hhrslaMeasureScheduleFrequency:
            *var_len = sizeof(int);
            int_value = psla_session->info.frequency;
            return (u_char *)(&int_value);
#endif

        case hhrslaMeasureMeasureIfDescr:
            memset(str_value, 0, SLA_STRING_LEN);
			struct ipc_mesg_n *pMsgRcv = sla_com_get_if_info(MODULE_ID_QOS);
			if(pMsgRcv)
            {
				struct sla_if_entry *if_struct = (struct sla_if_entry *)pMsgRcv->msg_data;

            if (if_struct != NULL && (psla_session->info.session_id >= if_struct->start_id
                                      && psla_session->info.session_id <= if_struct->end_id))
            {
                ifm_get_name_by_ifindex(if_struct->ifindex, str_value);
	            }
				mem_share_free((void *)pMsgRcv, MODULE_ID_SNMPD);
			}

            *var_len = strlen(str_value);
            return (str_value);

        case hhrslaMeasureL2smac:
            *var_len = 6;
            memcpy(&mac_value, psla_session->pkt_eth.smac, 6);
            return (mac_value);

        case hhrslaMeasureSACLos:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->info.sac_loss * SLA_SAC_FLR_BENCHMARK / SLA_PKT_LOSS_ENLAGE;
            return (u_char *)(&uint_value);

        case hhrslaMeasureSACDelay:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->info.sac_delay / SLA_LATENCY_ENLAGE;
            return (u_char *)(&uint_value);

        case hhrslaMeasureSACJitter:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->info.sac_jitter / SLA_LATENCY_ENLAGE;
            return (u_char *)(&uint_value);

        case hhrslaMeasureL2ctpid:
            *var_len = sizeof(int);

            if (0x9100 == psla_session->pkt_eth.c_tpid)
            {
                int_value = 1;
            }
            else if (0x9200 == psla_session->pkt_eth.c_tpid)
            {
                int_value = 2;
            }
            else if (0x88a8 == psla_session->pkt_eth.c_tpid)
            {
                int_value = 3;
            }
            else if (0x8100 == psla_session->pkt_eth.c_tpid)
            {
                int_value = 4;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)(&int_value);

        case hhrslaMeasureL2stpid:
            *var_len = sizeof(int);

            if (0x9100 == psla_session->pkt_eth.s_tpid)
            {
                int_value = 1;
            }
            else if (0x9200 == psla_session->pkt_eth.s_tpid)
            {
                int_value = 2;
            }
            else if (0x88a8 == psla_session->pkt_eth.s_tpid)
            {
                int_value = 3;
            }
            else if (0x8100 == psla_session->pkt_eth.s_tpid)
            {
                int_value = 4;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)(&int_value);

        case hhrslaMeasureL2scos:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->pkt_eth.s_cos;
            return (uchar *)(&uint_value);

        case hhrslaMeasureCir:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->info.cir;
            return (uchar *)(&uint_value);

        case hhrslaMeasureEir:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->info.eir;
            return (uchar *)(&uint_value);

        case hhrslaMeasureRFC2544PacketSize:
            memset(str_value, 0, STRING_LEN);
            sprintf(str_value, "size1 %d size2 %d size3 %d size4 %d size5 %d size6 %d size7 %d", \
                    psla_session->info.sla_2544_size[0], psla_session->info.sla_2544_size[1], \
                    psla_session->info.sla_2544_size[2], psla_session->info.sla_2544_size[3], \
                    psla_session->info.sla_2544_size[4], psla_session->info.sla_2544_size[5], \
                    psla_session->info.sla_2544_size[6]);
            *var_len = strlen(str_value);
            return str_value;

        case hhrslaMeasureY1564PacketSize:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->info.pkt_size;
            return (uchar *)(&uint_value);

        case hhrslaMeasureDuration:
            *var_len = sizeof(uint32_t);
            uint_value = psla_session->info.duration;
            return (uchar *)(&uint_value);

        default :
            return (NULL);
    }
}

