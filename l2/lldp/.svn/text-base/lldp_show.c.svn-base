/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_show.c		(lldp msg show)
*
*  date: 2016.8.4
*
*  modify:	2018.3.12 modified by liufuying to make LLDP module code beautiful
*
*/


#include <string.h>
#include <stdlib.h>

#include "lib/vty.h"
#include "lib/types.h"
#include "command.h"
#include "l2/l2_if.h"
#include "lib/hash1.h"

#include "lldp_show.h"

extern lldp_global lldp_global_msg;
extern struct hash_table l2if_table;


#if 0
unsigned char stp_proto_id[LLDP_802_3_PROTO_ID_LEN] = {0x00, 0x00, 0x42, 0x42,
                                                   0x03, 0x00, 0x00, 0x00};
unsigned char rstp_proto_id[LLDP_802_3_PROTO_ID_LEN] = {0x00, 0x00, 0x42, 0x42,
                                                    0x03, 0x00, 0x00, 0x02};
unsigned char mstp_proto_id[LLDP_802_3_PROTO_ID_LEN] = {0x00, 0x00, 0x42, 0x42,
                                                    0x03, 0x00, 0x00, 0x03};
unsigned char gmrp_proto_id[LLDP_802_3_PROTO_ID_LEN] = {0x00, 0x00, 0x42, 0x42,
                                                    0x03, 0x00, 0x01, 0x02};
unsigned char mmrp_proto_id[LLDP_MRP_PROTO_ID_LEN] = {0x81, 0x10};

unsigned char gvrp_proto_id[LLDP_802_3_PROTO_ID_LEN] = {0x00, 0x00, 0x42, 0x42,
                                                    0x03, 0x00, 0x01, 0x01};
unsigned char mvrp_proto_id[LLDP_MRP_PROTO_ID_LEN] = {0x81, 0x20};

unsigned char dot1x_proto_id[LLDP_DOT1X_PROTO_ID_LEN] = {0x88, 0x8e};

unsigned char lacp_proto_id[LLDP_SLOW_PROTO_ID_LEN] = {0x88, 0x09, 0x01};

unsigned char efm_oam_proto_id[LLDP_SLOW_PROTO_ID_LEN] = {0x88, 0x09, 0x03};


struct lldp_protocol_identity proto_id[LLDP_PROTO_MAX] =
{
  {stp_proto_id, LLDP_802_3_PROTO_ID_LEN, LLDP_PROTO_STP, \
   "Spanning Tree Protocol"},
  {rstp_proto_id, LLDP_802_3_PROTO_ID_LEN, LLDP_PROTO_RSTP,
   "Rapid Spanning Tree Protocol"},
  {mstp_proto_id, LLDP_802_3_PROTO_ID_LEN, LLDP_PROTO_MSTP,
   "Multiple Spanning Tree Protocol"},
  {gmrp_proto_id, LLDP_802_3_PROTO_ID_LEN, LLDP_PROTO_GMRP,
   "GARP Multicast Registration Protocol"},
  {mmrp_proto_id, LLDP_MRP_PROTO_ID_LEN, LLDP_PROTO_MMRP,
   "MRP Multicast Registration Protocol"},
  {gvrp_proto_id, LLDP_802_3_PROTO_ID_LEN, LLDP_PROTO_GVRP,
   "GARP VLAN Registration Protocol"},
  {mvrp_proto_id, LLDP_MRP_PROTO_ID_LEN, LLDP_PROTO_MVRP,
   "MRP VLAN Registration Protocol"},
  {lacp_proto_id, LLDP_SLOW_PROTO_ID_LEN, LLDP_PROTO_LACP,
   "Link Aggregation"},
  {dot1x_proto_id, LLDP_DOT1X_PROTO_ID_LEN, LLDP_PROTO_DOT1X,
   "Port Authentication"},
  {efm_oam_proto_id, LLDP_SLOW_PROTO_ID_LEN, LLDP_PROTO_EFM_OAM,
   "Ethernet OAM"},
  {NULL, LLDP_UNKNOWN_PROTO_ID_LEN, LLDP_PROTO_UNKNOWN},
};
#endif

static char lldp_sys_cap_str[LLDP_SYS_CAP_MAX][LLDP_SYS_CAP_STR_LEN] =
{
  "Other",								/*bit:1*/
  "Repeater",							/*bit:2*/
  "Bridge",								/*bit:3*/
  "WLAN Access Point",					/*bit:4*/
  "Router",								/*bit:5*/
  "Telephone",							/*bit:6*/
  "DOCSIS cable device",				/*bit:7*/
  "Station",							/*bit:8*/
  "C-VLAN Component of a VLAN Bridge",	/*bit:9*/
  "S-VLAN Component of a VLAN Bridge",	/*bit:10*/
  "Two-port MAC Relay (TPMR)",			/*bit:11*/
};

#if 0
struct lldp_protocol_identity *
lldp_get_protocol_id(enum lldp_protocol proto)
{
  if (proto >= LLDP_PROTO_UNKNOWN)
    return NULL;

  return &proto_id[proto];
}
#endif

