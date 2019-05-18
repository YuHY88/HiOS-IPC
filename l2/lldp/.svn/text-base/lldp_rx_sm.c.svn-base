/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_rx_sm.c	
*
*  date: 2016.8.1
*
*  modify:
*
*  note:MSAP(MAC service access point)	
*
*  modify:	2018.3.12 modified by liufuying to make LLDP module code beautiful
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
//#include <arpa/inet.h>

#include <lib/log.h>
#include "memtypes.h"
#include "lib/memory.h"
#include "lib/ifm_common.h"
#include "lib/route_com.h"

#include "lldp.h"
#include "lldp_init.h"
#include "lldp_rx_sm.h"
#include "lldp_api.h"
#include "efm/efm_link_monitor.h"
#include "lib/ospf_common.h"


extern unsigned char			msap_delete_msap_buff[2048];

void lldp_arp_info_notice(struct lldp_msap *msap_cache, uint16_t arp_learning_vlan, 
															uint32_t ifindex, uint8_t opcode)
{
	struct arp_entry arp_entry_info;

	if (NULL == msap_cache)
	{
		return;
	}

	if (msap_cache->neighbor_msg.mgmt_addr_sub_type != 1)
	{
		return;
	}
	
	memset(&arp_entry_info, 0, sizeof(struct arp_entry));
	arp_entry_info.status = LLDP_ARP_STATUS_LLDP;
	arp_entry_info.key.ipaddr = (uint32_t)((msap_cache->neighbor_msg.mgmt_addr[0] << 24) +
															(msap_cache->neighbor_msg.mgmt_addr[1] << 16) +
															(msap_cache->neighbor_msg.mgmt_addr[2] << 8) +
															msap_cache->neighbor_msg.mgmt_addr[3]);
	arp_entry_info.key.vpnid = msap_cache->vpn;
	memcpy(arp_entry_info.mac, msap_cache->msap_id, 6);

	if (IFM_TYPE_IS_TRUNK(msap_cache->pkt_ifindex))
	{
		arp_entry_info.ifindex = msap_cache->pkt_ifindex;
	}
	else
	{
		if (!arp_learning_vlan)
		{
			arp_entry_info.ifindex = ifindex;
		}
		else
		{
			if (IFM_TYPE_IS_VLANIF(msap_cache->pkt_ifindex))
			{
				arp_entry_info.ifindex = msap_cache->pkt_ifindex;
			}
			else
			{
				if (IFM_IS_SUBPORT(msap_cache->pkt_ifindex))
				{
					arp_entry_info.ifindex = msap_cache->pkt_ifindex;
				}
				else
				{
					//arp_entry_info.ifindex = IFM_SUBIFINDEX_GET(msap_cache->pkt_ifindex, msap_cache->svlan);
					//return;
					arp_entry_info.ifindex = ifindex;
				}
			}
		}
	}

	LLDP_LOG_DBG("[%s]LLDP %s arp info, ip=%d.%d.%d.%d, mac=%02x%02x.%02x%02x.%02x%02x, ifindex=0x%08x, vpn=%u, status=%s\n", 
						__FUNCTION__, 
						opcode ? "add" : "delete",
						msap_cache->neighbor_msg.mgmt_addr[0],
						msap_cache->neighbor_msg.mgmt_addr[1],
						msap_cache->neighbor_msg.mgmt_addr[2],
						msap_cache->neighbor_msg.mgmt_addr[3],
						msap_cache->msap_id[0],
						msap_cache->msap_id[1],
						msap_cache->msap_id[2],
						msap_cache->msap_id[3],
						msap_cache->msap_id[4],
						msap_cache->msap_id[5], 
						arp_entry_info.ifindex,
						msap_cache->vpn,
						(arp_entry_info.status == ARP_STATUS_LLDP) ? "LLDP":"other");
//not contain any vpn information for send arp entry 2019-01-28
	if (LLDP_TRUE == opcode)
	{
		/*ipc_send_ftm(&arp_entry_info, sizeof(struct lldp_arp_entry), 1,MODULE_ID_FTM,
								MODULE_ID_L2, IPC_TYPE_ARP, ARP_INFO_LLDP, IPC_OPCODE_ADD, arp_entry_info.ifindex);*/
		ipc_send_msg_n2(&arp_entry_info, sizeof(struct lldp_arp_entry), 1,MODULE_ID_FTM,
								MODULE_ID_L2, IPC_TYPE_ARP, ARP_INFO_LLDP, IPC_OPCODE_ADD, arp_entry_info.ifindex);
	}
	else
	{
		/*ipc_send_ftm(&arp_entry_info, sizeof(struct lldp_arp_entry), 1,MODULE_ID_FTM,
								MODULE_ID_L2, IPC_TYPE_ARP, ARP_INFO_LLDP, IPC_OPCODE_DELETE, arp_entry_info.ifindex);*/
		ipc_send_msg_n2(&arp_entry_info, sizeof(struct lldp_arp_entry), 1,MODULE_ID_FTM,
								MODULE_ID_L2, IPC_TYPE_ARP, ARP_INFO_LLDP, IPC_OPCODE_DELETE, arp_entry_info.ifindex);
	}
	
	return;
}


