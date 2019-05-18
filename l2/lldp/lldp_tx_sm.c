/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_tx_sm.c		(lldp transmit state machine)
*
*  date: 2016.7.29
*
*  modify:	2018.3.12 modified by liufuying to make LLDP module code beautiful
*
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "memtypes.h"
#include "lib/memory.h"
#include "lib/log.h"
#include "lib/types.h"
#include "lib/pkt_buffer.h"
#include "lib/module_id.h"
#include "lib/ifm_common.h"

#include "l2/l2_if.h"

#include "lldp_init.h"
#include "lldp_tx_sm.h"
#include "lldp_tlv.h"
#include "lldp_rx_sm.h"


extern lldp_global lldp_global_msg;


void lldp_get_management_ip(struct lldp_port *lldp_port)
{
	unsigned int			manege_if_index = 0;
	struct ifm_l3		l3;
	unsigned int			ipaddr = 0; 
	char vlanif_name[10] = {0};
	int ret = 0;

	memset(lldp_global_msg.source_ipaddr, 0, IPV4_LEN);

	/*get loopback 0 iP addr*/
	manege_if_index = ifm_get_ifindex_by_name("loopback", (char *)"128");
	ret = ifm_get_l3if(manege_if_index , MODULE_ID_L2, &l3);	
	if((!ret) && 
		(ipv4_is_valid(ntohl(l3.ipv4[0].addr))))
	{
		ipaddr = l3.ipv4[0].addr;
	}
	else
	{
		LLDP_LOG_DBG("%s:Error: Get management IP from loopback 0 timeout...L3if is NULL\n", __func__);
		if (NULL != lldp_port && lldp_port->arp_nd_learning_vlan)
		{
			sprintf(vlanif_name, "%d", lldp_port->arp_nd_learning_vlan);
			manege_if_index = ifm_get_ifindex_by_name("vlanif", (char *)vlanif_name);
			ret = ifm_get_l3if(manege_if_index , MODULE_ID_L2, &l3);
			if (!ret)
			{
				ipaddr = l3.ipv4[0].addr;
			}
		}
	}

	if(!ipv4_is_valid(ntohl(ipaddr)))
	{
		return;
	}

	lldp_global_msg.source_ipaddr[0] = (unsigned char)(ipaddr >> 24);
	lldp_global_msg.source_ipaddr[1] = (unsigned char)(ipaddr >> 16);
	lldp_global_msg.source_ipaddr[2] = (unsigned char)(ipaddr >> 8);
	lldp_global_msg.source_ipaddr[3] = (unsigned char)(ipaddr);
			
	return;
}