void lldp_show_if_neighbor(struct vty *vty, struct lldp_port *lldp_port)
{
	unsigned char			neighbor_num = 0, i = 0;
	struct lldp_msap*		msap_cache = lldp_port->msap_cache;
#if 0
	enum lldp_protocol 		proto;
	struct lldp_protocol_identity *proto_id;
#endif

	vty_out(vty, "%s", VTY_NEWLINE); 

	vty_out(vty,"Interface %s has %d neighbors:%s", lldp_port->if_name, lldp_port->msap_num, VTY_NEWLINE);

	if(0 == lldp_port->msap_num)
	{		
		return;
	}

	while(msap_cache != NULL)
	{
		neighbor_num += 1;
		
		vty_out(vty, "%s", VTY_NEWLINE); 
		vty_out(vty,"   %-30s:  %d %s", "Neighbor index", neighbor_num, VTY_NEWLINE);	

		/*svlan msg*/
		vty_out(vty,"   %-30s:  ", "svlan type");
		if(msap_cache->svlan_tpid != 0x0000)
		{
			vty_out(vty,"%04x", msap_cache->svlan_tpid);
		}
		vty_out(vty,"%s",VTY_NEWLINE);
		vty_out(vty,"   %-30s:  ", "svlan id");
		if(msap_cache->svlan != 0x0000)
		{
			vty_out(vty,"%d", msap_cache->svlan & 0x0FFF);
		}
		vty_out(vty,"%s",VTY_NEWLINE);

		/*cvlan msg*/
		vty_out(vty,"   %-30s:  ", "cvlan type");
		if(msap_cache->cvlan_tpid != 0x0000)
		{
			vty_out(vty,"%04x", msap_cache->cvlan_tpid);
		}
		vty_out(vty,"%s",VTY_NEWLINE);
		vty_out(vty,"   %-30s:  ", "cvlan id");
		if(msap_cache->cvlan != 0x0000)
		{
			vty_out(vty,"%d", msap_cache->cvlan & 0x0FFF);
		}
		vty_out(vty,"%s",VTY_NEWLINE);

		/*basic msg*/
		/*subtype:4 is mac addr*/
		if(4 == msap_cache->neighbor_msg.chassis_id_subtype)
		{			
			vty_out(vty,"   %-30s:  macAddress %s","Chassis type",VTY_NEWLINE);
			vty_out(vty,"   %-30s:  %02x:%02x:%02x:%02x:%02x:%02x %s",
							"Chassis ID",
							msap_cache->neighbor_msg.chassis_id_mac_addr[0],
							msap_cache->neighbor_msg.chassis_id_mac_addr[1],
							msap_cache->neighbor_msg.chassis_id_mac_addr[2],
							msap_cache->neighbor_msg.chassis_id_mac_addr[3],
							msap_cache->neighbor_msg.chassis_id_mac_addr[4],
							msap_cache->neighbor_msg.chassis_id_mac_addr[5],
							VTY_NEWLINE); 
		}
		/*subtype:4 is ip addr*/		
		else if(5 == msap_cache->neighbor_msg.chassis_id_subtype)
		{			
			vty_out(vty,"   %-30s:  IP Address %s","Chassis type", VTY_NEWLINE);
			vty_out(vty,"   %-30s:  %d.%d.%d.%d %s",
							"Chassis ID",
							msap_cache->neighbor_msg.chassis_id_net_addr[0],
							msap_cache->neighbor_msg.chassis_id_net_addr[1],
							msap_cache->neighbor_msg.chassis_id_net_addr[2],
							msap_cache->neighbor_msg.chassis_id_net_addr[3],
							VTY_NEWLINE);
		}

		
		if(PORT_ID_INTERFACE_NAME == msap_cache->neighbor_msg.port_id_subtype)
		{
			vty_out(vty,"   %-30s:  InterfaceName %s","Port ID type", VTY_NEWLINE);
			vty_out(vty,"   %-30s:  %s 	%s",
							"Port ID",
							msap_cache->neighbor_msg.if_name,
							VTY_NEWLINE);
		}
		else if(PORT_ID_NETWORK_ADDRESS == msap_cache->neighbor_msg.port_id_subtype)
		{
			vty_out(vty,"   %-30s:  NetworkAddr %s","Port ID type",VTY_NEWLINE);
			vty_out(vty,"   %-30s:  %d.%d.%d.%d 	%s",
							"Port ID",
							msap_cache->neighbor_msg.network_addr[0],
							msap_cache->neighbor_msg.network_addr[1],
							msap_cache->neighbor_msg.network_addr[2],
							msap_cache->neighbor_msg.network_addr[3],
							VTY_NEWLINE);
		}

		vty_out(vty,"   %-30s:  %ds	%s",
							"Expired time",
							msap_cache->neighbor_msg.rx_ttl,
							VTY_NEWLINE);

		vty_out(vty,"   %-30s:  %s %s",
							"Port description",
							msap_cache->neighbor_msg.pt_descr,
							VTY_NEWLINE);

		vty_out(vty,"   %-30s:  %s %s",
							"System name",
							msap_cache->neighbor_msg.sys_name,
							VTY_NEWLINE);

		vty_out(vty,"   %-30s:  %s %s",
							"System description",
							msap_cache->neighbor_msg.sys_descr,
							VTY_NEWLINE);

		vty_out(vty,"   %-30s:  ", "System capabilities supported");

		for (i = 0; i < LLDP_SYS_CAP_MAX; i++)
		{
			if(CHECK_FLAG(msap_cache->neighbor_msg.sys_cap, (1 << i)))
			{		
				vty_out(vty,"%s   ",lldp_sys_cap_str[i]);
			}
		}
		vty_out(vty,"%s",VTY_NEWLINE);

		vty_out(vty,"   %-30s:  ", "System capabilities enabled");

		for (i = 0; i < LLDP_SYS_CAP_MAX; i++)
		{
			if(CHECK_FLAG(msap_cache->neighbor_msg.sys_cap, (1 << i)))
			{		
				vty_out(vty,"%s   ",lldp_sys_cap_str[i]);
			}
		}
		vty_out(vty,"%s",VTY_NEWLINE);
				
		/*IP addr*/
		if(1 == msap_cache->neighbor_msg.mgmt_addr_sub_type)
		{				
			vty_out(vty,"   %-30s:  IP Address  %s","Management address type", VTY_NEWLINE);
			vty_out(vty,"   %-30s:  %d.%d.%d.%d 	%s",								
							"Management address",
							msap_cache->neighbor_msg.mgmt_addr[0],
							msap_cache->neighbor_msg.mgmt_addr[1],
							msap_cache->neighbor_msg.mgmt_addr[2],
							msap_cache->neighbor_msg.mgmt_addr[3],
							VTY_NEWLINE);
		}
		/*mac addr*/
		else if(6 == msap_cache->neighbor_msg.mgmt_addr_sub_type)
		{
			vty_out(vty,"   %-30s:  MAC Address  %s","Management address type", VTY_NEWLINE);
			vty_out(vty,"   %-30s:  %02x:%02x:%02x:%02x:%02x:%02x %s",
							"Management address",
							msap_cache->neighbor_msg.mgmt_addr[0],
							msap_cache->neighbor_msg.mgmt_addr[1],
							msap_cache->neighbor_msg.mgmt_addr[2],
							msap_cache->neighbor_msg.mgmt_addr[3],
							msap_cache->neighbor_msg.mgmt_addr[4],
							msap_cache->neighbor_msg.mgmt_addr[5], 
							VTY_NEWLINE); 
		}
		else
		{
			vty_out(vty,"   %-30s:  %s", "Management address type", VTY_NEWLINE);
			vty_out(vty,"   %-30s:  %s","Management address", VTY_NEWLINE);
		}

		vty_out(vty,"   %-30s:  %s, MAC:%02x:%02x:%02x:%02x:%02x:%02x%s", 
				"Arp notice", 
				msap_cache->arp_notice_flag ? "Yes" : "No",
				msap_cache->msap_id[0],
				msap_cache->msap_id[1],
				msap_cache->msap_id[2],
				msap_cache->msap_id[3],
				msap_cache->msap_id[4],
				msap_cache->msap_id[5], 
				VTY_NEWLINE);

			vty_out(vty,"   %-30s:  ifindex:0x%08x, vpn:%u%s", 
					" ", 
					msap_cache->pkt_ifindex, 
					msap_cache->vpn, 
					VTY_NEWLINE);

#if 0		
		/*802.1 msg*/
		vty_out(vty,"Port VLAN ID(PVID)\t\t:  %d %s", 
							msap_cache->neighbor_msg.port_vlan_id,
							VTY_NEWLINE); 
		vty_out(vty,"VLAN name of VLAN %d \t\t:  %s %s", 
							msap_cache->neighbor_msg.port_vlan_id,
							msap_cache->neighbor_msg.vlan_name,
							VTY_NEWLINE); 

		if(msap_cache->neighbor_msg.protocol)
		{
			vty_out(vty,"Protocol identity\t\t:  %s",VTY_NEWLINE); 
		
			for (proto = LLDP_PROTO_STP; proto < LLDP_PROTO_UNKNOWN; proto++)
			{
				if (CHECK_FLAG(msap_cache->neighbor_msg.protocol, (1 << proto))
					&& ((proto_id = lldp_get_protocol_id(proto)) != NULL))
				{
					vty_out(vty,"%s		%s", proto_id->protocol_str, VTY_NEWLINE); 
				}
			}
		}

		/*802.3 msg*/
		/*Auto-negotiation support/status*/
		vty_out(vty,"Auto-negotiation supported\t:  %s  %s",
							((msap_cache->neighbor_msg.autonego_support_status & 0x01)?
							"Yes":"NO"),
							VTY_NEWLINE); 
		vty_out(vty,"Auto-negotiation enabled\t:  %s  %s",
							(((msap_cache->neighbor_msg.autonego_support_status >> 1) & 0x01)?
							"Yes":"NO"),
							VTY_NEWLINE); 

		if(msap_cache->neighbor_msg.oper_mau_type == DOT3MAUTYPE10BASETHD)
		{		
			vty_out(vty,"Operational MAU type\t\t:  speed(10)/duplex(half) %s", VTY_NEWLINE); 
       	}
       	else if(msap_cache->neighbor_msg.oper_mau_type == DOT3MAUTYPE10BASETFD)
       	{
			vty_out(vty,"Operational MAU type\t\t:  speed(10)/duplex(full) %s", VTY_NEWLINE); 
       	}
       	else if(msap_cache->neighbor_msg.oper_mau_type == DOT3MAUTYPE100BASET2HD)
       	{
			vty_out(vty,"Operational MAU type\t\t:  speed(100)/duplex(half) %s", VTY_NEWLINE); 
       	}
       	else if(msap_cache->neighbor_msg.oper_mau_type == DOT3MAUTYPE100BASET2FD)
       	{
			vty_out(vty,"Operational MAU type\t\t:  speed(100)/duplex(full) %s", VTY_NEWLINE); 
       	}
       	else if(msap_cache->neighbor_msg.oper_mau_type == DOT3MAUTYPE1000BASETHD)
       	{
			vty_out(vty,"Operational MAU type\t\t:  speed(1000)/duplex(half) %s", VTY_NEWLINE); 
       	}
       	else if(msap_cache->neighbor_msg.oper_mau_type == DOT3MAUTYPE1000BASETFD)
       	{
			vty_out(vty,"Operational MAU type\t\t:  speed(1000)/duplex(full) %s", VTY_NEWLINE); 
       	}
       	else if(msap_cache->neighbor_msg.oper_mau_type == 0)
       	{
			vty_out(vty,"Operational MAU type\t\t:  UNKNOWN	%s", VTY_NEWLINE); 
		}

		vty_out(vty,"Power port class\t\t:  %s  %s",
							((msap_cache->neighbor_msg.mdi_power_support & 0x01)?
							"PSE":"PD"),
							VTY_NEWLINE); 
		vty_out(vty,"PSE power supported\t\t:  %s  %s",
							(((msap_cache->neighbor_msg.mdi_power_support >> 1) & 0x01)?
							"YES":"NO"),
							VTY_NEWLINE); 
		vty_out(vty,"PSE MDI power state\t\t:  %s  %s",
							(((msap_cache->neighbor_msg.mdi_power_support >> 2) & 0x01)?
							"YES":"NO"),
							VTY_NEWLINE); 
		vty_out(vty,"PSE pairs control ability\t:  %s  %s",
							(((msap_cache->neighbor_msg.mdi_power_support >> 3) & 0x01)?
							"YES":"NO"),
							VTY_NEWLINE); 
#if 0	/*defined in the RFC3621*/
		pethPsePortPowerPairs OBJECT-TYPE
		SYNTAX INTEGER	 {
				   signal(1),
				   spare(2)
		 }
#endif
		if(msap_cache->neighbor_msg.pse_power_pair == 1)
		{
			vty_out(vty,"Power pairs\t\t\t:  signal  %s",VTY_NEWLINE); 
		}
		else if(msap_cache->neighbor_msg.pse_power_pair == 2)
		{
			vty_out(vty,"Power pairs\t\t\t:  spare   %s",VTY_NEWLINE); 
		}
		else
		{
			vty_out(vty,"Power pairs\t\t\t:  UNKNOWN %s",VTY_NEWLINE); 
		}

#if 0	/*defined in the RFC3621*/
		pethPsePortPowerClassifications OBJECT-TYPE
		 SYNTAX INTEGER   {
				   class0(1),
				   class1(2),
				   class2(3),
				   class3(4),
				   class4(5)
		 }
#endif
		if(msap_cache->neighbor_msg.power_class == 1)
		{			
			vty_out(vty,"Port power classification\t:  class0 %s",VTY_NEWLINE); 
		}
		else if(msap_cache->neighbor_msg.power_class == 2)
		{			
			vty_out(vty,"Port power classification\t:  class1 %s",VTY_NEWLINE); 
		}
		else if(msap_cache->neighbor_msg.power_class == 3)
		{			
			vty_out(vty,"Port power classification\t:  class2 %s",VTY_NEWLINE); 
		}
		else if(msap_cache->neighbor_msg.power_class == 4)
		{			
			vty_out(vty,"Port power classification\t:  class3 %s",VTY_NEWLINE); 
		}
		else if(msap_cache->neighbor_msg.power_class == 5)
		{			
			vty_out(vty,"Port power classification\t:  class4 %s",VTY_NEWLINE); 
		}
		else
		{
			vty_out(vty,"Port power classification\t:  UNKNOWN %s",VTY_NEWLINE); 
		}

		vty_out(vty,"Link aggregation supported\t:  %s %s",
							((msap_cache->neighbor_msg.link_aggr_status_8023 & 0x01)?
							"Yes":"NO"),
							VTY_NEWLINE); 	

		vty_out(vty,"Link aggregation enabled\t:  %s %s",
							(((msap_cache->neighbor_msg.link_aggr_status_8023 >> 1) & 0x01)?
							"Yes":"NO"),
							VTY_NEWLINE);	

		vty_out(vty,"Aggregation port ID\t\t:  %d %s",
							msap_cache->neighbor_msg.link_aggr_id_8023,
							VTY_NEWLINE);	

		
		vty_out(vty,"Maximum frame Size\t\t:  %d %s",
							msap_cache->neighbor_msg.max_frame_size,
							VTY_NEWLINE); 
#endif
		
		msap_cache = msap_cache->next;
	}
	
	return;
}


