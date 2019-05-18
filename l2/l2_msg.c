/*
*   mpls message receive and send
*
*/
#include <unistd.h>							
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/zassert.h>
#include <lib/thread.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/oam_common.h>
#include <lib/ospf_common.h>
#include <lib/memshare.h>
#include <lib/snmp_common.h>

#include <ftm/pkt_eth.h>
#include "l2_if.h"
#include "l2_msg.h"
#include "trunk.h"	
#include "lacp/lacp.h"
#include "lldp/lldp_msg.h"
#include "lldp/lldp_api.h"
#include "mstp/mstp_bpdu.h"
#include "mstp/mstp_sm_procedure.h"
#include "loopd/loop_detect.h"				

#include "cfm/cfm_session.h"
#include "cfm/cfm.h"
#include "vlan.h"
#include "qinq.h"
#include "l2_snmp.h"
#include "mac_static.h"

#include "aps/elps.h"
#include "aps/elps_pkt.h"
#include "raps/erps.h"
#include "raps/erps_fsm.h"
#include "efm/efm_link_monitor.h"
#include "efm/efm_pkt.h"
#include "efm/efm_snmp.h"
#include "port_isolate.h"
#include "mstp/mstp_msg.h"
#include "efm/efm_agent_snmp.h"
#include "mstp/mstp_api.h"

extern struct thread_master *l2_master;  
extern unsigned int 	mstp_recv_debug_flag;
extern void delete_l2cp_config(uint32_t ifindex, uint32_t mode);
extern int erps_msg_rcv_get_bulk(struct ipc_msghdr_n  *pmsghdr, void *pdata);
extern int erps_msg_rcv_get_global(struct ipc_msghdr_n  *pmsghdr, void *pdata);

void l2_msg_rcv_subifevent(struct ipc_msghdr_n *phdr, struct ifm_event *pevent)
{
	uint32_t parent_ifindex = 0;
	struct hash_bucket *p_hash_bucket = NULL;
	struct l2if *pif = NULL;

	if (!phdr || !pevent)
	{
		return;
	}
	
	L2_COMMON_LOG_DBG("%s:Entering '%s',subtype:%d,ifindex:%x",__FILE__,__func__,
		phdr->msg_subtype, pevent->ifindex);

	if (phdr->opcode == IPC_OPCODE_EVENT)
	{
		parent_ifindex = IFM_PARENT_IFINDEX_GET(pevent->ifindex);
		p_hash_bucket = hios_hash_find(&l2if_table, (void*)parent_ifindex);
		if (p_hash_bucket == NULL)
		{
			return;
		}
		
		pif = (struct l2if *)p_hash_bucket->data;
		if (!pif)
		{
			return;
		}
		
		if((phdr->msg_subtype == IFNET_EVENT_DOWN) ||
			(phdr->msg_subtype == IFNET_EVENT_IF_DELETE))
		{
			lldp_subif_port_event(pif, pevent->ifindex, LLDP_FALSE);
		}
		else if((phdr->msg_subtype == IFNET_EVENT_UP) ||
			(phdr->msg_subtype == IFNET_EVENT_IF_ADD))
		{
			lldp_subif_port_event(pif, pevent->ifindex, LLDP_TRUE);
		}
	}
	
	return;
}


/* 从 ifm 接收接口事件 */
void l2_msg_rcv_ifevent(struct ipc_msghdr_n *phdr, struct ifm_event *pevent)
{
	uint32_t ifindex = 0;	
	
	L2_COMMON_LOG_DBG("%s:Entering '%s',subtype:%d,ifindex:%x",__FILE__,__func__,
		phdr->msg_subtype,pevent->ifindex);

	if (IFM_IS_SUBPORT(pevent->ifindex))
	{
		l2_msg_rcv_subifevent(phdr, pevent);
		return;
	}

    if(!IFM_TYPE_IS_PHYSICAL(pevent->ifindex))
    {
        zlog_err("%s[%d]:%s: err ifindex:%x is not physical\n",
                        __FILE__,__LINE__, __FUNCTION__, pevent->ifindex);
        return;
    }

	if (phdr->opcode == IPC_OPCODE_EVENT)
	{
		ifindex = pevent->ifindex;
		
		if(phdr->msg_subtype == IFNET_EVENT_DOWN)
		{
			l2if_down(ifindex); /* 接口 down 事件处理 */
			if(gelps.sess_enable)
			{
				elps_state_update(ifindex,0,PORT_STATE_DOWN);
			}
            
			if(gerps.sess_enable)
			{
				erps_state_update(ifindex,0,ERPS_PORT_DOWN);
			}
		}
		else if(phdr->msg_subtype == IFNET_EVENT_UP)
		{
			l2if_up(ifindex);	/* 接口 up 事件处理 */
			if(gelps.sess_enable)
			{
				elps_state_update(ifindex,0,PORT_STATE_UP);
			}
           
			if(gerps.sess_enable)
			{
				erps_state_update(ifindex,0,ERPS_PORT_UP);
			}
		}
		else if(phdr->msg_subtype == IFNET_EVENT_IF_DELETE)
		{
			l2if_delete(ifindex);/* 接口删除事件处理 */
			cfm_if_delete(ifindex);
		}
		else if(phdr->msg_subtype == IFNET_EVENT_IF_ADD)
		{
			l2if_get(ifindex);
			cfm_if_add(ifindex);
            
                           lldp_arp_learn_cfg_set(ifindex , lldp_arp_learn_cfg_status_get()) ; //for set arp learn config

                  }		
		else if (phdr->msg_subtype == IFNET_EVENT_MODE_CHANGE)
		{
			delete_l2cp_config(ifindex, pevent->mode);
			l2if_mode_change(ifindex, pevent->mode);/* 接口 mode 改变事件处理 */
		}
		else if(phdr->msg_subtype == IFNET_EVENT_SPEED_CHANGE)
		{	
			l2if_speed_change(ifindex);
		}		
	}
	L2_COMMON_LOG_DBG("%s:Leaving '%s',subtype:%d,ifindex:%x",__FILE__,__func__,
		phdr->msg_subtype,pevent->ifindex);

	return;
}

