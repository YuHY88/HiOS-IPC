/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_api.c 	(used for SNMP)
*
*  date: 2017.1
*
*  modify:	2018.3.10 modified by liufuying to make LLDP module code beautiful
*
*/
#include <string.h>
#include <time.h>

#include <lib/log.h>
#include <lib/hash1.h>

#include "errcode.h"

#include "msg_ipc.h"
#include "msg_ipc_n.h"

#include "lldp.h"
//#include "l2_if.h"
#include "l2/l2_if.h"
#include "lldp_api.h"
#include "l2_snmp.h"
#include "lib/ospf_common.h"
#include "lldp_rx_sm.h"


extern lldp_global 		lldp_global_msg;
struct lldp_msap_trap	msap_insert_msap_buff;
unsigned char			msap_delete_msap_buff[2048];


/*get lldp local global cfg*/
unsigned int lldp_snmp_global_cfg_get(struct ipc_msghdr_n *phdr)
{
	int		ret = -1;
	
	/*ret = ipc_send_reply(&lldp_global_msg, sizeof(lldp_global), phdr->sender_id, MODULE_ID_L2,
                        IPC_TYPE_SNMP, phdr->msg_subtype, phdr->msg_index);*/
	ret = ipc_send_reply_n2(&lldp_global_msg, sizeof(lldp_global), 1, phdr->sender_id, MODULE_ID_L2,
                        IPC_TYPE_SNMP, phdr->msg_subtype, phdr->sequence,phdr->msg_index, IPC_OPCODE_REPLY);
	if(ret < 0)
    {
		zlog_err("[%s %d]ERROR: ipc_send_reply ret(%d)", __FUNCTION__, __LINE__, ret);

		return ERRNO_FAIL;
	}

	return ERRNO_SUCCESS;
}

/*get lldp local port cfg*/
unsigned int lldp_snmp_port_cfg_get(struct ipc_msghdr_n  *phdr)
{
	unsigned int		if_index = 0;
	struct l2if			*pif = NULL;
	struct lldp_port	*port = NULL;	
	int					ret = -1;

	/*get interface*/
	if_index = phdr->msg_index;
	pif = l2if_lookup(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif(%d) is NULL.\n",__FILE__,__LINE__,__func__, if_index);
		/*ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_L2,
							   IPC_TYPE_SNMP, phdr->msg_subtype, phdr->msg_index );*/
		//ret = l2_msg_send_noack(0,phdr,phdr->msg_index );
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
	                        phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0,phdr->msg_index, IPC_OPCODE_NACK);
		if(ret < 0)
		{
			zlog_err("[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
					   phdr->msg_index, ret );
		}

		return ERRNO_FAIL;
	}

	/*return local port value*/
	port = pif->lldp_port_info;
	if(port != NULL)
	{
		memcpy(port->alias, pif->alias, IFM_ALIAS_STRING_LEN+1);
		/*ret = ipc_send_reply(port, sizeof(struct lldp_port), phdr->sender_id, MODULE_ID_L2,
	                        IPC_TYPE_SNMP, phdr->msg_subtype, phdr->msg_index);*/
		ret = ipc_send_reply_n2(port, sizeof(struct lldp_port), 1, phdr->sender_id, MODULE_ID_L2, 
                        IPC_TYPE_SNMP, phdr->msg_subtype, phdr->sequence,phdr->msg_index, IPC_OPCODE_REPLY);
		if(ret < 0)
	    {
			zlog_err("[%s %d]ERROR: ipc_send_reply ret(%d)", __FUNCTION__, __LINE__, ret);

			return ERRNO_FAIL;
		}		
	}
	else
	{
		/*ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_L2,
							   IPC_TYPE_SNMP, phdr->msg_subtype, phdr->msg_index );*/
		//ret = l2_msg_send_noack(0,phdr,phdr->msg_index );
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
	                        phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0,phdr->msg_index, IPC_OPCODE_NACK);
		if(ret < 0)
		{
			zlog_err("[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
					   phdr->msg_index, ret );
			return ERRNO_FAIL;
		}
	}

	return ERRNO_SUCCESS;
}

int
lldp_snmp_get_tlvs (struct lldp_port *lldp_port_info)
{
	int val = 0;

	if (NULL == lldp_port_info)
	{
		return val;
	}
	
	if (CHECK_FLAG (lldp_port_info->tlv_cfg.basic_tlv_tx_enable, PORT_DESCRIPTION_TLV_TX_ENABLE))
	{
		val += 1; 
	}

	if (CHECK_FLAG (lldp_port_info->tlv_cfg.basic_tlv_tx_enable, SYSTEM_NAME_TLV_TX_ENABLE))
	{
		val += 2;
	}

	if (CHECK_FLAG (lldp_port_info->tlv_cfg.basic_tlv_tx_enable, SYSTEM_DESCRIPTION_TLV_TX_ENABLE))
	{
        val += 4;
	}

	if (CHECK_FLAG (lldp_port_info->tlv_cfg.basic_tlv_tx_enable, SYSTEM_CAPABILITIES_TLV_TX_ENABLE))
	{
		val += 8;
	}

	return val;
}


uint32_t lldp_snmp_port_cnofig_info_get(uint32_t ifindex, 
																		struct lldp_port_config_snmp lldp_port_config_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext = NULL;
	struct l2if   *pif	   = NULL;
	struct lldp_port_config_snmp lldp_port_config;
	uint32_t data_num = 0;
	uint32_t msg_num  = 0;	
	int cursor = 0;
	uint32_t i;

	msg_num  = IPC_MSG_LEN/sizeof(struct lldp_port_config_snmp);
	msg_num -= 3;
	memset(&lldp_port_config, 0, sizeof(struct lldp_port_config_snmp));

	LLDP_LOG_DBG("[%s]msg_num = %d\n",__FUNCTION__, msg_num);
	
	if (0 == ifindex)                   	/* first get data */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if (NULL == pif)
			{
				continue;
			}

			if (NULL == pif->lldp_port_info)
			{
				lldp_port_config.ifindex = pif->ifindex;
				lldp_port_config.adminStatus = 0;
				lldp_port_config.tlv_tx_enable = 0;
			}
			else
			{
				lldp_port_config.ifindex = pif->lldp_port_info->if_index;
				lldp_port_config.adminStatus = pif->lldp_port_info->adminStatus;
				lldp_port_config.tlv_tx_enable = lldp_snmp_get_tlvs(pif->lldp_port_info);
			}

			memcpy(&lldp_port_config_mux[data_num++], &lldp_port_config, sizeof(  struct lldp_port_config_snmp ));
			memset(&lldp_port_config, 0, sizeof(struct lldp_port_config_snmp));

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
			return data_num;
		}
	
		if (NULL != pbucket)
		{
			for (i=0; i < msg_num; i++)
			{
				pnext = hios_hash_next_cursor(&l2if_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}
		
				pif = pnext->data;

				if (NULL == pif->lldp_port_info)
				{
					lldp_port_config.ifindex = pif->ifindex;
					lldp_port_config.adminStatus = 0;
					lldp_port_config.tlv_tx_enable = 0;
				}
				else
				{
					lldp_port_config.ifindex = pif->lldp_port_info->if_index;
					lldp_port_config.adminStatus = pif->lldp_port_info->adminStatus;
					lldp_port_config.tlv_tx_enable = lldp_snmp_get_tlvs(pif->lldp_port_info);
				}

				memcpy(&lldp_port_config_mux[data_num++], &lldp_port_config, sizeof(  struct lldp_port_config_snmp ));
				memset(&lldp_port_config, 0, sizeof(struct lldp_port_config_snmp));
		
				pbucket = pnext;
			}
		}
	}
	
	return data_num;
}


