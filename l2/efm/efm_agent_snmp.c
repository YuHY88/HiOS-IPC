#include <string.h>
#include <stdlib.h>
#include "thread.h"

#include <lib/hash1.h>
#include <lib/command.h>
#include <lib/memtypes.h>
#include <lib/errcode.h>
#include <lib/vty.h>
#include <lib/types.h>
#include <lib/memory.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/snmp_common.h>

#include <lib/log.h>
#include <lib/linklist.h>
#include <lib/alarm.h>
#include <lib/devm_com.h>
#include <netinet/in.h>
#include <linux/netlink.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>
#include "snmp/mibgroup/mib_efm_agent.h"
#include "efm_agent.h"
#include "efm_agent_snmp.h"
#include "l2/l2_msg.h"

/*==================================================*/
/*name : efm_agent_get_comm_info                                                                       */
/*para :                                                                                                                */
/*retu :                                                                                                                */
/*desc :  efm agent get comm info                                                                          */ 
/*==================================================*/

int efm_agent_get_comm_info(struct efm_comm_info_data  * efm_comm_info,struct efm_if * pefm)
{

	efm_agent_remote_base_info * aefm_RbaseInfo = NULL;
	efm_agent_remote_state_info * aefm_RstInfo = NULL;
	efm_agent_remote_SetGet_info * aefm_RsetGetInfo = NULL;

	if( NULL == pefm->aefm->aefm_RbaseInfo || NULL == pefm->aefm->aefm_RstInfo || NULL ==  pefm->aefm->aefm_RsetGetInfo )
	{
		zlog_err("%s[%d]:leave %s:aefm  is NULL\n",__FILE__,__LINE__,__func__);
		return EFM_FAILURE;		

	}	
	
	aefm_RbaseInfo = pefm->aefm->aefm_RbaseInfo;
	aefm_RstInfo = pefm->aefm->aefm_RstInfo;
	aefm_RsetGetInfo = pefm->aefm->aefm_RsetGetInfo;
	
	switch (pefm->aefm->efm_agent_remote_device_tid)
	{
		case EFM_REMOTE_11000A_TYPE:
			efm_comm_info->efm_SubType = 3;
			break;		
		case EFM_REMOTE_1101n_TYPE:
			efm_comm_info->efm_SubType = 4;
			break;
		case EFM_REMOTE_11000An_TYPE:
			efm_comm_info->efm_SubType = 7;
			break;
		case EFM_REMOTE_123_TYPE:
			efm_comm_info->efm_SubType = 8;
			break;
		default:
			break;
	}	   

	efm_comm_info->efm_SoftVer = aefm_RbaseInfo->efm_remote_soft_ver;
	efm_comm_info->efm_EthChNum = aefm_RbaseInfo->efm_remote_eth_internal_num; 
	efm_comm_info->efm_EthPtNum = aefm_RbaseInfo->efm_remote_eth_num;
	efm_comm_info->efm_OltPtNum = aefm_RbaseInfo->efm_remote_olt_num; 


	efm_comm_info->efm_HwSw  = LM(aefm_RstInfo->efm_agent_remote_state_data[efm_remote_state_HsSfi],7);		
	efm_comm_info->efm_SFI = LM(aefm_RstInfo->efm_agent_remote_state_data[efm_remote_state_HsSfi],1);
	
	efm_comm_info->efm_DataResum = aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_Data_Resume]&0x03;
	efm_comm_info->efm_LFPCtrl = aefm_RsetGetInfo->efm_agent_remote_SetGet_data[ efm_remote_set_LFPCtrl]&0x01;
	efm_comm_info->efm_ALSCtrl = aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_ALS]&0x01;
	efm_comm_info->efm_Jumbo = aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_JumFrame]&0x01;
	efm_comm_info->efm_Reboot = aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_Reboot]&0x01;
	efm_comm_info->efm_IpCtrl = aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_IpCtrl]&0x01;
	efm_comm_info->efm_EthStromfilter = aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_EthStromfilter]&0x07;
	efm_comm_info->efm_DHCPclient = aefm_RsetGetInfo->efm_agent_remote_SetGet_data[ efm_remote_set_DHCPclient]&0x01;

	efm_comm_info->efm_DipSwitchSta = 0;
	/*0--Hw/Sf*/
	efm_comm_info->efm_DipSwitchSta |= efm_comm_info->efm_HwSw ;
	
	/*1,2---OAM mode*/
	efm_comm_info->efm_DipSwitchSta |= ((aefm_RsetGetInfo->efm_agent_remote_SetGet_data[ efm_remote_set_Oam_Mode]&0x03)<<1);

	/*3bit--lfp*/
	efm_comm_info->efm_DipSwitchSta |= (efm_comm_info->efm_LFPCtrl << 3);

	/*4bit--ETH--FAUT(auto manual)*/	
	efm_comm_info->efm_DipSwitchSta |= (LM(aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPort],3)<<4);

	/*5bit---FSPD(100M 10M)*/
	efm_comm_info->efm_DipSwitchSta |= (((aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPort]>>1)&0x03)<<5);

	/*6bit----FFDX(full half)*/
	efm_comm_info->efm_DipSwitchSta |= ((aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPort]&0x01)<<6);
	/*7bit----Olt(auto manual)*/

	efm_comm_info->efm_DipSwitchSta |=   (LM(aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_Olt1Port],3)<<7);
	if(pefm->aefm->efm_agent_hostname)
		memcpy(efm_comm_info->efm_HostName, pefm->aefm->efm_agent_hostname, strlen(pefm->aefm->efm_agent_hostname));

	EFM_LOG_DBG("%s[%d]###########   %d\n",__FUNCTION__,__LINE__,efm_comm_info->efm_DipSwitchSta);
	return EFM_SUCCESS;
}
/*==================================================*/
/*name : efm_agent_snmp_comm_info_get                                                              */
/*para :                                                                                                                */
/*retu :  data_num                                                                                                */
/*desc :efm agent get local comm info                                                                     */ 
/*==================================================*/

int efm_agent_snmp_comm_info_get(uint32_t ifindex, struct efm_comm_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_comm_info_snmp efm_comm_snmp = {0}; 
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	efm_agent_info * aefm = NULL;

	msg_num = IPC_MSG_LEN/sizeof(struct efm_comm_info_snmp);
	msg_num -=3;

	memset(&efm_comm_snmp,0,sizeof(struct efm_comm_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);


	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm)||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY) \
				||(NULL == pif->pefm->aefm))
			{
				continue;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{

				continue;
			}
			EFM_LOG_DBG("[%s][%d]data_num = %d\n",__FUNCTION__,__LINE__,data_num);

			/*need to change*/
			efm_agent_get_comm_info(&efm_comm_snmp.efm_comm_info,pif->pefm);			
			efm_comm_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++], &efm_comm_snmp, sizeof(  struct efm_comm_info_snmp ));
			memset(&efm_comm_snmp,0,sizeof(struct efm_comm_info_snmp));
			
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
				if ((NULL == pif) || (NULL == pif->pefm) ||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)
					|| (NULL == pif->pefm->aefm))
				{
					continue;
				}
				aefm = pif->pefm->aefm;
				
				if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag
					|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
				{

					continue;
				}

				efm_agent_get_comm_info(&efm_comm_snmp.efm_comm_info,pif->pefm);			
				efm_comm_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_comm_snmp, sizeof(  struct efm_comm_info_snmp ));
				memset(&efm_comm_snmp,0,sizeof(struct efm_comm_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}
/*==================================================*/
/*name : efm_agent_snmp_vlan_info_get                                                               */
/*para :                                                                                                               */
/*retu :   data_num                                                                                               */
/*desc :efm agent get local vlan info                                                                       */ 
/*==================================================*/

int efm_agent_snmp_vlan_info_get(uint32_t ifindex,struct efm_vlan_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_vlan_info_snmp efm_vlan_snmp = {0}; 
	efm_agent_remote_SetGet_info * efm_RsetGetInfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_vlan_info_snmp);
	msg_num -=3;

	memset(&efm_vlan_snmp,0,sizeof(struct efm_vlan_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm)||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
					
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				continue;
			}
			
			efm_RsetGetInfo = aefm->aefm_RsetGetInfo;
			/*need to change*/
			/*0 ,1bits is valid*/
			efm_vlan_snmp.efm_VlanMode = efm_RsetGetInfo->efm_agent_remote_SetGet_data[ efm_remote_set_VlanMode] & 0x03;
			efm_vlan_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++], &efm_vlan_snmp, sizeof(  struct efm_vlan_info_snmp ));
			memset(&efm_vlan_snmp,0,sizeof(struct efm_vlan_info_snmp));

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
				if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
					|| (NULL == pif->pefm->aefm) || (NULL == pif->pefm->aefm->aefm_RsetGetInfo))
				{
					continue;
				}
				
				aefm = pif->pefm->aefm;
			   	if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
				{

					continue;
				}
				efm_RsetGetInfo = aefm->aefm_RsetGetInfo;

				efm_vlan_snmp.efm_VlanMode = efm_RsetGetInfo->efm_agent_remote_SetGet_data[ efm_remote_set_VlanMode] & 0x03;	
				efm_vlan_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_vlan_snmp, sizeof(  struct efm_vlan_info_snmp ));
				memset(&efm_vlan_snmp,0,sizeof(struct efm_vlan_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}
int efm_agent_get_oltvlan_info(struct efm_oltvlan_info_data  * efm_oltvlan_info,efm_agent_remote_SetGet_info * efm_RsetGetInfo)
{
	//0bit is valid
	efm_oltvlan_info->efm_OltPtInMode = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_OltPtInMode]&0x01;
	//0 bit is valid
	efm_oltvlan_info->efm_OltPtTagMode = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_OltPtTagMode]&0x01;
	//0 1 2bits is valid
	efm_oltvlan_info->efm_OltPtPrior = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_OltPtPrior] & 0x07;
	//  4 bits + 8 bits
	efm_oltvlan_info->efm_OltPtPVID |= (efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_OltPtPvidH]& 0x0f)<<8;
	efm_oltvlan_info->efm_OltPtPVID |= efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_OltPtPvidL];
	return EFM_SUCCESS;
}
/*==================================================*/
/*name : efm_agent_snmp_oltvlan_info_get                                                            */
/*para :                                                                                                                */
/*retu : data_num                                                                                                 */
/*desc :efm agent get local oltvlan info                                                                    */ 
/*==================================================*/

int efm_agent_snmp_oltvlan_info_get(uint32_t ifindex,struct efm_oltvlan_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_oltvlan_info_snmp efm_oltvlan_snmp = {0}; 
	efm_agent_remote_SetGet_info * efm_RsetGetInfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_oltvlan_info_snmp);
	msg_num -=3;

	memset(&efm_oltvlan_snmp,0,sizeof(struct efm_oltvlan_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) ||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY) \
				|| (NULL == pif->pefm->aefm)||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{

				continue;
			}
			
			efm_RsetGetInfo = aefm->aefm_RsetGetInfo;
			/*need to change*/
			/*0 ,1bits is valid*/
			efm_agent_get_oltvlan_info(&efm_oltvlan_snmp.efm_oltvlan_info,efm_RsetGetInfo);
			efm_oltvlan_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++], &efm_oltvlan_snmp, sizeof(  struct efm_oltvlan_info_snmp ));
			memset(&efm_oltvlan_snmp,0,sizeof(struct efm_oltvlan_info_snmp));

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
				if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
					|| (NULL == pif->pefm->aefm) || (NULL == pif->pefm->aefm->aefm_RsetGetInfo))
				{
					continue;
				}
				
				aefm = pif->pefm->aefm;
				if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
				{

					continue;
				}
				efm_RsetGetInfo = aefm->aefm_RsetGetInfo;

				efm_agent_get_oltvlan_info(&efm_oltvlan_snmp.efm_oltvlan_info,efm_RsetGetInfo);
				efm_oltvlan_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_oltvlan_snmp, sizeof(  struct efm_oltvlan_info_snmp ));
				memset(&efm_oltvlan_snmp,0,sizeof(struct efm_oltvlan_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}
int efm_agent_get_cpuvlan_info(struct efm_cpuvlan_info_data  * efm_cpuvlan_info,efm_agent_remote_SetGet_info * efm_RsetGetInfo)
{
	//0bit is valid
	efm_cpuvlan_info->efm_CpuPtInMode = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_CpuPtInMode]&0x01;
	//0 bit is valid
	efm_cpuvlan_info->efm_CpuPtTagMode = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_CpuPtTagMode]&0x01;
	//0 1 2bits is valid
	efm_cpuvlan_info->efm_CpuPtPrior = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_CpuPtPrior] & 0x07;
	//  4 bits + 8 bits
	efm_cpuvlan_info->efm_CpuPtPVID |= (efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_CpuPtPvidH]& 0x0f)<<8;
	efm_cpuvlan_info->efm_CpuPtPVID |= efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_CpuPtPvidL];

	return EFM_SUCCESS;
}
/*==================================================*/
/*name : efm_agent_snmp_cpuvlan_info_get                                                           */
/*para :                                                                                                                */
/*retu :  data_num                                                                                                */
/*desc :efm agent get local cpuvlan info                                                                   */ 
/*==================================================*/

int efm_agent_snmp_cpuvlan_info_get(uint32_t ifindex,struct efm_cpuvlan_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_cpuvlan_info_snmp efm_cpuvlan_snmp = {0}; 
	efm_agent_remote_SetGet_info * efm_RsetGetInfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_cpuvlan_info_snmp);
	msg_num -=3;

	memset(&efm_cpuvlan_snmp,0,sizeof(struct efm_cpuvlan_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{

				continue;
			}
			
			efm_RsetGetInfo = aefm->aefm_RsetGetInfo;
			/*need to change*/
			/*0 ,1bits is valid*/
			efm_agent_get_cpuvlan_info(&efm_cpuvlan_snmp.efm_cpuvlan_info,efm_RsetGetInfo);
			efm_cpuvlan_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++], &efm_cpuvlan_snmp, sizeof(  struct efm_cpuvlan_info_snmp ));
			memset(&efm_cpuvlan_snmp,0,sizeof(struct efm_cpuvlan_info_snmp));

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
				if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY) \
					|| (NULL == pif->pefm->aefm) || (NULL == pif->pefm->aefm->aefm_RsetGetInfo))
				{
					continue;
				}
				
				aefm = pif->pefm->aefm;
				if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
				{

					continue;
				}
				efm_RsetGetInfo = aefm->aefm_RsetGetInfo;

				efm_agent_get_cpuvlan_info(&efm_cpuvlan_snmp.efm_cpuvlan_info,efm_RsetGetInfo);
				efm_cpuvlan_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_cpuvlan_snmp, sizeof(  struct efm_cpuvlan_info_snmp ));
				memset(&efm_cpuvlan_snmp,0,sizeof(struct efm_cpuvlan_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}
int efm_agent_get_qinq_info(struct efm_qinq_info_data  * efm_qinq_info,efm_agent_remote_SetGet_info * efm_RsetGetInfo)
{
	//0bit is valid
	efm_qinq_info->efm_QinQDirection = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_QinQDirection]&0x01;
	//8 bits ---high
	efm_qinq_info->efm_QinQTPID |= efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_QinQTpidH]<<8;
	//0 1 2bits is valid
	efm_qinq_info->efm_QinQTPID |= efm_RsetGetInfo->efm_agent_remote_SetGet_data[  efm_remote_set_QinQTpidL] ;
	//  4 bits + 8 bits
	efm_qinq_info->efm_QinQVid |= efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_QinQVidH]<<8;
	efm_qinq_info->efm_QinQVid |= efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_QinQVidL];
	//0 1 2bits is valid
	efm_qinq_info->efm_QinQPrior = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_QinQPiror] & 0x07;
	return EFM_SUCCESS;
}
/*==================================================*/
/*name : efm_agent_snmp_qinq_info_get                                                               */
/*para :                                                                                                                */
/*retu : data_num                                                                                                 */
/*desc :efm agent get local qinq info                                                                       */ 
/*==================================================*/

