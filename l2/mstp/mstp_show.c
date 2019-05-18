
/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_show.c
*
*  date: 2017.4
*
*  modify:2017.10~2017.12
*
*  modify:	2018.3.12 modified by liufuying to make mstp module code beautiful
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lib/log.h"
#include "lib/vty.h"
#include "l2/l2_if.h"
#include "lib/hash1.h"

#include "mstp.h"
#include "mstp_show.h"
#include "mstp_port.h"
#include "mstp_base_procedure.h"

extern struct mstp_bridge	*mstp_global_bridge;
extern struct hash_table 	l2if_table;
extern struct mstp_vlan		bridge_cist_vlan_map;

#if 0
struct mstp_mode_str mstp_mode[] = 
{
	{PROTO_VERID_STP, "STP"},
	{PROTO_VERID_RSTP, "RSTP"},
	{PROTO_VERID_MSTP, "MSTP"}
};
#endif

struct mstp_port_status port_status[] =
{
	{MSTP_NULL, 			"NULL"},
	{MSTP_ST_DISCARDING,	"DISCARDING"},
	{MSTP_ST_LEARNING,		"LEARNING"},
	{MSTP_ST_FORWARDING, 	"FORWARDING"},
	
};

struct mstp_port_role port_role[] =
{
	{MSTP_NULL,					"NULL"},
	{MSTP_PORT_ROLE_DISABLED,	"Disabled"},
	{MSTP_PORT_ROLE_ALTERNATE,	"Alternate Port"},
	{MSTP_PORT_ROLE_BACKUP, 	"Backup Port"},
	{MSTP_PORT_ROLE_ROOT, 		"Root Port"},
	{MSTP_PORT_ROLE_DESIGNATED, "Designated Port"},
	{MSTP_PORT_ROLE_MASTER, 	"Master Port"}
};


struct mstp_bridge_status br_root_role[] =
{
	{MSTP_ROOT_DEF,				"NULL"},
	{MSTP_ROOT_MASTER,			"master"},
	{MSTP_ROOT_SLAVE,			"slave"},
};


unsigned int mstp_vlan_str_get_by_bitmap(unsigned char *bitmap, char *str)
{
	unsigned short		vlan_id = 0;
	int 				length = 0;
	int 				j = 0, k = 0;	/*j : max vlan num */

	/*-1 is invalid*/
	int 				tmp[MSTP_VLAN_MAX_ID] = {-1};

	/*get vlan id base on vlan map*/
	memset(tmp, -1, sizeof(int)*MSTP_VLAN_MAX_ID);
	for(vlan_id = 1, j = 0; vlan_id <= MSTP_VLAN_MAX_ID; vlan_id++)
	{
		if(mstp_vlan_map_get(bitmap, vlan_id))
		{
			tmp[j++] = vlan_id;
		}
	}

	if(j >= 1)
	{
		length = sprintf(str, "%d", tmp[0]);
	}
	else
	{
		return MSTP_TRUE;
	}

	k = 1;
	while(-1 != tmp[k] || k < j)
	{
		/*If the port number and the previous port number are continuous*/
		if(1 == tmp[k] - tmp[k-1])
		{
			/*if have ' '*/
			if(str[length-1] != ' ')
			{
				sprintf(&str[length++], "%c", ' ');
				sprintf(&str[length++], "%c", 't');
				sprintf(&str[length++], "%c", 'o');				
				sprintf(&str[length++], "%c", ' ');
			}
			/*To determine whether it is the last port number*/
			if(-1 == tmp[k+1] || k == (j-1))
			{
				length += sprintf(&str[length],"%d",tmp[k]);			
				break;
			}
			else
			{
				/*If not the last one, and not continuous*/
				if(1 != tmp[k+1] - tmp[k])
				{
					length += sprintf(&str[length],"%d",tmp[k]);
				}
			}
		}
		/*If not continuous*/
		else
		{
			/*To determine whether the comma, if not plus one*/
			if(str[length-1] != ' ')
			{
				sprintf(&str[length++],"%c", ',');
				sprintf(&str[length++],"%c", ' ');
			}

			length += sprintf(&str[length],"%d",tmp[k]);
		}

		k++;
	}

    return MSTP_TRUE;
}




