#include <string.h>
#include <lib/zassert.h>
#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <ifm/ifm.h>
#include "vlan.h"
#include "qinq.h"
#include "mac_static.h"
#include "l2_if.h"
#include "lldp/lldp_tx_sm.h"
#include "lldp/lldp_rx_sm.h"
#include "lldp/lldp.h"

#include "loopd/loop_detect.h"
#include "mstp/mstp.h"
#include "mstp/mstp_sm.h"
#include "mstp/mstp_port.h"
#include "mstp/mstp_base_procedure.h"
#include "mstp/mstp_base_procedure.h"
#include "mstp/mstp_init.h"

#include "trunk.h"
#include "efm/efm.h"
#include "efm/efm_link_monitor.h"
#include "efm/efm_agent.h"
#include "l2_msg.h"

struct hash_table l2if_table;
unsigned int devtype=0;

int vlanlist_sort(void* nvlan, void* ovlan)
{
	if((uint32_t)nvlan > (uint32_t)ovlan)
		return -1;
	else
		return 0;
}

/************************************************
 * Add by jhz,20180313,
 * Function: vlantag_list_sort
 * Description:
 *      Compare functions for list sorting.
 ************************************************/
int vlantag_list_sort(void* nvlan, void* ovlan)
{
	if(((struct vlan_tagged *)nvlan)->vlanid < ((struct vlan_tagged *)ovlan)->vlanid)
		return -1;
	else
		return 0;
}

static unsigned int compute_hash ( void *hash_key )
{
	 L2_COMMON_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
     return (uint32_t)hash_key;
}

static int compare ( void *item, void *hash_key )
{
    struct hash_bucket *pbucket = item;

    if ( NULL == item )
    {
        return ERRNO_FAIL;
    }

    if (pbucket->hash_key == hash_key)
    {
        return ERRNO_SUCCESS;
    }
    else
    {
		return ERRNO_FAIL; 
	}
}

void l2if_table_init ( int size )
{
	int i = 0;
    hios_hash_init ( &l2if_table, size, compute_hash, compare );

	for( i=0; i<3; i++ )
	{
		trunk_reserve_vlan_table[i].trunkid = 0;
		trunk_reserve_vlan_table[i].reserve_vlan = DEF_RESV_VLAN7 + i;
		trunk_reserve_vlan_table[i].reserve_vlan_bak = DEF_RESV_VLAN7 + i;
		trunk_reserve_vlan_table[i].flag = 0;
	}
}

int l2if_add(struct l2if *pif)
{
	int ret = 0;
	struct hash_bucket *p_hash_bucket = NULL;

	L2_COMMON_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

	if (NULL == pif)					//预防操作空指针
	{
		L2_COMMON_LOG_DBG("%s[%d]:%s error:can not input NULL\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}
	
	ret = l2_msg_send_hal_wait_ack(pif, sizeof(struct l2if), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, L2IF_INFO_INVALID, IPC_OPCODE_ADD, pif->ifindex);
	if(ret)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send l2if_add msg to hal\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}
	else
	{		
		p_hash_bucket = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
		if (NULL == p_hash_bucket)
		{
			zlog_err("%s[%d]:%s: error:fail XCALLOC struct hash_bucket\n",__FILE__,__LINE__,__func__);
			return ERRNO_FAIL;
		}
		p_hash_bucket->data = pif;
		p_hash_bucket->hash_key = (void*)pif->ifindex;

		ret = hios_hash_add(&l2if_table, p_hash_bucket);
		if(ret < 0)
		{
			L2_COMMON_LOG_DBG("%s[%d]:%s:hios_hash_add error\n",__FILE__,__LINE__,__func__);
			XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
			return ERRNO_FAIL;
		}
		else
		{
			L2_COMMON_LOG_DBG("%s[%d]:leave %s:successfully add node to l2if_table,ifindex = %#x\n",__FILE__,__LINE__,__func__,(uint32_t)p_hash_bucket->hash_key);
			return ERRNO_SUCCESS;
		}	
	}
}


int l2if_delete(uint32_t ifindex)
{
	int ret = 0;
	int i = 0;
	struct hash_bucket *p_hash_bucket = NULL;
	struct l2if *pif = NULL;
	
	L2_COMMON_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
	
	if (0 == ifindex)					
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s: error:ifindex == 0\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}
	
	p_hash_bucket = hios_hash_find(&l2if_table, (void*)ifindex);
	if(NULL == p_hash_bucket)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s: error:invalid ifindex\n",__FILE__,__LINE__,__func__);
		return ERRNO_SUCCESS;
	}

	pif = (struct l2if *)p_hash_bucket->data;
	
	/* trunk 成员删除 */
	if(pif->trunkid)
	{
		if(IFM_TYPE_IS_TRUNK(pif->ifindex))
		{
			trunk_delete(IFM_TRUNK_ID_GET(pif->ifindex));/*聚合口处理*/
		}
		else
		{
			trunk_port_delete(pif->trunkid, pif->ifindex);
		}
	}

	/*clean lldp port */
	if(pif->lldp_port_info != NULL)
	{	
		lldp_clean_port(pif->lldp_port_info);
		if(pif->lldp_port_info != NULL)
		{
			pif->lldp_port_info = NULL;
		}		
	}
	/*clean mstp port*/ 	
	if(pif->mstp_port_info != NULL)
	{			
		mstp_clean_mstp_port(pif->mstp_port_info);
		pif->mstp_port_info = NULL;
	}
         if(pif!=NULL || pif->pefm !=NULL)
         {
                efm_protocol_disable (pif);
		pif->pefm = NULL;
	}
	if(pif!=NULL||pif->ploopdetect !=NULL)
	{
		     loopdetect_clean_port(pif);
		     pif->ploopdetect = NULL;         
	}

	 /*Remove the interface from the VLAN port-list*/
	if((IFM_TYPE_IS_TRUNK(ifindex))||IFM_TYPE_IS_METHERNET(ifindex))
	{
		l2if_trunk_delete_vlan(ifindex); 
	}

	/*Delete the static mac configuration by ifindex*/
	if(IFM_TYPE_IS_TRUNK(ifindex))
	{
		//mac_static_delete_by_ifindex(ifindex);
        mac_static_delete_all_by_ifindex(ifindex);      //bug#59172
	}

	/*for 15X trunk l3*/
	if((IFM_TYPE_IS_TRUNK(ifindex)) && (!IFM_IS_SUBPORT(ifindex)))
	{
		if(devtype == ID_HT157 || devtype == ID_HT158 )
		{
			for(i =0 ;i < 3; i++)
			{
				if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(ifindex))
				{
					trunk_reserve_vlan_table[i].flag = 0;
					break;
				}
			}
		}
	}

	if(NULL != pif)
	{
		/*删除trunk接口或物理口，hal根据接口索引查找接口失败，必定超时*/
		if((!IFM_TYPE_IS_TRUNK(ifindex))&&(!IFM_TYPE_IS_METHERNET(ifindex)))
		{
			//ret = ipc_send_hal_wait_ack(pif, sizeof(struct l2if), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_INVALID, IPC_OPCODE_DELETE, ifindex);
			ret = l2_msg_send_hal_wait_ack(pif, sizeof(struct l2if), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_INVALID, IPC_OPCODE_DELETE, ifindex);
			if(ret)
			{
				L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send l2if_delete msg to hal\n",__FILE__,__LINE__,__func__);
				return ERRNO_FAIL;     
			}
			else
			{
				L2_COMMON_LOG_DBG("%s[%d]:%s:send delete node msg to hal\n",__FILE__,__LINE__,__func__);
			
				XFREE(MTYPE_L2, pif);
				pif = NULL;
			}
		}
		else
		{
			XFREE(MTYPE_L2, pif);
			pif = NULL;
		}
	}

	hios_hash_delete(&l2if_table, p_hash_bucket);	
	XFREE(MTYPE_HASH_BACKET, p_hash_bucket);

	L2_COMMON_LOG_DBG("%s[%d]:leave %s:successfully delete node to l2if_table\n",__FILE__,__LINE__,__func__);
	return ERRNO_SUCCESS;
}


struct l2if *l2if_lookup(uint32_t ifindex)
{
	struct hash_bucket *p_hash_bucket = NULL;

	L2_COMMON_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

	p_hash_bucket = hios_hash_find(&l2if_table, (void*)ifindex);
	if (NULL == p_hash_bucket)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s: error: find NULL from l2if_table\n",__FILE__,__LINE__,__func__);
		return NULL;
	}
	else
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:successfully find node from l2if_table\n",__FILE__,__LINE__,__func__);
		return (struct l2if *)p_hash_bucket->data;
	}
}


/* 创建 l2if 接口 */
struct l2if * l2if_create(uint32_t ifindex)
{
	int i = 0;
	int ret = 0;
	struct l2if *pif = NULL;
	struct ifm_info pifm= {0};
	struct trunk *trunk_entry = NULL;
	uint8_t rleg_ena = TRUE;
	
	/* 获取接口信息 */	
	if(ifm_get_all_info(ifindex, MODULE_ID_L2, &pifm) != 0) return NULL;	