int efm_agent_snmp_qinq_info_get(uint32_t ifindex,struct efm_qinq_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_qinq_info_snmp efm_qinq_snmp = {0}; 
	efm_agent_remote_SetGet_info * efm_RsetGetInfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_qinq_info_snmp);
	msg_num -=3;

	memset(&efm_qinq_snmp,0,sizeof(struct efm_qinq_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm)||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{

				continue;
			}
			
			efm_RsetGetInfo = aefm->aefm_RsetGetInfo;
			/*need to change*/
			/*0 ,1bits is valid*/
			efm_agent_get_qinq_info(&efm_qinq_snmp.efm_qinq_info,efm_RsetGetInfo);
			efm_qinq_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++], &efm_qinq_snmp, sizeof(  struct efm_qinq_info_snmp ));
			memset(&efm_qinq_snmp,0,sizeof(struct efm_qinq_info_snmp));

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
				if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
					|| (NULL == pif->pefm->aefm) || (NULL == pif->pefm->aefm->aefm_RsetGetInfo))
				{
					continue;
				}
				
				aefm = pif->pefm->aefm;
				if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
				{

					continue;
				}
				efm_RsetGetInfo = aefm->aefm_RsetGetInfo;

				efm_agent_get_qinq_info(&efm_qinq_snmp.efm_qinq_info,efm_RsetGetInfo);
				efm_qinq_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_qinq_snmp, sizeof(  struct efm_qinq_info_snmp ));
				memset(&efm_qinq_snmp,0,sizeof(struct efm_qinq_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}

/*==================================================*/
/*name : efm_agent_snmp_qos_info_get                                                                 */
/*para :                                                                                                                */
/*retu :  data_num                                                                                                */
/*desc :efm agent get local qos info                                                                         */ 
/*==================================================*/
int efm_agent_snmp_qos_info_get(uint32_t ifindex,struct efm_qos_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_qos_info_snmp efm_qos_snmp = {0}; 
	efm_agent_remote_SetGet_info * efm_RsetGetInfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	efm_agent_info * aefm = NULL;

	msg_num = IPC_MSG_LEN/sizeof(struct efm_qos_info_snmp);
	msg_num -=3;

	memset(&efm_qos_snmp,0,sizeof(struct efm_qos_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm)||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
					
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{

				continue;
			}
			efm_RsetGetInfo = aefm->aefm_RsetGetInfo;
			/*need to change*/
			/*0 ,1bits is valid*/

			efm_qos_snmp.ifindex = pif->ifindex;
			efm_qos_snmp.efm_1pQos = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_1pQos]&0x01;
			memcpy(&efm_snmp_mux[data_num++], &efm_qos_snmp, sizeof(  struct efm_qos_info_snmp ));
			memset(&efm_qos_snmp,0,sizeof(struct efm_qos_info_snmp));

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
				if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
					|| (NULL == pif->pefm->aefm) || (NULL == pif->pefm->aefm->aefm_RsetGetInfo))
				{
					continue;
				}
								
				aefm = pif->pefm->aefm;
				if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
				{

					continue;
				}
				efm_RsetGetInfo = aefm->aefm_RsetGetInfo;

				efm_qos_snmp.ifindex = pif->ifindex;
				//0bit is valid
				efm_qos_snmp.efm_1pQos = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_1pQos]&0x01;
				memcpy(&efm_snmp_mux[data_num++], &efm_qos_snmp, sizeof(  struct efm_qos_info_snmp ));
				memset(&efm_qos_snmp,0,sizeof(struct efm_qos_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}

/*==================================================*/
/*name : efm_agent_snmp_perf_info_get                                                                */
/*para :                                                                                                                */
/*retu :  data_num                                                                                                */
/*desc :efm agent get local perf info                                                                       */ 
/*==================================================*/
int efm_agent_snmp_perf_info_get(uint32_t ifindex,struct efm_perf_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_perf_info_snmp efm_perf_snmp = {0}; 
	efm_agent_remote_SetGet_info * efm_RsetGetInfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_perf_info_snmp);
	msg_num -=3;

	memset(&efm_perf_snmp,0,sizeof(struct efm_perf_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm)||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
					
			aefm = pif->pefm->aefm;		
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{

				continue;
			}
			
			efm_RsetGetInfo = aefm->aefm_RsetGetInfo;
			/*need to change*/
			/*0 ,1bits is valid*/

			efm_perf_snmp.ifindex = pif->ifindex;
			//0 bit is valid
			efm_perf_snmp.efm_CleanCounter = \
							 efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_CleanCounter]&0x01;
			memcpy(&efm_snmp_mux[data_num++], &efm_perf_snmp, sizeof(  struct efm_perf_info_snmp ));
			memset(&efm_perf_snmp,0,sizeof(struct efm_perf_info_snmp));

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
				if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
					|| (NULL == pif->pefm->aefm) || (NULL == pif->pefm->aefm->aefm_RsetGetInfo))
				{
					continue;
				}
							
				aefm = pif->pefm->aefm;
				if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
				{

					continue;
				}	
				
				efm_RsetGetInfo = aefm->aefm_RsetGetInfo;
				efm_perf_snmp.ifindex = pif->ifindex;
				//0bit is valid
				efm_perf_snmp.efm_CleanCounter = \
								 efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_CleanCounter]&0x01;
				memcpy(&efm_snmp_mux[data_num++], &efm_perf_snmp, sizeof(  struct efm_perf_info_snmp ));
				memset(&efm_perf_snmp,0,sizeof(struct efm_perf_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}
int efm_agent_get_oltperf_info(struct efm_oltperf_info_data  * efm_oltperf_info,efm_agent_info * aefm)
{
	int i = 0;

	efm_agent_remote_state_info * aefm_RstInfo = NULL;
	efm_agent_remote_SetGet_info * aefm_RsetGetInfo = NULL;

	aefm_RstInfo = aefm->aefm_RstInfo;
	aefm_RsetGetInfo = aefm->aefm_RsetGetInfo;

	efm_oltperf_info->efm_OltCntType = aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_OltCounterType];
	//no this value
	for(i = 0;i< BITS_NUM;i++)
	{
		efm_oltperf_info->efm_OltRxBytes[i] = aefm_RstInfo->efm_agent_remote_state_data[efm_remote_Olt_Bytes_Counter8+i];
	}
	efm_oltperf_info->efm_OltRxPCnt |= aefm_RstInfo->efm_agent_remote_state_data[ efm_remote_Olt_Counter4]<<24;
	efm_oltperf_info->efm_OltRxPCnt |= aefm_RstInfo->efm_agent_remote_state_data[ efm_remote_Olt_Counter3]<<16;
	efm_oltperf_info->efm_OltRxPCnt |= aefm_RstInfo->efm_agent_remote_state_data[ efm_remote_Olt_Counter2]<<8;
	efm_oltperf_info->efm_OltRxPCnt |= aefm_RstInfo->efm_agent_remote_state_data[ efm_remote_Olt_Counter1];

	efm_oltperf_info->efm_OltTxPCnt = 0;

	return EFM_SUCCESS;
}
/*==================================================*/
/*name : efm_agent_snmp_oltperf_info_get                                                              */
/*para :                                                                                                                 */
/*retu :  data_num                                                                                                  */
/*desc :efm agent get local oltperf info                                                                     */ 
/*==================================================*/

int efm_agent_snmp_oltperf_info_get(uint32_t ifindex,struct efm_oltperf_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_oltperf_info_snmp efm_oltperf_snmp = {0}; 
	efm_agent_info * aefm = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;


	msg_num = IPC_MSG_LEN/sizeof(struct efm_oltperf_info_snmp);
	msg_num -=3;

	memset(&efm_oltperf_snmp,0,sizeof(struct efm_oltperf_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm)||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
				
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{

				continue;
			}
			/*need to change*/
			/*0 ,1bits is valid*/
			efm_agent_get_oltperf_info(&efm_oltperf_snmp.efm_oltperf_info,aefm);
			efm_oltperf_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++], &efm_oltperf_snmp, sizeof(  struct efm_oltperf_info_snmp ));
			memset(&efm_oltperf_snmp,0,sizeof(struct efm_oltperf_info_snmp));

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
				if ((NULL == pif) || (NULL == pif->pefm)  || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
					||(NULL == pif->pefm->aefm))
				{
					continue;
				}
	
				aefm = pif->pefm->aefm;
				if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
				{

					continue;
				}

				efm_agent_get_oltperf_info(&efm_oltperf_snmp.efm_oltperf_info,aefm);
				efm_oltperf_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_oltperf_snmp, sizeof(  struct efm_oltperf_info_snmp ));
				memset(&efm_oltperf_snmp,0,sizeof(struct efm_oltperf_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}
int efm_agent_get_cpuperf_info(struct efm_cpuperf_info_data  * efm_cpuperf_info,efm_agent_info * aefm)
{
	int i = 0;

	efm_agent_remote_state_info * aefm_RstInfo = NULL;
	efm_agent_remote_SetGet_info * aefm_RsetGetInfo = NULL;

	aefm_RstInfo = aefm->aefm_RstInfo;
	aefm_RsetGetInfo = aefm->aefm_RsetGetInfo;

	efm_cpuperf_info->efm_CpuCntType = aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_CpuCounterType];
	//no this value
	for(i=0;i<BITS_NUM;i++)
	{
		efm_cpuperf_info->efm_CpuRxBytes[i] =  aefm_RstInfo->efm_agent_remote_state_data[ efm_remote_Cpu_Bytes_Counter8+i];

	}
	efm_cpuperf_info->efm_CpuRxPCnt |= aefm_RstInfo->efm_agent_remote_state_data[ efm_remote_Cpu_Counter4]<<24;
	efm_cpuperf_info->efm_CpuRxPCnt |= aefm_RstInfo->efm_agent_remote_state_data[ efm_remote_Cpu_Counter3]<<16;
	efm_cpuperf_info->efm_CpuRxPCnt |= aefm_RstInfo->efm_agent_remote_state_data[ efm_remote_Cpu_Counter2]<<8;
	efm_cpuperf_info->efm_CpuRxPCnt |= aefm_RstInfo->efm_agent_remote_state_data[ efm_remote_Cpu_Counter1];

	efm_cpuperf_info->efm_CpuTxPCnt = 0;

	return EFM_SUCCESS;
}
/*==================================================*/
/*name : efm_agent_snmp_cpuperf_info_get                                                            */
/*para :                                                                                                                */
/*retu :  data_num                                                                                                 */
/*desc :efm agent get local cpuperf info                                                                    */ 
/*==================================================*/

