/*
*     define of ha
*
*/

#ifndef HIOS_HA_H
#define HIOS_HA_H

#include <lib/msg_ipc.h>
#include <lib/thread.h>
#include <lib/sys_ha.h>
#include <netinet/in.h>


#define HA_MSG_LEN 65480
#define ETH1_IP "1.0.0."
#define ETH1_IF_SHIFT 32
//#define ETH1_IP "172.16.95."
/* HA 消息类型 */
enum HA_MSG_E
{
    HA_MSG_INVALID = 0,
    HA_MSG_HA_START,         /* ha 开始 */
    HA_MSG_HA_END,           /* ha 结束 */
    HA_MSG_HA_START_OK,      /* 板卡启动完成 */
};


/* ha 通道报文的数据结构 */
struct ha_msg_t
{
    struct ipc_msghdr_n msghdr;
    uint8_t msg_data[HA_MSG_LEN];
};

void ha_send_heartbeat(int slot);

int ha_start_heartbeat_timer(struct thread *thread);

int ha_init(unsigned int slot);      /* ha 模块初始化 */
int ha_send_msg(void *pdata, int data_len, int data_num, int module_id, int sender_id, enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode);
//int devm_msg_rcv_ha(struct ipc_msghdr *phdr, void *pdata);
int ha_thread ( struct thread *thread );
int ha_msg_rcv(struct ipc_mesg_n *pmsg, int imlen);

int ha_handle_ha_msg(struct ipc_mesg_n *pmsg);
int ha_send_pkt(void *pdata, struct ipc_msghdr_n *phdr);
int ha_recv_pkt(struct ipc_mesg_n *pmsg);
#if 0
int ha_send_pkt(struct ha_ipc_mesg *p_ha_mesg);
int ha_recv_pkt(struct ha_ipc_mesg *phamesg);
#endif
extern int inet_aton(const char *cp, struct in_addr *addr);


#endif

