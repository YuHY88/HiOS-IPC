#ifndef _VTYSH_MSG_H
#define _VTYSH_MSG_H

#include "vty.h"
#include "msg_ipc.h"
#include "pkt_buffer.h"

extern void vty_msg_init(void);
//extern void *vty_snmp_msg_rcv(void *arg);
extern int vtysh_reconfig_ifm(void *pdata);
extern void vty_cmd_msg_recv(struct ipc_mesg *mesg);
extern void vty_aaa_msg_recv(struct ipc_mesg *mesg);
extern struct login_session * vty_ssh_thread_creat_connect(uint32_t client_ip, uint16_t client_port, uint32_t server_ip);
extern void vty_msg_init(void);
extern void vty_create_connection(struct ipc_mesg_n *mesg);
extern void vty_connect_break_from_client(struct ipc_mesg_n *mesg);
extern struct login_session *vty_telnet_thread_creat(struct pkt_buffer *pkt);
extern struct login_session *vty_ssh_thread_creat(struct pkt_buffer *pkt);
extern void vtysh_handle_tcp_ctrl_msg(struct ipc_mesg_n *mesg);
//extern int vty_msg_rcv(struct thread *);
extern int vty_netconf_pck_rcv(void);
extern void vtysh_handle_tcp_data_msg(struct ipc_mesg_n *mesg);

extern void *ifm_event_config(void *arg);

void vtysh_sync_master_recv_msg(struct ipc_mesg *mesg);
int vtysh_msg_recv_n(struct ipc_mesg_n *pmsg, int len);
#endif

