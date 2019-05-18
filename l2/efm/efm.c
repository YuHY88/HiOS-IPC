/*author:  youcheng*/
/*time:  2017.12.7*/
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
#include "l2/l2_if.h"
#include "efm/efm.h"
#include "efm/efm_link_monitor.h"
#include "efm/efm_cmd.h"
#include "efm_state.h"
#include "efm/efm_pkt.h"
#include "efm/efm_agent.h"


uint8_t efm_mac[6] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x02};
/*efm all kinds of event type*/
const char * efm_link_event_type_str[] =
{
	"SYMBOL_PERIOD_EVENT",
	"FRAME_EVENT",
	"FRAME_PERIOD_EVENT",	
	"FRAME_SECONDS_SUM_EVENT",
	"ORG_SPEC_EVENT",
	"LINK_FAULT_EVENT",
	"DYING_GASP_EVENT",
	"CRITICAL_LINK_EVENT"
};
/*check efm dying gasp(device power down)*/
void efm_check_dying_gasp( void *arg)
{
	struct sockaddr_nl saddr, daddr;  
	struct nlmsghdr *nlhdr = NULL;  
	struct iovec iov;  
	struct msghdr msg;  

	int fd;  
	int ret = 1,on = 1;  
    unsigned char rx[10] = {0};

	fd = socket(AF_NETLINK, SOCK_RAW,POWEROFF_TEST);  
	if(fd<0)
    {
        return;
    }

	memset(&saddr, 0, sizeof(saddr));  
	memset(&daddr, 0, sizeof(daddr));  

	saddr.nl_family = AF_NETLINK;        
	saddr.nl_pid = getpid();  
	saddr.nl_groups =  1<<2 ; 
	ret = setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	if(ret < 0)
	{
		perror("setsockopt error!");  
	}
	ret = bind(fd, (struct sockaddr*)&saddr, sizeof(saddr));  
	if(ret < 0)
	{
		perror("bind error!");
		return ;
	}
	nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(NILMSGHDR_LEN));  

	while (1) {  

		memset(nlhdr, 0, NLMSG_SPACE(NILMSGHDR_LEN));  

		iov.iov_base = (void *)nlhdr;  
		iov.iov_len = NLMSG_SPACE(NILMSGHDR_LEN);  
		msg.msg_name = (void *)&daddr;  
		msg.msg_namelen = sizeof(daddr);  
		msg.msg_iov = &iov;  
		msg.msg_iovlen = 1;  

		ret = recvmsg(fd, &msg, 0);

		if (ret <= 0) 
        {
			continue;
        }

		memcpy(rx,NLMSG_DATA(nlhdr), 4);   //read nlmsg information
		printf("RECV: value=%d \n", rx[0]);
		if(1 == rx[0])
		{    
			efm_recv_local_event (EFM_DYING_GASP_EVENT,1);
			break;
		}


	}  
	pthread_exit(NULL);
	close(fd);  

}

/*turn on a thread function to check dyinggasp event*/
void
efm_pthread_dyinggasp_event_check(void)
{      
	int ret = 0;
	pthread_t pth;
	ret = pthread_create(&pth,NULL,(void *)efm_check_dying_gasp,NULL);
	if(ret < 0)
	{
		zlog_err ("%s[%d]:(%s)Pthread_create failed for dyinggasp event check from EFM\n", 
				__FILE__,__LINE__, __func__);
		return ;
	}
}
/*
	s_int32_t
efm_process_return (struct vty *vty, s_int32_t retval)
{
	s_int32_t ret = CMD_WARNING;

	switch (retval)
	{
		case EFM_ERR_IF_NOT_FOUND:
			if (vty)
				vty_out (vty, "%%Interface Not found%s", VTY_NEWLINE);
			else
				zlog_err ("Interface Not found\n");
			break;
		case EFM_ERR_NOT_ENABLED:
			if (vty)
				vty_out (vty, "%%EFM not enabled%s", VTY_NEWLINE);
			else
				zlog_err ("EFM not enabled\n");
			break;
		case EFM_ERR_ENABLED:
			if (vty)
				vty_out (vty, "%%EFM already enabled%s\n", VTY_NEWLINE);
			else
				zlog_err ("EFM already enabled\n");
			break;
		case EFM_ERR_MEMORY:
			if (vty)
				vty_out (vty, "%%Out of memory while updating%s", VTY_NEWLINE);
			else
				zlog_err ("Out of memory while updating\n");
			break;
		case EFM_ERR_REM_LB_NOT_SUPP:
			if (vty)
				vty_out (vty, "%%Remote Loopback not supported%s", VTY_NEWLINE);
			else
				zlog_err ("Remote Loopback not supported\n");
			break;
		case EFM_ERR_DISCOVERY:
			if (vty)
				vty_out (vty, "%%EFM discovery state is not SEND_ANY%s", VTY_NEWLINE);
			else
				zlog_err ("EFM discovery state is not SEND_ANY\n");
			break;
		case EFM_ERR_RB_MODE:
			if (vty)
				vty_out (vty, "%%EFM is in Remote Loopback mode%s", VTY_NEWLINE);
			else
				zlog_err ("EFM is in Remote Loopback mode\n");
			break;
		case EFM_ERR_IF_TYPE:
			if (vty)
				vty_out (vty, "%%EFM not enabled, invalid interface mode%s", VTY_NEWLINE);
			else
				zlog_err ("EFM not enabled, invalid interface mode\n");
			break;
		case EFM_ERR_UPDATE_PRI:
			if (vty)
				vty_out (vty, "%%Init the priority of EFMPDU failed%s", VTY_NEWLINE);
			else
				zlog_err ("Init the priority of EFMPDU failed\n");
			break;
		case EFM_ERR_SET_UNIDIREC:
			if (vty)
				vty_out (vty, "%%Set EFM Unidirectional Link Support failed%s", VTY_NEWLINE);
			else
				zlog_err ("Set EFM Unidirectional Link Support failed\n");
			break;
		case EFM_ERR_LOCALSIDE_MAKE_REMOTESIDE_STARTED:
			if (vty)
				vty_out (vty, "%%EFM is already start remote loopback%s", VTY_NEWLINE);
			else
				zlog_err ("EFM is already start remote loopback\n");
			break;
		case EFM_ERR_REMOTESIDE_MAKE_LOCALSIDE_STARTED:
			if (vty)
				vty_out (vty, "%%EFM is already start local loopback%s", VTY_NEWLINE);
			else
				zlog_err ("EFM is already start local loopback\n");
			break;
		case EFM_ERR_MODE_PASSIVE:
			if (vty)
				vty_out (vty, "%%EFM mode is passive%s", VTY_NEWLINE);
			else
				zlog_err ("EFM mode is passive\n");
			break;	
		default:
			ret = CMD_SUCCESS;
			break;
	}

	return ret;
}
*/
/* efm discovery state return value  */
/* refer to document 802.3ah-2004 section 57.3.2.1*/
	uint8_t *
efm_discovery_state_to_str (const uint8_t discovery_state)
{
	switch (discovery_state)
	{
		case EFM_DISCOVERY_FAULT_STATE:
			return (uint8_t *)"Fault";
		case EFM_DISCOVERY_ACTIVE_SEND_LOCAL:
			return (uint8_t *)"Active Send Local";
		case EFM_DISCOVERY_PASSIVE_WAIT:
			return (uint8_t *)"Passive Wait";
		case EFM_DISCOVERY_SEND_LOCAL_REMOTE:
			return (uint8_t *)"Send Local Remote";
		case EFM_DISCOVERY_SEND_LOCAL_REMOTE_OK:
			return (uint8_t *)"Send Local Remote OK";
		case EFM_DISCOVERY_SEND_ANY:
			return (uint8_t *)"Send Any";

		default:
			return (uint8_t *)"Invalid";
	}

	return (uint8_t *)"Invalid";
}

/* efm remote loopback state return value */
/* refer to document 802.3ah-2004 section 57.2.11 OAM remote loopback*/
	uint8_t *
efm_par_state_to_str (enum efm_local_par_action par_action)
{
	switch (par_action)
	{
		case EFM_PAR_FWD:
			return (uint8_t *)"Forward";
		case EFM_PAR_LB:
			return (uint8_t *)"Loopback";
		case EFM_PAR_DISCARD:
			return (uint8_t *)"Discard";
		case EFM_PAR_INVALID:
			return (uint8_t *)"Invalid";
		default:
			return (uint8_t *)"Invalid";
	}

	return (uint8_t *)"Invalid";
}

/* efm remote loopback state return value */
/* refer to document 802.3ah-2004 section 57.2.11 OAM remote loopback*/
	uint8_t *
efm_mux_state_to_str (enum efm_local_mux_action mux_action)
{
	switch (mux_action)
	{
		case EFM_MUX_FWD:
			return (uint8_t *)"Forward";
		case EFM_MUX_DISCARD:
			return (uint8_t *)"Discard";
		case EFM_MUX_INVALID:
			return (uint8_t *)"Invalid";
		default:
			return (uint8_t *)"Invalid";
	}

	return (uint8_t *)"Invalid";
}


/*efm enable*/
	s_int32_t