int efm_agent_snmp_cpuperf_info_get(uint32_t ifindex,struct efm_cpuperf_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_cpuperf_info_snmp efm_cpuperf_snmp = {0}; 
	efm_agent_info * aefm = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;


	msg_num = IPC_MSG_LEN/sizeof(struct efm_cpuperf_info_snmp);
	msg_num -=3;

	memset(&efm_cpuperf_snmp,0,sizeof(struct efm_cpuperf_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm))
			{
				continue;
			}
				
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{

				continue;
			}
			
			/*need to change*/
			/*0 ,1bits is valid*/
			efm_agent_get_cpuperf_info(&efm_cpuperf_snmp.efm_cpuperf_info,aefm);
			efm_cpuperf_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++], &efm_cpuperf_snmp, sizeof(  struct efm_cpuperf_info_snmp ));
			memset(&efm_cpuperf_snmp,0,sizeof(struct efm_cpuperf_info_snmp));

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
				if ((NULL == pif) || (NULL == pif->pefm) ||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
					||(NULL == pif->pefm->aefm))
				{
					continue;
				}
	
				aefm = pif->pefm->aefm;
				if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
				{

					continue;
				}

				efm_agent_get_cpuperf_info(&efm_cpuperf_snmp.efm_cpuperf_info,aefm);
				efm_cpuperf_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_cpuperf_snmp, sizeof(  struct efm_cpuperf_info_snmp ));
				memset(&efm_cpuperf_snmp,0,sizeof(struct efm_cpuperf_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}
int efm_agent_get_oam_info(struct efm_oam_info_data  * efm_oam_info,efm_agent_remote_SetGet_info * efm_RsetGetInfo,efm_agent_remote_state_info * efm_RstInfo)
{
	//0 1bits is valid
	efm_oam_info->efm_OamMode = efm_RsetGetInfo->efm_agent_remote_SetGet_data[ efm_remote_set_Oam_Mode]&0x03;
	//0 bit ---enable or disable loopback 1 bit---macswap
	//efm_oam_info->efm_OamLpbkSta = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_OamLoopb_Machange]&0x03;
	//6 bit---loopback state
	efm_oam_info->efm_OamLpbkSta = LM(efm_RstInfo->efm_agent_remote_state_data[efm_remote_state_PowOlos],6);
	return EFM_SUCCESS;
}

/*==================================================*/
/*name : efm_agent_snmp_oam_info_get                                                               */
/*para :                                                                                                                */
/*retu :  data_num                                                                                                */
/*desc :efm agent get local oam info                                                                       */ 
/*==================================================*/
int efm_agent_snmp_oam_info_get(uint32_t ifindex,struct efm_oam_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_oam_info_snmp efm_oam_snmp = {0}; 
	efm_agent_remote_SetGet_info * efm_RsetGetInfo = NULL;
	efm_agent_remote_state_info * efm_RstInfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_oam_info_snmp);
	msg_num -=3;

	memset(&efm_oam_snmp,0,sizeof(struct efm_oam_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY) ||(NULL == pif->pefm->aefm)\
					||(NULL == pif->pefm->aefm->aefm_RsetGetInfo) || (NULL == pif->pefm->aefm->aefm_RstInfo))
			{
				continue;
			}
	
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				continue;
			}
			
			efm_RsetGetInfo = aefm->aefm_RsetGetInfo;
			efm_RstInfo = aefm->aefm_RstInfo;
			/*need to change*/
			/*0 ,1bits is valid*/

			efm_agent_get_oam_info(&efm_oam_snmp.efm_oam_info,efm_RsetGetInfo,efm_RstInfo);
			efm_oam_snmp.ifindex = pif->ifindex;		
			memcpy(&efm_snmp_mux[data_num++], &efm_oam_snmp, sizeof(  struct efm_oam_info_snmp ));
			memset(&efm_oam_snmp,0,sizeof(struct efm_oam_info_snmp));

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
				if ((NULL == pif) || (NULL == pif->pefm) ||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY) || (NULL == pif->pefm->aefm)\
						|| (NULL == pif->pefm->aefm->aefm_RsetGetInfo) ||(NULL == pif->pefm->aefm->aefm_RstInfo))
				{
					continue;
				}
				
				aefm = pif->pefm->aefm;
				if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
				{

					continue;
				}
				
				efm_RsetGetInfo = aefm->aefm_RsetGetInfo;
				efm_RstInfo = aefm->aefm_RstInfo;
				efm_agent_get_oam_info(&efm_oam_snmp.efm_oam_info,efm_RsetGetInfo,efm_RstInfo);
				efm_oam_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_oam_snmp, sizeof(  struct efm_oam_info_snmp ));
				memset(&efm_oam_snmp,0,sizeof(struct efm_oam_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}
int efm_agent_get_addr_info(struct efm_addr_info_data  * efm_addr_info,efm_agent_remote_SetGet_info * efm_RsetGetInfo)
{
	int i = 0;

	for(i =0;i<IP_NUM;i++)
	{

		efm_addr_info->efm_IpAddr[i] = efm_RsetGetInfo->efm_agent_remote_SetGet_data[ efm_remote_set_4IpAddr+i];

	}
	// Total 32 bits
	for(i =0;i<IP_NUM;i++)
	{
		efm_addr_info->efm_IpMask[i] = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_4IpMask+i];
	}

	//Total 32 bits
	for(i =0;i<IP_NUM;i++)
	{
		efm_addr_info->efm_GatewayAddr[i] = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_4GatewayAddr+i];
	}

	//Total 48 bits
	for(i = 0;i < MAC_NUM;i++)
	{
		efm_addr_info->efm_MacAddr[i] = efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_6hpmcrtMacAddr+i];
	}

	return EFM_SUCCESS;
}
/*==================================================*/
/*name : efm_agent_snmp_addr_info_get                                                               */
/*para :                                                                                                                */
/*retu :   data_num                                                                                               */
/*desc :efm agent get local addr info                                                                       */ 
/*==================================================*/

int efm_agent_snmp_addr_info_get(uint32_t ifindex,struct efm_addr_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_addr_info_snmp efm_addr_snmp = {0}; 
	efm_agent_remote_SetGet_info * efm_RsetGetInfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_addr_info_snmp);
	msg_num -=3;

	memset(&efm_addr_snmp,0,sizeof(struct efm_addr_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm)||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
		
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				continue;
			}
			
			efm_RsetGetInfo = aefm->aefm_RsetGetInfo;
			/*need to change*/
			/*0 ,1bits is valid*/

			efm_agent_get_addr_info(&efm_addr_snmp.efm_addr_info,efm_RsetGetInfo);
			efm_addr_snmp.ifindex = pif->ifindex;		
			memcpy(&efm_snmp_mux[data_num++], &efm_addr_snmp, sizeof(  struct efm_addr_info_snmp ));
			memset(&efm_addr_snmp,0,sizeof(struct efm_addr_info_snmp));

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
				if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
					|| (NULL == pif->pefm->aefm) || (NULL == pif->pefm->aefm->aefm_RsetGetInfo))
				{
					continue;
				}
					
				aefm = pif->pefm->aefm;
				if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
				{

					continue;
				}
				
				efm_RsetGetInfo = aefm->aefm_RsetGetInfo;

				efm_agent_get_addr_info(&efm_addr_snmp.efm_addr_info,efm_RsetGetInfo);
				efm_addr_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_addr_snmp, sizeof(  struct efm_addr_info_snmp ));
				memset(&efm_addr_snmp,0,sizeof(struct efm_addr_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}
/*==================================================*/
/*name : efm_agent_snmp_conver_info_get                                                             */
/*para :                                                                                                                */
/*retu :  data_num                                                                                                */
/*desc :efm agent get local conver info                                                                    */ 
/*==================================================*/

int efm_agent_snmp_conver_info_get(uint32_t ifindex,struct efm_conver_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_conver_info_snmp efm_conver_snmp = {0}; 
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;

	msg_num = IPC_MSG_LEN/sizeof(struct efm_conver_info_snmp);
	msg_num -=3;

	memset(&efm_conver_snmp,0,sizeof(struct efm_conver_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if (NULL == pif  )
			{
				continue;
			}
			/*need to change*/
			/*0 ,1bits is valid*/

			efm_conver_snmp.efm_conver = (NULL == pif->pefm  || NULL == pif->pefm->aefm? EFM_FALSE : EFM_TRUE);
			
			efm_conver_snmp.ifindex = pif->ifindex;		
			memcpy(&efm_snmp_mux[data_num++], &efm_conver_snmp, sizeof(  struct efm_conver_info_snmp ));
			memset(&efm_conver_snmp,0,sizeof(struct efm_conver_info_snmp));

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
				if (NULL == pif )
				{
					continue;
				}
				
				efm_conver_snmp.efm_conver = (NULL == pif->pefm  || NULL == pif->pefm->aefm? EFM_FALSE : EFM_TRUE);
			
				efm_conver_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_conver_snmp, sizeof(  struct efm_conver_info_snmp ));
				memset(&efm_conver_snmp,0,sizeof(struct efm_conver_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}

/*==================================================*/
/*name : snmp_msg_rcv_efm_agent                                                                       */
/*para :                                                                                                                */
/*retu : NULL                                                                                                        */
/*desc :efm agent receive inquire form snmp one ifindex                                           */ 
/*==================================================*/

void snmp_msg_rcv_efm_agent(struct ipc_msghdr_n * phdr)
{
	
	int msg_num = 0;
	uint32_t ifindex = 0;

	if(NULL == phdr)
	{
		zlog_err("%s[%d]####### phdr is NULL",__FUNCTION__,__LINE__);
		return ;
	}
	
	EFM_LOG_DBG("%s[%d]##############snmp get efm agent info",__FUNCTION__,__LINE__);
	
	ifindex = phdr->msg_index;
	EFM_LOG_DBG("[%s][%d]:msg_ifindex = %d\n",__FUNCTION__,__LINE__,ifindex);

	switch(phdr->data_num)
	{

		case EFM_COMM_INFO_TYPE:
			{
				
				msg_num = IPC_MSG_LEN/sizeof(struct efm_comm_info_snmp);
				struct efm_comm_info_snmp efm_snmp[msg_num];
				memset(efm_snmp, 0, msg_num*sizeof(struct efm_comm_info_snmp));
				msg_num = efm_agent_snmp_comm_info_get(ifindex, efm_snmp);	 
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_COMM_INFO_TYPE);
			}

			break;

		case EFM_VLAN_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_vlan_info_snmp);
				struct efm_vlan_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_vlan_info_snmp));
				msg_num  = efm_agent_snmp_vlan_info_get(ifindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_VLAN_INFO_TYPE);
			}
			break;

		case EFM_OLTVLAN_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_oltvlan_info_snmp);
				struct efm_oltvlan_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_oltvlan_info_snmp));
				msg_num  = efm_agent_snmp_oltvlan_info_get(ifindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_OLTVLAN_INFO_TYPE);

			}
			break;

		case EFM_CPUVLAN_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_cpuvlan_info_snmp);
				struct efm_cpuvlan_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_cpuvlan_info_snmp));
				msg_num  = efm_agent_snmp_cpuvlan_info_get(ifindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_CPUVLAN_INFO_TYPE);
			}
			break;
		case EFM_QINQ_INFO_TYPE:
			{

				msg_num = IPC_MSG_LEN/sizeof(struct efm_qinq_info_snmp);
				struct efm_qinq_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_qinq_info_snmp));
				msg_num  = efm_agent_snmp_qinq_info_get(ifindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_QINQ_INFO_TYPE);

			}
			break;
		case EFM_QOS_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_qos_info_snmp);
				struct efm_qos_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_qos_info_snmp));
				msg_num  = efm_agent_snmp_qos_info_get(ifindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_QOS_INFO_TYPE);


			}
			break;
		case EFM_PERF_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_perf_info_snmp);
				struct efm_perf_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_perf_info_snmp));
				msg_num  = efm_agent_snmp_perf_info_get(ifindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_PERF_INFO_TYPE);			

			}
			break;
		case EFM_OLTPERF_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_oltperf_info_snmp);
				struct efm_oltperf_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_oltperf_info_snmp));
				msg_num  = efm_agent_snmp_oltperf_info_get(ifindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_OLTPERF_INFO_TYPE);

			}
			break;

		case EFM_CPUPERF_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_cpuperf_info_snmp);
				struct efm_cpuperf_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_cpuperf_info_snmp));
				msg_num  = efm_agent_snmp_cpuperf_info_get(ifindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_CPUPERF_INFO_TYPE);

			}
			break;
		case EFM_OAM_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_oam_info_snmp);
				struct efm_oam_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_oam_info_snmp));
				msg_num  = efm_agent_snmp_oam_info_get(ifindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_OAM_INFO_TYPE);

			}
			break;
		case EFM_ADDR_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_addr_info_snmp);
				struct efm_addr_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_addr_info_snmp));
				msg_num  = efm_agent_snmp_addr_info_get(ifindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_ADDR_INFO_TYPE);

			}
			break;
		
		case EFM_CONVER_INFO_TYPE:
			{	
				msg_num = IPC_MSG_LEN/sizeof(struct efm_conver_info_snmp);
				struct efm_conver_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_conver_info_snmp));
				msg_num  = efm_agent_snmp_conver_info_get(ifindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_CONVER_INFO_TYPE);
			}
			break;
		default:
			msg_num  = 0;
			efm_agent_send_data_to_snmp(phdr,NULL,msg_num,0,0);
			break;

	}

	return ;
}
void efm_agent_get_remote_refresh_oltptinfo(efm_agent_remote_SetGet_info * efm_RsetGetinfo)
{
	int i = 0;
	for(i = 0;i<PORT_NUM;i++)
	{
		efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_Oltifindex = i+1;
		//3bit---FAUT
		efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtNegCtrl =\
									     LM(efm_RsetGetinfo->efm_agent_remote_SetGet_data[ efm_remote_set_Olt1Port],3);
		//12bits---FSPD
		efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtSpdCtrl = \
									      (efm_RsetGetinfo->efm_agent_remote_SetGet_data[ efm_remote_set_Olt1Port]>>1)&0x03;
		//0bit----FFDx  olt only support full-duplex
		efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtDplCtrl =\
									     efm_RsetGetinfo->efm_agent_remote_SetGet_data[ efm_remote_set_Olt1Port] & 0x01;
		//7bit----Dis
		efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtDisCtrl =\
									     LM(efm_RsetGetinfo->efm_agent_remote_SetGet_data[ efm_remote_set_Olt1Port],7);		
		//0bit-----flow
		efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtFlowCtrl =\
									      efm_RsetGetinfo->efm_agent_remote_SetGet_data[efm_remote_set_Olt1Flow]&0x01;
		//high 8bits
		efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtIngressRateN |= \
										    (((uint16_t)efm_RsetGetinfo->efm_agent_remote_SetGet_data[efm_remote_set_OltPtIngressRateH])<<8);

		//low 8bits
		efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtIngressRateN |=\
										   efm_RsetGetinfo->efm_agent_remote_SetGet_data[efm_remote_set_OltPtIngressRateL];

		EFM_LOG_DBG("%s[%d]###################%d %02x\n",__FUNCTION__,__LINE__,\
				efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtIngressRateN,\
				efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtIngressRateN);

		//high 8bits
		efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtEgressRateN |= \
										   (((uint16_t)efm_RsetGetinfo->efm_agent_remote_SetGet_data[efm_remote_set_OltPtEgressRateH])<<8);
		//low 8bits
		efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtEgressRateN |= \
										   efm_RsetGetinfo->efm_agent_remote_SetGet_data[efm_remote_set_OltPtEgressRateL];


		EFM_LOG_DBG("%s[%d]###################%d %02x\n",__FUNCTION__,__LINE__,\
				efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtEgressRateN,\
				efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_OltPtEgressRateN);
	}
	
	return ;
}

int efm_agent_get_oltpt_info(struct efm_oltpt_info_data  * efm_oltpt_info,efm_oltpt_info_log efm_oltpt_log_data)
{


	efm_oltpt_info->efm_Oltifindex = efm_oltpt_log_data.efm_Log_Oltifindex;
	efm_oltpt_info->efm_OltPtDisCtrl = efm_oltpt_log_data.efm_Log_OltPtDisCtrl;
	efm_oltpt_info->efm_OltPtDplCtrl = efm_oltpt_log_data.efm_Log_OltPtDplCtrl;
	efm_oltpt_info->efm_OltPtEgressRateN = efm_oltpt_log_data.efm_Log_OltPtEgressRateN;
	efm_oltpt_info->efm_OltPtFlowCtrl =  efm_oltpt_log_data.efm_Log_OltPtFlowCtrl;
	efm_oltpt_info->efm_OltPtIngressRateN =  efm_oltpt_log_data.efm_Log_OltPtIngressRateN;
	efm_oltpt_info->efm_OltPtNegCtrl = efm_oltpt_log_data.efm_Log_OltPtNegCtrl;
	efm_oltpt_info->efm_OltPtSpdCtrl = efm_oltpt_log_data.efm_Log_OltPtSpdCtrl;


	return EFM_SUCCESS;
}
/*==================================================*/
/*name : efm_agent_snmp_oltpt_info_get                                                               */
/*para :                                                                                                                */
/*retu : data_num                                                                                                 */
/*desc :efm agent get local oltpt info                                                                       */ 
/*==================================================*/

