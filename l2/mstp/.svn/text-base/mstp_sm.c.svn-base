/*
*  Copyright (C) 2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_sm.c	(used for mstp state machine)
*
*  date: 2017.3
*
*  modify:2017.10~~2017.12
*
*  modify:	2018.3.12 modified by liufuying to make mstp module code beautiful
*/
#include "lib/log.h"

#include "mstp.h"
#include "mstp_bpdu.h"
#include "mstp_port.h"
#include "mstp_sm_procedure.h"

#include "mstp_sm.h"



/*The Port Timers state machine shall implement the function specified by the state diagram
in Figure 13-15 and the attendant definitions in 13.23 through 13.27.*/
void mstp_port_timer_sm(struct mstp_port *port)
{
	unsigned int				msti_count = 0;
	unsigned int				port_timer_last_state = 0;
	struct mstp_cist_port		*mstp_cist_port = NULL;
	struct mstp_port_variables	*port_variable = NULL;

/*The following abbreviation is used in 
this state diagram:
dec(x)
{ if (x !=0) x= x-1;
}
If there is more than one instance of 
x, e.g. per port, all instances are 
separately decremented.*/

	if (NULL == port)
	{
		return;
	}

	port_variable = &(port->port_variables);
	mstp_cist_port = &(port->cist_port);

	port_timer_last_state = port_variable->sm_timer_state;

	/*update port timer state*/
	if(port_variable->sm_timer_state_begin)
    {
        port_variable->sm_timer_state = MSTP_ONE_SECOND;
		port_variable->sm_timer_state_begin = MSTP_FALSE;
    }
    else if((MSTP_ONE_SECOND == port_variable->sm_timer_state)
		&& (MSTP_TRUE == port_variable->tick)) /*change port timer state*/
    {
        port_variable->sm_timer_state = MSTP_TICK;
    }
    else if(MSTP_TICK == port_variable->sm_timer_state) /*UCT*/
    {
        port_variable->sm_timer_state = MSTP_ONE_SECOND;
    }

	/*state no change*/
	if(port_timer_last_state == port_variable->sm_timer_state)
	{
		return;
	}

	switch(port_variable->sm_timer_state)
	{
		case MSTP_ONE_SECOND:
		{
/*tick = FALSE;*/
			port_variable->tick = MSTP_FALSE;
		}
			break;
		case MSTP_TICK:
		{
/*rfc-[chapter-13.27]
**dec(helloWhen); dec(tcWhile); dec(fdWhile); dec(rcvdInfoWhile);
**dec(rrWhile); dec(rbWhile);dec(mdelayWhile); dec(txCount);*/

			MSTP_DEC(port_variable->helloWhen);
			MSTP_DEC(port_variable->mdelayWhile);
			MSTP_DEC(port_variable->txCount);
			MSTP_DEC(port_variable->edgeDelayWhile);
			MSTP_DEC(mstp_cist_port->common_info.rbWhile);

			MSTP_LOG_DBG("%s[%d]#########edgetime = %d\n",\
				__FUNCTION__,__LINE__,port_variable->edgeDelayWhile);
			/*cist*/
			if(mstp_cist_port != NULL)
			{
				MSTP_DEC(mstp_cist_port->common_info.tcWhile);
				MSTP_DEC(mstp_cist_port->common_info.fdWhile);
				MSTP_DEC(mstp_cist_port->common_info.rcvdInfoWhile);
				MSTP_DEC(mstp_cist_port->common_info.rrWhile);
				MSTP_DEC(mstp_cist_port->common_info.rbWhile);
				MSTP_LOG_DBG("%s[%d]##############ifindex = %02x fdwhile = %d\n",\
					__FUNCTION__,__LINE__,mstp_cist_port->port_index, mstp_cist_port->common_info.fdWhile);
			}

			/*msti*/
			for(msti_count = 0; msti_count < MSTP_INSTANCE_MAX; msti_count++)
			{
				if(port->msti_port[msti_count] != NULL)
				{
					MSTP_DEC(port->msti_port[msti_count]->common_info.tcWhile);
					MSTP_DEC(port->msti_port[msti_count]->common_info.fdWhile);
					MSTP_DEC(port->msti_port[msti_count]->common_info.rcvdInfoWhile);
					MSTP_DEC(port->msti_port[msti_count]->common_info.rrWhile);
					MSTP_DEC(port->msti_port[msti_count]->common_info.rbWhile);
					MSTP_LOG_DBG("%s[%d]#####ifindex = %02x fdwhile = %d\n",\
						__FUNCTION__,__LINE__,port->msti_port[msti_count]->port_index,port->msti_port[msti_count]->common_info.fdWhile);
				}
			}

			if(port_variable->ageingTime > 0)
			{
				port_variable->ageingTime--;
				if(0 == port_variable->ageingTime)
				{
					/*flushing of dynamic filtering information that has been learned;*/
					mstp_flush(0,port, 0);
				}				
			}
		}
			break;

		default:
			break;
	}
	
	return;
}


/*The Port Receive state machine shall implement the function specified by the state diagram
in Figure 13-16 and the attendant definitions in 13.23 through 13.27.*/
void mstp_port_recv_sm(struct mstp_port *port)
{
	unsigned int				port_recv_last_state = 0;
	unsigned int				port_recv_change_flag = MSTP_FALSE;
	struct mstp_port_variables	*port_variables = NULL;
	struct mstp_common_br		*common_bridge = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return;
	}

	common_bridge = &port->m_br->common_br;
	port_variables = &(port->port_variables);
	port_recv_last_state = port_variables->sm_rcv_state;

	/*judge port recv state*/
/*BEGIN ||  ((rcvdBpdu || (edgeDelayWhile != MigrateTime)) && !portEnabled)*/	
//	if((common_bridge->begin)
	if((port_variables->sm_rcv_state_begin)
/*rcvdBpdu && !portEnabled*/
	//	||((port_variables->rcvdBpdu) && (!port_variables->portEnabled)))
	|| ((port_variables->rcvdBpdu || 
	(port_variables->edgeDelayWhile != common_bridge->migrate_time))&& (!port_variables->portEnabled)))
    {
        port_variables->sm_rcv_state = MSTP_RECV_SM_DISCARD;
		port_variables->sm_rcv_state_begin = MSTP_FALSE;
    }
    else if((MSTP_RECV_SM_DISCARD == port_variables->sm_rcv_state)
/*rcvdBpdu && portEnabled && enableBPDUrx*/
		&& (port_variables->rcvdBpdu) 
		&& (port_variables->portEnabled))
    {
        port_variables->sm_rcv_state = MSTP_RECV_SM_RECEIVE;
    }
    else if((MSTP_RECV_SM_RECEIVE == port_variables->sm_rcv_state)
/*rcvdBpdu && portEnabled && enableBPDUrx && !rcvdAnyMsg*/
		&& (port_variables->rcvdBpdu)
		&& (port_variables->portEnabled)
		&& (!(common_bridge->rcvd_any_msg(port))))
    {
        port_variables->sm_rcv_state = MSTP_RECV_SM_RECEIVE; /*Figure 13-12 [Port Receive state machine]*/
		port_recv_change_flag = MSTP_TRUE;
    }	

	/*state no change*/
	if((port_recv_last_state == port_variables->sm_rcv_state)
		&& (MSTP_FALSE == port_recv_change_flag))
	{
		return;
	}


	switch(port_variables->sm_rcv_state)
	{
		case MSTP_RECV_SM_DISCARD:
		{
/*rcvdBpdu = rcvdRSTP = rcvdSTP = FALSE; 
clearAllRcvdMsgs();
edgeDelayWhile = MigrateTime;*/	

			port_variables->rcvdBpdu = MSTP_FALSE;
			port_variables->rcvdRSTP = MSTP_FALSE;
			port_variables->rcvdSTP = MSTP_FALSE;
			
			mstp_clear_all_recv_msg(port);
			//port_variables->edgeDelayWhile = common_bridge->migrate_time;	
			port_variables->edgeDelayWhile = common_bridge->pdu_older(port);
		}
			break;
		case MSTP_RECV_SM_RECEIVE:
		{
/*updtBPDUVersion();
rcvdInternal = fromSameRegion();
rcvMsgs();
operEdge = isolate = rcvdBpdu = FALSE;
edgeDelayWhile = MigrateTime;*/

		    mstp_update_bpdu_version(port);
		    port_variables->rcvdInternal = mstp_recv_from_same_region(port);

		    mstp_set_recv_msg(port);
		    port_variables->rcvdBpdu = MSTP_FALSE;
			port_variables->operEdge = MSTP_FALSE;	/*because recv msg, not edge port any more*/
			port_variables->isolate = MSTP_FALSE;
			//port_variables->edgeDelayWhile = common_bridge->migrate_time;	
			
			/*bug#51830 Aging time inaccuracy*/
			/*<stp>stp模式，配置bpdu老化时间Max Age不准确*/
			port_variables->edgeDelayWhile = common_bridge->pdu_older(port);
		}
			break;

		default:
			break;
	}

	return;
}


/*The Port Protocol Migration state machine shall implement the function specified by the state
diagram in Figure 13-17 and the attendant definitions in 13.23 through 13.27.*/
void mstp_port_protocol_migration_sm(struct mstp_port *port)
{
	unsigned int				port_protocol_migration_last_state = 0;

	struct mstp_common_br		*common_bridge = NULL;
	struct mstp_port_variables	*port_variable = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return;
	}

	common_bridge = &port->m_br->common_br;
	port_variable = &(port->port_variables);

	port_protocol_migration_last_state = port_variable->sm_migration_state;

    if(port_variable->sm_migration_state_begin)
    {
		port_variable->sm_migration_state = MSTP_CHECKING_RSTP;
		port_variable->sm_migration_state_begin = MSTP_FALSE;
    }
    else if((MSTP_CHECKING_RSTP == port_variable->sm_migration_state)
/*(mdelayWhile == 0)*/
		&& (0 == port_variable->mdelayWhile))
    {
        port_variable->sm_migration_state = MSTP_SENSING;
    }
    else if((MSTP_CHECKING_RSTP == port_variable->sm_migration_state)
/*(mdelayWhile != MigrateTime) 
&& !portEnabled*/		
		&& (port_variable->mdelayWhile != common_bridge->migrate_time)
		&& (!port_variable->portEnabled))
    {
/*mcheck = FALSE;
sendRSTP = (rstpVersion);
mdelayWhile = Migrate Time;*/    
//		port_variable->mcheck = MSTP_FALSE;			//we think always true
        port_variable->sendRSTP = common_bridge->rstp_version(port);
        port_variable->mdelayWhile = common_bridge->migrate_time;
        port_variable->sm_migration_state = MSTP_CHECKING_RSTP;
    }
    else if((MSTP_SENSING == port_variable->sm_migration_state)
/*sendRSTP && rcvdSTP*/		
	    && (port_variable->sendRSTP)
	    && (port_variable->rcvdSTP))
    {
        port_variable->sm_migration_state = MSTP_SELECTING_STP;
    }
	/*bug#53021 mcheck is forever true <rstp>
	非边缘端口迁移至Forwarding，没有触发rstp设备的stp模式根端口发送TCN bpdu*/
    else if((MSTP_SENSING == port_variable->sm_migration_state)
/*!portEnabled || mcheck || 
((rstpVersion) && !sendRSTP && rcvdRSTP))*/		
		&& ((!port_variable->portEnabled) 
			//|| (port_variable->mcheck)
			|| ((common_bridge->rstp_version(port)) 
				&& (!port_variable->sendRSTP)
				&& (port_variable->rcvdRSTP))))
    {
        port_variable->sm_migration_state = MSTP_CHECKING_RSTP;
    }
    else if((MSTP_SELECTING_STP == port_variable->sm_migration_state)
/*(mdelayWhile == 0) ||
!portEnabled || mcheck*/		
		&& ((port_variable->mdelayWhile == 0)
			|| (!port_variable->portEnabled)
			//|| (port_variable->mcheck)
			))
    {
        port_variable->sm_migration_state = MSTP_SENSING;
    }	

	/*state no change*/
	if(port_protocol_migration_last_state == port_variable->sm_migration_state)
	{
		return;
	}

	switch(port_variable->sm_migration_state)
	{
		case MSTP_CHECKING_RSTP:
		{
/*mcheck = FALSE;
sendRSTP = (rstpVersion);
mdelayWhile = Migrate Time;*/			
//			port_variable->mcheck = MSTP_FALSE;			//we think always true
			port_variable->sendRSTP = common_bridge->rstp_version(port);
			port_variable->mdelayWhile = common_bridge->migrate_time;
		}
			break;
		case MSTP_SELECTING_STP:
		{
/*sendRSTP = FALSE;
mdelayWhile = Migrate Time;*/			
			port_variable->sendRSTP = MSTP_FALSE;
			port_variable->mdelayWhile = common_bridge->migrate_time;	
		}
			break;
		case MSTP_SENSING:
		{
/*rcvdRSTP = rcvdSTP = FALSE;*/
			port_variable->rcvdRSTP = MSTP_FALSE; 
			port_variable->rcvdSTP = MSTP_FALSE;
		}
			break;
		default:
			break;
	}

	return;
}