void lldp_show_if_statistics(struct vty *vty, struct lldp_port *lldp_port)
{
	vty_out(vty,"%s", VTY_NEWLINE); 

	vty_out(vty,"Statistics for %s :%s", lldp_port->if_name, VTY_NEWLINE); 

	vty_out(vty,"%-40s: %lld %s",
					"Transmitted Frames Total",
					lldp_port->tx.statistics.statsFramesOutTotal,
					VTY_NEWLINE); 
	
	vty_out(vty,"%-40s: %lld %s",
					"Received Frames Total",
					lldp_port->rx.statistics.statsFramesInTotal,
					VTY_NEWLINE); 

	vty_out(vty,"%-40s: %lld %s",
					"Frames Discarded Total",
					lldp_port->rx.statistics.statsFramesDiscardedTotal,
					VTY_NEWLINE); 

	vty_out(vty,"%-40s: %lld %s",
					"Frames Error Total",
					lldp_port->rx.statistics.statsFramesInErrorsTotal,
					VTY_NEWLINE); 

	vty_out(vty,"%-40s: %lld %s",
					"TLVs Discarded Total",
					lldp_port->rx.statistics.statsTLVsDiscardedTotal,
					VTY_NEWLINE); 

	vty_out(vty,"%-40s: %lld %s",
					"TLVs Unrecognized Total",
					lldp_port->rx.statistics.statsTLVsUnrecognizedTotal,
					VTY_NEWLINE); 

	vty_out(vty,"%-40s: %lld %s",
					"Neighbors Expired Total",
					lldp_port->rx.statistics.statsAgeoutsTotal,
					VTY_NEWLINE); 
	return;
}

