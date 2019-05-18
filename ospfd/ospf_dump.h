/*
 * OSPFd dump routine.
 * Copyright (C) 1999 Toshiaki Takada
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _ZEBRA_OSPF_DUMP_H
#define _ZEBRA_OSPF_DUMP_H

#include "lib/pkt_buffer.h"

#include "ospfd/ospf_lsa.h"



/* Debug Flags. */
#define OSPF_DEBUG_HELLO    0x01
#define OSPF_DEBUG_DB_DESC  0x02
#define OSPF_DEBUG_LS_REQ   0x04
#define OSPF_DEBUG_LS_UPD   0x08
#define OSPF_DEBUG_LS_ACK   0x10
#define OSPF_DEBUG_ALL      0x1f

#define OSPF_DEBUG_SEND     0x01
#define OSPF_DEBUG_RECV     0x02
#define OSPF_DEBUG_SEND_RECV    0x03
#define OSPF_DEBUG_DETAIL   0x04

#define OSPF_DEBUG_ISM_STATUS   0x01
#define OSPF_DEBUG_ISM_EVENTS   0x02
#define OSPF_DEBUG_ISM_TIMERS   0x04
#define OSPF_DEBUG_ISM      0x07
#define OSPF_DEBUG_NSM_STATUS   0x01
#define OSPF_DEBUG_NSM_EVENTS   0x02
#define OSPF_DEBUG_NSM_TIMERS   0x04
#define OSPF_DEBUG_NSM      0x07

#define OSPF_DEBUG_LSA_GENERATE 0x01
#define OSPF_DEBUG_LSA_FLOODING 0x02
#define OSPF_DEBUG_LSA_INSTALL  0x04
#define OSPF_DEBUG_LSA_REFRESH  0x08
#define OSPF_DEBUG_LSA      0x0F

#define OSPF_DEBUG_ZEBRA_INTERFACE     0x01
#define OSPF_DEBUG_ZEBRA_REDISTRIBUTE  0x02
#define OSPF_DEBUG_ZEBRA           0x03

#define OSPF_DEBUG_EVENT        0x01
#define OSPF_DEBUG_NSSA     	0x02
#define OSPF_DEBUG_TE          	0x04
#define OSPF_DEBUG_DCN          0x01
#define OSPF_DEBUG_BFD          0x01

/*******************new debug start*************************************/
#define OSPF_DBG_DCN          			(0x01 << 0)
#define OSPF_DBG_BFD          			(0x01 << 1)
#define OSPF_DBG_EVENT          		(0x01 << 2)
#define OSPF_DBG_TE          			(0x01 << 3)
#define OSPF_DBG_NSSA          			(0x01 << 4)
#define OSPF_DBG_ZEBRA_INTERFACE        (0x01 << 5)
#define OSPF_DBG_ZEBRA_REDISTRIBUTE		(0x01 << 6)
#define OSPF_DBG_LSA          			(0x01 << 7)
#define OSPF_DBG_LSA_REFRESH          	(0x01 << 8)
#define OSPF_DBG_LSA_FLOODING          	(0x01 << 9)
#define OSPF_DBG_LSA_GENERATE          	(0x01 << 10)
#define OSPF_DBG_NSM_TIMERS         	(0x01 << 11)
#define OSPF_DBG_NSM_EVENTS          	(0x01 << 12)
#define OSPF_DBG_ISM_TIMERS         	(0x01 << 13)
#define OSPF_DBG_ISM_EVENTS          	(0x01 << 14)
#define OSPF_DBG_SEND          			(0x01 << 15)
#define OSPF_DBG_RECV         			(0x01 << 16)
#define OSPF_DBG_DETAIL          		(0x01 << 17)
#define OSPF_DBG_HELLO          		(0x01 << 18)
#define OSPF_DBG_DB_DESC          		(0x01 << 19)
#define OSPF_DBG_LS_REQ         		(0x01 << 20)
#define OSPF_DBG_LS_UPD          		(0x01 << 21)
#define OSPF_DBG_LS_ACK          		(0x01 << 22)
#define OSPF_DBG_OTHER          		(0x01 << 23)
//#define OSPF_DBG_PKT_ALL         		(0x01 << 32)


/*******************new debug end***************************************/

/* Macro for setting debug option. */
#define CONF_DEBUG_PACKET_ON(a, b)      conf_debug_ospf_packet[a] |= (b)
#define CONF_DEBUG_PACKET_OFF(a, b)     conf_debug_ospf_packet[a] &= ~(b)
#define TERM_DEBUG_PACKET_ON(a, b)      term_debug_ospf_packet[a] |= (b)
#define TERM_DEBUG_PACKET_OFF(a, b)     term_debug_ospf_packet[a] &= ~(b)
#define DEBUG_PACKET_ON(a, b) \
    do { \
      CONF_DEBUG_PACKET_ON(a, b); \
      TERM_DEBUG_PACKET_ON(a, b); \
    } while (0)