void lldp_encap_frame(struct lldp_port *lldp_port, struct lldp_msap *msap_cache)
{
	unsigned char			*buf = NULL;
	unsigned short			tlv_header = 0;
	unsigned short			pkt_len = 0;
	struct l2if *pif = NULL;
	char alias[IFM_ALIAS_STRING_LEN+1] = {0};
	
	memset (alias, 0, IFM_ALIAS_STRING_LEN+1);

	buf = (unsigned char*)&lldp_port->tx.frame[0];

	/*dmac*/
	*buf++ = LLDP_DEST_MAC_0;
	*buf++ = LLDP_DEST_MAC_1;
	*buf++ = LLDP_DEST_MAC_2;
	*buf++ = LLDP_DEST_MAC_3;
	*buf++ = LLDP_DEST_MAC_4;
	*buf++ = LLDP_DEST_MAC_5;
	pkt_len += 6;

	/*smac*/
	//memcpy(buf, &lldp_global_msg.source_mac[0], 6);
	memcpy(buf, lldp_port->if_mac, 6);
	buf += 6;
	pkt_len += 6;

	/*vlan msg*/
	#if 0
	if(msap_cache != NULL)
	{
		/*svlan*/
		if((msap_cache->svlan_tpid != 0x0000) && (msap_cache->svlan != 0x0000))
		{
			*buf++	= (unsigned char)(msap_cache->svlan_tpid >> 8);
			*buf++	= (unsigned char)msap_cache->svlan_tpid;
			*buf++	= (unsigned char)(msap_cache->svlan >> 8);
			*buf++	= (unsigned char)msap_cache->svlan;
			pkt_len += 4;
		}
		/*cvlan*/
		if((msap_cache->cvlan_tpid != 0x0000) && (msap_cache->cvlan != 0x0000))
		{
			*buf++	= (unsigned char)(msap_cache->cvlan_tpid >> 8);
			*buf++	= (unsigned char)msap_cache->cvlan_tpid;
			*buf++	= (unsigned char)(msap_cache->cvlan >> 8);
			*buf++	= (unsigned char)msap_cache->cvlan;
			pkt_len += 4;
		}		
	}
	#endif
#if 0
	if (lldp_port->arp_nd_learning_vlan)
	{
		dot1q_tpid = 0x8100;
		*buf++	= (unsigned char)(dot1q_tpid >> 8);
		*buf++	= (unsigned char)dot1q_tpid;
		*buf++	= (unsigned char)(lldp_port->arp_nd_learning_vlan >> 8);
		*buf++	= (unsigned char)lldp_port->arp_nd_learning_vlan;
		pkt_len += 4;
	}
#endif
	/*eth type*/
	*buf++ = (unsigned char)(LLDP_ETH_TYPE >> 8);
	*buf++ = (unsigned char)LLDP_ETH_TYPE;
	pkt_len += 2;
	
/*add basic Mandatory TLV*/

	/*CHASSIS_ID_TLV, subtype is MAC*/
	LLDP_ENCODE_TYPE_LENGTH(CHASSIS_ID_TLV, MAC_LEN+1);
	*buf++ = CHASSIS_ID_MAC_ADDRESS;
	memcpy(buf, &lldp_global_msg.source_mac[0], MAC_LEN);
	buf += MAC_LEN;	
	pkt_len += MAC_LEN + 3;


	/*PORT_ID_TLV*/
	LLDP_ENCODE_TYPE_LENGTH(PORT_ID_TLV, strlen(lldp_port->if_name) + 1);
	*buf++ = PORT_ID_INTERFACE_NAME;
	memcpy(buf, &lldp_port->if_name[0], strlen(lldp_port->if_name));
	buf += strlen(lldp_port->if_name);
	pkt_len += strlen(lldp_port->if_name) + 3;

	/*TIME_TO_LIVE_TLV*/
	LLDP_ENCODE_TYPE_LENGTH (TIME_TO_LIVE_TLV, TTL_LEN);
	*buf++ = (unsigned char)(lldp_port->tx.txTTL >> 8);
	*buf++ = (unsigned char)lldp_port->tx.txTTL;
	pkt_len += TTL_LEN + 2;


/*add basic Optional TLV*/

	/*PORT_DESCRIPTION_TLV*/
	if(CHECK_FLAG(lldp_port->tlv_cfg.basic_tlv_tx_enable, 
					PORT_DESCRIPTION_TLV_TX_ENABLE))
	{
		#if 0
		LLDP_ENCODE_TYPE_LENGTH (PORT_DESCRIPTION_TLV, strlen(lldp_port->if_name));
		memcpy(buf, &lldp_port->if_name[0], strlen(lldp_port->if_name));
		buf += strlen(lldp_port->if_name);
		pkt_len += strlen(lldp_port->if_name) + 2;
		#else
		pif = l2if_lookup(lldp_port->if_index);
		if((NULL == pif) ||
			(!strcmp(pif->alias, alias)))
		{
			LLDP_ENCODE_TYPE_LENGTH (PORT_DESCRIPTION_TLV, strlen(lldp_port->if_name));
			memcpy(buf, &lldp_port->if_name[0], strlen(lldp_port->if_name));
			buf += strlen(lldp_port->if_name);
			pkt_len += strlen(lldp_port->if_name) + 2;
		}
		else
		{
			LLDP_ENCODE_TYPE_LENGTH (PORT_DESCRIPTION_TLV, strlen(pif->alias));
			memcpy(buf, &pif->alias[0], strlen(pif->alias));
			buf += strlen(pif->alias);
			pkt_len += strlen(pif->alias) + 2;
		}
		#endif
	}

	/*SYSTEM_NAME_TLV*/
	if(CHECK_FLAG(lldp_port->tlv_cfg.basic_tlv_tx_enable, 
					SYSTEM_NAME_TLV_TX_ENABLE))
	{
		LLDP_ENCODE_TYPE_LENGTH (SYSTEM_NAME_TLV, strlen((const char *)&lldp_global_msg.system_name));
		memcpy(buf, &lldp_global_msg.system_name[0], strlen((const char *)&lldp_global_msg.system_name));
		buf += strlen((const char *)&lldp_global_msg.system_name);
		pkt_len += strlen((const char *)&lldp_global_msg.system_name) + 2;
	}

	/*SYSTEM_DESCRIPTION_TLV*/
	if(CHECK_FLAG(lldp_port->tlv_cfg.basic_tlv_tx_enable, 
					SYSTEM_DESCRIPTION_TLV_TX_ENABLE))
	{
		LLDP_ENCODE_TYPE_LENGTH (SYSTEM_DESCRIPTION_TLV, strlen((const char *)&lldp_global_msg.system_desc));
		memcpy(buf, &lldp_global_msg.system_desc[0], strlen((const char *)&lldp_global_msg.system_desc));
		buf += strlen((const char *)&lldp_global_msg.system_desc);
		pkt_len += strlen((const char *)&lldp_global_msg.system_desc) + 2;		
	}

	/*SYSTEM_CAPABILITIES_TLV*/
	if(CHECK_FLAG(lldp_port->tlv_cfg.basic_tlv_tx_enable, 
					SYSTEM_CAPABILITIES_TLV_TX_ENABLE))
	{
		LLDP_ENCODE_TYPE_LENGTH (SYSTEM_CAPABILITIES_TLV, 4);
		*buf++ = (unsigned char)(lldp_port->sys_cap >> 8);
		*buf++ = (unsigned char)(lldp_port->sys_cap);
		*buf++ = (unsigned char)(lldp_port->sys_cap_enable >> 8);
		*buf++ = (unsigned char)(lldp_port->sys_cap_enable);	
		pkt_len += 4 + 2;		
	}

	/*MANAGEMENT_ADDRESS_TLV*/
	if(CHECK_FLAG(lldp_port->tlv_cfg.basic_tlv_tx_enable, 
					MANAGEMENT_ADDRESS_TLV_TX_ENABLE))
	{
		unsigned int			if_index;
		#if 0
		unsigned int			manege_if_index = 0;
		struct ifm_l3			*l3 = NULL;
		unsigned int			ipaddr = 0; 
		#endif
	
		if(lldp_global_msg.mng_addr_sub == MGMT_ADDR_SUB_ALL802)
		{
			LLDP_ENCODE_TYPE_LENGTH (MANAGEMENT_ADDRESS_TLV, MGMT_ADDR_MAC_TLV_LENGTH);
			*buf++ = MGMT_ADDR_SUBTYPE + MAC_LEN;
			*buf++ = MGMT_ADDR_SUB_ALL802;
			memcpy(buf, &lldp_global_msg.source_mac[0], 6);
			buf += 6;
			pkt_len += 8 + 2;		
		}
		else if(lldp_global_msg.mng_addr_sub == MGMT_ADDR_SUB_IPV4)
		{	
			#if 0
			memset(lldp_global_msg.source_ipaddr, 0, IPV4_LEN);

			/*get loopback 0 iP addr*/
			manege_if_index = ifm_get_ifindex_by_name("loopback", (char *)"0");
			l3 = ifm_get_l3if(manege_if_index , MODULE_ID_L2);	
			if(l3 == NULL)
			{
				LLDP_LOG_DBG("%s:Error: get management IP timeout...	L3if is NULL\n", __func__);			
			}
			else
			{
				ipaddr = l3->ipv4[0].addr;
				lldp_global_msg.source_ipaddr[0] = (unsigned char)(ipaddr >> 24);
				lldp_global_msg.source_ipaddr[1] = (unsigned char)(ipaddr >> 16);
				lldp_global_msg.source_ipaddr[2] = (unsigned char)(ipaddr >> 8);
				lldp_global_msg.source_ipaddr[3] = (unsigned char)(ipaddr);
			}
			#else
			lldp_get_management_ip(lldp_port);
			#endif

			if(lldp_global_msg.source_ipaddr[0] == 0x00
				/*&&lldp_global_msg.source_ipaddr[1] == 0x00
				&&lldp_global_msg.source_ipaddr[2] == 0x00
				&&lldp_global_msg.source_ipaddr[3] == 0x00*/)
			{
				LLDP_LOG_DBG("%s:Error: can not get management IP address...\n", __func__);

				/*if do not get IP address encapsulated MAC address*/
				LLDP_ENCODE_TYPE_LENGTH (MANAGEMENT_ADDRESS_TLV, MGMT_ADDR_MAC_TLV_LENGTH);
				*buf++ = MGMT_ADDR_SUBTYPE + MAC_LEN;
				*buf++ = MGMT_ADDR_SUB_ALL802;
				memcpy(buf, &lldp_global_msg.source_mac[0], 6);
				buf += 6;
				pkt_len += 8 + 2;	
			}
			else
			{
				LLDP_ENCODE_TYPE_LENGTH (MANAGEMENT_ADDRESS_TLV, MGMT_ADDR_IP_TLV_LENGTH);
				*buf++ = MGMT_ADDR_SUBTYPE + IPV4_LEN;
				*buf++ = MGMT_ADDR_SUB_IPV4;

				memcpy(buf, &lldp_global_msg.source_ipaddr[0], 4);						
				buf += 4;
				pkt_len += 6 + 2;
			}
		}

		*buf++ = IF_NUMBERING_IFINDEX;
		if_index = htonl(lldp_port->if_index);
		/*Interface number*/ 
		memcpy(buf, &if_index, sizeof(unsigned int));
		buf += 4;

		/*OID - 0 for us*/ 
		*buf = 0;
		pkt_len += 6;

		/*object identifier...*/
		//FIXME		
	}

	/*END_OF_LLDPDU_TLV*/
	LLDP_ENCODE_TYPE_LENGTH(END_OF_LLDPDU_TLV, 0);
	
	pkt_len += 2;
	
    if(pkt_len < 64)
	{
        lldp_port->tx.sendsize = 64;
    }
	else
	{
		lldp_port->tx.sendsize = pkt_len;
    }

/************send pkt**********************************************/
#if 0
	if(lldp_port->portEnabled == FALSE)
	{
		if(lldp_port->tx.frame != NULL)
		{
			memset(&lldp_port->tx.frame[0], 0, lldp_port->tx.sendsize);
		}

		return 0;
	}

	memset(&pktcontrol, 0, sizeof(union pkt_control));
		
	/*dmac*/
	pktcontrol.ethcb.dmac[0] = LLDP_DEST_MAC_0;
	pktcontrol.ethcb.dmac[1] = LLDP_DEST_MAC_1;
	pktcontrol.ethcb.dmac[2] = LLDP_DEST_MAC_2;
	pktcontrol.ethcb.dmac[3] = LLDP_DEST_MAC_3;
	pktcontrol.ethcb.dmac[4] = LLDP_DEST_MAC_4;
	pktcontrol.ethcb.dmac[5] = LLDP_DEST_MAC_5;
		
	/*smac*/
	memcpy(pktcontrol.ethcb.smac, &lldp_global_msg.source_mac[0], 6);

	/*vlan msg*/
	if(msap_cache != NULL)
	{
		/*svlan*/
		if((msap_cache->svlan_tpid != 0x0000) && (msap_cache->svlan != 0x0000))
		{
			pktcontrol.ethcb.svlan_tpid	= htons(msap_cache->svlan_tpid);
			pktcontrol.ethcb.svlan	= htons(msap_cache->svlan_tpid);
		}
		/*cvlan*/
		if((msap_cache->cvlan_tpid != 0x0000) && (msap_cache->cvlan != 0x0000))
		{
			pktcontrol.ethcb.cvlan_tpid = htons(msap_cache->cvlan_tpid);
			pktcontrol.ethcb.cvlan = htons(msap_cache->cvlan);
		}		
	}
		
	pktcontrol.ethcb.ethtype = htons(LLDP_ETH_TYPE);
	pktcontrol.ethcb.is_changed = 1;
	pktcontrol.ethcb.smac_valid = 1;		
	pktcontrol.ethcb.ifindex = lldp_port->if_index;

	pkt_send(PKT_TYPE_ETH, &pktcontrol, lldp_port->tx.frame, lldp_port->tx.sendsize);

	if(lldp_port->tx.frame != NULL)
	{
		memset(&lldp_port->tx.frame[0], 0, lldp_port->tx.sendsize);
	}

	lldp_port->tx.statistics.statsFramesOutTotal++;	

#endif	
/**********************************************************/

	return;
}