int efm_agent_snmp_oltpt_info_get(uint32_t ifindex, uint32_t SecIfindex,struct efm_oltpt_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_oltpt_info_snmp efm_oltpt_snmp = {0}; 
	efm_agent_remote_SetGet_info * efm_RsetGetinfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	int ifindex_flag = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_oltpt_info_snmp);
	msg_num -=3;

	memset(&efm_oltpt_snmp,0,sizeof(struct efm_oltpt_info_snmp));

	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);


	if ((0 == ifindex)&& (0 == SecIfindex))						/* first get data from efm */
	{

		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm)||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY) \
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				continue;
			}
			efm_RsetGetinfo = aefm->aefm_RsetGetInfo;

			/*need to clear original data*/
			memset(efm_RsetGetinfo->efm_oltpt_info_two,0,sizeof(efm_oltpt_info_log)*2);
			efm_agent_get_remote_refresh_oltptinfo( efm_RsetGetinfo);

			for(i = 0;i< PORT_NUM;i++)
			{

				efm_agent_get_oltpt_info(&efm_oltpt_snmp.efm_oltpt_info,efm_RsetGetinfo->efm_oltpt_info_two[i]);
				efm_oltpt_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++],&efm_oltpt_snmp,sizeof(struct efm_oltpt_info_snmp));
				memset(&efm_oltpt_snmp,0,sizeof(struct efm_oltpt_info_snmp));

				if (data_num == msg_num)
				{
					return data_num;
				}
			}

		}


	}


	else
	{		
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)	/* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
		{
			/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])	/* ~{51G09~O#M0A41m2;N*?U~} */
			{
				pbucket = l2if_table.buckets[val];
			}
			else										/* ~{51G09~O#M0A41mN*?U~} */
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


		while (pbucket)
		{

			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm)||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY) \
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				goto loop;
			}
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{

				goto loop;
			}
			efm_RsetGetinfo = aefm->aefm_RsetGetInfo;

			/*need to clear original data*/
			memset(efm_RsetGetinfo->efm_oltpt_info_two,0,sizeof(efm_oltpt_info_log)*2);
			efm_agent_get_remote_refresh_oltptinfo( efm_RsetGetinfo);
			for(i = 0;i< PORT_NUM;i++)
			{
				if((efm_RsetGetinfo->efm_oltpt_info_two[i].efm_Log_Oltifindex > SecIfindex) && !ifindex_flag)
				{


					efm_agent_get_oltpt_info(&efm_oltpt_snmp.efm_oltpt_info,efm_RsetGetinfo->efm_oltpt_info_two[i]);
					efm_oltpt_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_oltpt_snmp,sizeof(struct efm_oltpt_info_snmp));
					memset(&efm_oltpt_snmp,0,sizeof(struct efm_oltpt_info_snmp));

					if(data_num == msg_num)
					{
						return data_num;

					}

				}


				if(ifindex_flag)
				{

					efm_agent_get_oltpt_info(&efm_oltpt_snmp.efm_oltpt_info,efm_RsetGetinfo->efm_oltpt_info_two[i]);
					efm_oltpt_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_oltpt_snmp,sizeof(struct efm_oltpt_info_snmp));
					memset(&efm_oltpt_snmp,0,sizeof(struct efm_oltpt_info_snmp));

					if (data_num == msg_num)
					{

						return data_num;
					}
				}

			}
loop:
			pnext = hios_hash_next_cursor(&l2if_table, pbucket);
			if ((NULL == pnext) || (NULL == pnext->data))
			{
				break;
			}

			pbucket = pnext;
			ifindex_flag = EFM_TRUE;
		}

}
return data_num;
}
void efm_agent_get_remote_refresh_ethptinfo(efm_agent_remote_SetGet_info * efm_RsetGetinfo)
{
	int i = 0;
	for(i = 0;i< PORT_NUM;i++)
	{
		efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_Ethifindex = i+1;
		//3bit---FAUT
		efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtNegCtrl =\
									     LM(efm_RsetGetinfo->efm_agent_remote_SetGet_data[ efm_remote_set_EthPort],3);
		//12bits---FSPD
		efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtSpdCtrl = \
									      (efm_RsetGetinfo->efm_agent_remote_SetGet_data[ efm_remote_set_EthPort]>>1)&0x03;
		//0bit----FFDx  eth only support full-duplex
		efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtDplCtrl =\
									     efm_RsetGetinfo->efm_agent_remote_SetGet_data[ efm_remote_set_EthPort] & 0x01;
		//7bit----Dis
		efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtDisCtrl =\
									     LM(efm_RsetGetinfo->efm_agent_remote_SetGet_data[ efm_remote_set_EthPort],7);	
		EFM_LOG_DBG("%s[%d]###################   %d  %d\n",__FUNCTION__,__LINE__,\
				efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtDisCtrl,efm_RsetGetinfo->efm_agent_remote_SetGet_data[ efm_remote_set_EthPort]);
		//0bit-----flow
		efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtFlowCtrl =\
									      efm_RsetGetinfo->efm_agent_remote_SetGet_data[ efm_remote_set_EthFlow]&0x01;
		//high 8bits
		efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtIngressRateN |= \
										    (((uint16_t)efm_RsetGetinfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPtIngressRateH])<<8);

		//low 8bits
		efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtIngressRateN |=\
										   efm_RsetGetinfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPtIngressRateL];


		EFM_LOG_DBG("%s[%d]#####################    %d %02x \n",__FUNCTION__,__LINE__,\
				efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtIngressRateN,\
				efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtIngressRateN);
		//high 8bits
		efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtEgressRateN |= \
										   (((uint16_t)efm_RsetGetinfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPtEgressRateH])<<8);
		//low 8bits
		efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtEgressRateN |= \
										   efm_RsetGetinfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPtEgressRateL];


		EFM_LOG_DBG("%s[%d]#####################    %d %02x \n",__FUNCTION__,__LINE__,\
				efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtEgressRateN,\
				efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_EthPtEgressRateN);
	}
	return ;
}

int efm_agent_get_ethpt_info(struct efm_ethpt_info_data  * efm_ethpt_info,efm_ethpt_info_log efm_ethpt_log_data)
{


	efm_ethpt_info->efm_Ethifindex = efm_ethpt_log_data.efm_Log_Ethifindex;
	efm_ethpt_info->efm_EthPtDisCtrl = efm_ethpt_log_data.efm_Log_EthPtDisCtrl;
	efm_ethpt_info->efm_EthPtDplCtrl = efm_ethpt_log_data.efm_Log_EthPtDplCtrl;
	efm_ethpt_info->efm_EthPtEgressRateN = efm_ethpt_log_data.efm_Log_EthPtEgressRateN;
	efm_ethpt_info->efm_EthPtFlowCtrl =  efm_ethpt_log_data.efm_Log_EthPtFlowCtrl;
	efm_ethpt_info->efm_EthPtIngressRateN =  efm_ethpt_log_data.efm_Log_EthPtIngressRateN;
	efm_ethpt_info->efm_EthPtNegCtrl = efm_ethpt_log_data.efm_Log_EthPtNegCtrl;
	efm_ethpt_info->efm_EthPtSpdCtrl = efm_ethpt_log_data.efm_Log_EthPtSpdCtrl;


	return EFM_SUCCESS;
}
/*==================================================*/
/*name : efm_agent_snmp_ethpt_info_get                                                               */
/*para :                                                                                                                */
/*retu :   data_num                                                                                                             */
/*desc :efm agent get local ethpt info                                                                       */ 
/*==================================================*/

int efm_agent_snmp_ethpt_info_get(uint32_t ifindex, uint32_t SecIfindex,struct efm_ethpt_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_ethpt_info_snmp efm_ethpt_snmp = {0}; 
	efm_agent_remote_SetGet_info * efm_RsetGetinfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	int ifindex_flag = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_ethpt_info_snmp);
	msg_num -=3;

	memset(&efm_ethpt_snmp,0,sizeof(struct efm_ethpt_info_snmp));

	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);


	if ((0 == ifindex)&& (0 == SecIfindex))						/* first get data from efm */
	{

		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm)||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{

				continue;
			}
			efm_RsetGetinfo = aefm->aefm_RsetGetInfo;

			/*need to clear original data*/
			memset(efm_RsetGetinfo->efm_ethpt_info_two,0,sizeof(efm_ethpt_info_log)*2);
			efm_agent_get_remote_refresh_ethptinfo( efm_RsetGetinfo);
			for(i = 0;i< PORT_NUM;i++)
			{

				efm_agent_get_ethpt_info(&efm_ethpt_snmp.efm_ethpt_info,efm_RsetGetinfo->efm_ethpt_info_two[i]);
				efm_ethpt_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++],&efm_ethpt_snmp,sizeof(struct efm_ethpt_info_snmp));
				memset(&efm_ethpt_snmp,0,sizeof(struct efm_ethpt_info_snmp));

				if (data_num == msg_num)
				{
					return data_num;
				}
			}

		}


	}


	else
	{		
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)	/* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
		{
			/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])	/* ~{51G09~O#M0A41m2;N*?U~} */
			{
				pbucket = l2if_table.buckets[val];
			}
			else										/* ~{51G09~O#M0A41mN*?U~} */
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


		while (pbucket)
		{

			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) ||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				goto loop;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				goto loop;
			}
			efm_RsetGetinfo = aefm->aefm_RsetGetInfo;

			/*need to clear original data*/
			memset(efm_RsetGetinfo->efm_ethpt_info_two,0,sizeof(efm_ethpt_info_log)*2);
			efm_agent_get_remote_refresh_ethptinfo( efm_RsetGetinfo);
			for(i = 0;i< PORT_NUM;i++)
			{
				if((efm_RsetGetinfo->efm_ethpt_info_two[i].efm_Log_Ethifindex > SecIfindex) && !ifindex_flag)
				{


					efm_agent_get_ethpt_info(&efm_ethpt_snmp.efm_ethpt_info,efm_RsetGetinfo->efm_ethpt_info_two[i]);
					efm_ethpt_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_ethpt_snmp,sizeof(struct efm_ethpt_info_snmp));
					memset(&efm_ethpt_snmp,0,sizeof(struct efm_ethpt_info_snmp));

					if(data_num == msg_num)
					{
						return data_num;

					}

				}


				if(ifindex_flag)
				{

					efm_agent_get_ethpt_info(&efm_ethpt_snmp.efm_ethpt_info,efm_RsetGetinfo->efm_ethpt_info_two[i]);
					efm_ethpt_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_ethpt_snmp,sizeof(struct efm_ethpt_info_snmp));
					memset(&efm_ethpt_snmp,0,sizeof(struct efm_ethpt_info_snmp));

					if (data_num == msg_num)
					{

						return data_num;
					}
				}

			}
loop:
			pnext = hios_hash_next_cursor(&l2if_table, pbucket);
			if ((NULL == pnext) || (NULL == pnext->data))
			{
				break;
			}

			pbucket = pnext;
			ifindex_flag = EFM_TRUE;
		}

}
return data_num;
}
void efm_agent_get_remote_refresh_vlangroupinfo(efm_agent_remote_SetGet_info * efm_RsetGetinfo)
{
	int i  = 0;
	for(i = 0;i<EFM_AGENT_CONFIG_VLAN_GROUP;i++ )
	{
		efm_RsetGetinfo->efm_vlangroup_info_fif[i].efm_Log_VlanGroupNO = i+1;

		efm_RsetGetinfo->efm_vlangroup_info_fif[i].efm_Log_VlanMember = \
										efm_RsetGetinfo->efm_agent_remote_SetGet_data[efm_remote_set_ThVlanmber_FoVlanId1+i*2];	

		efm_RsetGetinfo->efm_vlangroup_info_fif[i].efm_Log_VlanId = \
									    efm_RsetGetinfo->efm_agent_remote_SetGet_data[efm_remote_set_ThVlanmber_FoVlanId1+i*2+1];

	}
}
int efm_agent_get_vlangroup_info(struct efm_vlangroup_info_data  *efm_vlangroup_info,efm_vlangroup_info_log efm_vlangroup_log_data)
{
	efm_vlangroup_info->efm_VlanGroupNO = efm_vlangroup_log_data.efm_Log_VlanGroupNO;
	// 4 5 6 bits is valid
	efm_vlangroup_info->efm_VlanMember = (efm_vlangroup_log_data.efm_Log_VlanMember>>4) & 0x07;
	// 4bits+8bits
	efm_vlangroup_info->efm_VlanId = 0;
	efm_vlangroup_info->efm_VlanId |= ((efm_vlangroup_log_data.efm_Log_VlanMember & 0x0f)<<8);
	// 8bits
	efm_vlangroup_info->efm_VlanId |= (efm_vlangroup_log_data.efm_Log_VlanId & 0xff);

	return EFM_SUCCESS;
}
/*==================================================*/
/*name : efm_agent_snmp_vlangroup_info_get                                                        */
/*para :                                                                                                                */
/*retu :  data_num                                                                                                */
/*desc :efm agent get local vlangroup info                                                               */ 
/*==================================================*/
int efm_agent_snmp_vlangroup_info_get(uint32_t ifindex, uint32_t SecIfindex,struct efm_vlangroup_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_vlangroup_info_snmp efm_vlangroup_snmp = {0}; 
	efm_agent_remote_SetGet_info * efm_RsetGetinfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	int ifindex_flag = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_vlangroup_info_snmp);
	msg_num -=3;

	memset(&efm_vlangroup_snmp,0,sizeof(struct efm_vlangroup_info_snmp));

	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);


	if ((0 == ifindex)&& (0 == SecIfindex))						/* first get data from efm */
	{

		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) ||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				continue;
			}
			
			efm_RsetGetinfo = aefm->aefm_RsetGetInfo;

			/*need to clear original data*/
			memset(efm_RsetGetinfo->efm_vlangroup_info_fif,0,sizeof(efm_vlangroup_info_log)*EFM_AGENT_CONFIG_VLAN_GROUP);
			efm_agent_get_remote_refresh_vlangroupinfo( efm_RsetGetinfo);
			for(i = 0;i< EFM_AGENT_CONFIG_VLAN_GROUP;i++)
			{

				efm_agent_get_vlangroup_info(&efm_vlangroup_snmp.efm_vlangroup_info,efm_RsetGetinfo->efm_vlangroup_info_fif[i]);
				efm_vlangroup_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++],&efm_vlangroup_snmp,sizeof(struct efm_vlangroup_info_snmp));
				memset(&efm_vlangroup_snmp,0,sizeof(struct efm_vlangroup_info_snmp));

				if (data_num == msg_num)
				{
					return data_num;
				}
			}

		}


	}


	else
	{		
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)	/* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
		{
			/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])	/* ~{51G09~O#M0A41m2;N*?U~} */
			{
				pbucket = l2if_table.buckets[val];
			}
			else										/* ~{51G09~O#M0A41mN*?U~} */
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


		while (pbucket)
		{

			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm)||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				goto loop;
			}

			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				goto loop;
			}
			
			efm_RsetGetinfo = aefm->aefm_RsetGetInfo;
			/*need to clear original data*/
			memset(efm_RsetGetinfo->efm_vlangroup_info_fif,0,sizeof(efm_vlangroup_info_log)*EFM_AGENT_CONFIG_VLAN_GROUP);
			efm_agent_get_remote_refresh_vlangroupinfo( efm_RsetGetinfo);
			for(i = 0;i< EFM_AGENT_CONFIG_VLAN_GROUP;i++)
			{
				if((efm_RsetGetinfo->efm_vlangroup_info_fif[i].efm_Log_VlanGroupNO> SecIfindex) && !ifindex_flag)
				{


					efm_agent_get_vlangroup_info(&efm_vlangroup_snmp.efm_vlangroup_info,efm_RsetGetinfo->efm_vlangroup_info_fif[i]);
					efm_vlangroup_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_vlangroup_snmp,sizeof(struct efm_vlangroup_info_snmp));
					memset(&efm_vlangroup_snmp,0,sizeof(struct efm_vlangroup_info_snmp));

					if(data_num == msg_num)
					{
						return data_num;

					}

				}


				if(ifindex_flag)
				{

					efm_agent_get_vlangroup_info(&efm_vlangroup_snmp.efm_vlangroup_info,efm_RsetGetinfo->efm_vlangroup_info_fif[i]);
					efm_vlangroup_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_vlangroup_snmp,sizeof(struct efm_vlangroup_info_snmp));
					memset(&efm_vlangroup_snmp,0,sizeof(struct efm_vlangroup_info_snmp));

					if (data_num == msg_num)
					{

						return data_num;
					}
				}

			}
