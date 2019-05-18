/*
*  Copyright (C) 2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_base_procedure.h	
*
*  date: 2017.3
*
*  modify:	2018.3.12 modified by liufuying to make mstp module code beautiful
*
*/

#ifndef _MSTP_BASE_PROCEDURE_H_
#define _MSTP_BASE_PROCEDURE_H_

#include "mstp.h"
#include "mstp_port.h"
#include "mstp_bpdu.h"
#include "msg_ipc.h"

void __list_add(struct list_head * new,struct list_head * prev,struct list_head * next);
void list_add_tail(struct list_head *new, struct list_head *head);
void __list_del(struct list_head * prev, struct list_head * next);
void list_del(struct list_head *entry);

int mstp_fwd_delay(struct mstp_port *port);
int mstp_edge_delay(struct mstp_port *port);
int mstp_pdu_older(struct mstp_port *port);
int mstp_forward_delay(struct mstp_port *port);
int mstp_rerooted(struct mstp_port *port, unsigned int instance_id);
int mstp_rstp_version(struct mstp_port *port);
int mstp_stp_version(struct mstp_port *port);
int mstp_all_synced(struct mstp_port *port, unsigned int instance_id);
int mstp_hello_time(struct mstp_port *port);
int mstp_max_age(struct mstp_port *port);
int mstp_all_transmit_ready(struct mstp_port *port);
int mstp_cist(unsigned int instance_id);
int mstp_cist_root_port(struct mstp_port *port);
int mstp_cist_designated_port(struct mstp_port *port);
int mstp_msti_designated_port(struct mstp_port *port);
int mstp_msti_master_port(struct mstp_port *port);
int mstp_rcvd_any_msg(struct mstp_port *port);
int mstp_rcvd_cist_msg(struct mstp_port *port);
int mstp_rcvd_msti_msg(struct mstp_port *port, unsigned int instance_id);
int mstp_restricted_role(struct mstp_port *port);
int mstp_restricted_tcn(struct mstp_port *port);
int mstp_updt_cist_info(struct mstp_port *port);
int mstp_updt_msti_info(struct mstp_port *port, unsigned int instance_id);
struct bridge_id mstp_get_br_id(unsigned short br_pri, unsigned int instance_id);
void mstp_mst_digest_recalc(void);
void mstp_update_port_status(struct mstp_port *port);
void mstp_change_vlan_map(unsigned char *new_map, unsigned int instance_id);
int mstp_vlan_map_get (unsigned char *map, unsigned short vlan);
void mstp_add_cist_port(struct mstp_port* mstp_port);
void mstp_vlan_map_set(unsigned char *map, unsigned short vlan);
void mstp_vlan_map_unset(unsigned char *map, unsigned short vlan);
unsigned int mstp_vlanid_belongto_vlanmap(unsigned char *vlan_map, unsigned short vlan_id);
unsigned int mstp_cmp_vlan_map(unsigned char *vlan_map, unsigned char *cvlan_map);
unsigned int mstp_cmp_vlan_map_same(unsigned char *vlan_map, unsigned char *cvlan_map);
void mstp_stp_vlan_set(unsigned short vid, unsigned int stg_id);
void mstp_del_cist_port(struct mstp_port* port);
void mstp_add_mport(struct mstp_port* mstp_port);
void mstp_del_mport(struct mstp_port* mstp_port);
void mstp_add_del_msti_port_by_instancedelete(struct mstp_port* mstp_port);
void mstp_add_del_msti_port_by_mportenable(struct mstp_port* mstp_port);
void mstp_add_delete_msti_port_by_instancevlan(unsigned int instance_id);
void mstp_port_vlan_map_get(unsigned int ifindex, struct mstp_vlan *vlan_map);
void mstp_send_bpdu(void);
void mstp_update_link_state(struct mstp_port* mstp_port);
int mstp_if_duplex_get (uint32_t msg_index,struct ifm_port *port_info);
void mstp_get_link_state(struct mstp_port* mstp_port, unsigned int if_index);
void mstp_trunk_if_speed_get(struct mstp_port * port);
unsigned short mstp_port_map(unsigned int ifindex, unsigned char port_pri);

#endif