void lldp_encap_shutdown_frame(struct lldp_port *lldp_port, struct lldp_msap *msap_cache)
{
	unsigned char			*buf = NULL;
	unsigned short			tlv_header = 0;
	unsigned short			pkt_len = 0;	
	
	buf = (unsigned char*)&lldp_port->tx.frame[0];

	/*dmac*/
	*buf++ = LLDP_DEST_MAC_0;
	*buf++ = LLDP_DEST_MAC_1;
	*buf++ = LLDP_DEST_MAC_2;
	*buf++ = LLDP_DEST_MAC_3;
	*buf++ = LLDP_DEST_MAC_4;
	*buf++ = LLDP_DEST_MAC_5;
	pkt_len += 6;

	/*smac*/
	//memcpy(buf, &lldp_global_msg.source_mac[0], 6);
	memcpy(buf, lldp_port->if_mac, 6);
	buf += 6;
	pkt_len += 6;

	/*vlan msg*/
	#if 0
	if(msap_cache != NULL)
	{
		/*svlan*/
		if((msap_cache->svlan_tpid != 0x0000) && (msap_cache->svlan != 0x0000))
		{
			*buf++	= (unsigned char)(msap_cache->svlan_tpid >> 8);
			*buf++	= (unsigned char)msap_cache->svlan_tpid;
			*buf++	= (unsigned char)(msap_cache->svlan >> 8);
			*buf++	= (unsigned char)msap_cache->svlan;
			pkt_len += 4;
		}
		/*cvlan*/
		if((msap_cache->cvlan_tpid != 0x0000) && (msap_cache->cvlan != 0x0000))
		{
			*buf++	= (unsigned char)(msap_cache->cvlan_tpid >> 8);
			*buf++	= (unsigned char)msap_cache->cvlan_tpid;
			*buf++	= (unsigned char)(msap_cache->cvlan >> 8);
			*buf++	= (unsigned char)msap_cache->cvlan;
			pkt_len += 4;
		}		
	}
	#endif

        #if 0
	if (lldp_port->arp_nd_learning_vlan)
	{
		dot1q_tpid = 0x8100;
		*buf++	= (unsigned char)(dot1q_tpid >> 8);
		*buf++	= (unsigned char)dot1q_tpid;
		*buf++	= (unsigned char)(lldp_port->arp_nd_learning_vlan >> 8);
		*buf++	= (unsigned char)lldp_port->arp_nd_learning_vlan;
		pkt_len += 4;
	}
        #endif

	/*eth type*/
	*buf++ = (unsigned char)(LLDP_ETH_TYPE >> 8);
	*buf++ = (unsigned char)LLDP_ETH_TYPE;
	pkt_len += 2;

	
/*add basic Mandatory TLV*/
	
	/*CHASSIS_ID_TLV, subtype is MAC*/
	LLDP_ENCODE_TYPE_LENGTH(CHASSIS_ID_TLV, MAC_LEN+1);
	*buf++ = CHASSIS_ID_MAC_ADDRESS;
	memcpy(buf, &lldp_global_msg.source_mac[0], MAC_LEN);
	buf += MAC_LEN; 
	pkt_len += MAC_LEN + 3;
	
	
	/*PORT_ID_TLV*/
	LLDP_ENCODE_TYPE_LENGTH(PORT_ID_TLV, strlen(lldp_port->if_name) + 1);
	*buf++ = PORT_ID_INTERFACE_NAME;
	memcpy(buf, &lldp_port->if_name[0], strlen(lldp_port->if_name));
	buf += strlen(lldp_port->if_name);
	pkt_len += strlen(lldp_port->if_name) + 3;
	
	
	/*TIME_TO_LIVE_TLV*/
	LLDP_ENCODE_TYPE_LENGTH (TIME_TO_LIVE_TLV, TTL_LEN);
	*buf++ = 0x00;
	*buf++ = 0x00;
	pkt_len += TTL_LEN + 2;
	
	/*END_OF_LLDPDU_TLV*/
	LLDP_ENCODE_TYPE_LENGTH(END_OF_LLDPDU_TLV, 0);
	pkt_len += 2;
	
    if(pkt_len < 64)
	{
        lldp_port->tx.sendsize = 64;
    }
	else
	{
		lldp_port->tx.sendsize = pkt_len;
    }

	/************send pkt**********************************************/
#if 0
	if(lldp_port->portEnabled == FALSE)
	{
		if(lldp_port->tx.frame != NULL)
		{
			memset(&lldp_port->tx.frame[0], 0, lldp_port->tx.sendsize);
		}
	
		return 0;
	}
	
	memset(&pktcontrol, 0, sizeof(union pkt_control));
		
	/*dmac*/
	pktcontrol.ethcb.dmac[0] = LLDP_DEST_MAC_0;
	pktcontrol.ethcb.dmac[1] = LLDP_DEST_MAC_1;
	pktcontrol.ethcb.dmac[2] = LLDP_DEST_MAC_2;
	pktcontrol.ethcb.dmac[3] = LLDP_DEST_MAC_3;
	pktcontrol.ethcb.dmac[4] = LLDP_DEST_MAC_4;
	pktcontrol.ethcb.dmac[5] = LLDP_DEST_MAC_5;
		
	/*smac*/
	memcpy(pktcontrol.ethcb.smac, &lldp_global_msg.source_mac[0], 6);

	/*vlan msg*/
	if(msap_cache != NULL)
	{
		/*svlan*/
		if((msap_cache->svlan_tpid != 0x0000) && (msap_cache->svlan != 0x0000))
		{
			pktcontrol.ethcb.svlan_tpid = htons(msap_cache->svlan_tpid);
			pktcontrol.ethcb.svlan	= htons(msap_cache->svlan_tpid);
		}
		/*cvlan*/
		if((msap_cache->cvlan_tpid != 0x0000) && (msap_cache->cvlan != 0x0000))
		{
			pktcontrol.ethcb.cvlan_tpid = htons(msap_cache->cvlan_tpid);
			pktcontrol.ethcb.cvlan = htons(msap_cache->cvlan);
		}		
	}
			
	pktcontrol.ethcb.ethtype = htons(LLDP_ETH_TYPE);
	pktcontrol.ethcb.is_changed = 1;
	pktcontrol.ethcb.smac_valid = 1;
	pktcontrol.ethcb.ifindex = lldp_port->if_index;
	
#if 0
	
	printf("total :tx.frame:\n");
	int i;							
		for(i = 0; i < lldp_port->tx.sendsize;i++)
		{
			printf("%02x ", lldp_port->tx.frame[i]);
		}
		printf("\n");	
		printf("lldp_port->tx.sendsize = %d\n", lldp_port->tx.sendsize);
#endif

	pkt_send(PKT_TYPE_ETH, &pktcontrol, lldp_port->tx.frame, lldp_port->tx.sendsize);

	if(lldp_port->tx.frame != NULL)
	{
		memset(&lldp_port->tx.frame[0], 0, lldp_port->tx.sendsize);
	}

	lldp_port->tx.statistics.statsFramesOutTotal++; 
#endif	
/**********************************************************/

	return;
}