loop:
			pnext = hios_hash_next_cursor(&l2if_table, pbucket);
			if ((NULL == pnext) || (NULL == pnext->data))
			{
				break;
			}

			pbucket = pnext;
			ifindex_flag = EFM_TRUE;
		}

}
return data_num;
}
void efm_agent_get_remote_refresh_oltptalminfo(efm_agent_remote_state_info * efm_RstInfo)
{
	int i  = 0;
	for(i = 0;i<PORT_NUM;i++ )
	{
		efm_RstInfo->efm_oltptalm_info_two[i].efm_Log_OltPtNO = i+1;
		//0,1,2bits
		efm_RstInfo->efm_oltptalm_info_two[i].efm_Log_OltPtStatInfo =  \
	
		efm_RstInfo->efm_agent_remote_state_data[efm_remote_state_olt1+i]&0x07;

		efm_RstInfo->efm_oltptalm_info_two[i].efm_Log_OltPtAlmInfo = efm_RstInfo->LinkFault_flag;
	}
	return ;
}
int efm_agent_get_oltptalm_info(struct efm_oltptalm_info_data  *efm_oltptalm_info,efm_oltptalm_info_log efm_oltptalm_log_data)
{
	efm_oltptalm_info->efm_OltPtalmNO= efm_oltptalm_log_data.efm_Log_OltPtNO;

	efm_oltptalm_info->efm_OltPtalmStatInfo= efm_oltptalm_log_data.efm_Log_OltPtStatInfo;

	efm_oltptalm_info->efm_OltPtalmAlmInfo = efm_oltptalm_log_data.efm_Log_OltPtAlmInfo;


	return EFM_SUCCESS;
}
/*==================================================*/
/*name : efm_agent_snmp_oltptalm_info_get                                                           */
/*para :                                                                                                                */
/*retu : data_num                                                                                                 */
/*desc :efm agent get local oltptalm info                                                                  */ 
/*==================================================*/

int efm_agent_snmp_oltptalm_info_get(uint32_t ifindex, uint32_t SecIfindex,struct efm_oltptalm_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_oltptalm_info_snmp efm_oltptalm_snmp = {0}; 
	efm_agent_remote_state_info * efm_RstInfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	int ifindex_flag = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_oltptalm_info_snmp);
	msg_num -=3;

	memset(&efm_oltptalm_snmp,0,sizeof(struct efm_oltptalm_info_snmp));

	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);


	if ((0 == ifindex)&& (0 == SecIfindex))						/* first get data from efm */
	{

		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm)||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RstInfo))
			{
				continue;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				continue;
			}
			
			efm_RstInfo= aefm->aefm_RstInfo;

			/*need to clear original data*/
			memset(efm_RstInfo->efm_oltptalm_info_two,0,sizeof(efm_oltptalm_info_log)*2);
			efm_agent_get_remote_refresh_oltptalminfo( efm_RstInfo);
			for(i = 0;i< PORT_NUM;i++)
			{

				efm_agent_get_oltptalm_info(&efm_oltptalm_snmp.efm_oltptalm_info,efm_RstInfo->efm_oltptalm_info_two[i]);
				efm_oltptalm_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++],&efm_oltptalm_snmp,sizeof(struct efm_oltptalm_info_snmp));
				memset(&efm_oltptalm_snmp,0,sizeof(struct efm_oltptalm_info_snmp));

				if (data_num == msg_num)
				{
					return data_num;
				}
			}

		}


	}


	else
	{		
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)	/* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
		{
			/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])	/* ~{51G09~O#M0A41m2;N*?U~} */
			{
				pbucket = l2if_table.buckets[val];
			}
			else										/* ~{51G09~O#M0A41mN*?U~} */
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


		while (pbucket)
		{

			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm)||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RstInfo))
			{
				goto loop;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				goto loop;
			}
			
			efm_RstInfo = aefm->aefm_RstInfo;

			/*need to clear original data*/
			memset(efm_RstInfo->efm_oltptalm_info_two,0,sizeof(efm_oltptalm_info_log)*2);
			efm_agent_get_remote_refresh_oltptalminfo( efm_RstInfo);
			for(i = 0;i< PORT_NUM;i++)
			{
				if((efm_RstInfo->efm_oltptalm_info_two[i].efm_Log_OltPtNO> SecIfindex) && !ifindex_flag)
				{


					efm_agent_get_oltptalm_info(&efm_oltptalm_snmp.efm_oltptalm_info,efm_RstInfo->efm_oltptalm_info_two[i]);
					efm_oltptalm_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_oltptalm_snmp,sizeof(struct efm_oltptalm_info_snmp));
					memset(&efm_oltptalm_snmp,0,sizeof(struct efm_oltptalm_info_snmp));

					if(data_num == msg_num)
					{
						return data_num;

					}

				}


				if(ifindex_flag)
				{

					efm_agent_get_oltptalm_info(&efm_oltptalm_snmp.efm_oltptalm_info,efm_RstInfo->efm_oltptalm_info_two[i]);
					efm_oltptalm_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_oltptalm_snmp,sizeof(struct efm_oltptalm_info_snmp));
					memset(&efm_oltptalm_snmp,0,sizeof(struct efm_oltptalm_info_snmp));

					if (data_num == msg_num)
					{

						return data_num;
					}
				}

			}
loop:
			pnext = hios_hash_next_cursor(&l2if_table, pbucket);
			if ((NULL == pnext) || (NULL == pnext->data))
			{
				break;
			}

			pbucket = pnext;
			ifindex_flag = EFM_TRUE;
		}

}
return data_num;
}
void efm_agent_get_remote_refresh_ethptalminfo(efm_agent_remote_state_info * efm_RstInfo)
{
	int i = 0;
	for(i = 0;i<PORT_NUM;i++)
	{
		/*only one eth port*/
		efm_RstInfo->efm_ethptalm_info_two[i].efm_Log_EthPtNO= i+1;
		//0,1,2bits
		efm_RstInfo->efm_ethptalm_info_two[i].efm_Log_EthPtStatInfo=  \
									      efm_RstInfo->efm_agent_remote_state_data[efm_remote_state_efh]&0x07;
		// 3 bits is valid--
		efm_RstInfo->efm_ethptalm_info_two[i].efm_Log_EthPtAlmInfo |= \
									    LM(efm_RstInfo->efm_agent_remote_state_data[efm_remote_state_efh],3);
		//DyingGasp
		efm_RstInfo->efm_ethptalm_info_two[i].efm_Log_EthPtAlmInfo |= \
									    efm_RstInfo->DyingGasp_flag<<1;
	}


	return ;
}
int efm_agent_get_ethptalm_info(struct efm_ethptalm_info_data  *efm_ethptalm_info,efm_ethptalm_info_log efm_ethptalm_log_data)
{
	efm_ethptalm_info->efm_EthPtNO = efm_ethptalm_log_data.efm_Log_EthPtNO;

	efm_ethptalm_info->efm_EthPtStatInfo = efm_ethptalm_log_data.efm_Log_EthPtStatInfo;

	efm_ethptalm_info->efm_EthPtAlmInfo = efm_ethptalm_log_data.efm_Log_EthPtAlmInfo;


	return EFM_SUCCESS;
}

/*==================================================*/
/*name : efm_agent_snmp_ethptalm_info_get                                                         */
/*para :                                                                                                                */
/*retu :  data_num                                                                                                */
/*desc :efm agent get local ethptalm info                                                                 */ 
/*==================================================*/
int efm_agent_snmp_ethptalm_info_get(uint32_t ifindex, uint32_t SecIfindex,struct efm_ethptalm_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_ethptalm_info_snmp efm_ethptalm_snmp = {0}; 
	efm_agent_remote_state_info * efm_RstInfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	int ifindex_flag = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_ethptalm_info_snmp);
	msg_num -=3;

	memset(&efm_ethptalm_snmp,0,sizeof(struct efm_ethptalm_info_snmp));

	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);


	if ((0 == ifindex)&& (0 == SecIfindex))						/* first get data from efm */
	{

		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm)||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RstInfo))
			{
				continue;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				continue;
			}
			
			efm_RstInfo= aefm->aefm_RstInfo;

			/*need to clear original data*/
			memset(efm_RstInfo->efm_ethptalm_info_two,0,sizeof(efm_ethptalm_info_log)*2);
			efm_agent_get_remote_refresh_ethptalminfo( efm_RstInfo);
			for(i = 0;i< PORT_NUM;i++)
			{

				efm_agent_get_ethptalm_info(&efm_ethptalm_snmp.efm_ethptalm_info,efm_RstInfo->efm_ethptalm_info_two[i]);
				efm_ethptalm_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++],&efm_ethptalm_snmp,sizeof(struct efm_ethptalm_info_snmp));
				memset(&efm_ethptalm_snmp,0,sizeof(struct efm_ethptalm_info_snmp));

				if (data_num == msg_num)
				{
					return data_num;
				}
			}

		}


	}


	else
	{		
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)	/* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
		{
			/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])	/* ~{51G09~O#M0A41m2;N*?U~} */
			{
				pbucket = l2if_table.buckets[val];
			}
			else										/* ~{51G09~O#M0A41mN*?U~} */
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


		while (pbucket)
		{

			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm)||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RstInfo))
			{
				goto loop;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				goto loop;
			}
			
			efm_RstInfo = aefm->aefm_RstInfo;

			/*need to clear original data*/
			memset(efm_RstInfo->efm_ethptalm_info_two,0,sizeof(efm_ethptalm_info_log)*2);
			efm_agent_get_remote_refresh_ethptalminfo( efm_RstInfo);
			for(i = 0;i< PORT_NUM;i++)
			{
				if((efm_RstInfo->efm_ethptalm_info_two[i].efm_Log_EthPtNO> SecIfindex) && !ifindex_flag)
				{


					efm_agent_get_ethptalm_info(&efm_ethptalm_snmp.efm_ethptalm_info,efm_RstInfo->efm_ethptalm_info_two[i]);
					efm_ethptalm_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_ethptalm_snmp,sizeof(struct efm_ethptalm_info_snmp));
					memset(&efm_ethptalm_snmp,0,sizeof(struct efm_ethptalm_info_snmp));

					if(data_num == msg_num)
					{
						return data_num;

					}

				}


				if(ifindex_flag)
				{

					efm_agent_get_ethptalm_info(&efm_ethptalm_snmp.efm_ethptalm_info,efm_RstInfo->efm_ethptalm_info_two[i]);
					efm_ethptalm_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_ethptalm_snmp,sizeof(struct efm_ethptalm_info_snmp));
					memset(&efm_ethptalm_snmp,0,sizeof(struct efm_ethptalm_info_snmp));

					if (data_num == msg_num)
					{

						return data_num;
					}
				}

			}
loop:
			pnext = hios_hash_next_cursor(&l2if_table, pbucket);
			if ((NULL == pnext) || (NULL == pnext->data))
			{
				break;
			}

			pbucket = pnext;
			ifindex_flag = EFM_TRUE;
		}

}
return data_num;
}
void efm_agent_get_remote_refresh_ethvlaninfo(efm_agent_remote_SetGet_info * efm_RsetGetInfo)
{
	int i = 0;
	for(i = 0;i<PORT_NUM;i++)
	{
		efm_RsetGetInfo->efm_ethvlan_info_two[i].efm_Log_EthPtNo = i+1;
		efm_RsetGetInfo->efm_ethvlan_info_two[i].efm_Log_EthPtInMode = \
									       efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPtInMode]&0x01;
		//0 bit is valid
		efm_RsetGetInfo->efm_ethvlan_info_two[i].efm_Log_EthPtTagMode = \
										efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPtTagMode]&0x01;
		//0 1 2bits is valid
		efm_RsetGetInfo->efm_ethvlan_info_two[i].efm_Log_EthPtPrior =\
									     efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPtPrior] & 0x07;
		//  4 bits + 8 bits
		efm_RsetGetInfo->efm_ethvlan_info_two[i].efm_Log_EthPtPVID |= \
									      (efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPtPvidH]& 0x0f)<<8;
		efm_RsetGetInfo->efm_ethvlan_info_two[i].efm_Log_EthPtPVID |=\
									     efm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPtPvidL];
	}


	return ;
}
int efm_agent_get_ethvlan_info(struct efm_ethvlan_info_data  * efm_ethvlan_info,efm_ethvlan_info_log efm_ethvlan_log_data)
{
	efm_ethvlan_info->efm_EthPtNo = efm_ethvlan_log_data.efm_Log_EthPtNo;
	//0bit is valid
	efm_ethvlan_info->efm_EthPtInMode = efm_ethvlan_log_data.efm_Log_EthPtInMode;
	//0 bit is valid
	efm_ethvlan_info->efm_EthPtTagMode = efm_ethvlan_log_data.efm_Log_EthPtTagMode;
	//0 1 2bits is valid
	efm_ethvlan_info->efm_EthPtPrior = efm_ethvlan_log_data.efm_Log_EthPtPrior;
	//  4 bits + 8 bits
	efm_ethvlan_info->efm_EthPtPVID = efm_ethvlan_log_data.efm_Log_EthPtPVID;

	return EFM_SUCCESS;
}
/*==================================================*/
/*name : efm_agent_snmp_ethvlan_info_get                                                           */
/*para :                                                                                                                */
/*retu :    data_num                                                                                              */
/*desc :efm agent get local ethvlan info                                                                   */ 
/*==================================================*/

