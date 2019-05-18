/*
 * Copyright (C) 2003 Yasuhiro Ohara
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

#include "memory.h"
#include "log.h"
#include "vty.h"
#include "command.h"
#include "thread.h"
#include "linklist.h"
#include "timer.h"
#include "checksum.h"
#include "pkt_buffer.h"
#include "msg_ipc.h"
#include "ftm/pkt_eth.h"
#include "ftm/pkt_ip.h"
#include "ftm/pkt_ipv6.h"
#include "prefix.h"


#include "ospf6_proto.h"
#include "ospf6_lsa.h"
#include "ospf6_lsdb.h"
#include "ospf6_message.h"

#include "ospf6_top.h"
#include "ospf6_area.h"
#include "ospf6_neighbor.h"
#include "ospf6_interface.h"

/* for structures and macros ospf6_lsa_examin() needs */
#include "ospf6_abr.h"
#include "ospf6_asbr.h"
#include "ospf6_intra.h"

#include "ospf6_flood.h"
#include "ospf6d.h"
#include "ospf6d/ospf6_debug.h"


#include <netinet/ip6.h>

unsigned char conf_debug_ospf6_message[6] = {0x03, 0, 0, 0, 0, 0};
static const struct message ospf6_message_type_str [] =
{
    { OSPF6_MESSAGE_TYPE_HELLO,    "Hello"    },
    { OSPF6_MESSAGE_TYPE_DBDESC,   "DbDesc"   },
    { OSPF6_MESSAGE_TYPE_LSREQ,    "LSReq"    },
    { OSPF6_MESSAGE_TYPE_LSUPDATE, "LSUpdate" },
    { OSPF6_MESSAGE_TYPE_LSACK,    "LSAck"    },
};
static const size_t ospf6_message_type_str_max = array_size(ospf6_message_type_str);

/* Minimum (besides the standard OSPF packet header) lengths for OSPF
   packets of particular types, offset is the "type" field. */
const u_int16_t ospf6_packet_minlen[OSPF6_MESSAGE_TYPE_ALL] =
{
    0,
    OSPF6_HELLO_MIN_SIZE,
    OSPF6_DB_DESC_MIN_SIZE,
    OSPF6_LS_REQ_MIN_SIZE,
    OSPF6_LS_UPD_MIN_SIZE,
    OSPF6_LS_ACK_MIN_SIZE
};

/* Minimum (besides the standard LSA header) lengths for LSAs of particular
   types, offset is the "LSA function code" portion of "LSA type" field. */
const u_int16_t ospf6_lsa_minlen[OSPF6_LSTYPE_SIZE] =
{
    0,
    /* 0x2001 */ OSPF6_ROUTER_LSA_MIN_SIZE,
    /* 0x2002 */ OSPF6_NETWORK_LSA_MIN_SIZE,
    /* 0x2003 */ OSPF6_INTER_PREFIX_LSA_MIN_SIZE,
    /* 0x2004 */ OSPF6_INTER_ROUTER_LSA_FIX_SIZE,
    /* 0x4005 */ OSPF6_AS_EXTERNAL_LSA_MIN_SIZE,
    /* 0x2006 */ 0,
    /* 0x2007 */ OSPF6_AS_EXTERNAL_LSA_MIN_SIZE,
    /* 0x0008 */ OSPF6_LINK_LSA_MIN_SIZE,
    /* 0x2009 */ OSPF6_INTRA_PREFIX_LSA_MIN_SIZE
};

/* print functions */

static void
ospf6_header_print(struct ospf6_header *oh)
{
    char router_id[16], area_id[16];
    inet_ntop(AF_INET, &oh->router_id, router_id, sizeof(router_id));
    inet_ntop(AF_INET, &oh->area_id, area_id, sizeof(area_id));
    zlog_debug(OSPF6_DBG_MESSAGE_RECV|OSPF6_DBG_MESSAGE_SEND, "    OSPFv%d Type:%d Len:%hu Router-ID:%s",
               oh->version, oh->type, ntohs(oh->length), router_id);
    zlog_debug(OSPF6_DBG_MESSAGE_RECV|OSPF6_DBG_MESSAGE_SEND, "    Area-ID:%s Cksum:%hx Instance-ID:%d",
               area_id, ntohs(oh->checksum), oh->instance_id);
}

void
ospf6_hello_print(struct ospf6_header *oh)
{
    struct ospf6_hello *hello;
    char options[16];
    char drouter[16], bdrouter[16], neighbor[16];
    char *p;
    ospf6_header_print(oh);
    assert(oh->type == OSPF6_MESSAGE_TYPE_HELLO);
    hello = (struct ospf6_hello *)
            ((caddr_t) oh + sizeof(struct ospf6_header));
    inet_ntop(AF_INET, &hello->drouter, drouter, sizeof(drouter));
    inet_ntop(AF_INET, &hello->bdrouter, bdrouter, sizeof(bdrouter));
    ospf6_options_printbuf(hello->options, options, sizeof(options));
    zlog_debug(OSPF6_DBG_MESSAGE_RECV|OSPF6_DBG_MESSAGE_SEND, "    I/F-Id:%lu Priority:%d Option:%s",
               (u_long) ntohl(hello->interface_id), hello->priority, options);
    zlog_debug(OSPF6_DBG_MESSAGE_RECV|OSPF6_DBG_MESSAGE_SEND, "    HelloInterval:%hu DeadInterval:%hu",
               ntohs(hello->hello_interval), ntohs(hello->dead_interval));
    zlog_debug(OSPF6_DBG_MESSAGE_RECV|OSPF6_DBG_MESSAGE_SEND, "    DR:%s BDR:%s", drouter, bdrouter);

    for (p = (char *)((caddr_t) hello + sizeof(struct ospf6_hello));
            p + sizeof(u_int32_t) <= OSPF6_MESSAGE_END(oh);
            p += sizeof(u_int32_t))
    {
        inet_ntop(AF_INET, (void *) p, neighbor, sizeof(neighbor));
        zlog_debug(OSPF6_DBG_MESSAGE_RECV|OSPF6_DBG_MESSAGE_SEND, "    Neighbor: %s", neighbor);
    }

    assert(p == OSPF6_MESSAGE_END(oh));
}

void
ospf6_dbdesc_print(struct ospf6_header *oh)
{
    struct ospf6_dbdesc *dbdesc;
    char options[16];
    char *p;
    
    ospf6_header_print(oh);
    assert(oh->type == OSPF6_MESSAGE_TYPE_DBDESC);
    dbdesc = (struct ospf6_dbdesc *)
             ((caddr_t) oh + sizeof(struct ospf6_header));
    
    ospf6_options_printbuf(dbdesc->options, options, sizeof(options));
    zlog_debug(OSPF6_DBG_MESSAGE_RECV|OSPF6_DBG_MESSAGE_SEND, "    MBZ: %#x Option: %s IfMTU: %hu",
               dbdesc->reserved1, options, ntohs(dbdesc->ifmtu));
    zlog_debug(OSPF6_DBG_MESSAGE_RECV|OSPF6_DBG_MESSAGE_SEND, "    MBZ: %#x Bits: %s%s%s SeqNum: %#lx",
               dbdesc->reserved2,
               (CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_IBIT) ? "I" : "-"),
               (CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_MBIT) ? "M" : "-"),
               (CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_MSBIT) ? "m" : "s"),
               (u_long) ntohl(dbdesc->seqnum));

    for (p = (char *)((caddr_t) dbdesc + sizeof(struct ospf6_dbdesc));
            p + sizeof(struct ospf6_lsa_header) <= OSPF6_MESSAGE_END(oh);
            p += sizeof(struct ospf6_lsa_header))
    {
        ospf6_lsa_header_print_raw((struct ospf6_lsa_header *) p);
    }

    assert(p == OSPF6_MESSAGE_END(oh));
}

void
ospf6_lsreq_print(struct ospf6_header *oh)
{
    char id[16], adv_router[16];
    char *p;
    ospf6_header_print(oh);
    assert(oh->type == OSPF6_MESSAGE_TYPE_LSREQ);

    for (p = (char *)((caddr_t) oh + sizeof(struct ospf6_header));
            p + sizeof(struct ospf6_lsreq_entry) <= OSPF6_MESSAGE_END(oh);
            p += sizeof(struct ospf6_lsreq_entry))
    {
        struct ospf6_lsreq_entry *e = (struct ospf6_lsreq_entry *) p;
        inet_ntop(AF_INET, &e->adv_router, adv_router, sizeof(adv_router));
        inet_ntop(AF_INET, &e->id, id, sizeof(id));
        zlog_debug(OSPF6_DBG_MESSAGE_RECV|OSPF6_DBG_MESSAGE_SEND, "    [%s Id:%s Adv:%s]",
                   ospf6_lstype_name(e->type), id, adv_router);
    }

    assert(p == OSPF6_MESSAGE_END(oh));
}

void
ospf6_lsupdate_print(struct ospf6_header *oh)
{
    struct ospf6_lsupdate *lsupdate;
    u_long num;
    char *p;
    ospf6_header_print(oh);
    assert(oh->type == OSPF6_MESSAGE_TYPE_LSUPDATE);
    lsupdate = (struct ospf6_lsupdate *)
               ((caddr_t) oh + sizeof(struct ospf6_header));
    num = ntohl(lsupdate->lsa_number);
    zlog_debug(OSPF6_DBG_MESSAGE_RECV|OSPF6_DBG_MESSAGE_SEND, "    Number of LSA: %ld", num);

    for (p = (char *)((caddr_t) lsupdate + sizeof(struct ospf6_lsupdate));
            p < OSPF6_MESSAGE_END(oh) &&
            p + OSPF6_LSA_SIZE(p) <= OSPF6_MESSAGE_END(oh);
            p += OSPF6_LSA_SIZE(p))
    {
        ospf6_lsa_header_print_raw((struct ospf6_lsa_header *) p);
    }

    assert(p == OSPF6_MESSAGE_END(oh));
}

void
ospf6_lsack_print(struct ospf6_header *oh)
{
    char *p;
    ospf6_header_print(oh);
    assert(oh->type == OSPF6_MESSAGE_TYPE_LSACK);

    for (p = (char *)((caddr_t) oh + sizeof(struct ospf6_header));
            p + sizeof(struct ospf6_lsa_header) <= OSPF6_MESSAGE_END(oh);
            p += sizeof(struct ospf6_lsa_header))
    {
        ospf6_lsa_header_print_raw((struct ospf6_lsa_header *) p);
    }

    assert(p == OSPF6_MESSAGE_END(oh));
}

static int
ospf6_hello_recv(struct in6_addr *src, struct in6_addr *dst,
                 struct ospf6_interface *oi, struct ospf6_header *oh)
{
    struct ospf6_hello *hello = NULL;
    struct ospf6_neighbor *on = NULL;
    char *p = NULL;
    int twoway = 0;
    int neighborchange = 0;
    int backupseen = 0;
    hello = (struct ospf6_hello *)
            ((caddr_t) oh + sizeof(struct ospf6_header));

	om6->ospf6_statics.r_hel++;

    /* HelloInterval check */
    if (ntohs(hello->hello_interval) != oi->hello_interval)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "HelloInterval mismatch");
        }
        oi->ospf6->o6_err.receive_hello_mismatch_count ++;
        return -1;
    }

    /* RouterDeadInterval check */
    if (ntohs(hello->dead_interval) != oi->dead_interval)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "RouterDeadInterval mismatch");
        }
        oi->ospf6->o6_err.receive_dead_mismatch_count ++;

        return -1;
    }

    /* E-bit check */
    if (OSPF6_OPT_ISSET(hello->options, OSPF6_OPT_E) !=
            OSPF6_OPT_ISSET(oi->area->options, OSPF6_OPT_E))
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "E-bit mismatch");
        }
        oi->ospf6->o6_err.receive_option_mismatch_count ++;

        return -1;
    }

    /* Find neighbor, create if not exist */
    on = ospf6_neighbor_lookup(oh->router_id, oi);

    if (on == NULL)
    {
        on = ospf6_neighbor_create(oh->router_id, oi);
        on->prev_drouter = on->drouter = hello->drouter;
        on->prev_bdrouter = on->bdrouter = hello->bdrouter;
        on->priority = hello->priority;
    }

    /* always override neighbor's source address and ifindex */
    on->ifindex = ntohl(hello->interface_id);
    memcpy(&on->linklocal_addr, src, sizeof(struct in6_addr));

    /* TwoWay check */
    for (p = (char *)((caddr_t) hello + sizeof(struct ospf6_hello));
            p + sizeof(u_int32_t) <= OSPF6_MESSAGE_END(oh);
            p += sizeof(u_int32_t))
    {
        u_int32_t *router_id = (u_int32_t *) p;

        if (*router_id == oi->area->ospf6->router_id)
        {
            twoway++;
        }
    }

    assert(p == OSPF6_MESSAGE_END(oh));

    /* RouterPriority check */
    if (on->priority != hello->priority)
    {
        on->priority = hello->priority;
        neighborchange++;
    }

    /* DR check */
    if (on->drouter != hello->drouter)
    {
        on->prev_drouter = on->drouter;
        on->drouter = hello->drouter;

        if (on->prev_drouter == on->router_id || on->drouter == on->router_id)
        {
            neighborchange++;
        }
    }

    /* BDR check */
    if (on->bdrouter != hello->bdrouter)
    {
        on->prev_bdrouter = on->bdrouter;
        on->bdrouter = hello->bdrouter;

        if (on->prev_bdrouter == on->router_id || on->bdrouter == on->router_id)
        {
            neighborchange++;
        }
    }

    /* BackupSeen check */
    if (oi->state == OSPF6_INTERFACE_WAITING)
    {
        if (hello->bdrouter == on->router_id)
        {
            backupseen++;
        }
        else if (hello->drouter == on->router_id && hello->bdrouter == htonl(0))
        {
            backupseen++;
        }
    }

    /* Execute neighbor events */
    thread_execute(master_ospf6, hello_received, on, 0);

    if (twoway)
    {
        thread_execute(master_ospf6, twoway_received, on, 0);
    }
    else
    {
        thread_execute(master_ospf6, oneway_received, on, 0);
    }

    /* Schedule interface events */
    if (backupseen)
    {
        thread_add_event(master_ospf6, backup_seen, oi, 0);
    }

    if (neighborchange)
    {
        thread_add_event(master_ospf6, neighbor_change, oi, 0);
    }
	return 0;
}

