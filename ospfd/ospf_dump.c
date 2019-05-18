/*
 * OSPFd dump routine.
 * Copyright (C) 1999, 2000 Toshiaki Takada
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

#include <zebra.h>


#include "lib/linklist.h"
#include "lib/thread.h"
#include "lib/prefix.h"
#include "lib/command.h"
#include "lib/stream.h"
#include "lib/log.h"
#include "lib/vty.h"
#include "lib/timer.h"
#include "lib/hptimer.h"


#include "ospfd/ospfd.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_ism.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_neighbor.h"
#include "ospfd/ospf_nsm.h"
#include "ospfd/ospf_dump.h"
#include "ospfd/ospf_packet.h"

const struct message ospf_ism_state_msg[] =
{
    { ISM_DependUpon,   "DependUpon" },
    { ISM_Down,         "Down" },
    { ISM_Loopback,     "Loopback" },
    { ISM_Waiting,      "Waiting" },
    { ISM_PointToPoint, "P-2-P" },
    { ISM_DROther,      "DROther" },
    { ISM_Backup,       "Backup" },
    { ISM_DR,           "DR" },
};
const int ospf_ism_state_msg_max = OSPF_ISM_STATE_MAX;

const struct message ospf_nsm_state_msg[] =
{
    { NSM_DependUpon, "DependUpon" },
    { NSM_Deleted,    "Deleted"    },
    { NSM_Down,       "Down" },
    { NSM_Attempt,    "Attempt" },
    { NSM_Init,       "Init" },
    { NSM_TwoWay,     "2-Way" },
    { NSM_ExStart,    "ExStart" },
    { NSM_Exchange,   "Exchange" },
    { NSM_Loading,    "Loading" },
    { NSM_Full,       "Full" },
};
const int ospf_nsm_state_msg_max = OSPF_NSM_STATE_MAX;

const struct message ospf_lsa_type_msg[] =
{
    { OSPF_UNKNOWN_LSA,      "unknown" },
    { OSPF_ROUTER_LSA,       "router" },
    { OSPF_NETWORK_LSA,      "network" },
    { OSPF_SUMMARY_LSA,      "summary" },
    { OSPF_ASBR_SUMMARY_LSA, "AS-summary" },
    { OSPF_AS_EXTERNAL_LSA,  "AS-external" },
    { OSPF_GROUP_MEMBER_LSA, "GROUP MEMBER" },
    { OSPF_AS_NSSA_LSA,      "NSSA" },
    { 8,                     "Type-8" },
    { OSPF_OPAQUE_LINK_LSA,  "Link-Local Opaque" },
    { OSPF_OPAQUE_AREA_LSA,  "Area-Local Opaque" },
    { OSPF_OPAQUE_AS_LSA,    "AS-external Opaque" },
};
const int ospf_lsa_type_msg_max = OSPF_MAX_LSA;

const struct message ospf_link_state_id_type_msg[] =
{
    { OSPF_UNKNOWN_LSA,      "(unknown)" },
    { OSPF_ROUTER_LSA,       "" },
    { OSPF_NETWORK_LSA,      "(address of Designated Router)" },
    { OSPF_SUMMARY_LSA,      "(summary Network Number)" },
    { OSPF_ASBR_SUMMARY_LSA, "(AS Boundary Router address)" },
    { OSPF_AS_EXTERNAL_LSA,  "(External Network Number)" },
    { OSPF_GROUP_MEMBER_LSA, "(Group membership information)" },
    { OSPF_AS_NSSA_LSA,      "(External Network Number for NSSA)" },
    { 8,                     "(Type-8 LSID)" },
    { OSPF_OPAQUE_LINK_LSA,  "(Link-Local Opaque-Type/ID)" },
    { OSPF_OPAQUE_AREA_LSA,  "(Area-Local Opaque-Type/ID)" },
    { OSPF_OPAQUE_AS_LSA,    "(AS-external Opaque-Type/ID)" },
};
const int ospf_link_state_id_type_msg_max = OSPF_MAX_LSA;

const struct message ospf_network_type_msg[] =
{
    { OSPF_IFTYPE_NONE,         	"NONE" },
    { OSPF_IFTYPE_POINTOPOINT,      "Point-to-Point" },
    { OSPF_IFTYPE_BROADCAST,        "Broadcast" },
    { OSPF_IFTYPE_NBMA,             "NBMA" },
    { OSPF_IFTYPE_POINTOMULTIPOINT, "Point-to-MultiPoint" },
    { OSPF_IFTYPE_VIRTUALLINK,      "Virtual-Link" },
};
const int ospf_network_type_msg_max = OSPF_IFTYPE_MAX;

/* AuType */
const struct message ospf_auth_type_str[] =
{
    { OSPF_AUTH_NULL,          "Null"          },
    { OSPF_AUTH_SIMPLE,        "Simple"        },
    { OSPF_AUTH_CRYPTOGRAPHIC, "Cryptographic" },
};
const size_t ospf_auth_type_str_max = sizeof (ospf_auth_type_str) /
                                      sizeof (ospf_auth_type_str[0]);

/* Configuration debug option variables. */
unsigned long conf_debug_ospf_packet[5] = {0, 0, 0, 0, 0};
unsigned long conf_debug_ospf_event = 0;
unsigned long conf_debug_ospf_ism = 0;
unsigned long conf_debug_ospf_nsm = 0;
unsigned long conf_debug_ospf_lsa = 0;
unsigned long conf_debug_ospf_zebra = 0;
unsigned long conf_debug_ospf_nssa = 0;
unsigned long conf_debug_ospf_te = 0;
unsigned long conf_debug_ospf_dcn = 0;
unsigned long conf_debug_ospf_bfd = 0;



/* Enable debug option variables -- valid only session. */
unsigned long term_debug_ospf_packet[5] = {0, 0, 0, 0, 0};
unsigned long term_debug_ospf_event = 0;
unsigned long term_debug_ospf_ism = 0;
unsigned long term_debug_ospf_nsm = 0;
unsigned long term_debug_ospf_lsa = 0;
unsigned long term_debug_ospf_zebra = 0;
unsigned long term_debug_ospf_nssa = 0;
unsigned long term_debug_ospf_te = 0;
unsigned long term_debug_ospf_dcn = 0;
unsigned long term_debug_ospf_bfd = 0;

extern void zlog_debug_set(struct vty *vty, unsigned int type, int enable);

const char *
ospf_redist_string(u_int route_type)
{
    return (route_type == ROUTE_PROTO_MAX) ?
           "Default" : zebra_route_string(route_type);
}

#define OSPF_AREA_STRING_MAXLEN  16
const char *
ospf_area_name_string (struct ospf_area *area)
{
    static char buf[OSPF_AREA_STRING_MAXLEN] = "";
    u_int32_t area_id;
    if (!area)
    {
        return "-";
    }
    area_id = ntohl (area->area_id.s_addr);
    snprintf (buf, OSPF_AREA_STRING_MAXLEN, "%d.%d.%d.%d",
              (area_id >> 24) & 0xff, (area_id >> 16) & 0xff,
              (area_id >> 8) & 0xff, area_id & 0xff);
    return buf;
}

#define OSPF_AREA_DESC_STRING_MAXLEN  30
const char *
ospf_area_desc_string (struct ospf_area *area)
{
    static char buf[OSPF_AREA_DESC_STRING_MAXLEN] = "";
    u_char type;
    if (!area)
    {
        return "(incomplete)";
    }
    type = area->external_routing;
    switch (type)
    {
    case OSPF_AREA_NSSA:
        snprintf (buf, OSPF_AREA_DESC_STRING_MAXLEN, "%s [NSSA]",
                  ospf_area_name_string (area));
        break;
    case OSPF_AREA_STUB:
        snprintf (buf, OSPF_AREA_DESC_STRING_MAXLEN, "%s [Stub]",
                  ospf_area_name_string (area));
        break;
    default:
        return ospf_area_name_string (area);
    }
    return buf;
}

