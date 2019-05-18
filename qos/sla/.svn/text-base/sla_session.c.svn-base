/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*/

#include <lib/errcode.h>
#include <lib/memory.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include <lib/ifm_common.h>
#include <lib/devm_com.h>
#include <qos/qos_main.h>
#include "lib/log.h"

#include "sla_session.h"

struct hash_table sla_session_table; /* sla 会话 hash 表，用 session_id 作为 key */
struct sla_if_entry sla_entry;/* 接口表*/
struct sla_loopback if_loopback;

/* sla 会话表计算hash*/
static uint16_t sla_session_compute_hash(void *hash_key)
{
	if(NULL == hash_key)
	{
		return 0;
	}

	return ((uint16_t)hash_key);
}

/* sla 会话表比较hash*/
static int sla_session_compare_hash(void *item, void *hash_key)
{
	struct hash_bucket *pbucket = item;

	if (NULL == item || NULL == hash_key)
	{
		return ERRNO_FAIL;
	}

	if (pbucket->hash_key == hash_key)
	{
		return ERRNO_SUCCESS;
	}
	
	return ERRNO_FAIL;
}

/* sla 会话表初始化 */
void sla_session_table_init (unsigned int size)
{
    memset(&sla_entry, 0, sizeof(struct sla_if_entry));
	memset(&if_loopback, 0, sizeof(struct sla_loopback));
	hios_hash_init(&sla_session_table, size, sla_session_compute_hash, sla_session_compare_hash);
}

/*sla 回话信息下发*/
void sla_session_send_down(uint16_t start_id, uint16_t end_id, enum IPC_OPCODE opcode, enum SLA_STATUS status)
{
	struct sla_session *p_session = NULL;
	//uint16_t data_size = 0;
	sint32 ret = ERRNO_SUCCESS;
	uint32_t i = 0, j = 0;

	for (i=start_id; i<((uint32_t)end_id + 1); i++)
	{
		p_session = sla_session_lookup(i);
		if(p_session == NULL)
		{
			return ;
		}
		if(p_session->info.proto == SLA_PROTO_RFC2544)
		{
			for(j = 0; j < 7; j++)
			{
				if(p_session->info.sla_2544_size[j] != 0)
				{
					p_session->info.pkt_size = p_session->info.sla_2544_size[j];
					break;
				}
			}
		}
		/*更新测试状态*/
		p_session->info.state = status;
		
		/*清空测试结果*/
		memset(&p_session->result, 0, sizeof(struct sla_measure_t));
		
		/*传入硬件索引*/
		ret = ipc_send_msg_n2(p_session, sizeof(struct sla_session), 1, MODULE_ID_HAL, MODULE_ID_QOS,
					IPC_TYPE_SLA, SLA_SUBTYPE_SESS, opcode, i - start_id);
		if (ret != ERRNO_SUCCESS)
		{
			zlog_err("%s[%d]:%s: error:session id %d\n",__FILE__,__LINE__,__func__, p_session->info.session_id);
			return ;
		}
	}
	
	return ;
}

/* 创建一个会话 */
struct sla_session *sla_session_create(uint16_t sess_id)
{
	struct sla_session *p_session = NULL;
	sint32 ret = ERRNO_SUCCESS;
	
	p_session = (struct sla_session *)XCALLOC(MTYPE_SLA_SESSION, sizeof(struct sla_session));
	if (NULL == p_session)
	{
		zlog_err("%s[%d]:%s: error:fail XCALLOC struct sla_session\n",__FILE__,__LINE__,__func__);
		return NULL;
	}
	
	memset(p_session, 0, sizeof(struct sla_session));
	p_session->info.session_id = sess_id;
	p_session->pkt_ip.dscp = SLA_DEF_DSCP;

	#if 0
	p_session->info.proto = SLA_PROTO_RFC2544;/*默认rfc2544*/
	p_session->info.state = SLA_STATUS_DISABLE;
	
