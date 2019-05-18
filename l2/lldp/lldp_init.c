/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_init.c		(lldp init)
*
*  date: 2016.7.29
*
*  modify:	2018.3.12 modified by liufuying to make LLDP module code beautiful
*
*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memtypes.h"
#include "lib/memory.h"
#include "devm_com.h"
#include "module_id.h"
#include "command.h"
#include "devm_com.h"
#include "lib/log.h"
#include "lib/ospf_common.h"


#include "lldp_tx_sm.h"
#include "lldp_rx_sm.h"
#include "lldp_init.h"
#include "lldp_tlv.h"

lldp_global 			lldp_global_msg;
extern unsigned char	l2_mac[6];



void lldp_g_cfg_init(void)
{
	int ret = 0;
	unsigned char 	Company_name[32] = {0};
	unsigned char	cmp_name[32];	
	unsigned char	version[64];

	/* whole config init */
	lldp_global_msg.msgTxInterval	= 30;
	lldp_global_msg.msgTxHold		= 4;
	lldp_global_msg.trapEnable		= LLDP_FALSE;
	lldp_global_msg.trapInterval	= 5;
	lldp_global_msg.timer			= 1;	/*default 1s*/

//	lldp_global_msg.mng_addr_sub	= MGMT_ADDR_SUB_ALL802;	/*default is mac*/   /*change default type to ipv4*/
         lldp_global_msg.mng_addr_sub	= MGMT_ADDR_SUB_IPV4 ;

	memset(lldp_global_msg.source_mac, 0, MAC_LEN);
	memset(lldp_global_msg.source_ipaddr, 0, IPV4_LEN);

         lldp_global_msg.arp_learn_enable = OSPF_DCN_ENABLE ;
         lldp_global_msg.dcn_vlan = OSPF_DCN_USE_VLAN ;
	
	/*get system name*/
	memset(lldp_global_msg.system_name, 0, SYSTEM_NAMSIZ);
	
	ret = devm_comm_get_dev_name(1, 0, MODULE_ID_L2,lldp_global_msg.system_name);
	if(ret)
	{
		zlog_err("%s: get dev name error!\n", __func__);
		sprintf((char *)&lldp_global_msg.system_name, "%s", "DEV NAME UNKNOWN");
	}
		
	/*get systemdesc*/
	memset(lldp_global_msg.system_desc, 0, MAX_SIZE);
	ret = devm_comm_get_manufacture(1, MODULE_ID_L2,Company_name);
	if(ret)
	{
		zlog_err("%s: get Company name error!\n", __func__);
		
		sprintf((char *)&cmp_name, "%s", "COMPANY NAME UNKNOWN");
	}
	else
	{	
		memset(cmp_name, 0, 32);
		memcpy(cmp_name, Company_name, strlen((const char *)&Company_name));
	}

	memset(version, 0, 64);
	devm_comm_get_sys_version(version);
	
	sprintf((char *)&lldp_global_msg.system_desc,"%s %s, %s %s",
								cmp_name,
								lldp_global_msg.system_name,
								"HuaHuan Versatile Routing Platform, Software version:",
								version);

	/*get management MAC addr*/
	memcpy((char *)&lldp_global_msg.source_mac, &l2_mac[0], MAC_LEN);

         lldp_get_management_ip(NULL);   /*get default ip*/

       
    return;
}

void lldp_init_port_rx(struct lldp_port *lldp_port)
{
	lldp_port->rx.frame = XMALLOC(MTYPE_LLDP_RX, lldp_port->max_frame_size);
	memset(lldp_port->rx.frame, 0, lldp_port->max_frame_size);
	lldp_port->rx.recvsize = 0;
	lldp_port->rx.state = LLDP_WAIT_PORT_OPERATIONAL;
	lldp_port->rx.badFrame = 0;
	lldp_port->rx.rcvFrame = LLDP_FALSE;
	lldp_port->rx.rxInfoAge = 0;
	lldp_port->rx.somethingChangedRemote = LLDP_FALSE;
	lldp_port->rx.timers.rxTTL = 0;
	memset(&lldp_port->rx.statistics, 0 , sizeof(struct lldp_rx_port_statistics));

	return;
}

void lldp_init_port_tx(struct lldp_port *lldp_port)
{
	lldp_port->tx.frame = XMALLOC(MTYPE_LLDP_TX, lldp_port->max_frame_size);
	memset(lldp_port->tx.frame, 0, lldp_port->max_frame_size);
	lldp_port->tx.sendsize = 0;
	lldp_port->tx.state = TX_LLDP_INITIALIZE;

	lldp_port->tx.somethingChangedLocal = LLDP_FALSE;
	lldp_port->tx.txTTL = min(65535, (lldp_global_msg.msgTxInterval * lldp_global_msg.msgTxHold));
	lldp_port->tx.txTTR = lldp_global_msg.msgTxInterval;
	memset(&lldp_port->tx.statistics, 0, sizeof(struct lldp_tx_port_statistics));
	
	return;
}

