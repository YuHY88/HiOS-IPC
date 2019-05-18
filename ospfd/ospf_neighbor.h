/*
 * OSPF Neighbor functions.
 * Copyright (C) 1999, 2000 Toshiaki Takada
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
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

#ifndef _ZEBRA_OSPF_NEIGHBOR_H
#define _ZEBRA_OSPF_NEIGHBOR_H

#include <ftm/pkt_ip.h>
#include "ospfd/ospf_packet.h"
#include "ospfd/ospf_bfd.h"

#if 0
/* from netinet/ip.h, 解决编译冲突 */
struct ip
{
#if BIG_ENDIAN_ON
    unsigned int ip_v:4; /* version */
    unsigned int ip_hl:4; /* header length */
#else
    unsigned int ip_hl:4; /* header length */
    unsigned int ip_v:4; /* version */
#endif
    u_int8_t ip_tos; /* type of service */
    u_short ip_len; /* total length */
    u_short ip_id; /* identification */
    u_short ip_off; /* fragment offset field */
#define IP_RF 0x8000 /* reserved fragment flag */
#define IP_DF 0x4000 /* dont fragment flag */
#define IP_MF 0x2000 /* more fragments flag */
#define IP_OFFMASK 0x1fff /* mask for fragmenting bits */
    u_int8_t ip_ttl; /* time to live */
    u_int8_t ip_p; /* protocol */
    u_short ip_sum; /* checksum */
    struct in_addr ip_src, ip_dst; /* source and dest address */
};
#endif

/* Neighbor Data Structure */
struct ospf_neighbor
{
    /* This neighbor's parent ospf interface. */
    struct ospf_interface *oi;

    /* OSPF neighbor Information */
    u_char state;             /* NSM status. */
    u_char dd_flags;          /* DD bit flags. */
    u_int32_t dd_seqnum;          /* DD Sequence Number. */

    /* Neighbor Information from Hello. */
    struct prefix address;        /* Neighbor Interface Address. */

    struct in_addr src;           /* Src address. */
    struct in_addr router_id;     /* Router ID. */
    u_char options;           /* Options. */
    int priority;             /* Router Priority. */
    struct in_addr d_router;      /* Designated Router. */
    struct in_addr bd_router;     /* Backup Designated Router. */

    /* Last sent Database Description packet. */
    struct ospf_packet *last_send;
    /* Timestemp when last Database Description packet was sent */
    struct timeval last_send_ts;

    /* Last received Databse Description packet. */
    struct
    {
        u_char options;
        u_char flags;
        u_int32_t dd_seqnum;
    } last_recv;

    /* LSA data. */
    struct ospf_lsdb ls_rxmt;
    struct ospf_lsdb db_sum;
    struct ospf_lsdb ls_req;
    struct ospf_lsa *ls_req_last;

    u_int32_t crypt_seqnum;           /* Cryptographic Sequence Number. */

    /* Timer values. */
    u_int32_t v_inactivity;
    u_int32_t v_db_desc;
    u_int32_t v_ls_req;
    u_int32_t v_ls_upd;

    /* Threads TIMER. */
    TIMERID 		t_inactivity;
    TIMERID 		t_db_desc;
    TIMERID 		t_ls_req;
    TIMERID 		t_ls_upd;
    TIMERID 		t_hello_reply;

    /* NBMA configured neighbour */
    struct ospf_nbr_nbma *nbr_nbma;

    /* Statistics */
    struct timeval ts_last_progress;  /* last advance of NSM            */
    struct timeval ts_last_regress;   /* last regressive NSM change     */
    const char *last_regress_str;     /* Event which last regressed NSM */
    u_int32_t state_change;           /* NSM state change counter       */
    u_char ef_flags;                  /* 0: valid; 1: invalid nbr for dr and bdr election*/

	u_char bfd_flag;				/*1:the nbr enable bfd; 0:the nbr disable bfd*/
#define OSPF_NEIGHBOR_NO_BFD 	0
#define OSPF_NEIGHBOR_BFD 		1
	struct ospf_bfd_sess_info bfd_session_info;
};

/* Macros. */
#define NBR_IS_DR(n)    IPV4_ADDR_SAME (&n->address.u.prefix4, &n->d_router)
#define NBR_IS_BDR(n)   IPV4_ADDR_SAME (&n->address.u.prefix4, &n->bd_router)

/* Prototypes. */
extern struct ospf_neighbor *ospf_nbr_new (struct ospf_interface *);
extern void ospf_nbr_free (struct ospf_neighbor *);
extern void ospf_nbr_delete (struct ospf_neighbor *);
extern int ospf_nbr_bidirectional (struct in_addr *, struct in_addr *, int);
extern void ospf_nbr_self_reset (struct ospf_interface *);
extern void ospf_nbr_add_self (struct ospf_interface *);
extern int ospf_nbr_count (struct ospf_interface *, int);
extern int ospf_nbr_count_opaque_capable (struct ospf_interface *);

extern struct ospf_neighbor *ospf_nbr_get (struct ospf_interface *,
        struct ospf_header *,
        struct ip , struct prefix *);
extern struct ospf_neighbor *ospf_nbr_lookup (struct ospf_interface *,
        struct ip ,
        struct ospf_header *);

extern struct ospf_neighbor *ospf_nbr_lookup_by_addr (struct route_table *,
        struct in_addr *);
extern struct ospf_neighbor *ospf_nbr_lookup_by_routerid (struct route_table
        *,
        struct in_addr *);
extern void ospf_renegotiate_optional_capabilities (struct ospf *top);

#endif /* _ZEBRA_OSPF_NEIGHBOR_H */


