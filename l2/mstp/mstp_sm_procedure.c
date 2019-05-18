/*
*  Copyright (C) 2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_sm_procedure.c	
*
*  date: 2017.3
*
*  modify:2017.10~2017.12
*
*  modify:	2018.3.13 modified by liufuying to make mstp module code beautiful
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "command.h"
#include "lib/log.h"
#include "ifm_common.h"
#include "lib/msg_ipc.h"
#include "lib/errcode.h"


#include "l2_if.h"
#include "mstp_init.h"
#include "mstp_sm.h"
#include "mstp_sm_procedure.h"
#include "mstp_base_procedure.h"

extern struct mstp_bridge	*mstp_global_bridge;
extern struct mstp_vlan		bridge_cist_vlan_map;
extern struct mstp_vlan 	global_ins_vlan_map[MSTP_INSTANCE_MAX];


/*clearAllRcvdMsgs()
function: Clears rcvdMsg for the CIST and all MSTIs, for all Ports.*/
void mstp_clear_all_recv_msg(struct mstp_port* port)
{
	unsigned int	instance_id = 0;

	struct mstp_msti_port* msti_port = NULL;

	if (NULL == port)
	{
		return;
	}

	/*cist port*/
    port->cist_port.common_info.rcvdMsg = MSTP_FALSE;

	/*msti ports*/
    for(instance_id = 0; instance_id < MSTP_INSTANCE_MAX; instance_id++)
    {
		msti_port = port->msti_port[instance_id];
        if(msti_port != NULL)
        {
            msti_port->common_info.rcvdMsg = MSTP_FALSE;
        }
    }
}

/*The following procedures perform the functions specified in 17.17 of IEEE Std 802.1D, 1998 Edition for the
CIST or any given MSTI instance.*/
void mstp_update_bpdu_version(struct mstp_port* port)
{
	if (NULL == port)
	{
		return;
	}

    if((MSTP_BPDUTYPE_TCN == port->cist_port.bpdu_cfg.message_type)
		|| (MSTP_BPDUTYPE_CONFIG == port->cist_port.bpdu_cfg.message_type))
    {
		port->port_variables.rcvdSTP = MSTP_TRUE;
    }

    if((MSTP_BPDUTYPE_RSTP == port->cist_port.bpdu_cfg.message_type)
		|| (MSTP_BPDUTYPE_MSTP == port->cist_port.bpdu_cfg.message_type))
    {
        port->port_variables.rcvdRSTP = MSTP_TRUE;
    }
}

/*fromSameRegion()
Returns TRUE if rcvdRSTP is TRUE, and the received 
BPDU conveys anMST Configuration Identifier that
matches that held for the Bridge. Returns FALSE otherwise.*/
int mstp_recv_from_same_region(struct mstp_port* port)
{
	struct mst_config_id	*br_mst_cfg_id = NULL;
	struct mst_config_id	*recv_mst_cfg_id = NULL;


	if ((NULL == mstp_global_bridge) ||
		(NULL == port) || 
		(NULL == port->m_br))
	{
		return MSTP_FALSE;
	}

	br_mst_cfg_id	= &port->m_br->mst_cfg_id;
	recv_mst_cfg_id	= &port->mst_cfg_id;

	/*judge force protocol version
	** ipran is always mstp protocol now*/
	if(mstp_global_bridge->common_br.force_pt_version != PROTO_VERID_MSTP)
	{
	    return MSTP_FALSE;
	}

	/*if recv the pkt is not mstp pkt 
	**think the pkt is come from other mst*/
	if(port->cist_port.bpdu_cfg.message_type != MSTP_BPDUTYPE_MSTP)
	{
		return MSTP_FALSE;
	}

	/*compare (MST) Configuration Name*/
	if(strncmp((const char *)br_mst_cfg_id->cfg_name, (const char *)recv_mst_cfg_id->cfg_name, NAME_LEN))
    {
        return MSTP_FALSE;
    }

	/*compare The Revision Level*/
    if(br_mst_cfg_id->revison_level != recv_mst_cfg_id->revison_level)
    {
        return MSTP_FALSE;
    }

	
	/*compare The Configuration Digest*/
    if(memcmp(br_mst_cfg_id->cfg_digest, recv_mst_cfg_id->cfg_digest, DIGEST_LEN))
    {
        return MSTP_FALSE;
	
    }
	

	return MSTP_TRUE;
}


/*setRcvdMsgs()
Sets rcvdMsg for the CIST, and makes the received CST or
CIST message available to the CIST Port Information state machines.*/
void mstp_set_recv_msg(struct mstp_port* port)
{
	unsigned int			instance_id = 0;
	struct mstp_cist_port	*cist_port = NULL;
	struct mstp_msti_port	*msti_port = NULL;

	if (NULL == port)
	{
		return;
	}

	cist_port = &port->cist_port;

	cist_port->common_info.rcvdMsg		= MSTP_TRUE;

#if 0	
	cist_port->common_info.loopPtState	= MSTP_LOOP_PTST_NO;

	for(instance_id = 0; instance_id < MSTP_INSTANCE_MAX; instance_id++)
	{
		msti_port = port->msti_port[instance_id];
		if (msti_port != NULL && msti_port->bpdu_cfg.new_msg == MSTP_TRUE)
		{
			msti_port->common_info.loopPtState = MSTP_FALSE;
		}
	}
#endif

	/*if rcvd msg from internal mst,  deal it*/
	if(MSTP_TRUE == port->port_variables.rcvdInternal)
	{
		for(instance_id = 0; instance_id < MSTP_INSTANCE_MAX; instance_id++)
		{
			msti_port = port->msti_port[instance_id];
			if((msti_port != NULL) && (MSTP_TRUE == msti_port->bpdu_cfg.new_msg))
			{
				msti_port->common_info.rcvdMsg = MSTP_TRUE;
				msti_port->bpdu_cfg.new_msg = MSTP_FALSE;
			}
		}
	}
}


/*#################RX###############################*/

void mstp_pkt_recv(struct l2if	*pif, struct pkt_buffer *pkt)
{
	unsigned int				bpdu_len = 0;
	unsigned int				mac_header_len = 0;
	unsigned int				eth_header_len = 0;
	unsigned int				bpdu_header_len = 0;
	unsigned int				mode_change = MSTP_FALSE;

	/*recv stp bpdu*/
	struct mac_header			*mac_header 	= NULL;
	struct eth_header			*eth_header 	= NULL;
	struct stp_bpdu_header		*bpdu_header 	= NULL; /**/
	struct mstp_bpdu			*bpdu_info 		= NULL; /*flags -to- end*/
	struct mstp_port			*mstp_port 		= NULL;

	if ((NULL == pif) || (NULL == pkt) || (NULL == mstp_global_bridge))
	{
		return;
	}

	mstp_port = mstp_find_mstp_port(pif);
	if(NULL == mstp_port)
	{
		MSTP_LOG_DBG("%s: ERROR: mstp recv bpdu find mstp_port error !!!\n", __func__);
		return;
	}

	/*filter port, no need to handle this message*/
	if (MSTP_TRUE == mstp_port->mstp_filter_port)
	{
		return;
	}
	
	/*add dmac & smac*/	
	memcpy(&mstp_port->rx_frame[0], &pkt->cb.ethcb.dmac[0], MAC_LEN);
	memcpy(&mstp_port->rx_frame[6], &pkt->cb.ethcb.smac[0], MAC_LEN);


#if 0
	/*add svlan msg*/
	/*if svlan_tpid or svlan which one is 0 , we think the value is invalid*/
	if((pkt->cb.ethcb.svlan_tpid != 0)
		&& (pkt->cb.ethcb.svlan != 0))
	{
		mstp_port->rx_frame[12] = (unsigned char)(pkt->cb.ethcb.svlan_tpid >> 8);
		mstp_port->rx_frame[13] = (unsigned char)pkt->cb.ethcb.svlan_tpid;
		mstp_port->rx_frame[14] = (unsigned char)(pkt->cb.ethcb.svlan >> 8);
		mstp_port->rx_frame[15] = (unsigned char)pkt->cb.ethcb.svlan;
	}
	else
	{
		mstp_port->rx_frame[12] = 0x00;
		mstp_port->rx_frame[13] = 0x00;
		mstp_port->rx_frame[14] = 0x00;
		mstp_port->rx_frame[15] = 0x00;
	}
	/*add cvlan msg*/
	/*if cvlan_tpid or cvlan which one is 0 , we think the value is invalid*/
	if((pkt->cb.ethcb.cvlan_tpid != 0)
		&& (pkt->cb.ethcb.cvlan != 0))
	{
		mstp_port->rx_frame[16] = (unsigned char)(pkt->cb.ethcb.cvlan_tpid >> 8);
		mstp_port->rx_frame[17] = (unsigned char)pkt->cb.ethcb.cvlan_tpid;
		mstp_port->rx_frame[18] = (unsigned char)(pkt->cb.ethcb.cvlan >> 8);
		mstp_port->rx_frame[19] = (unsigned char)pkt->cb.ethcb.cvlan;
	}
	else
	{
		mstp_port->rx_frame[16] = 0;
		mstp_port->rx_frame[17] = 0;
		mstp_port->rx_frame[18] = 0;
		mstp_port->rx_frame[19] = 0;
	}
#endif
	mstp_port->rx_frame[12] = (unsigned char)(pkt->data_len >> 8);
	mstp_port->rx_frame[13] = (unsigned char)(pkt->data_len);

	memcpy(&mstp_port->rx_frame[14], pkt->data, pkt->data_len);

	mac_header_len	= sizeof(struct mac_header);
	eth_header_len	= sizeof(struct eth_header);
	bpdu_header_len	= sizeof(struct stp_bpdu_header);

	/*mac header*/
	mac_header = (struct mac_header *)mstp_port->rx_frame;
	if((mac_header->dmac[0] != MSTP_DEST_MAC_0)
		|| (mac_header->dmac[1] != MSTP_DEST_MAC_1)
		|| (mac_header->dmac[2] != MSTP_DEST_MAC_2)
		|| (mac_header->dmac[3] != MSTP_DEST_MAC_3)
		|| (mac_header->dmac[4] != MSTP_DEST_MAC_4)
		|| (mac_header->dmac[5] != MSTP_DEST_MAC_5))
	{	
		MSTP_LOG_DBG("%s: ERROR: mstp recv error dest mac addr bpdu !!!\n", __func__);
		return;
	}

	/*eth header*/
	eth_header = (struct eth_header *)((unsigned char *)mstp_port->rx_frame + mac_header_len);
	if((eth_header->llc_dsap != BPDU_LLC_SAP)
		|| (eth_header->llc_ssap != BPDU_LLC_SAP)
		|| (eth_header->llc_control != BPDU_LLC_CONTRL))
	{
		MSTP_LOG_DBG("%s: ERROR: mstp recv error eth_header bpdu !!!\n", __func__);
		return;
	}

	/*bpdu header*/
	bpdu_header = (struct stp_bpdu_header *)((unsigned char *)mstp_port->rx_frame + mac_header_len + eth_header_len);
	if(bpdu_header->protocol_id != htons(MSTP_PROTO_ID))
	{
		MSTP_LOG_DBG("%s: ERROR: mstp recv error bpdu_header bpdu !!!\n", __func__);
		return;	
	}

/*bpdu protocol version judge*/

/*protocol version id
**stp cfg:0x00;  stp tcn:0x00;rstp:0x02; mstp:0x03*/

/*bpdu type
**stp:0x00; stp tcn:0x80; rstp&mstp:0x02;*/

	bpdu_info = (struct mstp_bpdu *)((unsigned char *)mstp_port->rx_frame + mac_header_len + eth_header_len + bpdu_header_len);
	if(PROTO_VERID_STP == bpdu_header->protocol_version_id)
	{
		/*port mode change	MODE_CHANGE*/
		/*1.		MSTP mode ---> STP mode
		**		RSTP mode ---> STP mode
		**/
		if(((PROTO_VERID_MSTP == mstp_global_bridge->stp_mode)
			|| (PROTO_VERID_RSTP == mstp_global_bridge->stp_mode))
			&& (mstp_port->stp_mode != PROTO_VERID_STP))
		{
			/*need to think care*/
			mstp_port->stp_mode = PROTO_VERID_STP;
			mode_change = MSTP_TRUE;
		}
	
		if(BPDU_TYPE_STP == bpdu_header->bpdu_type)
		{		
			/**/
			mstp_port->bpdu_config_recv += 1;

			/*deal stp msg*/
			mstp_decode_stp_bpdu(mstp_port, bpdu_info);
			if(PROTO_VERID_MSTP == mstp_global_bridge->stp_mode)
			{
				mstp_msti_port_decode_stp_bpdu(mstp_port, bpdu_info);
			}

			/*add sm deal*/
			mstp_rcv_sm_run(mstp_port);
		}
		else if(BPDU_TYPE_TCN == bpdu_header->bpdu_type)
		{		
			/**/
			mstp_port->bpdu_tcn_recv += 1;
		
			/*deal tcn msg*/
			mstp_decode_tcn_bpdu(mstp_port, bpdu_info);

			/*add sm deal*/
			mstp_rcv_sm_run(mstp_port);
		}
		else
		{
			MSTP_LOG_DBG("%s: ERROR: mstp recv bpdu (protocol_version_id == PROTO_VERID_STP), bpdu_type error !!!\n", __func__);
			return; 
		}
	}
	else if(PROTO_VERID_RSTP == bpdu_header->protocol_version_id)
	{
		if(BPDU_TYPE_RSTP == bpdu_header->bpdu_type)
		{
			/**/
			mstp_port->bpdu_rst_recv += 1;
			
			/*port mode change	MODE_CHANGE*/
			/*
			**	MSTP mode ---> RSTP mode
			**	STP mode ---> RSTP mode
			*/
			if(((PROTO_VERID_MSTP == mstp_global_bridge->stp_mode)
				|| (PROTO_VERID_RSTP == mstp_global_bridge->stp_mode))
				&& (mstp_port->stp_mode != PROTO_VERID_RSTP))
			{
				mstp_port->stp_mode = PROTO_VERID_RSTP;
				mode_change = MSTP_TRUE;
			}

			/*deal rstp msg*/
			mstp_decode_rstp_bpdu(mstp_port, bpdu_info);
			if(PROTO_VERID_MSTP == mstp_global_bridge->stp_mode)
			{
				mstp_msti_port_decode_stp_bpdu(mstp_port, bpdu_info);
			}

			/*add sm deal*/
			mstp_rcv_sm_run(mstp_port);
		}
		else
		{
			MSTP_LOG_DBG("%s: ERROR: mstp recv bpdu (protocol_version_id == PROTO_VERID_RSTP), bpdu_type error !!!\n", __func__);
			return; 
		}
	}
	else if(PROTO_VERID_MSTP == bpdu_header->protocol_version_id)
	{
		if(BPDU_TYPE_MSTP == bpdu_header->bpdu_type)
		{
			bpdu_len = pkt->data_len - 3/*0x42,0x42,0x03*/-4/*crc*/;

			/**/
			mstp_port->bpdu_mst_recv += 1;


			/*port mode change	MODE_CHANGE*/
			/*
			**  STP mode ---> MSTP mode
			**  RDTP mode ---> MSTP mode
			**  port mode back to MSTP mode
			*/
			if((PROTO_VERID_MSTP == mstp_global_bridge->stp_mode)
				&& (mstp_port->stp_mode != PROTO_VERID_MSTP))
			{
				mstp_port->stp_mode = PROTO_VERID_MSTP;				
				mode_change = MSTP_TRUE;
			}


			/*deal mstp msg*/
			mstp_decode_mstp_bpdu(mstp_port, bpdu_info, bpdu_len);			

			/*add sm deal*/
			mstp_rcv_sm_run(mstp_port);
		}
		else
		{
			MSTP_LOG_DBG("%s: ERROR: mstp recv bpdu (protocol_version_id == PROTO_VERID_MSTP), bpdu_type error !!!\n", __func__);
			return; 
		}
	}

	if(MSTP_TRUE == mode_change)
	{
		mstp_reinit();
	}

	mstp_rcv_sm_run(mstp_port);

	return;
}



