
/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_show.h
*
*  date: 2017.4
*
*  modify:
*
*/

#ifndef _MSTP_SHOW_H_
#define _MSTP_SHOW_H_

#include "mstp.h"
#include "mstp_bpdu.h"
#include "mstp_port.h"

unsigned int mstp_vlan_str_get_by_bitmap(unsigned char *bitmap, char *str);

/*display instance info*/
void mstp_show_port_common(struct vty *vty, struct mstp_port *mstp_port);
void mstp_show_msti_common(struct vty *vty, struct mstp_port *mstp_port, unsigned int instance_id);

void mstp_show_common(struct vty *vty);

/*display interface info*/
void mstp_show_all_cist_if(struct vty *vty);
void mstp_show_if(struct vty *vty, struct mstp_port *mstp_port);
void mstp_show_all_if(struct vty *vty);
void mstp_show_instance_cfg(struct vty *vty, unsigned int instance_id);
void mstp_show_msti_port(struct vty *vty, unsigned int instance_id, struct mstp_msti_port *msti_port);

void mstp_debug_all_if(struct vty *vty);
void mstp_debug_if(struct vty *vty, struct mstp_port *mstp_port);
#endif