#define OSPF_IF_STRING_MAXLEN  40
const char *
ospf_if_name_string (struct ospf_interface *oi)
{
    static char buf[OSPF_IF_STRING_MAXLEN] = "";
    u_int32_t ifaddr;

	if (!oi)
    {
        return "inactive";
    }
	
    if(oi->ifp == NULL)
    {
        return "inactive";
    }
    if(oi->ifp->name == NULL)
    {
        return "inactive";
    }
    if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
    {
        return oi->ifp->name;
    }
	
    if(oi->address == NULL)
    {
        return "inactive";
    }
	
    ifaddr = ntohl (oi->address->u.prefix4.s_addr);
    snprintf (buf, OSPF_IF_STRING_MAXLEN,
              "%s:%d.%d.%d.%d", oi->ifp->name,
              (ifaddr >> 24) & 0xff, (ifaddr >> 16) & 0xff,
              (ifaddr >> 8) & 0xff, ifaddr & 0xff);
    return buf;
}

const char *
ospf_if_name_brief (struct ospf_interface *oi)
{
    static char buf[OSPF_IF_STRING_MAXLEN] = "";
    //u_int32_t ifaddr;
    if (!oi)
    {
        return "inactive";
    }
    if (oi->type == OSPF_IFTYPE_VIRTUALLINK)
    {
        return oi->ifp->name;
    }
    //ifaddr = ntohl (oi->address->u.prefix4.s_addr);
    snprintf (buf, OSPF_IF_STRING_MAXLEN,
              "%s", oi->ifp->name);
    return buf;
}





void
ospf_nbr_state_message (struct ospf_neighbor *nbr, char *buf, size_t size)
{
    //int state;
    struct ospf_interface *oi = nbr->oi;
    if (oi == NULL)
    {
        return ;
    }
    #if 0
    if (IPV4_ADDR_SAME (&DR (oi), &nbr->address.u.prefix4))
    {
        state = ISM_DR;
    }
    else if (IPV4_ADDR_SAME (&BDR (oi), &nbr->address.u.prefix4))
    {
        state = ISM_Backup;
    }
    else
    {
        state = ISM_DROther;
    }
    #endif
    memset (buf, 0, size);
    snprintf (buf, size, "%s",
              LOOKUP (ospf_nsm_state_msg, nbr->state));
}

const char *
ospf_timeval_dump (struct timeval *t, char *buf, size_t size)
{
    /* Making formatted timer strings. */
#define MINUTE_IN_SECONDS   60
#define HOUR_IN_SECONDS     (60*MINUTE_IN_SECONDS)
#define DAY_IN_SECONDS      (24*HOUR_IN_SECONDS)
#define WEEK_IN_SECONDS     (7*DAY_IN_SECONDS)
    unsigned long w, d, h, m, s, ms, us;
    if (!t)
    {
        return "inactive";
    }
    w = d = h = m = s = ms = us = 0;
    memset (buf, 0, size);
    us = t->tv_usec;
    if (us >= 1000)
    {
        ms = us / 1000;
        us %= 1000;
    }
    if (ms >= 1000)
    {
        t->tv_sec += ms / 1000;
        ms %= 1000;
    }
    if (t->tv_sec > WEEK_IN_SECONDS)
    {
        w = t->tv_sec / WEEK_IN_SECONDS;
        t->tv_sec -= w * WEEK_IN_SECONDS;
    }
    if (t->tv_sec > DAY_IN_SECONDS)
    {
        d = t->tv_sec / DAY_IN_SECONDS;
        t->tv_sec -= d * DAY_IN_SECONDS;
    }
    if (t->tv_sec >= HOUR_IN_SECONDS)
    {
        h = t->tv_sec / HOUR_IN_SECONDS;
        t->tv_sec -= h * HOUR_IN_SECONDS;
    }
    if (t->tv_sec >= MINUTE_IN_SECONDS)
    {
        m = t->tv_sec / MINUTE_IN_SECONDS;
        t->tv_sec -= m * MINUTE_IN_SECONDS;
    }
    if (w > 99)
    {
        snprintf (buf, size, "%ldw%1ldd", (long)w, (long)d);
    }
    else if (w)
    {
        snprintf (buf, size, "%ldw%1ldd%02ldh", (long)w, (long)d, (long)h);
    }
    else if (d)
    {
        snprintf (buf, size, "%1ldd%02ldh%02ldm", (long)d, (long)h, (long)m);
    }
    else if (h)
    {
        snprintf (buf, size, "%ldh%02ldm%02lds", (long)h, (long)m, (long)t->tv_sec);
    }
    else if (m)
    {
        snprintf (buf, size, "%ldm%02lds", (long)m, (long)t->tv_sec);
    }
    else if (ms)
    {
        snprintf (buf, size, "%ld.%03lds", (long)t->tv_sec, (long)ms);
    }
    else
    {
        snprintf (buf, size, "%ld usecs", (long)t->tv_usec);
    }
    return buf;
}

const char *
ospf_timer_dump (struct thread *t, char *buf, size_t size)
{
    struct timeval result;
    if (!t)
    {
        return "inactive";
    }
    result = tv_sub (t->u.sands, time_get_recent_relative_time());
    return ospf_timeval_dump (&result, buf, size);
}

/*func:该函数是为了适应苏工高精度定时器新写的用来计算定时器剩余超时时间										*/
/*para：t			--目的定时器ID												*/
/*para：timer_size	--定时器时长												*/
/*para: buf			--计算结果，以字符串形势存储											*/
/*para：size			--字符串存储字节大小												*/
/*retu：返回字符串形势的结果															*/
const char *
ospf_timer_dump_new (TIMERID t, u_int32_t timer_size, char *buf, size_t size)
{
    struct timeval result;
	int pass_time = 0;
    if (!t)
    {
        return "inactive";
    }
	
	high_pre_timer_passtime(t, &pass_time);
	memset(&result, 0, sizeof(struct timeval));
	result.tv_sec = timer_size - pass_time;
	result.tv_usec = 0;
	result = tv_adjust (result);
	
    return ospf_timeval_dump (&result, buf, size);
}



#define OSPF_OPTION_STR_MAXLEN      24

char *
ospf_options_dump (u_char options)
{
    static char buf[OSPF_OPTION_STR_MAXLEN];
    snprintf(buf, OSPF_OPTION_STR_MAXLEN, "%s|%s|%s|%s|%s|%s|%s|%s",
             (options & OSPF_OPTION_DN) ? "DN" : "-",
             (options & OSPF_OPTION_O) ? "O" : "-",
             (options & OSPF_OPTION_DC) ? "DC" : "-",
             (options & OSPF_OPTION_EA) ? "EA" : "-",
             (options & OSPF_OPTION_NP) ? "N/P" : "-",
             (options & OSPF_OPTION_MC) ? "MC" : "-",
             (options & OSPF_OPTION_E) ? "E" : "-",
             (options & OSPF_OPTION_MT) ? "M/T" : "-");
    return buf;
}

