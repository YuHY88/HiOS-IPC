#ifndef HIOS_ELPS_TIMER_H
#define HIOS_ELPS_TIMER_H


int elps_wtr_expiry (void *thread);
void elps_start_wtr_timer (struct elps_sess *psess);
void elps_stop_wtr_timer (struct elps_sess *psess);
	

int elps_holdoff_expiry (void *thread);
void elps_start_holdoff_timer  (struct elps_sess *psess);

int elps_msg_expiry (void *thread);
void elps_start_msg_timer (struct elps_sess *psess);
void elps_stop_msg_timer (struct elps_sess *psess);

int elps_dfop_expiry (void *thread);
void elps_start_dfop_timer (struct elps_sess *psess);
void elps_stop_dfop_timer (struct elps_sess *psess);
int elps_clear_timer(struct elps_sess *psess);


#endif