void mstp_show_port_common(struct vty *vty, struct mstp_port *mstp_port)
{
	struct mstp_common_port		*common_info = NULL;

	if(NULL == mstp_port)
	{
		return;
	}

	common_info = &(mstp_port->cist_port.common_info);

	vty_out(vty, "%s", VTY_NEWLINE);

	/*example:		----[Port(Ethernet0/0/1)][FORWARDING]----*/ 	
	vty_out(vty, "-------[Port(%s)] status[%s]-------%s", mstp_port->if_name, port_status[common_info->sm_state_trans_state].port_status_str, VTY_NEWLINE);

	/*enable / disable*/
	vty_out(vty, "%-30s: %s	%s", 
	"Port Protocol",			
	(mstp_port->adminStatus)? "Enabled":"Disabled",
	VTY_NEWLINE);

	/*port stp mode: stp / rstp / mstp*/
	switch(mstp_port->stp_mode)
	{
		case PROTO_VERID_STP:
		{
			vty_out(vty, "%-30s: STP %s","Port STP Mode", VTY_NEWLINE);
		}
			break;
		case PROTO_VERID_RSTP:
		{
			vty_out(vty, "%-30s: RSTP %s","Port STP Mode", VTY_NEWLINE);
		}
			break;
		case PROTO_VERID_MSTP:
		{
			vty_out(vty, "%-30s: MSTP %s","Port STP Mode", VTY_NEWLINE);
		}
			break;
		default:
			break;
	}

	/*Designated Bridge*/
	if((MSTP_ST_DISCARDING == common_info->sm_state_trans_state) && (PORT_LINK_DOWN == mstp_port->link_status))
	{
		/*Designated Bridge*/
		vty_out(vty, "%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x  %s",
			"Designated Bridge",
			(unsigned short)mstp_port->cist_port.designated_priority.designated_bridge_id.pri[0] << 8
			| (unsigned short)mstp_port->cist_port.designated_priority.designated_bridge_id.pri[1],
			mstp_port->cist_port.designated_priority.designated_bridge_id.mac_addr[0],
			mstp_port->cist_port.designated_priority.designated_bridge_id.mac_addr[1],
			mstp_port->cist_port.designated_priority.designated_bridge_id.mac_addr[2],
			mstp_port->cist_port.designated_priority.designated_bridge_id.mac_addr[3],
			mstp_port->cist_port.designated_priority.designated_bridge_id.mac_addr[4],
			mstp_port->cist_port.designated_priority.designated_bridge_id.mac_addr[5],
			VTY_NEWLINE);
	}
	else
	{
		/*Designated Bridge*/
		vty_out(vty, "%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x  %s",
				"Designated Bridge",
				(unsigned short)mstp_port->cist_port.port_priority.designated_bridge_id.pri[0] << 8
				| (unsigned short)mstp_port->cist_port.port_priority.designated_bridge_id.pri[1],
				mstp_port->cist_port.port_priority.designated_bridge_id.mac_addr[0],
				mstp_port->cist_port.port_priority.designated_bridge_id.mac_addr[1],
				mstp_port->cist_port.port_priority.designated_bridge_id.mac_addr[2],
				mstp_port->cist_port.port_priority.designated_bridge_id.mac_addr[3],
				mstp_port->cist_port.port_priority.designated_bridge_id.mac_addr[4],
				mstp_port->cist_port.port_priority.designated_bridge_id.mac_addr[5],
				VTY_NEWLINE);
	}

	/*Port Role, example :Designated Port*/
	vty_out(vty, "%-30s: %s %s","Port Role", port_role[common_info->selectedRole].port_role_str, VTY_NEWLINE);

	/*Priority*/
	vty_out(vty, "%-30s: %u %s","Port Priority", mstp_port->port_pri, VTY_NEWLINE);

	/*Path Cost*/
	vty_out(vty, "%-30s: %u %s","Port Path Cost", mstp_port->cist_port.common_info.port_cost, VTY_NEWLINE);

	/*Edge Port*/
	vty_out(vty, "%-30s: %s %s", 
			"Port Edge Port",
			mstp_port->mstp_edge_port ? "Enable" : "Disable",
			VTY_NEWLINE);

	/*filter port*/
	vty_out(vty, "%-30s: %s %s", 
			"Port Filter Port",
			mstp_port->mstp_filter_port ? "Enable" : "Disable",
			VTY_NEWLINE);

	/*P2P*/
	vty_out(vty, "%-30s: %s %s", 
			"Port Mstp P2P",
			mstp_port->mstp_p2p ? "Enable" : "Disable",
			VTY_NEWLINE);
	
	vty_out(vty, "%s", VTY_NEWLINE);
	
	vty_out(vty, "%-30s: %llu %s", "TC   Send", mstp_port->bpdu_tc_send, VTY_NEWLINE);		
	vty_out(vty, "%-30s: %llu %s", "TCN  Send", mstp_port->bpdu_tcn_send, VTY_NEWLINE);
	vty_out(vty, "%-30s: %llu %s", "TC   Recv", mstp_port->bpdu_tc_recv, VTY_NEWLINE);		
	vty_out(vty, "%-30s: %llu %s", "TCN  Recv", mstp_port->bpdu_tcn_recv, VTY_NEWLINE);
	
	vty_out(vty, "%-30s: %llu %s", 
							"BPDU Send",
							mstp_port->bpdu_config_send
							+mstp_port->bpdu_rst_send
							+mstp_port->bpdu_mst_send,
							VTY_NEWLINE);
	
	vty_out(vty, "          %s: %llu, RST: %llu, MST: %llu %s",
							"Config",
							mstp_port->bpdu_config_send,
							mstp_port->bpdu_rst_send,
							mstp_port->bpdu_mst_send,
							VTY_NEWLINE);		
	vty_out(vty, "%s", VTY_NEWLINE);
	
	vty_out(vty, "%-30s: %llu %s", 
							"BPDU Received",
							mstp_port->bpdu_config_recv
							+mstp_port->bpdu_rst_recv
							+mstp_port->bpdu_mst_recv,
							VTY_NEWLINE);
	
	vty_out(vty, "          %s: %llu, RST: %llu, MST: %llu %s",
							"Config",
							mstp_port->bpdu_config_recv,
							mstp_port->bpdu_rst_recv,
							mstp_port->bpdu_mst_recv,
							VTY_NEWLINE);	
	
	vty_out(vty, "%s", VTY_NEWLINE);

	return;
}