	/* 分配内存 */
	pif = (struct l2if *)XCALLOC(MTYPE_IF, sizeof(struct l2if));
	if(pif == NULL)
	{
		zlog_err("%-15s[Func:%s]:Fail to malloc for new mpls_if.--Line:%d",__FILE__,__func__,__LINE__);
		
		return NULL;
	}
	pif->ifindex = ifindex;
	pif->mode = pifm.mode;
	pif->down_flag = pifm.status;
	pif->trunkid = pifm.trunkid;
	memcpy(pif->mac, pifm.mac, MAC_LEN);
	memset(pif->alias, 0, IFM_ALIAS_STRING_LEN+1);
	
	/*创建trunk数据结构*/
	if((IFM_TYPE_IS_TRUNK(ifindex)) && (!IFM_IS_SUBPORT(ifindex)))
	{
		trunk_entry = trunk_add(IFM_TRUNK_ID_GET(ifindex));
		if(trunk_entry != NULL)
		{
			pif->trunkid = trunk_entry->trunkid;
		}
		else
		{
			XFREE(MTYPE_IF, pif);
			L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to trunk_add ! \n",__FILE__,__LINE__,__func__);
			return NULL;
		}

		/*for 15X trunk l3*/
		if(devtype == ID_HT157 || devtype == ID_HT158 )
		{
			for(i =0 ;i < 3; i++)
			{
				if(trunk_reserve_vlan_table[i].flag == 0)
				{
					trunk_reserve_vlan_table[i].trunkid = trunk_entry->trunkid;
					//ret=ipc_send_hal(&trunk_reserve_vlan_table[i].reserve_vlan,sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_RESERVE_VLAN, IPC_OPCODE_UPDATE, ifindex);
					ret=ipc_send_msg_n2(&trunk_reserve_vlan_table[i].reserve_vlan,sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_RESERVE_VLAN, IPC_OPCODE_UPDATE, ifindex);
					if(ret)
					{
				        zlog_err("%s[%d]:leave %s:error:fail to send default reserve vlan msg to hal\n",__FILE__,__LINE__,__FUNCTION__);
						return NULL;
					}

					//ret=ipc_send_hal(&rleg_ena,sizeof(uint8_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_VLAN_RLEG, IPC_OPCODE_UPDATE, ifindex);
					ret=ipc_send_msg_n2(&rleg_ena,sizeof(uint8_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_VLAN_RLEG, IPC_OPCODE_UPDATE, ifindex);
					if(ret)
					{
						zlog_err("%s[%d]:leave %s:error:fail to send rleg msg to hal\n",__FILE__,__LINE__,__FUNCTION__);
						return NULL;
					}
                    
					trunk_reserve_vlan_table[i].flag = 1;
					pif->switch_info.access_vlan = trunk_reserve_vlan_table[i].reserve_vlan;
					break;
				}
			}
		}
	}

	/*初始化完成，加入hash*/
	if(ERRNO_FAIL == l2if_add(pif))
	{
		XFREE(MTYPE_IF, pif);
		return NULL;
	}

	l2if_init_switch( &(pif->switch_info),pif->ifindex);

	lldp_interface_admin_status_enable(pif);

	L2_COMMON_LOG_DBG("%-15s[Func:%s]:Leaving.--Line:%d", __FILE__, __func__, __LINE__);
	return pif;
}


/* 查找接口不存在则创建接口，只能在配置接口下的命令行时调用 */
struct l2if * l2if_get(uint32_t ifindex)
{
	struct l2if *p_l2if = NULL;

	p_l2if = l2if_lookup(ifindex);
	if(p_l2if == NULL)
	{
		p_l2if = l2if_create(ifindex);
	}

	return p_l2if;
}


/* 收到接口 down 事件*/
void l2if_down(uint32_t ifindex)
{
	unsigned int		counter = 0;
	struct l2if 		*pif = NULL;
	struct mstp_port	*mstp_port = NULL;

	pif = l2if_lookup(ifindex);
	if(pif == NULL)
		return;

	if(pif->down_flag == IFNET_LINKDOWN)
		return;
	
	pif->down_flag = IFNET_LINKDOWN;

	/*lldp port state link down*/
	if(pif->lldp_port_info != NULL)
	{
		pif->lldp_port_info->portEnabled = FALSE;

		/* add tx/rx fsm action*/
		lldp_rx_state_machine_action(pif->lldp_port_info);
		lldp_tx_state_machine_action(pif->lldp_port_info);
		lldp_if_port_event(pif, LLDP_FALSE);
	}

	if(pif->mstp_port_info != NULL)
	{
#if 0
		mstp_link_change_sm_action(pif->mstp_port_info);
#else
		mstp_port = pif->mstp_port_info;
		mstp_update_link_state(mstp_port);

		for(counter = 0; counter < TOPOLOGY_CHANGE_CALC_TIME; counter++)
		{
			mstp_state_machine_normal_action(mstp_port->m_br);
		}

		mstp_send_bpdu();

#endif
	}

	/* trunk 成员 down */
	if(pif->trunkid)
	{
		if(!IFM_TYPE_IS_TRUNK(pif->ifindex))
		{
			trunk_port_down(pif->trunkid, pif->ifindex);
		}
	}

	if (pif->pefm)
	{
                 
		efm_process_local_event(pif->pefm, EFM_LINK_FAULT_EVENT, 1);
		/*clear efm agent data*/
		if(pif->pefm->aefm)
		{	
			if(EFM_TRUE == pif->pefm->aefm->efm_agent_link_flag)
			{
				efm_agent_u0_info_send(pif->pefm->if_index,pif->pefm->aefm,IPC_OPCODE_DELETE);
				pif->pefm->aefm->efm_agent_link_flag = EFM_FALSE;
				clear_efm_agent_data(pif->pefm->aefm);
			}	
		}
	}
	return;
}


/* 收到接口 up 事件*/
void l2if_up(uint32_t ifindex)
{
	unsigned int		counter = 0;
	struct l2if 		*pif = NULL;
	struct mstp_port	*mstp_port = NULL;

	pif = l2if_lookup(ifindex);	
	if(pif == NULL)
		return;
	
	if(pif->down_flag == IFNET_LINKUP)
		return;
	
	pif->down_flag = IFNET_LINKUP;

	/*lldp port state link down*/
	if(pif->lldp_port_info != NULL)
	{
		pif->lldp_port_info->portEnabled = TRUE;

		/* add tx/rx fsm action*/	
		lldp_rx_state_machine_action(pif->lldp_port_info);
		lldp_tx_state_machine_action(pif->lldp_port_info);

		lldp_if_port_event(pif, LLDP_TRUE);
	}

	
	if(pif->mstp_port_info != NULL)
	{
#if 0
		mstp_link_change_sm_action(pif->mstp_port_info);
#else
		mstp_port = pif->mstp_port_info;
		mstp_update_link_state(mstp_port);
	
		for(counter = 0; counter < TOPOLOGY_CHANGE_CALC_TIME; counter++)
		{
			mstp_state_machine_normal_action(mstp_port->m_br);
		}

	
		mstp_send_bpdu();	
#endif
		}
	
	/* trunk 成员 up */
	if(pif->trunkid)
	{
		if(!IFM_TYPE_IS_TRUNK(pif->ifindex))
		{
			trunk_port_up(pif->trunkid, pif->ifindex);
		}
	}

	if (pif->pefm)
	{
		efm_process_local_event(pif->pefm, EFM_LINK_FAULT_EVENT, 0);
	}
	return;
}

void lldp_subif_port_event(struct l2if *pif, uint32_t ifindex, uint8_t flag)
{
	struct lldp_msap *msap_cache = NULL;

	if (!pif || !pif->lldp_port_info)
	{
		return;
	}

	if(0 == pif->lldp_port_info->msap_num)
	{
		return;
	}

	if (!CHECK_FLAG(pif->lldp_port_info->arp_nd_learning, LLDP_LEARNING_ARP))
	{
		return;
	}

	msap_cache = pif->lldp_port_info->msap_cache;
			 						
	while(msap_cache != NULL)
	{
		if (flag)//up
		{
			/*add arp info*/
			if ((LLDP_FALSE == msap_cache->arp_notice_flag) && 
				(ifindex == msap_cache->pkt_ifindex) &&
				/*(pif->lldp_port_info->arp_nd_learning_vlan == msap_cache->svlan) &&*/
				(msap_cache->neighbor_msg.mgmt_addr_sub_type == 1))
			{
				lldp_arp_info_notice(msap_cache, pif->lldp_port_info->arp_nd_learning_vlan, 
											pif->lldp_port_info->if_index, LLDP_TRUE);
				msap_cache->arp_notice_flag = LLDP_TRUE;
			}
		}
		else
		{
			/*delete arp info*/
			if ((LLDP_TRUE == msap_cache->arp_notice_flag) && 
				(ifindex == msap_cache->pkt_ifindex))
			{
				lldp_arp_info_notice(msap_cache, pif->lldp_port_info->arp_nd_learning_vlan, 
										pif->lldp_port_info->if_index, LLDP_FALSE);
				msap_cache->arp_notice_flag = LLDP_FALSE;
			}
		}
		
		msap_cache = msap_cache->next;
	}

	return;
}

void lldp_if_port_event(struct l2if *pif, uint8_t flag)
{
	struct lldp_msap *msap_cache = NULL;

	if (!pif || !pif->lldp_port_info)
	{
		return;
	}

	if(0 == pif->lldp_port_info->msap_num)
	{
		return;
	}

	if (!CHECK_FLAG(pif->lldp_port_info->arp_nd_learning, LLDP_LEARNING_ARP))
	{
		return;
	}

	msap_cache = pif->lldp_port_info->msap_cache;
			 						
	while(msap_cache != NULL)
	{
		if (flag)//up
		{
			/*add arp info*/
			if ((LLDP_FALSE == msap_cache->arp_notice_flag) &&
				/*(pif->lldp_port_info->arp_nd_learning_vlan == msap_cache->svlan) &&*/
				(msap_cache->neighbor_msg.mgmt_addr_sub_type == 1))
			{
				lldp_arp_info_notice(msap_cache, pif->lldp_port_info->arp_nd_learning_vlan, 
											pif->lldp_port_info->if_index, LLDP_TRUE);
				msap_cache->arp_notice_flag = LLDP_TRUE;
			}
		}
		else
		{
			/*delete arp info*/
			if (LLDP_TRUE == msap_cache->arp_notice_flag)
			{
				lldp_arp_info_notice(msap_cache, pif->lldp_port_info->arp_nd_learning_vlan, 
										pif->lldp_port_info->if_index, LLDP_FALSE);
				msap_cache->arp_notice_flag = LLDP_FALSE;
			}
		}
		
		msap_cache = msap_cache->next;
	}

	return;
}


/* 接口 mode 改变事件处理 */
void l2if_mode_change(uint32_t ifindex, uint32_t mode)
{
	struct l2if *pif= NULL;
	uint8_t rleg_ena = 0;
	int ret = 0;
	uint32_t reserve_vlanid = 0;
	uint8_t port = 0;
	int i = 0;
	
	pif = l2if_get(ifindex);
	if(NULL == pif)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to l2if_get ! \n",__FILE__,__LINE__,__func__);
		return;
	}