unsigned char lldp_tx_frame(struct lldp_port *lldp_port)
{
	union pkt_control pktcontrol;

	if(LLDP_FALSE == lldp_port->portEnabled)
	{
		if(lldp_port->tx.frame != NULL)
		{
			memset(&lldp_port->tx.frame[0], 0, lldp_port->tx.sendsize);
		}

		return 0;
	}


	pktcontrol.rawcb.out_ifindex = lldp_port->if_index;
	pktcontrol.rawcb.priority = 1;
	pkt_send(PKT_TYPE_RAW, &pktcontrol, lldp_port->tx.frame, lldp_port->tx.sendsize);


#if 0

		printf("total :tx.frame:\n");
int i;							
		for(i = 0; i < lldp_port->tx.sendsize;i++)
		{
			printf("%02x ", lldp_port->tx.frame[i]);
		}
		printf("\n");	
		printf("lldp_port->tx.sendsize = %d\n", lldp_port->tx.sendsize);	
						
#endif

    if(lldp_port->tx.frame != NULL)
	{
		memset(&lldp_port->tx.frame[0], 0, lldp_port->tx.sendsize);
	}

    lldp_port->tx.statistics.statsFramesOutTotal++;
	
    return 0;
}

void lldp_tx_change_state(struct lldp_port *lldp_port, unsigned char state) 
{
    switch(state)
	{
        case TX_LLDP_INITIALIZE: 
		{
        	if(lldp_port->portEnabled)
			{
            	/*ERROR*/
				zlog_err("%s: ERROR: TX SM change: port link change to TX_LLDP_INITIALIZE state!\n", __func__);
            } 
		}
			break;
        case TX_IDLE: 
		{
        	if(!(lldp_port->tx.state == TX_LLDP_INITIALIZE ||
            	lldp_port->tx.state == TX_INFO_FRAME))
            {
				/*ERROR*/
				zlog_err("%s: ERROR: TX SM change: no TX_LLDP_INITIALIZE || TX_INFO_FRAME state change to TX_IDLE!\n", __func__);
            }

		}
			break;
        case TX_SHUTDOWN_FRAME:
        case TX_INFO_FRAME: 
		{
			if(!((TX_IDLE == lldp_port->tx.state) || (TX_LLDP_INITIALIZE == lldp_port->tx.state)))
			{
				/*ERROR*/
				zlog_err("%s: ERROR: TX SM change: no tx_idle|| TX_LLDP_INITIALIZE state change to SHUTDOWN || TX_INFO_FRAME!\n", __func__);
			}
		}
			break;
        default:			
			/*ERROR*/
			zlog_err("%s: ERROR: LLDP tx state machine change: default!\n", __func__);
			break;
    };

    lldp_port->tx.state = state;
}

