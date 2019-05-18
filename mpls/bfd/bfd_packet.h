/*
*   bfd 收发包和状态机处理
*/
#ifndef HIOS_BFD_PACKET_H
#define HIOS_BFD_PACKET_H

#include <lib/types.h>
#include <ftm/pkt_eth.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_udp.h>
#include "bfd.h"
#include "bfd_session.h"


/*bfd控制包*/
struct bfd_message
{
#if BIG_ENDIAN_ON

	uint8_t version:3,
			diag:5;
	uint8_t sta:2,
			poll:1,
			final:1,
			CPI:1,
			auth:1,
			demand:1,
			resv:1;
#else 
	uint8_t diag:5,
			version:3;
	uint8_t resv:1,
			demand:1,
			auth:1,
			CPI:1,
			final:1,
			poll:1,
			sta:2;
#endif
	uint8_t detect_mult;
	uint8_t length;
	uint32_t local_disc;
	uint32_t remote_disc;
	uint32_t min_tx_interval;
	uint32_t min_rx_interval;
	uint32_t min_echo_rx_interval;
};


struct udp_bfd_packet
{
	struct iphdr ip;
	struct udphdr udp;
	struct bfd_message data;
};
struct udp_bfd_ip_packet
{
	struct udphdr udp;
	struct bfd_message data;
};


void bfd_init(void);
void bfd_pkt_register(void);
void bfd_pkt_unregister(void);
void bfd_pkt_dump(void *data, int len);
void bfd_pkt_debug(void *data, int len, uint32_t ifindex, int flag);
void bfd_fsm_start_debug(enum BFD_EVENT event, struct bfd_sess *old_psess);
void bfd_fsm_end_debug(struct bfd_sess *new_psess);
int bfd_pkt_encap(struct bfd_sess *psess, uint8_t *pkt);
int bfd_send_hal(struct bfd_sess *psess, enum IPC_OPCODE optcode, uint8_t subtype);
int bfd_send(struct bfd_sess *psess);
int bfd_send_cc(void *arg);
int bfd_cc_timeout(void *arg);
int bfd_send_retry(void *arg);
int bfd_send_poll(struct bfd_sess *psess, enum BFD_EVENT poll_event);
int bfd_send_final(struct bfd_sess *psess);
void bfd_recv_poll(struct bfd_sess *psess, struct bfd_message *pmsg);
void bfd_recv_final(struct bfd_sess *psess, struct bfd_message *pmsg);
void bfd_detect_timer_expire(struct thread* thread);
void bfd_init_timer_expire(struct thread* thread);
void bfd_recv(struct pkt_buffer *pkt);
void bfd_fsm(enum BFD_EVENT event, struct bfd_sess *psess);
int bfd_ip_pkt_encap(struct bfd_sess *psess, uint8_t *pkt);
void bfd_alarm_process(enum IPC_OPCODE opcode, uint32_t sess_id);

#endif