void lldp_send_port_config_to_snmp(struct ipc_msghdr_n * phdr, 
									void * pdata, uint32_t data_num, uint32_t ifindex)
{
	LLDP_LOG_DBG("[%s]data_num = %d\n", __FUNCTION__, data_num);
	
	if (data_num > 0)
	{
		/*ipc_send_reply_bulk(pdata, data_num*sizeof(struct lldp_port_config_snmp), 
				data_num, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP, phdr->msg_subtype, ifindex);*/
				
		ipc_send_reply_n2(pdata, data_num*sizeof(struct lldp_port_config_snmp), data_num, phdr->sender_id, MODULE_ID_L2,
                        IPC_TYPE_SNMP, phdr->msg_subtype, phdr->sequence,ifindex, IPC_OPCODE_REPLY);
	}
	else
	{
		/*ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP,
				phdr->msg_subtype, ifindex);*/
				
		ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_L2,
                        IPC_TYPE_SNMP, phdr->msg_subtype, phdr->sequence,ifindex, IPC_OPCODE_REPLY);
	}

	return;
}


void lldp_snmp_port_config_get(struct ipc_msghdr_n *phdr)
{
	uint32_t data_num = 0;
	uint32_t msg_num = 0;
	uint32_t ifindex = 0;

    ifindex = phdr->msg_index;
    
	msg_num = IPC_MSG_LEN/sizeof(struct lldp_port_config_snmp);
	struct lldp_port_config_snmp lldp_port_config[msg_num];
	
	memset(lldp_port_config, 0, msg_num*sizeof(struct lldp_port_config_snmp));
	
	data_num = lldp_snmp_port_cnofig_info_get(ifindex, lldp_port_config);
	lldp_send_port_config_to_snmp(phdr, lldp_port_config, data_num, ifindex);
	return;
}


uint32_t lldp_snmp_stats_txrx_port_info_get(uint32_t ifindex, 
																		struct lldp_stats_txrx_port_snmp lldp_stats_txrx_port_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext = NULL;
	struct l2if   *pif	   = NULL;
	struct lldp_stats_txrx_port_snmp lldp_stats_txrx_port;
	uint32_t data_num = 0;
	uint32_t msg_num  = 0;	
	int cursor = 0;
	uint32_t i;

	msg_num  = IPC_MSG_LEN/sizeof(struct lldp_stats_txrx_port_snmp);
	msg_num -= 3;
	memset(&lldp_stats_txrx_port, 0, sizeof(struct lldp_stats_txrx_port_snmp));

	LLDP_LOG_DBG("[%s]msg_num = %d\n",__FUNCTION__, msg_num);
	
	if (0 == ifindex)                   	/* first get data */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->lldp_port_info))
			{
				continue;
			}

			lldp_stats_txrx_port.ifindex = pif->lldp_port_info->if_index;
			lldp_stats_txrx_port.tx_statsFramesOutTotal = pif->lldp_port_info->tx.statistics.statsFramesOutTotal;
			lldp_stats_txrx_port.rx_statsTLVsDiscardedTotal = pif->lldp_port_info->rx.statistics.statsTLVsDiscardedTotal;
			lldp_stats_txrx_port.rx_statsFramesInErrorsTotal = pif->lldp_port_info->rx.statistics.statsFramesInErrorsTotal;
			lldp_stats_txrx_port.rx_statsFramesInTotal = pif->lldp_port_info->rx.statistics.statsFramesInTotal;
			lldp_stats_txrx_port.rx_statsFramesDiscardedTotal = pif->lldp_port_info->rx.statistics.statsFramesDiscardedTotal;
			lldp_stats_txrx_port.rx_statsTLVsUnrecognizedTotal = pif->lldp_port_info->rx.statistics.statsTLVsUnrecognizedTotal;
			lldp_stats_txrx_port.rx_statsAgeoutsTotal = pif->lldp_port_info->rx.statistics.statsAgeoutsTotal;

			memcpy(&lldp_stats_txrx_port_mux[data_num++], &lldp_stats_txrx_port, sizeof(  struct lldp_stats_txrx_port_snmp ));
			memset(&lldp_stats_txrx_port, 0, sizeof(struct lldp_stats_txrx_port_snmp));

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
			return data_num;
		}
	
		if (NULL != pbucket)
		{
			for (i=0; i < msg_num; i++)
			{
				pnext = hios_hash_next_cursor(&l2if_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}
		
				pif = pnext->data;
				if ((NULL == pif) || (NULL == pif->lldp_port_info))
				{
					continue;
				}

				lldp_stats_txrx_port.ifindex = pif->lldp_port_info->if_index;
				lldp_stats_txrx_port.tx_statsFramesOutTotal = pif->lldp_port_info->tx.statistics.statsFramesOutTotal;
				lldp_stats_txrx_port.rx_statsTLVsDiscardedTotal = pif->lldp_port_info->rx.statistics.statsTLVsDiscardedTotal;
				lldp_stats_txrx_port.rx_statsFramesInErrorsTotal = pif->lldp_port_info->rx.statistics.statsFramesInErrorsTotal;
				lldp_stats_txrx_port.rx_statsFramesInTotal = pif->lldp_port_info->rx.statistics.statsFramesInTotal;
				lldp_stats_txrx_port.rx_statsFramesDiscardedTotal = pif->lldp_port_info->rx.statistics.statsFramesDiscardedTotal;
				lldp_stats_txrx_port.rx_statsTLVsUnrecognizedTotal = pif->lldp_port_info->rx.statistics.statsTLVsUnrecognizedTotal;
				lldp_stats_txrx_port.rx_statsAgeoutsTotal = pif->lldp_port_info->rx.statistics.statsAgeoutsTotal;

				memcpy(&lldp_stats_txrx_port_mux[data_num++], &lldp_stats_txrx_port, sizeof(  struct lldp_stats_txrx_port_snmp ));
				memset(&lldp_stats_txrx_port, 0, sizeof(struct lldp_stats_txrx_port_snmp));
		
				pbucket = pnext;
			}
		}
	}
	
	return data_num;
}


