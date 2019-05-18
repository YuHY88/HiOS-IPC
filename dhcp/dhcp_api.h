/*   Copyright (C) 2014 huahuan, Inc. All Rights Reserved.  */

#ifndef __ZEBOS_DHCP_API_H__
#define __ZEBOS_DHCP_API_H__

//#include "dhcp.h"
/*
   DHCP
  
   This module declares the interface to DHCP
  
 */
typedef struct _inband_config_param_
{
	unsigned int   ipaddr;
	unsigned short vlanid;
	unsigned char priority;
	unsigned char mac[6]; 
}st_inband_config_param;

int dhcp_enable_get(void);
int dhcp_enable_set(int iEnable);
int dhcp_discover_get(void);
int dhcp_discover_set(int ack);
//int dhcp_conf_whole_get(dhcp_conf *pSysConf);
//int dhcp_conf_default_init(dhcp_conf *pConf);
void dhcp_netif_iplongtostr(char *pIpStr, int iBufLen, const unsigned long ulIP);
unsigned long dhcp_netif_ipstrtolong(const char *pIpStr);

#if defined(HH_CARD_1610)
void dhcp_all_vlan_add(void);
void dhcp_all_vlan_remove (int expvlan);
void dhcp_port_defvlan_set(int tag_flag);
int dhcp_add_vlan_num_get(void);
#endif
#if defined(HH_CARD_1610) || defined(HH_CARD_1604C) 
void dhcp_mgmt_vidflag_recover(void);
#endif

#endif

