
/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_init.c
*
*  date: 2017.3
*
*  modify:2017.10~2017.12
*
*  modify:	2018.3.12 modified by liufuying to make mstp module code beautiful
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "ifm_common.h"
#include "log.h"
#include "lib/log.h"
#include "lib/memory.h"
#include "lib/memtypes.h"
#include "module_id.h"


#include "mstp.h"
#include "mstp_sm.h"
#include "mstp_port.h"
#include "mstp_bpdu.h"
#include "mstp_cli.h"
#include "mstp_msg.h"
#include "mstp_timer.h"
#include "mstp_init.h"
#include "mstp_base_procedure.h"


/*when bridge init, malloc bridge*/
struct mstp_bridge			*mstp_global_bridge = NULL;
struct mstp_vlan			bridge_cist_vlan_map;
struct mstp_vlan 			global_ins_vlan_map[MSTP_INSTANCE_MAX];

extern unsigned char		l2_mac[MAC_LEN];


void mstp_init(void)
{
	int ret	= -1;
	
	/*creat mstp bridge*/
	ret = mstp_creat();
	if(ret < 0)
	{
		/*calloc bridge memery error!
		or mstp_get_mac_addr() error!*/
		return;
	}

	/*init cmd*/
	mstp_vty_init();

	mstp_pkt_register();

	mstp_timer_start(NULL);

	return;
}

int mstp_creat(void)
{
	int						ret = -1;
	unsigned int			ins_id = 0;

	/*have create mstp bridge*/
	if(mstp_global_bridge != NULL)
	{
		MSTP_LOG_DBG("%s: mstp create new bridge error, (mstp_global_bridge != NULL)!\n", __func__);
		return MSTP_RESULT_OK;
	}

	/*malloc mstp bridge*/
	mstp_global_bridge = (struct mstp_bridge*)XMALLOC(MTYPE_GLOBAL_BR, sizeof(struct mstp_bridge));
	if(NULL == mstp_global_bridge)
	{		
		MSTP_LOG_DBG("%s: ERROR: calloc bridge memery error!\n", __func__);
		return MSTP_RESULT_ERROR;
	}

	/*clear mstp_global_bridge*/
	memset(mstp_global_bridge, 0, sizeof(struct mstp_bridge));

	/*default global bridge priority*/
	mstp_global_bridge->bridge_mac.pri[0] = (unsigned char)(DEF_BRIDGE_PRIORITY >> 8);
	mstp_global_bridge->bridge_mac.pri[1] = (unsigned char)DEF_BRIDGE_PRIORITY;

	/*get mac addr*/
	if((ret = mstp_get_mac_addr()) < 0)
	{
		MSTP_LOG_DBG("%s: mstp mstp_get_mac_addr() error!\n", __func__);
		return MSTP_RESULT_ERROR;			
	}


	/*mstp mode init: default is mstp mode*/
	mstp_global_bridge->stp_mode = PROTO_VERID_DEF;

	mstp_bridge_init(mstp_global_bridge);

	/*default tick timer*/
//	mstp_global_bridge->mstp_tick_timer = 100;
/*1000ms  is 1s, we change it to 100ms*/
	mstp_global_bridge->mstp_tick_timer = 100;

	/*init port_head*/
	INIT_LIST_HEAD(&mstp_global_bridge->port_head);

	/*create cist instance 0*/
	mstp_create_cist_instance();

	/*mstp init MSTP_MSTI_MAX instances*/
	for(ins_id = 1; ins_id <= MSTP_INSTANCE_MAX; ins_id++)
	{
		mstp_instance_init(ins_id);
	}
	/*MST Region:vlan and instance mapping*/
	mstp_gen_cfg_digest(mstp_global_bridge);

	/*MST Region:vlan and instance mapping*/
	mstp_gen_cfg_digest(mstp_global_bridge);

	return MSTP_RESULT_OK;
}

int mstp_get_mac_addr(void)
{
	if (NULL == mstp_global_bridge)
	{
		return MSTP_RESULT_ERROR;
	}

	memcpy(mstp_global_bridge->bridge_mac.mac_addr, &l2_mac[0], MAC_LEN);

	return MSTP_RESULT_OK;
}