int efm_agent_snmp_ethvlan_info_get(uint32_t ifindex, uint32_t SecIfindex,struct efm_ethvlan_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_ethvlan_info_snmp efm_ethvlan_snmp = {0}; 
	efm_agent_remote_SetGet_info * efm_RsetGetInfo = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	int ifindex_flag = 0;
	efm_agent_info * aefm = NULL;
	
	msg_num = IPC_MSG_LEN/sizeof(struct efm_ethvlan_info_snmp);
	msg_num -=3;

	memset(&efm_ethvlan_snmp,0,sizeof(struct efm_ethvlan_info_snmp));

	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);


	if ((0 == ifindex)&& (0 == SecIfindex))						/* first get data from efm */
	{

		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm)||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				continue;
			}
			
			efm_RsetGetInfo= aefm->aefm_RsetGetInfo;

			/*need to clear original data*/
			memset(efm_RsetGetInfo->efm_ethvlan_info_two,0,sizeof(efm_ethvlan_info_log)*2);
			efm_agent_get_remote_refresh_ethvlaninfo( efm_RsetGetInfo);
			for(i = 0;i< PORT_NUM;i++)
			{

				efm_agent_get_ethvlan_info(&efm_ethvlan_snmp.efm_ethvlan_info,efm_RsetGetInfo->efm_ethvlan_info_two[i]);
				efm_ethvlan_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++],&efm_ethvlan_snmp,sizeof(struct efm_ethvlan_info_snmp));
				memset(&efm_ethvlan_snmp,0,sizeof(struct efm_ethvlan_info_snmp));

				if (data_num == msg_num)
				{
					return data_num;
				}
			}

		}


	}


	else
	{		
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)	/* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
		{
			/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])	/* ~{51G09~O#M0A41m2;N*?U~} */
			{
				pbucket = l2if_table.buckets[val];
			}
			else										/* ~{51G09~O#M0A41mN*?U~} */
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


		while (pbucket)
		{

			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) ||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm) ||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				goto loop;
			}
			
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				goto loop;
			}
			
			efm_RsetGetInfo = aefm->aefm_RsetGetInfo;

			/*need to clear original data*/
			memset(efm_RsetGetInfo->efm_ethvlan_info_two,0,sizeof(efm_ethvlan_info_log)*2);
			efm_agent_get_remote_refresh_ethvlaninfo( efm_RsetGetInfo);
			for(i = 0;i< PORT_NUM;i++)
			{
				if((efm_RsetGetInfo->efm_ethvlan_info_two[i].efm_Log_EthPtNo> SecIfindex) && !ifindex_flag)
				{


					efm_agent_get_ethvlan_info(&efm_ethvlan_snmp.efm_ethvlan_info,efm_RsetGetInfo->efm_ethvlan_info_two[i]);
					efm_ethvlan_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_ethvlan_snmp,sizeof(struct efm_ethvlan_info_snmp));
					memset(&efm_ethvlan_snmp,0,sizeof(struct efm_ethvlan_info_snmp));

					if(data_num == msg_num)
					{
						return data_num;

					}

				}


				if(ifindex_flag)
				{

					efm_agent_get_ethvlan_info(&efm_ethvlan_snmp.efm_ethvlan_info,efm_RsetGetInfo->efm_ethvlan_info_two[i]);
					efm_ethvlan_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_ethvlan_snmp,sizeof(struct efm_ethvlan_info_snmp));
					memset(&efm_ethvlan_snmp,0,sizeof(struct efm_ethvlan_info_snmp));

					if (data_num == msg_num)
					{

						return data_num;
					}
				}

			}
loop:
			pnext = hios_hash_next_cursor(&l2if_table, pbucket);
			if ((NULL == pnext) || (NULL == pnext->data))
			{
				break;
			}

			pbucket = pnext;
			ifindex_flag = EFM_TRUE;
		}

}
	
	return data_num;
}
int efm_agent_get_ethperf_info(struct efm_ethperf_info_data  * efm_ethperf_info,efm_agent_info * aefm)
{
	int i = 0;

	efm_agent_remote_state_info * aefm_RstInfo = NULL;
	efm_agent_remote_SetGet_info * aefm_RsetGetInfo = NULL;

	aefm_RstInfo = aefm->aefm_RstInfo;
	aefm_RsetGetInfo = aefm->aefm_RsetGetInfo;

	efm_ethperf_info->efm_EthPerfNo = 1;
	efm_ethperf_info->efm_EthCntType =aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_EthCounterType];
	//no this value
	for(i = 0;i<BITS_NUM;i++)
	{
		efm_ethperf_info->efm_EthRxBytes[i] = aefm_RstInfo->efm_agent_remote_state_data[efm_remote_Eth_Bytes_Counter8+i];
	}
	efm_ethperf_info->efm_EthRxPCnt |=  aefm_RstInfo->efm_agent_remote_state_data[efm_remote_Eth_Counter4]<<24;
	efm_ethperf_info->efm_EthRxPCnt |=  aefm_RstInfo->efm_agent_remote_state_data[efm_remote_Eth_Counter3]<<16;
	efm_ethperf_info->efm_EthRxPCnt |=  aefm_RstInfo->efm_agent_remote_state_data[efm_remote_Eth_Counter2]<<8;
	efm_ethperf_info->efm_EthRxPCnt |=  aefm_RstInfo->efm_agent_remote_state_data[efm_remote_Eth_Counter1];

	efm_ethperf_info->efm_EthTxPCnt = 0;

	return EFM_SUCCESS;
}

/*==================================================*/
/*name : efm_agent_snmp_ethperf_info_get                                                            */
/*para :                                                                                                                */
/*retu :  data_num                                                                                                */
/*desc :efm agent get local ethperf info                                                                   */ 
/*==================================================*/
int efm_agent_snmp_ethperf_info_get(uint32_t ifindex,struct efm_ethperf_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_ethperf_info_snmp efm_ethperf_snmp = {0}; 
	efm_agent_info * aefm = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;


	msg_num = IPC_MSG_LEN/sizeof(struct efm_ethperf_info_snmp);
	msg_num -=3;

	memset(&efm_ethperf_snmp,0,sizeof(struct efm_ethperf_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);

	if (0 == ifindex) 
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm) || (pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
				|| (NULL == pif->pefm->aefm)||(NULL == pif->pefm->aefm->aefm_RsetGetInfo))
			{
				continue;
			}
			aefm = pif->pefm->aefm;
			if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
			{
				continue;
			}
			
			/*need to change*/
			/*0 ,1bits is valid*/
			efm_agent_get_ethperf_info(&efm_ethperf_snmp.efm_ethperf_info,aefm);
			efm_ethperf_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++], &efm_ethperf_snmp, sizeof(  struct efm_ethperf_info_snmp ));
			memset(&efm_ethperf_snmp,0,sizeof(struct efm_ethperf_info_snmp));

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
				if ((NULL == pif) || (NULL == pif->pefm) ||(pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY)\
					|| (NULL == pif->pefm->aefm) || (NULL == pif->pefm->aefm->aefm_RsetGetInfo))
				{
					continue;
				}

				aefm = pif->pefm->aefm;
				if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
				{
					continue;
				}

				efm_agent_get_ethperf_info(&efm_ethperf_snmp.efm_ethperf_info,aefm);
				efm_ethperf_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_ethperf_snmp, sizeof(  struct efm_ethperf_info_snmp ));
				memset(&efm_ethperf_snmp,0,sizeof(struct efm_ethperf_info_snmp));


				pbucket = pnext;
			}
		}
	}

	return data_num;
}
/*==================================================*/
/*name : snmp_msg_rcv_efm_agent_SecIfindex_info                                                */
/*para :                                                                                                                */
/*retu :  data_num                                                                                                */
/*desc :efm agent receive inquire form snmp two ifindex                                           */ 
/*==================================================*/

void snmp_msg_rcv_efm_agent_SecIfindex_info(struct ipc_msghdr_n  *phdr,uint32_t SecIfindex)
{


	int msg_num = 0;
	uint32_t ifindex = 0;

	if(NULL == phdr)
	{
		zlog_err("%s[%d]####### phdr is NULL",__FUNCTION__,__LINE__);
		return ;
	}


	EFM_LOG_DBG("%s[%d]########### %d\n",__FUNCTION__,__LINE__,phdr->data_num);
	ifindex = phdr->msg_index;

	EFM_LOG_DBG("%s[%d]################### %d   ifindex = %02x\n",__FUNCTION__,__LINE__,phdr->data_num,ifindex);
	switch(phdr->data_num)
	{
		case EFM_OLTPT_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_oltpt_info_snmp);
				struct efm_oltpt_info_snmp efm_snmp[msg_num];
				memset(efm_snmp, 0, msg_num*sizeof(struct efm_oltpt_info_snmp));
				msg_num = efm_agent_snmp_oltpt_info_get(ifindex, SecIfindex,efm_snmp);	 
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_OLTPT_INFO_TYPE);

			}
			break;
		case EFM_ETHPT_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_ethpt_info_snmp);
				struct efm_ethpt_info_snmp efm_snmp[msg_num];
				memset(efm_snmp, 0, msg_num*sizeof(struct efm_ethpt_info_snmp));
				msg_num = efm_agent_snmp_ethpt_info_get(ifindex, SecIfindex,efm_snmp);	 
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex, EFM_ETHPT_INFO_TYPE);

			}
			break;
		case EFM_VLANGROUP_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_vlangroup_info_snmp);
				struct efm_vlangroup_info_snmp efm_snmp[msg_num];
				memset(efm_snmp, 0, msg_num*sizeof(struct efm_vlangroup_info_snmp));
				msg_num = efm_agent_snmp_vlangroup_info_get(ifindex, SecIfindex,efm_snmp);	 
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex, EFM_VLANGROUP_INFO_TYPE);

			}
			break;
		case EFM_OLTPTALM_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_oltptalm_info_snmp);
				struct efm_oltptalm_info_snmp efm_snmp[msg_num];
				memset(efm_snmp, 0, msg_num*sizeof(struct efm_oltptalm_info_snmp));
				msg_num = efm_agent_snmp_oltptalm_info_get(ifindex, SecIfindex,efm_snmp);	 
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex, EFM_OLTPTALM_INFO_TYPE);
			}
			break;
		case EFM_ETHPTALM_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_ethptalm_info_snmp);
				struct efm_ethptalm_info_snmp efm_snmp[msg_num];
				memset(efm_snmp, 0, msg_num*sizeof(struct efm_ethptalm_info_snmp));
				msg_num = efm_agent_snmp_ethptalm_info_get(ifindex, SecIfindex,efm_snmp);	 
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex, EFM_ETHPTALM_INFO_TYPE);
			}
			break;


		case EFM_ETHVLAN_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_ethvlan_info_snmp);
				struct efm_ethvlan_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_ethvlan_info_snmp));
				msg_num  = efm_agent_snmp_ethvlan_info_get(ifindex,SecIfindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_ETHVLAN_INFO_TYPE);

			}
			break;

		case EFM_ETHPERF_INFO_TYPE:
			{
				msg_num = IPC_MSG_LEN/sizeof(struct efm_ethperf_info_snmp);
				struct efm_ethperf_info_snmp efm_snmp[msg_num];
				memset(efm_snmp,0,msg_num * sizeof(struct efm_ethperf_info_snmp));
				msg_num  = efm_agent_snmp_ethperf_info_get(ifindex,efm_snmp);
				efm_agent_send_data_to_snmp(phdr,efm_snmp,msg_num,ifindex,EFM_ETHPERF_INFO_TYPE);
			}
			break;

		default:

			msg_num  = 0;
			efm_agent_send_data_to_snmp(phdr,NULL,msg_num,0,0);
			break;


	}
}
/*==================================================*/
/*name : efm_agent_send_data_to_snmp                                                                 */
/*para :                                                                                                                */
/*retu :  fail -1                                                                                                     */
/*desc :efm agent send existing data to snmp                                                          */ 
/*==================================================*/
int efm_agent_send_data_to_snmp(struct ipc_msghdr_n * phdr,void * pdata,int msg_num,uint32_t ifindex,uint8_t type)
{
	int data_len =0;
	int ret = 0;

	/*compute len of send snmp depend on type*/
	switch(type)
	{
		case EFM_COMM_INFO_TYPE:
			data_len = msg_num*sizeof(struct efm_comm_info_snmp);
			break;
		case EFM_OLTPT_INFO_TYPE:
			data_len = msg_num*sizeof(struct efm_oltpt_info_snmp);
			break;
		case EFM_ETHPT_INFO_TYPE:
			data_len = msg_num*sizeof(struct efm_ethpt_info_snmp);
			break;
		case EFM_OLTPTALM_INFO_TYPE:
			data_len = msg_num*sizeof(struct efm_oltptalm_info_snmp);
			break;
		case EFM_ETHPTALM_INFO_TYPE:
			data_len = msg_num*sizeof(struct efm_ethptalm_info_snmp);
			break;		
		case EFM_VLAN_INFO_TYPE:
			data_len = msg_num*sizeof(struct efm_vlan_info_snmp);
			break;
		case EFM_OLTVLAN_INFO_TYPE:
			data_len = msg_num*sizeof(struct efm_oltvlan_info_snmp);
			break;
		case EFM_ETHVLAN_INFO_TYPE:
			data_len = msg_num*sizeof(struct efm_ethvlan_info_snmp);
			break;
		case EFM_CPUVLAN_INFO_TYPE:
			data_len = msg_num*sizeof(struct efm_cpuvlan_info_snmp);
			break;
		case EFM_VLANGROUP_INFO_TYPE:
			data_len = msg_num * sizeof(struct efm_vlangroup_info_snmp);
			break;
		case EFM_QINQ_INFO_TYPE:
			data_len = msg_num *sizeof(struct efm_qinq_info_snmp);
			break;
		case EFM_QOS_INFO_TYPE:
			data_len = msg_num *sizeof(struct efm_qos_info_snmp);
			break;
		case EFM_PERF_INFO_TYPE:
			data_len = msg_num * sizeof(struct efm_perf_info_snmp);
			break;
		case EFM_OLTPERF_INFO_TYPE:
			data_len = msg_num * sizeof(struct efm_oltperf_info_snmp);
			break;
		case EFM_ETHPERF_INFO_TYPE:
			data_len = msg_num * sizeof(struct efm_ethperf_info_snmp);
			break;
		case EFM_CPUPERF_INFO_TYPE:
			data_len = msg_num * sizeof(struct efm_cpuperf_info_snmp);
			break;
		case EFM_OAM_INFO_TYPE:
			data_len = msg_num * sizeof(struct efm_oam_info_snmp);
			break;
		case EFM_ADDR_INFO_TYPE:
			data_len = msg_num * sizeof(struct efm_addr_info_snmp);
			break;
		case EFM_CONVER_INFO_TYPE:
			data_len = msg_num * sizeof(struct efm_conver_info_snmp);
			break;
		default:
			return 0;

	}
	EFM_LOG_DBG("[%s][%d]data_num = %d data_len = %d\n",__FUNCTION__,__LINE__,msg_num,data_len);


	if (msg_num > 0)
	{
		ipc_ack_to_snmp(phdr,pdata, data_len,msg_num);
		
	}
	else
	{
		ipc_noack_to_snmp(phdr);

	}

	return ret;
}
/*==================================================*/
/*name : snmp_msg_rcv_efm_agent_set                                                                 */
/*para :                                                                                                                */
/*retu : NULL                                                                                                        */
/*desc :snmp pass efm agent to config remote device info                                         */ 
/*==================================================*/

