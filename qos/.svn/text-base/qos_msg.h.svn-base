

#ifndef HIOS_QOS_MSG_H
#define HIOS_QOS_MSG_H

#include <lib/msg_ipc.h>

#include <lib/msg_ipc_n.h>

#if 0
int qos_msg_rcv(struct thread *pthread);
#else
int qos_msg_rcv(struct ipc_mesg_n *pmsg, int imlen);
#endif

int 
qos_ipc_send_and_wait_ack(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

#endif


