/*
*       handle ipc message
*
*/
#ifndef HIOS_CLOCK_MSG_H
#define HIOS_CLOCK_MSG_H

enum CLOCK_SNMP_SUBTYPE_INFO
{
    /*synce snmp subtype*/
    IPC_TYPE_SNMP_SYNCE_GLOBAL_INFO = 1,
    IPC_TYPE_SNMP_SYNCE_CLOCK_IF_NEXT_IFINDEX,
    IPC_TYPE_SNMP_SYNCE_CLOCK_IF_INFO,
    IPC_TYPE_SNMP_SYNCE_CLOCK_SRC_TABLE_NEXT_IFINDEX,
    IPC_TYPE_SNMP_SYNCE_CLOCK_SRC_TABLE_INFO,

};

int clock_msg_rcv_msg(struct ipc_mesg_n *pmesg);/* 接收 IPC 消息 */
int clock_msg_rcv_pkt(void);/* 接收报文 */
int clock_msg_rcv(struct thread *t); /* 处理 clock 的 IPC 消息和报文接收 */
int clock_msg_rcv_msg_n(struct ipc_mesg_n *pmesg, int imlen);


#endif