	/*clean*/
	if(mode == IFNET_MODE_INVALID)
	{
		/*clean lldp port */
		if(pif->lldp_port_info != NULL)
		{
			L2_COMMON_LOG_DBG("%s[%d]:leave %s: interface[%d] mode change! \n",__FILE__,__LINE__,__func__, ifindex);

			lldp_clean_port(pif->lldp_port_info);
			if(pif->lldp_port_info != NULL)
			{
				pif->lldp_port_info = NULL;
			}
		}
	
		/*clean mstp port*/	
		if(pif->mstp_port_info != NULL)
		{			
			mstp_clean_mstp_port(pif->mstp_port_info);
			pif->mstp_port_info = NULL;
		}
		if(pif->ploopdetect !=NULL)
		{
		     loopdetect_clean_port(pif);
		     pif->ploopdetect = NULL;         
		}
	}

	if(IFNET_MODE_L3 == mode)
	{
		/*clean mstp port*/		
		if(pif->mstp_port_info != NULL)
		{			
			mstp_clean_mstp_port(pif->mstp_port_info);
			pif->mstp_port_info = NULL;
		}
	}

	if(pif->mode == mode)
		return;
	
	/* l2 模式改变时，删除 swtich 配置 */
	if(pif->mode == IFNET_MODE_SWITCH)
	{
		l2if_delete_switch(pif);
		/*接口加入trunk，继承trunk的vlan属性*/
		if((pif->trunkid)&&IFM_TYPE_IS_METHERNET(ifindex))
		{
			l2if_trunk_add_port(pif->trunkid,ifindex);
		}
		//mac_static_delete_by_ifindex(ifindex);
        mac_static_delete_all_by_ifindex(ifindex);      //bug#59172
	}

	if (mode != IFNET_MODE_L3)
	{
		//not support lldp arp learning
		lldp_if_port_event(pif, LLDP_FALSE);
		if (pif->lldp_port_info)
		{
			UNSET_FLAG(pif->lldp_port_info->arp_nd_learning, LLDP_LEARNING_ARP);
			pif->lldp_port_info->arp_nd_learning_vlan = 0;
		}
	}

if(devtype == ID_HT157 || devtype == ID_HT158)
{
	if( pif->mode == IFNET_MODE_L3)
	{
		if(mode != IFNET_MODE_L3)
		{
			rleg_ena = FALSE;
		//	ret=ipc_send_hal(&rleg_ena,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_VLAN_RLEG, IPC_OPCODE_UPDATE, ifindex);
			ret=ipc_send_msg_n2(&rleg_ena,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_VLAN_RLEG, IPC_OPCODE_UPDATE, ifindex);
			if(ret)
			{
				zlog_err("%s[%d]:leave %s:error:fail to send rleg msg to hal\n",__FILE__,__LINE__,__FUNCTION__);
				return;
			}
		}

        //set physical port pvid to trunk reserved pvid, when add port to trunk
        if((pif->trunkid) && IFM_TYPE_IS_METHERNET(ifindex))
		{
            l2if_trunk_add_port(pif->trunkid, ifindex);
		}
	}
	else
	{
		if(mode == IFNET_MODE_L3)
		{
			rleg_ena = TRUE;
			
			if(IFM_TYPE_IS_TRUNK(ifindex))
			{
				for(i=0;i<3;i++)
				{
					if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(ifindex))
					{
						reserve_vlanid = trunk_reserve_vlan_table[i].reserve_vlan_bak;
					}
				}
			}
			else{
				port = IFM_PORT_ID_GET ( ifindex );
				switch(port)
				{
					case 1: reserve_vlanid = DEF_RESV_VLAN1; break;
					case 2: reserve_vlanid = DEF_RESV_VLAN2; break;
					case 3: reserve_vlanid = DEF_RESV_VLAN3; break;
					case 4: reserve_vlanid = DEF_RESV_VLAN4; break;
					case 5: reserve_vlanid = DEF_RESV_VLAN5; break;
					case 6: reserve_vlanid = DEF_RESV_VLAN6; break;
					default: break;
				}
			}			

			if(reserve_vlanid == 0)
			{
				zlog_err("%s[%d]:leave %s:reserve_vlanid is 0\n",__FILE__,__LINE__,__FUNCTION__);
				return;
			}
			
			vlan_delete(reserve_vlanid,reserve_vlanid);
			
			//ret=ipc_send_hal(&reserve_vlanid,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_RESERVE_VLAN, IPC_OPCODE_UPDATE, ifindex);
			ret=ipc_send_msg_n2(&reserve_vlanid,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_RESERVE_VLAN, IPC_OPCODE_UPDATE, ifindex);
			if(ret)
			{
		        zlog_err("%s[%d]:leave %s:error:fail to send default reserve vlan msg to hal\n",__FILE__,__LINE__,__FUNCTION__);
				return;
			}
			
			//ret=ipc_send_hal(&rleg_ena,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_VLAN_RLEG, IPC_OPCODE_UPDATE, ifindex);
			ret=ipc_send_msg_n2(&rleg_ena,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_VLAN_RLEG, IPC_OPCODE_UPDATE, ifindex);
			if(ret)
			{
				zlog_err("%s[%d]:leave %s:error:fail to send rleg msg to hal\n",__FILE__,__LINE__,__FUNCTION__);
				return;
			}
			
			if(IFM_TYPE_IS_TRUNK(ifindex))
			{
				trunk_reserve_vlan_table[i].reserve_vlan = reserve_vlanid;
			}
			else{
				reserve_vlan_table[port - 1] = reserve_vlanid;
			}
			
		}
	}
}
	pif->mode = mode;
	
	return;
}

void l2if_speed_change(uint32_t ifindex)
{
	struct l2if			*pif = NULL;
	struct mstp_port	*mstp_port = NULL;

	/*get l2 interface(physical/trunk)*/
	pif = l2if_get(ifindex);
	if(NULL == pif)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to l2if_get ! \n",__FILE__,__LINE__,__func__);
		return;
	}
	
	/*judge interface mode	(now just physical port)*/
	if((IFNET_MODE_INVALID != pif->mode) && (IFNET_MODE_L3 != pif->mode))
	{
		/*judge mstp port*/
		mstp_port = pif->mstp_port_info;
		if(mstp_port != NULL)
		{
			/*update speed then get speed*/
			/*need to get real speed*/
			mstp_update_link_state(mstp_port);		//FIXME

			/*recalculate*/
			mstp_reinit();
		}
	}

	/*if trunk deal this, please add here*/
	/*
	1.	if( physical port belong to trunk)
		{
			deal / modify trunk speed
		}
	*/
}
/* 删除 switch 配置 */
void l2if_delete_switch(struct l2if *pif)
{
	int i,port = 0;
	/*删除switch mode*/
	if(SWITCH_MODE_HYBRID != pif->switch_info.mode)
	{
		pif->switch_info.mode = SWITCH_MODE_HYBRID;
	}
	
	/*删除access vlan*/
	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		if(IFM_TYPE_IS_TRUNK(pif->ifindex) && !(IFM_TYPE_IS_SUBPORT(pif->ifindex)))
		{
			for(i = 0; i < 3; i++)
			{
				if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(pif->ifindex))
				{
					if(trunk_reserve_vlan_table[i].reserve_vlan_bak != pif->switch_info.access_vlan)
					{
						l2if_delete_access_vlan(pif);
					}
				}
			}
		}
		else
		{
		    port = IFM_PORT_ID_GET ( pif->ifindex );
		    if( port < 1 || port > 6 )
		    {
		        L2_COMMON_LOG_DBG ("Invalid port number!\n");
		        return ;
		    }
			if((DEF_RESV_VLAN1 + port - 1) != pif->switch_info.access_vlan)
			{
		        l2if_delete_access_vlan(pif);
			}
		}
	}
	else
	{
		if(DEFAULT_VLAN_ID != pif->switch_info.access_vlan)
		{
			l2if_delete_access_vlan(pif);
		}
	}
		
	/*删除switch vlan*/
	if(NULL != pif->switch_info.vlan_list->head)
	{
		l2if_clear_switch_vlan(pif);
	}

	if(IFM_TYPE_IS_METHERNET(pif->ifindex))
	{
		/*删除qinq*/
		if(NULL != pif->switch_info.qinq_list->head)
		{
			l2if_clear_qinq(pif);
		}
	
		/*删除dot1q tunnel*/
		if(DEFAULT_VLAN_ID != pif->switch_info.dot1q_tunnel)
		{
			dot1q_tunnel_delete(pif);
		}
	}	
	if(pif->switch_info.limit_num!=0)
	{
		l2if_clear_mac_limit(pif);
	}
	if((ENABLE==pif->switch_info.storm_control.storm_unicast)||(ENABLE==pif->switch_info.storm_control.storm_broadcast)||
			(ENABLE==pif->switch_info.storm_control.storm_multicast))
	{
		l2if_clear_storm_suppress(pif);
	}
}



