/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_tx_sm.c		(lldp transmit state machine)
*
*  date: 2016.7.29
*
*  modify:
*
*/


#ifndef _LLDP_TX_SM_H_
#define _LLDP_TX_SM_H_



#include "lldp.h"
#include "l2_if.h"

/*States from the transmit state machine diagram*/
#define TX_LLDP_INITIALIZE 0 
#define TX_IDLE            1 
#define TX_SHUTDOWN_FRAME  2 
#define TX_INFO_FRAME      3 

void lldp_get_management_ip(struct lldp_port *lldp_port);
void lldp_encap_frame(struct lldp_port *lldp_port, struct lldp_msap *msap_cache);
void lldp_encap_shutdown_frame(struct lldp_port *lldp_port, struct lldp_msap *msap_cache);

unsigned char lldp_tx_frame(struct lldp_port *lldp_port);
void lldp_tx_change_state(struct lldp_port *lldp_port, unsigned char state);
void lldp_tx_state_machine_run(struct lldp_port *lldp_port);
void lldp_tx_state_machine_action(struct lldp_port *lldp_port);
void lldp_tx_timers(struct lldp_port *lldp_port);
void lldp_send_shutdown_frame(struct lldp_port *lldp_port);
void lldp_send_frame(struct lldp_port *lldp_port);
void lldp_interface_admin_status_enable(struct l2if *pif);
void lldp_clean_port(struct lldp_port *clean_port);
void lldp_destroy_port(struct lldp_port *lldp_port);

#endif