void l2_if_init(void)
{
	int if_num = 0;
	struct ifm_info *if_buffer = NULL;
	struct ifm_info *pif_buffer = NULL;
	int idx;
	struct l2if *pif = NULL;
	uint32_t ifindex = 0;
    int buffer_len = IPC_MSG_LEN / sizeof ( struct ifm_info );
	struct ifm_info p_ifinfo[buffer_len];

	
do{
	if_num = 0;
	
	if_buffer = ifm_get_bulk(ifindex, MODULE_ID_L2, &if_num);
	pif_buffer = if_buffer;
	if (NULL == if_buffer || !if_num)
	{
		mem_share_free_bydata(pif_buffer, MODULE_ID_L2);
		return;
	}
    memset(p_ifinfo, 0, buffer_len*sizeof(struct ifm_info));
    memcpy(p_ifinfo, if_buffer, if_num*sizeof(struct ifm_info));
	
	for (idx = 0; idx < if_num; idx++)
	{
		ifindex = p_ifinfo[idx].ifindex;

		/*port must be physical && is not amanger port(1/0/1 slot = 0)*/
		if ((!IFM_TYPE_IS_PHYSICAL(p_ifinfo[idx].ifindex) )||(!p_ifinfo[idx].slot))
		{
			continue;
		}
		
		pif = l2if_get(p_ifinfo[idx].ifindex);
		if (NULL == pif)
		{
			continue;
		}
	}
	mem_share_free_bydata(pif_buffer, MODULE_ID_L2);
}while(if_num);

	
	
	return;
}

/* 接收 cfm 消息 */
void l2_msg_rcv_cfm(struct ipc_msghdr_n *phdr, void *pdata)
{
	uint32_t key;	
	uint32_t ifindex;

	key = phdr->msg_index;
	ifindex = key;
	
	switch(phdr->opcode)
	{
		case IPC_OPCODE_UP: 	  /* session up */
		{
			cfm_session_up(key);
		}
		break;
		
		case IPC_OPCODE_DOWN:	  /* session down */
		{
			cfm_session_down(key);
		}
		break;

		case IPC_OPCODE_ADD:	  /* add alarm */
		{
			cfm_session_alarm(key, IPC_OPCODE_ADD, phdr->msg_subtype);
		}		
		break;
		
		case IPC_OPCODE_CLEAR:	  /* clear alarm */
		{
			cfm_session_alarm(key, IPC_OPCODE_CLEAR, phdr->msg_subtype);
		}		
		break;
		
		case IPC_OPCODE_FINISH:
		{
			if(phdr->msg_subtype == OAM_SUBTYPE_LM)
			{
				cfm_session_set_lm(phdr->msg_index, pdata);
			}
			else if(phdr->msg_subtype == OAM_SUBTYPE_DM)
			{
				cfm_session_set_dm(phdr->msg_index, pdata);
			}
		}
		break;
		
		case IPC_OPCODE_GET_BULK:
		{
			cfm_snmp_manage(pdata, phdr);
		}		
		break;
		
		case IPC_OPCODE_UPDATE:
		{
			cfm_session_reconfig(ifindex);
		}		
		break;
		
		default:
			break;
		
	}

    return;
}


/* 接收 vlan 消息 */
void l2_msg_rcv_vlan(struct ipc_msghdr_n *phdr, void *pdata)
{	
    uint16_t vlanid = 0;
    uint32_t ifindex = 0;

    if (phdr->opcode == IPC_OPCODE_GET_BULK)
    {
        if(phdr->msg_type == IPC_TYPE_L2IF)
        {
            if(phdr->msg_subtype == L2IF_INFO_MAX)/*switch mode 的接口信息*/
            {
                ifindex = *(uint32_t *)pdata;
                l2if_reply_switch_info_bulk(phdr, ifindex);
            }
            else if(phdr->msg_subtype == L2IF_INFO_SWITCH_VLAN)/*接口下vlan_list*/
            {
                ifindex = *(uint32_t *)pdata;
                l2if_reply_switch_vlan_bulk(phdr, ifindex);
            }
            else if(phdr->msg_subtype == L2IF_INFO_SWITCH_VLAN_TAG)/*tag or untag vlan_list*/
            {
                ifindex = *(uint32_t *)pdata;
                l2if_reply_switch_tag_vlan_bulk(phdr, ifindex);
            }            
            else if(phdr->msg_subtype == L2IF_INFO_QINQ)/*接口下qinq_list*/
            {
                l2if_reply_vlan_mapping_bulk(pdata,phdr);					
            }	
            else if (phdr->msg_subtype == L2IF_INFO_ISOLATE)
            {
                l2if_reply_isolate_info_bulk(phdr, pdata);
            }
        }
        else if(phdr->msg_type == IPC_TYPE_VLAN)
        {
            if(phdr->msg_subtype == VLAN_INFO_MAX)/*全局vlan信息*/
            {
                vlanid = (*(uint32_t *)pdata) & 0xffff;
                vlan_reply_vlan_info_bulk(phdr, vlanid);
            }
        }
    }
}


/* 接收 vlan 消息 */
int l2_msg_rcv_reserve_vlan(struct ipc_msghdr_n *phdr, void *pdata)
{	
	int ret=0;
	int i,j;
	uint16_t reserve_vlan_all[9];

	for( i=0; i< 3; i++ )
	{
		reserve_vlan_all[i] = trunk_reserve_vlan_table[i].reserve_vlan;
	}
	for( j=0; j< 6; j++ )
	{
		reserve_vlan_all[i+j] = reserve_vlan_table[j];
	}

	if (phdr->opcode == IPC_OPCODE_GET)
	{
	#if 0
		ret = ipc_send_reply (&reserve_vlan_all, 9*sizeof(uint16_t), phdr->sender_id, phdr->module_id,phdr->msg_type, phdr->msg_subtype, phdr->msg_index );
	#endif
		ret = ipc_send_reply_n2(&reserve_vlan_all, 9*sizeof(uint16_t), 1, phdr->sender_id,
	                        phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0,phdr->msg_index, IPC_OPCODE_REPLY);
		if(ret<0)
		{
			L2_COMMON_LOG_DBG("%s[%d]:leave %s:error:ipc_send_reply ! \n",__FILE__,__LINE__,__func__);
			return -1;
		}
		return 0;
	}
	return 0;
}