/*deal mstp msg*/
void mstp_msti_port_decode_stp_bpdu(struct mstp_port *port, struct mstp_bpdu *bpdu)
{
	unsigned int				instance_id 	= 0;

	struct mstp_msti_port		*msti_port		= NULL;

	if ((NULL == port) || (NULL == bpdu))
	{
		return;
	}

	/* Its value is the number of octets taken by the parameters that follow in the BPDU. */
	/*deal the msti msg*/
	for(instance_id = 0; instance_id < MSTP_INSTANCE_MAX; instance_id++)
	{
		msti_port = port->msti_port[instance_id];
		if(NULL == msti_port)
		{
			continue;
		}
		
		memset(&msti_port->bpdu_cfg, 0, sizeof(struct mstp_msti_bpdu_cfg));

		if(PROTO_VERID_STP == port->stp_mode)
		{	
			/* deal flag*/
			/*bit 0*/
			/*check tc bit*/
			if(bpdu->flags.flag & BPDU_FLAG_TC)
			{
				msti_port->bpdu_cfg.topology_change = MSTP_TRUE;
				port->bpdu_tc_recv += 1;
			}
				
			/*check TCACK bit*/ 
			if(bpdu->flags.flag & BPDU_FLAG_TCACK)
			{
				msti_port->bpdu_cfg.topology_change_ack = MSTP_TRUE;
			}	
			msti_port->bpdu_cfg.role = mstp_get_role_by_flag((unsigned char)bpdu->flags.flag);

			msti_port->bpdu_cfg.new_msg = MSTP_TRUE;
			
			msti_port->common_info.reselect = MSTP_TRUE;
			/*it means:(pri[0])[bit7-4]is bridge_priority, (pri[0])[3-0]and (pri[1])[bit7-0] is MSTID*/ 	
			
			msti_port->msg_priority.rg_root_id.pri[0]		= bpdu->tx_bridge_id.pri[0] & 0xF0;
			msti_port->msg_priority.rg_root_id.pri[1]		= bpdu->tx_bridge_id.pri[1] & 0x00;
			memcpy(msti_port->msg_priority.rg_root_id.mac_addr, bpdu->tx_bridge_id.mac_addr, MAC_LEN);
			
			msti_port->msg_priority.internal_root_path_cost = ntohl(bpdu->cist_in_root_path_cost);
			msti_port->msg_priority.designated_bridge_id.pri[0] 	= bpdu->cist_bid.pri[0] & 0xF0;
			msti_port->msg_priority.designated_bridge_id.pri[1] 	= bpdu->cist_bid.pri[1] & 0x00;
			memcpy(msti_port->msg_priority.designated_bridge_id.mac_addr, bpdu->cist_bid.mac_addr, MAC_LEN);
			
			/*designated_port_id need to modify*/	//FIXME 
			msti_port->msg_priority.designated_port_id		= ntohs(bpdu->port_id);
			
			msti_port->msg_priority.recv_port_id = mstp_port_map(port->ifindex, port->port_pri);
			
			msti_port->msg_times.remaining_hops 		  = bpdu->cist_remain_hops;
		}
		else if(PROTO_VERID_RSTP == port->stp_mode)
		{
			/* deal flag*/
			
			/*bit 0*/
			if(bpdu->flags.flag & BPDU_FLAG_TC)
			{
				msti_port->bpdu_cfg.topology_change = MSTP_TRUE;
				
				port->bpdu_tc_recv += 1;
			}
					
			/*bit 1*/
			if(bpdu->flags.flag & BPDU_FLAG_PROPOSAL)
			{
				msti_port->bpdu_cfg.proposal = MSTP_TRUE;
			}
			
			/*flag:role bit[2-3]*/
			msti_port->bpdu_cfg.role = mstp_get_role_by_flag((unsigned char)bpdu->flags.flag);
			
			/*bit 4*/
			if(bpdu->flags.flag & BPDU_FLAG_LEARNING)
			{
				msti_port->bpdu_cfg.learning = MSTP_TRUE;
			}
					
			/*bit 5*/
			if(bpdu->flags.flag & BPDU_FLAG_FORWARDING)
			{
				msti_port->bpdu_cfg.forwarding = MSTP_TRUE;
			}
			
			/*bit 6*/
			if(bpdu->flags.flag & BPDU_FLAG_AGREEMENT)
			{
				msti_port->bpdu_cfg.agreement = MSTP_TRUE;
			}
					
			/*master bit flag*/			
			/*bit 7*/
			if(bpdu->flags.flag & BPDU_FLAG_TCACK)
			{
				msti_port->bpdu_cfg.topology_change_ack = MSTP_TRUE;
			}

			/*need to modify*/
#if 0
			/*set mastered value , 802.1Q-2005, page 167, 13.24.6
			802.1Q-2011 	13.25.23 master*/
			if(msti_port->bpdu_cfg.role == BPDU_ROLE_MASTER)
			{
				msti_port->mastered = MSTP_TRUE;
			}
#endif			
			msti_port->bpdu_cfg.new_msg = MSTP_TRUE;
			
			msti_port->common_info.reselect = MSTP_TRUE;
			
			/*it means:(pri[0])[bit7-4]is bridge_priority, (pri[0])[3-0]and (pri[1])[bit7-0] is MSTID*/ 	

			msti_port->msg_priority.rg_root_id.pri[0]		= bpdu->tx_bridge_id.pri[0] & 0xF0;
			msti_port->msg_priority.rg_root_id.pri[1]		= bpdu->tx_bridge_id.pri[1] & 0x00;
			memcpy(msti_port->msg_priority.rg_root_id.mac_addr, bpdu->tx_bridge_id.mac_addr, MAC_LEN);
			
			msti_port->msg_priority.internal_root_path_cost = ntohl(bpdu->cist_in_root_path_cost);
			msti_port->msg_priority.designated_bridge_id.pri[0] 	= bpdu->cist_bid.pri[0] & 0xF0;
			msti_port->msg_priority.designated_bridge_id.pri[1] 	= bpdu->cist_bid.pri[1] & 0x00;
			memcpy(msti_port->msg_priority.designated_bridge_id.mac_addr, bpdu->cist_bid.mac_addr, MAC_LEN);
			
			/*designated_port_id need to modify*/
			msti_port->msg_priority.designated_port_id		= ntohs(bpdu->port_id);
			
			msti_port->msg_priority.recv_port_id = mstp_port_map(port->ifindex, port->port_pri);
			
			msti_port->msg_times.remaining_hops 		  = bpdu->cist_remain_hops;

		}
	}
}



struct mstp_port *mstp_find_mstp_port(struct l2if *pif)
{
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s: ERROR: mstp_find_mstp_port pif == NULL, error !!!\n", __func__);
		return NULL;
	}

	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s: ERROR: mstp_port_info == NULL, maybe create mstp port error!!!\n", __func__);
		return NULL;
	}

	return pif->mstp_port_info;
}


void mstp_decode_stp_bpdu(struct mstp_port *mstp_port, struct mstp_bpdu *bpdu)
{
	struct mstp_cist_port*	cist_port = NULL;

	if ((NULL == mstp_port) || (NULL == bpdu))
	{
		return;
	}

	cist_port = &mstp_port->cist_port;

/*clear bpdu config*/
	memset(&cist_port->bpdu_cfg, 0, sizeof(struct mstp_cist_bpdu_cfg));
	cist_port->bpdu_cfg.message_type = MSTP_BPDUTYPE_CONFIG;

/*check tc bit*/
	if(bpdu->flags.flag & BPDU_FLAG_TC)
	{
		cist_port->bpdu_cfg.topology_change = MSTP_TRUE;
		
		mstp_port->bpdu_tc_recv += 1;
	}
	
/*check TCACK bit*/	
	if(bpdu->flags.flag & BPDU_FLAG_TCACK)
	{
		cist_port->bpdu_cfg.topology_change_ack = MSTP_TRUE;
	}

/*check recvd msg port role*/
	cist_port->bpdu_cfg.role = BPDU_ROLE_DESIGNATED;	//FIXME

	cist_port->msg_priority.root_id					= bpdu->root_id;
	cist_port->msg_priority.external_root_path_cost	= ntohl(bpdu->root_path_cost);
	cist_port->msg_priority.regional_root_id		= bpdu->tx_bridge_id;
	cist_port->msg_priority.internal_root_path_cost	= 0;
	cist_port->msg_priority.designated_bridge_id	= bpdu->tx_bridge_id;

	/*need to modify*/	//FIXME
	cist_port->msg_priority.designated_port_id		= ntohs(bpdu->port_id);
	cist_port->msg_priority.recv_port_id = mstp_port_map(cist_port->port_index, mstp_port->port_pri);

	/*need to confirm*/
	cist_port->msg_times.msg_age			= ntohs(bpdu->msg_age) >> 8;
	cist_port->msg_times.max_age			= ntohs(bpdu->max_age) >> 8;
	cist_port->msg_times.hello_time			= ntohs(bpdu->hello_time) >> 8;
	cist_port->msg_times.forward_delay		= ntohs(bpdu->forward_delay) >> 8;


	cist_port->msg_times.remaining_hops 	= bpdu->cist_remain_hops;

	return;
}

void mstp_decode_tcn_bpdu(struct mstp_port *port, struct mstp_bpdu *bpdu)
{
	struct mstp_cist_port		*cist_port = NULL;

	if ((NULL == port) || (NULL == bpdu))
	{
		return;
	}

	cist_port = &port->cist_port;

/*clear bpdu config*/
	memset(&cist_port->bpdu_cfg, 0, sizeof(struct mstp_cist_bpdu_cfg));
	cist_port->bpdu_cfg.message_type	= MSTP_BPDUTYPE_TCN;
	cist_port->bpdu_cfg.topology_change	= MSTP_TRUE;

	return;
}

void mstp_decode_rstp_bpdu(struct mstp_port *port, struct mstp_bpdu *bpdu)
{
	struct mstp_cist_port		*cist_port = NULL;

	if ((NULL == port) || (NULL == bpdu))
	{
		return;
	}

	cist_port = &port->cist_port;

	memset(&cist_port->bpdu_cfg, 0, sizeof(struct mstp_cist_bpdu_cfg));
	cist_port->bpdu_cfg.message_type = MSTP_BPDUTYPE_RSTP;

/*flags*/
/*bit 0*/
	if(bpdu->flags.flag & BPDU_FLAG_TC)
	{
		cist_port->bpdu_cfg.topology_change = MSTP_TRUE;
		
		port->bpdu_tc_recv += 1;
	}

/*bit 1*/
	if(bpdu->flags.flag & BPDU_FLAG_PROPOSAL)
	{
		cist_port->bpdu_cfg.proposal = MSTP_TRUE;
	}
/*bit [2-3]*/
	cist_port->bpdu_cfg.role = mstp_get_role_by_flag((unsigned char)bpdu->flags.flag);

/*bit 4*/
	if(bpdu->flags.flag & BPDU_FLAG_LEARNING)
	{
		cist_port->bpdu_cfg.learning = MSTP_TRUE;
	}

/*bit 5*/
	if(bpdu->flags.flag & BPDU_FLAG_FORWARDING)
	{
		cist_port->bpdu_cfg.forwarding = MSTP_TRUE;
	}

/*bit 6*/
	if(bpdu->flags.flag & BPDU_FLAG_AGREEMENT)
	{
		cist_port->bpdu_cfg.agreement = MSTP_TRUE;
	}

/*bit 7*/
	if(bpdu->flags.flag & BPDU_FLAG_TCACK)
	{
		cist_port->bpdu_cfg.topology_change_ack = MSTP_TRUE;
	}

/*bpdu priority*/
	cist_port->msg_priority.root_id					= bpdu->root_id;
	cist_port->msg_priority.external_root_path_cost	= ntohl(bpdu->root_path_cost);
	cist_port->msg_priority.regional_root_id		= bpdu->tx_bridge_id;
	cist_port->msg_priority.internal_root_path_cost	= 0;
	cist_port->msg_priority.designated_bridge_id	= bpdu->tx_bridge_id;
	cist_port->msg_priority.designated_port_id		= ntohs(bpdu->port_id);//FIXME

	cist_port->msg_priority.recv_port_id = mstp_port_map(cist_port->port_index, port->port_pri);

/*bpdu timer*/
/*need to confirm*/
	cist_port->msg_times.forward_delay		= ntohs(bpdu->forward_delay) >> 8;
	cist_port->msg_times.hello_time			= ntohs(bpdu->hello_time) >> 8;
	cist_port->msg_times.max_age			= ntohs(bpdu->max_age) >> 8;
	cist_port->msg_times.msg_age			= ntohs(bpdu->msg_age) >> 8;

	cist_port->msg_times.remaining_hops 	= bpdu->cist_remain_hops;

	return;
}

void mstp_decode_mstp_bpdu(struct mstp_port *port, struct mstp_bpdu *bpdu, unsigned int pkt_len)
{
	if ((NULL == port) || (NULL == bpdu))
	{
		return;
	}

	/* BPDU PROTECTION */
	if(/*(mstp_global_bridge->common_br.bpduProtc == MSTP_BPDU_PROTC_YES) && */
		(MSTP_TRUE == port->port_variables.operEdge))
	{		
		/*call sdk disable */
		/*edge port recv mstp bpdu*/
		port->bpdu_down = BPDUDOWN_SET;
		MSTP_LOG_DBG("%s: ERROR: mstp recv bpdu_type, bpdu_type is not mstp bpdu, error !!!\n", __func__);
		return;
	}


	mstp_decode_cist(port, bpdu);

	/*just deal the msg recv from the same mst*/
	if(MSTP_TRUE == mstp_recv_from_same_region(port))
	{
		if(0x00 == bpdu->mst_cfg_id.cfg_format_id_selector)
		{
			mstp_decode_msti_cfg_msg(port, bpdu, pkt_len);
		}
	}
}

void mstp_decode_cist(struct mstp_port *port, struct mstp_bpdu *bpdu)
{
	struct mstp_cist_port		*cist_port = NULL;

	if ((NULL == port) || (NULL == bpdu))
	{
		return;
	}

	cist_port = &port->cist_port;

	memset(&cist_port->bpdu_cfg, 0, sizeof(struct mstp_cist_bpdu_cfg));
	cist_port->bpdu_cfg.message_type = MSTP_BPDUTYPE_MSTP;

	/*bit 0*/
	if(bpdu->flags.flag & BPDU_FLAG_TC)
	{
		cist_port->bpdu_cfg.topology_change = MSTP_TRUE;
		
		port->bpdu_tc_recv += 1;
	}

	/*bit 1*/
	if(bpdu->flags.flag & BPDU_FLAG_PROPOSAL)
	{
		cist_port->bpdu_cfg.proposal = MSTP_TRUE;
	}

	/*bit [2-3]*/
	cist_port->bpdu_cfg.role = mstp_get_role_by_flag((unsigned char)bpdu->flags.flag);

	/*bit 4*/
	if(bpdu->flags.flag & BPDU_FLAG_LEARNING)
	{
		cist_port->bpdu_cfg.learning = MSTP_TRUE;
	}

	/*bit 5*/
	if(bpdu->flags.flag & BPDU_FLAG_FORWARDING)
	{
		cist_port->bpdu_cfg.forwarding = MSTP_TRUE;
	}
	
	/*bit 6*/
	if(bpdu->flags.flag & BPDU_FLAG_AGREEMENT)
	{
		cist_port->bpdu_cfg.agreement = MSTP_TRUE;
	}

	/*bit 7*/
	if(bpdu->flags.flag & BPDU_FLAG_TCACK)
	{
		cist_port->bpdu_cfg.topology_change_ack = MSTP_TRUE;
	}

/*bpdu->version1_length*/
	cist_port->msg_priority.root_id					= (bpdu->root_id);

	cist_port->msg_priority.external_root_path_cost	= ntohl(bpdu->root_path_cost);

	/*regional_root_id*/
	/*internal_root_path_cost*/
	/*designated_bridge_id*/
	cist_port->msg_priority.regional_root_id		= bpdu->tx_bridge_id;
	cist_port->msg_priority.internal_root_path_cost = ntohl(bpdu->cist_in_root_path_cost);
	cist_port->msg_priority.designated_bridge_id	= bpdu->cist_bid;
	cist_port->msg_priority.designated_port_id		= ntohs(bpdu->port_id);//FIXME

	cist_port->msg_priority.recv_port_id = mstp_port_map(cist_port->port_index, port->port_pri);

/*need to confirm*/

	cist_port->msg_times.msg_age			= ntohs(bpdu->msg_age) >> 8;
	cist_port->msg_times.max_age			= ntohs(bpdu->max_age) >> 8;
	cist_port->msg_times.hello_time 		= ntohs(bpdu->hello_time) >> 8;
	cist_port->msg_times.forward_delay		= ntohs(bpdu->forward_delay) >> 8;	
	cist_port->msg_times.remaining_hops 	= bpdu->cist_remain_hops;

/*mst config deal*/
	port->mst_cfg_id = bpdu->mst_cfg_id;
	port->mst_cfg_id.revison_level = ntohs(bpdu->mst_cfg_id.revison_level);

	return;
}

void mstp_decode_msti_cfg_msg(struct mstp_port *port, struct mstp_bpdu *bpdu, unsigned int pkt_len)
{
	unsigned int				i 				= 0;
	unsigned int				msti_msg_num	= 0;
	unsigned int				instance_id 	= 0;
//	unsigned short				ext_len 		= 0;
	unsigned short				port_id 		= 0;
	struct mstp_msti_port		*msti_port 		= NULL;
	struct msti_config_msg		*msti_cfg_msg 	= NULL;
	struct bridge_id			bridge_mac;

	if ((NULL == port) || (NULL == bpdu))
	{
		return;
	}

	memset((void *)&bridge_mac, 0, sizeof(struct bridge_id));

	if(pkt_len < MSTP_BPDU_LEN)
	{
		MSTP_LOG_DBG("%s: ERROR: mstp recv bpdu_type, bpdu_type is not mstp bpdu, error !!!\n", __func__);
		return; 
	}
	
/* Its value is the number of octets taken by the parameters that follow in the BPDU. */
//	ext_len = pkt_len - MSTP_BPDU_LEN + MSTP_EXT_BPDU_BASE_LEN;

#if 0
	if(ext_len != ntohs(bpdu->version_3_lenth))
	{
		MSTP_LOG_DBG("%s: ERROR: mstp recv bpdu pkt len, extern len is , error !!!\n", __func__);
		return;
	}
#endif

    msti_msg_num = (pkt_len - MSTP_BPDU_LEN + 4) / MSTP_BASE_MSTI_LENTH;
	if(0 == msti_msg_num)
	{
		MSTP_LOG_DBG("%s: ERROR: msti_msg_num == 0	!!!\n", __func__);
		return;
	}

	msti_cfg_msg	= &bpdu->msti_cfg_msg;
	bridge_mac		= port->cist_port.msg_priority.designated_bridge_id;
	port_id			= port->cist_port.msg_priority.designated_port_id & 0x0FFF;

/*deal the msti msg*/
	for(i = 0; i < msti_msg_num; i++)
	{
		instance_id = mstp_get_ins_by_brId(msti_cfg_msg[i].rg_root_id);

		msti_port = port->msti_port[instance_id-1];
		if(NULL == msti_port)
		{
			continue;
		}

		memset(&msti_port->bpdu_cfg, 0, sizeof(struct mstp_msti_bpdu_cfg));

		/* deal flag*/

		/*bit 0*/
		if(msti_cfg_msg[i].flag.flags & BPDU_FLAG_TC)
		{
			msti_port->bpdu_cfg.topology_change = MSTP_TRUE;
			
			port->bpdu_tc_recv += 1;
		}
		
		/*bit 1*/
		if(msti_cfg_msg[i].flag.flags & BPDU_FLAG_PROPOSAL)
		{
			msti_port->bpdu_cfg.proposal = MSTP_TRUE;
		}

		/*flag:role	bit[2-3]*/
		msti_port->bpdu_cfg.role = mstp_get_role_by_flag(msti_cfg_msg[i].flag.flags);		

		/*bit 4*/
		if(msti_cfg_msg[i].flag.flags & BPDU_FLAG_LEARNING)
		{
			msti_port->bpdu_cfg.learning = MSTP_TRUE;
		}
		
		/*bit 5*/
		if(msti_cfg_msg[i].flag.flags & BPDU_FLAG_FORWARDING)
		{
			msti_port->bpdu_cfg.forwarding = MSTP_TRUE;
		}

		/*bit 6*/
		if(msti_cfg_msg[i].flag.flags & BPDU_FLAG_AGREEMENT)
		{
			msti_port->bpdu_cfg.agreement = MSTP_TRUE;
		}
		
#if 1		/*master bit flag*/			
		/*bit 7*/
		if(msti_cfg_msg[i].flag.flags & BPDU_FLAG_TCACK)
		{
			msti_port->bpdu_cfg.topology_change_ack = MSTP_TRUE;
		}
#endif

/*set mastered value , 802.1Q-2005, page 167, 13.24.6
		802.1Q-2011		13.25.23 master*/
		if(BPDU_ROLE_MASTER == msti_port->bpdu_cfg.role)
		{
			msti_port->mastered = MSTP_TRUE;
		}

		msti_port->bpdu_cfg.new_msg = MSTP_TRUE;

		/*it means:(pri[0])[bit7-4]is bridge_priority, (pri[0])[3-0]and (pri[1])[bit7-0] is MSTID*/ 	
		msti_port->msg_priority.rg_root_id.pri[0]		= msti_cfg_msg[i].rg_root_id.pri[0] & 0xF0;
		msti_port->msg_priority.rg_root_id.pri[1]		= msti_cfg_msg[i].rg_root_id.pri[1] & 0x00;
		memcpy(msti_port->msg_priority.rg_root_id.mac_addr, msti_cfg_msg[i].rg_root_id.mac_addr, MAC_LEN);

		msti_port->msg_priority.internal_root_path_cost = ntohl(msti_cfg_msg[i].msti_in_rpc);

		/*need to confirm*/
		msti_port->msg_priority.designated_bridge_id.pri[0]		= (unsigned char)(msti_cfg_msg[i].msti_br_pri & 0xF0);
		msti_port->msg_priority.designated_bridge_id.pri[1]		= msti_cfg_msg[i].msti_br_pri & 0x00;
		memcpy(msti_port->msg_priority.designated_bridge_id.mac_addr, bridge_mac.mac_addr, MAC_LEN);

		/*designated_port_id need to modify*/	//FIXME	
		msti_port->msg_priority.designated_port_id		= ((unsigned short)msti_cfg_msg[i].msti_pt_pri << 8) | port_id;

		msti_port->msg_priority.recv_port_id = mstp_port_map(msti_port->port_index, port->port_pri);

		msti_port->msg_times.remaining_hops 		  = msti_cfg_msg[i].msti_remaining_hops;

	}

	return;
}

