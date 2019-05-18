#ifndef HIOS_CES_MSG_H
#define HIOS_CES_MSG_H

#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>

//int ces_msg_rcv(struct thread *thread);
int ces_msg_rcv_msg(struct ipc_mesg_n *pmesg, int imlen);
void ces_msg_send_noack(struct ipc_msghdr_n *pReqhdr, uint32_t errcode, unsigned int msg_index);
int ces_msg_send_hal_wait_reply(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                                enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

int ces_msg_send_hal_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                              enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

int ces_msg_send_hal(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                     enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

#endif

