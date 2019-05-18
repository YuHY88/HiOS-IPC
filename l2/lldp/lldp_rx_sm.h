/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_rx_sm.h	
*
*  date: 2016.8.1
*
*  modify:
*
*/
	
#ifndef _LLDP_RX_SM_H_
#define _LLDP_RX_SM_H_

#include "lldp.h"


// States from the receive state machine diagram (IEEE 802.1AB 10.5.5.3)
#define LLDP_WAIT_PORT_OPERATIONAL			4
#define DELETE_AGED_INFO					5
#define RX_LLDP_INITIALIZE					6
#define RX_WAIT_FOR_FRAME					7
#define RX_FRAME							8
#define DELETE_INFO							9
#define UPDATE_INFO							10



void lldp_arp_info_notice(struct lldp_msap *msap_cache, uint16_t arp_learning_vlan,
															uint32_t ifindex, uint8_t opcode);
int lldp_rx_process_frm(struct lldp_port *lldp_port) ;
unsigned char lldp_delete_neighbor(struct lldp_port *lldp_port);
void lldp_rx_change_state(struct lldp_port *lldp_port, unsigned char state);
unsigned char lldp_rx_state_machine_run(struct lldp_port *lldp_port);
void lldp_rx_state_machine_action(struct lldp_port *lldp_port);
void lldp_rx_decrement_timer(unsigned short *timer);
void lldp_rx_timers(struct lldp_port *lldp_port);
void lldp_rx_delete_aged_info(struct lldp_port *lldp_port);
void lldp_rx_wait_for_frame(struct lldp_port *lldp_port);
void lldp_rx_deal_frame(struct lldp_port *lldp_port);
void lldp_rx_delete_info(struct lldp_port *lldp_port);

uint8_t
lldp_tlv_cmp_update (struct lldp_neighbor_msg old_neighbor_info, struct lldp_neighbor_msg new_neighbor_info);




#endif
