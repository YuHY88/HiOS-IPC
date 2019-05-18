
/*
 *  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
 *
 *  liufy@huahuan.com 
 *
 *  file name: mstp_api.h
 *
 *  date: 2017.3
 *
 *  modify:
 *
 */
/*write by youcheng 2018.10.15*/

#include "lib/msg_ipc_n.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lib/log.h"
#include "lib/vty.h"
#include "l2/l2_if.h"
#include "lib/hash1.h"
#include "lib/errcode.h"

#include "mstp.h"
#include "mstp_show.h"
#include "mstp_port.h"
#include "mstp_bpdu.h"
#include "mstp_base_procedure.h"
#include "mstp_api.h"


extern struct mstp_bridge	*mstp_global_bridge;
extern struct mstp_vlan			bridge_cist_vlan_map;


void snmp_msg_rcv_mstp(struct ipc_msghdr_n * phdr)
{
	uint8_t ret = 0;
	uint16_t msg_num = 0;

	switch(phdr->data_num)
	{
		case MSTP_GLOBAL_CONFIG_TYPE:
			{	
						struct mstp_global_config_snmp mstp_info;
						memset(&mstp_info,0,sizeof(mstp_info));
						ret = mstp_global_config_get(&mstp_info);
						mstp_reply_snmp_request(&mstp_info,sizeof(mstp_info),ret,phdr);
			}
			break;

		case MSTP_PORT_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct mstp_port_info_snmp);
				struct mstp_port_info_snmp mstp_snmp[msg_num];
				memset(mstp_snmp, 0, sizeof(mstp_snmp));			
				ret = mstp_snmp_port_info_get(phdr->msg_index, mstp_snmp);
				mstp_reply_snmp_request(mstp_snmp,ret * sizeof(struct mstp_port_info_snmp),ret,phdr);
			}
			break;

		case MSTP_INSTANCE_TYPE:
			{
				if(phdr->msg_index > MSTP_INSTANCE_MAX)
				{
					mstp_reply_snmp_request(NULL,0,ret,phdr);

				}

				else
				{		
					msg_num = IPC_MSG_LEN/sizeof(struct mstp_instance_snmp);
					struct mstp_instance_snmp mstp_snmp[msg_num];
					memset(mstp_snmp, 0, sizeof(mstp_snmp));			
					ret = mstp_snmp_instance_get(phdr->msg_index, mstp_snmp);
					mstp_reply_snmp_request(mstp_snmp,ret * sizeof(struct mstp_instance_snmp),ret,phdr);
				}
			}
			break;

		case MSTP_PORT_STATISTICS_TYPE:
			{	
				msg_num = IPC_MSG_LEN/sizeof(struct mstp_port_statistics_snmp);
				struct mstp_port_statistics_snmp mstp_snmp[msg_num];
				memset(mstp_snmp, 0, sizeof(mstp_snmp));			
				ret = mstp_snmp_port_statistics_get(phdr->msg_index, mstp_snmp);
				mstp_reply_snmp_request(mstp_snmp,ret * sizeof(struct mstp_port_statistics_snmp),ret,phdr);

			}
		default:
			break;
	}

}
void snmp_msg_rcv_instance_port_info(struct ipc_msghdr_n *phdr,uint32_t instance_port_index)
{

	int ret = 0;
	int msg_num = 0;
	uint32_t instance_id = 0;

	instance_id = phdr->msg_index;
	if(MSTP_INSTANCE_PORT_TYPE == phdr->data_num)
	{      
		msg_num = IPC_MSG_LEN/sizeof(struct mstp_instance_port_snmp);
		struct mstp_instance_port_snmp mstp_snmp[msg_num];
		memset(mstp_snmp,0,sizeof(mstp_snmp));
		ret = mstp_snmp_instance_port_get(instance_id,instance_port_index,mstp_snmp);
		mstp_reply_snmp_request(mstp_snmp,ret * sizeof(struct mstp_instance_port_snmp),ret,phdr);
	}

}

void mstp_reply_snmp_request(void * pdata, int data_len, int data_num,struct ipc_msghdr_n * phdr)
{
	int8_t ret = 0;

	MSTP_LOG_DBG("[%s][%d]data_num = %d data_len = %d\n",__FUNCTION__,__LINE__,data_num,data_len);

	if (data_num > 0)
	{
	#if 0
		ret = ipc_send_reply_bulk(pdata, data_len, data_num, phdr->sender_id,
				MODULE_ID_L2, IPC_TYPE_SNMP, phdr->msg_subtype, phdr->msg_index);
	#endif
	
		ret = ipc_send_reply_n2(pdata, data_len, data_num, phdr->sender_id,
	    		MODULE_ID_L2, IPC_TYPE_SNMP, phdr->msg_subtype, 0,phdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
	#if 0
		ret  = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP,
				phdr->msg_subtype, phdr->msg_index);
	#endif
	
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_L2, 
						IPC_TYPE_SNMP, phdr->msg_subtype, 0,0, IPC_OPCODE_NACK);
	}
	if(ret < 0)
	{
		zlog_err("%s[%d]: err %s:l2 reply snmp fail!\n",__FILE__,__LINE__,__func__);
	}

}