void snmp_msg_rcv_efm_agent_set(struct ipc_msghdr_n *phdr,uint32_t * pdata)
{

	int8_t ret = 0;
	int i = 0;
	uint8_t num = 0;
	uint32_t set_value = 0;
	uint32_t set_item = 0;  
	uint32_t set_table = 0;
	uint32_t ifindex = 0;
	uint32_t two_ifindex = 0;
	struct l2if * pif = NULL;
	struct efm_if * pefm = NULL;
	efm_agent_info * aefm = NULL;

	if(NULL == phdr || NULL == pdata)
	{
		zlog_err("%s[%d]####### phdr is NULL",__FUNCTION__,__LINE__);
		return ;
	}

	ifindex = phdr->msg_index;

	EFM_LOG_DBG("%s[%d]########################### ifindex = %02x ",__FUNCTION__,ifindex);

	pif = l2if_lookup(ifindex);
	{
		if(NULL == pif || NULL == pif->pefm  ||pif->pefm->efm_discovery_state != EFM_DISCOVERY_SEND_ANY || NULL == pif->pefm->aefm)
		{
			zlog_err("%s[%d]####### pif or pefm or pefm(aefm)  is NULL",__FUNCTION__,__LINE__);
			//ipc_noack_to_snmp(phdr);
			 ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_L2, 
						IPC_TYPE_SNMP, phdr->msg_subtype, 0,0, IPC_OPCODE_NACK);
			return ;
		}
	}
	
	aefm = pif->pefm->aefm;
	
	if(EFM_FALSE == aefm->efm_agent_last_link_flag || EFM_FALSE == aefm->efm_agent_link_flag\
				|| aefm->aefm_RstInfo->DyingGasp_flag ||aefm->aefm_RstInfo->LinkFault_flag)
	{
		zlog_err("%s[%d]####### remote device is not 11000A/11000An/1101n",__FUNCTION__,__LINE__);
		//ipc_noack_to_snmp(phdr);
		 ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_L2, 
						IPC_TYPE_SNMP, phdr->msg_subtype, 0,0, IPC_OPCODE_NACK);

		return ;
	}



	for(i = 0;i  < phdr->data_num;i++)
	{
		EFM_LOG_DBG("%s[%d]################pdata = %d\n",__FUNCTION__,__LINE__,pdata[i]);
	}
	
	pefm = pif->pefm;
	two_ifindex = pdata[num++];                /*get second ifindex*/
	set_value = pdata[num++];		  /*get config value*/
	set_item = pdata[num++];		/*get config specific item*/	
	set_table = pdata[num++];		/*get config table*/
	
	EFM_LOG_DBG("###########set_table = %d\n  set_item = %d\n set_value = %d\n",set_table,set_item,set_value);

	if(num > phdr->data_num)
	{
		zlog_err("%s[%d]####### data from snmp   is err num: %d > data_num%d",\
				__FUNCTION__,__LINE__,num,phdr->data_num);
		return ;

	}
	/*deal with one ifindex and double ifindex*/
	if(ifindex ==  two_ifindex)
	{
		
		ret = efm_agent_set_remote_Table(pefm,set_value,set_item,set_table);
	}
	else
	{

		ret = efm_agent_set_remote_TwoIfindex_Table(two_ifindex,pefm,set_value,set_item,set_table);

	}
	if(ret < 0)
	{
		zlog_err("%s[%d]#######ifindex = %02x  set_value  =  %d    set_item = %d    set_table = %d  aefm set  fail!",\
				__FUNCTION__,__LINE__,two_ifindex,set_value,set_item,set_table);
		//ipc_noack_to_snmp(phdr);
		 ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, MODULE_ID_L2, 
						IPC_TYPE_SNMP, phdr->msg_subtype, 0,0, IPC_OPCODE_NACK);
		return ;

	}
	else 
	{
		//ipc_send_ack(MODULE_ID_SNMPD,MODULE_ID_L2,IPC_TYPE_SNMP,phdr->msg_subtype,phdr->msg_index);
		l2_msg_send_ack (phdr,ifindex);
	}
	

	return ;
}
/*==================================================*/
/*name : efm_agent_set_remote_Table                                                                    */
/*para :                                                                                                                */
/*retu : NULL                                                                                                        */
/*desc :efm agent set remote different table                                                             */ 
/*==================================================*/

int efm_agent_set_remote_Table(struct efm_if * pefm,uint32_t set_value,uint32_t set_item,uint32_t set_table)
{
	int ret = 0;
	efm_agent_info * aefm  = NULL;

	aefm = pefm->aefm;			


	if(NULL == aefm->aefm_RsetInfo)
	{		

		zlog_err("%s[%d]aefm  set space  is  NULL!",__FUNCTION__,__LINE__);
		return -1;			
	}

	switch(set_table)
	{
		case hpmcrtCommConf_set:
			ret = efm_agent_set_remote_CommConf_Item(pefm,set_value,set_item);
			break;

		case hpmcrtVlanConf_set:
			ret = efm_agent_set_remote_VlanConf_Item(pefm,set_value,set_item);
			break;

		case hpmcrtOltPtVlanConf_set:
			ret = efm_agent_set_remote_OltptVlanConf_Item(pefm,set_value,set_item);
			break;

		case hpmcrtCpuPtVlanConf_set:
			ret = efm_agent_set_remote_CpuptVlanConf_Item(pefm,set_value,set_item);
			break;

		case hpmcrtQinQ_set:
			ret = efm_agent_set_remote_QinQ_Item(pefm,set_value,set_item);
			break;

		case hpmcrtQoS_set:
			ret = efm_agent_set_remote_Qos_Item(pefm,set_value,set_item);
			break;

		case hpmcrtPerf_set:
			ret = efm_agent_set_remote_Perf_Item(pefm,set_value,set_item);
			break;

		case hpmcrtOltPerf_set:
			ret = efm_agent_set_remote_OltPerf_Item(pefm,set_value,set_item);
			break;

		case hpmcrtCpuPerf_set:
			ret = efm_agent_set_remote_CpuPerf_Item(pefm,set_value,set_item);
			break;

		case hpmcrtOam_set:
			ret = efm_agent_set_remote_OamMode_Item(pefm,set_value,set_item);
			break;

		case hpmcrtAddr_set:
			ret = efm_agent_set_remote_AddrInfo_Item(pefm,set_value,set_item);
			break;

		default:
			return -1;

	}
	if(ret < 0)
	{
		return ret;
	}
	pefm->aefm->efm_agent_pdu_type = EFM_AGENT_SET_INFO_PDU;
	pefm->aefm->aefm_RsetInfo->efm_remote_set_start_addr_num = 1;
	ret = efm_agent_pdu_tx (pefm);	

	return ret;

}
/*==================================================*/
/*name : efm_agent_set_remote_AddrInfo_Item                                                       */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some AddrInfo sequence                                              */ 
/*==================================================*/

int efm_agent_set_remote_AddrInfo_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;

	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}
	
	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;

	switch(set_item)
	{
		case hpmcrtIpAddr:

			/*now ip != previous ip*/
			if(memcmp(pefm->aefm->efm_agent_update_ip,&set_value,4))
			{
				memcpy(pefm->aefm->efm_agent_update_ip,&set_value,4);
				/*send config ip to ospf*/
				efm_agent_u0_info_send(pefm->if_index,pefm->aefm,IPC_OPCODE_UPDATE);
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_4IpAddr;	
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = (set_value>>24)&0xff;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1] = (set_value>>16)&0xff;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+2] = (set_value>>8)&0xff;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+3] = set_value&0xff;
			aefm_RsetInfo->efm_remote_set_addr_len = 4;
			break;

		case hpmcrtIpMask:

			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_4IpMask;	
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = (set_value>>24)&0xff;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1] = (set_value>>16)&0xff;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+2] = (set_value>>8)&0xff;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+3] = set_value&0xff;
			aefm_RsetInfo->efm_remote_set_addr_len = 4;
			break;

		case  hpmcrtGatewayAddr:

			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_4GatewayAddr;	
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = (set_value>>24)&0xff;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1] = (set_value>>16)&0xff;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+2] = (set_value>>8)&0xff;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+3] = set_value&0xff;
			aefm_RsetInfo->efm_remote_set_addr_len = 4;
			break;

		default:
			return -1;
	}


	return ret;

}
/*==================================================*/
/*name : efm_agent_set_remote_OamMode_Item                                                       */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some oam sequence                                                    */ 
/*==================================================*/

int efm_agent_set_remote_OamMode_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;


	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;

	if(hpmcrtOamMode != set_item)
	{
		return -1;
	}
	if(set_value > 3)
	{
		return -1;
	}
	aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_Oam_Mode;
	aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value&0x03;
	aefm_RsetInfo->efm_remote_set_addr_len = 1;

	return ret;

}
/*==================================================*/
/*name : efm_agent_set_remote_CpuPerf_Item                                                       */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some CpuPerf sequence                                              */ 
/*==================================================*/

int efm_agent_set_remote_CpuPerf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;


	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;


	if(set_item  !=    hpmcrtCpuCntType ||  set_value > 17)
	{
		return -1;
	}
	aefm_RsetInfo->efm_remote_set_start_addr =    efm_remote_set_CpuCounterType;	
	aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value&0xff;
	aefm_RsetInfo->efm_remote_set_addr_len = 1;


	return ret;

}
/*==================================================*/
/*name : efm_agent_set_remote_EthPerf_Item                                                         */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some EthPerf sequence                                                */ 
/*==================================================*/

int efm_agent_set_remote_EthPerf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;


	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;


	if(set_item  !=   hpmcrtEthCntType ||  set_value > 17)
	{
		return -1;
	}
	aefm_RsetInfo->efm_remote_set_start_addr =  efm_remote_set_EthCounterType;	
	aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value&0xff;
	aefm_RsetInfo->efm_remote_set_addr_len = 1;


	return ret;

}
/*==================================================*/
/*name : efm_agent_set_remote_OltPerf_Item                                                         */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some OltPerf sequence                                                */ 
/*==================================================*/

int efm_agent_set_remote_OltPerf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;


	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;


	if(set_item  !=    hpmcrtOltCntType ||  set_value > 17)
	{
		return -1;
	}
	aefm_RsetInfo->efm_remote_set_start_addr =   efm_remote_set_OltCounterType;	
	aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value&0xff;
	aefm_RsetInfo->efm_remote_set_addr_len = 1;

	return ret;

}
/*==================================================*/
/*name : efm_agent_set_remote_Perf_Item                                                         */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some Perf sequence                                                */ 
/*==================================================*/

int efm_agent_set_remote_Perf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;


	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;


	if(set_item  !=   hpmcrtCleanCounter ||  set_value > 1)
	{
		return -1;
	}
	aefm_RsetInfo->efm_remote_set_start_addr =  efm_remote_set_CleanCounter;	
	aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value&0x01;
	aefm_RsetInfo->efm_remote_set_addr_len = 1;

	return ret;

}
/*==================================================*/
/*name : efm_agent_set_remote_Qos_Item                                                         */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some Qos sequence                                                */ 
/*==================================================*/

int efm_agent_set_remote_Qos_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;


	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;


	if(set_item  !=   hpmcrt1pQos  ||  set_value > 1)
	{
		return -1;
	}
	aefm_RsetInfo->efm_remote_set_start_addr =  efm_remote_set_1pQos;	
	aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value&0x01;
	aefm_RsetInfo->efm_remote_set_addr_len = 1;

	return ret;

}
/*==================================================*/
/*name : efm_agent_set_remote_QinQ_Item                                                         */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some QinQ sequence                                                */ 
/*==================================================*/

int efm_agent_set_remote_QinQ_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;


	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;

	switch(set_item)
	{
		case hpmcrtQinQDirection:
			if(set_value > 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_QinQDirection;	
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value&0x01;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtQinQTPID:
			if(set_value > 65535)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_QinQTpidH;	
			// high 8 bits
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = (set_value>>8)&0xff;
			// low 8 high
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1] = set_value & 0xff;
			aefm_RsetInfo->efm_remote_set_addr_len = 2;
			break;

		case hpmcrtQinQVid:

			if(set_value > 4094 || set_value < 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = efm_remote_set_QinQVidH;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = (set_value>>8)&0xff;
			// low 8 high
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1] = set_value & 0xff;
			aefm_RsetInfo->efm_remote_set_addr_len = 2;
			break;

		case hpmcrtQinQPrior:
			if(set_value > 7)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = efm_remote_set_QinQPiror;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  = set_value & 0x07;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		default:
			return -1;
	}


	return ret;
}
/*==================================================*/
/*name : efm_agent_set_remote_CpuptVlanConf_Item                                              */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some CpuptVlanConf sequence                                      */ 
/*==================================================*/

int efm_agent_set_remote_CpuptVlanConf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;


	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;

	if(hpmcrtOltPtPVID != set_item || set_value > 4094 || set_value < 1)
	{
		return -1;
	}
	aefm_RsetInfo->efm_remote_set_start_addr =  efm_remote_set_CpuPtPvidH;
	//high 4 bits
	aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  = (set_value>>8)&0x0f;
	// low 8 bits
	aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1]  = set_value&0xff;
	aefm_RsetInfo->efm_remote_set_addr_len = 2;

	return ret;
}
/*==================================================*/
/*name : efm_agent_set_remote_OltptVlanConf_Item                                              */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some OltptVlanConf sequence                                      */ 
/*==================================================*/

