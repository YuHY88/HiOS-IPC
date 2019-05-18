#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/zassert.h>
#include <lib/vty.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <lib/types.h>
#include <lib/log.h>
#include <ifm/ifm.h>
#include <ftm/ftm_nhp.h>
#include "trunk.h"
#include "lacp/lacp.h"
#include "l2_if.h"
#include "mac_static.h"
#include "mstp/mstp_init.h"
#include "l2_msg.h"

struct cmd_node trunk_gloable_node =
{
    TRUNK_GLOABLE_NODE,
    "%s(config-trunk-gloable)# ",
    1
};
DEFUN( trunk_backup,
	trunk_backup_cmd,
	"backup non-failback",
	"Backup mode\n"
	"Interface no failback\n"
	)
{
    int ret = 0;
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
	struct l2if *l2_if = NULL;
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;
    int work_flag = 0;
    
    ifindex = ( uint32_t ) vty->index;
	l2_if = l2if_get(ifindex);
    if (NULL == l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }

    trunkid = IFM_TRUNK_ID_GET(ifindex);	
    /*获取trunkid对应的trunk表，没有则创建*/
    trunk_entry = trunk_lookup( trunkid );
    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

    if ((TRUNK_MODE_ECMP == trunk_entry->work_mode) ||
            (trunk_entry->failback != TRUNK_SWITCH_NOFAILBACK))
    {
        /*切换聚合模式，需将聚合成员删除*/
        if (trunk_entry->portlist.count > TRUNK_BACK_MEMBER_MAX)
        {
            vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_MEMBER_OUTOFNUM), VTY_NEWLINE);
            return L2_TRUNK_ERR_MEMBER_OUTOFNUM;
        }

        if (TRUNK_MODE_ECMP == trunk_entry->work_mode)
        {
            work_flag = 1;
        }
        
        /*发送至hal*/
        trunk_entry->work_mode = TRUNK_MODE_BACKUP;
        trunk_entry->failback = TRUNK_SWITCH_NOFAILBACK;
        trunk_entry->wtr = TRUNK_WTR_DEFAULT;
        //ret = ipc_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_WORK_MODE, IPC_OPCODE_UPDATE, 0 );
		 ret = l2_msg_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, 
		 					MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_WORK_MODE, IPC_OPCODE_UPDATE, 0 );
        if(ret != 0)
        {
            vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_HAL), VTY_NEWLINE);
            return L2_TRUNK_ERR_HAL;
        }
    }

    /*工作改变，重新进行端口选举*/
    if(work_flag)
    {
        if (TRUNK_DISABLE == trunk_entry->lacp_enable)
        {
            /*选择工作端口*/
            trunk_backup_port_select(trunk_entry);

            if (trunk_entry->portlist.count != TRUNK_BACK_MEMBER_MAX)

            {
                trunk_entry->status = BACKUP_STATUS_INVALID;
            }
            else
            {
                if (1 == trunk_entry->upport_num)
                {
				   for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
				   {
					   if(tport->linkstatus != IFNET_LINKUP)
					   {
						   continue;
					   }
			   
					   if(tport->ifindex == trunk_entry->master_if)
					   {
						   trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
					   }
					   else
					   {
						   trunk_backup_switch(trunk_entry, BACKUP_STATUS_SLAVE);
					   }
				   }
			   }
                else if (TRUNK_BACK_MEMBER_MAX == trunk_entry->upport_num)
			   {
				   trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
			   }
			   else
			   {
				   trunk_entry->status = BACKUP_STATUS_INVALID;
			   }
		   }
		   /*设置link状态*/
		   trunk_linkstatus_set(trunk_entry);
            trunk_speed_set(trunk_entry->trunkid);
		   /*上报/清除告警*/
		   trunk_member_alarm_report(trunk_entry->trunkid, trunk_entry->master_if);
       }
       else
       {
            /*选择工作端口*/
            trunk_backup_port_select(trunk_entry);
            for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
            {
                lacp_fsm(trunk_entry, tport->ifindex, LACP_EVENT_WORK_MODE);
				
				/*上报/清除告警*/
				trunk_member_alarm_report(trunk_entry->trunkid, tport->ifindex);
            }
        }

    }

    return CMD_SUCCESS;
}

DEFUN( trunk_backup_wtr,
	trunk_backup_wtr_cmd,
	"backup failback [wtr <0-3600>]",
	"Backup mode\n"
	"Interface failback\n"
	"Set wtr\n"
	"The default value is 30s. Please input an integer from 0 to 3600\n"
	)
{
    int ret = 0;
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
	struct l2if *l2_if = NULL;
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;
    int work_flag = 0;
    uint16_t wtr = 0;
    
    ifindex = ( uint32_t ) vty->index;
	l2_if = l2if_get(ifindex);
    if (NULL == l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }
	
    trunkid = IFM_TRUNK_ID_GET(ifindex);	
    /*获取trunkid对应的trunk表，没有则创建*/
    trunk_entry = trunk_lookup( trunkid );
    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

    if (NULL == argv[0])
    {
        wtr = TRUNK_WTR_DEFAULT;
    }
    else
    {
        VTY_GET_INTEGER_RANGE("wtr", wtr, argv[0], 0, 3600);
    }
        
        /*发送至hal*/
    if ((TRUNK_MODE_ECMP == trunk_entry->work_mode) ||
            (trunk_entry->failback != TRUNK_SWITCH_FAILBACK) ||
            (trunk_entry->wtr != wtr))
    {
        /*切换聚合模式，需将聚合成员删除*/
        if (trunk_entry->portlist.count > TRUNK_BACK_MEMBER_MAX)
        {
            vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_MEMBER_OUTOFNUM), VTY_NEWLINE);
            return L2_TRUNK_ERR_MEMBER_OUTOFNUM;
        }

        /*工作模式改变标志，重新选举工作端口*/
        if (TRUNK_MODE_ECMP == trunk_entry->work_mode)
        {
            work_flag = 1;
	    if(trunk_entry->trunk_under_interface_flag & TRUNK_UNDER_INTERFACE_ENABLE)
	    {
		trunk_entry->trunk_under_interface_flag = 0;
	    }
        }

        /*发送至hal*/
        trunk_entry->work_mode = TRUNK_MODE_BACKUP;
        trunk_entry->failback = TRUNK_SWITCH_FAILBACK;
        trunk_entry->wtr = wtr;
        //ret = ipc_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_WORK_MODE, IPC_OPCODE_UPDATE, 0);
		ret = l2_msg_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, 
		 					MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_WORK_MODE, IPC_OPCODE_UPDATE, 0 );
        if (ret != 0)
        {
            vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_HAL), VTY_NEWLINE);
            return L2_TRUNK_ERR_HAL;
        }
    }

    /*工作改变，重新进行端口选举*/
    if (work_flag)
    {
        if (TRUNK_DISABLE == trunk_entry->lacp_enable)
        {
            /*选择工作端口*/
            trunk_backup_port_select(trunk_entry);

            if (trunk_entry->portlist.count != TRUNK_BACK_MEMBER_MAX)

            {
                trunk_entry->status = BACKUP_STATUS_INVALID;
            }
            else
            {
                if (1 == trunk_entry->upport_num)
			   {
				   for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
				   {
					   if(tport->linkstatus != IFNET_LINKUP)
					   {
						   continue;
					   }
			   
					   if(tport->ifindex == trunk_entry->master_if)
					   {
						   trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
					   }
					   else
					   {
						   trunk_backup_switch(trunk_entry, BACKUP_STATUS_SLAVE);
					   }
				   }
			   }
                else if (TRUNK_BACK_MEMBER_MAX == trunk_entry->upport_num)
			   {
				   trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
			   }
			   else
			   {
				   trunk_entry->status = BACKUP_STATUS_INVALID;
			   }
		   }
		   /*设置link状态*/
		   trunk_linkstatus_set(trunk_entry);
            trunk_speed_set(trunk_entry->trunkid);
		   /*上报/清除告警*/
		   trunk_member_alarm_report(trunk_entry->trunkid, trunk_entry->master_if);
       }
       else
       {
            /*选择工作端口*/
            trunk_backup_port_select(trunk_entry);
            for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
            {
                lacp_fsm(trunk_entry, tport->ifindex, LACP_EVENT_WORK_MODE);
				
				/*上报/清除告警*/
                trunk_member_alarm_report(trunk_entry->trunkid, tport->ifindex);
            }
        }

    }

    return CMD_SUCCESS;

}

DEFUN( no_trunk_backup_wtr,
	no_trunk_backup_wtr_cmd,
	"no backup failback wtr",
	NO_STR
	"Backup mode\n"
	"Interface failback\n"
	"Set wtr\n"
	)
{
    struct trunk *trunk_entry = NULL;
	struct l2if *l2_if = NULL;
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;
    int ret = 0;

    ifindex = ( uint32_t ) vty->index;
	l2_if = l2if_get(ifindex);
    if (NULL == l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }

    /*获取trunkid对应的trunk表，没有则创建*/
    trunkid = IFM_TRUNK_ID_GET(ifindex);
    trunk_entry = trunk_lookup( trunkid );
    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

    /*负载分担方式下不可设置*/
    if (TRUNK_MODE_ECMP == trunk_entry->work_mode)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_ECMP_NO_SET), VTY_NEWLINE);
        return L2_TRUNK_ERR_ECMP_NO_SET;
    }

    /*设置回切时间，必须先设置回切方式*/
    if (TRUNK_SWITCH_FAILBACK == trunk_entry->failback)
    {
        if (trunk_entry->wtr != TRUNK_WTR_DEFAULT)
        {
            trunk_entry->wtr = TRUNK_WTR_DEFAULT;
            //ret = ipc_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_WORK_MODE, IPC_OPCODE_UPDATE, 0);
			ret = l2_msg_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, 
		 					MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_WORK_MODE, IPC_OPCODE_UPDATE, 0 );
            if (ret != 0)
            {
                vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_HAL), VTY_NEWLINE);
                return L2_TRUNK_ERR_HAL;
            }
        }
    }
    else
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOFAILBACK_NO_SET), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOFAILBACK_NO_SET;
    }

    return CMD_SUCCESS;
}

