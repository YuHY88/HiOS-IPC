#ifndef _DHCPRELAY_H
#define _DHCPRELAY_H

#include "dhcp_if.h"

struct option82_agent
{
	char agent, length;
	char str[16];
	char pad[2];
};

struct xid_item *xid_find(struct xid_item *xid_list,uint32_t xid);
int relay_xid_del_timeout(void *msg);

int dhcp_relay_fsm(struct if_dhcp *pif, struct pkt_buffer *pkt);

extern struct if_dhcp *pif_relay;

#define LOOP_XID_ITEM(list , cur)\
	for(cur = list ; cur ; cur =cur->next)

#endif