static int
ospf6_dbdesc_recv_master(struct ospf6_header *oh,
                         struct ospf6_neighbor *on)
{
    struct ospf6_dbdesc *dbdesc;
    char *p;
    dbdesc = (struct ospf6_dbdesc *)
             ((caddr_t) oh + sizeof(struct ospf6_header));

    if (on->state < OSPF6_NEIGHBOR_INIT)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor state less than Init, ignore");
        }
        on->ospf6_if->ospf6->o6_err.receive_mtu_mismatch_count ++;

        return -1;
    }

    switch (on->state)
    {
        case OSPF6_NEIGHBOR_TWOWAY:

            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor state is 2-Way, ignore");
            }

            return -1;
        case OSPF6_NEIGHBOR_INIT:
            thread_execute(master_ospf6, twoway_received, on, 0);

            if (on->state != OSPF6_NEIGHBOR_EXSTART)
            {
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor state is not ExStart, ignore");
                }

                return -1;
            }

            /* else fall through to ExStart */
        case OSPF6_NEIGHBOR_EXSTART:

            /* if neighbor obeys us as our slave, schedule negotiation_done
               and process LSA Headers. Otherwise, ignore this message */
            if (! CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_MSBIT) &&
                    ! CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_IBIT) &&
                    ntohl(dbdesc->seqnum) == on->dbdesc_seqnum)
            {
                /* execute NegotiationDone */
                thread_execute(master_ospf6, negotiation_done, on, 0);
                /* Record neighbor options */
                memcpy(on->options, dbdesc->options, sizeof(on->options));
            }
            else
            {
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s %d Negotiation failed",__func__,__LINE__);
                }

                return -1;
            }

            /* fall through to exchange */
        case OSPF6_NEIGHBOR_EXCHANGE:

            if (! memcmp(dbdesc, &on->dbdesc_last, sizeof(struct ospf6_dbdesc)))
            {
                /* Duplicated DatabaseDescription is dropped by master */
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Duplicated dbdesc discarded by Master, ignore");
                }

                return -1;
            }

            if (CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_MSBIT))
            {
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Master/Slave bit mismatch");
                }

                thread_add_event(master_ospf6, seqnumber_mismatch, on, 0);
                return -1;
            }

            if (CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_IBIT))
            {
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Initialize bit mismatch");
                }

                thread_add_event(master_ospf6, seqnumber_mismatch, on, 0);
                return -1;
            }

            if (memcmp(on->options, dbdesc->options, sizeof(on->options)))
            {
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Option field mismatch");
                }

                thread_add_event(master_ospf6, seqnumber_mismatch, on, 0);
                return -1;
            }

            if (ntohl(dbdesc->seqnum) != on->dbdesc_seqnum)
            {
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Sequence number mismatch (%#lx expected)",
                               (u_long) on->dbdesc_seqnum);

                thread_add_event(master_ospf6, seqnumber_mismatch, on, 0);
                return -1;
            }

            break;
        case OSPF6_NEIGHBOR_LOADING:
        case OSPF6_NEIGHBOR_FULL:

            if (! memcmp(dbdesc, &on->dbdesc_last, sizeof(struct ospf6_dbdesc)))
            {
                /* Duplicated DatabaseDescription is dropped by master */
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Duplicated dbdesc discarded by Master, ignore");
                }

                return -1;
            }

            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Not duplicate dbdesc in state %s",
                           ospf6_neighbor_state_str[on->state]);

            thread_add_event(master_ospf6, seqnumber_mismatch, on, 0);
            return -1;
        default:
            assert(0);
            break;
    }

    /* Process LSA headers */
    for (p = (char *)((caddr_t) dbdesc + sizeof(struct ospf6_dbdesc));
            p + sizeof(struct ospf6_lsa_header) <= OSPF6_MESSAGE_END(oh);
            p += sizeof(struct ospf6_lsa_header))
    {
        struct ospf6_lsa *his, *mine;
        struct ospf6_lsdb *lsdb = NULL;
        his = ospf6_lsa_create_headeronly((struct ospf6_lsa_header *) p);

        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s", his->name);
        }

        switch (OSPF6_LSA_SCOPE(his->header->type))
        {
            case OSPF6_SCOPE_LINKLOCAL:
                lsdb = on->ospf6_if->lsdb;
                break;
            case OSPF6_SCOPE_AREA:
                lsdb = on->ospf6_if->area->lsdb;
                break;
            case OSPF6_SCOPE_AS:
                lsdb = on->ospf6_if->area->ospf6->lsdb;
                break;
            case OSPF6_SCOPE_RESERVED:

                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Ignoring LSA of reserved scope");
                }

                ospf6_lsa_delete(his);
				his = NULL;
                continue;
                break;
        }

        if (ntohs(his->header->type) == OSPF6_LSTYPE_AS_EXTERNAL &&
                IS_AREA_STUB(on->ospf6_if->area))
        {
            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "SeqNumMismatch (E-bit mismatch), discard");
            }

            ospf6_lsa_delete(his);
			his = NULL;
            thread_add_event(master_ospf6, seqnumber_mismatch, on, 0);
            return -1;
        }

        mine = ospf6_lsdb_lookup(his->header->type, his->header->id,
                                 his->header->adv_router, lsdb);

        if (mine == NULL)
        {
            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Add request (No database copy)");
            }

            ospf6_lsdb_add(ospf6_lsa_copy(his), on->request_list);
        }
        else if (ospf6_lsa_compare(his, mine) < 0)
        {
            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Add request (Received MoreRecent)");
            }

            ospf6_lsdb_add(ospf6_lsa_copy(his), on->request_list);
        }
        else
        {
            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Discard (Existing MoreRecent)");
            }
        }

        ospf6_lsa_delete(his);
		his = NULL;
    }

    assert(p == OSPF6_MESSAGE_END(oh));
    /* Increment sequence number */
    on->dbdesc_seqnum ++;

    /* schedule send lsreq */
    if (on->request_list->count && (on->thread_send_lsreq == 0))
    {
        /*on->thread_send_lsreq =
            thread_add_event(master_ospf6, ospf6_lsreq_send, on, 0);*/
		on->thread_send_lsreq =
			high_pre_timer_add ((char *)"ospf6_lr_send_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_lsreq_send, (void *)on, 10);
    }

    OSPF6_TIMER_OFF(on->thread_send_dbdesc);

    /* More bit check */
    if (! CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_MBIT) &&
            ! CHECK_FLAG(on->dbdesc_bits, OSPF6_DBDESC_MBIT))
    {
        //thread_add_event(master_ospf6, exchange_done, on, 0);
        thread_execute(master_ospf6, exchange_done, on, 0);
    }
    else
    {
        /*on->thread_send_dbdesc =
            thread_add_event(master_ospf6, ospf6_dbdesc_send_newone, on, 0);*/
		on->thread_send_dbdesc =
			high_pre_timer_add ((char *)"ospf6_dd_send_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_dbdesc_send_newone, (void *)on, 10);
    }

    /* save last received dbdesc */
    memcpy(&on->dbdesc_last, dbdesc, sizeof(struct ospf6_dbdesc));
	return 0;
}

static int 
ospf6_dbdesc_recv_slave(struct ospf6_header *oh,
                        struct ospf6_neighbor *on)
{
    struct ospf6_dbdesc *dbdesc;
    char *p;
    dbdesc = (struct ospf6_dbdesc *)
             ((caddr_t) oh + sizeof(struct ospf6_header));

    if (on->state < OSPF6_NEIGHBOR_INIT)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor state less than Init, ignore");
        }

        return -1;
    }

    switch (on->state)
    {
        case OSPF6_NEIGHBOR_TWOWAY:

            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor state is 2-Way, ignore");
            }

            return -1;
        case OSPF6_NEIGHBOR_INIT:
            thread_execute(master_ospf6, twoway_received, on, 0);

            if (on->state != OSPF6_NEIGHBOR_EXSTART)
            {
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor state is not ExStart, ignore");
                }

                return -1;
            }

            /* else fall through to ExStart */
        case OSPF6_NEIGHBOR_EXSTART:

            /* If the neighbor is Master, act as Slave. Schedule negotiation_done
               and process LSA Headers. Otherwise, ignore this message */
            if (CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_IBIT) &&
                    CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_MBIT) &&
                    CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_MSBIT) &&
                    ntohs(oh->length) == sizeof(struct ospf6_header) +
                    sizeof(struct ospf6_dbdesc))
            {
                /* set the master/slave bit to slave */
                UNSET_FLAG(on->dbdesc_bits, OSPF6_DBDESC_MSBIT);
                /* set the DD sequence number to one specified by master */
                on->dbdesc_seqnum = ntohl(dbdesc->seqnum);
                /* schedule NegotiationDone */
                thread_execute(master_ospf6, negotiation_done, on, 0);
                /* Record neighbor options */
                memcpy(on->options, dbdesc->options, sizeof(on->options));
            }
            else
            {
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s %d Negotiation failed",__func__,__LINE__);
                }

                return -1;
            }

            break;
        case OSPF6_NEIGHBOR_EXCHANGE:

            if (! memcmp(dbdesc, &on->dbdesc_last, sizeof(struct ospf6_dbdesc)))
            {
                /* Duplicated DatabaseDescription causes slave to retransmit */
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Duplicated dbdesc causes retransmit");
                }

                OSPF6_TIMER_OFF(on->thread_send_dbdesc);
                /*on->thread_send_dbdesc =
                    thread_add_event(master_ospf6, ospf6_dbdesc_send, on, 0);*/
				on->thread_send_dbdesc =
					high_pre_timer_add ((char *)"ospf6_dd_send_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_dbdesc_send, (void *)on, 10);
                return -1;
            }

            if (! CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_MSBIT))
            {
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Master/Slave bit mismatch");
                }

                thread_add_event(master_ospf6, seqnumber_mismatch, on, 0);
                return -1;
            }

            if (CHECK_FLAG(dbdesc->bits, OSPF6_DBDESC_IBIT))
            {
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Initialize bit mismatch");
                }

                thread_add_event(master_ospf6, seqnumber_mismatch, on, 0);
                return -1;
            }

            if (memcmp(on->options, dbdesc->options, sizeof(on->options)))
            {
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Option field mismatch");
                }

                thread_add_event(master_ospf6, seqnumber_mismatch, on, 0);
                return -1;
            }

            if (ntohl(dbdesc->seqnum) != on->dbdesc_seqnum + 1)
            {
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Sequence number mismatch (%#lx expected)",
                               (u_long) on->dbdesc_seqnum + 1);

                thread_add_event(master_ospf6, seqnumber_mismatch, on, 0);
                return -1;
            }

            break;
        case OSPF6_NEIGHBOR_LOADING:
        case OSPF6_NEIGHBOR_FULL:

            if (! memcmp(dbdesc, &on->dbdesc_last, sizeof(struct ospf6_dbdesc)))
            {
                /* Duplicated DatabaseDescription causes slave to retransmit */
                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Duplicated dbdesc causes retransmit");
                }

                OSPF6_TIMER_OFF(on->thread_send_dbdesc);
                /*on->thread_send_dbdesc =
                    thread_add_event(master_ospf6, ospf6_dbdesc_send, on, 0);*/
				on->thread_send_dbdesc =
					high_pre_timer_add ((char *)"ospf6_dd_send_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_dbdesc_send, (void *)on, 10);
                return -1;
            }

            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Not duplicate dbdesc in state %s",
                           ospf6_neighbor_state_str[on->state]);

            thread_add_event(master_ospf6, seqnumber_mismatch, on, 0);
            return -1; 
        default:
            assert(0);
            break;
    }

    /* Process LSA headers */
    for (p = (char *)((caddr_t) dbdesc + sizeof(struct ospf6_dbdesc));
            p + sizeof(struct ospf6_lsa_header) <= OSPF6_MESSAGE_END(oh);
            p += sizeof(struct ospf6_lsa_header))
    {
        struct ospf6_lsa *his, *mine;
        struct ospf6_lsdb *lsdb = NULL;
        his = ospf6_lsa_create_headeronly((struct ospf6_lsa_header *) p);

        switch (OSPF6_LSA_SCOPE(his->header->type))
        {
            case OSPF6_SCOPE_LINKLOCAL:
                lsdb = on->ospf6_if->lsdb;
                break;
            case OSPF6_SCOPE_AREA:
                lsdb = on->ospf6_if->area->lsdb;
                break;
            case OSPF6_SCOPE_AS:
                lsdb = on->ospf6_if->area->ospf6->lsdb;
                break;
            case OSPF6_SCOPE_RESERVED:

                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Ignoring LSA of reserved scope");
                }

                ospf6_lsa_delete(his);
				his = NULL;
                continue;
                break;
        }

        if (OSPF6_LSA_SCOPE(his->header->type) == OSPF6_SCOPE_AS &&
                IS_AREA_STUB(on->ospf6_if->area))
        {
            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "E-bit mismatch with LSA Headers");
            }

            ospf6_lsa_delete(his);
			his = NULL;
            thread_add_event(master_ospf6, seqnumber_mismatch, on, 0);
            return -1;
        }

        mine = ospf6_lsdb_lookup(his->header->type, his->header->id,
                                 his->header->adv_router, lsdb);

        if (mine == NULL || ospf6_lsa_compare(his, mine) < 0)
        {
            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Add request-list: %s", his->name);
            }

            ospf6_lsdb_add(ospf6_lsa_copy(his), on->request_list);
        }

        ospf6_lsa_delete(his);
		his = NULL;
    }

    assert(p == OSPF6_MESSAGE_END(oh));
    /* Set sequence number to Master's */
    on->dbdesc_seqnum = ntohl(dbdesc->seqnum);

    /* schedule send lsreq */
    if ((on->thread_send_lsreq == 0) && (on->request_list->count))
    {
        /*on->thread_send_lsreq =
            thread_add_event(master_ospf6, ospf6_lsreq_send, on, 0);*/
		on->thread_send_lsreq =
			high_pre_timer_add ((char *)"ospf6_lr_send_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_lsreq_send, (void *)on, 10);
	}

    OSPF6_TIMER_OFF(on->thread_send_dbdesc);
    /*on->thread_send_dbdesc =
        thread_add_event(master_ospf6, ospf6_dbdesc_send_newone, on, 0);*/
	on->thread_send_dbdesc =
		high_pre_timer_add ((char *)"ospf6_dd_send_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_dbdesc_send_newone, (void *)on, 10);
    /* save last received dbdesc */
    memcpy(&on->dbdesc_last, dbdesc, sizeof(struct ospf6_dbdesc));
	return 0;
}

static int
ospf6_dbdesc_recv(struct in6_addr *src, struct in6_addr *dst,
                  struct ospf6_interface *oi, struct ospf6_header *oh)
{
    struct ospf6_neighbor *on;
    struct ospf6_dbdesc *dbdesc;
	om6->ospf6_statics.r_dd++;
    on = ospf6_neighbor_lookup(oh->router_id, oi);

    if (on == NULL)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor not found, ignore");
        }

        return -1;
    }

    dbdesc = (struct ospf6_dbdesc *)
             ((caddr_t) oh + sizeof(struct ospf6_header));

    /* Interface MTU check */
    if (!oi->mtu_ignore && ntohs(dbdesc->ifmtu) != oi->ifmtu)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "I/F MTU mismatch");
        }
        oi->ospf6->o6_err.receive_mtu_mismatch_count ++;
        return -1;
    }

    if (dbdesc->reserved1 || dbdesc->reserved2)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Non-0 reserved field in %s's DbDesc, correct",
                       on->name);

        dbdesc->reserved1 = 0;
        dbdesc->reserved2 = 0;
    }

    if (ntohl(oh->router_id) < ntohl(oi->ospf6->router_id))
    {
        ospf6_dbdesc_recv_master(oh, on);
    }
    else if (ntohl(oi->ospf6->router_id) < ntohl(oh->router_id))
    {
        ospf6_dbdesc_recv_slave(oh, on);
    }
    else
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Can't decide which is master, ignore");
        }
    }
	return 0;
}