/* 接收 mac 消息 */
void l2_msg_rcv_mac(struct ipc_msghdr_n *phdr, void *pdata)
{
	if (phdr->opcode == IPC_OPCODE_GET)
	{ 
		if(phdr->msg_subtype == MAC_INFO_MAC_CONFIG)/*mac全局配置*/
		{
			mac_static_reply_mac_config(phdr);
		}
	}
	else if (phdr->opcode == IPC_OPCODE_GET_BULK)
	{ 
		if(phdr->msg_subtype == MAC_INFO_MAC)/*静态mac配置*/
		{
			mac_static_reply_mac_info_bulk(pdata,phdr);
		}
	}
}

/* 接收trunk消息 */
void l2_msg_rcv_trunk(struct ipc_msghdr_n *phdr, void *pdata)
{
	int ret = 0;
	int msg_num = IPC_MSG_LEN/sizeof(struct trunk);
    struct trunk pif[msg_num];
	
	int num = IPC_MSG_LEN/sizeof(struct trunk_port);
	struct trunk_port tport[num];
	uint16_t trunkid = 0;
 
	switch(phdr->opcode)
	{
		case IPC_OPCODE_GET_BULK:
		{
			if(phdr->msg_subtype == TRUNK_INFO_TRUNK)
			{
				trunkid = phdr->msg_index;
				memset(pif, 0, msg_num*sizeof(struct trunk));			
				ret = trunk_info_bulk_get(trunkid, pif);
				if (ret > 0)
				{
					/*ret = ipc_send_reply_bulk(pif, ret*sizeof(struct trunk), ret, phdr->sender_id,
											MODULE_ID_L2, IPC_TYPE_TRUNK, phdr->msg_subtype, trunkid);*/

					ret = ipc_send_reply_n2(pif, ret*sizeof(struct trunk), ret, phdr->sender_id,MODULE_ID_L2,
											IPC_TYPE_TRUNK, phdr->msg_subtype,0,trunkid, IPC_OPCODE_REPLY);
				}
				else
				{
					/*ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_TRUNK,
											phdr->msg_subtype, trunkid);*/		
				ret = ipc_send_reply_n2(NULL,0,0, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_TRUNK, 
										phdr->msg_subtype, 0, 0, IPC_OPCODE_NACK);
				}
			}
			else if(phdr->msg_subtype == TRUNK_INFO_PORT)
			{
				trunkid = phdr->msg_index;
				memset(tport, 0, num*sizeof(struct trunk_port));  
				ret = trunk_port_info_bulk_get(trunkid, tport);
				if (ret > 0)
				{
					/*ret = ipc_send_reply_bulk(tport, ret*sizeof(struct trunk_port), ret, phdr->sender_id,
											MODULE_ID_L2, IPC_TYPE_TRUNK, phdr->msg_subtype, trunkid);*/
					ret = ipc_send_reply_n2(tport, ret*sizeof(struct trunk_port), ret,  phdr->sender_id,MODULE_ID_L2,
											IPC_TYPE_TRUNK, phdr->msg_subtype,0,trunkid, IPC_OPCODE_REPLY);
				}
				else
				{
					/*ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_TRUNK,
											phdr->msg_subtype, trunkid);*/
					ret = ipc_send_reply_n2(NULL,0,0, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_TRUNK, 
										phdr->msg_subtype, 0, 0, IPC_OPCODE_NACK);
				}
			}
			else if(phdr->msg_subtype == TRUNK_INFO_GLOBAL_LOADBALANCE)
			{
			#if 0
				ret = ipc_send_reply_bulk(&trunk_gloable_config, sizeof(struct trunk_config), ret, phdr->sender_id,
										MODULE_ID_L2, IPC_TYPE_TRUNK, phdr->msg_subtype, 0);
			#endif

				ret = ipc_send_reply_n2(&trunk_gloable_config, sizeof(struct trunk_config), ret, phdr->sender_id,
										MODULE_ID_L2, IPC_TYPE_TRUNK, phdr->msg_subtype, 0,phdr->msg_index, IPC_OPCODE_REPLY);
				//printf("%s:%d--->ret = %d\n", __FUNCTION__,__LINE__,ret);
			}
		}
		break;
		default:
			break;
	}

 	return;
}

void l2_msg_rcv_loopdetect(struct ipc_msghdr_n *phdr)
{
	int ret = 0;
	int msg_num = IPC_MSG_LEN/sizeof(struct loop_detect_snmp);
    struct loop_detect_snmp loopdetect_snmp[msg_num];
	uint32_t ifindex = 0;


	ifindex = phdr->msg_index;
	memset(loopdetect_snmp, 0, msg_num*sizeof(struct loop_detect_snmp));			
	ret = loopdetect_snmp_port_info_get(ifindex, loopdetect_snmp);
	if (ret > 0)
	{
	#if 0
		ret = ipc_send_reply_bulk(loopdetect_snmp, ret*sizeof(struct loop_detect_snmp), ret, phdr->sender_id,
								MODULE_ID_L2, IPC_TYPE_SNMP, phdr->msg_subtype, ifindex);
	#endif							
		ipc_ack_to_snmp(phdr,loopdetect_snmp, ret*sizeof(struct loop_detect_snmp), ret);
	}
	else
	{
	#if 0
		ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, MODULE_ID_L2, IPC_TYPE_SNMP,
								phdr->msg_subtype, ifindex);
	#endif
		ipc_noack_to_snmp(phdr);
	}

 	return;
}

