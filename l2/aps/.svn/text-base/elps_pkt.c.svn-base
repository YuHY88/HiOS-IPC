#include <string.h>
#include <stdlib.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/oam_common.h>
#include <lib/types.h>
#include <ftm/pkt_eth.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/errcode.h>
#include <lib/log.h>

#include "aps/elps_pkt.h"
#include "aps/elps_fsm.h"
#include "aps/elps_timer.h"
#include "cfm/cfm_session.h"
#include "l2_msg.h"

void aps_pkt_register(void)
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(union proto_reg));

	proto.ethreg.ethtype = 0x8902;
	proto.ethreg.oam_opcode = CFM_OPCODE_APS;

	pkt_register(MODULE_ID_L2, PROTO_TYPE_ETH, &proto);	

	return;
}


int aps_recv(struct pkt_buffer *ppkt)
{
	struct elps_sess *psess = NULL;
	struct hash_bucket *pbucket = NULL;
	struct aps_pkt recv_aps;
	int ret;
   	int cursor;

	if(ppkt == NULL)
	{
		return ERRNO_FAIL;
	}
	
	gelps.pkt_recv++;

	memset (&recv_aps, 0, sizeof(struct aps_pkt));
	memcpy(&recv_aps, ppkt->data, sizeof(struct aps_pkt));
	if(gelps.debug_packet)
	{
		printf("Recv APS Packet : %s\n", pkt_dump(&recv_aps,sizeof(struct aps_pkt)));
	}

	/* 根据报文查找对应的 elps session */
	HASH_BUCKET_LOOP(pbucket, cursor, elps_session_table)
	{
		psess =  (struct elps_sess *)pbucket->data;
		if(psess->info.status == APS_STATUS_ENABLE)
		{
			if((psess->info.master_port == ppkt->in_ifindex)||(psess->info.backup_port == ppkt->in_ifindex))
			{ 	
				if(ppkt->cb.ethcb.svlan== psess->info.pvlan)
				{	
					/*DFOP状态判断*/
					if ( psess->info.master_port == ppkt->in_ifindex )
					{	
						ELPS_LOG_DBG("%s:APS frame received on working path --the line of %d",__FILE__,__LINE__);
						/*psess->info.dfop_alarm = ELPS_RECV_APS_ON_MASTER;*/
						psess->info.dfop_alarm = 0;
						elps_stop_dfop_timer(psess);
					}
					else if(psess->info.backup_port == ppkt->in_ifindex)
					{
						psess->info.dfop_alarm = 0;
						elps_stop_dfop_timer(psess);
					}
					else
					{
						ELPS_LOG_DBG("%s:APS frame received not on protection path --the line of %d",__FILE__,__LINE__);
						gelps.pkt_err++;
						continue;

					}
					
					/* ABD -- 010x/001x/011x are  incorrect */
				    	if ( ((!recv_aps.port_type_A)&&( recv_aps.port_type_B)&&(!recv_aps.port_type_D)) || 
				         		((!recv_aps.port_type_A) &&( ! recv_aps.port_type_B)&& (recv_aps.port_type_D)) ||
				         		((!recv_aps.port_type_A )&& (recv_aps.port_type_B)&& (recv_aps.port_type_D )) )
					{
						ELPS_LOG_DBG("%s:validate elps aps pdu, Invalid ABDR in PDU --the line of %d",__FILE__,__LINE__);
						gelps.pkt_err++;
						psess->info.dfop_alarm = ELPS_RECV_APS_ABDR_INVALID;
						continue;
					}
					else
					{
						psess->info.dfop_alarm = 0;
					}
					if(recv_aps.port_type_B != 1)
					{
						ELPS_LOG_DBG("%s:validate elps aps pdu, Invalid B in PDU --the line of %d",__FILE__,__LINE__);
						gelps.pkt_err++;
						psess->info.dfop_alarm = ELPS_RECV_APS_B_INVALID;
						continue;
					}
					else
					{
						psess->info.dfop_alarm = 0;
					}
				
					psess->info.current_event=  recv_aps.request_state;

					ret = elps_fsm(psess, &recv_aps,psess->info.current_event);
					if(ret)
					{
						ELPS_LOG_DBG("[Func:%s]:elps_fsm return error %d.--Line:%d", __FUNCTION__, ret ,__LINE__);
						continue;
					}
				}
			}
		}
	}		
	return ERRNO_SUCCESS;
}