void mstp_show_msti_common(struct vty *vty, struct mstp_port *mstp_port, unsigned int instance_id)
{
	char				vlan_str[1024];

	if ((NULL == mstp_port) || (NULL == mstp_global_bridge))
	{
		return;
	}
	
	/*msti global info*/
	vty_out(vty, "-------[MSTI %u Global Info]-------%s", instance_id+1, VTY_NEWLINE);

	/*instance id*/
	vty_out(vty, "%-30s: %u %s", "Instance Id", instance_id+1, VTY_NEWLINE);

	/*vlan map*/
	mstp_vlan_str_get_by_bitmap(mstp_global_bridge->mstp_instance[instance_id].msti_vlan.vlan_map, vlan_str);
	vty_out(vty, "%-30s: %s %s", "Vlan Map", vlan_str, VTY_NEWLINE);

	/*vlan priority */
	vty_out(vty, "%-30s: %u %s", "Priority", mstp_global_bridge->mstp_instance[instance_id].msti_br_priority, VTY_NEWLINE);


	/*bridge id*/
	vty_out(vty,"%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x %s",
			"MSTI Bridge ID",
			(unsigned short)mstp_global_bridge->mstp_instance[instance_id].msti_br_priority,
			mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[0],
			mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[1],
			mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[2],
			mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[3],
			mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[4],
			mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[5], 
			VTY_NEWLINE);

	/*region root id*/
	vty_out(vty, "%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x / %d%s",
			"MSTI RegRoot/IRPC",
			(unsigned short)mstp_port->msti_port[instance_id]->designated_priority.rg_root_id.pri[0] << 8
			| (unsigned short)mstp_port->msti_port[instance_id]->designated_priority.rg_root_id.pri[1],
			mstp_port->msti_port[instance_id]->designated_priority.rg_root_id.mac_addr[0],
			mstp_port->msti_port[instance_id]->designated_priority.rg_root_id.mac_addr[1],
			mstp_port->msti_port[instance_id]->designated_priority.rg_root_id.mac_addr[2],
			mstp_port->msti_port[instance_id]->designated_priority.rg_root_id.mac_addr[3],
			mstp_port->msti_port[instance_id]->designated_priority.rg_root_id.mac_addr[4],
			mstp_port->msti_port[instance_id]->designated_priority.rg_root_id.mac_addr[5],
			mstp_port->msti_port[instance_id]->designated_priority.internal_root_path_cost,		
			VTY_NEWLINE);

	return;

}