void lldp_tx_state_machine_run(struct lldp_port *lldp_port)
{
	if(NULL == lldp_port)
	{
		return;
	}

    switch(lldp_port->tx.state)
	{
        case TX_LLDP_INITIALIZE: 
		{
			if((LLDP_TRUE == lldp_port->portEnabled) && (enabledRxTx == lldp_port->adminStatus))
			{    
				lldp_tx_change_state(lldp_port, TX_IDLE);
				break;
			}
			
			/* send a shutdown frame...*/
			if(disabled == lldp_port->adminStatus)
			{
				lldp_tx_change_state(lldp_port, TX_SHUTDOWN_FRAME);
				break;
			}

			/*tx timer is 0*/
			if(0 == lldp_port->tx.txTTR)
			{
				lldp_port->tx.txTTR = lldp_global_msg.msgTxInterval;
			}
		}
			break;
        case TX_IDLE: 
		{
			/* It's time to send a shutdown frame...*/
			if(disabled == lldp_port->adminStatus)
			{
				lldp_tx_change_state(lldp_port, TX_SHUTDOWN_FRAME);
				break;
			}
			
			if(LLDP_FALSE == lldp_port->portEnabled)
			{
				lldp_tx_change_state(lldp_port, TX_LLDP_INITIALIZE);
				break;
			}

			/* It's time to send a frame...*/
			if((0 == lldp_port->tx.txTTR) || (LLDP_TRUE == lldp_port->tx.somethingChangedLocal))
			{			
				lldp_port->tx.txTTR = lldp_global_msg.msgTxInterval;				
				lldp_port->tx.somethingChangedLocal = LLDP_FALSE;
				lldp_port->tx.txTTL = min(65535, (lldp_global_msg.msgTxInterval * lldp_global_msg.msgTxHold));				
				lldp_tx_change_state(lldp_port, TX_INFO_FRAME);
			}
		}
			break;
        case TX_SHUTDOWN_FRAME: 
		{
		}
			break;
        case TX_INFO_FRAME: 
		{   
			
		}
			break;
        default:
			/*ERROR*/
			zlog_err("%s: ERROR: LLDP tx state machine run: default!\n", __func__);
			break;
   	}

	return;
}