uint8_t mstp_global_config_get(struct mstp_global_config_snmp * mstp_info)
{


	if (NULL == mstp_global_bridge)
	{
		return MSTP_FALSE;
	}
	char cist_vlan_str[STRING_LEN] = {0};


	mstp_vlan_str_get_by_bitmap(bridge_cist_vlan_map.vlan_map, cist_vlan_str);

	memcpy(mstp_info->global_region_name,mstp_global_bridge->mst_cfg_id.cfg_name,strlen((char *)mstp_global_bridge->mst_cfg_id.cfg_name));

	mstp_info->global_revision_level = mstp_global_bridge->mst_cfg_id.revison_level;
	memcpy(mstp_info->global_vlan_Map,cist_vlan_str,strlen(cist_vlan_str));
	mstp_info->global_mode = \
				 mstp_global_bridge->stp_mode ? mstp_global_bridge->stp_mode : mstp_global_bridge->stp_mode+1;

	/*bridgeid = priority + mac*/
	memcpy(mstp_info->global_bridge_id,mstp_global_bridge->cist_br.bridge_mac.pri,2);
	memcpy(&mstp_info->global_bridge_id[2],mstp_global_bridge->bridge_mac.mac_addr,MAC_LEN);

	/*root_bridgeid = priority + mac*/
	memcpy(mstp_info->global_root_bridge_id,mstp_global_bridge->cist_br.root_priority.root_id.pri,2);
	memcpy(&mstp_info->global_root_bridge_id[2],mstp_global_bridge->cist_br.root_priority.root_id.mac_addr,MAC_LEN);

	/* global_extern_root_path_cost*/
	mstp_info->global_extern_root_path_cost = mstp_global_bridge->cist_br.root_priority.external_root_path_cost;

	/*global_region_root_bridge_id = priority + mac*/
	memcpy(mstp_info->global_region_root_bridge_id,mstp_global_bridge->cist_br.root_priority.regional_root_id.pri,2);
	memcpy(&mstp_info->global_region_root_bridge_id[2],mstp_global_bridge->cist_br.root_priority.regional_root_id.mac_addr,MAC_LEN);

	mstp_info->global_intern_root_path_cost = mstp_global_bridge->cist_br.root_priority.internal_root_path_cost;
	/*0 NULL 1 Master 2 Slave*/
	mstp_info->global_bridge_role = mstp_global_bridge->bridge_root_role;

	mstp_info->global_bridge_priority = mstp_global_bridge->br_priority;

	mstp_info->global_hello_time = mstp_global_bridge->hello_time;
	mstp_info->global_foward_delay = mstp_global_bridge->fwd_delay;
	mstp_info->global_max_age = mstp_global_bridge->msg_max_age;
	mstp_info->global_max_hop_count = mstp_global_bridge->msg_max_hop_count;


	return MSTP_TRUE;
}

