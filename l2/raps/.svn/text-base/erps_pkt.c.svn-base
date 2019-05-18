#include <string.h>
#include <stdlib.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/oam_common.h>
#include <lib/types.h>
#include <ftm/pkt_eth.h>
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <lib/log.h>


#include "raps/erps.h"
#include "raps/erps_pkt.h"
#include "raps/erps_timer.h"
#include "raps/erps_fsm_action.h"
#include "raps/erps_fsm.h"


void raps_pkt_register(void)
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));

    proto.ethreg.ethtype = 0x8902;
    proto.ethreg.oam_opcode = CFM_OPCODE_RAPS;

    pkt_register(MODULE_ID_L2, PROTO_TYPE_ETH, &proto);

    return;
}

int raps_validity_check(struct erps_sess *psess, struct raps_pkt *raps_pdu)
{
    if(psess && raps_pdu)
    {

        ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);

        if(erps_node_id_higher(raps_pdu->node_id, psess->info.node_id) == 0)
        {
            ERPS_LOG_DBG("My node id!! Drop packet!!\n");
            gerps.pkt_err++;
            return ERRNO_FAIL;
        }

        if((raps_pdu->request_state == ERPS_FORCED_SWITCH) ||
                (raps_pdu->request_state == ERPS_EVENT) ||
                (raps_pdu->request_state == ERPS_SIGNAL_FAIL) ||
                (raps_pdu->request_state == ERPS_MANUAL_SWITCH) ||
                (raps_pdu->request_state == ERPS_NO_REQUEST))
        {

            if((raps_pdu->request_state == ERPS_EVENT) &&
                    (raps_pdu->sub_code == 0) &&
                    (raps_pdu->status_reserved == 0))
            {
                return ERRNO_SUCCESS;
            }
            else
            {
                if(psess->guard_timer == 0)
                {
                    return ERRNO_SUCCESS;
                }
                else
                {
                    ERPS_LOG_DBG("session %d guard_timer is running drop it ! \n", psess->info.sess_id);
					return ERRNO_FAIL;
                }
            }
        }

    }

    return ERRNO_FAIL;
}

int raps_forward(struct erps_sess *psess,const struct raps_pkt *recv_aps,uint32_t	 ifindex, enum forward_direction direc)
{
    union pkt_control pktcontrol;
	struct raps_pkt raps_send;
	
    uint8_t mac[6] = {0x01, 0x19, 0xA7, 0x00, 0x00, 0x00};
    int ret = 0;

    ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);
    memset(&pktcontrol, 0, sizeof(union pkt_control));
	memset(&raps_send, 0, sizeof(struct raps_pkt));

	if(NULL == psess || psess->info.sub_ring != 1)
	{
		return ret;
	}

	if(psess->info.role == ERPS_ROLE_OWNER && psess->info.block_interface == psess->info.east_interface)
	{
		return ret;
	}
	
	memcpy(&raps_send, recv_aps, sizeof(struct raps_pkt));
    pktcontrol.ethcb.ethtype = ETH_P_CFM;
    pktcontrol.ethcb.smac_valid = 0;
    pktcontrol.ethcb.is_changed = 1;
    pktcontrol.ethcb.svlan_tpid = 0x8100;
    pktcontrol.ethcb.cos = psess->info.priority;

	if(direc == VIRTUAL_TUNNEL_2_INTERFACE)
	{
		
		mac[5] = psess->info.ring_id;
        memcpy(& pktcontrol.ethcb.dmac, mac, 6);
		raps_send.level = psess->info.level;
		pktcontrol.ethcb.svlan = psess->info.pvlan;
		if((psess->info.east_interface) && (psess->info.east_interface != ifindex))
		{
			pktcontrol.ethcb.ifindex = psess->info.east_interface;
			ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &raps_send, sizeof(struct raps_pkt));

	        if(ret == NET_FAILED)
	        {
	            ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);
	            return ERRNO_FAIL;
	        }
		}

		if(gerps.debug_packet)
	    {
	    	printf(" virtual tunnel 0x%x forward pkt to eastInterface 0x%x \r\n", ifindex, psess->info.east_interface);
	    }
	}
	else if(direc == INTERFACE_2_VIRTUAL_TUNNEL)
	{
		if(psess->info.attach_sess_id != 0)
		{
			psess->attach_to_sess = erps_sess_lookup(psess->info.attach_sess_id);
		}
		else
		{
			psess->attach_to_sess = NULL;
		}
		
		if(psess->attach_to_sess == NULL)
		{
			ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);
			return ERRNO_FAIL;
		}

		mac[5] = psess->attach_to_sess->info.ring_id;
        memcpy(& pktcontrol.ethcb.dmac, mac, 6);
        raps_send.level = psess->attach_to_sess->info.level;
		pktcontrol.ethcb.svlan = psess->info.pvlan;

		if((psess->attach_to_sess->info.east_interface) && (psess->attach_to_sess->info.east_interface != ifindex))
        {
            pktcontrol.ethcb.ifindex = psess->attach_to_sess->info.east_interface;
            ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &raps_send, sizeof(struct raps_pkt));

            if(ret == NET_FAILED)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);
                return ERRNO_FAIL;
            }
        }

        if((psess->attach_to_sess->info.west_interface) && (psess->attach_to_sess->info.west_interface != ifindex))
        {
            pktcontrol.ethcb.ifindex = psess->attach_to_sess->info.west_interface;
            ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &raps_send, sizeof(struct raps_pkt));

            if(ret == NET_FAILED)
            {
                ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);
                return ERRNO_FAIL;
            }
        }
			
		if(gerps.debug_packet)
	    {
	    	printf(" eastInterface 0x%x forward pkt to attachEastInterface 0x%x attachWestInterface 0x%x \r\n", ifindex, psess->attach_to_sess->info.east_interface,psess->attach_to_sess->info.west_interface);
	    }
	}
		
    gerps.pkt_send ++;
    return ERRNO_SUCCESS;
}