/* 初始化 switch 数据结构 */
int l2if_init_switch(struct ifm_switch  *pswitch,uint32_t ifindex)
{
	int i = 0;
	uint8_t port = 0;
	
	pswitch->vlan_list = list_new();
	pswitch->vlan_list->cmp = vlanlist_sort;
	pswitch->vlan_list->head = NULL;
	pswitch->vlan_list->tail= NULL;
	//Edit by jhz,20180313,add vlan_tag_list init
	pswitch->vlan_tag_list = list_new();
	pswitch->vlan_tag_list->cmp = vlantag_list_sort;
	pswitch->vlan_tag_list->head = NULL;
	pswitch->vlan_tag_list->tail= NULL;
	pswitch->qinq_list = list_new();
	pswitch->qinq_list->head = NULL;
	pswitch->qinq_list->tail= NULL;
	pswitch->dot1q_cos = 8;
	pswitch->dot1q_tunnel = DEFAULT_VLAN_ID;
	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		if( pswitch->access_vlan == 0 )
		{
			if(IFM_TYPE_IS_TRUNK(ifindex) && !(IFM_TYPE_IS_SUBPORT(ifindex)))
			{
				for(i = 0; i < 3; i++)
				{
					if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(ifindex))
					{
						pswitch->access_vlan = trunk_reserve_vlan_table[i].reserve_vlan_bak;
					}
				}
			}
			else
			{
			    port = IFM_PORT_ID_GET ( ifindex );
			    if( port < 1 || port > 6 )
			    {
			        L2_COMMON_LOG_DBG ("Invalid port number!\n");
			        return -1;
			    }
				pswitch->access_vlan = (DEF_RESV_VLAN1 + port - 1);
			}
		}
	}
	else{
		pswitch->access_vlan= DEFAULT_VLAN_ID;
	}
	pswitch->limit_num=0;
	return 0;
}


int l2if_get_switch_info_bulk(uint32_t ifindex, struct l2if entry_buff[])
{
	struct l2if *pif = NULL;
	struct hash_bucket *pbucket = NULL;
	int cursor;
	int data_num = 0;
	int msg_num  = IPC_MSG_LEN/sizeof(struct l2if);
	int flag=0;

	/*The first query*/
	if(0==ifindex)
	{
		HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
		{
			pif =(struct l2if *) pbucket->data;
			if(pif->mode!=IFNET_MODE_SWITCH)
				continue;
			memcpy(&entry_buff[data_num++],pif,sizeof(struct l2if));
			if (data_num == msg_num)
			{
			 	return data_num;
			}
		}
	}
	else
	{
		HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
		{
			pif =(struct l2if *) pbucket->data;
			if(0==flag)
			{
				if(ifindex == pif->ifindex)
				{
					flag=1;
				}
				continue;
			}
			else
			{
				if(pif->mode!=IFNET_MODE_SWITCH)
					continue;
				memcpy(&entry_buff[data_num++],pif,sizeof(struct l2if));
				if (data_num == msg_num)
				{
					flag=0;
				 	return data_num;
				}
			}
		}
		flag=0;
	}
	return data_num;

}


/*批量回应查询接口的switch_info信息*/
int l2if_reply_switch_info_bulk(struct ipc_msghdr_n *phdr, uint32_t ifindex)
{
	int msg_num  = IPC_MSG_LEN/sizeof(struct l2if);
	struct l2if entry_buff[IPC_MSG_LEN/sizeof(struct l2if)];
	//uint32_t ifindex;
	int ret;

	//ifindex=phdr->msg_index;
	memset(entry_buff, 0, msg_num*sizeof(struct l2if));
	
	ret=l2if_get_switch_info_bulk(ifindex,entry_buff);
	if(ret>0)
	{
		//ret = ipc_send_reply_bulk(entry_buff, ret*sizeof(struct l2if), ret, phdr->sender_id,
	      //                  phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
		ret = ipc_send_reply_n2(entry_buff, ret*sizeof(struct l2if), ret, phdr->sender_id,
	                        phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0,phdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
		//ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, phdr->module_id, phdr->msg_type,
	      //                  phdr->msg_subtype, phdr->msg_index);
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
	                        phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0,0, IPC_OPCODE_NACK);
	}
	return 0;
}

/*批量回应查询接口的switch_vlan信息*/
int l2if_reply_switch_vlan_bulk(struct ipc_msghdr_n *phdr, uint32_t ifindex)
{
	struct l2if *p_if_l2 = NULL;
	struct listnode *p_listnode = NULL;
	u_char msg_send[512];
	void *pdata = NULL;
	int ret=-1;
	uint32_t vlan_id;
	int pos = 0;
	//uint32_t ifindex;
	int i,port = 0;

	//ifindex = phdr->msg_index;
	p_if_l2 = l2if_lookup(ifindex);
	if(NULL == p_if_l2)
	{
		ret=0;
	}

	memset(msg_send, 0, sizeof(msg_send));

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		if(IFM_TYPE_IS_TRUNK(ifindex) && !(IFM_TYPE_IS_SUBPORT(ifindex)))
		{
			for(i = 0; i < 3; i++)
			{
				if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(ifindex))
				{
					if((trunk_reserve_vlan_table[i].reserve_vlan_bak == p_if_l2->switch_info.access_vlan)&&(listhead(p_if_l2->switch_info.vlan_list) == NULL))
					{
						ret=0;
					}
					if(trunk_reserve_vlan_table[i].reserve_vlan_bak != p_if_l2->switch_info.access_vlan)  
					{
						vlan_id = p_if_l2->switch_info.access_vlan;
						pos = 0x01FF & (vlan_id / 8);
						msg_send[pos] = msg_send[pos] | (0x80 >> (vlan_id % 8));	
					}
				}
			}
		}
		else
		{
		    port = IFM_PORT_ID_GET ( ifindex );
		    if( port < 1 || port > 6 )
		    {
		        L2_COMMON_LOG_DBG ("Invalid port number!\n");
		        return -1;
		    }
			if(((DEF_RESV_VLAN1 + port - 1) == p_if_l2->switch_info.access_vlan)&&(listhead(p_if_l2->switch_info.vlan_list) == NULL))
			{
				ret=0;
			}
			if((DEF_RESV_VLAN1 + port - 1) != p_if_l2->switch_info.access_vlan)  
			{
				vlan_id = p_if_l2->switch_info.access_vlan;
				pos = 0x01FF & (vlan_id / 8);
				msg_send[pos] = msg_send[pos] | (0x80 >> (vlan_id % 8));	
			}
		}
	}
	else
	{
		if((DEFAULT_VLAN_ID == p_if_l2->switch_info.access_vlan)&&(listhead(p_if_l2->switch_info.vlan_list) == NULL))
		{
			ret=0;
		}

		if(DEFAULT_VLAN_ID != p_if_l2->switch_info.access_vlan)  
		{
			vlan_id = p_if_l2->switch_info.access_vlan;
			pos = 0x01FF & (vlan_id / 8);
			msg_send[pos] = msg_send[pos] | (0x80 >> (vlan_id % 8));	
		}
	}
	/*接口下没有配置vlan*/
	if(ret==0)
	{
		/*ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, phdr->module_id, phdr->msg_type,
	                        phdr->msg_subtype, phdr->msg_index);*/
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, phdr->module_id, 
						phdr->msg_type, phdr->msg_subtype, 0,0, IPC_OPCODE_NACK);
							
		return 0;
	}	
	
	if(listhead(p_if_l2->switch_info.vlan_list) != NULL)
	{
		for(ALL_LIST_ELEMENTS_RO(p_if_l2->switch_info.vlan_list, p_listnode, pdata))
		{
			vlan_id = (uint32_t)pdata;
			pos = 0x01FF & (vlan_id / 8);
			msg_send[pos] = msg_send[pos] | (0x80 >> (vlan_id % 8));
		}
	}
	
	//ret = ipc_send_reply ( msg_send, 512, phdr->sender_id, phdr->module_id,phdr->msg_type, phdr->msg_subtype, phdr->msg_index );
	ret = ipc_send_reply_n2( msg_send, 512,1, phdr->sender_id, phdr->module_id,phdr->msg_type, phdr->msg_subtype,0, phdr->msg_index , IPC_OPCODE_REPLY);
	if(ret < 0)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:ipc_send_reply ! \n",__FILE__,__LINE__,__func__);
		return -1;
	}
	
	return 0;
}