uint8_t
lldp_tlv_cmp_update (struct lldp_neighbor_msg old_neighbor_info, struct lldp_neighbor_msg new_neighbor_info)
{
	uint8_t something_changed_remote = 0;

	if (old_neighbor_info.chassis_id_subtype != new_neighbor_info.chassis_id_subtype)
    {
      	something_changed_remote = 1;
	}

	if (old_neighbor_info.chassis_id_subtype == CHASSIS_ID_MAC_ADDRESS)
	{
		if (memcmp (old_neighbor_info.chassis_id_mac_addr, new_neighbor_info.chassis_id_mac_addr, MAC_LEN) != 0)
        {
			something_changed_remote = 1;
        }
	}
	else if (old_neighbor_info.chassis_id_subtype == CHASSIS_ID_NETWORK_ADDRESS)
	{
		if (memcmp (old_neighbor_info.chassis_id_net_addr, new_neighbor_info.chassis_id_net_addr, IPV4_LEN) != 0)
        {
			something_changed_remote = 1;
        }
	}

	if (memcmp (old_neighbor_info.network_addr, new_neighbor_info.network_addr, IPV4_LEN) != 0)
	{
		something_changed_remote = 1;
	}

	if (old_neighbor_info.port_id_subtype != new_neighbor_info.port_id_subtype)
    {
      	something_changed_remote = 1;
	}
	
	if (memcmp (old_neighbor_info.if_name, new_neighbor_info.if_name, INTERFACE_NAMSIZ) != 0)
    {
		something_changed_remote = 1;
    }

	if (memcmp (old_neighbor_info.pt_descr, new_neighbor_info.pt_descr, IFM_ALIAS_STRING_LEN+1) != 0)
    {
		something_changed_remote = 1;
    }
	
	if (memcmp (old_neighbor_info.sys_name, new_neighbor_info.sys_name, MAX_SIZE) != 0)
    {
		something_changed_remote = 1;
    }
	
	if (memcmp (old_neighbor_info.sys_descr, new_neighbor_info.sys_descr, SYSTEM_DESCR_MAX_SIZE) != 0)
    {
		something_changed_remote = 1;
    }

	if (old_neighbor_info.sys_cap != new_neighbor_info.sys_cap)
    {
      	something_changed_remote = 1;
	}

	if (old_neighbor_info.sys_cap_enabled != new_neighbor_info.sys_cap_enabled)
    {
      	something_changed_remote = 1;
	}

	if (old_neighbor_info.mgmt_addr_sub_type != new_neighbor_info.mgmt_addr_sub_type)
    {
      	something_changed_remote = 1;
	}

	if (memcmp (old_neighbor_info.mgmt_addr, new_neighbor_info.mgmt_addr, 31) != 0)
    {
		something_changed_remote = 1;
    }

	if (old_neighbor_info.if_numbering != new_neighbor_info.if_numbering)
    {
      	something_changed_remote = 1;
	}

	if (old_neighbor_info.if_number != new_neighbor_info.if_number)
    {
      	something_changed_remote = 1;
	}

	if (memcmp (old_neighbor_info.obj_oid, new_neighbor_info.obj_oid, OID_STRING_LEN) != 0)
    {
		something_changed_remote = 1;
    }

	return something_changed_remote;
}