/*begin:added by yanjy*/
DEFUN(gloable_trunk_link_aggregation,
      gloable_trunk_link_aggregation_cmd,
      "link-aggregation global load-sharing mode (destination-ip|destination-mac|destination-port|ingress-port|source-ip|source-mac|source-port|source-destination-port)",
      "Gloable link-aggregation load-sharing mode\n"
      "Gloable link-aggregation\n"
      "Gloable link-aggregation load-sharing mode\n"
      "Load-sharing mode\n"
      "Destination ip\n"
      "Destination mac\n"
      "Destination port\n"
      "Ingress port\n"
      "Source ip\n"
      "Source mac\n"
      "Source port \n"
      "Source port and dest port\n"
    )
{
	int ret = 0, i= 0;
	struct trunk *trunk_entry = NULL;
	struct trunk_port *tport = NULL;
	struct listnode *node = NULL;
	uint32_t ecmp_mode = TRUNK_BALANCE_SMAC;
	int work_flag = 0;
	uint8_t trunk_count = 0;


	if (!strncmp(argv[0], "destination-ip", 14))
	{
		ecmp_mode = TRUNK_BALANCE_DIP;
	}
	else if (!strncmp(argv[0], "source-ip", 9))
	{
		ecmp_mode = TRUNK_BALANCE_SIP;
	}
	else if (!strncmp(argv[0], "destination-mac", 15))
	{
		ecmp_mode = TRUNK_BALANCE_DMAC;
	}
	else if (!strncmp(argv[0], "source-mac",10))
	{
		ecmp_mode = TRUNK_BALANCE_SMAC;
	}
	else if (!strncmp(argv[0], "source-destination-port", 10))
	{
		ecmp_mode = TRUNK_BALANCE_SPORT_DPORT;
	}
	else
	{
		/*NULL;*/
	}
	
	/*search the trunkid one by one.*/
	  for(i = 0; i < IFM_TRUNK_MAX; i++)
    	{
		trunk_entry = trunk_table[i];
		if(NULL == trunk_entry)
		{	
			continue;
		}
		trunk_count++;
		/*change the work_mode.from backup to ECMP.*/
		if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
		{
			vty_info_out(vty,"The interface trunk %d is in backup mode,please change.%s",trunk_entry->trunkid, VTY_NEWLINE);
			continue;
		}
		if(trunk_entry->trunk_under_interface_flag & TRUNK_UNDER_INTERFACE_ENABLE)
		{
			vty_out(vty, "The ecmp_mode has configed in the interface trunk %d node.%s",trunk_entry->trunkid, VTY_NEWLINE);
			continue;
		}

		if (trunk_entry->ecmp_mode != ecmp_mode)
		{
			trunk_gloable_config.gloable_balance_mode = ecmp_mode;
			trunk_entry->ecmp_mode = trunk_gloable_config.gloable_balance_mode;

			/*ret = ipc_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, 
									MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, 
									TRUNK_INFO_GLOABLE_ECMP_MODE, IPC_OPCODE_UPDATE, 0);*/
			 ret = l2_msg_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, 
										MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, 
										TRUNK_INFO_GLOABLE_ECMP_MODE, IPC_OPCODE_UPDATE, 0);
			/*if one trunk's configuration failed,just return.*/
			if (ret != 0)
			{
			    vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_HAL), VTY_NEWLINE);
			    return L2_TRUNK_ERR_HAL;
			}
			
		}
		/*if work as ECMP,whether the lacp_enable is necessary to judge.*/
		if (work_flag)
		{
			if (TRUNK_DISABLE == trunk_entry->lacp_enable)
			{
				trunk_ecmp_port_select(trunk_entry);
				trunk_linkstatus_set(trunk_entry);
				trunk_speed_set(trunk_entry->trunkid);

				for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
				{
				    	trunk_member_alarm_report(trunk_entry->trunkid, tport->ifindex);
				}
			}
			else
			{
				for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
				{
					lacp_fsm(trunk_entry, tport->ifindex, LACP_EVENT_WORK_MODE);
					trunk_member_alarm_report(trunk_entry->trunkid, tport->ifindex);
				}
			}
		}
	}
	if(0 == trunk_count)
	{
		vty_info_out(vty, "There is no interface trunk,please create first. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    return CMD_SUCCESS;
}
DEFUN(no_gloable_trunk_link_aggregation,
      no_gloable_trunk_link_aggregation_cmd,
      "undo link-aggregation global load-sharing mode",
      NO_STR
      "Link-aggregation load-sharing mode\n"
      "Link-aggregation load-sharing mode\n"
      "Link-aggregation load-sharing mode\n"
     )
{

	int ret = 0;
	struct trunk *trunk_entry = NULL;
	uint16_t   i=0;
	uint32_t ecmp_mode = TRUNK_BALANCE_SMAC;
	uint8_t trunk_count = 0;

	/*search the trunkid one by one.*/
	  for(i = 0; i < IFM_TRUNK_MAX; i++)
    	  {
		trunk_entry = trunk_table[i];
		 if(NULL == trunk_entry || (TRUNK_MODE_BACKUP == trunk_entry->work_mode))
		 {	 
			 continue;
		 }
		trunk_count++;
		 if(trunk_entry->trunk_under_interface_flag & TRUNK_UNDER_INTERFACE_ENABLE)
		{
			vty_out(vty, "The ecmp_mode has configed in the interface trunk %d node.%s",trunk_entry->trunkid, VTY_NEWLINE);
			continue;
		}
		 if (trunk_entry->ecmp_mode != ecmp_mode)
		{
			trunk_gloable_config.gloable_balance_mode = ecmp_mode;
			trunk_entry->ecmp_mode = trunk_gloable_config.gloable_balance_mode;

			
			/*ret = ipc_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, 
									MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK,
									TRUNK_INFO_GLOABLE_ECMP_MODE, IPC_OPCODE_UPDATE, 0);*/
			 ret = l2_msg_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, 
										MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, 
										TRUNK_INFO_GLOABLE_ECMP_MODE, IPC_OPCODE_UPDATE, 0);
			if (ret != 0)
			{
				vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_HAL), VTY_NEWLINE);
				return L2_TRUNK_ERR_HAL;
			}	
		 }
	}
	if(0 == trunk_count)
	{
		vty_info_out(vty, "There is no interface trunk,please create first. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	return CMD_SUCCESS;

}



DEFUN(gloable_trunk_load_balance,
      gloable_trunk_load_balance_cmd,
      "gloable-load-balance (label|sip-dip|dip|sip|dmac|smac|sport-dmac|smac-dmac |sport-dport)",
      "Gloable-load-balance mode\n"
      "Label\n"
      "Source ip and dest ip\n"
      "Dest ip\n"
      "Source ip\n"
      "Dest mac\n"
      "Source mac\n"
      "Source port and dest mac\n"
      "Source mac and dest mac\n"
      "Source port and dest port\n"
     )
{
	int ret = 0, i = 0;
	struct trunk *trunk_entry = NULL;
	struct trunk_port *tport = NULL;
	struct listnode *node = NULL;
	uint32_t ecmp_mode = TRUNK_BALANCE_SMAC;
	int work_flag = 0;
	uint8_t trunk_count = 0;
	
	if (!strncmp(argv[0], "l", 1))
	{
		ecmp_mode = TRUNK_BALANCE_LABEL;
	}
	else if (!strncmp(argv[0], "sip-", 4))
	{
		ecmp_mode = TRUNK_BALANCE_SIP_DIP;
	}
	else if (!strncmp(argv[0], "di", 2))
	{
		ecmp_mode = TRUNK_BALANCE_DIP;
	}
	else if (!strncmp(argv[0], "sip", 3))
	{
		ecmp_mode = TRUNK_BALANCE_SIP;
	}
	else if (!strncmp(argv[0], "dm", 2))
	{
		ecmp_mode = TRUNK_BALANCE_DMAC;
	}
	else if (!strncmp(argv[0], "smac-", 5))
	{
		ecmp_mode = TRUNK_BALANCE_SMAC_DMAC;
	}
	else if (!strncmp(argv[0], "smac", 4))
	{
		ecmp_mode = TRUNK_BALANCE_SMAC;
	}
	else if (!strncmp(argv[0], "sport-dm", 8))
	{
		ecmp_mode = TRUNK_BALANCE_SPORT_DMAC;
	}
	else if (!strncmp(argv[0], "sport-dp", 8))
	{
		ecmp_mode = TRUNK_BALANCE_SPORT_DPORT;
	}
	else
	{
		/*NULL*/
	}
	
	/*search the trunkid one by one.*/
	  for(i = 0; i < IFM_TRUNK_MAX; i++)
    	{
		trunk_entry = trunk_table[i];
		if(NULL == trunk_entry)
		{	
			continue;
		}
		trunk_count++;
		/*change the work_mode.from backup to ECMP.*/
		if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
		{
			vty_info_out(vty,"The interface trunk %d is in backup mode,please change.%s",trunk_entry->trunkid, VTY_NEWLINE);
			continue;
		}

		if(trunk_entry->trunk_under_interface_flag & TRUNK_UNDER_INTERFACE_ENABLE)
		{
			vty_out(vty, "The ecmp_mode has configed in the interface trunk %d node.%s",trunk_entry->trunkid, VTY_NEWLINE);
			continue;
		}

		if (trunk_entry->ecmp_mode != ecmp_mode)
		{
			trunk_gloable_config.gloable_balance_mode = ecmp_mode;
			trunk_entry->ecmp_mode = trunk_gloable_config.gloable_balance_mode;
			
			/*ret = ipc_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, 
									MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, 
									TRUNK_INFO_GLOABLE_ECMP_MODE, IPC_OPCODE_UPDATE, 0);*/
					 ret = l2_msg_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, 
									MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, 
									TRUNK_INFO_GLOABLE_ECMP_MODE, IPC_OPCODE_UPDATE, 0);
			/*if one trunk's configuration failed,just return.*/
			if (ret != 0)
			{
			    vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_HAL), VTY_NEWLINE);
			    return L2_TRUNK_ERR_HAL;
			}
			
		}
		/*if work as ECMP,whether the lacp_enable is necessary to judge.*/
		if (work_flag)
		{
			if (TRUNK_DISABLE == trunk_entry->lacp_enable)
			{
				trunk_ecmp_port_select(trunk_entry);
				trunk_linkstatus_set(trunk_entry);
				trunk_speed_set(trunk_entry->trunkid);

				for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
				{
				    	trunk_member_alarm_report(trunk_entry->trunkid, tport->ifindex);
				}
			}
			else
			{
				for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
				{
					lacp_fsm(trunk_entry, tport->ifindex, LACP_EVENT_WORK_MODE);
					trunk_member_alarm_report(trunk_entry->trunkid, tport->ifindex);
				}
			}
		}
	}
	if(0 == trunk_count)
	{
		vty_info_out(vty, "There is no interface trunk,please create first. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    return CMD_SUCCESS;

}

DEFUN(no_gloable_trunk_load_balance,
      no_gloable_trunk_load_balance_cmd,
      "no gloable-load-balance",
      NO_STR
      "Gloable-load-balance mode\n"
     )
{
	int ret = 0 , i= 0;
	struct trunk *trunk_entry = NULL;
	uint32_t ecmp_mode = TRUNK_BALANCE_SMAC;
	uint8_t trunk_count = 0;
	/*search the trunkid one by one.*/
	  for(i = 0; i < IFM_TRUNK_MAX; i++)
    	{
		trunk_entry = trunk_table[i];
		 if(NULL == trunk_entry || (TRUNK_MODE_BACKUP == trunk_entry->work_mode))
		 {	 
			 continue;
		 }
		 if(trunk_entry->trunk_under_interface_flag & TRUNK_UNDER_INTERFACE_ENABLE)
		{
			vty_out(vty, "The ecmp_mode has configed in the interface trunk %d node.%s",trunk_entry->trunkid, VTY_NEWLINE);
			continue;
		}

		  if (trunk_entry->ecmp_mode != ecmp_mode)
    		{
			 trunk_gloable_config.gloable_balance_mode = ecmp_mode;
	        trunk_entry->ecmp_mode = trunk_gloable_config.gloable_balance_mode;
		
	        /*ret = ipc_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, 
								MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK,
								TRUNK_INFO_GLOABLE_ECMP_MODE, IPC_OPCODE_UPDATE, 0);*/
		 ret = l2_msg_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, 
								MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK,
								TRUNK_INFO_GLOABLE_ECMP_MODE, IPC_OPCODE_UPDATE, 0);
		        if (ret != 0)
		        {
		            vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_HAL), VTY_NEWLINE);
			     	return L2_TRUNK_ERR_HAL;
		        }
				
		 }
	}
  	if(0 == trunk_count)
  	{
  		vty_info_out(vty, "There is no interface trunk,please create first. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
    return CMD_SUCCESS;

}

/*end: added by yanjy*/

DEFUN( trunk_load_balance,
	trunk_load_balance_cmd,
	"load-balance (label |sip-dip|dip|sip|dmac|smac|sport-dmac|smac-dmac |sport-dport)",
	"Load-balance mode\n"
	"Label\n"
	"Source ip and dest ip\n"
	"Dest ip\n"
	"Source ip\n"
	"Dest mac\n"
	"Source mac\n"
	"Source port and dest mac\n"
	"Source mac and dest mac\n"
	"Source port and dest port\n"
	)
{
    int ret = 0;
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
	struct l2if *l2_if = NULL;
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;
    uint32_t ecmp_mode = TRUNK_BALANCE_SMAC;
	uint8_t ecmp_mode_before = TRUNK_BALANCE_SMAC;
    int work_flag = 0;
    

    ifindex = ( uint32_t ) vty->index;
	l2_if = l2if_get(ifindex);
    if (NULL == l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }

    /*获取trunkid对应的trunk表，没有则创建*/
    trunkid = IFM_TRUNK_ID_GET(ifindex);
    trunk_entry = trunk_lookup( trunkid );
    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

    if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
    {
        /*清除定时器*/
        //THREAD_OFF(trunk_entry->fb_timer);

        /*发送至hal*/
        trunk_entry->work_mode = TRUNK_MODE_ECMP;	 
        //ret = ipc_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_WORK_MODE, IPC_OPCODE_UPDATE, 0 );
	    ret = l2_msg_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL,
	 							MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_WORK_MODE, IPC_OPCODE_UPDATE, 0 );
        if (ret != 0)
        {
            vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_HAL), VTY_NEWLINE);
            return L2_TRUNK_ERR_HAL;
        }

        work_flag = 1;
    }


    /*设置负载分担方式*/
    if (!strncmp(argv[0], "l", 1))
    {
        ecmp_mode = TRUNK_BALANCE_LABEL;
    }
    else if (!strncmp(argv[0], "sip-", 4))
    {
        ecmp_mode = TRUNK_BALANCE_SIP_DIP;
    }
    else if (!strncmp(argv[0], "di", 2))
    {
        ecmp_mode = TRUNK_BALANCE_DIP;
    }
    else if (!strncmp(argv[0], "sip", 3))
    {
        ecmp_mode = TRUNK_BALANCE_SIP;
    }
    else if (!strncmp(argv[0], "dm", 2))
    {
        ecmp_mode = TRUNK_BALANCE_DMAC;
    }
    else if (!strncmp(argv[0], "smac-", 5))
    {
        ecmp_mode = TRUNK_BALANCE_SMAC_DMAC;
    }
    else if (!strncmp(argv[0], "smac", 4))
    {
        ecmp_mode = TRUNK_BALANCE_SMAC;
    }
    else if (!strncmp(argv[0], "sport-dm", 8))
    {
        ecmp_mode = TRUNK_BALANCE_SPORT_DMAC;
    }
    else if (!strncmp(argv[0], "sport-dp", 8))
    {
        ecmp_mode = TRUNK_BALANCE_SPORT_DPORT;
    }
	ecmp_mode_before = trunk_entry->ecmp_mode;
    /*更新负载分担方式，并发送至hal*/
    if (trunk_entry->ecmp_mode != ecmp_mode)
    {
    	//trunk_gloable_config.trunk_under_interface_flag[trunkid] |= TRUNK_UNDER_INTERFACE_ENABLE;
	 trunk_entry->trunk_under_interface_flag = TRUNK_UNDER_INTERFACE_ENABLE;
          trunk_entry->ecmp_mode = ecmp_mode;
	
       //ret = ipc_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_ECMP_MODE, IPC_OPCODE_UPDATE, 0);
	   ret = l2_msg_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL,
	   							MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_ECMP_MODE, IPC_OPCODE_UPDATE, 0);

        if (ret != 0)
        {
        		trunk_entry->ecmp_mode = ecmp_mode_before;
		//trunk_gloable_config.trunk_under_interface_flag[trunkid] &= ~TRUNK_UNDER_INTERFACE_ENABLE;
		trunk_entry->trunk_under_interface_flag = 0;

		if(ret == L2_TRUNK_ERR_TRUNK_UN_CONFIGURABLE)
		{
			 trunk_entry->work_mode = TRUNK_MODE_BACKUP;
			 vty_info_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_TRUNK_UN_CONFIGURABLE), VTY_NEWLINE);
            		return L2_TRUNK_ERR_TRUNK_UN_CONFIGURABLE;
		}
		else
		{
			vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_HAL), VTY_NEWLINE);
			return L2_TRUNK_ERR_HAL;
		}	
        }
    }
    else
    {
	vty_info_out(vty, "%s %s", "This ecmp mode has been configed.", VTY_NEWLINE);
    }

    if (work_flag)
    {
        if (TRUNK_DISABLE == trunk_entry->lacp_enable)
        {
            trunk_ecmp_port_select(trunk_entry);
            /*设置link状态*/
            trunk_linkstatus_set(trunk_entry);
            trunk_speed_set(trunk_entry->trunkid);

            for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
            {
                /*上报/清除告警*/
                trunk_member_alarm_report(trunk_entry->trunkid, tport->ifindex);
            }
        }
        else
        {
            for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
            {
                lacp_fsm(trunk_entry, tport->ifindex, LACP_EVENT_WORK_MODE);

                /*上报/清除告警*/
                trunk_member_alarm_report(trunk_entry->trunkid, tport->ifindex);
            }
        }
    }

    return CMD_SUCCESS;

}

