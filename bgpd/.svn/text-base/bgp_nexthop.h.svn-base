/* BGP nexthop scan
   Copyright (C) 2000 Kunihiro Ishiguro

This file is part of GNU Zebra.

GNU Zebra is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Zebra is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Zebra; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef _QUAGGA_BGP_NEXTHOP_H
#define _QUAGGA_BGP_NEXTHOP_H

#include "if.h"

#define BGP_SCAN_INTERVAL_DEFAULT   30
#define BGP_IMPORT_INTERVAL_DEFAULT 15

#define NEXTHOP_FAMILY(nexthop_len) ( \
  ((nexthop_len) ==  4 ||             \
   (nexthop_len) == 12 ? AF_INET :    \
  ((nexthop_len) == 16 ||             \
   (nexthop_len) == 24 ||             \
   (nexthop_len) == 48 ? AF_INET6 :   \
  AF_UNSPEC))                         \
)

extern int inet_aton(const char *cp, struct in_addr *addr);
extern void bgp_scan_init (void);
extern void bgp_scan_finish (void);
extern int bgp_nexthop_lookup (afi_t, struct peer *peer, struct bgp_info *,struct prefix *,uint32_t *);
extern void bgp_connected_add (struct connected *c);
extern void bgp_connected_delete (struct connected *c);
extern int bgp_multiaccess_check_v4 (struct in_addr, char *);
extern int bgp_nexthop_connect_check (uint16_t , union sockunion *);
extern int bgp_nexthop_onlink (afi_t, struct attr *, uint16_t);
extern int bgp_nexthop_self (struct attr *);
extern void bgp_address_init (void);
extern void bgp_address_destroy (void);
extern int bgp_add_routerId_from_connect_table(struct bgp *bgp);

#endif /* _QUAGGA_BGP_NEXTHOP_H */
