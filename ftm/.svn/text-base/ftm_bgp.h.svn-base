
#ifndef HIOS_FTM_BGP_H
#define HIOS_FTM_BGP_H


#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/msg_ipc_n.h>
#include <pkt_tcp.h>

struct bgp_tcp_connect_with_md5{
	int         used_flag;
	uint32_t    sip;
	uint32_t    dip;
	char        passwd[256];
};

#define PEER_MAX_NUM            32

int ftm_bgp_msg_proc(void *data,int data_len,uint16_t sender_id, uint8_t msg_subtype);
int bgp_peer_entry_lookup(uint32_t sip,uint32_t dip);
int bgp_md5_digest_make(int flag, struct tcphdr*th, char *data,int data_len, char* digest, int dir);
void bgp_port_switch_set(int enable);

#endif
