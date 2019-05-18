
/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_cli.c
*
*  date: 2017.4
*
*  modify:2017.10~2017.12
*	if add and set stg  lookup (STG_MODIFY)
*	if modify mode change , lookup (MODE_CHANGE)
*
*  modify:	2018.3.12 modified by liufuying to make mstp module code beautiful
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"
#include "lib/log.h"
#include "lib/hash1.h"
#include "lib/memory.h"

#include "ifm_common.h"

#include "l2_if.h"
#include "trunk.h"

#include "mstp.h"
#include "mstp_msg.h"
#include "mstp_port.h"
#include "mstp_init.h"
#include "mstp_show.h"
#include "mstp_base_procedure.h"
#include "mstp_sm_procedure.h"
#include "mstp_cli.h"

extern struct mstp_bridge	*mstp_global_bridge;
extern struct mstp_vlan		bridge_cist_vlan_map;
extern struct mstp_vlan		global_ins_vlan_map[MSTP_INSTANCE_MAX];
extern struct hash_table 	l2if_table;


/* stp node */
static struct cmd_node mstp_node =
{
    MSTP_NODE,
    "%s(config-mstp-instance)# ",
    1,
};


DEFUN(mstp_mode,
	mstp_mode_cmd,
	"mstp",
	"in mstp mode\n")
{
	vty->node = MSTP_NODE;

	return CMD_SUCCESS;
}

