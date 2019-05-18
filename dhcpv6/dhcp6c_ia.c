/*	$KAME: dhcp6c_ia.c,v 1.33 2005/07/22 08:50:05 jinmei Exp $	*/

/*
 * Copyright (C) 2003 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <netinet/in.h>

#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dhcp6.h"
#include "dhcpv6_if.h"
#include "common.h"
#include "dhcpv6_timer.h"
#include "dhcp6c.h"
#include "dhcp6c_ia.h"
#include "prefixconf.h"
#include "addrconf.h"
#include "auth.h"

#if 0
static int update_authparam(struct ia *, struct authparam *);
static void reestablish_ia(struct ia *);
static void callback(struct ia *);
static int release_ia(struct ia *);
static void remove_ia(struct ia *);
static struct ia *get_ia(iatype_t, struct dhv6_interface *, struct ia_conf *,
    struct dhcp6_listval *, struct duid *);
static struct ia *find_ia(struct ia_conf *, iatype_t, u_int32_t);
static struct dhcp6_timer *ia_timo(void *);

static const char *iastr(iatype_t);
static const char *statestr(iastate_t);
static int update_authparam(struct ia *ia, struct authparam *authparam)
{
	if (authparam == NULL)
		return (0);

	if (ia->authparam == NULL) {
		if ((ia->authparam = copy_authparam(authparam)) == NULL) {
			LOG(LOG_ERR, "failed to copy authparam");
			return (-1);
		}
		return (0);
	}

	/* update the previous RD value and flags */
	ia->authparam->prevrd = authparam->prevrd;
	ia->authparam->flags = authparam->flags;

	return (0);
}

static const char *iastr(iatype_t type)
{
	switch (type) {
	case IATYPE_PD:
		return ("PD");
	case IATYPE_NA:
		return ("NA");
	default:
		return ("???");	/* should be a bug */
	}
}

static const char *statestr(iastate_t state)
{
	switch (state) {
	case IAS_ACTIVE:
		return "ACTIVE";
	case IAS_RENEW:
		return "RENEW";
	case IAS_REBIND:
		return "REBIND";
	default:
		return "???";	/* should be a bug */
	}
}
#endif

