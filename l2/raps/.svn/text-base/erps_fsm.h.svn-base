#ifndef HIOS_ERPS_FSM_H
#define HIOS_ERPS_FSM_H

#include "raps/erps.h"

struct cfm_sess *erps_find_cfm(uint32_t ifindex, uint16_t sess_id);
int  erps_init_session(struct erps_sess *psess);
int erps_state_update(uint32_t ifindex, uint16_t sess_id, enum ERPS_PORT_EVENT event);
int erps_fsm(struct erps_sess *psess, struct raps_pkt *raps_pdu);


#endif

