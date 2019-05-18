/*
*       manage the l2 msg 
*
*/

#ifndef HIOS_L2_MSG_H
#define HIOS_L2_MSG_H

#include <lib/msg_ipc_n.h>

void l2_if_init(void);
void l2_msg_rcv_subifevent(struct ipc_msghdr_n *phdr, struct ifm_event *pevent);
void l2_msg_rcv_ifevent(struct ipc_msghdr_n *phdr, struct ifm_event *pevent);
void l2_msg_rcv_cfm(struct ipc_msghdr_n *phdr, void *pdata);

void l2_msg_rcv_vlan(struct ipc_msghdr_n *phdr, void *pdata);
void l2_msg_rcv_mac(struct ipc_msghdr_n *phdr, void *pdata);
void l2_msg_rcv_trunk(struct ipc_msghdr_n  *phdr, void *pdata);
void l2_msg_rcv_loopdetect(struct ipc_msghdr_n *phdr);
int l2_msg_rcv_msg(struct ipc_mesg_n *pmesg);
int l2_msg_rcv_pkt(void);

int l2_msg_rcv(struct thread *t);
int l2_msg_rcv_msg_n(struct ipc_mesg_n *pmesg, int imlen);
int l2_msg_rcv_pkt_n(struct ipc_mesg_n *pmesg);
int l2_msg_rcv_n(struct ipc_mesg_n *pmesg, int imlen);

int l2_msg_rcv_reserve_vlan(struct ipc_msghdr_n *phdr, void *pdata);

int l2_msg_send_ack (struct ipc_msghdr_n *rcvhdr,uint32_t msg_index);
int l2_msg_send_noack (uint32_t errcode, struct ipc_msghdr_n *rcvhdr,uint32_t msg_index);
int l2_msg_recv_hal_wait_reply(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                                               enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);
int l2_msg_send_hal_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
		enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);
#endif