void mstp_bridge_init(struct mstp_bridge *mstp_br)
{
	/*802.1Q-2011~Table 13-1 Configuration Digest Signature Key*/
	/*Table 13-2 Sample Configuration Digest Signature Keys*/
	/*need to modify*/
	/*this is deal look for hash digest key*/
	unsigned char 				digest_key[16] = {0x13, 0xAC, 0x06, 0xA6, 0x2E, 
									0x47, 0xFD, 0x51,0xF9, 0x5D, 0x2B, 0xA2,
									0x43, 0xCD, 0x03, 0x46};

	unsigned int				i = 0;
	struct mstp_common_br		*common_br = NULL;

	if ((NULL == mstp_global_bridge) || (NULL == mstp_br))
	{
		return;
	}

	common_br = &mstp_br->common_br;
	common_br->begin				= MSTP_TRUE;

	/*init mst info*/
	mstp_br->mst_cfg_id.cfg_format_id_selector = MSTP_DEFAULT_SELECTOR;

	/*config name : default is mac addr*/
	memset(&mstp_br->mst_cfg_id.cfg_name[0], 0, NAME_LEN);
	for(i = 0; i < MAC_LEN; i++)
	{
		sprintf((char *)&mstp_br->mst_cfg_id.cfg_name[2*i], "%02X", mstp_global_bridge->bridge_mac.mac_addr[i]);
	}

	/*revison level*/
	mstp_br->mst_cfg_id.revison_level = MSTP_DEFAULT_RV_LEVEL;
	memcpy(&mstp_br->mst_cfg_id.cfg_digest[0], digest_key, DIGEST_LEN);

	/*bridge priority*/
	mstp_br->br_priority		= DEF_BRIDGE_PRIORITY;

	/*config master and salve role in this bridge*/
	mstp_br->bridge_root_role	= MSTP_ROOT_DEF;

	/*instance times variable 
	**the value of hello_time/fwd_delay/msg_max_age should be 
	**Satisfy the following formula, or network will frequently
	2*(Forward Delay-1.0 second) >= Max Age
	Max Age >= 2*(Hello Time + 1.0 second)
	*/
	mstp_br->hello_time			= MSTP_DEFAULT_HELLO_TIME;
	mstp_br->fwd_delay			= MSTP_DEFAULT_FWD_DELAY;
	mstp_br->msg_max_age		= MSTP_DEFAULT_MAX_AGE;
	mstp_br->msg_max_hop_count	= MSTP_DEFAULT_MAX_HOP;


	/*init common bridge value*/
	common_br->force_pt_version		= PROTO_VERID_MSTP;
	common_br->tx_hold_count		= 6;	//we do not care, but in RFC is 6
	common_br->migrate_time			= 3;	//we do not care, but in RFC is 3
	common_br->bridge_max_age		= MSTP_DEFAULT_MAX_AGE;
	common_br->max_hops				= MSTP_DEFAULT_MAX_HOP;

	/*init base func*/
	common_br->edge_delay			= (MSTP_PORT_FUN)mstp_edge_delay;
	common_br->pdu_older            = (MSTP_PORT_FUN)mstp_pdu_older;
	common_br->forward_delay		= (MSTP_PORT_FUN)mstp_forward_delay;	
	common_br->fwd_delay			= (MSTP_PORT_FUN)mstp_fwd_delay;
	common_br->rerooted				= (MSTP_MSTI_FUN)mstp_rerooted;
	common_br->rstp_version			= (MSTP_PORT_FUN)mstp_rstp_version;
	common_br->stp_version			= (MSTP_PORT_FUN)mstp_stp_version;
	common_br->all_synced			= (MSTP_MSTI_FUN)mstp_all_synced;
	common_br->hello_time			= (MSTP_PORT_FUN)mstp_hello_time;
	common_br->max_age				= (MSTP_PORT_FUN)mstp_max_age;
	common_br->all_transmit_ready	= (MSTP_PORT_FUN)mstp_all_transmit_ready;
	common_br->cist					= (MSTP_CIST)mstp_cist;
	common_br->cist_root_port		= (MSTP_PORT_FUN)mstp_cist_root_port;
	common_br->cist_designated_port	= (MSTP_PORT_FUN)mstp_cist_designated_port;
	
	common_br->msti_designated_port = (MSTP_PORT_FUN)mstp_msti_designated_port;
	common_br->msti_master_port		= (MSTP_PORT_FUN)mstp_msti_master_port;
	common_br->rcvd_any_msg			= (MSTP_PORT_FUN)mstp_rcvd_any_msg;
	common_br->rcvd_cist_msg		= (MSTP_PORT_FUN)mstp_rcvd_cist_msg;
	common_br->rcvd_msti_msg		= (MSTP_MSTI_FUN)mstp_rcvd_msti_msg;
	common_br->restricted_role		= (MSTP_PORT_FUN)mstp_restricted_role;
	common_br->restricted_tcn		= (MSTP_PORT_FUN)mstp_restricted_tcn;
	common_br->updt_cist_info		= (MSTP_PORT_FUN)mstp_updt_cist_info;
	common_br->updt_msti_info		= (MSTP_MSTI_FUN)mstp_updt_msti_info;

	/*init msti bridge*/
	for (i = 0; i < MSTP_INSTANCE_MAX; i++)
	{
		mstp_br->msti_br[i] = NULL;
	}

	return;
}