/*The Port Transmit state machine shall implement the function specified by the state
diagram in Figure 13-19 and the attendant definitions in 13.23 through 13.27.*/
/*function:This state machine is responsible for transmitting BPDUs.*/
void mstp_port_tx_sm(struct mstp_port *port)
{
	unsigned int				port_tx_last_state = 0;
	struct mstp_port_variables	*port_variables = NULL;
	struct mstp_common_br		*common_bridge = NULL;
	struct mstp_common_port		*common_info = NULL;
	struct mstp_cist_port		*cist_port = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return;
	}

	cist_port = &(port->cist_port);
	common_info = &(port->cist_port.common_info);
	common_bridge = &(port->m_br->common_br);
	port_variables = &(port->port_variables);

	port_tx_last_state = port_variables->sm_tx_state;


/*All transtions, except UCT, are qualified by "&& allTransmitReady".*/

	if((port_variables->sm_tx_state_begin) 
/*BEGIN || !portEnabled || !enableBPDUtx*/			
		|| (!port_variables->portEnabled))	/*port unlink*/
	{
		port_variables->sm_tx_state = MSTP_TRANSMIT_INIT;
		port_variables->sm_tx_state_begin = MSTP_FALSE;
	}
	else if((MSTP_TRANSMIT_INIT == port_variables->sm_tx_state)		/*UCT*/
		|| (MSTP_TRANSMIT_PERIODIC == port_variables->sm_tx_state)	/*UCT*/
		|| (MSTP_TRANSMIT_CONFIG == port_variables->sm_tx_state)	/*UCT*/
		|| (MSTP_TRANSMIT_TCN == port_variables->sm_tx_state)		/*UCT*/
		|| (MSTP_TRANSMIT_RSTP == port_variables->sm_tx_state))		/*UCT*/
    {
        port_variables->sm_tx_state = MSTP_IDLE;
    }
	else if((MSTP_IDLE == port_variables->sm_tx_state)
		&& (common_bridge->all_transmit_ready(port))
/*helloWhen == 0*/
		&& (port_variables->helloWhen == 0))
	{
        port_variables->sm_tx_state = MSTP_TRANSMIT_PERIODIC;
	}
	else if((MSTP_IDLE == port_variables->sm_tx_state)
		&& (common_bridge->all_transmit_ready(port))

/*!sendRSTP
&& newInfo
&& cistDesignatedPort
&& (txCount < TxHoldCount)
&& (helloWhen != 0)*/
		&& (!port_variables->sendRSTP)
		&& (cist_port->new_info)
		&& (common_bridge->cist_designated_port(port)) /*(port == MSTP_PORT_ROLE_DESIGNATED)? 1:0*/
//		&& ((port_variables->txCount) < (common_bridge->tx_hold_count))	/*in fact, ipran do not care this*/
		&& (port_variables->helloWhen != 0))
	{
        port_variables->sm_tx_state = MSTP_TRANSMIT_CONFIG;
	}
	else if((MSTP_IDLE == port_variables->sm_tx_state)
		&& (common_bridge->all_transmit_ready(port))
/*!sendRSTP
&& newInfo
&& cistRootPort
&& (txCount < TxHoldCount)
&& (helloWhen != 0)*/	
		&& (!port_variables->sendRSTP)
		&& (cist_port->new_info)
		&& (common_bridge->cist_root_port(port))
//		&& ((port_variables->txCount) < (common_bridge->tx_hold_count))/*in fact, ipran do not care this*/
		&& (port_variables->helloWhen != 0))
		
	{
        port_variables->sm_tx_state = MSTP_TRANSMIT_TCN;
	}
	else if((MSTP_IDLE == port_variables->sm_tx_state)
		&& (common_bridge->all_transmit_ready(port))
/*
sendRSTP
&& (newInfo || (newInfoMsti && !mstiMasterPort))
&& (txCount < TxHoldCount)
&& (helloWhen !=0)
*/
		&& (port_variables->sendRSTP)
		&&((cist_port->new_info)
			||((port_variables->newInfoMsti) && (!common_bridge->msti_master_port(port))))
/*		&& ((port_variables->txCount) < (common_bridge->tx_hold_count))*/		//IPRAN do not care
		&& (port_variables->helloWhen != 0))
	{
		port_variables->sm_tx_state = MSTP_TRANSMIT_RSTP;
	}

	/*state no change*/
	if(port_tx_last_state == port_variables->sm_tx_state)
	{
		return;
	}

	switch(port_variables->sm_tx_state)
	{
		case MSTP_TRANSMIT_INIT:
		{
/*newInfo = newInfoMsti = TRUE;
txCount = 0;*/
			cist_port->new_info = MSTP_TRUE;
			port_variables->newInfoMsti = MSTP_TRUE;
			port_variables->txCount = 0;			
		}
			break;
		case MSTP_IDLE:
		{
			/*do nothing*/
            port_variables->helloWhen = common_bridge->hello_time(port);
		}
			break;
		case MSTP_TRANSMIT_PERIODIC:
		{
/*newInfo = newInfo || (cistDesignatedPort || 
(cistRootPort && (tcWhile !=0)));
newInfoMsti = newInfoMsti || 
mstiDesignatedOrTCpropagatingRootPort;*/	
			cist_port->new_info = (cist_port->new_info
						|| ((common_bridge->cist_designated_port(port))
						|| ((common_bridge->cist_root_port(port)) 
							&& (common_info->tcWhile != 0))));


            port_variables->newInfoMsti = port_variables->newInfoMsti ||
                			 		common_bridge->msti_designated_port(port);


		}
			break;
		case MSTP_TRANSMIT_CONFIG:
		{
/*newInfo = FALSE;
txConfig();
txCount +=1
tcAck = FALSE;
*/

			cist_port->new_info = MSTP_FALSE;
//			mstp_tx_config(port);
			/*send packet*/
			if(PROTO_VERID_STP == port->stp_mode)
			{
				mstp_tx_config(port);
			}
			else
			{
				mstp_tx_mstp(port);
			}
			
			port_variables->txCount += 1;	/*ipran do not care*/			
			/*ipran need statistics tx config pkt count*/		//FIXME
			
			port_variables->tcAck = MSTP_FALSE;
		}
			break;
		case MSTP_TRANSMIT_TCN:
		{
/* newInfo = FALSE;
txTcn();
txCount +=1;
*/

            cist_port->new_info = MSTP_FALSE;
			if(MSTP_ST_FORWARDING == port->cist_port.common_info.sm_state_trans_state)
			{
				mstp_tx_tcn(port);	//need to FIXME
				port_variables->txCount += 1;/*ipran do not care*/
			}
#if 0
			mstp_tx_tcn(port);	//need to FIXME
			port_variables->txCount += 1;/*ipran do not care*/
			/*ipran need statistics tx tcn pkt count*/	//FIXME		
#endif			
		}
			break;
		case MSTP_TRANSMIT_RSTP:
		{
/*newInfo = newInfoMsti = FALSE;
txRstp();
txCount +=1;
tcAck = FALSE;
*/

			cist_port->new_info = MSTP_FALSE;

			port_variables->newInfoMsti = MSTP_FALSE;
			/*send packet*/
			if(PROTO_VERID_STP == port->stp_mode)
			{
				mstp_tx_config(port);
			}
			else
			{
				mstp_tx_mstp(port);
			}

			port_variables->txCount += 1;/*ipran do not care*/
			/*ipran need statistics tx mstp pkt count*/	//FIXME
			port_variables->tcAck = MSTP_FALSE;
		}
			break;

		default:
			break;
	}

	return;
}