int elps_fsm(struct elps_sess *psess, struct aps_pkt *aps_pdu,enum ELPS_EVENT event)
{
	int ret = ERRNO_FAIL;
	
	if(psess == NULL)
	{
		ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
		return ERRNO_FAIL;
	}
	if(psess->info.status == APS_STATUS_DISABLE)
	{
		ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
		return ERRNO_FAIL;
	} 
	
	ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d event %d  current_status %d ",__FILE__,__func__,__LINE__,event,psess->info.current_status);
	switch(event)
	{
		/*远端事件*/
		case APS_ELPS_NR:
			if(psess->info.current_status == ELPS_NO_REQUEST )
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret =elps_aps_handle_nr(psess,aps_pdu);
			}
			break;
		case APS_ELPS_DNR:
			if(psess->info.current_status < ELPS_DO_NOT_REVERT)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret =elps_aps_handle_dnr(psess);
			}
			break;
		case APS_ELPS_WTR:
			if(psess->info.current_status < ELPS_WAIT_TO_RESTORE)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_aps_change_to_nrp(psess);
			}
			break;
		case APS_ELPS_MS:
			if(psess->info.current_status < ELPS_MANUAL_SWITCH)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_aps_change_to_nrp(psess);
			}
			break;
		case APS_ELPS_SF_W:
			if(psess->info.current_status < ELPS_SIGNAL_FAIL_FOR_WORKING)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_aps_change_to_nrp(psess);	
			}
			break;
		case APS_ELPS_FS:
			if(psess->info.current_status < ELPS_FORCED_SWITCH)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_aps_change_to_nrp(psess);
			}
			break;
		case APS_ELPS_SF_P:
			if(psess->info.current_status < ELPS_SIGNAL_FAIL_FOR_PROTECTION)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_aps_change_to_nrw(psess);
			}
			break;
		case APS_ELPS_LO:
			if(psess->info.current_status != ELPS_LOCKOUT_OF_PROTECTION)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_aps_change_to_nrw(psess);
			}
			break;
		/*本地事件*/
		case LOCAL_ELPS_LOP:
			if(psess->info.current_status != ELPS_LOCKOUT_OF_PROTECTION)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_local_handle_lop(psess);
			}
			break;
		case LOCAL_ELPS_SF_P:
			if(psess->info.current_status < ELPS_SIGNAL_FAIL_FOR_PROTECTION)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_local_handle_sf(psess);
			}
			break;
		case LOCAL_ELPS_FS:
			if(psess->info.current_status < ELPS_FORCED_SWITCH)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_local_handle_fs(psess);
			} 
			break;
		case LOCAL_ELPS_SF_W:
			if(psess->info.current_status < ELPS_SIGNAL_FAIL_FOR_WORKING)
			{ 
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_local_handle_sf(psess);
			}
			break;
		case LOCAL_ELPS_W_RECOVERS_FROM_SF:
			if(psess->info.current_status == ELPS_SIGNAL_FAIL_FOR_WORKING)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_local_handle_rsf(psess);
			}
			break;
		case LOCAL_ELPS_P_RECOVERS_FROM_SF:
			if(psess->info.current_status == ELPS_SIGNAL_FAIL_FOR_PROTECTION)
			{
				ret = elps_local_handle_rsf(psess);
			}
			break;
		case LOCAL_ELPS_MS:
			if(psess->info.current_status < ELPS_MANUAL_SWITCH)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_local_handle_ms(psess);
			}
			break;
		case LOCAL_ELPS_CLEAR:
			if(psess->info.current_status ==ELPS_FORCED_SWITCH)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_local_handle_fsclear(psess);
			}
			else if(psess->info.current_status ==ELPS_MANUAL_SWITCH)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_local_handle_msclear(psess);
			}
			else if(psess->info.current_status ==ELPS_LOCKOUT_OF_PROTECTION)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_local_handle_lopclear(psess);
			}
			break;
		case LOCAL_ELPS_WTR_EXPIRES:
			if(psess->info.current_status == ELPS_WAIT_TO_RESTORE)
			{
                ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				ret = elps_local_handle_wtr_timeout(psess);
			}
			break;
		default:
			break;
	}

	if(ret == ERRNO_SUCCESS)
	{
		ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_SESSION, IPC_OPCODE_UPDATE, psess->info.sess_id);
	    if(ret != ERRNO_SUCCESS)
	    {
	        psess->info.status = APS_STATUS_DISABLE;
	        ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
	        return ERRNO_IPC;
	    }
	}
	return ERRNO_SUCCESS;
}

