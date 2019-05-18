/******************************************************************************
 * Filename: aaa_packet.h
 *	Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2016.9.20  luoyz created
 *
******************************************************************************/

#ifndef _AAA_PACKET_H_
#define _AAA_PACKET_H_

#include <lib/aaa_common.h>
#include <lib/thread.h>
#include <lib/msg_ipc_n.h>

#define AUTH_PKT	0
#define ACCT_PKT	1


/* aaa protocol */
enum
{
	AAA_RADIUS = 0,
	AAA_TAC_PLUS,
	AAA_EAP,
	AAA_PROTO_MAX
};

enum
{
	AAA_PKT_SEND,
	AAA_PKT_RECV,
};

#define AAA_PKT_DBG_LEN  256

typedef struct _aaa_pkt_debug_
{
	uint8_t radius_send;
	uint8_t radius_recv;
	uint8_t tac_plus_send;
	uint8_t tac_plus_recv;
}aaa_pkt_debug;


void aaa_radius_pkt_register(void);
void aaa_radius_pkt_unregister(void);
void aaa_tac_plus_pkt_register(void);
void aaa_tac_plus_pkt_unregister(void);
void aaa_dot1x_pkt_register(void);
void aaa_dot1x_pkt_unregister(void);


int aaa_radius_pkt_send(char *buf, int len, int pkt_type);
/* get server address by tacacs+ pkt type;
 * call pkt_send() send tacacs+ pkt payload buffer.
 */
int aaa_tac_plus_pkt_send(int type, char *buf, uint32_t len, uint16_t src_port);

int aaa_handle_ftm_msg(struct ipc_mesg_n *pmsg);
void aaa_pkt_dump(const char *buf, uint32_t len, const int opt, const uint8_t proto, const char *caller);

int aaa_eap_pkt_send(char *buf, size_t len, uint32_t ifindex, char *dmac);


#endif  /* _AAA_PACKET_H_ */