/*return vlan tag/unag list*/
int l2if_reply_switch_tag_vlan_bulk(struct ipc_msghdr_n *phdr, uint32_t ifindex)
{
	struct l2if *p_if_l2 = NULL;
	struct listnode *p_listnode = NULL;
	u_char msg_send[512];
	void *pdata = NULL;
	int ret=-1;
	uint32_t vlan_id;
	int pos = 0;
	//uint32_t ifindex;
	int i,port = 0;
	struct vlan_tagged *pvlan_tagged = NULL;

	//ifindex = phdr->msg_index;
	p_if_l2 = l2if_lookup(ifindex);
	if(NULL == p_if_l2)
	{
		ret=0;
	}

	memset(msg_send, 0, sizeof(msg_send));

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		if(IFM_TYPE_IS_TRUNK(ifindex) && !(IFM_TYPE_IS_SUBPORT(ifindex)))
		{
			for(i = 0; i < 3; i++)
			{
				if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(ifindex))
				{
					if((trunk_reserve_vlan_table[i].reserve_vlan_bak == p_if_l2->switch_info.access_vlan)&&(listhead(p_if_l2->switch_info.vlan_list) == NULL))
					{
						ret=0;
					}
					if(trunk_reserve_vlan_table[i].reserve_vlan_bak != p_if_l2->switch_info.access_vlan)  
					{
						vlan_id = p_if_l2->switch_info.access_vlan;
						pos = 0x01FF & (vlan_id / 8);
						msg_send[pos] = msg_send[pos] | (0x80 >> (vlan_id % 8));
					}
				}
			}
		}
		else
		{
		    port = IFM_PORT_ID_GET ( ifindex );
		    if( port < 1 || port > 6 )
		    {
		        L2_COMMON_LOG_DBG ("Invalid port number!\n");
		        return -1;
		    }
			if(((DEF_RESV_VLAN1 + port - 1) == p_if_l2->switch_info.access_vlan)&&(listhead(p_if_l2->switch_info.vlan_list) == NULL))
			{
				ret=0;
			}
			if((DEF_RESV_VLAN1 + port - 1) != p_if_l2->switch_info.access_vlan)  
			{
				vlan_id = p_if_l2->switch_info.access_vlan;
				pos = 0x01FF & (vlan_id / 8);
				msg_send[pos] = msg_send[pos] | (0x80 >> (vlan_id % 8));
			}
		}
	}
	else
	{
		if((DEFAULT_VLAN_ID == p_if_l2->switch_info.access_vlan)&&(listhead(p_if_l2->switch_info.vlan_list) == NULL))
		{
			ret=0;
		}

		if(DEFAULT_VLAN_ID != p_if_l2->switch_info.access_vlan)  
		{
			vlan_id = p_if_l2->switch_info.access_vlan;
			pos = 0x01FF & (vlan_id / 8);
			msg_send[pos] = msg_send[pos] | (0x80 >> (vlan_id % 8));
		}
	}
	/*接口下没有配置vlan*/
	if(ret==0)
	{
		/*ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, phdr->module_id, phdr->msg_type,
	                        phdr->msg_subtype, phdr->msg_index);*/
							
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id, phdr->module_id, 
						phdr->msg_type, phdr->msg_subtype, 0,0, IPC_OPCODE_NACK);
		return 0;
	}	
	
	if(listhead(p_if_l2->switch_info.vlan_tag_list) != NULL)
	{
		for(ALL_LIST_ELEMENTS_RO(p_if_l2->switch_info.vlan_tag_list, p_listnode, pdata))
		{
			pvlan_tagged=(struct vlan_tagged*)pdata;
			vlan_id = pvlan_tagged->vlanid;
			pos = 0x01FF & (vlan_id / 8);
			if (pvlan_tagged->tag == TAGGED)
			{
			    msg_send[pos] = msg_send[pos] | (0x80 >> (vlan_id % 8));
			}
		}
	}

	/*ret = ipc_send_reply ( msg_send, 512, phdr->sender_id, phdr->module_id,phdr->msg_type, phdr->msg_subtype, phdr->msg_index );*/
	ret = ipc_send_reply_n2( msg_send, 512,1, phdr->sender_id, phdr->module_id,phdr->msg_type, phdr->msg_subtype,0, phdr->msg_index , IPC_OPCODE_REPLY);
	if(ret < 0)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:ipc_send_reply ! \n",__FILE__,__LINE__,__func__);
		return -1;
	}
	
	return 0;
}

/*删除trunk接口时,trunk配置的vlan中的port_list处理*/
int l2if_trunk_delete_vlan (uint32_t ifindex)
{
	struct l2if *pif = NULL;
	struct listnode *vlan_node = NULL;
	void *data = NULL;
	uint32_t vlanid=0;
	int i,port = 0;
	
	pif = l2if_lookup(ifindex);
	if(NULL == pif)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:invalid ifindex\n",__FILE__,__LINE__,__func__);
		return -1;
	}

	if(listhead(pif ->switch_info.vlan_list) != NULL)
	{
		for(ALL_LIST_ELEMENTS_RO(pif->switch_info.vlan_list, vlan_node, data))
		{	
			vlanid=(uint32_t)data;
			vlan_delete_interface((uint16_t)vlanid,(uint16_t)vlanid,ifindex);
		}
	}

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		if(IFM_TYPE_IS_TRUNK(ifindex) && !(IFM_TYPE_IS_SUBPORT(ifindex)))
		{
			for(i = 0; i < 3; i++)
			{
				if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(ifindex))
				{
					if(trunk_reserve_vlan_table[i].reserve_vlan_bak != pif->switch_info.access_vlan)  
					{
						vlan_delete_interface(pif->switch_info.access_vlan,pif->switch_info.access_vlan,ifindex);
					}
				}
			}
		}
		else
		{
		    port = IFM_PORT_ID_GET ( ifindex );
		    if( port < 1 || port > 6 )
		    {
		        L2_COMMON_LOG_DBG ("Invalid port number!\n");
		        return -1;
		    }
			if((DEF_RESV_VLAN1 + port - 1) != pif->switch_info.access_vlan)  
			{
				vlan_delete_interface(pif->switch_info.access_vlan,pif->switch_info.access_vlan,ifindex);
			}
		}
	}
	else
	{
		if(DEFAULT_VLAN_ID != pif->switch_info.access_vlan)  
		{
			vlan_delete_interface(pif->switch_info.access_vlan,pif->switch_info.access_vlan,ifindex);
		}
	}

	return 0;
}