/*This state machine is responsible for recording the Spanning Tree information currently 
**in use by the CIST or a given MSTI for a given Port*/
void mstp_port_info_sm(struct mstp_port *port, unsigned int	instance_id)
{
	unsigned int				port_info_last_state = 0;
	unsigned int				port_info_change_flag = MSTP_FALSE;
	unsigned int				updt_cist_msti_info = MSTP_FALSE; /*used for cist or msti*/
	unsigned int				rcvd_cist_msti_msg = MSTP_FALSE; /*used for cist or msti*/

	struct mstp_port_variables	*port_variables = NULL;
	struct mstp_common_br		*common_bridge = NULL;
	struct mstp_cist_port		*mstp_cist_port = NULL;	
	struct mstp_msti_port		*mstp_msti_port = NULL;
	struct mstp_common_port		*common_info = NULL; /*each cist and msti port have one*/

	if ((NULL == port) || (NULL == port->m_br))
	{
		return;
	}

	mstp_cist_port = &(port->cist_port);
	common_bridge = &(port->m_br->common_br);
	port_variables = &(port->port_variables);

	if(0 == instance_id)
	{
		mstp_cist_port = &(port->cist_port);
		common_info = &(port->cist_port.common_info);
		updt_cist_msti_info = common_bridge->updt_cist_info(port);
		rcvd_cist_msti_msg = common_bridge->rcvd_cist_msg(port);
	}
	else
	{
		mstp_msti_port = port->msti_port[instance_id-1];
		common_info = &(port->msti_port[instance_id-1]->common_info);
		updt_cist_msti_info = common_bridge->updt_msti_info(port, instance_id);
		rcvd_cist_msti_msg = common_bridge->rcvd_msti_msg(port, instance_id);
	}

	port_info_last_state = common_info->sm_info_state;

	if((common_info->sm_info_state_begin)
/*(!portEnabled && (infoIs != Disabled)) */
		|| ((!port_variables->portEnabled) 
			&& (common_info->infoIs != MSTP_INFO_DISABLED)))
	{
		common_info->sm_info_state = MSTP_DISABLED;
		common_info->sm_info_state_begin = MSTP_FALSE;
	}
	else if((MSTP_DISABLED == common_info->sm_info_state)
/*portEnabled*/
		&& (port_variables->portEnabled))
	{
		common_info->sm_info_state = MSTP_AGED;
	}
	else if((MSTP_AGED == common_info->sm_info_state)
/*(selected && updtInfo)*/
		&& (common_info->selected)
		&& (common_info->updtInfo))
	{
		common_info->sm_info_state = MSTP_UPDATE;
	}
	else if((MSTP_UPDATE == common_info->sm_info_state)				/*UCT*/
		|| (MSTP_SUPERIOR_DESIGNATED == common_info->sm_info_state)	/*UCT*/
		|| (MSTP_REPEATED_DESIGNATED == common_info->sm_info_state)	/*UCT*/
		|| (MSTP_INFERIOR_DESIGNATED == common_info->sm_info_state)/*UCT*/
		|| (MSTP_NOT_DESIGNATED == common_info->sm_info_state)	/*UCT*/
		|| (MSTP_OTHER == common_info->sm_info_state))	/*UCT*/
	{		
		common_info->sm_info_state = MSTP_CURRENT;
	}
	else if((MSTP_CURRENT == common_info->sm_info_state)
/*(selected && updtInfo)*/		
		&& (common_info->selected)
		&& (common_info->updtInfo))
	{
		common_info->sm_info_state = MSTP_UPDATE;
	}
	else if((MSTP_CURRENT == common_info->sm_info_state)
/*(infoIs == Received) && (rcvdInfoWhile == 0) && 
!updtInfo && !rcvdXstMsg*/		
		&& (MSTP_INFO_RECEIVED == common_info->infoIs)
		&& (0 == common_info->rcvdInfoWhile)
		&& (!common_info->updtInfo)
		&& (!rcvd_cist_msti_msg))/*modify by youcheng 2018/8/30*/
	{
		common_info->sm_info_state = MSTP_AGED;
	}
	else if((MSTP_CURRENT == common_info->sm_info_state)
/*rcvdXstMsg && !updtXstInfo*/		
		&& (rcvd_cist_msti_msg)
		&& (!updt_cist_msti_info))
	{
		common_info->sm_info_state = MSTP_RECEIVE;
	}
	else if((MSTP_DISABLED == common_info->sm_info_state)
/*rcvdMsg*/		
		&& (common_info->rcvdMsg))
	{
		common_info->sm_info_state = MSTP_DISABLED;
		port_info_change_flag = MSTP_TRUE;
	}
	else if((MSTP_RECEIVE == common_info->sm_info_state)
/*  rcvdInfo == SuperiorDesignatedInfo*/		
		&& (MSTP_INFO_SUPERIOR_DESIGNATED == common_info->rcvdInfo))
	{
		common_info->sm_info_state = MSTP_SUPERIOR_DESIGNATED;
	}
	else if((MSTP_RECEIVE == common_info->sm_info_state)
/* rcvdInfo == RepeatedDesignatedInfo*/		
		&& (MSTP_INFO_REPEATED_DESIGNATED == common_info->rcvdInfo))
	{
		common_info->sm_info_state = MSTP_REPEATED_DESIGNATED;
	}
	else if((MSTP_RECEIVE == common_info->sm_info_state)
/* rcvdInfo == InferiorDesignatedInfo*/		
		&& (MSTP_INFO_INFERIOR_DESIGNATED == common_info->rcvdInfo))
	{
		common_info->sm_info_state = MSTP_INFERIOR_DESIGNATED;		
	}
	else if((MSTP_RECEIVE == common_info->sm_info_state)
/*rcvdInfo == InferiorRootAlternateInfo*/		
		 && (MSTP_INFO_NOT_DESIGNATED == common_info->rcvdInfo))
    {
        common_info->sm_info_state = MSTP_NOT_DESIGNATED;
    }
	else if((MSTP_RECEIVE == common_info->sm_info_state)
/* rcvdInfo == OtherInfo*/		
		&& (MSTP_INFO_OTHER == common_info->rcvdInfo))
	{
		common_info->sm_info_state = MSTP_OTHER;
	}

	/*state no change*/
	if((port_info_last_state == common_info->sm_info_state)
		&& (MSTP_FALSE == port_info_change_flag))
	{
		return;
	}

	switch(common_info->sm_info_state)
	{
		case MSTP_DISABLED:
		{

/*rcvdMsg = FALSE;
proposing = proposed = agree = agreed = FALSE;
rcvdInfoWhile = 0;
infoIs = Disabled;
reselect = TRUE;
selected = FALSE; 
*/
            common_info->rcvdMsg = MSTP_FALSE;
            common_info->proposing = MSTP_FALSE;
			common_info->proposed = MSTP_FALSE;
			common_info->agree = MSTP_FALSE;
			common_info->agreed = MSTP_FALSE;
            common_info->rcvdInfoWhile = 0;
            common_info->infoIs = MSTP_INFO_DISABLED;
            common_info->reselect = MSTP_TRUE;
            common_info->selected = MSTP_FALSE;
		}
			break;
		case MSTP_AGED:
		{
/*infoIs = Aged;
reselect = TRUE;
selected = FALSE;*/
			common_info->infoIs = MSTP_INFO_AGED;
			common_info->reselect = MSTP_TRUE;
			common_info->selected = MSTP_FALSE;
			common_info->sync= MSTP_TRUE;     /*2018/9/4*/
		}
			break;
		case MSTP_UPDATE:
		{

/*
proposing = proposed = FALSE;
agreed = agreed && betterorsameInfo(Mine);
synced = synced && agreed;
portPriority = designatedPriority;
portTimes = designatedTimes;
updtInfo = FALSE; 
infoIs = Mine;
newInfoXst = TRUE;
*/
            common_info->proposing = MSTP_FALSE;
			common_info->proposed = MSTP_FALSE;
            common_info->agreed = ((common_info->agreed) 
								&& (mstp_better_or_same_info(port, instance_id, MSTP_INFO_MINE)));
            common_info->synced = ((common_info->synced)
								  && (common_info->agreed));
            common_info->updtInfo = MSTP_FALSE;
            common_info->infoIs = MSTP_INFO_MINE;
			if(0 == instance_id) /*CIST*/
			{
				mstp_cist_port->port_priority = mstp_cist_port->designated_priority;
				mstp_cist_port->port_times	= mstp_cist_port->designated_times;
				mstp_cist_port->new_info = MSTP_TRUE;
			}
			else /*MSTI*/
			{
				mstp_msti_port->port_priority = mstp_msti_port->designated_priority;
				mstp_msti_port->port_times	= mstp_msti_port->designated_times;

				port_variables->newInfoMsti	= MSTP_TRUE;
			}
		}
			break;
		case MSTP_CURRENT:
		{
			/*do nothing*/
		}
			break;
		case MSTP_RECEIVE:
		{
/*rcvdInfo = rcvInfoXst();
recordMasteredXst();*/	
            common_info->rcvdInfo = mstp_recv_info(port, instance_id);
            mstp_record_mastered(port, instance_id);
		}
			break;
		case MSTP_OTHER:
		{
/*rcvdMsg = FALSE*/			
			common_info->rcvdMsg = MSTP_FALSE;
		}
			break;
		case MSTP_SUPERIOR_DESIGNATED:
		{
/*infoInternal = rcvdInternal;
agreed = proposing = FALSE;
recordProposal();
setTcFlags();
agree = agree && betterorsameInfo(Received); 
recordAgreement();
synced = synced && agreed;
recordPriority();
recordTimes();
updtRcvdInfoWhile();

infoIs = Received;
reselect = TRUE;
selected = FALSE; 
rcvdMsg = FALSE;
*/
			common_info->rootPtState  = MSTP_FALSE;		/*may be need to open*/
			port_variables->infoInternal = port_variables->rcvdInternal;
			common_info->agreed = MSTP_FALSE;
			common_info->proposing = MSTP_FALSE;
			mstp_record_proposal(port, instance_id);
			mstp_set_tc_flags(port, instance_id);
			common_info->agree = common_info->agree && mstp_better_or_same_info(port, instance_id, MSTP_INFO_RECEIVED);
			mstp_record_agreement(port, instance_id);
			common_info->synced = ((common_info->synced) && (common_info->agreed));

			mstp_record_priority(port, instance_id);
			mstp_record_times(port, instance_id);
			//mstp_updt_rcvd_info_while(port, instance_id);

			common_info->infoIs = MSTP_INFO_RECEIVED;
			common_info->reselect = MSTP_TRUE;
			common_info->selected = MSTP_FALSE;			
			common_info->rcvdMsg = MSTP_FALSE;
			mstp_updt_rcvd_info_while(port, instance_id);
		}
			break;
		case MSTP_REPEATED_DESIGNATED:
		{
/*
infoInternal = rcvdInternal;
recordProposal();
setTcFlags();
recordAgreement();
updtRcvdInfoWhile(); 
rcvdMsg = FALSE;
*/
			port_variables->infoInternal = port_variables->rcvdInternal;
            mstp_record_proposal(port, instance_id);
            mstp_set_tc_flags(port, instance_id);
            mstp_record_agreement(port, instance_id);
            mstp_updt_rcvd_info_while(port, instance_id);
            common_info->rcvdMsg = MSTP_FALSE;
		}
			break;

        case MSTP_INFERIOR_DESIGNATED:
		{
/*need to confirm*/

/*recordDispute();
rcvdMsg = FALSE;*/

			mstp_record_dispute(port, instance_id);
			common_info->rcvdMsg = MSTP_FALSE;

			/*If the replacement port or root port received a low priority port issued
			a message, you need to re-select the role of this port */
			if(common_info->role == MSTP_PORT_ROLE_ALTERNATE)
			{
				common_info->disputed 	= MSTP_FALSE;
				common_info->updtInfo	= MSTP_TRUE;
				common_info->selectedRole = MSTP_PORT_ROLE_DESIGNATED;
				common_info->selected	= MSTP_TRUE;
				common_info->agreed		= MSTP_FALSE;
				common_info->sync		= MSTP_FALSE;
				common_info->synced		= MSTP_TRUE;
			}
			else if(common_info->role == MSTP_PORT_ROLE_ROOT)
			{
				common_info->infoIs		= MSTP_INFO_AGED;
				common_info->reselect	= MSTP_TRUE;
				common_info->selected	= MSTP_FALSE;
				common_info->agreed		= MSTP_FALSE;
				common_info->synced		= MSTP_FALSE;
			}

			common_info->disputed = MSTP_FALSE;
		}
			break;		
		case MSTP_NOT_DESIGNATED:
		{
/*
recordAgreement();
setTcFlags();
rcvdMsg = FALSE;
*/
			mstp_record_agreement(port, instance_id);
			mstp_set_tc_flags(port, instance_id);
			common_info->rcvdMsg = MSTP_FALSE;
		}
			break;

		default:
			break;
	}

	return;
}

