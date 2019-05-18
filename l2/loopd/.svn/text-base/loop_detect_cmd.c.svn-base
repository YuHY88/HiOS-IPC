/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*/

#include <lib/types.h>
#include <lib/memtypes.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/zassert.h>
#include <lib/module_id.h>
#include <lib/hash1.h>
#include "lib/thread.h"
#include "lib/log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lib/alarm.h>

#include "l2_if.h"
#include "loop_detect.h"

extern struct thread_master	*l2_master;

void loopdetect_info_show(struct vty *vty, struct l2if *pl2_if)
{
	char ifname[IFNET_NAMESIZE] = {0};

	if (NULL != pl2_if && pl2_if->ploopdetect != NULL)
	{
		vty_out (vty, "%-22s %-7s %-9s %-8s %-9s %-8s %10s%s","PortName","Vlan","Status","Action","Interval","Restore","Loopback",VTY_NEWLINE);
		vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
		
		ifm_get_name_by_ifindex(pl2_if->ifindex, ifname);
		vty_out (vty, "%-22s ", ifname);

		if (LOOPD_DEF_VLAN_ID == pl2_if->ploopdetect->vlan)
			vty_out (vty, "%-7s ", "Untag");
		else
			vty_out (vty, "%-7d ", pl2_if->ploopdetect->vlan);
		
		if (PORT_STATUS_BLOCK == pl2_if->ploopdetect->status)
			vty_out (vty, "%-9s ", "Block");
		else if (PORT_STATUS_SHUTDOWN == pl2_if->ploopdetect->status)
			vty_out (vty, "%-9s ", "Shutdown");
		else
			vty_out (vty, "%-9s ", "Forward");

		if (PORT_ACTION_BLOCK == pl2_if->ploopdetect->act)
			vty_out (vty, "%-6s ", "Block");
		else if (PORT_ACTION_SHUTDOWN == pl2_if->ploopdetect->act)
			vty_out (vty, "%-6s ", "Shutdown");
		else
			vty_out (vty, "%-6s ", "Invalid");
		
		vty_out (vty, "%6ds", pl2_if->ploopdetect->interval);
		vty_out (vty, "%8ds", pl2_if->ploopdetect->restore_time);
		vty_out (vty, "%10d%s", pl2_if->ploopdetect->loop_flag,VTY_NEWLINE);
		vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
		
		vty_out (vty, "%-17s %d%s", "Send pkt : ",pl2_if->ploopdetect->pkt_send,VTY_NEWLINE);
		vty_out (vty, "%-17s %d%s", "Recv pkt : ",pl2_if->ploopdetect->pkt_recv,VTY_NEWLINE);
		vty_out (vty, "%-17s %d%s", "Loopback times : ",pl2_if->ploopdetect->loop_times,VTY_NEWLINE);
		
		vty_out (vty, "--------------------------------------------------------------------------------%s",VTY_NEWLINE);
	}

	return ;
}
void loopdetect_clean_port(struct l2if *pif)
{
        if(pif->ploopdetect !=NULL)
	{
		    XFREE(MTYPE_L2_LOOPD, pif->ploopdetect);
        	}
		return ;
}
DEFUN(loopdetect_func,
	loopdetect_cmd,
	"loop-detect {vlan <1-4094>|interval <1-60>|restore-time <3-60000>| action (block|shutdown)}",
	"Loop detect\n"
	"Vlan node\n"
	"Vlanid <1-4094>\n"
	"Interval time\n"
	"Time in seconds\n"
	"Restore-time\n"
	"Time in seconds\n"
	"Loop action\n"
	"Action block\n"
	"Action shutdown\n"
	)
{
	uint32_t ifindex = 0;
	struct l2if *p_l2if = NULL;
	//uint8_t *mode = NULL;
	sint32 ret = 0;
	char ifname[IFNET_NAMESIZE] = {0};
	uint32_t interval = 0;
	uint32_t restore_time = 0;
	uint8_t mode = 0;

	/*~{TZ9~O#1mKQQ0~}ifindex~{6TS&=Z5c~},~{C;SPTr44=(~}*/
	ifindex = (uint32_t)vty->index;
	#if 0
	/*~{<l2i~}mode~{JG7qJG~}switch*/
	mode = ifm_get_mode(ifindex, MODULE_ID_L2);
	if(IFNET_MODE_SWITCH != *mode)
	{
		vty_error_out (vty, "Mode not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	#endif

    // 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
    ret = ifm_get_mode(ifindex, MODULE_ID_L2, &mode);
	if (ret)
	{
		vty_error_out (vty, "Mode not get %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
    if(IFNET_MODE_PHYSICAL == mode)
    {
        vty_error_out (vty, "the port not support loop-detect %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	p_l2if = l2if_get(ifindex);
	if(NULL == p_l2if)
	{
		zlog_err("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	if(p_l2if->trunkid)
	{
		vty_error_out (vty, "the port has been set as trunk member,loop-detect can't be set! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	/*ipran_alarm_port_register(IFM_FUN_ETH_TYPE, IFM_SLOT_ID_GET(p_l2if->ifindex), 
							IFM_PORT_ID_GET(p_l2if->ifindex), 0, 0); */
							
	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = p_l2if->ifindex;
	ipran_alarm_port_register(&gPortInfo);
	if (p_l2if->ploopdetect == NULL)
	{
		p_l2if->ploopdetect = (struct loop_detect*)XMALLOC(MTYPE_L2_LOOPD, sizeof(struct loop_detect));
		if(NULL == p_l2if->ploopdetect)
		{
			zlog_err("%s[%d]:leave %s:error:fail to XCALLOC struct loop_detect\n",__FILE__,__LINE__,__func__);
			return CMD_WARNING;
		}

		memset(p_l2if->ploopdetect, 0, sizeof(struct loop_detect));
		
		p_l2if->ploopdetect->vlan = LOOPD_DEF_VLAN_ID;
		p_l2if->ploopdetect->interval = LOOPD_DEF_INTERVAL;
		p_l2if->ploopdetect->restore_time = LOOPD_DEF_RES_TIME;
		p_l2if->ploopdetect->act  = PORT_ACTION_INVALID ;
	}
	else
	{

			LOOPDETECT_TIMER_OFF(p_l2if->ploopdetect->ptimer1);
			

			LOOPDETECT_TIMER_OFF(p_l2if->ploopdetect->ptimer2);

		
		ifm_get_name_by_ifindex(p_l2if->ifindex, ifname);
		
		if (p_l2if->ploopdetect->status == PORT_STATUS_BLOCK)
		{
			ret = ifm_set_block(p_l2if->ifindex, MODULE_ID_L2, IFNET_STP_FORWARD);
			if (ret < 0)
			{
				zlog_err("%s[%d]:%s:LPDT_LOOPED: set forward error on %s\n",__FILE__,__LINE__,__func__,ifname);
			}
			
			zlog_warn("%s[%d]:%s:LPDT_RECOVERED: Loopback on %s recovered\n",__FILE__,__LINE__,__func__,ifname);
		}
		
		if (p_l2if->ploopdetect->status == PORT_STATUS_SHUTDOWN)
		{
			ret = l2if_set_shutdown(p_l2if->ifindex, IFNET_NO_SHUTDOWN);
			if (ret < 0)
			{
				zlog_err("%s[%d]:%s:LPDT_LOOPED: set forward error on %s\n",__FILE__,__LINE__,__func__,ifname);
			}
			
			zlog_warn("%s[%d]:%s:LPDT_RECOVERED: Loopback on %s recovered\n",__FILE__,__LINE__,__func__,ifname);
		}	
		p_l2if->ploopdetect->status = PORT_STATUS_FORWARD;
		p_l2if->ploopdetect->loop_flag = LOOPD_FORWARD;
	}

	if (argv[0] != NULL)
		p_l2if->ploopdetect->vlan = atoi(argv[0]);

	if (argv[1] != NULL)
	{
		interval = atoi(argv[1]);
		
		if(argv[2] != NULL)
		{
			restore_time = atoi(argv[2]);
			
			if((interval*3) >restore_time)
			{
				vty_error_out (vty, "restore_time value not less than 3*interval value %s", VTY_NEWLINE);
										
			}
			else
			{  	
				p_l2if->ploopdetect->interval = interval;
				p_l2if->ploopdetect->restore_time = restore_time;
			}
		}
		else if((interval*3) > p_l2if->ploopdetect->restore_time)
		{
                     	vty_error_out (vty, "restore_time value not less than 3*interval value %s", VTY_NEWLINE);
					
		}
		else 
		{
			p_l2if->ploopdetect->interval = interval;
		}
		
		
	}
	
	else if(argv[2] != NULL)
	{
		restore_time = atoi(argv[2]);
		
		if((p_l2if->ploopdetect->interval * 3) > restore_time)
		{
				vty_error_out (vty, "restore_time value not less than 3*interval value %s", VTY_NEWLINE);
			
		}
		else 
		{
			p_l2if->ploopdetect->restore_time = restore_time;

		}
		
	}
	if (argv[3] != NULL)
	{
		if(0 == strncmp("bl", argv[3],strlen("bl")))
			p_l2if->ploopdetect->act  = PORT_ACTION_BLOCK;
		else if(0 == strncmp("sh", argv[3],strlen("sh")))
			p_l2if->ploopdetect->act = PORT_ACTION_SHUTDOWN;
	}

	
	L2_COMMON_LOG_DBG("%s[%d]:%s:vlan %d interval %d restore_time %d act %s\n\n",__FILE__,__LINE__,__func__, p_l2if->ploopdetect->vlan,p_l2if->ploopdetect->interval,p_l2if->ploopdetect->restore_time,p_l2if->ploopdetect->act == PORT_ACTION_BLOCK ? "block" : "shutdown");
	
	/*~{;7B7<l2b1(ND7"KM~}*/
	//p_l2if->ploopdetect->ptimer1 = thread_add_event(l2_master, loopdetect_pkt_send, p_l2if, 0);

	/*first time send once*/
	//LOOPDETECT_TIMER_ON_UNLOOP(p_l2if->ploopdetect->ptimer1,loopdetect_pkt_send,p_l2if,0); 
	loopdetect_pkt_send((void *)p_l2if);
	/*then Send it every once in a interval*/
	LOOPDETECT_TIMER_ON_LOOP(p_l2if->ploopdetect->ptimer1,loopdetect_pkt_send,p_l2if,p_l2if->ploopdetect->interval); 
	return CMD_SUCCESS;
}

DEFUN(no_loopdetect_func,
	no_loopdetect_cmd,
	"no loop-detect",
	"No command\n"
	"Loop-detect\n"
	)
{
	uint32_t ifindex = 0;
	struct l2if *p_l2if = NULL;
	//uint8_t *mode = NULL;
	sint32 ret = 0;
	char ifname[IFNET_NAMESIZE] = {0};

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	
	/*~{TZ9~O#1mKQQ0~}ifindex~{6TS&=Z5c~}*/
	ifindex = (uint32_t)vty->index;
	#if 0
	/*~{<l2i~}mode~{JG7qJG~}switch*/
	mode = ifm_get_mode(ifindex, MODULE_ID_L2);
	if(IFNET_MODE_SWITCH != *mode)
	{
		vty_error_out (vty, "Mode not switch %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	#endif
	p_l2if = l2if_lookup(ifindex);
	if(NULL == p_l2if)
	{
		zlog_err("%s[%d]:leave %s:error:fail to CALLOC if l2 table bucket\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}
	
	if (p_l2if->ploopdetect != NULL)
	{
		#if 0
		if(p_l2if->ploopdetect->ptimer1 != NULL)
		{
			thread_cancel(p_l2if->ploopdetect->ptimer1);
			p_l2if->ploopdetect->ptimer1 = NULL;
		}
		
		if(p_l2if->ploopdetect->ptimer2 != NULL)
		{
			thread_cancel(p_l2if->ploopdetect->ptimer2);
			p_l2if->ploopdetect->ptimer2 = NULL;
		}
		#endif
		if(p_l2if->ploopdetect->ptimer1)
		{
			LOOPDETECT_TIMER_OFF(p_l2if->ploopdetect->ptimer1);
		}
		
		if(p_l2if->ploopdetect->ptimer2)
		{
			LOOPDETECT_TIMER_OFF(p_l2if->ploopdetect->ptimer2);
		}
		/*~{;qH!6K?ZC{3F~}*/
		ifm_get_name_by_ifindex(p_l2if->ifindex, ifname);
		
		/*~{I>3};7B7<l2b#,;V846K?ZW4L,~}*/
		if (p_l2if->ploopdetect->status == PORT_STATUS_BLOCK)
		{
			ret = ifm_set_block(p_l2if->ifindex, MODULE_ID_L2, IFNET_STP_FORWARD);
			if (ret < 0)
			{
				zlog_err("%s[%d]:%s:LPDT_LOOPED: set forward error on %s\n",__FILE__,__LINE__,__func__,ifname);
			}
			
			zlog_warn("%s[%d]:%s:LPDT_RECOVERED: Loopback on %s recovered\n",__FILE__,__LINE__,__func__,ifname);
		}
		else if (p_l2if->ploopdetect->status == PORT_STATUS_SHUTDOWN)
		{
			ret = l2if_set_shutdown(p_l2if->ifindex, IFNET_NO_SHUTDOWN);
			if (ret < 0)
			{
				zlog_err("%s[%d]:%s:LPDT_LOOPED: set forward error on %s\n",__FILE__,__LINE__,__func__,ifname);
			}
			
			zlog_warn("%s[%d]:%s:LPDT_RECOVERED: Loopback on %s recovered\n",__FILE__,__LINE__,__func__,ifname);
		}
		//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(p_l2if->ifindex),IFM_PORT_ID_GET(p_l2if->ifindex),0,0,GPN_ALM_TYPE_FUN_ETH_LOOP_BACK,GPN_SOCK_MSG_OPT_CLEAN); 
		/*ipran_alarm_port_unregister(IFM_FUN_ETH_TYPE, IFM_SLOT_ID_GET(p_l2if->ifindex), 
							IFM_PORT_ID_GET(p_l2if->ifindex), 0, 0);*/
							
		gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
		gPortInfo.iIfindex = p_l2if->ifindex;
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LOOP_BACK, GPN_SOCK_MSG_OPT_CLEAN); 
		ipran_alarm_port_unregister(&gPortInfo);
		
		XFREE(MTYPE_L2_LOOPD, p_l2if->ploopdetect);
		p_l2if->ploopdetect = NULL;
	}
	
	return CMD_SUCCESS;
}

DEFUN(show_loopdetect,
	show_loopdetect_cmd,
	"show loop-detect interface {ethernet USP|gigabitethernet USP|xgigabitethernet USP}",
	"Show command\n"
	"Loop detect\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
	)
{
	uint32_t ifindex = 0;
	struct l2if *pl2_if = NULL;
	struct hash_bucket *pbucket = NULL;
	int cursor = 0;
	
	if (vty->node == CONFIG_NODE)
	{
		if (argv[0] != NULL)
		{
			ifindex = ifm_get_ifindex_by_name("ethernet", (char *)argv[0]);
			if (ifindex == 0)
			{
				vty_error_out (vty, "%%Command incomplete, please check out%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
		}
	
		else if (argv[1] != NULL)
		{
			ifindex = ifm_get_ifindex_by_name("gigabitethernet", (char *)argv[1]);
			if (ifindex == 0)
			{
				vty_error_out (vty, "%%Command incomplete, please check out%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
		}
		else if (argv[2] != NULL)
		{
			ifindex = ifm_get_ifindex_by_name("xgigabitethernet", (char *)argv[2]);
			if (ifindex == 0)
			{
				vty_error_out (vty, "%%Command incomplete, please check out%s", VTY_NEWLINE);
				return CMD_WARNING;
			}
		}
		else
		{
			HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
			{
				pl2_if = pbucket->data;
				loopdetect_info_show(vty, pl2_if);
			}
			
			return CMD_SUCCESS;
		}
	}
	else
	{
		ifindex = (uint32_t)vty->index;
	}
	
	/*~{TZ9~O#1mKQQ0~}ifindex~{6TS&=Z5c~}*/
	pl2_if = l2if_lookup(ifindex);
	loopdetect_info_show(vty, pl2_if);
	
	return CMD_SUCCESS;
}

ALIAS (show_loopdetect,
	show_loopdetect_interface_cmd,
	"show loop-detect",
	"show command\n"
	"loop detect\n")


DEFUN (loopdetect_pkt_debug,
	loopdetect_pkt_debug_cmd,
	"debug loop-detect packet",
	"Debug\n"
	"Loop detect\n"
	"Dump send and receive packet"
	)
{
	g_loopdetect_data.loopdetect_debug = 1;
	
	vty_out (vty, "debug loop-detect packet is enable%s",VTY_NEWLINE);
	
	return CMD_SUCCESS;
}

DEFUN (loopdetect_pkt_debug_disable,
	loopdetect_pkt_debug_disable_cmd,
	"no debug loop-detect packet",
	"No command\n"
	"Debug\n"
	"Loop detect\n"
	"Not dump send and receive packet"
	)
{
	g_loopdetect_data.loopdetect_debug = 0;
	
	vty_out (vty, "debug loop-detect packet is disable%s",VTY_NEWLINE);
	
	return CMD_SUCCESS;
}

void loopdetect_cli_init(void)
{
    install_element (PHYSICAL_IF_NODE, &loopdetect_cmd,CMD_SYNC);
    install_element (PHYSICAL_IF_NODE, &no_loopdetect_cmd,CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &show_loopdetect_interface_cmd,CMD_LOCAL);
    install_element (CONFIG_NODE, &loopdetect_pkt_debug_cmd,CMD_SYNC);
    install_element (CONFIG_NODE, &loopdetect_pkt_debug_disable_cmd,CMD_SYNC);
	install_element (CONFIG_NODE, &show_loopdetect_cmd,CMD_LOCAL);
}