static int
ospf6_lsreq_recv(struct in6_addr *src, struct in6_addr *dst,
                 struct ospf6_interface *oi, struct ospf6_header *oh)
{
    struct ospf6_neighbor *on;
    char *p;
    struct ospf6_lsreq_entry *e;
    struct ospf6_lsdb *lsdb = NULL;
    struct ospf6_lsa *lsa;
	
	om6->ospf6_statics.r_lsr ++;
    on = ospf6_neighbor_lookup(oh->router_id, oi);

    if (on == NULL)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor not found, ignore");
        }

        return -1;
    }

    if (on->state != OSPF6_NEIGHBOR_EXCHANGE &&
            on->state != OSPF6_NEIGHBOR_LOADING &&
            on->state != OSPF6_NEIGHBOR_FULL)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor state less than Exchange, ignore");
        }
        
        oi->ospf6->o6_err.receive_lsr_neighbor_low_count ++;
        return -1;
    }

    /* Process each request */
    for (p = (char *)((caddr_t) oh + sizeof(struct ospf6_header));
            p + sizeof(struct ospf6_lsreq_entry) <= OSPF6_MESSAGE_END(oh);
            p += sizeof(struct ospf6_lsreq_entry))
    {
        e = (struct ospf6_lsreq_entry *) p;

        switch (OSPF6_LSA_SCOPE(e->type))
        {
            case OSPF6_SCOPE_LINKLOCAL:
                lsdb = on->ospf6_if->lsdb;
                break;
            case OSPF6_SCOPE_AREA:
                lsdb = on->ospf6_if->area->lsdb;
                break;
            case OSPF6_SCOPE_AS:
                lsdb = on->ospf6_if->area->ospf6->lsdb;
                break;
            default:

                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Ignoring LSA of reserved scope");
                }

                continue;
                break;
        }

        /* Find database copy */
        lsa = ospf6_lsdb_lookup(e->type, e->id, e->adv_router, lsdb);

        if (lsa == NULL)
        {
            char id[16], adv_router[16];

            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            {
                inet_ntop(AF_INET, &e->id, id, sizeof(id));
                inet_ntop(AF_INET, &e->adv_router, adv_router,
                          sizeof(adv_router));
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Can't find requested [%s Id:%s Adv:%s]",
                           ospf6_lstype_name(e->type), id, adv_router);
            }
            
            oi->ospf6->o6_err.receive_bad_req_count ++;
            thread_add_event(master_ospf6, bad_lsreq, on, 0);
            return -1;
        }

        ospf6_lsdb_add(ospf6_lsa_copy(lsa), on->lsupdate_list);
    }

    assert(p == OSPF6_MESSAGE_END(oh));
    /* schedule send lsupdate */
    OSPF6_TIMER_OFF(on->thread_send_lsupdate);
    /*on->thread_send_lsupdate =
        thread_add_event(master_ospf6, ospf6_lsupdate_send_neighbor, on, 0);*/
	on->thread_send_lsupdate =
			high_pre_timer_add ((char *)"ospf6_lsupdate_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_lsupdate_send_neighbor, (void *)on, 10);
	return 0;
}

/* Verify, that the specified memory area contains exactly N valid IPv6
   prefixes as specified by RFC5340, A.4.1. */
static unsigned
ospf6_prefixes_examin
(
    struct ospf6_prefix *current, /* start of buffer    */
    unsigned length,
    const u_int32_t req_num_pfxs  /* always compared with the actual number of prefixes */
)
{
    u_char requested_pfx_bytes;
    u_int32_t real_num_pfxs = 0;

    while (length)
    {
        if (length < OSPF6_PREFIX_MIN_SIZE)
        {
            if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: undersized IPv6 prefix header", __func__);
            }

            return MSG_NG;
        }

        /* safe to look deeper */
        if (current->prefix_length > IPV6_MAX_BITLEN)
        {
            if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: invalid PrefixLength (%u bits)", __func__, current->prefix_length);
            }

            return MSG_NG;
        }

        /* covers both fixed- and variable-sized fields */
        requested_pfx_bytes = OSPF6_PREFIX_MIN_SIZE + OSPF6_PREFIX_SPACE(current->prefix_length);

        if (requested_pfx_bytes > length)
        {
            if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: undersized IPv6 prefix", __func__);
            }

            return MSG_NG;
        }

        /* next prefix */
        length -= requested_pfx_bytes;
        current = (struct ospf6_prefix *)((caddr_t) current + requested_pfx_bytes);
        real_num_pfxs++;
    }

    if (real_num_pfxs != req_num_pfxs)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: IPv6 prefix number mismatch (%u required, %u real)",
                       __func__, req_num_pfxs, real_num_pfxs);

        return MSG_NG;
    }

    return MSG_OK;
}

/* Verify an LSA to have a valid length and dispatch further (where
   appropriate) to check if the contents, including nested IPv6 prefixes,
   is properly sized/aligned within the LSA. Note that this function gets
   LSA type in network byte order, uses in host byte order and passes to
   ospf6_lstype_name() in network byte order again. */
static unsigned
ospf6_lsa_examin(struct ospf6_lsa_header *lsah, const u_int16_t lsalen, const u_char headeronly)
{
    struct ospf6_intra_prefix_lsa *intra_prefix_lsa;
    struct ospf6_as_external_lsa *as_external_lsa;
    struct ospf6_link_lsa *link_lsa;
    unsigned exp_length;
    u_int8_t ltindex;
    u_int16_t lsatype;
    /* In case an additional minimum length constraint is defined for current
       LSA type, make sure that this constraint is met. */
    lsatype = ntohs(lsah->type);
    ltindex = lsatype & OSPF6_LSTYPE_FCODE_MASK;

    if
    (
        ltindex < OSPF6_LSTYPE_SIZE &&
        ospf6_lsa_minlen[ltindex] &&
        lsalen < ospf6_lsa_minlen[ltindex] + OSPF6_LSA_HEADER_SIZE
    )
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: undersized (%u B) LSA", __func__, lsalen);
        }

        return MSG_NG;
    }

    switch (lsatype)
    {
        case OSPF6_LSTYPE_ROUTER:

            /* RFC5340 A.4.3, LSA header + OSPF6_ROUTER_LSA_MIN_SIZE bytes followed
               by N>=0 interface descriptions. */
            if ((lsalen - OSPF6_LSA_HEADER_SIZE - OSPF6_ROUTER_LSA_MIN_SIZE) % OSPF6_ROUTER_LSDESC_FIX_SIZE)
            {
                if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: interface description alignment error", __func__);
                }

                return MSG_NG;
            }

            break;
        case OSPF6_LSTYPE_NETWORK:

            /* RFC5340 A.4.4, LSA header + OSPF6_NETWORK_LSA_MIN_SIZE bytes
               followed by N>=0 attached router descriptions. */
            if ((lsalen - OSPF6_LSA_HEADER_SIZE - OSPF6_NETWORK_LSA_MIN_SIZE) % OSPF6_NETWORK_LSDESC_FIX_SIZE)
            {
                if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: router description alignment error", __func__);
                }

                return MSG_NG;
            }

            break;
        case OSPF6_LSTYPE_INTER_PREFIX:

            /* RFC5340 A.4.5, LSA header + OSPF6_INTER_PREFIX_LSA_MIN_SIZE bytes
               followed by 3-4 fields of a single IPv6 prefix. */
            if (headeronly)
            {
                break;
            }

            return ospf6_prefixes_examin
                   (
                       (struct ospf6_prefix *)((caddr_t) lsah + OSPF6_LSA_HEADER_SIZE + OSPF6_INTER_PREFIX_LSA_MIN_SIZE),
                       lsalen - OSPF6_LSA_HEADER_SIZE - OSPF6_INTER_PREFIX_LSA_MIN_SIZE,
                       1
                   );
        case OSPF6_LSTYPE_INTER_ROUTER:

            /* RFC5340 A.4.6, fixed-size LSA. */
            if (lsalen > OSPF6_LSA_HEADER_SIZE + OSPF6_INTER_ROUTER_LSA_FIX_SIZE)
            {
                if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: oversized (%u B) LSA", __func__, lsalen);
                }

                return MSG_NG;
            }

            break;
        case OSPF6_LSTYPE_AS_EXTERNAL: /* RFC5340 A.4.7, same as A.4.8. */
        case OSPF6_LSTYPE_TYPE_7:

            /* RFC5340 A.4.8, LSA header + OSPF6_AS_EXTERNAL_LSA_MIN_SIZE bytes
               followed by 3-4 fields of IPv6 prefix and 3 conditional LSA fields:
               16 bytes of forwarding address, 4 bytes of external route tag,
               4 bytes of referenced link state ID. */
            if (headeronly)
            {
                break;
            }

            as_external_lsa = (struct ospf6_as_external_lsa *)((caddr_t) lsah + OSPF6_LSA_HEADER_SIZE);
            exp_length = OSPF6_LSA_HEADER_SIZE + OSPF6_AS_EXTERNAL_LSA_MIN_SIZE;

            /* To find out if the last optional field (Referenced Link State ID) is
               assumed in this LSA, we need to access fixed fields of the IPv6
               prefix before ospf6_prefix_examin() confirms its sizing. */
            if (exp_length + OSPF6_PREFIX_MIN_SIZE > lsalen)
            {
                if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: undersized (%u B) LSA header", __func__, lsalen);
                }

                return MSG_NG;
            }

            /* forwarding address */
            if (CHECK_FLAG(as_external_lsa->bits_metric, OSPF6_ASBR_BIT_F))
            {
                exp_length += 16;
            }

            /* external route tag */
            if (CHECK_FLAG(as_external_lsa->bits_metric, OSPF6_ASBR_BIT_T))
            {
                exp_length += 4;
            }

            /* referenced link state ID */
            if (as_external_lsa->prefix.u._prefix_referenced_lstype)
            {
                exp_length += 4;
            }

            /* All the fixed-size fields (mandatory and optional) must fit. I.e.,
               this check does not include any IPv6 prefix fields. */
            if (exp_length > lsalen)
            {
                if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: undersized (%u B) LSA header", __func__, lsalen);
                }

                return MSG_NG;
            }

            /* The last call completely covers the remainder (IPv6 prefix). */
            return ospf6_prefixes_examin
                   (
                       (struct ospf6_prefix *)((caddr_t) as_external_lsa + OSPF6_AS_EXTERNAL_LSA_MIN_SIZE),
                       lsalen - exp_length,
                       1
                   );
        case OSPF6_LSTYPE_LINK:

            /* RFC5340 A.4.9, LSA header + OSPF6_LINK_LSA_MIN_SIZE bytes followed
               by N>=0 IPv6 prefix blocks (with N declared beforehand). */
            if (headeronly)
            {
                break;
            }

            link_lsa = (struct ospf6_link_lsa *)((caddr_t) lsah + OSPF6_LSA_HEADER_SIZE);
            return ospf6_prefixes_examin
                   (
                       (struct ospf6_prefix *)((caddr_t) link_lsa + OSPF6_LINK_LSA_MIN_SIZE),
                       lsalen - OSPF6_LSA_HEADER_SIZE - OSPF6_LINK_LSA_MIN_SIZE,
                       ntohl(link_lsa->prefix_num)  /* 32 bits */
                   );
        case OSPF6_LSTYPE_INTRA_PREFIX:

            /* RFC5340 A.4.10, LSA header + OSPF6_INTRA_PREFIX_LSA_MIN_SIZE bytes
               followed by N>=0 IPv6 prefixes (with N declared beforehand). */
            if (headeronly)
            {
                break;
            }

            intra_prefix_lsa = (struct ospf6_intra_prefix_lsa *)((caddr_t) lsah + OSPF6_LSA_HEADER_SIZE);
            return ospf6_prefixes_examin
                   (
                       (struct ospf6_prefix *)((caddr_t) intra_prefix_lsa + OSPF6_INTRA_PREFIX_LSA_MIN_SIZE),
                       lsalen - OSPF6_LSA_HEADER_SIZE - OSPF6_INTRA_PREFIX_LSA_MIN_SIZE,
                       ntohs(intra_prefix_lsa->prefix_num)  /* 16 bits */
                   );
    }

    /* No additional validation is possible for unknown LSA types, which are
       themselves valid in OPSFv3, hence the default decision is to accept. */
    return MSG_OK;
}