void mstp_show_common(struct vty *vty)
{
	char						cist_vlan_str[256] = {0};

	if (NULL == mstp_global_bridge)
	{
		return;
	}

	mstp_vlan_str_get_by_bitmap(bridge_cist_vlan_map.vlan_map, cist_vlan_str);

	/*show stp region config*/
	vty_out(vty,"%s", VTY_NEWLINE);
	vty_out(vty, "-------[ Region config ]-------%s", VTY_NEWLINE);	
	vty_out(vty,"%s", VTY_NEWLINE);
	vty_out(vty, "%-30s: %s%s", "Region   name", mstp_global_bridge->mst_cfg_id.cfg_name, VTY_NEWLINE);
	vty_out(vty, "%-30s: %u%s", "Revision level", mstp_global_bridge->mst_cfg_id.revison_level, VTY_NEWLINE);
	vty_out(vty, "%-30s: %s %s", "CIST Vlan Map", cist_vlan_str, VTY_NEWLINE);
	vty_out(vty,"%s", VTY_NEWLINE);
	vty_out(vty,"%s", VTY_NEWLINE);


	/*show mstp mode */
	switch(mstp_global_bridge->stp_mode)
	{
		case PROTO_VERID_STP:
		{
			vty_out(vty, "-------[CIST Global Info][Mode STP]-------%s", VTY_NEWLINE);
		}
			break;
		case PROTO_VERID_RSTP:
		{
			vty_out(vty, "-------[CIST Global Info][Mode RSTP]-------%s", VTY_NEWLINE);
		}
			break;
		case PROTO_VERID_MSTP:
		{
			vty_out(vty, "-------[CIST Global Info][Mode MSTP]-------%s", VTY_NEWLINE);
		}
			break;
		default:
			break;
	}

	/*show mstp mac addr*/
	vty_out(vty,"%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x %s",
							"Bridge",
							((unsigned short)mstp_global_bridge->cist_br.bridge_mac.pri[0] << 8 
							| (unsigned short)mstp_global_bridge->cist_br.bridge_mac.pri[1]),
							mstp_global_bridge->bridge_mac.mac_addr[0],
							mstp_global_bridge->bridge_mac.mac_addr[1],
							mstp_global_bridge->bridge_mac.mac_addr[2],
							mstp_global_bridge->bridge_mac.mac_addr[3],
							mstp_global_bridge->bridge_mac.mac_addr[4],
							mstp_global_bridge->bridge_mac.mac_addr[5], 
							VTY_NEWLINE);
	/*show mstp region root mac addr*/
	vty_out(vty,"%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x / %d%s",
							"CIST Root/ERPC",
							((unsigned short)mstp_global_bridge->cist_br.root_priority.root_id.pri[0] << 8 
							| (unsigned short)mstp_global_bridge->cist_br.root_priority.root_id.pri[1]),
							mstp_global_bridge->cist_br.root_priority.root_id.mac_addr[0],
							mstp_global_bridge->cist_br.root_priority.root_id.mac_addr[1],
							mstp_global_bridge->cist_br.root_priority.root_id.mac_addr[2],
							mstp_global_bridge->cist_br.root_priority.root_id.mac_addr[3],
							mstp_global_bridge->cist_br.root_priority.root_id.mac_addr[4],
							mstp_global_bridge->cist_br.root_priority.root_id.mac_addr[5], 
							mstp_global_bridge->cist_br.root_priority.external_root_path_cost,
							VTY_NEWLINE);	

	/*show mstp region root mac addr*/
	vty_out(vty,"%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x / %d%s",
							"CIST RegRoot/IRPC",
							((unsigned short)mstp_global_bridge->cist_br.root_priority.regional_root_id.pri[0] << 8 
							| (unsigned short)mstp_global_bridge->cist_br.root_priority.regional_root_id.pri[1]),
							mstp_global_bridge->cist_br.root_priority.regional_root_id.mac_addr[0],
							mstp_global_bridge->cist_br.root_priority.regional_root_id.mac_addr[1],
							mstp_global_bridge->cist_br.root_priority.regional_root_id.mac_addr[2],
							mstp_global_bridge->cist_br.root_priority.regional_root_id.mac_addr[3],
							mstp_global_bridge->cist_br.root_priority.regional_root_id.mac_addr[4],
							mstp_global_bridge->cist_br.root_priority.regional_root_id.mac_addr[5], 
							mstp_global_bridge->cist_br.root_priority.internal_root_path_cost,
							VTY_NEWLINE);	
	
	/*priority*/
	vty_out(vty, "%-30s: %u %s", "Bridge Priority", mstp_global_bridge->br_priority, VTY_NEWLINE);
	/*bridge root role*/
	vty_out(vty, "%-30s: %s %s",
		"Bridge Root Role",
		(br_root_role[mstp_global_bridge->bridge_root_role].port_status_str)
		, VTY_NEWLINE);

	/*time value*/
	vty_out(vty, "%-30s: %ds %s", "Hello Time", mstp_global_bridge->hello_time, VTY_NEWLINE);
	vty_out(vty, "%-30s: %ds %s", "Foward Delay", mstp_global_bridge->fwd_delay, VTY_NEWLINE);
	vty_out(vty, "%-30s: %ds %s", "Message Max Age", mstp_global_bridge->msg_max_age, VTY_NEWLINE);
	vty_out(vty, "%-30s: %d %s", "Message Max Hop Count", mstp_global_bridge->msg_max_hop_count, VTY_NEWLINE);


	return;
}


