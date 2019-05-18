/******************************************************************************
 * Filename: gpnStatCmd.c
 *	Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.11.17  lipf created
 *
******************************************************************************/
#include <zebra.h>

#include "lib/memory.h"
#include "lib/vty.h"
#include "lib/command.h"
#include "lib/linklist.h"
#include "lib/ifm_common.h"
#include "lib/alarm.h"
#include "lib/log.h"

#include "socketComm/gpnGlobalPortIndexDef.h"

#include "gpnStatCmd.h"
#include "gpnStatCmdShow.h"
#include "gpnStatDebug.h"

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

static struct cmd_node statisCliNode = {STATIS_NODE, "%s(config-statis)# ", 1};

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
DEFUN (statis_mode_enable,
		statis_mode_enable_cmd,
		"statis",
		"statis command node\n")
{
	vty->node = STATIS_NODE;

	return CMD_SUCCESS;
}
		

DEFUN (statis_test_fun,
       statis_test_cmd,
       "statis test",
       "Statistic\n"
       "Test\n")
{    
    statTest(vty);
    return(CMD_SUCCESS);
}



DEFUN (statis_monitor_add_fun,
		statis_monitor_add_cmd,
		"statis port-type (eth|eth-sfp|equ|soft|env|vuni|e1|tunnel) USP",
		"Statistic\n"
		"Port-type\n"		
		"ETH\n"
		"ETH SFP\n"
		"EQU\n"
		"SOFT\n"
		"ENV\n"
		"VUNI\n"
		"E1\n"
		"TUNNEL\n"
		"USP\n")
{
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	gPortInfo.iIfindex = ifm_get_ifindex_by_name("ethernet", (char *)argv[1]);
	
	if(0 == memcmp("eth", argv[0], 3))
	{
		if(_SHR_PORT_IS_SUBPORT(gPortInfo.iIfindex))
		{
			gPortInfo.iAlarmPort = IFM_FUN_ETH_SUB_TYPE;
		}
		else
		{
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
		}		
	}
	else if(0 == memcmp("eth-sfp", argv[0], 4))
	{
		gPortInfo.iAlarmPort = IFM_ETH_SFP_TYPE;
	}
	else if(0 == memcmp("equ", argv[0], 3))
	{
		gPortInfo.iAlarmPort = IFM_EQU_TYPE;
	}
	else if(0 == memcmp("soft", argv[0], 4))
	{
		gPortInfo.iAlarmPort = IFM_SOFT_TYPE;
	}
	else if(0 == memcmp("env", argv[0], 3))
	{
		gPortInfo.iAlarmPort = IFM_ENV_TYPE;
	}
	else if(0 == memcmp("vuni", argv[0], 4))
	{
		gPortInfo.iAlarmPort = IFM_VUNI_TYPE;
	}
	if(0 == memcmp("e1", argv[0], 2))
	{
		gPortInfo.iAlarmPort = IFM_E1_TYPE;
	}
	else if(0 == memcmp("tunnel", argv[0], 3))
	{
		gPortInfo.iAlarmPort = IFM_TUNNEL_TYPE;
	}

	static int cmd_cnt = 0;
	if(0 == cmd_cnt)
	{
		vty_out(vty, "alarmPort(%d), iIfindex(%x : %d/%d/%d.%d %s", gPortInfo.iAlarmPort, 
			gPortInfo.iIfindex, IFM_UNIT_ID_GET(gPortInfo.iIfindex) ,IFM_SLOT_ID_GET(gPortInfo.iIfindex),
			IFM_PORT_ID_GET(gPortInfo.iIfindex), IFM_SUBPORT_ID_GET(gPortInfo.iIfindex), VTY_NEWLINE);
		ipran_alarm_port_register(&gPortInfo);
		cmd_cnt = 1;
		return CMD_SUCCESS;
	}
	cmd_cnt = 0;

	vty_out(vty, "alarmPort(%d), ifindex(%x)%s", gPortInfo.iAlarmPort, gPortInfo.iIfindex, VTY_NEWLINE);
	debug_statis_add_port_monitor(&gPortInfo);
	return(CMD_SUCCESS);
}