void lldp_show_neighbor(struct vty *vty)
{
	struct lldp_port 	*lldp_port = NULL;
	struct hash_bucket	*pbucket = NULL;
	int					cursor = 0, flag = 0;
	struct l2if			*pif = NULL;

	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif = pbucket->data;
		if(pif != NULL)
		{
			lldp_port = pif->lldp_port_info;
			if(lldp_port != NULL)
			{
				lldp_show_if_neighbor(vty, lldp_port);
				flag = 1;
			}
		}
	}
	if(flag == 0)
	{			
		vty_out(vty,"%s", VTY_NEWLINE); 
		vty_out(vty,"Error: No lldp info, Maybe No Interface enable LLDP. %s", VTY_NEWLINE); 
	}

	return;
}

void lldp_show_statistics(struct vty *vty)
{
	struct lldp_port	*lldp_port = NULL;
	struct hash_bucket	*pbucket = NULL;
	int					cursor = 0, flag = 0;
	struct l2if			*pif = NULL;

	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif = pbucket->data;
		if(pif != NULL)
		{
			lldp_port = pif->lldp_port_info;
			
			if(lldp_port != NULL)
			{
				lldp_show_if_statistics(vty, lldp_port);
				flag = 1;
			}
		}
	}

	if(flag == 0)
	{	
		vty_out(vty,"%s", VTY_NEWLINE); 
		vty_out(vty,"Error: No lldp info, Maybe No Interface enable LLDP. %s", VTY_NEWLINE); 
	}
	
	return;
}

