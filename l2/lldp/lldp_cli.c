/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_cli.c
*
*  date: 2016.8.5
*
*  modify:2016.9.22 add (lldp management-address (ip | mac))	cmd
*
*  modify:2017.4 lldp support trunk
*
*  modify:	2018.3.10 modified by liufuying to make LLDP module code beautiful
*
*/
#include <string.h>
#include <stdlib.h>

#include "lib/command.h"
#include "lib/module_id.h"
#include "ifm_common.h"
#include "memtypes.h"
#include "lib/memory.h"
#include "lib/vty.h"
#include "l2/l2_if.h"
#include "lib/hash1.h"
#include "lib/log.h"

#include "lldp_show.h"
#include "lldp.h"
#include "lldp_tx_sm.h"
#include "lldp_init.h"
#include "lldp_tlv.h"
#include "lldp_rx_sm.h"
#include "lldp_cli.h"
#include "trunk.h"

extern lldp_global 			lldp_global_msg;
extern struct hash_table 	l2if_table;

static struct cmd_node lldp_node =
{ 
LLDP_NODE,  
"%s(config-lldp)# ",  
1, 
};

DEFUN (lldp_mode,
	lldp_mode_cmd,
	"lldp",
	"in lldp mode\n")
{
	vty->node = LLDP_NODE;
	
	return CMD_SUCCESS;
}

/************** whole config ********************/

DEFUN(lldp_management_address, 
		lldp_management_address_cmd,
        "lldp management-address (ip | mac)",
        LLDP_STR
        "Management address for Network managment\n"
        "ip address\n"
        "mac address\n")
{
	unsigned int		manege_if_index = 0, ipaddr = 0;	
	struct ifm_l3		l3;
	int ret = 0;

	/*real config, ip addr may be change, so get again*/ 		
	if(!strncmp(argv[0], "ip", 1))
	{
		lldp_global_msg.mng_addr_sub = MGMT_ADDR_SUB_IPV4;

		#if 1
		memset(lldp_global_msg.source_ipaddr, 0, IPV4_LEN);

		/*get loopback 0 iP addr*/
		manege_if_index = ifm_get_ifindex_by_name("loopback", (char *)"128");
		ret = ifm_get_l3if(manege_if_index , MODULE_ID_L2, &l3);			
		if(ret)
		{
			zlog_err("%s:Error: get management IP timeout...\n", __func__);
		}
		else
		{			
			ipaddr = l3.ipv4[0].addr;
			lldp_global_msg.source_ipaddr[0] = (unsigned char)(ipaddr >> 24);
			lldp_global_msg.source_ipaddr[1] = (unsigned char)(ipaddr >> 16);
			lldp_global_msg.source_ipaddr[2] = (unsigned char)(ipaddr >> 8);
			lldp_global_msg.source_ipaddr[3] = (unsigned char)(ipaddr);
		}

		if(lldp_global_msg.source_ipaddr[0] == 0x00
			&&lldp_global_msg.source_ipaddr[1] == 0x00
			&&lldp_global_msg.source_ipaddr[2] == 0x00
			&&lldp_global_msg.source_ipaddr[3] == 0x00)
		{
			vty_error_out(vty,"Can not get management IP address...%s", VTY_NEWLINE);
		}
		#endif
	}
	else if(!strncmp(argv[0], "mac", 1))
	{	
		/*mac addr is not change*/
		lldp_global_msg.mng_addr_sub = MGMT_ADDR_SUB_ALL802;
	}			
	
	return CMD_SUCCESS;
}

/*set / clear packet interval*/        
DEFUN(lldp_packet_interval, 
		lldp_packet_interval_cmd,
        "lldp packet interval <1-3600>",
        LLDP_STR
        "LLDP packet\n"
        "The interval at which LLDP frames are transmitted\n"
        "The default value is 30s. Please input an integer from 1 to 3600\n")
{
	unsigned short		pkt_interval = 0;
	struct hash_bucket	*pbucket = NULL;
	int					cursor = 0;
	struct l2if			*pif = NULL;
	struct lldp_port	*port = NULL;

	pkt_interval = atoi(argv[0]);
	if((0 == pkt_interval) || (pkt_interval > 3600))
	{	
		vty_error_out(vty," Wrong parameter range, interval is <1-3600>. %s", VTY_NEWLINE);
			
		return CMD_SUCCESS;
	}

	if(	lldp_global_msg.msgTxInterval == pkt_interval)
	{
		return CMD_SUCCESS;
	}
	
	lldp_global_msg.msgTxInterval = pkt_interval;

	/*local config change, send lldp packet right now*/
	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif = pbucket->data;
		if(NULL == pif)
		{		
			continue;
		}
		
		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(port->adminStatus == LLDP_TRUE)
			{
				port->tx.somethingChangedLocal = LLDP_TRUE;
				port->tx.txTTR = lldp_global_msg.msgTxInterval;				
				port->tx.txTTL = min(65535, (lldp_global_msg.msgTxInterval * lldp_global_msg.msgTxHold));				
				
				lldp_tx_state_machine_action(port);
			}
		}
	}

	return CMD_SUCCESS;
}


DEFUN(no_lldp_packet_interval, 
		no_lldp_packet_interval_cmd,
        "no lldp packet interval",
        NO_STR
        LLDP_STR
        "LLDP packet\n"
        "The interval at which LLDP frames are transmitted\n")
{	
	struct hash_bucket	*pbucket = NULL;
	int					cursor = 0;
	struct l2if			*pif = NULL;
	struct lldp_port	*port = NULL;

	if(30 == lldp_global_msg.msgTxInterval)
	{
		return CMD_SUCCESS;
	}

	lldp_global_msg.msgTxInterval = 30;

	/*local config change, send lldp packet right now*/
	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif = pbucket->data;
		if(NULL == pif)
		{		
			continue;
		}
		
		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(LLDP_TRUE == port->adminStatus)
			{
				port->tx.somethingChangedLocal = LLDP_TRUE;

				port->tx.txTTR = lldp_global_msg.msgTxInterval;				
				port->tx.txTTL = min(65535, (lldp_global_msg.msgTxInterval * lldp_global_msg.msgTxHold));				
				lldp_tx_state_machine_action(port);
			}
		}
	}

	return CMD_SUCCESS;
}

/*set / clear hold-period*/ 	   
DEFUN(lldp_neighbour_hold_period, 
		lldp_neighbour_hold_period_cmd,
        "lldp neighbour hold-period <2-10>",
        LLDP_STR
        "a equipment or port which support LLDP function\n"	
        "A multiplier on the msgTxInterval that determines the actual TTL value use in a LLDPDU\n"
        "The default value is 4. Please input an integer from 2 to 10\n")
{
	unsigned char		hold_multiplier = 0;
	struct hash_bucket	*pbucket = NULL;
	int					cursor = 0;
	struct l2if			*pif = NULL;
	struct lldp_port	*port = NULL;

	hold_multiplier = atoi(argv[0]);

	if((hold_multiplier < 2) || (hold_multiplier > 10))
	{	
		vty_error_out(vty," Wrong parameter range, hold-period is <2-10>. %s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	if(lldp_global_msg.msgTxHold == hold_multiplier)
	{		
		return CMD_SUCCESS;
	}

	lldp_global_msg.msgTxHold = hold_multiplier;

	/*if do/not send packet, when change hold-period*/
	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}

		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(LLDP_TRUE == port->adminStatus)
			{
				port->tx.txTTR = lldp_global_msg.msgTxInterval;				
				port->tx.txTTL = min(65535, (lldp_global_msg.msgTxInterval * lldp_global_msg.msgTxHold));				
							
				port->tx.somethingChangedLocal = LLDP_TRUE;
				lldp_tx_state_machine_action(port);
			}
		}
	}

	return CMD_SUCCESS;
}


DEFUN(no_lldp_neighbour_hold_period, 
		no_lldp_neighbour_hold_period_cmd,
        "no lldp neighbour hold-period",
        NO_STR
        LLDP_STR
        "A equipment or port which support LLDP function\n"	
        "A multiplier on the msgTxInterval that determines the actual TTL value use in a LLDPDU\n")
{
	struct hash_bucket	*pbucket = NULL;
	int 				cursor = 0;
	struct l2if			*pif = NULL;
	struct lldp_port	*port = NULL;

	if( 4 == lldp_global_msg.msgTxHold)
	{
		return CMD_SUCCESS;
	}
	
	lldp_global_msg.msgTxHold = 4;

	/*if do/not send packet, when change hold-period*/
	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif = pbucket->data;
		if(pif == NULL)
		{		
			continue;
		}
		
		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(port->adminStatus == LLDP_TRUE)
			{
				port->tx.txTTR = lldp_global_msg.msgTxInterval;				
				port->tx.txTTL = min(65535, (lldp_global_msg.msgTxInterval * lldp_global_msg.msgTxHold));				
				
				port->tx.somethingChangedLocal = LLDP_TRUE;
				lldp_tx_state_machine_action(port);
			}
		}
	}

	return CMD_SUCCESS;
}