int lldp_rx_process_frm(struct lldp_port *lldp_port) 
{
    struct eth_hdr		*ether_hdr = NULL;
	unsigned char		*buf = NULL, manage_addr_len = 0, type =0;
	unsigned char		msap_id_len = 0, update_info_ok = 0, badFrame = 0;
	unsigned short		len = 0, ttl_time = 0;
	unsigned char		msap_id[MSAP_NAMSIZ];/*smac+vlan+portid*/

	int 				ret = 0;
	
	struct lldp_msap			*msap_curr = NULL;
	struct lldp_msap			*new_msap = NULL;
	struct lldp_neighbor_msg	neighbor_msg;
	uint8_t update_arp_info_flag = LLDP_FALSE;

	int i;
	time_t uptime = 0;
	uint8_t something_changed_remote = 0;

	memset(&neighbor_msg, 0, sizeof(struct lldp_neighbor_msg));
	
	/* lldp pkt */
	ether_hdr = (struct eth_hdr *)&lldp_port->rx.frame[0];

	ether_hdr->svlan_tpid	= ntohs(ether_hdr->svlan_tpid);
	ether_hdr->svlan		= ntohs(ether_hdr->svlan);
	ether_hdr->cvlan_tpid	= ntohs(ether_hdr->cvlan_tpid);
	ether_hdr->cvlan		= ntohs(ether_hdr->cvlan);
	ether_hdr->ethertype	= ntohs(ether_hdr->ethertype);
	
    if(ether_hdr->dmac[0] !=  LLDP_DEST_MAC_0 ||
       ether_hdr->dmac[1] !=  LLDP_DEST_MAC_1 ||
       ether_hdr->dmac[2] !=  LLDP_DEST_MAC_2 ||
       ether_hdr->dmac[3] !=  LLDP_DEST_MAC_3 ||
       ether_hdr->dmac[4] !=  LLDP_DEST_MAC_4 ||
       ether_hdr->dmac[5] !=  LLDP_DEST_MAC_5 ||
       ether_hdr->ethertype != LLDP_ETH_TYPE)
	{
		/*ERROR frame:error dest mac addr or type*/
		zlog_err("%s: ERROR: error dest mac addr !!!\n", __func__);

		return -1;
    }

	/*smac*/
	memcpy(msap_id, &ether_hdr->smac[0], 6);

	/*svlan msg*/
	msap_id[6]	= (unsigned char)(ether_hdr->svlan_tpid >> 8);
	msap_id[7]	= (unsigned char)ether_hdr->svlan_tpid;
	msap_id[8]	= (unsigned char)(ether_hdr->svlan >> 8);
	msap_id[9]	= (unsigned char)ether_hdr->svlan;

	/*cvlan msg*/
	msap_id[10]	= (unsigned char)(ether_hdr->cvlan_tpid >> 8);
	msap_id[11]	= (unsigned char)ether_hdr->cvlan_tpid;
	msap_id[12]	= (unsigned char)(ether_hdr->cvlan >> 8);
	msap_id[13]	= (unsigned char)ether_hdr->cvlan;

	/*TLV msg*/
	buf = (unsigned char *)&lldp_port->rx.frame[sizeof(struct eth_hdr)];

	/*CHASSIS_ID_TLV*/
	LLDP_DECODE_TYPE_LENGTH(type, len);

	if (type != CHASSIS_ID_TLV)
	{
		lldp_port->rx.statistics.statsFramesDiscardedTotal++;
        lldp_port->rx.statistics.statsFramesInErrorsTotal++;
        badFrame++;
		
		zlog_err("%s: ERROR: First TLV is not Chassis ID TLV!!!\n", __func__);
		goto ERROR;
	}
	if((len < CHASSIS_ID_TLV_MIN_SIZE) || (len > CHASSIS_ID_TLV_MAX_SIZE))
	{
		lldp_port->rx.statistics.statsFramesDiscardedTotal++;
        lldp_port->rx.statistics.statsFramesInErrorsTotal++;
        badFrame++;
		
		zlog_err("%s: ERROR: Chassis ID TLV length is not in the range<2-256>!!!\n", __func__);
		goto ERROR;
	}

	neighbor_msg.chassis_id_subtype = *buf++;
	if(CHASSIS_ID_MAC_ADDRESS == neighbor_msg.chassis_id_subtype)
	{
		memcpy(neighbor_msg.chassis_id_mac_addr, buf, MAC_LEN);
		buf += MAC_LEN;
	}
	else if(CHASSIS_ID_NETWORK_ADDRESS == neighbor_msg.chassis_id_subtype)
	{		
		buf++; /* Ignoring the Network Address Family. see 802.1AB (table 8-2)*/
		memcpy(neighbor_msg.chassis_id_net_addr, buf, IPV4_LEN);
		buf += IPV4_LEN;		
	}

	/*PORT_ID_TLV*/
	LLDP_DECODE_TYPE_LENGTH(type, len);
	
	if (type != PORT_ID_TLV)
	{
		lldp_port->rx.statistics.statsFramesDiscardedTotal++;
        lldp_port->rx.statistics.statsFramesInErrorsTotal++;
        badFrame++;
		
		zlog_err("%s: ERROR: Second TLV is not Port ID TLV!!!\n", __func__);
		goto ERROR;
	}
	if((len < CHASSIS_ID_TLV_MIN_SIZE) || (len > CHASSIS_ID_TLV_MAX_SIZE))
	{
		lldp_port->rx.statistics.statsFramesDiscardedTotal++;
        lldp_port->rx.statistics.statsFramesInErrorsTotal++;
        badFrame++;
		
		zlog_err("%s: ERROR: Port ID TLV length is not in the range<2-256>!!!\n", __func__);
		goto ERROR;		
	}

	neighbor_msg.port_id_subtype = *buf++;
	if(PORT_ID_INTERFACE_NAME == neighbor_msg.port_id_subtype)
	{		
		if ((len - 1) > INTERFACE_NAMSIZ)
		{
			memcpy(neighbor_msg.if_name, buf, INTERFACE_NAMSIZ);
		}
		else
		{
			memcpy(neighbor_msg.if_name, buf, len-1);
		}
		memcpy(&msap_id[14], buf, len-1);/*get neighbor id: smac+vlan+portid*/
		buf += len - 1;
		msap_id_len = 14 + len -1;
		
	}/*do not support Actually*/
	else if(PORT_ID_NETWORK_ADDRESS == neighbor_msg.port_id_subtype)
	{
		buf++; /* Ignoring the Network Address Family. see 802.1AB (table 8-2)*/
		memcpy(neighbor_msg.network_addr, buf, IPV4_LEN);
		buf += IPV4_LEN;		
	}

	/*TIME_TO_LIVE_TLV*/
	LLDP_DECODE_TYPE_LENGTH(type, len);

	if(type != TIME_TO_LIVE_TLV)
	{
		lldp_port->rx.statistics.statsFramesDiscardedTotal++;
        lldp_port->rx.statistics.statsFramesInErrorsTotal++;
        badFrame++;
		
		zlog_err("%s: ERROR: Third TLV is not TTL TLV!!!\n", __func__);
		goto ERROR;
	}

	if(len != TTL_LEN)
	{
		lldp_port->rx.statistics.statsFramesDiscardedTotal++;
        lldp_port->rx.statistics.statsFramesInErrorsTotal++;
        badFrame++;
		
		zlog_err("%s: ERROR: TTL TLV length is not 2!!!\n", __func__);
		goto ERROR;			
	}

	memcpy(&ttl_time, buf, 2);
	neighbor_msg.rx_ttl = ntohs(ttl_time);
	buf += TTL_LEN;

	/*option TLV*/
	LLDP_DECODE_TYPE_LENGTH(type, len);
	
	while(type != END_OF_LLDPDU_TLV)
	{
		switch(type)
		{
			case PORT_DESCRIPTION_TLV:
			{
				/*add tlv lenth judge*/
				if (len > IFM_ALIAS_STRING_LEN)
				{
					memcpy(neighbor_msg.pt_descr, buf, IFM_ALIAS_STRING_LEN);
				}
				else
				{
					memcpy(neighbor_msg.pt_descr, buf, len);
				}
				buf += len;
			}
				break;
			case SYSTEM_NAME_TLV:
			{
				if (len > MAX_SIZE)
				{
					memcpy(neighbor_msg.sys_name, buf, MAX_SIZE);
				}
				else
				{
					memcpy(neighbor_msg.sys_name, buf, len);
				}
				buf += len;
			}
				break;
			case SYSTEM_DESCRIPTION_TLV:
			{
				if (len > SYSTEM_DESCR_MAX_SIZE)
				{
					memcpy(neighbor_msg.sys_descr, buf, SYSTEM_DESCR_MAX_SIZE);
				}
				else
				{
					memcpy(neighbor_msg.sys_descr, buf, len);
				}
				buf += len;
			}
				break;
			case SYSTEM_CAPABILITIES_TLV:
			{
				LLDP_TLV_UINT16_DECODE (buf, neighbor_msg.sys_cap);
				LLDP_TLV_UINT16_DECODE (buf, neighbor_msg.sys_cap_enabled);
			}
				break;
			case MANAGEMENT_ADDRESS_TLV:
			{
				manage_addr_len = *buf++;
				neighbor_msg.mgmt_addr_sub_type = *buf++;
				memcpy(neighbor_msg.mgmt_addr, buf, manage_addr_len-1);
				buf += manage_addr_len-1;

				neighbor_msg.if_numbering = *buf++;	
				LLDP_TLV_UINT32_DECODE(buf, neighbor_msg.if_number);

				neighbor_msg.oid_string_len = *buf++;
				memcpy(neighbor_msg.obj_oid, buf, neighbor_msg.oid_string_len);
				buf += neighbor_msg.oid_string_len;
				
			}
				break;
			default:	/*802.1 & 802.3 & other TLV do not support*/
				buf += len;
				break;
		}

		LLDP_DECODE_TYPE_LENGTH(type, len);
	}

	/*update info*/
	msap_curr = lldp_port->msap_cache;
	while(msap_curr != NULL)
	{
		if(msap_id_len == msap_curr->msap_id_length)
		{
			if(memcmp(msap_id, msap_curr->msap_id, msap_id_len) == 0)
			{
				//remote something changed
				//if(memcmp(&msap_curr->neighbor_msg, &neighbor_msg, sizeof(struct lldp_neighbor_msg)) != 0)
				something_changed_remote = lldp_tlv_cmp_update(msap_curr->neighbor_msg, neighbor_msg);
				if (something_changed_remote != 0)
				{
					if (msap_curr->neighbor_msg.time_mark_index >= 9)
					{
						msap_curr->neighbor_msg.time_mark_index = 0;
					}
					else
					{
						msap_curr->neighbor_msg.time_mark_index++;
					}

					neighbor_msg.time_mark_index = msap_curr->neighbor_msg.time_mark_index;

					uptime = efm_time_current(NULL);
					msap_curr->neighbor_msg.time_mark[neighbor_msg.time_mark_index] = uptime;
					memcpy(neighbor_msg.time_mark, msap_curr->neighbor_msg.time_mark, TIME_MARK_INDEX_MAX*sizeof(time_t));
				}
				else
				{
					memcpy(neighbor_msg.time_mark, msap_curr->neighbor_msg.time_mark, TIME_MARK_INDEX_MAX*sizeof(time_t));
				}
			
				if(neighbor_msg.rx_ttl == 0)
				{
					msap_curr->rxInfoTTL = neighbor_msg.rx_ttl;
				}
				else
				{
					msap_curr->rxInfoTTL = neighbor_msg.rx_ttl;

					/*
						arp learning, arp info update
						1) enable arp learning;
						2) management address is ip
						3) management address change
						4) arp mac address is msap address;
					*/
					if (CHECK_FLAG(lldp_port->arp_nd_learning, LLDP_LEARNING_ARP))
					{
						if ((memcmp(msap_curr->neighbor_msg.mgmt_addr, neighbor_msg.mgmt_addr, 6) != 0) ||
							(lldp_port->pkt_ifindex != msap_curr->pkt_ifindex))
						{
							if (LLDP_TRUE == msap_curr->arp_notice_flag)
							{
								lldp_arp_info_notice(msap_curr, lldp_port->arp_nd_learning_vlan, lldp_port->if_index, LLDP_FALSE);
								msap_curr->arp_notice_flag = LLDP_FALSE;
							}
							update_arp_info_flag = LLDP_TRUE;
							msap_curr->pkt_ifindex = lldp_port->pkt_ifindex;
							msap_curr->vpn = lldp_port->l3_vpnid;
						}
					}
					
					memset(&msap_curr->neighbor_msg, 0, sizeof(struct lldp_neighbor_msg));			
					msap_curr->neighbor_msg = neighbor_msg;

					if ((LLDP_TRUE == update_arp_info_flag) &&
						CHECK_FLAG(lldp_port->arp_nd_learning, LLDP_LEARNING_ARP))
					{
						if (/*(lldp_port->arp_nd_learning_vlan == msap_curr->svlan) && */
							(1 == neighbor_msg.mgmt_addr_sub_type))
						{
							lldp_arp_info_notice(msap_curr, lldp_port->arp_nd_learning_vlan, lldp_port->if_index, LLDP_TRUE);
							msap_curr->arp_notice_flag = LLDP_TRUE;
						}
					}
				}
				update_info_ok = 1;
			}
		}
		msap_curr = msap_curr->next;
	}

	if(0 == update_info_ok)
	{
		new_msap = XMALLOC(MTYPE_LLDP_MSAP, sizeof(struct lldp_msap));
		memset(new_msap, 0, sizeof(struct lldp_msap));

		memcpy(&new_msap->msap_id[0], &msap_id[0], msap_id_len);
		new_msap->msap_id_length	= msap_id_len;
		new_msap->svlan_tpid		= ether_hdr->svlan_tpid;
		new_msap->svlan				= ether_hdr->svlan;
		new_msap->cvlan_tpid		= ether_hdr->cvlan_tpid;
		new_msap->cvlan				= ether_hdr->cvlan;

		new_msap->vpn = lldp_port->l3_vpnid;
		new_msap->pkt_ifindex = lldp_port->pkt_ifindex;
		
		new_msap->rxInfoTTL = neighbor_msg.rx_ttl;
		new_msap->neighbor_msg = neighbor_msg;
		new_msap->next = NULL;

		/*add arp info*/
		if (CHECK_FLAG(lldp_port->arp_nd_learning, LLDP_LEARNING_ARP))
		{		
			if (/*(lldp_port->arp_nd_learning_vlan == new_msap->svlan) &&*/ 
				(1 == neighbor_msg.mgmt_addr_sub_type))
			{
				lldp_arp_info_notice(new_msap, lldp_port->arp_nd_learning_vlan, lldp_port->if_index, LLDP_TRUE);
				new_msap->arp_notice_flag = LLDP_TRUE;
			}
		}

		for (i = 0; i < TIME_MARK_INDEX_MAX; i++)
		{
			new_msap->neighbor_msg.time_mark[i] = -1;
		}
		
		new_msap->neighbor_msg.time_mark_index = 0;
		uptime = efm_time_current(NULL);
		new_msap->neighbor_msg.time_mark[new_msap->neighbor_msg.time_mark_index] = uptime;
		
		/*add new cache in the msap_cache head */
		new_msap->next = lldp_port->msap_cache;
		lldp_port->msap_cache = new_msap;
		if(lldp_port->msap_num < 255)
		{
			lldp_port->msap_num += 1;
		}
		
		lldp_port->rxChanges = LLDP_TRUE;
	    lldp_port->rx.somethingChangedRemote = LLDP_TRUE;

		/*do something, trap msg*/
		ret = lldp_trap_insert_msap_to_snmpd(lldp_port, new_msap);
		if(ret < 0)
		{
			zlog_err("%s[%d]: ERROR The LLDP trap Inserts Failed!\n", __FUNCTION__,__LINE__);
			return -1;
		}

	}
	
ERROR:
	if(badFrame)/*error frame*/
	{	
		lldp_port->rx.badFrame = LLDP_TRUE;
		
		/*do something for recv bad msg, trap msg*/

		return -1;
	}
	else
	{
		lldp_port->rx.statistics.statsFramesInTotal++;
	}

	return 0;
}

