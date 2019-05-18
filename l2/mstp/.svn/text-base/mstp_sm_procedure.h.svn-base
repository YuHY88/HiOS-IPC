/*
*  Copyright (C) 2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_sm_procedure.h	
*
*  date: 2017.3
*
*  modify:
*
*/

#ifndef _MSTP_SM_PROCEDURE_H_
#define _MSTP_SM_PROCEDURE_H_

#include "pkt_buffer.h"

#include "l2_if.h"
#include "mstp_port.h"





void mstp_clear_all_recv_msg(struct mstp_port* mport);
void mstp_update_bpdu_version(struct mstp_port* port);
int mstp_recv_from_same_region(struct mstp_port* port);

void mstp_pkt_recv(struct l2if	*pif, struct pkt_buffer *pkt);
struct mstp_port *mstp_find_mstp_port(struct l2if *pif);
void mstp_decode_stp_bpdu(struct mstp_port *mstp_port, struct mstp_bpdu *bpdu);
void mstp_decode_tcn_bpdu(struct mstp_port *port, struct mstp_bpdu *bpdu);
void mstp_decode_rstp_bpdu(struct mstp_port *port, struct mstp_bpdu *bpdu);
void mstp_decode_mstp_bpdu(struct mstp_port *port, struct mstp_bpdu *bpdu, unsigned int pkt_len);
void mstp_decode_cist(struct mstp_port *port, struct mstp_bpdu *bpdu);
void mstp_decode_msti_cfg_msg(struct mstp_port *port, struct mstp_bpdu *bpdu, unsigned int pkt_len);
unsigned int mstp_get_ins_by_brId(struct bridge_id bridge_id);
struct bridge_id mstp_get_brId_by_ins(unsigned short br_pri, struct bridge_id br_mac, unsigned int instance_id);
void mstp_set_recv_msg(struct mstp_port* port);
void mstp_tx_config(struct mstp_port* port);
void mstp_tx_tcn(struct mstp_port* port);
void mstp_tx_mstp(struct mstp_port* port);
void mstp_send_frame(struct mstp_port* port);
unsigned int mstp_better_or_same_info(struct mstp_port* port, unsigned int instance_id, int newInfoIs);
unsigned int mstp_recv_info(struct mstp_port* port, unsigned int instance_id);
void mstp_record_proposal(struct mstp_port* port, unsigned int instance_id);
void mstp_record_mastered(struct mstp_port* port, unsigned int instance_id);
void mstp_record_agreement(struct mstp_port* port, unsigned int instance_id);
void mstp_record_priority(struct mstp_port* port, unsigned int instance_id);
void mstp_record_times(struct mstp_port* port, unsigned int instance_id);
void mstp_record_dispute(struct mstp_port* port, unsigned int instance_id);
void mstp_updt_rcvd_info_while(struct mstp_port* port, unsigned int instance_id);
void mstp_set_tc_flags(struct mstp_port* port, unsigned int instance_id);
void mstp_updt_role_disabled_tree(struct mstp_bridge *mstp_bridge, unsigned int instance_id);
void mstp_clear_reselect_tree(struct mstp_bridge *mstp_bridge, unsigned int instance_id);
void mstp_updt_roles_tree(struct mstp_bridge *mstp_bridge, unsigned int instance_id);
void mstp_set_selected_tree(struct mstp_bridge *mstp_bridge, unsigned int instance_id);
void mstp_set_sync_tree(struct mstp_port* port, unsigned int instance_id);
void mstp_set_reroot_tree(struct mstp_port* port, unsigned int instance_id);
void mstp_set_port_state(struct mstp_port* port, unsigned int instance_id, port_forword_state_in_instance state);
void mstp_disable_learning(struct mstp_port* port, unsigned int instance_id);
void mstp_disable_forwarding(struct mstp_port* port, unsigned int instance_id);
void mstp_enable_learning(struct mstp_port* port, unsigned int instance_id);
void mstp_enable_forwarding(struct mstp_port* port, unsigned int instance_id);
void mstp_flush(uint32_t instance_id,struct mstp_port* port, unsigned int age_time);
void mstp_new_tc_while(struct mstp_port* port, unsigned int instance_id);
#if 1
void mstp_set_tc_prop_tree(struct mstp_port* port, unsigned int instance_id);
#else
void mstp_set_tc_prop_bridge(struct mstp_port* port, unsigned int instance_id);//FIXME
#endif
int mstp_cist_info_cmp(struct cist_priority_vector *pv, struct cist_priority_vector *cpv);
int mstp_msti_info_cmp(struct msti_priority_vector *pv, struct msti_priority_vector *cpv);
int mstp_cist_times_cmp(struct cist_timer_vector *ct, struct cist_timer_vector *cct);
int mstp_msti_times_cmp(struct msti_timer_vector *mt, struct msti_timer_vector *cmt);
unsigned char mstp_is_same_br(struct bridge_id bridge_id);
void mstp_sync_master(struct mstp_bridge *mstp_bridge);
int mstp_aging_time(struct mstp_port *port);
unsigned char mstp_get_port_role(struct mstp_port *port, unsigned int instance_id);
unsigned char mstp_get_role_by_flag(unsigned char flag);
unsigned char mstp_same_root(struct bridge_id src, struct bridge_id dst);
void mstp_add_stg_vlan(unsigned int instance_id, struct mstp_port* port, port_forword_state_in_instance state);

void mstp_msti_port_decode_stp_bpdu(struct mstp_port *port, struct mstp_bpdu *bpdu);

#endif