/* Verify if the provided input buffer is a valid sequence of LSAs. This
   includes verification of LSA blocks length/alignment and dispatching
   of deeper-level checks. */
static unsigned
ospf6_lsaseq_examin
(
    struct ospf6_lsa_header *lsah, /* start of buffered data */
    size_t length,
    const u_char headeronly,
    /* When declared_num_lsas is not 0, compare it to the real number of LSAs
       and treat the difference as an error. */
    const u_int32_t declared_num_lsas
)
{
    u_int32_t counted_lsas = 0;

    while (length)
    {
        u_int16_t lsalen;

        if (length < OSPF6_LSA_HEADER_SIZE)
        {
            if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: undersized (%zu B) trailing (#%u) LSA header",
                           __func__, length, counted_lsas);

            return MSG_NG;
        }

        /* save on ntohs() calls here and in the LSA validator */
        lsalen = OSPF6_LSA_SIZE(lsah);

        if (lsalen < OSPF6_LSA_HEADER_SIZE)
        {
            if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: malformed LSA header #%u, declared length is %u B",
                           __func__, counted_lsas, lsalen);

            return MSG_NG;
        }

        if (headeronly)
        {
            /* less checks here and in ospf6_lsa_examin() */
            if (MSG_OK != ospf6_lsa_examin(lsah, lsalen, 1))
            {
                if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: anomaly in header-only %s LSA #%u", __func__,
                               ospf6_lstype_name(lsah->type), counted_lsas);

                return MSG_NG;
            }

            lsah = (struct ospf6_lsa_header *)((caddr_t) lsah + OSPF6_LSA_HEADER_SIZE);
            length -= OSPF6_LSA_HEADER_SIZE;
        }
        else
        {
            /* make sure the input buffer is deep enough before further checks */
            if (lsalen > length)
            {
                if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: anomaly in %s LSA #%u: declared length is %u B, buffered length is %zu B",
                               __func__, ospf6_lstype_name(lsah->type), counted_lsas, lsalen, length);

                return MSG_NG;
            }

            if (MSG_OK != ospf6_lsa_examin(lsah, lsalen, 0))
            {
                if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: anomaly in %s LSA #%u", __func__,
                               ospf6_lstype_name(lsah->type), counted_lsas);

                return MSG_NG;
            }

            lsah = (struct ospf6_lsa_header *)((caddr_t) lsah + lsalen);
            length -= lsalen;
        }

        counted_lsas++;
    }

    if (declared_num_lsas && counted_lsas != declared_num_lsas)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: #LSAs declared (%u) does not match actual (%u)",
                       __func__, declared_num_lsas, counted_lsas);

        return MSG_NG;
    }

    return MSG_OK;
}

/* Verify a complete OSPF packet for proper sizing/alignment. */
static unsigned
ospf6_packet_examin(struct ospf6_header *oh, const unsigned bytesonwire)
{
    struct ospf6_lsupdate *lsupd;
    unsigned test;

    /* length, 1st approximation */
    if (bytesonwire < OSPF6_HEADER_SIZE)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: undersized (%u B) packet", __func__, bytesonwire);
        }

        return MSG_NG;
    }

    /* Now it is safe to access header fields. */
    if (bytesonwire != ntohs(oh->length))
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: packet length error (%u real, %u declared)",
                       __func__, bytesonwire, ntohs(oh->length));

        return MSG_NG;
    }

    /* version check */
    if (oh->version != OSPFV3_VERSION)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: invalid (%u) protocol version", __func__, oh->version);
        }

        return MSG_NG;
    }

    /* length, 2nd approximation */
    if
    (
        oh->type < OSPF6_MESSAGE_TYPE_ALL &&
        ospf6_packet_minlen[oh->type] &&
        bytesonwire < OSPF6_HEADER_SIZE + ospf6_packet_minlen[oh->type]
    )
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: undersized (%u B) %s packet", __func__,
                       bytesonwire, LOOKUP(ospf6_message_type_str, oh->type));

        return MSG_NG;
    }

    /* type-specific deeper validation */
    switch (oh->type)
    {
        case OSPF6_MESSAGE_TYPE_HELLO:

            /* RFC5340 A.3.2, packet header + OSPF6_HELLO_MIN_SIZE bytes followed
               by N>=0 router-IDs. */
            if (0 == (bytesonwire - OSPF6_HEADER_SIZE - OSPF6_HELLO_MIN_SIZE) % 4)
            {
                return MSG_OK;
            }

            if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: alignment error in %s packet",
                           __func__, LOOKUP(ospf6_message_type_str, oh->type));

            return MSG_NG;
        case OSPF6_MESSAGE_TYPE_DBDESC:
            /* RFC5340 A.3.3, packet header + OSPF6_DB_DESC_MIN_SIZE bytes followed
               by N>=0 header-only LSAs. */
            test = ospf6_lsaseq_examin
                   (
                       (struct ospf6_lsa_header *)((caddr_t) oh + OSPF6_HEADER_SIZE + OSPF6_DB_DESC_MIN_SIZE),
                       bytesonwire - OSPF6_HEADER_SIZE - OSPF6_DB_DESC_MIN_SIZE,
                       1,
                       0
                   );
            break;
        case OSPF6_MESSAGE_TYPE_LSREQ:

            /* RFC5340 A.3.4, packet header + N>=0 LS description blocks. */
            if (0 == (bytesonwire - OSPF6_HEADER_SIZE - OSPF6_LS_REQ_MIN_SIZE) % OSPF6_LSREQ_LSDESC_FIX_SIZE)
            {
                return MSG_OK;
            }

            if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: alignment error in %s packet",
                           __func__, LOOKUP(ospf6_message_type_str, oh->type));

            return MSG_NG;
        case OSPF6_MESSAGE_TYPE_LSUPDATE:
            /* RFC5340 A.3.5, packet header + OSPF6_LS_UPD_MIN_SIZE bytes followed
               by N>=0 full LSAs (with N declared beforehand). */
            lsupd = (struct ospf6_lsupdate *)((caddr_t) oh + OSPF6_HEADER_SIZE);
            test = ospf6_lsaseq_examin
                   (
                       (struct ospf6_lsa_header *)((caddr_t) lsupd + OSPF6_LS_UPD_MIN_SIZE),
                       bytesonwire - OSPF6_HEADER_SIZE - OSPF6_LS_UPD_MIN_SIZE,
                       0,
                       ntohl(lsupd->lsa_number)  /* 32 bits */
                   );
            break;
        case OSPF6_MESSAGE_TYPE_LSACK:
            /* RFC5340 A.3.6, packet header + N>=0 header-only LSAs. */
            test = ospf6_lsaseq_examin
                   (
                       (struct ospf6_lsa_header *)((caddr_t) oh + OSPF6_HEADER_SIZE + OSPF6_LS_ACK_MIN_SIZE),
                       bytesonwire - OSPF6_HEADER_SIZE - OSPF6_LS_ACK_MIN_SIZE,
                       1,
                       0
                   );
            break;
        default:

            if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: invalid (%u) message type", __func__, oh->type);
            }

            return MSG_NG;
    }

    if (test != MSG_OK && IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
    {
        zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: anomaly in %s packet", __func__, LOOKUP(ospf6_message_type_str, oh->type));
    }

    return test;
}

/* Verify particular fields of otherwise correct received OSPF packet to
   meet the requirements of RFC. */
static int
ospf6_rxpacket_examin(struct ospf6_interface *oi, struct ospf6_header *oh, const unsigned bytesonwire)
{
    char buf[2][INET_ADDRSTRLEN];

    if (MSG_OK != ospf6_packet_examin(oh, bytesonwire))
    {
        oi->ospf6->o6_err.receive_pkt_small_count ++;
        return MSG_NG;
    }

    /* Area-ID check */
    if (oh->area_id != oi->area->area_id)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            if (oh->area_id == OSPF_AREA_BACKBONE)
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: Message may be via Virtual Link: not supported", __func__);
            }
            else
            {
                zlog_debug
                (
                	OSPF6_DBG_MESSAGE_RECV, 
                    "%s: Area-ID mismatch (my %s, rcvd %s)", __func__,
                    inet_ntop(AF_INET, &oi->area->area_id, buf[0], INET_ADDRSTRLEN),
                    inet_ntop(AF_INET, &oh->area_id, buf[1], INET_ADDRSTRLEN)
                );
            }
        }
        oi->ospf6->o6_err.receive_bad_area_count ++;
        return MSG_NG;
    }

    /* Instance-ID check */
    if (oh->instance_id != oi->instance_id)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: Instance-ID mismatch (my %u, rcvd %u)", __func__, oi->instance_id, oh->instance_id);
        }
        oi->ospf6->o6_err.receive_bad_instance_count ++;
        return MSG_NG;
    }

    /* Router-ID check */
    if (oh->router_id == oi->area->ospf6->router_id)
    {
        zlog_warn("%s: Duplicate Router-ID (%s)", __func__, inet_ntop(AF_INET, &oh->router_id, buf[0], INET_ADDRSTRLEN));
        oi->ospf6->o6_err.receive_routerid_mismatch_count ++;
        return MSG_NG;
    }

    return MSG_OK;
}

static int
ospf6_lsupdate_recv(struct in6_addr *src, struct in6_addr *dst,
                    struct ospf6_interface *oi, struct ospf6_header *oh)
{
    struct ospf6_neighbor *on;
    struct ospf6_lsupdate *lsupdate;
    char *p;

	om6->ospf6_statics.r_lsu ++;
    on = ospf6_neighbor_lookup(oh->router_id, oi);

    if (on == NULL)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor not found, ignore");
        }

        return -1;
    }

    if (on->state != OSPF6_NEIGHBOR_EXCHANGE &&
            on->state != OSPF6_NEIGHBOR_LOADING &&
            on->state != OSPF6_NEIGHBOR_FULL)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor state less than Exchange, ignore");
        }

        return -1;
    }

    lsupdate = (struct ospf6_lsupdate *)
               ((caddr_t) oh + sizeof(struct ospf6_header));

    /* Process LSAs */
    for (p = (char *)((caddr_t) lsupdate + sizeof(struct ospf6_lsupdate));
            p < OSPF6_MESSAGE_END(oh) &&
            p + OSPF6_LSA_SIZE(p) <= OSPF6_MESSAGE_END(oh);
            p += OSPF6_LSA_SIZE(p))
    {
        ospf6_receive_lsa(oi->ospf6, on, (struct ospf6_lsa_header *) p);
    }

    assert(p == OSPF6_MESSAGE_END(oh));
	return 0;
}

static int
ospf6_lsack_recv(struct in6_addr *src, struct in6_addr *dst,
                 struct ospf6_interface *oi, struct ospf6_header *oh)
{
    struct ospf6_neighbor *on;
    char *p;
    struct ospf6_lsa *his, *mine;
    struct ospf6_lsdb *lsdb = NULL;

	om6->ospf6_statics.r_lsa ++;
    assert(oh->type == OSPF6_MESSAGE_TYPE_LSACK);
    on = ospf6_neighbor_lookup(oh->router_id, oi);

    if (on == NULL)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor not found, ignore");
        }

        return -1;
    }

    if (on->state != OSPF6_NEIGHBOR_EXCHANGE &&
            on->state != OSPF6_NEIGHBOR_LOADING &&
            on->state != OSPF6_NEIGHBOR_FULL)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Neighbor state less than Exchange, ignore");
        }
        
        oi->ospf6->o6_err.receive_lsack_neighbor_low_count ++;
        return -1;
    }

    for (p = (char *)((caddr_t) oh + sizeof(struct ospf6_header));
            p + sizeof(struct ospf6_lsa_header) <= OSPF6_MESSAGE_END(oh);
            p += sizeof(struct ospf6_lsa_header))
    {
        his = ospf6_lsa_create_headeronly((struct ospf6_lsa_header *) p);

        switch (OSPF6_LSA_SCOPE(his->header->type))
        {
            case OSPF6_SCOPE_LINKLOCAL:
                lsdb = on->ospf6_if->lsdb;
                break;
            case OSPF6_SCOPE_AREA:
                lsdb = on->ospf6_if->area->lsdb;
                break;
            case OSPF6_SCOPE_AS:
                lsdb = on->ospf6_if->area->ospf6->lsdb;
                break;
            case OSPF6_SCOPE_RESERVED:

                if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
                {
                    zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Ignoring LSA of reserved scope");
                }
                oi->ospf6->o6_err.receive_lsu_unlsa_count ++;

                ospf6_lsa_delete(his);
				his = NULL;
                continue;
                break;
        }

        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
        {
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s acknowledged by %s", his->name, on->name);
        }

        /* Find database copy */
        mine = ospf6_lsdb_lookup(his->header->type, his->header->id,
                                 his->header->adv_router, lsdb);

        if (mine == NULL)
        {
            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "No database copy");
            }

            ospf6_lsa_delete(his);
			his = NULL;
            continue;
        }

        /* Check if the LSA is on his retrans-list */
        mine = ospf6_lsdb_lookup(his->header->type, his->header->id,
                                 his->header->adv_router, on->retrans_list);

        if (mine == NULL)
        {
            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Not on %s's retrans-list", on->name);
            }

            ospf6_lsa_delete(his);
			his = NULL;
            continue;
        }

        if (ospf6_lsa_compare(his, mine) != 0)
        {
            /* Log this questionable acknowledgement,
               and examine the next one. */
            if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Questionable acknowledgement");
            }

            ospf6_lsa_delete(his);
			his = NULL;
            continue;
        }

        if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "Acknowledged, remove from %s's retrans-list",
                       on->name);

        ospf6_decrement_retrans_count(mine);

        if (OSPF6_LSA_IS_MAXAGE(mine))
        {
            ospf6_maxage_remove(on->ospf6_if->area->ospf6);
        }

        ospf6_lsdb_remove(mine, on->retrans_list);
        ospf6_lsa_delete(his);
		his = NULL;
    }

    assert(p == OSPF6_MESSAGE_END(oh));
	return 0;
}

static u_char *recvbuf = NULL;
static u_char *sendbuf = NULL;
static unsigned int iobuflen = 0;