void lldp_send_stats_txrx_port_to_snmp(struct ipc_msghdr_n * phdr, 
									void * pdata, uint32_t data_num, uint32_t ifindex)
{
	LLDP_LOG_DBG("[%s]data_num = %d\n", __FUNCTION__, data_num);
	
	if (data_num > 0)
	{
		ipc_send_reply_n2(pdata, data_num*sizeof(struct lldp_stats_txrx_port_snmp), 
				data_num, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP, phdr->msg_subtype, phdr->sequence, ifindex, IPC_OPCODE_REPLY);
	}
	else
	{
		ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP,
				phdr->msg_subtype, 0, ifindex, IPC_OPCODE_NACK);
	}

	return;
}


void lldp_snmp_stats_txrx_port_get(struct ipc_msghdr_n *phdr)
{
	uint32_t data_num = 0;
	uint32_t msg_num = 0;
	uint32_t ifindex = 0;

    ifindex = phdr->msg_index;
    
	msg_num = IPC_MSG_LEN/sizeof(struct lldp_stats_txrx_port_snmp);
	struct lldp_stats_txrx_port_snmp lldp_stats_txrx_port[msg_num];
	
	memset(lldp_stats_txrx_port, 0, msg_num*sizeof(struct lldp_stats_txrx_port_snmp));
	
	data_num = lldp_snmp_stats_txrx_port_info_get(ifindex, lldp_stats_txrx_port);
	lldp_send_stats_txrx_port_to_snmp(phdr, lldp_stats_txrx_port, data_num, ifindex);
	return;
}


uint32_t lldp_snmp_loc_port_info_get(uint32_t ifindex, 
																		struct lldp_loc_port_snmp lldp_loc_port_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext = NULL;
	struct l2if   *pif	   = NULL;
	struct lldp_loc_port_snmp lldp_loc_port;
	uint32_t data_num = 0;
	uint32_t msg_num  = 0;	
	int cursor = 0;
	uint32_t  i;
	char alias[IFM_ALIAS_STRING_LEN+1];
	
	memset (alias, 0, IFM_ALIAS_STRING_LEN+1);

	msg_num  = IPC_MSG_LEN/sizeof(struct lldp_loc_port_snmp);
	msg_num -= 3;
	memset(&lldp_loc_port, 0, sizeof(struct lldp_loc_port_snmp));

	LLDP_LOG_DBG("[%s]msg_num = %d\n",__FUNCTION__, msg_num);
	
	if (0 == ifindex)                   	/* first get data */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->lldp_port_info))
			{
				continue;
			}

			lldp_loc_port.ifindex = pif->lldp_port_info->if_index;
			memcpy(lldp_loc_port.if_name, pif->lldp_port_info->if_name, strlen(pif->lldp_port_info->if_name));
			if(!strcmp(pif->alias, alias))
			{
				memcpy(lldp_loc_port.alias, pif->lldp_port_info->if_name, strlen(pif->lldp_port_info->if_name));
			}
			else
			{
				memcpy(lldp_loc_port.alias, &pif->alias[0], strlen(pif->alias));
			}

			memcpy(&lldp_loc_port_mux[data_num++], &lldp_loc_port, sizeof(  struct lldp_loc_port_snmp ));
			memset(&lldp_loc_port, 0, sizeof(struct lldp_loc_port_snmp));

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
			return data_num;
		}
	
		if (NULL != pbucket)
		{
			for (i=0; i < msg_num; i++)
			{
				pnext = hios_hash_next_cursor(&l2if_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}
		
				pif = pnext->data;
				if ((NULL == pif) || (NULL == pif->lldp_port_info))
				{
					continue;
				}

				lldp_loc_port.ifindex = pif->lldp_port_info->if_index;
				memcpy(lldp_loc_port.if_name, pif->lldp_port_info->if_name, strlen(pif->lldp_port_info->if_name));
				if(!strcmp(pif->alias, alias))
				{
					memcpy(lldp_loc_port.alias, pif->lldp_port_info->if_name, strlen(pif->lldp_port_info->if_name));
				}
				else
				{
					memcpy(lldp_loc_port.alias, &pif->alias[0], strlen(pif->alias));
				}

				memcpy(&lldp_loc_port_mux[data_num++], &lldp_loc_port, sizeof(	struct lldp_loc_port_snmp ));
				memset(&lldp_loc_port, 0, sizeof(struct lldp_loc_port_snmp));
		
				pbucket = pnext;
			}
		}
	}
	
	return data_num;
}


void lldp_send_loc_port_to_snmp(struct ipc_msghdr_n * phdr, 
									void * pdata, uint32_t data_num, uint32_t ifindex)
{
	LLDP_LOG_DBG("[%s]data_num = %d\n", __FUNCTION__, data_num);
	
	if (data_num > 0)
	{
		ipc_send_reply_n2(pdata, data_num*sizeof(struct lldp_loc_port_snmp), 
				data_num, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP, phdr->msg_subtype, phdr->sequence, ifindex, IPC_OPCODE_REPLY);
	}
	else
	{
		ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP,
				phdr->msg_subtype, 0, ifindex, IPC_OPCODE_NACK);
	}

	return;
}


void lldp_snmp_loc_port_get(struct ipc_msghdr_n *phdr)
{
	uint32_t data_num = 0;
	uint32_t msg_num = 0;
	uint32_t ifindex = 0;

    ifindex = phdr->msg_index;
    
	msg_num = IPC_MSG_LEN/sizeof(struct lldp_loc_port_snmp);
	struct lldp_loc_port_snmp lldp_loc_port[msg_num];
	
	memset(lldp_loc_port, 0, msg_num*sizeof(struct lldp_loc_port_snmp));
	
	data_num = lldp_snmp_loc_port_info_get(ifindex, lldp_loc_port);
	lldp_send_loc_port_to_snmp(phdr, lldp_loc_port, data_num, ifindex);
	return;
}


