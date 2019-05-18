#ifndef HIOS_MPLS_APS_FSM_H
#define HIOS_MPLS_APS_FSM_H


#include <lib/pkt_buffer.h>
#include <ftm/pkt_mpls.h>


#include "mpls_aps/mpls_aps.h"




 


/* APS packet request/signal define */
#define APS_REQUEST_SIGNAL_NULL	      0
#define APS_REQUEST_SIGNAL_NORMAL     1

/* aps 报文格式 */
struct mplsaps_pkt
{
	
#if BIG_ENDIAN_ON 
	uint8_t level:3,
  		    version:5;
	uint8_t opcode;
	uint8_t flag;
	uint8_t tlv_offset;
	uint8_t request_state:4,
			port_type_A:1,
			port_type_B:1,
			port_type_D:1,
			port_type_R:1;
	uint8_t request_signal;
	uint8_t bridge_signal;
	uint8_t bridge_type:1,
  		    reserved:7;//session_id
#else
	uint8_t version:5,
			level:3;
	uint8_t opcode;
	uint8_t flag;
	uint8_t tlv_offset;
	uint8_t port_type_R:1,
	    	port_type_D:1,
	    	port_type_B:1,
	    	port_type_A:1,
			request_state:4;	
	uint8_t request_signal;
	uint8_t bridge_signal;
	uint8_t reserved:7,//session_id
  			bridge_type:1;
#endif
	uint8_t end_tlv;
}__attribute__((packed));	

struct aps_pkt_lsp
{
	struct mpls_label label;
	uint32_t ctrl_word;	
    struct mplsaps_pkt lb_pkt;
}__attribute__((packed));

void mplsaps_pkt_register(void);
int mplsaps_wtr_expiry (void *arg);
void mplsaps_start_wtr_timer (struct aps_sess  *psess);
void mplsaps_stop_wtr_timer (struct aps_sess  *psess);
int mplsaps_holdoff_expiry (void *arg);
void mplsaps_start_holdoff_timer  (struct aps_sess  *psess);
int mplsaps_msg_expiry (void *arg);
void mplsaps_start_msg_timer (struct aps_sess *psess);
void mplsaps_stop_msg_timer (struct aps_sess *psess);
int mplsaps_fsm(struct aps_sess *psess, struct mplsaps_pkt * aps_pdu,uint8_t current_event);
int mplsaps_local_handle_lop ( struct aps_sess  *psess);
int mplsaps_local_handle_fs ( struct aps_sess  *psess);
int mplsaps_local_handle_ms ( struct aps_sess  *psess);
int mplsaps_local_handle_sf( struct aps_sess  *psess);
int mplsaps_link_down_handler (struct aps_sess *psess);
int mplsaps_local_handle_fsclear ( struct aps_sess *psess);
int mplsaps_local_handle_msclear ( struct aps_sess *psess);
int mplsaps_local_handle_lopclear ( struct aps_sess *psess);
int mplsaps_local_handle_rsf ( struct aps_sess *psess);
int mplsaps_local_handle_wtr_timeout(struct aps_sess  * psess);
int mplsaps_aps_change_to_nrw(struct aps_sess  * psess);
int mplsaps_aps_change_to_nrp(struct aps_sess  * psess);
int mplsaps_aps_handle_nr(struct aps_sess   * psess,struct mplsaps_pkt   *aps_pdu);
int mplsaps_aps_handle_dnr(struct aps_sess  *psess);
int mplsaps_init_session (struct aps_sess *psess);
int mplsaps_state_update(uint16_t sess_id, enum APS_OAM_STATE_E event);
int mplsaps_pkt_send(struct aps_sess *psess);
int mplsaps_send (struct aps_sess *psess);
int mpls_aps_pkt_recv(struct pkt_buffer *ppkt);
int mplsaps_updata_backup_status ( struct aps_sess *psess);


#endif