static void
ospf_packet_hello_dump (struct stream *s, u_int16_t length)
{
    struct ospf_hello *hello;
    int i;
    hello = (struct ospf_hello *) STREAM_PNT (s);
    zlog_debug (OSPF_DBG_DETAIL, "Hello");
    zlog_debug (OSPF_DBG_DETAIL, "  NetworkMask %s", inet_ntoa (hello->network_mask));
    zlog_debug (OSPF_DBG_DETAIL, "  HelloInterval %d", ntohs (hello->hello_interval));
    zlog_debug (OSPF_DBG_DETAIL, "  Options %d (%s)", hello->options,
                ospf_options_dump (hello->options));
    zlog_debug (OSPF_DBG_DETAIL, "  RtrPriority %d", hello->priority);
    zlog_debug (OSPF_DBG_DETAIL, "  RtrDeadInterval %ld", (u_long)ntohl (hello->dead_interval));
    zlog_debug (OSPF_DBG_DETAIL, "  DRouter %s", inet_ntoa (hello->d_router));
    zlog_debug (OSPF_DBG_DETAIL, "  BDRouter %s", inet_ntoa (hello->bd_router));
    length -= OSPF_HEADER_SIZE + OSPF_HELLO_MIN_SIZE;
    zlog_debug (OSPF_DBG_DETAIL, "  # Neighbors %d", length / 4);
    for (i = 0; length > 0; i++, length -= sizeof (struct in_addr))
    {
        zlog_debug (OSPF_DBG_DETAIL, "    Neighbor %s", inet_ntoa (hello->neighbors[i]));
    }
}

static char *
ospf_dd_flags_dump (u_char flags, char *buf, size_t size)
{
    memset (buf, 0, size);
    snprintf (buf, size, "%s|%s|%s",
              (flags & OSPF_DD_FLAG_I) ? "I" : "-",
              (flags & OSPF_DD_FLAG_M) ? "M" : "-",
              (flags & OSPF_DD_FLAG_MS) ? "MS" : "-");
    return buf;
}

void
ospf_lsa_header_dump (struct lsa_header *lsah)
{
    const char *lsah_type = LOOKUP (ospf_lsa_type_msg, lsah->type);
    zlog_debug (OSPF_DBG_LSA_GENERATE, "  LSA Header");
    zlog_debug (OSPF_DBG_LSA_GENERATE, "    LS age %d", ntohs (lsah->ls_age));
    zlog_debug (OSPF_DBG_LSA_GENERATE, "    Options %d (%s)", lsah->options,
                ospf_options_dump (lsah->options));
    zlog_debug (OSPF_DBG_LSA_GENERATE, "    LS type %d (%s)", lsah->type,
                (lsah->type ? lsah_type : "unknown type"));
    zlog_debug (OSPF_DBG_LSA_GENERATE, "    Link State ID %s", inet_ntoa (lsah->id));
    zlog_debug (OSPF_DBG_LSA_GENERATE, "    Advertising Router %s", inet_ntoa (lsah->adv_router));
    zlog_debug (OSPF_DBG_LSA_GENERATE, "    LS sequence number 0x%lx", (u_long)ntohl (lsah->ls_seqnum));
    zlog_debug (OSPF_DBG_LSA_GENERATE, "    LS checksum 0x%x", ntohs (lsah->checksum));
    zlog_debug (OSPF_DBG_LSA_GENERATE, "    length %d", ntohs (lsah->length));
}

static char *
ospf_router_lsa_flags_dump (u_char flags, char *buf, size_t size)
{
    memset (buf, 0, size);
    snprintf (buf, size, "%s|%s|%s",
              (flags & ROUTER_LSA_VIRTUAL) ? "V" : "-",
              (flags & ROUTER_LSA_EXTERNAL) ? "E" : "-",
              (flags & ROUTER_LSA_BORDER) ? "B" : "-");
    return buf;
}

static void
ospf_router_lsa_dump (struct stream *s, u_int16_t length)
{
    int i = 0, len;
    char buf[BUFSIZ];
    struct router_lsa *rl = NULL;
	
    rl = (struct router_lsa *) STREAM_PNT (s);
    zlog_debug (OSPF_DBG_DETAIL, "  Router-LSA");
    zlog_debug (OSPF_DBG_DETAIL, "    flags %s",
                ospf_router_lsa_flags_dump (rl->flags, buf, BUFSIZ));
    zlog_debug (OSPF_DBG_DETAIL, "    # links %d", ntohs (rl->links));
    len = (ntohs (rl->header.length) - (OSPF_LSA_HEADER_SIZE + 4U));
	for (i = 0; len > 0;len -= 12, i++)
    {
        zlog_debug (OSPF_DBG_DETAIL, "    LS ID %s", inet_ntoa (rl->link[i].link_id));
        zlog_debug (OSPF_DBG_DETAIL, "    Link Data %s", inet_ntoa (rl->link[i].link_data));
        zlog_debug (OSPF_DBG_DETAIL, "    Type %d", (u_char) rl->link[i].type);
        zlog_debug (OSPF_DBG_DETAIL, "    TOS %d", (u_char) rl->link[i].tos);
        zlog_debug (OSPF_DBG_DETAIL, "    metric %d", ntohs (rl->link[i].metric));
        //len -= 12;
    }
}

static void
ospf_network_lsa_dump (struct stream *s, u_int16_t length)
{
    struct network_lsa *nl;
    int i, cnt;
    nl = (struct network_lsa *) STREAM_PNT (s);
    cnt = (ntohs (nl->header.length) - (OSPF_LSA_HEADER_SIZE + 4)) / 4;
    zlog_debug (OSPF_DBG_DETAIL, "  Network-LSA");
    /*
    zlog_debug ("LSA total size %d", ntohs (nl->header.length));
    zlog_debug ("Network-LSA size %d",
    ntohs (nl->header.length) - OSPF_LSA_HEADER_SIZE);
    */
    zlog_debug (OSPF_DBG_DETAIL, "    Network Mask %s", inet_ntoa (nl->mask));
    zlog_debug (OSPF_DBG_DETAIL, "    # Attached Routers %d", cnt);
    for (i = 0; i < cnt; i++)
    {
        zlog_debug (OSPF_DBG_DETAIL, "      Attached Router %s", inet_ntoa (nl->routers[i]));
    }
}

static void
ospf_summary_lsa_dump (struct stream *s, u_int16_t length)
{
    struct summary_lsa *sl;
    int size;
    sl = (struct summary_lsa *) STREAM_PNT (s);
    zlog_debug (OSPF_DBG_DETAIL, "  Summary-LSA");
    zlog_debug (OSPF_DBG_DETAIL, "    Network Mask %s", inet_ntoa (sl->mask));
    size = ntohs (sl->header.length) - (OSPF_LSA_HEADER_SIZE + 4U);
    for (; size > 0; size -= 4)
    {
        zlog_debug (OSPF_DBG_DETAIL, "    TOS=%d metric %d", sl->tos,
                    GET_METRIC (sl->metric));
    }
}

static void
ospf_as_external_lsa_dump (struct stream *s, u_int16_t length)
{
    struct as_external_lsa *al;
    int size;
    int i;
    al = (struct as_external_lsa *) STREAM_PNT (s);
    zlog_debug (OSPF_DBG_DETAIL, "  %s", ospf_lsa_type_msg[al->header.type].str);
    zlog_debug (OSPF_DBG_DETAIL, "    Network Mask %s", inet_ntoa (al->mask));
    size = ntohs (al->header.length) - (OSPF_LSA_HEADER_SIZE + 4U);
    for (i = 0; size > 0; size -= 12, i++)
    {
        zlog_debug (OSPF_DBG_DETAIL, "    bit %s TOS=%d metric %d",
                    IS_EXTERNAL_METRIC (al->e[i].tos) ? "E" : "-",
                    al->e[i].tos & 0x7f, GET_METRIC (al->e[i].metric));
        zlog_debug (OSPF_DBG_DETAIL, "    Forwarding address %s", inet_ntoa (al->e[i].fwd_addr));
        zlog_debug(OSPF_DBG_DETAIL, "    External Route Tag %u", al->e[i].route_tag);
    }
}