void mstp_create_cist_instance(void)
{	
	struct mstp_cist_br		*cist_br = NULL;
	int						i = 0;	/*instance id*/

	if (NULL == mstp_global_bridge)
	{
		return;
	}

	cist_br = &mstp_global_bridge->cist_br;
	
	INIT_LIST_HEAD(&cist_br->port_head);

	/* 4096 = 512 * 8; VLAN ID is 0-4095 bit*/
	/*map [1-4094] vlan id, (vlan id 4095 is out of control)*/
	for(i = 0; i < MSTP_VLAN_MAP_SIZE-1; i++)
	{
		if(i == 0)
		{
			bridge_cist_vlan_map.vlan_map[i] = 0xFE;
		}
		else
		{
			bridge_cist_vlan_map.vlan_map[i] = 0xFF;
		}
	}
	bridge_cist_vlan_map.vlan_map[i] = 0x7F;

	cist_br->root_port = NULL;
	cist_br->port_role_select_state_begin		= MSTP_TRUE;
	cist_br->port_role_select_state 			= 0;

	cist_br->bridge_times.max_age 			   = MSTP_DEFAULT_MAX_AGE;
	cist_br->bridge_times.hello_time 		   = MSTP_DEFAULT_HELLO_TIME;
	cist_br->bridge_times.forward_delay 	   = MSTP_DEFAULT_FWD_DELAY;
	cist_br->bridge_times.remaining_hops 	   = MSTP_DEFAULT_MAX_HOP;

	cist_br->bridge_mac 					   = mstp_get_br_id(MSTP_DEFAULT_BRIDGE_PRI, 0);
	cist_br->bridge_priority.root_id 		   = cist_br->bridge_mac;
	cist_br->bridge_priority.external_root_path_cost 	= 0;
	cist_br->bridge_priority.regional_root_id 			= cist_br->bridge_mac;
	cist_br->bridge_priority.internal_root_path_cost 	= 0;
	cist_br->bridge_priority.designated_bridge_id 		= cist_br->bridge_mac;
	cist_br->bridge_priority.designated_port_id 		= 0;
	cist_br->bridge_priority.recv_port_id 				= 0;

/*mstp MST */
	mstp_mst_digest_recalc();	//FIXME
	
	return;
}