uint32_t lldp_snmp_loc_port_priv_info_get(uint32_t ifindex, 
																		struct lldp_loc_port_priv_snmp lldp_loc_port_priv_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext = NULL;
	struct l2if   *pif	   = NULL;
	struct lldp_loc_port_priv_snmp lldp_loc_port_priv;
	uint32_t data_num = 0;
	uint32_t msg_num  = 0;	
	int cursor = 0;
	uint32_t i;

	msg_num  = IPC_MSG_LEN/sizeof(struct lldp_loc_port_priv_snmp);
	msg_num -= 3;
	memset(&lldp_loc_port_priv, 0, sizeof(struct lldp_loc_port_priv_snmp));

	LLDP_LOG_DBG("[%s]msg_num = %d\n",__FUNCTION__, msg_num);
	
	if (0 == ifindex)                   	/* first get data */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->lldp_port_info))
			{
				continue;
			}

			lldp_loc_port_priv.ifindex = pif->lldp_port_info->if_index;
			lldp_loc_port_priv.msap_num = pif->lldp_port_info->msap_num;
			memcpy(lldp_loc_port_priv.if_name, pif->lldp_port_info->if_name, strlen(pif->lldp_port_info->if_name));
			memcpy(lldp_loc_port_priv.source_mac, lldp_global_msg.source_mac, 6);
			if (!pif->lldp_port_info->arp_nd_learning)
			{
				lldp_loc_port_priv.arp_nd_learning = 2;
				lldp_loc_port_priv.arp_nd_learning_vlan = 0;
			}
			else
			{
				lldp_loc_port_priv.arp_nd_learning = pif->lldp_port_info->arp_nd_learning;
				lldp_loc_port_priv.arp_nd_learning_vlan = pif->lldp_port_info->arp_nd_learning_vlan;
			}

			memcpy(&lldp_loc_port_priv_mux[data_num++], &lldp_loc_port_priv, sizeof(  struct lldp_loc_port_priv_snmp ));
			memset(&lldp_loc_port_priv, 0, sizeof(struct lldp_loc_port_priv_snmp));

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
			return data_num;
		}
	
		if (NULL != pbucket)
		{
			for (i=0; i < msg_num; i++)
			{
				pnext = hios_hash_next_cursor(&l2if_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}
		
				pif = pnext->data;
				if ((NULL == pif) || (NULL == pif->lldp_port_info))
				{
					continue;
				}

				lldp_loc_port_priv.ifindex = pif->lldp_port_info->if_index;
				lldp_loc_port_priv.msap_num = pif->lldp_port_info->msap_num;
				memcpy(lldp_loc_port_priv.if_name, pif->lldp_port_info->if_name, strlen(pif->lldp_port_info->if_name));
				memcpy(lldp_loc_port_priv.source_mac, lldp_global_msg.source_mac, 6);
				if (!pif->lldp_port_info->arp_nd_learning)
				{
					lldp_loc_port_priv.arp_nd_learning = 2;
					lldp_loc_port_priv.arp_nd_learning_vlan = 0;
				}
				else
				{
					lldp_loc_port_priv.arp_nd_learning = pif->lldp_port_info->arp_nd_learning;
					lldp_loc_port_priv.arp_nd_learning_vlan = pif->lldp_port_info->arp_nd_learning_vlan;
				}

				memcpy(&lldp_loc_port_priv_mux[data_num++], &lldp_loc_port_priv, sizeof(	struct lldp_loc_port_priv_snmp ));
				memset(&lldp_loc_port_priv, 0, sizeof(struct lldp_loc_port_priv_snmp));
		
				pbucket = pnext;
			}
		}
	}
	
	return data_num;
}


void lldp_send_loc_port_priv_to_snmp(struct ipc_msghdr_n * phdr, 
									void * pdata, uint32_t data_num, uint32_t ifindex)
{
	LLDP_LOG_DBG("[%s]data_num = %d\n", __FUNCTION__, data_num);
	
	if (data_num > 0)
	{
		ipc_send_reply_n2(pdata, data_num*sizeof(struct lldp_loc_port_priv_snmp), 
				data_num, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP, phdr->msg_subtype, phdr->sequence, ifindex, IPC_OPCODE_REPLY);
	}
	else
	{
		ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP,
					phdr->msg_subtype, phdr->sequence, ifindex, IPC_OPCODE_REPLY);
	}

	return;
}


void lldp_snmp_loc_port_priv_get(struct ipc_msghdr_n *phdr)
{
	uint32_t data_num = 0;
	uint32_t msg_num = 0;
	uint32_t ifindex = 0;

    ifindex = phdr->msg_index;
    
	msg_num = IPC_MSG_LEN/sizeof(struct lldp_loc_port_priv_snmp);
	struct lldp_loc_port_priv_snmp lldp_loc_port_priv[msg_num];
	
	memset(lldp_loc_port_priv, 0, msg_num*sizeof(struct lldp_loc_port_priv_snmp));
	
	data_num = lldp_snmp_loc_port_priv_info_get(ifindex, lldp_loc_port_priv);
	lldp_send_loc_port_priv_to_snmp(phdr, lldp_loc_port_priv, data_num, ifindex);
	return;
}


/*get lldp local port msap cfg
**msap:MAC service access point (neighbor msg)
*/
#if 0
unsigned int lldp_snmp_port_msap_get(struct ipc_msghdr *phdr)
{
	unsigned char			buff[IPC_MSG_LEN1];
	unsigned int			if_index = 0;
	struct l2if 			*pif = NULL;
	struct lldp_port		*port = NULL;	
	int 					ret = 0;
	int						i = 0;
	struct lldp_msap		*port_msap = NULL;
	unsigned int			msap_len = 0;

	msap_len = sizeof(struct lldp_msap);

	/*get interface*/
	if_index = phdr->msg_index;
	
	pif = l2if_lookup(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		ret = ipc_send_noack ( 0, phdr->sender_id, MODULE_ID_L2,
							   IPC_TYPE_SNMP, phdr->msg_subtype, phdr->msg_index );
		if(ret < 0)
		{
			zlog_err("[%s %d] ipc_send_noack interface 0x%0x	erro ret %d", __FUNCTION__, __LINE__,
					   phdr->msg_index, ret );
		}

		return ERRNO_FAIL;
	}

	port = pif->lldp_port_info;
	if(port != NULL)
	{
		/*get port msap, can reply 14 msap max
		**if more than 14 msap, must send two frame : FIXME
		**we think max msap num is not more than 14*/

		port_msap = port->msap_cache;
		for(i = 0; i < port->msap_num; i++)
		{
			memcpy(&buff[i*msap_len], port_msap, sizeof(struct lldp_msap));
			port_msap = port_msap->next;
		}

		/*send port msap msg*/
		ret = ipc_send_reply(buff, port->msap_num * msap_len, phdr->sender_id, MODULE_ID_L2,
							IPC_TYPE_SNMP, phdr->msg_subtype, phdr->msg_index);

		if(ret < 0)
		{
			zlog_err("[%s %d]ERROR: ipc_send_reply ret(%d)", __FUNCTION__, __LINE__, ret);

			return ERRNO_FAIL;
		}		
	}
	else
	{
		ret = ipc_send_noack(0, phdr->sender_id, MODULE_ID_L2,
							   IPC_TYPE_SNMP, phdr->msg_subtype, phdr->msg_index );
		if(ret < 0)
		{
			return ERRNO_FAIL;
		}
	}

	return ERRNO_SUCCESS;
}
#else
void lldp_snmp_port_msap_get(struct ipc_msghdr_n *phdr, uint8_t msap_index)
{
	uint32_t data_num = 0;
	uint32_t msg_num = 0;
	uint32_t ifindex = 0;

    ifindex = phdr->msg_index;
    
	msg_num = IPC_MSG_LEN/sizeof(struct lldp_msap_snmp);
	struct lldp_msap_snmp lldp_msap[msg_num];
	
	memset(lldp_msap, 0, msg_num*sizeof(struct lldp_msap_snmp));
	
	data_num = lldp_snmp_msap_info_get(ifindex, msap_index, lldp_msap);
	lldp_send_msap_to_snmp(phdr, lldp_msap, data_num, ifindex);

	return;
}
#endif


