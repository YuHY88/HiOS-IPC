/* RIP debug routines
 * Copyright (C) 1999 Kunihiro Ishiguro <kunihiro@zebra.org>
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
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef _ZEBRA_RIP_DEBUG_H
#define _ZEBRA_RIP_DEBUG_H

/* RIP debug event flags. */
#define RIP_DEBUG_EVENT   0x01

/* RIP debug packet flags. */
#define RIP_DEBUG_PACKET  0x01
/* Debug flags. */
#define RIPNG_DEBUG_EVENT   0x01
#define RIPNG_DEBUG_PACKET  0x01

/* Debug related macro. */
//#define IS_RIP_DEBUG_EVENT  (rip_debug_event & RIP_DEBUG_EVENT)
//#define IS_RIP_DEBUG_PACKET (rip_debug_packet & RIP_DEBUG_PACKET)
#define IS_RIP_DEBUG_EVENT  1
#define IS_RIP_DEBUG_PACKET 1


/* Debug related macro. */
#define IS_RIPNG_DEBUG_EVENT  (ripng_debug_event & RIPNG_DEBUG_EVENT)
#define IS_RIPNG_DEBUG_PACKET (ripng_debug_packet & RIPNG_DEBUG_PACKET)

/* Rip debug type macro*/
#define RIP_DBG_EVENT		(1 << 0)		//rip event  debug
#define RIP_DBG_PACKET		(1 << 1)		//rip packet debug
#define RIPNG_DBG_EVENT		(1 << 2)		//ripng envet debug
#define RIPNG_DBG_PACKET	(1 << 3)		//ripng packet debug
#define RIPNG_DBG_ZEBRA		(1 << 4)		//ripng zebra  debug

//#define RIP_DBG_NONE		(1 << 4)		//none   debug
//#define RIP_DBG_DOT1X		(1 << 5)		//dot1x  debug
#define RIP_DBG_ALL			(RIP_DBG_EVENT |\
							 RIP_DBG_PACKET)	//所有debug

#define RIP_DBG_V6_ALL		(RIPNG_DBG_EVENT|\
							 RIPNG_DBG_PACKET|\
							 RIPNG_DBG_ZEBRA)	//所有debug


#define RIP_LOG_DEBUG(format, ...) \
    zlog_debug("%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define RIP_LOG_ERROR(format, ...) \
	zlog_err("%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

extern unsigned long rip_debug_event;
extern unsigned long rip_debug_packet;
extern unsigned long ripng_debug_event;
extern unsigned long ripng_debug_packet;

extern void rip_debug_init(void);
extern void rip_debug_reset(void);

#endif /* _ZEBRA_RIP_DEBUG_H */
