/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_api.h
*
*  date: 2017.3
*
*  modify:
*
*/
	
/*used for SNMP*/

#ifndef _MSTP_API_H_
#define _MSTP_API_H_

#include "lib/types.h"
#include "mstp.h"

#define BRIDGE_ID 8 

struct mstp_global_config_snmp
{

	uint8_t global_region_name[NAME_LEN];
	uint16_t global_revision_level;
	uint8_t global_vlan_Map[STRING_LEN];
	uint8_t global_mode;
	uint8_t global_bridge_id[BRIDGE_ID];
	uint8_t global_root_bridge_id[BRIDGE_ID];
	uint32_t global_extern_root_path_cost;
	uint8_t global_region_root_bridge_id[BRIDGE_ID];
	uint32_t global_intern_root_path_cost;
	uint8_t global_bridge_role;
	uint16_t global_bridge_priority;
	uint8_t global_hello_time;
	uint8_t global_foward_delay;
	uint8_t global_max_age;
	uint8_t global_max_hop_count;
};

struct mstp_port_info_snmp
{
	uint32_t ifindex;
	uint8_t port_protocol_state;
	uint8_t port_state;
	uint8_t port_mode;
	uint8_t port_designated_bridge_id[BRIDGE_ID];
	uint8_t port_role;
	uint8_t port_priority;
	uint32_t port_path_cost;
	uint16_t port_message;
	uint8_t port_edge_state;
	uint8_t port_filter_state;
	uint8_t port_p2p;
};

struct mstp_instance_snmp
{
	uint32_t instance_id;
	uint8_t instance_vlan_map[STRING_LEN];
	uint16_t instance_bridge_priority;
	uint8_t instance_bridge_id[BRIDGE_ID];
	uint8_t instance_region_root_bridge_id[BRIDGE_ID];
	uint32_t instance_internal_root_path_cost;
};

struct mstp_instance_port_snmp
{
	uint32_t instance_id;
	uint32_t instance_port_ifindex;
	uint8_t instance_port_state;
	uint8_t instance_port_role;
	uint8_t instance_port_priority;
	uint32_t instance_port_path_cost;
	uint8_t instance_port_message;
	uint8_t instance_port_designal_bridge_id[BRIDGE_ID];
	uint8_t instance_port_max_hop;
	uint8_t instance_port_edge_state;
 	uint8_t instance_port_filter_state;
	uint8_t instance_port_p2p;

};
struct mstp_port_statistics_snmp
{
	uint32_t ifindex;
	uint32_t port_tc_send;
	uint32_t port_tc_recv;
	uint32_t port_tcn_send;
	uint32_t port_tcn_recv;
	uint32_t port_bpdu_send;
	uint32_t port_bpdu_recv;
	uint32_t port_config_send;
	uint32_t port_config_recv;
	uint32_t port_rstp_send;
	uint32_t port_rstp_recv;
	uint32_t port_mstp_send;
	uint32_t port_mstp_recv;
};
enum mstp_snmp_info_type
{
		MSTP_GLOBAL_CONFIG_TYPE,
		MSTP_PORT_INFO_TYPE,
		MSTP_INSTANCE_TYPE,
		MSTP_INSTANCE_PORT_TYPE,
		MSTP_PORT_STATISTICS_TYPE,
};
void snmp_msg_rcv_mstp(struct ipc_msghdr_n * phdr);
int mstp_snmp_port_info_get(uint32_t ifindex, struct mstp_port_info_snmp  mstp_snmp_mux[]);
void mstp_port_info_get(struct mstp_port_info_snmp  * mstp_port_snmp ,struct mstp_port *mstp_port);
void snmp_msg_rcv_instance_port_info(struct ipc_msghdr_n *phdr,uint32_t instance_port_index);
int mstp_snmp_instance_get(uint32_t ifindex, struct mstp_instance_snmp  mstp_snmp_mux[]);
void mstp_instance_get(struct mstp_instance_snmp  * mstp_instance_snmp,uint8_t instance_id);
int mstp_snmp_instance_port_get(uint32_t ifindex,uint32_t second_index, struct mstp_instance_port_snmp  mstp_snmp_mux[]);
void mstp_instance_port_get(struct mstp_instance_port_snmp  * mstp_snmp,uint32_t instance_id, struct mstp_msti_port *msti_port);
int mstp_snmp_port_statistics_get(uint32_t ifindex, struct mstp_port_statistics_snmp  mstp_snmp_mux[]);
void mstp_port_statistics_get(struct mstp_port_statistics_snmp  * mstp_snmp ,struct mstp_port *mstp_port);
void mstp_reply_snmp_request(void * pdata, int data_len, int data_num,struct ipc_msghdr_n * phdr);
uint8_t mstp_global_config_get(struct mstp_global_config_snmp * mstp_info);


#endif

