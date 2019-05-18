#ifndef HIOS_EFM_PKT_H
#define HIOS_EFM_PKT_H

#include "l2/efm/efm.h"
#include "l2/efm/efm_cmd.h"
#include "l2/efm/efm_def.h"
#include <lib/pkt_buffer.h>

#define EFM_BUF_SIZE 						1500
#define EFM_INFO_TLV_LENGTH        16

#define EFM_TLV_UINT32_ENCODE(T,V)                                \
	do {																			\
		(V) = htonl(((uint32_t)(V) << 24) 						\
				+ ((uint32_t)(V) << 16)								\
				+ ((uint32_t)(V) << 8)								\
				+ ((uint32_t)(V)));									\
		memcpy(T, &V, 4);										  		\
		(T) += 4;																	\
	} while (0)


#define EFM_TLV_UCHAR_DECODE(T,V)                              \
	do {																											 \
		(V) = ((T)[j]); 																				 \
		j++;																										 \
		length--; 																							 \
	} while (0)


#define EFM_TLV_UINT16_DECODE(T,V)                             \
	do {																											 \
		(V) = ((uint16_t)((T)[j]) << 8) 																	 \
				+  ((uint16_t)((T)[j+1]));																	 \
		j += 2; 													\
		length -= 2;                                                                                                           \
	} while (0)

#define EFM_TLV_UINT32_DECODE(T,V)                             \
	do {																											 \
		(V) = ((uint32_t)((T)[j]) << 24)																 \
				+ ((uint32_t)((T)[j+1]) << 16)														 \
				+ ((uint32_t)((T)[j+2]) << 8)															 \
				+ ((uint32_t)((T)[j+3])); 																	 \
		j += 4; 											\
		length -= 4;										\
	} while (0)


#define EFM_TLV_UINT64_DECODE(T,V)                             \
		do {																											 \
			(V) = ((uint64_t)((T)[j]) << 56)													 \
						+ ((uint64_t)((T)[j+1]) << 48)										 \
						+ ((uint64_t)((T)[j+2]) << 40)											 \
						+ ((uint64_t)((T)[j+3]) << 32) 													 \
						+ ((uint64_t)((T)[j+4]) << 24)												\
						+ ((uint64_t)((T)[j+5]) << 16)												\
						+ ((uint64_t)((T)[j+6]) << 8)												\
						+ ((uint64_t)((T)[j+7]));									   		\
			j += 8; 											\
			length -= 8;										\
		} while (0)


void efm_pkt_register(void);
void efm_pkt_unregister(void);

int
efm_format_information_pdu (struct efm_if *pefm, register uint8_t *bufptr);

int
efm_format_loopback_pdu (struct efm_if *pefm, register uint8_t *bufptr);

int
efm_format_err_sym_period_pdu (struct efm_if *pefm, register uint8_t *bufptr);

int
efm_format_err_frame_pdu (struct efm_if *pefm, register uint8_t *bufptr);

int
efm_format_err_frame_period_pdu (struct efm_if *pefm, register uint8_t *bufptr);

int
efm_format_err_frame_seconds_pdu (struct efm_if *pefm, register uint8_t *bufptr);


int
efm_tx (struct efm_if *pefm, enum efm_pdu pdu_type);

int
efm_parse_efmpdu (uint8_t *buf, const int len,
              struct efm_oam_pdu *efm_pdu_t, struct efm_if *pefm);

void
efm_record_pdu (struct efm_if *pefm, struct efm_oam_pdu *efm_pdu_t, struct pkt_buffer *ppkt);

int efm_recv(struct pkt_buffer *ppkt);
int efm_send(struct efm_if *pefm, uint8_t *bufptr, int len);



#endif