/*Figure 13-15 Port Role Selection*/
void mstp_port_role_selection_sm(struct mstp_bridge *mstp_bridge, unsigned int instance_id)
{
	unsigned int				port_role_selection_last_state = 0;
	unsigned int				*port_role_selection_cur_state = NULL;
	unsigned int				*begin_flag = NULL;

	struct mstp_cist_port  		*cist_port = NULL, *next_cist_port = NULL;
	struct mstp_msti_port		*msti_port = NULL, *next_msti_port = NULL;


	if(NULL == mstp_bridge)
	{
		return;
	}

    if(0 == instance_id) /*CIST*/
    {
		port_role_selection_cur_state = &(mstp_bridge->cist_br.port_role_select_state);
		begin_flag = &mstp_bridge->cist_br.port_role_select_state_begin;
    }
    else /*MSTI*/
    {
		port_role_selection_cur_state = &(mstp_bridge->msti_br[instance_id-1]->port_role_select_state);
		begin_flag = &mstp_bridge->msti_br[instance_id-1]->port_role_select_state_begin;
    }

	port_role_selection_last_state = *port_role_selection_cur_state;


	if(*begin_flag)
	{
		*port_role_selection_cur_state = MSTP_INIT_BRIDGE;
		*begin_flag = MSTP_FALSE;
	}
	else if(MSTP_INIT_BRIDGE == *port_role_selection_cur_state)
	{
		*port_role_selection_cur_state = MSTP_ROLE_SELECTION;
	}
	else if(MSTP_ROLE_SELECTION == *port_role_selection_cur_state)
	{
		if(0 == instance_id)	/*cist*/
		{
			/*all cist ports*/
            list_for_each_entry_safe (cist_port, next_cist_port, &mstp_bridge->cist_br.port_head, port_list)
            {
				if(cist_port->common_info.reselect)
				{
					mstp_clear_reselect_tree(mstp_bridge, instance_id);
					mstp_updt_roles_tree(mstp_bridge, instance_id);
					mstp_set_selected_tree(mstp_bridge, instance_id);
					*port_role_selection_cur_state = MSTP_ROLE_SELECTION;

					break;
				}
			}
		}
		else/*msti*/
		{
			/*all msti ports*/
			list_for_each_entry_safe(msti_port, next_msti_port, &mstp_bridge->msti_br[instance_id-1]->port_head, port_list)
			{
				if(msti_port->common_info.reselect)
				{
					mstp_clear_reselect_tree(mstp_bridge, instance_id);
					mstp_updt_roles_tree(mstp_bridge, instance_id);
					mstp_set_selected_tree(mstp_bridge, instance_id);
					*port_role_selection_cur_state = MSTP_ROLE_SELECTION;

					break;
				}
			}
		}
	}

	/*state no change*/
	if(port_role_selection_last_state == *port_role_selection_cur_state)
	{
		return;
	}

	switch(*port_role_selection_cur_state)
	{
		case MSTP_INIT_BRIDGE:
		{
/*updtRolesDisabledTree();*/
			mstp_updt_role_disabled_tree(mstp_bridge, instance_id);
		}
			break;
		case MSTP_ROLE_SELECTION:
		{
/*clearReselectTree();
updtRolesTree();
setSelectedTree();*/
			mstp_clear_reselect_tree(mstp_bridge, instance_id);
			mstp_updt_roles_tree(mstp_bridge, instance_id);
			mstp_set_selected_tree(mstp_bridge, instance_id);
		}
			break;
		default:
			break;
	}
}


/*Figure 13-16 Port Role Transitions state machine?~{
~}
Part 1: Disabled, Alternate, and Backup Roles*/
void mstp_port_role_trans_disabled_sm(struct mstp_port* port, unsigned int instance_id)
{
	unsigned int				port_role_trans_disabled_last_state = 0;
	unsigned int				port_role_trans_disabled_change_flag = MSTP_FALSE;

	struct mstp_common_port		*common_info = NULL;
	struct mstp_common_br		*common_bridge = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return;
	}

	common_bridge = &(port->m_br->common_br);

	if(0 == instance_id) /*CIST*/
	{
		common_info = &(port->cist_port.common_info);
	}
	else /*MSTI*/
	{
		if (NULL == port->msti_port[instance_id-1])
		{
			return;
		}
		
		common_info = &(port->msti_port[instance_id-1]->common_info);
    }

    port_role_trans_disabled_last_state = common_info->sm_disable_role_state;


