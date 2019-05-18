
/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_msg.c	
*
*  date: 2016.8.11
*
*  modify:
*
*/

#ifndef _LLDP_MSG_H_
#define _LLDP_MSG_H_
#include <lib/pkt_buffer.h>
#include "l2/efm/efm.h"

#define ETH_HEAD_ADDR 0
#define LLDP_CH_TLV_ADDR 22
#define LLDP_PORT_TLV_ADDR 31
#define LLDP_TTL_TLV_ADDR(O) 34+(O)
#define LLDP_SYS_TLV_ADDR(O) 40+(O)



void lldp_pkt_register(void);
void lldp_pkt_rcv(struct l2if	*pif, struct pkt_buffer *pkt);
void lldp_pkt_rcv_efm_agent(uint32_t ifindex, efm_agent_info * aefm);



#endif