	p_session->info.pkt_size = 64;/*默认测试报文长度*/
	p_session->info.pkt_rate_upper = SLA_DEF_PKT_RATE;/*速率默认1000*/
	p_session->info.pkt_rate_lower = 0;/*速率默认1000*/
	p_session->info.interval = SLA_DEF_INTERVAL;/*默认测试间隔*/
	p_session->info.frequency = SLA_DEF_FREQUENCY;/*默认测试次数*/
	p_session->pkt_eth.c_tpid = 0x8100;
	p_session->pkt_eth.s_tpid = 0x8100;
	p_session->pkt_eth.c_cos = SLA_DEF_COS;
	p_session->pkt_eth.s_cos = SLA_DEF_COS;
	p_session->pkt_ip.dscp = SLA_DEF_DSCP;
	p_session->info.sac_loss = SLA_SAC_DEF_FLR * SLA_PKT_LOSS_ENLAGE/SLA_SAC_FLR_BENCHMARK;
	p_session->info.sac_delay = SLA_SAC_DEF_FTD * SLA_LATENCY_ENLAGE;
	p_session->info.sac_jitter = SLA_SAC_DEF_FDV * SLA_LATENCY_ENLAGE;
	p_session->pkt_ip.dport = UDP_PORT_ECHO;
	p_session->pkt_ip.sport = UDP_PORT_RFC2544;
	p_session->pkt_ip.ttl = SLA_DEF_IP_TTL;
	p_session->info.sla_2544_size[0] = 64;
	p_session->info.sla_2544_size[1] = 128;
	p_session->info.sla_2544_size[2] = 256;
	p_session->info.sla_2544_size[3] = 512;
	p_session->info.sla_2544_size[4] = 1024;
	p_session->info.sla_2544_size[5] = 1280;
	p_session->info.sla_2544_size[6] = 1518;
	#endif
	
	ret = devm_comm_get_mac ( 1, 0, MODULE_ID_QOS, p_session->pkt_eth.smac);
	if (ret)
	{
		zlog_err ( "%s[%d]:leave %s:ifm get mac from devm error,mac == NULL\n", __FILE__, __LINE__, __func__ );
	}
	
	return p_session;
}

/* 删除一个会话 */
int sla_session_delete(uint16_t sess_id)
{
	struct sla_session *p_session = NULL;
	struct hash_bucket *p_item = NULL;
	
	p_item = hios_hash_find(&sla_session_table, (void *)((uint32_t)sess_id));
	if (NULL == p_item)
	{
		zlog_err("%s[%d]:%s: error:fail find struct hash_bucket session id %d\n",__FILE__,__LINE__,__func__,sess_id);
		return ERRNO_FAIL;
	}
	
 	p_session = (struct sla_session *)p_item->data;
	hios_hash_delete(&sla_session_table, p_item);
	XFREE(MTYPE_SLA_SESSION, p_session);
    p_item->prev = NULL;
    p_item->next = NULL;
	XFREE(MTYPE_HASH_BACKET, p_item);
	
	return ERRNO_SUCCESS;
}

/* 添加到 hash 表 */
int sla_session_add(struct sla_session *psess)
{
	struct hash_bucket *p_item = NULL;
	sint32 ret = ERRNO_SUCCESS;

    if(sla_session_table.num_entries == sla_session_table.hash_size)
    {
        return ERRNO_OVERSIZE;
    }
    
	p_item = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if (NULL == p_item)
	{
		XFREE(MTYPE_SLA_SESSION, psess);
		zlog_err("%s[%d]:%s: error:fail XCALLOC struct hash_bucket\n",__FILE__,__LINE__,__func__);
		return ERRNO_MALLOC;
	}

	p_item->hash_key = (void *)((sint32)(psess->info.session_id));
	p_item->data = psess;
	ret = hios_hash_add(&sla_session_table, p_item);
	if (ret != ERRNO_SUCCESS)
	{
		zlog_err("%s[%d]:%s: error:fail add struct hash_bucket\n",__FILE__,__LINE__,__func__);
		XFREE(MTYPE_SLA_SESSION, psess);
		XFREE(MTYPE_HASH_BACKET, p_item);
		return ret;
	}
	
	return ret;
}