int raps_recv(struct pkt_buffer *ppkt)
{
    struct erps_sess *psess = NULL;
    struct hash_bucket *pbucket = NULL;
    struct raps_pkt recv_aps;
    uint16_t ring_id = 0;
    int ret = ERRNO_FAIL;
    int cursor;

    if(ppkt == NULL)
    {
        return ERRNO_FAIL;
    }

    gerps.pkt_recv++;

    ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);

    ring_id = ppkt->cb.ethcb.dmac[5];
    memset(&recv_aps, 0, sizeof(struct raps_pkt));
    memcpy(&recv_aps, ppkt->data, sizeof(struct raps_pkt));

    if(gerps.debug_packet)
    {
    	printf("ring_id %d vlan %d \r\n",ring_id,ppkt->cb.ethcb.svlan);
        printf("Recv R-APS Packet : %s\n", pkt_dump(&recv_aps, sizeof(struct raps_pkt)));
    }

    /* 根据报文查找对应�?erps session */
    HASH_BUCKET_LOOP(pbucket, cursor, erps_session_table)
    {
        psess = (struct erps_sess *)pbucket->data;

        if(psess->info.status == SESSION_STATUS_ENABLE)
        {
        	if(ppkt->cb.ethcb.svlan != psess->info.pvlan)
            {
            	ERPS_LOG_DBG("%s: '%s'--the line of %d ppkt->cb.ethcb.svlan %d psess->info.pvlan %d \r\n",
                           __FILE__, __func__, __LINE__, ppkt->cb.ethcb.svlan, psess->info.pvlan);
            	gerps.pkt_err++;
                continue;
            }

			if(psess->info.attach_sess_id != 0)
			{
				psess->attach_to_sess = erps_sess_lookup(psess->info.attach_sess_id);
			}
			else
			{
				psess->attach_to_sess = NULL;
			}
			
        	if(psess->attach_to_sess == NULL)
        	{
        		ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);
        		if((psess->info.west_interface != ppkt->in_ifindex) && (psess->info.east_interface != ppkt->in_ifindex))
                {
                	ERPS_LOG_DBG("%s: '%s'--the line of %d in_ifindex %x west_interface %x east_interface %x \r\n",
                           __FILE__, __func__, __LINE__, ppkt->in_ifindex, psess->info.west_interface, psess->info.east_interface);
                	gerps.pkt_err++;
                    continue;  
				}				
				
                if((recv_aps.level != psess->info.level) || (psess->info.ring_id != ring_id))
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d recv_aps.level %d ring_id %d psess->info.ring_id %d psess->info.level %d \r\n",
                           __FILE__, __func__, __LINE__, recv_aps.level, ring_id, psess->info.ring_id, psess->info.level);
                    gerps.pkt_err++;
                    continue;
                }

				ret = raps_validity_check(psess, &recv_aps);
                if(ret)
                {
                	ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);
                    gerps.pkt_err++;
                    continue;
                }
        	}
			else
			{		
                if((psess->info.west_interface == ppkt->in_ifindex) ||
                    (psess->info.east_interface == ppkt->in_ifindex))
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d recv_aps.level %d ring_id %d psess->info.ring_id %d psess->info.level %d \r\n",
                               __FILE__, __func__, __LINE__, recv_aps.level, ring_id, psess->info.ring_id, psess->info.level);

                    if((recv_aps.level != psess->info.level) || (psess->info.ring_id != ring_id))
                    {
                        ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);
                        gerps.pkt_err++;
                        continue;
                    }

					ret = raps_validity_check(psess, &recv_aps);
                    if(ret != ERRNO_SUCCESS)
                    {
                        ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);
                        gerps.pkt_err++;
                        continue;
                    }
					else
					{
						raps_forward(psess, &recv_aps, ppkt->in_ifindex, INTERFACE_2_VIRTUAL_TUNNEL);
					}
                }
                else if((psess->attach_to_sess->info.west_interface == ppkt->in_ifindex) ||
                        (psess->attach_to_sess->info.east_interface == ppkt->in_ifindex))
                {
                    ERPS_LOG_DBG("%s: '%s'--the line of %d recv_aps.level %d ring_id %d attach..ring_id %d attach_level %d \r\n",
                               __FILE__, __func__, __LINE__, recv_aps.level, ring_id, psess->attach_to_sess->info.ring_id, psess->attach_to_sess->info.level);

                    if((recv_aps.level != psess->attach_to_sess->info.level) ||
                            (psess->attach_to_sess->info.ring_id != ring_id))
                    {
                       ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);
                       gerps.pkt_err++;
                       continue; 
                    }
					
					ret = raps_validity_check(psess, &recv_aps);
                    if(ret != ERRNO_SUCCESS)
                    {
                    	ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);
                        gerps.pkt_err++;
                        continue;
                    }
					else
					{
					    raps_forward(psess, &recv_aps, ppkt->in_ifindex, VIRTUAL_TUNNEL_2_INTERFACE);
					}
                }
                else
                {
                	ERPS_LOG_DBG("%s: '%s'--the line of %d \r\n", __FILE__, __func__, __LINE__);
                    gerps.pkt_err++;
                    continue;
                }
			}

            switch(recv_aps.request_state)
            {
                case ERPS_FORCED_SWITCH:
                    psess->info.current_event = ERPS_EVENT_RAPS_FS;
                    break;

                case ERPS_SIGNAL_FAIL:
                    psess->info.current_event = ERPS_EVENT_RAPS_SF;
                    break;

                case ERPS_MANUAL_SWITCH:
                    psess->info.current_event = ERPS_EVENT_RAPS_MS;
                    break;

                case ERPS_NO_REQUEST:
                    if(recv_aps.rb)
                    {
                        psess->info.current_event = ERPS_EVENT_RAPS_NR_RB;
                    }
                    else
                    {
                        psess->info.current_event = ERPS_EVENT_RAPS_NR;
                    }

                    break;

                case ERPS_EVENT:
                default:
                    psess->info.current_event = ERPS_INVALID_EVENT;
                    break;
            }

            ERPS_LOG_DBG("%s: '%s'--the line of %d psess->info.current_event %d \r\n", __FILE__, __func__, __LINE__, psess->info.current_event);

            if(psess->info.current_event)
            {
                ret = erps_fsm(psess, &recv_aps);
                if(ret)
                {
                    ERPS_LOG_DBG("[Func:%s]:erps_fsm return error %d.--Line:%d \r\n", __FUNCTION__, ret , __LINE__);
                    gerps.pkt_err++;
                    continue;
                }
            }
            else
            {
                gerps.pkt_err++;
                ERPS_LOG_DBG("recv event is invalid drop it! \n");
                continue;
            }
        }
    }
    return ERRNO_SUCCESS;
}
int raps_send(struct erps_sess *psess)
{
    union pkt_control pktcontrol;
    uint8_t mac[6] = {0x01, 0x19, 0xA7, 0x00, 0x00, 0x00};
    int ret;


    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
    memset(&pktcontrol, 0, sizeof(union pkt_control));

    pktcontrol.ethcb.ethtype = ETH_P_CFM;
    pktcontrol.ethcb.smac_valid = 0;
    pktcontrol.ethcb.is_changed = 1;
    pktcontrol.ethcb.svlan_tpid = 0x8100;

    pktcontrol.ethcb.cos = psess->info.priority;

    if(psess->info.west_interface)
    {
        mac[5] = psess->info.ring_id;
        memcpy(& pktcontrol.ethcb.dmac, mac, 6);
        psess->r_aps.level = psess->info.level;
        pktcontrol.ethcb.svlan = psess->info.pvlan;
        pktcontrol.ethcb.ifindex = psess->info.west_interface;
        ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &psess->r_aps, sizeof(struct raps_pkt));

        if(ret == NET_FAILED)
        {
            ERPS_LOG_DBG("pkt_send fail.\n");
            return ERRNO_FAIL;
        }
    }
    else
    {
    	if(psess->info.attach_sess_id != 0)
		{
			psess->attach_to_sess = erps_sess_lookup(psess->info.attach_sess_id);
		}
		else
		{
			psess->attach_to_sess = NULL;
		}
		
        if(psess->attach_to_sess != NULL)
        {
            mac[5] = psess->attach_to_sess->info.ring_id;
            memcpy(& pktcontrol.ethcb.dmac, mac, 6);
            psess->r_aps.level = psess->attach_to_sess->info.level;
            pktcontrol.ethcb.svlan = psess->info.pvlan; 

            if(psess->attach_to_sess->info.east_interface)
            {
                pktcontrol.ethcb.ifindex = psess->attach_to_sess->info.east_interface;
                ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &psess->r_aps, sizeof(struct raps_pkt));

                if(ret == NET_FAILED)
                {
                    ERPS_LOG_DBG("pkt_send fail.\n");
                    return ERRNO_FAIL;
                }
            }

            if(psess->attach_to_sess->info.west_interface)
            {
                pktcontrol.ethcb.ifindex = psess->attach_to_sess->info.west_interface;
                ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &psess->r_aps, sizeof(struct raps_pkt));

                if(ret == NET_FAILED)
                {
                    ERPS_LOG_DBG("pkt_send fail.\n");
                    return ERRNO_FAIL;
                }
            }
        }
    }

    if(gerps.debug_packet)
    {
        printf("West interface Send R-APS Packet : %s\n", pkt_dump(&psess->r_aps, sizeof(struct raps_pkt)));
    }

    if(psess->info.east_interface)
    {
        mac[5] = psess->info.ring_id;
        memcpy(& pktcontrol.ethcb.dmac, mac, 6);
        psess->r_aps.level = psess->info.level;
        pktcontrol.ethcb.svlan = psess->info.pvlan;
        pktcontrol.ethcb.ifindex = psess->info.east_interface;
        ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &psess->r_aps, sizeof(struct raps_pkt));

        if(ret == NET_FAILED)
        {
            ERPS_LOG_DBG("pkt_send fail.\n");
            return ERRNO_FAIL;
        }
    }
    else
    {
    	if(psess->info.attach_sess_id != 0)
		{
			psess->attach_to_sess = erps_sess_lookup(psess->info.attach_sess_id);
		}
		else
		{
			psess->attach_to_sess = NULL;
		}
		
        if(psess->attach_to_sess != NULL)
        {
            mac[5] = psess->attach_to_sess->info.ring_id;
            memcpy(& pktcontrol.ethcb.dmac, mac, 6);
            psess->r_aps.level = psess->attach_to_sess->info.level;
            pktcontrol.ethcb.svlan = psess->info.pvlan;

            if(psess->attach_to_sess->info.east_interface)
            {
                pktcontrol.ethcb.ifindex = psess->attach_to_sess->info.east_interface;
                ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &psess->r_aps, sizeof(struct raps_pkt));

                if(ret == NET_FAILED)
                {
                    ERPS_LOG_DBG("pkt_send fail.\n");
                    return ERRNO_FAIL;
                }
            }

            if(psess->attach_to_sess->info.west_interface)
            {
                pktcontrol.ethcb.ifindex = psess->attach_to_sess->info.west_interface;
                ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &psess->r_aps, sizeof(struct raps_pkt));

                if(ret == NET_FAILED)
                {
                    ERPS_LOG_DBG("pkt_send fail.\n");
                    return ERRNO_FAIL;
                }
            }
        }
    }

    if(gerps.debug_packet)
    {
        printf("East interface Send R-APS Packet : %s\n", pkt_dump(&psess->r_aps, sizeof(struct raps_pkt)));
    }

    gerps.pkt_send ++;
    return ERRNO_SUCCESS;
}


int raps_send_burst(struct erps_sess *psess)
{
    int i ;

    if(psess == NULL)
    {
        return ERRNO_FAIL;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

	if(psess->info.block_interface != 0 &&
		psess->info.block_interface == psess->info.east_interface)
	{
		psess->r_aps.bpr = 1;
	}
	
    erps_stop_msg_timer(psess);

    for(i = 0; i < 3; i++)
    {
        raps_send(psess);
    }

    erps_start_msg_timer(psess);
    return ERRNO_SUCCESS;
}

void raps_stop(struct erps_sess *psess)
{

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    erps_stop_msg_timer(psess);
}