DEFUN (statis_monitor_add_fun2,
		statis_monitor_add_cmd2,
		"statis port-type (cfm|vs|vplspw|pw) <1-65535>",
		"Statistic\n"
		"Port-type\n"
		"CFM\n"
		"VS\n"
		"VPLSPW\n"
		"PW\n"
		"ID\n")
{
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	gPortInfo.iMsgPara1 = (uint32_t)atoi(argv[1]);
	
	if(0 == memcmp("cfm", argv[0], 3))
	{
		gPortInfo.iAlarmPort = IFM_CFM_MEP_TYPE;
	}
	else if(0 == memcmp("vs", argv[0], 2))
	{
		gPortInfo.iAlarmPort = IFM_VS_2_TYPE;
	}
	else if(0 == memcmp("vplspw", argv[0], 4))
	{
		gPortInfo.iAlarmPort = IFM_VPLSPW_TYPE;
	}
	else if(0 == memcmp("pw", argv[0], 2))
	{
		gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
	}

	static int cmd_cnt = 0;
	if(0 == cmd_cnt)
	{
		vty_out(vty, "alarmPort(%d), para(%d)%s", gPortInfo.iAlarmPort, gPortInfo.iMsgPara1, VTY_NEWLINE);
		ipran_alarm_port_register(&gPortInfo);
		cmd_cnt = 1;
		return CMD_SUCCESS;
	}
	cmd_cnt = 0;

	vty_out(vty, "alarmPort(%d), ID(%d)%s", gPortInfo.iAlarmPort, gPortInfo.iMsgPara1, VTY_NEWLINE);
	debug_statis_add_port_monitor(&gPortInfo);
	return(CMD_SUCCESS);
}


DEFUN (statis_show_register_info_fun,
		statis_show_register_info_cmd,
		"show statis register",
		SHOW_STR
		"Statis\n"
		"Register info\n")
{
	gpnStatShowRegisterInfo(vty);
	return CMD_SUCCESS;
}



/********************************************** debug ***********************************************/

const struct message statis_debug_name[] =
{
	{.key = STATIS_DBG_REGISTER, 		.str = "register"},
	{.key = STATIS_DBG_DATA_COLLECT,	.str = "data-collect"},
	{.key = STATIS_DBG_GET,				.str = "get"},
	{.key = STATIS_DBG_GET_NEXT, 		.str = "get-next"},
	{.key = STATIS_DBG_SET,				.str = "set"},
	{.key = STATIS_DBG_ALL,				.str = "all"}
};


DEFUN (statis_debug_monitor,
	statis_debug_monitor_cmd,
	"debug statis (enable|disable) (register|data-collect|get|get-next|set|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Type name of register messege\n"
	"Type name of data-collect messege\n"
	"Type name of get log queue \n"
	"Type name of get-next log queue \n"
	"Type name of set status\n"
	"Type name of all debug\n")
{
	unsigned int typeid = 0;
	int zlog_num;

	for(zlog_num = 0; zlog_num < array_size(statis_debug_name); zlog_num++)
	{
		if(!strncmp(argv[1], statis_debug_name[zlog_num].str, 4))
		{
			zlog_debug_set( vty, statis_debug_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

			return CMD_SUCCESS;
		}
	}

	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

/* æ˜¾ç¤ºä¸ªdebugä¿¡æ¯çš„çŠ¶æ€	*/
DEFUN (show_statis_debug_monitor,
	show_statis_debug_monitor_cmd,
	"show statis debug",
	SHOW_STR
	"Syslog"
	"Debug status\n")
{
	int type_num;

	vty_out(vty, "debug type		   status %s", VTY_NEWLINE);

	for(type_num = 0; type_num < array_size(statis_debug_name); ++type_num)
	{
		vty_out(vty, "%-15s  %-10s %s", statis_debug_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}


/*******************************************************************************************************/




/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
static int statis_config_write(struct vty *vty)
{
	return 0;  
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*------------------------------------------------------------*/
/*name: statis_cmd_init                                        */
/*desc: ¸æ¾¯Ä£¿éÃüÁîÐÐ³õÊ¼»¯                                  */
/*------------------------------------------------------------*/
int statis_cmd_init (void)
{
    install_node (&statisCliNode, statis_config_write);
	install_default (STATIS_NODE);

	/* aaa enter cmd */
	install_element (CONFIG_NODE, &statis_mode_enable_cmd, CMD_LOCAL);

	/* common cmd */
	install_element (STATIS_NODE, &statis_test_cmd, CMD_LOCAL);

	install_element (STATIS_NODE, &statis_monitor_add_cmd, CMD_LOCAL);
	install_element (STATIS_NODE, &statis_monitor_add_cmd2, CMD_LOCAL);
	install_element (STATIS_NODE, &statis_show_register_info_cmd, CMD_LOCAL);

	/* new debug */
	install_element (CONFIG_NODE, &statis_debug_monitor_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &show_statis_debug_monitor_cmd, CMD_SYNC);
    
    return(0);
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

