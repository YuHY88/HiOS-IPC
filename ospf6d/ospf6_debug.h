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

#ifndef _ZEBRA_OSPF6_DEBUG_H
#define _ZEBRA_OSPF6_DEBUG_H


/* Debug Flags. */
/*******************new debug start*************************************/
#define OSPF6_DBG_MESSAGE_RECV          			(0x01 << 0)
#define OSPF6_DBG_MESSAGE_SEND          			(0x01 << 1)
#define OSPF6_DBG_LSA_DEBUG							(0x01 << 2)
#define OSPF6_DBG_INTERFACE							(0x01 << 3)
#define OSPF6_DBG_NEIGHBOR							(0x01 << 4)
#define OSPF6_DBG_ZEBRA_MSG							(0x01 << 5)
#define OSPF6_DBG_SPF								(0x01 << 6)
#define OSPF6_DBG_ROUTE								(0x01 << 6)
#define OSPF6_DBG_BROUTER							(0x01 << 7)
#define OSPF6_DBG_ASBR								(0x01 << 8)
#define OSPF6_DBG_ABR								(0x01 << 9)
#define OSPF6_DBG_FLOODING							(0x01 << 10)
#define OSPF6_DBG_OTHER          					(0x01 << 11)



/*******************new debug end***************************************/

extern void install_element_ospf6_debug_interface(void);
extern void install_element_ospf6_debug_message(void);
extern void install_element_ospf6_debug_lsa(void);
extern void install_element_ospf6_debug_neighbor(void);
extern void install_element_ospf6_debug_zebra(void);
extern void install_element_ospf6_debug_spf(void);
extern void install_element_ospf6_debug_route(void);
extern void install_element_ospf6_debug_brouter(void);
extern void install_element_ospf6_debug_asbr(void);
extern void install_element_ospf6_debug_abr(void);
extern void install_element_ospf6_debug_flood(void);
extern void install_element_ospf6_debug_other(void);


#endif /* _ZEBRA_OSPF6_DEBUG_H */