unsigned char lldp_delete_neighbor(struct lldp_port *lldp_port)
{
	struct lldp_msap *current = lldp_port->msap_cache;
	struct lldp_msap *tail    = NULL;
	struct lldp_msap *tmp     = NULL;

	unsigned int	 msap_num = 0;

	while(current != NULL)
	{	
    	if(current->rxInfoTTL <= 0)
		{
			if(lldp_port->msap_num > 0)
			{
				lldp_port->msap_num -= 1;
			}
			
			if(current == lldp_port->msap_cache)
			{
				lldp_port->msap_cache = current->next;
			}
			else
			{
				tail->next = current->next;
			}

			tmp = current;
			current = current->next;

			/*delete arp info*/
			if (CHECK_FLAG(lldp_port->arp_nd_learning, LLDP_LEARNING_ARP))
			{
				if (LLDP_TRUE == tmp->arp_notice_flag)
				{
					lldp_arp_info_notice(tmp, lldp_port->arp_nd_learning_vlan, lldp_port->if_index, LLDP_FALSE);
					tmp->arp_notice_flag = LLDP_FALSE;
				}
			}

			/**/
			lldp_add_delete_msap_info(tmp, msap_num);
			msap_num++;


			/*clear neighbor msg*/
			memset(&tmp->neighbor_msg, 0, sizeof(struct lldp_neighbor_msg));
			memset(&tmp->msap_id, 0, MSAP_NAMSIZ);
			
			lldp_port->rx.statistics.statsAgeoutsTotal++;
			lldp_port->rxChanges = LLDP_TRUE;

			XFREE(MTYPE_LLDP_MSAP, tmp);			
		}
		else
		{
			tail = current;
			current = current->next;
		} 
  	}

	return 0;
}