static void
ospf_lsa_header_list_dump (struct stream *s, u_int16_t length)
{
    struct lsa_header *lsa;
    zlog_debug (OSPF_DBG_DETAIL, "  # LSA Headers %d", length / OSPF_LSA_HEADER_SIZE);
    /* LSA Headers. */
    while (length > 0)
    {
        lsa = (struct lsa_header *) STREAM_PNT (s);
        ospf_lsa_header_dump (lsa);
        stream_forward_getp (s, OSPF_LSA_HEADER_SIZE);
        length -= OSPF_LSA_HEADER_SIZE;
    }
}

static void
ospf_packet_db_desc_dump (struct stream *s, u_int16_t length)
{
    struct ospf_db_desc *dd;
    char dd_flags[8];
    u_int32_t gp;
    gp = stream_get_getp (s);
    dd = (struct ospf_db_desc *) STREAM_PNT (s);
    zlog_debug (OSPF_DBG_DETAIL, "Database Description");
    zlog_debug (OSPF_DBG_DETAIL, "  Interface MTU %d", ntohs (dd->mtu));
    zlog_debug (OSPF_DBG_DETAIL, "  Options %d (%s)", dd->options,
                ospf_options_dump (dd->options));
    zlog_debug (OSPF_DBG_DETAIL, "  Flags %d (%s)", dd->flags,
                ospf_dd_flags_dump (dd->flags, dd_flags, sizeof dd_flags));
    zlog_debug (OSPF_DBG_DETAIL, "  Sequence Number 0x%08lx", (u_long)ntohl (dd->dd_seqnum));
    length -= OSPF_HEADER_SIZE + OSPF_DB_DESC_MIN_SIZE;
    stream_forward_getp (s, OSPF_DB_DESC_MIN_SIZE);
    ospf_lsa_header_list_dump (s, length);
    stream_set_getp (s, gp);
}

static void
ospf_packet_ls_req_dump (struct stream *s, u_int16_t length)
{
    u_int32_t sp;
    u_int32_t ls_type;
    struct in_addr ls_id;
    struct in_addr adv_router;
    sp = stream_get_getp (s);
    length -= OSPF_HEADER_SIZE;
    zlog_debug (OSPF_DBG_DETAIL, "Link State Request");
    zlog_debug (OSPF_DBG_DETAIL, "  # Requests %d", length / 12);
    for (; length > 0; length -= 12)
    {
        ls_type = stream_getl (s);
        ls_id.s_addr = stream_get_ipv4 (s);
        adv_router.s_addr = stream_get_ipv4 (s);
        zlog_debug (OSPF_DBG_DETAIL, "  LS type %d", ls_type);
        zlog_debug (OSPF_DBG_DETAIL, "  Link State ID %s", inet_ntoa (ls_id));
        zlog_debug (OSPF_DBG_DETAIL, "  Advertising Router %s",
                    inet_ntoa (adv_router));
    }
    stream_set_getp (s, sp);
}

static void
ospf_packet_ls_upd_dump (struct stream *s, u_int16_t length)
{
    u_int32_t sp;
    struct lsa_header *lsa;
    int lsa_len;
    u_int32_t count;
    length -= OSPF_HEADER_SIZE;
    sp = stream_get_getp (s);
    count = stream_getl (s);
    length -= 4;
    zlog_debug (OSPF_DBG_DETAIL, "Link State Update");
    zlog_debug (OSPF_DBG_DETAIL, "  # LSAs %d", count);
    while (length > 0 && count > 0)
    {
        if (length < OSPF_HEADER_SIZE || length % 4 != 0)
        {
            zlog_debug (OSPF_DBG_DETAIL, "  Remaining %d bytes; Incorrect length.", length);
            break;
        }
        lsa = (struct lsa_header *) STREAM_PNT (s);
        lsa_len = ntohs (lsa->length);
        ospf_lsa_header_dump (lsa);
        switch (lsa->type)
        {
        case OSPF_ROUTER_LSA:
            ospf_router_lsa_dump (s, length);
            break;
        case OSPF_NETWORK_LSA:
            ospf_network_lsa_dump (s, length);
            break;
        case OSPF_SUMMARY_LSA:
        case OSPF_ASBR_SUMMARY_LSA:
            ospf_summary_lsa_dump (s, length);
            break;
        case OSPF_AS_EXTERNAL_LSA:
            ospf_as_external_lsa_dump (s, length);
            break;
        case OSPF_AS_NSSA_LSA:
            ospf_as_external_lsa_dump (s, length);
            break;
        case OSPF_OPAQUE_LINK_LSA:
        case OSPF_OPAQUE_AREA_LSA:
        case OSPF_OPAQUE_AS_LSA:
            ospf_opaque_lsa_dump (s, length);
            break;
        default:
            break;
        }
        stream_forward_getp (s, lsa_len);
        length -= lsa_len;
        count--;
    }
    stream_set_getp (s, sp);
}

static void
ospf_packet_ls_ack_dump (struct stream *s, u_int16_t length)
{
    u_int32_t sp;
    length -= OSPF_HEADER_SIZE;
    sp = stream_get_getp (s);
    zlog_debug (OSPF_DBG_DETAIL, "Link State Acknowledgment");
    ospf_lsa_header_list_dump (s, length);
    stream_set_getp (s, sp);
}

/* Expects header to be in host order */
void
ospf_ip_header_dump (struct ip *iph)
{
    /* IP Header dump. */
    zlog_debug (OSPF_DBG_DETAIL, "ip_v %d", iph->ip_v);
    zlog_debug (OSPF_DBG_DETAIL, "ip_hl %d", iph->ip_hl);
    zlog_debug (OSPF_DBG_DETAIL, "ip_tos %d", iph->ip_tos);
    zlog_debug (OSPF_DBG_DETAIL, "ip_len %d", iph->ip_len);
    zlog_debug (OSPF_DBG_DETAIL, "ip_id %u", (u_int32_t) iph->ip_id);
    zlog_debug (OSPF_DBG_DETAIL, "ip_off %u", (u_int32_t) iph->ip_off);
    zlog_debug (OSPF_DBG_DETAIL, "ip_ttl %d", iph->ip_ttl);
    zlog_debug (OSPF_DBG_DETAIL, "ip_p %d", iph->ip_p);
    zlog_debug (OSPF_DBG_DETAIL, "ip_sum 0x%x", (u_int32_t) iph->ip_sum);
    zlog_debug (OSPF_DBG_DETAIL, "ip_src %s",  inet_ntoa (iph->ip_src));
    zlog_debug (OSPF_DBG_DETAIL, "ip_dst %s", inet_ntoa (iph->ip_dst));
}

void
ospf_ip_header_recv_dump (struct ip_control *ipcb)
{
    /* IP Header dump. */
    char ip[INET_ADDRSTRLEN] = "";
    //zlog_debug (OSPF_DBG_RECV, "ip_v %d", iph->ip_v);
    //zlog_debug (OSPF_DBG_RECV, "ip_hl %d", iph->ip_hl);
    zlog_debug (OSPF_DBG_RECV, "ip_tos %d", ipcb->tos);
    //zlog_debug (OSPF_DBG_RECV, "ip_len %d", iph->ip_len);
    //zlog_debug (OSPF_DBG_RECV, "ip_id %u", (u_int32_t) iph->ip_id);
    zlog_debug (OSPF_DBG_RECV, "ip_off %u", (u_int32_t) ipcb->frag_off);
    zlog_debug (OSPF_DBG_RECV, "ip_ttl %d", ipcb->ttl);
    zlog_debug (OSPF_DBG_RECV, "ip_p %d", ipcb->protocol);
    //zlog_debug (OSPF_DBG_RECV, "ip_sum 0x%x", (u_int32_t) iph->ip_sum);
    zlog_debug (OSPF_DBG_RECV, "ip_src %s",  inet_ipv4tostr(ipcb->sip, ip));
    zlog_debug (OSPF_DBG_RECV, "ip_dst %s", inet_ipv4tostr(ipcb->dip, ip));
}