int efm_agent_set_remote_OltptVlanConf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;

	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;

	switch(set_item)
	{
		case hpmcrtOltPtInMode:
			if(set_value > 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_OltPtInMode;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  = set_value & 0x01;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtOltPtPVID:
			if(set_value > 4094)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_OltPtPvidH;
			//high 4 bits
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  = (set_value>>8)&0x0f;
			// low 8 bits
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1]  = set_value&0xff;
			aefm_RsetInfo->efm_remote_set_addr_len = 2;
			break;

		case hpmcrtOltPtPrior:
			if(set_value > 7)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_OltPtPrior;
			// low 3 bits
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  = set_value & 0x07;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtOltPtTagMode:
			if(set_value > 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr =  efm_remote_set_OltPtTagMode;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value & 0x01;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		default:
			return -1;

	}

	return ret;

}
/*==================================================*/
/*name : efm_agent_set_remote_ptVlanConf_Item                                              */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some ptVlanConf sequence                                      */ 
/*==================================================*/

int efm_agent_set_remote_VlanConf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;


	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;

	if(hpmcrtVlanMode != set_item)
	{
		return -1;
	}
	if(set_value > 2)
	{
		return -1;
	}
	aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_VlanMode;
	aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value&0x03;
	aefm_RsetInfo->efm_remote_set_addr_len = 1;

	return ret;

}
/*==================================================*/
/*name : efm_agent_set_remote_CommConf_Item                                                   */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some CommConf sequence                                          */ 
/*==================================================*/

int efm_agent_set_remote_CommConf_Item(struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;


	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;

	switch(set_item)
	{
		case hpmcrtDataResum:
			if(set_value > 2 )
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_Data_Resume;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtLFPCtrl:
			if(set_value > 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_LFPCtrl;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtALSCtrl:
			if(set_value > 1 )
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_ALS;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtJumbo:
			if(set_value > 1 )
			{
				return -1;

			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_JumFrame;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case  hpmcrtReboot:
			if(set_value > 1 )
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_Reboot;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case  hpmcrtIpCtrl:
			if(set_value > 1 )
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_IpCtrl;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtEthStromfilter:                         //123bits is valid
			if(set_value > 7 )
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_EthStromfilter;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtDHCPclient:
			if(set_value > 1 )
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_DHCPclient;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		default:
			return -1;
	}
	aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value;

	return ret;

}

/*==================================================*/
/*name : efm_agent_set_remote_TwoIfindex_Item                                                   */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent cope with twoifindex config from snmp inquire                               */ 
/*==================================================*/
int efm_agent_set_remote_TwoIfindex_Table(uint32_t two_ifindex,struct efm_if * pefm,uint32_t set_value,uint32_t set_item,uint32_t set_table)
{
	int ret = 0;
	efm_agent_info * aefm  = NULL;

	aefm = pefm->aefm;			


	if(NULL == aefm->aefm_RsetInfo)
	{		

		zlog_err("%s[%d]aefm  set space  is  NULL!",__FUNCTION__,__LINE__);
		return -1;			
	}

	switch(set_table)
	{

		case hpmcrtOltPtConf_set:
			ret = efm_agent_set_remote_OltptConf_Item(two_ifindex,pefm,set_value,set_item);
			break;

		case hpmcrtEthPtConf_set:
			ret = efm_agent_set_remote_EthptConf_Item(two_ifindex,pefm,set_value,set_item);
			break;

		case hpmcrtEthPtVlanConf_set:
			ret = efm_agent_set_remote_EthptVlanConf_Item(two_ifindex,pefm,set_value,set_item);
			break;

		case  hpmcrtVlanGroupConf_set:
			ret = efm_agent_set_remote_VlanGroupConf_Item(two_ifindex,pefm,set_value,set_item);
			break;

		case hpmcrtEthPtPerf_set:
			ret = efm_agent_set_remote_EthPerf_Item(pefm,set_value,set_item);
			break;

		default:
			return -1;
	}
	if(ret < 0)
	{
		return ret;
	}
	pefm->aefm->efm_agent_pdu_type = EFM_AGENT_SET_INFO_PDU;
	pefm->aefm->aefm_RsetInfo->efm_remote_set_start_addr_num = 1;
	ret = efm_agent_pdu_tx (pefm);	
	
	return ret;

}
/*==================================================*/
/*name : efm_agent_set_remote_VlanGroupConf_Item                                             */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some VlanGroupConf sequence                                     */ 
/*==================================================*/

int efm_agent_set_remote_VlanGroupConf_Item(uint32_t two_ifindex,struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	uint8_t i = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;
	efm_agent_remote_SetGet_info * aefm_RsetGetInfo = NULL;


	if(NULL == pefm->aefm->aefm_RsetInfo || NULL == pefm->aefm->aefm_RsetGetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	
	if(two_ifindex > 15 || two_ifindex < 1)
	{
		return -1;

	}
	
	EFM_LOG_DBG("%s[%d]################## set_value = %d two_ifindex = %d\n",\
			__FUNCTION__,__LINE__,set_value,two_ifindex);

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;
	aefm_RsetGetInfo  = pefm->aefm->aefm_RsetGetInfo;
	
	/*first get orignal data*/
	for(i = 0;i<15;i++)
	{
		
		aefm_RsetInfo->efm_remote_set_value[efm_remote_set_ThVlanmber_FoVlanId1+i*2 ] =  \
			aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_ThVlanmber_FoVlanId1+i *2];
		aefm_RsetInfo->efm_remote_set_value[efm_remote_set_ThVlanmber_FoVlanId1+i*2+1 ] =  \
			aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_ThVlanmber_FoVlanId1+i *2+1];

	}
	switch(set_item)
	{
		case hpmcrtVlanMember:
			if(set_value > 7)
			{
				return -1;
			}
			// 15 group data 
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_ThVlanmber_FoVlanId1+(two_ifindex-1)*2;
			// 4,5,6bit is valid
			/*need to clear orignal 4,5,6bits*/
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  &= (~0xf0);
			/*then refresh to config value*/
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  |= (set_value<<4);
			aefm_RsetInfo->efm_remote_set_addr_len = 2;
			break;

		case hpmcrtVlanId:
			if(set_value > 4094 || set_value < 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_ThVlanmber_FoVlanId1+(two_ifindex-1)*2;
			//high 4 bits
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] &= (~0x0f);
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  |= ((set_value>>8)&0x0f);
			// low 8 bits
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1]  = set_value&0xff;
			aefm_RsetInfo->efm_remote_set_addr_len = 2;
			break;


		default:
			return -1;

	}

	return ret;

}
/*==================================================*/
/*name : efm_agent_set_remote_EthptVlanConf_Item                                               */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some EthptVlanConf sequence                                      */ 
/*==================================================*/

int efm_agent_set_remote_EthptVlanConf_Item(uint32_t two_ifindex,struct efm_if * pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;


	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;

	switch(set_item)
	{
		case hpmcrtEthPtInMode:
			if(set_value > 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_EthPtInMode;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  = set_value & 0x01;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case  hpmcrtEthPtPVID:
			if(set_value > 4094)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_EthPtPvidH;
			//high 4 bits
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  = (set_value>>8)&0x0f;
			// low 8 bits
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1]  = set_value&0xff;
			aefm_RsetInfo->efm_remote_set_addr_len = 2;
			break;

		case hpmcrtEthPtPrior:
			if(set_value > 7)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr = efm_remote_set_EthPtPrior;
			// low 3 bits
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  = set_value & 0x07;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtEthPtTagMode:
			if(set_value > 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr =  efm_remote_set_EthPtTagMode;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value & 0x01;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		default:
			return -1;

	}

	return ret;
}
/*==================================================*/
/*name : efm_agent_set_remote_EthptConf_Item                                                     */
/*para :                                                                                                                */
/*retu : fail -1                                                                                                       */
/*desc :efm agent set remote some EthptConf sequence                                            */ 
/*==================================================*/

int efm_agent_set_remote_EthptConf_Item(uint32_t two_ifindex,struct efm_if *pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;
	efm_agent_remote_SetGet_info * aefm_RsetGetInfo = NULL;

	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;
	aefm_RsetGetInfo = pefm->aefm->aefm_RsetGetInfo;

	aefm_RsetInfo->efm_remote_set_value[efm_remote_set_EthPort]  = \
								       aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_EthPort];

	switch(set_item)
	{
		case hpmcrtEthPtNegCtrl :
			if(set_value > 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = efm_remote_set_EthPort;
			//0--FFDX //1,2---FSPD 3---FAUT()
			set_value <<= 3;

			/*need to clear original data this bit (3)*/
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  &=  (~0x08);

			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] |= (set_value&0x08);

			EFM_LOG_DBG("%s[%d]###################### %d  addr = %d\n",__FUNCTION__,__LINE__,\
					aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] ,aefm_RsetInfo->efm_remote_set_start_addr);

			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtEthPtSpdCtrl:
			/*eth port nonsupport 1000M */
			if(set_value >1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = efm_remote_set_EthPort;
			set_value  <<=1;

			/*need to clear original data this bit (1,2)*/
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] &=  (~0x06);

			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] |= (set_value&0x06);

			EFM_LOG_DBG("%s[%d]###################### %d  addr = %d\n",__FUNCTION__,__LINE__,\
					aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] ,aefm_RsetInfo->efm_remote_set_start_addr);

			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtEthPtDplCtrl:
			if(set_value > 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = efm_remote_set_EthPort;

			/*need to clear original data this bit (0)*/
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] &=  (~0x01);

			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] |= (set_value&0x01);

			EFM_LOG_DBG("%s[%d]###################### %d  addr = %d\n",__FUNCTION__,__LINE__,\
					aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] ,aefm_RsetInfo->efm_remote_set_start_addr);

			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case   hpmcrtEthPtDisCtrl :
			/*ETH poet nonsupport shutdown*/
			if(set_value > 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = efm_remote_set_EthPort;
			//7bit is valid
			set_value <<=7;
			/*need to clear original data this bit (0)*/
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  &=  (~0x80);
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] |= (set_value&0x80);
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtEthPtIngressRateN:
			if(set_value > 16000)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = \
								    efm_remote_set_EthPtIngressRateH;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  =\
													(uint8_t)((set_value>>8)&0x00ff);
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1] =\
													 (uint8_t)(set_value & 0x00ff);
			aefm_RsetInfo->efm_remote_set_addr_len = 2;
			break;

		case  hpmcrtEthPtEgressRateN :
			if(set_value > 16000)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = \
								    efm_remote_set_EthPtEgressRateH;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  =\
													(uint8_t)((set_value>>8)&0x00ff);
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1] =\
													 (uint8_t)(set_value & 0x00ff);
			aefm_RsetInfo->efm_remote_set_addr_len = 2;
			break;

		case  hpmcrtEthPtFlowCtrl :
			if(set_value>1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr =\
								  efm_remote_set_EthFlow;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value&0x01;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		default:
			return  -1;


	}

	return ret;
}

/*==================================================*/
/*name : efm_agent_set_remote_OltptConf_Item                                                     */
/*para :                                                                                                               */
/*retu : fail -1                                                                                                      */
/*desc :efm agent set remote some OltptConf sequence                                             */ 
/*==================================================*/
int efm_agent_set_remote_OltptConf_Item(uint32_t two_ifindex,struct efm_if *pefm,uint32_t set_value,uint32_t set_item)
{
	int ret = 0;
	efm_agent_remote_set_info * aefm_RsetInfo = NULL;
	efm_agent_remote_SetGet_info * aefm_RsetGetInfo = NULL;

	if(NULL == pefm->aefm->aefm_RsetInfo)
	{
		zlog_err("%s[%d]aefm malloc set space  fail!",__FUNCTION__,__LINE__);
		return -1;
	}

	aefm_RsetInfo = pefm->aefm->aefm_RsetInfo;
	aefm_RsetGetInfo  = pefm->aefm->aefm_RsetGetInfo;

	/*need to get original data*/
	aefm_RsetInfo->efm_remote_set_value[efm_remote_set_Olt1Port]  = \
									aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_Olt1Port];

	aefm_RsetInfo->efm_remote_set_value[efm_remote_set_Olt2Port]  = \
									aefm_RsetGetInfo->efm_agent_remote_SetGet_data[efm_remote_set_Olt2Port];

	switch(set_item)
	{
		case hpmcrtOltPtNegCtrl:
			/*FAUT   0-manaul mode 1-autonegotiation*/
			if(set_value > 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = \
								    1 == two_ifindex ? efm_remote_set_Olt1Port : efm_remote_set_Olt2Port;
			//0--FFDX //1,2---FSPD 3---FAUT()
			set_value <<= 3;
			/*need to clear original data this bit (3)*/

			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] &=  (~0x08);

			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] |= (set_value&0x08);

			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtOltPtSpdCtrl:
			if(set_value >2)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = \
								    1 == two_ifindex ? efm_remote_set_Olt1Port : efm_remote_set_Olt2Port;
			set_value  <<=1;
			/*need to clear original data this bit (1,2)*/

			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] &=  (~0x06);

			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] |= (set_value&0x06);

			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtOltPtDplCtrl:
			/*olt port nonsupport half-duplex*/
			if(1 != set_value)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = \
								    1 == two_ifindex ? efm_remote_set_Olt1Port : efm_remote_set_Olt2Port;
			/*need to clear original data this bit (0)*/

			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] &=  (~0x01);

			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] |= (set_value&0x01);

			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtOltPtDisCtrl :
			if(set_value > 1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = \
								    1 == two_ifindex ? efm_remote_set_Olt1Port : efm_remote_set_Olt2Port;
			//7bit is valid
			set_value <<=7;
			/*need to clear original data this bit (0)*/

			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] &= (~0x80);

			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] |= (set_value&0x80);

			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		case hpmcrtOltPtIngressRateN:
			if(set_value > 16000)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = \
								    efm_remote_set_OltPtIngressRateH;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  =\
													(uint8_t)((set_value>>8)&0x00ff);
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1] =\
													 (uint8_t)(set_value & 0x00ff);
			aefm_RsetInfo->efm_remote_set_addr_len = 2;
			break;

		case  hpmcrtOltPtEgressRateN :
			if(set_value > 16000)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr  = \
								    efm_remote_set_OltPtEgressRateH;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr]  =\
													(uint8_t)((set_value>>8)&0x00ff);
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr+1] =\
													 (uint8_t)(set_value & 0x00ff);
			aefm_RsetInfo->efm_remote_set_addr_len = 2;
			break;

		case  hpmcrtOltPtFlowCtrl:
			if(set_value>1)
			{
				return -1;
			}
			aefm_RsetInfo->efm_remote_set_start_addr =\
								  efm_remote_set_Olt1Flow;
			aefm_RsetInfo->efm_remote_set_value[aefm_RsetInfo->efm_remote_set_start_addr] = set_value&0x01;
			aefm_RsetInfo->efm_remote_set_addr_len = 1;
			break;

		default:
			return  -1;


	}
	
	return ret;
}


