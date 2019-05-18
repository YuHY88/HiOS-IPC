#ifndef _ISIS_IPC_H
#define _ISIS_IPC_H

#include <lib/msg_ipc.h>
#include <route/route.h>
#include <lib/if.h>

#define IPC_ISIS_LEN     8192

struct isis_ipv4
{
  u_char type;

  u_char flags;

  u_char message;

  u_char nexthop_num;
  
  u_char ifindex_num;
  ifindex_t *ifindex;

  u_char distance;

  u_int32_t metric;

  uint8_t  instance;

  u_int32_t mtu;

  uint16_t	vpn;

  vrf_id_t vrf_id;
};

int isis_send_message_route(struct route_entry route,enum IPC_OPCODE opcode);
int isis_zebra_if_add (struct ifm_event *pevent, vrf_id_t vrf_id);
int isis_zebra_if_del (struct ifm_event *pevent, vrf_id_t vrf_id);
int isis_zebra_if_state_up (struct ifm_event *pevent, vrf_id_t vrf_id);
int isis_zebra_if_state_down (struct ifm_event *pevent, vrf_id_t vrf_id);
int isis_ifm_msg_rcv();
//int isis_add_fifo_route(struct ipc_mesg_n *mesg);
void isis_read_mesg_from_routefifo(void);
int isis_msg_rcv(struct thread *thread);
void isis_msg_init(void);
int isis_asyc_send_route_cmd (void * argv);
void
isis_route_read_ipv4 (struct route_entry *proute,u_int8_t code);
void
isis_route_read_ipv6 (struct route_entry *proute,u_int8_t code);
void isis_route_manage(struct ipc_mesg_n *mesg);
int saveisis_ifm_msg_rcv();
int isis_l3vpn_event_dispose(struct ifm_event *pevent, vrf_id_t vrf_id);

int isis_ifm_manage(struct ipc_mesg_n *mesg);
int isis_send_multiplemessage_route(struct route_entry *route,enum IPC_OPCODE opcode, unsigned int item);
int isis_send_multiplemessage_route_n(struct route_entry *route,enum IPC_OPCODE opcode, unsigned int item);

int isis_l3vpn_event_register(void);
void isis_vpn_manage(void *pdata, struct ipc_msghdr_n *pmsghdr);
int isis_ifm_msg_rcv(void);
int isis_route_msg_rcv(void);

int isis_msg_rcv_n(struct ipc_mesg_n *pmesg, int imlen);


#endif