/*display interface info*/
void mstp_show_if(struct vty *vty, struct mstp_port *mstp_port)
{
	unsigned int				instance_id = 0;
	struct mstp_common_port		*common_info = NULL;

	if ((NULL == mstp_port) || (NULL == mstp_global_bridge))
	{
		return;
	}

	/*mstp port config(means cist port)*/
	mstp_show_port_common(vty, mstp_port);

	if(PROTO_VERID_MSTP == mstp_global_bridge->stp_mode)
	{
		for(instance_id = 0; instance_id < MSTP_INSTANCE_MAX; instance_id++)
		{
			if(NULL == mstp_port->msti_port[instance_id])
			{
				continue;
			}

			mstp_show_msti_common(vty, mstp_port, instance_id);

			common_info = &(mstp_port->msti_port[instance_id]->common_info);
			vty_out(vty, "%s", VTY_NEWLINE);
			/*example:		----[Port(Ethernet0/0/1)][FORWARDING]----*/ 	
			vty_out(vty, "-------[MSTI(%u) Port(%s)] status[%s]-------%s", instance_id+1, mstp_port->if_name, port_status[common_info->sm_state_trans_state].port_status_str, VTY_NEWLINE);

			/*Port Role :Designated Port*/
			vty_out(vty, "%-30s: %s %s", "Port Role", port_role[common_info->selectedRole].port_role_str, VTY_NEWLINE);

			/*Port priority*/
			vty_out(vty, "%-30s: %u %s", "Port Priority", mstp_port->msti_port_pri[instance_id], VTY_NEWLINE);

			/*Port cost*/
			vty_out(vty, "%-30s: %u %s", "Port Cost", mstp_port->msti_port_path_cost[instance_id], VTY_NEWLINE);

			/*Designated Bridge*/
#if 0
			vty_out(vty, "Designated Bridge\t\t: %d.%02x:%02x:%02x:%02x:%02x:%02x  %s",
					(unsigned short)mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.pri[0] << 8
					| (unsigned short)mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.pri[1],
					mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[0],
					mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[1],
					mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[2],
					mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[3],
					mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[4],
					mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[5],
					VTY_NEWLINE);
#else
			/*need to modify*/
			if((MSTP_ST_DISCARDING == common_info->sm_state_trans_state) && (PORT_LINK_DOWN == mstp_port->link_status))
			{
				/*Designated Bridge*/
				vty_out(vty, "%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x  %s",
						"Designated Bridge",
						(unsigned short)mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.pri[0] << 8
						| ((unsigned short)mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.pri[1] & 0x00),
						mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[0],
						mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[1],
						mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[2],
						mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[3],
						mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[4],
						mstp_port->msti_port[instance_id]->designated_priority.designated_bridge_id.mac_addr[5],
						VTY_NEWLINE);

				/*Port Times: RemHops 20*/
				vty_out(vty, "%-30s: RemHops %u %s", "Port Times", mstp_global_bridge->msti_br[instance_id]->bridge_times.remaining_hops, VTY_NEWLINE);	
			}
			else
			{
				vty_out(vty, "%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x  %s",
						"Designated Bridge",
						(unsigned short)mstp_port->msti_port[instance_id]->port_priority.designated_bridge_id.pri[0] << 8
						| ((unsigned short)mstp_port->msti_port[instance_id]->port_priority.designated_bridge_id.pri[1] & 0x00),
						mstp_port->msti_port[instance_id]->port_priority.designated_bridge_id.mac_addr[0],
						mstp_port->msti_port[instance_id]->port_priority.designated_bridge_id.mac_addr[1],
						mstp_port->msti_port[instance_id]->port_priority.designated_bridge_id.mac_addr[2],
						mstp_port->msti_port[instance_id]->port_priority.designated_bridge_id.mac_addr[3],
						mstp_port->msti_port[instance_id]->port_priority.designated_bridge_id.mac_addr[4],
						mstp_port->msti_port[instance_id]->port_priority.designated_bridge_id.mac_addr[5],
						VTY_NEWLINE);

				/*Port Times: RemHops 20*/
				vty_out(vty, "%-30s: RemHops %u %s", "Port Times", mstp_port->msti_port[instance_id]->port_times.remaining_hops, VTY_NEWLINE);
			}
#endif

			/*Port Times: RemHops 20*/
			vty_out(vty, "%s", VTY_NEWLINE);
			vty_out(vty, "%s", VTY_NEWLINE);
		}
	}

	return;
}

