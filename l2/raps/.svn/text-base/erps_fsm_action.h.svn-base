#ifndef HIOS_ERPS_FSM_ACTION_H
#define HIOS_ERPS_FSM_ACTION_H

#include "raps/erps.h"


int erps_node_id_higher(u_int8_t aps_node[], u_int8_t session_node[]);
int erps_init_handle_initialization(struct erps_sess *psess);
int erps_idlems_handle_raps_sf(struct erps_sess *psess);
int erps_idle_handle_raps_ms(struct erps_sess *psess);
int erps_idle_handle_admin_ms(struct erps_sess *psess);
int erps_idle_handle_raps_nrrb(struct erps_sess *psess);
int erps_idle_handle_raps_nr(struct erps_sess *psess, struct raps_pkt *raps_pdu);
int erps_protection_handle_clear_sf(struct erps_sess *psess);
int erps_ms_handle_raps_ms(struct erps_sess *psess);
int erps_fsms_handle_admin_clear(struct erps_sess *psess);
int erps_fs_handle_admin_fs(struct erps_sess *psess);
int erps_pending_handle_admin_clear(struct erps_sess *psess);
int erps_pending_handle_admin_fs(struct erps_sess *psess);

int erps_pending_handle_raps_fs(struct erps_sess *psess);
int erps_pending_handle_local_sf(struct erps_sess *psess);
int erps_pending_handle_raps_sf(struct erps_sess *psess);
int erps_pending_handle_raps_ms(struct erps_sess *psess);
int erps_pending_handle_admin_ms(struct erps_sess *psess);
int erps_pending_handle_wtre(struct erps_sess *psess);
int erps_pending_handle_wtbe(struct erps_sess *psess);
int erps_pending_handle_nrrb(struct erps_sess *psess);
int erps_pending_handle_nr(struct erps_sess *psess, struct raps_pkt *raps_pdu);
int erps_x_handle_admin_fs(struct erps_sess *psess);
int erps_x_handle_raps_fs(struct erps_sess *psess);
int erps_x_handle_local_sf(struct erps_sess *psess);
int erps_x_handle_raps_nr(struct erps_sess *psess);
void erps_link_down_handler(struct erps_sess *psess);

#endif

