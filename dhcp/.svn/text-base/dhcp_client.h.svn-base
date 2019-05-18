/* dhcpc.h */
#ifndef _DHCPCLIENT_H
#define _DHCPCLIENT_H

#include <stdint.h>

#include <lib/pkt_buffer.h>

#include "dhcp_msg.h"
#include "dhcp_if.h"

#define INIT_SELECTING	0
#define REQUESTING	1
#define BOUND		2
#define RENEWING	3
#define REBINDING	4
#define INIT_REBOOT	5
#define RENEW_REQUESTED 6
#define RELEASED	7
#define PRCTL_NAME_SIZE 8
#define PRCTL_NAME_MAX_SIZE 16

long uptime(void);
struct vendorsub_message *get_vendor_suboption(uint8_t *message, int code);
int dhcp_ip_address_set(uint32_t ifindex,uint32_t ip_addr,uint32_t mask, uint8_t del_flag, uchar zero);
int dhcp_pkt_send_timer(void *arg);
void dhcp_state_to_str(int state,char *str);
int dhcp_trap(void *arg);
int dhcp_zero_cancel(uint32_t ifindex);
void dhcp_zero_rcv_pthread(struct pkt_buffer *pkt);
int dhcp_client_fsm(struct if_dhcp *pif, struct pkt_buffer *pkt, enum DHCP_EVENT enType);
void *dhcp_zero_send(void *arg);
int dhcp_zero(uint32_t ifindex);
int dhcp_client_zero_disable(int ifindex);
void *dhcp_zero_rcv(void *arg);
int delete_zero_if(uint32_t ifindex);
uint32_t dhcp_cancel_all_if_zero_pthread(void);

#endif