void mstp_show_all_cist_if(struct vty *vty)
{
	int						cursor = 0, flag = 0;
	struct l2if				*pif = NULL;
	struct mstp_port		*mstp_port = NULL;
	struct hash_bucket		*pbucket = NULL;


	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}
		
		mstp_port = pif->mstp_port_info;

		if(mstp_port != NULL)
		{
			mstp_show_port_common(vty, mstp_port);
			flag = 1;
		}
	}

	if(0 == flag)
	{	
		vty_out(vty,"%s", VTY_NEWLINE); 
		vty_out(vty,"Info: No MSTP info, Maybe No Interface enable MSTP. %s", VTY_NEWLINE); 
	}

	return;
}


void mstp_show_all_if(struct vty *vty)
{
	int						cursor = 0, flag = 0;
	struct l2if				*pif = NULL;
	struct mstp_port		*mstp_port = NULL;
	struct hash_bucket		*pbucket = NULL;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}
		
		mstp_port = pif->mstp_port_info;

		if(mstp_port != NULL)
		{
			mstp_show_if(vty, mstp_port);
			flag = 1;
		}
	}

	if(0 == flag)
	{	
		vty_out(vty,"%s", VTY_NEWLINE); 
		vty_out(vty,"Info: No MSTP info, Maybe No Interface enable MSTP. %s", VTY_NEWLINE); 
	}

	return;
}


