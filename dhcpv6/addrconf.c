/******************************************************************************
 * Filename: addrconf.c
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.8.23  wumingming created
 *
******************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <sys/ioctl.h>

#include <net/if.h>

#include <netinet/in.h>

#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "dhcp6.h"
#include "dhcp6c.h"
#include "dhcpv6_if.h"
#include "common.h"
#include "timer.h"
#include "dhcp6c_ia.h"
#include "prefixconf.h"
#include "addrconf.h"
#include "dhcpv6_timer.h"
#if 0
TAILQ_HEAD(statefuladdr_list, statefuladdr);
struct iactl_na {
	struct iactl common;
	struct statefuladdr_list statefuladdr_head;
};
#define iacna_ia common.iactl_ia
#define iacna_callback common.callback
#define iacna_isvalid common.isvalid
#define iacna_duration common.duration
#define iacna_renew_data common.renew_data
#define iacna_rebind_data common.rebind_data
#define iacna_reestablish_data common.reestablish_data
#define iacna_release_data common.release_data
#define iacna_cleanup common.cleanup

struct statefuladdr {
	TAILQ_ENTRY (statefuladdr) link;

	struct dhcp6_statefuladdr addr;
	time_t updatetime;
	struct dhcp6_timer *timer;
	struct iactl_na *ctl;
	struct dhv6_interface *dhcpif;
};

static struct statefuladdr *find_addr(struct statefuladdr_list *,
    struct dhcp6_statefuladdr *);
static int remove_addr(struct statefuladdr *);
static int isvalid_addr(struct iactl *);
static u_int32_t duration_addr(struct iactl *);
static void cleanup_addr(struct iactl *);
static int renew_addr(struct iactl *, struct dhcp6_ia *,
    struct dhcp6_eventdata **, struct dhcp6_eventdata *);
static void na_renew_data_free(struct dhcp6_eventdata *);

static struct dhcp6_timer *addr_timo(void *);

//static int na_ifaddrconf(ifaddrconf_cmd_t, struct statefuladdr *addr);

extern struct dhcp6_timer *client6_timo(void *);
int update_address(struct ia *ia, struct dhcp6_statefuladdr *addr, 
    struct dhv6_interface *dhcpifp, struct iactl **ctlp, 
    void (*callback)(struct ia *))
{
	struct iactl_na *iac_na = (struct iactl_na *)*ctlp;
	struct statefuladdr *sa;
	int sacreate = 0;
	struct timeval timo;

	/*
	 * A client discards any addresses for which the preferred
         * lifetime is greater than the valid lifetime.
	 * [RFC3315 22.6] 
	 */
	if (addr->vltime != DHCP6_DURATION_INFINITE &&
	    (addr->pltime == DHCP6_DURATION_INFINITE ||
	    addr->pltime > addr->vltime)) {
		LOG(LOG_ERR,  "invalid address %s: "
		    "pltime (%u) is larger than vltime (%u)",
		    in6addr2str(&addr->addr, 0),
		    addr->pltime, addr->vltime);
		return -1;
	}

	if (iac_na == NULL) {
		if ((iac_na = malloc(sizeof(*iac_na))) == NULL) {
			LOG(LOG_ERR,  "memory allocation failed");
			return -1;
		}
		memset(iac_na, 0, sizeof(*iac_na));
		iac_na->iacna_ia = ia;
		iac_na->iacna_callback = callback;
		iac_na->iacna_isvalid = isvalid_addr;
		iac_na->iacna_duration = duration_addr;
		iac_na->iacna_cleanup = cleanup_addr;
		iac_na->iacna_renew_data =
		    iac_na->iacna_rebind_data =
		    iac_na->iacna_release_data =
		    iac_na->iacna_reestablish_data = renew_addr;

		TAILQ_INIT(&iac_na->statefuladdr_head);
		*ctlp = (struct iactl *)iac_na;
	}

	/* search for the given address, and make a new one if it fails */
	if ((sa = find_addr(&iac_na->statefuladdr_head, addr)) == NULL) {
		if ((sa = malloc(sizeof(*sa))) == NULL) {
			LOG(LOG_ERR,  "memory allocation failed");
			return -1;
		}
		memset(sa, 0, sizeof(*sa));
		sa->addr.addr = addr->addr;
		sa->ctl = iac_na;
		TAILQ_INSERT_TAIL(&iac_na->statefuladdr_head, sa, link);
		sacreate = 1;
	}

	/* update the timestamp of update */
	sa->updatetime = time(NULL);

	/* update the prefix according to addr */
	sa->addr.pltime = addr->pltime;
	sa->addr.vltime = addr->vltime;
	sa->dhcpif = dhcpifp;
	LOG(LOG_DEBUG,  "%s an address %s pltime=%u, vltime=%u",
	    sacreate ? "create" : "update",
	    in6addr2str(&addr->addr, 0), addr->pltime, addr->vltime);

	if (addr->vltime != 0) {
		if (dhcpv6_ip_address_set(dhcpifp->ifid, (char *)addr->addr.s6_addr, 0, 0))
			return -1;
	}
	/*
	 * If the new vltime is 0, this address immediately expires.
	 * Otherwise, set up or update the associated timer.
	 */
	switch (sa->addr.vltime) {
	case 0:
		if (remove_addr(sa) < 0)
			return -1;
		break;
	case DHCP6_DURATION_INFINITE:
		if (sa->timer)
			dhcp6_remove_timer(&sa->timer);
		break;
	default:
		if (sa->timer == NULL) {
			sa->timer = dhcp6_add_timer(addr_timo, sa);
			if (sa->timer == NULL) {
				LOG(LOG_ERR, "failed to add stateful addr timer");
				remove_addr(sa); /* XXX */
				return -1;
			}
		}
		/* update the timer */
		timo.tv_sec = sa->addr.vltime;
		timo.tv_usec = 0;

		dhcp6_set_timer(&timo, sa->timer);
		break;
	}

	return 0;
}