int elps_state_update(uint32_t ifindex,uint16_t sess_id,enum ELPS_PORT_EVENT event)
{
	struct hash_bucket *bucket = NULL;
	struct elps_sess *psess = NULL;
	struct cfm_sess * sess = NULL;
	int cursor = 0;
	int ret ;
	
	if(ifindex == 0)
	{
		ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d ",__FILE__,__func__,__LINE__);
		return ERRNO_FAIL;
	}
	
	ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d event %d ",__FILE__,__func__,__LINE__,event);
	HASH_BUCKET_LOOP(bucket, cursor, elps_session_table)
	{
		 psess = (struct elps_sess *)bucket->data; 
		 if(ifindex == psess->info.master_port)
		 {	
		 	
		 	if(event == PORT_STATE_DOWN)
			{
				psess->info.current_event = LOCAL_ELPS_SF_W;
			}
			else if (event == PORT_STATE_UP)
			{
				if(psess->info.master_cfm_session)
				{
					sess = elps_find_cfm(ifindex,psess->info.master_cfm_session);
				}
				

				if(sess)
				{
					if(sess->state == OAM_STATUS_DOWN)
					{
						psess->info.current_event = LOCAL_ELPS_SF_W;
					}
					else
					{
						if(psess->info.current_status== ELPS_SIGNAL_FAIL_FOR_WORKING)
						{
							psess->info.current_event = LOCAL_ELPS_W_RECOVERS_FROM_SF;
						}
						else
						{
							continue;
						}
					}
				} 
				else
				{
					if(psess->info.current_status== ELPS_SIGNAL_FAIL_FOR_WORKING)
					{
						psess->info.current_event = LOCAL_ELPS_W_RECOVERS_FROM_SF;
					}
					else
					{
						continue;
					}
				}
			}
			else if(event == CC_STATE_DOWN)
			{
				
				ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d psess->info.master_cfm_session %d sess_id %d",__FILE__,__func__,__LINE__,psess->info.master_cfm_session,sess_id);	
				if(psess->info.master_cfm_session == sess_id)
				{
					ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
					psess->info.current_event = LOCAL_ELPS_SF_W;
				}
				else 
				{
					continue;
				}
			}
			else if(event == CC_STATE_UP)
			{
				
				ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d psess->info.master_cfm_session %d sess_id %d",__FILE__,__func__,__LINE__,psess->info.master_cfm_session,sess_id);	
				if(psess->info.master_cfm_session == sess_id)
				{		
					ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
					if(psess->info.current_status== ELPS_SIGNAL_FAIL_FOR_WORKING)
					{
						psess->info.current_event = LOCAL_ELPS_W_RECOVERS_FROM_SF;
					}
					else
					{
						continue;
					}
				}
				else 
				{
					continue;
				}
			} 
			else
			{
				return ERRNO_FAIL;
			}
			ret = elps_fsm(psess,NULL,psess->info.current_event);
			if(ret)
			{
				ELPS_LOG_DBG("[Func:%s]:elps_fsm return error %d.--Line:%d", __FUNCTION__, ret ,__LINE__);
				continue;
			}
		 }
		 else if (ifindex == psess->info.backup_port)
		 {	
		 	if(event == PORT_STATE_DOWN)
			{
				psess->info.current_event = LOCAL_ELPS_SF_P;
			}
			else if (event == PORT_STATE_UP)
			{
				if(psess->info.backup_cfm_session)
				{
					sess = elps_find_cfm(ifindex,psess->info.backup_cfm_session);
				}

				if(sess)
				{
					if(sess->state == OAM_STATUS_DOWN)
					{
						psess->info.current_event = LOCAL_ELPS_SF_P;
					}
					else
					{
						if(psess->info.current_status== ELPS_SIGNAL_FAIL_FOR_PROTECTION)
						{
							psess->info.current_event = LOCAL_ELPS_P_RECOVERS_FROM_SF;
						}
						else
						{
							continue;
						}
					}
				}
				else
				{
					if(psess->info.current_status== ELPS_SIGNAL_FAIL_FOR_PROTECTION)
					{
						psess->info.current_event = LOCAL_ELPS_P_RECOVERS_FROM_SF;
					}
					else
					{
						continue;
					}
				}
			}
			else if(event == CC_STATE_DOWN)
			{
				
				ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d psess->info.backup_cfm_session %d sess_id %d",__FILE__,__func__,__LINE__,psess->info.backup_cfm_session,sess_id);	
				if(psess->info.backup_cfm_session== sess_id)
				{			
					ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
					psess->info.current_event = LOCAL_ELPS_SF_P;
				}
				else 
				{
					continue;
				}
			}
			else if(event == CC_STATE_UP)
			{
				if(psess->info.backup_cfm_session== sess_id)
				{
					ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d psess->info.master_cfm_session %d sess_id %d",__FILE__,__func__,__LINE__,psess->info.master_cfm_session,sess_id);	
					if(psess->info.current_status== ELPS_SIGNAL_FAIL_FOR_PROTECTION)
					{				
						ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
						psess->info.current_event = LOCAL_ELPS_P_RECOVERS_FROM_SF;
					}
					else
					{
						continue;
					}
				}
				else 
				{
					continue;
				}
			}
			else
			{
				return ERRNO_FAIL;
			}
			ret = elps_fsm(psess,NULL,psess->info.current_event);
			if(ret)
			{
				ELPS_LOG_DBG("[Func:%s]:elps_fsm return error %d.--Line:%d", __FUNCTION__, ret ,__LINE__);
				continue;
			}
		 } 
	}
	return ERRNO_SUCCESS;	
	
}
int aps_send(struct elps_sess *psess)
{
	union pkt_control pktcontrol;
	uint8_t  mac[6] ={0x01,0x80,0xc2,0x00,0x00,0x30}; 
	//struct cfm_sess *sess = NULL;
	struct aps_pkt  aps_pdu;
	int ret;

	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}
	
	memset (&aps_pdu, 0, sizeof(struct aps_pkt));
	if(psess->info.backup_cfm_session)
	{
		//sess = elps_find_cfm(psess->info.backup_port,psess->info.backup_cfm_session);
	}

	aps_pdu.level = 7;
	aps_pdu.opcode = CFM_APS_OPCODE;
	aps_pdu.tlv_offset = 0x04;
	aps_pdu.request_state = psess->info.current_status;
	if(psess->info.active_port == psess->info.master_port)
	{
		aps_pdu.request_signal = 0;
		aps_pdu.bridge_signal = 0;
	}
	else if(psess->info.active_port == psess->info.backup_port)
	{
		aps_pdu.request_signal = 1;
		aps_pdu.bridge_signal = 1;
	}
	aps_pdu.port_type_A = 1 ;
	aps_pdu.port_type_B = 1;
	aps_pdu.port_type_D = 1;
	if(psess->info.failback == FAILBACK_ENABLE)
	{
		aps_pdu.port_type_R = 1;
	}
	else
	{
		aps_pdu.port_type_R = 0;
	}	
	
	memset (&pktcontrol, 0, sizeof(union pkt_control));
	mac[5] = mac[5] |(aps_pdu.level);
	memcpy(& pktcontrol.ethcb.dmac, mac, 6);
	
	pktcontrol.ethcb.ethtype = ETH_P_CFM;
	if(psess->info.current_status == ELPS_SIGNAL_FAIL_FOR_PROTECTION)
	{
		pktcontrol.ethcb.ifindex = psess->info.master_port;
	}
	else
	{
		pktcontrol.ethcb.ifindex = psess->info.backup_port;
	}
	pktcontrol.ethcb.smac_valid = 0;
	pktcontrol.ethcb.is_changed = 1;
	pktcontrol.ethcb.svlan_tpid = 0x8100;
	pktcontrol.ethcb.svlan = psess->info.pvlan;
	pktcontrol.ethcb.cos = psess->info.priority;

	gelps.pkt_send++;
	if(gelps.debug_packet)
	{
		printf("Send APS Packet : %s\n", pkt_dump(&aps_pdu,sizeof(struct aps_pkt)));
	}
	if((psess->dfop_timer == 0)&&(psess->info.dfop_alarm != ELPS_NO_APS_RECV))
	{
		elps_start_dfop_timer(psess);
	}
	
	ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &aps_pdu, sizeof(struct aps_pkt)); 	
	if(ret == NET_FAILED)
	{
		ELPS_LOG_DBG("pkt_send fail.\n");
		return ERRNO_FAIL;
	}
	
	return ERRNO_SUCCESS;
	
}

int aps_send_burst (struct elps_sess *psess)
{	
	int i ;
	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}
	elps_stop_msg_timer(psess); 
	for(i = 0; i<3; i++)
	{ 
		aps_send(psess);
	}
	psess->pkeepalive_timer= 0;
	elps_start_msg_timer(psess);
	return ERRNO_SUCCESS;
}