void mstp_show_instance_cfg(struct vty *vty, unsigned int instance_id)
{
	char						vlan_str[256] = {0};

	if (NULL == mstp_global_bridge)
	{
		return;
	}

	/*get msti vlan bit map*/
	mstp_vlan_str_get_by_bitmap(mstp_global_bridge->mstp_instance[instance_id-1].msti_vlan.vlan_map, vlan_str);

	/*display msti config*/
	vty_out(vty, "%-30s: %s %s", "Vlan Map", vlan_str, VTY_NEWLINE);
	vty_out(vty, "%-30s: %u %s", "Instance Bridge Priority", mstp_global_bridge->mstp_instance[instance_id-1].msti_br_priority, VTY_NEWLINE);

	if(mstp_global_bridge->msti_br[instance_id-1] != NULL)
	{
		/*designed bridge id*/
		vty_out(vty, "%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x %s",
				"MSTI Bridge ID",
				(unsigned short)mstp_global_bridge->msti_br[instance_id-1]->bridge_priority.designated_bridge_id.pri[0] << 8
				| ((unsigned short)mstp_global_bridge->msti_br[instance_id-1]->bridge_priority.designated_bridge_id.pri[1] & 0x00),
				mstp_global_bridge->msti_br[instance_id-1]->bridge_priority.designated_bridge_id.mac_addr[0],
				mstp_global_bridge->msti_br[instance_id-1]->bridge_priority.designated_bridge_id.mac_addr[1],
				mstp_global_bridge->msti_br[instance_id-1]->bridge_priority.designated_bridge_id.mac_addr[2],
				mstp_global_bridge->msti_br[instance_id-1]->bridge_priority.designated_bridge_id.mac_addr[3],
				mstp_global_bridge->msti_br[instance_id-1]->bridge_priority.designated_bridge_id.mac_addr[4],
				mstp_global_bridge->msti_br[instance_id-1]->bridge_priority.designated_bridge_id.mac_addr[5],
				VTY_NEWLINE);
		
		/*region root id*/
		vty_out(vty, "%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x / %d%s",
				"MSTI RegRoot/IRPC",
				(unsigned short)mstp_global_bridge->msti_br[instance_id-1]->root_priority.rg_root_id.pri[0] << 8
				| ((unsigned short)mstp_global_bridge->msti_br[instance_id-1]->root_priority.rg_root_id.pri[1] & 0x00),
				mstp_global_bridge->msti_br[instance_id-1]->root_priority.rg_root_id.mac_addr[0],
				mstp_global_bridge->msti_br[instance_id-1]->root_priority.rg_root_id.mac_addr[1],
				mstp_global_bridge->msti_br[instance_id-1]->root_priority.rg_root_id.mac_addr[2],
				mstp_global_bridge->msti_br[instance_id-1]->root_priority.rg_root_id.mac_addr[3],
				mstp_global_bridge->msti_br[instance_id-1]->root_priority.rg_root_id.mac_addr[4],
				mstp_global_bridge->msti_br[instance_id-1]->root_priority.rg_root_id.mac_addr[5],
				mstp_global_bridge->msti_br[instance_id-1]->root_priority.internal_root_path_cost,
				VTY_NEWLINE);
	}

	return;
}