void lldp_rx_change_state(struct lldp_port *lldp_port, unsigned char state)
{
	switch(state)
	{
        case LLDP_WAIT_PORT_OPERATIONAL:
		{
            /*Do nothing*/
        }
			break;
		case DELETE_AGED_INFO:
		{
        	if(lldp_port->rx.state != LLDP_WAIT_PORT_OPERATIONAL)
			{
				/*ERROR Illegal Transition*/
				zlog_err("%s: ERROR LLDP STATE Illegal Transition:rx.state != LLDP_WAIT_PORT_OPERATIONAL!\n", __func__);			
			}
		}
			break;
        case RX_WAIT_FOR_FRAME:
		{    
			if(!(lldp_port->rx.state == LLDP_WAIT_PORT_OPERATIONAL ||
				lldp_port->rx.state == DELETE_INFO ||
				lldp_port->rx.state == RX_FRAME))
			{
				/*ERROR Illegal Transition*/
				zlog_err("%s: ERROR LLDP STATE Illegal Transition:LLDP_WAIT_PORT_OPERATIONAL || DELETE_INFO || RX_FRAME!\n", __func__);			
			}
        }
			break;
        case RX_FRAME:
		{
			if(lldp_port->rx.state != RX_WAIT_FOR_FRAME)
			{
				/*ERROR Illegal Transition*/
				zlog_err("%s: ERROR LLDP STATE Illegal Transition:rx.state != RX_WAIT_FOR_FRAME!\n", __func__);			
			}
		}
			break;
        case DELETE_INFO: 
		{
			if( lldp_port->rx.state != RX_WAIT_FOR_FRAME )
			{
				/*ERROR: Illegal Transition*/
				zlog_err("%s: ERROR LLDP STATE Illegal Transition:	RX_WAIT_FOR_FRAME\n", __func__);			
            }
		}
			break;
        default:
		{
			/*ERROR Illegal Transition*/            
			zlog_err("%s: ERROR: LLDP STATE Illegal Transition: default!\n", __func__);
         
			break;
		};
    };

    /* Now update the interface state */
    lldp_port->rx.state = state;
}