#define DEBUG_PACKET_OFF(a, b) \
    do { \
      CONF_DEBUG_PACKET_OFF(a, b); \
      TERM_DEBUG_PACKET_OFF(a, b); \
    } while (0)

#define CONF_DEBUG_ON(a, b)  conf_debug_ospf_ ## a |= (OSPF_DEBUG_ ## b)
#define CONF_DEBUG_OFF(a, b)     conf_debug_ospf_ ## a &= ~(OSPF_DEBUG_ ## b)
#define TERM_DEBUG_ON(a, b)  term_debug_ospf_ ## a |= (OSPF_DEBUG_ ## b)
#define TERM_DEBUG_OFF(a, b)     term_debug_ospf_ ## a &= ~(OSPF_DEBUG_ ## b)
#define DEBUG_ON(a, b) \
     do { \
       CONF_DEBUG_ON(a, b); \
       TERM_DEBUG_ON(a, b); \
     } while (0)
#define DEBUG_OFF(a, b) \
     do { \
       CONF_DEBUG_OFF(a, b); \
       TERM_DEBUG_OFF(a, b); \
     } while (0)

/* Macro for checking debug option. */
#define IS_DEBUG_OSPF_PACKET(a, b) \
    (term_debug_ospf_packet[a] & OSPF_DEBUG_ ## b)

//#define IS_DEBUG_OSPF_PACKET(a, b) 1

    
/*#define IS_DEBUG_OSPF(a, b) \
    (term_debug_ospf_ ## a & OSPF_DEBUG_ ## b)
*/

#define IS_DEBUG_OSPF(a, b) 1

#define IS_DEBUG_OSPF_EVENT IS_DEBUG_OSPF(event,EVENT)

#define IS_DEBUG_OSPF_NSSA  IS_DEBUG_OSPF(nssa,NSSA)

#define IS_DEBUG_OSPF_TE  IS_DEBUG_OSPF(te,TE)

#define IS_DEBUG_OSPF_DCN  IS_DEBUG_OSPF(dcn,DCN)

#define IS_DEBUG_OSPF_BFD  IS_DEBUG_OSPF(bfd,BFD)

#define IS_CONF_DEBUG_OSPF_PACKET(a, b) \
    (conf_debug_ospf_packet[a] & OSPF_DEBUG_ ## b)
#define IS_CONF_DEBUG_OSPF(a, b) \
    (conf_debug_ospf_ ## a & OSPF_DEBUG_ ## b)

#define OSPF_LOG_DEBUG(format, ...) \
    zlog_debug(OSPF_DBG_OTHER, "%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define OSPF_LOG_ERROR(format, ...) \
	zlog_err("%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#ifdef ORIGINAL_CODING
#else /* ORIGINAL_CODING */
struct stream;
#endif /* ORIGINAL_CODING */

#define AREA_NAME(A)    ospf_area_name_string ((A))
#define IF_NAME(I)      ospf_if_name_string ((I))
#define IF_NAME_BRIEF(I) ospf_if_name_brief((I))


/* Extern debug flag. */
extern unsigned long term_debug_ospf_packet[];
extern unsigned long term_debug_ospf_event;
extern unsigned long term_debug_ospf_ism;
extern unsigned long term_debug_ospf_nsm;
extern unsigned long term_debug_ospf_lsa;
extern unsigned long term_debug_ospf_zebra;
extern unsigned long term_debug_ospf_nssa;
extern unsigned long term_debug_ospf_te;
extern unsigned long term_debug_ospf_dcn;
extern unsigned long term_debug_ospf_bfd;



/* Message Strings. */
extern char *ospf_lsa_type_str[];
extern const struct message ospf_auth_type_str[];
extern const size_t ospf_auth_type_str_max;

/* Prototypes. */
extern const char *ospf_area_name_string(struct ospf_area *);
extern const char *ospf_area_desc_string(struct ospf_area *);
extern const char *ospf_if_name_string(struct ospf_interface *);
extern void ospf_nbr_state_message(struct ospf_neighbor *, char *, size_t);
extern char *ospf_options_dump(u_char);
extern const char *ospf_timer_dump(struct thread *, char *, size_t);
extern const char *ospf_timer_dump_new (TIMERID t, u_int32_t timer_size, char *buf, size_t size);
extern const char *ospf_timeval_dump(struct timeval *, char *, size_t);
extern void ospf_ip_header_dump(struct ip *);
extern void ospf_packet_dump(struct stream *);
extern void ospf_lsa_header_dump(struct lsa_header *);
extern void ospf_ip_header_recv_dump (struct ip_control *ipcb);
const char *ospf_if_name_brief (struct ospf_interface *);


extern void debug_init(void);

/* Appropriate buffer size to use with ospf_timer_dump and ospf_timeval_dump: */
#define OSPF_TIME_DUMP_SIZE 16

#endif /* _ZEBRA_OSPF_DUMP_H */