efm_protocol_enable (struct l2if *pif)
{        


    struct efm_if  *pefm = NULL;
	if (NULL == pif)
    {
		return EFM_ERR_IF_NOT_FOUND;
    }


	if (NULL == pif->pefm)
	{
		pif->pefm = (struct efm_if*)XMALLOC (MTYPE_EFM, sizeof (struct efm_if));

		if (NULL == pif->pefm)
		{
			zlog_err ("%s[%d]:(%s)Could not allocate memory for EFM\n", 
					__FILE__,__LINE__, __func__);
			return EFM_ERR_MEMORY;
		}

		/*initialise memory allocated*/
		memset(pif->pefm, 0, sizeof (struct efm_if));
	}
	else
	{
		if (pif->pefm->efm_local_oam_enable == EFM_TRUE)
		{
			return EFM_ERR_ENABLED;
		}
	}
   pefm = pif->pefm;



	pif->pefm->if_index = pif->ifindex;

	ifm_get_name_by_ifindex(pif->ifindex, pif->pefm->if_name);
	memcpy(pif->pefm->mac, pif->mac, 6);
	pif->pefm->efm_local_oam_enable = EFM_TRUE;
	pif->pefm->efm_local_lost_link_timer_done  = EFM_FALSE;

	EFM_LOG_DBG("EFM[EVENT]: EFM Enabled\n");

	if (!pif->down_flag)
	{
		pif->pefm->efm_local_link_status = EFM_OAM_LINK_UP;
	}
	else
	{
		pif->pefm->efm_local_link_status = EFM_OAM_LINK_FAULT;
	}		

	SET_FLAG (pif->pefm->local_info.oam_config, EFM_CFG_LINK_EVENT_SUPPORT |EFM_CFG_REM_LB_SUPPORT);

	UNSET_FLAG (pif->pefm->local_info.oam_config, EFM_CFG_VAR_RESPONSE_SUPPORT);

	pif->pefm->local_info.oam_pdu_config = EFM_MAX_OAMPDU_SIZE;

	/* By default the interface is in Passive mode */
	pif->pefm->efm_local_oam_mode = EFM_MODE_PASSIVE;
	UNSET_FLAG (pif->pefm->local_info.oam_config, EFM_CFG_OAM_MODE);

	if (pif->pefm->efm_local_link_status == EFM_OAM_LINK_FAULT )
    {
		pif->pefm->local_pdu = EFM_LF_INFO;
    }
	else
    {
		pif->pefm->local_pdu = EFM_RX_INFO;
    }

	pif->pefm->efm_local_stable = EFM_FALSE;
	pif->pefm->efm_discovery_state = EFM_DISCOVERY_FAULT_STATE;
	pif->pefm->efm_transmit_state = EFM_TRANSMIT_RESET;
	pif->pefm->efm_link_time = EFM_LINK_TIMER;
	pif->pefm->efm_remote_loopback_timeout = EFM_REM_LB_LINK_TIMER;
	pif->pefm->efm_pdu_time = EFM_PDU_TIMER;
	pif->pefm->efm_pdu_max_per_second  = EFM_MAX_RATE_DEFAULT;
	pif->pefm->efm_pdu_max   = EFM_MAX_RATE_DEFAULT * EFM_PDU_TIMER;
/*initial link event parameters,these value id defined in document 802.3ah-2004 section 57.5.3 LINK EVENT TLV*/
	pif->pefm->local_link_info.err_symbol_threshold = EFM_SYMBOL_PERIOD_THRES_DEF;
	pif->pefm->local_link_info.err_symbol_window = EFM_SYMBOL_PERIOD_WINDOW_DEF;

	pif->pefm->local_link_info.err_frame_threshold = EFM_ERR_FRAME_THRES_DEF;
	pif->pefm->local_link_info.err_frame_window = EFM_ERR_FRAME_WINDOW_DEF;

	pif->pefm->local_link_info.err_frame_period_threshold = EFM_FRAME_PERIOD_THRES_DEF;
	pif->pefm->local_link_info.err_frame_period_window = EFM_FRAME_PERIOD_WINDOW_DEF;

	pif->pefm->local_link_info.err_frame_sec_sum_threshold = EFM_FRAME_SECOND_THRES_DEF;
	pif->pefm->local_link_info.err_frame_sec_sum_window = EFM_FRAME_SECOND_WINDOW_DEF;

	/*efm inform hal to create data struct about link event monitor*/
#if 0
	if(ipc_send_hal ( NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_L2,	
				IPC_TYPE_EFM, EFM_TRUE, IPC_OPCODE_EFM, pif->pefm->if_index ))

	{
		zlog_err ( "%s[%d]:leave %s:error:efm send info to hal malloc struct  failure!\n", __FILE__, __LINE__, __func__ );
		return EFM_FALSE;

	}
#endif
	if(ipc_send_msg_n2( NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_L2,	
				IPC_TYPE_EFM, EFM_TRUE, IPC_OPCODE_EFM, pif->pefm->if_index ))

	{
		zlog_err ( "%s[%d]:leave %s:error:efm send info to hal malloc struct  failure!\n", __FILE__, __LINE__, __func__ );
		return EFM_FALSE;

	}
	/*efm send Errored Symbol Period Event parameters(threshold window) to hal*/
	if(efm_send_link_monitor_data_to_hal(pefm->if_index,EFM_ERR_SYMBOL_PERIOD_EVENT, pefm->local_link_info.err_symbol_window,
				pefm->local_link_info.err_symbol_threshold))
	{

                     zlog_err ( "%s[%d]:leave %s:error:efm send link monitor data failure!\n", __FILE__, __LINE__, __func__ );
		return EFM_FALSE;
	}
/*efm send Errored Frame Event parameters(threshold window) to hal*/
	if(efm_send_link_monitor_data_to_hal(pefm->if_index,EFM_ERR_FRAME_EVENT, (uint64_t)pefm->local_link_info.err_frame_window,
				(uint64_t)pefm->local_link_info.err_frame_threshold))

	{
		zlog_err ( "%s[%d]:leave %s:error:efm send link monitor data failure!\n", __FILE__, __LINE__, __func__ );
		return EFM_FALSE;


    }
/*efm send Errored Frame Period Event parameters(threshold window) to hal*/

	if(efm_send_link_monitor_data_to_hal(pefm->if_index, EFM_ERR_FRAME_PERIOD_EVENT,(uint64_t)pefm->local_link_info.err_frame_period_window,
				(uint64_t)pefm->local_link_info.err_frame_period_threshold))
	{
		zlog_err ( "%s[%d]:leave %s:error:efm send link monitor data failure!\n", __FILE__, __LINE__, __func__ );
		return EFM_FALSE;

    }
/*efm send Errored Symbol Seconds Event parameters(threshold window) to hal*/

	if(efm_send_link_monitor_data_to_hal(pefm->if_index,EFM_ERR_FRAME_SECONDS_SUM_EVENT, (uint64_t)pefm->local_link_info.err_frame_sec_sum_window,
				(uint64_t)pefm->local_link_info.err_frame_period_threshold))
	{
		zlog_err ( "%s[%d]:leave %s:error:efm send link monitor data failure!\n", __FILE__, __LINE__, __func__ );
		return EFM_FALSE;
	}
	

     
		
	pif->pefm->local_link_info.err_symbol_ev_notify_enable = EFM_NOTIFY_ON;
	pif->pefm->local_link_info.err_frame_ev_notify_enable = EFM_NOTIFY_ON;
	pif->pefm->local_link_info.err_frame_period_ev_notify_enable = EFM_NOTIFY_ON;
	pif->pefm->local_link_info.err_frame_sec_ev_notify_enable = EFM_NOTIFY_ON;

	pif->pefm->local_link_info.dying_gasp_enable = EFM_NOTIFY_ON;
	pif->pefm->local_link_info.critical_event_enable = EFM_NOTIFY_ON;

	/*Create a new list for storing Event Log Entries*/
	pif->pefm->efm_if_eventlog_list  = list_new();
	pif->pefm->num_event_log_entries = 0;
	pif->pefm->max_event_log_entries = EFM_MAX_EVLOG_ENTRIES_DEF;
	pif->pefm->event_log_index = 0;
	/*ipran_alarm_port_register(IFM_FUN_ETH_TYPE, IFM_SLOT_ID_GET(pif->ifindex), 
								IFM_PORT_ID_GET(pif->ifindex), 0, 0);*/
								
	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = pif->ifindex;
	ipran_alarm_port_register(&gPortInfo);


/*start efm discover*/
	efm_run_state_machine (pif->pefm);

	efm_system_uptime(&pif->pefm->local_link_info.last_symbol_period_event_ts);

	efm_system_uptime(&pif->pefm->local_link_info.last_frame_event_ts);

	efm_system_uptime(&pif->pefm->local_link_info.last_frame_period_event_ts);

	efm_system_uptime(&pif->pefm->local_link_info.last_frame_sec_sum_event_ts);


	/*Who send poweroff Information? maybe FPGA
	 *    this should process or init FPGA(Dying gasp)
	 */
	return EFM_SUCCESS; 
}

/*turn off efm function*/
	s_int32_t
efm_protocol_disable (struct l2if *pif)
{
	struct listnode *node = NULL;
	struct efm_event_log_entry *ev_log_entry = NULL;
	int ret = -1;
	uint8_t loopback_act = 0;
	if (NULL == pif)
    {
		return EFM_ERR_IF_NOT_FOUND;
    }

	if (NULL == pif->pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	if (pif->pefm->local_info.loopback == EFM_REM_LB)
	{
		/* Relinquish the remote loopback state when OAM is disabled */
		if(pif->pefm->efm_mac_swap_flag)
		{
			loopback_act = EFM_NO_MAC_SWAP;
#if 0
			if(ipc_send_hal ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
						IPC_TYPE_EFM, MAC_SWAP, IPC_OPCODE_CHANGE, pif->pefm->if_index ))
			{


				zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel mac swap failure!\n", __FILE__, __LINE__, __func__ );
				return EFM_FALSE;
			}
#endif
			if(ipc_send_msg_n2 ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
						IPC_TYPE_EFM, MAC_SWAP, IPC_OPCODE_CHANGE, pif->pefm->if_index ))
			{


				zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel mac swap failure!\n", __FILE__, __LINE__, __func__ );
				return EFM_FALSE;
			}
			pif->pefm->efm_mac_swap_flag = EFM_FALSE;
		}
		else 
		{
			loopback_act  = EFM_DISABLE_LB;
#if 0
			if(ipc_send_hal ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
						IPC_TYPE_EFM, REMOTE_LOOPBACK, IPC_OPCODE_CHANGE, pif->pefm->if_index ))
			{

				zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel loopback failure!\n", __FILE__, __LINE__, __func__ );
				return EFM_FALSE;

			}
#endif
			if(ipc_send_msg_n2 ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
						IPC_TYPE_EFM, REMOTE_LOOPBACK, IPC_OPCODE_CHANGE, pif->pefm->if_index ))
			{

				zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel loopback failure!\n", __FILE__, __LINE__, __func__ );
				return EFM_FALSE;

			}
		}
		//send local par_action FWD and mux_station FWD to hsl, if set fail, need return;
	}

	ret = efm_remote_unidir_link_set(pif->pefm, EFM_FALSE);
	if (ret < 0)
	{
		return EFM_ERR_SET_UNIDIREC;
	}

	if((SET_FLAG (pif->pefm->local_link_event_flags, EFM_LINK_EVENT_ON)))
	{

		pif->pefm->efm_link_monitor_t.link_monitor_on = EFM_FALSE;

#if 0		
		if(ipc_send_hal ( NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_EFM, pif->pefm->efm_link_monitor_t.link_monitor_on, IPC_OPCODE_LINK_MONITOR, pif->pefm->if_index ))
#endif
		if(ipc_send_msg_n2 ( NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_EFM, pif->pefm->efm_link_monitor_t.link_monitor_on, IPC_OPCODE_LINK_MONITOR, pif->pefm->if_index ))
		{
        	zlog_err ( "%s[%d]:leave %s:error:efm send info to hal turn off link monitor timer  failure!\n", __FILE__, __LINE__, __func__ );
			return EFM_FALSE;
		}
		
		UNSET_FLAG (pif->pefm->local_link_event_flags, EFM_LINK_EVENT_ON);
	}
	//send efm alarm clean, 
	//include efm_fail, efm_dyinggasp, efm_link_fault, efm_critical, 

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = pif->pefm->if_index;

	//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pif->pefm->if_index),IFM_PORT_ID_GET(pif->pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_FAIL,GPN_SOCK_MSG_OPT_CLEAN);
	//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pif->pefm->if_index),IFM_PORT_ID_GET(pif->pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_LINK_FAULT,GPN_SOCK_MSG_OPT_CLEAN);
	//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pif->pefm->if_index),IFM_PORT_ID_GET(pif->pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_CRITICAL,GPN_SOCK_MSG_OPT_CLEAN);
	//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pif->pefm->if_index),IFM_PORT_ID_GET(pif->pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_DYINGGASP ,GPN_SOCK_MSG_OPT_CLEAN);

	ipran_alarm_report (&gPortInfo, GPN_ALM_TYPE_FUN_EFM_FAIL, GPN_SOCK_MSG_OPT_CLEAN);
	ipran_alarm_report (&gPortInfo, GPN_ALM_TYPE_FUN_EFM_LINK_FAULT, GPN_SOCK_MSG_OPT_CLEAN);
	ipran_alarm_report (&gPortInfo, GPN_ALM_TYPE_FUN_EFM_CRITICAL, GPN_SOCK_MSG_OPT_CLEAN);
	ipran_alarm_report (&gPortInfo, GPN_ALM_TYPE_FUN_EFM_DYINGGASP, GPN_SOCK_MSG_OPT_CLEAN);

	
	//include l3_rem_pd, l3_rem_rx_los, efm_rem_lp_alm

	EFM_TIMER_OFF (pif->pefm->efm_link_timer);
	EFM_TIMER_OFF (pif->pefm->efm_tx_timer);
	EFM_TIMER_OFF (pif->pefm->efm_rem_lb_timer);

	/*Free the memory used for event log entries*/
	EFM_LIST_LOOP (pif->pefm->efm_if_eventlog_list, ev_log_entry, node) 
	{
		if (ev_log_entry)
		{
			XFREE (MTYPE_EFM_EVLOG_ENTRY, ev_log_entry);
		}
	}

	/* delete the list maintained for event logs on this interface */
	list_delete (pif->pefm->efm_if_eventlog_list);

	EFM_LOG_DBG("EFM[EVENT]: EFM OAM is Disabled\n");

	//cancel dying gasp in FPGA

	/*send hal free data*/