/* 查找一个会话 */
struct sla_session *sla_session_lookup(uint16_t sess_id)
{
	struct hash_bucket *pitem = NULL;

	pitem = hios_hash_find(&sla_session_table, (void *)((uint32_t)sess_id));
	if (NULL == pitem)
	{
		return NULL;
	}

	return (struct sla_session *)pitem->data;
}

/*HT157 vop enable 与 evc建立*/
void sla_session_send_event(uint32_t ifindex)
{
	sint32 ret = ERRNO_SUCCESS;
	
	ret = ipc_send_msg_n2(&ifindex, sizeof(uint32_t), 1, MODULE_ID_HAL, MODULE_ID_QOS,
				IPC_TYPE_SLA, SLA_SUBTYPE_SESS, IPC_OPCODE_EVENT, 0);
	if (ret != ERRNO_SUCCESS)
	{
		zlog_err("%s[%d]:%s: error: sla session enable send hal error!\n", __FILE__, __LINE__, __FUNCTION__);
	}
	
	return ;
}

/* 会话 enable */
void sla_session_enable(struct sla_if_entry *pentry)
{
	sint32 ret = ERRNO_SUCCESS;
	ret = ipc_send_msg_n2(pentry, sizeof(struct sla_if_entry), 1, MODULE_ID_HAL, MODULE_ID_QOS,
				IPC_TYPE_SLA, SLA_SUBTYPE_SESS, IPC_OPCODE_ENABLE, 0);
	if (ret != ERRNO_SUCCESS)
	{
		zlog_err("%s[%d]:%s: error: sla session enable send hal error!\n", __FILE__, __LINE__, __FUNCTION__);
	}
	
	return ;
}

/* 会话 disable */
void sla_session_disable(struct sla_if_entry *pentry)
{
	sint32 ret = ERRNO_SUCCESS;
    ret = ipc_send_msg_n2(pentry, sizeof(struct sla_if_entry), 1, MODULE_ID_HAL, 
    			MODULE_ID_QOS, IPC_TYPE_SLA, SLA_SUBTYPE_SESS, IPC_OPCODE_DISABLE, 0);
   	if (ret != ERRNO_SUCCESS)
	{
		zlog_err("%s[%d]:%s: error: sla session disable error!\n", __FILE__, __LINE__, __FUNCTION__);
	}
	
	return ;
}