void lldp_get_msap_info(struct lldp_msap_info_data *msap_info ,
															struct lldp_msap *msap)
{
	memset(msap_info, 0, sizeof (struct lldp_msap_info_data));
	
	msap_info->chassis_id_subtype = msap->neighbor_msg.chassis_id_subtype;

	if (msap->neighbor_msg.chassis_id_subtype == 4)//mac
	{
		memcpy(msap_info->chassis_id_addr, msap->neighbor_msg.chassis_id_mac_addr, 6);
	}
	else if (msap->neighbor_msg.chassis_id_subtype == 5)//ip
	{
		memcpy(msap_info->chassis_id_addr, msap->neighbor_msg.chassis_id_net_addr, 4);
	}

	msap_info->port_id_subtype = msap->neighbor_msg.port_id_subtype;

	if (msap->neighbor_msg.port_id_subtype == 4)//ip
	{
		memcpy(msap_info->port_id, msap->neighbor_msg.network_addr, 4);
	}
	else if (msap->neighbor_msg.port_id_subtype == 5)
	{
		memcpy(msap_info->port_id, msap->neighbor_msg.if_name, INTERFACE_NAMSIZ);
	}

	memcpy(msap_info->port_id, msap->neighbor_msg.if_name, INTERFACE_NAMSIZ);

	memcpy(msap_info->port_descr, msap->neighbor_msg.pt_descr, IFM_ALIAS_STRING_LEN+1);
	memcpy(msap_info->sys_name, msap->neighbor_msg.sys_name, MAX_SIZE);
	memcpy(msap_info->sys_descr, msap->neighbor_msg.sys_descr, SYSTEM_DESCR_MAX_SIZE);

	msap_info->sys_cap = msap->neighbor_msg.sys_cap;
	msap_info->sys_cap_enabled = msap->neighbor_msg.sys_cap_enabled;

	msap_info->svlan_tpid = msap->svlan_tpid;
	msap_info->svlan = msap->svlan;
	msap_info->cvlan_tpid = msap->cvlan_tpid;
	msap_info->cvlan = msap->cvlan;
	msap_info->rx_ttl = msap->neighbor_msg.rx_ttl;

	msap_info->mgmt_addr_sub_type = msap->neighbor_msg.mgmt_addr_sub_type;
	memcpy(msap_info->mgmt_addr, msap->neighbor_msg.mgmt_addr, 31);

	return;
}


void lldp_get_msap_man_addr_info(struct lldp_msap_man_addr_info_data *msap_info ,
															struct lldp_msap *msap)
{
	memset(msap_info, 0, sizeof (struct lldp_msap_man_addr_info_data));
	
	msap_info->mgmt_addr_sub_type = msap->neighbor_msg.mgmt_addr_sub_type;
	memcpy(msap_info->mgmt_addr, msap->neighbor_msg.mgmt_addr, 31);

	memcpy(msap_info->time_mark, msap->neighbor_msg.time_mark, TIME_MARK_INDEX_MAX*sizeof(time_t));
	msap_info->time_mark_index = msap->neighbor_msg.time_mark_index;
	msap_info->if_numbering = msap->neighbor_msg.if_numbering;
	msap_info->if_number = msap->neighbor_msg.if_number;

	return;
}


void lldp_snmp_port_msap_man_addr_get(struct ipc_msghdr_n *phdr, uint8_t msap_index)
{
	uint32_t data_num = 0;
	uint32_t msg_num = 0;
	uint32_t ifindex = 0;

    ifindex = phdr->msg_index;
    
	msg_num = IPC_MSG_LEN/sizeof(struct lldp_msap_man_addr_snmp);
	struct lldp_msap_man_addr_snmp lldp_msap[msg_num];
	
	memset(lldp_msap, 0, msg_num*sizeof(struct lldp_msap_man_addr_snmp));
	
	data_num = lldp_snmp_msap_man_addr_info_get(ifindex, msap_index, lldp_msap);
	lldp_send_msap_man_addr_to_snmp(phdr, lldp_msap, data_num, ifindex);

	return;
}