void lldp_show_if_local(struct vty *vty, struct lldp_port *lldp_port, struct l2if *pif)
{
	if ((NULL == pif) || (NULL == lldp_port))
	{
		vty_error_out(vty, " LLDP on the port is already disabled, or the port not exist! %s", VTY_NEWLINE);
		return;
	}

	vty_out(vty,"%s %-30s: %s",
						"Interface",						
						lldp_port->if_name, VTY_NEWLINE); 

	vty_out(vty,"%-40s: %s %s",
						"LLDP Enable Status",
						((lldp_port->adminStatus)?"enable":"disable"), 
						VTY_NEWLINE); 
	vty_out(vty,"%-40s: %s %s",
						"Port link Status",
						((lldp_port->portEnabled)?"link up":"link down"), 
						VTY_NEWLINE); 

	vty_out(vty,"%-40s: %d %s",
						"Total Neighbors",
						lldp_port->msap_num, 
						VTY_NEWLINE); 

	vty_out(vty,"%-40s: interface name %s",
						"Port ID subtype",
						VTY_NEWLINE); 
	
	vty_out(vty,"%-40s: %s %s",
						"Port ID",
						lldp_port->if_name,
						VTY_NEWLINE); 

	vty_out(vty,"%-40s: %s %s",
						"Port alias",
						pif->alias,
						VTY_NEWLINE); 

	vty_out(vty, "%-40s: %s(vlan=%u) %s", 
						"ARP learning",
						((CHECK_FLAG(lldp_port->arp_nd_learning, LLDP_LEARNING_ARP)) ? "enable" : "disable"),
						lldp_port->arp_nd_learning_vlan, 
						VTY_NEWLINE);

#if 0
	/*802.1*/
	vty_out(vty,"Port And Protocol VLAN ID(PPVID): do not supported %s \n", VTY_NEWLINE); 

	vty_out(vty,"Port VLAN ID(PVID)\t\t: %d %s", lldp_port->pvid, VTY_NEWLINE); 
	vty_out(vty,"VLAN name of VLAN %d\t\t: %s %s",
						lldp_port->pvid, 
						lldp_port->vlan_name,
						VTY_NEWLINE); 
	//FIXME
	vty_out(vty,"Protocol identity\t\t: do not support. %s", VTY_NEWLINE); 

	/*802.3*/
	vty_out(vty,"Auto-negotiation supported\t: %s %s",
						((lldp_port->autonego_support & 0x01)? "Yes" : "NO"),
						VTY_NEWLINE); 

	vty_out(vty,"Auto-negotiation enabled\t: %s %s",
						(((lldp_port->autonego_support >> 1) & 0x01)? "Yes" : "NO"),
						VTY_NEWLINE); 

	if(lldp_port->oper_mau_type == DOT3MAUTYPE10BASETHD)
	{		
		vty_out(vty,"Operational MAU type\t\t: speed(10)/duplex(half) 	%s", VTY_NEWLINE); 
    }
    else if(lldp_port->oper_mau_type == DOT3MAUTYPE10BASETFD)
    {
		vty_out(vty,"Operational MAU type\t\t: speed(10)/duplex(full) 	%s", VTY_NEWLINE); 
	}
	else if(lldp_port->oper_mau_type == DOT3MAUTYPE100BASET2HD)
	{
		vty_out(vty,"Operational MAU type\t\t: speed(100)/duplex(half) 	%s", VTY_NEWLINE); 
	}
	else if(lldp_port->oper_mau_type == DOT3MAUTYPE100BASET2FD)
	{
		vty_out(vty,"Operational MAU type\t\t: speed(100)/duplex(full)	%s", VTY_NEWLINE); 
	}
	else if(lldp_port->oper_mau_type == DOT3MAUTYPE1000BASETHD)
	{
		vty_out(vty,"Operational MAU type\t\t: speed(1000)/duplex(half)	%s", VTY_NEWLINE); 
	}
	else if(lldp_port->oper_mau_type == DOT3MAUTYPE1000BASETFD)
	{
		vty_out(vty,"Operational MAU type\t\t: speed(1000)/duplex(full)	%s", VTY_NEWLINE); 
	}
    else if(lldp_port->oper_mau_type == 0)
	{
		vty_out(vty,"Operational MAU type\t\t: UNKNOWN	%s", VTY_NEWLINE); 
	}	


	vty_out(vty,"Power port class\t\t: %s  %s",
						((lldp_port->mdi_power_support & 0x01)?
						"PSE":"PD"),
						VTY_NEWLINE); 
	vty_out(vty,"PSE power supported\t\t: %s  %s",
						(((lldp_port->mdi_power_support >> 1) & 0x01)?
						"YES":"NO"),
						VTY_NEWLINE); 
	vty_out(vty,"PSE MDI power state\t\t: %s  %s",
						(((lldp_port->mdi_power_support >> 2) & 0x01)?
						"YES":"NO"),
						VTY_NEWLINE); 
	vty_out(vty,"PSE pairs control ability\t: %s  %s",
						(((lldp_port->mdi_power_support >> 3) & 0x01)?
						"YES":"NO"),
						VTY_NEWLINE); 
	if(lldp_port->pse_power_pair == 1)
	{
		vty_out(vty,"Power pairs\t\t\t\t: signal  %s",VTY_NEWLINE); 
	}
	else if(lldp_port->pse_power_pair == 2)
	{
		vty_out(vty,"Power pairs\t\t\t\t: spare   %s",VTY_NEWLINE); 
	}
	else
	{
		vty_out(vty,"Power pairs\t\t\t: UNKNOWN %s",VTY_NEWLINE); 
	}

	if(lldp_port->power_class == 1)
	{			
		vty_out(vty,"Port power classification\t: class0 %s",VTY_NEWLINE); 
	}
	else if(lldp_port->power_class == 2)
	{			
		vty_out(vty,"Port power classification\t: class1 %s",VTY_NEWLINE); 
	}
	else if(lldp_port->power_class == 3)
	{			
		vty_out(vty,"Port power classification\t: class2 %s",VTY_NEWLINE); 
	}
	else if(lldp_port->power_class == 4)
	{			
		vty_out(vty,"Port power classification\t: class3 %s",VTY_NEWLINE); 
	}
	else if(lldp_port->power_class == 5)
	{			
		vty_out(vty,"Port power classification\t: class4 %s",VTY_NEWLINE); 
	}
	else
	{
		vty_out(vty,"Port power classification\t: UNKNOWN %s",VTY_NEWLINE); 
	}

	vty_out(vty,"Link aggregation supported\t: %s %s",
						((lldp_port->link_aggr_status & 0x01)?
						"Yes":"NO"),
						VTY_NEWLINE);	
	
	vty_out(vty,"Link aggregation enabled\t: %s %s",
						(((lldp_port->link_aggr_status >> 1) & 0x01)?
						"Yes":"NO"),
						VTY_NEWLINE);	
	
	vty_out(vty,"Aggregation port ID\t\t: %d %s",
						lldp_port->link_aggr_id,
						VTY_NEWLINE);	
	
	vty_out(vty,"Maximum frame Size\t\t: %d %s",
						lldp_port->max_frame_size,
						VTY_NEWLINE);	
#endif
	vty_out(vty," %s", VTY_NEWLINE); 

	return;
}