#if 0
	if(ipc_send_hal ( NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_L2,
			IPC_TYPE_EFM, EFM_FALSE, IPC_OPCODE_EFM, pif->pefm->if_index ))
	{
                  zlog_err ( "%s[%d]:leave %s:error:efm send info to hal free struct  failure!\n", __FILE__, __LINE__, __func__ );
		return EFM_FALSE;
	}
#endif
	if(ipc_send_msg_n2( NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_EFM, EFM_FALSE, IPC_OPCODE_EFM, pif->pefm->if_index ))
	{
		zlog_err ( "%s[%d]:leave %s:error:efm send info to hal free struct  failure!\n", __FILE__, __LINE__, __func__ );
		return EFM_FALSE;
	}
	/*ipran_alarm_port_unregister(IFM_FUN_ETH_TYPE, IFM_SLOT_ID_GET(pif->ifindex), 
									IFM_PORT_ID_GET(pif->ifindex), 0, 0);  */

	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = pif->ifindex;
	ipran_alarm_port_unregister(&gPortInfo);
	
	pif->pefm->efm_local_oam_enable = EFM_FALSE;
	
	if(pif->pefm->aefm != NULL)
	{
		efm_agent_disable(pif->pefm);	
		XFREE (MTYPE_EFM, pif->pefm->aefm);
		pif->pefm->aefm= NULL;
	}
	efm_clean_port(pif->pefm);
	pif->pefm = NULL;

	return EFM_SUCCESS;
}

/*efm mode set refer to document 802.3ah-2004 57.5.2.1 OAM Configation Table 57-8*/
	s_int32_t
efm_mode_active_set (struct efm_if *pefm)
{
	if (NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	if (pefm->efm_local_oam_enable == EFM_FALSE)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	if ((pefm->local_info.loopback != EFM_NONE_LB) ||
			(pefm->rem_info.loopback != EFM_NONE_LB ))
    {
		return EFM_ERR_RB_MODE;
    }

	pefm->efm_local_oam_mode = EFM_MODE_ACTIVE;

	EFM_LOG_DBG("EFM[EVENT]: EFM mode is set to active\n");

	SET_FLAG (pefm->local_info.oam_config, EFM_CFG_OAM_MODE);

	efm_discovery_fault(pefm);
	efm_run_state_machine (pefm);

	return EFM_SUCCESS;
}

/*efm mode set refer to document 802.3ah-2004 57.5.2.1 OAM Configuration Table 57-8*/
	s_int32_t
efm_mode_passive_set (struct efm_if *pefm)
{
    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

    if(EFM_FALSE == pefm->efm_local_oam_enable)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	if ((pefm->local_info.loopback != EFM_NONE_LB) ||
			(pefm->rem_info.loopback != EFM_NONE_LB ))
    {
		return EFM_ERR_RB_MODE;
    }

	pefm->efm_local_oam_mode = EFM_MODE_PASSIVE;

	EFM_LOG_DBG("EFM[EVENT]: EFM mode is set to Passive\n");

	UNSET_FLAG (pefm->local_info.oam_config, EFM_CFG_OAM_MODE);

	efm_discovery_fault (pefm);
	efm_run_state_machine (pefm);

	return EFM_SUCCESS;
}
/*efm set support unidirection send PDU refer to document 802.3ah-2004 57.5.2.1 OAM Configuration Table 57-8*/
	s_int32_t
efm_remote_unidir_link_set(struct efm_if *pefm, uint8_t enable)
{
	uint8_t act = 0;
    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

    if(EFM_TRUE == enable)
	{
		if (CHECK_FLAG(pefm->local_info.oam_config, EFM_CFG_UNI_DIR_SUPPORT))
        {
			return EFM_SUCCESS;
        }
	}
	else
	{
		if (!CHECK_FLAG(pefm->local_info.oam_config, EFM_CFG_UNI_DIR_SUPPORT))
        {
			return EFM_SUCCESS;
        }
    }
/*efm inform hal to support undirection send PDU*/
    if(EFM_TRUE == enable)
	{
		SET_FLAG (pefm->local_info.oam_config,EFM_CFG_UNI_DIR_SUPPORT);
		act = EFM_UNI_DIR_ENABLE;
#if 0
		ipc_send_hal ( &act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_EFM, LINK_FAULT, IPC_OPCODE_CHANGE, pefm->if_index );
#endif
		ipc_send_msg_n2( &act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_EFM, LINK_FAULT, IPC_OPCODE_CHANGE, pefm->if_index );
	}
	else
	{
		UNSET_FLAG (pefm->local_info.oam_config, EFM_CFG_UNI_DIR_SUPPORT);
		act = EFM_UNI_DIR_DISABLE;
#if 0
		ipc_send_hal ( &act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_EFM, LINK_FAULT, IPC_OPCODE_CHANGE, pefm->if_index );
#endif
		ipc_send_msg_n2( &act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_EFM, LINK_FAULT, IPC_OPCODE_CHANGE, pefm->if_index );
	}

	EFM_LOG_DBG("EFM[EVENT]: EFM Unidirectional Link Support is %s\n",
			(CHECK_FLAG (pefm->local_info.oam_config, 
				     EFM_CFG_UNI_DIR_SUPPORT)) ? "Enabled" : "Disabled");

	if (pefm->efm_pdu_cnt != 0)
	{
		pefm->valid_pdu_req = EFM_TRUE;
		efm_run_state_machine(pefm);
	}

	//set efm unidirection support to hsl and fpga, force tx, if set fail, need return;

	return EFM_SUCCESS;
}

/*efm set support remote loopback. refer to document 802.3ah-2004 57.5.2.1 OAM Configuration Table 57-8*/
	s_int32_t
efm_remote_loopback_set (struct efm_if *pefm, u_int8_t enable)
{
	if (pefm == NULL)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	if (enable == EFM_TRUE)
    {
		SET_FLAG (pefm->local_info.oam_config, EFM_CFG_REM_LB_SUPPORT);
    }
	else
    {
		UNSET_FLAG (pefm->local_info.oam_config, EFM_CFG_REM_LB_SUPPORT);
    }

	EFM_LOG_DBG("EFM[EVENT]: EFM Remote Loopback is %s\n",
			(CHECK_FLAG (pefm->local_info.oam_config,
				     EFM_CFG_REM_LB_SUPPORT)) ? "Enabled" : "Disabled");

	if (pefm->efm_pdu_cnt != 0)
	{
		pefm->valid_pdu_req = EFM_TRUE;
		efm_run_state_machine (pefm);
	}

	return EFM_SUCCESS;
}

/*efm initiate remote loopback timeout*/
	s_int32_t
efm_remote_loopback_timeout_set (struct efm_if *pefm, u_int8_t timeout)
{
	if (pefm == NULL)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	pefm->efm_remote_loopback_timeout = timeout;

	EFM_LOG_DBG("EFM[EVENT]: EFM Remote loopback timeout is set "
			"to %d secs\n", timeout);

	return EFM_SUCCESS;
}

/*efm remote loopback initial*/
	s_int32_t
efm_remote_loopback_start_api (struct efm_if *pefm)
{
	int ret = 0;

    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	if(EFM_DISCOVERY_SEND_ANY != pefm->efm_discovery_state)
    {
		return EFM_ERR_DISCOVERY;
    }
	//need xchange

    if((EFM_LOCAL_LB == pefm->local_info.loopback) ||
            (EFM_REM_LB == pefm->local_info.loopback) ||
            (EFM_LOCAL_LB == pefm->rem_info.loopback) ||
            (EFM_REM_LB == pefm->rem_info.loopback))
    {
        return EFM_ERR_RB_MODE;
    }

	ret = efm_remote_loopback_initialize (pefm);

	return ret;
}


	int
efm_remote_loopback_initialize(struct efm_if *pefm)
{
	EFM_LOG_DBG("EFM[EVENT]: Initialising Remote Loopback procedure\n");

    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

    if(EFM_MODE_PASSIVE == pefm->efm_local_oam_mode)
    {
		return EFM_ERR_MODE_PASSIVE;
    }

	//send local par_action discard and mux_action discard to hsl, if set fail, return error;


	//pefm->efm_localside_make_remoteside_loopback_start = EFM_TRUE;
    /*refer to document 802.3ah-2004 57.2.11.1 Initiating OAM remote loopback*/
	pefm->efm_local_mux_action_t = EFM_MUX_DISCARD;
	pefm->efm_local_par_action_t = EFM_PAR_DISCARD;
	pefm->efm_rem_loopback = EFM_TRUE;
	//new add
	pefm->local_info.loopback = EFM_LOCAL_LB;
	if (pefm->efm_remote_loopback_timeout != EFM_REM_LB_TIMEOUT_NONE)
	{

		EFM_TIMER_OFF (pefm->efm_rem_lb_timer);

		EFM_TIMER_ON_UNLOOP (pefm->efm_rem_lb_timer, efm_rem_lb_timer_expiry,
				pefm, pefm->efm_remote_loopback_timeout);
	}

	efm_tx (pefm, EFM_LOOPBACK_PDU);

	return EFM_SUCCESS;
}

#if 0
	int
efm_rem_lb_timer_expiry (struct thread *thread)
{
	struct efm_if *pefm = NULL;

    if(NULL == thread || NULL == thread->arg)
    {
		return 0;
    }

	pefm = (struct efm_if *)THREAD_ARG (thread);

	EFM_LOG_DBG("EFM[EVENT]: EFM Remote Loopback timer expired\n");

	if(NULL == pefm)
	{
		return 0;
	}

	//pefm->efm_rem_lb_timer = NULL;
	EFM_TIMER_OFF (pefm->efm_rem_lb_timer);

	if( EFM_REM_LB == pefm->rem_info.loopback)
	{
		return 0;
	}
	/* If the remote machine has not responded to remote loopback.
	   Exit from remote loopback mode */

	//send efm remote_loopback timeout alarm/event rise

	efm_remote_loopback_exit (pefm);

	//set local par_action FWD and mux_action FWD to hsl

	pefm->efm_local_mux_action_t = EFM_MUX_FWD;
	pefm->efm_local_par_action_t = EFM_PAR_FWD;
	//new add
	//pefm->efm_localside_make_remoteside_loopback_start = EFM_FALSE;
	pefm->efm_rem_loopback = EFM_FALSE;
	pefm->local_info.loopback = EFM_NONE_LB;

	//ipran_alarm_event_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_EVT_TYPE_FUN_EFM_REM_LP_TOUT,0);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = pefm->if_index;
	ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_FUN_EFM_REM_LP_TOUT,0);

	return EFM_SUCCESS;
}
#endif	
/*modify by youcheng 2018/8/26 due to timer change*/
	int