int mstp_snmp_port_info_get(uint32_t ifindex, struct mstp_port_info_snmp  mstp_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct mstp_port_info_snmp  mstp_port_snmp = {0};

	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;

	msg_num  = IPC_MSG_LEN/sizeof(struct mstp_port_info_snmp);
	msg_num -=3;
	MSTP_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);
	memset(&mstp_port_snmp,0,sizeof(struct mstp_port_info_snmp));

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			//if ((NULL == pif) || (NULL == pif->mstp_port_info))
			if (NULL == pif)
			{
				continue;
			}

			mstp_port_info_get(&mstp_port_snmp,pif->mstp_port_info);
			mstp_port_snmp.ifindex = pif->ifindex;
			memcpy(&mstp_snmp_mux[data_num++], &mstp_port_snmp, sizeof(  struct mstp_port_info_snmp  ));
			memset(&mstp_port_snmp,0,sizeof(struct mstp_port_info_snmp  ));

			if (data_num == msg_num)
			{
				return data_num;
			}
		}
	}
	else 
	{
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)   
		{

			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])    
			{
				pbucket = l2if_table.buckets[val];
			}
			else                                      
			{
				for (++val; val<HASHTAB_SIZE; ++val)
				{
					if (NULL != l2if_table.buckets[val])
					{
						pbucket = l2if_table.buckets[val];
					}
				}
			}
		}

		if (NULL != pbucket)
		{
			for (i=0; i<msg_num; i++)
			{
				pnext = hios_hash_next_cursor(&l2if_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}

				pif = pnext->data;
				if ((NULL == pif) || (NULL == pif->mstp_port_info))
				{
					continue;
				}


				mstp_port_info_get(&mstp_port_snmp,pif->mstp_port_info);
				mstp_port_snmp.ifindex = pif->ifindex;
				memcpy(&mstp_snmp_mux[data_num++], &mstp_port_snmp, sizeof(  struct mstp_port_info_snmp));
				memset(&mstp_port_snmp,0,sizeof(struct mstp_port_info_snmp ));

				pbucket = pnext;
			}
		}
	}

	return data_num;
}
void mstp_port_info_get(struct mstp_port_info_snmp  * mstp_port_snmp ,struct mstp_port *mstp_port)
{
	struct mstp_common_port		*common_info = NULL;

	
	mstp_port_snmp->port_protocol_state = mstp_port ? MSTP_ENABLE : MSTP_DISABLE+2;
	if(NULL == mstp_port)
	{
		return ;

	}
	common_info = &(mstp_port->cist_port.common_info);

	//mstp_port_snmp->port_protocol_state = MSTP_ENABLE;
	mstp_port_snmp->port_state = common_info->sm_state_trans_state;
	mstp_port_snmp->port_mode = mstp_port->stp_mode ? mstp_port->stp_mode : mstp_port->stp_mode+1;
	
	if((MSTP_ST_DISCARDING == common_info->sm_state_trans_state) && (PORT_LINK_DOWN == mstp_port->link_status))
	{
		memcpy(mstp_port_snmp->port_designated_bridge_id,\
				mstp_port->cist_port.designated_priority.designated_bridge_id.pri,2);
		memcpy(&mstp_port_snmp->port_designated_bridge_id[2],\
				mstp_port->cist_port.designated_priority.designated_bridge_id.mac_addr,MAC_LEN);
	}
	else
	{
		memcpy(mstp_port_snmp->port_designated_bridge_id,\
				mstp_port->cist_port.port_priority.designated_bridge_id.pri,2);
		memcpy(&mstp_port_snmp->port_designated_bridge_id[2],\
				mstp_port->cist_port.port_priority.designated_bridge_id.mac_addr,MAC_LEN);
	}
	
	mstp_port_snmp->port_role = common_info->selectedRole;
	mstp_port_snmp->port_priority = mstp_port->port_pri;
	mstp_port_snmp->port_path_cost = mstp_port->cist_port.common_info.port_cost;
	mstp_port_snmp->port_message = mstp_port->cist_port.port_times.msg_age;
	mstp_port_snmp->port_edge_state = mstp_port->mstp_edge_port ? 1 : 2;
	mstp_port_snmp->port_filter_state = mstp_port->mstp_filter_port ? 1 : 2;
	mstp_port_snmp->port_p2p = mstp_port->mstp_p2p ? 1: 2;


}
/*used for SNMP*/