void mstp_show_msti_port(struct vty *vty, unsigned int instance_id, struct mstp_msti_port *msti_port)
{
	struct mstp_common_port		*common_info = NULL;

	if((NULL == msti_port) || (NULL == mstp_global_bridge))
	{
		return;
	}

	common_info = &(msti_port->common_info);

	vty_out(vty, "%s", VTY_NEWLINE);

	/*example:		----[MSTI Port(Ethernet 1/1/1)][FORWARDING]----*/ 	
	vty_out(vty, "----------[MSTI Port(%s)] status[%s]----------%s",
			msti_port->mport->if_name,
			port_status[common_info->sm_state_trans_state].port_status_str, VTY_NEWLINE);
	
	/*Port Role :Designated Port*/
	vty_out(vty, "%-30s: %s %s",
			"Port Role",
			port_role[common_info->selectedRole].port_role_str,
			VTY_NEWLINE);

	/*MSTI Port Priority*/
	vty_out(vty, "%-30s: %u %s",
			"Port Priority",
			msti_port->mport->msti_port_pri[instance_id-1],
			VTY_NEWLINE);

	/*MSTI Port Cost*/
	vty_out(vty, "%-30s: %u %s",
			"Port Cost",
			msti_port->common_info.port_cost,
			VTY_NEWLINE);
	


	/*need to modify*/
	if((MSTP_ST_DISCARDING == common_info->sm_state_trans_state) && (PORT_LINK_DOWN == msti_port->mport->link_status))
	{
		/*Designated Bridge*/
		vty_out(vty, "%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x  %s",
				"Designated Bridge",
				(unsigned short)msti_port->designated_priority.designated_bridge_id.pri[0] << 8
				| ((unsigned short)msti_port->designated_priority.designated_bridge_id.pri[1] & 0x00),
				msti_port->designated_priority.designated_bridge_id.mac_addr[0],
				msti_port->designated_priority.designated_bridge_id.mac_addr[1],
				msti_port->designated_priority.designated_bridge_id.mac_addr[2],
				msti_port->designated_priority.designated_bridge_id.mac_addr[3],
				msti_port->designated_priority.designated_bridge_id.mac_addr[4],
				msti_port->designated_priority.designated_bridge_id.mac_addr[5],
				VTY_NEWLINE);

		/*Port Times: RemHops 20*/
		vty_out(vty, "%-30s: RemHops %u %s", "Port Times", mstp_global_bridge->msti_br[instance_id-1]->bridge_times.remaining_hops, VTY_NEWLINE);	
	}
	else
	{
		vty_out(vty, "%-30s: %u.%02x:%02x:%02x:%02x:%02x:%02x  %s",
				"Designated Bridge",
				(unsigned short)msti_port->port_priority.designated_bridge_id.pri[0] << 8
				| ((unsigned short)msti_port->port_priority.designated_bridge_id.pri[1] & 0x00),
				msti_port->port_priority.designated_bridge_id.mac_addr[0],
				msti_port->port_priority.designated_bridge_id.mac_addr[1],
				msti_port->port_priority.designated_bridge_id.mac_addr[2],
				msti_port->port_priority.designated_bridge_id.mac_addr[3],
				msti_port->port_priority.designated_bridge_id.mac_addr[4],
				msti_port->port_priority.designated_bridge_id.mac_addr[5],
				VTY_NEWLINE);

		/*Port Times: RemHops 20*/
		vty_out(vty, "%-30s: RemHops %u %s", "Port Times", msti_port->port_times.remaining_hops, VTY_NEWLINE);
	}

	vty_out(vty, "%s", VTY_NEWLINE);

	return;
}


/*debug mstp interface running info*/
void mstp_debug_if(struct vty *vty, struct mstp_port *mstp_port)
{

	if ((NULL == mstp_port) || (NULL == mstp_global_bridge))
	{
		return;
	}

	vty_out(vty, "interface %s, ifindex: %u%s", mstp_port->if_name, mstp_port->ifindex, VTY_NEWLINE);
	vty_out(vty, "Link status: %s%s", mstp_port->link_status ? "up" : "down", VTY_NEWLINE);

	switch(mstp_port->stp_mode)
	{
		case PROTO_VERID_STP:
		{
			vty_out(vty, "STP mode: stp%s", VTY_NEWLINE);
		}
		break;
		case PROTO_VERID_RSTP:
		{
			vty_out(vty, "STP mode: rstp%s", VTY_NEWLINE);
		}
			break;
		case PROTO_VERID_MSTP:
		{
			vty_out(vty, "STP mode: mstp%s", VTY_NEWLINE);
		}
			break;
		default:
			break;
	}

	vty_out(vty, "Admin status: %s%s", mstp_port->adminStatus ? "enable" : "disable", VTY_NEWLINE);

	vty_out(vty, "Port priority: %u%s", mstp_port->port_pri, VTY_NEWLINE);

	return;
}


void mstp_debug_all_if(struct vty *vty)
{
	int						cursor = 0, flag = 0;
	struct l2if				*pif = NULL;
	struct mstp_port		*mstp_port = NULL;
	struct hash_bucket		*pbucket = NULL;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;
		if(NULL == pif)
		{
			continue;
		}
		
		mstp_port = pif->mstp_port_info;

		if(mstp_port != NULL)
		{
			mstp_debug_if(vty, mstp_port);
			flag = 1;
		}
	}

	if(0 == flag)
	{	
		vty_out(vty,"%s", VTY_NEWLINE); 
		vty_out(vty,"Info: No MSTP info, Maybe No Interface enable MSTP. %s", VTY_NEWLINE); 
	}

	return;
}