static void
ospf_header_dump (struct ospf_header *ospfh)
{
    char buf[9];
    u_int16_t auth_type = ntohs (ospfh->auth_type);
    zlog_debug (OSPF_DBG_DETAIL, "Header");
    zlog_debug (OSPF_DBG_DETAIL, "  Version %d", ospfh->version);
    zlog_debug (OSPF_DBG_DETAIL, "  Type %d (%s)", ospfh->type,
                LOOKUP (ospf_packet_type_str, ospfh->type));
    zlog_debug (OSPF_DBG_DETAIL, "  Packet Len %d", ntohs (ospfh->length));
    zlog_debug (OSPF_DBG_DETAIL, "  Router ID %s", inet_ntoa (ospfh->router_id));
    zlog_debug (OSPF_DBG_DETAIL, "  Area ID %s", inet_ntoa (ospfh->area_id));
    zlog_debug (OSPF_DBG_DETAIL, "  Checksum 0x%x", ntohs (ospfh->checksum));
    zlog_debug (OSPF_DBG_DETAIL, "  AuType %s", LOOKUP (ospf_auth_type_str, auth_type));
    switch (auth_type)
    {
    case OSPF_AUTH_NULL:
        break;
    case OSPF_AUTH_SIMPLE:
        memset (buf, 0, 9);
        strncpy (buf, (char *) ospfh->u.auth_data, 8);
        zlog_debug (OSPF_DBG_DETAIL, "  Simple Password %s", buf);
        break;
    case OSPF_AUTH_CRYPTOGRAPHIC:
        zlog_debug (OSPF_DBG_DETAIL, "  Cryptographic Authentication");
        zlog_debug (OSPF_DBG_DETAIL, "  Key ID %d", ospfh->u.crypt.key_id);
        zlog_debug (OSPF_DBG_DETAIL, "  Auth Data Len %d", ospfh->u.crypt.auth_data_len);
        zlog_debug (OSPF_DBG_DETAIL, "  Sequence number %ld",
                    (u_long)ntohl (ospfh->u.crypt.crypt_seqnum));
        break;
    default:
        zlog_debug (OSPF_DBG_DETAIL, "* This is not supported authentication type");
        break;
    }
}

void
ospf_packet_dump (struct stream *s)
{
    struct ospf_header *ospfh;
    unsigned long gp;
    /* Preserve pointer. */
    gp = stream_get_getp (s);
    /* OSPF Header dump. */
    ospfh = (struct ospf_header *) STREAM_PNT (s);
    /* Until detail flag is set, return. */
    if (!(term_debug_ospf_packet[ospfh->type - 1] & OSPF_DEBUG_DETAIL))
    {
        return;
    }
    /* Show OSPF header detail. */
    ospf_header_dump (ospfh);
    stream_forward_getp (s, OSPF_HEADER_SIZE);
    switch (ospfh->type)
    {
    case OSPF_MSG_HELLO:
        ospf_packet_hello_dump (s, ntohs (ospfh->length));
        break;
    case OSPF_MSG_DB_DESC:
        ospf_packet_db_desc_dump (s, ntohs (ospfh->length));
        break;
    case OSPF_MSG_LS_REQ:
        ospf_packet_ls_req_dump (s, ntohs (ospfh->length));
        break;
    case OSPF_MSG_LS_UPD:
        ospf_packet_ls_upd_dump (s, ntohs (ospfh->length));
        break;
    case OSPF_MSG_LS_ACK:
        ospf_packet_ls_ack_dump (s, ntohs (ospfh->length));
        break;
    default:
        break;
    }
    stream_set_getp (s, gp);
}



DEFUN (debug_ospf_packet,
       debug_ospf_packet_all_cmd,
       "debug ospf (enable|disable) packet (hello|dd|ls-request|ls-update|ls-ack|all)",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF packets\n"
       "OSPF Hello\n"
       "OSPF Database Description\n"
       "OSPF Link State Request\n"
       "OSPF Link State Update\n"
       "OSPF Link State Acknowledgment\n"
       "OSPF all packets\n")
{
	int i = 0;
    int type = 0;
    int flag = 0;
	int enable = 0;
    unsigned int type_new = 0;

	if(argc <= 0)
    {
        zlog_warn("%s[%d] argc <= 0",__func__,__LINE__);
        return CMD_WARNING;
    }
    assert (argc > 0);

	if(argv[0][0] == 'e') enable = 1;
	
    /* Check packet type. */
    if (strncmp (argv[1], "h", 1) == 0)
    {
        type = OSPF_DEBUG_HELLO;
		type_new = OSPF_DBG_HELLO;
    }
    else if (strncmp (argv[1], "d", 1) == 0)
    {
        type = OSPF_DEBUG_DB_DESC;
		type_new = OSPF_DBG_DB_DESC;
    }
    else if (strncmp (argv[1], "ls-r", 4) == 0)
    {
        type = OSPF_DEBUG_LS_REQ;
		type_new = OSPF_DBG_LS_REQ;
    }
    else if (strncmp (argv[1], "ls-u", 4) == 0)
    {
        type = OSPF_DEBUG_LS_UPD;
		type_new = OSPF_DBG_LS_UPD;
    }
    else if (strncmp (argv[1], "ls-a", 4) == 0)
    {
        type = OSPF_DEBUG_LS_ACK;
		type_new = OSPF_DBG_LS_ACK;
    }
    else if (strncmp (argv[1], "a", 1) == 0)
    {
        type = OSPF_DEBUG_ALL;
		type_new = (OSPF_DBG_HELLO |\
					OSPF_DBG_DB_DESC |\
					OSPF_DBG_LS_REQ |\
					OSPF_DBG_LS_UPD |\
					OSPF_DBG_LS_ACK);
    }
    /* Default, both send and recv. */
    if (argc == 2)
    {
		
		if(enable)
		{
			type_new |= (OSPF_DBG_SEND | OSPF_DBG_RECV);
			flag = OSPF_DEBUG_SEND | OSPF_DEBUG_RECV;
		}
		else
		{
			//type_new |= OSPF_DBG_SEND | OSPF_DBG_RECV | OSPF_DBG_DETAIL;
			if(type == OSPF_DEBUG_ALL)
			{
				type_new |= OSPF_DBG_SEND | OSPF_DBG_RECV | OSPF_DBG_DETAIL;
			}
			
			flag = OSPF_DEBUG_SEND | OSPF_DEBUG_RECV | OSPF_DEBUG_DETAIL ;
		}
    }
    /* send or recv. */
    if (argc >= 3)
    {
        if (strncmp (argv[2], "s", 1) == 0)
        {
			
			if(enable)
			{
				type_new |= OSPF_DBG_SEND ;
				flag = OSPF_DEBUG_SEND;
			}
			else
			{
				//type_new |= OSPF_DBG_SEND;//| OSPF_DBG_DETAIL
				flag = OSPF_DEBUG_SEND | OSPF_DEBUG_DETAIL;
			}
        }
        else if (strncmp (argv[2], "r", 1) == 0)
        {
			
			if(enable)
			{
				type_new |= OSPF_DBG_RECV;
				flag = OSPF_DEBUG_RECV;
			}
			else
			{
				//type_new |= OSPF_DBG_RECV;// | OSPF_DBG_DETAIL
				flag = OSPF_DEBUG_RECV | OSPF_DEBUG_DETAIL;
			}
        }
        else if (strncmp (argv[2], "d", 1) == 0)
        {
			
			if(enable)
			{
				type_new |= (OSPF_DBG_SEND | OSPF_DBG_RECV | OSPF_DBG_DETAIL);
				flag = OSPF_DEBUG_SEND | OSPF_DEBUG_RECV | OSPF_DEBUG_DETAIL;
			}
			else
			{
				//type_new |= OSPF_DBG_SEND | OSPF_DBG_RECV;// | OSPF_DBG_DETAIL
				if(type == OSPF_DEBUG_ALL)
				{
					type_new |= OSPF_DBG_SEND | OSPF_DBG_RECV | OSPF_DBG_DETAIL;
				}
				flag = OSPF_DEBUG_SEND | OSPF_DEBUG_RECV | OSPF_DEBUG_DETAIL;
			}
        }
    }
    /* detail. */
    if (argc == 4)
    {
        if (strncmp (argv[3], "d", 1) == 0)
        {
			
			if(enable)
			{
				type_new |= OSPF_DBG_DETAIL;
				flag |= OSPF_DEBUG_DETAIL;
			}
			else
			{
				//type_new = OSPF_DBG_DETAIL;
				if(type == OSPF_DEBUG_ALL)
				{
					type_new = OSPF_DBG_DETAIL;
				}
				flag = OSPF_DEBUG_DETAIL;
			}
        }
    }
    for (i = 0; i < 5; i++)
    {
        if (type & (0x01 << i))
        {
        	if(enable)
       		{
      		  	TERM_DEBUG_PACKET_ON(i, flag);
       		}
			else
			{
				TERM_DEBUG_PACKET_OFF (i, flag);
				if(type != OSPF_DEBUG_ALL)//关debug时，使用原有的debug条件来约束，因为新的debug不够精细
				{
					type_new = 0;
				}
			}
			
			if(type_new)
			{
				zlog_debug_set(vty, type_new,  enable);
			}
        }
    }
    return CMD_SUCCESS;
}

