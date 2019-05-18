/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_init.h	
*
*  date: 2016.8.2
*
*  modify:
*
*/

#ifndef _LLDP_INIT_H_
#define _LLDP_INIT_H_


#include "lldp.h"


void lldp_g_cfg_init(void);
void lldp_init_port_rx(struct lldp_port *lldp_port);
void lldp_init_port_tx(struct lldp_port *lldp_port);


#endif