/*接口下no trunk <1-128>,下发hal删除接口继承的vlan属性*/
int l2if_trunk_delete_port (uint16_t trunkid,uint32_t ifindex)
{
	struct l2if *pif= NULL;
	struct vlan_range send_hal;
	struct vlan_range vlan_send;
	struct listnode *p_listnode = NULL;
	void *pdata = NULL;
	
	uint32_t switch_vlan_count[4094][2];
	uint32_t trunk_ifindex;
	uint16_t vlanid = DEFAULT_VLAN_ID;
	uint16_t id_num = 0;
	uint16_t line_num = 0;
	uint8_t send_num = 1;
	int ret=0;
	int i = 0;
	int j=0;
	
	trunk_ifindex=IFM_TRUNK_IFINDEX_GET(trunkid);
	pif = l2if_lookup(trunk_ifindex);
	if(NULL == pif)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:invalid ifindex\n",__FILE__,__LINE__,__func__);
		return -1;
	}
	
	/*删除access vlan*/
	if(DEFAULT_VLAN_ID != pif->switch_info.access_vlan)
	{
		/*发送access vlanid给hal*/
		//ret = ipc_send_hal_wait_ack(&vlanid, sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_ACCESS_VLAN, IPC_OPCODE_ADD,ifindex);
		ret = l2_msg_send_hal_wait_ack(&vlanid, sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_ACCESS_VLAN, IPC_OPCODE_ADD,ifindex);
		if(ret)
		{
			L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send l2if_set_access_vlan msg to hal\n",__FILE__,__LINE__,__func__);
			return -1;
		}
	}

	/*删除switch vlan*/
	if(NULL != pif->switch_info.vlan_list->head)
	{
		memset(switch_vlan_count,0,sizeof(switch_vlan_count));
		switch_vlan_count[line_num][0] = (uint32_t)listgetdata(pif->switch_info.vlan_list->head);
		switch_vlan_count[line_num][1] = (uint32_t)listgetdata(pif->switch_info.vlan_list->head);

		for(ALL_LIST_ELEMENTS_RO(pif->switch_info.vlan_list, p_listnode, pdata))
		{
			if((uint32_t)pdata == (switch_vlan_count[line_num][1] - 1))
			{
				switch_vlan_count[line_num][1] = (uint32_t)pdata;
			}
			else if((uint32_t)pdata != switch_vlan_count[line_num][0])
			{
				line_num++;
				switch_vlan_count[line_num][0] = (uint32_t)pdata;
				switch_vlan_count[line_num][1] = (uint32_t)pdata;
			}	
		}
	
		/*输出switch vlan*/
		for(i=line_num; i>=0; i--)
		{	
			send_hal.vlan_start = switch_vlan_count[i][1];
			send_hal.vlan_end = switch_vlan_count[i][0];
			
			id_num = send_hal.vlan_end- send_hal.vlan_start + 1;
			if(0 == id_num % 100)
				send_num = id_num / 100;
			else
				send_num = (id_num / 100) + 1;
			
			for(j=0; j<send_num; ++j)
			{
				memset(&vlan_send, 0, sizeof(struct vlan_range));				
				vlan_send.vlan_start = send_hal.vlan_start + j*100;
				if(j== send_num - 1)
					vlan_send.vlan_end = send_hal.vlan_end;
				else
					vlan_send.vlan_end = send_hal.vlan_start+ (j + 1)*100 - 1;
				
				//ret = ipc_send_hal_wait_ack(&vlan_send, sizeof(struct vlan_range), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_SWITCH_VLAN, IPC_OPCODE_DELETE, ifindex);
				ret = l2_msg_send_hal_wait_ack(&vlan_send, sizeof(struct vlan_range), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_SWITCH_VLAN, IPC_OPCODE_DELETE, ifindex);
				if(ret)
				{
					L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send l2if_delete_vlan msg to hal\n",__FILE__,__LINE__,__func__);
					return -1;
				}
			}
		}	
	}
	return 0;
}

//Edit by jhz,20180313,the inherited vlan attribute is modified by vlan_list to vlan_tag_list.
/*接口下trunk <1-128>,接口继承trunk的vlan属性*/
int l2if_trunk_add_port (uint16_t trunkid,uint32_t ifindex)
{
	struct l2if *pif= NULL;
	struct listnode *p_listnode = NULL;
	void *pdata = NULL;
	enum SWITCH_MODE mode;
	
	uint32_t switch_vlan_count[4094][2];
	uint32_t trunk_ifindex;
	uint16_t vlanid;
	uint16_t line_num = 0;
	int ret=0;
	struct vlan_tagged *pvlan_tagged = NULL;
	uint16_t tag_num = 0;
	uint16_t untag_num = 0;
	
	trunk_ifindex=IFM_TRUNK_IFINDEX_GET(trunkid);
	pif = l2if_lookup(trunk_ifindex);
	if(NULL == pif)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:invalid ifindex\n",__FILE__,__LINE__,__func__);
		return -1;
	}
	
	/*继承switch mode*/
	mode=pif->switch_info.mode;
//ret = ipc_send_hal_wait_ack(&mode, sizeof(enum SWITCH_MODE), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_MODE, IPC_OPCODE_ADD, ifindex);
	ret = l2_msg_send_hal_wait_ack(&mode, sizeof(enum SWITCH_MODE), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_MODE, IPC_OPCODE_ADD, ifindex);
	if(ret)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send switch mode  msg to hal\n",__FILE__,__LINE__,__func__);
		return -1;
	}
	
	/*继承access vlan*/
	if(DEFAULT_VLAN_ID != pif->switch_info.access_vlan)
	{
		vlanid=pif->switch_info.access_vlan;
		/*发送access vlanid给hal*/
		//ret = ipc_send_hal_wait_ack(&vlanid, sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_ACCESS_VLAN, IPC_OPCODE_ADD,ifindex);
		ret =  l2_msg_send_hal_wait_ack(&vlanid, sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_ACCESS_VLAN, IPC_OPCODE_ADD,ifindex);
		if(ret)
		{
			L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send l2if_set_access_vlan msg to hal\n",__FILE__,__LINE__,__func__);
			return -1;
		}
	}

	/*继承switch vlan*/
	if(NULL != pif->switch_info.vlan_list->head)
	{
		if(pif->switch_info.mode==SWITCH_MODE_TRUNK)
		{
			memset(switch_vlan_count,0,sizeof(switch_vlan_count));
			switch_vlan_count[line_num][0] = (uint32_t)listgetdata(pif->switch_info.vlan_list->head);
			switch_vlan_count[line_num][1] = (uint32_t)listgetdata(pif->switch_info.vlan_list->head);

			for(ALL_LIST_ELEMENTS_RO(pif->switch_info.vlan_list, p_listnode, pdata))
			{
				if((uint32_t)pdata == (switch_vlan_count[line_num][1] - 1))
				{
					switch_vlan_count[line_num][1] = (uint32_t)pdata;
				}
				else if((uint32_t)pdata != switch_vlan_count[line_num][0])
				{
					line_num++;
					switch_vlan_count[line_num][0] = (uint32_t)pdata;
					switch_vlan_count[line_num][1] = (uint32_t)pdata;
				}	
			}
			l2if_vlan_send_hal(switch_vlan_count, line_num, ifindex, TAGGED, SWITCH_MODE_TRUNK);
		}
		else if(pif->switch_info.mode==SWITCH_MODE_HYBRID)
		{
			/*tag vlan*/
			memset(switch_vlan_count, 0, sizeof(switch_vlan_count));
			for(ALL_LIST_ELEMENTS_RO(pif->switch_info.vlan_tag_list, p_listnode, pvlan_tagged))
			{
				if(pvlan_tagged->tag==TAGGED)
				{
					switch_vlan_count[line_num][0] = pvlan_tagged->vlanid;
					switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;		
					tag_num++;
					break;
				}
			}
			if(tag_num)		
			{
				for(ALL_LIST_ELEMENTS_RO(pif->switch_info.vlan_tag_list, p_listnode, pvlan_tagged))
				{
					if(pvlan_tagged->tag==TAGGED)
					{
						if(pvlan_tagged->vlanid == (switch_vlan_count[line_num][1] + 1))
						{
							switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
						}
						else if(pvlan_tagged->vlanid != switch_vlan_count[line_num][0])
						{
							line_num++;
							switch_vlan_count[line_num][0] = pvlan_tagged->vlanid;
							switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
						}
					}	
				}
				l2if_vlan_send_hal(switch_vlan_count, line_num, ifindex, TAGGED, SWITCH_MODE_HYBRID);
			}

			/*untag vlan*/
			memset(switch_vlan_count, 0, sizeof(switch_vlan_count));
			line_num=0;
			for(ALL_LIST_ELEMENTS_RO(pif->switch_info.vlan_tag_list, p_listnode, pvlan_tagged))
			{
				if(pvlan_tagged->tag==UNTAGGED)
				{
					switch_vlan_count[line_num][0] = pvlan_tagged->vlanid;
					switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;		
					untag_num++;
					break;
				}
			}
			if(untag_num) 	
			{
				for(ALL_LIST_ELEMENTS_RO(pif->switch_info.vlan_tag_list, p_listnode, pvlan_tagged))
				{
					if(pvlan_tagged->tag==UNTAGGED)
					{
						if(pvlan_tagged->vlanid == (switch_vlan_count[line_num][1] + 1))
						{
							switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
						}
						else if(pvlan_tagged->vlanid != switch_vlan_count[line_num][0])
						{
							line_num++;
							switch_vlan_count[line_num][0] = pvlan_tagged->vlanid;
							switch_vlan_count[line_num][1] = pvlan_tagged->vlanid;
						}
					}	
				}
				l2if_vlan_send_hal(switch_vlan_count, line_num, ifindex, UNTAGGED, SWITCH_MODE_HYBRID);
			}	
		}	
		
	}
	return 0;
}

/*获取l2if_table中的switch mode接口列表，返回数组地址*/
uint32_t* l2if_get_ifindex_list (uint16_t *total)
{
	static uint32_t ifindex_buff[150];
	struct hash_bucket *pbucket = NULL;
	struct l2if *pif = NULL;
	int cursor;
	int i=0;

	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif =(struct l2if *) pbucket->data;
		if(pif->mode!=IFNET_MODE_SWITCH)
				continue;
		
		ifindex_buff[i]=pif->ifindex;
		i++;
		*total=i;
	}
	return ifindex_buff;
}