unsigned char lldp_rx_state_machine_run(struct lldp_port *lldp_port)
{
	if((LLDP_FALSE == lldp_port->rx.rxInfoAge) && (LLDP_FALSE == lldp_port->portEnabled))
    {
		lldp_rx_change_state(lldp_port, LLDP_WAIT_PORT_OPERATIONAL);
    }
  
	switch(lldp_port->rx.state)
	{
		case LLDP_WAIT_PORT_OPERATIONAL:
		{
			if(LLDP_TRUE == lldp_port->rx.rxInfoAge)
			{
				lldp_rx_change_state(lldp_port, DELETE_AGED_INFO);
				break;
			}
			
			if((LLDP_TRUE == lldp_port->portEnabled) && (enabledRxTx == lldp_port->adminStatus))
			{
				lldp_rx_change_state(lldp_port, RX_WAIT_FOR_FRAME);
				break;
			}			
		}
			break;
		case DELETE_AGED_INFO:
		{
		}
			break;
    	case RX_WAIT_FOR_FRAME:
      	{
			if(LLDP_TRUE == lldp_port->rx.rxInfoAge)
			{
				lldp_rx_change_state(lldp_port, DELETE_INFO);
				break;
			}
			
			if(LLDP_TRUE == lldp_port->rx.rcvFrame)
			{
				lldp_rx_change_state(lldp_port, RX_FRAME);
				break;
			}
			
			if(disabled == lldp_port->adminStatus)
			{
				lldp_rx_change_state(lldp_port, LLDP_WAIT_PORT_OPERATIONAL);
			}
		}
			break;
		case DELETE_INFO:
		{
			
		}
			break;
		case RX_FRAME:
		{
		}
			break;
		default:
            /*ERROR The RX Global State Machine is broken!*/
			zlog_err("%s: ERROR The RX Global State Machine is broken!\n", __func__);
			break;
	}
  
	return 0;
}