ALIAS (debug_ospf_packet,
       debug_ospf_packet_send_recv_cmd,
       "debug ospf (enable|disable) packet (hello|dd|ls-request|ls-update|ls-ack|all) (send|recv|detail)",
       "Debugging functions\n"
       "OSPF information\n"
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF packets\n"
       "OSPF Hello\n"
       "OSPF Database Description\n"
       "OSPF Link State Request\n"
       "OSPF Link State Update\n"
       "OSPF Link State Acknowledgment\n"
       "OSPF all packets\n"
       "Packet sent\n"
       "Packet received\n"
       "Detail information\n")

ALIAS (debug_ospf_packet,
       debug_ospf_packet_send_recv_detail_cmd,
       "debug ospf (enable|disable) packet (hello|dd|ls-request|ls-update|ls-ack|all) (send|recv) (detail|)",
       "Debugging functions\n"
       "OSPF information\n"
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF packets\n"
       "OSPF Hello\n"
       "OSPF Database Description\n"
       "OSPF Link State Request\n"
       "OSPF Link State Update\n"
       "OSPF Link State Acknowledgment\n"
       "OSPF all packets\n"
       "Packet sent\n"
       "Packet received\n"
       "Detail Information\n")


DEFUN (debug_ospf_ism,
       debug_ospf_ism_cmd,
       "debug ospf (enable|disable) ifsm (events|timers)",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF Interface State Machine\n"
       "ISM Event Information\n"
       "ISM TImer Information\n")
{
	int enable = 0;
    unsigned int type_new = 0;

	if (argc <= 0)
	{
		return CMD_WARNING;
	}
	
	if(argv[0][0] == 'e') enable = 1;

    if (strncmp (argv[1], "e", 1) == 0)
    {
		type_new = OSPF_DBG_ISM_EVENTS;
    }
    else if (strncmp (argv[1], "t", 1) == 0)
    {
		type_new = OSPF_DBG_ISM_TIMERS;
    }
	
	if(type_new)
	{
		zlog_debug_set(vty, type_new,  enable);
	}
    return CMD_SUCCESS;  
}


DEFUN (debug_ospf_nsm,
       	debug_ospf_nsm_cmd,
		"debug ospf (enable|disable) nfsm (events|timers)",
		DEBUG_STR
		OSPF_STR
		"Ospf debug enable\n"
		"Ospf debug disable\n"
		"OSPF Neighbor State Machine\n"
		"NSM Event Information\n"
		"NSM Timer Information\n")
{
	int enable = 0;
    unsigned int type_new = 0;
	
	if (argc <= 0)
	{
		return CMD_WARNING;
	}
	
	if(argv[0][0] == 'e') enable = 1;
	
   	if (strncmp (argv[1], "e", 1) == 0)
    {
		type_new = OSPF_DBG_NSM_EVENTS;
    }
    else if (strncmp (argv[1], "t", 1) == 0)
    {
		type_new = OSPF_DBG_NSM_TIMERS;
    }

	if(type_new)
	{
		zlog_debug_set(vty, type_new,  enable);
	}
    return CMD_SUCCESS;
}


