/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_show.h		(lldp msg show)
*
*  date: 2016.8.3
*
*  modify:
*
*/
#ifndef _LLDP_SHOW_H_
#define _LLDP_SHOW_H_

#include "lldp.h"


#define 			LLDP_SYS_CAP_MAX		11
#define 			LLDP_SYS_CAP_STR_LEN	50




#if 0
struct lldp_protocol_identity *lldp_get_protocol_id(enum lldp_protocol proto);
#endif

void lldp_show_if_neighbor(struct vty *vty, struct lldp_port *lldp_port);
void lldp_show_if_statistics(struct vty *vty, struct lldp_port *lldp_port);
void lldp_show_neighbor(struct vty *vty);
void lldp_show_statistics(struct vty *vty);
void lldp_show_if_local(struct vty *vty, struct lldp_port *lldp_port, struct l2if *pif);
void lldp_show_local(struct vty *vty);
void lldp_show_if_tlv_cfg(struct vty *vty, struct lldp_port *lldp_port);
void lldp_show_tlv_cfg(struct vty *vty);



#endif