void lldp_tx_state_machine_action(struct lldp_port *lldp_port)
{
	if (NULL == lldp_port)
	{
		return;
	}

    lldp_tx_state_machine_run(lldp_port);

    switch(lldp_port->tx.state)
    {
        case TX_LLDP_INITIALIZE:
		{
		}
			break;
        case TX_IDLE:      
		{
		}
			break;
        case TX_SHUTDOWN_FRAME:
		{
			lldp_send_shutdown_frame(lldp_port);
		}
			break;
        case TX_INFO_FRAME:
		{
			lldp_send_frame(lldp_port);
			
        	lldp_tx_change_state(lldp_port, TX_IDLE);
		}
			break;
        default:
			zlog_err("%s: ERROR: LLDP TX STATE:default!\n", __func__);
            /*ERROR*/
			break;
   	}


	return;
}

void lldp_tx_timers(struct lldp_port *lldp_port)
{
    if(lldp_port->tx.txTTR > 1)
    {
		lldp_port->tx.txTTR--;
	}
	else
	{	
		lldp_port->tx.txTTR--;
		lldp_tx_state_machine_action(lldp_port);
	}

	return;
}

void lldp_send_shutdown_frame(struct lldp_port *lldp_port)
{
	struct lldp_msap		*msap_msg = NULL;

	msap_msg = lldp_port->msap_cache;

	if(lldp_port->msap_num)
	{
		while(msap_msg != NULL)
		{
			lldp_encap_shutdown_frame(lldp_port, msap_msg);

			lldp_tx_frame(lldp_port);

			msap_msg = msap_msg->next;
		}
	}
	else
	{
		lldp_encap_shutdown_frame(lldp_port, msap_msg);		
		lldp_tx_frame(lldp_port);
	}

	lldp_clean_port(lldp_port);

	return;
}

