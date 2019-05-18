#ifndef HIOS_ERPS_PKT_H
#define HIOS_ERPS_PKT_H


#include "erps.h"

enum forward_direction{
	VIRTUAL_TUNNEL_2_INTERFACE = 1,
	INTERFACE_2_VIRTUAL_TUNNEL = 2,
};
void raps_pkt_register(void);
int raps_validity_check(struct erps_sess *psess, struct raps_pkt *raps_pdu);
int raps_recv(struct pkt_buffer *ppkt);
int raps_send(struct erps_sess *psess);
void raps_stop(struct erps_sess *psess);
int raps_send_burst(struct erps_sess *psess);
int raps_forward(struct erps_sess *psess,const struct raps_pkt *recv_aps,uint32_t	 ifindex, enum forward_direction direc);


#endif