uint32_t lldp_snmp_msap_info_get(uint32_t ifindex, uint32_t msap_index,struct lldp_msap_snmp lldp_msap_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext = NULL;
	struct l2if   *pif	   = NULL;
	struct lldp_msap_snmp lldp_msap = {0};
	struct lldp_msap* 	msap_cache = NULL;
	uint8_t port_msap_count = 0;
	uint8_t msap_temp_count = 0;
	uint32_t data_num = 0;
	uint32_t msg_num  = 0;	
	uint8_t next_ifindex_flag = 0;
	int cursor = 0;

	msg_num  = IPC_MSG_LEN/sizeof(struct lldp_msap_snmp);
	memset(&lldp_msap, 0, sizeof(struct lldp_msap_snmp));

	LLDP_LOG_DBG("[%s]msg_num = %d\n",__FUNCTION__, msg_num);

	
	if ((0 == ifindex) && (0 == msap_index))                   	/* first get data from efm */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			port_msap_count = 0;
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->lldp_port_info))
			{
				continue;
			}

			if ((pif->lldp_port_info->msap_num == 0) ||
				(pif->lldp_port_info->msap_cache == NULL))
			{
				continue;
			}

			msap_cache = pif->lldp_port_info->msap_cache;
			
			while(msap_cache != NULL)
			{
				lldp_get_msap_info(&lldp_msap.lldp_msap_info, msap_cache);
				port_msap_count++;
				lldp_msap.lldp_msap_info.msap_index = port_msap_count;
				lldp_msap.ifindex = pif->lldp_port_info->if_index;

				memcpy(&lldp_msap_mux[data_num++], &lldp_msap, sizeof(struct lldp_msap_snmp));
				
				msap_cache = msap_cache->next;

				if (data_num == msg_num)
				{
					return data_num;
				}
			}

			if (msap_cache == NULL)
			{
				continue;
			}
		} 
	}
	else
	{        
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)	
		{
			return data_num;
		}

		while (pbucket)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->lldp_port_info))
			{
				goto loop;
			}

			if((msap_index !=0) && (!next_ifindex_flag))
			{  
				msap_cache = pif->lldp_port_info->msap_cache;
				
				while(msap_cache != NULL)
				{
					msap_temp_count++;

					if (msap_index >= msap_temp_count)
					{
						msap_cache = msap_cache->next;
						continue;
					}
					
					lldp_get_msap_info(&lldp_msap.lldp_msap_info, msap_cache);
					port_msap_count = msap_temp_count;
					lldp_msap.lldp_msap_info.msap_index = port_msap_count;
					lldp_msap.ifindex = pif->lldp_port_info->if_index;
				
					memcpy(&lldp_msap_mux[data_num++], &lldp_msap, sizeof(struct lldp_msap_snmp));
					
					msap_cache = msap_cache->next;
				
					if (data_num == msg_num)
					{
						return data_num;
					}
				}

				if (msap_cache == NULL)
				{
					goto loop;
				}	
			}
			else
			{
				msap_cache = pif->lldp_port_info->msap_cache;
				
				while(msap_cache != NULL)
				{
					lldp_get_msap_info(&lldp_msap.lldp_msap_info, msap_cache);
					port_msap_count++;
					lldp_msap.lldp_msap_info.msap_index = port_msap_count;
					lldp_msap.ifindex = pif->lldp_port_info->if_index;
				
					memcpy(&lldp_msap_mux[data_num++], &lldp_msap, sizeof(struct lldp_msap_snmp));
					
					msap_cache = msap_cache->next;
				
					if (data_num == msg_num)
					{
						return data_num;
					}
				}

				if (msap_cache == NULL)
				{
					goto loop;
				}	
			}
				
			loop:
			pnext = hios_hash_next_cursor(&l2if_table, pbucket);
			if ((NULL == pnext) || (NULL == pnext->data))
			{
				break;
			}

			next_ifindex_flag = 1;
			port_msap_count = 0;
			pbucket = pnext;
		}
	}
	
	return data_num;
}


uint32_t lldp_snmp_msap_man_addr_info_get(uint32_t ifindex, uint32_t msap_index,
													struct lldp_msap_man_addr_snmp lldp_msap_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext = NULL;
	struct l2if   *pif	   = NULL;
	struct lldp_msap_man_addr_snmp lldp_msap = {0};
	struct lldp_msap* 	msap_cache = NULL;
	uint8_t port_msap_count = 0;
	uint8_t msap_temp_count = 0;
	uint32_t data_num = 0;
	uint32_t msg_num  = 0;	
	uint8_t next_ifindex_flag = 0;
	int cursor = 0;

	msg_num  = IPC_MSG_LEN/sizeof(struct lldp_msap_man_addr_snmp);
	msg_num -= 3;
	memset(&lldp_msap, 0, sizeof(struct lldp_msap_man_addr_snmp));

	LLDP_LOG_DBG("[%s]msg_num = %d\n",__FUNCTION__, msg_num);

	
	if ((0 == ifindex) && (0 == msap_index))                   	/* first get data from efm */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			port_msap_count = 0;
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->lldp_port_info))
			{
				continue;
			}

			if ((pif->lldp_port_info->msap_num == 0) ||
				(pif->lldp_port_info->msap_cache == NULL))
			{
				continue;
			}

			msap_cache = pif->lldp_port_info->msap_cache;
			
			while(msap_cache != NULL)
			{
				lldp_get_msap_man_addr_info(&lldp_msap.lldp_msap_man_addr_info, msap_cache);
				port_msap_count++;
				lldp_msap.lldp_msap_man_addr_info.msap_index = port_msap_count;
				lldp_msap.ifindex = pif->lldp_port_info->if_index;

				memcpy(&lldp_msap_mux[data_num++], &lldp_msap, sizeof(struct lldp_msap_man_addr_snmp));
				
				msap_cache = msap_cache->next;

				if (data_num == msg_num)
				{
					return data_num;
				}
			}

			if (msap_cache == NULL)
			{
				continue;
			}
		} 
	}
	else
	{        
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)	
		{
			return data_num;
		}

		while (pbucket)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->lldp_port_info))
			{
				goto loop;
			}

			if((msap_index !=0) && (!next_ifindex_flag))
			{  
				msap_cache = pif->lldp_port_info->msap_cache;
				
				while(msap_cache != NULL)
				{
					msap_temp_count++;

					if (msap_index >= msap_temp_count)
					{
						msap_cache = msap_cache->next;
						continue;
					}
					
					lldp_get_msap_man_addr_info(&lldp_msap.lldp_msap_man_addr_info, msap_cache);
					port_msap_count = msap_temp_count;
					lldp_msap.lldp_msap_man_addr_info.msap_index = port_msap_count;
					lldp_msap.ifindex = pif->lldp_port_info->if_index;
				
					memcpy(&lldp_msap_mux[data_num++], &lldp_msap, sizeof(struct lldp_msap_man_addr_snmp));
					
					msap_cache = msap_cache->next;
				
					if (data_num == msg_num)
					{
						return data_num;
					}
				}

				if (msap_cache == NULL)
				{
					goto loop;
				}	
			}
			else
			{
				msap_cache = pif->lldp_port_info->msap_cache;
				
				while(msap_cache != NULL)
				{
					lldp_get_msap_man_addr_info(&lldp_msap.lldp_msap_man_addr_info, msap_cache);
					port_msap_count++;
					lldp_msap.lldp_msap_man_addr_info.msap_index = port_msap_count;
					lldp_msap.ifindex = pif->lldp_port_info->if_index;
				
					memcpy(&lldp_msap_mux[data_num++], &lldp_msap, sizeof(struct lldp_msap_man_addr_snmp));
					
					msap_cache = msap_cache->next;
				
					if (data_num == msg_num)
					{
						return data_num;
					}
				}

				if (msap_cache == NULL)
				{
					goto loop;
				}	
			}
				
			loop:
			pnext = hios_hash_next_cursor(&l2if_table, pbucket);
			if ((NULL == pnext) || (NULL == pnext->data))
			{
				break;
			}

			next_ifindex_flag = 1;
			port_msap_count = 0;
			pbucket = pnext;
		}
	}
	
	return data_num;
}