DEFUN( no_trunk_load_balance,
	no_trunk_load_balance_cmd,
	"no load-balance",
	NO_STR
	"Load-balance mode\n"
	)
{
    int ret = 0;
    struct trunk *trunk_entry = NULL;
	struct l2if *l2_if = NULL;
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;
    uint32_t ecmp_mode = TRUNK_BALANCE_SMAC;
	uint32_t ecmp_mode_before = TRUNK_BALANCE_SMAC;

    ifindex = (uint32_t) vty->index;
    l2_if = l2if_get(ifindex);

    if (NULL == l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }

    /*获取trunkid对应的trunk表，没有则创建*/
    trunkid = IFM_TRUNK_ID_GET(ifindex);
    trunk_entry = trunk_lookup(trunkid);

    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

    /*主备方式不可配*/
    if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_BACKUP_NO_SET), VTY_NEWLINE);
        return L2_TRUNK_ERR_BACKUP_NO_SET;
    }
   trunk_entry->trunk_under_interface_flag = 0;

    /*恢复负载分担方式为默认,发送至hal*/
    if (trunk_entry->ecmp_mode != ecmp_mode)
    {
    /*if gloable has configed before the interface,then recovery the gloable.*/
    	ecmp_mode_before = trunk_entry->ecmp_mode;
		
       	trunk_entry->ecmp_mode = trunk_gloable_config.gloable_balance_mode;
	 
        //ret = ipc_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_ECMP_MODE, IPC_OPCODE_UPDATE, 0);
		  ret = l2_msg_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, 
		  						MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_ECMP_MODE, IPC_OPCODE_UPDATE, 0);
        if (ret != 0)
        {
        		trunk_entry->ecmp_mode = ecmp_mode_before;
		trunk_entry->trunk_under_interface_flag = 0;
		
		if(ret == L2_TRUNK_ERR_TRUNK_UN_CONFIGURABLE)
		{
			trunk_entry->work_mode = TRUNK_MODE_BACKUP;
			 vty_info_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_TRUNK_UN_CONFIGURABLE), VTY_NEWLINE);
            		return L2_TRUNK_ERR_TRUNK_UN_CONFIGURABLE;
		}
		else
		{
			vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_HAL), VTY_NEWLINE);
			return L2_TRUNK_ERR_HAL;
		}
        }
    }
    else
    	{
		vty_info_out(vty, "%s %s", "The current ecmp mode is According to Source Mac.", VTY_NEWLINE);
	}

    return CMD_SUCCESS;

}