void lldp_rx_state_machine_action(struct lldp_port *lldp_port)
{
	lldp_rx_state_machine_run(lldp_port);

	switch(lldp_port->rx.state)
	{
		case LLDP_WAIT_PORT_OPERATIONAL:
		{
			/*do nothing*/
		}
			break;
		case DELETE_AGED_INFO:
		{
			lldp_rx_delete_aged_info(lldp_port);
			
			lldp_rx_change_state(lldp_port, LLDP_WAIT_PORT_OPERATIONAL);
		}
			break;
		case RX_WAIT_FOR_FRAME:
		{
			lldp_rx_wait_for_frame(lldp_port);
		}
			break;
		case RX_FRAME:
		{
			lldp_rx_deal_frame(lldp_port);
			
			lldp_rx_change_state(lldp_port, RX_WAIT_FOR_FRAME);
		}
			break;
		case DELETE_INFO:
		{
			lldp_rx_delete_info(lldp_port);
			lldp_rx_change_state(lldp_port, RX_WAIT_FOR_FRAME);
		}
			break;
		default:
			/*ERROR] The RX State Machine is broken!*/
			zlog_err("%s: ERROR: LLDP RX MACHINE STATE: default!\n", __func__);
			break;
    }
}

void lldp_rx_decrement_timer(unsigned short *timer)
{
	if((*timer) > 0)
	{
		(*timer)--;
	}

	return;
}