void lldp_send_msap_to_snmp(struct ipc_msghdr_n * phdr, void * pdata, uint32_t data_num, uint32_t ifindex)
{
	LLDP_LOG_DBG("[%s]data_num = %d\n", __FUNCTION__, data_num);
	
	if (data_num > 0)
	{
		/*ipc_send_reply_bulk(pdata, data_num*sizeof(struct lldp_msap_snmp), 
				data_num, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP, phdr->msg_subtype, ifindex);*/
				
		ipc_send_reply_n2(pdata, data_num*sizeof(struct lldp_msap_snmp), data_num, phdr->sender_id, MODULE_ID_L2,
                        IPC_TYPE_SNMP, phdr->msg_subtype, phdr->sequence,ifindex, IPC_OPCODE_REPLY);
	}
	else
	{
		/*ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP,
				phdr->msg_subtype, ifindex);*/
		
			ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_L2,
                        IPC_TYPE_SNMP, phdr->msg_subtype, phdr->sequence,ifindex, IPC_OPCODE_REPLY);
	}

	return;
}


void lldp_send_msap_man_addr_to_snmp(struct ipc_msghdr_n * phdr, void * pdata, 
												uint32_t data_num, uint32_t ifindex)
{
	LLDP_LOG_DBG("[%s]data_num = %d\n", __FUNCTION__, data_num);
	
	if (data_num > 0)
	{
		/*ipc_send_reply_bulk(pdata, data_num*sizeof(struct lldp_msap_man_addr_snmp), 
				data_num, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP, phdr->msg_subtype, ifindex);*/
				
		ipc_send_reply_n2(pdata, data_num*sizeof(struct lldp_msap_man_addr_snmp), data_num, phdr->sender_id, MODULE_ID_L2,
		 			IPC_TYPE_SNMP, phdr->msg_subtype,  phdr->sequence,ifindex, IPC_OPCODE_REPLY);
	}
	else
	{
		/*ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP,
				phdr->msg_subtype, ifindex);*/
				
		ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_L2, 
		 			IPC_TYPE_SNMP, phdr->msg_subtype,  phdr->sequence,ifindex, IPC_OPCODE_REPLY);
	}

	return;
}


