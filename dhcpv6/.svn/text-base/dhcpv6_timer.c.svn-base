#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/queue.h>

#include <netinet/in.h>

#include <unistd.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>

#include "dhcp6.h"
#include "dhcpv6_if.h"
#include "common.h"
#include "dhcpv6_timer.h"

#define MILLION 1000000

LIST_HEAD(, dhcp6_timer) timer_head;
static struct timeval tm_sentinel;
static struct timeval tm_max = {0x7fffffff, 0x7fffffff};

static void timeval_add(struct timeval *, struct timeval *, struct timeval *);
extern struct thread_master *dhcpv6_master;
void dhcp6_timer_init(void)
{
	LIST_INIT(&timer_head);
	tm_sentinel = tm_max;
}

struct dhcp6_timer *dhcp6_add_timer(struct dhcp6_timer *(*timeout)(void *), void *timeodata)
{
	DHCPV6_LOG_DEBUG("");
	struct dhcp6_timer *newtimer;
	LOG(LOG_DEBUG, "dhcpv6 add timer...");
	if ((newtimer = malloc(sizeof(*newtimer))) == NULL) {
		LOG(LOG_ERR, "can't allocate memory");
		return (NULL);
	}

	memset(newtimer, 0, sizeof(*newtimer));

	if (timeout == NULL) {
		LOG(LOG_ERR, "timeout function unspecified");
		exit(1);
	}
	newtimer->expire = timeout;
	newtimer->expire_data = timeodata;
	newtimer->tm = tm_max;

	LIST_INSERT_HEAD(&timer_head, newtimer, link);
	return (newtimer);
}

void dhcp6_remove_timer(struct dhcp6_timer **timer)
{
	DHCPV6_LOG_DEBUG("");
	LOG(LOG_ERR, "dhcpv6 remove timer...");
	LIST_REMOVE(*timer, link);
	if (*timer) {
		free(*timer);
		*timer = NULL;
	}
}

void dhcp6_set_timer(struct timeval *tm, struct dhcp6_timer *timer)
{
	struct timeval now;

	/* reset the timer */
	gettimeofday(&now, NULL);

	timeval_add(&now, tm, &timer->tm);

	/* update the next expiration time */
	if (TIMEVAL_LT(timer->tm, tm_sentinel))
		tm_sentinel = timer->tm;
	DHCPV6_LOG_DEBUG("tm_sentinel->tv_usec %ld, tm_sentinel->tv_sec %ld, timer->tv_usec %ld, timer->tv_sec %ld", 
		tm_sentinel.tv_usec, tm_sentinel.tv_sec,timer->tm.tv_usec, timer->tm.tv_sec);
	return;
}

/*
 * Check expiration for each timer. If a timer is expired,
 * call the expire function for the timer and update the timer.
 * Return the next interval for select() call.
 */
int dhcp6_check_timer(struct thread *argv)
{
	//DHCPV6_LOG_DEBUG("");
	static struct timeval returnval;
	struct timeval now;
	struct dhcp6_timer *tm, *tm_next;

	gettimeofday(&now, NULL);
	tm_sentinel = tm_max;
	for (tm = LIST_FIRST(&timer_head); tm; tm = tm_next) {
		tm_next = LIST_NEXT(tm, link);
		//DHCPV6_LOG_DEBUG("tm_sentinel->tv_usec %d, tm_sentinel->tv_sec %d," 
		//	"tm->tv_usec %d, tm->tv_sec %d, now->tv_usec %d, now->tv_sec %d", 
		//	tm_sentinel.tv_usec, tm_sentinel.tv_sec, tm->tm.tv_usec, tm->tm.tv_sec, 
		//	now.tv_usec, now.tv_sec);
		if (TIMEVALE_LEQ(tm->tm, now)) {
			DHCPV6_LOG_DEBUG("");
			if ((*tm->expire)(tm->expire_data) == NULL) {
				DHCPV6_LOG_DEBUG("");
				continue; /* timer has been freed */
			}
		}

		if (TIMEVAL_LT(tm->tm, tm_sentinel)) {
			tm_sentinel = tm->tm;
		}
		//DHCPV6_LOG_DEBUG("tm_sentinel->tv_usec %d, tm_sentinel->tv_sec %d, tm->tv_usec %d, tm->tv_sec %d", 
		//	tm_sentinel.tv_usec, tm_sentinel.tv_sec,tm->tm.tv_usec, tm->tm.tv_sec);
	}
	//DHCPV6_LOG_DEBUG("tm_sentinel->tv_usec %d, tm_sentinel->tv_sec %d", 
	//	tm_sentinel.tv_usec, tm_sentinel.tv_sec);
	if (TIMEVAL_EQUAL(tm_max, tm_sentinel)) {
		/* no need to timeout */
		return 0;
	} else if (TIMEVAL_LT(tm_sentinel, now)) {
		/* this may occur when the interval is too small */
		returnval.tv_sec = returnval.tv_usec = 0;
	} else {
		/* 距离下次还有多长时间 */
		timeval_sub(&tm_sentinel, &now, &returnval);
	}
	DHCPV6_LOG_DEBUG("returnval->tv_usec %ld, returnval->tv_sec %ld", returnval.tv_usec, returnval.tv_sec);
	thread_add_timer(dhcpv6_master,dhcp6_check_timer,NULL,2); // 2 s

	return (int)(&returnval.tv_sec);
}

struct timeval *dhcp6_timer_rest(struct dhcp6_timer *timer)
{
	struct timeval now;
	static struct timeval returnval; /* XXX */

	gettimeofday(&now, NULL);
	if (TIMEVALE_LEQ(timer->tm, now)) {
		LOG(LOG_DEBUG, "a timer must be expired, but not yet");
		returnval.tv_sec = returnval.tv_usec = 0;
	} else
		timeval_sub(&timer->tm, &now, &returnval);

	return (&returnval);
}

/* result = a + b */
static void timeval_add(struct timeval *a, struct timeval *b, struct timeval *result)
{
	long l;

	if ((l = a->tv_usec + b->tv_usec) < MILLION) {
		result->tv_usec = l;
		result->tv_sec = a->tv_sec + b->tv_sec;
	}
	else {
		result->tv_usec = l - MILLION;
		result->tv_sec = a->tv_sec + b->tv_sec + 1;
	}
}

/*
 * result = a - b
 * XXX: this function assumes that a >= b.
 */
void timeval_sub(struct timeval *a, struct timeval *b, struct timeval *result)
{
	long l;

	if ((l = a->tv_usec - b->tv_usec) >= 0) {
		result->tv_usec = l;
		result->tv_sec = a->tv_sec - b->tv_sec;
	}
	else {
		result->tv_usec = MILLION + l;
		result->tv_sec = a->tv_sec - b->tv_sec - 1;
	}
}
