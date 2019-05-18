
/*
 *  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
 *
 *  liufy@huahuan.com 
 *
 *  file name: mstp_msg.c
 *
 *  date: 2017.3
 *
 *  modify:2017.10~2017.12
 *
 */
#include <stdio.h>
#include <string.h>

#include "lib/log.h"
#include "module_id.h"
#include "lib/pkt_type.h"
#include "lib/hash1.h"
#include "lib/memory.h"
#include "command.h"


#include "mstp_bpdu.h"
#include "mstp.h"
#include "mstp_port.h"
#include "mstp_base_procedure.h"
#include "mstp_init.h"
#include "mstp_msg.h"
#include "mstp_sm_procedure.h"



/**************************FIXME********************************/

/*need to add vlan change register*/
/*need to add trunk change register*/
/***************************************************************/

extern struct mstp_bridge	*mstp_global_bridge;
extern struct mstp_vlan		bridge_cist_vlan_map;
extern struct mstp_vlan		global_ins_vlan_map[MSTP_INSTANCE_MAX];




void mstp_pkt_register(void)
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(union proto_reg));
	proto.ethreg.dmac[0] = MSTP_DEST_MAC_0;
	proto.ethreg.dmac[1] = MSTP_DEST_MAC_1;
	proto.ethreg.dmac[2] = MSTP_DEST_MAC_2;
	proto.ethreg.dmac[3] = MSTP_DEST_MAC_3;
	proto.ethreg.dmac[4] = MSTP_DEST_MAC_4;
	proto.ethreg.dmac[5] = MSTP_DEST_MAC_5;

	proto.ethreg.dmac_valid = 1;
	proto.ethreg.ethtype = 0x4242;
	proto.ethreg.sub_ethtype = 0x03;

	pkt_register(MODULE_ID_L2, PROTO_TYPE_ETH, &proto);

	return;
}
/************************************************
 * Function:    mstp_instance_add_mgt_vlan
 * Author:      youcheng
 * Date:        2018-10-19
 * Input:       manage vlan
 * 
 * Output:
 NONE
 * Return:
 *      1  successed 0 fail
 * Description:
 *      Instance 1 add manage vlan.
 ************************************************/


#if 0
int mstp_instance_add_mgt_vlan(int manage_vlan)
{


	unsigned int				instance_id = 0;
	unsigned char				vlan_map[MSTP_VLAN_MAP_SIZE];
	unsigned int				i = 0;
	uint8_t                     ret = 0;

	struct mstp_instance		*instance = NULL;


	/*vlan id range check*/
	if((manage_vlan < MSTP_VLAN_MIN_ID) || (manage_vlan > MSTP_VLAN_MAX_ID))
	{	
		printf("%s[%d]Manage vlan [%d] more than range. vlan id is <1-4094>.Mstp Function invalid",\
				__FUNCTION__,__LINE__,manage_vlan);	

		return MSTP_FALSE;
	}

	/*get vlan id map*/
	memset(vlan_map, 0, MSTP_VLAN_MAP_SIZE);

	mstp_vlan_map_set(vlan_map, manage_vlan);


	/*To determine whether a vlan belongs to other instances*/
	for(i = 0; i < MSTP_INSTANCE_MAX; i++)
	{
		if(MSTP_TRUE == mstp_cmp_vlan_map(vlan_map, global_ins_vlan_map[i].vlan_map))
		{
			MSTP_LOG_DBG(" MST instacne[%d] have the same vlan have configed",i+1);
			return MSTP_FALSE;
		}
	}

	/*add manage vlan to instance 1*/
	instance_id = MSTP_INSTANCE_MIN;

	if(NULL == mstp_global_bridge)
	{
		zlog_err ( "%s[%d]:leave %s:error:mstp init failure!\n", __FILE__, __LINE__, __func__);
		return MSTP_FALSE;

	}

	/*set MSTP mode*/
	mstp_global_bridge->stp_mode = PROTO_VERID_MSTP;

	/*copy vlan to instance vlan_map*/
	/*example:	instance 1: vlan 1-10, vlan 30-40
	 **			instance 2: vlan 11-29, vlan 41-50*/
	global_ins_vlan_map[instance_id-1].vlan_start = manage_vlan;
	global_ins_vlan_map[instance_id-1].vlan_end = manage_vlan;
	
	instance = &mstp_global_bridge->mstp_instance[instance_id-1];
	instance->msti_vlan.vlan_start = manage_vlan;
	instance->msti_vlan.vlan_end = manage_vlan;

	/*only add one vlan*/
	for(i = 0; i < MSTP_VLAN_MAP_SIZE; i++)
	{
		instance->msti_vlan.vlan_map[i]  = vlan_map[i];
		global_ins_vlan_map[instance_id-1].vlan_map[i] = vlan_map[i];
	}
	

	/*restart init cist vlan[1-4094]*/
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

	/*modify cist vlan id map, delete vlan map from cist map*/
	mstp_vlan_map_unset(bridge_cist_vlan_map.vlan_map, manage_vlan);

	/*all port enable stp*/
	ret = mstp_all_port_enable();
	if(0 == ret)
	{
		zlog_err("%s[%d]:%s: ERROR: port enable mstp fail!\n", __FILE__, __LINE__, __func__);
		return MSTP_FALSE;

	}
	
	/*if mode is not mstp mode , port no add to msti*/
	if(mstp_global_bridge->stp_mode == PROTO_VERID_MSTP)
	{
		/*vlan add, modify msti port*/
		mstp_add_delete_msti_port_by_instancevlan(instance_id);
	}
	
	MSTP_LOG_DBG(" Instance[%d] config vlan list: vlan start[%d], vlan end[%d]",\
			instance_id, instance->msti_vlan.vlan_start, instance->msti_vlan.vlan_end);

	/*MST Region:vlan and instance mapping*/
	mstp_gen_cfg_digest(mstp_global_bridge);	
	mstp_reinit();

	return MSTP_TRUE;

}
/************************************************
 * Function:    mstp_instance_delete_mgt_vlan
 * Author:      youcheng
 * Date:        2018-10-19
 * Input:       NULL
 * 
 * Output:
 NONE
 * Return:
 *      1  successed 0 fail
 * Description:
 *      Instance 1 delete manage vlan.
 ************************************************/