void lldp_rx_timers(struct lldp_port *lldp_port)
{
	struct lldp_msap *msap_cache = lldp_port->msap_cache;

	while((msap_cache != NULL) && (lldp_port->msap_num > 0))
	{
		lldp_rx_decrement_timer(&msap_cache->rxInfoTTL);

	    if(msap_cache->rxInfoTTL <= 0)
	    {
			lldp_port->rx.rxInfoAge = LLDP_TRUE;
			lldp_rx_state_machine_action(lldp_port);

			return;
		}

		msap_cache = msap_cache->next;
	}

	return;
}

void lldp_rx_delete_aged_info(struct lldp_port *lldp_port)
{
    int msap_num = lldp_port->msap_num;
	int delete_msap_num = 0;
	int trap_msap_info_len = 0;

	memset(msap_delete_msap_buff, 0x00, 2048);
	
	msap_delete_msap_buff[0] = 0;
	memcpy(&msap_delete_msap_buff[1],lldp_port->if_name, INTERFACE_NAMSIZ);


    lldp_delete_neighbor(lldp_port);
    lldp_port->rx.rxInfoAge = LLDP_FALSE;

	if(LLDP_TRUE == lldp_port->rxChanges)
	{   
		lldp_port->rx.somethingChangedRemote = LLDP_TRUE;
		/*do something, trap msg*/

		/*do something, trap msg*/	
		delete_msap_num = msap_num - lldp_port->msap_num;		
		msap_delete_msap_buff[0] = delete_msap_num;	
		if(delete_msap_num > 0)
		{
			trap_msap_info_len = delete_msap_num*(sizeof(struct lldp_neighbor_trap_info)) + 33;

			/*send trap*/
			lldp_trap_ageout_msap_to_snmpd(trap_msap_info_len);
		}		
	}
}

void lldp_rx_wait_for_frame(struct lldp_port *lldp_port)
{
    lldp_port->rx.badFrame = LLDP_FALSE;
    lldp_port->rx.rxInfoAge = LLDP_FALSE;
    lldp_port->rx.somethingChangedRemote = LLDP_FALSE;
}

void lldp_rx_deal_frame(struct lldp_port *lldp_port)
{
    lldp_port->rxChanges = LLDP_FALSE;
    lldp_port->rx.rcvFrame = LLDP_FALSE;
    lldp_rx_process_frm(lldp_port);

    /* Clear the frame buffer out to avoid weird problems.*/
    memset(&lldp_port->rx.frame[0], 0x0, lldp_port->rx.recvsize);
}

void lldp_rx_delete_info(struct lldp_port *lldp_port)
{
    int msap_num = lldp_port->msap_num;
	int delete_msap_num = 0;
	int trap_msap_info_len = 0;

	memset(msap_delete_msap_buff, 0x00, 2048);
	
	msap_delete_msap_buff[0] = 0;
	memcpy(&msap_delete_msap_buff[1],lldp_port->if_name, INTERFACE_NAMSIZ);

    lldp_delete_neighbor(lldp_port);
	
    lldp_port->rx.rxInfoAge = LLDP_FALSE;
	
	if(LLDP_TRUE == lldp_port->rxChanges)
	{   
		lldp_port->rx.somethingChangedRemote = LLDP_TRUE;
		
		/*do something, trap msg*/
		delete_msap_num = msap_num - lldp_port->msap_num;
		msap_delete_msap_buff[0] = delete_msap_num;	
		
		if(delete_msap_num > 0)
		{
			trap_msap_info_len = delete_msap_num*(sizeof(struct lldp_neighbor_trap_info)) + 33;

			/*send trap*/
			lldp_trap_delete_msap_to_snmpd(trap_msap_info_len);
		}
	}	
}