efm_rem_lb_timer_expiry (void * arg)
{
	struct efm_if *pefm = NULL;

	if(NULL == arg)
	{
		return 0;
	}

	pefm = (struct efm_if *)arg;

	EFM_LOG_DBG("EFM[EVENT]: EFM Remote Loopback timer expired\n");

    if(NULL == pefm)
    {
		return 0;
    }

	pefm->efm_rem_lb_timer = NULL;

    if( EFM_REM_LB == pefm->rem_info.loopback)
    {
		return 0;
    }
	/* If the remote machine has not responded to remote loopback.
	   Exit from remote loopback mode */

	//send efm remote_loopback timeout alarm/event rise

	efm_remote_loopback_exit (pefm);

	//set local par_action FWD and mux_action FWD to hsl

	pefm->efm_local_mux_action_t = EFM_MUX_FWD;
	pefm->efm_local_par_action_t = EFM_PAR_FWD;
	//new add
	//pefm->efm_localside_make_remoteside_loopback_start = EFM_FALSE;
	pefm->efm_rem_loopback = EFM_FALSE;
	pefm->local_info.loopback = EFM_NONE_LB;

	//ipran_alarm_event_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_EVT_TYPE_FUN_EFM_REM_LP_TOUT,0);

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = pefm->if_index;
	ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_FUN_EFM_REM_LP_TOUT,0);

	return EFM_SUCCESS;
}

/*efm exit remote loopback refer to document 802.3ah-2004 action 57.2.11.3 Exiting OAM remote loopback*/
	int
efm_remote_loopback_exit (struct efm_if *pefm)
{
	EFM_LOG_DBG("EFM[EVENT]: Exiting Remote Loopback procedure\n");

    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	if (pefm->efm_local_oam_mode == EFM_MODE_PASSIVE
			|| pefm->local_info.loopback != EFM_LOCAL_LB)                //LOCAL MAKE REMOTE  LOOPBAK
	{
		return EFM_SUCCESS;
	}

	//set local par_action FWD and mux_action FWD to hsl, if set fail, return error;

	//pefm->efm_localside_make_remoteside_loopback_start = EFM_FALSE;
	//pefm->efm_remoteside_make_localside_loopback_start = EFM_FALSE;
	pefm->local_info.loopback = EFM_NONE_LB;
	pefm->efm_rem_loopback= EFM_FALSE;

	pefm->efm_local_par_action_t = EFM_PAR_FWD;
	pefm->efm_local_mux_action_t = EFM_MUX_FWD;
	//pefm->efm_local_mux_action_t = EFM_MUX_DISCARD;
	EFM_TIMER_OFF (pefm->efm_rem_lb_timer);

	efm_tx (pefm, EFM_LOOPBACK_PDU);

	return EFM_SUCCESS;
}

/*efm set errored symbol event threshold*/
	s_int32_t
efm_symbol_period_threshold_set (struct efm_if *pefm,
		uint64_t threshold, efm_bool set)
{
    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	pefm->local_link_info.err_symbol_threshold = threshold;
	pefm->efm_link_monitor_t.symbol_period_threshold_set= set;

	EFM_LOG_DBG("EFM[EVENT]: Upper threshold of symbol period set to %llu\n", threshold);

	return EFM_SUCCESS;
}

/*efm set errored symbol event window*/
	s_int32_t
efm_symbol_period_window_set (struct efm_if *pefm,
		uint64_t window, efm_bool set)
{
    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	pefm->local_link_info.err_symbol_window = window;
	pefm->efm_link_monitor_t.symbol_period_window_set= set;

	EFM_LOG_DBG("EFM[EVENT]: Window size of symbol period set to %llu\n", window);

	return EFM_SUCCESS;
}

/*efm set errored frame event threshold*/
	s_int32_t
efm_err_frame_threshold_set (struct efm_if *pefm,
		uint32_t threshold, efm_bool set)
{
    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	pefm->local_link_info.err_frame_threshold = threshold;
	pefm->efm_link_monitor_t.frame_threshold_set = set;

	EFM_LOG_DBG("EFM[EVENT]:  threshold of Error frame Event is "
			"set to %d\n", threshold);

	return EFM_SUCCESS;
}

/*efm set errored frame event window*/
	s_int32_t
efm_err_frame_window_set (struct efm_if *pefm,
		uint16_t window, efm_bool set)
{
    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }


	pefm->local_link_info.err_frame_window = window;

	pefm->efm_link_monitor_t.frame_window_set = set;

	EFM_LOG_DBG("EFM[EVENT]: Window size of Error frame Event is "
			"set to %d\n", window);

	return EFM_SUCCESS;
}

/*efm set errored frame period event threshold*/
	s_int32_t
efm_err_frame_period_threshold_set (struct efm_if *pefm,
		uint32_t threshold, efm_bool set)
{
    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	pefm->local_link_info.err_frame_period_threshold = threshold;
	pefm->efm_link_monitor_t.frame_period_threshold_set = set;

	EFM_LOG_DBG("EFM[EVENT]: Lower threshold of Errored Frame Period Event"
			" is set to %u\n", threshold);

	return EFM_SUCCESS;
}

/*efm set errored frame period event window*/
	s_int32_t
efm_err_frame_period_window_set (struct efm_if *pefm,
		uint32_t window, efm_bool set)
{
    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	pefm->local_link_info.err_frame_period_window = window;
	pefm->efm_link_monitor_t.frame_period_window_set = set;

	EFM_LOG_DBG("EFM[EVENT]: Lower threshold of Errored Frame Period Event"
			" is set to %u\n", window);

	return EFM_SUCCESS;
}

/*efm set errored frame second event threshold*/
	s_int32_t
efm_err_frame_second_threshold_set (struct efm_if *pefm,
		uint16_t threshold, efm_bool set)
{
    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	pefm->local_link_info.err_frame_sec_sum_threshold = threshold;
	pefm->efm_link_monitor_t.frame_seconds_threshold_set = set;

	EFM_LOG_DBG("EFM[EVENT]: threshold of Errored Frame Seconds Event"
			" is set to %d\n", threshold);

	return EFM_SUCCESS;
}

/*efm set errored frame second window*/
	s_int32_t
efm_err_frame_second_window_set (struct efm_if *pefm,
		uint16_t window, efm_bool set)
{
    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	pefm->local_link_info.err_frame_sec_sum_window = window;
	pefm->efm_link_monitor_t.frame_seconds_window_set = set;

	EFM_LOG_DBG("EFM[EVENT]: threshold of Errored Frame Seconds Event"
			" is set to %d\n", window);

	return EFM_SUCCESS;
}

/*efm set all kinds of event happened  notify or no notify*/
	s_int32_t
