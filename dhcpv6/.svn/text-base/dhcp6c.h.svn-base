/*	$KAME: dhcp6c.h,v 1.6 2004/09/04 09:26:38 jinmei Exp $	*/

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
#include <netinet/in.h>
#include <ftm/pkt_ipv6.h>
#include <ftm/pkt_udp.h>
#include <lib/pkt_buffer.h>
#include "dhcpv6_msg.h"
#include "dhcpv6_if.h"

#define DHCP6C_CONF "/home/bin/dhcp6c.conf"
#define DHCP6C_PIDFILE "/var/run/dhcp6c.pid"
#define DUID_FILE   "/etc/dhcp6c_duid"
//struct duid client_duid;
//struct keyinfo ctlkey;
int ctldigestlen;


#define DHCIFF_INFO_ONLY 0x1
#define DHCIFF_RAPID_COMMIT 0x2

struct dhcp6_timer *client6_timo(void *);
int client6_start(struct dhv6_interface *);
void client6_send(struct dhcpv6_if *ifp);
int dhcpv6_ip_address_set(uint32_t ifindex, uint8_t *ip_addr, uint32_t mask, uchar pd_flag, uchar del_flag);
int dhcpv6_client_fsm(uint32_t ifindex, struct pkt_buffer *pkt, enum DHCPV6_EVENT type, uint32_t state, iatype_t ia_type );