int mstp_instance_init(unsigned int instance_id)
{
	struct mstp_msti_br		*msti_bridge = NULL;

	if (NULL == mstp_global_bridge)
	{
		return MSTP_RESULT_ERROR;
	}

/*init instance*/
	mstp_global_bridge->mstp_instance[instance_id-1].next = NULL;

	/*this instance list of all msti_port*/
	mstp_global_bridge->mstp_instance[instance_id-1].msti_port_list = NULL;

	/*this instance id*/
	mstp_global_bridge->mstp_instance[instance_id-1].instance_id = instance_id;

	/*this instance of work region vlan*/
	mstp_global_bridge->mstp_instance[instance_id-1].msti_vlan.vlan_start = 0;
	mstp_global_bridge->mstp_instance[instance_id-1].msti_vlan.vlan_end = 0;
	memset(mstp_global_bridge->mstp_instance[instance_id-1].msti_vlan.vlan_map, 0x00, MSTP_VLAN_MAP_SIZE);

	/*this instance of bridge priority*/
	/*range<1-614440>,
	**when msti_bridge_root_role is master config priority is 0
	**when msti_bridge_root_role is slave config priority is 4096*/
	mstp_global_bridge->mstp_instance[instance_id-1].msti_br_priority = DEF_BRIDGE_PRIORITY;
	mstp_global_bridge->mstp_instance[instance_id-1].AdminPathCost = MSTP_FALSE;

/*init msti bridge*/
	msti_bridge = mstp_global_bridge->msti_br[instance_id-1];

	if(msti_bridge == NULL)
	{
		/*malloc msti bridge space*/
		msti_bridge = (struct mstp_msti_br *)XMALLOC(MTYPE_MSTI_BR, sizeof(struct mstp_msti_br));
		if(msti_bridge == NULL)
		{
			MSTP_LOG_DBG("%s: ERROR: calloc msti_bridge memery error!\n", __func__);
			return MSTP_RESULT_ERROR;
		}
	}
	
	/*memset msti bridge space*/
	memset(msti_bridge, 0, sizeof(struct mstp_msti_br));

	mstp_global_bridge->msti_br[instance_id-1] = msti_bridge;

	msti_bridge->port_role_select_state_begin = MSTP_TRUE;
	msti_bridge->port_role_select_state = 0;
	
	msti_bridge->bridge_mac.pri[0] = (unsigned char)(mstp_global_bridge->mstp_instance[instance_id-1].msti_br_priority >> 8);
	msti_bridge->bridge_mac.pri[1] = (unsigned char)mstp_global_bridge->mstp_instance[instance_id-1].msti_br_priority;
	memcpy(msti_bridge->bridge_mac.mac_addr, mstp_global_bridge->bridge_mac.mac_addr, MAC_LEN);

	msti_bridge->bridge_priority.rg_root_id = msti_bridge->bridge_mac;
	msti_bridge->bridge_priority.internal_root_path_cost = 0;
	msti_bridge->bridge_priority.designated_bridge_id = msti_bridge->bridge_mac;
	msti_bridge->bridge_priority.designated_port_id = 0;
	msti_bridge->bridge_priority.recv_port_id = 0;
	
	msti_bridge->bridge_times.remaining_hops = mstp_global_bridge->common_br.max_hops;
	msti_bridge->root_port = NULL;
	INIT_LIST_HEAD(&msti_bridge->port_head);
	
#if 0	/*need to confirm, maybe need to delete*/
	vlan_map_pointer = (unsigned char *)&mstp_global_bridge->mstp_instance[instance_id].msti_vlan.vlan_map;
	/*change instance-vlan map*/
	mstp_change_vlan_map(vlan_map_pointer, instance_id);
#endif

	global_ins_vlan_map[instance_id-1].vlan_start = mstp_global_bridge->mstp_instance[instance_id-1].msti_vlan.vlan_start;
	global_ins_vlan_map[instance_id-1].vlan_end = mstp_global_bridge->mstp_instance[instance_id-1].msti_vlan.vlan_end;
	memcpy(&global_ins_vlan_map[instance_id-1].vlan_map[0], &mstp_global_bridge->mstp_instance[instance_id-1].msti_vlan.vlan_map[0], MSTP_VLAN_MAP_SIZE);

	/*in fact base vlan add port to instance id, but ipran enable port by manual operation*/
	//FIXME


	/*If ForceProtocolVersion or MSTConfigId are modified by
	management, BEGIN shall be asserted for all state machines.	
	2011~13.24 Per bridge variables.  2005~802.1Q 13.23.1 Page 163*/
	//mstp_reinit();

	return MSTP_RESULT_OK;
}