/*在给定的vlan区间中删除接口配置的switch-vlan*/
int l2if_delete_range_switch_vlan (uint32_t ifindex,uint16_t v_start,uint16_t v_end)
{
	struct l2if *p_l2if = NULL;
	struct vlan_range vlan_send;
	struct listnode *p_listnode = NULL;
	struct list *list=NULL;
	void *pdata = NULL;
	uint32_t switch_vlan_count[4094][2];
	uint16_t line_num = 0;
	uint16_t vlanid;
	uint8_t ret = 0;
	uint8_t flag=1;
	int i,port=0;
	int access_flag = 0;
	
	p_l2if = l2if_lookup(ifindex);
	if(NULL == p_l2if)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:l2if_lookup fail,invalid ifindex\n",__FILE__,__LINE__,__func__);
		return -1;
	}	

	list=p_l2if->switch_info.vlan_list;
	memset(switch_vlan_count, 0, sizeof(switch_vlan_count));
	
	if((listhead(list) == NULL)&&(DEFAULT_VLAN_ID == p_l2if->switch_info.access_vlan))
	{
		return -1;
	}
	if(listhead(list) != NULL)
	{
		for(ALL_LIST_ELEMENTS_RO(list, p_listnode, pdata))
		{
			if(((uint32_t)pdata>=v_start)&&((uint32_t)pdata<=v_end))
			{
				switch_vlan_count[line_num][0]=switch_vlan_count[line_num][1]=(uint32_t)pdata;
				break;
			}
		}
		
		for(ALL_LIST_ELEMENTS_RO(list, p_listnode, pdata))
		{
			if(((uint32_t)pdata>=v_start)&&((uint32_t)pdata<=v_end))
			{
				if((uint32_t)pdata == (switch_vlan_count[line_num][1] - 1))
				{
					switch_vlan_count[line_num][1] = (uint32_t)pdata;
				}
				else if((uint32_t)pdata != switch_vlan_count[line_num][0])
				{
					line_num++;
					switch_vlan_count[line_num][0] = (uint32_t)pdata;
					switch_vlan_count[line_num][1] = (uint32_t)pdata;
				}	
			}
		}
		
		/*删除switch vlan*/
		for(i=line_num; i>=0; i--)
		{
			vlan_send.vlan_start = switch_vlan_count[i][1];
			vlan_send.vlan_end  = switch_vlan_count[i][0];
			l2if_delete_vlan(p_l2if,&vlan_send,flag);
		}
	}

	if( devtype == ID_HT157 || devtype == ID_HT158 )
	{
		if(IFM_TYPE_IS_TRUNK(ifindex) && !(IFM_TYPE_IS_SUBPORT(ifindex)))
		{
			for(i = 0; i < 3; i++)
			{
				if(trunk_reserve_vlan_table[i].trunkid == IFM_TRUNK_ID_GET(ifindex))
				{
					if(trunk_reserve_vlan_table[i].reserve_vlan_bak != p_l2if->switch_info.access_vlan)  
					{
						vlanid=p_l2if->switch_info.access_vlan;
						if((vlanid>=v_start)&&(vlanid<=v_end))
						{
							/*发送access vlanid给hal*/		
							vlanid = trunk_reserve_vlan_table[i].reserve_vlan_bak;;
							vlan_send.vlan_start =  vlan_send.vlan_end  = p_l2if->switch_info.access_vlan;
							access_flag = 1;
						}
					}
				}
			}
		}
		else
		{
		    port = IFM_PORT_ID_GET ( p_l2if->ifindex );
			if( port < 1 || port > 6 )
			{
				zlog_err ("Invalid port number! \n");
				return -1;
			}
			if((DEF_RESV_VLAN1 + port - 1) != p_l2if->switch_info.access_vlan)
			{
				vlanid = p_l2if->switch_info.access_vlan;
				if((vlanid>=v_start)&&(vlanid<=v_end))
				{
					/*发送access vlanid给hal*/		
					vlanid = DEF_RESV_VLAN1 + port - 1;
					vlan_send.vlan_start =  vlan_send.vlan_end  = p_l2if->switch_info.access_vlan;
					access_flag = 1;
				}
			}
			
		}				
	}
	else
	{
		if(DEFAULT_VLAN_ID != p_l2if->switch_info.access_vlan)  
		{
			vlanid=p_l2if->switch_info.access_vlan;
			if((vlanid>=v_start)&&(vlanid<=v_end))
			{
				/*发送access vlanid给hal*/		
				vlanid=DEFAULT_VLAN_ID;
				vlan_send.vlan_start =  vlan_send.vlan_end  = p_l2if->switch_info.access_vlan;
				access_flag = 1;
			}
		}
	}

	if(access_flag)
	{
		ret = l2_msg_send_hal_wait_ack(&vlanid, sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF, (uint8_t)L2IF_INFO_ACCESS_VLAN, IPC_OPCODE_ADD, p_l2if->ifindex);
		if(ret)
		{
			L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send l2if_set_access_vlan msg to hal\n",__FILE__,__LINE__,__func__);
			return -1;
		}
		else
		{
		    ipc_send_msg_n2(&vlan_send, sizeof(struct vlan_range), 1,MODULE_ID_FTM, MODULE_ID_L2, IPC_TYPE_VLAN, VLAN_INFO_PORT,IPC_OPCODE_DELETE,p_l2if->ifindex);
			p_l2if->switch_info.access_vlan = vlanid;
		}
	}
	
	L2_COMMON_LOG_DBG("%s[%d]:%s:ifindex=%d delete switch_vlan %d-%d successfully\n",__FILE__,__LINE__,__func__,ifindex,v_start,v_end);
	return 0;
}

int l2if_delete_dot1q_tunnel_by_vlan(uint32_t ifindex,uint16_t vlanid)
{
	struct l2if *p_l2if = NULL;
	struct vlan_mapping t_vlan_mapping;
	int ret=0;
	
	p_l2if = l2if_lookup(ifindex);
	if(NULL == p_l2if)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:l2if_lookup fail,invalid ifindex\n",__FILE__,__LINE__,__func__);
		return -1;
	}
	
	if(vlanid==p_l2if->switch_info.dot1q_tunnel)
	{
		memset(&t_vlan_mapping, 0, sizeof(struct vlan_mapping));
		t_vlan_mapping.svlan_new.vlan_start = vlanid; 
		t_vlan_mapping.svlan_new.vlan_end= vlanid;	
		t_vlan_mapping.svlan_cos = p_l2if->switch_info.dot1q_cos;

		//ret = ipc_send_hal_wait_ack(&t_vlan_mapping, sizeof(struct vlan_mapping),1,MODULE_ID_HAL,MODULE_ID_L2,IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_DOT1Q_TUNNEL,IPC_OPCODE_DELETE,ifindex);
		ret = l2_msg_send_hal_wait_ack(&t_vlan_mapping, sizeof(struct vlan_mapping),1,MODULE_ID_HAL,MODULE_ID_L2,IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_DOT1Q_TUNNEL,IPC_OPCODE_DELETE,ifindex);
		if(ret)
		{
			L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send dot1q_tunnel_delete msg to hal\n",__FILE__,__LINE__,__func__);
			return ERRNO_FAIL;
		}
		else
		{
			p_l2if->switch_info.dot1q_tunnel = DEFAULT_VLAN_ID;
			p_l2if->switch_info.dot1q_cos = 8;		
			L2_COMMON_LOG_DBG("%s[%d]: leave %s: sucessfully  to delete dot1q trunel\n", __FILE__, __LINE__, __func__);
			return ERRNO_SUCCESS;
		}
	}
		
	return 0;
}

int l2if_set_shutdown ( uint32_t ifindex, uint8_t enable )
{
	struct ifm_info pifm = {0};

	/*端口shutdown,不处理*/
	if(ifm_get_all_info(ifindex, MODULE_ID_L2, &pifm) == 0)
	{
		if(pifm.shutdown == IFNET_SHUTDOWN)
		{
			return 0;
		}
	}
	#if 0
    return ipc_send_hal ( &enable, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
                          IPC_TYPE_L2IF, IFNET_INFO_SHUTDOWN, IPC_OPCODE_UPDATE, ifindex );
	#endif
	return ipc_send_msg_n2 ( &enable, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
                          IPC_TYPE_L2IF, IFNET_INFO_SHUTDOWN, IPC_OPCODE_UPDATE, ifindex );
}

int l2if_clear_mac_limit(struct l2if *pif)
{
	struct ifm_switch ifm_switch_t;
	int ret=0;

	memset(&ifm_switch_t,0,sizeof(struct ifm_switch));	
	//ret=ipc_send_hal_wait_ack(&ifm_switch_t,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_MAC_LIMIT, IPC_OPCODE_UPDATE, pif->ifindex);
	ret=l2_msg_send_hal_wait_ack(&ifm_switch_t,sizeof(struct ifm_switch), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_L2IF,(uint8_t)L2IF_INFO_MAC_LIMIT, IPC_OPCODE_UPDATE, pif->ifindex);
	if(ret)
	{
		L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:fail to send mac limit msg to hal\n",__FILE__,__LINE__,__func__);
		return -1;
	}

	pif->switch_info.limit_num=0;
	L2_COMMON_LOG_DBG("%s[%d]:%s:ifindex: %x,mac limit=%d success!\n",__FILE__,__LINE__,__FUNCTION__,pif->ifindex,pif->switch_info.limit_num);
	return 0;
}
int l2if_clear_mac_by_port_vlan(char * pdata,uint32_t data_num,uint32_t ifindex)
{
	uint32_t ret = 0;
	uint32_t data_len = 0;

	data_len = MSTP_VLAN_MAP_SIZE *sizeof(char);
	//ret = ipc_send_hal(pdata, data_len, data_num, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_MAC, MAC_INFO_MAC_CLEAR, IPC_OPCODE_DELETE, ifindex);
	ret = ipc_send_msg_n2(pdata, data_len, data_num, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_MAC, MAC_INFO_MAC_CLEAR, IPC_OPCODE_DELETE, ifindex);
	  if (ret)
	  {
			L2_COMMON_LOG_DBG("%s[%d]:%s:static mac clear error,hal return ack  fail !\n",__FILE__,__LINE__, __FUNCTION__);
			return MAC_ERROR_HAL_FAIL;
	  }
	 return ret ;
}