DEFUN( trunk_member_port_set,
	trunk_member_port_set_cmd,
	"trunk <1-128> {passive|priority <0-65535>}",
	"Set trunk member\n"
	"The value of trunk id\n"
	"Passive mode\n"
	"Port priority\n"
	"The default value is 32768. Please input an integer from 0 to 65535\n"
	)
{
    int ret = 0;
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct trunk_port *tport_tmp = NULL;
    struct listnode *node_tmp = NULL;
    struct l2if *l2_if = NULL;
    uint32_t trunk_ifindex = 0, ifindex = 0;
    uint16_t trunkid = 0;
    struct ifm_info ifm_info;
	struct l2if *trunk_l2_if = NULL;
	

    /*trunk id是否在有效范围内*/ 
    VTY_GET_INTEGER_RANGE ( "trunkid", trunkid, argv[0], 1, 128 );
    
    /*判断trunk是否已经存在*/
    trunk_ifindex = ifm_get_ifindex_by_name ( "trunk", (char *)argv[0] );
    
    ret = ifm_get_all_info(trunk_ifindex, MODULE_ID_L2, &ifm_info);
    if (ret)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_IF_NO_EXISTED), VTY_NEWLINE);
        return L2_TRUNK_ERR_IF_NO_EXISTED;
    }
    
	trunk_l2_if = l2if_get(trunk_ifindex);
    if (NULL == trunk_l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }
    /*获取trunkid对应的trunk结构体*/
    trunk_entry = trunk_lookup( trunkid );
    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

	/*管理口不支持*/
    ifindex = ( uint32_t ) vty->index;
    if (IFM_TYPE_IS_OUTBAND(ifindex))
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOSUPPORT_OUTBAND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOSUPPORT_OUTBAND;
    }

    /*获取接口索引，该接口是否已添加至其他聚合中*/
    l2_if = l2if_get(ifindex);
    if (NULL == l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }
    else
    {
        if ((l2_if->trunkid != 0) && (l2_if->trunkid != trunkid))
        {
            vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_MEMBER_USED), VTY_NEWLINE);
            return L2_TRUNK_ERR_MEMBER_USED;
        }
    }

    /*聚合组中不存在该端口，则添加*/
    tport = trunk_port_lookup(trunkid, ifindex);
    if(tport == NULL)
    {
        /*负载分担模式下最大端口数为8*/
        if (TRUNK_MODE_ECMP == trunk_entry->work_mode)
        {
            if (trunk_entry->portlist.count >= TRUNK_MEMBER_MAX)
            {
                vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_MEMBER_OUTOFNUM), VTY_NEWLINE);
                return L2_TRUNK_ERR_MEMBER_OUTOFNUM;
            }
        }
        /*主备模式下最大端口数为2*/
        else
        {
            if (trunk_entry->portlist.count >= TRUNK_BACK_MEMBER_MAX)
            {
                vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_MEMBER_OUTOFNUM), VTY_NEWLINE);
                return L2_TRUNK_ERR_MEMBER_OUTOFNUM;
            }
        }

        /*申请空间，并初始化*/
        tport = trunk_port_init(trunkid, ifindex);
        if (tport == NULL)
        {
            vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_MEMBER_INIT), VTY_NEWLINE);
            return L2_TRUNK_ERR_MEMBER_INIT;
        }

		/*将端口信息添加至trunk成员链表中*/
		ret = trunk_port_add(trunkid, tport);
        if (ret != 0)
        {
            XFREE(MTYPE_L2, tport);
            tport = NULL;

            /*HT201只支持4个成员*/
            if (ret == ERRNO_OVERSIZE)
            {
                vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_MEMBER_OUTOFNUM), VTY_NEWLINE);
                return L2_TRUNK_ERR_MEMBER_OUTOFNUM;
            }
            else
            {
                vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_MEMBER_ADD), VTY_NEWLINE);
                return L2_TRUNK_ERR_MEMBER_ADD;
            }
		}

		vty_info_out(vty,"Success to set,the original configuration has been clear.%s",VTY_NEWLINE);
    }

	/*设置端口的被动方式和端口优先级*/
    tport = trunk_port_lookup(trunkid, ifindex);
    if(tport != NULL)
    {
        if (argv[1] != NULL)
        {
            if (TRUNK_ENABLE == trunk_entry->lacp_enable)
            {
                if (tport->passive != TRUNK_ENABLE)
                {
                    tport->passive = TRUNK_ENABLE;
                    LACP_CLR_ACTIVITY(tport->link.actor_state);

                    if (IFNET_LINKUP == tport->linkstatus)
                    {
                        lacp_tx(trunk_entry, ifindex);
                    }
                }
            }
            else
            {
                vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_LACP_DISABLE), VTY_NEWLINE);
                //return L2_TRUNK_ERR_LACP_DISABLE;
            }
        }

	    /*设置端口优先级*/
        if (argv[2] != NULL)
        {
            if ((TRUNK_DISABLE == trunk_entry->lacp_enable) && (TRUNK_MODE_ECMP == trunk_entry->work_mode))
            {
                vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_MANUAL_ECMP_NO_SET), VTY_NEWLINE);
                //return L2_TRUNK_ERR_MANUAL_ECMP_NO_SET;
            }
            else
            {
                VTY_GET_INTEGER_RANGE("priority", tport->priority, argv[2], 0, 65535);

                /*选择主端口*/
                if (TRUNK_DISABLE == trunk_entry->lacp_enable)
                {
                    if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
                    {
                        trunk_backup_port_select(trunk_entry);

                        if (trunk_entry->portlist.count != TRUNK_BACK_MEMBER_MAX)

                        {
                            trunk_entry->status = BACKUP_STATUS_INVALID;
                        }
                        else
                        {
			                if(trunk_entry->upport_num == 1)
			                {
			                   for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
			                   {
			                       if(tport_tmp->linkstatus != IFNET_LINKUP)
			                       {
			                           continue;
			                       }

			                       if(tport_tmp->ifindex == trunk_entry->master_if)
			                       {
			                           trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
			                       }
                                    else
                                    {
                                        trunk_backup_switch(trunk_entry, BACKUP_STATUS_SLAVE);
                                    }
                                }
                            }
                            else if (TRUNK_BACK_MEMBER_MAX == trunk_entry->upport_num)
                            {
			                   trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
			                }
							else
							{
                                trunk_entry->status = BACKUP_STATUS_INVALID;
							}
						}
						
                        /*上报/清除告警*/
                        trunk_member_alarm_report(trunk_entry->trunkid, trunk_entry->master_if);
                    }
                }
                else
                {
                    if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
                    {
                        trunk_backup_port_select(trunk_entry);
                    }

                    lacp_pkt_tx(trunk_entry, ifindex);
                }
			}
	    }
    }
		/*modify by youcheng 2019/1/10 function:mstp support trunk*/
	if(trunk_l2_if->mstp_port_info)
	{
		mstp_reinit();		

	}
	
    return CMD_SUCCESS;
}

DEFUN( no_trunk_member_port_set,
	no_trunk_member_port_set_cmd,
	"no trunk <1-128> {passive|priority}",
	NO_STR
	"Set trunk member\n"
	"The value of trunk id\n"
    "Passive mode\n"
    "Port priority\n"
	)
{
    int ret = 0;
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct trunk_port *tport_tmp = NULL;
    struct listnode *node_tmp = NULL;  
    uint32_t ifindex = 0;  
    uint16_t trunkid = 0;

    /*trunk id是否在有效范围内*/
    VTY_GET_INTEGER_RANGE ( "trunkid", trunkid, argv[0], 1, 128 );

    /*获取trunkid对应的trunk结构体*/
    trunk_entry = trunk_lookup( trunkid );
    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

    /*管理口不支持*/
    ifindex = (uint32_t) vty->index;

    if (IFM_TYPE_IS_OUTBAND(ifindex))
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOSUPPORT_OUTBAND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOSUPPORT_OUTBAND;
    }

    /*该端口是否是其成员口*/
    tport = trunk_port_lookup(trunkid, ifindex);
    if (NULL == tport)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_TRUNK_MEMBER), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_TRUNK_MEMBER;
    }

    /*有其中一个不为空，是修改端口参数*/
    if((argv[1] != NULL) || (argv[2] != NULL))
    {
        if (argv[1] != NULL)
        {
            if (TRUNK_ENABLE == trunk_entry->lacp_enable)
            {
                /*设置主动方式*/
                if (tport->passive != TRUNK_DISABLE)
                {
                    tport->passive = TRUNK_DISABLE;
                    LACP_SET_ACTIVITY(tport->link.actor_state);

                    if (IFNET_LINKUP == tport->linkstatus)
                    {
                        lacp_tx(trunk_entry, ifindex);
                    }
                }
            }
            else
            {
                vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_LACP_DISABLE), VTY_NEWLINE);
                return L2_TRUNK_ERR_LACP_DISABLE;
            }
        }

        /*恢复默认端口优先级*/
        if (argv[2] != NULL)
        {
            if ((TRUNK_DISABLE == trunk_entry->lacp_enable) && (TRUNK_MODE_ECMP == trunk_entry->work_mode))
            {
                vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_MANUAL_ECMP_NO_SET), VTY_NEWLINE);
                return L2_TRUNK_ERR_MANUAL_ECMP_NO_SET;
            }
            else
			{
				tport->priority = TRUNK_PRIORITY_DEFAULT; 
                /*选择主端口*/
                if (TRUNK_DISABLE == trunk_entry->lacp_enable)
                {
                    if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
                    {
                        trunk_backup_port_select(trunk_entry);

                        if (trunk_entry->portlist.count != TRUNK_BACK_MEMBER_MAX)

                        {
                            trunk_entry->status = BACKUP_STATUS_INVALID;
                        }
                        else
                        {
                            if (1 == trunk_entry->upport_num)
                            {
                                for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
                                {
                                    if (tport_tmp->linkstatus != IFNET_LINKUP)
                                    {
                                        continue;
		                           }

		                           if(tport_tmp->ifindex == trunk_entry->master_if)
		                           {
		                               trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
		                           }
                                    else
                                    {
                                        trunk_backup_switch(trunk_entry, BACKUP_STATUS_SLAVE);
                                    }
                                }
                            }
                            else if (TRUNK_BACK_MEMBER_MAX == trunk_entry->upport_num)
                            {
                                trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
		                    }
							else
							{
								trunk_entry->status = BACKUP_STATUS_INVALID;
							}
						}
						
						/*上报/清除告警*/
						trunk_member_alarm_report(trunk_entry->trunkid, trunk_entry->master_if);
	                }
	            }
	            else
                {
                    if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
                    {
                        trunk_backup_port_select(trunk_entry);
                    }

                    lacp_pkt_tx(trunk_entry, ifindex);
                }
	        }
        }
    }
    /*将该端口从聚合组中删除*/
    else
    {
        ret = trunk_port_delete(trunkid, ifindex);

        if (ret != 0)
        {
            vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_MEMBER_DEL), VTY_NEWLINE);
            return L2_TRUNK_ERR_MEMBER_DEL;
        }
    }
    return CMD_SUCCESS;
}



DEFUN( trunk_lacp_enable,
	trunk_lacp_enable_cmd,
	"lacp enable",
	"Link aggregation control protocol\n"
	"Enable lacp\n"
	)
{
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
	struct l2if *l2_if = NULL;
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;
	int ret = 0;

    ifindex = ( uint32_t ) vty->index;
    l2_if = l2if_get(ifindex);
    if (NULL == l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }
	
    /*获取trunkid对应的trunk结构体，如果没有并创建*/
    trunkid = IFM_TRUNK_ID_GET(ifindex);
    trunk_entry = trunk_lookup( trunkid );
    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

    /*切换聚合模式，需将聚合成员删除*/
    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        /*使能lacp协议*/
        trunk_entry->lacp_enable = TRUNK_ENABLE;	
       //ret = ipc_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_LACP_ENABLE, IPC_OPCODE_UPDATE, 0);
	    ret = l2_msg_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1,
	  				MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_LACP_ENABLE, IPC_OPCODE_UPDATE, 0);			
        if (ret != 0)
        {
            vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_HAL), VTY_NEWLINE);
            return L2_TRUNK_ERR_HAL;
        }

        if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
        {
            /*选择工作端口*/
            trunk_backup_port_select(trunk_entry);
        }

        for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
        {
            lacp_fsm(trunk_entry, tport->ifindex, LACP_EVENT_ENABLE_LACP);
			
            /*上报/清除告警*/
            trunk_member_alarm_report(trunk_entry->trunkid, tport->ifindex);
        }

        /*设置端口的link状态*/
        trunk_linkstatus_set(trunk_entry);
        trunk_speed_set(trunk_entry->trunkid);
    }
    
    return CMD_SUCCESS;

}