/* 测量结束，芯片上报测试结果 */
void sla_session_finish(struct sla_session *psess, struct sla_measure_t* presult)
{
	uint16_t data_size = 0;
	int num = 0;
	int ret = 0;
	int i = 0, j = 0;
	uint8_t count = 0;
	uint32_t temp = 0;

	/*关于1564，每个小阶段拥有自己独立的状态即finish状态 */
	if(psess->info.proto == SLA_PROTO_Y1564)
	{
		psess->result.sla_1564_result.result_counter++;
		count = psess->result.sla_1564_result.result_counter;
	    /*清除 上次测试结果*/
		memset ( &psess->result, 0, sizeof ( struct sla_measure_t ) );
		memcpy(&psess->result.sla_1564_result, &presult->sla_1564_result, sizeof ( struct sla_1564_measure ));
		psess->result.sla_1564_result.result_counter = count;
		zlog_debug(QOS_DBG_SLA, "%s[%d]############### result_counter = %d\n",__FUNCTION__,__LINE__,\
					psess->result.sla_1564_result.result_counter);

		if(1 == psess->result.sla_1564_result.result_counter)
		{
			
			zlog_debug(QOS_DBG_SLA,  "%s[%d]: %s:session id: %d\n", __FILE__, __LINE__, __func__, \
														psess->info.session_id);
			for(i = 0; i < 4; i++)
			{
				zlog_debug(QOS_DBG_SLA,  "cir-IR %u, cir-lm %u\n cir-dm_max %u, cir-dm_mean %u, cir-dm_min %u\n"\
							"cir-jm_max %u, cir-jm_mean %u, cir-jm_min %u\n",
										presult->sla_1564_result.c_cir_ir[i], \
										presult->sla_1564_result.c_cir_lm[i], \
										presult->sla_1564_result.c_cir_dm_max[i],\
										presult->sla_1564_result.c_cir_dm_mean[i],\
										presult->sla_1564_result.c_cir_dm_min[i],\
										presult->sla_1564_result.c_cir_jm_max[i],\
										presult->sla_1564_result.c_cir_jm_mean[i],\
										presult->sla_1564_result.c_cir_jm_min[i]);
				zlog_debug(QOS_DBG_SLA, "*************************************************\n");
			}

			zlog_debug(QOS_DBG_SLA,  "eir-IR %u, eir-lm %u\n eir-dm_max %u, eir-dm_mean %u, eir-dm_min %u\n"\
							"eir-jm_max %u, eir-jm_mean %u, eir-jm_min %u\n",
										presult->sla_1564_result.c_eir_ir, \
										presult->sla_1564_result.c_eir_lm, \
										presult->sla_1564_result.c_eir_dm_max,\
										presult->sla_1564_result.c_eir_dm_mean,\
										presult->sla_1564_result.c_eir_dm_min,\
										presult->sla_1564_result.c_eir_jm_max,\
										presult->sla_1564_result.c_eir_jm_mean,\
										presult->sla_1564_result.c_eir_jm_min);
			zlog_debug(QOS_DBG_SLA, "*************************************************\n");
			zlog_debug(QOS_DBG_SLA,  "traf-IR %u, traf-lm %u\n traf-dm_max %u, traf-dm_mean %u, traf-dm_min %u\n"\
							"traf-jm_max %u, traf-jm_mean %u, traf-jm_min %u\n",
										presult->sla_1564_result.c_traf_ir, \
										presult->sla_1564_result.c_traf_lm, \
										presult->sla_1564_result.c_traf_dm_max,\
										presult->sla_1564_result.c_traf_dm_mean,\
										presult->sla_1564_result.c_traf_dm_min,\
										presult->sla_1564_result.c_traf_jm_max,\
										presult->sla_1564_result.c_traf_jm_mean,\
										presult->sla_1564_result.c_traf_jm_min);
			zlog_debug(QOS_DBG_SLA, "*************************************************\n");
			zlog_debug(QOS_DBG_SLA,  "p-IR %u, p-lm %u\n p-dm_max %u, p-dm_mean %u, p-dm_min %u\n"\
							"p-jm_max %u, p-jm_mean %u, p-jm_min %u\n",
										presult->sla_1564_result.p_ir, \
										presult->sla_1564_result.p_lm, \
										presult->sla_1564_result.p_dm_max,\
										presult->sla_1564_result.p_dm_mean,\
										presult->sla_1564_result.p_dm_min,\
										presult->sla_1564_result.p_jm_max,\
										presult->sla_1564_result.p_jm_mean,\
										presult->sla_1564_result.p_jm_min);
		}

		for(i = 0; i < 4; i++)
		{
			//cir_dm
			if(psess->result.sla_1564_result.c_cir_dm_max[i] < psess->result.sla_1564_result.c_cir_dm_mean[i])
			{
				temp = psess->result.sla_1564_result.c_cir_dm_max[i];
				psess->result.sla_1564_result.c_cir_dm_max[i] = psess->result.sla_1564_result.c_cir_dm_mean[i];
				psess->result.sla_1564_result.c_cir_dm_mean[i] = temp;	
			}

			if(psess->result.sla_1564_result.c_cir_dm_max[i] < psess->result.sla_1564_result.c_cir_dm_min[i])
			{ 
				temp = psess->result.sla_1564_result.c_cir_dm_max[i];
				psess->result.sla_1564_result.c_cir_dm_max[i] = psess->result.sla_1564_result.c_cir_dm_min[i];
				psess->result.sla_1564_result.c_cir_dm_min[i] = temp;	
			}

			if(psess->result.sla_1564_result.c_cir_dm_mean[i] < psess->result.sla_1564_result.c_cir_dm_min[i])
			{ 
				temp = psess->result.sla_1564_result.c_cir_dm_mean[i];
				psess->result.sla_1564_result.c_cir_dm_mean[i] = psess->result.sla_1564_result.c_cir_dm_min[i];
				psess->result.sla_1564_result.c_cir_dm_min[i] = temp;	
			}

			//cir_jm
			if(psess->result.sla_1564_result.c_cir_jm_max[i] < psess->result.sla_1564_result.c_cir_jm_mean[i])
			{
				temp = psess->result.sla_1564_result.c_cir_jm_max[i];
				psess->result.sla_1564_result.c_cir_jm_max[i] = psess->result.sla_1564_result.c_cir_jm_mean[i];
				psess->result.sla_1564_result.c_cir_jm_mean[i] = temp;	
			}

			if(psess->result.sla_1564_result.c_cir_jm_max[i] < psess->result.sla_1564_result.c_cir_jm_min[i])
			{ 
				temp = psess->result.sla_1564_result.c_cir_jm_max[i];
				psess->result.sla_1564_result.c_cir_jm_max[i] = psess->result.sla_1564_result.c_cir_jm_min[i];
				psess->result.sla_1564_result.c_cir_jm_min[i] = temp;	
			}

			if(psess->result.sla_1564_result.c_cir_jm_mean[i] < psess->result.sla_1564_result.c_cir_jm_min[i])
			{ 
				temp = psess->result.sla_1564_result.c_cir_jm_mean[i];
				psess->result.sla_1564_result.c_cir_jm_mean[i] = psess->result.sla_1564_result.c_cir_jm_min[i];
				psess->result.sla_1564_result.c_cir_jm_min[i] = temp;	
			}
		}

		//eir_jm
		if(psess->result.sla_1564_result.c_eir_jm_max < psess->result.sla_1564_result.c_eir_jm_mean)
		{
			temp = psess->result.sla_1564_result.c_eir_jm_max;
			psess->result.sla_1564_result.c_eir_jm_max = psess->result.sla_1564_result.c_eir_jm_mean;
			psess->result.sla_1564_result.c_eir_jm_mean = temp;	
		}

		if(psess->result.sla_1564_result.c_eir_jm_max < psess->result.sla_1564_result.c_eir_jm_min)
		{ 
			temp = psess->result.sla_1564_result.c_eir_jm_max;
			psess->result.sla_1564_result.c_eir_jm_max = psess->result.sla_1564_result.c_eir_jm_min;
			psess->result.sla_1564_result.c_eir_jm_min = temp;	
		}

		if(psess->result.sla_1564_result.c_eir_jm_mean < psess->result.sla_1564_result.c_eir_jm_min)
		{ 
			temp = psess->result.sla_1564_result.c_eir_jm_mean;
			psess->result.sla_1564_result.c_eir_jm_mean = psess->result.sla_1564_result.c_eir_jm_min;
			psess->result.sla_1564_result.c_eir_jm_min = temp;	
		}
		//eir_dm
		if(psess->result.sla_1564_result.c_eir_dm_max < psess->result.sla_1564_result.c_eir_dm_mean)
		{
			temp = psess->result.sla_1564_result.c_eir_dm_max;
			psess->result.sla_1564_result.c_eir_dm_max = psess->result.sla_1564_result.c_eir_dm_mean;
			psess->result.sla_1564_result.c_eir_dm_mean = temp;	
		}

		if(psess->result.sla_1564_result.c_eir_dm_max < psess->result.sla_1564_result.c_eir_dm_min)
		{ 
			temp = psess->result.sla_1564_result.c_eir_dm_max;
			psess->result.sla_1564_result.c_eir_dm_max = psess->result.sla_1564_result.c_eir_dm_min;
			psess->result.sla_1564_result.c_eir_dm_min = temp;	
		}

		if(psess->result.sla_1564_result.c_eir_dm_mean < psess->result.sla_1564_result.c_eir_dm_min)
		{ 
			temp = psess->result.sla_1564_result.c_eir_dm_mean;
			psess->result.sla_1564_result.c_eir_dm_mean = psess->result.sla_1564_result.c_eir_dm_min;
			psess->result.sla_1564_result.c_eir_dm_min = temp;	
		}
		
		//traf_jm
		if(psess->result.sla_1564_result.c_traf_jm_max < psess->result.sla_1564_result.c_traf_jm_mean)
		{
			temp = psess->result.sla_1564_result.c_traf_jm_max;
			psess->result.sla_1564_result.c_traf_jm_max = psess->result.sla_1564_result.c_traf_jm_mean;
			psess->result.sla_1564_result.c_traf_jm_mean = temp;	
		}

		if(psess->result.sla_1564_result.c_traf_jm_max < psess->result.sla_1564_result.c_traf_jm_min)
		{ 
			temp = psess->result.sla_1564_result.c_traf_jm_max;
			psess->result.sla_1564_result.c_traf_jm_max = psess->result.sla_1564_result.c_traf_jm_min;
			psess->result.sla_1564_result.c_traf_jm_min = temp;	
		}

		if(psess->result.sla_1564_result.c_traf_jm_mean < psess->result.sla_1564_result.c_traf_jm_min)
		{ 
			temp = psess->result.sla_1564_result.c_traf_jm_mean;
			psess->result.sla_1564_result.c_traf_jm_mean = psess->result.sla_1564_result.c_traf_jm_min;
			psess->result.sla_1564_result.c_traf_jm_min = temp;	
		}
		//traf_dm
		if(psess->result.sla_1564_result.c_traf_dm_max < psess->result.sla_1564_result.c_traf_dm_mean)
		{
			temp = psess->result.sla_1564_result.c_traf_dm_max;
			psess->result.sla_1564_result.c_traf_dm_max = psess->result.sla_1564_result.c_traf_dm_mean;
			psess->result.sla_1564_result.c_traf_dm_mean = temp;	
		}

		if(psess->result.sla_1564_result.c_traf_dm_max < psess->result.sla_1564_result.c_traf_dm_min)
		{ 
			temp = psess->result.sla_1564_result.c_traf_dm_max;
			psess->result.sla_1564_result.c_traf_dm_max = psess->result.sla_1564_result.c_traf_dm_min;
			psess->result.sla_1564_result.c_traf_dm_min = temp;	
		}

		if(psess->result.sla_1564_result.c_traf_dm_mean < psess->result.sla_1564_result.c_traf_dm_min)
		{ 
			temp = psess->result.sla_1564_result.c_traf_dm_mean;
			psess->result.sla_1564_result.c_traf_dm_mean = psess->result.sla_1564_result.c_traf_dm_min;
			psess->result.sla_1564_result.c_traf_dm_min = temp;	
		}

		//p_jm
		if(psess->result.sla_1564_result.p_jm_max < psess->result.sla_1564_result.p_jm_mean)
		{
			temp = psess->result.sla_1564_result.p_jm_max;
			psess->result.sla_1564_result.p_jm_max = psess->result.sla_1564_result.p_jm_mean;
			psess->result.sla_1564_result.p_jm_mean = temp;	
		}

		if(psess->result.sla_1564_result.p_jm_max < psess->result.sla_1564_result.p_jm_min)
		{ 
			temp = psess->result.sla_1564_result.p_jm_max;
			psess->result.sla_1564_result.p_jm_max = psess->result.sla_1564_result.p_jm_min;
			psess->result.sla_1564_result.p_jm_min = temp;	
		}

		if(psess->result.sla_1564_result.p_jm_mean < psess->result.sla_1564_result.p_jm_min)
		{ 
			temp = psess->result.sla_1564_result.p_jm_mean;
			psess->result.sla_1564_result.p_jm_mean = psess->result.sla_1564_result.p_jm_min;
			psess->result.sla_1564_result.p_jm_min = temp;	
		}
		//p_dm
		if(psess->result.sla_1564_result.p_dm_max < psess->result.sla_1564_result.p_dm_mean)
		{
			temp = psess->result.sla_1564_result.p_dm_max;
			psess->result.sla_1564_result.p_dm_max = psess->result.sla_1564_result.p_dm_mean;
			psess->result.sla_1564_result.p_dm_mean = temp;	
		}

		if(psess->result.sla_1564_result.p_dm_max < psess->result.sla_1564_result.p_dm_min)
		{ 
			temp = psess->result.sla_1564_result.p_dm_max;
			psess->result.sla_1564_result.p_dm_max = psess->result.sla_1564_result.p_dm_min;
			psess->result.sla_1564_result.p_dm_min = temp;	
		}

		if(psess->result.sla_1564_result.p_dm_mean < psess->result.sla_1564_result.p_dm_min)
		{ 
			temp = psess->result.sla_1564_result.p_dm_mean;
			psess->result.sla_1564_result.p_dm_mean = psess->result.sla_1564_result.p_dm_min;
			psess->result.sla_1564_result.p_dm_min = temp;	
		}
		
		/*设置测试状态*/
		if(((SLA_Y1564_CONFIGURATION == psess->info.sla_y1564_stage) && (6 == psess->result.sla_1564_result.result_counter))
			|| ((SLA_Y1564_PERFORMANCE == psess->info.sla_y1564_stage) && (1 == psess->result.sla_1564_result.result_counter)))
		{
			psess->info.state = SLA_STATUS_FINISH;
			sla_entry.state = SLA_STATUS_FINISH;
			
			/*复位测试状态判断条件*/
			psess->result.sla_1564_result.result_counter = 0;
		}
	}
	else if(psess->info.proto == SLA_PROTO_RFC2544)
	{
		for(i = 0; i < 7; i++)
		{
			data_size = 0;
			if(psess->info.pkt_size == psess->info.sla_2544_size[i])
			{
				num = i;
				for(j = i + 1; j < 7;j++)
				{
					if(psess->info.sla_2544_size[j] != 0)
					{
						data_size = psess->info.sla_2544_size[j];
						break;
					}
				}
			}
			if(data_size != 0)
			{
				break;
			}
            
		}
		
		zlog_debug(QOS_DBG_SLA,  "%s[%d]: %s:session id: %d,lm_max %u, lm_mean %u,lm_min %u\n", __FILE__, __LINE__, __func__, \
			psess->info.session_id, presult->sla_2544_result.lm[0], presult->sla_2544_result.lm[0], presult->sla_2544_result.lm[0]);

		zlog_debug(QOS_DBG_SLA,  "%s[%d]: %s:session id: %d, dm_max %u, dm_mean %u,dm_min %u\n", __FILE__, __LINE__, __func__, \
			psess->info.session_id, presult->sla_2544_result.dm_max[0], presult->sla_2544_result.dm_mean[0], presult->sla_2544_result.dm_min[0]);

		zlog_debug(QOS_DBG_SLA,  "%s[%d]: %s:session id: %d, pkt_tx:%lld, pkt_tx:%lld, throughput %u\n", __FILE__, __LINE__, __func__, \
			psess->info.session_id, presult->sla_2544_result.pkt_tx[0], presult->sla_2544_result.pkt_rx[0], presult->sla_2544_result.throughput[0]);
        sla_result_debug(num, psess->info.pkt_size, presult);
		
		psess->result.sla_2544_result.throughput[num] = presult->sla_2544_result.throughput[0];
		psess->result.sla_2544_result.pkt_rx[num]     = presult->sla_2544_result.pkt_rx[0];
		psess->result.sla_2544_result.pkt_tx[num]     = presult->sla_2544_result.pkt_tx[0];
		
		psess->result.sla_2544_result.lm[num]     = presult->sla_2544_result.lm[0];
		psess->result.sla_2544_result.lm[num]    = presult->sla_2544_result.lm[0];
		psess->result.sla_2544_result.lm[num]     = presult->sla_2544_result.lm[0];

		if(presult->sla_2544_result.dm_max[0] < presult->sla_2544_result.dm_mean[0])
		{
			temp = presult->sla_2544_result.dm_max[0];
			presult->sla_2544_result.dm_max[0] = presult->sla_2544_result.dm_mean[0];
			presult->sla_2544_result.dm_mean[0] = temp;	
		}

		if(presult->sla_2544_result.dm_max[0] < presult->sla_2544_result.dm_min[0])
		{ 
			temp = presult->sla_2544_result.dm_max[0];
			presult->sla_2544_result.dm_max[0] = presult->sla_2544_result.dm_min[0];
			presult->sla_2544_result.dm_min[0] = temp;	
		}

		if(presult->sla_2544_result.dm_mean[0] < presult->sla_2544_result.dm_min[0])
		{ 
			temp = presult->sla_2544_result.dm_mean[0];
			presult->sla_2544_result.dm_mean[0] = presult->sla_2544_result.dm_min[0];
			presult->sla_2544_result.dm_min[0] = temp;	
		}
		
		psess->result.sla_2544_result.dm_max[num]     = presult->sla_2544_result.dm_max[0];
		psess->result.sla_2544_result.dm_mean[num]    = (presult->sla_2544_result.dm_max[0]\
														+ presult->sla_2544_result.dm_mean[0]\
														+ presult->sla_2544_result.dm_min[0])/3;
		psess->result.sla_2544_result.dm_min[num]     = presult->sla_2544_result.dm_min[0];

		

		if(psess->info.state == SLA_STATUS_ENABLE)
		{
			if(0 == data_size)
			{
				psess->info.state = SLA_STATUS_FINISH;
				sla_entry.state   = SLA_STATUS_FINISH;
			}
			else
			{
				psess->info.pkt_size = data_size;
				ret = ipc_send_msg_n2(psess, sizeof(struct sla_session), 1, MODULE_ID_HAL, MODULE_ID_QOS, IPC_TYPE_SLA, SLA_SUBTYPE_SESS, IPC_OPCODE_UPDATE, 0);
				if (ret != ERRNO_SUCCESS){
					zlog_err("%s[%s]--line:%d ipc send fail:session_id = %d.\n", __FILE__, __FUNCTION__, __LINE__,  psess->info.session_id);
					return ;
				}
                
				//sla_session_enable(&sla_entry);
			} 
		}
	}
	
	return ;
}