void lldp_send_frame(struct lldp_port *lldp_port)
{
	struct lldp_msap		*msap_msg = NULL;
	msap_msg = lldp_port->msap_cache;

	if(lldp_port->msap_num)
	{
		while(msap_msg != NULL)
		{
			lldp_encap_frame(lldp_port, msap_msg);
			lldp_tx_frame(lldp_port);

			msap_msg = msap_msg->next;
		}
	}
	else
	{
		lldp_encap_frame(lldp_port, msap_msg);
		lldp_tx_frame(lldp_port);
	}

	return;
}


void lldp_interface_admin_status_enable(struct l2if *pif)
{
	struct lldp_port	*port = NULL;
	unsigned char		status;

	if(NULL == pif)
	{
		return;
	}

	/*judge the port mode*/
	if((IFNET_MODE_INVALID == pif->mode) || (IFNET_MODE_PHYSICAL == pif->mode))
	{
		return;	
	}

	//only support physical port
	if (!IFM_TYPE_IS_PHYSICAL(pif->ifindex))
	{
		return;
	}

	port = pif->lldp_port_info;
	if(port != NULL)
	{
		return;
	}

	if(  lldp_global_msg.source_mac[0] == 0x00
	  && lldp_global_msg.source_mac[1] == 0x00
	  && lldp_global_msg.source_mac[2] == 0x00
	  && lldp_global_msg.source_mac[3] == 0x00
	  && lldp_global_msg.source_mac[4] == 0x00
	  && lldp_global_msg.source_mac[5] == 0x00)
	{
		return;
	}
	
	pif->lldp_port_info = (struct lldp_port*)XMALLOC(MTYPE_LLDP, sizeof(struct lldp_port));
	if(NULL == pif->lldp_port_info)
	{		
		return;
	}

	memset(pif->lldp_port_info, 0, sizeof(struct lldp_port));
			
	/*get interface info*/
	 ifm_get_link(pif->ifindex, MODULE_ID_L2, &status);  
	if(status)
	{
		if(IFNET_LINKUP == status)
		{
			pif->lldp_port_info->portEnabled = LLDP_TRUE;
		}
		else
		{
			pif->lldp_port_info->portEnabled = LLDP_FALSE;				
		}			
	}
	else
	{		
		pif->lldp_port_info->portEnabled = LLDP_FALSE;
	}
		
	pif->lldp_port_info->adminStatus = LLDP_TRUE;
	
	ifm_get_name_by_ifindex(pif->ifindex ,pif->lldp_port_info->if_name);
	pif->lldp_port_info->if_index = pif->ifindex;

	memcpy(pif->lldp_port_info->if_mac, pif->mac, 6);
	
	/*get sys Capabilities*/
	pif->lldp_port_info->sys_cap = 0x0014; /*bit3:MAC Bridge &bit5:Router*/
	pif->lldp_port_info->sys_cap_enable = 0x0014;
	
	pif->lldp_port_info->rxChanges = LLDP_FALSE;
	pif->lldp_port_info->arp_nd_learning = 0;
		
	/*enable all tlv*/
	pif->lldp_port_info->tlv_cfg.basic_tlv_tx_enable = (PORT_DESCRIPTION_TLV_TX_ENABLE
											| SYSTEM_NAME_TLV_TX_ENABLE
											| SYSTEM_DESCRIPTION_TLV_TX_ENABLE
											| SYSTEM_CAPABILITIES_TLV_TX_ENABLE
											| MANAGEMENT_ADDRESS_TLV_TX_ENABLE);
	
	pif->lldp_port_info->max_frame_size = 9600;

	lldp_init_port_rx(pif->lldp_port_info);
	lldp_init_port_tx(pif->lldp_port_info);
		
	pif->lldp_port_info->msap_num = 0;
	pif->lldp_port_info->msap_cache = NULL;

	lldp_tx_state_machine_action(pif->lldp_port_info);
	
	if(TX_IDLE == pif->lldp_port_info->tx.state)
	{
		/*send frame*/
		lldp_send_frame(pif->lldp_port_info);
	}
	
	lldp_rx_state_machine_action(pif->lldp_port_info);

	return;		
}