int mstp_snmp_instance_get(uint32_t ifindex, struct mstp_instance_snmp  mstp_snmp_mux[])
{

	struct mstp_instance_snmp  mstp_instance_snmp = {0};
	uint8_t instance_id = 0;
	uint8_t data_num = 0;
	uint8_t msg_num = 0;

	msg_num = IPC_MSG_LEN/sizeof(struct mstp_instance_snmp);
	memset(&mstp_instance_snmp,0,sizeof(struct mstp_instance_snmp));

	if (ifindex <= MSTP_INSTANCE_MAX  && mstp_global_bridge && PROTO_VERID_MSTP == mstp_global_bridge->stp_mode) 
	{	

		/*all intance info*/
		for(instance_id = ifindex ? ++ifindex:1;instance_id<=MSTP_INSTANCE_MAX && data_num < msg_num;instance_id++)
		{	
			mstp_instance_get(&mstp_instance_snmp,instance_id);
			memcpy(&mstp_snmp_mux[data_num++], &mstp_instance_snmp, sizeof(struct mstp_instance_snmp));
			memset(&mstp_instance_snmp,0,sizeof(struct mstp_instance_snmp));
		}

	}

	return data_num;
}
void mstp_instance_get(struct mstp_instance_snmp  * mstp_instance_snmp,uint8_t instance_id)
{
	char						vlan_str[STRING_LEN] = {0};

	mstp_instance_snmp->instance_id = instance_id;

	mstp_vlan_str_get_by_bitmap(mstp_global_bridge->mstp_instance[instance_id-1].msti_vlan.vlan_map, vlan_str);
	memcpy(mstp_instance_snmp->instance_vlan_map,vlan_str,strlen(vlan_str));

	mstp_instance_snmp->instance_bridge_priority = mstp_global_bridge->mstp_instance[instance_id-1].msti_br_priority;

	if(mstp_global_bridge->msti_br[instance_id-1] != NULL)
	{
		mstp_global_bridge->msti_br[instance_id-1]->bridge_priority.designated_bridge_id.pri[1] &= 0x00;
		memcpy(mstp_instance_snmp->instance_bridge_id,mstp_global_bridge->msti_br[instance_id-1]->bridge_priority.designated_bridge_id.pri,2);
		memcpy(&mstp_instance_snmp->instance_bridge_id[2],mstp_global_bridge->msti_br[instance_id-1]->bridge_priority.designated_bridge_id.mac_addr,MAC_LEN);

		mstp_global_bridge->msti_br[instance_id-1]->root_priority.rg_root_id.pri[1] &= 0x00;
		memcpy(mstp_instance_snmp->instance_region_root_bridge_id,mstp_global_bridge->msti_br[instance_id-1]->root_priority.rg_root_id.pri,2);
		memcpy(&mstp_instance_snmp->instance_region_root_bridge_id[2],mstp_global_bridge->msti_br[instance_id-1]->root_priority.rg_root_id.mac_addr,MAC_LEN);
		mstp_instance_snmp->instance_internal_root_path_cost = mstp_global_bridge->msti_br[instance_id-1]->root_priority.internal_root_path_cost;

	}

	return ;
}
int mstp_snmp_instance_port_get(uint32_t ifindex,uint32_t second_index, struct mstp_instance_port_snmp  mstp_snmp_mux[])
{

	struct mstp_instance_port_snmp  mstp_snmp = {0};
	uint8_t instance_id = 0;
	uint8_t data_num = 0;
	uint8_t msg_num = 0;
	struct mstp_msti_port	*msti_port = NULL;

	msg_num = IPC_MSG_LEN/sizeof(mstp_snmp);
	memset(&mstp_snmp,0,sizeof(mstp_snmp));


	if (ifindex <= MSTP_INSTANCE_MAX  && mstp_global_bridge && PROTO_VERID_MSTP == mstp_global_bridge->stp_mode) 
	{	

		/*all intance info*/
		for(instance_id = ifindex+1;instance_id<=MSTP_INSTANCE_MAX && data_num < msg_num;instance_id++)
		{	
			/*MSTI port list*/
			msti_port = mstp_global_bridge->mstp_instance[instance_id-1].msti_port_list;
			if(msti_port)
			{
				for( ; msti_port != NULL; msti_port = msti_port->next)
				{
					if(second_index && second_index >= msti_port->port_index)
					{
						continue;
					}
					mstp_instance_port_get(&mstp_snmp,instance_id,msti_port);
					memcpy(&mstp_snmp_mux[data_num++], &mstp_snmp, sizeof(mstp_snmp));
					memset(&mstp_snmp,0,sizeof(mstp_snmp));
					if(data_num ==  msg_num)
					{
						return data_num;
					}
				}
			}
		}

	}

	return data_num;

}
void mstp_instance_port_get(struct mstp_instance_port_snmp  * mstp_snmp,uint32_t instance_id, struct mstp_msti_port *msti_port)
{
	struct mstp_common_port		*common_info = NULL;

	if((NULL == msti_port) || (NULL == mstp_global_bridge))
	{
		return;
	}

	common_info = &(msti_port->common_info);

	mstp_snmp->instance_id = instance_id;
	mstp_snmp->instance_port_ifindex = msti_port->port_index;
	mstp_snmp->instance_port_state = common_info->sm_state_trans_state;
	mstp_snmp->instance_port_role = common_info->selectedRole;
	mstp_snmp->instance_port_priority = msti_port->mport->msti_port_pri[instance_id-1];
	//printf("%s[%d]###########pri =  %d\n",__FUNCTION__,__LINE__,mstp_snmp->instance_port_priority);
	mstp_snmp->instance_port_path_cost = msti_port->common_info.port_cost;
	//printf("%s[%d]###########cost =  %d\n",__FUNCTION__,__LINE__,mstp_snmp->instance_port_path_cost);
	if((MSTP_ST_DISCARDING == common_info->sm_state_trans_state) && (PORT_LINK_DOWN == msti_port->mport->link_status))
	{
		msti_port->designated_priority.designated_bridge_id.pri[1] &= 0x00;
		memcpy(mstp_snmp->instance_port_designal_bridge_id,msti_port->designated_priority.designated_bridge_id.pri,2);
		memcpy(&mstp_snmp->instance_port_designal_bridge_id[2],msti_port->designated_priority.designated_bridge_id.mac_addr,MAC_LEN);
		mstp_snmp->instance_port_max_hop = mstp_global_bridge->msti_br[instance_id-1]->bridge_times.remaining_hops;
	}
	else
	{	
		msti_port->port_priority.designated_bridge_id.pri[1] &= 0x00;
		memcpy(mstp_snmp->instance_port_designal_bridge_id,msti_port->port_priority.designated_bridge_id.pri,2);
		memcpy(&mstp_snmp->instance_port_designal_bridge_id[2],msti_port->port_priority.designated_bridge_id.mac_addr,MAC_LEN);
		mstp_snmp->instance_port_max_hop =  msti_port->port_times.remaining_hops;
	}

	/*init value*/
	mstp_snmp->instance_port_message = 0;
	mstp_snmp->instance_port_edge_state = 2;
	mstp_snmp->instance_port_filter_state = 2;
	mstp_snmp->instance_port_p2p = 2;

	return ;
}