#if 0
/*set / clear trap-interval*/ 	   
DEFUN(lldp_trap_interval, 
		lldp_trap_interval_cmd,
        "lldp trap-interval <5-3600>",
        LLDP_STR
        "This parameter controls the trap-interval of LLDP notifications\n"	
        "The default value is 5s. Please input an integer from 5 to 3600\n")
{
	unsigned short trapInterval;

	trapInterval = atoi(argv[0]);

	if((trapInterval < 5) || (trapInterval > 3600))
	{	
		vty_error_out(vty," Wrong parameter range, trap-interval is <5-3600>. %s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	if(lldp_global_msg.trapInterval == trapInterval)
	{		
		return CMD_WARNING;
	}

	lldp_global_msg.trapInterval = trapInterval;
	/*do something*/

	return CMD_SUCCESS;
}


DEFUN(no_lldp_trap_interval, 
		no_lldp_trap_interval_cmd,
        "no lldp trap-interval",   
        NO_STR
        LLDP_STR
		"This parameter controls the trap-interval of LLDP notifications\n")
{
	if( 5 == lldp_global_msg.trapInterval)
	{
		return CMD_WARNING;
	}

	lldp_global_msg.trapInterval = 5;
	/*do something*/
	

	return CMD_SUCCESS;
}
#endif
/************** whole config end ********************/


/************** show ********************/
DEFUN (show_lldp_statistics,
		show_lldp_statistics_cmd,
        "show lldp statistics interface {ethernet USP |gigabitethernet USP |xgigabitethernet USP | trunk TRUNK}",
        SHOW_STR
        LLDP_STR        
        "LLDP statistics information\n"
        "Interface\n"
        "Ethernet interface type\n"
        "The port/subport of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n")
{
	struct l2if			*pif = NULL;
	struct lldp_port	*port = NULL;
	unsigned int 		if_index = 0;

	/* argv[0] : interface name , ethernet interface*/
	if(((NULL != argv[0])||(NULL != argv[1])||(NULL != argv[2])) && (NULL == argv[3]))
	{
		if((argv[0] != NULL) && (NULL == argv[1]) && (NULL == argv[2]))
	    {
	    	if_index = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
		}
		else if((NULL == argv[0]) && (argv[1] != NULL) && (NULL == argv[2]))
	    {
	    	if_index = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[1] );
		}
		else if((NULL == argv[0]) && (NULL == argv[1]) && (argv[2] != NULL))
	    {
	    	if_index = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[2] );
		}
		if(0 == if_index)	// 0 means interface not exist
		{
			zlog_err("%s: ERROR: get if index is NULL!\n", __func__);
			vty_error_out(vty," Please check your parameter, this para get interface NULL. %s", VTY_NEWLINE);
			
			return CMD_WARNING;
		}

		pif = l2if_get(if_index);
		if(NULL == pif)
		{
			zlog_err("%s[%d]%s:	pif is NULL.\n",__FILE__,__LINE__,__func__);

			vty_error_out(vty," get interface NULL. %s", VTY_NEWLINE);
			
			return CMD_WARNING;
		}

		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(LLDP_TRUE == port->adminStatus)
			{
				lldp_show_if_statistics(vty, port);
				return CMD_SUCCESS;
			}
			else
			{
				zlog_err("%s: Error: The LLDP is not enabled on this port!\n", __func__);
				vty_error_out(vty, " The LLDP is not enabled on this port!%s", VTY_NEWLINE);
				return CMD_WARNING;				
			}			
		}
		else
		{
			vty_error_out(vty, " No the interface lldp info, Maybe the interface no enabled lldp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}		
	}
	else if((NULL != argv[3]) && (NULL == argv[0])&&(NULL == argv[1])&&(NULL == argv[2]))	/*trunk interface*/
	{
		if(NULL == trunk_lookup(atoi(argv[3])))
		{
			vty_error_out(vty, " No lldp info, Maybe the interface no enabled lldp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		if_index = ifm_get_ifindex_by_name("trunk", (char *)argv[3]);
		if(0 == if_index)	/*0 means interface not exist*/
		{
			vty_error_out(vty, " Get trunk index is NULL! Maybe the interface no enabled lldp.%s", VTY_NEWLINE);

			return CMD_WARNING;
		}

		pif = l2if_get(if_index);
		if(NULL == pif)
		{
			zlog_err("%s[%d]%s:	pif is NULL.\n",__FILE__,__LINE__,__func__);

			vty_error_out(vty,"  get interface null. %s", VTY_NEWLINE);
			
			return CMD_WARNING;
		}
		
		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(LLDP_TRUE == port->adminStatus)
			{
				lldp_show_if_statistics(vty, port);
				
				return CMD_SUCCESS;
			}
			else
			{
				zlog_err("%s: Error: The LLDP is not enabled on this trunk!\n", __func__);
				vty_error_out(vty," The LLDP is not enabled on this trunk! %s", VTY_NEWLINE);
				return CMD_WARNING;				
			}			
		}
		else
		{
			vty_error_out(vty, " No trunk lldp info, Maybe the interface no enabled lldp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}		
	}
	else if((NULL == argv[0]) && (NULL == argv[1])&& (NULL == argv[2])&& (NULL == argv[3]))
	{
		lldp_show_statistics(vty);
		
		return CMD_SUCCESS;
	}

	
	return CMD_SUCCESS;
}

DEFUN (show_lldp_local,
		show_lldp_local_cmd,
        "show lldp local interface {ethernet USP |gigabitethernet USP |xgigabitethernet USP | trunk TRUNK}",
        SHOW_STR
        LLDP_STR        
        "LLDP information about the local device or ports\n"
        "Interface\n"
        "Ethernet interface type\n"
        "The port/subport of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_TRUNK_STR
       "The port of trunk, format: <1-128>\n")
{
	struct l2if			*pif = NULL;
	struct lldp_port	*port = NULL;
	unsigned int 		if_index = 0;

	/* argv[0] : interface name */
	if(((NULL != argv[0])||(NULL != argv[1])||(NULL != argv[2])) && (NULL == argv[3]))
	{
		if((argv[0] != NULL) && (NULL == argv[1]) && (NULL == argv[2]))
	    {
	    	if_index = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
		}
		else if((NULL == argv[0]) && (argv[1] != NULL) && (NULL == argv[2]))
	    {
	    	if_index = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[1] );
		}
		else if((NULL == argv[0]) && (NULL == argv[1]) && (argv[2] != NULL))
	    {
	    	if_index = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[2] );
		}
		if(0 == if_index)	// 0 means interface not exist
		{
			zlog_err("%s: ERROR: get if index is NULL!\n", __func__);
			
			vty_error_out(vty," Please check your parameter, this para get interface null. %s", VTY_NEWLINE);

			return CMD_WARNING;
		}

		pif = l2if_get(if_index);
		if(NULL == pif)
		{
			zlog_err("%s[%d]:%s	pif is NULL.\n",__FILE__,__LINE__,__func__);
			
			vty_error_out(vty," this para get interface null. %s", VTY_NEWLINE);
			
			return CMD_WARNING;
		}

		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(LLDP_TRUE == port->adminStatus)
			{
				lldp_show_if_local(vty, port, pif);
			}
			else
			{
				zlog_err("%s: Error: The LLDP is not enabled on this port!\n", __func__);
				vty_error_out(vty," The LLDP is not enabled on this port! %s", VTY_NEWLINE);
				return CMD_WARNING;				
			}
		}
		else
		{
			vty_error_out(vty, " No the interface lldp info, Maybe the interface no enabled lldp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}		
	}
 	else if((NULL != argv[3]) && (NULL == argv[0])&&(NULL == argv[1])&&(NULL == argv[2]))	/*trunk interface*/
	{
		if(NULL == trunk_lookup(atoi(argv[3])))
		{
			vty_error_out(vty, " No lldp info, Maybe the trunk no enabled lldp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		if_index = ifm_get_ifindex_by_name("trunk", (char *)argv[3]);
		if(0 == if_index) 	/* 0 means trunk not exist*/
		{
			zlog_err("%s: ERROR: get trunk index is NULL!\n", __func__);
			
			vty_error_out(vty," Please check your parameter, this para get trunk interface null. %s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		pif = l2if_get(if_index);
		if(NULL == pif)
		{
			zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);

			vty_error_out(vty," this para get trunk interface null. %s", VTY_NEWLINE);
			
			return CMD_WARNING;
		}
	
		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(port->adminStatus == LLDP_TRUE)
			{
				lldp_show_if_local(vty, port, pif);
			}
			else
			{
				zlog_err("%s: Error: The LLDP is not enabled on this trunk!\n", __func__);
				vty_error_out(vty,"  The LLDP is not enabled on this trunk! %s", VTY_NEWLINE);

				return CMD_WARNING; 			
			}
		}
	}
	else if((NULL == argv[0]) && (NULL == argv[1])&& (NULL == argv[2])&& (NULL == argv[3]))
	{
		lldp_show_local(vty);		
	}

	return CMD_SUCCESS;
}

DEFUN (show_lldp_neighbor,
		show_lldp_neighbor_cmd,
        "show lldp neighbor interface {ethernet USP |gigabitethernet USP |xgigabitethernet USP | trunk TRUNK}",
        SHOW_STR
        LLDP_STR        
        "Neighbor information\n"
        "Interface\n"
        "Ethernet interface type\n"
        "The port/subport of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n")
{
	struct l2if			*pif = NULL;
	struct lldp_port	*port = NULL;
	unsigned int 		if_index = 0;

	/* argv[0] : interface name */
	if(((NULL != argv[0])||(NULL != argv[1])||(NULL != argv[2])) && (NULL == argv[3]))
	{
		if((argv[0] != NULL) && (NULL == argv[1]) && (NULL == argv[2]))
	    {
	    	if_index = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
		}
		else if((NULL == argv[0]) && (argv[1] != NULL) && (NULL == argv[2]))
	    {
	    	if_index = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[1] );
		}
		else if((NULL == argv[0]) && (NULL == argv[1]) && (argv[2] != NULL))
	    {
	    	if_index = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[2] );
		}
		if(0 == if_index)	// 0 means interface not exist
		{
			zlog_err("%s: ERROR: get if index is NULL!\n", __func__);
			vty_error_out(vty," Please check your parameter, this para get interface null. %s", VTY_NEWLINE);
			
			return CMD_WARNING;
		}

		pif = l2if_get(if_index);
		if(NULL == pif)
		{
			zlog_err("%s[%d]:%s	pif is NULL.\n",__FILE__,__LINE__,__func__);
			
			vty_error_out(vty," get interface null. %s", VTY_NEWLINE);
			
			return CMD_WARNING;
		}

		port = pif->lldp_port_info;
		if(port != NULL)
		{
			lldp_show_if_neighbor(vty, port);
		}
		else
		{
			vty_error_out(vty, " No the interface lldp info, Maybe the interface no enabled lldp.	%s", VTY_NEWLINE);
			return CMD_WARNING;
		}		
	}
	else if((NULL != argv[3]) && (NULL == argv[0])&&(NULL == argv[1])&&(NULL == argv[2]))	/*trunk interface*/
	{
		if(NULL == trunk_lookup(atoi(argv[3])))
		{
			vty_error_out(vty, " No lldp info, Maybe the trunk no enabled lldp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		if_index = ifm_get_ifindex_by_name("trunk", (char *)argv[3]);
		if(0 == if_index)	/*0 means trunk not exist*/ 
		{
			zlog_err("%s: ERROR: get trunk index is NULL!\n", __func__);
			vty_error_out(vty," this para get trunk interface is null. %s", VTY_NEWLINE);

			return CMD_WARNING;
		}

		pif = l2if_get(if_index);
		if(NULL == pif)
		{
			zlog_err("%s[%d]:%s	trunk is NULL.\n",__FILE__,__LINE__,__func__);
			
			vty_error_out(vty," this para get trunk interface null. %s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		port = pif->lldp_port_info;
		if(port != NULL)
		{
			lldp_show_if_neighbor(vty, port);
		}
		else
		{
			vty_error_out(vty, " No the trunk lldp info, Maybe the trunk no enabled lldp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	else if((NULL == argv[0]) && (NULL == argv[1])&& (NULL == argv[2])&& (NULL == argv[3]))
	{
		lldp_show_neighbor(vty);		
	}

	return CMD_SUCCESS;	
}


DEFUN (show_lldp_tlv_config,
		show_lldp_tlv_config_cmd,
        "show lldp tlv-config interface {ethernet USP |gigabitethernet USP |xgigabitethernet USP | trunk TRUNK}",
        SHOW_STR
        LLDP_STR
        "Enable TLV information\n"
        "Interface\n"
        "Ethernet interface type\n"
        "The port/subport of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
		CLI_INTERFACE_TRUNK_STR
        "The port of trunk, format: <1-128>\n")
{
	struct l2if			*pif = NULL;
	struct lldp_port	*port = NULL;
	unsigned int 		if_index = 0;

	/* argv[0] : interface name */
	if(((NULL != argv[0])||(NULL != argv[1])||(NULL != argv[2])) && (NULL == argv[3]))
	{
		if((argv[0] != NULL) && (NULL == argv[1]) && (NULL == argv[2]))
	    {
	    	if_index = ifm_get_ifindex_by_name ( "ethernet", ( char * ) argv[0] );
		}
		else if((NULL == argv[0]) && (argv[1] != NULL) && (NULL == argv[2]))
	    {
	    	if_index = ifm_get_ifindex_by_name ( "gigabitethernet", ( char * ) argv[1] );
		}
		else if((NULL == argv[0]) && (NULL == argv[1]) && (argv[2] != NULL))
	    {
	    	if_index = ifm_get_ifindex_by_name ( "xgigabitethernet", ( char * ) argv[2] );
		}
		if(0 == if_index)	// 0 means interface not exist
		{
			zlog_err("%s: ERROR: get if index is NULL!\n", __func__);
			
			vty_error_out(vty," Please check your parameter, this para get interface null. %s", VTY_NEWLINE);

			return CMD_WARNING;
		}

		pif = l2if_get(if_index);
		if(NULL == pif)
		{
			zlog_err("%s[%d]: %s	pif is NULL.\n",__FILE__,__LINE__,__func__);
			
			vty_error_out(vty,"  get interface null. %s", VTY_NEWLINE);
			
			return CMD_WARNING;
		}

		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(LLDP_TRUE == port->adminStatus)
			{
				lldp_show_if_tlv_cfg(vty, port);
			}
			else
			{
				zlog_err("%s: Error: The LLDP is not enabled on this port!\n", __func__);

				vty_error_out(vty," The LLDP is not enabled on this port! %s", VTY_NEWLINE);

				return CMD_WARNING;				
			}			
		}
		else
		{
			vty_error_out(vty, " No the interface lldp info, maybe the interface no enabled lldp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}		
	}
	else if((NULL != argv[3]) && (NULL == argv[0])&&(NULL == argv[1])&&(NULL == argv[2]))	/*trunk interface*/
	{
		if(NULL == trunk_lookup(atoi(argv[3])))
		{
			vty_error_out(vty, " Please check your parameter, this para get interface null.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	
		if_index = ifm_get_ifindex_by_name("trunk", (char *)argv[3]);
		if(0 == if_index)	/*0 means trunk not exist*/
		{
			zlog_err("%s: ERROR: get trunk index is NULL!\n", __func__);
			
			vty_error_out(vty," this para get interface index null. %s", VTY_NEWLINE);
			
			return CMD_WARNING;
		}

		pif = l2if_get(if_index);
		if(NULL == pif)
		{
			zlog_err("%s[%d]: %s	trunk is NULL.\n",__FILE__,__LINE__,__func__);
			vty_error_out(vty,"  get interface null.%s", VTY_NEWLINE);

			return CMD_WARNING;
		}

		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(LLDP_TRUE == port->adminStatus)
			{
				lldp_show_if_tlv_cfg(vty, port);
			}
			else
			{
				zlog_err("%s: Error: The LLDP is not enabled on this trunk!\n", __func__);
				vty_error_out(vty," The LLDP is not enabled on this trunk! %s", VTY_NEWLINE);
				
				return CMD_WARNING;				
			}			
		}
		else
		{
			vty_error_out(vty, " No the trunk lldp info, maybe the trunk no enabled lldp.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}		
	}
	else if((NULL == argv[0]) && (NULL == argv[1])&& (NULL == argv[2])&& (NULL == argv[3]))
	{
		lldp_show_tlv_cfg(vty);		
	}

	return CMD_SUCCESS;		
}

/************** show end ********************/

/************** interface config option TLV ********************/

DEFUN(interface_lldp_enable, 
		interface_lldp_enable_cmd,
		"lldp enable",
		LLDP_STR
		"Enable LLDP protocol\n")
{
	struct lldp_port	*port = NULL;
	unsigned int		if_index = 0;	
	struct l2if			*pif = NULL;
	unsigned char		status;
	int ret = 0;

	if_index = (unsigned int)vty->index;
	
	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," get interface null. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	/*judge the port mode*/
	// 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
	if (IFNET_MODE_INVALID == pif->mode || IFNET_MODE_PHYSICAL == pif->mode)
	{
		vty_info_out(vty," The port mode is invalid, LLDP can not enabled.%s", VTY_NEWLINE);
		return CMD_WARNING;	
	}

	//only support physical port
	if (!IFM_TYPE_IS_PHYSICAL(pif->ifindex))
	{
		vty_info_out(vty," Only support physical port.%s", VTY_NEWLINE);
		return CMD_WARNING;	
	}

	port = pif->lldp_port_info;
	if(port != NULL)
	{
		if(LLDP_FALSE == port->adminStatus)
		{
			/**/
			zlog_err("%s: ERROR: the lldp port/trunk should not exist!\n", __func__);
			
			vty_error_out(vty," the lldp port/trunk should not exist! %s", VTY_NEWLINE);
			
			return CMD_WARNING;
		}
		else if(LLDP_TRUE == port->adminStatus)
		{
			vty_info_out(vty," LLDP on the port/trunk is already enabled.%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	else
	{
		if(  lldp_global_msg.source_mac[0] == 0x00
		  && lldp_global_msg.source_mac[1] == 0x00
		  && lldp_global_msg.source_mac[2] == 0x00
		  && lldp_global_msg.source_mac[3] == 0x00
		  && lldp_global_msg.source_mac[4] == 0x00
		  && lldp_global_msg.source_mac[5] == 0x00)
		{
			zlog_err("%s: ERROR: sys MAC addr is NULL!\n", __func__);
			vty_error_out(vty," SYS MAC addr is NULL, can not enable lldp.%s", VTY_NEWLINE);

			return CMD_WARNING;
		}
	
		pif->lldp_port_info = (struct lldp_port*)XMALLOC(MTYPE_LLDP, sizeof(struct lldp_port));
		if(NULL == pif->lldp_port_info)
		{		
			zlog_err("%s: ERROR: calloc new port memery error!\n", __func__);
			vty_error_out(vty," calloc new port memery error! %s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		memset(pif->lldp_port_info, 0, sizeof(struct lldp_port));
			
		/*get interface info*/
		ret = ifm_get_link(if_index, MODULE_ID_L2, &status); 
		if(!ret)
		{
			if(IFNET_LINKUP == status)
			{
				pif->lldp_port_info->portEnabled = LLDP_TRUE;
			}
			else
			{
				pif->lldp_port_info->portEnabled = LLDP_FALSE;				
			}			
		}
		else
		{		
			pif->lldp_port_info->portEnabled = LLDP_FALSE;
		}
		
		pif->lldp_port_info->adminStatus = LLDP_TRUE;
	
		ifm_get_name_by_ifindex(if_index ,pif->lldp_port_info->if_name);
		pif->lldp_port_info->if_index = if_index;

		memcpy(pif->lldp_port_info->if_mac, pif->mac, 6);
		
		/*get sys Capabilities*/
		pif->lldp_port_info->sys_cap = 0x0014; /*bit3:MAC Bridge &bit5:Router*/
		pif->lldp_port_info->sys_cap_enable = 0x0014;
		
		pif->lldp_port_info->rxChanges = LLDP_FALSE;
		pif->lldp_port_info->arp_nd_learning = 0;
		
		/*enable all tlv*/
		pif->lldp_port_info->tlv_cfg.basic_tlv_tx_enable = (PORT_DESCRIPTION_TLV_TX_ENABLE
												| SYSTEM_NAME_TLV_TX_ENABLE
												| SYSTEM_DESCRIPTION_TLV_TX_ENABLE
												| SYSTEM_CAPABILITIES_TLV_TX_ENABLE
												| MANAGEMENT_ADDRESS_TLV_TX_ENABLE);
		
		pif->lldp_port_info->max_frame_size = 9600;
	
		lldp_init_port_rx(pif->lldp_port_info);
		lldp_init_port_tx(pif->lldp_port_info);
		
		pif->lldp_port_info->msap_num = 0;
		pif->lldp_port_info->msap_cache = NULL;

		lldp_tx_state_machine_action(pif->lldp_port_info);
		if(TX_IDLE == pif->lldp_port_info->tx.state)
		{
			/*send frame*/
			lldp_send_frame(pif->lldp_port_info);
		}
		lldp_rx_state_machine_action(pif->lldp_port_info);

		vty_info_out(vty," The LLDP is enabled on the port successfully.%s", VTY_NEWLINE);		
	}

	return CMD_SUCCESS;		
}


DEFUN(no_interface_lldp_enable, 
		no_interface_lldp_enable_cmd,
        "no lldp enable",
        NO_STR
        LLDP_STR
        "Enable LLDP protocol\n")
{
	unsigned int		if_index = 0;
	struct lldp_port	*port = NULL;
	struct l2if			*pif = NULL;

	if_index = (unsigned int)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	/*judge the port mode*/
	// 2018-05-04, by caojt, add IFNET_MODE_PHYSICAL for L1 ethernet port
	if (IFNET_MODE_INVALID == pif->mode || IFNET_MODE_PHYSICAL == pif->mode)
	{
		vty_info_out(vty," The port mode is invalid, LLDP can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;	
	}

	port = pif->lldp_port_info;
	if(port != NULL)
	{
		if(LLDP_TRUE == port->adminStatus)
		{
			port->adminStatus = LLDP_FALSE;

			/*need to clean lldp msg*/
//			lldp_rx_state_machine_action(port);
			lldp_tx_state_machine_action(port);

			vty_info_out(vty," The LLDP is disable on the port successfully.%s", VTY_NEWLINE);			
		}
		else if(port->adminStatus == LLDP_FALSE)
		{
			zlog_err("%s: Error: LLDP on the port is already disabled, and the port should not exist!\n", __func__);

			vty_error_out(vty," LLDP on the port is already disabled, and the port should not exist! %s", VTY_NEWLINE);

			return CMD_WARNING;
		}
	}
	else
	{
		vty_error_out(vty, " No the interface lldp info, maybe the interface no enabled lldp.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}	

	return CMD_SUCCESS;		
}


DEFUN(interface_tlv_enable_basic_tlv, 
		interface_tlv_enable_basic_tlv_cmd,
		"lldp tlv-enable basic-tlv (all | port-description |  \
									system-name | system-description \
									| system-capability | management-address)",
		LLDP_STR
		"Enable optional TLV\n"
		"LLDP basic TLV configuration\n"
		"Enable basic all TLV\n"	
		"Enable basic port description TLV\n"
		"Enable basic system name TLV\n"
		"Enable basic system description TLV\n"
		"Enable basic system capability TLV\n"
		"Enable basic management address TLV\n")
{
	unsigned int		if_index = 0;
	struct lldp_port	*port = NULL;
	struct l2if			*pif = NULL;

	if_index = (unsigned int)vty->index;
	LLDP_LOG_DBG("%s:Info: ifindex %08X	...\n", __func__, if_index);

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," get interface null! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	port = pif->lldp_port_info;
	if(port != NULL)
	{
		if(LLDP_TRUE == port->adminStatus)
		{
			/*real config*/ 		
			if(!strncmp(argv[0], "all", 1))
			{
				port->tlv_cfg.basic_tlv_tx_enable = 0x00;
				port->tlv_cfg.basic_tlv_tx_enable = (PORT_DESCRIPTION_TLV_TX_ENABLE
													| SYSTEM_NAME_TLV_TX_ENABLE
													| SYSTEM_DESCRIPTION_TLV_TX_ENABLE
													| SYSTEM_CAPABILITIES_TLV_TX_ENABLE
													| MANAGEMENT_ADDRESS_TLV_TX_ENABLE);
			}
			else if(!strncmp(argv[0], "port-description", 1))
			{
				port->tlv_cfg.basic_tlv_tx_enable |= PORT_DESCRIPTION_TLV_TX_ENABLE;
			}			
			else if(!strncmp(argv[0], "system-name", 8))
			{
				port->tlv_cfg.basic_tlv_tx_enable |= SYSTEM_NAME_TLV_TX_ENABLE;
			}
			else if(!strncmp(argv[0], "system-description", 8))
			{
				port->tlv_cfg.basic_tlv_tx_enable |= SYSTEM_DESCRIPTION_TLV_TX_ENABLE;
			}
			else if(!strncmp(argv[0], "system-capability", 8))
			{
				port->tlv_cfg.basic_tlv_tx_enable |= SYSTEM_CAPABILITIES_TLV_TX_ENABLE;
			}
			else if(!strncmp(argv[0], "management-address", 1))
			{
				port->tlv_cfg.basic_tlv_tx_enable |= MANAGEMENT_ADDRESS_TLV_TX_ENABLE;
			}
			else
			{				
				vty_error_out(vty, " LLDP error TLV parameter.%s", VTY_NEWLINE);
			}
		}
		else if(port->adminStatus == LLDP_FALSE)
		{
			zlog_err("%s: Error: LLDP on the port is already disabled, and the port should not exist!\n", __func__);
			vty_error_out(vty," LLDP on the port is already disabled, and the port should not exist!%s", VTY_NEWLINE);

			return CMD_WARNING;
		}
	}
	else
	{
		vty_error_out(vty, " No the interface lldp info, maybe the interface no enabled lldp%s", VTY_NEWLINE);
		return CMD_WARNING;
	}	

	return CMD_SUCCESS;			
}


DEFUN(no_interface_tlv_enable_basic_tlv, 
		no_interface_tlv_enable_basic_tlv_cmd,
		"no lldp tlv-enable basic-tlv (all | port-description | \
									system-name | system-description \
									| system-capability | management-address)",
		NO_STR
		LLDP_STR
		"Enable optional TLV\n"
		"LLDP basic TLV configuration\n"
		"Enable basic all TLV\n"	
		"Enable basic port description TLV\n"
		"Enable basic system name TLV\n"
		"Enable basic system description TLV\n"
		"Enable basic system capability TLV\n"
		"Enable basic management address TLV\n")
{
	unsigned int		if_index = 0;
	struct lldp_port	*port = NULL;
	struct l2if			*pif = NULL;

	if_index = (unsigned int)vty->index;
	LLDP_LOG_DBG("%s:Info: ifindex %08X	...\n", __func__, if_index);

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);		
		vty_error_out(vty,"  get interface null. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	port = pif->lldp_port_info;
	if(port != NULL)
	{
		if(LLDP_TRUE == port->adminStatus)
		{
			/*real config*/ 		
			if(!strncmp(argv[0], "all", 1))
			{
				port->tlv_cfg.basic_tlv_tx_enable = 0x00;
			}
			else if(!strncmp(argv[0], "port-description", 1))
			{
				port->tlv_cfg.basic_tlv_tx_enable &= ~PORT_DESCRIPTION_TLV_TX_ENABLE;
			}			
			else if(!strncmp(argv[0], "system-name", 8))
			{
				port->tlv_cfg.basic_tlv_tx_enable &= ~SYSTEM_NAME_TLV_TX_ENABLE;
			}
			else if(!strncmp(argv[0], "system-description", 8))
			{
				port->tlv_cfg.basic_tlv_tx_enable &= ~SYSTEM_DESCRIPTION_TLV_TX_ENABLE;
			}
			else if(!strncmp(argv[0], "system-capability", 8))
			{
				port->tlv_cfg.basic_tlv_tx_enable &= ~SYSTEM_CAPABILITIES_TLV_TX_ENABLE;
			}
			else if(!strncmp(argv[0], "management-address", 1))
			{
				port->tlv_cfg.basic_tlv_tx_enable &= ~MANAGEMENT_ADDRESS_TLV_TX_ENABLE;
			}
			else
			{				
				vty_error_out(vty, " LLDP TLV parameter.%s", VTY_NEWLINE);
			}
		}
		else if(LLDP_FALSE == port->adminStatus)
		{
			zlog_err("%s: Error: LLDP on the port is already disabled, and the port should not exist!\n", __func__);
			vty_error_out(vty," LLDP on the port is already disabled, and the port should not exist!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	else
	{
		vty_error_out(vty, " No the interface lldp info, maybe the interface no enabled lldp.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}		

	return CMD_SUCCESS;
}


#if 0 /*do not support*/
DEFUN(interface_tlv_enable_dot1_tlv, 
		interface_tlv_enable_dot1_tlv_cmd,
		"lldp tlv-enable dot1-tlv(all | port-vlan-id | protocol-vlan-id \
									| vlan-name | protocol-identity)",
		LLDP_STR
		"Enable optional TLV\n"
		"Enable 802.1 TLV\n"
		"Enable 802.1 all TLV\n"
		"Enable 802.1 port vlan id TLV\n"	
		"Enable 802.1 protocol vlan TLV\n"
		"Enable 802.1 vlan name TLV\n"
		"Enable 802.1 protocol identity TLV\n")
{
	unsigned int		if_index = 0;
	struct lldp_port	*port;
	unsigned char		find_port = FALSE;



	if_index = (vty->index);
	vty_info_out(vty," Ifindex %08X		%s", if_index, VTY_NEWLINE);	

	/*get the lldp port*/
	if(lldp_global_msg.portsHead != NULL)
	{
		port = lldp_global_msg.portsHead;
		while(port != NULL)
		{
			if(port->if_index == if_index)
			{
				find_port = TRUE;
				break;
			}
			port = port->next;
		}
	}
	else
	{
		vty_error_out(vty, " LLDP No port list.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}	

	if(find_port == TRUE)
	{
		if(port->adminStatus == TRUE)
		{
			/*real config*/			
			if(!strncmp(argv[0],"all",1))
			{			
				port->tlv_cfg.sub_tlv_tx_enable |= (PORT_VLAN_ID_TX_ENABLE
												| PROTO_VLAN_ID_TX_ENABLE
												| PORT_VLAN_NAME_TX_ENABLE
												| PROTOCOL_ID_TX_ENABLE);
			}
			else if(!strncmp(argv[0],"port-vlan-id",2))
			{
				port->tlv_cfg.sub_tlv_tx_enable |= PORT_VLAN_ID_TX_ENABLE;
			}			
			else if(!strncmp(argv[0],"protocol-vlan-id",10))
			{
				port->tlv_cfg.sub_tlv_tx_enable |= PROTO_VLAN_ID_TX_ENABLE;
			}
			else if(!strncmp(argv[0],"vlan-name",1))
			{
				port->tlv_cfg.sub_tlv_tx_enable |= PORT_VLAN_NAME_TX_ENABLE;
			}
			else if(!strncmp(argv[0],"protocol-identity",10))
			{
				port->tlv_cfg.sub_tlv_tx_enable |= PROTOCOL_ID_TX_ENABLE;
			}
			else
			{				
				vty_error_out(vty, " LLDP error TLV parameter.%s", VTY_NEWLINE);
			}
		}
		else if(port->adminStatus == FALSE)
		{
			vty_error_out(vty," The LLDP is not enabled on this port.%s", VTY_NEWLINE);		
			return CMD_WARNING;
		}
	}
	else
	{
		vty_error_out(vty, " LLDP port list no this port.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}		

	return CMD_SUCCESS;
}



DEFUN(no_interface_tlv_enable_dot1_tlv, 
		no_interface_tlv_enable_dot1_tlv_cmd,
		"no lldp tlv-enable dot1-tlv(all | port-vlan-id | protocol-vlan-id \
									| vlan-name | protocol-identity)",
		NO_STR
		LLDP_STR
		"Enable optional TLV\n"
		"Enable 802.1 TLV\n"
		"Enable 802.1 all TLV\n"
		"Enable 802.1 port vlan id TLV\n"	
		"Enable 802.1 protocol vlan TLV\n"
		"Enable 802.1 vlan name TLV\n"
		"Enable 802.1 protocol identity TLV\n")
{
	unsigned int		if_index = 0;
	struct lldp_port	*port;
	unsigned char		find_port = FALSE;



	if_index = vty->index;
	vty_info_out(vty," Ifindex %08X		%s", if_index, VTY_NEWLINE);	

	/*get the lldp port*/
	if(lldp_global_msg.portsHead != NULL)
	{
		port = lldp_global_msg.portsHead;
		while(port != NULL)
		{
			if(port->if_index == if_index)
			{
				find_port = TRUE;
				break;
			}
			port = port->next;
		}
	}
	else
	{
		vty_error_out(vty, " LLDP No port list.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}	

	if(find_port == TRUE)
	{
		if(port->adminStatus == TRUE)
		{
			/*real config*/			
			if(!strncmp(argv[0],"all",1))
			{			
				port->tlv_cfg.sub_tlv_tx_enable &= ~(PORT_VLAN_ID_TX_ENABLE
												| PROTO_VLAN_ID_TX_ENABLE
												| PORT_VLAN_NAME_TX_ENABLE
												| PROTOCOL_ID_TX_ENABLE);
			}
			else if(!strncmp(argv[0],"port-vlan-id",4))
			{
				port->tlv_cfg.sub_tlv_tx_enable &= ~PORT_VLAN_ID_TX_ENABLE;
			}			
			else if(!strncmp(argv[0],"protocol-vlan-id",10))
			{
				port->tlv_cfg.sub_tlv_tx_enable &= ~PROTO_VLAN_ID_TX_ENABLE;
			}
			else if(!strncmp(argv[0],"vlan-name",1))
			{
				port->tlv_cfg.sub_tlv_tx_enable &= ~PORT_VLAN_NAME_TX_ENABLE;
			}
			else if(!strncmp(argv[0],"protocol-identity",10))
			{
				port->tlv_cfg.sub_tlv_tx_enable &= ~PROTOCOL_ID_TX_ENABLE;
			}
			else
			{				
				vty_error_out(vty, " LLDP error TLV parameter.%s", VTY_NEWLINE);
			}
		}
		else if(port->adminStatus == FALSE)
		{
			vty_error_out(vty," The LLDP is not enabled on this port.%s", VTY_NEWLINE);		
			return CMD_WARNING;
		}
	}
	else
	{
		vty_error_out(vty, " LLDP port list no this port.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}		

	return CMD_SUCCESS;
}


DEFUN(interface_tlv_enable_dot3_tlv, 
		interface_tlv_enable_dot3_tlv_cmd,
		"lldp tlv-enable dot3-tlv(all | mac-physic | power | link-aggregation \
									| max-frame-size)",
		LLDP_STR
		"Enable optional TLV\n"
		"Enable 802.3 TLV\n"
		"Enable 802.3 all TLV\n"
		"Enable 802.3 MAC/PHY configuration TLV\n"	
		"Enable 802.3 power TLV\n"
		"Enable 802.3 link aggregation TLV\n"
		"Enable 802.3 max frame size TLV\n")

{
	unsigned int		if_index = 0;
	struct lldp_port	*port;
	unsigned char		find_port = FALSE;


	if_index = vty->index;
	vty_info_out(vty," Ifindex %08X		%s", if_index, VTY_NEWLINE);	

	/*get the lldp port*/
	if(lldp_global_msg.portsHead != NULL)
	{
		port = lldp_global_msg.portsHead;
		while(port != NULL)
		{
			if(port->if_index == if_index)
			{
				find_port = TRUE;
				break;
			}
			port = port->next;
		}
	}
	else
	{
		vty_error_out(vty, " LLDP No port list.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}	

	if(find_port == TRUE)
	{
		if(port->adminStatus == TRUE)
		{
			/*real config*/			
			if(!strncmp(argv[0],"all",1))
			{			
				port->tlv_cfg.sub_tlv_tx_enable |= (MAC_PHY_CONFIG_STATUS_TX_ENABLE
												| POWER_TX_ENABLE
												| LINK_AGG_TX_ENABLE
												| MAX_FRAME_SIZE_TX_ENABLE);
			}
			else if(!strncmp(argv[0],"mac-physic",3))
			{
				port->tlv_cfg.sub_tlv_tx_enable |= MAC_PHY_CONFIG_STATUS_TX_ENABLE;
			}			
			else if(!strncmp(argv[0],"power",1))
			{
				port->tlv_cfg.sub_tlv_tx_enable |= POWER_TX_ENABLE;
			}
			else if(!strncmp(argv[0],"link-aggregation",1))
			{
				port->tlv_cfg.sub_tlv_tx_enable |= LINK_AGG_TX_ENABLE;
			}
			else if(!strncmp(argv[0],"max-frame-size",3))
			{
				port->tlv_cfg.sub_tlv_tx_enable |= MAX_FRAME_SIZE_TX_ENABLE;
			}
			else
			{				
				vty_error_out(vty, " LLDP error TLV parameter.%s", VTY_NEWLINE);
			}
		}
		else if(port->adminStatus == FALSE)
		{
			vty_error_out(vty," The LLDP is not enabled on this port.%s", VTY_NEWLINE);		
			return CMD_WARNING;
		}
	}
	else
	{
		vty_error_out(vty, " LLDP port list no this port.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}		

	return CMD_SUCCESS;
}


DEFUN(no_interface_tlv_enable_dot3_tlv, 
		no_interface_tlv_enable_dot3_tlv_cmd,
		"no lldp tlv-enable dot3-tlv(all | mac-physic | power | link-aggregation \
									| max-frame-size)",
		NO_STR
		LLDP_STR
		"Enable optional TLV\n"
		"Enable 802.3 TLV\n"
		"Enable 802.3 all TLV\n"
		"Enable 802.3 MAC/PHY configuration TLV\n"	
		"Enable 802.3 power TLV\n"
		"Enable 802.3 link aggregation TLV\n"
		"Enable 802.3 max frame size TLV\n")
{
	unsigned int		if_index = 0;
	struct lldp_port	*port;
	unsigned char		find_port = FALSE;

	if_index = vty->index;
	vty_info_out(vty," Ifindex %08X		%s", if_index, VTY_NEWLINE);	

	/*get the lldp port*/
	if(lldp_global_msg.portsHead != NULL)
	{
		port = lldp_global_msg.portsHead;
		while(port != NULL)
		{
			if(port->if_index == if_index)
			{
				find_port = TRUE;
				break;
			}
			port = port->next;
		}
	}
	else
	{
		vty_error_out(vty, " LLDP No port list.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}	

	if(find_port == TRUE)
	{
		if(port->adminStatus == TRUE)
		{
			/*real config*/			
			if(!strncmp(argv[0],"all",1))
			{			
				port->tlv_cfg.sub_tlv_tx_enable &= ~(MAC_PHY_CONFIG_STATUS_TX_ENABLE
												| POWER_TX_ENABLE
												| LINK_AGG_TX_ENABLE
												| MAX_FRAME_SIZE_TX_ENABLE);
			}
			else if(!strncmp(argv[0],"mac-physic",3))
			{
				port->tlv_cfg.sub_tlv_tx_enable &= ~MAC_PHY_CONFIG_STATUS_TX_ENABLE;
			}			
			else if(!strncmp(argv[0],"power",1))
			{
				port->tlv_cfg.sub_tlv_tx_enable &= ~POWER_TX_ENABLE;
			}
			else if(!strncmp(argv[0],"link-aggregation",1))
			{
				port->tlv_cfg.sub_tlv_tx_enable &= ~LINK_AGG_TX_ENABLE;
			}
			else if(!strncmp(argv[0],"max-frame-size",3))
			{
				port->tlv_cfg.sub_tlv_tx_enable &= ~MAX_FRAME_SIZE_TX_ENABLE;
			}
			else
			{				
				vty_error_out(vty, " LLDP error TLV parameter.%s", VTY_NEWLINE);
			}
		}
		else if(port->adminStatus == FALSE)
		{
			vty_error_out(vty," The LLDP is not enabled on this port.%s", VTY_NEWLINE);		
			return CMD_WARNING;
		}
	}
	else
	{
		vty_error_out(vty, " LLDP port list no this port.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}		

	return CMD_SUCCESS;
}
#endif
		
/************** interface config option TLV end ********************/


static int lldp_global_config_write(struct vty *vty)
{
	/*management addr type is IP*/
	if( 6 != lldp_global_msg.mng_addr_sub)
	{
		vty_out(vty, "lldp management-address ip%s", VTY_NEWLINE);		
	}
	
	if((lldp_global_msg.msgTxHold >= 1 ) && (lldp_global_msg.msgTxHold <= 10))
	{
		if(lldp_global_msg.msgTxHold != 4)
		{
			vty_out(vty, "lldp neighbour hold-period %d%s", lldp_global_msg.msgTxHold, VTY_NEWLINE);
		}		
	}
		
	if((lldp_global_msg.msgTxInterval >= 1) && (lldp_global_msg.msgTxInterval <= 3600))
	{
		if(lldp_global_msg.msgTxInterval != 30)
		{
			vty_out ( vty, "lldp packet interval %d%s",lldp_global_msg.msgTxInterval, VTY_NEWLINE);
		}		
	}	
	
	if(LLDP_TRUE == lldp_global_msg.trapEnable)
	{
		if((lldp_global_msg.trapInterval >= 5) && (lldp_global_msg.trapInterval <= 3600))
		{
			if(lldp_global_msg.trapInterval != 5)
			{
				vty_out ( vty, "lldp trap-interval %d%s",lldp_global_msg.trapInterval, VTY_NEWLINE);
			}		
		}	
	}

	return 0;
}


/**********************
	HT157 cmd, match h3c cmd
*/

/*set / clear packet interval*/ 			 
DEFUN(h3c_lldp_timer_tx_interval, 
		h3c_lldp_timer_tx_interval_cmd,
		"lldp timer tx-interval <5-32768>",
		LLDP_STR
		"LLDP timer\n"
		"The interval at which LLDP frames are transmitted\n"
		"The default value is 30s. Please input an integer from 5 to 32768\n")
{
	uint16_t		pkt_interval = 0;
	struct hash_bucket	*pbucket = NULL;
	int 				cursor = 0;
	struct l2if 		*pif = NULL;
	struct lldp_port	*port = NULL;

	pkt_interval = atoi(argv[0]);
	if((pkt_interval < 5) || (pkt_interval > 32768))
	{ 
		vty_error_out(vty," Wrong parameter range, interval is <5-32768>. %s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	if( lldp_global_msg.msgTxInterval == pkt_interval)
	{
		return CMD_SUCCESS;
	}
	
	lldp_global_msg.msgTxInterval = pkt_interval;

	/*local config change, send lldp packet right now*/
	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif = pbucket->data;
		if(NULL == pif)
		{ 	
			continue;
		}
		
		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(port->adminStatus == LLDP_TRUE)
			{
				port->tx.somethingChangedLocal = LLDP_TRUE;
				port->tx.txTTR = lldp_global_msg.msgTxInterval; 			
				port->tx.txTTL = min(65535, (lldp_global_msg.msgTxInterval * lldp_global_msg.msgTxHold)); 			
				
				lldp_tx_state_machine_action(port);
			}
		}
	}

	return CMD_SUCCESS;
}
	
	
DEFUN(undo_h3c_lldp_timer_tx_interval, 
		undo_h3c_lldp_timer_tx_interval_cmd,
		"undo lldp timer tx-interval",
		NO_STR
		LLDP_STR
		"LLDP timer\n"
		"The interval at which LLDP frames are transmitted\n")
{ 
	struct hash_bucket	*pbucket = NULL;
	int 				cursor = 0;
	struct l2if 		*pif = NULL;
	struct lldp_port	*port = NULL;

	if(30 == lldp_global_msg.msgTxInterval)
	{
		return CMD_SUCCESS;
	}

	lldp_global_msg.msgTxInterval = 30;

	/*local config change, send lldp packet right now*/
	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif = pbucket->data;
		if(NULL == pif)
		{ 	
			continue;
		}
		
		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(LLDP_TRUE == port->adminStatus)
			{
				port->tx.somethingChangedLocal = LLDP_TRUE;

				port->tx.txTTR = lldp_global_msg.msgTxInterval; 			
				port->tx.txTTL = min(65535, (lldp_global_msg.msgTxInterval * lldp_global_msg.msgTxHold)); 			
				lldp_tx_state_machine_action(port);
			}
		}
	}

	return CMD_SUCCESS;
}


/*set / clear hold-period*/ 	   
DEFUN(h3c_lldp_hold_multiplier, 
		h3c_lldp_hold_multiplier_cmd,
        "lldp hold-multiplier <2-10>",
        LLDP_STR
        "A multiplier on the msgTxInterval that determines the actual TTL value use in a LLDPDU\n"
        "The default value is 4. Please input an integer from 2 to 10\n")
{
	unsigned char		hold_multiplier = 0;
	struct hash_bucket	*pbucket = NULL;
	int					cursor = 0;
	struct l2if			*pif = NULL;
	struct lldp_port	*port = NULL;

	hold_multiplier = atoi(argv[0]);

	if((hold_multiplier < 2) || (hold_multiplier > 10))
	{	
		vty_error_out(vty," Wrong parameter range, hold-period is <2-10>. %s", VTY_NEWLINE);
		return CMD_SUCCESS;
	}

	if(lldp_global_msg.msgTxHold == hold_multiplier)
	{		
		return CMD_SUCCESS;
	}

	lldp_global_msg.msgTxHold = hold_multiplier;

	/*if do/not send packet, when change hold-period*/
	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}

		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(LLDP_TRUE == port->adminStatus)
			{
				port->tx.txTTR = lldp_global_msg.msgTxInterval;				
				port->tx.txTTL = min(65535, (lldp_global_msg.msgTxInterval * lldp_global_msg.msgTxHold));				
							
				port->tx.somethingChangedLocal = LLDP_TRUE;
				lldp_tx_state_machine_action(port);
			}
		}
	}

	return CMD_SUCCESS;
}


DEFUN(undo_h3c_lldp_hold_multiplier, 
		undo_h3c_lldp_hold_multiplier_cmd,
        "undo lldp hold-multiplier",
        NO_STR
        LLDP_STR
        "A multiplier on the msgTxInterval that determines the actual TTL value use in a LLDPDU\n")
{
	struct hash_bucket	*pbucket = NULL;
	int 				cursor = 0;
	struct l2if			*pif = NULL;
	struct lldp_port	*port = NULL;

	if( 4 == lldp_global_msg.msgTxHold)
	{
		return CMD_SUCCESS;
	}
	
	lldp_global_msg.msgTxHold = 4;

	/*if do/not send packet, when change hold-period*/
	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif = pbucket->data;
		if(pif == NULL)
		{		
			continue;
		}
		
		port = pif->lldp_port_info;
		if(port != NULL)
		{
			if(port->adminStatus == LLDP_TRUE)
			{
				port->tx.txTTR = lldp_global_msg.msgTxInterval;				
				port->tx.txTTL = min(65535, (lldp_global_msg.msgTxInterval * lldp_global_msg.msgTxHold));				
				
				port->tx.somethingChangedLocal = LLDP_TRUE;
				lldp_tx_state_machine_action(port);
			}
		}
	}

	return CMD_SUCCESS;
}


DEFUN(undo_h3c_interface_lldp_enable, 
		undo_h3c_interface_lldp_enable_cmd,
        "undo lldp enable",
        NO_STR
        LLDP_STR
        "Enable LLDP protocol\n")
{
	unsigned int		if_index = 0;
	struct lldp_port	*port = NULL;
	struct l2if			*pif = NULL;

	if_index = (unsigned int)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		return CMD_WARNING;
	}

	/*judge the port mode*/
	if (IFNET_MODE_INVALID == pif->mode || IFNET_MODE_PHYSICAL == pif->mode)
	{
		vty_info_out(vty," The port mode is invalid, LLDP can not config.%s", VTY_NEWLINE);
		return CMD_WARNING;	
	}

	port = pif->lldp_port_info;
	if(port != NULL)
	{
		if(LLDP_TRUE == port->adminStatus)
		{
			port->adminStatus = LLDP_FALSE;

			/*need to clean lldp msg*/
//			lldp_rx_state_machine_action(port);
			lldp_tx_state_machine_action(port);

			vty_info_out(vty," The LLDP is disable on the port successfully.%s", VTY_NEWLINE);			
		}
		else if(port->adminStatus == LLDP_FALSE)
		{
			zlog_err("%s: Error: LLDP on the port is already disabled, and the port should not exist!\n", __func__);

			vty_error_out(vty," LLDP on the port is already disabled, and the port should not exist! %s", VTY_NEWLINE);

			return CMD_WARNING;
		}
	}
	else
	{
		vty_error_out(vty, " No the interface lldp info, maybe the interface no enabled lldp.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}	

	return CMD_SUCCESS;		
}


DEFUN(h3c_interface_management_tlv, 
		h3c_interface_management_tlv_cmd,
		"lldp tlv-enable basic-tlv management-address-tlv",
		LLDP_STR
		"Enable optional TLV\n"
		"LLDP basic TLV configuration\n"
		"Enable basic management address TLV\n")
{
	unsigned int		if_index = 0;
	struct lldp_port	*port = NULL;
	struct l2if			*pif = NULL;

	if_index = (unsigned int)vty->index;
	LLDP_LOG_DBG("%s:Info: ifindex %08X	...\n", __func__, if_index);

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," get interface null! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	port = pif->lldp_port_info;
	if(port != NULL)
	{
		if(LLDP_TRUE == port->adminStatus)
		{
			/*real config*/ 		
			//port->tlv_cfg.basic_tlv_tx_enable = 0x00;
			port->tlv_cfg.basic_tlv_tx_enable |= MANAGEMENT_ADDRESS_TLV_TX_ENABLE;
		}
		else if(port->adminStatus == LLDP_FALSE)
		{
			zlog_err("%s: Error: LLDP on the port is already disabled, and the port should not exist!\n", __func__);
			vty_error_out(vty," LLDP on the port is already disabled, and the port should not exist!%s", VTY_NEWLINE);

			return CMD_WARNING;
		}
	}
	else
	{
		vty_error_out(vty, " No the interface lldp info, maybe the interface no enabled lldp%s", VTY_NEWLINE);
		return CMD_WARNING;
	}	

	return CMD_SUCCESS;			
}


DEFUN(undo_h3c_interface_basic_tlv, 
		undo_h3c_interface_basic_tlv_cmd,
		"undo lldp tlv-enable basic-tlv (all | port-description | \
									system-name | system-description \
									| system-capability | management-address-tlv)",
		NO_STR
		LLDP_STR
		"Enable optional TLV\n"
		"LLDP basic TLV configuration\n"
		"Enable basic all TLV\n"	
		"Enable basic port description TLV\n"
		"Enable basic system name TLV\n"
		"Enable basic system description TLV\n"
		"Enable basic system capability TLV\n"
		"Enable basic management address TLV\n")
{
	unsigned int		if_index = 0;
	struct lldp_port	*port = NULL;
	struct l2if			*pif = NULL;

	if_index = (unsigned int)vty->index;
	LLDP_LOG_DBG("%s:Info: ifindex %08X	...\n", __func__, if_index);

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);		
		vty_error_out(vty,"  get interface null. %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	port = pif->lldp_port_info;
	if(port != NULL)
	{
		if(LLDP_TRUE == port->adminStatus)
		{
			/*real config*/ 		
			if(!strncmp(argv[0], "all", 1))
			{
				port->tlv_cfg.basic_tlv_tx_enable = 0x00;
			}
			else if(!strncmp(argv[0], "port-description", 1))
			{
				port->tlv_cfg.basic_tlv_tx_enable &= ~PORT_DESCRIPTION_TLV_TX_ENABLE;
			}			
			else if(!strncmp(argv[0], "system-name", 8))
			{
				port->tlv_cfg.basic_tlv_tx_enable &= ~SYSTEM_NAME_TLV_TX_ENABLE;
			}
			else if(!strncmp(argv[0], "system-description", 8))
			{
				port->tlv_cfg.basic_tlv_tx_enable &= ~SYSTEM_DESCRIPTION_TLV_TX_ENABLE;
			}
			else if(!strncmp(argv[0], "system-capability", 8))
			{
				port->tlv_cfg.basic_tlv_tx_enable &= ~SYSTEM_CAPABILITIES_TLV_TX_ENABLE;
			}
			else if(!strncmp(argv[0], "management-address-tlv", 1))
			{
				port->tlv_cfg.basic_tlv_tx_enable &= ~MANAGEMENT_ADDRESS_TLV_TX_ENABLE;
			}
			else
			{				
				vty_error_out(vty, " LLDP TLV parameter.%s", VTY_NEWLINE);
			}
		}
		else if(LLDP_FALSE == port->adminStatus)
		{
			zlog_err("%s: Error: LLDP on the port is already disabled, and the port should not exist!\n", __func__);
			vty_error_out(vty," LLDP on the port is already disabled, and the port should not exist!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	else
	{
		vty_error_out(vty, " No the interface lldp info, maybe the interface no enabled lldp.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}		

	return CMD_SUCCESS;
}


DEFUN(h3c_lldp_global_enable, 
		h3c_lldp_global_enable_cmd,
		"lldp global enable",
		LLDP_STR
		"Global configuration\n"
		"Enable LLDP protocol\n")
{
	struct lldp_port	*port = NULL;
	uint8_t status;
	struct hash_bucket	*pbucket = NULL;
	int cursor = 0;
	struct l2if *pif = NULL;
	int ret = 0;

	if( lldp_global_msg.source_mac[0] == 0x00
		&& lldp_global_msg.source_mac[1] == 0x00
		&& lldp_global_msg.source_mac[2] == 0x00
		&& lldp_global_msg.source_mac[3] == 0x00
		&& lldp_global_msg.source_mac[4] == 0x00
		&& lldp_global_msg.source_mac[5] == 0x00)
	{
		zlog_err("%s: ERROR: sys MAC addr is NULL!\n", __func__);
		vty_error_out(vty," SYS MAC addr is NULL, can not enable lldp.%s", VTY_NEWLINE);
	
		return CMD_WARNING;
	}

	lldp_global_msg.global_enable = LLDP_TRUE;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;

		if (NULL == pif)
		{
			continue;
		}

		/*judge the port mode*/
		if (IFNET_MODE_INVALID == pif->mode || IFNET_MODE_PHYSICAL == pif->mode)
		{
			continue;	
		}

		//only support physical port
		if (!IFM_TYPE_IS_PHYSICAL(pif->ifindex))
		{
			continue;
		}

		port = pif->lldp_port_info;
		if(port != NULL)
		{
			continue;
		}
		else
		{
			pif->lldp_port_info = (struct lldp_port*)XMALLOC(MTYPE_LLDP, sizeof(struct lldp_port));
			if(NULL == pif->lldp_port_info)
			{		
				continue;
			}
			
			memset(pif->lldp_port_info, 0, sizeof(struct lldp_port));

			/*get interface info*/
			ret = ifm_get_link(pif->ifindex, MODULE_ID_L2, &status);  
			if(!ret)
			{
				if(IFNET_LINKUP == status)
				{
					pif->lldp_port_info->portEnabled = LLDP_TRUE;
				}
				else
				{
					pif->lldp_port_info->portEnabled = LLDP_FALSE;				
				}			
			}
			else
			{		
				pif->lldp_port_info->portEnabled = LLDP_FALSE;
			}

			pif->lldp_port_info->adminStatus = LLDP_TRUE;
	
			ifm_get_name_by_ifindex(pif->ifindex ,pif->lldp_port_info->if_name);
			pif->lldp_port_info->if_index = pif->ifindex;

			memcpy(pif->lldp_port_info->if_mac, pif->mac, 6);
			
			/*get sys Capabilities*/
			pif->lldp_port_info->sys_cap = 0x0014; /*bit3:MAC Bridge &bit5:Router*/
			pif->lldp_port_info->sys_cap_enable = 0x0014;
		
			pif->lldp_port_info->rxChanges = LLDP_FALSE;
		
			/*enable all tlv*/
			pif->lldp_port_info->tlv_cfg.basic_tlv_tx_enable = (PORT_DESCRIPTION_TLV_TX_ENABLE
												| SYSTEM_NAME_TLV_TX_ENABLE
												| SYSTEM_DESCRIPTION_TLV_TX_ENABLE
												| SYSTEM_CAPABILITIES_TLV_TX_ENABLE
												| MANAGEMENT_ADDRESS_TLV_TX_ENABLE);
		
			pif->lldp_port_info->max_frame_size = 9600;
	
			lldp_init_port_rx(pif->lldp_port_info);
			lldp_init_port_tx(pif->lldp_port_info);
		
			pif->lldp_port_info->msap_num = 0;
			pif->lldp_port_info->msap_cache = NULL;

			lldp_tx_state_machine_action(pif->lldp_port_info);
			
			if(TX_IDLE == pif->lldp_port_info->tx.state)
			{
				/*send frame*/
				lldp_send_frame(pif->lldp_port_info);
			}
			
			lldp_rx_state_machine_action(pif->lldp_port_info);
			
		}
		
	}

	return CMD_SUCCESS;		
}


DEFUN(undo_h3c_lldp_global_enable, 
		undo_h3c_lldp_global_enable_cmd,
        "undo lldp global enable",
        NO_STR
        LLDP_STR
        "Global configuration\n"
        "Enable LLDP protocol\n")
{
	struct lldp_port	*port = NULL;
	struct l2if			*pif = NULL;
	struct hash_bucket	*pbucket = NULL;
	int cursor = 0;

	lldp_global_msg.global_enable = LLDP_FALSE;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;

		if (NULL == pif)
		{
			continue;
		}

		/*judge the port mode*/
		if (IFNET_MODE_INVALID == pif->mode || IFNET_MODE_PHYSICAL == pif->mode)
		{
			continue;	
		}
		
		port = pif->lldp_port_info;
		if (NULL == port)
		{
			continue;
		}

		port->adminStatus = LLDP_FALSE;

		/*need to clean lldp msg*/
//	lldp_rx_state_machine_action(port);
		lldp_tx_state_machine_action(port);		
	}

	return CMD_SUCCESS;		
}


DEFUN(lldp_management_address_arp_learning, 
		lldp_management_address_arp_learning_cmd,
        "lldp management-address arp-learning {vlan <1-4094>}",
        LLDP_STR
        "Management address for Network managment\n"
        "LLDP arp learning\n"
        "VLAN\n"
        "VLAN value, 1-4094\n")
{
	uint32_t		if_index = 0;
	struct lldp_port	*port = NULL;
	struct l2if			*pif = NULL;
	struct lldp_msap* 	msap_cache = NULL;
	uint16_t		vlan = 0;

	if(argv[0] != NULL)
	{
		vlan = atoi(argv[0]);
		if((vlan < 1) || (vlan > 4094))
		{	
			vty_error_out(vty," Invalid vlan value, support 1-4094. %s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}

	if (lldp_global_msg.mng_addr_sub != MGMT_ADDR_SUB_IPV4)
	{
		vty_error_out(vty," LLDP management addr type is not IPV4! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," get interface null! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	#if 0
	if (pif->mode != IFNET_MODE_L3)
	{
		vty_error_out(vty," Invalid port mode, only support L3 mode! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	#endif

	port = pif->lldp_port_info;
	if(NULL == port)
	{
		vty_error_out(vty, " No the interface lldp info, maybe the interface no enabled lldp%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (LLDP_FALSE == port->adminStatus)
	{
		zlog_err("%s: Error: LLDP on the port is already disabled, and the port should not exist!\n", __func__);
		vty_error_out(vty," LLDP on the port is already disabled, and the port should not exist!%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	if(0 == port->msap_num)
	{ 	
		/*real config*/ 		
		SET_FLAG(port->arp_nd_learning, LLDP_LEARNING_ARP);
		port->arp_nd_learning_vlan = vlan;
		return CMD_SUCCESS;
	}

	msap_cache = port->msap_cache;

	while(msap_cache != NULL)
	{
		if (CHECK_FLAG(port->arp_nd_learning, LLDP_LEARNING_ARP) &&
			(port->arp_nd_learning_vlan != vlan) &&
			(LLDP_TRUE == msap_cache->arp_notice_flag))
		{
			lldp_arp_info_notice(msap_cache, port->arp_nd_learning_vlan, 
													port->if_index, LLDP_FALSE);
			msap_cache->arp_notice_flag = LLDP_FALSE;
		}
		
		msap_cache = msap_cache->next;
	}

	/*real config*/ 		
	SET_FLAG(port->arp_nd_learning, LLDP_LEARNING_ARP);
	port->arp_nd_learning_vlan = vlan;

	msap_cache = port->msap_cache;

	while(msap_cache != NULL)
	{
		if ((LLDP_FALSE == msap_cache->arp_notice_flag) &&
			/*(port->arp_nd_learning_vlan == msap_cache->svlan) &&*/
			(msap_cache->neighbor_msg.mgmt_addr_sub_type == 1))
		{
			lldp_arp_info_notice(msap_cache, port->arp_nd_learning_vlan, 
														port->if_index, LLDP_TRUE);	
			msap_cache->arp_notice_flag = LLDP_TRUE;
		}
		
		msap_cache = msap_cache->next;
	}

	return CMD_SUCCESS;
}


DEFUN(undo_h3c_lldp_management_address_arp_learning, 
		undo_h3c_lldp_management_address_arp_learning_cmd,
        "undo lldp management-address arp-learning",
        NO_STR
        LLDP_STR
        "Management address for Network managment\n"
        "LLDP arp learning\n")
{
	uint32_t		if_index = 0;
	struct lldp_port	*port = NULL;
	struct l2if			*pif = NULL;
	struct lldp_msap* 	msap_cache = NULL;

	if (lldp_global_msg.mng_addr_sub != MGMT_ADDR_SUB_IPV4)
	{
		vty_error_out(vty," LLDP management addr type is not IPV4! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," get interface null! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	#if 0
	if (pif->mode != IFNET_MODE_L3)
	{
		vty_error_out(vty," Invalid port mode, only support L3 mode! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	#endif

	port = pif->lldp_port_info;
	if(NULL == port)
	{
		vty_error_out(vty, " No the interface lldp info, maybe the interface no enabled lldp%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (LLDP_FALSE == port->adminStatus)
	{
		zlog_err("%s: Error: LLDP on the port is already disabled, and the port should not exist!\n", __func__);
		vty_error_out(vty," LLDP on the port is already disabled, and the port should not exist!%s", VTY_NEWLINE);

		return CMD_WARNING;
	}
	
	if (!CHECK_FLAG(port->arp_nd_learning, LLDP_LEARNING_ARP))
	{
		port->arp_nd_learning_vlan = 0;
		return CMD_SUCCESS;
	}

	msap_cache = port->msap_cache;
	
	if(0 == port->msap_num)
	{ 	
		/*real config*/ 		
		UNSET_FLAG(port->arp_nd_learning, LLDP_LEARNING_ARP);
		port->arp_nd_learning_vlan = 0;
		return CMD_SUCCESS;
	}

	while(msap_cache != NULL)
	{
		if (LLDP_TRUE == msap_cache->arp_notice_flag)
		{
			lldp_arp_info_notice(msap_cache, port->arp_nd_learning_vlan, port->if_index, LLDP_FALSE);
			msap_cache->arp_notice_flag = LLDP_FALSE;
		}
		
		msap_cache = msap_cache->next;
	}

	/*real config*/ 		
	UNSET_FLAG(port->arp_nd_learning, LLDP_LEARNING_ARP);
	port->arp_nd_learning_vlan = 0;

	return CMD_SUCCESS;
}

DEFUN(no_lldp_management_address_arp_learning, 
		no_lldp_management_address_arp_learning_cmd,
        "no lldp management-address arp-learning",
        NO_STR
        LLDP_STR
        "Management address for Network managment\n"
        "LLDP arp learning\n")
{
	uint32_t		if_index = 0;
	struct lldp_port	*port = NULL;
	struct l2if			*pif = NULL;
	struct lldp_msap* 	msap_cache = NULL;
	
	if (lldp_global_msg.mng_addr_sub != MGMT_ADDR_SUB_IPV4)
	{
		vty_error_out(vty," LLDP management addr type is not IPV4! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if_index = (uint32_t)vty->index;

	pif = l2if_get(if_index);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," get interface null! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	#if 0
	if (pif->mode != IFNET_MODE_L3)
	{
		vty_error_out(vty," Invalid port mode, only support L3 mode! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	#endif
	
	port = pif->lldp_port_info;
	if(NULL == port)
	{
		vty_error_out(vty, " No the interface lldp info, maybe the interface no enabled lldp%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (LLDP_FALSE == port->adminStatus)
	{
		zlog_err("%s: Error: LLDP on the port is already disabled, and the port should not exist!\n", __func__);
		vty_error_out(vty," LLDP on the port is already disabled, and the port should not exist!%s", VTY_NEWLINE);

		return CMD_WARNING;
	}

	if (!CHECK_FLAG(port->arp_nd_learning, LLDP_LEARNING_ARP))
	{
		port->arp_nd_learning_vlan = 0;
		return CMD_SUCCESS;
	}

	msap_cache = port->msap_cache;
	
	if(0 == port->msap_num)
	{ 	
		/*real config*/ 		
		UNSET_FLAG(port->arp_nd_learning, LLDP_LEARNING_ARP);
		port->arp_nd_learning_vlan = 0;
		return CMD_SUCCESS;
	}

	while(msap_cache != NULL)
	{
		if (LLDP_TRUE == msap_cache->arp_notice_flag)
		{
			lldp_arp_info_notice(msap_cache, port->arp_nd_learning_vlan, port->if_index, LLDP_FALSE);
			msap_cache->arp_notice_flag = LLDP_FALSE;
		}
		
		msap_cache = msap_cache->next;
	}

	/*real config*/ 		
	UNSET_FLAG(port->arp_nd_learning, LLDP_LEARNING_ARP);
	port->arp_nd_learning_vlan = 0;

	return CMD_SUCCESS;
}


DEFUN ( if_alias,
        if_alias_cmd,
        "alias STRING",
        "Set an interface alias\n"
        "The alias of Interface(no more than 256 characters)\n" )
{
    uint32_t ifindex = 0;
    const char *alias = argv[0];
	struct l2if			*pif = NULL;

    ifindex = ( uint32_t ) vty->index;
	
    if ( strlen ( alias ) > IFM_ALIAS_STRING_LEN )
    {
		vty_error_out(vty, "String out of range%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

	pif = l2if_get(ifindex);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," get interface null! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memcpy (pif->alias, alias, strlen ( alias ) + 1 );

    return CMD_SUCCESS;
}

DEFUN ( no_if_alias,
        no_if_alias_cmd,
        "no alias",
        NO_STR
        "Set an interface alias\n" )
{
    uint32_t ifindex = 0;
    //uchar alias[NAME_STRING_LEN] = {'\0'};
	struct l2if 		*pif = NULL;

    ifindex = ( uint32_t ) vty->index;

	pif = l2if_get(ifindex);
	if(NULL == pif)
	{
		zlog_err("%s[%d]:%s pif is NULL.\n",__FILE__,__LINE__,__func__);
		vty_error_out(vty," get interface null! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memset (pif->alias, 0, IFM_ALIAS_STRING_LEN+1);

    return CMD_SUCCESS;
}


void lldp_vty_init (void)
{
	install_node(&lldp_node, lldp_global_config_write);

	install_default (LLDP_NODE);
	install_element (CONFIG_NODE, &lldp_mode_cmd, CMD_SYNC);	    

/*whole config*/
	install_element (CONFIG_NODE, &lldp_management_address_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &lldp_packet_interval_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_lldp_packet_interval_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &lldp_neighbour_hold_period_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_lldp_neighbour_hold_period_cmd, CMD_SYNC);

	/*show information*/	
	install_element (CONFIG_NODE, &show_lldp_statistics_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_lldp_local_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_lldp_neighbor_cmd, CMD_LOCAL);
	install_element (CONFIG_NODE, &show_lldp_tlv_config_cmd, CMD_LOCAL);

/*ethernet if node*/
	install_element (PHYSICAL_IF_NODE, &show_lldp_statistics_cmd, CMD_LOCAL);
	install_element (PHYSICAL_IF_NODE, &show_lldp_local_cmd, CMD_LOCAL);
	install_element (PHYSICAL_IF_NODE, &show_lldp_neighbor_cmd, CMD_LOCAL);
	install_element (PHYSICAL_IF_NODE, &show_lldp_tlv_config_cmd, CMD_LOCAL);

	/*interface config*/
	install_element(PHYSICAL_IF_NODE, &interface_lldp_enable_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_interface_lldp_enable_cmd, CMD_SYNC);
	
	/*interface option TLV config*/
	install_element(PHYSICAL_IF_NODE, &interface_tlv_enable_basic_tlv_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_interface_tlv_enable_basic_tlv_cmd, CMD_SYNC);

/*trunk if node*/
	install_element (TRUNK_IF_NODE, &show_lldp_statistics_cmd, CMD_LOCAL);
	install_element (TRUNK_IF_NODE, &show_lldp_local_cmd, CMD_LOCAL);
	install_element (TRUNK_IF_NODE, &show_lldp_neighbor_cmd, CMD_LOCAL);
	install_element (TRUNK_IF_NODE, &show_lldp_tlv_config_cmd, CMD_LOCAL);

	/*interface config*/
	install_element(TRUNK_IF_NODE, &interface_lldp_enable_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &no_interface_lldp_enable_cmd, CMD_SYNC);

	/*interface option TLV config*/
	install_element(TRUNK_IF_NODE, &interface_tlv_enable_basic_tlv_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &no_interface_tlv_enable_basic_tlv_cmd, CMD_SYNC);

	/*HT157 cmd, match h3c cmd*/
	install_element (CONFIG_NODE, &h3c_lldp_timer_tx_interval_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &undo_h3c_lldp_timer_tx_interval_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &h3c_lldp_hold_multiplier_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &undo_h3c_lldp_hold_multiplier_cmd, CMD_SYNC);

	install_element(PHYSICAL_IF_NODE, &undo_h3c_interface_lldp_enable_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &undo_h3c_interface_lldp_enable_cmd, CMD_SYNC);

	install_element(PHYSICAL_IF_NODE, &h3c_interface_management_tlv_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &undo_h3c_interface_basic_tlv_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &h3c_interface_management_tlv_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &undo_h3c_interface_basic_tlv_cmd, CMD_SYNC);

	install_element (CONFIG_NODE, &h3c_lldp_global_enable_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &undo_h3c_lldp_global_enable_cmd, CMD_SYNC);

	install_element(PHYSICAL_IF_NODE, &lldp_management_address_arp_learning_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &undo_h3c_lldp_management_address_arp_learning_cmd, CMD_SYNC);
	install_element(PHYSICAL_IF_NODE, &no_lldp_management_address_arp_learning_cmd, CMD_SYNC);

	install_element(TRUNK_IF_NODE, &lldp_management_address_arp_learning_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &undo_h3c_lldp_management_address_arp_learning_cmd, CMD_SYNC);
	install_element(TRUNK_IF_NODE, &no_lldp_management_address_arp_learning_cmd, CMD_SYNC);

	install_element ( PHYSICAL_IF_NODE, &if_alias_cmd, CMD_SYNC );
	install_element ( PHYSICAL_IF_NODE, &no_if_alias_cmd, CMD_SYNC );
	
	install_element ( TRUNK_IF_NODE, &if_alias_cmd, CMD_SYNC );
	install_element ( TRUNK_IF_NODE, &no_if_alias_cmd, CMD_SYNC );

	return;
}