static struct statefuladdr *find_addr(struct statefuladdr_list *head, struct dhcp6_statefuladdr *addr)
{
	struct statefuladdr *sa;

	for (sa = TAILQ_FIRST(head); sa; sa = TAILQ_NEXT(sa, link)) {
		if (!IN6_ARE_ADDR_EQUAL(&sa->addr.addr, &addr->addr))
			continue;
		return (sa);
	}

	return (NULL);
}

static int remove_addr(struct statefuladdr *sa)
{
	LOG(LOG_DEBUG,  "remove an address %s",
	    in6addr2str(&sa->addr.addr, 0));

	if (sa->timer)
		dhcp6_remove_timer(&sa->timer);

	TAILQ_REMOVE(&sa->ctl->statefuladdr_head, sa, link);
	//ret = na_ifaddrconf(IFADDRCONF_REMOVE, sa);
	if (dhcpv6_ip_address_set(sa->dhcpif->ifid, NULL, 0, 1))
			return -1;
	free(sa);

	return 0;
}

static int isvalid_addr(struct iactl *iac)
{
	struct iactl_na *iac_na = (struct iactl_na *)iac;

	if (TAILQ_EMPTY(&iac_na->statefuladdr_head))
		return (0);	/* this IA is invalid */
	return (1);
}

static u_int32_t duration_addr(struct iactl *iac)
{
	struct iactl_na *iac_na = (struct iactl_na *)iac;
	struct statefuladdr *sa;
	u_int32_t base = DHCP6_DURATION_INFINITE, pltime, passed;
	time_t now;

	/* Determine the smallest period until pltime expires. */
	now = time(NULL);
	for (sa = TAILQ_FIRST(&iac_na->statefuladdr_head); sa;
	    sa = TAILQ_NEXT(sa, link)) {
		passed = now > sa->updatetime ?
		    (u_int32_t)(now - sa->updatetime) : 0;
		pltime = sa->addr.pltime > passed ?
		    sa->addr.pltime - passed : 0;

		if (base == DHCP6_DURATION_INFINITE || pltime < base)
			base = pltime;
	}

	return (base);
}

static void cleanup_addr( struct iactl *iac)
{
	struct iactl_na *iac_na = (struct iactl_na *)iac;
	struct statefuladdr *sa;

	while ((sa = TAILQ_FIRST(&iac_na->statefuladdr_head)) != NULL) {
		TAILQ_REMOVE(&iac_na->statefuladdr_head, sa, link);
		remove_addr(sa);
	}

	free(iac);
}

static int renew_addr(struct iactl *iac, struct dhcp6_ia *iaparam,
    struct dhcp6_eventdata **evdp, struct dhcp6_eventdata *evd)
{
	struct iactl_na *iac_na = (struct iactl_na *)iac;
	struct statefuladdr *sa;
	struct dhcp6_list *ial = NULL, pl;

	TAILQ_INIT(&pl);
	for (sa = TAILQ_FIRST(&iac_na->statefuladdr_head); sa;
	    sa = TAILQ_NEXT(sa, link)) {
		if (dhcp6_add_listval(&pl, DHCP6_LISTVAL_STATEFULADDR6,
		    &sa->addr, NULL) == NULL)
			goto fail;
	}

	if ((ial = malloc(sizeof(*ial))) == NULL)
		goto fail;
	TAILQ_INIT(ial);
	if (dhcp6_add_listval(ial, DHCP6_LISTVAL_IANA, iaparam, &pl) == NULL)
		goto fail;
	dhcp6_clear_list(&pl);

	evd->type = DHCP6_EVDATA_IANA;
	evd->data = (void *)ial;
	evd->privdata = (void *)evdp;
	evd->destructor = na_renew_data_free;

	return (0);

  fail:
	dhcp6_clear_list(&pl);
	if (ial)
		free(ial);
	return (-1);
}

static void na_renew_data_free(struct dhcp6_eventdata *evd)
{
	struct dhcp6_list *ial;

	if (evd->type != DHCP6_EVDATA_IANA) {
		LOG(LOG_ERR,  "assumption failure");
		exit(1);
	}

	if (evd->privdata)
		*(struct dhcp6_eventdata **)evd->privdata = NULL;
	ial = (struct dhcp6_list *)evd->data;
	dhcp6_clear_list(ial);
	free(ial);
}

static struct dhcp6_timer *addr_timo(void *arg)
{
	struct statefuladdr *sa = (struct statefuladdr *)arg;
	struct ia *ia;
	void (*callback)(struct ia *);

	LOG(LOG_DEBUG,  "address timeout for %s",
	    in6addr2str(&sa->addr.addr, 0));

	ia = sa->ctl->iacna_ia;
	callback = sa->ctl->iacna_callback;

	if (sa->timer)
		dhcp6_remove_timer(&sa->timer);

	remove_addr(sa);

	(*callback)(ia);

	return (NULL);
}
#endif
