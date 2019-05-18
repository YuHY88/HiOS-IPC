#ifndef _RMON_TIMER_H_
#define _RMON_TIMER_H_

#include <time.h>
#include <sys/time.h>

#include "thread.h"
#include "log.h"
#include "rmond.h"

//int         rmon_coll_history_timer_handler(struct thread *t);
int         rmon_coll_history_timer_handler(void *t);

//int         rmon_alarm_timer_handler(struct thread *t);
int         rmon_alarm_timer_handler(void *t);

int         rmon_alarm_falling_threshold_handler(struct rmon_AlarmGroup *alarm,
        ut_int64_t zero_threshold,
        struct rmon_if_stats if_stats);

int         rmon_alarm_rising_threshold_handler(struct rmon_AlarmGroup *alarm,
        ut_int64_t zero_threshold,
        struct rmon_if_stats if_stats);

u_int32_t   rmon_time_since_boot();

void        rmon_trap(struct rmon_AlarmGroup *alarm, u_int32_t rise_or_fall);

#endif /*_RMON_TIMER_H_ */