#if 0
/* 接收 IPC 消息 */
int l2_msg_rcv_msg(struct ipc_mesg *pmesg)
{	
    struct ipc_msghdr *phdr = NULL;
	void *pdata = NULL;
    int token = 100;
	int ret=0;

	while(token)
	{
		ret = ipc_recv_common(pmesg, MODULE_ID_L2);
		if(ret == -1)
			return ERRNO_FAIL;

		token--;
		
		/* process the ipc message */
		phdr = &(pmesg->msghdr);
		pdata = pmesg->msg_data;
		if(phdr->msg_type == IPC_TYPE_IFM) 
		{
			l2_msg_rcv_ifevent(phdr, pdata);
		}
		else if(phdr->msg_type == IPC_TYPE_CFM)
		{
			l2_msg_rcv_cfm(phdr, pdata);
		}
		else if(phdr->msg_type == IPC_TYPE_ELPS)
		{
			if(IPC_OPCODE_GET_BULK == phdr->opcode)
			{
				elps_msg_rcv_get_bulk(phdr, pdata);
			}
		}
		else if((phdr->msg_type == IPC_TYPE_VLAN)||(phdr->msg_type == IPC_TYPE_L2IF))
		{
			l2_msg_rcv_vlan(phdr, pdata);
		}
		else if(phdr->msg_type == IPC_TYPE_MAC)
		{
			l2_msg_rcv_mac(phdr, pdata);
		}
		else if(phdr->msg_type == IPC_TYPE_TRUNK)
		{
			l2_msg_rcv_trunk(phdr, pdata);
		}
		else if(phdr->msg_type == IPC_TYPE_EFM)
		{  
		          if(phdr->opcode == IPC_OPCODE_LINK_MONITOR)
		          {
                            efm_link_monitor_event_occur(phdr,(uint64_t *)pdata);
		          }	  
			 if(phdr->opcode == IPC_OPCODE_EVENT)
			 {
			 efm_recv_local_event (EFM_CRITICAL_LINK_EVENT, *((uint8_t *)pdata));
			 }
			 /*dcn update efm agent ip info*/
			 if(phdr->opcode == IPC_OPCODE_UPDATE)
			 {

				efm_agent_update_ip(pdata,phdr->data_len);

			 }
			 /*dcn enable and get efm agent info*/
			 if(phdr->opcode == IPC_OPCODE_ENABLE)
			 {
				efm_agent_send_info_reply_dcn();
			 }
			if (phdr->opcode == IPC_OPCODE_CHANGE)
			{
				efm_agent_update_hostname((struct u0_device_info *)pdata,phdr->data_len);
			}
		}
		/*deal snmpd->l2 snmp msg*/
		else if(phdr->msg_type == IPC_TYPE_SNMP)
		{
			switch(phdr->msg_subtype)
			{		
				/*lldp snmp subtype*/
				case IPC_TYPE_SNMP_LLDP_GLOBAL_CFG:
				{
					lldp_snmp_global_cfg_get(phdr);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_PORT:
				{
					lldp_snmp_port_cfg_get(phdr);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_PORT_CONFIG:
				{
					lldp_snmp_port_config_get(phdr);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_STATS_TXRX_PORT:
				{
					lldp_snmp_stats_txrx_port_get(phdr);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_LOC_PORT:
				{
					lldp_snmp_loc_port_get(phdr);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_LOC_PORT_PRIV:
				{
					lldp_snmp_loc_port_priv_get(phdr);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_PORT_MSAP:
				{
					lldp_snmp_port_msap_get(phdr, pmesg->msg_data[0]);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_PORT_MSAP_MAN_ADDR:
				{
					lldp_snmp_port_msap_man_addr_get(phdr, pmesg->msg_data[0]);
				}
					break;
				case IPC_TYPE_SNMP_LOOPDETECT_INFO:
				{
					l2_msg_rcv_loopdetect(phdr);
				}
				  break;
                                 case IPC_TYPE_SNMP_EFM_INFO:
				{
                                             
					 if(phdr->data_len)
					 {
					 	snmp_msg_rcv_efm_eventlog_info(phdr,*((uint32_t *)pdata));
					 }
					 else
					 {
					  	snmp_msg_rcv_efm(phdr);
					 }
				}
				break;
			    case IPC_TYPE_SNMP_EFM_AGENT_INFO:
				{
					if(IPC_OPCODE_GET == phdr->opcode)
					{
						
						if(phdr->data_len)
						{
							
							snmp_msg_rcv_efm_agent_SecIfindex_info(phdr,*((uint32_t *)pdata));
							
						}
						else  
						{
							snmp_msg_rcv_efm_agent(phdr);
						}
					}
					else if(IPC_OPCODE_UPDATE == phdr->opcode)
					{

						snmp_msg_rcv_efm_agent_set(phdr,(uint32_t *)pdata);
					}
				}
					break;
				case IPC_TYPE_SNMP_MSTP_INFO:
				{
					if(phdr->data_len)
					{
						snmp_msg_rcv_instance_port_info(phdr,*((uint32_t *)pdata));
					}
					else
					{
						snmp_msg_rcv_mstp(phdr);
					}
				}
				break;
				default:					
					L2_COMMON_LOG_DBG("%s: ERROR L2 SNMP SUBTYPE!\n", __func__);			
					break;
			}
		}	
		else if(phdr->msg_type == IPC_TYPE_RESERVE_VLAN)
		{
			l2_msg_rcv_reserve_vlan(phdr, pdata);
		}
		else if(phdr->msg_type == IPC_TYPE_ERPS)
		{
			if(IPC_OPCODE_GET_BULK == phdr->opcode)
			{
				erps_msg_rcv_get_bulk(phdr, pdata);
			}
			else if(IPC_OPCODE_GET == phdr->opcode)
			{
			    erps_msg_rcv_get_global(phdr, pdata);
			}
		}
		else if( phdr->msg_type == IPC_TYPE_STORM_CONTROL )
		{
			if( IPC_OPCODE_GET_BULK == phdr->opcode )
			{
				l2if_rcv_storm_suppress_get_bulk( phdr );
			}

		}
		else if( phdr->msg_type == IPC_TYPE_DOT1Q_TUNNEL )
		{
			if( IPC_OPCODE_GET_BULK == phdr->opcode )
			{
				l2if_rcv_dot1q_tunnel_get_bulk( phdr );
			}
		}

		else if( phdr->msg_type == IPC_TYPE_OSPF_DCN )
		{
		    if (IPC_SUB_TYPE_DCN_STATUS == phdr->msg_subtype)
                      {      
                            u_char arp_learnable = *(u_char *)pdata;      
                            lldp_arp_learn_cfg_set(0, arp_learnable);
                       }                  
		}        
	}
	
	return ret;
}


/* 接收报文 */
int l2_msg_rcv_pkt(void)
{
	struct pkt_buffer	*pkt = NULL;	
	struct l2if 		*pif = NULL;
	struct eth_control  *ethcb = NULL;
    int token = 100;
	int ret=0;

	while(token)
	{
		pkt = pkt_rcv(MODULE_ID_L2);
		if(pkt == NULL)
			return ERRNO_FAIL;

		token--;
        ethcb = (struct eth_control *)(&pkt->cb);
		
		if(pkt->in_ifindex)
		{
			/* when pw cfm, pif is L3 */
			if(pkt->protocol == ETH_P_CFM) /*cfm报文*/
			{
				cfm_pkt_rcv(pkt);
				continue;
			}

			if(pkt->protocol == ETH_P_LLDP) /*lldp报文*/
			{ 
				pif = l2if_lookup(pkt->in_port);		//physical port
				
				if (pkt->in_ifindex != pkt->in_port)	//maybe subport or trunk port
				{
					if (IFM_TYPE_IS_TRUNK_PHYSICAL(pkt->in_ifindex))
					{
						pif = l2if_lookup(pkt->in_ifindex);
					}
					else if (IFM_TYPE_IS_TRUNK_SUBPORT(pkt->in_ifindex))
					{
						pif = l2if_lookup(IFM_PARENT_IFINDEX_GET(pkt->in_ifindex));
					}
				}

				if(pif == NULL)
				{
					continue;
				}
				
				lldp_pkt_rcv(pif, pkt);
			}

			if(ETH_P_LACP == pkt->protocol  &&  ETH_SUBTYPE_EFM == ethcb->sub_ethtype)
			{
				efm_recv(pkt);
				continue;
			}
			else if(ETH_P_LOOPD == pkt->protocol)
			{
				loopdetect_pkt_recv(pkt);
				continue;
			}
			
			pif = l2if_lookup(pkt->in_ifindex);
			if(pif == NULL)
				continue;
		}

		/* 通过 ehtype 识别报文 */ 
		if((pkt->cb.ethcb.dmac[0] == MSTP_DEST_MAC_0)
			&& (pkt->cb.ethcb.dmac[1] == MSTP_DEST_MAC_1)
			&& (pkt->cb.ethcb.dmac[2] == MSTP_DEST_MAC_2)
			&& (pkt->cb.ethcb.dmac[3] == MSTP_DEST_MAC_3)
			&& (pkt->cb.ethcb.dmac[4] == MSTP_DEST_MAC_4)
			&& (pkt->cb.ethcb.dmac[5] == MSTP_DEST_MAC_5))
		{

			mstp_pkt_recv(pif, pkt);
		}
		else if(pkt->protocol == ETH_P_LOOPD) /*loopd协议*/
		{
			/*payload为网络序,FTM上送控制信息为主机序*/
			loopdetect_pkt_recv(pkt);
		}
		else if(pkt->protocol == ETH_P_LACP) /* lacp、SYNCE、EFM 等慢协议通过 sub ethtype 识别 */
		{
		    if(ethcb->sub_ethtype == ETH_SUBTYPE_LACP)
		    {
				lacp_pkt_rx(pif, pkt);
		    }

			else if(ethcb->sub_ethtype == ETH_SUBTYPE_EFM)
			{

				efm_recv(pkt);
			}
		}		
		else if(pkt->protocol == ETH_P_CFM) /*cfm报文*/
		{
			cfm_pkt_rcv(pkt);
		}
	}
	
	return ret;
}


/* 处理 mpls 的 IPC 消息和报文接收 */
int l2_msg_rcv(struct thread *t)
{
    static struct ipc_mesg mesg;

	l2_msg_rcv_msg(&mesg);
	l2_msg_rcv_pkt();
	
	usleep(1000);//让出 CPU
    thread_add_event ( l2_master, l2_msg_rcv, NULL, 0 );
	
    return ERRNO_SUCCESS;
}
#endif

static uint32_t gpnL2CoreProc (gpnSockMsg *pgnNsmMsgSp, uint32_t len)
{
	UINT32	   stat_type; 

	if(pgnNsmMsgSp == NULL)
	{
		zlog_err("%s,pgnNsmMsgSp:%p",__FUNCTION__,pgnNsmMsgSp);
		return 1;
	}

	L2_COMMON_LOG_DBG("%s : rcv msg(%08x) from (%d) !\n\r",\
			__FUNCTION__, pgnNsmMsgSp->iMsgType, pgnNsmMsgSp->iSrcId);

	stat_type = pgnNsmMsgSp->iMsgPara7;

	switch(pgnNsmMsgSp->iMsgType)
	{		
		case GPN_STAT_MSG_PORT_STAT_MON_ENABLE:
			if(stat_type == GPN_STAT_T_PTN_MEP_M_TYPE)
			{
				cfm_session_perfm_enable(pgnNsmMsgSp);
			}
			break;

		case GPN_STAT_MSG_PORT_STAT_MON_DISABLE:
			if(stat_type == GPN_STAT_T_PTN_MEP_M_TYPE)
			{
				cfm_session_perfm_disable(pgnNsmMsgSp);
			}
			break;	

		case GPN_STAT_MSG_PTN_MEP_M_STAT_GET:				
			if(stat_type == GPN_STAT_T_PTN_MEP_M_TYPE)
			{
				cfm_session_perfm_get(pgnNsmMsgSp);
			}
			break;

		default:
			break;
	}

	return 0;
}

/*replace "l2_msg_rcv_msg" with this new function */
int l2_msg_rcv_msg_n(struct ipc_mesg_n *pmesg, int imlen)
{
    int revln = 0;
	struct ipc_msghdr_n *phdr = NULL;
	void *pdata = NULL;
	if(NULL == pmesg)
	{
		return ERRNO_FAIL;
	}

    revln = (int)pmesg->msghdr.data_len + IPC_HEADER_LEN_N; 
	if(revln <= imlen)
	{
		/* process the ipc message */
		phdr = &(pmesg->msghdr);
		pdata = pmesg->msg_data;
		if(phdr->msg_type == IPC_TYPE_IFM) 
		{
			l2_msg_rcv_ifevent(phdr, pdata);
		}
		else if(phdr->msg_type == IPC_TYPE_CFM)
		{
			l2_msg_rcv_cfm(phdr, pdata);
		}
		else if(phdr->msg_type == IPC_TYPE_ELPS)
		{
			if(IPC_OPCODE_GET_BULK == phdr->opcode)
			{
				elps_msg_rcv_get_bulk(phdr, pdata);
			}
		}
		else if((phdr->msg_type == IPC_TYPE_VLAN)||(phdr->msg_type == IPC_TYPE_L2IF))
		{
			l2_msg_rcv_vlan(phdr, pdata);
		}
		else if(phdr->msg_type == IPC_TYPE_MAC)
		{
			l2_msg_rcv_mac(phdr, pdata);
		}
		else if(phdr->msg_type == IPC_TYPE_TRUNK)
		{
			l2_msg_rcv_trunk(phdr, pdata);
		}
		else if(phdr->msg_type == IPC_TYPE_EFM)
		{  
		          if(phdr->opcode == IPC_OPCODE_LINK_MONITOR)
		          {
                            efm_link_monitor_event_occur(phdr,(uint64_t *)pdata);
		          }	  
			 if(phdr->opcode == IPC_OPCODE_EVENT)
			 {
			 efm_recv_local_event (EFM_CRITICAL_LINK_EVENT, *((uint8_t *)pdata));
			 }
			 /*dcn update efm agent ip info*/
			 if(phdr->opcode == IPC_OPCODE_UPDATE)
			 {

				efm_agent_update_ip(pdata,phdr->data_len);

			 }
			 /*dcn enable and get efm agent info*/
			 if(phdr->opcode == IPC_OPCODE_ENABLE)
			 {
				efm_agent_send_info_reply_dcn();
			 }
		}
		/*deal snmpd->l2 snmp msg*/
		else if(phdr->msg_type == IPC_TYPE_SNMP)
		{
			switch(phdr->msg_subtype)
			{		
				/*lldp snmp subtype*/
				case IPC_TYPE_SNMP_LLDP_GLOBAL_CFG:
				{
					lldp_snmp_global_cfg_get(phdr);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_PORT:
				{
					lldp_snmp_port_cfg_get(phdr);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_PORT_CONFIG:
				{
					lldp_snmp_port_config_get(phdr);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_STATS_TXRX_PORT:
				{
					lldp_snmp_stats_txrx_port_get(phdr);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_LOC_PORT:
				{
					lldp_snmp_loc_port_get(phdr);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_LOC_PORT_PRIV:
				{
					lldp_snmp_loc_port_priv_get(phdr);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_PORT_MSAP:
				{
					lldp_snmp_port_msap_get(phdr, pmesg->msg_data[0]);
				}
					break;
				case IPC_TYPE_SNMP_LLDP_PORT_MSAP_MAN_ADDR:
				{
					lldp_snmp_port_msap_man_addr_get(phdr, pmesg->msg_data[0]);
				}
					break;
				case IPC_TYPE_SNMP_LOOPDETECT_INFO:
				{
					l2_msg_rcv_loopdetect(phdr);
				}
				  break;
                                 case IPC_TYPE_SNMP_EFM_INFO:
				{
                                             
					 if(phdr->data_len)
					 {
					 	snmp_msg_rcv_efm_eventlog_info(phdr,*((uint32_t *)pdata));
					 }
					 else
					 {
					  	snmp_msg_rcv_efm(phdr);
					 }
				}
				break;
			    case IPC_TYPE_SNMP_EFM_AGENT_INFO:
				{
					if(IPC_OPCODE_GET == phdr->opcode)
					{
						
						if(phdr->data_len)
						{
							
							snmp_msg_rcv_efm_agent_SecIfindex_info(phdr,*((uint32_t *)pdata));
							
						}
						else  
						{
							snmp_msg_rcv_efm_agent(phdr);
						}
					}
					else if(IPC_OPCODE_UPDATE == phdr->opcode)
					{

						snmp_msg_rcv_efm_agent_set(phdr,(uint32_t *)pdata);
					}
				}
					break;
				case IPC_TYPE_SNMP_MSTP_INFO:
				{
					if(phdr->data_len)
					{
						snmp_msg_rcv_instance_port_info(phdr,*((uint32_t *)pdata));
					}
					else
					{
						snmp_msg_rcv_mstp(phdr);
					}
				}
				break;
				default:					
					L2_COMMON_LOG_DBG("%s: ERROR L2 SNMP SUBTYPE!\n", __func__);			
					break;
			}
		}	
		else if(phdr->msg_type == IPC_TYPE_RESERVE_VLAN)
		{
			l2_msg_rcv_reserve_vlan(phdr, pdata);
		}
		else if(phdr->msg_type == IPC_TYPE_ERPS)
		{
			if(IPC_OPCODE_GET_BULK == phdr->opcode)
			{
				erps_msg_rcv_get_bulk(phdr, pdata);
			}
			else if(IPC_OPCODE_GET == phdr->opcode)
			{
			    erps_msg_rcv_get_global(phdr, pdata);
			}
		}
		else if( phdr->msg_type == IPC_TYPE_STORM_CONTROL )
		{
			if( IPC_OPCODE_GET_BULK == phdr->opcode )
			{
				l2if_rcv_storm_suppress_get_bulk( phdr );
			}

		}
		else if( phdr->msg_type == IPC_TYPE_DOT1Q_TUNNEL )
		{
			if( IPC_OPCODE_GET_BULK == phdr->opcode )
			{
				l2if_rcv_dot1q_tunnel_get_bulk( phdr );
			}
		}

		else if( phdr->msg_type == IPC_TYPE_OSPF_DCN )
		{
		    if (IPC_SUB_TYPE_DCN_STATUS == phdr->msg_subtype)
                      {      
                            u_char arp_learnable = *(u_char *)pdata;      
                            lldp_arp_learn_cfg_set(0, arp_learnable);
                       }                  
		}   
		else if(phdr->msg_type == IPC_TYPE_PACKET)
        {
            l2_msg_rcv_pkt_n(pmesg);
        }
		else if (phdr->msg_type == IPC_TYPE_STAT_DB)
		{
			gpnL2CoreProc((gpnSockMsg *)pmesg->msg_data, pmesg->msghdr.data_len);
		}
	}
    
	return ERRNO_SUCCESS;
}


/*replace "l2_msg_rcv_pkt" with this new function */
int l2_msg_rcv_pkt_n(struct ipc_mesg_n *pmesg)
{
	struct pkt_buffer	*pkt = NULL;	
	struct l2if 		*pif = NULL;
	struct eth_control  *ethcb = NULL;
	
	pkt = (struct pkt_buffer *)pmesg->msg_data;
	if(pkt == NULL)
	{
		return ERRNO_FAIL;
						
	}
	APP_RECV_PKT_FROM_FTM_DATA_SET(pkt);
	ethcb = (struct eth_control *)(&pkt->cb);
						
	if(pkt->in_ifindex)
	{
		/* when pw cfm, pif is L3 */
		if(pkt->protocol == ETH_P_CFM) /*cfmæŠ¥æ–‡*/
		{
			cfm_pkt_rcv(pkt);
			
			return ERRNO_SUCCESS;
		}
		if(pkt->protocol == ETH_P_LLDP) /*lldpæŠ¥æ–‡*/
		{ 
			pif = l2if_lookup(pkt->in_port);		//physical port
									
			if (pkt->in_ifindex != pkt->in_port)	//maybe subport or trunk port
			{
				if (IFM_TYPE_IS_TRUNK_PHYSICAL(pkt->in_ifindex))
				{
					pif = l2if_lookup(pkt->in_ifindex);
				}
				else if (IFM_TYPE_IS_TRUNK_SUBPORT(pkt->in_ifindex))
				{
					pif = l2if_lookup(IFM_PARENT_IFINDEX_GET(pkt->in_ifindex));
				}
			}
									
			if(pif != NULL)
			{
				lldp_pkt_rcv(pif, pkt);
			}
									
										
		}
							
		if(ETH_P_EFM == pkt->protocol  &&  ETH_SUBTYPE_EFM == ethcb->sub_ethtype)
		{
			efm_recv(pkt);
			return ERRNO_SUCCESS;
									
		}
		else if(ETH_P_LOOPD == pkt->protocol)
		{
			loopdetect_pkt_recv(pkt);
			return ERRNO_SUCCESS;
								
		}
							
		pif = l2if_lookup(pkt->in_ifindex);
		if(NULL == pif)
		{
			return ERRNO_FAIL; 
		}
	}
			
	/* é€šè¿‡ ehtype è¯†åˆ«æŠ¥æ–‡ */ 
	if((pkt->cb.ethcb.dmac[0] == MSTP_DEST_MAC_0)
	&& (pkt->cb.ethcb.dmac[1] == MSTP_DEST_MAC_1)
	&& (pkt->cb.ethcb.dmac[2] == MSTP_DEST_MAC_2)
	&& (pkt->cb.ethcb.dmac[3] == MSTP_DEST_MAC_3)
	&& (pkt->cb.ethcb.dmac[4] == MSTP_DEST_MAC_4)
	&& (pkt->cb.ethcb.dmac[5] == MSTP_DEST_MAC_5))
	{
					
			mstp_pkt_recv(pif, pkt);
	}
	else if(pkt->protocol == ETH_P_LOOPD) /*loopdåè®®*/
	{
		/*payload盲赂潞莽陆鈥樏慌撁ヂ郝?,FTM盲赂艩茅?聛忙沤搂氓藛露盲驴隆忙聛炉盲赂潞盲赂禄忙艙潞氓潞?*/
		loopdetect_pkt_recv(pkt);
	}
	else if(pkt->protocol == ETH_P_LACP) /* lacp茫鈧丼YNCE茫鈧丒FM 莽颅鈥懊︹?β⒚ヂ嵚徝┾偓拧猫驴鈥? sub ethtype 猫炉鈥犆ニ喡? */
	{
		if(ethcb->sub_ethtype == ETH_SUBTYPE_LACP)
		{
			lacp_pkt_rx(pif, pkt);
		}
					
		else if(ethcb->sub_ethtype == ETH_SUBTYPE_EFM)
		{
						
			efm_recv(pkt);
		}
	}		
	else if(pkt->protocol == ETH_P_CFM) /*cfm忙艩楼忙鈥撯??*/
	{
		cfm_pkt_rcv(pkt);
	}
	
	return ERRNO_SUCCESS;

}

int l2_msg_rcv_n(struct ipc_mesg_n *pmesg, int imlen)
{
	l2_msg_rcv_msg_n(pmesg, imlen);
		
	mem_share_free(pmesg, MODULE_ID_L2);
	return ERRNO_SUCCESS;
}

/************************************************************
* Function : l2_msg_send_ack
* return   : 0 ok , 1 error   
* description: send errcode as reply because of no local data
************************************************************/
				
/*send ack as reply*/
int l2_msg_send_ack (struct ipc_msghdr_n *rcvhdr,uint32_t msg_index)
{
					
	struct ipc_mesg_n * repmesg = NULL;
	uint32_t msg_len = 0;
						
	msg_len = sizeof(struct ipc_msghdr_n);/*info len*/
								
	repmesg = ipc_msg_malloc(msg_len,MODULE_ID_L2);
	if(NULL == repmesg)	
	{
		return ERRNO_FAIL;
								
	}
					
	memset(repmesg,0,msg_len);
						
	repmesg->msghdr.data_len = 0; 
	repmesg->msghdr.data_num = 0;                 
	repmesg->msghdr.msg_index = msg_index;
	repmesg->msghdr.opcode = IPC_OPCODE_ACK;

	/*send fail so free local malloc memory*/
	if(ipc_send_reply_n1(rcvhdr, repmesg, msg_len))
	{
		mem_share_free(repmesg, MODULE_ID_L2);	
		return ERRNO_FAIL;
	}
					
	return ERRNO_SUCCESS;
						
}

/************************************************************
 * Function : l2_msg_send_noack
 * return   : 0 ok , 1 error   
 * description: send errcode as reply because of no local data
 ************************************************************/

/*send noack as reply*/
int l2_msg_send_noack (uint32_t errcode, struct ipc_msghdr_n *rcvhdr,uint32_t msg_index)
{
	
		struct ipc_mesg_n * repmesg = NULL;
		uint32_t msg_len = 0;
		
		msg_len = sizeof(struct ipc_msghdr_n)+sizeof(uint32_t);
		
		repmesg = ipc_msg_malloc(msg_len,MODULE_ID_L2);
		if(NULL == repmesg)	
		{
			return ERRNO_FAIL;
				
		}
	
		memset(repmesg,0,msg_len);
		
		memcpy(repmesg->msg_data,&errcode,4);
		
		repmesg->msghdr.data_len = 4;   /*sizeof(errcode)*/
		repmesg->msghdr.data_num = 1;                 
		repmesg->msghdr.msg_index = msg_index;
		repmesg->msghdr.opcode = IPC_OPCODE_NACK;
		
		if(ipc_send_reply_n1(rcvhdr, repmesg, msg_len))
		{
			mem_share_free(repmesg, MODULE_ID_L2);	
				return ERRNO_FAIL;
		}
	
		return ERRNO_SUCCESS;
		
}
/* ***********************************************************
 *  Function : l2_msg_send_hal_wait_ack
 *  return   : 0 ok, 1 error
 *  description: send set msg to hal and wait ack, used to replace "ipc_send_hal_wait_ack"
 * ***********************************************************/
/*send info confirm ack or noack*/
int l2_msg_send_hal_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
		enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
		int ret = 0;
		int rcvlen = 0;
		struct ipc_mesg_n *pSndMsg = NULL;
		struct ipc_mesg_n * pRcvMsg = NULL;
		
		pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_L2);
		if(pSndMsg != NULL)
		{
			pSndMsg->msghdr.data_len    = data_len;
			pSndMsg->msghdr.module_id   = module_id;
			pSndMsg->msghdr.sender_id   = sender_id;
			pSndMsg->msghdr.msg_type    = msg_type;
			pSndMsg->msghdr.msg_subtype = msg_subtype;
			pSndMsg->msghdr.msg_index   = msg_index;
			pSndMsg->msghdr.data_num    = data_num;
			pSndMsg->msghdr.opcode      = opcode;
			pSndMsg->msghdr.msgflag     = IPC_MSG_FLAG_SYNC;
				
			if(pdata && data_len)
			{
				memcpy(pSndMsg->msg_data, pdata, data_len);
			}
			
			/*send info*/
			ret = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_msghdr_n) + data_len, &pRcvMsg, &rcvlen, 5000);
				
			switch(ret)
			{
				/*send fail*/
				case -1:
				{
					ipc_msg_free(pSndMsg,MODULE_ID_L2);
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
							memcpy(&ret,pRcvMsg->msg_data,sizeof(ret));
									       
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
						ipc_msg_free(pRcvMsg,MODULE_ID_L2);
					}
					else
					{
						return -1;
					}
				}
				break;
				
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




/* ***********************************************************
 *  Function : l2_msg_send_hal_wait_reply
 *  return   : 0 ok , 1 error
 *  description: send get msg to hal and wait reply data, used to replace "ipc_send_hal_wait_reply"
 * ***********************************************************/

/*send info wait reply*/
int l2_msg_recv_hal_wait_reply(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
		enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
		int    iRetv = 0;
		int    iRepL = 0;
		
		struct ipc_mesg_n *pMsgSnd = NULL;
		struct ipc_mesg_n *pMsgRcv = NULL;
		
		pMsgSnd = mem_share_malloc(sizeof(struct ipc_mesg_n), MODULE_ID_L2);
		
		if(pMsgSnd == NULL) 
		{
			return ERRNO_FAIL;
		}
	
		memset(pMsgSnd, 0, sizeof(struct ipc_mesg_n));
		
		/*fill msg header*/
		pMsgSnd->msghdr.data_len    = data_len;   
		pMsgSnd->msghdr.data_num    = data_num;
		pMsgSnd->msghdr.module_id   = module_id;
		pMsgSnd->msghdr.sender_id   = sender_id;
		pMsgSnd->msghdr.msg_type    = msg_type;
		pMsgSnd->msghdr.msg_subtype = msg_subtype;
		pMsgSnd->msghdr.opcode      = opcode;
		pMsgSnd->msghdr.msg_index   = msg_index;
		
		/*send info send fail:-1 recv fail:-2 ok:0 */
		iRetv = ipc_sync_send_n1(pMsgSnd, sizeof(struct ipc_mesg_n), &pMsgRcv, &iRepL, 5000);
		
		switch(iRetv)
		{
			case -1:  
			{	/*send fail*/
				 mem_share_free(pMsgSnd, MODULE_ID_L2);
				 return ERRNO_FAIL;
			}
			case -2:  
			{
				return ERRNO_FAIL;
			}
			case  0:
			{
				if(NULL == pMsgRcv) 
				{
					return ERRNO_FAIL;
				}
				else 
				{        
					memcpy(pdata,pMsgRcv->msg_data,\
						pMsgRcv->msghdr.data_len >= data_len ? data_len : pMsgRcv->msghdr.data_len);
							
					/*recv success free memory*/
					mem_share_free(pMsgRcv, MODULE_ID_L2);
					return ERRNO_SUCCESS;
				}
					
			}    
			default: 
				 return ERRNO_FAIL;
		}
}
