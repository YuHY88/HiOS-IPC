/**
 * @file      : cfm_cli.c
 * @brief     : define cfm command line
 * @details   : 
 * @author    : huoqq
 * @date      : 2018Äê3ÔÂ23ÈÕ 14:55:34
 * @version   : 
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      : 
 */

#include <string.h>
#include <stdlib.h>
#include <lib/linklist.h>
#include <lib/prefix.h>
#include <lib/vty.h>
#include <lib/ifm_common.h>
#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/command.h>
#include "lib/msg_ipc.h"
#include <lib/errcode.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/devm_com.h>

#include <lib/alarm.h>
#include <lib/log.h>

#include "cfm_session.h"
#include "l2_if.h"

#include "cfm.h"

#include "cfm_lb.h"
#include "cfm_lt.h"
#include "cfm_test.h"
#include "cfm_cli.h"

#include "../vlan.h"
#include "aps/elps_pkt.h"
#include "aps/elps.h"

extern int cfm_device_id; //2200:0x05F0

const struct message cfm_dbg_name[] = {
         {.key = CFM_DBG_LB, 	.str = "lb"},
         {.key = CFM_DBG_LT,    .str = "lt"},
         {.key = CFM_DBG_TEST,  .str = "test"},
         {.key = CFM_DBG_COMMN, .str = "common"},
         {.key = CFM_DBG_ALL,   .str = "all"},
};


struct cmd_node cfm_md_node =
{
  CFM_MD_NODE,
  "%s(config-md)# ",
  1,
};

struct cmd_node cfm_ma_node =
{
  CFM_MA_NODE,
  "%s(config-ma)# ",
  1,
};

struct cmd_node cfm_session_node =
{
  CFM_SESSION_NODE,
  "%s(config-session)# ",
  1,
};

DEFUN(md_view,
		md_view_cmd,
		"cfm md  <1-8>",
		"Connectivity fault management\n"
		"Maintenance domain\n"
		"Md index,<1-8>\n")
{
	struct cfm_md  *pmd=NULL;
	uint8_t			md_index;
	char 		   *pprompt = NULL;

	md_index = atoi(argv[0]);
	pmd 	 = cfm_md_lookup(md_index);

	if(!pmd)
	{
		pmd = cfm_md_create(md_index);
		if(!pmd)
		{
			vty_error_out(vty,"Can not create md %d %s",md_index,VTY_NEWLINE); 	
			return CMD_WARNING; 	
		}
		
		vty_info_out(vty,"Md %d create success %s",md_index, VTY_NEWLINE);	

	}

	vty->index = (void *)pmd;
	vty->node  = CFM_MD_NODE;
    pprompt    = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-md-%d)# ", md_index);
	}
	
	return CMD_SUCCESS;
}

DEFUN(no_md_view,
		no_md_view_cmd,
		"no cfm md  <1-8>",
		NO_STR
		"Connectivity fault management\n"
		"Maintenance domain\n"
		"Md index,<1-8>\n")
{
	struct cfm_md *pmd=NULL;
	uint8_t		   md_index;

	md_index = atoi(argv[0]);
	pmd 	 = cfm_md_lookup(md_index);

	if(!pmd)
	{
		vty_error_out(vty,"Md %d not exist%s",md_index,VTY_NEWLINE); 	
		return CMD_WARNING;		
	}

	if(pmd->elps_bind)
	{
		vty_error_out(vty,"Please check elps binding%s",VTY_NEWLINE);		
		return CMD_WARNING;		
	}

	if(pmd->name[0])
	{
		//ipc_send_hal( pmd, sizeof(struct cfm_md), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MD, IPC_OPCODE_DELETE, pmd->md_index);
		cfm_send_hal(pmd, sizeof(struct cfm_md), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MD, IPC_OPCODE_DELETE, pmd->md_index);
	}

	cfm_md_delete(md_index);
	
	return CMD_SUCCESS;
}