int sla_result_debug(int num, int pkt_size, struct sla_measure_t *sla_result)
{
	if(sla_result == NULL)
	{
		return 1;
	}

	QOS_LOG_SLA(" num = %d, pkt_size = %d\n", num, pkt_size);
	
	QOS_LOG_SLA("%4s%-25s: %s\n", "", "Throughput(Mbps)",
					sla_display_format(sla_result->sla_2544_result.throughput[0], 3, 2));
			
	QOS_LOG_SLA("%4s%-25s: %lld/%lld\n", "", "TxCount/RxCount",
		sla_result->sla_2544_result.pkt_tx[0], sla_result->sla_2544_result.pkt_rx[0]);


	QOS_LOG_SLA("%4s%-25s: %s%%/%s%%/%s%%\n", "", "Max/min/mean Loss Ratio",
					sla_display_format(sla_result->sla_2544_result.lm[0], 2, 2));
	zlog_debug(QOS_DBG_SLA, "%s%%/", sla_display_format(sla_result->sla_2544_result.lm[0], 2, 2));
	zlog_debug(QOS_DBG_SLA, "%s%%\n", sla_display_format(sla_result->sla_2544_result.lm[0], 2, 2));


	QOS_LOG_SLA("%4s%-25s: %s/%s/%s\n", "", "Max/min/mean Delay(us)",
					sla_display_format(sla_result->sla_2544_result.dm_max[0], 3, 3));
	zlog_debug(QOS_DBG_SLA, "%s/", sla_display_format(sla_result->sla_2544_result.dm_min[0], 3, 3));
	zlog_debug(QOS_DBG_SLA, "%s\n", sla_display_format(sla_result->sla_2544_result.dm_mean[0], 3, 3));

	return 0;
}