int
ospf6_iobuf_size(unsigned int size)
{
    u_char *recvnew = NULL, *sendnew = NULL;

    if (size <= iobuflen)
    {
        return iobuflen;
    }

    recvnew = XMALLOC(MTYPE_OSPF6_MESSAGE, size);
    sendnew = XMALLOC(MTYPE_OSPF6_MESSAGE, size);

    if (recvnew == NULL || sendnew == NULL)
    {
        if (recvnew)
        {
            XFREE(MTYPE_OSPF6_MESSAGE, recvnew);
        }

        if (sendnew)
        {
            XFREE(MTYPE_OSPF6_MESSAGE, sendnew);
        }
		recvnew = NULL;
		sendnew = NULL;

        zlog_debug(OSPF6_DBG_INTERFACE, "Could not allocate I/O buffer of size %d.", size);
        return iobuflen;
    }

    if (recvbuf)
    {
        XFREE(MTYPE_OSPF6_MESSAGE, recvbuf);
    }

    if (sendbuf)
    {
        XFREE(MTYPE_OSPF6_MESSAGE, sendbuf);
    }

    recvbuf = recvnew;
    sendbuf = sendnew;
    iobuflen = size;
    return iobuflen;
}

void
ospf6_message_terminate(void)
{
    if (recvbuf)
    {
        XFREE(MTYPE_OSPF6_MESSAGE, recvbuf);
        recvbuf = NULL;
    }

    if (sendbuf)
    {
        XFREE(MTYPE_OSPF6_MESSAGE, sendbuf);
        sendbuf = NULL;
    }

    iobuflen = 0;
}
#if 0
int
ospf6_receive(struct thread *thread)
{
    int sockfd;
    unsigned int len;
    char srcname[64], dstname[64];
    struct in6_addr src, dst;
    ifindex_t ifindex;
    struct iovec iovector[2];
    struct ospf6_interface *oi;
    struct ospf6_header *oh;
    /* add next read thread */
    sockfd = THREAD_FD(thread);
    thread_add_read(master_ospf6, ospf6_receive, NULL, sockfd);
    /* initialize */
    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));
    ifindex = 0;
    memset(recvbuf, 0, iobuflen);
    iovector[0].iov_base = recvbuf;
    iovector[0].iov_len = iobuflen;
    iovector[1].iov_base = NULL;
    iovector[1].iov_len = 0;
    /* receive message */
    len = ospf6_recvmsg(&src, &dst, &ifindex, iovector);

    if (len > iobuflen)
    {
        zlog_err("Excess message read");
        return 0;
    }

    oi = ospf6_interface_lookup_by_ifindex(ifindex);

    if (oi == NULL || oi->area == NULL || CHECK_FLAG(oi->flag, OSPF6_INTERFACE_DISABLE))
    {
        zlog_debug("Message received on disabled interface");
        return 0;
    }

    if (CHECK_FLAG(oi->flag, OSPF6_INTERFACE_PASSIVE))
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
            zlog_debug("%s: Ignore message on passive interface %s",
                       __func__, oi->interface->name);

        return 0;
    }

    oh = (struct ospf6_header *) recvbuf;

    if (ospf6_rxpacket_examin(oi, oh, len) != MSG_OK)
    {
        return 0;
    }

    /* Being here means, that no sizing/alignment issues were detected in
       the input packet. This renders the additional checks performed below
       and also in the type-specific dispatching functions a dead code,
       which can be dismissed in a cleanup-focused review round later. */

    /* Log */
    if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
    {
        inet_ntop(AF_INET6, &src, srcname, sizeof(srcname));
        inet_ntop(AF_INET6, &dst, dstname, sizeof(dstname));
        zlog_debug("%s received on %s",
                   LOOKUP(ospf6_message_type_str, oh->type), oi->interface->name);
        zlog_debug("    src: %s", srcname);
        zlog_debug("    dst: %s", dstname);

        switch (oh->type)
        {
            case OSPF6_MESSAGE_TYPE_HELLO:
                ospf6_hello_print(oh);
                break;
            case OSPF6_MESSAGE_TYPE_DBDESC:
                ospf6_dbdesc_print(oh);
                break;
            case OSPF6_MESSAGE_TYPE_LSREQ:
                ospf6_lsreq_print(oh);
                break;
            case OSPF6_MESSAGE_TYPE_LSUPDATE:
                ospf6_lsupdate_print(oh);
                break;
            case OSPF6_MESSAGE_TYPE_LSACK:
                ospf6_lsack_print(oh);
                break;
            default:
                assert(0);
        }
    }

    switch (oh->type)
    {
        case OSPF6_MESSAGE_TYPE_HELLO:
            ospf6_hello_recv(&src, &dst, oi, oh);
            break;
        case OSPF6_MESSAGE_TYPE_DBDESC:
            ospf6_dbdesc_recv(&src, &dst, oi, oh);
            break;
        case OSPF6_MESSAGE_TYPE_LSREQ:
            ospf6_lsreq_recv(&src, &dst, oi, oh);
            break;
        case OSPF6_MESSAGE_TYPE_LSUPDATE:
            ospf6_lsupdate_recv(&src, &dst, oi, oh);
            break;
        case OSPF6_MESSAGE_TYPE_LSACK:
            ospf6_lsack_recv(&src, &dst, oi, oh);
            break;
        default:
            assert(0);
    }

    return 0;
}
#endif

static
int ospf6_read_pkt(struct pkt_buffer *pkt)
{
    struct ospf6_interface *oi = NULL;
    struct ospf6_header *oh = NULL;
    unsigned int len = 0;
    struct in6_addr src, dst;
    char srcname[64], dstname[64];
	int ret = -1;

    memset(recvbuf, 0, iobuflen);
    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));

    /*ip packet header check*/
    if(ipv6_is_zeronet(&(pkt->cb.ipcb.sipv6)) || ipv6_is_zeronet(&(pkt->cb.ipcb.dipv6)))
    {
        zlog_warn("OSPF6 Receive Packet,ip header check failed:sipv6 or dipv6 is 0.");
        return -1;
    }

	/*do not receive form sub interface */
	if (!IFM_IS_SUBPORT(pkt->in_ifindex))
	{
		if(pkt->svlan != 0)
		{
			zlog_debug(OSPF6_DBG_MESSAGE_RECV, "OSPF6 Receive Packet,receive %0x from sub ifindex,vlan is %d.",pkt->in_ifindex,pkt->svlan);
        	return 1;
		}
	}

    oi = ospf6_interface_lookup_by_ifindex(pkt->in_ifindex);

    if (oi == NULL || oi->area == NULL || CHECK_FLAG(oi->flag, OSPF6_INTERFACE_DISABLE))
    {
        zlog_debug(OSPF6_DBG_MESSAGE_RECV, "OSPF6 Receive Packet,Message received on disabled interface");
        return -1;
    }
	if(recvbuf == NULL)
	{
	 	zlog_debug(OSPF6_DBG_MESSAGE_RECV, "OSPF6 Receive Packet,ifindex %0x recvbuf is NULL",pkt->in_ifindex);
		return -1;
	}

    memcpy(recvbuf, pkt->data, pkt->data_len);
    len = pkt->data_len;
    //recvbuf = (u_char *)pkt->data;

    IPV6_ADDR_COPY(&src, pkt->cb.ipcb.sipv6.ipv6);
    IPV6_ADDR_COPY(&dst, pkt->cb.ipcb.dipv6.ipv6);


    if (CHECK_FLAG(oi->flag, OSPF6_INTERFACE_PASSIVE))
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: Ignore message on passive interface %s",
                       __func__, oi->interface->name);

        return -1;
    }
    
    if (oi->state == OSPF6_INTERFACE_DOWN)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
            zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s: Ignore message because interface is down %s",
               __func__, oi->interface->name);
        oi->ospf6->o6_err.receive_down_count ++;
        return -1;
    }

    oh = (struct ospf6_header *) recvbuf;

    if (ospf6_rxpacket_examin(oi, oh, len) != MSG_OK)
    {
        return -1;
    }

    /* Being here means, that no sizing/alignment issues were detected in
       the input packet. This renders the additional checks performed below
       and also in the type-specific dispatching functions a dead code,
       which can be dismissed in a cleanup-focused review round later. */

    /* Log */
    if (IS_OSPF6_DEBUG_MESSAGE(oh->type, RECV))
    {
        inet_ntop(AF_INET6, &src, srcname, sizeof(srcname));
        inet_ntop(AF_INET6, &dst, dstname, sizeof(dstname));
        zlog_debug(OSPF6_DBG_MESSAGE_RECV, "%s received on %s",
                   LOOKUP(ospf6_message_type_str, oh->type), oi->interface->name);
        zlog_debug(OSPF6_DBG_MESSAGE_RECV, "    src: %s", srcname);
        zlog_debug(OSPF6_DBG_MESSAGE_RECV, "    dst: %s", dstname);

        switch (oh->type)
        {
            case OSPF6_MESSAGE_TYPE_HELLO:
                ospf6_hello_print(oh);
                break;
            case OSPF6_MESSAGE_TYPE_DBDESC:
                ospf6_dbdesc_print(oh);
                break;
            case OSPF6_MESSAGE_TYPE_LSREQ:
                ospf6_lsreq_print(oh);
                break;
            case OSPF6_MESSAGE_TYPE_LSUPDATE:
                ospf6_lsupdate_print(oh);
                break;
            case OSPF6_MESSAGE_TYPE_LSACK:
                ospf6_lsack_print(oh);
                break;
            default:
                assert(0);
        }
    }

    switch (oh->type)
    {
        case OSPF6_MESSAGE_TYPE_HELLO:
            ret = ospf6_hello_recv(&src, &dst, oi, oh);
            break;
        case OSPF6_MESSAGE_TYPE_DBDESC:
            ret = ospf6_dbdesc_recv(&src, &dst, oi, oh);
            break;
        case OSPF6_MESSAGE_TYPE_LSREQ:
            ret = ospf6_lsreq_recv(&src, &dst, oi, oh);
            break;
        case OSPF6_MESSAGE_TYPE_LSUPDATE:
            ret = ospf6_lsupdate_recv(&src, &dst, oi, oh);
            break;
        case OSPF6_MESSAGE_TYPE_LSACK:
            ret = ospf6_lsack_recv(&src, &dst, oi, oh);
            break;
        default:
            assert(0);
    }

    return ret;
}

#if 0
int ospf6_pkt_rcv(struct thread *thread)
{
    //struct thread *t_read = NULL;
    struct pkt_buffer *pkt = NULL;
    int ret = -1;
	om6->t_read_pkt = NULL;

    /* once receive 101 packet*/
    for (int i = 500; i >= 0; i--)
    {
        pkt = pkt_rcv(MODULE_ID_OSPF6);

        if (pkt == NULL)
        {
            break;/* 退出 while 循环 */
        }

        ret = ospf6_read_pkt(pkt);
		om6->ospf6_statics.recv_all ++;
		if(ret == -1)
			om6->ospf6_statics.recv_error ++;
		
    }

    do
    {
        usleep(10000);/* CPU 10ms */
        om6->t_read_pkt = thread_add_event(master_ospf6, ospf6_pkt_rcv, NULL, 0);

        if (om6->t_read_pkt == NULL)
        {
            zlog_err("%s():%d: ospfd thread_add_event() add ospf_msg_rcv() failed!", __FUNCTION__, __LINE__);
        }
    }
    while (om6->t_read_pkt == NULL);

    return 1;
}
#endif

int ospf6_pkt_rcv_new(struct ipc_mesg_n *pmsg)
{
    struct pkt_buffer *pkt = NULL;
    int ret = -1;

	zlog_debug(OSPF6_DBG_OTHER, "%s[%d]: sender_id:%d msg_type:%d msg_subtype:%d\n", __FUNCTION__, __LINE__,\
								pmsg->msghdr.sender_id, pmsg->msghdr.msg_type, pmsg->msghdr.msg_subtype);
	
	pkt = (struct pkt_buffer *)pmsg->msg_data;
	if (pkt == NULL)
    {
    	zlog_err("%-15s[%d]: pkt from ipc is NULL!", __FUNCTION__, __LINE__);
        return -1;
    }
	APP_RECV_PKT_FROM_FTM_DATA_SET(pkt);
	
	/*if(pkt->data == NULL)
	{
		zlog_err("%-15s[%d]: pkt->data is NULL! data_len = %d", __FUNCTION__, __LINE__, pkt->data_len);
		return -1;
	}*/
    
    ret = ospf6_read_pkt(pkt);
	om6->ospf6_statics.recv_all ++;
	if(ret == -1)
	{
		om6->ospf6_statics.recv_error ++;
	}

    return 1;
}


#define OSPF_WRITE_IPHL_SHIFT 2