DEFUN(md_name,
		md_name_cmd,
		"name  NAME",
		"Md name\n"
		"Md name--max.31\n")
{
	struct cfm_md *pmd=NULL;
	
	pmd = vty->index;

	if(pmd->ma_list.count)
	{
		vty_error_out(vty,"Please delete all MAs first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	
	if(!strcmp(pmd->name,argv[0]))
	{
		vty_error_out(vty,"Same to the old name%s",VTY_NEWLINE);
		return CMD_WARNING;
	}
	
    memcpy(pmd->name, argv[0], strlen(argv[0])+1);

	
	//ipc_send_hal( pmd, sizeof(struct cfm_md), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MD, IPC_OPCODE_ADD, pmd->md_index);

	cfm_send_hal(pmd, sizeof(struct cfm_md), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MD, IPC_OPCODE_ADD, pmd->md_index);
	
	vty_info_out(vty,"Set md name %s success %s",pmd->name, VTY_NEWLINE);

	return CMD_SUCCESS;
}


DEFUN(md_level,
		md_level_cmd,
		"level  <0-7>",
		"Maintenance domain level\n"
		"Md level ,<0-7>\n")
{
	struct cfm_md *pmd=NULL;
	uint8_t 	   md_level;

	md_level = atoi(argv[0]);
	pmd 	 = vty->index;

	if(pmd->ma_list.count)
	{
		vty_error_out(vty,"Please delete all MAs first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(pmd->level == md_level)
	{
		vty_error_out(vty,"The level not change %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*for(i=0 ; i<CFM_MD_NUM_MAX ; i++)
	{
		if(cfm_md_table[i] != NULL && cfm_md_table[i]->level == md_level)
		{				
			vty_error_out(vty, " The level has been used in md %d %s",cfm_md_table[i]->md_index ,VTY_NEWLINE);
			return CMD_WARNING;			
		}
	}*/
	
	
	pmd->level = md_level;
	vty_info_out(vty,"Set md level %d success %s",md_level, VTY_NEWLINE);

	if(pmd->name[0])
	{
		//ipc_send_hal( pmd, sizeof(struct cfm_md), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MD, IPC_OPCODE_ADD, pmd->md_index);
		cfm_send_hal(pmd, sizeof(struct cfm_md), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MD, IPC_OPCODE_ADD, pmd->md_index);
	}

	return CMD_SUCCESS;
}

DEFUN(no_md_level,
		no_md_level_cmd,
		"no level",
		NO_STR		
		"Maintenance domain level\n")
{
	struct cfm_md *pmd=NULL;

	pmd = vty->index;

	if(pmd->ma_list.count)
	{
		vty_error_out(vty,"Please delete all MAs first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(pmd->level)
	{
		pmd->level = 0;
		vty_info_out(vty,"Md level restored to 0 %s",VTY_NEWLINE);

		if(pmd->name[0])
		{
			//ipc_send_hal( pmd, sizeof(struct cfm_md), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MD, IPC_OPCODE_ADD, pmd->md_index);
			cfm_send_hal(pmd, sizeof(struct cfm_md), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MD, IPC_OPCODE_ADD, pmd->md_index);
		}		
	}

	return CMD_SUCCESS;
}
//-----------------------------MA------------------------------------------
DEFUN(ma_view,
		ma_view_cmd,
		"ma  <1-1024>",
		"Maintenance association\n"
		"Ma index ,<1-1024>\n")
{
	struct cfm_md *pmd = NULL;
	struct cfm_ma *pma = NULL;
	uint16_t 	   ma_index;
	char 		  *pprompt = NULL;


	pmd= (struct cfm_md *)vty->index;

	if(!pmd->name[0])
	{
		vty_error_out(vty,"Please set md name first %s",VTY_NEWLINE); 	
		return CMD_WARNING; 	
	}
	
	ma_index = atoi(argv[0]);
	pma 	 = cfm_ma_lookup(ma_index);

	if(!pma)
	{
		if(cfm_device_id != ID_HT2200 && cfm_device_id != ID_HT2200V2 && cfm_device_id != ID_HT2100V2 && cfm_device_id != ID_H9MOLMXE_VX)
		{			
			if(ma_index>512)
			{
				vty_error_out(vty,"ma index can not be greater than 512%s",VTY_NEWLINE);	
				return CMD_WARNING; 	
			}			
		}
		
		pma = cfm_ma_create(ma_index);
		if(!pma)
		{
			vty_error_out(vty,"Can not create ma %d %s",ma_index,VTY_NEWLINE); 	
			return CMD_WARNING; 	
		}
		
		vty_info_out(vty,"Ma %d create success in md %d %s",ma_index, pmd->md_index,VTY_NEWLINE);
		
		cfm_md_add_ma(pmd,pma); 
	}
	else if(pma->md_index != pmd->md_index)
	{
		vty_error_out(vty,"The ma does not blong to this md%s",VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	vty->index_sub 	=  (void *)pma;
	vty->node 	   	=  CFM_MA_NODE;	
    pprompt 		=  vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-md-%d-ma-%d)# ", pmd->md_index,ma_index);
	}
	
	return CMD_SUCCESS;
}

DEFUN(no_ma_view,
		no_ma_view_cmd,
		"no ma  <1-1024>",
		NO_STR
		"Maintenance association\n"
		"Ma index ,<1-1024>\n")
{
	struct cfm_md *pmd=NULL;
	struct cfm_ma *pma=NULL;
	uint16_t 	   ma_index;

	pmd= (struct cfm_md *)vty->index;

	ma_index 	= atoi(argv[0]);
	pma 		= cfm_ma_lookup(ma_index);

	if(!pma || pma->md_index!=pmd->md_index)
	{
		vty_error_out(vty,"Ma %d not exist%s",ma_index,VTY_NEWLINE); 	
		return CMD_WARNING;		
	}

	if(pma->elps_bind)
	{
		vty_error_out(vty,"Please check elps binding%s",VTY_NEWLINE);		
		return CMD_WARNING;		
	}

	
	cfm_md_delete_ma(pmd,pma);
	

	vty_info_out(vty,"Ma %d delete success in md %d %s",ma_index, pmd->md_index,VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN(ma_name ,
		ma_name_cmd,
		"name  NAME",
		"Ma name\n"
		"Ma name--max.31\n")
{
	struct cfm_ma *pma=NULL;
	struct cfm_md *pmd=NULL;

	pma = (struct cfm_ma *)vty->index_sub;

	pmd = cfm_md_lookup(pma->md_index);
	if(pmd == NULL)
	{
		vty_error_out(vty,"Pmd is NULL %s",VTY_NEWLINE);
		return CMD_WARNING;
	}


	if(pma->sess_list.count)
	{
		vty_error_out(vty,"Please delete all sessions first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}	

	if(!strcmp(pma->name,argv[0]))
	{
		vty_error_out(vty,"Same to the old name%s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(pma->name[0])
	{
		vty_error_out(vty,"Please delete the old name first%s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if((strlen(argv[0])+strlen(pmd->name))> 44)
	{
		vty_error_out(vty,"(length md + length ma) over max  length 44%s",VTY_NEWLINE);
		return CMD_WARNING;
	}
	
    memcpy(pma->name, argv[0], strlen(argv[0])+1);

	if(pma->vlan)
	{
		//ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
		cfm_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
	}

	vty_info_out(vty,"Set ma name %s success %s",pma->name, VTY_NEWLINE);
	
	return CMD_SUCCESS;
}

DEFUN(no_ma_name ,
		no_ma_name_cmd,
		"no name",
		NO_STR
		"Ma name\n")
{
	struct cfm_ma *pma=NULL;

	pma = (struct cfm_ma *)vty->index_sub;

	if(pma->sess_list.count)
	{
		vty_error_out(vty,"Please delete all sessions first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(pma->mip_enable)
	{
		vty_error_out(vty,"Please disable mip first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(!pma->name[0])
	{
		vty_error_out(vty,"The ma has no name%s",VTY_NEWLINE);
		return CMD_WARNING;
	}
		
    memset(pma->name, 0, NAME_STRING_LEN);

	if(pma->vlan)
	{
		//ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_DELETE, pma->ma_index);
		cfm_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_DELETE, pma->ma_index);
	}

	vty_info_out(vty,"Delete ma name success %s", VTY_NEWLINE);
	
	return CMD_SUCCESS;
}

DEFUN(ma_service_vlan,
		ma_service_vlan_cmd,
		"service vlan <1-4094>",
		"Service\n"
		"vlan\n"		
		"Vlan index ,<1-4094>\n")
{
	struct cfm_ma 	   *pma = NULL;
	uint16_t	   		vlan_index;
    //struct hash_bucket *pbucket = NULL;
    //int 				cursor;
	//struct cfm_ma	   *pma1 = NULL;

	vlan_index 	= atoi(argv[0]);
	pma			= (struct cfm_ma *)vty->index_sub;	


	if(pma->sess_list.count)
	{
		vty_error_out(vty,"Please delete all sessions first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(pma->vlan)
	{
		vty_error_out(vty,"Please delete vlan first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*HASH_BUCKET_LOOP(pbucket,cursor,cfm_ma_table)
	{
	
		pma1 = pbucket->data;
		
		if(pma1->md_index == pma->md_index && pma1->vlan == vlan_index)
		{
			vty_error_out(vty,"Vlan has been used in md %d %s",pma->md_index,VTY_NEWLINE);
			return CMD_WARNING;
		}
	}*/

	pma->vlan = vlan_index;
	vty_info_out(vty,"Ma map vlan %d success %s",vlan_index, VTY_NEWLINE);

	if(pma->name[0])
	{
		//ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
		cfm_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
	}
	
	return CMD_SUCCESS;
}

DEFUN(no_ma_service_vlan,
		no_ma_service_vlan_cmd,
		"no service vlan",
		NO_STR
		"Service\n"
		"Vlan\n")
{
	struct cfm_ma *pma=NULL;

	pma = (struct cfm_ma *)vty->index_sub;

	if(pma->sess_list.count)
	{
		vty_error_out(vty,"Please delete all sessions first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(pma->vlan == 0)
	{
		vty_error_out(vty,"Vlan has been default value 0 %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(pma->mip_enable)
	{
		vty_error_out(vty,"Please disable mip first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	pma->vlan = 0;

	if(pma->name[0])
	{
		//ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_DELETE, pma->ma_index);
		cfm_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_DELETE, pma->ma_index);
	}

	vty_info_out(vty,"Ma map vlan default value 0 %s", VTY_NEWLINE);
	
	return CMD_SUCCESS;
}

DEFUN(ma_priority ,
		ma_priority_cmd,
		"priority  <0-7>",
		"Packet cos priority\n"
		"Priority index ,<0-7>\n")
{
	struct cfm_ma *pma=NULL;
	uint16_t 	   priority;

	pma = (struct cfm_ma *)vty->index_sub;

	if(pma->sess_list.count)
	{
		vty_error_out(vty,"Please delete all sessions first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	priority = atoi(argv[0]);

	if(pma->priority != priority)
	{
		pma->priority = priority;

		if(pma->vlan && pma->name[0])
		{
			//ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
			cfm_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
		}
	}

	vty_info_out(vty,"Set ma priority %d success %s",priority, VTY_NEWLINE);
	
	return CMD_SUCCESS;
}

DEFUN(no_ma_priority ,
		no_ma_priority_cmd,
		"no priority",
		NO_STR
		"Packet cos priority\n")
{
	struct cfm_ma *pma=NULL;

	pma = (struct cfm_ma *)vty->index_sub;

	if(pma->sess_list.count)
	{
		vty_error_out(vty,"Please delete all sessions first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}


	if(pma->priority != 5)
	{
		pma->priority = 5;
		
		if(pma->vlan && pma->name[0])
		{
			//ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
			cfm_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
		}
	}

	vty_info_out(vty,"Ma priority restored to 5 %s", VTY_NEWLINE);
	
	return CMD_SUCCESS;
}


DEFUN(ma_mip_enable ,
		ma_mip_enable_cmd,
		"mip enable",
		"Maintenance association Intermediate Point\n"
		"Enable\n")
{
	struct cfm_ma *pma=NULL;

	pma = (struct cfm_ma *)vty->index_sub;

	if(pma->sess_list.count)
	{
		vty_error_out(vty,"Please delete all sessions first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(pma->mip_enable)
	{
		vty_error_out(vty,"Mip has been enabled %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(!pma->name[0])
	{
		vty_error_out(vty,"the ma does not config name%s",VTY_NEWLINE);
		return CMD_WARNING; 			
	}

	if(!pma->vlan)
	{
		vty_error_out(vty,"Please config ma vlan  first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	pma->mip_enable = 1;

	//ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
	cfm_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
	
	vty_info_out(vty,"Set mip enable success %s",VTY_NEWLINE);
	
	return CMD_SUCCESS;
}



DEFUN(no_ma_mip_enable ,
		no_ma_mip_enable_cmd,
		"no mip enable",
		NO_STR
		"Maintenance association Intermediate Point\n"
		"Enable\n")
{
	struct cfm_ma *pma=NULL;

	pma = (struct cfm_ma *)vty->index_sub;


	if(!pma->mip_enable)
	{
		vty_error_out(vty,"Mip has been disabled %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	pma->mip_enable = 0;

	//ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
	cfm_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
	
	vty_info_out(vty,"Set mip disable success %s",VTY_NEWLINE);
	
	return CMD_SUCCESS;
}

DEFUN(ma_1731_enable ,
		ma_1731_enable_cmd,
		"protocol y.1731",
		"protocol\n"
		"y.1731\n")
{
	struct cfm_ma *pma=NULL;

	pma = (struct cfm_ma *)vty->index_sub;

	if(pma->sess_list.count)
	{
		vty_error_out(vty, "Please delete all sessions first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(pma->mip_enable)
	{
		vty_error_out(vty,"Please disable mip first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(pma->y1731_en)
	{
		vty_error_out(vty,"Y.1731 has been enabled %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	pma->y1731_en = 1;

	if(pma->vlan && pma->name[0])
	{
		//ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_DELETE, pma->ma_index);	
		//ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
		cfm_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_DELETE, pma->ma_index);
		cfm_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
	}

	vty_info_out(vty,"Enable y.1731 success %s",VTY_NEWLINE);
	
	return CMD_SUCCESS;
}

DEFUN(no_ma_1731_enable ,
		no_ma_1731_enable_cmd,
		"no protocol y.1731",
		NO_STR
		"protocol\n"
		"y.1731\n")
{
	struct cfm_ma *pma=NULL;

	pma = (struct cfm_ma *)vty->index_sub;

	if(pma->sess_list.count)
	{
		vty_error_out(vty,"Please delete all sessions first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(pma->mip_enable)
	{
		vty_error_out(vty,"Please disable mip first %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(!pma->y1731_en)
	{
		vty_error_out(vty,"Y.1731 not enabled %s",VTY_NEWLINE);
		return CMD_WARNING;
	}

	pma->y1731_en = 0;

	if(pma->vlan && pma->name[0])
	{
		//ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_DELETE, pma->ma_index);	
		//ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
		cfm_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_DELETE, pma->ma_index);
		cfm_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_ADD, pma->ma_index);
	}

	vty_info_out(vty,"Disable y.1731 success %s",VTY_NEWLINE);
	
	return CMD_SUCCESS;
}


//--------------------------SESSION-----------------------------------------
DEFUN(session_name,
		session_name_cmd,
		"cfm session  <1-1024>",
		"Connectivity fault management\n"
		"Session\n"
		"Session index ,<1-1024>\n")
{
	struct cfm_sess *psess = NULL;
	uint16_t		 sess_index;
	int 			 ret;
	char 			*pprompt = NULL;


	sess_index = atoi(argv[0]);
	psess = cfm_session_lookup_new(sess_index);

	if(!psess)
	{
		psess = cfm_session_create(sess_index);
		if(!psess)
		{
			vty_error_out(vty,"Can not create cfm session %d %s",sess_index,VTY_NEWLINE);	
			return CMD_WARNING; 	
		}
		ret = cfm_session_add_new(psess);
		if(ret != ERRNO_SUCCESS)
		{
			XFREE(MTYPE_CFM_SESS,psess);
			
			if(ret == ERRNO_OVERSIZE)
				vty_error_out(vty,"Cfm session num oversize %s",VTY_NEWLINE);	
			
			return CMD_WARNING; 	
		}
		
		vty_info_out(vty,"Cfm session %d create success %s",sess_index,VTY_NEWLINE);
		
	}
	
	vty->index	= (void *)psess; 
	vty->node 	= CFM_SESSION_NODE;	
    pprompt 	= vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-cfm-session-%d)# ", sess_index);
	}
	
	return CMD_SUCCESS;
}

DEFUN(no_session_name,
		no_session_name_cmd,
		"no cfm session  <1-1024>",
		NO_STR
		"Connectivity fault management\n"
		"Session\n"
		"Session index ,<1-1024>\n")
{
	struct cfm_sess *psess = NULL;
	struct cfm_ma   *pma = NULL;	
	uint16_t		 sess_index;
	int 			 ret;

	sess_index 	= atoi(argv[0]);
	psess 		= cfm_session_lookup_new(sess_index);

	if(!psess)
	{
		vty_error_out(vty,"Cfm session %d not exist%s",sess_index,VTY_NEWLINE); 	
		return CMD_SUCCESS;		
	}


	if(psess->elps_bind)
	{
		vty_error_out(vty,"Please check elps binding or erps binding%s",VTY_NEWLINE);		
		return CMD_WARNING;		
	}

	pma = cfm_ma_lookup(psess->ma_index);

	//ipc_send_hal( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, 0);
	if(psess->ifindex)
	{
		//ret = ipc_send_hal_wait_ack( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, 0);
		ret = cfm_ipc_send_wait_ack(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, 0);
		if(ret != 0)
		{
			vty_error_out(vty,"Set error %s", VTY_NEWLINE); 	
			return CMD_WARNING; 	
		}
	}

	if(psess->ma_index && psess->local_mep)
		cfm_session_delete((psess->ma_index<<16)|psess->local_mep);	

	if(psess->ma_index)
		cfm_ma_delete_sess(pma,psess);
	
	cfm_session_delete_new(sess_index);//del sess  in sesslist first	
	
	//cfm_session_delete(sess_index);

	vty_info_out(vty,"Cfm session %d delete success %s",sess_index,VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN(session_service_ma,
		session_service_ma_cmd,
		"service ma <1-1024>",
		"Service\n"
		"Maintenance association\n"		
		"Ma index ,<1-1024>\n")
{
	struct cfm_sess *psess = NULL;
	struct cfm_ma 	*pma   = NULL;
	struct cfm_md 	*pmd   = NULL;	
	uint16_t		 ma_index;
		
	psess 	 = (struct cfm_sess *)vty->index;

	ma_index = atoi(argv[0]);

	if(psess->ifindex)
	{
		vty_error_out(vty,"Please delete cfm interface first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	if(psess->ma_index)
	{
		vty_error_out(vty,"Please delete ma in session first%s",VTY_NEWLINE);
		return CMD_WARNING;		
	}

	pma= cfm_ma_lookup(ma_index);

	if(!pma)
	{
		vty_error_out(vty,"Ma %d not exist,please config ma first%s",ma_index,VTY_NEWLINE);		
		return CMD_WARNING;		
	}

	
	//new map
	if(!pma->vlan)
	{
		vty_error_out(vty,"The ma does not config vlan%s",VTY_NEWLINE);
		return CMD_WARNING; 			
	}

	if(!pma->name[0])
	{
		vty_error_out(vty,"the ma does not config name%s",VTY_NEWLINE);
		return CMD_WARNING; 			
	}

	if(pma->mip_enable)
	{
		vty_error_out(vty,"The mip has been enabled in the ma,can not bind%s",VTY_NEWLINE);
		return CMD_WARNING; 			
	}

	if(pma->sess_list.count)
	{
		vty_error_out(vty,"The ma has include a session already%s",VTY_NEWLINE);
		return CMD_WARNING; 			
	}

	
	psess->ma_index = ma_index;
	psess->vlan 	= pma->vlan;
	
	psess->md_index = pma->md_index;			
	pmd 			= cfm_md_lookup(psess->md_index);
	psess->level	= pmd->level;

	cfm_ma_add_sess(pma,psess);

	if(psess->local_mep)
		cfm_session_add(psess);

	pma->elps_bind+=psess->elps_bind;
	pmd->elps_bind+=psess->elps_bind;
	zlog_debug(CFM_DBG_COMMN, "%s,pmd->elps_bind:%d \n",__FUNCTION__,pmd->elps_bind);			
	zlog_debug(CFM_DBG_COMMN, "%s,pma->elps_bind:%d \n",__FUNCTION__,pma->elps_bind);					
	
	vty_info_out(vty,"Session %d map ma %d success %s",psess->sess_id ,ma_index, VTY_NEWLINE);			
	
	
	return CMD_SUCCESS;
}

DEFUN(no_session_service_ma,
		no_session_service_ma_cmd,
		"no service ma",
		NO_STR
		"Service\n"
		"Maintenance association\n")		
{
	struct cfm_sess *psess = NULL;
	struct cfm_ma   *pma   = NULL;
	struct cfm_md   *pmd   = NULL;
	
	psess= (struct cfm_sess *)vty->index;
	
	if(psess->ifindex)
	{
		vty_error_out(vty,"Please delete cfm interface first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	if(!psess->ma_index)
	{
		vty_error_out(vty,"The session does not bind ma%s",VTY_NEWLINE);
		return CMD_WARNING;		
	}

	pma = cfm_ma_lookup(psess->ma_index);
	if(!pma)//this may not happen forever
	{
		vty_error_out(vty,"Ma %d not exist%s",psess->ma_index,VTY_NEWLINE);
		return CMD_WARNING;		
	}
	pmd = cfm_md_lookup(psess->md_index);
	if(!pmd)//this may not happen forever
	{
		vty_error_out(vty,"Md %d not exist%s",psess->md_index,VTY_NEWLINE);
		return CMD_WARNING;		
	}
	
	cfm_ma_delete_sess(pma,psess);

	if(psess->local_mep)
		cfm_session_delete((psess->ma_index<<16)|psess->local_mep);
	
	psess->ma_index 	= 0;
	psess->vlan 		= 0;	
	psess->md_index 	= 0;			
	psess->level 		= 0;


	pma->elps_bind-=psess->elps_bind;
	pmd->elps_bind-=psess->elps_bind;
	zlog_debug(CFM_DBG_COMMN, "%s,pmd->elps_bind:%d \n",__FUNCTION__,pmd->elps_bind);			
	zlog_debug(CFM_DBG_COMMN, "%s,pma->elps_bind:%d \n",__FUNCTION__,pma->elps_bind);					
	
	vty_info_out(vty,"Delete ma success in session %d%s",psess->sess_id,VTY_NEWLINE);
	
	return CMD_SUCCESS;
}
DEFUN(session_direction_down,
		session_direction_down_cmd,
		"direction down",
		"Direction\n"
		"Down:outward\n")
{
	struct cfm_sess *psess=NULL;
	
	psess = (struct cfm_sess *)vty->index;

	if(psess->ifindex)
	{
		vty_error_out(vty,"Please delete cfm interface first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}	

	psess->direct  = MEP_DIRECT_DOWN;
	psess->over_pw = 0;

	
	vty_info_out(vty,"Set session direction down success %s", VTY_NEWLINE);
	
	return CMD_SUCCESS;
}

DEFUN(session_direction_up,
		session_direction_up_cmd,
		"direction up  {over_pw}",
		"Direction\n"
		"Up:inward \n" 
		"Over pseudo wire\n")
{
	struct cfm_sess *psess	 = NULL;
	enum MEP_DIRECT  direct  = MEP_DIRECT_UP;
	uchar 			 over_pw = 0;
	
	psess= (struct cfm_sess *)vty->index;

	if(psess->ifindex)
	{
		vty_error_out(vty,"Please delete cfm interface first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	if(argv[0])
		over_pw = 1;
	
	psess->direct  = direct;
	psess->over_pw = over_pw;

	
	vty_info_out(vty,"Set session direction up %ssuccess %s",over_pw?"over pw ":"", VTY_NEWLINE);
	
	return CMD_SUCCESS;
}


DEFUN(no_session_direction,
		no_session_direction_cmd,
		"no direction",
		NO_STR
		"Direction\n")
{
	struct cfm_sess *psess = NULL;

	psess = (struct cfm_sess *)vty->index;

	if(psess->ifindex)
	{
		vty_error_out(vty,"Please delete cfm interface first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	/*if(psess->direct == MEP_DIRECT_DOWN)
	{
		vty_error_out(vty, "the session direction is already DOWN %s", VTY_NEWLINE);
		
		return CMD_WARNING; 		
	}*/
	
	psess->direct  = MEP_DIRECT_DOWN;
	psess->over_pw = 0;

	
	vty_info_out(vty,"Set session direction down success %s", VTY_NEWLINE);
	
	return CMD_SUCCESS;
}



DEFUN(session_localmep,
		session_localmep_cmd,
		"local-mep <1-8191>",
		"Local maintenance association end point\n"
		"Local-mep index\n")
{
	struct cfm_sess *psess = NULL;
	uint16_t         local_index;
		
	psess		= (struct cfm_sess *)vty->index;

	local_index = atoi(argv[0]);

	if(psess->ifindex)
	{
		vty_error_out(vty,"Please delete cfm interface first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	if(psess->local_mep)
	{
		vty_error_out(vty,"Please delete old value first%s",VTY_NEWLINE);
		return CMD_WARNING; 		
	}

	if(psess->remote_mep == local_index)
	{
		vty_error_out(vty,"The local-mep id is equal to remote-mep id%s",VTY_NEWLINE);
		return CMD_WARNING; 		
	}


	psess->local_mep = local_index;

	if(psess->ma_index)
		cfm_session_add(psess);

	vty_info_out(vty,"In session %d ,set local-mep id  %d success %s",psess->sess_id ,local_index,
		VTY_NEWLINE);

	
	return CMD_SUCCESS;
}




DEFUN(no_session_localmep,
		no_session_localmep_cmd,
		"no local-mep",
		NO_STR
		"Local maintenance association end point\n")
{
	struct cfm_sess *psess=NULL;
		
	psess = (struct cfm_sess *)vty->index;


	if(psess->ifindex)
	{
		vty_error_out(vty,"Please delete cfm interface first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	if(!psess->local_mep)
	{
		vty_info_out(vty,"In session %d , no local-mep config%s",psess->sess_id, VTY_NEWLINE);
		return CMD_WARNING; 		
	}
	
	if(psess->ma_index)
		cfm_session_delete((psess->ma_index<<16)|psess->local_mep);

	psess->local_mep = 0;


	vty_info_out(vty,"In session %d ,delete local-mep id success %s",psess->sess_id ,	VTY_NEWLINE);

	
	return CMD_SUCCESS;
}


DEFUN(session_rmep,
		session_rmep_cmd,
		"remote-mep <1-8191> {mac XX:XX:XX:XX:XX:XX}",
		"Remote maintenance association end point\n"
		"Remote-mep index\n"
		"Mac address\n"
		"Mac address\n")
{
	struct cfm_sess *psess = NULL;
	uint8_t 		 mac_addr[6];
	uint16_t 		 rmep_index;
		
	psess 		= (struct cfm_sess *)vty->index;

	rmep_index 	= atoi(argv[0]);

	if(psess->ifindex)
	{
		vty_error_out(vty,"Please delete cfm interface first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	if(psess->remote_mep)
	{
		vty_error_out(vty,"Please delete old value first%s",VTY_NEWLINE);
		return CMD_WARNING; 		
	}

	if(psess->local_mep == rmep_index)
	{
		vty_error_out(vty,"The remote-mep id is equal to local-mep id%s",VTY_NEWLINE);
		return CMD_WARNING; 		
	}


	//check mac
	if(argv[1])
	{
		if(ether_valid_mac(argv[1]))
		{
			vty_error_out(vty,"Invalid mac value%s",VTY_NEWLINE);
			return CMD_WARNING; 		
		}
		ether_string_to_mac(argv[1],mac_addr);
		
		if(mac_addr[0]&0x01)
		{
			vty_error_out(vty,"Not support multicast and broadcast MAC address%s",VTY_NEWLINE);
			return CMD_WARNING; 		
		}

		psess->dmac_valid = 1;
	}
	else
	{
		memset(mac_addr,0,6);
		psess->dmac_valid = 0;
	}

	psess->remote_mep = rmep_index;
	memcpy(psess->dmac,mac_addr,6);

	//cfm_send_session_to_hal(psess,1,1);

	if(psess->dmac_valid )
	{
		vty_info_out(vty,"In session %d ,set remote-mep id %d [%02X:%02X:%02X:%02X:%02X:%02X] success %s",psess->sess_id ,rmep_index,
			mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5],VTY_NEWLINE);
	}
	else
	{
		vty_info_out(vty,"In session %d ,set remote-mep id %d success %s",psess->sess_id ,rmep_index,
			VTY_NEWLINE);
	}

	
	return CMD_SUCCESS;
}

DEFUN(no_session_rmep,
		no_session_rmep_cmd,
		"no remote-mep",
		NO_STR
		"Remote maintenance association end point\n")
{
	struct cfm_sess *psess=NULL;
	uint8_t 		*mac_addr;

	psess = (struct cfm_sess *)vty->index;

	if(psess->ifindex)
	{
		vty_error_out(vty,"Please delete cfm interface first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}
	
	if(!psess->remote_mep)
	{		
		vty_info_out(vty,"In session %d , no remote-mep config%s",psess->sess_id, VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	if(psess->remote_mep)
	{
		mac_addr = psess->dmac;
			
		vty_info_out(vty,"In session %d , delete remote-mep id %d  [%02X:%02X:%02X:%02X:%02X:%02X] success %s",psess->sess_id,psess->remote_mep,
			mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5],VTY_NEWLINE);
		psess->remote_mep = 0;
		psess->dmac_valid = 0;		
		memset(psess->dmac,0,6);
		//cfm_send_session_to_hal(psess,1,0);		
	}
	
	return CMD_SUCCESS;
}

DEFUN(session_ccm_en,
		session_ccm_en_cmd,
		" cc enable (3ms | 10ms | 100ms | 1s | 10s | 60s | 600s)",
		"Continuity check message\n"
		"Enable\n"
		"Interval value 3ms\n"
		"Interval value 10ms\n"
		"Interval value 100ms\n"
		"Interval value 1s\n"
		"Interval value 10s\n"
		"Interval value 60s\n"
		"Interval value 600s\n")
{
	struct cfm_sess *psess       = NULL;
	uint32_t         cc_interval = 0;	

	psess = (struct cfm_sess *)vty->index;

    if(psess->state == OAM_STATUS_DISABLE)
    {   
        vty_error_out(vty,"CFM-session is not enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }    

	if(psess->cc_enable)
	{
		vty_error_out(vty,"Ccm is already enabled %s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}
	
	if(!strncmp(argv[0],"3m",2))
		cc_interval = 3;	
	else if(!strncmp(argv[0],"1s",2))
		cc_interval = 1000;
	else if(!strncmp(argv[0],"10m",3))
		cc_interval = 10;
	else if(!strncmp(argv[0],"100",3))
		cc_interval = 100;
	else if(!strncmp(argv[0],"10s",3))
		cc_interval = 10000;
	else if(!strncmp(argv[0],"60s",3))
		cc_interval = 60000;
	else if(!strncmp(argv[0],"600",3))
		cc_interval = 600000;

	psess->cc_interval = cc_interval;		
	psess->cc_enable   = 1;
	vty_info_out(vty,"In session %d ,set ccm enable success, interval %s %s",psess->sess_id,argv[0], VTY_NEWLINE);		

	psess->state = OAM_STATUS_UP;
	
	if(psess->alarm.alarm_loc)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_LOC);
		
		psess->alarm.alarm_loc = 0;
	}
	
	if(psess->alarm.alarm_rdi)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_RDI);
		
		psess->alarm.alarm_rdi = 0;
	}

	if(psess->alarm.alarm_ais)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_AIS);

		psess->alarm.alarm_ais = 0;		
	}

	//802.1ag
	if(psess->alarm.alarm_ma)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_CRE);

		/*bug:56719 modify*/
		//psess->alarm.alarm_ma = 0;
	}
	
	if(psess->alarm.alarm_error)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_FRE);
	
		/*bug:57605 57602*/
		//psess->alarm.alarm_error = 0;
	}

	//y.1731
	if(psess->alarm.alarm_level)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_MML);

		psess->alarm.alarm_level = 0;
	}

	if(psess->alarm.alarm_meg)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_MMG);

		psess->alarm.alarm_meg = 0;
	}

	if(psess->alarm.alarm_mep)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_UNM);

		psess->alarm.alarm_mep = 0;
	}

	if(psess->alarm.alarm_cc)
	{
		cfm_session_alarm_report(psess,IPC_OPCODE_CLEAR,GPN_ALM_TYPE_FUN_CFM_UNP);

		psess->alarm.alarm_cc = 0;
	}

	if(gelps.sess_enable)
	{
		elps_state_update(psess->ifindex,psess->sess_id,CC_STATE_UP);
	}

	if(vty->config_read_flag)
	{
		return CMD_SUCCESS; 		
	}
	
	//ipc_send_hal( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_CC, IPC_OPCODE_ENABLE, 0);
	cfm_send_hal( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_CC, IPC_OPCODE_ENABLE, 0);
	
	return CMD_SUCCESS;
}

DEFUN(no_session_ccm_en,
		no_session_ccm_en_cmd,
		"no cc enable",
		NO_STR
		"Continuity check message\n"
		"Enable\n")
{
	struct cfm_sess *psess=NULL;

	psess = (struct cfm_sess *)vty->index;

	if(!psess->cc_enable)
	{
		vty_error_out(vty,"Ccm is already disabled %s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}


	psess->cc_enable = 0;
	vty_info_out(vty,"In session %d ,ccm disable success %s",psess->sess_id, VTY_NEWLINE);		

	//ipc_send_hal( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_CC, IPC_OPCODE_DISABLE, 0);
	cfm_send_hal( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_CC, IPC_OPCODE_DISABLE, 0);
	
	return CMD_SUCCESS;
}

DEFUN(cfm_enable_interface,
		cfm_enable_interface_cmd,
		"cfm enable  interface {gigabitethernet USP|xgigabitethernet USP}",
		"Connectivity fault management\n"
		"Enable\n"	
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)
{
	struct cfm_sess 	*psess	 	 = NULL;
    uint32_t 			 ifindex	 = 0;
	uint8_t				 pmac[6] 	 = {0};
    struct hash_bucket  *pbucket	 = NULL;
	int					 cursor 	 = 0;
	struct cfm_sess		*psess_node  = NULL;
	struct l2if 		*pif 		 = NULL;
	int 				 ret		 = 0;
	
	//struct listnode *p_listnode = NULL;
	//void *data = NULL;
	//uint8_t flag = 0;
		
	psess = (struct cfm_sess *)vty->index;


	if(psess->ifindex)
	{
		vty_error_out(vty,"Please delete cfm interface in session first%s",VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(!psess->ma_index)
	{
		vty_error_out(vty,"Please config service ma first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	if(!psess->local_mep)
	{
		vty_error_out(vty,"Please config local mep first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	if(!psess->remote_mep)
	{
		vty_error_out(vty,"Please config remote mep first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}
	
	if(argv[0] != 0)
    {
    	ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[0] );
	}
	else if(argv[1] != 0)
    {
    	ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[1] );
	}
    if ( ifindex == 0 )
    {
        vty_error_out(vty,"Wrong format,please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

	pif = l2if_lookup(ifindex);
	if (NULL != pif)
	{
		if (pif->trunkid)
		{
        	vty_error_out(vty,"Please unbind form trunk port %d%s", pif->trunkid, VTY_NEWLINE );
			return CMD_WARNING;
		}
	}

	/*if(DISABLE == vlan_table[psess->vlan]->vlan.enable)
	{
		vty_error_out( vty, " the vlan %d not enable%s",psess->vlan, VTY_NEWLINE );
		return CMD_WARNING;
	}

	for(ALL_LIST_ELEMENTS_RO(&(vlan_table[psess->vlan]->portlist),p_listnode,data))
	{
		if(data == ifindex)
		{
			flag =1;
			break;
		}
	}

	if(flag == 0)
	{
		vty_error_out( vty, " the interface does not blong to the vlan %d %s",psess->vlan, VTY_NEWLINE );
		return CMD_WARNING;
	}*/

    if ( IFM_IS_SUBPORT ( ifindex ) )
    {
		if(psess->direct != MEP_DIRECT_UP || !psess->over_pw)
		{
			vty_error_out(vty,"Direction is not up over pw , can not bind subport%s", VTY_NEWLINE );
			return CMD_WARNING;
		}
        #if 0
		pifm_info = ifm_get_all_info(ifindex,MODULE_ID_L2);
		if(pifm_info)
		{
			if(pifm_info->encap.type == IFNET_ENCAP_QINQ)
			{
				if(!pifm_info->encap.cvlan.vlan_start)
				{
					vty_error_out( vty, "please encapsulate vlan for the subport first%s", VTY_NEWLINE );
					return CMD_WARNING;
				}
				
				if((psess->vlan < pifm_info->encap.cvlan.vlan_start) || (psess->vlan > pifm_info->encap.cvlan.vlan_end))
				{
					vty_error_out( vty, "ma vlan is not equal to the subport vlan%s", VTY_NEWLINE );
					return CMD_WARNING;
				}
			}
			else
			{
				if(!pifm_info->encap.svlan.vlan_start)
				{
					vty_error_out( vty, "please encapsulate vlan for the subport first%s", VTY_NEWLINE );
					return CMD_WARNING;
				}
				
				if((psess->vlan < pifm_info->encap.svlan.vlan_start) || (psess->vlan > pifm_info->encap.svlan.vlan_end))
				{
					vty_error_out( vty, "ma vlan is not equal to the subport vlan%s", VTY_NEWLINE );
					return CMD_WARNING;
				}
			}
		}
		#endif
	}

	HASH_BUCKET_LOOP(pbucket,cursor,cfm_sess_table)
	{
		psess_node = (struct cfm_sess *)pbucket->data;

		//level+vlan can not be same
		if(psess_node->ifindex== ifindex && (psess_node->level == psess->level && psess_node->vlan == psess->vlan))
		{
			vty_error_out(vty,"level+vlan+interface can not be all same%s", VTY_NEWLINE);  
			return CMD_WARNING; 		
		}		
	}	

	//get eth port smac ,eth subport has no mac, but equal to parent port. 
	if(!vty->config_read_flag)
	{
		if(ifm_get_mac(ifindex, MODULE_ID_L2, pmac) == 0)
		{		
			memcpy(psess->smac , pmac , 6);
			zlog_debug(CFM_DBG_COMMN, "get mac:%02x:%02x:%02x:%02x:%02x:%02x",
				pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);			
		}
		else
		{
			vty_error_out(vty,"Can not get mac on the interface,maybe the interface not exist %s",VTY_NEWLINE); 
			return CMD_WARNING; 		
		}
	}
	

	psess->ifindex = ifindex;

	if(psess->cc_enable)
		psess->state   = OAM_STATUS_UP;
	else
		psess->state   = OAM_STATUS_ENABLE;

	//clear alarms
	cfm_session_clear_alarm(psess);

	if(vty->config_read_flag)
	{
		return CMD_SUCCESS; 		
	}
	
	//ret = ipc_send_hal_wait_ack( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_ADD, 0);

	ret = cfm_ipc_send_wait_ack(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_ADD, 0);
	if(ret != 0)
	{
		zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
		
		/*ht201:128  ht2200:1024 */
		if(ret == ERRNO_OVERSIZE)
		{
			vty_error_out(vty,"Oversize!%s",VTY_NEWLINE);
		}
		else
		{
			vty_error_out(vty,"Set error!%s",VTY_NEWLINE);
		}
		
		psess->ifindex = 0;
		psess->state   = OAM_STATUS_DISABLE;
		
		return CMD_WARNING;
	}
	
	vty_info_out(vty,"Set success %s", VTY_NEWLINE);		

	return CMD_SUCCESS; 		

}

DEFUN(no_cfm_enable_interface,
		no_cfm_enable_interface_cmd,
		"no cfm enable  interface {gigabitethernet USP|xgigabitethernet USP}",
		NO_STR
		"Connectivity fault management\n"
		"Enable\n"	
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)
{
	struct cfm_sess *psess	 = NULL;
	struct l2if     *pif     = NULL;
    uint32_t 		 ifindex = 0;
	int 			 ret;
	
	psess= (struct cfm_sess *)vty->index;
	
	if(argv[0] != 0)
    {
    	ifindex = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[0] );
	}
	else if(argv[1] != 0)
    {
    	ifindex = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[1] );
	}
	
    if ( ifindex == 0 )
    {
        vty_error_out(vty,"Wrong format,please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

	pif = l2if_lookup(ifindex);
	if (NULL != pif)
	{
		if (pif->trunkid)
		{
        	vty_error_out(vty,"Please unbind form trunk port %d%s", pif->trunkid, VTY_NEWLINE );
			return CMD_WARNING;
		}
	}

	if(psess->cc_enable ||psess->lb_enable ||psess->lt_enable||(psess->lm_enable == OAM_STATUS_ENABLE)||(psess->dm_enable == OAM_STATUS_ENABLE)
		||psess->ais_enable)
	{
		vty_error_out(vty,"Can not delete cfm interface,please check ccm,lb,lt,lm,dm,ais enable.%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	if(!psess->ifindex)
	{
		vty_error_out(vty,"There is no interface binding %s", VTY_NEWLINE);		
		return CMD_WARNING;		
	}

	if(psess->ifindex != ifindex)
	{
		vty_error_out(vty,"The interface not map %s", VTY_NEWLINE);		
		return CMD_WARNING;		
	}

	//ret = ipc_send_hal_wait_ack( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, 0);

	ret = cfm_ipc_send_wait_ack(psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, 0);
	if(ret != 0)
	{
		vty_error_out(vty,"Set error %s", VTY_NEWLINE);		
		return CMD_WARNING;		
	}

	
	psess->ifindex = 0;
	psess->state   = OAM_STATUS_DISABLE;
	memset(psess->smac,0,6);

	
	//clear alarms
	cfm_session_clear_alarm(psess);

	
	vty_info_out(vty,"Set success %s", VTY_NEWLINE);		

	return CMD_SUCCESS;		


}


DEFUN(cfm_enable_interface_trunk,
		cfm_enable_interface_trunk_cmd,
		"cfm enable  interface trunk TRUNK",
		"Connectivity fault management\n"
		"Enable\n"	
		CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
		CLI_INTERFACE_TRUNK_VHELP_STR)
{
	struct cfm_sess 	*psess	    = NULL;
    uint32_t 			 ifindex    = 0;
	uint8_t				 pmac[6]    = {0};
    struct hash_bucket  *pbucket    = NULL;
	int 				 cursor     = 0;
	struct cfm_sess 	*psess_node = NULL;
	int					 ret 		= 0;
	
	psess = (struct cfm_sess *)vty->index;


	if(psess->ifindex)
	{
		vty_error_out(vty,"Please delete cfm interface in session first%s",VTY_NEWLINE);
		return CMD_WARNING;		
	}

	if(!psess->ma_index)
	{
		vty_error_out(vty,"Please config service ma first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	if(!psess->local_mep)
	{
		vty_error_out(vty,"Please config local mep first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	if(!psess->remote_mep)
	{
		vty_error_out(vty,"Please config remote mep first%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

    ifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        vty_error_out(vty,"Wrong format,please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }


	HASH_BUCKET_LOOP(pbucket,cursor,cfm_sess_table)
	{
		psess_node = (struct cfm_sess *)pbucket->data;

		//level+vlan can not be same
		if(psess_node->ifindex== ifindex && (psess_node->level == psess->level && psess_node->vlan == psess->vlan))
		{
			vty_error_out(vty,"level+vlan+interface can not be all same%s", VTY_NEWLINE);  
			return CMD_WARNING; 		
		}		
	}	

	//get trunk port mac
	if(!vty->config_read_flag)
	{

		if(ifm_get_mac(ifindex, MODULE_ID_L2,pmac) == 0)
		{
			memcpy(psess->smac , pmac , 6);
			zlog_debug(CFM_DBG_COMMN, "get mac:%02x:%02x:%02x:%02x:%02x:%02x",
				pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);
		}
		else
		{
			vty_error_out(vty,"Cannot get mac on the interface,maybe the interface not exist%s",VTY_NEWLINE);	
			return CMD_WARNING; 		
		}
	}

	psess->ifindex = ifindex;
	
	if(psess->cc_enable)
		psess->state   = OAM_STATUS_UP;
	else
		psess->state   = OAM_STATUS_ENABLE;

	//clear alarms
	cfm_session_clear_alarm(psess);

	if(vty->config_read_flag)
	{
		return CMD_SUCCESS; 		
	}

	//ret = ipc_send_hal_wait_ack( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_ADD, 0);

	ret = cfm_ipc_send_wait_ack( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_ADD, 0);
	if(ret != 0)
	{
		zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
		
		/*ht201:128  ht2200:1024 */
		if(ret == ERRNO_OVERSIZE)
		{
			vty_error_out(vty,"Oversize!%s",VTY_NEWLINE);
		}
		else
		{
			vty_error_out(vty,"Set error!%s",VTY_NEWLINE);
		}
		
		psess->ifindex = 0;
		psess->state   = OAM_STATUS_DISABLE;
		
		return CMD_WARNING;
	}

	
	vty_info_out(vty,"Set success %s", VTY_NEWLINE);		

	return CMD_SUCCESS; 		

}


DEFUN(no_cfm_enable_interface_trunk,
		no_cfm_enable_interface_trunk_cmd,
		"no cfm enable  interface trunk TRUNK",
		NO_STR
		"Connectivity fault management\n"
		"Enable\n"	
		CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
		CLI_INTERFACE_TRUNK_VHELP_STR)
{
	struct cfm_sess *psess	 = NULL;
    uint32_t 		 ifindex = 0;
	int				 ret	 = 0;
	
	psess 	= (struct cfm_sess *)vty->index;
	
    ifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[0] );
    if ( ifindex == 0 )
    {
        vty_error_out(vty,"Wrong format,please check out%s", VTY_NEWLINE );
        return CMD_WARNING;
    }

	if(psess->cc_enable ||psess->lb_enable ||psess->lt_enable||(psess->lm_enable == OAM_STATUS_ENABLE)||(psess->dm_enable == OAM_STATUS_ENABLE)
		||psess->ais_enable)
	{
		vty_error_out(vty,"Can not delete cfm interface,please check ccm,lb,lt,lm,dm,ais enable.%s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	if(!psess->ifindex)
	{
		vty_error_out(vty,"There is no interface binding %s", VTY_NEWLINE);		
		return CMD_WARNING;		
	}

	if(psess->ifindex != ifindex)
	{
		vty_error_out(vty,"The interface not map %s", VTY_NEWLINE);		
		return CMD_WARNING;		
	}

	//ipc_send_hal( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, 0);

	//ret = ipc_send_hal_wait_ack( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, 0);

	ret = cfm_ipc_send_wait_ack( psess, sizeof(struct cfm_sess), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_SESSION, IPC_OPCODE_DELETE, 0);
	if(ret != 0)
	{
		vty_error_out(vty,"Set error %s", VTY_NEWLINE);		
		return CMD_WARNING;		
	}
	
	psess->ifindex = 0;
	psess->state   = OAM_STATUS_DISABLE;
	memset(psess->smac,0,6);
	
	//clear alarms
	cfm_session_clear_alarm(psess);
	
	vty_info_out(vty,"Set success %s", VTY_NEWLINE);		

	return CMD_SUCCESS;		


}


DEFUN(cfm_lb_enable,
    cfm_lb_enable_cmd,
    "lb enable {mac XX:XX:XX:XX:XX:XX|size <95-1500>|discard <0-7>}",
    "Loopback.\n"
    "Enable.\n"
	"Mac address\n"
	"Mac address\n"
	"Number of data bytes\n"
	"Bytes 95 to 1500,default 95\n"
	"Discard loopback packets when congestion conditions\n"
	"Discard priority,default 0\n")
{
    struct cfm_sess *psess = (struct cfm_sess *)vty->index;
	int 			 ret   = 0;
    uchar 			 dmac[6];
	uint32_t		 size  = 95;

    if(psess->state == OAM_STATUS_DISABLE)
    {   
        vty_error_out(vty,"CFM-session is not enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    if(psess->lt_enable == 1)
    {
        vty_error_out(vty,"LT is already enable,can't enable LB.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    if(psess->lb_enable == 1)
    {
        vty_error_out(vty,"LB is already enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	//check mac
	if(argv[0])
	{
		if(ether_valid_mac(argv[0]))
		{
			vty_error_out(vty,"Invalid mac value%s",VTY_NEWLINE);
			return CMD_WARNING; 		
		}
		ether_string_to_mac(argv[0],dmac);
		
		if(dmac[0]&0x01)
		{
			vty_error_out(vty,"Not support multicast and broadcast MAC address%s",VTY_NEWLINE);
			return CMD_WARNING; 		
		}

	}
	else 
	{
		/*if(psess->dmac_valid)
		{
			memcpy(dmac,psess->dmac,6);
		}
		else
		{
			vty_error_out(vty, "please config remote mep mac%s", VTY_NEWLINE);
			return CMD_WARNING;
		}*/

		dmac[0] = 0x01;
		dmac[1] = 0x80;
		dmac[2] = 0xC2;
		dmac[3] = 0x00;
		dmac[4] = 0x00;
		dmac[5] = 0x30+psess->level;
		
	}

    if(argv[1])
    {
        size = atoi(argv[1]);        
    }


	ret = cfm_lb_start (psess, (void *)vty,dmac,size);
	if (ERRNO_SUCCESS != ret)
	{
        vty_error_out(vty,"LBM is sent failed.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}

    psess->lb_enable = 1;



    return CMD_WAIT;	
}



DEFUN(no_cfm_lb_enable,
    no_cfm_lb_enable_cmd,
    "no lb enable",
	NO_STR    
    "Loopback.\n"
    "Enable.\n")
{
    struct cfm_sess *psess = (struct cfm_sess *)vty->index;
    
    
    if(psess->lb_enable == 0)
    {
        vty_error_out(vty,"LB is already disable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	cfm_lb_stop (psess);
    psess->lb_enable = 0;



    return CMD_SUCCESS;
}


DEFUN(cfm_lt_enable,
    cfm_lt_enable_cmd,
    "lt enable {mac XX:XX:XX:XX:XX:XX}",
    "Link tracing.\n"
    "Enable.\n"
	"Mac address\n"
	"Mac address\n")
{
    struct cfm_sess *psess = (struct cfm_sess *)vty->index;
	int 			 ret   = 0;
    uchar 			 dmac[6];
	

    if(psess->state == OAM_STATUS_DISABLE)
    {   
        vty_error_out(vty, "CFM-session is not enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    if(psess->lb_enable == 1)
    {
        vty_error_out(vty, "LB is already enable,can't enable LT.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    if(psess->lt_enable == 1)
    {
        vty_error_out(vty, "LT is already enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	//check mac
	if(argv[0])
	{
		if(ether_valid_mac(argv[0]))
		{
			vty_error_out(vty,"Invalid mac value%s",VTY_NEWLINE);
			return CMD_WARNING; 		
		}
		ether_string_to_mac(argv[0],dmac);
		
		if(dmac[0]&0x01)
		{
			vty_error_out(vty,"Not support multicast and broadcast MAC address%s",VTY_NEWLINE);
			return CMD_WARNING; 		
		}

	}
	else 
	{
		if(psess->dmac_valid)
		{
			memcpy(dmac,psess->dmac,6);
		}
		else
		{
			vty_error_out(vty,"Please config remote mep mac%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}


	ret = cfm_lt_start (psess, (void *)vty,dmac);
	if (ERRNO_SUCCESS != ret)
	{
        vty_error_out(vty,"LTM is sent failed.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}

    psess->lt_enable = 1;
        
    
    return CMD_WAIT;	
}



DEFUN(no_cfm_lt_enable,
    no_cfm_lt_enable_cmd,
    "no lt enable",
    NO_STR
    "Link tracing.\n"
    "Enable.\n")
{
    struct cfm_sess *psess = (struct cfm_sess *)vty->index;
    
    if(psess->lt_enable == 0)
    {
        vty_error_out(vty,"LT is already disable.%s", VTY_NEWLINE);    
        return CMD_WARNING;
    }

	cfm_lt_stop (psess);
    psess->lt_enable = 0;        
    
    return CMD_SUCCESS;
}


DEFUN(cfm_lm_enable,
    cfm_lm_enable_cmd,
    "lm enable {interval <5-3600> | mac XX:XX:XX:XX:XX:XX }",
    "Loss Measurement\n"
    "Enable\n"
    "Interval of Loss Measurement Message\n"
    "Range of interval:<5-3600>,unit:second\n"
    "Mac address\n"
    "Mac address\n")
{
    struct cfm_sess *psess 	  = (struct cfm_sess *)vty->index;
    uint16_t 		 interval = 5;
	uint8_t 		 dmac[6];
	

    if(psess->state == OAM_STATUS_DISABLE)
    {   
        vty_error_out(vty,"CFM-session is not enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	if(psess->perfm_enable)
	{
        vty_error_out(vty,"performance monitor is running.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
    
    if(psess->lm_enable == 1)
    {
        vty_error_out(vty,"LM is running.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }


    if(psess->psla == NULL)
    {
        psess->psla = cfm_session_create_sla(psess);
        if(psess->psla == NULL)
        {
            vty_error_out(vty,"Malloc fail.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

	
    if(argv[0] != NULL)
    {
        interval = atoi(argv[0]);        
    }

	//check mac
	if(argv[1])
	{
		if(ether_valid_mac(argv[1]))
		{
			vty_error_out(vty,"Invalid mac value%s",VTY_NEWLINE);
			return CMD_WARNING; 		
		}
		ether_string_to_mac(argv[1],dmac);
		
		if(dmac[0]&0x01)
		{
			vty_error_out(vty,"Not support multicast and broadcast MAC address%s",VTY_NEWLINE);
			return CMD_WARNING; 		
		}

	}
	else 
	{
		if(psess->dmac_valid)
		{
			memcpy(dmac,psess->dmac,6);
		}
		else
		{
			vty_error_out(vty,"Please config remote mep mac%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}


    psess->lm_interval = interval;
	memcpy(psess->lm_dmac,dmac,6);
	
    cfm_session_lm_enable(psess);
        
    return CMD_SUCCESS;
}


DEFUN(no_cfm_lm_enable,
    no_cfm_lm_enable_cmd,
    "no lm enable",
    "Delete\n"
    "Loss Measurement\n"
    "Enable\n")
{
    struct cfm_sess *psess = (struct cfm_sess *)vty->index;

    if(psess->lm_enable != 1)
    {   
        vty_error_out(vty,"LM is not running.%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    
    cfm_session_lm_disable(psess);
    
    return CMD_SUCCESS;
}



DEFUN(cfm_dm_enable,
    cfm_dm_enable_cmd,
    "dm enable {interval <5-3600> | mac XX:XX:XX:XX:XX:XX}",
    "Delay Measurement\n"
    "Enable\n"
    "Interval of Delay Measurement Message\n"
    "Range of interval:<5-3600>,unit:second\n"
    "Mac address\n"
    "Mac address\n")
{
    struct cfm_sess *psess 	  = (struct cfm_sess *)vty->index;
    uint16_t		 interval = 5;    
	uint8_t 		 dmac[6];
	

    if(psess->state == OAM_STATUS_DISABLE)
    {   
        vty_error_out(vty,"CFM-session is not enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if(psess->dm_enable == 1)
    {
        vty_error_out(vty,"DM is running.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }


    if(psess->psla == NULL)
    {
        psess->psla = cfm_session_create_sla(psess);
        if(psess->psla == NULL)
        {
            vty_error_out(vty,"Malloc fail.%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    
    if(argv[0] != NULL)
    {
        interval = atoi(argv[0]);        
    }

	//check mac
	if(argv[1])
	{
		if(ether_valid_mac(argv[1]))
		{
			vty_error_out(vty,"Invalid mac value%s",VTY_NEWLINE);
			return CMD_WARNING; 		
		}
		ether_string_to_mac(argv[1],dmac);
		
		if(dmac[0]&0x01)
		{
			vty_error_out(vty,"Not support multicast and broadcast MAC address%s",VTY_NEWLINE);
			return CMD_WARNING; 		
		}

	}
	else 
	{
		if(psess->dmac_valid)
		{
			memcpy(dmac,psess->dmac,6);
		}
		else
		{
			vty_error_out(vty,"Please config remote mep mac%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	
    psess->dm_interval = interval;
	memcpy(psess->dm_dmac,dmac,6);

    cfm_session_dm_enable(psess);
    
    return CMD_SUCCESS;
}


DEFUN(no_cfm_dm_enable,
    no_cfm_dm_enable_cmd,
    "no dm enable",
    "Delete\n"
    "Delay Measurement\n"
    "Enable\n")
{
    struct cfm_sess *psess = (struct cfm_sess *)vty->index;

    if(psess->dm_enable != 1)
    {   
        vty_error_out(vty,"DM is not running.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    cfm_session_dm_disable(psess);
    
    return CMD_SUCCESS;
}


DEFUN(cfm_ais_enable,
    cfm_ais_enable_cmd,
    "ais enable interval (1s|60s) level <1-7>",
    "Alarm indication signal\n"
    "Enable\n"
    "Interval of AIS message\n"
    "interval: 1s\n"
    "interval: 60s\n"    
    "The md level\n"
    "The md level value: 1-7\n")
{
    struct cfm_sess *psess 	  = (struct cfm_sess *)vty->index;
    uint8_t 		 interval = 1;
    uint8_t			 level    = 0;
	struct cfm_ais  *pais	  = psess->pais;
	unsigned char 	flag 	  = 0;

    if(psess->state == OAM_STATUS_DISABLE)
    {   
        vty_error_out(vty,"CFM-session is not enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	
    /*if(psess->ais_enable == 1)
    {
		vty_error_out(vty,"ais is already enabled %s",VTY_NEWLINE);		
        return CMD_WARNING;
    }*/
	
	//interval
	if(!strncmp(argv[0],"1",1))
		interval = 1;	
	else 
		interval = 60;

	if(psess->ais_enable)
	{
		if(interval!= psess->ais_send_interval)
		{
			vty_error_out(vty,"Interval is not same to last config.%s", VTY_NEWLINE);
			return CMD_WARNING;			
		}
	}

	//level
	level = atoi(argv[1]);
	if(level <= psess->level)
	{
		vty_error_out(vty,"Level should be greater than the session level %d %s",psess->level,VTY_NEWLINE);		
		return CMD_WARNING;
	}

	if(psess->ais_enable)
	{
		if(psess->ais_send_level & (0x01<<level))
		{
			vty_error_out(vty,"Repeated level %s",VTY_NEWLINE);		
			return CMD_WARNING;
		}
	}

	if(!pais)
	{
		pais = (struct cfm_ais *)XMALLOC(MTYPE_TMP, sizeof(struct cfm_ais));
		if (NULL == pais)
		{
			vty_error_out(vty,"XMALLOC error%s",VTY_NEWLINE);
		
			return CMD_WARNING;
		}
		
		flag = 1;
		memset(pais,0,sizeof(struct cfm_ais));
	}

	vty_info_out(vty,"Set success %s", VTY_NEWLINE);		

	
	psess->ais_enable = 1;	
	psess->pais 	  = pais;
	
	psess->ais_send_interval = interval;		
	psess->ais_send_level   |= 0x01<<level;

	if(flag)
		cfm_ais_start(psess);
	
    return CMD_SUCCESS;
}



DEFUN(no_cfm_ais_enable,
    no_cfm_ais_enable_cmd,
    "no ais enable  ",
    NO_STR
    "Alarm indication signal\n"
    "Enable\n")
{
    struct cfm_sess *psess = (struct cfm_sess *)vty->index;

	if(!psess->ais_enable)
	{
		vty_error_out(vty,"Ais is already disabled %s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}	

	cfm_ais_stop(psess);

	psess->ais_enable 		 = 0;
	psess->ais_send_interval = 0;		
	psess->ais_send_level    = 0;
		

	vty_info_out(vty,"Set success %s",VTY_NEWLINE);		

	
    return CMD_SUCCESS;
}




DEFUN(cfm_tst_mode_in_service,
    cfm_tst_mode_in_service_cmd,
    "test mode in-service",
    "Eth-test\n"
    "Mode of eth-test\n"
    "MEP is configured for in-service testing\n")
{
    struct cfm_sess 	*psess = (struct cfm_sess *)vty->index;

    if(psess->tst_enable == 1)
    {
        vty_error_out(vty,"Eth-test is running.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

#if 0
	if(psess->tst_mode == 0)
	{
        vty_error_out(vty,"Eth-test mode is already in-service.%s", VTY_NEWLINE);
        return CMD_WARNING;
	}
#endif

	psess->tst_mode			 = 0;
	psess->lck_send_interval = 0;
	psess->lck_send_level	 = 0;  //clear lck target level
	
	vty_info_out(vty,"Set success %s",VTY_NEWLINE);		

	
    return CMD_SUCCESS;
}

DEFUN(cfm_tst_mode_out_service,
    cfm_tst_mode_out_service_cmd,
    "test mode out-of-service lck-interval (1s|60s) lck-level <1-7>",
    "Eth-test\n"
    "Mode of eth-test\n"
    "MEP is configured for out-of-service testing\n"
    "Tx Interval of lck message, should be same as ais tx interval\n"
    "interval: 1s\n"
    "interval: 60s\n"    
    "Lck target md level\n"
    "The md level value: 1-7\n")
{
    struct cfm_sess 	*psess = (struct cfm_sess *)vty->index;
	uint8_t interval = 1;
	uint8_t level 	 = 0;
	
    if(psess->tst_enable == 1)
    {
        vty_error_out(vty,"Eth-test is running.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }  

	//interval
	if(!strncmp(argv[0],"1",1))
		interval = 1;	
	else 
		interval = 60;

	if(psess->lck_send_interval && psess->lck_send_interval!=interval)
	{
		vty_error_out(vty,"Interval is not same to last config.%s", VTY_NEWLINE);
		return CMD_WARNING;			
	}
	
	if(psess->ais_send_interval && psess->ais_send_interval!=interval)
	{
		vty_error_out(vty,"Interval is not same to ais tx interval.%s", VTY_NEWLINE);
		return CMD_WARNING;			
	}
	//level
	level = atoi(argv[1]);
	if(level <= psess->level)
	{
		vty_error_out(vty,"Level should be greater than the session level %d %s",psess->level,VTY_NEWLINE);		
		return CMD_WARNING;
	}
#if 0
	if(psess->lck_send_level & (0x01<<level))
	{
		vty_error_out(vty,"Repeated level %s",VTY_NEWLINE);		
		return CMD_WARNING;
	}
#endif		
	psess->tst_mode = 1;
	psess->lck_send_interval = interval;	
	psess->lck_send_level 	|= 0x01<<level;  //add lck target level
	
	vty_info_out(vty,"Set success %s",VTY_NEWLINE);		

    return CMD_SUCCESS;
}


DEFUN(cfm_test_enable,
    cfm_test_enable_cmd,
    "test enable {"
    "mac XX:XX:XX:XX:XX:XX"
    "|size <64-1400>|number <1-10>"
    "|interval <0-10>"
    "|pattern (zero_without_crc|zero_with_crc|prbs_without_crc|prbs_with_crc)|discard <0-7>}",
    "Eth-test\n"
    "Enable\n"
    "Mac address\n"
    "Mac address\n"
	"Number of data bytes\n"
	"Bytes 64 to 1400,default 64\n"
	"Number of packets\n"
	"Packet numbers 1 to 10,default 5\n"
	"Interval of test message\n"
	"Interval 0 to 10s, default 1s\n"
	"Pattern of the test TLV in the TST message\n"
	"All zero without CRC32(default)\n"
	"All zero with CRC32\n"
	"Pseudo random bit sequence without CRC32\n"
	"Pseudo random bit sequence with CRC32\n"
	"Discard test packets when congestion conditions\n"
	"Discard priority,default 0\n")
{
    struct cfm_sess *psess = (struct cfm_sess *)vty->index;
	uint8_t  dmac[6];	
	uint32_t size 	  = 64;	
    uint8_t  num  	  = 5;  
    uint8_t  interval = 1; 
	uint8_t  discard  = 0;
    enum cfm_tst_pattern  pattern  = zero_without_crc32;  
	
	struct cfm_test     *ptest = psess->ptest;

    if(psess->state == OAM_STATUS_DISABLE)
    {   
        vty_error_out(vty,"CFM-session is not enable.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if(psess->tst_enable == 1)
    {
        vty_error_out(vty,"Eth-test is running.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }  

	//check mac
	if(argv[0])
	{
		if(ether_valid_mac(argv[0]))
		{
			vty_error_out(vty,"Invalid mac value%s",VTY_NEWLINE);
			return CMD_WARNING; 		
		}
		ether_string_to_mac(argv[0],dmac);
		
		if(dmac[0]&0x01)
		{
			vty_error_out(vty,"Not support multicast and broadcast MAC address%s",VTY_NEWLINE);
			return CMD_WARNING; 		
		}

	}
	else 
	{
		dmac[0] = 0x01;
		dmac[1] = 0x80;
		dmac[2] = 0xC2;
		dmac[3] = 0x00;
		dmac[4] = 0x00;
		dmac[5] = 0x30+psess->level;		
	}

	//size
    if(argv[1])
    {
        size = atoi(argv[1]);        
    }

	//number
    if(argv[2] != NULL)
    {
        num = atoi(argv[2]);        
    }

	//Interval
    if(argv[3] != NULL)
    {
        interval= atoi(argv[3]);        
    }

	//pattern
    if(argv[4] != NULL)
    {
		if(!strncmp(argv[4],"z",1))
		{
			if(!strncmp(argv[4]+9,"o",1))
			{
				pattern = zero_without_crc32;
			}
			else
			{
				pattern = zero_with_crc32;
			}
				
		}
		else 
		{
			if(!strncmp(argv[4]+9,"o",1))
			{
				pattern = prbs_without_crc32;
			}
			else
			{
				pattern = prbs_with_crc32;
			}
		}
    }

	//number
    if(argv[5] != NULL)
    {
        discard = atoi(argv[5]);        
    }

	
	if(!ptest)
	{
		ptest = (struct cfm_test *)XMALLOC(MTYPE_TMP, sizeof(struct cfm_test));
		if (NULL == ptest)
		{
			vty_error_out(vty,"XMALLOC error%s",VTY_NEWLINE);
		
			return CMD_WARNING;
		}
		memset(ptest,0,sizeof(struct cfm_test));
	}

	memcpy(ptest->tst_dmac,dmac,6);
	ptest->tst_len 		   = size;
	ptest->tst_tx_count	   = num;
	ptest->tst_tx_interval = interval;
    ptest->tst_pattern 	   = pattern;
	
	psess->ptest 	  = ptest;
	psess->tst_enable = 1;

	cfm_test_start(psess);
	
	vty_info_out(vty,"dmac:%02X:%02X:%02X:%02X:%02X:%02X,"
		"zize:%d,num:%d,interval:%d,pattern:%d,discard:%d %s",
		dmac[0],dmac[1],dmac[2],dmac[3],dmac[4],dmac[5],
		size,num,interval,pattern,discard,VTY_NEWLINE);
	
    return CMD_SUCCESS;
}

DEFUN(no_cfm_test_enable,
    no_cfm_test_enable_cmd,
    "no test enable",
    NO_STR
    "Eth-test\n"
    "Enable\n")
{
    struct cfm_sess *psess = (struct cfm_sess *)vty->index;

	if(psess->tst_enable == 0)
	{
		vty_error_out(vty,"Eth-test is already disabled %s",VTY_NEWLINE);		
		return CMD_WARNING; 		
	}

	cfm_test_stop(psess);	

	vty_info_out(vty,"Set success %s",VTY_NEWLINE); 
	
	return CMD_SUCCESS;
	
}


static void cfm_show_md_node(struct vty *vty ,struct cfm_md * pmd)
{
	struct cfm_ma   *pma = NULL;
	struct listnode *pnode;

	vty_out(vty," md index           : %d%s",pmd->md_index,VTY_NEWLINE);
	
	if(pmd->name[0])
		vty_out(vty," md name            : %s%s",pmd->name,VTY_NEWLINE);	
	else
		vty_out(vty," md name			 : %s%s","--",VTY_NEWLINE);	
	
	vty_out(vty," md level           : %d%s",pmd->level,VTY_NEWLINE);
	vty_out(vty," ma list            :   %s",VTY_NEWLINE);

	pnode = pmd->ma_list.head;
	while(pnode)
	{
		pma = listgetdata(pnode);
		vty_out(vty,"         ma index           : %d%s",pma->ma_index,VTY_NEWLINE);
		
		if(pma->name[0])
			vty_out(vty,"         ma name            : %s%s",pma->name,VTY_NEWLINE);
		else
			vty_out(vty,"         ma name            : %s%s","--",VTY_NEWLINE);
			
		
		if(pma->vlan)
		{
			vty_out(vty,"         ma vlan            : %d%s",pma->vlan,VTY_NEWLINE);
		}
		else
		{
			vty_out(vty,"         ma vlan            : %s%s","--",VTY_NEWLINE);
		}

		vty_out(vty,"         ma priority        : %d%s",pma->priority,VTY_NEWLINE);
		
		
		pnode = pnode->next;
	}

}

static void cfm_show_md(struct vty *vty ,uint8_t md_index)
{
	int 			i = 0,j = 0;
	struct cfm_md  *pmd = NULL;


	if(md_index)
	{	
		pmd = cfm_md_table[md_index-1];
		if(pmd != NULL)
		{
			cfm_show_md_node(vty,pmd);	
		}
		else
		{
			vty_out(vty,"Info: The specified MD does not exist. %s",VTY_NEWLINE);
		}
		return;
	}


	for(i=0 ; i<CFM_MD_NUM_MAX ; i++)
	{
		if(cfm_md_table[i] != NULL)
			j++;
	}

	vty_out(vty,"The total number of MDs is %d %s",j,VTY_NEWLINE);

	if(!j)
		return;
	
	for(i=0 ; i<CFM_MD_NUM_MAX ; i++)
	{
		if(cfm_md_table[i] != NULL)
		{	
			pmd = cfm_md_table[i];
			
			vty_out(vty,"------------------------------------------------ %s",VTY_NEWLINE);
			
			cfm_show_md_node(vty,pmd);
		}
	}


}


static void cfm_show_ma_node(struct vty *vty ,struct cfm_ma * pma)
{
	struct cfm_md   *pmd=NULL;	
	struct listnode *pnode;
	struct cfm_sess *psess=NULL;	

	//ma index
	vty_out(vty," %-19s: %d%s","ma index",pma->ma_index,VTY_NEWLINE);

	if(pma->name[0])
		vty_out(vty," %-19s: %s%s","ma name",pma->name,VTY_NEWLINE);
	else
		vty_out(vty," %-19s: %s%s","ma name","--",VTY_NEWLINE);
	
	//md index
	pmd = cfm_md_lookup(pma->md_index);
	if(pmd)
	{
		vty_out(vty," %-19s: %d%s","md index",pmd->md_index,VTY_NEWLINE);
		
		if(pmd->name[0])
			vty_out(vty," %-19s: %s%s","md name",pmd->name,VTY_NEWLINE);	
		else
			vty_out(vty," %-19s: %s%s","md name","--",VTY_NEWLINE);	
			
	}

	//protocol
	if(pma->y1731_en)
		vty_out(vty," %-19s: %s%s","protocol","y.1731",VTY_NEWLINE);
	else
		vty_out(vty," %-19s: %s%s","protocol","802.1ag",VTY_NEWLINE);
	

	//ma vlan
	vty_out(vty," %-19s: %d%s","ma vlan",pma->vlan,VTY_NEWLINE);

	//md level
	if(pmd)
		vty_out(vty," %-19s: %d%s","md level",pmd->level,VTY_NEWLINE);

	//priority
	vty_out(vty," %-19s: %d%s","ma priority",pma->priority,VTY_NEWLINE);

	//mip enable
	if(pma->mip_enable)
		vty_out(vty," %-19s: %s%s","ma mip","enable",VTY_NEWLINE);
	else
		vty_out(vty," %-19s: %s%s","ma mip","disable",VTY_NEWLINE);
		

	//session list
	vty_out(vty," %-19s: %s","session list",VTY_NEWLINE);
	
	pnode = pma->sess_list.head;
	while(pnode)
	{
		psess = listgetdata(pnode);

		vty_out(vty,"         %-19s: %d%s","session index",psess->sess_id,VTY_NEWLINE);

		if(psess->local_mep)
		{
			vty_out(vty,"         %-19s: %d%s","lmep index",psess->local_mep,VTY_NEWLINE);
		}
		else
		{
			vty_out(vty,"         %-19s: %s%s","lmep index","--",VTY_NEWLINE);
		}

		if(psess->remote_mep)
		{
			vty_out(vty,"         %-19s: %d%s","rmep index",psess->remote_mep,VTY_NEWLINE);		
		}
		else
		{
			vty_out(vty,"         %-19s: %s%s","rmep index","--",VTY_NEWLINE); 			
		}

		
		
		pnode = pnode->next;
	}

}

static void cfm_show_ma(struct vty *vty ,uint16_t ma_index)
{
	struct cfm_ma 		*pma=NULL;
    struct hash_bucket  *pbucket = NULL;
    int 				 cursor;

	if(ma_index)
	{
		pma = cfm_ma_lookup(ma_index);
		if(pma)
			cfm_show_ma_node(vty,pma);		
		else
			vty_out(vty,"Info: The specified MA does not exist. %s",VTY_NEWLINE);

		return;
	}

	vty_out(vty,"The total number of MAs is %d %s",cfm_ma_table.num_entries,VTY_NEWLINE);

	if(!cfm_ma_table.num_entries)
		return;
	

	HASH_BUCKET_LOOP(pbucket,cursor,cfm_ma_table)
	{
		pma = pbucket->data;
		
		if(pma)
		{
			vty_out(vty,"------------------------------------------------ %s",VTY_NEWLINE);
			cfm_show_ma_node(vty,pma);	
		}
	}

	
		
}



static void cfm_show_session_node(struct vty *vty ,struct cfm_sess * psess)
{
	struct cfm_md *pmd = NULL;	
	struct cfm_ma *pma = NULL;		
	uint8_t 	  *mac = NULL;
	char str1[50];
	char str2[50];
	char str3[50];
	uint8_t i;

	//session index
	vty_out(vty," %-19s: %d%s","session index",psess->sess_id,VTY_NEWLINE);	
	
	//state
	if(psess->state == OAM_STATUS_DISABLE)		
		sprintf(str1,"%s","disable");
	else if(psess->state == OAM_STATUS_ENABLE)
	{
		sprintf(str1,"%s","enable");			
	}	
	else if(psess->state == OAM_STATUS_DOWN)
	{	
		sprintf(str1,"%s","down");			
	}
	else if(psess->state == OAM_STATUS_UP)
	{	
		sprintf(str1,"%s","up");			
	}
	
	vty_out(vty," %-19s: %s%s","state",str1,VTY_NEWLINE);

	//local mep index
	if(psess->local_mep)
		vty_out(vty," %-19s: %d%s","lmep index",psess->local_mep,VTY_NEWLINE);
	else
		vty_out(vty," %-19s: %s%s","lmep index","--",VTY_NEWLINE);

	//rmep index
	if(psess->remote_mep)
	{
		sprintf(str1,"%d",psess->remote_mep);
	}
	else
	{
		sprintf(str1,"%s","--");
	}

	if(psess->dmac_valid)
	{
		mac = psess->dmac;
		sprintf(str2,"%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	}
	else
	{
		sprintf(str2,"%s","--");
	}
	
	vty_out(vty," %-19s: %s%s","rmep index",str1,VTY_NEWLINE);
	vty_out(vty," %-19s: %s%s","rmep mac",str2,VTY_NEWLINE);
	
	//md index
	pmd = cfm_md_lookup(psess->md_index);
	if(pmd)
	{
		sprintf(str1,"%d",pmd->md_index);

		if(pmd->name[0])
			sprintf(str2,"%s",pmd->name);
		else
			sprintf(str2,"%s","--");
			
		
	}
	else
	{
		sprintf(str1,"%s","--");
		sprintf(str2,"%s","--");		
	}
	
	vty_out(vty," %-19s: %s%s","md index",str1,VTY_NEWLINE);	
	vty_out(vty," %-19s: %s%s","md name",str2,VTY_NEWLINE);
	
	//ma index,ma name, ma vlan 
	pma = cfm_ma_lookup(psess->ma_index);
	if(pma)
	{
		sprintf(str1,"%d",pma->ma_index);
		sprintf(str2,"%s",pma->name);		
		sprintf(str3,"%d",pma->vlan);	
	}
	else
	{
		sprintf(str1,"%s","--");
		sprintf(str2,"%s","--");	
		sprintf(str3,"%s","--");		
	}

	vty_out(vty," %-19s: %s%s","ma index",str1,VTY_NEWLINE);
	vty_out(vty," %-19s: %s%s","ma name",str2,VTY_NEWLINE);
	vty_out(vty," %-19s: %s%s","ma vlan",str3,VTY_NEWLINE);

	//md level
	if(pmd)
	{
		sprintf(str1,"%d",pmd->level);
	}
	else
	{
		sprintf(str1,"%s","--");
	}

	vty_out(vty," %-19s: %s%s","md level",str1,VTY_NEWLINE); 


	//direction
	vty_out(vty," %-19s: %s%s","direction",psess->direct?"down":"up",VTY_NEWLINE);

	//over pw
	if(!psess->direct)
	{
		vty_out(vty," %-19s: %s%s","over pw",psess->over_pw?"yes":"no",VTY_NEWLINE);
	}
	else
	{
		vty_out(vty," %-19s: %s%s","over pw","--",VTY_NEWLINE);
	}

	//interface
	if(psess->ifindex)
	{
		ifm_get_name_by_ifindex(psess->ifindex,str1);
	}
	else
	{	
		sprintf(str1,"%s","--");			
	}

	vty_out(vty," %-19s: %s%s","interface",str1,VTY_NEWLINE);

	//cc enable, interval
	if(psess->cc_enable)
		sprintf(str1,"%s","yes");		
	else
		sprintf(str1,"%s","--");

	if(psess->cc_interval)
		sprintf(str2,"%dms",psess->cc_interval);
	else
		sprintf(str2,"%s","--");		
	

	vty_out(vty," %-19s: %s%s","ccm enable",str1,VTY_NEWLINE);
	vty_out(vty," %-19s: %s%s","ccm interval",str2,VTY_NEWLINE);

	//ais
	if(psess->ais_enable)
	{
		sprintf(str1,"%s","yes");	
		sprintf(str2,"%ds",psess->ais_send_interval);	

		str3[0]=0;
		for(i=0;i<8;i++)
		{
			if(psess->ais_send_level & (0x01<<i))
			{
				sprintf(str3+strlen(str3),"%d,",i);	
			}
		}

		str3[strlen(str3)-1] = 0;//last ,
	}
	else
	{
		sprintf(str1,"%s","no");	
		sprintf(str2,"%s","--");	
		sprintf(str3,"%s","--");	
	}

	vty_out(vty," %-19s: %s%s","ais enable",str1,VTY_NEWLINE);
	vty_out(vty," %-19s: %s%s","ais interval",str2,VTY_NEWLINE);
	vty_out(vty," %-19s: %s%s","ais level",str3,VTY_NEWLINE);

	//test ¡¢lck, note:tst_enable not show
	if(psess->tst_mode==0)
	{
		sprintf(str1,"%s","in-service");		
		sprintf(str2,"%s","--");	
		sprintf(str3,"%s","--");	
	}
	else
	{
		sprintf(str1,"%s","out-of-service");		
		sprintf(str2,"%ds",psess->lck_send_interval);	
		
		str3[0]=0;
		for(i=0;i<8;i++)
		{
			if(psess->lck_send_level & (0x01<<i))
			{
				sprintf(str3+strlen(str3),"%d,",i); 
			}
		}

		str3[strlen(str3)-1] = 0;//last ,
		
	}	

	vty_out(vty," %-19s: %s%s","tst mode",str1,VTY_NEWLINE);
	vty_out(vty," %-19s: %s%s","lck interval",str2,VTY_NEWLINE);
	vty_out(vty," %-19s: %s%s","lck level",str3,VTY_NEWLINE);

	
	//alarm
	if(pma)
	{
		if(!pma->y1731_en)
		{
			if(psess->alarm.alarm_ma == 1)
				sprintf(str1,"%s","DefXconCCM");		
			else if(psess->alarm.alarm_error == 1)
				sprintf(str1,"%s","DefErrorCCM");	
			else if(psess->alarm.alarm_lck == 1)
				sprintf(str1,"%s","LCK");			
			else if(psess->alarm.alarm_ais == 1)
				sprintf(str1,"%s","AIS");	
			else if(psess->alarm.alarm_loc == 1)
				sprintf(str1,"%s","LOC");		
			else if(psess->alarm.alarm_port_down== 1)
				sprintf(str1,"%s","interface down"); 	
			else if(psess->alarm.alarm_port_block== 1)
				sprintf(str1,"%s","port block");	
			else if(psess->alarm.alarm_rdi== 1)
				sprintf(str1,"%s","RDI");		
			else
				sprintf(str1,"%s","--");		
		}
		else
		{
			if(psess->alarm.alarm_level== 1)
				sprintf(str1,"%s","UnexpectedMEGLevel");		
			else if(psess->alarm.alarm_meg == 1)
				sprintf(str1,"%s","UnexpectedMEG");	
			else if(psess->alarm.alarm_mep == 1)
				sprintf(str1,"%s","UnexpectedMEP");	
			else if(psess->alarm.alarm_cc == 1)
				sprintf(str1,"%s","UnexpectedPeriod");	
			else if(psess->alarm.alarm_lck == 1)
				sprintf(str1,"%s","LCK");			
			else if(psess->alarm.alarm_ais == 1)
				sprintf(str1,"%s","AIS");	
			else if(psess->alarm.alarm_loc == 1)
				sprintf(str1,"%s","LOC");		
			else if(psess->alarm.alarm_rdi== 1)
				sprintf(str1,"%s","RDI");		
			else
				sprintf(str1,"%s","--");		
		}
	}
	else
	{
		sprintf(str1,"%s","--");		
	}

	vty_out(vty," %-19s: %s%s","alarm",str1,VTY_NEWLINE);

	//lm state
    if(psess->lm_enable == OAM_STATUS_ENABLE)
		sprintf(str1,"%s","Running");		
    else if(psess->lm_enable == OAM_STATUS_FINISH)
		sprintf(str1,"%s","Finish");		
    else if(psess->lm_enable == OAM_STATUS_DISABLE)
    	sprintf(str1,"%s","Disable");		

	vty_out(vty," %-19s: %s%s","LM-state",str1,VTY_NEWLINE);

	//dm state
    if(psess->dm_enable == OAM_STATUS_ENABLE)
		sprintf(str1,"%s","Running");		
    else if(psess->dm_enable == OAM_STATUS_FINISH)
		sprintf(str1,"%s","Finish");		
    else if(psess->dm_enable == OAM_STATUS_DISABLE)
		sprintf(str1,"%s","Disable");		

	vty_out(vty," %-19s: %s%s","DM-state",str1,VTY_NEWLINE);
    
	return ;
}

static void cfm_show_session(struct vty *vty ,uint16_t sess_index)
{
	struct cfm_sess    *psess   = NULL;
    struct hash_bucket *pbucket = NULL;
    int 				cursor;
	unsigned  short 	dis_count=0,en_count=0,up_count=0,down_count=0;
	
	if(sess_index)
	{
		psess = cfm_session_lookup_new(sess_index);
		if(psess)
			cfm_show_session_node(vty,psess);		
		else
			vty_out(vty,"Info: The specified session does not exist. %s",VTY_NEWLINE);

		return;
	}

	vty_out(vty,"The total number of sessions is %d %s",cfm_sess_table_new.num_entries,VTY_NEWLINE);

	if(!cfm_sess_table_new.num_entries)
		return;

	HASH_BUCKET_LOOP(pbucket,cursor,cfm_sess_table_new)
	{
		psess = pbucket->data;
		
		if(psess->state == OAM_STATUS_DISABLE)
		{
			dis_count++;
		}
		else if(psess->state == OAM_STATUS_ENABLE)
		{
			en_count++;
		}
		else if(psess->state == OAM_STATUS_UP)
		{
			up_count++;
		}
		else if(psess->state == OAM_STATUS_DOWN)
		{
			down_count++;
		}		
	}

	vty_out(vty,"up:%d , down:%d , enable:%d , disable:%d  %s",up_count,down_count,en_count,dis_count,VTY_NEWLINE);
	

	HASH_BUCKET_LOOP(pbucket,cursor,cfm_sess_table_new)
	{
		psess = pbucket->data;
		
		if(psess)
		{
			vty_out(vty,"------------------------------------------------ %s",VTY_NEWLINE);
			cfm_show_session_node(vty,psess);	
		}
	}
	
	
}

static void cfm_show_session_sla(struct vty *vty, struct cfm_sess *psess)
{   
    char f_max[10] 		 = {0};
    char f_min[10] 		 = {0};
    char f_mean[10] 	 = {0};
    char f_max_peer[10]  = {0};
    char f_min_peer[10]  = {0};
    char f_mean_peer[10] = {0};
	char str[300];
	char str1[50];
    
    if(NULL == psess || NULL == psess->psla)
    {
        return ;
    }
	
    vty_out(vty, "---------------------------------------------------------------%s", VTY_NEWLINE);
    vty_out(vty, "%-7s%-7s%-12s%-10s%-13s%-9s%-13s%s", "MD-Id", "MA-Id", "SESSION-Id", "LM-state", "LM-interval", "DM-state", "DM-interval",VTY_NEWLINE);
	//              1   2   3    4     5   6    7
	sprintf(str1,"%d",psess->md_index);
	sprintf(str,"%-7s",str1);

	sprintf(str1,"%d",psess->ma_index);
	sprintf(str+strlen(str),"%-7s",str1);

	sprintf(str1,"%d",psess->sess_id);
	sprintf(str+strlen(str),"%-12s",str1);
	
    if(psess->lm_enable == OAM_STATUS_ENABLE)
    {
		sprintf(str+strlen(str),"%-10s","Running");		
		sprintf(str1,"%ds",psess->lm_interval);
		sprintf(str+strlen(str),"%-13s",str1);
		
    }
    else if(psess->lm_enable == OAM_STATUS_DISABLE)
    {       	
		sprintf(str+strlen(str),"%-10s","Disable");		
		sprintf(str+strlen(str),"%-13s","--");
    }
    else if(psess->lm_enable == OAM_STATUS_FINISH)
    {
		sprintf(str+strlen(str),"%-10s","Finish");		
		sprintf(str1,"%ds",psess->lm_interval);
		sprintf(str+strlen(str),"%-13s",str1);
		
    }

    if(psess->dm_enable == OAM_STATUS_ENABLE)
    {
		sprintf(str+strlen(str),"%-9s","Running");		
		sprintf(str1,"%ds",psess->dm_interval);
		sprintf(str+strlen(str),"%-13s",str1);
		
    }
    else if(psess->dm_enable == OAM_STATUS_DISABLE)
    {       	
		sprintf(str+strlen(str),"%-9s","Disable");		
		sprintf(str+strlen(str),"%-13s","--");
    }
    else if(psess->dm_enable == OAM_STATUS_FINISH)
    {
		sprintf(str+strlen(str),"%-9s","Finish");		
		sprintf(str1,"%ds",psess->dm_interval);
		sprintf(str+strlen(str),"%-13s",str1);
		
    }

	
	vty_out(vty, "%s%s",str, VTY_NEWLINE);
	vty_out(vty, "---------------------------------------------------------------%s", VTY_NEWLINE);
	
    if(psess->lm_enable == OAM_STATUS_DISABLE)
    {
		vty_out(vty, "%-32s: %s%s", "Max frame loss ratio(near-end)", "--", VTY_NEWLINE);
		vty_out(vty, "%-32s: %s%s", "Min frame loss ratio(near-end)", "--", VTY_NEWLINE);
		vty_out(vty, "%-32s: %s%s", "Mean frame loss ratio(near-end)", "--", VTY_NEWLINE);
		vty_out(vty, "%-32s: %s%s", "Max frame loss ratio(far-end)", "--", VTY_NEWLINE);
		vty_out(vty, "%-32s: %s%s", "Min frame loss ratio(far-end)", "--", VTY_NEWLINE);
		vty_out(vty, "%-32s: %s%s", "Mean frame loss ratio(far-end)", "--", VTY_NEWLINE);
	}
	else
	{
		sprintf(f_max, "%d.%02d", psess->psla->lm_max/100, psess->psla->lm_max%100);
		sprintf(f_min, "%d.%02d", psess->psla->lm_min/100, psess->psla->lm_min%100);
		sprintf(f_mean, "%d.%02d", psess->psla->lm_mean/100, psess->psla->lm_mean%100);
		sprintf(f_max_peer, "%d.%02d", psess->psla->lm_max_peer/100, psess->psla->lm_max_peer%100);
		sprintf(f_min_peer, "%d.%02d", psess->psla->lm_min_peer/100, psess->psla->lm_min_peer%100);
		sprintf(f_mean_peer, "%d.%02d", psess->psla->lm_mean_peer/100, psess->psla->lm_mean_peer%100);
		
		vty_out(vty, "%-32s: %s%%%s", "Max frame loss ratio(near-end)", f_max, VTY_NEWLINE);
		vty_out(vty, "%-32s: %s%%%s", "Min frame loss ratio(near-end)", f_min, VTY_NEWLINE);
		vty_out(vty, "%-32s: %s%%%s", "Mean frame loss ratio(near-end)", f_mean, VTY_NEWLINE);
		vty_out(vty, "%-32s: %s%%%s", "Max frame loss ratio(far-end)", f_max_peer, VTY_NEWLINE);
		vty_out(vty, "%-32s: %s%%%s", "Min frame loss ratio(far-end)", f_min_peer, VTY_NEWLINE);
		vty_out(vty, "%-32s: %s%%%s", "Mean frame loss ratio(far-end)", f_mean_peer, VTY_NEWLINE);
		//vty_out(vty, "%s", VTY_NEWLINE);
	}

    if(psess->dm_enable == OAM_STATUS_DISABLE)
    {   
        vty_out(vty, "%-32s: %s%s", "The Max Delay(ns)", "--", VTY_NEWLINE);
        vty_out(vty, "%-32s: %s%s", "The Max Delay Jitter(ns)", "--", VTY_NEWLINE);
        vty_out(vty, "%-32s: %s%s", "The Min Delay(ns)", "--", VTY_NEWLINE);
        vty_out(vty, "%-32s: %s%s", "The Min Delay Jitter(ns)", "--", VTY_NEWLINE);
        vty_out(vty, "%-32s: %s%s", "The Mean Delay(ns)", "--", VTY_NEWLINE);
        vty_out(vty, "%-32s: %s%s", "The Mean Delay Jitter(ns)", "--", VTY_NEWLINE);

    }
	else
	{
		vty_out(vty, "%-32s: %u%s", "The Max Delay(ns)", psess->psla->dm_max, VTY_NEWLINE);
		vty_out(vty, "%-32s: %u%s", "The Max Delay Jitter(ns)", psess->psla->jitter_max, VTY_NEWLINE);
		vty_out(vty, "%-32s: %u%s", "The Min Delay(ns)", psess->psla->dm_min, VTY_NEWLINE);
		vty_out(vty, "%-32s: %u%s", "The Min Delay Jitter(ns)", psess->psla->jitter_min, VTY_NEWLINE);
		vty_out(vty, "%-32s: %u%s", "The Mean Delay(ns)", psess->psla->dm_mean, VTY_NEWLINE);
		vty_out(vty, "%-32s: %u%s", "The Mean Delay Jitter(ns)", psess->psla->jitter_mean, VTY_NEWLINE);		
	}

    
    return ;
}



static void cfm_show_session_test(struct vty *vty, struct cfm_sess *psess)
{   
    if(NULL == vty || NULL == psess)
    {
        return ;
    }
	vty_out(vty, "tx_num = %d , rx_num/error_num = %d/%d %s",psess->tst_next_trans_id,  psess->tst_rxnum, psess->tst_errnum,VTY_NEWLINE);		
	return;
}


DEFUN(show_cfm_md,
		show_cfm_md_cmd,
		"show cfm md [<1-8>]",
		SHOW_STR
		"Connectivity fault management\n"
		"Maintenance domain\n"
		"Md index,<1-8>\n")
{
	uint8_t md_index = 0;

	if(argv[0])
		md_index = atoi(argv[0]);
		
	cfm_show_md(vty,md_index);
		
	return CMD_SUCCESS;
}

DEFUN(show_cfm_ma,
		show_cfm_ma_cmd,
		"show cfm ma [<1-1024>]",
		SHOW_STR
		"Connectivity fault management\n"
		"Maintenance association\n"
		"Ma index,<1-1024>\n")
{
	uint16_t ma_index = 0;

	if(argv[0])
		ma_index = atoi(argv[0]);
		
	cfm_show_ma(vty,ma_index);
		
	return CMD_SUCCESS;
}

DEFUN(show_cfm_session,
		show_cfm_session_cmd,
		"show cfm session [<1-1024>]",
		SHOW_STR
		"Connectivity fault management\n"
		"Session\n"
		"Session index,<1-1024>\n")
{
	uint16_t sess_index = 0;

	if(argv[0])
		sess_index = atoi(argv[0]);
		
	cfm_show_session(vty,sess_index);
		
	return CMD_SUCCESS;
}

DEFUN(show_cfm_lm_dm_result,
	show_cfm_lm_dm_result_cmd,
	"show cfm session <1-1024> (sla|test)",
	SHOW_STR
	"Connectivity fault management\n"
	"Session\n"
	"Range of session index:<1-1024>\n"
	"Service Level Agreement\n"
    "Eth-test\n")
{
	struct cfm_sess *psess	 = NULL;
	uint16_t 		 sess_id = 0;

	sess_id = atoi(argv[0]);
    psess   = cfm_session_lookup_new(sess_id);
	if(psess == NULL)
	{
		vty_error_out(vty,"Session:%d is not exist!%s", sess_id, VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(!strncmp(argv[1],"sla",1))
	{
		cfm_show_session_sla(vty, psess);
	}
	else 
	{
		cfm_show_session_test(vty, psess);
	}

    
    return CMD_SUCCESS;
}

extern void zlog_debug_set(struct vty *vty, unsigned int type, int enable);
DEFUN (cfm_debug_fun,
	cfm_debug_cmd,
	"debug cfm (enable|disable) (lb|lt|test|common|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"CFM debug enable\n"
	"CFM debug disable\n"
	"CFM debug type lb\n"
	"CFM debug type lt\n"
	"CFM debug type test\n"
	"CFM debug type common\n"
	"CFM debug type all\n")
{
	unsigned int zlog_num = 0;
	
	for( ; zlog_num < array_size(cfm_dbg_name); zlog_num++)
	{
		if(!strncmp(argv[1], cfm_dbg_name[zlog_num].str, 2))
		{
			zlog_debug_set( vty, cfm_dbg_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

			return CMD_SUCCESS;
		}
	}

	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN (show_cfm_debug_fun,
         show_cfm_debug_cmd,
         "show cfm debug",
         SHOW_STR
         "Cfm"
         "Debug status\n")
{
         unsigned int type_num = 0;
         
         vty_out(vty, "debug type         status %s", VTY_NEWLINE);

         for(type_num = 0; type_num < array_size(cfm_dbg_name); ++type_num)
         {
                   vty_out(vty, "%-15s    %-10s %s", cfm_dbg_name[type_num].str, 
                            !!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
         }

         return CMD_SUCCESS;
}

static int cfm_md_config_write ( struct vty *vty )
{
	int 			 i	 = 0;
	struct cfm_md   *pmd = NULL;
	struct cfm_ma   *pma = NULL;
	struct listnode *pnode;
	
	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	
	for(i=0 ; i<CFM_MD_NUM_MAX ; i++)
	{
		if(cfm_md_table[i] != NULL)
		{	
			pmd = cfm_md_table[i];
		
			vty_out(vty,"cfm md %d %s",i+1,VTY_NEWLINE);
			if(pmd->name[0])
			{
				vty_out(vty," name %s %s",pmd->name,VTY_NEWLINE);
			}
			
			if(pmd->level)
			{
				vty_out(vty," level %d %s",pmd->level,VTY_NEWLINE);
			}

			pnode = pmd->ma_list.head;
			while(pnode)
			{
				pma = listgetdata(pnode);
				vty_out(vty," ma %d %s",pma->ma_index,VTY_NEWLINE);

				if(pma->y1731_en)
				{
					vty_out(vty,"  protocol y.1731 %s",VTY_NEWLINE);
				}

				if(pma->name[0])
				{
					vty_out(vty,"  name %s %s",pma->name,VTY_NEWLINE);
				}
				
				if(pma->vlan)
				{
					vty_out(vty,"  service vlan %d %s",pma->vlan,VTY_NEWLINE);
				}
				
				if(pma->priority!=5)
				{
					vty_out(vty,"  priority %d %s",pma->priority,VTY_NEWLINE);
				}

				if(pma->mip_enable)
				{
					vty_out(vty,"  mip enable %s",VTY_NEWLINE);
				}
				
				pnode = pnode->next;
			}

		}
	}

	return 0;
}


static int cfm_session_config_write ( struct vty *vty )
{
	struct cfm_sess    *psess 	= NULL;
    struct hash_bucket *pbucket = NULL;
    int 				cursor;
	uint8_t 		   *mac 	= NULL;
	char 				name[50];
	uint8_t 			i;
	
	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	
	HASH_BUCKET_LOOP(pbucket,cursor,cfm_sess_table_new)
	{
		psess = pbucket->data;
		vty_out(vty,"cfm session %d %s",psess->sess_id,VTY_NEWLINE);

		if(psess->local_mep)
		{
			vty_out(vty," local-mep %d  %s",psess->local_mep,VTY_NEWLINE);
		}
		
		if(psess->ma_index)
		{
			vty_out(vty," service ma %d %s",psess->ma_index,VTY_NEWLINE);
		}

		if(psess->direct == MEP_DIRECT_UP)
		{
			if(psess->over_pw)
				vty_out(vty," direction up over_pw%s",VTY_NEWLINE);
			else
				vty_out(vty," direction up %s",VTY_NEWLINE);
		}

		if(psess->remote_mep)
		{
			mac = psess->dmac;
			if(!psess->dmac_valid)				
				vty_out(vty," remote-mep %d  %s",psess->remote_mep,VTY_NEWLINE);
			else
				vty_out(vty," remote-mep %d mac %02X:%02X:%02X:%02X:%02X:%02X %s",psess->remote_mep,mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],VTY_NEWLINE);
		}

		if(psess->ifindex)
		{			
			ifm_get_name_by_ifindex(psess->ifindex,name);
			vty_out ( vty, " cfm enable  interface %s %s", name,VTY_NEWLINE );
		}

		if(psess->cc_enable)
		{
			if(psess->cc_interval == 3)
				vty_out(vty," cc enable 3ms %s",VTY_NEWLINE);
			else if(psess->cc_interval == 10)
				vty_out(vty," cc enable 10ms %s",VTY_NEWLINE);
			else if(psess->cc_interval == 100)
				vty_out(vty," cc enable 100ms %s",VTY_NEWLINE);
			else if(psess->cc_interval == 1000)
				vty_out(vty," cc enable 1s %s",VTY_NEWLINE);
			else if(psess->cc_interval == 10000)
				vty_out(vty," cc enable 10s %s",VTY_NEWLINE);
			else if(psess->cc_interval == 60000)
				vty_out(vty," cc enable 60s %s",VTY_NEWLINE);
			else if(psess->cc_interval == 600000)
				vty_out(vty," cc enable 600s %s",VTY_NEWLINE);
			
		}

		if(psess->ais_enable)
		{
			for(i=0;i<8;i++)
			{
				if(psess->ais_send_level & (0x01<<i))
				{
					vty_out(vty," ais enable interval %ds level %d %s",psess->ais_send_interval,i,VTY_NEWLINE);
				}
			}
		}

		if(psess->tst_mode == 1)//out-of-service
		{
			for(i=0;i<8;i++)
			{
				if(psess->lck_send_level & (0x01<<i))
				{
					vty_out(vty," test mode out-of-service lck-interval %ds lck-level %d %s",psess->lck_send_interval,i,VTY_NEWLINE);
				}
			}
		}
	}

	return 0;
	
}


void cfm_cmd_init(void)
{
	install_node (&cfm_md_node, cfm_md_config_write);
	install_node (&cfm_ma_node, NULL);
	install_node (&cfm_session_node, cfm_session_config_write);
		
	install_default (CFM_MD_NODE);
	install_default (CFM_MA_NODE);
	install_default (CFM_SESSION_NODE);

	install_element (CONFIG_NODE, &cfm_debug_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_cfm_debug_cmd,CMD_LOCAL);

	install_element (CONFIG_NODE, &md_view_cmd,CMD_SYNC);
	install_element (CONFIG_NODE, &no_md_view_cmd,CMD_SYNC);

	install_element (CFM_MD_NODE, &md_name_cmd,CMD_SYNC);
	install_element (CFM_MD_NODE, &md_level_cmd,CMD_SYNC);
	install_element (CFM_MD_NODE, &no_md_level_cmd,CMD_SYNC);
	install_element (CFM_MD_NODE, &ma_view_cmd,CMD_SYNC);
	install_element (CFM_MD_NODE, &no_ma_view_cmd,CMD_SYNC);

	install_element (CFM_MA_NODE, &ma_name_cmd,CMD_SYNC);
	install_element (CFM_MA_NODE, &no_ma_name_cmd,CMD_SYNC);	
	install_element (CFM_MA_NODE, &ma_service_vlan_cmd,CMD_SYNC);
	install_element (CFM_MA_NODE, &no_ma_service_vlan_cmd,CMD_SYNC);
	install_element (CFM_MA_NODE, &ma_priority_cmd,CMD_SYNC);
	install_element (CFM_MA_NODE, &no_ma_priority_cmd,CMD_SYNC);
	install_element (CFM_MA_NODE, &ma_mip_enable_cmd,CMD_SYNC);
	install_element (CFM_MA_NODE, &no_ma_mip_enable_cmd,CMD_SYNC);
	install_element (CFM_MA_NODE, &ma_1731_enable_cmd,CMD_SYNC);
	install_element (CFM_MA_NODE, &no_ma_1731_enable_cmd,CMD_SYNC);

	install_element (CONFIG_NODE, &session_name_cmd,CMD_SYNC);
	install_element (CONFIG_NODE, &no_session_name_cmd,CMD_SYNC);
	install_element (CFM_SESSION_NODE, &session_service_ma_cmd,CMD_SYNC);
	install_element (CFM_SESSION_NODE, &no_session_service_ma_cmd,CMD_SYNC);
	install_element (CFM_SESSION_NODE, &session_direction_down_cmd,CMD_SYNC);	
	install_element (CFM_SESSION_NODE, &session_direction_up_cmd,CMD_SYNC);
	install_element (CFM_SESSION_NODE, &no_session_direction_cmd,CMD_SYNC);	
	install_element (CFM_SESSION_NODE, &session_localmep_cmd,CMD_SYNC);
	install_element (CFM_SESSION_NODE, &no_session_localmep_cmd,CMD_SYNC);
	install_element (CFM_SESSION_NODE, &session_rmep_cmd,CMD_SYNC);
	install_element (CFM_SESSION_NODE, &no_session_rmep_cmd,CMD_SYNC);
	install_element (CFM_SESSION_NODE, &session_ccm_en_cmd,CMD_SYNC);
	install_element (CFM_SESSION_NODE, &no_session_ccm_en_cmd,CMD_SYNC);

    install_element (CFM_SESSION_NODE, &cfm_lb_enable_cmd,CMD_LOCAL);
    install_element (CFM_SESSION_NODE, &no_cfm_lb_enable_cmd,CMD_LOCAL);
	
    install_element (CFM_SESSION_NODE, &cfm_lt_enable_cmd,CMD_LOCAL);
    install_element (CFM_SESSION_NODE, &no_cfm_lt_enable_cmd,CMD_LOCAL);
	
    install_element (CFM_SESSION_NODE, &cfm_lm_enable_cmd,CMD_LOCAL);
    install_element (CFM_SESSION_NODE, &no_cfm_lm_enable_cmd,CMD_LOCAL);
    install_element (CFM_SESSION_NODE, &cfm_dm_enable_cmd,CMD_LOCAL);
    install_element (CFM_SESSION_NODE, &no_cfm_dm_enable_cmd,CMD_LOCAL);
    
	install_element (CONFIG_NODE, &show_cfm_md_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_cfm_ma_cmd,CMD_LOCAL);
	install_element (CONFIG_NODE, &show_cfm_session_cmd,CMD_LOCAL);
    install_element (CONFIG_NODE, &show_cfm_lm_dm_result_cmd,CMD_LOCAL);

	install_element (CFM_SESSION_NODE, &cfm_enable_interface_cmd,CMD_SYNC);	
	install_element (CFM_SESSION_NODE, &no_cfm_enable_interface_cmd,CMD_SYNC);

	install_element (CFM_SESSION_NODE, &cfm_enable_interface_trunk_cmd,CMD_SYNC);	
	install_element (CFM_SESSION_NODE, &no_cfm_enable_interface_trunk_cmd,CMD_SYNC);

	install_element (CFM_SESSION_NODE, &cfm_ais_enable_cmd,CMD_SYNC);
	install_element (CFM_SESSION_NODE, &no_cfm_ais_enable_cmd,CMD_SYNC);
	
	install_element (CFM_SESSION_NODE, &cfm_tst_mode_in_service_cmd,CMD_SYNC);
	install_element (CFM_SESSION_NODE, &cfm_tst_mode_out_service_cmd,CMD_SYNC);
	install_element (CFM_SESSION_NODE, &cfm_test_enable_cmd,CMD_LOCAL);
	install_element (CFM_SESSION_NODE, &no_cfm_test_enable_cmd,CMD_LOCAL);
}