int l2if_storm_suppress_get_bulk( struct mib_l2if_storm_control_info *storm_control_buf, uint32_t ifindex )
{
    struct l2if *pif = NULL;
	struct hash_bucket *pbucket = NULL;
	int cursor;
	int data_num = 0;
	int msg_num  = IPC_MSG_LEN/sizeof(struct mib_l2if_storm_control_info);
	int flag=0;
	
	L2_COMMON_LOG_DBG("%s:Entering the function of '%s'--the line of %d,ifindex=0x%x",__FILE__,__func__,__LINE__,ifindex);

	if(0==ifindex)
	{
		HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
		{
			pif =(struct l2if *) pbucket->data;
			if(pif->mode!=IFNET_MODE_SWITCH)
				continue;
			
			storm_control_buf[data_num].ifindex.ifindex = pif->ifindex;
			memcpy(&storm_control_buf[data_num].data, &(pif->switch_info.storm_control), sizeof(struct mib_l2if_storm_control_data));
			if( !storm_control_buf[data_num].data.storm_broadcast )
				storm_control_buf[data_num].data.storm_broadcast = 2;
			if( !storm_control_buf[data_num].data.storm_unicast )
				storm_control_buf[data_num].data.storm_unicast = 2;
			if( !storm_control_buf[data_num].data.storm_multicast )
				storm_control_buf[data_num].data.storm_multicast = 2;
			
			data_num++;
			
			if (data_num == msg_num)
			{
			 	return data_num;
			}
		}
	}
	else
	{
		HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
		{
			pif =(struct l2if *) pbucket->data;
			if(0==flag)
			{
				if(ifindex == pif->ifindex)
				{
					flag=1;
				}
				continue;
			}
			else
			{
				if(pif->mode!=IFNET_MODE_SWITCH)
					continue;
				
				storm_control_buf[data_num].ifindex.ifindex = pif->ifindex;
				memcpy(&storm_control_buf[data_num].data, &(pif->switch_info.storm_control), sizeof(struct mib_l2if_storm_control_data));
				if( !storm_control_buf[data_num].data.storm_broadcast )
					storm_control_buf[data_num].data.storm_broadcast = 2;
				if( !storm_control_buf[data_num].data.storm_unicast )
					storm_control_buf[data_num].data.storm_unicast = 2;
				if( !storm_control_buf[data_num].data.storm_multicast )
					storm_control_buf[data_num].data.storm_multicast = 2;
				
				data_num++;
				
				if (data_num == msg_num)
				{
					flag=0;
				 	return data_num;
				}
			}
		}
		flag=0;
	}
	
	return data_num;		
}

int l2if_rcv_storm_suppress_get_bulk( struct ipc_msghdr_n  *pmsghdr )
{
	uint32_t storm_control_max = IPC_MSG_LEN/sizeof(struct mib_l2if_storm_control_info);
    struct mib_l2if_storm_control_info storm_control_buf[storm_control_max];
	uint32_t storm_control_cnt = 0;
	int ret = 0;

	L2_COMMON_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	memset(storm_control_buf, 0, storm_control_max * sizeof(struct mib_l2if_storm_control_info));
    storm_control_cnt = l2if_storm_suppress_get_bulk(storm_control_buf, pmsghdr->msg_index);
	if (storm_control_cnt > 0)
	{
	#if 0
		ret = ipc_send_reply_bulk(storm_control_buf, storm_control_cnt * sizeof(struct mib_l2if_storm_control_info), storm_control_cnt, pmsghdr->sender_id,
                             MODULE_ID_L2, IPC_TYPE_STORM_CONTROL, pmsghdr->msg_subtype, pmsghdr->msg_index);
	#endif						 
		ret = ipc_send_reply_n2(storm_control_buf, storm_control_cnt * sizeof(struct mib_l2if_storm_control_info), storm_control_cnt, pmsghdr->sender_id,
	                        MODULE_ID_L2, IPC_TYPE_STORM_CONTROL, pmsghdr->msg_subtype, 0,pmsghdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
		L2_COMMON_LOG_DBG("%s:Entering the function of '%s'--the line of %d, ERRNO_NOT_FOUND",__FILE__,__func__,__LINE__);
	#if 0	
		ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_L2, IPC_TYPE_STORM_CONTROL,
                             pmsghdr->msg_subtype, pmsghdr->msg_index);
	#endif
		ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_L2, 
						IPC_TYPE_STORM_CONTROL, pmsghdr->msg_subtype, 0,0, IPC_OPCODE_NACK);
	}

	return ret;
}


int l2if_dot1q_tunnel_get_bulk( struct mib_l2if_dot1q_tunnel_info *dot1q_tunnel_buf, uint32_t ifindex )
{
    struct l2if *pif = NULL;
	struct hash_bucket *pbucket = NULL;
	int cursor;
	int data_num = 0;
	int msg_num  = IPC_MSG_LEN/sizeof(struct mib_l2if_dot1q_tunnel_info);
	int flag = 0;
	
	L2_COMMON_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	if(0==ifindex)
	{
		HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
		{
			pif =(struct l2if *) pbucket->data;
			if(pif->mode != IFNET_MODE_SWITCH)
				continue;

			if(pif->switch_info.dot1q_tunnel == DEFAULT_VLAN_ID)
				continue;

			dot1q_tunnel_buf[data_num].ifindex.ifindex = pif->ifindex;
			dot1q_tunnel_buf[data_num].data.dot1q_tunnel = pif->switch_info.dot1q_tunnel;
			dot1q_tunnel_buf[data_num].data.dot1q_cos = pif->switch_info.dot1q_cos;
			data_num++;
			
			if (data_num == msg_num)
			{
			 	return data_num;
			}
		}
	}
	else
	{
		HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
		{
			pif =(struct l2if *) pbucket->data;
			if( 0 == flag )
			{
				if( ifindex == pif->ifindex )
				{
					flag = 1;
				}
				continue;
			}
			else
			{
				if(pif->mode!=IFNET_MODE_SWITCH)
					continue;
				
				if(pif->switch_info.dot1q_tunnel == DEFAULT_VLAN_ID)
					continue;

				dot1q_tunnel_buf[data_num].ifindex.ifindex = pif->ifindex;
				dot1q_tunnel_buf[data_num].data.dot1q_tunnel = pif->switch_info.dot1q_tunnel;
				dot1q_tunnel_buf[data_num].data.dot1q_cos = pif->switch_info.dot1q_cos;
				data_num++;
				
				if (data_num == msg_num)
				{
					flag = 0;
				 	return data_num;
				}
			}
		}
		flag = 0;
	}
	
	return data_num;
}

int l2if_rcv_dot1q_tunnel_get_bulk( struct ipc_msghdr_n  *pmsghdr )
{
	uint32_t dot1q_tunnel_max = IPC_MSG_LEN/sizeof(struct mib_l2if_dot1q_tunnel_info);
    struct mib_l2if_dot1q_tunnel_info dot1q_tunnel_buf[dot1q_tunnel_max];
	uint32_t dot1q_tunnel_cnt = 0;
	int ret = 0;

	L2_COMMON_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	memset(dot1q_tunnel_buf, 0, dot1q_tunnel_max * sizeof(struct mib_l2if_dot1q_tunnel_info));
    dot1q_tunnel_cnt = l2if_dot1q_tunnel_get_bulk(dot1q_tunnel_buf, pmsghdr->msg_index);
	if (dot1q_tunnel_cnt > 0)
	{
	#if 0
		ret = ipc_send_reply_bulk(dot1q_tunnel_buf, dot1q_tunnel_cnt * sizeof(struct mib_l2if_dot1q_tunnel_info), dot1q_tunnel_cnt, pmsghdr->sender_id,
                             MODULE_ID_L2, IPC_TYPE_DOT1Q_TUNNEL, pmsghdr->msg_subtype, pmsghdr->msg_index);
	#endif
	
		ret = ipc_send_reply_n2(dot1q_tunnel_buf, dot1q_tunnel_cnt * sizeof(struct mib_l2if_dot1q_tunnel_info), dot1q_tunnel_cnt, pmsghdr->sender_id,
	                        MODULE_ID_L2, IPC_TYPE_DOT1Q_TUNNEL, pmsghdr->msg_subtype, 0,pmsghdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
		L2_COMMON_LOG_DBG("%s:Entering the function of '%s'--the line of %d, ERRNO_NOT_FOUND",__FILE__,__func__,__LINE__);
	#if 0
		ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_L2, IPC_TYPE_DOT1Q_TUNNEL,
                             pmsghdr->msg_subtype, pmsghdr->msg_index);
	#endif
	
		ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_L2, 
						IPC_TYPE_DOT1Q_TUNNEL, pmsghdr->msg_subtype, 0,0, IPC_OPCODE_NACK);
	}

	return ret;
}