int lldp_trap_insert_msap_to_snmpd(struct lldp_port *lldp_port, struct lldp_msap *msap_msg)
{
	if((NULL == lldp_port) || (NULL == msap_msg))
	{
		return -1;
	}
	
	memset(&msap_insert_msap_buff, 0x00, sizeof(struct lldp_msap_trap));

	/**/
	msap_insert_msap_buff.msap_num = 1;
	memcpy(msap_insert_msap_buff.if_name, lldp_port->if_name, INTERFACE_NAMSIZ);

	msap_insert_msap_buff.chassis_id_subtype = msap_msg->neighbor_msg.chassis_id_subtype;
	
	if(CHASSIS_ID_MAC_ADDRESS == msap_msg->neighbor_msg.chassis_id_subtype)
	{
		memcpy(msap_insert_msap_buff.chassis_id_mac_addr, msap_msg->neighbor_msg.chassis_id_mac_addr, MAC_LEN);
	}
	else if(CHASSIS_ID_NETWORK_ADDRESS == msap_msg->neighbor_msg.chassis_id_subtype)
	{		
		memcpy(msap_insert_msap_buff.chassis_id_net_addr, msap_msg->neighbor_msg.chassis_id_mac_addr, IPV4_LEN);	
	}

	msap_insert_msap_buff.port_id_subtype = msap_msg->neighbor_msg.port_id_subtype;

	if(PORT_ID_INTERFACE_NAME == msap_msg->neighbor_msg.port_id_subtype)
	{		
		memcpy(msap_insert_msap_buff.port_if_name, msap_msg->neighbor_msg.if_name, INTERFACE_NAMSIZ);		
	}
	else if(PORT_ID_NETWORK_ADDRESS == msap_msg->neighbor_msg.port_id_subtype)
	{
		memcpy(msap_insert_msap_buff.network_addr, msap_msg->neighbor_msg.network_addr, 4);
	}

	/*lldpRemPortId*/
	memcpy(msap_insert_msap_buff.pt_descr,  msap_msg->neighbor_msg.pt_descr, IFM_ALIAS_STRING_LEN+1);
	memcpy(msap_insert_msap_buff.sys_name,  msap_msg->neighbor_msg.sys_name, MAX_SIZE);
	memcpy(msap_insert_msap_buff.sys_descr, msap_msg->neighbor_msg.sys_descr,SYSTEM_DESCR_MAX_SIZE);

	/*if(-1 == ipc_send_common(&msap_insert_msap_buff, sizeof(struct lldp_msap_trap), 1,
					MODULE_ID_SNMPD, MODULE_ID_L2, IPC_TYPE_SNMP,
					IPC_TYPE_ALARM_LLDP_INSERT_TRAP, IPC_OPCODE_INVALID))*/
	if(-1 ==  ipc_send_msg_n2(&msap_insert_msap_buff, sizeof(struct lldp_msap_trap), 1,
					MODULE_ID_SNMPD_TRAP, MODULE_ID_L2, IPC_TYPE_SNMP,
					IPC_TYPE_ALARM_LLDP_INSERT_TRAP,IPC_OPCODE_INVALID,0))
	{
		zlog_err("%s[%d] : LLDP trap insert msap failed", __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}


void lldp_add_delete_msap_info(struct lldp_msap *msap_msg, unsigned int msap_num)
{
	struct lldp_neighbor_trap_info	neighbor_trap_info;
	
	unsigned int					low = 0;
	unsigned int					neighbor_trap_info_len = 0;

	if(NULL == msap_msg)
	{
		return;
	}

	neighbor_trap_info_len = sizeof(struct lldp_neighbor_trap_info);

	neighbor_trap_info.chassis_id_subtype = msap_msg->neighbor_msg.chassis_id_subtype;
	if(CHASSIS_ID_MAC_ADDRESS == msap_msg->neighbor_msg.chassis_id_subtype)
	{
		memcpy(neighbor_trap_info.chassis_id_mac_addr, msap_msg->neighbor_msg.chassis_id_mac_addr, MAC_LEN);
	}
	else if(CHASSIS_ID_NETWORK_ADDRESS == msap_msg->neighbor_msg.chassis_id_subtype)
	{		
		memcpy(neighbor_trap_info.chassis_id_net_addr, msap_msg->neighbor_msg.chassis_id_mac_addr, IPV4_LEN);	
	}

	neighbor_trap_info.port_id_subtype = msap_msg->neighbor_msg.port_id_subtype;
	if(PORT_ID_INTERFACE_NAME == msap_msg->neighbor_msg.port_id_subtype)
	{		
		memcpy(neighbor_trap_info.port_if_name, msap_msg->neighbor_msg.if_name, INTERFACE_NAMSIZ);		
	}
	else if(PORT_ID_NETWORK_ADDRESS == msap_msg->neighbor_msg.port_id_subtype)
	{
		memcpy(neighbor_trap_info.network_addr, msap_msg->neighbor_msg.network_addr, 4);
		
	}

	/*lldpRemPortId*/
	memcpy(neighbor_trap_info.pt_descr,  msap_msg->neighbor_msg.pt_descr, IFM_ALIAS_STRING_LEN+1);
	memcpy(neighbor_trap_info.sys_name,  msap_msg->neighbor_msg.sys_name, MAX_SIZE);
	memcpy(neighbor_trap_info.sys_descr, msap_msg->neighbor_msg.sys_descr,SYSTEM_DESCR_MAX_SIZE);

	low = msap_num*neighbor_trap_info_len + 33;

	memcpy(&msap_delete_msap_buff[low], &neighbor_trap_info, sizeof(struct lldp_neighbor_trap_info));
}

int lldp_trap_ageout_msap_to_snmpd(unsigned int trap_info_len)
{
	/*if(-1 == ipc_send_common(msap_delete_msap_buff, trap_info_len, 1,
					MODULE_ID_SNMPD, MODULE_ID_L2, IPC_TYPE_SNMP,
					IPC_TYPE_ALARM_LLDP_AGEOUT_TRAP, IPC_OPCODE_INVALID))*/
	if(-1 ==  ipc_send_msg_n2(msap_delete_msap_buff, trap_info_len, 1,
					MODULE_ID_SNMPD_TRAP, MODULE_ID_L2, IPC_TYPE_SNMP,
					IPC_TYPE_ALARM_LLDP_AGEOUT_TRAP, IPC_OPCODE_INVALID,0))
	{
		zlog_err("%s[%d] : LLDP trap ageout msap failed", __FUNCTION__, __LINE__);
		return -1;
	}
	
	return 0;
}

int lldp_trap_delete_msap_to_snmpd(unsigned int	 trap_info_len)
{	
	/*if(-1 == ipc_send_common(msap_delete_msap_buff, trap_info_len, 1,
					MODULE_ID_SNMPD, MODULE_ID_L2, IPC_TYPE_SNMP,
					IPC_TYPE_ALARM_LLDP_DELETE_TRAP, IPC_OPCODE_INVALID))*/
		if(-1 ==  ipc_send_msg_n2(msap_delete_msap_buff, trap_info_len, 1,
					MODULE_ID_SNMPD_TRAP, MODULE_ID_L2, IPC_TYPE_SNMP,
					IPC_TYPE_ALARM_LLDP_DELETE_TRAP, IPC_OPCODE_INVALID,0))
	{
		zlog_err("%s[%d] : LLDP trap delete msap failed", __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}


int lldp_arp_learn_cfg_set(uint32_t ifindex , int arp_learnable)
{    
    struct hash_bucket *pbucket = NULL;
    struct l2if   *pif	   = NULL;
    int cursor = 0;
    int ret = 0 ;
    
    if (arp_learnable != OSPF_DCN_ENABLE && arp_learnable != OSPF_DCN_DISABLE)
        return (-1);
    lldp_global_msg.arp_learn_enable = arp_learnable ;

    if (0 != ifindex)
    {
       ret = lldp_arp_learn_cfg_single_if_set(ifindex , arp_learnable);
        return ret ;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
    {        
        pif = (struct l2if *)pbucket->data;
        if ((NULL == pif) || (NULL == pif->lldp_port_info))
        {
            continue;
        }

        pif->lldp_port_info->arp_nd_learning = arp_learnable ;
        pif->lldp_port_info->arp_nd_learning_vlan = lldp_global_msg.dcn_vlan ;
        ret =  lldp_arp_learn_cfg_single_if_set(pif->ifindex , arp_learnable);
    }           
    return 0 ;
}

int lldp_arp_learn_cfg_single_if_set(uint32_t ifindex , int arp_learnable)
{    
    struct lldp_port	*port = NULL;
    struct l2if			*pif = NULL;
    struct lldp_msap* 	msap_cache = NULL;

    if (lldp_global_msg.mng_addr_sub != MGMT_ADDR_SUB_IPV4)
    {
        return -1;
    }
    
    pif = l2if_get(ifindex);
    if(NULL == pif)
    {
        return -1;
    }

    port = pif->lldp_port_info;
    if(NULL == port)
    {
         return -1;
    }

    if (LLDP_FALSE == port->adminStatus)
    {
         return -1;
    }

    if(0 == port->msap_num)
    { 
        /*real config*/ 		
        SET_FLAG(port->arp_nd_learning, arp_learnable);
        port->arp_nd_learning_vlan = lldp_global_msg.dcn_vlan;
        return 0;
    }

    msap_cache = port->msap_cache;

    while(msap_cache != NULL)
    {
        if (CHECK_FLAG(port->arp_nd_learning, LLDP_LEARNING_ARP) &&
            (port->arp_nd_learning_vlan != OSPF_DCN_USE_VLAN) &&
            (LLDP_TRUE == msap_cache->arp_notice_flag))
        {
            lldp_arp_info_notice(msap_cache, port->arp_nd_learning_vlan, 
                port->if_index, LLDP_FALSE);
            msap_cache->arp_notice_flag = LLDP_FALSE;
        }

        msap_cache = msap_cache->next;
    }

    msap_cache = port->msap_cache;

    while(msap_cache != NULL)
    {        
        if ((LLDP_FALSE == msap_cache->arp_notice_flag) &&
            /*(port->arp_nd_learning_vlan == msap_cache->svlan) &&*/
            (msap_cache->neighbor_msg.mgmt_addr_sub_type == 1))
        {
            lldp_arp_info_notice(msap_cache, port->arp_nd_learning_vlan, 
                port->if_index, LLDP_TRUE); 
            msap_cache->arp_notice_flag = LLDP_TRUE;
        }

        msap_cache = msap_cache->next;
    }

    return 0;
}

int lldp_arp_learn_cfg_status_get()
{
    return lldp_global_msg.arp_learn_enable ;
}