int mstp_instance_delete_mgt_vlan()
{


	unsigned int				instance_id = 0;
	unsigned int				i = 0;
	uint8_t                     ret = 0;

	struct mstp_instance		*instance = NULL;


	/*delete manage vlan from instance 8*/
	instance_id = MSTP_INSTANCE_MIN;

	if(NULL == mstp_global_bridge)
	{
		zlog_err ( "%s[%d]:leave %s:error:mstp init failure!\n", __FILE__, __LINE__, __func__);
	
		return MSTP_FALSE;

	}

	/*set MSTP mode*/
	mstp_global_bridge->stp_mode = PROTO_VERID_MSTP;

	/*copy vlan to instance vlan_map*/
	/*example:	instance 1: vlan 1-10, vlan 30-40
	 **			instance 2: vlan 11-29, vlan 41-50*/
	memset(&global_ins_vlan_map[instance_id-1],0,sizeof(struct mstp_vlan));

	instance = &mstp_global_bridge->mstp_instance[instance_id-1];

	memset(&instance->msti_vlan,0,sizeof(struct mstp_vlan));


	/*restart init cist vlan[1-4094]*/
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



	/*if mode is not mstp mode , port no add to msti*/
	if(mstp_global_bridge->stp_mode == PROTO_VERID_MSTP)
	{
		/*vlan add, modify msti port*/
		mstp_add_delete_msti_port_by_instancevlan(instance_id);
	}
	

	/*all port disable stp*/
	ret = mstp_all_port_disable();
	if(0 == ret)
	{
		MSTP_LOG_DBG("%s[%d]:%s: ERROR: port disable mstp fail!\n", __FILE__, __LINE__, __func__);
		return MSTP_FALSE;

	}
	/*MST Region:vlan and instance mapping*/
	mstp_gen_cfg_digest(mstp_global_bridge);
	
	mstp_reinit();

	return MSTP_TRUE;

}

/************************************************
 * Function:     mstp_all_port_enable
 * Author:      youcheng
 * Date:        2018-10-19
 * Input:       NULL
 * 
 * Output:
 NULL
 * Return:
 *      0   successed
 * Description:
 *      all port enable mstp.
 ************************************************/