DEFUN( no_trunk_lacp_enable,
	no_trunk_lacp_enable_cmd,
	"no lacp enable",
	NO_STR
	"Link aggregation control protocol\n"
	"Enable lacp\n"
	)
{
    struct trunk *trunk_entry = NULL;   
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
	struct l2if *l2_if = NULL;
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;
	int ret = 0;

    ifindex = ( uint32_t ) vty->index;
    l2_if = l2if_get(ifindex);
    if (NULL == l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }

    /*获取trunkid对应的trunk结构体，如果没有并创建*/
    trunkid = IFM_TRUNK_ID_GET(ifindex);
    trunk_entry = trunk_lookup(trunkid);

    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

    if (TRUNK_ENABLE == trunk_entry->lacp_enable)
    {
        trunk_entry->lacp_enable = TRUNK_DISABLE;	
     //   ret = ipc_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_LACP_ENABLE, IPC_OPCODE_UPDATE, 0);
	    ret = l2_msg_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, 
	  					MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_LACP_ENABLE, IPC_OPCODE_UPDATE, 0);
        if (ret != 0)
        {
            vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_HAL), VTY_NEWLINE);
            return L2_TRUNK_ERR_HAL;
        }

        if (trunk_entry->work_mode != TRUNK_MODE_ECMP)
        {
            /*选择工作端口*/
            trunk_backup_port_select(trunk_entry);

            if (trunk_entry->portlist.count != TRUNK_BACK_MEMBER_MAX)


            {
                trunk_entry->status = BACKUP_STATUS_INVALID;
            }
            else
            {
                if (1 == trunk_entry->upport_num)
                {
	               for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
	               {
	                   if(tport->linkstatus != IFNET_LINKUP)
	                   {
	                       continue;
	                   }
	    
	                   if(tport->ifindex == trunk_entry->master_if)
	                   {
	                       trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
	                   }
	                   else
	                   {
	                       trunk_backup_switch(trunk_entry, BACKUP_STATUS_SLAVE);
	                   }
	               }
	           }
                else if (TRUNK_BACK_MEMBER_MAX == trunk_entry->upport_num)
	           {
	               trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
	           }
			   else
			   {
				   trunk_entry->status = BACKUP_STATUS_INVALID;
			   }
		   }
		   
		   /*上报/清除告警*/
		   trunk_member_alarm_report(trunk_entry->trunkid, trunk_entry->master_if);
       }
       else
       {
            trunk_ecmp_port_select(trunk_entry);
			for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
			{
				/*上报/清除告警*/
				trunk_member_alarm_report(trunk_entry->trunkid, tport->ifindex);
			}
       }
	   
	   	/*设置端口的link状态*/
		trunk_linkstatus_set(trunk_entry);
        trunk_speed_set(trunk_entry->trunkid);
    }

	/*发送间隔恢复默认*/
	trunk_entry->lacp_interval = TRUNK_INTERVAL_DEFAULT;
    return CMD_SUCCESS;

}

DEFUN( trunk_lacp_interval,
	trunk_lacp_interval_cmd,
	"lacp interval <1-30>",
	"Link aggregation control protocol\n"
	"Packet send interval\n"
	"The default value is 1s. Please input an integer from 1 to 30\n"
	)
{
    struct trunk *trunk_entry = NULL;
	struct l2if *l2_if = NULL;
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;

    ifindex = ( uint32_t ) vty->index;
	l2_if = l2if_get(ifindex);
    if (NULL == l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }
    
    /*获取trunkid对应的trunk结构体，如果没有并创建*/
    trunkid = IFM_TRUNK_ID_GET(ifindex);
    trunk_entry = trunk_lookup( trunkid );
    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

    /*lacp不使能条件下不可配*/
    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_LACP_DISABLE), VTY_NEWLINE);
        return L2_TRUNK_ERR_LACP_DISABLE;
    }
    /*设置报文发送间隔*/
    VTY_GET_INTEGER_RANGE ( "interval", trunk_entry->lacp_interval, argv[0], 1, 30 );

    return CMD_SUCCESS;

}

DEFUN( no_trunk_lacp_interval,
	no_trunk_lacp_interval_cmd,
	"no lacp interval",
	NO_STR
	"Link aggregation control protocol\n"
	"Packet send interval\n"
	)
{
    struct trunk *trunk_entry = NULL;
	struct l2if *l2_if = NULL;
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;

    ifindex = ( uint32_t ) vty->index;
	l2_if = l2if_get(ifindex);

    if (NULL == l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }

    /*获取trunkid对应的trunk结构体，如果没有并创建*/
    trunkid = IFM_TRUNK_ID_GET(ifindex);
    trunk_entry = trunk_lookup(trunkid);

    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

    /*lacp不使能条件下不可配*/
    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_LACP_DISABLE), VTY_NEWLINE);
        return L2_TRUNK_ERR_LACP_DISABLE;
    }

    /*恢复默认报文发送间隔*/
    trunk_entry->lacp_interval = TRUNK_INTERVAL_DEFAULT;
    
    return CMD_SUCCESS;

}
DEFUN( trunk_lacp_priority,
	trunk_lacp_priority_cmd,
	"lacp priority <0-65535>",
	"Link aggregation control protocol\n"
	"Set lacp system priority\n"
	"The default value is 32768. Please input an integer from 0 to 65535\n"
	)
{
    struct trunk *trunk_entry = NULL;  
	struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
	struct l2if *l2_if = NULL;
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;

    ifindex = ( uint32_t ) vty->index;
	l2_if = l2if_get(ifindex);
    if (NULL == l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }

    /*获取trunkid对应的trunk结构体，如果没有并创建*/
    trunkid = IFM_TRUNK_ID_GET(ifindex);
    trunk_entry = trunk_lookup(trunkid);

    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

    /*lacp不使能条件下不可配*/
    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_LACP_DISABLE), VTY_NEWLINE);
        return L2_TRUNK_ERR_LACP_DISABLE;
    }

    /*设置系统优先级*/
    VTY_GET_INTEGER_RANGE ( "priority", trunk_entry->priority, argv[0], 0, 65535 );
	for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
	{
		lacp_pkt_tx(trunk_entry, tport->ifindex);
	}

    return CMD_SUCCESS;
}

DEFUN( no_trunk_lacp_priority,
	no_trunk_lacp_priority_cmd,
	"no lacp priority",
	NO_STR
	"Link aggregation control protocol\n"
	"Set lacp system priority\n"
	)
{
    struct trunk *trunk_entry = NULL;
	struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
	struct l2if *l2_if = NULL;
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;

    ifindex = ( uint32_t ) vty->index;
	l2_if = l2if_get(ifindex);
    if (NULL == l2_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return L2_TRUNK_ERR_GET_L2IF;
    }

    /*获取trunkid对应的trunk结构体，如果没有并创建*/
    trunkid = IFM_TRUNK_ID_GET(ifindex);
    trunk_entry = trunk_lookup(trunkid);

    if (NULL == trunk_entry)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_NOT_FOUND), VTY_NEWLINE);
        return L2_TRUNK_ERR_NOT_FOUND;
    }

    /*lacp不使能条件下不可配*/
    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_LACP_DISABLE), VTY_NEWLINE);
        return L2_TRUNK_ERR_LACP_DISABLE;
    }

    /*恢复默认系统优先级*/
    trunk_entry->priority = TRUNK_PRIORITY_DEFAULT;
	for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
	{
		lacp_pkt_tx(trunk_entry, tport->ifindex);
	}

    return CMD_SUCCESS;
}


DEFUN( debug_trunk_lacp_packet,
	debug_trunk_lacp_packet_cmd,
	"debug lacp packet {recv|send}",
	"Set lacp debug\n"
	"Link aggregation control protocol\n"
	"Recvice/send protocol packet\n"
	"Recvice protocol packet\n"
	"Send protocol packet\n"
	)
{
    if ((NULL == argv[0]) && (NULL == argv[1]))
    {
        lacp_recv_dbg = 1;
        lacp_send_dbg = 1;
    }
    else
    {
        if ( argv[0] != NULL )
        {
            lacp_recv_dbg = 1;
        }

        if ( argv[1] != NULL )
        {
            lacp_send_dbg = 1;
        }
    }
    
    return CMD_SUCCESS;
}

DEFUN( no_debug_trunk_lacp_packet,
	no_debug_trunk_lacp_packet_cmd,
	"no debug lacp packet {recv|send}",
	NO_STR
	"Set lacp debug\n"
	"Link aggregation control protocol\n"
	"Recvice/send protocol packet\n"
	"Recvice protocol packet\n"
	"Send protocol packet\n"
	)
{
    if ((NULL == argv[0]) && (NULL == argv[1]))
    {
        lacp_recv_dbg = 0;
        lacp_send_dbg = 0;
    }
    else
    {
        if ( argv[0] != NULL )
        {
            lacp_recv_dbg = 0;
        }

        if ( argv[1] != NULL )
        {
            lacp_send_dbg = 0;
        }
    }
    
    return CMD_SUCCESS;
}