static int
ospf6_write(struct in6_addr *src, struct in6_addr *dst,
            ifindex_t *ifindex , struct ospf6_header *oh)
{
    union pkt_control pkt_ctrl;
    int ret = -1;
    void *pdata = NULL;
    struct ipv6_hdr iphv6;

    /*组播报文填写*/
    if (strcmp(inet6_ntoa(*dst), ALLSPFROUTERS6)
            || strcmp(inet6_ntoa(*dst), ALLDROUTERS6))
    {
        /* 封装 ipv6头部 */
        memset(&iphv6, 0 , sizeof(struct ipv6_hdr));
        iphv6.version = 6;
        iphv6.tos1 = 0xc;
        iphv6.data_len = oh->length;
        iphv6.ttl = 1;
        iphv6.protocol = IPPROTO_OSPFIGP;
        IPV6_ADDR_COPY(iphv6.saddr.ipv6, src);
        IPV6_ADDR_COPY(iphv6.daddr.ipv6, dst);

        //pdata = malloc(sizeof(struct ipv6_hdr) + ntohs(oh->length));
		pdata = XMALLOC(MTYPE_TMP, sizeof(struct ipv6_hdr) + ntohs(oh->length));
        memcpy(pdata, &iphv6, sizeof(struct ipv6_hdr));
        memcpy((char*)pdata + sizeof(struct ipv6_hdr), (char*)oh, ntohs(oh->length));
		
    }

	if (!strcmp(inet6_ntoa(*dst), ALLSPFROUTERS6))
    {
        memset(&pkt_ctrl, 0, sizeof(pkt_ctrl));
        pkt_ctrl.ethcb.dmac[0] = 0x33;
        pkt_ctrl.ethcb.dmac[1] = 0x33;
        pkt_ctrl.ethcb.dmac[2] = 0x00;
        pkt_ctrl.ethcb.dmac[3] = 0x00;
        pkt_ctrl.ethcb.dmac[4] = 0x00;
        pkt_ctrl.ethcb.dmac[5] = 0x05;


        pkt_ctrl.ethcb.ifindex = *ifindex;
        pkt_ctrl.ethcb.ethtype = ETH_P_IPV6;
        pkt_ctrl.ethcb.cos = IPTOS_PREC_INTERNETCONTROL;
		if (IS_OSPF6_DEBUG_MESSAGE(oh->type, SEND))
		    zlog_debug(OSPF6_DBG_MESSAGE_SEND, "ospf_send use PKT_TYPE_ETH send multicast 05 packet via 0x%0x(0x%0x)",pkt_ctrl.ethcb.ifindex, *ifindex);

        ret = pkt_send(PKT_TYPE_ETH, &pkt_ctrl, pdata, sizeof(struct ipv6_hdr) + ntohs(oh->length));
    }

    else if (!strcmp(inet6_ntoa(*dst), ALLDROUTERS6))
    {
        memset(&pkt_ctrl, 0, sizeof(pkt_ctrl));
        pkt_ctrl.ethcb.dmac[0] = 0x33;
        pkt_ctrl.ethcb.dmac[1] = 0x33;
        pkt_ctrl.ethcb.dmac[2] = 0x00;
        pkt_ctrl.ethcb.dmac[3] = 0x00;
        pkt_ctrl.ethcb.dmac[4] = 0x00;
        pkt_ctrl.ethcb.dmac[5] = 0x06;

        pkt_ctrl.ethcb.ifindex = *ifindex;
        pkt_ctrl.ethcb.ethtype = ETH_P_IPV6;
        pkt_ctrl.ethcb.cos = IPTOS_PREC_INTERNETCONTROL;
		if (IS_OSPF6_DEBUG_MESSAGE(oh->type, SEND))
        	zlog_debug(OSPF6_DBG_MESSAGE_SEND, "ospf_send use PKT_TYPE_ETH send multicast packet via 0x%0x(0x%0x)",pkt_ctrl.ethcb.ifindex,*ifindex);
        ret = pkt_send(PKT_TYPE_ETH, &pkt_ctrl, pdata, sizeof(struct ipv6_hdr) + ntohs(oh->length));

    }
    else
    {
        /*PKT_TYPE_IPV6 */
        memset(&pkt_ctrl, 0, sizeof(pkt_ctrl));

        IPV6_ADDR_COPY(pkt_ctrl.ipcb.sipv6.ipv6, src);
        IPV6_ADDR_COPY(pkt_ctrl.ipcb.dipv6.ipv6, dst);
        pkt_ctrl.ipcb.is_changed = 1;
        pkt_ctrl.ipcb.vpn = 0;
        pkt_ctrl.ipcb.ttl = 1;
        pkt_ctrl.ipcb.pkt_type = PKT_TYPE_IPV6;
        pkt_ctrl.ipcb.tos = 3;
        pkt_ctrl.ipcb.protocol = IP_P_OSPF;
        pkt_ctrl.ipcb.ifindex = *ifindex;
		
		if (IS_OSPF6_DEBUG_MESSAGE(oh->type, SEND))
        	zlog_debug(OSPF6_DBG_MESSAGE_SEND, "ospf_send use PKT_TYPE_IPV6 send IPV6 packet via 0x%0x(0x%0x)",pkt_ctrl.ipcb.ifindex,*ifindex);

        ret = pkt_send(PKT_TYPE_IPV6, &pkt_ctrl, oh, ntohs(oh->length));

    }

    if (pdata != NULL)
    {
    	XFREE(MTYPE_TMP, pdata);
		pdata = NULL;
        //free(pdata);
    }

    return ret;
}

static void
ospf6_send(struct in6_addr *src, struct in6_addr *dst,
           struct ospf6_interface *oi, struct ospf6_header *oh)
{
    char srcname[64], dstname[64];
    struct prefix psrc , pdst;
    int ret = -1;

    /* fill OSPF header */
    oh->version = OSPFV3_VERSION;
    /* message type must be set before */
    /* message length must be set before */
    oh->router_id = oi->area->ospf6->router_id;
    oh->area_id = oi->area->area_id;
    /* checksum is calculated by kernel */
    oh->instance_id = oi->instance_id;
    oh->reserved = 0;

    /*OSPF checksum calculate*/
    memset(&psrc, 0, sizeof(struct prefix));
    memset(&pdst, 0, sizeof(struct prefix));
    IPV6_ADDR_COPY(&psrc.u.prefix6, src);
    IPV6_ADDR_COPY(&pdst.u.prefix6, dst);
    oh->checksum = in6_checksum(&psrc, &pdst, IPPROTO_OSPFIGP, (uint16_t *)oh, ntohs(oh->length));

    /* Log */
    if (IS_OSPF6_DEBUG_MESSAGE(oh->type, SEND))
    {
        inet_ntop(AF_INET6, dst, dstname, sizeof(dstname));

        if (src)
        {
            inet_ntop(AF_INET6, src, srcname, sizeof(srcname));
        }
        else
        {
            memset(srcname, 0, sizeof(srcname));
        }

        zlog_debug(OSPF6_DBG_MESSAGE_SEND, "%s send on %s",
                   LOOKUP(ospf6_message_type_str, oh->type), oi->interface->name);
        zlog_debug(OSPF6_DBG_MESSAGE_SEND, "    src: %s", srcname);
        zlog_debug(OSPF6_DBG_MESSAGE_SEND, "    dst: %s", dstname);

        switch (oh->type)
        {
            case OSPF6_MESSAGE_TYPE_HELLO:
                ospf6_hello_print(oh);
                break;
            case OSPF6_MESSAGE_TYPE_DBDESC:
                ospf6_dbdesc_print(oh);
                break;
            case OSPF6_MESSAGE_TYPE_LSREQ:
                ospf6_lsreq_print(oh);
                break;
            case OSPF6_MESSAGE_TYPE_LSUPDATE:
                ospf6_lsupdate_print(oh);
                break;
            case OSPF6_MESSAGE_TYPE_LSACK:
                ospf6_lsack_print(oh);
                break;
            default:
                zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Unknown message");
                assert(0);
                break;
        }
    }

    ret = ospf6_write(src, dst, &oi->interface->ifindex, oh);
	om6->ospf6_statics.send_all ++;

    if (ret == NET_FAILED)
    {
    	om6->ospf6_statics.send_error ++;
        zlog_debug(OSPF6_DBG_MESSAGE_SEND, "ospf_send PKT_TYPE_IP error dip");
    }

}

static uint32_t
ospf6_packet_max(struct ospf6_interface *oi)
{
    assert(oi->ifmtu > sizeof(struct ip6_hdr));
    return oi->ifmtu - (sizeof(struct ip6_hdr));
}

int
ospf6_hello_send(void *thread)
{
    struct ospf6_interface *oi = NULL;
    struct ospf6_header *oh = NULL;
    struct ospf6_hello *hello = NULL;
    u_char *p = NULL;
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf6_neighbor *on = NULL;

    oi = (struct ospf6_interface *)(thread);
    assert(oi);

    oi->thread_send_hello = 0;
	if(oi->linklocal_addr == NULL)
	{
		if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_HELLO, SEND))
			zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Unable to send Hello on linklocal is NULL interface %s",oi->interface->name);
		return 0;
	}

    if (oi->state <= OSPF6_INTERFACE_DOWN)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_HELLO, SEND))
            zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Unable to send Hello on down interface %s",
                       oi->interface->name);

        return 0;
    }

    if (iobuflen == 0)
    {
        zlog_err("Unable to send Hello on interface %s iobuflen is 0",
                 oi->interface->name);

        return 0;
    }

    /* set next thread */
    /*oi->thread_send_hello = thread_add_timer(master_ospf6, ospf6_hello_send,
                            oi, oi->hello_interval);*/
	oi->thread_send_hello = high_pre_timer_add ((char *)"ospf6_hello_timer", 
					LIB_TIMER_TYPE_NOLOOP, ospf6_hello_send, (void *)oi, oi->hello_interval*1000);
    memset(sendbuf, 0, iobuflen);
    oh = (struct ospf6_header *) sendbuf;
    hello = (struct ospf6_hello *)((caddr_t) oh + sizeof(struct ospf6_header));
    hello->interface_id = htonl(oi->interface->ifindex);
    hello->priority = oi->priority;
    hello->options[0] = oi->area->options[0];
    hello->options[1] = oi->area->options[1];
    hello->options[2] = oi->area->options[2];
    hello->hello_interval = htons(oi->hello_interval);
    hello->dead_interval = htons(oi->dead_interval);
    hello->drouter = oi->drouter;
    hello->bdrouter = oi->bdrouter;
    p = (u_char *)((caddr_t) hello + sizeof(struct ospf6_hello));

    for (ALL_LIST_ELEMENTS(oi->neighbor_list, node, nnode, on))
    {
        if (on->state < OSPF6_NEIGHBOR_INIT)
        {
            continue;
        }

        if (p - sendbuf + sizeof(u_int32_t) > ospf6_packet_max(oi))
        {
            if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_HELLO, SEND))
            {
                zlog_debug(OSPF6_DBG_MESSAGE_SEND, "sending Hello message: exceeds I/F MTU");
            }

            break;
        }

        memcpy(p, &on->router_id, sizeof(u_int32_t));
        p += sizeof(u_int32_t);
    }

    oh->type = OSPF6_MESSAGE_TYPE_HELLO;
    oh->length = htons(p - sendbuf);
	om6->ospf6_statics.s_hel ++;
	if(oi->linklocal_addr != NULL)
    	ospf6_send(oi->linklocal_addr, &allspfrouters6, oi, oh);
    return 0;
}

int
ospf6_dbdesc_send(void *thread)
{
    struct ospf6_neighbor *on = NULL;
    struct ospf6_header *oh = NULL;
    struct ospf6_dbdesc *dbdesc = NULL;
    u_char *p = NULL;
    struct ospf6_lsa *lsa = NULL;
    struct in6_addr *dst = NULL;
    on = (struct ospf6_neighbor *)(thread);
    assert(on);

    on->thread_send_dbdesc = 0;
	
	if(on->ospf6_if == NULL)
	{
	    zlog_debug(OSPF6_DBG_MESSAGE_SEND, "%s %d on->ospf6_if is NULL",__func__,__LINE__);
		return 0;
	}
	
	if(on->ospf6_if->linklocal_addr == NULL)
	{
		if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_DBDESC, SEND))
			zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Unable to send DbDesc on linklocal is NULL interface %s",on->ospf6_if->interface->name);
		return 0;
	}

    if (on->state < OSPF6_NEIGHBOR_EXSTART)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_DBDESC, SEND))
            zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Quit to send DbDesc to neighbor %s state %s",
                       on->name, ospf6_neighbor_state_str[on->state]);

        return 0;
    }

    /* set next thread if master */
    if (CHECK_FLAG(on->dbdesc_bits, OSPF6_DBDESC_MSBIT))
    {
        /*on->thread_send_dbdesc =
            thread_add_timer(master_ospf6, ospf6_dbdesc_send, on,
                             on->ospf6_if->rxmt_interval);*/
		on->thread_send_dbdesc =
			high_pre_timer_add ((char *)"ospf6_dd_send_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_dbdesc_send, (void *)on, (on->ospf6_if->rxmt_interval)*1000);
    }

    memset(sendbuf, 0, iobuflen);
    oh = (struct ospf6_header *) sendbuf;
    dbdesc = (struct ospf6_dbdesc *)((caddr_t) oh +
                                     sizeof(struct ospf6_header));

    /* if this is initial one, initialize sequence number for DbDesc */
    if (CHECK_FLAG(on->dbdesc_bits, OSPF6_DBDESC_IBIT) &&
            (on->dbdesc_seqnum == 0))
    {
        struct timeval tv;

        if (time_get_time(TIME_CLK_MONOTONIC, &tv) < 0)
        {
            tv.tv_sec = 1;
        }

        on->dbdesc_seqnum = tv.tv_sec;
    }

    dbdesc->options[0] = on->ospf6_if->area->options[0];
    dbdesc->options[1] = on->ospf6_if->area->options[1];
    dbdesc->options[2] = on->ospf6_if->area->options[2];
    dbdesc->ifmtu = htons(on->ospf6_if->ifmtu);
    dbdesc->bits = on->dbdesc_bits;
    dbdesc->seqnum = htonl(on->dbdesc_seqnum);
    /* if this is not initial one, set LSA headers in dbdesc */
    p = (u_char *)((caddr_t) dbdesc + sizeof(struct ospf6_dbdesc));

    if (! CHECK_FLAG(on->dbdesc_bits, OSPF6_DBDESC_IBIT))
    {
        for (lsa = ospf6_lsdb_head(on->dbdesc_list); lsa;
                lsa = ospf6_lsdb_next(lsa))
        {
            ospf6_lsa_age_update_to_send(lsa, on->ospf6_if->transdelay);

            /* MTU check */
            if (p - sendbuf + sizeof(struct ospf6_lsa_header) >
                    ospf6_packet_max(on->ospf6_if))
            {
                ospf6_lsdb_lsa_unlock(lsa);
                break;
            }

            memcpy(p, lsa->header, sizeof(struct ospf6_lsa_header));
            p += sizeof(struct ospf6_lsa_header);
        }
    }

    oh->type = OSPF6_MESSAGE_TYPE_DBDESC;
    oh->length = htons(p - sendbuf);

    if (on->ospf6_if->state == OSPF6_INTERFACE_POINTTOPOINT)
    {
        dst = &allspfrouters6;
    }
    else
    {
        dst = &on->linklocal_addr;
    }
	om6->ospf6_statics.s_dd ++;

    ospf6_send(on->ospf6_if->linklocal_addr, dst, on->ospf6_if, oh);
    return 0;
}

