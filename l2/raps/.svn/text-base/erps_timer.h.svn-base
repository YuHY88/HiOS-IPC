#ifndef HIOS_ERPS_TIMER_H
#define HIOS_ERPS_TIMER_H

#include "raps/erps.h"

int erps_wtr_expiry(void *thread);
void erps_start_wtr_timer(struct erps_sess *psess);
void erps_stop_wtr_timer(struct erps_sess *psess);

int erps_wtb_expiry(void *thread);
void erps_start_wtb_timer(struct erps_sess *psess);
void erps_stop_wtb_timer(struct erps_sess *psess);

int erps_guard_expiry(void *thread);
void erps_start_guard_timer(struct erps_sess *psess);
void erps_stop_guard_timer(struct erps_sess *psess);

int erps_holdoff_expiry(void *thread);
void erps_start_holdoff_timer(struct erps_sess *psess);

int erps_msg_expiry(void *thread);
void erps_start_msg_timer(struct erps_sess *psess);
void erps_stop_msg_timer(struct erps_sess *psess);
int erps_clear_timer(struct erps_sess *psess);



#endif