unsigned int mstp_get_ins_by_brId(struct bridge_id bridge_id)
{
	unsigned int		ins_id = 0;

	ins_id = (unsigned short)((bridge_id.pri[0] & 0x0F) << 8) | (unsigned short)bridge_id.pri[1];

	return (unsigned int)ins_id;
}

struct bridge_id mstp_get_brId_by_ins(unsigned short br_pri, struct bridge_id br_mac, unsigned int instance_id)
{
	unsigned short			temp = 0;
	struct bridge_id		bridge_id_temp;

	memset((void *)&bridge_id_temp, 0, sizeof(struct bridge_id));

	temp = ((br_pri & 0xf000) | (unsigned short)instance_id);
	bridge_id_temp.pri[0] = (unsigned char)(temp >> 8);
	bridge_id_temp.pri[1] = (unsigned char)temp;

	memcpy(bridge_id_temp.mac_addr, br_mac.mac_addr, MAC_LEN);

	return bridge_id_temp;
}

/*###############TX#################################*/


/* txConfig()
Transmits a Configuration BPDU. The first four components of the message priority 
vector (13.24.6) conveyed in the BPDU are set to the value of the CIST Root Identifier,
External Root Path Cost, Bridge Identifier, and Port Identifier components of
the cistPortPriority (13.24.8) for this Port. The topology change flag
is set if (tcWhile != 0) for the Port. The topology change acknowledgement flag is set to
the value of TcAckfor the Port. The remaining flags are set to zero. The value of the Message Age,
Max Age, Fwd Delay, and Hello Time parameters conveyed in the BPDU are set to the values 
held in cistPortTimes (13.24.9) for the Port.*/
void mstp_tx_config(struct mstp_port* port)
{	
	/**/

	unsigned short			bpdu_size	= 0;
	unsigned short			pkt_len		= 0;
	unsigned char			*pbuf		= NULL;
	unsigned short			lenth		= 0;
	unsigned char			bpdu_flag	= 0x00;
	unsigned short			ushort_value = 0;	
	unsigned int			uint_value	= 0;	

	if ((NULL == port) || (NULL == mstp_global_bridge))
	{
		return;
	}

	/*filter port, no need to tx bpdu*/
	if (MSTP_TRUE == port->mstp_filter_port)
	{
		return;
	}

	/*need clear tx_frame buff*/
	memset(port->tx_frame, 0x00, MSTP_MAX_FRAME_SIZE);
	pbuf = (unsigned char*)&port->tx_frame[0];

//	bpdu_size = (unsigned int)offsetof(struct mstp_bpdu, version_1_lenth);
	bpdu_size = MSTP_CONFIG_LEN;
	/*dmac*/
	*pbuf++ = MSTP_DEST_MAC_0;
	*pbuf++ = MSTP_DEST_MAC_1;
	*pbuf++ = MSTP_DEST_MAC_2;
	*pbuf++ = MSTP_DEST_MAC_3;
	*pbuf++ = MSTP_DEST_MAC_4;
	*pbuf++ = MSTP_DEST_MAC_5;
	pkt_len += 6;

	/*smac*/
	memcpy(pbuf, &mstp_global_bridge->bridge_mac.mac_addr[0], 6);
	pbuf += 6;
	pkt_len += 6;

	/*lenth*/
	lenth = bpdu_size + 3;	/*need to confirm*/
	*pbuf++ = (unsigned char)(lenth >> 8);
	*pbuf++ = (unsigned char)lenth;
	pkt_len += 2;

	/*LLC*/
	*pbuf++ = 0x42;/*DSAP*/
	*pbuf++ = 0x42;/*SSAP*/
	*pbuf++ = 0x03;/*control*/
	pkt_len += 3;

/*payload*/
	/*octet 1-2:protocol id*/
	*pbuf++ = 0;
	*pbuf++ = 0;
	pkt_len += 2;

	/*octet 3:protocol version id
	**stp cfg:0x00;  stp tcn:0x00;rstp:0x02; mstp:0x03*/
	*pbuf++ = PROTO_VERID_STP;
	pkt_len += 1;

	/*octet 4:bpdu type
	**stp:0x00; stp tcn:0x80; rstp&mstp:0x02;*/
	*pbuf++ = BPDU_TYPE_STP;
	pkt_len += 1;

	/*octet 5:flag*/
	if(port->cist_port.common_info.tcWhile != 0)//FIXME
	{
		bpdu_flag |= 0x01;

		port->bpdu_tc_send += 1;
	}
	if(port->port_variables.tcAck)//FIXME
	{
		bpdu_flag |= 0x80;
	}
	*pbuf++ = bpdu_flag;
	pkt_len += 1;


	/*octet 6-13:root id*/
	*pbuf++ = port->cist_port.designated_priority.root_id.pri[0];
	*pbuf++ = port->cist_port.designated_priority.root_id.pri[1];
	memcpy(pbuf, (unsigned char*)&port->cist_port.designated_priority.root_id.mac_addr[0], 6);	

	pbuf += 6;
	pkt_len += 8;

	/*octet 14-17:CIST External Root Path Cost*/
	uint_value = htonl(port->cist_port.designated_priority.external_root_path_cost);
	memcpy(pbuf, &uint_value, 4);
	pbuf += 4;
	pkt_len += 4;

	/*octet 18-25:CIST Regional Root Identifier*/
	*pbuf++ = port->cist_port.designated_priority.regional_root_id.pri[0];
	*pbuf++ = port->cist_port.designated_priority.regional_root_id.pri[1];
	memcpy(pbuf, (unsigned char*)&port->cist_port.designated_priority.regional_root_id.mac_addr[0], 6);	

	pbuf += 6;
	pkt_len += 8;

	/*octet 26-27:CIST Port Identifier */
	/*need to modify*/
	*pbuf++ =(unsigned char)(port->cist_port.designated_priority.designated_port_id >> 8);
	*pbuf++ =(unsigned char)(port->cist_port.designated_priority.designated_port_id);
	pkt_len += 2;

	/*octet 28-29:Message Age timer value */
	if(BIG_ENDIAN_ON)
	{
		ushort_value = htons(port->cist_port.designated_times.msg_age) << 8;
	}
	else
	{
		ushort_value = htons(port->cist_port.designated_times.msg_age);
	}

	*pbuf++ =(unsigned char)(ushort_value >> 8);
	*pbuf++ =(unsigned char)(ushort_value);	
	pkt_len += 2;

	/*octet 30-31:Max Age timer value*/
	if(BIG_ENDIAN_ON)
	{
		ushort_value = htons(port->cist_port.designated_times.max_age) << 8;
	}
	else
	{
		ushort_value = htons(port->cist_port.designated_times.max_age);
	}

	*pbuf++ =(unsigned char)(ushort_value >> 8);
	*pbuf++ =(unsigned char)(ushort_value);
	pkt_len += 2;

	/*octet 32-33:Hello Time timer value*/
	if(BIG_ENDIAN_ON)
	{
		ushort_value = htons(port->cist_port.designated_times.hello_time) << 8;
	}
	else
	{
		ushort_value = htons(port->cist_port.designated_times.hello_time);
	}

	*pbuf++ =(unsigned char)(ushort_value >> 8);
	*pbuf++ =(unsigned char)(ushort_value);	
	pkt_len += 2;

	/*octet 34-35: Forward Delay timer value.*/
	if(BIG_ENDIAN_ON)
	{
		ushort_value = htons(port->cist_port.designated_times.forward_delay) << 8;
	}
	else
	{
		ushort_value = htons(port->cist_port.designated_times.forward_delay);
	}

	*pbuf++ =(unsigned char)(ushort_value >> 8);
	*pbuf++ =(unsigned char)(ushort_value);	
	pkt_len += 2;
/*config pkt end*/


if(0)
{
	unsigned char	buf[1024];
	unsigned char	*pf = NULL;	/*pf: printf*/

	memcpy(buf, &port->tx_frame[0], pkt_len);
	pf = (unsigned char *)&buf[0];

	printf("send tx config msg: sendsize = %u:\n", pkt_len);

	/*dmac*/
	printf("dmac : %02x:%02x:%02x:%02x:%02x:%02x \n",
		pf[0], pf[1], pf[2], pf[3], pf[4], pf[5]);
	pf += MAC_LEN;

	/*smac*/
	printf("smac : %02x:%02x:%02x:%02x:%02x:%02x \n",
		pf[0], pf[1], pf[2], pf[3], pf[4], pf[5]);
	pf += MAC_LEN;
	

	/*lenth*/
	printf("msg lenth : %u \n",	((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

	/*LLC*/
	printf("LLC(DSAP[%02x] SSAP[%02x] CONTROL[%02x])\n",
					pf[0], pf[1], pf[2]);
	pf += 3;

/*payload*/
	/*octet 1-2:protocol id*/
	printf("protocol id: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

	/*octet 3:protocol version id
	**stp cfg:0x00;  stp tcn:0x00;rstp:0x02; mstp:0x03*/
	printf("protocol id: %02x\n", (pf[0]));
	pf += 1;

	/*octet 4:bpdu type
	**stp:0x00; stp tcn:0x80; rstp&mstp:0x02;*/
	printf("bpdu type: %02x\n", (pf[0]));
	pf += 1;

	/*octet 5:flag*/
	printf("flag: %02x\n", (pf[0]));
	pf += 1;

	/*octet 6-13:root id*/
	printf("root id priority: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

	/*root id mac*/
	printf("root id mac : %02x:%02x:%02x:%02x:%02x:%02x \n",
			pf[0], pf[1], pf[2], pf[3], pf[4], pf[5]);
	pf += MAC_LEN;


	/*octet 14-17:CIST External Root Path Cost*/
	printf("CIST External Root Path Cost : %u \n",
				 (unsigned int)(pf[0] << 24)
				|(unsigned int)(pf[1] << 16)
				|(unsigned int)(pf[2] << 8)
				|(unsigned int)(pf[3]));
	pf += 4;

	/*octet 18-25:CIST Regional Root Identifier*/
	printf("CIST Regional Root Identifier priority: %04x\n", 
				((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

	/*CIST Regional Root Identifier MAC*/
	printf("CIST Regional Root Identifier mac : %02x:%02x:%02x:%02x:%02x:%02x \n",
				pf[0], pf[1], pf[2], pf[3], pf[4], pf[5]);
	pf += MAC_LEN;

	/*octet 26-27:CIST Port Identifier */
	printf("CIST Port Identifier: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

	/*octet 28-29:Message Age timer value */
	printf("Message Age timer value: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

	/*octet 30-31:Max Age timer value*/
	printf("Message Age timer value: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

	/*octet 32-33:Hello Time timer value*/	
	printf("Hello Time timer value: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));	
	pf += 2;
	
	/*octet 34-35: Forward Delay timer value.*/
	printf("Forward Delay timer value: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

#if 0
int i;		
	for(i = 0; i < pkt_len; i++)
	{
		if(i % 15 == 0)
		{
			printf("\n");
		}

		printf("%02x ", port->tx_frame[i]);
	}
	printf("\n");
#endif
	
}

/*do something to send*/

	port->tx_len = pkt_len;
	/*encode over*/

	port->bpdu_config_send += 1;
	
	/*do something to send*/
	mstp_send_frame(port);

	return;
}

/*txTcn()
The following procedures perform the functions specified in 17.17 of IEEE Std 802.1D, 1998 Edition for the
CIST state machines.*/
void mstp_tx_tcn(struct mstp_port* port)
{
	/**/

	unsigned char			*pbuf	= NULL;
	unsigned short			pkt_len	= 0;
	unsigned short			lenth	= 0;

	if ((NULL == port) || (NULL == mstp_global_bridge))
	{
		return;
	}

	/*filter port, no need to tx bpdu*/
	if (MSTP_TRUE == port->mstp_filter_port)
	{
		return;
	}

	/*need clear tx_frame buff*/
	memset(port->tx_frame, 0x00, MSTP_MAX_FRAME_SIZE);
	pbuf = (unsigned char*)&port->tx_frame[0];

	/*dmac*/
	*pbuf++ = MSTP_DEST_MAC_0;
	*pbuf++ = MSTP_DEST_MAC_1;
	*pbuf++ = MSTP_DEST_MAC_2;
	*pbuf++ = MSTP_DEST_MAC_3;
	*pbuf++ = MSTP_DEST_MAC_4;
	*pbuf++ = MSTP_DEST_MAC_5;
	pkt_len += 6;

	/*smac*/
	memcpy(pbuf, &mstp_global_bridge->bridge_mac.mac_addr[0], 6);
	pbuf += 6;
	pkt_len += 6;

	/*lenth*/
	lenth = TCN_BPDU_LEN + 3;	/*need to confirm*/
	*pbuf++ = (unsigned char)(lenth >> 8);
	*pbuf++ = (unsigned char)lenth;
	pkt_len += 2;

	/*LLC*/
	*pbuf++ = 0x42;/*DSAP*/
	*pbuf++ = 0x42;/*SSAP*/
	*pbuf++ = 0x03;/*control*/
	pkt_len += 3;

/*payload*/
	/*octet 1-2:protocol id*/
	*pbuf++ = 0;
	*pbuf++ = 0;
	pkt_len += 2;

	/*octet 3:protocol version id
	**stp cfg:0x00;  stp tcn:0x00;rstp:0x02; mstp:0x03*/
	*pbuf++ = 0x00;
	pkt_len += 1;

	/*octet 4:bpdu type
	**stp:0x00; stp tcn:0x80; rstp&mstp:0x02;*/
	*pbuf++ = BPDU_TYPE_TCN;
	pkt_len += 1;

/*do something to send*/
	port->tx_len = pkt_len;
	/*encode over*/

	port->bpdu_tcn_send += 1;

	/*do something to send*/
	mstp_send_frame(port);


if(0)
{
	unsigned char *pf = NULL;	/*pf:	printf*/
	unsigned char buf[1024];
	
	memcpy(buf, &port->tx_frame[0], pkt_len);
	pf = (unsigned char*)&buf[0];

	printf("tx TCN frame, sendsize = %u:\n", pkt_len);
		
	/*dmac*/
	printf("dmac : %02x:%02x:%02x:%02x:%02x:%02x \n",
		pf[0], pf[1], pf[2], pf[3], pf[4], pf[5]);
	pf += MAC_LEN;
		
	/*smac*/
	printf("smac : %02x:%02x:%02x:%02x:%02x:%02x \n",
		pf[0], pf[1], pf[2], pf[3], pf[4], pf[5]);		
	pf += MAC_LEN;

	/*lenth*/
	printf("msg lenth : %u \n", 
			((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

	/*LLC*/
	printf("LLC(DSAP[%02x] SSAP[%02x] CONTROL[%02x])\n",
			pf[0], pf[1], pf[2]);	
	pf += 3;
		
/*payload*/
	/*octet 1-2:protocol id*/
	printf("protocol id: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;
		
	/*octet 3:protocol version id
	**stp cfg:0x00;  stp tcn:0x00;rstp:0x02; mstp:0x03*/
	printf("protocol id: %02x\n", pf[0]);
	pf += 1;

	/*octet 4:bpdu type
	**stp:0x00; stp tcn:0x80; rstp&mstp:0x02;*/
	printf("bpdu type: %02x\n", pf[0]);
	pf += 1;

#if 0
int i;		
	for(i = 0; i < pkt_len; i++)
	{
		if(i % 15 == 0)
		{
			printf("\n");
		}

		printf("%02x ", port->tx_frame[i]);
	}
	printf("\n");
#endif

}

	return;
}

/* txMstp()
Transmits an MST BPDU (14.3.3), encoded according to the specification contained in 14.6. The first six
components of the CIST message priority vector (13.24.6) conveyed in the BPDU are set to the value of cist-
PortPriority (13.24.8) for this Port. */
void mstp_tx_mstp(struct mstp_port* port)
{
	unsigned char				*pbuf		= NULL;
	unsigned int				instance_id	= 0;
	unsigned int				msti_count	= 0;
	unsigned int				uint_value	= 0;	
	unsigned short				ushort_value = 0;	
	
	unsigned short				pkt_len		= 0;	
	unsigned short				bpdu_size	= 0;
	unsigned short				lenth		= 0;
	unsigned short				version_3_lenth = 0;

	union bpdu_flags			flag;
	
	union msti_flag				msti_flag;
	
	struct msti_config_msg		msti_msg;
	struct mstp_msti_port		*instance_port = NULL;
	struct mstp_common_port		*common_info = NULL;

	if ((NULL == port) || (NULL == mstp_global_bridge))
	{
		return;
	}

	/*filter port, no need to tx bpdu*/
	if (MSTP_TRUE == port->mstp_filter_port)
	{
		return;
	}

	memset((void *)&flag, 0, sizeof(union bpdu_flags));
	


	/*need clear tx_frame buff*/
	memset(port->tx_frame, 0x00, MSTP_MAX_FRAME_SIZE);
	pbuf = (unsigned char*)&port->tx_frame[0];

	/*how many msti port in this port*/
	for(instance_id = 0; instance_id < MSTP_INSTANCE_MAX; ++instance_id)
	{
		instance_port = port->msti_port[instance_id];
		if(instance_port != NULL)
		{
			msti_count++;
		}
	}

	/*mstp bpdu size*/
	if(PROTO_VERID_MSTP == port->stp_mode) /*mstp mode*/
	{
		/*[Protocol Identifier -to-MSTI Configuration Messages(may be absent)] is 102B*/
		bpdu_size = MSTP_BPDU_LEN + msti_count * MSTP_BASE_MSTI_LENTH;
	}
	else if(PROTO_VERID_RSTP == port->stp_mode) /*rstp mode*/
	{
		bpdu_size =	MSTP_RST_BPDU_LEN;
	}
	else if(PROTO_VERID_STP == port->stp_mode) /*stp mode*/
	{
		/*do not use this send stp bpdu*/
		MSTP_LOG_DBG("%s[%u]:%s:ERROR: stp mode error!\n", __FILE__, __LINE__, __func__);
	}
	else
	{
		MSTP_LOG_DBG("%s[%u]:%s:ERROR: stp mode error!\n", __FILE__, __LINE__, __func__);
		return; 
	}


	/*dmac*/
	*pbuf++ = MSTP_DEST_MAC_0;
	*pbuf++ = MSTP_DEST_MAC_1;
	*pbuf++ = MSTP_DEST_MAC_2;
	*pbuf++ = MSTP_DEST_MAC_3;
	*pbuf++ = MSTP_DEST_MAC_4;
	*pbuf++ = MSTP_DEST_MAC_5;
	pkt_len += 6;

	/*smac*/
	memcpy(pbuf, &mstp_global_bridge->bridge_mac.mac_addr[0], 6);
	pbuf += 6;
	pkt_len += 6;

	/*lenth*/
	lenth = bpdu_size + 3;	/*3B:	(DSAP+SSAP+control)*/
	*pbuf++ = (unsigned char)(lenth >> 8);
	*pbuf++ = (unsigned char)lenth;
	pkt_len += 2;

	/*LLC*/
	*pbuf++ = 0x42;/*DSAP*/
	*pbuf++ = 0x42;/*SSAP*/
	*pbuf++ = 0x03;/*control*/
	pkt_len += 3;

/*payload*/
	/*octet 1-2:protocol id*/
	*pbuf++ = 0;
	*pbuf++ = 0;
	pkt_len += 2;

	/*octet 3:protocol version id
	**stp cfg:0x00;  stp tcn:0x00;rstp:0x02; mstp:0x03*/
	if(PROTO_VERID_MSTP == port->stp_mode) /*mstp mode*/
	{
		*pbuf++ = PROTO_VERID_MSTP;
		pkt_len += 1;
	}
	else if(PROTO_VERID_RSTP == port->stp_mode) /*rstp mode*/
	{
		*pbuf++ = PROTO_VERID_RSTP;
		pkt_len += 1;
	}
	else if(PROTO_VERID_STP == port->stp_mode) /*stp mode*/
	{
		/*do not use this send stp bpdu*/
		MSTP_LOG_DBG("%s[%u]:%s:ERROR: stp mode error!\n", __FILE__, __LINE__, __func__);
	}

	/*octet 4:bpdu type
	**stp:0x00; stp tcn:0x80; rstp&mstp:0x02;*/
	*pbuf++ = BPDU_TYPE_MSTP;
	pkt_len += 1;


	/*octet 5:flag*/
	if(port->cist_port.common_info.tcWhile)
	{
		flag.flag |= BPDU_FLAG_TC;
		
		port->bpdu_tc_send += 1;
	}
	/*bug#52884 bug#52835
	<rstp>P/A机制必须在点到点链路上进行
	<rstp>根端口down，指定端口收到更优rst bpdu，经过3个hello time才变为新的根端口*/
	
	if(port->cist_port.common_info.proposing)
	{
		flag.flag |= BPDU_FLAG_PROPOSAL;
	}
	
	flag.flag |= mstp_get_port_role(port, 0);
	
	if(port->cist_port.common_info.learning)
	{
		flag.flag |= BPDU_FLAG_LEARNING;
	}
	if(port->cist_port.common_info.forwarding)
	{
		flag.flag |= BPDU_FLAG_FORWARDING;
	}
	if(port->cist_port.common_info.agree)
	{
		flag.flag |= BPDU_FLAG_AGREEMENT;
	}

	*pbuf++ = flag.flag;
	pkt_len += 1;

	/*octet 6-13:root id*/
	*pbuf++ = port->cist_port.designated_priority.root_id.pri[0];
	*pbuf++ = port->cist_port.designated_priority.root_id.pri[1];
	memcpy(pbuf, (unsigned char*)&port->cist_port.designated_priority.root_id.mac_addr[0], 6);	

	pbuf += 6;
	pkt_len += 8;

	/*octet 14-17:CIST External Root Path Cost*/
	uint_value = htonl(port->cist_port.designated_priority.external_root_path_cost);
	memcpy(pbuf, &uint_value, 4);
	pbuf += 4;

	pkt_len += 4;

	/*octet 18-25:CIST Regional Root Identifier*/
	*pbuf++ = port->cist_port.designated_priority.regional_root_id.pri[0];
	*pbuf++ = port->cist_port.designated_priority.regional_root_id.pri[1];
	memcpy(pbuf, (unsigned char*)&port->cist_port.designated_priority.regional_root_id.mac_addr[0], MAC_LEN);

	pbuf += 6;
	pkt_len += 8;

	/*octet 26-27:CIST Port Identifier */
	/*need to modify*/
	*pbuf++ =(unsigned char)(port->cist_port.designated_priority.designated_port_id >> 8);
	*pbuf++ =(unsigned char)(port->cist_port.designated_priority.designated_port_id);
	pkt_len += 2;

	/*octet 28-29:Message Age timer value *///port->cist_port.port_times.msg_age
	if(BIG_ENDIAN_ON)
	{
		ushort_value = htons(port->cist_port.designated_times.msg_age) << 8;
	}
	else
	{
		ushort_value = htons(port->cist_port.designated_times.msg_age);
	}

	*pbuf++ =(unsigned char)(ushort_value >> 8);
	*pbuf++ =(unsigned char)(ushort_value); 
	pkt_len += 2;


	/*octet 30-31:Max Age timer value*///port->cist_port.port_times.max_age
	if(BIG_ENDIAN_ON)
	{
		ushort_value = htons(port->cist_port.designated_times.max_age) << 8;
	}
	else
	{
		ushort_value = htons(port->cist_port.designated_times.max_age);
	}

	*pbuf++ =(unsigned char)(ushort_value >> 8);
	*pbuf++ =(unsigned char)(ushort_value);
	pkt_len += 2;

	/*octet 32-33:Hello Time timer value*///port->cist_port.port_times.hello_time
	if(BIG_ENDIAN_ON)
	{
		ushort_value = htons(port->cist_port.designated_times.hello_time) << 8;
	}
	else
	{
		ushort_value = htons(port->cist_port.designated_times.hello_time);
	}
	
	*pbuf++ =(unsigned char)(ushort_value >> 8);
	*pbuf++ =(unsigned char)(ushort_value);
	pkt_len += 2;

	/*octet 34-35: Forward Delay timer value.*/
	if(BIG_ENDIAN_ON)
	{
		ushort_value = htons(port->cist_port.designated_times.forward_delay) << 8;
	}
	else
	{
		ushort_value = htons(port->cist_port.designated_times.forward_delay);
	}
	
	*pbuf++ =(unsigned char)(ushort_value >> 8);
	*pbuf++ =(unsigned char)(ushort_value);
	pkt_len += 2;

	/*octet 36: the Version 1 Length
	** This shall be transmitted as 0.*/
	*pbuf++ = 0x00;
	pkt_len += 1;
/*rstp end*/

	if(PROTO_VERID_MSTP == port->stp_mode)
	{
		/*octet 37-38:the Version 3 Length*/
		version_3_lenth = MSTP_EXT_BPDU_BASE_LEN + msti_count * MSTP_BASE_MSTI_LENTH;
		*pbuf++ = (unsigned char)(version_3_lenth >> 8);
		*pbuf++ = (unsigned char)version_3_lenth;
		pkt_len += 2;

/*octet 39-89:MST Configuration Identifier*/

		/*octet 39:The Configuration Identifier Format Selector:0x00*/
		*pbuf++ = mstp_global_bridge->mst_cfg_id.cfg_format_id_selector;
		pkt_len += 1;

		/*octet 40-71:The Configuration Name */
		for(int k =0; k < NAME_LEN; ++k)
		{
			*pbuf++ = mstp_global_bridge->mst_cfg_id.cfg_name[k];
		}

		pkt_len += NAME_LEN;

		/*octet 72-73:The Revision Level*/
		*pbuf++ = (unsigned char)(mstp_global_bridge->mst_cfg_id.revison_level >> 8);
		*pbuf++ = (unsigned char)(mstp_global_bridge->mst_cfg_id.revison_level);		
		pkt_len += 2;

		/*octet 74-89:The Configuration Digest */
		for(int k =0; k < DIGEST_LEN; ++k)
		{
			*pbuf++ = mstp_global_bridge->mst_cfg_id.cfg_digest[k];
		}		
		pkt_len += DIGEST_LEN;

		/*octet 90-93:the CIST Internal Root Path Cost*/
		uint_value = htonl(port->cist_port.designated_priority.internal_root_path_cost);
		memcpy(pbuf, &uint_value, 4);
		pbuf += 4;

		pkt_len += 4;

		/*octet 94-101:the CIST Bridge Identifier*/
		*pbuf++ = port->cist_port.designated_priority.designated_bridge_id.pri[0];
		*pbuf++ = port->cist_port.designated_priority.designated_bridge_id.pri[1];
		memcpy(pbuf, (unsigned char*)&port->cist_port.designated_priority.designated_bridge_id.mac_addr[0], MAC_LEN);
		pbuf += 6;
		pkt_len += 8;

		/*octet 102: the value of remaining Hops*/
		*pbuf++ = port->cist_port.designated_times.remaining_hops;
		pkt_len += 1;

/*octet 103-xxx:MSTI Configuration Messages*/
		/* (n*16)bytes */	

		for(instance_id = 0; instance_id < MSTP_INSTANCE_MAX; ++instance_id)
		{
			/*memset*/
			memset((void *)&msti_msg, 0x00, sizeof(struct msti_config_msg));
			memset((void *)&msti_flag, 0, sizeof(union msti_flag));
			
			instance_port = port->msti_port[instance_id];
			if(instance_port != NULL)
			{
				/*need to confirm flag*/
				/*Octets 1: flag*/
				common_info = &(port->msti_port[instance_id]->common_info);

				if(common_info->tcWhile)
				{
					msti_flag.flag |= BPDU_FLAG_TC;
				}
				if(common_info->proposing)
				{
					msti_flag.flag |= BPDU_FLAG_PROPOSAL;
				}
				msti_flag.flag |= mstp_get_port_role(port, instance_id+1);
	
				if(common_info->learning)
				{
					msti_flag.flag |= BPDU_FLAG_LEARNING;
				}
				if(common_info->forwarding)
				{
					msti_flag.flag |= BPDU_FLAG_FORWARDING;
				}
				if(common_info->agree)
				{
					msti_flag.flag |= BPDU_FLAG_AGREEMENT;
				}
				if(port->msti_port[instance_id]->master)
				{
					msti_flag.flag |= BPDU_ROLE_MASTER;
				}
				*pbuf++ = msti_flag.flag;

				pkt_len += 1;

				/*Octets 2-9: convey the Regional Root Identifier.*/
				unsigned char tmp;

				tmp = (unsigned char)(instance_port->designated_priority.rg_root_id.pri[0] & 0xF0)
						| (unsigned char)((instance_id+1) >> 8);
				*pbuf++ = tmp;
				tmp = (unsigned char)(instance_port->designated_priority.rg_root_id.pri[1])
						| (unsigned char)(instance_id+1);
				*pbuf++ = tmp;
				memcpy(pbuf, (unsigned char*)&instance_port->designated_priority.rg_root_id.mac_addr[0], 6);
				pbuf += 6;
				pkt_len += 8;

				/*Octets 10-13:the Internal Root Path Cost*/
				uint_value = htonl(instance_port->designated_priority.internal_root_path_cost);
				memcpy(pbuf, &uint_value, 4);
				pbuf += 4;

				pkt_len += 4;

				/*Octets 14:(bits[xxxx 0000])Bridge Identifier Priority for this MST*/
				*pbuf++ = instance_port->designated_priority.designated_bridge_id.pri[0] & 0xF0;				
				pkt_len += 1;

				/*Octets 15:MSTI Port Priority*/
				/*need to modify */	//FIXME
				*pbuf++ = (instance_port->designated_priority.designated_port_id >> 8) & 0xF0;
				pkt_len += 1;

				/*Octets 16:MSTI Remaining Hops */
				*pbuf++ = instance_port->designated_times.remaining_hops;				
				pkt_len += 1;
			}
		}
		
		port->bpdu_mst_send += 1;
	}
	else
	{
		port->bpdu_rst_send += 1;
	}
	
	port->tx_len = pkt_len;
/*encode over*/

/*do something to send*/
	mstp_send_frame(port);

/*print debug info*/
if(0)
{
	int						i, ins_id;
	unsigned char			*pf = NULL;		/*pf:	printf*/
	unsigned char			buf[1024];

	struct mstp_msti_port	*ins_port;
	unsigned char 			config_name[NAME_LEN];

	memcpy(buf, &port->tx_frame[0], pkt_len);
	pf = (unsigned char*)&buf[0];
	
	printf("send tx mstp frame: sendsize = %u:\n", pkt_len);
	
	/*dmac*/
	printf("dmac : %02x:%02x:%02x:%02x:%02x:%02x \n",
		pf[0], pf[1], pf[2], pf[3], pf[4], pf[5]);
	pf += MAC_LEN;
	
	/*smac*/
	printf("smac : %02x:%02x:%02x:%02x:%02x:%02x \n",
		pf[0], pf[1], pf[2], pf[3], pf[4], pf[5]);
	pf += MAC_LEN;
	
	/*lenth*/
	printf("msg lenth : %u \n", 
		((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;
	
	/*LLC*/
	printf("LLC(DSAP[%02x] SSAP[%02x] CONTROL[%02x])\n",
		pf[0], pf[1], pf[2]);	
	pf += 3;
	
/*payload*/
	/*octet 1-2:protocol id*/
	printf("protocol id: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));	
	pf += 2;
	
	/*octet 3:protocol version id
	**stp cfg:0x00;  stp tcn:0x00;rstp:0x02; mstp:0x03*/
	printf("protocol id: %02x\n", pf[0]);
	pf += 1;
	
	/*octet 4:bpdu type
	**stp:0x00; stp tcn:0x80; rstp&mstp:0x02;*/
	printf("bpdu type: %02x\n", pf[0]);	
	pf += 1;
	
	/*octet 5:flag*/
	printf("flag: %02x\n", pf[0]);
	pf += 1;	
	
	/*octet 6-13:root id*/
	printf("root id priority: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

	/*root id mac*/
	printf("root id mac : %02x:%02x:%02x:%02x:%02x:%02x \n",
		pf[0], pf[1], pf[2], pf[3], pf[4], pf[5]);
	pf += MAC_LEN;
		

	/*octet 14-17:CIST External Root Path Cost*/
	printf("CIST External Root Path Cost : %08x \n",
		 (unsigned int)(pf[0] << 24)
		|(unsigned int)(pf[1] << 16)
		|(unsigned int)(pf[2] << 8)
		|(unsigned int)(pf[3]));
	pf += 4;
	
	/*octet 18-25:CIST Regional Root Identifier*/
	printf("CIST Regional Root Identifier priority: %04x\n", 
		((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;
	
	/*CIST Regional Root Identifier MAC*/
	printf("CIST Regional Root Identifier mac : %02x:%02x:%02x:%02x:%02x:%02x \n",
		pf[0], pf[1], pf[2], pf[3], pf[4], pf[5]);
	pf += MAC_LEN;
	
	/*octet 26-27:CIST Port Identifier */
	printf("CIST Port Identifier: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;
	
	/*octet 28-29:Message Age timer value */
	printf("Message Age timer value: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;
	
	/*octet 30-31:Max Age timer value*/
	printf("MAX Age timer value: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;
	
	/*octet 32-33:Hello Time timer value*/	
	printf("Hello Time timer value: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

	/*octet 34-35: Forward Delay timer value.*/
	printf("Forward Delay timer value: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

/*octet 36: the Version 1 Length
** This shall be transmitted as 0.*/
	printf("the Version 1 Length: %02x\n", pf[0]);
	pf += 1;

	/*octet 37-38:the Version 3 Length*/
	printf("the Version 3 Length: %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;


/*octet 39-89:MST Configuration Identifier*/
	printf("MST Configuration Identifier: \n");

	/*octet 39:The Configuration Identifier Format Selector:0x00*/
	printf("The Configuration Identifier Format Selector: %02x\n", pf[0]);
	pf += 1;

	/*octet 40-71:The Configuration Name */
	for(i = 0; i < NAME_LEN; i++)
	{
		config_name[i] = pf[i];
	}
	pf += NAME_LEN;
	
	printf("The Configuration Name : %s\n", config_name);

	/*octet 72-73:The Revision Level*/
	printf("The Revision Level : %04x\n", ((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

	/*octet 74-89:The Configuration Digest */
	printf("The Configuration Digest : \n");
	for(i = 0; i < DIGEST_LEN; i++)
	{	
		printf(" %02x", pf[i]);
	}	
	pf += DIGEST_LEN;
	printf("\n");


/*octet 90-93:the CIST Internal Root Path Cost*/
	printf("the CIST Internal Root Path Cost : %08x\n", 
		 (unsigned int)(pf[0] << 24)
		|(unsigned int)(pf[1] << 16)
		|(unsigned int)(pf[2] << 8)
		|(unsigned int)(pf[3]));
	pf += 4;


/*octet 94-101:the CIST Bridge Identifier*/
	printf("CIST Regional Root Identifier priority: %04x\n", 
		((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
	pf += 2;

/*CIST Regional Root Identifier MAC*/
	printf("CIST Regional Root Identifier mac : %02x:%02x:%02x:%02x:%02x:%02x \n",
		pf[0], pf[1], pf[2], pf[3], pf[4], pf[5]);
	pf += MAC_LEN;

/*octet 102: the value of remaining Hops*/
	printf("the value of remaining Hops: %02x\n", (pf[0]));
	pf += 1;

/*octet 103-xxx:MSTI Configuration Messages*/
	/* (n*16)bytes */

	for(ins_id = 0; ins_id < MSTP_INSTANCE_MAX; ++ins_id)
	{
		ins_port = port->msti_port[ins_id];
		
		if(ins_port != NULL)
		{
			printf("the instance id: %u\n", ins_id+1);
		
			/*Octets 1: flag*/
			printf("flag: %02x\n", (pf[0]));
			pf += 1;

			/*Octets 2-9: convey the Regional Root Identifier.*/
			printf("Regional Root Identifier priority: %04x\n", 
				((unsigned short)(pf[0] << 8) | (unsigned short)(pf[1])));
			pf += 2;
			
			/*convey the Regional Root Identifier MAC*/
			printf("Regional Root Identifier MAC : %02x:%02x:%02x:%02x:%02x:%02x \n",
				pf[0], pf[1], pf[2], pf[3], pf[4], pf[5]);
			pf += MAC_LEN;
					
			/*Octets 10-13:the Internal Root Path Cost*/
			printf("the Internal Root Path Cost : %08x\n", 
				 (unsigned int)(pf[0] << 24)
				|(unsigned int)(pf[1] << 16)
				|(unsigned int)(pf[2] << 8)
				|(unsigned int)(pf[3]));
			pf += 4;

			/*Octets 14:(bits[xxxx 0000])Bridge Identifier Priority for this MST*/		
			printf("Bridge Identifier Priority: %02x\n", (pf[0]));
			pf += 1;
			
			/*Octets 15:MSTI Port Priority */		
			printf("MSTI Port Priority: %02x\n", (pf[0]));
			pf += 1;
			
			/*Octets 16:MSTI Remaining Hops */
			printf("MSTI Remaining Hops: %02x\n", (pf[0]));
			pf += 1;
		}
	}

#if 0
	int i;
	for(i = 0; i < pkt_len; i++)
	{
		if(i % 15 == 0)
		{
			printf("\n");
		}

		printf("%02x ", port->tx_frame[i]);
	}
	printf("\n");
#endif
}

}

void mstp_send_frame(struct mstp_port* port)
{
	union pkt_control pktcontrol;

	if (NULL == port)
	{
		return;
	}

	if(PORT_LINK_DOWN == port->link_status)
	{
		memset(&port->tx_frame[0], 0, MSTP_MAX_FRAME_SIZE);
		return;
	}

if(0)
{
	printf("total :tx.frame:\n");

	int i;							
	for(i = 0; i < port->tx_len;i++)
	{
		if(i%15 == 0)
		{
			printf("\n");
		}

		printf("%02x ", port->tx_frame[i]);
	}
	printf("\n");	
	printf("port(%08x)	port->tx_len = %u\n", port->ifindex, port->tx_len);	
}

	pktcontrol.rawcb.out_ifindex = port->ifindex;
	pktcontrol.rawcb.priority = 1;
	pkt_send(PKT_TYPE_RAW, &pktcontrol, port->tx_frame, port->tx_len);	

    return;
}


/*
betterorsameInfoCist()
Returns TRUE if the received CIST priority vector is better than or the same as 
(13.10) the CIST port priority vector.
*/
/*betterorsameInfoMsti()
Returns TRUE if the MSTI priority vector is better than or the same as 
(13.11) the MSTI port priority vector.*/
unsigned int mstp_better_or_same_info(struct mstp_port* port, unsigned int instance_id, int newInfoIs)
{
	struct mstp_cist_port		*cist_port = NULL;
	struct mstp_msti_port		*msti_port = NULL;
	struct mstp_common_port		*common_info = NULL;

	if (NULL == port)
	{
		return MSTP_FALSE;
	}

	cist_port = &port->cist_port;

/*cist*/
	if(0 == instance_id)
	{
		common_info = &cist_port->common_info;
		if((MSTP_INFO_RECEIVED == newInfoIs)
			&& (MSTP_INFO_RECEIVED == common_info->infoIs)
			&& (mstp_cist_info_cmp(&cist_port->port_priority, &cist_port->msg_priority) != INFO_WORSE))
		{
			return MSTP_TRUE;
		}
		else if((MSTP_INFO_MINE == newInfoIs)
			&& (MSTP_INFO_MINE == common_info->infoIs)
			&& (mstp_cist_info_cmp(&cist_port->port_priority, &cist_port->designated_priority) != INFO_WORSE))
		{
			return MSTP_TRUE;
		}
	}
	else /*msti*/
	{
		msti_port = port->msti_port[instance_id-1];
		common_info = &msti_port->common_info;
		if((MSTP_INFO_RECEIVED == newInfoIs)
			&& (MSTP_INFO_RECEIVED == common_info->infoIs)
			&& (mstp_msti_info_cmp(&msti_port->port_priority, &msti_port->msg_priority) != INFO_WORSE))
		{
			return MSTP_TRUE;
		}
		else if((MSTP_INFO_MINE == newInfoIs) && (MSTP_INFO_MINE == common_info->infoIs) &&
			mstp_msti_info_cmp(&msti_port->port_priority, &msti_port->designated_priority) != INFO_WORSE)
		{
			return MSTP_TRUE;
		}
	}
	
	return MSTP_FALSE;
}

/*
 rcvInfoCist()
Returns SuperiorDesignatedInfo if the received CIST message conveys a message priority (cistMsgPrior-
ity-13.24.6) that is superior (13.10) to the Port`s port priority vector, or any of the received timer parameter
values (cistMsgTimes-13.24.7) differ from those already held for the Port (cistPortTimes-13.24.9).*/
/*rcvInfoMsti()
Returns SuperiorDesignatedInfo if the received MSTI message conveys a message priority (mstiMsgPrior-
ity-13.24.15) that is superior (13.11) to the Port`s port priority vector, or any of the received timer parame-
ter values (mstiMsgTimes-13.24.16) differ from those already held for the Port (mstiPortTimes-
13.24.18).*/
unsigned int mstp_recv_info(struct mstp_port* port, unsigned int instance_id)
{
	int							cmp_ret = 0;
	unsigned int				ins_id = 0;
	unsigned int				type = MSTP_INFO_OTHER;

	struct mstp_cist_port		*cist_port = NULL;
	struct mstp_msti_port		*msti_port = NULL;
	struct mstp_common_port		*common_info = NULL;

	if (NULL == port)
	{
		return MSTP_INFO_OTHER;
	}

	cist_port = &port->cist_port;

/*setTcFlags()
If the received BPDU is a TCN BPDU, sets rcvdTcn TRUE 
and sets rcvdTc TRUE for each and everyMSTI.*/
/*Otherwise, returns OtherInfo.*/
	if(MSTP_BPDUTYPE_TCN == cist_port->bpdu_cfg.message_type)
	{
        port->port_variables.rcvdTcn		= MSTP_TRUE;
		cist_port->common_info.rcvdTc		= MSTP_TRUE;

		for(ins_id = 0; ins_id < MSTP_INSTANCE_MAX; ins_id++)
		{
			msti_port = port->msti_port[ins_id];
			if(msti_port != NULL)
			{
				msti_port->common_info.rcvdTc  = MSTP_TRUE;
			}
		}

		return MSTP_INFO_OTHER;
	}

	if(0 == instance_id) /*cist*/
	{
		common_info = &cist_port->common_info;
        cmp_ret   = mstp_cist_info_cmp(&cist_port->port_priority, &cist_port->msg_priority);

		if((INFO_WORSE == cmp_ret) && (MSTP_DESIGNATED_PORT == cist_port->common_info.sm_deg_role_state))
		{
			/*send packet*/
			if(PROTO_VERID_STP == port->stp_mode)
			{
				mstp_tx_config(port);
			}
			else
			{
				mstp_tx_mstp(port);
			}			
		}

		switch(cist_port->bpdu_cfg.role)
		{
			case BPDU_ROLE_DESIGNATED:
			{
/*Returns SuperiorDesignatedInfo if the received CIST message conveys a message priority 
(cistMsgPriority-13.24.6) that is superior (13.10) to the Port`s port priority vector, 
or any of the received timer parameter values (cistMsgTimes-13.24.7) differ from those already
held for the Port (cistPortTimes-13.24.9).*/				
				if((INFO_BETTER == cmp_ret)
					|| ((MSTP_FALSE == mstp_cist_times_cmp(&cist_port->port_times, &cist_port->msg_times))
						&& (INFO_SAME == cmp_ret)))
				{
					type = MSTP_INFO_SUPERIOR_DESIGNATED;
				}
/*Returns RepeatedDesignatedInfo if the received CIST message conveys a message priority 
vector and timer parameters that are the same as the Port`s port priority vector or timer values*/				
				else if((INFO_SAME == cmp_ret)
						&& (MSTP_TRUE == mstp_cist_times_cmp(&cist_port->port_times, &cist_port->msg_times))
						&& (MSTP_INFO_RECEIVED == common_info->infoIs))
				{
					type = MSTP_INFO_REPEATED_DESIGNATED;
				}

				else type = MSTP_INFO_INFERIOR_DESIGNATED;
			}
				break;
			case BPDU_ROLE_ALTBCK:
			case BPDU_ROLE_ROOT:
			{
/*Returns RootInfo if the received CIST message conveys a Root Port Role and a CIST message
priority that is the same or worse than the CIST port priority vector.*/
				if(cmp_ret <= INFO_WORSE)	
				{
					//type = MSTP_INFO_ROOT;
					type = MSTP_INFO_NOT_DESIGNATED;
				}
			}
				break;
			default:
				break;
		}
	}
	else	/*msti*/
	{
		msti_port = port->msti_port[instance_id-1];
		common_info = &msti_port->common_info;
        cmp_ret   = mstp_msti_info_cmp(&msti_port->port_priority, &msti_port->msg_priority);

		if((INFO_WORSE == cmp_ret) && (MSTP_DESIGNATED_PORT == msti_port->common_info.sm_deg_role_state))
		{
			/*send packet*/
			if(PROTO_VERID_STP == port->stp_mode)
			{
				mstp_tx_config(port);
			}
			else
			{
				mstp_tx_mstp(port);
			}			
		}

		switch(msti_port->bpdu_cfg.role)
		{
			case BPDU_ROLE_DESIGNATED:
			{
/*Returns SuperiorDesignatedInfo if the received MSTI message conveys a message priority (mstiMsgPrior-
ity-13.24.15) that is superior (13.11) to the Port`s port priority vector, or any of the received timer parame-
ter values (mstiMsgTimes-13.24.16) differ from those already held for the Port (mstiPortTimes-
13.24.18).*/				
				if((INFO_BETTER == cmp_ret)
					|| ((MSTP_FALSE == mstp_msti_times_cmp(&msti_port->port_times, &msti_port->msg_times))
						&& (INFO_SAME == cmp_ret)))
				{
					type = MSTP_INFO_SUPERIOR_DESIGNATED;
				}
/*Returns RepeatedDesignatedInfo if the received MSTI message conveys a message priority vector and timer
parameters that are the same as the Port	`s port priority vector or timer values.*/				
				else if((INFO_SAME == cmp_ret)
						&& (MSTP_TRUE == mstp_msti_times_cmp(&msti_port->port_times, &msti_port->msg_times))
						&& (MSTP_INFO_RECEIVED == common_info->infoIs))
				{
					type = MSTP_INFO_REPEATED_DESIGNATED;
				}
			}
				break;
			case BPDU_ROLE_ALTBCK:
			case BPDU_ROLE_ROOT:
			{
/*Returns RootInfo if the received MSTI message conveys a Root Port Role and a MSTI message priority that
is the same or worse than the MSTI port priority vector.*/
				if(cmp_ret <= INFO_WORSE)	
				{				
					//type = MSTP_INFO_ROOT;
					type = MSTP_INFO_NOT_DESIGNATED;
				}
			}
				break;
			default:
				break;	
		}
	}
	
    return type;
}


/*recordMasteredCist()
If the CIST message was received from a Bridge in a different MST Region, i.e. the rcvdInternal flag is clear,
the mstiMastered variable for this Port is cleared for all MSTIs.*/
/*recordMasteredMsti()
If theMSTIMessage was received on a point to point link and theMSTIMessage has theMaster flag set, set
the mstiMastered variable for the MSTI. Otherwise reset the mstiMastered variable.*/

void mstp_record_mastered(struct mstp_port* port, unsigned int instance_id)
{
	unsigned int				ins_id = 0;//instance_id


	struct mstp_msti_port		*msti_port = NULL;
	struct mstp_port_variables	*port_variables = NULL;

	if (NULL == port)
	{
		return;
	}

	port_variables = &port->port_variables;

	if(0 == instance_id) /*cist*/
	{
/*If the CIST message was received from a Bridge in a different MST Region, i.e. the rcvdInternal
flag is clear, the mstiMastered variable for this Port is cleared for all MSTIs.*/
		if(!port_variables->rcvdInternal)
		{
			for(ins_id = 0; ins_id < MSTP_INSTANCE_MAX; ins_id++)
			{
				msti_port = port->msti_port[ins_id];
				if(msti_port != NULL)
				{
					msti_port->mastered = MSTP_FALSE;
				}
			}
		}
	}
	else /*msti*/
	{
/*If theMSTIMessage was received on a point to point link and the MSTIMessage has theMaster
flag set, set the mstiMastered variable for the MSTI. Otherwise reset the mstiMastered variable*/
		msti_port = port->msti_port[instance_id-1];
		if((port_variables->operPointToPointMAC)
			&& (BPDU_ROLE_MASTER == msti_port->bpdu_cfg.role))
		{
			msti_port->mastered = MSTP_TRUE;
		}
		else
		{
			msti_port->mastered = MSTP_FALSE;
		}
	}
	
}


/* recordProposalCist()
If the CISTMessage was a Configuration Message received on a point to point link and has the Proposal flag
set, the CIST proposed flag is set. Otherwise the CIST proposed flag is cleared.*/
/*recordProposalMsti()
If the MSTI Message was received on a point to point link and has the Proposal flag set, the MSTI proposed
flag is set. Otherwise the MSTI proposed flag is cleared.*/
void mstp_record_proposal(struct mstp_port* port, unsigned int instance_id)
{
	unsigned int				ins_id = 0; //instance_id
	
	struct mstp_cist_port		*cist_port = NULL;
	struct mstp_msti_port		*msti_port = NULL;
	struct mstp_port_variables	*port_variables = NULL;

	if (NULL == port)
	{
		return;
	}

	cist_port = &port->cist_port;
	port_variables = &port->port_variables;

	if(0 == instance_id)	/*cist*/
	{
/*If the CISTMessage was a Configuration Message received on a point to point link and 
has the Proposal flag set, the CIST proposed flag is set. Otherwise the CIST proposed flag is cleared.*/	
		if((BPDU_ROLE_DESIGNATED == cist_port->bpdu_cfg.role)
			/*(port_variables->operPointToPointMAC)*/
			&& (cist_port->bpdu_cfg.proposal))
        {
            cist_port->common_info.proposed = MSTP_TRUE;
        }

/*Additionally, if the CIST message was received from a Bridge in a different MST Region
i.e. the rcvdInternal flag is clear, the proposed flags for this Port for allMSTIs are set or 
cleared to the same value as the CIST proposed flag. */
		if(!port_variables->rcvdInternal)
		{
			for(ins_id = 0; ins_id < MSTP_INSTANCE_MAX; ins_id++)
			{
				msti_port = port->msti_port[ins_id];
				if (msti_port != NULL)
				{
					msti_port->common_info.proposed = cist_port->common_info.proposed;
				}
			}
		}
	}
	else /*msti*/
	{
/*If the MSTI Message was received on a point to point link and has the Proposal flag set,
the MSTI proposed flag is set. Otherwise the MSTI proposed flag is cleared.*/	
		msti_port = port->msti_port[instance_id-1];
        if(/*(msti_port->bpdu_cfg.role == BPDU_ROLE_DESIGNATED)*/
			(port_variables->operPointToPointMAC)
			&& (msti_port->bpdu_cfg.proposal))
        {
            msti_port->common_info.proposed = MSTP_TRUE;
        }
		else	//FIXME
		{	
            msti_port->common_info.proposed = MSTP_FALSE;
		}
	}	
}

/*
 recordAgreementCist()
If the CIST Message was a Configuration Message received on a point to point link, and the CIST message
has the Agreement flag set, and conveys either*/
/*
recordAgreementMsti()
If the MSTI Message was received on a point to point link and:
a):
b):1/2/
*/
void mstp_record_agreement(struct mstp_port* port, unsigned int instance_id)
{
	unsigned int				ins_id = 0;//instance_id
	
	struct mstp_cist_port		*cist_port = NULL;
	struct mstp_msti_port		*msti_port = NULL;
	struct mstp_port_variables	*port_variables = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return;
	}

	cist_port = &port->cist_port;
	port_variables = &port->port_variables;

	if(instance_id == 0)
	{
/* If the CIST Message was a Configuration Message received on a point to point link, 
and the CIST message has the Agreement flag set, 
and conveys either:
1) a Root Port Role with message priority the same as or worse than the port priority vector, or
2) a Designated Port Role with message priority the same as or better than the port priority vector,

the CIST agreed flag is set. Otherwise the CIST agreed flag is cleared.
*/
/*bug#52884 bug#52835
<rstp>P/A机制必须在点到点链路上进行
<rstp>根端口down，指定端口收到更优rst bpdu，经过3个hello time才变为新的根端口*/

MSTP_LOG_DBG("%s[%d]#################ifindex = %02x agreement = %d %d %d\n",__FUNCTION__,__LINE__,\
cist_port->port_index,cist_port->bpdu_cfg.agreement,port_variables->operPointToPointMAC,port->m_br->common_br.rstp_version(port));
		if(port->m_br->common_br.rstp_version(port)			
			&& (port_variables->operPointToPointMAC)
			&& (cist_port->bpdu_cfg.agreement))
		{
			cist_port->common_info.agreed = MSTP_TRUE;
			cist_port->common_info.proposing = MSTP_FALSE;
		}
		else
		{
			cist_port->common_info.agreed = MSTP_FALSE;
		}
		
/*Additionally, if the CIST message was received from a Bridge in a different MST Region 
i.e. the rcvdInternal flag is clear, the agreed flags for this Port for all MSTIs are set or cleared to
the same value as the CIST agreed flag. If the CIST message was received from a Bridge in the 
same MST Region, the MSTI agreed*/
		if(!port_variables->rcvdInternal)
		{
			for (ins_id = 0; ins_id < MSTP_INSTANCE_MAX; ins_id++)
			{
				msti_port = port->msti_port[ins_id];
				if (msti_port != NULL)
				{
					msti_port->common_info.agreed = cist_port->common_info.agreed;
					msti_port->common_info.proposing = cist_port->common_info.proposing;
				}
			}
		}
	}
	else	/*msti*/
/*If the MSTI Message was received on a point to point link and:
a) the message priority vector of the CIST Message accompanying this MSTI Message (i.e. received in
the same BPDU) has the same CIST Root Identifier,
CIST External Root Path Cost, 
and Regional Root Identifier as the CIST port priority vector, and
b) the MSTI Message has the Agreement flag set, and conveys either
	1) a Root Port Role with message priority the same as or worse than the MSTI port priority vector,
	or
	2) a Designated Port Role with message priority the same as or better than the port priority vector,

the MSTI agreed flag is set. Otherwise the MSTI agreed flag is cleared.*/		
	{
		msti_port = port->msti_port[instance_id-1];

        if((port_variables->operPointToPointMAC)
			&& (mstp_same_root(cist_port->msg_priority.root_id, cist_port->port_priority.root_id))
			&& (cist_port->msg_priority.external_root_path_cost == cist_port->port_priority.external_root_path_cost)
			&& (mstp_same_root(cist_port->msg_priority.regional_root_id, cist_port->port_priority.regional_root_id))
			&& (msti_port->bpdu_cfg.agreement))
        {
            msti_port->common_info.agreed = MSTP_TRUE;
            msti_port->common_info.proposing = MSTP_FALSE;
        }
        else
        {
            msti_port->common_info.agreed = MSTP_FALSE;
        }
	}
}

/*13.27.17 recordPriority()
*Sets the components of the portPriority variable to the values of the corresponding msgPriority components.*/
void mstp_record_priority(struct mstp_port* port, unsigned int instance_id)
{
	struct mstp_msti_port		*msti_port = NULL;
	struct mstp_cist_port		*cist_port = NULL;

	if (NULL == port)
	{
		return;
	}

	cist_port = &port->cist_port;

    if(0 == instance_id) /*cist*/
    {
		cist_port->port_priority = cist_port->msg_priority;
    }
    else /*msti*/
    {
		msti_port = port->msti_port[instance_id-1];
		msti_port->port_priority = msti_port->msg_priority;
    }
}

/*13.27.15 recordDispute()*/
void mstp_record_dispute(struct mstp_port* port, unsigned int instance_id)
{
	unsigned int				i = 0;
	struct mstp_cist_port		*cist_port = NULL;
	struct mstp_msti_port		*msti_port = NULL;	
	struct mstp_port_variables	*port_variables = NULL;

	if (NULL == port)
	{
		return;
	}

	cist_port = &port->cist_port;
	port_variables = &port->port_variables;

	/**/
    if(0 == instance_id)
    {
    	/*For the CIST and a given port, if the CIST message has the learning flag set:
		a) The disputed variable is set; and
		b) The agreed variable is cleared.*/
        if(cist_port->bpdu_cfg.learning)
        {
            cist_port->common_info.disputed = MSTP_TRUE;
            cist_port->common_info.agreed = MSTP_FALSE;
        }

		/*Additionally, if the CIST message was received from a bridge in a different MST region (i.e., if the
		rcvdInternal flag is clear), then for all the MSTIs:
		c) The disputed variable is set; and
		d) The agreed variable is cleared.*/
		if (!port_variables->rcvdInternal)
		{
			for (i = 0;i < MSTP_MSTI_MAX;i++)
			{
				msti_port = port->msti_port[i];
				if (msti_port != NULL)
				{
					msti_port->common_info.disputed = MSTP_TRUE;
					msti_port->common_info.agreed = MSTP_FALSE;
				}
			}
		}
    }
    else
    {
    	/*For a given MSTI and port, if the received MSTI message has the learning flag set:
		e) The disputed variable is set; and
		f) The agreed variable is cleared.*/
		msti_port = port->msti_port[instance_id-1];
        if(msti_port->bpdu_cfg.learning)
        {
            msti_port->common_info.disputed = MSTP_TRUE;
            msti_port->common_info.agreed = MSTP_FALSE;
        }
    }

	return;
}

/*13.27.19 recordTimes()
For the CIST and a given port, sets portTimes`Message Age, Max Age, Forward Delay, and remainingHops
to the received values held in msgTimes and portTimes`Hello Time to the default specified in Table 13-5.
For a given MSTI and port, sets portTime`s remainingHops to the received value held in msgTimes.*/
void mstp_record_times(struct mstp_port* port, unsigned int instance_id)
{
	struct mstp_msti_port		*msti_port = NULL;
	struct mstp_cist_port		*cist_port = NULL;

	if (NULL == port)
	{
		return;
	}

	cist_port = &port->cist_port;

	if(0 == instance_id) /*cist*/
	{
		cist_port->port_times = cist_port->msg_times;
	}
	else /*msti*/
	{
		msti_port = port->msti_port[instance_id-1];
		msti_port->port_times = msti_port->msg_times;
	}
}

/*updtRcvdInfoWhileCist()
Calculates the effective age, or remainingHops, to limit the propagation and longevity of received Spanning
Tree information for the CIST, setting rcvdInfoWhile to the number of seconds that the information received
on a Port will be held before it is either refreshed by receipt of a further configuration message or aged out.*/
/*updtRcvdInfoWhileMsti()
Calculates the remainingHops, to limit the propagation and longevity of received Spanning Tree information
for an MSTI, setting rcvdInfoWhile to the number of seconds that the information received on a Port will be
held before it is either refreshed by receipt of a further configuration message or aged out.*/
void mstp_updt_rcvd_info_while(struct mstp_port* port, unsigned int instance_id)
{
	struct mstp_msti_port		*msti_port = NULL;
	struct mstp_cist_port		*cist_port = NULL;
	struct mstp_port_variables	*port_variables = NULL;

	if (NULL == port)
	{
		return;
	}

	cist_port = &port->cist_port;
	port_variables = &port->port_variables;

	if(0 == instance_id)
	{
		/*If the information was received from a Bridge external to 
    		the MST Region (rcvdInternal FALSE)*/
		if(((!port_variables->rcvdInternal)
			&& ((cist_port->port_times.msg_age + 1) <= cist_port->port_times.max_age))
		/*If the information was received from a Bridge in the same MST Region
		(rcvdInternal TRUE)
		zero, if either cistPortTimes Message Age effective exceeds Max Age 
		or remainingHops is less than or equal to zero*/
			|| ((port_variables->rcvdInternal) && ((cist_port->port_times.remaining_hops - 1) > 0)))
        {
        	/*bug#51830   <stp>stp模式，配置bpdu老化时间Max Age不准确*/
			//cist_port->common_info.rcvdInfoWhile = 3*(cist_port->port_times.hello_time);
			
			if(6 == cist_port->port_times.max_age)
			{
			
				cist_port->common_info.rcvdInfoWhile  = 15;
			}
			else 
			{
			
				cist_port->common_info.rcvdInfoWhile  = 20;
			
			}
			
        }
        else
        {   /*zero, if the effective age exceeds Max Age.*/
        	MSTP_LOG_DBG("%s[%d]######msg_age = %d,max_age = %d\n",\
    		__FUNCTION__,__LINE__,cist_port->port_times.msg_age + 1,cist_port->port_times.max_age);
            cist_port->common_info.rcvdInfoWhile = 0;

			#if 1
			/*bug#52418 bug#52419 recv optimal pdu but massge > max. refuse it */
			/*<stp>非根桥转发了Message比Max Age大的配置bpdu*/
			/*<stp>非根桥收到Message等于Max Age或比Max Age小1的配置bpdu时，根桥角色反复切换*/
			
			cist_port->common_info.infoIs = MSTP_INFO_AGED;
			cist_port->port_priority = cist_port->designated_priority;
			cist_port->port_times	= cist_port->designated_times;
			#endif
			#if 0
			cist_port->common_info.infoIs =  MSTP_INFO_MINE;
			cist_port->common_info.sm_info_state = MSTP_UPDATE;
			#endif
        }
    }
    else
    {
		msti_port = port->msti_port[instance_id-1];		
		/*zero, if mstiPortTimes remainingHops is less than or equal to zero.*/
        if((port_variables->rcvdInternal) && ((msti_port->port_times.remaining_hops - 1) > 0))
        {
        	/*bug#51830   <stp>stp模式，配置bpdu老化时间Max Age不准确*/
			//msti_port->common_info.rcvdInfoWhile = 3*(cist_port->port_times.hello_time);
			if(6 == cist_port->port_times.max_age)
			{
			
				msti_port->common_info.rcvdInfoWhile = 15;
			}
			else 
			{
			
				msti_port->common_info.rcvdInfoWhile  = 20;
			
			}
        }
        else
        {
            msti_port->common_info.rcvdInfoWhile = 0;
			msti_port->common_info.infoIs = MSTP_INFO_AGED;
			msti_port->port_priority = msti_port->designated_priority;
			msti_port->port_times = msti_port->designated_times;
        }
    }
}


/*setTcFlags()
If the received BPDU is a TCN BPDU, sets rcvdTcn TRUE and sets rcvdTc TRUE for each and everyMSTI.*/
void mstp_set_tc_flags(struct mstp_port* port, unsigned int instance_id)
{

/*If the received BPDU is a TCN BPDU, sets rcvdTcn TRUE and sets rcvdTc TRUE 
for each and everyMSTI.			[this is done in mstp_recv_info() ]*/

	unsigned int				ins_id = 0; //instance_id

	struct mstp_cist_port		*cist_port = NULL;
	struct mstp_msti_port		*msti_port = NULL;
	struct mstp_port_variables	*port_variables = NULL;

	if (NULL == port)
	{
		return;
	}

	cist_port = &port->cist_port;	
	port_variables = &port->port_variables;

/*Otherwise, if the received BPDU is a ConfigBPDU or RST BPDU:*/
	if(0 == instance_id)
	{
		/*If the Topology Change Acknowledgment flag is set in the CST message, 
		sets rcvdTcAck TRUE.*/
		if(cist_port->bpdu_cfg.topology_change_ack)
		{
			port_variables->rcvdTcAck = MSTP_TRUE;
		}
		
#if 0		//need to open
		if (cist_port->bpdu_cfg.message_type == MSTP_BPDUTYPE_TCN)
		{
			port_variables->rcvdTcn = MSTP_TRUE;
		}
#endif

		/*If rcvdInternal is clear and the Topology Change flag is set in the CST message, 
		sets rcvdTc for the CIST and for each and every MSTI.*/
		if(!port_variables->rcvdInternal && cist_port->bpdu_cfg.topology_change)
		{
			/*cist*/
			cist_port->common_info.rcvdTc = MSTP_TRUE;

			/*msti*/
			for(ins_id = 0; ins_id < MSTP_INSTANCE_MAX; ins_id++)
			{
				msti_port = port->msti_port[ins_id];
				if (msti_port != NULL)
				{
					msti_port->common_info.rcvdTc = MSTP_TRUE;
				}
			}
		}

		/*If rcvdInternal is set, sets rcvdTc for the CIST if the Topology Change flag is set 
		in the CST message, */
		if((port_variables->rcvdInternal) && cist_port->bpdu_cfg.topology_change)
		{
			cist_port->common_info.rcvdTc = MSTP_TRUE;
		}
	}
	else
/*and sets rcvdTc for each MSTI for which the Topology Change flag
is set in the corresponding MSTI message.*/		
    {
		msti_port = port->msti_port[instance_id - 1];
        if(msti_port->bpdu_cfg.topology_change)
        {
            msti_port->common_info.rcvdTc = MSTP_TRUE;
        }
    }
}

/* These procedures have also been renamed relative to IEEE Std 802.1D, 1998 Edition; updtRoleDisabled-
Bridge to updtRoleDisabledTree, clearReselectBridge to clearReselectTree, updtRolesBridge to updtRolesCist and upd-
tRolesMsti, and setSelectedBridge to setSelectedTree.*/
void mstp_updt_role_disabled_tree(struct mstp_bridge *mstp_bridge, unsigned int instance_id)
{
	struct mstp_cist_port *cist_port = NULL, *next_cist_port = NULL;
	struct mstp_msti_port *msti_port = NULL, *next_msti_port = NULL;

	//MSTP_LOG_DBG("%s:  mstp_updt_role_disabled_tree() !\n", __func__);

	if (NULL == mstp_bridge)
	{
		return;
	}

    if(0 == instance_id)
    {
		/*cist port list*/
        list_for_each_entry_safe(cist_port, next_cist_port, &mstp_bridge->cist_br.port_head, port_list)
        {
            cist_port->common_info.selectedRole = MSTP_PORT_ROLE_DISABLED;
        }
    }
    else
    {
		/*msti port list*/
        list_for_each_entry_safe(msti_port, next_msti_port, &mstp_bridge->msti_br[instance_id-1]->port_head, port_list)
        {
            msti_port->common_info.selectedRole = MSTP_PORT_ROLE_DISABLED;
        }
    }
}

/*clearReselectTree()
Clears reselect for the tree (the CIST or a given MSTI) for all Ports of the Bridge.*/
void mstp_clear_reselect_tree(struct mstp_bridge *mstp_bridge, unsigned int instance_id)
{
    struct mstp_cist_port *cist_port = NULL, *next_cist_port = NULL;
	struct mstp_msti_port *msti_port = NULL, *next_msti_port = NULL;

	if (NULL == mstp_bridge)
	{
		return;
	}

    if(0 == instance_id)
    {    
		/*cist port list*/
        list_for_each_entry_safe(cist_port, next_cist_port, &mstp_bridge->cist_br.port_head, port_list)
        {
            cist_port->common_info.reselect = MSTP_FALSE;
        }
    }
    else
    {    
		/*msti port list*/
        list_for_each_entry_safe(msti_port, next_msti_port, &mstp_bridge->msti_br[instance_id-1]->port_head, port_list)
        {
            msti_port->common_info.reselect = MSTP_FALSE;
        }
    }
}

/*updtRolesTree();
This procedure calculates the following priority vectors (13.8, 13.9 for the CIST, 13.10 for a MSTI) 
and timer values, for the CIST or a given MSTI:*/
void mstp_updt_roles_tree(struct mstp_bridge *mstp_bridge, unsigned int instance_id)
{
	unsigned int				last_external_path_cost = 0;
	struct bridge_id			last_regional_root_id;

    struct mstp_cist_port		*cist_port = NULL, *next_cist_port = NULL;
    struct mstp_msti_port		*msti_port = NULL, *next_msti_port = NULL;
	struct mstp_cist_br			*cist_br = NULL;
	struct mstp_msti_br			*msti_br = NULL;
	struct mstp_port			*mport = NULL, *next_mport = NULL;
	struct mstp_common_port		*common_info = NULL;
	struct cist_priority_vector	cist_pri;
	struct cist_priority_vector	cist_pri_path;	/* Temporary CIST root path priority vector */
	struct msti_priority_vector msti_pri;
	struct msti_priority_vector msti_pri_path;	/* Temporary MSTI the root path priority vector */

	if (NULL == mstp_bridge)
	{
		return;
	}

	if(0 == instance_id)	/*cist*/
	{
		/* 浠庢ˉ浼樺厛绾у悜閲忓拰鎵�鏈夌殑鏍硅矾寰勪紭鍏堢骇鍚戦噺涓?
		閫夋嫨鏈�浼樼殑浣滀负鏍逛紭鍏堢骇鍚戦噺 */
		cist_br = &mstp_bridge->cist_br;
		cist_pri = cist_br->bridge_priority;
		cist_br->root_port = NULL;

		last_regional_root_id = cist_br->root_priority.regional_root_id;
		last_external_path_cost = cist_br->root_priority.external_root_path_cost;

		list_for_each_entry_safe(cist_port, next_cist_port, &cist_br->port_head, port_list)
		{
			common_info = &cist_port->common_info;

			/*a) The root path priority vector for each Bridge Port that is not Disabled and has a port priority vector
			(portPriority plus portId鈥攕ee 13.25.33 and 13.25.32) that has been recorded from a received message
			and not aged out (infoIs == Received).*/
			if(MSTP_INFO_RECEIVED == common_info->infoIs)
			{
				cist_pri_path = cist_port->port_priority;

				/*msg come from internal mst */
				if(cist_port->mport->port_variables.rcvdInternal)
				{
					cist_pri_path.internal_root_path_cost += common_info->port_cost;
				}
				else	/*msg come from external mst */
				{
					cist_pri_path.external_root_path_cost += common_info->port_cost;
					/*confirm*/
					cist_pri_path.regional_root_id = cist_br->bridge_priority.designated_bridge_id;
					cist_pri_path.internal_root_path_cost = 0;
				}


				/*b) The Bridge鈥檚 root priority vector (rootPortId, rootPriority鈥?3.24.7, 13.24.8), chosen as the best of
				the set of priority vectors comprising the bridge鈥檚 own  bridge priority vector (BridgePriority鈥?
				13.24.2) plus all calculated root path priority vectors whose:
				1) DesignatedBridgeID Bridge Address component is not equal to that component of the bridge鈥檚
				own bridge priority vector (13.9) and,
				2) Port鈥檚 restrictedRole parameter is FALSE.*/

				/*when msg recv is the same bridge, can not run this case*/
				if((MSTP_FALSE == mstp_is_same_br(cist_pri_path.designated_bridge_id))
//					&& (cist_port->mport->port_variables.restrictedRole == MSTP_FALSE)
					&& (INFO_BETTER == mstp_cist_info_cmp(&cist_pri, &cist_pri_path)))
				{
					cist_pri = cist_pri_path;
					cist_br->root_port = cist_port;
				}
			}
		}

		cist_br->root_priority = cist_pri;


		/*c) The bridge`s root times, (rootTimes鈥?3.24.9), set equal to:
		1) BridgeTimes (13.24.3), if the chosen root priority vector is the bridge priority vector; otherwise,
		2) portTimes (13.25.34) for the port associated with the  selected root priority vector, with the
		Message Age component incremented by 1 second and rounded to the nearest whole second if
		the information was received from a bridge external to the MST Region (rcvdInternal FALSE),
		and with remainingHops decremented by one if the information was received from a bridge
		internal to the MST Region (rcvdInternal TRUE).*/

		if(NULL == cist_br->root_port)
		{
			cist_br->root_times = cist_br->bridge_times;
		}
		else
		{
			cist_br->root_times = cist_br->root_port->port_times;
			/*same mst region*/
			if(cist_br->root_port->mport->port_variables.rcvdInternal)
			{
				cist_br->root_times.remaining_hops--;
			}
			else/*out mst region*/
			{
				cist_br->root_times.msg_age++;
			}
		}

		if((MSTP_FALSE == mstp_same_root(cist_br->root_priority.regional_root_id, last_regional_root_id))
			|| (last_external_path_cost != 0)
			|| (cist_br->root_priority.external_root_path_cost != 0))
		{
			mstp_sync_master(mstp_bridge);
		}
	}
	else
	{
		/*The root priority vector calculation MSTI bridge*/
		msti_br = mstp_bridge->msti_br[instance_id-1];
		msti_pri = msti_br->bridge_priority;
		msti_br->root_port = NULL;

		list_for_each_entry_safe(msti_port, next_msti_port, &msti_br->port_head, port_list)
		{
			common_info = &msti_port->common_info;
			if(MSTP_INFO_RECEIVED == common_info->infoIs)
			{
				msti_pri_path = msti_port->port_priority;
				if(msti_port->mport->port_variables.rcvdInternal) /*come from internal mst*/
				{
					msti_pri_path.internal_root_path_cost += common_info->port_cost;
				}
				else /*come from externel mst*/
				{
					msti_pri_path.internal_root_path_cost = 0;
				}

				if((MSTP_FALSE == mstp_is_same_br(msti_pri_path.designated_bridge_id))
					&& (INFO_BETTER == mstp_msti_info_cmp(&msti_pri, &msti_pri_path)))
				{
					msti_pri = msti_pri_path;
					msti_br->root_port = msti_port;
				}
			}
		}

		msti_br->root_priority = msti_pri;

		if(NULL == msti_br->root_port)
		{
			msti_br->root_times = msti_br->bridge_times;
		}
		else
		{
			msti_br->root_times = msti_br->root_port->port_times;
			if(msti_br->root_port->mport->port_variables.rcvdInternal)
			{
				msti_br->root_times.remaining_hops--;
			}
		}
	}


	/*According to the value and priority vector relationship infoIs variable 
	for bridge in this instance all port selection role of the tree*/
    list_for_each_entry_safe(mport, next_mport, &mstp_bridge->port_head, port_list)
	{
		if(0 == instance_id)
		{
			cist_br = &mstp_bridge->cist_br;
			cist_port = &mport->cist_port;
			common_info = &cist_port->common_info;
			cist_port->designated_priority.root_id = cist_br->root_priority.root_id;
			cist_port->designated_priority.external_root_path_cost = cist_br->root_priority.external_root_path_cost;
			cist_port->designated_priority.regional_root_id = cist_br->root_priority.regional_root_id;
			cist_port->designated_priority.internal_root_path_cost = cist_br->root_priority.internal_root_path_cost;
			cist_port->designated_priority.designated_bridge_id = cist_br->bridge_priority.designated_bridge_id;

			cist_port->designated_priority.designated_port_id= mstp_port_map(cist_port->port_index, mport->port_pri);
			cist_port->designated_priority.recv_port_id = mstp_port_map(cist_port->port_index, mport->port_pri);

			cist_port->designated_times = cist_br->root_times;
		}
		else
		{
			msti_br = mstp_bridge->msti_br[instance_id-1];
			msti_port = mport->msti_port[instance_id-1];
			if(NULL == msti_port || NULL == msti_br)
			{
				continue;
			}

			common_info = &msti_port->common_info;
			msti_port->designated_priority.rg_root_id = msti_br->root_priority.rg_root_id;
			msti_port->designated_priority.internal_root_path_cost = msti_br->root_priority.internal_root_path_cost;

			msti_port->designated_priority.designated_bridge_id = msti_br->bridge_priority.designated_bridge_id;

			/*need to modify */	//FIXME
			msti_port->designated_priority.designated_port_id = mstp_port_map(msti_port->port_index, mport->msti_port_pri[instance_id-1]);
			msti_port->designated_priority.recv_port_id = mstp_port_map(msti_port->port_index, mport->msti_port_pri[instance_id-1]);

			msti_port->designated_times = msti_br->root_times;
		}

		if(MSTP_INFO_DISABLED == common_info->infoIs)
		{
			common_info->selectedRole = MSTP_PORT_ROLE_DISABLED;
		}
		else if((common_info->infoIs != MSTP_INFO_DISABLED)
			&& (instance_id != 0)
			&& (MSTP_INFO_RECEIVED == mport->cist_port.common_info.infoIs)
			&& (MSTP_FALSE == mport->port_variables.infoInternal))
		{
			/*Only use in MSTI instance, the purpose is to choose the domain edge port role,
			including the master port and Alternate port*/
			if(MSTP_PORT_ROLE_ROOT == mport->cist_port.common_info.selectedRole)
			{
				common_info->selectedRole = MSTP_PORT_ROLE_MASTER;
				if((mstp_msti_info_cmp(&msti_port->port_priority, &msti_port->designated_priority) != INFO_SAME)
					|| ((msti_br->root_port != NULL)
						&& (MSTP_FALSE == mstp_msti_times_cmp(&msti_port->port_times, &msti_br->root_port->port_times))))
				{
					common_info->updtInfo = MSTP_TRUE;
				}
			}
			else if(MSTP_PORT_ROLE_ALTERNATE == mport->cist_port.common_info.selectedRole)
			{
				common_info->selectedRole = MSTP_PORT_ROLE_ALTERNATE;
				if((mstp_msti_info_cmp(&msti_port->port_priority, &msti_port->designated_priority) != INFO_SAME)
					|| ((msti_br->root_port != NULL)
						&& (MSTP_FALSE == mstp_msti_times_cmp(&msti_port->port_times, &msti_br->root_port->port_times))))
				{
					common_info->updtInfo = MSTP_TRUE;
				}
			}
		}
		else if((common_info->infoIs != MSTP_INFO_DISABLED)
			&& ((0 == instance_id)
				|| ((instance_id != 0) 
					&& ((common_info->infoIs != MSTP_INFO_RECEIVED)
						|| (MSTP_TRUE == mport->port_variables.infoInternal)))))
		{
			/*cist and all msti do run here*/
			switch(common_info->infoIs)
			{/*bug#52418 bug#52419*/
			/*<stp>非根桥转发了Message比Max Age大的配置bpdu*/
			/*<stp>非根桥收到Message等于Max Age或比Max Age小1的配置bpdu时，根桥角色反复切换*/
				case MSTP_INFO_AGED:
				{
					common_info->updtInfo = MSTP_TRUE;
					common_info->selectedRole = MSTP_PORT_ROLE_DESIGNATED;
				}
					break;
				case MSTP_INFO_MINE:
				{
					common_info->selectedRole = MSTP_PORT_ROLE_DESIGNATED;
					if(0 == instance_id)
					{
		                if((mstp_cist_info_cmp(&cist_port->designated_priority, &cist_port->port_priority) != INFO_SAME)
							|| ((cist_br->root_port != NULL)
								&& (MSTP_FALSE == mstp_cist_times_cmp(&cist_port->port_times, &cist_br->root_port->port_times))))
		                {
		                    common_info->updtInfo = MSTP_TRUE;
		                }
					}
					else
					{
						if((mstp_msti_info_cmp(&msti_port->designated_priority, &msti_port->port_priority) != INFO_SAME)
							|| ((msti_br->root_port != NULL) 
								&& (MSTP_FALSE == mstp_msti_times_cmp(&msti_port->port_times, &msti_br->root_port->port_times))))
		                {
		                    common_info->updtInfo = MSTP_TRUE;
		                }
					}
				}
					break;
				case MSTP_INFO_RECEIVED:
				{
					if(0 == instance_id)
					{
						if(cist_br->root_port == cist_port)
		                {
		                	if(MSTP_PORT_ROLE_DESIGNATED == common_info->selectedRole)
		                	{
								
								common_info->learn  = common_info->forward = MSTP_FALSE;			
							}
		                    common_info->selectedRole = MSTP_PORT_ROLE_ROOT;
		                    common_info->updtInfo = MSTP_FALSE;
		                }
		                else
		                {
		                    if(INFO_WORSE == mstp_cist_info_cmp(&cist_port->designated_priority, &cist_port->port_priority))
		                    {
		                        common_info->selectedRole = MSTP_PORT_ROLE_DESIGNATED;
		                        common_info->updtInfo = MSTP_TRUE;
		                    }
		                    else
		                    {
								if(MSTP_TRUE == mstp_is_same_br(cist_port->port_priority.designated_bridge_id))
		                        {
		                            common_info->selectedRole = MSTP_PORT_ROLE_BACKUP;
		                            common_info->updtInfo = MSTP_FALSE;
		                        }
		                        else
		                        {
		                            common_info->selectedRole = MSTP_PORT_ROLE_ALTERNATE;
		                            common_info->updtInfo = MSTP_FALSE;
								}
							}
						}
					}
					else
					{
						if(msti_br->root_port == msti_port)
		                {
		                    common_info->selectedRole = MSTP_PORT_ROLE_ROOT;
		                    common_info->updtInfo = MSTP_FALSE;
		                }
		                else
		                {
		                    if(INFO_WORSE == mstp_msti_info_cmp(&msti_port->designated_priority, &msti_port->port_priority))
		                    {
		                        common_info->selectedRole = MSTP_PORT_ROLE_DESIGNATED;
		                        common_info->updtInfo = MSTP_TRUE;
		                    }
		                    else
		                    {
		                        if(MSTP_TRUE == mstp_is_same_br(msti_port->port_priority.designated_bridge_id))
		                        {
		                            common_info->selectedRole = MSTP_PORT_ROLE_BACKUP;
		                            common_info->updtInfo = MSTP_FALSE;
		                        }
		                        else
		                        {
		                            common_info->selectedRole = MSTP_PORT_ROLE_ALTERNATE;
		                            common_info->updtInfo = MSTP_FALSE;
	                        	}
	                    	}
	                	}
					}
				}
					break;
				default:
					break;
			}
		}
    }
}


/*13.27.21 setSelectedTree()
Sets selected TRUE for this tree (the CIST or a given MSTI) for all ports of the bridge if reselect is FALSE
for all ports in this tree. If reselect is TRUE for any port in this tree, this procedure takes no action.*/
void mstp_set_selected_tree(struct mstp_bridge *mstp_bridge, unsigned int instance_id)
{
	struct mstp_cist_port *cist_port = NULL, *next_cist_port = NULL;
	struct mstp_msti_port *msti_port = NULL, *next_mist_port = NULL;

	if (NULL == mstp_bridge)
	{
		return;
	}

    if(0 == instance_id)
    {
		/*cist port list*/
        list_for_each_entry_safe(cist_port, next_cist_port, &mstp_bridge->cist_br.port_head, port_list)
        {
            cist_port->common_info.selected = MSTP_TRUE;
        }
    }
    else
    {
		/*msti port list*/
        list_for_each_entry_safe(msti_port, next_mist_port, &mstp_bridge->msti_br[instance_id-1]->port_head, port_list)
        {
            msti_port->common_info.selected = MSTP_TRUE;
        }
    }
}

/*13.27.22 setSyncTree()
Sets sync TRUE for this tree (the CIST or a given MSTI) for all ports of the bridge.*/
void mstp_set_sync_tree(struct mstp_port* port, unsigned int instance_id)
{
	struct mstp_cist_port	*cist_port = NULL, *next_cist_port = NULL;
	struct mstp_msti_port	*msti_port = NULL, *next_mist_port = NULL;
	struct mstp_bridge		*m_br = port->m_br;

	if (NULL == port)
	{
		return;
	}

    if(0 == instance_id)
    {
		/*cist port list*/
        list_for_each_entry_safe(cist_port, next_cist_port, &m_br->cist_br.port_head, port_list)
        {
            cist_port->common_info.sync = MSTP_TRUE;
        }
    }
    else
    {    
		/*msti port list*/
        list_for_each_entry_safe(msti_port, next_mist_port, &m_br->msti_br[instance_id-1]->port_head, port_list)
        {
            msti_port->common_info.sync = MSTP_TRUE;
        }
    }
}

/*13.27.20 setReRootTree()
Sets reRoot TRUE for this tree (the CIST or a given MSTI) for all ports of the bridge.*/
void mstp_set_reroot_tree(struct mstp_port* port, unsigned int instance_id)
{
	struct mstp_cist_port 		*cist_port = NULL, *next_cist_port = NULL;
	struct mstp_msti_port 		*msti_port = NULL, *next_mist_port = NULL;
	struct mstp_bridge			*m_br = NULL;

	if (NULL == port)
	{
		return;
	}

	m_br = port->m_br;
	if (NULL == m_br)
	{
		return;
	}

    if(0 == instance_id)
    {
    	/*cist port list*/
        list_for_each_entry_safe(cist_port, next_cist_port, &m_br->cist_br.port_head, port_list)
        {
            cist_port->common_info.reRoot = MSTP_TRUE;
        }
    }
    else
    {    
		/*msti port list*/
        list_for_each_entry_safe(msti_port, next_mist_port, &m_br->msti_br[instance_id-1]->port_head, port_list)
        {
            msti_port->common_info.reRoot = MSTP_TRUE;
        }
    }
}

void mstp_add_stg_vlan(unsigned int instance_id, struct mstp_port* port, port_forword_state_in_instance state)
{
	int 					vlan_id = 0;
	int 					ret = -1;
	unsigned int			subtype = MSTP_STG_ADD;
	struct mstp_ipc_msg		ipc_msg;

	if (NULL == port)
	{
		return;
	}
	
	
	memset((void *)&ipc_msg, 0x00, sizeof(struct mstp_ipc_msg));


	/*get port index*/
	ipc_msg.port_index = port->ifindex;


	/*get instance id
	**vlan map*/
	if(0 == instance_id)
	{
		if(PROTO_VERID_MSTP == mstp_global_bridge->stp_mode)
		{
			ipc_msg.instance_index = 1;
			ipc_msg.vlan_map = bridge_cist_vlan_map;
		}
		/*bridge stp mode is STP/RSTP, cist port set vlan range <1-4094>*/
		else	
		{
			ipc_msg.instance_index = 1;
			
			/* 4096 = 512 * 8; VLAN ID is 0-4095 bit*/
			/*map [1-4094] vlan id, (vlan id 4095 is out of control)*/
			for(vlan_id = 0; vlan_id < MSTP_VLAN_MAP_SIZE-1; vlan_id++)
			{
				if(0 == vlan_id)
				{
					ipc_msg.vlan_map.vlan_map[vlan_id] = 0xFE;
				}
				else
				{
					ipc_msg.vlan_map.vlan_map[vlan_id] = 0xFF;
				}
			}
			ipc_msg.vlan_map.vlan_map[vlan_id] = 0x7F;
		}
	}
	else
	{
		ipc_msg.instance_index = instance_id + 1;
		
		ipc_msg.vlan_map = global_ins_vlan_map[instance_id-1];
	
	}
	
	/*port state is blocking*/
//	ipc_msg.port_state = state;
	switch(state)
	{
		case MSTP_STATE_DISCARDING:
		{
			ipc_msg.port_state = PORT_STATE_BLOCKING;
		}
			break;
		case MSTP_STATE_BLOCKING:
		{			
			ipc_msg.port_state = PORT_STATE_BLOCKING;
		}
			break;
		case MSTP_STATE_LEARNING:
		{			
			ipc_msg.port_state = PORT_STATE_LEARNING;
		}
			break;
		case MSTP_STATE_FORWARDING:
		{
			ipc_msg.port_state = PORT_STATE_FORWARDING;
		}
			break;
		case MSTP_STATE_REMOVE:
		{
			ipc_msg.port_state = PORT_STATE_REMOVE;
		}
			break;			
			
		default:
			break;
	}

	//printf("%s[%d]###############index = %x\n",__FUNCTION__,__LINE__,port->ifindex);
	ret = ipc_send_msg_n2(&ipc_msg, sizeof(struct mstp_ipc_msg), 1, MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_MSTP, subtype, IPC_OPCODE_ADD, port->ifindex);
    if(ret != ERRNO_SUCCESS)
    {
        MSTP_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
        return;
    }


	return;
}


void mstp_set_port_state(struct mstp_port* port, unsigned int instance_id, port_forword_state_in_instance state)
{
	struct mstp_port_status status[] =
	{
		{PORT_STATE_DISABLE,		"PORT_STATE_DISABLE"},
		{PORT_STATE_BLOCKING,		"PORT_STATE_BLOCKING"},
		{PORT_STATE_LISTEN,			"PORT_STATE_LISTEN"},
		{PORT_STATE_LEARNING,		"PORT_STATE_LEARNING"},
		{PORT_STATE_FORWARDING,		"PORT_STATE_FORWARDING"},
		{PORT_STATE_REMOVE,			"PORT_STATE_REMOVE"},
	};

	int 					ret = -1;
	unsigned int			subtype = MSTP_STG_SET;
	struct mstp_ipc_msg		ipc_msg;

	if (NULL == port)
	{
		return;
	}
	
	memset((void *)&ipc_msg, 0x00, sizeof(struct mstp_ipc_msg));
	
	ipc_msg.port_index = port->ifindex;

	/*get instance id*/
	if(0 == instance_id)
	{
		ipc_msg.instance_index = 1;
	}
	else
	{
		ipc_msg.instance_index = instance_id + 1;
	}

	ipc_msg.port_state = PORT_STATE_BLOCKING;

	switch(state)
	{
		case MSTP_STATE_DISCARDING:
		{
			ipc_msg.port_state = PORT_STATE_BLOCKING;
		}
			break;
		case MSTP_STATE_BLOCKING:
		{			
			ipc_msg.port_state = PORT_STATE_BLOCKING;
		}
			break;
		case MSTP_STATE_LEARNING:
		{			
			ipc_msg.port_state = PORT_STATE_LEARNING;
		}
			break;
		case MSTP_STATE_FORWARDING:
		{
			ipc_msg.port_state = PORT_STATE_FORWARDING;
		}
			break;
		default:
			break;
	}

	MSTP_LOG_DBG("%s:  port(%s), instance_id = %u, state(%s)!\n",
		__func__, port->if_name, instance_id, status[ipc_msg.port_state].port_status_str);

	//ret = ipc_send_hal(&ipc_msg, sizeof(struct mstp_ipc_msg), 1, MODULE_ID_HAL, MODULE_ID_L2,
	//			IPC_TYPE_MSTP, subtype, IPC_OPCODE_ADD, port->ifindex);
	
	ret = ipc_send_msg_n2(&ipc_msg, sizeof(struct mstp_ipc_msg), 1, MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_MSTP, subtype, IPC_OPCODE_ADD, port->ifindex);
	//printf("%s[%d]###############index = %x\n",__FUNCTION__,__LINE__,port->ifindex);
    if(ret != ERRNO_SUCCESS)
    {
        MSTP_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

	return;
}


/*13.27.5 disableLearning()
An implementation dependent procedure that causes the Learning Process (8.7) to stop learning from the
source address of frames received on the port. The procedure does not complete until learning has stopped.
*/
void mstp_disable_learning(struct mstp_port* port, unsigned int instance_id)
{
	if (NULL == port)
	{
		return;
	}

	MSTP_LOG_DBG("%s: mstp_disable_learning() port(%s), instance_id = %u!\n", __func__, port->if_name, instance_id);
	
	/*call driver interface, set corresponding ports state */
	mstp_set_port_state(port, instance_id, MSTP_STATE_DISCARDING);
	
	//STG_MODIFY
}

/*13.27.4 disableForwarding()
An implementation dependent procedure that causes the Forwarding Process (8.6) to stop forwarding frames
through the port. The procedure does not complete until forwarding has stopped.*/
void mstp_disable_forwarding(struct mstp_port* port, unsigned int instance_id)
{
	if (NULL == port)
	{
		return;
	}

	MSTP_LOG_DBG("%s:  mstp_disable_forwarding() port(%s), instance_id = %u!\n", __func__, port->if_name, instance_id);

	/*call driver interface, set corresponding ports state */
	mstp_set_port_state(port, instance_id, MSTP_STATE_DISCARDING);
	
	//STG_MODIFY
}

/*13.27.7 enableLearning()
An implementation dependent procedure that causes the Learning Process (8.7) to start learning from frames
received on the port. The procedure does not complete until learning has been enabled.*/
void mstp_enable_learning(struct mstp_port* port, unsigned int instance_id)
{
	if (NULL == port)
	{
		return;
	}

	MSTP_LOG_DBG("%s:  mstp_enable_learning() port(%s), instance_id = %u!\n", __func__, port->if_name, instance_id);

	/*call driver interface, set corresponding ports state */
	mstp_set_port_state(port, instance_id, MSTP_STATE_LEARNING);
	
	//STG_MODIFY
}


/*13.27.6 enableForwarding()
An implementation dependent procedure that causes the Forwarding Process (8.6) to start forwarding frames
through the port. The procedure does not complete until forwarding has been enabled.*/
void mstp_enable_forwarding(struct mstp_port* port, unsigned int instance_id)
{
	if (NULL == port)
	{
		return;
	}

	MSTP_LOG_DBG("%s:  mstp_enable_forwarding() port(%s), instance_id = %u!\n", __func__, port->if_name, instance_id);

	/*call driver interface, set corresponding ports state */
	mstp_set_port_state(port, instance_id, MSTP_STATE_FORWARDING);
	
	//STG_MODIFY
}


/*
The following procedures perform the functions specified in 17.17 of IEEE Std 802.1D, 
1998 Edition for the CIST or any given MSTI instance:
f) flush()


1.Automatic learning of dynamic filtering information for unicast destination addresses through obser-
vation of source addresses of frames, together with the ageing out or flushing of that information to
support the movement of end stations and changes in active topology;
2.Ageing out or flushing of dynamic filtering information that has been learned
3.Temporary cuts in the active topology, introduced to ensure that rapid Port State transitions to Forwarding do
not cause loops, and do not therefore cause Filtering Database entries to be flushed throughout the network,
unless they are accompanied by Port Role changes
4.On receipt of a CIST TCN Message from a Bridge Port not internal to the Region, or on a change in Port
Role for a Bridge Port not internal to the Region, TCN Messages are transmitted through each of the other
Ports of the receiving Bridge for each MSTI, and the Filtering Databases for those ports are flushed
*/
void mstp_flush(uint32_t instance_id,struct mstp_port* port, unsigned int age_time)
	{
		unsigned char		is_trunk_port = 0;
		char instance_vlan[MSTP_VLAN_MAP_SIZE] = {0};
		uint32_t i = 0;
		
		
		if(NULL == port)
		{
			zlog_err("%s[%d]err:port is NULL",__FUNCTION__,__LINE__);
			return ;
		}
		if(0 == age_time)
		{
			/*flush mac base*/
			if(is_trunk_port)
			{
				/*deal*/
			}
			else
			{	/*mstp clear mac*/
				if(instance_id)
				{		
					for(i =0;i<MSTP_VLAN_MAP_SIZE;i++)
					{
						 instance_vlan[i] = global_ins_vlan_map[instance_id-1].vlan_map[i];
					}
					 l2if_clear_mac_by_port_vlan(instance_vlan,MSTP_TRUE,port->ifindex);
				}
				/*flush*/
				else
				{
					 l2if_clear_mac_by_port_vlan(NULL,MSTP_FALSE,port->ifindex);
					
				}
				
			}
		}
		else
		{
			port->port_variables.ageingTime = age_time;
		}
	
		return;
	}


/*13.27.10 newTcWhile()
If the value of tcWhile is zero and sendRSTP is TRUE, this procedure sets the value of tcWhile to HelloTime
plus one second and sets either newInfo TRUE for the CIST or newInfoMsti TRUE for a given MSTI. The
value of HelloTime is taken from the CIST鈥檚 portTimes parameter (13.25.34) for this port.
If the value of tcWhile is zero and sendRSTP is FALSE, this procedure sets the value of tcWhile to the sum
of the Max Age and Forward Delay components of  rootTimes and does not change the value of either
newInfo or newInfoMsti.Otherwise the procedure takes no action. 
*/
void mstp_new_tc_while(struct mstp_port* port, unsigned int instance_id)
{
	struct mstp_msti_port* msti_port = NULL;
	struct mstp_cist_port* cist_port = NULL;

	if (NULL == port)
	{
		return;
	}

	cist_port = &port->cist_port;

    if(0 == instance_id)
    {
    	/*This procedure sets the value of tcWhile, if and only if it is currently zero, 
		to twice HelloTime on point-to-point links (i.e., links where the operPointToPointMAC
		parameter is TRUE; see 6.4.3) where the partner bridge port is RSTP capable, and to 
		the sum of the Max Age and Forward Delay components of rootTimes otherwise
		 (non-RSTP capable partners or shared media)*/
        if((0 == cist_port->common_info.tcWhile) && (port->port_variables.sendRSTP))
        {
            cist_port->common_info.tcWhile = cist_port->port_times.hello_time + 1;
            cist_port->new_info = MSTP_TRUE;
        }

        if((0 == cist_port->common_info.tcWhile) && (!port->port_variables.sendRSTP))
        {
            cist_port->common_info.tcWhile = cist_port->cist_bridge->root_times.max_age
											+ cist_port->cist_bridge->root_times.forward_delay;
        }
    }
	else
	{
		msti_port = port->msti_port[instance_id-1];
        if((0 == msti_port->common_info.tcWhile) && (port->port_variables.sendRSTP))
        {
            msti_port->common_info.tcWhile = cist_port->port_times.hello_time + 1;
            port->port_variables.newInfoMsti = MSTP_TRUE;
        }

		if((0 == msti_port->common_info.tcWhile) && (!port->port_variables.sendRSTP))
        {
            msti_port->common_info.tcWhile = cist_port->cist_bridge->root_times.max_age 
											+ cist_port->cist_bridge->root_times.forward_delay;
        }
	}
}

#if 1
/*13.27.24 setTcPropTree()
If and only if restrictedTcn is FALSE for the port that invoked the procedure, sets tcProp TRUE for the given
tree (the CIST or a given MSTI) for all other ports.*/
void mstp_set_tc_prop_tree(struct mstp_port* port, unsigned int instance_id)
{
	struct mstp_cist_port 	*cist_port = NULL, *next_cist_port = NULL;
	struct mstp_msti_port 	*msti_port = NULL, *next_msti_port = NULL;
	struct mstp_bridge		*m_br = NULL;

	if (NULL == port)
	{
		return;
	}

	m_br = port->m_br;
	if (NULL == m_br)
	{
		return;
	}

    if(0 == instance_id)
    {
        if(!port->port_variables.restrictedTcn)
        {        
    		/*cist port list*/
            list_for_each_entry_safe(cist_port, next_cist_port, &m_br->cist_br.port_head, port_list)
            {
				if (cist_port->mport != port)
				{
                    cist_port->common_info.tcProp = MSTP_TRUE;
				}
            }
        }
    }
	else
    {
        if(!port->port_variables.restrictedTcn)
        {
			/*msti port list*/
            list_for_each_entry_safe(msti_port, next_msti_port, &m_br->msti_br[instance_id-1]->port_head, port_list)
            {
				if (msti_port->mport != port)
				{
					msti_port->common_info.tcProp = MSTP_TRUE;
				}   
            }
        }
    }
}

#else
/*
setTcPropTree()
Sets tcProp TRUE for the given tree (the CIST or anMSTI) for all Ports except the Port that invoked the pro-
cedure.
*/
void mstp_set_tc_prop_bridge(struct mstp_port* port, unsigned int instance_id)
{

}
#endif

/*Returns TRUE if the received CIST priority vector is better than or the same as 
(13.10) the CIST port priority vector.
para 1:port priority vector, para2: recv msg priority vector
*/
int mstp_cist_info_cmp(struct cist_priority_vector *pv, struct cist_priority_vector *cpv)
{

/*need to confirm the function whether is right or not*/

/*鏍逛氦鎹㈣澶嘔D
澶栭儴璺緞寮�閿�
鍩熸牴ID
鍐呴儴璺緞寮�閿�
鎸囧畾浜ゆ崲璁惧ID
鎸囧畾绔彛ID
鎺ユ敹绔彛ID 
*/
/*鍚戦噺鐨勪紭鍏堢骇浠庝笂鍒颁笅渚濇閫掑噺*/
/*	struct bridge_id		root_id;
	unsigned int			external_root_path_cost;
	struct bridge_id		regional_root_id;
	unsigned int			internal_root_path_cost;
	struct bridge_id		designated_bridge_id;
	unsigned short		designated_port_id;	
	unsigned short		recv_port_id;
*/


/*compare root_id: pri*/

if(memcmp(cpv->root_id.pri, pv->root_id.pri, 2)&& !memcmp(cpv->root_id.mac_addr, pv->root_id.mac_addr, MAC_LEN))
	{
		mstp_reinit();
	}
	if(memcmp(cpv->root_id.pri, pv->root_id.pri, 2) < 0)
	{
		MSTP_LOG_DBG("%s: mstp_cist_info_cmp()	root_id.pri		INFO_BETTER!\n", __func__);
	
		return INFO_BETTER;
	}
	else if(memcmp(cpv->root_id.pri, pv->root_id.pri, 2) > 0)
	{
		return INFO_WORSE;
	}
/*compare root_id: mac addr*/
	if(memcmp(cpv->root_id.mac_addr, pv->root_id.mac_addr, MAC_LEN) < 0)
	{
		MSTP_LOG_DBG("%s:mstp_cist_info_cmp()	root_id.mac_addr		INFO_BETTER!\n", __func__);

		return INFO_BETTER;
	}
	else if(memcmp(cpv->root_id.mac_addr, pv->root_id.mac_addr, MAC_LEN) > 0)
	{
		return INFO_WORSE;
	}

/*compare external_root_path_cost*/
    if(cpv->external_root_path_cost < pv->external_root_path_cost)
	{
		MSTP_LOG_DBG("%s:mstp_cist_info_cmp()	external_root_path_cost		INFO_BETTER!\n", __func__);

		return INFO_BETTER;
	}
    else if(cpv->external_root_path_cost > pv->external_root_path_cost)
    {
		return INFO_WORSE;
	}

/*compare regional_root_id:pri*/
	if(memcmp(cpv->regional_root_id.pri, pv->regional_root_id.pri, 2) < 0)
	{
		MSTP_LOG_DBG("%s: mstp_cist_info_cmp()	regional_root_id.pri		INFO_BETTER!\n", __func__);
	
		return INFO_BETTER;
	}
	else if(memcmp(cpv->regional_root_id.pri, pv->regional_root_id.pri, 2) > 0)
	{
		return INFO_WORSE;
	}

/*compare regional_root_id:mac_addr*/
	if(memcmp(cpv->regional_root_id.mac_addr, pv->regional_root_id.mac_addr, MAC_LEN) < 0)
	{
		MSTP_LOG_DBG("%s:mstp_cist_info_cmp() regional_root_id.mac_addr		INFO_BETTER!\n", __func__);

		return INFO_BETTER;
	}
	else if(memcmp(cpv->regional_root_id.mac_addr, pv->regional_root_id.mac_addr, MAC_LEN) > 0)
	{
		return INFO_WORSE;
	}

/*compare internal_root_path_cost*/	
    if(cpv->internal_root_path_cost < pv->internal_root_path_cost)
	{
		MSTP_LOG_DBG("%s:mstp_cist_info_cmp()	internal_root_path_cost		INFO_BETTER!\n", __func__);

		return INFO_BETTER;
	}
    else if(cpv->internal_root_path_cost > pv->internal_root_path_cost)
    {
		return INFO_WORSE;
	}
	
/*compare designated_bridge_id: pri*/
	if(memcmp(cpv->designated_bridge_id.pri, pv->designated_bridge_id.pri, 2) < 0)
	{
		MSTP_LOG_DBG("%s:mstp_cist_info_cmp()	designated_bridge_id.mac_addr		INFO_BETTER!\n", __func__);
	
		return INFO_BETTER;
	}
	else if(memcmp(cpv->designated_bridge_id.pri, pv->designated_bridge_id.pri, 2) > 0)
	{
		return INFO_WORSE;
	}
	
/*compare designated_bridge_id: mac addr*/
	if(memcmp(cpv->designated_bridge_id.mac_addr, pv->designated_bridge_id.mac_addr, MAC_LEN) < 0)
	{
		MSTP_LOG_DBG("%s:mstp_cist_info_cmp()	designated_bridge_id.mac_addr		INFO_BETTER!\n", __func__);

		return INFO_BETTER;
	}
	else if(memcmp(cpv->designated_bridge_id.mac_addr, pv->designated_bridge_id.mac_addr, MAC_LEN) > 0)
	{
		return INFO_WORSE;
	}


/*compare designated_port_id*/	
/*need to modify */	//FIXME
    if(cpv->designated_port_id < pv->designated_port_id)
	{
		MSTP_LOG_DBG("%s:mstp_cist_info_cmp()	designated_port_id		INFO_BETTER!\n", __func__);
	
		return INFO_BETTER;
    }
	else if(cpv->designated_port_id > pv->designated_port_id)
	{
		return INFO_WORSE;
	}

/*compare recv_port_id*/
    if(cpv->recv_port_id < pv->recv_port_id)
	{
		MSTP_LOG_DBG("%s:mstp_cist_info_cmp()	recv_port_id		INFO_BETTER!\n", __func__);
	
		return INFO_BETTER;
    }
    else if(cpv->recv_port_id > pv->recv_port_id)
    {
		return INFO_WORSE;
    }

	return INFO_SAME;
}

/*Returns TRUE if the MSTI priority vector is better than or the same as 
(13.11) the MSTI port priority vector.
para 1: MSTI port priority vector, para2:MSTI priority vector,*/
int mstp_msti_info_cmp(struct msti_priority_vector *pv, struct msti_priority_vector *cpv)
{
/*need to confirm the function whether is right or not*/

/*鍩熸牴ID
鍐呴儴璺緞寮�閿�
鎸囧畾浜ゆ崲璁惧ID
鎸囧畾绔彛ID
鎺ユ敹绔彛ID
*/
/*鍚戦噺鐨勪紭鍏堢骇浠庝笂鍒颁笅渚濇閫掑噺*/

/*	struct bridge_id		rg_root_id;
	unsigned int			internal_root_path_cost;	
	struct bridge_id		designated_bridge_id;
	unsigned short		designated_port_id;		
	unsigned short		recv_port_id;
*/

/*compare rg_root_id: pri*/
	if(memcmp(cpv->rg_root_id.pri, pv->rg_root_id.pri, 2) < 0)
	{
		return INFO_BETTER;
	}
	else if(memcmp(cpv->rg_root_id.pri, pv->rg_root_id.pri, 2) > 0)
	{
		return INFO_WORSE;
	}
/*compare rg_root_id: mac addr*/
	if(memcmp(cpv->rg_root_id.mac_addr, pv->rg_root_id.mac_addr, MAC_LEN) < 0)
	{
		return INFO_BETTER;
	}
	else if(memcmp(cpv->rg_root_id.mac_addr, pv->rg_root_id.mac_addr, MAC_LEN) > 0)
	{
		return INFO_WORSE;
	}

/*compare internal_root_path_cost*/
    if(cpv->internal_root_path_cost < pv->internal_root_path_cost)
	{
		return INFO_BETTER;
	}
	else if(cpv->internal_root_path_cost > pv->internal_root_path_cost)
	{
		return INFO_WORSE;
	}

/*compare designated_bridge_id*/
	if(memcmp(cpv->designated_bridge_id.pri, pv->designated_bridge_id.pri, 2) < 0)
	{
		return INFO_BETTER;
	}
	else if(memcmp(cpv->designated_bridge_id.pri, pv->designated_bridge_id.pri, 2) > 0)
	{
		return INFO_WORSE;
	}

	if(memcmp(cpv->designated_bridge_id.mac_addr, pv->designated_bridge_id.mac_addr, MAC_LEN) < 0)
	{
		return INFO_BETTER;
	}
	else if(memcmp(cpv->designated_bridge_id.mac_addr, pv->designated_bridge_id.mac_addr, MAC_LEN) > 0)
	{
		return INFO_WORSE;
	}

/*compare designated_port_id*/
/*need to modify */ //FIXME
    if(cpv->designated_port_id < pv->designated_port_id)
	{
		return INFO_BETTER;
    }
    else if(cpv->designated_port_id > pv->designated_port_id)
    {
		return INFO_WORSE;
    }

/*compare recv_port_id*/
	if(cpv->recv_port_id < pv->recv_port_id)
	{
		return INFO_BETTER;
	}
    else if(cpv->recv_port_id > pv->recv_port_id)
    {
		return INFO_WORSE;
	}
	
	return INFO_SAME;
}


int mstp_cist_times_cmp(struct cist_timer_vector *ct, struct cist_timer_vector *cct)
{
	if ((NULL == ct) || (NULL == cct))
	{
		return MSTP_FALSE;
	}
	
	if((ct->forward_delay != cct->forward_delay)
		|| (ct->hello_time != cct->hello_time)
		|| (ct->max_age != cct->max_age)
		|| (ct->msg_age != cct->msg_age)
		|| (ct->remaining_hops != cct->remaining_hops))
	{
		return MSTP_FALSE;
	}
	
	return MSTP_TRUE;
}

int mstp_msti_times_cmp(struct msti_timer_vector *mt, struct msti_timer_vector *cmt)
{
	if ((NULL == mt) || (NULL == cmt))
	{
		return MSTP_FALSE;
	}

	if(mt->remaining_hops != cmt->remaining_hops)
	{
		return MSTP_FALSE;
	}

	return MSTP_TRUE;
}

unsigned char mstp_is_same_br(struct bridge_id bridge_id)
{
	if (NULL == mstp_global_bridge)
	{
		return MSTP_FALSE;
	}

	if(0 == memcmp(bridge_id.mac_addr, mstp_global_bridge->bridge_mac.mac_addr, MAC_LEN))
	{
		return MSTP_TRUE;
	}

	return MSTP_FALSE;
}

/*13.27.25 syncMaster()
For all MSTIs, for each port that has infoInternal set:
a) Clears the agree, agreed, and synced variables; and
b) Sets the sync variable.*/
void mstp_sync_master(struct mstp_bridge *mstp_bridge)
{
	unsigned int			ins_id = 0;		/*instance id*/
	struct mstp_msti_port	*msti_port = NULL, *next_msti_port = NULL;
	struct mstp_msti_br		*msti_br = NULL;

	if (NULL == mstp_bridge)
	{
		return;
	}

	for(ins_id = 0; ins_id < MSTP_INSTANCE_MAX; ins_id++)
	{
		msti_br = mstp_bridge->msti_br[ins_id];
		if(msti_br != NULL)
		{
			list_for_each_entry_safe(msti_port, next_msti_port, &msti_br->port_head, port_list)
			{
				if(msti_port->mport->port_variables.rcvdInternal)
				{
			    	msti_port->common_info.agree = MSTP_FALSE;
					msti_port->common_info.agreed = MSTP_FALSE;
					msti_port->common_info.synced = MSTP_FALSE;
					msti_port->common_info.sync = MSTP_TRUE;
				}
			}
		}
	}

	return;
}

/*13.25.2 ageingTime
Filtering database entries for this port are aged out after ageingTime has elapsed since they were first created
or refreshed by the Learning Process. The value of this parameter is normally Ageing Time (8.8.3, Table 8-
6), and is changed to FwdDelay (13.26.8) for a period of FwdDelay after  fdbFlush (13.25.13) is set by the
topology change state machine if stpVersion (13.26.20) is TRUE.*/
int mstp_aging_time(struct mstp_port *port)
{
	if ((NULL == port) || (NULL == port->m_br))
	{
		return 0;
	}

	if(port->m_br->common_br.stp_version(port))
	{
		return  mstp_fwd_delay(port);
	}

	return 0;
}


unsigned char mstp_get_port_role(struct mstp_port *port, unsigned int instance_id)
{
	unsigned char				port_role = 0xFF;	/*0xFF is invalid*/
	struct mstp_common_port		*common_info = NULL;

	if (NULL == port)
	{
		return port_role;
	}

    if(0 == instance_id)	/*cist*/
    {
        common_info = &port->cist_port.common_info;
    }
    else	/*msti*/
    {
        common_info = &port->msti_port[instance_id-1]->common_info;
    }


    switch(common_info->role)
    {
        case MSTP_PORT_ROLE_ROOT:
        {
			port_role = BPDU_ROLE_ROOT;
        }
            break;
		case MSTP_PORT_ROLE_MASTER:
		{
			port_role = BPDU_ROLE_MASTER;
		}
			break;
        case MSTP_PORT_ROLE_DESIGNATED:
        {
			port_role = BPDU_ROLE_DESIGNATED;
        }
            break;
        case MSTP_PORT_ROLE_ALTERNATE:
        case MSTP_PORT_ROLE_BACKUP:
        {
			port_role = BPDU_ROLE_ALTBCK;
		}
            break;
		default:
			break;
    }

	return port_role;
}

unsigned char mstp_get_role_by_flag(unsigned char flag)
{
	/*bit [0000 1100]*/
	return flag & 0x0c;
}

unsigned char mstp_same_root(struct bridge_id src, struct bridge_id dst)
{
	if((src.pri[0] != dst.pri[0])
		|| (src.pri[1] != dst.pri[1]))
	{
		return MSTP_FALSE;
	}

	if(0 == memcmp(src.mac_addr, dst.mac_addr, MAC_LEN))
	{
		return MSTP_TRUE;
	}
	else
	{
		return MSTP_FALSE;
	}

	return MSTP_TRUE;
}