/*******************************global config*****************************/
/*region name*/
DEFUN (stp_region_name,
        stp_region_name_cmd,
        "stp region-name NAME",
        MSTP_STR
        "Specify region name\n"
        "Region name, A maximum of 32 characters can be entered\n")
{
	unsigned char			region_name_len = 0;
	unsigned char			old_region_name_len = 0;
	
	unsigned char			cfg_name[NAME_LEN] = {0};
	memset(&cfg_name[0], 0, NAME_LEN);

	region_name_len = strlen(argv[0]);
	if((region_name_len < 1) || (region_name_len > NAME_LEN))
	{	
		MSTP_LOG_DBG("%s[%d]:%s:Error: region name len is error, please input right name(lenth)! \n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," region name len is error, please input right name(lenth)! %s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	old_region_name_len = strlen((const char *)&mstp_global_bridge->mst_cfg_id.cfg_name);

	memcpy(cfg_name, argv[0], region_name_len);

	if(old_region_name_len == region_name_len)
	{
		if(strncmp((const char *)&cfg_name, (const char *)&mstp_global_bridge->mst_cfg_id.cfg_name, old_region_name_len) == 0)
		{
			vty_info_out(vty," region name: %s%s", mstp_global_bridge->mst_cfg_id.cfg_name, VTY_NEWLINE);
		
			return CMD_SUCCESS; 
		}
	}



	memset(mstp_global_bridge->mst_cfg_id.cfg_name, 0, NAME_LEN);
	memcpy(mstp_global_bridge->mst_cfg_id.cfg_name, argv[0], region_name_len);


	vty_info_out(vty," region name: %s%s", mstp_global_bridge->mst_cfg_id.cfg_name, VTY_NEWLINE);

	vty_info_out(vty," This operation may take a few seconds. Please wait for a moment...");

	/*restart mstp*/
	mstp_reinit();

	vty_out(vty,"done. %s", VTY_NEWLINE);

    return CMD_SUCCESS;	
}

/*region name*/
DEFUN (no_stp_region_name,
        no_stp_region_name_cmd,
        "no stp region-name",
        NO_STR
        MSTP_STR
        "Specify region name\n")
{
	unsigned int 			i = 0;

	unsigned char			cfg_name[NAME_LEN] = {0};

	memset(cfg_name, 0x00, NAME_LEN);
	for(i = 0; i < MAC_LEN; i++)
	{
		sprintf((char *)&cfg_name[2*i],
						"%02X",
						mstp_global_bridge->bridge_mac.mac_addr[i]);
	}

	if(0 == strncmp((const char *)&mstp_global_bridge->mst_cfg_id.cfg_name, (const char *)&cfg_name, NAME_LEN))
	{
		vty_info_out(vty," region name to default: %s %s",
						mstp_global_bridge->mst_cfg_id.cfg_name,
						VTY_NEWLINE);	
		return CMD_SUCCESS; 
	}


	memset(mstp_global_bridge->mst_cfg_id.cfg_name, 0x00, NAME_LEN);

	for(i = 0; i < MAC_LEN; i++)
	{
		sprintf((char *)&mstp_global_bridge->mst_cfg_id.cfg_name[2*i],
						"%02X",
						mstp_global_bridge->bridge_mac.mac_addr[i]);
	}

	vty_info_out(vty," region name to default: %s %s",
					mstp_global_bridge->mst_cfg_id.cfg_name,
					VTY_NEWLINE);

	vty_info_out(vty," This operation may take a few seconds. Please wait for a moment...");

	mstp_reinit();

	vty_out(vty,"done. %s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

/*revision level*/
DEFUN (stp_revision_level,
        stp_revision_level_cmd,
        "stp revision-level <0-65535>",
        MSTP_STR
        "Specify revision level\n"
        "revision level, Please input an integer from 0 to 65535, default is 0\n")
{
	unsigned int			revision_level = 0;
	unsigned int			old_revision_level = mstp_global_bridge->mst_cfg_id.revison_level;

	revision_level = atoi(argv[0]);

	if(old_revision_level == revision_level)
	{
		vty_info_out(vty, " revison level[%d] %s", mstp_global_bridge->mst_cfg_id.revison_level, VTY_NEWLINE);

		return CMD_SUCCESS;
	}

	if(revision_level > MAX_REVISION_LEVEL)
	{
		MSTP_LOG_DBG("%s[%d]:%s:ERROR: revision level parameter!\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," revision level parameter, please input an integer from 0 to 65535! %s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	mstp_global_bridge->mst_cfg_id.revison_level = revision_level;

	vty_info_out(vty, " revison level[%d] %s", mstp_global_bridge->mst_cfg_id.revison_level, VTY_NEWLINE);

	vty_info_out(vty," This operation may take a few seconds. Please wait for a moment...");

	mstp_reinit();

	vty_out(vty,"done. %s", VTY_NEWLINE);


    return CMD_SUCCESS;
}

DEFUN (no_stp_revision_level,
        no_stp_revision_level_cmd,
        "no stp revision-level",
        NO_STR
        MSTP_STR
        "Specify revision level\n")
{
	if(0 == mstp_global_bridge->mst_cfg_id.revison_level)
	{
		vty_info_out(vty, " revison level[%d] %s", mstp_global_bridge->mst_cfg_id.revison_level, VTY_NEWLINE);

		return CMD_SUCCESS;
	}

	mstp_global_bridge->mst_cfg_id.revison_level = 0;

	vty_info_out(vty, " revison level[%d] %s", mstp_global_bridge->mst_cfg_id.revison_level, VTY_NEWLINE);

	vty_info_out(vty," This operation may take a few seconds. Please wait for a moment...");

	mstp_reinit();		//FIXME

	vty_out(vty,"done. %s", VTY_NEWLINE);


    return CMD_SUCCESS;	
}

/*stp mode cmd*/
DEFUN(stp_mode, 
		stp_mode_cmd,
        "stp mode (stp|rstp|mstp)",
        MSTP_STR
        "Specify state machine mode\n"
        "Spanning Tree Protocol (STP) mode\n"
        "Rapid Spanning Tree Protocol (RSTP) mode\n"
        "Multiple Spanning Tree Protocol (MSTP) mode\n")
{
	unsigned char			mstp_mode = 0;
	/*all instance, all interface msti delete*/
	int 					cursor = 0;

	struct l2if 			*pif		= NULL;
	struct mstp_port		*mstp_port	= NULL;
	struct hash_bucket		*pbucket	= NULL;

	/*config stp mode*/
	if(!strncmp(argv[0], "s", 1))
	{
		/*config stp mode*/
		mstp_mode = PROTO_VERID_STP;
	}
	else if(!strncmp(argv[0], "r", 1))
	{
		/*config rstp mode*/
		mstp_mode = PROTO_VERID_RSTP;
	}
	else if(!strncmp(argv[0], "m", 1))
	{
		/*config mstp mode*/
		mstp_mode = PROTO_VERID_MSTP;
	}
	else
	{
		/*reminder error info*/
		vty_error_out(vty, " wrong parameter! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*mstp mode no change*/
	if(mstp_mode == mstp_global_bridge->stp_mode)
	{	
		switch(mstp_global_bridge->stp_mode)
		{
			case PROTO_VERID_STP:
			{				
				vty_info_out(vty, " mstp mode (stp). %s", VTY_NEWLINE);
			}
				break;
			case PROTO_VERID_RSTP:
			{
				vty_info_out(vty, " mstp mode (rstp). %s", VTY_NEWLINE);
			}
				break;
			case PROTO_VERID_MSTP:
			{
				vty_info_out(vty, " mstp mode (mstp). %s", VTY_NEWLINE);
			}
				break;				
			default:
				break;
		}
		
		return CMD_SUCCESS;
	}

	/*stp/rstp mode --->mstp mode*/
	if((mstp_global_bridge->stp_mode != PROTO_VERID_MSTP)
		&& (PROTO_VERID_MSTP == mstp_mode))
	{
		/*add msti port*/
		mstp_mode_modify_add_msti_port();
	}
	/*mstp mode --->stp/rstp mode*/
	else if((PROTO_VERID_MSTP == mstp_global_bridge->stp_mode)
		&& (mstp_mode != PROTO_VERID_MSTP))	
	{
		/*delete msti port*/
		mstp_mode_modify_delete_msti_port();		
	}


	mstp_global_bridge->common_br.force_pt_version = mstp_mode;
	mstp_global_bridge->stp_mode = mstp_mode;

	/*all mstp port mode is mstp_mode */
	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}

		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if((IFNET_MODE_INVALID != pif->mode )
			&& (IFNET_MODE_L3 != pif->mode ))
		{
			/*mstp port have config [mstp enable]*/
			if((mstp_port != NULL) && (MSTP_TRUE == mstp_port->adminStatus))
			{
				mstp_port->stp_mode = mstp_mode;

MSTP_LOG_DBG("%s: mstp_add_stg port(%s), instance_id = 0, state(%s)!\n",
		__func__, mstp_port->if_name, "MSTP_STATE_BLOCKING");

				//STG_MODIFY
				/*add instance_id(0), vlan_bitmap, port, state(DISCARDING) */
				mstp_add_stg_vlan(0, mstp_port, MSTP_STATE_BLOCKING);

				if(PORT_LINK_UP == mstp_port->port_variables.portEnabled)
				{
					/*send packet*/
					if(PROTO_VERID_STP == mstp_port->stp_mode)
					{
						mstp_tx_config(mstp_port);
					}
					else
					{
						mstp_tx_mstp(mstp_port);
					}
				}
			}
		}
	}
	
	switch(mstp_global_bridge->stp_mode)
	{
		case PROTO_VERID_STP:
		{				
			vty_info_out(vty, " mstp mode (stp). %s", VTY_NEWLINE);
		}
			break;
		case PROTO_VERID_RSTP:
		{
			vty_info_out(vty, " mstp mode (rstp). %s", VTY_NEWLINE);
		}
			break;
		case PROTO_VERID_MSTP:
		{
			vty_info_out(vty, " mstp mode (mstp). %s", VTY_NEWLINE);
		}
			break;				
		default:
			break;
	}

	vty_info_out(vty," This operation may take a few seconds. Please wait for a moment...");

	mstp_reinit();
	
	vty_out(vty,"done. %s", VTY_NEWLINE);


	return CMD_SUCCESS;
}

/*stp mode cmd*/
DEFUN(no_stp_mode,
		no_stp_mode_cmd,
        "no stp mode",
        NO_STR
        MSTP_STR
        "Specify state machine mode\n")
{
	/*all instance, all interface msti delete*/
	int 					cursor = 0;

	struct l2if 			*pif		= NULL;
	struct mstp_port		*mstp_port	= NULL;
	struct hash_bucket		*pbucket	= NULL;

	/*store mode*/
	unsigned char mstp_mode = mstp_global_bridge->stp_mode;

	/*set default mode (mstp mode)*/
	mstp_global_bridge->stp_mode = PROTO_VERID_MSTP;


	/*need to think care*/
	/*if mode no change*/
	/*mstp mode --->mstp mode*/
	if(mstp_mode == mstp_global_bridge->stp_mode)
	{
		vty_info_out(vty, " mstp mode to default(mstp). %s", VTY_NEWLINE);
		/*mode no change*/
		return CMD_SUCCESS;
	}
	/*stp/rstp mode --->mstp mode*/
	else if(mstp_mode != mstp_global_bridge->stp_mode)
	{
		/*mode  change*/
		/*add msti port*/
		mstp_mode_modify_add_msti_port();
	}

	/*need to open*/
	mstp_global_bridge->common_br.force_pt_version = PROTO_VERID_MSTP;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}

		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if((IFNET_MODE_INVALID != pif->mode)
			&& (IFNET_MODE_L3 != pif->mode ))
		{
			/*mstp port have config [mstp enable]*/
			if((mstp_port != NULL) && (MSTP_TRUE == mstp_port->adminStatus))
			{
				/*MODE_CHANGE*/
				mstp_port->stp_mode = PROTO_VERID_MSTP;

MSTP_LOG_DBG("%s: mstp_add_stg port(%s), instance_id = 0, state(%s)!\n",
			__func__, mstp_port->if_name, "MSTP_STATE_BLOCKING");

				//STG_MODIFY
				/*add instance_id(0), vlan_bitmap, port, state(DISCARDING) */
				mstp_add_stg_vlan(0, mstp_port, MSTP_STATE_BLOCKING);		

				/*send packet*/
				if(PROTO_VERID_STP == mstp_port->stp_mode)
				{
					mstp_tx_config(mstp_port);
				}
				else
				{
					mstp_tx_mstp(mstp_port);
				}
			}
		}
	}
	
	vty_info_out(vty, " mstp mode to default(mstp). %s", VTY_NEWLINE);

	vty_info_out(vty," This operation may take a few seconds. Please wait for a moment...");

	mstp_reinit();

	vty_out(vty,"done. %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

/*stp priority cmd*/
DEFUN(stp_priority,
		stp_priority_cmd,
        "stp priority <0-61440>",
        MSTP_STR
        "Specify bridge priority\n"
        "Bridge priority, in steps of 4096, the default value is 32768. Please input an integer from 0 to 61440\n")
{
	/*change bridge priority value*/
	unsigned short			priority = 0;
	unsigned short			old_priority = mstp_global_bridge->br_priority;
	
	struct mstp_cist_br		*cist_br = NULL;

	cist_br = &mstp_global_bridge->cist_br;
	if(NULL == cist_br)
	{
		vty_error_out(vty," CIST bridge is NULL, can not config! %s", VTY_NEWLINE);
		
		MSTP_LOG_DBG("%s[%d]:%s:ERROR: CIST bridge is NULL, can not config!\n", __FILE__, __LINE__, __func__);
		return CMD_WARNING;
	}

	/*check bridge root config, if config the bridge root role, can not change the bridge priority*/	
	/*0:default, 1:master, 2:slave*/
	if(mstp_global_bridge->bridge_root_role)
	{
		/**/
		vty_error_out(vty," This bridge have config master/slave, can not change priority! %s", VTY_NEWLINE);

		MSTP_LOG_DBG("%s[%d]:%s:ERROR: this bridge have config master/slave, can not change priority!\n", __FILE__, __LINE__, __func__);
		return CMD_WARNING;
	}

	priority = atoi(argv[0]);
	if(priority > 61440)
	{
		vty_error_out(vty," Wrong parameter range, vlan id is <0-61440>. %s", VTY_NEWLINE);
			
		return CMD_WARNING;
	}
	
    if(0 != priority % 4096)
	{	
		vty_out(vty,"Error: Bridge Priority must be in step of 4096, Allowed values are: %s", VTY_NEWLINE);

		vty_out(vty,"0     4096  8192  12288 16384 20480 24576 28672 %s", VTY_NEWLINE);
		vty_out(vty,"32768 36864 40960 45056 49152 53248 57344 61440 %s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	if(old_priority == priority)
	{
		vty_info_out(vty," Bridge priority[%d]. %s", mstp_global_bridge->br_priority, VTY_NEWLINE);	
		return CMD_SUCCESS;
	}


	mstp_global_bridge->br_priority = priority;

	/*update bridge priority*/
	cist_br = &mstp_global_bridge->cist_br;
	if(cist_br != NULL)
	{
		cist_br->bridge_mac 					   		= mstp_get_br_id(mstp_global_bridge->br_priority, 0);
		cist_br->bridge_priority.root_id		   		= cist_br->bridge_mac;
		cist_br->bridge_priority.regional_root_id		= cist_br->bridge_mac;
		cist_br->bridge_priority.designated_bridge_id	= cist_br->bridge_mac;
	}

	vty_info_out(vty," Bridge priority[%d]. %s", mstp_global_bridge->br_priority, VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}

/*no stp priority cmd*/
DEFUN(no_stp_priority,
		no_stp_priority_cmd,
        "no stp priority",
        NO_STR
        MSTP_STR
        "Specify bridge priority\n")
{
	struct mstp_cist_br		*cist_br = NULL;

	/*0:default, 1:master, 2:slave*/
	if(mstp_global_bridge->bridge_root_role)
	{
		/**/
		vty_error_out(vty," This bridge have config master/slave , can not change priority! %s", VTY_NEWLINE);
		MSTP_LOG_DBG("%s[%d]:%s:ERROR: this bridge have config master/slave, can not change priority!\n", __FILE__, __LINE__, __func__);

		return CMD_WARNING;
	}

	if(DEF_BRIDGE_PRIORITY == mstp_global_bridge->br_priority)
	{
		vty_info_out(vty," Bridge priority[%d].%s", mstp_global_bridge->br_priority, VTY_NEWLINE);
	
		return CMD_SUCCESS;
	}

	/*change bridge priority to default value*/
	mstp_global_bridge->br_priority = DEF_BRIDGE_PRIORITY;
	
	/*update bridge priority*/
	cist_br = &mstp_global_bridge->cist_br;
	if(cist_br != NULL)
	{
		cist_br->bridge_mac 					   		= mstp_get_br_id(mstp_global_bridge->br_priority, 0);
		cist_br->bridge_priority.root_id		   		= cist_br->bridge_mac;
		cist_br->bridge_priority.regional_root_id		= cist_br->bridge_mac;
		cist_br->bridge_priority.designated_bridge_id	= cist_br->bridge_mac;
	}

	vty_info_out(vty," Bridge priority[%d].%s", mstp_global_bridge->br_priority, VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}

/*stp root cmd*/
DEFUN(stp_root,
		stp_root_cmd,
        "stp root (master | slave)",
        MSTP_STR
        "Specify root switch\n"
        "Primary root bridge\n"
        "Secondary root bridge\n")
{
	struct mstp_cist_br		*cist_br = NULL;
	enum MSTP_ROOT_ROLE		old_bridge_root_role = mstp_global_bridge->bridge_root_role;

	if(!strncmp(argv[0], "master", 1))
	{
		mstp_global_bridge->bridge_root_role = MSTP_ROOT_MASTER;

		/*master: config mstp bridge priority is 0*/
		mstp_global_bridge->br_priority = 0;

		vty_info_out(vty," Bridge root[%s]. %s", "master", VTY_NEWLINE);		
	}
	else if(!strncmp(argv[0], "slave", 1))
	{		
		mstp_global_bridge->bridge_root_role = MSTP_ROOT_SLAVE;

		/*slave:	 config (mstp instance) bridge priority is 4096*/
		mstp_global_bridge->br_priority = 4096;

		vty_info_out(vty," Bridge root[%s]. %s", "slave", VTY_NEWLINE);
	}
	else
	{
		vty_error_out(vty, " stp error root parameter.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	if(old_bridge_root_role == mstp_global_bridge->bridge_root_role)
	{
		return CMD_SUCCESS;		
	}

	/*update bridge priority*/
	cist_br = &mstp_global_bridge->cist_br;
	if(cist_br != NULL)
	{
		cist_br->bridge_mac 					   		= mstp_get_br_id(mstp_global_bridge->br_priority, 0);
		cist_br->bridge_priority.root_id		   		= cist_br->bridge_mac;
		cist_br->bridge_priority.regional_root_id		= cist_br->bridge_mac;
		cist_br->bridge_priority.designated_bridge_id	= cist_br->bridge_mac;
	}

	mstp_reinit();

	return CMD_SUCCESS;
}

/*no stp root cmd*/
DEFUN(no_stp_root,
		no_stp_root_cmd,
        "no stp root",
        NO_STR
        MSTP_STR
        "Specify root switch\n")
{
	struct mstp_cist_br		*cist_br = NULL;

	/*step1: delete stp root config,
	**step2: use (stp priority <0-61440>) cmd change bridge priority*/
	if(MSTP_ROOT_DEF == mstp_global_bridge->bridge_root_role)
	{
		vty_info_out(vty, " stp root config have delete,  bridge priority[%d] .%s",
			mstp_global_bridge->br_priority,
			VTY_NEWLINE);	
		return CMD_SUCCESS;
	}

	/*instance root role to default*/
	mstp_global_bridge->bridge_root_role = MSTP_ROOT_DEF;

	/*instance bridge priority to default*/
	mstp_global_bridge->br_priority = DEF_BRIDGE_PRIORITY;

	vty_info_out(vty, " stp root config have delete,  bridge priority[%d] .%s",
		mstp_global_bridge->br_priority,
		VTY_NEWLINE);

	
	/*update bridge priority*/
	cist_br = &mstp_global_bridge->cist_br;
	if(cist_br != NULL)
	{
		cist_br->bridge_mac 					   		= mstp_get_br_id(mstp_global_bridge->br_priority, 0);
		cist_br->bridge_priority.root_id		   		= cist_br->bridge_mac;
		cist_br->bridge_priority.regional_root_id		= cist_br->bridge_mac;
		cist_br->bridge_priority.designated_bridge_id	= cist_br->bridge_mac;
	}

	mstp_reinit();

	return CMD_SUCCESS;
}

/*stp hello interval cmd*/
DEFUN(stp_hello_time,
		stp_hello_time_cmd,
		"stp hello-time <1-10>",
		MSTP_STR
		"Specify hello time interval\n"
		"The default value is 2. Please input an integer from 1 to 10\n")
{
	unsigned int				hello_timer = 0;	
	int							cursor = 0;

	struct l2if 				*pif = NULL;
	struct hash_bucket			*pbucket = NULL;
	struct mstp_port			*mstp_port = NULL;



	hello_timer = atoi(argv[0]);
	if((hello_timer < MIN_HELLO_TIMER) || (hello_timer > MAX_HELLO_TIMER))
	{
		vty_error_out(vty," Wrong parameter range, hello timer range is <1-10>. %s", VTY_NEWLINE);
		MSTP_LOG_DBG("%s[%d]:%s:Error: Wrong parameter range, hello timer range is <1-10>.\n", __FILE__, __LINE__, __func__);

		return CMD_SUCCESS;
	}

	/*config vlaue no change*/
	if(mstp_global_bridge->hello_time == hello_timer)
	{
		vty_info_out(vty," Bridge hello time[%d]s. %s", mstp_global_bridge->hello_time, VTY_NEWLINE);	
		return CMD_SUCCESS;
	}


	/*instance get*/
	mstp_global_bridge->hello_time = hello_timer;
	mstp_global_bridge->cist_br.bridge_times.hello_time = hello_timer;


	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}

		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if(IFNET_MODE_INVALID == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}
		else if(IFNET_MODE_L3 == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}

		
		if(mstp_port != NULL)
		{	
			/*change mstp_port (this port is cist port value)*/
			mstp_port->cist_port.port_times.hello_time = mstp_global_bridge->hello_time;
		}
	}


	vty_info_out(vty," Bridge hello time[%d]s. %s", mstp_global_bridge->hello_time, VTY_NEWLINE); 	

	mstp_reinit();

	return CMD_SUCCESS;
}

/*no stp hello interval cmd*/
DEFUN(no_stp_hello_time,
		no_stp_hello_time_cmd,
        "no stp hello-time",
        NO_STR
        MSTP_STR
        "Specify hello time interval\n")
{
	/*change hello time is default 2s.*/
	int							cursor = 0;

	struct mstp_port			*mstp_port = NULL;
	struct l2if 				*pif = NULL;
	struct hash_bucket			*pbucket = NULL;

	/*config vlaue no change*/
	if(MSTP_DEFAULT_HELLO_TIME == mstp_global_bridge->hello_time)
	{
		vty_info_out(vty," Bridge hello time to default[%d]s. %s", mstp_global_bridge->hello_time, VTY_NEWLINE);	
		return CMD_SUCCESS;
	}

	/*instance get*/
	mstp_global_bridge->hello_time = MSTP_DEFAULT_HELLO_TIME;	
	mstp_global_bridge->cist_br.bridge_times.hello_time = MSTP_DEFAULT_HELLO_TIME;
	
	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}

		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if(IFNET_MODE_INVALID == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}
		else if(IFNET_MODE_L3 == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}

		if(mstp_port != NULL)
		{	
			/*change mstp_port (this port is cist port value)*/
			mstp_port->cist_port.port_times.hello_time = mstp_global_bridge->hello_time;
		}
	}

	vty_info_out(vty," Bridge hello time to default[%d]s. %s", mstp_global_bridge->hello_time, VTY_NEWLINE); 	

	mstp_reinit();

	return CMD_SUCCESS;
}

/*stp forward delay cmd*/
DEFUN(stp_forward_delay,
		stp_forward_delay_cmd,
        "stp forward-delay <4-30>",
        MSTP_STR
        "Specify forward delay\n"
        "The default value is 15s. Please input an integer from 4 to 30\n")
{
	/*config forward delay*/
	unsigned int				forward_delay = 0;
	int							cursor = 0;

	struct mstp_port			*mstp_port = NULL;
	struct l2if 				*pif = NULL;
	struct hash_bucket			*pbucket = NULL;


	forward_delay = atoi(argv[0]);
	if((forward_delay < MIN_FORWARD_DELAY) || (forward_delay > MAX_FORWARD_DELAY))
	{
		vty_error_out(vty," Wrong parameter range, forward delay range is <4-30>. %s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	/*config vlaue no change*/
	if(mstp_global_bridge->fwd_delay == forward_delay)
	{
		vty_info_out(vty," Bridge forward delay[%d]s. %s", mstp_global_bridge->fwd_delay , VTY_NEWLINE);	
		return CMD_SUCCESS;
	}

	/*instance get*/
	mstp_global_bridge->fwd_delay = forward_delay;
	mstp_global_bridge->cist_br.bridge_times.forward_delay = forward_delay;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}

		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if(IFNET_MODE_INVALID == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}
		else if(IFNET_MODE_L3 == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}

		if(mstp_port != NULL)
		{	
			/*change mstp_port (this port is cist port value)*/
			mstp_port->cist_port.port_times.forward_delay = mstp_global_bridge->fwd_delay;
		}
	}
	vty_info_out(vty," Bridge forward delay[%d]s. %s", mstp_global_bridge->fwd_delay , VTY_NEWLINE); 	

	mstp_reinit();

	return CMD_SUCCESS;
}

/*no stp forward delay cmd*/
DEFUN(no_stp_forward_delay,
		no_stp_forward_delay_cmd,
        "no stp forward-delay",
        NO_STR
        MSTP_STR
        "Specify forward delay\n")
{
	/*config forward delay time to default(15s)*/
	int							cursor = 0;

	struct mstp_port			*mstp_port = NULL;	
	struct l2if 				*pif = NULL;
	struct hash_bucket			*pbucket = NULL;


	/*config vlaue no change*/
	if(MSTP_DEFAULT_FWD_DELAY == mstp_global_bridge->fwd_delay)
	{
		vty_info_out(vty," Mstp_global_bridge forward delay to default[%d]s. %s", mstp_global_bridge->fwd_delay , VTY_NEWLINE); 	
		return CMD_SUCCESS;
	}

	mstp_global_bridge->fwd_delay = MSTP_DEFAULT_FWD_DELAY;
	mstp_global_bridge->cist_br.bridge_times.forward_delay = MSTP_DEFAULT_FWD_DELAY;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}

		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if(IFNET_MODE_INVALID == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}
		else if(IFNET_MODE_L3 == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}

		if(mstp_port != NULL)
		{	
			/*change mstp_port (this port is cist port value)*/
			/*need to modify*/
			mstp_port->cist_port.port_times.forward_delay = mstp_global_bridge->fwd_delay;
			mstp_port->cist_port.designated_times.forward_delay = mstp_global_bridge->fwd_delay;
		}
	}

	vty_info_out(vty," Mstp_global_bridge forward delay to default[%d]s. %s", mstp_global_bridge->fwd_delay , VTY_NEWLINE); 	

	mstp_reinit();

	return CMD_SUCCESS;
}

/*stp max-age cmd*/
DEFUN(stp_max_age,
		stp_max_age_cmd,
        "stp max-age <6-40>",
        MSTP_STR
        "Specify max age\n"
        "The default value is 20s. Please input an integer from 6 to 40\n")
{
	/*config max-age time*/
	int							cursor = 0;
	unsigned int				max_age = 0;

	struct mstp_port			*mstp_port = NULL;
	struct l2if 				*pif = NULL;
	struct hash_bucket			*pbucket = NULL;

	max_age = atoi(argv[0]);
	if((max_age < MIN_MAXAGE) || (max_age > MAX_MAXAGE))
	{
		vty_error_out(vty," Wrong parameter range, max age range is <6-40>. %s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	/*config vlaue no change*/
	if(mstp_global_bridge->msg_max_age == max_age)
	{
		vty_info_out(vty," Bridge max age[%d]s. %s", mstp_global_bridge->msg_max_age, VTY_NEWLINE); 	
		return CMD_SUCCESS;
	}

	mstp_global_bridge->msg_max_age = max_age;
	mstp_global_bridge->cist_br.bridge_times.max_age = max_age;
	mstp_global_bridge->common_br.bridge_max_age = max_age;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}

		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if(IFNET_MODE_INVALID == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}
		else if(IFNET_MODE_L3 == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}

		if(mstp_port != NULL)
		{	
			/*need to modify*/
			mstp_port->cist_port.port_times.max_age = mstp_global_bridge->msg_max_age;
			mstp_port->cist_port.designated_times.max_age = mstp_global_bridge->msg_max_age;

		}
	}

	vty_info_out(vty," Bridge max age[%d]s. %s", mstp_global_bridge->msg_max_age, VTY_NEWLINE); 	

	mstp_reinit();

	return CMD_SUCCESS;
}

/*no stp max-age cmd*/
DEFUN(no_stp_max_age,
		no_stp_max_age_cmd,
        "no stp max-age",
        NO_STR
        MSTP_STR
        "Specify max age\n")
{
	/*config max-age time to default 20s*/
	int							cursor = 0;

	struct mstp_port			*mstp_port = NULL;
	struct l2if 				*pif = NULL;
	struct hash_bucket			*pbucket = NULL;


	/*config vlaue no change*/
	if(MSTP_DEFAULT_MAX_AGE == mstp_global_bridge->msg_max_age)
	{
		vty_info_out(vty," Bridge max age to default[%d]s. %s", mstp_global_bridge->msg_max_age , VTY_NEWLINE); 	
		return CMD_SUCCESS;
	}

	mstp_global_bridge->msg_max_age = MSTP_DEFAULT_MAX_AGE;
	mstp_global_bridge->cist_br.bridge_times.max_age = MSTP_DEFAULT_MAX_AGE;
	mstp_global_bridge->common_br.bridge_max_age = MSTP_DEFAULT_MAX_AGE;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}
				
		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if(IFNET_MODE_INVALID == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}
		else if(IFNET_MODE_L3 == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}

		if(mstp_port != NULL)
		{	
			/*need to modify*/
			mstp_port->cist_port.port_times.max_age = mstp_global_bridge->msg_max_age;
			mstp_port->cist_port.designated_times.max_age = mstp_global_bridge->msg_max_age;
		}
	}

	vty_info_out(vty," Bridge max age to default[%d]s. %s", mstp_global_bridge->msg_max_age , VTY_NEWLINE); 	

	mstp_reinit();
	
	return CMD_SUCCESS;
}

/*stp max-hop cmd*/
DEFUN(stp_max_hop,
		stp_max_hop_cmd,
        "stp max-hop <1-40>",
        MSTP_STR
        "Specify max hops\n"
        "The default value is 20. Please input an integer from 1 to 40\n")
{
	/*config max-hops*/
	unsigned int				max_hop 	= 0;
	int							cursor 		= 0;
	unsigned int				instance_id = 0;
	
	struct l2if 				*pif 		= NULL;
	struct hash_bucket			*pbucket 	= NULL;
	struct mstp_port			*mstp_port 	= NULL;
    struct mstp_msti_br    		*msti_br 	= NULL;


	max_hop = atoi(argv[0]);
	if((max_hop < MIN_MAXHOP) || (max_hop > MAX_MAXHOP))
	{
		vty_error_out(vty," Wrong parameter range, max hop range is <1-40>. %s", VTY_NEWLINE);

		return CMD_SUCCESS;
	}
	
	/*config vlaue no change*/
	if(mstp_global_bridge->msg_max_hop_count == max_hop)
	{
		vty_info_out(vty," Bridge max hop[%d]. %s", mstp_global_bridge->msg_max_hop_count, VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	mstp_global_bridge->msg_max_hop_count = max_hop;
	mstp_global_bridge->cist_br.bridge_times.remaining_hops = max_hop;

	/*update cist*/
	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}

		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if(IFNET_MODE_INVALID == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}
		else if(IFNET_MODE_L3 == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}

		if(mstp_port != NULL)
		{	
			mstp_port->cist_port.port_times.remaining_hops = mstp_global_bridge->msg_max_hop_count;
		}
	}

	/*update msti*/
	for(instance_id = 0; instance_id < MSTP_INSTANCE_MAX; instance_id++)
	{
		msti_br = mstp_global_bridge->msti_br[instance_id];
		if (msti_br != NULL)
		{
			msti_br->bridge_times.remaining_hops = max_hop;
		}
	}

	vty_info_out(vty," Bridge max hop[%d]. %s", mstp_global_bridge->msg_max_hop_count, VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}

/*no stp max-hop cmd*/
DEFUN(no_stp_max_hop,
		no_stp_max_hop_cmd,
        "no stp max-hop",
        NO_STR
        MSTP_STR
        "Specify max hops\n")
{
	/*config max-hops to default (20)*/
	int							cursor 		= 0;
	unsigned int				instance_id = 0;

	struct l2if 				*pif 		= NULL;
	struct hash_bucket			*pbucket 	= NULL;
	struct mstp_port			*mstp_port 	= NULL;
    struct mstp_msti_br    		*msti_br 	= NULL;

	/*config vlaue no change*/
	if(MSTP_DEFAULT_MAX_HOP == mstp_global_bridge->msg_max_hop_count)
	{
		vty_info_out(vty," Bridge max hop to default[%d]. %s", mstp_global_bridge->msg_max_hop_count , VTY_NEWLINE);	
		return CMD_SUCCESS;
	}

	mstp_global_bridge->msg_max_hop_count = MSTP_DEFAULT_MAX_HOP;
	mstp_global_bridge->cist_br.bridge_times.remaining_hops = MSTP_DEFAULT_MAX_HOP;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}

		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if(IFNET_MODE_INVALID == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}
		else if(IFNET_MODE_L3 == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			continue;
		}

		if(mstp_port != NULL)
		{	
			mstp_port->cist_port.port_times.remaining_hops = MSTP_DEFAULT_MAX_HOP;
		}
	}

	/*update msti*/
	for(instance_id = 0; instance_id < MSTP_INSTANCE_MAX; instance_id++)
	{
		msti_br = mstp_global_bridge->msti_br[instance_id];
		if (msti_br != NULL)
		{
			msti_br->bridge_times.remaining_hops = MSTP_DEFAULT_MAX_HOP;
		}
	}

	vty_info_out(vty," Bridge max hop to default[%d]. %s", mstp_global_bridge->msg_max_hop_count , VTY_NEWLINE); 	

	mstp_reinit();

	return CMD_SUCCESS;
}




/*************************enter instance view & config **********************/
/*create instance, enter stp instance view*/
DEFUN (stp_instance,
        stp_instance_cmd,
        "stp instance <1-8>",
        MSTP_STR
        "Spanning tree instance\n"
        "Identifier of spanning tree instance, Please input an integer from 1 to 8\n")
{
	struct mstp_instance		*instance 	= NULL;
	unsigned int				instance_id = 0;

	instance_id = atoi(argv[0]);
	if((instance_id < MSTP_INSTANCE_MIN) || (instance_id > MSTP_INSTANCE_MAX))
	{
		vty_error_out(vty," Wrong parameter range, instance id is <1-8>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*check stp instance exist status*/
	instance = &mstp_global_bridge->mstp_instance[instance_id-1];
	if(NULL == instance)
	{
		vty_error_out(vty," Instance == NULL, error!!! %s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	vty->node =(int) MSTP_NODE;
	vty->index = (void*)instance_id;

    return CMD_SUCCESS;
}

/*delete stp instance cmd*/
DEFUN (no_stp_instance,
        no_stp_instance_cmd,
        "no stp instance <1-8>",
		NO_STR
		MSTP_STR
        "Spanning tree instance\n"
        "Identifier of spanning tree instance, Please input an integer from 1 to 8\n")
{
	int 					cursor 		= 0;

	unsigned int			instance_id = 0;
	struct l2if 			*pif 		= NULL;
	struct mstp_port		*mstp_port 	= NULL;
	struct hash_bucket		*pbucket 	= NULL;

	/*we do not deal instance 0*/
	instance_id = atoi(argv[0]);
	if((instance_id < MSTP_INSTANCE_MIN) || (instance_id > MSTP_INSTANCE_MAX))
	{
		vty_error_out(vty," Wrong parameter range, instance id is <1-8>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*clean instance all config*/
	mstp_clean_instance(vty, instance_id);

	/*need to think about trunk port*/

	/*instance delete , we think vlan change
	** instance 0 ---cist need to reset port forwarding/discarding state
	*/

	
	/*check all port about this instance*/
	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(pif == NULL)
		{
			continue;
		}
		
		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if((IFNET_MODE_INVALID != pif->mode)
			&& (IFNET_MODE_L3 != pif->mode))
		{
			/*mstp port have config [mstp enable]*/
			if((mstp_port != NULL) && (mstp_port->adminStatus == MSTP_TRUE))
			{
				mstp_port->cist_port.common_info.sm_state_trans_state = 0;

MSTP_LOG_DBG("%s: mstp_add_stg port(%s), instance_id = 0, state(%s)!\n",
			__func__, mstp_port->if_name, "MSTP_STATE_BLOCKING");

				//STG_MODIFY
				/*add instance_id(0), vlan_bitmap, port, state(FORWARDING) */ 	
				mstp_add_stg_vlan(0, mstp_port, MSTP_STATE_BLOCKING);	
				
			}
		}
	}
	
	vty_info_out(vty," Instance[%d] config have delete. %s", instance_id, VTY_NEWLINE);

	/*MST Region:vlan and instance mapping*/
	mstp_gen_cfg_digest(mstp_global_bridge);
	mstp_reinit();
	
    return CMD_SUCCESS;
}


/*stp vlan cmd:
*1.	each vlan must be belong to one msti,one msti can contain some vlan
*2.	vlan config in mstp instance view
*/
DEFUN(stp_vlan,
		stp_vlan_cmd,
        "stp vlan <1-4094> {to <1-4094>}",
        MSTP_STR
        "vlan\n"
        "vlan range, Please input an integer from 1 to 4094\n"
        "range of vlan\n"
        "vlan range, Please input an integer from 1 to 4094\n")
{
	struct mstp_instance		*instance = NULL;
	unsigned int				instance_id = 0;
	unsigned int				vlan_start = 0, vlan_end = 0, vlan_tmp = 0;
    unsigned char				vlan_map[MSTP_VLAN_MAP_SIZE];
	unsigned int				vlan_id = 0;
	unsigned int				i = 0;
	int 						cursor = 0;

	struct l2if 				*pif = NULL;
	struct mstp_port			*mstp_port = NULL;
	struct hash_bucket			*pbucket = NULL;

	/*get instance id*/
	instance_id = (unsigned int)vty->index;

	/*get vlan id/range*/
	vlan_start = atoi(argv[0]);
	if(argv[1] != NULL)
	{
		vlan_end = atoi(argv[1]);
	}
	else
	{
		vlan_end = vlan_start;
	}

	/*vlan id range check*/
	if((vlan_start < MSTP_VLAN_MIN_ID) || (vlan_start > MSTP_VLAN_MAX_ID)
	  || (vlan_end < MSTP_VLAN_MIN_ID) || (vlan_end > MSTP_VLAN_MAX_ID))
	{	
		vty_error_out(vty," Wrong parameter range, vlan id is <1-4094>. %s", VTY_NEWLINE);			
		return CMD_WARNING;
	}

	/*vlan_start must <= vlan_end*/
	if(vlan_end <= vlan_start)
	{
		vlan_tmp = vlan_start;
		vlan_start = vlan_end;
		vlan_end = vlan_tmp;
	}

	/*vlan_start must not lower than vlan_end*/
	if(vlan_end < vlan_start)
	{
		vty_error_out (vty, " The VLAN list is invalid.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get vlan id map*/
	memset(vlan_map, 0, MSTP_VLAN_MAP_SIZE);
	for(vlan_id = vlan_start; vlan_id <= vlan_end; vlan_id++)
	{
		mstp_vlan_map_set(vlan_map, vlan_id);
	}

	/*To determine whether a vlan belongs to other instances*/
	for(i = 0; i < MSTP_INSTANCE_MAX; i++)
	{
		if(MSTP_TRUE == mstp_cmp_vlan_map(vlan_map, global_ins_vlan_map[i].vlan_map))
		{
			vty_error_out(vty," MST instacne[%d] vlan config failed, have the same vlan have configed in the instances[%d]. %s", instance_id, i+1, VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	/*copy vlan to instance vlan_map*/
	/*example: 	instance 1: vlan 1-10, vlan 30-40
	**			instance 2: vlan 11-29, vlan 41-50*/
	global_ins_vlan_map[instance_id-1].vlan_start = vlan_start;
	global_ins_vlan_map[instance_id-1].vlan_end = vlan_end;

	instance = &mstp_global_bridge->mstp_instance[instance_id-1];
	instance->msti_vlan.vlan_start = vlan_start;
	instance->msti_vlan.vlan_end = vlan_end;

	for(i = 0; i < MSTP_VLAN_MAP_SIZE; i++)
	{
		instance->msti_vlan.vlan_map[i] |= vlan_map[i];
		global_ins_vlan_map[instance_id-1].vlan_map[i] |= vlan_map[i];
	}

	/*modify cist vlan id map, delete vlan map from cist map*/
	for(vlan_id = vlan_start; vlan_id <= vlan_end; vlan_id++)
	{
		mstp_vlan_map_unset(bridge_cist_vlan_map.vlan_map, vlan_id);
	}


	/*check all port about this instance*/
	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}
		
		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if((IFNET_MODE_INVALID != pif->mode)
			&& (IFNET_MODE_L3 != pif->mode))
		{
			/*mstp port have config [mstp enable]*/
			if((mstp_port != NULL) && (mstp_port->adminStatus == MSTP_TRUE))
			{
MSTP_LOG_DBG("%s: mstp_add_stg port(%s), instance_id = 0, state(%s)!\n",
			__func__, mstp_port->if_name, "MSTP_STATE_BLOCKING");				
				//STG_MODIFY
				/*add instance_id(0), vlan_bitmap, port, state(DISCARDING) */
				mstp_add_stg_vlan(0, mstp_port, MSTP_STATE_BLOCKING);			
			}
		}
	}


	/*if mode is not mstp mode , port no add to msti*/
	if(mstp_global_bridge->stp_mode == PROTO_VERID_MSTP)
	{
		/*vlan add, modify msti port*/
		mstp_add_delete_msti_port_by_instancevlan(instance_id);
	}

	vty_info_out(vty," Instance[%d] config vlan list: vlan start[%d], vlan end[%d]. %s", instance_id, vlan_start, vlan_end, VTY_NEWLINE);

	/*MST Region:vlan and instance mapping*/
	mstp_gen_cfg_digest(mstp_global_bridge);
	mstp_reinit();

	return CMD_SUCCESS;
}


/*no stp vlan cmd*/
DEFUN(no_stp_vlan,
		no_stp_vlan_cmd,
        "no stp vlan <1-4094> {to <1-4094>}",
        NO_STR
        MSTP_STR
        "vlan\n"
        "vlan range, Please input an integer from 1 to 4094\n"
        "range of vlan\n"
        "vlan range, Please input an integer from 1 to 4094\n")
{
	struct mstp_instance		*instance = NULL;
	unsigned int				instance_id = 0;
	unsigned int				vlan_start = 0,vlan_end = 0, vlan_tmp = 0;
    unsigned char				vlan_map[MSTP_VLAN_MAP_SIZE];
	unsigned int				vlan_id = 0;
	unsigned int				vlan_change_flag = MSTP_FALSE;
	
	int 						cursor = 0;

	struct l2if 				*pif = NULL;
	struct mstp_port			*mstp_port = NULL;
	struct hash_bucket			*pbucket = NULL;


	/*get instance id*/
	instance_id = (unsigned int)vty->index;

	/*get vlan id/range*/
	vlan_start = atoi(argv[0]);
	if(argv[1] != NULL)
	{
		vlan_end = atoi(argv[1]);
	}
	else
	{
		vlan_end = vlan_start;
	}

	/*vlan id check*/
	if((vlan_start < MSTP_VLAN_MIN_ID) || (vlan_start > MSTP_VLAN_MAX_ID)
	  || (vlan_end < MSTP_VLAN_MIN_ID) || (vlan_end > MSTP_VLAN_MAX_ID))
	{
		vty_error_out(vty," Wrong parameter range, vlan id is <1-4094>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*vlan_start must <= vlan_end*/
	if(vlan_end <= vlan_start)
	{
		vlan_tmp = vlan_start;
		vlan_start = vlan_end;
		vlan_end = vlan_tmp;
	}
	
	/*vlan_start must not lower than vlan_end*/
	if(vlan_end < vlan_start)
	{
		vty_error_out(vty, " The VLAN list is invalid, (vlan start) smaller than (vlan end).%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*get vlan id map*/
	/*vlan range must contain in global_ins_vlan_map[instance_id].vlan_map*/
	memset(vlan_map, 0, MSTP_VLAN_MAP_SIZE);
	instance = &mstp_global_bridge->mstp_instance[instance_id-1];

	/*check which vlan id belong to this instance*/
	for(vlan_id = vlan_start; vlan_id <= vlan_end; vlan_id++)
	{
		/*if this vlan id belong to this instance id*/
		if(mstp_vlanid_belongto_vlanmap(global_ins_vlan_map[instance_id-1].vlan_map, vlan_id))
		{		
			/*cist vlan map*/
			mstp_vlan_map_set(bridge_cist_vlan_map.vlan_map, vlan_id);

			/*msti vlan map*/
			mstp_vlan_map_unset(global_ins_vlan_map[instance_id-1].vlan_map, vlan_id);

			/*instance vlan map*/
			mstp_vlan_map_unset(instance->msti_vlan.vlan_map, vlan_id);

			vlan_change_flag = MSTP_TRUE;
		}
	}
	
	/*check all port about this instance*/
	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}
		
		mstp_port = pif->mstp_port_info;

		/*judge interface mode*/
		if((IFNET_MODE_INVALID != pif->mode)
			&& (IFNET_MODE_L3 != pif->mode))
		{
			/*mstp port have config [mstp enable]*/
			if((mstp_port != NULL) && (MSTP_TRUE == mstp_port->adminStatus))
			{
MSTP_LOG_DBG("%s: mstp_add_stg port(%s), instance_id = 0, state(%s)!\n",
			__func__, mstp_port->if_name, "MSTP_STATE_BLOCKING");				
				//STG_MODIFY
				/*add instance_id(0), vlan_bitmap, port, state(DISCARDING) */
				mstp_add_stg_vlan(0, mstp_port, MSTP_STATE_BLOCKING);
			
			}
		}
	}

	/*vlan change, modify msti port*/
	if(MSTP_TRUE == vlan_change_flag)
	{
		/*if mode is not mstp mode , port no add to msti
		***/
		if(PROTO_VERID_MSTP == mstp_global_bridge->stp_mode)
		{
			/*vlan add, modify msti port*/
			mstp_add_delete_msti_port_by_instancevlan(instance_id);
		}
	}

	vty_info_out(vty," Instance[%d] delete vlan list: vlan start[%d], vlan end[%d]. %s", instance_id, vlan_start, vlan_end, VTY_NEWLINE);

	/*MST Region:vlan and instance mapping*/
	mstp_gen_cfg_digest(mstp_global_bridge);
	mstp_reinit();

	return CMD_SUCCESS;
}

/*stp priority cmd*/
DEFUN(stp_msti_priority,
		stp_msti_priority_cmd,
        "stp priority <0-61440>",
        MSTP_STR
        "Specify msti priority\n"
        "MSTI priority, in steps of 4096, the default value is 32768. Please input an integer from 0 to 61440\n")
{
	/*change bridge priority value*/
	struct mstp_instance	*instance 	= NULL;
	unsigned short			priority 	= 0;
	unsigned int			instance_id = 0;
    struct mstp_msti_br    	*msti_br 	= NULL;


	instance_id = (unsigned int)vty->index;
	instance = &mstp_global_bridge->mstp_instance[instance_id-1];
	if(NULL == instance)
	{	
		vty_error_out(vty," Instance[%d] is NULL, Can not config. %s", instance_id,VTY_NEWLINE);
		return CMD_WARNING;
	}
	

	priority = atoi(argv[0]);
	if(priority > 61440)
	{
		vty_error_out(vty," Wrong parameter range, vlan id is <0-61440>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
    if(0 != priority % 4096)
	{	
		vty_out(vty,"Error: Priority must be in step of 4096, Allowed values are: %s", VTY_NEWLINE);

		vty_out(vty,"0     4096  8192  12288 16384 20480 24576 28672 %s", VTY_NEWLINE);
		vty_out(vty,"32768 36864 40960 45056 49152 53248 57344 61440 %s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*value no change*/
	if(instance->msti_br_priority == priority)
	{
		vty_info_out(vty," Instance[%d] bridge priority[%d]. %s", instance_id, instance->msti_br_priority, VTY_NEWLINE);
		return CMD_SUCCESS;
	}


	instance->msti_br_priority = priority;
	mstp_global_bridge->msti_br[instance_id-1]->bridge_mac.pri[0] = (unsigned char)(priority >> 8);
	mstp_global_bridge->msti_br[instance_id-1]->bridge_mac.pri[1] = (unsigned char)priority;


	/*update bridge priority*/
	msti_br = mstp_global_bridge->msti_br[instance_id-1];
	if(msti_br != NULL)
	{
		msti_br->bridge_mac 					   		= mstp_get_br_id(priority, instance_id);
		msti_br->bridge_priority.rg_root_id		   		= msti_br->bridge_mac;
		msti_br->bridge_priority.designated_bridge_id	= msti_br->bridge_mac;
	}


	vty_info_out(vty," Instance[%d] bridge priority[%d]. %s", instance_id, instance->msti_br_priority, VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}

/*no stp priority cmd*/
DEFUN(no_stp_msti_priority,
		no_stp_msti_priority_cmd,
        "no stp priority",
        NO_STR
        MSTP_STR
        "Specify bridge priority\n")
{
	struct mstp_instance	*instance = NULL;
	unsigned int			instance_id = 0;
    struct mstp_msti_br    	*msti_br = NULL;

	instance_id = (unsigned int)vty->index;

	/*instance get*/
	instance = &mstp_global_bridge->mstp_instance[instance_id-1];
	if(NULL == instance)
	{	
		vty_error_out(vty," Instance[%d] is NULL, Can not config. %s", instance_id,VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*value no change*/
	if(DEF_BRIDGE_PRIORITY == instance->msti_br_priority)
	{
		vty_info_out(vty," Instance[%d] bridge priority[%d].%s", instance_id, instance->msti_br_priority, VTY_NEWLINE);
		return CMD_SUCCESS;
	}


	/*change bridge priority to default value*/
	instance->msti_br_priority = DEF_BRIDGE_PRIORITY;
	mstp_global_bridge->msti_br[instance_id-1]->bridge_mac.pri[0] = (unsigned char)(DEF_BRIDGE_PRIORITY >> 8);
	mstp_global_bridge->msti_br[instance_id-1]->bridge_mac.pri[1] = (unsigned char)DEF_BRIDGE_PRIORITY;

	/*update bridge priority*/
	msti_br = mstp_global_bridge->msti_br[instance_id-1];
	if(msti_br != NULL)
	{
		msti_br->bridge_mac 					   		= mstp_get_br_id(DEF_BRIDGE_PRIORITY, instance_id);
		msti_br->bridge_priority.rg_root_id		   		= msti_br->bridge_mac;
		msti_br->bridge_priority.designated_bridge_id	= msti_br->bridge_mac;
	}


	vty_info_out(vty," Instance[%d] bridge priority[%d].%s", instance_id, instance->msti_br_priority, VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}

/*************************enter interface view & config ********************************************************/

/*port stp enable cmd*/
DEFUN(interface_stp_enable,
		interface_stp_enable_cmd,
		"stp enable",
		MSTP_STR
		"Enable Spanning Tree Protocol (STP)\n")
{
	unsigned int				if_index	= 0;
	struct l2if					*pif		= NULL;
	struct mstp_port	   		*mstp_port	= NULL;


	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		vty_error_out(vty," %s[%d]:%s interface is NULL, get by ifindex.%s", __FILE__, __LINE__, __func__, VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		vty_error_out(vty," The port mode is invalid, mstp can not enabled.%s", VTY_NEWLINE);
		return CMD_WARNING; 
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING; 	
	}
	
	/*judge mstp port interspace exist status*/
	mstp_port = pif->mstp_port_info;
	if(NULL == mstp_port)	/*port is not exist*/
	{
		/*malloc l2if_stp_port size*/
		pif->mstp_port_info = (struct mstp_port *)XMALLOC(MTYPE_MSTP_PORT, sizeof(struct mstp_port));
		if(NULL == pif->mstp_port_info)
		{
			MSTP_LOG_DBG("%s[%d]:%s: ERROR: L2 stp port calloc memery error!\n", __FILE__, __LINE__, __func__);
			vty_error_out(vty," (L2 stp port) calloc interface space memery error! %s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		mstp_port = pif->mstp_port_info;

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
	else
	{
		/*check port enable function*/
		if(MSTP_TRUE == mstp_port->adminStatus)
		{
			vty_error_out(vty, " Mstp on the port is already enabled. %s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	/*if mode is not mstp mode , port no add to msti
	***/
	if(PROTO_VERID_MSTP == mstp_global_bridge->stp_mode)
	{
		/*port mstp enable, judge if add instance(msti-port-list)*/
		mstp_add_del_msti_port_by_mportenable(mstp_port);
	}

	mstp_reinit();


	return CMD_SUCCESS;
}


/*port no stp enable cmd*/
DEFUN(no_interface_stp_enable, 
		no_interface_stp_enable_cmd,
		"no stp enable",
	    NO_STR
		MSTP_STR
		"Enable Spanning Tree Protocol (STP)\n")
{
	/*delete instance, clear all port stp in this instance*/
	struct mstp_port			*port		= NULL;
	struct l2if					*pif		= NULL;
	unsigned int				if_index = 0;

	/*interface index*/
	if_index = (unsigned int)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		vty_error_out(vty," The port mode is invalid, mstp can not (no stp enabled).%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING; 	
	}

	/*judge mstp port exist status*/
	port = pif->mstp_port_info;
	if(NULL == port)	/*port is not exist*/
	{
		vty_error_out(vty," The port mstp not enabled, can not (no stp enabled) cmd.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}
	
	/*mstp port mstp disable*/
	port->adminStatus = MSTP_FALSE;

	/*delete bridge port list*/

	/*delete cist port*/
	list_del(&port->port_list);
	mstp_del_cist_port(port);

	/*add instance_id(0), vlan_bitmap, port, state(FORWARDING) */
	//STG_MODIFY
MSTP_LOG_DBG("%s: mstp_add_stg port(%s), instance_id = 0, state(%s)!\n",
				__func__, port->if_name, "MSTP_STATE_REMOVE");	
	mstp_add_stg_vlan(0, port, MSTP_STATE_REMOVE);


	/*if mode is not mstp mode , port no add to msti
	***/
	if(mstp_global_bridge->stp_mode == PROTO_VERID_MSTP)
	{
		/*delete MSTI port*/
		mstp_add_del_msti_port_by_mportenable(port);
	}

	XFREE(MTYPE_MSTP_PORT, port);
	pif->mstp_port_info = NULL;

	mstp_reinit();


	return CMD_SUCCESS;
}


/*config interface priority cmd*/
DEFUN(interface_stp_priority, 
		interface_stp_priority_cmd,
		"stp priority <0-240>",
		MSTP_STR
		"Specify port priority\n"
		"Port priority, in steps of 16\n")
{
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;
	unsigned char				priority	= 0;

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	priority = atoi(argv[0]);
	if(priority > MAX_PORT_PRIORITY)
	{
		vty_error_out(vty," Wrong parameter range, instance id range is <0-240>. %s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	if(0 != priority % 16)
	{
		vty_out(vty,"Error: Port Priority must be in step of 16, Allowed values are: %s", VTY_NEWLINE);

		vty_out(vty,"0    16   32  48   64   80   96   112%s", VTY_NEWLINE);
		vty_out(vty,"128  144  160 176  192  208  224  240 %s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*value no change*/
	if(pif->mstp_port_info->port_pri == priority)
	{
		vty_info_out(vty," The port config priority[%d] %s", priority, VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	
	pif->mstp_port_info->port_pri = priority;

	/*in mstp_updt_roles_tree have make priority introduced*/

	vty_info_out(vty," The port config priority[%d] %s", priority, VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}

/*no stp priority cmd*/
DEFUN(no_interface_stp_priority, 
		no_interface_stp_priority_cmd,
		"no stp priority",
	    NO_STR
		MSTP_STR
		"Specify port priority\n"
		"Port priority, in steps of 16\n")
{
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}
		
	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*value no change*/
	if(DEF_PORT_PRIORITY == pif->mstp_port_info->port_pri)
	{
		vty_info_out(vty," The port config priority to default[%d] %s", DEF_PORT_PRIORITY, VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	pif->mstp_port_info->port_pri = DEF_PORT_PRIORITY;
	
	vty_info_out(vty," The port config priority to default[%d] %s", DEF_PORT_PRIORITY, VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}

/*stp port cost cmd*/
DEFUN(interface_stp_cost, 
		interface_stp_cost_cmd,
		"stp cost <1-200000000>",
		MSTP_STR
		"Specify port path cost\n"
		"Port path cost, Please input an integer from 1 to 200,000,000\n")
{
	/*config stp port path cost*/
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;	
	unsigned int				port_cost 	= 0;

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}
		
	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}


	port_cost = atoi(argv[0]);
	if((port_cost < MIN_PORT_PATH_COST) || (port_cost > MAX_PORT_PATH_COST))
	{	
		vty_error_out(vty," Wrong parameter range, port path cost range is <1-200,000,000>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*value no change*/
	if(pif->mstp_port_info->port_path_cost == port_cost)
	{
		vty_info_out(vty," The port config cost[%d] %s", port_cost, VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	/*set l2if port path cost*/
	pif->mstp_port_info->port_path_cost = port_cost;
	pif->mstp_port_info->cist_port.common_info.port_cost = port_cost;
	pif->mstp_port_info->cist_port.common_info.AdminPathCost = MSTP_TRUE;

	vty_info_out(vty," The port config cost[%d] %s", port_cost, VTY_NEWLINE);

	mstp_reinit();
	
	return CMD_SUCCESS;
}


/*no stp port cost cmd*/
DEFUN(no_interface_stp_cost,
		no_interface_stp_cost_cmd,
		"no stp cost",		
	    NO_STR
		MSTP_STR
		"Specify port path cost\n")
{
	/*config stp port path cost is default value*/
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;	

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}


	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*value no change*/
	if(pif->mstp_port_info->port_path_cost == DEF_PORT_PATH_COST)
	{
		vty_info_out(vty," The port config cost to default[%d] %s", DEF_PORT_PATH_COST, VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	/*set l2if port path cost*/
	pif->mstp_port_info->port_path_cost = DEF_PORT_PATH_COST;	
	pif->mstp_port_info->cist_port.common_info.port_cost = DEF_PORT_PATH_COST;
	pif->mstp_port_info->cist_port.common_info.AdminPathCost = MSTP_FALSE;

	vty_info_out(vty," The port config cost to default[%d] %s", DEF_PORT_PATH_COST, VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}


/*config interface priority cmd*/
DEFUN(interface_stp_instance_priority, 
		interface_stp_instance_priority_cmd,
		"stp instance <1-8> port priority <0-240>",
		MSTP_STR		
		"Spanning tree instance\n"
		"Identifier of spanning tree instance"
		"Specify port parameter\n"
		"Specify port priority\n"
		"Port priority, in steps of 16, The default is 128.\n")

{
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;	
	unsigned int				instance_id	= 0;
	unsigned int				priority	= 0;


	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}


	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(pif->mstp_port_info == NULL)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}


	/*get instance value*/
	instance_id = atoi(argv[0]);
	if((instance_id < MSTP_INSTANCE_MIN) || (instance_id > MSTP_INSTANCE_MAX))
	{	
		vty_error_out(vty," Wrong parameter range, instance id range is <1-8>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	/*get priority value*/
	priority = atoi(argv[1]);
	if(priority > MAX_PORT_PRIORITY)
	{
		vty_error_out(vty," Wrong parameter range, instance id range is <0-240>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	if(0 != priority % 16)
	{
		vty_out(vty,"Error: Port Priority must be in step of 16, Allowed values are: %s", VTY_NEWLINE);

		vty_out(vty,"0    16   32  48   64   80   96   112%s", VTY_NEWLINE);
		vty_out(vty,"128  144  160 176  192  208  224  240 %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(pif->mstp_port_info->msti_port_pri[instance_id-1] == priority)
	{
		vty_info_out(vty," The	instance[%d] port priority[%d] %s", instance_id, priority, VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	pif->mstp_port_info->msti_port_pri[instance_id-1] = priority;

	vty_info_out(vty," The  instance[%d] port priority[%d] %s", instance_id, priority, VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}


/*config interface priority cmd*/
DEFUN(no_interface_stp_instance_priority, 
		no_interface_stp_instance_priority_cmd,
		"no stp instance <1-8> port priority",
		NO_STR
		MSTP_STR
		"Spanning tree instance\n"
		"Identifier of spanning tree instance"
		"Specify port parameter\n"
		"Specify port priority\n"
		"Port priority, in steps of 16, The default is 128.\n")

{
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;	
	unsigned int				instance_id	= 0;


	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}


	/*get instance value*/
	instance_id = atoi(argv[0]);
	if((instance_id < MSTP_INSTANCE_MIN) || (instance_id > MSTP_INSTANCE_MAX))
	{	
		vty_error_out(vty," Wrong parameter range, instance id range is <1-8>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*value no change*/
	if(DEF_PORT_PRIORITY == pif->mstp_port_info->msti_port_pri[instance_id-1])
	{
		vty_info_out(vty, " The instance[%d] port priority to default[%d] %s", instance_id, DEF_PORT_PRIORITY, VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	pif->mstp_port_info->msti_port_pri[instance_id-1] = DEF_PORT_PRIORITY;

	vty_info_out(vty, " The instance[%d] port priority to default[%d] %s", instance_id, DEF_PORT_PRIORITY, VTY_NEWLINE);

	mstp_reinit();
	
	return CMD_SUCCESS;
}


/*config interface priority cmd*/
DEFUN(interface_stp_instance_cost, 
		interface_stp_instance_cost_cmd,
		"stp instance <1-8> cost <1-200000000>",
		MSTP_STR
		"Spanning tree instance\n"
		"Identifier of spanning tree instance"
		"Specify port path cost\n"
		"Port path cost, Please input an integer from 1 to 200,000,000\n")
{
	unsigned int				if_index	= 0;	
	unsigned int				instance_id	= 0;
	unsigned int				port_cost;
	
	struct l2if					*pif		= NULL;
	struct mstp_instance		*instance	= NULL;

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*get instance value*/
	instance_id = atoi(argv[0]);
	if((instance_id < MSTP_INSTANCE_MIN) || (instance_id > MSTP_INSTANCE_MAX))
	{	
		vty_error_out(vty," Wrong parameter range, instance id range is <1-8>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	/*get port cost value*/
	port_cost = atoi(argv[1]);
	if((port_cost < MIN_PORT_PATH_COST) || (port_cost > MAX_PORT_PATH_COST))
	{	
		vty_error_out(vty," Wrong parameter range, port path cost range is <1-200,000,000>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*value no change*/
	if(pif->mstp_port_info->msti_port_path_cost[instance_id-1] == port_cost)
	{
		vty_info_out(vty," The instance[%d] port cost[%d] %s", instance_id, port_cost, VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	/*set l2if port path cost*/
	pif->mstp_port_info->msti_port_path_cost[instance_id-1] = port_cost;

	/*when msti port is exist, change port cost vlaue*/
	if(pif->mstp_port_info->msti_port[instance_id-1] != NULL)
	{
		pif->mstp_port_info->msti_port[instance_id-1]->common_info.AdminPathCost = MSTP_TRUE;
		pif->mstp_port_info->msti_port[instance_id-1]->common_info.port_cost = port_cost;
	}
	instance = &mstp_global_bridge->mstp_instance[instance_id-1];
	if(instance != NULL)
	{
		/*if msti port is NULL, when msti port init copy value to msti_AdminPathCost*/
		instance->AdminPathCost = MSTP_TRUE;
	}


	vty_info_out(vty," The instance[%d] port cost[%d] %s", instance_id, port_cost, VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}


/*config interface priority cmd*/
DEFUN(no_interface_stp_instance_cost, 
		no_interface_stp_instance_cost_cmd,
		"no stp instance <1-8> cost",
		NO_STR
		MSTP_STR
		"Spanning tree instance\n"
		"Identifier of spanning tree instance"
		"Specify port path cost\n")
{
	unsigned int				if_index	= 0;	
	unsigned int				instance_id	= 0;

	struct l2if					*pif		= NULL;
	struct mstp_instance		*instance 	= NULL;

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}


	/*get instance value*/
	instance_id = atoi(argv[0]);
	if((instance_id < MSTP_INSTANCE_MIN) || (instance_id > MSTP_INSTANCE_MAX))
	{	
		vty_error_out(vty," Wrong parameter range, instance id range is <1-8>. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*value no change*/
	if(DEF_PORT_PATH_COST == pif->mstp_port_info->msti_port_path_cost[instance_id-1])
	{
		vty_info_out(vty," The instance[%d] port cost[%d] %s", instance_id, DEF_PORT_PATH_COST, VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	pif->mstp_port_info->msti_port_path_cost[instance_id-1] = DEF_PORT_PATH_COST;
	
	/*when msti port is exist, change port cost vlaue*/
	if(pif->mstp_port_info->msti_port[instance_id-1] != NULL)
	{
		pif->mstp_port_info->msti_port[instance_id-1]->common_info.AdminPathCost = MSTP_FALSE;
		pif->mstp_port_info->msti_port[instance_id-1]->common_info.port_cost = DEF_PORT_PATH_COST;
	}

	instance = &mstp_global_bridge->mstp_instance[instance_id-1];
	if(instance != NULL)
	{	
		/*if msti port is NULL, when msti port init copy value to msti_AdminPathCost*/
		instance->AdminPathCost = MSTP_FALSE;
	}

	vty_info_out(vty," The instance[%d] port cost[%d] %s", instance_id, DEF_PORT_PATH_COST, VTY_NEWLINE);

	mstp_reinit();
	
	return CMD_SUCCESS;
}


/*stp edge-port cmd*/
DEFUN(interface_stp_edge, 
		interface_stp_edge_cmd,
		"stp edge-port",
		MSTP_STR
		"Specify edge port\n")
{
	/*config stp port edge port(enable)*/
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*value no change*/
	if(MSTP_TRUE == pif->mstp_port_info->mstp_edge_port)
	{	
		vty_info_out(vty," The port config edge port Enable%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	/*set mstp port edge*/
	pif->mstp_port_info->mstp_edge_port = MSTP_TRUE;
	pif->mstp_port_info->port_variables.AdminEdge = MSTP_TRUE;

	vty_info_out(vty," The port config edge port Enable%s", VTY_NEWLINE);

	mstp_reinit();		//FIXME

	return CMD_SUCCESS;
}


/*no stp edge-port cmd*/
DEFUN(no_interface_stp_edge,
		no_interface_stp_edge_cmd,
		"no stp edge-port",
	    NO_STR
		MSTP_STR
		"Specify edge port\n")
{
	/*config stp port edge port(disable)*/
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*value no change*/
	if(MSTP_FALSE == pif->mstp_port_info->mstp_edge_port)
	{	
		vty_info_out(vty," The port config edge port Disable%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	pif->mstp_port_info->mstp_edge_port = MSTP_FALSE;	
	pif->mstp_port_info->port_variables.AdminEdge = MSTP_FALSE;

	vty_info_out(vty," The port config edge port Disable%s", VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}



/*stp filter-port cmd*/
DEFUN(interface_stp_filter_port, 
		interface_stp_filter_port_cmd,
		"stp filter-port",
		MSTP_STR
		"Enable port's BPDU filter function\n")
{
	/*config stp edge filter port*/
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*value no change*/
	if(MSTP_TRUE == pif->mstp_port_info->mstp_filter_port)
	{
		vty_info_out(vty," The port config filter port Enable%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	pif->mstp_port_info->mstp_filter_port = MSTP_TRUE;

	/*the function is no work, need to modify*/
	vty_info_out(vty," The port config filter port Enable%s", VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}

/*no stp filter-port cmd*/
DEFUN(no_interface_stp_filter_port, 
		no_interface_stp_filter_port_cmd,
		"no stp filter-port",
	    NO_STR
		MSTP_STR
		"Enable port's BPDU filter function\n")
{
	/*config stp*/
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*value no change*/
	if(MSTP_FALSE == pif->mstp_port_info->mstp_filter_port)
	{
		vty_info_out(vty," The port config filter port Disable%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	pif->mstp_port_info->mstp_filter_port = MSTP_FALSE;
	
	vty_info_out(vty," The port config filter port Disable%s", VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}

/*stp p2p cmd*/
DEFUN(interface_stp_p2p, 
		interface_stp_p2p_cmd,
		"stp p2p",
		MSTP_STR
		"Specify point to point link\n")
{
	/*config stp port point to point link*/
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*vlaue no change*/
	if(MSTP_TRUE == pif->mstp_port_info->mstp_p2p)
	{
		vty_info_out(vty," The port config P2P Enable%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	/*maybe need to check mport link duplex*/
	pif->mstp_port_info->mstp_p2p = MSTP_TRUE;
	pif->mstp_port_info->port_variables.AdminLink = MSTP_TRUE;

	/*bug#52884 bug#52835
	<rstp>P/A机制必须在点到点链路上进行
	<rstp>根端口down，指定端口收到更优rst bpdu，经过3个hello time才变为新的根端口*/
	
	mstp_get_link_state(pif->mstp_port_info, if_index);

	vty_info_out(vty," The port config P2P Enable%s", VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}


/*no stp p2p cmd*/
DEFUN(no_interface_stp_p2p, 
		no_interface_stp_p2p_cmd,
		"no stp p2p",
	    NO_STR
		MSTP_STR
		"Specify point to point link\n")
{
	/*config no stp port point to point link*/
	/*Decided by Spanning Tree Protocol*/
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}
	
	/*vlaue no change*/
	if(MSTP_FALSE == pif->mstp_port_info->mstp_p2p)
	{
		vty_info_out(vty," The port config P2P Disable%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	/*maybe need to check mport link duplex*/
	pif->mstp_port_info->mstp_p2p = MSTP_FALSE;
	pif->mstp_port_info->port_variables.AdminLink = MSTP_FALSE;

	/*bug#52884 bug#52835
	<rstp>P/A机制必须在点到点链路上进行
	<rstp>根端口down，指定端口收到更优rst bpdu，经过3个hello time才变为新的根端口*/
	
	mstp_get_link_state(pif->mstp_port_info, if_index);

	vty_info_out(vty," The port config P2P Disable%s", VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}
#if 0
/*stp force_true or frce_false cmd*/
/*
a)ForceTrue, operPointToPointMAC shall be TRUE, regardless of any indications to the contrary
generated by the service providing entity.
b) ForceFalse, operPointToPointMAC shall be FALSE.
*/
DEFUN(interface_stp_force, 
		interface_stp_force_cmd,
		"stp force (true | false)",
		MSTP_STR
		"Force point to point link\n"
		"Force p2p function\n")
{
	/*config stp port point to point link*/
	struct l2if					*pif		= NULL;
	unsigned int				if_index	= 0;

	/*get interface index*/
	if_index = (unsigned int)vty->index;
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		MSTP_LOG_DBG("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," Port get pif is NULL by interface index.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*judge interface mode*/
	if(IFNET_MODE_INVALID == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is invalid, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else if(IFNET_MODE_L3 == pif->mode)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(NULL == pif->mstp_port_info)
	{
		MSTP_LOG_DBG("%s[%d]:%s Error: no the interface mstp info, maybe the interface no enabled mstp.\n", __FILE__, __LINE__, __func__);
		vty_error_out(vty," No the interface mstp info, maybe the interface no enabled mstp.%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	/*vlaue no change*/
	if(MSTP_TRUE == pif->mstp_port_info->mstp_p2p)
	{
		vty_info_out(vty," The port config P2P Enable%s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	if(!strncmp(argv[0],"t",strlen("t")))
	{
		pif->mstp_port_info->port_variables.operPointToPointMAC = MSTP_TRUE; 
		pif->mstp_port_info->mstp_force = MSTP_TRUE;
		
	}
	else 
	{
		pif->mstp_port_info->port_variables.operPointToPointMAC = MSTP_FALSE; 
		pif->mstp_port_info->mstp_force = MSTP_FALSE;
	}
	
	vty_info_out(vty," The port config P2P Enable%s", VTY_NEWLINE);

	mstp_reinit();

	return CMD_SUCCESS;
}
#endif

/*********************show cmd****************************************/
/*show stp interface [USP]*/
DEFUN (show_stp_interface,
		show_stp_interface_cmd,
		"show stp interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP | trunk TRUNK}",
        SHOW_STR
        MSTP_STR
        "Specify interface\n"
        "Ethernet interface type\n"
        "The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
		"The port/subport of gigabit ethernet, format: <0-7>/<0-31>/<1-255>\n"
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		"The port/subport of 10gigabit ethernet, format: <0-7>/<0-31>/<1-255>\n"
        "trunk interface type\n"
        "The port/subport of trunk, format: <1-128>\n")
{
	/*show stp interface include port forward state and port statics*/
	unsigned int 			if_index 	= 0;
	struct l2if				*pif 		= NULL;
	struct mstp_port		*mstp_port 	= NULL;


	/* argv[0] : interface name , ethernet interface*/
	if((NULL != argv[0]) || (NULL != argv[1]) || (NULL != argv[2]) || (NULL != argv[3]))
	{
		if((NULL != argv[0]) && (NULL == argv[1]) && (NULL == argv[2]) && (NULL == argv[3]))
		{
			if_index = ifm_get_ifindex_by_name("ethernet", (char *)argv[0]);
		}
		else if((NULL == argv[0]) && (NULL != argv[1]) && (NULL == argv[2]) && (NULL == argv[3]))
		{
			if_index = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[1]);
		}
		else if((NULL == argv[0]) && (NULL == argv[1]) && (NULL != argv[2]) && (NULL == argv[3]))
		{
			if_index = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[2]);
		}
		else if((NULL == argv[0]) && (NULL == argv[1]) && (NULL == argv[2]) && (NULL != argv[3]))
		{
			if(NULL == trunk_lookup(atoi(argv[3])))
			{
				vty_error_out(vty, " No mstp info, Maybe the trunk no enabled mstp.%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
			if_index = ifm_get_ifindex_by_name("trunk", (char *)argv[3]);
		}
	
		if(0 == if_index)	// 0 means interface not exist
		{
			vty_error_out(vty, " Get if index is NULL, Maybe the interface no exist.%s", VTY_NEWLINE);
			MSTP_LOG_DBG("%s: ERROR: get if index is NULL, Maybe the interface no exist.\n", __func__);
			return CMD_WARNING;
		}

		pif = l2if_get(if_index);
		if(NULL == pif)
		{
			vty_error_out(vty, " Maybe the interface no exist.%s", VTY_NEWLINE);
			MSTP_LOG_DBG("%s[%d]%s: pif is NULL.\n",__FILE__,__LINE__,__func__);
			return CMD_WARNING;
		}

		/*judge interface mode*/
		if(IFNET_MODE_INVALID == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not execute.\n", __FILE__, __LINE__, __func__);
			vty_error_out(vty," The port mode is invalid, mstp cmd can not execute.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		else if(IFNET_MODE_L3 == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
			return CMD_WARNING; 	
		}

		mstp_port = pif->mstp_port_info;
		if(mstp_port != NULL)
		{
			/*display mstp region and global config*/
			mstp_show_common(vty);

			/*display (mstp interface config & role) and (msti port config & role)*/
			mstp_show_if(vty, mstp_port);
			return CMD_SUCCESS;
		}
		else
		{
			vty_error_out(vty, " No the mstp info, Maybe the interface no enabled stp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	/*show all*/	
	else if((NULL == argv[0]) && (NULL == argv[1]) && (NULL == argv[2]) && (NULL == argv[3]))
	{	
		mstp_show_common(vty);
		mstp_show_all_if(vty);

		return CMD_SUCCESS;
	}

	return CMD_SUCCESS;
}

/*show stp instance*/
DEFUN(show_stp_instance,
		show_stp_instance_cmd,
        "show stp {instance <0-8>}",
        SHOW_STR
        MSTP_STR
        "Spanning tree instance\n"
        "Identifier of spanning tree instance, Please input an integer from 0 to 8\n")
{
	unsigned int				instance_id = 0;
	struct mstp_msti_port		*msti_port = NULL;
	
	/*Specify the instance number*/
	if(NULL != argv[0])	 
	{
		/*get instance id*/
		instance_id = atoi(argv[0]);

		if(0 == instance_id)
		{		
			/*show mstp region / config info */
			mstp_show_common(vty);

			/*all interface cist info*/
			mstp_show_all_cist_if(vty);
		}
		else
		{
			if(PROTO_VERID_MSTP == mstp_global_bridge->stp_mode)
			{
				/*show mstp region / config info */
				mstp_show_common(vty);			
			
				if((instance_id < MSTP_INSTANCE_MIN) || (instance_id > MSTP_INSTANCE_MAX))
				{
					vty_error_out(vty," Wrong instance range, instance id is <1-8>. %s", VTY_NEWLINE);				
					return CMD_WARNING;
				}
				
				vty_out(vty,"%s", VTY_NEWLINE);
				vty_out(vty, "-------[Instance %d Global Info]-------%s", instance_id, VTY_NEWLINE);
				
				/*instance config info*/
				mstp_show_instance_cfg(vty, instance_id);
				vty_out(vty,"%s", VTY_NEWLINE);
				
				/*MSTI port list*/
				msti_port = mstp_global_bridge->mstp_instance[instance_id-1].msti_port_list;
				if(NULL == msti_port)
				{
					vty_out(vty, "Info: No instance[%d] port information is available. %s", instance_id, VTY_NEWLINE);
					return CMD_WARNING;
				}
				
				for( ; msti_port != NULL; msti_port = msti_port->next)
				{
					/*display msti port*/
					mstp_show_msti_port(vty, instance_id, msti_port);
				}
			}
			else
			{
				vty_out(vty, "Info: No instance[%d] information is available. %s", instance_id, VTY_NEWLINE);
				return CMD_WARNING;
			}
		}
	}
	else  /*all instance*/
	{	
		/*show mstp region / config info */
		mstp_show_common(vty);

		/*all interface cist info*/
		mstp_show_all_cist_if(vty);

		/*mstp mode*/
		if(PROTO_VERID_MSTP == mstp_global_bridge->stp_mode)
		{
			/*all intance info*/
			for(instance_id = 1; instance_id <= MSTP_INSTANCE_MAX; instance_id++)
			{		
				vty_out(vty,"%s", VTY_NEWLINE);
				vty_out(vty, "----------[Instance %d Global Info]----------%s", instance_id, VTY_NEWLINE);
				
				/*instance config info*/
				mstp_show_instance_cfg(vty, instance_id);
				vty_out(vty,"%s", VTY_NEWLINE);

				/*mstp port list*/
				vty_out(vty, "Instance[%d], enabled mstp port: %s", instance_id, VTY_NEWLINE);

				/*MSTI port list*/		
				msti_port = mstp_global_bridge->mstp_instance[instance_id-1].msti_port_list;
				if(NULL == msti_port)
				{
					vty_out(vty, "Info: No instance[%d] port information is available. %s", instance_id, VTY_NEWLINE);			
					continue;
				}

				for( ; msti_port != NULL; msti_port = msti_port->next)
				{
					mstp_show_msti_port(vty, instance_id, msti_port);
				}
			}
		}
	}
	
	return CMD_SUCCESS;
}


/*debug mstp running para*/
DEFUN(debug_stp_running_info,
		debug_stp_running_info_cmd,
        "debug mstp running-info interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP | trunk TRUNK}",
        DEBUG_STR
        MSTP_STR
        "The current running info of mstp\n"
        "Specify interface\n"
        "Ethernet interface type\n"
        "The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
		"The port/subport of gigabit ethernet, format: <0-7>/<0-31>/<1-255>\n"
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		"The port/subport of 10gigabit ethernet, format: <0-7>/<0-31>/<1-255>\n"
        "trunk interface type\n"
        "The port/subport of trunk, format: <1-128>\n")
{
	unsigned int 			if_index 	= 0;
	struct l2if				*pif 		= NULL;
	struct mstp_port		*mstp_port 	= NULL;

	/* argv[0] : interface name , ethernet interface*/
	if((NULL != argv[0]) || (NULL != argv[1]) || (NULL != argv[2]) || (NULL != argv[3]))
	{
		if((NULL != argv[0]) && (NULL == argv[1]) && (NULL == argv[2]) && (NULL == argv[3]))
		{
			if_index = ifm_get_ifindex_by_name("ethernet", (char *)argv[0]);
		}
		else if((NULL == argv[0]) && (NULL != argv[1]) && (NULL == argv[2]) && (NULL == argv[3]))
		{
			if_index = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[1]);
		}
		else if((NULL == argv[0]) && (NULL == argv[1]) && (NULL != argv[2]) && (NULL == argv[3]))
		{
			if_index = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[2]);
		}
		else if((NULL == argv[0]) && (NULL == argv[1]) && (NULL == argv[2]) && (NULL != argv[3]))
		{
			if(NULL == trunk_lookup(atoi(argv[3])))
			{
				vty_error_out(vty, " No mstp info, Maybe the trunk no enabled mstp.%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
			if_index = ifm_get_ifindex_by_name("trunk", (char *)argv[3]);
		}
	
		if(0 == if_index)	// 0 means interface not exist
		{
			vty_error_out(vty, " Get if index is NULL, Maybe the interface no exist.%s", VTY_NEWLINE);
			MSTP_LOG_DBG("%s: ERROR: get if index is NULL, Maybe the interface no exist.\n", __func__);
			return CMD_WARNING;
		}

		pif = l2if_get(if_index);
		if(NULL == pif)
		{
			vty_error_out(vty, " Maybe the interface no exist.%s", VTY_NEWLINE);
			MSTP_LOG_DBG("%s[%d]%s: pif is NULL.\n",__FILE__,__LINE__,__func__);
			return CMD_WARNING;
		}

		/*judge interface mode*/
		if(IFNET_MODE_INVALID == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is invalid, mstp cmd can not execute.\n", __FILE__, __LINE__, __func__);
			vty_error_out(vty," The port mode is invalid, mstp cmd can not execute.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		else if(IFNET_MODE_L3 == pif->mode)
		{
			MSTP_LOG_DBG("%s[%d]:%s Error: the port mode is L3, mstp cmd can not config.\n", __FILE__, __LINE__, __func__);
			vty_error_out(vty," The port mode is L3, mstp cmd can not config.%s", VTY_NEWLINE);
			return CMD_WARNING; 	
		}

		mstp_port = pif->mstp_port_info;
		if(mstp_port != NULL)
		{
			mstp_debug_if(vty, mstp_port);
			return CMD_SUCCESS;
		}
		else
		{
			vty_error_out(vty, " No the mstp info, Maybe the interface no enabled stp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	/*debug all*/	
	else if((NULL == argv[0]) && (NULL == argv[1]) && (NULL == argv[2]) && (NULL == argv[3]))
	{	
		mstp_debug_all_if(vty);
		return CMD_SUCCESS;
	}
	
	return CMD_SUCCESS;
}


int mstp_config_write(struct vty *vty)
{
	/*get instance set cmd*/
	unsigned int			instance_id = 0;	/*instance id*/
	unsigned int			i = 0;
	char					vlan_str[256] = {0}, *vlan_item_str = NULL, *pvlan_cmd = NULL;
	char					vlan_cmd[256];
	char					mst_cfg_name[64];
	
	/*------------------whole config--------------------*/

	/*region name config*/
	for(i = 0; i < MAC_LEN; i++)
	{
		sprintf(&mst_cfg_name[2*i], "%02X", mstp_global_bridge->bridge_mac.mac_addr[i]);
	}

	if(memcmp(mstp_global_bridge->mst_cfg_id.cfg_name, mst_cfg_name, MAC_LEN*2) != 0)
	{
		vty_out(vty, "stp region-name %s %s", mstp_global_bridge->mst_cfg_id.cfg_name, VTY_NEWLINE);
	}

	/*revision level*/
	if(mstp_global_bridge->mst_cfg_id.revison_level != 0)
	{
		vty_out(vty, "stp revision-level %d %s", mstp_global_bridge->mst_cfg_id.revison_level, VTY_NEWLINE);
	}



	/*stp mode*/
	if(mstp_global_bridge->stp_mode != PROTO_VERID_MSTP)
	{
		switch(mstp_global_bridge->stp_mode)
		{
			case PROTO_VERID_STP:
			{
				vty_out(vty, "stp mode stp %s", VTY_NEWLINE);
			}
				break;
			case PROTO_VERID_RSTP:
			{
				vty_out(vty, "stp mode rstp %s", VTY_NEWLINE);
			}
				break;
			/*	
			case PROTO_VERID_MSTP:
			{
				vty_out(vty, "stp mode mstp %s", VTY_NEWLINE);
			}
			*/
				break;
			default:
				break;
		}
	}

	/*stp priority*/
	if(mstp_global_bridge->br_priority != DEF_BRIDGE_PRIORITY)
	{
		vty_out(vty, "stp priority %d %s", mstp_global_bridge->br_priority, VTY_NEWLINE);
	}


	/*stp root cmd*/
	switch(mstp_global_bridge->bridge_root_role)
	{
	
		case MSTP_ROOT_DEF:
		{
			/*do nothing*/
		}
			break;
		case MSTP_ROOT_MASTER:
		{
			vty_out(vty, "stp root master %s", VTY_NEWLINE);
		}
			break;
		case MSTP_ROOT_SLAVE:
		{
			vty_out(vty, "stp root slave %s", VTY_NEWLINE);
		}
			break;
		default:
			break;
	}
	
	/*stp hello interval cmd*/
	if(mstp_global_bridge->hello_time != MSTP_DEFAULT_HELLO_TIME)
	{			
		vty_out(vty, "stp hello-time %d %s", mstp_global_bridge->hello_time, VTY_NEWLINE);
	}
	
	/*stp forward delay cmd*/
	if(mstp_global_bridge->fwd_delay != MSTP_DEFAULT_FWD_DELAY)
	{			
		vty_out(vty, "stp forward-delay %d %s", mstp_global_bridge->fwd_delay, VTY_NEWLINE);
	}
	
	/*stp max-age cmd*/
	if(mstp_global_bridge->msg_max_age != MSTP_DEFAULT_MAX_AGE)
	{			
		vty_out(vty, "stp max-age %d %s", mstp_global_bridge->msg_max_age, VTY_NEWLINE);
	}
	
	/*stp max-hop cmd*/
	if(mstp_global_bridge->msg_max_hop_count != MSTP_DEFAULT_MAX_HOP)
	{			
		vty_out(vty, "stp max-hop %d %s", mstp_global_bridge->msg_max_hop_count, VTY_NEWLINE);
	}


	/*----------------------instance config-----------------------------*/
	
	/*traverse all instance*/
	for(instance_id = 0; instance_id < MSTP_INSTANCE_MAX; instance_id++)
	{
		/*inter instance view*/
		vty_out(vty, "stp instance %d %s", instance_id+1, VTY_NEWLINE);

		/*vlan id config*/
		for(i = 0; i < MSTP_VLAN_MAP_SIZE; i++)
		{
			/*To determine whether a vlan id belongs to the instances*/ 
			if(global_ins_vlan_map[instance_id].vlan_map[i] != 0)
			{
				/*get instance id vlan map*/
				mstp_vlan_str_get_by_bitmap(mstp_global_bridge->mstp_instance[instance_id].msti_vlan.vlan_map, vlan_str);
				pvlan_cmd = vlan_str;

				/*write instance vlan cmd one by one*/
				while((vlan_item_str = strtok(pvlan_cmd, ",")) != NULL)
				{
					memset(vlan_cmd, 0, 256);
					sprintf(vlan_cmd, "stp vlan %s", vlan_item_str);
					vty_out(vty, "%s %s", vlan_cmd, VTY_NEWLINE);

					pvlan_cmd = NULL;
				}
				break;
			}			
		}	

		/*stp priority cmd*/
		if(mstp_global_bridge->mstp_instance[instance_id].msti_br_priority != DEF_BRIDGE_PRIORITY)
		{
			vty_out(vty, "stp priority %d %s", mstp_global_bridge->mstp_instance[instance_id].msti_br_priority, VTY_NEWLINE);
		}
	}

    return CMD_SUCCESS;
}

void mstp_vty_init(void)
{

    install_node (&mstp_node, mstp_config_write);
    install_default (MSTP_NODE);
	install_element (CONFIG_NODE, &mstp_mode_cmd, CMD_SYNC ); 	

	/*------------------------whole config cmd-------------------------*/

	/*region name*/
    install_element (CONFIG_NODE, &stp_region_name_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_stp_region_name_cmd, CMD_SYNC);
	/*revision level*/
	install_element (CONFIG_NODE, &stp_revision_level_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_stp_revision_level_cmd, CMD_SYNC);
	/*stp mode*/
	install_element (CONFIG_NODE, &stp_mode_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_stp_mode_cmd, CMD_SYNC);
	/*stp priority*/
	install_element (CONFIG_NODE, &stp_priority_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_stp_priority_cmd, CMD_SYNC);
	/*stp root*/
	install_element (CONFIG_NODE, &stp_root_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_stp_root_cmd, CMD_SYNC);
	/*stp hello-time*/
	install_element (CONFIG_NODE, &stp_hello_time_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_stp_hello_time_cmd, CMD_SYNC);
	/*stp forward-delay*/
	install_element (CONFIG_NODE, &stp_forward_delay_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_stp_forward_delay_cmd, CMD_SYNC);
	/*stp max-age*/
	install_element (CONFIG_NODE, &stp_max_age_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_stp_max_age_cmd, CMD_SYNC);
	/*stp max-age*/
	install_element (CONFIG_NODE, &stp_max_hop_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_stp_max_hop_cmd, CMD_SYNC);

	
	
	/*---------------------instance config cmd----------------------*/

	/*instance config*/
	install_element (CONFIG_NODE, &stp_instance_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_stp_instance_cmd, CMD_SYNC);
	/*vlan config*/
	install_element (MSTP_NODE, &stp_vlan_cmd, CMD_SYNC);
	install_element (MSTP_NODE, &no_stp_vlan_cmd, CMD_SYNC);
	/*vlan config*/
	install_element (MSTP_NODE, &stp_msti_priority_cmd, CMD_SYNC);
	install_element (MSTP_NODE, &no_stp_msti_priority_cmd, CMD_SYNC);
	
	
	
	/*---------------------interface config cmd----------------------*/
	/*physical interface node cmd*/

	/*stp enable*/
	install_element (PHYSICAL_IF_NODE, &interface_stp_enable_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_interface_stp_enable_cmd, CMD_SYNC);
	/*stp priority*/
	install_element (PHYSICAL_IF_NODE, &interface_stp_priority_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_interface_stp_priority_cmd, CMD_SYNC);	
	/*stp cost*/
	install_element (PHYSICAL_IF_NODE, &interface_stp_cost_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_interface_stp_cost_cmd, CMD_SYNC);	
	/*stp instance<1-8> priority*/
	install_element (PHYSICAL_IF_NODE, &interface_stp_instance_priority_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_interface_stp_instance_priority_cmd, CMD_SYNC);	
	/*stp instance<1-8> cost*/
	install_element (PHYSICAL_IF_NODE, &interface_stp_instance_cost_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_interface_stp_instance_cost_cmd, CMD_SYNC);	
	/*stp edge*/
	install_element (PHYSICAL_IF_NODE, &interface_stp_edge_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_interface_stp_edge_cmd, CMD_SYNC);	
	/*stp filter_port*/
	install_element (PHYSICAL_IF_NODE, &interface_stp_filter_port_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_interface_stp_filter_port_cmd, CMD_SYNC);		
	/*stp p2p_port*/
	install_element (PHYSICAL_IF_NODE, &interface_stp_p2p_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &no_interface_stp_p2p_cmd, CMD_SYNC);		
	
	
	/*trunk interface node cmd*/
	/*stp enable*/
	install_element (TRUNK_IF_NODE, &interface_stp_enable_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &no_interface_stp_enable_cmd, CMD_SYNC);
	/*stp priority*/
	install_element (TRUNK_IF_NODE, &interface_stp_priority_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &no_interface_stp_priority_cmd, CMD_SYNC);	
	/*stp cost*/
	install_element (TRUNK_IF_NODE, &interface_stp_cost_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &no_interface_stp_cost_cmd, CMD_SYNC);	
	/*stp instance<1-8> priority*/
	install_element (TRUNK_IF_NODE, &interface_stp_instance_priority_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &no_interface_stp_instance_priority_cmd, CMD_SYNC);	
	/*stp instance<1-8> cost*/
	install_element (TRUNK_IF_NODE, &interface_stp_instance_cost_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &no_interface_stp_instance_cost_cmd, CMD_SYNC);	
	/*stp edge*/
	install_element (TRUNK_IF_NODE, &interface_stp_edge_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &no_interface_stp_edge_cmd, CMD_SYNC);	
	/*stp filter_port*/
	install_element (TRUNK_IF_NODE, &interface_stp_filter_port_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &no_interface_stp_filter_port_cmd, CMD_SYNC);		
	/*stp p2p_port*/
	install_element (TRUNK_IF_NODE, &interface_stp_p2p_cmd, CMD_SYNC);
	install_element (TRUNK_IF_NODE, &no_interface_stp_p2p_cmd, CMD_SYNC);		

	/*-------------------------show cmd-------------------------------*/

	/*config mode*/
	install_element (CONFIG_NODE, &show_stp_interface_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_stp_instance_cmd,CMD_LOCAL);			

	/*instance mode*/
	install_element (MSTP_NODE, &show_stp_interface_cmd,CMD_LOCAL);
	install_element (MSTP_NODE, &show_stp_instance_cmd,CMD_LOCAL);

	/*interface mode*/
	install_element (PHYSICAL_IF_NODE, &show_stp_interface_cmd,CMD_LOCAL);
	install_element (PHYSICAL_IF_NODE, &show_stp_instance_cmd,CMD_LOCAL);		
	install_element (TRUNK_IF_NODE, &show_stp_interface_cmd,CMD_LOCAL);
	install_element (TRUNK_IF_NODE, &show_stp_instance_cmd,CMD_LOCAL);		

	/*mstp debug cmd*/
	install_element (CONFIG_NODE, &debug_stp_running_info_cmd,CMD_LOCAL);

	return;
}