void lldp_show_local(struct vty *vty)
{
	struct lldp_port 	*lldp_port = NULL;
	struct hash_bucket	*pbucket = NULL;
	int					cursor = 0;
	struct l2if			*pif = NULL;
	char				mng_addr[256];

	/*sys msg*/
	vty_out(vty,"%s", VTY_NEWLINE); 
	vty_out(vty,"sys infomation: %s",VTY_NEWLINE);
	
	vty_out(vty,"%-40s: macAddress %s",
					"Chassis type",
					VTY_NEWLINE);
	vty_out(vty,"%-40s: %02x:%02x:%02x:%02x:%02x:%02x %s",
							"Chassis ID",
							lldp_global_msg.source_mac[0],
							lldp_global_msg.source_mac[1],
							lldp_global_msg.source_mac[2],
							lldp_global_msg.source_mac[3],
							lldp_global_msg.source_mac[4],
							lldp_global_msg.source_mac[5], 
							VTY_NEWLINE);
	
	vty_out(vty,"%-40s: %s %s",
							"System name",
							lldp_global_msg.system_name,
							VTY_NEWLINE);
	
	vty_out(vty,"%-40s: %s %s", 
							"System description",
							lldp_global_msg.system_desc,
							VTY_NEWLINE); 
	
	/*System configuration msg*/
	vty_out(vty,"%s", VTY_NEWLINE); 
	vty_out(vty,"System configuration:			   %s",VTY_NEWLINE); 
	
	vty_out(vty,"%-40s: %d    (default is 30s)%s",
							"LLDP Message Tx Interval",
							lldp_global_msg.msgTxInterval,
							VTY_NEWLINE);
	
	vty_out(vty,"%-40s: %d    (default is 4)%s",
							"LLDP Message Tx Hold Multiplier",
							lldp_global_msg.msgTxHold,
							VTY_NEWLINE);
	
#if 0
	vty_out(vty,"LLDP Notification Interval\t\t: %d \t\t(default is 5s)%s",
							lldp_global_msg.trapInterval,
							VTY_NEWLINE);
	
	vty_out(vty,"LLDP Notification Enable\t\t: %s \t(default is disable)%s",
							((lldp_global_msg.trapEnable)? "enable":"disable"),
							VTY_NEWLINE);
#endif

	/*MAC addr*/
	if(6 == lldp_global_msg.mng_addr_sub)
	{
		sprintf(mng_addr, "%-40s: %s[%02x:%02x:%02x:%02x:%02x:%02x]    (default is MAC Address)",
				"LLDP Management Address",
				"MAC",
				lldp_global_msg.source_mac[0], lldp_global_msg.source_mac[1],
				lldp_global_msg.source_mac[2], lldp_global_msg.source_mac[3],
				lldp_global_msg.source_mac[4], lldp_global_msg.source_mac[5]);
	}
	else if(1 == lldp_global_msg.mng_addr_sub)/*IP addr*/
	{
		if((0x00 == lldp_global_msg.source_ipaddr[0])
			&& (0x00 == lldp_global_msg.source_ipaddr[1])
			&& (0x00 == lldp_global_msg.source_ipaddr[2])
			&& (0x00 == lldp_global_msg.source_ipaddr[3]))
		{
			sprintf(mng_addr, "%-40s: %s[%02x:%02x:%02x:%02x:%02x:%02x], management address config:IP Address.    (default is MAC Address)",
					"LLDP Management Address",
					"MAC",
					lldp_global_msg.source_mac[0], lldp_global_msg.source_mac[1],
					lldp_global_msg.source_mac[2], lldp_global_msg.source_mac[3],
					lldp_global_msg.source_mac[4], lldp_global_msg.source_mac[5]);		
		}
		else
		{
			sprintf(mng_addr, "%-40s:%s[%d.%d.%d.%d]    (default is MAC Address)",
					"LLDP Management Address",
					"IP",
					lldp_global_msg.source_ipaddr[0], lldp_global_msg.source_ipaddr[1],
					lldp_global_msg.source_ipaddr[2], lldp_global_msg.source_ipaddr[3]);		
		}
	}
	vty_out(vty, "%s %s", mng_addr, VTY_NEWLINE);


	/*if msg*/
	vty_out(vty,"%s", VTY_NEWLINE); 
	vty_out(vty,"Port information:				   %s",VTY_NEWLINE); 
	
	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif = pbucket->data;
		if(pif != NULL)
		{
			lldp_port = pif->lldp_port_info;
			if(lldp_port != NULL)
			{
				lldp_show_if_local(vty, lldp_port, pif);
			}
		}
	}

	return;
}

