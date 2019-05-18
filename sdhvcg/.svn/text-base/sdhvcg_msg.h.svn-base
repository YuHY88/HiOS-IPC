#ifndef HIOS_SDHVCG_MSG_H
#define HIOS_SDHVCG_MSG_H

extern int vcg_msg_rcv_msg(struct ipc_mesg_n *pmesg, int imlen);
extern int vcg_msg_rcv(struct ipc_mesg_n *pmsg, int imlen);
extern int vcg_msg_send_hal_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                              enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);
#endif

