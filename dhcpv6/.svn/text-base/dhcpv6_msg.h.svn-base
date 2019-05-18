#ifndef _ZEBRA_DHCPV6_MSG_H
#define _ZEBRA_DHCPV6_MSG_H
#include <lib/thread.h>
enum DHCPV6_EVENT
{
	DHCPV6_EVENT_INVALID = 0,
	DHCPV6_EVENT_FROM_CMD,		
	DHCPV6_EVENT_FROM_PKT,
	DHCPV6_EVENT_FROM_IPC,	
	DHCPV6_EVENT_FROM_TIMER,
	DHCPV6_COMMAND_START,
	DHCPV6_COMMAND_STOP
};

void dhcpv6_pkt_register(void);
int dhcpv6_pkt_rcv(struct thread *argv);
void dhcpv6_event_register(void);
void dhcpv6_pkt_dump (void *data, int len);
int dhcpv6_ipc_rcv(struct thread *argv);

#endif
