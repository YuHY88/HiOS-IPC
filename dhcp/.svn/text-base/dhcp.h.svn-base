/* Copyright (C) 2013-2014  HUAHUAN, Inc. All Rights Reserved. */

#ifndef _ZEBOS_DHCP_H
#define _ZEBOS_DHCP_H

//#include "nsm_message.h"
//#include "sdm_messages.h"
#ifdef HAVE_EMD
#include "em_messages.h"
#endif

#include "../lib/vector.h"
#include <lib/msg_ipc_n.h>

#define bool_t char //aaaaaaaaaaaaaa
////////////////////////////////////////////////////////////////////////////////////////////////////
#define DHCP_PORT_CPU                            0


/////////////////////////////////////////////defualt config///////////////////////////////////////////
#define DHCP_CONF_DEF_DHCP                 0
#define DHCP_CONF_DEF_INBAND               0
#define DHCP_CONF_DEF_VID          		   1
#if defined(HH_CARD_1604C)
#define DHCP_CONF_DEF_INBANDVID            3999
#elif defined(HH_CARD_1610) 
#define DHCP_CONF_DEF_INBANDVID            1
#else
#define DHCP_CONF_DEF_INBANDVID            1
#endif
#define DHCP_CONF_DEF_INBANDPRI            7
#define DHCP_CONF_DEF_INBAND_IPV4_ADDR     0xc0a80201
#define DHCP_CONF_DEF_INBAND_IPV4_MASK     0xffffff00
#define DHCP_CONF_DEF_INBAND_IPV6_ADDR     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define DHCP_CONF_DEF_INBAND_IPV6_MASK     255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0
//////////////////////////////////////////////////////////////////////////////////////////////////

#define DHCP_IF_LINK_DOWN						  0	
#define DHCP_IF_LINK_UP							  1		
#define DHCP_STATUS_ENABLE              	      1
#define DHCP_STATUS_DISABLE             		  0
#define DHCP_IPV6ADDR_MLEN   					  16
#define DHCP_LINE_TYPE_MAX						  100

#define DHCP_MAX_VLAN                             4094/*modify by wangqian 2015/09/24, 4000-4094 is reserved*/
//#define DHCP_MAX_VLAN                             3999
#define DHCP_MIN_VLAN                             1
#define DHCP_PORT_ALL                             -1
#define DHCP_UNIT_DEFAULT                         0
#define DHCP_VLAN_DEFAULT                         1
#define DHCP_PRI_DEFAULT                          7
#define DHCP_VLAN_UNTAG							  0
#define DHCP_VLAN_TAG							  1
#define DHCP_PROBE_MAX							  4


#define DHCP_VLAN_FLAG							  1

#define DHCP_ADD_VLAN_NUM_NONE						  0
#define DHCP_EXCEPT_VLAN_NONE					  -1
#define DHCP_PROBE_TIMES_MAX			 		  4
#define DHCP_IS_INBAND							  1
#define DHCP_TRAPPORT_DEFAULT					  162

#define DHCP_MAN_INBANDIF_NAME					  "eth2:0"

///////////////////////////////////////////////////////////////////////////////

typedef struct _dhcp_conf
{
    int            DhcpEnable;
  
    int            InBandVlanID;
    int            InBandPriority;
    
    unsigned long  InBandAddrIpV4;    
    unsigned long  InBandMaskIpV4;    
    unsigned long  InBandDefGwIpV4;    

    
    unsigned char  InBandAddrIpV6[DHCP_IPV6ADDR_MLEN];    
    unsigned char  InBandMaskIpV6[DHCP_IPV6ADDR_MLEN];    
    unsigned char  InBandDefGwIpV6[DHCP_IPV6ADDR_MLEN];    

    int ne_discover_ack;

    /*global parm*/
    int             MgmtVidFlag;
    int             DhcpWaitState;
    int             DhcpRestartProbe;
	int 			if_updown_state;
	
}dhcp_conf;

typedef struct _dhcp_ginfo
{
	char TrapInfoStr[64];
	//struct nsm_msg_mgmvlan nsm_msg;  //aaaaaaaaaaaaaaaaa
#ifdef HAVE_SDMD		
	struct sdm_msg_dhcp sdm_msg;
#endif
//added by lidingcheng for support emd 20150105
#ifdef HAVE_EMD	
	struct em_msg_dhcp emd_msg;
#endif
//lidingcheng added end 20150105
	
}dhcp_ginfo;

typedef struct _dhcp_globals
{


    struct thread      *basetimer;

    //struct dhcp_server  *server;

    char               *desc;

  /* Active PMs for this VR.  */
    //modbmap_t           module;

  /* Configured PMs for this VR.  */
   // modbmap_t           module_config;

  /* Filename for configuration. */
    dhcp_conf		 *conf;
    
 	dhcp_ginfo		  *ginfo;

	//pthread_t dhcpthread;

  /* Vector for IMI line connection.  */
  vector lines[DHCP_LINE_TYPE_MAX];

  /* Set after NMS is caming with the VR add msg. */
  bool_t im_nsm_add_flag;
}dhcp_globals;

extern dhcp_globals dhcpcm;



#endif /* _ZEBOS_DHCP_H */
