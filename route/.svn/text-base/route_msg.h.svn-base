/**
 * @file      : route_msg.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月27日 16:20:35
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_ROUTE_MSG_H
#define HIOS_ROUTE_MSG_H

#include <lib/thread.h>
#include <lib/msg_ipc_n.h>

/* route 模块发送消息结构 */
struct route_ipc_mesg
{
    struct ipc_msghdr_n msghdr;
    void *pdata;
};

extern int route_ipc_msg_process(struct ipc_mesg_n *pmesg, int imlen);
extern int route_msg_rcv(struct thread *thread);
extern void route_msg_send_thread_callback(void *arg);
extern int route_ipc_msg_callback(struct ipc_mesg_n *pmesg, int imlen);

extern void route_add_msg_to_send_list(void *pdata, int data_len, int data_num, int module_id,
                                        int sender_id, enum IPC_TYPE msg_type, uint8_t subtype,
                                        enum IPC_OPCODE opcode, uint32_t msg_index);


#endif


