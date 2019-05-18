
#ifndef HIOS_MPLSTP_OAM_AIS_H
#define HIOS_MPLSTP_OAM_AIS_H

#include "mpls_oam/mpls_oam.h"
#include <ftm/pkt_mpls.h>



struct oam_ais_pw
{
    struct cfm_pdu_header oam_header;
    uint8_t               end_tlv;
}__attribute__((packed));


struct oam_ais_lsp
{
	struct mpls_label  gal_label;
	uint32_t           oam_ach;	
    struct oam_ais_pw  ais_pdu;
}__attribute__((packed));

struct oam_ais
{
    uint16_t ais_interval;
	uint8_t  level;
	uint8_t  pad;
	struct list *ais_transport;
    struct oam_ais_pw ais_pw;
    struct oam_ais_lsp ais_lsp;
};

int mplsoam_ais_pdu_construct(struct oam_session *psess);
int mplsoam_ais_start(struct oam_session *psess);
int mplsoam_ais_send(void *arg);
int mplsoam_ais_if_get_clients_info(struct oam_session *psess, uint32_t ifindex);
int mplsoam_ais_lsp_get_clients_info(struct oam_session *psess, uint32_t lspindex);
int mplsoam_ais_rcv_pkt(struct pkt_buffer *pkt);
int mplsoam_ais_clear(void *arg);
void mplsoam_ais_register(void);




#endif