void lldp_show_if_tlv_cfg(struct vty *vty, struct lldp_port *lldp_port)
{	
	vty_out(vty,"LLDP tlv-config of port[%s]: %s", lldp_port->if_name, VTY_NEWLINE); 

	vty_out(vty,"%-40s state        Default   %s",
				"name",
				VTY_NEWLINE);

	/*basic*/
	vty_out(vty,"Basic optional TLV:   		  %s", VTY_NEWLINE);
	vty_out(vty,"%-40s %s          Yes %s",
				"Port Description TLV",
				((CHECK_FLAG(lldp_port->tlv_cfg.basic_tlv_tx_enable, 
					PORT_DESCRIPTION_TLV_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);
	
	vty_out(vty,"%-40s %s          Yes %s",
				"System Name TLV",
				((CHECK_FLAG(lldp_port->tlv_cfg.basic_tlv_tx_enable, 
					SYSTEM_NAME_TLV_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);
	
	vty_out(vty,"%-40s %s          Yes %s",
				"System Description TLV",
				((CHECK_FLAG(lldp_port->tlv_cfg.basic_tlv_tx_enable, 
					SYSTEM_DESCRIPTION_TLV_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);

	vty_out(vty,"%-40s %s          Yes %s",
				"System Capabilities TLV",
				((CHECK_FLAG(lldp_port->tlv_cfg.basic_tlv_tx_enable, 
					SYSTEM_CAPABILITIES_TLV_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);

	vty_out(vty,"%-40s %s          Yes %s",
				"Management Address TLV",
				((CHECK_FLAG(lldp_port->tlv_cfg.basic_tlv_tx_enable, 
					MANAGEMENT_ADDRESS_TLV_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);
#if 0
	/*802.1*/
	vty_out(vty,"Port VLAN ID TLV\t\t\t  %s\t\t NO %s",
				((CHECK_FLAG(lldp_port->tlv_cfg.sub_tlv_tx_enable, 
					PORT_VLAN_ID_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);
	
	vty_out(vty,"Port And Protocol VLAN ID TLV\t\t  %s\t\t NO %s",
				((CHECK_FLAG(lldp_port->tlv_cfg.sub_tlv_tx_enable, 
					PROTO_VLAN_ID_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);
	
	vty_out(vty,"VLAN Name TLV\t\t\t\t  %s\t\t NO %s",
				((CHECK_FLAG(lldp_port->tlv_cfg.sub_tlv_tx_enable, 
					PORT_VLAN_NAME_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);
	vty_out(vty,"Protocol identity TLV\t\t\t  %s\t\t NO %s",
				((CHECK_FLAG(lldp_port->tlv_cfg.sub_tlv_tx_enable, 
					PROTOCOL_ID_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);

	/*802.3*/
	vty_out(vty,"MAC-Physic TLV\t\t\t\t  %s\t\t NO %s",
				((CHECK_FLAG(lldp_port->tlv_cfg.sub_tlv_tx_enable, 
					MAC_PHY_CONFIG_STATUS_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);
	
	vty_out(vty,"Power via MDI TLV\t\t\t  %s\t\t NO %s",
				((CHECK_FLAG(lldp_port->tlv_cfg.sub_tlv_tx_enable, 
					POWER_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);
	
	vty_out(vty,"Link Aggregation TLV\t\t\t  %s\t\t NO %s",
				((CHECK_FLAG(lldp_port->tlv_cfg.sub_tlv_tx_enable, 
					LINK_AGG_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);
	
	vty_out(vty,"Maximum Frame Size TLV\t\t\t  %s\t\t NO %s",
				((CHECK_FLAG(lldp_port->tlv_cfg.sub_tlv_tx_enable, 
					MAX_FRAME_SIZE_TX_ENABLE))? "Yes":"NO"),
				VTY_NEWLINE);
#endif
	return;
}

void lldp_show_tlv_cfg(struct vty *vty)
{
	struct lldp_port 	*lldp_port = NULL;
	struct hash_bucket	*pbucket = NULL;
	int					cursor = 0, flag = 0;
	struct l2if			*pif = NULL;
	
	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(pif != NULL)
		{
			lldp_port = pif->lldp_port_info;
			if(lldp_port != NULL)
			{
				lldp_show_if_tlv_cfg(vty, lldp_port);
				flag = 1;
			}
		}
	}

	if(0 == flag)
	{	
		vty_out(vty,"%s", VTY_NEWLINE); 
		vty_error_out(vty," No lldp info, Maybe No Interface enable LLDP. %s", VTY_NEWLINE); 
	}

	return;	
}