/*********************************************
    显示lacp报文统计信息
    LacpRevPdu:     接收到的lacp报文数量
    LacpSendPdu:   发送的lacp报文数量
**********************************************/
DEFUN( show_trunk_lacp_statistics,
	show_trunk_lacp_statistics_cmd,
	"show lacp statistics",
	SHOW_STR
	"Link aggregation control protocol\n"
	"Lacp packet statistics\n"
	)
{
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
    char ifname[IFNET_NAMESIZE];
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;
	uint32_t pkts_recv = 0, pkts_send = 0;

    ifindex = ( uint32_t ) vty->index;
    
    /*获取trunkid对应的trunk结构体*/
    trunkid = IFM_TRUNK_ID_GET(ifindex);
    trunk_entry = trunk_lookup(trunkid);
    if(trunk_entry != NULL)
    {
        /*静态聚合报文显示*/
        if (TRUNK_ENABLE == trunk_entry->lacp_enable)
        {
            pkts_recv = 0;
            pkts_send = 0;
            vty_out ( vty, "Eth-Trunk%u's PDU statistic is:%s%s", trunkid,VTY_NEWLINE,VTY_NEWLINE );
            vty_out ( vty, "-----------------------------------------------%s",VTY_NEWLINE);
            vty_out ( vty, "%-21s %-10s %-10s%s", "Port", "LacpRevPdu" , "LacpSentPdu", VTY_NEWLINE);
            for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
            {
                pkts_recv += tport->link.pkt_rx;
                pkts_send += tport->link.pkt_tx;
                ifm_get_name_by_ifindex(tport->ifindex, ifname);
                vty_out ( vty, "%-21s %-10u %-10u%s", ifname, tport->link.pkt_rx,tport->link.pkt_tx,VTY_NEWLINE );
            } 
            
            vty_out ( vty, "Lacp packet receive is:%u%s", pkts_recv,VTY_NEWLINE);
            vty_out ( vty, "Lacp packet send is:%u%s",pkts_send,VTY_NEWLINE);
        }
    }
    return CMD_SUCCESS;
}

/*显示特定接口的信息*/
DEFUN ( show_ifm_trunk, show_ifm_trunk_cmd,
        "show interface trunk TRUNK",
        SHOW_STR
        CLI_INTERFACE_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR )

{
    uint16_t trunk_id = 0;
    struct trunk *trunk_entry = NULL;
    uint32_t ifindex = 0;

    ifindex = ifm_get_ifindex_by_name("trunk", (char *) argv[0]);

    if (ifindex != 0)
    {
        if (!IFM_IS_SUBPORT(ifindex))
        {
            VTY_GET_INTEGER_RANGE("trunkid", trunk_id, argv[0], 1, 128);

            trunk_entry = trunk_lookup(trunk_id);

            if (trunk_entry != NULL)
            {
                trunk_info_show(vty, trunk_entry);
            }
        }
    }

    return CMD_SUCCESS;
}

/*显示所有trunk接口的信息*/
DEFUN ( show_ifm_trunk_all, show_ifm_trunk_all_cmd,
        "show interface trunk ",
        SHOW_STR
        CLI_INTERFACE_STR
        CLI_INTERFACE_TDM_STR)
{
    return CMD_SUCCESS;
}

/*显示特定接口的配置信息*/
DEFUN ( show_ifm_trunk_config, show_ifm_trunk_config_cmd,
        "show interface config trunk TRUNK",
        SHOW_STR
        CLI_INTERFACE_STR
        "Interface all config\n"
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR )
{
    uint16_t trunk_id = 0;
    struct trunk *trunk_entry = NULL;
	uint32_t ifindex = 0;
	
	ifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[0] );

    if (ifindex != 0)
    {
        if (!IFM_IS_SUBPORT(ifindex))
        {
            VTY_GET_INTEGER_RANGE("trunkid", trunk_id, argv[0], 1, 128);

            trunk_entry = trunk_lookup(trunk_id);

            if (trunk_entry != NULL)
            {
                trunk_config_show(vty, trunk_entry);
            }
        }
    }

    return CMD_SUCCESS;
}

/*显示所有trunk接口的配置信息*/
DEFUN ( show_ifm_trunk_all_config, show_ifm_trunk_all_config_cmd,
        "show interface config trunk",
        SHOW_STR
        CLI_INTERFACE_STR
        "Interface all config\n"
        CLI_INTERFACE_TRUNK_STR)
{
    return CMD_SUCCESS;    
}

DEFUN ( clear_statistics,
        clear_statistics_cmd,
        "statistics clear",
        "Interface statistics\n"
        "Clear	statistics\n" )
{
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
    uint32_t ifindex = 0;
    uint16_t trunkid = 0;

    ifindex = (uint32_t) vty->index;

    /*获取trunkid对应的trunk结构体*/
    trunkid = IFM_TRUNK_ID_GET(ifindex);
    trunk_entry = trunk_lookup(trunkid);

    if (trunk_entry != NULL)
    {
        for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
        {
            tport->link.pkt_rx = 0;
            tport->link.pkt_tx = 0;
        }
    }

    return CMD_SUCCESS;
}

/*清除报文统计信息*/
DEFUN ( clear_if_statistics,
        clear_if_statistics_cmd,
        "statistics clear interface trunk TRUNK",
        "Interface statistics\n"
        "Clear	statistics\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR)
{
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
    uint16_t trunkid = 0;
	uint32_t ifindex = 0;

	ifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[0] );
    if (ifindex != 0)
    {
        if (!IFM_IS_SUBPORT(ifindex))
        {
            VTY_GET_INTEGER_RANGE("trunkid", trunkid, argv[0], 1, 128);
            trunk_entry = trunk_lookup(trunkid);

            if (trunk_entry != NULL)
            {
                for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
                {
                    tport->link.pkt_rx = 0;
                    tport->link.pkt_tx = 0;
                }
            }
        }
    }


    return CMD_SUCCESS;
}

DEFUN (no_if_trunk, no_if_trunk_cmd,
        "no interface trunk TRUNK",
        NO_STR
        CLI_INTERFACE_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR )
{
	uint32_t ifindex = 0;
	uint16_t trunkid = 0;
    struct trunk *trunk_entry = NULL;

    ifindex = ifm_get_ifindex_by_name ( "trunk", ( char * ) argv[0] );
    if (ifindex != 0)
    {
        if (!IFM_IS_SUBPORT(ifindex))
        {
            trunkid = IFM_TRUNK_ID_GET(ifindex);

            /*聚合组中存在成员口，不允许删除*/
            trunk_entry = trunk_lookup(trunkid);

            if (trunk_entry != NULL)
            {
                if (trunk_entry->portlist.count != 0)
                {
                    vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_TRUNK_DEL), VTY_NEWLINE);
                    return L2_TRUNK_ERR_TRUNK_DEL;
                }
            }
        }
    }

    return CMD_SUCCESS;  
}

int trunk_config_write(struct vty *vty)
{
    int i;
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
	char ifname[IFNET_NAMESIZE];
    char buf[1024];
    
    for(i = 1; i <= IFM_TRUNK_MAX; i++)
    {
        trunk_entry = trunk_lookup(i);

        if (NULL == trunk_entry)
        {
            continue;
        }

        vty_out ( vty, "interface trunk %u%s", trunk_entry->trunkid, VTY_NEWLINE );

        if (TRUNK_ENABLE == trunk_entry->lacp_enable)
        {
            vty_out(vty, " lacp enable%s",VTY_NEWLINE);
            if(trunk_entry->priority != TRUNK_PRIORITY_DEFAULT)
            {
                vty_out(vty, " lacp priority %u%s", trunk_entry->priority,VTY_NEWLINE);
            }
            
            if(trunk_entry->lacp_interval != TRUNK_INTERVAL_DEFAULT)
            {
                vty_out(vty, " lacp interval %u%s", trunk_entry->lacp_interval,VTY_NEWLINE);
            }
        }
        
        if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
        {
            if (TRUNK_SWITCH_NOFAILBACK == trunk_entry->failback)
            {
                vty_out(vty, " backup non-failback%s",VTY_NEWLINE);
            }
            else
            {
                if(trunk_entry->wtr != TRUNK_WTR_DEFAULT)
                {
                    vty_out(vty, " backup failback wtr %u%s",trunk_entry->wtr, VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, " backup failback%s",VTY_NEWLINE);
                }
            }
        }
        else
        {
            if (TRUNK_BALANCE_DIP == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " load-balance dip%s", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SIP == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " load-balance sip%s", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SIP_DIP == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " load-balance sip-dip%s", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_LABEL == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " load-balance label%s", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_DMAC == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " load-balance dmac%s", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SMAC_DMAC == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " load-balance smac-dmac%s", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SPORT_DMAC == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " load-balance sport-dmac%s", VTY_NEWLINE);
            }
	      else if(TRUNK_BALANCE_SPORT_DPORT == trunk_entry->ecmp_mode)
	      	{
	      		vty_out(vty," load-balance sport-dport%s",VTY_NEWLINE);
	      	}
		  else
		  {
		  /**/
            }    
        }

        for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
        {
            ifm_get_name_by_ifindex(tport->ifindex, ifname); 
            vty_out ( vty, "interface %s%s", ifname, VTY_NEWLINE );
            memset(buf, 0, sizeof(buf));
            snprintf(buf+strlen(buf),sizeof(buf)-strlen(buf),"trunk %u ",trunk_entry->trunkid);

            if (TRUNK_ENABLE == trunk_entry->lacp_enable)
            {
                if(tport->passive != TRUNK_DISABLE)
                {
                    snprintf(buf+strlen(buf),sizeof(buf)-strlen(buf),"passive ");
                }

				if(tport->priority != TRUNK_PRIORITY_DEFAULT)
	            {
	                snprintf(buf+strlen(buf),sizeof(buf)-strlen(buf),"priority %u ",tport->priority);
	            }
            }
			else
			{
                if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
				{
					if(tport->priority != TRUNK_PRIORITY_DEFAULT)
					{
						snprintf(buf+strlen(buf),sizeof(buf)-strlen(buf),"priority %u ",tport->priority);
					}
				}
			}

            vty_out ( vty, " %s%s", buf, VTY_NEWLINE );
        } 
     
    }

    return 0;
}
int trunk_config_write_all(struct vty *vty)
{
	if(TRUNK_BALANCE_SMAC  != trunk_gloable_config.gloable_balance_mode)
	{
		if (TRUNK_BALANCE_DIP == trunk_gloable_config.gloable_balance_mode)
		{
			vty_out(vty, "gloable-load-balance dip%s", VTY_NEWLINE);
		}
		else if (TRUNK_BALANCE_SIP == trunk_gloable_config.gloable_balance_mode)
		{
			vty_out(vty, "gloable-load-balance sip%s", VTY_NEWLINE);
		}
		else if (TRUNK_BALANCE_SIP_DIP == trunk_gloable_config.gloable_balance_mode)
		{
			vty_out(vty, "gloable-load-balance sip-dip%s", VTY_NEWLINE);
		}
		else if (TRUNK_BALANCE_DMAC == trunk_gloable_config.gloable_balance_mode)
		{
			vty_out(vty, "gloable-load-balance dmac%s", VTY_NEWLINE);
		}
		else if (TRUNK_BALANCE_SMAC_DMAC == trunk_gloable_config.gloable_balance_mode)
		{
			vty_out(vty, "gloable-load-balance smac-dmac%s", VTY_NEWLINE);
		}
		else if (TRUNK_BALANCE_SPORT_DMAC == trunk_gloable_config.gloable_balance_mode)
		{
			vty_out(vty, "gloable-load-balance sport-dmac%s", VTY_NEWLINE);
		}
		else if(TRUNK_BALANCE_SPORT_DPORT == trunk_gloable_config.gloable_balance_mode)
		{
			vty_out(vty,"gloable-load-balance sport-dport%s",VTY_NEWLINE);
		}
		else if(TRUNK_BALANCE_LABEL == trunk_gloable_config.gloable_balance_mode)
		{
			vty_out(vty,"gloable-load-balance label%s",VTY_NEWLINE);
		}
		
		else
		{
		/**/
		}
	}

	return CMD_SUCCESS;
	
}

