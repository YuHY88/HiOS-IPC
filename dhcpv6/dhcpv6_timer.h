#ifndef _ZEBRA_DHCPV6_TIMER_H
#define _ZEBRA_DHCPV6_TIMER_H
#include <lib/thread.h>
/* a < b */
#define TIMEVAL_LT(a, b) (((a).tv_sec < (b).tv_sec) ||\
			  (((a).tv_sec == (b).tv_sec) && \
			    ((a).tv_usec < (b).tv_usec)))
/* a <= b */
#define TIMEVALE_LEQ(a, b) (((a).tv_sec < (b).tv_sec) ||\
			   (((a).tv_sec == (b).tv_sec) &&\
 			    ((a).tv_usec <= (b).tv_usec)))
/* a == b */
#define TIMEVAL_EQUAL(a, b) ((a).tv_sec == (b).tv_sec &&\
			     (a).tv_usec == (b).tv_usec)

struct dhcp6_timer {
	LIST_ENTRY(dhcp6_timer) link;

	struct timeval tm;

	struct dhcp6_timer *(*expire)(void *);
	void *expire_data;
};

void dhcp6_timer_init(void);
struct dhcp6_timer *dhcp6_add_timer(struct dhcp6_timer *(*)(void *), void *);
void dhcp6_set_timer(struct timeval *, struct dhcp6_timer *);
void dhcp6_remove_timer(struct dhcp6_timer **);
int dhcp6_check_timer(struct thread *argv);
struct timeval * dhcp6_timer_rest(struct dhcp6_timer *);

void timeval_sub(struct timeval *, struct timeval *, struct timeval *);

#endif
