
#ifndef HIOS_ELPS_FSM_H
#define HIOS_ELPS_FSM_H

struct cfm_sess *elps_find_cfm(uint32_t ifindex,uint16_t sess_id);
int elps_init_session (struct elps_sess  *psess);
int elps_local_handle_lop ( struct elps_sess  *psess);
int elps_local_handle_fs ( struct elps_sess  *psess);
int elps_local_handle_ms ( struct elps_sess  *psess);
int elps_local_handle_sf( struct elps_sess *psess);
int elps_local_handle_fsclear ( struct elps_sess *psess);
int elps_local_handle_msclear ( struct elps_sess *psess);
int elps_local_handle_lopclear ( struct elps_sess *psess);
int elps_local_handle_rsf ( struct elps_sess *psess);
int elps_local_handle_wtr_timeout(struct elps_sess * psess);

int elps_aps_change_to_nrp(struct elps_sess * psess);
int elps_aps_change_to_nrw(struct elps_sess * psess);
int elps_aps_handle_nr(struct elps_sess * psess,struct aps_pkt *aps_pdu);
int elps_aps_handle_dnr(struct elps_sess *psess);

int elps_link_down_handler (struct elps_sess *psess);




#endif