int
ospf6_dbdesc_send_newone(void *thread)
{
    struct ospf6_neighbor *on = NULL;
    struct ospf6_lsa *lsa = NULL;
    unsigned int size = 0;
    on = (struct ospf6_neighbor *)(thread);
    assert(on);
    ospf6_lsdb_remove_all(on->dbdesc_list);
    /* move LSAs from summary_list to dbdesc_list (within neighbor structure)
       so that ospf6_send_dbdesc () can send those LSAs */
    size = sizeof(struct ospf6_lsa_header) + sizeof(struct ospf6_dbdesc);

    for (lsa = ospf6_lsdb_head(on->summary_list); lsa;
            lsa = ospf6_lsdb_next(lsa))
    {
        if (size + sizeof(struct ospf6_lsa_header) > ospf6_packet_max(on->ospf6_if))
        {
            ospf6_lsdb_lsa_unlock(lsa);
            break;
        }

        ospf6_lsdb_add(ospf6_lsa_copy(lsa), on->dbdesc_list);
        ospf6_lsdb_remove(lsa, on->summary_list);
        size += sizeof(struct ospf6_lsa_header);
    }

    if (on->summary_list->count == 0)
    {
        UNSET_FLAG(on->dbdesc_bits, OSPF6_DBDESC_MBIT);
    }

    /* If slave, More bit check must be done here */
    if (! CHECK_FLAG(on->dbdesc_bits, OSPF6_DBDESC_MSBIT) &&  /* Slave */
            ! CHECK_FLAG(on->dbdesc_last.bits, OSPF6_DBDESC_MBIT) &&
            ! CHECK_FLAG(on->dbdesc_bits, OSPF6_DBDESC_MBIT))
    {
        //thread_add_event(master_ospf6, exchange_done, on, 0);
        thread_execute(master_ospf6, exchange_done, on, 0);
    }

    //thread_execute(master_ospf6, ospf6_dbdesc_send, on, 0);
	high_pre_timer_add ((char *)"ospf6_dd_send_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_dbdesc_send, (void *)on, 10);
    return 0;
}

int
ospf6_lsreq_send(void *thread)
{
    struct ospf6_neighbor *on = NULL;
    struct ospf6_header *oh = NULL;
    struct ospf6_lsreq_entry *e = NULL;
    u_char *p = NULL;
    struct ospf6_lsa *lsa = NULL, *last_req = NULL;
    on = (struct ospf6_neighbor *)(thread);
    assert(on);
    on->thread_send_lsreq = 0;

	if(on->ospf6_if->linklocal_addr == NULL)
	{
		if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_LSREQ, SEND))
			zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Unable to send LSReq on linklocal is NULL interface %s",on->ospf6_if->interface->name);
		return 0;
	}

    /* LSReq will be sent only in ExStart or Loading */
    if (on->state != OSPF6_NEIGHBOR_EXCHANGE &&
            on->state != OSPF6_NEIGHBOR_LOADING)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_LSREQ, SEND))
            zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Quit to send LSReq to neighbor %s state %s",
                       on->name, ospf6_neighbor_state_str[on->state]);

        return 0;
    }

    /* schedule loading_done if request list is empty */
    if (on->request_list->count == 0)
    {
        //thread_add_event(master_ospf6, loading_done, on, 0);
        thread_execute(master_ospf6, loading_done, on, 0);
        return 0;
    }

    memset(sendbuf, 0, iobuflen);
    oh = (struct ospf6_header *) sendbuf;
    last_req = NULL;
    /* set Request entries in lsreq */
    p = (u_char *)((caddr_t) oh + sizeof(struct ospf6_header));

    for (lsa = ospf6_lsdb_head(on->request_list); lsa;
            lsa = ospf6_lsdb_next(lsa))
    {
        /* MTU check */
        if (p - sendbuf + sizeof(struct ospf6_lsreq_entry) > ospf6_packet_max(on->ospf6_if))
        {
            ospf6_lsdb_lsa_unlock(lsa);
            break;
        }

        e = (struct ospf6_lsreq_entry *) p;
        e->type = lsa->header->type;
        e->id = lsa->header->id;
        e->adv_router = lsa->header->adv_router;
        p += sizeof(struct ospf6_lsreq_entry);
        last_req = lsa;
    }

    if (last_req != NULL)
    {
        if (on->last_ls_req != NULL)
        {
            ospf6_lsa_unlock(on->last_ls_req);
        }

        ospf6_lsa_lock(last_req);
        on->last_ls_req = last_req;
    }

    oh->type = OSPF6_MESSAGE_TYPE_LSREQ;
    oh->length = htons(p - sendbuf);
	om6->ospf6_statics.s_lsr ++;

    if (on->ospf6_if->state == OSPF6_INTERFACE_POINTTOPOINT)
        ospf6_send(on->ospf6_if->linklocal_addr, &allspfrouters6,
                   on->ospf6_if, oh);
    else
        ospf6_send(on->ospf6_if->linklocal_addr, &on->linklocal_addr,
                   on->ospf6_if, oh);

    /* set next thread */
    if (on->request_list->count != 0)
    {
        /*on->thread_send_lsreq =
            thread_add_timer(master_ospf6, ospf6_lsreq_send, on,
                             on->ospf6_if->rxmt_interval);*/
		on->thread_send_lsreq =
			high_pre_timer_add ((char *)"ospf6_lr_send_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_lsreq_send, (void *)on, (on->ospf6_if->rxmt_interval)*1000);
    }

    return 0;
}

int
ospf6_lsupdate_send_neighbor(void *thread)
{
    struct ospf6_neighbor *on = NULL;
    struct ospf6_header *oh = NULL;
    struct ospf6_lsupdate *lsupdate = NULL;
    u_char *p = NULL;
    int lsa_cnt;
    struct ospf6_lsa *lsa = NULL;
    on = (struct ospf6_neighbor *)(thread);
    assert(on);
	if(!on)
	{
		zlog_err("%s[%d]: invalid argv!\n", __FUNCTION__, __LINE__);
		return 0;
	}

    on->thread_send_lsupdate = 0;

    if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_LSUPDATE, SEND))
    {
        zlog_debug(OSPF6_DBG_MESSAGE_SEND, "LSUpdate to neighbor %s", on->name);
    }

	if(on->ospf6_if->linklocal_addr == NULL)
	{
		if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_LSUPDATE, SEND))
			zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Unable to send LSU on linklocal is NULL interface %s",on->ospf6_if->interface->name);
		return 0;
	}

    if (on->state < OSPF6_NEIGHBOR_EXCHANGE)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_LSUPDATE, SEND))
            zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Quit to send (neighbor state %s)",
                       ospf6_neighbor_state_str[on->state]);

        return 0;
    }

    memset(sendbuf, 0, iobuflen);
    oh = (struct ospf6_header *) sendbuf;
    lsupdate = (struct ospf6_lsupdate *)
               ((caddr_t) oh + sizeof(struct ospf6_header));
    p = (u_char *)((caddr_t) lsupdate + sizeof(struct ospf6_lsupdate));
    lsa_cnt = 0;

    /* lsupdate_list lists those LSA which doesn't need to be
       retransmitted. remove those from the list */
    for (lsa = ospf6_lsdb_head(on->lsupdate_list); lsa;
            lsa = ospf6_lsdb_next(lsa))
    {
        /* MTU check */
        if ((p - sendbuf + (unsigned int)OSPF6_LSA_SIZE(lsa->header))
                > ospf6_packet_max(on->ospf6_if))
        {
            ospf6_lsdb_lsa_unlock(lsa);
            break;
        }

        ospf6_lsa_age_update_to_send(lsa, on->ospf6_if->transdelay);
        memcpy(p, lsa->header, OSPF6_LSA_SIZE(lsa->header));
        p += OSPF6_LSA_SIZE(lsa->header);
        lsa_cnt++;
        assert(lsa->lock == 2);
        ospf6_lsdb_remove(lsa, on->lsupdate_list);
    }

    if (lsa_cnt)
    {
        oh->type = OSPF6_MESSAGE_TYPE_LSUPDATE;
        oh->length = htons(p - sendbuf);
        lsupdate->lsa_number = htonl(lsa_cnt);

        if ((on->ospf6_if->state == OSPF6_INTERFACE_POINTTOPOINT) ||
                (on->ospf6_if->state == OSPF6_INTERFACE_DR) ||
                (on->ospf6_if->state == OSPF6_INTERFACE_BDR))
            ospf6_send(on->ospf6_if->linklocal_addr, &allspfrouters6,
                       on->ospf6_if, oh);
        else
            ospf6_send(on->ospf6_if->linklocal_addr, &on->linklocal_addr,
                       on->ospf6_if, oh);
    }

    /* The addresses used for retransmissions are different from those sent the
       first time and so we need to separate them here.
    */
    memset(sendbuf, 0, iobuflen);
    oh = (struct ospf6_header *) sendbuf;
    lsupdate = (struct ospf6_lsupdate *)
               ((caddr_t) oh + sizeof(struct ospf6_header));
    p = (u_char *)((caddr_t) lsupdate + sizeof(struct ospf6_lsupdate));
    lsa_cnt = 0;

    for (lsa = ospf6_lsdb_head(on->retrans_list); lsa;
            lsa = ospf6_lsdb_next(lsa))
    {
        /* MTU check */
        if ((p - sendbuf + (unsigned int)OSPF6_LSA_SIZE(lsa->header))
                > ospf6_packet_max(on->ospf6_if))
        {
            ospf6_lsdb_lsa_unlock(lsa);
            break;
        }

        ospf6_lsa_age_update_to_send(lsa, on->ospf6_if->transdelay);
        memcpy(p, lsa->header, OSPF6_LSA_SIZE(lsa->header));
        p += OSPF6_LSA_SIZE(lsa->header);
        lsa_cnt++;
    }

    if (lsa_cnt)
    {
        oh->type = OSPF6_MESSAGE_TYPE_LSUPDATE;
        oh->length = htons(p - sendbuf);
        lsupdate->lsa_number = htonl(lsa_cnt);
		om6->ospf6_statics.s_lsu ++;

        if (on->ospf6_if->state == OSPF6_INTERFACE_POINTTOPOINT)
            ospf6_send(on->ospf6_if->linklocal_addr, &allspfrouters6,
                       on->ospf6_if, oh);
        else
            ospf6_send(on->ospf6_if->linklocal_addr, &on->linklocal_addr,
                       on->ospf6_if, oh);
    }

    if (on->lsupdate_list->count != 0)
	{
    	/*on->thread_send_lsupdate =
        	thread_add_event(master_ospf6, ospf6_lsupdate_send_neighbor, on, 0);*/
		on->thread_send_lsupdate =
			high_pre_timer_add ((char *)"ospf6_lsupdate_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_lsupdate_send_neighbor, (void *)on, 10);
	}
    else if (on->retrans_list->count != 0)
	{
    	/*on->thread_send_lsupdate =
        	thread_add_timer(master_ospf6, ospf6_lsupdate_send_neighbor, on,
                         on->ospf6_if->rxmt_interval);*/
		on->thread_send_lsupdate =
			high_pre_timer_add ((char *)"ospf6_lsupdate_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_lsupdate_send_neighbor, (void *)on, (on->ospf6_if->rxmt_interval)*1000);
	}

    return 0;
}

int
ospf6_lsupdate_send_interface(struct thread *thread)
{
    struct ospf6_interface *oi = NULL;
    struct ospf6_header *oh = NULL;
    struct ospf6_lsupdate *lsupdate = NULL;
    u_char *p = NULL;
    int lsa_cnt;
    struct ospf6_lsa *lsa = NULL;
    oi = (struct ospf6_interface *) THREAD_ARG(thread);
    assert(oi);
    oi->thread_send_lsupdate = (struct thread *) NULL;

	if(oi->linklocal_addr == NULL)
	{
		if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_LSUPDATE, SEND))
			zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Unable to send LSU on linklocal is NULL interface %s",oi->interface->name);
		return 0;
	}

    if (oi->state <= OSPF6_INTERFACE_WAITING)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_LSUPDATE, SEND))
            zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Quit to send LSUpdate to interface %s state %s",
                       oi->interface->name, ospf6_interface_state_str[oi->state]);

        return 0;
    }

    /* if we have nothing to send, return */
    if (oi->lsupdate_list->count == 0)
    {
        return 0;
    }

    memset(sendbuf, 0, iobuflen);
    oh = (struct ospf6_header *) sendbuf;
    lsupdate = (struct ospf6_lsupdate *)((caddr_t) oh +
                                         sizeof(struct ospf6_header));
    p = (u_char *)((caddr_t) lsupdate + sizeof(struct ospf6_lsupdate));
    lsa_cnt = 0;

    for (lsa = ospf6_lsdb_head(oi->lsupdate_list); lsa;
            lsa = ospf6_lsdb_next(lsa))
    {
        /* MTU check */
        if ((p - sendbuf + ((unsigned int)OSPF6_LSA_SIZE(lsa->header)))
                > ospf6_packet_max(oi))
        {
            ospf6_lsdb_lsa_unlock(lsa);
            break;
        }

        ospf6_lsa_age_update_to_send(lsa, oi->transdelay);
        memcpy(p, lsa->header, OSPF6_LSA_SIZE(lsa->header));
        p += OSPF6_LSA_SIZE(lsa->header);
        lsa_cnt++;
        assert(lsa->lock == 2);
        ospf6_lsdb_remove(lsa, oi->lsupdate_list);
    }

    if (lsa_cnt)
    {
        lsupdate->lsa_number = htonl(lsa_cnt);
        oh->type = OSPF6_MESSAGE_TYPE_LSUPDATE;
        oh->length = htons(p - sendbuf);
		om6->ospf6_statics.s_lsu ++;

        if ((oi->state == OSPF6_INTERFACE_POINTTOPOINT) ||
                (oi->state == OSPF6_INTERFACE_DR) ||
                (oi->state == OSPF6_INTERFACE_BDR))
        {
            ospf6_send(oi->linklocal_addr, &allspfrouters6, oi, oh);
        }
        else
        {
            ospf6_send(oi->linklocal_addr, &alldrouters6, oi, oh);
        }
    }

    if (oi->lsupdate_list->count > 0)
    {
        oi->thread_send_lsupdate =
            thread_add_event(master_ospf6, ospf6_lsupdate_send_interface, oi, 0);
    }

    return 0;
}