int mstp_snmp_port_statistics_get(uint32_t ifindex, struct mstp_port_statistics_snmp  mstp_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct mstp_port_statistics_snmp  mstp_snmp = {0};

	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;


	msg_num  = IPC_MSG_LEN/sizeof(struct mstp_port_statistics_snmp);
	msg_num -=3;
	MSTP_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);
	memset(&mstp_snmp,0,sizeof(mstp_snmp));

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->mstp_port_info))
			{
				continue;
			}

			mstp_port_statistics_get(&mstp_snmp,pif->mstp_port_info);
			mstp_snmp.ifindex = pif->ifindex;
			memcpy(&mstp_snmp_mux[data_num++], &mstp_snmp, sizeof(mstp_snmp));
			memset(&mstp_snmp,0,sizeof(mstp_snmp));

			if (data_num == msg_num)
			{
				return data_num;
			}
		}
	}
	else 
	{
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)  
		{

			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])  
			{
				pbucket = l2if_table.buckets[val];
			}
			else                                     
			{
				for (++val; val<HASHTAB_SIZE; ++val)
				{
					if (NULL != l2if_table.buckets[val])
					{
						pbucket = l2if_table.buckets[val];
					}
				}
			}
		}

		if (NULL != pbucket)
		{
			for (i=0; i<msg_num; i++)
			{
				pnext = hios_hash_next_cursor(&l2if_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}

				pif = pnext->data;
				if ((NULL == pif) || (NULL == pif->mstp_port_info))
				{
					continue;
				}


				mstp_port_statistics_get(&mstp_snmp,pif->mstp_port_info);
				mstp_snmp.ifindex = pif->ifindex;
				memcpy(&mstp_snmp_mux[data_num++], &mstp_snmp, sizeof(mstp_snmp));
				memset(&mstp_snmp,0,sizeof(mstp_snmp));

				pbucket = pnext;
			}
		}
	}

	return data_num;
}
void mstp_port_statistics_get(struct mstp_port_statistics_snmp  * mstp_snmp ,struct mstp_port *mstp_port)
{
	mstp_snmp->port_tc_send = mstp_port->bpdu_tc_send;
	mstp_snmp->port_tc_recv = mstp_port->bpdu_tc_recv;
	mstp_snmp->port_tcn_send = mstp_port->bpdu_tcn_send;
	mstp_snmp->port_tcn_recv = mstp_port->bpdu_tcn_recv;

	mstp_snmp->port_bpdu_send = mstp_port->bpdu_config_send + mstp_port->bpdu_rst_send + mstp_port->bpdu_mst_send;
	mstp_snmp->port_bpdu_recv = mstp_port->bpdu_config_recv + mstp_port->bpdu_rst_recv + mstp_port->bpdu_mst_recv;

	mstp_snmp->port_config_send = mstp_port->bpdu_config_send;
	mstp_snmp->port_config_recv = mstp_port->bpdu_config_recv;

	mstp_snmp->port_rstp_send = mstp_port->bpdu_rst_send;
	mstp_snmp->port_rstp_recv = mstp_port->bpdu_rst_recv;

	mstp_snmp->port_mstp_send = mstp_port->bpdu_mst_send;
	mstp_snmp->port_mstp_recv = mstp_port->bpdu_mst_recv;

	return ;
}