void lldp_clean_port(struct lldp_port *clean_port)
{
	struct l2if			*pif = NULL;

	if (NULL == clean_port)
	{
		return;
	}
	
	pif = l2if_lookup(clean_port->if_index);
	if(pif == NULL)
	{
		zlog_err("%s: ERROR: LLDP clean port no find port struct pif!\n", __func__);
		return;
	}				

	lldp_destroy_port(clean_port);

	if(pif->lldp_port_info != NULL)
	{
		pif->lldp_port_info = NULL;
	}

	return;
}


void lldp_destroy_port(struct lldp_port *lldp_port)
{
	struct lldp_msap *msap_cache = NULL;

    msap_cache = lldp_port->msap_cache;

	while(msap_cache != NULL)
	{
		/*delete arp info*/
		if (CHECK_FLAG(lldp_port->arp_nd_learning, LLDP_LEARNING_ARP))
		{		
			if (LLDP_TRUE == msap_cache->arp_notice_flag)
			{
				lldp_arp_info_notice(msap_cache, lldp_port->arp_nd_learning_vlan, lldp_port->if_index, LLDP_FALSE);
				msap_cache->arp_notice_flag = LLDP_FALSE;
			}
		}
		
		msap_cache = msap_cache->next;
		
		memset(lldp_port->msap_cache->msap_id, 0, MSAP_NAMSIZ);

		lldp_port->msap_cache->next = NULL;
		XFREE(MTYPE_LLDP_MSAP, lldp_port->msap_cache);

		lldp_port->msap_cache = msap_cache;
	}

	if(lldp_port->rx.frame != NULL)
	{		
		XFREE(MTYPE_LLDP_RX, lldp_port->rx.frame);
		lldp_port->rx.frame = NULL;
	}

	if(lldp_port->tx.frame != NULL)
	{		
		XFREE(MTYPE_LLDP_TX, lldp_port->tx.frame);
		lldp_port->tx.frame = NULL;
	}
	
    lldp_port->msap_cache = NULL;

	XFREE(MTYPE_LLDP, lldp_port);

	return;
}