int
ospf6_lsack_send_neighbor(struct thread *thread)
{
    struct ospf6_neighbor *on = NULL;
    struct ospf6_header *oh = NULL;
    u_char *p = NULL;
    struct ospf6_lsa *lsa = NULL;
    int lsa_cnt = 0;
    on = (struct ospf6_neighbor *) THREAD_ARG(thread);
    assert(on);

    on->thread_send_lsack = (struct thread *) NULL;

    if (on->state < OSPF6_NEIGHBOR_EXCHANGE)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_LSACK, SEND))
            zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Quit to send LSAck to neighbor %s state %s",
                       on->name, ospf6_neighbor_state_str[on->state]);

        return 0;
    }

    /* if we have nothing to send, return */
    if (on->lsack_list->count == 0)
    {
        return 0;
    }

    memset(sendbuf, 0, iobuflen);
    oh = (struct ospf6_header *) sendbuf;
    p = (u_char *)((caddr_t) oh + sizeof(struct ospf6_header));

    for (lsa = ospf6_lsdb_head(on->lsack_list); lsa;
            lsa = ospf6_lsdb_next(lsa))
    {
        /* MTU check */
        if (p - sendbuf + sizeof(struct ospf6_lsa_header) > ospf6_packet_max(on->ospf6_if))
        {
            /* if we run out of packet size/space here,
               better to try again soon. */
            THREAD_OFF(on->thread_send_lsack);
            on->thread_send_lsack =
                thread_add_event(master_ospf6, ospf6_lsack_send_neighbor, on, 0);
            ospf6_lsdb_lsa_unlock(lsa);
            break;
        }

        ospf6_lsa_age_update_to_send(lsa, on->ospf6_if->transdelay);
        memcpy(p, lsa->header, sizeof(struct ospf6_lsa_header));
        p += sizeof(struct ospf6_lsa_header);
        assert(lsa->lock == 2);
        ospf6_lsdb_remove(lsa, on->lsack_list);
        lsa_cnt++;
    }

    if (lsa_cnt)
    {
		om6->ospf6_statics.s_lsa ++;
        oh->type = OSPF6_MESSAGE_TYPE_LSACK;
        oh->length = htons(p - sendbuf);
        ospf6_send(on->ospf6_if->linklocal_addr, &on->linklocal_addr,
                   on->ospf6_if, oh);
    }

    if (on->thread_send_lsack == NULL && on->lsack_list->count > 0)
    {
        on->thread_send_lsack =
            thread_add_event(master_ospf6, ospf6_lsack_send_neighbor, on, 0);
    }

    return 0;
}

int
ospf6_lsack_send_interface(void *thread)
{
    struct ospf6_interface *oi = NULL;
    struct ospf6_header *oh = NULL;
    u_char *p = NULL;
    struct ospf6_lsa *lsa = NULL;
    int lsa_cnt = 0;
    oi = (struct ospf6_interface *)(thread);
    //assert(oi);
	if(!oi)
	{
		zlog_err("%s[%d]: invalid argv!\n", __FUNCTION__, __LINE__);
		return 0;
	}
    oi->thread_send_lsack = 0;

    if (oi->state <= OSPF6_INTERFACE_WAITING)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_LSACK, SEND))
            zlog_debug(OSPF6_DBG_MESSAGE_SEND, "Quit to send LSAck to interface %s state %s",
                       oi->interface->name, ospf6_interface_state_str[oi->state]);

        return 0;
    }

    /* if we have nothing to send, return */
    if (oi->lsack_list->count == 0)
    {
        return 0;
    }

    memset(sendbuf, 0, iobuflen);
    oh = (struct ospf6_header *) sendbuf;
    p = (u_char *)((caddr_t) oh + sizeof(struct ospf6_header));

    for (lsa = ospf6_lsdb_head(oi->lsack_list); lsa;
            lsa = ospf6_lsdb_next(lsa))
    {
        /* MTU check */
        if (p - sendbuf + sizeof(struct ospf6_lsa_header) > ospf6_packet_max(oi))
        {
            /* if we run out of packet size/space here,
               better to try again soon. */
            OSPF6_TIMER_OFF(oi->thread_send_lsack);
            /*oi->thread_send_lsack =
                thread_add_event(master_ospf6, ospf6_lsack_send_interface, oi, 0);*/
			oi->thread_send_lsack =
				high_pre_timer_add ((char *)"ospf6_lsack_timer", LIB_TIMER_TYPE_NOLOOP,\
					ospf6_lsack_send_interface, (void *)oi, 10);
            ospf6_lsdb_lsa_unlock(lsa);
            break;
        }

        ospf6_lsa_age_update_to_send(lsa, oi->transdelay);
        memcpy(p, lsa->header, sizeof(struct ospf6_lsa_header));
        p += sizeof(struct ospf6_lsa_header);
        assert(lsa->lock == 2);
        ospf6_lsdb_remove(lsa, oi->lsack_list);
        lsa_cnt++;
    }

    if (lsa_cnt)
    {
        oh->type = OSPF6_MESSAGE_TYPE_LSACK;
        oh->length = htons(p - sendbuf);
		om6->ospf6_statics.s_lsa ++;

        if ((oi->state == OSPF6_INTERFACE_POINTTOPOINT) ||
                (oi->state == OSPF6_INTERFACE_DR) ||
                (oi->state == OSPF6_INTERFACE_BDR))
        {
            ospf6_send(oi->linklocal_addr, &allspfrouters6, oi, oh);
        }
        else
        {
            ospf6_send(oi->linklocal_addr, &alldrouters6, oi, oh);
        }
    }

    if (oi->thread_send_lsack == 0 && oi->lsack_list->count > 0)
    {
        /*oi->thread_send_lsack =
            thread_add_event(master_ospf6, ospf6_lsack_send_interface, oi, 0);*/
		oi->thread_send_lsack =
					high_pre_timer_add ((char *)"ospf6_lsack_timer", LIB_TIMER_TYPE_NOLOOP,\
						ospf6_lsack_send_interface, (void *)oi, 10);
    }

    return 0;
}

#if 0
/* Commands */
DEFUN(debug_ospf6_message,
      debug_ospf6_message_cmd,
      "debug ospf6 packet (unknown|hello|dbdesc|lsreq|lsupdate|lsack|all)",
      DEBUG_STR
      OSPF6_STR
      "Debug OSPF6 packet\n"
      "Debug Unknown message\n"
      "Debug Hello message\n"
      "Debug Database Description message\n"
      "Debug Link State Request message\n"
      "Debug Link State Update message\n"
      "Debug Link State Acknowledgement message\n"
      "Debug All message\n"
     )
{
    unsigned char level = 0;
    int type = 0;
    int i;
    assert(argc > 0);

    /* check type */
    if (! strncmp(argv[0], "u", 1))
    {
        type = OSPF6_MESSAGE_TYPE_UNKNOWN;
    }
    else if (! strncmp(argv[0], "h", 1))
    {
        type = OSPF6_MESSAGE_TYPE_HELLO;
    }
    else if (! strncmp(argv[0], "d", 1))
    {
        type = OSPF6_MESSAGE_TYPE_DBDESC;
    }
    else if (! strncmp(argv[0], "lsr", 3))
    {
        type = OSPF6_MESSAGE_TYPE_LSREQ;
    }
    else if (! strncmp(argv[0], "lsu", 3))
    {
        type = OSPF6_MESSAGE_TYPE_LSUPDATE;
    }
    else if (! strncmp(argv[0], "lsa", 3))
    {
        type = OSPF6_MESSAGE_TYPE_LSACK;
    }
    else if (! strncmp(argv[0], "a", 1))
    {
        type = OSPF6_MESSAGE_TYPE_ALL;
    }

    if (argc == 1)
    {
        level = OSPF6_DEBUG_MESSAGE_SEND | OSPF6_DEBUG_MESSAGE_RECV;
    }
    else if (! strncmp(argv[1], "s", 1))
    {
        level = OSPF6_DEBUG_MESSAGE_SEND;
    }
    else if (! strncmp(argv[1], "r", 1))
    {
        level = OSPF6_DEBUG_MESSAGE_RECV;
    }

    if (type == OSPF6_MESSAGE_TYPE_ALL)
    {
        for (i = 0; i < 6; i++)
        {
            OSPF6_DEBUG_MESSAGE_ON(i, level);
        }
    }
    else
    {
        OSPF6_DEBUG_MESSAGE_ON(type, level);
    }

    return CMD_SUCCESS;
}

ALIAS(debug_ospf6_message,
      debug_ospf6_message_sendrecv_cmd,
      "debug ospf6 packet (unknown|hello|dbdesc|lsreq|lsupdate|lsack|all) (send|recv)",
      DEBUG_STR
      OSPF6_STR
      "Debug OSPF6 packet\n"
      "Debug Unknown message\n"
      "Debug Hello message\n"
      "Debug Database Description message\n"
      "Debug Link State Request message\n"
      "Debug Link State Update message\n"
      "Debug Link State Acknowledgement message\n"
      "Debug All message\n"
      "Debug only sending message\n"
      "Debug only receiving message\n"
     )


DEFUN(no_debug_ospf6_message,
      no_debug_ospf6_message_cmd,
      "no debug ospf6 packet (unknown|hello|dbdesc|lsreq|lsupdate|lsack|all)",
      NO_STR
      DEBUG_STR
      OSPF6_STR
      "Debug OSPF6 packet\n"
      "Debug Unknown message\n"
      "Debug Hello message\n"
      "Debug Database Description message\n"
      "Debug Link State Request message\n"
      "Debug Link State Update message\n"
      "Debug Link State Acknowledgement message\n"
      "Debug All message\n"
     )
{
    unsigned char level = 0;
    int type = 0;
    int i;
    assert(argc > 0);

    /* check type */
    if (! strncmp(argv[0], "u", 1))
    {
        type = OSPF6_MESSAGE_TYPE_UNKNOWN;
    }
    else if (! strncmp(argv[0], "h", 1))
    {
        type = OSPF6_MESSAGE_TYPE_HELLO;
    }
    else if (! strncmp(argv[0], "d", 1))
    {
        type = OSPF6_MESSAGE_TYPE_DBDESC;
    }
    else if (! strncmp(argv[0], "lsr", 3))
    {
        type = OSPF6_MESSAGE_TYPE_LSREQ;
    }
    else if (! strncmp(argv[0], "lsu", 3))
    {
        type = OSPF6_MESSAGE_TYPE_LSUPDATE;
    }
    else if (! strncmp(argv[0], "lsa", 3))
    {
        type = OSPF6_MESSAGE_TYPE_LSACK;
    }
    else if (! strncmp(argv[0], "a", 1))
    {
        type = OSPF6_MESSAGE_TYPE_ALL;
    }

    if (argc == 1)
    {
        level = OSPF6_DEBUG_MESSAGE_SEND | OSPF6_DEBUG_MESSAGE_RECV;
    }
    else if (! strncmp(argv[1], "s", 1))
    {
        level = OSPF6_DEBUG_MESSAGE_SEND;
    }
    else if (! strncmp(argv[1], "r", 1))
    {
        level = OSPF6_DEBUG_MESSAGE_RECV;
    }

    if (type == OSPF6_MESSAGE_TYPE_ALL)
    {
        for (i = 0; i < 6; i++)
        {
            OSPF6_DEBUG_MESSAGE_OFF(i, level);
        }
    }
    else
    {
        OSPF6_DEBUG_MESSAGE_OFF(type, level);
    }

    return CMD_SUCCESS;
}

ALIAS(no_debug_ospf6_message,
      no_debug_ospf6_message_sendrecv_cmd,
      "no debug ospf6 packet "
      "(unknown|hello|dbdesc|lsreq|lsupdate|lsack|all) (send|recv)",
      NO_STR
      DEBUG_STR
      OSPF6_STR
      "Debug OSPF6 packet\n"
      "Debug Unknown message\n"
      "Debug Hello message\n"
      "Debug Database Description message\n"
      "Debug Link State Request message\n"
      "Debug Link State Update message\n"
      "Debug Link State Acknowledgement message\n"
      "Debug All message\n"
      "Debug only sending message\n"
      "Debug only receiving message\n"
     )
#endif
int
config_write_ospf6_debug_message(struct vty *vty)
{
    const char *type_str[] = {"unknown", "hello", "dbdesc",
                              "lsreq", "lsupdate", "lsack"
                             };
    unsigned char s = 0, r = 0;
    int i;

    for (i = 0; i < 6; i++)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(i, SEND))
        {
            s |= 1 << i;
        }

        if (IS_OSPF6_DEBUG_MESSAGE(i, RECV))
        {
            r |= 1 << i;
        }
    }

    if (s == 0x3f && r == 0x3f)
    {
        vty_out(vty, "debug ospf6 packet all%s", VNL);
        return 0;
    }

    if (s == 0x3f && r == 0)
    {
        vty_out(vty, "debug ospf6 packet all send%s", VNL);
        return 0;
    }
    else if (s == 0 && r == 0x3f)
    {
        vty_out(vty, "debug ospf6 packet all recv%s", VNL);
        return 0;
    }

    /* Unknown message is logged by default */
    if (! IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, SEND) &&
            ! IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
    {
        vty_out(vty, "no debug ospf6 packet unknown%s", VNL);
    }
    else if (! IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, SEND))
    {
        vty_out(vty, "no debug ospf6 packet unknown send%s", VNL);
    }
    else if (! IS_OSPF6_DEBUG_MESSAGE(OSPF6_MESSAGE_TYPE_UNKNOWN, RECV))
    {
        vty_out(vty, "no debug ospf6 packet unknown recv%s", VNL);
    }

    for (i = 1; i < 6; i++)
    {
        if (IS_OSPF6_DEBUG_MESSAGE(i, SEND) &&
                IS_OSPF6_DEBUG_MESSAGE(i, RECV))
        {
            vty_out(vty, "debug ospf6 packet %s%s", type_str[i], VNL);
        }
        else if (IS_OSPF6_DEBUG_MESSAGE(i, SEND))
            vty_out(vty, "debug ospf6 packet %s send%s", type_str[i],
                    VNL);
        else if (IS_OSPF6_DEBUG_MESSAGE(i, RECV))
            vty_out(vty, "debug ospf6 packet %s recv%s", type_str[i],
                    VNL);
    }

    return 0;
}

#if 0
void
install_element_ospf6_debug_message(void)
{
    install_element(CONFIG_NODE, &debug_ospf6_message_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_debug_ospf6_message_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &debug_ospf6_message_sendrecv_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_debug_ospf6_message_sendrecv_cmd, CMD_SYNC);
}
#endif


