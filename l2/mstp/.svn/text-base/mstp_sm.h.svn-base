
/*
*  Copyright (C) 2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_sm.h	(used for mstp state machine)
*
*  date: 2017.3
*
*  modify:
*
*/

#ifndef _MSTP_SM_H_
#define _MSTP_SM_H_

#include "mstp_port.h"

void mstp_port_timer_sm(struct mstp_port *port);
void mstp_port_recv_sm(struct mstp_port *port);
void mstp_port_protocol_migration_sm(struct mstp_port *port);
void mstp_port_info_sm(struct mstp_port *port, unsigned int	instance_id);
void mstp_port_tx_sm(struct mstp_port *port);
void mstp_port_role_selection_sm(struct mstp_bridge *mstp_bridge, unsigned int instance_id);
void mstp_port_role_trans_disabled_sm(struct mstp_port* port, unsigned int instance_id);
void mstp_port_role_trans_alternate_backup_sm(struct mstp_port* port, unsigned int instance_id);
void mstp_port_role_trans_root_sm(struct mstp_port* port, unsigned int instance_id);
void mstp_port_role_trans_designated_sm(struct mstp_port* port, unsigned int instance_id);
void mstp_port_role_trans_master_sm(struct mstp_port* port, unsigned int instance_id);
void mstp_port_role_trans_sm(struct mstp_port* port, unsigned int instance_id);
void mstp_port_state_trans_sm(struct mstp_port *port, unsigned int instance_id);
void mstp_port_Topology_change_sm(struct mstp_port *port, unsigned int instance_id);
void mstp_br_detection_sm(struct mstp_port *port);
void mstp_rcv_sm_run(struct mstp_port* port);
void mstp_state_machine_normal_action(struct mstp_bridge *mstp_bridge);
void mstp_state_machine_one_run(struct mstp_port *port);
/*modify: mstp_state_machine_two_run		change	(mstp_port_msti_state_machine_run)*/
void mstp_state_machine_two_run(struct mstp_bridge *mstp_bridge);
void mstp_state_machine_three_run(struct mstp_port *port);
void mstp_link_change_sm_action(struct mstp_port* mstp_port);


#endif