efm_event_notify_set(struct efm_if *pefm, const char *str1,efm_bool is_set)
{


    if(NULL== pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	switch(strlen(str1))
	{      
		case 6:
			
            if(EFM_FALSE == is_set)
            {
				pefm->local_link_info.err_symbol_ev_notify_enable = EFM_NOTIFY_OFF;
            }
			else
            {
				pefm->local_link_info.err_symbol_ev_notify_enable = EFM_NOTIFY_ON;
            }
			break;
		case 5:
			
            if(EFM_FALSE == is_set)
            {
				pefm->local_link_info.err_frame_ev_notify_enable = EFM_NOTIFY_OFF;
            }
			else  
			{
				pefm->local_link_info.err_frame_ev_notify_enable = EFM_NOTIFY_ON;
            }
			
			break;
		case 12:
			if (!strcmp(str1, "frame-period"))
			{
				
                if(EFM_FALSE == is_set)
                {
					pefm->local_link_info.err_frame_period_ev_notify_enable = EFM_NOTIFY_OFF;
                }
				else
                {
					pefm->local_link_info.err_frame_period_ev_notify_enable = EFM_NOTIFY_ON;
                }
			}
			else if (!strcmp(str1, "frame-second"))
			{
				
                if(EFM_FALSE == is_set)
                {
					pefm->local_link_info.err_frame_sec_ev_notify_enable = EFM_NOTIFY_OFF;
                }
				else
                {
					pefm->local_link_info.err_frame_sec_ev_notify_enable = EFM_NOTIFY_ON;
                }
			}
			break;
		case 10:
			
            if(EFM_FALSE == is_set)
            {
				pefm->local_link_info.dying_gasp_enable = EFM_NOTIFY_OFF;
            }
			else
            {
				pefm->local_link_info.dying_gasp_enable = EFM_NOTIFY_ON;
            }

			break;
		case 14:
			
            if(EFM_FALSE == is_set)
            {
				pefm->local_link_info.critical_event_enable = EFM_NOTIFY_OFF;
            }
			else
            {
				pefm->local_link_info.critical_event_enable = EFM_NOTIFY_ON;
            }
			break;

		default:
			return EFM_FAILURE;
	}


	return EFM_SUCCESS;
}

/*efm set link discover time*/
	s_int32_t
efm_set_link_timer (struct efm_if *pefm, uint32_t secs)
{

	uint32_t expired_secs = 0;

    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	if (pefm->efm_link_timer)
	{
		//remain_secs = thread_timer_remain_second (pefm->efm_link_timer);

		/*last set time - remain sec = has run time*/
		//expired_secs = pefm->efm_link_time - remain_secs;
		if(!high_pre_timer_passtime(pefm->efm_link_timer,(int *)&expired_secs))
		{
			if (expired_secs > secs)
			{
				efm_discovery_fault (pefm);
				efm_discovery_state_machine (pefm);
			}
			else
			{
				EFM_TIMER_OFF (pefm->efm_link_timer);
				EFM_TIMER_ON_UNLOOP (pefm->efm_link_timer, efm_link_timer_expiry,
						pefm, secs - expired_secs); 
			}
		}
	}

	pefm->efm_link_time = secs;

	EFM_LOG_DBG("EFM[EVENT]: EFM link timer is set to %d secs\n", secs);

	return EFM_SUCCESS;
}

/*efm set rate to send PDU*/
	s_int32_t
efm_pdu_timer_set (struct efm_if *pefm, uint8_t secs)
{
    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	pefm->efm_pdu_time = secs;

	EFM_LOG_DBG("EFM[EVENT]: EFM pdu timer is set to %d secs\n", secs);

	return EFM_SUCCESS;
}
/*efm clear all kinds of send and recvie PDU of data statistics*/
	s_int32_t
efm_clear_statistics (struct efm_if *pefm)
{
    if(NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	pefm->tx_count [EFM_INFORMATION_PDU] = 0;
	pefm->rx_count [EFM_INFORMATION_PDU] = 0;
	pefm->tx_count [EFM_ERR_SYMBOL_PERIOD_EVENT_PDU] = 0;
	pefm->tx_count [EFM_ERR_FRAME_EVENT_PDU] = 0;
	pefm->tx_count [EFM_ERR_FRAME_PERIOD_EVENT_PDU] = 0;
	pefm->tx_count [EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU] = 0; 
	pefm->rx_count [EFM_ERR_SYMBOL_PERIOD_EVENT_PDU] = 0;
	pefm->rx_count [EFM_ERR_FRAME_EVENT_PDU] = 0;
	pefm->rx_count [EFM_ERR_FRAME_PERIOD_EVENT_PDU] = 0; 
	pefm->rx_count [EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU] = 0; 
	pefm->tx_count [EFM_LOOPBACK_PDU] = 0; 
	pefm->rx_count [EFM_LOOPBACK_PDU] = 0; 
	pefm->rx_count [EFM_PDU_INVALID] = 0;
	pefm->tx_count [EFM_ERR_SYMBOL_PERIOD_EVENT_PDU] = 0;

	if(pefm->aefm != NULL)
	{	
		pefm->tx_count[EFM_SPECIFIC_PDU] = 0;
		pefm->rx_count[EFM_SPECIFIC_PDU] = 0;
		memset(pefm->aefm->efm_tx_DifSpecific_pdu,0,sizeof(pefm->aefm->efm_tx_DifSpecific_pdu));
		memset(pefm->aefm->efm_rx_DifSpecific_pdu,0,sizeof(pefm->aefm->efm_rx_DifSpecific_pdu));
	}
	return EFM_SUCCESS;
}

/*efm show all kinds of recvie and send PDU statistics*/
	void
efm_show_if_statistics (struct efm_if *pefm, struct vty *vty)
{
    if(NULL == pefm)
    {
		return;
    }

	char device_name[10] = {0};
	vty_out (vty, "%s%s", pefm->if_name, VTY_NEWLINE);
	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "Counters:%s", VTY_NEWLINE);
	vty_out (vty, "----------%s", VTY_NEWLINE);

	vty_out (vty, "    Information OAMPDU Tx                  : %u%s",
			pefm->tx_count [EFM_INFORMATION_PDU], VTY_NEWLINE);
	vty_out (vty, "    Information OAMPDU Rx                  : %u%s",
			pefm->rx_count [EFM_INFORMATION_PDU], VTY_NEWLINE);
	if(pefm->aefm)
	{
	vty_out (vty, "                                                   %s", VTY_NEWLINE);
	vty_out (vty, "    efm agent specific TR:                               %s", VTY_NEWLINE);
	
	vty_out (vty, "    specific OAMPDU Tx                     : %u%s",
			pefm->tx_count [EFM_SPECIFIC_PDU], VTY_NEWLINE);	
	vty_out (vty, "    specific OAMPDU Rx                     : %u%s",
			pefm->rx_count [EFM_SPECIFIC_PDU], VTY_NEWLINE);
	
	vty_out (vty, "    specific device info OAMPDU Tx         : %u%s",	
			pefm->aefm->efm_tx_DifSpecific_pdu [EFM_AGENT_DEVICE_INFO_PDU], VTY_NEWLINE);
	vty_out (vty, "    specific device info OAMPDU Rx         : %u%s",
			pefm->aefm->efm_rx_DifSpecific_pdu[EFM_AGENT_DEVICE_INFO_PDU], VTY_NEWLINE);
	
	vty_out (vty, "    specific get info OAMPDU Tx            : %u%s",
			pefm->aefm->efm_tx_DifSpecific_pdu [EFM_AGENT_GET_INFO_PDU], VTY_NEWLINE);
	vty_out (vty, "    specific get info OAMPDU Rx            : %u%s",
			pefm->aefm->efm_rx_DifSpecific_pdu [EFM_AGENT_GET_INFO_PDU], VTY_NEWLINE);
	
	vty_out (vty, "    specific set info OAMPDU Tx            : %u%s",
			pefm->aefm->efm_tx_DifSpecific_pdu [EFM_AGENT_SET_INFO_PDU], VTY_NEWLINE);
	vty_out (vty, "    specific set info OAMPDU Rx            : %u%s",
			pefm->aefm->efm_rx_DifSpecific_pdu [EFM_AGENT_SET_INFO_PDU], VTY_NEWLINE);
	
	vty_out (vty, "    specific get set info OAMPDU Tx        : %u%s",
			pefm->aefm->efm_tx_DifSpecific_pdu [EFM_AGENT_SET_GET_INFO_PDU], VTY_NEWLINE);	
	vty_out (vty, "    specific get set info OAMPDU Rx        : %u%s",
			pefm->aefm->efm_rx_DifSpecific_pdu [EFM_AGENT_SET_GET_INFO_PDU], VTY_NEWLINE);

	if(pefm->aefm->efm_agent_link_flag && pefm->aefm->efm_agent_last_link_flag)
	{
	vty_out (vty, "    EFM agent IP                           : %u.%u.%u.%u%s",
		pefm->aefm->efm_agent_update_ip[0],pefm->aefm->efm_agent_update_ip[1],\
		pefm->aefm->efm_agent_update_ip[2],pefm->aefm->efm_agent_update_ip[3],VTY_NEWLINE);

	vty_out (vty, "    EFM agent ID 		           : %d%s",
			pefm->aefm->efm_agent_update_id,VTY_NEWLINE);
		vty_out (vty, "    EFM agent hostname 		       : %s%s",
						pefm->aefm->efm_agent_hostname,VTY_NEWLINE);
}
	switch (pefm->aefm->efm_agent_remote_device_tid)
	{
		case EFM_REMOTE_11000A_TYPE:
			memcpy(device_name,"11000A",strlen("11000A"));
			break;		
		case EFM_REMOTE_1101n_TYPE:
			memcpy(device_name,"1101n",strlen("1101n"));
			break;
		case EFM_REMOTE_11000An_TYPE:
			memcpy(device_name,"11000An",strlen("11000An"));
			break;
			
		default:
			memcpy(device_name,"unknow",strlen("unknow"));
			break;
	}	 
	vty_out (vty, "    EFM agented device                     : %s%s",device_name,VTY_NEWLINE);
	
	vty_out (vty, "                                                   %s", VTY_NEWLINE);
	}
	vty_out (vty, "    Event Notification OAMPDU Tx           : %u%s",
			pefm->tx_count [EFM_ERR_SYMBOL_PERIOD_EVENT_PDU] +
			pefm->tx_count [EFM_ERR_FRAME_EVENT_PDU] +
			pefm->tx_count [EFM_ERR_FRAME_PERIOD_EVENT_PDU] +
			pefm->tx_count [EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU], VTY_NEWLINE);
	vty_out (vty, "    Event Notification OAMPDU Rx           : %u%s",
			pefm->rx_count [EFM_ERR_SYMBOL_PERIOD_EVENT_PDU] +
			pefm->rx_count [EFM_ERR_FRAME_EVENT_PDU] +
			pefm->rx_count [EFM_ERR_FRAME_PERIOD_EVENT_PDU] +
			pefm->rx_count [EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU], VTY_NEWLINE);
	vty_out (vty, "    Loopback Control OAMPDU Tx             : %u%s",
			pefm->tx_count [EFM_LOOPBACK_PDU], VTY_NEWLINE);
	vty_out (vty, "    Loopback Control OAMPDU Rx             : %u%s",
			pefm->rx_count [EFM_LOOPBACK_PDU], VTY_NEWLINE);
	vty_out (vty, "    Unsupported OAMPDU Rx                  : %u%s",
			pefm->rx_count [EFM_PDU_INVALID], VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "Local event logs:%s", VTY_NEWLINE);
	vty_out (vty, "-----------------%s", VTY_NEWLINE);
	vty_out (vty, "    %d Errored Symbol Period records%s",
			pefm->tx_count [EFM_ERR_SYMBOL_PERIOD_EVENT_PDU], VTY_NEWLINE);
	vty_out (vty, "    %d Errored Frame records%s",
			pefm->tx_count [EFM_ERR_FRAME_EVENT_PDU], VTY_NEWLINE);
	vty_out (vty, "    %d Errored Frame Period records%s",
			pefm->tx_count [EFM_ERR_FRAME_PERIOD_EVENT_PDU], VTY_NEWLINE);
	vty_out (vty, "    %d Errored Frame Seconds records%s",
			pefm->tx_count [EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU], VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "Remote event logs:%s", VTY_NEWLINE);
	vty_out (vty, "-----------------%s", VTY_NEWLINE);
	vty_out (vty, "    %d Errored Symbol Period records%s",
			pefm->rx_count [EFM_ERR_SYMBOL_PERIOD_EVENT_PDU], VTY_NEWLINE);
	vty_out (vty, "    %d Errored Frame records%s",
			pefm->rx_count [EFM_ERR_FRAME_EVENT_PDU], VTY_NEWLINE);
	vty_out (vty, "    %d Errored Frame Period records%s",
			pefm->rx_count [EFM_ERR_FRAME_PERIOD_EVENT_PDU], VTY_NEWLINE);
	vty_out (vty, "    %d Errored Frame Seconds records%s",
			pefm->rx_count [EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU], VTY_NEWLINE);
	vty_out (vty, "%s", VTY_NEWLINE);
	vty_out (vty, "-----------------%s", VTY_NEWLINE);

	return;
}

/*efm show the recive and send data statistics of interface or configure */
	void
efm_show_statistics (struct vty *vty)
{
	struct efm_if *pefm = NULL;
	struct hash_bucket	*pbucket = NULL;
	int cursor, flag = 0;
	struct l2if *pif = NULL;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;

		if (pif != NULL)
		{
			pefm = pif->pefm;
			if (pefm != NULL)
			{
				efm_show_if_statistics(pefm, vty);
				flag = EFM_TRUE;
			}
		}
	}

    if(EFM_FALSE == flag)
	{	
		vty_out(vty,"%s", VTY_NEWLINE); 
		vty_out(vty,"%%Error: No EFM info, Maybe No Interface enable EFM. %s", VTY_NEWLINE); 
	}

	return;
}

/*show efm all kinds of initiating configuration*/
	void 