DEFUN (debug_ospf_lsa,
       debug_ospf_lsa_cmd,
       "debug ospf (enable|disable) lsa",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF Link State Advertisement\n")
{
	int enable = 0;
	unsigned int type_new = 0;

	if (argc <= 0)
	{
		return CMD_WARNING;
	}

	if(argv[0][0] == 'e') enable = 1;

    if (argc == 1)
    {
		type_new = OSPF_DBG_LSA;
    }
    else if (argc == 2)
    {
        if (strncmp (argv[1], "g", 1) == 0)
        {
			type_new = OSPF_DBG_LSA_GENERATE;
        }
        else if (strncmp (argv[1], "f", 1) == 0)
        {
			type_new = OSPF_DBG_LSA_FLOODING;
        }
        else if (strncmp (argv[1], "r", 1) == 0)
        {
			type_new = OSPF_DBG_LSA_REFRESH;
        }
    }
	if(type_new)
	{
		zlog_debug_set(vty, type_new,  enable);
	}
    return CMD_SUCCESS;
}

ALIAS (debug_ospf_lsa,
       debug_ospf_lsa_sub_cmd,
       "debug ospf (enable|disable) lsa (generate|flooding|refresh)",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF Link State Advertisement\n"
       "LSA Generation\n"
       "LSA Flooding\n"
       "LSA Refresh\n")


DEFUN (debug_ospf_rib,
       debug_ospf_rib_cmd,
       "debug ospf (enable|disable) msg (interface|redistribute)",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF msg information\n"
       "rib interface\n"
       "rib redistribute\n")
{
	
	int enable = 0;
	unsigned int type_new = 0;

	if (argc <= 0)
	{
		return CMD_WARNING;
	}

	if(argv[0][0] == 'e') enable = 1;
	
    if (strncmp (argv[1], "i", 1) == 0)
    {
		type_new = OSPF_DBG_ZEBRA_INTERFACE;
    }
    else if (strncmp (argv[1], "r", 1) == 0)
    {
		type_new = OSPF_DBG_ZEBRA_REDISTRIBUTE;
    }

	if(type_new)
	{
		zlog_debug_set(vty, type_new,  enable);
	}
    return CMD_SUCCESS;   
}


DEFUN (debug_ospf_event,
       debug_ospf_event_cmd,
       "debug ospf (enable|disable) event",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF event information\n")
{
	int enable = 0;
	unsigned int type_new = 0;

	if (argc <= 0)
	{
		return CMD_WARNING;
	}

	if(argv[0][0] == 'e') enable = 1;
	
	type_new = OSPF_DBG_EVENT;
	zlog_debug_set(vty, type_new,  enable);
		
    return CMD_SUCCESS;
}


DEFUN (debug_ospf_nssa,
       debug_ospf_nssa_cmd,
       "debug ospf (enable|disable) nssa",
       DEBUG_STR
       OSPF_STR
       "Ospf debug enable\n"
       "Ospf debug disable\n"
       "OSPF nssa information\n")
{
	int enable = 0;
	unsigned int type_new = 0;

	if (argc <= 0)
	{
		return CMD_WARNING;
	}

	if(argv[0][0] == 'e') enable = 1;
	
	type_new = OSPF_DBG_NSSA;
	zlog_debug_set(vty, type_new,  enable);
		
    return CMD_SUCCESS;
}


DEFUN (debug_ospf_dcn,
	  debug_ospf_dcn_cmd,
	  "debug ospf (enable|disable) dcn",
	  DEBUG_STR
	  OSPF_STR
	  "Ospf debug enable\n"
      "Ospf debug disable\n"
	  "OSPF dcn information\n")
{
	int enable = 0;
	unsigned int type_new = 0;

	if (argc <= 0)
	{
		return CMD_WARNING;
	}

	if(argv[0][0] == 'e') enable = 1;
	
	type_new = OSPF_DBG_DCN;
	zlog_debug_set(vty, type_new,  enable);
	return CMD_SUCCESS;
}

DEFUN (debug_ospf_bfd,
	debug_ospf_bfd_cmd,
	"debug ospf (enable|disable) bfd",
	DEBUG_STR
	OSPF_STR
	"Ospf debug enable\n"
    "Ospf debug disable\n"
	"OSPF bfd information\n")
{
	int enable = 0;
	unsigned int type_new = 0;

	if (argc <= 0)
	{
		return CMD_WARNING;
	}

	if(argv[0][0] == 'e') enable = 1;
	
	type_new = OSPF_DBG_BFD;
	zlog_debug_set(vty, type_new,  enable);
	return CMD_SUCCESS;
}

DEFUN (debug_ospf_thread,
		debug_ospf_other_cmd,
		"debug ospf (enable|disable) other ",
		DEBUG_STR
		OSPF_STR
		"Ospf debug enable\n"
		"Ospf debug disable\n"
		"Thread output information\n")
{
	int enable = 0;
	unsigned int type_new = 0;

	if (argc <= 0)
	{
		return CMD_WARNING;
	}

	if(argv[0][0] == 'e') enable = 1;
	
	type_new = OSPF_DBG_OTHER;
	zlog_debug_set(vty, type_new,  enable);

	return CMD_SUCCESS;
}


DEFUN (show_debugging_ospf,
       show_debugging_ospf_cmd,
       "show ospf debug",
       SHOW_STR
       OSPF_STR
       DEBUG_STR)
{
    int i;
    vty_out (vty, "OSPF debugging status:%s", VTY_NEWLINE);
    /* Show debug status for events. */
    if (vty->monitor & OSPF_DBG_EVENT)
    {
        vty_out (vty, "  OSPF event debugging is on%s", VTY_NEWLINE);
    }
    /* Show debug status for ISM. */
    if (vty->monitor & OSPF_DBG_ISM_EVENTS)
    {
        vty_out (vty, "  OSPF ISM event debugging is on%s", VTY_NEWLINE);
    }
    if (vty->monitor & OSPF_DBG_ISM_TIMERS)
    {
        vty_out (vty, "  OSPF ISM timer debugging is on%s", VTY_NEWLINE);
    }
    
    /* Show debug status for NSM. */
    if (vty->monitor & OSPF_DBG_NSM_EVENTS)
    {
        vty_out (vty, "  OSPF NSM event debugging is on%s", VTY_NEWLINE);
    }
    if (vty->monitor & OSPF_DBG_NSM_TIMERS)
    {
        vty_out (vty, "  OSPF NSM timer debugging is on%s", VTY_NEWLINE);
    }
	
    /* Show debug status for OSPF Packets. */
    /*for (i = 0; i < 5; i++)
    {
		if(vty->monitor & (1 << (18+i)))//start with hello
		{
	        if ((vty->monitor & OSPF_DBG_SEND) && (vty->monitor & OSPF_DBG_RECV))
	        {
            	vty_out (vty, "  OSPF packet %s%s debugging is on%s",
                     LOOKUP (ospf_packet_type_str, i + 1),
                     (vty->monitor & OSPF_DBG_DETAIL) ? " detail" : "",
                     VTY_NEWLINE);
	        }
	        else
	        {
	            if (vty->monitor & OSPF_DBG_SEND)
	            {
                	vty_out (vty, "  OSPF packet %s send%s debugging is on%s",
                         LOOKUP (ospf_packet_type_str, i + 1),
                         (vty->monitor & OSPF_DBG_DETAIL) ? " detail" : "",
                         VTY_NEWLINE);
	            }
	            if ((vty->monitor & OSPF_DBG_RECV))
				{
                	vty_out (vty, "  OSPF packet %s receive%s debugging is on%s",
                         LOOKUP (ospf_packet_type_str, i + 1),
                         (vty->monitor & OSPF_DBG_DETAIL) ? " detail" : "",
                         VTY_NEWLINE);
				}

	        }
		}
    }*/
	for (i = 0; i < 5; i++)
    {
        if (IS_DEBUG_OSPF_PACKET (i, SEND) && IS_DEBUG_OSPF_PACKET (i, RECV))
        {
            vty_out (vty, "  OSPF packet %s%s debugging is on%s",
                     LOOKUP (ospf_packet_type_str, i + 1),
                     IS_DEBUG_OSPF_PACKET (i, DETAIL) ? " detail" : "",
                     VTY_NEWLINE);
        }
        else
        {
            if (IS_DEBUG_OSPF_PACKET (i, SEND))
                vty_out (vty, "  OSPF packet %s send%s debugging is on%s",
                         LOOKUP (ospf_packet_type_str, i + 1),
                         IS_DEBUG_OSPF_PACKET (i, DETAIL) ? " detail" : "",
                         VTY_NEWLINE);
            if (IS_DEBUG_OSPF_PACKET (i, RECV))
                vty_out (vty, "  OSPF packet %s receive%s debugging is on%s",
                         LOOKUP (ospf_packet_type_str, i + 1),
                         IS_DEBUG_OSPF_PACKET (i, DETAIL) ? " detail" : "",
                         VTY_NEWLINE);
        }
    }
    /* Show debug status for OSPF LSAs. */
    if (vty->monitor & OSPF_DBG_LSA)
    {
        vty_out (vty, "  OSPF LSA debugging is on%s", VTY_NEWLINE);
    }
  
    if (vty->monitor & OSPF_DBG_LSA_GENERATE)
    {
        vty_out (vty, "  OSPF LSA generation debugging is on%s", VTY_NEWLINE);
    }
    if (vty->monitor & OSPF_DBG_LSA_FLOODING)
    {
        vty_out (vty, "  OSPF LSA flooding debugging is on%s", VTY_NEWLINE);
    }
    if (vty->monitor & OSPF_DBG_LSA_REFRESH)
    {
        vty_out (vty, "  OSPF LSA refresh debugging is on%s", VTY_NEWLINE);
    }
   
    /* Show debug status for Zebra. */
    if (vty->monitor & OSPF_DBG_ZEBRA_INTERFACE)
    {
        vty_out (vty, "  OSPF Zebra interface debugging is on%s", VTY_NEWLINE);
    }
    if (vty->monitor & OSPF_DBG_ZEBRA_REDISTRIBUTE)
    {
        vty_out (vty, "  OSPF Zebra redistribute debugging is on%s", VTY_NEWLINE);
    }

    /* Show debug status for NSSA. */
    if (vty->monitor & OSPF_DBG_NSSA)
    {
        vty_out (vty, "  OSPF NSSA debugging is on%s", VTY_NEWLINE);
    }
	/* Show debug status for OSPF DCN. */
    if (vty->monitor & OSPF_DBG_DCN)
    {
        vty_out (vty, "  OSPF DCN debugging is on%s", VTY_NEWLINE);
    }
	/* Show debug status for OSPF BFD. */
    if (vty->monitor & OSPF_DBG_BFD)
    {
        vty_out (vty, "  OSPF BFD debugging is on%s", VTY_NEWLINE);
    }
    return CMD_SUCCESS;
}
		

/* Debug node. */
static struct cmd_node debug_node =
{
    DEBUG_NODE,
    "",
    1 /* VTYSH */
};
	
#if 0
static int
ospf_config_write_debug (struct vty *vty)
{
    int write = 0;
    int i, r;
    const char *type_str[] = {"hello", "dd", "ls-request", "ls-update", "ls-ack"};
    const char *detail_str[] = {"", " send", " recv", "", " detail",
                                " send detail", " recv detail", " detail"
                               };
    /* debug ospf ism (status|events|timers). */
    if (IS_CONF_DEBUG_OSPF (ism, ISM) == OSPF_DEBUG_ISM)
    {
        vty_out (vty, "debug ospf ifsm%s", VTY_NEWLINE);
    }
    else
    {
        if (IS_CONF_DEBUG_OSPF (ism, ISM_STATUS))
        {
            vty_out (vty, "debug ospf ifsm status%s", VTY_NEWLINE);
        }
        if (IS_CONF_DEBUG_OSPF (ism, ISM_EVENTS))
        {
            vty_out (vty, "debug ospf ifsm event%s", VTY_NEWLINE);
        }
        if (IS_CONF_DEBUG_OSPF (ism, ISM_TIMERS))
        {
            vty_out (vty, "debug ospf ifsm timer%s", VTY_NEWLINE);
        }
    }
    /* debug ospf nsm (status|events|timers). */
    if (IS_CONF_DEBUG_OSPF (nsm, NSM) == OSPF_DEBUG_NSM)
    {
        vty_out (vty, "debug ospf nfsm%s", VTY_NEWLINE);
    }
    else
    {
        if (IS_CONF_DEBUG_OSPF (nsm, NSM_STATUS))
        {
            vty_out (vty, "debug ospf nfsm status%s", VTY_NEWLINE);
        }
        if (IS_CONF_DEBUG_OSPF (nsm, NSM_EVENTS))
        {
            vty_out (vty, "debug ospf nfsm event%s", VTY_NEWLINE);
        }
        if (IS_CONF_DEBUG_OSPF (nsm, NSM_TIMERS))
        {
            vty_out (vty, "debug ospf nfsm timer%s", VTY_NEWLINE);
        }
    }
    /* debug ospf lsa (generate|flooding|install|refresh). */
    if (IS_CONF_DEBUG_OSPF (lsa, LSA) == OSPF_DEBUG_LSA)
    {
        vty_out (vty, "debug ospf lsa%s", VTY_NEWLINE);
    }
    else
    {
        if (IS_CONF_DEBUG_OSPF (lsa, LSA_GENERATE))
        {
            vty_out (vty, "debug ospf lsa generate%s", VTY_NEWLINE);
        }
        if (IS_CONF_DEBUG_OSPF (lsa, LSA_FLOODING))
        {
            vty_out (vty, "debug ospf lsa flooding%s", VTY_NEWLINE);
        }
        if (IS_CONF_DEBUG_OSPF (lsa, LSA_INSTALL))
        {
            vty_out (vty, "debug ospf lsa install%s", VTY_NEWLINE);
        }
        if (IS_CONF_DEBUG_OSPF (lsa, LSA_REFRESH))
        {
            vty_out (vty, "debug ospf lsa refresh%s", VTY_NEWLINE);
        }
        write = 1;
    }
    /* debug ospf zebra (interface|redistribute). */
    if (IS_CONF_DEBUG_OSPF (zebra, ZEBRA) == OSPF_DEBUG_ZEBRA)
    {
        vty_out (vty, "debug ospf rib%s", VTY_NEWLINE);
    }
    else
    {
        if (IS_CONF_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
        {
            vty_out (vty, "debug ospf rib interface%s", VTY_NEWLINE);
        }
        if (IS_CONF_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
        {
            vty_out (vty, "debug ospf rib redistribute%s", VTY_NEWLINE);
        }
        write = 1;
    }
    /* debug ospf event. */
    if (IS_CONF_DEBUG_OSPF (event, EVENT) == OSPF_DEBUG_EVENT)
    {
        vty_out (vty, "debug ospf event%s", VTY_NEWLINE);
        write = 1;
    }
    /* debug ospf nssa. */
    if (IS_CONF_DEBUG_OSPF (nssa, NSSA) == OSPF_DEBUG_NSSA)
    {
        vty_out (vty, "debug ospf nssa%s", VTY_NEWLINE);
        write = 1;
    }
	/* debug ospf DCN. */
    if (IS_CONF_DEBUG_OSPF (dcn, DCN) == OSPF_DEBUG_DCN)
    {
        vty_out (vty, "debug ospf dcn%s", VTY_NEWLINE);
        write = 1;
    }
	/* debug ospf BFD. */
    if (IS_CONF_DEBUG_OSPF (bfd, BFD) == OSPF_DEBUG_BFD)
    {
        vty_out (vty, "debug ospf bfd%s", VTY_NEWLINE);
        write = 1;
    }
    /* debug ospf packet all detail. */
    r = OSPF_DEBUG_SEND_RECV|OSPF_DEBUG_DETAIL;
    for (i = 0; i < 5; i++)
    {
        r &= conf_debug_ospf_packet[i] & (OSPF_DEBUG_SEND_RECV|OSPF_DEBUG_DETAIL);
    }
    if (r == (OSPF_DEBUG_SEND_RECV|OSPF_DEBUG_DETAIL))
    {
        vty_out (vty, "debug ospf packet all detail%s", VTY_NEWLINE);
        return 1;
    }
    /* debug ospf packet all. */
    r = OSPF_DEBUG_SEND_RECV;
    for (i = 0; i < 5; i++)
    {
        r &= conf_debug_ospf_packet[i] & OSPF_DEBUG_SEND_RECV;
    }
    if (r == OSPF_DEBUG_SEND_RECV)
    {
        vty_out (vty, "debug ospf packet all%s", VTY_NEWLINE);
        for (i = 0; i < 5; i++)
        {
            if (conf_debug_ospf_packet[i] & OSPF_DEBUG_DETAIL)
                vty_out (vty, "debug ospf packet %s detail%s",
                         type_str[i],
                         VTY_NEWLINE);
        }
        return 1;
    }
    /* debug ospf packet (hello|dd|ls-request|ls-update|ls-ack)
       (send|recv) (detail). */
    for (i = 0; i < 5; i++)
    {
        if (conf_debug_ospf_packet[i] == 0)
        {
            continue;
        }
        vty_out (vty, "debug ospf packet %s%s%s",
                 type_str[i], detail_str[conf_debug_ospf_packet[i]],
                 VTY_NEWLINE);
        write = 1;
    }
    return write;
}
#endif
/* Initialize debug commands. */
void
debug_init ()
{
    //install_node (&debug_node, ospf_config_write_debug);
    install_node (&debug_node, NULL);
    install_element (CONFIG_NODE, &show_debugging_ospf_cmd, CMD_LOCAL);
	
    install_element (CONFIG_NODE, &debug_ospf_packet_send_recv_detail_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &debug_ospf_packet_send_recv_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &debug_ospf_packet_all_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &debug_ospf_ism_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &debug_ospf_nsm_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &debug_ospf_lsa_sub_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &debug_ospf_lsa_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &debug_ospf_rib_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &debug_ospf_event_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &debug_ospf_nssa_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &debug_ospf_dcn_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &debug_ospf_bfd_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &debug_ospf_other_cmd, CMD_SYNC);
	
    /*install_element (CONFIG_NODE, &no_debug_ospf_packet_send_recv_detail_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_packet_send_recv_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_packet_all_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_ism_sub_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_ism_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_nsm_sub_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_nsm_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_lsa_sub_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_lsa_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_rib_sub_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_rib_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_event_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_nssa_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_debug_ospf_dcn_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &no_debug_ospf_bfd_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &no_debug_ospf_thread_cmd, CMD_SYNC);*/
}


