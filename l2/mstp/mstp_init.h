
/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_init.h
*
*  date: 2017.1.4
*
*  modify:
*
*/

#ifndef _MSTP_INIT_H_
#define _MSTP_INIT_H_

#include "mstp_port.h"
#include "mstp_cfg_digest.h"

void mstp_init(void);
int mstp_creat(void);
int mstp_get_mac_addr(void);
void mstp_bridge_init(struct mstp_bridge *mstp_br);
void mstp_create_cist_instance(void);
int mstp_instance_init(unsigned int instance_id);
void mstp_reinit(void);
void l2_stp_port_init(struct mstp_port *mstp_port, unsigned int if_index);


#endif