efm_show_if_interface (struct efm_if *pefm, struct vty *vty)
{
    if(NULL == pefm)
    {
		return ;
    }

	vty_out (vty, "%s%s", pefm->if_name, VTY_NEWLINE);
	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "Discovery State Machine Details:%s", VTY_NEWLINE);
	vty_out (vty, "--------------------------------%s", VTY_NEWLINE);

	vty_out (vty, "EFM Discovery Machine State:              %s%s",
			efm_discovery_state_to_str (pefm->efm_discovery_state), VTY_NEWLINE);

	vty_out (vty, "Local Parser State:                       %s%s",
			efm_par_state_to_str (pefm->efm_local_par_action_t), VTY_NEWLINE);

	vty_out (vty, "Local Multiplexer State:                  %s%s",
			efm_mux_state_to_str (pefm->efm_local_mux_action_t), VTY_NEWLINE);

	vty_out (vty, "Remote Parser State:                      %s%s",
			efm_par_state_to_str
			(pefm->rem_info.state & EFM_PAR_MASK), VTY_NEWLINE);

	vty_out (vty, "Remote Multiplexer State:                 %s%s",
			efm_mux_state_to_str
			(pefm->rem_info.state >> EFM_MUX_BIT_SHIFT), VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "Local Client:%s", VTY_NEWLINE);
	vty_out (vty, "-------------%s", VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Symbol Period Error:%s", VTY_NEWLINE);

	vty_out (vty, "        Window:                           %llu Symbols%s",
			pefm->local_link_info.err_symbol_window, VTY_NEWLINE);

	vty_out (vty, "        Threshold:                        %llu Symbols%s",
			pefm->local_link_info.err_symbol_threshold, VTY_NEWLINE);

	vty_out (vty, "        Last Window Symbols Errors:       %llu Symbols%s",
			pefm->local_link_info.err_symbols, VTY_NEWLINE);

	vty_out (vty, "        Total Symbols Errors:             %llu Symbols%s",
			pefm->local_link_info.err_symbols_total, VTY_NEWLINE);

	vty_out (vty, "        Total Symbols Errors Events:      %d Events%s",
			pefm->local_link_info.err_sym_event_total, VTY_NEWLINE);

	vty_out (vty, "        Relative Timestamp of the Event:  %u "
			"x 100 milliseconds%s",
			pefm->local_link_info.err_symbol_period_event_ts, VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Frame Error:%s", VTY_NEWLINE);

	vty_out (vty, "        Window:                           %u seconds%s",
			pefm->local_link_info.err_frame_window, VTY_NEWLINE);

	vty_out (vty, "        Threshold:                        %u Error Frames%s",
			pefm->local_link_info.err_frame_threshold, VTY_NEWLINE);

	vty_out (vty, "        Last Window Frame Errors:         %u Frames%s",
			pefm->local_link_info.err_frames, VTY_NEWLINE);

	vty_out (vty, "        Total Frame Errors:               %llu Frames%s",
			pefm->local_link_info.err_frame_total, VTY_NEWLINE);

	vty_out (vty, "        Total Frame Errors Events:        %u Events%s",
			pefm->local_link_info.err_frame_event_total, VTY_NEWLINE);

	vty_out (vty, "        Relative Timestamp of the Event:  %u "
			"x 100 milliseconds%s",
			pefm->local_link_info.err_frame_event_ts, VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Frame Period Error:%s", VTY_NEWLINE);

	vty_out (vty, "        Window:                           %u Frames%s",
			pefm->local_link_info.err_frame_period_window, VTY_NEWLINE);

	vty_out (vty, "        Threshold:                        %u Error Frames%s",
			pefm->local_link_info.err_frame_period_threshold, VTY_NEWLINE);

	vty_out (vty, "        Last Window Frame Errors:         %u Frames%s",
			pefm->local_link_info.err_frame_period_frames, VTY_NEWLINE);

	vty_out (vty, "        Total Frame Errors:               %llu Frames%s",
			pefm->local_link_info.err_frame_period_error_total, VTY_NEWLINE);

	vty_out (vty, "        Total Frame Period Errors Events: %u Events%s",
			pefm->local_link_info.err_frame_period_event_total, VTY_NEWLINE);

	vty_out (vty, "        Relative Timestamp of the Event:  %u "
			"x 100 milliseconds%s",
			pefm->local_link_info.err_frame_period_event_ts, VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Frame Seconds Error:%s", VTY_NEWLINE);

	vty_out (vty, "        Window:                           %u seconds%s",
			pefm->local_link_info.err_frame_sec_sum_window, VTY_NEWLINE);

	vty_out (vty, "        Threshold:                        %u Error Seconds%s",
			pefm->local_link_info.err_frame_sec_sum_threshold, VTY_NEWLINE);

	vty_out (vty, "        Last Window Frame Second Errors:  %u Error Seconds%s",
			pefm->local_link_info.err_frame_sec_error, VTY_NEWLINE);

	vty_out (vty, "        Total Frame Second Errors:        %u Error Seconds%s",
			pefm->local_link_info.err_frame_sec_error_total, VTY_NEWLINE);

	vty_out (vty, "        Total Frame Second Errors Events: %u Events%s",
			pefm->local_link_info.err_frame_sec_event_total, VTY_NEWLINE);

	vty_out (vty, "        Relative Timestamp of the Event:  %u "
			"x 100 milliseconds%s",
			pefm->local_link_info.err_frame_sec_sum_event_ts, VTY_NEWLINE);

    if(EFM_FALSE == pefm->efm_remote_state_valid)
    {
		return;
    }

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "Remote Client:%s", VTY_NEWLINE);
	vty_out (vty, "----------------%s", VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Symbol Period Error:%s", VTY_NEWLINE);

	vty_out (vty, "        Window:                           %llu Symbols%s",
			pefm->rem_link_info.err_symbol_window, VTY_NEWLINE);

	vty_out (vty, "        Threshold:                        %llu Symbols%s",
			pefm->rem_link_info.err_symbol_threshold, VTY_NEWLINE);

	vty_out (vty, "        Last Window Symbols Errors:       %llu Symbols%s",
			pefm->rem_link_info.err_symbols, VTY_NEWLINE);

	vty_out (vty, "        Total Symbols Errors:             %llu Symbols%s",
			pefm->rem_link_info.err_symbols_total, VTY_NEWLINE);

	vty_out (vty, "        Total Symbols Errors Events:      %u Events%s",
			pefm->rem_link_info.err_sym_event_total, VTY_NEWLINE);

	vty_out (vty, "        Relative Timestamp of the Event:  %u "
			"x 100 milliseconds%s",
			pefm->rem_link_info.err_symbol_period_event_ts, VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Frame Error:%s", VTY_NEWLINE);

	vty_out (vty, "        Window:                           %u seconds%s",
			pefm->rem_link_info.err_frame_window, VTY_NEWLINE);

	vty_out (vty, "        Threshold:                        %u Error Frames%s",
			pefm->rem_link_info.err_frame_threshold, VTY_NEWLINE);

	vty_out (vty, "        Last Window Frame Errors:         %u Frames%s",
			pefm->rem_link_info.err_frames, VTY_NEWLINE);

	vty_out (vty, "        Total Frame Errors:               %llu Frames%s",
			pefm->rem_link_info.err_frame_total, VTY_NEWLINE);

	vty_out (vty, "        Total Frame Errors Events:        %u Events%s",
			pefm->rem_link_info.err_frame_event_total, VTY_NEWLINE);

	vty_out (vty, "        Relative Timestamp of the Event:  %u "
			"x 100 milliseconds%s",
			pefm->rem_link_info.err_frame_event_ts, VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Frame Period Error:%s", VTY_NEWLINE);

	vty_out (vty, "        Window:                           %u Frames%s",
			pefm->rem_link_info.err_frame_period_window, VTY_NEWLINE);

	vty_out (vty, "        Threshold:                        %u Error Frames%s",
			pefm->rem_link_info.err_frame_period_threshold, VTY_NEWLINE);

	vty_out (vty, "        Last Window Frame Errors:         %u Frames%s",
			pefm->rem_link_info.err_frame_period_frames, VTY_NEWLINE);

	vty_out (vty, "        Total Frame Errors:               %llu Frames%s",
			pefm->rem_link_info.err_frame_period_error_total, VTY_NEWLINE);

	vty_out (vty, "        Total Frame Period Errors Events: %u Events%s",
			pefm->rem_link_info.err_frame_period_event_total, VTY_NEWLINE);

	vty_out (vty, "        Relative Timestamp of the Event:  %u "
			"x 100 milliseconds%s",
			pefm->rem_link_info.err_frame_period_event_ts, VTY_NEWLINE);


	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Frame Seconds Error:%s", VTY_NEWLINE);

	vty_out (vty, "        Window:                           %u seconds%s",
			pefm->rem_link_info.err_frame_sec_sum_window, VTY_NEWLINE);

	vty_out (vty, "        Threshold:                        %u Error Seconds%s",
			pefm->rem_link_info.err_frame_sec_sum_threshold, VTY_NEWLINE);

	vty_out (vty, "        Last Window Frame Second Errors:  %u Error Seconds%s",
			pefm->rem_link_info.err_frame_sec_error, VTY_NEWLINE);

	vty_out (vty, "        Total Frame Second Errors:        %u Error Seconds%s",
			pefm->rem_link_info.err_frame_sec_error_total, VTY_NEWLINE);

	vty_out (vty, "        Total Frame Second Errors Events: %u Events%s",
			pefm->rem_link_info.err_frame_sec_event_total, VTY_NEWLINE);

	vty_out (vty, "        Relative Timestamp of the Event:  %u x 100 milliseconds%s",
			pefm->rem_link_info.err_frame_sec_sum_event_ts, VTY_NEWLINE);
	vty_out (vty, "--------------------------------%s", VTY_NEWLINE);
	vty_out (vty, "%s", VTY_NEWLINE);
}

/*show efm the all kinds of initiating configuration of cofiguration or interface*/

	void
efm_show_interface (struct vty *vty)
{
	struct efm_if *pefm = NULL;
	struct hash_bucket	*pbucket = NULL;
	int cursor, flag = 0;
	struct l2if *pif = NULL;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;

		if (pif != NULL)
		{
			pefm = pif->pefm;
			if (pefm != NULL)
			{
				efm_show_if_interface(pefm, vty);
				flag = EFM_TRUE;
			}
		}
	}

    if( EFM_FALSE == flag)
	{	
		vty_out(vty,"%s", VTY_NEWLINE); 
		vty_out(vty,"%%Error: No EFM info, Maybe No Interface enable EFM. %s", VTY_NEWLINE); 
	}

	return;
}

/*show efm discovery info*/
	void
efm_show_if_discovery (struct efm_if *pefm, struct vty *vty)
{
    if(NULL == pefm)
    {
		return;
    }

	vty_out (vty, "%s%s", pefm->if_name, VTY_NEWLINE);
	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "Local client:%s", VTY_NEWLINE);
	vty_out (vty, "-------------%s", VTY_NEWLINE);

	vty_out (vty, "  Administrative configurations:%s", VTY_NEWLINE);

	vty_out (vty, "     Mode:                              %s %s",
			pefm->efm_local_oam_mode == EFM_MODE_ACTIVE ? "active":"passive", VTY_NEWLINE);

	vty_out (vty, "     Unidirection:                      %s %s",
			CHECK_FLAG (pefm->local_info.oam_config,
				EFM_CFG_UNI_DIR_SUPPORT) ? "support" : "not support", VTY_NEWLINE);

	vty_out (vty, "     Link monitor:                      %s%s %s",
			CHECK_FLAG (pefm->local_info.oam_config,
				EFM_CFG_LINK_EVENT_SUPPORT)
			? "support" : "not support",
			CHECK_FLAG (pefm->local_link_event_flags,
				EFM_LINK_EVENT_ON)
			? "(on)" : "(off)", VTY_NEWLINE);

	vty_out (vty, "     Remote Loopback:                   %s %s",
			CHECK_FLAG (pefm->local_info.oam_config,
				EFM_CFG_REM_LB_SUPPORT)
			? "support" : "not support", VTY_NEWLINE);

	vty_out (vty, "     MIB retrieval:                     not support%s", VTY_NEWLINE);
	vty_out (vty, "     MTU Size    :                      %d%s",
			pefm->local_info.oam_pdu_config, VTY_NEWLINE);

	vty_out (vty, "  Operational status:%s", VTY_NEWLINE);

	vty_out (vty, "     Port status:                       %s %s",
			pefm->efm_local_link_status == EFM_OAM_LINK_UP
			? "operational" : "not operational", VTY_NEWLINE);

	vty_out (vty, "     Remote Loopback status:            %s %s",
			pefm->efm_rem_loopback ? "loopback" : "no loopback", VTY_NEWLINE);

	vty_out (vty, "     PDU revision:                      %d %s",
			pefm->local_info.revision, VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	if (pefm->efm_remote_state_valid == EFM_FALSE)
		return;

	vty_out (vty, "Remote client:%s", VTY_NEWLINE);
	vty_out (vty, "--------------%s", VTY_NEWLINE);

	vty_out (vty, "  MAC address: %02x%02x.%02x%02x.%02x%02x%s",
			pefm->rem_info.mac_addr[0], pefm->rem_info.mac_addr[1],
			pefm->rem_info.mac_addr[2], pefm->rem_info.mac_addr[3],
			pefm->rem_info.mac_addr[4], pefm->rem_info.mac_addr[5],
			VTY_NEWLINE);

	vty_out (vty, "  Vendor(oui): %x %x %x%s",
			pefm->rem_info.oui [0], pefm->rem_info.oui [1],
			pefm->rem_info.oui [2], VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "  Administrative configurations:%s", VTY_NEWLINE);

	vty_out (vty, "     Mode:                              %s %s",
			CHECK_FLAG (pefm->rem_info.oam_config, EFM_CFG_OAM_MODE) ?
			"active":"passive", VTY_NEWLINE);

	vty_out (vty, "     Unidirection:                      %s %s",
			CHECK_FLAG (pefm->rem_info.oam_config,
				EFM_CFG_UNI_DIR_SUPPORT)
			? "support" : "not support", VTY_NEWLINE);

	vty_out (vty, "     Link monitor:                      %s %s",
			CHECK_FLAG (pefm->rem_info.oam_config,
				EFM_CFG_LINK_EVENT_SUPPORT)
			? "support" : "not support", VTY_NEWLINE);

	vty_out (vty, "     Remote Loopback:                   %s %s",
			CHECK_FLAG (pefm->rem_info.oam_config,
				EFM_CFG_REM_LB_SUPPORT)
			? "support" : "not support", VTY_NEWLINE);

	vty_out (vty, "     MIB retrieval:                     %s %s",
			CHECK_FLAG (pefm->rem_info.oam_config,
				EFM_CFG_VAR_RESPONSE_SUPPORT)
			? "support" : "not support", VTY_NEWLINE);

	vty_out (vty, "     MTU Size    :                      %d%s",
			pefm->rem_info.oam_pdu_config, VTY_NEWLINE);

	vty_out (vty, "--------------%s", VTY_NEWLINE);
	vty_out (vty, "%s", VTY_NEWLINE);

	return;
}

/*show efm the discovery info of configartion or interface*/
	void
efm_show_discovery (struct vty *vty)
{
	struct efm_if *pefm = NULL;
	struct hash_bucket	*pbucket = NULL;
	int cursor, flag = 0;
	struct l2if *pif = NULL;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;

		if (pif != NULL)
		{
			pefm = pif->pefm;
			if (pefm != NULL)
			{
				efm_show_if_discovery(pefm, vty);
				flag = EFM_TRUE;
			}
		}
	}

    if(EFM_FALSE == flag)
	{	
		vty_out(vty,"%s", VTY_NEWLINE); 
		vty_out(vty,"%%Error: No EFM info, Maybe No Interface enable EFM. %s", VTY_NEWLINE); 
	}

	return;
}

/*show efm satus info*/
	void
efm_show_if_status (struct efm_if *pefm, struct vty *vty)
{
    if(NULL == pefm)
    {
		return;
    }

	vty_out (vty, "%s%s", pefm->if_name, VTY_NEWLINE);
	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "General:%s", VTY_NEWLINE);
	vty_out (vty, "-------%s", VTY_NEWLINE);

	vty_out (vty, "   Mode:                              %s %s",
			pefm->efm_local_oam_mode == EFM_MODE_ACTIVE ? "active":"passive", VTY_NEWLINE);


	vty_out (vty, "   PDU min rate:                      1 packet per %d second%s",
			pefm->efm_pdu_time, VTY_NEWLINE);

	vty_out (vty, "   Link timeout:                      %d seconds%s",
			pefm->efm_link_time, VTY_NEWLINE);
	vty_out (vty, "   Remote loopback  timeout:          %d seconds%s",
			pefm->efm_remote_loopback_timeout, VTY_NEWLINE);
	

	vty_out(vty, "   Remote loopback mac swap:          %s%s",
			pefm->efm_mac_swap?"support":"not support",VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);
	
	vty_out(vty,"LInk Event Notify:%s",VTY_NEWLINE);
	vty_out (vty, "----------------%s", VTY_NEWLINE);

          vty_out(vty,"  Symbol period event               %s%s",pefm->local_link_info.err_symbol_ev_notify_enable == EFM_NOTIFY_OFF?"(no)":"(yes)",VTY_NEWLINE);
          vty_out(vty,"  Frame event                       %s%s",pefm->local_link_info.err_frame_ev_notify_enable == EFM_NOTIFY_OFF?"(no)":"(yes)",VTY_NEWLINE);
	 vty_out(vty,"  Frame period event                %s%s",pefm->local_link_info.err_frame_period_ev_notify_enable == EFM_NOTIFY_OFF?"(no)":"(yes)",VTY_NEWLINE);
	 vty_out(vty,"  Frame sec event                   %s%s",pefm->local_link_info.err_frame_sec_ev_notify_enable== EFM_NOTIFY_OFF?"(no)":"(yes)",VTY_NEWLINE);
	 vty_out(vty,"  Dying gasp                        %s%s",pefm->local_link_info.dying_gasp_enable== EFM_NOTIFY_OFF?"(no)":"(yes)",VTY_NEWLINE);
	 vty_out(vty,"  Critical event                    %s%s",pefm->local_link_info.critical_event_enable== EFM_NOTIFY_OFF?"(no)":"(yes)",VTY_NEWLINE);
	 vty_out (vty, "%s", VTY_NEWLINE);
	 
	 

	vty_out (vty, "Link Monitoring:%s", VTY_NEWLINE);
	vty_out (vty, "----------------%s", VTY_NEWLINE);

	vty_out (vty, "     Status:                          %s%s %s",
			CHECK_FLAG (pefm->local_info.oam_config,
				EFM_CFG_LINK_EVENT_SUPPORT)
			? "support" : "not support",
			CHECK_FLAG (pefm->local_link_event_flags,
				EFM_LINK_EVENT_ON)
			? "(on)" : "(off) ", VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Event log size:                  %d Entries %s",
			pefm->max_event_log_entries, VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Symbol Period Error:%s", VTY_NEWLINE);

	vty_out (vty, "        Window:                       %llu symbols%s",
			pefm->local_link_info.err_symbol_window, VTY_NEWLINE);

	vty_out (vty, "        Threshold:                    %llu error symbols%s",
			pefm->local_link_info.err_symbol_threshold, VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Frame Error:%s", VTY_NEWLINE);

	vty_out (vty, "        Window:                       %u seconds%s",
			pefm->local_link_info.err_frame_window, VTY_NEWLINE);

	vty_out (vty, "        Threshold:                    %u error frames%s",
			pefm->local_link_info.err_frame_threshold, VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Frame Period Error:%s", VTY_NEWLINE);

	vty_out (vty, "        Window:                       %u  frames%s",
			pefm->local_link_info.err_frame_period_window, VTY_NEWLINE);

	vty_out (vty, "        Threshold:                    %u error frames%s",
			pefm->local_link_info.err_frame_period_threshold, VTY_NEWLINE);

	vty_out (vty, "%s", VTY_NEWLINE);

	vty_out (vty, "     Frame Seconds Error:%s", VTY_NEWLINE);

	vty_out (vty, "        Window:                       %u seconds%s",
			pefm->local_link_info.err_frame_sec_sum_window, VTY_NEWLINE);

	vty_out (vty, "        Threshold:                    %u error seconds%s",
			pefm->local_link_info.err_frame_sec_sum_threshold, VTY_NEWLINE);
	vty_out (vty, "----------------%s", VTY_NEWLINE);
	vty_out (vty, "%s", VTY_NEWLINE);

	return;
}

/*show efm the staus info of configuration or interface*/
	void
efm_show_status (struct vty *vty)
{
	struct efm_if *pefm = NULL;
	struct hash_bucket	*pbucket = NULL;
	int cursor, flag = 0;
	struct l2if *pif = NULL;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;

		if (pif != NULL)
		{
			pefm = pif->pefm;
			if (pefm != NULL)
			{
				efm_show_if_status(pefm, vty);
				flag = EFM_TRUE;
			}
		}
	}

    if(EFM_FALSE == flag)
	{	
		vty_out(vty,"%s", VTY_NEWLINE); 
		vty_out(vty,"%%Error: No EFM info, Maybe No Interface enable EFM. %s", VTY_NEWLINE); 
	}

	return;
}

/*show efm eventlog info*/
	void
efm_show_if_event_log (struct efm_if *pefm, struct vty *vty)
{
	struct listnode *node = NULL;
	struct efm_event_log_entry *eventlog = NULL;
	char* event_type = NULL;
	uint32_t index = 0;

    if(NULL == pefm)
    {
		return;
    }

	vty_out (vty, "IFNAME: %s%s", pefm->if_name, VTY_NEWLINE);
	vty_out (vty, "--------------------------------%s", VTY_NEWLINE);
	vty_out (vty, "%s", VTY_NEWLINE);

	EFM_LIST_LOOP (pefm->efm_if_eventlog_list, eventlog, node)
	{
		if (!eventlog)
        {
			return;
        }

		vty_out (vty, "\nEvent Log details for logIndex: %u %s", 
				eventlog->event_log_Index, VTY_NEWLINE);

		vty_out (vty, "-----------------------------------%s", VTY_NEWLINE); 

		vty_out (vty, "Time stamp of event       : %llu %s", 
				eventlog->event_log_timestamp, VTY_NEWLINE);

		vty_out (vty, "oui                       : %02x %02x %02x%s",
				eventlog->event_log_oui[0],
				eventlog->event_log_oui[1],
				eventlog->event_log_oui[2], 
				VTY_NEWLINE);    

		if (eventlog->event_log_type <= EFM_ORG_SPEC_EVENT)
		{
			event_type = (char *)efm_link_event_type_str[eventlog->event_log_type - 1]; 
		}
		else if (eventlog->event_log_type <= EFM_CRITICAL_LINK_EVENT)
		{      
			index = eventlog->event_log_type - 0xfb ;

			if(index <= 7)
			{
				event_type = (char *)efm_link_event_type_str[index];
			}
		}

		vty_out (vty, "event type                : %s %s", 
				(!event_type) ?  "NULL" : event_type, VTY_NEWLINE);

		vty_out (vty, "event location            : %s %s", 
				(eventlog->event_log_location == EFM_EVENT_LOCAL)?
				"LOCAL":"REMOTE", VTY_NEWLINE);

		if (eventlog->event_log_type <= EFM_ERR_FRAME_SECONDS_SUM_EVENT)
		{
			/*This event type has window and threshold associated with it.*/
			vty_out (vty, "window             : %llu %s", 
					eventlog->event_log_window, VTY_NEWLINE);
			vty_out (vty, "threshold          : %llu %s", 
					eventlog->event_log_threshold, VTY_NEWLINE);
			vty_out (vty, "error value        : %llu %s", 
					eventlog->event_log_value, VTY_NEWLINE);
			vty_out (vty, "total error value  : %llu %s", 
					eventlog->event_log_running_total, VTY_NEWLINE);
			vty_out (vty, "total event count  : %u %s", 
					eventlog->event_log_event_total, VTY_NEWLINE);
		}
	}
	vty_out (vty, "%s", VTY_NEWLINE);
	vty_out (vty, "-----------------------------------%s", VTY_NEWLINE); 

	return;
}

/*show efm the eventlog info of configuration or interface*/
	void
efm_show_event_log (struct vty *vty)
{
	struct efm_if *pefm = NULL;
	struct hash_bucket	*pbucket = NULL;
	int cursor, flag = 0;
	struct l2if *pif = NULL;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;

		if (pif != NULL)
		{
			pefm = pif->pefm;
			if (pefm != NULL)
			{
				efm_show_if_event_log(pefm, vty);
				flag = EFM_TRUE;
			}
		}
	}

    if(EFM_FALSE == flag)
	{	
		vty_out(vty,"%s", VTY_NEWLINE); 
		vty_out(vty,"%%Error: No EFM info, Maybe No Interface enable EFM. %s", VTY_NEWLINE); 
	}

	return;
}


/*save efm no-default command configuration*/
void l2if_efm_config_write(struct vty *vty, struct efm_if *pefm)
{
	char ipv4_addr[20] = {0};
	uint32_t ip_cmp = 0;
	
	vty_out(vty, " efm enable%s", VTY_NEWLINE);
	 

   if(pefm->aefm)
   {
		 
	vty_out(vty, " efm agent enable%s", VTY_NEWLINE);
		
		if(memcmp(pefm->aefm->efm_agent_update_ip,&ip_cmp,4))
		{
			
		sprintf(ipv4_addr, "%u.%u.%u.%u",\
			pefm->aefm->efm_agent_update_ip[0], 
			pefm->aefm->efm_agent_update_ip[1],
			pefm->aefm->efm_agent_update_ip[2],
			pefm->aefm->efm_agent_update_ip[3]);
		vty_out(vty," efm agent-ip %s%s",ipv4_addr,VTY_NEWLINE);
		}
		if(memcmp(&pefm->aefm->efm_agent_update_id,&ip_cmp,4))
		{
		vty_out(vty," efm agent-id %d%s",pefm->aefm->efm_agent_update_id,VTY_NEWLINE);
		}
		if(memcmp(pefm->aefm->efm_agent_hostname,"Hios",strlen("Hios")))
		{
			vty_out(vty," efm agent-hostname %s%s",pefm->aefm->efm_agent_hostname,VTY_NEWLINE);
		}
   }
	if(EFM_MODE_ACTIVE == pefm->efm_local_oam_mode)    
	{
		vty_out (vty, " efm mode active%s", VTY_NEWLINE);
    }
	
	if (pefm->local_link_info.err_symbol_ev_notify_enable != EFM_NOTIFY_ON)

    {
		vty_out (vty, " no efm event-notify symbol%s", VTY_NEWLINE);
    }

	if (pefm->local_link_info.err_frame_ev_notify_enable != EFM_NOTIFY_ON)

    {
		vty_out (vty, " no efm event-notify frame%s", VTY_NEWLINE);
    }

	if (pefm->local_link_info.err_frame_period_ev_notify_enable != EFM_NOTIFY_ON)

    {
		vty_out (vty, " no efm event-notify frame-period%s", VTY_NEWLINE);
    }

	if (pefm->local_link_info.err_frame_sec_ev_notify_enable != EFM_NOTIFY_ON)
		
    {
		vty_out (vty, " no efm event-notify frame-second%s", VTY_NEWLINE);
    }

	if (pefm->local_link_info.dying_gasp_enable != EFM_NOTIFY_ON)		

    {
		vty_out (vty, " no efm event-notify dying-gasp%s", VTY_NEWLINE);
    }

	if (pefm->local_link_info.critical_event_enable != EFM_NOTIFY_ON)		

    {
		vty_out (vty, " no efm event-notify critical-event%s", VTY_NEWLINE);
    }

	if (pefm->efm_link_time != EFM_LINK_TIMER)
    {
		vty_out (vty, " efm timeout %u%s", pefm->efm_link_time, VTY_NEWLINE);
    }

	if (pefm->efm_pdu_time != EFM_PDU_TIMER)
    {
		vty_out (vty, " efm rate %u%s", pefm->efm_pdu_time, VTY_NEWLINE);
    }
	if(pefm->efm_mac_swap)
    {
		vty_out(vty," efm remote-loopback mac-swap%s",VTY_NEWLINE);
    }
	if (!CHECK_FLAG (pefm->local_info.oam_config, EFM_CFG_REM_LB_SUPPORT))
	{
			vty_out (vty, " no efm remote-loopback support%s", VTY_NEWLINE);
	}
         #if 1
	if (pefm->efm_rem_loopback)
    {
		vty_out (vty, " efm remote-loopback enable%s", VTY_NEWLINE);
    }
         #endif
		 

	if (pefm->efm_remote_loopback_timeout != EFM_REM_LB_LINK_TIMER)
		vty_out (vty, " efm remote-loopback timeout %u%s",
				pefm->efm_remote_loopback_timeout, VTY_NEWLINE);

	if (CHECK_FLAG (pefm->local_info.oam_config, EFM_CFG_UNI_DIR_SUPPORT))
    {
		vty_out (vty, " efm unidirectional-link support%s", VTY_NEWLINE);
    }

	if (!CHECK_FLAG (pefm->local_info.oam_config, EFM_CFG_LINK_EVENT_SUPPORT))
    {
		vty_out (vty, " no efm link-monitor support%s", VTY_NEWLINE);
    }

	if (pefm->efm_link_monitor_t.symbol_period_threshold_set || 
			pefm->efm_link_monitor_t.symbol_period_window_set)
	{
		vty_out (vty, " efm link-monitor symbol-period");

		if (pefm->efm_link_monitor_t.symbol_period_threshold_set)
        {
			vty_out (vty, " threshold %llu", pefm->local_link_info.err_symbol_threshold);
        }

		if (pefm->efm_link_monitor_t.symbol_period_window_set)
        {
			vty_out (vty, " window %llu", pefm->local_link_info.err_symbol_window);
        }

		vty_out (vty, "%s", VTY_NEWLINE);
	}

	if (pefm->efm_link_monitor_t.frame_threshold_set || 
			pefm->efm_link_monitor_t.frame_window_set)
	{
		vty_out (vty, " efm link-monitor frame");

		if (pefm->efm_link_monitor_t.frame_threshold_set)
        {
			vty_out (vty, " threshold %u", pefm->local_link_info.err_frame_threshold);
        }

		if (pefm->efm_link_monitor_t.frame_window_set)
        {
			vty_out (vty, " window %u", pefm->local_link_info.err_frame_window);
        }

		vty_out (vty, "%s", VTY_NEWLINE);
	}

	if (pefm->efm_link_monitor_t.frame_period_threshold_set || 
			pefm->efm_link_monitor_t.frame_period_window_set)
	{
		vty_out (vty, " efm link-monitor frame-period");

		if (pefm->efm_link_monitor_t.frame_period_threshold_set)
        {
			vty_out (vty, " threshold %u", pefm->local_link_info.err_frame_period_threshold);
        }

		if (pefm->efm_link_monitor_t.frame_period_window_set)
        {
			vty_out (vty, " window %u", pefm->local_link_info.err_frame_period_window);
        }

		vty_out (vty, "%s", VTY_NEWLINE);
	}

	if (pefm->efm_link_monitor_t.frame_seconds_threshold_set || 
			pefm->efm_link_monitor_t.frame_seconds_window_set)
	{
		vty_out (vty, " efm link-monitor frame-seconds");

		if (pefm->efm_link_monitor_t.frame_seconds_threshold_set)
        {
			vty_out (vty, " threshold %u", pefm->local_link_info.err_frame_sec_sum_threshold);
        }

		if (pefm->efm_link_monitor_t.frame_seconds_window_set)
        {
			vty_out (vty, " window %u", pefm->local_link_info.err_frame_sec_sum_window);
        }

		vty_out (vty, "%s", VTY_NEWLINE);
	}

	if (CHECK_FLAG (pefm->local_link_event_flags, EFM_LINK_EVENT_ON))
    {
		vty_out (vty, " efm link-monitor enable%s", VTY_NEWLINE);
    }

	if(pefm->max_event_log_entries != EFM_MAX_EVLOG_ENTRIES_DEF)
		vty_out (vty, " efm event-log-number %u%s", 
				pefm->max_event_log_entries, VTY_NEWLINE);

	return;
}


void efm_init(void)
{
	efm_cmd_init();
	efm_pkt_register();
	efm_pthread_dyinggasp_event_check();

}

void efm_deinit(void)
{
	efm_pkt_unregister();
}
void efm_clean_port(struct efm_if *pefm)
{
	if(pefm!=NULL)
	{
         XFREE (MTYPE_EFM, pefm);
	pefm = NULL;
	}

}