/*If ForceProtocolVersion or MSTConfigId are modified by management, BEGIN shall be 
asserted for all state machines. 
a).2011~13.24 Per bridge variables.
b).2005~802.1Q 13.23.1 Page 163*/
void mstp_reinit(void)
{
	unsigned int			ins_id = 0;
	struct mstp_port 		*mstp_port = NULL, *next = NULL;
	struct mstp_cist_br		*cist_br = NULL;
	struct mstp_msti_br		*msti_br = NULL;
	struct mstp_cist_port	*cist_port = NULL;
	struct mstp_msti_port	*msti_port = NULL;

	if (NULL == mstp_global_bridge)
	{
		return;
	}

	/*get mstp info: we need mstp_status & protocol_type*/
	list_for_each_entry_safe(mstp_port, next, &mstp_global_bridge->port_head, port_list)
	{
		/*set begin need to set true*/
		mstp_port->port_variables.sm_timer_state_begin		= MSTP_TRUE;
		mstp_port->port_variables.sm_rcv_state_begin		= MSTP_TRUE;
		mstp_port->port_variables.sm_migration_state_begin	= MSTP_TRUE;
		mstp_port->port_variables.sm_tx_state_begin			= MSTP_TRUE;
		mstp_port->port_variables.sm_br_detect_state_begin	= MSTP_TRUE;

		/*port State machine status init*/
		mstp_port->port_variables.sm_timer_state		= 0;
		mstp_port->port_variables.sm_rcv_state			= 0;
		mstp_port->port_variables.sm_migration_state	= 0;
		mstp_port->port_variables.sm_tx_state			= 0;
		mstp_port->port_variables.sm_br_detect_state	= 0;

		/*update port link status*/
		mstp_update_link_state(mstp_port);

		/*port State machine status init*/
		cist_port = &mstp_port->cist_port;

		/*set cist port sm begin need to set true*/
		cist_port->common_info.sm_info_state_begin			= MSTP_TRUE;
		cist_port->common_info.sm_disable_role_state_begin	= MSTP_TRUE;
		cist_port->common_info.sm_state_trans_state_begin	= MSTP_TRUE;
		cist_port->common_info.sm_tplg_change_state_begin	= MSTP_TRUE;

		/*set cist port sm status*/
		cist_port->common_info.sm_info_state			= 0;
		cist_port->common_info.sm_disable_role_state	= 0;
		cist_port->common_info.sm_master_role_state		= 0;
		cist_port->common_info.sm_root_role_state		= 0;
		cist_port->common_info.sm_deg_role_state		= 0;
		cist_port->common_info.sm_ab_role_state			= 0;
		cist_port->common_info.sm_state_trans_state		= 0;
		cist_port->common_info.sm_tplg_change_state		= 0;
		cist_port->common_info.role						= 0;

		/*all msti port init*/
		for(ins_id = 0; ins_id < MSTP_INSTANCE_MAX; ins_id++)
		{
			msti_port = mstp_port->msti_port[ins_id];
			if(msti_port != NULL)
			{
				/*set msti port sm begin need to set true*/
				msti_port->common_info.sm_info_state_begin			= MSTP_TRUE;
				msti_port->common_info.sm_disable_role_state_begin	= MSTP_TRUE;
				msti_port->common_info.sm_state_trans_state_begin	= MSTP_TRUE;
				msti_port->common_info.sm_tplg_change_state_begin	= MSTP_TRUE;

				/*set msti port sm status*/
				msti_port->common_info.sm_info_state			= 0;
				msti_port->common_info.sm_disable_role_state	= 0;
				msti_port->common_info.sm_master_role_state		= 0;
				msti_port->common_info.sm_root_role_state		= 0;
				msti_port->common_info.sm_deg_role_state		= 0;
				msti_port->common_info.sm_ab_role_state			= 0;
				msti_port->common_info.sm_state_trans_state		= 0;
				msti_port->common_info.sm_tplg_change_state		= 0;
				msti_port->common_info.role						= 0;
			}
		}

		/*bug#51828  add mstp about trunk pathcost calculate function*/
		/*<stp>trunk口上应用stp功能，对应的链路开销值不正确*/
		mstp_trunk_if_speed_get(mstp_port);
	}

	cist_br = &mstp_global_bridge->cist_br;

	/*set cist bridge sm begin need to set true
	**cist port role status
	*/
	cist_br->port_role_select_state_begin = MSTP_TRUE;
	cist_br->port_role_select_state	= 0;

	for(ins_id = 0; ins_id < MSTP_INSTANCE_MAX; ins_id++)
	{
		msti_br = mstp_global_bridge->msti_br[ins_id];
		if(msti_br != NULL)
		{		
			/*set msti bridge sm begin need to set true*/
			msti_br->port_role_select_state_begin = MSTP_TRUE;
			msti_br->port_role_select_state = 0;			
		}
	}

	/*get mstp enable status: always true*/
#if 0
	if((mstp_enable_status == MSTP_ENABLE)
		&& (protocol_type != PROTO_DISBALE))
	{
		/*run states*/
		mstp_state_machine_normal_action(mstp_global_bridge);
	}
#else


	mstp_state_machine_normal_action(mstp_global_bridge);

#endif

	return;
}


