/*
*  Copyright (C) 2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_api.h
*
*  date: 2016.8.5-2017.1
*
*  modify:	2018.3.10 modified by liufuying to make LLDP module code beautiful
*
*/

/*used for SNMP*/


#ifndef _LLDP_API_H_
#define _LLDP_API_H_

#include "lib/msg_ipc.h"

struct lldp_port_config_snmp
{
	uint32_t ifindex;
	uint32_t adminStatus;
	uint32_t tlv_tx_enable;
};

struct lldp_stats_txrx_port_snmp
{
	uint32_t ifindex;
	unsigned long long tx_statsFramesOutTotal;
	unsigned long long	rx_statsTLVsDiscardedTotal;
	unsigned long long	rx_statsFramesInErrorsTotal;
	unsigned long long	rx_statsFramesInTotal;
	unsigned long long	rx_statsFramesDiscardedTotal;
	unsigned long long	rx_statsTLVsUnrecognizedTotal;
	unsigned long long	rx_statsAgeoutsTotal;
};

struct lldp_loc_port_snmp
{
	uint32_t ifindex;
	char	 if_name[INTERFACE_NAMSIZ];	/*The interface name*/
	char alias[IFM_ALIAS_STRING_LEN+1];
};

struct lldp_loc_port_priv_snmp
{
	uint32_t ifindex;
	uint32_t msap_num;
	char	 if_name[INTERFACE_NAMSIZ]; /*The interface name*/
	unsigned char source_mac[MAC_LEN];
	uint32_t arp_nd_learning;
	uint32_t arp_nd_learning_vlan;
};

struct lldp_msap_info_data
{
	uint8_t		msap_index;
	uint8_t 		chassis_id_subtype;
	uint8_t 		chassis_id_addr[6]; /*max length is 6*/

	uint8_t 		port_id_subtype;
	uint8_t 		port_id[INTERFACE_NAMSIZ];/*max length is 32*/
	uint8_t 		port_descr[IFM_ALIAS_STRING_LEN+1];

	uint8_t 		sys_name[MAX_SIZE];
	uint8_t 		sys_descr[SYSTEM_DESCR_MAX_SIZE];

	uint16_t	sys_cap;
	uint16_t	sys_cap_enabled;	 

	uint16_t	svlan_tpid;		
	uint16_t	svlan;					
	uint16_t	cvlan_tpid;
	uint16_t	cvlan;		
	uint16_t	rx_ttl;
	unsigned char			mgmt_addr_sub_type;
	unsigned char			mgmt_addr[31];	
};

struct lldp_msap_snmp
{
	uint32_t ifindex;
	struct lldp_msap_info_data lldp_msap_info;
};

struct lldp_msap_man_addr_info_data
{
	uint8_t		msap_index;
	unsigned char			mgmt_addr_sub_type;
	unsigned char			mgmt_addr[31];	

	time_t time_mark[TIME_MARK_INDEX_MAX];
	uint8_t time_mark_index;
	uint8_t if_numbering;
	uint32_t if_number;
};

struct lldp_msap_man_addr_snmp
{
	uint32_t ifindex;
	struct lldp_msap_man_addr_info_data lldp_msap_man_addr_info;
};

unsigned int lldp_snmp_global_cfg_get(struct ipc_msghdr_n *phdr);
unsigned int lldp_snmp_port_cfg_get(struct ipc_msghdr_n *phdr);
uint32_t lldp_snmp_port_cnofig_info_get(uint32_t ifindex, 
																		struct lldp_port_config_snmp lldp_port_config_mux[]);
void lldp_send_port_config_to_snmp(struct ipc_msghdr_n * phdr, 
									void * pdata, uint32_t data_num, uint32_t ifindex);
void lldp_snmp_port_config_get(struct ipc_msghdr_n *phdr);
uint32_t lldp_snmp_stats_txrx_port_info_get(uint32_t ifindex, 
																		struct lldp_stats_txrx_port_snmp lldp_stats_txrx_port_mux[]);
void lldp_send_stats_txrx_port_to_snmp(struct ipc_msghdr_n * phdr, 
									void * pdata, uint32_t data_num, uint32_t ifindex);
void lldp_snmp_stats_txrx_port_get(struct ipc_msghdr_n *phdr);

uint32_t lldp_snmp_loc_port_info_get(uint32_t ifindex, 
																		struct lldp_loc_port_snmp lldp_loc_port_mux[]);
void lldp_send_loc_port_to_snmp(struct ipc_msghdr_n * phdr, 
									void * pdata, uint32_t data_num, uint32_t ifindex);
void lldp_snmp_loc_port_get(struct ipc_msghdr_n *phdr);

uint32_t lldp_snmp_loc_port_priv_info_get(uint32_t ifindex, 
																		struct lldp_loc_port_priv_snmp lldp_loc_port_priv_mux[]);
void lldp_send_loc_port_priv_to_snmp(struct ipc_msghdr_n * phdr, 
									void * pdata, uint32_t data_num, uint32_t ifindex);
void lldp_snmp_loc_port_priv_get(struct ipc_msghdr_n *phdr);

void lldp_snmp_port_msap_get(struct ipc_msghdr_n *phdr, uint8_t msap_index);
void lldp_snmp_port_msap_man_addr_get(struct ipc_msghdr_n *phdr, uint8_t msap_index);
uint32_t lldp_snmp_msap_info_get(uint32_t ifindex, uint32_t msap_index,struct lldp_msap_snmp lldp_msap_mux[]);
uint32_t lldp_snmp_msap_man_addr_info_get(uint32_t ifindex, uint32_t msap_index,
																				struct lldp_msap_man_addr_snmp lldp_msap_mux[]);
void lldp_send_msap_to_snmp(struct ipc_msghdr_n * phdr, void * pdata, uint32_t data_num, uint32_t ifindex);
void lldp_send_msap_man_addr_to_snmp(struct ipc_msghdr_n * phdr, void * pdata, 
																							uint32_t data_num, uint32_t ifindex);
void lldp_get_msap_info(struct lldp_msap_info_data *msap_info ,
															struct lldp_msap *msap);
void lldp_get_msap_man_addr_info(struct lldp_msap_man_addr_info_data *msap_info ,
															struct lldp_msap *msap);
int lldp_trap_insert_msap_to_snmpd(struct lldp_port *lldp_port, struct lldp_msap *msap_msg);
void lldp_add_delete_msap_info(struct lldp_msap *msap_msg, unsigned int msap_num);
int lldp_trap_delete_msap_to_snmpd(unsigned int	 trap_info_len);
int lldp_trap_ageout_msap_to_snmpd(unsigned int trap_info_len);

int lldp_arp_learn_cfg_set(uint32_t ifindex , int arp_learnable);
int
lldp_snmp_get_tlvs (struct lldp_port *lldp_port_info);
int lldp_arp_learn_cfg_single_if_set(uint32_t ifindex , int arp_learnable);
int lldp_arp_learn_cfg_status_get() ;

#endif
