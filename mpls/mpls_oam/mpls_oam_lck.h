
#ifndef HIOS_MPLSTP_OAM_LOCK_H
#define HIOS_MPLSTP_OAM_LOCK_H

#include "mpls_oam/mpls_oam.h"
#include <ftm/pkt_mpls.h>
#include "mpls_oam/mpls_oam_ais.h"


struct oam_lck
{
	uint16_t lck_interval;
	uint8_t  level;
	struct list *lck_transport;
	struct oam_ais_pw lck_pw;
	struct oam_ais_lsp lck_lsp;
};


int mplsoam_lck_pdu_construct(struct oam_session *psess);
int mplsoam_lck_start(struct oam_session *psess);
int mplsoam_lck_send(void *arg);
int mplsoam_lck_rcv_pkt(struct pkt_buffer *pkt);
int mplsoam_lck_clear(void *arg);
void mplsoam_lck_register(void);
int mplsoam_lck_if_get_clients_info(struct oam_session *psess, uint32_t ifindex);
int mplsoam_lck_lsp_get_clients_info(struct oam_session *psess, uint32_t lspindex);

#endif

