#ifndef HIOS_EFM_STATE_H
#define HIOS_EFM_STATE_H

#include "efm/efm.h"
#include "efm/efm_cmd.h"
#include "efm/efm_def.h"

int
efm_pdu_timer_expiry (void * arg);

int
efm_link_timer_expiry (void * arg);

int
efm_discovery_fault (struct efm_if *pefm);

int
efm_discovery_active_send_local (struct efm_if *pefm);

int
efm_discovery_passive_wait (struct efm_if *pefm);

int
efm_discovery_send_local_remote (struct efm_if *pefm);

int
efm_discovery_send_local_remote_ok (struct efm_if *pefm);

int
efm_discovery_send_any (struct efm_if *pefm);

int
efm_transmit_reset (struct efm_if *pefm, enum efm_pdu pdu);

int
efm_transmit_wait_for_tx (struct efm_if *pefm);

int
efm_transmit_dec_pdu_cnt (struct efm_if *pefm, enum efm_pdu pdu);

int
efm_transmit_tx_oampdu (struct efm_if *pefm, enum efm_pdu pdu);



void
efm_discovery_state_machine (struct efm_if *pefm );

void
efm_transmit_state_machine (struct efm_if *pefm, enum efm_pdu pdu);

void
efm_run_state_machine (struct efm_if *pefm);

void
efm_process_rem_loopback (struct efm_if *pefm);

#endif