/*init mstp port value to default*/
void l2_stp_port_init(struct mstp_port *mstp_port, unsigned int if_index)
{
	unsigned int				i = 0;
	unsigned int				instance_id = 0;
	unsigned char				status = 0;

	if ((NULL == mstp_port) || (NULL == mstp_global_bridge))
	{
		return;
	}

	/*store mstp port if_index*/
	mstp_port->ifindex = if_index;

	/*get mstp port interface name*/
	ifm_get_name_by_ifindex(if_index , (char *)mstp_port->if_name);

	/*get mstp port link status*/
	if(ifm_get_link(if_index, MODULE_ID_L2, &status) == 0)
	{
		if(IFNET_LINKUP == status)
		{
			mstp_port->link_status = PORT_LINK_UP;
		}
		else
		{
			mstp_port->link_status = PORT_LINK_DOWN;
		}			
	}
	else
	{		
		mstp_port->link_status = PORT_LINK_DOWN;
	}

	/*default is mstp mode*/
	mstp_port->stp_mode = mstp_global_bridge->stp_mode;

	/*always true*/
//	mstp_port->mcheck = MSTP_TRUE;

	/*in steps of 16, default is 128*/
	mstp_port->port_pri = DEF_PORT_PRIORITY;

	/*port cost value*/
	mstp_port->port_path_cost = DEF_PORT_PATH_COST;

	/*msti port default value*/
	for(instance_id = 0; instance_id < MSTP_INSTANCE_MAX; instance_id++)
	{
		/*port priority*/
		mstp_port->msti_port_pri[instance_id] = DEF_PORT_PRIORITY;

		/*port cost*/
		mstp_port->msti_port_path_cost[instance_id] = DEF_PORT_PATH_COST;
	}


	/*edge port role
	**the port don't participate in the calculation of spanning tree
	*/
	mstp_port->mstp_edge_port = MSTP_FALSE;

	/*filter port role
	**the port do not deal and send bpdu, if value is true*/
	mstp_port->mstp_filter_port = MSTP_FALSE;

	/*mstp port link type*/
	mstp_port->mstp_p2p = MSTP_FALSE;

	/*used for bpdu protect function
	**when if interface cfg bpdu protect, and because this interface down*/
	mstp_port->bpdu_down = MSTP_FALSE;

	/*default is false*/
	mstp_port->port_variables.restrictedTcn = MSTP_FALSE;
	mstp_port->port_variables.mcheck					= MSTP_TRUE;

/*init mstp_port_variables begin flag to make sm run*/
	mstp_port->port_variables.sm_timer_state_begin		= MSTP_TRUE;
	mstp_port->port_variables.sm_rcv_state_begin		= MSTP_TRUE;
	mstp_port->port_variables.sm_migration_state_begin 	= MSTP_TRUE;
	mstp_port->port_variables.sm_tx_state_begin 		= MSTP_TRUE;
	mstp_port->port_variables.sm_br_detect_state_begin 	= MSTP_TRUE;

	mstp_port->port_variables.helloWhen = mstp_global_bridge->hello_time;
	mstp_port->port_variables.mdelayWhile = 0;

	/*in fact: port vlaue is common use for [cist_port & msti_port & (stp & rstp) ports]*/
	mstp_port->port_variables.AdminEdge 	= MSTP_FALSE;
	mstp_port->port_variables.AutoEdge 		= MSTP_TRUE;
	mstp_port->port_variables.AdminLink 	= MSTP_FALSE;

	/*when the port exist mst instances more than one*/
	for(i = 0; i < MSTP_INSTANCE_MAX; i++)
	{
		mstp_port->msti_port[i] = NULL;
	}

	mstp_port->m_br = mstp_global_bridge;

	/*store recv msg mst*/
	memset(&mstp_port->mst_cfg_id, 0x00, sizeof(struct mst_config_id));

/*used for mstp bpdu*/
	/*frame buff*/
	memset(mstp_port->tx_frame, 0x00, MSTP_MAX_FRAME_SIZE);
	memset(mstp_port->rx_frame, 0x00, MSTP_MAX_FRAME_SIZE);

	/*port send bpdu statics*/
	mstp_port->bpdu_config_send	= 0;
	mstp_port->bpdu_rst_send	= 0;
	mstp_port->bpdu_mst_send	= 0;
	mstp_port->bpdu_tc_send		= 0;
	mstp_port->bpdu_tcn_send	= 0;

	/*port recv bpdu statics*/
	mstp_port->bpdu_config_recv	= 0;
	mstp_port->bpdu_rst_recv	= 0;
	mstp_port->bpdu_mst_recv	= 0;
	mstp_port->bpdu_tc_recv		= 0;
	mstp_port->bpdu_tcn_recv	= 0;

	return;
}

