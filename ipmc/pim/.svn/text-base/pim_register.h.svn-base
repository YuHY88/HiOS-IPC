#ifndef PIM_REGISTER_H
#define PIM_REGISTER_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/linklist.h>
#include <lib/thread.h>

#include "../ipmc_if.h" 

/* PIM_REGISTER definitions */
#define PIM_REGISTER_BORDER_BIT         	0x80000000
#define PIM_REGISTER_NULL_REGISTER_BIT  	0x40000000

/* PIM_REGISTER-related definitions */
#define PIM_REGISTER_SUPPRESSION_TIME_DEFAULT	60
#define PIM_REGISTER_PROBE_TIME_DEFAULT		5/* Used to send NULL_REGISTER */
#define PIM_REGISTER_HEADER_LENGTH		8

/* Other timeout default values */
#define PIM_RP_KEEPALIVE_PERIOD_DEFAULT		(3 * PIM_REGISTER_SUPPRESSION_TIME_DEFAULT + PIM_REGISTER_PROBE_TIME_DEFAULT)

/* register state machine state*/
#define PIM_REG_STATE_BASE 0x10
#define PIM_REG_STATE_NI	(PIM_REG_STATE_BASE + 1)
#define PIM_REG_STATE_J		(PIM_REG_STATE_BASE + 2)
#define PIM_REG_STATE_P		(PIM_REG_STATE_BASE + 3)
#define PIM_REG_STATE_JP	(PIM_REG_STATE_BASE + 4)
/* register state machine event */
#define PIM_REG_RST_EXPIRE 	 0x01
#define PIM_REG_COULD_REG 	 0x02
#define PIM_REG_COULD_NOT_REG 0x03
#define PIM_REG_REG_STOP_RCV  0x04
#define PIM_REG_RP_CHANGE     0x05

uint8_t CouldRegisterSG(struct pim_mrt_sg *mrt_sg, uint32_t in_port);
sint32 pim_register_stop_timer_expire(void *para);
void pim_reg_state_machine(struct pim_mrt_sg *mrt_sg, uint32_t event);
uint32_t pim_data_pkt_forward(struct pim_mrt_sg *mrt_sg, uint8_t *udp_data, uint32_t data_len);
sint32 pim_data_pkt_recv(struct pkt_buffer *ppkt);
sint32 pim_register_null_send(uint32_t ifindex, uint32_t dst, uint32_t inner_src, uint32_t inner_dst);
sint32 pim_register_send(uint32_t rp_addr, uint8_t rs_tos,  uint32_t inner_sip, uint32_t inner_grp, 
								struct pkt_buffer *ppkt, uint8_t null_reg_flag);
sint32 pim_register_recv(uint32_t outer_dip, uint32_t outer_sip, uint8_t outer_ip_tos,
								uint8_t *data, uint16_t data_len);
sint32 pim_register_stop_send(uint32_t outer_dip, uint32_t outer_sip, uint8_t outer_ip_tos,
								uint32_t inner_sip, uint32_t inner_grp);
sint32 pim_register_stop_recv(uint32_t outer_dip, uint32_t outer_sip,
									uint8_t *data, uint16_t data_len);


#endif
