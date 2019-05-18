/*
*   处理 aps 报文和 elps 状态机
*/
#ifndef HIOS_ELPS_PKT_H
#define HIOS_ELPS_PKT_H


#include "aps/elps.h"
#include "aps/elps_cmd.h"
#include <lib/pkt_buffer.h>


int elps_fsm(struct elps_sess *psess,struct aps_pkt *aps_pdu,enum ELPS_EVENT event); /* elps 状态机*/
int aps_recv(struct pkt_buffer *ppkt); /* 接收 aps 报文 */
int aps_send(struct elps_sess *psess);    /* 发送 aps 报文 */
int aps_send_burst (struct elps_sess *psess);/**/
void aps_pkt_register(void);
int elps_state_update(uint32_t ifindex,uint16_t sess_id,enum ELPS_PORT_EVENT event);


#endif