void trunk_info_show(struct vty *vty, struct trunk *trunk_entry)
{
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL; 
	struct l2if *l2_trunk_if = NULL;
    char ifname[IFNET_NAMESIZE];
    char mac_str[128];
    int num = 0;
	
	l2_trunk_if = l2if_get(trunk_entry->ifindex);
    if (NULL == l2_trunk_if)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(L2_TRUNK_ERR_GET_L2IF), VTY_NEWLINE);
        return;
    }

    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        /*手工聚合主备方式显示*/
        if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
        {
            vty_out(vty, " Eth-Trunk%u's state information is:%s", trunk_entry->trunkid, VTY_NEWLINE);
            vty_out(vty, " WorkingMode: %s%s", "Manual", VTY_NEWLINE);
            vty_out(vty, " WorkingWay: %s%s", "Backup", VTY_NEWLINE);

            if (TRUNK_SWITCH_FAILBACK == trunk_entry->failback)
            {
                vty_out(vty, " Failback: %s%s", "Yes", VTY_NEWLINE);
                vty_out(vty, " Wtr(s): %u%s", trunk_entry->wtr, VTY_NEWLINE);
            }
            else
            {
                vty_out ( vty, " Failback: %s%s", "No",VTY_NEWLINE );
            }

            num = trunk_entry->portlist.count;
            vty_out ( vty, " Number Of Ports in Trunk = %u%s", num,VTY_NEWLINE );
            num = trunk_entry->upport_num;
            vty_out ( vty, " Number Of UP Ports in Trunk = %u%s", num,VTY_NEWLINE );
            vty_out(vty, " Operate status: %s%s", (trunk_entry->down_flag == IFNET_LINKDOWN) ? "down" : "up", VTY_NEWLINE);

            if (trunk_entry->master_if != 0)
            {
                for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
                {
                    if (BACKUP_STATUS_MASTER == trunk_entry->status)
                    {
                        ifm_get_name_by_ifindex(trunk_entry->master_if, ifname);
                        break;
                    }
                    else if (BACKUP_STATUS_SLAVE == trunk_entry->status)
                    {
                        if(tport->ifindex != trunk_entry->master_if)
                        {
                            ifm_get_name_by_ifindex(tport->ifindex, ifname);
                            break;
                        }
                    }
                    else
                    {
                        memset(ifname, 0, IFNET_NAMESIZE);
                    }
                }
            }
            else
            {
                memset(ifname, 0, IFNET_NAMESIZE);
            }
            vty_out ( vty, " Work port: %s%s", ifname, VTY_NEWLINE);
            if(trunk_entry->portlist.count != 0)
            {
                vty_out ( vty, " --------------------------------------------%s",VTY_NEWLINE);
                vty_out ( vty, " %-21s %-8s %-3s %-10s%s", "PortName", "PortPri", "M/S", "linkState",VTY_NEWLINE);
                for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
                {
                    ifm_get_name_by_ifindex(tport->ifindex, ifname);
                    vty_out ( vty, " %-21s %-8u %-3s %-10s%s", ifname,tport->priority,(trunk_entry->master_if == tport->ifindex)? "M":"S",
                                (tport->linkstatus == IFNET_LINKUP)? "Up":"Down",VTY_NEWLINE );
                } 
            }
        }
        /*手工聚合负载分担显示*/
        else
        {
            vty_out ( vty, " Eth-Trunk%u\'s state information is:%s", trunk_entry->trunkid,VTY_NEWLINE );
            vty_out ( vty, " WorkingMode: %s%s", "Manual",VTY_NEWLINE );
            vty_out ( vty, " WorkingWay: %s%s", "Ecmp",VTY_NEWLINE );

            if (TRUNK_BALANCE_LABEL == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic: %s%s", "According to Label", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_DIP == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic: %s%s", "According to Dest IP", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SIP == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic: %s%s", "According to Source IP", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SIP_DIP == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic: %s%s", "According to Source and Dest IP", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SMAC == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic: %s%s", "According to Source Mac", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_DMAC == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic: %s%s", "According to Dest Mac", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SMAC_DMAC == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic: %s%s", "According to Source and Dest Mac", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SPORT_DMAC == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic: %s%s", "According to Source Port and Dest Mac", VTY_NEWLINE);
            }
		else if(TRUNK_BALANCE_SPORT_DPORT == trunk_entry->ecmp_mode)
		{
			vty_out(vty," Hash arithmetic: %s%s", "According to Source Port and Dest Port", VTY_NEWLINE);
	      	}

            num = trunk_entry->portlist.count;
            vty_out ( vty, " Number Of Ports in Trunk = %u%s", num,VTY_NEWLINE );
            num = trunk_entry->upport_num;
            vty_out ( vty, " Number Of UP Ports in Trunk = %u%s", num,VTY_NEWLINE );
            vty_out ( vty, " operate status: %s%s", (trunk_entry->down_flag == IFNET_LINKDOWN)?"down":"up",VTY_NEWLINE );
            
            if(trunk_entry->portlist.count != 0)
            {
                vty_out ( vty, " ---------------------------------%s",VTY_NEWLINE);
                vty_out ( vty, " %-21s %-10s%s", "PortName", "linkState",VTY_NEWLINE);
                for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
                {
                    ifm_get_name_by_ifindex(tport->ifindex, ifname);
                    vty_out ( vty, " %-21s %-10s%s", ifname, (tport->linkstatus == IFNET_LINKUP)? "Up":"Down",VTY_NEWLINE );
                } 
            }
        }
    }
    /*静态聚合显示*/
    else
    {
        if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
        {
            vty_out(vty, " Eth-Trunk%u's state information is:%s", trunk_entry->trunkid, VTY_NEWLINE);
            vty_out(vty, " WorkingMode:%s%s", "Static", VTY_NEWLINE);
            vty_out(vty, " WorkingWay: %s%s", "backup", VTY_NEWLINE);

            if (TRUNK_SWITCH_FAILBACK == trunk_entry->failback)
            {
                vty_out(vty, " FailBack: %s%s", "Yes", VTY_NEWLINE);
                vty_out(vty, " Wtr: %d%s", trunk_entry->wtr, VTY_NEWLINE);
            }
            else
            {
                vty_out ( vty, " FailBack: %s%s", "No",VTY_NEWLINE );
            }
            vty_out ( vty, " System priority: %u%s", trunk_entry->priority,VTY_NEWLINE );
            memset(mac_str, 0, sizeof(char) * 128);
            snprintf(mac_str, sizeof(char) * 128, "%02x%02x-%02x%02x-%02x%02x",l2_trunk_if->mac[0],l2_trunk_if->mac[1],
                l2_trunk_if->mac[2],l2_trunk_if->mac[3],l2_trunk_if->mac[4],l2_trunk_if->mac[5]);
            vty_out ( vty, " System ID: %s%s", mac_str, VTY_NEWLINE);

            num = trunk_entry->portlist.count;
            vty_out ( vty, " Number Of Ports in Trunk = %u%s", num,VTY_NEWLINE );
            num = trunk_entry->upport_num;
            vty_out ( vty, " Number Of UP Ports in Trunk = %u%s", num,VTY_NEWLINE );
            vty_out ( vty, " Operate status: %s%s", (trunk_entry->down_flag == IFNET_LINKDOWN)?"down":"up",VTY_NEWLINE );

            if(trunk_entry->portlist.count != 0)
            {
                vty_out ( vty, " -------------------------------------------------------------------------%s",VTY_NEWLINE);
                vty_out ( vty, " %-21s %-8s %-6s %-7s %-9s %-8s%s","ActorPortName", "PortPri", "PortNo", "PortKey", "PortState", "State", VTY_NEWLINE);
                for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
                {
                    ifm_get_name_by_ifindex(tport->ifindex, ifname);
                    vty_out ( vty, " %-21s %-8u %-6u %-7u %-9u %-8s%s", ifname,tport->priority,tport->link.actor_port,
                      tport->link.actor_key, tport->link.actor_state,(tport->link.sync_state == 1)? "Selected":"Unselect",VTY_NEWLINE );
                }  
                
                vty_out ( vty, "%s", VTY_NEWLINE);
                vty_out ( vty, " Partner:%s", VTY_NEWLINE);
                vty_out ( vty, " -------------------------------------------------------------------------%s",VTY_NEWLINE);
                vty_out ( vty, " %-21s %-6s %-15s %-7s %-6s %-7s %-8s%s", "ActorPortName", "SysPri", "SystemID", "PortPri", "PortNo", "PortKey", "PortState", VTY_NEWLINE);
                for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
                {
                    ifm_get_name_by_ifindex(tport->ifindex, ifname);
                    memset(mac_str, 0, sizeof(char) * 128);
                    snprintf(mac_str, sizeof(char) * 128, "%02x%02x-%02x%02x-%02x%02x",tport->link.partner_sys[0],tport->link.partner_sys[1],
                    tport->link.partner_sys[2],tport->link.partner_sys[3],tport->link.partner_sys[4],tport->link.partner_sys[5]);
                    vty_out ( vty, " %-21s %-6u %-15s %-7u %-6u %-7u %-8u%s", ifname,tport->link.partner_spri,mac_str,
                      tport->link.partner_ppri, tport->link.partner_port,tport->link.partner_key, tport->link.partner_state,VTY_NEWLINE);
                }
            }
        }
        else
        {
            vty_out ( vty, " Eth-Trunk%u's state information is:%s", trunk_entry->trunkid,VTY_NEWLINE );
            vty_out ( vty, " WorkingMode:%s%s", "Static",VTY_NEWLINE );
            vty_out ( vty, " WorkingWay: %s%s", "Ecmp",VTY_NEWLINE );
            vty_out(vty, " System priority: %u%s", trunk_entry->priority, VTY_NEWLINE);
            memset(mac_str, 0, sizeof(char) * 128);
            snprintf(mac_str, sizeof(char) * 128, "%02x%02x-%02x%02x-%02x%02x", l2_trunk_if->mac[0], l2_trunk_if->mac[1],
                     l2_trunk_if->mac[2], l2_trunk_if->mac[3], l2_trunk_if->mac[4], l2_trunk_if->mac[5]);
            vty_out(vty, " System ID: %s%s", mac_str, VTY_NEWLINE);

            if (TRUNK_BALANCE_LABEL == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic:%s%s", "According to Label", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_DIP == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic:%s%s", "According to Dest IP", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SIP == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic:%s%s", "According to Source IP", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SIP_DIP == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic:%s%s", "According to Source and Dest IP", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SMAC == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic:%s%s", "According to Source Mac", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_DMAC == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic:%s%s", "According to Dest Mac", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SMAC_DMAC == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic:%s%s", "According to Source and Dest Mac", VTY_NEWLINE);
            }
            else if (TRUNK_BALANCE_SPORT_DMAC == trunk_entry->ecmp_mode)
            {
                vty_out(vty, " Hash arithmetic:%s%s", "According to Source Port and Dest Mac", VTY_NEWLINE);
            }
	     else if(TRUNK_BALANCE_SPORT_DPORT == trunk_entry->ecmp_mode)
	     	{
			vty_out(vty," Hash arithmetic: %s%s", "According to Source Port and Dest Port", VTY_NEWLINE);
	      	}

            num = trunk_entry->portlist.count;
            vty_out ( vty, " Number Of Ports in Trunk = %u%s", num,VTY_NEWLINE );
            num = trunk_entry->upport_num;
            vty_out ( vty, " Number Of UP Ports in Trunk = %u%s", num,VTY_NEWLINE );
            vty_out ( vty, " operate status: %s%s", (trunk_entry->down_flag == IFNET_LINKDOWN)?"down":"up",VTY_NEWLINE );

            if(trunk_entry->portlist.count != 0)
            {
                vty_out ( vty, " -------------------------------------------------------------------------%s",VTY_NEWLINE);
                vty_out ( vty, " %-21s %-8s %-6s %-7s %-9s %-8s%s","ActorPortName", "PortPri", "PortNo", "PortKey", "PortState", "State", VTY_NEWLINE);
                for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
                {
                    ifm_get_name_by_ifindex(tport->ifindex, ifname);
                    vty_out ( vty, " %-21s %-8u %-6u %-7u %-9u %-8s%s", ifname,tport->priority,tport->link.actor_port,
                        tport->link.actor_key, tport->link.actor_state,(tport->link.sync_state == 1)? "Selected":"Unselect",VTY_NEWLINE );
                }  
                
                vty_out ( vty, "%s", VTY_NEWLINE);
                vty_out ( vty, " Partner:%s", VTY_NEWLINE);
                vty_out ( vty, " -------------------------------------------------------------------------%s",VTY_NEWLINE);
                vty_out ( vty, " %-21s %-6s %-15s %-7s %-6s %-7s %-8s%s", "ActorPortName", "SysPri", "SystemID", "PortPri", "PortNo", "PortKey", "PortState", VTY_NEWLINE);
                for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
                {
                    ifm_get_name_by_ifindex(tport->ifindex, ifname);
                    memset(mac_str, 0, sizeof(char) * 128);
                    snprintf(mac_str, sizeof(char) * 128, "%02x%02x-%02x%02x-%02x%02x",tport->link.partner_sys[0],tport->link.partner_sys[1],
                    tport->link.partner_sys[2],tport->link.partner_sys[3],tport->link.partner_sys[4],tport->link.partner_sys[5]);
                    vty_out ( vty, " %-21s %-6u %-15s %-7u %-6u %-7u %-8u%s", ifname,tport->link.partner_spri,mac_str,
                      tport->link.partner_ppri, tport->link.partner_port,tport->link.partner_key, tport->link.partner_state,VTY_NEWLINE);
                }
            }
        }
   }  
}