/*All transtions, except UCT,
are qualified by:
"&& selected && !updtInfo".*/
//	if(common_bridge->begin)
	if(common_info->sm_disable_role_state_begin)
	{
	    port_role_trans_disabled_last_state = 0;
		common_info->sm_disable_role_state = MSTP_INIT_PORT;
		common_info->sm_disable_role_state_begin = MSTP_FALSE;
	}
	else if(MSTP_INIT_PORT == common_info->sm_disable_role_state)/*UCT*/
	{
		common_info->sm_disable_role_state = MSTP_DISABLE_PORT;
	}
	else if((MSTP_DISABLE_PORT == common_info->sm_disable_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/* !learning
&& !forwarding*/		
		&& (!common_info->learning)
		&& (!common_info->forwarding))
	{
		common_info->sm_disable_role_state = MSTP_DISABLED_PORT;
	}
	else if((MSTP_DISABLED_PORT == common_info->sm_disable_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*
(fdWhile != MaxAge) 
||sync || reRoot || !synced
*/
		&& ((common_info->fdWhile != common_bridge->max_age(port))
			|| (common_info->sync)
			|| (common_info->reRoot)
			|| (!common_info->synced)))
	{
		port_role_trans_disabled_change_flag = MSTP_TRUE;
		common_info->sm_disable_role_state = MSTP_DISABLED_PORT;
	}
	else if(
/*((selectedRole == DisabledPort)
&& (role != selectedRole)*/		
		(MSTP_PORT_ROLE_DISABLED == common_info->selectedRole)
		&& (common_info->role != common_info->selectedRole)
		&& (common_info->selected)
		&& (!common_info->updtInfo))
    {
        port_role_trans_disabled_last_state = 0;
		common_info->sm_disable_role_state = MSTP_DISABLE_PORT;
    }


	/*state no change*/
	if((port_role_trans_disabled_last_state == common_info->sm_disable_role_state)
		&& (MSTP_FALSE == port_role_trans_disabled_change_flag))
	{
		return;
	}

	switch(common_info->sm_disable_role_state)
	{
		case MSTP_INIT_PORT:
		{
/*role = DisabledPort;
learn = forward = FALSE;
synced = FALSE;
sync = reRoot = TRUE;
rrWhile = FwdDelay;
fdWhile = MaxAge;
rbWhile = 0;*/			
			common_info->role		= MSTP_PORT_ROLE_DISABLED;
            common_info->learn   	= common_info->forward = MSTP_FALSE;
            common_info->synced  	= MSTP_FALSE;
            common_info->sync    	= common_info->reRoot = MSTP_TRUE;
			common_info->rrWhile 	= common_bridge->fwd_delay(port);
			common_info->fdWhile 	= common_bridge->max_age(port);
			common_info->rbWhile 	= 0;
		}
			break;
		case MSTP_DISABLE_PORT:
		{
/*role = selectedRole;
learn= forward = FALSE;*/

			common_info->role = common_info->selectedRole;
			common_info->learn = MSTP_FALSE;
			common_info->forward = MSTP_FALSE;
		}
			break;
		case MSTP_DISABLED_PORT:
		{
/*
fdWhile = MaxAge;
synced = TRUE;
rrWhile = 0; 
sync = reRoot = FALSE;
*/
//printf("MSTP_DISABLED_PORT before fdWhile = %d\n", common_info->fdWhile);
			common_info->fdWhile = common_bridge->max_age(port);
//printf("MSTP_DISABLED_PORT	fdWhile = %d\n", common_info->fdWhile);
			common_info->synced = MSTP_TRUE;
			common_info->rrWhile = 0;
			common_info->sync = MSTP_FALSE;
			common_info->reRoot = MSTP_FALSE;

		}
			break;
		default:
			break;			
	}

	return;
}

/*Figure 13-16 Port Role Transitions state machine?~{
~}
Part 1: Disabled, Alternate, and Backup Roles*/
void mstp_port_role_trans_alternate_backup_sm(struct mstp_port* port, unsigned int instance_id)
{
/*ab:alternate & backup*/
	unsigned int				port_role_trans_ab_last_state = 0;
	unsigned int				port_role_trans_ab_change_flag = MSTP_FALSE;

	struct mstp_common_port		*common_info = NULL;
	struct mstp_common_br		*common_bridge = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return;
	}

	common_bridge = &(port->m_br->common_br);

    if(0 == instance_id)
    {
        common_info = &port->cist_port.common_info;
    }
    else
    {
    	if (NULL == port->msti_port[instance_id-1])
    	{
			return;
    	}
		
        common_info = &port->msti_port[instance_id-1]->common_info;
    }

	port_role_trans_ab_last_state = common_info->sm_ab_role_state;


/*All transtions, except UCT, are qualified by "&& selected && !updtInfo".*/

/*((selectedRole == AlternatePort) || (selectedRole == BackupPort))
&& (role !=selectedRole)*/
	if(((MSTP_PORT_ROLE_ALTERNATE == common_info->selectedRole)
			|| (MSTP_PORT_ROLE_BACKUP == common_info->selectedRole))
		&& (common_info->role != common_info->selectedRole)
		&& (common_info->selected)
		&& (!common_info->updtInfo))
	{
		port_role_trans_ab_last_state = 0;
		/*modify by youcheng 2018/8/16*/
#if 1	/*change port forward delay info, but in the rfc do not need this*/
		//common_info->fdWhile = common_bridge->forward_delay(port);
		common_info->fdWhile = common_bridge->fwd_delay(port);
#endif
		common_info->sm_ab_role_state = MSTP_BLOCK_PORT;
	}
	else if((MSTP_BLOCK_PORT == common_info->sm_ab_role_state)
/* !learning
&& !forwarding*/		
		&& (!common_info->learning)
		&& (!common_info->forwarding)
		&& (common_info->selected)
		&& (!common_info->updtInfo))
	{
		common_info->sm_ab_role_state = MSTP_ALTERNATE_PORT;
	}
	else if((MSTP_ALTERNATE_PORT == common_info->sm_ab_role_state)	
/*(fdWhile != forwardDelay) || sync || reRoot || !synced*/		
		&& ((common_info->fdWhile != common_bridge->forward_delay(port))
			|| (common_info->sync)
			|| (common_info->reRoot)
			|| (!common_info->synced))
		&& (common_info->selected)
		&& (!common_info->updtInfo))
	{
		port_role_trans_ab_change_flag = MSTP_TRUE;
		common_info->sm_ab_role_state = MSTP_ALTERNATE_PORT;
	}
	else if((MSTP_ALTERNATE_PORT == common_info->sm_ab_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*(rbWhile != 2*HelloTime)
&&(role == BackupPort)*/		
		&& (common_info->rbWhile != 2*(common_bridge->hello_time(port)))
		&& (MSTP_PORT_ROLE_BACKUP == common_info->role))
	{
		common_info->sm_ab_role_state = MSTP_BACKUP_PORT;
	}
	else if((MSTP_ALTERNATE_PORT == common_info->sm_ab_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*
(allSynced && !agree) 
||(proposed && agree)
*/
		&& ((common_bridge->all_synced(port, instance_id) && (!common_info->agree))
			||((common_info->proposed) && (common_info->agree))))		
	{
		common_info->sm_ab_role_state = MSTP_ALTERNATE_AGREED;
	}
	else if((MSTP_ALTERNATE_PORT == common_info->sm_ab_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*proposed && !agree*/		
		&& (common_info->proposed)
		&& (!common_info->agree))
	{		
		common_info->sm_ab_role_state = MSTP_ALTERNATE_PROPOSED;
	}
	else if((MSTP_BACKUP_PORT == common_info->sm_ab_role_state)
		|| (MSTP_ALTERNATE_AGREED == common_info->sm_ab_role_state)
		|| (MSTP_ALTERNATE_PROPOSED == common_info->sm_ab_role_state))
	{
		common_info->sm_ab_role_state = MSTP_ALTERNATE_PORT;
	}

	/*state no change*/
	if((port_role_trans_ab_last_state == common_info->sm_ab_role_state)
		&& (MSTP_FALSE == port_role_trans_ab_change_flag))
	{
		return;
	}


	switch(common_info->sm_ab_role_state)
	{
		case MSTP_BLOCK_PORT:
		{
/*role = selectedRole;
learn= forward = FALSE;*/		
			common_info->role = common_info->selectedRole;
			common_info->learn = MSTP_FALSE;
			common_info->forward = MSTP_FALSE;
		}
			break;
		case MSTP_ALTERNATE_PORT:
		{
/*	
fdWhile = forwardDelay;
synced = TRUE;
rrWhile = 0;
sync = reRoot = FALSE;*/

			common_info->fdWhile = common_bridge->fwd_delay(port);
			common_info->synced = MSTP_TRUE;
			common_info->rrWhile = 0;
			common_info->sync = MSTP_FALSE;
			common_info->reRoot = MSTP_FALSE;			
		}
			break;
		case MSTP_BACKUP_PORT:
		{
/*rbWhile = 2*HelloTime;*/			
			common_info->rbWhile = 2*(common_bridge->hello_time(port));
		}
			break;	
		case MSTP_ALTERNATE_AGREED:
		{
/*proposed = FALSE;
agree = TRUE;
newInfoXst = TRUE;*/
			common_info->proposed = MSTP_FALSE;
			common_info->agree = MSTP_TRUE;
			if(instance_id == 0)
            {
                port->cist_port.new_info = MSTP_TRUE;
            }
            else
            {            
                port->port_variables.newInfoMsti = MSTP_TRUE;
            }
		}
			break;
		case MSTP_ALTERNATE_PROPOSED:
		{
/*setSyncTree();
proposed = FALSE;*/
			mstp_set_sync_tree(port, instance_id);
            common_info->proposed = MSTP_FALSE;
		}
			break;

		default:
			break;					
	}

	return;
}

/*Figure 13-17 Port Role Transitions state machine?~{
~}
Part 2: Root Roles*/
void mstp_port_role_trans_root_sm(struct mstp_port* port, unsigned int instance_id)
{
	unsigned int				port_role_trans_root_last_state = 0;

	struct mstp_common_port		*common_info = NULL;
	struct mstp_common_br		*common_bridge = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return;
	}

	common_bridge = &(port->m_br->common_br);

    if(0 == instance_id)
    {
        common_info = &port->cist_port.common_info;
    }
    else
    {
    	if (NULL == port->msti_port[instance_id-1])
    	{
			return;
		}

        common_info = &port->msti_port[instance_id-1]->common_info;
    }

	port_role_trans_root_last_state = common_info->sm_root_role_state;

/*All transitions, except UCT, are qualified by "&& selected && !updtInfo".*/
/*
(selectedRole == RootPort) && 
(role !=selectedRole)*/
	if((MSTP_PORT_ROLE_ROOT == common_info->selectedRole)
		&& (common_info->role != common_info->selectedRole)
		&& (common_info->selected)
		&& (!common_info->updtInfo))
	{
		port_role_trans_root_last_state = 0;
		/*modify by youcheng 2018/8/16*/
#if 1	/*change port forward delay info, but in the rfc do not need this*/
		//common_info->fdWhile = common_bridge->forward_delay(port);
		common_info->fdWhile = common_bridge->fwd_delay(port);
#endif		
		common_info->sm_root_role_state = MSTP_ROOT_PORT;
	}
	else if((MSTP_ROOT_PORT == common_info->sm_root_role_state)
/*rrWhile != FwdDelay*/
		&& (common_info->rrWhile != common_bridge->fwd_delay(port))
		&& (common_info->selected)
		&& (!common_info->updtInfo))
	{
	/*role = RootPort; 
rrWhile = FwdDelay;*/
		common_info->role = MSTP_PORT_ROLE_ROOT;
		common_info->rrWhile = common_bridge->fwd_delay(port);
		common_info->sm_root_role_state = MSTP_ROOT_PORT;
	}
	else if((MSTP_ROOT_PORT == common_info->sm_root_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*reRoot && forward*/	
		&& (common_info->reRoot)
		&& (common_info->forward))
	{
		common_info->sm_root_role_state = MSTP_REROOTED;
	}
	else if((MSTP_ROOT_PORT == common_info->sm_root_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*((fdWhile == 0) 
	|| (reRooted && (rbWhile == 0) && rstpVersion)) 
&& !learn*/		
		&& (!common_info->learn)
		&& ((0 == common_info->fdWhile)
			|| ((common_bridge->rerooted(port, instance_id))
				&& (0 == common_info->rbWhile)
				&& (common_bridge->rstp_version(port)))))
	{
		common_info->sm_root_role_state = MSTP_ROOT_LEARN;
	}
	else if((MSTP_ROOT_PORT == common_info->sm_root_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*((fdWhile == 0)
	|| (reRooted && (rbWhile == 0) && rstpVersion))
&& learn
&& !forward*/
		&& (common_info->learn)
		&& (!common_info->forward)
		&& ((0 == common_info->fdWhile)
			|| ((common_bridge->rerooted(port, instance_id))
				&& (0 == common_info->rbWhile)
				&& (common_bridge->rstp_version(port)))))
	{
		common_info->sm_root_role_state = MSTP_ROOT_FORWARD;
	}
	else if((MSTP_ROOT_PORT == common_info->sm_root_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*
!forward 
&& (rbWhile == 0)
&& !reRoot
*/
		&& (!common_info->forward)
		&& (!common_info->reRoot))
	{
		common_info->sm_root_role_state = MSTP_REROOT;
	}
	else if((MSTP_ROOT_PORT == common_info->sm_root_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*(agreed && !synced) ||
(sync && synced) */
		&& (((common_info->agreed) && (!common_info->synced))
			|| ((common_info->sync) && (common_info->synced))))
	{
		common_info->sm_root_role_state = MSTP_ROOT_SYNCED;
	}
	else if((MSTP_ROOT_PORT == common_info->sm_root_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*(allSynced && !agree) ||
(proposed && agree)*/
		&& (((common_bridge->all_synced(port, instance_id)) && (!common_info->agree))
			|| ((common_info->proposed) && (common_info->agree))))
	{
		common_info->sm_root_role_state = MSTP_ROOT_AGREED;
	}
	else if((MSTP_ROOT_PORT == common_info->sm_root_role_state)		
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*proposed && !agree*/
		&& (common_info->proposed)
		&& (!common_info->agree))
	{
		common_info->sm_root_role_state = MSTP_ROOT_PROPOSED;
	}
	else if((MSTP_ROOT_PROPOSED == common_info->sm_root_role_state)
		|| (MSTP_ROOT_AGREED == common_info->sm_root_role_state)
		|| (MSTP_ROOT_SYNCED == common_info->sm_root_role_state)
		|| (MSTP_REROOT == common_info->sm_root_role_state )
		|| (MSTP_ROOT_FORWARD == common_info->sm_root_role_state)
		|| (MSTP_ROOT_LEARN == common_info->sm_root_role_state)
		|| (MSTP_REROOTED == common_info->sm_root_role_state))
	{
		common_info->sm_root_role_state = MSTP_ROOT_PORT;		
	}
	
	/*state no change*/
	if(port_role_trans_root_last_state == common_info->sm_root_role_state)
	{
		return;
	}

	switch(common_info->sm_root_role_state)
	{
		case MSTP_ROOT_PORT:
		{
/*role = RootPort; 
rrWhile = FwdDelay;*/
			common_info->role = MSTP_PORT_ROLE_ROOT;
			common_info->rrWhile = common_bridge->fwd_delay(port);
		}
			break;
		case MSTP_REROOTED:
		{
/*reRoot = FALSE;*/
			common_info->reRoot = MSTP_FALSE;
		}
			break;
		case MSTP_ROOT_LEARN:
		{
/* fdWhile= forwardDelay;
 learn = TRUE;*/
 			common_info->fdWhile = common_bridge->fwd_delay(port);
			common_info->learn = MSTP_TRUE;
		}
			break;
		case MSTP_ROOT_FORWARD:
		{
/*forward = TRUE;
fdWhile = 0;*/
			common_info->forward = MSTP_TRUE;
			common_info->fdWhile = 0;
//printf("MSTP_ROOT_FORWARD fdWhile = 0\n");
		
		}
			break;
		case MSTP_REROOT:
		{
/*setReRootTree();*/
			mstp_set_reroot_tree(port, instance_id);
		}
			break;
		case MSTP_ROOT_SYNCED:
		{
/*synced = TRUE; 
sync = FALSE;*/
			common_info->synced = MSTP_TRUE;
			common_info->sync = MSTP_FALSE;
		}
			break;
		case MSTP_ROOT_AGREED:
		{
/*proposed = sync = FALSE;
agree = TRUE; 
newInfoXst = TRUE;*/
			common_info->proposed = MSTP_FALSE;
			common_info->sync = MSTP_FALSE;
			common_info->agree = MSTP_TRUE;

            if(0 == instance_id)
            {
                port->cist_port.new_info = MSTP_TRUE;
            }
            else
            {            
                port->port_variables.newInfoMsti = MSTP_TRUE;
            }
		}
			break;
		case MSTP_ROOT_PROPOSED:
		{
/*setSyncTree();
proposed = FALSE;*/
			mstp_set_sync_tree(port, instance_id);
			common_info->proposed = MSTP_FALSE;
		}
			break;
		default:
			break;
	}

	return;
}

/*Figure 13-17 Port Role Transitions state machine
Part 2: Root, Designated, and Master Roles*/
void mstp_port_role_trans_designated_sm(struct mstp_port* port, unsigned int instance_id)
{
	unsigned int	port_role_trans_designated_last_state = 0;

	struct mstp_common_port		*common_info = NULL;
	struct mstp_common_br		*common_bridge = NULL;
	struct mstp_port_variables	*port_variables = NULL;
	//struct mstp_cist_port  		*cist_port = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return;
	}

	common_bridge = &(port->m_br->common_br);
	port_variables = &(port->port_variables);

    if(0 == instance_id)	/*cist*/
    {
        common_info = &port->cist_port.common_info;
		//cist_port = &port->cist_port;
    }
    else	/*msti*/
    {
    	if (NULL == port->msti_port[instance_id-1])
    	{
			return;
		}

        common_info = &port->msti_port[instance_id-1]->common_info;
    }

	port_role_trans_designated_last_state = common_info->sm_deg_role_state;

/*All transitions, except UCT, are qualified by "&& selected && !updtInfo".*/

/*(selectedRole == DesignatedPort) 
&& (role != selectedRole)*/
	if((MSTP_PORT_ROLE_DESIGNATED == common_info->selectedRole)
		&& (common_info->role != common_info->selectedRole)
		&& (common_info->selected)
		&& (!common_info->updtInfo))
	{
		port_role_trans_designated_last_state = 0;
		/*modify by youcheng 2018/8/16*/
	/*change port forward delay info, but in the rfc do not need this*/
		//common_info->fdWhile = common_bridge->forward_delay(port);
		/*bug#52837 time is false <rstp>指定端口收不到A置位的报文，仍快速进入forwarding状态*/
		common_info->fdWhile = common_bridge->fwd_delay(port);

//printf("selectedRole == MSTP_PORT_ROLE_DESIGNATED\n	fdWhile= %d\n", common_info->fdWhile);
		common_info->sm_deg_role_state = MSTP_DESIGNATED_PORT;
	}
	else if((MSTP_DESIGNATED_PORT == common_info->sm_deg_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*reRoot && (rrWhile == 0)*/
		&& (common_info->reRoot)
		&& (0 == common_info->rrWhile))
	{
		common_info->sm_deg_role_state = MSTP_DESIGNATED_RETIRED;
	}
	else if((MSTP_DESIGNATED_PORT == common_info->sm_deg_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*((sync && !synced) || (reRoot && (rrWhile != 0)) || disputed || isolate) 
&& !operEdge
&& (learn || forward)*/
		&& ((common_info->learn) || (common_info->forward))
		&& (!port_variables->operEdge)
		&& (((common_info->sync) && (!common_info->synced))
			|| ((common_info->reRoot) && (common_info->rrWhile != 0))
			|| (common_info->disputed)
			/*|| (port_variables->isolate)*/))
	{
		common_info->sm_deg_role_state = MSTP_DESIGNATED_DISCARD;
	}
	else if((MSTP_DESIGNATED_PORT == common_info->sm_deg_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*((fdWhile == 0) || agreed || operEdge) 
&& ((rrWhile ==0) || !reRoot)
&& !sync 
&& !learn 
&& !isolate*/
//need to confirm [reroot]
		&&((0 == common_info->fdWhile) || common_info->agreed || port_variables->operEdge)
		&&((0 == common_info->rrWhile) || !common_bridge->rerooted(port, instance_id))
		&&(!common_info->sync)
		&&(!common_info->learn)
		/*&&(!port_variables->isolate)*/)
	{	
		MSTP_LOG_DBG("%s[%d]#####learn######### edgeDelayWhile = %d\n",\
		__FUNCTION__,__LINE__,port_variables->edgeDelayWhile);
		//FIXME 		!reRoot 	need to confirm
		common_info->sm_deg_role_state = MSTP_DESIGNATED_LEARN;
	}
	else if((MSTP_DESIGNATED_PORT == common_info->sm_deg_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*((fdWhile == 0) || agreed || operEdge)
&& ((rrWhile ==0) || !reRoot)
&& !sync
&& (learn && !forward) 
&& !isolate*/
		&& (common_info->learn)
		&& (!common_info->forward)
		&& ((common_info->fdWhile == 0) || (common_info->agreed) || (port_variables->operEdge))
		&& ((common_info->rrWhile == 0) || (!common_bridge->rerooted(port, instance_id)))
		&& (!common_info->sync)
		/*&& (!port_variables->isolate)*/)
	{
		//FIXME			!reRoot		need to confirm
		MSTP_LOG_DBG("%s[%d]#####forward#########operEdge = %d fdWhile = %d\n",\
		__FUNCTION__,__LINE__,port_variables->operEdge,common_info->fdWhile);
		common_info->sm_deg_role_state = MSTP_DESIGNATED_FORWARD;
	}
	else if((MSTP_DESIGNATED_PORT == common_info->sm_deg_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*(!learning && !forwarding && !synced)
|| (agreed && !synced) 
||(operEdge && !synced)
|| (sync && synced) */
		&& ((!common_info->learning && !common_info->forwarding && !common_info->synced)
			|| (common_info->agreed && !common_info->synced) 
			|| (port_variables->operEdge && !common_info->synced)
			|| (common_info->sync && common_info->synced)))
	{
		common_info->sm_deg_role_state = MSTP_DESIGNATED_SYNCED;
	}
	else if((MSTP_DESIGNATED_PORT == common_info->sm_deg_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*allSynced 
&& (proposed || !agree)*/
		&& (common_bridge->all_synced(port, instance_id))		
		&& ((common_info->proposed) || (!common_info->agree)))
	{
		common_info->sm_deg_role_state = MSTP_DESIGNATED_AGREED;
	}
	else if((MSTP_DESIGNATED_PORT == common_info->sm_deg_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*!forward
&& !agreed 
&& !proposing 
&& !operEdge*/
		&& (!common_info->forward)
		&& (!common_info->agreed)
		&& (!common_info->proposing)
		&& (!port_variables->operEdge))
	{
		common_info->sm_deg_role_state = MSTP_DESIGNATED_PROPOSE;
	}
	else if((MSTP_DESIGNATED_PROPOSE == common_info->sm_deg_role_state)
		|| (MSTP_DESIGNATED_AGREED == common_info->sm_deg_role_state)
		|| (MSTP_DESIGNATED_SYNCED == common_info->sm_deg_role_state)
		|| (MSTP_DESIGNATED_RETIRED == common_info->sm_deg_role_state)
		|| (MSTP_DESIGNATED_FORWARD == common_info->sm_deg_role_state)
		|| (MSTP_DESIGNATED_LEARN == common_info->sm_deg_role_state)
		|| (MSTP_DESIGNATED_DISCARD == common_info->sm_deg_role_state))
	{
		common_info->sm_deg_role_state = MSTP_DESIGNATED_PORT;
	}

	/*state no change*/
	if(port_role_trans_designated_last_state == common_info->sm_deg_role_state)
	{
		return;
	}

	switch(common_info->sm_deg_role_state)
	{
		case MSTP_DESIGNATED_PORT:
		{
/*role = DesignatedPort; 
if (cist) 
{ proposing = proposing || (!AdminEdge && !AutoEdge && AutoIsolate && operPointToPoint); 
}*/
			common_info->role = MSTP_PORT_ROLE_DESIGNATED;
			/*bug#52884 bug#52835
			<rstp>P/A机制必须在点到点链路上进行
			<rstp>根端口down，指定端口收到更优rst bpdu，经过3个hello time才变为新的根端口*/
			
			if(&port->cist_port)
			{
				common_info->proposing = common_info->proposing ||(!port_variables->AdminEdge && 
				!port_variables->AutoEdge && port_variables->isolate && port_variables->operPointToPointMAC);
			}
		}
			break;
		case MSTP_DESIGNATED_RETIRED:
		{
/*reRoot = FALSE;*/
            common_info->reRoot = MSTP_FALSE;
		}
			break;
		case MSTP_DESIGNATED_DISCARD:
		{
/*learn = forward = disputed  = FALSE;
fdWhile = forwardDelay;*/		
			common_info->learn = MSTP_FALSE;
			common_info->forward = MSTP_FALSE;
			common_info->disputed = MSTP_FALSE;
            //common_info->fdWhile = common_bridge->forward_delay(port);
            /*bug#52837 forward delay is false*/
            common_info->fdWhile = common_bridge->fwd_delay(port);
//printf("MSTP_DESIGNATED_DISCARD fdWhile = %d\n", common_info->fdWhile);
			
		}
			break;
		case MSTP_DESIGNATED_LEARN:
		{
/*learn = TRUE;
fdWhile= forwardDelay;*/
			common_info->learn = MSTP_TRUE;
			 /*bug#52837 forward delay is false*/
			common_info->fdWhile = common_bridge->fwd_delay(port);
		}
			break;
		case MSTP_DESIGNATED_FORWARD:
		{
/*forward = TRUE;
fdWhile = 0;
agreed = sendRSTP;*/			
			common_info->forward = MSTP_TRUE;
			common_info->fdWhile = 0;
//printf("MSTP_DESIGNATED_FORWARD fdWhile = 0\n");

            common_info->agreed = port_variables->sendRSTP;
            common_info->proposing = MSTP_FALSE;
		}
			break;
		case MSTP_DESIGNATED_SYNCED:
		{
/*rrWhile = 0;
synced = TRUE; 
sync = FALSE;*/			
			common_info->rrWhile = 0;
            common_info->synced = MSTP_TRUE;
			common_info->sync = MSTP_FALSE;
		}
			break;			
		case MSTP_DESIGNATED_AGREED:
		{
/*proposed = sync = FALSE;
agree = TRUE;
newInfoXst = TRUE;*/
			common_info->proposed = MSTP_FALSE;
			common_info->sync = MSTP_FALSE;
            common_info->agree = MSTP_TRUE;

			if(0 == instance_id)
            {
                port->cist_port.new_info = MSTP_TRUE;
            }
            else
            {            
                port_variables->newInfoMsti = MSTP_TRUE;
            }
		}
			break;
		case MSTP_DESIGNATED_PROPOSE:
		{
/*proposing = TRUE;
if (cist) { edgeDelayWhile = EdgeDelay; }
newInfoXst = TRUE;*/
			common_info->proposing = MSTP_TRUE;
			if (common_bridge->cist(instance_id))
            {
				port_variables->edgeDelayWhile = common_bridge->pdu_older(port);
			}

			if(0 == instance_id)
            {
                port->cist_port.new_info = MSTP_TRUE;
            }
            else
            {            
                port_variables->newInfoMsti = MSTP_TRUE;
            }
		}
			break;
		default:
			break;
	}

	return;
}

/*Figure 13-17 Port Role Transitions state machine?~{
~}
Part 2: Root, Designated, and Master Roles
master role status~~Figure 13-23?~{P~}ort Role Transitions state machine -MasterPort(802.1Q-2011 )
*/
void mstp_port_role_trans_master_sm(struct mstp_port* port, unsigned int instance_id)
{	
	unsigned int				port_role_trans_master_last_state = 0;

	struct mstp_common_port		*common_info = NULL;
	struct mstp_common_br		*common_bridge = NULL;
	struct mstp_port_variables	*port_variables = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return;
	}

	common_bridge = &(port->m_br->common_br);
	port_variables = &(port->port_variables);

    if(0 == instance_id)
    {
        common_info = &port->cist_port.common_info;
    }
    else
    {
    	if (NULL == port->msti_port[instance_id-1])
    	{
			return;
		}

        common_info = &port->msti_port[instance_id-1]->common_info;
    }	

	port_role_trans_master_last_state = common_info->sm_master_role_state;


/*All transitions, except UCT, are qualified by "&& selected && !updtInfo".*/

	if((MSTP_PORT_ROLE_MASTER == common_info->selectedRole)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*(role !=selectedRole)*/		
		&& (common_info->role != common_info->selectedRole))
	{
		port_role_trans_master_last_state = 0;
		/*modify by youcheng 2018/8/16*/
#if 1	/*change port forward delay info, but in the rfc do not need this*/
		//common_info->fdWhile = common_bridge->forward_delay(port);
		common_info->fdWhile = common_bridge->fwd_delay(port);
#endif		
		common_info->sm_master_role_state = MSTP_MASTER_PORT;
	}
	else if((MSTP_MASTER_PORT == common_info->sm_master_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*reRoot
&&(rrWhile == 0)*/
		&& (common_info->reRoot)
		&& (common_info->rrWhile == 0))
	{
		common_info->sm_master_role_state = MSTP_MASTER_RETIRED;
	}
	else if((MSTP_MASTER_PORT == common_info->sm_master_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*((sync && !synced) || (reRoot && (rrWhile != 0)) || disputed)
&& !operEdge
&& (learn || forward)*/		
		&& (common_info->learn || common_info->forward)
		&& (!port_variables->operEdge)
		&& ((common_info->sync && !common_info->synced)
			|| (common_info->reRoot && common_info->rrWhile != 0) 
			|| common_info->disputed))
	{
		common_info->sm_master_role_state = MSTP_MASTER_DISCARD;
	}
	else if((MSTP_MASTER_PORT == common_info->sm_master_role_state) 
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*((fdWhile == 0) || allSynced)
&& !learn*/
		&& ((!common_info->learn)
			&& ((common_info->fdWhile == 0)
				|| (common_bridge->all_synced(port, instance_id)))))
	{
		common_info->sm_master_role_state = MSTP_MASTER_LEARN;
	}
	else if((MSTP_MASTER_PORT == common_info->sm_master_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*((fdWhile == 0) || allSynced)
&& (learn && !forward)*/
		&& ((common_info->learn)
			&& (!common_info->forward)
			&& ((common_info->fdWhile == 0)
				|| (common_bridge->all_synced(port, instance_id)))))
	{
		common_info->sm_master_role_state = MSTP_MASTER_FORWARD;
	}
	else if((MSTP_MASTER_PORT == common_info->sm_master_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*(!learning && !forwarding && !synced) ||
(agreed && !synced) ||
(operEdge && !synced) ||
(sync && synced) */
		&& ((!common_info->learning && !common_info->forwarding && !common_info->synced)
			|| (common_info->agreed && !common_info->synced)
			|| (port_variables->operEdge && !common_info->synced)
			|| (common_info->sync && common_info->synced)))
	{
		common_info->sm_master_role_state = MSTP_MASTER_SYNCED;
	}
	else if((MSTP_MASTER_PORT == common_info->sm_master_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*(allSynced && !agree) ||
(proposed && agree)*/
		&& ((common_bridge->all_synced(port, instance_id) && !common_info->agree)
			||(common_info->proposed && common_info->agree)))
	{
		common_info->sm_master_role_state = MSTP_MASTER_AGREED;
	}
	else if((MSTP_MASTER_PORT == common_info->sm_master_role_state)
		&& (common_info->selected)
		&& (!common_info->updtInfo)
/*proposed 
&& !agree*/
		&& (common_info->proposed)
		&& (!common_info->agree))
	{
		common_info->sm_master_role_state = MSTP_MASTER_PROPOSED;
	}
	else if((MSTP_MASTER_FORWARD == common_info->sm_master_role_state)
		|| (MSTP_MASTER_LEARN == common_info->sm_master_role_state)
		|| (MSTP_MASTER_DISCARD == common_info->sm_master_role_state)
		|| (MSTP_MASTER_PROPOSED == common_info->sm_master_role_state)
		|| (MSTP_MASTER_AGREED == common_info->sm_master_role_state)
		|| (MSTP_MASTER_SYNCED == common_info->sm_master_role_state)
		|| (MSTP_MASTER_RETIRED == common_info->sm_master_role_state))
	{
		common_info->sm_master_role_state = MSTP_MASTER_PORT;
	}


	/*state no change*/
	if(port_role_trans_master_last_state == common_info->sm_master_role_state)
	{
		return;
	}

	switch(common_info->sm_master_role_state)
	{
		case MSTP_MASTER_PORT:
		{
/*role = selectedRole;*/			
			common_info->role = MSTP_PORT_ROLE_MASTER;
		}
			break;
		case MSTP_MASTER_RETIRED:
		{
/*reRoot = FALSE;*/
			common_info->reRoot = MSTP_FALSE;
		}
			break;
		case MSTP_MASTER_DISCARD:
		{
/*learn = forward = disputed = FALSE;
fdWhile= forwardDelay;*/		
			common_info->learn = MSTP_FALSE;
			common_info->forward = MSTP_FALSE;
			common_info->disputed = MSTP_FALSE;
			//common_info->fdWhile = common_bridge->forward_delay(port);	
			common_info->fdWhile = common_bridge->fwd_delay(port);
//printf("MSTP_MASTER_DISCARD fdWhile = %d\n", common_info->fdWhile);
			
		}
			break;
		case MSTP_MASTER_LEARN:
		{
/*learn = TRUE;
fdWhile= FwdDelay;*/
			common_info->learn = MSTP_TRUE;
			//common_info->fdWhile = common_bridge->forward_delay(port);
			common_info->fdWhile = common_bridge->fwd_delay(port);
//printf("MSTP_MASTER_LEARN fdWhile = %d\n", common_info->fdWhile);
			
		}
			break;
		case MSTP_MASTER_FORWARD:
		{
/*forward = TRUE; fdWhile = 0;
agreed = sendRSTP;*/			
			common_info->forward = MSTP_TRUE;
			common_info->fdWhile = 0;
//printf("MSTP_MASTER_FORWARD fdWhile = 0\n");

            common_info->agreed = port_variables->sendRSTP;
		}
			break;
		case MSTP_MASTER_SYNCED:
		{
/*rrWhile = 0;
synced = TRUE; 
sync = FALSE;*/			
			common_info->rrWhile = 0;
            common_info->synced = MSTP_TRUE;
			common_info->sync = MSTP_FALSE;		
		}
			break;
		case MSTP_MASTER_AGREED:
		{
/*proposed = sync = FALSE;
agree = TRUE;*/
			common_info->proposed = MSTP_FALSE;
			common_info->sync = MSTP_FALSE;
            common_info->agree = MSTP_TRUE;
		}
			break;
		case MSTP_MASTER_PROPOSED:
		{
/*setSyncTree();
proposed = FALSE;*/
			mstp_set_sync_tree(port, instance_id); 
			common_info->proposed = MSTP_FALSE;			
		}
			break;				
		default:
			break;
	}

	return;
}

/*Port Role Transitions state machine*/
void mstp_port_role_trans_sm(struct mstp_port* port, unsigned int instance_id)
{
	/**/
	struct mstp_common_port		*common_info = NULL;

	if (NULL == port)
	{
		return;
	}

    if(0 == instance_id) /*cist*/
    {
        common_info = &port->cist_port.common_info;
    }
    else /*msti*/
    {
    	if (NULL == port->msti_port[instance_id-1])
    	{
			return;
		}

        common_info = &port->msti_port[instance_id-1]->common_info;
    }	


	switch(common_info->selectedRole)
	{
		case MSTP_PORT_ROLE_DISABLED:
		{
			mstp_port_role_trans_disabled_sm(port, instance_id);
		}
			break;
		case MSTP_PORT_ROLE_ALTERNATE:
		case MSTP_PORT_ROLE_BACKUP:
		{
			mstp_port_role_trans_alternate_backup_sm(port, instance_id);
		}
			break;
		case MSTP_PORT_ROLE_ROOT:
		{
			mstp_port_role_trans_root_sm(port, instance_id);
		}
			break;
		case MSTP_PORT_ROLE_DESIGNATED:
		{
			mstp_port_role_trans_designated_sm(port, instance_id);
		}
			break;
		case MSTP_PORT_ROLE_MASTER:
		{
			mstp_port_role_trans_master_sm(port, instance_id);
		}
			break;
		default:
			break;
	}

	return;
}

/*Figure 13-18 Port State Transition state machine*/
void mstp_port_state_trans_sm(struct mstp_port *port, unsigned int instance_id)
{
	unsigned char				port_state_trans_last_state = 0;

	struct mstp_common_port		*common_info = NULL;

	if (NULL == port)
	{
		return;
	}

    if(0 == instance_id)/*CIST*/
    {
        common_info = &(port->cist_port.common_info);
    }
    else /*MSTI*/
    {
    	if (NULL == port->msti_port[instance_id-1])
    	{
			return;
		}

        common_info = &(port->msti_port[instance_id-1]->common_info);
    }

	port_state_trans_last_state = common_info->sm_state_trans_state;

	if(common_info->sm_state_trans_state_begin)
	{
		common_info->sm_state_trans_state = MSTP_ST_DISCARDING;
		common_info->sm_state_trans_state_begin = MSTP_FALSE;
	}
	else if((MSTP_ST_DISCARDING == common_info->sm_state_trans_state)
/* learn*/
		&& (common_info->learn))
	{
		common_info->sm_state_trans_state = MSTP_ST_LEARNING;
	}
	else if((MSTP_ST_LEARNING == common_info->sm_state_trans_state)
/*forward*/
		&& (common_info->forward))
	{
		common_info->sm_state_trans_state = MSTP_ST_FORWARDING;
	}
	else if((MSTP_ST_LEARNING == common_info->sm_state_trans_state)
/* !learn*/
		&& (!common_info->learn))
	{
		common_info->sm_state_trans_state = MSTP_ST_DISCARDING;
	}
	else if((MSTP_ST_FORWARDING == common_info->sm_state_trans_state)
/*!forward*/
		&& (!common_info->forward))
	{
		common_info->sm_state_trans_state = MSTP_ST_DISCARDING;
	}


	/*state no change*/
	if(port_state_trans_last_state == common_info->sm_state_trans_state)
	{
		return;
	}

	switch(common_info->sm_state_trans_state)
	{
		case MSTP_ST_DISCARDING:
		{
/*in rfc -[13.34 chapter]
disableLearning();
learning = FALSE;
disableForwarding();
forwarding = FALSE;*/
			mstp_disable_learning(port, instance_id);
			common_info->learning = MSTP_FALSE;
			mstp_disable_forwarding(port, instance_id);
			common_info->forwarding = MSTP_FALSE;
		}
			break;
		case MSTP_ST_LEARNING:
		{
/*in rfc -[13.34 chapter]
enableLearning();
learning = TRUE;*/		
			mstp_enable_learning(port, instance_id);
			common_info->learning = MSTP_TRUE;
		}
			break;
		case MSTP_ST_FORWARDING:
		{
/*in rfc -[13.34 chapter]
enableForwarding();
forwarding = TRUE;*/

			mstp_enable_forwarding(port, instance_id);
			common_info->forwarding = MSTP_TRUE;		
		}
			break;
		default:
			break;
	}
	
	return;
}

/*Figure 13-19 Topology Change state machine*/
void mstp_port_Topology_change_sm(struct mstp_port *port, unsigned int instance_id)
{
	unsigned char				port_topology_change_last_state = 0;

	struct mstp_common_port		*common_info = NULL;
	struct mstp_common_br		*common_br = NULL;
	struct mstp_port_variables	*port_variables = NULL;

	if ((NULL == port) || (NULL == port->m_br))
	{
		return;
	}

	common_br = &(port->m_br->common_br);
	port_variables = &(port->port_variables);

    if (instance_id == 0) /*CIST*/
    {
        common_info = &(port->cist_port.common_info);
    }
    else /*MSTI*/
    {
        common_info = &(port->msti_port[instance_id-1]->common_info);
    }
			
	port_topology_change_last_state = common_info->sm_tplg_change_state;


	if(common_info->sm_tplg_change_state_begin)
	{
		common_info->sm_tplg_change_state = MSTP_INACTIVE;
		common_info->sm_tplg_change_state_begin = MSTP_FALSE;
	}
	else if((MSTP_LEARNING == common_info->sm_tplg_change_state)
/*(role != RootPort) &&
(role != DesignatedPort) &&
(role != MasterPort) &&
!(learn || learning) &&
!(rcvdTc || rcvdTcn || rcvdTcAck || tcProp)*/
			&& (common_info->role != MSTP_PORT_ROLE_ROOT)
			&& (common_info->role != MSTP_PORT_ROLE_DESIGNATED)
			&& (common_info->role != MSTP_PORT_ROLE_MASTER)
			&& (!(common_info->learn || common_info->learning))
			&& (!(common_info->rcvdTc || port_variables->rcvdTcn
				|| port_variables->rcvdTcAck || common_info->tcProp)))

	{
		common_info->sm_tplg_change_state = MSTP_INACTIVE;
	}
    else if((MSTP_INACTIVE == common_info->sm_tplg_change_state)
/*learn && !fdbFlush*/
		&& (common_info->learn && !common_info->fdbFlush))
    {
        common_info->sm_tplg_change_state = MSTP_LEARNING;
    }
    else if((MSTP_LEARNING == common_info->sm_tplg_change_state)
/*rcvdTc || rcvdTcn
|| rcvdTcAck || tcProp*/		
		&& (common_info->rcvdTc || port_variables->rcvdTcn 
			|| port_variables->rcvdTcAck || common_info->tcProp))
    {
/*if (cist) 
{
	rcvdTc = rcvdTcn = rcvdTcAck = FALSE;
}
rcvdTc = tcProp =  FALSE;*/
		if(common_br->cist(instance_id))
        {
            common_info->rcvdTc = MSTP_FALSE;
			port_variables->rcvdTcn = MSTP_FALSE;
			port_variables->rcvdTcAck = MSTP_FALSE;
        }
        common_info->rcvdTc = MSTP_FALSE;
		common_info->tcProp = MSTP_FALSE;
        common_info->sm_tplg_change_state = MSTP_LEARNING;
    }
	else if((MSTP_ACTIVE == common_info->sm_tplg_change_state)
/*((role != RootPort) && (role != DesignatedPort) && (role != MasterPort)) 
|| operEdge*/
	&&(((common_info->role != MSTP_PORT_ROLE_ROOT)
			&& (common_info->role != MSTP_PORT_ROLE_DESIGNATED)
			&& (common_info->role != MSTP_PORT_ROLE_MASTER))
		|| port_variables->operEdge))
    {
        common_info->sm_tplg_change_state = MSTP_LEARNING;
    }
    else if((MSTP_LEARNING == common_info->sm_tplg_change_state)
/*((role == RootPort) ||
(role == DesignatedPort) ||
(role == MasterPort)) &&
 forward && !operEdge*/		
		&&((MSTP_PORT_ROLE_ROOT == common_info->role)
			|| (MSTP_PORT_ROLE_DESIGNATED == common_info->role)
			|| (MSTP_PORT_ROLE_MASTER == common_info->role))
		&& (common_info->forward)
		&& (!port_variables->operEdge))
    {
        common_info->sm_tplg_change_state = MSTP_DETECTED;
    }
    else if((MSTP_ACTIVE == common_info->sm_tplg_change_state)
/* rcvdTcn*/		
		&& port_variables->rcvdTcn)
    {
        common_info->sm_tplg_change_state = MSTP_NOTIFIED_TCN;
    }
    else if((MSTP_ACTIVE == common_info->sm_tplg_change_state)
/* rcvdTc*/
		&& common_info->rcvdTc)
    {
        common_info->sm_tplg_change_state = MSTP_NOTIFIED_TC;
    }
    else if((MSTP_ACTIVE == common_info->sm_tplg_change_state)
/* tcProp && !operEdge*/
		&& common_info->tcProp
		&& !port_variables->operEdge)
    {
        common_info->sm_tplg_change_state = MSTP_PROPAGATING;
    }
    else if((MSTP_ACTIVE == common_info->sm_tplg_change_state)
/*rcvdTcAck*/
		&& port_variables->rcvdTcAck)
    {
        common_info->sm_tplg_change_state = MSTP_ACKNOWLEDGED;
    }
	else if(MSTP_NOTIFIED_TCN == common_info->sm_tplg_change_state)	/* UCT */
	{
		common_info->sm_tplg_change_state = MSTP_NOTIFIED_TC;
	}
	else if((MSTP_NOTIFIED_TC == common_info->sm_tplg_change_state)	/* UCT */
		|| (MSTP_PROPAGATING == common_info->sm_tplg_change_state)
		|| (MSTP_ACKNOWLEDGED == common_info->sm_tplg_change_state)
		|| (MSTP_DETECTED == common_info->sm_tplg_change_state))
	{
		common_info->sm_tplg_change_state = MSTP_ACTIVE;
	}

	/*state no change*/
	if(port_topology_change_last_state == common_info->sm_tplg_change_state)
	{
		return;
	}

	switch(common_info->sm_tplg_change_state)
	{
		case MSTP_INACTIVE:
		{
/*fdbFlush = TRUE; tcDetected = 0;
tcWhile = 0; if (cist) tcAck = FALSE;*/

			common_info->fdbFlush = MSTP_TRUE;
			mstp_flush(instance_id,port, mstp_aging_time(port)); //FIXME			
			common_info->fdbFlush = MSTP_FALSE;
			
			common_info->tcWhile = 0;
			if(common_br->cist(instance_id))
            {
                port_variables->tcAck = MSTP_FALSE;
            }
		}
			break;
		case MSTP_LEARNING:
		{
/*if (cist) {rcvdTc = rcvdTcn = rcvdTcAck = FALSE;}
rcvdTc = tcProp = FALSE;*/
			if(common_br->cist(instance_id))
			{
				common_info->rcvdTc = MSTP_FALSE;
				port_variables->rcvdTcn = MSTP_FALSE;
				port_variables->rcvdTcAck = MSTP_FALSE;
			}

			/*rcvdTc variable have done*/
			common_info->rcvdTc = MSTP_FALSE;
			common_info->tcProp = MSTP_FALSE;
		}
			break;
		case MSTP_DETECTED:
		{
/*newTcWhile(); setTcPropTree();
newTcDetected(); newInfoXst = TRUE;*/
			mstp_new_tc_while(port, instance_id);
			mstp_set_tc_prop_tree(port, instance_id);

			if(0 == instance_id) /* CIST */
            {
				port->cist_port.new_info = MSTP_TRUE;
            }
            else /* MSTI */
            {            
                port_variables->newInfoMsti = MSTP_TRUE;
            }
		}
			break;
		case MSTP_ACTIVE:
		{
		/*do nothing*/
		}
			break;
		case MSTP_NOTIFIED_TCN:
		{
/*newTcWhile();*/
			mstp_new_tc_while(port, instance_id);
		}
			break;
		case MSTP_NOTIFIED_TC:
		{
/*if (cist) rcvdTcn = FALSE;
rcvdTc = FALSE; 
if (cist && (role == DesignatedPort)) tcAck = TRUE; 
setTcPropTree();*/
			if(common_br->cist(instance_id))
			{
				port_variables->rcvdTcn = MSTP_FALSE;
			}
			common_info->rcvdTc = MSTP_FALSE;

			if((common_br->cist(instance_id))
				&& (MSTP_PORT_ROLE_DESIGNATED == common_info->role))
			{
				port_variables->tcAck = MSTP_TRUE;
			}

			mstp_set_tc_prop_tree(port, instance_id);
		}
			break;
		case MSTP_PROPAGATING:
		{
/*newTcWhile(); fdbFlush = TRUE;
tcProp = FALSE;*/

			mstp_new_tc_while(port, instance_id);
			common_info->fdbFlush = MSTP_TRUE;
			mstp_flush(instance_id,port, mstp_aging_time(port)); //FIXME
			common_info->fdbFlush = MSTP_FALSE;
			common_info->tcProp = MSTP_FALSE;
		}
			break;
		case MSTP_ACKNOWLEDGED:
		{
/* tcWhile = 0;  
rcvdTcAck = FALSE;*/
			common_info->tcWhile = 0;
			port_variables->rcvdTcAck = MSTP_FALSE;
		}
			break;
		default:
			break;
	}

	return;
}

void mstp_br_detection_sm(struct mstp_port *port)
{
	unsigned int				last_state = 0;
	struct mstp_port_variables	*port_variables	= NULL;

	if (NULL == port)
	{
		return;
	}

	port_variables = &(port->port_variables);

	last_state = port_variables->sm_br_detect_state;


/*BEGIN && !AdminEdge*/
    if(port_variables->sm_br_detect_state_begin && port_variables->AdminEdge)
    {
        port_variables->sm_br_detect_state = MSTP_EDGE;
		port_variables->sm_br_detect_state_begin = MSTP_FALSE;
	}
	else if(port_variables->sm_br_detect_state_begin && !port_variables->AdminEdge)
/*BEGIN && AdminEdge*/
    {
        port_variables->sm_br_detect_state = MSTP_NOT_EDGE;		
		port_variables->sm_br_detect_state_begin = MSTP_FALSE;
    }
    else if((port_variables->sm_br_detect_state == MSTP_EDGE)
/*((!portEnabled  || !AutoEdge) && !AdminEdge) || !operEdge*/		
		&&(((!port_variables->portEnabled || !port_variables->AutoEdge) && !port_variables->AdminEdge)
			|| (!port_variables->operEdge)))
    {
		port_variables->sm_br_detect_state = MSTP_NOT_EDGE;
    }
	else if((MSTP_NOT_EDGE == port_variables->sm_br_detect_state)
/*(!portEnabled && AdminEdge) ||
((edgeDelayWhile == 0) && AutoEdge &&sendRSTP && proposing)*/		
		&& ((!port_variables->portEnabled && port_variables->AdminEdge)
			|| ((0 == port_variables->edgeDelayWhile)
				&& port_variables->AutoEdge && port_variables->sendRSTP
				&& port->cist_port.common_info.proposing)))
    {
		port_variables->sm_br_detect_state = MSTP_EDGE;
    }
	#if 0
	else if((port_variables->sm_br_detect_state == MSTP_NOT_EDGE)
	/*(edgeDelayWhile == 0) && !AdminEdge && !AutoEdge && sendRSTP && proposing &&operPointToPoint*/
		&&(0 == port_variables->edgeDelayWhile)
		&&(!port_variables->AdminEdge)
		&&(!port_variables->AutoEdge)
		&&(port_variables->sendRSTP)
		&&(port->cist_port.common_info.proposing)
		&&(port_variables->operPointToPointMAC))    /*TRUE if operPointToPointMAC (6.6.3) is TRUE for the Bridge Port*/
	{
		port_variables->sm_br_detect_state = MSTP_ISOLATED;
	}
	else if((port_variables->sm_br_detect_state == MSTP_ISOLATED)
		/*AdminEdge || AutoEdge || !isolate || !operPointToPoint*/
		&&(port_variables->AdminEdge || 
		port_variables->AutoEdge ||!port_variables->isolate || !port_variables->operPointToPointMAC))
	{
		port_variables->sm_br_detect_state = MSTP_NOT_EDGE;
	}
	#endif
	if(last_state != port_variables->sm_br_detect_state)
	{
		switch(port_variables->sm_br_detect_state)
        {
			case MSTP_EDGE:
			{
/*operEdge = TRUE; isolate = FALSE;*/				
				port_variables->operEdge = MSTP_TRUE;
			}
            	break;
			case MSTP_NOT_EDGE:
			{
/*operEdge = FALSE; isolate = FALSE;*/				
				port_variables->operEdge = MSTP_FALSE;
			}
				break;
			case MSTP_ISOLATED:
/*operEdge = FALSE; isolate = TRUE;*/
			{
				port_variables->operEdge = MSTP_FALSE;
				//port_variables->isolate = MSTP_TRUE;
				
			}
			default:
				break;
		}
	}
	
	return;
}

void mstp_rcv_sm_run(struct mstp_port* port)
{
	if (NULL == port)
	{
		return;
	}

	port->port_variables.rcvdBpdu = MSTP_TRUE;

	mstp_state_machine_one_run(port);
	mstp_state_machine_two_run(port->m_br);
	mstp_state_machine_three_run(port);
}


void mstp_state_machine_normal_action(struct mstp_bridge *mstp_bridge)
{
	struct mstp_port* mstp_port = NULL, *next = NULL;

	if (NULL == mstp_bridge)
	{
		return;
	}

	list_for_each_entry_safe(mstp_port, next, &mstp_bridge->port_head, port_list)
	{
		mstp_state_machine_one_run(mstp_port);
		
	}

    mstp_state_machine_two_run(mstp_bridge);
	list_for_each_entry_safe(mstp_port, next, &mstp_bridge->port_head, port_list)
	{
		mstp_state_machine_three_run(mstp_port);
	}


	return;
}

void mstp_state_machine_one_run(struct mstp_port *port)
{
	unsigned int		ins_id = 0;	/*instance id*/

	if (NULL == port)
	{
		return;
	}

	mstp_port_timer_sm(port);
	mstp_port_recv_sm(port);

	mstp_port_protocol_migration_sm(port);
	mstp_br_detection_sm(port);

	/*list mstp_port`s msti port*/
	for(ins_id = 0; ins_id <= MSTP_INSTANCE_MAX; ins_id++)
	{
		if((ins_id == 0)
			|| ((ins_id > 0) && (port->msti_port[ins_id-1] != NULL)))
		{
			/*port info:*/
			mstp_port_info_sm(port, ins_id);
		}
	}
}

void mstp_state_machine_two_run(struct mstp_bridge *mstp_bridge)
{
	unsigned int		ins_id = 0;

	if (NULL == mstp_bridge)
	{
		return;
	}

	for(ins_id = 0; ins_id <= MSTP_INSTANCE_MAX; ins_id++)
	{
		if((ins_id == 0)
			|| ((ins_id > 0) && (mstp_bridge->msti_br[ins_id-1] != NULL)))
		{
			mstp_port_role_selection_sm(mstp_bridge, ins_id);
		}
	}

	return;
}

void mstp_state_machine_three_run(struct mstp_port *port)
{
	unsigned int	ins_id = 0;

	if (NULL == port)
	{
		return;
	}

	for(ins_id = 0; ins_id <= MSTP_INSTANCE_MAX; ins_id++)
	{
		if((ins_id == 0)
			|| ((ins_id > 0) && (port->msti_port[ins_id-1] != NULL)))
		{
			mstp_port_role_trans_sm(port, ins_id);
			mstp_port_state_trans_sm(port, ins_id);
			mstp_port_Topology_change_sm(port, ins_id);
		}
	}

	mstp_port_tx_sm(port);

	return;
}

/*when port change link status:*/
void mstp_link_change_sm_action(struct mstp_port* mstp_port)
{
	if (NULL == mstp_port)
	{
		return;
	}

	mstp_state_machine_one_run(mstp_port);
	mstp_state_machine_two_run(mstp_port->m_br);
	mstp_state_machine_three_run(mstp_port);

	return;
}