int  mstp_all_port_enable()
{

	uint32_t                    if_index = 0;
	struct l2if 				*pif = NULL;
	struct hash_bucket			*pbucket = NULL;
	int 						cursor = 0;
	struct mstp_port			*mstp_port = NULL;

	HASH_BUCKET_LOOP(pbucket,cursor,l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}

		/*judge interface mode*/
		if(IFNET_MODE_INVALID == pif->mode)
		{
			zlog_err("%s[%d]:%s: ERROR: The port mode is invalid, mstp can not enabled!\n", __FILE__, __LINE__, __func__);
			continue;
		}
		else if(IFNET_MODE_L3 == pif->mode)
		{
			zlog_err("%s[%d]:%s: ERROR: The port mode is L3, mstp cmd can not config!\n", __FILE__, __LINE__, __func__);
			continue; 	
		}
		/*judge mstp port interspace exist status*/
		mstp_port = pif->mstp_port_info;
		if(NULL == mstp_port)	/*port is not exist*/
		{
			/*malloc l2if_stp_port size*/
			pif->mstp_port_info = (struct mstp_port *)XMALLOC(MTYPE_MSTP_PORT, sizeof(struct mstp_port));
			if(NULL == pif->mstp_port_info)
			{
				zlog_err("%s[%d]:%s: ERROR: L2 stp port calloc memery error!\n", __FILE__, __LINE__, __func__);
				return MSTP_FALSE;
			}
			mstp_port = pif->mstp_port_info;
			if_index = pif->ifindex;

			/*init mstp port variable*/
			l2_stp_port_init(pif->mstp_port_info, if_index);

			/*update link state*/
			mstp_get_link_state(mstp_port, if_index);

			/*when this mstp port enable mstp function,  add mstp port to [cist_mstp_port_list]*/
			mstp_add_cist_port(pif->mstp_port_info);

			/*add port to [mstp bridge mstp-port-list]*/
			INIT_LIST_HEAD(&mstp_port->port_list);
			mstp_add_mport(pif->mstp_port_info);

			mstp_port->adminStatus = MSTP_TRUE;

			/*add instance_id(0), vlan_bitmap, port, state(DISCARDING) */
			//STG_MODIFY
			MSTP_LOG_DBG("%s:mstp_add_stg  port(%s), instance_id = 0, state(%s)!\n",
					__func__, mstp_port->if_name, "MSTP_STATE_BLOCKING");	
			
			mstp_add_stg_vlan(0, mstp_port, MSTP_STATE_BLOCKING);

		}

	}

	return MSTP_TRUE;
}
/************************************************
 * Function:     mstp_all_port_disable
 * Author:      youcheng
 * Date:        2018-10-19
 * Input:       NULL
 * 
 * Output:
 NULL
 * Return:
 *      0   successed
 * Description:
 *      all port disable mstp.
 ************************************************/

int  mstp_all_port_disable()
{

	struct l2if 				*pif = NULL;
	struct hash_bucket			*pbucket = NULL;
	int 						cursor = 0;
	struct mstp_port			*mstp_port = NULL;

	HASH_BUCKET_LOOP(pbucket,cursor,l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}
		/*judge mstp port interspace exist status*/

		/*judge interface mode*/
		if(IFNET_MODE_INVALID == pif->mode)
		{
			zlog_err("%s[%d]:%s: ERROR: The port mode is invalid, mstp can not disabled!\n", __FILE__, __LINE__, __func__);
			continue;
		}
		else if(IFNET_MODE_L3 == pif->mode)
		{
			zlog_err("%s[%d]:%s: ERROR: The port mode is L3, mstp cmd can not config!\n", __FILE__, __LINE__, __func__);
			continue; 	
		}

		mstp_port = pif->mstp_port_info;
		if(NULL == mstp_port)	/*port is not exist*/
		{
			zlog_err("%s[%d]:%s: ERROR: The port mstp not enabled, can not (no stp enabled) cmd!\n", __FILE__, __LINE__, __func__);
			continue; 	
		}

		/*mstp port mstp disable*/
		mstp_port->adminStatus = MSTP_FALSE;

		/*delete bridge port list*/
		/*delete cist port*/
		list_del(&mstp_port->port_list);
		mstp_del_cist_port(mstp_port);

		/*add instance_id(0), vlan_bitmap, port, state(FORWARDING) */
		//STG_MODIFY
		MSTP_LOG_DBG("%s: mstp_add_stg port(%s), instance_id = 0, state(%s)!\n",
				__func__, mstp_port->if_name, "MSTP_STATE_REMOVE");	
		
		mstp_add_stg_vlan(0, mstp_port, MSTP_STATE_REMOVE);


		XFREE(MTYPE_MSTP_PORT, mstp_port);
		pif->mstp_port_info = NULL;


	}
	return MSTP_TRUE;
}
#endif