void trunk_config_show(struct vty *vty, struct trunk *trunk_entry)
{
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
    char ifname[IFNET_NAMESIZE];

    if (NULL == trunk_entry)
    {
        return ;
    }

    if (TRUNK_ENABLE == trunk_entry->lacp_enable)
    {
        vty_out(vty, " Lacp : Enable%s", VTY_NEWLINE);
        vty_out(vty, " System priority : %u%s", trunk_entry->priority, VTY_NEWLINE);
        vty_out(vty, " Lacp interval : %u%s", trunk_entry->lacp_interval, VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, " Lacp : Disable%s", VTY_NEWLINE);
    }

    if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
    {
        vty_out(vty, " Work mode : Backup%s", VTY_NEWLINE);

        if (TRUNK_SWITCH_NOFAILBACK == trunk_entry->failback)
        {
            vty_out(vty, " Failback : No%s", VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, " Failback : Yes%s", VTY_NEWLINE);
            vty_out(vty, " Wtr : %u%s",trunk_entry->wtr, VTY_NEWLINE);
        }
    }
    else
    {
        vty_out ( vty, " Work mode : Ecmp%s",VTY_NEWLINE);

        if (TRUNK_BALANCE_LABEL == trunk_entry->ecmp_mode)
        {
            vty_out(vty, " Load balance : Label%s", VTY_NEWLINE);
        }
        else if (TRUNK_BALANCE_DIP == trunk_entry->ecmp_mode)
        {
            vty_out(vty, " Load balance : Dip%s", VTY_NEWLINE);
        }
        else if (TRUNK_BALANCE_SIP == trunk_entry->ecmp_mode)
        {
            vty_out(vty, " Load balance : Sip%s", VTY_NEWLINE);
        }
        else if (TRUNK_BALANCE_SIP_DIP == trunk_entry->ecmp_mode)
        {
            vty_out(vty, " Load balance : Sip-Dip%s", VTY_NEWLINE);
        }
        else if (TRUNK_BALANCE_SMAC == trunk_entry->ecmp_mode)
        {
            vty_out(vty, " Load balance : Smac%s", VTY_NEWLINE);
        }
        else if (TRUNK_BALANCE_DMAC == trunk_entry->ecmp_mode)
        {
            vty_out(vty, " Load balance : Dmac%s", VTY_NEWLINE);
        }
        else if (TRUNK_BALANCE_SMAC_DMAC == trunk_entry->ecmp_mode)
        {
            vty_out(vty, " Load balance : Smac-Dmac%s", VTY_NEWLINE);
        }
        else if (TRUNK_BALANCE_SPORT_DMAC == trunk_entry->ecmp_mode)
        {
            vty_out(vty, " Load balance : Sport-Dmac%s", VTY_NEWLINE);
        }
	else if(TRUNK_BALANCE_SPORT_DPORT == trunk_entry->ecmp_mode)
	{
		vty_out(vty,"Load balance : Sport-Dport%s",VTY_NEWLINE);
	}
    }

    if (trunk_entry->portlist.count != 0)
    {
        if (TRUNK_ENABLE == trunk_entry->lacp_enable)
        {
            vty_out(vty, " ----------------------------------%s", VTY_NEWLINE);
            vty_out(vty, " %-21s %-8s %-8s%s", "PortName" , "PortPri", "Passive", VTY_NEWLINE);

            for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
            {
                ifm_get_name_by_ifindex(tport->ifindex, ifname);
                vty_out(vty, " %-16s %-8u %-8s%s", ifname, tport->priority, (tport->passive == TRUNK_ENABLE) ? "Enable" : "Disable", VTY_NEWLINE);
            }
        }
        else
        {
            if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
            {
                vty_out(vty, " ------------------------%s", VTY_NEWLINE);
                vty_out(vty, " %-21s %-8s%s", "PortName", "PortPri", VTY_NEWLINE);

                for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
                {
	                ifm_get_name_by_ifindex(tport->ifindex, ifname); 
	                vty_out ( vty, " %-21s %-8u%s", ifname,tport->priority,VTY_NEWLINE);
	            }
        	}
			else
			{
				vty_out ( vty, " ------------%s",VTY_NEWLINE );
	            vty_out ( vty, " PortName%s",VTY_NEWLINE );
	            for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
	            {
	                ifm_get_name_by_ifindex(tport->ifindex, ifname); 
	                vty_out ( vty, " %-21s%s", ifname,VTY_NEWLINE);
	            }
			}
        }
    }

    return;
}


void trunk_cmd_init(void)
{
    TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

	install_node(&trunk_gloable_node, trunk_config_write_all);
	install_default(TRUNK_GLOABLE_NODE);
    install_element ( TRUNK_IF_NODE, &trunk_backup_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &trunk_backup_wtr_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &no_trunk_backup_wtr_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &trunk_load_balance_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &no_trunk_load_balance_cmd, CMD_SYNC );    
    install_element ( TRUNK_IF_NODE, &trunk_lacp_enable_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &no_trunk_lacp_enable_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &trunk_lacp_interval_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &no_trunk_lacp_interval_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &trunk_lacp_priority_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &no_trunk_lacp_priority_cmd, CMD_SYNC );
    install_element ( TRUNK_IF_NODE, &show_trunk_lacp_statistics_cmd, CMD_LOCAL );
    install_element ( PHYSICAL_IF_NODE, &trunk_member_port_set_cmd, CMD_SYNC );
    install_element ( PHYSICAL_IF_NODE, &no_trunk_member_port_set_cmd, CMD_SYNC );

	/*调试开关*/
    install_element ( CONFIG_NODE, &debug_trunk_lacp_packet_cmd, CMD_LOCAL  );
    install_element ( CONFIG_NODE, &no_debug_trunk_lacp_packet_cmd, CMD_LOCAL  );

    /*信息显示*/
    install_element ( CONFIG_NODE, &show_ifm_trunk_cmd, CMD_LOCAL  );
    install_element ( CONFIG_NODE, &show_ifm_trunk_all_cmd, CMD_LOCAL  );
    install_element ( CONFIG_NODE, &show_ifm_trunk_config_cmd, CMD_LOCAL  );
    install_element ( CONFIG_NODE, &show_ifm_trunk_all_config_cmd, CMD_LOCAL  );

    /*统计清除*/
    install_element ( TRUNK_IF_NODE, &clear_statistics_cmd, CMD_SYNC  );
    install_element ( CONFIG_NODE, &clear_if_statistics_cmd, CMD_SYNC  );

	/*删除聚合*/
    install_element ( CONFIG_NODE, &no_if_trunk_cmd, CMD_SYNC );

    /*added by yanjy,2018-4-27*/
   /*add the gloable cmd for HT157.vtss chip only has the gloable configation,*/
   /*which is different from the bcm chip.*/
	install_element(CONFIG_NODE,&gloable_trunk_load_balance_cmd,CMD_LOCAL);
	install_element(CONFIG_NODE,&no_gloable_trunk_load_balance_cmd,CMD_LOCAL);
	install_element(CONFIG_NODE,&gloable_trunk_link_aggregation_cmd,CMD_LOCAL);
	install_element(CONFIG_NODE,&no_gloable_trunk_link_aggregation_cmd,CMD_LOCAL);
	TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
}









