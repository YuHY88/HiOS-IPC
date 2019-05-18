#ifndef HIOS_MPLSTP_OAM_CSF_H
#define HIOS_MPLSTP_OAM_CSF_H

#include "mpls_oam/mpls_oam.h"
#include <ftm/pkt_mpls.h>
#include "mpls_oam/mpls_oam_ais.h"

struct oam_csf
{
	uint16_t csf_interval;
	uint8_t  level;
	struct oam_ais_pw csf_pw;
};

int mplsoam_csf_pdu_construct(struct oam_session *psess);
int mplsoam_csf_start(struct oam_session *psess);
int mplsoam_csf_send(void *arg);
int mplsoam_csf_rcv_pkt(struct pkt_buffer *pkt);
int mplsoam_csf_clear(void *arg);
void mplsoam_csf_register(void);

#endif

